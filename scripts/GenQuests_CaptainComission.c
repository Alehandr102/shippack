// Gregg: вынесено из GenQuests.c (сплит god-файла) — секция «Поручение капитана: выкуп» (CaptainComission).
// Файл ТЕКСТОВО включается в GenQuests.c на исходное место (#include), поэтому видит его file-scope
// глобалы (iGenRank/sGen*) и #define (ShipSituation_*) — видимость и поведение идентичны.

//=====================================================================================================================================
// Ugeen. ГЕНЕРАТОР  "ПОРУЧЕНИЕ КАПИТАНА - ВЫКУП"
//=====================================================================================================================================
void CaptainComission_GenerateSituation(ref chref)
{
	if(CheckAttribute(chref, "RealEncounterType") && sti(chref.RealEncounterType) != ENCOUNTER_TYPE_ALONE && !CheckAttribute(pchar,"GenQuest.CaptainComission"))
	{
		aref chrShip;
		makearef(chrShip, chref.Back.Ship);
		if (CheckAttribute(chrShip, "Mode") && chrShip.Mode == "trade")
		{
			if(rand(4) == 1 || bBettaTestMode) // временно, для бетатеста
			{
				pchar.GenQuest.CaptainComission = "Begin";
				pchar.GenQuest.CaptainComission.CapId = chref.CaptanId;
				pchar.GenQuest.CaptainComission.CapName = chref.Name;
				pchar.GenQuest.CaptainComission.ShipType = GetCharacterShipType(chrShip);
			}
		}
	}
}

void CaptainComission_Init(ref chref)
{
	pchar.GenQuest.CaptainComission.Nation = chref.nation;
	pchar.GenQuest.CaptainComission.City = GetQuestNationsCity(sti(chref.nation));
	pchar.GenQuest.CaptainComission.Name = GenerateRandomName_Generator(sti(chref.nation), "man");
	pchar.GenQuest.CaptainComission.SlaveName = GenerateRandomName_Generator(sti(chref.nation), "man");
	pchar.GenQuest.CaptainComission.ShipName = chref.Back.Ship.Name;
	pchar.GenQuest.CaptainComission.Speak_Tavern = false;
	pchar.GenQuest.CaptainComission.variant = "A0";
	pchar.GenQuest.CaptainComission.Item = GenQuest_GenerateTotem();
}

void CaptainComission_Speak(string qName)
{
	if(pchar.GenQuest.CaptainComission == "Begin_1" || pchar.GenQuest.CaptainComission == "Begin_2")
	{
		AddQuestRecord("CaptainComission1", "2");
		AddQuestUserData("CaptainComission1", "sSex", GetSexPhrase("","а"));
		CloseQuestHeader("CaptainComission1");
		// LEO: срок вышел - убираем и посредника, а не только данные квеста. Иначе он остаётся с нодой
		//   CaptainComission_3: имена подставляются стёртые, а CaptainComission_6 не находит своей ветки -
		//   диалог без текста и ответов, выход только alt+f4.
		CaptainComission_EndTavern();
	}
}

void CaptainComission_SellSlave(string qName)
{
	pchar.GenQuest.CaptainComission.DaysLeft = true;
}

void CaptainComission_EndTavern()
{
	ref sld;
	sld = characterFromId("CapComission_1");
	sld.lifeDay = 0;
	LAi_CharacterDisableDialog(sld);

	sld = characterFromId("CapComission_Slave");
	RemovePassenger(pchar, sld);
	sld.lifeDay = 0;

	DeleteAttribute(pchar,"GenQuest.CaptainComission");
}

void CaptainComission_GenerateChar(string qName)
{
	ref sld = GetCharacter(NPC_GenerateCharacter("CapComission_1" , "officer_12", "man", "man", 10, sti(pchar.GenQuest.CaptainComission.Nation), -1, true));
	FantomMakeCoolFighter(sld, 10, 30, 35, LinkRandPhrase("blade22","blade34","blade21"), "pistol2", 10);
	sld.name = pchar.GenQuest.CaptainComission.Name;
	sld.lastname = "";
	sld.SaveItemsForDead = true; // сохранять на трупе вещи
	sld.DontClearDead = true; // не убирать труп через 200с
	sld.dialog.FileName = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "CaptainComission_3";
	sld.greeting = "cit_quest";
	LAi_SetSitType(sld);
	LAi_SetImmortal(sld, true);	// До поры нельзя убить
	string sLoc = pchar.GenQuest.CaptainComission.City + "_tavern";
	FreeSitLocator(sLoc, "sit2");
	ChangeCharacterAddressGroup(sld, sLoc, "sit", "sit2");
}

void CaptainComission_GetPirateIsland()
{
	ref sld;
	string CityId, PirateId;
	switch (rand(1))
	{
		case 0:
			CityId = "Pirates"; // Шарп-Таун
			PirateId = "Jackman";
			sld = CharacterFromID("Jackman");
			pchar.GenQuest.CaptainComission.PirateName = ChangeNameCase(NAMETYPE_MAIN, sld.name, NAME_ACC);
		break;
		case 1:
			CityId = "LeFransua"; // Ле-Франсуа на Мартинике
			PirateId = "LeFransua_Mayor";
			sld = CharacterFromID("LeFransua_Mayor");
			pchar.GenQuest.CaptainComission.PirateName = GetFullName(sld);
		break;
	}
	pchar.GenQuest.CaptainComission.PiratesCity = CityId;
	pchar.GenQuest.CaptainComission.PirateId = PirateId;
}

void CaptainComission_GetFamilyType()
{
	pchar.GenQuest.CaptainComission.FamilyType = drand(4) + 1;
}

void CaptainComission_30DaysIsLeft(string qName) // прошло 30 дней после разговора в таверне
{
	ref sld;
	if(pchar.GenQuest.CaptainComission.GetSlave == "NoSlave")
	{
		sld = characterFromId("CapComission_1");
		ChangeCharacterAddress(sld, "none", "");
		sld.lifeDay = 0;
		int idx = GetCharacterIndex("Bishop_manager");
		if(idx != -1)
		{
			sld = &characters[idx];
			sld.lifeDay = 0;
		}
		if(pchar.GenQuest.CaptainComission.AfterTavernSpeak == "goodSpeak")
		{
			ChangeCharacterReputation(pchar, -5);
			AddQuestRecord("CaptainComission1", "6");
			AddQuestUserData("CaptainComission1", "sSex", GetSexPhrase("не по-джентльменски","некрасиво"));
			AddQuestUserData("CaptainComission1", "sSex1", GetSexPhrase("","а"));
			AddQuestUserData("CaptainComission1", "sSex2", GetSexPhrase("","ла"));
			AddQuestUserData("CaptainComission1", "sCharName", pchar.GenQuest.CaptainComission.Name);
			AddQuestUserData("CaptainComission1", "sCapName", pchar.GenQuest.CaptainComission.CapName);
		}
		if(pchar.GenQuest.CaptainComission.AfterTavernSpeak == "badSpeak")
		{
			ChangeCharacterReputation(pchar, -3);
			AddQuestRecord("CaptainComission1", "7");
			AddQuestUserData("CaptainComission1", "sCharName", pchar.GenQuest.CaptainComission.Name);
			ChangeCharacterHunterScore(pchar, NationShortName(sti(pchar.GenQuest.CaptainComission.Nation)) + "hunter", 50);
		}
		CloseQuestHeader("CaptainComission1");
		DeleteAttribute(pchar,"GenQuest.CaptainComission");
	}
}

