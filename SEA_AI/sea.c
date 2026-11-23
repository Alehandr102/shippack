#include "storm-engine\sea_ai\script_defines.h"
#include "storm-engine\sea_ai\sea_people.h"

#include "sea_ai\AIGroup.c"
#include "sea_ai\AIShip.c"
#include "sea_ai\AIFort.c"
#include "sea_ai\AISea.c"
#include "sea_ai\AICameras.c"
#include "sea_ai\AIAbordage.c"
#include "sea_ai\Boarding.c"
#include "sea_ai\ShipTowRuntime.c"
#include "sea_ai\ShipTowOrders.c"
#include "sea_ai\ShipDestruction.c"
#include "sea_ai\Cabin.c" //boal
#include "sea_ai\AIFantom.c"
#include "sea_ai\AICannon.c"
#include "sea_ai\CannonHeatHaze.c"
#include "sea_ai\AIBalls.c"
#include "sea_ai\AIIsland.c"
#include "sea_ai\AISeaGoods.c"
#include "sea_ai\AITasks\AITasks.c"

#include "sea_ai\ShipBortFire.c"
#include "sea_ai\ShipDead.c"
#include "sea_ai\ShipWalk.c"

#include "sea_ai\CoastFoam.c"

#include "sea_ai\Telescope.c"

#include "battle_interface\BattleInterface.c"


#event_handler("Sea_FirstInit", "Sea_FirstInit");
#event_handler("SeaLoad_GetPointer", "SeaLoad_GetPointer");

#define PLAYER_GROUP	"OurGroup"
#define SHIPS_PC_SQUAD_DOUBLE 3
#define SHIPS_NUM_DOUBLE 3

int	sCurrentSeaExecute = EXECUTE;
int	sCurrentSeaRealize = REALIZE;

int		iAITemplatesNum;
bool	bSeaActive;
bool	bSeaLoaded = false;
bool 	bSkipSeaLogin = false;
bool	bIslandLoaded = false;
bool	bSeaReloadStarted = false;
bool	bNotEnoughBalls;
bool	bStorm, bTornado;
bool	bSeaQuestGroupHere = false;
bool 	bSeaCanGenerateShipSituation = true;

int		iStormLockSeconds = 0;

object	Island, IslandReflModel, sLightModel, lighthouseLightModel;
object	Touch, AISea;
object	SeaFader;
object	Seafoam, BallSplash, SinkEffect, PeopleOnShip, Telescope, SeaOperator, Artifact;
object	SpriteFX; // LEO: спрайтовые эффекты (движковая сущность SPRITE_FX, конфиг sprite_fx.ini)
object	Sharks;
object	SeaLighter;
object  ShipTracks;

object	SeaLocatorShow;
object	LoginGroupsNow;
bool	bSeaShowLocators = true;
bool	bQuestDisableMapEnter = false;
bool	bFromCoast = false;
bool    bFortCheckFlagYet = false; //eddy. флаг на распознавание врага фортом

float	SeaMapLoadX = -1570.99;
float	SeaMapLoadZ = 950.812;
float	SeaMapLoadAY = 10.54;

float	fSeaExp = 0.0;
float	fSeaExpTimer = 0.0;

int	iSeaSectionLang = -1;

void fnDrawShootDistance()
{
	isDrawShootDistance = !isDrawShootDistance;
	SendMessage(&AISea, "ll", MSG_SHIP_SHOW_SHOTTING_DISTANCE, isDrawShootDistance);
}

void DeleteSeaEnvironment()
{
	SeaTowOrders_Suspend();
	SeaTow_Suspend();
	Ship_Walk_Delete();

	StopMusic();
	bSeaActive = false;
	bSeaLoaded = false;

	sCurrentSeaExecute = EXECUTE;
	sCurrentSeaRealize = REALIZE;

	pchar.Ship.Stopped = true;
	DeleteBattleInterface();

	DelEventHandler(SHIP_BORT_FIRE, "Ship_BortFire");
	DelEventHandler(BALL_FLY_UPDATE, "Ball_OnFlyUpdate");

	SendMessage(&AISea, "l", AI_MESSAGE_UNLOAD);

	DeleteSea();

	DeleteClass(&ShipTracks);

	DeleteClass(&Island);
	DeleteGrass(); // трава на остров
	DeleteClass(&IslandReflModel);
	DeleteClass(&Touch);
	DeleteClass(&Seafoam);
	DeleteClass(&BallSplash);
	DeleteClass(&SinkEffect);
	DeleteClass(&SpriteFX); // LEO: спрайтовые эффекты — удалять с морским окружением (иначе утечка/дубли при входе в море)
	DeleteClass(&SeaLocatorShow);
	DeleteClass(&SeaOperator);
	DeleteClass(&Telescope);
	DeleteClass(&Sharks);
	DeleteClass(&sLightModel);
	DeleteClass(&lighthouseLightModel);

	DeleteClass(&SeaLighter);

	if (IsEntity(&Artifact))
		DeleteClass(&Artifact);

	DeleteBallsEnvironment();
	DeleteCannonsEnvironment();
	DeleteSeaCamerasEnvironment();
	DeleteShipEnvironment();
	DeleteFortEnvironment();
	DeleteAbordageEnvironment();
	DeleteSeaGoodsEnvironment();

	DeleteWeatherEnvironment();

	DeleteCoastFoamEnvironment();

	DeleteAttribute(&AISea,"");

	LayerFreeze(SEA_EXECUTE, true);
	LayerFreeze(SEA_REALIZE, true);

	LayerFreeze(REALIZE, false);
	LayerFreeze(EXECUTE, false);

	DeleteClass(&AISea);

	DeleteAnimals();

	// delete masts fall modules
		DeleteEntitiesByType("mast");
		
	// delete hulls fall modules
	DeleteEntitiesByType("hull");	

 	// delete particle system

	// delete our group
		Group_DeleteGroup(PLAYER_GROUP);

	// delete fantom and dead groups
		Group_DeleteUnusedGroup();

	//
		LanguageCloseFile(iSeaSectionLang); iSeaSectionLang = -1;

	//
		Encounter_DeleteDeadQuestMapEncounters();

	// delete mast damage attributes for all ships
		DeleteShipMastDamage();
}

void CreateSeaEnvironment()
{
	if (IsEntity(&Sea)) { Trace("ERROR: CreateSeaEnvironment Sea Already Loaded!!!"); return; } //fix

	// LEO: Численные параметры применяются при каждом входе в море.
	CannonHeatHaze_ApplyConfig();

	sCurrentSeaExecute = SEA_EXECUTE;
	sCurrentSeaRealize = SEA_REALIZE;

	iSeaSectionLang = LanguageOpenFile("SeaSection.txt");

	CreateParticleEntity();

	Ship_Walk_Init();

	LayerFreeze(REALIZE, true);
	LayerFreeze(EXECUTE, true);
	LayerFreeze(SEA_REFLECTION, false);
	LayerFreeze(SEA_REFLECTION2, false);

	InterfaceStates.Buttons.Resume.enable = true;

	bSeaActive = true;

	LayerSetRealize(SEA_REALIZE);
	LayerSetExecute(SEA_EXECUTE);

	LayerFreeze(SEA_EXECUTE, false);
	LayerFreeze(SEA_REALIZE, false);

	CreateSea(SEA_EXECUTE, SEA_REALIZE);			ReloadProgressUpdate();
	CreateWeather(SEA_EXECUTE, SEA_REALIZE);		ReloadProgressUpdate();

	CreateEntity(&AISea, "sea_ai");					ReloadProgressUpdate();
	LayerAddObject(SEA_EXECUTE, &AISea, 1);
	LayerAddObject(SEA_REALIZE, &AISea, -1);
	AISea.BoardingVisuals.CutTapsLabel = GetConvertStr("BoardingCutTaps", "SeaBoarding.txt");
	AISea.BoardingVisuals.CutRopesLabel = GetConvertStr("BoardingCutRopes", "SeaBoarding.txt");
	AISea.BoardingVisuals.CutTapLabel = GetConvertStr("BoardingCutTapPrompt", "SeaBoarding.txt");
	AISea.BoardingVisuals.CutY = 0.80;
	AISea.BoardingVisuals.CutFont = "interface_normal";
	AISea.BoardingVisuals.CutFontScale = 1.32;
	AISea.BoardingVisuals.CutChanceLowColor = argb(255, 255, 101, 90);
	AISea.BoardingVisuals.CutChanceMidColor = argb(255, 255, 213, 115);
	AISea.BoardingVisuals.CutChanceHighColor = argb(255, 128, 233, 144);
	AISea.BoardingVisuals.CutProgressTrack = argb(255, 89, 74, 54);
	AISea.BoardingVisuals.CutProgressFill = argb(255, 230, 193, 124);
	AISea.BoardingVisuals.RopeColor = argb(255, 189, 161, 113);
	AISea.BoardingVisuals.HookColor = argb(255, 208, 208, 208);
	// LEO: Общий минимум RGB для канатов буксира, абордажа и крюков; основной свет берётся из текущей погоды.
	AISea.RopeLighting.MinLight = 0.12;
	AISea.BoardingVisuals.FightPanelLift = 126.0; // LEO: Отступ центра таймера над кораблями при высоте экрана 1080.
	AISea.BoardingVisuals.FightRingRadius = 22.0;
	AISea.BoardingVisuals.FightRingWidth = 3.0;
	AISea.BoardingVisuals.FightRingBackground = argb(229, 28, 24, 20);
	AISea.BoardingVisuals.FightRingTrack = argb(255, 88, 70, 48);
	AISea.BoardingVisuals.FightRingFill = argb(255, 234, 198, 125);

	CreateEntity(&Touch, "touch");					ReloadProgressUpdate();
	LayerAddObject(SEA_EXECUTE, &Touch, 1);
	Touch.CollisionDepth = -10.0;

	CreateEntity(&BallSplash, "BallSplash");		ReloadProgressUpdate();
	LayerAddObject(SEA_EXECUTE, &BallSplash, -1);
	LayerAddObject(SEA_REALIZE, &BallSplash, 65535);

	CreateEntity(&SinkEffect, "SINKEFFECT");		ReloadProgressUpdate();
	LayerAddObject(SEA_EXECUTE, &SinkEffect, 65532);
	LayerAddObject(SEA_REALIZE, &SinkEffect, 65532);

	// LEO: спрайтовые эффекты (атлас-анимация на плейнах, конфиг RESOURCE\INI\sprite_fx.ini)
	CreateEntity(&SpriteFX, "SPRITE_FX");			ReloadProgressUpdate();
	LayerAddObject(SEA_EXECUTE, &SpriteFX, 200);
	LayerAddObject(SEA_REALIZE, &SpriteFX, 65535);	// высокий приоритет — поверх воды/кораблей

	CreateEntity(&ShipTracks, "ShipTracks");		ReloadProgressUpdate();
	LayerAddObject(SEA_EXECUTE, &ShipTracks, 100);
	LayerAddObject(SEA_REALIZE, &ShipTracks, 65531);

	CreateEntity(&SeaLocatorShow, "SeaLocatorShow"); ReloadProgressUpdate();
	LayerAddObject(SEA_REALIZE, &SeaLocatorShow, -1);

	CreateSeaAnimals();								ReloadProgressUpdate();

	// create all other environment
	CreateBallsEnvironment();						ReloadProgressUpdate();
	CreateCannonsEnvironment();						ReloadProgressUpdate();
	CreateSeaCamerasEnvironment();					ReloadProgressUpdate();
	CreateShipEnvironment();						ReloadProgressUpdate();
	CreateFortEnvironment();						ReloadProgressUpdate();
	CreateAbordageEnvironment();					ReloadProgressUpdate();
	CreateSeaGoodsEnvironment();					ReloadProgressUpdate();

	SetEventHandler("MSG_TELESCOPE_REQUEST", "Telescope_Request", 0);
	SetEventHandler(SHIP_BORT_FIRE, "Ship_BortFire", 0);

	bNotEnoughBalls = false;

	Sharks.execute = SEA_EXECUTE;
	Sharks.realize = SEA_REALIZE;
	Sharks.executeModels = 75;
	Sharks.realizeModels = 75;
	Sharks.executeParticles = 78;
	Sharks.realizeParticles = 100001;

	CreateEntity(&Sharks, "Sharks");				ReloadProgressUpdate();
	/* Задаем цвет дистанции выстрела. Альфа LOAD +0, +0x15, +0x25, Альфа UNLOAD не может быть больше 0x30 */
	SendMessage(&AISea, "ll", MSG_SHIP_SET_COLOR_LOAD_SHOW_SHOTTING_DISTANCE, argb(1,0,255,255));
	SendMessage(&AISea, "ll", MSG_SHIP_SET_COLOR_UNLOAD_SHOW_SHOTTING_DISTANCE, argb(37,128,00,00));
}
// boal -->
string Sea_FindNearColony()
{
	aref aLocators;
	int iNum =  GetAttributesNum(arIslandReload);
	string sColony = "none";

	for(int i = 0; i < iNum; i++)
	{
		aLocators = GetAttributeN(arIslandReload, i);
		if(aLocators.name == sIslandLocator)
		{
			sColony = aLocators.go;
            if(CheckAttribute(&locations[FindLocation(sColony)], "fastreload"))
            {
                sColony = locations[FindLocation(sColony)].fastreload;
                break;
            }
		}
	}
	return sColony;
}
// boal <--
int PortSborForCharacter(ref character)
{
	return (sti(GetCharacterShipHP(character)) + sti(GetCrewQuantity(character)) * 10 + sti(GetCargoMaxSpace(character))) / 5;
}

// LEO: продолжение захода на сушу после автосейва - ПЕРЕЗАХОД в Sea_LandLoad (её локальные расчёты
//   портового сбора/болезней разрезать нельзя; повторный прогон безопасен, побочек там нет).
//   Второй раз сохранение не сработает - одноразовый гард AutoSaveJustDone в AutoSave_Start.
void Sea_LandLoad_AfterAutoSave()
{
	DelEventHandler("evntSave", "Sea_LandLoad_AfterAutoSave");
	SetEventHandler("frame", "Sea_LandLoad_Frame", 0); // кадр на дозапись слота (как Continue_Sea_AbordageLoadPre)
}

void Sea_LandLoad_Frame()
{
	DelEventHandler("frame", "Sea_LandLoad_Frame");
	if (!bSeaActive) return;
	Sea_LandLoad();
}

