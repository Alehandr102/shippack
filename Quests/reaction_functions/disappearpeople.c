// Gregg: ==> «Исчезновение людей» (перенос из К:ПС)
//
// Источник: К:ПС, PROGRAM\lugger\Quests.c, раздел «ИСЧЕЗНОВЕНИЕ ЛЮДЕЙ».
// Имена функций сохранены как в оригинале, чтобы можно было сверяться с источником.
//
// Завязка автоматическая: 10 ранг -> первый заход в город -> первый заход к любой
// пещере. Дальше: Билл Метью и первая монета -> засада на берегу -> Жюль Готье
// в Мариго -> табличка у храма Инков на Мейне -> засада скелетов у храма ->
// блокада Сан-Мартина -> обратно к Готье -> блокада Барбадоса -> пещера Барбадоса,
// Бен Грегори.
//
// Отличия от оригинала - в комментариях по коду. Главные:
// - SHIP_WARSHIP в CSP нет; вместо линкора тяжёлый ост-индец SHIP_HERCULES через
//   FantomMakeCoolestSailor. Калибр опущен с 32 до 20 фунтов - у ост-индца это потолок;
// - ранги противников ФИКСИРОВАНЫ, а не считаются от ранга ГГ: квест ранний,
//   привязка к игроку на 30-м ранге давала бы толпу 40-х;
// - GetRank(PChar, N) не используется: в CSP она сама прибавляет ранг ГГ внутри,
//   и при передаче PChar он считался дважды;
// - награда с тела Грегори: валлонская шпага blade33 вместо topor4, которого в CSP нет
//   (и который слабее наших же топоров);
// - табличка из храма ложится на локатор item2, а не item1: на item1 в храме может
//   лежать чужой предмет - Скрижали Аскольда (Rock_letter, один из пяти вариантов).

#define DP_RANK_REQ			10		// с какого ранга завязывается квест (снимается опцией старта)
#define DP_RANK_SKELETON	13		// рядовые скелеты по всему квесту
#define DP_RANK_SEACAP		20		// капитаны-скелеты на ост-индцах
#define DP_RANK_GREGORI		60		// Бен Грегори, финальный бой
#define DP_GREGORI_HP		999.0	// его же запас здоровья, ставится жёстко
#define DP_GREGORI_MONEY	111111	// денег на теле
// Gregg: запись журнала одной строкой - и сразу род ГГ. Функции в .txt не работают,
// поэтому в тексте записи стоят плейсхолдеры, а значения приходят отсюда.
// AddQuestUserData цепляется к ПОСЛЕДНЕЙ добавленной записи (quests.c:404), а не
// к квесту целиком, поэтому подстановку повторяем после КАЖДОЙ записи.
// sSex - обычное окончание, sSex1 - возвратная форма.
void DP_Record(string sRec)
{
	AddQuestRecord("DisappearPeople", sRec);
	AddQuestUserData("DisappearPeople", "sSex", GetSexPhrase("", "а"));
	AddQuestUserData("DisappearPeople", "sSex1", GetSexPhrase("ся", "ась"));
}



// Gregg: в К:ПС это DisableLandEncounters из lugger\utils.c, в CSP такой обёртки нет -
// локации гасят встречи атрибутом напрямую.
void DP_SetEncounters(string sLocation, bool bDisable)
{
	int iLoc = FindLocation(sLocation);
	if (iLoc < 0) return;
	if (bDisable)
	{
		Locations[iLoc].DisableEncounters = true;
		return;
	}
	if (CheckAttribute(&Locations[iLoc], "DisableEncounters")) DeleteAttribute(&Locations[iLoc], "DisableEncounters");
}

// Gregg: SetQuestNPCDeadFunction из lugger\utils.c - условие победы на смерть
// сразу нескольких NPC с общим префиксом имени.
void DP_SetNPCDeadFunction(string sQuest, string sFunction, string sName, int iQty)
{
	// Gregg: счётчик локальный, а не общий i
	int iN;
	string sKey;
	for (iN = 1; iN <= iQty; iN++)
	{
		sKey = "l" + iN;
		PChar.quest.(sQuest).win_condition.(sKey) = "NPC_Death";
		PChar.quest.(sQuest).win_condition.(sKey).character = sName + iN;
	}
	PChar.quest.(sQuest).function = sFunction;
}

