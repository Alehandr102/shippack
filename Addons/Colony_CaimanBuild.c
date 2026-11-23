// Gregg: вынесено из Colony.c (сплит god-файла) — одноразовая постройка колонии Кайман (Create*Caiman*/Create*InCaiman/CreateTownCaiman).
// #include на исходное место — file-scope дефайны/глобалы остаются в Colony.c выше include; поведение идентично.

void CreateCaimanCitizens(aref loc)
{
	if(!CheckAttribute(loc, "id"))
	{
	 	return;
	}

	if(loc.id != "Caiman_town")
	{
		return;
	}

	if(PChar.ColonyBuilding.Defence.Battle == true)
	{
		DeleteAllFantomCharactersFromLocation(loc);
		return;
	}

	if(PChar.ColonyBuilding.Defence.DeadInTown == true)
	{
		SetDeadmansToLocation(loc, "citizens");
		PChar.ColonyBuilding.Defence.DeadInTown = false;

		if(CheckAttribute(loc, "hidden_effects"))
		{
			DeleteAttribute(loc, "hidden_effects");
		}
		if(CheckAttribute(&Locations[FindLocation("Caiman_ExitTown")], "hidden_effects"))
		{
			DeleteAttribute(&Locations[FindLocation("Caiman_ExitTown")], "hidden_effects");
		}
		return;
	}

	if(isLocationHasCitizens(loc.id))
	{
		return;
	}

	int iCitizensQuantity, iModel;
	ref chr;
	int iChar, i, iSex;
	string slai_group, locatorName, sType;

	int iNation = PIRATE;
    	slai_group = GetNationNameByType(iNation)  + "_citizens";
	int iCitizen, iSoldier, iPatrol, iMusketoor, iSoldiers, iGenres, iMechant, iCitizens;
	iSoldiers = sti(PChar.ColonyBuilding.Soldiers);

	iSoldier = (iSoldiers / 2) / 10;
	iPatrol = (iSoldiers / 5) / 10;
	iMusketoor = (iSoldiers / 10) / 10;

	if(iSoldiers <= 0)
	{
		iSoldier = 0;
		iPatrol = 0
		iMusketoor = 0;
	}

	iCitizens = sti(PChar.ColonyBuilding.Population);
	iCitizen = (iCitizens / 2) / 10;
	iGenres = (iCitizens / 10) / 10;
	iMechant = (iCitizens / 10) / 10;


	if(iCitizens <= 0)
	{
		iCitizen = 0;
		iGenres = 0;
		iMechant = 0;
	}

	if(iCitizen > 20) { iCitizen = 20; }

    	arrayNPCModelHow = 0;

    	int iSoldiersNation = sti(PChar.ColonyBuilding.ColonyNation);
    	string sSoldiersType = PChar.ColonyBuilding.SoldiersType;

    	string sSoldierType = "";
	switch(sSoldiersType)
	{
		case "officer": sSoldierType = "pofficer"; break;
		case "pirate": sSoldierType = "pirate"; break;
		case "nation": sSoldierType = "soldier"; break;
	}

	if (checkAttribute(loc, "soldiers") && CheckAttribute(loc, "locators.soldiers"))
	{
		aref solderLoc;
		makearef(st, loc.locators.soldiers);

		if(iSoldier > GetAttributesNum(st))
		{
			iSoldier = GetAttributesNum(st);
		}

		for (i=0; i<iSoldier; i++)
		{
			if(sSoldiersType == "nation")
			{
				iChar = NPC_GeneratePhantomCharacter(sSoldierType, iSoldiersNation, MAN, 0);
			}
			else
			{
				iChar = NPC_GeneratePhantomCharacter(sSoldierType, PIRATE, MAN, 0);
			}

			chr = &characters[iChar];
			SetNPCModelUniq(chr, sSoldierType, MAN);
			chr.City = "Caiman";
            		chr.CityType = "soldier";
			chr.RebirthPhantom = true;
			chr.nation = PIRATE;
			LAi_CharacterReincarnation(chr, true, true);
			LAi_SetReincarnationRankStep(chr, MOD_SKILL_ENEMY_RATE*3+2);
            		SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*3, true);
			LAi_SetLoginTime(chr, 6.0, 23.0);

			solderLoc = GetAttributeN(st, i);
			locatorName = GetAttributeName(solderLoc);
			ChangeCharacterAddressGroup(chr, pchar.location, "soldiers", locatorName);

			if (findsubstr(locatorName, "protector" , 0) != -1 && iNation != PIRATE)
			{
				chr.protector = true;
			}

			LAi_SetGuardianType(chr);

			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
			chr.greeting = "Gr_questOfficer";

			chr.dialog.filename = "Common_Soldier.c";
			chr.dialog.currentnode = "first time";
			SetFoodToCharacter(chr, 3, 20);
		}
	}

	if(checkAttribute(loc, "soldiers") && CheckAttribute(loc, "locators.patrol"))
	{
		makearef(st, loc.locators.patrol);
		if(iPatrol > 7)
		{
			iPatrol = 7;
		}

		if(iPatrol > GetAttributesNum(st) - 3)
		{
			iPatrol = GetAttributesNum(st) - 3;
		}

		for (i=0; i<iPatrol-3; i++)
		{
			if(sSoldiersType == "nation")
			{
				iChar = NPC_GeneratePhantomCharacter(sSoldierType, iSoldiersNation, MAN, 0);
			}
			else
			{
				iChar = NPC_GeneratePhantomCharacter(sSoldierType, PIRATE, MAN, 0);
			}

			chr = &characters[iChar];
			SetNPCModelUniq(chr, sSoldierType, MAN);
			chr.nation = PIRATE;
			chr.City = "Caiman";
            chr.CityType = "soldier";
            SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*3, true);
			chr.RebirthPhantom = true;
			LAi_CharacterReincarnation(chr, true, true);
			LAi_SetReincarnationRankStep(chr, MOD_SKILL_ENEMY_RATE*3+2);
			LAi_SetLoginTime(chr, 0.0, 24.0);
			LAi_SetPatrolType(chr);

			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
			chr.greeting = "Gr_questOfficer";
			PlaceCharacter(chr, "patrol", "random_free");

			chr.dialog.filename = "Common_Soldier.c";
			chr.dialog.currentnode = "first time";
		}

		if(iMusketoor > 3)
		{
			iMusketoor = 3;
		}

		for (i=1; i<=iMusketoor; i++)
		{
			if(sSoldiersType != "nation")
			{
                sType = "mushketer_" + (rand(4)+1);
			}
			else
			{
                sType = NationShortName(iSoldiersNation) + "_rifle_" + i;
			}

			chr = GetCharacter(NPC_GenerateMusketeer("GenChar_", sType, "man", sti(pchar.rank), PIRATE, 0, false));
			chr.id = "GenChar_" + chr.index;
			chr.reputation = 10 + rand(80);
			chr.City = "Caiman";
            chr.CityType = "soldier";
			chr.greeting = "soldier_common";
			chr.nation = PIRATE;
			chr.RebirthPhantom = true;
			LAi_CharacterReincarnation(chr, true, true);
			LAi_SetReincarnationRankStep(chr, MOD_SKILL_ENEMY_RATE*3+2);
            SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*3, true);
			LAi_SetLoginTime(chr, 6.0, 23.0);

			PlaceCharacter(chr, "patrol", "random_free");

			LAi_SetPatrolType(chr);

			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
			chr.greeting = "Gr_questOfficer";

			chr.dialog.filename = "Common_Soldier.c";
			chr.dialog.currentnode = "first time";
			SetFoodToCharacter(chr, 3, 20);
		}
	}

	for(i=0; i<iCitizen; i++)
	{
		iSex = rand(3);
		if(iSex > 1) iSex = 1;

		iChar = NPC_GeneratePhantomCharacter("citizen", iNation, iSex, 0);

		chr = &characters[iChar];
		sType = "citizen";
		SetNPCModelUniq(chr, sType, iSex);

		chr.City = "Caiman";
		chr.CityType = "citizen";

		LAi_SetLoginTime(chr, 6.0, 21.99);
		LAi_SetCitizenType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);

		PlaceCharacter(chr, "goto", "random_free");
		chr.dialog.filename    = "Common_citizen.c";
		chr.dialog.currentnode = "First time";
		if(chr.sex == "man")
		{
			if(rand(1) == 1)
			{
				chr.greeting = "cit_common1";
			}
			else
			{
				chr.greeting = "cit_common";
			}
		}
		else
		{
			chr.greeting = "Gr_Woman_Citizen";
		}
	}

	aref st;
	if (checkAttribute(loc, "locators.merchant"))
	{
		makearef(st, loc.locators.merchant);

		if(iMechant > GetAttributesNum(st))
		{
			iMechant = GetAttributesNum(st);
		}

		for (i=0; i<iMechant; i++)
		{
			iSex = rand(1);
			iChar = NPC_GeneratePhantomCharacter("citizen", iNation, iSex, 0);

			chr = &characters[iChar];
			SetNPCModelUniq(chr, "citizen", iSex);

			chr.City = "Caiman";
			chr.CityType = "citizen";
			LAi_SetLoginTime(chr, 7.0, 20.0);

			ChangeCharacterAddressGroup(chr, loc.id, "merchant", "merchant"+(i+1));
			LAi_SetMerchantType(chr);
			chr.dialog.filename = "Common_ItemTrader.c";
			chr.dialog.currentnode = "first time";
			//-->
			if (chr.sex == "man") { chr.greeting = "tra_common"; } else { chr.greeting = "tra_woman_common"; }
			//<--
			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
		}
	}

	if(CheckAttribute(loc, "carrier") && IsLoginTime())
	{
		int iTemp;
		int iQtyCarrier = rand(2) + 2;
		string locReload[7];
		locReload[0] = "reload4_back";
		locReload[1] = "reload5_back";
		locReload[2] = "reload6_back";
		locReload[3] = "reload7_back";
		locReload[4] = "reload8_back";
		locReload[5] = "reload10_back";
		locReload[6] = "reload9_back";
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

		if(iGenres > 9)
		{
			iGenres = 9;
		}

		for (i=iGenres; i>0; i--)
		{
			iSex = 1;
			while (iSex == 1)
			{
				iTemp = rand(9);
				if (CarrierName[iTemp] != "") iSex = 0;
			}
			chr = GetCharacter(NPC_GenerateCharacter("Carrier", CarrierName[iTemp], "man", "genres", 35, PIRATE, 0, false));
			chr.id = chr.id + "_" + chr.index;
			aref aLocator;
			makearef(aLocator, loc.locators.reload);
			CarrierName[iTemp] = "";
			LAi_SetImmortal(chr, true);
			LAi_SetLoginTime(chr, 7.0, 20.0);
			iSex = 1;
			while (iSex == 1)
			{
				iTemp = rand(6);
				if (CheckAttribute(aLocator, locReload[iTemp]) && locReload[iTemp] != "")
				{
					chr.firstLoc = locReload[iTemp];
					chr.firstLoc.timer = frand(7) + 4.0;
					locReload[iTemp] = "";
					iSex = 0;
				}
			}
			ChangeCharacterAddressGroup(chr, loc.id, "reload", "gate");
			LAi_SetCarrierType(chr);
			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
		}
	}
}

