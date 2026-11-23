int g_LocLngFileID = -1;

object objLandInterface;
object objFastReloadTable;
object objTownStateTable;
int g_intRetVal;

bool bDisableFastReload = false;

#define	BLI_UPDATE_PERIOD 400

#define	LAND_ENEMY_BARS_MAX 32

#define	LAND_ENEMY_BARS_SCALE 4.0

#define	LAND_ENEMY_BARS_DIST 20.0

#define	LAND_ENEMY_BARS_FADE 0.8
#define	LAND_ENEMY_BARS_TEX "LocEfx\state_bars.tga"
#define	LAND_ENEMY_BARS_FILL_START 0.0 // LEO: Границы заливки - доли ширины текстуры; 0..1 заполняет её целиком.
#define	LAND_ENEMY_BARS_FILL_END   1.0
#define	LAND_ENEMY_BARS_GHOST 1
#define	LAND_ENEMY_BARS_GHOST_SPEED 7.0
#define	LAND_ENEMY_BARS_GHOST_HOLD 0.12  // LEO: Задержка хвоста урона, секунды.

#define	LAND_EFXICON_TEX "BATTLE_INTERFACE\WeaponEffects.dds"
#define	LAND_EFXICON_PERROW 4       // LEO: Число клеток в строке атласа; клетки 64x64.
#define	LAND_EFXICON_SIZE 1.2       // LEO: Размер в долях высоты бара: 0.1..6.
#define	LAND_EFXICON_GAP 0.15       // LEO: Зазор в долях размера иконки: 0..4.
#define	LAND_EFXICON_RISE 0.35      // LEO: Подъём в долях высоты бара: -4..8.
#define	LAND_EFXICON_TIMER 1.0      // LEO: Масштаб таймера: 0..4; 0 скрывает таймер.

#define	LAND_DMGTEXT_SCALE 1.0

#define	LAND_DMGTEXT_DIST 30.0

#define	LAND_DMGTEXT_RISE 0.3

#define	LAND_DMGTEXT_FADE 0.4

#define	LAND_RADAR_SHAPE 0     // 0 = квадрат, 1 = круг
#define	LAND_RADAR_ROTATE 1    // 1 = вращать карту по компасу (направлению ГГ), 0 = север вверху
#define	LAND_RADAR_INVERT 0    // 1 = инвертировать направление вращения (если карта крутится зеркально)
#define	LAND_RADAR_RADIUS 210.0 // радиус/полуразмер радара, пиксели
#define	LAND_RADAR_RANGE 60.0   // мировая дальность охвата (в радиус), метры
#define	LAND_RADAR_MARGIN 24.0  // отступ от правого/нижнего края экрана, пиксели
#define	RADARCAT_CITIZEN 0
#define	RADARCAT_PATROL  1
#define	RADARCAT_MUSKET  2
#define	RADARCAT_ALLY    3
#define	RADARCAT_SKELETON 4
#define	RADARCAT_QUEST    5
#define	RADARCAT_COMBAT_ALLY  6
#define	RADARCAT_COMBAT_ENEMY 7
#define	LAND_RADAR_INSIDE_CLIP 2.5 // LEO: Отсечение потолка над ногами ГГ, метры.
#define	LAND_RADAR_RANGE_INSIDE 10.0 // LEO: Дальность радара в интерьерах, метры.
#define	LAND_RADAR_MAP_RES 2048    // LEO: Разрешение карты: 256..4096; увеличение требует больше ресурсов.
#define	LAND_RADAR_CATMASK_DEF 511 // LEO: Биты 0..7 - категории персонажей, бит 8 - мёртвые.
#define	LAND_RADAR_POIMASK_DEF 7   // LEO: Биты 0..2 - предметы, сундуки, травы.
#define	LAND_RADAR_BLIP_DEF 1.0    // LEO: Масштаб отметок: 0.2..5.0.

#define	LAND_RADAR_TEX_ITEM  ""
#define	LAND_RADAR_TEX_CHEST "INTERFACES\BoxImage.dds"
#define	LAND_RADAR_TEX_ALLY  ""
#define	LAND_RADAR_TEX_DOORS "" // LEO: Двери рисуются примитивами: кольцом или крестиком.
#define	LAND_RADAR_TEX_FRAME_CIRCLE "INTERFACES\Radar_Circle.dds"
#define	LAND_RADAR_TEX_FRAME_SQUARE "INTERFACES\Radar_Qoob.dds"
#define	LAND_RADAR_FRAME_SCALE 1.05                                // LEO: Полуразмер рамки равен радиусу, умноженному на масштаб.
#event_handler("evntBattleCommandSound","procBattleCommandSound");

bool bLandInterfaceStart = false;
bool dead = false;

void procBattleCommandSound()
{
	string comName = GetEventData();
	if(comName=="deactivate")
	{
		PlaySound("interface\ok.wav");
	}
	if(comName=="activate")
	{
		PlaySound("interface\ok.wav");
	}
	if(comName=="right" || comName=="left")
	{
		PlaySound("interface\ok.wav");
	}
}

void InitBattleLandInterface()
{
	if( IsEntity(&objLandInterface) ) {
		EndBattleLandInterface();
	}
	bLandInterfaceStart = false;
	SetEventHandler(EVENT_LOCATION_LOAD,"StartBattleLandInterface",0);
	SetEventHandler(EVENT_LOCATION_LOAD,"LandRadar_OnLocationLoad",0); // LEO: Настройки радара обновляем на каждой локации, независимо от bLandInterfaceStart.
	SetEventHandler(EVENT_LOCATION_UNLOAD,"EndBattleLandInterface",0);

	SetEventHandler("EventStartQuestMovie","LandRadar_OnQuestMovieStart",0);
	SetEventHandler("EndStartQuestMovie","LandRadar_OnQuestMovieEnd",0);
	LoadLIStates();
}

void BLI_EnableShow()
{
	SetLandInterfaceShow( sti(InterfaceStates.BattleShow.Command) );
	LandRadar_ReconcileVisibility();
}

void BLI_DisableShow()
{
	ResetTimeScale();
	bYesBoardStatus=false;
	DeleteClass(&IBoardingStatus);
	DeleteAttribute(&IBoardingStatus,"");
	SetLandInterfaceShow(false);
	LandRadar_ReconcileVisibility();
}

void SetLandInterfaceShow(bool commandosShow)
{
	SendMessage(&objLandInterface,"ll",MSG_BATTLE_LAND_SET_SHOW,commandosShow);
}

bool LandRadar_IsCharMusketeer(aref chr)
{
	return IsCharacterUsingMusket(chr);
}

// LEO: Тип warrior не отличает стражу от абордажников и тренировочных врагов.
bool LandRadar_IsCharPatrol(aref chr)
{
	if(CheckAttribute(chr, "chr_ai.type"))
	{
		if(chr.chr_ai.type == "patrol") return true;
		if(chr.chr_ai.type == "guardian") return true;
	}
	if(CheckAttribute(chr, "CityType"))
	{
		if(chr.CityType == "soldier") return true;
		if(chr.CityType == "fortPatrol") return true;
	}
	return false;
}

// LEO: Приоритет цветов: именная партия, скелеты, враги по роли, боевые союзники, мирные роли.
int LandRadar_GetCharCat(aref chr)
{
	int idx = sti(chr.index);
	ref rch = GetCharacter(idx);
	if(idx == GetMainCharacterIndex()) return RADARCAT_ALLY;
	if(IsOfficer(rch)) return RADARCAT_ALLY;
	if(IsCompanion(rch)) return RADARCAT_ALLY;

	if(CheckAttribute(chr, "sex"))
	{
		if(chr.sex == "skeleton") return RADARCAT_SKELETON;
	}
	if(LAi_group_IsEnemy(chr, GetMainCharacter()))
	{
		if(LandRadar_IsCharMusketeer(chr)) return RADARCAT_MUSKET;
		if(LandRadar_IsCharPatrol(chr)) return RADARCAT_PATROL;
		return RADARCAT_COMBAT_ENEMY;
	}
	if(LAi_IsBoardingProcess()) return RADARCAT_COMBAT_ALLY;
	if(LAi_group_IsActivePlayerAlarm()) return RADARCAT_COMBAT_ALLY;
	if(CheckAttribute(chr, "quest.questflag.model"))
	{
		if(chr.quest.questflag.model != "") return RADARCAT_QUEST;
	}
	if(LandRadar_IsCharMusketeer(chr)) return RADARCAT_MUSKET;
	if(LandRadar_IsCharPatrol(chr)) return RADARCAT_PATROL;
	return RADARCAT_CITIZEN;
}

void LandRadar_TryAddChest(aref loc, string locID, string boxName)
{
	if(!CheckAttribute(loc, boxName)) return;
	aref boxRef;
	makearef(boxRef, loc.(boxName));
	bool hasContent = false;
	if(GetChrItemQuantity(boxRef) > 0) hasContent = true; // LEO: Считаем атрибуты через makearef, а не чтением значения узла.
	if(!hasContent)
	{
		if(CheckAttribute(boxRef, "money"))
		{
			if(sti(boxRef.money) > 0) hasContent = true;
		}
	}
	if(!hasContent) return;
	aref lr;
	if(!FindLocator(locID, boxName, &lr, true)) return;
	SendMessage(loadedLocation, "llff", MSG_LOCATION_RADARPOI_ADD, 1, makeFloat(lr.x), makeFloat(lr.z));
}

// LEO: Неизвестный непустой тип считаем интерьером; открытые типы перечислены явно.
bool LandRadar_IsInside(aref loc)
{

	if(CheckAttribute(loc, "id"))
	{
		if(HasSubStr(loc.id, "Graveyard")) return false;

		if(loc.id == "BOARDING_FORT") return false;
	}
	if(!CheckAttribute(loc, "type")) return false;
	string t = loc.type;
	if(t == "jungle") return false;
	if(t == "Jungle") return false;
	if(t == "seashore") return false;
	if(t == "town") return false;
	if(t == "port") return false;
	if(t == "mayak") return false;
	if(t == "fort") return false;
	if(t == "questisland") return false;
	if(t == "land") return false;
	if(t == "MountainPath") return false;
	if(t == "underwater") return false;
	if(t == "DeckWithReefs") return false;
	if(t == "deck_fight") return false;
	if(t == "Reefs_Chapter") return false;
	if(t == "LostShipsCity") return false;
	if(t == "arena") return false;
	if(t == "teno") return false;
	return true;
}

void LandRadar_BuildPOI()
{
	int locIdx = FindLocation(pchar.location);
	if(locIdx < 0) return;
	aref loc;
	makearef(loc, Locations[locIdx]);
	if(!CheckAttribute(loc, "id")) return;
	string locID = loc.id;
	SendMessage(loadedLocation, "l", MSG_LOCATION_RADARPOI_CLEAR);
	aref lr;
	string nm;
	int i;
	for(i = 1; i < MAX_LOADED_RANDITEMS; i++)
	{
		nm = "RandItemType" + i;
		if(!CheckAttribute(loc, nm)) continue;
		if(sti(loc.(nm)) == -1) continue;
		if(!FindLocator(locID, "randitem" + i, &lr, true)) continue;
		SendMessage(loadedLocation, "llff", MSG_LOCATION_RADARPOI_ADD, 0, makeFloat(lr.x), makeFloat(lr.z));
	}

	int qi; string slc; int ptype;
	for(qi = 0; qi < ITEMS_QUANTITY; qi++)
	{
		if(!CheckAttribute(Items[qi], "shown")) continue;
		if(Items[qi].shown != "1") continue;
		if(!CheckAttribute(Items[qi], "startLocation")) continue;
		if(Items[qi].startLocation != locID) continue;
		if(!CheckAttribute(Items[qi], "startLocator")) continue;
		slc = Items[qi].startLocator;
		if(slc == "") continue;
		if(HasSubStr(slc, "randitem")) continue;
		if(!FindLocator(locID, slc, &lr, true)) continue;
		ptype = 0;
		if(HasSubStr(slc, "flower")) ptype = 2;
		SendMessage(loadedLocation, "llff", MSG_LOCATION_RADARPOI_ADD, ptype, makeFloat(lr.x), makeFloat(lr.z));
	}
	for(i = 1; i < MAX_HANDLED_BOXES; i++)
	{
		LandRadar_TryAddChest(loc, locID, "box" + i);
		LandRadar_TryAddChest(loc, locID, "private" + i);
	}

	if(CheckAttribute(loc, "reload"))
	{
		aref rlroot;
		makearef(rlroot, loc.reload);
		int rn = GetAttributesNum(rlroot);
		aref re;
		string rname;
		int rt;
		for(i = 0; i < rn; i++)
		{
			re = GetAttributeN(rlroot, i);
			if(!CheckAttribute(re, "name")) continue;
			rname = re.name;
			if(rname == "") continue;
			if(!FindLocator(locID, rname, &lr, true)) continue;
			rt = 3;
			if(chrCheckReload(loc, rname)) rt = 4;
			SendMessage(loadedLocation, "llff", MSG_LOCATION_RADARPOI_ADD, rt, makeFloat(lr.x), makeFloat(lr.z));
		}
	}
}

void StartBattleLandInterface()
{
	if(bLandInterfaceStart)
	{
		return;
	}
	AltIntIcons = sti(InterfaceStates.AltIntIcons) == 1; // LEO: Альтернативные иконки не зависят от выбранной формы интерфейса.
	bLandInterfaceStart = true;

	BLI_SetObjectData();
	BLI_SetMessageParameters();
	CreateEntity(&objLandInterface,"battle_land_interface");
	LayerAddObject(EXECUTE,&objLandInterface,-1);
	LayerAddObject(REALIZE,&objLandInterface,-1);

	SetEventHandler(EVENT_CHANGE_OFFICERS,"BLI_UpdateOfficers",0);
	SetEventHandler(EVENT_DIALOG_START,"BLI_DisableShow",0);
	SetEventHandler(EVENT_DIALOG_EXIT,"BLI_EnableShow",0);

	if(dialogRun || questMovieProcess > 0) BLI_DisableShow();
	else BLI_EnableShow();

	SetEventHandler("evntBLI_Update","BLI_UpdateObjectData",0);
	SetEventHandler("BI_CommandEndChecking","BLI_CheckCommand",0);
	SetEventHandler("BI_LaunchCommand","BLI_ExecuteCommand",0);
	SetEventHandler("BI_SetPossibleCommands","BLI_SetPossibleCommands",0);
	SetEventHandler("evntFindDialogChar","procFindDialogChar",0);
	SetEventHandler("Location_CharacterEntryToLocator", "BLI_ChrEnterToLocation", 0);
	SetEventHandler("Location_CharacterExitFromLocator", "BLI_ChrExitFromLocation", 0);
	SetEventHandler("evntPerkAgainUsable","BLI_RefreshCommandMenu",0);
	SetEventHandler("Control Activation","LI_ProcessControlPress",0);

	Event("evntBLI_Update");
	Event("evntFindDialogChar");

	if(FindLoadedLocation() < 0) return;

	int bLandBars = 1;
	if (CheckAttribute(&InterfaceStates, "CharCapLandBars")) bLandBars = sti(InterfaceStates.CharCapLandBars);
	SendMessage(loadedLocation, "ll", MSG_LOCATION_VIEWSTATEBARS, bLandBars);
	SendMessage(loadedLocation, "ls", MSG_LOCATION_SETBARSTEX, LAND_ENEMY_BARS_TEX);
	SendMessage(loadedLocation, "ll", MSG_LOCATION_SETBARSLIMIT, GetEnemyBarMax());
	SendMessage(loadedLocation, "lfff", MSG_LOCATION_SETBARSPARAMS, GetEnemyBarSize(), GetEnemyBarDist(), LAND_ENEMY_BARS_FADE);
	SendMessage(loadedLocation, "lff", MSG_LOCATION_SETBARSFILL, LAND_ENEMY_BARS_FILL_START, LAND_ENEMY_BARS_FILL_END);
	int landGhostCol = argb(255, 255, 255, 255);
	SendMessage(loadedLocation, "llffl", MSG_LOCATION_SETBARSGHOST, LAND_ENEMY_BARS_GHOST, LAND_ENEMY_BARS_GHOST_SPEED, LAND_ENEMY_BARS_GHOST_HOLD, landGhostCol);
	SendMessage(loadedLocation, "lffff", MSG_LOCATION_SETDMGTEXT, LAND_DMGTEXT_SCALE, LAND_DMGTEXT_DIST, LAND_DMGTEXT_RISE, LAND_DMGTEXT_FADE);
	LandEffectIcons_ApplyLive();
}

// LEO: Иконки эффектов независимы от видимости баров; общий у них только масштаб.
void LandEffectIcons_ApplyLive()
{
	if (!bLandInterfaceStart) return;
	int iOn = 1;
	if (CheckAttribute(&InterfaceStates, "EnabledFXMarks")) iOn = sti(InterfaceStates.EnabledFXMarks);
	int colTimer = argb(255, 255, 255, 255);

	string sEffAttrs = "swift,understun,Trauma,poison,blooding";

	if (iOn == 0) sEffAttrs = "";
	SendMessage(loadedLocation, "llsslffffl", MSG_LOCATION_SETEFFICONS, iOn, LAND_EFXICON_TEX, sEffAttrs, LAND_EFXICON_PERROW, LAND_EFXICON_SIZE, LAND_EFXICON_GAP, LAND_EFXICON_RISE, LAND_EFXICON_TIMER, colTimer);
}

// LEO: Палитра радара; argb() нельзя использовать в #define Storm.
void LandRadar_SendColors()
{
	int colCitizen  = argb(255, 255, 255, 255); // горожанин — белый
	int colPatrol   = argb(255, 255, 128, 0);   // патруль/солдат — оранжевый
	int colMusket   = argb(255, 255, 32, 32);   // мушкетёр — красный
	int colAlly     = argb(255, 48, 224, 48);   // ГГ/союзник — зелёный
	int colSkeleton = argb(255, 255, 102, 204); // скелет — розовый
	int colQuest    = argb(255, 180, 76, 240);  // квестмарка — фиолетовый
	int colDead     = argb(255, 144, 144, 144); // мёртвый ОБЛУТАННЫЙ — серый
	int colDeadFresh = argb(255, 255, 255, 255);// мёртвый НЕ облутанный — белый
	int colItem     = argb(255, 255, 224, 64);  // предмет — жёлтый (фолбэк-примитив)
	int colChest    = argb(255, 200, 134, 30);  // сундук — золотой (фолбэк-примитив)
	int colFrame    = argb(96, 32, 40, 48);     // подложка рамки (фолбэк, если карта не построена)
	int colCombatAlly  = argb(255, 90, 170, 255); // абордаж/форт: свой — светло-синий
	int colCombatEnemy = argb(255, 255, 110, 110);// абордаж/форт: враг — светло-красный
	int colHerb        = argb(255, 102, 255, 51); // травка (flower-локаторы) — ярко-зелёный
	SendMessage(loadedLocation, "lllllllllllllll", MSG_LOCATION_SETRADARCOLORS, colCitizen, colPatrol, colMusket, colAlly, colSkeleton, colQuest, colDead, colDeadFresh, colItem, colChest, colFrame, colCombatAlly, colCombatEnemy, colHerb);
}

