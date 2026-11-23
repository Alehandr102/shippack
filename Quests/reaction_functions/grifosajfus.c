// Gregg: ==> «Предательство Грифо Сайфуса» (перенос из К:ПС, глава 3)
//
// Источник: К:ПС, PROGRAM\lugger\MainLine.c, раздел «3. ПРЕДАТЕЛЬСТВО ГРИФО САЙФУСА».
// Имена функций сохранены как в оригинале, чтобы можно было сверяться с источником.
//
// Маршрут: пиратская таверна Бермуд (весть о казни Сайфуса) -> неделя ожидания ->
// шебека «Хамелеон» у Пуэрто-Рико, захват или потопление -> обратно в таверну ->
// неделя ожидания -> торговец с африканскими алмазами в джунглях Бермуд ->
// сундук с алмазами на северном берегу Теркса -> незнакомец с запиской в Пиратском
// городе -> Отто Гроберг в таверне -> шебека «Марлин» у Пуэрто-Принсипе ->
// высадка Отто -> обратно в таверну.
//
// Отличия от оригинала - в комментариях по коду. Главные:
// - «Хамелеон» перенесён с Кубы на Пуэрто-Рико: в оригинале игрок мотается на Кубу
//   дважды через весь архипелаг, а финал всё равно у Пуэрто-Принсипе;
// - уникальные типы кораблей SHIP_XEBEC_CHAMELEON и SHIP_XEBEC_MARLIN не переносятся.
//   «Хамелеон» - рядовая шебека, он и по сюжету пустышка; «Марлин» - лёгкая шебека
//   топ-генерации с поднятым до 12 фунтов калибром, по образцу «Дозорного»;
// - сроки ожидания вестей урезаны до недели (в оригинале 45 и 30 дней);
// - два входа: из альтернативного пролога и от трактирщика. Без пролога у ГГ нет
//   ни «Пираньи», ни личных счётов с Сайфусом, поэтому рамка переписана - разводятся
//   флагом NoPiranha.

#define GS_TRADER_HP		150.0	// торговец в джунглях - его надо не убить, а сбить до сдачи
#define GS_WAIT_DAYS		7		// сколько ждать вестей в таверне
// Gregg: запись журнала одной строкой - и сразу род ГГ. Функции в .txt не работают,
// поэтому в тексте записи стоят плейсхолдеры, а значения приходят отсюда.
// AddQuestUserData цепляется к ПОСЛЕДНЕЙ добавленной записи (quests.c:404), а не
// к квесту целиком, поэтому подстановку повторяем после КАЖДОЙ записи.
// sSex - обычное окончание, sSex1 - возвратная форма, sSex2 - творительный падеж
// краткого причастия: «посчитав меня убитым/убитой».
void GS_Record(string sRec)
{
	AddQuestRecord("TreacheryOfGrifoSajfus", sRec);
	AddQuestUserData("TreacheryOfGrifoSajfus", "sSex", GetSexPhrase("", "а"));
	AddQuestUserData("TreacheryOfGrifoSajfus", "sSex1", GetSexPhrase("ся", "ась"));
	AddQuestUserData("TreacheryOfGrifoSajfus", "sSex2", GetSexPhrase("ым", "ой"));
}



// Gregg: в К:ПС это DisableLandEncounters из lugger\utils.c, в CSP такой обёртки нет -
// локации гасят встречи атрибутом напрямую.
void GS_SetEncounters(string sLocation, bool bDisable)
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

// Gregg: снять Отто с офицерских слотов и из пассажиров. В оригинале этот кусок
// повторялся дважды слово в слово.
void GS_ReleaseOtto(ref Otto)
{
	if (IsOfficer(Otto))
	{
		// Gregg: счётчик локальный, а не общий i
		int iSlot;
		for (iSlot = 1; iSlot <= 3; iSlot++)
		{
			if (GetOfficersIndex(PChar, iSlot) == GetCharacterIndex(Otto.id)) RemoveOfficersIndex(PChar, iSlot);
		}
	}
	SetCharacterRemovable(Otto, true);
	RemovePassenger(PChar, Otto);
}

