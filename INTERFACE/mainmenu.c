string isUsersName="";

//ref rCharacter; // boal
int iChar;
#event_handler("Control Activation","TempIControlProcess");

#define MIN_FAKE_FIRE_DELAY 2
#define MAX_FAKE_FIRE_DELAY 10

// LEO: записка не застывает раз и навсегда - раз в HINT_ROTATE_MS подсказка в ней меняется.
//   Сам виджет при этом стоит на месте: гаснет и проявляется только текст, через альфу строк.
#define HINT_ROTATE_MS		15000	// как часто менять подсказку, мс
#define HINT_FADE_STEPS		10		// ступеней затухания и столько же проявления
#define HINT_FADE_MS		60		// длительность одной ступени, мс (12 x 60 = ~0.7 сек в каждую сторону)

object HintAnim;	// какая подсказка сейчас в какой записке (живёт только пока открыто меню)

void TempIControlProcess()
{
	string ControlName;
	ControlName = GetEventData();
	if (ControlName == "WhrPrevWeather")
	{
		Sea.MaxSeaHeight = 1.0;
		Sea.isDone = "";
	}
	if (ControlName == "WhrNextWeather")
	{
		Sea.MaxSeaHeight = 1.0;
		Sea.isDone = "";
	}
	if (ControlName == "WhrUpdateWeather")
	{
		Sea.MaxSeaHeight = 1.0;
		Sea.isDone = "";
	}
}

void InitInterface(string iniName)
{
	Event("DoInfoShower","sl","MainMenuLaunch",false);

	aref arScrShoter;
	if( !GetEntity(&arScrShoter,"scrshoter") ) {
		CreateScreenShoter();
	}

	CreateBackEnvironment();
	SetEventHandler("DoMainMenuFakeFire","DoMainMenuFakeFire",0);
	PostEvent("DoMainMenuFakeFire", (MIN_FAKE_FIRE_DELAY+rand(3))*1000);

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	// boal ver num -->
    CreateString(true,"VerNum", VERSION_NUMBER1 + GetVerNum(), FONT_NORMAL, COLOR_NORMAL, 240, 580, SCRIPT_ALIGN_RIGHT, 1.2);
    // boal ver num <--

	SetEventHandler("backgroundcommand","ProcCommand",0);

	GameInterface.SavePath = "SAVE";

	ResetSound(); // new
	// ВВОД СВОИХ СХЕМ В ЗАВИСИМОСТИ ОТ ПОГОДЫ (BY LOKK)
	if (Whr_IsRain())
	{ if (Whr_IsNight())
		{
			SetSoundScheme("mainmenu_night_rain");
		}
		else
		{
			SetSoundScheme("mainmenu_day_rain");
		}
    }
	else
	{ if (Whr_IsNight())
		{
			SetSoundScheme("mainmenu_night");
		}
		else
		{
			SetSoundScheme("mainmenu_day");
		}

	}
	oldMusicID = 0;
	musicName = "";
	MainMenuUpdateRPC();
	SetMusic("music_MainMenu");
	SetEventHandler("ShowChangesWindow","ShowChangesWindow",0);
	// LEO: покачивание записок и смена подсказок на месте
	DeleteAttribute(&HintAnim, "");
	SetEventHandler("HintRotate","HintRotateStart",0);
	SetEventHandler("HintFade","HintFadeStep",0);
	PostEvent("ShowChangesWindow", 500, "l", 1);//задержка перед 1 запиской
	PostEvent("ShowChangesWindow", 3000, "l", 2);//задержка перед 2 запиской
	PostEvent("ShowChangesWindow", 5500, "l", 3);//задержка перед 3 запиской
}

void NewGamePress()
{
	IDoExit( RC_INTERFACE_DO_NEW_GAME, true );
}

void LoadPress()
{
	IDoExit( RC_INTERFACE_DO_LOAD_GAME, false );
}

void SavePress()
{
	IDoExit( RC_INTERFACE_DO_SAVE_GAME, false );
}

void OptionsPress()
{
	IDoExit( RC_INTERFACE_DO_OPTIONS, false );
}

void ControlsPress()
{
	IDoExit( RC_INTERFACE_DO_CONTROLS, false );
}

void CreditsPress()
{
	IDoExit( RC_INTERFACE_DO_CREDITS, false );
}

void QuitPress()
{
    DelEventHandler("frame","QuitPress");
	DeleteBackEnvironment();
	EngineLayersOffOn(false);
	IDoExit(-1, false);
	ExitProgram();
}

void procGameQuit()
{
    DeleteBackEnvironment();
	ExitProgram();
}

