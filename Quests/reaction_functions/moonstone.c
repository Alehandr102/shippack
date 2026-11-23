// Gregg: запись журнала одной строкой - и сразу род ГГ. Функции в .txt не работают,
// поэтому в тексте записи стоят плейсхолдеры, а значения приходят отсюда.
// AddQuestUserData цепляется к ПОСЛЕДНЕЙ добавленной записи (quests.c:404), а не
// к квесту целиком, поэтому подстановку повторяем после КАЖДОЙ записи.
// sSex - обычное окончание, sSex1 - возвратная форма.
void MS_Record(string sRec)
{
	AddQuestRecord("MoonStone", sRec);
	AddQuestUserData("MoonStone", "sSex", GetSexPhrase("", "а"));
	AddQuestUserData("MoonStone", "sSex1", GetSexPhrase("ся", "ась"));
}


//==> Gregg: "Легенда о призрачном свете" (перенос из К:ПС, MoonStone)
// Продолжение "Помощи святой инквизиции": начинается сразу после гибели Натана Рича.
// Оригинал: Damned Destiny, PROGRAM\lugger\Quests.c. Имена функций сохранены.
//
// Отличия от оригинала:
//  - дом Натана Рича заменён на комнату таверны Бас-Тера, как и в первом квесте,
//    поэтому весь код восстановления двери reload.l26 в BasTer_town выброшен;
//  - хелперы DisableLandEncounters и FreeLocator в CSP отсутствуют: первый заменён
//    на HSI_SetEncounters из inquisition.c (тот квест перенесён раньше), второй
//    написан здесь локально;
//  - ранги врагов считаются напрямую от pchar.rank: GetRank() в CSP, в отличие от
//    К:ПС, сама прибавляет ранг ГГ внутри, и при передаче PChar он считался дважды.

// Gregg: освободить локатор - выкинуть из игры того, кто на нём стоит. В CSP есть
// только FreeSitLocator для сидячих мест, для goto/sit-базы аналога нет.
void MS_FreeLocator(string sLocation, string sLocator)
{
	int n;
	ref chr;
	for (n = 0; n < MAX_CHARACTERS; n++)
	{
		makeref(chr, Characters[n]);
		if (!CheckAttribute(chr, "location")) continue;
		if (chr.location != sLocation) continue;
		if (!CheckAttribute(chr, "location.locator")) continue;
		if (chr.location.locator != sLocator) continue;
		if (CheckAttribute(chr, "NotRemoved")) continue;
		if (n == nMainCharacterIndex) continue;

		LogoffCharacter(chr);
		return;
	}
}

void MoonStoneStart()
{
	// Gregg: амулет выдаётся здесь, в момент победы: по сюжету ГГ снимает его с тела.
	// На сам труп его НЕ класть - финал срабатывает по смерти Рича, раньше всякого
	// обыска, так что проверка инвентаря от задвоения не спасает: один амулет пришёл
	// бы отсюда и второй - с тела.
	TakeNItems(PChar, "MoonStoneAmulet", 1);

	SetQuestHeader("MoonStone");
	MS_Record("1");

	PChar.MoonStone = "1";
	// Gregg: десять дней на то, чтобы дойти до таверны Бермуд. Не успел - Клейн ушёл.
	SetTimerFunction("MoonStoneStartFailed", 0, 0, 10);
	SetQuestFunction("MoonStoneToBermudesTavern", "MoonStoneToBermudesTavern", "l", "Pirates_tavern");
}

void MoonStoneStartFailed(string qName)
{
	MS_Record("15");
	CloseQuestHeader("MoonStone");

	PChar.MoonStone = "END";
	PChar.quest.MoonStoneToBermudesTavern.over = "yes";
	// Gregg: в оригинале тут возвращалась дверь в дом Натана Рича. Дома нет - нечего и возвращать.
}

