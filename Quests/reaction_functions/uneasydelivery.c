// Gregg: ==> «Товар для Ричарда Тилльманна» (перенос из К:ПС, глава 2 «Непростая доставка»)
//
// Источник: К:ПС, PROGRAM\lugger\MainLine.c, раздел «2. НЕПРОСТАЯ ДОСТАВКА».
// Имена функций сохранены как в оригинале, чтобы можно было сверяться с источником.
//
// Маршрут: Бриджтаун (дом Тилльманна, груз фарфора) -> Порт-о-Принс (дом Преноля,
// засада) -> церковь Порт-о-Принса -> таверна Сент-Джонса (Шон Стабловски) ->
// обратно в дом Преноля за письмом -> Бриджтаун, убийство Тилльманна ->
// жемчужный промысел (индеец Чарли) -> пиратская таверна (Боб Маккейн) ->
// ночной берег, расправа над Пренолем -> схрон у маяка Сантьяго -> расчёт с Шоном.
//
// Отличия от оригинала - в комментариях по коду. Главные:
// - суммы урезаны на порядок (схрон 50 000 вместо 300 000, долг Шона 20 000
//   вместо 200 000, и так же все суммы в репликах):
//   квест ранний, цепляется сразу за пролог;
// - срок ожидания Шона в репликах: две недели вместо трёх месяцев;
// - GetRank(PChar, N) заменён на sti(pchar.rank) + N: в CSP GetRank сама прибавляет
//   ранг ГГ внутри, и при передаче PChar он считался дважды;
// - двери домов ищутся по ИМЕНИ локатора, а не по номеру reload.lN.

#define UD_CHEST_MONEY		50000	// в схроне у маяка

// Gregg: найти ключ двери по ИМЕНИ локатора. Номера reload.lN в разных городах
// расходятся - в К:ПС дом Тилльманна висел на l27, а в CSP тот же houseF2 сидит на l22.
// Цепляться за номер значит однажды перенацелить чужой дом.
string UD_FindReloadKeyByName(string sLocation, string sLocatorName)
{
	int loc = FindLocation(sLocation);
	if (loc < 0) return "";
	aref ar, el;
	makearef(ar, Locations[loc].reload);
	// Gregg: счётчик локальный, а не общий i
	int iDoor;
	for (iDoor = 0; iDoor < GetAttributesNum(ar); iDoor++)
	{
		el = GetAttributeN(ar, iDoor);
		if (!CheckAttribute(el, "name")) continue;
		if (el.name == sLocatorName) return GetAttributeName(el);
	}
	return "";
}

// Gregg: запись журнала одной строкой - и сразу род ГГ. Функции в .txt не работают,
// поэтому в тексте записи стоят плейсхолдеры, а значения приходят отсюда.
// AddQuestUserData цепляется к ПОСЛЕДНЕЙ добавленной записи (quests.c:404 берёт
// GetAttributesNum(...)-1), а не к квесту целиком, поэтому подстановку повторяем
// после КАЖДОЙ записи - один вызов при старте оставил бы в поздних записях дырку.
// sSex - обычное окончание, sHe и sHim - о ГГ в третьем лице: так о нём пишет
// Тилльманн в перехваченном письме.
void UD_Record(string sNum)
{
	AddQuestRecord("UneasyDelivery", sNum);
	AddQuestUserData("UneasyDelivery", "sSex", GetSexPhrase("", "а"));
	AddQuestUserData("UneasyDelivery", "sHe", GetSexPhrase("он", "она"));
	AddQuestUserData("UneasyDelivery", "sHim", GetSexPhrase("него", "неё"));
}

// Gregg: перенацелить дверь-заглушку на квестовый дом и обратно.
// Суточный рандомайзер дверей (reload.c) пишет disable = rand(1) каждой двери, у которой
// в .go есть подстрока «Common». Как только мы меняем .go на свой дом, подстрока пропадает,
// а выпавшая до этого единица остаётся в двери навсегда - вход мёртв и пешком, и быстрым
// переходом. Поэтому при открытии disable снимается явно. Обжигались на конторе ГВИК.
void UD_SetHouseDoor(string sTown, string sLocatorName, string sHouse, string sStub, string sLabel, bool bOpen)
{
	int loc = FindLocation(sTown);
	if (loc < 0) return;
	string sKey = UD_FindReloadKeyByName(sTown, sLocatorName);
	if (sKey == "") return;
	if (bOpen)
	{
		if (FindLocation(sHouse) < 0) return;
		Locations[loc].reload.(sKey).go = sHouse;
		// Gregg: своя подпись над дверью, иначе дом не отличить от соседних заглушек
		Locations[loc].reload.(sKey).label = sLabel;
		if (CheckAttribute(&Locations[loc], "reload." + sKey + ".disable")) DeleteAttribute(&Locations[loc], "reload." + sKey + ".disable");
		if (CheckAttribute(&Locations[loc], "reload." + sKey + ".canEnter")) DeleteAttribute(&Locations[loc], "reload." + sKey + ".canEnter");
		return;
	}
	Locations[loc].reload.(sKey).go = sStub;
	Locations[loc].reload.(sKey).label = "House";
}

