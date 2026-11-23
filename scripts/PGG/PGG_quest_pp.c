//==================================================
// Quests
//==================================================
//========== Офицер ===========
void PGG_BecomeHiredOfficer()
{
	ref chr = &Characters[sti(pchar.questTemp.HiringOfficerIDX)];

	AddPassenger(pchar, chr, false);

	chr.location = "None";
	chr.PGGAi.location.town = "Офицер";
	chr.PGGAi.IsPGG = false;
	chr.Dialog.CurrentNode = "hired";
	chr.Dialog.FileName = "Enc_Officer_dialog.c";
	//офицер только раз.
	chr.PGGAi.Officer = 1;
	// учет зп -->
	chr.Money   = 0;
	chr.Payment = true;
	// учет зп <--

	DeleteAttribute(pchar, "questTemp.HiringOfficerIDX");
	PGG_ChangeRelation2MainCharacter(chr, 10);
	SetCharacterTask_FollowCharacter(chr, pchar);
}

void PGG_FireOfficer()
{
	ref chr = &Characters[sti(pchar.questTemp.FiringOfficerIDX)];
	CheckForReleaseOfficer(sti(pchar.questTemp.FiringOfficerIDX));

	RemovePassenger(pchar, chr);
	RemoveCharacterCompanion(pchar, chr);

	LAi_SetWarriorType(chr);

	chr.Dialog.CurrentNode = "Second time";
	chr.Dialog.TempNode = "Second time";
	chr.Dialog.FileName = "PGG_Dialog.c";
	chr.location = "none";
	chr.PGGAi.location.town = PGG_TownForRelease(chr);
	chr.PGGAi.IsPGG = true;

	PGG_ClearOfficerLeftovers(chr); // LEO: Payment/RestorePost - хвосты службы, штатное увольнение офицера их снимает, а этот путь не снимал
	DeleteAttribute(pchar, "questTemp.FiringOfficerIDX"); // LEO: был неверный путь quest.TempFiringOfficerIDX — реальный атрибут висел
	PGG_ChangeRelation2MainCharacter(chr, -20);
}

//========== Компаньон ===========
void PGG_CompanionLeave(string qName)
{
	ref chr;
	chrDisableReloadToLocation = true;
	chr = &Characters[sti(pchar.questTemp.PGG_Companion_Leave.index)];
	PlaceCharacter(chr, "goto", "random_must_be_near");

	chr.Dialog.CurrentNode = "companion_leave";
	chr.Dialog.FileName = "PGG_Dialog.c";
	LAi_SetActorType(chr);
	LAi_ActorDialog(chr, pchar, "", 2.0, 0);
	DoQuestCheckDelay("OpenTheDoors", 5.0);   //fix
}

//========== Квестовая проверка ===========
bool PGG_CheckForQuestOffer(ref chr)
{
    bool retVal = false;

    if (rand(2) == 1 || CheckAttribute(chr, "PGGWhisperQuestStart")) return retVal;
	int iTst = PGG_PP_OFFER_CHANCE; //фиксированные 10%
	bool bOkRel = PGG_ChangeRelation2MainCharacter(chr, 0) >= PGG_REL_FRIENDLY; //если отношение меньше 50 - не подходить

    bool bOkShip = sti(chr.Ship.Type) != SHIP_NOTUSED && sti(PChar.Ship.Type) != SHIP_NOTUSED && GetCharacterShipClass(PChar) <= 4;
    bool bOkRnd = rand(99) > iTst;
    int  iDays = GetQuestPastDayParam("QuestOffer");
    bool bOkDaysPassed = iDays > PGG_PP_OFFER_CD_BASE + drand(PGG_PP_OFFER_CD_RAND); // кд от 25 до 45 дней
    bool bOkCompanionsMax = GetCompanionQuantity(PChar) < COMPANION_MAX;
    bool bOk = PGG_IsQuestAvaible();

    if (bOkRel && bOk && bOkShip && bOkRnd && bOkDaysPassed && bOkCompanionsMax)
    {
        chr.PGGAi.ActiveQuest.QstNumber = 0;
        retVal = true;
        SaveCurrentQuestDateParam("QuestOffer"); // какое-то время ПГГ не будут докучать нам
    }

    if (retVal)
    {
        chrDisableReloadToLocation = true;
        PlaceCharacter(chr, "goto", "random_must_be_near");
        if (!CheckAttribute(chr, "PGGAi.Task.SetSail"))
        {
            PChar.GenQuest.PGG_Quest = 1;
        }
        chr.Dialog.CurrentNode = "quest_onStay";
        chr.PGGAi.ActiveQuest = 1;
        LAi_SetActorType(chr);
        LAi_ActorDialog(chr, pchar, "", 2.0, 0);
        DoQuestCheckDelay("OpenTheDoors", 5.0);
    }
    return retVal;
}

bool PGG_IsQuestAvaible()
{
	string sTown;
	int iCol;
	bool retVal = true;

	sTown = GetCurrentTown();
	iCol = FindColony(sTown);
	// ⚠️ РАЗНЕСЕНО ПО ОДНОЙ ПРОВЕРКЕ НАМЕРЕННО: Storm НЕ сокращает `||`, и в прежней однострочной цепочке
	//   Colonies[FindColony(sTown)] вычислялся всегда - в том числе для пустого города и не-колонии.
	//   Отсюда «invalid index -1 / GetColonyByIndex stack error» в error.log.
	if (sTown == "") retVal = false;
	if (sTown == "Panama") retVal = false;
	if (iCol < 0) retVal = false;
	else
	{
		if (Colonies[iCol].from_sea == "") retVal = false;
		if (CheckAttribute(Colonies[iCol], "HasNoFort")) retVal = false;
	}
	return retVal;
}

void RunToBoat()
{
	LAi_ActorRunToLocator(CharacterFromID(PChar.questTemp.Chr2Remove), "reload", "boat", "RemoveCharacterFromLocation", 5.0);
}