// Gregg: у обоих входов общая часть - заголовок квеста и его первая стадия.
// Записи в журнал разные, поэтому пишет их вызывающий.
void GS_Setup()
{
	PChar.TreacheryOfGrifoSajfus = "1";
	SetQuestHeader("TreacheryOfGrifoSajfus");
}

// Gregg: вход из альтернативного пролога - ГГ и есть тот, кого Сайфус предал у Доминики.
void TreacheryOfGrifoSajfusStart()
{
	GS_Setup();
	GS_Record("1");
}

// Gregg: вход через трактирщика для обычной игры. «Пираньи» у ГГ никогда не было,
// историю он слышит как чужую, поэтому и записи, и половина реплик - свои.
// Записи здесь нет намеренно: узел вызывает эту функцию ДО того, как трактирщик
// рассказал байку, и запись забегала вперёд, пересказывая неуслышанное. Пишем
// одну, в TreacheryOfGrifoSajfusPiratesBarmenDialogEnd, когда разговор окончен.
void TreacheryOfGrifoSajfusStartFromTavern()
{
	GS_Setup();
	PChar.TreacheryOfGrifoSajfus.NoPiranha = true;
}

void TreacheryOfGrifoSajfusPiratesBarmenDialogEnd()
{
	PChar.TreacheryOfGrifoSajfus = "2";
	if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha")) GS_Record("17");
	else GS_Record("2");

	// Gregg: 45 дней оригинала - это полтора месяца простоя ради одной реплики
	SetTimerFunction("TreacheryOfGrifoSajfusXebecNews", 0, 0, GS_WAIT_DAYS);
}

void TreacheryOfGrifoSajfusXebecNews(string qName)
{
	PChar.TreacheryOfGrifoSajfus = "3";
}

// Gregg: имя функции оставлено оригинальным для сверки с источником, но шебека
// теперь стоит у Пуэрто-Рико, а не у Кубы - см. шапку файла.
void TreacheryOfGrifoSajfusXebecNewsInCuba()
{
	PChar.TreacheryOfGrifoSajfus = "4";
	GS_Record("3");

	PChar.quest.TreacheryOfGrifoSajfusXebecInCuba.win_condition.l1 = "location";
	PChar.quest.TreacheryOfGrifoSajfusXebecInCuba.win_condition.l1.location = "PuertoRico";
	PChar.quest.TreacheryOfGrifoSajfusXebecInCuba.function = "TreacheryOfGrifoSajfusXebecInCuba";
	SetTimerFunction("TreacheryOfGrifoSajfusXebecTimer", 0, 0, 30);
}

void TreacheryOfGrifoSajfusXebecInCuba(string qName)
{
	PChar.TreacheryOfGrifoSajfus = "5";
	if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha")) GS_Record("18");
	else GS_Record("4");

	// Gregg: «Хамелеон» - обманка, приза из него не делаем: рядовая лёгкая шебека
	// без всяких прибавок. Тип тот же, что у «Марлина», и это принципиально:
	// по сюжету все три корабля с той верфи внешне неотличимы друг от друга.
	int iRank = sti(PChar.rank) + MOD_SKILL_ENEMY_RATE*2;
	ref chr = GetCharacter(NPC_GenerateCharacter("Chameleon", "off_spa_1", "man", "man", iRank, SPAIN, -1, true));
	FantomMakeCoolSailor(chr, SHIP_XEBECLIGHT, iTxt("GS_ChameleonShipName"), CANNON_TYPE_CANNON_LBS8, 45, 45, 45);
	chr.greeting = "Gr_ArenaMember";
	chr.Coastal_Captain = true;
	chr.AlwaysEnemy = true;
	LAi_NPC_Equip(chr, iRank, true, true);
	FaceMaker(chr);

	Group_FindOrCreateGroup("Chameleon");
	Group_AddCharacter("Chameleon", "Chameleon");
	Group_SetType("Chameleon", "pirate");
	Group_SetGroupCommander("Chameleon", "Chameleon");
	Group_SetAddress("Chameleon", "PuertoRico", "quest_ships", "quest_ship_3");
	Group_SetTaskAttack("Chameleon", PLAYER_GROUP);

	PChar.quest.TreacheryOfGrifoSajfusXebecCapture.win_condition.l1 = "Character_Capture";
	PChar.quest.TreacheryOfGrifoSajfusXebecCapture.win_condition.l1.character = "Chameleon";
	PChar.quest.TreacheryOfGrifoSajfusXebecCapture.function = "TreacheryOfGrifoSajfusXebecCapture";

	PChar.quest.TreacheryOfGrifoSajfusXebecSink.win_condition.l1 = "Character_sink";
	PChar.quest.TreacheryOfGrifoSajfusXebecSink.win_condition.l1.character = "Chameleon";
	PChar.quest.TreacheryOfGrifoSajfusXebecSink.function = "TreacheryOfGrifoSajfusXebecSink";
}