// LEO: Фильтры по умолчанию включены; маску собираем арифметикой - в Storm нет битовых операций.
int LandRadar_GetShow(string name)
{
	string a = "RadarShow" + name;
	if (!CheckAttribute(&InterfaceStates, a)) return 1;
	return sti(InterfaceStates.(a));
}

// LEO: Веса категорий совпадают с битами движка: персонажи 1..256, предметы/сундуки/травы 1/2/4.
void LandRadar_SendFilter()
{
	if (!bLandInterfaceStart) return; // LEO: В главном меню и море loadedLocation не инициализирована; проверяем bLandInterfaceStart.
	int cm = 0;
	if (LandRadar_GetShow("Citizen"))     cm = cm + 1;
	if (LandRadar_GetShow("Patrol"))      cm = cm + 2;
	if (LandRadar_GetShow("Musket"))      cm = cm + 4;
	if (LandRadar_GetShow("Ally"))        cm = cm + 8;
	if (LandRadar_GetShow("Skeleton"))    cm = cm + 16;
	if (LandRadar_GetShow("Quest"))       cm = cm + 32;
	if (LandRadar_GetShow("CombatAlly"))  cm = cm + 64;
	if (LandRadar_GetShow("CombatEnemy")) cm = cm + 128;
	if (LandRadar_GetShow("Dead"))        cm = cm + 256;
	int pm = 0;
	if (LandRadar_GetShow("Item"))  pm = pm + 1;
	if (LandRadar_GetShow("Chest")) pm = pm + 2;
	if (LandRadar_GetShow("Herb"))  pm = pm + 4;
	if (LandRadar_GetShow("Doors")) pm = pm + 8;
	if (!CheckAttribute(&InterfaceStates, "RadarBlipScale")) InterfaceStates.RadarBlipScale = LAND_RADAR_BLIP_DEF;
	SendMessage(loadedLocation, "lllf", MSG_LOCATION_SETRADARFILTER, cm, pm, stf(InterfaceStates.RadarBlipScale));
}

void LandRadar_SendParams()
{
	if (!bLandInterfaceStart) return;
	int sh = LAND_RADAR_SHAPE;  if (CheckAttribute(&InterfaceStates, "RadarShape"))  sh = sti(InterfaceStates.RadarShape);
	int ro = LAND_RADAR_ROTATE; if (CheckAttribute(&InterfaceStates, "RadarRotate")) ro = sti(InterfaceStates.RadarRotate);
	float rd = LAND_RADAR_RADIUS; if (CheckAttribute(&InterfaceStates, "RadarRadius")) rd = stf(InterfaceStates.RadarRadius);
	SendMessage(loadedLocation, "llllfff", MSG_LOCATION_SETRADARPARAMS, sh, ro, LAND_RADAR_INVERT, rd, stf(InterfaceStates.RadarRange), LAND_RADAR_MARGIN);
	loadedLocation.HUDLayoutRadarX=HUDLayout_GetOffset("land","radar","x");
	loadedLocation.HUDLayoutRadarY=HUDLayout_GetOffset("land","radar","y");
}

// LEO: Смена разрешения требует повторного рендера карты.
void LandRadar_ApplyQuality()
{
	if (!bLandInterfaceStart) return;
	int res = LAND_RADAR_MAP_RES; if (CheckAttribute(&InterfaceStates, "RadarMapRes")) res = sti(InterfaceStates.RadarMapRes);
	SendMessage(loadedLocation, "ll", MSG_LOCATION_SETRADARQUALITY, res);
}

void LandBars_ApplyLive()
{
	if (!bLandInterfaceStart) return;
	int v = 1;
	if (CheckAttribute(&InterfaceStates, "CharCapLandBars")) v = sti(InterfaceStates.CharCapLandBars);
	SendMessage(loadedLocation, "ll", MSG_LOCATION_VIEWSTATEBARS, v);
}

void LandBars_ApplyParamsLive()
{
	if (!bLandInterfaceStart) return;
	SendMessage(loadedLocation, "ll", MSG_LOCATION_SETBARSLIMIT, GetEnemyBarMax());
	SendMessage(loadedLocation, "lfff", MSG_LOCATION_SETBARSPARAMS, GetEnemyBarSize(), GetEnemyBarDist(), LAND_ENEMY_BARS_FADE);
}

void LandRadar_SendInside()
{
	if (!bLandInterfaceStart) return;
	int li = FindLocation(pchar.location);
	int bIn = 0;
	if (li >= 0)
	{
		aref l;
		makearef(l, Locations[li]);
		if (LandRadar_IsInside(l)) bIn = 1;
	}
	SendMessage(loadedLocation, "llff", MSG_LOCATION_SETRADARINSIDE, bIn, LAND_RADAR_INSIDE_CLIP, stf(InterfaceStates.RadarRangeInside));
}

bool LandRadar_ShouldHide()
{
    if (CheckAttribute(GetMainCharacter(), "Throwing3DActive")) return true;
	if (dialogRun) return true;

	if (questMovieProcess > 0) return true;
	if (CheckAttribute(pchar, "chr_ai.type") && pchar.chr_ai.type != "player") return true;
	if (LocCamera_GetCurState() != -1) return true;
	if (CheckAttribute(&InterfaceStates, "Launched"))
	{
		if (sti(InterfaceStates.Launched) != 0) return true;
	}
	return false;
}

// LEO: Отправляем видимость только при изменении: радар включён, показан горячей клавишей и не скрыт сценой.
void LandRadar_ReconcileVisibility()
{
	if (!bLandInterfaceStart) return;

	if (!CheckAttribute(&InterfaceStates, "RadarEnabled")) return;
	if (!CheckAttribute(&InterfaceStates, "RadarVisible")) return;
	int desired = 0;
	if (sti(InterfaceStates.RadarEnabled) == 1)
	{
		if (sti(InterfaceStates.RadarVisible) == 1)
		{
			if (!LandRadar_ShouldHide()) desired = 1;
		}
	}
	int shown = -1;
	if (CheckAttribute(&InterfaceStates, "RadarShownNow")) shown = sti(InterfaceStates.RadarShownNow);
	if (desired != shown)
	{
		SendMessage(loadedLocation, "ll", MSG_LOCATION_SETRADARON, desired);
		InterfaceStates.RadarShownNow = desired;
	}
}

void LandRadar_OnQuestMovieStart()
{
	if (!bLandInterfaceStart) return;
	SendMessage(loadedLocation, "ll", MSG_LOCATION_SETRADARON, 0);
	InterfaceStates.RadarShownNow = 0;
}

// LEO: После конца сцены ждём кадр: в момент события questMovieProcess ещё не уменьшен.
void LandRadar_OnQuestMovieEnd()
{
	SetEventHandler("frame","LandRadar_ReconcileByFrameEvent",0);
}

void LandRadar_ReconcileByFrameEvent()
{
	DelEventHandler("frame","LandRadar_ReconcileByFrameEvent");
	LandRadar_ReconcileVisibility();
}

void LandRadar_OnLocationLoad()
{
	if (!CheckAttribute(&InterfaceStates, "RadarEnabled")) InterfaceStates.RadarEnabled = 0;
	if (!CheckAttribute(&InterfaceStates, "RadarVisible")) InterfaceStates.RadarVisible = 0;
	if (!CheckAttribute(&InterfaceStates, "RadarRange")) InterfaceStates.RadarRange = LAND_RADAR_RANGE;
	if (!CheckAttribute(&InterfaceStates, "RadarRangeInside")) InterfaceStates.RadarRangeInside = LAND_RADAR_RANGE_INSIDE;
	SendMessage(loadedLocation, "lssss", MSG_LOCATION_SETRADARTEX, LAND_RADAR_TEX_ITEM, LAND_RADAR_TEX_CHEST, LAND_RADAR_TEX_ALLY, LAND_RADAR_TEX_DOORS);
	SendMessage(loadedLocation, "lssf", MSG_LOCATION_SETRADARFRAMETEX, LAND_RADAR_TEX_FRAME_CIRCLE, LAND_RADAR_TEX_FRAME_SQUARE, LAND_RADAR_FRAME_SCALE);
	LandRadar_ApplyQuality();
	LandRadar_SendColors();
	LandRadar_SendInside();
	LandRadar_SendParams();
	LandRadar_SendFilter();
	int radarEff = 0;
	if (sti(InterfaceStates.RadarEnabled) == 1 && sti(InterfaceStates.RadarVisible) == 1) radarEff = 1;
	SendMessage(loadedLocation, "ll", MSG_LOCATION_SETRADARON, radarEff);
	InterfaceStates.RadarShownNow = radarEff; // LEO: Согласуем кэш видимости с LandRadar_ReconcileVisibility.
	LandRadar_BuildPOI();
}

void LandRadar_AdjustRange(float mult)
{
	if (!bLandInterfaceStart) return;
	int locIdx = FindLocation(pchar.location);
	if (locIdx < 0) return;
	aref loc;
	makearef(loc, Locations[locIdx]);
	if (LandRadar_IsInside(loc))
	{
		float ri = LAND_RADAR_RANGE_INSIDE;
		if (CheckAttribute(&InterfaceStates, "RadarRangeInside")) ri = stf(InterfaceStates.RadarRangeInside);
		ri = ri * mult;
		if (ri < 5.0) ri = 5.0;
		if (ri > 120.0) ri = 120.0;
		InterfaceStates.RadarRangeInside = ri;
	}
	else
	{
		float ro = LAND_RADAR_RANGE;
		if (CheckAttribute(&InterfaceStates, "RadarRange")) ro = stf(InterfaceStates.RadarRange);
		ro = ro * mult;
		if (ro < 20.0) ro = 20.0;
		if (ro > 300.0) ro = 300.0;
		InterfaceStates.RadarRange = ro;
	}
	LandRadar_OnLocationLoad();
}

void BLI_SetFlags()
{
	int init = 0;
	if (CheckAttribute(&objLandInterface, "DoSetFlags")) init = sti(objLandInterface.DoSetFlags);
	trace("LandInterface DoSetFlags: " + init);
	init++;
	objLandInterface.DoSetFlags = init;
	if (init >= sti(Characters[GetMainCharacterIndex()].numcoastalindocks))
	{
		DeleteAttribute(&objLandInterface, "DoSetFlags");
		DelEventHandler("frame", "BLI_SetFlags");
	}
}
ref BLI_CheckCommand()
{
	string commandName = GetEventData();

	aref uiref;	makearef(uiref,objLandInterface.UserIcons);
	aref ar;
	int nq = GetAttributesNum(uiref);
	int i;

	bool bUsed = false;
	for(i=0; i<nq; i++)
	{
		ar = GetAttributeN(uiref,i);
		ar.enable = false;
	}
	g_intRetVal = BI_COMMODE_USER_ICONS;

	switch(commandName)
	{
		case "cancel":
			g_intRetVal = -1;
		break;
		case "BI_FastReload":
			bUsed = SetReloadIcons();
		break;
		case "BI_ItemsUse":
			bUsed = SetUsedPotionIcons();
		break;
		case "BI_UseFood":
			bool bEnableFood = true;
			if(bEnableFood)
			{
				bUsed = SetUsedFoodIcons();
			}
			else
			{
				g_intRetVal = 0;
			}
		break;
		case "BI_Smoke":
			bUsed = SetUsedSmokeIcons();
		break;
		case "BI_DialogStart":
			g_intRetVal = 0;
		break;
		case "BI_ItemsChange":
			g_intRetVal = 0;
		break;
		case "BI_TakeItem":
			g_intRetVal = 0;
		break;
		case "BI_PlaceItem":
			g_intRetVal = 0;
		break;
		case "BI_OutDoor":
			g_intRetVal = 0;
		break;
		case "BI_UseBox":
			g_intRetVal = 0;
		break;
		case "BI_DeadBox":
			g_intRetVal = 0;
		break;
		case "BI_TalkSelf":
			g_intRetVal = 0;
		break;
		case "BI_Craft":
			g_intRetVal = 0;
		break;
		case "BI_ActivateRush":
			g_intRetVal = 0;
		break;
		case "BI_Exit_Deck":
			g_intRetVal = 0;
		break;
	}
	return &g_intRetVal;
}

void BLI_ExecuteCommand()
{
	int		chrIdx = GetEventData();
	string	commName = GetEventData();
	int		targChrIdx = GetEventData();
	string	evntName = GetEventData();

	aref arFader;
	if( GetEntity(arFader,"fader") ) {return;}

	aref	uiref;
	int		curLocIdx;
	int		tmpi;

	if( commName=="cancel" || evntName=="cancel" ) {
		SendMessage(&objLandInterface,"ls",MSG_BATTLE_LAND_MAKE_COMMAND,"cancel");
		return;
	}

	switch(commName)
	{
	case "BI_FastReload":
		{
			curLocIdx = FindLoadedLocation();
			if(curLocIdx==-1) return;
			if( FindUserIcon(evntName,&uiref) )
			{
				string sTravelLocator = "";
				if(CheckAttribute(uiref,"locator")) {sTravelLocator=uiref.locator;}
				if(CheckAttribute(uiref,"location"))
				{
					if(CheckAttribute(loadedLocation,"islandId"))
					{
						if (loadedLocation.islandId == "Caiman")
						{
							DoQuestReloadToLocation(uiref.location, "reload", "reload1", "");
						}
						else PlayerFastTravel(curLocIdx, uiref.location, sTravelLocator);
					}

					else PlayerFastTravel(curLocIdx, uiref.location, sTravelLocator);
				}
			}
		}
	break;
	case "BI_ItemsUse":
		if( FindUserIcon(evntName,&uiref) )
		{
			if(CheckAttribute(uiref,"potion"))
			{
				if(CheckAttribute(PChar, "Arena.Mode"))
				{
					if (PChar.Arena.Mode == "Duel" || PChar.Arena.Mode == "Tournament") break;
				}
				aref arItm;
				if( Items_FindItem(uiref.potion,&arItm) >= 0 )
				{
					if( CheckAttribute(arItm,"potion") )
					{
						DoCharacterUsedItem(pchar,arItm.id);
					}
				}
			}
		}
	break;
	case "BI_UseFood":
		if( FindUserIcon(evntName,&uiref) )
		{
			if(CheckAttribute(uiref,"Food"))
			{
				if( Items_FindItem(uiref.Food,&arItm) >= 0 )
				{
					if( CheckAttribute(arItm,"Food") )
					{
						DoCharacterUsedFood(pchar,arItm.id);
					}
				}
			}
		}
	break;
	case "BI_Smoke":
		if( FindUserIcon(evntName,&uiref) )
		{
			if(CheckAttribute(uiref,"Smoke"))
			{
				if( Items_FindItem(uiref.Smoke,&arItm) >= 0 )
				{
					if( CheckAttribute(arItm,"Smoke") )
					{
						DoCharacterUsedSmoke(pchar,arItm.id);
					}
				}
			}
		}
	break;
	case "BI_DialogStart":
		tmpi = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"FindDialogCharacter");
		if(tmpi>=0)	Event("dlgReady","l",tmpi);
	break;
	case "BI_ItemsChange":
		tmpi = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"FindDialogCharacter");

		bool bItemDone = false;
		if(tmpi>=0)
		{
			if(isOfficerInShip(GetCharacter(tmpi), true)) { LaunchCharacterItemChange(GetCharacter(tmpi)); bItemDone = true; }
			else if(CheckAttribute(GetCharacter(tmpi), "IsCompanionClone")) { LaunchCharacterItemChange(GetCharacter(tmpi)); bItemDone = true; }
		}
		if(!bItemDone)
		{

			tmpi = LAi_FindNearestVisCharacter(GetMainCharacter(), 1);
			if(tmpi>0) { if(IsOfficer(GetCharacter(tmpi)) && IsCharacterPerkOn(pchar,"SharedExperience")) LaunchCharacterItemChange(GetCharacter(tmpi)); }
		}
	break;
	case "BI_TakeItem":
		Item_OnPickItem();
	break;
	case "BI_PlaceItem":
		Item_OnUseItem();
	break;
	case "BI_OutDoor":
		Event("Control Activation","s","ChrAction");
	break;
	case "BI_UseBox":
		OpenBoxProcedure();
	break;
	case "BI_DeadBox":
		Dead_OpenBoxProcedure();
	break;
	case "BI_TalkSelf":
		DoQuestCheckDelay("TalkSelf_Start", 0.2);
	break;
	case "BI_Craft":
		pchar.questTemp.CraftType = "Blacksmith";
		LaunchCraftAll();
	break;
	case "BI_ActivateRush":
		ActivateCharacterPerk(pchar,"Rush");
	break;
    case "BI_Exit_Deck":
        if (isShipInside(pchar.location) || CheckShipSituation_GenQuest())
        {
            Return2SeaAfterCabin();
        }
        else
        {
            int idxLoadLoc = FindLoadedLocation();
            if (idxLoadLoc != -1)
            {
	            UnloadLocation(&Locations[idxLoadLoc]);
            }
            wdmReloadToSea();
        }
    break;
	}
}

void EndBattleLandInterface()
{
	if(!bLandInterfaceStart) return;
	bLandInterfaceStart = false;

	DeleteAttribute( pchar, "boxname" );

	bYesBoardStatus=false;
	DeleteClass(&IBoardingStatus);
	DeleteAttribute(&IBoardingStatus,"");

	BLI_DisableShow();
	SendMessage(&objLandInterface,"l",MSG_BATTLE_LAND_END);

	DeleteClass(&objLandInterface);

	DelEventHandler(EVENT_CHANGE_OFFICERS,"BLI_UpdateOfficers");
	DelEventHandler(EVENT_DIALOG_START,"BLI_DisableShow");
	DelEventHandler(EVENT_DIALOG_EXIT,"BLI_EnableShow");
	DelEventHandler("evntBLI_Update","BLI_UpdateObjectData");
	DelEventHandler("BI_CommandEndChecking","BLI_CheckCommand");
	DelEventHandler("BI_LaunchCommand","BLI_ExecuteCommand");
	DelEventHandler("BI_SetPossibleCommands","BLI_SetPossibleCommands");
	DelEventHandler("evntFindDialogChar","procFindDialogChar");
	DelEventHandler("Location_CharacterEntryToLocator", "BLI_ChrEnterToLocation");
	DelEventHandler("Location_CharacterExitFromLocator", "BLI_ChrExitFromLocation");
	DelEventHandler("evntPerkAgainUsable","BLI_RefreshCommandMenu");
	DelEventHandler("Control Activation","LI_ProcessControlPress");

    if(CheckAttribute(&BattleInterface, "LAi_ActivateReload")) {
        Log_SetActiveAction("Reload");
    }
    else {
        Log_SetActiveAction("Nothing");
    }
}

// LEO: Боеприпасы в LIST_ICON2: сетка 8x8, четвёртый ряд - клетки 24..31.
int Cons_AmmoPicNum(string sAmmo)
{
	if(sAmmo == "grapeshot") return 25;
	if(sAmmo == "cartridge") return 26;
	if(sAmmo == "GunEchin") return 27;
	if(sAmmo == "powder_pellet") return 28;
	if(sAmmo == "grenade") return 29;
	if(sAmmo == "harpoon") return 30;
	if(sAmmo == "shotgun_cartridge") return 31;
	return 24;
}

