bool	bAbordageStarted = false;
bool	bAbordageFortCanBe;
bool	bAbordagePlaceShipNear = false;
bool	bMCAbordageInitiator = true;
string	sAbordageLocator;
int		iAbordageFortEnemyCharacter;
int		iAbordageCharacter;
int		iAbordageMode;
float	fOldMaxSeaHeight;

void DeleteAbordageEnvironment()
{
	DelEventHandler(ABORDAGE_CAN_BE,"Abordage_CanBe");
}

void CreateAbordageEnvironment()
{
	bAbordageStarted = false;
	SetEventHandler(ABORDAGE_CAN_BE,"Abordage_CanBe",0);
	bAbordageFortCanBe = false;
}

void Abordage_CanBe()
{

}

void Abordage_ReloadStartFade()
{
	int a = GetEventData();
	aref reload_fader = GetEventData();
	LayerFreeze(SEA_EXECUTE,true);
	LayerFreeze(SEA_REALIZE,true);
	LayerFreeze(SEA_REFLECTION2,true);
	LayerFreeze(EXECUTE,false);
	LayerFreeze(REALIZE,false);

	MoveWeatherToLayers(EXECUTE, REALIZE);
	MoveSeaToLayers(EXECUTE, REALIZE);
    HideGrass();

	SendMessage(&AIBalls, "l", MSG_MODEL_RELEASE);
	// LEO: Сохраняем защиту пары до остановки морской сцены и удаления снарядов.
	if (Boarding_Busy(pchar)) SendMessage(&AISea, "la", AI_MESSAGE_SHIP_BOARDING_END, pchar);

	DelEventHandler("FaderEvent_StartFade", "Abordage_ReloadStartFade");

	fOldMaxSeaHeight = stf(Sea.MaxSeaHeight);
	Sea.MaxSeaHeight = 1.2;						// set maxinum sea height for ship abordage
}

void Go2LocationAfterAbordage()
{
	Sea.MaxSeaHeight = fOldMaxSeaHeight;		// restore old MaxSeaHeight
	bAbordageStarted = false;
	// boal -->
	bCabinStarted    = false;
	bDeckBoatStarted = false;
	// boal <--
	bSeaReloadStarted = false;
	Sea.AbordageMode = false;

    SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // boal
	LayerFreeze(SEA_REFLECTION2, false);

	DeleteAttribute(pchar, "abordage_active");
}

void Return2SeaAfterAbordage()
{
	LayerFreeze(EXECUTE, true);
	LayerFreeze(REALIZE, true);

	LayerFreeze(SEA_REFLECTION2, false);
	LayerFreeze(SEA_EXECUTE, false);
	LayerFreeze(SEA_REALIZE, false);

	MoveWeatherToLayers(SEA_EXECUTE, SEA_REALIZE);
	MoveSeaToLayers(SEA_EXECUTE, SEA_REALIZE);
    ShowGrass();

	Sea.MaxSeaHeight = fOldMaxSeaHeight;		// restore old MaxSeaHeight
	bAbordageStarted = false;
	Sea.AbordageMode = false;
    
	if (!IsEntity(BattleInterface))
	{
		InitBattleInterface();
		StartBattleInterface();
		RefreshBattleInterface();
	}

	SetSchemeForSea();

    SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // boal

	bSeaReloadStarted = false;
	SeaTow_Resume();
    //#20190117-03
	Ship_RecreateStaticSounds();
	DeleteAttribute(pchar, "abordage_active");
}