void CreateCaimanGuardingBase(aref loc)
{
	if(PChar.ColonyBuilding.SlavesInShore.CurShore == "LandGuardingPort")
	{
		return;
	}

	if(!CheckAttribute(loc, "id") || !CheckAttribute(loc, "type"))
	{
		return;
	}

	if(loc.id != "LandGuardingFort" && loc.id != "LandGuardingPort")
	{
		return;
	}

	int iSoldiers = sti(PChar.ColonyBuilding.Soldiers);

	if(iSoldiers <= 0)
	{
		return;
	}

	if(isLocationHasCitizens(loc.id))
	{
		return;
	}

	int iSoldier = (iSoldiers / 2) / 10;
	int iPatrol = (iSoldiers / 5) / 10;

	if(loc.id == "LandGuardingFort")
	{
		if(iSoldier > 15) iSoldier = 15;
	}
	else
	{
		if(iSoldier > 10) iSoldier = 10;
	}

	if(iPatrol > 10) iPatrol = 10;

	int iNation = PIRATE;

	ref chr;
	int iChar, i;
	string sType;
	arrayNPCModelHow = 0;

    	int iSoldiersNation = sti(PChar.ColonyBuilding.ColonyNation);
    	string sSoldiersType = PChar.ColonyBuilding.SoldiersType;

	switch(sSoldiersType)
	{
		case "officer": sType = "pofficer"; break;
		case "pirate": sType = "pirate"; break;
		case "nation": sType = "soldier"; break;
	}

	for (i=0; i<iSoldier; i++)
	{
		if(sSoldiersType == "nation")
		{
			iChar = NPC_GeneratePhantomCharacter(sType, iSoldiersNation, MAN, 0);
		}
		else
		{
			iChar = NPC_GeneratePhantomCharacter(sType, PIRATE, MAN, 0);
		}

		chr = &characters[iChar];
		SetNPCModelUniq(chr, sType, MAN);
		chr.City = "Caiman";
		chr.CityType = "soldier";
		chr.nation = PIRATE;
		chr.RebirthPhantom = true;
		LAi_CharacterReincarnation(chr, true, true);
		LAi_SetReincarnationRankStep(chr, MOD_SKILL_ENEMY_RATE*3+2);
		SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*3, true);
		LAi_SetLoginTime(chr, 0.0, 24.0);
		LAi_SetGuardianType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
		chr.dialog.filename = "Common_Fort.c";
		chr.dialog.currentnode = "First time";
		chr.greeting = "Gr_questOfficer";
		SetFoodToCharacter(chr, 3, 20);
		PlaceCharacter(chr, "goto", "random_free");
	}

	for (i=0; i<iPatrol; i++)
	{
		if(sSoldiersType == "nation")
		{
			iChar = NPC_GeneratePhantomCharacter(sType, iSoldiersNation, MAN, 0);
		}
		else
		{
			iChar = NPC_GeneratePhantomCharacter(sType, PIRATE, MAN, 0);
		}

		chr = &characters[iChar];
		SetNPCModelUniq(chr, sType, MAN);
		chr.City = "Caiman";
		chr.CityType = "fortPatrol";
		chr.nation = PIRATE;
		LAi_SetPatrolType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
		chr.greeting = "Gr_questOfficer";
		chr.dialog.filename = "Common_Fort.c";
		chr.dialog.currentnode = "First time";
		PlaceCharacter(chr, "goto", "random_free");
	}
}