//========== Первый квест ===========
//убрать корабль ПГГ при отказе от квеста, чтобы не болтался...
void PGG_Q1RemoveShip(string qName)
{
	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	string sQuestID = "Gen_PGGQuest1"; if (chr.sex == "woman") sQuestID = "Gen_PGGQuest1" + chr.sex;

	chr.Dialog.CurrentNode = "Second Time";
	DeleteAttribute(chr, "DeckDialogNode");

	Group_SetAddress("PGGQuest", "None", "", "");
	Group_DelCharacter("PGGQuest", chr.id);

	if (CheckAttribute(PChar, "GenQuest.PGG_Quest.Stage") && sti(PChar.GenQuest.PGG_Quest.Stage) < 2 && sti(PChar.GenQuest.PGG_Quest.Stage) >= -1)
	{
		chr.Dialog.CurrentNode = "Second Time";
		int nRelChange = -10;
		if(sti(PChar.GenQuest.PGG_Quest.Stage) == -1) nRelChange = -20;
		PGG_ChangeRelation2MainCharacter(chr, nRelChange);
		CloseQuestHeader(sQuestID);
	}
	
	DeleteAttribute(chr, "PGGAi.ActiveQuest");
	DeleteAttribute(PChar, "GenQuest.PGG_Quest");
	DeleteAttribute(chr, "AlwaysFriend");
}

void PGG_Q1AcceptedQuestDeleteFantom(string qName)
{
	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);

	chr.Dialog.CurrentNode = "Second Time";
	DeleteAttribute(chr, "DeckDialogNode");
	DeleteAttribute(PChar, "PGG_EnemyPP");
	Group_SetAddress("PGGQuest", "None", "", "");
	Group_DelCharacter("PGGQuest", chr.id);
	//#20180927-01
}
//после драки на палубе при отказе от квеста.
void PGG_Q1AfterDeckFight()
{
	ref chr, rGroup;
	string sGroup;
	sGroup = PChar.GenQuest.PGG_Quest.GrpID;

	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	chr.AlwaysEnemy = true;
	chr.Coastal_Captain    = true;  // не ругать нации
	chr.Abordage.Enable = false;
	//#20190612-05
	chr.WatchFort = true;
	LAi_SetImmortal(chr, false);


	DelEventHandler("LAi_event_GroupKill", "PGG_Q1AfterDeckFight");
	DeleteAttribute(PChar, "GenQuest.PGG_Quest.GrpID");
	chrDisableReloadToLocation = false;
	string PGGSex = "";
	if(chr.sex == "woman")	PGGSex = "woman";

	if (sti(PChar.GenQuest.PGG_Quest.Stage) < 4)
	{
		AddQuestRecord("Gen_PGGQuest1"+PGGSex, "q1_RefuseFight");
		AddQuestUserData("Gen_PGGQuest1"+PGGSex, "sSex", GetSexPhrase("ся","ась"));
		AddQuestUserData("Gen_PGGQuest1"+PGGSex, "sSex1", GetSexPhrase("ин","на"));
		AddQuestUserData("Gen_PGGQuest1"+PGGSex, "sNation", NationNameGenitive(sti(PChar.GenQuest.PGG_Quest.Nation)));
		AddQuestUserData("Gen_PGGQuest1"+PGGSex, "sPsName", GetFullName(chr));
		LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], true);
	}
	else
	{
		if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.Ok"))
		{
			AddQuestRecord("Gen_PGGQuest1"+PGGSex, "q1_FailNotPay");
			AddQuestUserData("Gen_PGGQuest1"+PGGSex, "sSex", GetSexPhrase("","а"));
			AddQuestUserData("Gen_PGGQuest1"+PGGSex, "sSex1", GetSexPhrase("ен","на"));
			AddQuestUserData("Gen_PGGQuest1"+PGGSex, "sPsName", GetFullName(chr));
		}
		else
		{
			AddQuestRecord("Gen_PGGQuest1"+PGGSex, "q1_OkNotShare");
			AddQuestUserData("Gen_PGGQuest1"+PGGSex, "sSex", GetSexPhrase("","а"));
		}
	}
	DoQuestCheckDelay("hide_weapon", 2.0);

	rGroup = Group_FindOrCreateGroup("PGGQuest");
	PChar.Quest.PGGQuest1_AfterDeckFight.win_condition.l1 = "location";
	PChar.Quest.PGGQuest1_AfterDeckFight.win_condition.l1.location = rGroup.Location;
	PChar.Quest.PGGQuest1_AfterDeckFight.function = "PGG_Q1SeaFightAfterDeck";

	CloseQuestHeader("Gen_PGGQuest1"+PGGSex);
}

//выход в море и нападение ПГГ на игрока после боевки на палубе.
void PGG_Q1SeaFightAfterDeck(string qName)
{
	string sTemp = "PGGQuest";
	Group_SetTaskAttackEx(sTemp, PLAYER_GROUP, false);
	Group_LockTask(sTemp);

	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	LAi_SetImmortal(chr, false);

	PChar.Quest.PGGQuest1_AfterSeaFight.win_condition.l1 = "Group_Death";
	PChar.Quest.PGGQuest1_AfterSeaFight.win_condition.l1.group = sTemp;
	PChar.Quest.PGGQuest1_AfterSeaFight.function = "PGG_Q1AfterSeaFight";

	if (sti(PChar.GenQuest.PGG_Quest.Stage) < 4)
	{
		PChar.Quest.PGGQuest1_Runaway.win_condition.l1 = "ExitFromSea";
		PChar.Quest.PGGQuest1_Runaway.function = "PGG_Q1AfterSeaFight";
	}
	else
	{
		PChar.Quest.PGGQuest1_Runaway.win_condition.l1 = "MapEnter";
		PChar.Quest.PGGQuest1_Runaway.function = "PGG_Q1AfterSeaFight";
	}
}

//закрытие квеста при потопление ПГГ или, если игрок свалил...
void PGG_Q1AfterSeaFight(string qName)
{
	ref chr;

	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	chr.Abordage.Enable = true;
	chr.Dialog.CurrentNode = "Second Time";
	// LEO: бэкап отработал - удаляем, иначе устаревшая нация лежит в сейве до конца игры
	if (CheckAttribute(chr, "Nation.Bak"))
	{
		chr.Nation = chr.Nation.Bak;
		DeleteAttribute(chr, "Nation.Bak");
	}
	chr.PGGAi.IsPGG = true;
	chr.RebirthPhantom = true;
	if (chr.PGGAi.location.town == "none")
	{
		chr.PGGAi.location.town = PGG_FindRandomTown(chr);
	}

	Group_SetAddress("PGGQuest", "None", "", "");
	Group_DeleteGroup("PGGQuest");

	PChar.Quest.PGGQuest1_AfterSeaFight.Over = "yes";
	PChar.Quest.PGGQuest1_Runaway.Over = "yes";

	ChangeCharacterAddressGroup(chr, "None", "", "");

	DeleteAttribute(chr, "AlwaysEnemy");
	DeleteAttribute(chr, "Coastal_Captain");
	DeleteAttribute(chr, "PGGAi.ActiveQuest");
	DeleteAttribute(PChar, "GenQuest.PGG_Quest");
}

