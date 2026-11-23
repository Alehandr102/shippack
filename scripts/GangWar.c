//=====================================================================================================================================
// Джунглевое событие "Стычка банды и патруля" (битва бандитов против военного патруля)
// 10 бандитов vs 15 солдат (офицер + 10 рукопашников + 4 мушкетёра). Обе группы нейтральны к ГГ и дерутся между собой.
// Спавнится только в локациях, где могут появиться И патруль, И бандиты.
// ЭТАП 1: спавн, бой групп, очистка при уходе (если ГГ не вмешался), таймер-страховка.
// ЭТАП 2 (позже): исход боя, награды, истории, отпор победителя.
//=====================================================================================================================================

// Удалить персонажей группы по префиксу id
void GangWar_RemoveGroupChars(string sPrefix, int iQty)
{
	int i;
	ref m;
	for (i = 0; i < iQty; i++)
	{
		if (GetCharacterIndex(sPrefix + i) == -1) continue;
		m = CharacterFromID(sPrefix + i);
		LAi_RemoveCheckMinHP(m);
		DeleteAttribute(m, "items"); // на всякий случай - чтобы при уборке трупа не осталось лута
		m.money = 0;
		m.LifeDay = 0; // пометить на удаление
	}
}

// Снять лут с павших в стычке (трупы обыскиваются через items+money) - игрок не наживается на чужой битве
void GangWar_StripDeadLoot(string sPrefix, int iQty)
{
	int i;
	ref m;
	for (i = 0; i < iQty; i++)
	{
		if (GetCharacterIndex(sPrefix + i) == -1) continue;
		m = CharacterFromID(sPrefix + i);
		if (!LAi_isDead(m)) continue; // живых не трогаем - выжившие уходят со своим
		DeleteAttribute(m, "items"); // очищает в т.ч. экипированный клинок/мушкет и items.gold
		m.money = 0;
		if (CheckAttribute(m, "bGangLeader")) { DeleteAttribute(m, "bGangLeader"); UnmarkCharacter(m); } // снять знак с погибшего вожака
	}
}

// Запомнить дело ГГ в стычке для таверных слухов (сбрасывает счётчик пересказов - новость свежая)
// LEO: слух ходит только по тавернам острова стычки (раньше - по всему архипелагу, и каждая стычка обнуляла
//   счётчик пересказов). То же дело на том же острове - не новость, счётчик не сбрасываем.
void GangWar_SetRumor(string sDeed)
{
	string sIsland = "";
	int iLoc = FindLocation(pchar.location);
	if (iLoc >= 0)
	{
		if (CheckAttribute(&Locations[iLoc], "islandId")) sIsland = Locations[iLoc].islandId;
	}
	if (CheckAttribute(pchar, "GangWarRumor") && CheckAttribute(pchar, "GangWarRumorIsland"))
	{
		if (pchar.GangWarRumor == sDeed && pchar.GangWarRumorIsland == sIsland) return;
	}
	pchar.GangWarRumorIsland = sIsland;
	pchar.GangWarRumor = sDeed;
	pchar.GangWarRumorDay = DateToInt(0); // дата события: через месяц быль быльём порастёт (фикс вечного слуха)
	DeleteAttribute(pchar, "GangWarRumorTold");
}

// Полная очистка события
void GangWar_ClearAll()
{
	if (!CheckAttribute(pchar, "GangWar.active")) return;
	string sIdx = pchar.GangWar.idx;
	GangWar_RemoveGroupChars("GWband_" + sIdx + "_", 10);
	GangWar_RemoveGroupChars("GWsold_" + sIdx + "_", 15);
	LAi_group_RemoveCheck("GWband_" + sIdx);
	LAi_group_Delete("GWband_" + sIdx);
	LAi_group_RemoveCheck("GWsold_" + sIdx);
	LAi_group_Delete("GWsold_" + sIdx);
	DeleteGameModeCondition("GangWar_Watch");
	string sQ = "GWover_" + sIdx;
	if (CheckAttribute(pchar, "quest." + sQ)) pchar.quest.(sQ).over = "yes";
	DeleteAttribute(pchar, "GangWar");
}

