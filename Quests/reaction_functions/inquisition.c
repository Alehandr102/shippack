//==> Gregg: "Помощь святой инквизиции" (перенос из К:ПС, HelpSacredInquisition)
// Оригинал: Damned Destiny, PROGRAM\lugger\Quests.c. Имена функций сохранены один в один,
// чтобы можно было сверяться с источником.
//
// Отличия от оригинала:
//  - дом Натана Рича заменён на комнату таверны Бас-Тера (BasTer_tavern_upstairs):
//    новая локация не нужна, и не приходится трогать дверь reload.l26 в BasTer_town;
//  - хелперы SetQuestLocatorFunction / SetQuestNPCDeadFunction / DisableLandEncounters
//    в CSP отсутствуют - написаны ниже локально;
//  - в финале, как и в оригинале, стартует "Легенда о призрачном свете" (moonstone.c).
//
// Тексты, видимые игроку, идут через iTxt(): файл общий для обеих локалей, русские
// литералы отсюда текут в английскую игру (AGENTS.md р.2).

// Gregg: локаторное прерывание. В CSP готового хелпера нет, узел пишем руками -
// образец Program\Addons\Arena.c:215.
void HSI_SetLocatorFunction(string sQuest, string sFunction, string sLocation, string sGroup, string sLocator)
{
	DeleteAttribute(pchar, "quest." + sQuest); // Gregg: стереть протухшее over от прошлого прохождения
	pchar.quest.(sQuest).win_condition.l1 = "locator";
	pchar.quest.(sQuest).win_condition.l1.location = sLocation;
	pchar.quest.(sQuest).win_condition.l1.locator_group = sGroup;
	// Gregg: поле называется locator, НЕ locator_name - с неверным именем прерывание
	// просто никогда не совпадает, и молча (образец: Program\Addons\Arena.c:215)
	pchar.quest.(sQuest).win_condition.l1.locator = sLocator;
	pchar.quest.(sQuest).function = sFunction;
}

// Gregg: прерывание "перебиты все с таким префиксом id". Тоже своё: в CSP есть только
// одиночный NPC_Death, а тут ждём пятерых стражников либо двух охранников Натана.
void HSI_SetGroupDeadFunction(string sQuest, string sFunction, string sPrefix, int iQty)
{
	DeleteAttribute(pchar, "quest." + sQuest);
	int i;
	string sLink;
	for (i = 1; i <= iQty; i++)
	{
		sLink = "l" + i;
		pchar.quest.(sQuest).win_condition.(sLink) = "NPC_Death";
		pchar.quest.(sQuest).win_condition.(sLink).character = sPrefix + i;
	}
	pchar.quest.(sQuest).function = sFunction;
}

// Gregg: в К:ПС была функция DisableLandEncounters, в CSP - поле локации
void HSI_SetEncounters(string sLocation, bool bDisable)
{
	int iLoc = FindLocation(sLocation);
	if (iLoc < 0) return;
	Locations[iLoc].DisableEncounters = bDisable;
}

// Gregg: записи журнала - через обёртку, чтобы в тексте работали гендерные формы.
// sSex - обычное окончание («нашёл» / «нашла»), sSex1 - возвратное («ся» / «ась»).
void HSI_Record(string sRec)
{
	AddQuestRecord("HelpSacredInquisition", sRec);
	AddQuestUserData("HelpSacredInquisition", "sSex", GetSexPhrase("", "а"));
	AddQuestUserData("HelpSacredInquisition", "sSex1", GetSexPhrase("ся", "ась"));
}

void HelpSacredInquisitionStart()
{
	PChar.HelpSacredInquisition = "1";

	SetQuestHeader("HelpSacredInquisition");
	HSI_Record("1");
}

void HelpSacredInquisitionFirstToBasTer()
{
	PChar.HelpSacredInquisition = "2";
	HSI_Record("2");
}

void HelpSacredInquisitionToRichHouse()
{
	PChar.HelpSacredInquisition = "3";
	HSI_Record("3");

	// Gregg: вместо отдельного дома - комната, которую Натан снимал в таверне.
	// Локация уже есть и уже связана с таверной, дверь вешать не надо.
	SetQuestBoxItems("BasTer_tavern_upstairs", "HelpSacredInquisition_Letter1", 1);
	SetQuestBoxItems("BasTer_tavern_upstairs", "HelpSacredInquisition_Letter2", 1);

	SetQuestFunction("HelpSacredInquisitionInRichHouse", "HelpSacredInquisitionInRichHouse", "l", "BasTer_tavern_upstairs");
}

