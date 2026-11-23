#include "storm-engine\layers.h"
#include "storm-engine\events.h"

#include "_mod_on_off.h"
#include "globals.c"
#include "animals.c"
#include "NumberUtilities.c" // Warship 26.07.09. Работа с числами
#include "sea_ai\sea.c"
#include "sea_ai\ShipTowOperations.c"
#include "ships\ships.c"
#include "Encounters\Encounters.c"
#include "worldmap\worldmap.c"
// LEO: завершение игры - зал славы. Идёт ПОСЛЕ worldmap/sea (берёт их объекты, чтобы
//   отличить сушу от моря) и ДО locations - там нужны его дефайны.
#include "scripts\FinalGame.c"
#include "locations\locations.c"
#include "Loc_ai\LAi_init.c"
#include "store\goods.h"
#include "cannons\cannons.c"
#include "nations\nations.c"
#include "particles.c"
#include "characters\characters.c"
#include "interface\interface.h"
#include "store\storeutilite.c"
#include "dialog.c"
#include "quests\quests.c"
#include "islands\islands.c"
#include "colonies\colonies.c"
#include "reload.c"
#include "battle_interface\loginterface.c"
#include "calendar.c"
#include "teleport.c"
#include "utils.c"
#include "weather\WhrWeather.c"
#include "controls\controls.c"
#include "ITEMS\items.h"
#include "ITEMS\itemLogic.c"
#include "ITEMS\items_utilite.c"
#include "Achievements.c"
#include "store\store.h"
#include "interface\interface.c"
#include "discord\discord.c"
#include "OnLoadReinit.c"
#include "Addons\utils.c"
#include "Controls\hotkey_travel.c"
#include "scripts\shooting_competition.c"
#include "Controls\throwing_hotkey.c"
#include "scripts\throwing_competition.c"
#include "BUHO_CIC.c"

extern void ShipsInit(); //Boyer add
extern void InitBaseCannons();
extern void InitCharacters();
extern void InitBaseInterfaces();

extern void wdmInitWorldMap();
extern void InitGoods();
extern void InitStores();
extern int InitItems();
extern void InitCharactersTables();
extern void InitCharactersNames();
extern void InitPiratesNames();
extern void InitGeneratorNames();
extern void InitIndianNames();
extern void CreateCharacters();

extern void ActiveF4Control(); // boal debuger
extern void ActiveF5Control(); // boal debuger
extern void ActiveF7Control(); // boal debuger
extern void ActiveF10Control(); // boal debuger
extern void ActiveF12Control(); // boal debuger
extern void ActiveINSERTControl(); // boal debuger

extern void setEquipmentComplect(int num);

native float Bring2RangeNoCheck(float fMin1, float fMax1, float fMin2, float fMax2, float fValue);
native float Bring2Range(float fMin1, float fMax1, float fMin2, float fMax2, float fValue);
native float Degree2Radian(float fDegree);
native float Clampf(float fValue);
native int Clamp(int iValue, int iMin, int iMax);
// Gregg: габариты модели корабля без постановки его в морскую сцену. sModel — путь вида
// "Ships/Barque1/barque1" относительно RESOURCE\MODELS; iAxis: 0=X ширина по реям,
// 1=Y высота по мачтам, 2=Z длина. Считает объединение боксов узлов .gm-иерархии (как
// SHIP::CalcRealBoxsize), читая только заголовки файлов; результат кэшируется в движке.
native float GetShipModelBox(string sModel, int iAxis);
native int RDTSC_B();
native int RDTSC_E(int iRDTSC);
native void SetDiscordRPC(string details, string state, string button, string nation);
native void ApplyVideoMode(int vbNewW, int vbNewH, int render, int msaa, bool vsync);
native void SetFpsOptions(int bShowFps, int iMaxFps); // Gregg: показ FPS + кап FPS наживую (пишет engine.ini)
native void SetLetterbox(int on); // Gregg: тумблер letterbox наживую (пишет engine.ini [Interface] Letterbox)
native int SetProgressAnimation(string sTexture); // LEO: смена загрузочной анимации (engine.ini [ProgressImage] Texture)
native int SetInterfaceTheme(int bBlue); // LEO: синяя тема интерфейсов (engine.ini [Interface] BlueTheme)
native void SetRetainMB(int mb); // Gregg: бюджет кэша текстур наживую (пишет engine.ini [Texture] RetainMB)
native void SetGeometryRetainMB(int mb); // Gregg: бюджет кэша геометрии наживую (пишет engine.ini [Geometry] RetainMB)
native void SetWarmAssets(int on); // Gregg: прогрев ассетов в ОЗУ вкл/выкл (пишет engine.ini [Cache] WarmAssets; рестарт-онли)
native void SetWarmTexturesMB(int mb); // Gregg: объём прогрева текстур в ОЗУ, МБ (пишет engine.ini [Cache] WarmTexturesMB; рестарт-онли)
native void SetLargeBackBuffer(int on); // Gregg: суперсэмплинг — бэкбуфер больше рабочего стола (пишет engine.ini UseLargeBackBuffer; рестарт-онли)
native void SetPinTextures(int on); // Gregg: кэш текстур неба вкл/выкл (пишет engine.ini [Sky] PinTextures; рестарт-онли)
native void SetScriptCache(int mode); // Gregg: кэш скомпилированных скриптов 0/1/2 (пишет engine.ini [script] cache_mode; рестарт-онли)
native void SetShadowMode(int idx); // Gregg: режим теней 0=Нет/1=Упрощённые/2=Стандартные (engine.ini [Shadow]; рестарт-онли)
native void SetFlareMode(int idx);  // Gregg: режим корабельных фларов 0=Сокращённые/1=Полные (engine.ini [Ship] FlareMode; рестарт-онли)
// LEO: Экспериментальная жара выстрелов: включение, численные параметры и счётчики нагрузки.
native int SetCannonHeatHaze(int enabled);
native int SetCannonHeatHazeParam(string name, float value);
native float GetCannonHeatHazeStat(string name);
native int SetFireHeatSource(string name, int mode);
native int ClearFireHeatSources();
// LEO: Привязки живут в постоянном сегменте: повторная загрузка меню не должна сбрасывать адреса.
native int GetFlagPhysicsOption();
native int SetFlagPhysicsOption(int enabled);
native int GetRagdollOption(int field);
native int SetRagdollOptions(int enabled, int count, int seconds);
native void GeoLogMark(string label); // Gregg: диагностика гео-дедупа — маркер границы локации в geoDedup.txt (при geometry_log=1)

native int SetTexturePath(int iLevel, string sPath);
native int SetGlowParams(float fBlurBrushSize, int Intensivity, int BlurPasses);
native int RPrint(int x, int y, string sPrint);
native int GetTexture(string name);
native void ReleaseTexture(int texId);
native int MakeShotToFile(string path, int width, int height); // LEO: снимок кадра в файл

#libriary "script_libriary_test"
#libriary "dx9render_script_libriary"
#libriary "FlagPhysicsOptionsLibrary"
#libriary "RagdollOptionsLibrary"

#event_handler(NEW_GAME_EVENT,"NewGame");
#event_handler(GAME_OVER_EVENT,"GameOverE");
#event_handler("Control Activation","ProcessControls");
#event_handler("MainMenuKey","ProcessMainMenuKey");
#event_handler("InterfaceKey","ProcessInterfaceKey");
#event_handler("CameraPosAng","ProcessCameraPosAng");
#event_handler("Cheat","ProcessCheat");
#event_handler("SeaDogs_ClearSaveData", "ClearLocationsSaveData");
#event_handler("StopQuestCheckProcessFreeze","ProcessStopQuestCheckProcessFreeze"); // boal 240804
//#20171009-03
#event_handler("ResetDevice", "ResetDevice");

float fHighPrecisionDeltaTime;

#define CONFIRMMODE_PROFILE_DELETE	1
#define CONFIRMMODE_SAVE_DELETE		2
#define CONFIRMMODE_SAVE_OVERWRITE	3
#define CONFIRMMODE_LOAD_GAME		4

int g_nConfirmMode;
string g_sConfirmReturnWindow;
int g_nInterfaceFileID = -1;

void DoConfirm( int nConfirmMode )
{
	g_sConfirmReturnWindow = "MAIN_WINDOW";
	if( XI_IsWindowEnable("PROFILE_WINDOW") ) {
		g_sConfirmReturnWindow = "PROFILE_WINDOW";
	}
	XI_WindowDisable( g_sConfirmReturnWindow, true );
	XI_WindowDisable( "CONFIRM_WINDOW", false );
	XI_WindowShow( "CONFIRM_WINDOW", true );
	SetCurrentNode( "CONFIRM_YES" );

	g_nConfirmMode = nConfirmMode;
	switch( nConfirmMode )
	{
	case CONFIRMMODE_PROFILE_DELETE:
		SetFormatedText( "CONFIRM_TEXT", LanguageConvertString(g_nInterfaceFileID,"Delete profile confirm") );
		break;
	case CONFIRMMODE_SAVE_DELETE:
		SetFormatedText( "CONFIRM_TEXT", LanguageConvertString(g_nInterfaceFileID,"Delete savefile confirm") );
		break;
	case CONFIRMMODE_SAVE_OVERWRITE:
		SetFormatedText( "CONFIRM_TEXT", LanguageConvertString(g_nInterfaceFileID,"Overwrite savefile confirm") );
		break;
	case CONFIRMMODE_LOAD_GAME:
		SetFormatedText( "CONFIRM_TEXT", LanguageConvertString(g_nInterfaceFileID,"Load game confirm") );
		break;
	}
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "CONFIRM_TEXT", 5 ); // центрируем по вертикали
}

void ProcessStopQuestCheckProcessFreeze() // boal 240804
{
    bQuestCheckProcessFreeze = false;
    wdmLockReload            = false;
    QuestsCheck(); // выполнить отложенные

    if (rand(20) == 10) // 16.01.06 не помню уже зачем разлочивал ГГ, но на деле мешает до квестам, но если где повиснет, то спасет ф2
    {
    	DoQuestCheckDelay("pchar_back_to_player", 0.5); // если в диалоге залочило, вернем управление
    }
}

void ProcessVersionCheck() // boal 271004
{
    ref mc = GetMainCharacter();

    if (CheckAttribute(mc, "HeroParam")) // признак, что есть герой
    {
        if (!CheckAttribute(mc, "VersionNumber"))
    	{
            // LEO: предупреждение о несовместимом сейве - красным с миганием
            Log_InfoColoredAnim("red", GetConvertStrWithReplace("Variable_seadogs_1", "Logs.txt", "#space#", " "));
            Log_InfoColoredAnim("red", GetConvertStrWithReplace("Variable_seadogs_2", "Logs.txt", "#space#", " "));
    	}
        else
        {
            if (sti(mc.VersionNumberCompatibility) < VERSION_NUM_PRE && mc.VersionNumber != GetVerNum())
            {
                // LEO: предупреждение о несовместимом сейве - красным с миганием
                Log_InfoColoredAnim("red", GetConvertStrWithReplace("Variable_seadogs_3", "Logs.txt", "#space#", " ") + mc.VersionNumber +".");
                Log_InfoColoredAnim("red", GetConvertStrWithReplace("Variable_seadogs_4", "Logs.txt", "#space#", " "));
            }
        }
    }
}

