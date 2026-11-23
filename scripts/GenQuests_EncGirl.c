// Gregg: вынесено из GenQuests.c (сплит god-файла) — энкаунтер «Девушка» (EncGirl).
// #include на исходное место — file-scope глобалы/дефайны остаются в GenQuests.c выше include; поведение идентично.

void EncGirl_Saved()
{
	ref sld = CharacterFromID("CangGirl");
	sld.greeting = "Enc_RapersGirl_2";
	sld.dialog.filename = "Enc_RapersGirl_dialog.c";
	if(pchar.GenQuest.EncGirl == "toParents")
	{
		sld.dialog.currentnode = "Node_250"; // ушли без боевки
	}
	else
	{
		sld.dialog.currentnode = "ThanksForSave"; // грохнули бандюков
	}
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void EncGirl_GirlGoAway()
{
	ChangeCharacterReputation(pchar, -5);
	ref sld = CharacterFromID("CangGirl");
	LAi_SetActorType(sld);
	string sTemp = LAi_FindNearestFreeLocator2Pchar("reload");
	LAi_ActorGoToLocation(sld, "reload", sTemp, "none", "", "","", -1.0);
	sld.lifeDay = 0;
	DeleteAttribute(pchar, "GenQuest.EncGirl");
}

void EncGirl_toChurch(string qName)
{
	ref sld = CharacterFromID("CangGirl");
	ChangeCharacterAddressGroup(sld, pchar.GenQuest.EncGirl.city + "_church", "goto", "goto1");
	LAi_SetStayType(sld);
}

void EncGirl_GenQuest_GetCoins(string qName)
{
	ref sld = CharacterFromID("CangGirl");
	ChangeCharacterAddress(sld, "None", "");
	sld.lifeDay = 0;
	AddSimpleRumour(GetConvertStrWithReplace("Variable_GenQuests_1", "AddRumours.txt", "#space#", " ") + pchar.GenQuest.EncGirl.name + GetConvertStrWithReplace("Variable_GenQuests_2", "AddRumours.txt", "#space#", " "), sti(sld.nation), 5, 1);
	ChangeCharacterReputation(pchar, -10);
	AddQuestRecord("JungleGirl", "3");
	AddQuestUserData("JungleGirl", "sSex", GetSexPhrase("",GetConvertStrWithReplace("Variable_GenQuests_3", "AddRumours.txt", "#space#", " ")));
	AddQuestUserData("JungleGirl", "sName", pchar.GenQuest.EncGirl.name);
	CloseQuestHeader("JungleGirl");
	DeleteAttribute(pchar, "GenQuest.EncGirl");
}

void EncGirl_GenQuest_GetChestPlaceName()
{
	int iTemp = rand(4);
	switch (iTemp)
	{
		case 0:
			pchar.GenQuest.EncGirl.islandId = "Bermudes"
			pchar.GenQuest.EncGirl.placeId = "Bermudes_Cavern";
			pchar.GenQuest.EncGirl.shoreId = "Shore_ship1";
		break;
		case 1:
			pchar.GenQuest.EncGirl.islandId = "Caiman";
			pchar.GenQuest.EncGirl.placeId = "Caiman_Grot";
			pchar.GenQuest.EncGirl.shoreId = "Shore17";
		break;
		case 2:
			pchar.GenQuest.EncGirl.islandId = "Dominica";
			pchar.GenQuest.EncGirl.placeId = "Dominica_Grot";
			pchar.GenQuest.EncGirl.shoreId = "Shore26";
		break;
		case 3:
			pchar.GenQuest.EncGirl.islandId = "Terks";
			pchar.GenQuest.EncGirl.placeId = "Terks_Grot";
			pchar.GenQuest.EncGirl.shoreId = "Shore56";
		break;
		case 4:
			pchar.GenQuest.EncGirl.islandId = "StLucia";
			pchar.GenQuest.EncGirl.placeId = "StLucia_Grot";
			pchar.GenQuest.EncGirl.shoreId = "Shore66";
		break;
	}
}

void EncGirl_Death(string qName)
{
	AddQuestRecord("JungleGirl", "5");
    AddQuestUserData("JungleGirl", "sName", pchar.GenQuest.EncGirl.name);
	CloseQuestHeader("JungleGirl");
	pchar.quest.EncGirl_FindChest.over = "yes";
	ChangeCharacterReputation(pchar, -30);
    pchar.quest.DeleteGirlGroup.win_condition.l1 = "ExitFromLocation";
    pchar.quest.DeleteGirlGroup.win_condition.l1.location = pchar.location;
    pchar.quest.DeleteGirlGroup.function = "EncGirl_DeleteGirlGroup";
}

void EncGirl_DeathAgain(string qName)
{
	AddQuestRecord("JungleGirl", "7");
	AddQuestUserData("JungleGirl", "sName", pchar.GenQuest.EncGirl.name);
	CloseQuestHeader("JungleGirl");
	ChangeCharacterReputation(pchar, -30);
	pchar.quest.EncGirl_DeliveBack.over = "yes";
	pchar.quest.DeleteGirlGroup.win_condition.l1 = "ExitFromLocation";
    pchar.quest.DeleteGirlGroup.win_condition.l1.location = pchar.location;
    pchar.quest.DeleteGirlGroup.function = "EncGirl_DeleteGirlGroup";
}

void EncGirl_DeathSimple(string qName)
{
	if(pchar.GenQuest.EncGirl == "toParents" || pchar.GenQuest.EncGirl == "FindLover")
	{
		AddQuestRecord("JungleGirl", "10");
		AddQuestUserData("JungleGirl", "sName", pchar.GenQuest.EncGirl.name);
		CloseQuestHeader("JungleGirl");
	}
	ChangeCharacterReputation(pchar, -30);
	pchar.quest.EncGirl_DeliveBack.over = "yes";
	pchar.quest.DeleteGirlGroup.win_condition.l1 = "ExitFromLocation";
    pchar.quest.DeleteGirlGroup.win_condition.l1.location = pchar.location;
    pchar.quest.DeleteGirlGroup.function = "EncGirl_DeleteGirlGroup";
}

void EncGirl_FindChest(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_LocationDisableMonstersGen(pchar.location, true);
	pchar.quest.EncGirl_Death.over = "yes";
	ref sld = characterFromId("CangGirl");
	sld.dialog.filename = "Enc_RapersGirl_dialog.c";
	sld.dialog.currentnode = "Node_132_12";
	LAi_SetActorTypeNoGroup(sld);
	GetCharacterPos(sld, &locx, &locy, &locz);
	ChangeCharacterAddressGroup(sld, pchar.location, "monsters", LAi_FindFarLocator("monsters", locx, locy, locz));
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void EncGirl_AddPassenger(string qName)
{
	ref sld = characterFromId("CangGirl");
	AddPassenger(pchar, sld, false);
	SetCharacterRemovable(sld, false);
	LAi_type_actor_Reset(sld);
	ChangeCharacterAddress(sld, "none", "");
}

void EncGirl_DialogAtShore(string qName)
{
	ref sld = characterFromId("CangGirl");
	sld.dialog.filename = "Enc_RapersGirl_dialog.c";
	sld.dialog.currentnode = "Node_132_11_1";
	PlaceCharacter(sld, "goto", pchar.location);
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
	pchar.quest.EncGirl_EnterToSea.win_condition.l1 = "EnterToSea";
	pchar.quest.EncGirl_EnterToSea.function = "EncGirl_EnterToSea";
	pchar.quest.EncGirl_Death.over = "yes";
}

void EncGirl_EnterToSea(string qName)
{
	ref sld = characterFromId("CangGirl");
	if(pchar.GenQuest.EncGirl == "ChestGetted") // забрали клад
	{
		LAi_type_actor_Reset(sld);
	}
	else // кинули тетку на острове, клад не стали брать
	{
		pchar.quest.EncGirl_DeliveBack.over = "yes"; // снимаем ненужные прерывания
		pchar.quest.EncGirl_DeathAgain.over = "yes";
		pchar.quest.EncGirl_FindChest.over = "yes";
		ChangeCharacterReputation(pchar, -5);
		AddQuestRecord("JungleGirl", "5_1");
		AddQuestUserData("JungleGirl", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("JungleGirl", "sName", pchar.GenQuest.EncGirl.name);
		CloseQuestHeader("JungleGirl");
		RemovePassenger(pchar, sld);
		ChangeCharacterAddress(sld, "none", "");
		sld.lifeDay = 0;
		DeleteAttribute(pchar, "GenQuest.EncGirl");
	}
}

void EncGirl_DeleteGirlGroup(string qName)
{
	LAi_group_Delete("DeliveQuest_Girl");
	DeleteAttribute(pchar, "GenQuest.EncGirl");
}

void EncGirl_DeliveBack(string qName)
{
	ref sld = characterFromId("CangGirl");
	sld.dialog.filename = "Enc_RapersGirl_dialog.c";
	if(pchar.GenQuest.EncGirl == "ChestGetted")
	{
		chrDisableReloadToLocation = true;
		LAi_type_actor_Reset(sld);
		sld.dialog.currentnode = "Node_132_18";
		pchar.quest.EncGirl_DeathAgain.over = "yes";
	}
	if(pchar.GenQuest.EncGirl == "FindCoins")
	{
		sld.dialog.currentnode = "Node_260";
		pchar.quest.EncGirl_DeathSimple.over = "yes";
	}
	if(pchar.GenQuest.EncGirl == "HorseToTavern")
	{
		sld.dialog.currentnode = "Node_225";
		pchar.quest.EncGirl_DeathSimple.over = "yes";
	}
	PlaceCharacter(sld, "goto", "random_must_be_near");
	if(pchar.GenQuest.EncGirl != "FindCoins")
	{
		LAi_SetActorType(pchar);
		LAi_ActorTurnToCharacter(pchar, sld);
		LAi_SetActorTypeNoGroup(sld);
		SetActorDialogAny2Pchar(sld.id, "", -1, 1.0);
		LAi_ActorFollow(sld, pchar, "ActorDialog_Any2Pchar", -1);
	}
	else
	{
		LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);
	}
}

void EncGirl_GenerateChest(ref chref)
{
	pchar.GenQuest.EncGirl.Treasure.Island = GetArealByCityName(chref.city);
	Log_TestInfo(pchar.GenQuest.EncGirl.Treasure.Island);
	pchar.GenQuest.EncGirl.Treasure.Location = GetLocationForTreasure(pchar.GenQuest.EncGirl.Treasure.Island);
	pchar.GenQuest.TreasureEncGirl = pchar.GenQuest.EncGirl.Treasure.Location;
	Log_TestInfo(pchar.GenQuest.EncGirl.Treasure.Location);
	pchar.GenQuest.EncGirl.Treasure.Locator = GetBoxForTreasure(pchar.GenQuest.EncGirl.Treasure.Island, pchar.GenQuest.EncGirl.Treasure.Location);
	pchar.GenQuest.TreasureEncGirlBox = pchar.GenQuest.EncGirl.Treasure.Locator;
	Log_TestInfo(pchar.GenQuest.EncGirl.Treasure.Locator);

	pchar.quest.EncGirl_FindTreasure.win_condition.l1 = "location";
	pchar.quest.EncGirl_FindTreasure.win_condition.l1.location = pchar.GenQuest.EncGirl.Treasure.Location;
	pchar.quest.EncGirl_FindTreasure.function = "EncGirl_FindTreasure";
}

void EncGirl_FindTreasure(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_LocationDisableMonstersGen(pchar.location, true);
	pchar.quest.EncGirl_DeathSimple.over = "yes";
	ref sld = characterFromId("CangGirl");
	sld.dialog.filename = "Enc_RapersGirl_dialog.c";
	sld.dialog.currentnode = "Node_207";
	LAi_SetActorTypeNoGroup(sld);
	GetCharacterPos(sld, &locx, &locy, &locz);
	ChangeCharacterAddressGroup(sld, pchar.location, "monsters", LAi_FindFarLocator("monsters", locx, locy, locz));
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

// LEO: игрок отказался вести девицу дальше. EncGirl_GirlFollow снимает LifeDay и включает
//   LAi_ActorFollowEverywhere, а ветки «close» метили только квест - отменять преследование было
//   некому. Штатная уборка (LandEnc_RapersBadExit) срабатывает ещё в джунглях, задолго до этого.
void EncGirl_Dismiss()
{
	ref sld;
	int idx = GetCharacterIndex("CangGirl");
	if (idx != -1)
	{
		sld = GetCharacter(idx);
		LAi_type_actor_Reset(sld); // снимает location.follower и переводит в ожидание
		sld.lifeDay = 0;           // вернуть авто-уборку: follow её удалял
	}
	pchar.quest.DeleteGirlGroup.win_condition.l1 = "ExitFromLocation";
	pchar.quest.DeleteGirlGroup.win_condition.l1.location = pchar.location;
	pchar.quest.DeleteGirlGroup.function = "EncGirl_DeleteGirlGroup";
}

void EncGirl_GirlFollow()
{
	sGlobalTemp = "Saved_CangGirl";
	ref sld = characterFromId("CangGirl");
	DeleteAttribute(sld, "LifeDay");
	GiveItem2Character(sld, "unarmed");
	EquipCharacterbyItem(sld, "unarmed");
	LAi_SetActorType(sld);
	LAi_group_Register("DeliveQuest_Girl");
	LAi_group_SetRelation("DeliveQuest_Girl", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
	LAi_group_MoveCharacter(sld, "DeliveQuest_Girl");
	LAi_ActorFollowEverywhere(sld, "", -1);

	if(pchar.GenQuest.EncGirl != "GetChest")
	{
		pchar.quest.EncGirl_DeathSimple.win_condition.l1 = "NPC_Death";
		pchar.quest.EncGirl_DeathSimple.win_condition.l1.character = "CangGirl";
		pchar.quest.EncGirl_DeathSimple.function = "EncGirl_DeathSimple";
	}
}

void EncGirl_MeetRapers(string qName)
{
	ref sld;
	int iRank;
	string model[25], sAreal, sCity;
	int iMassive;

	if (sti(pchar.rank) > 1)
	{
		if (sti(pchar.rank) > 20) iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*3*2.5/3);
		else iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*3*1.6/3);
	}
	else iRank = sti(pchar.rank);

	model[0] = "pirate_1";
	model[1] = "pirate_2";
	model[2] = "pirate_3";
	model[3] = "pirate_4";
	model[4] = "pirate_5";
	model[5] = "pirate_6";
	model[6] = "pirate_7";
	model[7] = "pirate_8";
	model[8] = "pirate_9";
	model[9] = "pirate_10";
	model[10] = "pirate_11";
	model[11] = "pirate_12";
	model[12] = "pirate_13";
	model[13] = "pirate_14";
	model[14] = "pirate_15";
	model[15] = "pirate_16";
	model[16] = "pirate_17";
	model[17] = "pirate_18";
	model[18] = "pirate_19";
	model[19] = "pirate_20";
	model[20] = "pirate_21";
	model[21] = "pirate_22";
	model[22] = "pirate_23";
	model[23] = "pirate_24";
	model[24] = "pirate_25";

	for(i = 1; i <= 3; i++)
	{
		iMassive = rand(24);
		sld = GetCharacter(NPC_GenerateCharacter("GangMan_" + i, model[iMassive], "man", "man", iRank, PIRATE, 1, true));
		SetFantomParamFromRank(sld, iRank, true);

		sAreal = GiveArealByLocation(sld.location);
		sCity = GetCityNameByIsland(sAreal);
		sld.city = sCity;
		LAi_SetImmortal(sld, false);
		LAi_SetActorType(sld);
		LAi_type_actor_Reset(sld);
		sld.dialog.filename = "Enc_Rapers_dialog.c";
		sld.dialog.currentnode = "Node_8";
		sld.greeting = "Enc_Raiders";
		sld.location = "none";
		PlaceCharacter(sld, "goto", "random_must_be_near");
		LAi_ActorFollow(sld, pchar, "", -1);
        LAi_group_MoveCharacter(sld, "EnemyFight");
	}
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER_OWN, LAI_GROUP_NEITRAL);

	LAi_group_ClearAllTargets();
	LAi_SetFightModeForOfficers(false);

    sld = characterFromID("GangMan_1");
    LAi_type_actor_Reset(sld);
    LAi_ActorDialog(sld, pchar, "", 4.0, 0);
	chrDisableReloadToLocation = true;
	DoQuestCheckDelay("OpenTheDoors", 5.0);
}

void EncGirl_GenerateLeatherBag(string qName)
{
	ref location;
	string boxId = "box1";
	aref boxItems;

	if(pchar.GenQuest.EncGirl == "Bag_BrothelRoomUp") // верхняя комната
	{
		location = &Locations[FindLocation(pchar.GenQuest.EncGirl.city + "_Brothel_room")];
	}
	if(pchar.GenQuest.EncGirl == "Bag_BrothelRoom")
	{
		location = &Locations[FindLocation(pchar.GenQuest.EncGirl.city + "_SecBrRoom")];
	}

	makearef(boxItems, location.(boxId).items);

	boxItems.leather_bag = 1;
	location.(boxId) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
	Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_26", "Logs.txt", "#space#", " "));
}

void EncGirl_RapersChest(string qName)
{
	ref location = &Locations[FindLocation(pchar.GenQuest.TreasureEncGirl)];
	string boxId = pchar.GenQuest.TreasureEncGirlBox;
	aref boxItems;

	makearef(boxItems, location.(boxId).items);

	boxItems.gold = GetSummonSkillFromName(pchar, SKILL_FORTUNE) * 100 + rand(3000);

	if(rand(3) == 1)
	{
		boxItems.jewelry2 = 10 + rand(3);
	}
	if(rand(1) == 1)
	{
		boxItems.jewelry1 = 15 + rand(10);
	}
	if(rand(1) == 1)
	{
		boxItems.jewelry12 = 25 + rand(15);
	}
	if(rand(1) == 1)
	{
		boxItems.jewelry11 = 18 + rand(20);
	}
	if(rand(1) == 1)
	{
		boxItems.jewelry7 = 22 + rand(12);
	}

	location.(boxId) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
	location.(boxId).Treasure = true; // Признак сокровища в сундуке
}

void EncGirl_TreasureLose(string qName)
{
	ref location = &Locations[FindLocation(pchar.GenQuest.TreasureEncGirl)];
	string boxId = pchar.GenQuest.TreasureEncGirlBox;

	pchar.quest.EncGirl_RapersTreasure.over = "yes";

	DeleteAttribute(location, boxId + ".items");
	DeleteAttribute(location, boxId + ".Treasure");

	DeleteAttribute(pchar, "GenQuest.EncGirl");
}

void EncGirl_RapersAfter(string qName)
{
	pchar.quest.EncGirl_TreasureLose.win_condition.l1 = "Timer";
	pchar.quest.EncGirl_TreasureLose.win_condition.l1.date.day = GetAddingDataDay(0, 0, 1);
	pchar.quest.EncGirl_TreasureLose.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
	pchar.quest.EncGirl_TreasureLose.win_condition.l1.date.year = GetAddingDataYear(0, 0, 1);
	pchar.quest.EncGirl_TreasureLose.function = "EncGirl_TreasureLose";

	pchar.quest.EncGirl_RapersTreasure.win_condition.l1 = "location";
	pchar.quest.EncGirl_RapersTreasure.win_condition.l1.location = pchar.GenQuest.TreasureEncGirl;
	pchar.quest.EncGirl_RapersTreasure.function = "EncGirl_RapersChest";
}

void EncGirl_SpeakInRoom(string qName)
{
	chrDisableReloadToLocation = true;
	LocatorReloadEnterDisable(locations[reload_location_index].fastreload + "_tavern", "reload2_back", true);
	DoQuestFunctionDelay("EncGirl_SpeakInRoomEnter", 2);
}

void EncGirl_SpeakInRoomEnter(string qName)
{
	LAi_SetFightMode(pchar, false);
	LAi_LockFightMode(pchar, true);
	ref sld = characterFromId("CangGirl");
	sld.dialog.filename = "Enc_RapersGirl_dialog.c";
	sld.dialog.currentnode = "Node_228";
	PlaceCharacter(sld, "goto", "random_must_be_near"));
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", 5.0, 0);
}