// Gregg: доводка уже созданной локации. Правки в фабрике действуют только на новую
// игру: массив Locations целиком лежит в сейве, и на старой партии дом приезжает
// оттуда таким, каким был создан. Поэтому обе вещи делаем в рантайме:
// - id.label: именно ИЗ НЕГО рисуется подпись у двери в городе (LandInterface.c:2195
//   берёт label ЦЕЛЕВОЙ локации, а не двери - на этом я и ошибся сначала);
// - MustSetReloadBack: по нему CreateInsideHouseEncounters заводит в доме постороннего.
void UD_TuneHouse(string sHouse, string sLabel)
{
	int loc = FindLocation(sHouse);
	if (loc < 0) return;
	Locations[loc].id.label = sLabel;
	if (CheckAttribute(&Locations[loc], "MustSetReloadBack")) DeleteAttribute(&Locations[loc], "MustSetReloadBack");
}

void UD_OpenTillmanHouse(bool bOpen)
{
	UD_TuneHouse("HouseRichardTillman", "UD_TillmanHouse");
	UD_SetHouseDoor("Bridgetown_town", "houseF2", "HouseRichardTillman", "CommonRoom_MH3", "UD_TillmanHouse", bOpen);
}

void UD_OpenPrenolHouse(bool bOpen)
{
	UD_TuneHouse("HouseJacquePrenol", "UD_PrenolHouse");
	UD_SetHouseDoor("PortPax_town", "houseSp2", "HouseJacquePrenol", "CommonStoneHouse", "UD_PrenolHouse", bOpen);
}

// Gregg: общая часть двух входов. Запись в журнал НЕ здесь: у пролога и у таверны
// разные завязки, и прологовый текст про груз какао для второго пути - вранье.
void UD_Setup()
{
	SetQuestHeader("UneasyDelivery");

	PChar.UneasyDelivery = "1";

	ref chr = GetCharacter(NPC_GenerateCharacter("Richard Tillman", "trader_15", "man", "man", 1, ENGLAND, -1, true));
	chr.name = iTxt("UD_TillmanName");
	chr.lastname = iTxt("UD_TillmanLastName");
	chr.Dialog.FileName = "Quest\KPS\UneasyDelivery\UD_Tillman.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_QuestMan";
	LAi_SetOwnerType(chr);
	LAi_SetImmortal(chr, true);

	ChangeCharacterAddressGroup(chr, "HouseRichardTillman", "barmen", "stay");
	UD_OpenTillmanHouse(true);
}

// Gregg: вход из альтернативного пролога - ГГ приезжает с грузом какао убитого Толстяка
void UneasyDeliveryStart()
{
	UD_Setup();
	UD_Record("1");
}

// Gregg: вход БЕЗ альтернативного пролога - через трактирщика Бриджтауна.
// В прологе ГГ приезжает с грузом какао от убитого Томми Толстяка, и первые узлы
// диалога Тилльманна построены на этом. Здесь ничего этого нет: капитана просто
// прислал трактирщик, поэтому разговор начинается сразу с поручения о фарфоре.
// Флаг NoCocoa снимает отсылки к Толстяку и дальше, в диалоге Преноля.
void UneasyDeliveryStartFromTavern()
{
	UD_Setup();
	UD_Record("18");
	PChar.UneasyDelivery.NoCocoa = true;
	// Gregg: через промежуточный ref. Присваивание атрибуту у РЕЗУЛЬТАТА функции
	// (CharacterFromID(...).Dialog.CurrentNode = ...) Storm молча не выполняет.
	ref rTillman = CharacterFromID("Richard Tillman");
	rTillman.Dialog.CurrentNode = "30";
}

void UneasyDeliveryCancel()
{
	PChar.UneasyDelivery = "END";
	PChar.UneasyDelivery.Cancel = true;
	UD_OpenTillmanHouse(false);

	if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa")) UD_Record("20");
	else UD_Record("2");
	CloseQuestHeader("UneasyDelivery");

	Characters[GetCharacterIndex("Richard Tillman")].LifeDay = 0;
}

void UneasyDeliveryAgree()
{
	TakeNItems(PChar, "RichardTillmanNotice", 1);
	TakeNItems(PChar, "UneasyDeliveryPorcelain", 25);

	PChar.UneasyDelivery = "2";
	// Gregg: запись 3 рассказывает про жалкие 750 за какао - для входа через таверну
	// этого не было, там своя, 19-я
	if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa")) UD_Record("19");
	else UD_Record("3");

	UD_OpenPrenolHouse(true);

	ref chr = GetCharacter(NPC_GenerateCharacter("Jacque Prenol", "trader_14", "man", "man", 1, FRANCE, -1, true));
	chr.name = iTxt("UD_PrenolName");
	chr.lastname = iTxt("UD_PrenolLastName");
	chr.Dialog.FileName = "Quest\KPS\UneasyDelivery\UD_Prenol.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_QuestMan";
	chr.nonTable = true;
	LAi_SetOwnerType(chr);
	LAi_SetImmortal(chr, true);

	ChangeCharacterAddressGroup(chr, "HouseJacquePrenol", "barmen", "stay");
}

void UneasyDeliveryTimer()
{
	PChar.UneasyDelivery = "3";
	UD_Record("4");

	SetTimerFunction("UneasyDeliveryTimerEnd", 0, 0, 6);
}

void UneasyDeliveryTimerEnd(string qName)
{
	PChar.UneasyDelivery = "4";

	PChar.quest.UneasyDeliveryDialogWithPrenol.win_condition.l1 = "location";
	PChar.quest.UneasyDeliveryDialogWithPrenol.win_condition.l1.location = "HouseJacquePrenol";
	PChar.quest.UneasyDeliveryDialogWithPrenol.function = "UneasyDeliveryDialogWithPrenol";
}

