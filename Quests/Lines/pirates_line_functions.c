// Gregg: вынесено из reaction_functions.c (сплит god-файла) — семейство «Пиратская линейка» (PiratesLine / PQ4-8 / PL).
// Файл ТЕКСТОВО включается в reaction_functions.c на исходное место (#include) — видимость и поведение идентичны.

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Пиратская линейка     начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PiratesLine_q1_tavernEnd()
{
	chrDisableReloadToLocation = false;
	bDisableFastReload = false;
	LAI_SetPlayerType(pchar);
    DoReloadCharacterToLocation("PuertoPrincipe_tavern", "tables", "stay3");
    sld = CharacterFromID("QuestPirate1");
    LAI_SetSitType(sld);
}

void PiratesLine_q1_arrest(string qName)
{
	chrDisableReloadToLocation = true;
	bDisableFastReload = true;
	GetCharacterPos(pchar, &locx, &locy, &locz);
    for (i=1; i<=3; i++)
    {
		sld = GetCharacter(NPC_GenerateCharacter("QuestSold_"+i, "sold_eng_"+i, "man", "man", 20, ENGLAND, 0, true));
		sld.City = "PortRoyal";
		sld.BreakTmplAndFightGroup = true; //реагировать на атаку
		sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
		sld.dialog.currentnode = "ArrestInPR";
		LAi_SetActorType(sld);
		LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
		ChangeCharacterAddressGroup(sld, pchar.location, "patrol", "patrol16");
		if (i == 1) LAi_ActorDialog(sld, pchar, "", -1, 0);
		else LAi_ActorFollow(sld, characterFromId("QuestSold_1"), "", -1);
	}
	sld = characterFromId("QuestPirate2");
	ChangeCharacterAddressGroup(sld, pchar.location, "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
	LAi_SetActorTypeNoGroup(sld);
	LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
	LAi_ActorFollow(sld, pchar, "", -1);
}

void PiratesLine_q1_toPrison()
{
	sld = characterFromId("QuestPirate2");
	sld.lifeDay = 0;
	ChangeCharacterAddress(sld, "none", "");
	RemovePassenger(pchar, sld);
	DoReloadCharacterToLocation("PortRoyal_prison",  "goto", "goto9");
	RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
	LAi_LocationFightDisable(&Locations[FindLocation("PortRoyal_prison")], true);
    DoQuestFunctionDelay("PiratesLine_q1_MorganInPrison", 10.0);
    sld = characterFromId("Henry Morgan");
    LAi_SetStayTypeNoGroup(sld);
    ChangeCharacterAddressGroup(sld, "PortRoyal_prison", "goto", "goto13");
	locations[FindLocation("PortRoyal_fort")].loginNpc = true;
}

void PiratesLine_q1_MorganInPrison(string qName)
{
    sld = characterFromID("Henry Morgan");
    LAi_SetActorTypeNoGroup(sld);
    LAi_ActorGoToLocator(sld, "goto", "goto23", "PiratesLine_q1_MorganGoTo", -1);
}

void PiratesLine_q1_MorganEnd()
{
	chrDisableReloadToLocation = false;
	bDisableFastReload = false;
	LocatorReloadEnterDisable("Portroyal_town", "houseS1", false); //откроем дом Моргана
	LAi_LocationFightDisable(&Locations[FindLocation("PortRoyal_prison")], false);
	pchar.questTemp.jailCanMove = true; //чтобы спокойно выйти из тюрьмы
	AddQuestRecord("Pir_Line_1_JohnBolton", "4");
	DoQuestReloadToLocation("PortRoyal_prison", "goto", "goto12", "");
    sld = characterFromId("Henry Morgan");
    LAi_SetHuberTypeNoGroup(sld);
    ChangeCharacterAddressGroup(sld, "PortRoyal_houseS1", "sit", "sit2");
}
//пиратка, квест №3, поиски Лоу
void PiratesLine_q3_LoyHouse(string qName)
{
	AddQuestRecord("Pir_Line_3_KillLoy", "2");
	AddQuestUserData("Pir_Line_3_KillLoy", "sSex", GetSexPhrase("ся","ась"));
	AddQuestUserData("Pir_Line_3_KillLoy", "sSex1", GetSexPhrase("","а"));
	pchar.questTemp.piratesLine = "KillLoy_toSeek";
}

void PiratesLine_q3_loginSeaWolf(string qName)
{
	if (GetCompanionQuantity(pchar) <= 1 && sti(RealShips[sti(pchar.ship.type)].Class) >= 5)
	{
		sld = characterFromId("EdwardLoy");
		sld.dialog.currentnode   = "SeaWolf";
		sld.greeting = "CapSinkShip";
		pchar.questTemp.piratesLine = "KillLoy_SeaWolfBattle";
		AddQuestRecord("Pir_Line_3_KillLoy", "15");
		AddQuestUserData("Pir_Line_3_KillLoy", "sSex", GetSexPhrase("ел","ла"));
		AddQuestUserData("Pir_Line_3_KillLoy", "sSex1", GetSexPhrase("","а"));
		Group_AddCharacter("SW_Group", "EdwardLoy");
		Group_SetType("SW_Group", "pirate");
		Group_SetGroupCommander("SW_Group", "EdwardLoy");
		sTemp = "quest_ship_" + (rand(4)+5);
		Group_SetAddress("SW_Group", "Cumana", "quest_ships", sTemp);
		Group_SetTaskAttack("SW_Group", PLAYER_GROUP);
		pchar.Quest.PiratesLine_q3_over1.win_condition.l1 = "Location_Type";
		pchar.Quest.PiratesLine_q3_over1.win_condition.l1.location_type = "town";
		pchar.Quest.PiratesLine_q3_over1.function = "PiratesLine_q3_over";
		pchar.Quest.PiratesLine_q3_over2.win_condition.l1 = "Location_Type";
		pchar.Quest.PiratesLine_q3_over2.win_condition.l1.location_type = "seashore";
		pchar.quest.PiratesLine_q3_over2.function = "PiratesLine_q3_over";
	}
	else
	{
		pchar.quest.PiratesLine_q3_SeaWolfAgain.win_condition.l1 = "ExitFromLocation";
		pchar.quest.PiratesLine_q3_SeaWolfAgain.win_condition.l1.location = "Cumana";
		pchar.quest.PiratesLine_q3_SeaWolfAgain.function = "PiratesLine_q3_SeaWolfAgain";
	}
}

void PiratesLine_q3_SeaWolfAgain(string qName)
{
	pchar.quest.PiratesLine_q3_loginSeaWolf.win_condition.l1 = "location";
	pchar.quest.PiratesLine_q3_loginSeaWolf.win_condition.l1.location = "Cumana";
	pchar.quest.PiratesLine_q3_loginSeaWolf.function = "PiratesLine_q3_loginSeaWolf";
}

void PiratesLine_q3_over(string qName)
{
	pchar.quest.PiratesLine_q3_over1.over = "yes";
	pchar.quest.PiratesLine_q3_over2.over = "yes";
	if (GetCharacterIndex("EdwardLoy") == -1)
	{
		QuestSetCurrentNode("Henry Morgan", "PL_Q3_GoodWork");
		if (sti(RealShips[sti(pchar.ship.type)].basetype) == SHIP_BRIGSW)
		{
			AddQuestRecord("Pir_Line_3_KillLoy", "17");
			pchar.questTemp.piratesLine.T1 = "KillLoy_GoodWork"; //выполнено все, бриг захвачен
		}
		else
		{
		    //#20171229-02 Check companion ships
		    bool bShipOK = false;
		    for(int i = 0; i < COMPANION_MAX; i++)
            {
                int cn = GetCompanionIndex(PChar, i);
                if(cn != -1)
                {
                    int iShipType = sti(characters[cn].ship.type);
                    if(iShipType != SHIP_NOTUSED)
                    {
                        if(sti(RealShips[iShipType].basetype) == SHIP_BRIGSW)
                        {
                            bShipOK = true;
                            break;
                        }
                    }
                }
            }
            //#20171231-01 Fix Low Quest
            if(bShipOK == true)
            {
                AddQuestRecord("Pir_Line_3_KillLoy", "17");
                pchar.questTemp.piratesLine.T1 = "KillLoy_GoodWork"; //Лоу убит, бриг не захвачен
                pchar.questTemp.KillLoy_GoodWork = true;
            }
            else //#20171229-02 Check companion ships
            {
                AddQuestRecord("Pir_Line_3_KillLoy", "18");
				pchar.questTemp.piratesLine.T1 = "KillLoy_LoyIsDied"; //Лоу убит, бриг не захвачен
                pchar.questTemp.KillLoy_LoyIsDied = true;
            }
		}
	}
	else
	{
		AddQuestRecord("Pir_Line_3_KillLoy", "16");
		AddQuestUserData("Pir_Line_3_KillLoy", "sSex", GetSexPhrase("","ла"));
		pchar.questTemp.piratesLine = "KillLoy_loose"; //Лоу остался жив, полный провал
		sld = characterFromId("EdwardLoy");
		sld.lifeDay = 0;
		QuestSetCurrentNode("Henry Morgan", "PL_Q3_notFound");
	}
	group_DeleteGroup("SW_Group");
}
//пиратка, квест №4, грабежи жемчужных тартан
void PQ4_SeaPearl_login(string qName)
{
	pchar.quest.PQ4_SeaPearl_Late.over = "yes";
	LAi_group_Delete("EnemyFight");
	group_DeleteGroup("PearlGroup");
	iTemp = sti(pchar.questTemp.Sharp.PearlAreal.qtyShips);
	Group_FindOrCreateGroup("PearlGroup");
	Group_SetType("PearlGroup", "trade");
	for (int j = 1; j <= iTemp; j++)
	{
		sTemp = "PearlCapitain_" + j;
		sld = GetCharacter(NPC_GenerateCharacter(sTemp, "trader_"+(rand(15)+1), "man", "man", 10, PIRATE, 30, true));
		sld.Ship.Type = GenerateShip(SHIP_LEUDO, false);
		SetRandomNameToShip(sld);
		SetBaseShipData(sld);
		SetCrewQuantityFull(sld);
		Fantom_SetBalls(sld, "pirate");
		sld.ShipTaskLock = true;
		sld.Abordage.Enable = false;
		sld.SinkTenPercent = true; //экономия в АИ шипов
		sld.PearlTartane = true; //тартана ловцов жемчуга, можно грабить
		LAi_group_MoveCharacter(sld, "EnemyFight");
		Group_AddCharacter("PearlGroup", sTemp);
	}
    // ==> стравливание
	Group_SetGroupCommander("PearlGroup", "PearlCapitain_1");
	Group_SetAddress("PearlGroup", pchar.questTemp.Sharp.PearlAreal.Island, "quest_ships", pchar.questTemp.Sharp.PearlAreal.locator);
	Group_SetTaskRunaway("PearlGroup", PLAYER_GROUP);
	Group_LockTask("PearlGroup");
	pchar.quest.PQ4_SeaPearl_DieHard1.win_condition.l1 = "ExitFromLocation";
	pchar.quest.PQ4_SeaPearl_DieHard1.win_condition.l1.location = pchar.questTemp.Sharp.PearlAreal.Island;
	pchar.quest.PQ4_SeaPearl_DieHard1.function = "PQ4_SeaPearl_DieHard";
	Pchar.quest.PQ4_SeaPearl_DieHard2.win_condition.l1 = "MapEnter";
	pchar.quest.PQ4_SeaPearl_DieHard2.function = "PQ4_SeaPearl_DieHard";
}

void PQ4_SeaPearl_Late(string qName)
{
	pchar.quest.PQ4_SeaPearl_login.over = "yes";
	AddQuestRecord("Pir_Line_4_Pearl", "2");
	AddQuestUserData("Pir_Line_4_Pearl", "sSex", GetSexPhrase("","а"));
	pchar.questTemp.piratesLine = "PearlQ4_late";
	sld = characterFromId("Henry Morgan");
    LAi_SetHuberTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, "PortRoyal_houseS1", "sit", "sit2");
	QuestSetCurrentNode("Henry Morgan", "PL_Q4_late");
}

