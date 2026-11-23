// Gregg: вынесено из GenQuests.c (сплит god-файла) — квест трюма (Hold_GenQuest).
// #include на исходное место — file-scope глобалы/дефайны остаются в GenQuests.c выше include; поведение идентично.

void Hold_GenQuest_Init(ref chref)
{
	ref rColony;
	string sColony;

	if(!CheckAttribute(chref,"EncType")) chref.EncType = "trade";

	switch(chref.EncType)
	{
		case "trade":
			chref.Hold_GenQuest.variant = rand(2);
		break;
		case "war":
			chref.Hold_GenQuest.variant = 0;
		break;
		case "pirate":
			chref.Hold_GenQuest.variant = rand(1);
		break;
	}

	switch(sti(chref.Hold_GenQuest.variant))
	{
		case 0: // "наводка"
			chref.Hold_GenQuest.Goods = Hold_GenQuest_GetVictimShipGoods();
			chref.Hold_GenQuest.Nation = FindEnemyNation2NationWithoutPirates(GetBaseHeroNation());
			if(sti(chref.Hold_GenQuest.Nation) < 0)
			{
				chref.Hold_GenQuest.Nation = rand(3);
			}
			chref.Hold_GenQuest.Name = GenerateRandomName_Generator(sti(chref.Hold_GenQuest.Nation), "man");

			sColony = Sea_FindNearColony();
			if (sColony != "none")
			{
				chref.Hold_GenQuest.ToCity = FindAlliedColonyForNationExceptColony(sColony);
			}
			else
			{
				chref.Hold_GenQuest.ToCity = GetQuestNationsCity(sti(chref.Hold_GenQuest.Nation));
			}
			chref.Hold_GenQuest.FromCity = FindAlliedColonyForNationExceptColony(chref.Hold_GenQuest.ToCity);
			chref.Hold_GenQuest.ShipName = GenerateRandomNameToShip(sti(chref.Hold_GenQuest.Nation));
			if(rand(1) == 0) 	chref.Hold_GenQuest.City = chref.Hold_GenQuest.ToCity;
			else 				chref.Hold_GenQuest.City = chref.Hold_GenQuest.FromCity;
		break;

		case 1: // "подельник"
			chref.Hold_GenQuest.Nation = rand(3);
			chref.Hold_GenQuest.City = GetQuestNationsCity(sti(chref.Hold_GenQuest.Nation));
			chref.Hold_GenQuest.Name = GenerateRandomName_Generator(sti(chref.Hold_GenQuest.Nation), "man");
			chref.Hold_GenQuest.PirateName = "l" + rand(GetNamesCount(NAMETYPE_VIP) - 1);
		break;

		case 2: // "выкуп"
			chref.Hold_GenQuest.City = GetQuestNationsCity(rand(3));
			rColony = GetColonyByIndex(FindColony(chref.Hold_GenQuest.City));
			chref.Hold_GenQuest.Nation = sti(rColony.nation);
			chref.Hold_GenQuest.Name = GenerateRandomName_Generator(sti(chref.Hold_GenQuest.Nation), "man");
		break;
	}
}

int Hold_GenQuest_GetVictimShipGoods()
{
	int aGoods[13];
	aGoods[0] = GOOD_SANDAL;
	aGoods[1] = GOOD_TOBACCO;
	aGoods[2] = GOOD_COFFEE;
	aGoods[3] = GOOD_MAHOGANY;
	aGoods[4] = GOOD_FOOD;
	aGoods[5] = GOOD_EBONY;
	aGoods[6] = GOOD_SILK;
	aGoods[7] = GOOD_RUM;
	aGoods[8] = GOOD_WEAPON;
	aGoods[9] = GOOD_WINE;
	aGoods[10] = GOOD_CHOCOLATE;
	aGoods[11] = GOOD_CINNAMON;
	aGoods[12] = GOOD_LEATHER;
	return aGoods[rand(12)];
}

void Hold_GenQuest_SetMerchant_pre(string qName)
{
	SetFunctionLocationCondition("Hold_GenQuest_SetMerchant", pchar.GenQuest.Hold_GenQuest.Island, false);
}