void CaptainComission_PirateShipsOver(string qName)
{
	pchar.quest.CapComission_PirateAttack.over = "yes";
	AddQuestRecord("CaptainComission1", "26");
	AddQuestUserData("CaptainComission1", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("CaptainComission1", "sPirateName", ChangeNameCase(NAMETYPE_MAIN, pchar.GenQuest.CaptainComission.PirateName, NAME_DAT));
	pchar.GenQuest.CaptainComission.PirateShips = "goaway";
	ChangeOfficersLoyality("bad_all", 1);
}

void CaptainComission_GetRandomShore()
{
	pchar.GenQuest.CaptainComission.Island = GetRandomIslandId();
	while(pchar.GenQuest.CaptainComission.Island == Islands[GetCharacterCurrentIsland(pchar)].id)
	{
		pchar.GenQuest.CaptainComission.Island = GetRandomIslandId();
	}
	pchar.GenQuest.CaptainComission.Island.Shore = GetIslandRandomShoreId(pchar.GenQuest.CaptainComission.Island);
	while(pchar.GenQuest.CaptainComission.Island.Shore == "")
	{
		pchar.GenQuest.CaptainComission.Island = GetRandomIslandId();
		pchar.GenQuest.CaptainComission.Island.Shore = GetIslandRandomShoreId(pchar.GenQuest.CaptainComission.Island);
		if (!isLocationFreeForQuests(pchar.GenQuest.CaptainComission.Island)) pchar.GenQuest.CaptainComission.Island.Shore = "";
	}
}

void CaptainComission_GenerateManager()
{
    ref sld;
	string sModel, sName, sLastName;
	int idx;
	switch(rand(2))
	{
		case 0:
			sModel = "trader_2";
			sName = GetConvertStrWithReplace("Variable_GenQuests_7", "Names.txt", "#space#", " ");
			sLastName = GetConvertStrWithReplace("Variable_GenQuests_8", "Names.txt", "#space#", " ");
		break;
		case 1:
			sModel = "pirate_1";
			sName = GetConvertStrWithReplace("Variable_GenQuests_9", "Names.txt", "#space#", " ");
			sLastName = GetConvertStrWithReplace("Variable_GenQuests_10", "Names.txt", "#space#", " ");
		break;
		case 2:
			sModel = "trader_3";
			sName = GetConvertStrWithReplace("Variable_GenQuests_11", "Names.txt", "#space#", " ");
			sLastName = GetConvertStrWithReplace("Variable_GenQuests_12", "Names.txt", "#space#", " ");
		break;
	}

	Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_27", "Logs.txt", "#space#", " "));
   	sld = GetCharacter(NPC_GenerateCharacter("Bishop_manager", sModel, "man", "man", 1, GetCityNation("Bridgetown"), -1, false));
    sld.name = sName;
    sld.lastname = sLastName;
	sld.dialog.filename = "GenQuests_Dialog.c";
    sld.Dialog.CurrentNode = "CaptainComission_10";
    sld.greeting = "Gr_Bishop";

	string sTemp = GetNationNameByType(GetCityNation("Bridgetown")) + "_citizens";
    LAi_group_MoveCharacter(sld, sTemp);
    LAi_SetCitizenType(sld);
	LAi_SetImmortal(sld, true);

	ChangeCharacterAddressGroup(sld, "Plantation_Sp1", "goto", "goto1");

	LAi_LocationDisableOfficersGen("BridgeTown_Plantation", true);

	idx = GetCharacterIndex("Bishop");
	if(idx != -1) // убираем полковника с плантации
	{
		sld = &characters[idx];
		ChangeCharacterAddress(sld, "None", "");
	}
}

void CaptainComission_GenerateSlave(string qName)
{
	string model[10];
	string sTemp = "SlaveGuards";
	int iNation = GetCityNation("Bridgetown");
	string slai_group = GetNationNameByType(iNation)  + "_citizens";

	model[0] = "citiz_5";
	model[1] = "shipowner_11";
	model[2] = "barmen_1";
	model[3] = "barmen_9";
	model[4] = "citiz_11";
	model[5] = "shipowner_10";
	model[6] = "trader_13";

	int iMassive = rand(6);

	ref sld = GetCharacter(NPC_GenerateCharacter("CapComission_Slave", model[iMassive], "man", "man", rand(5) + 5, sti(pchar.GenQuest.CaptainComission.Nation), -1, true));
	sld.city = pchar.GenQuest.CaptainComission.City;
	sld.name = pchar.GenQuest.CaptainComission.SlaveName;
	sld.lastname = "";
	sld.greeting = "Gr_slave";
	GiveItem2Character(sld, "unarmed");
	EquipCharacterbyItem(sld, "unarmed");
	sld.dialog.filename = "GenQuests_Dialog.c";
	LAi_SetHP(sld, 50.0, 60.0); // делаем раба задохликом

	if(pchar.GenQuest.CaptainComission.GetSlave == "GetSlave")
	{
		Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_28", "Logs.txt", "#space#", " "));
		sld.dialog.currentnode = "CaptainComission_20";
		PlaceCharacter(sld, "goto", "random_must_be_near");
		LAi_SetActorType(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);
	}
	if(pchar.GenQuest.CaptainComission.GetSlave == "NoSlave")
	{
		Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_29", "Logs.txt", "#space#", " "));
		LAi_LocationFightDisable(&Locations[FindLocation("BridgeTown_Plantation")], true);
		LAi_SetFightMode(pchar, false);
		LAi_LockFightMode(pchar, true);
		sld.dialog.currentnode = "CaptainComission_70";
		ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "quest", "sit2");
		LAi_SetActorType(sld);
		LAi_ActorSetLayMode(sld);
		LAi_SetGroundSitTypeNoGroup(sld);

		// генерим стражу
		for (int i = 0; i < 3; i++)
		{
			sld = GetCharacter(NPC_GenerateCharacter("SlaveGuard_"+i, "sold_eng_"+(rand(7)+1), "man", "man", 10, GetCityNation("Bridgetown"), 1, false));
			SetFantomParamFromRank(sld, sti(pchar.rank) + MOD_SKILL_ENEMY_RATE*3, true); // бравые орлы
            LAi_SetActorType(sld);
            LAi_SetStayType(sld);
            LAi_group_MoveCharacter(sld, sTemp);
			LAi_group_SetLookRadius(sTemp, 10);
			LAi_group_SetHearRadius(sTemp, 10);
			LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
			LAi_group_SetRelation(sTemp, slai_group, LAI_GROUP_FRIEND);
            if(i < 2) 	ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "quest", "sit"+(2*i+1));
			else		PlaceCharacter(sld, "goto", "random_free");
			sld.dialog.filename = "GenQuests_Dialog.c";
			sld.dialog.currentnode = "CaptainComission_80";
        }
		SetFunctionTimerConditionParam("CaptainComission_SlaveIsOver", 0, 0, 1, MakeInt(24 - GetHour()), false); // прерывание на исчезновение раба  в течение суток
		pchar.quest.CapComission_SlaveDeath.win_condition.l1 = "NPC_Death";
		pchar.quest.CapComission_SlaveDeath.win_condition.l1.character = "CapComission_Slave";
		pchar.quest.CapComission_SlaveDeath.function = "CaptainComission_SlaveDeath";
	}
}

void CaptainComission_GeneratePassengerSlave()
{
	string model[3];
	string sTemp = "TmpEnemy";

	model[0] = "citiz_5";
	model[1] = "shipowner_10";
	model[2] = "barmen_1";

	int iMassive = rand(2);

	ref sld = GetCharacter(NPC_GenerateCharacter("CapComission_Slave", model[iMassive], "man", "man", rand(5) + 5, sti(pchar.GenQuest.CaptainComission.Nation), -1, true));
	sld.city = pchar.GenQuest.CaptainComission.City;
	sld.name = pchar.GenQuest.CaptainComission.SlaveName;
	sld.lastname = "";

	GiveItem2Character(sld, "unarmed");
	EquipCharacterbyItem(sld, "unarmed");

	AddPassenger(pchar, sld, false);
	SetCharacterRemovable(sld, false);
	pchar.GenQuest.CaptainComission.GetSlave = "GetSlave";
}

void CaptainComission_GuardsAfter(string qName)
{
	pchar.quest.CaptainComission_SlaveIsOver.over = "yes";

	ref sld = CharacterFromId("CapComission_Slave");

	if (!LAi_IsDead(sld))
	{
		sld.dialog.filename = "GenQuests_Dialog.c";
		sld.dialog.currentnode = "CaptainComission_85";
		sld.greeting = "Gr_slave";
		LAi_SetStayType(sld);
		LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);

		pchar.quest.CaptainComission_SlaveOnMayak.win_condition.l1 = "Location";
		pchar.quest.CaptainComission_SlaveOnMayak.win_condition.l1.location = "Mayak2";
		pchar.quest.CaptainComission_SlaveOnMayak.function = "CaptainComission_OnMayak";

		SetFunctionTimerConditionParam("CaptainComission_SlaveDeathJungles", 0, 0, 2, MakeInt(24 - GetHour()), false);
	}

}

void CaptainComission_SlaveDeathJungles(string qName) // если не пришёл на маяк в течение 2-х суток - раб заблудился в джунглях и помер
{
	pchar.GenQuest.CaptainComission.GetSlave = "SlaveDied";
	pchar.quest.CaptainComission_SlaveOnMayak.over = "yes";
	AddQuestRecord("CaptainComission1", "17_1");
	AddQuestUserData("CaptainComission1", "sName", pchar.GenQuest.CaptainComission.SlaveName);
	ref sld = characterFromId("CapComission_Slave");
	ChangeCharacterAddress(sld, "none", "");
	sld.lifeDay = 0;
}