void ProcessCheat()
{
	string sCheatName;
	ref mc;
	sCheatName = GetEventData();
	mc = GetMainCharacter();

	switch(sCheatName)
	{
		case "Immortal":
			if(LAi_IsImmortal(GetMainCharacter()))
			{
				LAi_SetImmortal(GetMainCharacter(), false);
				Log_SetStringToLog("God mode OFF");
			}else{

				LAi_SetImmortal(GetMainCharacter(), true);
				Log_SetStringToLog("God mode ON");
			}
		break;
		case "ShotGun":
			if(globalSGMode != false)
			{
				globalSGMode = false;
				Log_SetStringToLog("Shotgun mode OFF");
			}else{
				globalSGMode = true;
				Log_SetStringToLog("Shotgun mode ON");
			}
		break;
		case "Gold":
			mc.money = sti(mc.money) + 100000;
			Log_SetStringToLog(" + 100000 G");
		break;
		case "Skill":
			mc.skill.freeskill = 1;

			mc.skill.freeskill = sti(mc.skill.freeskill) + 50;
			Log_SetStringToLog(" + 50 SP");
		break;
		case "Reputation":
			mc.reputation = REPUTATION_NEUTRAL;
			Log_SetStringToLog("Reputation Set to NEUTRAL");
		break;
		case "Morale":

		break;
		case "Encounters":
			if(CheckAttribute(mc,"worldmapencountersoff") == 0)
			{
				mc.worldmapencountersoff = "1";
				Log_SetStringToLog("Worldmap encounters OFF");
			}
			else
			{
				if(mc.worldmapencountersoff == "1")
				{
					mc.worldmapencountersoff = "0";
					Log_SetStringToLog("Worldmap encounters ON");
				}
				else
				{
					mc.worldmapencountersoff = "1";
					Log_SetStringToLog("Worldmap encounters OFF");
				}
			}
		break;
		case "MainCharacter":
			mc.model	= "Danielle";
			mc.sex = "woman";
			mc.FaceId = 30;
			mc.model.animation = "woman";
			mc.model.height = 1.75;
		break;
		case "soundoff":
			SendMessage(&Sound,"ll",MSG_SOUND_SET_ENABLED,0);
			Log_SetStringToLog("Sound OFF");
		break;
	}
}

object Camera;

void ProcessCameraPosAng()
{
	Camera.Pos.x = GetEventData();
	Camera.Pos.y = GetEventData();
	Camera.Pos.z = GetEventData();

	Camera.Ang.x = GetEventData();
	Camera.Ang.y = GetEventData();
	Camera.Ang.z = GetEventData();
}

void proc_break_video()
{
	PictureAsVideoBreak();
}

void Main()
{
	if (!CheckAttribute(InterfaceStates,"VISUAL_CIRASS")) InterfaceStates.VISUAL_CIRASS = 0;
	screenscaling = BI_COMPARE_HEIGHT;

	ReloadProgressStart();

	ControlsInit(GetTargetPlatform(),true, 0);
	nTeleportLocation = 1;

	NationsInit();
	ReloadProgressUpdate();

	EncountersInit();
	ReloadProgressUpdate();

	CannonsInit();
	ReloadProgressUpdate();

	ShipsInit();
	ReloadProgressUpdate();

	IslandsInit();
	ReloadProgressUpdate();

	WeatherInit();
	ReloadProgressUpdate();

	InitPerks();
	ReloadProgressUpdate();

	ReloadProgressUpdate();

	if(LoadSegment("store\initGoods.c"))
	{
		InitGoods();
		UnloadSegment("store\initGoods.c");
	}

	if(LoadSegment("Interface\BaseInterface.c"))
	{
		InitBaseInterfaces_main();
		InitBaseInterfaces();
		UnloadSegment("Interface\BaseInterface.c");
	}

	SetEventHandler("Control Activation","proc_break_video",0);
	InterfaceStates.Launched = false;
	SetEventHandler(EVENT_END_VIDEO,"Main_LogoVideo",0);
	InterfaceStates.videoIdx = 1;
	Event(EVENT_END_VIDEO);
	ReloadProgressEnd();

	SetGlowParams(1.0, 50, 2);
}

void ReloadMain() {
	InterfaceStates.showGameMenuOnExit = false;
	DeleteEntities();
	ClearEvents();
	ClearPostEvents();
	ResetSound();
	Main();
}

void Main_InitGame()
{
	nTeleportLocation      = 1;
    bDisableLandEncounters = false;
	bDisableCharacterMenu  = false;
	bWorldAlivePause       = true;
	bPauseContrabandMetro  = false;
	bMonstersGen           = false;
	bLandEncountersGen     = true;
	chrDisableReloadToLocation = false;
	bDisableFastReload         = false;
	bDisableMapEnter           = false;
	bDisableSailTo             = false;
	bEnableIslandSailTo        = false;
	bReloadCanBe               = false;
	bMapEnter                  = false;
	bQuestDisableMapEnter      = false;
    bAbordageStarted           = false;
    bCabinStarted              = false;
	bDeckBoatStarted           = false;
	LAi_IsCapturedLocation     = false;
	LAi_IsBoarding             = false;
	LAi_restoreStates          = false;
	LAi_boarding_process       = false;
	boarding_location          = -1;

	NationsInit();
	EncountersInit();
	CannonsInit();
	ShipsInit();
	IslandsInit();
	WeatherInit();
	InitPerks();
	if(LoadSegment("store\initGoods.c"))
	{
		InitGoods();
		UnloadSegment("store\initGoods.c");
	}

	if(LoadSegment("Interface\BaseInterface.c"))
	{
		InitBaseInterfaces_main();
		InitBaseInterfaces();
		UnloadSegment("Interface\BaseInterface.c");
	}
}
void Main_LogoVideo()
{
	int i = sti(InterfaceStates.videoIdx);
	switch(i)
	{
	// LEO: заставочного ролика (Videos/Title.wmv) в моде нет много лет - движок каждый запуск честно
	//   пытался его открыть и писал «Video Error!!!». Ролик не запускаем, но ШАГ ЦЕПОЧКИ сохраняем:
	//   раньше он давал такт на завершение инициализации (ReloadProgressEnd вызывается сразу после
	//   Event(EVENT_END_VIDEO)). Без него старт игры уезжал вперёд, и вместо загрузочного экрана
	//   оставался залитый цветом кадр.
	case 1:
		InterfaceStates.videoIdx = 4;
		PostEvent(EVENT_END_VIDEO, 1);
	break;

	default:
		DelEventHandler(EVENT_END_VIDEO,"Main_LogoVideo");
		DeleteClass(&aviVideoObj);
		Event("DoInfoShower","sl","game prepare",true);
		SetEventHandler("frame","Main_Start",1);
	break;
	}
}

void Main_Start()
{
	ReloadProgressStart();

	DelEventHandler("Control Activation","proc_break_video");
	DelEventHandler("frame","Main_Start");

	if( GetTargetPlatform()=="pc" )
	{
		ControlsInit(GetTargetPlatform(),false, 0);
		objControlsState.controls_set_version = CONTROLS_SET_VERSION;	// новая игра стартует с актуальной версией набора контролов
	}

	ReloadProgressUpdate();
	InitGame();
	ReloadProgressUpdate();

	Environment.date.hour = worldMap.date.hour;
	Environment.date.min = worldMap.date.min;
	Environment.date.sec = worldMap.date.sec;
	Environment.time = stf(worldMap.date.hour) + stf(worldMap.date.min)/60.0 + stf(worldMap.date.sec)/3600.0;
	Environment.date.year = worldMap.date.year;
	Environment.date.month = worldMap.date.month;
	Environment.date.day = worldMap.date.day;


	InterfaceStates.Buttons.Load.enable = true;

	Event("DoInfoShower","sl","game prepare",false);

	StartLanguageSetting(LanguageGetLanguage());
 	LoadPlayerProfileDefault(); // boal
	LaunchMainMenu();

	CharacterIsDead(GetMainCharacter());

	ReloadProgressEnd();
}

void SaveGame()
{
	string saveName = GetEventData();
	string saveData = GetEventData();
	DelEventHandler("evntSave","SaveGame");

	aref arTmp;
	if( !GetEntity(arTmp,"fader") && !Shooting_Active() && !CheckAttribute(pchar,"Shooting.pending") && !Throwing3D_IsActive() && !CheckAttribute(pchar,"Throwing.pending") )
	{
		pchar.version_number = 104;
		SaveEngineState(saveName);
		ISetSaveData(saveName,saveData);
	}
	PostEvent("DoInfoShower",1,"sl","save game",false);
	DeleteEntitiesByType("scrshoter");
	HideQuickSaveMenu();
}

void LoadGame()
{
    if (Shooting_Active()) SR_Stop();
    if (Throwing3D_IsActive()) T3D_Stop();
	PauseAllSounds(); // boal fix
    ResetSound();  // fix
    if (bSeaActive && !bAbordageStarted)
    {
		SendMessage(&AIBalls, "l", MSG_MODEL_RELEASE);
	}
	// вылетам у форта НЕТ <--
	string saveName = GetEventData();
	DelEventHandler("evntLoad","LoadGame");

	string retStr="";
	SendMessage(&GameInterface,"lse",MSG_INTERFACE_GET_SAVE_DATA,saveName,&retStr);
	if( retStr=="" ) {return;}

	DeleteEntities();
	ClearEvents();
	ClearPostEvents();
	SetTimeScale(1.0);
	TimeScaleCounter = 0;

	CreateEntity(&LanguageObject,"obj_strservice");
	CreateEntity(&reload_fader, "fader");
	SendMessage(&reload_fader, "ls",FADER_PICTURE0, "loading\jonny_load\load\load_3.dds");
	SendMessage(&reload_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);
	ReloadProgressStart();
	pchar.savegamename = saveName;
	SetEventHandler("frame","LoadGame_continue",1);
	iCalculateSaveLoadCount("Load");
}

void LoadGame_continue()
{
	DelEventHandler("frame","LoadGame_continue");
	FreezeGroupControls(curKeyGroupName,true);
	LoadEngineState(pchar.savegamename);
}