void Abordage_ReloadEndFade()
{
	float fHP, fHP1, fHP2;

	// Delete current cannonballs
	AIBalls.Clear = "";

    ResetSound(); // new

	if (bAbordagePlaceShipNear)
	{
		SendMessage(AISea, "laff", AI_MESSAGE_SAIL_2_CHARACTER, &Characters[iAbordageCharacter], 40.0 + frnd() * 60.0, frnd() * PIm2);
	}


	// start abordage location
	int a = GetEventData();
	aref reload_fader = GetEventData();
	switch (iAbordageMode)
	{
		case FORT_ABORDAGE:
			DeleteSeaEnvironment();
			LAi_StartBoarding(BRDLT_FORT, &Characters[iAbordageCharacter], true);
		break;

		case SHIP_ABORDAGE:
			// calc & apply abordage damage for ships
				fHP1 = stf(Characters[iAbordageCharacter].Ship.HP);
				fHP2 = stf(Characters[nMainCharacterIndex].Ship.HP);
				fHP = 0.1 + frnd() * 0.2;
				if (fHP1 > fHP2) { fHP = fHP * fHP2; }
				if (fHP1 < fHP2) { fHP = fHP * fHP1; }

				int AbordageStike;
				bSeaReloadStarted = false;
				AbordageStike = fHP * (1.05 - stf(Characters[iAbordageCharacter].TmpSkill.Sneak));
				Ship_ApplyHullHitpoints(&Characters[iAbordageCharacter], AbordageStike/1.5, KILL_BY_TOUCH, -1);
				AbordageStike = fHP * (1.05 - stf(Characters[nMainCharacterIndex].TmpSkill.Sneak))
				Ship_ApplyHullHitpoints(&Characters[nMainCharacterIndex], AbordageStike/1.5, KILL_BY_TOUCH, -1);
				bSeaReloadStarted = true;

			// load boarding
				LAi_StartBoarding(BRDLT_SHIP, &Characters[iAbordageCharacter], bMCAbordageInitiator);
		break;
	}
	DelEventHandler("FaderEvent_EndFade", "Abordage_ReloadEndFade");
	SendMessage(&reload_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);

	characters[iAbordageCharacter].abordage_twice = 1;
}

void Sea_AbordageLoad(int _iAbordageMode, bool _bMCAbordageInitiator)
{
	if (Boarding_Busy(pchar)) return;
	if (_iAbordageMode == FORT_ABORDAGE)
	{
		Sea_AbordageLoadResolved(FORT_ABORDAGE, _bMCAbordageInitiator, iAbordageFortEnemyCharacter);
		return;
	}

	int target = iAbordageShipEnemyCharacter;
	// LEO: Морские фантомы живут за пределами списка постоянных NPC.
	if (target < 0 || target >= TOTAL_CHARACTERS) return;
	ref enemy = GetCharacter(target);
	if (LAi_IsDead(enemy)) return;
	if (CheckAttribute(pchar,"GhostCap.Fight") && !CheckAttribute(pchar,"GenQuest.GhostShip.LastBattle")) return;
	if (CheckAttribute(enemy, "abordage_twice") && !CheckAttribute(enemy, "PGGAi.Task.SetSail"))
	{
		Log_SetStringToLog(XI_ConvertString("The Repeated boarding the ship is impossible"));
		return;
	}
	if (_bMCAbordageInitiator) Boarding_Begin(pchar, enemy, true, 0.0, 0.0);
	else Boarding_Begin(enemy, pchar, true, 0.0, 0.0);
}

bool Sea_AbordageLoadResolved(int _iAbordageMode, bool _bMCAbordageInitiator, int target)
{
	if (target < 0 || target >= TOTAL_CHARACTERS) return false;
	if (CheckAttribute(pchar,"GhostCap.Fight") && !CheckAttribute(pchar,"GenQuest.GhostShip.LastBattle")) return false;
	if(!CheckAttribute(pchar, "abordage_active"))
	{
		if( !CheckAttribute(pchar,"abordage_active_count") )
		{
			if (!CheckAttribute(&characters[target], "abordage_twice") || _iAbordageMode == FORT_ABORDAGE || CheckAttribute(&characters[target],"PGGAi.Task.SetSail"))
			{
				Sea_AbordageLoad_ActiveCountStart();

				pchar.boarding_info.mode = _iAbordageMode;
				pchar.boarding_info.indicator = _bMCAbordageInitiator;
				pchar.boarding_info.target = target;
				pchar.boarding_info.staged = (_iAbordageMode == SHIP_ABORDAGE && Boarding_Busy(pchar));

				// LEO: Автосейв хранит цель до перехода на палубу.
				if(AutoSave_Start("Board"))
				{
					SetEventHandler("evntSave","Continue_Sea_AbordageLoadPre", 0);
				}
				else
				{
					Continue_Sea_AbordageLoad();
				}
				return true;
			} else {
				Log_SetStringToLog(XI_ConvertString("The Repeated boarding the ship is impossible"));
			}
		}
	}

	return false;
}

void Sea_AbordageLoad_ActiveCountStart()
{
	pchar.abordage_active_count = 4;
	SetEventHandler("frame","Sea_AbordageLoad_ActiveCount",1);
}

void Sea_AbordageLoad_ActiveCount()
{
	int n = sti(pchar.abordage_active_count) - 1;
	if( n<=0 ) {
		DelEventHandler("frame","Sea_AbordageLoad_ActiveCount");
		DeleteAttribute(pchar,"abordage_active_count");
	} else {
		pchar.abordage_active_count = n;
	}
}