void Sea_LandLoad()
{
	if (CheckAttribute(pchar, "LockShoreReload"))
	{
		Log_Info(pchar.LockShoreReload);
		PlaySound("interface\knock.wav");
		return;
	}
	// LEO: КРАШФИКС - переход уже идёт, см. Sea_MapLoad. Второй клик по высадке запускал Reload повторно.
	if (bSeaReloadStarted) return;

	string sColony = Sea_FindNearColony(); // boal
	int iColony = FindColony(sColony);
	string sCityNation;
	int iCityNation;

	bool payPortSbor = bPortPermission && (iColony != -1);
	bool sborCancelledByPatent = false;
	if (payPortSbor)
	{
		payPortSbor = payPortSbor && (pchar.questTemp.sbormoney != sColony);
		sCityNation = Colonies[iColony].nation;
		iCityNation = sti(sCityNation);
		payPortSbor = payPortSbor && (iCityNation != PIRATE) && (sCityNation != "none");
		string city_id = Colonies[iColony].id;
		payPortSbor = payPortSbor && (city_id != "LostShipsCity");

		if (isMainCharacterPatented())
		{
			sborCancelledByPatent = iCityNation == GetPatentNation();
		}
	}

	int sbormoney = 0;
	if (payPortSbor && !sborCancelledByPatent)
	{
		sbormoney = PortSborForCharacter(pchar);

		int i;
		for (i = 0; i < COMPANION_MAX; i++)
		{
			int companion_index = GetCompanionIndex(pchar, i);
			if (companion_index >= 0)
			{
				ref companion = GetCharacter(companion_index);
				sbormoney = sbormoney + PortSborForCharacter(companion);
			}
		}

		if (sti(pchar.money) < sbormoney)
		{
			Log_Info(GetConvertStrWithReplace("Variable_sea_1", "Logs.txt", "#space#", " "));
			Log_Info(GetConvertStrWithReplace("Variable_sea_2", "Logs.txt", "#space#", " ")+ (sbormoney - sti(pchar.money)) +GetConvertStrWithReplace("Variable_sea_3", "Logs.txt", "#space#", " "));
			return;
		}
	}

	if (iColony != -1)
	{
		if (CheckAttribute(pchar, "ship.crew.disease"))  // to_do
		{
			if (pchar.ship.crew.disease == "1")
			{
				if (Colonies[iColony].disease != "1" && sti(Colonies[iColony].nation) != PIRATE)
				{
					LaunchDiseaseAlert(DISEASE_ON_SHIP);
					return;
				}
			}
		}
		if(CheckAttribute(&Colonies[iColony], "disease.time"))
		{
			if(sti(Colonies[iColony].disease.time > 0))
			{
				LaunchDiseaseAlert(DISEASE_ON_COLONY);  // to_do
				return;
			}
		}
	}

	pchar.CheckEnemyCompanionType = "Sea_LandLoad"; // откуда вход
	if (!CheckEnemyCompanionDistance2GoAway(true)) return; // && !bBettaTestMode  табличка выхода из боя

	// LEO: автосейв ПЕРЕД заходом на сушу (см. пояснение в Sea_MapLoad - переход ждёт evntSave)
	if (AutoSave_Start("Land"))
	{
		SetEventHandler("evntSave", "Sea_LandLoad_AfterAutoSave", 0);
		return;
	}
	bSeaReloadStarted = true;
	PauseAllSounds();
	ResetSound(); // new

	if (!bSeaActive) return;
	if (bCanEnterToLand)
	{
		//#20190717-01
		resetGroupRel();
		LayerFreeze(REALIZE, false);
		LayerFreeze(EXECUTE, false);
		Reload(arIslandReload, sIslandLocator, sIslandID);
		ReleaseMapEncounters();
		EmptyAllFantomShips(); // boal
		DeleteAttribute(pchar, "CheckStateOk"); // проверка протектором
		Group_FreeAllDead();
	}
	
	PerkLoad(true);
	
	if (sborCancelledByPatent)
	{
		Log_Info(GetConvertStrWithReplace("Variable_sea_4", "Logs.txt", "#space#", " "));
	}
	else
	{
		if (payPortSbor)
		{
			bool relationEnemy = GetNationRelation(iCityNation, sti(pchar.nation)) == RELATION_ENEMY; // доработка портового сбора.
			if (relationEnemy)
			{
				if (!CheckAttribute(pchar, "Arrive.EnemyPort")) pchar.Arrive.EnemyPort = true; //При причаливании во враждебный город дается аттрибут на вражду.
				return;
			}

			Log_Info(GetConvertStrWithReplace("Variable_sea_5", "Logs.txt", "#space#", " ")+ sbormoney +GetConvertStrWithReplace("Variable_sea_6", "Logs.txt", "#space#", " "));

			IncAndUnlock("taxes", 1);

			pchar.money = sti(pchar.money) - sbormoney;

			if (GetDaysContinueNationLicence(iCityNation) < 2) // доработка портового сбора. Если причалил успешно, то условный портовик даёт пропуск на два дня.
			{
				GiveUniLicence(2); // универсальная лицензия ГВИК (добавит время, если уже есть; TakeNationLicence не зовём - он снял бы носитель)
				Log_Info(GetConvertStrWithReplace("Variable_sea_7", "Logs.txt", "#space#", " "));
			}

			// Не дадим брать сбор несколько раз подряд
			// До тех пор, пока не причалим в другой город или не выйдем на глобалку
			pchar.questTemp.sbormoney = sColony;
		}
	}
}

void Sea_MapStartFade()
{
	DelEventHandler("FaderEvent_StartFade", "Sea_MapStartFade");
	DeleteSeaEnvironment();
	EmptyAllFantomCharacter(); // трем НПС
	EmptyAllFantomShips();    // трем корабли
	wdmEmptyAllDeadQuestEncounter(); // трем случайки
	pchar.location = "";
	Siege_DailyUpdate();//homo осады 05/11/06
	wdmUpdateAllEncounterLivetime(); // homo карта 25/03/07
	Flag_Rerise(); // переподнять флаг при выходе на карту, прменить отношения нации в столбец ГГ
}

void Land_MapStartFade()
{
	DelEventHandler("FaderEvent_StartFade", "Land_MapStartFade");
	string deckID = pchar.location;

	ref loc = &locations[FindLocation(deckID)];

	UnloadLocation(loc);
	EmptyAllFantomShips();
	wdmEmptyAllDeadQuestEncounter(); // трем случайки
}

void Sea_MapEndFade()
{
	DelEventHandler("FaderEvent_EndFade", "Sea_MapEndFade");
	Partition_SetValue("after");// Дележ добычи уход на карту
	wdmCreateMap(SeaMapLoadX, SeaMapLoadZ, SeaMapLoadAY);
}

void Sea_MapLoadXZ_AY(float x, float z, float ay)
{
	Sea_MapLoad();

	SeaMapLoadX = x;
	SeaMapLoadZ = z;
	SeaMapLoadAY = ay;
}

// LEO: продолжение выхода на глобалку после автосейва - ПЕРЕЗАХОД в Sea_MapLoad (повторный прогон её
//   проверок безопасен). Второй раз сохранение не сработает - гард AutoSaveJustDone в AutoSave_Start.
void Sea_MapLoad_AfterAutoSave()
{
	DelEventHandler("evntSave", "Sea_MapLoad_AfterAutoSave");
	SetEventHandler("frame", "Sea_MapLoad_Frame", 0); // кадр на дозапись слота (как Continue_Sea_AbordageLoadPre)
}

void Sea_MapLoad_Frame()
{
	DelEventHandler("frame", "Sea_MapLoad_Frame");
	if (!bSeaActive) return;
	Sea_MapLoad();
}

void Sea_MapLoad()
{
	if (SeaTow_HasLinks()) return;
	if (Boarding_Busy(pchar)) return;
	if (CheckAttribute(pchar, "LockMapReload"))
	{
		Log_Info(pchar.LockMapReload);
		PlaySound("interface\knock.wav");
		return;
	}
	// LEO: КРАШФИКС - «Бросить своих» планирует Sea_MapLoad отложенно, и клик по «на глобалку» в этом окне
	//   стартовал переход ДВАЖДЫ: вторая карта, движок шёл по кораблям снесённой первой -> AV.
	if (bSeaReloadStarted) return;
	if (IsEntity(worldMap)) return; // мы уже на глобалке
	// boal 201004 проверка на перегруз и мин команду -->
	ref  rPlayer = GetMainCharacter();
    int  i, cn;
    ref  chref;
    bool ok = true;
    for (i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(rPlayer,i);
		if( cn>=0 )
		{
			chref = GetCharacter(cn);
			if (!GetRemovable(chref)) continue;

            if (GetCargoLoad(chref) > GetCargoMaxSpace(chref))
            {
                ok = false;
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_sea_8", "Logs.txt", "#space#", " ") +  chref.Ship.Name + GetConvertStrWithReplace("Variable_sea_9", "Logs.txt", "#space#", " "));
            }
            if (MOD_SKILL_ENEMY_RATE > 1) // халява и юнга - послабление
    		{
	            if (i > 0 && GetMinCrewQuantity(chref) > GetCrewQuantity(chref))
	            {
	                ok = false;
	                Log_SetStringToLog(GetConvertStrWithReplace("Variable_sea_10", "Logs.txt", "#space#", " ") +  chref.Ship.Name + GetConvertStrWithReplace("Variable_sea_11", "Logs.txt", "#space#", " "));
	            }
			}

            if (GetMaxCrewQuantity(chref) < GetCrewQuantity(chref))
            {
                ok = false;
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_sea_12", "Logs.txt", "#space#", " ") +  chref.Ship.Name + GetConvertStrWithReplace("Variable_sea_13", "Logs.txt", "#space#", " "));
            }
        }
    }
    if (!ok)
    {
        Log_Info(GetConvertStrWithReplace("Variable_sea_14", "Logs.txt", "#space#", " "));
        PlaySound("knock");
        return;
    }
    pchar.CheckEnemyCompanionType = "Sea_MapLoad"; // откуда вход
    if (!CheckEnemyCompanionDistance2GoAway(true)) return; // && !bBettaTestMode  табличка выхода из боя
	// LEO: автосейв ПЕРЕД выходом на глобалку. Сохранение асинхронное (кадр на скриншот), а ниже создаётся
	//   фейдер, при котором сейв уже не пишется - поэтому переход откладываем до evntSave (как абордаж).
	if (AutoSave_Start("Map"))
	{
		SetEventHandler("evntSave", "Sea_MapLoad_AfterAutoSave", 0);
		return;
	}
	LAi_SetAlcoholNormal(pchar);
	LAi_grp_alarmactive = false;
    LAi_grp_playeralarm = 0.0;
    alarmed = 0;
    //#20170530-01
    seaAlarmed = false;
    //#20190717-01
    resetGroupRel();
	bSeaReloadStarted = true;
	PauseAllSounds();

	ResetSound(); // new

	SetEventHandler("FaderEvent_StartFade", "Sea_MapStartFade", 0);
	PGG_DailyUpdateSeaReload();
	SetEventHandler("FaderEvent_EndFade", "Sea_MapEndFade", 0);

	CreateEntity(&SeaFader, "fader");
	SendMessage(&SeaFader, "lfl", FADER_OUT, SEAMAP_FADE_OUT, true); // Gregg: дефайн морской карты (было 0.7)
	SendMessage(&SeaFader, "l", FADER_STARTFRAME);
	pchar.loadscreen = "loading\jonny_load\sea\sea_"+rand(3)+".dds";
	SendMessage(&SeaFader, "ls", FADER_PICTURE0, pchar.loadscreen);

	bSkipSeaLogin = true;

	SeaMapLoadX = stf(pchar.Ship.Pos.x);
	SeaMapLoadZ = stf(pchar.Ship.Pos.z);
	SeaMapLoadAY = stf(pchar.Ship.Ang.y);
	PerkLoad(true);
}

// нигде не пользуетя, может глючить для абордажа
void Land_MapLoad()
{
	if (SeaTow_HasLinks()) return;
	bSeaReloadStarted = true;
	PauseAllSounds();

	ResetSound(); // new

	SetEventHandler("FaderEvent_StartFade", "Land_MapStartFade", 0);
	SetEventHandler("FaderEvent_EndFade", "Sea_MapEndFade", 0);

	CreateEntity(&SeaFader, "fader");
	SendMessage(&SeaFader, "lfl", FADER_OUT, SEAMAP_FADE_OUT, true); // Gregg: дефайн морской карты (было 0.7, мёртвый Land_MapLoad)
	SendMessage(&SeaFader, "l", FADER_STARTFRAME);
	pchar.loadscreen = "loading\jonny_load\sea\sea_"+rand(3)+".dds";
	SendMessage(&SeaFader, "ls", FADER_PICTURE0, pchar.loadscreen);

	bSkipSeaLogin = true;

	SeaMapLoadX = stf(pchar.Ship.Pos.x);
	SeaMapLoadZ = stf(pchar.Ship.Pos.z);
	SeaMapLoadAY = stf(pchar.Ship.Ang.y);
}

string	sTaskList[2];

void Sea_FreeTaskList()
{
	ref rMassive;
	makeref(rMassive, sTaskList);
	SetArraySize(rMassive, 2);
}

void Sea_AddGroup2TaskList(string sGroupID)
{
	ref rMassive;
	makeref(rMassive, sTaskList);
	int iSize = GetArraySize(rMassive);
	SetArraySize(rMassive, iSize + 1);
	sTaskList[iSize-2] = sGroupID;
}

void Sea_LoginGroupNow(string sGroupID)
{
	LoginGroupsNow.QuestGroups = "";
	aref arGroups; makearef(arGroups, LoginGroupsNow.QuestGroups);
	string sID = "n" + GetAttributesNum(arGroups);
	arGroups.(sID) = sGroupID;
}