void PQ4_SeaPearl_DieHard(string qName)
{
	group_DeleteGroup("PearlGroup");
	pchar.quest.PQ4_SeaPearl_DieHard1.over = "yes";
	pchar.quest.PQ4_SeaPearl_DieHard2.over = "yes";
	if (pchar.questTemp.Sharp == "SeaPearl_success")
	{	//успешное нападение
		AddQuestRecord("Pir_Line_4_Pearl", "3");
		AddQuestUserData("Pir_Line_4_Pearl", "iSmall", pchar.questTemp.Sharp.PearlAreal.SmallPearlQty);
		AddQuestUserData("Pir_Line_4_Pearl", "iBig", pchar.questTemp.Sharp.PearlAreal.BigPearlQty);
		pchar.questTemp.piratesLine = "PearlQ4_goodWork";
		QuestSetCurrentNode("Henry Morgan", "PL_Q4_goodWork");
		pchar.questTemp.piratesLine.Qty.SmallPearl = pchar.questTemp.Sharp.PearlAreal.SmallPearlQty;
		pchar.questTemp.piratesLine.Qty.BigPearl = pchar.questTemp.Sharp.PearlAreal.BigPearlQty;
	}
	else
	{	//неудачное нападение
		AddQuestRecord("Pir_Line_4_Pearl", "4");
		AddQuestUserData("Pir_Line_4_Pearl", "sSex", GetSexPhrase("","ла"));
		pchar.questTemp.piratesLine = "PearlQ4_badWork";
		QuestSetCurrentNode("Henry Morgan", "PL_Q4_badWork");
	}
	pchar.questTemp.Sharp = "seekSharp"; //опять начинаем с борделей
	pchar.questTemp.Sharp.brothelChance = rand(10);
	DeleteAttribute(pchar, "questTemp.Sharp.PearlAreal");
	CloseQuestHeader("SharpPearl");
}
//пиратка, квест №5, работа ОЗГа
void PQ5_meetInPortRoyal(string qName)
{
	chrDisableReloadToLocation = true;
	sld = GetCharacter(NPC_GenerateCharacter("CapGoodly", "reservist_7", "man", "man", 20, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_13", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_14", "Names.txt", "#space#", " ");
	sld.city = "PortRoyal";
	sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
	sld.dialog.currentnode   = "PQ5CapGoodly";
	sld.greeting = "Gr_EvilPirate";
    ChangeCharacterAddressGroup(sld, "PortRoyal_town", "patrol", "patrol16");
	LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	LAi_ActorDialog(sld, pchar, "", 2.0, 0);
}

void PQ5_inOrryHouse(string qName)
{
	pchar.questTemp.piratesLine = "PL5Hunter_exitReload";
	LAi_LocationFightDisable(&locations[FindLocation("Pirates_houseS1")], true); //закроем боевки
	chrDisableReloadToLocation = true;
	LAi_group_Delete("EnemyFight");

	sld = GetCharacter(NPC_GenerateCharacter("Aivory", "officer_7", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 100, 100, "blade28", "pistol3", 80);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_15", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_16", "Names.txt", "#space#", " ");
	sld.city = "Pirates";
	sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
	sld.dialog.currentnode   = "PQ5Hero";
	sld.greeting = "Gr_MiddPirate";
	LAi_SetStayType(sld);
    ChangeCharacterAddressGroup(sld, "Pirates_houseS1", "goto", "goto1");

	sld = GetCharacter(NPC_GenerateCharacter("Orry", "officer_15", "man", "man", 27, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 27, 100, 100, "topor2", "pistol3", 50);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_17", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_18", "Names.txt", "#space#", " ");
	sld.city = "Pirates";
	sld.money = 51600;
	sld.SaveItemsForDead   = true; // сохранять на трупе вещи
	sld.DontClearDead = true;  // не убирать труп через 200с
	sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
	sld.dialog.currentnode   = "PQ5Orry";
	sld.greeting = "pirat_quest";
	LAi_SetStayType(sld);
    ChangeCharacterAddressGroup(sld, "Pirates_houseS1", "goto", "goto2");

	pchar.quest.PQ5_afterFight.win_condition.l1 = "NPC_Death";
	pchar.quest.PQ5_afterFight.win_condition.l1.character = "Aivory";
	pchar.quest.PQ5_afterFight.win_condition.l2 = "NPC_Death";
	pchar.quest.PQ5_afterFight.win_condition.l2.character = "Orry";
	pchar.quest.PQ5_afterFight.function = "PQ5_afterFight";
}

void PQ5_afterChangeMainHero()
{
	LAi_LocationFightDisable(&locations[FindLocation("Pirates_houseS1")], false);
    SetMainCharacterIndex(1);
    PChar = GetMainCharacter();
    LAi_SetPlayerType(PChar);
	locCameraTarget(PChar)
    locCameraFollow();
	rCharacter = characterFromID("Orry");
	LAi_SetActorType(rCharacter);
	LAi_ActorTurnToCharacter(rCharacter, pchar);
	sld = characterFromID("Aivory");
	sld.dialog.currentnode = "PQ5Aivory";
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
	EndQuestMovie();
}

void PQ5_afterChangeMainHero2()
{
	rCharacter = characterFromID("Orry");
	LAi_group_MoveCharacter(rCharacter, "EnemyFight");
	sld = characterFromID("Aivory");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	LAi_SetFightMode(pchar, true);
}

void PQ5_afterFight(string qName)
{
	AddQuestRecord("Pir_Line_5_Hunter", "11");
	AddQuestUserData("Pir_Line_5_Hunter", "sCity", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_4 + "Gen"));
	pchar.questTemp.piratesLine = "PL5Hunter_toMorgan";
	chrDisableReloadToLocation = false;
	pchar.questTemp.piratesLine.PLQ5Money = true;
	Pchar.quest.PQ5_inPortPoyalTavern.win_condition.l1 = "location";
	Pchar.quest.PQ5_inPortPoyalTavern.win_condition.l1.location = "PortRoyal_tavern";
	Pchar.quest.PQ5_inPortPoyalTavern.function = "PQ5_inPortPoyalTavern";
}