void CreateCaimanMinesCitizens(aref loc)
{
	if(!CheckAttribute(loc, "id") || !CheckAttribute(loc, "type"))
	{
		return;
	}

	if(loc.id != "Caiman_Mines")
	{
		return;
	}

	if(PChar.Mines.Defence.Battle == true)
	{
		DeleteAllFantomCharactersFromLocation(loc);
		return;
	}

	if(PChar.Mines.Defence.DeadInTown == true)
	{
		SetDeadmansToLocation(loc, "pirate");
		PChar.Mines.Defence.DeadInTown = false;
		return;
	}

	bool bBuildingGoldMine = PChar.ColonyBuilding.GoldMine.Resize.BuildingTime;
	bool bBuildingSilverMine = PChar.ColonyBuilding.SilverMine.Resize.BuildingTime;
	bool bBuildingIronMine = PChar.ColonyBuilding.IronMine.Resize.BuildingTime;

	if(bBuildingGoldMine && bBuildingSilverMine && bBuildingIronMine)
	{
		DeleteAllFantomCharactersFromLocation(loc);
		return;
	}

	if(isLocationHasCitizens(loc.id))
	{
		return;
	}

	int iSoldiers = sti(PChar.Mines.GuardiansQuantity);
	int iSlaves = sti(PChar.Mines.SlavesQuantity);

	int iPoorSlaves = iSlaves / 15;
	if(iPoorSlaves > 15) iPoorSlaves = 15;
	if(iPoorSlaves < 0) iPoorSlaves = 0;

	int iStaySlaves = iSlaves / 10;
	if(iStaySlaves > 15) iStaySlaves = 15;
	if(iStaySlaves < 0) iStaySlaves = 0;

	int iPatrol = iSoldiers / 10;
	if(iPatrol > 25) iPatrol = 25;
	if(iPatrol < 0) iPatrol = 0;

	int iNation = PIRATE;
	int iMassive;

	ref chr;
	int iChar, i;
	string sType;
	arrayNPCModelHow = 0;

    	int iSoldiersNation = sti(PChar.ColonyBuilding.ColonyNation);
    	string sSoldiersType = PChar.ColonyBuilding.SoldiersType;

	switch(sSoldiersType)
	{
		case "officer": sType = "pofficer"; break;
		case "pirate": sType = "pirate"; break;
		case "nation": sType = "soldier"; break;
	}

	if(iPatrol > 0)
	{
		for (i=0; i<=iPatrol; i++)
		{
			if(sSoldiersType == "nation")
			{
				iChar = NPC_GeneratePhantomCharacter(sType, iSoldiersNation, MAN, 0);
			}
			else
			{
				iChar = NPC_GeneratePhantomCharacter(sType, PIRATE, MAN, 0);
			}

			chr = &characters[iChar];
			SetNPCModelUniq(chr, sType, MAN);

			chr.City = "Caiman";
			chr.CityType = "soldier";
			chr.nation = PIRATE;
			SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*3, true);
			chr.RebirthPhantom = true;
			LAi_CharacterReincarnation(chr, true, true);
			LAi_SetReincarnationRankStep(chr, MOD_SKILL_ENEMY_RATE*3+2);
			LAi_SetLoginTime(chr, 0.0, 24.0);
			PlaceCharacter(chr, "goto", "random_free");
			LAi_SetPatrolType(chr);
			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
			chr.greeting = "Gr_questOfficer";
			chr.dialog.filename = "Common_Soldier.c";
			chr.dialog.currentnode = "first time";
		}
	}

	string sAni;

	if(iPoorSlaves > 0)
	{

		for(int s=1; s <= iPoorSlaves; s++)
		{
	    	iMassive = rand(22);

        	sAni = "man";

			chr = GetCharacter(NPC_GenerateCharacter("Caiman_Mines_Poor_Slave_" + s, GetSlaveModel(iMassive), "man", sAni, 7, PIRATE, 0, false));
			chr.dialog.filename = "Quest\CapBloodLine\questNPC.c";
			chr.greeting = "Gr_slave";
			chr.CityType = "citizen";
			chr.city = "Caiman";
			chr.PoorSit = true;
			LAi_SetLoginTime(chr, 6.0, 22.99);

			PlaceCharacter(chr, "slaves_poor", "random_free");
			LAi_SetGroundSitType(chr);
			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
		}
	}

	if(iStaySlaves > 0)
	{
		for(int n=1; n <= iStaySlaves; n++)
		{
			iMassive = rand(22);

	        sAni = "man";

			chr = GetCharacter(NPC_GenerateCharacter("Caiman_Mines_Stay_Slave_" + n, GetSlaveModel(iMassive), "man", sAni, 7, PIRATE, 0, false));
			chr.dialog.filename = "Quest\CapBloodLine\questNPC.c";
			chr.greeting = "Gr_slave";
			chr.CityType = "citizen";
			chr.city = "Caiman";
			LAi_SetLoginTime(chr, 6.0, 22.99);

			PlaceCharacter(chr, "goto", "random_free");
			LAi_SetCitizenType(chr);
			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
		}
	}
}


void CreateCaimanPlantationCitizens(aref loc)
{
	if(!CheckAttribute(loc, "id") || !CheckAttribute(loc, "type"))
	{
		return;
	}

	if(loc.id != "Caiman_Plantation")
	{
		return;
	}

	if(PChar.Plantation.Defence.Battle == true || PChar.ColonyBuilding.Defence.Battle == true)
	{
		DeleteAllFantomCharactersFromLocation(loc);
		return;
	}

	if(PChar.Plantation.Defence.DeadInTown == true)
	{
		SetDeadmansToLocation(loc, "pirate");
		PChar.Plantation.Defence.DeadInTown = false;
		return;
	}

	if(PChar.Plantation.Size.Resize.BuildingTime == true)
	{
		DeleteAllFantomCharactersFromLocation(loc);
		return;
	}

	if(isLocationHasCitizens(loc.id))
	{
		return;
	}

	int iSoldiers = sti(PChar.Plantation.Guardians.Quantity);
	int iSlaves = sti(PChar.Plantation.Slaves.Quantity);

	int iStaySlaves = iSlaves / 10;
	if(iStaySlaves > 20) iStaySlaves = 20;
	if(iStaySlaves < 0) iStaySlaves = 0;

	int iPatrol = iSoldiers / 10;
	if(iPatrol > 20) iPatrol = 20;
	if(iPatrol < 0) iPatrol = 0;

	int iNation = sti(PChar.ColonyBuilding.ColonyNation);
	int iMassive;

	ref chr;
	int iChar, i;
	string sType;
	arrayNPCModelHow = 0;

	SaveCurrentNpcQuestDateParam(loc, "Caiman_Plantation_date");

    	int iSoldiersNation = sti(PChar.ColonyBuilding.ColonyNation);
    	string sSoldiersType = PChar.ColonyBuilding.SoldiersType;

	switch(sSoldiersType)
	{
		case "officer": sType = "pofficer"; break;
		case "pirate": sType = "pirate"; break;
		case "nation": sType = "soldier"; break;
	}

	if(iPatrol > 0)
	{
		for(i=0; i<=iPatrol; i++)
		{
			if(sSoldiersType == "nation")
			{
				iChar = NPC_GeneratePhantomCharacter(sType, iSoldiersNation, MAN, 0);
			}
			else
			{
				iChar = NPC_GeneratePhantomCharacter(sType, PIRATE, MAN, 0);
			}

			chr = &characters[iChar];
			SetNPCModelUniq(chr, sType, MAN);

			chr.City = "Caiman";
			chr.CityType = "soldier";
			chr.nation = PIRATE;
			SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*3, true);
			chr.RebirthPhantom = true;
			LAi_CharacterReincarnation(chr, true, true);
			LAi_SetReincarnationRankStep(chr, MOD_SKILL_ENEMY_RATE*3+2);
			LAi_SetLoginTime(chr, 0.0, 24.0);
			PlaceCharacter(chr, "goto", "random_free");
			LAi_SetPatrolType(chr);
			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
			chr.greeting = "Gr_questOfficer";
			chr.dialog.filename = "Common_Soldier.c";
			chr.dialog.currentnode = "first time";
		}
	}


	if(iStaySlaves > 0)
	{
		for(int n=1; n <= iStaySlaves; n++)
		{
	        iMassive = rand(22);

	        string sAni = "man";

			chr = GetCharacter(NPC_GenerateCharacter("Caiman_Plantation_Stay_Slave_" + n, GetSlaveModel(iMassive), "man", sAni, 7, PIRATE, 0, false));
			chr.dialog.filename = "Quest\CapBloodLine\questNPC.c";
			chr.greeting = "Gr_slave";
			chr.CityType = "citizen";
			chr.city = "Caiman";
			LAi_SetLoginTime(chr, 6.0, 22.99);

			PlaceCharacter(chr, "patrol", "random_free");
			LAi_SetCitizenType(chr);
			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
		}
	}
}