void UneasyDeliveryDialogWithPrenol(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Jacque Prenol");
	chr.Dialog.CurrentNode = "10";

	ChangeCharacterAddressGroup(chr, "HouseJacquePrenol", "goto", "goto4");
	ChangeCharacterAddressGroup(PChar, "HouseJacquePrenol", "goto", "goto1");
	SetDialogCharacter2Character(PChar, chr, 1.0, true);

	// Gregg: ранг фиксированный, а не от ранга ГГ. Квест ранний, и привязка к игроку
	// давала бы на 30-м ранге толпу 33-го - при том что дерёшься с ними ты в начале игры.
	// Ориентир - Томми Толстяк из пролога (ранг 3, 80/100/125 HP): эти чуть крепче,
	// но их трое, поэтому поштучно накидываем немного.
	int iRank = 5;
	int iHP = 110;
	if (MOD_SKILL_ENEMY_RATE == SAILOR_DIFFICULTY) iHP = 90;
	if (MOD_SKILL_ENEMY_RATE == ADMIRAL_DIFFICULTY) iHP = 135;
	// Gregg: счётчик локальный, а не общий i - внутри цикла зовутся чужие функции
	int iMan;
	string sModel = "";
	for (iMan = 1; iMan <= 3; iMan++)
	{
		if (iMan == 1) sModel = "citiz_2";
		if (iMan == 2) sModel = "citiz_3";
		if (iMan == 3) sModel = "citiz_8";
		chr = GetCharacter(NPC_GenerateCharacter("Jacque Prenol Killer " + iMan, sModel, "man", "man", iRank, PIRATE, 0, true));
		ChangeCharacterAddressGroup(chr, "HouseJacquePrenol", "reload", "reload2");
		chr.chr_ai.hp = iHP;
		chr.chr_ai.hp_max = iHP;
		SetFoodToCharacter(chr, 5, 30);
		LAi_SetStayType(chr);
	}
}

void UneasyDeliveryPrenolExit()
{
	LAi_SetActorType(CharacterFromID("Jacque Prenol"));
	// Gregg: таймаут конечный, а не -1: при провале маршрута LAi_tmpl_runto_Teleport
	// срабатывает только при timeout >= 0, иначе шаблон встаёт в "falure" навсегда
	LAi_ActorRunToLocation(CharacterFromID("Jacque Prenol"), "reload", "reload1", "none", "", "", "", 8.0);

	DoQuestFunctionDelay("UneasyDeliveryBattleWithPirates", 3.0);
}

