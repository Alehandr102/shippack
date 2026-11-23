// Gregg: вынесено из GenQuests.c (сплит god-файла) — квест «Повод поспешить» (ReasonToFast).
// #include на исходное место — file-scope глобалы/дефайны остаются в GenQuests.c выше include; поведение идентично.

void ReasonToFast_CheckHorse(string qName)
{
	if(!CheckAttribute(pchar, "questTemp.ReasonToFast.speakHorse"))
	{
		DeleteAttribute(pchar, "questTemp.ReasonToFast");
	}
}

void ReasonToFast_MeetPatrolShore(string qName) // генерация патруля
{
	aref grp;
	ref chr, rCharacter;
	string encGroup, str, locator;
	int num, i, iNation, n;
	int iRank = sti(pchar.rank);
	ref pLoc;
	bool bOk = false;

	pLoc = &locations[FindLocation(pchar.questTemp.ReasonToFast.PatrolLocation)];
	encGroup = LAi_FindRandomLocator("encdetector");
	if (!isLocationFreeForQuests(pLoc.id) || encGroup == "") return;
	str = "locators." + encGroup;
	makearef(grp, pLoc.(str));
	iNation = sti(pchar.questTemp.ReasonToFast.GuardNation); // нация патруля
	num = GetAttributesNum(grp); //кол-во человек в патруле
	if (num <= 0) num = 1;

	str = "Patrol"+ pLoc.index + "_";
	string sGroup = "PatrolGroup_" + pLoc.index; //имя группы
	LAi_group_SetLookRadius(sGroup, 30);
	LAi_group_SetHearRadius(sGroup, 30);

	pchar.questTemp.ReasonToFast = "MeetPatrol";
	for (i = 0; i < num; i++)
	{
		if (i == 0)
		{
			int iPatrolOfficer = NPC_GenerateCharacter(str + i, "off_"+NationShortName(iNation)+"_"+(rand(1)+1), "man", "man", iRank + 2, iNation, 1, true);
			chr = &characters[iPatrolOfficer];
			SetFantomParamFromRank(chr, iRank + 2, true);
			chr.greeting = "soldier_arest";
			chr.dialog.filename = "Enc_Patrol.c";
			if(GetOfficersQuantity(pchar) > 0)
			{
				chr.dialog.currentnode = "First Time";
				pchar.questTemp.ReasonToFast = "MeetPatrolFail";
			}
			else
			{
				if(sti(pchar.questTemp.ReasonToFast.p1) > 40 + GetSummonSkillFromName(pchar, SKILL_FORTUNE)/2)
				{
					chr.dialog.currentnode = "First Time";
					pchar.questTemp.ReasonToFast = "MeetPatrolFail";
				}
				else
				{
					chr.dialog.currentnode = "Reason_To_Fast_1";
					bOk = true;
				}
			}
		}
		else
		{
			if (i == 3)
			{
				chr = GetCharacter(NPC_GenerateMusketeer(str + i, NationShortName(iNation)+"_rifle_"+(rand(2)+1), "man", iRank, iNation, 1, false));
				chr.dialog.filename = "Enc_Patrol.c";
				chr.dialog.currentnode = "Node_2";
			}
			else
			{
				chr = GetCharacter(NPC_GenerateCharacter(str + i, "sold_"+NationShortName(iNation)+"_"+(rand(7)+1), "man", "man", iRank, iNation, 1, true));
				SetFantomParamFromRank(chr, iRank, true);
				chr.dialog.filename = "Enc_Patrol.c";
				chr.dialog.currentnode = "Node_2";
			}
		}
		chr.City = pchar.questTemp.ReasonToFast.city;
		chr.CityType = "soldier";
		chr.EncQty = num;
		LAi_SetStayType(chr);
		LAi_group_MoveCharacter(chr, sGroup);
		pchar.questTemp.ReasonToFast.LocIdx = pLoc.index;
		pchar.questTemp.ReasonToFast.PatrolNum = num;
		//Получим локатор для логина
		locator = GetAttributeName(GetAttributeN(grp, i));
		ChangeCharacterAddressGroup(chr, pLoc.id, encGroup, locator);
		LAi_SetActorType(chr);
		if (i == 0) LAi_ActorDialog(chr, pchar, "", -1, 0);
		else LAi_ActorFollow(chr, &characters[iPatrolOfficer], "", -1);
	}
	if(bOk) // если это правильный патруль
	{
		LAi_LocationFightDisable(pLoc, true);
		LAi_SetFightMode(pchar, false);
		LAi_LockFightMode(pchar, true);
		chrDisableReloadToLocation = true;
	}
}