void PGG_Q1PlaceShipsNearIsland()
{
	ref chr;
	string sCharId, sLoc, sType;
	int i, iRnd, iNation, iLifeDay, iRank, iGoods, iSpace;

	sLoc = PChar.GenQuest.PGG_Quest.Island;
	iRank = sti(PChar.Rank);
	iGoods = sti(PChar.GenQuest.PGG_Quest.Goods);
	iNation = sti(PChar.GenQuest.PGG_Quest.Nation);
	iLifeDay = sti(PChar.GenQuest.PGG_Quest.Days);

	iRnd = 3 + drand(2);
	for (i = 0; i < iRnd; i++)
	{
		sCharId = "RandQuestCap_0" + i;

		if (i < 2) chr = PGG_Q1CreateShip(sCharId, "war"); // первые два военные, остальные торговые
		else       chr = PGG_Q1CreateShip(sCharId, "trade");

		SetCharacterPerk(chr,PerksChars());
		chr.AlwaysSandbankManeuver = true;
		Group_AddCharacter("PGGQuest", chr.id);
		chr.AlwaysEnemy = true;
	}
	Group_SetGroupCommander("PGGQuest", "RandQuestCap_01");
	sType = "Quest_Ship_" + (9 + rand(1));
	ref rIsland = GetIslandByID(sLoc);
	if (rIsland.id == sLoc)
	{
		if (!CheckAttribute(rIsland, "Quest_Ships." + sType + ".x") ||
			!CheckAttribute(rIsland, "Quest_Ships." + sType + ".z") ||
			!CheckAttribute(rIsland, "Quest_Ships." + sType + ".ay"))
		{
			// На Санта-Люсии нет Quest_Ship_10; сохраняем исходный вызов RNG.
			if (CheckAttribute(rIsland, "Quest_Ships.Quest_Ship_9.x") &&
				CheckAttribute(rIsland, "Quest_Ships.Quest_Ship_9.z") &&
				CheckAttribute(rIsland, "Quest_Ships.Quest_Ship_9.ay")) sType = "Quest_Ship_9";
		}
	}
	Group_SetAddress("PGGQuest", PChar.GenQuest.PGG_Quest.Island, "Quest_Ships", sType);
	Group_SetTaskNone("PGGQuest");


	PChar.Quest.PGGQuest1_GroupDead.win_condition.l1 = "Group_Death";
	PChar.Quest.PGGQuest1_GroupDead.win_condition.l1.Group = "PGGQuest";
	PChar.Quest.PGGQuest1_GroupDead.function = "PGG_Q1AfterBattle";

	PChar.Quest.PGGQuest1_CheckStartState.win_condition.l1 = "Location";
	PChar.Quest.PGGQuest1_CheckStartState.win_condition.l1.Location = PChar.GenQuest.PGG_Quest.Island;
	PChar.Quest.PGGQuest1_CheckStartState.function = "PGG_Q1CheckStartState";
}

// Создание фантом-корабля для квеста ПП (караван у острова).
// Слияние бывших PGG_Q1CreateWarShip / PGG_Q1CreateTradeShip: общая часть — здесь,
// различия War/Trade вынесены в ветки по sEncType. Поведение каждой ветки сохранено
// точь-в-точь (тот же порядок вызовов и те же формулы), включая порядок rand/drand.
// sEncType: "war" (военный конвой) либо "trade" (торговое судно).
ref PGG_Q1CreateShip(string sCharId, string sEncType)
{
	int iRank    = sti(PChar.Rank);
	int iGoods   = sti(PChar.GenQuest.PGG_Quest.Goods);
	int iNation  = sti(PChar.GenQuest.PGG_Quest.Nation);
	int iLifeDay = sti(PChar.GenQuest.PGG_Quest.Days);

	// Капитан: модель по нации и роли ("war"/"trade"); ранг = половина ранга ГГ
	// + случайная добавка до полного ранга ГГ; время жизни фантома = длительность квеста.
	ref chr = GetCharacter(NPC_GenerateCharacter(sCharId, GetCaptainModelByNation(iNation, sEncType), "man", "man", MakeInt(iRank / 2 + rand(iRank)), iNation, iLifeDay, true));
	chr.EncType = sEncType;

	// Тип судна и стартовые параметры по роли.
	if (sEncType == "war") CreatePGG_War(chr, iNation, pchar);
	else                   CreatePGG_Trade(chr, iNation, pchar);

	SetRandomNameToShip(chr);
	SetBaseShipData(chr);
	SetCrewQuantityFull(chr);

	// Пушки и боезапас по роли.
	// РАЗЛИЧИЕ: военному дополнительно раскладывается боевой груз (Fantom_SetGoods);
	// у торговца его нет — весь трюм пойдёт под целевой товар ниже.
	Fantom_SetCannons(chr, sEncType);
	Fantom_SetBalls(chr, sEncType);
	if (sEncType == "war") Fantom_SetGoods(chr, sEncType);

	// Сколько целевого товара (iGoods) положить в трюм. РАЗЛИЧИЕ в формуле:
	//   war:   свободное место / (4 + drand(2))     — военный везёт меньше товара;
	//   trade: свободное место * 3 / (3 + drand(2))  — торговец везёт больше.
	// drand(2) одинаков в пределах корабля; суммарный груз каравана копится в Goods.Qty.
	int iSpace;
	if (sEncType == "war") iSpace = GetCharacterFreeSpace(chr, iGoods) / (4 + drand(2));
	else                   iSpace = GetCharacterFreeSpace(chr, iGoods) * 3 / (3 + drand(2));
	if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.Goods.Qty")) PChar.GenQuest.PGG_Quest.Goods.Qty = 0;
	PChar.GenQuest.PGG_Quest.Goods.Qty = sti(PChar.GenQuest.PGG_Quest.Goods.Qty) + iSpace;
	Fantom_SetCharacterGoods(chr, iGoods, iSpace, 1);

	// Финальные морские параметры. РАЗЛИЧИЕ:
	//   war:   «крутой» капитан-охотник (SetFantomParamHunter) + повторный
	//          SetRandomNameToShip (как было в оригинале PGG_Q1CreateWarShip);
	//   trade: обычные морские параметры (SetSeaFantomParam).
	if (sEncType == "war")
	{
		SetRandomNameToShip(chr);
		SetFantomParamHunter(chr);
	}
	else
	{
		SetSeaFantomParam(chr, sEncType);
	}
	SetCaptanModelByEncType(chr, sEncType);

	return chr;
}