void UneasyDeliveryBattleWithPirates(string _tmp)
{
	LAi_SetPlayerType(PChar);

	int iMan;
	ref chr;
	for (iMan = 1; iMan <= 3; iMan++)
	{
		chr = CharacterFromID("Jacque Prenol Killer " + iMan);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.UneasyDeliveryBattleWithPiratesWinner.win_condition.l1 = "NPC_Death";
	PChar.quest.UneasyDeliveryBattleWithPiratesWinner.win_condition.l1.character = "Jacque Prenol Killer 1";
	PChar.quest.UneasyDeliveryBattleWithPiratesWinner.win_condition.l2 = "NPC_Death";
	PChar.quest.UneasyDeliveryBattleWithPiratesWinner.win_condition.l2.character = "Jacque Prenol Killer 2";
	PChar.quest.UneasyDeliveryBattleWithPiratesWinner.win_condition.l3 = "NPC_Death";
	PChar.quest.UneasyDeliveryBattleWithPiratesWinner.win_condition.l3.character = "Jacque Prenol Killer 3";
	PChar.quest.UneasyDeliveryBattleWithPiratesWinner.function = "UneasyDeliveryBattleWithPiratesWinner";
}

void UneasyDeliveryBattleWithPiratesWinner(string qName)
{
	chrDisableReloadToLocation = false;

	UD_Record("5");
	PChar.UneasyDelivery = "5";

	FreeSitLocator("PortPax_church", "sit1");
	LAi_SetSitType(CharacterFromID("Jacque Prenol"));
	ChangeCharacterAddressGroup(CharacterFromID("Jacque Prenol"), "PortPax_church", "sit", "sit1");

	Characters[GetCharacterIndex("Jacque Prenol")].Dialog.CurrentNode = "11";
}

void UneasyDeliveryJacquePrenolBackToHouse()
{
	ChangeCharacterAddressGroup(CharacterFromID("Jacque Prenol"), "PortPax_church", "goto", "goto2");
	LAi_SetActorType(CharacterFromID("Jacque Prenol"));
	LAi_ActorGoToLocation(CharacterFromID("Jacque Prenol"), "reload", "reload1", "none", "", "", "", 8.0);

	LAi_SetPlayerType(PChar);
	UD_Record("6");
	PChar.UneasyDelivery = "6";

	SetQuestFunction("UneasyDeliveryShonNeedKill", "UneasyDeliveryShonNeedKill", "efl", PChar.location);
}

void UneasyDeliveryShonNeedKill(string qName)
{
	LAi_SetOwnerType(CharacterFromID("Jacque Prenol"));
	ChangeCharacterAddressGroup(CharacterFromID("Jacque Prenol"), "HouseJacquePrenol", "barmen", "stay");

	ref chr = GetCharacter(NPC_GenerateCharacter("Shon Stablovski", "trader_9", "man", "man", 1, ENGLAND, -1, true));
	chr.name = iTxt("UD_ShonName");
	chr.lastname = iTxt("UD_ShonLastName");
	chr.greeting = "Gr_QuestMan";
	chr.Dialog.FileName = "Quest\KPS\UneasyDelivery\UD_Shon.c";
	chr.Dialog.CurrentNode = "First time";
	LAi_SetSitType(chr);
	LAi_SetImmortal(chr, true);

	FreeSitLocator("SentJons_tavern", "sit_base2");
	ChangeCharacterAddressGroup(chr, "SentJons_tavern", "sit", "sit_base2");
}

void UneasyDeliveryAntiguaDialogExit()
{
	PChar.UneasyDelivery = "7";
	ref chr = CharacterFromID("Shon Stablovski");
	chr.Dialog.CurrentNode = "1";
	// Gregg: Шон ждёт в таверне Сент-Джонса, а таверна не пустая - помечаем значком.
	SetQuestMarkToCharacter(chr, true);
}

void UneasyDeliveryShonFirstSitDialog()
{
	ref chr = CharacterFromID("Shon Stablovski");
	chr.Dialog.CurrentNode = "3";

	FreeSitLocator("SentJons_tavern", "sit_front2");
	ChangeCharacterAddressGroup(PChar, "SentJons_tavern", "sit", "sit_front2");
	LAi_SetSitType(PChar);
	LAi_SetActorType(chr);
	LAi_ActorSetSitMode(chr);
	LAi_ActorDialogNow(chr, PChar, "", -1);
}

void UneasyDeliveryShonDialogExit()
{
	PChar.UneasyDelivery = "8";

	ChangeCharacterAddressGroup(PChar, "SentJons_tavern", "goto", "goto4");
	LAi_SetPlayerType(PChar);

	ref chrShon = CharacterFromID("Shon Stablovski");
	LAi_SetSitType(chrShon);
	// Gregg: задание получено, до возвращения с деньгами Шон не нужен - значок снимаем.
	SetQuestMarkToCharacter(chrShon, false);

	UD_Record("7");

	// Gregg: срок «зайти через две недели» (в К:ПС - три месяца) остался только словами
	// в репликах Шона. Механически он ничего не гейтит: продвигает квест не время,
	// а найденная в доме Преноля записка, поэтому таймер и флаг здесь не заводятся.

	ChangeCharacterAddress(CharacterFromID("Jacque Prenol"), "None", "");
	SetQuestBoxItems("HouseJacquePrenol", "RichardTillmanNotice", 1);

	PChar.quest.UneasyDeliveryFindNotice.win_condition.l1 = "item";
	PChar.quest.UneasyDeliveryFindNotice.win_condition.l1.item = "RichardTillmanNotice";
	PChar.quest.UneasyDeliveryFindNotice.function = "UneasyDeliveryFindNotice";
}

void UneasyDeliveryFindNotice(string qName)
{
	ClearQuestBoxStay("HouseJacquePrenol");

	// Gregg: в тексте письма Тилльманн ссылается на убитого Толстяка - для входа
	// через таверну этого не было, там своя запись, 21-я
	if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa")) UD_Record("21");
	else UD_Record("8");
	PChar.UneasyDelivery = "9";

	AddQuestUserData("UneasyDelivery", "sMainCharacterName", GetHeroLastName());

	PChar.quest.UneasyDeliveryPiratesKillTillman2.win_condition.l1 = "location";
	PChar.quest.UneasyDeliveryPiratesKillTillman2.win_condition.l1.location = "HouseRichardTillman";
	PChar.quest.UneasyDeliveryPiratesKillTillman2.function = "UneasyDeliveryPiratesKillTillman2";

	UD_OpenPrenolHouse(false);
}

void UneasyDeliveryPiratesKillTillman2(string qName)
{
	LAi_SetActorType(PChar);
	DoQuestFunctionDelay("UneasyDeliveryPiratesKillTillman", 0.1);
}

void UneasyDeliveryPiratesKillTillman(string _tmp)
{
	float x, y, z;
	GetCharacterPos(PChar, &x, &y, &z);

	y += 1.5;
	z += 8.0;
	x -= 1;
	locCameraToPos(x, y, z, false);

	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	chrDisableReloadToLocation = true;

	ref chr;
	// Gregg: тот же фиксированный уровень, что и у засады в доме Преноля
	int iRank = 5;
	int iHP = 110;
	if (MOD_SKILL_ENEMY_RATE == SAILOR_DIFFICULTY) iHP = 90;
	if (MOD_SKILL_ENEMY_RATE == ADMIRAL_DIFFICULTY) iHP = 135;

	int iMan;
	for (iMan = 1; iMan <= 3; iMan++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("Richard Tillman Killer " + iMan, "pirate_" + iMan, "man", "man", iRank, PIRATE, 0, true));
		ChangeCharacterAddressGroup(chr, "HouseRichardTillman", "goto", "goto" + iMan);
		chr.chr_ai.hp = iHP;
		chr.chr_ai.hp_max = iHP;
		SetFoodToCharacter(chr, 5, 30);
		LAi_SetActorType(chr);
		LAi_ActorAttack(chr, CharacterFromID("Richard Tillman"), "");
	}

	LAi_SetHP(CharacterFromID("Richard Tillman"), 70.0, 70.0);
	LAi_SetImmortal(CharacterFromID("Richard Tillman"), false);

	PChar.quest.UneasyDeliveryTillmanDead.win_condition.l1 = "NPC_Death";
	PChar.quest.UneasyDeliveryTillmanDead.win_condition.l1.character = "Richard Tillman";
	PChar.quest.UneasyDeliveryTillmanDead.function = "UneasyDeliveryTillmanDead";
}