void PQ5_inPortPoyalTavern(string qName)
{
	FreeSitLocator("PortRoyal_tavern", "sit3");
	sld = characterFromID("CapGoodly");
	sld.dialog.currentnode = "PQ5CapGoodlyEnd";
	LAi_SetSitTypeNoGroup(sld);
    ChangeCharacterAddressGroup(sld, "PortRoyal_tavern", "sit", "sit3");
}

void PQ5_Morgan_1()
{
	LAi_SetActorType(pchar);
	LAi_ActorGoToLocator(pchar, "goto", "goto2", "PQ5_Morgan_2", -1.0);
}

void PQ5_Morgan_4()
{
    SetMainCharacterIndex(1);
    PChar = GetMainCharacter();
    LAi_SetPlayerType(PChar);
	locCameraTarget(PChar)
    locCameraFollow();
	rCharacter = characterFromID("CapGoodly");
	LAi_SetActorType(rCharacter);
	LAi_ActorTurnToCharacter(rCharacter, pchar);
	sld = characterFromID("Henry Morgan");
	LAi_SetActorType(pchar);
	LAi_ActorDialog(pchar, sld, "", 0, 0);
}

void PQ5_Morgan_5()
{
	LAi_group_Delete("EnemyFight");
	LAi_SetPlayerType(pchar);
	sld = characterFromID("CapGoodly");
	LAi_group_MoveCharacter(sld, "EnemyFight");
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	pchar.quest.PQ5_Morgan_afterFight.win_condition.l1 = "NPC_Death";
	pchar.quest.PQ5_Morgan_afterFight.win_condition.l1.character = "CapGoodly";
	pchar.quest.PQ5_Morgan_afterFight.function = "PQ5_Morgan_afterFight";
	LAi_SetFightMode(pchar, true);
}

void PQ5_Morgan_afterFight(string qName)
{
	QuestSetCurrentNode("Henry Morgan", "PL_Q5_afterFight");
	AddQuestRecord("Pir_Line_5_Hunter", "13");
	AddQuestUserData("Pir_Line_5_Hunter", "sSex", GetSexPhrase("","а"));
}
//пиратка, квест №6, двойник
void PQ6_ShoreBattle(string qName)
{
    LAi_group_Delete("EnemyFight");
	LocatorReloadEnterDisable("Shore7", "boat", true); //закроем выход в море
	GetCharacterPos(pchar, &locx, &locy, &locz);
	//наши
	string sTemp = NationShortName(sti(pchar.nation));
    for (i=1; i<=5; i++)
    {
        iTemp = 10 + rand(10);
		sld = GetCharacter(NPC_GenerateCharacter("OwnPirate_"+i, "pirate_"+(rand(24)+1), "man", "man", iTemp, sti(pchar.nation), 0, true));
        LAi_SetWarriorType(sld);
		LAi_warrior_DialogEnable(sld, false);
        LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
        ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto8");
    }
	//враги
	for (i=1; i<=(MOD_SKILL_ENEMY_RATE + 3); i++)
    {
        iTemp = 15 + rand(10);
		sld = GetCharacter(NPC_GenerateCharacter("QuestPirate_"+i, "officer_"+(rand(42)+1), "man", "man", iTemp, PIRATE, -1, true));
        FantomMakeCoolFighter(sld, iTemp, 80, 80, BLADE_LONG, "pistol3", 50);
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        PlaceCharacter(sld, "goto", PChar.location);
    }
    LAi_group_SetLookRadius("EnemyFight", 100);
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
    Pchar.quest.PQ6_JungleDialog.win_condition.l1 = "location";
    Pchar.quest.PQ6_JungleDialog.win_condition.l1.location = "Common_jungle_01";
    Pchar.quest.PQ6_JungleDialog.function = "PQ6_JungleDialog";
}

void PQ6_JungleDialog(string qName)
{
	LAi_LocationFightDisable(&Locations[FindLocation("Common_jungle_01")], true);
	chrDisableReloadToLocation = true;
	string sModel = pchar.model;
	sld = GetCharacter(NPC_GenerateCharacter("QuestPirateHead", sModel, "man", pchar.model.animation, 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 90, 90, "blade27", "pistol6", 100);
	sld.name = pchar.name;
	sld.lastname = pchar.lastname; // LEO: двойник ГГ - фамилия как есть, без подмены на имя
    sld.dialog.Filename = "Quest\PiratesLine_dialog.c";
	sld.dialog.currentnode = "QuestPirateHead";
	sld.SaveItemsForDead = true;
	TakeNItems(sld, "talisman17", 1);
	ChangeCharacterAddressGroup(sld, "Common_jungle_01", "goto", "goto1");
    LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	LAi_ActorDialog(sld, pchar, "", 2.0, 0);
}

void PQ6_JungleBattle()
{
	LAi_LocationFightDisable(loadedLocation, false);
	LAi_SetFightMode(pchar, true);
	//наши
	string sTemp = NationShortName(sti(pchar.nation));
    for (i=1; i<=5; i++)
    {
        iTemp = 10 + rand(10);
		sld = GetCharacter(NPC_GenerateCharacter("OwnPirate2_"+i, "pirate_"+(rand(24)+1), "man", "man", iTemp, sti(pchar.nation), 0, true));
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
        ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
    }
	//враги
	for (i=1; i<=(MOD_SKILL_ENEMY_RATE + 3); i++)
    {
        iTemp = 17 + rand(10);
		sld = GetCharacter(NPC_GenerateCharacter("QuestPirate2_"+i, "officer_"+(rand(42)+1), "man", "man", iTemp, PIRATE, -1, true));
        FantomMakeCoolFighter(sld, iTemp, 90, 90, "topor2", "pistol6", 100);
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, "Common_jungle_01", "goto", "goto2");
    }
    LAi_group_SetLookRadius("EnemyFight", 100);
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	LAi_group_SetCheck("EnemyFight", "PQ6_afterBattle");
}
//пиратка, квест №7, мочим Соукинса
void PQ7_FriendLinney(string qName)
{
	if (GetQuestPastMinutesParam("questTemp.piratesLine") < 20)
	{
		sld = GetCharacter(NPC_GenerateCharacter("FriendLinney", "", "man", "man", 20, sti(pchar.nation), 5, true));
		SetCaptanModelByEncType(sld, "pirate");
		sld.dialog.filename = "Quest\PiratesLine_dialog.c";
		sld.DeckDialogNode = "FriendLinney";
		FantomMakeCoolFighter(sld, 20, 80, 70, "blade32", "pistol3", 20);
		FantomMakeCoolSailor(sld, SHIP_LUGGER, "", CANNON_TYPE_CULVERINE_LBS16, 70, 70, 70);
		sld.AlwaysSandbankManeuver = true;
		sld.DontRansackCaptain = true;
		sld.Ship.Mode = "Pirate";
		DeleteAttribute(sld, "SinkTenPercent");
		DeleteAttribute(sld, "SaveItemsForDead");
		DeleteAttribute(sld, "DontClearDead");
		DeleteAttribute(sld, "AboardToFinalDeck");
		DeleteAttribute(sld, "SinkTenPercent");
		DeleteAttribute(sld, "DontRansackCaptain");
		sld.AnalizeShips = true;  //анализировать вражеские корабли при выборе таска
		Group_AddCharacter("FriendLinney_Group", "FriendLinney");
		Group_SetType("FriendLinney_Group", "pirate");
		Group_SetGroupCommander("FriendLinney_Group", "FriendLinney");
		Group_SetAddress("FriendLinney_Group", "Hispaniola1", "quest_ships", "quest_ship_4");
		Group_SetTaskNone("FriendLinney_Group");
	}
	else
	{
		AddQuestRecord("Pir_Line_7_Soukins", "3");
		AddQuestUserData("Pir_Line_7_Soukins", "sSex", GetSexPhrase("","а"));
		pchar.questTemp.piratesLine = "Soukins_LaVegaLate";
		QuestSetCurrentNode("Henry Morgan", "PL_Q7_LaVegaLate");
	}
}