// LEO: Расходники оружия в LIST_ICON2: порох - 16, капсюли - 17, гильзы 12 калибра - 21.
int Cons_PowderPicNum(string sGunpowder)
{
	if(sGunpowder == "GunCap_colt") return 17;
	if(sGunpowder == "12_gauge")    return 21;
	return 16;
}
string Cons_PowderItem(string sGunpowder)
{
	if(sGunpowder == "GunCap_colt") return "GunCap_colt";
	if(sGunpowder == "12_gauge")    return "12_gauge";
	return "GunPowder";
}

string Cons_ItemId(int index)
{
	string ammo;
	switch(index)
	{
	case 0:
		ammo = LAi_GetCharacterBulletType(pchar);
		if(ammo == "") ammo = "bullet";
		return ammo;
	break;
	case 1: return Cons_PowderItem(LAi_GetCharacterGunpowderType(pchar)); break;
	case 2: return "potion1"; break;
	case 3: return "potion2"; break;
	case 4: return "potion3"; break;
	case 5: return "potion4"; break;
	case 6: return "potionrum"; break;
	case 7: return "potionwine"; break;
	case 13: return "StrengthTube"; break;
	case 14: return "EnergyTube"; break;
	case 15: return "HealthTube"; break;
	}
	return "Food"+(index-7);
}

// LEO: Проверка раз в BLI_UPDATE_PERIOD; скрытые счётчики не создаём заново.
int BLI_UpdateConsumablesCounts()
{
	int mask = 0;
	int bit = 1;
	int count;
	string node;
	bool enabled;
	for(int i=0; i<16; i++)
	{
		if(i<8)
		{
			enabled = sti(InterfaceStates.ConsumablesNoTop) == 0;
			node = "konsTc"+i;
		}
		else
		{
			enabled = sti(InterfaceStates.ConsumablesNoBottom) == 0;
			node = "konsBc"+(i-8);
		}
		if(enabled)
		{
			count = GetCharacterItem(pchar, Cons_ItemId(i));
			if(count>0) mask = mask+bit;
			if(CheckAttribute(&objLandInterface,"textinfo."+node))
			{
				if(objLandInterface.textinfo.(node).text != ""+count) objLandInterface.textinfo.(node).text = ""+count;
			}
		}
		bit = bit*2;
	}
	return mask;
}

// LEO: До 8 значков - один ряд; выше - два сбалансированных, короткий по центру.
// LEO: Смещения HUDLayout относятся к итоговым рядам, категории фильтруются до раскладки.
void BLI_CompactConsumables(int mask, int bx, int by, int tx, int ty, int w, int h, int gap, int countDx, int countH)
{
	int rest = mask;
	int total = 0;
	int i;
	for(i=0; i<16; i++)
	{
		if(rest%2 == 1) total++;
		rest = rest/2;
	}
	int topCount = 0;
	if(total>8) topCount = (total+1)/2;
	int bottomCount = total-topCount;
	if(sti(InterfaceStates.ConsumablesNoBottom) == 1) { bx=tx; by=ty; }
	int slot = 0;
	int x;
	int y;
	string image;
	string text;
	rest = mask;
	for(i=0; i<16; i++)
	{
		if(i<8) { image="konsT"+i; text="konsTc"+i; }
		else { image="konsB"+(i-8); text="konsBc"+(i-8); }
		if(rest%2 == 0)
		{
			DeleteAttribute(&objLandInterface,"imageslist."+image);
			DeleteAttribute(&objLandInterface,"textinfo."+text);
		}
		else
		{
			if(slot<topCount)
			{
				x = tx+(8-topCount)*(w+gap)/2+slot*(w+gap);
				y = ty;
			}
			else
			{
				x = bx+(8-bottomCount)*(w+gap)/2+(slot-topCount)*(w+gap);
				y = by;
			}
			objLandInterface.imageslist.(image).pos = ""+x+","+y+","+(x+w)+","+(y+h);
			objLandInterface.textinfo.(text).pos.x = x+countDx;
			objLandInterface.textinfo.(text).pos.y = y+h-countH;
			slot++;
		}
		rest = rest/2;
	}
}

// LEO: После живой пересборки нужен MSG_BATTLE_LAND_REFRESH_HUD для перечитывания imageslist и textinfo.
// LEO: Нижний ряд UseFood.dds (8x2): фрукты, хлеб, сыр, рыба, мясо, Здоровяк, Старый докер, Бодрячок.
// LEO: Верхний ряд LIST_ICON2 (8x8): боеприпас, порох, шесть зелий из клеток 40..45.
void BLI_BuildConsumablesHUD()
{
	BLI_RemoveConsumablesHUD(); // LEO: Удаляем прежние атрибуты, иначе отключённый ряд останется видимым.
	float fHtRatio = stf(Render.screen_y) / screenscaling;
	string khFoodTex = CommandIcons_Texture("UseFood");
	string khIconTex = CommandIcons_Texture("LIST_ICON2");
	int khBPic[8]; khBPic[0]=4; khBPic[1]=0; khBPic[2]=1; khBPic[3]=2; khBPic[4]=3; khBPic[5]=5; khBPic[6]=6; khBPic[7]=7;
	int khTPic[6]; khTPic[0]=40; khTPic[1]=41; khTPic[2]=42; khTPic[3]=43; khTPic[4]=44; khTPic[5]=45;
	int khAmmoPic = Cons_AmmoPicNum(LAi_GetCharacterBulletType(pchar));
	objLandInterface.ConsAmmoPic = khAmmoPic;
	objLandInterface.ConsPowderPic = Cons_PowderPicNum(LAi_GetCharacterGunpowderType(pchar));

	float khScale = 1.0;
	if(CheckAttribute(&InterfaceStates,"ConsumablesHUDScale")) khScale = stf(InterfaceStates.ConsumablesHUDScale);
	if(khScale < 0.5) khScale = 0.5;
	if(khScale > 2.0) khScale = 2.0;
	float khR = fHtRatio * khScale;
	objLandInterface.ConsScaleApplied = khScale;
	int khW     = RecalculateHIcon(makeint(34 * khR));
	int khH     = RecalculateVIcon(makeint(34 * khR));
	int khGap   = RecalculateHIcon(makeint(3 * khR));
	int khBott  = RecalculateVIcon(makeint(14 * fHtRatio));
	int khCntH  = RecalculateVIcon(makeint(17 * khR));
	int khRowGap= RecalculateVIcon(makeint(3 * khR));

	float khPos = 0.5;
	if(CheckAttribute(&InterfaceStates,"ConsumablesHUDPos")) khPos = stf(InterfaceStates.ConsumablesHUDPos);
	if(khPos < 0.0) khPos = 0.0;
	if(khPos > 1.0) khPos = 1.0;
	objLandInterface.ConsPosApplied = khPos;
	int khSide  = RecalculateHIcon(makeint(8 * fHtRatio));
	int khBRowW = 8*khW + 7*khGap;
	int khFree  = sti(showWindow.right) - sti(showWindow.left) - 2*khSide - khBRowW;
	if(khFree < 0) khFree = 0;
	int khBX0   = sti(showWindow.left) + khSide + makeint(makefloat(khFree) * khPos);
	int khBIconY= sti(showWindow.bottom) - khBott - khH;
	int khTX0   = khBX0;
	int khTIconY= khBIconY - khH - khRowGap;

	int khBCntY = khBIconY + khH - khCntH;
	int khTCntY = khTIconY + khH - khCntH;
	int khNoTop    = sti(InterfaceStates.ConsumablesNoTop);
	int khNoBottom = sti(InterfaceStates.ConsumablesNoBottom);
	if(khNoBottom == 1) { khTIconY = khBIconY; khTCntY = khBCntY; } // LEO: При отключённом нижнем ряде верхний занимает его место.

	khBX0 = khBX0 + HUDLayout_GetOffset("land","consumables_bottom","x");
	khBIconY = khBIconY + HUDLayout_GetOffset("land","consumables_bottom","y");
	khBCntY = khBCntY + HUDLayout_GetOffset("land","consumables_bottom","y");
	khTX0 = khTX0 + HUDLayout_GetOffset("land","consumables_top","x");
	khTIconY = khTIconY + HUDLayout_GetOffset("land","consumables_top","y");
	khTCntY = khTCntY + HUDLayout_GetOffset("land","consumables_top","y");
	objLandInterface.ConsumablesCfg = khNoTop*2 + khNoBottom;
	int khI; int khPic; int khCol; int khRow; string khUV; int khX; string khNm; string khCnt;
	for(khI=0; khI<8; khI++)
	{
		if(khNoBottom == 1) break;
		khPic = khBPic[khI];
		khCol = khPic % 8; khRow = khPic / 8;
		khUV = FloatToString(stf(khCol)/8.0,3)+","+FloatToString(stf(khRow)/2.0,3)+","+FloatToString(stf(khCol+1)/8.0,3)+","+FloatToString(stf(khRow+1)/2.0,3);
		khX = khBX0 + khI*(khW+khGap);
		khNm = "konsB"+khI;
		objLandInterface.imageslist.(khNm).texture = khFoodTex;
		objLandInterface.imageslist.(khNm).color = argb(255,128,128,128);
		objLandInterface.imageslist.(khNm).uv = khUV;
		objLandInterface.imageslist.(khNm).pos = ""+khX+","+khBIconY+","+(khX+khW)+","+(khBIconY+khH);
		khCnt = "konsBc"+khI;
		objLandInterface.textinfo.(khCnt).font = "interface_normal";
		objLandInterface.textinfo.(khCnt).scale = 1.0 * khR;
		objLandInterface.textinfo.(khCnt).color = argb(255,255,255,255);
		objLandInterface.textinfo.(khCnt).shadow = 1;
		objLandInterface.textinfo.(khCnt).align = "left";
		objLandInterface.textinfo.(khCnt).pos.x = khX + RecalculateHIcon(makeint(3 * khR));
		objLandInterface.textinfo.(khCnt).pos.y = khBCntY;
		objLandInterface.textinfo.(khCnt).refreshable = 1;
		objLandInterface.textinfo.(khCnt).text = "";
	}
	int khCols; int khRows; string khTex;
	for(khI=0; khI<8; khI++)
	{
		if(khNoTop == 1) break;
		if(khI < 2)
		{
			khTex = khIconTex; khCols = 8; khRows = 8;
			if(khI==0) khPic = khAmmoPic;
			else khPic = Cons_PowderPicNum(LAi_GetCharacterGunpowderType(pchar));
		}
		else
		{
			khTex = khIconTex; khCols = 8; khRows = 8;
			khPic = khTPic[khI-2];
		}
		khCol = khPic % khCols; khRow = khPic / khCols;
		khUV = FloatToString(stf(khCol)/stf(khCols),3)+","+FloatToString(stf(khRow)/stf(khRows),3)+","+FloatToString(stf(khCol+1)/stf(khCols),3)+","+FloatToString(stf(khRow+1)/stf(khRows),3);
		khX = khTX0 + khI*(khW+khGap);
		khNm = "konsT"+khI;
		objLandInterface.imageslist.(khNm).texture = khTex;
		objLandInterface.imageslist.(khNm).color = argb(255,128,128,128);
		objLandInterface.imageslist.(khNm).uv = khUV;
		objLandInterface.imageslist.(khNm).pos = ""+khX+","+khTIconY+","+(khX+khW)+","+(khTIconY+khH);
		khCnt = "konsTc"+khI;
		objLandInterface.textinfo.(khCnt).font = "interface_normal";
		objLandInterface.textinfo.(khCnt).scale = 1.0 * khR;
		objLandInterface.textinfo.(khCnt).color = argb(255,255,255,255);
		objLandInterface.textinfo.(khCnt).shadow = 1;
		objLandInterface.textinfo.(khCnt).align = "left";
		objLandInterface.textinfo.(khCnt).pos.x = khX + RecalculateHIcon(makeint(3 * khR));
		objLandInterface.textinfo.(khCnt).pos.y = khTCntY;
		objLandInterface.textinfo.(khCnt).refreshable = 1;
		objLandInterface.textinfo.(khCnt).text = "";
	}
	int khContext = 0;
	if(CheckAttribute(&InterfaceStates,"ConsumablesHUDContext") && sti(InterfaceStates.ConsumablesHUDContext)==1) khContext = 1;
	objLandInterface.ConsContextApplied = khContext;
	int khVisible = BLI_UpdateConsumablesCounts();
	objLandInterface.ConsVisibleMask = khVisible;
	if(khContext == 1) BLI_CompactConsumables(khVisible,khBX0,khBIconY,khTX0,khTIconY,khW,khH,khGap,RecalculateHIcon(makeint(3*khR)),khCntH);
}

void BLI_RemoveConsumablesHUD()
{
	int khI;
	for(khI=0; khI<8; khI++)
	{
		DeleteAttribute(&objLandInterface, "imageslist.konsB"+khI);
		DeleteAttribute(&objLandInterface, "textinfo.konsBc"+khI);
	}
	for(khI=0; khI<8; khI++)
	{
		DeleteAttribute(&objLandInterface, "imageslist.konsT"+khI);
		DeleteAttribute(&objLandInterface, "textinfo.konsTc"+khI);
	}
	DeleteAttribute(&objLandInterface,"ConsContextApplied");
	DeleteAttribute(&objLandInterface,"ConsVisibleMask");
}

string LandHUD_OptionKey(int index)
{
	string key="";
	switch(index)
	{
		case 0: key="LandExpLogX"; break;
		case 1: key="LandExpLogY"; break;
		case 2: key="LandCrewBarsX"; break;
		case 3: key="LandCrewBarsY"; break;
		case 4: key="LandInfoOpacity"; break;
		case 5: key="LandExpLogLines"; break;
		case 6: key="LandAimOutline"; break;
		case 7: key="LandAimInvert"; break;
	}
	return key;
}

float LandHUD_Default(string key)
{
	if(key=="LandExpLogY") return 0.16;
	if(key=="LandCrewBarsY") return 0.82;
	if(key=="LandInfoOpacity") return 0.55;
	if(key=="LandExpLogLines") return 8.0;
	if(key=="LandAimOutline" || key=="LandAimInvert") return 0.0;
	return 1.0;
}

float LandHUD_GetOption(string key)
{
	float value=LandHUD_Default(key);
	if(CheckAttribute(&InterfaceStates,key)) value=stf(InterfaceStates.(key));
	if(key=="LandExpLogLines") return makefloat(ExpLog_ClampLines(value));
	if(value<0.0) return 0.0;
	if(value>1.0) return 1.0;
	return value;
}

void LandHUD_SaveOptions(ref options)
{
	for(int i=0;i<8;i++)
	{
		string key=LandHUD_OptionKey(i);
		options.cameramode.(key)=LandHUD_GetOption(key);
	}
}

void LandHUD_LoadOptions(ref options)
{
	for(int i=0;i<8;i++)
	{
		string key=LandHUD_OptionKey(i);
		InterfaceStates.(key)=LandHUD_Default(key);
		if(CheckAttribute(options,"cameramode."+key)) InterfaceStates.(key)=options.cameramode.(key);
		InterfaceStates.(key)=LandHUD_GetOption(key);
	}
}

// LEO: Ползунок двигает весь блок между краями игровой области, а не его центральную точку.
int LandHUD_Position(int start,int end,int extent,int margin,float fraction)
{
	int available=end-start-extent-2*margin;
	if(available<0) available=0;
	return start+margin+makeint(available*fraction);
}

void BLI_LayoutExperience(float ratio)
{
	int width=RecalculateHIcon(makeint(320*ratio));
	int step=RecalculateVIcon(makeint(20*ratio));
	int lines=makeint(LandHUD_GetOption("LandExpLogLines"));
	float posX=LandHUD_GetOption("LandExpLogX");
	float posY=LandHUD_GetOption("LandExpLogY");
	int logHeight=lines*step;
	int x=LandHUD_Position(sti(showWindow.left),sti(showWindow.right),width,RecalculateHIcon(makeint(43*ratio)),posX);
	int y=LandHUD_Position(sti(showWindow.top),sti(showWindow.bottom),logHeight,RecalculateVIcon(makeint(10*ratio)),posY);
	int dx=HUDLayout_GetOffset("land","experience","x");
	int dy=HUDLayout_GetOffset("land","experience","y");
	if(x+dx<sti(showWindow.left)) dx=sti(showWindow.left)-x;
	if(x+dx+width>sti(showWindow.right)) dx=sti(showWindow.right)-x-width;
	if(y+dy<sti(showWindow.top)) dy=sti(showWindow.top)-y;
	if(y+dy+logHeight>sti(showWindow.bottom)) dy=sti(showWindow.bottom)-y-logHeight;
	objLandInterface.landHUD.expX=posX;
	objLandInterface.landHUD.expY=posY;
	objLandInterface.landHUD.expLines=lines;
	for(int z=1;z<=ExpLog_MaxLines();z++)
	{
		string line="EXP"+z;
		objLandInterface.textinfo.(line).font="interface_normal";
		objLandInterface.textinfo.(line).scale=1.2*ratio;
		objLandInterface.textinfo.(line).align="right";
		objLandInterface.textinfo.(line).maxWidth=width;
		objLandInterface.textinfo.(line).maxLines=1;
		objLandInterface.textinfo.(line).pos.x=x+dx+width;
		objLandInterface.textinfo.(line).pos.y=y+dy+(z-1)*step;
		objLandInterface.textinfo.(line).refreshable=true;
		if(!CheckAttribute(&objLandInterface,"textinfo."+line+".text")) objLandInterface.textinfo.(line).text="";
	}
}

void BLI_LayoutCrewBars(float ratio)
{
	int width=RecalculateHIcon(makeint(180*ratio));
	int badge=RecalculateHIcon(makeint(34*ratio));
	int gap=RecalculateHIcon(makeint(4*ratio));
	int height=RecalculateVIcon(makeint(16*ratio));
	int rowGap=RecalculateVIcon(makeint(4*ratio));
	float posX=LandHUD_GetOption("LandCrewBarsX");
	float posY=LandHUD_GetOption("LandCrewBarsY");
	int x=LandHUD_Position(sti(showWindow.left),sti(showWindow.right),badge+gap+width,RecalculateHIcon(makeint(43*ratio)),posX);
	int y=LandHUD_Position(sti(showWindow.top),sti(showWindow.bottom),2*height+rowGap,RecalculateVIcon(makeint(18*ratio)),posY);
	IBoardingStatus.crewHud=1;
	IBoardingStatus.layoutX=posX;
	IBoardingStatus.layoutY=posY;
	IBoardingStatus.myLeft=x+badge+gap;
	IBoardingStatus.enemyLeft=x+badge+gap;
	IBoardingStatus.myTop=y;
	IBoardingStatus.enemyTop=y+height+rowGap;
	IBoardingStatus.width=width;
	IBoardingStatus.height=height;
	IBoardingStatus.badgeWidth=badge;
	IBoardingStatus.badgeGap=gap;
	IBoardingStatus.badgeScale=0.76*ratio;
	IBoardingStatus.myColor=argb(255,112,172,100);
	IBoardingStatus.enemyColor=argb(255,211,113,105);
}

// LEO: Левая граница панели служит ориентиром и для быстрой команды.
int BLI_InfoPanelLeft(float ratio)
{
	return sti(showWindow.right)-RecalculateHIcon(makeint(228*ratio));
}