// Gregg: поднять одного скелета. В оригинале этот блок повторялся четырежды слово в слово.
ref DP_MakeSkeleton(string sId)
{
	string sModel = "Skel" + (rand(3)+1);
	ref chr = GetCharacter(NPC_GenerateCharacter(sId, sModel, "skeleton", "man", DP_RANK_SKELETON, PIRATE, 0, true));
	chr.NotRemoved = true;
	return chr;
}

// Gregg: найти ключ двери по ИМЕНИ локатора, а не по номеру reload.lN - номера
// в разных городах расходятся, и цепляться за них значит однажды перецепить чужой дом.
// Те же грабли ловили в «Товаре для Ричарда Тилльманна».
string DP_FindReloadKeyByName(string sLocation, string sLocatorName)
{
	int loc = FindLocation(sLocation);
	if (loc < 0) return "";
	aref ar, el;
	makearef(ar, Locations[loc].reload);
	int iDoor;
	for (iDoor = 0; iDoor < GetAttributesNum(ar); iDoor++)
	{
		el = GetAttributeN(ar, iDoor);
		if (!CheckAttribute(el, "name")) continue;
		if (el.name == sLocatorName) return GetAttributeName(el);
	}
	return "";
}

// Gregg: перецепить дверь-заглушку на дом Готье и обратно. Суточный рандомайзер
// дверей (reload.c) пишет disable = rand(1) каждой двери, у которой в .go есть
// подстрока «Common». Меняем .go на свой дом - подстрока пропадает, а выпавшая
// единица остаётся навсегда, и вход мёртв. Поэтому disable снимаем явно.
void DP_SetGotieDoor(bool bOpen)
{
	int loc = FindLocation("Marigo_town");
	if (loc < 0) return;
	string sKey = DP_FindReloadKeyByName("Marigo_town", "houseSp1");
	if (sKey == "")
	{
		Log_TestInfo("DisappearPeople: в Мариго не найден локатор двери houseSp1");
		return;
	}
	if (bOpen)
	{
		if (FindLocation("HouseJuleGotie") < 0)
		{
			Log_TestInfo("DisappearPeople: локация HouseJuleGotie не создана");
			return;
		}
		Locations[loc].reload.(sKey).go = "HouseJuleGotie";
		Locations[loc].reload.(sKey).label = "House Jule Gotie";
	}
	else
	{
		Locations[loc].reload.(sKey).go = "CommonDoubleflourHouse_1";
		Locations[loc].reload.(sKey).label = "House";
	}
	if (CheckAttribute(&Locations[loc], "reload." + sKey + ".disable")) DeleteAttribute(&Locations[loc], "reload." + sKey + ".disable");
}