void PQ7_LoginLeon()
{
	LAi_group_Delete("EnemyFight");
	group_DeleteGroup("FriendLinney_Group"); //сносим друга Стива Линнея
	sld = GetCharacter(NPC_GenerateCharacter("LeonCapitain", "", "man", "man", 25, PIRATE, 30, true));
	SetCaptanModelByEncType(sld, "pirate");
	sld.dialog.filename = "Quest\PiratesLine_dialog.c";
	sld.dialog.currentnode = "LeonCapitain";
	sld.greeting = "CapSinkShip";
	FantomMakeCoolFighter(sld, 25, 80, 80, "blade32", "pistol6", 50);
	FantomMakeCoolSailor(sld, SHIP_FRIGATE, GetConvertStr("Variable_reaction_functions_2", "ShipNames.txt"), CANNON_TYPE_CULVERINE_LBS24, 80, 80, 80);
	DeleteAttribute(sld, "SinkTenPercent");
	DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");
	DeleteAttribute(sld, "AboardToFinalDeck");
	DeleteAttribute(sld, "SinkTenPercent");
	DeleteAttribute(sld, "DontRansackCaptain");
	sld.Ship.Mode = "Pirate";
	sld.AlwaysSandbankManeuver = true;
	sld.AnalizeShips = true;  //анализировать вражеские корабли при выборе таска
	sld.DontRansackCaptain = true;
	SetCharacterPerk(sld, "FastReload");
	SetCharacterPerk(sld, "HullDamageUp");
	SetCharacterPerk(sld, "SailsDamageUp");
	SetCharacterPerk(sld, "CrewDamageUp");
	SetCharacterPerk(sld, "CriticalShoot");
	SetCharacterPerk(sld, "LongRangeShoot");
	SetCharacterPerk(sld, "CannonProfessional");
	SetCharacterPerk(sld, "ShipDefenseProfessional");
	SetCharacterPerk(sld, "SwordplayProfessional");
	SetCharacterPerk(sld, "AdvancedDefense");
	SetCharacterPerk(sld, "CriticalHit");
	SetCharacterPerk(sld, "Sliding");
	LAi_group_MoveCharacter(sld, "EnemyFight");

	string sGroup = "Leon_Group";
	Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup,"trade");
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Group_AddCharacter(sGroup, sld.id);
	Group_SetGroupCommander(sGroup, sld.id);
	SetRandGeraldSail(sld, sti(sld.Nation));

	SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
	sld.mapEnc.type = "warrior";
	sld.mapEnc.worldMapShip = "quest_ship";
	sld.mapEnc.Name = GetConvertStrWithReplace("Variable_reaction_functions_19", "Names.txt", "#space#", " ");
	Map_CreateWarrior("shore33", "LeonCapitain", 30);//указание стартового локатора не работает. для военных сейчас нет указания стартовой точки в коде
	Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_3", "Logs.txt", "#space#", " "));
}

void PQ7_inSoukinsResidence(string qName)
{
	QuestSetCurrentNode("Henry Morgan", "PL_Q7_BadWork"); //нода, если ничего не сделал
	if (IsLoginTime())
	{
		DoQuestFunctionDelay("PQ7_inSoukinsResidence_2", 1.5);
	}
}

void PQ7_inSoukinsResidence_2(string qName)
{
	LAi_SetActorType(pchar);
	sld = characterFromId("Richard_Soukins");
	QuestSetCurrentNode("Richard_Soukins", "PL_Q7_5");
	LAi_ActorDialog(pchar, sld, "", 0, 0);
}

void PQ7_loadToRecidence(string qName)
{
	DoQuestReloadToLocation("PuertoPrincipe_townhall", "reload", "reload2", "");
}

void PQ7_setBattleShip(string qName)
{
	sld = &locations[FindLocation("PuertoPrincipe_townhall")];
	DeleteAttribute(sld, "reload.l2"); //уберем возможность спуститься с лестницы
	//#20180903-01 Change from 100 minutes to 24 hours
	if (GetQuestPastMinutesParam("questTemp.piratesLine") > 100)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_4", "Logs.txt", "#space#", " "));
		pchar.questTemp.piratesLine = "Soukins_battlshipLate";
		AddQuestRecord("Pir_Line_7_Soukins", "10");
		AddQuestUserData("Pir_Line_7_Soukins", "sSex", GetSexPhrase("","а"));
	}
	else
	{
		sld = GetCharacter(NPC_GenerateCharacter("QuestCap_PL7", "SpaOfficer1", "man", "man", 40, SPAIN, -1, true));
		sld.dialog.filename = "Quest\PiratesLine_dialog.c";
		sld.dialog.currentnode = "QuestCap_PL7";
		sld.DeckDialogNode = "QuestCap_PL7_inDeck";
		sld.greeting = "CapSinkShip";
		FantomMakeCoolFighter(sld, 40, 90, 90, "blade32", "pistol4", 200);
		FantomMakeCoolSailor(sld, SHIP_REDOUTABLE, GetConvertStr("Variable_reaction_functions_3", "ShipNames.txt"), CANNON_TYPE_CULVERINE_LBS32, 90, 90, 90);
		DeleteAttribute(sld, "SinkTenPercent");
		DeleteAttribute(sld, "SaveItemsForDead");
		DeleteAttribute(sld, "DontClearDead");
		DeleteAttribute(sld, "AboardToFinalDeck");
		DeleteAttribute(sld, "SinkTenPercent");
		DeleteAttribute(sld, "DontRansackCaptain");
		sld.AlwaysSandbankManeuver = true;
		sld.AnalizeShips = true;  //анализировать вражеские корабли при выборе таска
		sld.DontRansackCaptain = true;
		SetCharacterPerk(sld, "FastReload");
		SetCharacterPerk(sld, "HullDamageUp");
		SetCharacterPerk(sld, "SailsDamageUp");
		SetCharacterPerk(sld, "CrewDamageUp");
		SetCharacterPerk(sld, "CriticalShoot");
		SetCharacterPerk(sld, "LongRangeShoot");
		SetCharacterPerk(sld, "CannonProfessional");
		SetCharacterPerk(sld, "ShipDefenseProfessional");
		SetCharacterPerk(sld, "ShipTurnRateUp");
		SetCharacterPerk(sld, "ShipTurnRateUp");
		SetCharacterPerk(sld, "StormProfessional");
		SetCharacterPerk(sld, "SwordplayProfessional");
		SetCharacterPerk(sld, "AdvancedDefense");
		SetCharacterPerk(sld, "CriticalHit");
		SetCharacterPerk(sld, "Sliding");
		SetCharacterPerk(sld, "Tireless");
		SetCharacterPerk(sld, "HardHitter");
		SetCharacterPerk(sld, "GunProfessional");

		string sGroup = "Quest_Ship";
		Group_FindOrCreateGroup(sGroup);
		Group_SetType(sGroup,"trade");
		Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
		Group_LockTask(sGroup);
		Group_AddCharacter(sGroup, sld.id);
		Group_SetGroupCommander(sGroup, sld.id);
		SetRandGeraldSail(sld, sti(sld.Nation));

		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
		sld.mapEnc.type = "trade";
		sld.mapEnc.worldMapShip = "quest_ship";
		sld.mapEnc.Name = GetConvertStrWithReplace("Variable_reaction_functions_20", "Names.txt", "#space#", " ");
		Map_CreateTraderXZ(1009.22, -992.708, 988.514, 515.407, "QuestCap_PL7", 15);
		Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_5", "Logs.txt", "#space#", " "));
	}
}

void PQ7_winBattleShip(string qName)
{
	pchar.questTemp.piratesLine = "Soukins_battlshipWin";
	QuestSetCurrentNode("Henry Morgan", "PL_Q7_GoodWork");
	AddQuestRecord("Pir_Line_7_Soukins", "13");
	pchar.quest.PQ7_BattleShip_dieHard.over = "yes";
}

