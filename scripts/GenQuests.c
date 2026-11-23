// Генераторы. Отдельный файл
// ситуации на захваченном корабле - "Взрыв" или "Эпидемия"
#define ShipSituation_0				0 	// "Взрыв на корабле" или "Эпидемия" - обшарили каюту или труп
#define ShipSituation_1				1	// "Взрыв на корабле" или "Эпидемия" - любое использовование интерфейса "грабежа" --> перегрузка отдельных товаров или нажатие кнопки "Взять все" - после этого корабль взрывается или его сами топим
#define ShipSituation_2				2	// "Эпидемия" - сажаем компаньона с командой на инфицированный корабль
#define ShipSituation_3				3	// "Эпидемия" - ГГ пересаживается на инфицированный корабль
#define ShipSituation_4				4	// ничего не обшаривали

// LEO: крысы-безбилетники из выловленной бочки (Barrel_SetRatsStowaway / Barrel_RatsCheck)
#define BARREL_RATS_CHANCE			15	// базовый шанс выводка, %; из него вычитается удача (при 10 удачи остаётся 5%)
#define BARREL_RATS_MIN_DAYS		5	// через сколько дней выводок отъестся
#define BARREL_RATS_MAX_DAYS		10
#define BARREL_RATS_EAT_MIN_PCT		60	// сколько провианта с корабля ГГ сожрут разом, %
#define BARREL_RATS_EAT_MAX_PCT		80
#define BARREL_RATS_TEST_MODE		0	// LEO: ТЕСТ крыс: 0=боевой, 1=выводок с каждой бочки и жрёт на следующий день

int iGenRank;
string sGenLocation, sGenlocator, sGenBlade, sGenGun, sQuestTitle;

void Rat_GenerateRat()
{
	/* Скопировать файл Rat.gm (модель) в Resource/Models/Characters
	Файл Rat.tx (текстуру) в Resource/Textures/Characters
	Настроить дебагер - В файле Engine.ini поменять debugwindow = 0 на debugwindow = 1
	Потом по F5 в игре в левой части набрать Rat_GenerateRat();
	Модельки маленькие - с первого раза можно не заметить.
	Остальные коменты по ходу...*/

	ref rChar;
	int iRatCount = 1; // Скока крыс будем генерить?
	for(int i=1; i<=iRatCount; i++)
	{
		rChar = GetCharacter(NPC_GenerateCharacter("Test_Rat_"+i, "rat", "man", "rat", sti(PChar.rank)*5, PIRATE, -1, true));
		SetFantomParamFromRank(rChar, sti(PChar.rank)*5, true); // Крутая крыса!
		rChar.Dialog.FileName = "GenQuests_Dialog.c";
		LAi_SetStayType(rChar); // ТИП ! От этого анимация и зависит как-раз.
		sGenLocation = "goto"; // Если нуна не goto, то поменять тут
		sGenlocator = FindNearestFreeLocator(sGenLocation); // Ближайший свободный локатор группы goto
		ChangeCharacterAddressGroup(rChar, PChar.location, sGenLocation, sGenlocator); // Перенесем в локу к ГГ
	}
}

//=====================================================================================================================================
// Генератор контрабандистов. Доставка письма -->
//=====================================================================================================================================
void ContraDeliverQuest_EnterToTavernRoom(string sQuest) // Зашли в комнату наверху
{
	sGenLocation = PChar.GenQuest.ContraDeliver.ToColony;
	LocatorReloadEnterDisable(sGenLocation + "_Tavern", "reload2_back", true); // Закроем сразу
	chrDisableReloadToLocation = true;
	bDisableFastReload         = true;
	DoQuestFunctionDelay("ContraDeliverQuest_ReloadConterToRoom", 3.0); // Ждем 3 сек.
}

void ContraDeliverQuest_ReloadConterToRoom(string sQuest) // генерим клона контрабандиста
{
	ref rChar = GetCharacter(NPC_GenerateCharacter("ContraDeliverQuest_Contra_1", PChar.GenQuest.ContraDeliver.Model, "man", PChar.GenQuest.ContraDeliver.Model.Animation, sti(PChar.rank)*2, PIRATE, 0, true));
	rChar.DontClearDead = true;
	SetFantomParamFromRank(rChar, sti(PChar.rank)*2, true);
	rChar.Name = GetConvertStrWithReplace("Variable_GenQuests_1", "Names.txt", "#space#", " ");
	rChar.LastName = "";
	rChar.greeting = "Gr_Smuggler Agent";
	LAi_SetActorType(rChar);
	rChar.Dialog.FileName = "GenQuests_Dialog.c";
	LAi_SetImmortal(rChar, true);
	ChangeCharacterAddressGroup(rChar, PChar.location, "reload", "reload1_back");
	LAi_ActorDialog(rChar, PChar, "", -1, 5);
}

void ContraDeliverQuest_ContraIsDead(string sQuest)
{
	ref rChar = &characters[GetCharacterIndex(sGenLocation + "_Smuggler")];
	rChar.model    = "pirate_" + (rand(9)+1); // Меняем модель (типо, туда уже другой пришёл)
	FaceMaker(rChar);
	chrDisableReloadToLocation = false;
	bDisableFastReload         = false;
	ChangeContrabandRelation(PChar, -10);
	sQuestTitle = PChar.GenQuest.ContraDeliver.QuestTown + "ContraDeliverQuest";
	AddQuestRecordEx(sQuestTitle, "ContraDeliverQuest", "5");
	AddQuestUserData(sQuestTitle, "sColony", XI_ConvertString("Colony" + PChar.GenQuest.ContraDeliver.ToColony + "Gen"));
	CloseQuestHeader(sQuestTitle);
	DeleteAttribute(PChar, "GenQuest.ContraDeliver");
}

void ContraDeliverQuest_GeneratePatrolToRoom()
{
	ref rChar;
	sGenLocation = PChar.GenQuest.ContraDeliver.ToColony;
	int iNation = sti(characters[GetCharacterIndex(sGenLocation + " Fort Commander")].nation);
	string sShortNation = NationShortName(iNation); // Для модельки
	string sModel[3];
	sModel[0] = "off_" + sShortNation + "_1";
	sModel[1] = "sold_" + sShortNation + "_1";
	sModel[2] = "sold_" + sShortNation + "_2";
	string sGenGroup[3];
	sGenGroup[0] = "reload";
	sGenGroup[1] = "Quest";
	sGenGroup[2] = "Quest";
	string sGenLocatorArroy[3];
	sGenLocatorArroy[0] = "reload1_back";
	sGenLocatorArroy[1] = "Quest1";
	sGenLocatorArroy[2] = "Quest2";
	for(int i=0; i<=2; i++)
	{
		rChar = GetCharacter(NPC_GenerateCharacter("ContraDeliverQuest_Soldier_" + i, sModel[i], "man", "man", sti(PChar.rank)*2, iNation, 0, true));
		SetFantomParamFromRank(rChar, sti(PChar.rank)*2, true);
		rChar.Name = GetConvertStrWithReplace("Variable_GenQuests_2", "Names.txt", "#space#", " ");
		rChar.LastName = "";
		rChar.Greeting = "soldier_arest";
		LAi_SetActorType(rChar);
		rChar.Dialog.FileName = "GenQuests_Dialog.c";
		LAi_SetImmortal(rChar, true);
		ChangeCharacterAddressGroup(rChar, PChar.location, sGenGroup[i], sGenLocatorArroy[i]);
		if(i != 0) LAi_CharacterDisableDialog(rChar);
		else LAi_ActorDialog(rChar, PChar, "", -1, 5);
	}

	rChar = CharacterFromID("ContraDeliverQuest_Contra_1");
	LAi_ActorGoToLocator(rChar, "goto", "goto2", "", 3);
}

void ContraDeliverQuest_SoldiersGoOut()
{
	ref rChar;
	for(int i=0; i<=2; i++)
	{
		rChar = CharacterFromID("ContraDeliverQuest_Soldier_" + i);
		LAi_ActorGoToLocation(rChar, "reload", "reload1_back", "none", "", "", "", 3.0);
	}

	rChar = CharacterFromID("ContraDeliverQuest_Contra_1");
	PChar.GenQuest.CallFunctionParam = "ContraDeliverQuest_AfterSoldiersOut";
	LAi_ActorGoToLocation(rChar, "reload", "reload1_back", "none", "", "", "CallFunctionParam", 3.0);
}