void MoonStoneToBermudesTavern(string qName)
{
	chrDisableReloadToLocation = true;
	MS_Record("2");
	PChar.quest.MoonStoneStartFailed.over = "yes";

	MS_FreeLocator("Pirates_tavern", "sit_base1");
	ref chr = GetCharacter(NPC_GenerateCharacter("Klein", "officer_3", "man", "man", 10, PIRATE, -1, true));
	chr.name = iTxt("MS_KleinName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\MoonStone\MoonStone.c";
	chr.Dialog.CurrentNode = "Claine_0";
	chr.greeting = "Gr_MiddPirate";
	LAi_SetImmortal(chr, true);
	LAi_SetSitType(chr);
	FreeSitLocator("Pirates_tavern", "sit_base1");
	ChangeCharacterAddressGroup(chr, "Pirates_tavern", "sit", "sit_base1");
	SetQuestMarkToCharacter(chr, true);
}

void MoonStoneToBermudesTavernTalk()
{
	LAi_Fade("", "");

	MS_FreeLocator("Pirates_tavern", "sit_front1");
	ChangeCharacterAddressGroup(PChar, "Pirates_tavern", "sit", "sit_front1");
	LAi_SetSitType(PChar);
}

void MoonStoneToFortFrance()
{
	chrDisableReloadToLocation = false;
	MS_Record("3");

	LAi_Fade("", "");
	ChangeCharacterAddressGroup(PChar, "Pirates_tavern", "goto", "goto1");
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("Klein");
	SetQuestMarkToCharacter(chr, false);
	ChangeCharacterAddressGroup(chr, "Pirates_tavern", "goto", "goto2");

	LAi_SetActorType(chr);
	LAi_ActorGoToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	// Gregg: Клейн едет с ГГ пассажиром до Мартиники
	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);

	SetQuestFunction("MoonStoneInFortFrance", "MoonStoneInFortFrance", "l", "FortFrance_town");
}

void MoonStoneInFortFrance(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Klein");
	chr.Dialog.CurrentNode = "Claine_5";
	RemovePassenger(PChar, chr);
	ChangeCharacterAddressGroup(chr, "FortFrance_town", "reload", "reload1_back");

	SetDialogCharacter2Character(PChar, chr, 60.0, false);
}

void MoonStoneExitFortFrance()
{
	chrDisableReloadToLocation = false;
	MS_Record("4");

	ref chr = CharacterFromID("Klein");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "gate_back", "none", "", "", "", -1);

	SetTimerFunction("MoonStoneFirstToBasTer", 0, 0, 7);
}

void MoonStoneFirstToBasTer(string qName)
{
	// Gregg: связной Клейна ждёт в той же комнате таверны, где в первом квесте
	// лежали письма Натана. Отдельного дома в CSP нет.
	SetQuestFunction("MoonStoneFirstInBasTer", "MoonStoneFirstInBasTer", "l", "BasTer_tavern_upstairs");
}

void MoonStoneFirstInBasTer(string qName)
{
	ref chr = GetCharacter(NPC_GenerateCharacter("Klein Man", "officer_16", "man", "man", 10, PIRATE, -1, true));
	chr.Dialog.FileName = "Quest\KPS\MoonStone\MoonStone.c";
	chr.Dialog.CurrentNode = "Claine_7";
	chr.greeting = "Gr_MiddPirate";
	LAi_SetImmortal(chr, true);
	LAi_SetStayType(chr);
	ChangeCharacterAddressGroup(chr, "BasTer_tavern_upstairs", "goto", "goto1");
}

void MoonStoneToSentJons()
{
	MS_Record("5");
	PChar.MoonStone = "2";

	SetQuestFunction("MoonStoneInSentJons", "MoonStoneInSentJons", "l", "SentJons_tavern");
}

void MoonStoneInSentJons(string qName)
{
	MS_FreeLocator("SentJons_tavern", "sit_base4");
	ref chr = GetCharacter(NPC_GenerateCharacter("Adam Kromvel", "officer_20", "man", "man", 10, PIRATE, -1, true));
	chr.name = iTxt("MS_KromvelName");
	chr.lastname = iTxt("MS_KromvelLastName");
	chr.Dialog.FileName = "Quest\KPS\MoonStone\MoonStone.c";
	chr.Dialog.CurrentNode = "Adam_Kromvel_1";
	chr.greeting = "Gr_Tavern_Mate";
	LAi_SetImmortal(chr, true);
	LAi_SetSitType(chr);
	ChangeCharacterAddressGroup(chr, "SentJons_tavern", "sit", "sit_base4");
	SetQuestMarkToCharacter(chr, true);
}

void MoonStoneToSentJonsExit()
{
	MS_Record("6");
	SetQuestFunction("MoonStoneInSentJonsExit", "MoonStoneInSentJonsExit", "l", "SentJons_ExitTown");
	HSI_SetEncounters("SentJons_ExitTown", true);
}