void PQ8_timeOver(string qName) //убираем моргана
{
	pchar.questTemp.piratesLine = "over";
	sld = characterFromId("Henry Morgan");
	ChangeCharacterAddress(sld, "none", "");
	AddQuestRecord("Pir_Line_8_Panama", "2");
	AddQuestUserData("Pir_Line_8_Panama", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("Pir_Line_8_Panama");
	SetTimerFunction("PQ8_openPanama", 0, 0, 40);
	QuestSetCurrentNode("Oliver_Green", "PL_Q8_over"); //ноду секретарю
}

void PQ8_openPanama(string qName) //откроем Панаму
{
	LocatorReloadEnterDisable("Shore48", "reload1_back", false);
	LocatorReloadEnterDisable("PortoBello_Jungle_01", "reload1_back", false);
	sld = characterFromId("Henry Morgan");
	sld.dialog.currentnode = "First time";
    ChangeCharacterAddressGroup(sld, "PortRoyal_houseS1", "sit", "sit2");
    LAi_SetHuberType(sld);
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
}

void PQ8_openPanama_2(string qName) //откроем Панаму
{
	sld = characterFromId("Henry Morgan");
	if (pchar.questTemp.piratesLine == "Panama_backToShip")
	{
		SetTimerFunction("PQ8_openPanama_3", 0, 0, 60); // Warship 18.07.09 fix - было 180 почему-то
	}
	else
	{
		sld.dialog.currentnode = "I_know_you_good";
		ChangeCharacterAddressGroup(sld, "PortRoyal_houseS1", "sit", "sit2");
		LAi_SetImmortal(sld, true);
		LAi_LoginInCaptureTown(sld, false);
		LAi_SetHuberType(sld);
		LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	}
	//возвращаем Джекмена
	iTemp = GetCharacterIndex("Jackman");
	if (iTemp != -1)
	{
		if(IsCompanion(&characters[iTemp]))
		{
			RemoveCharacterCompanion(pchar, &characters[iTemp]);
		}
		LAi_SetHuberType(&characters[iTemp]);
		LAi_group_MoveCharacter(&characters[iTemp], "PIRATE_CITIZENS");
        ChangeCharacterAddressGroup(&characters[iTemp], "Pirates_townhall", "sit", "sit1");
        characters[iTemp].dialog.currentnode = "I_know_you_good";
    }
    //Возвращаем на место Соукинса
	iTemp = GetCharacterIndex("Richard_Soukins");
	if (iTemp != -1)
	{
		if(IsCompanion(&characters[iTemp]))
		{
			RemoveCharacterCompanion(pchar, &characters[iTemp]);
		}
		LAi_SetHuberType(&characters[iTemp]);
		LAi_group_MoveCharacter(&characters[iTemp], "PIRATE_CITIZENS");
        ChangeCharacterAddressGroup(&characters[iTemp], "PuertoPrincipe_townhall", "sit", "sit1");
        characters[iTemp].dialog.currentnode = "First time";
    }
	LocatorReloadEnterDisable("Panama_ExitTown", "reload4", false);
	SetLocationCapturedState("Panama_town", false);
	bDisableLandEncounters = false;
	pchar.quest.PL_Q8_backFight_1.over = "yes"; //на всякий случай
}

void PQ8_openPanama_3(string qName) //Морган после поездки в Лондон
{
	sld = characterFromId("Henry Morgan");
	sld.dialog.currentnode = "PL_Q8_afterPanama";
	ChangeCharacterAddressGroup(sld, "PortRoyal_houseS1", "sit", "sit2");
	LAi_SetImmortal(sld, true);
	LAi_LoginInCaptureTown(sld, false);
	LAi_SetHuberType(sld);
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
}

void PQ8_PanamaTimerOver(string qName)
{
    if (IsEntity(worldMap))
    {
        if (!bQuestCheckProcessFreeze) // можно показать
		{
	        pchar.quest.PQ8_PanamaTimerOver_01.over = "yes"; // сброс, если уже откладывали
	        pchar.quest.PQ8_PanamaTimerOver_02.over = "yes";
			pchar.quest.PQ8_EnterPortoBello.over = "yes"; //снимаем прерывание на вход

            aref arOldMapPos;
			makearef(arOldMapPos, worldMap.old);
            WdmPrepareMapForAbordage(arOldMapPos);
            MakeCloneShipDeck(pchar, true); // подмена палубы
            DoReloadFromWorldMapToLocation("Ship_deck", "goto", "goto5");
	        ReloadFromWMtoL_complete();

            sld = characterFromID("Henry Morgan");
            ChangeCharacterAddressGroup(sld, "Ship_deck", "goto", "goto6");
            LAi_SetWarriorType(sld);
	        LAi_warrior_DialogEnable(sld, true);
	        sld.dialog.currentnode = "PL_Q8_timeOver";
   	        // ==> антураж
            for (i=1; i<5; i++)
        	{
                sTemp = "pirate_" + i;
                sld = GetCharacter(NPC_GenerateCharacter(sTemp, sTemp, "man", "man", 15, PIRATE, 1, true));
                sld.dialog.Filename = "Quest\sailors_dialog.c";
	            sld.dialog.currentnode = "Morgan_wait_you";
                LAi_SetWarriorType(sld);
                LAi_warrior_DialogEnable(sld, true);
                LAi_SetLoginTime(sld, 0.0, 24.0);
                LAi_SetHP(sld, 100.0, 100.0);
                ChangeCharacterAddressGroup(sld, "Ship_deck", "goto", "goto"+i);
            }
            pchar.quest.Munity = "";  // признак выхода с палубы (закрыто, а то можно не говорить)
            LAi_LockFightMode(pchar, true);
		}
		else
		{   // отложим до разморозки (пролет табличек)
      		Pchar.quest.PQ8_PanamaTimerOver_01.win_condition.l1 = "MapEnter";
            Pchar.quest.PQ8_PanamaTimerOver_01.function = "PQ8_PanamaTimerOver";
		}
	}
	else
	{   // подождем выхода на карту, раз время прошло, но мы не на ней
		Pchar.quest.PQ8_PanamaTimerOver_02.win_condition.l1 = "MapEnter";
        Pchar.quest.PQ8_PanamaTimerOver_02.function = "PQ8_PanamaTimerOver";
	}
}

void PQ8_EnterPortoBello()
{
	sld = characterFromId("Henry Morgan");
	LAi_SetImmortal(sld, false);
	pchar.quest.PQ8_PanamaTimerOver.over = "yes"; //снимаем таймер
}

void PQ8_MorganInPortoBello()
{
	chrDisableReloadToLocation = true;
	DoQuestFunctionDelay("PQ8_MorganInPortoBello_2", 3.0);
}

void PQ8_MorganInPortoBello_2(string qName)
{
	sld = characterFromId("Henry Morgan");
	LAi_LoginInCaptureTown(sld, true);
	LAi_SetCurHPMax(sld);
	sld.dialog.currentNode = "PL_Q8_PBResidence";
	ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
    LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void PQ8_PBExitResidence(string qName)
{
	iTemp = GetCharacterIndex("Henry Morgan");
	if (iTemp != -1)
	{
		if(IsCompanion(&characters[iTemp]))
		{
			RemoveCharacterCompanion(pchar, &characters[iTemp]);
		}
		ChangeCharacterAddress(&characters[iTemp], "none", "");
	}
	iTemp = GetCharacterIndex("Jackman");
	if (iTemp != -1)
	{
		if(IsCompanion(&characters[iTemp]))
		{
			RemoveCharacterCompanion(pchar, &characters[iTemp]);
		}
		ChangeCharacterAddress(&characters[iTemp], "none", "");
	}
	iTemp = GetCharacterIndex("John Morris");
	if (iTemp != -1)
	{
		if(IsCompanion(&characters[iTemp]))
		{
			RemoveCharacterCompanion(pchar, &characters[iTemp]);
		}
		ChangeCharacterAddress(&characters[iTemp], "none", "");
	}
	sld = characterFromId("Richard_Soukins");
	sld.DeckDialogNode = "PL_Q8_deck_2";
	AddQuestRecord("Pir_Line_8_Panama", "5");
	pchar.Quest.PQ8_fromPBport.win_condition.l1 = "EnterToSea";
	pchar.Quest.PQ8_fromPBport.function = "PQ8_fromPBport";
	SetTimerFunction("PQ8_controlShore48", 0, 0, 2);
}

void PQ8_fromPBport(string qName)
{
	pchar.Quest.PQ8_controlShore48_1.win_condition.l1 = "Location_Type";
	pchar.Quest.PQ8_controlShore48_1.win_condition.l1.location_type = "town";
	pchar.Quest.PQ8_controlShore48_1.function = "PQ8_controlShore48";
	pchar.Quest.PQ8_controlShore48_2.win_condition.l1 = "Location_Type";
	pchar.Quest.PQ8_controlShore48_2.win_condition.l1.location_type = "seashore";
	pchar.Quest.PQ8_controlShore48_2.function = "PQ8_controlShore48";
}

void PQ8_controlShore48(string qName)
{
	pchar.Quest.PQ8_controlShore48.over = "yes";
	pchar.Quest.PQ8_controlShore48_1.over = "yes";
	pchar.Quest.PQ8_controlShore48_2.over = "yes";
	if (pchar.location == "Shore48")
	{	//ГГ вовремя и на месте -->
		LAi_LocationFightDisable(&Locations[FindLocation("Shore48")], true); //закрываем локацию для боевки
		LocatorReloadEnterDisable("Shore48", "boat", true); //Не даем выходить в море
		chrDisableReloadToLocation = true;
		bDisableLandEncounters = true; //энкаунтеры не генерим
		pchar.questTemp.piratesLine = "Panama_inShore";
		pchar.questTemp.piratesLine.crewHero = makeint(GetCrewQuantity(pchar) / 100 / 2);	//запомним кол-во десанта от ГГ
		if (sti(pchar.questTemp.piratesLine.crewHero) > 5) pchar.questTemp.piratesLine.crewHero = 5;
		pchar.questTemp.piratesLine.crewRichard = 3;   //запомним кол-во десанта от Соукинса
		//бойцы ГГ. предположительно ГГ имет 3 абордажника.
		SetCrewQuantity(pchar, GetMinCrewQuantity(pchar)); //минимальную команду на шип
		iTemp = sti(pchar.questTemp.piratesLine.crewHero) ; //число обычных солдат ГГ
        for (i=1; i<=iTemp; i++)
        {
            sld = GetCharacter(NPC_GenerateCharacter("MHpirate_"+i, "pirate_"+(rand(24)+1), "man", "man", sti(pchar.rank), PIRATE, -1, true));
            LAi_SetWarriorType(sld);
			TakeNItems(sld,"potion1", rand(2)+1);
			LAi_warrior_DialogEnable(sld, false);
            LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
            ChangeCharacterAddressGroup(sld, "Shore48", "goto",  "goto12");
        }
        for (i=1; i<=iTemp; i++) //мушкетеры ГГ
        {
            sld = GetCharacter(NPC_GenerateMusketeer("MHmush_"+i, GetPirateMushketerModel(), "man", sti(pchar.rank), PIRATE, -1, false));
            LAi_SetWarriorType(sld);
			TakeNItems(sld,"potion1", rand(2)+1);
			LAi_warrior_DialogEnable(sld, false);
            LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
            ChangeCharacterAddressGroup(sld, "Shore48", "goto",  "goto12");
        }
		//бойцы Соукинса
		iTemp = sti(pchar.questTemp.piratesLine.crewRichard); //число обычных солдат Соукинса
        for (i=1; i<=iTemp; i++)
        {
            sld = GetCharacter(NPC_GenerateCharacter("RSpirate_"+i, "pirate_"+(rand(24)+1), "man", "man", 25, PIRATE, -1, true));
            LAi_SetWarriorType(sld);
			TakeNItems(sld,"potion1", rand(2)+1);
			LAi_warrior_DialogEnable(sld, false);
            LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
            ChangeCharacterAddressGroup(sld, "Shore48", "goto",  "goto2");
		}
        for (i=1; i<=iTemp; i++) //мушкетеры Соукинса
        {
            sld = GetCharacter(NPC_GenerateMusketeer("RSmush_"+i, GetPirateMushketerModel(), "man", 25, PIRATE, -1, false));
            LAi_SetWarriorType(sld);
			LAi_warrior_DialogEnable(sld, false);
			TakeNItems(sld,"potion1", rand(2)+1);
            LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
            ChangeCharacterAddressGroup(sld, "Shore48", "goto",  "goto2");
		}
		//офицеры Соукинса
        for (i=1; i<=3; i++)
        {
            sld = GetCharacter(NPC_GenerateCharacter("RSofficer_"+i, "officer_"+(rand(42)+1), "man", "man", 30, PIRATE, -1, true));
            FantomMakeCoolFighter(sld, 30, 90, 90, "blade28", "pistol4", 100);
			LAi_SetWarriorType(sld);
			LAi_warrior_DialogEnable(sld, false);
            LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
            ChangeCharacterAddressGroup(sld, "Shore48", "goto",  "goto4");
		}
		//сам Соукинс
		sld = characterFromId("Richard_Soukins");
		sld.dialog.currentNode = "Shore48";
		LAi_SetImmortal(sld, false);
		LAi_NoRebirthEnable(sld);
		GiveItem2Character(sld, "blade23");
		EquipCharacterbyItem(sld, "blade23");
		GiveItem2Character(sld, "pistol4");
		EquipCharacterbyItem(sld, "pistol4");
		TakeNItems(sld,"potion1", 5);
		RemoveCharacterCompanion(pchar, sld); //убираем Соукинса из компаньонов
		ChangeCharacterAddressGroup(sld, "Shore48", "goto", "goto11");
		LAi_SetActorType(sld);
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
		LAi_ActorDialog(sld, pchar, "", -1.0, 0);
		//прерывание на вход в джанглы
		pchar.Quest.PQ8_jungle_05.win_condition.l1 = "location";
		pchar.Quest.PQ8_jungle_05.win_condition.l1.location = "Panama_jungle_05";
		pchar.Quest.PQ8_jungle_05.function = "PQ8_jungle_05";
	}
	else
	{	//ГГ не успел или не туда зашёл -->
		iTemp = GetCharacterIndex("Richard_Soukins");
		if (iTemp != -1)
		{
			if(IsCompanion(&characters[iTemp]))
			{
				RemoveCharacterCompanion(pchar, &characters[iTemp]);
			}
			LAi_SetHuberType(&characters[iTemp]);
			LAi_group_MoveCharacter(&characters[iTemp], "PIRATE_CITIZENS");
            ChangeCharacterAddressGroup(&characters[iTemp], "PuertoPrincipe_townhall", "sit", "sit1");
            characters[iTemp].dialog.currentnode = "First time";
        }
        //Возвращаем на место Джекмена
		iTemp = GetCharacterIndex("Jackman");
		if (iTemp != -1)
		{
			LAi_SetHuberType(&characters[iTemp]);
			LAi_group_MoveCharacter(&characters[iTemp], "PIRATE_CITIZENS");
            ChangeCharacterAddressGroup(&characters[iTemp], "Pirates_townhall", "sit", "sit1");
            characters[iTemp].dialog.currentnode = "First time";
        }
		AddQuestRecord("Pir_Line_8_Panama", "6");
		AddQuestUserData("Pir_Line_8_Panama", "sSex", GetSexPhrase("ся","ась"));
		AddQuestUserData("Pir_Line_8_Panama", "sSex", GetSexPhrase("","а"));
		CloseQuestHeader("Pir_Line_8_Panama");
		SetTimerFunction("PQ8_openPanama", 0, 0, 40);
		LocatorReloadEnterDisable("Shore48", "reload1_back", true); //закрывем подходы к Панаме
		LocatorReloadEnterDisable("PortoBello_Jungle_01", "reload1_back", true);
		pchar.questTemp.piratesLine = "over";
	}
}

void PQ8_jungle_05(string qName)
{
	chrDisableReloadToLocation = true;
	pchar.questTemp.piratesLine = "Panama_inJungle";
	//--------------------------- наши ------------------------------
	//расставляем бойцов ГГ
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHpirate_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc5");
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHmush_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc6");
		}
    }
    //расставляем бойцов Соукинса
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSpirate_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc7");
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSmush_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc8");
		}
    }
    //офицеры Соукинса
	for (i=1; i<=3; i++)
    {
        iTemp = GetCharacterIndex("RSofficer_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc"+i);
		}
	}
	//сам Соукинс
	iTemp = GetCharacterIndex("Richard_Soukins");
	if(iTemp != -1 && !LAi_IsDead(&characters[iTemp]))
	{
		sld = &characters[iTemp];
		LAi_SetCurHPMax(sld);
		LAi_SetStayType(sld);
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
		ChangeCharacterAddressGroup(sld, pchar.location, "rld", "loc0");
	}
    //--------------------------- испанцы ------------------------------
    int Rank = sti(pchar.rank) - 5 + MOD_SKILL_ENEMY_RATE;
	if (Rank < 1) Rank = 1;
	for (i=12; i<=18; i++) //max - 21
    {
        sTemp = "sold_spa_"+(rand(7)+1);
		if (i==16) sTemp = "off_spa_2";
 		sld = GetCharacter(NPC_GenerateCharacter("SpaEnemy_"+i, sTemp, "man", "man", Rank, SPAIN, -1, true));
		FantomMakeCoolFighter(sld, sti(pchar.rank), 80, 80, "blade34", "pistol6", 80);
		sld.BreakTmplAndFightGroup = true;
		LAi_SetActorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
    }
	iTemp = 8;
	if (MOD_SKILL_ENEMY_RATE < 5) iTemp = 6;
    if (MOD_SKILL_ENEMY_RATE == 1) iTemp = 3;
	for (i=1; i<=iTemp; i++) //max - 11
    {
        sld = GetCharacter(NPC_GenerateMusketeer("SpaMush_"+i, "spa_rifle_"+(rand(2)+1), "man", Rank, SPAIN, -1, false));
		sld.MusketerDistance = 0;
		LAi_SetWarriorType(sld);
		if (MOD_SKILL_ENEMY_RATE > 5) TakeNItems(sld,"potion1", rand(MOD_SKILL_ENEMY_RATE)+1);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
	}
	//стравливаем
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
	LAi_group_SetCheck("EnemyFight", "PQ8_jungle_afterFight");
	pchar.Quest.PQ8_jungle_04.win_condition.l1 = "location";
	pchar.Quest.PQ8_jungle_04.win_condition.l1.location = "Panama_jungle_04";
	pchar.Quest.PQ8_jungle_04.function = "PQ8_jungle_04";
}