void ContraDeliverQuest_AfterSoldiersOut()
{
	chrDisableReloadToLocation = false;
	bDisableFastReload         = false;
	sQuestTitle = PChar.GenQuest.ContraDeliver.QuestTown + "ContraDeliverQuest";
	AddQuestRecordEx(sQuestTitle, "ContraDeliverQuest", "7");
	CloseQuestHeader(sQuestTitle);
	DeleteAttribute(PChar, "GenQuest.ContraDeliver");
}
//=====================================================================================================================================
// <-- Генератор контрабандистов. Доставка письма
//=====================================================================================================================================

//=====================================================================================================================================
// Генератор контрабандистов. Встреча в бухте -->
//=====================================================================================================================================
void ContraMeetManQuest_InitStartParam() // Инициализация стартовых параметров
{
	if(rand(1) == 0)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_1", "Logs.txt", "#space#", " "));
		PChar.GenQuest.ContraMeetMan.SoldiersInShore = true;
	}
	else
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_2", "Logs.txt", "#space#", " "));
		if(rand(1) == 0)
		{
			Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_3", "Logs.txt", "#space#", " "));
			PChar.GenQuest.ContraMeetMan.ManIsTraitor = true;
		}
		else Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_4", "Logs.txt", "#space#", " "));
	}
}

void ContraMeetManQuest_EnterToShore(string sQuest) // Вошли в бухту
{
	ref rChar;
	if(!CheckAttribute(PChar, "GenQuest.ContraMeetMan.SoldiersInShore")) //Встречаем чела
	{
		rChar = GetCharacter(NPC_GenerateCharacter("ContraMeetManQuest_QuestMan", "citiz_"+(rand(11)+1), "man", "man", sti(PChar.rank)*2, PIRATE, -1, true));
		SetFantomParamFromRank(rChar, sti(PChar.rank)*2, true);
		rChar.Greeting = "cit_common";
		LAi_SetCitizenType(rChar);
		rChar.Dialog.FileName = "GenQuests_Dialog.c";
		rChar.DontClearDead = true;
		LAi_SetImmortal(rChar, true); // До поры нельзя завалить
		ChangeCharacterAddressGroup(rChar, PChar.location, "goto", "goto1");
	}
	else // Солдеры
	{
		sGenLocation = PChar.GenQuest.ContraMeetMan.QuestTown;
		int iNation = sti(characters[GetCharacterIndex(sGenLocation + " Fort Commander")].nation);
		string sShortNation = NationShortName(iNation); // Для модельки
		int iSoldCount = 4+rand(2);
		PChar.GenQuest.ContraMeetMan.SoldCount = iSoldCount; // Запомнит, т.к. потом их нужно удалять
		for(int i=0; i<=iSoldCount; i++)
		{
			sGenlocator = LAi_FindRandomLocator("goto");
			if(i==0) // Офицер
			{
				rChar = GetCharacter(NPC_GenerateCharacter("ContraMeetManQuest_Officer_" + i, "off_"+sShortNation+"_1", "man", "man", sti(PChar.rank)*2, iNation, -1, true));
				SetFantomParamFromRank(rChar, sti(PChar.rank)*2, true);
				rChar.Name = GetConvertStrWithReplace("Variable_GenQuests_3", "Names.txt", "#space#", " ");
				rChar.LastName = "";
			}
			else // Солдаты
			{
				rChar = GetCharacter(NPC_GenerateCharacter("ContraMeetManQuest_Soldier_" + i, "sold_"+sShortNation+"_1", "man", "man", sti(PChar.rank), iNation, -1, true));
				SetFantomParamFromRank(rChar, sti(PChar.rank), true);
				rChar.Name = GetConvertStrWithReplace("Variable_GenQuests_4", "Names.txt", "#space#", " ");
				rChar.LastName = "";
			}

			rChar.Greeting = "soldier_arest";
			LAi_SetCitizenType(rChar);
			rChar.Dialog.FileName = "GenQuests_Dialog.c";
			LAi_SetImmortal(rChar, true);
			ChangeCharacterAddressGroup(rChar, PChar.location, "goto", sGenlocator);
		}
	}
}

void ContraMeetManQuest_DeletePatrolFromShore()
{
	int iSoldCount = sti(PChar.GenQuest.ContraMeetMan.SoldCount);
	ref rChar;
	for(int i=0; i<=iSoldCount; i++)
	{
		if(i==0) rChar = CharacterFromID("ContraMeetManQuest_Officer_"+i);
		else rChar = CharacterFromID("ContraMeetManQuest_Soldier_"+i);

		rChar.LifeDay = 0;
	}
}

void ContraMeetManQuest_TimeIsLeft(string sQuest) // Вышло время
{
	DeleteQuestCondition("ContraMeetManQuest_EnterToShore"); // Завершим прерывание на вход в бухту
	if(GetCharacterIndex("ContraMeetManQuest_QuestMan") != -1)
	{
		ref rChar = CharacterFromID("ContraMeetManQuest_QuestMan");
		rChar.LifeDay = 0;
		ChangeCharacterAddressGroup(rChar, "none", "", "");
	}

	if(CheckAttribute(PChar, "Quest.ContraMeetManQuest_EnterToTavernRoom"))
	{
		LocatorReloadEnterDisable(PChar.GenQuest.ContraMeetMan.QuestTown + "_tavern", "reload2_back", true); // Закроем комнату
		DeleteQuestCondition("ContraMeetManQuest_EnterToTavernRoom");
	}

	sQuestTitle = PChar.GenQuest.ContraMeetMan.QuestTown + "ContraMeetManQuest";
	AddQuestRecordEx(sQuestTitle, "ContraMeetManQuest", "2");
	AddQuestUserData(sQuestTitle, "sDestination", XI_ConvertString(PChar.GenQuest.ContraMeetMan.ShoreID + "Dat"));
	CloseQuestHeader(sQuestTitle);
	DeleteAttribute(&Locations[FindLocation(PChar.GenQuest.ContraMeetMan.ShoreID)], "DisableEncounters");
	DeleteAttribute(PChar, "GenQuest.ContraMeetMan");
}

void ContraMeetManQuest_EnterToTavernRoom(string sQuest)
{
	sGenLocation = PChar.GenQuest.ContraMeetMan.QuestTown;
	DeleteQuestCondition("ContraMeetManQuest_TimeIsLeft"); // Завершаем прерывание на таймер
	LocatorReloadEnterDisable(sGenLocation + "_Tavern", "reload2_back", true); // Закроем комнату
	DisableAllExits(true);
	DoQuestFunctionDelay("ContraMeetManQuest_ReloadConterToRoom", 3.0);
}

void ContraMeetManQuest_ReloadConterToRoom(string sQuest)
{
	ref rChar;
	string sGenGroup[2];
	sGenGroup[0] = "reload";
	sGenGroup[1] = "Quest";
	string sGenLocatorArroy[2];
	sGenLocatorArroy[0] = "reload1_back";
	sGenLocatorArroy[1] = "Quest2";
	for(int i=0; i<=1; i++)
	{
		rChar = GetCharacter(NPC_GenerateCharacter("ContraMeetManQuest_Contrick_" + i, "Pirate_"+(rand(9)+1), "man", "man", sti(PChar.rank)*2, PIRATE, 0, true));
		SetFantomParamFromRank(rChar, sti(PChar.rank)*2, true);
		rChar.Name = GetConvertStrWithReplace("Variable_GenQuests_5", "Names.txt", "#space#", " ");
		rChar.LastName = "";
		rChar.Greeting = "Gr_Smuggler Agent";
		LAi_SetActorType(rChar);
		rChar.Dialog.FileName = "GenQuests_Dialog.c";
		LAi_SetImmortal(rChar, true);
		rChar.SuperShooter  = true; // даный НПС будет чаще юзать пистоль (вероятность от сложности, но в разы выше, чем у обычных, близка офам)
		ChangeCharacterAddressGroup(rChar, PChar.location, sGenGroup[i], sGenLocatorArroy[i]);
		if(i != 0) LAi_CharacterDisableDialog(rChar);
		else LAi_ActorDialog(rChar, PChar, "", -1, 5);
	}

	rChar = CharacterFromID("ContraMeetManQuest_QuestMan"); // Отправляем чела в дальний локатор, чтоб не мешал
	LAi_ActorGoToLocator(rChar, "goto", "goto2", "", 3);
}