void ReasonToFast_DeletePatrolFromShore(string qName) //  прибъем патруль на берегу после разговора
{
	ref pLoc, rChar;

	int iPatrolCount = sti(pchar.questTemp.ReasonToFast.PatrolNum);
	pLoc = &locations[FindLocation(pchar.questTemp.ReasonToFast.PatrolLocation)];
	LAi_LocationFightDisable(pLoc, false);
	string str = "Patrol"+ pLoc.index + "_";
	string sTemp = "PatrolGroup_" + pchar.questTemp.ReasonToFast.LocIdx;
	for(int i=0; i<iPatrolCount; i++)
	{
		if(i == 0) rChar = CharacterFromID(str + i);
		else rChar = CharacterFromID(str + i);
		rChar.LifeDay = 0;
	}
	LAi_group_RemoveCheck(sTemp);
	LAi_group_Delete(sTemp);
}

void ReasonToFast_SpeakMayor(string qName)
{
	if(CheckAttribute(pchar,"questTemp.ReasonToFast.speakAfterPatrolDied") && pchar.questTemp.ReasonToFast.speakAfterPatrolDied == false)
	{
		AddQuestRecord("ReasonToFast", "13");
		AddQuestUserData("ReasonToFast", "sSex", GetSexPhrase("ся","ась"));
		CloseQuestHeader("ReasonToFast");
		DeleteAttribute(pchar, "questTemp.ReasonToFast");
	}
}

void ReasonToFast_GenerateHunter() // генерация ОЗГ
{
	ref chr;

	int iRank = sti(pchar.rank) + rand(MOD_SKILL_ENEMY_RATE*3);
	chr = GetCharacter(NPC_GenerateCharacter("ReasonToFast_Hunter", "officer_"+(rand(42)+1), "man", "man", iRank, PIRATE, -1, true));
	FantomMakeCoolFighter(chr, iRank, 80, 80, "topor2", "pistol3", 50);
	int jewelType = sti(pchar.questTemp.ReasonToFast.p4);
	string sItem = "jewelry" + jewelType;
	AddItems(chr, sItem, 15);
	chr.SaveItemsForDead = true;
	chr.dialog.Filename = "GenQuests_Dialog.c";
	chr.greeting = "Enc_Raiders";
	SetRandomNameToCharacter(chr);
	ChangeCharacterAddressGroup(chr, pchar.location, "reload", "reload1");
	LAi_group_MoveCharacter(chr, "PatrolGroup_" + pchar.questTemp.ReasonToFast.LocIdx);

	LAi_SetActorType(pchar);
	LAi_ActorTurnToCharacter(pchar, chr);
	LAi_SetActorType(chr);
	SetActorDialogAny2Pchar(chr.id, "", 0.0, 1.0);
	LAi_ActorFollow(chr, pchar, "ActorDialog_Any2Pchar", -1);
}

int ReasonToFast_GetVictimShipType()
{
	string sTemp;
	int iShipType = rand(3);
	switch (iShipType)
	{
	    case 0:
			iShipType = SHIP_BARQUE;
			sTemp = "Барк";
		break;
	    case 1:
			iShipType = SHIP_FLEUT;
			sTemp = "Флейт";
		break;
	    case 2:
			iShipType = SHIP_ROTERLOW;
			sTemp = "Галеон";
		break;
	    case 3:
			iShipType = SHIP_PINNACE;
			sTemp = "Пинас";
		break;
	}
	pchar.questTemp.ReasonToFast.ShipTypeName = sTemp;

	return iShipType;
}

int ReasonToFast_GetVictimShipGoods()
{
	int aGoods[7];
	aGoods[0] = GOOD_SANDAL;
	aGoods[1] = GOOD_TOBACCO;
	aGoods[2] = GOOD_COFFEE;
	aGoods[3] = GOOD_MAHOGANY;
	aGoods[4] = GOOD_SLAVES;
	aGoods[5] = GOOD_EBONY;
	aGoods[6] = GOOD_SILK;
	return aGoods[rand(6)];
}