void InterfaceDoExit()
{
	DelEventHandler("frame","InterfaceDoExit");
	InterfaceStates.Launched=false;
	if (Sea_InTacticalView()) SeaShipCamera.TacHideHUD = 0; // возврат к морю -> вернуть тактический HUD
	if(sti(InterfaceStates.doUnFreeze)==true)
	{
		SendMessage(&GameInterface,"l",MSG_INTERFACE_RELEASE);
		if(!IsEntity(&aviVideoObj))
		{
			if( bSeaActive && !bAbordageStarted )
			{
				LayerFreeze(SEA_REALIZE,false);
				LayerFreeze(SEA_EXECUTE,false);
			}
			else
			{
				LayerFreeze(REALIZE,false);
				LayerFreeze(EXECUTE,false);
			}
		}
		DeleteAttribute(&GameInterface,"");
	}
	switch(interfaceResultCommand)
	{
		case RC_INTERFACE_FORTCAPTURE_EXIT:
			ReloadAfterFortCapture();
			break;

		case RC_INTERFACE_DO_NOTHING:
			break;
		case RC_INTERFACE_MAIN_MENU_EXIT:
			break;
		case RC_INTERFACE_DO_NEW_GAME:
			InterfaceStates.doUnFreeze = false;
			DeleteEntities();
			ResetSound();  // fix
			ClearEvents();
			ClearPostEvents();

            if (CheckAttribute(pchar, "HeroParam")) // признак, что есть герой
            {
				trace("Main_InitGame");
				Main_InitGame(); // boal вынес все в одинметод, новая игра из страой глючит
			}
			else
			{
				if(LoadSegment("Interface\BaseInterface.c"))
				{
					InitBaseInterfaces_main();
					InitBaseInterfaces();
					UnloadSegment("Interface\BaseInterface.c");
				}
			}
			DeleteEntities();
			ClearEvents();
			SetEventHandler("frame","NewGame",1);
			InterfaceStates.doUnFreeze = false;
		break;
		case RC_INTERFACE_DO_LOAD_GAME:
			if(CheckAttribute(&InterfaceStates,"Buttons.Resume.enable") && sti(InterfaceStates.Buttons.Resume.enable) == true)
			{
				LaunchLoadGame(false);
			}
			else
			{
				LaunchLoadGame(true);
			}
			break;
		case RC_INTERFACE_DO_SAVE_GAME:
			LaunchSaveGame();
			break;
		case RC_INTERFACE_DO_OPTIONS:
			LaunchOptionScreen();
			break;

		case RC_INTERFACE_LOCKPICK: //New Abilities - взлом сундука
			LaunchLockPicking();
		break;

		case RC_INTERFACE_DO_CREDITS:
		    LaunchAboutScreen();
		break;
		case RC_INTERFACE_DO_RESUME_GAME:
		break;
		case RC_INTERFACE_RANSACK_MAIN_EXIT:
			if (CheckAttribute(pchar,"ignorereturn2sea")) DeleteAttribute(pchar,"ignorereturn2sea");
			else Return2SeaAfterAbordage();
			break;
		case RC_INTERFACE_CHARACTER_SELECT_EXIT:
			DoQuestFunctionDelay("WayBeginning", 1.0);
		break;
		case RC_INTERFACE_SPEAK_EXIT_AND_CAPTURE:
			string sTargetChr = pchar.speakchr;
			pchar.abordage = 1;
			Sea_AbordageStartNow(SHIP_ABORDAGE, GetCharacterIndex(sTargetChr), true, true);
			SetTimeScale(1.0);
			TimeScaleCounter = 0;
			DelPerkFromActiveList("TimeSpeed"); //boal
			pchar.speakchr = 0;
			pchar.whospeak = 0;
		break;
		case RC_INTERFACE_SPEAK_EXIT_AND_TRADE:
			LaunchStore(SHIP_STORE);
		break;
		case RC_INTERFACE_TO_CHAR:
			pchar = GetMainCharacter();
			LaunchCharacter(pchar);
		break;
		case RC_INTERFACE_TO_SHIP:
			LaunchShipState();
		break;
		case RC_INTERFACE_TO_LOGBOOK:
			LaunchQuestBook();
		break;
		case RC_INTERFACE_TO_CREATENOTICE: // Gregg: из журнала открыть заметки/планировщик
			LaunchCreateNotice();
		break;
		case RC_INTERFACE_TO_ITEMS:
			LaunchItems(); // to_do
		break;
		case RC_INTERFACE_LAUNCH_GAMEMENU:
			LaunchGameMenuScreen();
			break;
		case RC_INTERFACE_GAME_LOG: // LEO: журнал игры
			LaunchGameLogScreen();
			break;
		case RC_INTERFACE_FINAL_GAME: // LEO: завершение игры - переход в зал славы
			FinalGame_Start();
			break;
		// boal -->
  		case RC_INTERFACE_DO_BOAL_BETA:
		     LaunchBoalBetaScreen();
        break;
		// Gregg: Debug tools -->
		case RC_INTERFACE_DO_GOODS_SPAWN:
			LaunchGoodsSpawnScreen();
		break;
		case RC_INTERFACE_DO_SHIP_CHANGE:
			LaunchShipChangeScreen();
		break;
		// Gregg: Debug tools <--
		// boal <--

		// Warship -->
		case RC_INTERFACE_BEST_MAP:
			LaunchBestMapScreen(); // Смотрим отличную карту
			break;
		// <-- Warship

		case RC_INTERFACE_MAPVIEW:
			LaunchMapViewScreen(); // Смотрим атлас карт
			break;
		case RC_INTERFACE_POINTS_EX:
			LaunchPointsEX();
			break;
		// Крафт
		case RC_INTERFACE_CRAFT_ALL:
			LaunchCraftAll();
		break;
		// Обмен драгоценных камней у ростовщика
		case RC_ITEMS_EXCHANGE:
			LaunchItemsExchange();
		break;
		
		case RC_INTERFACE_TO_CANNONS:
			ref NPChar;
			makeref(NPChar,CharacterRef);
			int iTest = FindColony(NPChar.City); // город магазина
			ref rColony = GetColonyByIndex(iTest);
			LaunchCannons(sti(rColony.StoreNum));
		break;
		
		case RC_INTERFACE_TO_SHIPYARD:
			makeref(NPChar,CharacterRef);
			LaunchShipyard(npchar);
		break;
		
		case RC_SHIPS_UPGRADES:
			makeref(NPChar,CharacterRef);
			LaunchShipUpgrades(npchar)
		break;
	}
}
void EngineLayersOffOn(bool on)
{
	on = !on;
	if( on ) {
		LayerFreeze(REALIZE,on);
		LayerFreeze(EXECUTE,on);
		LayerFreeze(SEA_REALIZE,on);
		LayerFreeze(SEA_EXECUTE,on);
	} else {
		if(bSeaActive && !bAbordageStarted) {
			LayerFreeze(SEA_REALIZE,on);
			LayerFreeze(SEA_EXECUTE,on);
		} else {
			LayerFreeze(REALIZE,on);
			LayerFreeze(EXECUTE,on);
		}
	}
}

string seadogs_saveFrom = "";

void OnSave()
{
	savefile_info = GetVerNum();
	seadogs_saveFrom = "";
	if(IsEntity(&worldMap) != 0)
	{
		seadogs_saveFrom = "world map";
	}
	else
	{
		if(IsEntity(&AISea) != 0)
		{
			Sea_Save();
			seadogs_saveFrom = "sea";
		}
		else
		{
			int ldLoc = FindLoadedLocation();
			if(ldLoc >= 0)
			{
				seadogs_saveFrom = "location";
				SendMessage(&Locations[ldLoc], "l", MSG_LOCATION_SETCHRPOSITIONS);
				LAi_SaveInfo();
				PostEvent("SeaDogs_ClearSaveData", 400, "a", &Locations[ldLoc]);
			}
			else
			{
				Trace("OnSave() -> unknown character location");
			}
		}
	}
	iCalculateSaveLoadCount("Save");
}

//#20180226-01
bool CanSave()
{
    if (Shooting_Active() || CheckAttribute(pchar,"Shooting.pending")) return false;
    if (Throwing3D_IsActive() || CheckAttribute(pchar,"Throwing.pending")) return false;
    // LEO: в зале славы не сохраняются - игра там уже подведена к финалу, а возврата из него нет.
    //   Гард стоит именно здесь: через CanSave проходят и ручное сохранение, и все автосейвы.
    if( CheckAttribute(pchar, "location") && pchar.location == FINAL_GAME_LOCATION ) return false;

    bool bReturn = false;
    if(IsEntity(&worldMap) != 0)
	{
		bReturn = true;
	}
	else
	{
		if(IsEntity(&AISea) != 0)
		{
			bReturn = true;
		}
		else
		{
			int ldLoc = FindLoadedLocation();
			if(ldLoc >= 0)
			{
				bReturn = true;
			}
		}
	}
	return bReturn;
}

void ClearLocationsSaveData()
{
	aref loc = GetEventData();
	SendMessage(loc, "l", MSG_LOCATION_CLRCHRPOSITIONS);
}

int actLoadFlag = 0;