void EncGirl_SetBerglar(string qName)
{
	int iTemp = sti(pchar.rank) + rand(5);
	ref sld = GetCharacter(NPC_GenerateCharacter("Berglar_EncGirl", "officer_"+(rand(42)+1), "man", "man", iTemp, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, iTemp, 50 + rand(30), 50 + rand(30), "topor2", "pistol6", 50);
	sld.dialog.filename = "Enc_Rapers_dialog.c";
	sld.dialog.currentnode = "EncGirl_Berglar";
	sld.greeting = "Enc_Raiders";
	PlaceCharacter(sld, "goto", "random_must_be_near"));
	LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	LAi_ActorDialog(sld, pchar, "", 4.0, 0);
}

void EncGirlFack_outRoom()
{
	pchar.GenQuest.EncGirl = "EncGirlFack_fighted";
	AddCharacterExpToSkill(pchar, "Pistol", 100);
	sld = characterFromId("Berglar_EncGirl");
	sld.LifeDay = 0;
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", "reload1_back", "none", "", "", "OpenTheDoors", -1.0);
	SetFunctionTimerCondition("EncGirl_SpeakTavernKeeper", 0, 0, 1, false);
	LAi_LockFightMode(pchar, false);
}

void EncGirlFack_outRoomRaped()
{
	// ==> Забираем клинки, пистоли и деньги.
	RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
    while (FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE) != "")
    {
        TakeItemFromCharacter(pchar, FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE));
    }
    while (FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE) != "")
    {
        TakeItemFromCharacter(pchar, FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE));
    }
    pchar.money = 0;
    // <== Забираем клинки, пистоли и деньги.
	pchar.GenQuest.EncGirl = "EncGirlFack_fighted";
	AddCharacterExpToSkill(pchar, "Pistol", 100);
	sld = characterFromId("Berglar_EncGirl");
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", "reload1_back", "none", "", "", "OpenTheDoors", -1.0);
	SetFunctionTimerCondition("EncGirl_SpeakTavernKeeper", 0, 0, 1, false);
	LAi_LockFightMode(pchar, false);
}

