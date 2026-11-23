// Gregg: ==> «Корабль обезьян» (перенос из К:ПС)
//
// Источник: К:ПС, PROGRAM\lugger\Quests.c, раздел «КОРАБЛЬ ОБЕЗЬЯН».
// Имена функций сохранены как в оригинале, чтобы можно было сверяться с источником.
//
// Завязка: Аллен Гладстонн у церкви Бас-Тера, с 13 ранга. Дальше: остров, где его
// якобы абордировали -> грот, капитан Маугли, безнадёжный бой и плен -> охрана
// в гроте -> церковь Белиза, Аллен обманывает второй раз -> засада за воротами ->
// обратно в грот, Ральф Ваттерсон -> банк, жена Ральфа, гонка со временем ->
// расправа с Алленом -> Чёрный Мальчик на берегу -> договор с демоном в дальнем
// гроте -> сожжение договора -> морской бой с «Банановым раем».
//
// Отличия от оригинала - в комментариях по коду. Главные:
// - НЕ отбираются вещи, деньги и корабль. В К:ПС плен снимал с игрока всё подчистую
//   (CaptainMonkeyRemoveItems складывал инвентарь в атрибуты ГГ, корабль подменялся
//   на SHIP_PLOT), а возвращал через ящик в гроте. Это тонна состояния в сейве,
//   потерянные офицеры и стык с нашей эскадрой - выброшено целиком;
// - сцена плена переделана: бой с Маугли идёт 45 секунд ЛИБО до просадки ГГ
//   до 5 HP, что раньше. Потом ГГ убирает оружие, второй разговор, Маугли уходит,
//   а грот запирают скелеты - пассивные, пока их не тронешь;
// - Аллен во второй раз ждёт в церкви Белиза, а не в случайной из девяти;
// - финальный корабль - галеон SHIP_GALEON_H со своими статами вместо
//   несуществующего у нас SHIP_BATTLESHIP, команда - обезьяны-нежить;
// - капитан Глюк выброшен: он существовал только чтобы вернуть отобранный корабль;
// - анимация скелетов man2 из К:ПС у нас отсутствует, везде man.

#define CM_RANK_REQ			13		// с какого ранга Аллен вообще заговорит. САМА проверка
									// живёт в диалоге (дефайны в сегмент не доезжают) и там же
									// смотрит на тумблер bQuestsRank - правишь тут, правь и в CM_Allen.c
#define CM_GROT_FIGHT_SEC	45.0	// сколько держаться против Маугли
#define CM_MAUGLI_RANK		66		// ранг Маугли
#define CM_MAUGLI_HP		900.0	// здоровье в гроте: бой заведомо проигрышный, и полоса
									// почти не двигается - «сталь его не берёт»
#define CM_MAUGLI_HP_FINAL	450.0	// здоровье в каюте: там ДВЕ фазы, и вторая начинается
									// с полного восстановления (LAi_SetCurHPMax в узле 8)
#define CM_BANK_DAYS		7		// сколько есть на гонку с Алленом до банка