void ReasonToFast_GenerateVictimShip(int iShipType, int iShipGoods) // генерация призового корабля
{
	int hcrew;
	int iNation = sti(pchar.questTemp.ReasonToFast.GuardNation);

	ref chref = GetCharacter(NPC_GenerateCharacter("VictimCap_1", "officer_"+(rand(42)+1), "man", "man", 25, iNation, -1, true));

	chref.Ship.Type = GenerateShipExt(iShipType, 1, chref);
	chref.Ship.Name = pchar.questTemp.ReasonToFast.ShipName;
    SetBaseShipData(chref);

    hcrew = GetMinCrewQuantity(chref);
    SetCrewQuantity(chref, hcrew);

    DeleteAttribute(chref,"ship.sails");
    RepairMasts(chref);
    DeleteAttribute(chref,"ship.blots");

	Fantom_SetCannons(chref, "trade");
    Fantom_SetBalls(chref, "trade");

	int iSpace = GetCharacterFreeSpace(chref, iShipGoods);
	Fantom_SetCharacterGoods(chref, iShipGoods, iSpace, 1);

	string sGroup = "Sea_Victim_1";
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
    Group_SetType(sGroup, "trade");

    Group_AddCharacter(sGroup, "VictimCap_1");
    Group_SetGroupCommander(sGroup, "VictimCap_1");
	Group_SetAddress(sGroup, GetArealByCityName(pchar.questTemp.ReasonToFast.city),"quest_ships", "quest_ship_1");
	Group_SetTaskRunaway(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);

	pchar.quest.ReasonToFast_SetShip_1.win_condition.l1 = "Character_sink";
    pchar.quest.ReasonToFast_SetShip_1.win_condition.l1.character = "VictimCap_1";
    pchar.quest.ReasonToFast_SetShip_1.function = "ReasonToFast_KillVictim";

	pchar.quest.ReasonToFast_SetShip_2.win_condition.l1 = "Character_Capture";
    pchar.quest.ReasonToFast_SetShip_2.win_condition.l1.character = "VictimCap_1";
    pchar.quest.ReasonToFast_SetShip_2.function = "ReasonToFast_CaptureVictim";

    SetTimerCondition("ReasonToFast_ClearVictim", 0, 0, 1, false);
}

// кэп пинаса утоп вместе с кораблём или ГГ уплыл на глобалку
void ReasonToFast_KillVictim(string qName)
{
	ref sld = characterFromId("VictimCap_1");
	sld.lifeDay = 0;
	pchar.quest.ReasonToFast_ClearVictim.over = "yes";
	pchar.quest.ReasonToFast_SetShip_1.over = "yes";
	pchar.quest.ReasonToFast_SetShip_2.over = "yes";
	DeleteAttribute(pchar, "GenQuest.CannotWait");
	AddQuestRecord("ReasonToFast","19");
	CloseQuestHeader("ReasonToFast");
	Group_DeleteGroup("Sea_Victim_1");
	DeleteAttribute(pchar,"questTemp.ReasonToFast");
}

// кэп пинаса захвачен или прибит на абордаже
void ReasonToFast_CaptureVictim(string qName)
{
	ref sld = characterFromId("VictimCap_1");
	sld.lifeDay = 0;
	AddQuestRecord("ReasonToFast","18");
	Group_DeleteGroup("Sea_Victim_1");
	pchar.questTemp.ReasonToFast.SpeakOther = true; // допрыгался, к губеру или призонбоссу теперь низзя !!!!
	pchar.quest.ReasonToFast_ClearVictim.over = "yes";
	pchar.quest.ReasonToFast_SetShip_1.over = "yes";
	pchar.quest.ReasonToFast_SetShip_2.over = "yes";
	DeleteAttribute(pchar, "GenQuest.CannotWait");
	pchar.quest.ReasonToFast_SetPirateShip.win_condition.l1 = "MapEnter";
    pchar.quest.ReasonToFast_SetPirateShip.function = "ReasonToFast_PreparePirateShip";
}

void ReasonToFast_RemoveVictim()
{
	ref sld = characterFromId("VictimCap_1");
	sld.lifeDay = 0;
	Group_DeleteGroup("Sea_Victim_1"); // прибъем пинас
	pchar.quest.ReasonToFast_ClearVictim.over = "yes";
	pchar.quest.ReasonToFast_SetShip_1.over = "yes";
	pchar.quest.ReasonToFast_SetShip_2.over = "yes";
	DeleteAttribute(pchar, "GenQuest.CannotWait");
}

