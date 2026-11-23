
void OSL_WriteGameOption()
{
	object gopt;
	PrepareDefaultOption(&gopt);
	GetRealOptions(&gopt);
	SaveSavedOptions(&gopt);
}

void OSL_ReadGameOption()
{
	object gopt;
	PrepareDefaultOption(&gopt);
	ReadSavedOptions(&gopt);
	SetCurentOptions(&gopt);
	// LEO: Первый options получает ControlsInit; существующие привязки не перезаписываем.
	if (!CheckAttribute(&gopt, "controls"))
	{
		OSL_WriteGameOption();
	}
}

void PrepareDefaultOption(ref optref)
{
	optref.volume.music = 0.5;
	optref.volume.sound = 0.5;
	optref.volume.dialog = 0.5;
	optref.cameramode.follow_on = true;
	optref.alwaysrun = true;
	optref.video.grassquantity = 0;
	optref.video.FlagAllWdm = 2;
	optref.seadetails = 1.0;
    optref.perspdetails = 0.0;
    optref.persp2details = 0.0;
    optref.raddetails = 0.0;

	SeaParametrs.MaxVertices = MaxVertices;
	SeaParametrs.MaxIndices = MaxIndices;
	SeaParametrs.GridStepX = GridStepX;
	SeaParametrs.GridStepPC = GridStepPC;
	SeaParametrs.LodScale = LodScale;
	SeaParametrs.MaxWaveDistance = MaxWaveDistance;
}