void ContraMeetManQuest_ContersGoQut() // Контрики сваливают из комнаты вместе с челом
{
	ref rChar;
	for(int i=0; i<=1; i++)
	{
		if(GetCharacterIndex("ContraMeetManQuest_Contrick_"+i) != -1 && !LAi_isDead(CharacterFromID("ContraMeetManQuest_Contrick_"+i)))
		{
			rChar = CharacterFromID("ContraMeetManQuest_Contrick_" + i);
			LAi_Type_Actor_Reset(rChar);
			LAi_ActorGoToLocation(rChar, "reload", "reload1_back", "none", "", "", "", 3.0);
		}
	}

	PChar.GenQuest.CallFunctionParam = "ContraMeetManQuest_AfterContersOut";
	rChar = CharacterFromID("ContraMeetManQuest_QuestMan");
	LAi_Type_Actor_Reset(rChar);
	LAi_ActorGoToLocation(rChar, "reload", "reload1_back", "none", "", "", "CallFunctionParam", 3.0);
}

void ContraMeetManQuest_AfterContersOut()
{
	DisableAllExits(false); // Откроем выходы
	ref rChar = &Characters[GetCharacterIndex(Locations[FindLocation(PChar.location)].townsack + "_Smuggler")];
	SaveCurrentNpcQuestDateParam(rChar, "work_date");
}

void ContraMeetManQuest_ContraBeatMan() // Контрики начинают мочить чела
{
	LAi_Group_Delete("ContraMeetManQuest_Group1"); // Потрем прежние группы
	LAi_Group_Delete("ContraMeetManQuest_Group2");
	DisableAllExits(false); // Откроем выходы
	ref rChar = CharacterFromID("ContraMeetManQuest_QuestMan");
	rChar.LifeDay = 0;
	LAi_SetImmortal(rChar, false);
	LAi_Group_MoveCharacter(rChar, "ContraMeetManQuest_Group1");
	for(int i=0; i<=1; i++)
	{
		rChar = CharacterFromID("ContraMeetManQuest_Contrick_"+i);
		LAi_Group_MoveCharacter(rChar, "ContraMeetManQuest_Group2");
	}
	LAi_Group_SetRelation("ContraMeetManQuest_Group1", "ContraMeetManQuest_Group2", LAI_GROUP_ENEMY);
	LAi_Group_FightGroups("ContraMeetManQuest_Group1", "ContraMeetManQuest_Group2", true);
	SetFunctionNPCDeathCondition("ContraMeetManQuest_ContraBeatMan_Complete", "ContraMeetManQuest_QuestMan", false);
}

void ContraMeetManQuest_ContraBeatMan_Complete(string sQuest)
{
	// Их может уже не быть
	if(GetCharacterIndex("ContraMeetManQuest_Contrick_0") == -1 || GetCharacterIndex("ContraMeetManQuest_Contrick_1") == -1) return;

	ref rChar;
	for(int i=0; i<=1; i++)
	{
		rChar = CharacterFromID("ContraMeetManQuest_Contrick_"+i);
		LAi_SetActorType(rChar);
		LAi_ActorGoToLocation(rChar, "reload", "reload1_back", "none", "", "", "", 3.0);
	}
}