// LEO: вход в море — минимальные дистанции спавна, чтобы не кидало ГГ в центр эскадры (тунабл)
// LEO: ДИНАМИЧЕСКАЯ дистанция спавна — эскадра С ФОРМАЦИЕЙ (военная >=2) ставится ДАЛЬШЕ (виден подход строем),
// мелочь/пираты/одиночки — БЛИЖЕ (не утомлять на ранней игре). Все тунабл.
#define SEA_ENC_PDIST_FORM   700.0  // формация: дист. эскадры до ГГ
#define SEA_ENC_GDIST_FORM   600.0  // формация: дист. между эскадрами
#define SEA_ENC_PDIST_PLAIN  400.0  // без формации: дист. до ГГ (ближе)
#define SEA_ENC_GDIST_PLAIN  300.0  // без формации: дист. между эскадрами
#define SEA_ENC_FACE_FWD     632.0  // AI-vs-AI: центр линии противостояния впереди ГГ (632²+300²≈700²)
#define SEA_ENC_FACE_HALF    300.0  // AI-vs-AI: полудистанция между фракциями (600/2)
#define SEA_ENC_MAX          64     // потолок числа отслеживаемых эскадр

// LEO: угол курса ay из компонент направления (sin=sx, cos=cz) — аналог atan2(sx, cz); в скрипте нет штатного.
float Sea_AY_FromDir(float sx, float cz)
{
	if (cz > 0.0)  return atan(sx / cz);
	if (cz < 0.0)
	{
		if (sx >= 0.0) return atan(sx / cz) + PI;
		return atan(sx / cz) - PI;
	}
	if (sx > 0.0)  return PI * 0.5;
	if (sx < 0.0)  return -PI * 0.5;
	return 0.0;
}