void GetRealOptions(ref optref)
{
	ShipInfoBars_SaveOptions(optref); // LEO: Отдельный режим полос, без изменения старых настроек подписей.
	float ftmp1,ftmp2,ftmp3;

	ftmp1 = stf(optref.volume.sound);
	ftmp2 = stf(optref.volume.music);
	ftmp3 = stf(optref.volume.dialog);
	SendMessage(&sound,"leee", MSG_SOUND_GET_MASTER_VOLUME, &ftmp1, &ftmp2,	&ftmp3 );
	optref.volume.sound = ftmp1;
	optref.volume.music = ftmp2;
	optref.volume.dialog = ftmp3;


	optref.cameramode.follow_on = !locCameraEnableSpecialMode;

	optref.video.grassquantity = iGrassQuality;
	optref.video.FlagAllWdm = iFlagAllWdm;
	if( CheckAttribute(&InterfaceStates,"SeaDetails") ) {
		optref.seadetails = stf(InterfaceStates.SeaDetails);
	}
	if( CheckAttribute(&InterfaceStates,"GrassDistanceLand") ) {
		optref.GrassDistanceLand = stf(InterfaceStates.GrassDistanceLand);
		UpdateLandGrass();
	}
	if( CheckAttribute(&InterfaceStates,"GrassDistanceSea") ) {
		optref.GrassDistanceSea = stf(InterfaceStates.GrassDistanceSea);
		UpdateSeaGrass();
	}
	if( CheckAttribute(&InterfaceStates,"GrassCountLand") ) {
		optref.GrassCountLand = stf(InterfaceStates.GrassCountLand);
		UpdateLandGrass();
	}
	if( CheckAttribute(&InterfaceStates,"GrassCountSea") ) {
		optref.GrassCountSea = stf(InterfaceStates.GrassCountSea);
		UpdateSeaGrass();
	}
	if( CheckAttribute(&InterfaceStates,"IslandVegType") ) {
		optref.IslandVegType = sti(InterfaceStates.IslandVegType);
	}	else {
		optref.IslandVegType = 1;
	}
	if( CheckAttribute(&InterfaceStates,"IslandVegDensity") ) {
		optref.IslandVegDensity = sti(InterfaceStates.IslandVegDensity);
	}	else {
		optref.IslandVegDensity = 1;
	}
	if( CheckAttribute(&InterfaceStates,"IslandVegBand") ) {
		optref.IslandVegBand = sti(InterfaceStates.IslandVegBand);
	}	else {
		optref.IslandVegBand = 1;
	}
	if( CheckAttribute(&InterfaceStates,"PerspDetails") ) {
		optref.perspdetails = stf(InterfaceStates.PerspDetails);
	}
	if( CheckAttribute(&InterfaceStates,"Persp2Details") ) {
		optref.persp2details = stf(InterfaceStates.Persp2Details);
	}
	if( CheckAttribute(&InterfaceStates,"RadDetails") ) {
		optref.raddetails = stf(InterfaceStates.RadDetails);
	}
	if( CheckAttribute(&InterfaceStates,"DialogStyle") ) {
		optref.DialogStyle = sti(InterfaceStates.DialogStyle);
	}
	if( CheckAttribute(&InterfaceStates,"InvertCameras") ) {
		optref.cameramode.InvertCameras = sti(InterfaceStates.InvertCameras);
	} else {
		optref.cameramode.InvertCameras = false;
	}

	if( CheckAttribute(&InterfaceStates,"ShowBattleMode") ) {
		optref.cameramode.ShowBattleMode = sti(InterfaceStates.ShowBattleMode);
	} else {
		optref.cameramode.ShowBattleMode = false;
	}

	if( CheckAttribute(&InterfaceStates,"EnabledAutoSaveMode") ) {
		optref.cameramode.EnabledAutoSaveMode = sti(InterfaceStates.EnabledAutoSaveMode);
	} else {
		optref.cameramode.EnabledAutoSaveMode = 1;
	}

	if( CheckAttribute(&InterfaceStates,"EnabledQuestsMarks") ) {
		optref.cameramode.EnabledQuestsMarks = sti(InterfaceStates.EnabledQuestsMarks);
	} else {
		optref.cameramode.EnabledQuestsMarks = true;
	}

	if( CheckAttribute(&InterfaceStates,"EnabledFXMarks") ) {
		optref.cameramode.EnabledFXMarks = sti(InterfaceStates.EnabledFXMarks);
	} else {
		optref.cameramode.EnabledFXMarks = true;
	}

	if( CheckAttribute(&InterfaceStates,"EnabledCMControls") ) {
		optref.cameramode.EnabledCMControls = sti(InterfaceStates.EnabledCMControls);
	} else {
		optref.cameramode.EnabledCMControls = true;
	}

	if( CheckAttribute(&InterfaceStates,"EnabledOldStore") ) {
		optref.cameramode.EnabledOldStore = sti(InterfaceStates.EnabledOldStore);
	} else {
		optref.cameramode.EnabledOldStore = false;
	}

	if( CheckAttribute(&InterfaceStates,"EnabledAltSoundsGun") ) {
		optref.sound.EnabledAltSoundsGun = sti(InterfaceStates.EnabledAltSoundsGun);
	} else {
		optref.sound.EnabledAltSoundsGun = false;
	}

	if( CheckAttribute(&InterfaceStates,"DiscordRPC") ) {
		optref.game.DiscordRPC = sti(InterfaceStates.DiscordRPC);
	} else {
		optref.game.DiscordRPC = true;
	}
	SwitchRPC(optref.game.DiscordRPC);

	if( CheckAttribute(&InterfaceStates,"StreamerMode") ) {
		optref.sound.StreamerMode = sti(InterfaceStates.StreamerMode);
	} else {
		optref.sound.StreamerMode = false;
	}
	
	if( CheckAttribute(&InterfaceStates,"ShowExpLogs") ) {
		optref.cameramode.ShowExpLogs = sti(InterfaceStates.ShowExpLogs);
	} else {
		optref.cameramode.ShowExpLogs = false;
	}

	if( CheckAttribute(&InterfaceStates,"ShowGameLog") ) {
		optref.cameramode.ShowGameLog = sti(InterfaceStates.ShowGameLog);
	} else {
		optref.cameramode.ShowGameLog = 1; // LEO: Храним int: преобразование sti("true") даёт 0 и отключает журнал.
	}

	if( CheckAttribute(&InterfaceStates,"ConsumablesHUD") ) {
		optref.cameramode.ConsumablesHUD = sti(InterfaceStates.ConsumablesHUD);
	} else {
		optref.cameramode.ConsumablesHUD = false;
	}
	if( CheckAttribute(&InterfaceStates,"BoardingSignals") ) {
		optref.cameramode.BoardingSignals = sti(InterfaceStates.BoardingSignals);
	} else {
		optref.cameramode.BoardingSignals = 1;
	}
	if( CheckAttribute(&InterfaceStates,"PortraitShakeEnable") ) {
		optref.cameramode.PortraitShakeEnable = sti(InterfaceStates.PortraitShakeEnable);
	} else {
		optref.cameramode.PortraitShakeEnable = 1;
	}
	if( CheckAttribute(&InterfaceStates,"PortraitPingEnable") ) {
		optref.cameramode.PortraitPingEnable = sti(InterfaceStates.PortraitPingEnable);
	} else {
		optref.cameramode.PortraitPingEnable = 1;
	}
	// LEO: Сохраняем включение радара; видимость по горячей клавише не сохраняется.
	if( CheckAttribute(&InterfaceStates,"RadarEnabled") ) {
		optref.cameramode.RadarEnabled = sti(InterfaceStates.RadarEnabled);
	} else {
		optref.cameramode.RadarEnabled = 0;
	}
	if( CheckAttribute(&InterfaceStates,"RadarBlipScale") ) optref.cameramode.RadarBlipScale = stf(InterfaceStates.RadarBlipScale); else optref.cameramode.RadarBlipScale = 1.0;
	if( CheckAttribute(&InterfaceStates,"RadarRange") ) optref.cameramode.RadarRange = stf(InterfaceStates.RadarRange); else optref.cameramode.RadarRange = 60.0;
	if( CheckAttribute(&InterfaceStates,"RadarRangeInside") ) optref.cameramode.RadarRangeInside = stf(InterfaceStates.RadarRangeInside); else optref.cameramode.RadarRangeInside = 10.0;
	if( CheckAttribute(&InterfaceStates,"RadarMapRes") ) optref.cameramode.RadarMapRes = sti(InterfaceStates.RadarMapRes); else optref.cameramode.RadarMapRes = 2048;
	if( CheckAttribute(&InterfaceStates,"RadarRadius") ) optref.cameramode.RadarRadius = stf(InterfaceStates.RadarRadius); else optref.cameramode.RadarRadius = 210.0;
	if( CheckAttribute(&InterfaceStates,"RadarShowCitizen") ) optref.cameramode.RadarShowCitizen = sti(InterfaceStates.RadarShowCitizen); else optref.cameramode.RadarShowCitizen = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowPatrol") ) optref.cameramode.RadarShowPatrol = sti(InterfaceStates.RadarShowPatrol); else optref.cameramode.RadarShowPatrol = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowMusket") ) optref.cameramode.RadarShowMusket = sti(InterfaceStates.RadarShowMusket); else optref.cameramode.RadarShowMusket = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowAlly") ) optref.cameramode.RadarShowAlly = sti(InterfaceStates.RadarShowAlly); else optref.cameramode.RadarShowAlly = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowSkeleton") ) optref.cameramode.RadarShowSkeleton = sti(InterfaceStates.RadarShowSkeleton); else optref.cameramode.RadarShowSkeleton = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowQuest") ) optref.cameramode.RadarShowQuest = sti(InterfaceStates.RadarShowQuest); else optref.cameramode.RadarShowQuest = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowCombatAlly") ) optref.cameramode.RadarShowCombatAlly = sti(InterfaceStates.RadarShowCombatAlly); else optref.cameramode.RadarShowCombatAlly = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowCombatEnemy") ) optref.cameramode.RadarShowCombatEnemy = sti(InterfaceStates.RadarShowCombatEnemy); else optref.cameramode.RadarShowCombatEnemy = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowDead") ) optref.cameramode.RadarShowDead = sti(InterfaceStates.RadarShowDead); else optref.cameramode.RadarShowDead = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowItem") ) optref.cameramode.RadarShowItem = sti(InterfaceStates.RadarShowItem); else optref.cameramode.RadarShowItem = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowChest") ) optref.cameramode.RadarShowChest = sti(InterfaceStates.RadarShowChest); else optref.cameramode.RadarShowChest = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowHerb") ) optref.cameramode.RadarShowHerb = sti(InterfaceStates.RadarShowHerb); else optref.cameramode.RadarShowHerb = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShowDoors") ) optref.cameramode.RadarShowDoors = sti(InterfaceStates.RadarShowDoors); else optref.cameramode.RadarShowDoors = 1;
	if( CheckAttribute(&InterfaceStates,"RadarShape") ) optref.cameramode.RadarShape = sti(InterfaceStates.RadarShape); else optref.cameramode.RadarShape = 0;
	if( CheckAttribute(&InterfaceStates,"RadarRotate") ) optref.cameramode.RadarRotate = sti(InterfaceStates.RadarRotate); else optref.cameramode.RadarRotate = 1;
	if( CheckAttribute(&InterfaceStates,"ConsumablesNoTop") ) {
		optref.cameramode.ConsumablesNoTop = sti(InterfaceStates.ConsumablesNoTop);
	} else {
		optref.cameramode.ConsumablesNoTop = false;
	}
	if( CheckAttribute(&InterfaceStates,"ConsumablesNoBottom") ) {
		optref.cameramode.ConsumablesNoBottom = sti(InterfaceStates.ConsumablesNoBottom);
	} else {
		optref.cameramode.ConsumablesNoBottom = false;
	}
	if( CheckAttribute(&InterfaceStates,"ConsumablesHUDContext") ) {
		optref.cameramode.ConsumablesHUDContext = sti(InterfaceStates.ConsumablesHUDContext);
	} else {
		optref.cameramode.ConsumablesHUDContext = 0;
	}

	if( CheckAttribute(&InterfaceStates,"ShipBarsHorizontal") ) {
		optref.cameramode.ShipBarsHorizontal = sti(InterfaceStates.ShipBarsHorizontal);
	} else {
		optref.cameramode.ShipBarsHorizontal = 0;
	}

	if( CheckAttribute(&InterfaceStates,"SpyglassFromMars") ) {
		optref.cameramode.SpyglassFromMars = sti(InterfaceStates.SpyglassFromMars);
	} else {
		optref.cameramode.SpyglassFromMars = 0;
	}

	if( CheckAttribute(&InterfaceStates,"RollingBroadside") ) {
		optref.cameramode.RollingBroadside = sti(InterfaceStates.RollingBroadside);
	} else {
		optref.cameramode.RollingBroadside = 0;
	}

	if( CheckAttribute(&InterfaceStates,"SeaSpeakSignals") ) {
		optref.cameramode.SeaSpeakSignals = sti(InterfaceStates.SeaSpeakSignals);
	} else {
		optref.cameramode.SeaSpeakSignals = 1;
	}

	if( CheckAttribute(&InterfaceStates,"SeaCamFreeAzimuth") ) {
		optref.cameramode.SeaCamFreeAzimuth = sti(InterfaceStates.SeaCamFreeAzimuth);
	} else {
		optref.cameramode.SeaCamFreeAzimuth = 0;
	}
	if( CheckAttribute(&InterfaceStates,"GunTrail") ) {
		optref.cameramode.GunTrail = sti(InterfaceStates.GunTrail);
	} else {
		optref.cameramode.GunTrail = 1;
	}
	if( CheckAttribute(&InterfaceStates,"NpcHeadLook") ) {
		optref.cameramode.NpcHeadLook = sti(InterfaceStates.NpcHeadLook);
	} else {
		optref.cameramode.NpcHeadLook = 1;
	}
	if( CheckAttribute(&InterfaceStates,"ChopBodies") ) {
		optref.cameramode.ChopBodies = sti(InterfaceStates.ChopBodies);
	} else {
		optref.cameramode.ChopBodies = 1;
	}

	if( CheckAttribute(&InterfaceStates,"AutoSaveBoarding") ) { optref.cameramode.AutoSaveBoarding = sti(InterfaceStates.AutoSaveBoarding); } else { optref.cameramode.AutoSaveBoarding = 1; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveToSea") ) { optref.cameramode.AutoSaveToSea = sti(InterfaceStates.AutoSaveToSea); } else { optref.cameramode.AutoSaveToSea = 0; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveMapToSea") ) { optref.cameramode.AutoSaveMapToSea = sti(InterfaceStates.AutoSaveMapToSea); } else { optref.cameramode.AutoSaveMapToSea = 0; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveToLand") ) { optref.cameramode.AutoSaveToLand = sti(InterfaceStates.AutoSaveToLand); } else { optref.cameramode.AutoSaveToLand = 0; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveToMap") ) { optref.cameramode.AutoSaveToMap = sti(InterfaceStates.AutoSaveToMap); } else { optref.cameramode.AutoSaveToMap = 0; }
	if( CheckAttribute(&InterfaceStates,"SaveListFilter") ) { optref.cameramode.SaveListFilter = sti(InterfaceStates.SaveListFilter); } else { optref.cameramode.SaveListFilter = 0; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveSlotsBoard") ) { optref.cameramode.AutoSaveSlotsBoard = sti(InterfaceStates.AutoSaveSlotsBoard); } else { optref.cameramode.AutoSaveSlotsBoard = 3; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveSlotsSea") ) { optref.cameramode.AutoSaveSlotsSea = sti(InterfaceStates.AutoSaveSlotsSea); } else { optref.cameramode.AutoSaveSlotsSea = 3; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveSlotsMapSea") ) { optref.cameramode.AutoSaveSlotsMapSea = sti(InterfaceStates.AutoSaveSlotsMapSea); } else { optref.cameramode.AutoSaveSlotsMapSea = 3; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveSlotsLand") ) { optref.cameramode.AutoSaveSlotsLand = sti(InterfaceStates.AutoSaveSlotsLand); } else { optref.cameramode.AutoSaveSlotsLand = 3; }
	if( CheckAttribute(&InterfaceStates,"AutoSaveSlotsMap") ) { optref.cameramode.AutoSaveSlotsMap = sti(InterfaceStates.AutoSaveSlotsMap); } else { optref.cameramode.AutoSaveSlotsMap = 3; }

		if( CheckAttribute(&InterfaceStates,"ConsumablesHUDScale") ) {
			optref.cameramode.ConsumablesHUDScale = stf(InterfaceStates.ConsumablesHUDScale);
		} else {
			optref.cameramode.ConsumablesHUDScale = 1.0;
		}

	if( CheckAttribute(&InterfaceStates,"EnabledShipMarks") ) {
		optref.cameramode.EnabledShipMarks = sti(InterfaceStates.EnabledShipMarks);
	} else {
		optref.cameramode.EnabledShipMarks = true;
	}
	
	if( CheckAttribute(&InterfaceStates,"DisableShipMarksSelf") ) {
		optref.cameramode.DisableShipMarksSelf = sti(InterfaceStates.DisableShipMarksSelf);
	} else {
		optref.cameramode.DisableShipMarksSelf = false;
	}
	
	if( CheckAttribute(&InterfaceStates,"DisableShipMarksCompanions") ) {
		optref.cameramode.DisableShipMarksCompanions = sti(InterfaceStates.DisableShipMarksCompanions);
	} else {
		optref.cameramode.DisableShipMarksCompanions = false;
	}
	// LEO: Текстура баров: 0 - CSP, 1 - старые HD.
	if( CheckAttribute(&InterfaceStates,"OldShipBars") ) {
		optref.cameramode.OldShipBars = sti(InterfaceStates.OldShipBars);
	} else {
		optref.cameramode.OldShipBars = 0;
	}

	if( CheckAttribute(&InterfaceStates,"RegionsWorldmap") ) {
		optref.cameramode.RegionsWorldmap = sti(InterfaceStates.RegionsWorldmap);
	} else {
		optref.cameramode.RegionsWorldmap = true;
	}

	if( CheckAttribute(&InterfaceStates,"WorldRadarOn") )        optref.cameramode.WorldRadarOn = sti(InterfaceStates.WorldRadarOn);
	if( CheckAttribute(&InterfaceStates,"WorldRadarShape") )     optref.cameramode.WorldRadarShape = sti(InterfaceStates.WorldRadarShape);
	if( CheckAttribute(&InterfaceStates,"WorldRadarRange") )     optref.cameramode.WorldRadarRange = stf(InterfaceStates.WorldRadarRange);
	if( CheckAttribute(&InterfaceStates,"WorldRadarSize") )      optref.cameramode.WorldRadarSize = stf(InterfaceStates.WorldRadarSize);
	if( CheckAttribute(&InterfaceStates,"WorldRadarShowEnc") )   optref.cameramode.WorldRadarShowEnc = sti(InterfaceStates.WorldRadarShowEnc);
	if( CheckAttribute(&InterfaceStates,"WorldRadarShowStorm") ) optref.cameramode.WorldRadarShowStorm = sti(InterfaceStates.WorldRadarShowStorm);
	if( CheckAttribute(&InterfaceStates,"WorldRadarShowForts") ) optref.cameramode.WorldRadarShowForts = sti(InterfaceStates.WorldRadarShowForts);
	if( CheckAttribute(&InterfaceStates,"WorldRadarBlip") )      optref.cameramode.WorldRadarBlip = stf(InterfaceStates.WorldRadarBlip);
	if( CheckAttribute(&InterfaceStates,"WorldRadarMapRes") )    optref.cameramode.WorldRadarMapRes = sti(InterfaceStates.WorldRadarMapRes);

	if( CheckAttribute(&InterfaceStates,"SimpleSea") ) {
		optref.cameramode.SimpleSeaMode = sti(InterfaceStates.SimpleSea);
	} else {
		optref.cameramode.SimpleSeaMode = true;
	}

	if( CheckAttribute(&InterfaceStates,"SpyglassTextures") ) {
		optref.cameramode.SpyglassTextures = sti(InterfaceStates.SpyglassTextures);
	} else {
		optref.cameramode.SpyglassTextures = true;
	}

	if( CheckAttribute(&InterfaceStates,"HUDStyle") ) {
		optref.cameramode.HUDStyle = sti(InterfaceStates.HUDStyle);
	} else {
		optref.cameramode.HUDStyle = true;
	}

	if( CheckAttribute(&InterfaceStates,"VISUAL_CIRASS") ) {
		optref.cameramode.VISUAL_CIRASS = sti(InterfaceStates.VISUAL_CIRASS);
	} else {
		optref.cameramode.VISUAL_CIRASS = 0;
	}

	NewHUD_SaveOptions(optref);
	NewSeaHUD_SaveOptions(optref);
	LandHUD_SaveOptions(optref);
	CommandIcons_SaveOptions(optref);
	if( CheckAttribute(&InterfaceStates,"InterfaceSelection") ) {
		optref.cameramode.InterfaceSelection = sti(InterfaceStates.InterfaceSelection);
	} else {
		optref.cameramode.InterfaceSelection = 0;
	}
	if( CheckAttribute(&InterfaceStates,"InterfaceShipString") ) {
		optref.cameramode.InterfaceShipString = InterfaceStates.InterfaceShipString;
	} else {
		optref.cameramode.InterfaceShipString = "interfaces\ships\";
	}
	if( CheckAttribute(&InterfaceStates,"AltFont") ) {
		optref.cameramode.AltFont = sti(InterfaceStates.AltFont);
	} else {
		optref.cameramode.AltFont = 0;
	}
	
	if( CheckAttribute(&InterfaceStates,"BombsParticles") ) {
		optref.cameramode.BombsParticles = sti(InterfaceStates.BombsParticles);
	} else {
		optref.cameramode.BombsParticles = 0;
	}
	if( CheckAttribute(&InterfaceStates,"FireParticles") ) {
		optref.cameramode.FireParticles = sti(InterfaceStates.FireParticles);
	} else {
		optref.cameramode.FireParticles = 0;
	}
	if( CheckAttribute(&InterfaceStates,"OtherParticles") ) {
		optref.cameramode.OtherParticles = sti(InterfaceStates.OtherParticles);
	} else {
		optref.cameramode.OtherParticles = 0;
	}

	if( CheckAttribute(&InterfaceStates,"NoInt") ) {
		optref.cameramode.NoInt = sti(InterfaceStates.NoInt);
	} else {
		optref.cameramode.NoInt = true;
	}

	if( CheckAttribute(&InterfaceStates,"IslandLoader") ) {
		optref.cameramode.IslandLoader = sti(InterfaceStates.IslandLoader);
	} else {
		optref.cameramode.IslandLoader = true;
	}

	if( CheckAttribute(&InterfaceStates,"DirectSail") ) { // Gregg: сохранение опции доплытия
		optref.cameramode.DirectSail = sti(InterfaceStates.DirectSail);
	} else {
		optref.cameramode.DirectSail = false;
	}

	if( CheckAttribute(&InterfaceStates,"DialogPortrait") ) { // Gregg: сохранение опции портрета в диалоге
		optref.cameramode.DialogPortrait = sti(InterfaceStates.DialogPortrait);
	} else {
		optref.cameramode.DialogPortrait = true;
	}

	// LEO: Дублируем тему и анимацию из engine.ini в options для восстановления выбора в меню.
	if( CheckAttribute(&InterfaceStates,"BlueInterfaceTheme") ) {
		optref.cameramode.BlueInterfaceTheme = sti(InterfaceStates.BlueInterfaceTheme);
	} else {
		optref.cameramode.BlueInterfaceTheme = false;
	}
	if( CheckAttribute(&InterfaceStates,"ProgressAnimFile") ) {
		optref.cameramode.ProgressAnimFile = InterfaceStates.ProgressAnimFile;
	} else {
		optref.cameramode.ProgressAnimFile = "";
	}

	if( CheckAttribute(&InterfaceStates,"AltIntIcons") ) {
		optref.cameramode.AltIntIcons = sti(InterfaceStates.AltIntIcons);
	} else {
		optref.cameramode.AltIntIcons = true;
	}
	
	if( CheckAttribute(&InterfaceStates,"CharCap") ) {
		optref.cameramode.CharCap = sti(InterfaceStates.CharCap);
	} else {
		optref.cameramode.CharCap = true;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapShowOthers") ) {
		optref.cameramode.CharCapShowOthers = sti(InterfaceStates.CharCapShowOthers);
	} else {
		optref.cameramode.CharCapShowOthers = true;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapShowOff") ) {
		optref.cameramode.CharCapShowOff = sti(InterfaceStates.CharCapShowOff);
	} else {
		optref.cameramode.CharCapShowOff = false;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapShowNotifications") ) {
		optref.cameramode.CharCapShowNotifications = sti(InterfaceStates.CharCapShowNotifications);
	} else {
		optref.cameramode.CharCapShowNotifications = false;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapShowRank") ) {
		optref.cameramode.CharCapShowRank = sti(InterfaceStates.CharCapShowRank);
	} else {
		optref.cameramode.CharCapShowRank = false;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapShowSpec") ) {
		optref.cameramode.CharCapShowSpec = sti(InterfaceStates.CharCapShowSpec);
	} else {
		optref.cameramode.CharCapShowSpec = false;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapFontScale") ) {
		optref.cameramode.CharCapFontScale = stf(InterfaceStates.CharCapFontScale);
	} else {
		optref.cameramode.CharCapFontScale = 0.3;
	}
	if( CheckAttribute(&InterfaceStates,"DialogInputDelay") ) {
		optref.cameramode.DialogInputDelay = stf(InterfaceStates.DialogInputDelay);
	} else {
		optref.cameramode.DialogInputDelay = 0.5;
	}
	if( CheckAttribute(&InterfaceStates,"DialogDelay") ) {
		optref.cameramode.DialogDelay = sti(InterfaceStates.DialogDelay);
	} else {
		optref.cameramode.DialogDelay = 1;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapNotifScale") ) {
		optref.cameramode.CharCapNotifScale = stf(InterfaceStates.CharCapNotifScale);
	} else {
		optref.cameramode.CharCapNotifScale = 0.3;
	}
	if( CheckAttribute(&InterfaceStates,"EnemyBarSize") ) {
		optref.cameramode.EnemyBarSize = stf(InterfaceStates.EnemyBarSize);
	} else {
		optref.cameramode.EnemyBarSize = 0.4286;
	}
	if( CheckAttribute(&InterfaceStates,"EnemyBarDist") ) {
		optref.cameramode.EnemyBarDist = stf(InterfaceStates.EnemyBarDist);
	} else {
		optref.cameramode.EnemyBarDist = 0.4286;
	}
	if( CheckAttribute(&InterfaceStates,"EnemyBarMax") ) {
		optref.cameramode.EnemyBarMax = stf(InterfaceStates.EnemyBarMax);
	} else {
		optref.cameramode.EnemyBarMax = 0.2;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapTargetOfficers") ) {
		optref.cameramode.CharCapTargetOfficers = sti(InterfaceStates.CharCapTargetOfficers);
	} else {
		optref.cameramode.CharCapTargetOfficers = false;
	}
	if( CheckAttribute(&InterfaceStates,"CharCapLandBars") ) {
		optref.cameramode.CharCapLandBars = sti(InterfaceStates.CharCapLandBars);
	} else {
		optref.cameramode.CharCapLandBars = 1;
	}
	NPCShowName(sti(InterfaceStates.CharCapShowOff), sti(InterfaceStates.CharCapShowOthers), sti(InterfaceStates.CharCapTargetOfficers));
	
	if( CheckAttribute(&InterfaceStates,"ShipCap") ) {
		optref.cameramode.ShipCap = sti(InterfaceStates.ShipCap);
	} else {
		optref.cameramode.ShipCap = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShipNames1") ) {
		optref.cameramode.ShipCapShipNames1 = sti(InterfaceStates.ShipCapShipNames1);
	} else {
		optref.cameramode.ShipCapShipNames1 = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShipNames2") ) {
		optref.cameramode.ShipCapShipNames2 = sti(InterfaceStates.ShipCapShipNames2);
	} else {
		optref.cameramode.ShipCapShipNames2 = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowActions") ) {
		optref.cameramode.ShipCapShowActions = sti(InterfaceStates.ShipCapShowActions);
	} else {
		optref.cameramode.ShipCapShowActions = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowActionsPlus") ) {
		optref.cameramode.ShipCapShowActionsPlus = sti(InterfaceStates.ShipCapShowActionsPlus);
	} else {
		optref.cameramode.ShipCapShowActionsPlus = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowAllies") ) {
		optref.cameramode.ShipCapShowAllies = sti(InterfaceStates.ShipCapShowAllies);
	} else {
		optref.cameramode.ShipCapShowAllies = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowOther") ) {
		optref.cameramode.ShipCapShowOther = sti(InterfaceStates.ShipCapShowOther);
	} else {
		optref.cameramode.ShipCapShowOther = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowNotifications") ) {
		optref.cameramode.ShipCapShowNotifications = sti(InterfaceStates.ShipCapShowNotifications);
	} else {
		optref.cameramode.ShipCapShowNotifications = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowCrit") ) {
		optref.cameramode.ShipCapShowCrit = sti(InterfaceStates.ShipCapShowCrit);
	} else {
		optref.cameramode.ShipCapShowCrit = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowFortGun") ) {
		optref.cameramode.ShipCapShowFortGun = sti(InterfaceStates.ShipCapShowFortGun);
	} else {
		optref.cameramode.ShipCapShowFortGun = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowDamage") ) {
		optref.cameramode.ShipCapShowDamage = sti(InterfaceStates.ShipCapShowDamage);
	} else {
		optref.cameramode.ShipCapShowDamage = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapShowDamageSelf") ) {
		optref.cameramode.ShipCapShowDamageSelf = sti(InterfaceStates.ShipCapShowDamageSelf);
	} else {
		optref.cameramode.ShipCapShowDamageSelf = true;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapNotifDistance") ) {
		optref.cameramode.ShipCapNotifDistance = stf(InterfaceStates.ShipCapNotifDistance);
	} else {
		optref.cameramode.ShipCapNotifDistance = 0.112;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapNotifScale") ) {
		optref.cameramode.ShipCapNotifScale = stf(InterfaceStates.ShipCapNotifScale);
	} else {
		optref.cameramode.ShipCapNotifScale = 0.3;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapNamesScale") ) {
		optref.cameramode.ShipCapNamesScale = stf(InterfaceStates.ShipCapNamesScale);
	} else {
		optref.cameramode.ShipCapNamesScale = 0.3;
	}
	if( CheckAttribute(&InterfaceStates,"ShipCapIconsScale") ) {
		optref.cameramode.ShipCapIconsScale = stf(InterfaceStates.ShipCapIconsScale);
	} else {
		optref.cameramode.ShipCapIconsScale = 0.3;
	}
	ShipShowName();
	
	if( CheckAttribute(&InterfaceStates,"ParticlesCount") ) {
		optref.cameramode.ParticlesCount = stf(InterfaceStates.ParticlesCount);
	} else {
		optref.cameramode.ParticlesCount = 0.0;
	}
	
	if( CheckAttribute(&InterfaceStates,"ShowBoardMode") ) {
		optref.cameramode.ShowBoardMode = sti(InterfaceStates.ShowBoardMode);
	} else {
		optref.cameramode.ShowBoardMode = true;
	}
	
	if( CheckAttribute(&InterfaceStates,"CharVoice") ) {
		optref.cameramode.CharVoice = sti(InterfaceStates.CharVoice);
	} else {
		optref.cameramode.CharVoice = true;
	}
	
	if( CheckAttribute(&InterfaceStates,"EnableSailors") ) {
		optref.cameramode.EnableSailors = sti(InterfaceStates.EnableSailors);
	} else {
		optref.cameramode.EnableSailors = true;
	}

	GetControlsOptions(optref);

	// mouse
	if( CheckAttribute(&InterfaceStates,"mouse.x_sens") ) {
		optref.mouse.x_sensitivity = InterfaceStates.mouse.x_sens;
	} else {
		optref.mouse.x_sensitivity = 0.5;
	}
	if( CheckAttribute(&InterfaceStates,"mouse.y_sens") ) {
		optref.mouse.y_sensitivity = InterfaceStates.mouse.y_sens;
	} else {
		optref.mouse.y_sensitivity = 0.5;
	}
	// video colors
	if( CheckAttribute(&InterfaceStates,"video.contrast") ) {
		optref.video.contrast = InterfaceStates.video.contrast;
	} else {
		optref.video.contrast = 1.0;
	}
	if( CheckAttribute(&InterfaceStates,"video.gamma") ) {
		optref.video.gamma = InterfaceStates.video.gamma;
	} else {
		optref.video.gamma = 1.0;
	}
	if( CheckAttribute(&InterfaceStates,"video.brightness") ) {
		optref.video.brightness = InterfaceStates.video.brightness;
	} else {
		optref.video.brightness = 0.0;
	}
	if( CheckAttribute(&InterfaceStates,"menu") ) {
		optref.menu = InterfaceStates.menu;
	} else {
		optref.menu = 1;
	}

	SeaParametrs.MaxVertices = MaxVertices;
	SeaParametrs.MaxIndices = MaxIndices;
	SeaParametrs.GridStepX = GridStepX;
	SeaParametrs.GridStepPC = GridStepPC;
	SeaParametrs.MaxWaveDistance = MaxWaveDistance;
	SeaParametrs.LodScale = LodScale;

	// always run
	if( CheckAttribute(&InterfaceStates,"alwaysrun") ) {
		optref.alwaysrun = InterfaceStates.alwaysrun;
	} else {
		optref.alwaysrun = false;
	}
}