void ContraMeetManQuest_PlayerBeatContra()
{
	LAi_Group_Delete("ContraMeetManQuest_Group1"); // Потрем прежние группы
	ref rChar = CharacterFromID("ContraMeetManQuest_QuestMan");
	rChar.LifeDay = 0;
	LAi_Group_MoveCharacter(rChar, LAI_GROUP_PLAYER);
	for(int i=0; i<=1; i++)
	{
		rChar = CharacterFromID("ContraMeetManQuest_Contrick_"+i);
		LAi_SetImmortal(rChar, false);
		LAi_Group_MoveCharacter(rChar, "ContraMeetManQuest_Group1");
	}
	LAi_Group_SetRelation("ContraMeetManQuest_Group1", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_Group_FightGroups("ContraMeetManQuest_Group1", LAI_GROUP_PLAYER, true);
	LAi_SetFightMode(PChar, true);
	ChangeContrabandRelation(PChar, -15); // Хотим рубнуть контров - отношение в минус
	SetGameModeCondition("ContraMeetManQuest_CheckContraDead");
}

void ContraMeetManQuest_CheckContraDead()
{
	if(LAi_isDead(CharacterFromID("ContraMeetManQuest_Contrick_0")) && LAi_isDead(CharacterFromID("ContraMeetManQuest_Contrick_1")))
	{
		DeleteGameModeCondition("ContraMeetManQuest_CheckContraDead");
		Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_5", "Logs.txt", "#space#", " "));
		LAi_SetFightMode(PChar, false);
		LAi_LocationFightDisable(LoadedLocation, true);
		ref rChar = CharacterFromID("ContraMeetManQuest_QuestMan");
		PChar.GenQuest.ContraMeetMan.ContraIsDead = true;
		LAi_SetActorType(rChar);
		LAi_ActorDialog(rChar, PChar, "", -1, 5);
	}
}
//=====================================================================================================================================
// <-- Генератор контрабандистов. Встреча в бухте
//=====================================================================================================================================

//=====================================================================================================================================
// Миниквест жителей. Поиск обручального кольца -->
//=====================================================================================================================================
void CitizenFindItemMiniQuest_CheckItem()
{
	if(CheckCharacterItem(PChar, "DOjeronRing")) // Проверка незначительная - тормозов не будет
	{
		DeleteGameModeCondition("CitizenFindItemMiniQuest_CheckItem");
		PChar.GenQuest.CitizenFindItem.GiveItem = true; // Флаг - взяли предмет
		string sCity = XI_ConvertString("Colony" + PChar.GenQuest.CitizenFindItem.City + "Gen");
		Log_Info(GetConvertStrWithReplace("Variable_GenQuests_6", "Logs.txt", "#space#", " ") + sCity + GetConvertStrWithReplace("Variable_GenQuests_7", "Logs.txt", "#space#", " "));
	}
}

void CitizenFindItemMiniQuest_Timer(string sQuest)
{
	DeleteGameModeCondition("CitizenFindItemMiniQuest_CheckItem");
	if(CheckAttribute(PChar, "GenQuest.CitizenFindItem.GiveItem")) // Если взяли предмет
	{
		RemoveItems(PChar, "DOjeronRing", 1);
		BackItemDescribe("DOjeronRing");
		items[FindItem("DOjeronRing")].Name = "itmname_DOjeronRing";
		DeleteAttribute(&items[FindItem("DOjeronRing")], "City");
	}
	else
	{
		string sCity = PChar.GenQuest.CitizenFindItem.City + + "_ExitTown";
		string sTemp = PChar.GenQuest.CitizenFindItem.ItemLocator;
		PChar.GenQuestRandItem.(sCity).(sTemp) = "potionrum";
		BackItemDescribe("DOjeronRing");
		items[FindItem("DOjeronRing")].Name = "itmname_DOjeronRing";
		DeleteAttribute(&items[FindItem("DOjeronRing")], "City");
	}
	DeleteAttribute(PChar, "GenQuest.CitizenFindItem");
}
//=====================================================================================================================================
// <-- Миниквест жителей. Поиск обручального кольца
//=====================================================================================================================================

//=====================================================================================================================================
// Warship. ЦЕРКОВНЫЙ ГЕНЕРАТОР КВЕСТ № 1 -->
//=====================================================================================================================================
// Gregg: церковные генераторы (Church-квесты) вынесено в отдельный файл (сплит god-файла GenQuests.c).
#include "scripts\GenQuests_Church.c"
//=====================================================================================================================================
// <-- Церковный квест №2 "Сбор пожертвований"
//=====================================================================================================================================

//=====================================================================================================================================
// Ugeen. ГЕНЕРАТОР "ПОВОД ДЛЯ СПЕШКИ "
//=====================================================================================================================================
// Gregg: квест «Повод поспешить» (ReasonToFast) вынесено в отдельный файл (сплит god-файла GenQuests.c).
#include "scripts\GenQuests_ReasonToFast.c"
//=====================================================================================================================================
// <-- ГЕНЕРАТОР "ПОВОД ДЛЯ СПЕШКИ "
//=====================================================================================================================================

//=====================================================================================================================================
// Warship. ГЕНЕРАТОР "СГОРЕВШЕЕ СУДНО" -->
//=====================================================================================================================================
void BurntShipQuest_TimeIsOver(String _quest)
{
	ref character = CharacterFromID(PChar.Quest.(_quest).PortmanId);

	SetRandomNameToCharacter(character);
	character.model = "usurer_" + (rand(7) + 1);

	character.Quest.BurntShip.TimeIsOver = true;
}
//=====================================================================================================================================
// <-- ГЕНЕРАТОР "СГОРЕВШЕЕ СУДНО"
//=====================================================================================================================================

//=====================================================================================================================================
// Ugeen. МИНИ-ГЕНЕРАТОРЫ  "ВЗРЫВ ИЛИ ЭПИДЕМИЯ НА ВРАЖЕСКОМ КОРАБЛЕ"
//=====================================================================================================================================
bool CheckShipSituation_GenQuest()
{
	if(CheckAttribute(pchar,"GenQuest.ShipSituation.Explosion") || CheckAttribute(pchar,"GenQuest.ShipSituation.Epidemy")) return true;
	if(CheckAttribute(pchar,"GenQuest.FortAgentBuyOut")) return true; // выкуп посыльного-агента в каюте: выход в море без формы захвата (корабль агента уходит свободным)
	else return false;
}

void RemoveCrewEpidemy_GenQuest()
{
	ref mainCh = GetMainCharacter();
	ref chref;
	int i, cn, crew, hcrew;
	int ShipType;

	Log_SetStringToLog(GetConvertStrWithReplace("Variable_GenQuests_19", "Logs.txt", "#space#", " "));
	for(i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(mainCh,i);
		if(cn >= 0)
		{
			chref = GetCharacter(cn);
			if (!GetRemovable(chref)) continue;
			ShipType = sti(chref.Ship.Type);
			crew = makeint(GetCrewQuantity(chref)/(20 - 10 * GetCrewQuantity(chref)/GetMaxCrewQuantity(chref) + GetCharacterSPECIAL(pchar,"Luck")));
			if(ShipType != sti(pchar.GenQuest.ShipSituation.Epidemy.ShipType) && !IsMainCharacter(chref))
			{
				crew = makeint(crew/2);
			}
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_GenQuests_20", "Logs.txt", "#space#", " ") + chref.Ship.Name + " " + crew + GetConvertStrWithReplace("Variable_GenQuests_21", "Logs.txt", "#space#", " "));
			Statistic_AddValue(mainCh, "Sailors_dead", crew);
			hcrew = GetCrewQuantity(chref);
			SetCrewQuantity(chref, hcrew - crew);
		}
	}
}

int CheckShipSituationDaily_GenQuest(ref refChar)
{
	int cn = 1;
	int ShipType = sti(refChar.Ship.Type);
	bool bOk;

	if(CheckAttribute(pchar,"GenQuest.ShipSituation.Epidemy"))
	{
		bOk = CheckAttribute(pchar,"GenQuest.ShipSituation.Epidemy.ShipType") && (ShipType == sti(pchar.GenQuest.ShipSituation.Epidemy.ShipType));
		if(bOk || IsMainCharacter(refChar))
		{
			switch(sti(pchar.GenQuest.ShipSituation.Epidemy))
			{
				case ShipSituation_0:
					cn = 1;
				break;
				case ShipSituation_1:
					cn = 2;
				break;
				case ShipSituation_2:
					cn = 2;
				break;
				case ShipSituation_3:
					cn = 3;
				break;
				case ShipSituation_4:
					cn = 1;
				break;
			}
		}
	}
	return cn;
}

// освобождаем квесты и устанавливаем таймер на повторную генерацию ситуаций
void SetFreeQuestAboardShipSituation(string qName)
{
	if(CheckShipSituation_GenQuest())
	{
		DeleteAttribute(pchar,"GenQuest.ShipSituation");
	}
	bSeaCanGenerateShipSituation = false;
	pchar.CanGenerateShipSituation = false;
	SetFunctionTimerCondition("CanGenerateQuestAboardShipSituation", 0, 1, rand(15) + 15, false);
}

// можем генерировать ситуацию ещё раз до упора, пока на нее не напорется геймер
void CanGenerateQuestAboardShipSituation(string qName)
{
	bSeaCanGenerateShipSituation = true;
	pchar.CanGenerateShipSituation = true;
}

// =====================================================================================
// LEO: бездиалоговые ЛОВУШКИ на ДРЕЙФУЮЩИХ кораблях, брошенных САМИМ ИИ после его абордажа.
//   ИИ топит / присваивает / бросает приз в дрейф (companions.c). На брошенный в дрейф корпус
//   вешаем ловушку - игрок не знает, что за "халявный" приз ему достанется.
//   5% на каждый такой корабль, тип 50/50 взрыв|эпидемия, одновременно одно событие для ГГ.
//   Метка живёт на самом дрейф-корабле (driftChr.AITrap), срабатывает когда игрок его обчистит.
//   Взрыв: детонация через ~1 сек после выхода из интерфейса (+ Log_Info), задевает корабль игрока рядом.
//   Эпидемия: если забрал себе - не продать на верфи (отказ + штатный мор, минуя кулдаун 15-30 дн).
//   Рандом - дневной dRand с декорреляцией по индексу корабля (не сейв-скамится, per-ship).
// =====================================================================================
#define AITRAP_TEST_MODE  0   // LEO: ТЕСТ дрейф-ловушек: 0=боевой (шанс по классу, тип 50/50), 1=всегда ВЗРЫВ, 2=всегда ЭПИДЕМИЯ

// Арм при создании дрейф-приза, брошенного ИИ (companions.c, AILeaveCapturedShipDrift). Метим сам корпус.
void AICapturedTrap_ArmDrift(ref driftChr)
{
	if (!CheckAttribute(driftChr, "Ship.Type")) return;
	if (sti(driftChr.Ship.Type) == SHIP_NOTUSED) return;

	if (AITRAP_TEST_MODE == 1) { driftChr.AITrap = "Explosion"; return; }   // ТЕСТ: гарантированный взрыв
	if (AITRAP_TEST_MODE == 2) { driftChr.AITrap = "Epidemy";   return; }   // ТЕСТ: гарантированная эпидемия

	int idx = sti(driftChr.index);
	// шанс ловушки растёт с классом корабля: 6кл=10%, 5=15%, 4=20%, 3=25%, 2=30%, 1=35% (оба исхода вместе)
	int shipClass = GetCharacterShipClass(driftChr);
	if (shipClass < 1) shipClass = 1;
	if (shipClass > 6) shipClass = 6;
	int chance = 10 + (6 - shipClass) * 5;                        // 10..35
	chance = chance - makeint(GetCharacterSPECIAL(pchar, "LUCK")); // удача ГГ (1-10) снижает шанс на минус столько же %
	if (chance < 2) chance = 2;                                   // но хвостик риска всегда остаётся
	if (((dRand2(9999) + idx * 37) % 100) >= chance) return;     // не заминирован (per-ship, не сейв-скамится)
	if (((dRand3(9999) + idx * 53) % 2) == 0) driftChr.AITrap = "Explosion"; // тип 50/50
	else driftChr.AITrap = "Epidemy";
}

// Вызывается на выходе из интерфейса грабежа/обмена (transfer_main.c, IDoExit). shipChr = ТЕКУЩИЙ корабль
// (xi_refCharacter): при захвате - офицер-назначенец, при отпуске - беглец, при ре-дрейфе - дрейф-кэп.
// Ловушка лежит в pending на ГГ (перенесена в InitInterface_RRS при ОТКРЫТИИ - пути выхода подменяют xi на временный персонаж).
void AICapturedTrap_OnRansackExit(ref shipChr)
{
	if (!CheckAttribute(pchar, "AITrap.Active")) return;
	if (!CheckAttribute(pchar, "AITrap.Type")) return;
	if (CheckAttribute(pchar, "AITrap.Armed")) return;          // эпидемия уже взведена (ждёт верфь) - повторно не реагируем

	if (pchar.AITrap.Type == "Explosion")
	{
		if (CheckAttribute(shipChr, "Ship.Type"))
		{
			if (sti(shipChr.Ship.Type) != SHIP_NOTUSED)
			{
				if (!LAi_IsDead(shipChr))   // если корабль потоплен сам - детонировать нечего
				{
					shipChr.situation = "Explosion";  // усиление урона в Ship_BranderDetonate (AIShip.c)
					PostEvent(SHIP_BRANDER_DETONATE, 3000+rand(7000), "l", sti(shipChr.index)); // рванёт через ~3-10 сек после выхода
					Log_InfoColored("red", XI_ConvertString("ShipTrapExplosionLog")); // LEO: "корабль заминирован!" - красным, без анимации
				}
			}
		}
		DeleteAttribute(pchar, "AITrap");          // ловушка отработала
		return;
	}
	// Эпидемия заражает ПЕРСОНАЛЬНО этот корабль - только если он ЗАБРАН себе (в эскадру / пересел). Иначе впустую.
	bool kept = false;
	if (IsCompanion(shipChr))     kept = true;
	if (IsMainCharacter(shipChr)) kept = true;
	if (kept)
	{
		pchar.AITrap.CapIdx   = sti(shipChr.index);
		pchar.AITrap.ShipType = sti(shipChr.Ship.Type);
		if (CheckAttribute(shipChr, "Ship.Name")) pchar.AITrap.ShipName = shipChr.Ship.Name;
		else pchar.AITrap.ShipName = "";
		pchar.AITrap.Armed    = true;
		AICapturedTrap_StartEpidemy(); // заражаем именно этот корабль (раскрытие само собой, см. ниже)
	}
	else
	{
		DeleteAttribute(pchar, "AITrap"); // бросил/потопил/отпустил - ловушка впустую, снимаем блок
	}
}

// =====================================================================================
// LEO: МУЛЬТИ-эпидемия. Штатная GenQuest.ShipSituation.Epidemy держит ОДНУ эпидемию за раз -
// второй заражённый корабль её перетирал (в СЖ/на верфи виден только последний). Свой список:
// pchar.AIEpidemy.c<индекс кэпа> { days, idx, shiptype, shipname }. КАЖДЫЙ заражённый корабль
// независим: свой отсчёт (5-10 дн), своя гибель команды, свой отказ на верфи, свои строки лога.
// =====================================================================================

// гибель команды на ОДНОМ заражённом корабле (раз в сутки); если это флагман - болеет и сам ГГ
void AIEpidemy_ShipHit(ref c, bool bFirst)
{
	if (!CheckAttribute(c, "Ship.Type")) return;
	if (sti(c.Ship.Type) == SHIP_NOTUSED) return;
	int crew = GetCrewQuantity(c);
	if (crew > 0)
	{
		int dead = crew / 8;          // ~12% команды в сутки
		if (bFirst) dead = crew / 4;  // первый день - четверть
		if (dead < 1) dead = 1;
		if (dead > crew) dead = crew;
		SetCrewQuantity(c, crew - dead);
		Statistic_AddValue(GetMainCharacter(), "Sailors_dead", dead);
	}
	if (IsMainCharacter(c)) pchar.Health.Damg = stf(pchar.chr_ai.hp_max) * 3; // ГГ на заражённом корабле сам слёг
}

// добавить присвоенный корабль в список заражённых (зовётся из OnRansackExit при kept)
void AICapturedTrap_StartEpidemy()
{
	if (!CheckAttribute(pchar, "AITrap.Armed")) return;
	int idx = sti(pchar.AITrap.CapIdx);
	// LUCK уже отыграла на стадии арма (AICapturedTrap_ArmDrift) - тут НЕ дублируем: раз заминирован эпидемией и забран, заражаем
	string sKey = "c" + idx;
	if (!CheckAttribute(pchar, "AIEpidemy." + sKey)) // ещё не заражён именно этот корабль
	{
		pchar.AIEpidemy.(sKey).days     = 5 + rand(5); // 5-10 дней
		pchar.AIEpidemy.(sKey).idx      = idx;
		pchar.AIEpidemy.(sKey).shiptype = sti(pchar.AITrap.ShipType);
		pchar.AIEpidemy.(sKey).shipname = pchar.AITrap.ShipName;
		// ТИХО: ни лога, ни записи, ни урона при присвоении - вскроется записью в СЖ на СЛЕД. сутки (сюрприз). См. AIEpidemy_Daily.
	}
	DeleteAttribute(pchar, "AITrap"); // ловушка отработала
}

// LEO: найти заражённый корабль в ЭСКАДРЕ игрока по ТИПУ (флагман + компаньоны). Возвращает индекс персонажа или -1.
// Ищем по флоту, а НЕ по сохранённому индексу кэпа (он волатилен между море<->карта).
int AIEpidemy_FindFleetShipByType(int shipType)
{
	ref mc = GetMainCharacter();
	if (CheckAttribute(mc, "Ship.Type")) { if (sti(mc.Ship.Type) == shipType) return GetMainCharacterIndex(); }
	int i, cn;
	ref c;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if (cn <= 0) continue;
		c = GetCharacter(cn);
		if (!CheckAttribute(c, "Ship.Type")) continue;
		if (sti(c.Ship.Type) == shipType) return cn;
	}
	return -1;
}

