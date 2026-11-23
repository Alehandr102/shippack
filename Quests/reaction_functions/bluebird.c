// Gregg: вынесено из reaction_functions.c (сплит god-файла) — секция «шебека Синяя Птица».
// #include на исходное место — видимость и поведение идентичны.

void BlueBird_inCavern(string qName)
{
	pchar.questTemp.BlueBird = "seenCarriers";
	DoQuestCheckDelay("TalkSelf_Quest", 0.2); //диалог сам-на-сам
}

void BlueBird_endCaveDialog()
{
	LAi_SetActorType(pchar);
	LAi_ActorTurnByLocator(pchar, "goto", "goto1");
	string sQuest = "";
	string CarrierName[10];
	CarrierName[0] = "GenresBag1";
	CarrierName[1] = "GenresBag2";
	CarrierName[2] = "GenresBarrel1";
	CarrierName[3] = "GenresBarrel2";
	CarrierName[4] = "GenresBarrelTop1";
	CarrierName[5] = "GenresBarrelTop2";
	CarrierName[6] = "GenresBottle1";
	CarrierName[7] = "GenresBottle2";
	CarrierName[8] = "GenresChest1";
	CarrierName[9] = "GenresChest2";
	for (i=0; i<10; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("Carrier_" + i, CarrierName[i], "man", "genres", 35, PIRATE, 0, true));
		sld.gotoGroup = "reload";
		sld.gotoLocator = "reload1_back";
		LAi_SetCarrierType(sld);
		ChangeCharacterAddressGroup(sld, "Bermudes_Cavern", "reload", "reload2");
	}
	DoQuestFunctionDelay("BlueBird_endCaveScript", 33.0);
}

void BlueBird_endCaveScript(string qName)
{
	LAi_LocationDisableMonstersGen("Bermudes_Cavern", false); //монстров генерить
	LAi_LocationDisableOfficersGen("Bermudes_Cavern", false); //офицеров пускать
	for (i=0; i<10; i++)
	{
		sld = characterFromId("Carrier_" + i);
		ChangeCharacterAddress(sld, "none", "");
	}
	AddQuestRecord("Xebeca_BlueBird", "6");
	AddQuestUserData("Xebeca_BlueBird", "sSex", GetSexPhrase("ел","ла"));
	LAi_SetPlayerType(pchar);
	pchar.questTemp.BlueBird = "toSeaBattle";
	pchar.quest.BlueBird_seaBattle.win_condition.l1 = "location";
	pchar.quest.BlueBird_seaBattle.win_condition.l1.location = "Bermudes";
	pchar.quest.BlueBird_seaBattle.function = "BlueBird_seaBattle";
}