void UneasyDeliveryTillmanDead(string qName)
{
	InterfaceStates.Buttons.Save.enable = true;
	EndQuestMovie();

	LAi_Fade("", "");
	locCameraFollow();

	LAi_SetPlayerType(PChar);

	ref chr;
	int iMan;
	for (iMan = 1; iMan <= 3; iMan++)
	{
		chr = CharacterFromID("Richard Tillman Killer " + iMan);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.UneasyDeliveryTillmanKillersDead.win_condition.l1 = "NPC_Death";
	PChar.quest.UneasyDeliveryTillmanKillersDead.win_condition.l1.character = "Richard Tillman Killer 1";
	PChar.quest.UneasyDeliveryTillmanKillersDead.win_condition.l2 = "NPC_Death";
	PChar.quest.UneasyDeliveryTillmanKillersDead.win_condition.l2.character = "Richard Tillman Killer 2";
	PChar.quest.UneasyDeliveryTillmanKillersDead.win_condition.l3 = "NPC_Death";
	PChar.quest.UneasyDeliveryTillmanKillersDead.win_condition.l3.character = "Richard Tillman Killer 3";
	PChar.quest.UneasyDeliveryTillmanKillersDead.function = "UneasyDeliveryTillmanKillersDead";
}

void UneasyDeliveryTillmanKillersDead(string qName)
{
	// Gregg: в К:ПС письмо лежало на трупе первого головореза и требовалось обыском,
	// но этот кусок там был закомментирован и записка выдавалась сразу. Оставлено как есть.
	UneasyDeliveryFindTillmanKillersNotice("");
	TakeNItems(PChar, "TillmanKillersNotice", 1);
}

void UneasyDeliveryFindTillmanKillersNotice(string qName)
{
	chrDisableReloadToLocation = false;

	UD_Record("9");
	PChar.UneasyDelivery = "10";

	UD_OpenTillmanHouse(false);
}

void UneasyDeliveryShonReadNotice()
{
	UD_Record("10");
	PChar.UneasyDelivery = "11";

	ref chr = GetCharacter(NPC_GenerateCharacter("Charly Grand Pipiska", "indsair2", "man", "man", 1, PIRATE, -1, true));
	chr.name = iTxt("UD_CharlyName");
	chr.lastname = iTxt("UD_CharlyLastName");
	chr.Dialog.FileName = "Quest\KPS\UneasyDelivery\UD_Charly.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_PearlIndian";
	LAi_SetImmortal(chr, true);
	LAi_SetStayType(chr);
	ChangeCharacterAddressGroup(chr, "Pearl_Jungle_02", "goto", "goto12");
	Locations[FindLocation("Pearl_Jungle_02")].reload.l2.disable = 1;
	// Gregg: DisableLandEncounters в CSP нет, используем свою обёртку из Инквизиции
	HSI_SetEncounters("Pearl_Jungle_02", true);
}

void UneasyDeliveryDialogWithCharlyFailed()
{
	LAi_SetActorType(CharacterFromID("Charly Grand Pipiska"));
	LAi_ActorRunToLocation(CharacterFromID("Charly Grand Pipiska"), "reload", "reload1_back", "none", "", "", "", 8.0);

	Characters[GetCharacterIndex("Shon Stablovski")].LifeDay = 0;
	Characters[GetCharacterIndex("Charly Grand Pipiska")].LifeDay = 0;

	PChar.UneasyDelivery = "END";
	UD_Record("11");
	CloseQuestHeader("UneasyDelivery");

	Locations[FindLocation("Pearl_Jungle_02")].reload.l2.disable = 0;
	HSI_SetEncounters("Pearl_Jungle_02", false);

	PChar.quest.UneasyDeliveryPearlOpen.win_condition.l1 = "ExitFromLocation";
	PChar.quest.UneasyDeliveryPearlOpen.win_condition.l1.location = PChar.location;
	PChar.quest.UneasyDeliveryPearlOpen.function = "UneasyDeliveryPearlOpen";
}

void UneasyDeliveryPearlOpen(string qName)
{
	Locations[FindLocation("Pearl_Jungle_02")].reload.l2.disable = 0;
	HSI_SetEncounters("Pearl_Jungle_02", false);
}

void UneasyDeliveryDialogWithCharlyGood()
{
	PChar.UneasyDelivery = "12";
	UD_Record("12");

	LAi_SetActorType(CharacterFromID("Charly Grand Pipiska"));
	LAi_ActorRunToLocation(CharacterFromID("Charly Grand Pipiska"), "reload", "reload1_back", "none", "", "", "", 8.0);

	Characters[GetCharacterIndex("Charly Grand Pipiska")].LifeDay = 0;

	ref chr = GetCharacter(NPC_GenerateCharacter("Bob McCain", "citiz_9", "man", "man", 1, PIRATE, -1, true));
	chr.name = iTxt("UD_McCainName");
	chr.lastname = iTxt("UD_McCainLastName");
	chr.Dialog.FileName = "Quest\KPS\UneasyDelivery\UD_McCain.c";
	chr.Dialog.CurrentNode = "First time";
	chr.greeting = "Gr_Tavern_Mate_1";
	LAi_SetSitType(chr);
	LAi_SetImmortal(chr, true);
	FreeSitLocator("Pirates_tavern", "sit_base1");

	ChangeCharacterAddressGroup(chr, "Pirates_tavern", "sit", "sit_base1");

	PChar.quest.UneasyDeliveryPearlOpen.win_condition.l1 = "ExitFromLocation";
	PChar.quest.UneasyDeliveryPearlOpen.win_condition.l1.location = PChar.location;
	PChar.quest.UneasyDeliveryPearlOpen.function = "UneasyDeliveryPearlOpen";
}

void UneasyDeliveryDialogWithBermudesBarmen()
{
	PChar.UneasyDelivery = "13";
	ref chr = CharacterFromID("Bob McCain");
	chr.Dialog.CurrentNode = "1";
	// Gregg: до разговора с барменом пьянчуга только зазывал выпить, а теперь он -
	// единственная ниточка к Пренолю. Значок снимается в UneasyDeliveryDialogWithMcCainEnd,
	// когда место встречи уже куплено; при отказе платить он остаётся висеть.
	SetQuestMarkToCharacter(chr, true);
}

void UneasyDeliveryDialogWithMcCain()
{
	ref chr = CharacterFromID("Bob McCain");
	chr.Dialog.CurrentNode = "4";

	FreeSitLocator("Pirates_tavern", "sit_front1");
	ChangeCharacterAddressGroup(PChar, "Pirates_tavern", "sit", "sit_front1");
	LAi_SetSitType(PChar);
	LAi_SetActorType(chr);
	LAi_ActorSetSitMode(chr);
	LAi_ActorDialogNow(chr, PChar, "", -1);
}

void UneasyDeliveryDialogWithMcCainEnd()
{
	UD_Record("13");
	PChar.UneasyDelivery = "14";

	ChangeCharacterAddressGroup(PChar, "Pirates_tavern", "goto", "goto2");
	LAi_SetPlayerType(PChar);

	ref chrMcCain = CharacterFromID("Bob McCain");
	LAi_SetSitType(chrMcCain);
	// Gregg: место встречи куплено, больше пьянчуга ничего не даёт - значок долой.
	SetQuestMarkToCharacter(chrMcCain, false);

	chrMcCain.LifeDay = 0;

	HSI_SetEncounters("Shore_ship1", true);
	PChar.quest.UneasyDeliveryInBermudesShore2.win_condition.l1 = "location";
	PChar.quest.UneasyDeliveryInBermudesShore2.win_condition.l1.location = "Shore_ship1";
	PChar.quest.UneasyDeliveryInBermudesShore2.function = "UneasyDeliveryInBermudesShore2";
	// Gregg: страховочного таймера здесь нет намеренно. В оригинале через двое суток
	// вызывалась UneasyDeliveryReloadInBermudesShore, и она швартовала игрока к бухте
	// откуда угодно, хоть с другого архипелага. Условия выше достаточно: место названо
	// и в реплике Мак-Кейна, и в записи журнала, локация есть на карте мира, а два часа
	// ночи UneasyDeliveryInBermudesShore2 выставляет сама, когда игрок придёт.
}

void UneasyDeliveryInBermudesShore2(string qName)
{
	SetCurrentTime(2, 10);
	RecalculateJumpTable();
	RefreshWeather();
	RefreshLandTime();

	LAi_SetActorType(PChar);
	DoQuestFunctionDelay("UneasyDeliveryInBermudesShore", 0.1);
}

void UneasyDeliveryInBermudesShore(string _tmp)
{
	chrDisableReloadToLocation = true;
	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, 30, 5, -10, false);

	ChangeCharacterAddressGroup(PChar, "Shore_ship1", "goto", "quest");

	LAi_SetActorType(CharacterFromID("Jacque Prenol"));
	ChangeCharacterAddressGroup(CharacterFromID("Jacque Prenol"), "Shore_ship1", "goto", "goto5");

	ref chr;
	// Gregg: финал квеста - эти двое заметно крепче тройки головорезов, но тоже
	// фиксированы: см. пояснение в UneasyDeliveryDialogWithPrenol.
	int iRank = 8;
	int iHP = 160;
	if (MOD_SKILL_ENEMY_RATE == SAILOR_DIFFICULTY) iHP = 130;
	if (MOD_SKILL_ENEMY_RATE == ADMIRAL_DIFFICULTY) iHP = 195;

	chr = GetCharacter(NPC_GenerateCharacter("Prenol Killer 1", "officer_17", "man", "man", iRank, PIRATE, 0, true));
	chr.name = iTxt("UD_KillerJohnName");
	chr.lastname = "";
	chr.greeting = "Gr_ArenaMember";
	chr.Dialog.FileName = "Quest\KPS\UneasyDelivery\UD_PrenolKiller.c";
	chr.Dialog.CurrentNode = "1";
	FantomMakeCoolFighter(chr, iRank, 10, 10, "", "", 10);
	QuestDuelEquipCharacterSkills(chr, true);
	LAi_NPC_Equip(chr, sti(chr.rank), true, true);
	LAi_SetActorType(chr);
	ChangeCharacterAddressGroup(chr, "Shore_ship1", "goto", "goto6");
	chr.chr_ai.hp = iHP;
	chr.chr_ai.hp_max = iHP;
	SetFoodToCharacter(chr, 5, 30);

	chr = GetCharacter(NPC_GenerateCharacter("Prenol Killer 2", "officer_7", "man", "man", iRank, PIRATE, 0, true));
	chr.name = iTxt("UD_KillerNilsName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\UneasyDelivery\UD_PrenolKiller.c";
	chr.Dialog.CurrentNode = "6";
	chr.greeting = "Gr_ArenaMember";
	FantomMakeCoolFighter(chr, iRank, 10, 10, "", "", 10);
	QuestDuelEquipCharacterSkills(chr, true);
	LAi_NPC_Equip(chr, sti(chr.rank), true, true);
	SetFoodToCharacter(chr, 5, 30);
	LAi_SetActorType(chr);
	ChangeCharacterAddressGroup(chr, "Shore_ship1", "goto", "goto7");
	chr.chr_ai.hp = iHP;
	chr.chr_ai.hp_max = iHP;

	LAi_ActorTurnToCharacter(CharacterFromID("Jacque Prenol"), CharacterFromID("Prenol Killer 1"));
	LAi_ActorTurnToCharacter(CharacterFromID("Prenol Killer 1"), CharacterFromID("Jacque Prenol"));
	LAi_ActorTurnToCharacter(CharacterFromID("Prenol Killer 2"), CharacterFromID("Jacque Prenol"));

	DoQuestFunctionDelay("UneasyDeliveryDialogKillersWithPrenol", 2.5);
}

void UneasyDeliveryDialogKillersWithPrenol(string _tmp)
{
	SetDialogCharacter2Character(PChar, CharacterFromID("Prenol Killer 1"), 0.001, true);
}

void UneasyDeliveryPrenolKill()
{
	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, 30, 5, -10, false);

	LAi_SetHP(CharacterFromID("Jacque Prenol"), 25.0, 25.0);
	LAi_SetImmortal(CharacterFromID("Jacque Prenol"), false);

	LAi_SetActorType(CharacterFromID("Prenol Killer 1"));
	LAi_ActorAttack(CharacterFromID("Prenol Killer 1"), CharacterFromID("Jacque Prenol"), "");

	LAi_SetStayType(CharacterFromID("Jacque Prenol"));

	PChar.quest.UneasyDeliveryPrenolDead.win_condition.l1 = "NPC_Death";
	PChar.quest.UneasyDeliveryPrenolDead.win_condition.l1.character = "Jacque Prenol";
	PChar.quest.UneasyDeliveryPrenolDead.function = "UneasyDeliveryPrenolDead";
}