#define SEA_BOARD_DELAY 30 // LEO: сек ДАЛЬНЕГО боя после входа до разрешения абордажа (виден фокус-огонь; тунабл)
void SeaLogin(ref Login)
{
	int		i, j, k, iShipType;
	float	x, y, z, ay;
	ref		rCharacter, rGroup, rEncounter;
	aref	rRawGroup;
	aref	arQCGroups;
	string	sGName;
	int		iNumQCGroups;
	ref 	rFantom;
	bSeaLoaded = false;
    //#20200408-01
    bAbordageStarted = false;
	Sea.AbordageMode = false;

	int iRDTSC = RDTSC_B();

	// clear load groups now object
	DeleteAttribute(&LoginGroupsNow, "");

	iStormLockSeconds = 0;
	iNumFantoms = 0;
	bSkipSeaLogin = false;
	bSeaReloadStarted = false;
	bSeaQuestGroupHere = false;
	bIslandLoaded = false;

	fSeaExp = 0.0;
	fSeaExpTimer = 0.0;

	// LEO: момент, ПОСЛЕ которого ИИ разрешён абордаж (сперва дальний бой). Гейт читается в Ship_UP_TargetTactics.
	pchar.SeaBoardEnableTime = GetSeaTime() + SEA_BOARD_DELAY;

	Sea_FreeTaskList();

	Encounter_DeleteDeadQuestMapEncounters();

	// weather parameters
	WeatherParams.Tornado = false;
	WeatherParams.Storm = false;
	if (CheckAttribute(&Login,"Storm")) { WeatherParams.Storm = Login.Storm; }
	if (CheckAttribute(&Login,"Tornado")) { WeatherParams.Tornado = Login.Tornado; }
	bStorm = sti(WeatherParams.Storm);
	bTornado = sti(WeatherParams.Tornado);
	if (bStorm)
	{
		iStormLockSeconds = 60;
	}

	// Island
	int iIslandIndex = FindIsland(Login.Island);
	string sIslandID = "";
	if (iIslandIndex != -1) { sIslandID = Islands[iIslandIndex].id; }

	// main character
	pchar.Ship.Stopped = false;
	pchar.Ship.POS.Mode = SHIP_SAIL;
	pchar.location = sIslandID;

	// clear old fantom relations in our character
	if (CheckAttribute(pchar, "Relation"))
	{
		aref	arRelations; makearef(arRelations, pchar.Relation);
		int		iNumRelations = GetAttributesNum(arRelations);
		for (i=0; i<iNumRelations; i++)
		{
			aref arRelation = GetAttributeN(arRelations, i);
			string sRName = GetAttributeName(arRelation);
			if (sti(sRName) >= FANTOM_CHARACTERS)
			{
				DeleteAttribute(arRelations, sRName);
				iNumRelations--;
				i--;
			}
		}
	}


	// Quest check
	Event(EVENT_SEA_LOGIN, "");
	if (bSkipSeaLogin) return;

	CreateEntity(&SeaFader, "fader");
	SendMessage(&SeaFader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true); // Gregg: единый IN-дефайн (было 0.5)
	SendMessage(&SeaFader, "ls", FADER_PICTURE0, pchar.loadscreen);

	// create all sea modules
	CreateSeaEnvironment();

	// delete our group
	Group_DeleteGroup(PLAYER_GROUP);

	// set commander to group
	Group_SetGroupCommander(PLAYER_GROUP, Characters[nMainCharacterIndex].id);
	Group_SetXZ_AY(PLAYER_GROUP, stf(Login.PlayerGroup.x), stf(Login.PlayerGroup.z), stf(Login.PlayerGroup.ay) );
	NullCharacter.Login.PlayerGroup.x  = Login.PlayerGroup.x;  // 1.2.3 попытка записать коорд ГГ для размещения Group_SetPursuitGroup в кильватерную линию
	NullCharacter.Login.PlayerGroup.z  = Login.PlayerGroup.z;
	NullCharacter.Login.PlayerGroup.ay = Login.PlayerGroup.ay;
    pchar.Ship.Pos.x = stf(Login.PlayerGroup.x);
    pchar.Ship.Pos.z = stf(Login.PlayerGroup.z);
	Sea.MaxSeaHeight = 200;

	ReloadProgressUpdate();

	// login island if exist
	if(sIslandID == "StLucia" && PChar.MysteriousIsland.StLuciaHide == true)
	{
		sIslandID = "";
	}
	
	Sea_LoadIsland(sIslandID);

	AISea.Island = sIslandID;

	// clear some of group attributes
	for (i=0; i<MAX_SHIP_GROUPS; i++)
	{
		rGroup = Group_GetGroupByIndex(i);
		if (CheckAttribute(rGroup, "AlreadyLoaded"))
		{
			DeleteAttribute(rGroup, "AlreadyLoaded");
		}
	}

	ReloadProgressUpdate();

	// from coast check (move / stop)
	bFromCoast = false;
	if (CheckAttribute(&Login, "FromCoast")) { bFromCoast = sti(Login.FromCoast); }

	// login main player and his friends
	int iCompanionIndex;

	int iPlayerCompanionsQ = GetCompanionQuantity(pchar); //Include PC + Companions

	//Lipsar 2 ряда кораблей
	pchar.SeaAI.Group.Name = PLAYER_GROUP;
	pchar.Ship.Type = Characters[nMainCharacterIndex].Ship.Type;
	pchar.Ship.Stopped = false;
	Partition_SetValue("before");// Дележ добычи
	Ship_Add2Sea(nMainCharacterIndex, bFromCoast, "");
	Group_AddCharacter(PLAYER_GROUP, Characters[nMainCharacterIndex].id);
	if(iPlayerCompanionsQ > 1)
	{
		int iCompFormSlot = 0; // LEO: плотный счётчик слотов (без дыр от -1); слот 0 = флагман
		int iFleetForm = Sea_GetFleetFormation(); // LEO: формация флота одна на весь строй — читаем 1 раз до цикла
		for (i=1; i<COMPANION_MAX; i++)
		{
			iCompanionIndex = GetCompanionIndex(&Characters[nMainCharacterIndex],i);
			if (iCompanionIndex == -1)
			{
				continue;
			}
			DeleteAttribute(&Characters[iCompanionIndex], "SeaAI"); // сброс для кэпов офов из пленных
			Characters[iCompanionIndex].SeaAI.Group.Name = PLAYER_GROUP;
			Ship_Add2Sea(iCompanionIndex, bFromCoast, "");

			// add companion to player group
			Group_AddCharacter(PLAYER_GROUP, Characters[iCompanionIndex].id);
			Ship_SetTaskDefend(PRIMARY_TASK, iCompanionIndex, nMainCharacterIndex);

			// LEO: крейсерский слот компаньона по ВЫБРАННОЙ формации (декартовы FormFwd/FormLat в
			// сеа-юнитах; движок GetFollowPoint/DEFEND держит эту позицию). Слот 0 = флагман, компаньоны 1..N.
			float fFormSp = 200.0 * Sea_FormSpacingFactor(&Characters[iCompanionIndex]); // интервал с учётом размера/класса
			Characters[iCompanionIndex].Ship.SeaAI.FormFwd = Sea_FormationOffset(iFleetForm, iCompFormSlot + 1, 0) * fFormSp;
			Characters[iCompanionIndex].Ship.SeaAI.FormLat = Sea_FormationOffset(iFleetForm, iCompFormSlot + 1, 1) * fFormSp;
			iCompFormSlot++;
		}
	}
	// set ship for sea camera
	SeaCameras_SetShipForSeaCamera(&pchar);

	// login encounters
	object oResult;
	int iFantomIndex;

	if (sIslandID != "")
	{
		GenerateIslandShips(sIslandID);
	}

	ReloadProgressUpdate();

	// login quest group if island exist
	ReloadProgressUpdate();
	if (sIslandID != "")
	{
		for (i=0; i<MAX_SHIP_GROUPS; i++)
		{
			rGroup = Group_GetGroupByIndex(i);
			if (!CheckAttribute(rGroup,"AlreadyLoaded"))
			{
				DeleteAttribute(rGroup,"AlreadyLoaded");
			}

			if (!CheckAttribute(rGroup, "id")) { continue; }
			if (!CheckAttribute(rGroup, "location")) { continue; }
			if (rGroup.location != sIslandID) { continue; }

			Sea_LoginGroup(rGroup.id);
		}
	}

	// login quest groups to sea
	if (CheckAttribute(&Login, "QuestGroups"))
	{
		arQCGroups; makearef(arQCGroups, Login.QuestGroups);
		iNumQCGroups = GetAttributesNum(arQCGroups);
		for (i=0; i<iNumQCGroups; i++)
		{
			Sea_LoginGroup(GetAttributeValue(GetAttributeN(arQCGroups, i)));
		}
	}

	ReloadProgressUpdate();

	// login quest groups to sea from LoginGroupsNow object
	if (CheckAttribute(&LoginGroupsNow, "QuestGroups"))
	{
		makearef(arQCGroups, LoginGroupsNow.QuestGroups);
		iNumQCGroups = GetAttributesNum(arQCGroups);
		for (i=0; i<iNumQCGroups; i++)
		{
			Sea_LoginGroup(GetAttributeValue(GetAttributeN(arQCGroups, i)));
		}
	}

	ReloadProgressUpdate();

	// login fantom groups
	aref arEncounters;
	makearef(arEncounters,Login.Encounters);
	int iNumGroups = GetAttributesNum(arEncounters);

	// === LEO: разнесение спавна энкаунтера — не кидать ГГ в центр эскадры (см. #define SEA_ENC_*) ===
	// Правка позиций делается ИНЛАЙН в цикле ниже, где x,z читаются и уходят в Group_SetXZ_AY
	// (фантомы -> @947; ALONE/квест -> rGroup.Pos -> Sea_LoginGroup@1332). Прежний пре-пасс писал
	// в Login.Encounters заранее, но write-back в aref не персистился — не работало.
	float fEncPx = stf(Login.PlayerGroup.x);   // позиция ГГ (точка входа)
	float fEncPz = stf(Login.PlayerGroup.z);
	float aEncSpreadX[SEA_ENC_MAX];            // позиции уже расставленных эскадр (для меж-эскадренной дистанции)
	float aEncSpreadZ[SEA_ENC_MAX];
	int   iEncSpreadN = 0;

	// ═══ LEO: ПРЕ-ПАСС геометрии спавна. Собираем нацию/размер/формацию каждой группы в ЛОКАЛЬНЫЕ массивы (читаются
	//     в цикле ниже по индексу i; write-back в Login.Encounters НЕ персистит — потому массивы) + детект AI-vs-AI
	//     (две взаимно-вражьи ФОРМАЦИОННЫЕ фракции) для треугольного противостояния носом-друг-на-друга. ═══
	int   aEncNat[SEA_ENC_MAX];
	int   aEncForm[SEA_ENC_MAX];   // 1 = будет с формацией (военная эскадра, не пират, 2..9)
	int   iPreN = iNumGroups;
	if (iPreN > SEA_ENC_MAX) iPreN = SEA_ENC_MAX;
	float fCentX = 0.0;
	float fCentZ = 0.0;
	int   iCentN = 0;
	int   ip;
	int   iEncT2;
	int   iNat2;
	int   iShp2;
	int   iAci;
	aref  rPre;   // GetAttributeN возвращает aref (как rRawGroup@640) — переуказывается через '='; ref нельзя (no DST)
	ref   rEnc;
	for (ip = 0; ip < iPreN; ip++)
	{
		aEncNat[ip] = -1;
		aEncForm[ip] = 0;
		rPre = GetAttributeN(arEncounters, ip);
		rEnc = GetMapEncounterRef(sti(rPre.type));
		if (!CheckAttribute(rEnc, "RealEncounterType")) continue;
		iEncT2 = sti(rEnc.RealEncounterType);
		iNat2 = PIRATE;
		iShp2 = 0;
		if (iEncT2 == ENCOUNTER_TYPE_ALONE)
		{
			iAci = GetCharacterIndex(rEnc.CharacterID);
			if (iAci >= 0)
			{
				iNat2 = sti(characters[iAci].nation);
				iShp2 = GetCompanionQuantity(&characters[iAci]) + 1;
			}
		}
		else
		{
			if (CheckAttribute(rEnc, "Nation")) iNat2 = sti(rEnc.Nation);
			if (CheckAttribute(rEnc, "NumWarShips")) iShp2 = iShp2 + sti(rEnc.NumWarShips);
			if (CheckAttribute(rEnc, "NumMerchantShips")) iShp2 = iShp2 + sti(rEnc.NumMerchantShips);
		}
		aEncNat[ip] = iNat2;
		if (iNat2 != PIRATE)
		{
			if (iShp2 >= 2 && iShp2 <= 9) aEncForm[ip] = 1; // ~bSquadForm (см. CalculateGroupShipPos)
		}
		fCentX = fCentX + stf(rPre.x);
		fCentZ = fCentZ + stf(rPre.z);
		iCentN = iCentN + 1;
	}
	// детект AI-vs-AI: пара ФОРМАЦИОННЫХ групп с взаимно-вражьими нациями. Сторона A = нация первой такой.
	bool  bAiVsAi = false;
	int   iNatA = -1;
	int   ipa;
	int   ipb;
	for (ipa = 0; ipa < iPreN; ipa++)
	{
		if (bAiVsAi) break;
		if (aEncForm[ipa] != 1) continue;
		for (ipb = 0; ipb < iPreN; ipb++)
		{
			if (ipb == ipa) continue;
			if (aEncForm[ipb] != 1) continue;
			if (GetNationRelation(aEncNat[ipa], aEncNat[ipb]) == RELATION_ENEMY)
			{
				bAiVsAi = true;
				iNatA = aEncNat[ipa];
				break;
			}
		}
	}
	// направление ПРОТИВОСТОЯНИЯ = от ГГ к центроиду сырых позиций; якоря двух фракций = центр ±перп
	float fFwX = 0.0;
	float fFwZ = 1.0;
	if (bAiVsAi)
	{
		if (iCentN > 0)
		{
			float fmagx = fCentX / makefloat(iCentN);
			float fmagz = fCentZ / makefloat(iCentN);
			float fml = GetDistance2D(fEncPx, fEncPz, fmagx, fmagz);
			if (fml > 1.0)
			{
				fFwX = (fmagx - fEncPx) / fml;
				fFwZ = (fmagz - fEncPz) / fml;
			}
		}
	}
	float fPerpX = -1.0 * fFwZ;
	float fPerpZ = fFwX;
	float fFaceCX = fEncPx + fFwX * SEA_ENC_FACE_FWD;
	float fFaceCZ = fEncPz + fFwZ * SEA_ENC_FACE_FWD;
	float fAncAx = fFaceCX + fPerpX * SEA_ENC_FACE_HALF;
	float fAncAz = fFaceCZ + fPerpZ * SEA_ENC_FACE_HALF;
	float fAncBx = fFaceCX - fPerpX * SEA_ENC_FACE_HALF;
	float fAncBz = fFaceCZ - fPerpZ * SEA_ENC_FACE_HALF;
	int   iFacACnt = 0;
	int   iFacBCnt = 0;

	for (i=0; i<iNumGroups; i++)
	{
		int iAloneCharIndex = -1;

		rRawGroup = GetAttributeN(arEncounters, i);
		rEncounter = GetMapEncounterRef(sti(rRawGroup.type));

		if (!CheckAttribute(rEncounter, "RealEncounterType")) // boal проверка на лажу
		{
			continue;
		}
		int iEncounterType = sti(rEncounter.RealEncounterType);

		x = stf(rRawGroup.x);
		z = stf(rRawGroup.z);
		ay = stf(rRawGroup.ay);

		// === LEO: геометрия спавна — динамическая дистанция (формация далеко / мелочь близко) + AI-vs-AI треугольник ===
		// x,z сейчас = позиция с глобалки; отсюда её берут ОБА пути логина (фантом @Group_SetXZ_AY,
		// ALONE/квест -> rGroup.Pos -> Sea_LoginGroup). Правим ДО ветвления ALONE/фантом.
		float fdd;
		// дистанция ПО НАЛИЧИЮ ФОРМАЦИИ у этой группы: формация -> ДАЛЬШЕ (виден подход строем), мелочь/пираты -> БЛИЖЕ
		float fPMin = SEA_ENC_PDIST_PLAIN;
		float fGMin = SEA_ENC_GDIST_PLAIN;
		int   iMyForm = 0;
		if (i < SEA_ENC_MAX) iMyForm = aEncForm[i];
		if (iMyForm == 1) { fPMin = SEA_ENC_PDIST_FORM; fGMin = SEA_ENC_GDIST_FORM; }

		bool bPlacedFaceoff = false;
		// --- AI-vs-AI: формационные группы -> к якорю СВОЕЙ фракции, НОСОМ на врага (треугольник, ГГ-наблюдатель на вершине) ---
		if (bAiVsAi)
		{
			if (iMyForm == 1)
			{
				bool bSideA = true;
				if (GetNationRelation(aEncNat[i], iNatA) == RELATION_ENEMY) bSideA = false; // вражья стороне A -> сторона B
				float fBaseX = fAncAx;
				float fBaseZ = fAncAz;
				float fFaceX = -1.0 * fPerpX; // сторона A смотрит на B (вдоль -перп)
				float fFaceZ = -1.0 * fPerpZ;
				int   iCnt = iFacACnt;
				if (bSideA)
				{
					iFacACnt = iFacACnt + 1;
				}
				else
				{
					fBaseX = fAncBx; fBaseZ = fAncBz;
					fFaceX = fPerpX; fFaceZ = fPerpZ; // сторона B смотрит на A
					iCnt = iFacBCnt;
					iFacBCnt = iFacBCnt + 1;
				}
				// несколько групп на стороне -> смещаем В ГЛУБИНУ (вперёд, за переднюю линию) вдоль fFw на gdist,
				// чтоб не накладывались И не подтягивались к ГГ (первая группа каждой стороны = iCnt 0 -> без смещения)
				float fOff = makefloat(iCnt) * SEA_ENC_GDIST_FORM;
				x = fBaseX + fFwX * fOff;
				z = fBaseZ + fFwZ * fOff;
				ay = Sea_AY_FromDir(fFaceX, fFaceZ); // нос на вражью фракцию
				bPlacedFaceoff = true;
			}
		}

		if (!bPlacedFaceoff)
		{
			// --- обычный разлёт вокруг ГГ (стая на ГГ / одиночка): оттолкнуть, кластер вперёд, разнести, нос на ГГ ---
			// 1) оттолкнуть от ГГ
			fdd = GetDistance2D(fEncPx, fEncPz, x, z);
			if (fdd < fPMin)
			{
				if (fdd < 1.0) { x = fEncPx + fPMin; z = fEncPz; }
				else { x = fEncPx + (x - fEncPx) / fdd * fPMin; z = fEncPz + (z - fEncPz) / fdd * fPMin; }
			}
			// 2) КЛАСТЕР: держать эскадры в ПЕРЕДНЕМ полукруге относительно первой, чтобы ГГ не оказывался
			//    ЗАЖАТ между эскадрами. Из заднего полукруга отражаем вперёд (дистанция сохраняется).
			if (iEncSpreadN > 0)
			{
				float frl = GetDistance2D(fEncPx, fEncPz, aEncSpreadX[0], aEncSpreadZ[0]);
				if (frl > 1.0)
				{
					float frx = (aEncSpreadX[0] - fEncPx) / frl;
					float frz = (aEncSpreadZ[0] - fEncPz) / frl;
					float fdot = (x - fEncPx) * frx + (z - fEncPz) * frz;
					if (fdot < 0.0)
					{
						x = x - 2.0 * fdot * frx;
						z = z - 2.0 * fdot * frz;
					}
				}
			}
			// 3) оттолкнуть от уже расставленных эскадр
			for (j = 0; j < iEncSpreadN; j++)
			{
				fdd = GetDistance2D(aEncSpreadX[j], aEncSpreadZ[j], x, z);
				if (fdd < fGMin)
				{
					if (fdd < 1.0) { x = aEncSpreadX[j] + fGMin; z = aEncSpreadZ[j]; }
					else { x = aEncSpreadX[j] + (x - aEncSpreadX[j]) / fdd * fGMin; z = aEncSpreadZ[j] + (z - aEncSpreadZ[j]) / fdd * fGMin; }
				}
			}
			// 4) повторно гарантируем дистанцию до ГГ (шаги 2-3 могли подтянуть обратно)
			fdd = GetDistance2D(fEncPx, fEncPz, x, z);
			if (fdd < fPMin)
			{
				if (fdd < 1.0) { x = fEncPx + fPMin; z = fEncPz; }
				else { x = fEncPx + (x - fEncPx) / fdd * fPMin; z = fEncPz + (z - fEncPz) / fdd * fPMin; }
			}
			// 5) РАЗВЕРНУТЬ носом к ГГ: строй тянется от флагмана НАЗАД -> уйдёт ОТ ГГ (иначе хвост накрывает ГГ)
			fdd = GetDistance2D(fEncPx, fEncPz, x, z);
			if (fdd > 1.0) ay = Sea_AY_FromDir((fEncPx - x) / fdd, (fEncPz - z) / fdd);
		}
		// запомнить позицию для разнесения/кластера следующих эскадр
		if (iEncSpreadN < SEA_ENC_MAX) { aEncSpreadX[iEncSpreadN] = x; aEncSpreadZ[iEncSpreadN] = z; iEncSpreadN++; }

		ReloadProgressUpdate();

		int iCompanionsQ;
		int cn;

		if (iEncounterType == ENCOUNTER_TYPE_ALONE)
		{
			iAloneCharIndex = GetCharacterIndex(rEncounter.CharacterID);
			if (iAloneCharIndex < 0)
			{
				continue;
			}
			sGName = "Sea_" + rEncounter.CharacterID; //boal для удобства манипулирования в др методах "EncTypeAlone_" + iAloneCharIndex;
			// можно задать группу их 10 кораблей или всего 4 компаньона - работает и так, и так. И даже задать и то, и то. Будут вместе.

			Group_AddCharacter(sGName, rEncounter.CharacterID);

			iCompanionsQ = GetCompanionQuantity(&Characters[iAloneCharIndex]);
			if(iCompanionsQ > 1)
			{
				for(k = 1; k < COMPANION_MAX; k++)
				{
					cn = GetCompanionIndex(&characters[iAloneCharIndex], k);
					if (cn != -1)
					{
						Group_AddCharacter(sGName, characters[cn].id);
					}
				}
			}
			Group_SetGroupCommander(sGName, characters[iAloneCharIndex].id);
			if(GetNationRelation2MainCharacter(sti(characters[iAloneCharIndex].nation)) == RELATION_ENEMY)
			{
				Group_SetTaskAttack(sGName, PLAYER_GROUP);
				Group_LockTask(sGName);
			}
			rEncounter.qID = sGName; // перевел все в группу boal 23/06/06
		}
		else
		{
			sGName = rEncounter.GroupName;
		}
		// check for Quest fantom
		if (CheckAttribute(rEncounter, "qID"))
		{
			Trace("SEA: Login quest encounter " + rEncounter.qID);
			Group_SetAddressNone(rEncounter.qID);
			Group_SetXZ_AY(rEncounter.qID, x, z, ay);
			if (rEncounter.qID == "Sea_BSBons" && CheckAttribute(pchar, "BonsFlintRunning")) { continue; }
			Sea_LoginGroup(rEncounter.qID);
			continue;
		}

		Sea_AddGroup2TaskList(sGName);

		rGroup = Group_FindOrCreateGroup(sGName); // надо так
		Group_SetXZ_AY(sGName, x, z, ay);
		Group_SetType(sGName, rEncounter.Type);
		Group_DeleteAtEnd(sGName);

		// copy task attributes from map encounter to fantom group
		if (CheckAttribute(rEncounter, "Task"))
		{
			rGroup.Task = rEncounter.Task;
		}
		if (CheckAttribute(rEncounter, "Task.Target"))
		{
			rGroup.Task.Target = rEncounter.Task.Target;
		}
		if (CheckAttribute(rEncounter, "Task.Pos"))
		{
			rGroup.Task.Target.Pos.x = rEncounter.Task.Pos.x;
			rGroup.Task.Target.Pos.z = rEncounter.Task.Pos.z;
		}
		if (CheckAttribute(rEncounter, "Lock") && sti(rEncounter.Lock)) { Group_LockTask(sGName); }
		int iNumWarShips = 0;
		int iNumMerchantShips = 0;
		int iNation = PIRATE;
		if(CheckAttribute(rEncounter, "NumWarShips"))
		{
			iNumWarShips = sti(rEncounter.NumWarShips);
		}

		if(CheckAttribute(rEncounter, "NumMerchantShips"))
		{
			iNumMerchantShips = sti(rEncounter.NumMerchantShips);
		}
		if(CheckAttribute(rEncounter, "Nation"))
		{
		iNation = sti(rEncounter.Nation);
		}

		int iNumFantomShips;
		if (checkattribute(rEncounter, "v2")) iNumFantomShips = Fantom_GenerateShips_ForEnc_v2(rEncounter, iEncounterType, sGName);
			else iNumFantomShips = Fantom_GenerateEncounterExt(sGName, iEncounterType, iNumWarShips, iNumMerchantShips, iNation);

		// Ugeen --> генерация параметров	для спецэнкаунтеров
		if (iEncounterType == ENCOUNTER_TYPE_BARREL || iEncounterType == ENCOUNTER_TYPE_BOAT)
		{
			iFantomIndex = FANTOM_CHARACTERS + iNumFantoms;
			rFantom = &Characters[iFantomIndex];
			rFantom.id = iFantomIndex;
			rFantom.index = iFantomIndex;
			rFantom.Nation = PIRATE;
			rFantom.EncType  = "pirate";
            rFantom.RealEncounterType = iEncounterType;//boal
			rFantom.reputation = 10 + rand(80);
			rFantom.EncGroupName = sGName;
			rFantom.MainCaptanId = Characters[iFantomIndex].id;
			rFantom.location = sIslandID;
			rGroup.EmptyFantom = true;
			rFantom.sex = "man";
			rFantom.model.animation = "man";
			SetCaptanModelByEncType(rFantom, rFantom.EncType);
			SetRandomNameToCharacter(rFantom);
			SetSeaFantomParam(rFantom, rEncounter.Type);
			int iRank = sti(pchar.rank) - 3;
			if (iRank < 1) iRank = 1;
			SetFantomParamFromRank(rFantom, iRank, false);
			RemoveSpeciality(rFantom);
			SetSpeciality(rFantom,PerksChars());
			rFantom.SeaAI.Group.Name = sGName;
			Group_AddCharacter(sGName, rFantom.id);
			Log_TestInfo("Generate Special Encounter");
			EmptyFantom_DropGoodsToSea(rFantom, iEncounterType);
			continue;
		}
		// <-- Ugeen

        //navy --> 28.12.2009 изменение алгоритам загрузки кораблей случаек в море, чтобы ГГ мордой в центр экскадры не грузился.
        float b, x_mc, z_mc, ay_mc, ay_res, ay_e, z1;
        bool isMChrAttack = false;

        //координаты ГГ
        x_mc = 	stf(Login.PlayerGroup.x);
        z_mc = 	stf(Login.PlayerGroup.z);
        ay_mc = stf(Login.PlayerGroup.ay);

		if(ay_mc < 0) ay_mc = ay_mc + PIm2;
		ay_e = ay;
		if(ay < 0) ay_e = ay_e + PIm2;

        //угол результирующего вектора между случайкой и ГГ на карте
        ay_res = atan(-((z_mc-z)/(x-x_mc)));

		Trace("ay_res = " + ay_res);

        //т.к. арктангенс даёт только острые углы, то считаем тупые
        if (ay_res < 0 && z > z_mc)
        {
            ay_res += PI;
        }

        if (ay_res > 0 && z < z_mc)
        {
            ay_res += PI;
        }
        if (cos(ay_res - ay_mc) > 0)
		{
			isMChrAttack = true;
		}

        if (isMChrAttack)
        {
            //определяем знак приращения координаты Х
            if (abs(ay) < PI) k = 1;
            else k = -1;

            //уравнение прямой для случайки z = k * x + b
            b = z - ay_e * x;

			Trace("Set group new coords : " + sGName + ", x = " + x + ", z = " + z + ", ay_e = " + ay_e + ", b = " + b + ", k = " + k);

        }

        //navy <--
		// load ship to sea
		if (iNumFantomShips)
		{
			for (j=0; j<iNumFantomShips; j++)
			{
				iFantomIndex = FANTOM_CHARACTERS + iNumFantoms - iNumFantomShips + j;
				rFantom = &Characters[iFantomIndex];
                DeleteAttribute(rFantom, "items"); // boal 28.07.04 фикс кучи сабель, когда идёт в плен
				rFantom.id = "fenc_" + iFantomIndex;
                // boal 26.02.2004 -->
				rFantom.location = sIslandID;
				// boal 26.02.2004 <--
				// set commander to group
				if (j==0)
				{
					Group_SetGroupCommander(sGName, Characters[iFantomIndex].id);
					// LEO: персистентная форма СТРОЯ из генерации (MapEncounters) -> на командира. CalculateGroupShipPos
					//      читает командира -> при перезаходе форма ТА ЖЕ (раньше катилась rand на новом фантоме -> рероллилась).
					if (CheckAttribute(rEncounter, "RndFormation")) Characters[iFantomIndex].SeaAI.RndFormation = sti(rEncounter.RndFormation);
				}

				// set random character and ship names, face id
				rFantom.sex = "man";
				rFantom.model.animation = "man";
				rFantom.Nation = iNation;
				// boal разговор в море -->
                rFantom.reputation = 10 + rand(80);
                rFantom.EncType      = rEncounter.Type; // тип  war, trade pirate
                rFantom.RealEncounterType = iEncounterType;//boal
                rFantom.EncGroupName = sGName;
                rFantom.MainCaptanId = Characters[iFantomIndex - j].id;
				rFantom.WatchFort = true; //следить за фортом
				rFantom.AnalizeShips = true; //анализить враждебные корабли сразу же с загрузки и далее
				if(rand(10) == 1) rFantom.DontRansackCaptain = true;
                if (CheckAttribute(rFantom, "Ship.Mode"))
                {
                	SetCaptanModelByEncType(rFantom, rFantom.Ship.Mode);
                }
                else
                {
                    SetCaptanModelByEncType(rFantom, rEncounter.Type);
                }
                // boal разговор в море <--

				SetRandomNameToCharacter(rFantom);
				SetRandomNameToShip(rFantom);


				SetSeaFantomParam(rFantom, rEncounter.Type); // все там

				if(j == 0 && rFantom.EncType == "pirate")
				{
					rFantom.Flags.Pirate = rand(4);
				}

				trace("bSeaCanGenerateShipSituation = " + bSeaCanGenerateShipSituation);

				//ugeen --> установка возможных ситуаций в каюте кэпа при абордаже - взрыв или эпидемия
				if(j != 0 && bSeaCanGenerateShipSituation && iEncounterType != ENCOUNTER_TYPE_ALONE && pchar.CanGenerateShipSituation)  // флагманы и одиночки исключаем
				{
					if (CheckAttribute(rFantom, "hunter")) // ОЗГ или ДУ
					{
						if(rFantom.hunter == "hunter") Fantom_SetQuestSitiation(rFantom, "hunter");
						else Fantom_SetQuestSitiation(rFantom, "pirate");
					}
					else
					{
						if(sti(rFantom.Nation) == PIRATE) Fantom_SetQuestSitiation(rFantom, "pirate");
						else
						{
							if(rFantom.EncType == "trade") // военники в составе торговых караванов
							{
								if (CheckAttribute(rFantom, "Ship.Mode") && rFantom.Ship.Mode != "trade")
								{
									Fantom_SetQuestSitiation(rFantom, rFantom.Ship.Mode); // торгаши отдельно
								}
							}
							else Fantom_SetQuestSitiation(rFantom, rFantom.EncType); // военные корабли - патрули и пр.
						}
					}
				}
				if(bSeaCanGenerateShipSituation && pchar.CanGenerateShipSituation)
				{
					if (iEncounterType == ENCOUNTER_TYPE_MERCHANT_SMALL || iEncounterType == ENCOUNTER_TYPE_MERCHANT_MEDIUM) // а вот тут торгаши - устанавливаем для флотилии из одного-двух кораблей
					{
						Fantom_SetQuestSitiation(rFantom, rFantom.EncType);
					}
				}
				// <-- ugeen
				Fantom_SetCannons(rFantom, rEncounter.Type);
				Fantom_SetSails(rFantom, rEncounter.Type);
                // boal <--
				rFantom.SeaAI.Group.Name = sGName;
				rFantom.Experience = 0;
				rFantom.Skill.FreeSkill = 0;

				// boal герб на флагман -->
				DeleteAttribute(rFantom, "ShipSails.gerald_name");   // мог быть с того раза
				if (j == 0 || GetCharacterShipClass(rFantom) == 1)
				{
					SetRandGeraldSail(rFantom, sti(rFantom.Nation));
				}
                // boal герб на флагман <--

				// add fantom
				Group_AddCharacter(sGName, rFantom.id);
			}

			for (j = 0; j < iNumFantomShips; j++)
			{
				// add to sea
				Ship_Add2Sea(FANTOM_CHARACTERS + iNumFantoms - iNumFantomShips + j, 0, rEncounter.Type);
			}
		}
	}

	ReloadProgressUpdate();

	// set tasks 2 all groups
	for (i=0; i<GetArraySize(&sTaskList)-2; i++)
	{
		string sGroupID = sTaskList[i];

		rGroup = Group_GetGroupByID(sGroupID);

		// set task
		switch (sti(rGroup.Task))
		{
			case AITASK_RUNAWAY:
				Group_SetTaskRunAway(sGroupID, rGroup.Task.Target);
			break;
			case AITASK_ATTACK:
				Group_SetTaskAttack(sGroupID, rGroup.Task.Target);
			break;
			case AITASK_MOVE:
				if (CheckAttribute(rGroup, "Task.Target.Pos"))
				{
					Group_SetTaskMove(sGroupID, stf(rGroup.Task.Target.Pos.x), stf(rGroup.Task.Target.Pos.z));
				}
				else
				{
					x = 10000.0 * sin(stf(rGroup.Pos.ay));
					z = 10000.0 * cos(stf(rGroup.Pos.ay));
					Group_SetTaskMove(sGName, x, z);
				}
			break;
		}
		if(!CheckAttribute(rGroup,"EmptyFantom"))
		{
			rCharacter = Group_GetGroupCommanderR(rGroup);
			int iRelation = GetRelation(nMainCharacterIndex, sti(rCharacter.index));

		// set relations to all characters in this group
			int qq = 0;
			while (true)
			{
				int iCharacterIndex = Group_GetCharacterIndexR(rGroup, qq); qq++;
				if (iCharacterIndex < 0) { break; }
				SetCharacterRelationBoth(iCharacterIndex, nMainCharacterIndex, iRelation);
			}
		}
	}

	// update AISea
	AISea.DistanceBetweenGroupShips = 250.0;
	AISea.isDone = "";

	InitBattleInterface();							ReloadProgressUpdate();
	StartBattleInterface();							ReloadProgressUpdate();
	RefreshBattleInterface();						ReloadProgressUpdate();
	//Boyer change
	TelescopeInitParameters();

	SendMessage(&Telescope, "leee", MSG_TELESCOPE_INIT_ARRAYS, &Nations, &RealShips, &Goods);

	PostEvent(SHIP_CHECK_RELOAD_ENABLE, 1);

	SetSchemeForSea();								ReloadProgressUpdate();
	sSeaStartMusicName = oldMusicName;

	iRDTSC = RDTSC_E(iRDTSC);

	pchar.space_press = "0";
	DeleteAttribute(pchar, "SkipEshipIndex");// boal

	PostEvent("Sea_FirstInit", 1);
	
	ShipShowName();
	SeaUpdateRPC(Login);
}