void BlueBird_seaBattle(string qName)
{
	LAi_group_Delete("EnemyFight");
	group_DeleteGroup("BlueBird_Group");
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("BlueBirdCapitain", "PGG_DeLuck_0", "man", "man_fast", 20, PIRATE, 30, true));
	else sld = GetCharacter(NPC_GenerateCharacter("BlueBirdCapitain", "PGG_DeLuck_0", "man", "man", 20, PIRATE, 30, true));
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.7", "Names.txt");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_7", "Names.txt", "#space#", " ");
	sld.dialog.filename = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode = "BlueBirdCapitain";
	sld.greeting = "CapSinkShip";
	// Прописка всех моделей для кирас. -->
	sld.HeroModel = "PGG_DeLuck_0,PGG_DeLuck_1,PGG_DeLuck_2,PGG_DeLuck_3,PGG_DeLuck_4,PGG_DeLuck_5";
	// Прописка всех моделей для кирас. <--
	int iRank = sti(PChar.rank) + MOD_SKILL_ENEMY_RATE*3;
	FantomMakeCoolFighter(sld, iRank, 70, 70, "blade37", "pistol6", 10);
	FantomMakeCoolestSailor(sld, SHIP_XebekVML, GetConvertStr("Variable_reaction_functions_1", "ShipNames.txt"), CANNON_TYPE_CANNON_LBS20, 50, 50, 50);
	DeleteAttribute(sld, "SinkTenPercent");
	DeleteAttribute(sld, "AboardToFinalDeck");
	DeleteAttribute(sld, "SinkTenPercent");
	DeleteAttribute(sld, "DontRansackCaptain");
	sld.AlwaysSandbankManeuver = true;
	sld.SaveItemsForDead = true;
	sld.DontClearDead = true;
	sld.AnalizeShips = true;  //анализировать вражеские корабли при выборе таска
	sld.DontRansackCaptain = true;
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	// LEO: Убрал нахер перебафы
	SetCharacterPerk(sld, "MusketsShoot");
	LAi_group_MoveCharacter(sld, "EnemyFight");
	GiveItem2Character(sld, "indian5");
	GiveItem2Character(sld, "jewelry15");
	GiveItem2Character(sld, "icollection");
	GiveItem2Character(sld, "spyglass4");
	EquipCharacterbyItem(sld, "spyglass4");
	GiveItem2Character(sld, "talisman12");
	sld.ship.Crew.Morale = 100;
	ChangeCrewExp(sld, "Sailors", 100);
	ChangeCrewExp(sld, "Cannoners", 100);
	ChangeCrewExp(sld, "Soldiers", 70);

	Group_AddCharacter("BlueBird_Group", "BlueBirdCapitain");
	Group_SetType("BlueBird_Group", "pirate");
	Group_SetGroupCommander("BlueBird_Group", "BlueBirdCapitain");
	sTemp = "quest_ship_10";
	if (pchar.location.from_sea == "Shore_ship1") sTemp = "quest_ship_8";
	Group_SetAddress("BlueBird_Group", "Bermudes", "quest_ships", sTemp);
	Group_SetTaskAttack("BlueBird_Group", PLAYER_GROUP);

	pchar.quest.BlueBird_over.win_condition.l1 = "NPC_Death";
	pchar.quest.BlueBird_over.win_condition.l1.character = "BlueBirdCapitain";
	pchar.quest.BlueBird_over.function = "BlueBird_over";

	pchar.quest.BlueBird_DieHard1.win_condition.l1 = "MapEnter";
	pchar.quest.BlueBird_DieHard1.function = "BlueBird_DieHard";
}

void BlueBird_over(string qName)
{
	pchar.quest.BlueBird_DieHard1.over = "yes";
	// Older saves may also have a second MapEnter watcher.
	if (CheckAttribute(pchar, "quest.BlueBird_DieHard2")) pchar.quest.BlueBird_DieHard2.over = "yes";
	AddQuestRecord("Xebeca_BlueBird", "7");
	pchar.questTemp.BlueBird = "weWon";
	// Вуазье сбежал, ставим другого торгаша  <-- ugeen
	sld = characterFromId("Pirates_trader");
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_8", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_9", "Names.txt", "#space#", " ");
	sld.model = "trader_2";
	LAi_SetOwnerType(sld);
	// дадим слух
	AddSimpleRumourCity(GetConvertStrWithReplace("Variable_reaction_functions_1", "AddRumours.txt", "#space#", " ")+ GetSexPhrase(GetConvertStrWithReplace("Variable_reaction_functions_2", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_reaction_functions_3", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_reaction_functions_4", "AddRumours.txt", "#space#", " ")+
		GetConvertStrWithReplace("Variable_reaction_functions_5", "AddRumours.txt", "#space#", " "),"Pirates", 30, 3, "");
	// --> ugeen
}

void BlueBird_DieHard(string qName)
{
	sld = characterFromId("BlueBirdCapitain");
	sld.lifeDay = 0;
	group_DeleteGroup("BlueBird_Group");
	pchar.quest.BlueBird_over.over = "yes";
	pchar.quest.BlueBird_DieHard1.over = "yes";
	// Older saves can still have a second MapEnter watcher.
	if (CheckAttribute(pchar, "quest.BlueBird_DieHard2")) pchar.quest.BlueBird_DieHard2.over = "yes";
	AddQuestRecord("Xebeca_BlueBird", "8");
	pchar.questTemp.BlueBird = "DieHard";
}