// Очистка при уходе с локации, если ГГ не вмешался (вызывается в начале LAi_CreateEncounters)
void GangWar_CheckExitCleanup(ref location)
{
	if (!CheckAttribute(pchar, "GangWar.active")) return;
	if (pchar.GangWar.loc == location.id) return;     // мы всё ещё в локации события
	if (CheckAttribute(pchar, "GangWar.side")) return; // ГГ вмешался - чистит исход/таймер
	GangWar_ClearAll();
}

// LEO: Сейв хранит бой и вражду групп. Восстанавливаем проверку победы и подход вожака с наградой.
void GangWar_RestoreOnLoad(ref location)
{
	// LEO: В старых сохранениях могла остаться покадровая проверка по вражде групп.
	DeleteGameModeCondition("GangWar_Watch");
	if (!CheckAttribute(pchar, "GangWar.active")) return;
	if (pchar.GangWar.loc != location.id) return; // не в локации события

	string sIdx = pchar.GangWar.idx;
	string sBandGroup = "GWband_" + sIdx;
	string sSoldGroup = "GWsold_" + sIdx;

	// --- СЛУЧАЙ А: бой ещё идёт - вернуть только ДЕТЕКТОРЫ исхода (сам бой/вражду групп не трогаем, они живы) ---
	if (!CheckAttribute(pchar, "GangWar.winner"))
	{
		LAi_group_SetCheck(sBandGroup, "GangWar_BandWiped"); // гибель банды -> OnVictory
		LAi_group_SetCheck(sSoldGroup, "GangWar_SoldWiped"); // гибель патруля -> OnVictory
		return;
	}

	// --- СЛУЧАЙ Б: бой кончен, награда ещё НЕ получена (resolved=="thanks") - восстановить благодарность вожака ---
	// КОРЕНЬ БАГА (подтверждён трассировкой загрузки): этот хук бежит из LAi_CharacterPostLogin, а СРАЗУ после него
	// LocAi_PostInit вызывает LAi_PostLoginInit для КАЖДОГО перса -> реинит актёра из сейва -> LAi_type_actor_Init
	// форсит STAY-режим и ЗАТИРАЕТ "dialoggo"/follow, выставленные LAi_ActorDialog. Поэтому вожак не подбегает,
	// а рантайм-контекст диалога актёра сбит -> разговор не приводит к награде. (Так же на этом пути поступает
	// SpySeeker - откладывает actor-dialog.) РЕШЕНИЕ: персистентную ноду ставим сразу, а актёра+подход - ОТЛОЖЕННО.
	if (CheckAttribute(pchar, "GangWar.resolved") && pchar.GangWar.resolved == "thanks")
	{
		string sWinPrefix;
		if (pchar.GangWar.winner == "bandits") sWinPrefix = "GWband_" + sIdx + "_";
		else                                   sWinPrefix = "GWsold_" + sIdx + "_";
		string sLeaderId = sWinPrefix + "0";
		if (GetCharacterIndex(sLeaderId) == -1) return;
		ref ld = CharacterFromID(sLeaderId);
		if (LAi_isDead(ld)) return;
		ld.dialog.filename = "Enc_GangWar.c";
		if (pchar.GangWar.winner == "patrol") ld.dialog.currentnode = "ThankPatrol";
		else                                  ld.dialog.currentnode = "ThankBandits";
		ld.greeting = "";
		pchar.GangWar.restoreLeaderId = sLeaderId;
		DoQuestFunctionDelay("GangWar_RestoreActor", 1.0); // через кадр (после LAi_PostLoginInit) -> dialoggo/follow выживет
	}
}