// LEO: Тот же фон, что у морской панели; пустые строки острова и колонии на палубе не занимают место.
void BLI_LayoutInfoPanel(float ratio)
{
	int right=sti(showWindow.right)-RecalculateHIcon(makeint(14*ratio));
	int left=BLI_InfoPanelLeft(ratio);
	int width=right-left;
	int top=sti(showWindow.top)+RecalculateVIcon(makeint(6*ratio));
	int pad=RecalculateVIcon(makeint(8*ratio));
	int step=RecalculateVIcon(makeint(20*ratio));
	int y=top+pad+RecalculateVIcon(makeint(3*ratio));
	string names[4]; names[0]="islandname"; names[1]="villagename"; names[2]="locationname"; names[3]="datatext";
	for(int i=0;i<4;i++)
	{
		string line=names[i];
		objLandInterface.textinfo.(line).font="interface_normal";
		objLandInterface.textinfo.(line).scale=0.95*ratio;
		objLandInterface.textinfo.(line).align="center";
		objLandInterface.textinfo.(line).pos.x=left+width/2;
		objLandInterface.textinfo.(line).pos.y=y;
		objLandInterface.textinfo.(line).maxWidth=width-RecalculateHIcon(makeint(18*ratio));
		objLandInterface.textinfo.(line).lineHeight=step;
		objLandInterface.textinfo.(line).maxLines=1;
		if(line=="locationname" && GetStringWidth(objLandInterface.textinfo.(line).text,"interface_normal",0.95*ratio) > sti(objLandInterface.textinfo.(line).maxWidth))
			objLandInterface.textinfo.(line).maxLines=2;
		if(objLandInterface.textinfo.(line).text!="") y=y+step*sti(objLandInterface.textinfo.(line).maxLines);
	}
	string rect=left+","+top+","+right+","+(y+RecalculateVIcon(makeint(5*ratio)));
	float opacity=LandHUD_GetOption("LandInfoOpacity");
	objLandInterface.landHUD.infoOpacity=opacity;
	int color=argb(makeint(255*opacity),255,255,255);
	HUDLayout_SetImage9Slice(&objLandInterface,"land_info","battle_interface\NewHUD\Navigation\InfoBacking.dds",rect,color,1024,256,16,makeint(3.0*ratio+0.5));
}

void BLI_ApplyAimHUDOptions()
{
	objLandInterface.landHUD.aimOutline=LandHUD_GetOption("LandAimOutline")>0.0;
	objLandInterface.landHUD.aimInvert=LandHUD_GetOption("LandAimInvert")>0.0;
}

void BLI_RefreshLandHUDSettings()
{
	if(!IsEntity(&objLandInterface)) return;
	BLI_ApplyAimHUDOptions();
	float ratio=stf(Render.screen_y)/screenscaling;
	bool refresh=false;
	if(stf(objLandInterface.landHUD.expX)!=LandHUD_GetOption("LandExpLogX") ||
	   stf(objLandInterface.landHUD.expY)!=LandHUD_GetOption("LandExpLogY") ||
	   sti(objLandInterface.landHUD.expLines)!=ExpLog_LineCount(false))
	{
		BLI_LayoutExperience(ratio);
		ExpLog_Apply(&objLandInterface,false);
		refresh=true;
	}
	if(stf(objLandInterface.landHUD.infoOpacity)!=LandHUD_GetOption("LandInfoOpacity"))
	{
		BLI_LayoutInfoPanel(ratio);
		refresh=true;
	}
	if(refresh) SendMessage(&objLandInterface,"l",MSG_BATTLE_LAND_REFRESH_HUD);
	Log_RefreshCrewBars(ratio);
}