void PGG_Q1PGGDead(string qName)
{
	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	string sQuestID = "Gen_PGGQuest1"; if (chr.sex == "woman") sQuestID = "Gen_PGGQuest1" + chr.sex;
	if (sti(PChar.GenQuest.PGG_Quest.Template))
	{
		if (sti(PChar.GenQuest.PGG_Quest.Stage) < 3)
		{
			Group_SetAddress("PGGQuest", "None", "", "");
			Group_DeleteGroup("PGGQuest");

			if (CheckAttribute(chr, "PGGAi.ActiveQuest"))
			{
				AddQuestRecord(sQuestID, "q1_PGGDeadNoShips");
			}
			else
			{
				AddQuestRecord(sQuestID, "q1_PGGDeadNoShips_1");
			}
			PChar.Quest.PGGQuest1_CheckStartState.Over = "yes"; //fix doors
		}
		else
		{
			AddQuestRecord(sQuestID, "q1_PGGDeadShips");
		}
		AddQuestUserData(sQuestID, "sPsName", GetFullName(chr));

		PChar.Quest.PGGQuest1_GroupDead.Over = "yes";
		PChar.Quest.PGGQuest1_Time2Late.Over = "yes";
		PChar.Quest.PGGQuest1_Time2Fight.Over = "yes";
		CloseQuestHeader(sQuestID);
	}
	else
	{
		AddQuestRecord(sQuestID, "q1_PGGDead");
		AddQuestUserData(sQuestID, "sPsName", GetFullName(chr));
	}
	ChangeCharacterReputation(PChar, -25);
	PChar.GenQuest.PGG_Quest.PGGid.Dead = 1;
	DeleteAttribute(chr, "PGGAi.ActiveQuest");
	PChar.Quest.PGGQuest1_GroupDead.Over = "yes";
	PChar.Quest.PGGQuest1_PrizesDone.Over = "yes";
	PChar.Quest.PGGQuest1_Time2Fight.Over = "yes";
	PChar.Quest.PGGQuest1_PGGDead.Over = "Yes";
	if (sti(PChar.GenQuest.PGG_Quest.Template)) DeleteAttribute(PChar, "GenQuest.PGG_Quest");
}