void MultiPress()
{
	//PrepareNetEnvironment();
	DeleteBackEnvironment();
	IDoExit(RC_INTERFACE_NET_FINDGAME, false);
	InterfaceStates.BackEnvironmentIsCreated = false;
}
void IDoExit(int exitCode, bool bClear)
{
	InterfaceStates.BackEnvironmentIsCreated = true;

	DelEventHandler("backgroundcommand","ProcCommand");
	DelEventHandler("ShowChangesWindow","ShowChangesWindow");
	DelEventHandler("DoMainMenuFakeFire","DoMainMenuFakeFire");
	// LEO: анимация записок - снимаем обработчики и базу, иначе отложенные события будут стучаться
	// в узлы уже закрытого интерфейса
	DelEventHandler("HintRotate","HintRotateStart");
	DelEventHandler("HintFade","HintFadeStep");
	DeleteAttribute(&HintAnim, "");

	interfaceResultCommand = exitCode;
	EndCancelInterface(bClear);
}

void ExitCancel()
{
	IDoExit(-1,true);
}

void ProcCommand()
{
	string comName = GetEventData();

	switch(comName)
	{
	case "New":			NewGamePress(); break;
	case "Load":		LoadPress(); break;
	case "Save":		SavePress(); break;
	case "Options":		OptionsPress(); break;
	case "Credits":		CreditsPress(); break;
	case "Exit":		SetEventHandler("frame","QuitPress",0);  break;
	}
}

object InterfaceBackScene;
void CreateBackEnvironment()
{
	LayerFreeze(EXECUTE,false);
	LayerFreeze(REALIZE,false);

	if( CheckAttribute(&InterfaceStates,"BackEnvironmentIsCreated") && InterfaceStates.BackEnvironmentIsCreated=="1" ) {
		return;
	}

	// сбрасываем цвет фона на 0
	Render.BackColor = 0;
	// выключаем эффект моря
	Render.SeaEffect = false;
	// 14.07.2007 - отключаем подводную часть
	Sea.UnderWater = false;
	bMainCharacterInFire = false;
	bMainMenu = true;

	// create weather
	ICreateWeather();

	CreateEntity(&InterfaceBackScene,"InterfaceBackScene");
	LayerAddObject(EXECUTE, &InterfaceBackScene, -1);
	LayerAddObject(REALIZE, &InterfaceBackScene, 1000);

	SendMessage(&InterfaceBackScene, "ls", 0, "MainMenu\MainMenu"); // set model
	SendMessage(&InterfaceBackScene, "ls", 1, "camera"); // set camera

	aref arMenu;
	makearef(arMenu,InterfaceBackScene.menu);
	arMenu.l1.locname = "menu02";	arMenu.l1.sel = "mainmenu\menu02_active";	arMenu.l1.norm = "mainmenu\menu02_passive"; arMenu.l1.event = "New";			arMenu.l1.path = LanguageGetLanguage();
	arMenu.l2.locname = "menu03";	arMenu.l2.sel = "mainmenu\menu03_active";	arMenu.l2.norm = "mainmenu\menu03_passive"; arMenu.l2.event = "Load";			arMenu.l2.path = LanguageGetLanguage();
	arMenu.l3.locname = "menu04";	arMenu.l3.sel = "mainmenu\menu04_active";	arMenu.l3.norm = "mainmenu\menu04_passive"; arMenu.l3.event = "Options";		arMenu.l3.path = LanguageGetLanguage();
	arMenu.l4.locname = "menu05";	arMenu.l4.sel = "mainmenu\menu05_active";	arMenu.l4.norm = "mainmenu\menu05_passive"; arMenu.l4.event = "Credits";		arMenu.l4.path = LanguageGetLanguage();
	arMenu.l5.locname = "menu06";	arMenu.l5.sel = "mainmenu\menu06_active";	arMenu.l5.norm = "mainmenu\menu06_passive"; arMenu.l5.event = "Exit";			arMenu.l5.path = LanguageGetLanguage();
	arMenu.l6.locname = "AOPBoard";
 	arMenu.l6.event = "";				arMenu.l6.path = LanguageGetLanguage();
	SendMessage(&InterfaceBackScene, "lla", 3, 1, arMenu ); // set menu


	// create ship
	MainMenu_CreateShip();

	if( Whr_IsNight() ) {
		InterfaceBackScene.light.turnon = true;
		InterfaceBackScene.light.model = "mainmenu\Fonar_night";
		InterfaceBackScene.light.lightcolormin = argb(0,200,200,120);//argb(255,114,114,80);
		InterfaceBackScene.light.lightcolormax = argb(48,255,255,180);
		InterfaceBackScene.light.colorperiod = 0.4;
		InterfaceBackScene.light.addcolorperiod = 1.0;
		InterfaceBackScene.light.rangemin = 10.0;
		InterfaceBackScene.light.rangemax = 14.0;
		InterfaceBackScene.light.rangeperiod = 1.5;
		InterfaceBackScene.light.locator = "Light";
		InterfaceBackScene.light.lightlocator = "fonar";
		InterfaceBackScene.light.flarelocator = "fire";
		InterfaceBackScene.light.flaresize = 0.5;
		InterfaceBackScene.light.minflarecolor = 120.0;
		InterfaceBackScene.light.maxflarecolor = 200.0;
	} else {
		if (!Whr_IsStorm()&& !Whr_IsRain()) MainMenu_CreateAnimals(); // теперь в дождь и в шторм животных нет

		InterfaceBackScene.light.turnon = false;
		InterfaceBackScene.light.model = "mainmenu\Fonar_day";
		InterfaceBackScene.light.locator = "Light";
		InterfaceBackScene.light.lightlocator = "fonar";
	}
	SendMessage(&InterfaceBackScene, "ls", 8, "light" );

	if( Whr_IsNight() )
	{
		// create particles
		InitParticles();
		CreateParticleSystem("candle", stf(InterfaceBackScene.lightpos.x),stf(InterfaceBackScene.lightpos.y),stf(InterfaceBackScene.lightpos.z), 0.0,0.0,0.0, 0);
	}
	bMainMenu = false;
}