void Hold_GenQuest_SetMerchant(string qName)
{
	ref chref, sld;
	pchar.quest.Hold_GenQuest_MerchantOver.over = "yes";

	int iChar = NPC_GenerateCharacter("Hold_QuestMerchant", "off_spa_2", "man", "man", 5, sti(pchar.GenQuest.Hold_GenQuest.Nation), 3, true);
	makeref(chref, Characters[iChar]);
	int iRank = SetShipTypeMerchant(chref);
	chref.Ship.Name = pchar.GenQuest.Hold_GenQuest.ShipName;
	SetFantomParamHunter(chref); //крутые парни

	SetCaptanModelByEncType(chref, "trade");
	int iSpace = GetCharacterFreeSpace(chref, sti(pchar.GenQuest.Hold_GenQuest.Goods));

    float del = makefloat(7 - iRank + rand(1))/10;
    if (del > 1.0 ) del = 1;
    Log_TestInfo("goods load = "+iSpace+"/"+makeint(iSpace*del));
    iSpace = makeint(iSpace * del); // чтоб не так жирно было
    Fantom_SetCharacterGoods(chref, sti(pchar.GenQuest.Hold_GenQuest.Goods), iSpace,1);

	string sGroup = "Sea_" + chref.id
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
    Group_SetType(sGroup, "trade");

	int IslandID = FindIsland(GetArealByCityName(pchar.GenQuest.Hold_GenQuest.City));
	ref pLoc = &Islands[IslandID];
	float x, y, z;
	FindIslandLocatorXYZ(IslandID, "reload1", &x, &y, &z);
	string sLocator = GetSeaQuestShipFarLocator(pLoc, "Quest_ships", x, y, z);

	chref.DontRansackCaptain = true;

	Group_SetAddress(sGroup, GetArealByCityName(pchar.GenQuest.Hold_GenQuest.City),"quest_ships", sLocator);
    Group_AddCharacter(sGroup, chref.id);
    Group_SetGroupCommander(sGroup, chref.id);
	Group_SetTaskRunaway(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);

	pchar.quest.Hold_GenQuest_SinkMerchant.win_condition.l1 = "Character_sink";
    pchar.quest.Hold_GenQuest_SinkMerchant.win_condition.l1.character = "Hold_QuestMerchant";
    pchar.quest.Hold_GenQuest_SinkMerchant.function = "Hold_GenQuest_SinkMerchant";

	pchar.quest.Hold_GenQuest_CaptureMerchant.win_condition.l1 = "Character_Capture";
    pchar.quest.Hold_GenQuest_CaptureMerchant.win_condition.l1.character = "Hold_QuestMerchant";
    pchar.quest.Hold_GenQuest_CaptureMerchant.function = "Hold_GenQuest_CaptureMerchant";

	SetFunctionTimerConditionParam("Hold_GenQuest_TimeIsOver", 0, 0, 1, MakeInt(24 - GetHour()), false);

	int gcount = rand(3)-1;
    if (gcount < 0 || makeint(pchar.rank) < 5) gcount = 0;

    for (int i = 0; i < gcount; i++) // генерим эскорт
    {
        iChar = NPC_GenerateCharacter("Hold_QuestMerchantGuard_"+i, "off_spa_2", "man", "man", 5, sti(pchar.GenQuest.Hold_GenQuest.Nation), 3, true));
        makeref(sld, Characters[iChar]);
        SetShipHunter(sld);
        SetFantomParamHunter(sld); //крутые парни
        SetCaptanModelByEncType(sld, "war");
        sld.Ship.Mode = "war";
		sld.DontRansackCaptain = true;
        Group_AddCharacter(sGroup, sld.id);
    }
    chref.gcount = gcount;
}

void Hold_GenQuest_MerchantOver(string qName) // время вышло
{
	pchar.quest.Hold_GenQuest_SetMerchant.over = "yes";
	AddQuestRecord("HoldQuest", "3");
	AddQuestUserData("HoldQuest", "sShipName", pchar.GenQuest.Hold_GenQuest.ShipName);
	AddQuestUserData("HoldQuest", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.Hold_GenQuest.Goods)].Name + "Gen")));
	CloseQuestHeader("HoldQuest");
	DeleteAttribute(pchar,"GenQuest.Hold_GenQuest");
}

void Hold_GenQuest_SinkMerchant(string qName)
{
	pchar.quest.Hold_GenQuest_CaptureMerchant.over = "yes";
	pchar.quest.Hold_GenQuest_TimeIsOver.over = "yes";
	AddQuestRecord("HoldQuest", "5");
	AddQuestUserData("HoldQuest", "sShipName", pchar.GenQuest.Hold_GenQuest.ShipName);
	AddQuestUserData("HoldQuest", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.Hold_GenQuest.Goods)].Name + "Gen")));
	CloseQuestHeader("HoldQuest");
	DeleteAttribute(pchar,"GenQuest.Hold_GenQuest");
}