void BLI_SetObjectData()
{
    HUDLayout_Init();
    float fHtRatio = stf(Render.screen_y) / screenscaling;
    int fTmp, fTmp2;
	DeleteAttribute(&objLandInterface,"");

	BLI_ApplyAimHUDOptions();
	objLandInterface.parameters.DoShowCommandos = InterfaceStates.BattleShow.Command;
	objLandInterface.data.Alarm = 0;

	aref ar;
	int i,cn;
	ref curCh;
	string attrName;
	for(i=0; i<=MAX_NUM_FIGHTERS; i++)
	{
		cn = GetOfficersIndex(pchar,i);
		if(cn==-1) continue;
		curCh = GetCharacter(cn);
		attrName = "id"+i;
		makearef(ar,objLandInterface.data.icons.(attrName));
		SetCharacterIconData(cn,ar);
	}
	BLI_SetMessageParameters();
	int idLngFile = LanguageOpenFile("commands_name.txt");
	if(AltIntIcons) objLandInterface.CommandTextures.list.t0.name = "battle_interface\LandCommands_Konshud.dds";
	else objLandInterface.CommandTextures.list.t0.name = "battle_interface\LandCommands.dds";
	objLandInterface.CommandTextures.list.t0.xsize = 16;
	objLandInterface.CommandTextures.list.t0.ysize = 4;

 	if(AltIntIcons) objLandInterface.CommandTextures.list.t1.name = "battle_interface\LandTarget_SLB_Konshud.dds";
 	else objLandInterface.CommandTextures.list.t1.name = "battle_interface\LandTarget_SLB.dds";
	objLandInterface.CommandTextures.list.t1.xsize = 16;
	objLandInterface.CommandTextures.list.t1.ysize = 2;

	if(AltIntIcons) objLandInterface.CommandTextures.list.t2.name = "battle_interface\Cancel_Konshud.dds";
	else objLandInterface.CommandTextures.list.t2.name = "battle_interface\Cancel.dds";
	objLandInterface.CommandTextures.list.t2.xsize = 2;
	objLandInterface.CommandTextures.list.t2.ysize = 1;
	if(AltIntIcons) objLandInterface.CommandTextures.list.t3.name = "battle_interface\UseFood_Konshud.dds";
	else objLandInterface.CommandTextures.list.t3.name = "battle_interface\UseFood.dds";
	objLandInterface.CommandTextures.list.t3.xsize = 8;
	objLandInterface.CommandTextures.list.t3.ysize = 2;
	objLandInterface.CommandTextures.CommandTexNum = 0;
	aref commandSkinOwner;
	makearef(commandSkinOwner,objLandInterface);
	CommandIcons_Setup(commandSkinOwner,0);
	PerkTimers_CreateSlots(&objLandInterface, false);

	objLandInterface.ConsumablesBuilt = 0;
	if(CheckAttribute(&InterfaceStates,"ConsumablesHUD") && sti(InterfaceStates.ConsumablesHUD)==1)
	{
		BLI_BuildConsumablesHUD();
		objLandInterface.ConsumablesBuilt = 1;
	}
	DeleteAttribute(&objIconsNote,"");
	objIconsNote.1x0 = LanguageConvertString(idLngFile, "go_store");
	objIconsNote.1x1 = LanguageConvertString(idLngFile, "go_shipyard");
	objIconsNote.1x2 = LanguageConvertString(idLngFile, "go_tavern");
	objIconsNote.1x3 = LanguageConvertString(idLngFile, "go_usurer");
	objIconsNote.1x4 = LanguageConvertString(idLngFile, "go_residence");
	objIconsNote.1x5 = LanguageConvertString(idLngFile, "go_prison");
	objIconsNote.1x6 = LanguageConvertString(idLngFile, "go_church");
	objIconsNote.1x7 = LanguageConvertString(idLngFile, "go_fort");
	objIconsNote.1x8 = LanguageConvertString(idLngFile, "go_portman");
	objIconsNote.1x9 = LanguageConvertString(idLngFile, "go_brothel");
	objIconsNote.1x10 = LanguageConvertString(idLngFile, "go_port");
	objIconsNote.1x13 = LanguageConvertString(idLngFile, "go_townexit");
	objIconsNote.1x14 = LanguageConvertString(idLngFile, "go_storehouse");
	objIconsNote.1x15 = LanguageConvertString(idLngFile, "go_minehouse");
	objIconsNote.1x16 = LanguageConvertString(idLngFile, "go_planthouse");
	objIconsNote.1x17 = LanguageConvertString(idLngFile, "go_forthouse");
	objIconsNote.1x18 = LanguageConvertString(idLngFile, "go_graveyard");
	objIconsNote.1x19 = LanguageConvertString(idLngFile, "go_GWIK");
	objIconsNote.1x20 = LanguageConvertString(idLngFile, "go_Inquisition");
	objIconsNote.1x21 = LanguageConvertString(idLngFile, "go_Fernando");
	objIconsNote.1x22 = LanguageConvertString(idLngFile, "go_Apteka");
	objLandInterface.Commands.Cancel.enable		= false;
	objLandInterface.Commands.Cancel.picNum		= 1;
	objLandInterface.Commands.Cancel.selPicNum	= 0;
	objLandInterface.Commands.Cancel.texNum		= 2;
	objLandInterface.Commands.Cancel.event		= "Cancel";
	objLandInterface.Commands.Cancel.name		= "Cancel";
	objLandInterface.Commands.Cancel.note		= LanguageConvertString(idLngFile, "sea_cancel");

	objLandInterface.Commands.FastReload.enable		= true;
	objLandInterface.Commands.FastReload.picNum		= 27;
	objLandInterface.Commands.FastReload.selPicNum	= 11;
	objLandInterface.Commands.FastReload.texNum		= 1;
	objLandInterface.Commands.FastReload.event		= "BI_FastReload";
	objLandInterface.Commands.FastReload.note		= LanguageConvertString(idLngFile, "land_FastReload");

	objLandInterface.Commands.OutDoor.enable		= true;
	objLandInterface.Commands.OutDoor.picNum		= 28;
	objLandInterface.Commands.OutDoor.selPicNum		= 12;
	objLandInterface.Commands.OutDoor.texNum		= 1;
	objLandInterface.Commands.OutDoor.event			= "BI_OutDoor";
	objLandInterface.Commands.OutDoor.note			= LanguageConvertString(idLngFile, "land_OutDoor");

    objLandInterface.Commands.ItemsUse.enable		= true;
 	objLandInterface.Commands.ItemsUse.picNum		= 20;
	objLandInterface.Commands.ItemsUse.selPicNum		= 4;
	objLandInterface.Commands.ItemsUse.texNum		= 0;
	objLandInterface.Commands.ItemsUse.event		= "BI_ItemsUse";
	objLandInterface.Commands.ItemsUse.note			= LanguageConvertString(idLngFile, "land_ItemsUse");

    objLandInterface.Commands.UseFood.enable		= true;
 	objLandInterface.Commands.UseFood.picNum		= 20;
	objLandInterface.Commands.UseFood.selPicNum		= 4;
	objLandInterface.Commands.UseFood.texNum		= 0;
	objLandInterface.Commands.UseFood.event			= "BI_UseFood";
	objLandInterface.Commands.UseFood.note			= LanguageConvertString(idLngFile, "land_UseFood");

	objLandInterface.Commands.DialogStart.enable	= true;
 	objLandInterface.Commands.DialogStart.picNum		= 17;
	objLandInterface.Commands.DialogStart.selPicNum		= 1;
	objLandInterface.Commands.DialogStart.texNum		= 0;
	objLandInterface.Commands.DialogStart.event		= "BI_DialogStart";
	objLandInterface.Commands.DialogStart.note		= LanguageConvertString(idLngFile, "land_DialogStart");

	objLandInterface.Commands.ItemsChange.enable	= true;
 	objLandInterface.Commands.ItemsChange.picNum		= 19;
	objLandInterface.Commands.ItemsChange.selPicNum		= 3;
	objLandInterface.Commands.ItemsChange.texNum		= 0;
	objLandInterface.Commands.ItemsChange.event		= "BI_ItemsChange";
	objLandInterface.Commands.ItemsChange.note		= LanguageConvertString(idLngFile, "land_ItemsChange");

	objLandInterface.Commands.TakeItem.enable		= true;
 	objLandInterface.Commands.TakeItem.picNum		= 16;
	objLandInterface.Commands.TakeItem.selPicNum		= 0;
	objLandInterface.Commands.TakeItem.texNum		= 0;
	objLandInterface.Commands.TakeItem.event		= "BI_TakeItem";
	objLandInterface.Commands.TakeItem.note			= LanguageConvertString(idLngFile, "land_TakeItem");

	objLandInterface.Commands.PlaceItem.enable		= true;
 	objLandInterface.Commands.PlaceItem.picNum		= 16;
	objLandInterface.Commands.PlaceItem.selPicNum		= 0;
	objLandInterface.Commands.PlaceItem.texNum		= 0;
	objLandInterface.Commands.PlaceItem.event		= "BI_PlaceItem";
	objLandInterface.Commands.PlaceItem.note		= LanguageConvertString(idLngFile, "land_PlaceItem");

	objLandInterface.Commands.UseBox.enable			= true;
 	objLandInterface.Commands.UseBox.picNum		    = 16;
	objLandInterface.Commands.UseBox.selPicNum		= 0;
	objLandInterface.Commands.UseBox.texNum		= 0;
	objLandInterface.Commands.UseBox.event			= "BI_UseBox";
	objLandInterface.Commands.UseBox.note			= LanguageConvertString(idLngFile, "land_UseBox");

	objLandInterface.Commands.DeadBox.enable			= true;
 	objLandInterface.Commands.DeadBox.picNum		    = 20;
	objLandInterface.Commands.DeadBox.selPicNum		= 4;
	objLandInterface.Commands.DeadBox.texNum		= 0;
	objLandInterface.Commands.DeadBox.event			= "BI_DeadBox";
	objLandInterface.Commands.DeadBox.note			= GetConvertStrWithReplace("Variable_LandInterface_1", "Battle_Interface.txt", "#space#", " ");
	objLandInterface.Commands.TalkSelf.enable	= true;
 	objLandInterface.Commands.TalkSelf.picNum	 	= 18;
	objLandInterface.Commands.TalkSelf.selPicNum	= 2;
	objLandInterface.Commands.TalkSelf.texNum		= 0;
	objLandInterface.Commands.TalkSelf.event		= "BI_TalkSelf";
	objLandInterface.Commands.TalkSelf.note		    = GetConvertStrWithReplace("Variable_LandInterface_2", "Battle_Interface.txt", "#space#", " ");

	objLandInterface.Commands.Smoke.enable			= true;
	objLandInterface.Commands.Smoke.picNum			= 20;
	objLandInterface.Commands.Smoke.selPicNum		= 4;
	objLandInterface.Commands.Smoke.texNum			= 0;
	objLandInterface.Commands.Smoke.event			= "BI_Smoke";
	objLandInterface.Commands.Smoke.note			= LanguageConvertString(idLngFile, "land_Smoke");

	objLandInterface.Commands.Craft.enable	= true;
 	objLandInterface.Commands.Craft.picNum	 	= 48;
	objLandInterface.Commands.Craft.selPicNum	= 32;
	objLandInterface.Commands.Craft.texNum		= 0;
	objLandInterface.Commands.Craft.event		= "BI_Craft";
	objLandInterface.Commands.Craft.note		= GetConvertStrWithReplace("Variable_LandInterface_3", "Battle_Interface.txt", "#space#", " ");

	objLandInterface.Commands.ActivateRush.enable	= true;
	objLandInterface.Commands.ActivateRush.picNum	= 28;
	objLandInterface.Commands.ActivateRush.selPicNum		= 12;
	objLandInterface.Commands.ActivateRush.texNum		= 0;
	objLandInterface.Commands.ActivateRush.event	= "BI_ActivateRush";
	objLandInterface.Commands.ActivateRush.note		= LanguageConvertString(idLngFile, "land_ActivateRush");

	objLandInterface.Commands.Exit_Deck.enable		= true;
	objLandInterface.Commands.Exit_Deck.picNum		= 30;
	objLandInterface.Commands.Exit_Deck.selPicNum		= 14;
	objLandInterface.Commands.Exit_Deck.texNum		= 0;
	objLandInterface.Commands.Exit_Deck.event		= "BI_Exit_Deck";
	objLandInterface.Commands.Exit_Deck.note		= LanguageConvertString(idLngFile, "land_Exit");
	LanguageCloseFile(idLngFile);
	int idxloc = FindLoadedLocation();
	string outGroupName = "";
	RecalculateHireCrew(idxloc);
	if(idxloc>=0)
	if(CheckAttribute(&Locations[idxloc],"fastreload"))
	{
		outGroupName = Locations[idxloc].fastreload;
		CreateReloadPaths(outGroupName);
	}

	objLandInterface.ManSign.useUDFBars = true;
	objLandInterface.ManSign.UDFColor1 = ARGB(255,128,128,128);
	objLandInterface.ManSign.UDFColor2 = ARGB(255,0,255,0);

	if (sti(InterfaceStates.InterfaceSelection) != KRUGLYI)
	{
		objLandInterface.ManSign.UDFWidth 				= makeint(59.0 * fHtRatio);
		objLandInterface.ManSign.UDFHeight 				= makeint(3.0 * fHtRatio);
		objLandInterface.ManSign.UDFPos2 				= makeint(20.0 * fHtRatio)+","+makeint(85.0 * fHtRatio);
		objLandInterface.ManSign.UDFPos1 				= makeint(20.0 * fHtRatio)+","+makeint(85.0 * fHtRatio);
		objLandInterface.ManSign.UDFOffsets 			= "0,"+makeint(85.0 * fHtRatio);

		objLandInterface.ManSign.backtexturename		= "battle_interface\Char_Border.dds";
		objLandInterface.ManSign.backcolor				= argb(255,128,128,128);
		objLandInterface.ManSign.backuv					= "0.0,0.0,1.0,1.0";
		fTmp 											= makeint(-10.0 * fHtRatio);
		fTmp2 											= makeint(-5.0 * fHtRatio);
		objLandInterface.ManSign.backoffset				= fTmp + "," + fTmp2;
		objLandInterface.ManSign.backiconsize			= makeint(64.0 * fHtRatio)+","+makeint(74.0 * fHtRatio);

		objLandInterface.ManSign.alarmtexturename		= "battle_interface\AlarmQ.dds";
		objLandInterface.ManSign.alarmhighcolor			= argb(255,168,28,28);
		objLandInterface.ManSign.alarmlowcolor			= argb(64,168,28,28);
		objLandInterface.ManSign.alarmuptime			= 0.5;
		objLandInterface.ManSign.alarmdowntime			= 1.5;
		objLandInterface.ManSign.alarmuv				= "0.0,0.0,1.0,1.0";
		fTmp 											= makeint(-10.0 * fHtRatio);
		fTmp2 											= makeint(-10.0 * fHtRatio);
		objLandInterface.ManSign.alarmoffset			= fTmp + "," + fTmp2;
		fTmp 											= makeint(62.0 * fHtRatio);
		objLandInterface.ManSign.alarmiconsize			= fTmp + "," + fTmp;

		objLandInterface.ManSign.manstatetexturename	= "battle_interface\ShipHP_SP.dds";
		objLandInterface.ManSign.manstatecolor			= argb(255,128,128,128);

		objLandInterface.ManSign.manhpuv				= "0.0,0.0,0.5,1.0";
		objLandInterface.ManSign.manenegryuv			= "0.5,0.0,1.0,1.0";

		fTmp 											= makeint(-44.5 * fHtRatio);
		fTmp2 											= makeint(-11.0 * fHtRatio);
		objLandInterface.ManSign.manhpoffset			= fTmp + "," + fTmp2;
		fTmp 											= makeint(6.0 * fHtRatio);
		fTmp2 											= makeint(63.0 * fHtRatio);
		objLandInterface.ManSign.manhpiconsize			= fTmp + "," + fTmp2;

		fTmp 											= makeint(24.5 * fHtRatio);
		fTmp2 											= makeint(-11.0 * fHtRatio);
		objLandInterface.ManSign.manenegryoffset		= fTmp + "," + fTmp2;
		fTmp 											= makeint(6.0 * fHtRatio);
		fTmp2 											= makeint(63.0 * fHtRatio);
		objLandInterface.ManSign.manenergyiconsize		= fTmp + "," + fTmp2;

		objLandInterface.ManSign.manstatetemptexturename	= "battle_interface\ShipHP_SP.dds";
		objLandInterface.ManSign.manhptempcolor 			= argb(255, 255, 100, 100);
		objLandInterface.ManSign.manenergytempcolor 		= argb(255, 100, 100, 255);
		objLandInterface.ManSign.manhptempuv 				= objLandInterface.ManSign.manhpuv;
		objLandInterface.ManSign.manenegrytempuv 			= objLandInterface.ManSign.manenegryuv;
		objLandInterface.ManSign.manhptempoffset 			= objLandInterface.ManSign.manhpoffset;
		objLandInterface.ManSign.manhptempiconsize 			= objLandInterface.ManSign.manhpiconsize;
		objLandInterface.ManSign.manenegrytempoffset 		= objLandInterface.ManSign.manenegryoffset;
		objLandInterface.ManSign.manenergytempiconsize 		= objLandInterface.ManSign.manenergyiconsize;

		objLandInterface.ManSign.gunchargetexturename	= "battle_interface\GunCharge_CSP.dds";
		objLandInterface.ManSign.gunchargecolor			= argb(255,0, 250, 154);
		objLandInterface.ManSign.gunchargebackcolor		= argb(255,0, 0, 0);
		objLandInterface.ManSign.gunchargeuv			= "0.0,0.0,1.0,1.0";
		objLandInterface.ManSign.gunchargeprogress		= "0.0625, 0.219, 0.359, 0.5, 0.641, 0.781, 0.983";

		fTmp 											= makeint(-10.0 * fHtRatio);
		fTmp2 											= makeint(-48.0 * fHtRatio);
		objLandInterface.ManSign.gunchargeoffset		= fTmp + "," + fTmp2;
		fTmp 											= makeint(80.0 * fHtRatio);
		fTmp2 											= makeint(20.0 * fHtRatio);
		objLandInterface.ManSign.gunchargeiconsize		= fTmp + "," + fTmp2;

		objLandInterface.ManSign.manfacecolor			= argb(255,128,128,128);
		fTmp 											= makeint(-10.0 * fHtRatio);
		fTmp2 											= makeint(-10.0 * fHtRatio);
		objLandInterface.ManSign.manfaceoffset			= fTmp + "," + fTmp2;
		fTmp 											= makeint(62.0 * fHtRatio);
		objLandInterface.ManSign.manfaceiconsize		= fTmp + "," + fTmp;

		int khBoardSig = 1;
		if(CheckAttribute(&InterfaceStates,"BoardingSignals")) khBoardSig = sti(InterfaceStates.BoardingSignals);

		objLandInterface.ManSign.shakeenable			= 1;
		if(CheckAttribute(&InterfaceStates,"PortraitShakeEnable")) objLandInterface.ManSign.shakeenable = sti(InterfaceStates.PortraitShakeEnable);
		if(khBoardSig == 0) objLandInterface.ManSign.shakeenable = 0;
		objLandInterface.ManSign.shakeamp				= 3.0 * fHtRatio;	// LEO: Амплитуда в пикселях с поправкой на разрешение.
		objLandInterface.ManSign.shaketime				= 0.28;				// LEO: Длительность затухания, секунды.

		objLandInterface.ManSign.pingenable				= 1;
		if(CheckAttribute(&InterfaceStates,"PortraitPingEnable")) objLandInterface.ManSign.pingenable = sti(InterfaceStates.PortraitPingEnable);
		if(khBoardSig == 0) objLandInterface.ManSign.pingenable = 0;
		objLandInterface.ManSign.pingthreshold			= 0.3;				// LEO: Порог здоровья, доля 0..1.
		objLandInterface.ManSign.pingblinkfreq			= 5.0;				// LEO: Частота мигания, Гц.
		objLandInterface.ManSign.pingblinkmin			= 0.5;				// LEO: Минимальный множитель яркости, 0..1.
		objLandInterface.ManSign.pingflashtime			= 0.5;				// LEO: Длительность начальной вспышки, секунды.
		objLandInterface.ManSign.pingflashamp			= 2.0;				// LEO: Добавка к яркости на пике: 1 удваивает яркость.

		objLandInterface.ManSign.manstateghoston		= 1;
		objLandInterface.ManSign.manstateghostspeed		= 7.0;
		objLandInterface.ManSign.manstateghosthold		= 0.12;				// LEO: Задержка хвоста урона, секунды.
		objLandInterface.ManSign.manstateghostcolor		= argb(255,255,255,255);

		objLandInterface.ManSign.commandlistverticaloffset = -36 * fHtRatio;
		fTmp 											= makeint(60.0 * fHtRatio);
		fTmp2 											= makeint(85.0 * fHtRatio);

		objLandInterface.ManSign.gunreloadtexturename	= "";
		objLandInterface.ManSign.gunreloadcolor 		= ARGB(255, 0, 255, 62);
		objLandInterface.ManSign.gunreloadbackcolor		= ARGB(255, 0, 255, 62);
		objLandInterface.ManSign.gunreloadUV			= "0.0,0.0,1.0,1.0";
		fTmp 											= makeint(-10.0 * fHtRatio);
		fTmp2 											= makeint(26.0 * fHtRatio);
		objLandInterface.ManSign.gunreloadoffset		= fTmp + "," + fTmp2;
		fTmp 											= makeint(56.0 * fHtRatio);
		fTmp2 											= makeint(5.0 * fHtRatio);
		objLandInterface.ManSign.gunreloadiconsize		= fTmp + "," + fTmp2;
	}
	else
	{

		objLandInterface.ManSign.UDFWidth 				= makeint(38.0 * fHtRatio);
		objLandInterface.ManSign.UDFHeight 				= makeint(10.0 * fHtRatio);
		objLandInterface.ManSign.UDFPos2 				= makeint(41.0 * fHtRatio)+","+makeint(81.0 * fHtRatio);
		objLandInterface.ManSign.UDFPos1 				= makeint(41.0 * fHtRatio)+","+makeint(81.0 * fHtRatio);
		objLandInterface.ManSign.UDFOffsets 			= "0,"+makeint(85.0 * fHtRatio);

		objLandInterface.ManSign.backtexturename		= "battle_interface\CharBackIcon.dds";
		objLandInterface.ManSign.backcolor				= argb(255,128,128,128);
		objLandInterface.ManSign.backuv					= "0.0,0.0,1.0,1.0";
		fTmp 											= makeint(0.0 * fHtRatio);
		fTmp2 											= makeint(-12.0 * fHtRatio);
		objLandInterface.ManSign.backoffset				= fTmp + "," + fTmp2;
		fTmp 											= makeint(90.0 * fHtRatio);
		objLandInterface.ManSign.backiconsize			= fTmp + "," + fTmp;

		objLandInterface.ManSign.alarmtexturename		= "battle_interface\AlarmBack.dds";
		objLandInterface.ManSign.alarmhighcolor			= argb(255,168,28,28);
		objLandInterface.ManSign.alarmlowcolor			= argb(64,168,28,28);
		objLandInterface.ManSign.alarmuptime			= 0.5;
		objLandInterface.ManSign.alarmdowntime			= 1.5;
		objLandInterface.ManSign.alarmuv				= "0.0,0.0,1.0,1.0";
		fTmp 											= makeint(0.0 * fHtRatio);
		fTmp2 											= makeint(-16.0 * fHtRatio);
		objLandInterface.ManSign.alarmoffset			= fTmp + "," + fTmp2;
		fTmp 											= makeint(92.0 * fHtRatio);
		objLandInterface.ManSign.alarmiconsize			= fTmp + "," + fTmp;

		objLandInterface.ManSign.manstatetexturename	= "battle_interface\Char_State.dds";
		objLandInterface.ManSign.manstatecolor			= argb(255,128,128,128);
		objLandInterface.ManSign.manhpuv				= "0.0,0.13,0.72,0.89";
		objLandInterface.ManSign.manenegryuv			= "0.30,0.13,1.0,0.89";

		fTmp 											= makeint(-14.0 * fHtRatio);
		fTmp2 											= makeint(-11.0 * fHtRatio);
		objLandInterface.ManSign.manhpoffset			= fTmp + "," + fTmp2;
		fTmp 											= makeint(61.0 * fHtRatio);
		fTmp2 											= makeint(68.5 * fHtRatio);
		objLandInterface.ManSign.manhpiconsize			= fTmp + "," + fTmp2;
		fTmp 											= makeint(14.0 * fHtRatio);
		fTmp2 											= makeint(-11.0 * fHtRatio);
		objLandInterface.ManSign.manenegryoffset		= fTmp + "," + fTmp2;
		fTmp 											= makeint(61.0 * fHtRatio);
		fTmp2 											= makeint(68.5 * fHtRatio);
		objLandInterface.ManSign.manenergyiconsize		= fTmp + "," + fTmp2;

		objLandInterface.ManSign.manstatetemptexturename	= "battle_interface\Char_State.dds";
		objLandInterface.ManSign.manhptempcolor 			= argb(255, 255, 100, 100);
		objLandInterface.ManSign.manenergytempcolor 		= argb(255, 100, 100, 255);
		objLandInterface.ManSign.manhptempuv = objLandInterface.ManSign.manhpuv;
		objLandInterface.ManSign.manenegrytempuv = objLandInterface.ManSign.manenegryuv;
		objLandInterface.ManSign.manhptempoffset = objLandInterface.ManSign.manhpoffset;
		objLandInterface.ManSign.manhptempiconsize = objLandInterface.ManSign.manhpiconsize;
		objLandInterface.ManSign.manenegrytempoffset = objLandInterface.ManSign.manenegryoffset;
		objLandInterface.ManSign.manenergytempiconsize = objLandInterface.ManSign.manenergyiconsize;

		objLandInterface.ManSign.manstateghoston	= 1;
		objLandInterface.ManSign.manstateghostspeed	= 7.0;
		objLandInterface.ManSign.manstateghosthold	= 0.12;
		objLandInterface.ManSign.manstateghostcolor	= argb(255,255,255,255);

		objLandInterface.ManSign.gunchargetexturename	= "battle_interface\GunCharge.dds";
		objLandInterface.ManSign.gunchargecolor			= argb(255,0, 250, 154);
		objLandInterface.ManSign.gunchargebackcolor		= argb(255,0, 0, 0);
		objLandInterface.ManSign.gunchargeuv			= "0.0,0.0,1.0,1.0";
		objLandInterface.ManSign.gunchargeprogress		= "0.0625, 0.219, 0.359, 0.5, 0.641, 0.781, 0.983";
		fTmp 											= makeint(0.0 * fHtRatio);
		fTmp2 											= makeint(-50.0 * fHtRatio);
		objLandInterface.ManSign.gunchargeoffset		= fTmp + "," + fTmp2;
		fTmp 											= makeint(66.0 * fHtRatio);
		fTmp2 											= makeint(15.0 * fHtRatio);
		objLandInterface.ManSign.gunchargeiconsize		= fTmp + "," + fTmp2;

		objLandInterface.ManSign.manfacecolor			= argb(255,128,128,128);
		fTmp 											= makeint(0.0 * fHtRatio);
		fTmp2											= makeint(-16.0 * fHtRatio);
		objLandInterface.ManSign.manfaceoffset			= fTmp + "," + fTmp2;
		fTmp 											= makeint(58.0 * fHtRatio);
		objLandInterface.ManSign.manfaceiconsize		= fTmp + "," + fTmp;

		objLandInterface.ManSign.commandlistverticaloffset = -36 * fHtRatio;
		fTmp 											= makeint(60.0 * fHtRatio);
		fTmp2 											= makeint(85.0 * fHtRatio);

		objLandInterface.ManSign.gunreloadtexturename	= "";
		objLandInterface.ManSign.gunreloadcolor 		= ARGB(255, 0, 255, 62);
		objLandInterface.ManSign.gunreloadbackcolor		= ARGB(255, 0, 255, 62);
		objLandInterface.ManSign.gunreloadUV			= "0.0,0.0,1.0,1.0";
		fTmp 											= makeint(0.0 * fHtRatio);
		fTmp2 											= makeint(20.0 * fHtRatio);
		objLandInterface.ManSign.gunreloadoffset		= fTmp + "," + fTmp2;
		fTmp 											= makeint(39.0 * fHtRatio);
		fTmp2 											= makeint(10.0 * fHtRatio);
		objLandInterface.ManSign.gunreloadiconsize		= fTmp + "," + fTmp2;
	}

	fTmp    = makeint(60.0 * fHtRatio);
	fTmp2   = makeint(85.0 * fHtRatio);

	for(i = 1; i<=MAX_NUM_FIGHTERS + 1; i++) {
		string sOffsetIcon = "iconoffset" + i;
		objLandInterface.ManSign.(sOffsetIcon) = fTmp + "," + (fTmp + (i-1)*fTmp2);
	}

	makearef(ar, objLandInterface.ManSign);
	float thinLandDiameter=NewHUD_SetLandLayout(ar,sti(InterfaceStates.InterfaceSelection),MAX_NUM_FIGHTERS+1,fHtRatio,stf(Render.screen_y));
	NewHUD_Setup(ar, sti(InterfaceStates.InterfaceSelection), GetMainCharacterIndex(), false, false, thinLandDiameter, fHtRatio);
	if(CheckAttribute(ar, "manstateghoston")) ar.newhud.ghostenabled = sti(ar.manstateghoston) * NewHUD_GetOption("NewHUDGhosts");
	if(CheckAttribute(ar, "manstateghosthold")) ar.newhud.ghosthold = ar.manstateghosthold;
	if(CheckAttribute(ar, "manstateghostspeed")) ar.newhud.ghostspeed = ar.manstateghostspeed;
	if(CheckAttribute(ar, "manstateghostcolor")) ar.newhud.ghostcolor = ar.manstateghostcolor;

	objLandInterface.textinfo.islandname.text = "";
	objLandInterface.textinfo.villagename.text = "";
	objLandInterface.textinfo.locationname.text = "";
	int nLoc = FindLoadedLocation();
	if(nLoc >= 0)
	{
		int nFile = LanguageOpenFile("LocLables.txt");
		if(nFile >= 0) {
			if (CheckAttribute(&locations[nLoc],"islandId"))
			{
				if (locations[nLoc].islandId != "Mein")
				{
					objLandInterface.textinfo.islandname.text = XI_convertString("Island:") + LanguageConvertString(nFile, locations[nLoc].islandId);
				}
				else
				{
					objLandInterface.textinfo.islandname.text = LanguageConvertString(nFile, locations[nLoc].islandId);
				}
			}
			else
			{
				objLandInterface.textinfo.islandname.text = "";
			}
            string sCaimanName = PChar.ColonyBuilding.ColonyName;

			if (!CheckAttribute(&locations[nLoc],"fastreload"))
			{
				objLandInterface.textinfo.villagename.text = "";

				if(CheckAttribute(&Locations[nLoc], "islandId"))
				{
					if(Locations[nLoc].islandId == "Caiman")
					{
						if(sCaimanName == "")
						{
							sCaimanName = GetConvertStrWithReplace("Variable_LandInterface_4", "Battle_Interface.txt", "#space#", " ");
						}

						if(PChar.ColonyBuilding.Stage != "0")
						{
							objLandInterface.textinfo.villagename.text = XI_ConvertString("Colony:") + sCaimanName;
						}
					}
				}
			}
			else
			{
				objLandInterface.textinfo.villagename.text = XI_ConvertString("Colony:") + LanguageConvertString(nFile, locations[nLoc].fastreload + " Town");

				bool bCaiman = false;
				if(CheckAttribute(&Locations[nLoc], "islandId"))
				{
					if(Locations[nLoc].islandId == "Caiman") bCaiman = true;
				}

				if(locations[nLoc].fastreload == "Caiman" || locations[nLoc].fastreload == "CaimanSmall" || bCaiman)
				{
					if(sCaimanName == "")
					{
						sCaimanName = GetConvertStrWithReplace("Variable_LandInterface_5", "Battle_Interface.txt", "#space#", " ");
					}

					objLandInterface.textinfo.villagename.text = XI_ConvertString("Colony:") + sCaimanName;
				}
			}
			objLandInterface.textinfo.locationname.text = LanguageConvertString(nFile, locations[nLoc].id.label);
			if (CheckAttribute(pchar,"boatinfo"))
			{
				ref boatinfo = &Characters[sti(pchar.boatinfo)];
				objLandInterface.textinfo.locationname.text = XI_ConvertString(RealShips[sti(boatinfo.Ship.Type)].BaseName) + " ''" + boatinfo.Ship.Name + "''";
				DeleteAttribute(pchar,"boatinfo");
			}
			LanguageCloseFile( nFile );
		}
	}
	RefreshLandTime();
	objLandInterface.textinfo.datatext.refreshable = true;
	BLI_LayoutInfoPanel(fHtRatio);
	BLI_LayoutExperience(fHtRatio);
	ExpLog_Apply(&objLandInterface,false);

	if(CheckAttribute(loadedLocation,"fastreload"))
	{
		objLandInterface.textinfo.AltInfo.font = "interface_normal";
		objLandInterface.textinfo.AltInfo.color = argb(243,254,252,169);
		objLandInterface.textinfo.AltInfo.scale = 1.1 * fHtRatio;
		objLandInterface.textinfo.AltInfo.pos.x = sti(showWindow.left) + RecalculateHIcon(300 * fHtRatio);
		objLandInterface.textinfo.AltInfo.pos.y = RecalculateVIcon(20 * fHtRatio);
		objLandInterface.textinfo.AltInfo.refreshable = true;

		objLandInterface.textinfo.AltInfo1.font = "interface_normal";
		objLandInterface.textinfo.AltInfo1.scale = 1.1 * fHtRatio;
		objLandInterface.textinfo.AltInfo1.pos.x = sti(showWindow.left) + RecalculateHIcon(285 * fHtRatio);
		objLandInterface.textinfo.AltInfo1.pos.y = RecalculateVIcon(40 * fHtRatio);
		objLandInterface.textinfo.AltInfo1.refreshable = true;

		objLandInterface.textinfo.AltInfo2.font = "interface_normal";
		objLandInterface.textinfo.AltInfo2.scale = 1.1 * fHtRatio;
		objLandInterface.textinfo.AltInfo2.pos.x = sti(showWindow.left) + RecalculateHIcon(285 * fHtRatio);
		objLandInterface.textinfo.AltInfo2.pos.y = RecalculateVIcon(60 * fHtRatio);
		objLandInterface.textinfo.AltInfo2.refreshable = true;

		objLandInterface.textinfo.AltInfo3.font = "interface_normal";
		objLandInterface.textinfo.AltInfo3.scale = 1.1 * fHtRatio;
		objLandInterface.textinfo.AltInfo3.pos.x = sti(showWindow.left) + RecalculateHIcon(306 * fHtRatio);
		objLandInterface.textinfo.AltInfo3.pos.y = RecalculateVIcon(80 * fHtRatio);
		objLandInterface.textinfo.AltInfo3.refreshable = true;

		objLandInterface.textinfo.AltInfo4.font = "interface_normal";
		objLandInterface.textinfo.AltInfo4.scale = 1.1 * fHtRatio;
		objLandInterface.textinfo.AltInfo4.align = "left";
		objLandInterface.textinfo.AltInfo4.pos.x = sti(showWindow.left) + RecalculateHIcon(makeint(147 * fHtRatio));
		objLandInterface.textinfo.AltInfo4.pos.y = RecalculateVIcon(makeint(105 * fHtRatio));
		objLandInterface.textinfo.AltInfo4.color = argb(255, 200, 255, 200);
		objLandInterface.textinfo.AltInfo4.shadow = 1;
		objLandInterface.textinfo.AltInfo4.refreshable = true;
	}

	objLandInterface.CommandList.CommandMaxIconQuantity = 15;
	objLandInterface.CommandList.CommandIconSpace = 1;
	objLandInterface.CommandList.CommandIconLeft = makeint(110 * fHtRatio);
	objLandInterface.CommandList.CommandIconWidth = RecalculateHIcon(makeint(54 * fHtRatio));
	objLandInterface.CommandList.CommandIconHeight = RecalculateVIcon(makeint(54 * fHtRatio));

	objLandInterface.CommandList.CommandNoteFont = "interface_button";
	objLandInterface.CommandList.CommandNoteColor = argb(255,255,255,255);
	objLandInterface.CommandList.CommandNoteScale = 1.2 * fHtRatio;
	objLandInterface.CommandList.CommandNoteOffset = RecalculateHIcon(0) + "," + RecalculateVIcon(makeint(-50 * fHtRatio));

	objLandInterface.CommandList.UDArrow_Texture = "battle_interface\arrowly.dds";
	objLandInterface.CommandList.UDArrow_UV_Up = "0.0,1.0,1.0,0.0";
	objLandInterface.CommandList.UDArrow_UV_Down = "0.0,0.0,1.0,1.0";
	objLandInterface.CommandList.UDArrow_Size = RecalculateHIcon(makeint(32 * fHtRatio)) + "," + RecalculateVIcon(makeint(32 * fHtRatio));
	objLandInterface.CommandList.UDArrow_Offset_Up = RecalculateHIcon(makeint(-41 * fHtRatio)) + "," + RecalculateVIcon(makeint(-30 * fHtRatio));
	objLandInterface.CommandList.UDArrow_Offset_Down = RecalculateHIcon(makeint(-41 * fHtRatio)) + "," + RecalculateVIcon(makeint(46 * fHtRatio));
	HUDLayout_ApplySigns(ar,"land",MAX_NUM_FIGHTERS+1,true);
	aref layoutCommands;
	makearef(layoutCommands,objLandInterface.CommandList);
	HUDLayout_SetCommands(layoutCommands,"land",GetMainCharacterIndex());

	ModifyTextInfo();

	SendMessage(&objLandInterface,"l",MSG_BATTLE_LAND_START);
}