void Sea_LoginGroup(string sGroupID)
{
	if (!isEntity(&Sea)) { Trace("Error: Sea_LoginGroup sGroupID = " + sGroupID + ", but Sea is not active!"); return; }

	if (Group_FindGroup(sGroupID) < 0)
	{
		Trace("Not find group '" + sGroupID + "' in groups massive, but login try spotted");
		return;
	}

	ref rGroup = Group_GetGroupByID(sGroupID);
	if (!CheckAttribute(rGroup, "Quest"))			{ Trace("Error: Sea_LoginGroup sGroupID = " + sGroupID + ", but group doesn't contain any quest ships!"); return; }
	if (CheckAttribute(rGroup, "AlreadyLoaded"))	{ Trace("Error: Group sGroupID = " + sGroupID + ", already loaded... check for duplicate group login"); return; }
	if (Group_GetGroupCommanderIndexR(rGroup) < 0)
	{
		Group_DeleteAtEnd(sGroupID);
		return;
	}

	aref arQuestShips; makearef(arQuestShips, rGroup.Quest);

	int iNumQuestShips = GetAttributesNum(arQuestShips);
	if (iNumQuestShips == 0) { return; }

	float x, z, ay; x = 0.0; z = 0.0; ay = 0.0;

	// find group position
	if (CheckAttribute(rGroup, "location") && CheckAttribute(rGroup, "location.group") && rGroup.location != "none")
	{
		string sLocationGroup = rGroup.location.group;
		string sLocationLocator = rGroup.location.locator;

		ref rIsland = GetIslandByID(rGroup.location);

		string sTst = sLocationGroup + "." + sLocationLocator + ".x";
		if (CheckAttribute(rIsland, sTst))
		{
			x = stf(rIsland.(sLocationGroup).(sLocationLocator).x);
			z = stf(rIsland.(sLocationGroup).(sLocationLocator).z);
			ay = stf(rIsland.(sLocationGroup).(sLocationLocator).ay);
		}
		else
		{
			x = 0.0; z = 0.0; ay = 0.0;
			Trace("ERR: Group " + sGroupID + ", Island " + rGroup.location);
			Trace("ERR: Find locators Group " + sLocationGroup + ", Locator " + sLocationLocator);
		}
	}
	else
	{
		if (CheckAttribute(rGroup, "Pos"))
		{
			x = stf(rGroup.Pos.x);
			z = stf(rGroup.Pos.z);
			ay = stf(rGroup.Pos.ay);
		}
		else
		{
			Trace("Error: Sea_LoginGroup sGroupID = " + sGroupID + ", I can't find any locators or position for this group, maybe you can check this???");
		}
	}
	//Lipsar 2 ряда кораблей

    // 1.2.3 - кильваторный строй, вместо каши в  Group_SetPursuitGroup -->
    if (CheckAttribute(rGroup, "location.neargroup"))
	{
  		float fAngle = frnd() * PIm2;
  		ay = 500.0 + rand(300);
  		x  = stf(NullCharacter.Login.PlayerGroup.x) + ay*cos(fAngle); // окружность радиуса 500
  		z  = stf(NullCharacter.Login.PlayerGroup.z) + ay*sin(fAngle);
  		ay = stf(NullCharacter.Login.PlayerGroup.ay) + fAngle;
	}
	// 1.2.3 - кильваторный строй, вместо каши в  Group_SetPursuitGroup   <--
	// set group position
	Group_SetXZ_AY(sGroupID, x, z, ay);

	// set group commander
	ref rGroupCommander = Group_GetGroupCommander(sGroupID);
	// update commander for SEA AI
	if(sti(rGroupCommander.index) <= 0)
	{
		return;
	}
	Group_SetGroupCommander(sGroupID, rGroupCommander.id);  // странная проверка и назначение, но было и пусть будет


	// set location near
	if (CheckAttribute(rGroup, "location.neargroup"))
	{
		Group_SetPursuitGroup(sGroupID, rGroup.location.neargroup);
	}

	// load group ships
	int iNumDeadCharacters = 0;
	for (int i=0; i<iNumQuestShips; i++)
	{
		aref arShip;

		arShip = GetAttributeN(arQuestShips, i);
		int itmp = GetCharacterIndex(GetAttributeValue(arShip));  // homo fix 29/09/06
		if (itmp != -1)
		{
    		ref rCharacter = GetCharacter(itmp);// bad homo
    		if (!CheckAttribute(rCharacter, "index") || rCharacter.index == "none") // boal for slib
    		{
    			continue;
    		}
    		int iCharacterIndex = sti(rCharacter.index);

    		if(iCharacterIndex <= 0)
    		{
    			continue;
    		}

    		if (LAi_IsDead(rCharacter))
    		{
    			iNumDeadCharacters++; continue;
    		}
    		if (iCharacterIndex == nMainCharacterIndex)
    		{
    			Trace("Error: You assigned main character to quest group... This is a error!");
    			continue;
    		}

    		rCharacter.SeaAI.Group.Name = sGroupID;
    		if (CheckAttribute(rGroup, "location"))
    		{
            	rCharacter.location = rGroup.location;  // установка НПС лацации
            }
    		Ship_Add2Sea(sti(rCharacter.index), bFromCoast, "");

    		ReloadProgressUpdate();
        }
	}

	rGroup.AlreadyLoaded = "";

	if (iNumDeadCharacters == iNumQuestShips)
	{
		Trace("Warn: I am automatic delete group '" + sGroupID +"', because it's empty");
		Group_DeleteAtEnd(sGroupID);
		return;
	}

	bSeaQuestGroupHere = true;

	Sea_AddGroup2TaskList(sGroupID);
}