void Continue_Sea_AbordageLoadPre()
{
	DelEventHandler("evntSave","Continue_Sea_AbordageLoadPre");
	// LEO: Абордаж не проходит повторно через AutoSave_Start; снимаем флаг здесь.
	DeleteAttribute(&InterfaceStates, "AutoSaveJustDone");
	SetEventHandler("frame","Continue_Sea_AbordageLoad",1);
}

void Continue_Sea_AbordageLoad()
{
	DelEventHandler("frame","Continue_Sea_AbordageLoad");
	if (!CheckAttribute(pchar, "boarding_info.mode")) return;
	int _iAbordageMode = sti(pchar.boarding_info.mode);
	bool _bMCAbordageInitiator = sti(pchar.boarding_info.indicator) != 0;

	int _iAbordageCharacter = iAbordageShipEnemyCharacter;
	if (_iAbordageMode == FORT_ABORDAGE)
	{
		_iAbordageCharacter = iAbordageFortEnemyCharacter;
	}

	if (CheckAttribute(pchar, "boarding_info.target")) _iAbordageCharacter = sti(pchar.boarding_info.target);
	DeleteAttribute(pchar, "boarding_info");
	if (!bSeaActive) return;
	if (_iAbordageCharacter < 0 || _iAbordageCharacter >= TOTAL_CHARACTERS ||
		(_iAbordageMode == SHIP_ABORDAGE && LAi_IsDead(GetCharacter(_iAbordageCharacter))))
	{
		if (Boarding_Busy(pchar)) SendMessage(&AISea, "la", AI_MESSAGE_SHIP_BOARDING_END, pchar);
		return;
	}
	if (!CheckAttribute(&characters[_iAbordageCharacter], "abordage_twice") || _iAbordageMode == FORT_ABORDAGE || CheckAttribute(&characters[_iAbordageCharacter],"PGGAi.Task.SetSail"))
	{
		Sea_AbordageStartNow(_iAbordageMode, _iAbordageCharacter, false, _bMCAbordageInitiator);
	}
	else
	{
		if (Boarding_Busy(pchar)) SendMessage(&AISea, "la", AI_MESSAGE_SHIP_BOARDING_END, pchar);
		Log_SetStringToLog(XI_ConvertString("The Repeated boarding the ship is impossible"));
	}
}

void Sea_AbordageStartNow(int _iAbordageMode, int _iAbordageCharacter, bool _bPlaceOurShipNear, bool _bMCAbordageInitiator)
{
	bSeaReloadStarted = true;

	// Если ГГ был в тактическом режиме (вид сверху/пауза) — корректно свернуть его,
	// иначе тактический HUD, метки и пауза «зависнут» при переходе в абордаж.
	if (Sea_InTacticalView()) Sea_TacticalViewOff();

	iAbordageCharacter = _iAbordageCharacter;
	iAbordageMode = _iAbordageMode;
	bAbordagePlaceShipNear = _bPlaceOurShipNear;
	bMCAbordageInitiator = _bMCAbordageInitiator;

	object reload_fader;
	CreateEntity(&reload_fader, "fader");
	SetEventHandler("FaderEvent_StartFade", "Abordage_ReloadStartFade", 0);
	SetEventHandler("FaderEvent_EndFade", "Abordage_ReloadEndFade", 0);

	pchar.loadscreen = LAi_GetBoardingImage(GetCharacter(iAbordageCharacter), bMCAbordageInitiator);
	SendMessage(&reload_fader, "ls", FADER_PICTURE0, pchar.loadscreen);
	SendMessage(&reload_fader, "lfl", FADER_OUT, 2.0, false);
	SendMessage(&reload_fader, "l", FADER_STARTFRAME);

	// boal -->
 	ClearAllFire();
    // boal <--
	PauseAllSounds();
    PlayVoice("INTERFACE\"+LanguageGetLanguage()+"_GTBoard_"+rand(6)+".wav"); // boal
    pchar.abordage_active = 1;  // чтоб два раза подряд не жали

	Event(ABORDAGE_START_EVENT, "ll", nMainCharacterIndex, iAbordageCharacter);

	bAbordageStarted = true;
	Sea.AbordageMode = true;

	if(_bMCAbordageInitiator != true && _iAbordageMode != FORT_ABORDAGE)
	{
		characters[_iAbordageCharacter].abordage_iniciator = 1;
	}
}

void MakeAutoSaveAndGoOnAbord()
{
    MakeAutoSave();
	//eddy. задержка вызова абордажа
	pchar.GenQuest.CallFunctionParam = "Continue_Sea_AbordageLoadPre";
	DoQuestCheckDelay("CallFunctionParam", 2.0);
}