void ReasonToFast_PreparePirateShip(string qName) // генерация пиратского корабля
{
	ref sld;
	int iRank = sti(pchar.rank) + 5 + rand(MOD_SKILL_ENEMY_RATE*3);
	int iShipRank, iShipType;
	string sTemp, sTemp1;

    if(makeint(pchar.rank) > 13) { iShipRank = rand(2) + 3; }
    if(makeint(pchar.rank) > 6 && makeint(pchar.rank) < 13) { iShipRank = rand(2); }
	if(makeint(pchar.rank) < 7) { iShipRank = rand(1); }

	switch (iShipRank)
	{
	    case 0:
			iShipType = SHIP_SCHOONER;
			sTemp1 = "шхуна";
		break;
	    case 1:
			iShipType = SHIP_BRIG;
			sTemp1 = "бриг";
		break;
	    case 2:
			iShipType = SHIP_BRIGANTINE;
			sTemp1 = "бригантина";
		break;
	    case 3:
			iShipType = SHIP_CORVETTE;
			sTemp1 = "корвет";
		break;
	    case 4:
			iShipType = SHIP_FRIGATE;
			sTemp1 = "фрегат";
		break;
	    case 5:
			iShipType = SHIP_LINESHIP;
			sTemp1 = "военный корабль";
		break;
	}

	Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_11", "Logs.txt", "#space#", " "));

	sld = GetCharacter(NPC_GenerateCharacter("PirateCapt", "officer_"+(rand(42)+1), "man", "man", iRank, PIRATE, 30, true));
	sld.dialog.filename      = "GenQuests_Dialog.c";
	sld.dialog.currentnode   = "ReasonToFast_FightCap";
	sTemp = GenerateRandomNameToShip(PIRATE);
	FantomMakeCoolSailor(sld, iShipType, sTemp, CANNON_TYPE_CULVERINE_LBS24, 50+rand(40), 50+rand(40), 50 + rand(40));
	sld.cirassId = Items_FindItemIdx("cirass5");  // предмета нет, но влияение есть
    FantomMakeCoolFighter(sld, makeint(pchar.rank) + rand(10) + 5, 60 + rand(30), 50, "blade25", "pistol3", 80);

    sld.SuperShooter  = true;
	SetCharacterPerk(sld, "MusketsShoot");
	SetCharacterPerk(sld, "Energaiser"); 					// скрытый перк даёт 1.5 к приросту энергии, дается ГГ и боссам уровней

	LAi_SetHP(sld, 100 + makeint(pchar.rank) * 2, 100 + makeint(pchar.rank) * 2);

	sld.mapEnc.type = "trade";
	sld.mapEnc.worldMapShip = "quest_ship";
	sld.mapEnc.Name = sTemp1 + " '" + sTemp + "'";

	sld.ship.Crew.Morale = 60 + rand(20);
	ChangeCrewExp(sld, "Sailors", 60 + rand(20));
	ChangeCrewExp(sld, "Cannoners", 60 + rand(20));
	ChangeCrewExp(sld, "Soldiers", 60 + rand(20));

	string sGroup = "Quest_Ship";
	Group_DeleteGroup(sGroup);
    Group_FindOrCreateGroup(sGroup);
	Group_AddCharacter(sGroup, "PirateCapt");
    Group_SetGroupCommander(sGroup, "PirateCapt");

	SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
    Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_SetTaskAttack(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);

	SetNationRelation2MainCharacter(PIRATE, RELATION_ENEMY);

	Map_CreateWarrior("", "PirateCapt", 7);

    pchar.quest.ReasonToFast_PirateShip_Dead.win_condition.l1 = "NPC_Death";
	pchar.quest.ReasonToFast_PirateShip_Dead.win_condition.l1.character = "PirateCapt";
	pchar.quest.ReasonToFast_PirateShip_Dead.function = "ReasonToFast_PirateCapt_Dead";

	pchar.quest.ReasonToFast_PirateShip_Sink.win_condition.l1 = "Character_sink";
    pchar.quest.ReasonToFast_PirateShip_Sink.win_condition.l1.character = "PirateCapt";
    pchar.quest.ReasonToFast_PirateShip_Sink.function = "ReasonToFast_PirateCapt_Sink";
}