void ShowChangesWindow()
{
	int number = GetEventData();//ивент понимает какую записку передаёт
	switch (number)
	{
		case 1://координаты 1 записки
			AddNodeToMoveList("CHANGES_FRAME_WINDOW"+number,-684,3,-534,172);
			AddNodeToMoveList("CHANGES_CAPTION"+number,-684,2,-534,14);
			AddNodeToMoveList("CHANGES_PAPER"+number,-711,15,-508,173);
			AddNodeToMoveList("CHANGES_TEXT"+number,-675,20,-542,169);
		break;
		case 2://координаты 2 записки
			AddNodeToMoveList("CHANGES_FRAME_WINDOW"+number,-684,175,-534,344);
			AddNodeToMoveList("CHANGES_CAPTION"+number,-684,174,-534,186);
			AddNodeToMoveList("CHANGES_PAPER"+number,-711,187,-508,345);
			AddNodeToMoveList("CHANGES_TEXT"+number,-675,192,-542,341);
		break;
		case 3://координаты 3 записки
			AddNodeToMoveList("CHANGES_FRAME_WINDOW"+number,-684,347,-534,516);
			AddNodeToMoveList("CHANGES_CAPTION"+number,-684,346,-534,358);
			AddNodeToMoveList("CHANGES_PAPER"+number,-711,359,-508,517);
			AddNodeToMoveList("CHANGES_TEXT"+number,-675,364,-542,513);
		break;
	}
	XI_WindowShow("CHANGES_WINDOW"+number, true);
	XI_WindowDisable("CHANGES_WINDOW"+number, false);
	
	MoveAllNodesToDirSoftly("right", 10, 550, "open");//10 - вроде бы скорость, 550 - расстояние
	PlaySound("Interface\MAGIC_Whoosh.wav");
	
	int tmpLangFileID = LanguageOpenFile("Hints.txt");
	
	SetFormatedText("CHANGES_CAPTION"+number, XI_ConvertString("Hints"));
	int hintnum;
	// LEO: каждая записка берёт подсказку из своей трети Hints.txt. Границы держать в согласии с
	//   файлом: прежние (30/31+30/61+32) оставляли последнюю подсказку непоказанной вовсе, а
	//   Hints_61 выпадал сразу в двух записках.
	switch (number)
	{
		case 1: hintnum = rand(43); break;		//тексты 1 записки, иначе не работают переносы
		case 2: hintnum = 44+rand(42); break;	//тексты 2 записки, иначе не работают переносы
		case 3: hintnum = 87+rand(47); break;	//тексты 3 записки, иначе не работают переносы
	}
	SetFormatedText("CHANGES_TEXT"+number, LanguageConvertString(tmpLangFileID, "Hints_"+hintnum));
	SetAlignmentFormatedText("CHANGES_TEXT"+number, SCRIPT_ALIGN_LEFT);
	
	LanguageCloseFile(tmpLangFileID);

	// LEO: заводим смену подсказок в этой записке
	string sHint = "note" + number;
	HintAnim.(sHint).hint = hintnum;
	PostEvent("HintRotate", HINT_ROTATE_MS, "l", number);
}