void OnLoad()
{
    Ach_EnsureMinigames();
    Shooting_OnLoad();
    Throwing_OnLoad();
	HH_Reset();
	actLoadFlag = 1;

	if(screenscaling < 50) screenscaling = BI_COMPARE_HEIGHT;

	bYesBoardStatus=false;
	DeleteClass(&IBoardingStatus);
	DeleteAttribute(&IBoardingStatus,"");

	DeleteAttribute( pchar, "abordage_active_count" );
	FreezeGroupControls(curKeyGroupName,false);

	if( CharacterIsDead(pchar) ) {
		pchar.chr_ai.hp = 1.0;
	}

	ResetSound();

	if(LoadSegment("Interface\BaseInterface.c"))
	{
		InitBaseInterfaces();
		InitInterfaceTables();
		UnloadSegment("Interface\BaseInterface.c");
	}

	ReloadProgressUpdate();

	DialogsInit();
	ReloadProgressUpdate();

	//Boyer add to fix missing ship init
	ShipsInit();

	InitCharacterEvents();
	ReloadProgressUpdate();

	QuestsInit();
	ReloadProgressUpdate();

	InitTeleport();
	ReloadProgressUpdate();

	InitParticles();
	ReloadProgressUpdate();

	WeatherInit();
	ReloadProgressUpdate();

	InterfaceStates.Buttons.Resume.enable = true;
	InterfaceStates.Buttons.Save.enable = true;
	InterfaceStates.Buttons.Load.enable = true;

	//#20190327-01
	if(!CheckAttribute(pchar, "camEye"))
        pchar.camEye = 0;

	// LEO: лечим начатые партии - у служанки Ле-Франсуа в ините был опечатан адрес таверны
	//   ("LeFransuae_tavern" вместо "LeFransua_tavern"), поэтому она не логинилась в локацию
	//   вообще. Адрес лежит в сейве, инит его не перезаписывает - правим на загрузке.
	ref chrWaitressFix = CharacterFromID("LeFransua_waitress");
	if (CheckAttribute(chrWaitressFix, "location"))
	{
		if (chrWaitressFix.location == "LeFransuae_tavern")
		{
			ChangeCharacterAddressGroup(chrWaitressFix, "LeFransua_tavern", "waitress", "barmen");
		}
	}
	Headhunter_HealVillemstadExits(); // LEO: Виллемстад, запертый со сцены Халена (см. reaction_functions.c)
	GWIK_FortOrangeDoorNightClose(); // LEO: ночное закрытие конторы ГВИК в уже открытом Форт-Оранже
	TwoSisters_RepairSanJuanBrothel(); // LEO: исправляем сохранённый вход до восстановления локации.
	BO_RepairBryanHouseReturn(); // LEO: возврат из комнаты Брайана к верхней двери в старых сейвах.
	Blood_RestoreAgentBonuses(); // LEO: недостающие бонусы агента после завершённого пролога Блада.
	Undead_RepairGravediggerBlade(); // LEO: сохраняем оружие могильщика до создания его нативной модели.
	RepairSpecialShipMinCrew(); // LEO: исправляем минимум Марлина и улучшенного Пса до создания морских сущностей.
	RepairDefaultShipMinCrew(); // LEO: восстанавливаем минимум обычной генерации по сохранённым бонусам.
	BO_RepairQuestRewardsAndWaits(); // LEO: цены заслуженных наград и старые межглавные ожидания.

	SM_PusherOnLoad();

	LoadGameOptions();

	if(seadogs_saveFrom == "location")
	{
		ref mainchar;
		mainchar = GetMainCharacter();
		LoadMainCharacterInFirstLocation(mainchar.location,"", mainchar.location.from_sea);
		int ldLoc = FindLoadedLocation();
		if(ldLoc >= 0)
		{
			SendMessage(&Locations[ldLoc], "l", MSG_LOCATION_TLPCHRPOSITIONS);
		}
	}
	else
	{
		if (seadogs_saveFrom == "sea")
		{
			Sea_Load();
 		}
		else
		{
			if(seadogs_saveFrom == "world map")
			{
				wdmLoadSavedMap();
			}
			else
			{
				Trace("Unknown seadogs_saveFrom == " + seadogs_saveFrom);
			}
		}
	}

	EngineLayersOffOn(true);
	DialogRun = false;
	InterfaceStates.Launched = false;

	ReloadProgressUpdate();

	PerkLoad(false);

	ReloadProgressUpdate();

	/* Управление интерфейсом мышью в море при настройке управления под ГПК */
	if(CheckAttribute(InterfaceStates, "EnabledCMControls") && sti(InterfaceStates.EnabledCMControls) == 0)
	{
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsConfirmLButton", CI_GetKeyCode("VK_LBUTTON"), 0, false );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsCancelRButton", CI_GetKeyCode("VK_RBUTTON"), 0, false );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsLeftWheel", CI_GetKeyCode("VK_MWHEEL_DOWN"), 0, false );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsRightWheel", CI_GetKeyCode("VK_MWHEEL_UP"), 0, false );
		/* С зажатым Alt колесико работает как вверх/вниз */
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsActivateAlt", CI_GetKeyCode("Alt"), 0, false );
		/* Активация меню на Alt + LClick */
		CI_CreateAndSetControls( "Sailing3Pers", "BICommandsActivateAlt", CI_GetKeyCode("Alt"), 0, false );
		CI_CreateAndSetControls( "Sailing3Pers", "BICommandsActivateLButton", CI_GetKeyCode("VK_LBUTTON"), 0, false );
	}

	if(CheckAttribute(InterfaceStates, "EnabledCMControls") && sti(InterfaceStates.EnabledCMControls) == 1)
	{
		CI_CreateAndSetControls( "Sailing3Pers", "BICommandsActivate", CI_GetKeyCode("VK_MBUTTON"), 0, true );
		CI_CreateAndSetControls( "PrimaryLand", "LICommandsActivate", CI_GetKeyCode("VK_MBUTTON"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsConfirm", CI_GetKeyCode("VK_LBUTTON"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsCancel", CI_GetKeyCode("VK_RBUTTON"), 0, false );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsLeft", CI_GetKeyCode("VK_MWHEEL_DOWN"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsRight", CI_GetKeyCode("VK_MWHEEL_UP"), 0, true );
	}

	ReloadProgressEnd();
    //#20200519-02
    BattleInterface.ShifInfoVisible = InterfaceStates.enabledshipmarks;
    BattleInterface.battleborder.used = InterfaceStates.ShowBattleMode;
	if( CheckAttribute(&InterfaceStates,"WorldSituationUpdateStep") &&
		sti(InterfaceStates.WorldSituationUpdateStep) < 36 )
	{
		Event("EvSituationsUpdate", "l", sti(InterfaceStates.WorldSituationUpdateStep));
	}

	actLoadFlag = 0;
	////
	ProcessVersionCheck();

	//#20181015-02
	if(bSeaActive && !bMapEnter)
        Sound_OnSeaAlarm555(seaAlarmed, true);

	if (IsEntity(pchar) && GetCharacterEquipByGroup(pchar, PATENT_ITEM_TYPE) != "" && !CheckAttribute(pchar, "EquipedPatentId")) SetEquipedItemToCharacter(pchar, PATENT_ITEM_TYPE, GetCharacterEquipByGroup(pchar, PATENT_ITEM_TYPE)); // фикс неправильной экипировки патента

	if (isMainCharacterPatented() && !checkattribute(pchar, "PatentNation")) 
		pchar.PatentNation = NationShortName(GetPatentNation());//фикс неправильной выдачи патента Тайному агенту, при загрузке сейва

	iCalculateSaveLoadCount("Load");
	
	ResetExpLogs();
	
	if (!CheckAttribute(pchar,"BombsFix30072025"))
	{
		if(LoadSegment("store\initGoods.c"))
		{
			InitGoods();
			UnloadSegment("store\initGoods.c");
		}
		pchar.BombsFix30072025 = true;
	}
	if (!CheckAttribute(pchar,"FastReloadApteka"))
	{
		objFastReloadTable.table.Marigo.l13.pic = 22;
		objFastReloadTable.table.Marigo.l13.tex = 0;
		objFastReloadTable.table.Marigo.l13.location = "Farmacia";
		pchar.FastReloadApteka = true;
	}
	// Gregg: контора ГВИК Форт-Оранжа в быстрый переход (как аптека выше). objFastReloadTable персистит в сейве и не перестраивается на старых сейвах, поэтому добавляем рантаймом разово. CheckFastJump прячет пункт, пока контора не открыта (дверь reload_H3 не ведёт в FortOrange_GWIK).
	if (!CheckAttribute(pchar,"FastReloadFortOrangeGWIK"))
	{
		objFastReloadTable.table.FortOrange.l10.pic = 19;   // FRP_GWIK
		objFastReloadTable.table.FortOrange.l10.tex = 0;    // FRT_GWIK
		objFastReloadTable.table.FortOrange.l10.location = "FortOrange_GWIK";
		pchar.FastReloadFortOrangeGWIK = true;
	}
	SetPerspectiveSettings();//#20171223-01 Camera perspective option
	SetPerspective2Settings();//#20190815-01
	
	// Пересобираем управление, только если версия набора контролов в сейве отстаёт от текущей.
	// (Раньше сравнивали ЧИСЛО контролов с CONTROLS_NUMBER, но реальное число плавает от
	//  standart / EnabledCMControls / MOD_BETTATESTMODE, и проверка ложно срабатывала почти каждую загрузку.)
	if (!CheckAttribute(&objControlsState, "controls_set_version") ||
	    sti(objControlsState.controls_set_version) != CONTROLS_SET_VERSION ||
	    !CheckAttribute(&objControlsState, "keygroups.PrimaryLand.HotkeysHelp") ||
	    !CheckAttribute(&objControlsState, "keygroups.PrimaryLand.HotkeysHelpPrevPage") ||
	    !CheckAttribute(&objControlsState, "keygroups.PrimaryLand.HotkeysHelpNextPage"))
	{
		if (objControlsState.keygroups.primaryland.ChrAction == "vk_lbutton") ControlsInit(GetTargetPlatform(),false, 1);
		else ControlsInit(GetTargetPlatform(),false, 0);
		// LEO: ControlsInit очищает раскладку, восстанавливаем пользовательские привязки.
		RestoreSavedControlOptions();
		ControlsMakeInvert();
		objControlsState.controls_set_version = CONTROLS_SET_VERSION;	// пометить набор как актуальный (попадёт в сейв)
	}
	// LEO: Старые сейвы могут хранить переключение мушкета только в боевой группе.
	if (!CheckAttribute(&objControlsState, "keygroups.PrimaryLand.MushketHotkey"))
		MapControlToGroup("MushketHotkey", "PrimaryLand");
	if (!CheckAttribute(&objControlsState, "musket_key_migrated"))
	{
		if (!RestoreSavedControlOptions()) MigrateMusketBinding();
		objControlsState.musket_key_migrated = true;
	}
}

void NewGame()
{
	HH_Reset();
	DeleteEntities();
	DelEventHandler("frame","NewGame");

	CreateEntity(&LanguageObject,"obj_strservice");
	CreateEntity(&reload_fader, "fader");
	SendMessage(&reload_fader, "ls",FADER_PICTURE0, "loading\jonny_load\load\load_3.dds");
	SendMessage(&reload_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);

	bYesBoardStatus=false;
	DeleteClass(&IBoardingStatus);
	DeleteAttribute(&IBoardingStatus,"");

	for (i = 0; i < TOTAL_CHARACTERS; i++) InitCharacter(&Characters[i], i);

	SetEventHandler("frame","NewGame_continue",1);
}

void NewGame_continue()
{
	DeleteAttribute( pchar, "abordage_active_count" );  // это тут не рабоатет :)  Но и не мешает Boal

	ReloadProgressStart();

	DelEventHandler("frame","NewGame_continue");

	if(LoadSegment("Interface\BaseInterface.c"))
	{
		InitBaseInterfaces();
		UnloadSegment("Interface\BaseInterface.c");
	}

	InitGame();

	ReloadProgressUpdate();

	CreateColonyCommanders();

	ReloadProgressUpdate();

	SetNames();
	ReloadProgressUpdate();

	Environment.date.hour = worldMap.date.hour;
	Environment.date.min = worldMap.date.min;
	Environment.date.sec = worldMap.date.sec;
	Environment.time = stf(worldMap.date.hour) + stf(worldMap.date.min)/60.0 + stf(worldMap.date.sec)/3600.0; //boal
	Environment.date.year = worldMap.date.year;
	Environment.date.month = worldMap.date.month;
	Environment.date.day = worldMap.date.day;

	InterfaceStates.WorldSituationUpdateStep = 100;
	InterfaceStates.Buttons.Resume.enable = true;
	InterfaceStates.Buttons.Save.enable = true;
	InterfaceStates.Buttons.Load.enable = true;

	pchar = GetMainCharacter(); //fix
	//#20190327-01
    pchar.camEye = 0;
	setNewMainCharacter(pchar, 1);
	ReloadProgressUpdate();

    RumourInit();  //homo 23/06/06
	ReloadProgressUpdate();

	ActivateTimeEvents();

	ReloadProgressUpdate();

	SetDamnedDestinyVariable();
	ReloadProgressUpdate();

	pchar.questTemp.sbormoney = -1;

	startGameWeather = true;
		InterfaceStates.startGameWeather = FindWeather("11 Hour");
		LoadMainCharacterInFirstLocationGroup("SelectMainCharacter_Cabine", "reload", "reload1");
		pchar.quest.SelectMainCharacterInCabine.win_condition.l1          = "location";
		pchar.quest.SelectMainCharacterInCabine.win_condition.l1.location = "SelectMainCharacter_Cabine";
		pchar.quest.SelectMainCharacterInCabine.function                  = "SelectMainCharacterInCabine";
	ReloadProgressUpdate();

	UpdateCrewInColonies(); // пересчет наемников в городах
	ReloadProgressUpdate();

    //#20190909-01
    pchar.fix20190909 = true;
	ReloadProgressUpdate();
	ReloadProgressEnd();
}

void InitGame()
{
	InitSound();

	ReloadProgressUpdate();

	DeleteSeaEnvironment();
	if(LoadSegment("worldmap\worldmap_init.c"))
	{
		wdmInitWorldMap();
		UnloadSegment("worldmap\worldmap_init.c");
	}
    InitPerks();
    ReloadProgressUpdate();

	IslandsInit();
	ReloadProgressUpdate();

	LocationInit();
	ReloadProgressUpdate();

	DialogsInit();
	ReloadProgressUpdate();

	InitTeleport();
	ReloadProgressUpdate();

	InitParticles();
	ReloadProgressUpdate();

    if(LoadSegment("items\initItems.c"))
	{
		InitItems();
		UnloadSegment("items\initItems.c");
	}
	ReloadProgressUpdate();
	//Boyer change #20170301-6...CharactersInit assigns RealShips, and after a bunch of 'New' games in a
	//session, RealShips array overflows, so call new function to reset
	ResetRealShipArray();
	ReloadProgressUpdate();

	CharactersInit();
	ReloadProgressUpdate();

	ColoniesInit();
	ReloadProgressUpdate();

	if(LoadSegment("store\initStore.c"))
	{
		InitStores();
		UnloadSegment("store\initStore.c");
	}

	QuestsInit();
	ReloadProgressUpdate();


	SeaAIGroupsInit();
	ReloadProgressUpdate();

	InitQuestMapEncounters();
	ReloadProgressUpdate();

	ResetQuestMovie();
	ReloadProgressUpdate();

	LoadGameOptions();

	ReloadProgressUpdate();
	InfoShowSetting();

}