string GetFastReloadLocByPic(string sTown, int iPic)
{
	if (!CheckAttribute(&objFastReloadTable, "table." + sTown)) return "";
	aref locList;
	makearef(locList, objFastReloadTable.table.(sTown));
	int nq = GetAttributesNum(locList);
	for (int i = 0; i < nq; i++)
	{
		aref curloc = GetAttributeN(locList, i);

		if (CheckAttribute(curloc, "pic"))
		{
			if (sti(curloc.pic) == iPic)
			{
				if (CheckAttribute(curloc, "location")) return curloc.location;
			}
		}
	}
	return "";
}

// LEO: Скобки отмечают текущее заведение, пустая строка - недоступное или отсутствующее.
string FormatHKT(string sCurLoc, string sTargetLoc, string sName)
{
	if (sTargetLoc == "") return "";
	if (sCurLoc == sTargetLoc) return "[" + sName + "]";
	if (!CheckFastJump(sCurLoc, sTargetLoc)) return "";
	return sName;
}

string AddHKT(string sLine, string sItem)
{
	if (sItem == "") return sLine;
	if (sLine == "") return sItem;
	return sLine + ", " + sItem;
}

// LEO: ALT-колонка правее всего hero HUD; master off оставляет исходные X=147 и Y без сдвига.
void BLI_AltInfoPosition(aref origin)
{
	float ratio=stf(Render.screen_y)/screenscaling;
	origin.x=sti(showWindow.left)+RecalculateHIcon(makeint(147*ratio));
	origin.y=0;
	if(NewHUD_GetOption("NewHUDEnabled")==0) return;
	aref signs;
	makearef(signs,objLandInterface.ManSign);
	if(!CheckAttribute(signs,"newhud.enabled")) return;
	if(sti(signs.newhud.enabled)==0) return;
	if(!CheckAttribute(signs,"newhud.portraitwidth")) return;
	if(!CheckAttribute(signs,"iconoffset1")) return;
	float diameter=stf(signs.newhud.portraitwidth);
	if(diameter<=0.0) return;

	if(!CheckAttribute(&objLandInterface,"AltInfoLayout.referenceRight")) objLandInterface.AltInfoLayout.referenceRight=1024.0;
	if(!CheckAttribute(&objLandInterface,"AltInfoLayout.referenceCenter")) objLandInterface.AltInfoLayout.referenceCenter=240.0;
	if(!CheckAttribute(&objLandInterface,"AltInfoLayout.referenceDiameter")) objLandInterface.AltInfoLayout.referenceDiameter=293.3333333;
	if(!CheckAttribute(&objLandInterface,"AltInfoLayout.gap")) objLandInterface.AltInfoLayout.gap=12.0;
	aref layout;
	makearef(layout,objLandInterface.AltInfoLayout);
	if(stf(layout.referenceDiameter)<=0.0) return;
	float right=stf(GetSubStringByNum(signs.iconoffset1,0))+(stf(layout.referenceRight)-stf(layout.referenceCenter))*diameter/stf(layout.referenceDiameter);
	int x=makeint(right+RecalculateHIcon(makeint(stf(layout.gap)*ratio)))+1;
	if(x>sti(origin.x)) origin.x=x;

	if(CheckAttribute(signs,"hudlayoutbase.iconoffset1"))
		origin.y=sti(GetSubStringByNum(signs.iconoffset1,1))-sti(GetSubStringByNum(signs.hudlayoutbase.iconoffset1,1));
}

