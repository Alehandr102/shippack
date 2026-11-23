#include "interface\options_game_settings.c"
#include "interface\options_newhud.c"
#include "interface\options_shipinfobars.c"
#include "interface\options_cannon_heat_haze.c"
#include "interface\options_flag_physics.c"
#include "interface\options_ragdoll.c"

#define PANIM_QUANTITY	12	// LEO: Стандартная анимация и варианты из OptionalMods.

int g_nCurControlsMode = -1;
int g_ControlsLngFile = -1;
bool blockkey = true;
string curkey = "";
bool g_bToolTipStarted = false;
int OptionsTab = 1;

float 	fHUDRatio 	= 1.0;
float 	newBase 	= stf(screenscaling);

#define BI_LOW_RATIO 	0.25
#define BI_HI_RATIO 	4.0
#define BI_DIF_RATIO 	3.75

float CalcHUDBase(float fSlider, float MyScreen)
{
    float fRes = BI_DIF_RATIO * fSlider;
    float curBase = MyScreen / (BI_LOW_RATIO + fRes);

    return curBase;
}

float CalcHUDSlider(float fRatio)
{
    float fRes = fRatio - BI_LOW_RATIO;
    fRes /= BI_DIF_RATIO;

    return fRes;
}

float roundFloat(float value) {
	float newValue = stf(round(value * 100.0)) / 100.0
	float ostatok = sti(newValue * 100) % 5 / 100.0
	return newValue - ostatok
}

void InitInterface(string iniName)
{
	g_nCurControlsMode = -1;
	GameInterface.title = "titleOptions";
	g_ControlsLngFile = LanguageOpenFile("ControlsNames.txt");

	bool fromGame=false;
	if(CheckAttribute(&InterfaceStates,"showGameMenuOnExit")) fromGame=sti(InterfaceStates.showGameMenuOnExit)!=0;
	if(fromGame) {
		LoadGameOptions();
	} else {
		DeleteAttribute( &PlayerProfile, "name" );
		LoadGameOptions();
	}

	IReadVariableBeforeInit();
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	IReadVariableAfterInit();

	SetControlsTabMode(1);

	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("eTabControlPress","procTabChange",0);
	SetEventHandler("eventBtnAction","procBtnAction",0);
	SetEventHandler("eventKeyChange","procKeyChange",0);

	SetEventHandler("CheckButtonChange","procCheckBoxChange",0);
	SetEventHandler("eSlideChange","procSlideChange",0);

	SetEventHandler("evntKeyChoose","procKeyChoose",0);
	SetEventHandler("ShowInfo", "ShowInfo", 0);
	SetEventHandler("MouseRClickUP","HideInfo",0);

	SetEventHandler("checkSoundParam","ClickSoundOptions",0);
	SetEventHandler("checkGameParam","ClickGameOptions",0);
	SetEventHandler("setupGameParam","SetupGameOptions",0);
	SetEventHandler("ePanimSelect","ProcPanimSelect",0);

	SetEventHandler("infoSoundParam","RightClickSoundOptions",0);
	SetEventHandler("infoGameParam","RightClickGameOptions",0);

	SetEventHandler("checkMasterParam","ClickMasterOptions",0);
	SetEventHandler("setupMasterParam","SetupMasterOptions",0);
	SetEventHandler("infoMasterParam","RightClickMasterOptions",0);

	SetEventHandler("evFaderFrame","FaderFrame",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);

	SetFormatedText("TEXT_NATIONS_PIC_SETTINGS", GetConvertStrWithReplace("Variable_option_screen_1", "Interface.txt", "#space#", " "));
	SetFormatedText("TEXT_VISUAL_CIRASS_PIC_SETTINGS", GetConvertStrWithReplace("Variable_option_screen_2", "Interface.txt", "#space#", " "));
	SetFormatedText("TEXT_FONT_QUESTBOOK_PIC_SETTINGS", GetConvertStrWithReplace("Variable_option_screen_3", "Interface.txt", "#space#", " "));
	SetFormatedText("TEXT_PARTICLES_PIC_SETTINGS", GetConvertStrWithReplace("Variable_option_screen_4", "Interface.txt", "#space#", " "));
	SetFormatedText("TEXT_ALT_GUN_SOUNDS_SETTINGS", GetConvertStrWithReplace("Variable_option_screen_5", "Interface.txt", "#space#", " "));
	SetFormatedText("TEXT_INTERFACE_SELECTION", GetConvertStrWithReplace("Variable_option_screen_6", "Interface.txt", "#space#", " "));
	SetFormatedText("TEXT_STREAMERMODE_PIC_SETTINGS", XI_ConvertString("StreamerMode")); 
	SetFormatedText("STR_OPTIONS_SIMPLE", GetConvertStrWithReplace("Variable_option_screen_7", "Interface.txt", "#space#", " "));
	SetFormatedText("STR_OPTIONS_MASTER", GetConvertStrWithReplace("Variable_option_screen_8", "Interface.txt", "#space#", " "));

	aref ar; makearef(ar,objControlsState.key_codes);
	SendMessage(&GameInterface,"lsla",MSG_INTERFACE_MSG_TO_NODE,"KEY_CHOOSER", 0,ar);

	float ftmp1 = -1.0;
	float ftmp2 = -1.0;
	float ftmp3 = -1.0;
	SendMessage(&sound,"leee",MSG_SOUND_GET_MASTER_VOLUME,&ftmp1,&ftmp2,&ftmp3);
	if( ftmp1==-1.0 && ftmp2==-1.0 && ftmp3==-1.0 )
	{
		SetSelectable("MUSIC_SLIDE",false);
		SetSelectable("SOUND_SLIDE",false);
		SetSelectable("DIALOG_SLIDE",false);
	}
	// LEO: Пределы камеры берём из AICameras.c, чтобы подписи не расходились с расчётом.
	float fCamPersp = DEFAULT_CAM_PERSP + (1 / DEFAULT_CAM_PERSP_DEN);
    string sMsg = "#"+ FloatToString(fCamPersp, 3);
    SendMessage(&GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "TITLES_STR", 1, 17, sMsg);
    fCamPersp = DEFAULT_LCAM_PERSP + (1 / DEFAULT_LCAM_PERSP_DEN);
    sMsg = "#"+ FloatToString(fCamPersp, 3);
    SendMessage(&GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "TITLES_STR", 1, 21, sMsg);
    float fCamRad = CalcLandRadiusNew();
    sMsg = "#"+ FloatToString(fCamRad, 1);
    SendMessage(&GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "TITLES_STR", 1, 24, sMsg);

	fHUDRatio = stf(Render.screen_y) / screenscaling;
	float sl = CalcHUDSlider(fHUDRatio);
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HUD_SLIDE", 0, sl);
	GameInterface.nodes.hud_slide.value = sl;
	// LEO: Слайдеры инициализируем до показа окна: изменение на открытом окне подвешивает игру.
	if( !CheckAttribute(&InterfaceStates,"RadarBlipScale") ) InterfaceStates.RadarBlipScale = 1.0;
	if( !CheckAttribute(&InterfaceStates,"RadarRange") ) InterfaceStates.RadarRange = 60.0;
	if( !CheckAttribute(&InterfaceStates,"RadarRangeInside") ) InterfaceStates.RadarRangeInside = 10.0;
	if( !CheckAttribute(&InterfaceStates,"RadarMapRes") ) InterfaceStates.RadarMapRes = 2048;
	float rbsl = (stf(InterfaceStates.RadarBlipScale) - 0.2) / 4.8;
	GameInterface.nodes.radar_blip_scale_slide.value = rbsl;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "RADAR_BLIP_SCALE_SLIDE", 0, rbsl);
	if( !CheckAttribute(&InterfaceStates,"ConsumablesHUDScale") ) InterfaceStates.ConsumablesHUDScale = 1.0;
	float cshs = stf(InterfaceStates.ConsumablesHUDScale);
	if(cshs < 0.5) cshs = 0.5;
	if(cshs > 2.0) cshs = 2.0;
	float cssl = (cshs - 0.5) / 1.5;
	GameInterface.nodes.consumables_scale_slide.value = cssl;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CONSUMABLES_SCALE_SLIDE", 0, cssl);
	if( !CheckAttribute(&InterfaceStates,"ConsumablesHUDPos") ) InterfaceStates.ConsumablesHUDPos = 0.5;
	float cshp = stf(InterfaceStates.ConsumablesHUDPos);
	if(cshp < 0.0) cshp = 0.0;
	if(cshp > 1.0) cshp = 1.0;
	GameInterface.nodes.consumables_pos_slide.value = cshp;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CONSUMABLES_POS_SLIDE", 0, cshp);
	float rmsl = (stf(InterfaceStates.RadarRange) - 20.0) / 280.0;
	GameInterface.nodes.radar_map_scale_slide.value = rmsl;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "RADAR_MAP_SCALE_SLIDE", 0, rmsl);
	float risl = (stf(InterfaceStates.RadarRangeInside) - 5.0) / 115.0;
	GameInterface.nodes.radar_inside_scale_slide.value = risl;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "RADAR_INSIDE_SCALE_SLIDE", 0, risl);
	if( !CheckAttribute(&InterfaceStates,"RadarRadius") ) InterfaceStates.RadarRadius = 210.0;
	float rszl = (stf(InterfaceStates.RadarRadius) - 120.0) / 200.0;
	GameInterface.nodes.radar_size_slide.value = rszl;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "RADAR_SIZE_SLIDE", 0, rszl);
	WorldRadar_InitDefaults();
	float wrszl = (stf(InterfaceStates.WorldRadarSize) - 120.0) / 200.0;
	GameInterface.nodes.wradar_size_slide.value = wrszl;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "WRADAR_SIZE_SLIDE", 0, wrszl);
	float wrzml = (stf(InterfaceStates.WorldRadarRange) - 0.3) / 2.7;
	GameInterface.nodes.wradar_zoom_slide.value = wrzml;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "WRADAR_ZOOM_SLIDE", 0, wrzml);
	float wrblp = (stf(InterfaceStates.WorldRadarBlip) - 0.2) / 4.8;
	GameInterface.nodes.wradar_blip_slide.value = wrblp;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "WRADAR_BLIP_SLIDE", 0, wrblp);
    newBase = CalcHUDBase(sl, stf(Render.screen_y));
	OptionsTab = sti(InterfaceStates.menu);
	SetOptionsWindow(OptionsTab);
	ChangeVideoColor();
	ChangeSeaDetail();
	ChangePerspDetail();
	ChangePersp2Detail();
	ChangeRadDetail();
	ChangeSoundSetting();
	ChangeMouseSensitivity();
	ChangeHUDDetail();
	ChangeGrassDistanceLand();
	ChangeGrassDistanceSea();
	ChangeGrassCountLand();
	ChangeGrassCountSea();
	ChangeCharCap();
	ChangeShipCap();
	ChangeParticles();
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	if(nodName=="SEA_HUD_WINDOW_CLOSE")
	{
		if(comName=="click" || comName=="activate") ShowNewSeaHUDWindow(false);
	}
	// LEO: Парсер Storm не принимает вложенное ИЛИ внутри условия с И.
	if(nodName=="LAND_HUD_WINDOW_CLOSE")
	{
		if(comName=="click" || comName=="activate") ShowNewLandHUDWindow(false);
	}
	if(nodName == "NEWHUD_WINDOW_CLOSE")
	{
		if(comName=="click" || comName=="activate") ShowNewHUDWindow(false);
	}
	if (nodName == "CHARCAP_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowCharCapWindow(false);
		}
	}
	if (nodName == "SHIPCAP_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowShipCapWindow(false);
		}
	}
	if (nodName == "CONSUMABLES_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowConsumablesWindow(false);
		}
	}
	if (nodName == "DIALOG_DELAY_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowDialogDelayWindow(false);
		}
	}
	if (nodName == "TIME_ACCEL_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowTimeAccelWindow(false);
		}
	}
	if (nodName == "AUTOSAVE_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowAutoSaveWindow(false);
		}
	}
	if (nodName == "PANIM_CLOSE")
	{
		if(comName=="click")
		{
			ShowProgressAnimWindow(false);
		}
	}
	if (nodName == "RADAR_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowRadarWindow(false);
		}
	}
	if (nodName == "WRADAR_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowWorldRadarWindow(false);
		}
	}
	if (nodName == "BOARDING_SIGNALS_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowBoardingSignalsWindow(false);
		}
	}
	if (nodName == "FORMATION_WINDOW_CLOSE")
	{
		if(comName=="click")
		{
			ShowFormationWindow(false);
		}
	}
}

void ProcessCancelExit()
{
	// LEO: Esc сначала закрывает вложенное окно, затем весь экран настроек.
	if (CheckAttribute(&InterfaceStates, "OptSubWin"))
	{
		string sw = InterfaceStates.OptSubWin;
		if (sw == "CHARCAP") ShowCharCapWindow(false);
		if (sw == "SHIPCAP") ShowShipCapWindow(false);
		if (sw == "CONSUMABLES") ShowConsumablesWindow(false);
		if (sw == "DIALOGDELAY") ShowDialogDelayWindow(false);
		if (sw == "BOARDINGSIGNALS") ShowBoardingSignalsWindow(false);
		if (sw == "RADAR") ShowRadarWindow(false);
		if (sw == "WORLDRADAR") ShowWorldRadarWindow(false);
		if (sw == "TIMEACCEL") ShowTimeAccelWindow(false);
		if (sw == "AUTOSAVE") ShowAutoSaveWindow(false);
		if (sw == "FORMATION") ShowFormationWindow(false);
		if (sw == "PANIM") ShowProgressAnimWindow(false);
		if (sw == "NEWHUD") ShowNewHUDWindow(false);
		if (sw == "RAGDOLL") ShowRagdollWindow(false);
		if (sw == "SEA_HUD") {ShowNewSeaHUDWindow(false);return;}
		if (sw == "LAND_HUD") {ShowNewLandHUDWindow(false);return;}
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		return;
	}
	int oldHUDStyle=sti(InterfaceStates.InterfaceSelection);
	int oldHUDIcons=sti(InterfaceStates.AltIntIcons);
	int oldHUDBars=sti(InterfaceStates.ShipBarsHorizontal);
	int oldShipBars=0;
	int oldTheme=0;
	string oldProgress="";
	if(CheckAttribute(&InterfaceStates,"OldShipBars")) oldShipBars=sti(InterfaceStates.OldShipBars);
	if(CheckAttribute(&InterfaceStates,"BlueInterfaceTheme")) oldTheme=sti(InterfaceStates.BlueInterfaceTheme);
	if(CheckAttribute(&InterfaceStates,"ProgressAnimFile")) oldProgress=InterfaceStates.ProgressAnimFile;
	// LEO: Сбрасываем черновик до загрузки options: первый запуск может записать defaults.
	Ragdoll_InitOptions();
	LoadGameOptions();
	// LEO: Отмена возвращает не только options, но и уже применённые live-настройки.
	int restoredShipBars=0;
	int restoredTheme=0;
	string restoredProgress="";
	if(CheckAttribute(&InterfaceStates,"OldShipBars")) restoredShipBars=sti(InterfaceStates.OldShipBars);
	if(CheckAttribute(&InterfaceStates,"BlueInterfaceTheme")) restoredTheme=sti(InterfaceStates.BlueInterfaceTheme);
	if(CheckAttribute(&InterfaceStates,"ProgressAnimFile")) restoredProgress=InterfaceStates.ProgressAnimFile;
	if(bSeaActive)
	{
		if(oldShipBars!=restoredShipBars) ReloadShipBars();
	}
	if(oldTheme!=restoredTheme) SetInterfaceTheme(restoredTheme);
	if(oldProgress!=restoredProgress)
	{
		if(restoredProgress=="") restoredProgress=PanimFile(0);
		SetProgressAnimation(restoredProgress);
	}
	if(oldHUDStyle != sti(InterfaceStates.InterfaceSelection) || oldHUDIcons != sti(InterfaceStates.AltIntIcons) || oldHUDBars != sti(InterfaceStates.ShipBarsHorizontal) || NewHUD_MasterLayoutChanged()) NewHUD_RebuildLayout();
	else NewHUD_ApplyLive();
	BLI_RefreshLandHUDSettings();
	ProcessExit();
}

void ProcessOkExit()
{
	int previousFlagPhysics = GetFlagPhysicsOption();
	// LEO: Ошибка записи флагов оставляет окно открытым до любых других изменений.
	if (!SaveFlagPhysicsOption())
	{
		g_bToolTipStarted = true;
		CreateTooltip("#"+XI_ConvertString("FlagPhysicsOpt"),XI_ConvertString("FlagPhysicsSaveError"),argb(255,255,255,255),"",0,"",0,"",0,"none","","",256,256);
		return;
	}
	if (!Ragdoll_SaveOptions())
	{
		// LEO: Откатываем запись, сохраняя черновик для повторного ОК.
		if (iFlagPhysicsDraft != previousFlagPhysics)
		{
			if (SetFlagPhysicsOption(previousFlagPhysics) == 0)
			{
				string flagRollbackWarning = "Could not save ragdoll or restore the flag setting. Flag physics may already have changed. Check write access and retry OK.";
				if (LanguageGetLanguage() == "russian") flagRollbackWarning = "Не удалось сохранить ragdoll и восстановить настройку флагов. Физика флагов могла уже измениться. Проверьте доступ на запись и повторите ОК.";
				SetFormatedText("RAGDOLL_NOTE", flagRollbackWarning);
			}
		}
		return;
	}
	bool bHudScaleChanged = (roundFloat(screenscaling) != roundFloat(newBase));
	screenscaling = newBase;
	// LEO: Масштаб читается при создании HUD; для применения требуется пересоздание.
	if (bHudScaleChanged)
	{
		NewHUD_RebuildLayout();
	}
	else NewHUD_SyncMaster();

	if (sti(InterfaceStates.CharCap)==0)
	{
		NPCShowName(false, false, false);
	}
	else NPCShowName(sti(InterfaceStates.CharCapShowOff), sti(InterfaceStates.CharCapShowOthers), sti(InterfaceStates.CharCapTargetOfficers));
	
	if (sti(InterfaceStates.ShipCap)==0)
	{
		InterfaceStates.ShipCapShipNames1 = false;
		InterfaceStates.ShipCapShipNames2 = false;
		InterfaceStates.ShipCapShowActions = false;
		InterfaceStates.ShipCapShowActionsPlus = false;
		InterfaceStates.ShipCapShowAllies = false;
		InterfaceStates.ShipCapShowOther = false;
		InterfaceStates.ShipCapShowNotifications = false;
		InterfaceStates.ShipCapShowCrit = false;
		InterfaceStates.ShipCapShowFortGun = false;
		InterfaceStates.ShipCapShowDamage = false;
		InterfaceStates.ShipCapShowDamageSelf = false;
		InterfaceStates.EnabledShipMarks = false;
		InterfaceStates.DisableShipMarksSelf = false;
		InterfaceStates.DisableShipMarksCompanions = false;
	}
	ShipShowName();
	
	if (sti(InterfaceStates.InterfaceSelection) == POTC) InterfaceStates.InterfaceShipString = "interfaces\ships_PotC\";
	else InterfaceStates.InterfaceShipString = "interfaces\ships\";

	SaveGameOptions();

	// LEO: Видео сохраняем до перезапуска: живой device Reset ломает текстуры моделей и HUD.
	// LEO: Сравниваем с отложенными Render.video*; letterbox, кэши и FPS применяются сразу.
	if( CheckAttribute(&InterfaceStates,"video.width") )
	{
		int vbNewW  = sti(InterfaceStates.video.width);
		int vbNewH  = sti(InterfaceStates.video.height);
		int vbFull  = sti(Render.videoFull); // Gregg: полный экран убран из меню — держим текущее (ApplyVideoMode его не сбрасывает)
		int vbMsaa  = sti(InterfaceStates.video.msaa);
		int vbVsync = sti(InterfaceStates.video.vsync);
		bool vbChanged = false;
		if( vbNewW  != sti(Render.videoW) )     vbChanged = true;
		if( vbNewH  != sti(Render.videoH) )     vbChanged = true;
		if( vbMsaa  != sti(Render.videoMsaa) )  vbChanged = true;
		if( vbVsync != sti(Render.videoVsync) ) vbChanged = true;
		if( vbChanged )
		{
			ApplyVideoMode(vbNewW, vbNewH, vbFull, vbMsaa, vbVsync); // persist-only -> engine.ini; применится рестартом
		}

		// Gregg: letterbox — ЖИВОЙ (без Reset): пересчёт движковой канвы + перекладка HUD.
		int vbLetterbox = sti(InterfaceStates.video.letterbox);
		if( vbLetterbox != sti(Render.letterbox) )
		{
			SetLetterbox(vbLetterbox); // запись engine.ini [Interface] Letterbox
			SendMessage(&GameInterface, "l", MSG_INTERFACE_RELOAD_CANVAS);
			NewHUD_RebuildLayout();
		}

		// Gregg: показ FPS / кап FPS — наживую, без Reset устройства
		int vbShowFps = sti(InterfaceStates.video.showFps);
		int vbMaxFps  = sti(InterfaceStates.video.maxFps);
		bool vbFpsChanged = false;
		if( vbShowFps != sti(Render.show_fps) ) vbFpsChanged = true;
		if( vbMaxFps  != sti(Render.max_fps) )  vbFpsChanged = true;
		if( vbFpsChanged )
		{
			SetFpsOptions(vbShowFps, vbMaxFps);
		}

		NpcHeadLookSet(sti(InterfaceStates.NpcHeadLook)); // LEO: взгляды NPC - наживую

		// Gregg: кэш текстур (RetainMB) — наживую, без Reset устройства; при понижении движок сам вытесняет лишнее
		int vbRetain = sti(InterfaceStates.video.retainMB);
		if( vbRetain != sti(Render.retainMB) ) SetRetainMB(vbRetain);

		// Gregg: кэш геометрии (GeoRetainMB) — наживую; при понижении движок вытесняет лишнее
		int vbGeoRetain = sti(InterfaceStates.video.geoRetainMB);
		if( vbGeoRetain != sti(Render.geoRetainMB) ) SetGeometryRetainMB(vbGeoRetain);

		// Gregg: прогрев ОЗУ + прогрев текстур — РЕСТАРТ-ОНЛИ (прогреватель одноразовый на старте); только персист engine.ini
		int vbWarm = sti(InterfaceStates.video.warmAssets);
		if( vbWarm != sti(Render.warmAssets) ) SetWarmAssets(vbWarm);
		int vbWarmMB = sti(InterfaceStates.video.warmTexMB);
		if( vbWarmMB != sti(Render.warmTexturesMB) ) SetWarmTexturesMB(vbWarmMB);

		// Gregg: кэш неба (PinTextures) + кэш скриптов (cache_mode) — РЕСТАРТ-ОНЛИ; только персист engine.ini
		int vbSky = sti(InterfaceStates.video.pinSky);
		if( vbSky != sti(Render.pinTextures) ) SetPinTextures(vbSky);
		int vbSC = sti(InterfaceStates.video.scriptCache);
		if( vbSC != sti(Render.scriptCacheMode) ) SetScriptCache(vbSC);
		// Gregg: тени/флары — рестарт-онли; персист engine.ini через нативки
		int vbShadow = sti(InterfaceStates.video.shadowMode);
		if( vbShadow != sti(Render.shadowMode) ) SetShadowMode(vbShadow);
		int vbFlare = sti(InterfaceStates.video.flareMode);
		if( vbFlare != sti(Render.flareMode) ) SetFlareMode(vbFlare);
		SaveCannonHeatHazeOption();
	}

	ProcessExit();
	Event("eventChangeOption");

	SetSeaGridStep(stf(InterfaceStates.SeaDetails));// change sea settings
	SetPerspectiveSettings();
	SetPerspective2Settings();
	SetExpLogsBack();
}

void ProcessExit()
{

	if(sti(InterfaceStates.EnabledCMControls) == 1)
	{
		CI_CreateAndSetControls( "Sailing3Pers", "BICommandsActivate", CI_GetKeyCode("VK_MBUTTON"), 0, true );
		CI_CreateAndSetControls( "PrimaryLand", "LICommandsActivate", CI_GetKeyCode("VK_MBUTTON"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsConfirm", CI_GetKeyCode("VK_LBUTTON"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsCancel", CI_GetKeyCode("VK_RBUTTON"), 0, false );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsLeft", CI_GetKeyCode("VK_MWHEEL_DOWN"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsRight", CI_GetKeyCode("VK_MWHEEL_UP"), 0, true );
	}
	else
	{
		CI_CreateAndSetControls( "Sailing3Pers", "BICommandsActivate", CI_GetKeyCode("VK_RETURN"), 0, true );
		CI_CreateAndSetControls( "PrimaryLand", "LICommandsActivate", CI_GetKeyCode("VK_RETURN"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsConfirm", CI_GetKeyCode("VK_RETURN"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsCancel", CI_GetKeyCode("VK_ESCAPE"), 0, false );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsLeft", CI_GetKeyCode("VK_LEFT"), 0, true );
		CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsRight", CI_GetKeyCode("VK_RIGHT"), 0, true );
	}

	bool fromGame=false;
	if(CheckAttribute(&InterfaceStates,"showGameMenuOnExit")) fromGame=sti(InterfaceStates.showGameMenuOnExit)!=0;
	if(fromGame) {
		IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU);
		return;
	}

	IDoExit(RC_INTERFACE_OPTIONSCREEN_EXIT);
	bool instant=false;
	if(CheckAttribute(&InterfaceStates,"InstantExit")) instant=sti(InterfaceStates.InstantExit)!=0;
	if(!instant) {
		ReturnToMainMenu();
	}
}

void IDoExit(int exitCode)
{
	DelEventHandler("evntKeyChoose","procKeyChoose");
	DelEventHandler("eSlideChange","procSlideChange");
	DelEventHandler("CheckButtonChange","procCheckBoxChange");

	DelEventHandler("eventKeyChange","procKeyChange");
	DelEventHandler("eventBtnAction","procBtnAction");
	DelEventHandler("eTabControlPress","procTabChange");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ShowInfo", "ShowInfo");
	DelEventHandler("MouseRClickUP","HideInfo");
	DelEventHandler("evFaderFrame","FaderFrame");
	DelEventHandler("InterfaceBreak","ProcessCancelExit");

	DelEventHandler("checkSoundParam","ClickSoundOptions");
	DelEventHandler("checkGameParam","ClickGameOptions");
	DelEventHandler("setupGameParam","SetupGameOptions");
	DelEventHandler("ePanimSelect","ProcPanimSelect");

	DelEventHandler("infoSoundParam","RightClickSoundOptions");
	DelEventHandler("infoGameParam","RightClickGameOptions");
	DelEventHandler("checkMasterParam","ClickMasterOptions");
	DelEventHandler("setupMasterParam","SetupMasterOptions");
	DelEventHandler("infoMasterParam","RightClickMasterOptions");

	DelEventHandler("ievnt_command","ProcessCommandExecute");
	LanguageCloseFile( g_ControlsLngFile );

	interfaceResultCommand = exitCode;
	bool instant=false;
	if(CheckAttribute(&InterfaceStates,"InstantExit")) instant=sti(InterfaceStates.InstantExit)!=0;
	EndCancelInterface(instant);
	ControlsMakeInvert();
	if(bSeaActive) {
        BattleInterface.ShifInfoVisible = InterfaceStates.enabledshipmarks;
        BattleInterface.battleborder.used = InterfaceStates.ShowBattleMode;
	}
}