int counter = 0;
void ProcessControls()
{
	string ControlName;
	ControlName = GetEventData();
    if (Shooting_Active()) { if (ControlName == "SR_Exit") SR_Exit(); return; }
    if (Throwing3D_IsActive())
    {
        if (ControlName == "T3D_Exit") T3D_Exit();
        return;
    }

	// Passive HUD: only its own controls are handled; gameplay keeps running.
	if (ControlName == "HotkeysHelp") { HH_Toggle(); return; }
	if (ControlName == "HotkeysHelpPrevPage") { HH_ChangePage(-1); return; }
	if (ControlName == "HotkeysHelpNextPage") { HH_ChangePage(1); return; }
	// boal -->
    int   itmIdx;
	String itemID;
	bool bOk, bOk1;

    if (ControlName == "QuickSave") { MakeQuickSave(); return; }
    if (ControlName == "QuickLoad") { MakeQuickLoad(); return; }

    // LEO: цифры 1..9 в диалоге разбирает ДВИЖОК (dialog.cpp, Realize) - только он знает, дочитан
    //   ли текст ноды. Здешний вариант звал ProcessDialogEvent() сразу и съедал непрочитанные страницы.
    if (dialogRun) return;
	if (ControlName == "SaveDelete" && CurrentInterface == INTERFACE_SAVELOAD)
	{
		if (XI_IsWindowEnable("PROFILE_WINDOW")){DoConfirm(1); return;}
		else {DoConfirm(2); return;}
	}
	if (sti(InterfaceStates.Launched)==true) 
	{
		if (CurrentInterface == INTERFACE_ITEMSTRADE || INTERFACE_STORE)
		{
			string func = "";
			if (ControlName == "ChangeTable" && CurrentInterface != INTERFACE_STORE) 
			{
				func = "SwitchTradeItemsTable";
				call func();
				return;
			}
			else if (ControlName == "ILeft2") SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, GetCurrentNode(), -1, 0, ACTION_LEFTSTEP);
			else if (ControlName == "IRight2") SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, GetCurrentNode(), -1, 0, ACTION_RIGHTSTEP);
		}
		return;
	}

	if (ControlName == "WhrPrevWeather")	{ Whr_LoadNextWeather(-1); return; }
	if (ControlName == "WhrNextWeather")	{ Whr_LoadNextWeather(1); return; }
	if (ControlName == "WhrUpdateWeather")	{ Whr_UpdateWeather(); return; }

	if(bSeaActive && !bAbordageStarted)
	{
		if (ControlName == "Press7" && Boarding_PlayerCut()) return;
		switch(ControlName)
		{
		    case "Ship_Fire": // Gregg: вне тактики - залп. В тактике: CSP(Ship_Fire=пробел) = пауза; ГПК(Ship_Fire=ЛКМ) - выбор/выстрел отдаём движку (Sea_TacClick/evntTacFire), пауза в ГПК - на пробеле (BIFastCommand)
		        if (Sea_InTacticalView())
		        {
		            if (objControlsState.keygroups.primaryland.ChrAction != "vk_lbutton") Sea_ToggleTacticalPause();
		        }
		        else Ship_DoFire();
		        return; break;
		    case "Interface": ProcessInterfaceKey(); return; break;
			case "MainMenu": ProcessMainMenuKey();	return; break;
			case "MainMenuDouble": ProcessMainMenuKey(); return; break;
			case "Sea_CameraSwitch": SeaCameras_Switch(); return; break;
			case "Sea_TacticalView": Sea_ToggleTacticalView(); return; break;
			// LEO: режим "Марс" (воронье гнездо). Вход/выход Z; переход между мачтами < и > опрашивает сам движок (DECK_CAMERA::Move)
			case "Sea_MarsView": Sea_ToggleMarsView(); return; break;
			// Gregg: X — циклическое переключение режима палубного прицела (обычный/дуга+круг+точки/круг+точки)
			case "Sea_AimMode": Sea_ToggleAimMode(); return; break;
			case "Sea_TacCenterSelf": Sea_TacticalRecenterSelf(); return; break;
			// пан/зум тактической камеры (Sea_TacPan*/Sea_TacZoom*) обрабатываются в движке
			// (SHIP_CAMERA::Move) покадровым опросом клавиш — плавно и на паузе
			case "Ship_Fire": Ship_DoFire(); break;
			case "Tele": Sea_ReloadStart(); return; break;
			 //// {*} BUHO-CIC - ADDED CODE - Key event response.
            //#20180226-02
            case "CIC_LoadBalls" :
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_5", "Logs.txt", "#space#", " "));
                CIC_SendCommand(ControlName);
                return;
            break;
            case "CIC_LoadGrapes" :
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_6", "Logs.txt", "#space#", " "));
                CIC_SendCommand(ControlName);
                return;
            break;
            case "CIC_LoadChain" :
                if (Boarding_Busy(pchar))
                {
                    if (!sti(pchar.SeaAI.Boarding.Owner)) return;
                }
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_7", "Logs.txt", "#space#", " "));
                CIC_SendCommand(ControlName);
                return;
            break;
            case "CIC_LoadBombs" :
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_8", "Logs.txt", "#space#", " "));
                CIC_SendCommand(ControlName);
                return;
            break;
            case "CIC_LoadAtWill" :
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_9", "Logs.txt", "#space#", " "));
                CIC_SendCommand(ControlName);
				PlayVoice("INTERFACE\SignalCompanion.wav");
                return;
            break;
            case "CIC_SailAway" :
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_10", "Logs.txt", "#space#", " "));
                CIC_SendCommand(ControlName);
				PlayVoice("INTERFACE\SignalCompanion.wav");
                return;
            break;
            case "CIC_LowerSails" :
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_11", "Logs.txt", "#space#", " "));
                CIC_SendCommand(ControlName);
				PlayVoice("INTERFACE\"+LanguageGetLanguage()+"_strike_down.wav");
                return;
            break;
            case "CIC_ProtFlagship" :
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_12", "Logs.txt", "#space#", " "));
                CIC_SendCommand(ControlName);
				PlayVoice("INTERFACE\SignalCompanion.wav");
                return;
            break;
            //// {*} BUHO END ADDITION
			case "GoCabin":
				// LEO: тот же гейт, что на кнопке каюты в меню - хоткей шёл мимо интерфейса
				if (Sea_BlockedByBalls()) return;
				Sea_CabinStartNow();
                CIC_SendCommand(ControlName);
                return;
            break;
		}
	}
	else
	{
		switch(ControlName)
		{
			case "ZoomIn":
				CalcWheelDistance(-1);
			break;
			case "ZoomOut":
				CalcWheelDistance(1);
			break;
			case "SwitchShootingMode":// переключение режима стрельбы для двустволок
				string sCurrentMode = "DoubleShot";
				if (CheckAttribute(pchar,"ShootingMode"))
				{
					if (pchar.ShootingMode == sCurrentMode)
					{
						sCurrentMode = "SingleShot";
					}
				}
				pchar.ShootingMode = sCurrentMode;
				log_info(XI_ConvertString(sCurrentMode));
				return;
			break;
			case "GoCabin":
				if (isShipInside(pchar.location) || CheckShipSituation_GenQuest())
				{
					if (!chrDisableReloadToLocation) Return2SeaAfterCabin(); // Фикс Тича с выходом в море - блок через хоткей
				}
				return;
			break;
            case "BOAL_UsePotion": // boal KEY_X
                if (bLandInterfaceStart)
				{
					bool bEnablePotion1 = true;
					if(CheckAttribute(PChar, "AcademyLand"))
					{
						if(PChar.AcademyLand == "Start")
						{
							bEnablePotion1 = sti(PChar.AcademyLand.Temp.EnablePotion);
						}
					}
					if(CheckAttribute(PChar, "Arena.Mode"))
					{
						if (PChar.Arena.Mode == "Duel" || PChar.Arena.Mode == "Tournament") bEnablePotion1 = false;
					}
					if(!bEnablePotion1)
					{
						Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_13", "Logs.txt", "#space#", " "));
					}
					else
					{
						if(LAi_IsPoison(pchar))
						{
							if (UseBestPotion(pchar, true) == 0)
							{
								if (UseBestPotion(pchar, false) == 0)
								{
									Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_14", "Logs.txt", "#space#", " "));
								}
							}
						}
						else
						{
							if (UseBestPotion(pchar, false) == 0)
							{
								if (UseBestPotion(pchar, true) == 0)
								{
									Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_15", "Logs.txt", "#space#", " "));
								}
							}
						}
					}
				}
            return;
            break;
			case "csmRunLootCollector":
				// LEO: в бою на суше сундук заперт - только стук в ответ (абордаж/штурм города не трогаем)
				if (csmLootCollectorAllowed())
				{
					csmLootCollector();
				}
				else
				{
					PlaySound("interface\knock.wav");
				}
			break;
            case "BOAL_ActivateRush":  // boal KEY_F
				if (bLandInterfaceStart)
				{
					if (GetCharacterPerkUsing(pchar, "Rush") && LAi_IsFightMode(pchar) && PChar.location != "FencingTown_Arena" && PChar.location != "FencingTown_ExitTown")
					{
						ActivateCharacterPerk(pchar, "Rush");
						PlayVoice(GetSexPhrase("interface\Bers_"+rand(5)+".wav","interface\Bersf_"+rand(4)+".wav"));
						int iQueryIndex = 0;
						string sQueryIndex = ""+iQueryIndex;
						float fEnergyMax = pchar.chr_ai.energyMax;
						while(CheckAttribute(pchar, "chr_ai.FoodEnergy."+sQueryIndex)){
							iQueryIndex++;
							sQueryIndex = ""+iQueryIndex;
						}
						pchar.chr_ai.FoodEnergy.(sQueryIndex) = 0.8 * fEnergyMax;//100% - 1...5%(existing energy) - 10-15% (regen)
						pchar.chr_ai.FoodEnergySPD.(sQueryIndex) = 0.08 * fEnergyMax;//0.1 * pchar.chr_ai.FoodEnergy.(sQueryIndex)
					}
					else
                    {
                        PlaySound("knock");
                    }
                }
				return;
            break;

			case "AltModificatorOff":
				// LEO: флаг гасим ПЕРВЫМ делом. Ниже три ранних выхода, и на переходе между локациями
				//   (после Alt+цифра) отпускание Alt через них не доходило до сброса - флаг залипал,
				//   и дальше любая цифра уходила в быстрый переход вместо обычного действия.
				bAltInfo = false;
				if (!IsEntity(&ILogAndActions)) return;
				if (FindLoadedLocation() < 0) return; // LEO: Alt жмут и вне локации - читать её нечего (адрес ссылки тут брать нельзя, см. ModifyTextInfo)
				if(loadedLocation.type == "Underwater") return;
				ModifyTextInfo();
				
				if (bBettaTestMode)
				{
					bLocatorShow = false;
					HideAllLocators();
				}
			break;

			case "AltModificator":
				if (!IsEntity(&ILogAndActions)) return;
				if (FindLoadedLocation() < 0) return; // LEO: Alt жмут и вне локации - читать её нечего (адрес ссылки тут брать нельзя, см. ModifyTextInfo)
				if(loadedLocation.type == "Underwater") return;
				bAltInfo = true;
				SendMessage(&ILogAndActions,"l",LI_CLEAR_STRINGS);
				ModifyTextInfo();
				if (bBettaTestMode)
				{
					bLocatorShow = true;
					ShowAllLocators();
				}
			break;

			case "Fast_port":
				if(bAltInfo)  HKT_Button(ControlName);
			break;

			case "Fast_store":
				if(bAltInfo) HKT_Button(ControlName);

			break;

			case "Fast_Shipyard":
				 if(bAltInfo) HKT_Button(ControlName);
			break;

			case "Fast_tavern":
				if(bAltInfo) HKT_Button(ControlName);
			break;

			case "Fast_townhall":
				if(bAltInfo) HKT_Button(ControlName);
			break;

			case "Fast_bank":
				if(bAltInfo) HKT_Button(ControlName);
			break;

			case "Fast_church":
				if(bAltInfo) HKT_Button(ControlName);
			break;

			case "Fast_Brothel":
				if(bAltInfo) HKT_Button(ControlName);
			break;

			case "Fast_PortOffice":
				if(bAltInfo) HKT_Button(ControlName);
			break;

			case "Fast_prison":
				if(bAltInfo) HKT_Button(ControlName);
			break;

            case "OfficersCharge":
                LandTac_ReleaseAll(); // Gregg: снять командный стейт наземной тактики (этап D)
                pchar.OfficerAttRange = 35.0;
                OfficersFollow();
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_16", "Logs.txt", "#space#", " "));
                return;
            break;
            case "OfficersFree":
                LandTac_ReleaseAll();
                pchar.OfficerAttRange = 35.0;
                OfficersFree();
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_17", "Logs.txt", "#space#", " "));
                return;
            break;
            case "OfficersFollow":
                LandTac_ReleaseAll();
                pchar.OfficerAttRange = 9.0;
                OfficersFollow();
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_18", "Logs.txt", "#space#", " "));
                return;
            break;
            case "OfficersHold":
                LandTac_ReleaseAll();
                pchar.OfficerAttRange = 1.0;
                OfficersHold();
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_19", "Logs.txt", "#space#", " "));
                return;
            break;
            case "LandTacTest": // Gregg: наземный тактический режим — переключатель (Y)
                LandTacToggle();
                return;
            break;
			case "MainMenu": ProcessMainMenuKey();	return; break;
			case "MainMenuDouble": ProcessMainMenuKey(); return; break;
			case "Interface": ProcessInterfaceKey();return; break;
			case "Tele":
				StartQuickTeleport();
			return;
            break;
			case "TeleBack": Teleport(-1); return; break;
			case "Action":
			return;
            break;
		}
	}

    // boal 27.11.03 time -->
  	if (ControlName=="TimeScaleFaster" || ControlName == "TimeScaleSlower" ||
		ControlName=="TimeScaleFasterBA" || ControlName=="TimeScaleSlowerBA")
  	{
		bool CheckForSlowBlock = bAltBalanceTimeSlow || MOD_SKILL_ENEMY_RATE == 3;
		bool CheckForSeaAbordage = LAi_IsBoardingProcess() || !bSeaActive;
		if (CheckForSeaAbordage && CheckForSlowBlock && TimeScaleCounter == 0)
		{
			if (ControlName == "TimeScaleSlower" || ControlName == "TimeScaleSlowerBA")
			{
				/* Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_20", "Logs.txt", "#space#", " "));
				return; */
			}

		}
		if (PChar.location == "FencingTown_Arena" || PChar.location == "FencingTown_ExitTown")
		{
			if (InterfaceStates.Buttons.Save.enable == false)
			{
				if (CheckAttribute(pchar,"SkipBattle") && pchar.SkipBattle == true)
				{
					float EffHp1,EffHp2, ASkill1, ASkill2;
					ASkill1 = LAi_GetCharacterFightLevel(CharacterFromID(pchar.ArenaAttack));
					ASkill2 = LAi_GetCharacterFightLevel(CharacterFromID(pchar.ArenaEnemy));
					if (ASkill1 > ASkill2)
					{
						EffHp1 = GetCharacterEffectiveHp(CharacterFromID(pchar.ArenaAttack)) / (1.0 + 0.7 * (ASkill2 - ASkill1));
						EffHp2 = GetCharacterEffectiveHp(CharacterFromID(pchar.ArenaEnemy));
					}
					else
					{
						EffHp1 = GetCharacterEffectiveHp(CharacterFromID(pchar.ArenaAttack));
						EffHp2 = GetCharacterEffectiveHp(CharacterFromID(pchar.ArenaEnemy)) / (1.0 + 0.7 * (ASkill1 - ASkill2));
					}
					Log_TestInfo("Effhp1:" + sti(effhp1) + " Effhp2:" + sti(effhp2));
					if (EffHp1 > EffHp2) 
						LAi_KillCharacter(CharacterFromID(pchar.ArenaEnemy));
					else
						LAi_KillCharacter(CharacterFromID(pchar.ArenaAttack));
					pchar.SkipBattle = false;
					return;
				}
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_21", "Logs.txt", "#space#", " "));
				return;
			}
		}

		DeleteAttribute(pchar, "pause");
		if (ControlName == "TimeScaleFaster" || ControlName == "TimeScaleFasterBA")
     	{
			if (TimeScaleCounter >= 16){TimeScaleCounter += 16;}
			else
			{
				if (TimeScaleCounter >= 8){TimeScaleCounter += 8;}
				else
				{
					if (TimeScaleCounter >= 4){TimeScaleCounter += 4;}
					else
					{
						if (TimeScaleCounter >= 2){TimeScaleCounter += 2;}
						else {TimeScaleCounter++;}
					}
				}
			}
		}
     	else
     	{
			if (TimeScaleCounter > 16){TimeScaleCounter -= 16;}
			else
			{
				if (TimeScaleCounter > 8){TimeScaleCounter -= 8;}
				else
				{
					if (TimeScaleCounter > 4){TimeScaleCounter -= 4;}
					else
					{
						if (TimeScaleCounter > 2){TimeScaleCounter -= 2;}
						else {TimeScaleCounter--;}
					}
				}
			}
		}
		if (true) // MOD_SKILL_ENEMY_RATE > 1) // запрет 0.25 скорости
        {
		   if (!bBettaTestMode && TimeScaleCounter < -2) TimeScaleCounter = -2;
		}
        if (bDisableMapEnter)
        {
            if (TimeScaleCounter > 32) TimeScaleCounter = 32;
        }
        else
        {   // без боя
            if (bSeaActive && !bAbordageStarted )
            {
            	if (TimeScaleCounter > 48) TimeScaleCounter = 48; // море
            }
            else
            {
            	if (TimeScaleCounter > 32) TimeScaleCounter = 32; // суша
				//Квест "Кот в мешке" ==>
				//Купаясь с акулой нельзя ускоряться.
				if (CheckAttribute(pchar, "questTemp.CSP_KvM_Akula"))
				{
					if (TimeScaleCounter > 4) TimeScaleCounter = 4;
				}
				//Квест "Кот в мешке" <==
            }
        }
        if (IsEntity(worldMap))
        {
            if (TimeScaleCounter > 32) TimeScaleCounter = 32;
        }
     	float newTimeScale = 1 + (TimeScaleCounter)*0.25; // GetSeaTimeScale()
     	if (newTimeScale < 0) //don't wanna crash the game
     	{
        		TimeScaleCounter = -4;
        		newTimeScale = 0;
     	}

     	Log_SetStringToLog("x" + newTimeScale);
     	if (TimeScaleCounter == 0) //back to normal
     	{
			if(IsPerkIntoList("TimeSpeed"))
        	{
        		SetTimeScale(1.0);
        		DelPerkFromActiveList("TimeSpeed");
        	}
     	}
     	else
     	{
        	SetTimeScale(newTimeScale);
        	//if perk not in list
        	if(!IsPerkIntoList("TimeSpeed"))
        	{
				AddPerkToActiveList("TimeSpeed");
			}
     	}
        return;
	}
	// boal <--
	if (HasSubStr(ControlName,"Telescope"))
	{
		aref arItmScope;
		if( Items_FindItem(GetCharacterEquipByGroup(pchar,SPYGLASS_ITEM_TYPE),&arItmScope)>=0 )
		string texName = "telescope.tga.tx";
		float fZoom = 5.5;
		int updateTime = 150;
		int izoomTime = 400; // LEO: время плавного перехода зума трубы (вход/выход/смена уровня), мс
		int ilvl = 0;        // LEO: рабочая переменная уровня зума (5 уровней)
		if( CheckAttribute(arItmScope,"scope.texture") )
		{	
			texName = arItmScope.scope.texture;
		}
		if( CheckAttribute(arItmScope,"scope.zoom") )
		{	
			fZoom = stf(arItmScope.scope.zoom);
		}
		if( CheckAttribute(arItmScope,"scope.time_update") )
		{	
			updateTime = sti(arItmScope.scope.time_update);
		}
	}
	switch(ControlName)
	{
        case "TimeScale":
            DeleteAttribute(pchar, "pause");
			if (PChar.location == "FencingTown_Arena" || PChar.location == "FencingTown_ExitTown")
			{
				if (InterfaceStates.Buttons.Save.enable == false)
				{
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_22", "Logs.txt", "#space#", " "));
					return;
				}
			}
			if(IsPerkIntoList("TimeSpeed"))
			{
				SetTimeScale(1.0);
				TimeScaleCounter = 0;
                bGameLogSkip = 1; Log_SetStringToLog("Time x1"); bGameLogSkip = 0; // LEO: не в Журнал игры
				DelPerkFromActiveList("TimeSpeed");
			}
			else
			{
				float fTimeAccelScale = GetSeaTimeScale();
				SetTimeScale(fTimeAccelScale);
				TimeScaleCounter = makeint((fTimeAccelScale - 1.0) / 0.25);
                bGameLogSkip = 1; Log_SetStringToLog("Time x" + its(makeint(fTimeAccelScale))); bGameLogSkip = 0; // LEO: не в Журнал игры
				AddPerkToActiveList("TimeSpeed");
			}
		break;

		case "VK_PAUSETimePause":
			if (PChar.location == "FencingTown_Arena" && PChar.location == "FencingTown_ExitTown")
			{
				if (InterfaceStates.Buttons.Save.enable == false)
				{
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_23", "Logs.txt", "#space#", " "));
					return;
				}
			}
			if (!CheckAttribute(pchar, "pause"))
			{
				pchar.pause = true;
				SetTimeScale(0.0);
			    TimeScaleCounter = -3;
			    AddPerkToActiveList("TimeSpeed");
			}
			else
			{
                DeleteAttribute(pchar, "pause");
				SetTimeScale(1.0);
				DelPerkFromActiveList("TimeSpeed");
				TimeScaleCounter = 0;
			}
		break;
		case "WMapCancel":
			if(IsEntity(&worldMap))
			{
				pchar.space_press = 1;
				DeleteAttribute(pchar, "SkipEshipIndex");// boal
			}
		break;
		// LEO: миникарта глобалки — P вкл/выкл, Б/Ю зум охвата (как радар на суше)
		case "WMapRadarToggle":
			if(IsEntity(&worldMap)) WorldRadar_ToggleLive();
		break;
		case "WMapRadarRangeDown": // < (запятая, Б) — зум-АУT (охват больше), как на суше
			if(IsEntity(&worldMap)) WorldRadar_AdjustRange(1.25);
		break;
		case "WMapRadarRangeUp":   // > (точка, Ю) — зум-ИН (охват меньше), как на суше
			if(IsEntity(&worldMap)) WorldRadar_AdjustRange(0.8);
		break;

		case "MushketHotkey":
			if (!(bAltInfo && curKeyGroupName == "PrimaryLand" && MusketSharesStoreKey()))
				TogglePlayerMusket();
		break;

		case "PusherHotkey":
			if (!CheckCharacterPerk(pchar, "Buldozer")) break;

			if (!CheckAttribute(pchar,"CSM.CrowdPusher"))
			{
				Log_Info(GetConvertStrWithReplace("Variable_seadogs_24", "Logs.txt", "#space#", " "));
				SM_PusherEnable();
			}
			else
			{
				Log_Info(GetConvertStrWithReplace("Variable_seadogs_25", "Logs.txt", "#space#", " "));
				SM_PusherDisable();
			}
		break;

		case "Map_Best":
			if(CheckCharacterItem(PChar, "Map_Best") || bBettaTestMode) LaunchBestMapScreen();
		break;
		
		case "DropMine":
			if (bSeaActive && !bAbordageStarted && GetCargoGoods(PChar, GOOD_POWDER) >= 200) SetMineFree(PChar, 1); // fix ugeen 21.12.13
			else PlaySound("interface\knock.wav");
		break;
		
		// LEO: 5 уровней зума трубы. Уровень 1..5, множитель = 1 + (макс_зум - 1) * уровень/5.
		// Уровень 5 = scope.zoom конкретной трубы («своя дальность»). Вход/выход/смена уровня — плавно
		// (izoomTime>0, движок сглаживает экспоненциально). pchar.izoom — итоговый множитель, izoomlevel — уровень.
		case "TelescopeIn":
			pchar.izoomlevel = 1;
			ilvl = sti(pchar.izoomlevel);
			pchar.izoom = 1.0 + (fZoom - 1.0) * makefloat(ilvl) / 5.0;
			SendMessage(&objISpyGlass, "lsfll", MSG_TELESCOPE_SET_TYPE, texName, stf(pchar.izoom), izoomTime, updateTime);
		break;

		case "TelescopeOut":
			pchar.izoomlevel = 1;
			pchar.izoom = 1.0;
			SendMessage(&objISpyGlass, "lsfll", MSG_TELESCOPE_SET_TYPE, texName, stf(pchar.izoom), izoomTime, updateTime);
		break;

		case "TelescopeZoomIn":
			ilvl = sti(pchar.izoomlevel);
			if (ilvl < 5)
			{
				ilvl = ilvl + 1;
				pchar.izoomlevel = ilvl;
				pchar.izoom = 1.0 + (fZoom - 1.0) * makefloat(ilvl) / 5.0;
				SendMessage(&objISpyGlass, "lsfll", MSG_TELESCOPE_SET_TYPE, texName, stf(pchar.izoom), izoomTime, updateTime);
			}
		break;

		case "TelescopeZoomOut":
			ilvl = sti(pchar.izoomlevel);
			if (ilvl > 1)
			{
				ilvl = ilvl - 1;
				pchar.izoomlevel = ilvl;
				pchar.izoom = 1.0 + (fZoom - 1.0) * makefloat(ilvl) / 5.0;
				SendMessage(&objISpyGlass, "lsfll", MSG_TELESCOPE_SET_TYPE, texName, stf(pchar.izoom), izoomTime, updateTime);
			}
		break;

		// --> ugeen
		case "MapView":
			bOk = bSeaActive && !bAbordageStarted;
			bOk1 = bLandInterfaceStart && !LAi_IsFightMode(pchar);
			if(bOk || bOk1)
			{
				// LEO: Счётчик достижения может отставать от карт в инвентаре.
				if(CheckCharacterItem(PChar, "MapsAtlas")) LaunchMapViewScreen();
			}
		break;
		// <-- ugeen
		case "BOAL_UsePotion": // boal KEY_C
            if (bLandInterfaceStart)
            {
				bool bEnablePotion = true;
				if(CheckAttribute(PChar, "AcademyLand"))
				{
					if(PChar.AcademyLand == "Start")
					{
						bEnablePotion = sti(PChar.AcademyLand.Temp.EnablePotion);
					}
				}
				if(CheckAttribute(PChar, "Arena.Mode"))
				{
					if (PChar.Arena.Mode == "Duel" || PChar.Arena.Mode == "Tournament") bEnablePotion = false;
				}
				if(!bEnablePotion)
				{
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_26", "Logs.txt", "#space#", " "));
				}
				else
				{
					if(LAi_IsPoison(pchar))
					{
						if (UseBestPotion(pchar, true) == 0)
						{
							if (UseBestPotion(pchar, false) == 0)
							{
								Log_SetStringToLog("No Potion Found!");
							}
						}
					}
					else
					{
						if (UseBestPotion(pchar, false) == 0)
						{
							if (UseBestPotion(pchar, true) == 0)
							{
								Log_SetStringToLog("No Potion Found!");
							}
						}
					}
				}
	        }
		break;

		// Warship 13.06.09 Выпить противоядие KEY_V
		case "UseAntidote":
			if(bLandInterfaceStart)
            {
				if(CheckAttribute(PChar, "Arena.Mode"))
				{
					if (PChar.Arena.Mode == "Duel" || PChar.Arena.Mode == "Tournament") break;
				}
				if(FindCharacterAntidote(PChar, &itemID)) // В itemID запишется ID предмета, который можно использовать
				{
					DoCharacterUsedItem(PChar, itemID);
					Log_Info(GetConvertStrWithReplace("Variable_seadogs_27", "Logs.txt", "#space#", " "));
				}
			}
		break;
		case "PaperMap":
			LaunchPaperMapScreen();
		break;

        case "BOAL_ActivateRush":  // boal KEY_F
			if (bLandInterfaceStart)
            {
				if (GetCharacterPerkUsing(pchar, "Rush") && LAi_IsFightMode(pchar))
				{
					ActivateCharacterPerk(pchar, "Rush");
					PlayVoice(GetSexPhrase("interface\Bers_"+rand(5)+".wav","interface\Bersf_"+rand(4)+".wav"));
					iQueryIndex = 0;
					sQueryIndex = ""+iQueryIndex;
					fEnergyMax = pchar.chr_ai.energyMax;
					while(CheckAttribute(pchar, "chr_ai.FoodEnergy."+sQueryIndex)){
						iQueryIndex++;
						sQueryIndex = ""+iQueryIndex;
					}
					pchar.chr_ai.FoodEnergy.(sQueryIndex) = 0.8 * fEnergyMax;//100% - 1...5%(existing energy) - 10-15% (regen)
					pchar.chr_ai.FoodEnergySPD.(sQueryIndex) = 0.08 * fEnergyMax;//0.1 * pchar.chr_ai.FoodEnergy.(sQueryIndex)
				}
				else
                {
                    PlaySound("interface\knock.wav");
                }
            }
	    break;

//JA 16/8/09 -->
		case "BOAL_ActivateTurn180":
		if (!CheckOfficersPerk(pchar, "Turn180") && GetOfficersPerkUsing(pchar, "Turn180"))
		{
			ActivateCharacterPerk(pchar,"Turn180");
		}
		else
		{
			Log_Info(GetConvertStrWithReplace("Variable_seadogs_28", "Logs.txt", "#space#", " "));
			PlaySound("interface\knock.wav");
		}
		break;

		case "BOAL_ActivateImmediateReload":
		if(bSeaActive == true)
		{
			if (!CheckOfficersPerk(pchar, "ImmediateReload") && GetOfficersPerkUsing(pchar, "ImmediateReload"))
			{
				ActivateCharacterPerk(pchar,"ImmediateReload");
			}
			else
			{
				Log_Info(GetConvertStrWithReplace("Variable_seadogs_29", "Logs.txt", "#space#", " "));
				PlaySound("interface\knock.wav");
			}
		}
		break;

		case "BOAL_ActivateLightRepair":
		if(bSeaActive == true)
		{
			if (!CheckOfficersPerk(pchar, "LightRepair") && GetOfficersPerkUsing(pchar,"LightRepair"))
			{
				ActivateCharacterPerk(pchar,"LightRepair");
				ActivateSpecRepair(pchar,0);
			}
			else
			{
				Log_Info(GetConvertStrWithReplace("Variable_seadogs_30", "Logs.txt", "#space#", " "));
				PlaySound("interface\knock.wav");
			}
		}
		break;

		case "BOAL_ActivateInstantRepair":
		if(bSeaActive == true)
		{
			if (CheckInstantRepairCondition (pchar))
			{
				ActivateCharacterPerk(pchar,"InstantRepair");
				ActivateSpecRepair(pchar,1);
			}
			else
			{
				Log_Info(GetConvertStrWithReplace("Variable_seadogs_31", "Logs.txt", "#space#", " "));
				PlaySound("interface\knock.wav");
			}
		}
		break;
	    case "BOAL_DeadSearch":  // boal KEY_1
			if (bLandInterfaceStart && !bAltInfo)
            {
                itmIdx = Dead_FindCloseBody();
			    if (itmIdx != -1)
			    {
					Dead_OpenBoxProcedure();
				}
				else
				{
                    Log_Info(GetConvertStrWithReplace("Variable_seadogs_32", "Logs.txt", "#space#", " "));
				}
		    }
	    break;
//<--JA
		case "BOAL_SetCamera":
		    // по F10
		    if (MOD_BETTATESTMODE == "On")
		    {
                if(LoadSegment("Debuger.c"))
            	{
                    ActiveF10Control();
            		UnloadSegment("Debuger.c");
            	}
		    }
			else LaunchPsHeroScreen();
		break;
        case "BOAL_Control":
		    // по F11 вызывает окно отладчика
		    if (MOD_BETTATESTMODE == "On" || MOD_BETTATESTMODE == "Test")
		    {
		       LaunchDebuderMenu();
		    }
		break;

		case "Say": // KEY_Y
			// Интерфейс автозакупки товаров
			if(bLandInterfaceStart && IsPCharHaveTreasurer()) // Если есть казначей и ГГ находится на суше
				LaunchTransferGoodsScreen();
		break;
        case "TeleportActive":
		    if (MOD_BETTATESTMODE == "On")
		    {
                if(LoadSegment("Debuger.c"))
            	{
                    ActiveF4Control();
            		UnloadSegment("Debuger.c");
            	}
		    }
		break;
		case "BOAL_ControF5":
            if (MOD_BETTATESTMODE == "On")
            {
                if(LoadSegment("Debuger.c"))
                {
                    ActiveF5Control();
                    UnloadSegment("Debuger.c");
                }
                Statistic_AddValue(PChar, "Cheats.F5", 1);
            }
        break;
        case "BOAL_ControF7":
            // по F7 вызывает окно отладчика
            if (MOD_BETTATESTMODE == "On")
            {
                if(LoadSegment("Debuger.c"))
                {
                    ActiveF7Control();
                    UnloadSegment("Debuger.c");
                }
                Statistic_AddValue(PChar, "Cheats.F7", 1);
            }
        break;
        case "BOAL_Control2": // F12
            //Найти ближайшего видимого персонажа в заданном радиусе
            if(LoadSegment("Debuger.c"))
        	{
                ActiveF12Control(); // можно меять пряв в рабочей игре
        		UnloadSegment("Debuger.c");
        	}
        break;
        case "BOAL_ControlDebug": // VK_INSERT
            if (MOD_BETTATESTMODE == "On")
		    {
                if(LoadSegment("Debuger.c"))
            	{
                    ActiveINSERTControl();
            		UnloadSegment("Debuger.c");
            	}
        	}
        break;
		case "UseFood":
			if (bLandInterfaceStart)
			{
				bool bEnableFood = true;
				if(CheckAttribute(PChar, "AcademyLand"))
				{
					if(PChar.AcademyLand == "Start")
					{
						bEnableFood = sti(PChar.AcademyLand.Temp.EnableFood);
					}
				}

				if(!bEnableFood)
				{
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_seadogs_33", "Logs.txt", "#space#", " "));
				}
				else
				{
					pchar.pressedFoodButton = true;
					EatSomeFood();
				}
			}
		break;
        // boal <--
		case "CreateNotice":
			LaunchCreateNotice();
		break;
		case "InterfaceDisabler":
			ChangeShowIntarface();
		break;
		case "drawShootDistance":
			fnDrawShootDistance();
		break;
		case "setEquipment1":
			if (bLandInterfaceStart && !bDisableCharacterMenu)
            {
				if(CheckAttribute(PChar, "AcademyLand"))
				{
					if(LoadSegment("interface\items.c"))
					{
						setEquipmentComplect(1);
						UnloadSegment("interface\items.c");
					}
				}
			}
		break;
		case "setEquipment2":
			if (bLandInterfaceStart && !bDisableCharacterMenu)
            {
				if(CheckAttribute(PChar, "AcademyLand"))
				{
					if(LoadSegment("interface\items.c"))
					{
						setEquipmentComplect(2);
						UnloadSegment("interface\items.c");
					}
				}
			}
		break;
		case "setEquipment3":
			if (bLandInterfaceStart && !bDisableCharacterMenu)
            {
				if(CheckAttribute(PChar, "AcademyLand"))
				{
					if(LoadSegment("interface\items.c"))
					{
						setEquipmentComplect(3);
						UnloadSegment("interface\items.c");
					}
				}
			}
		break;
		case "setEquipment4":
			if (bLandInterfaceStart && !bDisableCharacterMenu)
            {
				if(CheckAttribute(PChar, "AcademyLand"))
				{
					if(LoadSegment("interface\items.c"))
					{
						setEquipmentComplect(4);
						UnloadSegment("interface\items.c");
					}
				}
			}
		break;
		case "setEquipment5":
			if (bLandInterfaceStart && !bDisableCharacterMenu)
            {
				if(CheckAttribute(PChar, "AcademyLand"))
				{
					if(LoadSegment("interface\items.c"))
					{
						setEquipmentComplect(5);
						UnloadSegment("interface\items.c");
					}
				}
			}
		break;
		case "setEquipment6":
			if (bLandInterfaceStart && !bDisableCharacterMenu)
            {
				if(CheckAttribute(PChar, "AcademyLand"))
				{
					if(LoadSegment("interface\items.c"))
					{
						setEquipmentComplect(6);
						UnloadSegment("interface\items.c");
					}
				}
			}
		break;
		// LEO: сухопутный радар/миникарта — вкл/выкл (P). Работает на суше, в т.ч. в бою.
		case "RadarToggle":
			// LEO Этап 3: карта показывается ТОЛЬКО хоткеем и лишь если фича включена чекбоксом в настройках (RadarEnabled)
			if (bLandInterfaceStart && sti(InterfaceStates.RadarEnabled) == 1)
			{
				int rv = 1 - sti(InterfaceStates.RadarVisible);
				InterfaceStates.RadarVisible = rv;
				LandRadar_ReconcileVisibility(); // LEO: применить видимость через общий Reconcile (учтёт диалог/катсцену, синхронит кэш)
				if (rv == 1) LandRadar_BuildPOI(); // LEO этап 2б: обновить предметы/сундуки при включении (ловит подобранное)
				PlaySound("interface\ok.wav");
			}
		break;
		case "RadarRangeUp":   // LEO: > (точка) — зум-ин (дальность меньше), плавно
			LandRadar_AdjustRange(0.8);
		break;
		case "RadarRangeDown": // LEO: < (запятая) — зум-аут (дальность больше), плавно
			LandRadar_AdjustRange(1.25);
		break;
	}
}