void PGG_Q1CheckStartState(string qName)
{
	ref chr;
	int i, n, iNum;

	if (CheckAttribute(PChar,"PGG_EnemyPP") && !CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid.Dead") && sti(PChar.GenQuest.PGG_Quest.Template))
	{
		Group_AddCharacter("EnemyPGG", PChar.GenQuest.PGG_Quest.PGGid);
		Group_SetGroupCommander("EnemyPGG", PChar.GenQuest.PGG_Quest.PGGid);
		PGG_SpawnPGG();

		SetTimerCondition("PGGQuest1_Time2Fight", 0, 0, 2, false);

		PChar.Quest.PGGQuest1_ExitBattleField.win_condition.l1 = "ExitFromLocation";
		PChar.Quest.PGGQuest1_ExitBattleField.win_condition.l1.Location = pchar.location;
		PChar.Quest.PGGQuest1_ExitBattleField.function = "PGG_Q1EndClear";
		PChar.Quest.PGGQuest1.SeaBattle = "1";
	}
	else
	{
		PChar.GenQuest.PGG_Quest.Stage = 3;
		PChar.Quest.PGGQuest1_Time2Late.Over = "yes";
		PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";

		if (sti(PChar.GenQuest.PGG_Quest.Template))
		{
			SetTimerCondition("PGGQuest1_Time2Fight", 0, 0, 2, false);
			PChar.Quest.PGGQuest1_Time2Fight.function = "PGG_Q1AfterBattle";
		}
		else
		{
			if (!isLocationFreeForQuests(PChar.GenQuest.PGG_Quest.Island.Shore))
			{
				DeleteAttribute(PChar, "Quest.PGGQuest1_CheckStartState.Over");
				return;
			}
			else PGG_Q1FightOnShore();
		}
		PChar.GenQuest.PGG_Quest.StartGoods = 0;
		n = GetCompanionQuantity(PChar);
		for (i = 0; i < n; i++)
		{
			iNum = GetCompanionIndex(PChar, i);
			if (iNum != -1)
			{
				chr = GetCharacter(iNum);
				if (GetRemovable(chr))
				{
					PChar.GenQuest.PGG_Quest.StartGoods = sti(PChar.GenQuest.PGG_Quest.StartGoods) + GetCargoGoods(chr, sti(PChar.GenQuest.PGG_Quest.Goods));
				}
			}
		}
	}
}

// LEO: приз, брошенный в дрейф у острова каравана (ГГ из transfer_main или компаньон из companions.c), ждёт квест, пока
//   добычу не заберут. Группа своя, без задач: в боевой PGGQuest групповая атака раздавала призу вторичную задачу
//   (она важнее дрейфа), и он то стоял, то шёл в бой - «жил своей жизнью». Ждёт её PGG_Q1WaitPrizes.
void PGG_Q1KeepDriftPrize(ref rDriftCap)
{
	if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.Island")) return;
	if (!CheckAttribute(&AISea, "Island")) return;
	if (AISea.Island != PChar.GenQuest.PGG_Quest.Island) return;
	Group_AddCharacter("PGGQuestDrift", rDriftCap.id);
	Group_DeleteAtEnd("PGGQuestDrift");
}

// LEO: караван разбит, но на плаву его дрейф-призы (PGGQuestDrift) или сдавшиеся (SURR_GROUP, метка из Ship_Neutral) -
//   бой не окончен, пока их не заберут, не отпустят или не потопят: любой исход убирает корабль из группы либо мёртвым.
//   Условие своё: сработавшее GroupDead уже помечено over, перевзвести его из обработчика нельзя.
bool PGG_Q1WaitPrizes()
{
	if (!bSeaActive) return false;
	bool bWait = false;
	if (!Group_isDead("PGGQuestDrift")) bWait = true;
	if (CheckAttribute(PChar, "GenQuest.PGG_Quest.SurrPending"))
	{
		if (Group_isDead("SURR_GROUP")) DeleteAttribute(PChar, "GenQuest.PGG_Quest.SurrPending");
		else bWait = true;
	}
	if (!bWait) return false;
	PChar.Quest.PGGQuest1_PrizesDone.win_condition.l1 = "Group_Death";
	PChar.Quest.PGGQuest1_PrizesDone.win_condition.l1.Group = "PGGQuestDrift";
	PChar.Quest.PGGQuest1_PrizesDone.win_condition.l2 = "Group_Death";
	PChar.Quest.PGGQuest1_PrizesDone.win_condition.l2.Group = "SURR_GROUP";
	PChar.Quest.PGGQuest1_PrizesDone.function = "PGG_Q1AfterBattle";
	return true;
}

void PGG_Q1AfterBattle(string qName)
{
	//Boyer add
	int cn;
	int nNumShips = 0;

	ref chr;
	aref arOldMapPos;
	int i, n, iNum, iRnd;
	string sLoc;
	bool bLater = false;
	if (PGG_Q1WaitPrizes()) return;
	bool bOk = !Group_isDead("PGGQuest") && !bMapEnter;

	if (bOk || !bSeaActive)
	{
		if (IsEntity(worldMap))
		{
			bLater = true;
		}
		else
		{
			SetTimerConditionParam("PGGQuest1_Time2Fight", "PGGQuest1_Time2Fight", 0, 0, 0, MakeInt(GetHour() + 2), false);
			DeleteAttribute(PChar, "Quest.PGGQuest1_Time2Fight.Over");
			return;
		}
	}

	//теперь весь товар после боя минус то, что было изначально!
	PChar.GenQuest.PGG_Quest.Goods.Taken = 0;
	n = GetCompanionQuantity(PChar);
	for (i = 0; i < n; i++)
	{
		iNum = GetCompanionIndex(PChar, i);
		if (iNum != -1)
		{
			chr = GetCharacter(iNum);
			if (GetRemovable(chr))
			{
				PChar.GenQuest.PGG_Quest.Goods.Taken = sti(PChar.GenQuest.PGG_Quest.Goods.Taken) + GetCargoGoods(chr, sti(PChar.GenQuest.PGG_Quest.Goods));
			}
		}
	}

	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	sLoc = PChar.GenQuest.PGG_Quest.Island.Shore;

	PChar.GenQuest.PGG_Quest.Goods.Taken = sti(PChar.GenQuest.PGG_Quest.Goods.Taken) - sti(PChar.GenQuest.PGG_Quest.StartGoods);
	PChar.GenQuest.PGG_Quest.GrpLoc = Island_FindNearestLocator2PChar("Quest_Ships");
	PChar.GenQuest.PGG_Quest.GrpLocation = PChar.location;

	if (!bLater)
	{
		if (isLocationFreeForQuests(sLoc))
		{
			PChar.location.from_sea = sLoc;
			Locations[FindLocation(sLoc)].DisableEncounters = true;
			DoReloadFromSeaToLocation(sLoc, "reload", "sea");
		}
		else
		{
			MakeCloneShipDeck(pchar, true); // подмена палубы
			DoReloadFromSeaToLocation("Ship_deck", "goto", "goto5");
   			pchar.quest.Munity = "";
			sLoc = "Ship_deck";
		}
	}
	else
	{
		makearef(arOldMapPos, worldMap.old);
		WdmPrepareMapForAbordage(arOldMapPos);
		MakeCloneShipDeck(pchar, true); // подмена палубы
		LAi_LocationFightDisable(&Locations[FindLocation("Ship_deck")], true);
		DoReloadFromWorldMapToLocation("Ship_deck", "goto", "goto5");
		ReloadFromWMtoL_complete();
		LAi_LockFightMode(pchar, true);
		PChar.Quest.Munity = "";
		sLoc = "Ship_deck";
	}

	PChar.Quest.PGGQuest1_LocationLoaded.win_condition.l1 = "Location";
	PChar.Quest.PGGQuest1_LocationLoaded.win_condition.l1.Location = sLoc;
	PChar.Quest.PGGQuest1_LocationLoaded.function = "PGG_Q1LocationLoaded";

	PChar.Quest.PGGQuest1_PGGDead.Over = "yes";
	PChar.Quest.PGGQuest1_GroupDead.Over = "yes";
	PChar.Quest.PGGQuest1_PrizesDone.Over = "yes";
	PChar.Quest.PGGQuest1_Time2Fight.Over = "yes";
	PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
	PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";
}
void PGG_Q1LocationLoaded(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr;
	int i, iRnd;
	string attrName;

	//Boyer fix...battle interface stills showing after teleport fix in quests.c:  DoReloadCharacterToLocation
	DeleteBattleInterface();
	//End Boyer fix
	Group_SetAddress("PGGQuest", "", "", "");
	Group_SetAddress("PGGQuestE", "", "", "");
	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	chr.Dialog.CurrentNode = "Quest_1_CheckResult";
	LAi_SetImmortal(chr, true);

	PlaceCharacter(chr, "goto", "random_must_be_near");
	LAi_SetActorType(chr);
	LAi_ActorDialog(chr, pchar, "", 2.0, 0);

	iRnd = 3 + rand(2);
	PChar.GenQuest.PGG_Quest.GrpID.Qty = iRnd;
	chr.location = pchar.location;
	for (i = 1; i < iRnd; i++)
	{
		attrName = "pirate_" + i;
		chr = GetCharacter(NPC_GenerateCharacter(attrName, attrName, "man", "man", 15, PIRATE, 0, true));
		chr.dialog.Filename = "Quest\sailors_dialog.c";
		LAi_SetWarriorType(chr);
		LAi_SetLoginTime(chr, 0.0, 24.0);
		LAi_SetHP(chr, 100.0, 100.0);
		ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto"+i);
		LAi_group_MoveCharacter(chr, "PGGTmp");
	}
	LAi_group_SetRelation("PGGTmp", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);

	PChar.GenQuest.PGG_Quest.GrpID = "PGGTmp";
	PChar.location.from_sea = PChar.location;
}

void PGG_Q1EndClear(string qName)
{
	ref rFailPGG;

	// LEO: гасим условия ВСЕГДА и в одном месте - ветки гасили их порознь, а Failed*/Clear не гасил
	//   никто. Непогашенное условие держит слот и зовёт эту функцию снова на каждом выходе из моря,
	//   уже без квестовых данных.
	PChar.Quest.PGGQuest1_FailedExitSea.Over = "Yes";
	PChar.Quest.PGGQuest1_FailedExitLoc.Over = "Yes";
	PChar.Quest.PGGQuest1_Clear.Over = "Yes";
	PChar.Quest.PGGQuest1_EndExitSea.Over = "Yes";
	PChar.Quest.PGGQuest1_EndExitLoc.Over = "Yes";
	PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
	PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";

	if (CheckAttribute(PChar,"PGG_EnemyPP"))
	{
		// LEO: группу убираем безусловно - гейт по SeaBattle оставлял её жить с залоченной атакой:
		//   квест закрыт, а ПГГ продолжал нападать в море.
		Group_SetAddress("PGGQuest", "none", "none", "none");
		LAi_group_Delete("PGGQuest");
		Group_DeleteGroup("PGGQuest");

		// LEO: снимаем вражду: обида за провал живёт в ОТНОШЕНИИ (его понизили на выходе из диалога),
		//   а AlwaysEnemy - вечный флаг, он переживал даже примирение через дипломата.
		if (CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid"))
		{
			rFailPGG = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
			if (CheckAttribute(rFailPGG, "index")) PGG_MakePeace(rFailPGG);
		}

		DeleteAttribute(PChar, "DisableBIFace");
		DeleteAttribute(PChar,"PGG_EnemyPP");
		DeleteAttribute(PChar,"Quest.PGGQuest1.SeaBattle");
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
	}
	else
	{
		ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
		DeleteAttribute(&Locations[FindLocation(PChar.GenQuest.PGG_Quest.Island.Shore)],"disableencounters");
		chr.Dialog.CurrentNode = "Second Time";
		chr.PGGAi.IsPGG = true;
		chr.RebirthPhantom = true;
		// LEO: бэкапа может не быть - квест не всегда менял нацию, а голое чтение сыпало в error.log.
		//   Отработавший бэкап удаляем, иначе устаревшая нация лежит в сейве до конца игры.
		if (CheckAttribute(chr, "Nation.Bak"))
		{
			chr.Nation = chr.Nation.Bak;
			DeleteAttribute(chr, "Nation.Bak");
		}
		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		SetCharacterRemovable(chr, true);

		LAi_LocationFightDisable(&Locations[FindLocation("Ship_deck")], false);

		DeleteAttribute(PChar, "DisableBIFace");
		DeleteAttribute(chr, "PGGAi.ActiveQuest");
		DeleteAttribute(chr, "AlwaysFriend");
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");

		if(!CheckAttribute(chr, "PGGAfterShare"))
		{
			chr.PGGAi.location.town = PGG_FindRandomTown(chr);
		}
		else
		{
			DeleteAttribute(chr, "PGGAfterShare");
		}
		ChangeCharacterAddressGroup(chr, "None", "", "");
		LAi_group_Delete("PGGTmp"); //попробуем потереть в конце всего.
	}
	DeleteAttribute(PChar,"Quest.PGGQuest1.SeaBattle");
	if (CheckAttribute(pchar,"ScamFanActive") && !CheckAttribute(pchar,"ScamDestroyed"))
	{
		chr = CharacterFromID("ScamCharacter");
		LAi_SetActorTypeNoGroup(chr);
		LAi_ActorFollowEverywhere(chr, "", -1);
		chr.chr_ai.tmpl.state = "stay";
	}
}

void PGG_Q1Time2Late(string qName)
{
	int i;
	string attrName;
	ref chr;
	aref arOldMapPos;
	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	if (CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid.Dead"))
	{
		string PGGQuestID = "Gen_PGGQuest1";
		if (chr.sex == "woman") PGGQuestID = "Gen_PGGQuest1" + chr.sex;
		AddQuestRecord(PGGQuestID, "q1_PGGDeadAndTimeIsUp");
		CloseQuestHeader(PGGQuestID);
		PChar.Quest.PGGQuest1_CheckStartState.Over = "yes";
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
		return;
	}
	if (!bQuestCheckProcessFreeze)
	{
		if (IsEntity(worldMap))
		{
			PChar.Quest.PGGQuest1_Time2Late_01.over = "yes"; // сброс, если уже откладывали
			PChar.Quest.PGGQuest1_Time2Late_02.over = "yes";

			makearef(arOldMapPos, worldMap.old);
			WdmPrepareMapForAbordage(arOldMapPos);
			MakeCloneShipDeck(pchar, true); // подмена палубы
			LAi_LocationFightDisable(&Locations[FindLocation("Ship_deck")], true);
			DoReloadFromWorldMapToLocation("Ship_deck", "goto", "goto7");
			LAi_LockFightMode(pchar, true);

			ChangeCharacterAddressGroup(chr, "Ship_deck", "goto", "goto2");
			LAi_group_MoveCharacter(chr, "GroupDeck");
			chr.Dialog.CurrentNode = "Quest_1_Time2Late";

			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 3.0, 0); // все работает, смотрите как работают темплейты в АИ актеров. eddy

			// ==> антураж
			for (i=1; i<5; i++)
			{
				attrName = "pirate_" + i;
				Log_TestInfo(attrName);
				chr = GetCharacter(NPC_GenerateCharacter(attrName, attrName, "man", "man", 15, PIRATE, 1, true));
				SetFantomParam(chr);
				chr.dialog.Filename = "Quest\sailors_dialog.c";
				LAi_SetWarriorType(chr);
				LAi_SetLoginTime(chr, 0.0, 24.0);
				LAi_SetHP(chr, 100.0, 100.0);
				ChangeCharacterAddressGroup(chr, "Ship_deck", "goto", "goto"+(i+2));
				LAi_group_MoveCharacter(chr, "GroupDeck");
			}
    		pchar.quest.Munity = "";  // признак выхода с палубы (закрыто, а то можно не говорить)
		}
		else
		{
			if (isLocationFreeForQuests(PChar.location) && !bSeaActive && !LAi_grp_alarmactive) //проверка на бой??
			{
				if (PlaceCharacter(chr, "goto", "random_must_be_near") != "")
				{
					PChar.Quest.PGGQuest1_Time2Late_01.over = "yes";
					PChar.Quest.PGGQuest1_Time2Late_02.over = "yes";

					chrDisableReloadToLocation = true;

					chr.Dialog.CurrentNode = "Quest_1_Time2Late";
					LAi_SetActorType(chr);
					LAi_ActorDialog(chr, pchar, "", 2.0, 0);
				}
				else
				{
					PChar.Quest.PGGQuest1_Time2Late_02.win_condition.l1 = "ExitFromLocation";
					PChar.Quest.PGGQuest1_Time2Late_02.win_condition.l1.Location = PChar.location;
					PChar.Quest.PGGQuest1_Time2Late_02.function = "PGG_Q1Time2Late";
					DeleteAttribute(PChar, "Quest.PGGQuest1_Time2Late_02.Over");
				}
			}
			else
			{
				PChar.Quest.PGGQuest1_Time2Late_02.win_condition.l1 = "ExitFromLocation";
				PChar.Quest.PGGQuest1_Time2Late_02.win_condition.l1.Location = PChar.location;
				PChar.Quest.PGGQuest1_Time2Late_02.function = "PGG_Q1Time2Late";
				DeleteAttribute(PChar, "Quest.PGGQuest1_Time2Late_02.Over");
			}
		}
	}
	else
	{
		// отложим до разморозки (пролет табличек)
		PChar.Quest.PGGQuest1_Time2Late_01.win_condition.l1 = "MapEnter";
		PChar.Quest.PGGQuest1_Time2Late_01.function = "PGG_Q1Time2Late";
	}
}

void PGG_Q1FightOnShore()
{
	ref chr;
	string attrName, sLoc, sLoc_2, relation;
	int i, iRnd, iNation;

	if (CheckAttribute(PChar,"PGG_EnemyPP") && !IsCompanion(CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid))) relation = LAI_GROUP_PLAYER_OWN;
	else
	{
		DeleteAttribute(PChar, "PGG_EnemyPP");
		relation = LAI_GROUP_PLAYER;
	}
	PChar.PGG_FightOnShore = true;
	sLoc = PChar.GenQuest.PGG_Quest.Island.Shore;
	iNation = sti(PChar.GenQuest.PGG_Quest.Nation);

	chrDisableReloadToLocation = true;
	//our
	iRnd = 10 + drand(15) + MOD_SKILL_ENEMY_RATE*3;
	if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid.Dead"))
	{
		chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
		ChangeCharacterAddressGroup(chr,pchar.location, "officers", "sea_1");
		LAi_SetWarriorType(chr);
		LAi_SetImmortal(chr, false);
		if (relation != LAI_GROUP_PLAYER_OWN) LAi_SetCheckMinHP(chr, 1, true, "PGG_CheckHP");
		LAi_group_MoveCharacter(chr, relation);
	}
	else
	{
		iRnd = makeint(iRnd/2);
	}
	PChar.GenQuest.PGG_Quest.GrpID.Qty = iRnd;
	PChar.GenQuestFort.FarLocator = false;
	sLoc = LAi_FindNPCLocator("officers_sea");
	for (i = 1; i < iRnd; i++)
	{
		
		if (i % 2 == 0 && !CheckAttribute(PChar,"PGG_EnemyPP"))
		{
			chr = SetFantomDefenceForts("officers", sLoc, PIRATE, LAI_GROUP_PLAYER);
		}
		else 
		{
			chr = SetFantomDefenceForts("officers", sLoc, PIRATE, relation);
			chr.wouldfight = true;
		}
		chr.id = "ally" + i;
		SetCharacterPerk(chr,PerksChars());
		FantomMakeCoolFighterWRankDepend(chr,sti(pchar.rank),25+drand(75),25+drand(75),50);	
	}
	Pchar.GenQuestFort.FarLocator = true;

	//enemy
	iRnd = 15 + drand(15) + MOD_SKILL_ENEMY_RATE*3 + GetOfficersQuantity(pchar);
	Pchar.GenQuestFort.FarLocator = true;
	sLoc = LAi_FindNPCLocator("goto");
	sLoc_2 = LAi_FindNPCLocator("smugglers");
	for (i = 1; i < iRnd; i++)
	{
		if (i % 2 == 0)
		{
			chr = SetFantomDefenceForts("goto", sLoc, iNation, "PGGTmp");
		}
		else
		{
			chr = SetFantomDefenceForts("smugglers", sLoc_2, iNation, "PGGTmp");
		}
		chr.id = "pirate_" + i;
		SetCharacterPerk(chr,PerksChars());
		FantomMakeCoolFighterWRankDepend(chr,sti(pchar.rank),25+drand(75),25+drand(75),50);
	}
	//натравим.
	LAi_group_SetHearRadius("PGGTmp", 100.0);
	LAi_group_FightGroupsEx("PGGTmp", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
	LAi_group_FightGroupsEx("PGGTmp", relation, true, Pchar, -1, false, false);
	LAi_group_FightGroupsEx("PGGTmp", "PGGAlly", true, Pchar, -1, false, false);

	if(CheckAttribute(PChar,"PGG_EnemyPP"))
	{
		LAi_group_FightGroupsEx(relation, LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
		LAi_group_SetCheck(relation, "PGG_Q1AfterShoreFight");
	}
	else LAi_group_SetCheck("PGGTmp", "PGG_Q1AfterShoreFight");
	
	LAi_SetFightMode(pchar, true);
	PChar.GenQuest.PGG_Quest.GrpID = "PGGTmp";
}

void PGG_Q1AfterShoreFight()
{
	ref chr, rGroup;
	string sGroup;
	sGroup = PChar.GenQuest.PGG_Quest.GrpID;

	//даю немного целевого товара в любом случае, даже если места нет... пусть разбираются :)
	PChar.GenQuest.PGG_Quest.Goods.Taken = 500 + drand(500) + MakeInt(GetSquadronFreeSpace(PChar, sti(PChar.GenQuest.PGG_Quest.Goods)) / (3 + drand(2)))
	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	LAi_RemoveCheckMinHP(chr); //fix
	if(CheckAttribute(PChar,"GenQuest.PGG_Quest.PGGid.Dead"))
	{
		SetCharacterGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods), sti(PChar.GenQuest.PGG_Quest.Goods.Taken) + GetCargoGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods)));
		string PGGQuestID = "Gen_PGGQuest1";
		if (chr.sex == "woman") PGGQuestID = "Gen_PGGQuest1" + chr.sex;
		AddQuestRecord(PGGQuestID, "q1_PGGDeadAfterBattle");
		AddQuestUserData(PGGQuestID, "sTaken", PChar.GenQuest.PGG_Quest.Goods.Taken);
		AddQuestUserData(PGGQuestID, "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(PChar.GenQuest.PGG_Quest.Goods)].Name + "Gen")));
		CloseQuestHeader(PGGQuestID);
		chrDisableReloadToLocation = false;
		PChar.Quest.PGGQuest1_EndExitSea.Over = "Yes";
		PChar.Quest.PGGQuest1_EndExitLoc.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";
		DeleteAttribute(PChar, "DisableBIFace");
		DeleteAttribute(PChar,"Quest.PGGQuest1.SeaBattle");
		DeleteAttribute(chr, "PGGAi.ActiveQuest");
		DeleteAttribute(chr, "AlwaysFriend");
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
		DeleteAttribute(PChar,"PGG_FightOnShore");
		DeleteAttribute(loadedLocation,"disableencounters");
		return;
	}
	if(!CheckAttribute(PChar,"PGG_EnemyPP"))
	{
		DoQuestCheckDelay("hide_weapon", 2.0);
		ChangeCharacterAddressGroup(chr,pchar.location,"reload", "sea");
		PChar.Quest.PGGQuest1_PGGDead.Over = "yes";
		chr.Dialog.CurrentNode = "Quest_1_SharePrise";
		LAi_SetImmortal(chr, false);
		LAi_SetActorType(chr);
		LAi_ActorFollow(chr,pchar, "", 8.0);
		LAi_SetStayType(chr);
		LAi_SetActorType(chr);
		LAi_ActorDialog(chr, pchar, "", 20.0, 0);
	}
	else
	{
		SetCharacterGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods), sti(PChar.GenQuest.PGG_Quest.Goods.Taken) + GetCargoGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods)));
		chrDisableReloadToLocation = false;
		PChar.Quest.PGGQuest1_EndExitSea.Over = "Yes";
		PChar.Quest.PGGQuest1_EndExitLoc.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";
		DeleteAttribute(PChar, "DisableBIFace");
		DeleteAttribute(PChar,"PGG_EnemyPP");
		DeleteAttribute(PChar,"Quest.PGGQuest1.SeaBattle");
		DeleteAttribute(chr, "PGGAi.ActiveQuest");
		DeleteAttribute(chr, "AlwaysFriend");
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
	}
	DeleteAttribute(PChar,"PGG_FightOnShore");
	DeleteAttribute(PChar,"Quest.PGGQuest1_PGGDead.PGG_Dead");
	DeleteAttribute(loadedLocation,"disableencounters");
}