// Gregg: общая сборка морского противника. В оригинале два почти одинаковых блока
// на Сан-Мартине и Барбадосе; разошлись они только именем корабля и адресом.
void DP_MakeSeaShip(string sId, string sGroup, string sShipName, string sIsland, bool bTop)
{
	Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup, "war");

	// Gregg: база капитана-скелета у нас PGG_Skeletcap_0, а не skeletcap из К:ПС.
	// Система вариантов брони дописывает к модели номер (офицерская кираса - _4),
	// и у skeletcap такого файла нет: модель не грузилась, а BLADE::Realize потом
	// падал на абордаже, пытаясь прицепить клинок к несуществующему персонажу.
	ref sld = GetCharacter(NPC_GenerateCharacter(sId, "PGG_Skeletcap_0", "skeleton", "man", DP_RANK_SEACAP, PIRATE, 0, true));
	// Gregg: у нас флаг команды-скелетов называется иначе, чем в К:ПС
	sld.GenQuest.CrewSkelModeClassic = true;
	// Gregg: тяжёлый ост-индец вместо несуществующего у нас линкора. Калибр 20 -
	// у Hercules это потолок, 32 фунта из оригинала всё равно срезались бы.
	// «Армагеддон» идёт рядовым (GenerateShipExt), «Демон» - топовым (GenerateShipTop):
	// второй встречается позже, он же и получает собственные статы.
	if (bTop) FantomMakeCoolestSailor(sld, SHIP_HERCULES, sShipName, CANNON_TYPE_CANNON_LBS20, 100, 95, 95);
	else FantomMakeCoolSailor(sld, SHIP_HERCULES, sShipName, CANNON_TYPE_CANNON_LBS20, 100, 95, 95);
	FantomMakeCoolFighter(sld, DP_RANK_SEACAP, 100, 100, "", "", 300);
	SetGeraldDetails(sld, -1, rand(7), "", "pir_" + (rand(5)+1));

	if (bTop)
	{
		// Gregg: ==> статы «Демона». Патчится слот в RealShips, заведённый GenerateShipTop,
		// а не шаблон типа, поэтому прибавки уезжают вместе с кораблём к тому,
		// кто возьмёт его на абордаж.
		realships[sti(sld.ship.type)].HP = 4200;                  // было 4000
		realships[sti(sld.ship.type)].SpeedRate = 14.2;           // было 13.0
		realships[sti(sld.ship.type)].TurnRate = 17.5;            // было 14.5
		realships[sti(sld.ship.type)].WindAgainstSpeed = 3.5;     // было 4.0
		realships[sti(sld.ship.type)].Capacity = 2400;            // было 2000
		// Gregg: <== статы «Демона»
	}

	Group_AddCharacter(sGroup, sId);
	Group_SetGroupCommander(sGroup, sId);
	Group_SetTaskAttack(sGroup, PLAYER_GROUP);
	Group_SetAddress(sGroup, sIsland, "quest_ships", "quest_ship_4");
	Group_LockTask(sGroup);
}

void DisappearPeoplePrepare()
{
	PChar.DisappearPeople.Action = false;
	// LEO: порог ранга - через переменную с оглядкой на опцию старта "квесты без требований к рангам",
	//   как у остальных перенесённых квестов. Раньше 10 стояло прямо в условии, и опция его не снимала:
	//   это был единственный из восьми квестов К:ПС, куда чек не прокинули.
	int iRankReq = 1;
	if (!bQuestsRank) iRankReq = DP_RANK_REQ;
	// Gregg: у нас условие пишется с заглавной - Rank
	PChar.quest.DisappearPeoplePreStart.win_condition.l1 = "Rank";
	PChar.quest.DisappearPeoplePreStart.win_condition.l1.value = iRankReq;
	// Gregg: без operation TestIntValue сравнивать нечем - у нас это обязательный параметр
	PChar.quest.DisappearPeoplePreStart.win_condition.l1.operation = ">=";
	PChar.quest.DisappearPeoplePreStart.function = "DisappearPeoplePreStart";
}

void DisappearPeoplePreStart(string qName)
{
	PChar.DisappearPeople.Action = true;
	PChar.quest.DisappearPeopleStart.win_condition.l1 = "Location_Type";
	PChar.quest.DisappearPeopleStart.win_condition.l1.location_type = "town";
	PChar.quest.DisappearPeopleStart.function = "DisappearPeopleStart";
}

void DisappearPeopleStart(string qName)
{
	SetQuestHeader("DisappearPeople");
	DP_Record("1");

	PChar.DisappearPeople = "1";

	// Gregg: тип условия перенесён в quests_check.c - совпадение по имени локации,
	// а не по атрибуту CaveEntrance, как в К:ПС: атрибут пришлось бы вешать на
	// 29 локаций, а он приезжает из сейва и на старых партиях его не было бы.
	PChar.quest.DisappearPeopleFisrtBattle.win_condition.l1 = "Location_Type_CaveEntrance";
	PChar.quest.DisappearPeopleFisrtBattle.function = "DisappearPeopleFisrtBattle";
}