void ModifyTextInfo()
{
	// LEO: До загрузки локации даже IsEntity(&loadedLocation) падает на неинициализированной ссылке.

	if(FindLoadedLocation() < 0) return;

	float fHtRatio = stf(Render.screen_y) / screenscaling;

	object origin;
	BLI_AltInfoPosition(&origin);
	int startX = sti(origin.x);
	int textOffsetY = sti(origin.y);
	int curY = 25;
	if(bAltInfo)
	{
		bool bHasFastReload = false;
		if(CheckAttribute(loadedLocation, "fastreload"))
		{
			bHasFastReload = true;
			string sTown = loadedLocation.fastreload;
			string curLoc = loadedLocation.id;

			string locPort = "";
			if (CheckAttribute(loadedLocation, "townsack"))
			{
				string ts = loadedLocation.townsack;
				if(ts != "PuertoPrincipe" && ts != "FortOrange" && ts != "DesMoines") locPort = ts + "_town";
				else locPort = ts + "_port";
			}

			if (CheckAttribute(pchar, "location.from_sea"))
			{
				if (curLoc == pchar.location.from_sea) locPort = curLoc;
			}

			string validLocs[15];
			int locCount = 0;
			string tmpStr = "";

			tmpStr = FormatHKT(curLoc, locPort, XI_ConvertString("HKT_Port"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 0), XI_ConvertString("HKT_Store"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 1), XI_ConvertString("HKT_Shipyard"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 2), XI_ConvertString("HKT_Tavern"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 4), XI_ConvertString("HKT_Residence"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 3), XI_ConvertString("HKT_Bank"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 6), XI_ConvertString("HKT_Church"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 9), XI_ConvertString("HKT_Brothel"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 8), XI_ConvertString("HKT_PortOffice"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			tmpStr = FormatHKT(curLoc, GetFastReloadLocByPic(sTown, 5), XI_ConvertString("HKT_Prison"));
			if(tmpStr != "") { validLocs[locCount] = tmpStr; locCount++; }

			string sLine[4];
			sLine[1] = ""; sLine[2] = ""; sLine[3] = "";

			int currentLine = 1;
			int itemsOnThisLine = 0;

			for (int i = 0; i < locCount; i++)
			{
				sLine[currentLine] = AddHKT(sLine[currentLine], validLocs[i]);
				itemsOnThisLine++;
				if (itemsOnThisLine >= 4 && currentLine < 3)
				{
					currentLine++;
					itemsOnThisLine = 0;
				}
			}

			objLandInterface.textinfo.AltInfo.align = "left";
			objLandInterface.textinfo.AltInfo.pos.x = startX;
			objLandInterface.textinfo.AltInfo.pos.y = RecalculateVIcon(makeint(curY * fHtRatio)) + textOffsetY;
			objLandInterface.textinfo.AltInfo.text  = XI_ConvertString("HKT_Title");
			curY += 20;

			for(int L = 1; L <= 3; L++)
			{
				string nodeName = "AltInfo" + L;
				objLandInterface.textinfo.(nodeName).align = "left";
				objLandInterface.textinfo.(nodeName).pos.x = startX;

				if (sLine[L] != "")
				{
					objLandInterface.textinfo.(nodeName).pos.y = RecalculateVIcon(makeint(curY * fHtRatio)) + textOffsetY;
					objLandInterface.textinfo.(nodeName).text  = sLine[L];
					curY += 20;
				}
				else
				{
					objLandInterface.textinfo.(nodeName).text = " ";
				}
			}

			curY += 10;
		}
		else
		{
			objLandInterface.textinfo.AltInfo.text  = " ";
			objLandInterface.textinfo.AltInfo1.text = " ";
			objLandInterface.textinfo.AltInfo2.text = " ";
			objLandInterface.textinfo.AltInfo3.text = " ";
			curY = 25;
		}

		bool bActiveFreight = false;
		bool bFailedFreight = false;

		if (CheckAttribute(pchar, "quest.generate_trade_quest_progress"))
		{
			if (pchar.quest.generate_trade_quest_progress == "begin") bActiveFreight = true;
			if (pchar.quest.generate_trade_quest_progress == "failed") bFailedFreight = true;
		}

		bool bShowFreight = false;
		if (bActiveFreight || bFailedFreight)
		{
			if (CheckAttribute(pchar, "CargoQuest.iTradeGoods")) bShowFreight = true;
		}

		objLandInterface.textinfo.AltInfo4.align = "left";
		objLandInterface.textinfo.AltInfo4.pos.x = startX;
		objLandInterface.textinfo.AltInfo4.pos.y = RecalculateVIcon(makeint(curY * fHtRatio)) + textOffsetY;

		curY += 20;
		curY += 10;

		if (bShowFreight)
		{
			int iGoods = sti(pchar.CargoQuest.iTradeGoods);
			int qty = sti(pchar.CargoQuest.iQuantityGoods);
			int iType = sti(pchar.CargoQuest.iOrderType);
			string sCity = pchar.CargoQuest.iTradeColony;

			int daysLeft = sti(pchar.CargoQuest.iDaysExpired) - GetQuestPastDayParam("CargoQuest");

			string sTarget = XI_ConvertString("Colony" + sCity);
			if (iType == 7) sTarget = XI_ConvertString("Freight_ToFort") + XI_ConvertString("Colony" + sCity + "Gen") + ")";
			if (iType == 5) sTarget = XI_ConvertString("Freight_ToSmugglers") + XI_ConvertString("Colony" + sCity) + ")";
			if (iType == 6) sTarget = XI_ConvertString("Freight_ToGovernor") + XI_ConvertString("Colony" + sCity + "Gen") + ")";

			string sStatus = "";

			if (bFailedFreight || daysLeft <= 0)
			{
				objLandInterface.textinfo.AltInfo4.color = argb(255, 255, 80, 80);
				if (daysLeft < 0) sStatus = XI_ConvertString("Freight_Overdue_1") + (0 - daysLeft) + XI_ConvertString("Freight_Overdue_2");
				else sStatus = XI_ConvertString("Freight_Expired");
			}
			else if (daysLeft == 1)
			{
				objLandInterface.textinfo.AltInfo4.color = argb(255, 255, 140, 40);
				sStatus = XI_ConvertString("Freight_Today");
			}
			else if (daysLeft <= 3)
			{
				objLandInterface.textinfo.AltInfo4.color = argb(255, 255, 230, 80);
				sStatus = XI_ConvertString("Freight_DaysLeft") + daysLeft + XI_ConvertString("Freight_Days") + XI_ConvertString("Freight_Urgent");
			}
			else
			{
				objLandInterface.textinfo.AltInfo4.color = argb(255, 180, 255, 180);
				sStatus = XI_ConvertString("Freight_DaysLeft") + daysLeft + XI_ConvertString("Freight_Days");
			}

			objLandInterface.textinfo.AltInfo4.text = XI_ConvertString("Freight_Label") + GetGoodsNameAlt(iGoods) + " (" + qty + XI_ConvertString("Freight_Pcs") + ") -> " + sTarget + sStatus;
		}
		else
		{
			objLandInterface.textinfo.AltInfo4.color = argb(255, 255, 150, 150);
			objLandInterface.textinfo.AltInfo4.text = XI_ConvertString("Freight_NoQuest");
		}

		bool bShowUpgrade = false;
		int upgLines = 0;
		string upgText[30];
		int upgColor[30];

		string sCurrentCity = "";
		if (CheckAttribute(loadedLocation, "townsack")) sCurrentCity = loadedLocation.townsack;
		else if (CheckAttribute(loadedLocation, "fastreload")) sCurrentCity = loadedLocation.fastreload;

		int iShip = sti(pchar.Ship.Type);
		if (iShip != SHIP_NOTUSED)
		{
			ref rShip = &RealShips[iShip];
			if (CheckAttribute(rShip, "GoodsForUpgrade"))
			{
				aref arGoods;
				makearef(arGoods, rShip.GoodsForUpgrade);
				int num = GetAttributesNum(arGoods);

				if (num > 0)
				{
					string sYardName = "";
					if (CheckAttribute(rShip, "Shipyarder"))
					{
						int idxYard = sti(rShip.Shipyarder);
						if (idxYard >= 0)
						{
							ref rYard = GetCharacter(idxYard);
							if (CheckAttribute(rYard, "city")) sYardName = " (" + XI_ConvertString("Colony" + rYard.city) + ")";
						}
					}

					int totalNeedCount = 0;
					int totalReadyCount = 0;

					bool bNeedsSomething = false;
					for (int j = 0; j < num; j++)
					{
						aref arCur = GetAttributeN(arGoods, j);
						int qtyNeed = sti(GetAttributeValue(arCur));

						if (qtyNeed > 0)
						{
							totalNeedCount++;

							if (!bNeedsSomething)
							{
								upgLines += 2;
								bNeedsSomething = true;
							}

							string sGood = GetAttributeName(arCur);
							string sGoodName = XI_ConvertString(sGood);
							int idxGood = FindGood(sGood);

							int qtyHave = 0;
							if (idxGood >= 0)
							{
								for (int c = 0; c < COMPANION_MAX; c++)
								{
									int cn = GetCompanionIndex(pchar, c);
									if (cn >= 0) qtyHave += GetCargoGoods(&characters[cn], idxGood);
								}
							}

							int iTotalInStores = 0;
							int iStoresCount = 0;
							string sStoreDetails = "";

							for (int st = 0; st < STORE_QUANTITY; st++)
							{
								ref refStoreObj = &stores[st];
								if (CheckAttribute(refStoreObj, "Storage.Goods"))
								{
									aref arGoodsStore;
									makearef(arGoodsStore, refStoreObj.Storage.Goods);
									if (CheckAttribute(arGoodsStore, sGood))
									{
										aref arGoodNode;
										makearef(arGoodNode, arGoodsStore.(sGood));
										if (CheckAttribute(arGoodNode, "quantity"))
										{
											int inStore = sti(arGoodNode.quantity);
											if (inStore > 0)
											{
												iTotalInStores += inStore;
												iStoresCount++;

												string sCityName = refStoreObj.Colony;
												string sColName = XI_ConvertString("Colony" + sCityName);
												if (sColName == "") sColName = sCityName;

												if (sCurrentCity != "" && sCityName == sCurrentCity)
												{
													sColName = "*" + sColName;
												}

												if (sStoreDetails != "") sStoreDetails += ", ";
												sStoreDetails += sColName + ": " + inStore;
											}
										}
									}
								}
							}

							if (iTotalInStores == 0 && CheckAttribute(pchar, "Storage"))
							{
								aref arStorage;
								makearef(arStorage, pchar.Storage);
								int stNum = GetAttributesNum(arStorage);
								for (int stg = 0; stg < stNum; stg++)
								{
									aref arCity = GetAttributeN(arStorage, stg);
									string sPcharCity = GetAttributeName(arCity);
									if (CheckAttribute(arCity, "Goods"))
									{
										aref arGoodsStore2;
										makearef(arGoodsStore2, arCity.Goods);
										if (CheckAttribute(arGoodsStore2, sGood))
										{
											aref arGoodNode2;
											makearef(arGoodNode2, arGoodsStore2.(sGood));
											if (CheckAttribute(arGoodNode2, "quantity"))
											{
												int inStore2 = sti(arGoodNode2.quantity);
												if (inStore2 > 0)
												{
													iTotalInStores += inStore2;
													iStoresCount++;

													string sPcharColName = XI_ConvertString("Colony" + sPcharCity);
													if (sPcharColName == "") sPcharColName = sPcharCity;

													if (sCurrentCity != "" && sPcharCity == sCurrentCity)
													{
														sPcharColName = "*" + sPcharColName;
													}

													if (sStoreDetails != "") sStoreDetails += ", ";
													sStoreDetails += sPcharColName + ": " + inStore2;
												}
											}
										}
									}
								}
							}

							string sStoreInfo = "";
							if (iTotalInStores > 0)
							{
								if (iStoresCount > 1) sStoreInfo = XI_ConvertString("UpgTracker_Stores") + iTotalInStores + 										XI_ConvertString("UpgTracker_PcsInfo") + sStoreDetails + "]";
								else sStoreInfo = " [" + sStoreDetails + "]";
							}

							int iColor = argb(255, 255, 140, 140);

							if ((qtyHave + iTotalInStores) >= qtyNeed)
							{
								iColor = argb(255, 100, 220, 255);
							}

							if (qtyHave >= qtyNeed)
							{
								iColor = argb(255, 120, 255, 120);
								totalReadyCount++;
							}

							upgText[upgLines] = " - " + sGoodName + ": " + qtyHave + " / " + qtyNeed + sStoreInfo;
							upgColor[upgLines] = iColor;
							upgLines++;

							if (upgLines >= 29) break;
						}
					}

					if (bNeedsSomething)
					{
						bShowUpgrade = true;

						upgText[0] = XI_ConvertString("UpgTracker_Title") + sYardName + " [" + totalReadyCount + "/" + totalNeedCount + "]:";
						upgColor[0] = argb(255, 255, 230, 80);

						upgText[1] = XI_ConvertString("UpgTracker_Legend");
						upgColor[1] = argb(255, 180, 180, 160);
					}
				}
			}
		}

		for (int k = 0; k < 30; k++)
		{
			string sNode = "UpgInfo" + k;
			if (bShowUpgrade && k < upgLines)
			{
				objLandInterface.textinfo.(sNode).align = "left";
				objLandInterface.textinfo.(sNode).pos.x = startX;
				objLandInterface.textinfo.(sNode).pos.y = RecalculateVIcon(makeint((curY + k * 20) * fHtRatio)) + textOffsetY;
				objLandInterface.textinfo.(sNode).font = "interface_normal";
				objLandInterface.textinfo.(sNode).scale = 1.0 * fHtRatio;
				objLandInterface.textinfo.(sNode).shadow = 1;
				objLandInterface.textinfo.(sNode).refreshable = true;
				objLandInterface.textinfo.(sNode).color = upgColor[k];
				objLandInterface.textinfo.(sNode).text = upgText[k];
			}
			else
			{
				if (CheckAttribute(&objLandInterface, "textinfo." + sNode))
				{
					objLandInterface.textinfo.(sNode).text = " ";
				}
			}
		}

		SendMessage(&objLandInterface, "l", MSG_BATTLE_LAND_REFRESH_HUD);
	}
	else
	{
		objLandInterface.textinfo.AltInfo.text  = " ";
		objLandInterface.textinfo.AltInfo1.text = " ";
		objLandInterface.textinfo.AltInfo2.text = " ";
		objLandInterface.textinfo.AltInfo3.text = " ";
		objLandInterface.textinfo.AltInfo4.text = " ";

		for (int m = 0; m < 30; m++)
		{
			string sNodeClear = "UpgInfo" + m;
			if (CheckAttribute(&objLandInterface, "textinfo." + sNodeClear))
			{
				objLandInterface.textinfo.(sNodeClear).text = " ";
			}
		}

		SendMessage(&objLandInterface, "l", MSG_BATTLE_LAND_REFRESH_HUD);
	}
}

void RefreshChargeTime()
{
	i = Dead_FindCloseBody();
	if (i != -1)
	{
		Log_SetActiveAction("DeadBox");
		dead = true;
	}
	else
	{
		if (dead)
		{
			Log_SetActiveAction("Nothing");
			dead = false;
		}
	}
}

bool FindUserIcon(string id,ref uiref)
{
	aref arroot,ar; makearef(arroot,objLandInterface.UserIcons);
	int iMax = GetAttributesNum(arroot);
	for(int i=0; i<iMax; i++)
	{
		ar = GetAttributeN(arroot,i);
		if(ar.name != id) continue;
		uiref = ar;
		return true;
	}
	return false;
}

void BLI_SetMessageParameters()
{
	objLandInterface.MessageIcons.IconWidth = RecalculateHIcon(64);
	objLandInterface.MessageIcons.IconHeight = RecalculateVIcon(24);
	objLandInterface.MessageIcons.IconDist = RecalculateVIcon(2);
	objLandInterface.MessageIcons.IconBottom = sti(showWindow.bottom)-RecalculateHIcon(80+40);
	objLandInterface.MessageIcons.IconMaxQuantity = 4;
	objLandInterface.MessageIcons.BlendTime = 3.0;
	objLandInterface.MessageIcons.FallSpeed = 22.0;
	objLandInterface.MessageIcons.argbHighBlind = argb(255,128,128,128);
	objLandInterface.MessageIcons.argbLowBlind = argb(255,68,68,68);
	objLandInterface.MessageIcons.BlindUpTime = 0.5;
	objLandInterface.MessageIcons.BlindDownTime = 1.0;
	objLandInterface.MessageIcons.texture = "battle_interface\MessageIcons.dds";
	objLandInterface.MessageIcons.TexHSize = 2;
	objLandInterface.MessageIcons.TexVSize = 2;
}

void BLI_UpdateConsumablesHUD()
{
	int khWant = 0;
	if(CheckAttribute(&InterfaceStates,"ConsumablesHUD") && sti(InterfaceStates.ConsumablesHUD)==1) khWant = 1;
	int khRebuild = 0;
	float khCurScale = 1.0;
	float khCurPos = 0.5;
	int khWantAlt = 0;
	int khCurAlt = 0;
	if(sti(objLandInterface.ConsumablesBuilt) != khWant) khRebuild = 1;

	if(khWant == 1 && sti(objLandInterface.ConsumablesBuilt) == 1)
	{
		int khContext = 0;
		if(CheckAttribute(&InterfaceStates,"ConsumablesHUDContext") && sti(InterfaceStates.ConsumablesHUDContext)==1) khContext = 1;
		if(sti(objLandInterface.ConsContextApplied) != khContext) khRebuild = 1;
		int khVisible = BLI_UpdateConsumablesCounts();
		if(khContext == 1 && sti(objLandInterface.ConsVisibleMask) != khVisible) khRebuild = 1;
		if(sti(objLandInterface.ConsAmmoPic) != Cons_AmmoPicNum(LAi_GetCharacterBulletType(pchar))) khRebuild = 1;
		if(sti(objLandInterface.ConsPowderPic) != Cons_PowderPicNum(LAi_GetCharacterGunpowderType(pchar))) khRebuild = 1;
		if(sti(objLandInterface.ConsumablesCfg) != (sti(InterfaceStates.ConsumablesNoTop)*2 + sti(InterfaceStates.ConsumablesNoBottom))) khRebuild = 1;
		if(CheckAttribute(&InterfaceStates,"ConsumablesHUDScale")) khCurScale = stf(InterfaceStates.ConsumablesHUDScale);
		if(khCurScale < 0.5) khCurScale = 0.5;
		if(khCurScale > 2.0) khCurScale = 2.0;
		if(stf(objLandInterface.ConsScaleApplied) != khCurScale) khRebuild = 1;
		if(CheckAttribute(&InterfaceStates,"ConsumablesHUDPos")) khCurPos = stf(InterfaceStates.ConsumablesHUDPos);
		if(khCurPos < 0.0) khCurPos = 0.0;
		if(khCurPos > 1.0) khCurPos = 1.0;
		if(stf(objLandInterface.ConsPosApplied) != khCurPos) khRebuild = 1;
		if(sti(InterfaceStates.AltIntIcons) == 1) khWantAlt = 1;
		if(AltIntIcons) khCurAlt = 1;
		if(khWantAlt != khCurAlt)
		{
			AltIntIcons = (khWantAlt == 1);
			khRebuild = 1;
		}
	}
	if(khRebuild)
	{
		if(khWant==1) BLI_BuildConsumablesHUD();
		else BLI_RemoveConsumablesHUD();
		objLandInterface.ConsumablesBuilt = khWant;
		SendMessage(&objLandInterface, "l", MSG_BATTLE_LAND_REFRESH_HUD);
	}
}

void BLI_UpdateObjectData()
{
	bGameLogSkip = 0; // LEO: Сбрасываем застрявший флаг пропуска журнала, иначе новые записи не отображаются.
	if(CheckAttribute(pchar, "radarPOIDirty")) // LEO: После закрытия сундука itemsbox помечает POI устаревшими; обновляем в этом контексте.
	{
		DeleteAttribute(pchar, "radarPOIDirty");
		LandRadar_BuildPOI();
	}

	LandRadar_RetagCombat();
	LandRadar_ReconcileVisibility();
	bool bIsRiskAlarm = LAi_group_IsActivePlayerAlarm();
	if(sti(objLandInterface.data.Alarm)!=bIsRiskAlarm) BLI_RefreshCommandMenu();
	objLandInterface.data.Alarm = bIsRiskAlarm;
	BLI_UpdateConsumablesHUD();

	aref ar,arItm;
	int i,cn;
	ref curCh;
	string attrName;
	for(i=0; i<=MAX_NUM_FIGHTERS; i++)
	{
		attrName = "id"+i;
		cn = GetOfficersIndex(pchar,i);
		if(cn<0 || cn>=TOTAL_CHARACTERS)
		{
			DeleteAttribute(&objLandInterface,"data.icons."+attrName);
			continue;
		}
		if(!IsEntity(&Characters[cn]))
		{
			DeleteAttribute(&objLandInterface,"data.icons."+attrName);
			continue;
		}
		curCh = GetCharacter(cn);
		makearef(ar,objLandInterface.data.icons.(attrName));
		SetCharacterIconData(cn,ar);
	}

	if(LAi_IsBoardingProcess() && !dialogRun && questMovieProcess<=0 && sti(InterfaceStates.BattleShow.Command)!=0)
	{
		int brdI;
		int brdIx;
		ref brdC;

		float brdMyHP = makefloat(boarding_player_crew);
		int brdMyAlive = boarding_player_crew;
		if(!LAi_IsDead(GetMainCharacter())) { brdMyHP = brdMyHP + LAi_GetCharacterRelHP(GetMainCharacter()); brdMyAlive = brdMyAlive + 1; }
		for(brdI = 0; brdI < LAi_numloginedcharacters; brdI++)
		{
			brdIx = LAi_loginedcharacters[brdI];
			if(brdIx < 0) continue;
			if(brdIx == GetMainCharacterIndex()) continue;
			makeref(brdC, Characters[brdIx]);
			if(!CheckAttribute(brdC, "chr_ai.group")) continue;
			if(brdC.chr_ai.group != LAI_GROUP_PLAYER) continue;
			if(LAi_IsDead(brdC)) continue;
			brdMyHP = brdMyHP + LAi_GetCharacterRelHP(brdC);
			brdMyAlive = brdMyAlive + 1;
		}
		int brdMyStart = boarding_player_crew_start + 1; // LEO: ГГ не входит в исходную численность *_start.
		if(brdMyStart < 1) brdMyStart = 1;

		float brdEnHP = makefloat(boarding_enemy_crew);
		int brdEnAlive = boarding_enemy_crew;
		for(brdI = 0; brdI < LAi_numloginedcharacters; brdI++)
		{
			brdIx = LAi_loginedcharacters[brdI];
			if(brdIx < 0) continue;
			makeref(brdC, Characters[brdIx]);
			if(!CheckAttribute(brdC, "chr_ai.group")) continue;
			if(brdC.chr_ai.group != LAI_GROUP_BRDENEMY) continue;
			if(LAi_IsDead(brdC)) continue;
			brdEnHP = brdEnHP + LAi_GetCharacterRelHP(brdC);
			brdEnAlive = brdEnAlive + 1;
		}
		int brdEnStart = boarding_enemy_crew_start;
		if(brdEnStart < 1) brdEnStart = 1;

		float brdMyFrac = brdMyHP / makefloat(brdMyStart);
		if(brdMyFrac > 1.0) brdMyFrac = 1.0;
		if(brdMyFrac < 0.0) brdMyFrac = 0.0;
		float brdEnFrac = brdEnHP / makefloat(brdEnStart);
		if(brdEnFrac > 1.0) brdEnFrac = 1.0;
		if(brdEnFrac < 0.0) brdEnFrac = 0.0;

		DrawCharacterHP(brdMyFrac, brdEnFrac, brdMyAlive, brdEnAlive);
	}
	else
	{
		if(bYesBoardStatus)
		{
			bYesBoardStatus = false;
			DeleteClass(&IBoardingStatus);
			DeleteAttribute(&IBoardingStatus,"");
		}
	}

	PostEvent("evntBLI_Update",BLI_UPDATE_PERIOD);
}

void BLI_SetPossibleCommands()
{
	int chrMode = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"IsFightMode");

	bool bTmpBool;
	bool bTmpBool2;
	bool bTmpBool3;
	bool bUseCommand = false;
	int i;
	aref rootcom,curcom;
	makearef(rootcom,objLandInterface.Commands);
	int iMax = GetAttributesNum(rootcom);
	for(i=0; i<iMax; i++)
	{
		curcom = GetAttributeN(rootcom,i);
		curcom.enable = false;
	}

	if(chrMode==0)
	{
		bTmpBool = true;
		if(LAi_group_IsActivePlayerAlarm()) bTmpBool = false;
		if(!LAi_IsCharacterControl(pchar)) bTmpBool = false;
		if(bDisableFastReload)	bTmpBool = false;
		if(!IsEnableFastTravel()) bTmpBool = false;
		if (chrDisableReloadToLocation) bTmpBool = false;
		if (!CheckAttribute(loadedLocation,"fastreload")) bTmpBool = false;
		if (bTmpBool)
		{
		    string sNation = Colonies[FindColony(loadedLocation.fastreload)].nation;
			if (sNation != "none")
			{
				i = sti(sNation);
				bTmpBool = (GetNationRelation2MainCharacter(i) == RELATION_ENEMY) || GetRelation2BaseNation(i) == RELATION_ENEMY;
				string SNK = SKILL_SNEAK;
				int skillSNK = sti(pchar.Skill.(SNK));
				bTmpBool2 = skillSNK >= 50 && CheckNationLicence(i);
				bTmpBool3 = GetNationRelation2MainCharacter(i) != RELATION_ENEMY;
				if (bTmpBool2 && bTmpBool3) bTmpBool = false;
				if (bTmpBool && (i != PIRATE))
				{
					bTmpBool = false;
				}
				else
				{
		    		bTmpBool = true;
				}
			}
		}
		objLandInterface.Commands.FastReload.enable	= bTmpBool;
		i = -1;
		if (CheckAttribute(pchar,"dialogready")) i=sti(pchar.dialogready);
		if (i>=0)
		{
			objLandInterface.Commands.DialogStart.enable	= true;
			if (isOfficerInShip(&Characters[i], true) || CheckAttribute(&Characters[i], "IsCompanionClone"))
			{
				objLandInterface.Commands.ItemsChange.enable = true;
			}
		}
		else
		{
		    i = LAi_FindNearestVisCharacter(pchar, 1);

		    if( i>0 ) { if( IsOfficer(GetCharacter(i)) && IsCharacterPerkOn(pchar, "SharedExperience") ) objLandInterface.Commands.ItemsChange.enable = true; }
		}
		bUseCommand = true;
	}
	if (objLandInterface.Commands.DialogStart.enable == false && LAi_IsCharacterControl(pchar) && !LAi_IsFightMode(pchar) && !LAi_grp_alarmactive && !chrDisableReloadToLocation)
	{
	objLandInterface.Commands.TalkSelf.enable = true;
    	bUseCommand = true;
    }
	if (!LAi_IsFightMode(pchar) && !LAi_grp_alarmactive )
	{
	objLandInterface.Commands.Craft.enable = true;
    	bUseCommand = true;
    }
	if(GetCharacterPerkUsing(pchar,"Rush") && PChar.location != "FencingTown_Arena" && PChar.location != "FencingTown_ExitTown")
	{
		objLandInterface.Commands.ActivateRush.enable = true;
		bUseCommand = true;
	}
	if( CanBeUseItem(pchar) )
	{
		objLandInterface.Commands.ItemsUse.enable = true;
		bUseCommand = true;
	}
	if( CanBeUseFood(pchar) )
	{
		objLandInterface.Commands.UseFood.enable = true;
		bUseCommand = true;
	}
	if( CanBeUseSmoke(pchar) )
	{
		objLandInterface.Commands.Smoke.enable = true;
		bUseCommand = true;
	}
	if(CheckAttribute(pchar,"boxname"))
	{
		objLandInterface.Commands.UseBox.enable	= true;
		bUseCommand = true;
	}
	if( CheckAttribute(pchar,"activeItem") && pchar.activeItem!="" )
	{
		bUseCommand = true;
		if( CheckAttribute(pchar,"activeItem.pick") && pchar.activeItem.pick=="1" )
		{
			objLandInterface.Commands.TakeItem.enable = true;
		}
		else
		{
			objLandInterface.Commands.PlaceItem.enable = true;
		}
	}
	if( chrWaitReloadLocator != "" )
	{
		bUseCommand = true;
		objLandInterface.Commands.OutDoor.enable = true;
	}
    i = Dead_FindCloseBody();
    if (i != -1)
    {
        objLandInterface.Commands.DeadBox.enable	= true;
		bUseCommand = true;
    }
    if (isShipInside(pchar.location) && !chrDisableReloadToLocation)
    {
       bUseCommand = true;
       objLandInterface.Commands.Exit_Deck.enable = true;
    }

	if(CheckAttribute(loadedLocation, "CabinType") && !chrDisableReloadToLocation)
	{
		if(CheckShipSituation_GenQuest())
		{
			bUseCommand = true;
			objLandInterface.Commands.Exit_Deck.enable = true;
		}
	}
    if (pchar.location == "Ship_deck")
    {
		if (CheckQuestAttribute("Munity", "Deads"))
		{
			bUseCommand = true;
			objLandInterface.Commands.Exit_Deck.enable = true;
		}
    }
	if(!bUseCommand)
	{
		SendMessage(&objLandInterface,"llllll",BI_IN_SET_COMMAND_MODE,0,-1,-1,-1,-1);
	}
}

void procFindDialogChar()
{
	int dlgChar = -1;
	if(!LAi_IsBoardingProcess())
		dlgChar = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"FindDialogCharacter");

	int oldDlgChar = -1;
	if(CheckAttribute(pchar,"dialogready")) oldDlgChar = sti(pchar.dialogready);

	if(dlgChar!=oldDlgChar)
	{
		pchar.dialogready = dlgChar;
		LI_CareUpdateCommandList();
	}
	PostEvent("evntFindDialogChar",1000);
}

void BLI_ChrEnterToLocation()
{
	aref loc = GetEventData();
	aref chr = GetEventData();
	string group = GetEventData();
	string locator = GetEventData();
	aref locList,locCur;
	int i,n;

	if(sti(chr.index) != nMainCharacterIndex) return;

	if(group=="reload")
	{
		makearef(locList,loc.reload);
		n = GetAttributesNum(locList);
		for(i=0;i<n;i++)
		{
			locCur = GetAttributeN(locList,i);
			if(locCur.name == locator) break;
		}
		if(i<n)
		{
			i = FindLocation(locCur.go);
			if(i>=0)
			{
				if(CheckAttribute(&locations[i],"id.label"))
				{
					Log_SetStringToLog( LanguageConvertString(g_LocLngFileID,locations[i].id.label) );
				}
			}
		}
		LI_CareUpdateCommandList();
	}
}

void BLI_ChrExitFromLocation()
{
	LI_CareUpdateCommandList();
}

void LI_Reload()
{
	Event("Control Activation","s","ChrAction");
}