void HelpSacredInquisitionInRichHouse(string qName)
{
	chrDisableReloadToLocation = true;

	DeleteAttribute(pchar, "quest.HelpSacredInquisitionFindFirstLetters");
	PChar.quest.HelpSacredInquisitionFindFirstLetters.win_condition.l1 = "item";
	PChar.quest.HelpSacredInquisitionFindFirstLetters.win_condition.l1.item = "HelpSacredInquisition_Letter1";
	PChar.quest.HelpSacredInquisitionFindFirstLetters.win_condition.l2 = "item";
	PChar.quest.HelpSacredInquisitionFindFirstLetters.win_condition.l2.item = "HelpSacredInquisition_Letter2";
	PChar.quest.HelpSacredInquisitionFindFirstLetters.function = "HelpSacredInquisitionFindFirstLetters";
}

void HelpSacredInquisitionFindFirstLetters(string qName)
{
	chrDisableReloadToLocation = false;
	ClearQuestBoxStay("BasTer_tavern_upstairs");

	PChar.HelpSacredInquisition = "4";
	HSI_Record("4");
}

void HelpSacredInquisitionToBasTerExitTown()
{
	PChar.HelpSacredInquisition = "5";

	HSI_SetEncounters("BasTer_ExitTown", true);
	SetQuestFunction("HelpSacredInquisitionInBasTerExitTown", "HelpSacredInquisitionInBasTerExitTown", "l", "BasTer_ExitTown");
}

void HelpSacredInquisitionInBasTerExitTown(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Fransua Defau", "officer_6", "man", "man", 1, PIRATE, 0, true));
	chr.name = iTxt("HSI_DefauName");
	chr.lastname = iTxt("HSI_DefauLastName");
	ChangeCharacterAddressGroup(chr, "BasTer_ExitTown", "goto", "goto5");
	RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(chr, GUN_ITEM_TYPE);

	// Gregg: счётчик локальный, а не общий i - внутри цикла зовётся чужая функция,
	// а глобал в Storm один на всю программу
	int iBlood;
	for (iBlood = 0; iBlood <= 4; iBlood++)
	{
		LaunchBlood(chr, 7.0, true);
	}

	// Gregg: в К:ПС (и в штатных квестах CSP) "труп" - это бессмертный актёр в позе лёжа,
	// а дневник выдавался локатором: подошёл вплотную - предмет молча оказался в кармане.
	// Тело при этом живое: его можно толкать и бить, а обыскать нельзя. Здесь Франсуа
	// убит по-настоящему, дневник лежит на нём, и игрок забирает его обыском.
	chr.SaveItemsForDead = true; // вещи остаются на теле
	chr.NotRemoved = true;       // тело не уберут при чистке
	// Gregg: и, главное, труп не должен раствориться сам. LAi_events.c:709 через 500 секунд
	// после смерти шлёт LAi_event_Character_Dead всем, у кого нет DontClearDead, а MOD_DEAD_CLEAR -
	// это #define "On", не настройка. Дневник продвигает квест условием по предмету и лежит
	// только на теле: не успел обыскать за восемь минут - тело исчезло вместе с дневником,
	// и стадия 5 не закрывается уже никогда.
	chr.DontClearDead = true;
	TakeNItems(chr, "HelpSacredInquisition_NoteBook", 1);
	LAi_SetStayType(chr);
	// Gregg: убиваем СЛЕДУЮЩИМ кадром. В список обыскиваемых тел персонаж попадает через
	// Dead_AddLoginedCharacter, а та первым делом просит GetCharacterPos - у только что
	// поставленного NPC позиции ещё нет, функция возвращает false и молча выходит.
	// Тело в итоге лежит, но обыскать его нельзя ("Некого обыскивать").
	DoQuestFunctionDelay("HelpSacredInquisitionKillDefau", 0.5);

	// Gregg: продвигает квест сам предмет, а не локатор - иначе дневник можно было бы
	// получить, не притронувшись к телу.
	DeleteAttribute(pchar, "quest.HelpSacredInquisitionFindDefauNoteBook");
	pchar.quest.HelpSacredInquisitionFindDefauNoteBook.win_condition.l1 = "item";
	pchar.quest.HelpSacredInquisitionFindDefauNoteBook.win_condition.l1.item = "HelpSacredInquisition_NoteBook";
	pchar.quest.HelpSacredInquisitionFindDefauNoteBook.function = "HelpSacredInquisitionFindDefauNoteBook";
}

// Gregg: отложенное убийство - см. комментарий в HelpSacredInquisitionInBasTerExitTown
void HelpSacredInquisitionKillDefau(string _tmp)
{
	int capIndex = GetCharacterIndex("Fransua Defau");
	if (capIndex == -1) return;

	ref chr = &characters[capIndex];
	LAi_SetWarriorType(chr);
	LAi_KillCharacter(chr);
}