//возмездие торгашам. нападение на личный флейт торговца
void BlueBird_loginFleut(string qName)
{
	if (GetQuestPastMinutesParam("questTemp.BlueBird") < 10)
	{
		LAi_group_Delete("EnemyFight");
		group_DeleteGroup("BlueBird_Group");
		sld = GetCharacter(NPC_GenerateCharacter("BlueBirdTrader", "", "man", "man", 20, sti(pchar.questTemp.BlueBird.nation), 30, true));
		sld.dialog.filename = "Quest\ForAll_dialog.c";
		sld.dialog.currentnode = "BlueBirdTrader";
		sld.greeting = "CapSinkShip";
		sld.AlwaysEnemy = true;
		SetCaptanModelByEncType(sld, "trade");
		FantomMakeCoolSailor(sld, SHIP_FLEUT, pchar.questTemp.BlueBird.Ship.Name, CANNON_TYPE_CULVERINE_LBS24, 70, 70, 70);
		sld.Ship.Mode = "trade"; //торговец
		sld.DontRansackCaptain = true;
		LAi_group_MoveCharacter(sld, "EnemyFight");
		Group_AddCharacter("BlueBird_Group", "BlueBirdTrader");
		Group_SetGroupCommander("BlueBird_Group", "BlueBirdTrader");
		Group_SetAddress("BlueBird_Group", pchar.questTemp.BlueBird.Island, "quest_ships", "quest_ship_"+(rand(5)+1));
		Group_SetTaskRunaway("BlueBird_Group", PLAYER_GROUP);
		pchar.questTemp.BlueBird = "attackFleut"; //флаг квеста
		//прерывания на уход из локации
		pchar.Quest.BlueBirdFleut_over1.win_condition.l1 = "Location_Type";
		pchar.Quest.BlueBirdFleut_over1.win_condition.l1.location_type = "town";
		pchar.Quest.BlueBirdFleut_over1.function = "BlueBirdFleut_over";
		pchar.Quest.BlueBirdFleut_over2.win_condition.l1 = "Location_Type";
		pchar.Quest.BlueBirdFleut_over2.win_condition.l1.location_type = "seashore";
		pchar.Quest.BlueBirdFleut_over2.function = "BlueBirdFleut_over";
		Pchar.quest.BlueBirdFleut_over3.win_condition.l1 = "MapEnter";
		pchar.Quest.BlueBirdFleut_over3.function = "BlueBirdFleut_over";
		//прерывание на абордаж
    	pchar.quest.BlueBirdFleut_board.win_condition.l1 = "NPC_death";
    	pchar.quest.BlueBirdFleut_board.win_condition.l1.character = "BlueBirdTrader";
    	pchar.quest.BlueBirdFleut_board.function = "BlueBirdFleut_board";
	}
	else
	{
		AddQuestRecord("Xebeca_BlueBird", "11");
		AddQuestUserData("Xebeca_BlueBird", "sSex", GetSexPhrase("ел","ла"));
		AddQuestUserData("Xebeca_BlueBird", "sIsland", XI_ConvertString(pchar.questTemp.BlueBird.Island + "Gen"));
		BlueBurd_RumourUpdate();
	}
}

void BlueBirdFleut_over(string qName)
{
	LAi_group_Delete("EnemyFight");
	group_DeleteGroup("BlueBird_Group");
	pchar.Quest.BlueBirdFleut_over1.over = "yes";
	pchar.Quest.BlueBirdFleut_over2.over = "yes";
	pchar.Quest.BlueBirdFleut_over3.over = "yes";
	pchar.Quest.BlueBirdFleut_board.over = "yes";
	AddQuestRecord("Xebeca_BlueBird", "12");
	AddQuestUserData("Xebeca_BlueBird", "sSex", GetSexPhrase("ел","ла"));
	AddQuestUserData("Xebeca_BlueBird", "sIsland", XI_ConvertString(pchar.questTemp.BlueBird.Island + "Gen"));
	BlueBurd_RumourUpdate();
}
//флейт абордирован, задание выполнено
void BlueBirdFleut_board(string qName)
{
	pchar.Quest.BlueBirdFleut_over1.over = "yes";
	pchar.Quest.BlueBirdFleut_over2.over = "yes";
	pchar.Quest.BlueBirdFleut_over3.over = "yes";
	int iFleits = sti(pchar.questTemp.BlueBird.count) + 1; //счётчик потопленных флейтов
	pchar.questTemp.BlueBird.count = iFleits;
	if (iFleits > 4) 
	{
		Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_1", "Logs.txt", "#space#", " ") + iFleits + GetConvertStrWithReplace("Variable_reaction_functions_2", "Logs.txt", "#space#", " "));
		AddQuestRecord("Xebeca_BlueBird", "13F");
	}
	else 
	{
		AddQuestRecord("Xebeca_BlueBird", "13");
		AddQuestUserData("Xebeca_BlueBird", "sSex", GetSexPhrase("ым","ой"));
		AddQuestUserData("Xebeca_BlueBird", "sIsland", XI_ConvertString(pchar.questTemp.BlueBird.Island + "Gen"));
	}
	BlueBurd_RumourUpdate();
}