void DisappearPeopleFisrtBattle(string qName)
{
	DP_SetEncounters(PChar.location, true);

	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	chrDisableReloadToLocation = true;

	LAi_SetActorType(PChar);
	ref chr;

	chr = GetCharacter(NPC_GenerateCharacter("Bill Matthew", "officer_" + (rand(19)+1), "man", "man", 15, PIRATE, 0, true));
	chr.name = iTxt("DP_BillName");
	chr.lastname = iTxt("DP_BillLastName");
	chr.NotRemoved = true;
	chr.Dialog.FileName = "Quest\KPS\DisappearPeople\DP_Quest.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetWarriorType(chr);
	LAi_SetImmortal(chr, true);
	LAi_group_MoveCharacter(chr, "BILL_MATTHEW");
	ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");

	// Gregg: у входов в пещеры две разные модели локаторов. Jungles\CaveEnt1 и CaveEnt2
	// дают monster1..3, а JunglesN\CaveEnt1 - monsters1..3, с буквой s. По второй модели
	// живут Картахена, Ямайка, Маракайбо и Панама, и там телепорт скелетов срывался
	// с ошибкой в лог. Определяем имя по факту наличия локатора.
	// LEO: FindLocator возвращает ссылку на атрибут локатора через aref.
	string sMonPrefix = "monster";
	aref rProbe;
	if (!FindLocator(PChar.location, "monster1", &rProbe, true)) sMonPrefix = "monsters";

	int iSkel;
	for (iSkel = 1; iSkel <= 3; iSkel++)
	{
		chr = DP_MakeSkeleton("Bill Matthew Killer " + iSkel);
		ChangeCharacterAddressGroup(chr, PChar.location, "monsters", sMonPrefix + iSkel);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "BILL_MATTHEW_KILLERS");
	}

	LAi_group_SetRelation("BILL_MATTHEW_KILLERS", "BILL_MATTHEW", LAI_GROUP_ENEMY);
	LAi_group_FightGroups("BILL_MATTHEW_KILLERS", "BILL_MATTHEW", true);

	ref loc = PCharLoc();
	DeleteAllFantomCharactersFromLocation(loc);

	DoQuestFunctionDelay("DisappearPeopleFisrtBattleStart", 7.0);
}