void CaptainComission_OnMayak(string qName)
{
	aref grp;
	ref pLoc, sld;
	string encGroup, str, locator;
	int	num;

	AddQuestRecord("CaptainComission1", "18");
	AddQuestUserData("CaptainComission1", "sName", pchar.GenQuest.CaptainComission.SlaveName);

	pchar.quest.CaptainComission_SlaveDeathJungles.over = "yes";

	chrDisableReloadToLocation = true;

	pLoc = &locations[FindLocation("Mayak2")];
	encGroup = LAi_FindRandomLocator("encdetector");
	str = "locators." + encGroup;
	makearef(grp, pLoc.(str));
	num = GetAttributesNum(grp);
	if (num <= 0) num = 1;

	sld = CharacterFromId("CapComission_Slave");
	LAi_CharacterEnableDialog(sld);
	sld.dialog.filename = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "CaptainComission_90";

	string sEquipItem = "blade5"; // вооружим кинжалом, он его где-то стащил :)
	AddItems(sld, sEquipItem, 1);
	EquipCharacterByItem(sld, sEquipItem);

	LAi_SetStayType(sld);
	locator = GetAttributeName(GetAttributeN(grp, 1));
	ChangeCharacterAddressGroup(sld, pLoc.id, encGroup, locator);
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void CaptainComission_SlaveDeath(string qName)
{
	pchar.GenQuest.CaptainComission.GetSlave = "SlaveDied";
	pchar.quest.CaptainComission_SlaveOnMayak.over = "yes";
	AddQuestRecord("CaptainComission1", "17");
	AddQuestUserData("CaptainComission1", "sName", pchar.GenQuest.CaptainComission.SlaveName);
}

void CaptainComission_SlaveIsOver(string qName)
{
	pchar.GenQuest.CaptainComission.GetSlave = "SlaveDied";
	ref sld = characterFromId("CapComission_Slave");
	ChangeCharacterAddress(sld, "none", "");
	sld.lifeDay = 0;
}

void CaptainComission_GeneratePatrol()
{
	ref pLoc, chr;
	string str;
	int iRank, i, num;
	int iNation = GetCityNation("Bridgetown");

	pLoc = &locations[FindLocation("Mayak2")];

	str = "Patrol"+ pLoc.index + "_";
	string sGroup = "PatrolGroup_" + pLoc.index; //имя группы
	iRank = sti(pchar.rank);
	if(MOD_SKILL_ENEMY_RATE*3 > 5) num = 5;
	else						 num = 3;

	for(i = 0; i < num; i++)
	{
		if(i == 0)
		{
			int iPatrolOfficer = NPC_GenerateCharacter(str + i, "off_"+NationShortName(iNation)+"_"+(rand(1)+1), "man", "man", iRank + 2, iNation, 1, true);
			chr = &characters[iPatrolOfficer];
			SetFantomParamFromRank(chr, iRank + 2, true);
			chr.greeting = "soldier_arest";
		}
		else
		{
			chr = GetCharacter(NPC_GenerateCharacter(str + i, "sold_"+NationShortName(iNation)+"_"+(rand(7)+1), "man", "man", iRank, iNation, 1, true));
			SetFantomParamFromRank(chr, iRank, true);
		}
		chr.City = "Bridgetown";
		chr.CityType = "soldier";
		LAi_SetWarriorType(chr);
		ChangeCharacterAddressGroup(chr, pLoc.id, "reload", "reload1");
		LAi_group_MoveCharacter(chr, sGroup);
	}

	LAi_group_SetRelation(sGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups(sGroup, LAI_GROUP_PLAYER, true);
	LAi_group_SetCheckFunction(sGroup, "CaptainComission_ExitFromMayak");
	LAi_group_SetCheck(sGroup, "OpenTheDoors");
}

void CaptainComission_ExitFromMayak(string qName)
{
	ref sld;

	sld = characterFromId("CapComission_Slave");
	if (!LAi_IsDead(sld)) 	pchar.GenQuest.CaptainComission.GetSlave = "GetSlave";
	else 					pchar.GenQuest.CaptainComission.GetSlave = "SlaveDied";

	if(pchar.GenQuest.CaptainComission.GetSlave == "GetSlave")
	{
		pchar.quest.CapComission_SlaveDeath.over = "yes";
		AddPassenger(pchar, sld, false);
		SetCharacterRemovable(sld, false);

		if(pchar.GenQuest.CaptainComission == "toEnemyTavern")
		{
			pchar.quest.CapComission_DeliveSlave.win_condition.l1 = "location";
			pchar.quest.CapComission_DeliveSlave.win_condition.l1.location = pchar.GenQuest.CaptainComission.SlaveCity + "_tavern";
			pchar.quest.CapComission_DeliveSlave.function = "CaptainComission_DeliveSlave";
		}
	}
}

void CaptainComission_GetSlave()
{
	ref sld = CharacterFromId("Bishop_manager");
	sld.lifeDay = 1;

	pchar.quest.CapComission_GetSlave.win_condition.l1 = "ExitFromLocation";
	pchar.quest.CapComission_GetSlave.win_condition.l1.location = pchar.location;
	pchar.quest.CapComission_GetSlave.function = "CaptainComission_GenerateSlave";

	pchar.quest.CaptainComission_30DaysIsLeft.over = "yes";
}

void CaptainComission_DeliveSlave(string qName)
{
	chrDisableReloadToLocation = true;
	ref sld = characterFromId("CapComission_Slave");
	RemovePassenger(pchar, sld);
	sld.dialog.filename = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "CaptainComission_100";
	PlaceCharacter(sld, "goto", "random_must_be_near");
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void GetSlaveSpeciality()
{
	string Speciality;
	int Spec = drand(2);
	switch (Spec)
    {
        case 0 :
             Speciality = "в кораблестроении";
        break;
        case 1 :
            Speciality = "в артиллерии";
        break;
        case 2 :
            Speciality = "в навигации";
        break;
    }
	pchar.GenQuest.CaptainComission.SlaveSpeciality = Speciality;
	pchar.GenQuest.CaptainComission.Speciality = Spec;
}

void CaptainComission_GeneratePirateShips(string qName)
{
	int i, ShipType, Rank, iShipRank, iCannonType;
	ref sld;
	string Blade, sTemp;

	Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_30", "Logs.txt", "#space#", " "));
    Island_SetReloadEnableGlobal(pchar.GenQuest.CaptainComission.Island, false);
    Group_FindOrCreateGroup("Pirate_Attack");
	Group_SetType("Pirate_Attack", "war");
	// ==> Пиратские кэпы
	for (i=1; i<=2; i++)
	{
		Rank = sti(pchar.rank) + rand(MOD_SKILL_ENEMY_RATE*3);
		if(makeint(pchar.rank) > 13) { iShipRank = rand(2) + 3; }
		if(makeint(pchar.rank) > 6 && makeint(pchar.rank) < 13) { iShipRank = rand(2); }
		if(makeint(pchar.rank) < 7) { iShipRank = rand(1); }
		switch (iShipRank)
		{
			case 0:
				ShipType = SHIP_SCHOONER;
				Rank = 10 + rand(5);
                Blade = "blade19";
				iCannonType = CANNON_TYPE_CANNON_LBS16;
			break;
			case 1:
				ShipType = SHIP_BRIG;
				Rank = 14 + rand(5);
                Blade = "blade22";
				iCannonType = CANNON_TYPE_CANNON_LBS24;
			break;
			case 2:
				ShipType = SHIP_BRIGANTINE;
				Rank = 20 + rand(5);
                Blade = "blade31";
				iCannonType = CANNON_TYPE_CANNON_LBS24;
			break;
			case 3:
				ShipType = SHIP_CORVETTE;
				Rank = 25 + rand(5);
                Blade = "blade34";
				iCannonType = CANNON_TYPE_CANNON_LBS32;
			break;
			case 4:
				ShipType = SHIP_FRIGATE;
				Rank = 30 + rand(5);
                Blade = "blade15";
				iCannonType = CANNON_TYPE_CANNON_LBS32;
			break;
			case 5:
				ShipType = SHIP_LINESHIP;
				Rank = 35 + rand(5);
                Blade = "blade21";
				iCannonType = CANNON_TYPE_CANNON_LBS32;
			break;
		}
		if(i == 1) 	sTemp = pchar.GenQuest.CaptainComission.ShipName1;
		else 		sTemp = pchar.GenQuest.CaptainComission.ShipName2;
		sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, "officer_"+(rand(42)+1), "man", "man", Rank, PIRATE, 3, true));
		FantomMakeCoolSailor(sld, ShipType, sTemp, iCannonType, 60 + rand(20), 95, 95);
		FantomMakeCoolFighter(sld, Rank, 50 + rand(30), 50 + rand(20), Blade, "pistol3", 50);
		Group_AddCharacter("Pirate_Attack", "CaptainAttack_"+i);

		sld.SuperShooter  = true;

		SetCharacterPerk(sld, "Tireless");
		SetCharacterPerk(sld, "HardHitter");
		SetCharacterPerk(sld, "GunProfessional");
		SetCharacterPerk(sld, "MusketsShoot");

		sld.ship.Crew.Morale = 50 + rand(30);
		ChangeCrewExp(sld, "Sailors", 70 + rand(10));
		ChangeCrewExp(sld, "Cannoners", 70 + rand(10));
		ChangeCrewExp(sld, "Soldiers", 70 + rand(10));
    }
    // ==> стравливание
	Group_SetGroupCommander("Pirate_Attack", "CaptainAttack_1");
	Group_SetTaskAttack("Pirate_Attack", PLAYER_GROUP);
	Group_SetPursuitGroup("Pirate_Attack", PLAYER_GROUP);
	Group_SetAddress("Pirate_Attack", pchar.GenQuest.CaptainComission.Island, "", "");
	Group_LockTask("Pirate_Attack");

    // ==> прерывание на убиение эскадры
    pchar.quest.CapComission_AfterBattle.win_condition.l1 = "Group_Death";
	pchar.quest.CapComission_AfterBattle.win_condition.l1.group = "Pirate_Attack";
	pchar.quest.CapComission_AfterBattle.function = "CaptainComission_PirateAttack_AfterBattle";
	pchar.quest.CapComission_DieHard.win_condition.l1 = "MapEnter";
    pchar.quest.CapComission_DieHard.function = "CaptainComission_PirateAttack_DieHard";
}

void CaptainComission_PirateAttack_AfterBattle(string qName)
{
	Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_31", "Logs.txt", "#space#", " "));
	Group_DeleteGroup("Pirate_Attack");
	Island_SetReloadEnableGlobal(pchar.GenQuest.CaptainComission.Island, true);
    pchar.quest.CapComission_DieHard.over = "yes";
	pchar.quest.CaptainComission_PirateShipsOver.over = "yes";
	AddQuestRecord("CaptainComission1", "25");
	AddQuestUserData("CaptainComission1", "sName", ChangeNameCase(NAMETYPE_MAIN, pchar.GenQuest.CaptainComission.PirateName, NAME_GEN));
	AddQuestUserData("CaptainComission1", "sShipName1", pchar.GenQuest.CaptainComission.ShipName1);
	AddQuestUserData("CaptainComission1", "sShipName2", pchar.GenQuest.CaptainComission.ShipName2);
	AddQuestUserData("CaptainComission1", "sCity", XI_ConvertString("Colony" + pchar.GenQuest.CaptainComission.PiratesCity + "Gen"));
	AddQuestUserData("CaptainComission1", "sNameNom", ChangeNameCase(NAMETYPE_MAIN, pchar.GenQuest.CaptainComission.PirateName, NAME_NOM));
	pchar.GenQuest.CaptainComission.PirateShips = "died";
	DeleteAttribute(pchar,"GenQuest.CaptainComission.RepeatSpeak");
}

void CaptainComission_PirateAttack_DieHard(string qName)
{
	Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_32", "Logs.txt", "#space#", " "));
	Group_DeleteGroup("Pirate_Attack");
    Island_SetReloadEnableGlobal(pchar.GenQuest.CaptainComission.Island, true);
    pchar.quest.CapComission_AfterBattle.over = "yes";
	pchar.quest.CaptainComission_PirateShipsOver.over = "yes";
	AddQuestRecord("CaptainComission1", "33");
	AddQuestUserData("CaptainComission1", "sSex", GetSexPhrase("","ла"));
	AddQuestUserData("CaptainComission1", "sShipName1", pchar.GenQuest.CaptainComission.ShipName1);
	AddQuestUserData("CaptainComission1", "sShipName2", pchar.GenQuest.CaptainComission.ShipName2);
	AddQuestUserData("CaptainComission1", "sName", ChangeNameCase(NAMETYPE_MAIN, pchar.GenQuest.CaptainComission.PirateName, NAME_DAT));
	pchar.GenQuest.CaptainComission.PirateShips = "goaway";
	ChangeOfficersLoyality("bad_all",1);
}
//=====================================================================================================================================
// <-- ГЕНЕРАТОР  "ПОРУЧЕНИЕ КАПИТАНА - ВЫКУП"
//=====================================================================================================================================

//=====================================================================================================================================
// Ugeen. ГЕНЕРАТОР  "ПОРУЧЕНИЕ КАПИТАНА - ОПЕРАЦИЯ 'ГАЛЕОН'"
//=====================================================================================================================================
void CaptainComission_GaleonInit(ref chref)
{
	pchar.GenQuest.CaptainComission.Nation = chref.nation;
	pchar.GenQuest.CaptainComission.City = GetQuestNationsPrison(sti(chref.nation));
	pchar.GenQuest.CaptainComission.Name = GenerateRandomName_Generator(sti(chref.nation), "man");
	pchar.GenQuest.CaptainComission.ShipName = chref.Back.Ship.Name;
	pchar.GenQuest.CaptainComission.City1 = FindAlliedColonyForNationExceptColony(pchar.GenQuest.CaptainComission.City);
	pchar.GenQuest.CaptainComission.Goods = CaptainComission_GetVictimShipGoods();
	pchar.GenQuest.CaptainComission.Speak_Tavern = false;
	pchar.GenQuest.CaptainComission.Prize = GenQuest_GenerateTotem();
	CaptainComission_GenerateShips();
}

void CaptainComission_GenerateShips()
{
	int iShipType, iShipTypeVictim;
	if(sti(pchar.rank) < 20)
	{
		switch(rand(1))
		{
			case 0: iShipType = SHIP_LUGGER; 	break;
			case 1: iShipType = SHIP_SLOOP;		break;
		}
		switch(rand(2))
		{
			case 0: iShipTypeVictim = SHIP_CARAVEL; 	break;
			case 1: iShipTypeVictim = SHIP_BARKENTINE;	break;
			case 2: iShipTypeVictim = SHIP_FLEUT;		break;
		}
	}
	else
	{
		switch(rand(2))
		{
			case 0: iShipType = SHIP_BARQUE;	break;
			case 1: iShipType = SHIP_BRIG;		break;
			case 2:	iShipType = SHIP_CORVETTE;	break;
		}
		switch(rand(2))
		{
			case 0: iShipTypeVictim = SHIP_ROTERLOW;	break;
			case 1: iShipTypeVictim = SHIP_GALEON_H;	break;
			case 2:	iShipTypeVictim = SHIP_PINNACE;		break;
		}
	}
	pchar.GenQuest.CaptainComission.ShipType = iShipType;
	pchar.GenQuest.CaptainComission.ShipTypeVictim = iShipTypeVictim;
	pchar.GenQuest.CaptainComission.ShipTypeName = GenerateRandomNameToShip(sti(pchar.GenQuest.CaptainComission.Nation));
	pchar.GenQuest.CaptainComission.VictimShipName = GenerateRandomNameToShip(rand(3));
}

// не успели за 15 дней - закрываем квест
void CaptainComission_TimeIsOver(string qName)
{
	ref sld;
	if(CheckAttribute(pchar,"GenQuest.CaptainComission.variant"))
	{
		if(pchar.GenQuest.CaptainComission.variant == "A1")
		{
			pchar.quest.CaptainComission_SailToShore.over = "yes";
			Group_DeleteGroup("Sea_CapComission_1");
			sld = CharacterFromID("CapComission_1");
			sld.LifeDay = 0;
			AddQuestRecord("CaptainComission2", "8");
			AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
			AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
			CloseQuestHeader("CaptainComission2");
			DeleteAttribute(pchar, "GenQuest.CaptainComission");
			return;
		}
		if(pchar.GenQuest.CaptainComission.variant == "A2")
		{
			AddQuestRecord("CaptainComission2", "8");
			AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
			AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
			CloseQuestHeader("CaptainComission2");
			DeleteAttribute(pchar, "GenQuest.CaptainComission");
			return;
		}
		if(pchar.GenQuest.CaptainComission.variant == "A3")
		{
			pchar.quest.CaptainComission_MeetCanoneerInShoreTimer.over = "yes";
			sld = CharacterFromID("CapComission_Canoneer");
			sld.LifeDay = 0;
			AddQuestRecord("CaptainComission2", "44");
			AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
			AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
			AddQuestUserData("CaptainComission2", "sCharName", GetName( NAMETYPE_ORIG, pchar.GenQuest.CaptainComission.CanoneerName, NAME_NOM));
			CloseQuestHeader("CaptainComission2");
			ChangeOfficersLoyality("bad_all", 1);
			DeleteAttribute(pchar, "GenQuest.CaptainComission");
			return;
		}
	}

	AddQuestRecord("CaptainComission2", "2");
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.CapName);
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

// генерим продолжение квеста в зависимости от срока прибытия к заданному острову
void CaptainComission_GenerateQuestContinuation(string qName)
{
	int DaysIsLeft = GetQuestPastDayParam("GenQuest.CaptainComission");

	pchar.quest.CaptainComission_TimeIsOver.over = "yes"; // снимаем таймер на опоздание
	if(DaysIsLeft <= 5) // вариант A1 - 100 процентов
	{
		pchar.GenQuest.CaptainComission.variant = "A1";
	}
	else
	{
		if(DaysIsLeft <= 10) // вариант A1 - 50 процентов  && A2 - 50 процентов
		{
			if(rand(1) == 0) 	pchar.GenQuest.CaptainComission.variant = "A1";
			else				pchar.GenQuest.CaptainComission.variant = "A2";
		}
		else // вариант A1 - 30 процентов, A2 - 30 процентов, A3 - 40 процентов
		{
			int iRnd = rand(9);
			if(iRnd < 3) pchar.GenQuest.CaptainComission.variant = "A1";
			else
			{
				if(iRnd < 6) 	pchar.GenQuest.CaptainComission.variant = "A2";
				else			pchar.GenQuest.CaptainComission.variant = "A3";
			}
		}
	}
	CaptainComission_GenerateQuestVariant();
}

void CaptainComission_GenerateQuestVariant()
{
	switch(pchar.GenQuest.CaptainComission.variant)
	{
		case "A1":
			Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_33", "Logs.txt", "#space#", " "));
			CaptainComission_GenerateShip();
		break;
		case "A2":
			Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_34", "Logs.txt", "#space#", " "));
			CaptainComission_GenerateRumourCaptainPrison();
		break;
		case "A3":
			Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_35", "Logs.txt", "#space#", " "));
			CaptainComission_GenerateRumourCaptainDeath();
		break;
	}
}

void CaptainComission_GenerateShip()
{
	int iRank = sti(pchar.rank) + 5 + rand(MOD_SKILL_ENEMY_RATE*3);
	int iNation = sti(pchar.GenQuest.CaptainComission.Nation);
	ref chref = GetCharacter(NPC_GenerateCharacter("CapComission_1", "off_"+NationShortName(iNation)+"_"+(rand(1)+1), "man", "man", iRank, iNation, -1, true));

	chref.name = pchar.GenQuest.CaptainComission.Name;
	chref.lastname = "";
	chref.city = pchar.GenQuest.CaptainComission.City;
	chref.dialog.filename      = "GenQuests_Dialog.c";
	chref.DeckDialogNode	   = "CaptainComission_301";
	chref.dialog.currentnode   = "CaptainComission_301";

	chref.Ship.Name = pchar.GenQuest.CaptainComission.ShipTypeName;
	FantomMakeCoolSailor(chref, sti(pchar.GenQuest.CaptainComission.ShipType), chref.Ship.Name, CANNON_TYPE_CULVERINE_LBS24, 90, 90, 90);
	FantomMakeCoolFighter(chref, makeint(pchar.rank) + rand(10) + 5, 90, 50, LinkRandPhrase("blade22","blade34","blade21"), "pistol3", 80);
	chref.cirassId = Items_FindItemIdx("cirass5");
	LAi_SetHP(chref, 200 + makeint(pchar.rank) * 5, 200 + makeint(pchar.rank) * 5);

	SetCharacterPerk(chref, "MusketsShoot");
	SetCharacterPerk(chref, "Energaiser");

	chref.ship.Crew.Morale = 90;
	ChangeCrewExp(chref, "Sailors", 100);
	ChangeCrewExp(chref, "Cannoners", 100);
	ChangeCrewExp(chref, "Soldiers", 100);

	chref.AlwaysFriend = true;
	chref.Abordage.Enable = false; //нельзя брать на абордаж

	string sGroup = "Sea_CapComission_1";
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
    Group_SetType(sGroup, "war");

	SetCharacterRelationBoth(sti(chref.index), GetMainCharacterIndex(), RELATION_FRIEND);

	int IslandID = FindIsland(GetArealByCityName(pchar.GenQuest.CaptainComission.City));
	ref pLoc = &Islands[IslandID];
	float x, y, z;
	FindIslandLocatorXYZ(IslandID, "reload1", &x, &y, &z);
	string sLocator = GetSeaQuestShipFarLocator(pLoc, "Quest_ships", x, y, z);

	Group_AddCharacter(sGroup, "CapComission_1");
    Group_SetGroupCommander(sGroup, "CapComission_1");
	Group_SetAddress(sGroup, GetArealByCityName(pchar.GenQuest.CaptainComission.City),"quest_ships", sLocator);
	Group_SetTaskNone(sGroup);

	SetFunctionTimerConditionParam("CaptainComission_TimeIsOver", 0, 0, 1, MakeInt(24 - GetHour()), false);
}

void CaptainComission_ExitFromLoc(string qName)
{
	ref sld;
	if(pchar.GenQuest.CaptainComission == "close")
	{
		if(pchar.GenQuest.CaptainComission.variant == "A1")
		{
			sld = CharacterFromID("CapComission_1");
			sld.Dialog.Filename = "Capitans_dialog.c"; // при повторном диалоге
			sld.greeting        = "Gr_Commander";
			sld.DeckDialogNode  = "Go_away";
			sld.LifeDay = 2;					// сам помрет через 2 дня
		}
		if(pchar.GenQuest.CaptainComission.variant == "A3")
		{
			sld = CharacterFromID("CapComission_Canoneer");
			sld.LifeDay = 0;
		}
		DeleteAttribute(pchar, "GenQuest.CaptainComission");
	}
}

int CaptainComission_GetVictimShipGoods()
{
	int iGoodIdx;
	int aGoods[10];
	aGoods[0] = GOOD_SANDAL;
	aGoods[1] = GOOD_TOBACCO;
	aGoods[2] = GOOD_COFFEE;
	aGoods[3] = GOOD_MAHOGANY;
	aGoods[4] = GOOD_GOLD;
	aGoods[5] = GOOD_EBONY;
	aGoods[6] = GOOD_SILK;
	aGoods[7] = GOOD_SILVER;
	aGoods[8] = GOOD_SILVER;
	aGoods[9] = GOOD_GOLD;
	int iPick = rand(7 + makeint(GetCharacterSPECIALSimple(pchar, SPECIAL_L)/5));
	if (iPick <= 9) iGoodIdx = aGoods[iPick];
	return iGoodIdx;
}

void CaptainComission_CapIsOut(string qName) // свалили из боя
{
	ref sld;
	pchar.GenQuest.CaptainComission = "CaptainIsOut";
	pchar.quest.CaptainComission_CapIsDead.over = "yes";
	Group_DeleteGroup("Sea_CapComission_1");
	sld = CharacterFromID("CapComission_1");
	sld.LifeDay = 0;

	AddQuestRecord("CaptainComission2", "6");
	AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("ся","ась"));
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	AddQuestUserData("CaptainComission2", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.CaptainComission.Goods)].Name + "Gen")));
	AddQuestUserData("CaptainComission2", "sShoreName", XI_ConvertString(pchar.GenQuest.CaptainComission.ShoreLocation + "Gen"));
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_CapIsDead(string qName)
{
	pchar.quest.CaptainComission_CapIsOut.over = "yes";
	AddQuestRecord("CaptainComission2", "5");
	AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("ся","ась"));
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	AddQuestUserData("CaptainComission2", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.CaptainComission.Goods)].Name + "Gen")));
	AddQuestUserData("CaptainComission2", "sShoreName", XI_ConvertString(pchar.GenQuest.CaptainComission.ShoreLocation + "Gen"));
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_toShore() // вышвыриваем в бухту без штанов
{
	ref sld;
	Group_DeleteGroup("Sea_CapComission_1");
	sld = CharacterFromID("CapComission_1");
	sld.LifeDay = 0;

	DeleteAttribute(pchar, "ship"); // допрыгался
	pchar.ship.name = "";
	pchar.ship.type = SHIP_NOTUSED;

	SetCharacterShipLocation(pchar, "");

	pchar.money = 0;
	RemoveAllPassengersAndCompanions();
	CleanAllCabinBoxes();

	AddSimpleRumour(RandPhraseSimple(GetConvertStrWithReplace("Variable_GenQuests_6", "AddRumours.txt", "#space#", " "),
				GetConvertStrWithReplace("Variable_GenQuests_7", "AddRumours.txt", "#space#", " ")), sti(pchar.GenQuest.CaptainComission.Nation), 5, 1);
	pchar.quest.Munity = "Deads";
	pchar.location.from_sea = pchar.GenQuest.CaptainComission.City + "_town";

	AddQuestRecord("CaptainComission2", "4");
	AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	AddQuestUserData("CaptainComission2", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.CaptainComission.Goods)].Name + "Gen")));
	AddQuestUserData("CaptainComission2", "sShoreName", XI_ConvertString(pchar.GenQuest.CaptainComission.ShoreLocation + "Gen"));
	CloseQuestHeader("CaptainComission2");

	if(bDisableMapEnter) bDisableMapEnter = false; //  если был бой

	DoReloadFromDeckToLocation(pchar.GenQuest.CaptainComission.ShoreLocation, "reload", "sea");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_SailToShore(string qName)
{
	Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_36", "Logs.txt", "#space#", " "));
	ref sld = characterFromId("CapComission_1");
	SetCharacterShipLocation(sld, pchar.GenQuest.CaptainComission.ShoreLocation);
	SetFunctionLocationCondition("CaptainComission_MeetInShore", pchar.GenQuest.CaptainComission.ShoreLocation, false);
}