void SetControlsTabMode(int nMode)
{
	int nColor1 = argb(255,196,196,196);
	int nColor2 = nColor1;
	int nColor3 = nColor1;
	int nColor4 = nColor1;

	string sPic1 = "TabDeSelected";
	string sPic2 = sPic1;
	string sPic3 = sPic1;
	string sPic4 = sPic1;

	switch( nMode )
	{
	case 1: // море от первого лица
		sPic1 = "TabSelected";
		nColor1 = argb(255,255,255,255);
	break;
	case 2: // режим путешествий на земле
		sPic2 = "TabSelected";
		nColor2 = argb(255,255,255,255);
	break;
	case 3: // море от 3-го лица
		sPic3 = "TabSelected";
		nColor3 = argb(255,255,255,255);
	break;
	case 4: // режим боя на земле
		sPic4 = "TabSelected";
		nColor4 = argb(255,255,255,255);
	break;
	}

	SetNewGroupPicture("TABBTN_SAILING_1ST", "TABS", sPic1);
	SetNewGroupPicture("TABBTN_PRIMARY_LAND", "TABS", sPic2);
	SetNewGroupPicture("TABBTN_SAILING_3RD", "TABS", sPic3);
	SetNewGroupPicture("TABBTN_FIGHT_MODE", "TABS", sPic4);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SAILING_1ST", 8,0,nColor1);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_PRIMARY_LAND", 8,0,nColor2);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SAILING_3RD", 8,0,nColor3);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_FIGHT_MODE", 8,0,nColor4);

	FillControlsList(nMode);
}

void procTabChange()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( sNodName == "TABBTN_SAILING_1ST" ) {
		SetControlsTabMode( 1 );
		return;
	}
	if( sNodName == "TABBTN_PRIMARY_LAND" ) {
		SetControlsTabMode( 2 );
		return;
	}
	if( sNodName == "TABBTN_SAILING_3RD" ) {
		SetControlsTabMode( 3 );
		return;
	}
	if( sNodName == "TABBTN_FIGHT_MODE" ) {
		SetControlsTabMode( 4 );
		return;
	}
	
	if( sNodName == "TABBTN_GRAPHICS" ) {
		SetOptionsWindow( 1 );
		return;
	}
	if( sNodName == "TABBTN_SOUND" ) {
		SetOptionsWindow( 2 );
		return;
	}
	if( sNodName == "TABBTN_CONTROLS" ) {
		SetOptionsWindow( 3 );
		return;
	}
	if( sNodName == "TABBTN_OPTIONS" ) {
		SetOptionsWindow( 4 );
		return;
	}
}

void SetOptionsWindow(int num)
{
	int nColor1 = argb(255,196,196,196);
	int nColor2 = nColor1;
	int nColor3 = nColor1;
	int nColor4 = nColor1;

	string sPic1 = "TabDeSelected";
	string sPic2 = sPic1;
	string sPic3 = sPic1;
	string sPic4 = sPic1;

	switch( num )
	{
	case 1: // море от первого лица
		sPic1 = "TabSelected";
		nColor1 = argb(255,255,255,255);
	break;
	case 2: // режим путешествий на земле
		sPic2 = "TabSelected";
		nColor2 = argb(255,255,255,255);
	break;
	case 3: // море от 3-го лица
		sPic3 = "TabSelected";
		nColor3 = argb(255,255,255,255);
	break;
	case 4: // режим боя на земле
		sPic4 = "TabSelected";
		nColor4 = argb(255,255,255,255);
	break;
	}

	SetNewGroupPicture("TABBTN_GRAPHICS", "TABS", sPic1);
	SetNewGroupPicture("TABBTN_SOUND", "TABS", sPic2);
	SetNewGroupPicture("TABBTN_CONTROLS", "TABS", sPic3);
	SetNewGroupPicture("TABBTN_OPTIONS", "TABS", sPic4);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_GRAPHICS", 8,0,nColor1);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SOUND", 8,0,nColor2);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CONTROLS", 8,0,nColor3);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_OPTIONS", 8,0,nColor4);
	
	XI_WindowDisable("GRAPHICS_WINDOW",true);
	XI_WindowDisable("SOUNDS_WINDOW",true);
	XI_WindowDisable("CONTROLS_WINDOW",true);
	XI_WindowDisable("OTHERS_WINDOW",true);
	XI_WindowShow("GRAPHICS_WINDOW",false);
	XI_WindowShow("SOUNDS_WINDOW",false);
	XI_WindowShow("CONTROLS_WINDOW",false);
	XI_WindowShow("OTHERS_WINDOW",false);
	switch (num)
	{
		case 1:
			XI_WindowShow("GRAPHICS_WINDOW",true);
			XI_WindowDisable("GRAPHICS_WINDOW",false);
		break;
		case 2:
			XI_WindowShow("SOUNDS_WINDOW",true);
			XI_WindowDisable("SOUNDS_WINDOW",false);
		break;
		case 3:
			XI_WindowShow("CONTROLS_WINDOW",true);
			XI_WindowDisable("CONTROLS_WINDOW",false);
		break;
		case 4:
			XI_WindowShow("OTHERS_WINDOW",true);
			XI_WindowDisable("OTHERS_WINDOW",false);
		break;
	}
	InterfaceStates.menu = num;
}

void procBtnAction()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();
	if(NewSeaHUD_Button(sNodName)) return;
	if(NewLandHUD_Button(sNodName)) return;
	if(Ragdoll_Button(sNodName)) return;

	if( sNodName == "BTN_OK" ) {
		if( iComIndex==ACTION_ACTIVATE || iComIndex==ACTION_MOUSECLICK ) {
			ProcessOkExit();
		}
		return;
	}
	if( sNodName == "BTN_CONTROLS_DEFAULT" ) {
		RestoreDefaultKeys(0);
		return;
	}

	if( sNodName == "BTN_CONTROLS_DEFAULT_GPK" ) {
		RestoreDefaultKeys(1);
		return;
	}
	
	if( sNodName == "CONSUMABLES_POS_DEFAULT" ) {
		GameInterface.nodes.consumables_pos_slide.value = 0.5;
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CONSUMABLES_POS_SLIDE", 0, stf(GameInterface.nodes.consumables_pos_slide.value));
		ChangeConsumablesHUDPos();
		return;
	}

	if( sNodName == "GFXHDR_VEG_DEFAULT" ) {
		RestoreDefaultVegetation();
		return;
	}

	if( sNodName == "HUD_SCALE_DEFAULT" ) {
		GameInterface.nodes.hud_slide.value = CalcHUDSlider(roundFloat(stf(Render.screen_y) / 900.0));
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HUD_SLIDE", 0, stf(GameInterface.nodes.hud_slide.value));
		ChangeHUDDetail();
		return;
	}
	
	if( sNodName == "HUD_DIALOG_LEFT_BUTTON" ) {
		ChangeDialogStyle(false);
		return;
	}
	
	if( sNodName == "HUD_DIALOG_RIGHT_BUTTON" ) {
		ChangeDialogStyle(true);
		return;
	}
	if( sNodName == "RADAR_QUALITY_LEFT" ) {
		ChangeRadarQuality(false);
		return;
	}
	if( sNodName == "RADAR_QUALITY_RIGHT" ) {
		ChangeRadarQuality(true);
		return;
	}
	if( sNodName == "WRADAR_QUALITY_LEFT" ) {
		ChangeWorldRadarQuality(false);
		return;
	}
	if( sNodName == "WRADAR_QUALITY_RIGHT" ) {
		ChangeWorldRadarQuality(true);
		return;
	}
	if( sNodName == "RESOLUTION_LEFT" ) {
		ChangeResolution(false);
		return;
	}
	if( sNodName == "RESOLUTION_RIGHT" ) {
		ChangeResolution(true);
		return;
	}
	if( sNodName == "SCRIPTCACHE_LEFT" ) { ChangeScriptCache(false); return; }
	if( sNodName == "SCRIPTCACHE_RIGHT" ){ ChangeScriptCache(true);  return; }
	if( sNodName == "SHADOW_LEFT" )      { ChangeShadow(false);      return; }
	if( sNodName == "SHADOW_RIGHT" )     { ChangeShadow(true);       return; }
	if( sNodName == "FLARE_LEFT" )       { ChangeFlare(false);       return; }
	if( sNodName == "FLARE_RIGHT" )      { ChangeFlare(true);        return; }
	if( sNodName == "CANNONHEAT_LEFT" )  { ChangeCannonHeatHaze(); return; }
	if( sNodName == "CANNONHEAT_RIGHT" ) { ChangeCannonHeatHaze(); return; }
	if( sNodName == "FLAGPHYSICS_LEFT" )  { ChangeFlagPhysics(); return; }
	if( sNodName == "FLAGPHYSICS_RIGHT" ) { ChangeFlagPhysics(); return; }
	if( sNodName == "MSAA_LEFT" )        { ChangeMSAA(false);       return; }
	if( sNodName == "MSAA_RIGHT" )       { ChangeMSAA(true);        return; }
	if( sNodName == "VSYNC_LEFT" )       { ChangeVSync(false);      return; }
	if( sNodName == "VSYNC_RIGHT" )      { ChangeVSync(true);       return; }
	if( sNodName == "SHOWFPS_LEFT" )     { ChangeShowFps(false);    return; }
	if( sNodName == "SHOWFPS_RIGHT" )    { ChangeShowFps(true);     return; }
	if( sNodName == "MAXFPS_LEFT" )      { ChangeMaxFps(false);     return; }
	if( sNodName == "MAXFPS_RIGHT" )     { ChangeMaxFps(true);      return; }
	if( sNodName == "LETTERBOX_LEFT" )   { ChangeLetterbox(false);  return; }
	if( sNodName == "LETTERBOX_RIGHT" )  { ChangeLetterbox(true);   return; }
	if( sNodName == "RETAINMB_LEFT" )    { ChangeRetainMB(false);   return; }
	if( sNodName == "SHIPINFO_BARS_LEFT" ) { ShipInfoBars_ChangeMode(false); return; }
	if( sNodName == "SHIPINFO_BARS_RIGHT" ) { ShipInfoBars_ChangeMode(true); return; }
	if( sNodName == "RETAINMB_RIGHT" )   { ChangeRetainMB(true);    return; }
	if( sNodName == "GEORETAINMB_LEFT" ) { ChangeGeoRetainMB(false); return; }
	if( sNodName == "GEORETAINMB_RIGHT" ){ ChangeGeoRetainMB(true);  return; }
	if( sNodName == "WARMASSETS_LEFT" )  { ChangeWarmAssets(false); return; }
	if( sNodName == "WARMASSETS_RIGHT" ) { ChangeWarmAssets(true);  return; }
	if( sNodName == "WARMTEXMB_LEFT" )   { ChangeWarmTexMB(false);  return; }
	if( sNodName == "WARMTEXMB_RIGHT" )  { ChangeWarmTexMB(true);   return; }
	if( sNodName == "PINSKY_LEFT" )      { ChangePinSky(false); return; }
	if( sNodName == "PINSKY_RIGHT" )     { ChangePinSky(true);  return; }

	if (sNodName == "CHARCAP_FONT_SCALE_DEFAULT") {
		ResetCharCapSliders(0);
		return;
	}
	if (sNodName == "CHARCAP_NOTIFICATIONS_SCALE_DEFAULT") {
		ResetCharCapSliders(1);
		return;
	}
	if (sNodName == "CHARCAP_ENEMYBAR_SIZE_DEFAULT") {
		ResetCharCapSliders(2);
		return;
	}
	if (sNodName == "CHARCAP_ENEMYBAR_DIST_DEFAULT") {
		ResetCharCapSliders(3);
		return;
	}
	if (sNodName == "CHARCAP_ENEMYBAR_MAX_DEFAULT") {
		ResetCharCapSliders(4);
		return;
	}
	if (sNodName == "SHIPCAP_NOTIFICATION_DISTANCE_DEFAULT") {
		ResetShipCapSliders(0);
		return;
	}
	
	if (sNodName == "SHIPCAP_NOTIFICATION_SCALE_DEFAULT") {
		ResetShipCapSliders(1);
		return;
	}
	
	if (sNodName == "SHIPCAP_NAMES_SCALE_DEFAULT") {
		ResetShipCapSliders(2);
		return;
	}
	
	if (sNodName == "SHIPCAP_ICONS_SCALE_DEFAULT") {
		ResetShipCapSliders(3);
		return;
	}
	
	if (sNodName == "PARTICLES_COUNT_DEFAULT") {
		ResetParticles();
		return;
	}
	if (sNodName == "TIME_ACCEL_WINDOW_RESET") {
		ResetTimeAccel();
		return;
	}
}

void ResetParticles()
{
	InterfaceStates.ParticlesCount = 0.0;
	GameInterface.nodes.PARTICLES_COUNT_SLIDE.value = InterfaceStates.ParticlesCount;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "PARTICLES_COUNT_SLIDE", 0, stf(GameInterface.nodes.PARTICLES_COUNT_SLIDE.value));
}

void ResetShipCapSliders(int num)
{
	switch (num)
	{
		case 0:
			InterfaceStates.ShipCapNotifDistance = 0.112;
			GameInterface.nodes.SHIPCAP_NOTIFICATION_DISTANCE_SLIDE.value = InterfaceStates.ShipCapNotifDistance;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "SHIPCAP_NOTIFICATION_DISTANCE_SLIDE", 0, stf(GameInterface.nodes.SHIPCAP_NOTIFICATION_DISTANCE_SLIDE.value));
		break;
		case 1:
			InterfaceStates.ShipCapNotifScale = 0.3;
			GameInterface.nodes.SHIPCAP_NOTIFICATION_SCALE_SLIDE.value = InterfaceStates.ShipCapNotifScale;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "SHIPCAP_NOTIFICATION_SCALE_SLIDE", 0, stf(GameInterface.nodes.SHIPCAP_NOTIFICATION_SCALE_SLIDE.value));
		break;
		case 2:
			InterfaceStates.ShipCapNamesScale = 0.3;
			GameInterface.nodes.SHIPCAP_NAMES_SCALE_SLIDE.value = InterfaceStates.ShipCapNamesScale;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "SHIPCAP_NAMES_SCALE_SLIDE", 0, stf(GameInterface.nodes.SHIPCAP_NAMES_SCALE_SLIDE.value));
		break;
		case 3:
			InterfaceStates.ShipCapIconsScale = 0.3;
			GameInterface.nodes.SHIPCAP_ICONS_SCALE_SLIDE.value = InterfaceStates.ShipCapIconsScale;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "SHIPCAP_ICONS_SCALE_SLIDE", 0, stf(GameInterface.nodes.SHIPCAP_ICONS_SCALE_SLIDE.value));
		break;
	}
	ChangeShipCap();
}

void ResetCharCapSliders(int num)
{
	switch (num)
	{
		case 0:
			InterfaceStates.CharCapFontScale = 0.3;
			GameInterface.nodes.CHARCAP_FONT_SCALE_SLIDE.value = InterfaceStates.CharCapFontScale;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_FONT_SCALE_SLIDE", 0, stf(GameInterface.nodes.CHARCAP_FONT_SCALE_SLIDE.value));
		break;
		case 1:
			InterfaceStates.CharCapNotifScale = 0.3;
			GameInterface.nodes.CHARCAP_NOTIFICATIONS_SCALE_SLIDE.value = InterfaceStates.CharCapNotifScale;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_NOTIFICATIONS_SCALE_SLIDE", 0, stf(GameInterface.nodes.CHARCAP_NOTIFICATIONS_SCALE_SLIDE.value));
		break;
		case 2: // LEO: размер баров врагов — дефолт 4.0 (raw 0.4286)
			InterfaceStates.EnemyBarSize = 0.4286;
			GameInterface.nodes.CHARCAP_ENEMYBAR_SIZE_SLIDE.value = InterfaceStates.EnemyBarSize;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_ENEMYBAR_SIZE_SLIDE", 0, stf(GameInterface.nodes.CHARCAP_ENEMYBAR_SIZE_SLIDE.value));
		break;
		case 3: // LEO: дальность баров врагов — дефолт 20 (raw 0.4286)
			InterfaceStates.EnemyBarDist = 0.4286;
			GameInterface.nodes.CHARCAP_ENEMYBAR_DIST_SLIDE.value = InterfaceStates.EnemyBarDist;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_ENEMYBAR_DIST_SLIDE", 0, stf(GameInterface.nodes.CHARCAP_ENEMYBAR_DIST_SLIDE.value));
		break;
		case 4: // LEO: кол-во баров врагов — дефолт 32 (raw 0.2)
			InterfaceStates.EnemyBarMax = 0.2;
			GameInterface.nodes.CHARCAP_ENEMYBAR_MAX_SLIDE.value = InterfaceStates.EnemyBarMax;
			SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_ENEMYBAR_MAX_SLIDE", 0, stf(GameInterface.nodes.CHARCAP_ENEMYBAR_MAX_SLIDE.value));
		break;
	}
	ChangeCharCap();
}

void IReadVariableBeforeInit()
{
	GetSoundOptionsData();
	GetMouseOptionsData();
	GetVideoOptionsData();
}

void IReadVariableAfterInit()
{
	GetHerbOptionsData();
	GetControlsStatesData();

	GetFlagAllOptionsData();
	GetStreamerModeData();
	GetVisualCirassOptionsData();
	GetInterfaceSelectionData();
	GetAltFontOptionsData();
	GetBombsParticlesOptionsData();
	GetFireParticlesOptionsData();
	GetOtherParticlesOptionsData();
	GetIslVegOptionsData(); // Gregg: растительность на островах
	GetVideoModeData(); // Show the selected resolution.

	if( CheckAttribute(&InterfaceStates,"DialogStyle") ) SetFormatedText("HUD_DIALOG_TEXT", InterfaceStates.DialogStyle);
	else 
	{
		InterfaceStates.DialogStyle = 1;
		SetFormatedText("HUD_DIALOG_TEXT", InterfaceStates.DialogStyle);
	}

	if( !CheckAttribute(&InterfaceStates,"RegionsWorldmap") ) 		InterfaceStates.RegionsWorldmap = 0;
	if( !CheckAttribute(&InterfaceStates,"SimpleSea") ) 			InterfaceStates.SimpleSea = 0;
	if( !CheckAttribute(&InterfaceStates,"EnabledShipMarks") ) 		InterfaceStates.EnabledShipMarks = 1;//default
	if( !CheckAttribute(&InterfaceStates,"OldShipBars") ) 			InterfaceStates.OldShipBars = 0;//LEO: 0=новые CSP-бары, 1=старые indicators_HD
	if( !CheckAttribute(&InterfaceStates,"EnabledAutoSaveMode") ) 	InterfaceStates.EnabledAutoSaveMode = 1;//default
	if( !CheckAttribute(&InterfaceStates,"ShowBattleMode") ) 		InterfaceStates.ShowBattleMode = 0;
	if( !CheckAttribute(&InterfaceStates,"SpyglassTextures") ) 		InterfaceStates.SpyglassTextures = 0;
	if( !CheckAttribute(&InterfaceStates,"HUDStyle") )				InterfaceStates.HUDStyle = 0;
	if( !CheckAttribute(&InterfaceStates,"ShowBoardMode") ) 		InterfaceStates.ShowBoardMode = 0;
	if( !CheckAttribute(&InterfaceStates,"EnableSailors") )			InterfaceStates.EnableSailors = 0;
	if( !CheckAttribute(&InterfaceStates,"CharVoice") ) 			InterfaceStates.CharVoice = 0;
	if( !CheckAttribute(&InterfaceStates,"IslandLoader") ) 			InterfaceStates.IslandLoader = 0;
	if( !CheckAttribute(&InterfaceStates,"DirectSail") ) 			InterfaceStates.DirectSail = 0; // Gregg: опция доплытия между островами
	if( !CheckAttribute(&InterfaceStates,"AltIntIcons") ) 			InterfaceStates.AltIntIcons = 0;
	if( !CheckAttribute(&InterfaceStates,"BlueInterfaceTheme") )	InterfaceStates.BlueInterfaceTheme = 0; // LEO: синяя тема интерфейсов
	
	if( !CheckAttribute(&InterfaceStates,"BoardingSignals") ) 		InterfaceStates.BoardingSignals = 1; // LEO: мастер сигналов абордажников
	if( !CheckAttribute(&InterfaceStates,"PortraitShakeEnable") ) 	InterfaceStates.PortraitShakeEnable = 1; // LEO: тряска портрета
	if( !CheckAttribute(&InterfaceStates,"PortraitPingEnable") ) 	InterfaceStates.PortraitPingEnable = 1;  // LEO: пинг портрета
	if( !CheckAttribute(&InterfaceStates,"CharCap") ) 				InterfaceStates.CharCap = 1;
	if( !CheckAttribute(&InterfaceStates,"CharCapShowOthers") ) 	InterfaceStates.CharCapShowOthers = 1;
	if( !CheckAttribute(&InterfaceStates,"CharCapShowOff") ) 		InterfaceStates.CharCapShowOff = 0;
	if( !CheckAttribute(&InterfaceStates,"CharCapShowNotifications") ) 	InterfaceStates.CharCapShowNotifications = 0;
	if( !CheckAttribute(&InterfaceStates,"CharCapShowRank") ) 	InterfaceStates.CharCapShowRank = 0;
	if( !CheckAttribute(&InterfaceStates,"CharCapShowSpec") ) 	InterfaceStates.CharCapShowSpec = 0;
	if( !CheckAttribute(&InterfaceStates,"CharCapTargetOfficers") ) 	InterfaceStates.CharCapTargetOfficers = 0;
	if( !CheckAttribute(&InterfaceStates,"CharCapLandBars") ) 	InterfaceStates.CharCapLandBars = 1; // LEO: бары над персонажами на суше (деф вкл, миграция старых сейвов)
	GetCharCapOptionsData();
	
	if( !CheckAttribute(&InterfaceStates,"ShipCap") ) 				InterfaceStates.ShipCap = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShipNames1") ) 	InterfaceStates.ShipCapShipNames1 = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShipNames2") ) 		InterfaceStates.ShipCapShipNames2 = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowActions") ) 	InterfaceStates.ShipCapShowActions = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowActionsPlus") ) 	InterfaceStates.ShipCapShowActionsPlus = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowAllies") ) 	InterfaceStates.ShipCapShowAllies = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowOther") ) 	InterfaceStates.ShipCapShowOther = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowNotifications") ) 	InterfaceStates.ShipCapShowNotifications = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowCrit") ) 	InterfaceStates.ShipCapShowCrit = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowFortGun") ) 	InterfaceStates.ShipCapShowFortGun = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowDamage") ) 	InterfaceStates.ShipCapShowDamage = 1;
	if( !CheckAttribute(&InterfaceStates,"ShipCapShowDamageSelf") ) 	InterfaceStates.ShipCapShowDamageSelf = 1;
	if( !CheckAttribute(&InterfaceStates,"DisableShipMarksSelf") ) 	InterfaceStates.DisableShipMarksSelf = 0;
	if( !CheckAttribute(&InterfaceStates,"DisableShipMarksCompanions") ) 	InterfaceStates.DisableShipMarksCompanions = 0;
	if( !CheckAttribute(&InterfaceStates,"ConsumablesHUDContext") ) InterfaceStates.ConsumablesHUDContext = 0;
	GetShipCapOptionsData();
	
	if( !CheckAttribute(&InterfaceStates,"EnabledQuestsMarks") ) 	InterfaceStates.EnabledQuestsMarks = 1;//default
	if( !CheckAttribute(&InterfaceStates,"EnabledFXMarks") ) 		InterfaceStates.EnabledFXMarks = 1;//default
	if( !CheckAttribute(&InterfaceStates,"EnabledCMControls") ) 	InterfaceStates.EnabledCMControls = 0;
	if( !CheckAttribute(&InterfaceStates,"EnabledOldStore") ) 		InterfaceStates.EnabledOldStore = 0;
	if( !CheckAttribute(&InterfaceStates,"EnabledAltSoundsGun") ) 	InterfaceStates.EnabledAltSoundsGun = 0;
	if( !CheckAttribute(&InterfaceStates,"DiscordRPC") ) 	        InterfaceStates.DiscordRPC = 1;
	if( !CheckAttribute(&InterfaceStates,"StreamerMode") ) 	        InterfaceStates.StreamerMode = 0;
	if( !CheckAttribute(&InterfaceStates,"ShowExpLogs") ) 			InterfaceStates.ShowExpLogs = 0;
	if( !CheckAttribute(&InterfaceStates,"ShowGameLog") ) 			InterfaceStates.ShowGameLog = 1;
	if( !CheckAttribute(&InterfaceStates,"ConsumablesHUD") ) 				InterfaceStates.ConsumablesHUD = 0;
	if( !CheckAttribute(&InterfaceStates,"DialogDelay") ) 				InterfaceStates.DialogDelay = 1;
	if( !CheckAttribute(&InterfaceStates,"DialogInputDelay") ) 				InterfaceStates.DialogInputDelay = 0.5;
	if( !CheckAttribute(&InterfaceStates,"TimeAccelSettings") ) 	InterfaceStates.TimeAccelSettings = 0; // Борода: свой множитель ускорения времени - по умолчанию выкл (старое поведение x2)
	if( !CheckAttribute(&InterfaceStates,"ProgressAnimFile") )		InterfaceStates.ProgressAnimFile = "Loading\progress.tga"; // LEO: спрайт загрузочной анимации
	if( !CheckAttribute(&InterfaceStates,"ProgressAnimation") )		InterfaceStates.ProgressAnimation = 1; // LEO: строка-мастер (окно выбора по клику)
	if( !CheckAttribute(&InterfaceStates,"TimeAccelSeaBattle") )	InterfaceStates.TimeAccelSeaBattle = 2; // множитель на бою на море, int 2..9 (потолок скрипта x9)
	if( !CheckAttribute(&InterfaceStates,"TimeAccelSea") )			InterfaceStates.TimeAccelSea = 2;       // множитель в открытом море, int 2..13 (потолок скрипта x13)
	if( !CheckAttribute(&InterfaceStates,"TimeAccelLand") )		InterfaceStates.TimeAccelLand = 2;      // множитель на суше, int 2..9 (потолок скрипта x9)
	if( !CheckAttribute(&InterfaceStates,"ConsumablesNoTop") ) 					InterfaceStates.ConsumablesNoTop = 0;
	if( !CheckAttribute(&InterfaceStates,"ConsumablesNoBottom") ) 				InterfaceStates.ConsumablesNoBottom = 0;
	if( !CheckAttribute(&InterfaceStates,"ShipBarsHorizontal") ) 	InterfaceStates.ShipBarsHorizontal = 0;
	if( !CheckAttribute(&InterfaceStates,"SpyglassFromMars") ) 		InterfaceStates.SpyglassFromMars = 0; // LEO: труба по CTRL от 3-го лица — выкл=с палубы, вкл=с марса
	if( !CheckAttribute(&InterfaceStates,"RollingBroadside") ) 		InterfaceStates.RollingBroadside = 0; // LEO: залп бортом нос->корма (выкл=разнобой) — все корабли
	if( !CheckAttribute(&InterfaceStates,"SeaSpeakSignals") ) InterfaceStates.SeaSpeakSignals = 1; // LEO: входящие переговоры в море (вкл по умолчанию)
	if( !CheckAttribute(&InterfaceStates,"SeaCamFreeAzimuth") ) InterfaceStates.SeaCamFreeAzimuth = 0; // LEO: свободная камера в море (выкл по умолчанию - ванильное поведение)
	if( !CheckAttribute(&InterfaceStates,"GunTrail") ) InterfaceStates.GunTrail = 1; // LEO: дымный след пули на суше (вкл по умолчанию)
	if( !CheckAttribute(&InterfaceStates,"ChopBodies") ) InterfaceStates.ChopBodies = 1; // LEO: расчленение тел при взрывах
	if( !CheckAttribute(&InterfaceStates,"NpcHeadLook") ) InterfaceStates.NpcHeadLook = 1; // LEO: NPC поглядывают на соседей
	// LEO: По умолчанию автосейвы только при абордаже; в кольце 1..10 слотов.
	if( !CheckAttribute(&InterfaceStates,"AutoSaveBoarding") )	InterfaceStates.AutoSaveBoarding = 1; // ваниль: единственный автосейв
	if( !CheckAttribute(&InterfaceStates,"AutoSaveToSea") )		InterfaceStates.AutoSaveToSea = 0;    // перед выходом в море с суши
	if( !CheckAttribute(&InterfaceStates,"AutoSaveMapToSea") )	InterfaceStates.AutoSaveMapToSea = 0; // перед выходом в море с глобалки
	if( !CheckAttribute(&InterfaceStates,"AutoSaveToLand") )		InterfaceStates.AutoSaveToLand = 0;   // перед заходом на сушу с моря
	if( !CheckAttribute(&InterfaceStates,"AutoSaveToMap") )		InterfaceStates.AutoSaveToMap = 0;    // перед выходом на глобалку
	if( !CheckAttribute(&InterfaceStates,"AutoSaveSlotsBoard") )	InterfaceStates.AutoSaveSlotsBoard = 3;  // слотов в кольце, 1..10
	if( !CheckAttribute(&InterfaceStates,"AutoSaveSlotsSea") )	InterfaceStates.AutoSaveSlotsSea = 3;
	if( !CheckAttribute(&InterfaceStates,"AutoSaveSlotsMapSea") )	InterfaceStates.AutoSaveSlotsMapSea = 3;
	if( !CheckAttribute(&InterfaceStates,"AutoSaveSlotsLand") )	InterfaceStates.AutoSaveSlotsLand = 3;
	if( !CheckAttribute(&InterfaceStates,"AutoSaveSlotsMap") )	InterfaceStates.AutoSaveSlotsMap = 3;
	if( !CheckAttribute(&InterfaceStates,"FleetFormation") ) 		InterfaceStates.FleetFormation = 0; // LEO: индекс формации флота (0..5, 0=Коробка)
	if( !CheckAttribute(&InterfaceStates,"FleetFormationCfg") ) 	InterfaceStates.FleetFormationCfg = 0; // LEO: чекбокс строки-мастера «Выбор формации» (косметика, окно открывается по клику)
	if( !CheckAttribute(&InterfaceStates,"RadarEnabled") ) 			InterfaceStates.RadarEnabled = 0; // LEO Этап 3: фича радара по умолчанию выключена (вкл чекбоксом, карта по хоткею P)
	if( !CheckAttribute(&InterfaceStates,"RadarBlipScale") )			InterfaceStates.RadarBlipScale = 1.0;  // размер объектов (0.2..5.0)
	if( !CheckAttribute(&InterfaceStates,"RadarRange") )			InterfaceStates.RadarRange = 60.0;     // дальность улицы, м (20..300; синхрон с хоткеями)
	if( !CheckAttribute(&InterfaceStates,"RadarRangeInside") )		InterfaceStates.RadarRangeInside = 10.0;// дальность интерьера, м (5..120)
	if( !CheckAttribute(&InterfaceStates,"RadarMapRes") )			InterfaceStates.RadarMapRes = 2048;    // качество карты-текстуры (1024/2048/4096)
	if( !CheckAttribute(&InterfaceStates,"RadarRadius") )			InterfaceStates.RadarRadius = 210.0;   // LEO: экранный размер радара, px (120..320)
	if( !CheckAttribute(&InterfaceStates,"RadarShape") )			InterfaceStates.RadarShape = 0;        // 0=квадрат, 1=круг
	if( !CheckAttribute(&InterfaceStates,"RadarRotate") )			InterfaceStates.RadarRotate = 1;       // вращать карту по компасу
	WorldRadar_InitDefaults(); // LEO: дефолты радара глобалки (WorldRadarOn/Shape/Range/Size/Show*) — до заполнения мастер-таблицы

	NewHUD_OptionsData();
	Ragdoll_InitOptions();
	FillOptionsTable("TABLE_GAME_OPTIONS");
	FillOptionsTable("TABLE_GAME_OPTIONS_MASTER"); // LEO: правая таблица мастер-опций
	FillOptionsTable("TABLE_SOUND_OPTIONS");
}