void DisappearPeopleFisrtBattleStart(string _tmp)
{
	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;

	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("Bill Matthew");
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	LAi_group_Delete("BILL_MATTHEW");
	LAi_group_Delete("BILL_MATTHEW_KILLERS");

	int iSkel;
	for (iSkel = 1; iSkel <= 3; iSkel++)
	{
		chr = CharacterFromID("Bill Matthew Killer " + iSkel);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DP_SetNPCDeadFunction("DisappearPeopleFisrtBattleWin", "DisappearPeopleFisrtBattleWin", "Bill Matthew Killer ", 3);
}

void DisappearPeopleFisrtBattleWin(string qName)
{
	ref chr = CharacterFromID("Bill Matthew");
	SetDialogCharacter2Character(PChar, chr, 15.0, false);
}

void DisappearPeopleTakeCoin()
{
	DP_SetEncounters(PChar.location, false);
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Bill Matthew");
	LAi_SetActorType(chr);
	// Gregg: таймаут -1 отключает LAi_tmpl_runto_Teleport, и актёр, не нашедший путь,
	// залипает в «falure» навсегда. Обжигались на Педро Карлосе в прологе.
	LAi_ActorRunToLocation(chr, "reload", "reload2_back", "none", "", "", "", 30.0);

	DP_Record("2");
	PChar.DisappearPeople = "2";

	TakeNItems(PChar, "DisappearPeople_Coin", 1);
	PChar.quest.DisappearPeopleTwoMeeting.win_condition.l1 = "Location_Type";
	PChar.quest.DisappearPeopleTwoMeeting.win_condition.l1.location_type = "seashore";
	PChar.quest.DisappearPeopleTwoMeeting.function = "DisappearPeopleTwoMeeting";
}

void DisappearPeopleTwoMeeting(string qName)
{
	DP_SetEncounters(PChar.location, true);
	LAi_SetActorType(PChar);

	chrDisableReloadToLocation = true;

	ref chr;
	int iSkel;
	for (iSkel = 1; iSkel <= 4; iSkel++)
	{
		chr = DP_MakeSkeleton("DisappearPeople in Shore Killer " + iSkel);
		ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto" + iSkel);
		LAi_SetActorType(chr);
		LAi_ActorTurnToCharacter(chr, PChar);
	}

	ref loc = PCharLoc();
	DeleteAllFantomCharactersFromLocation(loc);

	DoQuestFunctionDelay("DisappearPeopleTwoMeetingBattle", 3.0);
}

void DisappearPeopleTwoMeetingBattle(string _tmp)
{
	LAi_SetPlayerType(PChar);
	ref chr;
	int iSkel;
	for (iSkel = 1; iSkel <= 4; iSkel++)
	{
		chr = CharacterFromID("DisappearPeople in Shore Killer " + iSkel);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DP_SetNPCDeadFunction("DisappearPeopleTwoMeetingBattleWin", "DisappearPeopleTwoMeetingBattleWin", "DisappearPeople in Shore Killer ", 4);
}

void DisappearPeopleTwoMeetingBattleWin(string qName)
{
	DP_SetEncounters(PChar.location, false);
	chrDisableReloadToLocation = false;

	DP_Record("3");
	PChar.DisappearPeople = "3";
}

void DisappearPeopleToGotie()
{
	DP_Record("4");
	PChar.DisappearPeople = "4";

	DP_SetGotieDoor(true);

	ref chr = GetCharacter(NPC_GenerateCharacter("Jule Gotie", "usurer_5", "man", "man", 1, PIRATE, -1, true));
	chr.name = iTxt("DP_GotieName");
	chr.lastname = iTxt("DP_GotieLastName");
	chr.Dialog.FileName = "Quest\KPS\DisappearPeople\DP_Gotie.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "HouseJuleGotie", "barmen", "stay");
}

// Gregg: трактирщик голландской колонии назвал город. Первая наводка обходится
// «одной из колоний Нидерландов», а их три - без этой записи игрок,
// расспросив трактирщика, назавтра забудет, куда идти.
void DisappearPeopleGotieFound()
{
	if (CheckAttribute(PChar, "DisappearPeople.GotieTown")) return;
	PChar.DisappearPeople.GotieTown = true;
	DP_Record("11");
}

void DisappearPeopleToTemple()
{
	// LEO: игрок нашёл Готье сам - подсказка трактирщика больше не нужна, гасим строчку.
	PChar.DisappearPeople.GotieTown = true;
	ref itm = ItemsFromID("DisappearPeople_Table");
	itm.shown = true;
	itm.startLocation = "Temple";
	// Gregg: item1 в храме занят - туда может лечь Rock_letter из линейки Аскольда
	itm.startLocator = "item2";

	// Gregg: в оригинале после этого разговора в журнал не писалось ничего, и игрок,
	// проскочивший диалог, оставался без единой подсказки, куда плыть
	DP_Record("9");

	// Gregg: засада вешается на ЛОКАТОР таблички, а не на появление её в инвентаре.
	// Условие "item" опрашивается только на очередном прогоне QuestsCheck(), а он
	// к подбору вещи не привязан: скелеты поднимались с большим опозданием - иногда
	// лишь когда игрок открывал инвентарь. Локаторное условие проверяется в тике локации.
	PChar.quest.DisappearPeopleFindTable.win_condition.l1 = "locator";
	PChar.quest.DisappearPeopleFindTable.win_condition.l1.location = "Temple";
	PChar.quest.DisappearPeopleFindTable.win_condition.l1.locator_group = "item";
	PChar.quest.DisappearPeopleFindTable.win_condition.l1.locator = "item2";
	PChar.quest.DisappearPeopleFindTable.function = "DisappearPeopleFindTable";
}

void DisappearPeopleFindTable(string qName)
{
	DP_Record("6");
	chrDisableReloadToLocation = true;

	ref chr;
	int iSkel;
	for (iSkel = 1; iSkel <= 5; iSkel++)
	{
		chr = DP_MakeSkeleton("DisappearPeople in Temple " + iSkel);
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	ref loc = PCharLoc();
	DeleteAllFantomCharactersFromLocation(loc);

	DP_SetNPCDeadFunction("DisappearPeopleBattleAtTemple", "DisappearPeopleBattleAtTemple", "DisappearPeople in Temple ", 5);
}

void DisappearPeopleBattleAtTemple(string qName)
{
	chrDisableReloadToLocation = false;
	// Gregg: и здесь оригинал молчал - табличка на руках, а куда с ней идти, неясно
	DP_Record("10");
	ref rGotie = CharacterFromID("Jule Gotie");
	rGotie.Dialog.CurrentNode = "4";

	PChar.quest.DisappearPeopleAtSentMartin.win_condition.l1 = "location";
	PChar.quest.DisappearPeopleAtSentMartin.win_condition.l1.location = "SentMartin";
	PChar.quest.DisappearPeopleAtSentMartin.function = "DisappearPeopleAtSentMartin";
}

void DisappearPeopleAtSentMartin(string qName)
{
	bQuestDisableMapEnter = true;
	Island_SetReloadEnableGlobal("SentMartin", false);

	DP_MakeSeaShip("DisappearPeopleSeaShip_1", "DisappearPeopleSea", iTxt("DP_ShipArmageddon"), "SentMartin", false);

	PChar.quest.DisappearPeopleAtSentMartinWinner.win_condition.l1 = "Group_Death";
	PChar.quest.DisappearPeopleAtSentMartinWinner.win_condition.l1.group = "DisappearPeopleSea";
	PChar.quest.DisappearPeopleAtSentMartinWinner.function = "DisappearPeopleAtSentMartinWinner";
}

void DisappearPeopleAtSentMartinWinner(string qName)
{
	bQuestDisableMapEnter = false;
	Island_SetReloadEnableGlobal("SentMartin", true);

	DP_Record("5");
	PChar.DisappearPeople = "5";
}

void DisappearPeopleToBarbados()
{
	PChar.DisappearPeople = "6";
	DP_Record("7");

	PChar.quest.DisappearPeopleInBarbados.win_condition.l1 = "location";
	PChar.quest.DisappearPeopleInBarbados.win_condition.l1.location = "Barbados";
	PChar.quest.DisappearPeopleInBarbados.function = "DisappearPeopleInBarbados";
}

void DisappearPeopleInBarbados(string qName)
{
	bQuestDisableMapEnter = true;
	Island_SetReloadEnableGlobal("Barbados", false);

	DP_MakeSeaShip("DisappearPeopleMoon_1", "DisappearPeopleMoon", iTxt("DP_ShipDemon"), "Barbados", true);

	PChar.quest.DisappearPeopleInBarbadosWinner.win_condition.l1 = "Group_Death";
	PChar.quest.DisappearPeopleInBarbadosWinner.win_condition.l1.group = "DisappearPeopleMoon";
	PChar.quest.DisappearPeopleInBarbadosWinner.function = "DisappearPeopleInBarbadosWinner";
}

void DisappearPeopleInBarbadosWinner(string qName)
{
	bQuestDisableMapEnter = false;
	Island_SetReloadEnableGlobal("Barbados", true);

	PChar.DisappearPeople = "7";
	DP_SetEncounters("Barbados_CaveEntrance", true);

	PChar.quest.DisappearPeopleToCaveEntrance.win_condition.l1 = "location";
	PChar.quest.DisappearPeopleToCaveEntrance.win_condition.l1.location = "Barbados_CaveEntrance";
	PChar.quest.DisappearPeopleToCaveEntrance.function = "DisappearPeopleToCaveEntrance";
}

void DisappearPeopleToCaveEntrance(string qName)
{
	DP_SetEncounters(PChar.location, true);
	chrDisableReloadToLocation = true;

	ref chr;
	int iSkel;
	for (iSkel = 1; iSkel <= 5; iSkel++)
	{
		chr = DP_MakeSkeleton("DisappearPeople At Cave " + iSkel);
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DP_SetNPCDeadFunction("DisappearPeopleToCaveEntranceWinner", "DisappearPeopleToCaveEntranceWinner", "DisappearPeople At Cave ", 5);
}

void DisappearPeopleToCaveEntranceWinner(string qName)
{
	DP_SetEncounters(PChar.location, false);
	chrDisableReloadToLocation = false;
	PChar.DisappearPeople = "8";

	// Gregg: расширяем радиус локатора, чтобы разговор с Грегори завязался наверняка
	Locations[FindLocation("Barbados_Cave")].locators_radius.goto.goto3 = 2.5;

	PChar.quest.DisappearPeopleFinal.win_condition.l1 = "location";
	PChar.quest.DisappearPeopleFinal.win_condition.l1.location = "Barbados_Cave";
	PChar.quest.DisappearPeopleFinal.function = "DisappearPeopleFinal";
}

void DisappearPeopleFinal(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Ben Gregori", "Skel_5", "skeleton", "man", DP_RANK_GREGORI, PIRATE, 0, true));
	chr.name = iTxt("DP_GregoriName");
	chr.lastname = iTxt("DP_GregoriLastName");
	// Gregg: анимация man2 из К:ПС у нас отсутствует - персонаж просто не создавался
	// («can invalide model or animation» в compile.log), и Грегори в пещере не было
	chr.Dialog.FileName = "Quest\KPS\DisappearPeople\DP_Quest.c";
	chr.Dialog.CurrentNode = "7";
	chr.SaveItemsForDead = true;
	// Gregg: без этого тело с наградой растворяется через 500 секунд после смерти -
	// ровно та мина, на которой стоял труп Дефо у ворот Бас-Тера в Инквизиции
	chr.DontClearDead = true;
	LAi_SetImmortal(chr, true);
	FantomMakeCoolFighter(chr, DP_RANK_GREGORI, 100, 100, "", "", DP_GREGORI_HP);
	// Gregg: здоровье прибиваем гвоздями - FantomMakeCoolFighter принимает его как
	// пожелание, а не как окончательное значение
	LAi_SetHP(chr, DP_GREGORI_HP, DP_GREGORI_HP);
	DeleteAttribute(chr, "equip");
	DeleteAttribute(chr, "items");
	// Gregg: topor4 в CSP нет, да и по цифрам он слабее наших топоров. Валлонская шпага.
	GiveItem2Character(chr, "blade33");
	EquipCharacterByItem(chr, "blade33");
	GiveItem2Character(chr, "pistol6");
	EquipCharacterByItem(chr, "pistol6");
	SetFoodToCharacter(chr, 2, 15);
	chr.money = DP_GREGORI_MONEY;
	LAi_SetActorType(chr);
	ChangeCharacterAddressGroup(chr, "Barbados_Cave", "item", "berglar1");

	PChar.quest.DisappearPeopleFinalTalk.win_condition.l1 = "locator";
	PChar.quest.DisappearPeopleFinalTalk.win_condition.l1.location = "Barbados_Cave";
	PChar.quest.DisappearPeopleFinalTalk.win_condition.l1.locator_group = "goto";
	PChar.quest.DisappearPeopleFinalTalk.win_condition.l1.locator = "goto3";
	PChar.quest.DisappearPeopleFinalTalk.function = "DisappearPeopleFinalTalk";
}

void DisappearPeopleFinalTalk(string qName)
{
	ref chr = CharacterFromID("Ben Gregori");
	SetDialogCharacter2Character(PChar, chr, 20.0, false);
}

void DisappearPeopleFinalFight()
{
	ref chr = CharacterFromID("Ben Gregori");
	LAi_SetImmortal(chr, false);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.DisappearPeopleHappyEnd.win_condition.l1 = "NPC_Death";
	PChar.quest.DisappearPeopleHappyEnd.win_condition.l1.character = "Ben Gregori";
	PChar.quest.DisappearPeopleHappyEnd.function = "DisappearPeopleHappyEnd";
}

void DisappearPeopleHappyEnd(string qName)
{
	Locations[FindLocation("Barbados_Cave")].locators_radius.goto.goto3 = 0.5;
	DP_SetEncounters("Barbados_CaveEntrance", false);

	chrDisableReloadToLocation = false;

	PChar.DisappearPeople = "END";
	DP_Record("8");
	CloseQuestHeader("DisappearPeople");
	pchar.questdata.DisappearPeople = 1;
	UnlockAchievement("DisappearPeople_quest", 3);

	TakeNItems(PChar, "DisappearPeople_Coin", -1);
	TakeNItems(PChar, "DisappearPeople_Coins", 1);

	PChar.DisappearPeople.Action = false;
	SetTimerFunction("DisappearPeopleDeleteJuleGotie", 0, 0, 20);
}

void DisappearPeopleDeleteJuleGotie(string qName)
{
	DP_SetGotieDoor(false);
	Characters[GetCharacterIndex("Jule Gotie")].LifeDay = 0;
}
// Gregg: <== «Исчезновение людей»
