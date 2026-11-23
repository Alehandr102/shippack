#include "storm-engine\interface\messages.h"
#include "interface\utils\interface.c"
#include "interface\utils\MessageBox.c"
#include "interface\utils\IconSelector.c"
#include "interface\utilite.c"
#include "interface\interface_utils.c"
#include "interface\perks\perks.c"

#define FONT_NORMAL	"interface_normal"
#define FONT_CAPTION	"interface_button"
#define FONT_BOLD_NUMBERS	"bold_numbers"

#define COLOR_DESELECT	4286611584
#define COLOR_NORMAL	4294967295
#define COLOR_MONEY		4292915296
#define COLOR_RED		4294901760
#define COLOR_WARNING	4294934656	// Gregg: argb(255,255,128,128) розово-красный — предупреждение/нехватка. ДЕЦИМАЛ (не argb-макрос): argb() не компайл-константа, ломала интерфейс-сегменты синтакс-ошибкой.
#define COLOR_OK		4286644096	// Gregg: argb(255,128,255,128) светло-зелёный — «ок»/достаточно (децимал-константа)

#define BLIND_MIN	4282400832 //argb(255,96,96,96)
#define BLIND_MAX	4286611584 //argb(255,128,128,128)

#define MONEY_SIGN		""
#define MONEY_DELIVER	" "

#define MAX_CHARACTER_SKILL		10

#define DISEASE_ON_SHIP	0
#define DISEASE_ON_COLONY	1




#event_handler("LaunchIAfterFrame","ILaunchAfterFrame");
#event_handler("ievent_GameOver","IProcEventGameOver");
#event_handler("ievent_SetGameTime","IProcEventSetGameTime");
#event_handler("evntPreLoad","procPreLoad");

#event_handler("FaderEvent_StartFade","ProcBreakInterface");
#event_handler("FaderEvent_StartFadeIn","ProcBreakInterface");
#event_handler("FaderEvent_EndFade","ProcBreakInterface");
#event_handler("FaderEvent_EndFadeIn","ProcBreakInterface");

#event_handler("Fader_GetTipsPath","procGetTipsPath");
#event_handler("Fader_GetFaderPicture","procGetFaderPicture");

#event_handler("evntOptionsBreak","procOptionsBreak");

#event_handler("evMouseWeel","procMouseWeel");
#event_handler("evGetMouseWeel","procGetMouseWeel");

bool EnableVideoBreak = false;

int CurrentInterface;
int gStoreNum;

extern void InitInterface(string iniFile);
extern void InitInterface_R(string iniFile, ref rParam);
extern void InitInterface_RR(string iniFile, ref rParam1, ref rParam2);
extern void InitInterface_S(string iniFile, string sParam1);
extern void InitInterface_GM(string iniFile);
extern void InitInterface_RS(string iniFile, ref rParam1, string sParam2);
extern void InitInterface_RI(string iniFile, ref rParam1, int iParam2);
extern void InitInterface_I(string iniFile, int iParam1);
extern void InitInterface_B(string iniFile, bool bParam1);
extern void InitInterface_BB(string iniFile, bool bParam1, bool bParam2);
extern void InitInterface_RRS(string iniFile, ref rParam1, ref rParam2, string sParam3);
extern void InitInterface_RIS(string iniFile, ref rParam1, int iParam2, string sParam3);


extern ref GetMyCharacterRef();
extern ref GetEnemyCharacterRef();
string ICurNode;
int	   nPrevInterface;

bool bGamePadChangeEnable = false;
bool bPlayVideoNow = false;
bool bMainMenuLaunchAfterVideo = false;

// Общий запуск интерфейсов без аргументов инициализации.
void IPrepare(int code)
{
	if(procInterfacePrepare(code))
	{
		nPrevInterface = -1;
		CurrentInterface = code;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchBestMapScreen() { IPrepare(INTERFACE_BEST_MAP); }	// Интерфейс отличной карты

void LaunchTavernWaitScreen() // Интерфейс отдыха
{
	if(procInterfacePrepare(INTERFACE_TAVERN_WAIT))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_TAVERN_WAIT;
		InitInterface_GM(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchTransferGoodsScreen() { IPrepare(INTERFACE_GOODS_TRANSFER); } // Интерфейс закупки товара казначеем
void LaunchTreasurerReport() { IPrepare(INTERFACE_TREASURER_REPORT); } // Gregg: отчёт казначея о рейде по городу

void LaunchMapViewScreen() { IPrepare(INTERFACE_MAPVIEW); }	// Интерфейс атласа карт

void LaunchStorage(int storageNum) // интерфейс склада
{
	if(storageNum < 0)	return;
	if(storageNum > STORE_QUANTITY - 1)	return;
	gStoreNum = storageNum;
	if(procInterfacePrepare(INTERFACE_STORAGE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_STORAGE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, &stores[storageNum]);
	}
}


void LaunchCreateNotice() { IPrepare(INTERFACE_CREATENOTICE); }

void LaunchAboutScreen() { IPrepare(INTERFACE_ABOUT); }

// LEO: запуск экрана «Журнал игры»
void LaunchGameLogScreen() { IPrepare(INTERFACE_GAMELOG); }

void LaunchNationLegend() { IPrepare(INTERFACE_NATION_LEGEND); }

void LaunchPsHeroScreen() { IPrepare(INTERFACE_PS_HERO); }

void LaunchLeaveBattleScreen() { IPrepare(INTERFACE_LEAVE_BATTLE); }
void LaunchFrameForm() { IPrepare(INTERFACE_FRAMEFORM); }

void LaunchCardsGame() { IPrepare(INTERFACE_CARDSGAME); }

void LaunchDiceGame() { IPrepare(INTERFACE_DICE_GAME); }
void LaunchDebuderMenu() { IPrepare(INTERFACE_DEBUGER); }

void LaunchBoalBetaScreen() { IPrepare(INTERFACE_BOAL_BETA); }
void LaunchGoodsSpawnScreen() { IPrepare(INTERFACE_GOODS_SPAWN); }
void LaunchShipChangeScreen() { IPrepare(INTERFACE_SHIP_CHANGE); }

void LaunchQuickSaveMenu() { IPrepare(INTERFACE_QUICK_SAVE); }

void HideQuickSaveMenu()
{

	interfaceResultCommand = RC_INTERFACE_QUICK_SAVE;
	EndCancelInterface(true);
}

void LaunchSalaryScreen(string _tmp)
{
	if(procInterfacePrepare(INTERFACE_SALARY))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SALARY;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchStoreHouse(int storeNum)
{
  	if(storeNum < 0)
  	{
  		return;
  	}

  	if(storeNum > STORE_QUANTITY - 1)
  	{
  		return;
	}

	gStoreNum = storeNum;

	if(procInterfacePrepare(INTERFACE_STOREHOUSE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_STOREHOUSE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,&stores[storeNum]);
	}
}


void LaunchPlantation() { IPrepare(INTERFACE_PLANTATION); }

void LaunchColony() { IPrepare(INTERFACE_COLONY); }

void LaunchAttackMessage() { IPrepare(INTERFACE_ATTACK_MESSAGE); }

void LaunchMines() { IPrepare(INTERFACE_MINES); }

void LaunchColonyBuilding(bool bCreate, bool bArh)
{
	if(procInterfacePrepare(INTERFACE_COLONYBUILDING))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_COLONYBUILDING;
		InitInterface_BB(Interfaces[CurrentInterface].IniFile, bCreate, bArh);
	}
}

void LaunchColonyLandGuard() { IPrepare(INTERFACE_COLONY_LAND_GUARD); }

void LaunchColonyGuardiang() { IPrepare(INTERFACE_COLONYGUARDING); }

void LaunchMekakhrom() { IPrepare(INTERFACE_MEKAKHROM); }

void LaunchQuicklyTrade() { IPrepare(INTERFACE_QUICKLY_TRADER); }

void LaunchCreateCharacter() { IPrepare(INTERFACE_CREATE_CHARACTER); }

void LaunchArena(bool bTournamentResult)
{
	if(procInterfacePrepare(INTERFACE_ARENA))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ARENA;
		InitInterface_B(Interfaces[CurrentInterface].IniFile, bTournamentResult);
	}
}

void LaunchAcademyLand() { IPrepare(INTERFACE_ACADEMY_LAND); }


void LaunchNetMenu() { IPrepare(INTERFACE_NET_NETMENU); }

void LaunchNetFindGame() { IPrepare(INTERFACE_NET_FINDGAME); }

void LaunchNetBuyScreen() { IPrepare(INTERFACE_NET_BUY); }

void LaunchNetGameStateScreen() { IPrepare(INTERFACE_NET_GAMESTATE); }

void LaunchNetPlayerSettings() { IPrepare(INTERFACE_NET_PLAYERSETTINGS); }

void LaunchNetChatString() { IPrepare(INTERFACE_NET_CHATSTRING); }

void LaunchNetEndGameScreen() { IPrepare(INTERFACE_NET_ENDGAME); }

void LaunchNetTopListScreen() { IPrepare(INTERFACE_NET_TOPLIST); }

void LaunchNetCreateGame() { IPrepare(INTERFACE_NET_CREATEGAME); }
void LaunchMapScreen() { IPrepare(INTERFACE_MAP); }

void LaunchDirSailEnc()
{
    Interfaces[INTERFACE_DIRSAILENC].SectionName = "interface\DirSailEnc.c";
	if(procInterfacePrepare(INTERFACE_DIRSAILENC))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_DIRSAILENC;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchPaperMapScreen() { IPrepare(INTERFACE_PAPER_MAP); }

void LaunchColonyInfoScreen()
{
	if (procInterfacePrepare(INTERFACE_COLONY_INFO))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_COLONY_INFO;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

// Дележ добычи
void LaunchSailsGeraldScreen(ref chref,ref chreff)
{
	if(procInterfacePrepare(INTERFACE_SAILSGERALD))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SAILSGERALD;
		InitInterface_RR(Interfaces[CurrentInterface].IniFile, chref, chreff);
	}
}



void LaunchForTest() { IPrepare(INTERFACE_FOR_TEST); }


void LaunchItemsTrade(ref chref)
{
	if(procInterfacePrepare(INTERFACE_ITEMSTRADE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSTRADE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,chref);
	}
}

void LaunchColonyCapture(ref city)
{
	if (procInterfacePrepare(INTERFACE_COLONY_CAPTURE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_COLONY_CAPTURE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, city);
	}
}

void LaunchSelectMenu()
{
	if (CheckAttribute(&InterfaceStack,"SelectMenu_node"))
	{
  		string sTemp = InterfaceStack.SelectMenu_node;
  		call sTemp();
	}
	else
	{
	    LaunchMainCharacter();
	}
}
void LaunchOptionScreen() { IPrepare(INTERFACE_OPTIONSCREEN); }


void LaunchGameMenuScreen()
{
	SetEventHandler("makescrshot","LaunchGameMenuContinue",0);
	aref arScrShoter;
	if( !GetEntity(&arScrShoter,"scrshoter") ) {
		CreateScreenShoter();
		PostEvent("makescrshot",1);
	} else {
		Event("makescrshot");
	}
}

void LaunchGameMenuContinue()
{
	DelEventHandler("makescrshot","LaunchGameMenuContinue");
	if(procInterfacePrepare(INTERFACE_GAMEMENU))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_GAMEMENU;
		InitInterface_GM(Interfaces[CurrentInterface].IniFile);
	}
}

void CreateScreenShoter()
{
	object scrshoter;
	LayerSetRealize(REALIZE)
	CreateEntity(&scrshoter,"scrshoter");
	scrshoter.SavePath = "SAVE";
	if(bSeaActive && !bAbordageStarted) {
		LayerAddObject(SEA_REALIZE,&scrshoter,-1);
	} else {
		LayerAddObject(REALIZE,&scrshoter,-1);
	}
}



void LaunchCharacterItemChange(ref chref)
{
	if(procInterfacePrepare(INTERFACE_ITEMSBOX))
	{
		if (checkattribute(chref, "RealCompanionIdx")) chref = GetCharacter(sti(chref.RealCompanionIdx)); // - Фикс дюпа предметов через компаньонов
		chref.HoldEquip = true; // Сохраняем экипировку при обмене.
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSBOX;
		aref charef; makearef(charef,chref);
		pchar.boxx = "";
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,&charef,chref.FaceID);
	}
}

#event_handler("OpenExchange","OpenExchange");
void OpenExchange()
{
	if(procInterfacePrepare(INTERFACE_ITEMSBOX))
	{
		aref chref = GetEventData();
		chref.HoldEquip = true;
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSBOX;
		aref charef; makearef(charef,chref);
		pchar.boxx = ""; // Убираем старый выбор сундука перед обменом персонажей.
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,&charef,chref.FaceID);
	}
}

void LaunchItemsBox(ref boxRef)
{
    if(CheckAttribute(pchar, "chr_ai.type.state") && pchar.chr_ai.type.state == "follow") {
         Log_SetStringToLog(xiStr("CantNow"));
         return;
    }
	if(procInterfacePrepare(INTERFACE_ITEMSBOX))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSBOX;
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,boxRef,"");
	}
}