bool SetUsedPotionIcons()
{
	aref arItm, ar, uiref;
	int i, itmIdx, nq;

	bool bUsed = false;

	makearef(uiref,objLandInterface.UserIcons);
	nq = GetAttributesNum(uiref);
	for(i=0; i<nq; i++)
	{
		ar = GetAttributeN(uiref,i);
		if( CheckAttribute(ar,"potion") ) {
			DeleteAttribute(uiref,GetAttributeName(ar));
			nq--; i--;
		}
	}

	string UI_name;
	int UI_idx = 0;

	itmIdx = FindPotionFromChr(pchar, &arItm, 0);
	while(itmIdx>=0)
	{
		if(EnablePotionUsing(PChar, arItm) || EnableAntidoteUsing(PChar, arItm))
		{
			UI_idx++;
			UI_name = "potion"+UI_idx;
			objLandInterface.UserIcons.(UI_name).enable = true;
			objLandInterface.UserIcons.(UI_name).pic = GetPotionPicture(arItm);
            objLandInterface.UserIcons.(UI_name).selpic = GetPotionPicture(arItm) - 16;
			objLandInterface.UserIcons.(UI_name).tex = GetPotionTexture(arItm);
			objLandInterface.UserIcons.(UI_name).name = UI_name;
			objLandInterface.UserIcons.(UI_name).note = GetConvertStr(arItm.name, "ItemsDescribe.txt");
			objLandInterface.UserIcons.(UI_name).potion = arItm.id;
			bUsed = true;
		}
		itmIdx = FindPotionFromChr(pchar, &arItm, itmIdx+1);
	}
	return bUsed;
}

bool SetUsedFoodIcons()
{
	aref arItm, ar, uiref;
	int i, itmIdx, nq;

	bool bUsed = false;

	makearef(uiref,objLandInterface.UserIcons);
	nq = GetAttributesNum(uiref);
	for(i=0; i<nq; i++)
	{
		ar = GetAttributeN(uiref,i);
		if( CheckAttribute(ar,"Food") ) {
			DeleteAttribute(uiref,GetAttributeName(ar));
			nq--; i--;
		}
	}

	string UI_name;
	int UI_idx = 0;

	itmIdx = FindFoodFromChr(pchar, &arItm, 0);
	while(itmIdx>=0)
	{
		if( EnableFoodUsing(pchar,arItm) )
		{
			UI_idx++;
			UI_name = "Food"+UI_idx;
			objLandInterface.UserIcons.(UI_name).enable = true;
			objLandInterface.UserIcons.(UI_name).pic = GetFoodPicture(arItm);
            		objLandInterface.UserIcons.(UI_name).selpic = GetFoodSelectPicture(arItm);
			objLandInterface.UserIcons.(UI_name).tex = GetFoodTexture(arItm);
			objLandInterface.UserIcons.(UI_name).name = UI_name;
			objLandInterface.UserIcons.(UI_name).note = GetConvertStr(arItm.name, "ItemsDescribe.txt");
			objLandInterface.UserIcons.(UI_name).Food = arItm.id;
			bUsed = true;
		}
		itmIdx = FindFoodFromChr(pchar, &arItm, itmIdx+1);
	}

	return bUsed;
}

bool SetUsedSmokeIcons()
{
	aref arItm, ar, uiref;
	int i, itmIdx, nq;

	bool bUsed = false;

	makearef(uiref,objLandInterface.UserIcons);
	nq = GetAttributesNum(uiref);
	for(i=0; i<nq; i++)
	{
		ar = GetAttributeN(uiref,i);
		if( CheckAttribute(ar,"Smoke") ) {
			DeleteAttribute(uiref,GetAttributeName(ar));
			nq--; i--;
		}
	}

	string UI_name;
	int UI_idx = 0;

	itmIdx = FindSmokeFromChr(pchar, &arItm, 0);
	while(itmIdx>=0)
	{
		if( EnableSmokeUsing(pchar,arItm) )
		{
			UI_idx++;
			UI_name = "Smoke"+UI_idx;
			objLandInterface.UserIcons.(UI_name).enable = true;
			objLandInterface.UserIcons.(UI_name).pic = GetSmokePicture(arItm);
			objLandInterface.UserIcons.(UI_name).selpic = GetSmokeSelectPicture(arItm);
			objLandInterface.UserIcons.(UI_name).tex = GetSmokeTexture(arItm);
			objLandInterface.UserIcons.(UI_name).name = UI_name;
			objLandInterface.UserIcons.(UI_name).note = GetConvertStr(arItm.name, "ItemsDescribe.txt");
			objLandInterface.UserIcons.(UI_name).Smoke = arItm.id;
			bUsed = true;
		}
		itmIdx = FindSmokeFromChr(pchar, &arItm, itmIdx+1);
	}

	return bUsed;
}

bool SetReloadIcons()
{
	int i, nq, locidx;
	aref ar, curloc, locref, locList, uiref;

	bool bUse = false;

	makearef(uiref,objLandInterface.UserIcons);
	nq = GetAttributesNum(uiref);
	for(i=0; i<nq; i++)
	{
		ar = GetAttributeN(uiref,i);
		if( CheckAttribute(ar,"location") ) {
			DeleteAttribute(uiref,GetAttributeName(ar));
			nq--;
			i--;
		}
	}

	int idxloc = FindLoadedLocation();
	string str1,str2,fastLocName;
	string outGroupName = "";
	if(idxloc>=0)
	if(CheckAttribute(&Locations[idxloc],"fastreload"))
	{
		outGroupName = Locations[idxloc].fastreload;

		if( CheckAttribute(&objFastReloadTable,"table."+outGroupName) )
		{
			makearef(locList,objFastReloadTable.table.(outGroupName));
			nq = GetAttributesNum(locList);
					objLandInterface.UserIcons.port.enable = true;
                    objLandInterface.UserIcons.port.pic = 26;
                    objLandInterface.UserIcons.port.selpic = 10;
                    objLandInterface.UserIcons.port.tex = 1;
                    objLandInterface.UserIcons.port.name = "reloadtoport";
                    string townsack = Locations[idxloc].townsack;
                    if(townsack != "PuertoPrincipe" && townsack != "FortOrange" && townsack != "DesMoines") objLandInterface.UserIcons.port.location = townsack+"_town";
					else objLandInterface.UserIcons.port.location = townsack+"_port";
                    objLandInterface.UserIcons.port.locator = "reload1";
                    objLandInterface.UserIcons.port.note = GetNodeForIcon(1, 10);
                    bUse = true;
			for(i=0; i<nq; i++)
			{
				curloc = GetAttributeN(locList,i);
				if(!CheckFastJump(Locations[idxloc].id, curloc.location) )
				{
					continue;
				}
				fastLocName = "fr_"+(i+1);
				objLandInterface.UserIcons.(fastLocName).enable = true;
				objLandInterface.UserIcons.(fastLocName).pic = sti(curloc.pic)+16;
				objLandInterface.UserIcons.(fastLocName).selpic = curloc.pic;
				objLandInterface.UserIcons.(fastLocName).tex = 1+sti(curloc.tex);
				objLandInterface.UserIcons.(fastLocName).name = "reload"+i;
				objLandInterface.UserIcons.(fastLocName).location = curloc.location;
				objLandInterface.UserIcons.(fastLocName).note = GetNodeForIcon(1+sti(curloc.tex), sti(curloc.pic));
				if(!CheckAttribute(Locations[idxloc],"islandId")) Locations[idxloc].islandId = 0;
				if (HasSubStr(objLandInterface.UserIcons.(fastLocName).note,GetConvertStrWithReplace("Variable_LandInterface_11", "Battle_Interface.txt", "#space#", " ")))
				{
					objLandInterface.UserIcons.(fastLocName).pic = 51;
					objLandInterface.UserIcons.(fastLocName).selpic = 35;
					objLandInterface.UserIcons.(fastLocName).tex = 0;
				}
				if (Locations[idxloc].islandId == "Caiman")
				{
					if (objLandInterface.UserIcons.(fastLocName).note == GetConvertStrWithReplace("Variable_LandInterface_12", "Battle_Interface.txt", "#space#", " "))
					{
						objLandInterface.UserIcons.(fastLocName).pic = 50;
						objLandInterface.UserIcons.(fastLocName).selpic = 34;
						objLandInterface.UserIcons.(fastLocName).tex = 0;
						continue;
					}
					if (objLandInterface.UserIcons.(fastLocName).note == GetConvertStrWithReplace("Variable_LandInterface_13", "Battle_Interface.txt", "#space#", " "))
					{
						objLandInterface.UserIcons.(fastLocName).pic = 49;
						objLandInterface.UserIcons.(fastLocName).selpic = 33;
						objLandInterface.UserIcons.(fastLocName).tex = 0;
						continue;
					}
				}
				if (Locations[idxloc].islandId == "Curacao" || Locations[idxloc].islandId == "SentMartin" || Locations[idxloc].islandId == "Jamaica") // Gregg: + Ямайка (контора ГВИК в Форт-Оранже использует ту же нестандартную иконку)
				{
					if (objLandInterface.UserIcons.(fastLocName).note == GetConvertStrWithReplace("Variable_LandInterface_14", "Battle_Interface.txt", "#space#", " "))
					{
						objLandInterface.UserIcons.(fastLocName).pic = 53;
						objLandInterface.UserIcons.(fastLocName).selpic = 37;
						objLandInterface.UserIcons.(fastLocName).tex = 0;
					}
					if (objLandInterface.UserIcons.(fastLocName).note == GetConvertStrWithReplace("Variable_LandInterface_18", "Battle_Interface.txt", "#space#", " "))
					{
						objLandInterface.UserIcons.(fastLocName).pic = 54;
						objLandInterface.UserIcons.(fastLocName).selpic = 38;
						objLandInterface.UserIcons.(fastLocName).tex = 0;
					}
					continue;
				}
				if (Locations[idxloc].islandId == "Barbados")
				{
					if (objLandInterface.UserIcons.(fastLocName).note == GetConvertStrWithReplace("Variable_LandInterface_15", "Battle_Interface.txt", "#space#", " "))
					{
						objLandInterface.UserIcons.(fastLocName).pic = 50;
						objLandInterface.UserIcons.(fastLocName).selpic = 34;
						objLandInterface.UserIcons.(fastLocName).tex = 0;
						continue;
					}
				}
				if (Locations[idxloc].islandId == "Cuba")
				{
					if (objLandInterface.UserIcons.(fastLocName).note == GetConvertStrWithReplace("Variable_LandInterface_16", "Battle_Interface.txt", "#space#", " "))
					{
						objLandInterface.UserIcons.(fastLocName).pic = 52;
						objLandInterface.UserIcons.(fastLocName).selpic = 36;
						objLandInterface.UserIcons.(fastLocName).tex = 0;
						continue;
					}
				}
				if (Locations[idxloc].islandId == "LostShipsCity")
				{
					if (objLandInterface.UserIcons.(fastLocName).note == GetConvertStrWithReplace("Variable_LandInterface_17", "Battle_Interface.txt", "#space#", " "))
					{
						objLandInterface.UserIcons.(fastLocName).pic = 54;
						objLandInterface.UserIcons.(fastLocName).selpic = 38;
						objLandInterface.UserIcons.(fastLocName).tex = 0;
						continue;
					}
				}
				bUse = true;
			}
		}
	}
	int idLngFile = LanguageOpenFile("commands_name.txt");

	return bUse;
}

void SetOfficerTexture(int officerIdx)
{
	aref ar; makearef(ar,objLandInterface.Parameters);
	string attrName = "iconTexture"+officerIdx;

	int cn = GetOfficersIndex(pchar,officerIdx);
	if(cn<0) {
		DeleteAttribute(ar,attrName);
		ar.(attrName) = "";
	} else {
		ar.(attrName) = "BATTLE_INTERFACE\PORTRAITS\face_"+Characters[cn].FaceID+".tga";
	}
}

void BLI_UpdateOfficers()
{
	for(int i=0;i<=MAX_NUM_FIGHTERS;i++) {
		SetOfficerTexture(i);
	}
	for(i=0;i<=MAX_NUM_FIGHTERS;i++) {
		string sTexture;
		sTexture = "iconTexture" + i;
		SendMessage(&objLandInterface, "lls", MSG_BATTLE_LAND_SET_ICONTEX, i, objLandInterface.Parameters.(sTexture));
	}
	BLI_UpdateObjectData();
	LI_CareUpdateCommandList();
	SendMessage(&objLandInterface,"ls",MSG_BATTLE_LAND_MAKE_COMMAND,"cancel");
}

int GetPotionPicture(aref arItm)
{
	if( CheckAttribute(arItm,"potion.pic") ) {
		return sti(arItm.potion.pic);
	}
	trace("Item "+arItm.id+ " hav`t POTION.PIC attribute");
	return 0;
}

int GetPotionTexture(aref arItm)
{
	if( !CheckAttribute(arItm,"potion.tex") ) {
		trace("Item "+arItm.id+ " hav`t POTION.TEX attribute");
		return 0;
	}
	return sti(arItm.potion.tex);
}

int GetFoodPicture(aref arItm)
{
	if( CheckAttribute(arItm,"Food.pic") ) {
		return sti(arItm.Food.pic);
	}
	return 0;
}

int GetFoodSelectPicture(aref arItm)
{
	if( CheckAttribute(arItm,"Food.selpic") ) {
		return sti(arItm.Food.selpic);
	}
	return 0;
}

int GetFoodTexture(aref arItm)
{
	if( !CheckAttribute(arItm,"Food.tex") ) {
		return 0;
	}
	return sti(arItm.Food.tex);
}

int GetSmokePicture(aref arItm)
{
	if( CheckAttribute(arItm,"Smoke.pic") ) {
		return sti(arItm.Smoke.pic);
	}
	return 0;
}

int GetSmokeSelectPicture(aref arItm)
{
	if( CheckAttribute(arItm,"Smoke.selpic") ) {
		return sti(arItm.Smoke.selpic);
	}
	return 0;
}

int GetSmokeTexture(aref arItm)
{
	if( !CheckAttribute(arItm,"Smoke.tex") ) {
		return 0;
	}
	return sti(arItm.Smoke.tex);
}

bool CanBeUseItem(ref chref)
{
	for(int i=0; i<ITEMS_QUANTITY; i++)
	{
		if( CheckAttribute(&Items[i],"potion") && CheckAttribute(chref,"items."+Items[i].id) ) {
			if( EnablePotionUsing(chref,&Items[i]) ) {return true;}
		}
	}
	return false;
}

bool CanBeUseFood(ref chref)
{
	for(int i=0; i<ITEMS_QUANTITY; i++)
	{
		if( CheckAttribute(&Items[i],"Food") && CheckAttribute(chref,"items."+Items[i].id) ) {
			if( EnableFoodUsing(chref,&Items[i]) ) {return true;}
		}
	}
	return false;
}

bool CanBeUseSmoke(ref chref)
{
	if(sti(chref.index) != GetMainCharacterIndex()) return false;
	for(int i=0; i<ITEMS_QUANTITY; i++)
	{
		if( CheckAttribute(&Items[i],"Smoke") && CheckAttribute(chref,"items."+Items[i].id) ) {
			if( EnableSmokeUsing(chref,&Items[i]) ) {return true;}
		}
	}
	return false;
}
object objIconsNote;
string GetNodeForIcon(int nTex, int nPic)
{
	string attrName = nTex+"x"+nPic;
	if( CheckAttribute(objIconsNote,attrName) ) return objIconsNote.(attrName);
	return "";
}

void LI_ProcessControlPress()
{
	string ControlName = GetEventData();

	switch(ControlName)
	{
		case "hk_Cure":
			if( LAi_GetCharacterRelHP(pchar) < 1.0 )
			{
				string btl = FindHealthForCharacter(pchar);
				if (btl != "") DoCharacterUsedItem(pchar, btl);
			}
		break;

		case "LICommandsActivate":
			if (CheckAttribute(pchar, "LandTacOn")) break; // Gregg: в тактической паузе СКМ = вращение камеры, не команда-меню
			PlaySound("interface\ok.wav");
			if(bAltInfo)
			{
				bAltInfo = false;
				ModifyTextInfo();
			}
		break;
	}
}

void BLI_RefreshCommandMenu()
{
	SendMessage( &objLandInterface,"l",BI_MSG_CARE_COMMANDLIST_UPDATE );
}
void LI_CareUpdateCommandList()
{
	SendMessage( &objLandInterface,"l",BI_MSG_CARE_COMMANDLIST_UPDATE );
}

void SetCharacterIconData(int chrindex, aref arData)
{
	ref chref = GetCharacter(chrindex);
	arData.chrindex = chrindex;
	arData.health = LAi_GetCharacterRelHP(chref);
	arData.energy = LAi_GetCharacterRelEnergy(chref);
	NewHUD_SetCharacterExtras(chref,arData);
	if(CheckAttribute(chref, "chr_ai.hp_bottle.0")) {
		int iQueryIndex = 0;
		string sQueryIndex = ""+iQueryIndex;
		float ht = 0;
		while (CheckAttribute(chref, "chr_ai.hp_bottle."+sQueryIndex)) {
			ht += stf(chref.chr_ai.hp_bottle.(sQueryIndex));
			iQueryIndex++;
			sQueryIndex = ""+iQueryIndex;
		}
		ht = (ht + stf(chref.chr_ai.hp))/LAi_GetCharacterMaxHP(chref);
		if(ht > 1.0) {
				ht = 1.0;
		}
		arData.healthtemp = ht;
	}
	else {
		arData.healthtemp = 0.0;
	}

	if(CheckAttribute(chref, "chr_ai.swift")) {
		arData.energytemp = stf(chref.chr_ai.curen)/LAi_GetCharacterMaxEnergy(chref);
	}
	else if (CheckAttribute(chref, "chr_ai.FoodEnergy.0")) {
		iQueryIndex = 0;
		sQueryIndex = ""+iQueryIndex;
		ht = 0;
		while (CheckAttribute(chref, "chr_ai.FoodEnergy."+sQueryIndex)) {
			ht += stf(chref.chr_ai.FoodEnergy.(sQueryIndex));
			iQueryIndex++;
			sQueryIndex = ""+iQueryIndex;
		}
		ht = (ht + stf(chref.chr_ai.energy))/LAi_GetCharacterMaxEnergy(chref);
		if(ht > 1.0) {
				ht = 1.0;
		}
		arData.energytemp = ht;
	}
	else {
	 	arData.energytemp = 0.0;
	}

	arData.shootMax = LAi_GetCharacterChargeQuant(chref);
	arData.shootCur = LAi_GetCharacterChargeCur(chref);
	arData.gunprogress = LAi_GetGunChargeProgress(chref);
	if( CheckAttribute(chref,"FaceId") ) {
		if (sti(InterfaceStates.InterfaceSelection) != KRUGLYI) arData.texture = "battle_interface\portraits_csp\face_"+chref.FaceID+".tga";
		else arData.texture = "battle_interface\portraits\face_"+chref.FaceID+".tga";
		if( CheckAttribute(chref,"FaceUV") ) {
			arData.uv = chref.FaceUV;
		} else {
			arData.uv = "0.0,0.0,1.0,1.0";
		}
	} else {
		arData.texture = "battle_interface\two.dds";
		if( chrindex == 1 ) {
			arData.uv = "0.5,0.0,1.0,1.0";
		} else {
			arData.uv = "0.0,0.0,0.5,1.0";
		}
	}
}