void procCheckBoxChange()
{
	string sNodName = GetEventData();
	int nBtnIndex = GetEventData();
	int bBtnState = GetEventData();
	if(CommandIcons_CheckChange(sNodName,nBtnIndex,bBtnState)) return;
	if(NewHUD_CheckChange(sNodName,bBtnState)) return;
	if(NewLandHUD_CheckChange(sNodName,bBtnState)) return;
	if(Ragdoll_CheckChange(sNodName,bBtnState)) return;

	if( sNodName == "HERB_CHECKBOX" )
	{
		if( bBtnState==true ) {
			switch( nBtnIndex ) {
			case 1: iGrassQuality=3; break;
			case 2: iGrassQuality=2; break;
			case 3: iGrassQuality=1; break;
			case 4: iGrassQuality=0; break;
			}
		}
		return;
	}
	if( sNodName == "ALWAYS_RUN_CHECKBOX" )		SetAlwaysRun( bBtnState );
	if( sNodName == "INVERT_MOUSE_CHECKBOX" )	InterfaceStates.InvertCameras = bBtnState;
	
	if( sNodName == "CHECK_BOARDING_SHAKE" )	InterfaceStates.PortraitShakeEnable = bBtnState; // LEO: тряска портрета
	if( sNodName == "CHECK_BOARDING_PING" )		InterfaceStates.PortraitPingEnable = bBtnState;  // LEO: пинг портрета

	// LEO Этап 3: чекбоксы групп радара — флаг RadarShow* + живое применение фильтра (LandRadar_SendFilter сам гейтит по локации)
	if( sNodName == "CHECK_RADAR_CITIZEN" )		{ InterfaceStates.RadarShowCitizen = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_PATROL" )		{ InterfaceStates.RadarShowPatrol = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_MUSKET" )		{ InterfaceStates.RadarShowMusket = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_ALLY" )		{ InterfaceStates.RadarShowAlly = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_SKELETON" )	{ InterfaceStates.RadarShowSkeleton = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_QUEST" )		{ InterfaceStates.RadarShowQuest = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_CALLY" )		{ InterfaceStates.RadarShowCombatAlly = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_CENEMY" )		{ InterfaceStates.RadarShowCombatEnemy = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_DEAD" )		{ InterfaceStates.RadarShowDead = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_ITEM" )		{ InterfaceStates.RadarShowItem = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_CHEST" )		{ InterfaceStates.RadarShowChest = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_HERB" )		{ InterfaceStates.RadarShowHerb = bBtnState; LandRadar_SendFilter(); }
	if( sNodName == "CHECK_RADAR_DOORS" )		{ InterfaceStates.RadarShowDoors = bBtnState; LandRadar_SendFilter(); }
	// LEO Этап 3: форма (круглая/квадратная) и вращение карты — через SETRADARPARAMS живьём
	if( sNodName == "CHECK_RADAR_SHAPE" )		{ InterfaceStates.RadarShape = bBtnState; LandRadar_SendParams(); }
	if( sNodName == "CHECK_RADAR_ROTATE" )		{ InterfaceStates.RadarRotate = bBtnState; LandRadar_SendParams(); }
	// LEO: чекбоксы радара ГЛОБАЛКИ — флаг + живое применение (WorldRadar_ApplyLive гейтит IsEntity внутри)
	if( sNodName == "CHECK_WRADAR_ENC" )		{ InterfaceStates.WorldRadarShowEnc = bBtnState; WorldRadar_ApplyLive(); }
	if( sNodName == "CHECK_WRADAR_STORM" )		{ InterfaceStates.WorldRadarShowStorm = bBtnState; WorldRadar_ApplyLive(); }
	if( sNodName == "CHECK_WRADAR_FORTS" )		{ InterfaceStates.WorldRadarShowForts = bBtnState; WorldRadar_ApplyLive(); }
	if( sNodName == "CHECK_WRADAR_SHAPE" )		{ InterfaceStates.WorldRadarShape = bBtnState; WorldRadar_ApplyLive(); }
	if( sNodName == "CHECK_CHARCAP_SHOWOTHER" )	InterfaceStates.CharCapShowOthers = bBtnState;
	if( sNodName == "CHECK_CHARCAP_SHOWOFF" )	InterfaceStates.CharCapShowOff = bBtnState;
	if( sNodName == "CHECK_CHARCAP_SHOWNOTIFICATIONS" )	InterfaceStates.CharCapShowNotifications = bBtnState;
	if( sNodName == "CHECK_CHARCAP_SHOWRANK" )	InterfaceStates.CharCapShowRank = bBtnState;
	if( sNodName == "CHECK_CHARCAP_SHOWSPEC" )	InterfaceStates.CharCapShowSpec = bBtnState;
	if( sNodName == "CHECK_CHARCAP_TARGET_OFICERS" )	InterfaceStates.CharCapTargetOfficers = bBtnState;
	// LEO: бары над персонажами (суша) и иконки боевых эффектов — живое переключение;
	// знаки квестов — применяются при следующей маркировке NPC
	if( sNodName == "CHECK_CHARCAP_LANDBARS" )	{ InterfaceStates.CharCapLandBars = bBtnState; LandBars_ApplyLive(); }
	if( sNodName == "CHECK_CHARCAP_QUESTMARKS" )	InterfaceStates.EnabledQuestsMarks = bBtnState;
	if( sNodName == "CHECK_CHARCAP_FXMARKS" )	{ InterfaceStates.EnabledFXMarks = bBtnState; LandEffectIcons_ApplyLive(); }

	// LEO: контекстная компоновка независима от фильтров категорий расходников.
	if( sNodName == "CHECK_CONSUMABLES_CONTEXT" )
	{
		InterfaceStates.ConsumablesHUDContext = bBtnState;
		return;
	}
	// LEO: окно HUD расходников — РАДИО из 3 опций (оба ряда / выкл верхний / выкл нижний), взаимоисключающие
	if( sNodName == "RADIO_CONSUMABLES" )
	{
		if( bBtnState == true )
		{
			switch( nBtnIndex )
			{
				case 1: // показывать оба ряда
					InterfaceStates.ConsumablesNoTop = 0;
					InterfaceStates.ConsumablesNoBottom = 0;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_CONSUMABLES", 2, 2, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_CONSUMABLES", 2, 3, false );
				break;
				case 2: // отключить зелья и патроны (верхний ряд)
					InterfaceStates.ConsumablesNoTop = 1;
					InterfaceStates.ConsumablesNoBottom = 0;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_CONSUMABLES", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_CONSUMABLES", 2, 3, false );
				break;
				case 3: // отключить еду и трубки (нижний ряд)
					InterfaceStates.ConsumablesNoTop = 0;
					InterfaceStates.ConsumablesNoBottom = 1;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_CONSUMABLES", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_CONSUMABLES", 2, 2, false );
				break;
			}
		}
		return;
	}

	if( sNodName == "RADIO_FORMATION" )
	{
		if( bBtnState == true )
		{
			switch( nBtnIndex )
			{
				case 1: InterfaceStates.FleetFormation = 0; break;
				case 2: InterfaceStates.FleetFormation = 1; break;
				case 3: InterfaceStates.FleetFormation = 2; break;
				case 4: InterfaceStates.FleetFormation = 3; break;
				case 5: InterfaceStates.FleetFormation = 4; break;
				case 6: InterfaceStates.FleetFormation = 5; break;
			}
			int fi;
			for (fi = 1; fi <= 6; fi++)
			{
				if (fi != nBtnIndex) SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_FORMATION", 2, fi, false );
			}
			Sea_ApplyFleetFormation(); // LEO: смена формации на лету — в море компаньоны сразу перестроятся
		}
		return;
	}

	// LEO: типы автоматических сейвов - чекбоксы внутри окна мастер-опции «Автоматические сейвы»
	if( sNodName == "CHECK_AUTOSAVE_BOARD" )		InterfaceStates.AutoSaveBoarding = bBtnState;
	if( sNodName == "CHECK_AUTOSAVE_SEA" )			InterfaceStates.AutoSaveToSea = bBtnState;
	if( sNodName == "CHECK_AUTOSAVE_MAPSEA" )		InterfaceStates.AutoSaveMapToSea = bBtnState;
	if( sNodName == "CHECK_AUTOSAVE_LAND" )			InterfaceStates.AutoSaveToLand = bBtnState;
	if( sNodName == "CHECK_AUTOSAVE_MAP" )			InterfaceStates.AutoSaveToMap = bBtnState;

	if( sNodName == "CHECK_SHIPCAP_SHIPNAMES1" )	InterfaceStates.ShipCapShipNames1 = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHIPNAMES2" )	InterfaceStates.ShipCapShipNames2 = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHOWACTIONS" )	InterfaceStates.ShipCapShowActions = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHOWACTIONSPLUS" )	
	{
		InterfaceStates.ShipCapShowActionsPlus = bBtnState;
		ChangeShowIntarface();
		ChangeShowIntarface();
	}
	if( sNodName == "CHECK_SHIPCAP_SHOWALLIES" )	InterfaceStates.ShipCapShowAllies = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHOWOTHER" )		InterfaceStates.ShipCapShowOther = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHOWNOTIFICATIONS" )		InterfaceStates.ShipCapShowNotifications = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHOWCRIT" )		InterfaceStates.ShipCapShowCrit = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHOWFORTGUN" )		InterfaceStates.ShipCapShowFortGun = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHOWDAMAGE" )		InterfaceStates.ShipCapShowDamage = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHOWDAMAGESELF" )		InterfaceStates.ShipCapShowDamageSelf = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHIPMARKS" )
	{
		InterfaceStates.EnabledShipMarks = bBtnState;
		// Apply ship HUD and marker visibility immediately.
		if(bSeaActive) BattleInterface.ShifInfoVisible = bBtnState; // имя поля именно ShifInfoVisible (опечатка в кодовой базе)
		if(bSeaActive) ShipShowName();
	}
	if( sNodName == "CHECK_SHIPCAP_SHIPMARKS_NOSELF" )		InterfaceStates.DisableShipMarksSelf = bBtnState;
	if( sNodName == "CHECK_SHIPCAP_SHIPMARKS_NOCOMPANIONS" )		InterfaceStates.DisableShipMarksCompanions = bBtnState;
	// LEO: переключение текстуры баров над кораблями (новые CSP <-> старые indicators_HD) — на лету, если в море
	if( sNodName == "CHECK_SHIPCAP_OLDBARS" )
	{
		InterfaceStates.OldShipBars = bBtnState;
		if(bSeaActive) ReloadShipBars(); // пересоздать сущность баров с новой текстурой/UV
	}
	// Ship-bar orientation.
	if( sNodName == "CHECK_SHIPCAP_HORIZBARS" )
	{
		InterfaceStates.ShipBarsHorizontal = bBtnState;
		NewHUD_RebuildLayout();
	}
	if( sNodName == "FLAGALLWDM_CHECKBOX" )
	{
		if( bBtnState == true )
		{
			switch( nBtnIndex )
			{
				case 1:
					iFlagAllWdm=2;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FLAGALLWDM_CHECKBOX", 2, 2, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FLAGALLWDM_CHECKBOX", 2, 3, false );
				break;
				case 2:
					iFlagAllWdm=1;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FLAGALLWDM_CHECKBOX", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FLAGALLWDM_CHECKBOX", 2, 3, false );
				break;
				case 3:
					iFlagAllWdm=0;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FLAGALLWDM_CHECKBOX", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FLAGALLWDM_CHECKBOX", 2, 2, false );
				break;
			}
		}
		return;
	}

	if( sNodName == "STREAMERMODE_CHECKBOX" )
	{
		if( bBtnState == true )
		{
			switch( nBtnIndex )
			{
				case 1:
					InterfaceStates.StreamerMode = 0;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STREAMERMODE_CHECKBOX", 2, 2, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STREAMERMODE_CHECKBOX", 2, 3, false );
				break;
				case 2:
					InterfaceStates.StreamerMode = 1;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STREAMERMODE_CHECKBOX", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STREAMERMODE_CHECKBOX", 2, 3, false );
				break;
				case 3:
					InterfaceStates.StreamerMode = 2;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STREAMERMODE_CHECKBOX", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STREAMERMODE_CHECKBOX", 2, 2, false );
				break;
			}
			ReapplyMusicSet(); // LEO: применяем набор музыки сразу, а не при следующей смене локации
		}
		return;
	}

	if( sNodName == "VISUAL_CIRASS_CHECKBOX" )
	{
		InterfaceStates.VISUAL_CIRASS = 0

		if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "VISUAL_CIRASS_CHECKBOX", 3, 1))
		{
			InterfaceStates.VISUAL_CIRASS = sti(InterfaceStates.VISUAL_CIRASS)+1;
		}
		if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "VISUAL_CIRASS_CHECKBOX", 3, 2))
		{
			InterfaceStates.VISUAL_CIRASS = sti(InterfaceStates.VISUAL_CIRASS)+3;
		}
		if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "VISUAL_CIRASS_CHECKBOX", 3, 3))
		{
			InterfaceStates.VISUAL_CIRASS = sti(InterfaceStates.VISUAL_CIRASS)+5;
		}
		return;
	}
	
	if( sNodName == "INTERFACE_SELECTION" )
	{
		if( bBtnState == true )
		{
			switch( nBtnIndex )
			{
				case 1:
					InterfaceStates.InterfaceSelection=0;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INTERFACE_SELECTION", 2, 2, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INTERFACE_SELECTION", 2, 3, false );
				break;
				case 2:
					InterfaceStates.InterfaceSelection=1;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INTERFACE_SELECTION", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INTERFACE_SELECTION", 2, 3, false );
				break;
				case 3:
					InterfaceStates.InterfaceSelection=2;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INTERFACE_SELECTION", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INTERFACE_SELECTION", 2, 2, false );
				break;
			}
		}
		NewHUD_RebuildLayout();
		return;
	}

	if( sNodName == "ALTFONT_CHECKBOX" )
	{
		if( bBtnState == true )
		{
			switch( nBtnIndex )
			{
				case 1:
					InterfaceStates.AltFont=0;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALTFONT_CHECKBOX", 2, 2, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALTFONT_CHECKBOX", 2, 3, false );
				break;
				case 2:
					InterfaceStates.AltFont=1;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALTFONT_CHECKBOX", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALTFONT_CHECKBOX", 2, 3, false );
				break;
				case 3:
					InterfaceStates.AltFont=2;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALTFONT_CHECKBOX", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALTFONT_CHECKBOX", 2, 2, false );
				break;
			}
		}
		return;
	}

	if( sNodName == "PARTICLES_CHECKBOX" )
	{
		if( bBtnState == true )
		{
			switch( nBtnIndex )
			{
				case 1:
					InterfaceStates.BombsParticles=0;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_CHECKBOX", 2, 2, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_CHECKBOX", 2, 3, false );
				break;
				case 2:
					InterfaceStates.BombsParticles=1;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_CHECKBOX", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_CHECKBOX", 2, 3, false );
				break;
				case 3:
					InterfaceStates.BombsParticles=2;
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_CHECKBOX", 2, 1, false );
					SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_CHECKBOX", 2, 2, false );
				break;
			}
		}
		return;
	}
	// Apply island vegetation immediately from the graphics tab.
	if( sNodName == "ISLVEG_TYPE_CHECKBOX")
	{
		InterfaceStates.IslandVegType = (-1 + nBtnIndex);
		ApplyIslandVegetation();
		return;
	}
	if( sNodName == "ISLVEG_DENSITY_CHECKBOX")
	{
		InterfaceStates.IslandVegDensity = (-1 + nBtnIndex);
		ApplyIslandVegetation();
		return;
	}
	if( sNodName == "ISLVEG_BAND_CHECKBOX")
	{
		InterfaceStates.IslandVegBand = (-1 + nBtnIndex);
		ApplyIslandVegetation();
		return;
	}
	if( sNodName == "PARTICLES_FIRE_CHECKBOX")
	{
		InterfaceStates.FireParticles = (-1 + nBtnIndex);
		return;
	}
	if( sNodName == "PARTICLES_OTHER_CHECKBOX")
	{
		InterfaceStates.OtherParticles = (-1 + nBtnIndex);
		return;
	}
}

void procSlideChange()
{
	string sNodeName = GetEventData();
	int nVal = GetEventData();
	float fVal = GetEventData();
	if(Ragdoll_SlideChange(sNodeName,nVal)) return;
	if(NewSeaHUD_SlideChange(sNodeName,fVal)) return;
	if(NewLandHUD_SlideChange(sNodeName,fVal)) return;

	if( sNodeName=="GAMMA_SLIDE" || sNodeName=="BRIGHT_SLIDE" || sNodeName=="CONTRAST_SLIDE" ) {
		ChangeVideoColor();
		return;
	}
	if( sNodeName == "SEA_DETAILS_SLIDE" ) {
		ChangeSeaDetail();
		return;
	}
	if( sNodeName == "HERB_LAND_DISTANCE_SLIDE" ) {
		ChangeGrassDistanceLand();
		return;
	}
	if( sNodeName == "HERB_SEA_DISTANCE_SLIDE" ) {
		ChangeGrassDistanceSea();
		return;
	}
	if( sNodeName == "HERB_QUANTITY_SLIDE" ) {
		ChangeGrassCountLand();
		return;
	}
	if( sNodeName == "HERB_TRANSP_SLIDE" ) {
		ChangeGrassCountSea();
		return;
	}
	if( sNodeName == "CAM_PERSP_SLIDE" ) {
		ChangePerspDetail();
		return;
	}
	if( sNodeName == "CAM_PERSP_SLIDE2" ) {
		ChangePersp2Detail();
		return;
	}
	if( sNodeName == "CAM_RAD_SLIDE" ) {
		ChangeRadDetail();
		return;
	}
	if( sNodeName == "HUD_SLIDE" ) {
		ChangeHUDDetail();
		return;
	}
	if( sNodeName=="MUSIC_SLIDE" || sNodeName=="SOUND_SLIDE" || sNodeName=="DIALOG_SLIDE" ) {
		ChangeSoundSetting();
		return;
	}
	if( sNodeName=="CHARCAP_FONT_SCALE_SLIDE" || sNodeName=="CHARCAP_NOTIFICATIONS_SCALE_SLIDE" || sNodeName=="CHARCAP_RANGE_SLIDE" || sNodeName=="CHARCAP_ENEMYBAR_SIZE_SLIDE" || sNodeName=="CHARCAP_ENEMYBAR_DIST_SLIDE" || sNodeName=="CHARCAP_ENEMYBAR_MAX_SLIDE" ) {
		ChangeCharCap();
		return;
	}
	if( sNodeName=="SHIPCAP_NOTIFICATION_DISTANCE_SLIDE" || sNodeName=="SHIPCAP_NOTIFICATION_SCALE_SLIDE" || sNodeName=="SHIPCAP_NAMES_SCALE_SLIDE" || sNodeName=="SHIPCAP_ICONS_SCALE_SLIDE") {
		ChangeShipCap();
		return;
	}
	if( sNodeName=="VMOUSE_SENSITIVITY_SLIDE" || sNodeName=="HMOUSE_SENSITIVITY_SLIDE" ) {
		ChangeMouseSensitivity();
	}
	if( sNodeName=="PARTICLES_COUNT_SLIDE") {
		ChangeParticles();
	}
	if( sNodeName=="DIALOG_INPUT_DELAY_SLIDE") {
		ChangeDialogInputDelay();
	}
	if( sNodeName=="TIME_ACCEL_SEABATTLE_SLIDE") {
		ChangeTimeAccelSeaBattle();
	}
	if( sNodeName=="TIME_ACCEL_SEA_SLIDE") {
		ChangeTimeAccelSea();
	}
	if( sNodeName=="TIME_ACCEL_LAND_SLIDE") {
		ChangeTimeAccelLand();
	}
	// LEO: слайдеры числа слотов автосейвов (по типам)
	if( sNodeName=="AUTOSAVE_BOARD_SLIDE") {
		ChangeAutoSaveSlots("Board", "autosave_board_slide", "AUTOSAVE_BOARD_TEXT");
	}
	if( sNodeName=="AUTOSAVE_SEA_SLIDE") {
		ChangeAutoSaveSlots("Sea", "autosave_sea_slide", "AUTOSAVE_SEA_TEXT");
	}
	if( sNodeName=="AUTOSAVE_MAPSEA_SLIDE") {
		ChangeAutoSaveSlots("MapSea", "autosave_mapsea_slide", "AUTOSAVE_MAPSEA_TEXT");
	}
	if( sNodeName=="AUTOSAVE_LAND_SLIDE") {
		ChangeAutoSaveSlots("Land", "autosave_land_slide", "AUTOSAVE_LAND_TEXT");
	}
	if( sNodeName=="AUTOSAVE_MAP_SLIDE") {
		ChangeAutoSaveSlots("Map", "autosave_map_slide", "AUTOSAVE_MAP_TEXT");
	}
	if( sNodeName=="RADAR_BLIP_SCALE_SLIDE") {
		ChangeRadarBlipScale();
	}
	if( sNodeName=="CONSUMABLES_SCALE_SLIDE") {
		ChangeConsumablesHUDScale();
	}
	if( sNodeName=="CONSUMABLES_POS_SLIDE") {
		ChangeConsumablesHUDPos();
	}
	if( sNodeName=="RADAR_MAP_SCALE_SLIDE") {
		ChangeRadarMapSize();
	}
	if( sNodeName=="RADAR_INSIDE_SCALE_SLIDE") {
		ChangeRadarRangeInside();
	}
	if( sNodeName=="RADAR_SIZE_SLIDE") {
		ChangeRadarSize();
	}
	if( sNodeName=="WRADAR_SIZE_SLIDE") {
		ChangeWorldRadarSize();
	}
	if( sNodeName=="WRADAR_ZOOM_SLIDE") {
		ChangeWorldRadarZoom();
	}
	if( sNodeName=="WRADAR_BLIP_SLIDE") {
		ChangeWorldRadarBlip();
	}
}