void SetCurentOptions(ref optref)
{
	ShipInfoBars_LoadOptions(optref); // LEO: Старые options без нового поля сохраняют показ «всегда».
	SendMessage(&sound,"lfff", MSG_SOUND_SET_MASTER_VOLUME, stf(optref.volume.sound),	stf(optref.volume.music),	stf(optref.volume.dialog));

	locCameraEnableSpecialMode = !sti(optref.cameramode.follow_on);

	if( CheckAttribute(optref,"cameramode.ShowBattleMode") ) {
		InterfaceStates.ShowBattleMode = optref.cameramode.ShowBattleMode;
	} else {
		InterfaceStates.ShowBattleMode = false;
	}

	if( CheckAttribute(optref,"cameramode.EnabledAutoSaveMode") ) {
		InterfaceStates.EnabledAutoSaveMode = optref.cameramode.EnabledAutoSaveMode;
	} else {
		InterfaceStates.EnabledAutoSaveMode = 1;
	}

	if( CheckAttribute(optref,"cameramode.EnabledQuestsMarks") ) {
		InterfaceStates.EnabledQuestsMarks = optref.cameramode.EnabledQuestsMarks;
	} else {
		InterfaceStates.EnabledQuestsMarks = true;
	}

	if( CheckAttribute(optref,"cameramode.EnabledFXMarks") ) {
		InterfaceStates.EnabledFXMarks = optref.cameramode.EnabledFXMarks;
	} else {
		InterfaceStates.EnabledFXMarks = true;
	}

	if( CheckAttribute(optref,"cameramode.EnabledCMControls") ) {
		InterfaceStates.EnabledCMControls = optref.cameramode.EnabledCMControls;
	} else {
		InterfaceStates.EnabledCMControls = false;
	}

	if( CheckAttribute(optref,"cameramode.EnabledOldStore") ) {
		InterfaceStates.EnabledOldStore = optref.cameramode.EnabledOldStore;
	} else {
		InterfaceStates.EnabledOldStore = false;
	}

	if( CheckAttribute(optref,"sound.EnabledAltSoundsGun") ) {
		InterfaceStates.EnabledAltSoundsGun = optref.sound.EnabledAltSoundsGun;
	} else {
		InterfaceStates.EnabledAltSoundsGun = false;
	}

	if( CheckAttribute(optref,"game.DiscordRPC") ) {
		InterfaceStates.DiscordRPC = optref.game.DiscordRPC;
	} else {
		InterfaceStates.DiscordRPC = true;
	}
	SwitchRPC(InterfaceStates.DiscordRPC);

	if( CheckAttribute(optref,"sound.StreamerMode") ) {
		InterfaceStates.StreamerMode = optref.sound.StreamerMode;
	} else {
		InterfaceStates.StreamerMode = false;
	}

	if( CheckAttribute(optref,"cameramode.ShowExpLogs") ) {
		InterfaceStates.ShowExpLogs = optref.cameramode.ShowExpLogs;
	} else {
		InterfaceStates.ShowExpLogs = false;
	}

	if( CheckAttribute(optref,"cameramode.ShowGameLog") ) {
		InterfaceStates.ShowGameLog = optref.cameramode.ShowGameLog;
	} else {
		InterfaceStates.ShowGameLog = 1; // LEO: int, не bool — иначе sti("true")=0 глушил журнал
	}

	if( CheckAttribute(optref,"cameramode.ConsumablesHUD") ) {
		InterfaceStates.ConsumablesHUD = optref.cameramode.ConsumablesHUD;
	} else {
		InterfaceStates.ConsumablesHUD = false;
	}
	// LEO: Сигналы абордажников — мастер вкл/выкл (главный чекбокс) + под-опции тряска/пинг (по умолчанию ВКЛ)
	if( CheckAttribute(optref,"cameramode.BoardingSignals") ) {
		InterfaceStates.BoardingSignals = optref.cameramode.BoardingSignals;
	} else {
		InterfaceStates.BoardingSignals = 1;
	}
	if( CheckAttribute(optref,"cameramode.PortraitShakeEnable") ) {
		InterfaceStates.PortraitShakeEnable = optref.cameramode.PortraitShakeEnable;
	} else {
		InterfaceStates.PortraitShakeEnable = 1;
	}
	if( CheckAttribute(optref,"cameramode.PortraitPingEnable") ) {
		InterfaceStates.PortraitPingEnable = optref.cameramode.PortraitPingEnable;
	} else {
		InterfaceStates.PortraitPingEnable = 1;
	}
	// LEO Этап 3: радар/миникарта — мастер вкл/выкл фичи
	if( CheckAttribute(optref,"cameramode.RadarEnabled") ) {
		InterfaceStates.RadarEnabled = optref.cameramode.RadarEnabled;
	} else {
		InterfaceStates.RadarEnabled = 0;
	}
	// LEO Этап 3: настройки радара — слайдеры/качество/фильтр групп
	if( CheckAttribute(optref,"cameramode.RadarBlipScale") ) InterfaceStates.RadarBlipScale = optref.cameramode.RadarBlipScale; else InterfaceStates.RadarBlipScale = 1.0;
	// LEO: Дальности грузим только при старте: открытие опций не должно стирать зум, изменённый хоткеями.
	if( !CheckAttribute(&InterfaceStates,"RadarRange") ) { if( CheckAttribute(optref,"cameramode.RadarRange") ) InterfaceStates.RadarRange = optref.cameramode.RadarRange; else InterfaceStates.RadarRange = 60.0; }
	if( !CheckAttribute(&InterfaceStates,"RadarRangeInside") ) { if( CheckAttribute(optref,"cameramode.RadarRangeInside") ) InterfaceStates.RadarRangeInside = optref.cameramode.RadarRangeInside; else InterfaceStates.RadarRangeInside = 10.0; }
	if( CheckAttribute(optref,"cameramode.RadarMapRes") ) InterfaceStates.RadarMapRes = optref.cameramode.RadarMapRes; else InterfaceStates.RadarMapRes = 2048;
	if( CheckAttribute(optref,"cameramode.RadarRadius") ) InterfaceStates.RadarRadius = optref.cameramode.RadarRadius; else InterfaceStates.RadarRadius = 210.0;
	if( CheckAttribute(optref,"cameramode.RadarShowCitizen") ) InterfaceStates.RadarShowCitizen = optref.cameramode.RadarShowCitizen; else InterfaceStates.RadarShowCitizen = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowPatrol") ) InterfaceStates.RadarShowPatrol = optref.cameramode.RadarShowPatrol; else InterfaceStates.RadarShowPatrol = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowMusket") ) InterfaceStates.RadarShowMusket = optref.cameramode.RadarShowMusket; else InterfaceStates.RadarShowMusket = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowAlly") ) InterfaceStates.RadarShowAlly = optref.cameramode.RadarShowAlly; else InterfaceStates.RadarShowAlly = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowSkeleton") ) InterfaceStates.RadarShowSkeleton = optref.cameramode.RadarShowSkeleton; else InterfaceStates.RadarShowSkeleton = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowQuest") ) InterfaceStates.RadarShowQuest = optref.cameramode.RadarShowQuest; else InterfaceStates.RadarShowQuest = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowCombatAlly") ) InterfaceStates.RadarShowCombatAlly = optref.cameramode.RadarShowCombatAlly; else InterfaceStates.RadarShowCombatAlly = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowCombatEnemy") ) InterfaceStates.RadarShowCombatEnemy = optref.cameramode.RadarShowCombatEnemy; else InterfaceStates.RadarShowCombatEnemy = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowDead") ) InterfaceStates.RadarShowDead = optref.cameramode.RadarShowDead; else InterfaceStates.RadarShowDead = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowItem") ) InterfaceStates.RadarShowItem = optref.cameramode.RadarShowItem; else InterfaceStates.RadarShowItem = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowChest") ) InterfaceStates.RadarShowChest = optref.cameramode.RadarShowChest; else InterfaceStates.RadarShowChest = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowHerb") ) InterfaceStates.RadarShowHerb = optref.cameramode.RadarShowHerb; else InterfaceStates.RadarShowHerb = 1;
	if( CheckAttribute(optref,"cameramode.RadarShowDoors") ) InterfaceStates.RadarShowDoors = optref.cameramode.RadarShowDoors; else InterfaceStates.RadarShowDoors = 1;
	if( CheckAttribute(optref,"cameramode.RadarShape") ) InterfaceStates.RadarShape = optref.cameramode.RadarShape; else InterfaceStates.RadarShape = 0;
	if( CheckAttribute(optref,"cameramode.RadarRotate") ) InterfaceStates.RadarRotate = optref.cameramode.RadarRotate; else InterfaceStates.RadarRotate = 1;
	if( CheckAttribute(optref,"cameramode.ConsumablesNoTop") ) {
		InterfaceStates.ConsumablesNoTop = optref.cameramode.ConsumablesNoTop;
	} else {
		InterfaceStates.ConsumablesNoTop = false;
	}
	if( CheckAttribute(optref,"cameramode.ConsumablesNoBottom") ) {
		InterfaceStates.ConsumablesNoBottom = optref.cameramode.ConsumablesNoBottom;
	} else {
		InterfaceStates.ConsumablesNoBottom = false;
	}
	if( CheckAttribute(optref,"cameramode.ConsumablesHUDContext") ) {
		InterfaceStates.ConsumablesHUDContext = optref.cameramode.ConsumablesHUDContext;
	} else {
		InterfaceStates.ConsumablesHUDContext = 0;
	}

	if( CheckAttribute(optref,"cameramode.ShipBarsHorizontal") ) {
		InterfaceStates.ShipBarsHorizontal = optref.cameramode.ShipBarsHorizontal;
	} else {
		InterfaceStates.ShipBarsHorizontal = 0;
	}

	// LEO: труба по CTRL от 3-го лица — с марса (1) / с палубы (0)
	if( CheckAttribute(optref,"cameramode.SpyglassFromMars") ) {
		InterfaceStates.SpyglassFromMars = optref.cameramode.SpyglassFromMars;
	} else {
		InterfaceStates.SpyglassFromMars = 0;
	}

	// LEO: залп бортом нос->корма (все корабли)
	if( CheckAttribute(optref,"cameramode.RollingBroadside") ) {
		InterfaceStates.RollingBroadside = optref.cameramode.RollingBroadside;
	} else {
		InterfaceStates.RollingBroadside = 0;
	}

	// LEO: входящие переговоры в море
	if( CheckAttribute(optref,"cameramode.SeaSpeakSignals") ) {
		InterfaceStates.SeaSpeakSignals = optref.cameramode.SeaSpeakSignals;
	} else {
		InterfaceStates.SeaSpeakSignals = 1;
	}

	// LEO: свободная камера в море
	if( CheckAttribute(optref,"cameramode.SeaCamFreeAzimuth") ) {
		InterfaceStates.SeaCamFreeAzimuth = optref.cameramode.SeaCamFreeAzimuth;
	} else {
		InterfaceStates.SeaCamFreeAzimuth = 0;
	}
	// LEO: дымный след пули на суше
	if( CheckAttribute(optref,"cameramode.GunTrail") ) {
		InterfaceStates.GunTrail = optref.cameramode.GunTrail;
	} else {
		InterfaceStates.GunTrail = 1;
	}
	// LEO: NPC поглядывают на соседей
	if( CheckAttribute(optref,"cameramode.NpcHeadLook") ) {
		InterfaceStates.NpcHeadLook = optref.cameramode.NpcHeadLook;
	} else {
		InterfaceStates.NpcHeadLook = 1;
	}
	// LEO: расчленение тел при взрывах
	if( CheckAttribute(optref,"cameramode.ChopBodies") ) {
		InterfaceStates.ChopBodies = optref.cameramode.ChopBodies;
	} else {
		InterfaceStates.ChopBodies = 1;
	}

	// LEO: АВТОСЕЙВЫ - типы и число слотов кольца
	if( CheckAttribute(optref,"cameramode.AutoSaveBoarding") ) { InterfaceStates.AutoSaveBoarding = optref.cameramode.AutoSaveBoarding; } else { InterfaceStates.AutoSaveBoarding = 1; }
	if( CheckAttribute(optref,"cameramode.AutoSaveToSea") ) { InterfaceStates.AutoSaveToSea = optref.cameramode.AutoSaveToSea; } else { InterfaceStates.AutoSaveToSea = 0; }
	if( CheckAttribute(optref,"cameramode.AutoSaveMapToSea") ) { InterfaceStates.AutoSaveMapToSea = optref.cameramode.AutoSaveMapToSea; } else { InterfaceStates.AutoSaveMapToSea = 0; }
	if( CheckAttribute(optref,"cameramode.AutoSaveToLand") ) { InterfaceStates.AutoSaveToLand = optref.cameramode.AutoSaveToLand; } else { InterfaceStates.AutoSaveToLand = 0; }
	if( CheckAttribute(optref,"cameramode.AutoSaveToMap") ) { InterfaceStates.AutoSaveToMap = optref.cameramode.AutoSaveToMap; } else { InterfaceStates.AutoSaveToMap = 0; }
	if( CheckAttribute(optref,"cameramode.SaveListFilter") ) { InterfaceStates.SaveListFilter = optref.cameramode.SaveListFilter; } else { InterfaceStates.SaveListFilter = 0; }
	if( CheckAttribute(optref,"cameramode.AutoSaveSlotsBoard") ) { InterfaceStates.AutoSaveSlotsBoard = optref.cameramode.AutoSaveSlotsBoard; } else { InterfaceStates.AutoSaveSlotsBoard = 3; }
	if( CheckAttribute(optref,"cameramode.AutoSaveSlotsSea") ) { InterfaceStates.AutoSaveSlotsSea = optref.cameramode.AutoSaveSlotsSea; } else { InterfaceStates.AutoSaveSlotsSea = 3; }
	if( CheckAttribute(optref,"cameramode.AutoSaveSlotsMapSea") ) { InterfaceStates.AutoSaveSlotsMapSea = optref.cameramode.AutoSaveSlotsMapSea; } else { InterfaceStates.AutoSaveSlotsMapSea = 3; }
	if( CheckAttribute(optref,"cameramode.AutoSaveSlotsLand") ) { InterfaceStates.AutoSaveSlotsLand = optref.cameramode.AutoSaveSlotsLand; } else { InterfaceStates.AutoSaveSlotsLand = 3; }
	if( CheckAttribute(optref,"cameramode.AutoSaveSlotsMap") ) { InterfaceStates.AutoSaveSlotsMap = optref.cameramode.AutoSaveSlotsMap; } else { InterfaceStates.AutoSaveSlotsMap = 3; }

		// LEO: размер HUD расходников (float)
		if( CheckAttribute(optref,"cameramode.ConsumablesHUDScale") ) {
			InterfaceStates.ConsumablesHUDScale = stf(optref.cameramode.ConsumablesHUDScale);
		} else {
			InterfaceStates.ConsumablesHUDScale = 1.0;
		}

	if( CheckAttribute(optref,"cameramode.EnabledShipMarks") ) {
		InterfaceStates.EnabledShipMarks = optref.cameramode.EnabledShipMarks;
	} else {
		InterfaceStates.EnabledShipMarks = true;
	}
	
	if( CheckAttribute(optref,"cameramode.DisableShipMarksSelf") ) {
		InterfaceStates.DisableShipMarksSelf = optref.cameramode.DisableShipMarksSelf;
	} else {
		InterfaceStates.DisableShipMarksSelf = false;
	}
	
	if( CheckAttribute(optref,"cameramode.DisableShipMarksCompanions") ) {
		InterfaceStates.DisableShipMarksCompanions = optref.cameramode.DisableShipMarksCompanions;
	} else {
		InterfaceStates.DisableShipMarksCompanions = false;
	}
	// LEO: переключатель текстуры баров над кораблями (0=новые CSP, 1=старые HD)
	if( CheckAttribute(optref,"cameramode.OldShipBars") ) {
		InterfaceStates.OldShipBars = optref.cameramode.OldShipBars;
	} else {
		InterfaceStates.OldShipBars = 0;
	}

	if( CheckAttribute(optref,"cameramode.RegionsWorldmap") ) {
		InterfaceStates.RegionsWorldmap = optref.cameramode.RegionsWorldmap;
	} else {
		InterfaceStates.RegionsWorldmap = false;
	}

	// LEO: Включение и зум глобалки грузим только при старте, сохраняя изменения игровых хоткеев.
	if( !CheckAttribute(&InterfaceStates,"WorldRadarOn") ) { if( CheckAttribute(optref,"cameramode.WorldRadarOn") ) InterfaceStates.WorldRadarOn = optref.cameramode.WorldRadarOn; }
	if( CheckAttribute(optref,"cameramode.WorldRadarShape") )     InterfaceStates.WorldRadarShape = optref.cameramode.WorldRadarShape;
	if( !CheckAttribute(&InterfaceStates,"WorldRadarRange") ) { if( CheckAttribute(optref,"cameramode.WorldRadarRange") ) InterfaceStates.WorldRadarRange = optref.cameramode.WorldRadarRange; }
	if( CheckAttribute(optref,"cameramode.WorldRadarSize") )      InterfaceStates.WorldRadarSize = optref.cameramode.WorldRadarSize;
	if( CheckAttribute(optref,"cameramode.WorldRadarShowEnc") )   InterfaceStates.WorldRadarShowEnc = optref.cameramode.WorldRadarShowEnc;
	if( CheckAttribute(optref,"cameramode.WorldRadarShowStorm") ) InterfaceStates.WorldRadarShowStorm = optref.cameramode.WorldRadarShowStorm;
	if( CheckAttribute(optref,"cameramode.WorldRadarShowForts") ) InterfaceStates.WorldRadarShowForts = optref.cameramode.WorldRadarShowForts;
	if( CheckAttribute(optref,"cameramode.WorldRadarBlip") )      InterfaceStates.WorldRadarBlip = optref.cameramode.WorldRadarBlip;
	if( CheckAttribute(optref,"cameramode.WorldRadarMapRes") )    InterfaceStates.WorldRadarMapRes = optref.cameramode.WorldRadarMapRes;

	if( CheckAttribute(optref,"cameramode.SimpleSeaMode") ) {
		InterfaceStates.SimpleSea = optref.cameramode.SimpleSeaMode;
	} else {
		InterfaceStates.SimpleSea = false;
	}

	if( CheckAttribute(optref,"cameramode.SpyglassTextures") ) {
		InterfaceStates.SpyglassTextures = optref.cameramode.SpyglassTextures;
	} else {
		InterfaceStates.SpyglassTextures = false;
	}

	if( CheckAttribute(optref,"cameramode.HUDStyle") ) {
		InterfaceStates.HUDStyle = optref.cameramode.HUDStyle;
	} else {
		InterfaceStates.HUDStyle = false;
	}

	if( CheckAttribute(optref,"cameramode.VISUAL_CIRASS") ) {
		InterfaceStates.VISUAL_CIRASS = optref.cameramode.VISUAL_CIRASS;
	} else {
		InterfaceStates.VISUAL_CIRASS = 0;
	}

	NewHUD_LoadOptions(optref);
	NewSeaHUD_LoadOptions(optref);
	LandHUD_LoadOptions(optref);
	CommandIcons_LoadOptions(optref);
	if( CheckAttribute(optref,"cameramode.InterfaceSelection") ) {
		InterfaceStates.InterfaceSelection = optref.cameramode.InterfaceSelection;
	} else {
		InterfaceStates.InterfaceSelection = 0;
	}
	if( CheckAttribute(optref,"cameramode.InterfaceShipString") ) {
		InterfaceStates.InterfaceShipString = optref.cameramode.InterfaceShipString;
	} else {
		InterfaceStates.InterfaceShipString = "interfaces\ships\";
	}
	if( CheckAttribute(optref,"cameramode.AltFont") ) {
		InterfaceStates.AltFont = optref.cameramode.AltFont;
	} else {
		InterfaceStates.AltFont = 0;
	}
	if( CheckAttribute(optref,"cameramode.BombsParticles") ) {
		InterfaceStates.BombsParticles = optref.cameramode.BombsParticles;
	} else {
		InterfaceStates.BombsParticles = 0;
	}
	if( CheckAttribute(optref,"cameramode.FireParticles") ) {
		InterfaceStates.FireParticles = optref.cameramode.FireParticles;
	} else {
		InterfaceStates.FireParticles = 0;
	}
	if( CheckAttribute(optref,"cameramode.OtherParticles") ) {
		InterfaceStates.OtherParticles = optref.cameramode.OtherParticles;
	} else {
		InterfaceStates.OtherParticles = 0;
	}
	if( CheckAttribute(optref,"cameramode.NoInt") ) {
		InterfaceStates.NoInt = optref.cameramode.NoInt;
	} else {
		InterfaceStates.NoInt = false;
	}
	if( CheckAttribute(optref,"cameramode.IslandLoader") ) {
		InterfaceStates.IslandLoader = optref.cameramode.IslandLoader;
	} else {
		InterfaceStates.IslandLoader = false;
	}
	if( CheckAttribute(optref,"cameramode.DirectSail") ) { // Gregg: загрузка опции доплытия
		InterfaceStates.DirectSail = optref.cameramode.DirectSail;
	} else {
		InterfaceStates.DirectSail = false;
	}
	if( CheckAttribute(optref,"cameramode.DialogPortrait") ) { // Gregg: загрузка опции портрета в диалоге
		InterfaceStates.DialogPortrait = optref.cameramode.DialogPortrait;
	} else {
		InterfaceStates.DialogPortrait = true; // по умолчанию портрет включён
	}

	if( CheckAttribute(optref,"cameramode.BlueInterfaceTheme") ) {
		InterfaceStates.BlueInterfaceTheme = optref.cameramode.BlueInterfaceTheme;
	} else {
		InterfaceStates.BlueInterfaceTheme = false;
	}
	if( CheckAttribute(optref,"cameramode.ProgressAnimFile") ) {
		InterfaceStates.ProgressAnimFile = optref.cameramode.ProgressAnimFile;
	} else {
		InterfaceStates.ProgressAnimFile = "";
	}

	if( CheckAttribute(optref,"cameramode.AltIntIcons") ) {
		InterfaceStates.AltIntIcons = optref.cameramode.AltIntIcons;
	} else {
		InterfaceStates.AltIntIcons = false;
	}
	
	if( CheckAttribute(optref,"cameramode.CharCap") ) {
		InterfaceStates.CharCap = optref.cameramode.CharCap;
	} else {
		InterfaceStates.CharCap = true;
	}
	if( CheckAttribute(optref,"cameramode.CharCapShowOthers") ) {
		InterfaceStates.CharCapShowOthers = optref.cameramode.CharCapShowOthers;
	} else {
		InterfaceStates.CharCapShowOthers = true;
	}
	if( CheckAttribute(optref,"cameramode.CharCapShowOff") ) {
		InterfaceStates.CharCapShowOff = optref.cameramode.CharCapShowOff;
	} else {
		InterfaceStates.CharCapShowOff = false;
	}
	if( CheckAttribute(optref,"cameramode.CharCapShowNotifications") ) {
		InterfaceStates.CharCapShowNotifications = optref.cameramode.CharCapShowNotifications;
	} else {
		InterfaceStates.CharCapShowNotifications = false;
	}
	if( CheckAttribute(optref,"cameramode.CharCapShowRank") ) {
		InterfaceStates.CharCapShowRank = optref.cameramode.CharCapShowRank;
	} else {
		InterfaceStates.CharCapShowRank = false;
	}
	if( CheckAttribute(optref,"cameramode.CharCapShowSpec") ) {
		InterfaceStates.CharCapShowSpec = optref.cameramode.CharCapShowSpec;
	} else {
		InterfaceStates.CharCapShowSpec = false;
	}
	if( CheckAttribute(optref,"cameramode.CharCapFontScale") ) {
		InterfaceStates.CharCapFontScale = optref.cameramode.CharCapFontScale;
	} else {
		InterfaceStates.CharCapFontScale = 0.3;
	}
	if( CheckAttribute(optref,"cameramode.DialogInputDelay") ) {
		InterfaceStates.DialogInputDelay = optref.cameramode.DialogInputDelay;
	} else {
		InterfaceStates.DialogInputDelay = 0.5;
	}
	if( CheckAttribute(optref,"cameramode.DialogDelay") ) {
		InterfaceStates.DialogDelay = optref.cameramode.DialogDelay;
	} else {
		InterfaceStates.DialogDelay = 1;
	}
	if( CheckAttribute(optref,"cameramode.CharCapNotifScale") ) {
		InterfaceStates.CharCapNotifScale = optref.cameramode.CharCapNotifScale;
	} else {
		InterfaceStates.CharCapNotifScale = 0.3;
	}
	if( CheckAttribute(optref,"cameramode.EnemyBarSize") ) {
		InterfaceStates.EnemyBarSize = optref.cameramode.EnemyBarSize;
	} else {
		InterfaceStates.EnemyBarSize = 0.4286;
	}
	if( CheckAttribute(optref,"cameramode.EnemyBarDist") ) {
		InterfaceStates.EnemyBarDist = optref.cameramode.EnemyBarDist;
	} else {
		InterfaceStates.EnemyBarDist = 0.4286;
	}
	if( CheckAttribute(optref,"cameramode.EnemyBarMax") ) {
		InterfaceStates.EnemyBarMax = optref.cameramode.EnemyBarMax;
	} else {
		InterfaceStates.EnemyBarMax = 0.2;
	}
	if( CheckAttribute(optref,"cameramode.CharCapTargetOfficers") ) {
		InterfaceStates.CharCapTargetOfficers = optref.cameramode.CharCapTargetOfficers;
	} else {
		InterfaceStates.CharCapTargetOfficers = false;
	}
	if( CheckAttribute(optref,"cameramode.CharCapLandBars") ) {
		InterfaceStates.CharCapLandBars = optref.cameramode.CharCapLandBars;
	} else {
		InterfaceStates.CharCapLandBars = 1;
	}
	NPCShowName(sti(InterfaceStates.CharCapShowOff), sti(InterfaceStates.CharCapShowOthers), sti(InterfaceStates.CharCapTargetOfficers));
	
	if( CheckAttribute(optref,"cameramode.ShipCap") ) {
		InterfaceStates.ShipCap = optref.cameramode.ShipCap;
	} else {
		InterfaceStates.ShipCap = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShipNames1") ) {
		InterfaceStates.ShipCapShipNames1 = optref.cameramode.ShipCapShipNames1;
	} else {
		InterfaceStates.ShipCapShipNames1 = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShipNames2") ) {
		InterfaceStates.ShipCapShipNames2 = optref.cameramode.ShipCapShipNames2;
	} else {
		InterfaceStates.ShipCapShipNames2 = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShowActions") ) {
		InterfaceStates.ShipCapShowActions = optref.cameramode.ShipCapShowActions;
	} else {
		InterfaceStates.ShipCapShowActions = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShowActionsPlus") ) {
		InterfaceStates.ShipCapShowActionsPlus = optref.cameramode.ShipCapShowActionsPlus;
	} else {
		InterfaceStates.ShipCapShowActionsPlus = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShowAllies") ) {
		InterfaceStates.ShipCapShowAllies = optref.cameramode.ShipCapShowAllies;
	} else {
		InterfaceStates.ShipCapShowAllies = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShowOther") ) {
		InterfaceStates.ShipCapShowOther = optref.cameramode.ShipCapShowOther;
	} else {
		InterfaceStates.ShipCapShowOther = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShowCrit") ) {
		InterfaceStates.ShipCapShowCrit = optref.cameramode.ShipCapShowCrit;
	} else {
		InterfaceStates.ShipCapShowCrit = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShowFortGun") ) {
		InterfaceStates.ShipCapShowFortGun = optref.cameramode.ShipCapShowFortGun;
	} else {
		InterfaceStates.ShipCapShowFortGun = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShowDamage") ) {
		InterfaceStates.ShipCapShowDamage = optref.cameramode.ShipCapShowDamage;
	} else {
		InterfaceStates.ShipCapShowDamage = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapShowDamageSelf") ) {
		InterfaceStates.ShipCapShowDamageSelf = optref.cameramode.ShipCapShowDamageSelf;
	} else {
		InterfaceStates.ShipCapShowDamageSelf = true;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapNotifDistance") ) {
		InterfaceStates.ShipCapNotifDistance = optref.cameramode.ShipCapNotifDistance;
	} else {
		InterfaceStates.ShipCapNotifDistance = 0.112;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapNotifScale") ) {
		InterfaceStates.ShipCapNotifScale = optref.cameramode.ShipCapNotifScale;
	} else {
		InterfaceStates.ShipCapNotifScale = 0.3;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapNamesScale") ) {
		InterfaceStates.ShipCapNamesScale = optref.cameramode.ShipCapNamesScale;
	} else {
		InterfaceStates.ShipCapNamesScale = 0.3;
	}
	if( CheckAttribute(optref,"cameramode.ShipCapIconsScale") ) {
		InterfaceStates.ShipCapIconsScale = optref.cameramode.ShipCapIconsScale;
	} else {
		InterfaceStates.ShipCapIconsScale = 0.3;
	}
	ShipShowName();
	
	if( CheckAttribute(optref,"cameramode.ParticlesCount") ) {
		InterfaceStates.ParticlesCount = optref.cameramode.ParticlesCount;
	} else {
		InterfaceStates.ParticlesCount = 0.0;
	}
	
	if( CheckAttribute(optref,"cameramode.ShowBoardMode") ) {
		InterfaceStates.ShowBoardMode = optref.cameramode.ShowBoardMode;
	} else {
		InterfaceStates.ShowBoardMode = false;
	}
	
	if( CheckAttribute(optref,"cameramode.CharVoice") ) {
		InterfaceStates.CharVoice = optref.cameramode.CharVoice;
	} else {
		InterfaceStates.CharVoice = false;
	}
	
	if( CheckAttribute(optref,"cameramode.EnableSailors") ) {
		InterfaceStates.EnableSailors = optref.cameramode.EnableSailors;
	} else {
		InterfaceStates.EnableSailors = false;
	}


	// mouse
	if( CheckAttribute(optref,"cameramode.InvertCameras") ) {
		InterfaceStates.InvertCameras = optref.cameramode.InvertCameras;
	} else {
		InterfaceStates.InvertCameras = false;
	}
	if( CheckAttribute(optref,"mouse.x_sensitivity") ) {
		InterfaceStates.mouse.x_sens = optref.mouse.x_sensitivity;
	} else {
		InterfaceStates.mouse.x_sens = 0.5;
	}
	if( CheckAttribute(optref,"mouse.y_sensitivity") ) {
		InterfaceStates.mouse.y_sens = optref.mouse.y_sensitivity;
	} else {
		InterfaceStates.mouse.y_sens = 0.5;
	}
	// video colors
	if( CheckAttribute(optref,"video.contrast") ) {
		InterfaceStates.video.contrast = optref.video.contrast;
	} else {
		InterfaceStates.video.contrast = 1.0;
	}
	if( CheckAttribute(optref,"video.gamma") ) {
		InterfaceStates.video.gamma = optref.video.gamma;
	} else {
		InterfaceStates.video.gamma = 1.0;
	}
	if( CheckAttribute(optref,"video.brightness") ) {
		InterfaceStates.video.brightness = optref.video.brightness;
	} else {
		InterfaceStates.video.brightness = 0.0;
	}
	if( CheckAttribute(optref,"menu") ) {
		InterfaceStates.menu = optref.menu;
	} else {
		InterfaceStates.menu = 1;
	}

	if( CheckAttribute(optref,"video.grassquantity") ) {
		iGrassQuality = sti(optref.video.grassquantity);
	}
	if( CheckAttribute(optref,"video.FlagAllWdm") ) {
		iFlagAllWdm = sti(optref.video.FlagAllWdm);
	}
	if( CheckAttribute(optref,"seadetails") ) {
		InterfaceStates.SeaDetails = stf(optref.seadetails);
	}
	if( CheckAttribute(optref,"GrassDistanceLand") ) {
		InterfaceStates.GrassDistanceLand = stf(optref.GrassDistanceLand);
	}	else {
		InterfaceStates.GrassDistanceLand = 0.33;
	}
	if( CheckAttribute(optref,"GrassDistanceSea") ) {
		InterfaceStates.GrassDistanceSea = stf(optref.GrassDistanceSea);
	}	else {
		InterfaceStates.GrassDistanceSea = 0.33;
	}
	if( CheckAttribute(optref,"GrassCountLand") ) {
		InterfaceStates.GrassCountLand = stf(optref.GrassCountLand);
	}	else {
		InterfaceStates.GrassCountLand = 0.33;
	}
	if( CheckAttribute(optref,"GrassCountSea") ) {
		InterfaceStates.GrassCountSea = stf(optref.GrassCountSea);
	}	else {
		InterfaceStates.GrassCountSea = 0.33;
	}
	// Gregg: растительность на островах. Умолчания едины со скриптом отправки в движок
	// (utils.c ApplyIslandVegetation): смешанная, густота средняя, полоса «склон»
	if( CheckAttribute(optref,"IslandVegType") ) {
		InterfaceStates.IslandVegType = sti(optref.IslandVegType);
	}	else {
		InterfaceStates.IslandVegType = 1;
	}
	if( CheckAttribute(optref,"IslandVegDensity") ) {
		InterfaceStates.IslandVegDensity = sti(optref.IslandVegDensity);
	}	else {
		InterfaceStates.IslandVegDensity = 1;
	}
	if( CheckAttribute(optref,"IslandVegBand") ) {
		InterfaceStates.IslandVegBand = sti(optref.IslandVegBand);
	}	else {
		InterfaceStates.IslandVegBand = 1;
	}
    //#20171223-01 Camera perspective option
	if( CheckAttribute(optref,"perspdetails") ) {
		InterfaceStates.PerspDetails = stf(optref.perspdetails);
	}
	//#20190815-01
	if( CheckAttribute(optref,"persp2details") ) {
		InterfaceStates.Persp2Details = stf(optref.persp2details);
	}
	if(!CheckAttribute(InterfaceStates,"RadDetails"))
	{		
		if( CheckAttribute(optref,"raddetails") ) {
			InterfaceStates.RadDetails = stf(optref.raddetails);
		}
	}
	if(!CheckAttribute(InterfaceStates,"DialogStyle"))
    {        
        if( CheckAttribute(optref,"DialogStyle") ) {
            InterfaceStates.DialogStyle = sti(optref.DialogStyle);
        } else { InterfaceStates.DialogStyle = 1; }
    }
	XI_SetColorCorrection(stf(InterfaceStates.video.contrast),stf(InterfaceStates.video.gamma),stf(InterfaceStates.video.brightness));

	RestoreKeysFromOptions(optref);

	// always run
	if( CheckAttribute(&optref,"alwaysrun") ) {
		InterfaceStates.alwaysrun = optref.alwaysrun;
	} else {
		InterfaceStates.alwaysrun = false;
	}

	ControlsMakeInvert();
	SetRealMouseSensitivity();
	SetPerspectiveSettings();
}