void TreacheryOfGrifoSajfusXebecCapture(string qName)
{
	if (Group_FindGroup("Chameleon") != -1) Group_DeleteGroup("Chameleon");

	PChar.TreacheryOfGrifoSajfus = "6";
	PChar.TreacheryOfGrifoSajfus.ChameleonSink = false;
	PChar.quest.TreacheryOfGrifoSajfusXebecSink.over = "yes";
	PChar.quest.TreacheryOfGrifoSajfusXebecTimer.over = "yes";
	// Gregg: запись 5 написана от лица того, кто «Пиранью» знал в лицо. Без пролога
	// ГГ судит по чужим рассказам, а не по узнаванию - для него запись 23.
	if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha")) GS_Record("23");
	else GS_Record("5");
}

void TreacheryOfGrifoSajfusXebecSink(string qName)
{
	if (Group_FindGroup("Chameleon") != -1) Group_DeleteGroup("Chameleon");

	PChar.TreacheryOfGrifoSajfus = "6";
	PChar.TreacheryOfGrifoSajfus.ChameleonSink = true;
	PChar.quest.TreacheryOfGrifoSajfusXebecCapture.over = "yes";
	PChar.quest.TreacheryOfGrifoSajfusXebecTimer.over = "yes";
	GS_Record("6");
}

void TreacheryOfGrifoSajfusItIsNotShip()
{
	PChar.TreacheryOfGrifoSajfus = "7";
	GS_Record("24");
	SetTimerFunction("TreacheryOfGrifoSajfusAfricaNews", 0, 0, GS_WAIT_DAYS);
}

void TreacheryOfGrifoSajfusXebecTimer(string qName)
{
	PChar.TreacheryOfGrifoSajfus = "END";
	PChar.quest.TreacheryOfGrifoSajfusXebecCapture.over = "yes";
	PChar.quest.TreacheryOfGrifoSajfusXebecSink.over = "yes";
	PChar.quest.TreacheryOfGrifoSajfusXebecInCuba.over = "yes";
	GS_Record("7");
	CloseQuestHeader("TreacheryOfGrifoSajfus");
}

void TreacheryOfGrifoSajfusAfricaNews(string qName)
{
	PChar.TreacheryOfGrifoSajfus = "8";
}

void TreacheryOfGrifoSajfusNeedTrader()
{
	PChar.TreacheryOfGrifoSajfus = "9";
	GS_Record("25");

	ref chr = GetCharacter(NPC_GenerateCharacter("GrifoSajfus Trader", "trader_12", "man", "man", 10, PIRATE, -1, true));
	chr.name = iTxt("GS_TraderName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\TreacheryOfGrifoSajfus\GS_Trader.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "cit_common";
	LAi_SetCitizenType(chr);
	LAi_SetImmortal(chr, true);
	// Gregg: в CSP локация называется с маленькой «j» - Bermudes_jungle_01
	ChangeCharacterAddressGroup(chr, "Bermudes_jungle_01", "goto", "goto1");
	GS_SetEncounters("Bermudes_jungle_01", true);

	SetTimerFunction("TreacheryOfGrifoSajfusTraderTimer", 0, 0, 30);
}

void TreacheryOfGrifoSajfusTraderTimer(string qName)
{
	PChar.TreacheryOfGrifoSajfus = "END";
	GS_Record("7");
	CloseQuestHeader("TreacheryOfGrifoSajfus");

	// Gregg: торговца надо убрать вместе с запретом на энкаунтеры, иначе джунгли
	// останутся стерильными до конца партии
	GS_SetEncounters("Bermudes_jungle_01", false);
	if (GetCharacterIndex("GrifoSajfus Trader") != -1)
	{
		Characters[GetCharacterIndex("GrifoSajfus Trader")].LifeDay = 0;
	}
}

void TreacheryOfGrifoSajfusBattleWithTrader()
{
	PChar.quest.TreacheryOfGrifoSajfusTraderTimer.over = "yes";

	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("GrifoSajfus Trader");
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);
	// Gregg: 250 HP оригинала - это долгая молотилка на пятом ранге, квест ранний
	LAi_SetHP(chr, GS_TRADER_HP, GS_TRADER_HP);
	LAi_SetImmortal(chr, false);
	LAi_SetCheckMinHP(chr, 1.0, true, "TreacheryOfGrifoSajfus_1");

	// Gregg: сдача торговца приходит с пустым qname - через модульный диспетч
	// в quests_reaction.c, а не кейсом в свитче
	PChar.quest.TreacheryOfGrifoSajfus_1.function = "TreacheryOfGrifoSajfusTraderMinHealth";
}