// встречаемся на берегу
void CaptainComission_MeetInShore(string qName)
{
	chrDisableReloadToLocation = true;
	pchar.quest.CaptainComission_TimeIsOver.over = "yes";
	ref sld = characterFromId("CapComission_1");
	if(pchar.GenQuest.CaptainComission.variant == "A1")
	{
		sld.dialog.currentnode = "CaptainComission_315";
	}
	if(pchar.GenQuest.CaptainComission.variant == "A2")
	{
		sld.dialog.currentnode = "CaptainComission_383";
		LAi_LocationDisableOfficersGen(pchar.GenQuest.CaptainComission.ConvoyShore, true);
		pchar.quest.CaptainComission_CapEscapeTimer.over = "yes";
		DeleteAttribute(pchar,"GenQuest.CaptainComission.canSpeakBoatswain");
		CaptainComission_GenerateGangInShore();
	}
	PlaceCharacter(sld, "goto", "random_must_be_near");
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void CaptainComission_GenerateShorePatrol()
{
	ref sld;
	string attrName;

	LAi_SetFightMode(pchar, false);
	LAi_LockFightMode(pchar, true);
	int iTemp = sti(pchar.GenQuest.CaptainComission.Nation);// Нация патруля
	pchar.GenQuest.CaptainComission.SoldierQty = makeint(2 * GetOfficersQuantity(pchar) + 3);
	for (int i = 2; i <= sti(pchar.GenQuest.CaptainComission.SoldierQty); i++)
	{
		sld = SetFantomDefenceForts("", "", iTemp, "CoastalGuards");
		attrName = "SoldierIDX" + i;
    	pchar.GenQuest.CaptainComission.(attrName) = sld.index;
		LAi_SetActorType(sld);
    	LAi_ActorFollow(sld, pchar, "", 38);
		LAi_CharacterDisableDialog(sld);
		sld.greeting = "Gr_Costal_Guards";
	}
	sld = SetFantomOfficer("", "", iTemp, "CoastalGuards");
	pchar.GenQuest.CaptainComission.SoldierIDX1 = sld.index;
	LAi_SetActorType(sld);
	sld.Dialog.Filename = "GenQuests_Dialog.c";
	sld.Dialog.CurrentNode = "CaptainComission_317";
	sld.greeting = "Gr_Costal_Guards";

	LAi_ActorDialog(&Characters[makeint(pchar.GenQuest.CaptainComission.SoldierIDX1)], pchar, "", 35, 1);
	LAi_group_SetCheck("CoastalGuards", "OpenTheDoors");
	LAi_group_SetCheckFunction("CoastalGuards", "CaptainComission_AfterShoreBattle");
}

void CaptainComission_AfterShoreBattle(string qName)
{
	ref sld = characterFromId("CapComission_1");
	if (LAi_IsDead(sld))
	{
		AddQuestRecord("CaptainComission2", "9");
		AddQuestUserData("CaptainComission2", "sShoreName", XI_ConvertString(pchar.GenQuest.CaptainComission.ShoreLocation + "Voc"));
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		CloseQuestHeader("CaptainComission2");
		DeleteAttribute(pchar, "GenQuest.CaptainComission");
	}
	else
	{
		sld.dialog.currentnode = "CaptainComission_320";
		LAi_SetActorType(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);
	}
}

void CaptainComission_GenerateCoastalPatrol()
{
	ref CoastGuard;
	int i;
	int iNation = sti(pchar.GenQuest.CaptainComission.Nation);// Нация патруля
	string Model;

	for (i = 1; i <= 3; i++)
    {
        Model = "off_" + NationShortName(iNation) + "_" + (rand(1) + 1);
		CoastGuard = GetCharacter(NPC_GenerateCharacter("Coastal_Captain0" + i, Model, "man", "man", sti(pchar.rank) + rand(MOD_SKILL_ENEMY_RATE*3), iNation, 3, true)); // 3 дня, потом сами пропадут
		SetFantomParam(CoastGuard);
		SelectCoastalGuardShip(CoastGuard);
		CoastGuard.AlwaysEnemy = true;
		CoastGuard.DontRansackCaptain = true;
		CoastGuard.AlwaysSandbankManeuver = true;
		Group_AddCharacter("Coastal_Guards", CoastGuard.id);
		SetCharacterRelationBoth(sti(CoastGuard.index), GetMainCharacterIndex(), RELATION_ENEMY);
		if (makeint(pchar.rank) < 6 && i == 1 && GetCompanionQuantity(pchar) == 1) break;
		if (makeint(pchar.rank) < 9 && i == 2 && GetCompanionQuantity(pchar) < 3) break;
    }
	Group_SetGroupCommander("Coastal_Guards", "Coastal_Captain01");
	Group_SetAddress("Coastal_Guards", Islands[GetCharacterCurrentIsland(Pchar)].id, "", "");
	Group_SetPursuitGroup("Coastal_Guards", PLAYER_GROUP);
	Group_SetTaskAttack("Coastal_Guards", PLAYER_GROUP);
	Group_LockTask("Coastal_Guards");
}

void CaptainComission_CapShip_Sink(string qName)
{
	DeleteAttribute(pchar,"GenQuest.CannotWait");
	pchar.quest.CaptainComission_MapEnter.over = "yes";
	AddQuestRecord("CaptainComission2", "12");
	AddQuestUserData("CaptainComission2", "sShipType", GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")));
	AddQuestUserData("CaptainComission2", "sShipName", pchar.GenQuest.CaptainComission.ShipTypeName);
	AddQuestUserData("CaptainComission2", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.CaptainComission.Goods)].Name + "Gen")));
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.CapName);
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_MapEnter(string qName)
{
	DeleteAttribute(pchar,"GenQuest.CannotWait");
	pchar.quest.CaptainComission_CapShip_Sink.over = "yes";
	SetFunctionLocationCondition("CaptainComission_ConvoyInShore", pchar.GenQuest.CaptainComission.ConvoyShore, false);
	pchar.quest.CaptainComission_Ship_Sink.win_condition.l1 = "Character_sink";
	pchar.quest.CaptainComission_Ship_Sink.win_condition.l1.character = "CapComission_1";
	pchar.quest.CaptainComission_Ship_Sink.function = "CaptainComission_Ship_Sink";
	SetFunctionTimerCondition("CaptainComission_ConvoyTimeIsOut", 0, 0, sti(pchar.GenQuest.CaptainComission.iDay) * 2, false);
	SaveCurrentQuestDateParam("GenQuest.CaptainComission");
}