void LaunchItemsBarrel(ref boxRef)
{
	if(procInterfacePrepare(INTERFACE_ITEMSBOX))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSBOX;
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,boxRef,"Barrel");
	}
}

void LaunchFortCapture(ref chref)
{
	if(procInterfacePrepare(INTERFACE_FORTCAPTURE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_FORTCAPTURE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,chref);
	}
}

void LaunchShipState()
{
	if(procInterfacePrepare(INTERFACE_SHIP_CHOOSE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIP_CHOOSE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, pchar);
	}
}

void LaunchShipStateNPC(ref _chr)
{
	if(procInterfacePrepare(INTERFACE_SHIP_CHOOSE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIP_CHOOSE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, _chr);
	}
}

void LaunchStore(int storeNum)
{
	if (storeNum < 0) return;
	if (storeNum > STORE_QUANTITY - 1) return;
	gStoreNum = storeNum;

	int storeInterfaceId = INTERFACE_STORE;
	if (CheckAttribute(InterfaceStates, "EnabledOldStore") && sti(InterfaceStates.EnabledOldStore) == 1)
		storeInterfaceId = INTERFACE_STORE_GOOD_OLD;

	if(procInterfacePrepare(storeInterfaceId))
	{
		nPrevInterface = -1;
		CurrentInterface = storeInterfaceId;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,&stores[storeNum]);
	}
}

void LaunchShipyard(ref shipmaster)
{
	if(procInterfacePrepare(INTERFACE_SHIPYARD))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIPYARD;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,shipmaster);
	}
}

void LaunchShipyard1(ref shipmaster)
{
	if(procInterfacePrepare(INTERFACE_SHIPYARD1))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIPYARD1;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,shipmaster);
	}
}

void LaunchShipyard2(ref shipmaster)
{
	if(procInterfacePrepare(INTERFACE_SHIPYARD2))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIPYARD2;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,shipmaster);
	}
}

void LaunchShipyard3(ref shipmaster)
{
	if(procInterfacePrepare(INTERFACE_SHIPYARD3))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIPYARD3;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,shipmaster);
	}
}

void LaunchPortStock(ref _portman) // стоянка кораблей в портовом управлении
{
	if(procInterfacePrepare(INTERFACE_PORTSTOCK))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_PORTSTOCK;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, _portman);
	}
}

void LaunchPortAuction(ref _portman) // LEO: аукцион (продажа корабля) в портовом управлении
{
	if(procInterfacePrepare(INTERFACE_PORTAUCTION))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_PORTAUCTION;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, _portman);
	}
}

void LaunchRepair(ref refCharacter)
{
	if(procInterfacePrepare(INTERFACE_REPAIR))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_REPAIR;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,refCharacter);
	}
}

void LaunchDiseaseAlert(int iMode)
{
	if(procInterfacePrepare(INTERFACE_DISEASE_ALERT))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_DISEASE_ALERT;
		InitInterface_I(Interfaces[CurrentInterface].IniFile, iMode);
	}
}


void LaunchCannons(int storeNum)
{
    if(storeNum<0)	return;
  	if(storeNum>STORE_QUANTITY-1)	return;
	if(procInterfacePrepare(INTERFACE_CANNONS))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CANNONS;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, &stores[storeNum]);
	}
}

void LaunchCharacter(ref refCharacter)
{
	if(procInterfacePrepare(INTERFACE_CHARACTER))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CHARACTER;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,refCharacter);
	}
}

void LaunchMainCharacter()
{
 	if(procInterfacePrepare(INTERFACE_CHARACTER_ALL))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CHARACTER_ALL;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchMainMenu_afterVideo()
{
	DelEventHandler(EVENT_END_VIDEO,"LaunchMainMenu_afterVideo");
	LaunchNewMainMenu();
}

void LaunchNewMainMenu() { IPrepare(INTERFACE_MAINMENU); }

void LaunchMainMenu()
{
	LayerSetRealize(REALIZE)

	if(procEnableInterfaceLaunch(INTERFACE_MAINMENU)==false)
	{
		if( CheckAttribute(&InterfaceStates,"LaunchAnyway") && sti(InterfaceStates.LaunchAnyway)==true )
		{
			PostEvent("LaunchIAfterFrame",1,"sl","I_MAINMENU",1);
			InterfaceStates.LaunchAfterFrame = "I_MAINMENU";
		}
		return;
	}
	DeleteAttribute(&InterfaceStates,"LaunchAnyway");
	if(g_ibVideoExecuting) return;
	InterfaceStates.Launched = true;
	InterfaceStates.doUnFreeze = false;

	ISetSoundEvents();
	nPrevInterface = -1;
	CurrentInterface = INTERFACE_MAINMENU;
	EngineLayersOffOn(false);

	bGamePadChangeEnable = true;
	LoadSegment(Interfaces[CurrentInterface].SectionName);
	InitInterface(Interfaces[CurrentInterface].IniFile);
	SetTimeScale(1.0);
	TimeScaleCounter = 0;
    DelPerkFromActiveList("TimeSpeed");
}

void LaunchSaveGame()
{
	if(procInterfacePrepare(INTERFACE_SAVELOAD))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SAVELOAD;
		InitInterface_BB(Interfaces[CurrentInterface].IniFile,true,false);
	}
}

void LaunchLoadGame(bool isMainMenu)
{
	if(procInterfacePrepare(INTERFACE_SAVELOAD))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SAVELOAD;
		InitInterface_BB(Interfaces[CurrentInterface].IniFile,false, isMainMenu);
	}
}


void LaunchHireCrew() { IPrepare(INTERFACE_HIRECREW); }

//New Abilities - взлом сундука
void LaunchLockPicking()
{
	if(procInterfacePrepare(INTERFACE_LOCKPICK))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_LOCKPICK;
		InitInterface_GM(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchPointsEX() {
	if(procInterfacePrepare(INTERFACE_POINTS_EX)) {
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_POINTS_EX;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}



void LaunchRansackMain(ref _refMy,ref _refEnemy,string captureState)
{
	if (SeaTowOperations_Block(_refEnemy)) return;
	if(procInterfacePrepare(INTERFACE_RANSACK_MAIN))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_RANSACK_MAIN;
		InitInterface_RRS(Interfaces[CurrentInterface].IniFile,_refMy,_refEnemy,captureState);
	}
}

void LaunchTransferMain(ref _refMy, ref _refEnemy, string newCurNod)
{
	if (SeaTowOperations_BlockSwap(_refMy, _refEnemy)) return;
	if(procInterfacePrepare(INTERFACE_TRANSFER_MAIN))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_TRANSFER_MAIN;
		InitInterface_RRS(Interfaces[CurrentInterface].IniFile,_refMy,_refEnemy,newCurNod);
	}
}

void LaunchQuestBook() { IPrepare(INTERFACE_QUESTBOOK); }

void LaunchTradeBook() { IPrepare(INTERFACE_TRADEBOOK); }

void LaunchNationRelation() { IPrepare(INTERFACE_NATIONRELATION); }


void LaunchItems() { IPrepare(INTERFACE_ITEMS); }

void LaunchSelectCharacter()
{

	SetEventHandler(EVENT_END_VIDEO,"ISTART_SelCharacter",0);
	PostEvent(EVENT_END_VIDEO,0);
}

void LaunchSelectNation()
{

	SetEventHandler(EVENT_END_VIDEO,"ISTART_SelNation",0);
	PostEvent(EVENT_END_VIDEO,0);
}

void ISTART_SelCharacter()
{
	InterfaceStates.Launched = true;
	DelEventHandler(EVENT_END_VIDEO,"ISTART_SelCharacter");
	ISetSoundEvents();
	nPrevInterface = -1;
	CurrentInterface = INTERFACE_CHARACTER_SELECT;
	LoadSegment(Interfaces[CurrentInterface].SectionName);
	InitInterface(Interfaces[CurrentInterface].IniFile);
}

void LaunchSpeakInterface(aref chr, int whospeak, string sMode)
{
trace("LaunchSpeakInterface, chr.id:" + chr.id + ", whospeak: " + whospeak + ",sMode:" + sMode);
	if(procInterfacePrepare(INTERFACE_SPEAK))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SPEAK;

		InitInterface_RIS(Interfaces[CurrentInterface].IniFile, chr, whospeak, sMode);
	}
}