void PQ8_jungle_04(string qName)
{
	chrDisableReloadToLocation = true;
	pchar.questTemp.piratesLine = "Panama_inJungle";
	//--------------------------- наши ------------------------------
	//расставляем бойцов ГГ
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHpirate_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc5");
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHmush_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc6");
		}
    }
    //расставляем бойцов Соукинса
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSpirate_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc7");
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSmush_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc8");
		}
    }
    //офицеры Соукинса
	for (i=1; i<=3; i++)
    {
        iTemp = GetCharacterIndex("RSofficer_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc"+i);
		}
	}
	//сам Соукинс
	iTemp = GetCharacterIndex("Richard_Soukins");
	if(iTemp != -1 && !LAi_IsDead(&characters[iTemp]))
	{
		sld = &characters[iTemp];
		LAi_SetCurHPMax(sld);
		LAi_SetStayType(sld);
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
		ChangeCharacterAddressGroup(sld, pchar.location, "rld", "loc0");
	}
    //--------------------------- испанцы ------------------------------
    int Rank = sti(pchar.rank) - 5 + MOD_SKILL_ENEMY_RATE;
	if (Rank < 1) Rank = 1;
	for (i=11; i<=13; i++)
    {
        sTemp = "sold_spa_"+(rand(7)+1);
		if (i==14) sTemp = "off_spa_1";
 		sld = GetCharacter(NPC_GenerateCharacter("SpaEnemy_"+i, sTemp, "man", "man", Rank, SPAIN, -1, true));
        FantomMakeCoolFighter(sld, sti(pchar.rank), 90, 90, "blade26", "pistol4", 100);
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
    }
	for (i=14; i<=17; i++) //индейцы
    {
        if (i==14 || i==16) sTemp = "indsair2";
		else sTemp = "indsar1";
		sld = GetCharacter(NPC_GenerateCharacter("IndEnemy_"+i, "indsair2", "man", "man", Rank, SPAIN, -1, true)); //to_do анимация
		FantomMakeCoolFighter(sld, Rank, 90, 90, "topor2", "", 5);
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
    }
	iTemp = 10;
	if (MOD_SKILL_ENEMY_RATE < 5) iTemp = 7;
    if (MOD_SKILL_ENEMY_RATE == 1) iTemp = 4;
    for (i=1; i<=iTemp; i++)
    {
        sld = GetCharacter(NPC_GenerateMusketeer("SpaMush_"+i, "spa_rifle_"+(rand(2)+1), "man", Rank, SPAIN, -1, false));
		if (MOD_SKILL_ENEMY_RATE > 5) TakeNItems(sld,"potion1", rand(MOD_SKILL_ENEMY_RATE)+1);
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
	}
	//стравливаем
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
	LAi_group_SetCheck("EnemyFight", "PQ8_jungle_afterFight");
	pchar.Quest.PQ8_jungle_03.win_condition.l1 = "location";
	pchar.Quest.PQ8_jungle_03.win_condition.l1.location = "Panama_jungle_03";
	pchar.Quest.PQ8_jungle_03.function = "PQ8_jungle_03";
}