// Gregg: срабатывает, когда дневник оказался у ГГ. Предмет он снимает с тела сам,
// поэтому выдавать тут уже нечего - только зовём стражу.
void HelpSacredInquisitionFindDefauNoteBook(string qName)
{
	Log_InfoS(iTxt("HSI_FoundNoteBook"));

	DoQuestFunctionDelay("HelpSacredInquisitionBasTerExitSold", 2.5);
}

void HelpSacredInquisitionBasTerExitSold(string _tmp)
{
	int iNation = sti(Colonies[FindColony("BasTer")].nation);
	string sNation = GetShortNationNameByType(iNation);

	int iRand = 7;
	string sModel = "sold_" + sNation + "_";

	if (iNation == PIRATE)
	{
		sModel = "pirate_";
		iRand = 9;
	}

	// Gregg: НЕ GetRank(PChar, ...) - в CSP эта функция сама прибавляет ранг ГГ внутри
	// (в К:ПС не прибавляла), и при передаче PChar ранг считался дважды: на 25-м уровне
	// игрока стража выходила 55-го. Считаем прямо.
	int iRank = sti(pchar.rank) + 5 + MOD_SKILL_ENEMY_RATE;
	int iSold;
	ref chr;
	for (iSold = 1; iSold <= 5; iSold++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("HelpSacredInquisition Sold " + iSold, sModel + (rand(iRand) + 1), "man", "man", iRank, PIRATE, 0, true));
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload4");
		chr.Dialog.FileName = "Quest\KPS\HelpSacredInquisition\HelpSacredInquisition.c";
		chr.Dialog.CurrentNode = "BasTer_Guardians_1";
		chr.greeting = "soldier_arest";
		LAi_SetActorType(chr);
		LAi_ActorFollow(chr, PChar, "", 60.0);

		if (iSold == 1)
		{
			SetDialogCharacter2Character(PChar, chr, 60.0, false);
		}
	}
}