// Gregg: в К:ПС это DisableLandEncounters из lugger\utils.c, в CSP такой обёртки нет.
void CM_SetEncounters(string sLocation, bool bDisable)
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
// нескольких NPC с общим префиксом имени.
void CM_SetNPCDeadFunction(string sQuest, string sFunction, string sName, int iQty)
{
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

// Gregg: нация колонии, стоящей на этом острове. Нужна «Банановому раю»: пиратский
// флаг под боком у форта означал, что сторожевики успевали разнести Маугли ещё до
// того, как игрок до него доберётся. Читаем именно текущую нацию, а не зашитую -
// колонию за время игры могли и перезахватить.
int CM_GetIslandNation(string sIsland)
{
	int iN;
	for (iN = 0; iN < MAX_COLONIES; iN++)
	{
		if (!CheckAttribute(&Colonies[iN], "island")) continue;
		if (Colonies[iN].island != sIsland) continue;
		return sti(Colonies[iN].nation);
	}
	return PIRATE;
}

// Gregg: согнать со стула случайного прихожанина, но НЕ квестового персонажа.
// Общий FreeSitLocator из scripts\utils.c для этого не годится: он выкидывает «в никуда»
// вообще всех, кто стоит на локаторе, - в том числе только что усаженного туда Аллена,
// из-за чего в церкви оказывалось пусто. В К:ПС аналогичный FreeLocator пропускал
// персонажей с атрибутом NotRemoved, здесь эта проверка возвращена.
void CM_FreeSitLocator(string sLocation, string sLocator)
{
	ref chr;
	int iN;
	for (iN = 0; iN < MAX_CHARACTERS; iN++)
	{
		makeref(chr, Characters[iN]);
		if (!CheckAttribute(chr, "location")) continue;
		if (chr.location != sLocation) continue;
		if (chr.location.locator != sLocator) continue;
		if (CheckAttribute(chr, "NotRemoved")) continue;
		if (iN == GetMainCharacterIndex()) continue;
		LogoffCharacter(chr);
		return;
	}
}

// Gregg: поднять скелета-охранника. В оригинале блок повторялся четырежды,
// и везде с анимацией man2, которой у нас нет.
ref CM_MakeSkeleton(string sId, int iRank)
{
	string sModel = "Skel_" + (rand(4)+1);
	ref chr = GetCharacter(NPC_GenerateCharacter(sId, sModel, "skeleton", "man", iRank, PIRATE, 0, true));
	// Gregg: генератор выдаёт нежити человеческое имя и человеческое приветствие.
	// Охрана грота пассивна, к ней МОЖНО подойти и заговорить - и без своего файла
	// диалога это была «Ошибка открытия файла диалога» от Маколма Лодердэйла.
	chr.name = iTxt("CM_SkeletonName");
	chr.lastname = "";
	chr.greeting = "";
	chr.Dialog.FileName = "Quest\KPS\CaptainMonkey\CM_Skeleton.c";
	chr.Dialog.CurrentNode = "First time";
	return chr;
}

// Gregg: случайные места действия. Церковь зафиксирована на Белизе - в оригинале
// она выбиралась из девяти, и игрок обходил их по кругу в поисках Аллена.
// Переписано на if/else: return внутри case - известная ловушка Storm.
string CaptainMonkeyGenerateLocation(string sType)
{
	if (sType == "Church") return "Beliz";

	int iRoll;
	if (sType == "Island")
	{
		iRoll = rand(2);
		if (iRoll == 0) return "Antigua";
		if (iRoll == 1) return "Jamaica";
		return "Tortuga";
	}
	if (sType == "Grot")
	{
		iRoll = rand(4);
		if (iRoll == 0) return "Caracas_Grot";
		if (iRoll == 1) return "Charles_Grot";
		if (iRoll == 2) return "Cuba_Grot";
		if (iRoll == 3) return "Dominica_Grot";
		return "Trinidad_Grot";
	}
	if (sType == "Bank")
	{
		iRoll = rand(7);
		if (iRoll == 0) return "SanJuan";
		if (iRoll == 1) return "Havana";
		if (iRoll == 2) return "Marigo";
		if (iRoll == 3) return "Charles";
		if (iRoll == 4) return "SantoDomingo";
		if (iRoll == 5) return "Panama";
		if (iRoll == 6) return "Cartahena";
		return "Cumana";
	}
	return "PortRoyal";
}

string CaptainMonkeyGetGrotIsland()
{
	string sGrot = PChar.CaptainMonkey.GrotID;
	if (sGrot == "Caracas_Grot")  return iTxt("CM_GrotCaracas");
	if (sGrot == "Charles_Grot")  return iTxt("CM_GrotCharles");
	if (sGrot == "Cuba_Grot")     return iTxt("CM_GrotCuba");
	if (sGrot == "Dominica_Grot") return iTxt("CM_GrotDominica");
	if (sGrot == "Trinidad_Grot") return iTxt("CM_GrotTrinidad");
	return iTxt("CM_GrotDefault");
}

void CaptainMonkeyPrepare()
{
	ref chr = GetCharacter(NPC_GenerateCharacter("Allen Gladstonn", "pirate_" + (rand(9)+1), "man", "man", 10, PIRATE, -1, false));
	chr.name = iTxt("CM_AllenName");
	chr.lastname = iTxt("CM_AllenLastName");
	chr.Dialog.FileName = "Quest\KPS\CaptainMonkey\CM_Allen.c";
	chr.Dialog.CurrentNode = "First time";
	// Gregg: в К:ПС стоял Gr_Crazy, но такой секции нет ни в одном Greetings_alias.ini -
	// озвучки не было бы вовсе. Аллен побирается у церкви, ему подходит Gr_poorman.
	chr.greeting = "Gr_poorman";
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "BasTer_town", "goto", "goto23");
	SetQuestMarkToCharacter(chr, true);

	// Gregg: диалог Аллена читает это значение напрямую, поэтому атрибут должен
	// существовать с самого начала игры - короткого замыкания в Storm нет
	PChar.CaptainMonkey = "0";

	PChar.CaptainMonkey.IslandID = CaptainMonkeyGenerateLocation("Island");
	PChar.CaptainMonkey.ChurchID = CaptainMonkeyGenerateLocation("Church");
	PChar.CaptainMonkey.BankID = CaptainMonkeyGenerateLocation("Bank");
	PChar.CaptainMonkey.GrotID = CaptainMonkeyGenerateLocation("Grot");

	// Gregg: берег того же острова - в К:ПС его выдавал GetIslandRandomShoreIdRand
	string sIsl = PChar.CaptainMonkey.IslandID;
	if (sIsl == "Antigua")  PChar.CaptainMonkey.IslandShoreID = "Shore1";
	if (sIsl == "Jamaica")  PChar.CaptainMonkey.IslandShoreID = "Shore36";
	if (sIsl == "Tortuga")  PChar.CaptainMonkey.IslandShoreID = "Shore58";

	PChar.CaptainMonkey.CitizModel = "citiz_" + (rand(11)+1);
	PChar.CaptainMonkey.GirlModel = "girl_" + (rand(9)+1);
}