void UneasyDeliveryPrenolDead(string qName)
{
	LAi_ActorTurnToCharacter(CharacterFromID("Prenol Killer 1"), CharacterFromID("Prenol Killer 2"));
	SetDialogCharacter2Character(PChar, CharacterFromID("Prenol Killer 2"), 0.1, true);
}

void UneasyDeliveryBattleWithPrenolKillers()
{
	locCameraFollow();
	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;

	LAi_SetWarriorType(CharacterFromID("Prenol Killer 1"));
	LAi_group_MoveCharacter(CharacterFromID("Prenol Killer 1"), "EnemyFight");

	LAi_SetWarriorType(CharacterFromID("Prenol Killer 2"));
	LAi_group_MoveCharacter(CharacterFromID("Prenol Killer 2"), "EnemyFight");

	LAi_SetPlayerType(PChar);

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.UneasyDeliveryBattleWithPrenolKillersWinner.win_condition.l1 = "NPC_Death";
	PChar.quest.UneasyDeliveryBattleWithPrenolKillersWinner.win_condition.l1.character = "Prenol Killer 1";
	PChar.quest.UneasyDeliveryBattleWithPrenolKillersWinner.win_condition.l2 = "NPC_Death";
	PChar.quest.UneasyDeliveryBattleWithPrenolKillersWinner.win_condition.l2.character = "Prenol Killer 2";
	PChar.quest.UneasyDeliveryBattleWithPrenolKillersWinner.function = "UneasyDeliveryBattleWithPrenolKillersWinner";
}