void MoonStoneInSentJonsExit(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Adam Kromvel");
	// Gregg: разговор в таверне позади, дальше Кромвель поджидает у выхода - значок снимаем
	SetQuestMarkToCharacter(chr, false);
	chr.Dialog.CurrentNode = "Adam_Kromvel_3";
	chr.greeting = "soldier_arest";
	chr.LifeDay = 0;

	int iRank = sti(pchar.rank) + 10;
	FantomMakeCoolFighter(chr, iRank, 100, 100, "", "", 350);
	LAi_NPC_Equip(chr, iRank, true, true);
	SetFoodToCharacter(chr, 5, 50);

	ChangeCharacterAddressGroup(chr, "SentJons_ExitTown", "goto", "goto5");
	SetDialogCharacter2Character(PChar, chr, 60.0, false);
}

void MoonStoneFightWithKromvel()
{
	ref chr = CharacterFromID("Adam Kromvel");
	LAi_SetImmortal(chr, false);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DeleteAttribute(pchar, "quest.MoonStoneFightWithKromvelWin");
	PChar.quest.MoonStoneFightWithKromvelWin.win_condition.l1 = "NPC_Death";
	PChar.quest.MoonStoneFightWithKromvelWin.win_condition.l1.character = "Adam Kromvel";
	PChar.quest.MoonStoneFightWithKromvelWin.function = "MoonStoneFightWithKromvelWin";
}

void MoonStoneFightWithKromvelWin(string qName)
{
	HSI_SetEncounters("SentJons_ExitTown", false);
	chrDisableReloadToLocation = false;
	MS_Record("7");
	TakeNItems(PChar, "HelpSacredInquisition_Letter3", 1);

	ref chr = CharacterFromID("Klein Man");
	chr.Dialog.CurrentNode = "Claine_11";

	// Gregg: убит бывший инквизитор, и молва это заметит. В К:ПС штраф был -15,
	// по решению автора мода смягчён вдвое: за квест таких убийств три подряд.
	ChangeCharacterReputation(PChar, -8);
}

void MoonStoneToNevis()
{
	MS_Record("8");
	PChar.MoonStone = "4";
}

void MoonStoneToNevisTavernRoom()
{
	PChar.MoonStone = "5";
	Locations[FindLocation("Charles_tavern")].reload.l2.disable = false;
	SetQuestFunction("MoonStoneInNevisTavernRoom", "MoonStoneInNevisTavernRoom", "l", "Charles_tavern_upstairs");
}

void MoonStoneInNevisTavernRoom(string qName)
{
	Locations[FindLocation("Charles_tavern")].reload.l2.disable = true;
	chrDisableReloadToLocation = true;

	int iRank = sti(pchar.rank) + 5;
	ref chr = GetCharacter(NPC_GenerateCharacter("Frensis Ajvo", "trader_6", "man", "man", iRank, PIRATE, 0, true));
	chr.name = iTxt("MS_AjvoName");
	chr.lastname = iTxt("MS_AjvoLastName");
	chr.Dialog.FileName = "Quest\KPS\MoonStone\MoonStone.c";
	chr.Dialog.CurrentNode = "Francis_Ayvo_2";
	chr.greeting = "Gr_Crazy";
	FantomMakeCoolFighter(chr, iRank, 100, 100, "", "", 50);
	LAi_NPC_Equip(chr, iRank, true, true);
	SetFoodToCharacter(chr, 5, 50);
	ChangeCharacterAddressGroup(chr, "Charles_tavern_upstairs", "goto", "goto2");

	LAi_SetActorType(chr);
	LAi_ActorAfraid(chr, PChar, false);
	LAi_SetImmortal(chr, true);

	DoQuestFunctionDelay("MoonStoneInNevisTavernRoomGuardianLogin", 8.0);
}