// Gregg: записи журнала - через обёртку, чтобы в тексте работали гендерные формы.
// sSex - обычное окончание («нашёл» / «нашла»), sSex1 - возвратное («ся» / «ась»).
void CM_Record(string sRec)
{
	AddQuestRecord("CaptainMonkey", sRec);
	AddQuestUserData("CaptainMonkey", "sSex", GetSexPhrase("", "а"));
	AddQuestUserData("CaptainMonkey", "sSex1", GetSexPhrase("ся", "ась"));
}

void CaptainMonkeyStart()
{
	PChar.CaptainMonkey = "1";

	SetQuestHeader("CaptainMonkey");
	CM_Record("1");

	string sLocation = PChar.CaptainMonkey.IslandID;
	AddQuestUserData("CaptainMonkey", "sIsland", XI_ConvertString(sLocation + "Gen"));

	ref chr = CharacterFromID("Allen Gladstonn");
	SetQuestMarkToCharacter(chr, false);
	LAi_SetActorType(chr);
	// Gregg: таймаут -1 отключает LAi_tmpl_runto_Teleport, актёр залипает в «falure».
	LAi_ActorRunToLocation(chr, "reload", "gate_back", "none", "", "", "", 30.0);

	PChar.quest.CaptainMonkeyInIslandSea.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyInIslandSea.win_condition.l1.location = sLocation;
	PChar.quest.CaptainMonkeyInIslandSea.function = "CaptainMonkeyInIslandSea";
}

void CaptainMonkeyInIslandSea(string qName)
{
	CM_Record("3");

	string sLocation = PChar.CaptainMonkey.IslandID;
	AddQuestUserData("CaptainMonkey", "sIsland", XI_ConvertString(sLocation + "Gen"));

	sLocation = PChar.CaptainMonkey.IslandID + "_Grot";
	PChar.quest.CaptainMonkeyToGrot.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyToGrot.win_condition.l1.location = sLocation;
	PChar.quest.CaptainMonkeyToGrot.function = "CaptainMonkeyToGrot";
	CM_SetEncounters(sLocation, true);
}

void CaptainMonkeyToGrot(string qName)
{
	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	chrDisableReloadToLocation = true;

	// Gregg: офицеров в гроте быть не должно - сцена задумана как разговор один на один
	DeleteAllOfficersFromLocation();

	ref chr = GetCharacter(NPC_GenerateCharacter("Captain Monkey", "monkey", "skeleton", "monkey", CM_MAUGLI_RANK, PIRATE, -1, false));
	chr.name = iTxt("CM_MaugliName");
	chr.lastname = iTxt("CM_MaugliLastName");
	chr.Dialog.FileName = "Quest\KPS\CaptainMonkey\CM_Maugli.c";
	chr.Dialog.CurrentNode = "1";
	// Gregg: Gr_Monkey из К:ПС у нас нет, да и разговор Маугли начинает сам -
	// приветствие всё равно не отыграется
	chr.greeting = "Gr_poorman";
	LAi_SetMonkeyType(chr);
	LAi_SetImmortal(chr, true);
	// Gregg: в К:ПС стоял goto2, но у нас Антигуа и Ямайка используют grotto1_locators,
	// а там из «goto» есть только goto1 - на двух островах из трёх Маугли не появился бы
	ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto1");
	SetCameraDialogMode(chr);

	// Gregg: макокраб - штатное «оружие крабов и обезьян», Арена выдаёт его так же.
	// SaveItemsForDead намеренно НЕ ставим: в финале он не должен упасть игроку.
	RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(chr, GUN_ITEM_TYPE);
	GiveItem2Character(chr, "monkrab");
	EquipCharacterByItem(chr, "monkrab");

	SetDialogCharacter2Character(PChar, chr, 15.0, false);
}

void CaptainMonkeyBattleWithCaptainInGrot()
{
	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;

	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Captain Monkey");
	LAi_SetMonkeyType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	LAi_SetHP(chr, CM_MAUGLI_HP, CM_MAUGLI_HP);

	// Gregg: сцена сюжетно проигрышная, но бессмертия игроку не даём. Два выхода:
	// продержаться CM_GROT_FIGHT_SEC секунд либо просесть до 5 HP - что раньше.
	// Второй триггер и есть страховка от провала, он же стоял в оригинале.
	LAi_SetImmortal(PChar, false);
	LAi_SetCheckMinHP(PChar, 5.0, true, "CaptainMonkey_1");
	PChar.quest.CaptainMonkey_1.function = "CaptainMonkeyBattleWithCaptainInGrotEnd";

	DoQuestFunctionDelay("CaptainMonkeyBattleWithCaptainInGrotEnd", CM_GROT_FIGHT_SEC);
}