void ReasonToFast_PirateCapt_Dead(string qName)
{
	ref sld = characterFromId("PirateCapt");
	sld.lifeDay = 0;
	pchar.quest.ReasonToFast_PirateShip_Sink.over = "yes";
	pchar.quest.ReasonToFast_PirateShip_Dead.over = "yes";
	AddQuestRecord("ReasonToFast","22");
	AddQuestUserData("ReasonToFast", "sName", GetName( NAMETYPE_MAIN, pchar.questTemp.ReasonToFast.p3, NAME_DAT));
	CloseQuestHeader("ReasonToFast");
	DeleteAttribute(pchar,"questTemp.ReasonToFast");
}

void ReasonToFast_PirateCapt_Sink(string qName)
{
	ref sld = characterFromId("PirateCapt");
	sld.lifeDay = 0;
	pchar.quest.ReasonToFast_PirateShip_Sink.over = "yes";
	pchar.quest.ReasonToFast_PirateShip_Dead.over = "yes";
	AddQuestRecord("ReasonToFast","25");
	AddQuestUserData("ReasonToFast", "sName", GetName( NAMETYPE_MAIN, pchar.questTemp.ReasonToFast.p3, NAME_DAT));
	CloseQuestHeader("ReasonToFast");
	DeleteAttribute(pchar,"questTemp.ReasonToFast");
}

void ReasonToFast_SetHunterCoastal()
{
	pchar.quest.ReasonToFast_SetHunterPort_1.win_condition.l1 = "Location_Type";
	pchar.quest.ReasonToFast_SetHunterPort_1.win_condition.l1.location_type = "town";
	pchar.quest.ReasonToFast_SetHunterPort_1.function = "ReasonToFast_SetTreasureHunter";

	pchar.quest.ReasonToFast_SetHunterPort_2.win_condition.l1 = "Location_Type";
	pchar.quest.ReasonToFast_SetHunterPort_2.win_condition.l1.location_type = "seashore";
	pchar.quest.ReasonToFast_SetHunterPort_2.function = "ReasonToFast_SetTreasureHunter";
}

void ReasonToFast_SetTreasureHunter(string qName)
{
    int    j, i, k;
	string sTemp, sCapId, sLocGroup;
	ref    sld;
	bool   ok;

	pchar.quest.ReasonToFast_SetHunterPort_1.over = "yes";
	pchar.quest.ReasonToFast_SetHunterPort_2.over = "yes";

	LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false);

	Pchar.GenQuest.Hunter2Pause  = true;
	sCapId = "LandHunter0";
	sTemp = "LAND_HUNTER";
	ok = true;
	arrayNPCModelHow = 0;

    for (i = 1; i <= 3; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_hol_2", "man", "man", 5, PIRATE, 1, true));
        SetFantomParamHunter(sld);
        sld.dialog.filename = "Hunter_dialog.c";
		if(pchar.questTemp.ReasonToFast == "GetMap" || pchar.questTemp.ReasonToFast == "LakeyExitSuccess") // получена карта или сокровища от лакея
		{
			sld.Dialog.CurrentNode = "ReasonToFast_HunterShore1";
		}
		if(pchar.questTemp.ReasonToFast == "LakeyExitFail") // лакей сбежал
		{
			sld.Dialog.CurrentNode = "ReasonToFast_HunterShore2";
		}
		if(pchar.questTemp.ReasonToFast == "GetTreasure") // ГГ взял сокровища из клада
		{
			sld.Dialog.CurrentNode = "ReasonToFast_THunter_1";
		}
        sld.greeting = "Gr_HUNTER";
        sld.location = "none"; // вот где порылась собака!!!!!!!!!!!

		SetOZGModel(sld);
        k = 0;
		while (!CheckNPCModelUniq(sld) && k < 10)
		{
		    k++;
			SetOZGModel(sld);
		}
		arrayNPCModel[arrayNPCModelHow] = sld.model;
		arrayNPCModelHow++;

		LAi_SetActorTypeNoGroup(sld);

		if(LAi_GetLocatorNum("goto") > 0) 	sLocGroup = "goto";
		else								sLocGroup = "encdetector";

		if (PlaceCharacter(sld, sLocGroup, "random_must_be_near") == "" && i == 1) // fix если вдруг нет в локации
        {
            ok = false;
            break;
        }
		LAi_ActorFollow(sld, pchar, "", 8.0);
        LAi_group_MoveCharacter(sld, sTemp);
	}
	LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER_OWN, LAI_GROUP_NEITRAL);

	LAi_group_ClearAllTargets();
	LAi_SetFightModeForOfficers(false);
	if (ok)
    {
        pchar.HunterCost.Qty = i;
        sld = characterFromID(sCapId + "1");
        LAi_type_actor_Reset(sld);
        LAi_ActorDialog(sld, pchar, "", 4.0, 0);
		chrDisableReloadToLocation = true;
		bDisableFastReload = true;
    }
}