void LaunchCraftAll()
{
	DelEventHandler("makescrshot","LaunchGameMenuContinue");
	if(procInterfacePrepare(INTERFACE_CRAFT_ALL))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CRAFT_ALL;
		InitInterface_GM(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchItemsExchange()
{
	if(procInterfacePrepare(ITEMS_EXCHANGE))
	{
		nPrevInterface = -1;
		CurrentInterface = ITEMS_EXCHANGE;
		InitInterface_GM(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchShipUpgrades(ref shipmaster)
{
	if(procInterfacePrepare(SHIPS_UPGRADES))
	{
		nPrevInterface = -1;
		CurrentInterface = SHIPS_UPGRADES;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, shipmaster);
	}
}


void EndOkInterface()
{
    UnloadSegment(Interfaces[CurrentInterface].SectionName);

	if(nPrevInterface == INTERFACE_STORE)
	{
		int cn = GetCompanionIndex(pchar,sti(GameInterface.FourImage.current));
		if(cn!=-1)
		{
			DeleteAttribute(&GameInterface,"");
			CurrentInterface = INTERFACE_STORE;
			LoadSegment(Interfaces[CurrentInterface].SectionName);
			InitInterface_RR(Interfaces[CurrentInterface].IniFile,GetCharacter(cn),&stores[gStoreNum]);
		}
		else
		{
			IDeleteSoundEvents();
			Start_InterfaceDoExit();
		}
	}
	else
	{
		IDeleteSoundEvents();
	    Start_InterfaceDoExit();
	}
}

void EndCancelInterface(bool bYesRelease)
{
	KillMessageBox();
	KillIconSelector();

	ref refObj1,refObj2;
	bGamePadChangeEnable = false;
	switch(interfaceResultCommand)
	{

	}
	InterfaceStates.doUnFreeze = bYesRelease;
   UnloadSegment(Interfaces[CurrentInterface].SectionName);
	IDeleteSoundEvents();
	switch(interfaceResultCommand)
	{

	}

    Start_InterfaceDoExit();
	if(bYesRelease)	PostEvent("evntQuestsCheck",1);
	SetExpLogsBack(); //собственно то, что на основе кэша опыта возвращает строчки после прыжка в гейминтерфейс
}

int ExpLog_MaxLines()
{
	return 20;
}

int ExpLog_ClampLines(float value)
{
	if(value<1.0) return 1;
	if(value>ExpLog_MaxLines()) return ExpLog_MaxLines();
	return makeint(value+0.5);
}

float ExpLog_LinesToSlider(float value)
{
	int lines=ExpLog_ClampLines(value);
	if(lines==1) return 0.0;
	if(lines==ExpLog_MaxLines()) return 1.0;
	// LEO: Движок отбрасывает дробную часть позиции; запас защищает шаг от погрешности float.
	return (lines-1+0.001)/(ExpLog_MaxLines()-1);
}

int ExpLog_LinesFromSlider(float value)
{
	return ExpLog_ClampLines(1.0+value*(ExpLog_MaxLines()-1));
}

int ExpLog_LineCount(bool sea)
{
	if(sea) return makeint(NewSeaHUD_GetOption("SeaExpLogLines"));
	return makeint(LandHUD_GetOption("LandExpLogLines"));
}

bool ExpLog_IsSea()
{
	return bSeaActive && !bAbordageStarted;
}

// LEO: Одинаковое присваивание тоже может вызвать AttributeChanged; пишем только изменившийся текст.
bool ExpLog_SetLineText(aref owner,string line,string text)
{
	string path="textinfo."+line;
	if(!CheckAttribute(owner,path+".font")) return false;
	if(CheckAttribute(owner,path+".text"))
	{
		if(owner.textinfo.(line).text==text) return false;
	}
	owner.textinfo.(line).text=text;
	return true;
}

bool ExpLog_SetCachedText(string cache,string line,string text)
{
	if(CheckAttribute(pchar,cache+"."+line))
	{
		if(pchar.(cache).(line)==text) return false;
	}
	else if(text=="") return false;
	pchar.(cache).(line)=text;
	return true;
}

// LEO: Кэш и число видимых строк независимы; в старом кэше отсутствующий хвост считается пустым.
int ExpLog_Apply(aref owner,bool sea)
{
	if(!CheckAttribute(owner,"textinfo.EXP1.font")) return 0;
	string cache="LIEXP";
	if(sea) cache="BIEXP";
	int count=0;
	if(sti(InterfaceStates.ShowExpLogs)==1) count=ExpLog_LineCount(sea);
	int changed=0;
	for(int i=1;i<=ExpLog_MaxLines();i++)
	{
		string line="EXP"+i;
		string text="";
		if(i<=count)
		{
			if(CheckAttribute(pchar,cache+"."+line)) text=pchar.(cache).(line);
		}
		if(ExpLog_SetLineText(owner,line,text)) changed++;
	}
	return changed;
}

void ExpLog_PushLand(string text)
{
	for(int i=ExpLog_MaxLines();i>1;i--)
	{
		string line="EXP"+i;
		string previous="EXP"+(i-1);
		string oldText="";
		if(CheckAttribute(pchar,"LIEXP."+previous)) oldText=pchar.LIEXP.(previous);
		ExpLog_SetCachedText("LIEXP",line,oldText);
	}
	ExpLog_SetCachedText("LIEXP","EXP1",text);
	ExpLog_Apply(&objLandInterface,false);
}

string ExpLog_FormatReputation(int change,string icon,int current)
{
	if(change>=0) return "ᶑ"+change+" rep "+icon+" "+current;
	return "Ẹ"+(0-change)+" rep "+icon+" "+current;
}

void ExpLog_AddSea(string type,float delta,string icon,int current,bool reputation)
{
	int slot=0;
	for(int i=1;i<=ExpLog_MaxLines();i++)
	{
		string key="EXP"+i;
		string path="BIEXP."+key;
		if(CheckAttribute(pchar,path+".type"))
		{
			if(pchar.BIEXP.(key).type==type)
			{
				slot=i;
				break;
			}
		}
		else if(slot==0)
		{
			// LEO: Старую строку без типа показываем как есть, не угадывая её навык или сумму.
			if(!CheckAttribute(pchar,path)) slot=i;
			else if(pchar.BIEXP.(key)=="") slot=i;
		}
	}
	if(slot==0) return;
	string line="EXP"+slot;
	float previous=0.0;
	bool hadValue=CheckAttribute(pchar,"BIEXP."+line+".val");
	if(hadValue) previous=stf(pchar.BIEXP.(line).val);
	float total=previous+delta;
	string text;
	if(reputation) text=ExpLog_FormatReputation(makeint(total),icon,current);
	else text="ᶑ"+FloatToString(total,2)+" "+icon+" exp";
	ExpLog_SetCachedText("BIEXP",line,text);
	if(!CheckAttribute(pchar,"BIEXP."+line+".type")) pchar.BIEXP.(line).type=type;
	if(!hadValue || total!=previous) pchar.BIEXP.(line).val=total;
	if(slot<=ExpLog_LineCount(true)) ExpLog_SetLineText(&BattleInterface,line,text);
}

void ExpLog_Reputation(string type,int change,string icon,int current)
{
	if(ExpLog_IsSea()) ExpLog_AddSea(type,makefloat(change),icon,current,true);
	else ExpLog_PushLand(ExpLog_FormatReputation(change,icon,current));
}

void SetExpLogsBack()
{
	if(ExpLog_IsSea()) ExpLog_Apply(&BattleInterface,true);
	else ExpLog_Apply(&objLandInterface,false);
}

void ResetExpLogs()
{
	DeleteAttribute(pchar,"BIEXP");
	DeleteAttribute(pchar,"LIEXP");
	SetExpLogsBack();
}

// стек для вложенных интерфейсов
object InterfaceStack;
void ClearInterfaceStack()
{
	DeleteAttribute(InterfaceStack,"");
	InterfaceStack.size=0;
}
void PushInterface(int iIndex,string iTitle,string iStartNod)
{
	if(!CheckAttribute(&InterfaceStack,"size"))	InterfaceStack.size = 0;
	string StackSlot = "Slot"+InterfaceStack.size;
	InterfaceStack.(StackSlot).index = iIndex;
	InterfaceStack.(StackSlot).title = iTitle;
	InterfaceStack.(StackSlot).start = iStartNod;
	InterfaceStack.size = sti(InterfaceStack.size)+1;
}
bool PopInterface()
{
	if(!CheckAttribute(&InterfaceStack,"size"))	InterfaceStack.size = 0;
	if(sti(InterfaceStack.size)<1) return false;
	InterfaceStack.size = sti(InterfaceStack.size)-1;
	string StackSlot = "Slot"+InterfaceStack.size;
	CurrentInterface = sti(InterfaceStack.(StackSlot).index);
	DeleteAttribute(&InterfaceStack,StackSlot);
	return true;
}
void I_PushString(string str)
{
	aref arStrStack;
	makearef(arStrStack,InterfaceStack.strings);
	int q = GetAttributesNum(arStrStack);
	string stackSlotName = "s"+q;
	InterfaceStack.strings.(stackSlotName) = str;
}
string I_PopString()
{
	aref arStrStack;
	makearef(arStrStack,InterfaceStack.strings);
	int q = GetAttributesNum(arStrStack)-1;
	if(q<0) return "";
	string stackSlotName = "s"+q;
	string retStr = InterfaceStack.strings.(stackSlotName);
	DeleteAttribute(&InterfaceStack,"strings."+stackSlotName);
	return retStr;
}

void ISetSoundEvents()
{
	SetEventHandler(ISOUND_EVENT,"IDoSoundEvent",0);
}
void IDeleteSoundEvents()
{
	DelEventHandler(ISOUND_EVENT,"IDoSoundEvent");
}
void IDoSoundEvent()
{
	int comCode = GetEventData();
	switch( comCode )
	{
	case 1: PlaySound("interface\ok.wav"); break; // любая клавиша
	case 2: PlaySound("interface\menu_select_01.wav"); break; // выбор элемента
	}
}

#event_handler("evntPostVideo","stPostVideo");
void StartPictureAsVideo( string picname, float time )
{
	InterfaceStates.GameOverPicture = picname;
	PostEvent( "DoInfoShower", 1, "sl", "Game Over Picture", true );
	SetEventHandler("PictureAsVideoBreak","PictureAsVideoBreak",0);
	PostEvent( "PictureAsVideoBreak", makeint(time * 3000) );
}
void PictureAsVideoBreak()
{
    ResetSound();
	DelEventHandler("PictureAsVideoBreak","PictureAsVideoBreak");
	Event( "DoInfoShower", "sl", "Game Over Picture", false );
	Event( EVENT_END_VIDEO );
}

object aviVideoObj;
void StartPostVideo(string vidName,int n)
{
	SetEventHandler("evntPostVideo","stPostVideo",0);
	PostEvent("evntPostVideo",n,"s",vidName);
}
void stPostVideo()
{
	DelEventHandler("evntPostVideo","stPostVideo");
	string sVidName = GetEventData();
	StartVideo(sVidName);
}
void PostVideoAndQuest(string vidName,int n,string sQuestName)
{
	aviVideoObj.afterQuestName = sQuestName;
	StartPostVideo(vidName,n);
}
bool g_ibVideoExecuting = false;
void StartVideo(string vidName)
{
	if(IsEntity(&aviVideoObj))
	{
		DeleteClass(&aviVideoObj);
		StartPostVideo(vidName,1);
	}
	g_ibVideoExecuting = true;
	PauseAllSounds();
	ResetSound();
	if(sti(InterfaceStates.Launched))
	{
		InterfaceStates.InstantExit = true;
		Event("exitCancel");
		DeleteAttribute(&InterfaceStates,"InstantExit");
	}
	SetEventHandler("ievntEndVideo","_Procedure_EndVideoPlay",0);
	EngineLayersOffOn(false);
	if(bSeaActive && !bAbordageStarted)
	{
		aviVideoObj.layer = "sea";
	}
	else
	{
		aviVideoObj.layer = "land";
	}
	if( GetTargetPlatform()=="pc" )	{CreateEntity(&aviVideoObj,"CAviPlayer");}
	else	{CreateEntity(&aviVideoObj,"WMVideoPlay");}
	SendMessage(&aviVideoObj,"ls",MSG_SET_VIDEO_PLAY,GetVideoFileName(vidName));
	bGamePadChangeEnable = true;
	bPlayVideoNow = true;

	if( vidName=="Artefact" ||
		vidName=="Ending" ||
		vidName=="Invasion" ||
		vidName=="Temple"
		) {bBreakVideoDisable = true;}
	else {bBreakVideoDisable = false;}

	DeleteAttribute(&InterfaceStates,"VideoBreakControls");
	SetEventHandler("Control Activation","IVideoBreakPrepare",0);
	SetEventHandler("Control Deactivation","IVideoBreak",0);
	if(!IsEntity(aviVideoObj))
	{
		Trace("Can`t create video player");
		PostEvent("ievntEndVideo",0);
	}
}
bool bBreakVideoDisable = false;
void IVideoBreakPrepare()
{
	string ControlName = GetEventData();
	InterfaceStates.VideoBreakControls.(ControlName) = 1;
}
void IVideoBreak()
{
	string ControlName = GetEventData();
	if( !CheckAttribute(&InterfaceStates,"VideoBreakControls."+ControlName) ) {return;}
	DeleteAttribute(&InterfaceStates,"VideoBreakControls");

	bool bMakeBreak = false;
	if( ControlName=="ICancel" )	{bMakeBreak = true;}
	if( !bBreakVideoDisable )
	{
		if( ControlName=="IStartButton" || ControlName=="IAllCancel" ||
			ControlName=="IAction" ) {bMakeBreak = true;}
	} else {
		if(!EnableVideoBreak) {bMakeBreak = false;}
	}
	if(bMakeBreak)
	{
		DelEventHandler("Control Activation","IVideoBreakPrepare");
		DelEventHandler("Control Deactivation","IVideoBreak");
		Event("ievntEndVideo");
	}
}
void _Procedure_EndVideoPlay()
{
	g_ibVideoExecuting = false;
	bBreakVideoDisable = false;
	DelEventHandler("Control Activation","IVideoBreakPrepare");
	DelEventHandler("Control Deactivation","IVideoBreak");
	DeleteClass(&aviVideoObj);
	if(bMainMenuLaunchAfterVideo)
	{
		bMainMenuLaunchAfterVideo = false;
		Event("DoInfoShower","sl","MainMenuLaunch",true);
	} else {InfoShowSetting();}
	bGamePadChangeEnable = false;
	bPlayVideoNow = false;
	DelEventHandler("ievntEndVideo","_Procedure_EndVideoPlay");
	if(aviVideoObj.layer == "land")
	{
		LayerFreeze(REALIZE,false);
		LayerFreeze(EXECUTE,false);
	}
	if(aviVideoObj.layer == "sea")
	{
		LayerFreeze(SEA_REALIZE,false);
		LayerFreeze(SEA_EXECUTE,false);
	}
	if(sti(InterfaceStates.Launched)) EngineLayersOffOn(false);
	if(CheckAttribute(&aviVideoObj,"afterQuestName"))
	{
		DoQuestCheckDelay(aviVideoObj.afterQuestName,0.1);
		DeleteAttribute(&aviVideoObj,"afterQuestName");
	}
	PostEvent(EVENT_END_VIDEO,1);
}

int GetCurrentInterface()
{
	return CurrentInterface;
}

bool procEnableInterfaceLaunch(int _interfaceCode)
{
	if(sti(InterfaceStates.Launched)==true) return false;
	if(IsEntity(reload_fader)) return false;
	if(DialogRun!=0)
	{
		return false;
	}
	return true;
}

void ReturnToMainMenu()
{
	DelEventHandler("frame","InterfaceDoExit");
	EngineLayersOffOn(false);
	InterfaceStates.Launched = false;
	InterfaceStates.MakeScreenShot = false;
	LaunchMainMenu();
}

void ILaunchAfterFrame()
{
	string nodName = GetEventData();
	int n = GetEventData();
	if( CheckAttribute(&InterfaceStates,"LaunchAfterFrame") && InterfaceStates.LaunchAfterFrame!=nodName ) {
		return;
	}
	if(n>0 || sti(InterfaceStates.Launched))
	{
		PostEvent("LaunchIAfterFrame",1,"sl", nodName, n-1);
		return;
	}

	switch(nodName)
	{
		case "I_CHARACTER":
			// LEO: режим чеккера NPC (подзорка в бетта-тесте) - вкладка персонажа показывает проверяемого, не ГГ
			if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx"))
			{
				pchar.SystemInfo.OnlyShowCharacter = true; // LEO: держим read-only при возврате на вкладку
				LaunchCharacter(GetCharacter(sti(pchar.SystemInfo.CheckNpcIdx)));
			}
			else LaunchMainCharacter();
			return; break;
		case "I_SHIP":
			// LEO: чеккер NPC - вкладка корабля показывает корабль проверяемого (read-only через гард в ship.c)
			if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) LaunchShipStateNPC(GetCharacter(sti(pchar.SystemInfo.CheckNpcIdx)));
			else LaunchShipState();
			return; break;
		case "I_TREASURER_REPORT":	LaunchTreasurerReport();	return; break; // Gregg: отчёт казначея
		case "I_QUESTBOOK":		LaunchQuestBook();	return; break;
		case "I_TRADEBOOK":		LaunchTradeBook();	return; break;
		case "I_NATIONS":		LaunchNationRelation();	return; break;
		case "I_ITEMS":			LaunchItems();	return; break;
		case "TransferMain":
			if( CheckAttribute(pchar,"TransferChar") )
			{
				LaunchTransferMain(pchar, GetCharacter(sti(pchar.TransferChar)), "Transfer");
				DeleteAttribute(pchar, "TransferChar");
			}
			return;
		break;
	}
}

bool procInterfacePrepare(int interfaceCode)
{
	if(procEnableInterfaceLaunch(interfaceCode)==false) return false;
	if(g_ibVideoExecuting) return false;

	if(interfaceCode != INTERFACE_FORTCAPTURE && interfaceCode != INTERFACE_RANSACK_MAIN && interfaceCode != INTERFACE_ARENA)
	{
		aref arFader;
		if( GetEntity(arFader,"fader") ) {return false;}
	}

	if( LoadSegment(Interfaces[interfaceCode].SectionName) )
	{
		Telescope_Off();
		if (Sea_InTacticalView()) SeaShipCamera.TacHideHUD = 1; // открыт GameInterface -> спрятать тактический HUD (камера читает флаг)
		Event("Interface_Started");
		InterfaceStates.Launched = true;
		InterfaceStates.doUnFreeze = false;
		EngineLayersOffOn(false);
		ISetSoundEvents();
		DeleteAttribute(&GameInterface,"");
		DeleteAttribute(&InterfaceStates,"tooltip");
		if(IsPerkIntoList("TimeSpeed")) {SetTimeScale(1.0);}
		LayerFreeze(INTERFACE_REALIZE,false);
		LayerFreeze(INTERFACE_EXECUTE,false);
		return true;
	}
	else
	{
		interfaceResultCommand = RC_INTERFACE_ERROR;
		Start_InterfaceDoExit();
		return false;
	}
}

void Start_InterfaceDoExit()
{
	if(IsPerkIntoList("TimeSpeed"))
	{
		// LEO: Только смена NewHUD сохраняет точное ручное ускорение/замедление до открытия меню.
		if(bNewHUDRestoreTime && TimeScaleCounter!=0) SetTimeScale(1.0+TimeScaleCounter*0.25);
		else SetTimeScale(GetSeaTimeScale());
	}
	bNewHUDRestoreTime=false;
	SetEventHandler("frame","InterfaceDoExit",1);
}

extern void OSL_WriteGameOption();
extern void OSL_ReadGameOption();

void LoadGameOptions()
{
	if( LoadSegment("interface\option_sl.c") )
	{
		OSL_ReadGameOption();
		UnloadSegment("interface\option_sl.c");
	}
	// LEO: флаги, которые читает движок, - отдать ему сразу после загрузки опций
	if (CheckAttribute(&InterfaceStates, "NpcHeadLook")) NpcHeadLookSet(sti(InterfaceStates.NpcHeadLook));
}

void SaveGameOptions()
{
	if( LoadSegment("interface\option_sl.c") )
	{
		OSL_WriteGameOption();
		UnloadSegment("interface\option_sl.c");
	}
}

void ReloadAfterFortCapture()
{
	if (!CheckAttribute(pchar,"from_interface"))
	{
		if (MOD_BETTATESTMODE == "On") Log_Info("Error: ReloadAfterFortCapture miss from_interface.fortCharacterIdx");
		return;
	}

	int fortChr = sti(pchar.from_interface.fortCharacterIdx);
	AfterTownBattle(); //трем неоконченый захват

    // перезапомним
    pchar.GenQuestFort.fortCharacterIdx = fortChr;

    bool ExitToSea = CheckAttribute(pchar, "from_interface.Exit_to_Sea");
	ref chref = GetCharacter(fortChr);

	string sToLocation = "";
	string sToLocator = "";

	aref ar_FindLoc = FindIslandReloadLocator(chref.location,chref.location.locator);
	if(CheckAttribute(ar_FindLoc,"GoAfterBoard.location"))
	{
		ar_FindLoc = FindIslandReloadLocator(chref.location,ar_FindLoc.GoAfterBoard.location);
		if( CheckAttribute(ar_FindLoc,"go") )
		{	sToLocation = ar_FindLoc.go;
		}
		if( CheckAttribute(ar_FindLoc,"emerge") )
		{	sToLocator = ar_FindLoc.emerge;
		}
	}

	DeleteAttribute(pchar,"from_interface");

	chref.Fort.Mode = FORT_DEAD;
	// data of frot die
	chref.Fort.DieTime.Year = GetDataYear();
	chref.Fort.DieTime.Month = GetDataMonth();
	chref.Fort.DieTime.Day = GetDataDay();
	chref.Fort.DieTime.Time = GetTime();

    pchar.GenQuest.Hunter2Pause = true;  // boal бойня в форте
	// опыт
    AddCharacterExpToSkillSquadron(GetMainCharacter(), "Leadership", 500); //добавка в городе, если  бой непрерывен
    AddCharacterExpToSkillSquadron(GetMainCharacter(), "Sailing", 500);
    AddCharacterExpToSkillSquadron(GetMainCharacter(), "Sneak", 300);

    Statistic_AddValue(GetMainCharacter(), "Fort", 1);
    Statistic_AddValue(GetMainCharacter(), NationShortName(sti(chref.nation)) + "_KillFort", 1);
    fOldMaxSeaHeight = 0.6; // fix потоп в порту ФФ boal 03.01.05
	if(sToLocation != "")
	{
		string sColony = Locations[FindLocation(sToLocation)].fastreload;
		SetTownCapturedState(sColony,true);
		Log_TestInfo("ReloadAfterFortCapture");
		Go2LocationAfterAbordage();
		pchar.location.from_sea = sToLocation;
		if (true)
	    {
	        DoQuestReloadToLocation(sToLocation, "reload", sToLocator, "Capture_Forts");
	    }
	    else
	    {
	        DoQuestReloadToLocation(sToLocation, "reload", sToLocator, "Capture_Forts_ExitToSea");
	    }
	}
	else
	{
	    trace("ReloadAfterFortCapture has no sToLocation");
	    Log_info("ReloadAfterFortCapture has no sToLocation");
	}
}

void IProcEventGameOver()
{
	string str = GetEventData();
	if( sti(InterfaceStates.Launched) )
	{
		InterfaceStates.InstantExit = true;
		Event("exitCancel");
		DeleteAttribute(&InterfaceStates,"InstantExit");
	}
	GameOver(str);
}

void IProcEventSetGameTime()
{
	int itmp = GetEventData();
	InterfaceStates.GameTime.hour = itmp;

	itmp = GetEventData();
	InterfaceStates.GameTime.min = itmp;

	itmp = GetEventData();
	InterfaceStates.GameTime.sec = itmp;
}

//==============================================================
//	Секция Показывающая какую либо инфо на экране
//==============================================================
#event_handler("DoInfoShower", "procInfoShow");
object objInfoList[10];
object objInfoFader;
bool bInfoHandlerAvailable = false; // В этом движке нет нативного класса InfoHandler.

void procInfoShow()
{
	string sInfoID = GetEventData();
	int nInfoSet = false;
	if( sInfoID=="" ) {
		sInfoID = "MainMenuLaunch";
	} else {
		nInfoSet = GetEventData();
	}

	int nInfoIdx = 0;
	switch(sInfoID)
	{
	case "MainMenuLaunch":		nInfoIdx = 0; break;
	case "save game":			nInfoIdx = 1; break;
	case "Low Storage":			nInfoIdx = 2; break;
	case "game prepare":		nInfoIdx = 3; break;
	case "OptionsBreak":		nInfoIdx = 4; break;
	case "lost controller":		nInfoIdx = 5; break;
	case "Game Over Picture":	nInfoIdx = 6; break;
	}

	int prevQ = 0;
	if( CheckAttribute(&objInfoList[nInfoIdx],"refr") ) {
		prevQ = sti(objInfoList[nInfoIdx].refr);
	}

	if( nInfoSet!=0 ) prevQ++;
	else prevQ--;
	if(prevQ<0) prevQ=0;
	objInfoList[nInfoIdx].refr = prevQ;

	if( prevQ>0 )
	{
		if( !IsEntity(&objInfoList[nInfoIdx]) )
		{
			objInfoList[nInfoIdx].backColor = 0;
			objInfoList[nInfoIdx].foreColor = 0;
			objInfoList[nInfoIdx].borderWidth = 32;
			objInfoList[nInfoIdx].offset = 30;

			switch(sInfoID)
			{
			case "save game":
				objInfoList[nInfoIdx].picbackfilename = "loading\" + LanguageGetLanguage() + "\save_game.tga";
			break;

			case "lost controller":
				objInfoList[nInfoIdx].picbackfilename = "loading\" + LanguageGetLanguage() + "\lost_controller.tga";
			break;

			case "Low Storage":
				objInfoList[nInfoIdx].picbackfilename = "loading\" + LanguageGetLanguage() + "\low_storage.tga";
			break;

			case "game prepare":
				objInfoList[nInfoIdx].picbackfilename = "loading\jonny_load\load\load_"+rand(8)+".dds";
			break;

			case "MainMenuLaunch":
				objInfoList[nInfoIdx].picbackfilename = "interfaces\mainmenu_ss.dds";
			break;

			case "OptionsBreak":
				objInfoList[nInfoIdx].picbackfilename = "loading\" + LanguageGetLanguage() + "\options.tga";
			break;

			case "Game Over Picture":
				objInfoList[nInfoIdx].picbackfilename = InterfaceStates.GameOverPicture;
			break;
			}

			if( bInfoHandlerAvailable )
			{
				CreateEntity(&objInfoList[nInfoIdx],"InfoHandler");
			}
			else
			{
				InfoShowFallback(true, objInfoList[nInfoIdx].picbackfilename);
			}

		}
	}
	else
	{
		if( bInfoHandlerAvailable )
		{
			if( IsEntity(&objInfoList[nInfoIdx]) )
			{
				DeleteClass(&objInfoList[nInfoIdx]);
			}
		}
		else
		{
			InfoShowFallback(false, "");
		}
	}

	InfoShowSetting();
}

bool InfoShowFallbackHasActive()
{
	for(int i=0; i<7; i++)
	{
		if( CheckAttribute(&objInfoList[i], "refr") && sti(objInfoList[i].refr)>0 )
		{
			return true;
		}
	}
	return false;
}

void InfoShowFallback(bool bShow, string sPic)
{
	if( bShow )
	{
		if( IsEntity(&objInfoFader) )
		{
			DeleteClass(&objInfoFader);
		}
		CreateEntity(&objInfoFader, "fader");
		SendMessage(&objInfoFader, "ls", FADER_PICTURE0, sPic);
		SendMessage(&objInfoFader, "lfl", FADER_OUT, 0.00001, false);
		return;
	}

	if( !InfoShowFallbackHasActive() && IsEntity(&objInfoFader) )
	{
		DeleteClass(&objInfoFader);
	}
}

void InfoShowSetting()
{
	bool bAlreadySet = false;
	bool bMakeSet;
	for(int i=6; i>=0; i--)
	{
		bMakeSet = false;
		if( IsEntity(&objInfoList[i]) )
		{
			if(!bAlreadySet) {
				bMakeSet = true;
				if( i==5 )
				{
					if( g_ibVideoExecuting || !CheckAttribute(&InterfaceStates,"Buttons.Resume.enable") ||
						sti(InterfaceStates.Buttons.Resume.enable)==false )
					{
						bMakeSet = false;
					}
				}
			}
		}

		if( bMakeSet ) {
			LayerAddObject(INFO_REALIZE,&objInfoList[i],-1);
			bAlreadySet = true;
		} else {
			LayerDelObject(INFO_REALIZE,&objInfoList[i]);
		}

		if(i==2) {
			if(bMakeSet) {
				SetEventHandler("Control Activation","InfoShow_Control",0);
			} else {
				DelEventHandler("Control Activation","InfoShow_Control");
			}
		}
		if(i==4) {
			if(bMakeSet) {
				SetEventHandler("Control Activation","InfoShow_Control2",0);
			} else {
				DelEventHandler("Control Activation","InfoShow_Control2");
			}
		}
		if(i==5) {
			if(bMakeSet) {
				SetTimeScale(0.0);
			} else {
				if(IsPerkIntoList("TimeSpeed")) {
					if( sti(InterfaceStates.Launched) == true ) {
						SetTimeScale(GetSeaTimeScale());
					} else { SetTimeScale(1.0); }
				} else {SetTimeScale(1.0);}
			}
		}
	}

	if(bAlreadySet) {
		LayerFreeze(INTERFACE_REALIZE,true);
		LayerFreeze(INTERFACE_EXECUTE,true);
	} else {
		LayerFreeze(INTERFACE_REALIZE,false);
		LayerFreeze(INTERFACE_EXECUTE,false);
	}
}


void InfoShow_Control()
{
	string ControlName = GetEventData();

	if(ControlName=="IAction" || ControlName=="ICancel")
	{
		PostEvent("DoInfoShower",0,"sl","Low Storage",false);
		if(ControlName=="ICancel")
		{
			SendMessage(&GameInterface,"l",MSG_INTERFACE_LAUNCH_DASHBOARD);
		}
		else Event("evntLowStorageBreak");
		LayerFreeze(INTERFACE_REALIZE,false);
		LayerFreeze(INTERFACE_EXECUTE,false);
	}
}

void InfoShow_Control2()
{
	string ControlName = GetEventData();

	if(ControlName=="IAction" || ControlName=="IStartButton")
	{
		PostEvent("DoInfoShower",0,"sl","OptionsBreak",false);
		Event("evntOptionsBreak");
		LayerFreeze(INTERFACE_REALIZE,false);
		LayerFreeze(INTERFACE_EXECUTE,false);
	}
}

void procAttractExit()
{
	PostEvent("ievntEndVideo",1);
	DelEventHandler("Control Activation","procAttractExit");
}

float GetSeaTimeScale()
{
	int iMult = 2; // дефолт, если своя настройка выключена/не задана

	if (!CheckAttribute(&InterfaceStates,"TimeAccelSettings") || sti(InterfaceStates.TimeAccelSettings) == 0)
	{
		return makefloat(iMult);
	}

	// Борода: три отдельных потолка - как у ручного ускорения TimeScaleFaster/Slower (см. seadogs.c ProcessControls). Множитель - целое число.
	if (bDisableMapEnter) // бой на море
	{
		if (CheckAttribute(&InterfaceStates,"TimeAccelSeaBattle")) iMult = sti(InterfaceStates.TimeAccelSeaBattle);
		if (iMult > 9) iMult = 9;
	}
	else if (bSeaActive && !bAbordageStarted) // открытое море
	{
		if (CheckAttribute(&InterfaceStates,"TimeAccelSea")) iMult = sti(InterfaceStates.TimeAccelSea);
		if (iMult > 13) iMult = 13;
	}
	else // суша
	{
		if (CheckAttribute(&InterfaceStates,"TimeAccelLand")) iMult = sti(InterfaceStates.TimeAccelLand);
		if (iMult > 9) iMult = 9;
	}
	if (IsEntity(worldMap) && iMult > 9) iMult = 9;
	if (iMult < 2) iMult = 2; // минимум x2

	return makefloat(iMult);
}

void SetShowWindowParameters(bool TVused, int w,int h, int l,int t,int r,int b)
{
	showWindow.TVused = TVused;
	showWindow.width = w;
	showWindow.height = h;
	showWindow.aspectRatio = 1.0;
	showWindow.left = RecalculateHIcon(l);
	showWindow.top = RecalculateVIcon(t);
	showWindow.right = w - RecalculateHIcon(w-r);
	showWindow.bottom = h - RecalculateVIcon(h-b);
	showWindow.sw = r-l;
	showWindow.sh = b-t;
	showWindow.scale =  makefloat(w) / 1024.0;
}

int RecalculateHIcon(int curHSize)
{
	return makeint(curHSize);
}

int RecalculateVIcon(int curVSize)
{
	return makeint(stf(showWindow.aspectRatio)*curVSize);
}

int RecalculateHIconScaled(int curHSize)
{
	return makeint(stf(showWindow.scale)*curHSize);
}

int RecalculateVIconScaled(int curVSize)
{
	return makeint(stf(showWindow.scale)*stf(showWindow.aspectRatio)*curVSize);
}

string GetVideoFileName(string baseName)
{
	if(baseName=="Credits")
	{
		baseName = "Credits_ru";
	}

	if( GetTargetPlatform()=="pc" ) return baseName+".wmv";
	return baseName+".xmv";
}

void DisableMenuLaunch(bool bDisable)
{
	if(bDisable) {
		InterfaceStates.LaunchDisable = true;
	} else {
		DeleteAttribute(&InterfaceStates,"LaunchDisable");
	}
}
bool IsEnableMenuLaunch()
{
	if( CheckAttribute(&InterfaceStates,"LaunchDisable") ) {
		return !sti(InterfaceStates.LaunchDisable);
	}
	return true;
}

void DisableFastTravel(bool bDisable)
{
	if(bDisable) {
		InterfaceStates.DisFastTravel = true;
	} else {
		DeleteAttribute(&InterfaceStates,"DisFastTravel");
	}
}
bool IsEnableFastTravel()
{
	if( CheckAttribute(&InterfaceStates,"DisFastTravel") ) {
		return !sti(InterfaceStates.DisFastTravel);
	}
	return true;
}

void procPreLoad()
{
	DeleteAttribute(&objActivePerkShower,"PerkList");
	SetTimeScale(1.0);
}

void ProcBreakInterface()
{
	if( CheckAttribute(&InterfaceStates,"Launched") &&
		sti(InterfaceStates.Launched)==true )
	{
		if( CurrentInterface == INTERFACE_RANSACK_MAIN || CurrentInterface == INTERFACE_FORTCAPTURE)
		{
			return;
		}
	}
	InterfaceStates.InstantExit = true;
	Event("exitCancel");
	DeleteAttribute(&InterfaceStates,"InstantExit");
}

bool g_bOptionsBreak = false;
void procOptionsBreak()
{
	g_bOptionsBreak = true;
}

string g_sTipsPath;
ref procGetTipsPath()
{
	string sLngID = LanguageGetLanguage();
	if(sLngID=="")	g_sTipsPath = "tips";
	else g_sTipsPath = "tips\" + sLngID;
	return &g_sTipsPath;
}

string g_sFaderPic;
ref procGetFaderPicture()
{
	string sPicName = GetEventData();
	g_sFaderPic = LanguageGetFaderPic(sPicName);
	return &g_sFaderPic;
}

void StartLanguageSetting(string lngID)
{
	if(lngID!="") LanguageSetLanguage(lngID);

	string segmentName = "Characters\" + LanguageGetLanguage() + "\Characters_names.c";
	if( LoadSegment(segmentName) )
	{
		InitCharactersNames();
		UnloadSegment(segmentName);
	} else {
		Trace("Error! Can`t Load Segment: " + segmentName);
	}

	segmentName = "Characters\" + LanguageGetLanguage() + "\Pirates_names.c";
	if( LoadSegment(segmentName) )
	{
		InitPiratesNames();
		UnloadSegment(segmentName);
	} else {
		Trace("Error! Can`t Load Segment: " + segmentName);
	}
	
	segmentName = "Characters\" + LanguageGetLanguage() + "\Indian_names.c";
	if( LoadSegment(segmentName) )
	{
		InitIndianNames();
		UnloadSegment(segmentName);
	} else {
		Trace("Error! Can`t Load Segment: " + segmentName);
	}

	segmentName = "Characters\" + LanguageGetLanguage() + "\Generator_names.c";
	if( LoadSegment(segmentName) )
	{
		InitGeneratorNames();
		UnloadSegment(segmentName);
	} else {
		Trace("Error! Can`t Load Segment: " + segmentName);
	}

	segmentName = "Ships\" + LanguageGetLanguage() + "\ships_name.c";
	if ( LoadSegment(segmentName) )
	{
		InitRandomShipsNames();
		UnloadSegment(segmentName);
	} else {
		Trace("Error! Can`t Load Segment: " + segmentName);
	}

	segmentName = LanguageGetLanguage() + "\Greetings_alias.ini";
	SendMessage( &Sound, "ls", MSG_SOUND_ALIAS_ADD,  segmentName);
}

int nWeelStep = 0;
void procMouseWeel()
{
	nWeelStep = GetEventData();
}
ref procGetMouseWeel()
{
	return &nWeelStep;
}

int SetAlphaIntoColor(int col, int alpha)
{
	int newCol = and(col,16777215) + shl(alpha,24);
	return newCol;
}

void SetSkillShow(string skillName, int skillVal)
{
	GameInterface.strings.(skillName) = skillVal;
	int color = COLOR_NORMAL;
	if(skillVal==SKILL_MAX)	color = COLOR_MONEY;
	ChangeStringColor(skillName,SetAlphaIntoColor(color, 55 + skillVal*2)); // 55-255
}

void SetSkillShowEx(aref xi_refCharacter, string skillName, int skillVal, int skillTempVal, int skillUpVal, int iX, int iY)
{
	string sReal = skillName+"real";
	int color = COLOR_NORMAL;

	if(skillVal >= MAX_CHARACTER_SKILL) color = COLOR_MONEY;
	int iResult = skillVal;

	if (skillTempVal > skillVal)
	{
		if (skillVal > 0)
		{
			iresult = skillVal;
		}
		else
		{
			iResult = 1;
		}
	}

	if (skillUPVal > iResult && pchar.id == xi_refCharacter.id)
	{
		color = COLOR_OK;

		if(skillVal < 1)
		{
			iResult = skillUPVal + skillVal;
		}
		else
		{
			iResult = skillUPVal;
		}
	}

	if (skillTempVal > skillVal)
	{
		color = COLOR_RED;
	}
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		skillName, its(iResult), FONT_BOLD_NUMBERS,iX,iY, SetAlphaIntoColor(color,GetAlphaFromSkill(iResult)),0,
		SCRIPT_ALIGN_RIGHT, true, 0.7, 420);

	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		sReal, its(skillTempVal), FONT_BOLD_NUMBERS,(iX+37),iY, SetAlphaIntoColor(COLOR_NORMAL,GetAlphaFromSkill(skillTempVal)),0,
		SCRIPT_ALIGN_RIGHT, true, 0.7, 420);
}

int GetAlphaFromSkill(int nskill)
{
	switch (nskill)
	{
    	case 0:		return 55;	break;
    	case 1:		return 75;	break;
    	case 2:		return 95;	break;
    	case 3:		return 115;	break;
    	case 4:		return 135;	break;
    	case 5:		return 155;	break;
    	case 6:		return 175;	break;
    	case 7:		return 195;	break;
    	case 8:		return 215;	break;
    	case 9:		return 235;	break;
    	case 10:	return 255;	break;
    	case 11:	return 255;	break;
    	case 12:	return 255;	break;
	}
	return 55;
}

//==============================
// save load section
//==============================
void MakeAutoReload()
{
	if( bPlayVideoNow ) {return;}
	string curSave = PlayerProfile.name + " AutoReloadSave";
	SetEventHandler("evntLoad","LoadAutoGame",0);
	PostEvent("evntLoad",0,"s", "SAVE\" + PlayerProfile.name+"\"+curSave);
}
void MakeQuickLoad()
{
	if( bPlayVideoNow ) {return;}
	GameInterface.SavePath = "SAVE\" + PlayerProfile.name;
    string saveName;
    int nSaveSize;
    SendMessage(&GameInterface,"llee",MSG_INTERFACE_SAVE_FILE_FIND,0,&saveName,&nSaveSize);
    string curSave = saveName;
	
	SetEventHandler("evntLoad","LoadGame",0);
	PostEvent("evntLoad",0,"s", GameInterface.SavePath+"\"+saveName);
}

void MakeQuickSave()
{
	if( bPlayVideoNow ) {return;}
	if( dialogRun ) {return;}  // лесник - если диялог ,то быстре сохарнение отменяется. лесник

	aref arTmp;
	if( GetEntity(arTmp,"fader") ) {return;}

	if (!CheckSaveGameEnabled()) {return;}

	if( InterfaceStates.Launched != 0 ) {return;}

	if (!QuickSaveGameEnabledHardcore())
	{
        Log_Info(GetConvertStrWithReplace("Variable_interface_1", "Interface.txt", "#space#", " "));
		return;
	}

	if(!CanSave()){
        Log_Info("SaveCheckFail");
		return;
	}
	SetTimeScale(1.0);
	TimeScaleCounter = 0;
	DelPerkFromActiveList("TimeSpeed");
	DeleteAttribute(pchar, "pause");

	aref arScrShoter;
	if( !GetEntity(&arScrShoter,"scrshoter") ) {
		SetEventHandler("makescrshot","QuickSaveContinue",0);
		CreateScreenShoter();
		PostEvent("makescrshot",1);
	} else { return; }
}

void QuickSaveContinue()
{
	DelEventHandler("makescrshot","QuickSaveContinue");

	LaunchQuickSaveMenu();
	int QuickSaveIndex = 1;
	if( CheckAttribute(&PlayerProfile,"QuickSaveIndex") ) {
		QuickSaveIndex = sti(PlayerProfile.QuickSaveIndex);
		if(QuickSaveIndex == 100) {
			QuickSaveIndex = 1;
		}
		else {
			QuickSaveIndex++;
		}
	}
	PlayerProfile.QuickSaveIndex = QuickSaveIndex;

	string curSave = PlayerProfile.name + " QuickSave " + QuickSaveIndex;
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,curSave);
	string sSaveDescriber = GetSaveDataString(GetCurLocationName());

	SetEventHandler("evntSave","SaveGame",1);
	PostEvent("evntSave",0,"ss", "SAVE\" + PlayerProfile.name+"\"+curSave, sSaveDescriber);
}

void MakeAutoSave()
{
	if( InterfaceStates.Launched != 0 ) {return;}

	aref arScrShoter;
	if( !GetEntity(&arScrShoter,"scrshoter") ) {
		SetEventHandler("makescrshot","AutoSaveContinue",0);
		CreateScreenShoter();
		PostEvent("makescrshot",1);
	} else { return; }
}

// Autosave rings by transition type.
// EnabledAutoSaveMode gates per-type toggles and ring sizes (1..10).
// Slot names: <profile> AutoSave <type> <N>.
// Saving is asynchronous: the screenshot needs a frame, and an active fader prevents SaveGame.
// Callers must defer the transition until evntSave; see AIAbordage.c.

// LEO: ключ типа -> имя чекбокса опции. Ключ идёт и в имя файла слота (латиница - безопасно для ФС).
string AutoSave_OptName(string sKey)
{
	if (sKey == "Board")  return "AutoSaveBoarding";
	if (sKey == "Sea")    return "AutoSaveToSea";
	if (sKey == "MapSea") return "AutoSaveMapToSea";
	if (sKey == "Land")   return "AutoSaveToLand";
	if (sKey == "Map")    return "AutoSaveToMap";
	return "";
}

// LEO: сколько слотов в кольце у типа (слайдер, 1..10)
int AutoSave_SlotsQty(string sKey)
{
	string sAttr = "AutoSaveSlots" + sKey;
	int n = 3;
	if (CheckAttribute(&InterfaceStates, sAttr)) n = sti(InterfaceStates.(sAttr));
	if (n < 1) n = 1;
	if (n > 10) n = 10;
	return n;
}

// LEO: запустить автосейв типа sKey. true = сохранение ПОШЛО, вызывающий обязан отложить переход до evntSave.
//   false = сохранять не надо/нельзя - переход выполняется сразу.
bool AutoSave_Start(string sKey)
{
	// Consume the matching one-shot key when the caller resumes after evntSave.
	// Discard stale keys belonging to other transition types.
	if (CheckAttribute(&InterfaceStates, "AutoSaveJustDone"))
	{
		string sDone = InterfaceStates.AutoSaveJustDone;
		DeleteAttribute(&InterfaceStates, "AutoSaveJustDone");
		if (sDone == sKey) return false;
	}
	if (bHardcoreGame) return false;                                            // в хардкоре автосейвов нет
	if (!CheckAttribute(&InterfaceStates, "EnabledAutoSaveMode")) return false;  // мастер-тумблер
	if (sti(InterfaceStates.EnabledAutoSaveMode) == 0) return false;
	string sOpt = AutoSave_OptName(sKey);
	if (sOpt == "") return false;
	// чекбокс конкретного типа. Опции ещё нет (старый профиль/сейв) - берём ВАНИЛЬНЫЙ дефолт:
	//   абордаж включён, прочие типы выключены. Так новый игрок получает привычное поведение.
	int iOptOn = 0;
	if (sKey == "Board") iOptOn = 1;
	if (CheckAttribute(&InterfaceStates, sOpt)) iOptOn = sti(InterfaceStates.(sOpt));
	if (iOptOn == 0) return false;
	if (!CanSave()) return false;               // «нигде» (середина релоада) - сохранять нечего
	if (InterfaceStates.Launched != 0) return false;
	aref arScrShoter;
	if (GetEntity(&arScrShoter, "scrshoter")) return false; // уже идёт другой снимок - не мешаем

	// Advance the ring only after every refusal check, so skipped saves do not consume slots.
	string sIdxAttr = "AutoSaveIdx" + sKey;
	int iMax = AutoSave_SlotsQty(sKey);
	int idx = 1;
	if (CheckAttribute(&PlayerProfile, sIdxAttr)) idx = sti(PlayerProfile.(sIdxAttr)) + 1;
	if (idx > iMax) idx = 1;
	if (idx < 1) idx = 1;
	PlayerProfile.(sIdxAttr) = idx;

	InterfaceStates.AutoSaveSlotName = PlayerProfile.name + " AutoSave " + sKey + " " + idx;
	SetEventHandler("makescrshot","AutoSaveContinue",0);
	CreateScreenShoter();
	PostEvent("makescrshot",1);
	InterfaceStates.AutoSaveJustDone = sKey; // повторный заход этого типа сохранять не будет (см. гард выше)
	return true;
}

void AutoSaveContinue()
{
	DelEventHandler("makescrshot","AutoSaveContinue");

	// LEO: имя слота задаёт AutoSave_Start (тип + номер в кольце); без него - старый единственный слот
	string curSave = PlayerProfile.name + " AutoSave";
	if (CheckAttribute(&InterfaceStates, "AutoSaveSlotName"))
	{
		curSave = InterfaceStates.AutoSaveSlotName;
		DeleteAttribute(&InterfaceStates, "AutoSaveSlotName");
	}
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,curSave);
	string sSaveDescriber = GetSaveDataString(GetCurLocationName());

	SetEventHandler("evntSave","SaveGame",1);
	PostEvent("evntSave",0,"ss", "SAVE\" + PlayerProfile.name+"\"+curSave, sSaveDescriber);
}

string GetSaveDataString(string label)
{
	string fighter1, fighter2, fighter3, fighter4, fighter5, fighter6, fighter7, fighter8, fighter9, fighter10, fighter11, navigator, boatswain, cannoner, doctor, treasurer, carpenter;

	// officers pictures
	if( GetOfficersIndex(pchar,0) < 0 ) { fighter1 = "*";
	} else { fighter1 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,0) );
	}
	if( GetOfficersIndex(pchar,1) < 0 ) { fighter2 = "*";
	} else { fighter2 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,1) );
	}
	if( GetOfficersIndex(pchar,2) < 0 ) { fighter3 = "*";
	} else { fighter3 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,2) );
	}
	if( GetOfficersIndex(pchar,3) < 0 ) { fighter4 = "*";
	} else { fighter4 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,3) );
	}
	if( GetOfficersIndex(pchar,4) < 0 ) { fighter5 = "*";
	} else { fighter5 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,4) );
	}
	if( GetOfficersIndex(pchar,5) < 0 ) { fighter6 = "*";
	} else { fighter6 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,5) );
	}
	if( GetOfficersIndex(pchar,6) < 0 ) { fighter7 = "*";
	} else { fighter7 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,6) );
	}
	if( GetOfficersIndex(pchar,7) < 0 ) { fighter8 = "*";
	} else { fighter8 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,7) );
	}
	if( GetOfficersIndex(pchar,8) < 0 ) { fighter9 = "*";
	} else { fighter9 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,8) );
	}
	if( GetOfficersIndex(pchar,9) < 0 ) { fighter10 = "*";
	} else { fighter10 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,9) );
	}
	if( GetOfficersIndex(pchar,10) < 0 ) { fighter11 = "*";
	} else { fighter11 = GetMinifiedFaceGroupName( GetOfficersIndex(pchar,10) );
	}
	// specials pictures
	if( !CheckAttribute(pchar,"Fellows.Passengers.navigator") || sti(pchar.Fellows.Passengers.navigator)<0 ) { navigator = "*";
	} else { navigator = GetMinifiedFaceGroupName( sti(pchar.Fellows.Passengers.navigator) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.boatswain") || sti(pchar.Fellows.Passengers.boatswain)<0 ) { boatswain = "*";
	} else { boatswain = GetMinifiedFaceGroupName( sti(pchar.Fellows.Passengers.boatswain) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.cannoner") || sti(pchar.Fellows.Passengers.cannoner)<0 ) { cannoner = "*";
	} else { cannoner = GetMinifiedFaceGroupName( sti(pchar.Fellows.Passengers.cannoner) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.doctor") || sti(pchar.Fellows.Passengers.doctor)<0 ) { doctor = "*";
	} else { doctor = GetMinifiedFaceGroupName( sti(pchar.Fellows.Passengers.doctor) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.treasurer") || sti(pchar.Fellows.Passengers.treasurer)<0 ) { treasurer = "*";
	} else { treasurer = GetMinifiedFaceGroupName( sti(pchar.Fellows.Passengers.treasurer) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.carpenter") || sti(pchar.Fellows.Passengers.carpenter)<0 ) { carpenter = "*";
	} else { carpenter = GetMinifiedFaceGroupName( sti(pchar.Fellows.Passengers.carpenter) );
	}
	string savedata = label +
		"@" + fighter1 + "," + fighter2 + "," + fighter3 + "," + fighter4 + "," + fighter5 + "," + fighter6 + "," + fighter7 + "," + fighter8 + "," + fighter9 + "," + fighter10 + "," + fighter11 + "," +
		navigator + "," + boatswain + "," + cannoner + "," +
		doctor + "," + treasurer + "," + carpenter +
		"@" + GetStringTime(GetTime()) + " " + GetStringDate( GetDataDay(),GetDataMonth(),GetDataYear() ) +
		"@" + GetPlayTime() +
		"@" + GetCurShip() +
		"@" + GetCurNation() +
		"@" + GetCurDifficulty() +
		"@" + GetCurMoney() +
		"@" + GetCurRank() +
		"@" + LanguageGetLanguage();

	return savedata;
}

string GetCurLocationName()
{
	if( nMainCharacterIndex < 0 ) return "";
	if( !CheckAttribute(pchar,"location") ) return "";
	string locLabel = pchar.location;
	int locidx = FindLocation(pchar.location);
	if( locidx>=0 )
	{
		int nLablesFileID = LanguageOpenFile("LocLables.txt");
        locLabel = "";
		if (CheckAttribute(&locations[locidx], "fastreload"))
		{
			locLabel += LanguageConvertString(nLablesFileID, Locations[locidx].fastreload + " Town") + " ";
		}
		if (CheckAttribute(&Locations[locidx],"id.label") )
		{
			locLabel += LanguageConvertString(nLablesFileID, Locations[locidx].id.label);
		}
		LanguageCloseFile(nLablesFileID);
	}
	else
	{
		if (worldMap.island != "")
		{
            locLabel =  worldMap.island;
			if (locLabel == "Cuba1") locLabel = "Cuba";
			if (locLabel == "Cuba2") locLabel = "Cuba";
			if (locLabel == "Hispaniola1") locLabel = "Hispaniola";
            if (locLabel == "Hispaniola2") locLabel = "Hispaniola";
			locLabel = GetConvertStr(locLabel, "LocLables.txt");
			if (locLabel == "")
      		{
      		    locLabel = GetConvertStr("Mein", "LocLables.txt");
      		}
			locLabel  += " - " + XI_ConvertString("Sea");
		}
		else
		{
			locLabel = XI_ConvertString("Open Sea");
		}
	}
	return locLabel;
}

string GetCurLocationName1()
{
	if( nMainCharacterIndex < 0 ) return "";
	if( !CheckAttribute(pchar,"location") ) return "";
	string locLabel = pchar.location;
	int locidx = FindLocation(pchar.location);
	if( locidx>=0 )
	{
		int nLablesFileID = LanguageOpenFile("LocLables.txt");
        locLabel = "";
		if (CheckAttribute(&locations[locidx], "fastreload"))
		{
			locLabel += LanguageConvertString(nLablesFileID, Locations[locidx].fastreload + " Town") + " ";
		}
		if (CheckAttribute(&Locations[locidx],"id.label") )
		{
			locLabel += LanguageConvertString(nLablesFileID, Locations[locidx].id.label);
		}
		LanguageCloseFile(nLablesFileID);
	}
	else
	{
		if (worldMap.island != "")
		{
            locLabel =  worldMap.island;

			int iIslandIndex = FindIsland(locLabel);
			if (iIslandIndex != -1 && Islands[iIslandIndex].visible == true)
			{
				if (locLabel == "Cuba1") locLabel = "Cuba";
				if (locLabel == "Cuba2") locLabel = "Cuba";
				if (locLabel == "Hispaniola1") locLabel = "Hispaniola";
				if (locLabel == "Hispaniola2") locLabel = "Hispaniola";
				locLabel = GetConvertStr(locLabel, "LocLables.txt");
				if (locLabel == "")
				{
					locLabel = GetConvertStr("Mein", "LocLables.txt");
				}
				locLabel  += " - " + XI_ConvertString("Sea");
			}
			else
			{
				locLabel = XI_ConvertString("Open Sea");
			}
		}
		else
		{
			locLabel = XI_ConvertString("Open Sea");
		}
	}
	return locLabel;
}

string GetPlayTime()
{
	int hours = sti( InterfaceStates.GameTime.hour );
	int days = hours / 24;
	hours = hours - days*24;
	string sPlayTime = "";
	if( days>0 ) {
		sPlayTime = days + GetConvertStrWithReplace("Variable_interface_2", "Interface.txt", "#space#", " ");
	}
	sPlayTime += hours + GetConvertStrWithReplace("Variable_interface_3", "Interface.txt", "#space#", " ") + InterfaceStates.GameTime.min + GetConvertStrWithReplace("Variable_interface_4", "Interface.txt", "#space#", " ");
	return sPlayTime;
}

string GetCurShip()
{
	int iShip = pchar.ship.type;
	if (iShip != SHIP_NOTUSED)
	{
		ref refBaseShip = GetRealShip(iShip);
		string sShip = refBaseShip.BaseName;
		return sShip;
	}
	else return "1000";
}

string GetCurNation()
{
	return GetNationNameByType(sti(pchar.nation));
}

string GetCurDifficulty()
{
	return its(MOD_SKILL_ENEMY_RATE);
}

string GetCurMoney()
{
	return pchar.money;
}

string GetCurRank()
{
	return pchar.rank;
}

void LaunchContrabandTrade(ref ContraTrader, int storeNum)
{
	if(storeNum<0)	return;
	if(storeNum>STORE_QUANTITY-1)	return;
	gStoreNum=storeNum;
	if(procInterfacePrepare(INTERFACE_CONTRATRADE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CONTRATRADE;
		InitInterface_RR(Interfaces[CurrentInterface].IniFile, ContraTrader, &stores[storeNum]);
	}
}

string GetStringTime(float time)
{
	int hour = makeint(time);
	int minute = makeint((time - hour) * 60.0 + 0.01);
	string retVal;
	if(hour<10) retVal = "0"+hour;
	else retVal = its(hour);
	retVal += ":";
	if(minute<10) retVal += "0"+minute;
	else retVal += its(minute);
	return retVal;
}

string GetStringDate(int day, int month, int year)
{
	string result = "";
	if (day < 10)
	{
	    result = result + "0";
	}
	result = result + day + ".";
	if (month < 10)
	{
	    result = result + "0";
	}
	result = result + month + ".";
	if (year < 10)
	{
	    result = result + "0";
	}
	result = result + year;
	return result;
}

void ISetSaveData(string saveName, string saveData)
{
	SendMessage(&GameInterface,"lss",MSG_INTERFACE_DO_SAVE_DATA,saveName,saveData);
}

string IGetSaveString(string saveName)
{
	string retStr;
	SendMessage(&GameInterface,"lse",MSG_INTERFACE_GET_SAVE_DATA,saveName,&retStr);
	return retStr;
}

string GetConvertStr(string _param, string _file)
{
    int    idLngFile = -1;
    string totalInfo;

    idLngFile = LanguageOpenFile(_file);
    totalInfo = LanguageConvertString(idLngFile, _param);
    LanguageCloseFile(idLngFile);
    return totalInfo;
}

string GetConvertStrWithReplace(string _param, string _file, string toRemove, string toPlace)
{
    int    idLngFile = -1;
    string totalInfo;

    idLngFile = LanguageOpenFile(_file);
    totalInfo = LanguageConvertString(idLngFile, _param);
    LanguageCloseFile(idLngFile);
	totalInfo = strreplace(totalInfo, toRemove, toPlace);
    return totalInfo;
}

// Gregg: сокращение частой идиомы — строка из Interface.txt с заменой #space# на пробел.
// xiStr is reserved for XI_ConvertString.
string iTxt(string id)
{
	return GetConvertStrWithReplace(id, "Interface.txt", "#space#", " ");
}

void GetXYWindowOffset(ref offsetX, ref offsetY)
{
	float dwScreenHeight 			= 600.0;
	float dwScreenWidth 			= sti(showWindow.width) * dwScreenHeight/ sti(showWindow.height);
	if(dwScreenWidth < 800.0)  dwScreenWidth = 800.0;

	float offX = (dwScreenWidth - 800.0)/2.0;
	float offY = 0.0;

	offsetX = offX;
	offsetY = offY;
}