void CaptainMonkeyBattleWithCaptainInGrotEnd(string _tmp)
{
	// Gregg: функция срабатывает по таймеру ИЛИ по просадке HP, поэтому защищаемся
	// от повторного входа - иначе вторая сработка поднимет вторую партию охраны
	if (CheckAttribute(PChar, "CaptainMonkey.GrotDone")) return;
	PChar.CaptainMonkey.GrotDone = true;

	LAi_RemoveCheckMinHP(PChar);
	LAi_UseAtidoteBottle(PChar);
	LAi_SetCurHPMax(PChar);
	LAi_SetFightMode(PChar, false);

	// Gregg: бой окончен - Маугли выходит из схватки и говорит второй раз
	ref chr = CharacterFromID("Captain Monkey");
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	LAi_SetMonkeyType(chr);

	// Gregg: гейт по HP выдал игроку бессмертие - снимаем. LAi_ProcessCheckMinHP
	// (LAi_character.c) при срабатывании сам зовёт LAi_SetImmortal(chr, true), а
	// LAi_RemoveCheckMinHP убирает только сам чекер. Без этой строки проигравший
	// Маугли ГГ оставался неуязвимым до конца игры. Снимаем ПОСЛЕ вывода Маугли
	// из боевой группы, иначе между снятием и миром остаётся окно на удар.
	LAi_SetImmortal(PChar, false);

	chr.Dialog.CurrentNode = "3";
	SetDialogCharacter2Character(PChar, chr, 5.0, true);
}

// Gregg: Маугли уходит, грот запирают. Зовётся из диалога Маугли.
void CaptainMonkeyReloadInGrot()
{
	LAi_SetPlayerType(PChar);
	ChangeCharacterAddress(CharacterFromID("Captain Monkey"), "None", "");

	PChar.CaptainMonkey = "2";
	CM_Record("2");

	// Gregg: выход глушим до последнего охранника
	LocatorReloadEnterDisable(PChar.location, "reload1_back", true);

	int iRank = sti(PChar.rank) + 2;
	ref chr;
	int iN;
	for (iN = 1; iN <= 4; iN++)
	{
		chr = CM_MakeSkeleton("Captain Monkey Grot Guard " + iN, iRank);
		ChangeCharacterAddressGroup(chr, PChar.location, "monsters", "monster" + iN);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "CM_GROT_GUARDS");
	}

	// Gregg: стоят смирно, пока их не тронешь. Отношение нейтральное, но по тревоге
	// группа становится враждебной - это и есть «первым не нападают».
	LAi_group_SetRelation("CM_GROT_GUARDS", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetAlarmReaction("CM_GROT_GUARDS", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY, LAI_GROUP_NEITRAL);

	CM_SetNPCDeadFunction("CaptainMonkeyExitFromGrot", "CaptainMonkeyExitFromGrot", "Captain Monkey Grot Guard ", 4);

	// Gregg: грот - локация типа cave с группой monsters, и открытие сундука с шансом
	// 50% зовёт генератор «Проклятый капитан» (spawnToughSkeleton в items\itemLogic.c).
	// Скелет 20+ рангов посреди квестовой сцены лишний, глушим штатным таймером.
	// Срок с запасом: в этот же грот игрок вернётся ещё раз, за Ральфом.
	LAi_LocationDisableToughSkeletonTimer(PChar.location, 60);

	// Gregg: сундук в гроте остаётся обычным лутом - в оригинале в нём лежала рапира
	// взамен отобранной, а вещи мы больше не отбираем
	SetQuestBoxItems(PChar.location, "potion1", 6);
	SetQuestBoxItems(PChar.location, "potion2", 4);
	SetQuestBoxItems(PChar.location, "food1", 10);
	// Gregg: награда одноразовая. SetQuestBoxItems всегда ставит stay, а с ним
	// SetLocationQuestBox накладывает шаблон при КАЖДОМ входе - грот превращался
	// в бесконечный источник зелий, и клад с карты сокровищ в этом сундуке
	// затирался (CopyAttributes заменяет содержимое целиком). Без stay шаблон
	// удалится сам после первого наполнения.
	DeleteAttribute(PChar, "GenQuestBox." + PChar.location + ".stay");
}

void CaptainMonkeyExitFromGrot(string qName)
{
	chrDisableReloadToLocation = false;
	LocatorReloadEnterDisable(PChar.location, "reload1_back", false);
	CM_SetEncounters(PChar.location, false);

	// Gregg: наводку на Белиз игрок получает не с потолка, а от трактирщика Бас-Тера -
	// узлы CaptainMonkey_1..2 в Common_Tavern.c. До разговора Аллена нет нигде.
	PChar.CaptainMonkey = "3";
	CM_Record("4");
}

