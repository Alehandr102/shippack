// Gregg: вынесено из reaction_functions.c (сплит god-файла) — семейство квеста «Работорговец» (Slavetrader).
// Файл ТЕКСТОВО включается в reaction_functions.c на исходное место (#include) — видимость и поведение идентичны.

void Slavetrader_SetBankAccess(string city, bool disable)
{
	int town = FindLocation(city + "_town");
	int bank = FindLocation(city + "_bank");
	if (town < 0 || bank < 0) return;
	if (!CheckAttribute(&Locations[town], "reload")) return;
	aref reloads, door;
	makearef(reloads, Locations[town].reload);
	int count = GetAttributesNum(reloads);
	for (int i = 0; i < count; i++)
	{
		door = GetAttributeN(reloads, i);
		if (!CheckAttribute(door, "go") || !CheckAttribute(door, "name")) continue;
		// Номер локатора различается между городами; выбираем переход в нужный банк.
		if (FindLocation(door.go) == bank)
			LocatorReloadEnterDisable(city + "_town", door.name, disable);
	}
}

void Slavetrader_GetRandomShore()//выбор бухты
{
	switch (rand(4))
	{
		case 0:
			pchar.questTemp.Slavetrader.Island = "Terks";
			break;
		case 1:
			pchar.questTemp.Slavetrader.Island = "Caiman";
			break;
		case 2:
			pchar.questTemp.Slavetrader.Island = "StLucia";
			break;
		case 3:
			pchar.questTemp.Slavetrader.Island = "Dominica";
			break;
		case 4:
			pchar.questTemp.Slavetrader.Island = "Bermudes";
			break;
	}
	pchar.questTemp.Slavetrader.Island.Shore = GetIslandRandomShoreId(pchar.questTemp.Slavetrader.Island);
	while(pchar.questTemp.Slavetrader.Island.Shore == "")
	{
		pchar.questTemp.Slavetrader.Island = GetRandomIslandId();
		pchar.questTemp.Slavetrader.Island.Shore = GetIslandRandomShoreId(pchar.questTemp.Slavetrader.Island);
		if (!isLocationFreeForQuests(pchar.questTemp.Slavetrader.Island)) pchar.questTemp.Slavetrader.Island.Shore = "";//уж лучше фик куда, чем глюки и конфликты
	}
}

void Slavetrader_SlaveShipsOver(string qName)//просроченный таймер
{
	pchar.quest.Slavetrader_ShipsAttack.over = "yes";
	AddQuestRecord("Slavetrader", "7");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
	pchar.questTemp.Slavetrader = "goaway";
	ChangeOfficersLoyality("bad_all", 1);
}

void Slavetrader_ShoreShipsOver(string qName)//просроченный таймер пинаса
{
	pchar.quest.Slavetrader_ShoreAttack.over = "yes";
	AddQuestRecord("Slavetrader", "7_1");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
	pchar.questTemp.Slavetrader = "goaway_pinas";
	ChangeOfficersLoyality("bad_all", 1);
}

void Slavetrader_CreateSlaveShips(string qName)//создание кораблей в бухте
{
	int i, ShipType, Rank, iShipRank, iCannonType;
	ref sld;
	string Blade, sTemp;

	sTemp = pchar.questTemp.Slavetrader.ShipName;//имя корабля
    Island_SetReloadEnableGlobal(pchar.questTemp.Slavetrader.Island, false);
    Group_FindOrCreateGroup("Slave_Attack");//создать группу
	Group_SetType("Slave_Attack", "war");//тип группы
	// ==> Контрабандист
	for (i=1; i<=2; i++)//постоянно такая структура
	{
		Rank = sti(pchar.rank) + 5 + rand(MOD_SKILL_ENEMY_RATE);
		if(makeint(pchar.rank) >= 20) { iShipRank = 4; }
		if(makeint(pchar.rank) >= 13 && makeint(pchar.rank) < 20) { iShipRank = rand(1)+2; }
		if(makeint(pchar.rank) >= 7 && makeint(pchar.rank) < 13) { iShipRank = rand(1); }
		if(makeint(pchar.rank) < 7) { iShipRank = 0; }
		switch (iShipRank)
		{
			case 0:
				ShipType = SHIP_BRIG;
				Rank = 12 + rand(5);
                Blade = "blade15";
				iCannonType = CANNON_TYPE_CANNON_LBS24;
			break;
			case 1:
				ShipType = SHIP_BRIGANTINE;
				Rank = 15 + rand(5);
                Blade = "blade21";
				iCannonType = CANNON_TYPE_CANNON_LBS24;
			break;
			case 2:
				ShipType = SHIP_CORVETTE;
				Rank = 20 + rand(5);
                Blade = "blade21";
				iCannonType = CANNON_TYPE_CANNON_LBS32;
			break;
			case 3:
				ShipType = SHIP_FRIGATE;
				Rank = 25 + rand(5);
                Blade = "blade21";
				iCannonType = CANNON_TYPE_CANNON_LBS32;
			break;
			case 4:
				ShipType = SHIP_LINESHIP;
				Rank = 30 + rand(5);
                Blade = "blade33";
				iCannonType = CANNON_TYPE_CANNON_LBS32;
			break;
		}
		sld = GetCharacter(NPC_GenerateCharacter("CaptainSlaveAttack_"+i, "officer_"+(rand(42)+1), "man", "man", Rank, PIRATE, 3, true));//создание кэпа
		if (i == 1)
		{
		FantomMakeCoolSailor(sld, ShipType, "", iCannonType, 70, 50, 50);//создание кораблей
		FantomMakeCoolFighter(sld, Rank, 40, 40, Blade, "pistol2", 30);//создание фантома кэпа
		}
		if (i == 2)
		{
		FantomMakeCoolSailor(sld, SHIP_ROTERLOW, sTemp, CANNON_TYPE_CANNON_LBS24, 70, 50, 50); //создание кораблей
		FantomMakeCoolFighter(sld, Rank, 40, 40, "blade16", "pistol2", 30);//создание фантома кэпа
		}

		Group_AddCharacter("Slave_Attack", "CaptainSlaveAttack_"+i);//добавление в группу
		sld.SuperShooter  = true;
		SetCharacterPerk(sld, "Tireless");
		SetCharacterPerk(sld, "HardHitter");
		SetCharacterPerk(sld, "GunProfessional");
		SetCharacterGoods(sld, GOOD_SLAVES, 700 + rand(100));//положить в трюм рабов

		sld.ship.Crew.Morale = 50;
		ChangeCrewExp(sld, "Sailors", 70);
		ChangeCrewExp(sld, "Cannoners", 70);
		ChangeCrewExp(sld, "Soldiers", 50);
    }
    // ==> стравливание
	Group_SetGroupCommander("Slave_Attack", "CaptainSlaveAttack_1");
	Group_SetTaskAttack("Slave_Attack", PLAYER_GROUP);
	Group_SetPursuitGroup("Slave_Attack", PLAYER_GROUP);
	Group_SetAddress("Slave_Attack", pchar.questTemp.Slavetrader.Island, "", "");
	Group_LockTask("Slave_Attack");

    // ==> прерывание на убиение эскадры
    pchar.quest.Slavetrader_AfterBattle.win_condition.l1 = "Group_Death";//условие победы
	pchar.quest.Slavetrader_AfterBattle.win_condition.l1.group = "Slave_Attack";//какую группу уничтожить
	pchar.quest.Slavetrader_AfterBattle.function = "Slavetrader_SlaveAttack_AfterBattle";//это победа
	pchar.quest.Slavetrader_DieHard.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.Slavetrader_DieHard.function = "Slavetrader_SlaveAttack_DieHard";//это слинял

	AddQuestRecord("Slavetrader", "3_1");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
}