void TreacheryOfGrifoSajfusTraderMinHealth(string qName)
{
	ref chr = CharacterFromID("GrifoSajfus Trader");
	LAi_RemoveCheckMinHP(chr);
	LAi_UseAtidoteBottle(chr);
	LAi_SetHP(chr, 100.0, 100.0);
	LAi_SetImmortal(chr, true);

	// Gregg: бой надо погасить ДО диалога. В оригинале торговец оставался врагом
	// в активной схватке, и боевой ИИ продолжал им рулить прямо поверх разговора -
	// он шагал по поляне, пока игрок читал реплики.
	// Отношение самой группы «EnemyFight» не трогаем: она общемодовая, и десятки мест
	// кидают туда врагов, НЕ выставляя отношение заново, - подружив её, мы бы сделали
	// пассивной чужую засаду. Достаточно вынуть из неё одного торговца.
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	LAi_SetCitizenType(chr);
	LAi_SetFightMode(PChar, false);

	chr.Dialog.CurrentNode = "5";
	SetDialogCharacter2Character(PChar, chr, 1.0, true);
}

void TreacheryOfGrifoSajfusTraderExit()
{
	chrDisableReloadToLocation = false;
	GS_SetEncounters("Bermudes_jungle_01", false);

	ref chr = CharacterFromID("GrifoSajfus Trader");
	LAi_SetActorType(chr);
	// Gregg: таймаут -1 отключает LAi_tmpl_runto_Teleport, и актёр, не нашедший путь,
	// залипает в «falure» навсегда. Обжигались на Педро Карлосе в прологе.
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	LAi_SetPlayerType(PChar);
	PChar.TreacheryOfGrifoSajfus = "10";
	if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha")) GS_Record("19");
	else GS_Record("8");

	Characters[GetCharacterIndex("GrifoSajfus Trader")].LifeDay = 0;

	ref chest = ItemsFromID("SajfusDiamondChest");
	chest.shown = true;
	chest.startLocation = "Shore56";
	chest.startLocator = "diamond_chest";

	// Gregg: в К:ПС подбор сундука ловился в общем обработчике предметов (lugger\utils.c),
	// которого в CSP нет. Ставим штатное условие победы по предмету.
	PChar.quest.TreacheryOfGrifoSajfusFindDiamondChest.win_condition.l1 = "item";
	PChar.quest.TreacheryOfGrifoSajfusFindDiamondChest.win_condition.l1.item = "SajfusDiamondChest";
	PChar.quest.TreacheryOfGrifoSajfusFindDiamondChest.function = "TreacheryOfGrifoSajfusFindDiamondChest";
}

void TreacheryOfGrifoSajfusFindDiamondChest(string qName)
{
	PChar.TreacheryOfGrifoSajfus = "11";
	GS_Record("9");

	PChar.quest.TreacheryOfGrifoSajfusToBermudes.win_condition.l1 = "location";
	PChar.quest.TreacheryOfGrifoSajfusToBermudes.win_condition.l1.location = "Pirates_town";
	PChar.quest.TreacheryOfGrifoSajfusToBermudes.function = "TreacheryOfGrifoSajfusToBermudes";
}