void Teleport(int step)
{
	nTeleportLocation = nTeleportLocation + step;
	if(nTeleportLocation >=	nLocationsNum) nTeleportLocation = 1;
	if(nTeleportLocation <=	0) nTeleportLocation = nLocationsNum - 1;

	sTeleportLocName = Locations[nTeleportLocation].id;
	Trace("Teleport to '" + sTeleportLocName + "'");

    ReleaseSound(0);
	ClearEvents();
	ClearPostEvents();
	DeleteEntities();
	SetEventHandler("frame","NewGame",1);
}

void ProcessMainMenuKey()
{
	if (CheckAttribute(GetMainCharacter(), "LandTacOn")) LandTacToggle(); // Gregg: F1/ESC в тактической паузе — сначала корректно выйти (иначе меню ломает паузу: ходьба на месте)
	if (interfacestates.buttons.resume.enable == "1")
	{
		LaunchGameMenuScreen();
	}
}
// вызов меню по Ф2
void ProcessInterfaceKey()
{
	/* if( CharacterIsDead(GetMainCharacter()) ) {return;}
	if(bSeaActive && !bAbordageStarted)
	{
		if( CheckAttribute(&BattleInterface,"ComState") && sti(BattleInterface.ComState) != 0 )
			{return;}
	}
	else
	{
		if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") != 0 )
			{return;}
		if( CheckAttribute(&objLandInterface,"ComState") && sti(objLandInterface.ComState) != 0 )
			{return;}
	}
	if (bDisableCharacterMenu) // boal
	{
		Log_SetStringToLog(XI_ConvertString("You can`t launch menu"));
		return;
	}
	if (bAbordageStarted && !bCabinStarted && !bDeckBoatStarted)
	{
		InterfaceStack.SelectMenu_node = "LaunchItems";
		LaunchSelectMenu();
		return;
	} */
	LaunchSelectMenu();
}