// Gregg: зовётся из диалога трактирщика Бас-Тера
void CaptainMonkeyBarmanTold()
{
	PChar.CaptainMonkey = "3a";
	CM_Record("14");

	// Gregg: Аллен ждёт в церкви Белиза - в оригинале в случайной из девяти
	ref chr = CharacterFromID("Allen Gladstonn");
	chr.Dialog.CurrentNode = "9";
	chr.nonTable = true;
	chr.NotRemoved = true;
	string sLocation = PChar.CaptainMonkey.ChurchID + "_church";

	LAi_SetSitType(chr);
	ChangeCharacterAddressGroup(chr, sLocation, "sit", "sit2");

	PChar.quest.CaptainMonkeyInChurch.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyInChurch.win_condition.l1.location = sLocation;
	PChar.quest.CaptainMonkeyInChurch.function = "CaptainMonkeyInChurch";
}

void CaptainMonkeyInChurch(string qName)
{
	chrDisableReloadToLocation = true;
	CM_FreeSitLocator(PChar.location, "sit2");
}

void CaptainMonkeyFromChurchToTown()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Allen Gladstonn");
	ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto1");

	// Gregg: церковь всегда белизская, так что и локатор выхода один
	LAi_SetActorType(chr);
	LAi_ActorGoToLocation(chr, "reload", "reload1", "none", "", "", "", 30.0);

	string sLocation = PChar.CaptainMonkey.ChurchID + "_town";
	PChar.quest.CaptainMonkeyFromChurchInTown.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyFromChurchInTown.win_condition.l1.location = sLocation;
	PChar.quest.CaptainMonkeyFromChurchInTown.function = "CaptainMonkeyFromChurchInTown";
}

void CaptainMonkeyFromChurchInTown(string qName)
{
	ref chr = CharacterFromID("Allen Gladstonn");
	ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload7");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "gate_back", "none", "", "", "", 30.0);

	string sLocation = PChar.CaptainMonkey.ChurchID + "_ExitTown";
	PChar.quest.CaptainMonkeyFromChurchToExitTown.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyFromChurchToExitTown.win_condition.l1.location = sLocation;
	PChar.quest.CaptainMonkeyFromChurchToExitTown.function = "CaptainMonkeyFromChurchToExitTown";
	CM_SetEncounters(sLocation, true);
}

void CaptainMonkeyFromChurchToExitTown(string qName)
{
	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Allen Gladstonn");
	ChangeCharacterAddressGroup(chr, PChar.location, "reload", PChar.location.locator);
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload2_back", "none", "", "", "", 30.0);

	DoQuestFunctionDelay("CaptainMonkeyFromChurchInExitTown", 15.0);
}

void CaptainMonkeyFromChurchInExitTown(string _tmp)
{
	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;

	ref chr;
	int iRank = sti(PChar.rank) + 3;
	int iN;
	for (iN = 1; iN <= 5; iN++)
	{
		chr = CM_MakeSkeleton("Captain Monkey ExitTown Guard " + iN, iRank);
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload2_back");
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	CM_SetNPCDeadFunction("CaptainMonkeyFromChurchInExitTownWin", "CaptainMonkeyFromChurchInExitTownWin", "Captain Monkey ExitTown Guard ", 5);
}

void CaptainMonkeyFromChurchInExitTownWin(string qName)
{
	chrDisableReloadToLocation = false;
	CM_SetEncounters(PChar.location, false);

	PChar.CaptainMonkey = "4";
	CM_Record("5");

	string sGrot = PChar.CaptainMonkey.IslandID + "_Grot";
	CM_SetEncounters(sGrot, true);
	PChar.quest.CaptainMonkeyRetryToGrot.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyRetryToGrot.win_condition.l1.location = sGrot;
	PChar.quest.CaptainMonkeyRetryToGrot.function = "CaptainMonkeyRetryToGrot";
}

void CaptainMonkeyRetryToGrot(string qName)
{
	chrDisableReloadToLocation = true;

	string sModel = PChar.CaptainMonkey.CitizModel;
	ref chr = GetCharacter(NPC_GenerateCharacter("Ralph Vatterson", sModel, "man", "man", 10, PIRATE, -1, false));
	chr.name = iTxt("CM_RalphName");
	chr.lastname = iTxt("CM_RalphLastName");
	chr.Dialog.FileName = "Quest\KPS\CaptainMonkey\CM_Ralph.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_AllowToDump";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto1");
	LAi_SetActorType(chr);
	LAi_ActorAfraid(chr, PChar, false);

	int iRank = sti(PChar.rank) + 2;
	int iN;
	for (iN = 1; iN <= 4; iN++)
	{
		chr = CM_MakeSkeleton("Captain Monkey Grott Guard " + iN, iRank);
		ChangeCharacterAddressGroup(chr, PChar.location, "monsters", "monster" + iN);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);
	CM_SetNPCDeadFunction("CaptainMonkeyRetryToGrotWinBattle", "CaptainMonkeyRetryToGrotWinBattle", "Captain Monkey Grott Guard ", 4);
}

void CaptainMonkeyRetryToGrotWinBattle(string qName)
{
	ref chr = CharacterFromID("Ralph Vatterson");
	LAi_type_actor_Reset(chr);

	LAi_SetFightMode(PChar, false);
	SetDialogCharacter2Character(PChar, chr, 15.0, false);
}

void CaptainMonkeyToBankColony()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Ralph Vatterson");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);

	PChar.CaptainMonkey = "5";
	CM_Record("6");

	string sLocation = PChar.CaptainMonkey.IslandID;
	AddQuestUserData("CaptainMonkey", "sIsland", XI_ConvertString(sLocation + "Gen"));

	sLocation = PChar.CaptainMonkey.BankID;
	AddQuestUserData("CaptainMonkey", "sBankIsland", XI_ConvertString("Colony" + sLocation + "Gen"));

	sLocation += "_bank";
	PChar.quest.CaptainMonkeyInBankColony.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyInBankColony.win_condition.l1.location = sLocation;
	PChar.quest.CaptainMonkeyInBankColony.function = "CaptainMonkeyInBankColony";
	// Gregg: неделя вместо трёх в оригинале
	SetTimerFunction("CaptainMonkeyToBankColonyTime", 0, 0, CM_BANK_DAYS);

	int iRank = sti(PChar.rank) + 10;
	chr = CharacterFromID("Allen Gladstonn");
	chr.rank = iRank;
	SetFantomParamFromRank(chr, iRank, true);
}