void ReasonToFast_GenerateTreasureMap(ref item)
{
    item.MapIslId   = GetIslandForTreasure();
    item.MapLocId   = GetLocationForTreasure(item.MapIslId);
    item.MapBoxId   = GetBoxForTreasure(item.MapIslId, item.MapLocId);
	item.MapOwnId   = GetName( pchar.questTemp.ReasonToFast.mapIdx, pchar.questTemp.ReasonToFast.map, NAME_NOM);

	DeleteAttribute(item, "BoxTreasure");

	pchar.questTemp.ReasonToFast.Treasure.Island = item.MapIslId;
	pchar.questTemp.ReasonToFast.Treasure.Location = item.MapLocId;
	pchar.questTemp.ReasonToFast.Treasure.Locator = item.MapBoxId;

    pchar.quest.ReasonToFast_SetTreasureFromMap.win_condition.l1  = "location";
    pchar.quest.ReasonToFast_SetTreasureFromMap.win_condition.l1.location = item.MapLocId;
    pchar.quest.ReasonToFast_SetTreasureFromMap.function = "ReasonToFast_SetTreasureBoxFromMap";
}

void ReasonToFast_ExitFromTownFight(string qName)
{
	Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_12", "Logs.txt", "#space#", " "));
	bQuestDisableMapEnter = true;
	QuestCloseSeaExit();
    DoQuestCheckDelay("ReasonToFast_MeetWithSailor", 1);
}

void ReasonToFast_SetTreasureBoxFromMap(string qName)
{
    aref   item;
    ref    loc;
    string box, itmName;
    aref   arToBox;
    aref   arFromBox;


    if (GetCharacterItem(pchar, "mapQuest") > 0 )
    {

        Log_Info(GetConvertStrWithReplace("Variable_GenQuests_13", "Logs.txt", "#space#", " "));

		//Запрет спавна проклятого кэпа в этой локации
		ref locLoad = &locations[reload_location_index];
		pchar.treasurelocation = locLoad.id;

        PlaySound("interface\notebook.wav");
		Statistic_AddValue(pchar, "Treasure", 1);

        // немного веселой жизни
        pchar.quest.ReasonToFast_SetTreasureHunter.win_condition.l1          = "ExitFromLocation";
        pchar.quest.ReasonToFast_SetTreasureHunter.win_condition.l1.location = pchar.location;
        pchar.quest.ReasonToFast_SetTreasureHunter.function    				= "ReasonToFast_SetTreasureHunter";
		bDisableLandEncounters = true;

        Items_FindItem("mapQuest", &item);

		itmName = "icollection"; // генерим сокровища
		item.BoxTreasure.(itmName) = sti(pchar.questTemp.ReasonToFast.p8);

		itmName = "Chest"; // генерим сундуки
		item.BoxTreasure.(itmName) = sti(pchar.questTemp.ReasonToFast.p7);

		itmName = "mineral5"; // генерим ведра
		item.BoxTreasure.(itmName) = rand(5) + 2;

		itmName = "mineral8"; // генерим  башмаки
		item.BoxTreasure.(itmName) = rand(7) + 4;

        box = item.MapBoxId;

        loc = &locations[FindLocation(item.MapLocId)];
        loc.(box).items = "";

        makearef(arToBox, loc.(box).items);
        makearef(arFromBox, item.BoxTreasure);
        CopyAttributes(arToBox, arFromBox);

        loc.(box) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
        loc.(box).Treasure =  true; // признак сокровища в сундуке

        DeleteAttribute(item, "MapIslId");
        TakeNItems(pchar, "mapQuest", -1);
		pchar.questTemp.ReasonToFast = "GetTreasure";
    }
}