// суточная обработка ВСЕХ заражённых кораблей (зовётся из QuestActions, LSC_Q2Utilite.c).
// Раскрытие: на ПЕРВЫЙ суточный тик каждого заражённого корабля - запись в СЖ (тема "AICapturedEpidemy") + первый удар.
void AIEpidemy_Daily()
{
	if (!CheckAttribute(pchar, "AIEpidemy")) return;
	aref arEp; makearef(arEp, pchar.AIEpidemy);
	int i;
	int n = GetAttributesNum(arEp);
	// проход 1: суточный тик на каждый заражённый корабль.
	// LEO: ЖИЗНЬ записи = ТОЛЬКО счётчик days (независимо от судьбы корабля - юзер так и просил). БЕЗ day-guard:
	// тик = один вызов QuestActions (как у штатного мора рядом). При пропуске времени движок гоняет догоняющий
	// цикл QuestActions с УЖЕ переведённой датой - day-guard схлопывал все итерации в один тик (за месяц не отбаливало).
	// От лазеек (сбежать, сплавив корабль) защищают БЛОКИ продажи/стоянки/вольного плавания, а не привязка к флоту.
	for (i = 0; i < n; i++)
	{
		aref e = GetAttributeN(arEp, i);

		// раскрытие - один раз, на первый тик: запись в СЖ появляется в любом случае (был факт заражения)
		bool bJustRevealed = false;
		if (!CheckAttribute(e, "revealed"))
		{
			e.revealed = 1;
			bJustRevealed = true;
			AddQuestRecord("AICapturedEpidemy", "1");
			AddQuestUserData("AICapturedEpidemy", "sShipName", e.shipname);
			pchar.QuestInfo.AICapturedEpidemy.Complete = false; // на случай рецидива после закрытия темы - вернуть в активные
			// иконка темы ставится авто-циклом quests.c по LogName из секции [QUEST_AICapturedEpidemy] в pictures.ini (юзер прописал)
		}

		// удар по команде - best-effort: если корабль этого типа найден в эскадре (НЕ влияет на жизнь записи)
		int cn = AIEpidemy_FindFleetShipByType(sti(e.shiptype));
		if (cn >= 0) AIEpidemy_ShipHit(GetCharacter(cn), bJustRevealed); // первый удар = в день раскрытия

		// завершение - только по таймеру; на выздоровлении ИМЕННО этого корабля - своя запись в СЖ
		int d = sti(e.days) - 1;
		if (d <= 0)
		{
			AddQuestRecord("AICapturedEpidemy", "2"); // "мор на корабле X отступил"
			AddQuestUserData("AICapturedEpidemy", "sShipName", e.shipname);
			e.expired = 1;
		}
		else e.days = d;
	}
	// проход 2: убираем отболевшие записи ТРЕКЕРА (записи в СЖ при этом ОСТАЮТСЯ как история)
	for (i = GetAttributesNum(arEp) - 1; i >= 0; i--)
	{
		aref e2 = GetAttributeN(arEp, i);
		if (CheckAttribute(e2, "expired")) DeleteAttribute(pchar, "AIEpidemy." + GetAttributeName(e2));
	}
	// все отболели - тему СЖ НЕ удаляем (записи = история, как у штатных квестов), а ЗАКРЫВАЕМ (уходит в архив); чистим только трекер
	if (GetAttributesNum(arEp) <= 0)
	{
		if (CheckAttribute(pchar, "QuestInfo.AICapturedEpidemy")) CloseQuestHeader("AICapturedEpidemy");
		DeleteAttribute(pchar, "AIEpidemy");
	}
}