void ReadSavedOptions(ref gopt)
{
	string sFileName = "options";
	SendMessage(&GameInterface, "lsa", MSG_INTERFACE_LOADOPTIONS, sFileName, gopt);
}

void SaveSavedOptions(ref gopt)
{
	string sFileName = "options";
	SendMessage(&GameInterface, "lsa", MSG_INTERFACE_SAVEOPTIONS, sFileName, gopt);
}

void GetControlsOptions(ref optref)
{
	int ng,qg, nc,qc;
	aref arIn, arOut, arGrp, arCntrl;
	string grName, cntrlName;

	makearef(arOut,objControlsState.keygroups);
	optref.musket_key_migrated = true;
	optref.controls = true;
	makearef(arIn,optref.controls);

	qg = GetAttributesNum(arOut);
	for(ng=0; ng<qg; ng++)
	{
		arGrp = GetAttributeN(arOut,ng);
		grName = GetAttributeName(arGrp);
		if(!CheckAttribute(&objControlsState,"grouplist."+grName)) continue;
		if(sti(objControlsState.grouplist.(grName))!=0)
		{
			qc = GetAttributesNum(arGrp);
			for(nc=0; nc<qc; nc++)
			{
				arCntrl = GetAttributeN(arGrp,nc);
				if(!CheckAttribute(arCntrl,"remapping")) continue;
				if(sti(arCntrl.remapping)!=0)
				{
					cntrlName = GetAttributeName(arCntrl);
					arIn.(grName).(cntrlName) = GetAttributeValue(arCntrl);
					arIn.(grName).(cntrlName).state = arCntrl.state;
				}
			}
		}
	}
}