// Gregg: финализатор квеста работорговца — было 8 идентичных пар (закрытие журнала + флаг questdata)
void FinishSlavetraderQuest()
{
	CloseQuestHeader("Slavetrader");
	pchar.questdata.Rabotorgovets = 1;
}

void Slavetrader_SlaveAttack_AfterBattle(string qName)//реакция на победу
{
	Group_DeleteGroup("Slave_Attack");
	Island_SetReloadEnableGlobal(pchar.questTemp.Slavetrader.Island, true);
    pchar.quest.Slavetrader_DieHard.over = "yes";
	pchar.quest.Slavetrader_SlaveShipsOver.over = "yes";
	AddQuestRecord("Slavetrader", "4");
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
	pchar.questTemp.Slavetrader = "died";//нода на диалог
}

void Slavetrader_SlaveAttack_DieHard(string qName)//реакция на поражение
{
	Group_DeleteGroup("Slave_Attack");
    Island_SetReloadEnableGlobal(pchar.questTemp.Slavetrader.Island, true);
    pchar.quest.Slavetrader_AfterBattle.over = "yes";
	pchar.quest.Slavetrader_SlaveShipsOver.over = "yes";
	AddQuestRecord("Slavetrader", "5");
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
	pchar.questTemp.Slavetrader = "goaway";//нода на диалог
	ChangeOfficersLoyality("bad_all",1);
}


void Slavetrader_CreateShoreShips(string qName)//создание пинаса в бухте
{
	int hcrew;
	ref sld;
	string sTemp, sNation;

	sNation = pchar.questTemp.Slavetrader.Nation;//нация
	sTemp = pchar.questTemp.Slavetrader.ShipName;//имя корабля
	pchar.questTemp.Slavetrader = "TakeShoreCap";//направление в диалоговый файл
    Island_SetReloadEnableGlobal(pchar.questTemp.Slavetrader.Island, false);
    Group_FindOrCreateGroup("Shore_Attack");//создать группу
	Group_SetType("Shore_Attack", "war");//тип группы
	sld = GetCharacter(NPC_GenerateCharacter("Slaveshorecap", "trader_"+(rand(15)+1), "man", "man", 25, sNation, 3, true));//создание кэпа
		sld.DontRansackCaptain = true;
		FantomMakeCoolSailor(sld, SHIP_PINNACE, sTemp, CANNON_TYPE_CANNON_LBS24, 100, 80, 80);//создание кораблей
		sld.Ship.Mode = "trade";
		sld.Nation = sti(sNation);
		FantomMakeCoolFighter(sld, 30, 50, 50, "blade21", "pistol3", 50);//создание фантома кэпа
		Group_AddCharacter("Shore_Attack", "Slaveshorecap");//добавление в группу
		// LEO: засада нации ростовщика (часто и ГГ) - враг при любом флаге, как бригантина ниже
		sld.AlwaysEnemy = true;
		sld.SuperShooter  = true;
		SetCharacterPerk(sld, "Tireless");
		SetCharacterPerk(sld, "HardHitter");
		SetCharacterPerk(sld, "MusketsShoot");
		hcrew = GetMaxCrewQuantity(sld);
		SetCrewQuantity(sld, hcrew);

		sld.ship.Crew.Morale = 100;
		ChangeCrewExp(sld, "Sailors", 70);
		ChangeCrewExp(sld, "Cannoners", 70);
		ChangeCrewExp(sld, "Soldiers", 100);
    // ==> стравливание
	Group_SetGroupCommander("Shore_Attack", "Slaveshorecap");
	Group_SetTaskAttack("Shore_Attack", PLAYER_GROUP);
	// LEO: у бухты из диалога, а не в 500-800 м от ГГ (Group_SetPursuitGroup); у ГГ - только пока у бухты нет координат
	sTemp = Slavetrader_ShoreReloadAttr(pchar.questTemp.Slavetrader.Island, pchar.questTemp.Slavetrader.Island.Shore);
	if (sTemp != "")
	{
		Group_SetAddress("Shore_Attack", pchar.questTemp.Slavetrader.Island, "reload", sTemp);
	}
	else
	{
		Group_SetPursuitGroup("Shore_Attack", PLAYER_GROUP);
		Group_SetAddress("Shore_Attack", pchar.questTemp.Slavetrader.Island, "", "");
	}
	Group_LockTask("Shore_Attack");

    // ==> прерывание на убиение эскадры
    pchar.quest.Slavetrader_AfterBattle.win_condition.l1 = "Group_Death";//условие победы
	pchar.quest.Slavetrader_AfterBattle.win_condition.l1.group = "Shore_Attack";//какую группу уничтожить
	pchar.quest.Slavetrader_AfterBattle.function = "Slavetrader_ShoreAttack_AfterBattle";//это победа
	pchar.quest.Slavetrader_DieHard.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.Slavetrader_DieHard.function = "Slavetrader_ShoreAttack_DieHard";//это слинял
	pchar.quest.Slavetrader_ShoreShipsOver.over = "yes";
}