void CaptainComission_ConvoyInShore(string qName)
{
	ref sld;
	pchar.quest.CaptainComission_Ship_Sink.over = "yes";
	pchar.quest.CaptainComission_ConvoyTimeIsOut.over = "yes";

	int DaysIsLeft = GetQuestPastDayParam("GenQuest.CaptainComission");

	sld = characterFromId("CapComission_1");
	PlaceCharacter(sld, "goto", "random_must_be_near");
	if(DaysIsLeft <= sti(pchar.GenQuest.CaptainComission.iDay))
	{
		LAi_SetActorTypeNoGroup(sld);
		CaptainComission_GenerateGangInShore();
	}
	else
	{
		sld.dialog.currentnode = "CaptainComission_340";
		LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);
	}
}

void CaptainComission_GenerateGangInShore()
{
	string encGroup, str, locator;
	int num, i, k, iNation, iRank, n;
	ref pLoc, sld;
	aref grp;

	chrDisableReloadToLocation = true;
	pLoc = &locations[FindLocation(pchar.GenQuest.CaptainComission.ConvoyShore)];
	encGroup = "smugglers";
	str = "locators." + encGroup;
	makearef(grp, pLoc.(str));
	num = GetAttributesNum(grp); //кол-во человек в банде
	if (num <= 0) num = 1;

	str = "Gang" + "_";
	string sGroup = "GangGroup_0"; //имя группы
	LAi_LocationFightDisable(pLoc, true);
	LAi_SetFightMode(pchar, false);
	LAi_LockFightMode(pchar, true);

	arrayNPCModelHow = 0;

	for (i = 0; i < num; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter(str + i, "off_hol_2", "man", "man", 5, PIRATE, 3, true));
        SetFantomParamHunter(sld);
		SetModelPirate(sld);
		k = 0;
		while (!CheckNPCModelUniq(sld) && k < 10)
		{
		    k++;
			SetModelPirate(sld);
		}
		arrayNPCModel[arrayNPCModelHow] = sld.model;
		arrayNPCModelHow++;

		LAi_SetStayType(sld);
		LAi_group_MoveCharacter(sld, sGroup);
		pchar.GenQuest.CaptainComission.GangNum = num;
		sld.greeting = "Gr_Smuggler_OnShore";
		locator = GetAttributeName(GetAttributeN(grp, i));
		ChangeCharacterAddressGroup(sld, pLoc.id, encGroup, locator);
		LAi_SetImmortal(sld, true);
		if (i == 0)
		{
			sld.SaveItemsForDead = true;
			sld.DontClearDead = true;
			sld.dialog.filename = "GenQuests_Dialog.c";
			if(pchar.GenQuest.CaptainComission.variant == "A1")
			{
				sld.Dialog.CurrentNode = "CaptainComission_326";
			}
			if(pchar.GenQuest.CaptainComission.variant == "A2")
			{
				if(pchar.GenQuest.CaptainComission == "GetGoodsEscape")
				{
					sld.Dialog.CurrentNode = "CaptainComission_396";
				}
				else
				{
					sld.Dialog.CurrentNode = "CaptainComission_384";
				}
			}
			if(pchar.GenQuest.CaptainComission.variant == "A3")
			{
				sld.Dialog.CurrentNode = "CaptainComission_360";
			}
		}
		else
		{
			LAi_CharacterDisableDialog(sld);
		}
	}
	sld = CharacterFromID("CapComission_1");
	if(pchar.GenQuest.CaptainComission.variant == "A1")
	{
		sld.Dialog.CurrentNode = "CaptainComission_326_0";
	}
	if(pchar.GenQuest.CaptainComission.variant == "A2" && pchar.GenQuest.CaptainComission == "GetGoodsEscape")
	{
		sld.Dialog.CurrentNode = "CaptainComission_396_0";
	}
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void CaptainComission_DialogInShore()
{
	ref sld = CharacterFromID("CapComission_1");
	sld.CaptainComission.Name = pchar.GenQuest.CaptainComission.Name;
	sld.CaptainComission.CapName = pchar.GenQuest.CaptainComission.CapName;
	sld.CaptainComission.FullName = GetFullName(pchar);
	sld.CaptainComission.ShipTypeVictim = pchar.GenQuest.CaptainComission.ShipTypeVictim;

	bDisableCharacterMenu = true;
	ChangeShowIntarface();
	InterfaceStates.Buttons.Save.enable = false;
	StartQuestMovie(true, true, true);
	LAi_SetActorType(pchar);

	SetMainCharacterIndex(GetCharacterIndex("CapComission_1"));
	pchar = GetMainCharacter();
	locCameraTarget(pchar)
	locCameraFollow();
	LAi_SetActorType(pchar);
	LAi_SetActorType(CharacterFromID("Gang_0"));
	SetActorDialogAny2Pchar("Gang_0", "", -1.0, 0.0);
	LAi_ActorFollow(pchar, CharacterFromID("Gang_0"), "ActorDialog_Any2Pchar", -1);
}