void UneasyDeliveryBattleWithPrenolKillersWinner(string qName)
{
	UD_Record("14");
	PChar.UneasyDelivery = "15";

	LAi_SetPlayerType(PChar);

	ref chest = ItemsFromID("PrenolChest");
	// Gregg: каталог предметов приезжает из сейва позиционно, поэтому правки
	// определения на идущей партии не подхватятся - доводим здесь.
	// groupID делал сундук взламываемым (просил отмычку), stat1 - не та модель.
	if (CheckAttribute(chest, "groupID")) DeleteAttribute(chest, "groupID");
	chest.folder = "items";
	chest.model = "Chest2";
	chest.picIndex = 1;
	chest.picTexture = "ITEMS_4";
	chest.Weight = 1.0;
	chest.shown = true;
	chest.startLocation = "Mayak9";
	// Gregg: item_chest дописан в модель маяка. В стоковой у CSP группа item содержала
	// ровно один item1, и занимать его под квест не хотелось: он раздаёт случайные находки.
	// Точка стоит на патче ходьбы в полутора метрах от item1, посажена на грунт.
	// Раскладку .gm восстановил по эталонному писателю Blender-аддона
	// (Engine\CSP	oolslender-gm-export): между блоком NAMES и секцией текстур идёт
	// ТАБЛИЦА СМЕЩЕНИЙ ИМЁН, names * int32. Первая попытка её не расширила - движок читал
	// на запись больше, залезал в текстуры, и локаторы локации разъезжались.
	// Модель вне гита (.gitignore: *RESOURCE/MODELS/), оригинал сохранён отдельно.
	chest.startLocator = "item_chest";

	// Gregg: карта должна быть обычным квестовым предметом, а не рабочей картой
	// архипелага: с MAPS_ITEM_TYPE её можно экипировать и открыть. Определение
	// поправлено, но каталог предметов приезжает из сейва позиционно, поэтому
	// на уже идущей партии тип чиним здесь.
	ref rMap = ItemsFromID("PrenolMap");
	if (CheckAttribute(rMap, "groupID")) DeleteAttribute(rMap, "groupID");
	TakeNItems(PChar, "PrenolMap", 1);

	// Gregg: подобрал схрон - деньги и запись в журнал. В К:ПС этот вызов был
	// закомментирован, а звать UneasyDeliveryFindPrenolMoney оказалось некому:
	// игрок уносил сундук в трюме, и этап не закрывался.
	PChar.quest.UneasyDeliveryFindPrenolMoney.win_condition.l1 = "item";
	PChar.quest.UneasyDeliveryFindPrenolMoney.win_condition.l1.item = "PrenolChest";
	PChar.quest.UneasyDeliveryFindPrenolMoney.function = "UneasyDeliveryFindPrenolMoney";

	UneasyDeliveryBattleWithPrenolKillersWinner2("");
}