void PGG_AddMoneyToCharacter(ref chr, int iMoney)
{
	int Norm = PGG_MONEY_CAP_BASE + sti(chr.rank)*PGG_MONEY_CAP_PER_RANK;
	// LEO: призовые за турниры арены НЕ подпадают под общий потолок - иначе выигрыш в пару миллионов
	//   срезался при первом же обычном доходе (в таблице у победителя так и оставалось 400 000 по рангу).
	//   Копим их отдельной суммой и на неё поднимаем персональный потолок: деньги реально накапливаются.
	if (CheckAttribute(chr, "PGG_ArenaPurse")) Norm = Norm + sti(chr.PGG_ArenaPurse);
	AddMoneyToCharacter(chr, iMoney);
	if (sti(chr.Money) > Norm) chr.Money = Norm;
}

void PGG_SpawnPGG()
{
	Group_SetAddress("EnemyPGG", PChar.GenQuest.PGG_Quest.Island,"","");
	Group_SetPursuitGroup("EnemyPGG", PLAYER_GROUP);
	Group_SetTaskAttack("EnemyPGG", PLAYER_GROUP);
	Group_LockTask("EnemyPGG");
}

//Выбрать случайного ПГГ из списка живущих.
//Фильтрация по полу и анимации
//Если отправить в поля пустые строки, выбор происходит без фильтрации
// LEO: КВЕСТ ВАЖНЕЕ ТУРНИРА. Отбор идёт в два круга: сперва берём свободных, и только если таких нет -
//   гостя арены, снимая его с турнира целиком (PGG_ReleaseFromTournamentForQuest). Оставлять его
//   «на двух стульях» нельзя: парковка вешает на персонажа общий immortal, и в сухопутной сцене квеста
//   он становится бессмертным - именно так ломались «Клан Ламбрини» и сцена клада.
string SelectRandomPGG(string sex, string animation)
{
	int i;
	ref sld;
	int selectPGG[100];
	int numPGG = 0;
	int arenaPGG[100];
	int numArena = 0;

	for (i = 1; i <= PsHeroQty; i++)
	{
		sld = CharacterFromID("PsHero_"+i);
		if (sld.PGGAi.location == "Dead" || IsCompanion(sld) || IsOfficer(sld)) continue;
		bool bSex = (sld.sex == sex) || (sex == "");
		bool bAnim = (sld.model.animation == animation) || (animation == "");
		if (bSex && bAnim)
		{
			if (CheckAttribute(sld, "PGG_Tournament"))
			{
				arenaPGG[numArena] = i;   // запасной круг: подойдёт, но сперва придётся снять с турнира
				numArena++;
			}
			else
			{
				selectPGG[numPGG] = i;
				numPGG++;
			}
		}
	}

	if (numPGG > 0)
	{
		i = rand(numPGG-1);//возвращаю рандомность, чтоб не первый подходящий
		sld = CharacterFromID("PsHero_" + selectPGG[i]);
		PGG_Disband_Fleet(sld);
		Log_TestInfo(GetConvertStrWithReplace("Variable_PsHero_13", "Logs.txt", "#space#", " ")+ GetFullName(sld));
		return sld.id;
	}

	if (numArena > 0)
	{
		i = rand(numArena-1);
		sld = CharacterFromID("PsHero_" + arenaPGG[i]);
		PGG_ReleaseFromTournamentForQuest(sld);   // с турнира снимаем полностью: место в сетке, стоянку и неуязвимость
		PGG_Disband_Fleet(sld);
		Log_TestInfo(GetConvertStrWithReplace("Variable_PsHero_13", "Logs.txt", "#space#", " ")+ GetFullName(sld));
		return sld.id;
	}
	Log_TestInfo(GetConvertStrWithReplace("Variable_PsHero_14", "Logs.txt", "#space#", " "))
	return "";
}

string SelectWhisperPGG()
{
	int i;
	ref sld;
	for (i = 1; i <= PsHeroQty; i++)
	{
		sld = CharacterFromID("PsHero_"+i);
		if (sld.name == GetConvertStrWithReplace("Variable_PsHero_4", "Names.txt", "#space#", " "))
		{
			return sld.id;
		}
	}
	return "";
}