// LEO: заражён ли корабль данного ТИПА (есть в трекере AIEpidemy). Общая проверка лазеек: верфь/стоянка/вольное плавание.
bool AICapturedTrap_IsShipTypeInfected(int shipType)
{
	if (!CheckAttribute(pchar, "AIEpidemy")) return false;
	aref arEp; makearef(arEp, pchar.AIEpidemy);
	int n = GetAttributesNum(arEp);
	int i;
	for (i = 0; i < n; i++)
	{
		aref e = GetAttributeN(arEp, i);
		if (sti(e.shiptype) == shipType) return true;
	}
	return false;
}

// LEO: заражён ли корабль данного персонажа (по типу). Удобная обёртка.
bool AICapturedTrap_IsShipInfected(ref shipChr)
{
	if (!CheckAttribute(shipChr, "Ship.Type")) return false;
	return AICapturedTrap_IsShipTypeInfected(sti(shipChr.Ship.Type));
}

// LEO: заражён ли корабль компаньона по ОФИЦЕРУ в диалоге (офицер и судовой персонаж могут различаться - матчим по имени).
bool AICapturedTrap_IsCompanionShipInfected(ref officerChr)
{
	if (AICapturedTrap_IsShipInfected(officerChr)) return true; // прямой корабль офицера
	int i, cn;
	ref c;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if (cn <= 0) continue;
		c = GetCharacter(cn);
		if (c.name == officerChr.name)
		{
			if (AICapturedTrap_IsShipInfected(c)) return true;
		}
	}
	return false;
}

// Проверка при попытке продать корабль на верфи (shipyard.c). true = отказать (ИМЕННО этот корабль заражён).
bool AICapturedTrap_ShipyardEpidemy(ref shipChr)
{
	return AICapturedTrap_IsShipInfected(shipChr);
}

// проверка квестовой ситуации перед выходом из каюты поверженного кэпа в режим "море"
void CheckCaptainFreeQuestAboardSituation()
{
	ref sld;
	if(CheckShipSituation_GenQuest())
	{
		if(CheckAttribute(pchar,"GenQuest.ShipSituation.Explosion"))
		{
			sld = CharacterFromID(pchar.GenQuest.ShipSituation.Explosion.CapId);
			SetCaptainFreeQuestAboardSituation(sld);
		}
		if(CheckAttribute(pchar,"GenQuest.ShipSituation.Epidemy"))
		{
			sld = CharacterFromID(pchar.GenQuest.ShipSituation.Epidemy.CapId);
			pchar.GenQuest.ShipSituation.Epidemy.ShipName = sld.Ship.Name;
			pchar.GenQuest.ShipSituation.Epidemy.ShipType = sti(sld.Ship.Type);
			if(pchar.GenQuest.ShipSituation.Epidemy.CapState == "Died") // кэп помер - вот бедолага
			{
				SetCaptainDiedQuestAboardSituation(sld);
				ShipSituation_SetConsequence(sti(pchar.GenQuest.ShipSituation.Epidemy));
			}
			if(pchar.GenQuest.ShipSituation.Epidemy.CapState == "Live") // кэп остался жить - недолго ему осталось
			{
				SetCaptainFreeQuestAboardSituation(sld);
				ShipSituation_SetConsequence(sti(pchar.GenQuest.ShipSituation.Epidemy));
			}
		}
	}
}

// установка последствий или их отсутствий
void ShipSituation_SetConsequence(int iSituation)
{
	if(GetCharacterSPECIAL(pchar,"LUCK") * 10 > rand(120)) // шутник блин
	{
		DoQuestFunctionDelay("SetFreeQuestAboardShipSituation", 1.0);
	}
	else // кэп не шутил
	{
		switch (iSituation)
		{
			case ShipSituation_0: // обшарили каюту или труп
				pchar.GenQuest.ShipSituation.Epidemy.Days = 3;
			break;
			case ShipSituation_1: // что-то делали в интерфейсе грабежа
				pchar.GenQuest.ShipSituation.Epidemy.Days = 3 + rand(2);  // LEO: 3-5 дней
			break;
			case ShipSituation_2: // сажаем компаньона с командой на инфицированный корабль
				pchar.GenQuest.ShipSituation.Epidemy.Days = 5 + rand(5);  // LEO: 5-10 дней
			break;
			case ShipSituation_3: // ГГ пересаживается на инфицированный корабль
				pchar.GenQuest.ShipSituation.Epidemy.Days = 5 + rand(5);  // LEO: 5-10 дней
			break;
			case ShipSituation_4: // ничего не обшаривали
				pchar.GenQuest.ShipSituation.Epidemy.Days = 1;
			break;
		}
		pchar.GenQuest.ShipSituation.Epidemy.DaysQty = sti(pchar.GenQuest.ShipSituation.Epidemy.Days);
	}
}

void ShipSituation_CheckConsequence(int iSituation)
{
	int DayQty;
	if(sti(pchar.GenQuest.ShipSituation.Epidemy.Days) > 0)
	{
		DayQty = sti(pchar.GenQuest.ShipSituation.Epidemy.DaysQty) - sti(pchar.GenQuest.ShipSituation.Epidemy.Days);
		if(DayQty == 0)
		{
			ReOpenQuestHeader("ShipEpidemy");
		}
		switch(iSituation)
		{
			case ShipSituation_0:
				if(DayQty == 0)
				{
					pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*10;
					AddQuestRecord("ShipEpidemy","1");
					AddQuestUserData("ShipEpidemy", "sSex", GetSexPhrase("","а"));
					AddQuestUserData("ShipEpidemy", "sShipName", pchar.GenQuest.ShipSituation.Epidemy.ShipName);
				}
				else
				{
					if(DayQty < 3) pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*5;
				}
			break;
			case ShipSituation_1:
				if(DayQty == 0)
				{
					pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*10;
					ChangeOfficersLoyality("bad_all", 1);
					AddQuestRecord("ShipEpidemy","3");
					AddQuestUserData("ShipEpidemy", "sShipName", pchar.GenQuest.ShipSituation.Epidemy.ShipName);
					RemoveCrewEpidemy_GenQuest();
				}
				else
				{
					if(DayQty < 3) pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*5;
				}
			break;
			case ShipSituation_2:
				if(DayQty == 0)
				{
					pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*10;
					ChangeOfficersLoyality("bad_all", 1);
					AddQuestRecord("ShipEpidemy","3");
					AddQuestUserData("ShipEpidemy", "sShipName", pchar.GenQuest.ShipSituation.Epidemy.ShipName);
					RemoveCrewEpidemy_GenQuest();
				}
				else
				{
					if(DayQty < 3) pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*5;
				}
			break;
			case ShipSituation_3:
				if(DayQty == 0)
				{
					pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*10;
					ChangeOfficersLoyality("bad_all", 2);
					AddQuestRecord("ShipEpidemy","3");
					AddQuestUserData("ShipEpidemy", "sShipName", pchar.GenQuest.ShipSituation.Epidemy.ShipName);
					RemoveCrewEpidemy_GenQuest();
				}
				else
				{
					if(DayQty < 3) pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*5;
				}
			break;
			case ShipSituation_4:
				pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*10;
				AddQuestRecord("ShipEpidemy","1");
				AddQuestUserData("ShipEpidemy", "sShipName", pchar.GenQuest.ShipSituation.Epidemy.ShipName);
			break;
		}
		pchar.GenQuest.ShipSituation.Epidemy.Days = sti(pchar.GenQuest.ShipSituation.Epidemy.Days) - 1;
	}
	else
	{
		if(iSituation < 4)
		{
			switch(iSituation)
			{
				case ShipSituation_0:
					AddQuestRecord("ShipEpidemy","2");
				break;
				case ShipSituation_1:
					AddQuestRecord("ShipEpidemy","4");
				break;
				case ShipSituation_2:
					AddQuestRecord("ShipEpidemy","4");
				break;
				case ShipSituation_3:
					AddQuestRecord("ShipEpidemy","4");
				break;
			}
		}
		CloseQuestHeader("ShipEpidemy");
		DoQuestFunctionDelay("SetFreeQuestAboardShipSituation", 5.0);
	}
}