void ChangeMouseSensitivity()
{
	InterfaceStates.mouse.x_sens = stf(GameInterface.nodes.hmouse_sensitivity_slide.value);
	InterfaceStates.mouse.y_sens = stf(GameInterface.nodes.vmouse_sensitivity_slide.value);
	SetRealMouseSensitivity();
	SetFormatedText("HMOUSE_SENSITIVITY_TEXT", its(makeint(makefloat(InterfaceStates.mouse.x_sens)*100)));
	SetFormatedText("VMOUSE_SENSITIVITY_TEXT", its(makeint(makefloat(InterfaceStates.mouse.y_sens)*100)));
}

// LEO: слайдер задержки приёма ввода диалога. value слайдера 0..1 -> задержка 0..2 сек (InterfaceStates.DialogInputDelay),
//      её движок берёт через Dialog.InputDelay (см. dialog.c) и гейтит DlgAction первые N сек на каждой ноде.
void ChangeDialogInputDelay()
{
	float sl = stf(GameInterface.nodes.dialog_input_delay_slide.value);
	InterfaceStates.DialogInputDelay = sl * 2.0;
	SetFormatedText("DIALOG_INPUT_DELAY_TEXT", its(makeint(sl * 2.0 * 1000)) + " ms");
}

// Борода: слайдеры множителя ускорения времени. Диапазон 2..потолок скрипта (seadogs.c ProcessControls,
// TimeScaleFaster/Slower): бой на море и суша - x9, открытое море - x13. Минимум - x2 (дефолт), ниже нельзя.
void ChangeTimeAccelSeaBattle()
{
	float sl = stf(GameInterface.nodes.time_accel_seabattle_slide.value);
	int iVal = 2 + makeint(sl * 7.0 + 0.5);
	InterfaceStates.TimeAccelSeaBattle = iVal;
	SetFormatedText("TIME_ACCEL_SEABATTLE_TEXT", "x"+its(iVal));
}

void ChangeTimeAccelSea()
{
	float sl = stf(GameInterface.nodes.time_accel_sea_slide.value);
	int iVal = 2 + makeint(sl * 11.0 + 0.5);
	InterfaceStates.TimeAccelSea = iVal;
	SetFormatedText("TIME_ACCEL_SEA_TEXT", "x"+its(iVal));
}

void ChangeTimeAccelLand()
{
	float sl = stf(GameInterface.nodes.time_accel_land_slide.value);
	int iVal = 2 + makeint(sl * 7.0 + 0.5);
	InterfaceStates.TimeAccelLand = iVal;
	SetFormatedText("TIME_ACCEL_LAND_TEXT", "x"+its(iVal));
}

// Борода: сброс множителей на дефолт (x2) и синхронизация ползунков/текста в открытом окне
void ResetTimeAccel()
{
	InterfaceStates.TimeAccelSeaBattle = 2;
	InterfaceStates.TimeAccelSea = 2;
	InterfaceStates.TimeAccelLand = 2;

	GameInterface.nodes.TIME_ACCEL_SEABATTLE_SLIDE.value = 0.0;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "TIME_ACCEL_SEABATTLE_SLIDE", 0, stf(GameInterface.nodes.TIME_ACCEL_SEABATTLE_SLIDE.value));
	SetFormatedText("TIME_ACCEL_SEABATTLE_TEXT", "x2");

	GameInterface.nodes.TIME_ACCEL_SEA_SLIDE.value = 0.0;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "TIME_ACCEL_SEA_SLIDE", 0, stf(GameInterface.nodes.TIME_ACCEL_SEA_SLIDE.value));
	SetFormatedText("TIME_ACCEL_SEA_TEXT", "x2");

	GameInterface.nodes.TIME_ACCEL_LAND_SLIDE.value = 0.0;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "TIME_ACCEL_LAND_SLIDE", 0, stf(GameInterface.nodes.TIME_ACCEL_LAND_SLIDE.value));
	SetFormatedText("TIME_ACCEL_LAND_TEXT", "x2");
}

// LEO Этап 3: слайдер размера блипов радара (0..1 -> 0.2..5.0; движок клампит 0.2..5.0). Живо применяем фильтр.
void ChangeRadarBlipScale()
{
	float sl = stf(GameInterface.nodes.radar_blip_scale_slide.value);
	int k = makeint(sl * 48.0 + 0.5); // округляем к шагу (pitchCounter=48); иначе float-погрешность даёт неровные шаги
	float sc = 0.2 + k * 0.1;         // 0.2..5.0 шаг 0.1
	InterfaceStates.RadarBlipScale = sc;
	SetFormatedText("RADAR_BLIP_SCALE_TEXT", FloatToString(sc, 1));
	LandRadar_SendFilter(); // гейтит bLandInterfaceStart внутри (в меню — ничего не шлёт)
}

// LEO: слайдер размера HUD расходников (0.5..2.0×, шаг 0.05); поллинг LandInterface живо пересобирает HUD
void ChangeConsumablesHUDScale()
{
	float sl = stf(GameInterface.nodes.consumables_scale_slide.value);
	int k = makeint(sl * 30.0 + 0.5); // pitchCounter=30
	float sc = 0.5 + k * 0.05;        // 0.5..2.0 шаг 0.05
	InterfaceStates.ConsumablesHUDScale = sc;
	SetFormatedText("CONSUMABLES_SCALE_TEXT", FloatToString(sc, 2));
}

// LEO: слайдер положения HUD расходников по горизонтали (0 - к левому краю экрана, 0.5 - центр,
//   1 - к правому, шаг 5%). Поллинг LandInterface живо пересобирает HUD по новому значению.
void ChangeConsumablesHUDPos()
{
	float sl = stf(GameInterface.nodes.consumables_pos_slide.value);
	int k = makeint(sl * 20.0 + 0.5); // pitchCounter=20
	float ps = k * 0.05;
	InterfaceStates.ConsumablesHUDPos = ps;
	SetFormatedText("CONSUMABLES_POS_TEXT", its(k * 5) + "%");
}

// LEO Этап 3: слайдер «Размер карты (улица)» = дальность обзора на суше RadarRange (0..1 -> 20..300 м).
// Синхронизирован с хоткеями скейлинга Б/Ю (та же InterfaceStates.RadarRange). Живо применяем параметры.
void ChangeRadarMapSize()
{
	float sl = stf(GameInterface.nodes.radar_map_scale_slide.value);
	int rg = 20 + makeint(sl * 280.0 + 0.5); // округляем к шагу 1м (pitchCounter=280, диапазон 20..300)
	InterfaceStates.RadarRange = rg;
	SetFormatedText("RADAR_MAP_SCALE_TEXT", its(rg));
	LandRadar_SendParams();
}

// LEO Этап 3: слайдер «Размер карты (внутри)» = дальность обзора в интерьере RadarRangeInside (0..1 -> 5..120 м).
// Синхронизирован с хоткеями Б/Ю в интерьерах. Живо применяем (SETRADARINSIDE).
void ChangeRadarRangeInside()
{
	float sl = stf(GameInterface.nodes.radar_inside_scale_slide.value);
	int rg = 5 + makeint(sl * 115.0 + 0.5); // округляем к шагу 1м (pitchCounter=115, диапазон 5..120)
	InterfaceStates.RadarRangeInside = rg;
	SetFormatedText("RADAR_INSIDE_SCALE_TEXT", its(rg));
	LandRadar_SendInside();
}

// LEO: слайдер «Размер радара» = экранный полуразмер RadarRadius (0..1 -> 120..320 px). Живо применяем (SETRADARPARAMS).
void ChangeRadarSize()
{
	float sl = stf(GameInterface.nodes.radar_size_slide.value);
	int px = 120 + makeint(sl * 200.0 + 0.5); // округляем к шагу 1px (pitchCounter=200, диапазон 120..320)
	InterfaceStates.RadarRadius = px;
	SetFormatedText("RADAR_SIZE_TEXT", its(px));
	LandRadar_SendParams(); // живо (читает RadarRadius)
}

// LEO: слайдер «Размер радара» ГЛОБАЛКИ = экранный полуразмер WorldRadarSize (0..1 -> 120..320 px). Живо применяем.
void ChangeWorldRadarSize()
{
	float sl = stf(GameInterface.nodes.wradar_size_slide.value);
	int px = 120 + makeint(sl * 200.0 + 0.5); // округляем к шагу 1px (pitchCounter=200, диапазон 120..320)
	InterfaceStates.WorldRadarSize = px;
	SetFormatedText("WRADAR_SIZE_TEXT", its(px));
	WorldRadar_ApplyLive(); // живо (гейтит IsEntity внутри)
}

// LEO: слайдер «Охват (множитель)» ГЛОБАЛКИ = WorldRadarRange (0..1 -> 0.3..3.0). Синхрон с хоткеями Б/Ю (та же переменная). Живо применяем.
void ChangeWorldRadarZoom()
{
	float sl = stf(GameInterface.nodes.wradar_zoom_slide.value);
	int k = makeint(sl * 54.0 + 0.5); // округляем к шагу 0.05 (pitchCounter=54, диапазон 0.3..3.0)
	float f = 0.3 + k * 0.05;
	InterfaceStates.WorldRadarRange = f;
	SetFormatedText("WRADAR_ZOOM_TEXT", FloatToString(f, 2) + "x");
	WorldRadar_ApplyLive();
}

// LEO: слайдер «Размер объектов» ГЛОБАЛКИ = масштаб блипов WorldRadarBlip (0..1 -> 0.2..5.0, шаг 0.1). Живо применяем.
void ChangeWorldRadarBlip()
{
	float sl = stf(GameInterface.nodes.wradar_blip_slide.value);
	int k = makeint(sl * 48.0 + 0.5); // pitchCounter=48, шаг 0.1
	float sc = 0.2 + k * 0.1;         // 0.2..5.0
	InterfaceStates.WorldRadarBlip = sc;
	SetFormatedText("WRADAR_BLIP_TEXT", FloatToString(sc, 1));
	WorldRadar_ApplyLive();
}

// LEO: стрелки «Качество карты» ГЛОБАЛКИ (циклически 1024<->2048<->4096). Смена = пере-рендер RT в движке (на входе на карту).
void ChangeWorldRadarQuality(bool way)
{
	int res = 1024;
	if( CheckAttribute(&InterfaceStates,"WorldRadarMapRes") ) res = sti(InterfaceStates.WorldRadarMapRes);
	if( way )
	{
		if( res == 1024 ) res = 2048;
		else if( res == 2048 ) res = 4096;
		else res = 1024;
	}
	else
	{
		if( res == 4096 ) res = 2048;
		else if( res == 2048 ) res = 1024;
		else res = 4096;
	}
	InterfaceStates.WorldRadarMapRes = res;
	SetFormatedText("WRADAR_QUALITY_TEXT", its(res));
	WorldRadar_ApplyLive();
}

// LEO Этап 3: стрелки качества карты (циклически 1024<->2048<->4096). Дорого (ре-рендер карты) — применяем по клику.
void ChangeRadarQuality(bool way)
{
	int res = 2048;
	if( CheckAttribute(&InterfaceStates,"RadarMapRes") ) res = sti(InterfaceStates.RadarMapRes);
	if( way )
	{
		if( res == 1024 ) res = 2048;
		else if( res == 2048 ) res = 4096;
		else res = 1024;
	}
	else
	{
		if( res == 4096 ) res = 2048;
		else if( res == 2048 ) res = 1024;
		else res = 4096;
	}
	InterfaceStates.RadarMapRes = res;
	SetFormatedText("RADAR_QUALITY_TEXT", its(res));
	LandRadar_ApplyQuality();
}

// Resolution index in InterfaceStates.video.resIdx maps to width and height.
// Gregg: список под аспекты 16:9 / 16:10 / 4:3 / 5:4 + опциональные высокие (4K / 21:9), по возрастанию ширины.
// Всего пунктов = 17, дефолт (idx при незнакомом разрешении) = 10 (1920x1080).
int VideoResW(int idx)
{
	int w = 1920;
	switch(idx)
	{
		case 0:  w = 1024; break; // 4:3
		case 1:  w = 1280; break; // 16:9
		case 2:  w = 1280; break; // 16:10
		case 3:  w = 1280; break; // 4:3
		case 4:  w = 1280; break; // 5:4
		case 5:  w = 1366; break; // 16:9
		case 6:  w = 1440; break; // 16:10
		case 7:  w = 1600; break; // 16:9
		case 8:  w = 1600; break; // 4:3
		case 9:  w = 1680; break; // 16:10
		case 10: w = 1920; break; // 16:9
		case 11: w = 1920; break; // 16:10
		case 12: w = 2560; break; // 21:9
		case 13: w = 2560; break; // 16:9
		case 14: w = 2560; break; // 16:10 (2K)
		case 15: w = 3440; break; // 21:9
		case 16: w = 3840; break; // 16:9 (4K)
	}
	return w;
}

int VideoResH(int idx)
{
	int h = 1080;
	switch(idx)
	{
		case 0:  h = 768;  break; // 1024x768   4:3
		case 1:  h = 720;  break; // 1280x720   16:9
		case 2:  h = 800;  break; // 1280x800   16:10
		case 3:  h = 960;  break; // 1280x960   4:3
		case 4:  h = 1024; break; // 1280x1024  5:4
		case 5:  h = 768;  break; // 1366x768   16:9
		case 6:  h = 900;  break; // 1440x900   16:10
		case 7:  h = 900;  break; // 1600x900   16:9
		case 8:  h = 1200; break; // 1600x1200  4:3
		case 9:  h = 1050; break; // 1680x1050  16:10
		case 10: h = 1080; break; // 1920x1080  16:9
		case 11: h = 1200; break; // 1920x1200  16:10
		case 12: h = 1080; break; // 2560x1080  21:9
		case 13: h = 1440; break; // 2560x1440  16:9
		case 14: h = 1600; break; // 2560x1600  16:10 (2K)
		case 15: h = 1440; break; // 3440x1440  21:9
		case 16: h = 2160; break; // 3840x2160  16:9 (4K)
	}
	return h;
}

void ApplyResIndex(int idx)
{
	if( idx < 0 )    idx = 0;
	if( idx >= 17 )  idx = 16;
	int w = VideoResW(idx);
	int h = VideoResH(idx);
	InterfaceStates.video.width  = w;
	InterfaceStates.video.height = h;
	SetFormatedText("RESOLUTION_TEXT", its(w) + "x" + its(h));
}

void ChangeResolution(bool way)
{
	int N = 17;
	int idx = 10;
	if( CheckAttribute(&InterfaceStates,"video.resIdx") ) idx = sti(InterfaceStates.video.resIdx);
	if( way ) { idx = idx + 1; if( idx >= N ) idx = 0; }
	else      { idx = idx - 1; if( idx < 0 )  idx = N - 1; }
	InterfaceStates.video.resIdx = idx;
	ApplyResIndex(idx);
	UpdateRestartHint();
}

// Gregg: кэш скриптов (cache_mode 0/1/2) — рестарт-онли; спиннер по кругу 0->1->2->0
void ApplyScriptCacheIdx(int v)
{
	InterfaceStates.video.scriptCache = v;
	if( v == 0 )      SetFormatedText("SCRIPTCACHE_TEXT", XI_ConvertString("VideoOff"));
	else if( v == 1 ) SetFormatedText("SCRIPTCACHE_TEXT", XI_ConvertString("VideoOn"));
	else              SetFormatedText("SCRIPTCACHE_TEXT", XI_ConvertString("ScriptCacheFast"));
}
void ChangeScriptCache(bool way)
{
	int v = 0;
	if( CheckAttribute(&InterfaceStates,"video.scriptCache") ) v = sti(InterfaceStates.video.scriptCache);
	if( way ) { v = v + 1; if( v > 2 ) v = 0; }
	else      { v = v - 1; if( v < 0 ) v = 2; }
	ApplyScriptCacheIdx(v);
	UpdateRestartHint();
}

// Gregg: Тени персонажей (0=Нет,1=Упрощённые,2=Стандартные) — рестарт-онли
void ApplyShadowIdx(int v)
{
	InterfaceStates.video.shadowMode = v;
	if( v == 0 )      SetFormatedText("SHADOW_TEXT", XI_ConvertString("ShadowOff"));
	else if( v == 1 ) SetFormatedText("SHADOW_TEXT", XI_ConvertString("ShadowSimple"));
	else              SetFormatedText("SHADOW_TEXT", XI_ConvertString("ShadowStandard"));
}
void ChangeShadow(bool way)
{
	int v = 2;
	if( CheckAttribute(&InterfaceStates,"video.shadowMode") ) v = sti(InterfaceStates.video.shadowMode);
	if( way ) { v = v + 1; if( v > 2 ) v = 0; }
	else      { v = v - 1; if( v < 0 ) v = 2; }
	ApplyShadowIdx(v);
	UpdateRestartHint();
}

// Gregg: Корабельные флары (0=Сокращённые,1=Полные) — рестарт-онли
void ApplyFlareIdx(int v)
{
	InterfaceStates.video.flareMode = v;
	if( v == 0 ) SetFormatedText("FLARE_TEXT", XI_ConvertString("FlareReduced"));
	else         SetFormatedText("FLARE_TEXT", XI_ConvertString("FlareFull"));
}
void ChangeFlare(bool way)
{
	int v = 1;
	if( CheckAttribute(&InterfaceStates,"video.flareMode") ) v = sti(InterfaceStates.video.flareMode);
	if( way ) { v = v + 1; if( v > 1 ) v = 0; }
	else      { v = v - 1; if( v < 0 ) v = 1; }
	ApplyFlareIdx(v);
	UpdateRestartHint();
}

// LEO: старый движок не публикует возможности; с новым показываем только поддерживаемые режимы.
bool IsMSAASupported(int samples)
{
	if (samples == 0) return true;
	if (samples < 2 || samples > 16) return false;
	if (!CheckAttribute(&Render, "msaaSupportedMask")) return true;
	return and(sti(Render.msaaSupportedMask), shl(1, samples)) != 0;
}