void CaptainMonkeyToBankColonyTime(string qName)
{
	PChar.CaptainMonkey.BankTime = true;
}

void CaptainMonkeyInBankColony(string qName)
{
	PChar.quest.CaptainMonkeyToBankColonyTime.over = "yes";
	RemovePassenger(PChar, CharacterFromID("Ralph Vatterson"));

	if (!CheckAttribute(PChar, "CaptainMonkey.BankTime"))
	{
		chrDisableReloadToLocation = true;

		string sModel = PChar.CaptainMonkey.GirlModel;
		ref chr = GetCharacter(NPC_GenerateCharacter("Emily Vatterson", sModel, "woman", "woman", 1, PIRATE, 0, false));
		chr.name = iTxt("CM_EmilyName");
		chr.lastname = iTxt("CM_RalphLastName");
		chr.Dialog.FileName = "Quest\KPS\CaptainMonkey\CM_Ralph.c";
		chr.Dialog.CurrentNode = "4";
		chr.greeting = "Gr_Woman_Citizen";
		LAi_SetImmortal(chr, true);
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload2");
		SetDialogCharacter2Character(PChar, chr, 15.0, false);
		return;
	}
	Characters[GetCharacterIndex("Allen Gladstonn")].LifeDay = 0;
	// Gregg: провал. В К:ПС игрок должен был ещё поговорить с ростовщиком, и уже тот
	// узлом CaptainMonkey_1..3 в Usurer_dialog.c звал CaptainMonkeyBankTimeIsLeft.
	// Заводить ветку в общем диалоге ростовщика ради одного квеста не стали: Ральф
	// и так стоит рядом, поэтому зовём напрямую - иначе таймер был бы декоративным,
	// а опоздавший игрок застрял бы в квесте навсегда.
	CaptainMonkeyBankTimeIsLeft();
}

void CaptainMonkeyBankTimeIsLeft()
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Ralph Vatterson");
	chr.Dialog.CurrentNode = "7";
	chr.LifeDay = 0;
	SetDialogCharacter2Character(PChar, chr, 15.0, false);
}

// Gregg: снять шаблоны квестовых сундуков обоих гротов. Зовётся из финалов:
// брошенный квест не должен оставлять на карте пещеру с вечным лутом.
void CM_ClearQuestBoxes()
{
	if (CheckAttribute(PChar, "CaptainMonkey.IslandID"))
	{
		ClearQuestBoxStay(PChar.CaptainMonkey.IslandID + "_Grot");
	}
	if (CheckAttribute(PChar, "CaptainMonkey.GrotID"))
	{
		ClearQuestBoxStay(PChar.CaptainMonkey.GrotID);
	}
}

void CaptainMonkeyBankTimeIsLeftClosedQuest()
{
	chrDisableReloadToLocation = false;
	CM_ClearQuestBoxes();

	ref chr = CharacterFromID("Ralph Vatterson");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	PChar.CaptainMonkey = "END";
	CM_Record("7");
	CloseQuestHeader("CaptainMonkey");
}

void CaptainMonkeyFromBankToExitTown()
{
	PChar.CaptainMonkey = "6";
	CM_Record("8");
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Ralph Vatterson");
	chr.LifeDay = 0;
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	chr = CharacterFromID("Emily Vatterson");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	string sLocation = PChar.CaptainMonkey.BankID + "_ExitTown";
	CM_SetEncounters(sLocation, true);

	PChar.quest.CaptainMonkeyFromBankInExitTown.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyFromBankInExitTown.win_condition.l1.location = sLocation;
	PChar.quest.CaptainMonkeyFromBankInExitTown.function = "CaptainMonkeyFromBankInExitTown";
}