void GameOverE()
{
	// вылетам у форта НЕТ -->
    if (bSeaActive && !bAbordageStarted)
    {
		SendMessage(&AIBalls, "l", MSG_MODEL_RELEASE);
	}
	// вылетам у форта НЕТ <--
	GameOver("sea");
	DeleteSeaEnvironment();
}

void GameOver(string sName)
{
	ref mc;
	int nSelect;
	string sFileName;
	PauseAllSounds();
	ResetSound();
	EngineLayersOffOn(false);

	mc = GetMainCharacter();

	ClearEvents();
	ClearPostEvents();
	DeleteEntities();

	if(sti(InterfaceStates.Launched)) {
		UnloadSegment(Interfaces[CurrentInterface].SectionName);
	}

	if(LoadSegment("Interface\BaseInterface.c"))
	{
		InitBaseInterfaces();
		UnloadSegment("Interface\BaseInterface.c");
	}
	InterfaceStates.showGameMenuOnExit = false;
	InterfaceStates.Buttons.Resume.enable = false;
	InterfaceStates.Buttons.Save.enable = false;

	InitSound();
	SetEventHandler(EVENT_END_VIDEO,"LaunchMainMenu_afterVideo",0);
	SetEventHandler("Control Activation","proc_break_video",0);
	FadeOutMusic(3);
	switch(sName)
	{
		case "sea":
			StartPictureAsVideo( "loading\jonny_load\death\end_game_sea_"+rand(1)+".dds", 4 );
			PlayStereoOGG("music_ship_dead");
		break;
		case "boarding":
			StartPictureAsVideo( "loading\jonny_load\death\end_game_sea_"+rand(1)+".dds", 4 );
			PlayStereoOGG("music_ship_dead");
		break;
		case "land":
			StartPictureAsVideo( "loading\jonny_load\death\end_game_"+rand(1)+".dds", 4 );
			PlayStereoOGG("music_death");
		break;
		case "mutiny":
			StartPictureAsVideo( "loading\jonny_load\death\end_game_"+rand(1)+".dds", 4 );
			PlayStereoOGG("music_death");
		break;
		case "town":
			StartPictureAsVideo( "loading\jonny_load\death\end_game_"+rand(1)+".dds", 4 );
			PlayStereoOGG("music_death");
		break;
		case "blood":
			StartPictureAsVideo( "loading\jonny_load\death\end_game_"+rand(1)+".dds", 4 );
			PlayStereoOGG("music_death");
		break;
	}
}