void ReasonToFast_ClearTreasureBox(string LocId)
{
	ref loc;
	aref boxLocator;
	string locatorName;

	loc = &locations[FindLocation(LocId)];

	for (int i=1; i < MAX_HANDLED_BOXES; i++)
	{
		locatorName = "box"+i;
		if(!CheckAttribute(loc, locatorName)) break;
		DeleteAttribute(loc, locatorName + ".items");
		loc.(locatorName).items = "";
		loc.(locatorName).money = 0;
		loc.(locatorName) = Items_MakeTime(0, 0, 1, 2003);
	}
}

int ReasonToFast_CheckTreasure(string ItmName, int ReqQty)
{
	ref loc;
	string locatorName, sName;
	aref arItems, arCur;
	int itemsQty;

	loc = &locations[FindLocation(pchar.questTemp.ReasonToFast.Treasure.Location)];
	locatorName = pchar.questTemp.ReasonToFast.Treasure.Locator;

	makearef(arItems, loc.(locatorName).items)

    int Qty = GetAttributesNum(arItems);
    for (int a = 0; a < Qty; a++)
    {
        sName = GetAttributeName(GetAttributeN(arItems, a));
		if(sName == ItmName)
		{
			arCur = GetAttributeN(arItems,a);
			itemsQty = sti(GetAttributeValue(arCur));
			return ReqQty - itemsQty;
		}
    }
	return ReqQty;
}

int ReasonToFast_CheckTreasureQty(string ItmName, int ReqQty)
{
	int iQty, iNeedQty = 0;
	int iPassenger, cn;
	ref sld;

	iQty = GetCharacterItem(pchar, ItmName);
	if(iQty > 0)
	{
		iNeedQty += iQty;
		if(iNeedQty >= ReqQty) return iNeedQty;
	}
	for (int io = 0; io < GetPassengersQuantity(pchar); io++)
	{
		iPassenger = GetPassenger(pchar, io);
		if (iPassenger != -1)
		{
			sld = GetCharacter(iPassenger);
			iQty = GetCharacterItem(sld, ItmName);
			if(iQty > 0)
			{
				iNeedQty += iQty;
				if(iNeedQty >= ReqQty) return iNeedQty;
			}
		}
	}
	return iNeedQty;
}

void ReasonToFast_GetTreasure(string ItmName, int ReqQty)
{
	int iQty;
	int iPassenger, cn;
	ref sld;

	// отберем предметы сначала у ГГ
	iQty = GetCharacterItem(pchar, ItmName);
	if(iQty > 0)
	{
		TakeNItems(pchar, ItmName, -iQty);
		if(iQty == ReqQty) return;
		ReqQty = ReqQty - iQty;
	}
	// теперь у офицеров и пассажиров
	for (int io = 0; io < GetPassengersQuantity(pchar); io++)
	{
		iPassenger = GetPassenger(pchar, io);
		if (iPassenger != -1)
		{
			sld = GetCharacter(iPassenger);
			iQty = GetCharacterItem(sld, ItmName);
			if(iQty > 0)
			{
				TakeNItems(sld, ItmName, -iQty);
				if(iQty == ReqQty) return;
				ReqQty = ReqQty - iQty;
			}
		}
	}
	// у компаньонов
	for (int i=1; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if(cn != -1)
		{
			sld = GetCharacter(cn);
			iQty = GetCharacterItem(sld, ItmName);
			if(iQty > 0)
			{
				TakeNItems(sld, ItmName, -iQty);
				if(iQty == ReqQty) return;
				ReqQty = ReqQty - iQty;
			}
		}
	}
}