void PQ8_jungle_03(string qName)
{
	chrDisableReloadToLocation = true;
	pchar.questTemp.piratesLine = "Panama_inJungle";
	//--------------------------- наши ------------------------------
	//расставляем бойцов ГГ
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHpirate_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc5");
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHmush_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc6");
		}
    }
    //расставляем бойцов Соукинса
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSpirate_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc7");
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSmush_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc8");
		}
    }
    //офицеры Соукинса
	for (i=1; i<=3; i++)
    {
        iTemp = GetCharacterIndex("RSofficer_"+i);
		if(iTemp != -1)
		{
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc"+i);
		}
	}
	//сам Соукинс
	iTemp = GetCharacterIndex("Richard_Soukins");
	if(iTemp != -1 && !LAi_IsDead(&characters[iTemp]))
	{
		sld = &characters[iTemp];
		LAi_SetCurHPMax(sld);
		LAi_SetStayType(sld);
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
		ChangeCharacterAddressGroup(sld, pchar.location, "rld", "loc0");
	}
    //--------------------------- испанцы ------------------------------
    int Rank = sti(pchar.rank) - 5 + MOD_SKILL_ENEMY_RATE;
	if (Rank < 1) Rank = 1;
	for (i=11; i<=16; i++)
    {
        sTemp = "sold_spa_"+(rand(7)+1);
		if (i==13) sTemp = "off_spa_2";
 		sld = GetCharacter(NPC_GenerateCharacter("SpaEnemy_"+i, sTemp, "man", "man", Rank, SPAIN, -1, true));
		FantomMakeCoolFighter(sld, sti(pchar.rank), 90, 90, "blade26", "pistol4", 120);
		sld.BreakTmplAndFightGroup = true;
		LAi_SetActorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
    }
	iTemp = 10;
	if (MOD_SKILL_ENEMY_RATE < 5) iTemp = 7;
    if (MOD_SKILL_ENEMY_RATE == 1) iTemp = 4;
    for (i=1; i<=iTemp; i++)
    {
        sld = GetCharacter(NPC_GenerateMusketeer("SpaMush_"+i, "spa_rifle_"+(rand(2)+1), "man", Rank, SPAIN, -1, false));
		sld.MusketerDistance = 0;
		if (MOD_SKILL_ENEMY_RATE > 5) TakeNItems(sld,"potion1", rand(MOD_SKILL_ENEMY_RATE)+1);
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
	}
	//стравливаем
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
	LAi_group_SetCheck("EnemyFight", "PQ8_jungle_afterFight");
	pchar.Quest.PQ8_ExitTown.win_condition.l1 = "location";
	pchar.Quest.PQ8_ExitTown.win_condition.l1.location = "Panama_ExitTown";
	pchar.Quest.PQ8_ExitTown.function = "PQ8_ExitTown";
}

void PQ8_ExitTown(string qName)
{
	iTemp = GetCharacterIndex("Richard_Soukins");
	if(LAi_IsDead(&characters[iTemp]))
	{	//Соукинс мёртв, задание выполнено
		AddQuestRecord("Pir_Line_8_Panama", "7");
		pchar.questTemp.piratesLine = "Panama_RichardGoodWork";
	}
	else
	{	//Соукинс жив, задание провалено
		AddQuestRecord("Pir_Line_8_Panama", "8");
		pchar.questTemp.piratesLine = "Panama_RichardBadWork";
		sld = &characters[iTemp];
		LAi_SetCurHPMax(sld);
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
		ChangeCharacterAddressGroup(sld, pchar.location, "rld", "loc2");
	}
	chrDisableReloadToLocation = true;
	// Сохраняем результат встречи с Соукинсом при выходе из Панамы.
	//--------------------------- наши ------------------------------
	//расставляем бойцов ГГ
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHpirate_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].lifeDay = 10;
			SaveCurrentNpcQuestDateParam(&characters[iTemp], "LifeTimeCreate");
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc15");
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHmush_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].lifeDay = 10;
			SaveCurrentNpcQuestDateParam(&characters[iTemp], "LifeTimeCreate");
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc16");
		}
    }
    //расставляем бойцов Соукинса
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSpirate_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].lifeDay = 10;
			SaveCurrentNpcQuestDateParam(&characters[iTemp], "LifeTimeCreate");
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc17");
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSmush_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].lifeDay = 10;
			SaveCurrentNpcQuestDateParam(&characters[iTemp], "LifeTimeCreate");
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc18");
		}
    }
    //офицеры Соукинса
	for (i=1; i<=3; i++)
    {
        iTemp = GetCharacterIndex("RSofficer_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].lifeDay = 10;
			SaveCurrentNpcQuestDateParam(&characters[iTemp], "LifeTimeCreate");
			LAi_group_MoveCharacter(&characters[iTemp], LAI_GROUP_PLAYER);
			ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "rld",  "loc"+(i+10));
		}
	}
    //--------------------------- люди Моргана ------------------------------
    for (i=1; i<=5; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter("HMpirate_"+i, "pirate_"+(rand(24)+1), "man", "man", 25, PIRATE, 10, true));
        LAi_SetWarriorType(sld);
		LAi_warrior_DialogEnable(sld, false);
        LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "loc20");
	}
    for (i=1; i<=5; i++)
    {
        sld = GetCharacter(NPC_GenerateMusketeer("HMmush_"+i, "mushketer_"+(rand(4)+1), "man", 25, PIRATE, 10, false));
        LAi_SetWarriorType(sld);
		LAi_warrior_DialogEnable(sld, false);
        LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "loc19");
	}
    //--------------------------- испанцы ------------------------------
    int Rank = sti(pchar.rank) - 5 + MOD_SKILL_ENEMY_RATE;
	if (Rank < 1) Rank = 1;
	for (i=11; i<=15; i++)
    {
        sTemp = "sold_spa_"+(rand(7)+1);
		if (i==12) sTemp = "off_spa_1";
 		sld = GetCharacter(NPC_GenerateCharacter("SpaEnemy_"+i, sTemp, "man", "man", Rank, SPAIN, -1, true));
        FantomMakeCoolFighter(sld, sti(pchar.rank), 90, 90, "blade26", "pistol4", 120);
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
    }
    for (i=1; i<=10; i++)
    {
        sld = GetCharacter(NPC_GenerateMusketeer("SpaMush_"+i, "spa_rifle_"+(rand(2)+1), "man", Rank, SPAIN, -1, false));
		if (MOD_SKILL_ENEMY_RATE > 5) TakeNItems(sld,"potion1", rand(MOD_SKILL_ENEMY_RATE)+1);
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "aloc"+i);
	}
	//стравливаем
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
	LAi_group_SetCheck("EnemyFight", "PQ8_ExitTown_afterFight");
}