void CaptainComission_GangDialog()
{
	ref sld;

	SetMainCharacterIndex(1);
    pchar = GetMainCharacter();
    LAi_SetPlayerType(pchar);
	locCameraTarget(pchar);
    locCameraFollow();
    EndQuestMovie();
	ChangeShowIntarface();
	bDisableCharacterMenu = false;

	sld = CharacterFromID("Gang_0");
	sld.Dialog.CurrentNode = "CaptainComission_328";
	LAi_SetActorType(pchar);
	LAi_ActorTurnToCharacter(pchar, sld);
	LAi_SetActorType(sld);
	SetActorDialogAny2Pchar(sld.id, "", 0.0, 1.0);
	LAi_ActorFollow(sld, pchar, "ActorDialog_Any2Pchar", -1);
}

void CaptainComission_GangExit()
{
	ref sld;
	int iTemp;
	float locx, locy, locz;

	DoQuestCheckDelay("OpenTheDoors", 1.0);
	LAi_SetPlayerType(pchar);
	GetCharacterPos(pchar, &locx, &locy, &locz);
    for (int i = 0; i < sti(pchar.GenQuest.CaptainComission.GangNum); i++)
	{
		sld = characterFromID("Gang_" + i);
		LAi_SetActorType(sld);
		LAi_ActorGoToLocation(sld, "reload", LAi_FindNearestLocator("reload", locx, locy, locz), "none", "", "", "", -1.0);
		iTemp = GetCharacterIndex("Gang_" + i);
		if (iTemp != -1)
		{
			characters[iTemp].lifeDay = 0;
		}
	}
	sld = characterFromID("CapComission_1");
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", LAi_FindNearestLocator("reload", locx, locy, locz), "none", "", "", "", -1.0);
	sld.lifeDay = 0;
	pchar.quest.CaptainComission_ExitFromShoreAfterGang.win_condition.l1 = "MapEnter";
    pchar.quest.CaptainComission_ExitFromShoreAfterGang.function = "CaptainComission_ExitFromShoreAfterGang";
	InterfaceStates.Buttons.Save.enable = true;
	RemoveCharacterCompanion(pchar, sld);
}

void CaptainComission_ExitFromShoreAfterGang(string qName)
{
	if (GetCharacterIndex("CapComission_2") > 0)
	{
		characters[GetCharacterIndex("CapComission_2")].lifeDay = 0;
	}
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_ChangeCaptain()
{
	int iRank = sti(pchar.rank) + 5 + rand(MOD_SKILL_ENEMY_RATE*3);
	int iNation = sti(pchar.GenQuest.CaptainComission.Nation);
	aref	arShip1, arShip2;
	ref chref, sld;

	chref = GetCharacter(NPC_GenerateCharacter("CapComission_2", "officer_"+(rand(42)+1), "man", "man", iRank, iNation, -1, true));
	chref.name = GenerateRandomName_Generator(sti(chref.nation), "man");
	chref.lastname = "";
	SetCharacterPerk(chref, "MusketsShoot");
	SetCharacterPerk(chref, "Energaiser");
	FantomMakeCoolFighter(chref, makeint(pchar.rank) + rand(10) + 5, 90, 50, "blade25", "pistol3", 80);
	chref.cirassId = Items_FindItemIdx("cirass5");  // предмета нет, но влияение есть
	LAi_SetHP(chref, 200 + makeint(pchar.rank) * 5, 200 + makeint(pchar.rank) * 5);

	DeleteAttribute(chref, "ship");
	chref.ship = "";

	sld = CharacterFromID("CapComission_1");
	if (CheckAttribute(sld, "curshipnum"))
	{
		int curshipnum = sti(sld.curshipnum);
		chref.curshipnum = curshipnum;
		DeleteAttribute(sld, "curshipnum");
	}
	makearef(arShip1, sld.Ship);
	makearef(arShip2, chref.Ship);
	CopyAttributes(arShip2,arShip1);
	DeleteAttribute(sld, "ship");
	sld.ship.type = SHIP_NOTUSED;
	SeaAI_SetOfficer2ShipAfterAbordage(chref, sld);

	RemoveCharacterCompanion(pchar, sld);

	Group_DelCharacter(PLAYER_GROUP, sld.id);

	string sGroup = "Sea_CapComission_2";
	Group_FindOrCreateGroup(sGroup);
    Group_SetType(sGroup, "war");

	Group_AddCharacter(sGroup, "CapComission_2");
    Group_SetGroupCommander(sGroup, "CapComission_2");
	Group_SetAddress(sGroup, Islands[GetCharacterCurrentIsland(Pchar)].id, "", "");
	Group_SetPursuitGroup(sGroup, PLAYER_GROUP);

	chref.nation = PIRATE;
	Ship_NationAgressivePatent(chref);
	Ship_FlagRefresh(chref); //флаг на лету

	SetCharacterRelationBoth(sti(chref.index), GetMainCharacterIndex(), RELATION_ENEMY);
	DeleteAttribute(chref, "AlwaysFriend");
	DeleteAttribute(chref,"Abordage.Enable");

	Group_SetTaskAttack(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);
}

void CaptainComission_GangDied(string qName)
{
	if(pchar.GenQuest.CaptainComission.variant == "A1")
	{
		AddQuestRecord("CaptainComission2", "16");
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		AddQuestUserData("CaptainComission2", "sShoreName", UpperFirst(XI_ConvertString(pchar.GenQuest.CaptainComission.ConvoyShore + "Voc")));
		CloseQuestHeader("CaptainComission2");
		pchar.quest.CaptainComission_MapEnterAfterGang.win_condition.l1 = "MapEnter";
		pchar.quest.CaptainComission_MapEnterAfterGang.function = "CaptainComission_ExitFromShoreAfterGang";
	}
	if(pchar.GenQuest.CaptainComission.variant == "A2")
	{
		ref sld = CharacterFromId("CapComission_1");
		if (LAi_IsDead(sld)) // погиб
		{
			AddQuestRecord("CaptainComission2", "33");
			AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
			CloseQuestHeader("CaptainComission2");
			ChangeCharacterReputation(pchar, -3);
			OfficersReaction("bad");
			DeleteAttribute(pchar, "GenQuest.CaptainComission");
		}
		else
		{
			sld.dialog.currentnode = "CaptainComission_401";
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
		}
	}
	InterfaceStates.Buttons.Save.enable = true;
}

void CaptainComission_ConvoyTimeIsOut(string qName)
{
	pchar.quest.CaptainComission_ConvoyInShore.over = "yes";
	pchar.quest.CaptainComission_Ship_Sink.over = "yes";
	ref sld = CharacterFromId("CapComission_1");
	RemoveCharacterCompanion(pchar, sld);
	AddQuestRecord("CaptainComission2", "14");
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	AddQuestUserData("CaptainComission2", "sShoreName", XI_ConvertString(pchar.GenQuest.CaptainComission.ConvoyShore + "Acc"));
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_Ship_Sink(string qName)
{
	pchar.quest.CaptainComission_ConvoyInShore.over = "yes";
	pchar.quest.CaptainComission_ConvoyTimeIsOut.over = "yes";
	AddQuestRecord("CaptainComission2", "13");
	AddQuestUserData("CaptainComission2", "sShipType", GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name"))));
	AddQuestUserData("CaptainComission2", "sShipName", pchar.GenQuest.CaptainComission.ShipTypeName);
	AddQuestUserData("CaptainComission2", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.CaptainComission.Goods)].Name + "Gen")));
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

// ветка №2
void CaptainComission_GenerateRumourCaptainPrison()
{
	string sShipType = GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc"));
	AddSimpleRumourCity(LinkRandPhrase(GetConvertStrWithReplace("Variable_GenQuests_8", "AddRumours.txt", "#space#", " ") + GetAddress_Form(pchar) + GetConvertStrWithReplace("Variable_GenQuests_9", "AddRumours.txt", "#space#", " ") + sShipType + " " + pchar.GenQuest.CaptainComission.Name + GetConvertStrWithReplace("Variable_GenQuests_10", "AddRumours.txt", "#space#", " "),
		GetConvertStrWithReplace("Variable_GenQuests_11", "AddRumours.txt", "#space#", " ") + sShipType + " " + pchar.GenQuest.CaptainComission.Name + GetConvertStrWithReplace("Variable_GenQuests_12", "AddRumours.txt", "#space#", " ") + GetAddress_Form(pchar) + GetConvertStrWithReplace("Variable_GenQuests_13", "AddRumours.txt", "#space#", " "),
		RandPhraseSimple(GetConvertStrWithReplace("Variable_GenQuests_14", "AddRumours.txt", "#space#", " ") + pchar.GenQuest.CaptainComission.Name + GetConvertStrWithReplace("Variable_GenQuests_15", "AddRumours.txt", "#space#", " ") + GetAddress_Form(pchar) + GetConvertStrWithReplace("Variable_GenQuests_16", "AddRumours.txt", "#space#", " "),
		GetConvertStrWithReplace("Variable_GenQuests_17", "AddRumours.txt", "#space#", " ") + sShipType + " " + pchar.GenQuest.CaptainComission.Name + GetConvertStrWithReplace("Variable_GenQuests_18", "AddRumours.txt", "#space#", " "))),
		pchar.GenQuest.CaptainComission.City, 5, 3, "CaptainComission_rumourCapPrison");
}