// LEO: reload-атрибут острова ("l3") с выходом в бухту sShore; "" - нет такого или нет координат
string Slavetrader_ShoreReloadAttr(string sIsland, string sShore)
{
	int iIsland = FindIsland(sIsland);
	if (iIsland < 0) return "";
	aref arReload, arLocator;
	makearef(arReload, Islands[iIsland].reload);
	int iNum = GetAttributesNum(arReload);
	string sShoreLow = GetStrSmallRegister(sShore);
	for (int j = 0; j < iNum; j++)
	{
		arLocator = GetAttributeN(arReload, j);
		if (!CheckAttribute(arLocator, "go")) continue;
		if (GetStrSmallRegister(arLocator.go) != sShoreLow) continue;
		if (!CheckAttribute(arLocator, "x") || !CheckAttribute(arLocator, "z")) return "";
		return GetAttributeName(arLocator);
	}
	return "";
}

void Slavetrader_ShoreAttack_DieHard(string qName)//реакция на поражение
{
	Group_DeleteGroup("Shore_Attack");
    Island_SetReloadEnableGlobal(pchar.questTemp.Slavetrader.Island, true);
    pchar.quest.Slavetrader_AfterBattle.over = "yes";
	AddQuestRecord("Slavetrader", "11");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","ла"));
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
	pchar.questTemp.Slavetrader = "End_quest";
	FinishSlavetraderQuest();
	ChangeOfficersLoyality("bad_all",1);
}

void Slavetrader_ShoreAttack_AfterBattle(string qName)//реакция на победу
{
	if (!CheckAttribute(pchar, "GenQuest.LastQuestPrisonerIdx"))
	{
	Group_DeleteGroup("Shore_Attack");
	Island_SetReloadEnableGlobal(pchar.questTemp.Slavetrader.Island, true);
    pchar.quest.Slavetrader_DieHard.over = "yes";
	AddQuestRecord("Slavetrader", "12");
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
	FinishSlavetraderQuest();
	pchar.questTemp.Slavetrader = "End_quest";
	}
	else
	{
		characters[sti(pchar.GenQuest.LastQuestPrisonerIdx)].id = "Slaveshorecap"; //полная победа, если захватил, базар в каюте, проверка ИД кэпа
	}
}

void Slavetrader_BrigOver(string qName)//реакция на просроченный таймер
{
	pchar.questTemp.Slavetrader_BrigAttack.over = "yes";//ликвидация бригантины
	AddQuestRecord("Slavetrader", "15");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","ла"));
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetraderbrig.ShipName);
	FinishSlavetraderQuest();
	ChangeCharacterHunterScore(pchar, NationShortName(sti(pchar.questTemp.Slavetrader.Nation)) + "hunter", 50);
	pchar.questTemp.Slavetrader = "goodbye";
	ChangeOfficersLoyality("bad_all", 1);
}