void EncGirlFack_fight()
{
	LAi_LockFightMode(pchar, false);
	AddCharacterExpToSkill(pchar, "FencingLight", 30);
	AddCharacterExpToSkill(pchar, "Fencing", 30);
	AddCharacterExpToSkill(pchar, "FencingHeavy", 30);
	LAi_SetFightMode(pchar, true);
	pchar.GenQuest.EncGirl = "EncGirlFack_fighted";
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	pchar.quest.EncGirl_afterFight.win_condition.l1 = "NPC_Death";
	pchar.quest.EncGirl_afterFight.win_condition.l1.character = "Berglar_EncGirl";
	pchar.quest.EncGirl_afterFight.win_condition = "OpenTheDoors";
	SetFunctionTimerCondition("EncGirl_SpeakTavernKeeper", 0, 0, 1, false);
}

void EncGirl_SpeakTavernKeeper(string qName)
{
	DeleteAttribute(pchar, "GenQuest.EncGirl");
}

void EncGirl_GenQuest_GetBag(string qName)
{
	ref sld = CharacterFromID("CangGirl");
	sld.lifeDay = 0;
	AddSimpleRumour(RandPhraseSimple(GetConvertStrWithReplace("Variable_GenQuests_4", "AddRumours.txt", "#space#", " "),
				GetConvertStrWithReplace("Variable_GenQuests_5", "AddRumours.txt", "#space#", " ")),
				sti(pchar.GenQuest.EncGirl.nation), 3, 1);
	ChangeCharacterReputation(pchar, -8);
	pchar.quest.EncGirl_GenerateBag.over = "yes";
	pchar.quest.EncGirl_SpeakHorse.over = "yes";
	AddQuestRecord("JungleGirl", "22");
	CloseQuestHeader("JungleGirl");
	DeleteAttribute(pchar, "GenQuest.EncGirl");
}