void CaptainComission_RumourCaptainPrison()
{
	if(!CheckAttribute(pchar,"GenQuest.CaptainComission.GetRumour"))
	{
		AddQuestRecord("CaptainComission2", "18");
		AddQuestUserData("CaptainComission2", "sShipType", GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")));
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		pchar.GenQuest.CaptainComission.GetRumour = true;
		SaveCurrentQuestDateParam("GenQuest.CaptainComission.GetRumour");
		SetFunctionTimerConditionParam("CaptainComission_TimeIsOver", 0, 0, 2, MakeInt(24 - GetHour()), false);
	}
}

void CaptainComission_GetSecretTimeIsOut(string qName)
{
	AddQuestRecord("CaptainComission2", "20");
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	AddQuestUserData("CaptainComission2", "sCity", XI_ConvertString("Colony" + pchar.GenQuest.CaptainComission.City + "Gen"));
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_GenerateCaptainInPrison()
{
	ref sld = GetCharacter(NPC_GenerateCharacter("CapComission_1" , "officer_12", "man", "man", 10, sti(pchar.GenQuest.CaptainComission.Nation), -1, true));
	SetSPECIAL(sld, 10,9,10,5,10,9,10);
	SetRandSelfSkill(sld, 75, 95);
	SetRandShipSkill(sld, 75, 95);
	FantomMakeCoolFighter(sld, sti(pchar.rank) + rand(15), 95, 95, LinkRandPhrase("blade22","blade34","blade21"), "pistol2", 10);
	sld.name = pchar.GenQuest.CaptainComission.Name;
	sld.lastname = "";
	sld.greeting = "Gr_prison";
	sld.dialog.FileName = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "CaptainComission_370";
	RemoveAllCharacterItems(sld, true);
	ChangeCharacterAddress(sld, "none", "");
}

void CaptainComission_ExitFromTown(string qName)
{
	bQuestDisableMapEnter = true;
	QuestCloseSeaExit();
    DoQuestCheckDelay("CaptainComission_MeetWithCaptain", 1);
}

void CaptainComission_CapEscapeTimer(string qName)
{
	Log_QuestInfo(GetConvertStrWithReplace("Variable_GenQuests_37", "Logs.txt", "#space#", " "));
	ref sld = characterFromId("CapComission_1");
	RemovePassenger(pchar, sld);
	ChangeCharacterAddress(sld, "none", "");
	sld.lifeDay = 0;
	pchar.GenQuest.CaptainComission = "CapEscape";
	pchar.GenQuest.CaptainComission.canSpeakBoatswain = true;
	pchar.TownEscape = true;
	pchar.quest.CaptainComission_ExitFromTown.win_condition.l1 	= "EnterToSea";
	pchar.quest.CaptainComission_ExitFromTown.function    		= "CaptainComission_ExitFromTown";
}

void CaptainComission_GangDialogGetGoods()
{
	ref sld;
	for (int i = 0; i < sti(pchar.GenQuest.CaptainComission.GangNum); i++)
	{
		sld = characterFromID("Gang_" + i);
		LAi_SetActorType(sld);
		if (i == 0) LAi_ActorDialog(sld, pchar, "", -1, 0);
		else LAi_ActorFollow(sld, &characters[GetCharacterIndex("Gang_0")], "", -1);
	}
}

void CaptainComission_CheckCaptainAfterBattle(string qName)
{
	ref sld = characterFromId("CapComission_1");
	if (LAi_IsDead(sld)) // погиб
	{
		AddQuestRecord("CaptainComission2", "22");
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		CloseQuestHeader("CaptainComission2");
		DeleteAttribute(pchar, "GenQuest.CaptainComission");
	}
	else // кэп выжил
	{
		if(GetCharacterEquipByGroup(sld, BLADE_ITEM_TYPE) == "") // снял клинок с трупа
		{
			string sEquipItem = GenQuest_GenerateGoodBlade();
			AddItems(sld, sEquipItem, 1);
			EquipCharacterByItem(sld, sEquipItem);
		}
		sld.dialog.currentnode = "CaptainComission_387";
		LAi_SetActorType(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);
	}
}

void CaptainComission_CheckCaptainDied(string qName)
{
	if(CheckAttribute(pchar,"GenQuest.CaptainComission.MayorTalkGood"))
	{
		AddQuestRecord("CaptainComission2", "28");
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		AddQuestUserData("CaptainComission2", "sCity", XI_ConvertString("Colony" + pchar.GenQuest.CaptainComission.City + "Gen"));
		pchar.GenQuest.CaptainComission = "GetGoodsSuccess";
		OfficersReaction("good");
		SetFunctionTimerConditionParam("CaptainComission_SpeakMayorGoods", 0, 0, 1, MakeInt(24 - GetHour()), false);
	}
	else
	{
		AddQuestRecord("CaptainComission2", "27");
		AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		AddQuestUserData("CaptainComission2", "sGoodsQuantity", pchar.GenQuest.CaptainComission.MaxGoodsQty);
		AddQuestUserData("CaptainComission2", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.CaptainComission.Goods)].Name + "Gen")));
		CloseQuestHeader("CaptainComission2");
		int iGoods = GetSquadronFreeSpace(pchar, sti(pchar.GenQuest.CaptainComission.Goods));
		if(iGoods < sti(pchar.GenQuest.CaptainComission.GoodsQty)) pchar.GenQuest.CaptainComission.GoodsQty = iGoods;
		SetCharacterGoods(pchar, sti(pchar.GenQuest.CaptainComission.Goods), GetCargoGoods(pchar, sti(pchar.GenQuest.CaptainComission.Goods)) + sti(pchar.GenQuest.CaptainComission.GoodsQty));// перегруз
		OfficersReaction("bad");
		ChangeCharacterReputation(pchar, -5);
		CaptainComission_GenerateCoastalPatrol();
		pchar.quest.CaptainComission_MapEnter.win_condition.l1 = "MapEnter";
		pchar.quest.CaptainComission_MapEnter.win_condition = "CaptainComission_MapEnterCapDied";
	}
}

void CaptainComission_SpeakMayorGoods(string qName)
{
	AddQuestRecord("CaptainComission2", "29");
	AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	pchar.GenQuest.CaptainComission = "GetGoods";
}

void CaptainComission_CapMeetInShore(string qName)
{
	int DaysIsLeft = GetQuestPastDayParam("GenQuest.CaptainComission.DeleiveCapShore");

	ref sld = characterFromId("CapComission_1");
	RemovePassenger(pchar, sld);
	PlaceCharacter(sld, "goto", "random_must_be_near");
	if(DaysIsLeft <= sti(pchar.GenQuest.CaptainComission.iDay))
	{
		LAi_SetActorTypeNoGroup(sld);
		CaptainComission_GenerateGangInShore();
	}
	else
	{
		sld.dialog.currentnode = "CaptainComission_392";
		LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);
	}
}