void TreacheryOfGrifoSajfusToBermudes(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GeneratePhantomCharacter("Otto Groberg Notice", PIRATE, MAN, 0));
	chr.id = "Otto Groberg Notice";
	chr.name = iTxt("GS_StrangerName");
	chr.lastname = "";
	chr.model = "pirate_" + (rand(9)+1);
	chr.model.animation = "man";
	chr.Dialog.FileName = "Quest\KPS\TreacheryOfGrifoSajfus\GS_Otto.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "cit_common";
	SetNewModelToChar(chr);
	LAi_SetStayType(chr);
	ChangeCharacterAddressGroup(chr, "Pirates_town", "goto", "goto8");
	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

void TreacheryOfGrifoSajfusTakeNotice()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Otto Groberg Notice");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload4_back", "none", "", "", "", 30.0);

	LAi_SetPlayerType(PChar);
	TakeNItems(PChar, "OttoGrobergNotice", 1);

	PChar.quest.TreacheryOfGrifoSajfusTakeNoticeInTavern.win_condition.l1 = "location";
	PChar.quest.TreacheryOfGrifoSajfusTakeNoticeInTavern.win_condition.l1.location = "Pirates_tavern";
	PChar.quest.TreacheryOfGrifoSajfusTakeNoticeInTavern.function = "TreacheryOfGrifoSajfusTakeNoticeInTavern";
}

void TreacheryOfGrifoSajfusTakeNoticeInTavern(string qName)
{
	// Gregg: chrDisableReloadToLocation держим до конца разговора с Отто
	chrDisableReloadToLocation = true;
	FreeSitLocator("Pirates_tavern", "sit_base4");
	FreeSitLocator("Pirates_tavern", "sit_front4");

	ref chr = GetCharacter(NPC_GenerateCharacter("Otto Groberg", "officer_18", "man", "man", 1, PIRATE, -1, true));
	chr.name = iTxt("GS_OttoName");
	chr.lastname = iTxt("GS_OttoLastName");
	chr.Dialog.FileName = "Quest\KPS\TreacheryOfGrifoSajfus\GS_Otto.c";
	chr.Dialog.CurrentNode = "3";
	chr.greeting = "Gr_questOfficer";
	LAi_SetSitType(chr);
	LAi_SetImmortal(chr, true);

	ChangeCharacterAddressGroup(chr, "Pirates_tavern", "sit", "sit_base4");
	SetQuestMarkToCharacter(chr, true);
}