// вражеский кэп помер от стали и свинца - выходим сразу в режим "море"
void SetCaptainDiedQuestAboardSituation(ref chr)
{
	ShipDead(sti(chr.index), KILL_BY_ABORDAGE, sti(pchar.index));
}

// отпустили кэпа при переходе из вражеской каюты в режим "море"
void SetCaptainFreeQuestAboardSituation(ref chr)
{
	aref    arTo, arFrom;
	ref sld = GetCharacter(NPC_GenerateCharacter(chr.id + "_free", "off_hol_2", "man", "man", 60, sti(chr.nation), 0, true)); // фантом, на время
	ChangeAttributesFromCharacter(sld, chr, false);
	DeleteAttribute(sld, "ship");
	sld.ship = "";

	makearef(arTo, sld.ship);
	makearef(arFrom, chr.Ship);
	CopyAttributes(arTo, arFrom);
	sld.AlwaysFriend = true;
	sld.Abordage.Enable = false; // запрет абордажа

	SeaAI_SetCaptainFree(sld, chr);
	chr.location = "none";

	if (bSeaActive)
	{
		PostEvent("evntQuestsCheck", 400);
		ShipTakenFree(sti(chr.index), KILL_BY_ABORDAGE, sti(pchar.index)); // тут умер реальный кэп, апдайтим ещё
		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_FRIEND);
		UpdateRelations();
		RefreshBattleInterface();
		CheckQuestAboardCabinSituation(sld);
		DoQuestCheckDelay("NationUpdate", 1.5);
	}
}

void ShipSituation_SetQuestSituation(int iSituation)
{
	if(CheckAttribute(pchar,"GenQuest.ShipSituation.Explosion"))
	{
		pchar.GenQuest.ShipSituation.Explosion = iSituation;
	}
	if(CheckAttribute(pchar,"GenQuest.ShipSituation.Epidemy"))
	{
		pchar.GenQuest.ShipSituation.Epidemy = iSituation;
	}
}

// делаем большой "бум" на вражеском корабле --> превращаем его в брандер
void ShipSituation_MakeDetonate(string qName)
{
	Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_22", "Logs.txt", "#space#", " "));
	ref sld = CharacterFromID(pchar.GenQuest.ShipSituation.Explosion.CapId + "_free");
	ref rBaseShip = GetRealShip(sti(sld.Ship.Type));
	Event(SHIP_BRANDER_DETONATE,"l", sti(sld.index));
    PlaySound("Sea Battles\Vzriv_fort_001.wav");
	Ship_SetTaskNone(SECONDARY_TASK, sti(sld.index));
	Log_Info("" + XI_ConvertString(rBaseShip.BaseName) + " '" + sld.Ship.Name + "' ŝ"); // LEO: иконка Брандер заменяет "взорвал/взорвала крюйт камеру"
	bQuestDisableMapEnter = false;
	RefreshBattleInterface();
	DoQuestFunctionDelay("SetFreeQuestAboardShipSituation", 5.0);
}

void CheckQuestAboardCabinSituation(ref echar)
{
	if(CheckAttribute(pchar,"GenQuest.ShipSituation.Explosion"))
	{
		echar.FaceId = 333;
		echar.name = GetConvertStrWithReplace("Variable_GenQuests_6", "Names.txt", "#space#", " ");
		echar.lastname = "";
		bQuestDisableMapEnter = true;

		if(sti(pchar.GenQuest.ShipSituation.Explosion) == ShipSituation_1)
		{
			DoQuestFunctionDelay("ShipSituation_MakeDetonate", 0.5);
		}
		else
		{
			if(sti(pchar.GenQuest.ShipSituation.Explosion) == ShipSituation_0)
			{
				if(iArcadeSails == 1)
				{
					DoQuestFunctionDelay("ShipSituation_MakeDetonate", 15.0);
				}
				else
				{
					DoQuestFunctionDelay("ShipSituation_MakeDetonate", 40.0);
				}
			}
			else
			{
				if(iArcadeSails == 1)
				{
					DoQuestFunctionDelay("ShipSituation_MakeDetonate", 30.0);
				}
				else
				{
					DoQuestFunctionDelay("ShipSituation_MakeDetonate", 75.0);
				}
			}
		}
	}
	if(CheckAttribute(pchar,"GenQuest.ShipSituation.Epidemy"))
	{
		pchar.GenQuest.ShipSituation.Epidemy.ShipName = echar.Ship.Name;
		pchar.GenQuest.ShipSituation.Epidemy.ShipType = sti(echar.Ship.Type);
		ShipSituation_SetConsequence(sti(pchar.GenQuest.ShipSituation.Epidemy));
	}
}
//=====================================================================================================================================
// <-- МИНИ-ГЕНЕРАТОРЫ "ВЗРЫВ ИЛИ ЭПИДЕМИЯ  НА ВРАЖЕСКОМ КОРАБЛЕ"
//=====================================================================================================================================

//=====================================================================================================================================
// Ugeen. ГЕНЕРАТОР  "ДЕВИЦА В ДЖУНГЛЯХ"
//=====================================================================================================================================
// Gregg: энкаунтер «Девушка» (EncGirl) вынесено в отдельный файл (сплит god-файла GenQuests.c).
#include "scripts\GenQuests_EncGirl.c"
//=====================================================================================================================================
// <-- ГЕНЕРАТОР  "ДЕВИЦА В ДЖУНГЛЯХ"
//=====================================================================================================================================