//-------------------------------------------------------------------------------
// Стройка - куча перенаправлений
//-------------------------------------------------------------------------------
void CreateSmallColonyCaiman()
{
	ref PChar = GetMainCharacter();
	PChar.ColonyBuilding.Stage = "1";
	LAi_QuestDelay("LiveOfTheColonyUptade_1", 0.5);
	LAi_QuestDelay("CheckDefenceColonyUptade_1", 0.5);

	PChar.ColonyBuilding.Food.MaxQuantity = 5000;
	PChar.ColonyBuilding.Medicament.MaxQuantity = 2500;

	Locations[FindLocation("Caiman_town")].reload.l2.disable = true;
	Locations[FindLocation("Caiman_town")].reload.l4.disable = true;
	Locations[FindLocation("Caiman_town")].reload.l5.disable = true;
	Locations[FindLocation("Caiman_town")].reload.l6.disable = true;
	Locations[FindLocation("Caiman_town")].reload.l7.disable = true;
	Locations[FindLocation("Caiman_town")].reload.l8.disable = true;
	Locations[FindLocation("Caiman_town")].reload.l10.disable = true;
	Locations[FindLocation("Caiman_town")].reload.l11.disable = true;

	Locations[FindLocation("Shore17")].type = "port";
	Locations[FindLocation("Shore17")].fastreload = "Caiman";
	Locations[FindLocation("Shore17")].reload.l1.go = "Caiman_ExitTown";
	Locations[FindLocation("Shore17")].reload.l1.label = "ExitTown";
	Locations[FindLocation("Shore17")].reload.l1.emerge = "reload2";
	Locations[FindLocation("Shore17")].reload.l1.autoreload = "1";

	Locations[FindLocation("Caiman_Jungle_01")].reload.l1.name = "reload1_back";
	Locations[FindLocation("Caiman_Jungle_01")].reload.l1.go = "Caiman_ExitTown";
	Locations[FindLocation("Caiman_Jungle_01")].reload.l1.emerge = "reload1";
	Locations[FindLocation("Caiman_Jungle_01")].reload.l1.autoreload = "1";
	Locations[FindLocation("Caiman_Jungle_01")].reload.l1.label = "ExitTown";

	Islands[FindIsland("Caiman")].reload.l1.label = "Caiman Port";
	Islands[FindIsland("Caiman")].colonyquantity = 1;
	Islands[FindIsland("Caiman")].reload.l1.istown = true;

	int iColony = FindColony("Caiman");
	ref rColony = &Colonies[iColony];
	rColony.StoreNum   = Caiman_STORE;
	rColony.smuggling_nation = PIRATE;
	rColony.nation = PIRATE;
	rColony.island = "Caiman";
	rColony.num = 1;
	rColony.HeroOwn = true;
	rColony.isBought = true;
	rColony.DontSetShipInPort = true;

	RenamedColonyCaiman();

	AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_80", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_81", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_82", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_83", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + " " + GetSexPhrase(GetConvertStrWithReplace("Variable_Colony_84", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Colony_85", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_86", "AddRumours.txt", "#space#", " "), 5, 1);
}