void TreacheryOfGrifoSajfusToPuertoPrincipe()
{
	PChar.TreacheryOfGrifoSajfus = "12";
	GS_Record("10");

	TakeNItems(PChar, "OttoGrobergNotice", -1);
	ref chr = CharacterFromID("Otto Groberg");
	SetQuestMarkToCharacter(chr, false);
	ChangeCharacterAddressGroup(chr, "Pirates_tavern", "goto", "goto2");

	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "TreacheryOfGrifoSajfus_2", 10.0);

	LAi_SetImmortal(chr, false);

	SetTimerFunction("TreacheryOfGrifoSajfusMarlinTimer", 0, 0, 10);

	// Gregg: «Марлин» - лёгкая шебека в топ-генерации. FantomMakeCoolestSailor, в отличие
	// от FantomMakeCoolSailor, зовёт внутри GenerateShipTop: полный комплект пушек
	// и номинальные статы.
	int iRank = sti(PChar.rank) + MOD_SKILL_ENEMY_RATE*2;
	ref sld = GetCharacter(NPC_GenerateCharacter("MarlinShip", "", "man", "man", iRank, PIRATE, -1, true));
	SetCaptanModelByEncType(sld, "pirate");
	FantomMakeCoolestSailor(sld, SHIP_XEBECLIGHT, iTxt("GS_MarlinShipName"), CANNON_TYPE_CANNON_LBS12, 70, 70, 70);

	// Gregg: уникальной шебеке - свои статы поверх стоковых, по образцу «Дозорного».
	// Патчится слот в RealShips, заведённый GenerateShipTop, а не шаблон типа, поэтому
	// прибавки уезжают вместе с кораблём к тому, кто возьмёт его на абордаж.
	realships[sti(sld.ship.type)].MaxCaliber = 12;              // было 10
	realships[sti(sld.ship.type)].SpeedRate = 18.5;             // было 16.5
	realships[sti(sld.ship.type)].WindAgainstSpeed = 10.0;      // было 9.0
	realships[sti(sld.ship.type)].Capacity = 1850;              // было 1600
	realships[sti(sld.ship.type)].HP = 2000;                    // было 1700
	realships[sti(sld.ship.type)].OptCrew = 140;                    // было 120
	realships[sti(sld.ship.type)].MaxCrew = 175;                    // было 150
	// LEO: минимум составляет 20% штатных 140 моряков, а не команды базовой шебеки.
	realships[sti(sld.ship.type)].MinCrew = 28;

	// Gregg: запоминаем тип корабля - по нему потом отличим захват от потопления.
	// В оригинале сравнивали имя модели с «Xebec1», хотя тип объявлен как «Xebec»,
	// так что ветка «корабль достался игроку» не срабатывала никогда.
	PChar.TreacheryOfGrifoSajfus.MarlinShipType = sld.ship.type;

	DeleteAttribute(sld, "SinkTenPercent");
	DeleteAttribute(sld, "DontRansackCaptain");

	// Gregg: К:ПС снимала с капитана «Марлина» ещё и SaveItemsForDead, DontClearDead
	// и AboardToFinalDeck - тело исчезало вместе с вещами. Выходило, что подставной
	// капитан в середине квеста лут даёт, а финальный нет. Оставляем все три:
	// абордаж доводится до капитанской палубы, труп не убирается, вещи с него не снимаются.
	// Снаряжение FantomMakeCoolestSailor не выдаёт - равняем на «Хамелеона» вручную,
	// иначе снимать с трупа было бы попросту нечего.
	LAi_NPC_Equip(sld, iRank, true, true);
	sld.AlwaysSandbankManeuver = true;
	sld.AnalizeShips = true;
	sld.DontRansackCaptain = true;
	sld.AlwaysEnemy = true;
	SetGeraldDetails(sld, -1, 1, "", "");
	LAi_group_MoveCharacter(sld, "EnemyFight");

	Group_FindOrCreateGroup("MarlinShip");
	Group_AddCharacter("MarlinShip", "MarlinShip");
	Group_SetType("MarlinShip", "pirate");
	Group_SetGroupCommander("MarlinShip", "MarlinShip");
	Group_SetAddress("MarlinShip", "Cuba1", "quest_ships", "quest_ship_10");
	Group_SetTaskAttack("MarlinShip", PLAYER_GROUP);

	PChar.quest.TreacheryOfGrifoSajfusMarlinSink.win_condition.l1 = "NPC_Death";
	PChar.quest.TreacheryOfGrifoSajfusMarlinSink.win_condition.l1.character = "MarlinShip";
	PChar.quest.TreacheryOfGrifoSajfusMarlinSink.function = "TreacheryOfGrifoSajfusMarlinSink";

	// Gregg: Отто уходит из таверны через шаблон актёра - тоже пустой qname
	PChar.quest.TreacheryOfGrifoSajfus_2.function = "TreacheryOfGrifoSajfusOttoLeaveTavern";
}

// Gregg: своя функция вместо кейса в свитче - Отто добежал до выхода из таверны
void TreacheryOfGrifoSajfusOttoLeaveTavern(string qName)
{
	chrDisableReloadToLocation = false;
	ref chr = CharacterFromID("Otto Groberg");
	LAi_SetOfficerType(chr);
	AddPassenger(PChar, chr, 0);
	// Gregg: закрываем от назначений. Гейт в экране персонажей - GetRemovable
	// (character_all.c:1821), то есть атрибут NonRemovable; isquest там не смотрят,
	// он работает только в окнах обмена вещами. Так же закрыт фактор Компании в ГВИК.
	SetCharacterRemovable(chr, false);
}

void TreacheryOfGrifoSajfusMarlinTimer(string qName)
{
	// Gregg: не успел за 10 дней - Отто сходит на берег, «Марлин» уходит
	ref Otto = CharacterFromID("Otto Groberg");
	GS_ReleaseOtto(Otto);
	Otto.LifeDay = 0;

	if (Group_FindGroup("MarlinShip") != -1) Group_DeleteGroup("MarlinShip");
	PChar.TreacheryOfGrifoSajfus = "END";
	GS_Record("11");
	CloseQuestHeader("TreacheryOfGrifoSajfus");

	PChar.quest.TreacheryOfGrifoSajfusMarlinSink.over = "yes";
}