void PQ8_piratesInPanama(string qName)
{
	DeleteAttribute(&colonies[FindColony("PortoBello")], "notCaptured"); //Порто Белло можно себе
	//фантома мэра Панамы - резиденцию
	rCharacter = characterFromId("Panama_Mayor");
	sld = GetCharacter(NPC_GenerateCharacter("FantomMayor", rCharacter.model, "man", "man", sti(rCharacter.rank), sti(rCharacter.nation), -1, false));
	sld.quest.type = "hovernor";
	sld.City = "Panama";
	sld.location = "Panama_TownhallRoom";
	sld.greeting = "spa_gov_common";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
    sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
	sld.Dialog.CurrentNode = "PQ8_Mayor";
	LAi_SetStayType(sld);
	LAi_LoginInCaptureTown(sld, true);
	//--------------------------- наши ------------------------------
	//расставляем бойцов ГГ
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHpirate_"+i);
		if(iTemp != -1)
		{
			sld = &characters[iTemp];
			LAi_warrior_DialogEnable(sld, true);
			LAi_LoginInCaptureTown(sld, true);
			sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
			sld.Dialog.CurrentNode = "PQ8_PirInPan";
			PlaceCharacter(sld, "goto", pchar.location);
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHmush_"+i);
		if(iTemp != -1)
		{
			sld = &characters[iTemp];
			LAi_warrior_DialogEnable(sld, true);
			LAi_LoginInCaptureTown(sld, true);
			sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
			sld.Dialog.CurrentNode = "PQ8_PirInPan";
			PlaceCharacter(sld, "goto", pchar.location);
		}
    }
    //расставляем бойцов Соукинса
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSpirate_"+i);
		if(iTemp != -1)
		{
			sld = &characters[iTemp];
			LAi_warrior_DialogEnable(sld, true);
			LAi_LoginInCaptureTown(sld, true);
			sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
			sld.Dialog.CurrentNode = "PQ8_RsPirInPan";
			PlaceCharacter(sld, "goto", pchar.location);
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSmush_"+i);
		if(iTemp != -1)
		{
			sld = &characters[iTemp];
			LAi_warrior_DialogEnable(sld, true);
			LAi_LoginInCaptureTown(sld, true);
			sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
			sld.Dialog.CurrentNode = "PQ8_RsPirInPan";
			PlaceCharacter(sld, "goto", pchar.location);
		}
    }
    //офицеры Соукинса
	for (i=1; i<=3; i++)
    {
        iTemp = GetCharacterIndex("RSofficer_"+i);
		if(iTemp != -1)
		{
			sld = &characters[iTemp];
			LAi_warrior_DialogEnable(sld, true);
			LAi_LoginInCaptureTown(sld, true);
			sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
			sld.Dialog.CurrentNode = "PQ8_RsPirInPan";
			PlaceCharacter(sld, "goto", pchar.location);
		}
	}
    //--------------------------- люди Моргана ------------------------------
    for (i=1; i<=5; i++)
    {
        iTemp = GetCharacterIndex("HMpirate_"+i);
		if(iTemp != -1)
		{
			sld = &characters[iTemp];
			LAi_warrior_DialogEnable(sld, true);
			LAi_LoginInCaptureTown(sld, true);
			sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
			sld.Dialog.CurrentNode = "PQ8_MPirInPan";
			PlaceCharacter(sld, "goto", pchar.location);
		}
	}
    for (i=1; i<=5; i++)
    {
        iTemp = GetCharacterIndex("HMmush_"+i);
		if(iTemp != -1)
		{
			sld = &characters[iTemp];
			LAi_warrior_DialogEnable(sld, true);
			LAi_LoginInCaptureTown(sld, true);
			sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
			sld.Dialog.CurrentNode = "PQ8_MPirInPan";
			PlaceCharacter(sld, "goto", pchar.location);
		}
	}
}

void PQ8_MorganInPanama_1(string qName)
{
	chrDisableReloadToLocation = true;
	//делаем видимым ключ
	sld = ItemsFromID("keyPanama");
	sld.shown = true;
	sld.startLocation = "Panama_prison";
	sld.startLocator = "item1";
	BackItemDescribe("keyPanama");
	BackItemName("keyPanama");
	//Моргана в город и на диалог
	GetCharacterPos(pchar, &locx, &locy, &locz);
	sld = characterFromId("Henry Morgan");
	sld.dialog.currentNode = "PL_Q8_Panama1";
	LAi_LoginInCaptureTown(sld, true);
	ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto6");
	LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void PL_Q8_ResEnterBox(string qName)
{
	if (CheckCharacterItem(pchar, "keyPanama"))
    {
		TakeItemFromCharacter(pchar, "keyPanama");
		chrDisableReloadToLocation = true;
		
		pchar.quest.PL_Q8_MorganBox.win_condition.l1 = "Money";
		pchar.quest.PL_Q8_MorganBox.win_condition.l1.value = 50000000;
		pchar.quest.PL_Q8_MorganBox.win_condition.l1.operation = ">=";
		PChar.quest.PL_Q8_MorganBox.function = "PL_Q8_MorganBox";
	}
	else
	{
		Pchar.quest.PL_Q8_ResEnterBox_2.win_condition.l1 = "ExitFromLocation";
        Pchar.quest.PL_Q8_ResEnterBox_2.win_condition.l1.location = pchar.location;
        Pchar.quest.PL_Q8_ResEnterBox_2.function = "PL_Q8_ResEnterBox_2";
	}
}

void PL_Q8_ResEnterBox_2(string qName)
{
	pchar.quest.PL_Q8_ResEnterBox.win_condition.l1 = "locator";
	pchar.quest.PL_Q8_ResEnterBox.win_condition.l1.location = "Panama_TownhallRoom";
	pchar.quest.PL_Q8_ResEnterBox.win_condition.l1.locator_group = "box";
	pchar.quest.PL_Q8_ResEnterBox.win_condition.l1.locator = "private1";
	pchar.quest.PL_Q8_ResEnterBox.function = "PL_Q8_ResEnterBox";
}

void PL_Q8_MorganBox(string qName)
{
	sld = characterFromId("Henry Morgan");
	sld.dialog.currentNode = "PL_Q8_Panama2";
	ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto6");
	LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
	LAi_ActorDialog(sld, pchar, "", 0, 0);
}

void PL_Q8_inPanamaFort2(string qName)
{
	Pchar.quest.PL_Q8_inPanamaPirates_1.win_condition.l1 = "location";
	Pchar.quest.PL_Q8_inPanamaPirates_1.win_condition.l1.location = "Panama_town";
	Pchar.quest.PL_Q8_inPanamaPirates_1.function = "PL_Q8_inPanamaPirates_1";
}

void PL_Q8_inPanamaPirates_1(string qName)
{
	chrDisableReloadToLocation = true;
	//--------------------------- меняем ноды у оставшихся пиратов ------------------------------
	//расставляем бойцов ГГ
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHpirate_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].Dialog.CurrentNode = "PQ8_FackMorgan";
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewHero); i++)
    {
        iTemp = GetCharacterIndex("MHmush_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].Dialog.CurrentNode = "PQ8_FackMorgan";
		}
    }
    //расставляем бойцов Соукинса
	for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSpirate_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].Dialog.CurrentNode = "PQ8_FackMorgan";
		}
    }
    for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
    {
        iTemp = GetCharacterIndex("RSmush_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].Dialog.CurrentNode = "PQ8_FackMorgan";
		}
    }
    //офицеры Соукинса
	for (i=1; i<=3; i++)
    {
        iTemp = GetCharacterIndex("RSofficer_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].Dialog.CurrentNode = "PQ8_FackMorgan";
		}
	}
    //--------------------------- люди Моргана ------------------------------
    for (i=1; i<=5; i++)
    {
        iTemp = GetCharacterIndex("HMpirate_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].Dialog.CurrentNode = "PQ8_FackMorgan";
		}
	}
    for (i=1; i<=5; i++)
    {
        iTemp = GetCharacterIndex("HMmush_"+i);
		if(iTemp != -1)
		{
			characters[iTemp].Dialog.CurrentNode = "PQ8_FackMorgan";
		}
	}
	//разговорный пират
	GetCharacterPos(pchar, &locx, &locy, &locz);
	sld = GetCharacter(NPC_GenerateCharacter("QuestPirate", "pirate_"+(rand(24)+1), "man", "man", 25, PIRATE, 0, true));
	sld.Dialog.Filename = "Quest\PiratesLine_dialog.c";
	sld.dialog.currentNode = "PL_Q8_QFackMorgan";
	LAi_LoginInCaptureTown(sld, true);
	ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto10");
	LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void PL_Q8_backFight_1(string qName)
{
    int Rank = sti(pchar.rank) - 5 + MOD_SKILL_ENEMY_RATE;
	if (Rank < 1) Rank = 1;
	for (i=1; i<=8; i++)
    {
        sTemp = "sold_spa_"+(rand(7)+1);
		if (i==2) sTemp = "off_spa_2";
 		sld = GetCharacter(NPC_GenerateCharacter("SpaEnemy_"+i, sTemp, "man", "man", Rank, SPAIN, 12, true));
		FantomMakeCoolFighter(sld, sti(pchar.rank), 90, 90, "blade26", "pistol4", 120);
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "loc1");
    }
    for (i=1; i<=4; i++)
    {
        sld = GetCharacter(NPC_GenerateMusketeer("SpaMush_"+i, "spa_rifle_"+(rand(2)+1), "man", Rank, SPAIN, 12, false));
		if (MOD_SKILL_ENEMY_RATE > 5) TakeNItems(sld,"potion1", rand(MOD_SKILL_ENEMY_RATE)+1);
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "rld",  "loc3");
	}
	//стравливаем
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Пиратская линейка     конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////