void EncGirl_ExitTavernRoom(string qName)
{
	LocatorReloadEnterDisable(pchar.GenQuest.EncGirl.city + "_tavern", "reload2_back", false);

	pchar.quest.EncGirl_SpeakHorse.win_condition.l1 = "location";
	pchar.quest.EncGirl_SpeakHorse.win_condition.l1.location = pchar.GenQuest.EncGirl.city + "_tavern_upstairs";
	pchar.quest.EncGirl_SpeakHorse.function = "EncGirl_SpeakHorse";
	ref sld = characterFromId("CangGirl");
	ChangeCharacterAddress(sld, "none", "");
}

void EncGirl_SpeakHorse(string qName)
{
	ref sld = characterFromId("CangGirl");
	sld.dialog.filename = "Enc_RapersGirl_dialog.c";
	sld.dialog.currentnode = "Node_240_1";
	ChangeCharacterAddress(sld, pchar.location, "goto1");
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", 2.0, 0);
}

void EncGirl_DeliveToParents(string qName)
{
	ref sld;
	sld = characterFromId("CangGirl");
	RemovePassenger(pchar, sld);
	sld.dialog.filename = "Enc_RapersGirl_dialog.c";
	sld.dialog.currentnode = "Node_245";
	PlaceCharacter(sld, "goto", pchar.location);
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void EncGirl_GenerateLover(string qName)
{
	int iTemp = sti(pchar.rank) + rand(5);
	ref sld = GetCharacter(NPC_GenerateCharacter("Lover_EncGirl", "officer_"+(rand(42)+1), "man", "man", iTemp, sti(pchar.GenQuest.EncGirl.nation), -1, true));
	sld.name = pchar.GenQuest.EncGirl.sLoverId;
	sld.lastname = "";
	FantomMakeCoolFighter(sld, iTemp, 80, 80, "topor2", "pistol6", 50);
	pchar.quest.EncGirl_SpeakLoverTavern.win_condition.l1 = "location";
	pchar.quest.EncGirl_SpeakLoverTavern.win_condition.l1.location = pchar.GenQuest.EncGirl.city + "_tavern";
	pchar.quest.EncGirl_SpeakLoverTavern.function = "EncGirl_SpeakLover";
	pchar.GenQuest.EncGirl.SpeakLover = true;
}

void EncGirl_SpeakLover(string qName)
{
	// quest ended
	bool bOk = CheckAttribute(pchar, "GenQuest.EncGirl");
	// quest ended but another started with different outcome
	bool bOk2 = CheckAttribute(pchar, "GenQuest.EncGirl.SpeakLover") && pchar.GenQuest.EncGirl.SpeakLover == true;
	if (!bOk || !bOk2) {
		trace("Interesting outcome");
		return;
	}
	chrDisableReloadToLocation = true;
	ref sld = characterFromId("Lover_EncGirl");
	sld.dialog.filename = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "EncGirl_Lover";
	sld.greeting = "Gr_Officer";
	PlaceCharacter(sld, "goto", pchar.GenQuest.EncGirl.city + "_tavern");
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1.0, 0);
}