void TreacheryOfGrifoSajfusMarlinSink(string qName)
{
	PChar.TreacheryOfGrifoSajfus = "13";
	PChar.quest.TreacheryOfGrifoSajfusMarlinTimer.over = "yes";

	// Gregg: захват от потопления отличаем по типу корабля под игроком - см. выше,
	// почему сравнение по имени модели из оригинала не годится
	// Gregg: шебека считается взятой и тогда, когда игрок пересел на неё сам,
	// и тогда, когда посадил на неё офицера - во втором случае корабль уходит
	// в эскадру, ship.type у ГГ остаётся прежним, и раньше это читалось как «утопил».
	bool bCaptured = false;
	if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.MarlinShipType"))
	{
		int iMarlin = sti(PChar.TreacheryOfGrifoSajfus.MarlinShipType);
		if (sti(PChar.ship.type) == iMarlin) bCaptured = true;
		int iComp;
		int iCn;
		for (iComp = 1; iComp < COMPANION_MAX; iComp++)
		{
			iCn = GetCompanionIndex(PChar, iComp);
			if (iCn < 0) continue;
			if (!CheckAttribute(&Characters[iCn], "ship.type")) continue;
			if (sti(Characters[iCn].ship.type) == iMarlin) bCaptured = true;
		}
	}

	bool bOttoDead = LAi_IsDead(CharacterFromID("Otto Groberg"));

	if (bCaptured)
	{
		PChar.TreacheryOfGrifoSajfus.MarlinSink = false;
		if (bOttoDead)
		{
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha")) GS_Record("20");
			else GS_Record("12");
			PChar.TreacheryOfGrifoSajfus = "14";
		}
		else
		{
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha")) GS_Record("21");
			else GS_Record("13");
		}
	}
	else
	{
		PChar.TreacheryOfGrifoSajfus.MarlinSink = true;
		if (bOttoDead)
		{
			GS_Record("14");
			PChar.TreacheryOfGrifoSajfus = "14";
		}
		else
		{
			GS_Record("15");
		}
	}

	if (!bOttoDead)
	{
		GS_SetEncounters("PuertoPrincipe_port", true);
		PChar.quest.TreacheryOfGrifoSajfusOttoToPuertoPrincipe.win_condition.l1 = "location";
		PChar.quest.TreacheryOfGrifoSajfusOttoToPuertoPrincipe.win_condition.l1.location = "PuertoPrincipe_port";
		PChar.quest.TreacheryOfGrifoSajfusOttoToPuertoPrincipe.function = "TreacheryOfGrifoSajfusOttoToPuertoPrincipe";

		GS_ReleaseOtto(CharacterFromID("Otto Groberg"));
	}
}

void TreacheryOfGrifoSajfusOttoToPuertoPrincipe(string qName)
{
	ref chr = CharacterFromID("Otto Groberg");
	ChangeCharacterAddressGroup(chr, "PuertoPrincipe_port", "goto", "goto9");
	LAi_SetStayType(chr);
	chr.Dialog.CurrentNode = "16";

	SetDialogCharacter2Character(PChar, chr, 5.0, true);
}

void TreacheryOfGrifoSajfusOttoExit()
{
	GS_SetEncounters("PuertoPrincipe_port", false);
	ref chr = CharacterFromID("Otto Groberg");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	chr.LifeDay = 0;

	LAi_SetPlayerType(PChar);
	PChar.TreacheryOfGrifoSajfus = "14";
}

void TreacheryOfGrifoSajfusEnd()
{
	PChar.TreacheryOfGrifoSajfus = "END";
	GS_Record("16");
	CloseQuestHeader("TreacheryOfGrifoSajfus");
	pchar.questdata.TreacheryOfGrifoSajfus = 1;
	UnlockAchievement("GrifoSajfus_quest", 3);
}
// Gregg: <== «Предательство Грифо Сайфуса»