void CaptainMonkeyFromBankInExitTown(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Allen Gladstonn");
	chr.Dialog.CurrentNode = "11";
	ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto1");
	SetDialogCharacter2Character(PChar, chr, 25.0, false);
}

void CaptainMonkeyBattleWithAllen()
{
	ref chr = CharacterFromID("Allen Gladstonn");
	// Gregg: отказались от работы - Аллен кидается с ножом, значок над врагом ни к чему
	SetQuestMarkToCharacter(chr, false);
	chr.LifeDay = 0;
	LAi_SetWarriorType(chr);
	LAi_SetImmortal(chr, false);

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.CaptainMonkeyBattleWithAllenWin.win_condition.l1 = "NPC_Death";
	PChar.quest.CaptainMonkeyBattleWithAllenWin.win_condition.l1.character = "Allen Gladstonn";
	PChar.quest.CaptainMonkeyBattleWithAllenWin.function = "CaptainMonkeyBattleWithAllenWin";
}

void CaptainMonkeyBattleWithAllenWin(string qName)
{
	chrDisableReloadToLocation = false;
	CM_SetEncounters(PChar.location, false);

	PChar.CaptainMonkey = "7";
	CM_Record("9");

	// Gregg: GetIslandRandomShoreIdRand из К:ПС у нас нет - берег берём явно по острову
	string sShore = PChar.CaptainMonkey.IslandShoreID;

	ref chr = GetCharacter(NPC_GenerateCharacter("Black Boy", "slave4", "man", "man", 1, PIRATE, -1, false));
	chr.NotRemoved = true;
	chr.name = iTxt("CM_BoyName");
	chr.lastname = iTxt("CM_BoyLastName");
	chr.Dialog.FileName = "Quest\KPS\CaptainMonkey\CM_Allen.c";
	chr.Dialog.CurrentNode = "14";
	chr.greeting = "Gr_JohnDavy";
	LAi_SetCitizenType(chr);
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, sShore, "goto", "goto1");
}

void CaptainMonkeyFindContract()
{
	ref chr = CharacterFromID("Black Boy");
	chr.LifeDay = 0;
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	PChar.CaptainMonkey = "8";
	CM_Record("10");
	AddQuestUserData("CaptainMonkey", "sGrotIsland", CaptainMonkeyGetGrotIsland());

	string sGrot = PChar.CaptainMonkey.GrotID;
	// Gregg: и здесь глушим «Проклятого капитана» - сундук с договором открывают
	// ровно так же, а плыть до дальнего грота через полкарты
	LAi_LocationDisableToughSkeletonTimer(sGrot, 60);
	SetQuestBoxItems(sGrot, "CaptainMonkey_Contract", 1);

	CM_SetEncounters(sGrot, true);
	PChar.quest.CaptainMonkeyFindedContract.win_condition.l1 = "item";
	PChar.quest.CaptainMonkeyFindedContract.win_condition.l1.item = "CaptainMonkey_Contract";
	PChar.quest.CaptainMonkeyFindedContract.function = "CaptainMonkeyFindedContract";
}

void CaptainMonkeyFindedContract(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_SetActorType(PChar);

	// Gregg: контракт в руках - шаблон сундука больше не нужен. У него stay оставлен
	// намеренно: квестовый предмет иначе мог сгинуть при респауне сундука, если игрок
	// зашёл в грот, но не открыл его.
	ClearQuestBoxStay(PChar.CaptainMonkey.GrotID);

	float x, y, z;
	GetCharacterPos(PChar, &x, &y, &z);

	x += 0.5;
	CreateParticleSystem("fire", x, y, z, -1.57, 0, 0, 0);
	DoQuestFunctionDelay("CaptainMonkeyBurnContract", 2.0);
}

void CaptainMonkeyBurnContract(string _tmp)
{
	LAi_SetPlayerType(PChar);
	CM_Record("11");
	TakeNItems(PChar, "CaptainMonkey_Contract", -1);

	int iRank = sti(PChar.rank) + 5;
	ref chr;
	int iN;
	for (iN = 1; iN <= 7; iN++)
	{
		chr = CM_MakeSkeleton("Captain Monkey Contract Guard " + iN, iRank);
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);
	CM_SetNPCDeadFunction("CaptainMonkeyBurnContractWin", "CaptainMonkeyBurnContractWin", "Captain Monkey Contract Guard ", 7);
}

void CaptainMonkeyBurnContractWin(string qName)
{
	chrDisableReloadToLocation = false;
	CM_Record("12");

	PChar.CaptainMonkey = "9";
	CM_SetEncounters(PChar.location, false);

	string sLocation = PChar.CaptainMonkey.IslandID;
	AddQuestUserData("CaptainMonkey", "sIsland", XI_ConvertString(sLocation + "Gen"));

	PChar.quest.CaptainMonkeyLastBattle.win_condition.l1 = "location";
	PChar.quest.CaptainMonkeyLastBattle.win_condition.l1.location = sLocation;
	PChar.quest.CaptainMonkeyLastBattle.function = "CaptainMonkeyLastBattle";
}