void ApplyMSAAVal(int s)
{
	if (s < 0) s = 0;
	if (s > 16) s = 16;
	while (s > 0)
	{
		if (IsMSAASupported(s)) break;
		s = s - 1;
	}
	InterfaceStates.video.msaa = s;
	if( s == 0 ) SetFormatedText("MSAA_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("MSAA_TEXT", its(s) + "x");
}
void ChangeMSAA(bool way)
{
	int s = 0;
	if( CheckAttribute(&InterfaceStates,"video.msaa") ) s = sti(InterfaceStates.video.msaa);
	int idx = 0;
	if( s == 2 ) idx = 1; else if( s == 4 ) idx = 2; else if( s == 8 ) idx = 3; else if( s == 16 ) idx = 4; else idx = 0;
	int ns = 0;
	for (int step = 0; step < 5; step++)
	{
		if( way ) { idx = idx + 1; if( idx > 4 ) idx = 0; }
		else      { idx = idx - 1; if( idx < 0 ) idx = 4; }
		switch(idx) { case 0: ns = 0; break; case 1: ns = 2; break; case 2: ns = 4; break; case 3: ns = 8; break; case 4: ns = 16; break; }
		if (IsMSAASupported(ns))
		{
			ApplyMSAAVal(ns);
			break;
		}
	}
	UpdateRestartHint();
}

// Gregg: Верт. синхронизация (0/1)
void ApplyVSyncIdx(int v)
{
	InterfaceStates.video.vsync = v;
	if( v == 0 ) SetFormatedText("VSYNC_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("VSYNC_TEXT", XI_ConvertString("VideoOn"));
}
void ChangeVSync(bool way)
{
	int v = 0;
	if( CheckAttribute(&InterfaceStates,"video.vsync") ) v = sti(InterfaceStates.video.vsync);
	if( v == 0 ) v = 1; else v = 0;
	ApplyVSyncIdx(v);
	UpdateRestartHint();
}

// Gregg: Letterbox (0/1) — чёрные поля вместо обрезки интерфейса на узких аспектах (4:3/5:4/16:10)
void ApplyLetterboxIdx(int v)
{
	InterfaceStates.video.letterbox = v;
	if( v == 0 ) SetFormatedText("LETTERBOX_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("LETTERBOX_TEXT", XI_ConvertString("VideoOn"));
}
void ChangeLetterbox(bool way)
{
	int v = 0;
	if( CheckAttribute(&InterfaceStates,"video.letterbox") ) v = sti(InterfaceStates.video.letterbox);
	if( v == 0 ) v = 1; else v = 0;
	ApplyLetterboxIdx(v);
}

// Gregg: Показ FPS (0/1)
void ApplyShowFpsIdx(int v)
{
	InterfaceStates.video.showFps = v;
	if( v == 0 ) SetFormatedText("SHOWFPS_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("SHOWFPS_TEXT", XI_ConvertString("VideoOn"));
}
void ChangeShowFps(bool way)
{
	int v = 0;
	if( CheckAttribute(&InterfaceStates,"video.showFps") ) v = sti(InterfaceStates.video.showFps);
	if( v == 0 ) v = 1; else v = 0;
	ApplyShowFpsIdx(v);
}

// Gregg: Макс. FPS (0 = без ограничения / 30 / 60 / 120 / 144 / 165 / 180 / 240 / 280)
void ApplyMaxFpsVal(int f)
{
	InterfaceStates.video.maxFps = f;
	if( f == 0 ) SetFormatedText("MAXFPS_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("MAXFPS_TEXT", its(f));
}
void ChangeMaxFps(bool way)
{
	int f = 0;
	if( CheckAttribute(&InterfaceStates,"video.maxFps") ) f = sti(InterfaceStates.video.maxFps);
	int idx = 0;
	if( f == 30 ) idx = 1; else if( f == 60 ) idx = 2; else if( f == 120 ) idx = 3; else if( f == 144 ) idx = 4; else if( f == 165 ) idx = 5; else if( f == 180 ) idx = 6; else if( f == 240 ) idx = 7; else if( f == 280 ) idx = 8; else idx = 0;
	if( way ) { idx = idx + 1; if( idx > 8 ) idx = 0; }
	else      { idx = idx - 1; if( idx < 0 ) idx = 8; }
	int nf = 0;
	switch(idx) { case 1: nf = 30; break; case 2: nf = 60; break; case 3: nf = 120; break; case 4: nf = 144; break; case 5: nf = 165; break; case 6: nf = 180; break; case 7: nf = 240; break; case 8: nf = 280; break; }
	ApplyMaxFpsVal(nf);
}

// Gregg: Кэш текстур (VRAM), МБ — LRU-удержание текстур между локациями (0=выкл, шаг 512 до 8192 = 8 ГБ)
void ApplyRetainMBVal(int v)
{
	InterfaceStates.video.retainMB = v;
	if( v == 0 ) SetFormatedText("RETAINMB_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("RETAINMB_TEXT", its(v) + " MB");
}
void ChangeRetainMB(bool way)
{
	int v = 0;
	if( CheckAttribute(&InterfaceStates,"video.retainMB") ) v = sti(InterfaceStates.video.retainMB);
	// Gregg: VRAM-текстуры — шаг 512, потолок 8192 (8 ГБ); кламп без заворота
	if( way ) { v = v + 512; if( v > 8192 ) v = 8192; }
	else      { v = v - 512; if( v < 0 )    v = 0;    }
	ApplyRetainMBVal(v);
}

// Gregg: Кэш геометрии (VRAM), МБ — LRU-удержание моделей (.gm) между локациями (0=выкл, шаг 256 до 4096 = 4 ГБ)
void ApplyGeoRetainMBVal(int v)
{
	InterfaceStates.video.geoRetainMB = v;
	if( v == 0 ) SetFormatedText("GEORETAINMB_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("GEORETAINMB_TEXT", its(v) + " MB");
}
void ChangeGeoRetainMB(bool way)
{
	int v = 0;
	if( CheckAttribute(&InterfaceStates,"video.geoRetainMB") ) v = sti(InterfaceStates.video.geoRetainMB);
	// Gregg: VRAM-геометрия — шаг 256, потолок 4096 (4 ГБ); кламп без заворота
	if( way ) { v = v + 256; if( v > 4096 ) v = 4096; }
	else      { v = v - 256; if( v < 0 )    v = 0;    }
	ApplyGeoRetainMBVal(v);
}

// LEO: Подсказку перезапуска сравниваем с живым устройством и boot-значениями, не с отложенным выбором.
void UpdateRestartHint()
{
	bool need = false;
	if( CheckAttribute(&InterfaceStates,"video.width") )
	{
		if( sti(InterfaceStates.video.width)      != sti(Render.screen_x) )    need = true;
		if( sti(InterfaceStates.video.height)     != sti(Render.screen_y) )    need = true;
		if( sti(InterfaceStates.video.scriptCache) != sti(Render.scriptCacheModeBoot) ) need = true;
		if( sti(InterfaceStates.video.msaa)       != sti(Render.msaa) )        need = true;
		if( sti(InterfaceStates.video.vsync)      != sti(Render.vsync) )       need = true;
		if( sti(InterfaceStates.video.warmAssets) != sti(Render.warmAssetsBoot) )     need = true;
		if( sti(InterfaceStates.video.warmTexMB)  != sti(Render.warmTexturesMBBoot) ) need = true;
		if( sti(InterfaceStates.video.pinSky) != sti(Render.pinTexturesBoot) ) need = true;
		if( sti(InterfaceStates.video.shadowMode) != sti(Render.shadowModeBoot) ) need = true;
		if( sti(InterfaceStates.video.flareMode)  != sti(Render.flareModeBoot) )  need = true;
	}
	if( need ) SetFormatedText("RESTART_HINT", XI_ConvertString("RestartRequired"));
	else       SetFormatedText("RESTART_HINT", "");
}

// Gregg: Прогрев в ОЗУ (WarmAssets, 0/1) — фоновая предзагрузка моделей+звуков в RAM при старте. РЕСТАРТ-ОНЛИ.
void ApplyWarmAssetsIdx(int v)
{
	InterfaceStates.video.warmAssets = v;
	if( v == 0 ) SetFormatedText("WARMASSETS_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("WARMASSETS_TEXT", XI_ConvertString("VideoOn"));
}
void ChangeWarmAssets(bool way)
{
	int v = 0;
	if( CheckAttribute(&InterfaceStates,"video.warmAssets") ) v = sti(InterfaceStates.video.warmAssets);
	if( v == 0 ) v = 1; else v = 0;
	ApplyWarmAssetsIdx(v);
	UpdateRestartHint();
}

// Gregg: Прогрев текстур (МБ) в ОЗУ (WarmTexMB, 0=выкл, шаг 512 до 32768 = 32 ГБ). Работает при включённом прогреве. РЕСТАРТ-ОНЛИ.
void ApplyWarmTexMBVal(int v)
{
	InterfaceStates.video.warmTexMB = v;
	if( v == 0 ) SetFormatedText("WARMTEXMB_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("WARMTEXMB_TEXT", its(v) + " MB");
}
void ChangeWarmTexMB(bool way)
{
	int v = 0;
	if( CheckAttribute(&InterfaceStates,"video.warmTexMB") ) v = sti(InterfaceStates.video.warmTexMB);
	// Gregg: RAM-текстуры — шаг 512, потолок 32768 (32 ГБ); кламп без заворота
	if( way ) { v = v + 512; if( v > 32768 ) v = 32768; }
	else      { v = v - 512; if( v < 0 )     v = 0;     }
	ApplyWarmTexMBVal(v);
	UpdateRestartHint();
}

// Gregg: Кэш неба (PinTextures 0/1) — доп. ref на наборы неба, без хитча при смене часа суток. РЕСТАРТ-ОНЛИ.
void ApplyPinSkyIdx(int v)
{
	InterfaceStates.video.pinSky = v;
	if( v == 0 ) SetFormatedText("PINSKY_TEXT", XI_ConvertString("VideoOff"));
	else         SetFormatedText("PINSKY_TEXT", XI_ConvertString("VideoOn"));
}
void ChangePinSky(bool way)
{
	int v = 1;
	if( CheckAttribute(&InterfaceStates,"video.pinSky") ) v = sti(InterfaceStates.video.pinSky);
	if( v == 0 ) v = 1; else v = 0;
	ApplyPinSkyIdx(v);
	UpdateRestartHint();
}

void GetVideoModeData()
{
	// LEO: При каждом открытии перечитываем видеонастройки из Render:
	// InterfaceStates может прийти из старого сейва.
	// LEO: Настройки, требующие перезапуска, берём из отложенных Render.video*,
	// а не из текущих размеров экрана, используемых HUD.
	// LEO: Разрешение сопоставляем целиком: одна ширина встречается у разных пропорций.
	int cw = sti(Render.videoW);
	int ch = sti(Render.videoH);
	int idx = 10;
	bool bResFound = false;
	int i;
	for( i = 0; i < 17; i++ )
	{
		if( VideoResW(i) == cw )
		{
			if( VideoResH(i) == ch ) { idx = i; bResFound = true; }
		}
	}
	InterfaceStates.video.resIdx     = idx;
	InterfaceStates.video.scriptCache = sti(Render.scriptCacheMode);
	InterfaceStates.video.msaa       = sti(Render.videoMsaa);
	InterfaceStates.video.vsync      = sti(Render.videoVsync);
	InterfaceStates.video.showFps    = sti(Render.show_fps);
	InterfaceStates.video.maxFps     = sti(Render.max_fps);
	InterfaceStates.video.letterbox  = sti(Render.letterbox);
	InterfaceStates.video.retainMB   = sti(Render.retainMB);
	InterfaceStates.video.geoRetainMB = sti(Render.geoRetainMB);
	InterfaceStates.video.warmAssets = sti(Render.warmAssets);
	InterfaceStates.video.warmTexMB  = sti(Render.warmTexturesMB);
	InterfaceStates.video.pinSky = sti(Render.pinTextures);
	InterfaceStates.video.shadowMode = sti(Render.shadowMode);
	InterfaceStates.video.flareMode = sti(Render.flareMode);
	// LEO: Неизвестное разрешение сохраняем: ОК без изменений не должен его понижать.
	if( bResFound )
	{
		ApplyResIndex(idx);
	}
	else
	{
		InterfaceStates.video.width  = cw;
		InterfaceStates.video.height = ch;
		SetFormatedText("RESOLUTION_TEXT", its(cw) + "x" + its(ch));
	}
	ApplyScriptCacheIdx(sti(InterfaceStates.video.scriptCache));
	ApplyShadowIdx(sti(InterfaceStates.video.shadowMode));
	ApplyFlareIdx(sti(InterfaceStates.video.flareMode));
	InitCannonHeatHazeOption();
	InitFlagPhysicsOption();
	ApplyMSAAVal(sti(InterfaceStates.video.msaa));
	ApplyVSyncIdx(sti(InterfaceStates.video.vsync));
	ApplyShowFpsIdx(sti(InterfaceStates.video.showFps));
	ApplyMaxFpsVal(sti(InterfaceStates.video.maxFps));
	ApplyLetterboxIdx(sti(InterfaceStates.video.letterbox));
	ApplyRetainMBVal(sti(InterfaceStates.video.retainMB));
	ApplyGeoRetainMBVal(sti(InterfaceStates.video.geoRetainMB));
	ApplyWarmAssetsIdx(sti(InterfaceStates.video.warmAssets));
	ApplyWarmTexMBVal(sti(InterfaceStates.video.warmTexMB));
	ApplyPinSkyIdx(sti(InterfaceStates.video.pinSky));
	UpdateRestartHint();
}

void ChangeVideoColor()
{
	float fCurContrast = stf(GameInterface.nodes.contrast_slide.value);
	float fCurGamma = stf(GameInterface.nodes.GAMMA_SLIDE.value);
	float fCurBright = stf(GameInterface.nodes.BRIGHT_SLIDE.value);

	float fContrast = ConvertContrast(fCurContrast,false);
	float fGamma = ConvertGamma(fCurGamma,false);
	float fBright = ConvertBright(fCurBright,false);

	// LEO: В старом профиле может отсутствовать любая часть группы video.
	bool changed=true;
	if(CheckAttribute(&InterfaceStates,"video.contrast"))
	{
		if(CheckAttribute(&InterfaceStates,"video.gamma"))
		{
			if(CheckAttribute(&InterfaceStates,"video.brightness"))
				changed=stf(InterfaceStates.video.contrast)!=fContrast || stf(InterfaceStates.video.gamma)!=fGamma || stf(InterfaceStates.video.brightness)!=fBright;
		}
	}
	if(changed) {
			InterfaceStates.video.contrast = fContrast;
			InterfaceStates.video.gamma = fGamma;
			InterfaceStates.video.brightness = fBright;
			XI_SetColorCorrection(fContrast,fGamma,fBright);
	}
	SetFormatedText("GAMMA_TEXT", its(makeint(fCurGamma*100)));
	SetFormatedText("BRIGHTNESS_TEXT", its(makeint(fCurBright*100)));
	SetFormatedText("CONTRAST_TEXT", its(makeint(fCurContrast*100)));
}

void ChangeSeaDetail()
{
	float fCurSeaDetail = stf(GameInterface.nodes.sea_details_slide.value);
	float fSeaDetail = ConvertSeaDetails(fCurSeaDetail,false);
	InterfaceStates.SeaDetails = fSeaDetail;
	SetFormatedText("SEA_DETAIL_TEXT", its(makeint(fSeaDetail*100)));
}

void ChangePerspDetail()
{
    float fCurPerspDetail = stf(GameInterface.nodes.cam_persp_slide.value);
	InterfaceStates.PerspDetails = fCurPerspDetail;
	float fCamPersp = CalcSeaPerspective();
	SetFormatedText("SEACAMPERSP_TEXT", FloatToString(fCamPersp,3));
}
void ChangePersp2Detail()
{
    float fCurPerspDetail = stf(GameInterface.nodes.cam_persp_slide2.value);
	InterfaceStates.Persp2Details = fCurPerspDetail;
	float fCamPersp = CalcLandPerspective();
	SetFormatedText("LANDCAMPERSP_TEXT", FloatToString(fCamPersp,3));
}

void ChangeRadDetail()
{
    float fCurPerspDetail = stf(GameInterface.nodes.cam_rad_slide.value);
	InterfaceStates.RadDetails = fCurPerspDetail;
	float fCamPersp = CalcLandRadiusNew();
	SetFormatedText("CAMDISTANCE_TEXT", FloatToString(fCamPersp,1));
}

void ChangeSoundSetting()
{
	float fMusic = stf(GameInterface.nodes.music_slide.value);
	float fSound = stf(GameInterface.nodes.sound_slide.value);
	float fDialog = stf(GameInterface.nodes.dialog_slide.value);
	SendMessage(&sound,"lfff", MSG_SOUND_SET_MASTER_VOLUME, fSound,	fMusic,	fDialog);
	SetFormatedText("MUSIC_TEXT", its(makeint(fMusic*100)));
	SetFormatedText("SOUND_TEXT", its(makeint(fSound*100)));
	SetFormatedText("DIALOG_TEXT", its(makeint(fDialog*100)));
}

void FillControlsList(int nMode)
{
	int n,qC,idx;
	string groupName;
	aref arGrp, arC;

	if( nMode == g_nCurControlsMode ) {return;}
	g_nCurControlsMode = nMode;
	DeleteAttribute(&GameInterface,"controls_list");
	GameInterface.controls_list.select = 0;

	groupName = GetGroupNameByMode(nMode);
	if( CheckAttribute(&objControlsState,"keygroups."+groupName) )
	{
		makearef(arGrp,objControlsState.keygroups.(groupName));
		qC = GetAttributesNum(arGrp);
		idx = 0;
		for( n=0; n<qC; n++ ) {
			arC = GetAttributeN(arGrp,n);
			bool visible=true;
			if(CheckAttribute(arC,"invisible")) visible=arC.invisible!="1";
			if(visible)
			{
				if (GetAttributeValue(arC) == "") continue;
				bool remappable=false;
				if(CheckAttribute(arC,"remapping")) remappable=arC.remapping=="1";
				if( AddToControlsList( idx, GetAttributeName(arC), GetAttributeValue(arC), remappable ) )
				{
					idx++;
				}
			}
		}
	}
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "CONTROLS_LIST", 0 );
}

bool AddToControlsList(int row, string sControl, string sKey, bool bRemapable)
{
	string rowname = "tr" + (row+1);
	GameInterface.controls_list.(rowname).userdata.remapable = bRemapable;
	GameInterface.controls_list.(rowname).userdata.control = sControl;
	GameInterface.controls_list.(rowname).userdata.key = sKey;
	GameInterface.controls_list.(rowname).td1.str = LanguageConvertString(g_ControlsLngFile,sControl);
	GameInterface.controls_list.(rowname).td1.textoffset = "0,2";
	if( GameInterface.controls_list.(rowname).td1.str == "" ) {
		trace("Warning!!! " + sControl + " hav`t translate value");
	}
	if( !bRemapable ) { // выделение контролок которые нельзя поменять
		GameInterface.controls_list.(rowname).td1.color = argb(255,128,128,128);
	}
	if( CheckAttribute(&objControlsState,"key_codes."+sKey+".img") )
	{
		GameInterface.controls_list.(rowname).td2.fontidx = 0;
		GameInterface.controls_list.(rowname).td2.textoffset = "-1,-1";
		GameInterface.controls_list.(rowname).td2.scale = 0.6;
		GameInterface.controls_list.(rowname).td2.str = objControlsState.key_codes.(sKey).img;
	}
	return true;
}

string GetGroupNameByMode(int nMode)
{
	switch( nMode ) {
	case 1: return "Sailing1Pers"; break;
	case 2: return "PrimaryLand"; break;
	case 3: return "Sailing3Pers"; break;
	case 4: return "FightModeControls"; break;
	}
	return "unknown";
}

void GetSoundOptionsData()
{
	float fCurMusic = 0.5;
	float fCurSound = 0.5;
	float fCurDialog = 0.5;
	SendMessage(&sound,"leee",MSG_SOUND_GET_MASTER_VOLUME,&fCurSound,&fCurMusic,&fCurDialog);
	GameInterface.nodes.music_slide.value = fCurMusic;
	GameInterface.nodes.sound_slide.value = fCurSound;
	GameInterface.nodes.dialog_slide.value = fCurDialog;
}

void GetMouseOptionsData()
{
	float fCurXSens = 0.5;
	float fCurYSens = 0.5;
	if( CheckAttribute(&InterfaceStates,"mouse.x_sens") ) {fCurXSens=stf(InterfaceStates.mouse.x_sens);}
	if( CheckAttribute(&InterfaceStates,"mouse.y_sens") ) {fCurYSens=stf(InterfaceStates.mouse.y_sens);}
	if(fCurXSens<0.0) fCurXSens = 0.0;
	if(fCurXSens>1.0) fCurXSens = 1.0;
	if(fCurYSens<0.0) fCurYSens = 0.0;
	if(fCurYSens>1.0) fCurYSens = 1.0;
	GameInterface.nodes.hmouse_sensitivity_slide.value = fCurXSens;
	GameInterface.nodes.vmouse_sensitivity_slide.value = fCurYSens;
}

void GetVideoOptionsData()
{
	// LEO: Значения по умолчанию нужны до чтения слайдеров и IReadVariableAfterInit.
	if( !CheckAttribute(&InterfaceStates,"TimeAccelSettings") ) 	InterfaceStates.TimeAccelSettings = 0;
	if( !CheckAttribute(&InterfaceStates,"TimeAccelSeaBattle") )	InterfaceStates.TimeAccelSeaBattle = 2;
	if( !CheckAttribute(&InterfaceStates,"TimeAccelSea") )			InterfaceStates.TimeAccelSea = 2;
	if( !CheckAttribute(&InterfaceStates,"TimeAccelLand") )		InterfaceStates.TimeAccelLand = 2;

	float fC = 1.0;
	float fG = 1.0;
	float fB = 0.0;
	float fD = 1.0;
	float fE = 0.0;
	float fE2 = 0.0;
	float fR = 0.0;
	int fDS = 0;

	if( CheckAttribute(&InterfaceStates,"video.contrast") ) {
		fC = stf(InterfaceStates.video.contrast);
	}
	if( CheckAttribute(&InterfaceStates,"video.gamma") ) {
		fG = stf(InterfaceStates.video.gamma);
	}
	if( CheckAttribute(&InterfaceStates,"video.brightness") ) {
		fB = stf(InterfaceStates.video.brightness);
	}

	if( CheckAttribute(&InterfaceStates,"SeaDetails") ) {
		fD = stf(InterfaceStates.SeaDetails);
	}
	if( CheckAttribute(&InterfaceStates,"GrassDistanceLand") ) {
		GameInterface.nodes.HERB_LAND_DISTANCE_SLIDE.value = stf(InterfaceStates.GrassDistanceLand);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HERB_LAND_DISTANCE_SLIDE", 0, stf(GameInterface.nodes.HERB_LAND_DISTANCE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"GrassDistanceSea") ) {
		GameInterface.nodes.HERB_SEA_DISTANCE_SLIDE.value = stf(InterfaceStates.GrassDistanceSea);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HERB_SEA_DISTANCE_SLIDE", 0, stf(GameInterface.nodes.HERB_SEA_DISTANCE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"GrassCountLand") ) {
		GameInterface.nodes.HERB_QUANTITY_SLIDE.value = stf(InterfaceStates.GrassCountLand);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HERB_QUANTITY_SLIDE", 0, stf(GameInterface.nodes.HERB_QUANTITY_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"GrassCountSea") ) {
		GameInterface.nodes.HERB_TRANSP_SLIDE.value = stf(InterfaceStates.GrassCountSea);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HERB_TRANSP_SLIDE", 0, stf(GameInterface.nodes.HERB_TRANSP_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"CharCapFontScale") ) {
		GameInterface.nodes.CHARCAP_FONT_SCALE_SLIDE.value = stf(InterfaceStates.CharCapFontScale);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_FONT_SCALE_SLIDE", 1, stf(GameInterface.nodes.CHARCAP_FONT_SCALE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"CharCapNotifScale") ) {
		GameInterface.nodes.CHARCAP_NOTIFICATIONS_SCALE_SLIDE.value = stf(InterfaceStates.CharCapNotifScale);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_NOTIFICATIONS_SCALE_SLIDE", 1, stf(GameInterface.nodes.CHARCAP_NOTIFICATIONS_SCALE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"EnemyBarSize") ) {
		GameInterface.nodes.CHARCAP_ENEMYBAR_SIZE_SLIDE.value = stf(InterfaceStates.EnemyBarSize);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_ENEMYBAR_SIZE_SLIDE", 1, stf(GameInterface.nodes.CHARCAP_ENEMYBAR_SIZE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"EnemyBarDist") ) {
		GameInterface.nodes.CHARCAP_ENEMYBAR_DIST_SLIDE.value = stf(InterfaceStates.EnemyBarDist);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_ENEMYBAR_DIST_SLIDE", 1, stf(GameInterface.nodes.CHARCAP_ENEMYBAR_DIST_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"EnemyBarMax") ) {
		GameInterface.nodes.CHARCAP_ENEMYBAR_MAX_SLIDE.value = stf(InterfaceStates.EnemyBarMax);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "CHARCAP_ENEMYBAR_MAX_SLIDE", 1, stf(GameInterface.nodes.CHARCAP_ENEMYBAR_MAX_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapNotifDistance") ) {
		GameInterface.nodes.SHIPCAP_NOTIFICATION_DISTANCE_SLIDE.value = stf(InterfaceStates.ShipCapNotifDistance);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "SHIPCAP_NOTIFICATION_DISTANCE_SLIDE", 1, stf(GameInterface.nodes.SHIPCAP_NOTIFICATION_DISTANCE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapNotifScale") ) {
		GameInterface.nodes.SHIPCAP_NOTIFICATION_SCALE_SLIDE.value = stf(InterfaceStates.ShipCapNotifScale);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "SHIPCAP_NOTIFICATION_SCALE_SLIDE", 1, stf(GameInterface.nodes.SHIPCAP_NOTIFICATION_SCALE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapNamesScale") ) {
		GameInterface.nodes.SHIPCAP_NAMES_SCALE_SLIDE.value = stf(InterfaceStates.ShipCapNamesScale);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "SHIPCAP_NAMES_SCALE_SLIDE", 1, stf(GameInterface.nodes.SHIPCAP_NAMES_SCALE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapIconsScale") ) {
		GameInterface.nodes.SHIPCAP_ICONS_SCALE_SLIDE.value = stf(InterfaceStates.ShipCapIconsScale);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "SHIPCAP_ICONS_SCALE_SLIDE", 1, stf(GameInterface.nodes.SHIPCAP_ICONS_SCALE_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"ParticlesCount") ) {
		GameInterface.nodes.PARTICLES_COUNT_SLIDE.value = stf(InterfaceStates.ParticlesCount);
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "PARTICLES_COUNT_SLIDE", 1, stf(GameInterface.nodes.PARTICLES_COUNT_SLIDE.value));
	}
	if( CheckAttribute(&InterfaceStates,"DialogInputDelay") ) {
		GameInterface.nodes.DIALOG_INPUT_DELAY_SLIDE.value = stf(InterfaceStates.DialogInputDelay) / 2.0;
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "DIALOG_INPUT_DELAY_SLIDE", 1, stf(GameInterface.nodes.DIALOG_INPUT_DELAY_SLIDE.value));
		SetFormatedText("DIALOG_INPUT_DELAY_TEXT", its(makeint(stf(InterfaceStates.DialogInputDelay)*1000)) + " ms");
	}
	// LEO: бегунки числа слотов автосейвов (шкала 1..10 -> 0.0..1.0)
	int iAsB = AutoSave_SlotsQty("Board");
	GameInterface.nodes.AUTOSAVE_BOARD_SLIDE.value = makefloat(iAsB - 1) / 9.0;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "AUTOSAVE_BOARD_SLIDE", 1, stf(GameInterface.nodes.AUTOSAVE_BOARD_SLIDE.value));
	SetFormatedText("AUTOSAVE_BOARD_TEXT", its(iAsB));
	int iAsS = AutoSave_SlotsQty("Sea");
	GameInterface.nodes.AUTOSAVE_SEA_SLIDE.value = makefloat(iAsS - 1) / 9.0;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "AUTOSAVE_SEA_SLIDE", 1, stf(GameInterface.nodes.AUTOSAVE_SEA_SLIDE.value));
	SetFormatedText("AUTOSAVE_SEA_TEXT", its(iAsS));
	int iAsMS = AutoSave_SlotsQty("MapSea");
	GameInterface.nodes.AUTOSAVE_MAPSEA_SLIDE.value = makefloat(iAsMS - 1) / 9.0;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "AUTOSAVE_MAPSEA_SLIDE", 1, stf(GameInterface.nodes.AUTOSAVE_MAPSEA_SLIDE.value));
	SetFormatedText("AUTOSAVE_MAPSEA_TEXT", its(iAsMS));
	int iAsL = AutoSave_SlotsQty("Land");
	GameInterface.nodes.AUTOSAVE_LAND_SLIDE.value = makefloat(iAsL - 1) / 9.0;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "AUTOSAVE_LAND_SLIDE", 1, stf(GameInterface.nodes.AUTOSAVE_LAND_SLIDE.value));
	SetFormatedText("AUTOSAVE_LAND_TEXT", its(iAsL));
	int iAsM = AutoSave_SlotsQty("Map");
	GameInterface.nodes.AUTOSAVE_MAP_SLIDE.value = makefloat(iAsM - 1) / 9.0;
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "AUTOSAVE_MAP_SLIDE", 1, stf(GameInterface.nodes.AUTOSAVE_MAP_SLIDE.value));
	SetFormatedText("AUTOSAVE_MAP_TEXT", its(iAsM));

	if( CheckAttribute(&InterfaceStates,"TimeAccelSeaBattle") ) {
		int iTASeaBattle = sti(InterfaceStates.TimeAccelSeaBattle);
		if (iTASeaBattle < 2) iTASeaBattle = 2; // минимум x2
		GameInterface.nodes.TIME_ACCEL_SEABATTLE_SLIDE.value = makefloat(iTASeaBattle - 2) / 7.0;
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "TIME_ACCEL_SEABATTLE_SLIDE", 1, stf(GameInterface.nodes.TIME_ACCEL_SEABATTLE_SLIDE.value));
		SetFormatedText("TIME_ACCEL_SEABATTLE_TEXT", "x"+its(iTASeaBattle));
	}
	if( CheckAttribute(&InterfaceStates,"TimeAccelSea") ) {
		int iTASea = sti(InterfaceStates.TimeAccelSea);
		if (iTASea < 2) iTASea = 2; // минимум x2
		GameInterface.nodes.TIME_ACCEL_SEA_SLIDE.value = makefloat(iTASea - 2) / 11.0;
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "TIME_ACCEL_SEA_SLIDE", 1, stf(GameInterface.nodes.TIME_ACCEL_SEA_SLIDE.value));
		SetFormatedText("TIME_ACCEL_SEA_TEXT", "x"+its(iTASea));
	}
	if( CheckAttribute(&InterfaceStates,"TimeAccelLand") ) {
		int iTALand = sti(InterfaceStates.TimeAccelLand);
		if (iTALand < 2) iTALand = 2; // минимум x2
		GameInterface.nodes.TIME_ACCEL_LAND_SLIDE.value = makefloat(iTALand - 2) / 7.0;
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "TIME_ACCEL_LAND_SLIDE", 1, stf(GameInterface.nodes.TIME_ACCEL_LAND_SLIDE.value));
		SetFormatedText("TIME_ACCEL_LAND_TEXT", "x"+its(iTALand));
	}
	
	if( CheckAttribute(&InterfaceStates,"PerspDetails") ) {
		fE = stf(InterfaceStates.PerspDetails);
	}
	if( CheckAttribute(&InterfaceStates,"Persp2Details") ) {
		fE2 = stf(InterfaceStates.Persp2Details);
	}
	if( CheckAttribute(&InterfaceStates,"RadDetails") ) {
		fR = stf(InterfaceStates.RadDetails);
	}
	ISetColorCorrection( fC, fG, fB, fD, fE, fE2, fR );
}
void ISetColorCorrection(float fContrast, float fGamma, float fBright, float fSeaDetails, float fPerspDetails, float fPersp2, float fRad)
{
	float fCurContrast = ConvertContrast(fContrast,true);
	float fCurGamma = ConvertGamma(fGamma,true);
	float fCurBright = ConvertBright(fBright,true);
	float fCurSeaDetails = ConvertSeaDetails(fSeaDetails, true);

	if(fCurContrast>1.0) fCurContrast = 1.0;
	if(fCurContrast<0.0) fCurContrast = 0.0;
	if(fCurGamma>1.0) fCurGamma = 1.0;
	if(fCurGamma<0.0) fCurGamma = 0.0;
	if(fCurBright>1.0) fCurBright = 1.0;
	if(fCurBright<0.0) fCurBright = 0.0;
	if(fCurSeaDetails<0.0) fCurSeaDetails = 0.0;
	if(fCurSeaDetails>1.0) fCurSeaDetails = 1.0;
	if(fPerspDetails<0.0) fPerspDetails = 0.0;
	if(fPerspDetails>1.0) fPerspDetails = 1.0;
	if(fPersp2<0.0) fPersp2 = 0.0;
	if(fPersp2>1.0) fPersp2 = 1.0;
	if(fRad<0.0) fRad = 0.0;
	if(fRad>1.0) fRad = 1.0;

	GameInterface.nodes.CONTRAST_SLIDE.value = fCurContrast;
	GameInterface.nodes.GAMMA_SLIDE.value = fCurGamma;
	GameInterface.nodes.BRIGHT_SLIDE.value = fCurBright;
	GameInterface.nodes.SEA_DETAILS_SLIDE.value = fCurSeaDetails;
	GameInterface.nodes.CAM_PERSP_SLIDE.value = fPerspDetails;
	GameInterface.nodes.CAM_PERSP_SLIDE2.value = fPersp2;
	GameInterface.nodes.CAM_RAD_SLIDE.value = fRad;

	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"CONTRAST_SLIDE", 0,fCurContrast);
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"GAMMA_SLIDE", 0,fCurGamma);
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"BRIGHT_SLIDE", 0,fCurBright);

	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"SEA_DETAILS_SLIDE", 0, fCurSeaDetails);
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"CAM_PERSP_SLIDE", 0, fPerspDetails);
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"CAM_PERSP_SLIDE2", 0, fPersp2);
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"CAM_RAD_SLIDE", 0, fRad);

	XI_SetColorCorrection(fContrast,fGamma,fBright);
}