//=====================================================================================================================================
// Ugeen. ГЕНЕРАТОР  "Специальный энкаунтер - бочка"
//=====================================================================================================================================
void Set_TreasureBarrel()
{
	ref			nulChr;
	aref     	trBarrel;

	nulChr = &NullCharacter;
	if(CheckAttribute(nulChr,"GenQuest.Barrel")) DeleteAttribute(nulChr,"GenQuest.Barrel");

	makearef(trBarrel, nulChr.GenQuest.Barrel);
	int irand;

	if(GetSummonSkillFromName(pchar, "Fortune") > (drand(200)-(GetCharacterSPECIALSimple(pchar, SPECIAL_L)*5)))
	{
		irand = drand(20);
		if(irand == 1) trBarrel.items.jewelry1 = rand(18) + 27;
		irand = drand(20);
		if(irand == 2) trBarrel.items.jewelry2 = rand(22) + 22;
		irand = drand(20);
		if(irand == 3) trBarrel.items.jewelry3 = rand(15) + 32;
		irand = drand(20);
		if(irand == 4) trBarrel.items.jewelry4 = rand(22) + 15;
		irand = drand(20);
		if(irand == 5) trBarrel.items.jewelry5 = rand(19) + 22;
		irand = drand(20);
		if(irand == 6) trBarrel.items.jewelry6 = rand(22) + 16;
		irand = drand(20);
		if(irand == 7) trBarrel.items.jewelry7 = rand(17) + 32;
		irand = drand(20);
		if(irand == 8) trBarrel.items.jewelry8 = rand(5) + 18;
		irand = drand(20);
		if(irand == 9) trBarrel.items.jewelry9 = rand(8) + 12;
		irand = drand(20);
		if(irand == 10) trBarrel.items.jewelry10 = rand(35) + 12;
		irand = drand(20);
		if(irand == 11) trBarrel.items.jewelry11 = rand(13) + 27;
		irand = drand(20);
		if(irand == 12) trBarrel.items.jewelry12 = rand(16) + 18);
		irand = drand(20);
		if(irand == 13) trBarrel.items.jewelry13 = rand(30) + 15);
		irand = drand(20);
		if(irand == 14) trBarrel.items.jewelry14 = rand(30) + 15);
		irand = drand(20);
		if(irand == 15) trBarrel.items.jewelry16 = rand(35) + 14);
		irand = drand(20);
		if(irand == 16) trBarrel.items.jewelry17 = rand(24) + 18);
		irand = drand(20);
		if(irand == 17) trBarrel.items.jewelry18 = rand(17) + 13);
		irand = drand(20);
		if(irand == 18) trBarrel.items.chest = rand(2) + 1;
		irand = drand(20);
		if(irand == 19) trBarrel.items.coins = rand(10) + 7;

		irand = drand(25);
		if(irand == 6) trBarrel.items.indian20 = 1;
		irand = drand(25);
		if(irand == 9) trBarrel.items.indian21 = 1;
		irand = drand(25);
		if(irand == 12) trBarrel.items.indian11 = 1;
		irand = drand(25);
		if(irand == 15) 	trBarrel.items.indian12 = 1;
		irand = drand(25);
		if(irand == 18) 	trBarrel.items.indian6 = 1;
		irand = drand(25);
		if(irand == 21) trBarrel.items.incas_collection = 1;

		irand = drand(35);
		if(irand == 20) trBarrel.items.sculMa2 = 1;
		irand = drand(35);
		if(irand == 28) trBarrel.items.sculMa3 = 1;

		irand = drand(25);
		if(irand == 24 && GetCharacterItem(pchar, "map_full") == 0)
		{
			if (GetCharacterItem(pchar, "map_part1") == 0) trBarrel.items.map_part1 = 1;
			else
			{
				if (GetCharacterItem(pchar, "map_part2") == 0) trBarrel.items.map_part2 = 1;
			}
		}
	}
	else
	{
		if(rand(1) == 0) trBarrel.items.mineral2 = rand(7) + 7;
		if(rand(1) == 0) trBarrel.items.mineral3 = rand(15) + 5;
		if(rand(1) == 0) trBarrel.items.mineral4 = rand(10) + 2;
		if(rand(1) == 0) trBarrel.items.mineral5 = rand(8) + 4;
		if(rand(1) == 0) trBarrel.items.mineral6 = rand(14) + 7;
		if(rand(1) == 0) trBarrel.items.mineral7 = rand(12) + 8;
		if(rand(1) == 0) trBarrel.items.mineral8 = rand(12) + 6;
		if(rand(1) == 0) trBarrel.items.mineral9 = rand(8) + 2;
		if(rand(1) == 0) trBarrel.items.mineral10 = rand(16) + 6;
	}
	trBarrel.items.gold = drand(5000) + 5000;
	LaunchItemsBarrel(&trBarrel);
	Barrel_SetRatsStowaway();
}

// LEO: КРЫСЫ-БЕЗБИЛЕТНИКИ. Вместе с добром из выловленной бочки на борт перебирается крысиный выводок:
//   через 5-10 дней он единоразово выжирает часть провианта. Срабатывает не на каждой бочке (шанс режется
//   удачей), и пока прошлый выводок не отъелся, новый не заводится - иначе таймеры копились бы пачками.
//   Дата ставится на pchar: суточный тик сам проверит срок (Barrel_RatsCheck в WorldSituationsUpdate).
void Barrel_SetRatsStowaway()
{
	if (CheckAttribute(pchar, "Barrel.Rats.Date.control_year")) return; // выводок уже на борту
	// LEO: «Крысиный бог» (indian11) - от него крысы бегут, как от огня, и на борт они не перебираются.
	//   Проверка по наличию в инвентаре, как в DailyRatsEatGoodsUpdate (food.c) - надевать оберег не требуется.
	if (GetCharacterItem(pchar, "indian11") > 0) return;

	int iChance = BARREL_RATS_CHANCE - GetCharacterSPECIAL(pchar, "Luck");
	if (iChance < 1) iChance = 1; // везунчику всё равно оставляем шанс попасться
	if (BARREL_RATS_TEST_MODE == 1) iChance = 100;
	if (drand(99) >= iChance) return;

	SaveCurrentQuestDateParam("Barrel.Rats.Date");
	pchar.Barrel.Rats.Days = BARREL_RATS_MIN_DAYS + drand(BARREL_RATS_MAX_DAYS - BARREL_RATS_MIN_DAYS);
	if (BARREL_RATS_TEST_MODE == 1) pchar.Barrel.Rats.Days = 1;
}

// Суточная проверка: срок вышел - крысы разом сжирают часть провианта и уходят (одноразово).
// Жрут ТОЛЬКО в трюме ГГ (бочку выловил он), на эскадру не расползаются - отсюда RemoveCharacterGoodsSelf.
void Barrel_RatsCheck()
{
	if (!CheckAttribute(pchar, "Barrel.Rats.Date.control_year")) return;
	if (GetQuestPastDayParam("Barrel.Rats.Date") < sti(pchar.Barrel.Rats.Days)) return;
	// LEO: оберег мог появиться уже ПОСЛЕ бочки - тогда выводок просто не кормится, пока он в трюме.
	//   Метку намеренно не снимаем: расстанется ГГ с оберегом - крысы своё возьмут.
	if (GetCharacterItem(pchar, "indian11") > 0) return;

	int iFood = GetCargoGoods(pchar, GOOD_FOOD);
	DeleteAttribute(pchar, "Barrel.Rats");   // выводок отработал - метку снимаем в любом случае
	if (iFood < 1) return;                   // жрать нечего, крысы ушли ни с чем

	int iEaten = makeint(iFood * (BARREL_RATS_EAT_MIN_PCT + drand(BARREL_RATS_EAT_MAX_PCT - BARREL_RATS_EAT_MIN_PCT)) / 100);
	if (iEaten < 1) iEaten = 1;
	if (iEaten > iFood) iEaten = iFood;
	RemoveCharacterGoodsSelf(pchar, GOOD_FOOD, iEaten);

	Log_InfoColored("red", GetConvertStrWithReplace("Variable_GenQuests_Rats", "Logs.txt", "#space#", " ") + iEaten + GetConvertStrWithReplace("Variable_GenQuests_Rats2", "Logs.txt", "#space#", " "));
}
//=====================================================================================================================================
// Ugeen. ГЕНЕРАТОР  "Специальный энкаунтер - бочка"
//=====================================================================================================================================

// Gregg: секция CaptainComission («Поручение капитана: выкуп») вынесена в отдельный файл (сплит god-файла GenQuests.c).
#include "scripts\GenQuests_CaptainComission.c"
//=====================================================================================================================================
// Ugeen. ГЕНЕРАТОР  "КАТОРЖАНЕ"
//=====================================================================================================================================
// Gregg: квест «Каторжник» (Convict) вынесено в отдельный файл (сплит god-файла GenQuests.c).
#include "scripts\GenQuests_Convict.c"
//=====================================================================================================================================
// <--. ГЕНЕРАТОР  "КАТОРЖАНЕ"
//=====================================================================================================================================

//=====================================================================================================================================
// Ugeen. ГЕНЕРАТОР "СИТУАЦИИ В ТРЮМЕ" -->
//=====================================================================================================================================
// Gregg: квест трюма (Hold_GenQuest) вынесено в отдельный файл (сплит god-файла GenQuests.c).
#include "scripts\GenQuests_Hold.c"
//=====================================================================================================================================
// <-- ГЕНЕРАТОР "СИТУАЦИИ В ТРЮМЕ"
//=====================================================================================================================================

//=====================================================================================================================================
// Ugeen. разрешаем генерацию монстров в пещерах для главного персонажа -->
//=====================================================================================================================================
void GenQuest_EnableMonstersGen(string qName)
{
	if(CheckAttribute(pchar, "GenQuest.monstersTimer"))
	{
		DeleteAttribute(pchar, "GenQuest.monstersTimer");
	}
}

bool GenQuest_CheckMonstersGen()
{
	if(CheckAttribute(pchar, "GenQuest.monstersTimer")) return false;
	else return true;
}
//=====================================================================================================================================
// <-- разрешаем генерацию монстров в пещерах для главного персонажа
//=====================================================================================================================================
//Gregg T102 Терминатор разблок двери
void T102_DoorUnlock(string qName)
{
	chrDisableReloadToLocation = false;
}