bool ReasonToFast_CreateResidenceNPC(aref loc)
{
	ref chr;
	int iBaseCity = FindLocation(loc.fastreload + "_town");
	int iNation = GetCityNation(pchar.questTemp.ReasonToFast.city);

	if (!CheckAttribute(&locations[iBaseCity], "houseEnc")) return false;

	if (CheckAttribute(loc, "PlayHall") || CheckAttribute(loc, "Bedroom") || (loc.id.label  == "TownhallRoom") || CheckAttribute(loc, "Office"))
	{
		string slai_group = GetNationNameByType(iNation) + "_citizens";

		if (GetCharacterIndex("ReasonToFastLakey_1") > 0) return false; //если есть
		Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_14", "Logs.txt", "#space#", " "));
		chr = GetCharacter(NPC_GenerateCharacter("ReasonToFastLakey_1", "barmen_"+(rand(15)+1), "man", "man", 5, iNation, 1, true));
		chr.dialog.Filename = "GenQuests_Dialog.c";
		chr.dialog.currentnode = "ReasonToFast_Lakey";
		chr.CityType = "citizen";
		chr.city = loc.fastreload; //НЗГ
		chr.greeting = "worker";
		ChangeCharacterAddressGroup(chr, loc.id, "goto", "goto1");
		LAi_SetOwnerType(chr);
		LAi_group_MoveCharacter(chr, slai_group);
		pchar.questTemp.ReasonToFast.LakeyLocation = loc.id;
		bDisableFastReload = true;
	}
	return true;
}

void ReasonToFast_ClearLakey(string qName)
{
	AddQuestRecord("ReasonToFast","19");
	CloseQuestHeader("ReasonToFast");
	if (GetCharacterIndex("ReasonToFastLakey_1") > 0) // прибъем лакея если он есть
	{
		ref sld = CharacterFromID("ReasonToFastLakey_1");
		sld.LifeDay = 0;
	}
	DeleteAttribute(pchar,"questTemp.ReasonToFast");
}

void ReasonToFast_InitVariables()
{
	int s1, s2;

	pchar.questTemp.ReasonToFast.p1 = drand(100);
	pchar.questTemp.ReasonToFast.p2 = drand(100);
	pchar.questTemp.ReasonToFast.p3 = "l" + drand(GetNamesCount(NAMETYPE_MAIN) - 1); 	// id пиратского атамана
	pchar.questTemp.ReasonToFast.p4 = drand(3) + 1; 	// id камня (1-4)
	pchar.questTemp.ReasonToFast.p5 = (110 - GetSummonSkillFromName(pchar, SKILL_FORTUNE)) * MOD_SKILL_ENEMY_RATE*3 * 1000;
	pchar.questTemp.ReasonToFast.p6 = GenQuest_GenerateGoodBlade(); // генерация клинка
	s1 = GetCharacterSPECIAL(pchar,"Charisma");
	s1 = s1 - drand(makeint(s1/2));
	s2 = GetCharacterSPECIAL(pchar,"Intellect");
	s2 = s2 - drand(makeint(s2/2));
	pchar.questTemp.ReasonToFast.p7 = s1;
	pchar.questTemp.ReasonToFast.p8 = s2;
	s1 = GetCharacterSPECIAL(pchar,"Charisma");
	s2 = GetCharacterSPECIAL(pchar,"Intellect");
	pchar.questTemp.ReasonToFast.p9 = 50 * (5 + s1 - 2 * drand(makeint(s1/2)) + (s2 - 2 * drand(makeint(s2/2))) * 4) * (200 + GetSummonSkillFromName(pchar, SKILL_COMMERCE));
	pchar.questTemp.ReasonToFast.p10 = 4500000 * (s1 + 4 * s2)/(200 + GetSummonSkillFromName(pchar, SKILL_COMMERCE));
	pchar.questTemp.ReasonToFast.target = drand(1);
	if(drand(1) == 0)
	{
		pchar.questTemp.ReasonToFast.map = "l" + drand(GetNamesCount(NAMETYPE_NICK) - 1);
		pchar.questTemp.ReasonToFast.mapIdx = NAMETYPE_NICK;
	}
	else
	{
		pchar.questTemp.ReasonToFast.map = "l" + rand(GetNamesCount(NAMETYPE_VIP) - 1);
		pchar.questTemp.ReasonToFast.mapIdx = NAMETYPE_VIP;
	}
	// ручками трем этот атрибут :( , если квест взят во второй раз
	if(CheckAttribute(pchar,"questTemp.ReasonToFast.speakOther")) DeleteAttribute(pchar,"questTemp.ReasonToFast.speakOther");
	if(CheckAttribute(pchar,"questTemp.ReasonToFast.speakAfterPatrolDied")) DeleteAttribute(pchar,"questTemp.ReasonToFast.speakAfterPatrolDied");
}