// Отложенный перезапуск благодарности вожака (вызывается DoQuestFunctionDelay из Case Б, уже ПОСЛЕ LAi_PostLoginInit).
// Только теперь актёрское состояние "dialoggo"/follow не затирается реинитом -> вожак подбегает и заводит благодарность,
// диалог открывается на ноде ThankPatrol/ThankBandits -> срабатывает награда (GangWar_RewardPatrol/RewardBandits).
void GangWar_RestoreActor(string qName) // qName - имя отложенного вызова (механизм DoQuestFunctionDelay зовёт `call sFunction(name)`)
{
	if (!CheckAttribute(pchar, "GangWar.restoreLeaderId")) return;
	string sLeaderId = pchar.GangWar.restoreLeaderId;
	DeleteAttribute(pchar, "GangWar.restoreLeaderId");
	if (!CheckAttribute(pchar, "GangWar.active")) return;
	if (!CheckAttribute(pchar, "GangWar.resolved") || pchar.GangWar.resolved != "thanks") return; // уже награждён/иной исход
	if (GetCharacterIndex(sLeaderId) == -1) return;
	ref ld = CharacterFromID(sLeaderId);
	if (LAi_isDead(ld)) return;
	ld.dialog.filename = "Enc_GangWar.c";
	if (pchar.GangWar.winner == "patrol") ld.dialog.currentnode = "ThankPatrol";
	else                                  ld.dialog.currentnode = "ThankBandits";
	ld.greeting = "";
	LAi_SetActorTypeNoGroup(ld);
	LAi_ActorDialog(ld, pchar, "", 25.0, 0); // подбегает; PostLoginInit уже отработал -> не затрётся
}

string GangWar_AttackSide(aref target)
{
	if (!CheckAttribute(pchar, "GangWar.active") || !CheckAttribute(target, "id")) return "";
	string suffix = "_" + pchar.GangWar.idx + "_";
	if (findsubstr(target.id, "GWband" + suffix, 0) == 0) return "patrol";
	if (findsubstr(target.id, "GWsold" + suffix, 0) == 0) return "bandits";
	return "";
}

// LEO: Сторона определяется атакой игрока до расчёта урона, в том числе при убийстве первым ударом.
void GangWar_RecordAttack(string side)
{
	if (side == "" || CheckAttribute(pchar, "GangWar.winner")) return;
	if (!CheckAttribute(pchar, "GangWar.side")) pchar.GangWar.side = side;
	else if (pchar.GangWar.side != side) pchar.GangWar.hitBoth = true;
}