void Slavetrader_CreateBrig(string qName)//создание бригантины
{
	int iNation = sti(pchar.questTemp.Slavetrader.Nation);
	ref sld;
	string sTemp, sNation;

    Island_SetReloadEnableGlobal(pchar.questTemp.Slavetraderbrig.Island, false);
    Group_FindOrCreateGroup("Brig_Attack");//создать группу
	Group_SetType("Brig_Attack", "war");//тип группы

		sTemp = pchar.questTemp.Slavetraderbrig.ShipName;//имя корабля
		sNation = pchar.questTemp.Slavetrader.Nation;//нация корабля
		sld = GetCharacter(NPC_GenerateCharacter("CapBrigAttack", "off_" + NationShortName(iNation) + "_" + (rand(1) + 1), "man", "man", 35, sNation, 3, true));//создание кэпа
		FantomMakeCoolSailor(sld, SHIP_BRIGANTINE, sTemp, CANNON_TYPE_CANNON_LBS24, 100, 100, 100);//создание кораблей
		FantomMakeCoolFighter(sld, 35, 90, 90, "blade26", "pistol3", 90);//создание фантома кэпа
		Group_AddCharacter("Brig_Attack", "CapBrigAttack");//добавление в группу
		sld.SuperShooter  = true;
		sld.AlwaysEnemy = true;
		DeleteAttribute(sld, "SaveItemsForDead");
		DeleteAttribute(sld, "DontClearDead");
		SetCharacterPerk(sld, "Tireless");
		SetCharacterPerk(sld, "HardHitter");
		SetCharacterPerk(sld, "GunProfessional");
		SetCharacterPerk(sld, "MusketsShoot");

		sld.ship.Crew.Morale = 80;
		ChangeCrewExp(sld, "Sailors", 80);
		ChangeCrewExp(sld, "Cannoners", 100);
		ChangeCrewExp(sld, "Soldiers", 100);

    // ==> стравливание
	Group_SetGroupCommander("Brig_Attack", "CapBrigAttack");
	Group_SetTaskAttack("Brig_Attack", PLAYER_GROUP);
	Group_SetPursuitGroup("Brig_Attack", PLAYER_GROUP);
	Group_SetAddress("Brig_Attack", pchar.questTemp.Slavetraderbrig.Island, "", "");
	Group_LockTask("Brig_Attack");

    // ==> прерывание на убиение эскадры
    pchar.quest.Slavetrader_AfterBattle.win_condition.l1 = "Group_Death";//условие победы
	pchar.quest.Slavetrader_AfterBattle.win_condition.l1.group = "Brig_Attack";//какую группу уничтожить
	pchar.quest.Slavetrader_AfterBattle.function = "Slavetrader_BrigAttack_AfterBattle";//это победа
	pchar.quest.Slavetrader_DieHard.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.Slavetrader_DieHard.function = "Slavetrader_BrigAttack_DieHard";//это слинял
}

void Slavetrader_BrigAttack_AfterBattle(string qName)//реакция на победу
{
	Group_DeleteGroup("Brig_Attack");
	Island_SetReloadEnableGlobal(pchar.questTemp.Slavetraderbrig.Island, true);
    pchar.quest.Slavetrader_DieHard.over = "yes";
	pchar.quest.Slavetrader_BrigOver.over = "yes";
	if (CheckCharacterItem(pchar, "letter_1"))
	{
		AddQuestRecord("Slavetrader", "16");
		AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetraderbrig.ShipName);
		pchar.questTemp.Slavetrader = "winbrig";//нода на диалог
	}
	else
	{
		AddQuestRecord("Slavetrader", "17");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("ел","ла"));
		AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetraderbrig.ShipName);
		pchar.questTemp.Slavetrader = "goodbye_1";//нода на диалог
	}
}

void Slavetrader_BrigAttack_DieHard(string qName)//реакция на поражение
{
	Group_DeleteGroup("Brig_Attack");
    Island_SetReloadEnableGlobal(pchar.questTemp.Slavetraderbrig.Island, true);
    pchar.quest.Slavetrader_AfterBattle.over = "yes";
	pchar.quest.Slavetrader_BrigShipsOver.over = "yes";
	AddQuestRecord("Slavetrader", "15");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","ла"));
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetraderbrig.ShipName);
	FinishSlavetraderQuest();
	ChangeCharacterHunterScore(pchar, NationShortName(sti(pchar.questTemp.Slavetrader.Nation)) + "hunter", 50);
	pchar.questTemp.Slavetrader = "goodbye";//нода на диалог
	ChangeOfficersLoyality("bad_all",1);
}

void Slavetrader_enterSoldiers() //арестовывающие солдеры
{
	LAi_SetPlayerType(pchar);
	LAi_group_Delete("EnemyFight");
	pchar.questTemp.Slavetrader = "After_enterSoldiers"; //это состояние квеста, пропишем для следующей ноды
	iTemp = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE - 2; //это ранг от сложности
	int iNation = sti(pchar.questTemp.Slavetrader.Nation);
	for (i=1; i<=3; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter("SLSold"+i, "sold_" + NationShortName(iNation) + "_" + (rand(1) + 1), "man", "man", iTemp, iNation, 0, true));
		SetFantomParamFromRank(sld, iTemp, true);
		LAi_SetWarriorType(sld);
		LAi_warrior_DialogEnable(sld, false);
        LAi_group_MoveCharacter(sld, "EnemyFight");
		// LEO: иначе не войдут в «захваченный» город (Дес-Мойнес под проклятием)
		LAi_LoginInCaptureTown(sld, true);
        ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto3");
    }
    sld = GetCharacter(NPC_GenerateCharacter("SLOfficer", "off_" + NationShortName(iNation) + "_" + (rand(1) + 1), "man", "man", iTemp, iNation, 0, true));
	FantomMakeCoolFighter(sld, 25, 100, 100, "topor2", "pistol3", 80);
	sld.Dialog.Filename = "Quest\Other_Quests_NPC.c"; //диалоговый файл, из которого говорить будет
	sld.dialog.currentnode = "Slave_arest"; // диалоговый узел
	sld.greeting = "soldier_arest";
    LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	LAi_LoginInCaptureTown(sld, true);
	ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload1");
	LAi_ActorDialog(sld, pchar, "", 1.0, 0);
}
void Slavetrader_Bonanza(string qName)//cоздание индейцев
{
	chrDisableReloadToLocation = true;
    for (i=1; i<=8; i++)
    {
		if (i == 2 || i == 4 || i == 6 || i == 8)
        {
			sld = GetCharacter(NPC_GenerateCharacter("Indian"+i, "indsar1", "man", "man", 25, PIRATE, 0, true));
            FantomMakeCoolFighter(sld, 25, 80, 100, "topor2", "pistol3", 100);
		}
		else
        {
			sld = GetCharacter(NPC_GenerateCharacter("Indian"+i, "indsair2", "man", "man", 35, PIRATE, 0, true));
		}
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        GetCharacterPos(pchar, &locx, &locy, &locz);
				sTemp = LAi_FindNearestFreeLocator("goto", locx, locy, locz);
				if (sTemp == "") sTemp = LAi_FindNearestLocator("goto", locx, locy, locz);
               	ChangeCharacterAddressGroup(sld, "Shore18", "goto", sTemp);
    }
	sld = GetCharacter(NPC_GenerateCharacter("Nabuki", "mummy", "man", "man", 25, PIRATE, 0, true));
    sld.name 	= GetConvertStr("Variable_reaction_functions_0.15", "Names.txt");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_172", "Names.txt", "#space#", " ");
		FantomMakeCoolFighter(sld, 60, 100, 100, "toporAZ", "pistol4", 100);
                TakeNItems(sld, "jewelry2", 120+rand(80));
				TakeNItems(sld, "jewelry3", 120+rand(60));
				TakeNItems(sld, "jewelry4", 140+rand(50));
				TakeNItems(sld, "jewelry15", rand(50));
	TakeNItems(sld, "indian11", 1);
	sld.SaveItemsForDead = true; // сохранять на трупе вещи
	sld.DontClearDead = true;
					sld.cirassId = Items_FindItemIdx("cirass5");
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
                GetCharacterPos(pchar, &locx, &locy, &locz);
				sTemp = LAi_FindNearestFreeLocator("goto", locx, locy, locz);
				if (sTemp == "") sTemp = LAi_FindNearestLocator("goto", locx, locy, locz);
               	ChangeCharacterAddressGroup(sld, "Shore18", "goto", sTemp);

    LAi_group_SetLookRadius("EnemyFight", 100);
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
    // Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
    LAi_SetFightMode(PChar, true);
	LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
}

