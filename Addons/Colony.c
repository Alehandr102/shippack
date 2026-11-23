
#define		COLONY_AUTO_PURCHASE_FOOD		7
#define		COLONY_AUTO_PURCHASE_MEDICAMENT		8
#define		COLONY_AUTO_PURCHASE_TOOLS		23
#define		COLONY_MIN_MORALE_DOWN			3

#define 	COLONY_FOOD_POPULATION_DELIT		25
#define 	COLONY_FOOD_SOLDIERS_DELIT		50
#define 	COLONY_MEDICAMENT_DELIT			50

/////////////////////////////////////////////////////////////////////////////////
// COLONY
/////////////////////////////////////////////////////////////////////////////////
void GenerateColonyVariable()
{
	ref PChar = GetMainCharacter();

	PChar.ColonyBuilding.bAch = false;
	PChar.ColonyBuilding.bCreate = false;

	PChar.ColonyBuilding.SlavesInShore.CurShore = "none";

	PChar.ColonyBuilding.CurrentIconsTexture = 1;
	PChar.ColonyBuilding.MessageInfo = "";
	PChar.ColonyBuilding.Colony.AutoPurchase = false;
	PChar.ColonyBuilding.Mines.AutoPurchase = false;
	PChar.ColonyBuilding.Plantation.AutoPurchase = false;
	PChar.ColonyBuilding.AutoPurchaseFirst = false;
	PChar.ColonyBuilding.AutoPurchaseFirst.Disable = false;

	PChar.ColonyBuilding.Mines.AutoSell = false;
	PChar.ColonyBuilding.Plantation.AutoSell = false;
	PChar.ColonyBuilding.Mines.AutoSell.Qty = 100;
	PChar.ColonyBuilding.Plantation.AutoSell.Qty = 100;

	PChar.ColonyBuilding.Stage = "0";
	PChar.ColonyBuilding.Money = 0;
	PChar.ColonyBuilding.Money.Temp = 0;
	PChar.ColonyBuilding.Nalog = 1;
	PChar.ColonyBuilding.Population = rand(15) + 20;
	PChar.ColonyBuilding.Population.Morale = rand(20) + 20;
	PChar.ColonyBuilding.Population.Max = 250 + (rand(70) + rand(50));
	PChar.ColonyBuilding.Population.Optimal = (sti(PChar.ColonyBuilding.Population.Max) - 35) - rand(15);

	PChar.ColonyBuilding.Population.Epidemy = false;
	PChar.ColonyBuilding.Money.CanGet = false;
	PChar.ColonyBuilding.Store = false;
	PChar.ColonyBuilding.Shipyard = false;
	PChar.ColonyBuilding.Tavern = false;
	PChar.ColonyBuilding.Church = false;
	PChar.ColonyBuilding.Bank = false;
	PChar.ColonyBuilding.HeadPort = false;
	PChar.ColonyBuilding.Expidition = false;
	PChar.ColonyBuilding.StoreHouse = false;
	PChar.ColonyBuilding.StoreHouse.Resize = false;
	PChar.ColonyBuilding.Plantation = false;
	PChar.ColonyBuilding.Plantation.Resize = false;
	PChar.ColonyBuilding.Mines = false;
	PChar.ColonyBuilding.GoldMine = false;
	PChar.ColonyBuilding.SilverMine = false;
	PChar.ColonyBuilding.IronMine = false;
	PChar.ColonyBuilding.GoldMine.Resize = false;
	PChar.ColonyBuilding.SilverMine.Resize = false;
	PChar.ColonyBuilding.IronMine.Resize = false;
	PChar.ColonyBuilding.Store.BuildingTime = false;
	PChar.ColonyBuilding.Shipyard.BuildingTime = false;
	PChar.ColonyBuilding.Tavern.BuildingTime = false;
	PChar.ColonyBuilding.Church.BuildingTime = false;
	PChar.ColonyBuilding.Bank.BuildingTime = false;
	PChar.ColonyBuilding.HeadPort.BuildingTime = false;
	PChar.ColonyBuilding.Expidition.BuildingTime = false;
	PChar.ColonyBuilding.StoreHouse.BuildingTime = false;
	PChar.ColonyBuilding.StoreHouse.Resize.BuildingTime = false;
	PChar.ColonyBuilding.Plantation.Resize.BuildingTime = false;
	PChar.ColonyBuilding.Plantation.BuildingTime = false;
	PChar.ColonyBuilding.GoldMine.BuildingTime = false;
	PChar.ColonyBuilding.SilverMine.BuildingTime = false;
	PChar.ColonyBuilding.IronMine.BuildingTime = false;
	PChar.ColonyBuilding.GoldMine.Resize.BuildingTime = false;
	PChar.ColonyBuilding.SilverMine.Resize.BuildingTime = false;
	PChar.ColonyBuilding.IronMine.Resize.BuildingTime = false;

	PChar.ColonyBuilding.Expidition.Cost 		= (5000 * MOD_SKILL_ENEMY_RATE*3) + (rand(2500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.Plantation.Cost		= (70000 * MOD_SKILL_ENEMY_RATE*3) + (rand(5000) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.StoreHouse.Cost 		= (10000 * MOD_SKILL_ENEMY_RATE*3) + (rand(2500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.StoreHouse.ResizeCost 	= (5000 * MOD_SKILL_ENEMY_RATE*3) + (rand(2500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.GoldMine.Cost 			= (30000 * MOD_SKILL_ENEMY_RATE*3) + (rand(10000) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.SilverMine.Cost 		= (25000 * MOD_SKILL_ENEMY_RATE*3) + (rand(5000) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.IronMine.Cost 			= (20000 * MOD_SKILL_ENEMY_RATE*3) + (rand(1500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.GoldMine.ResizeCost 	= (15000 * MOD_SKILL_ENEMY_RATE*3) + (rand(5000) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.SilverMine.ResizeCost 	= (10000 * MOD_SKILL_ENEMY_RATE*3) + (rand(2500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.IronMine.ResizeCost 	= (8500 * MOD_SKILL_ENEMY_RATE*3) + (rand(2000) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.Store.Cost 			= (15000 * MOD_SKILL_ENEMY_RATE*3) + (rand(2500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.Shipyard.Cost 			= (17500 * MOD_SKILL_ENEMY_RATE*3) + (rand(2850) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.Tavern.Cost 			= (15000 * MOD_SKILL_ENEMY_RATE*3) + (rand(500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.Church.Cost 			= (20000 * MOD_SKILL_ENEMY_RATE*3) + (rand(2500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.Bank.Cost 				= (20000 * MOD_SKILL_ENEMY_RATE*3) + (rand(2500) * MOD_SKILL_ENEMY_RATE*3);
	PChar.ColonyBuilding.HeadPort.Cost 			= (12500 * MOD_SKILL_ENEMY_RATE*3) + (rand(2550) * MOD_SKILL_ENEMY_RATE*3);

	PChar.BuildingColony.Blocks = 1000; // Кирпичи
	PChar.BuildingColony.Planks = 1000; // Доски
	PChar.BuildingColony.Mahogany = 400; // Красное дерево
	PChar.BuildingColony.Ebony = 400; // Чёрное дерево
	PChar.BuildingColony.Slaves = 200; // Рабы
	PChar.BuildingColony.Food = 400; // Провиант

	PChar.BuildingColony.ColonyTime = rand(10) + 30;
	PChar.BuildingColony.ColonyModification = rand(5) + 40;
	PChar.BuildingColony.ColonyFort = rand(5) + 35 + rand(5);

	PChar.ColonyBuilding.Hovernor = "";
	PChar.ColonyBuilding.MineMessage = false;
	PChar.ColonyBuilding.Action = false;
	PChar.ColonyBuilding.FortBuilding = false;
	PChar.ColonyBuilding.Fort = false;
	PChar.ColonyBuilding.FirstBuild = false;

	PChar.ColonyBuilding.EventBlock = false;

	PChar.ColonyBuilding.Defence = false;
	PChar.ColonyBuilding.Defence.Battle = false;
	PChar.ColonyBuilding.Defence.DeadInTown = false;
	PChar.ColonyBuilding.Defence.Type = "Mutiny";

	PChar.ColonyBuilding.DeleteGoods = "";

	PChar.ColonyBuilding.Soldiers = 0;
	PChar.ColonyBuilding.Soldiers.MaxQuantity = 1000;
	PChar.ColonyBuilding.Food = 0;
	PChar.ColonyBuilding.Food.MaxQuantity = 5000;
	PChar.ColonyBuilding.Medicament = 0;
	PChar.ColonyBuilding.Medicament.MaxQuantity = 1000;

	PChar.ColonyBuilding.ColonyName = GetConvertStrWithReplace("Variable_Colony_1", "Names.txt", "#space#", " ");
	PChar.ColonyBuilding.ColonyNation = 0;
	PChar.ColonyBuilding.SoldiersType = "officer";

	PChar.ColonyBuilding.Architecture = false;
	PChar.ColonyBuilding.Architecture.Town = 1;
	PChar.ColonyBuilding.Architecture.Store = 1;
	PChar.ColonyBuilding.Architecture.Shipyard = 1;
	PChar.ColonyBuilding.Architecture.Tavern = 1;
	PChar.ColonyBuilding.Architecture.Bank = 1;
	PChar.ColonyBuilding.Architecture.Townhall = 1;

	PChar.ColonyBuilding.Architecture.Town.Cost.Type_1 		= 100000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Town.Cost.Type_2 		= 200000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Town.Cost.Type_3 		= 150000 * MOD_SKILL_ENEMY_RATE*3;

	PChar.ColonyBuilding.Architecture.Store.Cost.Type_1 	= 10000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Store.Cost.Type_2 	= 15000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Store.Cost.Type_3 	= 20000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Store.Cost.Type_4 	= 17500 * MOD_SKILL_ENEMY_RATE*3;

	PChar.ColonyBuilding.Architecture.Shipyard.Cost.Type_1 	= 10000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Shipyard.Cost.Type_2 	= 15000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Shipyard.Cost.Type_3 	= 20000 * MOD_SKILL_ENEMY_RATE*3;

	PChar.ColonyBuilding.Architecture.Tavern.Cost.Type_1 	= 12500 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Tavern.Cost.Type_2 	= 15000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Tavern.Cost.Type_3 	= 20000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Tavern.Cost.Type_4 	= 10000 * MOD_SKILL_ENEMY_RATE*3;

	PChar.ColonyBuilding.Architecture.Bank.Cost.Type_1 		= 20000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Bank.Cost.Type_2 		= 25000 * MOD_SKILL_ENEMY_RATE*3;

	PChar.ColonyBuilding.Architecture.Townhall.Cost.Type_1 	= 30000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Townhall.Cost.Type_2 	= 20000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Townhall.Cost.Type_3 	= 35000 * MOD_SKILL_ENEMY_RATE*3;
	PChar.ColonyBuilding.Architecture.Townhall.Cost.Type_4 	= 25000 * MOD_SKILL_ENEMY_RATE*3;

	SetColonyMessageInfo("", "", "", PIRATE, 0, true, false);
	InitMines();
	StoreHouseInit();
}

bool GetDisableAllColonyEvent()
{
	bool bEnable = PChar.ColonyBuilding.EventBlock;

	if(bEnable)
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

void LiveOfTheColonyUptade()
{
	ref PChar = GetMainCharacter();

	Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_1", "Logs.txt", "#space#", " "));
	AutoPurchaseColonyGoods("Colony");

	int iPopulationQuantity = sti(PChar.ColonyBuilding.Population);
	int iPopulationQuantityOpt = sti(PChar.ColonyBuilding.Population.Optimal);
	int iPopulationMorale = sti(PChar.ColonyBuilding.Population.Morale);
	int iNalog = sti(PChar.ColonyBuilding.Nalog);

	int iPopulationReSize = 0;
	int iQuantityResize = 0;
	int iMoraleMinus = COLONY_MIN_MORALE_DOWN;

	if(iPopulationQuantity > iPopulationQuantityOpt) { iMoraleMinus = 7; }
	switch(iNalog) { case 1: iMoraleMinus += 3; break; case 2: iMoraleMinus += 17; break; case 3: iMoraleMinus += 24; break; }

	// Влияние зданий на мораль
	//------------>>>>>>>>>>>>>>
	bool temp = false;
	temp = sti(PChar.ColonyBuilding.Store);
	if(!temp) { iMoraleMinus += 2; }else{ iMoraleMinus -= 1; }

	temp = sti(PChar.ColonyBuilding.Tavern);
	if(!temp) { iMoraleMinus += 4; }else{ iMoraleMinus -= 2; }

	if(PChar.ColonyBuilding.Stage == "2" || PChar.ColonyBuilding.Stage == "3")
	{
		temp = sti(PChar.ColonyBuilding.Shipyard);
		if(!temp) { iMoraleMinus += 3; }else{ iMoraleMinus -= 2; }

		temp = sti(PChar.ColonyBuilding.Church);
		if(!temp) { iMoraleMinus += 7; }else{ iMoraleMinus -= 5; }

		temp = sti(PChar.ColonyBuilding.Bank);
		if(!temp) { iMoraleMinus += 5; }else{ iMoraleMinus -= 4; }

		temp = sti(PChar.ColonyBuilding.HeadPort);
		if(!temp) { iMoraleMinus += 4; }else{ iMoraleMinus -= 3; }
	}
	//<<<<<<<<<<<<<<------------

	if(iMoraleMinus < COLONY_MIN_MORALE_DOWN) { iMoraleMinus = COLONY_MIN_MORALE_DOWN; }
	ChangeColonyMorale(-iMoraleMinus, false);

	iPopulationMorale = sti(PChar.ColonyBuilding.Population.Morale);
	bool bDisable = GetDisableAllColonyEvent();
	if(iPopulationMorale < 20 && rand(100) < 10)
	{
		if(!bDisable)
		{
			CreateMutinyColony();
		}
	}

	iQuantityResize = iPopulationQuantity / 5;
	if(iQuantityResize < 12) iQuantityResize = 12;
	if(iQuantityResize > 60) iQuantityResize = 60;


	if(iPopulationMorale < 40)
	{
		ChangeColonyPopulation(-iQuantityResize, false);
	}
	else
	{
		ChangeColonyPopulation(iQuantityResize, false);
	}

	int iMinPeople = GetColonyMinPeople();
	if(sti(PChar.ColonyBuilding.Population) < iMinPeople)
	{
		ChangeColonyPopulation(iMinPeople, true);
	}

	if(rand(15) == 1)
	{
		ColonyCheckEpidemy();
		PlaySound("notebook");
	}

	ColonyCheckFoodNeed();

	MoneyOfTheColony();
	BaseCompletationColonyGuards();
	ColonyGuardingAutoCrewHire("Colony");
}

void MoneyOfTheColony()
{
	ref PChar = GetMainCharacter();

	if(PChar.ColonyBuilding.Hovernor == "")
	{
		Log_InfoS(GetConvertStrWithReplace("Variable_Colony_4", "Logs.txt", "#space#", " "));
		return;
	}

	int iPeopleQuantity = sti(PChar.ColonyBuilding.Population);
	if(iPeopleQuantity <= 0) return;

	int iBaseMoney, iMoney;
	int iNalog = sti(PChar.ColonyBuilding.Nalog);

	int iMayorIndex = nMainCharacterIndex;
	if(GetCharacterIndex(PChar.ColonyBuilding.Hovernor) != -1)
	{
		iMayorIndex = GetCharacterIndex(PChar.ColonyBuilding.Hovernor);
	}

	ref mayor = &Characters[iMayorIndex];
	int iLeaderShip = GetCharacterSkill(mayor, SKILL_LEADERSHIP) * 5.5;
	int iCommerce = GetCharacterSkill(mayor, SKILL_COMMERCE) * 2.8;

	float fNalog = 1.1;
	switch(iNalog)
	{
		case 1: fNalog = 1.2; break;
		case 2: fNalog = 1.9; break;
		case 3: fNalog = 2.5; break;
	}

	iBaseMoney = (iLeaderShip + iCommerce) * fNalog;
	iMoney = iPeopleQuantity * iBaseMoney;

	if(iMoney < 0)
	{
		iMoney = 0;
	}

	if(iMoney > 1000000)
	{
		iMoney = 1000000;
	}

	ChangeColonyMoney(iMoney, false);
}

void ColonyCheckEpidemy()
{
	int iMedicament = sti(PChar.ColonyBuilding.Medicament);
	int iPopulationQuantity = sti(PChar.ColonyBuilding.Population);

	if(iPopulationQuantity < 20)
	{
		return;
	}

	int iPopulationIll = iPopulationQuantity / rand(6)+4;
	int temp = 0;

	if(iMedicament >= iPopulationIll)
	{
		temp = rand(10);
		ChangeColonyGoods(-(iPopulationIll-temp), "Medicament");
		ChangeColonyPopulation(-temp, false);
		ChangeColonyMorale(50, true);

		if(temp > 0)
		{
			Log_Info(GetConvertStrWithReplace("Variable_Colony_7", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
			AddQuestRecord("MY_COLONY", "11");
			AddQuestUserData("MY_COLONY", "sEpidemyDead", temp);
		}

		return;
	}
	else
	{
		temp = iPopulationIll - iMedicament;
		if(temp <= 0) { temp = 0; }

		ChangeColonyGoods(-temp, "Medicament");
		ChangeColonyPopulation(-temp, false);
		ChangeColonyMorale(-10, false);

		if(temp > 0)
		{
			Log_Info(GetConvertStrWithReplace("Variable_Colony_8", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
			AddQuestRecord("MY_COLONY", "12");
			AddQuestUserData("MY_COLONY", "sEpidemyDead", temp);
		}
	}
}

void ColonyCheckFoodNeed()
{
	int iPopulation = sti(PChar.ColonyBuilding.Population);
	int iFood = sti(PChar.ColonyBuilding.Food);
	int iPopulationDiff = makeint(iPopulation / COLONY_FOOD_POPULATION_DELIT);

	if (iPopulation <= 0) return;

	bool bNotice = false;
	if(iPopulationDiff < 1)
	{
		iPopulationDiff = 1;
	}

	int iFoodNeed = iPopulationDiff * 7;
	float fCoff = makefloat(iPopulationDiff) / makefloat(iPopulation);

	if (fCoff <= 0) return;

	int iDead = 0;
	if(iPopulation > 0)
	{
		if(iFood > iFoodNeed)
		{
			ChangeColonyGoods(-iFoodNeed, "Food");
		}
		else
		{
			if(!bNotice)
			{
				bNotice = true;
				AddQuestRecord("MY_COLONY", "16");
			}

			iDead = iFoodNeed - iFood;
			iDead = makeint(iDead/7);
			iDead = makeint(iDead/fCoff);

			ChangeColonyMorale(-20, false);

			if(iDead > iPopulation)
			{
				iDead = iPopulation;
			}

			ChangeColonyGoods(-iFood, "Food");
			ChangeColonyPopulation(-iDead, false);

			Log_Info(GetConvertStrWithReplace("Variable_Colony_9", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
			AddQuestRecord("MY_COLONY", "17");
			AddQuestUserData("MY_COLONY", "sFoodNeed", iDead);
		}
	}

	int iSoldiers = sti(PChar.ColonyBuilding.Soldiers);
	if(iSoldiers < 1)
	{
		return;
	}

	int iSoldiersDiff = makeint(iSoldiers / COLONY_FOOD_SOLDIERS_DELIT);
	iFood = sti(PChar.ColonyBuilding.Food);

	if(iSoldiersDiff < 1)
	{
		iSoldiersDiff = 1;
	}

	iFoodNeed = iSoldiersDiff * 7;
	fCoff = makefloat(iSoldiersDiff) / makefloat(iSoldiers);

	if(iSoldiers > 0)
	{
		if(iFood > iFoodNeed)
		{
			ChangeColonyGoods(-iFoodNeed, "Food");
		}
		else
		{
			if(!bNotice)
			{
				bNotice = true;
				AddQuestRecord("MY_COLONY", "16");
			}

			iDead = iFoodNeed - iFood;
			iDead = makeint(iDead/7);
			iDead = makeint(iDead/fCoff);

			if(iDead > iSoldiers)
			{
				iDead = iSoldiers;
			}

			ChangeColonyGoods(-iFood, "Food");
			ChangeColonySoldiers(-iDead, false);
			ChangeColonyMorale(-10, false);

			Log_Info(GetConvertStrWithReplace("Variable_Colony_10", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
			AddQuestRecord("MY_COLONY", "25");
			AddQuestUserData("MY_COLONY", "sFoodNeed", iDead);
		}
	}
}

int GetColonyFoodNeedDays()
{
	int iDays = 0;
	int iFood = sti(PChar.ColonyBuilding.Food);
	int iPopulation = sti(PChar.ColonyBuilding.Population);
	int iSoldiers = sti(PChar.ColonyBuilding.Soldiers);
	int iDay = (iPopulation / COLONY_FOOD_POPULATION_DELIT) + (iSoldiers / COLONY_FOOD_SOLDIERS_DELIT);

	if(iPopulation + iSoldiers <= 0)
	{
		return iFood;
	}

	if(iFood <= 0)
	{
		return 0;
	}

	if(iDay <= 0)
	{
		return iFood;
	}

	iDay *= 21;
	iDays = iFood / iDay;

	if(iDays < 0)
	{
		iDays = 0;
	}

	return iDays;
}

//-------------------------------------------------------------------------------
// НАПАДЕНИЯ НА КОЛОНИЮ
//-------------------------------------------------------------------------------
void CheckDefenceColony()
{
	Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_11", "Logs.txt", "#space#", " "));

	bool bDisable = GetDisableAllColonyEvent();
	if(PChar.ColonyBuilding.Defence == true || PChar.ColonyBuilding.Action == true || bDisable)
	{
		Log_TestInfo("Нападения не будет, т.к. идёт стройка или уже действует/готовится какое то нападение.");
		return;
	}

	int iRand = rand(5);

	int iMoney = sti(PChar.ColonyBuilding.Money);
	if(iMoney >= 1000000 && rand(1) == 1)
	{
		if(rand(10) > 5) iRand = 2; else iRand = 5;
	}

	if(iRand != 2 && iRand != 5)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_15", "Logs.txt", "#space#", " "));
		return;
	}
	else
	{
		if(PChar.ColonyBuilding.Stage == "1")
		{
			CreateLandDefenceColony();
			Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_16", "Logs.txt", "#space#", " "));
			return;
		}
		else
		{

			if(iRand == 2)
			{
				CreateSeaDefenceColony();
				Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_17", "Logs.txt", "#space#", " "));
				return;
			}

			if(iRand == 5)
			{
				CreateLandDefenceColony();
				Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_18", "Logs.txt", "#space#", " "));
				return;
			}
		}
	}
}

//-------------------------------------------------------------------------------
// ПЕРВЫЙ ВАРИАНТ - МЯТЕЖ
//-------------------------------------------------------------------------------
void CreateMutinyColony()
{
	Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_19", "Logs.txt", "#space#", " "));

	int iPopulationQuantity = sti(PChar.ColonyBuilding.Population);
	int iPopulationMorale = sti(PChar.ColonyBuilding.Population.Morale);

	if(iPopulationQuantity <= 100)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_20", "Logs.txt", "#space#", " "));
		return;
	}

	if(rand(100) < 20)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_21", "Logs.txt", "#space#", " "));
		return;
	}

	if(PChar.ColonyBuilding.Defence == true || PChar.ColonyBuilding.Action == true)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_22", "Logs.txt", "#space#", " "));
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_23", "Logs.txt", "#space#", " ") + sti(PChar.ColonyBuilding.Action));
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_24", "Logs.txt", "#space#", " ") + sti(PChar.ColonyBuilding.Defence));
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_25", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.Defence.Type);
		return;
	}

	// Включаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = true;
	// Включаем блокировку всех нападений <--

	PChar.ColonyBuilding.Defence = true;
	PChar.ColonyBuilding.Defence.Type = "Mutiny";

	int iSoldiersQuantity = sti(PChar.ColonyBuilding.Soldiers) / 100;
	int iRebels = iPopulationQuantity / 60;

	if(iSoldiersQuantity < 1) { InstantColonyDefendLoose("Colony", "Mutiny"); return; }

	if(iRebels < 2) iRebels = 2;
	if(iRebels > 20) iRebels = 20;

	ChangeColonyPopulation(-(iRebels*15), false);

	if(iSoldiersQuantity < 2) iSoldiersQuantity = 2;
	if(iSoldiersQuantity > 20) iSoldiersQuantity = 25;

	PChar.ColonyBuilding.Defence.Nation = PIRATE;
	PChar.ColonyBuilding.Defence.Soldiers = iSoldiersQuantity;
	PChar.ColonyBuilding.Defence.Soldiers.Delete = rand(3) + 3;
	PChar.ColonyBuilding.Defence.EnemyQuantity = iRebels;

	Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_26", "Logs.txt", "#space#", " ") + iRebels);
	Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_27", "Logs.txt", "#space#", " ") + iSoldiersQuantity);

	int iRandMinus = 2;
	if(iRebels > iSoldiersQuantity)
	{
		iRandMinus -= (iRebels - iSoldiersQuantity);
	}
	else
	{
		iRandMinus += (iSoldiersQuantity - iRebels);
	}

	if(iRandMinus <= -7) { InstantColonyDefendLoose("Colony", "Mutiny"); return; }

	int iRand = iRandMinus - rand(2);

	int iEnd = 0;
	if(iRand > 0) { iEnd = 1; }else{ iEnd = 0; }


	PChar.ColonyBuilding.Defence.End = iEnd;

	int iTime = 7 + rand(10) + iRandMinus;
	if(iTime < 7) { iTime = 7; }
	if(iTime > 30) { iTime = 30; }

	string sClearQuest = "ClearMutinyColony_" + rand(1000000);
	PChar.ColonyBuilding.ClearQuest = sClearQuest;

	PChar.quest.(sClearQuest).win_condition.l1 = "Timer";
	PChar.quest.(sClearQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.hour = 12;
	PChar.quest.(sClearQuest).function = "ClearMutinyColony";

	Log_Info(GetConvertStrWithReplace("Variable_Colony_28", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");

	string sTime = FindRussianDaysString(iTime);

	AddQuestRecord("MY_COLONY", "1");
	AddQuestUserData("MY_COLONY", "sMutinyTime", sTime);

	string sBattleQuest = "BattleMutinyColony_" + rand(1000000);
	PChar.ColonyBuilding.BattleQuest = sBattleQuest;

	PChar.quest.(sBattleQuest).win_condition.l1 = "location";
	PChar.quest.(sBattleQuest).win_condition.l1.location = "Caiman_town";
	PChar.quest.(sBattleQuest).function = "BattleMutinyColony";

	SetColonyMessageInfo("Colony", "Mutiny", "", PIRATE, iTime, true, true);
}

void BattleMutinyColony(string _tmp)
{
	string sClearQuest = PChar.ColonyBuilding.ClearQuest;
	PChar.quest.(sClearQuest).over = "yes";

	chrDisableReloadToLocation = true;
	bDisableFastReload = true;
	LAi_grp_alarmactive = true;
	InterfaceStates.Buttons.Save.enable = false;
	PChar.ColonyBuilding.Defence.Battle = true;

	int i = 0;
	int iSoldiers = sti(PChar.ColonyBuilding.Defence.Soldiers);
	int iRebels = sti(PChar.ColonyBuilding.Defence.EnemyQuantity);
	ref chr;

	if(PChar.ColonyBuilding.Stage != "3")
	{
		ChangeCharacterAddress(CharacterFromID("Builder"), "none", "none");
	}

	if(PChar.ColonyBuilding.Stage == "1" && PChar.location.locator == "reload6")
	{
		ChangeCharacterAddressGroup(PChar, PChar.location, "rld", "aloc0");
	}

	string sModel = "";
	string sLocator = "";

	for(i=1; i <= iRebels; i++)
	{
		sModel = "citiz_" + (rand(11)+1);

		if(PChar.ColonyBuilding.Stage == "1")
		{
			sLocator = "reload6";
		}
		else
		{
			sLocator = "reload3_back";
		}

		chr = GetCharacter(NPC_GeneratePhantomCharacter("officer_1", PIRATE, 1, 0));
		chr.rank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + 2;
		SetFantomParamFromRank(chr, sti(chr.rank), true);
		chr.id = "My Colony Mutiny Rebel " + i;
		chr.model = sModel;
		chr.model.animation = "man";
		chr.NotRemoved = true;
		SetNewModelToChar(chr);
    		FaceMaker(chr);
		LAi_NoRebirthEnable(chr);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", sLocator);
	}

	int iNation = sti(PChar.ColonyBuilding.ColonyNation);
	string sNation = NationShortName(iNation);

	int iMusketeers = FightInColonyGetMusketeerQty(iSoldiers);
	bool bMusketeer = false;

	for(i=1; i <= iSoldiers; i++)
	{

		bMusketeer = false;
		if(iSoldiers - i < iMusketeers) { bMusketeer = true; }

		if(PChar.ColonyBuilding.SoldiersType == "officer")
		{
			sModel = "officer_" + (rand(42) + 1);

			if(bMusketeer) { sModel = "officer_" + (rand(42) + 1); }
		}
		else
		{
			if(PChar.ColonyBuilding.SoldiersType == "pirate")
			{
				sModel = "pirate_" + (rand(24) + 1);

				if(bMusketeer) { sModel = GetPirateMushketerModel(); }
			}
			else
			{
				sModel = "sold_" + sNation + "_" + (rand(7) + 1);

				if(bMusketeer) { sModel = sNation + "_rifle_" + (rand(2)+1); }
			}
		}

		if(PChar.ColonyBuilding.Stage == "1")
		{
			sLocator = "reload3";
		}
		else
		{
			sLocator = "gate_back";
		}

		chr = GetCharacter(NPC_GeneratePhantomCharacter("officer_1", PIRATE, MAN, 0));
		chr.rank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + 4;
		SetFantomParamFromRank(chr, sti(chr.rank), true);
		chr.id = "My Colony Mutiny Defender " + i;
		chr.model = sModel;
		chr.model.animation = "man";
		if (bMusketeer) ConfigureGeneratedMusket(chr);
		chr.NotRemoved = true;
		SetNewModelToChar(chr);
		LAi_NoRebirthEnable(chr);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", sLocator);
	}

	LAi_group_SetHearRadius("EnemyFight", 100.0);
	LAi_group_FightGroupsEx("EnemyFight", LAI_GROUP_PLAYER, true, PChar, -1, false, false);
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_SetFightMode(PChar, true);

	string cnd, rbl;
	for(i=1; i<=iRebels; i++)
	{
		cnd = "l" + i;
		rbl = "My Colony Mutiny Rebel " + i;
		PChar.quest.BattleMutinyColonyWinner.win_condition.(cnd) = "NPC_Death";
		PChar.quest.BattleMutinyColonyWinner.win_condition.(cnd).character = rbl;
	}

	PChar.quest.BattleMutinyColonyWinner.function = "BattleMutinyColonyWinner";

	ref loc = PCharLoc();
	DeleteAllFantomCharactersFromLocation(loc);

	SetDeadmansToLocation(&Locations[FindLocation(PChar.location)], "citizens");
}

void BattleMutinyColonyWinner(string _tmp)
{
	InterfaceStates.Buttons.Save.enable = true;
	chrDisableReloadToLocation = false;
	bDisableFastReload = false;
	LAi_grp_alarmactive = false;
	PChar.ColonyBuilding.Defence.Battle = false;

	LAi_LockFightMode(PChar, false);
        LAi_SetFightMode(PChar, false);

	if(PChar.ColonyBuilding.Stage != "3")
	{
		ChangeCharacterAddressGroup(CharacterFromID("Builder"), "Caiman_town", "goto", "goto8");
	}

	AddQuestRecord("MY_COLONY", "2");
	ChangeColonyMorale(50, true);

	PChar.ColonyBuilding.Defence = false;
	PChar.ColonyBuilding.Defence.Type = "";

	int iSoldiers = sti(PChar.ColonyBuilding.Defence.Soldiers);
	string sDefender = "";
	int iDead = 0;
	for(int i=1; i<=iSoldiers; i++)
	{
		sDefender = "My Colony Mutiny Defender " + i;

		if(LAi_IsDead(CharacterFromID(sDefender)))
		{
			iDead++;
		}
	}

	int iSoldiersDead = iDead * 50;
	ChangeColonySoldiers(-iSoldiersDead, false);

	// Выключаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = false;
	// Выключаем блокировку всех нападений <--

	AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_1", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_2", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_3", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_4", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_5", "AddRumours.txt", "#space#", " ") + GetMainCharacterNameDat() + "!", 5, 1);
}

void ClearMutinyColony(string qName)
{
	string sBattleQuest = PChar.ColonyBuilding.BattleQuest;
	PChar.quest.(sBattleQuest).over = "yes";

	PChar.ColonyBuilding.Defence = false;
	PChar.ColonyBuilding.Defence.Type = "";
	PChar.ColonyBuilding.Defence.DeadInTown = true;
	SetTimerCondition("ColonyNotSetDead", 0, 0, 6, false);
	PChar.ColonyBuilding.Defence.Battle = false;

	int iEnd = sti(PChar.ColonyBuilding.Defence.End);
	int iSoldiers = sti(PChar.ColonyBuilding.Defence.Soldiers);

	if(iSoldiers > 0)
	{
		iSoldiers /= sti(PChar.ColonyBuilding.Defence.Soldiers.Delete);
	}

	int iSoldiersDead = iSoldiers * 50;

	int iRebels = sti(PChar.ColonyBuilding.Defence.EnemyQuantity);
	int iMoney = sti(PChar.ColonyBuilding.Money);

	if(iEnd == 1)
	{
		Log_Info(GetConvertStrWithReplace("Variable_Colony_29", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
		AddQuestRecord("MY_COLONY", "2");
		ChangeColonySoldiers(-iSoldiersDead, false);
		ChangeColonyMorale(50, true);
		SetColonyMessageInfo("Colony", "Mutiny", "Good", PIRATE, 0, false, true);
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_6", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_7", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_8", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_9", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_10", "AddRumours.txt", "#space#", " "), 5, 1);
	}
	else
	{
		Log_Info(GetConvertStrWithReplace("Variable_Colony_30", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
		AddQuestRecord("MY_COLONY", "3");
		ChangeColonySoldiers(0, true);
		ChangeColonyMorale(0, true);
		ChangeColonyMoney(-(iMoney/iRebels), false);
		SetColonyMessageInfo("Colony", "Mutiny", "", PIRATE, 0, false, true);
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_11", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_12", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_13", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_14", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_15", "AddRumours.txt", "#space#", " "), 5, 1);
	}

	if(PChar.ColonyBuilding.Stage != "3")
	{
		ChangeCharacterAddressGroup(CharacterFromID("Builder"), "Caiman_town", "goto", "goto8");
	}

	// Выключаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = false;
	// Выключаем блокировку всех нападений <--
}

//-------------------------------------------------------------------------------
// ВТОРОЙ ВАРИАНТ - НАПАДЕНИЕ С СУШИ
//-------------------------------------------------------------------------------
void CreateLandDefenceColony()
{
	if(rand(100) < 5)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_31", "Logs.txt", "#space#", " "));
		return;
	}

	if(PChar.ColonyBuilding.Fort == true && rand(100) < 25)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_32", "Logs.txt", "#space#", " "));
		return;
	}

	int iBaseNation = GetBaseHeroNation();
	int iNation = FindEnemyNation2Nation(iBaseNation); //FindEnemyNation2Character(nMainCharacterIndex);
	if(iNation == -1)
	{
		iNation = PIRATE;
	}

	PChar.ColonyBuilding.Defence.Nation = iNation;

	// Включаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = true;
	// Включаем блокировку всех нападений <--

	PChar.ColonyBuilding.Defence = true;
	PChar.ColonyBuilding.Defence.Type = "BattleLand";

	int iSoldiersQuantity = sti(PChar.ColonyBuilding.Soldiers) / 100;
	int iEnemy = 0;

	if(PChar.ColonyBuilding.Stage == "1")
	{
		iEnemy = 5 + rand(4) + rand(4);
	}
	else
	{
		iEnemy = 7 + rand(4) + rand(6) + rand(3) + 5;
	}

	int iCitizen = 0;
	if(sti(PChar.ColonyBuilding.Population.Morale) >= 70)
	{
		iCitizen = sti(PChar.ColonyBuilding.Population) / 70;
	}

	string sDestroyBuild = "none";
	if(rand(5) > 3)
	{
		sDestroyBuild = GetColonyBuildDestroy();
	}
	PChar.ColonyBuilding.Defence.DestroyBuild = sDestroyBuild;
	SetBestGoodForDeleteFromMyColony();

	if(iSoldiersQuantity < 1) { InstantColonyDefendLoose("Colony", "BattleLand"); return; }

	if(iCitizen < 0) iCitizen = 0;
	if(iCitizen > 7) iCitizen = 7;

	if(iSoldiersQuantity < 2) iSoldiersQuantity = 2;
	if(iSoldiersQuantity > 22) iSoldiersQuantity = 22;

	if(iEnemy < 2) iEnemy = 2;
	if(iEnemy > 20) iEnemy = 20;

	Log_TestInfo("Команда гарнизона + жителей: " + (iSoldiersQuantity + iCitizen));
	Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_33", "Logs.txt", "#space#", " ") + iEnemy);
	if(PChar.ColonyBuilding.Fort == true)
	{
		iEnemy -= (sti(PChar.ColonyBuilding.Soldiers) / 10) / iEnemy;

		if(iEnemy < 10)
		{
			iEnemy = 10;
		}

		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_34", "Logs.txt", "#space#", " ") + iEnemy);
	}

	PChar.ColonyBuilding.Defence.Soldiers = iSoldiersQuantity;
	PChar.ColonyBuilding.Defence.Citizens = iCitizen;

	PChar.ColonyBuilding.Defence.Soldiers.Delete = rand(2) + 2;
	PChar.ColonyBuilding.Defence.Citizens.Delete = rand(3) + 3;

	PChar.ColonyBuilding.Defence.EnemyQuantity = iEnemy;

	int iRandMinus = -2;
	if(iEnemy > iSoldiersQuantity)
	{
		iRandMinus -= (iEnemy - (iSoldiersQuantity+iCitizen));
	}
	else
	{
		iRandMinus += ((iSoldiersQuantity+iCitizen) - iEnemy);
	}

	if(iRandMinus < -10) { InstantColonyDefendLoose("Colony", "BattleLand"); return; }

	int iRand = iRandMinus - rand(2);

	int iEnd = 0;
	if(iRand > 0) { iEnd = 1; }else{ iEnd = 0; }

	PChar.ColonyBuilding.Defence.End = iEnd;

	int iTime = 7 + rand(5) + iRandMinus;
	if(iTime < 7) { iTime = 7; }
	if(iTime > 30) { iTime = 30; }

	string sClearQuest = "ClearLandDefenceColony_" + rand(1000000);
	PChar.ColonyBuilding.ClearQuest = sClearQuest;

	PChar.quest.(sClearQuest).win_condition.l1 = "Timer";
	PChar.quest.(sClearQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.hour = 12;
	PChar.quest.(sClearQuest).function = "ClearLandDefenceColony";

	string sTime = FindRussianDaysString(iTime);

	Log_Info(GetConvertStrWithReplace("Variable_Colony_35", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
	AddQuestRecord("MY_COLONY", "4");
	AddQuestUserData("MY_COLONY", "sLandBattleTime", sTime);

	string sLocation, sEnemyLocator, sSoldiersLocator, sCitizenLocator;
	if(PChar.ColonyBuilding.Stage == "1")
	{
		sLocation = "Caiman_ExitTown";
		sEnemyLocator = "reload1_back";
		sSoldiersLocator = "reload3";
		sCitizenLocator = "reload3";

		Locations[FindLocation(sLocation)].hidden_effects = true;
	}
	else
	{
		if(PChar.ColonyBuilding.Plantation == true)
		{
			sLocation = "Caiman_Plantation";
			sEnemyLocator = "reload2_back";
			sSoldiersLocator = "reload1_back";
			sCitizenLocator = "reload1_back";
		}
		else
		{
			sLocation = "Caiman_ExitTown";
			sEnemyLocator = "reload2_back";
			sSoldiersLocator = "reload3";
			sCitizenLocator = "reload3";
		}
	}

	string sBattleQuest = "BattleLandDefenceColony_" + rand(1000000);
	PChar.ColonyBuilding.BattleQuest = sBattleQuest;

	PChar.quest.(sBattleQuest).win_condition.l1 = "location";
	PChar.quest.(sBattleQuest).win_condition.l1.location = sLocation;
	PChar.quest.(sBattleQuest).function = "BattleLandDefenceColony";

	PChar.ColonyBuilding.Defence.Locator.Enemy = sEnemyLocator;
	PChar.ColonyBuilding.Defence.Locator.Soldiers = sSoldiersLocator;
	PChar.ColonyBuilding.Defence.Locator.Citizen = sCitizenLocator;

	SetColonyMessageInfo("Colony", "BattleLand", "", iNation, iTime, true, true);
}

void BattleLandDefenceColony(string _tmp)
{
	string sClearQuest = PChar.ColonyBuilding.ClearQuest;
	PChar.quest.(sClearQuest).over = "yes";

	chrDisableReloadToLocation = true;
	bDisableFastReload = true;
	LAi_grp_alarmactive = true;
	InterfaceStates.Buttons.Save.enable = false;
	PChar.ColonyBuilding.Defence.Battle = true;

	int i = 0;
	int iSoldiers = sti(PChar.ColonyBuilding.Defence.Soldiers);
	int iCitizens = sti(PChar.ColonyBuilding.Defence.Citizens);
	int iEnemy = sti(PChar.ColonyBuilding.Defence.EnemyQuantity);

	ref chr;

	string sModel = "";
	string sLocator = "";

    	arrayNPCModelHow = 0;
	int iEnemyNation = sti(PChar.ColonyBuilding.Defence.Nation);
	string sEnemyNation = NationShortName(iEnemyNation);

	if(PChar.ColonyBuilding.Stage == "1")
	{
		ChangeCharacterAddressGroup(PChar, PChar.location, "rld", "aloc0");
	}
	else
	{
		ChangeCharacterAddressGroup(PChar, PChar.location, "rld", "loc0");
	}

	int iMusketeers = FightInColonyGetMusketeerQty(iEnemy);
	bool bMusketeer = false;

	for(i=1; i <= iEnemy; i++)
	{

		bMusketeer = false;
		if(iEnemy - i < iMusketeers) { bMusketeer = true; }

		sLocator = PChar.ColonyBuilding.Defence.Locator.Enemy;

		if(iEnemyNation == PIRATE)
		{
			sModel = "pirate_" + (rand(24) + 1);
			if(bMusketeer) { sModel = GetPirateMushketerModel(); }
		}
		else
		{
			sModel = "sold_" + sEnemyNation + "_" + (rand(7) + 1);
			if(bMusketeer) { sModel = sEnemyNation + "_rifle_" + (rand(2)+1); }
		}

		chr = GetCharacter(NPC_GeneratePhantomCharacter("soldier", iEnemyNation, MAN, 0));
		chr.rank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + 5;
		SetFantomParamFromRank(chr, sti(chr.rank), true);
		chr.id = "My Colony Land Defence Pirate " + i;
		chr.model = sModel;
		chr.model.animation = "man";
		if (bMusketeer) ConfigureGeneratedMusket(chr);
		chr.NotRemoved = true;
		SetNewModelToChar(chr);
    		FaceMaker(chr);
		LAi_NoRebirthEnable(chr);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", sLocator);
	}

	int iNation = sti(PChar.ColonyBuilding.ColonyNation);
	string sNation = NationShortName(iNation);

	iMusketeers = FightInColonyGetMusketeerQty(iSoldiers);
	for(i=1; i <= iSoldiers; i++)
	{

		bMusketeer = false;
		if(iSoldiers - i < iMusketeers) { bMusketeer = true; }

		if(PChar.ColonyBuilding.SoldiersType == "officer")
		{
			sModel = "officer_" + (rand(42) + 1);

			if(bMusketeer) { sModel = "officer_" + (rand(42) + 1); }
		}
		else
		{
			if(PChar.ColonyBuilding.SoldiersType == "pirate")
			{
				sModel = "pirate_" + (rand(24) + 1);

				if(iEnemyNation == PIRATE)
				{
					sModel = "officer_" + (rand(42) + 1);
				}

				if(bMusketeer) { sModel = "Mushketer_" + (rand(4)+1); }
			}
			else
			{
				sModel = "sold_" + sNation + "_" + (rand(7) + 1);

				if(bMusketeer) { sModel = sNation + "_rifle_" + (rand(2)+1); }

				if(iNation == iEnemyNation)
				{
					sModel = "officer_" + (rand(42) + 1);
					if(bMusketeer) { sModel = "officer_" + (rand(42) + 1); }
				}
			}
		}

		sLocator = PChar.ColonyBuilding.Defence.Locator.Soldiers;
		chr = GetCharacter(NPC_GeneratePhantomCharacter("officer_1", PIRATE, MAN, 0));
		chr.rank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + 4;
		SetFantomParamFromRank(chr, sti(chr.rank), true);
		chr.id = "My Colony Mutiny Defender " + i;
		chr.model = sModel;
		chr.model.animation = "man";
		if (bMusketeer) ConfigureGeneratedMusket(chr);
		chr.NotRemoved = true;
		SetNewModelToChar(chr);
		LAi_NoRebirthEnable(chr);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", sLocator);
	}

	for(i=1; i <= iCitizens; i++)
	{
		sModel = "citiz_" + (rand(11)+1);
		sLocator = PChar.ColonyBuilding.Defence.Locator.Citizen;

		chr = GetCharacter(NPC_GeneratePhantomCharacter("citiz_1", PIRATE, 1, 0));
		chr.id = "My Colony Mutiny Cit Defender " + i;
		chr.rank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + 2;
		SetFantomParamFromRank(chr, sti(chr.rank), true);
		chr.model = sModel;
		chr.model.animation = "man";
		chr.NotRemoved = true;
		SetNewModelToChar(chr);
		LAi_NoRebirthEnable(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
		ChangeCharacterAddressGroup(chr, PChar.location, "reload", sLocator);
	}

	LAi_group_SetHearRadius("EnemyFight", 100.0);
	LAi_group_FightGroupsEx("EnemyFight", LAI_GROUP_PLAYER, true, PChar, -1, false, false);
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_SetFightMode(PChar, true);

	string cnd, enm;
	for(i=1; i<=iEnemy; i++)
	{
		cnd = "l" + i;
		enm = "My Colony Land Defence Pirate " + i;
		PChar.quest.BattleLandDefenceColonyWinner.win_condition.(cnd) = "NPC_Death";
		PChar.quest.BattleLandDefenceColonyWinner.win_condition.(cnd).character = enm;
	}

	PChar.quest.BattleLandDefenceColonyWinner.function = "BattleLandDefenceColonyWinner";
}

void BattleLandDefenceColonyWinner(string _tmp)
{
	InterfaceStates.Buttons.Save.enable = true;
	chrDisableReloadToLocation = false;
	bDisableFastReload = false;
	LAi_grp_alarmactive = false;
	PChar.ColonyBuilding.Defence.Battle = false;

	LAi_LockFightMode(PChar, false);
        LAi_SetFightMode(PChar, false);

	AddQuestRecord("MY_COLONY", "5");
	AddQuestUserData("MY_COLONY", "sSex", GetSexPhrase("","а")); // LEO: регистрируем sSex ПОСЛЕ записи, иначе @<sSex> в тексте пустой

	ChangeColonyMorale(70, true);

	int iSoldiers = sti(PChar.ColonyBuilding.Defence.Soldiers);
	int iCitizens = sti(PChar.ColonyBuilding.Defence.Citizens);

	string sDefender = "";
	int iDead = 0;
	int i = 0;
	for(i=1; i<=iSoldiers; i++)
	{
		sDefender = "My Colony Mutiny Defender " + i;

		if(LAi_IsDead(CharacterFromID(sDefender)))
		{
			iDead++;
		}
	}

	int iSoldiersDead = iDead * 50;
	ChangeColonySoldiers(-iSoldiersDead, false);

	iDead = 0;
	for(i=1; i<=iCitizens; i++)
	{
		sDefender = "My Colony Mutiny Cit Defender " + i;
		if(LAi_IsDead(CharacterFromID(sDefender)))
		{
			iDead++;
		}
	}

	iSoldiersDead = iDead * 35;
	ChangeColonyPopulation(-iSoldiersDead, false);

	PChar.ColonyBuilding.Defence = false;
	PChar.ColonyBuilding.Defence.Type = "";

	if(CheckAttribute(&Locations[FindLocation(PChar.Location)], "hidden_effects"))
	{
		DeleteAttribute(&Locations[FindLocation(PChar.Location)], "hidden_effects");
	}

	// Выключаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = false;
	// Выключаем блокировку всех нападений <--

	AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_16", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_17", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_18", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_19", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_20", "AddRumours.txt", "#space#", " "), 5, 1);
}

void ClearLandDefenceColony(string qName)
{
	int iEnd = sti(PChar.ColonyBuilding.Defence.End);

	int iSoldiers = sti(PChar.ColonyBuilding.Defence.Soldiers);
	int iCitizens = sti(PChar.ColonyBuilding.Defence.Citizens);

	if(iSoldiers > 0)
	{
		iSoldiers /= sti(PChar.ColonyBuilding.Defence.Soldiers.Delete);
	}

	if(iCitizens > 0)
	{
		iCitizens /= sti(PChar.ColonyBuilding.Defence.Citizens.Delete);
	}

	int iSoldiersDead = iSoldiers*50;
	int iCitizensDead = iCitizens*35;

	if(iEnd == 1)
	{
		Log_Info(GetConvertStrWithReplace("Variable_Colony_36", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
		AddQuestRecord("MY_COLONY", "6");

		ChangeColonySoldiers(-iSoldiersDead, false);
		ChangeColonyPopulation(-iCitizensDead, false);
		ChangeColonyMorale(70, true);
		DeleteBestGoodFromMyColony(false);
		SetColonyMessageInfo("Colony", "BattleLand", "Good", PIRATE, 0, false, true);

		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_21", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_22", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_23", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_24", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_25", "AddRumours.txt", "#space#", " "), 5, 1);
	}
	else
	{
		Log_Info(GetConvertStrWithReplace("Variable_Colony_37", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
		AddQuestRecord("MY_COLONY", "7");
		ChangeColonyPopulation(-iCitizensDead, false);
		ChangeColonySoldiers(0, true);
		ChangeColonyMorale(20, true);
		ChangeColonyMoney(0, true);
		PChar.ColonyBuilding.Defence.DeadInTown = true;
		SetTimerCondition("ColonyNotSetDead", 0, 0, 6, false);

		Locations[FindLocation("Caiman_town")].hidden_effects = true;

		string sDestroyBuild = PChar.ColonyBuilding.Defence.DestroyBuild;
		DestroyBuildInCaimanColony(sDestroyBuild);
		DeleteBestGoodFromMyColony(true);

		SetColonyMessageInfo("Colony", "BattleLand", "", PIRATE, 0, false, true);
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_26", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_27", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_28", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_29", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_30", "AddRumours.txt", "#space#", " "), 5, 1);
	}

	string sBattleQuest = PChar.ColonyBuilding.BattleQuest;
	PChar.quest.(sBattleQuest).over = "yes";

	PChar.ColonyBuilding.Defence.Battle = false;
	PChar.ColonyBuilding.Defence = false;
	PChar.ColonyBuilding.Defence.Type = "";

	// Выключаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = false;
	// Выключаем блокировку всех нападений <--
}

//-------------------------------------------------------------------------------
// ТРЕТИЙ ВАРИАНТ - НАПАДЕНИЕ С МОРЯ
//-------------------------------------------------------------------------------
void CreateSeaDefenceColony()
{
	if(rand(100) < 5)
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_Colony_38", "Logs.txt", "#space#", " "));
		return;
	}

	// Включаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = true;
	// Включаем блокировку всех нападений <--

	PChar.ColonyBuilding.Defence = true;
	PChar.ColonyBuilding.Defence.Type = "BattleSea";

	int iBaseNation = GetBaseHeroNation();
	int iNation = FindEnemyNation2Nation(iBaseNation); //FindEnemyNation2Character(nMainCharacterIndex);
	if(iNation == -1)
	{
		iNation = PIRATE;
	}

	PChar.ColonyBuilding.Defence.Nation = iNation;

	int iSquadronQuantity = rand(3) + 2;
	PChar.ColonyBuilding.Defence.ShipQuantity = iSquadronQuantity;

	int iChar = 0;
	ref chr;
	int iShipType = 0;

	int iRank = sti(PChar.rank) + MOD_SKILL_ENEMY_RATE*3 + rand(MOD_SKILL_ENEMY_RATE*3);
	int iFencing = func_max(sti(PChar.skill.FencingLight), sti(PChar.skill.Fencing), sti(PChar.skill.FencingHeavy));
	int iPistol = sti(PChar.skill.Pistol);

	SetBestGoodForDeleteFromMyColony();

	string sDestroyBuild = "none";
	if(rand(5) > 3)
	{
		sDestroyBuild = GetColonyBuildDestroy();
	}
	PChar.ColonyBuilding.Defence.DestroyBuild = sDestroyBuild;
	SetBestGoodForDeleteFromMyColony();
	string sCapId = "ColonySeaAttacker";
	Group_FindOrCreateGroup("Sea_ColonySeaAttacker1");
	Group_SetType("Sea_ColonySeaAttacker1", "war");

	ref rShip;
	int iEnemyClass = 0;
	
	string nationmodel;
	switch (iNation)
	{
		case ENGLAND:
			nationmodel = "off_eng_"+(rand(1)+1);
		break;
		case FRANCE:
			nationmodel = "off_fra_"+(rand(1)+1);
		break;
		case SPAIN:
			nationmodel = "off_spa_"+(rand(1)+1);
		break;
		case HOLLAND:
			nationmodel = "off_hol_"+(rand(1)+1);
		break;
		case PIRATE:
			nationmodel = "pirate_" + (rand(24)+1);
		break;
	}

	for(int i=1; i <= iSquadronQuantity; i++)
	{
		iChar = NPC_GenerateCharacter(sCapId+i, nationmodel, "man", "man", iRank, iNation, -1, true);
		chr = GetCharacter(iChar);

		iShipType = GetShipTypeForColonyBattleSea(chr);
		chr.Ship.Type = GenerateShipExt(iShipType, false, chr);
		chr.AlwaysEnemy = true;
		chr.Coastal_Captain = true;
		chr.AnalizeShips = true;
		chr.AlwaysSandbankManeuver = true;
		chr.DontRansackCaptain = true;
		SetRandomNameToShip(chr);
		SetBaseShipData(chr);
		SetCrewQuantityFull(chr);		
		Fantom_SetBalls(chr, "pirate");
		SetFantomParamHunter(chr);
		FantomMakeCoolFighter(chr, iRank, iFencing, iPistol, BLADE_LONG, "pistol3", 100);
		chr.SinkTenPercent = true;
		SetGeraldDetails(chr, -1, rand(7), "", "");
		SetRandGeraldSail(chr, iNation);

		Group_AddCharacter("Sea_ColonySeaAttacker1", sCapId+i);

		if(i == 1)
		{
			Group_SetGroupCommander("Sea_ColonySeaAttacker1", sCapId+i);
		}

		rShip = GetRealShip(sti(chr.Ship.Type));
		iEnemyClass += sti(rShip.Class);
	}

	int iRes = 5 - iSquadronQuantity;
	for(int e=1; e <= iRes; e++)
	{
		iEnemyClass += 7;
	}

	ref rGuardian;
	string sGuardian = "";
	int iGuardianClass = 7;

	for(int g=1; g <= 4; g++)
	{
		sGuardian = "Ship" + g;

		iChar = sti(PChar.Colony.Guardians.(sGuardian));
		if(iChar == -1)
		{
			iGuardianClass += 7;
			continue;
		}
		else
		{
			iChar = sti(PChar.Colony.Guardians.(sGuardian));
			rGuardian = GetCharacter(iChar);
			rShip = GetRealShip(sti(rGuardian.Ship.Type));
			iGuardianClass += sti(rShip.Class);
		}
	}

	if(iGuardianClass - iEnemyClass >= 8)
	{
		BattleSeaDefenceColonyClearAttackers();
		InstantColonyDefendLoose("Colony", "BattleSea");
		return;
	}

	int iEnd = 0;
	if(iGuardianClass <= iEnemyClass)
	{
		iEnd = 1;
	}

	if(rand(9) > 8)
	{
		iEnd = 0;
	}

	PChar.ColonyBuilding.Defence.End = iEnd;
	PChar.ColonyBuilding.Defence.KillShips = rand(1) * rand(1) * rand(1) * rand(1);

	int iTime = 10 + rand(5) - iGuardianClass;
	if(iTime < 7) { iTime = 7; }
	if(iTime > 30) { iTime = 30; }

	string sClearQuest = "ClearBattleSeaColony_" + rand(1000000);
	PChar.ColonyBuilding.ClearQuest = sClearQuest;

	PChar.quest.(sClearQuest).win_condition.l1 = "Timer";
	PChar.quest.(sClearQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, iTime);
	PChar.quest.(sClearQuest).win_condition.l1.date.hour = 12;
	PChar.quest.(sClearQuest).function = "ClearBattleSeaColony";

	Log_Info(GetConvertStrWithReplace("Variable_Colony_39", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");

	string sTime = FindRussianDaysString(iTime);

	AddQuestRecord("MY_COLONY", "8");
	AddQuestUserData("MY_COLONY", "sSeaBattleTime", sTime);

	string sNationName = GetNationNameByType(iNation) + "Gen";
	AddQuestUserData("MY_COLONY", "sNationName", XI_ConvertString(sNationName));

	string sBattleQuest = "BattleSeaDefenceColony_" + rand(1000000);
	PChar.ColonyBuilding.BattleQuest = sBattleQuest;

	PChar.quest.(sBattleQuest).win_condition.l1 = "location";
	PChar.quest.(sBattleQuest).win_condition.l1.location = "Caiman";
	PChar.quest.(sBattleQuest).function = "BattleSeaDefenceColony";

	SetColonyMessageInfo("Colony", "BattleSea", "", iNation, iTime, true, true);
}

void BattleSeaDefenceColonyClearAttackers()
{
	ref chr;
	string sCharacter = "";

	for(int i = 1; i <= 5; i++)
	{
		sCharacter = "ColonySeaAttacker_" + i;
		if(GetCharacterIndex(sCharacter) == -1)
		{
			continue;
		}

		chr = CharacterFromID(sCharacter);
		chr.LifeDay = 0;
	}
}

void BattleSeaDefenceColony(string _tmp)
{
	Island_SetReloadEnableGlobal("Caiman", false);

	string sGroup = "";
	string sShip = "";
	for(int i=1; i<=4; i++)
	{
		sGroup = "InterfaceColonyGuardians_" + i;
		sShip = "Ship" + i;
		if(Group_FindGroup(sGroup) != -1 && PChar.Colony.Guardians.(sShip).Active == true)
		{
			if(i != 1 && Group_FindGroup("InterfaceColonyGuardians_1") != -1)
			{
				Group_SetPursuitGroup(sGroup, "InterfaceColonyGuardians_1");
			}
			else
			{
				Group_SetAddress(sGroup, "Caiman", "quest_ships", "Quest_ship_" + (7-i));
			}


			for(q=1; q<=Group_GetCharactersNum("Sea_ColonySeaAttacker1"); q++)
			{
				iCharacter = Group_GetCharacterIndexR(Group_GetGroupByID("Sea_ColonySeaAttacker1"), i);
			}

			Group_SetTaskAttack(sGroup, "Sea_ColonySeaAttacker1");
			Group_LockTask(sGroup);
		}
	}

	Group_SetAddress("Sea_ColonySeaAttacker1", "Caiman", "quest_ships", "Quest_ship_7");

	int q=0;
	int iCharacter = 0;
	for(q=1; q<=4; q++)
	{
		sGroup = "InterfaceColonyGuardians_" + q;
		if(Group_FindGroup(sGroup) != -1)
		{
			iCharacter = Group_GetGroupCommanderIndex(sGroup);
			Group_SetEnemyToCharacter("Sea_ColonySeaAttacker1", iCharacter);
		}
	}

	Group_SetEnemyToCharacter("Sea_ColonySeaAttacker1", nMainCharacterIndex);
	Group_SetTaskAttack("Sea_ColonySeaAttacker1", PLAYER_GROUP);
	Group_LockTask("Sea_ColonySeaAttacker1");
	
	UpdateRelations();

	PChar.quest.BattleSeaDefenceColonyWinner.win_condition.l1 = "Group_Death";
	PChar.quest.BattleSeaDefenceColonyWinner.win_condition.l1.group = "Sea_ColonySeaAttacker1";
	PChar.quest.BattleSeaDefenceColonyWinner.function = "BattleSeaDefenceColonyWinner";

	PChar.quest.ClearBattleSeaColonyFromMapEnter.win_condition.l1 = "MapEnter";
	PChar.quest.ClearBattleSeaColonyFromMapEnter.function = "ClearBattleSeaColonyFromMapEnter";
}

void ClearBattleSeaColonyFromMapEnter(string qName)
{
	string sClearQuest = PChar.ColonyBuilding.ClearQuest;
	PChar.quest.(sClearQuest).over = "yes";
	PChar.quest.BattleSeaDefenceColonyWinner.over = "yes";

	ClearBattleSeaColony("");
}

void BattleSeaDefenceColonyWinner(string _tmp)
{
	string sClearQuest = PChar.ColonyBuilding.ClearQuest;
	PChar.quest.(sClearQuest).over = "yes";
	PChar.quest.ClearBattleSeaColony.over = "yes";
	PChar.quest.ClearBattleSeaColonyFromMapEnter.over = "yes";

	AddQuestRecord("MY_COLONY", "5");
	AddQuestUserData("MY_COLONY", "sSex", GetSexPhrase("","а")); // LEO: регистрируем sSex ПОСЛЕ записи, иначе @<sSex> в тексте пустой

	Group_DeleteGroup("Sea_ColonySeaAttacker1");
	Island_SetReloadEnableGlobal("Caiman", true);

	ChangeColonyMorale(90, true);
	PChar.ColonyBuilding.Defence = false;
	PChar.ColonyBuilding.Defence.Type = "";

	BattleSeaDefenceColonyClearAttackers();
	CheckDeadColonyGuard();

	// Выключаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = false;
	// Выключаем блокировку всех нападений <--

	PChar.quest.BattleSeaDefenceColonyExitBattleField.win_condition.l1 = "ExitFromLocation";
	PChar.quest.BattleSeaDefenceColonyExitBattleField.win_condition.l1.location = PChar.location;
	PChar.quest.BattleSeaDefenceColonyExitBattleField.function = "BattleSeaDefenceColonyExitBattleField";

	int iNation = sti(PChar.ColonyBuilding.Defence.Nation);
	string sNationName = GetNationNameByType(iNation) + "Gen";
	AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_31", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_32", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_33", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_34", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_35", "AddRumours.txt", "#space#", " ") + XI_ConvertString(sNationName) + GetConvertStrWithReplace("Variable_Colony_36", "AddRumours.txt", "#space#", " "), 5, 1);
}

void BattleSeaDefenceColonyExitBattleField(string qName)
{
	string sShip = "";
	for(int i=1; i<=4; i++)
	{
		sShip = "Ship" + i;
		if(Group_FindGroup("InterfaceColonyGuardians_" + i) != -1 && PChar.Colony.Guardians.(sShip).Active == true)
		{
			Group_SetAddress("InterfaceColonyGuardians_" + i, "Caiman", "quest_ships", "Quest_ship_" + i);
			Group_SetTaskNone("InterfaceColonyGuardians_" + i);
		}
	}
}

void ClearBattleSeaColony(string qName)
{
	Group_DeleteGroup("Sea_ColonySeaAttacker1");
	BattleSeaDefenceColonyClearAttackers();

	Island_SetReloadEnableGlobal("Caiman", true);

	int iEnd = sti(PChar.ColonyBuilding.Defence.End);
	int iSoldiers = sti(PChar.ColonyBuilding.Defence.Soldiers);
	int iSoldiersDead = iSoldiers/2;

	int iNation = sti(PChar.ColonyBuilding.Defence.Nation);
	string sNationName = GetNationNameByType(iNation) + "Abl";

	Log_Info(GetConvertStrWithReplace("Variable_Colony_40", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");
	if(iEnd == 1)
	{
		AddQuestRecord("MY_COLONY", "14");
		sNationName = "people" + GetNationNameByType(iNation) + "Dat";
		AddQuestUserData("MY_COLONY", "sNationName", XI_ConvertString(sNationName));

		ChangeColonySoldiers(-iSoldiersDead, false);
		ChangeColonyMorale(50, true);

		DeleteBestGoodFromMyColony(false);
		SetColonyMessageInfo("Colony", "BattleSea", "Good", iNation, 0, 0, true);

		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_37", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_38", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_39", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_40", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_41", "AddRumours.txt", "#space#", " ") + XI_ConvertString(sNationName) + GetConvertStrWithReplace("Variable_Colony_42", "AddRumours.txt", "#space#", " "), 5, 1);
	}
	else
	{
		AddQuestRecord("MY_COLONY", "9");
		AddQuestUserData("MY_COLONY", "sSex", GetSexPhrase("","а")); // LEO: регистрируем sSex ПОСЛЕ записи, иначе @<sSex> в тексте пустой
		sNationName = GetNationNameByType(iNation) + "Abl";
		AddQuestUserData("MY_COLONY", "sNationName", XI_ConvertString(sNationName));

		ChangeColonyPopulation((-rand(100)-100), true);
		ChangeColonySoldiers(0, true);
		ChangeColonyMorale(0, true);
		ChangeColonyMoney(0, true);
		PChar.ColonyBuilding.Defence.DeadInTown = true;
		SetTimerCondition("ColonyNotSetDead", 0, 0, 6, false);
		PChar.ColonyBuilding.Defence.KillShips = 4;
		string sDestroyBuild = PChar.ColonyBuilding.Defence.DestroyBuild;
		DestroyBuildInCaimanColony(sDestroyBuild);
		DeleteBestGoodFromMyColony(true);
		SetColonyMessageInfo("Colony", "BattleSea", "", iNation, 0, false, true);
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_43", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_44", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_45", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_46", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_47", "AddRumours.txt", "#space#", " ") + XI_ConvertString(sNationName) + GetConvertStrWithReplace("Variable_Colony_48", "AddRumours.txt", "#space#", " "), 5, 1);
	}

	int iKillShips = sti(PChar.ColonyBuilding.Defence.KillShips);
	if (iKillShips > 0)
	{
		for(int ks = 1; ks <= iKillShips; ks++)
		{
			KillColonyGuarding(ks);
		}
	}

	CheckDeadColonyGuard();
	RepairAllGuardShips();

	string sBattleQuest = PChar.ColonyBuilding.BattleQuest;
	PChar.quest.(sBattleQuest).over = "yes";
	PChar.quest.ClearBattleSeaColonyFromMapEnter.over = "yes";

	PChar.ColonyBuilding.Defence = false;
	PChar.ColonyBuilding.Defence.Type = "";

	// Выключаем блокировку всех нападений -->
	PChar.ColonyBuilding.EventBlock = false;
	// Выключаем блокировку всех нападений <--

	PChar.quest.BattleSeaDefenceColonyExitBattleField.win_condition.l1 = "ExitFromLocation";
	PChar.quest.BattleSeaDefenceColonyExitBattleField.win_condition.l1.location = PChar.location;
	PChar.quest.BattleSeaDefenceColonyExitBattleField.function = "BattleSeaDefenceColonyExitBattleField";
}

/////////////////////////////////////////////////////////////////////////////////
// МГНОВЕННОЕ ПОРАЖЕНИЕ
/////////////////////////////////////////////////////////////////////////////////
void InstantColonyDefendLoose(string sTarget, string sType)
{
	Log_Info(GetConvertStrWithReplace("Variable_Colony_41", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + "!");

	int iRebels = 0;
	int iMoney = 0;
	int iCitizensDead = sti(rand(15)+10)*10;

	int iNation = PIRATE;
	string sNationName = "";
	string sDestroyBuild = "Store";

	int iOreMinus, iGoodMinus, iGold, iSilver, iIron, iSlaves;
	switch(sTarget)
	{
		case "Colony":
			switch(sType)
			{
				case "Mutiny":
					AddQuestRecord("MY_COLONY", "26");
					iRebels = sti(PChar.ColonyBuilding.Defence.EnemyQuantity);
					iMoney = sti(PChar.ColonyBuilding.Money);
					ChangeColonyMoney(-(iMoney/iRebels), false);

					AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_49", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_50", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_51", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_52", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_53", "AddRumours.txt", "#space#", " "), 5, 1);
				break;

				case "BattleLand":
					sDestroyBuild = PChar.ColonyBuilding.Defence.DestroyBuild;

					AddQuestRecord("MY_COLONY", "27");
					ChangeColonyMoney(0, true);
					DestroyBuildInCaimanColony(sDestroyBuild);
					DeleteBestGoodFromMyColony(true);

					AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_54", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_55", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_56", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_57", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_58", "AddRumours.txt", "#space#", " "), 5, 1);
				break;

				case "BattleSea":
					iNation = sti(PChar.ColonyBuilding.Defence.Nation);
					sNationName = GetNationNameByType(iNation) + "Gen";
					sDestroyBuild = PChar.ColonyBuilding.Defence.DestroyBuild;

					AddQuestRecord("MY_COLONY", "28");
					AddQuestUserData("MY_COLONY", "sNationName", XI_ConvertString(sNationName));
					ChangeColonyMoney(0, true);
					PChar.ColonyBuilding.Defence.KillShips = 4;
					DestroyBuildInCaimanColony(sDestroyBuild);
					DeleteBestGoodFromMyColony(true);
					Group_DeleteGroup("Sea_ColonySeaAttacker1");
					Island_SetReloadEnableGlobal("Caiman", true);

					AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_59", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_60", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_61", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_62", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_63", "AddRumours.txt", "#space#", " ") + XI_ConvertString(sNationName) + GetConvertStrWithReplace("Variable_Colony_64", "AddRumours.txt", "#space#", " "), 5, 1);
				break;
			}

			ChangeColonyPopulation(-iCitizensDead, false);
			ChangeColonySoldiers(0, true);
			ChangeColonyMorale(0, true);

			PChar.ColonyBuilding.Defence = false;
			PChar.ColonyBuilding.Defence.Type = "";
			PChar.ColonyBuilding.Defence.DeadInTown = true;
			PChar.ColonyBuilding.Defence.Battle = false;
			SetTimerCondition("ColonyNotSetDead", 0, 0, 6, false);
		break;

		case "Mines":
			switch(sType)
			{
				ChangeMinesSoldiers(0, true);
				ChangeMinesSlaves(0, true);

				case "Mutiny":
					iRebels = sti(PChar.Mines.Defence.EnemyQuantity);
					AddQuestRecord("MY_COLONY", "29");

					iOreMinus = iRebels * (3 + rand(2));
					ChangeMinesOre(-iOreMinus, "Gold", false);
					iOreMinus = iRebels * (3 + rand(2));
					ChangeMinesOre(-iOreMinus, "Silver", false);
					iOreMinus = iRebels * (3 + rand(2));
					ChangeMinesOre(-iOreMinus, "Iron", false);

					iGoodMinus = iRebels * 50;
					ChangeMinesMainGoods(-iGoodMinus, "Food", false);
					iGoodMinus = iRebels * 25;
					ChangeMinesMainGoods(-iGoodMinus, "Medicament", false);

					AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_65", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_66", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_67", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_68", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_69", "AddRumours.txt", "#space#", " "), 5, 1);
				break;

				case "BattleLand":
					AddQuestRecord("MY_COLONY", "30");

					iGold = sti(PChar.Mines.Defence.Minus.Gold);
					iSilver = sti(PChar.Mines.Defence.Minus.Silver);
					iIron = sti(PChar.Mines.Defence.Minus.Iron);
					iSlaves = sti(PChar.Mines.Defence.Minus.Slaves);

					ChangeMinesOre(-iGold, "Gold", false);
					ChangeMinesOre(-iSilver, "Silver", false);
					ChangeMinesOre(-iIron, "Iron", false);

					AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_70", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_71", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_72", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_73", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_74", "AddRumours.txt", "#space#", " "), 5, 1);
				break;
			}

			PChar.Mines.Defence = false;
			PChar.Mines.Defence.Type = "";
			PChar.Mines.Defence.DeadInTown = true;
			PChar.Mines.Defence.Battle = false;

			SetTimerCondition("MinesNotSetDead", 0, 0, 6, false);
			SetTimerCondition("MinesUptade5", 0, 0, 10, false);
		break;

		case "Plantation":
			switch(sType)
			{
				case "Mutiny":
					iRebels = sti(PChar.Plantation.Defence.EnemyQuantity);

					AddQuestRecord("MY_COLONY", "31");
					ChangePlantationSlaves(0, true);
					ChangePlantationSoldiers(0, true);

					iOreMinus = iRebels * (3 + rand(2));
					ChangePlantationExtract(-iOreMinus, "Sugar", false);
					iOreMinus = iRebels * (3 + rand(2));
					ChangePlantationExtract(-iOreMinus, "Coffee", false);

					iGoodMinus = iRebels * 50;
					ChangePlantationGoods(-iOreMinus, "Food", false);
					iGoodMinus = iRebels * 25;
					ChangePlantationGoods(-iOreMinus, "Medicament", false);

					AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Colony_75", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_76", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Colony_77", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Colony_78", "AddRumours.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_79", "AddRumours.txt", "#space#", " "), 5, 1);
				break;

				PChar.Plantation.Defence = false;
				PChar.Plantation.Defence.Type = "";
				PChar.Plantation.Defence.DeadInTown = true;
				PChar.Plantation.Defence.Battle = false;

				SetTimerCondition("PlantationNotSetDead", 0, 0, 6, false);
				SetTimerCondition("PlantationUptade5", 0, 0, 10, false);
			}
		break;
	}

	PChar.ColonyBuilding.EventBlock = false;

	sType += "Loose";
	SetColonyMessageInfo(sTarget, sType, "", iNation, 0, 0, 1);
}

//-------------------------------------------------------------------------------
// OTHER FUNCTIONS
//-------------------------------------------------------------------------------
void SetBestGoodForDeleteFromMyColony()
{
	string sGoodName = "";
	string sGood = "";

	ref rStoreHouse = &Stores[STORE_HOUSE];

	int iDeleteGoods = 0;
	int iQuantity = 0;
	int iRand = 0;
	for(int i=0; i < GOODS_QUANTITY; i++)
	{
		if(sti(Goods[i].Cost) >= 100)
		{
			sGoodName = Goods[i].Name;

			if(sti(rStoreHouse.Goods.(sGoodName).Quantity) > 0)
			{
				iRand = rand(5) + 3;
				iQuantity = sti(rStoreHouse.Goods.(sGoodName).Quantity) / iRand;

				if(iQuantity > 0)
				{
					iDeleteGoods++;
					sGood = "Good_" + iDeleteGoods;

					PChar.ColonyBuilding.DeleteGoods.(sGood) = sGoodName;
					PChar.ColonyBuilding.DeleteGoods.(sGood).Qty = iQuantity;
				}
			}
		}
	}

	PChar.ColonyBuilding.DeleteGoods.DeleteQty = iDeleteGoods;
}

void DeleteBestGoodFromMyColony(bool bDelete)
{
	if(!bDelete)
	{
		return;
	}

	int iDeleteGoods = 0;
	if(CheckAttribute(PChar, "ColonyBuilding.DeleteGoods.DeleteQty"))
	{
		iDeleteGoods = sti(PChar.ColonyBuilding.DeleteGoods.DeleteQty);
	}

	if(iDeleteGoods <= 0)
	{
		return;
	}

	string sGood, sGoodName;
	int iQty = 0;

	Log_InfoS(GetConvertStrWithReplace("Variable_Colony_42", "Logs.txt", "#space#", " "));

	int iGood = 0;
	int iStoreQty = 0;
	for(int i=1; i<=iDeleteGoods; i++)
	{
		sGood = "Good_" + i;
		sGoodName = PChar.ColonyBuilding.DeleteGoods.(sGood);
		iQty = sti(PChar.ColonyBuilding.DeleteGoods.(sGood).Qty);

		iGood = FindGood(sGoodName);
		iStoreQty = Stores[STORE_HOUSE].Goods.(sGoodName).Quantity;

		SetStoreGoodsEdition(&Stores[STORE_HOUSE], iGood, iStoreQty - iQty, iQty, true);

		Log_Info(XI_ConvertString(sGoodName) + GetConvertStrWithReplace("Variable_Colony_43", "Logs.txt", "#space#", " ") + iQty + GetConvertStrWithReplace("Variable_Colony_44", "Logs.txt", "#space#", " "));
	}

	if(CheckAttribute(PChar, "ColonyBuilding.DeleteGoods"))
	{
		DeleteAttribute(PChar, "ColonyBuilding.DeleteGoods");
	}

	PChar.ColonyBuilding.DeleteGoods = "";
}

void SetDeadmansToLocation(ref loc, string sType)
{
	if(actLoadFlag) return;
	if(loc.id != "Caiman_town" && loc.id != "Caiman_Plantation" && loc.id != "Caiman_mines") return;

	int iPopulation = 100;
	int iMan = iPopulation / 10;
	int iWoman = iPopulation / 20;
    int iMassive;

	if(iMan > 15) iMan = 15;
	if(iMan < 0) iMan = 0;

	if(iWoman > 10) iWoman = 10;
	if(iWoman < 0) iWoman = 0;

	if(sti(PChar.ColonyBuilding.Stage) <= 1)
	{
		if(iMan > 8) iMan = 8;
		if(iWoman > 7) iWoman = 7;
	}

	int i=1;
	int b=1;
	int iSex = 1;

	ref dead;
	string sModel, sAni;

	iSex = 1;

	for(i=1; i <= iMan; i++)
	{
		dead = GetCharacter(NPC_GeneratePhantomCharacter("citiz_1", PIRATE, iSex, 0));

		sModel = "citiz_" + (rand(11)+1);
		sAni = "man";

		if(sType == "pirate")
		{

			iMassive = rand(22);

		    sModel = GetSlaveModel(iMassive);
		    sAni = "man";
		}


		dead.model = sModel;
		dead.model.animation = sAni;
		dead.NotRemoved = true;
		SetNewModelToChar(dead);

		LAi_SetImmortal(dead, true);
		LAi_SetActorType(dead);
		LAi_ActorSetLayMode(dead);
		PlaceCharacter(dead, "goto", "random_free");
		RemoveCharacterEquip(dead, BLADE_ITEM_TYPE);
		RemoveCharacterEquip(dead, GUN_ITEM_TYPE);

		for(b=0; b <= 4; b++)
		{
			LaunchBlood(dead, 7.0, true);
		}
	}

	if(sType == "pirate")
	{
		return;
	}

	iSex = 0;
	for(i=1; i <= iWoman; i++)
	{
		dead = GetCharacter(NPC_GeneratePhantomCharacter("girl_1", PIRATE, iSex, 0));

		sModel = "girl_" + (rand(10)+1);
		sAni = "towngirl";

		dead.model = sModel;
		dead.model.animation = sAni;
		dead.NotRemoved = true;
		SetNewModelToChar(dead);

		LAi_SetImmortal(dead, true);
		LAi_SetActorType(dead);
		LAi_ActorSetLayMode(dead);
		PlaceCharacter(dead, "goto", "random_free");
		RemoveCharacterEquip(dead, BLADE_ITEM_TYPE);
		RemoveCharacterEquip(dead, GUN_ITEM_TYPE);

		for(b=0; b <= 4; b++)
		{
			LaunchBlood(dead, 7.0, true);
		}
	}
}

int FightInColonyGetMusketeerQty(int iQty)
{
	if(iQty < 5)
	{
		return 0;
	}

	if(iQty >= 20)
	{
		return iQty / 4;
	}
	else
	{
		return iQty / 3;
	}

	return 0;
}

int GetColonyMinPeople()
{
	int iQty = 0;

	if(PChar.ColonyBuilding.Store == true && PChar.ColonyBuilding.Store.BuildingTime == false) 		{ iQty++; }
	if(PChar.ColonyBuilding.Shipyard == true && PChar.ColonyBuilding.Shipyard.BuildingTime == false) 	{ iQty++; }
	if(PChar.ColonyBuilding.Tavern == true && PChar.ColonyBuilding.Tavern.BuildingTime == false) 		{ iQty += 2; }
	if(PChar.ColonyBuilding.Church == true && PChar.ColonyBuilding.Church.BuildingTime == false) 		{ iQty++; }
	if(PChar.ColonyBuilding.Bank == true && PChar.ColonyBuilding.Bank.BuildingTime == false) 		{ iQty++; }
	if(PChar.ColonyBuilding.HeadPort == true && PChar.ColonyBuilding.HeadPort.BuildingTime == false) 	{ iQty++; }
	if(PChar.ColonyBuilding.StoreHouse == true && PChar.ColonyBuilding.StoreHouse.BuildingTime == false) 	{ iQty++; }

	return iQty;
}

void ChangeColonyPopulation(int iQty, bool bSet)
{
	int iMax = sti(PChar.ColonyBuilding.Population.Max);

	if(bSet)
	{
		PChar.ColonyBuilding.Population = iQty;
	}
	else
	{
		PChar.ColonyBuilding.Population = sti(PChar.ColonyBuilding.Population) + iQty;
	}

	if(sti(PChar.ColonyBuilding.Population) < 0) PChar.ColonyBuilding.Population = 0;
	if(sti(PChar.ColonyBuilding.Population) > iMax) PChar.ColonyBuilding.Population = iMax;
}

void ChangeColonyMorale(int iQty, bool bSet)
{
	if(bSet)
	{
		PChar.ColonyBuilding.Population.Morale = iQty;
	}
	else
	{
		PChar.ColonyBuilding.Population.Morale = sti(PChar.ColonyBuilding.Population.Morale) + iQty;
	}

	if(sti(PChar.ColonyBuilding.Population.Morale) < 0) PChar.ColonyBuilding.Population.Morale = 0;
	if(sti(PChar.ColonyBuilding.Population.Morale) > 100) PChar.ColonyBuilding.Population.Morale = 100;
}

void ChangeColonySoldiers(int iQty, bool bSet)
{
	int iMax = sti(PChar.ColonyBuilding.Soldiers.MaxQuantity);

	if(bSet)
	{
		PChar.ColonyBuilding.Soldiers = iQty;
	}
	else
	{
		PChar.ColonyBuilding.Soldiers = sti(PChar.ColonyBuilding.Soldiers) + iQty;
	}

	if(sti(PChar.ColonyBuilding.Soldiers) < 0) PChar.ColonyBuilding.Soldiers = 0;
	if(sti(PChar.ColonyBuilding.Soldiers) > iMax) PChar.ColonyBuilding.Soldiers = iMax;
}

void ChangeColonyGoods(int iQty, string sGood)
{
	int iMax = sti(PChar.ColonyBuilding.(sGood).MaxQuantity);

	PChar.ColonyBuilding.(sGood) = sti(PChar.ColonyBuilding.(sGood)) + iQty;

	if(sti(PChar.ColonyBuilding.(sGood)) < 0) PChar.ColonyBuilding.(sGood) = 0;
	if(sti(PChar.ColonyBuilding.(sGood)) > iMax) PChar.ColonyBuilding.(sGood) = iMax;
}

void ChangeColonyMoney(int iQty, bool bSet)
{
	if(bSet)
	{
		PChar.ColonyBuilding.Money = iQty;
	}
	else
	{
		PChar.ColonyBuilding.Money = sti(PChar.ColonyBuilding.Money) + iQty;
	}

	if(sti(PChar.ColonyBuilding.Money) < 0) PChar.ColonyBuilding.Money = 0;
}

int GetMyColonyMoney()
{
	if(!CheckAttribute(PChar, "ColonyBuilding.Money"))
	{
		return 0;
	}

	return sti(PChar.ColonyBuilding.Money);
}

void SetColonyMessageInfo(string sTarget, string sType, string sEnd, int iNation, int iTime, bool bFirst, bool bInterface)
{
	PChar.ColonyBuilding.MessageInfo.Target = sTarget;
	PChar.ColonyBuilding.MessageInfo.Type = sType;
	PChar.ColonyBuilding.MessageInfo.End = sEnd;
	PChar.ColonyBuilding.MessageInfo.Nation = iNation;
	PChar.ColonyBuilding.MessageInfo.Time = iTime;
	PChar.ColonyBuilding.MessageInfo.First = bFirst;

	if(!dialogRun && !bQuestCheckProcessFreeze && !bAbordageStarted)
	{
		if(bInterface)
		{
			DoQuestFunctionDelay("ColonyMessageStartInfo", 2.5);
		}
	}
}

void ColonyMessageStartInfo(string _tmp)
{
	LaunchAttackMessage();
}

// Разрушить/закрыть здание
void DestroyBuildInCaimanColony(string sBuild)
{
	if(sBuild == "none")
	{
		return; // не сломали ничего
	}

	if(PChar.ColonyBuilding.(sBuild) == false)
	{
		return; // такого здания ещё не построили
	}

	PChar.ColonyBuilding.(sBuild) = false;
	PChar.ColonyBuilding.(sBuild).BuildingTime = false;

	int iCost = 0;

	switch(sBuild)
	{
		case "Store":
			Locations[FindLocation("Caiman_town")].reload.l6.disable = true;
			iCost = 15000 + rand(2500);
		break;

		case "Shipyard":
			Locations[FindLocation("Caiman_town")].reload.l5.disable = true;
			iCost = 17500 + rand(2850);
		break;

		case "Tavern":
			Locations[FindLocation("Caiman_town")].reload.l4.disable = true;
			iCost = 15000 + rand(500);
		break;

		case "Church":
			Locations[FindLocation("Caiman_town")].reload.l7.disable = true;
			iCost = 20000 + rand(2500);
		break;

		case "Bank":
			Locations[FindLocation("Caiman_town")].reload.l8.disable = true;
			iCost = 20000 + rand(2500);
		break;

		case "HeadPort":
			Locations[FindLocation("Caiman_town")].reload.l10.disable = true;
			iCost = 12500 + rand(2550);
		break;
	}

	PChar.ColonyBuilding.(sBuild).Cost = iCost;

	AddQuestRecord("MY_COLONY", "15");
	AddQuestUserData("MY_COLONY", "sDestroyBuild", XI_ConvertString("Destroy" + sBuild));

	if(sBuild == "Store")
	{
		AddQuestRecord("MY_COLONY", "32");
	}
}

void RenamedColonyCaiman()
{
	worldMap.labels.Shore17.text = "";

	string sColonyName = PChar.ColonyBuilding.ColonyName;
	worldMap.labels.Shore17.text = sColonyName;
	worldMap.labels.Shore17.icon = PIRATE;
}

string GetColonyBuildDestroy()
{
	switch(rand(5))
	{
		case 0: return "Store"; break;
		case 1: return "Shipyard"; break;
		case 2: return "Tavern"; break;
		case 3: return "Church"; break;
		case 4: return "Bank"; break;
		case 5: return "HeadPort"; break;
	}

	return "Store";
}

int GetShipTypeForColonyBattleSea(ref Hunter)
{
    int Squadron, hcrew;

    int j, q;
    object rShip;
    bool bOk = false;
    int iNation = sti(Hunter.nation);
    aref aNation;
    string sAttr;

    while(!bOk)
    {
        Squadron = 100 + rand(24); // LEO: Чуть-чуть 2 класс и остальное 1 класс

        rShip = GetShipByType(Squadron);
        makearef(aNation, rShip.nation);
        q = GetAttributesNum(aNation);
        for(j = 0; j < q; j++)
        {
            sAttr = GetAttributeName(GetAttributeN(aNation, j));
            if(GetNationTypeByName(sAttr) == iNation && rShip.nation.(sAttr) == true) bOk = true;
        }
    }
    return Squadron;
}

/////////////////////////////////////////////////////////////////////////////////
// АВТОЗАКУПКА ТОВАРА НА КОЛОНИЮ, РУДНИКИ И ПЛАНТАЦИЮ
/////////////////////////////////////////////////////////////////////////////////
int GetSumAutoPurchaseColonyGoods(string sType, bool bCurrent)
{
	int iCost = 0;
	int iNeedFood = 0;
	int iNeedMedicament = 0;
	int iNeedTools = 0;

	if(bCurrent)
	{
		switch(sType)
		{
			case "Colony":
				iNeedFood = sti(PChar.ColonyBuilding.Food.MaxQuantity) - sti(PChar.ColonyBuilding.Food);
				iNeedMedicament = sti(PChar.ColonyBuilding.Medicament.MaxQuantity) - sti(PChar.ColonyBuilding.Medicament);
			break;

			case "Mines":
				iNeedFood = sti(PChar.Mines.FoodQuantity.Max) - sti(PChar.Mines.FoodQuantity);
				iNeedMedicament = sti(PChar.Mines.MedicamentQuantity.Max) - sti(PChar.Mines.MedicamentQuantity);
			break;

			case "Plantation":
				iNeedFood = sti(PChar.Plantation.Food.MaxQuantity) - sti(PChar.Plantation.Food.Quantity);
				iNeedMedicament = sti(PChar.Plantation.Medicament.MaxQuantity) - sti(PChar.Plantation.Medicament.Quantity);
				iNeedTools = sti(PChar.Plantation.Items.MaxQuantity) - sti(PChar.Plantation.Items.Quantity);
			break;
		}
	}
	else
	{
		switch(sType)
		{
			case "Colony":
				iNeedFood = sti(PChar.ColonyBuilding.Food.MaxQuantity);
				iNeedMedicament = sti(PChar.ColonyBuilding.Medicament.MaxQuantity);
			break;

			case "Mines":
				iNeedFood = sti(PChar.Mines.FoodQuantity.Max);
				iNeedMedicament = sti(PChar.Mines.MedicamentQuantity.Max);
			break;

			case "Plantation":
				iNeedFood = sti(PChar.Plantation.Food.MaxQuantity);
				iNeedMedicament = sti(PChar.Plantation.Medicament.MaxQuantity);
				iNeedTools = sti(PChar.Plantation.Items.MaxQuantity);
			break;
		}
	}

	iNeedFood *= COLONY_AUTO_PURCHASE_FOOD;
	iNeedMedicament *= COLONY_AUTO_PURCHASE_MEDICAMENT;
	iNeedTools *= COLONY_AUTO_PURCHASE_TOOLS;

	iCost = iNeedFood + iNeedMedicament + iNeedTools;

	return iCost;
}

void AutoPurchaseColonyGoods(string sType)
{
	bool bCan = PChar.ColonyBuilding.(sType).AutoPurchase;
	if(!bCan)
	{
		return;
	}

	if(PChar.ColonyBuilding.Store == false)
	{
		return;
	}

	int iCost = GetSumAutoPurchaseColonyGoods(sType, true);
	if(iCost <= 0)
	{
		return;
	}

	int iColonyMoney = sti(PChar.ColonyBuilding.Money);

	string sTypeText = "колонии";
	switch(sType)
	{
		case "Colony": sTypeText = "вашей колонии '" + PChar.ColonyBuilding.ColonyName + "'"; break;
		case "Mines": sTypeText = "ваших рудников"; break;
		case "Plantation": sTypeText = "вашей плантации"; break;
	}

	if(iColonyMoney < iCost)
	{
		Log_InfoS(GetConvertStrWithReplace("Variable_Colony_45", "Logs.txt", "#space#", " ") + sTypeText + GetConvertStrWithReplace("Variable_Colony_46", "Logs.txt", "#space#", " "));
		return;
	}
	else
	{
		ChangeColonyMoney(-iCost, false);
	}

	int iFoodMax = 0;
	int iMedicamentMax = 0;
	int iToolsMax = 0;

	switch(sType)
	{
		case "Colony":
			iFoodMax = sti(PChar.ColonyBuilding.Food.MaxQuantity);
			iMedicamentMax = sti(PChar.ColonyBuilding.Medicament.MaxQuantity);

			ChangeColonyGoods(iFoodMax, "Food");
			ChangeColonyGoods(iMedicamentMax, "Medicament");

			Log_InfoS(GetConvertStrWithReplace("Variable_Colony_47", "Logs.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Colony_48", "Logs.txt", "#space#", " "));
			Log_Info(GetConvertStrWithReplace("Variable_Colony_49", "Logs.txt", "#space#", " ") + iCost + GetConvertStrWithReplace("Variable_Colony_50", "Logs.txt", "#space#", " "));
		break;

		case "Mines":
			iFoodMax = sti(PChar.Mines.FoodQuantity.Max);
			iMedicamentMax = sti(PChar.Mines.MedicamentQuantity.Max);

			ChangeMinesMainGoods(iFoodMax, "Food", true);
			ChangeMinesMainGoods(iMedicamentMax, "Medicament", true);

			Log_InfoS(GetConvertStrWithReplace("Variable_Colony_51", "Logs.txt", "#space#", " "));
			Log_Info(GetConvertStrWithReplace("Variable_Colony_52", "Logs.txt", "#space#", " ") + iCost + GetConvertStrWithReplace("Variable_Colony_53", "Logs.txt", "#space#", " "));
		break;

		case "Plantation":
			iFoodMax = sti(PChar.Plantation.Food.MaxQuantity);
			iMedicamentMax = sti(PChar.Plantation.Medicament.MaxQuantity);
			iToolsMax = sti(PChar.Plantation.Items.MaxQuantity);

			ChangePlantationGoods(iFoodMax, "Food", true);
			ChangePlantationGoods(iMedicamentMax, "Medicament", true);
			ChangePlantationGoods(iToolsMax, "Items", true);

			Log_InfoS(GetConvertStrWithReplace("Variable_Colony_54", "Logs.txt", "#space#", " "));
			Log_Info(GetConvertStrWithReplace("Variable_Colony_55", "Logs.txt", "#space#", " ") + iCost + GetConvertStrWithReplace("Variable_Colony_56", "Logs.txt", "#space#", " "));
		break;
	}
}

//-------------------------------------------------------------------------------
// РУДНИКИ
//-------------------------------------------------------------------------------
void MinesBuilding(string sMine)
{
	ref PChar = GetMainCharacter();

	sMine = sMine + "Mine";

	PChar.Mines.(sMine) = true;

	PChar.Mines.GoldQuantity = 0;
	PChar.Mines.SilverQuantity = 0;
	PChar.Mines.IronQuantity = 0;
	PChar.Mines.GuardiansQuantity = 0;
	PChar.Mines.HireGuardiansQuantity = 0;
	PChar.Mines.GuardiansCost = 200;
	PChar.Mines.MaxGuardiansQuantity = 100;
	PChar.Mines.SlavesQuantity = 0;
	PChar.Mines.MaxSlavesQuantity = 100;
	PChar.Mines.FoodQuantity = 0;
	PChar.Mines.Temp.GoldQuantity = 0;
	PChar.Mines.Temp.SilverQuantity = 0;
	PChar.Mines.Temp.IronQuantity = 0;
	PChar.Mines.Temp.GuardiansQuantity = 0;
	PChar.Mines.Temp.SlavesQuantity = 0;
	PChar.Mines.Temp.FoodQuantity = 0;
}

//-------------------------------------------------------------------------------
// STOREHOUSE
//-------------------------------------------------------------------------------
void StoreHouseInit()
{
	ref PChar = GetMainCharacter();

	PChar.ColonyBuilding.StoreHouse.Size = "Small";

	int iRand = 1000 * rand(15);

	Stores[STORE_HOUSE].max_weight = 60000 + iRand;

	iRand = 1000 * rand(15);
	Stores[STORE_HOUSE].max_resize_weight = sti(Stores[STORE_HOUSE].max_weight) + 35000 + iRand;

	string sGood = "";

	for(int g=0; g < GOODS_QUANTITY; g++)
	{
		sGood = Goods[g].Name;
		Stores[STORE_HOUSE].goods.(sGood).quantity = 0;
	}
}

//-------------------------------------------------------------------------------
// Заселяем все постройки
//-------------------------------------------------------------------------------
// Gregg: постройка колонии Кайман вынесена в отдельный файл (сплит god-файла Colony.c).
#include "Addons\Colony_CaimanBuild.c"

void ResetLightsInLocation(ref rTown)
{
	//Day dynamic light
	rTown.models.day.lights.candles = "candle";
	rTown.models.day.lights.candles_medium = "candelabrum";
	rTown.models.day.lights.chandeliers = "chandelier";
	rTown.models.day.lights.heaters = "heater";
	rTown.models.day.lights.torchlightes = "torchlight";
	rTown.models.day.lights.outside = "outside_day";
	rTown.models.day.lights.incas_light = "incas";
	rTown.models.day.lights.incas_sky = "incasskyday";
	rTown.models.day.lights.lamp = "lamp";
	rTown.models.day.lights.blueteleport = "blueteleport";
	rTown.models.day.lights.redteleport = "redteleport";
	rTown.models.day.lights.greenteleport = "greenteleport";
	rTown.models.day.lights.magsteleport = "magsteleport";

	//Night dynamic light
	rTown.models.night.lights.candles = "candle";
	rTown.models.night.lights.candles_medium = "candelabrum";
	rTown.models.night.lights.chandeliers = "chandelier";
	rTown.models.night.lights.heaters = "heater";
	rTown.models.night.lights.torchlightes = "torchlight";
	rTown.models.night.lights.torchlightes_o = "torchlight";
	rTown.models.night.lights.fonar = "lamp";
	rTown.models.night.lights.outside = "outside_night";
	rTown.models.night.lights.incas_light = "incas";
	rTown.models.night.lights.incas_sky = "incasskynight";
	rTown.models.night.lights.lamp = "lamp";
	rTown.models.night.lights.blueteleport = "blueteleport";
	rTown.models.night.lights.redteleport = "redteleport";
	rTown.models.night.lights.greenteleport = "greenteleport";
	rTown.models.night.lights.magsteleport = "magsteleport";
}

/////////////////////////////////////////////////////////////////////////////
// AUTO SELL
/////////////////////////////////////////////////////////////////////////////
int GetCurrentGoodBaseCostForAutoSell(int iGood)
{
	ref rStore = &Stores[Caiman_STORE];

	int iStoreCost = 0;
	iStoreCost = GetStoreGoodsPrice(rStore, iGood, PRICE_TYPE_SELL, PChar, 1) / 1.3;

	return iStoreCost;
}

string GetColonyAutoSellWork()
{
	bool bMines = PChar.ColonyBuilding.Mines.AutoSell;
	bool bPlantation = PChar.ColonyBuilding.Plantation.AutoSell;

	if(!bMines && !bPlantation)
	{
		return XI_ConvertString("ColonyGoodsAutoSell_1");
	}
	if(bMines && !bPlantation)
	{
		return XI_ConvertString("ColonyGoodsAutoSell_2");
	}
	if(!bMines && bPlantation)
	{
		return XI_ConvertString("ColonyGoodsAutoSell_3");
	}
	if(bMines && bPlantation)
	{
		return XI_ConvertString("ColonyGoodsAutoSell_4");
	}

	return "bug";
}

int GetColonyAutoSellCurrentQty(string sType)
{
	if(CheckAttribute(PChar, "ColonyBuilding." + sType + ".AutoSell.Qty"))
	{
		return sti(PChar.ColonyBuilding.(sType).AutoSell.Qty);
	}

	return 0;
}