void MoonStoneInNevisTavernRoomGuardianLogin(string _tmp)
{
	int iRank = sti(pchar.rank) + 10;
	ref chr = GetCharacter(NPC_GenerateCharacter("Frensis Ajvo Guardian", "officer_2", "man", "man", iRank, PIRATE, 0, true));
	chr.Dialog.FileName = "Quest\KPS\MoonStone\MoonStone.c";
	chr.Dialog.CurrentNode = "Francis_Ayvo_1";
	chr.greeting = "soldier_arest";
	LAi_SetImmortal(chr, true);
	FantomMakeCoolFighter(chr, iRank, 100, 100, "", "", 350);
	LAi_NPC_Equip(chr, iRank, true, true);
	SetFoodToCharacter(chr, 5, 50);
	ChangeCharacterAddressGroup(chr, "Charles_tavern_upstairs", "reload", "reload1");
	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

void MoonStoneFightWithAjvoGardian()
{
	ref chr = CharacterFromID("Frensis Ajvo Guardian");
	LAi_SetImmortal(chr, false);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DeleteAttribute(pchar, "quest.MoonStoneFightWithAjvoGardianWin");
	PChar.quest.MoonStoneFightWithAjvoGardianWin.win_condition.l1 = "NPC_Death";
	PChar.quest.MoonStoneFightWithAjvoGardianWin.win_condition.l1.character = "Frensis Ajvo Guardian";
	PChar.quest.MoonStoneFightWithAjvoGardianWin.function = "MoonStoneFightWithAjvoGardianWin";
}

void MoonStoneFightWithAjvoGardianWin(string qName)
{
	ref chr = CharacterFromID("Frensis Ajvo");
	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

void MoonStoneFightWithAjvo()
{
	ref chr = CharacterFromID("Frensis Ajvo");
	LAi_SetImmortal(chr, false);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DeleteAttribute(pchar, "quest.MoonStoneFightWithAjvoWin");
	PChar.quest.MoonStoneFightWithAjvoWin.win_condition.l1 = "NPC_Death";
	PChar.quest.MoonStoneFightWithAjvoWin.win_condition.l1.character = "Frensis Ajvo";
	PChar.quest.MoonStoneFightWithAjvoWin.function = "MoonStoneFightWithAjvoWin";
}

void MoonStoneFightWithAjvoWin(string qName)
{
	// Gregg: Айво был безоружным учёным, за такое спрашивают строже. В К:ПС -20,
	// смягчено вдвое вместе с прочими штрафами квеста.
	ChangeCharacterReputation(PChar, -10);
	chrDisableReloadToLocation = false;

	MS_Record("9");
	PChar.MoonStone = "6";

	ref chr = CharacterFromID("Klein Man");
	chr.Dialog.CurrentNode = "Claine_15";
}

void MoonStoneFightWithKleinMan()
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Klein Man");
	chr.LifeDay = 0;
	chr.SaveItemsForDead = true;
	int iRank = sti(pchar.rank) + 10;
	FantomMakeCoolFighter(chr, iRank, 100, 100, "", "", 400);
	DeleteAttribute(chr, "items");
	chr.money = 25000;
	LAi_NPC_Equip(chr, iRank, true, true);
	SetFoodToCharacter(chr, 2, 15);
	LAi_SetImmortal(chr, false);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DeleteAttribute(pchar, "quest.MoonStoneFightWithKleinManWin");
	PChar.quest.MoonStoneFightWithKleinManWin.win_condition.l1 = "NPC_Death";
	PChar.quest.MoonStoneFightWithKleinManWin.win_condition.l1.character = "Klein Man";
	PChar.quest.MoonStoneFightWithKleinManWin.function = "MoonStoneFightWithKleinManWin";
}

void MoonStoneFightWithKleinManWin(string qName)
{
	chrDisableReloadToLocation = false;

	MS_Record("10");
	PChar.MoonStone = "7";
}

void MoonStoneToFortFranceExit()
{
	// Gregg: в оригинале тут возвращалась дверь в дом Натана Рича - дома нет.
	PChar.MoonStone = "8";
	MS_Record("11");

	HSI_SetEncounters("FortFrance_ExitTown", true);
	SetQuestFunction("MoonStoneInFortFranceExit", "MoonStoneInFortFranceExit", "l", "FortFrance_ExitTown");
}

void MoonStoneInFortFranceExit(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Klein");
	chr.Dialog.CurrentNode = "Claine_23";
	chr.LifeDay = 0;

	ChangeCharacterAddressGroup(chr, "FortFrance_ExitTown", "goto", "goto2");
	SetDialogCharacter2Character(PChar, chr, 60.0, false);
}

void MoonStoneToNevisIsland()
{
	chrDisableReloadToLocation = false;

	PChar.MoonStone = "9";
	MS_Record("12");

	ref chr = CharacterFromID("Klein");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	HSI_SetEncounters("FortFrance_ExitTown", false);
	SetQuestFunction("MoonStoneInNevisIsland", "MoonStoneInNevisIsland", "l", "Charles_Grot");
}

void MoonStoneInNevisIsland(string qName)
{
	chrDisableReloadToLocation = true;
	// Gregg: iRank остался для Педро и общей выдачи, Джейку ранг задаётся жёстко ниже
	int iRank = sti(pchar.rank) + 20;
	int iHP = 0;

	string sName = "";
	string sLastName = "";
	string sModel = "";
	string sId = "";
	string sLocator = "";
	bool bEquip = true;

	ref chr;
	// Gregg: счётчик локальный, а не общий i - внутри цикла зовутся чужие функции
	int iMan;
	for (iMan = 1; iMan <= 2; iMan++)
	{
		if (iMan == 1)
		{
			sId = "Jack Smitt";
			sName = iTxt("MS_SmittName");
			sLastName = iTxt("MS_SmittLastName");
			sModel = "officer_7";
			sLocator = "goto6";
			bEquip = true;
		}
		else
		{
			sId = "Pedro Carlos";
			sName = iTxt("MS_CarlosName");
			sLastName = iTxt("MS_CarlosLastName");
			sModel = "usurer_5";
			// Gregg: в К:ПС он стоял на goto4, но в патче ходьбы CSP под этой точкой нет
			// ни одного треугольника (как и под goto3) - персонаж оказывался вне проходимой
			// области и не мог ни подойти к игроку, ни уйти по приказу: движок ругался
			// "NPCharacter <Pedro Carlos> -> trace node not found". goto2 - ближайший
			// к прежнему месту локатор, лежащий на патче.
			sLocator = "goto2";
			bEquip = false;
		}

		chr = GetCharacter(NPC_GenerateCharacter(sId, sModel, "man", "man", 10, PIRATE, 0, bEquip));
		ChangeCharacterAddressGroup(chr, "Charles_Grot", "goto", sLocator);
		LAi_SetImmortal(chr, true);

		chr.Dialog.FileName = "Quest\KPS\MoonStone\MoonStone.c";
		chr.name = sName;
		chr.lastname = sLastName;

		if (iMan == 1)
		{
			chr.Dialog.CurrentNode = "Jack_Smitt_1";
			chr.greeting = "Gr_EvilPirate";
			chr.SaveItemsForDead = true;
			// Gregg: главарь братства - боец фиксированной силы, а не «ранг ГГ плюс двадцать».
			// Ранг и HP выставляем ПОСЛЕ вызова: FantomMakeCoolFighter прогоняет оба
			// параметра через GetCoffDiff (x0.6 / x1.0 / x1.4), и точные числа иначе не задать.
			FantomMakeCoolFighter(chr, 50, 100, 100, "", "", 0);
			chr.rank = 50;
			iHP = 900;
			if (MOD_SKILL_ENEMY_RATE == SAILOR_DIFFICULTY) iHP = 750;
			if (MOD_SKILL_ENEMY_RATE == ADMIRAL_DIFFICULTY) iHP = 1111;
			chr.chr_ai.hp = iHP;
			chr.chr_ai.hp_max = iHP;
			DeleteAttribute(chr, "items");
			chr.money = 50000;
			LAi_NPC_Equip(chr, iRank, true, true);
			SetFoodToCharacter(chr, 2, 15);
			SetDialogCharacter2Character(PChar, chr, 30.0, false);
		}
		else
		{
			chr.Dialog.CurrentNode = "Pedro_Carlos_1";
			chr.greeting = "Gr_Crazy";
			LAi_SetActorType(chr);
			// Gregg: canMove=false - стоит на месте и боится (ветка «только стоять»
			// в LAi_afraid.c). С true шаблон при каждом сближении с игроком перегонял
			// его на случайный goto-локатор, и весь бой с Джеком Педро бродил по гроту.
			LAi_ActorAfraid(chr, PChar, false);
		}
	}
}

void MoonStoneFightWithJackSmitt()
{
	ref chr = CharacterFromID("Jack Smitt");
	LAi_SetImmortal(chr, false);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	DeleteAttribute(pchar, "quest.MoonStoneFightWithJackSmittWin");
	PChar.quest.MoonStoneFightWithJackSmittWin.win_condition.l1 = "NPC_Death";
	PChar.quest.MoonStoneFightWithJackSmittWin.win_condition.l1.character = "Jack Smitt";
	PChar.quest.MoonStoneFightWithJackSmittWin.function = "MoonStoneFightWithJackSmittWin";
}

void MoonStoneFightWithJackSmittWin(string qName)
{
	ref chr = CharacterFromID("Pedro Carlos");
	// Gregg: третий параметр - таймаут подхода (уходит в LAi_tmpl_SetFollow): по его
	// истечении разговор начинается принудительно, где бы Педро ни был. В К:ПС стояло
	// 30 секунд, то есть полминуты тишины над трупом Джейка.
	SetDialogCharacter2Character(PChar, chr, 2.0, false);
}

void MoonStoneCarlosToExit()
{
	ref chr = CharacterFromID("Pedro Carlos");
	LAi_SetActorType(chr);
	// Gregg: выход из грота - reload1_back (Nevis.c, reload.l1 -> Charles_CaveEntrance),
	// и он лежит на патче. reload1 - это точка ПОЯВЛЕНИЯ при входе, она висит на 7 м
	// ниже патча: маршрут к ней не строится, и приказ вырождался в телепорт.
	// Таймаут держим конечным: при провале маршрута LAi_tmpl_runto_Teleport
	// (LAi_runto.c) срабатывает ТОЛЬКО при timeout >= 0, иначе шаблон встаёт
	// в состояние "falure" и персонаж застывает навсегда, без повторных попыток.
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 10.0);

	// Gregg: камень появляется в сундуке грота, квест закрывает сам факт взятия предмета.
	// Открытие сундука в локации типа "cave" с группой locators.monsters с шансом 50%
	// вызывает генератор "Проклятый капитан" (spawnToughSkeleton в items\itemLogic.c).
	// Грот подходит под оба условия, и скелет 20+ рангов посреди финала квесту лишний.
	// Штатная блокировка - таймер локации; ставим на сутки, до взятия камня хватит.
	LAi_LocationDisableToughSkeletonTimer("Charles_Grot", 1);
	SetQuestBoxItems("Charles_Grot", "MoonStone", 1);
	SetQuestFunction("MoonStoneHappyEnd", "MoonStoneHappyEnd", "i", "MoonStone");

	// Gregg: пауза до перезагрузки грота - она нужна, чтобы SetQuestBoxItems выше
	// доехал до сундука. В К:ПС тут было 10 секунд глухой тишины, игрок успевал
	// решить, что квест завис.
	DoQuestFunctionDelay("MoonStoneReload", 2.0);
}