void HelpSacredInquisitionFightWithSoldAtExitBasTer()
{
	int iSold;
	ref chr;
	for (iSold = 1; iSold <= 5; iSold++)
	{
		chr = CharacterFromID("HelpSacredInquisition Sold " + iSold);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	HSI_SetGroupDeadFunction("HelpSacredInquisitionFightWithSoldAtExitBasTerW", "HelpSacredInquisitionFightWithSoldAtExitBasTerW", "HelpSacredInquisition Sold ", 5);
}

void HelpSacredInquisitionFightWithSoldAtExitBasTerW(string qName)
{
	chrDisableReloadToLocation = false;
	PChar.HelpSacredInquisition = "6";
	HSI_Record("5");

	HSI_SetEncounters("BasTer_ExitTown", false);
}

void HelpSacredInquisitionFindJacklin()
{
	PChar.HelpSacredInquisition = "7";

	ref chr = GetCharacter(NPC_GenerateCharacter("Jacklin Defau", "girl_8", "woman", "woman", 1, FRANCE, -1, false));
	chr.Dialog.FileName = "Quest\KPS\HelpSacredInquisition\HelpSacredInquisition.c";
	chr.Dialog.CurrentNode = "Jaklin_Defo_1";
	chr.name = iTxt("HSI_JacklinName");
	chr.lastname = iTxt("HSI_DefauLastName");
	chr.greeting = "Gr_Dama";
	LAi_SetCitizenType(chr);
	ChangeCharacterAddressGroup(chr, "Tortuga_town", "goto", "goto8");
	SetQuestMarkToCharacter(chr, true);
}

void HelpSacredInquisitionToBermudes()
{
	ref chr = CharacterFromID("Jacklin Defau");
	SetQuestMarkToCharacter(chr, false);
	chr.LifeDay = 0;

	HSI_Record("6");

	HSI_SetEncounters("Shore3", true);
	SetQuestFunction("HelpSacredInquisitionInBermudes", "HelpSacredInquisitionInBermudes", "l", "Shore3");
}

void HelpSacredInquisitionInBermudes(string qName)
{
	chrDisableReloadToLocation = true;
	ChangeCharacterAddressGroup(PChar, "Shore3", "reload", "reload1");

	ref chr;
	// Gregg: НЕ GetRank(PChar, ...) - в CSP эта функция сама прибавляет ранг ГГ внутри
	// (в К:ПС не прибавляла), и при передаче PChar ранг считался дважды: на 25-м уровне
	// игрока стража выходила 55-го. Считаем прямо.
	int iRank = sti(pchar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	string sCharacter = "";
	string sModel = "";
	int iMan;
	for (iMan = 1; iMan <= 3; iMan++)
	{
		sCharacter = "Natan Rich Guardian " + (iMan - 1);
		sModel = "pirate_" + (rand(9) + 1);
		if (iMan == 1)
		{
			sCharacter = "Natan Rich";
			sModel = "officer_17";
		}

		chr = GetCharacter(NPC_GenerateCharacter(sCharacter, sModel, "man", "man", iRank, PIRATE, 0, true));
		chr.Dialog.FileName = "Quest\KPS\HelpSacredInquisition\HelpSacredInquisition.c";
		chr.Dialog.CurrentNode = "Natan_Rich_1";
		chr.SaveItemsForDead = true;
		chr.greeting = "soldier_arest";
		ChangeCharacterAddressGroup(chr, "Shore3", "goto", "goto" + (iMan + 11));
		LAi_SetImmortal(chr, true);
		LAi_SetActorType(chr);

		if (iMan == 1)
		{
			// Gregg: имя - только настоящему Ричу. Стражам остаются случайные имена
			// из генератора, иначе над всеми тремя висит «Натан Рич» и цель не отличить.
			chr.name = iTxt("HSI_NatanName");
			chr.lastname = iTxt("HSI_NatanLastName");
			FantomMakeCoolFighter(chr, 30, 100, 100, "", "", 500);
			LAi_NPC_Equip(chr, iRank, true, true);
			SetFoodToCharacter(chr, 2, 15);
		}
		else
		{
			LAi_ActorTurnToCharacter(chr, CharacterFromID("Natan Rich"));
		}
	}

	HSI_SetLocatorFunction("HelpSacredInquisitionInBermudesTalk", "HelpSacredInquisitionInBermudesTalk", "Shore3", "encdetector", "enc02");
}

void HelpSacredInquisitionInBermudesTalk(string qName)
{
	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	LAi_SetActorType(PChar);
	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, 10, 3.5, 10, false);

	DoQuestFunctionDelay("HelpSacredInquisitionBermudesDialog", 4.0);
}

void HelpSacredInquisitionBermudesDialog(string _tmp)
{
	ref chr = CharacterFromID("Natan Rich");
	SetDialogCharacter2Character(PChar, chr, 0.1, true);
}

void HelpSacredInquisitionInBermudesFirstFight()
{
	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;

	LAi_SetPlayerType(PChar);

	int iMan;
	ref chr;
	for (iMan = 1; iMan <= 2; iMan++)
	{
		chr = CharacterFromID("Natan Rich Guardian " + iMan);
		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	HSI_SetGroupDeadFunction("HelpSacredInquisitionInBermudesFirstFightWin", "HelpSacredInquisitionInBermudesFirstFightWin", "Natan Rich Guardian ", 2);
}

void HelpSacredInquisitionInBermudesFirstFightWin(string qName)
{
	ref chr = CharacterFromID("Natan Rich");
	chr.Dialog.CurrentNode = "Natan_Rich_5";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void HelpSacredInquisitionInBermudesFightWithRich()
{
	ref chr = CharacterFromID("Natan Rich");
	LAi_SetImmortal(chr, false);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DeleteAttribute(pchar, "quest.HelpSacredInquisitionInBermudesFightWithRichWin");
	PChar.quest.HelpSacredInquisitionInBermudesFightWithRichWin.win_condition.l1 = "NPC_Death";
	PChar.quest.HelpSacredInquisitionInBermudesFightWithRichWin.win_condition.l1.character = "Natan Rich";
	PChar.quest.HelpSacredInquisitionInBermudesFightWithRichWin.function = "HelpSacredInquisitionInBermudesFightWithRichWin";
}

void HelpSacredInquisitionInBermudesFightWithRichWin(string qName)
{
	chrDisableReloadToLocation = false;
	HSI_SetEncounters("Shore3", false);

	TakeNItems(PChar, "HelpSacredInquisition_Letter1", -1);
	TakeNItems(PChar, "HelpSacredInquisition_Letter2", -1);
	TakeNItems(PChar, "HelpSacredInquisition_NoteBook", -1);

	PChar.HelpSacredInquisition = "Good";
	HSI_Record("7");

	// Gregg: отсюда сразу начинается "Легенда о призрачном свете" - в К:ПС так же.
	MoonStoneStart();
}

void HelpSacredInquisitionEnd()
{
	AddMoneyToCharacter(PChar, 100000);
	CloseQuestHeader("HelpSacredInquisition");
	pchar.questdata.HelpSacredInquisition = 1;
	UnlockAchievement("Inquisition_quest", 3);
	PChar.HelpSacredInquisition = "END";
}
//<== Gregg: "Помощь святой инквизиции"