void EncGirl_LoverExit()
{
	string sTemp;
	pchar.quest.EncGirl_DeathSimple.over = "yes";
	AddQuestRecord("JungleGirl", "14");
	AddQuestUserData("JungleGirl", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("JungleGirl");
	ChangeCharacterReputation(pchar, -3);
	sTemp = LAi_FindNearestFreeLocator2Pchar("reload");

	for (i = 1; i <= 2; i++)
	{
		if (i == 1) sld = characterFromId("Lover_EncGirl");
		else sld = characterFromId("CangGirl");
		sld.lifeDay = 0;
		LAi_SetActorType(sld);
		LAi_ActorGoToLocation(sld, "reload", sTemp, "none", "", "", "OpenTheDoors", 5.0);
	}

	DeleteAttribute(pchar, "GenQuest.EncGirl");
}

void EncGirl_ToLoverParents()
{
	ref sld;
	string sTemp;
	pchar.quest.EncGirl_DeathSimple.over = "yes";
	AddQuestRecord("JungleGirl", "15");
	AddQuestUserData("JungleGirl", "sSex", GetSexPhrase("ся","ась"));
	AddQuestUserData("JungleGirl", "sCity", XI_ConvertString("Colony" + pchar.GenQuest.EncGirl.LoverCity));
	ChangeCharacterReputation(pchar, 1);
	sTemp = LAi_FindNearestFreeLocator2Pchar("reload");

	for (i = 1; i <= 2; i++)
	{
		if (i == 1) sld = characterFromId("Lover_EncGirl");
		else sld = characterFromId("CangGirl");
		AddPassenger(pchar, sld, false);
		SetCharacterRemovable(sld, false);
		LAi_SetActorType(sld);
		LAi_ActorGoToLocation(sld, "reload", sTemp, "none", "", "", "OpenTheDoors", 5.0);
	}

	pchar.quest.EncGirl_DeliveToLoverParents.win_condition.l1 = "location";
	pchar.quest.EncGirl_DeliveToLoverParents.win_condition.l1.location = pchar.GenQuest.EncGirl.LoverCity + "_town";
	pchar.quest.EncGirl_DeliveToLoverParents.function = "EncGirl_DeliveToLoverParents";
}

void EncGirl_ToLoverParentsExit()
{
	pchar.quest.EncGirl_DeathSimple.over = "yes";
	ChangeCharacterReputation(pchar, -3);
	AddMoneyToCharacter(pchar, 500 * (sti(pchar.rank) + 10) + drand(5000));
	AddQuestRecord("JungleGirl", "13");
	AddQuestUserData("JungleGirl", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("JungleGirl");
	ref sld = CharacterFromID("CangGirl");
	string sTemp = LAi_FindNearestFreeLocator2Pchar("reload");
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", sTemp, "none", "", "","", -1.0);
	sld.lifeDay = 0;
	DeleteAttribute(pchar, "GenQuest.EncGirl");
}

void EncGirl_DeliveToLoverParents(string qName)
{
	ref sld;
	sld = characterFromId("CangGirl");
	RemovePassenger(pchar, sld);
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", "reload1_back", "none", "", "", "", 5.0);
	sld.lifeday = 0;
	sld = characterFromId("Lover_EncGirl");
	if (pchar.location == "DesMoines_town" && CheckAttribute(pchar, "questTemp.MCzagadka"))	// Sinistra Иммунитет от квеста "Зачарованный город" (не блокирует двери)
	{
		LAi_LoginInCaptureTown(sld, true);
	}
	RemovePassenger(pchar, sld);
	chrDisableReloadToLocation = true;
	sld.dialog.filename = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "EncGirl_Lover6";
	PlaceCharacter(sld, "goto", pchar.location);
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void EncGirl_GenQuest_GetLoverFather(string qName)
{
	AddQuestRecord("JungleGirl", "17");
	AddQuestUserData("JungleGirl", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("JungleGirl");
	DeleteAttribute(pchar, "GenQuest.EncGirl");
}