void Hold_GenQuest_CaptureMerchant(string qName)
{
	pchar.quest.Hold_GenQuest_SinkMerchant.over = "yes";
	pchar.quest.Hold_GenQuest_TimeIsOver.over = "yes";
	AddQuestRecord("HoldQuest", "4");
	AddQuestUserData("HoldQuest", "sShipName", pchar.GenQuest.Hold_GenQuest.ShipName);
	AddQuestUserData("HoldQuest", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.Hold_GenQuest.Goods)].Name + "Gen")));
	CloseQuestHeader("HoldQuest");
	DeleteAttribute(pchar,"GenQuest.Hold_GenQuest");
}

void Hold_GenQuest_TimeIsOver(string qName)
{
	pchar.quest.Hold_GenQuest_SinkMerchant.over = "yes";
	pchar.quest.Hold_GenQuest_CaptureMerchant.over = "yes";
	ref sld = characterFromId("Hold_QuestMerchant");
	Group_DeleteGroup("Sea_" + sld.id);
	AddQuestRecord("HoldQuest", "2");
	AddQuestUserData("HoldQuest", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("HoldQuest", "sShipName", pchar.GenQuest.Hold_GenQuest.ShipName);
	CloseQuestHeader("HoldQuest");
	DeleteAttribute(pchar,"GenQuest.Hold_GenQuest");
}

void Hold_GenQuest_GenerateChar()
{
	int Rank = sti(pchar.rank) + rand(10);
	ref sld = GetCharacter(NPC_GenerateCharacter("Hold_GenQuestChar", "trader_"+(rand(5)+1), "man", "man", 10, sti(pchar.GenQuest.Hold_GenQuest.Nation), 3, true));
	FantomMakeCoolFighter(sld, Rank, 100, 90, "blade9", "pistol3", 50);
	sld.name = pchar.GenQuest.Hold_GenQuest.Name;
	sld.lastname = "";
	sld.dialog.FileName = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "Hold_GenQuest1";
	sld.greeting = "cit_quest";
	sld.CityType = "citizen";
	sld.City = pchar.GenQuest.Hold_GenQuest.City;
	LAi_SetImmortal(sld, true);	// До поры нельзя убить
	string sTemp = pchar.GenQuest.Hold_GenQuest.City + "_" + pchar.GenQuest.Hold_GenQuest.found;
	LAi_SetStayType(sld);
	ChangeCharacterAddressGroup(sld, sTemp, "goto", "goto2");
}

void Hold_GenQuest_AfterBattle(string qName)
{
	chrDisableReloadToLocation = false;
	LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], true);
}