void Sea_FirstInit()
{
	bSeaLoaded = true;
	SeaTowOrders_Suspend();
	SeaTow_Resume();
	pchar.Tmp.BortsReloaded.SkipFirst = true; // движок на первом кадре после логина шлёт Shp_BortReloaded по всем бортам (QuickRecharge), доклад "Орудия заряжены" тут не нужен
	RefreshBattleInterface();
	if( SeaCameras.Camera == "SeaDeckCamera" ) {
		Sailors.IsOnDeck = "1";
	}

	CreateEntity(&Seafoam,"Seafoam");
	LayerAddObject(SEA_EXECUTE, &Seafoam, -1);
	LayerAddObject(SEA_REALIZE, &Seafoam, -1);
	if (Whr_IsStorm()) { Seafoam.storm = "true"; }

	FiveArmies_SeaCheck(); // LEO: пасхалка - догрузить остальные четыре воинства

	QuestsCheck(); // boal 26/05/06 тут ему место
}

void Sea_Reload()
{
	DelEventHandler("Sea_Reload", "Sea_Reload");

	object Login;

	Login.PlayerGroup.ay = 0.0;
	Login.PlayerGroup.x = 0.0;
	Login.PlayerGroup.y = 0.0;
	Login.PlayerGroup.z = 0.0;
	Login.Island = pchar.location;

	SeaLogin(&Login);
}

void Sea_ReloadStart()
{
	if (!bSeaActive) { return; }
	if (Boarding_Busy(pchar)) return;
	ShipsInit();
	DeleteSeaEnvironment();
	SetEventHandler("Sea_Reload", "Sea_Reload", 0);
	PostEvent("Sea_Reload", 1);
}

ref		rSeaLoadResult;
object	oSeaSave;

void Sea_Save()
{
	DeleteAttribute(&oSeaSave, "");

	// LEO: запомнить режим камеры (палуба/3-е лицо/свободная) в pchar (сохраняется), чтобы восстановить при загрузке без скачка
	if (CheckAttribute(&SeaCameras, "Camera")) pchar.seacamera = SeaCameras.Camera;

	SendMessage(&AISea, "l", AI_MESSAGE_SEASAVE);
}

// boal -->
float SetMaxSeaHeight(int islandIdx)
{
	if (!bSeaActive) return   6.0; // ситуция когда нет моря, нет координат pchar.Ship.Pos.x
	if (bStorm) return 200.0;
	string sIslandID = Islands[islandIdx].id;

	float  fMaxViewDist;
    int    i, iQty;

	if (CheckAttribute(Islands[islandIdx], "MaxSeaHeight")) return stf(Islands[islandIdx].MaxSeaHeight);

	aref arReloadLoc, arLocator;
	makearef(arReloadLoc, Islands[islandIdx].reload);
	string  sLabel;
	iQty = GetAttributesNum(arReloadLoc);
	for (i=0; i<iQty; i++)
	{
		arLocator = GetAttributeN(arReloadLoc, i);
		sLabel = arLocator.label;

		//расстояние до бухт и маяков
		if (findsubstr(sLabel, "Shore" , 0) != -1 || findsubstr(sLabel, "Mayak" , 0) != -1)
		{
			if (CheckAttribute(pchar, "Ship.Pos.x") && CheckAttribute(arLocator, "x"))  // fix кривых локаторов у острова, правка должна быть в модели to_do
			{
				if (GetDistance2D(stf(pchar.Ship.Pos.x), stf(pchar.Ship.Pos.z), stf(arLocator.x), stf(arLocator.z)) < 1500)
					return 6.0;
			}
			else
			{
				trace("Error: проблема определения SetMaxSeaHeight для " + sLabel);
			}
		}
		//расстояние до форта
		if (findsubstr(sLabel, "Fort" , 0) != -1)
		{
			if (CheckAttribute(pchar, "Ship.Pos.x") && CheckAttribute(arLocator, "x"))  // fix кривых локаторов у острова, правка должна быть в модели to_do
			{
				if (GetDistance2D(stf(pchar.Ship.Pos.x), stf(pchar.Ship.Pos.z), stf(arLocator.x), stf(arLocator.z)) < 1700)
					return 6.0;
			}
			else
			{
				trace("Error: проблема определения SetMaxSeaHeight для " + sLabel);
			}
		}
		//расстояние до порта
		if (findsubstr(sLabel, "Port" , 0) != -1)
		{
			if (CheckAttribute(pchar, "Ship.Pos.x") && CheckAttribute(arLocator, "x"))  // fix кривых локаторов у острова, правка должна быть в модели to_do
			{
				if (GetDistance2D(stf(pchar.Ship.Pos.x), stf(pchar.Ship.Pos.z), stf(arLocator.x), stf(arLocator.z)) < 2000)
					return 6.0;
			}
			else
			{
				trace("Error: проблема определения SetMaxSeaHeight для " + sLabel);
			}
		}
	}
	return 200.0;
}

/*!
 * \brief Установка параметров травы в море значениями из настроек
 */
void UpdateSeaGrass()
{
	if (!CheckAttribute(&AISea,"Island"))
	{
		return;
	}
	string sIslandID = AISea.Island;
	if (sIslandID == "") { return; }
	if(sIslandID == "StLucia" && PChar.MysteriousIsland.StLuciaHide == true)
	{
		sIslandID = "";
	}
	int iIslandIndex = FindIsland(sIslandID);
	if (iIslandIndex != -1 && Islands[iIslandIndex].visible == true)
	{
		/* Установка параметров травы: масштаб, максимальная ширина, максимальная высота, минимальная дистанция переключения лодов, максимальная отображаемая дистанция, кол-во травы, 1 - нет, 0 - максимум */
		if( CheckAttribute(&Islands[iIslandIndex],"jungle") )
		{
			float fJungleScale = 10.0;
			if( CheckAttribute(&Islands[iIslandIndex],"jungle.scale") )
			{
				fJungleScale = stf(Islands[iIslandIndex].jungle.scale);
			}
			float GrassDistanceSea = stf(InterfaceStates.GrassDistanceSea)*4000.0;
			// Gregg: ползунок «Количество спрайтов» -> пол LOD через обратную формулу покрытия
			// (см. GrassLodFromAmount в utils.c). Линейный ход не годился: уровни LOD в .grs
			// нелинейны, из-за чего верхняя треть ползунка не меняла ничего, а дальше был обрыв.
			float GrassCountSea = GrassLodFromAmount(stf(InterfaceStates.GrassCountSea));
			DeleteGrass();
			CreateGrass("resource\models\islands\"+ Islands[iIslandIndex].id +"\"+ Islands[iIslandIndex].jungle.patch + ".grs", "Grass\"+Islands[iIslandIndex].jungle.texture+".tga.tx", fJungleScale, 32.0, 256.0, GrassDistanceSea/10, GrassDistanceSea, GrassCountSea);
		}
	}
}

// boal <--
void Sea_LoadIsland(string sIslandID)
{
	bIsFortAtIsland = false;
	if (sIslandID == "") { return; }

	int iIslandIndex = FindIsland(sIslandID);
	if (iIslandIndex != -1 && Islands[iIslandIndex].visible == true)
	{
		// boal -->
		float  fMaxViewDist;
        Sea.MaxSeaHeight = SetMaxSeaHeight(iIslandIndex); // тут нужно для загрузки игры из сайва, для нормального перехода не работает, тк ГГ ещё не в море, нет коорд
        Log_TestInfo("Sea_LoadIsland Sea.MaxSeaHeight " + Sea.MaxSeaHeight);
		// boal <--
		CreateEntity(&Island, "Island");
		Island.LightingPath = "";
		Island.dynamicLightsOn = 1;
		Island.ImmersionDistance = Islands[iIslandIndex].ImmersionDistance;			// distance = fRadius * ImmersionDistance, from island begin immersion
		Island.ImmersionDepth = Islands[iIslandIndex].ImmersionDepth;			// immersion depth = (Distance2Camera / (fRadius * ImmersionDistance) - 1.0) * ImmersionDepth
		string sTexturePath = "islands\" + Islands[iIslandIndex].TexturePath + "\";
		SetTexturePath(0, sTexturePath);
		Island.FogDensity = Weather.Fog.IslandDensity;
		// трава на остров
		if( CheckAttribute(&Islands[iIslandIndex],"jungle") )
		{
			float fJungleScale = 10.0;
			if( CheckAttribute(&Islands[iIslandIndex],"jungle.scale") )
			{
				fJungleScale = stf(Islands[iIslandIndex].jungle.scale);
			}
			float GrassDistanceSea = stf(InterfaceStates.GrassDistanceSea)*4000.0;
			// Gregg: та же формула, что и во втором месте создания травы (см. комментарий выше)
			float GrassCount = GrassLodFromAmount(stf(InterfaceStates.GrassCountSea));
			CreateGrass("resource\models\islands\"+ Islands[iIslandIndex].id +"\"+ Islands[iIslandIndex].jungle.patch + ".grs", "Grass\"+Islands[iIslandIndex].jungle.texture+".tga.tx", fJungleScale, 32.0, 256.0, GrassDistanceSea/10, GrassDistanceSea, GrassCount);
		}
		SendMessage(&SeaLighter, "ss", "ModelsPath", Islands[iIslandIndex].filespath.models);
		SendMessage(&SeaLighter, "ss", "LightPath", "");

		SendMessage(&Island, "lsss", MSG_ISLAND_LOAD_GEO, "islands", Islands[iIslandIndex].filespath.models, Islands[iIslandIndex].model);
		LayerAddObject(SEA_REALIZE, &Island, 4);
		LayerAddObject(MAST_ISLAND_TRACE, &Island, 1);
		LayerAddObject(SUN_TRACE, &Island, 1);
		fMaxViewDist = 6000.0;
		if(CheckAttribute(&Islands[iIslandIndex], "maxviewdist"))
		{
			fMaxViewDist = stf(Islands[iIslandIndex].maxviewdist);
		}
		SendMessage(&Island, "lf", MSG_MODEL_SET_MAX_VIEW_DIST, fMaxViewDist);

		CreateEntity(&IslandReflModel, "MODELR");
		string sReflModel = Islands[iIslandIndex].filespath.models + "\" + Islands[iIslandIndex].refl_model;
		SendMessage(&IslandReflModel, "ls", MSG_MODEL_SET_LIGHT_PATH, "");
		SendMessage(&IslandReflModel, "ls", MSG_MODEL_LOAD_GEO, sReflModel);
		SendMessage(&IslandReflModel, "lllf", MSG_MODEL_SET_FOG, 1, 1, stf(Weather.Fog.IslandDensity));
		LayerAddObject(SEA_REFLECTION2, &IslandReflModel, -1);
		SendMessage(&SeaLighter, "ssi", "AddModel", Islands[iIslandIndex].refl_model, &IslandReflModel);
		Sea_CreateLighthouse(sIslandID);
		bIslandLoaded = true;
		SendMessage(&SeaLocatorShow, "a", &Islands[iIslandIndex]);
		Fort_Login(iIslandIndex);
		SetTexturePath(0, "");

		CreateCoastFoamEnvironment(sIslandID, SEA_EXECUTE, SEA_REALIZE);
		//eddy. запишем в переменные координаты форта
		aref arReloadFort;
		makearef(arReloadFort, Islands[iIslandIndex].reload.l2);
		if (!CheckAttribute(arReloadFort, "colonyname")) return;
		string sTest = arReloadFort.colonyname + " Fort";
		if (arReloadFort.label == sTest && CheckAttribute(arReloadFort, "x") && CheckAttribute(arReloadFort, "z"))
		{
			fFort_x = stf(arReloadFort.x);
			fFort_z = stf(arReloadFort.z);
			int iColony = FindColony(arReloadFort.colonyname);
			iFortNation = sti(colonies[iColony].nation);
			iFortCommander = sti(colonies[iColony].commanderIdx);
			//следить за фортом только, если он жив
			if (!CheckAttribute(&characters[iFortCommander], "Fort.Mode") || sti(characters[iFortCommander].Fort.Mode) != FORT_DEAD)
			{
				bIsFortAtIsland = true;
			}
		}
	}
	DeleteAttribute(pchar,"ContraInterruptWaiting");
}

// Warship Метод по созданию освещения маяков в море
void Sea_CreateLighthouse(String _islandID)
{
	ref islandRef;
	int islandIndex = FindIsland(_islandID);

	String lighthouseLightModelName;

	if(IsEntity(lighthouseLightModel))
	{
		DeleteClass(&lighthouseLightModel);
	}

	if(islandIndex == -1) return;

	islandRef = &Islands[islandIndex];

	//--> eddy. да будет свет на маяке
	if(CheckAttribute(&islandRef, "mayak"))
	{
		CreateEntity(&sLightModel, "MODELR");

		if(isDay())
		{
			lighthouseLightModelName = islandRef.filespath.models + "\" + islandRef.mayak.model_day;
		}
		else
		{
			lighthouseLightModelName = islandRef.filespath.models + "\" + islandRef.mayak.model_night;
		}

		SendMessage(&sLightModel, "ls", MSG_MODEL_LOAD_GEO, lighthouseLightModelName);
		SendMessage(&island, "li", MSG_ISLAND_ADD_FORT,  &sLightModel); // &island - обьект-остров (сущность), а не элемент массива Islands[]
		SendMessage(SeaLighter, "ssi", "AddModel", islandRef.mayak.model_night, &sLightModel);
	}
	//<-- eddy. да будет свет на маяке

	// Warship Новое - создание света маяка -->
	// Атрибут маяка у острова тут не проверяется - он есть только у маяка Ямайки
	if(_islandID == "Jamaica" || _islandID == "Cuba1" || _islandID == "Cuba2")
	{
		if(!IsDay())
		{
			CreateEntity(&lighthouseLightModel, "MODELR");
			// Коммент - текстура для модельки будет браться из "RESOURCE\TEXTURES\" + DIRPATH и никак это не исправить :(
			// В общем, текстура должна лежать в той же папке, что и модель, только в каталоге "TEXTURES"
			SendMessage(&lighthouseLightModel, "ls", MSG_MODEL_LOAD_GEO, "lighthouse_volumeLight");
			SendMessage(&lighthouseLightModel, "ls", MSG_MODEL_SET_TECHNIQUE, "LighthouseLight");
			LayerAddObject(SEA_EXECUTE, &lighthouseLightModel, ITEMS_LAYER);
			LayerAddObject(SEA_REALIZE, &lighthouseLightModel, ITEMS_LAYER);

			if(_islandID == "Jamaica")
			{
				SendMessage(&lighthouseLightModel, "lffffffffffff", MSG_MODEL_SET_POSITION, 3000.0, 24.5, -1679.1, 1, 0, 0, 0, 1, 0, 0, 0, 1);
			}

			if(_islandID == "Cuba1")
			{
				// TO_DO Крутануть его - криво сейчас
				SendMessage(&lighthouseLightModel, "lffffffffffff", MSG_MODEL_SET_POSITION, 627.0, 24.0, -2170.5, 1, 0, 0, 0, 1, 0, 0, 0, 1);
			}

			if(_islandID == "Cuba2")
			{
				SendMessage(&lighthouseLightModel, "lffffffffffff", MSG_MODEL_SET_POSITION, -413.0, 65.5, 815.1, 1, 0, 0, 0, 1, 0, 0, 0, 1);
			}
		}
	}
	// <-- создание света маяка
}
bool bSeaLoad = false;

void Sea_Load()
{
	SeaUpdateRPC(AISea);
	int i, iTemp[MAX_SHIPS_ON_SEA];
	ref chr;

	bSeaLoad = true;

	for (i = 0; i < MAX_SHIPS_ON_SEA; i++) { ShipModelrList[i] = -1; Ships[i] = -1;}
	iNumShips = 0;

	CreateSeaEnvironment();
	// login island if exist
	Sea_LoadIsland(AISea.Island);

	ShipOnLoadModelrList = GetCurrentModelrNumber(); //  вот так вот -- вычисляем номер модельки корабля ГГ  ДО загрузки остальных шипов в море !!!!
	trace("ShipOnLoadModelrList = " + ShipOnLoadModelrList + " Ships[0] = " + Ships[0]);

	SendMessage(&AISea, "l", AI_MESSAGE_SEALOAD);
	Boarding_OnSeaLoad();

	for (i = 0; i < MAX_SHIPS_ON_SEA; i++) iTemp[i] = ShipModelrList[i];

	//принципиальный момент !!!  двигаем  массив номеров моделек шипов вправо !! - иначе для флагмана ГГ приписывается неправильный номер модельки
	// два дня понять не мог в чём дело - почему неправильно выставляются флаги при загрузке игры в режиме "море"
	for (i = 0; i < MAX_SHIPS_ON_SEA - 1; i++) { ShipModelrList[i + 1] = iTemp[i]; }
	ShipModelrList[0] = ShipOnLoadModelrList;

	for (i = 0; i < MAX_SHIPS_ON_SEA; i++)
	{
		if(Ships[i] > 0)
		{
			chr = GetCharacter(Ships[i]);
			sTemp = ""; // LEO: у стёртого слота имени корабля нет вовсе - читаем через проверку
			if(CheckAttribute(chr, "Ship.Name")) sTemp = chr.Ship.Name;
		}
		else
		{
			sTemp = "";
			ShipModelrList[i] = -1;
		}
		if(ShipModelrList[i] == -1) continue;
		trace("i = " + i + " ShipModelrList[i] = " + ShipModelrList[i] + " Ships[i] = " + Ships[i] + " ship.name = " + sTemp);
	}

	SendMessage(&Telescope, "leee", MSG_TELESCOPE_INIT_ARRAYS, &Nations, &RealShips, &Goods);

	PostEvent(SHIP_CHECK_RELOAD_ENABLE, 1);
	// LEO: восстановить сохранённый режим камеры (палуба/3-е лицо) + пере-привязать к кораблю.
	// Иначе скриптовый SeaCameras.Camera рассинхронен с движковой активной камерой => резкий скачок
	// и неверная схема управления после загрузки. Делаем ДО SetSchemeForSea (схема зависит от камеры) и ДО первого кадра.
	if (CheckAttribute(pchar, "seacamera"))
	{
		SeaCameras.Camera = pchar.seacamera;
		SeaCameras_SetShipForSeaCamera(&pchar);
	}
	SetSchemeForSea();
	PostEvent("Sea_FirstInit", 1);

	DeleteAttribute(&oSeaSave, "");
	bSeaLoad = false;

	InitBattleInterface();
	StartBattleInterface();
	RefreshBattleInterface();
	ShipShowName();
}

/*!
 * \brief Отрисовка текста над кораблями
 */
void ShipShowName()
{
	int i;
	ref chr;

	for (i = 0; i < MAX_SHIPS_ON_SEA; i++)
	{
		if(Ships[i] > 0)
		{
			chr = GetCharacter(Ships[i]);
			
			if(sti(InterfaceStates.EnabledShipMarks) == 1)
			{
				SendMessage(chr, "l", MSG_SHIP_SHOW_MARK);
			}
			else
			{
				SendMessage(chr, "l", MSG_SHIP_HIDE_MARK);
			}

			/* Отрисовка компаньонов */
			if(IsCompanion(chr))
			{
				if(sti(InterfaceStates.ShipCapShowAllies) == 0)
				{
					SendMessage(chr, "l", MSG_SHIP_HIDE_CAPTURE);
				}
				else
				{
					SendMessage(chr, "l", MSG_SHIP_SHOW_CAPTURE);
				}

				/* Отключение отрисовки баров у компаньонов */ 
				if(sti(InterfaceStates.DisableShipMarksCompanions) == 0 && sti(InterfaceStates.EnabledShipMarks) == 1)
				{
					SendMessage(chr, "l", MSG_SHIP_SHOW_MARK);
				}
				else
				{
					SendMessage(chr, "l", MSG_SHIP_HIDE_MARK);
				}
			}
			/* Отрисовка остальных */
			else
			{
				// LEO: в бетта-тесте показываем подписи ВСЕХ кораблей сразу, без подзорки (в т.ч. ПГГ+свиту)
				if (!CheckAttribute(chr,"SpyglassChecked") && !bBettaTestMode) continue;
				if(sti(InterfaceStates.ShipCapShowOther) == 0)
				{
					SendMessage(chr, "l", MSG_SHIP_HIDE_CAPTURE);
				}
				else
				{
					SendMessage(chr, "l", MSG_SHIP_SHOW_CAPTURE);
				}
			}
			
			/* Тип корабля */
			if(sti(InterfaceStates.ShipCapShipNames2) == 0)
			{
				SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT1, "");
			}
			else
			{
				string type = XI_ConvertString(GetShipTypeName(chr));
				SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT1, type + " ");
				SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT1_FONT_SCALE, GetShipCapNamesScale());
			}

			/* Имя корабля */
			if(sti(InterfaceStates.ShipCapShipNames1) == 0)
			{
				SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT2, "");
			}
			else
			{
				// LEO: имя может отсутствовать (стёртый слот, форт) - тогда подпись просто пустая
				string sShipCapName = "";
				if(CheckAttribute(chr, "Ship.Name")) sShipCapName = chr.Ship.Name + " ";
				SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT2, sShipCapName);
				SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT2_FONT_SCALE, GetShipCapNamesScale());
			}
			
			/* Иконка действия */
			// LEO: глифы задачи и снаряда надо не только СТАВИТЬ, но и СНИМАТЬ. Раньше веток «снять» не
			// было вовсе: перестал быть компаньоном (обмен кораблями, роспуск) - иконки залипали навсегда,
			// в том числе на корабле игрока, ведь при обмене корпус переезжает вместе со своим captureText.
			if (!IsMainCharacter(chr))
			{
				if (IsCompanion(chr) || bBettaTestMode || CheckAttribute(chr, "SpyglassCheckedFull"))
				{
					if(sti(InterfaceStates.ShipCapShowActions) == 0)
					{
						SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");
					}
					else
					{
						SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, Ship_BuildActionCaption(chr, sti(chr.SeaAI.Task), sti(chr.Ship.Cannons.Charge.Type)));
						SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT3_FONT_SCALE, GetShipCapIconsScale());
					}
				}
				else
				{
					// Уже не наш: глифы снять, иначе останутся от прежнего владельца
					SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");
				}
			}
			else
			{
				// Корабль игрока глифов действия не показывает никогда - но мог получить их вместе
				// с корпусом при обмене с компаньоном, поэтому чистим явно.
				SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");

				/* Отключение отрисовки баров у игрока */ 
				if(sti(InterfaceStates.DisableShipMarksSelf) == 0 && sti(InterfaceStates.EnabledShipMarks) == 1)
				{
					SendMessage(chr, "l", MSG_SHIP_SHOW_MARK);
				}
				else
				{
					SendMessage(chr, "l", MSG_SHIP_HIDE_MARK);
				}
			}
		}
	}
}