void CreateModificyColonyCaiman()
{
	ref PChar = GetMainCharacter();
	PChar.ColonyBuilding.Stage = "2";
	PChar.ColonyBuilding.Population.Max = sti(PChar.ColonyBuilding.Population.Max) * 2 + rand(25);
	PChar.ColonyBuilding.Population.Optimal = (sti(PChar.ColonyBuilding.Population.Max) - (75 + rand(15))) - rand(25);
	PChar.ColonyBuilding.Soldiers.MaxQuantity = 2000;

	PChar.ColonyBuilding.Food.MaxQuantity = 10000;

	// МАГАЗИН
	Characters[GetCharacterIndex("Caiman_trader")].from_sea = "Caiman_town";

	ref rStore = &Locations[FindLocation("Caiman_store")];
	DeleteAttribute(rStore, "models");

	int iStore = sti(PChar.ColonyBuilding.Architecture.Store);
	string sStore = "Store0" + iStore;

	rStore.filespath.models = "locations\inside\" + sStore + "\";
	rStore.models.always.locators = sStore + "_locators";
	rStore.models.always.store = sStore;
	rStore.models.always.store.level = 65538;
	rStore.models.always.window = sStore + "_window";
	rStore.models.always.window.tech = "LocationWindows";
	rStore.models.always.window.level = 65539;
	rStore.models.always.back = "..\inside_back";
	rStore.models.always.back.level = 65529;
	rStore.models.day.charactersPatch = sStore + "_patch";
	rStore.models.night.charactersPatch = sStore + "_patch";
	rStore.locators_radius.camdetector.camera1 = 5;

	DeleteAttribute(rStore, "reload");
	rStore.reload.l1.name = "reload1_back";
	rStore.reload.l1.go = "Caiman_town";
	rStore.reload.l1.emerge = "reload6";
	rStore.reload.l1.autoreload = "0";
	rStore.reload.l1.label = "Street";

	// ВЕРФЬ
	int iShipyard = sti(PChar.ColonyBuilding.Architecture.Shipyard);
	string sShipyard = "shipyard0" + iShipyard;

	ref rShipyard = &Locations[FindLocation("Caiman_shipyard")];
	DeleteAttribute(rShipyard, "models");

	rShipyard.filespath.models = "locations\inside\" + sShipyard + "\";
	rShipyard.models.always.locators = sShipyard + "_locators";
	rShipyard.models.always.shipyard = sShipyard;
	rShipyard.models.always.shipyard.level = 65538;
	rShipyard.models.always.window = sShipyard + "_window";
	rShipyard.models.always.window.tech = "LocationWindows";
	rShipyard.models.always.window.level = 65539;
	rShipyard.models.always.back = "..\inside_back";
	rShipyard.models.always.back.level = 65529;
	rShipyard.models.day.charactersPatch = sShipyard + "_patch";
	rShipyard.models.night.charactersPatch = sShipyard + "_patch";

	// ТАВЕРНА
	int iTavern = sti(PChar.ColonyBuilding.Architecture.Tavern);
	string sTavern = "tavern0" + iTavern;

	ref rTavern = &Locations[FindLocation("Caiman_tavern")];
	DeleteAttribute(rTavern, "models");

	rTavern.filespath.models = "locations\inside\" + sTavern + "\";
	rTavern.models.always.tavern = sTavern;
	rTavern.models.always.tavern.level = 65538;
	rTavern.models.always.locators = sTavern + "_locators";

	if(iTavern == 1)
	{
		rTavern.models.always.window = "tavern01_windows";
	}

	rTavern.models.always.window.tech = "LocationWindows";
	rTavern.models.always.window.level = 65539;
	rTavern.models.always.back = "..\inside_back";
	rTavern.models.always.back.level = 65529;
	rTavern.models.day.charactersPatch = sTavern + "_patch";
	rTavern.models.night.charactersPatch = sTavern + "_patch";

	DeleteAttribute(rTavern, "reload");
	rTavern.reload.l1.name = "reload1_back";
	rTavern.reload.l1.go = "Caiman_town";
	rTavern.reload.l1.emerge = "reload4";
	rTavern.reload.l1.autoreload = "0";
	rTavern.reload.l1.label = "Street";

	rTavern.reload.l2.name = "reload2_back";
	rTavern.reload.l2.go = "Caiman_tavern_upstairs";
	rTavern.reload.l2.emerge = "reload1";
	rTavern.reload.l2.autoreload = "0";
	rTavern.reload.l2.label = "Tavern upstairs";
	rTavern.reload.l2.disable = 1;
	// ДОМ РОСТОВЩИКА
	int iBank = sti(PChar.ColonyBuilding.Architecture.Bank);
	string sBank = "Bank0" + iBank;

	ref rBank = &Locations[FindLocation("Caiman_Bank")];
	DeleteAttribute(rBank, "models");

	rBank.filespath.models = "locations\inside\" + sBank + "\";
	rBank.models.always.locators = sBank + "_locators";
	rBank.models.always.store = sBank;
	rBank.models.always.store.level = 65538;
	rBank.models.always.window = sBank + "_windows";
	rBank.models.always.window.tech = "LocationWindows";
	rBank.models.always.window.level = 65539;
	rBank.models.always.back = "..\inside_back";
	rBank.models.always.back.level = 65529;
	rBank.models.day.charactersPatch = sBank + "_patch";
	rBank.models.night.charactersPatch = sBank + "_patch";

	// РЕЗИДЕНЦИЯ
	int iTownhall = sti(PChar.ColonyBuilding.Architecture.Townhall);
	string sTownhall = "Residence0" + iTownhall;

	ref rResidence = &Locations[FindLocation("Caiman_townhall")];
	DeleteAttribute(rResidence, "models");

	rResidence.image = "loading\jonny_load\inside\residence03.dds";
	rResidence.filespath.models = "locations\inside\" + sTownhall;
	rResidence.models.always.locators = sTownhall + "_locators";
	rResidence.models.always.l1 = sTownhall;
	rResidence.models.always.l1.level = 65538;
	rResidence.models.always.window = sTownhall + "_window";
	rResidence.models.always.window.tech = "LocationWindows";
	rResidence.models.always.window.level = 65539;
	rResidence.models.always.back = "..\inside_back";
	rResidence.models.always.back.level = 65529;
	rResidence.models.day.charactersPatch = sTownhall + "_patch";
	rResidence.models.night.charactersPatch = sTownhall + "_patch";

	DeleteAttribute(rResidence, "reload");
	rResidence.reload.l1.name = "reload1_back";
	rResidence.reload.l1.go = "Caiman_town";
	rResidence.reload.l1.emerge = "reload3";
	rResidence.reload.l1.autoreload = "0";
	rResidence.reload.l1.label = "Street";

	// СКЛАД

	ref rStoreHouse = &Locations[FindLocation("Caiman_StoreHouse")];
	DeleteAttribute(rStoreHouse, "rStoreHouse");

	string sShoreHouseEmerge = "houseS1";
	int iTown = sti(PChar.ColonyBuilding.Architecture.Town);
	switch(iTown)
	{
		case 1: sShoreHouseEmerge = "houseS1"; break;
		case 2: sShoreHouseEmerge = "houseS6"; break;
		case 3: sShoreHouseEmerge = "houseS2"; break;
	}

	rStoreHouse.reload.l1.name = "reload1";
	rStoreHouse.reload.l1.go = "Caiman_town";
	rStoreHouse.reload.l1.emerge = sShoreHouseEmerge;
	rStoreHouse.reload.l1.autoreload = "0";
	rStoreHouse.reload.l1.label = "Street";

	// ГОРОД
	CreateTownCaiman();

	// ВЫХОД ИЗ ГОРОДА
	ref rExitTown = &Locations[FindLocation("Caiman_ExitTown")];
	DeleteAttribute(rExitTown, "models");

	rExitTown.type = "jungle";
	rExitTown.islandId = "Caiman";
	rExitTown.islandIdAreal = "Caiman";

	rExitTown.filespath.models = "locations\Outside\Jungles\TownExitY";
	rExitTown.models.always.townExit = "townExitY";
	rExitTown.models.always.townWalls = "townExitY_stucco";
	rExitTown.models.always.locators = "townExitY_locators";
	rExitTown.models.always.grassPatch = "townExitY_grass";
	rExitTown.models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	rExitTown.models.always.l1 = "plan1";
	rExitTown.models.always.l1.level = 9;
	rExitTown.models.always.l1.tech = "DLightModel";
	rExitTown.models.always.l2 = "plan2";
	rExitTown.models.always.l2.level = 8;
	rExitTown.models.always.l2.tech = "DLightModel";
	rExitTown.models.always.l3 = "plan3";
	rExitTown.models.always.l3.level = 7;
	rExitTown.models.always.l3.tech = "DLightModel";
	rExitTown.models.day.charactersPatch = "townExitY_patch";
	rExitTown.models.day.fonars = "townExitY_fd";
	rExitTown.models.night.charactersPatch = "townExitY_patch";
	rExitTown.models.night.fonars = "townExitY_fn";

	rExitTown.environment.weather = "true";
	rExitTown.environment.sea = "false";

	rExitTown.reload.l1.name = "reload3";
	rExitTown.reload.l1.go = "Caiman_town";
	rExitTown.reload.l1.emerge = "gate_back";
	rExitTown.reload.l1.autoreload = "0";
	rExitTown.reload.l1.label = "Caiman";

	rExitTown.reload.l2.name = "reload1_back";
	rExitTown.reload.l2.go = "Caiman_CaveEntrance";
	rExitTown.reload.l2.emerge = "reload2";
	rExitTown.reload.l2.autoreload = "1";
	rExitTown.reload.l2.label = "Jungle";
	rExitTown.locators_radius.reload.reload1_back = 2.0;

	rExitTown.reload.l3.name = "reload2_back";
	rExitTown.reload.l3.go = "Shore16";
	rExitTown.reload.l3.emerge = "reload1";
	rExitTown.reload.l3.autoreload = "1";
	rExitTown.reload.l3.label = "Shore16";
	rExitTown.locators_radius.reload.reload2_back = 2.0;
	rExitTown.locators_radius.quest.detector1 = 2.0;

	Islands[FindIsland("Caiman")].reload.l1.label = "Caiman Port";
	Islands[FindIsland("Caiman")].reload.l1.name = "reload_2";
	Islands[FindIsland("Caiman")].reload.l1.go = "Caiman_town";
	Islands[FindIsland("Caiman")].reload.l1.emerge = "reload1";
	Islands[FindIsland("Caiman")].reload.l1.radius = 600.0;
	Islands[FindIsland("Caiman")].reload.l1.pic = 0;
	Islands[FindIsland("Caiman")].reload.l1.tex = "t1";
	Islands[FindIsland("Caiman")].reload.l1.istown = true;

	ref rShore = &Locations[FindLocation("Shore16")];
	rShore.reload.l1.go = "Caiman_ExitTown";
	rShore.reload.l1.emerge = "reload2";
	rShore.reload.l1.autoreload = "1";
	rShore.reload.l1.label = "ExitTown";

	ref rCaveEntrance = &Locations[FindLocation("Caiman_CaveEntrance")];
	rCaveEntrance.reload.l2.go = "Caiman_ExitTown";
	rCaveEntrance.reload.l2.emerge = "reload1";
	rCaveEntrance.reload.l2.autoreload = "1";
	rCaveEntrance.reload.l2.label = "ExitTown";

	ref rJungle = &Locations[FindLocation("Caiman_Jungle_01")];
	rJungle.reload.l1.name = "reload1_back";
	rJungle.reload.l1.go = "Caiman_Plantation";
	rJungle.reload.l1.emerge = "reload2";
	rJungle.reload.l1.autoreload = "1";
	rJungle.reload.l1.label = "Plantation";

	rJungle.reload.l2.name = "reload2_back";
	rJungle.reload.l2.go = "Shore16";
	rJungle.reload.l2.emerge = "reload1";
	rJungle.reload.l2.autoreload = "1";
	rJungle.reload.l2.label = "Shore16";

	rJungle.reload.l3.name = "reload3_back";
	rJungle.reload.l3.go = "Caiman_CaveEntrance";
	rJungle.reload.l3.emerge = "reload2";
	rJungle.reload.l3.autoreload = "1";
	rJungle.reload.l3.label = "Jungle";

	Stores[Caiman_STORE].StoreSize = "large";

	AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_87", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_88", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_89", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_90", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + " "+ GetSexPhrase(GetConvertStrWithReplace("Variable_Colony_91", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Colony_92", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Colony_93", "AddRumours.txt", "#space#", " ")+PChar.ColonyBuilding.ColonyName+"'.", 5, 1);
}