// Создание события (вызывается из LAi_CreateEncounters, case новой стычки)
void GangWar_CreateEncounter(ref location, string encGroup, int iNation, string sCity)
{
	aref grp;
	string str, locBand, locSold;
	int i, iRankB, iRankS, numLoc;
	ref chr;

	str = "locators." + encGroup;
	makearef(grp, location.(str));
	numLoc = GetAttributesNum(grp);
	if (numLoc <= 0) return;

	// если осталась незавершённая прошлая стычка (ГГ вмешался и ушёл в тот же день, до срабатывания таймера) -
	// полностью её свернуть, иначе старые side/winner/resolved перетекут в новое событие и сломают исход (пустой dialog.filename)
	if (CheckAttribute(pchar, "GangWar.active")) GangWar_ClearAll();

	// два локатора-кластера: бандиты и солдаты порознь, сойдутся и сцепятся
	locBand = GetAttributeName(GetAttributeN(grp, 0));
	if (numLoc > 1) locSold = GetAttributeName(GetAttributeN(grp, 1));
	else            locSold = locBand;

	string sIdx = location.index;
	string sBandGroup = "GWband_" + sIdx;
	string sSoldGroup = "GWsold_" + sIdx;
	LAi_group_Register(sBandGroup);
	LAi_group_Register(sSoldGroup);

	// --- ранги как в штатных событиях ---
	// бандиты (как рейдеры, case 0)
	if (sti(pchar.rank) > 6 || MOD_SKILL_ENEMY_RATE == 3)
	{
		if (sti(pchar.rank) > 20) iRankB = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*3*2.5);
		else iRankB = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*3*1.5);
	}
	else iRankB = sti(pchar.rank);
	// солдаты (как патруль, case 3)
	if (sti(pchar.rank) > 14) iRankS = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*3*2.5);
	else iRankS = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*3*1.5);

	// --- 10 бандитов ---
	object models;
	for (i = 1; i <= 25; i++)
	{
		str = "s" + (i - 1);
		models.(str) = "pirate_" + i;
	}
	RandomShuffle(&models);
	for (i = 0; i < 10; i++)
	{
		str = "s" + (i % 25);
		chr = GetCharacter(NPC_GenerateCharacter("GWband_" + sIdx + "_" + i, models.(str), "man", "man", iRankB, iNation, 1, true));
		SetFantomParamFromRank(chr, iRankB, true);
		chr.greeting = "Enc_Raiders";
		ChangeCharacterAddressGroup(chr, location.id, encGroup, locBand);
		LAi_SetWarriorTypeNoGroup(chr);
		LAi_group_MoveCharacter(chr, sBandGroup);
	}

	// --- 15 солдат: офицер + 10 рукопашников + 4 мушкетёра ---
	for (i = 0; i < 15; i++)
	{
		if (i == 0) // офицер
		{
			chr = GetCharacter(NPC_GenerateCharacter("GWsold_" + sIdx + "_" + i, "off_" + NationShortName(iNation) + "_" + (rand(1)+1), "man", "man", iRankS+2, iNation, 1, true));
			SetFantomParamFromRank(chr, iRankS+2, true);
		}
		else
		{
			if (i >= 11) // мушкетёры (11..14)
			{
				chr = GetCharacter(NPC_GenerateMusketeer("GWsold_" + sIdx + "_" + i, NationShortName(iNation) + "_rifle_" + (rand(2)+1), "man", iRankS, iNation, 1, false));
				SetFantomParamFromRank(chr, iRankS, true);
			}
			else // рукопашники (1..10)
			{
				chr = GetCharacter(NPC_GenerateCharacter("GWsold_" + sIdx + "_" + i, "sold_" + NationShortName(iNation) + "_" + (rand(7)+1), "man", "man", iRankS, iNation, 1, true));
				SetFantomParamFromRank(chr, iRankS, true);
			}
		}
		chr.City = sCity;
		chr.CityType = "soldier";
		chr.greeting = "soldier_arest";
		ChangeCharacterAddressGroup(chr, location.id, encGroup, locSold);
		LAi_SetWarriorTypeNoGroup(chr);
		LAi_group_MoveCharacter(chr, sSoldGroup);
	}

	// --- отношения: обе нейтральны к ГГ, враждебны друг другу, начинают бой ---
	LAi_group_SetRelation(sBandGroup, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(sSoldGroup, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(sBandGroup, sSoldGroup, LAI_GROUP_ENEMY);
	LAi_group_FightGroups(sBandGroup, sSoldGroup, true);
	// при атаке ГГ включается тревога; без заданной alarm-реакции движок на миг сбрасывает вражду групп -> бойцы теряют цель
	// и "перевыхватывают" оружие (убирают/достают). Фиксируем вражду групп и В тревоге, и после - тогда они не теряют цели.
	LAi_group_SetAlarmReaction(sBandGroup, sSoldGroup, LAI_GROUP_ENEMY, LAI_GROUP_ENEMY);
	LAi_group_SetAlarmReaction(sSoldGroup, sBandGroup, LAI_GROUP_ENEMY, LAI_GROUP_ENEMY);
	LAi_group_SetLookRadius(sSoldGroup, 100);
	LAi_group_SetHearRadius(sSoldGroup, 100);

	// знаки мини-босса над главарём банды и офицером патруля (помогают опознать вожаков; на выжившем держится и после боя)
	ref ldBand = CharacterFromID("GWband_" + sIdx + "_0");
	ref ldSold = CharacterFromID("GWsold_" + sIdx + "_0");
	ldBand.bGangLeader = true; MarkCharacter(ldBand, "QM_MiniBoss"); // главарь
	ldSold.bGangLeader = true; MarkCharacter(ldSold, "QM_MiniBoss"); // офицер

	// детект гибели групп (исход - этап 2)
	LAi_group_SetCheck(sBandGroup, "GangWar_BandWiped");
	LAi_group_SetCheck(sSoldGroup, "GangWar_SoldWiped");

	// состояние события
	pchar.GangWar.active = true;
	pchar.GangWar.loc    = location.id;
	pchar.GangWar.idx    = sIdx;
	pchar.GangWar.nation = iNation;

	// таймер-страховка: убрать на след. сутки
	str = "GWover_" + sIdx;
	pchar.quest.(str).win_condition.l1            = "Timer";
	pchar.quest.(str).win_condition.l1.date.day   = GetAddingDataDay(0, 0, 1);
	pchar.quest.(str).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
	pchar.quest.(str).win_condition.l1.date.year  = GetAddingDataYear(0, 0, 1);
	pchar.quest.(str).win_condition               = "GangWar_Over";
}

// Сколько живых в группе (по префиксу)
int GangWar_CountAlive(string sPrefix, int iQty)
{
	int i, n;
	ref m;
	n = 0;
	for (i = 0; i < iQty; i++)
	{
		if (GetCharacterIndex(sPrefix + i) == -1) continue;
		m = CharacterFromID(sPrefix + i);
		if (!LAi_isDead(m)) n++;
	}
	return n;
}

// Выживших победившей стороны делаем мирными (расходятся), с отпором при атаке. iStart - с какого индекса (1 - пропустить главаря)
void GangWar_MakeSurvivorsIdle(string sPrefix, int iQty, int iStart)
{
	int i;
	ref m;
	for (i = iStart; i < iQty; i++)
	{
		if (GetCharacterIndex(sPrefix + i) == -1) continue;
		m = CharacterFromID(sPrefix + i);
		if (LAi_isDead(m)) continue;
		// LEO: НЕ горожанин. Его шаблон полицейски реагирует на обнажённый клинок: вооружённый вместо
		//   испуга лезет в разговор «уберите оружие» - в джунглях это лишнее, а файла диалога у рядовых
		//   нет, и игрок получал «Ошибка открытия файла диалога». Шаблон stay ни к кому не лезет,
		//   отпор при атаке остаётся на хуке GangWar_WinnerHit ниже.
		LAi_SetStayTypeNoGroup(m);
		DeleteAttribute(m, "dialog"); // рядовые не разговорчивы: без dialog.currentnode разговор не стартует (иначе пустой filename -> ошибка в эррор-лог)
		m.greeting = "";              // убрать приветственный бубнёж энкаунтера
		LAi_SetCheckMinHP(m, LAi_GetCharacterHP(m)-1, false, "GangWar_WinnerHit"); // атака по победителю -> отпор
	}
}

// Победившая сторона даёт отпор ГГ (после боя ГГ ударил победителя)
void GangWar_WinnerFightBack()
{
	if (!CheckAttribute(pchar, "GangWar.winner")) return;
	string sIdx = pchar.GangWar.idx;
	string sWinGroup, sWinPrefix;
	int winQty, i;
	ref m;
	if (pchar.GangWar.winner == "bandits") { sWinGroup = "GWband_" + sIdx; sWinPrefix = "GWband_" + sIdx + "_"; winQty = 10; }
	else                                   { sWinGroup = "GWsold_" + sIdx; sWinPrefix = "GWsold_" + sIdx + "_"; winQty = 15; }

	// предательство: ГГ нападает на ту сторону, которой помогал
	string sSide = "";
	if (CheckAttribute(pchar, "GangWar.side")) sSide = pchar.GangWar.side;
	if (pchar.GangWar.winner == "patrol" && sSide == "patrol")
	{
		GangWar_SetRumor("betrayPatrol");        // слух о предательстве патруля (штраф за солдат идёт поимённо в LAi_SetResultOfDeath по side!="bandits")
	}
	if (pchar.GangWar.winner == "bandits" && sSide == "bandits")
	{
		pchar.GangWarBandPenalty = "betray";     // одноразово: в след. бандитском событии награды не будет
		GangWar_SetRumor("betrayBandits");       // слух о предательстве банды
	}

	for (i = 0; i < winQty; i++)
	{
		if (GetCharacterIndex(sWinPrefix + i) == -1) continue;
		m = CharacterFromID(sWinPrefix + i);
		LAi_RemoveCheckMinHP(m);
		if (LAi_isDead(m)) continue;
		LAi_SetWarriorTypeNoGroup(m);
		LAi_group_MoveCharacter(m, sWinGroup);
	}
	LAi_group_SetRelation(sWinGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups(sWinGroup, LAI_GROUP_PLAYER, true);
}

// Завершение разговора-благодарности: главарь становится мирным, повторно не наградит
void GangWar_FinishThanks()
{
	if (!CheckAttribute(pchar, "GangWar.winner")) return;
	string sIdx = pchar.GangWar.idx;
	string sWinPrefix;
	if (pchar.GangWar.winner == "bandits") sWinPrefix = "GWband_" + sIdx + "_";
	else sWinPrefix = "GWsold_" + sIdx + "_";
	string sLeaderId = sWinPrefix + "0";
	if (GetCharacterIndex(sLeaderId) != -1)
	{
		ref ld = CharacterFromID(sLeaderId);
		LAi_SetCheckMinHP(ld, LAi_GetCharacterHP(ld)-1, false, "GangWar_WinnerHit"); // отпор, если ГГ потом нападёт
		ld.dialog.currentnode = "AfterThanks"; // повторно не наградит
	}
	pchar.GangWar.resolved = "rewarded";
}

// Награда за помощь патрулю (вызывается из диалога, нода ThankPatrol)
void GangWar_RewardPatrol()
{
	AddCharacterExpToSkill(pchar, "Leadership", 150);
	AddCharacterExpToSkill(pchar, "Fortune", 150);
	if (CheckAttribute(pchar, "GangWar.nation")) ChangeCharacterNationReputation(pchar, sti(pchar.GangWar.nation), 3);
	ChangeCharacterReputation(pchar, 1);
	GangWar_FinishThanks();
}

// Награда за помощь бандитам (вызывается из диалога, нода ThankBandits)
void GangWar_RewardBandits()
{
	// штраф (предал банду в прошлый раз ИЛИ помогал патрулю в прошлом событии) - награды нет
	if (CheckAttribute(pchar, "GangWar.bandWithhold"))
	{
		GangWar_FinishThanks();
		return;
	}
	string sIdx = pchar.GangWar.idx;
	int iMoney = GangWar_CountAlive("GWband_" + sIdx + "_", 10) * 1000 + sti(pchar.rank) * (1000 + rand(500));
	AddCharacterExpToSkill(pchar, "Leadership", 150);
	AddCharacterExpToSkill(pchar, "Sneak", 150);
	if (CheckAttribute(pchar, "GangWar.nation")) ChangeCharacterNationReputation(pchar, sti(pchar.GangWar.nation), -3);
	ChangeCharacterReputation(pchar, -1);
	AddMoneyToCharacter(pchar, iMoney);
	GangWar_FinishThanks();
}

// Одна из групп уничтожена (вызывается из quest_reaction по LAi_group_SetCheck)
void GangWar_OnVictory(string sWinner)
{
	if (!CheckAttribute(pchar, "GangWar.active")) return;
	if (CheckAttribute(pchar, "GangWar.winner")) return; // исход уже обработан
	pchar.GangWar.winner = sWinner;

	string sIdx = pchar.GangWar.idx;
	LAi_group_RemoveCheck("GWband_" + sIdx);
	LAi_group_RemoveCheck("GWsold_" + sIdx);

	// убрать весь выпавший лут со стычки (трупы обеих групп)
	GangWar_StripDeadLoot("GWband_" + sIdx + "_", 10);
	GangWar_StripDeadLoot("GWsold_" + sIdx + "_", 15);

	string sWinPrefix, sLeaderId;
	int winQty;
	if (sWinner == "bandits") { sWinPrefix = "GWband_" + sIdx + "_"; winQty = 10; }
	else                      { sWinPrefix = "GWsold_" + sIdx + "_"; winQty = 15; }
	sLeaderId = sWinPrefix + "0";

	bool bLeaderAlive = false;
	if (GetCharacterIndex(sLeaderId) != -1)
	{
		if (!LAi_isDead(CharacterFromID(sLeaderId))) bLeaderAlive = true;
	}

	string sSide = "";
	if (CheckAttribute(pchar, "GangWar.side")) sSide = pchar.GangWar.side;

	// LEO: Сначала проверяем атаку обеих сторон, даже если одна группа погибла раньше другой.
	bool bMassacre = false;
	if (sSide != "")
	{
		if (CheckAttribute(pchar, "GangWar.hitBoth")) bMassacre = true;
		if (!bMassacre && GangWar_CountAlive(sWinPrefix, winQty) == 0) bMassacre = true; // перебиты ОБЕ стороны
	}
	if (bMassacre)
	{
		// штраф за лично убитых солдат уже начислен поимённо в LAi_SetResultOfDeath (side!="bandits").
		// если же ГГ номинально был на стороне банды (там солдат поимённо не штрафуем) - снимаем нация −3 разом за вырезанный патруль, т.к. награды нет.
		if (sSide == "bandits" && CheckAttribute(pchar, "GangWar.nation")) ChangeCharacterNationReputation(pchar, sti(pchar.GangWar.nation), -3);
		GangWar_SetRumor("massacre");
		pchar.GangWar.resolved = "massacre";
		if (GangWar_CountAlive(sWinPrefix, winQty) == 0) // никого не осталось - бой окончен, гасим алярм и прячем оружие
		{
			LAi_group_ClearAllTargets();
			LAi_SetFightMode(pchar, false);
			LAi_SetFightModeForOfficers(false);
			LAi_grp_alarmactive = false;
		}
		return; // если выжившие враждебны ГГ - тревогу НЕ снимаем, доколачивают ГГ
	}

	// ГГ помог проигравшим -> победитель враждебен ГГ, добивает его, наград нет (тревогу НЕ снимаем - бой с ГГ продолжается)
	if (sSide != "" && sSide != sWinner)
	{
		pchar.GangWar.resolved = "fightplayer";
		return;
	}

	// бой окончен мирно для ГГ: гасим боевую тревогу и прячем оружие у ГГ, его офицеров и выживших победителей
	LAi_group_SetRelation("GWband_" + sIdx, "GWsold_" + sIdx, LAI_GROUP_NEITRAL); // снять вражду групп (проигравшие мертвы, но реляция держит алярм)
	LAi_group_ClearAllTargets();
	LAi_SetFightMode(pchar, false);          // ГГ убирает клинок (иначе движок держит алярм ещё некоторое время)
	LAi_SetFightModeForOfficers(false);      // и сопровождающие офицеры
	LAi_grp_alarmactive = false;

	// ГГ реально помог победителю (и не повернул против него) -> штраф/слух и личная благодарность
	if (sSide == sWinner)
	{
		if (sSide == "patrol") pchar.GangWarBandPenalty = "soldiers"; // в СЛЕДУЮЩЕМ бандитском событии банды не наградят (одноразово)
		if (sWinner == "patrol") GangWar_SetRumor("patrol");
		else                     GangWar_SetRumor("bandits");

		// главарь/офицер жив -> подбегает с благодарностью и наградой (награда в ноде диалога ThankPatrol/ThankBandits)
		if (bLeaderAlive)
		{
			ref ld = CharacterFromID(sLeaderId);
			ld.dialog.filename = "Enc_GangWar.c";
			if (sWinner == "patrol")
			{
				ld.dialog.currentnode = "ThankPatrol";
			}
			else
			{
				// бандитская благодарность: расходуем штраф (помог патрулю / предал банду в прошлый раз)
				if (CheckAttribute(pchar, "GangWarBandPenalty"))
				{
					pchar.GangWar.bandWithhold = pchar.GangWarBandPenalty; // "soldiers" или "betray"
					DeleteAttribute(pchar, "GangWarBandPenalty"); // одноразово
				}
				ld.dialog.currentnode = "ThankBandits";
			}
			ld.greeting = "";
			LAi_SetActorTypeNoGroup(ld);
			LAi_ActorDialog(ld, pchar, "", 25.0, 0); // главарь/офицер сам подбегает к ГГ и заводит благодарность
			GangWar_MakeSurvivorsIdle(sWinPrefix, winQty, 1); // рядовые расходятся
			pchar.GangWar.resolved = "thanks";
			return;
		}

		// помог, но главарь погиб в бою (не от руки ГГ) -> выжившие просто расходятся, личной благодарности нет (слух уже разошёлся)
		GangWar_MakeSurvivorsIdle(sWinPrefix, winQty, 0);
		pchar.GangWar.resolved = "helped";
		return;
	}

	// ГГ не вмешивался -> победитель мирный, можно расспросить главаря
	GangWar_MakeSurvivorsIdle(sWinPrefix, winQty, 0);
	if (bLeaderAlive)
	{
		ref ld2 = CharacterFromID(sLeaderId);
		ld2.dialog.filename = "Enc_GangWar.c";
		ld2.dialog.currentnode = "Story";
	}
	pchar.GangWar.resolved = "idle";
}
//=====================================================================================================================================
// <-- ЭТАП 1
//=====================================================================================================================================