void ShipShowNameSingle(ref chr)
{
	if(sti(InterfaceStates.ShipCapShowOther) == 0)
	{
		return;
	}
	else
	{
		SendMessage(chr, "l", MSG_SHIP_SHOW_CAPTURE);
	}
			
	/* Тип корабля */
	if(sti(InterfaceStates.ShipCapShipNames2) == 0)
	{
		SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT1, "");
	}
	else
	{
		string type = XI_ConvertString(GetShipTypeName(chr));
		SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT1, type + " ");
		SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT1_FONT_SCALE, GetShipCapNamesScale());
	}
	/* Имя корабля */
	
	if(sti(InterfaceStates.ShipCapShipNames1) == 0)
	{
		SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT2, "");
	}
	else
	{
		SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT2, chr.Ship.Name + " ");
		SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT2_FONT_SCALE, GetShipCapNamesScale());
	}
	
	/* Иконка действия */
	// LEO: та же правка, что в ShipShowName - глифы надо снимать, а не только ставить.
	if (IsMainCharacter(chr))
	{
		SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");
	}
	else
	{
		if (bBettaTestMode || IsCompanion(chr) || CheckAttribute(chr, "SpyglassCheckedFull"))
		{
			if(sti(InterfaceStates.ShipCapShowActions) == 0)
			{
				SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");
			}
			else
			{
				SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, Ship_BuildActionCaption(chr, sti(chr.SeaAI.Task), sti(chr.Ship.Cannons.Charge.Type)));
				SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT3_FONT_SCALE, GetShipCapIconsScale());
			}
		}
		else
		{
			SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");
		}
	}
}

// === Тактическая пауза: метки над ВСЕМИ кораблями (Вариант B, Стадия 1) ======
// Форсирует маркер свой/чужой + тип + имя + иконку действия над каждым кораблём
// боя, в обход штатных опций (SpyglassChecked и т.п.). Зовётся из тактического
// вида (AICameras.c: Sea_TacticalViewOn). На выходе: Sea_TacticalShipLabels(false),
// затем ShipShowName() — вернуть отображение к настройкам игрока.
void Sea_TacticalShipLabels(bool bShow)
{
	int i;
	ref chr;
	for (i = 0; i < MAX_SHIPS_ON_SEA; i++)
	{
		if (Ships[i] <= 0) continue;
		chr = GetCharacter(Ships[i]);

		if (!bShow)
		{
			// сами подписи гасим здесь; маркеры/штатные подписи восстановит ShipShowName()
			SendMessage(chr, "l", MSG_SHIP_HIDE_CAPTURE);
			continue;
		}

		// маркер свой/чужой + полоски
		SendMessage(chr, "l", MSG_SHIP_SHOW_MARK);
		// подпись: тип + имя
		SendMessage(chr, "l", MSG_SHIP_SHOW_CAPTURE);
		string sType = XI_ConvertString(GetShipTypeName(chr));
		SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT1, sType + " ");
		SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT1_FONT_SCALE, GetShipCapNamesScale());
		SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT2, chr.Ship.Name + " ");
		SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT2_FONT_SCALE, GetShipCapNamesScale());

		// иконка действия (заряд + текущая задача ИИ) — не для ГГ
		if (!IsMainCharacter(chr))
		{
			string sAct = "";
			// Gregg: задачу и заряд ВРАГА игроку не раскрываем — иконки только у не-враждебных
			// (relation живьём, флаг IsEnemyTacShip для поздних врагов ненадёжен);
			// пустой TEXT3 шлём всегда — стереть иконку, если корабль стал врагом
			if (GetRelation(GetMainCharacterIndex(), Ships[i]) != RELATION_ENEMY)
			{
				if (CheckAttribute(chr, "Ship.Cannons.Charge.Type")) sAct = GetChargedIcon(sti(chr.Ship.Cannons.Charge.Type)) + " ";
				if (CheckAttribute(chr, "SeaAI.Task"))                sAct = sAct + getShipIconFromSeaAITask(sti(chr.SeaAI.Task));
				// LEO: имя корабля-цели в скобках — бетта / компаньон (цель свиты всегда) / полный чек хорошей трубой
				if (Ship_ShowActionTarget(chr))
				{
					string sTgt = Ship_GetTaskTargetShipName(chr);
					if (sTgt != "") sAct = sAct + " (" + sTgt + ")";
				}
			}
			SendMessage(chr, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, sAct);
			SendMessage(chr, "lf", MSG_SHIP_SET_CAPTURE_TEXT3_FONT_SCALE, GetShipCapIconsScale());
		}
	}
}