float ConvertContrast(float fContrast, bool Real2Slider)
{ // контрастность от 0.75 до 1.25
	if(Real2Slider) {
		return fContrast*2.0-1.5;
	}
	return fContrast*0.5+0.75;
}

float ConvertGamma(float fGamma, bool Real2Slider)
{ // гамма от 0.5 до 2.0
	if(Real2Slider)
	{
		return fGamma*2.0/3.0-1.0/3.0;
	}
	return fGamma*1.5+0.5;
}

float ConvertBright(float fBright, bool Real2Slider)
{
	if(Real2Slider) {
		return (fBright+50.0)/100.0;
	}
	return fBright*100.0-50.0;
}

float ConvertSeaDetails(float fDetails, bool Real2Slider)
{
	return fDetails;
}
void GetHerbOptionsData()
{
	int nSelBtn = 0;
	switch( iGrassQuality ) {
	case 0: nSelBtn=4; break;
	case 1: nSelBtn=3; break;
	case 2: nSelBtn=2; break;
	case 3: nSelBtn=1; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"HERB_CHECKBOX", 2, nSelBtn, true );
}

void GetFlagAllOptionsData()
{
	int nSelBtn = 0;
	switch( iFlagAllWdm ) {
	case 0: nSelBtn=3; break;
	case 1: nSelBtn=2; break;
	case 2: nSelBtn=1; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FLAGALLWDM_CHECKBOX", 2, nSelBtn, true );
}

void GetStreamerModeData()
{
	int nSelBtn = 1;
	switch( sti(InterfaceStates.StreamerMode) ) {
	case 0: nSelBtn=1; break;
	case 1: nSelBtn=2; break;
	case 2: nSelBtn=3; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STREAMERMODE_CHECKBOX", 2, nSelBtn, true );
}

void GetVisualCirassOptionsData()
{
	int nSelBtn = 0;
	switch( sti(InterfaceStates.VISUAL_CIRASS) )
	{
		case 0:
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 1, false );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 2, false );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 3, false );
		break;
		case 1:
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 1, true );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 2, false );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 3, false );
		break;
		case 3:
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 1, false );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 2, true );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 3, false );
		break;
		case 4:
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 1, true );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 2, true );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 3, false );
		break;
		case 5:
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 1, false );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 2, false );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 3, true );
		break;
		case 6:
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 1, true );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 2, false );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 3, true );
		break;
		case 8:
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 1, false );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 2, true );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 3, true );
		break;
		case 9:
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 1, true );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 2, true );
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"VISUAL_CIRASS_CHECKBOX", 2, 3, true );
		break;
	}
}

void GetInterfaceSelectionData()
{
	int nSelBtn = 0;
	switch( sti(InterfaceStates.InterfaceSelection) ) {
	case 0: nSelBtn=1; break;
	case 1: nSelBtn=2; break;
	case 2: nSelBtn=3; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INTERFACE_SELECTION", 2, nSelBtn, true );
}

void GetAltFontOptionsData()
{
	int nSelBtn = 0;
	switch( sti(InterfaceStates.AltFont) ) {
	case 0: nSelBtn=1; break;
	case 1: nSelBtn=2; break;
	case 2: nSelBtn=3; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALTFONT_CHECKBOX", 2, nSelBtn, true );
}

void GetBombsParticlesOptionsData()
{
	int nSelBtn = 0;
	switch( sti(InterfaceStates.BombsParticles) ) {
	case 0: nSelBtn=1; break;
	case 1: nSelBtn=2; break;
	case 2: nSelBtn=3; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_CHECKBOX", 2, nSelBtn, true );
}

// LEO: Параметры растительности согласованы с option_sl.c и ApplyIslandVegetation.
void GetIslVegOptionsData()
{
	int nType = 1;
	int nDens = 1;
	int nBand = 1;
	if( CheckAttribute(&InterfaceStates,"IslandVegType") )    { nType = sti(InterfaceStates.IslandVegType); }
	if( CheckAttribute(&InterfaceStates,"IslandVegDensity") ) { nDens = sti(InterfaceStates.IslandVegDensity); }
	if( CheckAttribute(&InterfaceStates,"IslandVegBand") )    { nBand = sti(InterfaceStates.IslandVegBand); }
	nType = nType + 1;
	nDens = nDens + 1;
	nBand = nBand + 1;
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ISLVEG_TYPE_CHECKBOX", 2, nType, true );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ISLVEG_DENSITY_CHECKBOX", 2, nDens, true );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ISLVEG_BAND_CHECKBOX", 2, nBand, true );
}

void GetFireParticlesOptionsData()
{
	int nSelBtn = 0;
	switch( sti(InterfaceStates.FireParticles) ) {
	case 0: nSelBtn=1; break;
	case 1: nSelBtn=2; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_FIRE_CHECKBOX", 2, nSelBtn, true );
}

void GetOtherParticlesOptionsData()
{
	int nSelBtn = 0;
	switch( sti(InterfaceStates.OtherParticles) ) {
	case 0: nSelBtn=1; break;
	case 1: nSelBtn=2; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"PARTICLES_OTHER_CHECKBOX", 2, nSelBtn, true );
}

void GetCharCapOptionsData()
{
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_SHOWOTHER", 2, 1, sti(InterfaceStates.CharCapShowOthers));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_SHOWOFF", 2, 1, sti(InterfaceStates.CharCapShowOff));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_SHOWNOTIFICATIONS", 2, 1, sti(InterfaceStates.CharCapShowNotifications));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_SHOWRANK", 2, 1, sti(InterfaceStates.CharCapShowRank));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_SHOWSPEC", 2, 1, sti(InterfaceStates.CharCapShowSpec));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_TARGET_OFICERS", 2, 1, sti(InterfaceStates.CharCapTargetOfficers));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_LANDBARS", 2, 1, sti(InterfaceStates.CharCapLandBars)); // бары над персонажами
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_QUESTMARKS", 2, 1, sti(InterfaceStates.EnabledQuestsMarks)); // знаки квест
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CHARCAP_FXMARKS", 2, 1, sti(InterfaceStates.EnabledFXMarks)); // знаки эффектов
}

void GetShipCapOptionsData() 
{
	ShipInfoBars_RefreshOption();
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHIPNAMES1", 2, 1, sti(InterfaceStates.ShipCapShipNames1));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHIPNAMES2", 2, 1, sti(InterfaceStates.ShipCapShipNames2));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWACTIONS", 2, 1, sti(InterfaceStates.ShipCapShowActions));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWACTIONSPLUS", 2, 1, sti(InterfaceStates.ShipCapShowActionsPlus));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWALLIES", 2, 1, sti(InterfaceStates.ShipCapShowAllies));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWOTHER", 2, 1, sti(InterfaceStates.ShipCapShowOther));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWNOTIFICATIONS", 2, 1, sti(InterfaceStates.ShipCapShowNotifications));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWCRIT", 2, 1, sti(InterfaceStates.ShipCapShowCrit));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWFORTGUN", 2, 1, sti(InterfaceStates.ShipCapShowFortGun));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWDAMAGE", 2, 1, sti(InterfaceStates.ShipCapShowDamage));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHOWDAMAGESELF", 2, 1, sti(InterfaceStates.ShipCapShowDamageSelf));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHIPMARKS", 2, 1, sti(InterfaceStates.EnabledShipMarks));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHIPMARKS_NOSELF", 2, 1, sti(InterfaceStates.DisableShipMarksSelf));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_SHIPMARKS_NOCOMPANIONS", 2, 1, sti(InterfaceStates.DisableShipMarksCompanions));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_OLDBARS", 2, 1, sti(InterfaceStates.OldShipBars));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_SHIPCAP_HORIZBARS", 2, 1, sti(InterfaceStates.ShipBarsHorizontal));
	// LEO: чекбоксы окна HUD расходников
	// LEO: радио HUD расходников — выбрать секцию по текущему состоянию (1=оба ряда, 2=выкл верхний, 3=выкл нижний)
	int iConsRadio = 1;
	if(sti(InterfaceStates.ConsumablesNoTop)==1) iConsRadio = 2;
	if(sti(InterfaceStates.ConsumablesNoBottom)==1) iConsRadio = 3;
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_CONSUMABLES", 2, iConsRadio, true );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_CONSUMABLES_CONTEXT", 2, 1, sti(InterfaceStates.ConsumablesHUDContext));
}

void GetControlsStatesData()
{
	int nAlwaysRun = 0;
	if( CheckAttribute(&InterfaceStates,"alwaysrun") ) {
		nAlwaysRun = sti(InterfaceStates.alwaysrun);
	}
	int nInvertCam = 0;
	if( CheckAttribute(&InterfaceStates,"InvertCameras") ) {
		nInvertCam = sti(InterfaceStates.InvertCameras);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALWAYS_RUN_CHECKBOX", 2, 1, nAlwaysRun );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INVERT_MOUSE_CHECKBOX", 2, 1, nInvertCam );
}

void SetAlwaysRun(bool bRun)
{
	InterfaceStates.alwaysrun = bRun;
}

void procKeyChange()
{
	string srow = "tr" + GameInterface.controls_list.select;
	if( !CheckAttribute(&GameInterface,"controls_list."+srow) ) {return;}
	if( sti(GameInterface.controls_list.(srow).userdata.remapable)!=1 ) {return;}
	ChooseOtherControl();
}

void ChooseOtherControl()
{
	XI_WindowDisable("MAIN_WINDOW",true);
	XI_WindowShow("CHANGEKEY_WINDOW",true);
	SetCurrentNode("KEY_CHOOSER");
	string srow = "tr" + GameInterface.controls_list.select;
	SetFormatedText("CHANGEKEY_TEXT", XI_ConvertString("Press any key"));
	AddLineToFormatedText("CHANGEKEY_TEXT", " ");
	AddLineToFormatedText("CHANGEKEY_TEXT", GameInterface.controls_list.(srow).td2.str);
	AddLineToFormatedText("CHANGEKEY_TEXT", " ");
	AddLineToFormatedText("CHANGEKEY_TEXT", XI_ConvertString("KeyAlreadyUsed"));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 8, 0, COLOR_WARNING );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 8, 4, argb(0,255,64,64) );
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 5);
}

int glob_retVal;
ref procKeyChoose()
{
	int keyIdx = GetEventData();
	int stickUp = GetEventData();

	glob_retVal = false;

	if (keyIdx == 7) {
		ReturnFromReassign();
		glob_retVal = true;
		return &glob_retVal;
	}

	if( DoMapToOtherKey(keyIdx,stickUp) )
	{
		ReturnFromReassign();
		glob_retVal = true;
	}

	return &glob_retVal;
}

void ReturnFromReassign()
{
	XI_WindowShow("CHANGEKEY_WINDOW",false);
	XI_WindowDisable("MAIN_WINDOW",false);
	SetCurrentNode("CONTROLS_LIST");
}

bool DoMapToOtherKey(int keyIdx,int stickUp)
{
	string srow = "tr" + GameInterface.controls_list.select;
	string groupName = GetGroupNameByMode( g_nCurControlsMode );
	string sControl = GameInterface.controls_list.(srow).userdata.control;
	string sKey = GameInterface.controls_list.(srow).userdata.key;


	aref arControlGroup;
	aref arKeyRoot,arKey;
	string tmpstr;
	int keyCode;

	if( stickUp )
	{
		return false;
	}

	makearef(arControlGroup,objControlsState.keygroups.(groupName));
	makearef(arKeyRoot,objControlsState.key_codes);
	if (keyIdx >= GetAttributesNum(arKeyRoot)) {
		trace("ERROR: DoMapToOtherKey keyIdx more than arKeyRoot length")
		return false;
	}
	arKey = GetAttributeN(arKeyRoot,keyIdx);
	keyCode = sti(GetAttributeValue(arKey));

	// LEO: Клавиша общей справки не должна запускать другое действие.
	if (HH_BindingConflicts(sControl, GetAttributeName(arKey)))
	{
		SetKeyChooseWarning(XI_ConvertString("KeyAlreadyUsed"));
		return false;
	}

	// LEO: Общие команды справки уже проверены на конфликт выше.
	if(sControl == "HotkeysHelp" || sControl == "HotkeysHelpPrevPage" || sControl == "HotkeysHelpNextPage") { }
	else if(
		keyCode==sti(objControlsState.key_codes.VK_F2) ||
		keyCode==sti(objControlsState.key_codes.VK_F6) ||
		keyCode==sti(objControlsState.key_codes.VK_F8) ||
		keyCode==sti(objControlsState.key_codes.VK_F9) )
	{
		return false;
	}

	if(CheckAttribute(arKey,"stick"))
	{
		if(sti(arKey.stick)!=0) return false;
	}

	if( KeyAlreadyUsed(groupName, sControl, GetAttributeName(arKey)) )
	{
		SetKeyChooseWarning( XI_ConvertString("KeyAlreadyUsed") );
		if (curkey != "" && curkey != GetAttributeName(arKey))
		{
			curkey = GetAttributeName(arKey);
			return false;
		}
		if (blockkey)
		{
			blockkey = false;
			curkey = GetAttributeName(arKey);
			return false;
		}
	}

	tmpstr = arControlGroup.(sControl);
	if(CheckAttribute(arKeyRoot,tmpstr+".stick"))
	{
		if(sti(arKeyRoot.(tmpstr).stick)!=0) return false;
	}

	int state = 0;
	if(CheckAttribute(arControlGroup,sControl+".state"))
	{	state = sti(arControlGroup.(sControl).state);	}

	CI_CreateAndSetControls( groupName,sControl,keyCode, state, true );
	GameInterface.controls_list.(srow).userdata.key = arKey;
	GameInterface.controls_list.(srow).td2.str = arKey.img;
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "CONTROLS_LIST", 0 );
	curkey = "";
	blockkey = true;
	return true;
}

void RestoreDefaultKeys(int choice)
{
	SetMouseToDefault();
	if (choice == 0)
	{
		ControlsInit(GetTargetPlatform(),false, 0);
	}
	if (choice == 1)
	{
		ControlsInit(GetTargetPlatform(),false, 1);
	}

	int nMode = g_nCurControlsMode;
	g_nCurControlsMode = -1;
	FillControlsList(nMode);
}

void SetMouseToDefault()
{
	InterfaceStates.InvertCameras = false;
	InterfaceStates.mouse.x_sens = 0.5;
	InterfaceStates.mouse.y_sens = 0.5;

	SetRealMouseSensitivity();
	SetAlwaysRun(true);

	GetControlsStatesData();
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"VMOUSE_SENSITIVITY_SLIDE", 0,stf(InterfaceStates.mouse.y_sens));
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"HMOUSE_SENSITIVITY_SLIDE", 0,stf(InterfaceStates.mouse.x_sens));
}

void ShowSimpleOptionInfo(string sNode)
{
	g_bToolTipStarted = true;
	string sHeader = "TEST";

	string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
	sPicture = "none";
	int xx = 256;
	int yy = 256;

	switch (sNode)
	{
		case "RagdollEnabled":
			sHeader=XI_ConvertString("RagdollEnabled");
			sText1=XI_ConvertString("RagdollPerformance") + " " + XI_ConvertString("RagdollApplyAfterTransition");
			break;
		case "NewHUDEnabled":
			sHeader=XI_ConvertString("NewHUDEnabled");
			sText1=XI_ConvertString("NewHUDEnabled_desc");
			sPicture = "INTERFACES\FaqPictures\NEWHUD.dds";
			xx = 384;
			yy = 384;
		break;
		case "RegionsWorldmap":
			sHeader = XI_ConvertString("RegionsWorldmap");
			sText1 = XI_ConvertString("RegionsWorldmap Mode_descr");
            sPicture = "INTERFACES\FaqPictures\SEA_REGIONS.dds";
			xx = 256;
			yy = 256;
		break;
		case "SimpleSea":		//ПКМ для настроек простых внизу слева
			sHeader = XI_ConvertString("SimpleSea");
			sText1 = XI_ConvertString("SimpleSea Mode_descr");
		break;
		case "DialogPortrait": // Gregg: модельный портрет собеседника в диалоге
			sHeader = XI_ConvertString("DialogPortrait");
			sText1 = XI_ConvertString("DialogPortrait_desc");
		break;
		case "EnabledShipMarks":
			sHeader = XI_ConvertString("EnabledShipMarks");
			sText1 = XI_ConvertString("ShipMark Mode_descr");
		break;
		case "EnabledAutoSaveMode":
			sHeader = XI_ConvertString("EnabledAutoSaveMode");
			sText1 = XI_ConvertString("AutoSave Mode_descr");
		break;
		case "ShowBattleMode":
			sHeader = XI_ConvertString("ShowBattleMode");
			sText1 = XI_ConvertString("Show battle mode_descr");
		break;
		case "SpyglassTextures":
			sHeader = XI_ConvertString("SpyglassTextures");
			sText1 = XI_ConvertString("SpyglassTextures_desc");
            sPicture = "INTERFACES\FaqPictures\SPYGLASSTEX_CHECKBOX.dds";
			xx = 256;
			yy = 256;
		break;
		case "HUDStyle":
			sHeader = XI_ConvertString("HUDStyle");
			sText1 = XI_ConvertString("HUDStyle_desc");
            sPicture = "INTERFACES\FaqPictures\HUDStyle_CHECKBOX.dds";
			xx = 256;
			yy = 256;
		break;
		case "ShowBoardMode":
			sHeader = XI_ConvertString("ShowBoardMode");
			sText1 = XI_ConvertString("BOARD_MODE_descr");
            sPicture = "INTERFACES\FaqPictures\BOARD_MODE_CHECKBOX.dds";
			xx = 600;
			yy = 300;
		break;
		case "EnableSailors":
			sHeader = XI_ConvertString("EnableSailors");
			sText1 = XI_ConvertString("SAILORS_CHECKBOX_desc");
		break;
		case "CharVoice":
			sHeader = XI_ConvertString("CharVoice");
			sText1 = XI_ConvertString("CHAR_VOICE_CHECKBOX_desc");
            PlayVoice("Kopcapkz\Voices\Traders\Trader_man_0"+rand(9)+".ogg");
		break;
		case "IslandLoader":
			sHeader = XI_ConvertString("IslandLoader");
			sText1 = XI_ConvertString("IslandLoader_desc");
		break;
		case "DirectSail": // Gregg: тултип опции доплытия
			sHeader = XI_ConvertString("DirectSail");
			sText1 = XI_ConvertString("DirectSail_desc");
		break;
		case "BlueInterfaceTheme":
			sHeader = XI_ConvertString("BlueInterfaceTheme");
			sText1 = XI_ConvertString("BlueInterfaceTheme_descr");
		break;

		case "NewCommandIconsEnabled":
			sHeader = XI_ConvertString("NewCommandIconsEnabled");
			sText1 = XI_ConvertString("NewCommandIconsEnabled_desc");
			sText2 = XI_ConvertString("CommandIconsFAQCompare");
			sPicture = "INTERFACES\FaqPictures\NEW_COMMAND_ICONS.dds";
			xx = 512; yy = 192;
		break;
		case "AltIntIcons":
			sHeader = XI_ConvertString("AltIntIcons");
			sText1 = XI_ConvertString("AltIntIcons_desc");
			sText2 = XI_ConvertString("AltCommandIconsFAQCompare");
			// LEO: Пример следует текущему набору иконок, не меняя ни один из двух флагов.
			sPicture = "INTERFACES\FaqPictures\ALT_COMMAND_ICONS_LEGACY.dds";
			if(CommandIcons_Enabled()) sPicture = "INTERFACES\FaqPictures\ALT_COMMAND_ICONS_NEW.dds";
			xx = 512; yy = 192;
		break;
		case "CharCap":
			sHeader = XI_ConvertString("CharCap");
			sText1 = XI_ConvertString("CharCap_desc");
		break;
		case "ShipCap":
			sHeader = XI_ConvertString("ShipCap");
			sText1 = XI_ConvertString("ShipCap_desc");
		break;
		case "EnabledQuestsMarks":
			sHeader = XI_ConvertString("EnabledQuestsMarks");
			sText1 = XI_ConvertString("QuestMark Mode_descr");
            sPicture = "INTERFACES\FaqPictures\QUESTMARK_CHECKBOX.dds";
			xx = 256;
			yy = 256;
		break;
		case "EnabledFXMarks":
			sHeader = XI_ConvertString("EnabledFXMarks");
			sText1 = XI_ConvertString("FXMark Mode_descr");
            sPicture = "INTERFACES\FaqPictures\FXMARK_CHECKBOX.dds";
			xx = 256;
			yy = 256;
		break;
		case "EnabledCMControls":
			sHeader = XI_ConvertString("EnabledCMControls");
			sText1 = XI_ConvertString("CMControls Mode_descr");
		break;
		case "EnabledOldStore":
			sHeader = XI_ConvertString("EnabledOldStore");
			sText1 = XI_ConvertString("Store Mode_descr");
            sPicture = "INTERFACES\FaqPictures\OLDSTORE_CHECKBOX.dds";
			xx = 607;
			yy = 258;
		break;
		case "EnabledAltSoundsGun":
			sHeader = XI_ConvertString("EnabledAltSoundsGun");
			sText1 = XI_ConvertString("AltSoundsGun_descr");
            PlayVoice("CSR\GUNSFIRE_ALT\Fort_cannon_0"+rand(5)+".wav");
		break;
		case "ShowExpLogs":
			sHeader = XI_ConvertString("ShowExpLogs");
			sText1 = XI_ConvertString("ShowExpLogs_descr");
            sPicture = "INTERFACES\FaqPictures\SHOWEXPLOGS.dds";
			xx = 256;
			yy = 256;
		break;
		case "ShowGameLog":
			sHeader = XI_ConvertString("ShowGameLog");
			sText1 = XI_ConvertString("ShowGameLog_descr");
		break;
		case "ConsumablesHUD":
			sHeader = XI_ConvertString("ConsumablesHUD");
			sText1 = XI_ConvertString("ConsumablesHUD_descr");
			sPicture = "INTERFACES\FaqPictures\Hood_of_consumables.dds";
			xx = 512; yy = 256; // LEO: текстура 512x256 (2:1)
		break;
		case "DialogDelay":
			sHeader = XI_ConvertString("DialogDelay");
			sText1 = XI_ConvertString("DialogDelay_descr");
		break;
		case "TimeAccelSettings":
			sHeader = XI_ConvertString("TimeAccelSettings");
			sText1 = XI_ConvertString("TimeAccelSettings_descr");
		break;
		case "ProgressAnimation":
			sHeader = XI_ConvertString("ProgressAnimation");
			sText1 = XI_ConvertString("ProgressAnimation_descr");
		break;
		case "BoardingSignals":
			sHeader = XI_ConvertString("BoardingSignals");
			sText1 = XI_ConvertString("BoardingSignals_descr");
		break;
		case "FleetFormationCfg":
			sHeader = XI_ConvertString("FleetFormationCfg");
			sText1 = XI_ConvertString("FleetFormationCfg_descr");
		break;
		case "RadarEnabled": // LEO Этап 3: описание мастер-опции радара (ПКМ по строке)
			sHeader = XI_ConvertString("RadarEnabled");
			sText1 = XI_ConvertString("RadarEnabled_descr");
			sPicture = "INTERFACES\FaqPictures\MINIMAP_RADAR.dds";
			xx = 256; yy = 256;
		break;
		case "WorldRadarOn": // LEO: описание мастер-опции радара глобальной карты (ПКМ по строке)
			sHeader = XI_ConvertString("WorldRadarOn");
			sText1 = XI_ConvertString("WorldRadarOn_descr");
			sPicture = "INTERFACES\FaqPictures\MINIMAP_WDM_RADAR.dds";
			xx = 256; yy = 256;
		break;
		case "ShipBarsHorizontal":
			sHeader = XI_ConvertString("ShipBarsHorizontal");
			sText1 = XI_ConvertString("ShipBarsHorizontal_descr");
			sPicture = "INTERFACES\FaqPictures\HOR_BARS.dds";
			xx = 512; yy = 256; // LEO: HOR_BARS.dds 1024x512 (2:1), без растяжения в квадрат
		break;
		case "SpyglassFromMars":
			sHeader = XI_ConvertString("SpyglassFromMars");
			sText1 = XI_ConvertString("SpyglassFromMars_descr");
		break;
		case "RollingBroadside":
			sHeader = XI_ConvertString("RollingBroadside");
			sText1 = XI_ConvertString("RollingBroadside_descr");
		break;
		case "SeaSpeakSignals":
			sHeader = XI_ConvertString("SeaSpeakSignals");
			sText1 = XI_ConvertString("SeaSpeakSignals_descr");
		break;
		case "SeaCamFreeAzimuth":
			sHeader = XI_ConvertString("SeaCamFreeAzimuth");
			sText1 = XI_ConvertString("SeaCamFreeAzimuth_descr");
		break;
		case "GunTrail":
			sHeader = XI_ConvertString("GunTrail");
			sText1 = XI_ConvertString("GunTrail_descr");
		break;
		case "NpcHeadLook":
			sHeader = XI_ConvertString("NpcHeadLook");
			sText1 = XI_ConvertString("NpcHeadLook_descr");
		break;
		case "ChopBodies":
			sHeader = XI_ConvertString("ChopBodies");
			sText1 = XI_ConvertString("ChopBodies_descr");
		break;
		case "DiscordRPC":
			sHeader = XI_ConvertString("DiscordRPC");
			sText1 = XI_ConvertString("DiscordRPC_descr");
		break;
		case "StreamerMode":
			sHeader = XI_ConvertString("StreamerMode");
			sText1 = XI_ConvertString("StreamerMode_descr");
		break;
	}

	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,255,255,255), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, xx, yy);
}