void Hold_GenQuest_FindCharTimeIsOver(string qName)
{
	ref sld = characterFromId("Hold_QuestChar");
	sld.LifeDay = 0;
	AddQuestRecord("HoldQuest", "9");
	AddQuestUserData("HoldQuest", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("HoldQuest", "sName", pchar.GenQuest.Hold_GenQuest.Name);
	pchar.GenQuest.Hold_GenQuest = "prisoner_escaped";
}

void Hold_GenQuest_PrisonerEscaped()
{
	pchar.quest.Hold_GenQuest_ExitFromTown.win_condition.l1			= "EnterToSea";
	pchar.quest.Hold_GenQuest_ExitFromTown.function    				= "Hold_GenQuest_ExitFromTownFight";
	pchar.GenQuest.Hold_GenQuest.canSpeakSailor = true;
	pchar.TownEscape = true;
}

void Hold_GenQuest_ExitFromTownFight(string qName)
{
	Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_41", "Logs.txt", "#space#", " "));
	bQuestDisableMapEnter = true;
	QuestCloseSeaExit();
    DoQuestFunctionDelay("Hold_GenQuest_MeetWithSailor", 1);
}

void Hold_GenQuest_MeetWithSailor(string qName)
{
	Sea_DeckBoatStartNow(pchar);
}

void Hold_GenQuest_GetMapTimeIsOver(string qName)
{
	pchar.quest.Hold_GenQuest_SpeakMapCharTavern.over = "yes";
	ref sld = characterFromId("Hold_QuestChar");
	sld.LifeDay = 0;
	AddQuestRecord("HoldQuest", "14");
	AddQuestUserData("HoldQuest", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("HoldQuest", "sName", pchar.GenQuest.Hold_GenQuest.Name);
	CloseQuestHeader("HoldQuest");
	DeleteAttribute(pchar,"GenQuest.Hold_GenQuest");
}

void Hold_GenQuest_GenerateMapChar()
{
	ref sld = GetCharacter(NPC_GenerateCharacter("Hold_GenQuestChar", "citiz_"+(rand(11)+1), "man", "man", 5, sti(pchar.GenQuest.Hold_GenQuest.Nation), 3, true));
	sld.name = pchar.GenQuest.Hold_GenQuest.Name;
	sld.lastname = "";
	LAi_SetImmortal(sld, true);
	pchar.quest.Hold_GenQuest_SpeakMapCharTavern.win_condition.l1 = "location";
	pchar.quest.Hold_GenQuest_SpeakMapCharTavern.win_condition.l1.location = pchar.GenQuest.Hold_GenQuest.City + "_tavern";
	pchar.quest.Hold_GenQuest_SpeakMapCharTavern.function = "Hold_GenQuest_SpeakMapChar";
}

void Hold_GenQuest_SpeakMapChar(string qName)
{
	chrDisableReloadToLocation = true;
	ref sld = characterFromId("Hold_GenQuestChar");
	sld.dialog.filename = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "Hold_GenQuest80";
	sld.greeting = "cit_quest";
	sld.CityType = "citizen";
	sld.City = pchar.GenQuest.Hold_GenQuest.City;
 	ChangeCharacterAddressGroup(sld, pchar.GenQuest.Hold_GenQuest.city + "_tavern", "goto", "goto1");
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1.0, 0);
}

void Hold_GenQuest_GenerateTreasureMap(ref item)
{
    item.MapIslId   = GetIslandForTreasure();
    item.MapLocId   = GetLocationForTreasure(item.MapIslId);
    item.MapBoxId   = GetBoxForTreasure(item.MapIslId, item.MapLocId);
	item.MapOwnId   = GetName(NAMETYPE_VIP, pchar.GenQuest.Hold_GenQuest.PirateName, NAME_NOM);

	DeleteAttribute(item, "BoxTreasure");

	pchar.GenQuest.Hold_GenQuest.Treasure.Island = item.MapIslId;
	pchar.GenQuest.Hold_GenQuest.Treasure.Location = item.MapLocId;
	pchar.GenQuest.Hold_GenQuest.Treasure.Locator = item.MapBoxId;

	SetFunctionLocationCondition("Hold_GenQuest_SetTreasureBoxFromMap", item.MapLocId, false);
}

void Hold_GenQuest_SetTreasureBoxFromMap(string qName)
{
    ref    loc;
    string box, itmName;
    aref   item, arToBox, arFromBox;
	if (GetCharacterItem(pchar, "mapQuest") > 0 )
    {
        Log_Info(GetConvertStrWithReplace("Variable_GenQuests_42", "Logs.txt", "#space#", " "));
		//Запрет спавна проклятого кэпа в этой локации
		ref locLoad = &locations[reload_location_index];
		pchar.treasurelocation = locLoad.id;

        PlaySound("interface\notebook.wav");
		Statistic_AddValue(Pchar, "Treasure", 1);

		Items_FindItem("mapQuest", &item);

		if(sti(pchar.GenQuest.Hold_GenQuest.Treasure) == 0)
		{
			itmName = GenQuest_GenerateTotem();
			item.BoxTreasure.(itmName) = 1;

			if(rand(1) == 0)
			{
				itmName = GenQuest_GenerateGoodBlade();
				item.BoxTreasure.(itmName) = 1;
			}

			itmName = "icollection"; // генерим сокровища
			item.BoxTreasure.(itmName) = 1 + makeint(sti(pchar.rank)/6);

			itmName = "jewelry2";
			item.BoxTreasure.(itmName) = rand(5) + 5;

			itmName = "jewelry3";
			item.BoxTreasure.(itmName) = rand(4) + 10;
		}
		else
		{
			itmName = GetGeneratedItem("blade5");
			item.BoxTreasure.(itmName) = rand(8) + 4;

			itmName = "mineral5"; // генерим ведра
			item.BoxTreasure.(itmName) = rand(5) + 2;

			itmName = "mineral8"; // генерим  башмаки
			item.BoxTreasure.(itmName) = rand(7) + 4;

			if (rand(2) == 1)
	        {
            	item.BoxTreasure.rabble = 5 + rand(100);
	        }
	        else
	        {
				itmName = GetGeneratedItem("blade1");
                item.BoxTreasure.(itmName) = 5 + rand(100);//fix
	        }
			if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral4 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral6 = 5 + rand(200);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral7 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral9 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral10 = 25 + rand(300);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.suit_1 = 1;
	        }
		}
        box = item.MapBoxId;

        loc = &locations[FindLocation(item.MapLocId)];
        loc.(box).items = "";

        makearef(arToBox, loc.(box).items);
        makearef(arFromBox, item.BoxTreasure);
        CopyAttributes(arToBox, arFromBox);

        loc.(box) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
        loc.(box).Treasure = true; // признак сокровища в сундуке
		loc.(box).Hold_GenQuest_Treasure = true;

        DeleteAttribute(item, "MapIslId");
        TakeNItems(pchar, "mapQuest", -1);

		pchar.GenQuest.Hold_GenQuest = "in_TreasureLoc";
	}
}