string its(int iNumber)
{
	string sText = iNumber;
	return sText;
}

// тяжкая игра - сайв токо в церкви 17.03.05 boal
bool QuickSaveGameEnabledHardcore()
{
    bool TmpBool = false;
    ref mchref = GetMainCharacter();

	if (bHardcoreGame)
	{
		int idxLoadLoc = FindLoadedLocation();
	    if( idxLoadLoc!=-1 )
	    {
	        if (CheckAttribute(&Locations[idxLoadLoc], "type"))
	        {
	            if (Locations[idxLoadLoc].type == "tavern" || Locations[idxLoadLoc].id == "FleuronTavern")
	            {
	                TmpBool = true;
	            }
	        }
	    }
    }
    else
    {
        TmpBool = true;
    }

    return TmpBool;
}

bool CheckBattleSeaSaveEnabled()
{
	string sSaveEnable = GetConvertStr("SeaBattleMode_SaveEnable", KVL_MODS_FILE);
	string sSmallRegStr = GetStrSmallRegister(sSaveEnable);
	if(sSmallRegStr == "on" || sSmallRegStr == "да")
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CheckSaveGameEnabled()
{
    if (Throwing3D_IsActive() || CheckAttribute(pchar,"Throwing.pending")) return false;
    // LEO: в зале славы не сохраняются. Гард по локации, а не по Buttons.Save.enable: фейдер
    //   перехода запоминает старое значение флага и возвращает его после загрузки (LAi_utils).
    if( CheckAttribute(pchar, "location") && pchar.location == FINAL_GAME_LOCATION ) return false;

    bool TmpBool = true;
    ref mchref = GetMainCharacter();

    if (CheckAttribute(&InterfaceStates, "Buttons.Save.enable"))
	{
		if(InterfaceStates.Buttons.Save.enable != 1)
		{
			TmpBool = false;
		}
	}
	else
	{
		TmpBool = false;
	}

	if (bAbordageStarted) {TmpBool = false;}

	if(LAi_IsBoardingProcess()) {TmpBool = false;}

	if( CharacterIsDead(mchref) ) {TmpBool = false;}

	if(LAi_IsFightMode(mchref) && MOD_SKILL_ENEMY_RATE > 1) TmpBool = false;

	if(bDisableMapEnter && !CheckBattleSeaSaveEnabled()) TmpBool = false;

	if (TmpBool)
	{
		int idxLoadLoc = FindLoadedLocation();
	    if (idxLoadLoc != -1 )
	    {
	        if (Locations[idxLoadLoc].id == "Ship_deck" || Locations[idxLoadLoc].id == "Deck_Near_Ship" ) // сайв на палубе глюкавый
	        {
	            TmpBool = false;
	        }
			if (CheckAttribute(Locations[idxLoadLoc],"fastreload"))
			{
				if (!CheckAttribute(Pchar, "questTemp.CapBloodLine") || Pchar.questTemp.CapBloodLine == false)
				{
					string sNation = Colonies[FindColony(Locations[idxLoadLoc].fastreload)].nation;//фикс ошибок в логах?
					if (sNation != "none" && sNation != "4")
					{
						int i = sti(sNation);
						bool bTmpBool = (GetNationRelation2MainCharacter(i) == RELATION_ENEMY) || GetRelation2BaseNation(i) == RELATION_ENEMY;
						if (HasSubStr(Locations[idxLoadLoc].id, "_Town") && bTmpBool) TmpBool = false;
					}
				}
			}
	    }

	}
    return TmpBool;
}

//Boyer change #20170418-01
void restoreQuestItems()
{
    ref		rItemRef;
    if(CheckAttribute(pchar, "restoreLSCKeymasterKey")){
        rItemRef =  ItemsFromID(pchar.restoreLSCKeymasterKey);
        rItemRef.shown = true;
        rItemRef.startLocation = "Villemstad_houseSp5";
        rItemRef.startLocator = "item2";
    }
    if(CheckAttribute(pchar, "restoreLSCTrustLetter")){
        ChangeItemName(pchar.restoreLSCTrustLetter, "itmname_letter_LSC_1");
        ChangeItemDescribe(pchar.restoreLSCTrustLetter, "itmdescr_letter_LSC_1");
    }
	if(CheckAttribute(pchar, "HugtorpQuestStart") && !CheckAttribute(pchar, "HugtorpQuestFinish"))
	{
		ref itm = ItemsFromID("DOjeronRing");
		itm.picIndex = 15;
		itm.picTexture = "ITEMS_13";
		ChangeItemName("DOjeronRing", "itmname_HugtorpRing");
		ChangeItemDescribe("DOjeronRing", "itmdescr_HugtorpRing");
    }
}

//Boyer change #20170418-01
void removeQuestItemAttribute(string itemID)
{
    switch(itemID)
    {
        case "keyQuestLSC":
            if(CheckAttribute(pchar, "restoreLSCKeymasterKey"))
                DeleteAttribute(pchar, "restoreLSCKeymasterKey");
        break;
        case "letter_LSC":
            if(CheckAttribute(pchar, "restoreLSCTrustLetter"))
                DeleteAttribute(pchar, "restoreLSCTrustLetter");
        break;
    }
}

extern void SetCurrentProfile( string profileName );
//#20171009-03
void ResetDevice()
{
	if(InterfaceStates.Launched == true) {
        if(CurrentInterface == INTERFACE_SAVELOAD) {
            SetCurrentProfile( PlayerProfile.name );
        }
	}
	else {
        ChangeShowIntarface();
        ChangeShowIntarface();
	}
}
int iCalculateSaveLoadCount(string _SaveLoad)
{
	int iCount;
	ref MChref = GetMainCharacter();
	string sAttrName = _SaveLoad + "Count";
	iCount = sti(MChref.SystemInfo.(sAttrName));
	iCount++;
	MChref.SystemInfo.(sAttrName) = iCount;
	return iCount;
}

void CalcWheelDistance(int where)
{
    InterfaceStates.RadDetails = Clampf(stf(InterfaceStates.RadDetails)+(0.1*where));
    float fMod = GetMultiplyFromScreenRatio(); //добавил множитель от соотношения сторон экрана
    SendMessage(&locCamera, "lf", MSG_CAMERA_SET_RADIUS, CalcLandRadiusNew() * fMod);
}