void ShowInfo()
{
	g_bToolTipStarted = true;
	string sHeader = "TEST";
	string sNode = GetCurrentNode();
	if(NewHUD_OptionInfo(sNode)) return;
	if (sNode == "SHIPINFO_BARS_TEXT" || sNode == "SHIPINFO_BARS_LEFT" || sNode == "SHIPINFO_BARS_RIGHT") sNode = "SHIPINFO_BARS_LABEL";

	string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
	string sText4 = ""; // LEO: 4-я строка тултипа (для легенды радара — двери)
	sPicture = "none";
	int xx = 256;
	int yy = 256;

	switch (sNode)
	{
		case "GAMMA_SLIDE":
			sHeader = XI_ConvertString("gamma");
			sText1 = XI_ConvertString("gamma_descr");
		break;
		case "BRIGHT_SLIDE":
			sHeader = XI_ConvertString("Brightness");
			sText1 = XI_ConvertString("brightness_descr");
		break;
		case "CONTRAST_SLIDE":
			sHeader = XI_ConvertString("Contrast");
			sText1 = XI_ConvertString("Contrast_descr");
		break;
		case "LAND_DETAILS_SLIDE":
			sHeader = XI_ConvertString("Land Detail");
			sText1 = XI_ConvertString("Land Detail_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomance");
			sText3 = XI_ConvertString("NeedToExitFromLand");
		break;
		case "SEA_DETAILS_SLIDE":
			sHeader = XI_ConvertString("Sea Detail");
			sText1 = XI_ConvertString("Sea Detail_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomance");
			sText3 = XI_ConvertString("NeedToExitFromSea");
		break;
		case "HERB_LAND_DISTANCE_SLIDE":
            sHeader = XI_ConvertString("Herb Land Distance");
            sText1 = XI_ConvertString("Herb Land Distance_descr");
            sText2 = XI_ConvertString("Herb Distance_warning");
        break;
		case "HERB_SEA_DISTANCE_SLIDE":
            sHeader = XI_ConvertString("Herb Sea Distance");
            sText1 = XI_ConvertString("Herb Sea Distance_descr");
            sText2 = XI_ConvertString("Herb Distance_warning");
        break;
		case "HERB_QUANTITY_SLIDE":
            sHeader = XI_ConvertString("Herb Quantity");
            sText1 = XI_ConvertString("Herb Quantity_descr");
            sText2 = XI_ConvertString("Herb Distance_warning");
        break;
		case "HERB_TRANSP_SLIDE":
            sHeader = XI_ConvertString("Herb Sea Transp");
            sText1 = XI_ConvertString("Herb Sea Transp_descr");
            sText2 = XI_ConvertString("ItCanRedusePerfomance");
        break;
		case "MUSIC_SLIDE":
			sHeader = XI_ConvertString("Music Volume");
			sText1 = XI_ConvertString("Music Volume_descr");
		break;

		case "SOUND_SLIDE":
			sHeader = XI_ConvertString("Sound Volume");
			sText1 = XI_ConvertString("Sound Volume_descr");
		break;

		case "DIALOG_SLIDE":
			sHeader = XI_ConvertString("Dialog Volume");
			sText1 = XI_ConvertString("Dialog Volume_descr");
		break;

		case "HUD_SLIDE":
			sHeader = XI_ConvertString("HUD_SLIDE");
			sText1 = XI_ConvertString("HUD_SLIDE_descr");
		break;

		case "ALWAYS_RUN_CHECKBOX":
			sHeader = XI_ConvertString("Always Run");
			sText1 = XI_ConvertString("Always Run_descr");
		break;

		case "INVERT_MOUSE_CHECKBOX":
			sHeader = XI_ConvertString("Invert Vertical Mouse Control");
			sText1 = XI_ConvertString("Invert Vertical Mouse Control_descr");
		break;

		case "VMOUSE_SENSITIVITY_SLIDE":
			sHeader = XI_ConvertString("Vertical Mouse Sensitivity");
			sText1 = XI_ConvertString("Vertical Mouse Sensitivity_descr");
		break;

		case "HMOUSE_SENSITIVITY_SLIDE":
			sHeader = XI_ConvertString("Horizontal Mouse Sensitivity");
			sText1 = XI_ConvertString("Horizontal Mouse Sensitivity_descr");
		break;

		case "FLAGALLWDM_CHECKBOX":
			sHeader = XI_ConvertString("FlagAllWDM");
			sText1 = XI_ConvertString("FlagAllWDM_descr");
            sPicture = "INTERFACES\FaqPictures\FLAGALLWDM_CHECKBOX.dds";
			xx = 600;
			yy = 200;
		break;
		case "STREAMERMODE_CHECKBOX":
			sHeader = XI_ConvertString("StreamerMode");
			sText1 = XI_ConvertString("StreamerMode_descr");
		break;
		case "VISUAL_CIRASS_CHECKBOX":
			sHeader = XI_ConvertString("VISUAL_CIRASS_title");
			sText1 = XI_ConvertString("VISUAL_CIRASS_desc");
            sPicture = "INTERFACES\FaqPictures\VISUAL_CIRASS_CHECKBOX.dds";
			xx = 512;
			yy = 256;
		break;
		case "ALTFONT_CHECKBOX":
			sHeader = XI_ConvertString("AltFont_title");
			sText1 = XI_ConvertString("AltFont_desc");
            sPicture = "INTERFACES\FaqPictures\ALTFONT_CHECKBOX.dds";
			xx = 570;
			yy = 494;
		break;
		case "INTERFACE_SELECTION":
			sHeader = XI_ConvertString("INTERFACE_SELECTION");
			sText1 = XI_ConvertString("INTERFACE_SELECTION_desc");
            sPicture = "INTERFACES\FaqPictures\INTERFACE_STYLE.dds";
			xx = 494;
			yy = 494;
		break;
		case "HUD_DIALOG_BACK":
			sHeader = XI_ConvertString("Hud Dialog");
			sText1 = XI_ConvertString("Hud Dialog_descr");
            sPicture = "INTERFACES\FaqPictures\HUD_DIALOG_SLIDE.dds";
			xx = 512;
			yy = 256;
		break;
		case "PARTICLES_CHECKBOX":
			sHeader = XI_ConvertString("Particles_title");
			sText1 = XI_ConvertString("Particles_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomance");
		break;

		case "CAM_PERSP_SLIDE":
			sHeader = XI_ConvertString("cameraPersp_title");
			sText1 = XI_ConvertString("cameraPersp_desc");
		break;

		case "CAM_RAD_SLIDE":
			sHeader = XI_ConvertString("cameraRad_title");
			sText1 = XI_ConvertString("cameraRad_desc");
		break;

		case "CAM_PERSP_SLIDE2":
			sHeader = XI_ConvertString("cameraPersp2_title");
			sText1 = XI_ConvertString("cameraPersp2_desc");
		break;
		
		case "CHECK_BOARDING_SHAKE":
			sHeader = XI_ConvertString("CHECK_BOARDING_SHAKE");
			sText1 = XI_ConvertString("CHECK_BOARDING_SHAKE_desc");
		break;
		case "CHECK_BOARDING_PING":
			sHeader = XI_ConvertString("CHECK_BOARDING_PING");
			sText1 = XI_ConvertString("CHECK_BOARDING_PING_desc");
		break;

		case "CHECK_CHARCAP_SHOWOTHER":
			sHeader = XI_ConvertString("CHECK_CHARCAP_SHOWOTHER");
			sText1 = XI_ConvertString("CharCapShowOthers_desc");
		break;
		case "CHECK_CHARCAP_SHOWOFF":
			sHeader = XI_ConvertString("CHECK_CHARCAP_SHOWOFF");
			sText1 = XI_ConvertString("CharCapShowOff_desc");
		break;
		case "CHECK_CHARCAP_SHOWNOTIFICATIONS":
			sHeader = XI_ConvertString("CHECK_CHARCAP_SHOWNOTIFICATIONS");
			sText1 = XI_ConvertString("CharCapShowNotifications_desc");
		break;
		case "CHECK_CHARCAP_SHOWRANK":
			sHeader = XI_ConvertString("CHECK_CHARCAP_SHOWRANK");
			sText1 = XI_ConvertString("CharCapShowRank_desc");
		break;
		case "CHECK_CHARCAP_SHOWSPEC":
			sHeader = XI_ConvertString("CHECK_CHARCAP_SHOWSPEC");
			sText1 = XI_ConvertString("CharCapShowSpec_desc");
		break;
		case "CHECK_CHARCAP_TARGET_OFICERS":
			sHeader = XI_ConvertString("CHECK_CHARCAP_TARGET_OFICERS");
			sText1 = XI_ConvertString("CHECK_CHARCAP_TARGET_OFICERS_desc");
		break;
		case "CHECK_CHARCAP_LANDBARS":
			sHeader = XI_ConvertString("CHECK_CHARCAP_LANDBARS");
			sText1 = XI_ConvertString("CHECK_CHARCAP_LANDBARS_desc");
			sPicture = "INTERFACES\FaqPictures\HP_BAR_LAND.dds";
			xx = 256; yy = 256;
		break;
		case "CHECK_CHARCAP_QUESTMARKS": // переиспользуем заголовок/описание/картинку старой опции
			sHeader = XI_ConvertString("EnabledQuestsMarks");
			sText1 = XI_ConvertString("QuestMark Mode_descr");
			sPicture = "INTERFACES\FaqPictures\QUESTMARK_CHECKBOX.dds";
			xx = 256; yy = 256;
		break;
		case "CHECK_CHARCAP_FXMARKS":
			sHeader = XI_ConvertString("EnabledFXMarks");
			sText1 = XI_ConvertString("FXMark Mode_descr");
			sPicture = "INTERFACES\FaqPictures\FXMARK_CHECKBOX.dds";
			xx = 256; yy = 256;
		break;
		case "DIALOG_INPUT_DELAY_SLIDE":
			sHeader = XI_ConvertString("DIALOG_INPUT_DELAY_SLIDE");
			sText1 = XI_ConvertString("DialogInputDelay_desc");
		break;
		case "CHARCAP_FONT_SCALE_SLIDE":
			sHeader = XI_ConvertString("CHARCAP_FONT_SCALE_SLIDE");
			sText1 = XI_ConvertString("CharCapFontScale_desc");
		break;
		case "CHARCAP_NOTIFICATIONS_SCALE_SLIDE":
			sHeader = XI_ConvertString("CHARCAP_NOTIFICATIONS_SCALE_SLIDE");
			sText1 = XI_ConvertString("CharCapNotifScale_desc");
		break;
		case "CHARCAP_ENEMYBAR_SIZE_SLIDE":
			sHeader = XI_ConvertString("CHARCAP_ENEMYBAR_SIZE_SLIDE");
			sText1 = XI_ConvertString("EnemyBarSize_desc");
		break;
		case "CHARCAP_ENEMYBAR_DIST_SLIDE":
			sHeader = XI_ConvertString("CHARCAP_ENEMYBAR_DIST_SLIDE");
			sText1 = XI_ConvertString("EnemyBarDist_desc");
		break;
		case "CHARCAP_ENEMYBAR_MAX_SLIDE":
			sHeader = XI_ConvertString("CHARCAP_ENEMYBAR_MAX_SLIDE");
			sText1 = XI_ConvertString("EnemyBarMax_desc");
		break;
		
		// LEO: типы автоматических сейвов (чекбоксы в окне мастер-опции)
		case "CHECK_AUTOSAVE_BOARD":
			sHeader = XI_ConvertString("CHECK_AUTOSAVE_BOARD");
			sText1 = XI_ConvertString("AutoSaveBoarding_descr");
		break;
		case "CHECK_AUTOSAVE_SEA":
			sHeader = XI_ConvertString("CHECK_AUTOSAVE_SEA");
			sText1 = XI_ConvertString("AutoSaveToSea_descr");
		break;
		case "CHECK_AUTOSAVE_MAPSEA":
			sHeader = XI_ConvertString("CHECK_AUTOSAVE_MAPSEA");
			sText1 = XI_ConvertString("AutoSaveMapToSea_descr");
		break;
		case "CHECK_AUTOSAVE_LAND":
			sHeader = XI_ConvertString("CHECK_AUTOSAVE_LAND");
			sText1 = XI_ConvertString("AutoSaveToLand_descr");
		break;
		case "CHECK_AUTOSAVE_MAP":
			sHeader = XI_ConvertString("CHECK_AUTOSAVE_MAP");
			sText1 = XI_ConvertString("AutoSaveToMap_descr");
		break;

		case "SHIPINFO_BARS_LABEL":
			sHeader = GetConvertStr("ShipInfoBarsLabel", "ShipInfoBars.txt");
			sText1 = GetConvertStr("ShipInfoBarsDesc", "ShipInfoBars.txt");
		break;
		case "CHECK_SHIPCAP_SHIPNAMES1":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHIPNAMES1");
			sText1 = XI_ConvertString("ShipCapShipNames1_desc");
		break;
		case "CHECK_SHIPCAP_SHIPNAMES2":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHIPNAMES2");
			sText1 = XI_ConvertString("ShipCapShipNames2_desc");
		break;
		case "CHECK_SHIPCAP_SHOWACTIONS":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWACTIONS");
			sText1 = XI_ConvertString("ShipCapShowActions_desc");
		break;
		case "CHECK_SHIPCAP_SHOWACTIONSPLUS":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWACTIONSPLUS");
			sText1 = XI_ConvertString("ShipCapShowActionsPlus_desc");
		break;
		case "CHECK_SHIPCAP_SHOWALLIES":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWALLIES");
			sText1 = XI_ConvertString("ShipCapShowAllies_desc");
		break;
		case "CHECK_SHIPCAP_SHOWOTHER":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWOTHER");
			sText1 = XI_ConvertString("ShipCapShowOther_desc");
		break;
		case "CHECK_SHIPCAP_SHOWNOTIFICATIONS":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWNOTIFICATIONS");
			sText1 = XI_ConvertString("ShipCapShowNotifications_desc");
		break;
		case "RADIO_CONSUMABLES":
			sHeader = XI_ConvertString("consumables_config");
			sText1 = XI_ConvertString("RADIO_CONSUMABLES_desc");
		break;
		case "CHECK_CONSUMABLES_CONTEXT":
			sHeader = XI_ConvertString("ConsumablesHUDContext");
			sText1 = XI_ConvertString("ConsumablesHUDContext_desc");
		break;
		case "CHECK_SHIPCAP_SHOWCRIT":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWCRIT");
			sText1 = XI_ConvertString("ShipCapShowCrit_desc");
			sPicture = "INTERFACES\FaqPictures\Damage_Ship_Critical.dds";
			xx = 256; yy = 256;
		break;
		case "CHECK_SHIPCAP_SHOWFORTGUN":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWFORTGUN");
			sText1 = XI_ConvertString("ShipCapShowFortGun_desc");
		break;
		case "CHECK_SHIPCAP_SHOWDAMAGE":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWDAMAGE");
			sText1 = XI_ConvertString("ShipCapShowDamage_desc");
			sPicture = "INTERFACES\FaqPictures\Damage_Ship_Enemy.dds";
			xx = 256; yy = 256;
		break;
		case "CHECK_SHIPCAP_SHOWDAMAGESELF":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHOWDAMAGESELF");
			sText1 = XI_ConvertString("ShipCapShowDamageSelf_desc");
			sPicture = "INTERFACES\FaqPictures\Damage_Ship_Player.dds";
			xx = 256; yy = 256;
		break;
		case "CHECK_SHIPCAP_SHIPMARKS":
			sHeader = XI_ConvertString("EnabledShipMarks");
			sText1 = XI_ConvertString("ShipMark Mode_descr");
            sPicture = "INTERFACES\FaqPictures\SHIPMARK_CHECKBOX.dds";
			xx = 256;
			yy = 256;
		break;
		case "CHECK_SHIPCAP_SHIPMARKS_NOSELF":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHIPMARKS_NOSELF");
			sText1 = XI_ConvertString("ShipCapShipmarkNoSelf_desc");
		break;
		case "CHECK_SHIPCAP_SHIPMARKS_NOCOMPANIONS":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_SHIPMARKS_NOCOMPANIONS");
			sText1 = XI_ConvertString("ShipCapShipmarkNoCompanions_desc");
		break;
		case "CHECK_SHIPCAP_OLDBARS":
			sHeader = XI_ConvertString("CHECK_SHIPCAP_OLDBARS");
			sText1 = XI_ConvertString("CHECK_SHIPCAP_OLDBARS_desc");
		break;
		case "CHECK_SHIPCAP_HORIZBARS":
			sHeader = XI_ConvertString("ShipBarsHorizontal");
			sText1 = XI_ConvertString("ShipBarsHorizontal_descr");
		break;
		case "SHIPCAP_NOTIFICATION_DISTANCE_SLIDE":
			sHeader = XI_ConvertString("SHIPCAP_NOTIFICATION_DISTANCE_SLIDE");
			sText1 = XI_ConvertString("ShipCapNotifDistance_desc");
		break;
		case "SHIPCAP_NOTIFICATION_SCALE_SLIDE":
			sHeader = XI_ConvertString("SHIPCAP_NOTIFICATION_SCALE_SLIDE");
			sText1 = XI_ConvertString("ShipCapNotifScale_desc");
		break;
		case "SHIPCAP_NAMES_SCALE_SLIDE":
			sHeader = XI_ConvertString("SHIPCAP_NAMES_SCALE_SLIDE");
			sText1 = XI_ConvertString("ShipCapNamesScale_desc");
		break;
		case "SHIPCAP_ICONS_SCALE_SLIDE":
			sHeader = XI_ConvertString("SHIPCAP_ICONS_SCALE_SLIDE");
			sText1 = XI_ConvertString("ShipCapIconsScale_desc");
		break;
		case "PARTICLES_COUNT_SLIDE":
			sHeader = XI_ConvertString("Particles_Counts_title");
			sText1 = XI_ConvertString("Particles_Counts_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomance");
		break;
		// Gregg: растительность островов. У каждого case свой блок — составные метки Storm не берёт
		case "STR_ISLVEG_TYPE":
			sHeader = XI_ConvertString("IslVeg_title");
			sText1 = XI_ConvertString("IslVeg_Type_descr");
		break;
		case "ISLVEG_TYPE_CHECKBOX":
			sHeader = XI_ConvertString("IslVeg_title");
			sText1 = XI_ConvertString("IslVeg_Type_descr");
		break;
		case "STR_ISLVEG_DENSITY":
			sHeader = XI_ConvertString("IslVeg_Density_title");
			sText1 = XI_ConvertString("IslVeg_Density_descr");
		break;
		case "ISLVEG_DENSITY_CHECKBOX":
			sHeader = XI_ConvertString("IslVeg_Density_title");
			sText1 = XI_ConvertString("IslVeg_Density_descr");
		break;
		case "STR_ISLVEG_BAND":
			sHeader = XI_ConvertString("IslVeg_Band_title");
			sText1 = XI_ConvertString("IslVeg_Band_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomanceExt");
		break;
		case "ISLVEG_BAND_CHECKBOX":
			sHeader = XI_ConvertString("IslVeg_Band_title");
			sText1 = XI_ConvertString("IslVeg_Band_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomanceExt");
		break;
		case "PARTICLES_FIRE_CHECKBOX":
			sHeader = XI_ConvertString("Particles_Fire_title");
			sText1 = XI_ConvertString("Particles_Fire_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomanceExt");
		break;
		case "STR_PARTICLES_FIRE_PIC_SETTINGS":
			sHeader = XI_ConvertString("Particles_Fire_title");
			sText1 = XI_ConvertString("Particles_Fire_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomanceExt");
		break;
		case "PARTICLES_OTHER_CHECKBOX":
			sHeader = XI_ConvertString("Particles_Other_title");
			sText1 = XI_ConvertString("Particles_Other_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomanceExt");
		break;
		case "STR_PARTICLES_OTHER_PIC_SETTINGS":
			sHeader = XI_ConvertString("Particles_Other_title");
			sText1 = XI_ConvertString("Particles_Other_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomanceExt");
		break;
		// LEO Этап 3: легенда палитры радара (ПКМ по строке «Легенда» в окне настроек)
		case "RADAR_LEGEND":
			sHeader = XI_ConvertString("radar_legend_hdr");
			sText1 = XI_ConvertString("radar_legend_l1");
			sText2 = XI_ConvertString("radar_legend_l2");
			sText3 = XI_ConvertString("radar_legend_l3");
			sText4 = XI_ConvertString("radar_legend_l4"); // двери/переходы
		break;
		// LEO: Подсказки к подписям настроек радара.
		case "RADAR_QUALITY_TEXT":
			sHeader = XI_ConvertString("radar_lbl_quality");
			sText1 = XI_ConvertString("radar_quality_desc");
		break;
		case "RADAR_BLIP_SCALE_SLIDE":
			sHeader = XI_ConvertString("radar_lbl_blip");
			sText1 = XI_ConvertString("radar_blip_desc");
		break;
		case "CONSUMABLES_SCALE_SLIDE":
			sHeader = XI_ConvertString("consumables_lbl_scale");
			sText1 = XI_ConvertString("consumables_scale_desc");
		break;
		case "CONSUMABLES_POS_SLIDE":
			sHeader = XI_ConvertString("consumables_lbl_pos");
			sText1 = XI_ConvertString("consumables_pos_desc");
		break;
		case "RADAR_SIZE_SLIDE":
			sHeader = XI_ConvertString("radar_lbl_size");
			sText1 = XI_ConvertString("radar_size_desc");
		break;
		case "WRADAR_SIZE_SLIDE": // LEO: слайдер размера радара глобалки
			sHeader = XI_ConvertString("wradar_lbl_size");
			sText1 = XI_ConvertString("wradar_size_desc");
		break;
		case "WRADAR_ZOOM_SLIDE": // LEO: слайдер множителя охвата радара глобалки
			sHeader = XI_ConvertString("wradar_lbl_zoom");
			sText1 = XI_ConvertString("wradar_zoom_desc");
		break;
		case "WRADAR_BLIP_SLIDE": // LEO: слайдер размера объектов радара глобалки
			sHeader = XI_ConvertString("wradar_lbl_blip");
			sText1 = XI_ConvertString("wradar_blip_desc");
		break;
		case "WRADAR_QUALITY_TEXT": // LEO: качество карты радара глобалки
			sHeader = XI_ConvertString("wradar_lbl_quality");
			sText1 = XI_ConvertString("wradar_quality_desc");
		break;
		case "RADAR_MAP_SCALE_SLIDE":
			sHeader = XI_ConvertString("radar_lbl_map");
			sText1 = XI_ConvertString("radar_map_desc");
		break;
		case "RADAR_INSIDE_SCALE_SLIDE":
			sHeader = XI_ConvertString("radar_lbl_inside");
			sText1 = XI_ConvertString("radar_inside_desc");
		break;
		// LEO: Подсказки к видеонастройкам.
		case "RESOLUTION_BACK":
			sHeader = XI_ConvertString("ScreenResolution");
			sText1 = XI_ConvertString("ScreenResolution_descr");
		break;
		case "SCRIPTCACHE_BACK":
			sHeader = XI_ConvertString("ScriptCacheOpt");
			sText1 = XI_ConvertString("ScriptCacheOpt_descr");
		break;
		case "MSAA_BACK":
			sHeader = XI_ConvertString("Antialiasing");
			sText1 = XI_ConvertString("Antialiasing_descr");
		break;
		case "VSYNC_BACK":
			sHeader = XI_ConvertString("VSyncOpt");
			sText1 = XI_ConvertString("VSyncOpt_descr");
		break;
		case "SHOWFPS_BACK":
			sHeader = XI_ConvertString("ShowFpsOpt");
			sText1 = XI_ConvertString("ShowFpsOpt_descr");
		break;
		case "MAXFPS_BACK":
			sHeader = XI_ConvertString("MaxFpsOpt");
			sText1 = XI_ConvertString("MaxFpsOpt_descr");
		break;
		case "LETTERBOX_BACK":
			sHeader = XI_ConvertString("LetterboxOpt");
			sText1 = XI_ConvertString("LetterboxOpt_descr");
		break;
		case "RETAINMB_BACK":
			sHeader = XI_ConvertString("RetainMBOpt");
			sText1 = XI_ConvertString("RetainMBOpt_descr");
		break;
		case "GEORETAINMB_BACK":
			sHeader = XI_ConvertString("GeoRetainMBOpt");
			sText1 = XI_ConvertString("GeoRetainMBOpt_descr");
		break;
		case "WARMASSETS_BACK":
			sHeader = XI_ConvertString("WarmAssetsOpt");
			sText1 = XI_ConvertString("WarmAssetsOpt_descr");
		break;
		case "WARMTEXMB_BACK":
			sHeader = XI_ConvertString("WarmTexMBOpt");
			sText1 = XI_ConvertString("WarmTexMBOpt_descr");
		break;
		case "PINSKY_BACK":
			sHeader = XI_ConvertString("SkyCacheOpt");
			sText1 = XI_ConvertString("SkyCacheOpt_descr");
		break;
		case "SHADOW_BACK":
			sHeader = XI_ConvertString("ShadowOpt");
			sText1 = XI_ConvertString("ShadowOpt_descr");
		break;
		case "FLARE_BACK":
			sHeader = XI_ConvertString("FlareOpt");
			sText1 = XI_ConvertString("FlareOpt_descr");
		break;
		case "CANNONHEAT_BACK":
			sHeader = XI_ConvertString("CannonHeatHazeOpt");
			sText1 = XI_ConvertString("CannonHeatHazeOpt_descr");
		break;
		case "FLAGPHYSICS_BACK":
			sHeader = XI_ConvertString("FlagPhysicsOpt");
			sText1 = XI_ConvertString("FlagPhysicsOpt_descr");
		break;
	}

	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,255,255,255), sText4, argb(255,255,255,255), sPicture, sGroup, sGroupPicture, xx, yy);
}

void HideInfo()
{
	if( g_bToolTipStarted ) {
		g_bToolTipStarted = false;
		CloseTooltip();
	}
}

