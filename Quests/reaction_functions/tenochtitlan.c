// Gregg: вынесено из reaction_functions.c (сплит god-файла) — секция «Линейка Теночтитлана».
// #include на исходное место — видимость и поведение идентичны.

void Teno_startInShore(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_LocationFightDisable(loadedLocation, true);
	sld = GetCharacter(NPC_GenerateCharacter("QuestMan_Shore", "shipowner_1", "man", "man", 100, PIRATE, 0, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_148", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_149", "Names.txt", "#space#", " ");
	sld.dialog.filename   = "Quest\Tenochtitlan\ForAll_Teno.c";
	sld.dialog.currentnode   = "Shore53";
	ChangeCharacterAddressGroup(sld, "Shore53", "goto", "goto7");
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", 5.0, 0);
}

void Teno_startInShore_2()
{
	pchar.questTemp.Teno = "toMontesuma"; //флаг квеста
	sld = characterFromId("QuestMan_Shore");
	LAi_SetActorType(sld);
	LAi_ActorRunToLocation(sld, "reload", "sea", "none", "", "", "CanFightCurLocation", 10.0);
	//Ставим вождя
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("Montesuma", "Aztec", "man", "man_fast", 50, PIRATE, -1, true)); // LEO: Превозмогаторам - страдать 01.12.2021
	else sld = GetCharacter(NPC_GenerateCharacter("Montesuma", "Aztec", "man", "man", 50, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 50, 100, 100, "toporAZ", "", 500);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_150", "Names.txt", "#space#", " ");
	sld.lastname = "";
	sld.location = "Tenochtitlan";
	sld.location.group = "quest";
	sld.location.locator = "quest2";
	sld.dialog.filename   = "Quest\Tenochtitlan\Montesuma.c";
	sld.dialog.currentnode   = "First time";
	sld.greeting = "Gr_Montesuma";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	sld.reactionOnFightModeOn = true;
	sld.BreakTmplAndFight = true;
	sld.SaveItemsForDead = true;
	sld.DontClearDead = true;
	LAi_SetWarriorType(sld);
	LAi_warrior_SetStay(sld, true);
	LAi_RemoveLoginTime(sld);
	LAi_SetImmortal(sld, true);
	//GiveItem2Character(sld, "Totem_15");
	LAi_group_MoveCharacter(sld, "MontesumaGroup");
	//ставим прерывание на вход в Теночтитлан
	pchar.quest.Teno_createAztecTwin.win_condition.l1 = "location";
	pchar.quest.Teno_createAztecTwin.win_condition.l1.location = "Tenochtitlan";
	pchar.quest.Teno_createAztecTwin.function = "Teno_createAztecTwin";

	pchar.quest.Teno_talkWithTwin.win_condition.l1 = "locator";
	pchar.quest.Teno_talkWithTwin.win_condition.l1.location = "Tenochtitlan";
	pchar.quest.Teno_talkWithTwin.win_condition.l1.locator_group = "teleport";
	pchar.quest.Teno_talkWithTwin.win_condition.l1.locator = "fire20";
	pchar.quest.Teno_talkWithTwin.function = "Teno_talkWithTwin";
	//открываем квест в СЖ
	SetQuestHeader("Tenochtitlan");
	AddQuestRecord("Tenochtitlan", "1");
	AddQuestUserData("Tenochtitlan", "sSex", GetSexPhrase("","а"));
}

//ставим ацтеков граждан
void Teno_createAztecTwin(string qName)
{
	//генерим близнецов
	sld = GetCharacter(NPC_GenerateCharacter("AztecCitizen_1", "AztecCitizen1", "man", "man", 15, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_151", "Names.txt", "#space#", " ");
	sld.lastname = "";
	sld.dialog.filename   = "Quest\Tenochtitlan\AztecCitizens.c";
	sld.dialog.currentnode   = "AztecFT";
	sld.greeting = "cit_common";
	LAi_RemoveLoginTime(sld);
	LAi_SetImmortal(sld, true);
	LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "AztecCitizenGroup");
	ChangeCharacterAddressGroup(sld, "Tenochtitlan", "goto", "goto23");

	sld = GetCharacter(NPC_GenerateCharacter("AztecCitizen_2", "AztecCitizen2", "man", "man", 15, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_152", "Names.txt", "#space#", " ");
	sld.lastname = "";
	sld.dialog.filename   = "Quest\Tenochtitlan\AztecCitizens.c";
	sld.dialog.currentnode   = "AztecFT2";
	sld.greeting = "cit_common";
	LAi_SetLoginTime(sld, 7.0, 20.0);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "AztecCitizenGroup");
	ChangeCharacterAddressGroup(sld, "Tenochtitlan", "goto", "goto14");
	//Монтесуму на ГГ
	sld = characterFromId("Montesuma");
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", 5.0, 0);
}

void Teno_talkWithTwin(string qName)
{
	sld = characterFromId("AztecCitizen_1");
	LAi_ActorDialog(sld, pchar, "", 2.5, 0);
}

void Teno_MontesumaGoQuest1()
{
	AddQuestRecord("Tenochtitlan", "2");
	AddQuestUserData("Tenochtitlan", "sSex", GetSexPhrase("","а"));
	sld = characterFromId("Montesuma");
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorRunToLocator(sld, "quest", "quest1", "Teno_MontesumaArrived", -1);
}

// Gregg: вся стража Великого храма выбита - ставим флаг зачистки, он открывает нефритовый череп
// (QuestCheckCanTakeItem). Флаг снимается при каждой новой генерации стражи в CreatTenochtitlanInside,
// то есть держится ровно один визит. Взводится через LAi_group_SetCheckFunction, чек одноразовый.
void Teno_TempleGuardsDead(string sFunc)
{
	ref rLoc = &locations[FindLocation("Temple_great")];
	if (CheckAttribute(rLoc, "TenoGuardsCleared")) return; // уже отмечено (напр. добили бога в той же группе)
	rLoc.TenoGuardsCleared = true;
	Log_Info(GetConvertStrWithReplace("Variable_teno_temple_cleared", "Logs.txt", "#space#", " "));
}

// Gregg: сколько стражей храма ещё на ногах - только для подсказки в логе при отказе взять череп.
// Считаем по детерминированным ID генератора: 7 групп по TenoGuardsNum (см. CreatTenochtitlanInside).
// Через LAi_loginedcharacters[] не идём - массив локален LAi_login.c.
int Teno_CountAliveGuards()
{
	ref rLoc = &locations[FindLocation("Temple_great")];
	if (!CheckAttribute(rLoc, "TenoGuardsNum")) return 0;
	int iQty = sti(rLoc.TenoGuardsNum);
	int iAlive = 0;
	int idx, n, i;
	ref ch;
	for (n = 1; n <= 7; n++)
	{
		for (i = 0; i < iQty; i++)
		{
			idx = GetCharacterIndex("AztecWarrior"+rLoc.index+"_"+n+""+i);
			if (idx < 0) continue;
			ch = GetCharacter(idx);
			if (LAi_IsDead(ch)) continue;
			iAlive++;
		}
	}
	return iAlive;
}

void Teno_fightInTemple()
{
	LAi_group_Delete("EnemyFight");
	LAi_LocationFightDisable(loadedLocation, false);
	LAi_SetFightMode(pchar, true);
	LAi_group_SetRelation(LAI_GROUP_MONSTERS, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	sld = characterFromId("DeadmansGod");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "teno_monsters_group");
	LAi_group_SetRelation("teno_monsters_group", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("teno_monsters_group", LAI_GROUP_PLAYER, true);
	// Gregg: вместо единственного чека на смерть - цепочка фаз 3/4 -> 1/2 -> 1/4 -> смерть.
	// Порог считаем в отдельную переменную, а не инлайном в аргумент - Storm на составных
	// выражениях внутри вызова ведёт себя непредсказуемо (компиляция проходит, рантайм падает).
	float fGodMaxHP = LAi_GetCharacterMaxHP(sld);
	LAi_SetCheckMinHP(sld, fGodMaxHP * 0.75, false, "Teno_MiktlanRaise");
	// Страховка на случай, если бога убьют в обход фаз одним ударом - см. Teno_GodDiedEarly
	pchar.quest.Teno_GodDiedEarly.win_condition.l1 = "NPC_Death";
	pchar.quest.Teno_GodDiedEarly.win_condition.l1.character = "DeadmansGod";
	pchar.quest.Teno_GodDiedEarly.function = "Teno_GodDiedEarly";
}

// Gregg: фазы боя с Миктлантекутли.
// LAi_ProcessCheckMinHP снимает чек при срабатывании, поэтому каждая фаза перевзводит следующую -
// схема Салазара (SpawnSalasarSupports). Диспетч идёт через QuestComplete, отсюда кейсы
// "Teno_Miktlan*" в quests_reaction.c, которые просто зовут эти функции.

// 3/4 HP: бог поднимает павшую стражу и уходит в погребальный обряд - 15 секунд неуязвим.
// Бить его в это время бесполезно, надо разбираться с поднятыми.
void Teno_MiktlanRaise()
{
	int idx = GetCharacterIndex("DeadmansGod");
	if (idx < 0) return;
	ref rGod = GetCharacter(idx);
	ref rRisen;
	int i;
	int iRank = sti(pchar.rank);
	if (iRank < 20) iRank = 20;
	Log_Info(GetConvertStrWithReplace("Variable_teno_miktlan_raise", "Logs.txt", "#space#", " "));
	LAi_CharacterPlaySound(rGod, "DeadmansGod");
	LAi_SetImmortal(rGod, true);
	for (i = 0; i < 4; i++)
	{
		rRisen = GetCharacter(NPC_GenerateCharacter("MiktlanRisen_"+i, "AztecWarrior"+(rand(4)+1), "skeleton", "man", iRank, PIRATE, 0, true));
		FantomMakeCoolFighter(rRisen, iRank, 90, 90, "toporAZ", "", 100);
		LAi_SetHP(rRisen, 200, 200); // ПОСЛЕ FantomMakeCoolFighter - тот прибавляет своё
		LAi_SetCurHPMax(rRisen);
		LAi_SetWarriorType(rRisen);
		// та же группа, что у бога: внутри одной группы урон не проходит, поднятые его не заденут
		// и стравить их на него нельзя (см. LAi_ApplyCharacterAttackDamage)
		LAi_group_MoveCharacter(rRisen, "teno_monsters_group");
		// Локаторы ЯВНО, а не поиском ближайшего. Группа monsters в храме разложена на трёх ярусах
		// (проверено в игре: ацтеки прибегали сверху), а бой идёт на нижнем, у алтаря.
		// LAi_FindNearestFreeLocator меряет по прямой и выбирал точки этажом выше - воскресшие
		// появлялись за перекрытием и бежали вниз кругом. monster61..64 лежат в ряд на ярусе боя.
		ChangeCharacterAddressGroup(rRisen, "Temple_great", "monsters", "monster6" + (i + 1));
		LAi_SetFightMode(rRisen, true);
	}
	// Канал фиксированной длины - одноразовый таймер, без цепочки перевзводов
	DoQuestFunctionDelay("Teno_MiktlanChannelEnd", 15.0);
	float fGodMaxHP = LAi_GetCharacterMaxHP(rGod);
	LAi_SetCheckMinHP(rGod, fGodMaxHP * 0.5, false, "Teno_MiktlanSeal");
}

// Обряд окончен - бог снова уязвим (поднятые остаются)
void Teno_MiktlanChannelEnd(string qName)
{
	int idx = GetCharacterIndex("DeadmansGod");
	if (idx < 0) return;
	ref rGod = GetCharacter(idx);
	LAi_SetImmortal(rGod, false);
	Log_Info(GetConvertStrWithReplace("Variable_teno_miktlan_open", "Logs.txt", "#space#", " "));
}

// 1/2 HP: печать смерти - тяжёлый яд и кровоток. Оба эффекта самозатухающие и переживают
// сейв/лоад, в отличие от ручной блокировки регенерации.
void Teno_MiktlanSeal()
{
	int idx = GetCharacterIndex("DeadmansGod");
	if (idx < 0) return;
	ref rGod = GetCharacter(idx);
	Log_Info(GetConvertStrWithReplace("Variable_teno_miktlan_seal", "Logs.txt", "#space#", " "));
	LAi_CharacterPlaySound(rGod, "DeadmansGod");
	MakePoisonAttack(rGod, pchar, LAi_CalcPoisonDuration(1));
	MakeBloodingAttack(pchar, rGod, 10.0);
	float fGodMaxHP = LAi_GetCharacterMaxHP(rGod);
	LAi_SetCheckMinHP(rGod, fGodMaxHP * 0.25, false, "Teno_MiktlanFury");
}

// 1/4 HP: обсидиановая ярость - бог перестаёт защищаться. Ранг роняем со 100 до 70, и делитель
// урона 45/rank смягчается с x0.45 до x0.64 - последняя четверть тает примерно в полтора раза
// быстрее остального пула. Взамен он не дёргается от попаданий и пробивает блок насквозь.
void Teno_MiktlanFury()
{
	int idx = GetCharacterIndex("DeadmansGod");
	if (idx < 0) return;
	ref rGod = GetCharacter(idx);
	Log_Info(GetConvertStrWithReplace("Variable_teno_miktlan_fury", "Logs.txt", "#space#", " "));
	LAi_CharacterPlaySound(rGod, "DeadmansGod");
	// Полный бак энергии: от неё зависит темп атак и блоков (Lai_CharacterChangeEnergy,
	// GetEnergyBladeDrain), к финалу он её просаживает. Ярость должна быть видна в натиске.
	rGod.chr_ai.energy = LAi_GetCharacterMaxEnergy(rGod);
	rGod.rank = 70;
	// Сопротивление огнестрелу держим прежним, добавляем пропуск флинча и сквозное пробитие блока.
	// В этой фазе клинок почти догоняет пулю (45/70 = x0.64 против x0.67 у огнестрела) - на
	// добивании подойти вплотную уже не хуже, чем отстреливаться.
	SetBossDefense(rGod, 0.0, 0.33, 100, true);
	rGod.nodisarm = true;
	rGod.chr_ai.special.valueCrB = 40; // клинок перезаписывает specials только при экипировке
	LAi_SetCheckMinHP(rGod, 5.0, true, "Teno_youWinFight");
}

// Gregg: СТРАХОВКА ОТ СОФТЛОКА. Фазовые LAi_SetCheckMinHP не спасают от удара, который сносит бога
// с уровня выше порога сразу в ноль: смерть считается СИНХРОННО в момент удара
// (LAi_CheckKillCharacter вызывается прямо из LAi_ApplyCharacterAttackDamage), а обработчик фазы
// уходит в LAi_QuestDelay и срабатывает лишь следующим кадром. Терминальный чек при этом взводится
// только в последней фазе, запасного обработчика смерти у бога не было, а chrDisableReloadToLocation
// снимается лишь в победной ноде диалога (WinInTemple_6) - игрок оставался заперт в храме навсегда,
// и загрузка сейва не помогала. Вешаем NPC_Death: в штатном ходе боя он НЕ срабатывает, потому что
// терминальный чек стоит с immortal=true и бог замирает на 5 HP, не умирая. Только аварийный путь.
void Teno_GodDiedEarly(string qName)
{
	if (CheckAttribute(pchar, "questTemp.Teno"))
	{
		if (pchar.questTemp.Teno == "YouWinGod") return; // штатная победа уже отработала
	}
	int i, iTemp;
	ref sld;
	pchar.questTemp.Teno = "YouWinGod";
	chrDisableReloadToLocation = false; // главное: выпускаем игрока из храма
	LAi_SetFightMode(pchar, false);
	for (i = 0; i < 4; i++)
	{
		iTemp = GetCharacterIndex("MiktlanRisen_"+i);
		if (iTemp != -1) LAi_KillCharacter(&characters[iTemp]);
	}
	iTemp = GetCharacterIndex("AztecCitizen_1");
	if (iTemp != -1) characters[iTemp].dialog.currentnode = "Aztec1AF";
	iTemp = GetCharacterIndex("AztecCitizen_2");
	if (iTemp != -1) characters[iTemp].dialog.currentnode = "Aztec2AF";
	LocatorReloadEnterDisable("Tenochtitlan", "reload1_back", true);
	iTemp = GetCharacterIndex("Montesuma");
	if (iTemp != -1)
	{
		sld = GetCharacter(iTemp);
		sld.lastname = "II";
		sld.dialog.currentnode = "AfterGTemple";
		DeleteAttribute(sld, "reactionOnFightModeOn");
		DeleteAttribute(sld, "BreakTmplAndFight");
		LAi_SetActorType(sld);
		LAi_ActorStay(sld);
		ChangeCharacterAddressGroup(sld, "Tenochtitlan", "teleport", "fire1");
	}
	pchar.quest.Teno_exitFromTeno.win_condition.l1 = "locator";
	pchar.quest.Teno_exitFromTeno.win_condition.l1.location = "Tenochtitlan";
	pchar.quest.Teno_exitFromTeno.win_condition.l1.locator_group = "teleport";
	pchar.quest.Teno_exitFromTeno.win_condition.l1.locator = "fire1";
	pchar.quest.Teno_exitFromTeno.function = "Teno_exitFromTeno";
	AddQuestRecord("Tenochtitlan", "4");
	AddQuestUserData("Tenochtitlan", "sSex", GetSexPhrase("","а"));
	Log_Info(GetConvertStrWithReplace("Variable_teno_god_fell", "Logs.txt", "#space#", " "));
}

void Teno_exitFromTeno(string qName)
{
	sld = characterFromId("Montesuma");
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", 0.0, 0);
}

void Teno_clearGroups(string qName)
{
	LAi_group_Delete("AztecCitizenGroup");
	LAi_group_Delete("MontesumaGroup");
	LAi_group_Delete("teno_monsters_group");
	//ChangeItemDescribe("SkullAztec", "itmdescr_SkullAztecAdd");
	AddQuestRecord("Tenochtitlan", "6");
	CloseQuestHeader("Tenochtitlan");
	pchar.questdata.Teno = 1;
}