void CaptainComission_CaptainDiedDuel(string qName)
{
	if(CheckAttribute(pchar, "GenQuest.CaptainComission.Duel"))
	{
		AddQuestRecord("CaptainComission2", "33");
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	}
	else
	{
		AddQuestRecord("CaptainComission2", "31");
		AddQuestUserData("CaptainComission2", "sShoreName", XI_ConvertString(pchar.GenQuest.CaptainComission.ConvoyShore + "Voc"));
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	}
	CloseQuestHeader("CaptainComission2");
	ChangeCharacterReputation(pchar, -8);
	OfficersReaction("bad");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_BattleInShore(string qName)
{
	ref sld;
	string sTemp = "Gang_";
	string sGroup = "GangGroup_0";
	LAi_LocationFightDisable(&locations[FindLocation(pchar.GenQuest.CaptainComission.ConvoyShore)], false);
	int iTemp = sti(pchar.GenQuest.CaptainComission.GangNum);
	chrDisableReloadToLocation = true;
	sld = CharacterFromID("CapComission_1");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);

	for(int i = 0; i < iTemp; i++)
	{
		sld = CharacterFromID(sTemp + i);
		LAi_SetWarriorType(sld);
		LAi_group_MoveCharacter(sld, sGroup);
		LAi_SetImmortal(sld, false);
	}
	LAi_group_SetRelation(sGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups(sGroup, LAI_GROUP_PLAYER, true);
	LAi_group_SetCheck(sGroup, "OpenTheDoors");
	LAi_group_SetCheckFunction(sGroup, "CaptainComission_GangDied");
	LAi_SetFightMode(pchar, true);
}

bool CaptainComission_CheckAllPassengersHired()
{
    int iQty = GetPassengersQuantity(pchar);
    int i = 0;
	int n = 0;
	int idx;
	ref ch;

 	while (i < iQty)
	{
		i++;
		idx = GetPassenger(pchar, n);
		if (idx == -1) break;
		ch = GetCharacter(idx);
		if(CheckAttribute(ch, "CaptainComission")) return true;
		n++;
	}
    for (i=0; i<COMPANION_MAX; i++)
	{
		idx = GetCompanionIndex(pchar, i);
		if (idx != -1)
		{
            ch = GetCharacter(idx);
			if(CheckAttribute(ch, "CaptainComission")) return true;
		}
	}
	return false;
}

// ветка №3
void CaptainComission_GenerateRumourCaptainDeath()
{
	string sShipType = GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc"));
	AddSimpleRumourCity(LinkRandPhrase(GetConvertStrWithReplace("Variable_GenQuests_19", "AddRumours.txt", "#space#", " ") + GetAddress_Form(pchar) + GetConvertStrWithReplace("Variable_GenQuests_20", "AddRumours.txt", "#space#", " ") + sShipType +" " + pchar.GenQuest.CaptainComission.Name + GetConvertStrWithReplace("Variable_GenQuests_21", "AddRumours.txt", "#space#", " "),
		GetConvertStrWithReplace("Variable_GenQuests_22", "AddRumours.txt", "#space#", " ") + GetAddress_Form(pchar) + GetConvertStrWithReplace("Variable_GenQuests_23", "AddRumours.txt", "#space#", " ") + sShipType + " " + pchar.GenQuest.CaptainComission.Name + GetConvertStrWithReplace("Variable_GenQuests_24", "AddRumours.txt", "#space#", " "),
		RandPhraseSimple(GetConvertStrWithReplace("Variable_GenQuests_25", "AddRumours.txt", "#space#", " ") + GetAddress_Form(pchar) + GetConvertStrWithReplace("Variable_GenQuests_26", "AddRumours.txt", "#space#", " "),
		GetConvertStrWithReplace("Variable_GenQuests_27", "AddRumours.txt", "#space#", " ") + pchar.GenQuest.CaptainComission.Name + GetConvertStrWithReplace("Variable_GenQuests_28", "AddRumours.txt", "#space#", " ") + sShipType + GetConvertStrWithReplace("Variable_GenQuests_29", "AddRumours.txt", "#space#", " ") + GetAddress_Form(pchar) +GetConvertStrWithReplace("Variable_GenQuests_30", "AddRumours.txt", "#space#", " "))),
	pchar.GenQuest.CaptainComission.City, 5, 3, "CaptainComission_rumourCapDeath");
}

void CaptainComission_RumourCaptainDeath()
{
	if(!CheckAttribute(pchar,"GenQuest.CaptainComission.GetRumour"))
	{
		AddQuestRecord("CaptainComission2", "37");
		AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("CaptainComission2", "sShipType", GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")));
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		pchar.GenQuest.CaptainComission.GetRumour = true;
		SaveCurrentQuestDateParam("GenQuest.CaptainComission.GetRumour");
	}
}

void CaptainComission_GenerateCanoneer(string qName)
{
	int Rank = sti(pchar.rank) + 3 + rand(10);
	ref sld = GetCharacter(NPC_GenerateCharacter("CapComission_Canoneer", "officer_15", "man", "man", 10, sti(pchar.GenQuest.CaptainComission.Nation), -1, true));
	FantomMakeCoolFighter(sld, Rank, 100, 90, "blade9", "pistol3", 150);
	sld.name = GetName( NAMETYPE_ORIG, pchar.GenQuest.CaptainComission.CanoneerName, NAME_NOM);
	sld.lastname = "";
	sld.dialog.FileName = "GenQuests_Dialog.c";
	sld.dialog.currentnode = "CaptainComission_Canoneer";
	sld.greeting = "cit_quest";
	sld.CityType = "citizen";
	sld.City = pchar.GenQuest.CaptainComission.City;
	LAi_SetImmortal(sld, true);	// До поры нельзя убить
	string sLoc = pchar.GenQuest.CaptainComission.City + "_tavern";
	ref pLoc = &locations[FindLocation(sLoc)];
	if (!LAi_CheckLocatorFree("sit", "sit_front2")) FreeSitLocator(sLoc, "sit_front2");
	FreeSitLocator(sLoc, "sit_base2");
	ChangeCharacterAddressGroup(sld, sLoc, "sit", "sit_base2");
	LAi_SetSitType(sld);
}

// канонир ждет деньги до конца суток .. если бабла нет, то закрываем квест с записью в СЖ
void CaptainComission_CanoneerWaitMoney(string qName)
{
	ref sld = characterFromId("CapComission_Canoneer");
	ChangeCharacterAddress(sld, "none", "");
	sld.LifeDay = 0;
	ChangeOfficersLoyality("bad_all", 1);
	AddQuestRecord("CaptainComission2", "42");
	AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	AddQuestUserData("CaptainComission2", "sCharName", GetName( NAMETYPE_ORIG, pchar.GenQuest.CaptainComission.CanoneerName, NAME_NOM));
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_MeetCanoneerInShoreTimer(string qName)
{
	SetFunctionLocationCondition("CaptainComission_MeetCanoneerInShore", pchar.GenQuest.CaptainComission.ConvoyShore, false);
}

void CaptainComission_MeetCanoneerInShore(string qName)
{
	pchar.quest.CaptainComission_TimeIsOver.over = "yes";
	pchar.quest.CaptainComission_CanoneerWaitMoney.over = "yes";
	if(pchar.Location == pchar.location.from_sea)
	{
		chrDisableReloadToLocation = true;
		CaptainComission_GenerateGangInShore();
		ref sld = characterFromId("CapComission_Canoneer");
		LAi_SetImmortal(sld, true);
		LAi_CharacterEnableDialog(sld);
		sld.dialog.currentnode = "CaptainComission_350";
		PlaceCharacter(sld, "goto", "random_must_be_near");
		LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);
	}
	else // нефик тупить
	{
		sld = CharacterFromID("CapComission_Canoneer");
		sld.LifeDay = 0;
		AddQuestRecord("CaptainComission2", "44");
		AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		AddQuestUserData("CaptainComission2", "sCharName", GetName( NAMETYPE_ORIG, pchar.GenQuest.CaptainComission.CanoneerName, NAME_NOM));
		CloseQuestHeader("CaptainComission2");
		ChangeOfficersLoyality("bad_all", 1);
		DeleteAttribute(pchar, "GenQuest.CaptainComission");
	}
}

void CaptainComission_NoGangDialog(string qName)
{
	ref sld;
	int iTemp;
	float locx, locy, locz;
	string sTemp;

	GetCharacterPos(pchar, &locx, &locy, &locz);
	sTemp = LAi_FindNearestLocator("reload", locx, locy, locz);
	sld = characterFromId("CapComission_Canoneer");
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", sTemp, "none", "", "", "OpenTheDoors", -1.0);
	sld.LifeDay = 0;
	for(int i = 0; i < sti(pchar.GenQuest.CaptainComission.GangNum); i++)
	{
		iTemp = GetCharacterIndex("Gang_" + i);
		if (iTemp != -1)
		{
			sld = characterFromId("Gang_" + i);
			LAi_SetActorType(sld);
			LAi_ActorGoToLocation(sld, "reload", sTemp, "none", "", "", "", -1.0);
			sld.lifeDay = 0;
		}
	}
	AddQuestRecord("CaptainComission2", "45");
	AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
	AddQuestUserData("CaptainComission2", "sCharName", GetName( NAMETYPE_ORIG, pchar.GenQuest.CaptainComission.CanoneerName, NAME_NOM));
	CloseQuestHeader("CaptainComission2");
	DeleteAttribute(pchar, "GenQuest.CaptainComission");
}

void CaptainComission_CheckGangAfterBattle(string qName)
{
	ref sld;
	int iGoods = GetSquadronFreeSpace(pchar, sti(pchar.GenQuest.CaptainComission.Goods));
	if(iGoods < sti(pchar.GenQuest.CaptainComission.GoodsQty)) pchar.GenQuest.CaptainComission.GoodsQty = iGoods;
	SetCharacterGoods(pchar, sti(pchar.GenQuest.CaptainComission.Goods), GetCargoGoods(pchar, sti(pchar.GenQuest.CaptainComission.Goods)) + sti(pchar.GenQuest.CaptainComission.GoodsQty));// перегруз
	if (GetCharacterIndex("CapComission_Canoneer") > 0) // канонир остался жив
	{
		sld = CharacterFromId("CapComission_Canoneer");
		AddPassenger(pchar, sld, false);
		SetCharacterRemovable(sld, false);
		sld.dialog.currentnode = "CaptainComission_365";
		LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", -1, 0);
		AddQuestRecord("CaptainComission2", "47");
		AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		AddQuestUserData("CaptainComission2", "sSum", iGoods);
		AddQuestUserData("CaptainComission2", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.CaptainComission.Goods)].Name + "Gen")));
		AddQuestUserData("CaptainComission2", "sCharName", GetName( NAMETYPE_ORIG, pchar.GenQuest.CaptainComission.CanoneerName, NAME_ACC));
		AddQuestUserData("CaptainComission2", "sCity", XI_ConvertString("Colony" + pchar.GenQuest.CaptainComission.CanoneerCity));
		SetFunctionLocationCondition("CaptainComission_DeleiveCanoneer", pchar.GenQuest.CaptainComission.CanoneerCity + "_town", false);
	}
	else // помер .. вот бедолага
	{
		AddQuestRecord("CaptainComission2", "46");
		AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
		AddQuestUserData("CaptainComission2", "sCharName", GetName( NAMETYPE_ORIG, pchar.GenQuest.CaptainComission.CanoneerName, NAME_NOM));
		AddQuestUserData("CaptainComission2", "sShipType", GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")));
		AddQuestUserData("CaptainComission2", "sShipName", pchar.GenQuest.CaptainComission.ShipTypeName);
		AddQuestUserData("CaptainComission2", "sSum", iGoods);
		AddQuestUserData("CaptainComission2", "sGoods", GetStrSmallRegister(XI_ConvertString(Goods[sti(pchar.GenQuest.CaptainComission.Goods)].Name + "Gen")));
		CloseQuestHeader("CaptainComission2");
		OfficersReaction("bad");
		ChangeCharacterReputation(pchar, -5);
	}
	CaptainComission_EnterToSeaAfterShoreBattle();
	pchar.quest.CaptainComission_MapEnterAfterShoreBattle.win_condition.l1 = "MapEnter";
    pchar.quest.CaptainComission_MapEnterAfterShoreBattle = "CaptainComission_MapEnterAfterShoreBattle";
}

void CaptainComission_DeleiveCanoneer(string qName)
{
	ref sld = characterFromId("CapComission_Canoneer");
	RemovePassenger(pchar, sld);
	sld.dialog.currentnode = "CaptainComission_366";
	PlaceCharacter(sld, "goto", pchar.location);
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void CaptainComission_EnterToSeaAfterShoreBattle()
{
	ref sld;
	Group_DeleteGroup("Pirate_Guards");
	Group_FindOrCreateGroup("Pirate_Guards");
    Group_SetType("Pirate_Guards", "pirate");
	for (int i = 1; i <= 3; i++)
    {
		sld = GetCharacter(NPC_GenerateCharacter("Pirate_Captain0" + i, "officer_"+(rand(42)+1), "man", "man", 5, PIRATE, 3, true));
		FantomMakeCoolFighter(sld, makeint(pchar.rank) + rand(10) + 5, 90, 50, "blade25", "pistol3", 80);
		SetFantomParam(sld);
		SelectCoastalGuardShip(sld);
		sld.AlwaysEnemy = true;
		sld.DontRansackCaptain = true;
		sld.AlwaysSandbankManeuver = true;
		Group_AddCharacter("Pirate_Guards", sld.id);
		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
    }
	Group_SetGroupCommander("Pirate_Guards", "Pirate_Captain01");
	Group_SetPursuitGroup("Pirate_Guards", PLAYER_GROUP);
	Group_SetAddress("Pirate_Guards", Islands[GetCharacterCurrentIsland(Pchar)].id, "", "");
	Group_SetTaskAttack("Pirate_Guards", PLAYER_GROUP);
	Group_LockTask("Pirate_Guards");
}
//=====================================================================================================================================
// <-- ГЕНЕРАТОР  "ПОРУЧЕНИЕ КАПИТАНА - ОПЕРАЦИЯ 'ГАЛЕОН'"
//=====================================================================================================================================