bool KeyAlreadyUsed(string sGrpName, string sControl, string sKey)
{
	if( !CheckAttribute(&objControlsState,"keygroups."+sGrpName+"."+sControl) ) {return false;}
	if( objControlsState.keygroups.(sGrpName).(sControl) == sKey ) {return false;}

	bool bAlreadyUsed = false;
	int n,q, i,grp;
	aref arGrp,arCntrl, arGrpList;

	// проверка на совпадение в той же группе
	makearef(arGrp,objControlsState.keygroups.(sGrpName));
	q = GetAttributesNum(arGrp);
	for(n=0; n<q; n++)
	{
		arCntrl = GetAttributeN(arGrp,n);
		if( GetAttributeValue(arCntrl) == sKey ) {
			bAlreadyUsed = true;
			break;
		}
	}

	if( bAlreadyUsed ) {return bAlreadyUsed;}

	// найдем группу в которой эта контролка также отображается
	makearef(arGrpList, objControlsState.keygroups);
	grp = GetAttributesNum(arGrpList);
	for( i=0; i<grp; i++ )
	{
		arGrp = GetAttributeN(arGrpList,i);
		if( !CheckAttribute(arGrp,sControl) ) {continue;}

		q = GetAttributesNum(arGrp);
		for(n=0; n<q; n++)
		{
			arCntrl = GetAttributeN(arGrp,n);
			if( GetAttributeValue(arCntrl) == sKey ) {
				bAlreadyUsed = true;
				break;
			}
		}
		if( bAlreadyUsed ) {break;}
	}

	return bAlreadyUsed;
}

void ShowCharCapWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("CHARCAP_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "CHARCAP"; // LEO: трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("CHARCAP_WINDOW", false);
		XI_WindowShow("CHARCAP_WINDOW", true);
		EI_CreateFrame("CHARCAP_WINDOW_BORDERS",190,190,610,360);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
	}
	else
	{
		XI_WindowDisable("CHARCAP_WINDOW", true);
		XI_WindowShow("CHARCAP_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

void ShowShipCapWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("SHIPCAP_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "SHIPCAP"; // LEO: трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("SHIPCAP_WINDOW", false);
		XI_WindowShow("SHIPCAP_WINDOW", true);
		EI_CreateFrame("SHIPCAP_WINDOW_BORDERS",190,190,610,360);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
	}
	else
	{
		XI_WindowDisable("SHIPCAP_WINDOW", true);
		XI_WindowShow("SHIPCAP_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

// LEO: окно тонкой настройки доп. HUD расходников (по образцу ShowShipCapWindow)
void ShowConsumablesWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("CONSUMABLES_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "CONSUMABLES"; // LEO: трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("CONSUMABLES_WINDOW", false);
		XI_WindowShow("CONSUMABLES_WINDOW", true);
		EI_CreateFrame("CONSUMABLES_WINDOW_BORDERS",210,180,590,480);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
	}
	else
	{
		XI_WindowDisable("CONSUMABLES_WINDOW", true);
		XI_WindowShow("CONSUMABLES_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

// LEO: окно "Выбор формации" — 6 круглых радио-кнопок (по образцу ShowConsumablesWindow)
void ShowFormationWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("FORMATION_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "FORMATION"; // трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("FORMATION_WINDOW", false);
		XI_WindowShow("FORMATION_WINDOW", true);
		EI_CreateFrame("FORMATION_WINDOW_BORDERS",240,180,560,400);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		// выбрать текущую формацию в радио (кнопка 1..6 = индекс формации 0..5 + 1)
		int iFormRadio = sti(InterfaceStates.FleetFormation) + 1;
		if (iFormRadio < 1) iFormRadio = 1;
		if (iFormRadio > 6) iFormRadio = 6;
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"RADIO_FORMATION", 2, iFormRadio, true );
	}
	else
	{
		XI_WindowDisable("FORMATION_WINDOW", true);
		XI_WindowShow("FORMATION_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

// Борода: окно настройки множителей ускорения времени (по образцу ShowDialogDelayWindow)
void ShowTimeAccelWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("TIME_ACCEL_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "TIMEACCEL"; // трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("TIME_ACCEL_WINDOW", false);
		XI_WindowShow("TIME_ACCEL_WINDOW", true);
		EI_CreateFrame("TIME_ACCEL_WINDOW_BORDERS",190,150,610,400);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		SetFormatedText("TIME_ACCEL_SEABATTLE_TEXT", "x"+its(sti(InterfaceStates.TimeAccelSeaBattle)));
		SetFormatedText("TIME_ACCEL_SEA_TEXT", "x"+its(sti(InterfaceStates.TimeAccelSea)));
		SetFormatedText("TIME_ACCEL_LAND_TEXT", "x"+its(sti(InterfaceStates.TimeAccelLand)));
	}
	else
	{
		XI_WindowDisable("TIME_ACCEL_WINDOW", true);
		XI_WindowShow("TIME_ACCEL_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

// LEO: окно «Автоматические сейвы» - сколько слотов в кольце у каждого типа (1..10).
//   Сами типы вкл/выкл чекбоксами в левой таблице, мастер-тумблер - строка этого окна в правой.
void ShowAutoSaveWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("AUTOSAVE_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "AUTOSAVE"; // трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("AUTOSAVE_WINDOW", false);
		XI_WindowShow("AUTOSAVE_WINDOW", true);
		EI_CreateFrame("AUTOSAVE_WINDOW_BORDERS",190,150,610,430);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		// LEO: галки типов - выставить по текущему состоянию (2 = «поставить значение чекбокса»)
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_AUTOSAVE_BOARD",  2, 1, sti(InterfaceStates.AutoSaveBoarding));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_AUTOSAVE_SEA",    2, 1, sti(InterfaceStates.AutoSaveToSea));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_AUTOSAVE_MAPSEA", 2, 1, sti(InterfaceStates.AutoSaveMapToSea));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_AUTOSAVE_LAND",   2, 1, sti(InterfaceStates.AutoSaveToLand));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_AUTOSAVE_MAP",    2, 1, sti(InterfaceStates.AutoSaveToMap));
		SetFormatedText("AUTOSAVE_BOARD_TEXT",  its(AutoSave_SlotsQty("Board")));
		SetFormatedText("AUTOSAVE_SEA_TEXT",    its(AutoSave_SlotsQty("Sea")));
		SetFormatedText("AUTOSAVE_MAPSEA_TEXT", its(AutoSave_SlotsQty("MapSea")));
		SetFormatedText("AUTOSAVE_LAND_TEXT",   its(AutoSave_SlotsQty("Land")));
		SetFormatedText("AUTOSAVE_MAP_TEXT",    its(AutoSave_SlotsQty("Map")));
	}
	else
	{
		XI_WindowDisable("AUTOSAVE_WINDOW", true);
		XI_WindowShow("AUTOSAVE_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

// LEO: слайдер числа слотов -> атрибут + подпись. sl всегда 0.0..1.0, шкала 1..10 (pitchCounter=9 в ini).
// LEO: GameInterface.nodes хранит имена в нижнем регистре; sSlideNodeLow уже нормализован.
// LEO: В Storm нет strlwr; используем строчные имена узлов, как в TimeAccel.
void ChangeAutoSaveSlots(string sKey, string sSlideNodeLow, string sTextNode)
{
	float sl = stf(GameInterface.nodes.(sSlideNodeLow).value);
	int iVal = 1 + makeint(sl * 9.0 + 0.5);
	if (iVal < 1) iVal = 1;
	if (iVal > 10) iVal = 10;
	string sAttr = "AutoSaveSlots" + sKey;
	InterfaceStates.(sAttr) = iVal;
	SetFormatedText(sTextNode, its(iVal));
}

// LEO: окно тонкой настройки задержки ввода диалога (по образцу ShowConsumablesWindow)
void ShowDialogDelayWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("DIALOG_DELAY_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "DIALOGDELAY"; // LEO: трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("DIALOG_DELAY_WINDOW", false);
		XI_WindowShow("DIALOG_DELAY_WINDOW", true);
		EI_CreateFrame("DIALOG_DELAY_WINDOW_BORDERS",190,200,610,345);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		// LEO: Обновляем только текст: повторная инициализация слайдера в видимом окне вызывает eSlideChange.
		SetFormatedText("DIALOG_INPUT_DELAY_TEXT", its(makeint(stf(InterfaceStates.DialogInputDelay)*1000)) + " ms");
	}
	else
	{
		XI_WindowDisable("DIALOG_DELAY_WINDOW", true);
		XI_WindowShow("DIALOG_DELAY_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

// LEO: окно настройки «Сигналы абордажников» — под-чекбоксы тряски и пинга (по образцу ShowConsumablesWindow)
void ShowBoardingSignalsWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("BOARDING_SIGNALS_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "BOARDINGSIGNALS"; // LEO: трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("BOARDING_SIGNALS_WINDOW", false);
		XI_WindowShow("BOARDING_SIGNALS_WINDOW", true);
		EI_CreateFrame("BOARDING_SIGNALS_WINDOW_BORDERS",240,200,560,345);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		// LEO: проставить состояние галочек под-опций при открытии окна
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_BOARDING_SHAKE", 2, 1, sti(InterfaceStates.PortraitShakeEnable));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_BOARDING_PING", 2, 1, sti(InterfaceStates.PortraitPingEnable));
	}
	else
	{
		XI_WindowDisable("BOARDING_SIGNALS_WINDOW", true);
		XI_WindowShow("BOARDING_SIGNALS_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

// LEO: Спрайты OptionalMods выбираются через SetProgressAnimation со следующей загрузки.
// LEO: У всех сетка 8x8; первый кадр превью занимает UV 0..0.125.
string PanimFile(int n)
{
	if(n == 1) return "Loading\progress_Artefact.dds";
	if(n == 2) return "Loading\progress_AztecCoins.dds";
	if(n == 3) return "Loading\progress_Cannon.dds";
	if(n == 4) return "Loading\progress_Compas.dds";
	if(n == 5) return "Loading\progress_EMPBomb.dds";
	if(n == 6) return "Loading\progress_Knipels.dds";
	if(n == 7) return "Loading\progress_QuarkBomb.dds";
	if(n == 8) return "Loading\progress_SoleyRoyal.dds";
	if(n == 9) return "Loading\progress_StanBomb.dds";
	if(n == 10) return "Loading\progress_Steering.dds";
	if(n == 11) return "Loading\progress_Watch.dds";
	return "Loading\progress.dds";
}
string PanimName(int n)
{
	if(n == 1) return XI_ConvertString("PanimArtefact");
	if(n == 2) return XI_ConvertString("PanimAztec");
	if(n == 3) return XI_ConvertString("PanimCannon");
	if(n == 4) return XI_ConvertString("PanimCompas");
	if(n == 5) return XI_ConvertString("PanimEMP");
	if(n == 6) return XI_ConvertString("PanimKnipels");
	if(n == 7) return XI_ConvertString("PanimQuark");
	if(n == 8) return XI_ConvertString("PanimSoley");
	if(n == 9) return XI_ConvertString("PanimStan");
	if(n == 10) return XI_ConvertString("PanimSteering");
	if(n == 11) return XI_ConvertString("PanimWatch");
	return XI_ConvertString("PanimDefault");
}
int PanimCurrent()
{
	string sCur = "";
	if(CheckAttribute(&InterfaceStates,"ProgressAnimFile")) sCur = InterfaceStates.ProgressAnimFile;
	for(int i = 0; i < PANIM_QUANTITY; i++)
	{
		if(PanimFile(i) == sCur) return i;
	}
	return 0;
}

void PanimSetPreview(int n)
{
	SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, "PANIM_PREVIEW", 2, 0, PanimFile(n));
	// первый кадр сетки 8x8; порядок аргументов у PICTURE - left, right, top, bottom
	SendMessage(&GameInterface, "lslffff", MSG_INTERFACE_MSG_TO_NODE, "PANIM_PREVIEW", 1, 0.0, 0.125, 0.0, 0.125);
}

void FillPanimList()
{
	string srow;
	aref row;
	int nCur = PanimCurrent();
	for(int i = 0; i < PANIM_QUANTITY; i++)
	{
		srow = "tr" + (i+1);
		makearef(row, GameInterface.PANIM_LIST.(srow));
		row.hidden = false;
		row.td1.str = PanimName(i);
		if(i == nCur) row.td1.color = argb(255,255,255,255);
		else          row.td1.color = argb(255,180,180,180);
	}
	GameInterface.PANIM_LIST.select = nCur + 1;
	Table_UpdateWindow("PANIM_LIST");
	PanimSetPreview(nCur);
}

// клик по строке: сразу показываем кадр и применяем выбор
void ProcPanimSelect()
{
	int nSel = sti(GameInterface.PANIM_LIST.select) - 1;
	if(nSel < 0) return;
	if(nSel >= PANIM_QUANTITY) return;

	InterfaceStates.ProgressAnimFile = PanimFile(nSel);
	SetProgressAnimation(PanimFile(nSel));
	FillPanimList();
}

void ShowProgressAnimWindow(bool show)
{
	if(show)
	{
		SetCurrentNode("PANIM_CLOSE");
		InterfaceStates.OptSubWin = "PANIM";
		XI_WindowDisable("PANIM_WINDOW", false);
		XI_WindowShow("PANIM_WINDOW", true);
		EI_CreateFrame("PANIM_WINDOW_BORDERS",150,110,650,505);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		SetFormatedText("PANIM_CAPTION", XI_ConvertString("ProgressAnimation"));
		FillPanimList();
	}
	else
	{
		DeleteAttribute(&InterfaceStates,"OptSubWin");
		XI_WindowDisable("PANIM_WINDOW", true);
		XI_WindowShow("PANIM_WINDOW", false);
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

void ShowRadarWindow(bool show)
{
	if (show)
	{
		SetCurrentNode("RADAR_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "RADAR"; // LEO: трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("RADAR_WINDOW", false);
		XI_WindowShow("RADAR_WINDOW", true);
		EI_CreateFrame("RADAR_WINDOW_BORDERS",150,110,650,505);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		// LEO Этап 3: проставить галочки групп из флагов RadarShow* (деф 1=показывать)
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_CITIZEN", 2, 1, LandRadar_GetShow("Citizen"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_PATROL", 2, 1, LandRadar_GetShow("Patrol"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_MUSKET", 2, 1, LandRadar_GetShow("Musket"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_ALLY", 2, 1, LandRadar_GetShow("Ally"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_SKELETON", 2, 1, LandRadar_GetShow("Skeleton"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_QUEST", 2, 1, LandRadar_GetShow("Quest"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_CALLY", 2, 1, LandRadar_GetShow("CombatAlly"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_CENEMY", 2, 1, LandRadar_GetShow("CombatEnemy"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_DEAD", 2, 1, LandRadar_GetShow("Dead"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_ITEM", 2, 1, LandRadar_GetShow("Item"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_CHEST", 2, 1, LandRadar_GetShow("Chest"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_HERB", 2, 1, LandRadar_GetShow("Herb"));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_DOORS", 2, 1, LandRadar_GetShow("Doors"));
		if( !CheckAttribute(&InterfaceStates,"RadarShape") ) InterfaceStates.RadarShape = 0;  // 0=квадрат, 1=круг
		if( !CheckAttribute(&InterfaceStates,"RadarRotate") ) InterfaceStates.RadarRotate = 1; // вращать по компасу
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_SHAPE", 2, 1, sti(InterfaceStates.RadarShape));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RADAR_ROTATE", 2, 1, sti(InterfaceStates.RadarRotate));
		// LEO Этап 3: тексты значений (слайдеры/качество) — ставим ТОЛЬКО текст при открытии (слайдер не дёргаем, его позиция выставлена в init-блоке экрана)
		SetFormatedText("RADAR_BLIP_SCALE_TEXT", FloatToString(stf(InterfaceStates.RadarBlipScale), 1));
		SetFormatedText("RADAR_MAP_SCALE_TEXT", its(makeint(stf(InterfaceStates.RadarRange))));
		SetFormatedText("RADAR_INSIDE_SCALE_TEXT", its(makeint(stf(InterfaceStates.RadarRangeInside))));
		SetFormatedText("RADAR_QUALITY_TEXT", its(sti(InterfaceStates.RadarMapRes)));
		SetFormatedText("RADAR_SIZE_TEXT", its(makeint(stf(InterfaceStates.RadarRadius))));
	}
	else
	{
		XI_WindowDisable("RADAR_WINDOW", true);
		XI_WindowShow("RADAR_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

// LEO: окно тонкой настройки радара ГЛОБАЛЬНОЙ карты (мастер-опция «Радар (глобальная карта)», дабл-клик)
void ShowWorldRadarWindow(bool show)
{
	if (show)
	{
		WorldRadar_InitDefaults();
		SetCurrentNode("WRADAR_WINDOW_CLOSE");
		InterfaceStates.OptSubWin = "WORLDRADAR"; // трекаем открытое под-окно (закрытие по Esc)
		XI_WindowDisable("WORLDRADAR_WINDOW", false);
		XI_WindowShow("WORLDRADAR_WINDOW", true);
		EI_CreateFrame("WRADAR_WINDOW_BORDERS",175,120,625,340);
		XI_WindowDisable("OTHERS_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		// галочки фильтров/формы из флагов (деф показывать)
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_WRADAR_ENC", 2, 1, sti(InterfaceStates.WorldRadarShowEnc));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_WRADAR_STORM", 2, 1, sti(InterfaceStates.WorldRadarShowStorm));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_WRADAR_FORTS", 2, 1, sti(InterfaceStates.WorldRadarShowForts));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_WRADAR_SHAPE", 2, 1, sti(InterfaceStates.WorldRadarShape));
		// тексты значений слайдеров/качества (сами слайдеры спозиционированы в init-блоке экрана — тут только текст)
		SetFormatedText("WRADAR_SIZE_TEXT", its(makeint(stf(InterfaceStates.WorldRadarSize))));
		SetFormatedText("WRADAR_ZOOM_TEXT", FloatToString(stf(InterfaceStates.WorldRadarRange), 2) + "x");
		SetFormatedText("WRADAR_BLIP_TEXT", FloatToString(stf(InterfaceStates.WorldRadarBlip), 1));
		SetFormatedText("WRADAR_QUALITY_TEXT", its(sti(InterfaceStates.WorldRadarMapRes)));
	}
	else
	{
		XI_WindowDisable("WORLDRADAR_WINDOW", true);
		XI_WindowShow("WORLDRADAR_WINDOW", false);
		DeleteAttribute(&InterfaceStates, "OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
	}
}

void SetKeyChooseWarning( string sWarningText )
{
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 10, 4, &sWarningText );
	SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 5 );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 8, 4, argb(255,255,64,64) );
	PostEvent("evFaderFrame",700,"lll",3000,0,50);
}

void FaderFrame()
{
	int nTotalTime = GetEventData();
	int nCurTime = GetEventData();
	int nDeltaTime = GetEventData();

	nCurTime = nCurTime + nDeltaTime;
	if( nCurTime>nTotalTime ) {nCurTime=nTotalTime;}

	int nAlpha = 255*(nTotalTime-nCurTime) / nTotalTime;
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 8, 4, argb(nAlpha,255,64,64) );

	if( nCurTime<nTotalTime ) {
		PostEvent("evFaderFrame",nDeltaTime,"lll",nTotalTime,nCurTime,nDeltaTime);
	}
}

void ChangeHUDDetail()
{
    float sl = stf(GameInterface.nodes.hud_slide.value);
	newBase = CalcHUDBase(sl, stf(Render.screen_y));
	fHUDRatio = stf(Render.screen_y) / newBase;
	SetFormatedText("HUD_DESCRIP_TEXT", Render.screen_y + "  / " + round(newBase) + " : " + roundFloat(fHUDRatio));
	if (roundFloat(fHUDRatio) != roundFloat(stf(Render.screen_y) / 900.0)) SetSelectable("HUD_SCALE_DEFAULT",true);
	else SetSelectable("HUD_SCALE_DEFAULT",false);
}

void ChangeDialogStyle(bool way)
{
	int value = sti(InterfaceStates.DialogStyle);
	if (way)
	{
		value++;
	}
	else
	{
		value--;
	}
	if (value < 1) value = 5;
	else if (value > 5) value = 1;
	InterfaceStates.DialogStyle = its(value);
	SetFormatedText("HUD_DIALOG_TEXT", InterfaceStates.DialogStyle);
}

// LEO: сброс блока «Растительность» к рекомендованным значениям. Слайдеры хранят долю 0..1,
//   а подпись рядом считается по своей шкале: суша-дальность x500, деревья-дальность x4000,
//   оба «количества» x100. Меняешь шкалу в Change*() - меняй и числа здесь.
void RestoreDefaultVegetation()
{
	GameInterface.nodes.HERB_LAND_DISTANCE_SLIDE.value = 0.2; // 100
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HERB_LAND_DISTANCE_SLIDE", 0, stf(GameInterface.nodes.HERB_LAND_DISTANCE_SLIDE.value));
	ChangeGrassDistanceLand();
	GameInterface.nodes.HERB_QUANTITY_SLIDE.value = 1.0; // 100
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HERB_QUANTITY_SLIDE", 0, stf(GameInterface.nodes.HERB_QUANTITY_SLIDE.value));
	ChangeGrassCountLand();
	GameInterface.nodes.HERB_SEA_DISTANCE_SLIDE.value = 0.5; // 2000
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HERB_SEA_DISTANCE_SLIDE", 0, stf(GameInterface.nodes.HERB_SEA_DISTANCE_SLIDE.value));
	ChangeGrassDistanceSea();
	GameInterface.nodes.HERB_TRANSP_SLIDE.value = 1.0; // 100
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE, "HERB_TRANSP_SLIDE", 0, stf(GameInterface.nodes.HERB_TRANSP_SLIDE.value));
	ChangeGrassCountSea();
	InterfaceStates.IslandVegType = 0;    // спрайты
	InterfaceStates.IslandVegDensity = 2; // высокая
	InterfaceStates.IslandVegBand = 3;    // очень высоко
	GetIslVegOptionsData(); // расставить переключатели по атрибутам
	ApplyIslandVegetation();
}

void ChangeGrassDistanceLand()
{
    float gds = stf(GameInterface.nodes.HERB_LAND_DISTANCE_SLIDE.value)
	float maxd = gds*500.0;
	InterfaceStates.GrassDistanceLand = gds;
	SetFormatedText("HERB_LAND_DISTANCE_TEXT", ""+makeint(maxd));
}

void ChangeGrassDistanceSea()
{
    float gds = stf(GameInterface.nodes.HERB_SEA_DISTANCE_SLIDE.value)
	float maxd = gds*4000.0;
	InterfaceStates.GrassDistanceSea = gds;
	SetFormatedText("HERB_SEA_DISTANCE_TEXT", ""+makeint(maxd));
	ApplyIslandVegetation(); // Gregg: применяем живьём, без перезахода в локацию
}

void ChangeGrassCountLand()
{
    float gds = stf(GameInterface.nodes.HERB_QUANTITY_SLIDE.value)
	float maxd = gds*100;
	InterfaceStates.GrassCountLand = gds;
	SetFormatedText("HERB_QUANTITY_TEXT", ""+makeint(maxd));
}

void ChangeGrassCountSea()
{
    float gds = stf(GameInterface.nodes.HERB_TRANSP_SLIDE.value)
	float maxd = gds*100;
	InterfaceStates.GrassCountSea = gds;
	SetFormatedText("HERB_TRANSP_TEXT", ""+makeint(maxd));
	ApplyIslandVegetation(); // Gregg: применяем живьём, без перезахода в локацию
}

void ChangeCharCap()
{
	float FontScale = stf(GameInterface.nodes.CHARCAP_FONT_SCALE_SLIDE.value);
	float NotificationsScale = stf(GameInterface.nodes.CHARCAP_NOTIFICATIONS_SCALE_SLIDE.value);
	InterfaceStates.CharCapFontScale = FontScale;
	InterfaceStates.CharCapNotifScale = NotificationsScale;
	SetFormatedText("CHARCAP_FONT_SCALE_DESCRIP_TEXT", FloatToString(GetCharCapFontScale(),2));
	SetFormatedText("CHARCAP_NOTIFICATIONS_SCALE_DESCRIP_TEXT", FloatToString(GetCharCapNotifScale(),2));
	// LEO: бары ХП/энергии над врагами на суше — 3 слайдера (размер/дальность/кол-во)
	InterfaceStates.EnemyBarSize = stf(GameInterface.nodes.CHARCAP_ENEMYBAR_SIZE_SLIDE.value);
	InterfaceStates.EnemyBarDist = stf(GameInterface.nodes.CHARCAP_ENEMYBAR_DIST_SLIDE.value);
	InterfaceStates.EnemyBarMax  = stf(GameInterface.nodes.CHARCAP_ENEMYBAR_MAX_SLIDE.value);
	SetFormatedText("CHARCAP_ENEMYBAR_SIZE_DESCRIP_TEXT", FloatToString(GetEnemyBarSize(),1));
	SetFormatedText("CHARCAP_ENEMYBAR_DIST_DESCRIP_TEXT", ""+makeint(GetEnemyBarDist()));
	SetFormatedText("CHARCAP_ENEMYBAR_MAX_DESCRIP_TEXT", ""+GetEnemyBarMax());
	LandBars_ApplyParamsLive(); // живое пере-применение размера/дальности/лимита в текущей локации (гард внутри)
}

void ChangeShipCap()
{
	float NotificationDistance = stf(GameInterface.nodes.SHIPCAP_NOTIFICATION_DISTANCE_SLIDE.value);
	float NotificationScale = stf(GameInterface.nodes.SHIPCAP_NOTIFICATION_SCALE_SLIDE.value);
	float NamesScale = stf(GameInterface.nodes.SHIPCAP_NAMES_SCALE_SLIDE.value);
	float IconsScale = stf(GameInterface.nodes.SHIPCAP_ICONS_SCALE_SLIDE.value);
	InterfaceStates.ShipCapNotifDistance = NotificationDistance;
	InterfaceStates.ShipCapNotifScale = NotificationScale;
	InterfaceStates.ShipCapNamesScale = NamesScale;
	InterfaceStates.ShipCapIconsScale = IconsScale;
	SetFormatedText("SHIPCAP_NOTIFICATION_DISTANCE_DESCRIP_TEXT", ""+makeint(GetShipCapNotifDistance()));
	SetFormatedText("SHIPCAP_NOTIFICATION_SCALE_DESCRIP_TEXT", FloatToString(GetShipCapNotifScale(),2));
	SetFormatedText("SHIPCAP_NAMES_SCALE_DESCRIP_TEXT", FloatToString(GetShipCapNamesScale(),2));
	SetFormatedText("SHIPCAP_ICONS_SCALE_DESCRIP_TEXT", FloatToString(GetShipCapIconsScale(),2));
}

void ChangeParticles()
{
	float part = stf(GameInterface.nodes.PARTICLES_COUNT_SLIDE.value);
	InterfaceStates.ParticlesCount = part;
	// LEO: потолок поднят до 200000 (движок: MAX_BILLBOARDS + 32-битные индексы) под массовые бои
	int maxP = 4000 + (makeint(part * 196000));
	if(IsEntity(&Particles)) Particles.max = maxP;
	SetFormatedText("PARTICLES_COUNT_TEXT", "" + maxP);
}