void CreatePlantationInCaiman()
{
	LAi_QuestDelay("PlantationUptade1", 1.0);

	PChar.ColonyBuilding.Plantation = true;

	ref rTown = &Locations[FindLocation("Caiman_town")];
	rTown.reload.gate.name = "gate_back";
	rTown.reload.gate.go = "Caiman_Plantation";
	rTown.reload.gate.emerge = "reload1";
	rTown.reload.gate.autoreload = "0";
	rTown.reload.gate.label = "Caiman Plantation";

	ref rJungle = &Locations[FindLocation("Caiman_Jungle_01")];
	rJungle.reload.l1.name = "reload1_back";
	rJungle.reload.l1.go = "Caiman_Plantation";
	rJungle.reload.l1.emerge = "reload2";
	rJungle.reload.l1.autoreload = "1";
	rJungle.reload.l1.label = "Caiman Plantation";

	if(PChar.ColonyBuilding.Mines == false)
	{
		ref rCaveEntrance = &Locations[FindLocation("Caiman_CaveEntrance")];
		rCaveEntrance.reload.l2.go = "Caiman_Jungle_01";
		rCaveEntrance.reload.l2.emerge = "reload3";
		rCaveEntrance.reload.l2.autoreload = "1";
		rCaveEntrance.reload.l2.label = "Jungle";
	}
	else
	{
		rJungle.reload.l3.name = "reload3_back";
		rJungle.reload.l3.go = "Caiman_ExitMines";
		rJungle.reload.l3.emerge = "reload2";
		rJungle.reload.l3.autoreload = "1";
		rJungle.reload.l3.label = "Caiman Mines Exit";

		ref rExitMines = &Locations[FindLocation("Caiman_ExitMines")];
		rExitMines.reload.l2.name = "reload2_back";
		rExitMines.reload.l2.go = "Caiman_Jungle_01";
		rExitMines.reload.l2.emerge = "reload3";
		rExitMines.reload.l2.autoreload = "1";
		rExitMines.reload.l2.label = "Jungle";
	}

	if(PChar.ColonyBuilding.Fort == false)
	{
		ref rShore = &Locations[FindLocation("Shore16")];
		rShore.reload.l1.go = "Caiman_Jungle_01";
		rShore.reload.l1.emerge = "reload2";
		rShore.reload.l1.autoreload = "1";
		rShore.reload.l1.label = "Jungle";
	}
	else
	{
		rJungle.reload.l2.name = "reload2_back";
		rJungle.reload.l2.go = "LandGuardingPort";
		rJungle.reload.l2.emerge = "reload1";
		rJungle.reload.l2.autoreload = "1";
		rJungle.reload.l2.label = "Land Guarding Port";

		ref rBase = &Locations[FindLocation("LandGuardingPort")];
		rBase.reload.l1.name = "reload1_back";
		rBase.reload.l1.go = "Caiman_Jungle_01";
		rBase.reload.l1.emerge = "reload2";
		rBase.reload.l1.autoreload = "1";
		rBase.reload.l1.label = "Jungle";
	}

	AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_94", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_95", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_96", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_97", "AddRumours.txt", "#space#", " ")+PChar.ColonyBuilding.ColonyName+GetConvertStrWithReplace("Variable_Colony_98", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + GetConvertStrWithReplace("Variable_Colony_99", "AddRumours.txt", "#space#", " "), 5, 1);
}

void CreateMinesInCaiman()
{
	LAi_QuestDelay("MinesUptade1", 1.0);


	PChar.ColonyBuilding.Mines = true;

	ref rExitTown = &Locations[FindLocation("Caiman_ExitTown")];
	rExitTown.reload.l2.name = "reload1_back";
	rExitTown.reload.l2.go = "Caiman_ExitMines";
	rExitTown.reload.l2.emerge = "reload2";
	rExitTown.reload.l2.autoreload = "1";
	rExitTown.reload.l2.label = "Caiman Mines Exit";
	rExitTown.locators_radius.reload.reload1_back = 2.0;

	ref rCaveEntrance = &Locations[FindLocation("Caiman_CaveEntrance")];
	rCaveEntrance.reload.l2.go = "Caiman_ExitMines";
	rCaveEntrance.reload.l2.emerge = "reload1";
	rCaveEntrance.reload.l2.autoreload = "1";
	rCaveEntrance.reload.l2.label = "Caiman Mines Exit";

	if(PChar.ColonyBuilding.Plantation == true && PChar.ColonyBuilding.Plantation.BuildingTime == false)
	{
		ref rJungle = &Locations[FindLocation("Caiman_Jungle_01")];
		rJungle.reload.l3.name = "reload3_back";
		rJungle.reload.l3.go = "Caiman_ExitMines";
		rJungle.reload.l3.emerge = "reload2";
		rJungle.reload.l3.autoreload = "1";
		rJungle.reload.l3.label = "Caiman Mines Exit";

		ref rExitMines = &Locations[FindLocation("Caiman_ExitMines")];
		rExitMines.reload.l2.name = "reload2_back";
		rExitMines.reload.l2.go = "Caiman_Jungle_01";
		rExitMines.reload.l2.emerge = "reload3";
		rExitMines.reload.l2.autoreload = "1";
		rExitMines.reload.l2.label = "Jungle";
	}

	AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_100", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_101", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_102", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_103", "AddRumours.txt", "#space#", " ")+PChar.ColonyBuilding.ColonyName+GetConvertStrWithReplace("Variable_Colony_104", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + GetConvertStrWithReplace("Variable_Colony_105", "AddRumours.txt", "#space#", " "), 5, 1);
}

void CreateFortInCaiman()
{
	PChar.ColonyBuilding.Food.MaxQuantity = 12500;

	PChar.ColonyBuilding.Fort = true;
	PChar.ColonyBuilding.Stage = "3";
	PChar.ColonyBuilding.Soldiers.MaxQuantity = 2500;

	if(PChar.ColonyBuilding.Plantation == true)
	{
		ref rBase = &Locations[FindLocation("LandGuardingPort")];
		rBase.reload.l1.name = "reload1_back";
		rBase.reload.l1.go = "Caiman_Jungle_01";
		rBase.reload.l1.emerge = "reload2";
		rBase.reload.l1.autoreload = "1";
		rBase.reload.l1.label = "Jungle";

		ref rJungle = &Locations[FindLocation("Caiman_Jungle_01")];
		rJungle.reload.l2.name = "reload2_back";
		rJungle.reload.l2.go = "LandGuardingPort";
		rJungle.reload.l2.emerge = "reload1";
		rJungle.reload.l2.autoreload = "1";
		rJungle.reload.l2.label = "Land Guarding Port";
	}
	else
	{
		ref rExitTown = &Locations[FindLocation("Caiman_ExitTown")];
		rExitTown.reload.l3.name = "reload2_back";
		rExitTown.reload.l3.go = "LandGuardingPort";
		rExitTown.reload.l3.emerge = "reload1";
		rExitTown.reload.l3.autoreload = "1";
		rExitTown.reload.l3.label = "Land Guarding Port";
	}

	Islands[FindIsland("Caiman")].reload.l2.label = "Land Guarding Port";
	Islands[FindIsland("Caiman")].reload.l2.name = "reload_1";
	Islands[FindIsland("Caiman")].reload.l2.go = "LandGuardingPort";
	Islands[FindIsland("Caiman")].reload.l2.emerge = "sea";
	Islands[FindIsland("Caiman")].reload.l2.radius = 600.0;
	Islands[FindIsland("Caiman")].reload.l2.pic = 0;
	Islands[FindIsland("Caiman")].reload.l2.tex = "t1";

	int idLngFile = LanguageOpenFile("LocLables.txt");
	worldMap.labels.Shore16.text = CheckingTranslate(idLngFile,"Land Guarding Port");
	LanguageCloseFile(idLngFile);

	AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_106", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_107", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_108", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_109", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + GetConvertStrWithReplace("Variable_Colony_110", "AddRumours.txt", "#space#", " "), 5, 1);
}