void CaptainMonkeyLastBattle(string qName)
{
	bQuestDisableMapEnter = true;
	Island_SetReloadEnableGlobal(PChar.location, false);

	// Gregg: «Банановый рай» - галеон вместо несуществующего у нас SHIP_BATTLESHIP,
	// команда из обезьян-нежити. Капитан Глюк со вторым кораблём выброшен: он был
	// нужен только чтобы вернуть отобранный у игрока корабль, а мы его не отбираем.
	ref chr = CharacterFromID("Captain Monkey");
	LAi_SetImmortal(chr, false);
	chr.GenQuest.CrewMonkeyMode = true;
	FantomMakeCoolestSailor(chr, SHIP_GALEON_H, iTxt("CM_ShipName"), CANNON_TYPE_CANNON_LBS28, 100, 95, 95);
	SetFantomParamFromRank(chr, CM_MAUGLI_RANK, true);
	// Gregg: нация - как у колонии острова, иначе форт и сторожевики топят Маугли
	// раньше игрока. Пиратский вымпел по той же причине не вешаем - остаются только
	// рваные паруса.
	chr.nation = CM_GetIslandNation(PChar.CaptainMonkey.IslandID);
	SetShipSailsFromFile(chr, "ships\parus_common_torn.dds");
	// Gregg: одного Group_SetTaskAttack мало - вся морская ИИ-логика спрашивает
	// GetRelation, а он у дружественной нации возвращает «друг», и «Банановый рай»
	// мирно шёл рядом. Персональное отношение (nations.c) проверяется РАНЬШЕ
	// национального и на форт не влияет - но САМО ПО СЕБЕ НЕ ЖИВЁТ: при загрузке
	// корабля в море Ship_SetSeaAIType (AIShip.c:1020) стирает весь блок relation
	// у всех, кроме помеченных AlwaysEnemy/AlwaysFriend, и там же врагам отношение
	// проставляется заново. Поэтому обязателен именно атрибут.
	chr.AlwaysEnemy = true;
	SetCharacterRelationBoth(sti(chr.index), GetMainCharacterIndex(), RELATION_ENEMY);

	// Gregg: ==> статы «Бананового рая» поверх стоковых GALEON_H
	realships[sti(chr.ship.type)].HP = 7000;          // было 6200
	realships[sti(chr.ship.type)].TurnRate = 11.0;    // было 9.2
	realships[sti(chr.ship.type)].MaxCrew = 440;      // было 400
	realships[sti(chr.ship.type)].HullArmor = 30;     // класс 3 даёт 24, ставим как второму
	// Gregg: <== статы «Бананового рая»

	Group_FindOrCreateGroup("CaptainMonkey");
	Group_SetType("CaptainMonkey", "war");
	Group_AddCharacter("CaptainMonkey", "Captain Monkey");
	Group_SetGroupCommander("CaptainMonkey", "Captain Monkey");
	Group_SetTaskAttack("CaptainMonkey", PLAYER_GROUP);
	Group_SetAddress("CaptainMonkey", PChar.location, "quest_ships", "quest_ship_3");
	Group_LockTask("CaptainMonkey");

	PChar.quest.CaptainMonkeyLastBattleWin.win_condition.l1 = "Group_Death";
	PChar.quest.CaptainMonkeyLastBattleWin.win_condition.l1.Group = "CaptainMonkey";
	PChar.quest.CaptainMonkeyLastBattleWin.function = "CaptainMonkeyLastBattleWin";
}

void CaptainMonkeyLastBattleWin(string qName)
{
	bQuestDisableMapEnter = false;
	Island_SetReloadEnableGlobal(PChar.location, true);

	Group_DeleteGroup("CaptainMonkey");

	// Gregg: лапа выдаётся прямо в инвентарь, а не кладётся на тело: у Маугли
	// намеренно нет SaveItemsForDead, иначе игроку упал бы и макокраб ценой в 100 000
	TakeNItems(PChar, "MaugliPaw", 1);
	// LEO: каталог Items приходит из сейва - цену из initItems получат только новые партии. Ставим и здесь,
	//   чтобы на уже начатых лапа тоже не была квестовой (выкладывается, передаётся, продаётся).
	int iPaw = FindItem("MaugliPaw");
	if (iPaw >= 0) Items[iPaw].price = 15000;

	CM_Record("13");
	CM_ClearQuestBoxes();
	CloseQuestHeader("CaptainMonkey");
	pchar.questdata.CaptainMonkey = 1;
	UnlockAchievement("CaptainMonkey_quest", 3);
	PChar.CaptainMonkey = "END";
}
// Gregg: <== «Корабль обезьян»