void Slavetrader_GetHispanShore()//определение бухты, из которой галеон выйдет
{
	pchar.questTemp.Slavetrader.Island = "Caracas";
	pchar.questTemp.Slavetrader.Island.Shore = "Shore21";
	if(LanguageGetLanguage() != "russian")
	{
		pchar.questTemp.SlavetraderAreal.add = "from Caracas";
		while(pchar.questTemp.Slavetrader.Island == Islands[GetCharacterCurrentIsland(pchar)].id)
		{
			pchar.questTemp.Slavetrader.Island = "Cumana";
			pchar.questTemp.Slavetrader.Island.Shore = "Shore19";
			pchar.questTemp.SlavetraderAreal.add = "from Cumana";
		}
	}
	else
	{
		pchar.questTemp.SlavetraderAreal.add = "из Каракаса";
		while(pchar.questTemp.Slavetrader.Island == Islands[GetCharacterCurrentIsland(pchar)].id)
		{
			pchar.questTemp.Slavetrader.Island = "Cumana";
			pchar.questTemp.Slavetrader.Island.Shore = "Shore19";
			pchar.questTemp.SlavetraderAreal.add = "из Куманы";
		}
	}
}

void Slavetrader_SlaveHalleon_AfterBattle(string qName)//реакция на победу
{
	Group_DeleteGroup("SlaveGalleon");
	pchar.quest.Slavetrader_SlaveGalleonOver.over = "yes";
	AddQuestRecord("Slavetrader", "19");
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
	ChangeCharacterReputation(pchar, -5);
	pchar.questTemp.Slavetrader = "wingalleon";//нода на диалог
}

void Slavetrader_SlaveGalleonOver(string qName)//просроченный таймер
{
	pchar.quest.Slavetrader_SlaveGalleon.over = "yes";
	AddQuestRecord("Slavetrader", "7_2");
	AddQuestUserData("Slavetrader", "sShipName", pchar.questTemp.Slavetrader.ShipName);
	pchar.questTemp.Slavetrader = "lostgalleon";
	ChangeOfficersLoyality("bad_all", 1);
}

void Slavetrader_RatCorvette(string qName)//создание корвета крысы
{
	ref sld;
	string sTemp;

    Island_SetReloadEnableGlobal("Tortuga", false);
    Group_FindOrCreateGroup("Rat_Attack");//создать группу
	Group_SetType("Rat_Attack", "war");//тип группы

		sTemp = pchar.questTemp.Slavetrader.ShipName;//имя корабля
		sld = GetCharacter(NPC_GenerateCharacter("RatCap", "officer_16", "man", "man", 35, FRANCE, 3, true));//создание кэпа
		sld.name = GetConvertStr("Variable_reaction_functions_173", "Names.txt");
		sld.lastname = GetConvertStr("Variable_reaction_functions_174", "Names.txt");
		FantomMakeCoolSailor(sld, SHIP_CORVETTE, sTemp, CANNON_TYPE_CANNON_LBS32, 100, 100, 100);//создание кораблей
		FantomMakeCoolFighter(sld, 35, 80, 80, "blade26", "pistol5", 90);//создание фантома кэпа
		DeleteAttribute(sld, "SaveItemsForDead");
		DeleteAttribute(sld, "DontClearDead");
		sld.Ship.Mode = "pirate";
		Group_AddCharacter("Rat_Attack", "RatCap");//добавление в группу
		sld.SuperShooter  = true;
		sld.AlwaysEnemy = true;
		SetCharacterPerk(sld, "Tireless");
		SetCharacterPerk(sld, "HardHitter");
		SetCharacterPerk(sld, "GunProfessional");
		SetCharacterPerk(sld, "MusketsShoot");

		sld.ship.Crew.Morale = 100;
		ChangeCrewExp(sld, "Sailors", 100);
		ChangeCrewExp(sld, "Cannoners", 100);
		ChangeCrewExp(sld, "Soldiers", 100);

    // ==> стравливание
	Group_SetGroupCommander("Rat_Attack", "RatCap");
	Group_SetTaskAttack("Rat_Attack", PLAYER_GROUP);
	Group_SetPursuitGroup("Rat_Attack", PLAYER_GROUP);
	Group_SetAddress("Rat_Attack", "Tortuga", "", "");
	Group_LockTask("Rat_Attack");

    // ==> прерывание на убиение эскадры
    pchar.quest.Slavetrader_AfterBattle.win_condition.l1 = "Group_Death";//условие победы
	pchar.quest.Slavetrader_AfterBattle.win_condition.l1.group = "Rat_Attack";//какую группу уничтожить
	pchar.quest.Slavetrader_AfterBattle.function = "Slavetrader_RatAttack_AfterBattle";//это победа
	pchar.quest.Slavetrader_DieHard.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.Slavetrader_DieHard.function = "Slavetrader_RatAttack_DieHard";//это слинял
	pchar.quest.Slavetrader_RatCorvetteOver.over = "yes";
}