ref SeaLoad_GetPointer()
{
	string sType = GetEventData();
	int iIndex = GetEventData();

	switch (sType)
	{
		case "character":
			makeref(rSeaLoadResult, Characters[iIndex]);
		break;
		case "ship":
			makeref(rSeaLoadResult, RealShips[iIndex]);
		break;
		case "seacameras":
			makeref(rSeaLoadResult, SeaCameras);
		break;
		case "seasave":
			makeref(rSeaLoadResult, oSeaSave);
		break;
	}
	return rSeaLoadResult;
}
// ============================================================================
// LEO: данные 6 формаций свиты/эскадры. Слот = смещение корабля относительно
// ФЛАГМАНА в единицах интервала (1 = distanceBetweenShips=200): fwd>0 вперёд, lat>0 вправо.
// Слот 0 = флагман (центр). 1..7 = корабли строя. Слоты >7 (большие эскадры) достраиваются
// рядами позади. iAxis: 0=fwd, 1=lat. Формы: 0 Коробка, 1 Ромб, 2 Линия, 3 Щит, 4 Зигзаг, 5 Клин.
// Тюнинг форм — прямо тут (числа), пересборка НЕ нужна.
// ============================================================================
float Sea_FormationOffset(int iForm, int iSlot, int iAxis)
{
	float fwd = 0.0;
	float lat = 0.0;
	if (iSlot >= 8)
	{
		// достройка для больших эскадр — ряды позади, попарно по бортам
		int iExtRank = (iSlot - 8) / 2;
		float fExtSide = -1.0;
		if (((iSlot - 8) % 2) == 1) fExtSide = 1.0;
		fwd = -1.0 * (4.0 + iExtRank * 1.2);
		lat = fExtSide * 1.2;
	}
	else if (iSlot >= 1)
	{
		int iKey = iForm * 10 + iSlot; // десятки = формация, единицы = слот (плоский switch — надёжно в Storm)
		switch (iKey)
		{
			// 0 Коробка — флагман впереди, прямоугольник позади
			case 1:  fwd = -1.0; lat = -1.0; break;
			case 2:  fwd = -1.0; lat =  1.0; break;
			case 3:  fwd = -2.0; lat = -1.0; break;
			case 4:  fwd = -2.0; lat =  1.0; break;
			case 5:  fwd = -3.0; lat = -1.0; break;
			case 6:  fwd = -3.0; lat =  1.0; break;
			case 7:  fwd = -3.5; lat =  0.0; break;
			// 1 Ромб — флагман в центре ромба + хвост
			case 11: fwd =  1.0; lat =  0.0; break;
			case 12: fwd =  0.5; lat = -1.0; break;
			case 13: fwd =  0.5; lat =  1.0; break;
			case 14: fwd = -0.5; lat = -1.0; break;
			case 15: fwd = -0.5; lat =  1.0; break;
			case 16: fwd = -1.0; lat =  0.0; break;
			case 17: fwd = -2.0; lat =  0.0; break;
			// 2 Линия — бок-о-бок поперёк курса + 1 сзади
			case 21: fwd =  0.0; lat = -1.0; break;
			case 22: fwd =  0.0; lat =  1.0; break;
			case 23: fwd =  0.0; lat = -2.0; break;
			case 24: fwd =  0.0; lat =  2.0; break;
			case 25: fwd =  0.0; lat = -3.0; break;
			case 26: fwd =  0.0; lat =  3.0; break;
			case 27: fwd = -1.5; lat =  0.0; break;
			// 3 Щит — флагман в центре, фланги по 3 с бортов + хвост
			case 31: fwd =  1.0; lat = -1.0; break;
			case 32: fwd =  1.0; lat =  1.0; break;
			case 33: fwd =  0.0; lat = -1.0; break;
			case 34: fwd =  0.0; lat =  1.0; break;
			case 35: fwd = -1.0; lat = -1.0; break;
			case 36: fwd = -1.0; lat =  1.0; break;
			case 37: fwd = -2.0; lat =  0.0; break;
			// 4 Зигзаг — 2 РЯДА позади (шахматка): ряд1 линия из 4, ряд2 линия из 3 (смещён между ними)
			case 41: fwd = -1.0; lat = -0.5; break; // ряд1 (ближний), 4 в линию
			case 42: fwd = -1.0; lat =  0.5; break;
			case 43: fwd = -1.0; lat = -1.5; break;
			case 44: fwd = -1.0; lat =  1.5; break;
			case 45: fwd = -2.0; lat =  0.0; break; // ряд2 (дальний), 3 в линию, в шахмат
			case 46: fwd = -2.0; lat = -1.0; break;
			case 47: fwd = -2.0; lat =  1.0; break;
			// 5 Клин — V остриём вперёд, флагман в центре-сзади
			case 51: fwd =  1.0; lat =  0.0; break;
			case 52: fwd =  0.3; lat = -0.9; break;
			case 53: fwd =  0.3; lat =  0.9; break;
			case 54: fwd = -0.4; lat = -1.8; break;
			case 55: fwd = -0.4; lat =  1.8; break;
			case 56: fwd = -1.1; lat = -2.7; break;
			case 57: fwd = -1.1; lat =  2.7; break;
		}
	}
	if (iAxis == 0) return fwd;
	return lat;
}

// LEO: выбранная в настройках формация флота ГГ (0..5, дефолт 0=Коробка).
int Sea_GetFleetFormation()
{
	int iForm = 0;
	if (CheckAttribute(&InterfaceStates, "FleetFormation")) iForm = sti(InterfaceStates.FleetFormation);
	if (iForm < 0) iForm = 0;
	if (iForm > 5) iForm = 0;
	return iForm;
}

// LEO: множитель интервала строя по классу/размеру корабля (крупный -> шире строй, чтоб не толкались).
// Класс 1 = крупный (манор), 7 = мелкий; база = класс 4. Тунабл (0.22 — крутизна, клампы).
float Sea_FormSpacingFactor(ref rChar)
{
	int iClass = 4;
	if (CheckAttribute(rChar, "Ship.Type")) iClass = GetCharacterShipClass(rChar);
	float f = 0.7 + (4 - iClass) * 0.2;
	if (f < 0.6) f = 0.6;
	if (f > 1.6) f = 1.6;
	return f;
}

// LEO: переприменить выбранную формацию ВСЕМ компаньонам ГГ. Зовётся при СМЕНЕ формации в опциях —
// в море компаньоны сразу перестроятся (DEFEND читает FormFwd/FormLat каждый кадр). Слот 0 = флагман, свита 1..N.
void Sea_ApplyFleetFormation()
{
	if (!bSeaActive) return; // только в море (в гл.меню/на суше pchar невалиден; на логине формация применится сама)
	int iForm = Sea_GetFleetFormation();
	int iSlot = 0;
	int k;
	int idx;
	float fSp;
	for (k = 1; k < COMPANION_MAX; k++)
	{
		idx = GetCompanionIndex(pchar, k);
		if (idx == -1) continue;
		iSlot++;
		fSp = 200.0 * Sea_FormSpacingFactor(&Characters[idx]);
		Characters[idx].Ship.SeaAI.FormFwd = Sea_FormationOffset(iForm, iSlot, 0) * fSp;
		Characters[idx].Ship.SeaAI.FormLat = Sea_FormationOffset(iForm, iSlot, 1) * fSp;
	}
}

#event_handler("CalculateGroupShipPos", "CalculateGroupShipPos")
ref CalculateGroupShipPos()
{
	float distanceBetweenShips = 200.0;//TO DO сделать расстояние между кораблей от их базовых размеров??? или регулируемым в настройках?
	int shipIndex = GetEventData();
	float centerPosX = GetEventData();
	float rotation = GetEventData();
	float centerPosZ = GetEventData();
	aref aShipChar = GetEventData();
	int shipCount;
	if (IsCompanion(aShipChar)) shipCount = GetCompanionQuantity(pchar); 
						else shipCount = Group_GetLiveCharactersNum(aShipChar.SeaAI.Group.Name);//костыль - для ГГ Group_GetLiveCharactersNum не работает
	float result[3];
	result[0] = centerPosX;
	result[1] = rotation;
	result[2] = centerPosZ;
	float offset_in_line, offset_sign;

	// LEO: выбранная формация для ВСЕГО флота ГГ (флагман+свита) по членству в PLAYER_GROUP —
	// надёжнее IsCompanion (тот на спавне мог не сработать -> падал в старую колонную логику). Разворот
	// группы (Do180Turn) и эскадры — пока по старой колонной логике ниже.
	bool bFleetForm = false;
	if (CheckAttribute(aShipChar, "SeaAI.Group.Name"))
	{
		if (aShipChar.SeaAI.Group.Name == PLAYER_GROUP)
		{
			bool bDo180 = false;
			if (CheckAttribute(aShipChar,"Do180Turn")) { if (aShipChar.Do180Turn == true) bDo180 = true; }
			if (!bDo180) bFleetForm = true;
		}
	}

	// LEO: РАНДОМ-формация ВСЕМ прочим эскадрам (не игрока, ≤9 кораблей, не разворот). Назначается ОДИН
	// раз на группу и хранится (rGrp.RndFormation), чтобы вся эскадра встала в ОДНУ формацию. Крупные (>9, напр.
	// Золотой Флот) и развороты — по старой колонной логике ниже.
	bool bSquadForm = false;
	int iSquadForm = 0;
	string sGrpN = "";
	if (CheckAttribute(aShipChar, "SeaAI.Group.Name")) sGrpN = aShipChar.SeaAI.Group.Name;
	bool bDo180s = false;
	if (CheckAttribute(aShipChar,"Do180Turn")) { if (aShipChar.Do180Turn == true) bDo180s = true; }
	// LEO: ПИРАТАМ формацию НЕ давать — идут вразнобой (окружение сделаем позже, после общей тактики). Пока = старая колонная логика.
	bool bSquadPirate = false;
	if (CheckAttribute(aShipChar, "nation")) { if (sti(aShipChar.nation) == PIRATE) bSquadPirate = true; }
	if (!bFleetForm && !bDo180s && sGrpN != "" && !bSquadPirate)
	{
		if (sGrpN != PLAYER_GROUP && shipCount >= 1 && shipCount <= 9)
		{
			ref rGrpF = Group_FindOrCreateGroup(sGrpN);
			if (CheckAttribute(rGrpF, "RndFormation"))
			{
				iSquadForm = sti(rGrpF.RndFormation); // уже выбрано в этой сессии
			}
			else
			{
				// LEO: формация ПЕР-ЭНКАУНТЕР. Значение катится в ГЕНЕРАЦИИ (GenerateMapEncounter -> персистентный
				//      MapEncounters, до входа) и ШТАМПУЕТСЯ на командира при загрузке (sea.c/DirSailEnc). Тут просто читаем
				//      его с командира -> при перезаходе форма ТА ЖЕ, не сейв-скамится. else = фолбэк (напр. квест-ALONE без
				//      генерации): катим rand ОДИН раз на командира (он персистентен для квестовых).
				int iCmdIdx = Group_GetGroupCommanderIndex(sGrpN);
				if (iCmdIdx >= 0)
				{
					ref rCmdF = GetCharacter(iCmdIdx);
					if (CheckAttribute(rCmdF, "SeaAI.RndFormation")) iSquadForm = sti(rCmdF.SeaAI.RndFormation);
					else { iSquadForm = rand(5); rCmdF.SeaAI.RndFormation = iSquadForm; } // фолбэк 0..5
				}
				else
				{
					iSquadForm = rand(5); // командира ещё нет — крайний случай
				}
				rGrpF.RndFormation = iSquadForm; // кэш на текущую сессию боя
			}
			bSquadForm = true;
		}
	}

	if (bFleetForm)
	{
		int iFleetForm = Sea_GetFleetFormation();
		float fFormSp = Sea_FormSpacingFactor(aShipChar); // интервал с учётом размера/класса
		offset_in_line = -1.0 * Sea_FormationOffset(iFleetForm, shipIndex, 0) * fFormSp; // fwd>0 вперёд -> назад
		offset_sign    = -1.0 * Sea_FormationOffset(iFleetForm, shipIndex, 1) * fFormSp; // lat>0 вправо
	}
	else if (bSquadForm)
	{
		float fSqSp = Sea_FormSpacingFactor(aShipChar);
		offset_in_line = -1.0 * Sea_FormationOffset(iSquadForm, shipIndex, 0) * fSqSp;
		offset_sign    = -1.0 * Sea_FormationOffset(iSquadForm, shipIndex, 1) * fSqSp;
	}
	else if (shipcount < 4)	//не строить в две колонны, если меньше 4 кораблей
	{
		offset_sign = 0;
		offset_in_line = shipIndex;
		if (CheckAttribute(aShipChar,"Do180Turn") && aShipChar.Do180Turn == true) 
		{
			if (shipIndex == 0) Group_SetDo180Turn(aShipChar.SeaAI.Group.Name); 
			result[1] = rotation-PI;//радианы
			aShipChar.Do180Turn = false;
			offset_in_line = shipCount - 1 - offset_in_line;
		}
	}
	else
	{
		if (shipcount > 9)	//делаем три колонны - Золотой Флот
		{
			//при тесте ЗФ спавнится в обратном порядке - в конце три сан-фелиппе, дальше алексисы, впереди галеоны		И флагман не выдвигается вперёд. строятся прямоугольником
			offset_in_line = makeint((shipIndex + 2) / 3);
			int iTemp = shipIndex % 3; 
			switch (iTemp)
			{
				case 0: offset_sign = 0; break;
				case 1: offset_sign = -1; break;
				case 2: offset_sign = 1; break;
			}
//		2	5	8	11
//	1	4	7	10	13
//		3	6	9	12
		}
		else
		{//стандарт - от 4 до 9 кораблей - в две колонны
			offset_sign = -0.433;//чётный
			if ((shipIndex % 2) == 0) offset_sign = 0.433;//нечётный
			offset_in_line = makeint((shipIndex + 1) / 2);
			if (shipIndex == 0) {offset_sign = 0; offset_in_line = 0;} //флагман в точку
		
			if (CheckAttribute(aShipChar,"Do180Turn") && aShipChar.Do180Turn == true) 
			{
				if (shipIndex == 0) Group_SetDo180Turn(aShipChar.SeaAI.Group.Name); 
				result[1] = rotation-PI;//радианы
				aShipChar.Do180Turn = false;
				offset_in_line = makeint((shipCount - shipIndex)/2);
				if (shipIndex == 0) offset_in_line -= 0.134;
			}
			else 
			{
				if (shipcount == (shipIndex+1) && (shipIndex % 2) == 1) { offset_in_line -= 0.134; offset_sign = 0; }//последний _И_ чётный	
			}
			if (offset_in_line > 0) offset_in_line -= 0.134;
		}
	}
	result[0] -= distanceBetweenShips * (offset_in_line * sin(rotation) + offset_sign * cos(rotation));
	result[2] -= distanceBetweenShips * (offset_in_line * cos(rotation) - offset_sign * sin(rotation));
	return &result;
}

void DeleteShipMastDamage()
{
	ref chr, rShip;
	for(int i=0;i<MAX_CHARACTERS;i++)
	{
		makeref(chr, Characters[i]);
		if (CheckAttribute(chr,"Ship.Type") && sti(chr.Ship.Type) != SHIP_NOTUSED) 
		{	
			rShip = GetRealShip(sti(chr.Ship.Type));
			DeleteAttribute(rShip,"mastHP");
			DeleteAttribute(rShip,"mastHPrep");
		}
	}
}