// Подсказка из своей трети файла, но не та же самая, что висит сейчас.
int HintPickNumber(int number, int iCurrent)
{
	int iNew = iCurrent;
	int iGuard = 0;
	while (iNew == iCurrent && iGuard < 8)
	{
		if (number == 1) iNew = rand(43);
		if (number == 2) iNew = 44 + rand(42);
		if (number == 3) iNew = 87 + rand(47);
		iGuard++;
	}
	return iNew;
}

// Цвет строк подсказки: гасим и проявляем текст через альфу, сам виджет при этом не трогаем.
void HintSetTextAlpha(int number, int iAlpha)
{
	SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "CHANGES_TEXT" + number, 8, 0,
		argb(iAlpha, 255, 255, 255));
}

// Пора менять подсказку: запускаем затухание. Виджет остаётся на месте, едет только текст по альфе.
void HintRotateStart()
{
	int number = GetEventData();
	if (!CheckAttribute(&HintAnim, "note" + number + ".hint")) return;	// меню уже закрыли
	PostEvent("HintFade", HINT_FADE_MS, "lll", number, HINT_FADE_STEPS, 0);
}

// Ступень плавной смены: сначала гасим до нуля, на дне подменяем текст, потом проявляем обратно.
void HintFadeStep()
{
	int number = GetEventData();
	int iStep = GetEventData();
	int iUp = GetEventData();
	int hintnum;
	int tmpLangFileID;
	string sHint = "note" + number;
	if (!CheckAttribute(&HintAnim, sHint + ".hint")) return;

	if (iUp == 0)
	{
		iStep--;
		if (iStep > 0)
		{
			HintSetTextAlpha(number, makeint(255.0 * iStep / HINT_FADE_STEPS));
			PostEvent("HintFade", HINT_FADE_MS, "lll", number, iStep, 0);
			return;
		}
		// дно: текста не видно - самое время подменить строку
		HintSetTextAlpha(number, 0);
		hintnum = HintPickNumber(number, sti(HintAnim.(sHint).hint));
		HintAnim.(sHint).hint = hintnum;

		tmpLangFileID = LanguageOpenFile("Hints.txt");
		SetFormatedText("CHANGES_TEXT" + number, LanguageConvertString(tmpLangFileID, "Hints_" + hintnum));
		SetAlignmentFormatedText("CHANGES_TEXT" + number, SCRIPT_ALIGN_LEFT);
		LanguageCloseFile(tmpLangFileID);
		HintSetTextAlpha(number, 0);	// новый текст приходит непрозрачным - прячем его снова

		PostEvent("HintFade", HINT_FADE_MS, "lll", number, 0, 1);
		return;
	}

	iStep++;
	HintSetTextAlpha(number, makeint(255.0 * iStep / HINT_FADE_STEPS));
	if (iStep < HINT_FADE_STEPS)
	{
		PostEvent("HintFade", HINT_FADE_MS, "lll", number, iStep, 1);
		return;
	}
	PostEvent("HintRotate", HINT_ROTATE_MS, "l", number);	// показали - ждём до следующей смены
}

void MainMenu_CreateShip()
{
	//int
	iChar = GenerateCharacter(rand(4), WITHOUT_SHIP, "citizen", MAN, 0, WARRIOR);   //PIRATE
	ref	rCharacter = &characters[iChar];
	int nChoosedBaseShipType = 3 + rand(136);
	rCharacter.ship.type = GenerateShip(nChoosedBaseShipType, false);
	SetBaseShipData( rCharacter );
	ref refBaseShip = GetRealShip( sti(rCharacter.ship.type) );
	refBaseShip.SpeedRate = 1.0; // чтоб не летал
	Ship_SetFantomData( rCharacter );
	Ship_ClearImpulseStrength( rCharacter );
	Ship_SetLightsAndFlares( rCharacter );
	Ship_SetTrackSettings( rCharacter );

	rCharacter.Features.GeraldSails = true;
	rCharacter.Ship.Speed.z = 1.0;
	//rCharacter.Ship.Stopped = true;
	rCharacter.Ship.Pos.Mode = SHIP_SAIL;
	rCharacter.Ship.Pos.x = 0.0;
	rCharacter.Ship.Pos.z = 0.0;
	rCharacter.Ship.Ang.y = 0.0;
	rCharacter.Ship.Strand = false;
	rCharacter.Ship.Strand.SndID = -1;

	rCharacter.Ship.LastBallCharacter = -1;
	rCharacter.Ship.WindDotShip = 1.0;

	rCharacter.TmpPerks = "";
	aref aTmpPerks; makearef(aTmpPerks, rCharacter.TmpPerks);
	aTmpPerks.StormProfessional		= 0;

	SendMessage(&InterfaceBackScene, "lsa", 2, "ship", &rCharacter); // set ship position
	CreateEntity( rCharacter, "ship" );
	ref rBaseShip = GetRealShip(sti(rCharacter.ship.type));
	SendMessage( rCharacter, "laa", MSG_SHIP_CREATE, &rCharacter, &rBaseShip );
	mainMenuCharIdx = iChar;
}