void Slavetrader_RatAttack_AfterBattle(string qName)//реакция на победу
{
	Group_DeleteGroup("Rat_Attack");
	Island_SetReloadEnableGlobal("Tortuga", true);
    pchar.quest.Slavetrader_DieHard.over = "yes";
	AddQuestRecord("Slavetrader", "21_12");
	pchar.questTemp.Slavetrader = "wincorvette";//нода на диалог
}

void Slavetrader_RatAttack_DieHard(string qName)//реакция на поражение
{
	Group_DeleteGroup("Rat_Attack");
    Island_SetReloadEnableGlobal("Tortuga", true);
    pchar.quest.Slavetrader_AfterBattle.over = "yes";
	AddQuestRecord("Slavetrader", "21_11");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("счел","сочла"));
	pchar.questTemp.Slavetrader = "lostcorvette";//нода на диалог
	ChangeOfficersLoyality("bad_all", 1);
}

void Slavetrader_RatCorvetteOver(string qName)//просроченный таймер корвета
{
	pchar.quest.Slavetrader_RatAttack.over = "yes";
	AddQuestRecord("Slavetrader", "21_10");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","а"));
	pchar.questTemp.Slavetrader = "lostcorvette";
	ChangeOfficersLoyality("bad_all", 1);
}


void Slavetrader_GetEscapeShore()//выбираем маяк, куда беглых рабов запхнем
{
	if(LanguageGetLanguage() != "russian")
	{
		switch (rand(9))
		{
			case 0:
				pchar.questTemp.Slavetrader.EsIsland = "Barbados";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak2";
				pchar.questTemp.SlavetraderAreal.add = "off the coast of Barbados";
				break;
			case 1:
				pchar.questTemp.Slavetrader.EsIsland = "Guadeloupe";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak4";
				pchar.questTemp.SlavetraderAreal.add = "off the coast of Guadeloupe";
				break;
			case 2:
				pchar.questTemp.Slavetrader.EsIsland = "Cuba1";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak9";
				pchar.questTemp.SlavetraderAreal.add = "near Santiago";
				break;
			case 3:
				pchar.questTemp.Slavetrader.EsIsland = "Cuba2";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak10";
				pchar.questTemp.SlavetraderAreal.add = "near Havana";
				break;
			case 4:
				pchar.questTemp.Slavetrader.EsIsland = "Tortuga";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak6";
				pchar.questTemp.SlavetraderAreal.add = "off the coast of Tortuga";
				break;
			case 5:
				pchar.questTemp.Slavetrader.EsIsland = "Trinidad";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak1";
				pchar.questTemp.SlavetraderAreal.add = "off the coast of Trinidad";
				break;
			case 6:
				pchar.questTemp.Slavetrader.EsIsland = "Nevis";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak5";
				pchar.questTemp.SlavetraderAreal.add = "off the coast of Nevis";
				break;
			case 7:
				pchar.questTemp.Slavetrader.EsIsland = "Hispaniola1";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak8";
				pchar.questTemp.SlavetraderAreal.add = "near Santo Domingo";
				break;
			case 8:
				pchar.questTemp.Slavetrader.EsIsland = "Hispaniola2";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak7";
				pchar.questTemp.SlavetraderAreal.add = "near Port-au-Prince";
				break;
			case 9:
				pchar.questTemp.Slavetrader.EsIsland = "Cartahena";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak11";
				pchar.questTemp.SlavetraderAreal.add = "near Cartagena";
				break;
		}
	}
	else
	{
		switch (rand(9))
		{
			case 0:
				pchar.questTemp.Slavetrader.EsIsland = "Barbados";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak2";
				pchar.questTemp.SlavetraderAreal.add = "у берегов Барбадоса";
				break;
			case 1:
				pchar.questTemp.Slavetrader.EsIsland = "Guadeloupe";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak4";
				pchar.questTemp.SlavetraderAreal.add = "у берегов Гваделупы";
				break;
			case 2:
				pchar.questTemp.Slavetrader.EsIsland = "Cuba1";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak9";
				pchar.questTemp.SlavetraderAreal.add = "недалеко от Сантьяго";
				break;
			case 3:
				pchar.questTemp.Slavetrader.EsIsland = "Cuba2";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak10";
				pchar.questTemp.SlavetraderAreal.add = "недалеко от Гаваны";
				break;
			case 4:
				pchar.questTemp.Slavetrader.EsIsland = "Tortuga";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak6";
				pchar.questTemp.SlavetraderAreal.add = "у берегов Тортуги";
				break;
			case 5:
				pchar.questTemp.Slavetrader.EsIsland = "Trinidad";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak1";
				pchar.questTemp.SlavetraderAreal.add = "у берегов Тринидада";
				break;
			case 6:
				pchar.questTemp.Slavetrader.EsIsland = "Nevis";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak5";
				pchar.questTemp.SlavetraderAreal.add = "у берегов Невиса";
				break;
			case 7:
				pchar.questTemp.Slavetrader.EsIsland = "Hispaniola1";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak8";
				pchar.questTemp.SlavetraderAreal.add = "недалеко от Санто-Доминго";
				break;
			case 8:
				pchar.questTemp.Slavetrader.EsIsland = "Hispaniola2";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak7";
				pchar.questTemp.SlavetraderAreal.add = "недалеко от Порт-о-Принса";
				break;
			case 9:
				pchar.questTemp.Slavetrader.EsIsland = "Cartahena";
				pchar.questTemp.Slavetrader.Island.Shore = "Mayak11";
				pchar.questTemp.SlavetraderAreal.add = "недалеко от Картахены";
				break;
		}
	}
}