void UneasyDeliveryBattleWithPrenolKillersWinner2(string qName)
{
	chrDisableReloadToLocation = false;
	HSI_SetEncounters("Shore_ship1", false);
}

void UneasyDeliveryFindPrenolMoney(string qName)
{
	UD_Record("15");
	PChar.UneasyDelivery = "16";
	// Gregg: клад найден, теперь снова к Шону - за расчётом.
	SetQuestMarkToCharacter(CharacterFromID("Shon Stablovski"), true);

	TakeNItems(PChar, "PrenolMap", -1);
	TakeNItems(PChar, "PrenolChest", -1);

	// Gregg: в К:ПС в схроне лежало 300 000. Квест ранний, цепляется сразу за пролог,
	// поэтому сумма урезана до UD_CHEST_MONEY - см. дефайн в шапке.
	AddMoneyToCharacter(PChar, UD_CHEST_MONEY);
}

// Gregg: подарок Шона за честный расчёт. В К:ПС это была намертво прошитая blade23
// «Бретта» - для раннего квеста слишком щедро и никак не связано с тем, чем игрок дерётся.
// Даём клинок или ствол качества good, подобранный по САМОМУ СИЛЬНОМУ оружейному навыку:
// коллекционер не стал бы совать абордажнику рапиру. Внутри каждой ветки - случайный
// из подходящих, чтобы награда не была одинаковой у всех.
void UD_GiveShonReward()
{
	int iLight = GetSummonSkillFromName(PChar, SKILL_F_LIGHT);
	int iMedium = GetSummonSkillFromName(PChar, SKILL_FENCING);
	int iHeavy = GetSummonSkillFromName(PChar, SKILL_F_HEAVY);
	int iPistol = GetSummonSkillFromName(PChar, SKILL_PISTOL);

	// Gregg: «лестницей», а не через сравнения в одном условии - смешивать && и ||
	// в одном if нельзя, а вложенность тут только запутает
	string sKind = "light";
	int iBest = iLight;
	if (iMedium > iBest) { iBest = iMedium; sKind = "medium"; }
	if (iHeavy > iBest) { iBest = iHeavy; sKind = "heavy"; }
	if (iPistol > iBest) { iBest = iPistol; sKind = "pistol"; }

	string sItem = "blade22";
	int iRoll = rand(4);
	if (sKind == "light")
	{
		if (iRoll == 0) sItem = "blade19";
		if (iRoll == 1) sItem = "blade22";
		if (iRoll == 2) sItem = "blade36";
		if (iRoll == 3) sItem = "blade05";
		if (iRoll == 4) sItem = "blade015";
	}
	if (sKind == "medium")
	{
		if (iRoll == 0) sItem = "blade31";
		if (iRoll == 1) sItem = "blade34";
		if (iRoll == 2) sItem = "blade40";
		if (iRoll == 3) sItem = "blade46";
		if (iRoll == 4) sItem = "blade06";
	}
	if (sKind == "heavy")
	{
		if (iRoll == 0) sItem = "blade01";
		if (iRoll == 1) sItem = "blade15";
		if (iRoll == 2) sItem = "blade010";
		if (iRoll == 3) sItem = "blade21";
		if (iRoll == 4) sItem = "topor2";
	}
	if (sKind == "pistol")
	{
		if (iRoll == 0) sItem = "pistol11";
		if (iRoll == 1) sItem = "pistol6";
		if (iRoll == 2) sItem = "mushket_Temppo";
		if (iRoll == 3) sItem = "mushket_SHZ";
		if (iRoll == 4) sItem = "flint_mushketon";
	}

	// Gregg: диалог должен говорить о том, что реально дали, - клинок или ствол
	PChar.UneasyDelivery.RewardGun = false;
	if (sKind == "pistol") PChar.UneasyDelivery.RewardGun = true;

	TakeNItems(PChar, sItem, 1);
}

void UneasyDeliveryShonCancelMoney()
{
	UD_Record("16");
	CloseQuestHeader("UneasyDelivery");
	pchar.questdata.UneasyDelivery = 1;
	UnlockAchievement("UneasyDelivery_quest", 3);
	PChar.UneasyDelivery = "END";

	ChangeCharacterReputation(PChar, -20);
	SetQuestMarkToCharacter(CharacterFromID("Shon Stablovski"), false);
	Characters[GetCharacterIndex("Shon Stablovski")].LifeDay = 0;
}

void UneasyDeliveryShonYesMoney()
{
	UD_Record("17");
	CloseQuestHeader("UneasyDelivery");
	pchar.questdata.UneasyDelivery = 1;
	UnlockAchievement("UneasyDelivery_quest", 3);
	PChar.UneasyDelivery = "END";

	// Gregg: деньги списывает и клинок дарит САМ диалог Шона (узлы 25 и 27) -
	// повторять это здесь значит списать дважды. Подарок, кстати, не испанская
	// рапира из описаний прохождения, а blade23 «Бретта» - так в коде оригинала.

	ChangeCharacterReputation(PChar, 10);
	SetQuestMarkToCharacter(CharacterFromID("Shon Stablovski"), false);
	Characters[GetCharacterIndex("Shon Stablovski")].LifeDay = 0;
}
// <== Gregg: «Товар для Ричарда Тилльманна»