void DoMainMenuFakeFire()
{
	SendMessage(&Characters[mainMenuCharIdx],"ls",MSG_SHIP_DO_FAKE_FIRE,"cannonl");
	PostEvent("DoMainMenuFakeFire", clamp(rand(MAX_FAKE_FIRE_DELAY), MIN_FAKE_FIRE_DELAY, MAX_FAKE_FIRE_DELAY)*1000);
}

void DeleteBackEnvironment()
{
	LayerDelObject(EXECUTE, &InterfaceBackScene);
	LayerDelObject(REALIZE, &InterfaceBackScene);

	MainMenu_DeleteAnimals();
	DeleteClass( GetCharacter(iChar));//  boal fix FANTOM_CHARACTERS) );
	DeleteShipEnvironment();
	Ship_Walk_Delete();
	DeleteWeather();
	DeleteSea();
	DeleteClass( InterfaceBackScene );
	DeleteCoastFoamEnvironment();

	iNextWeatherNum = -1;
	iCurWeatherHour = -1;
}

void MainMenu_CreateAnimals()
{
	InterfaceBackScene.seagull.locator = "seagull";
	InterfaceBackScene.seagull.model = "mainmenu\seagull";
	InterfaceBackScene.seagull.animation = "seagull";
	InterfaceBackScene.seagull.aniaction = "idle";
	SendMessage(&InterfaceBackScene, "ls", 9, "seagull" );
	InterfaceBackScene.amazon.locator = "amazon";
	InterfaceBackScene.amazon.model = "mainmenu\amazon";
	InterfaceBackScene.amazon.animation = "amazon";
	InterfaceBackScene.amazon.aniaction = "idle";
	SendMessage(&InterfaceBackScene, "ls", 9, "amazon" );
}

void MenuCreateLogo()
{
	InterfaceBackScene.Logo.locator = "";
	InterfaceBackScene.Logo.model = "mainmenu\AOPBoard";
	if( !Whr_IsNight() ) {
		InterfaceBackScene.Logo.technique = "InterfaceBackScene_Logo";
	}
	SendMessage(&InterfaceBackScene, "ls", 10, "Logo" );
}

void MainMenu_DeleteAnimals()
{
	if (IsEntity(Animals))
		DeleteClass(Animals);
}

void ICreateWeather()
{
	// choose random weather
	int n = 0;
	if( CheckAttribute(&InterfaceStates,"mainmenuweather") )
	{
		n = sti(InterfaceStates.mainmenuweather);
	}
	else
	{
		n = rand(iTotalNumWeathers-1);
		int oldN=n;
		while (true)
		{
			if( !CheckAttribute( &Weathers[n], "skip" ) || Weathers[n].skip!="1" )
			{
				break;
			}
			//candle
			n++;
			if( n==iTotalNumWeathers ) {n = 0;}
			if( n==oldN ) {break;}
		}
	}
	if (n < 0 || n >= iTotalNumWeathers) n = 0;
	SetNextWeather(Weathers[n].id);
	iBlendWeatherNum = -1; // залоченная погода
	Whr_LoadNextWeather(0);
	iBlendWeatherNum = -1; // залоченная погода
	InterfaceBackScene.current_weather = n;
	InterfaceStates.mainmenuweather    = n;

	Ship_Walk_Init();
	LayerFreeze(SEA_EXECUTE, false);
	LayerFreeze(SEA_REALIZE, false);

	CreateSea(EXECUTE, REALIZE);
    CreateWeather(EXECUTE, REALIZE);

	CreateShipEnvironment();
	Sea.MaxSeaHeight = 1.0;
	Sea.isDone = "";

	CreateCoastFoamEnvironment("MainMenu", EXECUTE, REALIZE);
	iBlendWeatherNum = -1; // залоченная погода
}