void Slavetrader_EscapeSlaveInShore(string qName)
{
	string sShore;
	sShore = pchar.questTemp.Slavetrader.Island.Shore
    LAi_group_Delete("EnemyFight");
	chrDisableReloadToLocation = true; //закроем локацию
	GetCharacterPos(pchar, &locx, &locy, &locz);
	//наши
    for (i=1; i<=3; i++)
    {
        iTemp = 10 + rand(10);
		sld = GetCharacter(NPC_GenerateCharacter("OwnCrew_"+i, "pirate_"+(rand(23)+2), "man", "man", iTemp, sti(pchar.nation), 0, true));
        LAi_SetWarriorType(sld);
		LAi_warrior_DialogEnable(sld, false);
        LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
		sTemp = LAi_FindNearestFreeLocator("goto", locx, locy, locz);
		if (sTemp == "") sTemp = LAi_FindNearestLocator("goto", locx, locy, locz);
        ChangeCharacterAddressGroup(sld, sShore, "goto", sTemp);
    }
	//враги
	for (i=21; i<=27; i++)
    {
		iTemp = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE + 10;
		sld = GetCharacter(NPC_GenerateCharacter("EscapeSlave_"+i, "Pirate_1", "man", "man", iTemp, PIRATE, -1, true));
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        GetCharacterPos(pchar, &locx, &locy, &locz);
        ChangeCharacterAddressGroup(sld, sShore, "goto", "goto"+i);
    }
			sld = GetCharacter(NPC_GenerateCharacter("Jimmy", "capitan_3", "man", "man", 35, PIRATE, 0, true));
			sld.name 	= GetConvertStr("Variable_reaction_functions_0.16", "Names.txt");
			sld.lastname = "";
			FantomMakeCoolFighter(sld, 35, 100, 100, "topor2", "pistol3", 80);
			sld.SaveItemsForDead = true; // сохранять на трупе вещи
			sld.cirassId = Items_FindItemIdx("cirass3");
			sld.DontClearDead = true;
			sld.HeroModel = "capitan_3,capitan_3_1,capitan_3_2,capitan_3_3,capitan_3_4,capitan_3_5";
	LAi_SetWarriorType(sld);
    LAi_group_MoveCharacter(sld, "EnemyFight");
    GetCharacterPos(pchar, &locx, &locy, &locz);
    ChangeCharacterAddressGroup(sld, sShore, "goto", "goto30");

    LAi_group_SetLookRadius("EnemyFight", 100);
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
    // Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
    LAi_SetFightMode(PChar, true);
	LAi_group_SetCheck("EnemyFight", "Slavetrader_EscapeSlaves_Win");
}

void Slavetrader_Slavewoman()
{
		  sld = characterFromId("Slavewoman");
	      LAi_SetPlayerType(pchar);
          SetCharacterRemovable(sld, false);
          LAi_SetActorType(sld);
		  LAi_ActorRunToLocation(sld, "goto", "goto2", "none", "", "", "", 15.0);
		  sld.lifeday = 0;
		  for (i=10; i<=25; i++)
			{
				sld = GetCharacter(NPC_GenerateCharacter("SlaveShor_"+i, "Pirate_1", "man", "man", 10, PIRATE, 0, true));
				FantomMakeCoolFighter(sld, 25, 100, 100, "", "", 80);
				ChangeCharacterAddressGroup(sld, pchar.questTemp.Slavetrader.Island.Shore, "goto",  "goto"+i);
				sld = characterFromID("SlaveShor_"+i);
				LAi_SetImmortal(sld, true);
				LAi_SetActorType(sld);
				LAi_SetCitizenTypeNoGroup(sld);
				sld.lifeday = 0;//на всякий случай
				LAi_CharacterDisableDialog(sld);
			}
		  SetCharacterGoods(pchar, GOOD_SLAVES, 1000 + rand(100));
		  sTemp = GetSquadronGoods(Pchar, GOOD_SLAVES);
		  AddQuestRecord("Slavetrader", "22");
		  AddQuestUserData("Slavetrader", "sQty", sTemp);
		  pchar.questTemp.Slavetrader = "escapeslave_win";
		  ChangeCharacterReputation(pchar, -1);
		  OfficersReaction("bad");
}