void CreateTownCaiman()
{
	ref rTown;
	int n = FindLocation("Caiman_town");
	makeref(rTown, Locations[n]);

	DeleteAttribute(rTown, "models.always");
	DeleteAttribute(rTown, "models.day");
	DeleteAttribute(rTown, "models.night");

	rTown.id.label = "Street";
	rTown.worldmap = "Caiman";
	rTown.image = "loading\jonny_load\city\Caiman_Town.dds";
	rTown.citizens = true;
	rTown.soldiers = true;
	rTown.carrier = true;
	rTown.type = "town";
	LAi_LocationFantomsGen(rTown, false);
	rTown.houseEnc = false;

	int iTown = sti(PChar.ColonyBuilding.Architecture.Town);
	string sTown = "Antigua";

	switch(iTown) { case 1: sTown = "Antigua"; break; case 2: sTown = "Maracaibo"; break; case 3: sTown = "Cartahena"; break; }

	rTown.filespath.models = "locations\Town_" + sTown + "\Town\";
	rTown.models.always.town = sTown;
	rTown.models.always.town.sea_reflection = 1;
	rTown.models.always.L1 = sTown + "_signsA";
	rTown.models.always.seabed =  sTown + "_sb";
	rTown.models.always.locators =  sTown + "_locators";
	rTown.models.always.grassPatch =  sTown + "_grass";
	rTown.models.day.fonar =  sTown + "_fd";
	rTown.models.day.charactersPatch =  sTown + "_patch_day";
	rTown.models.day.rinok =  sTown + "_rinok";
	rTown.models.night.fonar =  sTown + "_fn";
	rTown.models.night.charactersPatch =  sTown + "_patch_night";
	rTown.models.always.plan = "plan1";
	rTown.models.always.plan.level = 9;

	rTown.models.always.MillFan = "MillFan";
	rTown.models.always.MillFan.locator.group = "WindMill";
	rTown.models.always.MillFan.locator.name ="Fan";
	rTown.models.always.MillFan.rotate.x = 0.0;
	rTown.models.always.MillFan.rotate.y = 0.0;
	rTown.models.always.MillFan.rotate.z = 0.3;

	if(sTown == "Maracaibo") { rTown.models.always.MillFan.rotate.z = 0.26; }

	if(sTown == "Maracaibo")
	{
		rTown.models.always.Waterfall1 = "fontain1";
		rTown.models.always.Waterfall1.uvslide.v0 = 0.5;
		rTown.models.always.Waterfall1.uvslide.v1 = 0.0;
		rTown.models.always.Waterfall1.tech = "LocationWaterFall";
		rTown.models.always.Waterfall1.level = 50;

		rTown.models.always.Waterfall2 = "fontain2";
		rTown.models.always.Waterfall2.uvslide.v0 = 0.2;
		rTown.models.always.Waterfall2.uvslide.v1 = 0.0;
		rTown.models.always.Waterfall2.tech = "LocationWaterFall";
		rTown.models.always.Waterfall2.level = 49;
	}

	if(sTown == "Cartahena")
	{
		rTown.models.always.Waterfall1 = "river1";
		rTown.models.always.Waterfall1.uvslide.v0 = 0.2;
		rTown.models.always.Waterfall1.uvslide.v1 = 0.0;
		rTown.models.always.Waterfall1.tech = "LocationWaterFall";
		rTown.models.always.Waterfall1.level = 50;

		rTown.models.always.Waterfall2 = "river2";
		rTown.models.always.Waterfall2.uvslide.v0 = 0.3;
		rTown.models.always.Waterfall2.uvslide.v1 = 0.0;
		rTown.models.always.Waterfall2.tech = "LocationWaterFall";
		rTown.models.always.Waterfall2.level = 49;
	}

	rTown.environment.weather = "true";
	rTown.environment.sea = "true";

	DeleteAttribute(rTown, "reload");

	rTown.reload.l1.name = "reload1_back";
	rTown.reload.l1.go = "Caiman";
	rTown.reload.l1.emerge = "reload_2";
	rTown.reload.l1.autoreload = "0";
	rTown.reload.l1.label = "Sea";
	rTown.locators_radius.reload.reload1_back = 1.8;

	rTown.reload.l2.name = "reload2_back";
	rTown.reload.l2.go = "Caiman";
	rTown.reload.l2.emerge = "reload_2";
	rTown.reload.l2.autoreload = "0";
	rTown.reload.l2.label = "Sea";
	rTown.locators_radius.reload.reload2_back = 1.8;

	rTown.reload.l3.name = "reload3_back";
	rTown.reload.l3.go = "Caiman_townhall";
	rTown.reload.l3.emerge = "reload1";
	rTown.reload.l3.autoreload = "0";
	rTown.reload.l3.label = "Townhall";
	rTown.reload.l3.disable = false;

	rTown.reload.l4.name = "reload4_back";
	rTown.reload.l4.go = "Caiman_tavern";
	rTown.reload.l4.emerge = "reload1";
	rTown.reload.l4.autoreload = "0";
	rTown.reload.l4.label = "Tavern";
	rTown.reload.l4.disable = true;

	rTown.reload.l5.name = "reload5_back";
	rTown.reload.l5.go = "Caiman_shipyard";
	rTown.reload.l5.emerge = "reload1";
	rTown.reload.l5.autoreload = "0";
	rTown.reload.l5.label = "Shipyard";
	rTown.reload.l5.disable = true;
	rTown.reload.l5.close_for_night = 1;

	rTown.reload.l6.name = "reload6_back";
	rTown.reload.l6.go = "Caiman_store";
	rTown.reload.l6.emerge = "reload1";
	rTown.reload.l6.autoreload = "0";
	rTown.reload.l6.label = "Store";
	rTown.reload.l6.close_for_night = 1;
	rTown.reload.l6.disable = true;

	rTown.reload.l7.name = "reload7_back";
	rTown.reload.l7.go = "Caiman_church";
	rTown.reload.l7.emerge = "reload1";
	rTown.reload.l7.autoreload = "0";
	rTown.reload.l7.label = "Church";
	rTown.reload.l7.disable = true;
	rTown.reload.l7.close_for_night = 1;

	rTown.reload.l8.name = "reload8_back";
	rTown.reload.l8.go = "Caiman_bank";
	rTown.reload.l8.emerge = "reload1";
	rTown.reload.l8.autoreload = "0";
	rTown.reload.l8.label = "Usurer House";
	rTown.reload.l8.close_for_night = 1;
	rTown.reload.l8.disable = true;

	rTown.reload.l10.name = "reload10_back";
	rTown.reload.l10.go = "Caiman_PortOffice";
	rTown.reload.l10.emerge = "reload1";
	rTown.reload.l10.autoreload = "0";
	rTown.reload.l10.label = "PortOffice";
	rTown.reload.l10.close_for_night = 1;
	rTown.reload.l10.disable = true;

	rTown.reload.gate.name = "gate_back";
	rTown.reload.gate.go = "Caiman_ExitTown";
	rTown.reload.gate.emerge = "reload3";
	rTown.reload.gate.autoreload = "0";
	rTown.reload.gate.label = "ExitTown";

	if(sTown == "Antigua") 		{ rTown.reload.l11.name = "houseS1"; }
	if(sTown == "Maracaibo") 	{ rTown.reload.l11.name = "houseS6"; }
	if(sTown == "Cartahena") 	{ rTown.reload.l11.name = "houseS2"; }

	rTown.reload.l11.go = "Caiman_StoreHouse";
	rTown.reload.l11.emerge = "reload1";
	rTown.reload.l11.autoreload = "0";
	rTown.reload.l11.label = "Caiman Store House";
	rTown.reload.l11.disable = true;
	rTown.reload.l11.close_for_night = 1;

	if(PChar.ColonyBuilding.Store == true) { rTown.reload.l6.disable = false; }
	if(PChar.ColonyBuilding.StoreHouse == true) { rTown.reload.l11.disable = false; }
	if(PChar.ColonyBuilding.Tavern == true) { rTown.reload.l4.disable = false; }
	
	ResetLightsInLocation(rTown);

	if(sTown == "Antigua")
	{
		rTown.reload.l20.name = "houseF2";
		rTown.reload.l20.go = "CommonRoom_MH2";
		rTown.reload.l20.emerge = "reload1";
		rTown.reload.l20.autoreload = "0";
		rTown.reload.l20.label = "House";

		rTown.reload.l21.name = "houseSp3";
		rTown.reload.l21.go = "CommonDoubleflourHouse_1";
		rTown.reload.l21.emerge = "reload1";
		rTown.reload.l21.autoreload = "0";
		rTown.reload.l21.label = "House";

		rTown.reload.l22.name = "houseS4";
		rTown.reload.l22.go = "CommonCobHouse";
		rTown.reload.l22.emerge = "reload1";
		rTown.reload.l22.autoreload = "0";
		rTown.reload.l22.label = "House";

		rTown.reload.l23.name = "houseH2";
		rTown.reload.l23.go = "CommonRoom_MH4";
		rTown.reload.l23.emerge = "reload1";
		rTown.reload.l23.autoreload = "0";
		rTown.reload.l23.label = "House";

		rTown.reload.l24.name = "houseS2";
		rTown.reload.l24.go = "CommonPirateHouse";
		rTown.reload.l24.emerge = "reload1";
		rTown.reload.l24.autoreload = "0";
		rTown.reload.l24.label = "House";

		rTown.reload.l25.name = "houseSp2";
		rTown.reload.l25.go = "CommonStoneHouse";
		rTown.reload.l25.emerge = "reload1";
		rTown.reload.l25.autoreload = "0";
		rTown.reload.l25.label = "House";

		rTown.reload.l26.name = "houseSp1";
		rTown.reload.l26.go = "CommonResidence_BackHall";
		rTown.reload.l26.emerge = "reload1";
		rTown.reload.l26.autoreload = "0";
		rTown.reload.l26.label = "House";

		rTown.reload.l27.name = "houseF1";
		rTown.reload.l27.go = "CommonRoom_MH5";
		rTown.reload.l27.emerge = "reload1";
		rTown.reload.l27.autoreload = "0";
		rTown.reload.l27.label = "House";

		rTown.reload.l28.name = "houseH1";
		rTown.reload.l28.go = "CommonHut";
		rTown.reload.l28.emerge = "reload1";
		rTown.reload.l28.autoreload = "0";
		rTown.reload.l28.label = "House";

		rTown.reload.l29.name = "houseS3";
		rTown.reload.l29.go = "CommonStoneHouse";
		rTown.reload.l29.emerge = "reload1";
		rTown.reload.l29.autoreload = "0";
		rTown.reload.l29.label = "House";

		rTown.reload.l30.name = "houseF4";
		rTown.reload.l30.go = "CommonRoom_MH5";
		rTown.reload.l30.emerge = "reload1";
		rTown.reload.l30.autoreload = "0";
		rTown.reload.l30.label = "House";

		RecalculateJumpTable();
		return;
	}

	if(sTown == "Maracaibo")
	{
		rTown.reload.l20.name = "houseSp3";
		rTown.reload.l20.go = "CommonDoubleflourHouse_1";
		rTown.reload.l20.emerge = "reload1";
		rTown.reload.l20.autoreload = "0";
		rTown.reload.l20.label = "House";

		rTown.reload.l21.name = "houseSp2";
		rTown.reload.l21.go = "CommonStoneHouse";
		rTown.reload.l21.emerge = "reload1";
		rTown.reload.l21.autoreload = "0";
		rTown.reload.l21.label = "House";

		rTown.reload.l22.name = "houseF3";
		rTown.reload.l22.go = "CommonRoom_MH2";
		rTown.reload.l22.emerge = "reload1";
		rTown.reload.l22.autoreload = "0";
		rTown.reload.l22.label = "House";

		rTown.reload.l23.name = "houseS2";
		rTown.reload.l23.go = "CommonPirateHouse";
		rTown.reload.l23.emerge = "reload1";
		rTown.reload.l23.autoreload = "0";
		rTown.reload.l23.label = "House";

		rTown.reload.l24.name = "houseF4";
		rTown.reload.l24.go = "CommonRoom_MH";
		rTown.reload.l24.emerge = "reload1";
		rTown.reload.l24.autoreload = "0";
		rTown.reload.l24.label = "House";

		rTown.reload.l25.name = "houseS5";
		rTown.reload.l25.go = "CommonStoneHouse";
		rTown.reload.l25.emerge = "reload1";
		rTown.reload.l25.autoreload = "0";
		rTown.reload.l25.label = "House";

		rTown.reload.l26.name = "houseSp1";
		rTown.reload.l26.go = "CommonDoubleflourHouse_2";
		rTown.reload.l26.emerge = "reload1";
		rTown.reload.l26.autoreload = "0";
		rTown.reload.l26.label = "House";

		rTown.reload.l27.name = "houseF2";
		rTown.reload.l27.go = "CommonRoom_MH3";
		rTown.reload.l27.emerge = "reload1";
		rTown.reload.l27.autoreload = "0";
		rTown.reload.l27.label = "House";

		rTown.reload.l28.name = "houseS4";
		rTown.reload.l28.go = "CommonCobHouse";
		rTown.reload.l28.emerge = "reload1";
		rTown.reload.l28.autoreload = "0";
		rTown.reload.l28.label = "House";

		rTown.reload.l29.name = "houseS3";
		rTown.reload.l29.go = "CommonPirateHouse";
		rTown.reload.l29.emerge = "reload1";
		rTown.reload.l29.autoreload = "0";
		rTown.reload.l29.label = "House";

		rTown.reload.l30.name = "houseS1";
		rTown.reload.l30.go = "CommonFlamHouse";
		rTown.reload.l30.emerge = "reload1";
		rTown.reload.l30.autoreload = "0";
		rTown.reload.l30.label = "House";

		rTown.reload.l31.name = "houseF1";
		rTown.reload.l31.go = "CommonRoom_MH9";
		rTown.reload.l31.emerge = "reload1";
		rTown.reload.l31.autoreload = "0";
		rTown.reload.l31.label = "House";

		RecalculateJumpTable();
		return;
	}

	if(sTown == "Cartahena")
	{
		rTown.reload.l20.name = "houseS1";
		rTown.reload.l20.go = "CommonCobHouse";
		rTown.reload.l20.emerge = "reload1";
		rTown.reload.l20.autoreload = "0";
		rTown.reload.l20.label = "House";

		rTown.reload.l21.name = "houseSp1";
		rTown.reload.l21.go = "CommonPirateHouse";
		rTown.reload.l21.emerge = "reload1";
		rTown.reload.l21.autoreload = "0";
		rTown.reload.l21.label = "House";

		rTown.reload.l22.name = "houseSp2";
		rTown.reload.l22.go = "CommonDoubleflourHouse_1";
		rTown.reload.l22.emerge = "reload1";
		rTown.reload.l22.autoreload = "0";
		rTown.reload.l22.label = "House";

		rTown.reload.l23.name = "houseH1";
		rTown.reload.l23.go = "CommonRoom_MH3";
		rTown.reload.l23.emerge = "reload1";
		rTown.reload.l23.autoreload = "0";
		rTown.reload.l23.label = "House";
		rTown.reload.l23.close_for_night = 1;

		rTown.reload.l24.name = "houseSp3";
		rTown.reload.l24.go = "CommonResidence_BackHall";
		rTown.reload.l24.emerge = "reload1";
		rTown.reload.l24.autoreload = "0";
		rTown.reload.l24.label = "House";

		rTown.reload.l25.name = "houseS3";
		rTown.reload.l25.go = "CommonPirateHouse";
		rTown.reload.l25.emerge = "reload1";
		rTown.reload.l25.autoreload = "0";
		rTown.reload.l25.label = "House";

		rTown.reload.l26.name = "houseH2";
		rTown.reload.l26.go = "CommonRoom_MH4";
		rTown.reload.l26.emerge = "reload1";
		rTown.reload.l26.autoreload = "0";
		rTown.reload.l26.label = "House";

		rTown.reload.l27.name = "houseS4";
		rTown.reload.l27.go = "CommonStoneHouse";
		rTown.reload.l27.emerge = "reload1";
		rTown.reload.l27.autoreload = "0";
		rTown.reload.l27.label = "House";

		rTown.reload.l28.name = "houseF2";
		rTown.reload.l28.go = "CommonRoom_MH9";
		rTown.reload.l28.emerge = "reload1";
		rTown.reload.l28.autoreload = "0";
		rTown.reload.l28.label = "House";

		rTown.reload.l29.name = "houseF1";
		rTown.reload.l29.go = "CommonRoom_MH7";
		rTown.reload.l29.emerge = "reload1";
		rTown.reload.l29.autoreload = "0";
		rTown.reload.l29.label = "House";

		RecalculateJumpTable();
		return;
	}
}