void MoonStoneReload(string _tmp)
{
	MS_Record("13");

	// Gregg: страховка на случай, если Педро к этому моменту ещё не вышел сам -
	// перезагрузка создала бы его заново по прежнему адресу. Адрес правим ПРЯМОЙ
	// записью: ChangeCharacterAddressGroup(chr, "none", ...) для этого не годится -
	// FindLocation не знает локации "none", функция уходит в морскую ветку, делает
	// LogoffCharacter и возвращает false, НЕ изменив адрес (characters_login.c).
	// Идиома проекта - LogoffCharacter плюс запись location вручную (образец -
	// BS_Final_1.c). Приказ бежать снимаем, иначе шаблон вернёт его обратно.
	int iCarlos = GetCharacterIndex("Pedro Carlos");
	if (iCarlos != -1)
	{
		ref rCarlos = &characters[iCarlos];
		LAi_SetStayType(rCarlos);
		LogoffCharacter(rCarlos);
		rCarlos.location = "none";
		rCarlos.LifeDay = 0;
	}
	DoQuestReloadToLocation("Charles_Grot", "goto", "goto6", "");
	chrDisableReloadToLocation = false;
}

void MoonStoneHappyEnd(string qName)
{
	MS_Record("14");
	CloseQuestHeader("MoonStone");
	pchar.questdata.MoonStone = 1;
	UnlockAchievement("MoonStone_quest", 3);
	PChar.MoonStone = "END";

	// Gregg: амулет отработал - он был опознавательным знаком для Клейна
	TakeNItems(PChar, "MoonStoneAmulet", -1);

	ClearQuestBoxStay("Charles_Grot");
}
//<== Gregg: "Легенда о призрачном свете"