void Slavetrader_CreateLineShips(string qName)//создание линейных кораблей
{
	int i;
	ref sld;
	string sTemp;

	Island_SetReloadEnableGlobal("Cuba2", false);
    Group_FindOrCreateGroup("Havana_Attack");//создать группу
	Group_SetType("Havana_Attack", "war");//тип группы
	SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);//ссорим ГГ и испанцев
	for (i=1; i<=2; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("HavanaAttack_"+i, "off_spa_1", "man", "man", 35, SPAIN, 3, true));//создание кэпа
		FantomMakeCoolSailor(sld, SHIP_LINESHIP, "", CANNON_TYPE_CANNON_LBS32, 100, 100, 100);//создание кораблей
		FantomMakeCoolFighter(sld, 45, 100, 100, "blade28", "pistol5", 100);//создание фантома кэпа
		Group_AddCharacter("Havana_Attack", "HavanaAttack_"+i);//добавление в группу
		sld.SuperShooter  = true;
		sld.AlwaysEnemy = true;
		DeleteAttribute(sld, "SaveItemsForDead");
		DeleteAttribute(sld, "DontClearDead");
		SetCharacterPerk(sld, "MusketsShoot");
		SetCharacterPerk(sld, "Tireless");
		SetCharacterPerk(sld, "HardHitter");
		SetCharacterPerk(sld, "GunProfessional");

		sld.ship.Crew.Morale = 100;
		ChangeCrewExp(sld, "Sailors", 100);
		ChangeCrewExp(sld, "Cannoners", 100);
		ChangeCrewExp(sld, "Soldiers", 100);
    }
    // ==> стравливание
	Group_SetGroupCommander("Havana_Attack", "HavanaAttack_1");
	Group_SetTaskAttack("Havana_Attack", PLAYER_GROUP);
	Group_SetPursuitGroup("Havana_Attack", PLAYER_GROUP);
	Group_SetAddress("Havana_Attack", "Cuba2", "", "");
	Group_LockTask("Havana_Attack");
	bQuestDisableMapEnter = true; //запрещаем выход на глобу, чтоб не взяли форт до потопления
    // ==> прерывание на убиение эскадры
    pchar.quest.Slavetrader_AfterBattle.win_condition.l1 = "Group_Death";//условие победы
	pchar.quest.Slavetrader_AfterBattle.win_condition.l1.group = "Havana_Attack";//какую группу уничтожить
	pchar.quest.Slavetrader_AfterBattle.function = "Slavetrader_HavanaAttack_AfterBattle";//это победа
	pchar.quest.Slavetrader_DieHardHavana.win_condition.l1 = "MapEnter";//не лезь на карту
    pchar.quest.Slavetrader_DieHardHavana.function = "Slavetrader_HavanaAttack_DieHard";//теперь пеняй на себя
	AddQuestRecord("Slavetrader", "25");
}

void Slavetrader_HavanaAttack_AfterBattle(string qName)//реакция на победу
{
	bQuestDisableMapEnter = false;
	Group_DeleteGroup("Havana_Attack");
	Island_SetReloadEnableGlobal("Cuba2", true);
	AddQuestRecord("Slavetrader", "26");
}

void Slavetrader_HavanaAttack_DieHard(string qName)//если надумал починиться
{
	pchar.quest.Slavetrader_HavanaOver.over = "yes";//снимем таймер
	characters[GetCharacterIndex("Havana_Mayor")].dialog.captureNode = "CAPTURE_Main";//снимем ноду губеру
	AddQuestRecord("Slavetrader", "26_1");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","а"));
	FinishSlavetraderQuest();
	pchar.questTemp.Slavetrader = "End_quest";
}

void Slavetrader_HavanaOver(string qName)//просроченный таймер
{
	pchar.quest.Slavetrader_HavanaAttack.over = "yes";
	AddQuestRecord("Slavetrader", "28");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","ла"));
	ChangeOfficersLoyality("bad_all", 1);
	FinishSlavetraderQuest();
	characters[GetCharacterIndex("Havana_Mayor")].dialog.captureNode = "CAPTURE_Main";
	pchar.questTemp.Slavetrader = "End_quest";
}

void Slavetrader_FiveTSlavesOver(string qName)//просроченный таймер
{
	ChangeCharacterHunterScore(pchar, "spahunter", 100);
	ChangeCharacterHunterScore(pchar, "holhunter", 100);
	ChangeCharacterHunterScore(pchar, "frahunter", 100);
	ChangeCharacterHunterScore(pchar, "enghunter", 100);
	pchar.questTemp.Slavetrader = "End_quest_bad";
	AddQuestRecord("Slavetrader", "30");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","а"));
	FinishSlavetraderQuest();
}

void Slavetrader_HavanaSeekOver(string qName)//просроченный таймер
{
	ChangeCharacterHunterScore(pchar, "spahunter", 100);
	ChangeCharacterHunterScore(pchar, "holhunter", 100);
	ChangeCharacterHunterScore(pchar, "frahunter", 100);
	ChangeCharacterHunterScore(pchar, "enghunter", 100);
	pchar.questTemp.Slavetrader = "End_quest_bad";
	AddQuestRecord("Slavetrader", "31");
	AddQuestUserData("Slavetrader", "sSex", GetSexPhrase("","а"));
	FinishSlavetraderQuest();
}

void Slavetrader_UsurerEscape(string qName)//меняем модель ростовщика
{
	sld = characterFromId(pchar.questTemp.Slavetrader.UsurerId);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_175", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_176", "Names.txt", "#space#", " ");
	sld.model = "trader_8";
	Slavetrader_SetBankAccess(sld.city, false);
}
