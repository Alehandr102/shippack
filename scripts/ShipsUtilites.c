
#define REAL_SHIPS_QUANTITY 900

object	RealShips[REAL_SHIPS_QUANTITY];
// LEO: неверный индекс возвращает отдельную заглушку, а не элемент за границей ShipsTypes.
object	RealShipDummy;

// LEO: КЭШ подбора типа корабля по ключу (classMin,classMax,type,nation) для GetShipTypeExt — каталог ShipsTypes
//   статичен в сессии, а ключи сильно повторяются между генерациями энкаунтеров. Хит = O(1) вместо скана 143 типов
//   (главный источник фриза генерации). Сбрасывается в InitShips при (пере)построении каталога. Не сейвится.
object	g_ShipTypeCache;

// LEO: роллинг-хинт «следующий вероятно свободный слот» для FindFirstEmptyShip — чтобы не сканировать
//   RealShips[900] от нуля на КАЖДЫЙ создаваемый корабль (пакетный спавн свиты ПГГ = скан ×N). Сбрасывается
//   в ResetRealShipArray. Скан с оборотом всё равно найдёт любой свободный слот, если хинт «промахнулся».
int	g_shipHint = 0;
// LEO: диагностика переполнения RealShips пишется один раз, пока сборка мусора снова не начнёт освобождать слоты
int	g_shipDumpDone = 0;
#define SAILSCOLOR_PRICE_RATE    0.1
#define SAILSGERALD_PRICE_RATE   0.2

#define SAIL_COST_PERCENT 10
#define HULL_COST_PERCENT 20

//<----------------------------------------------------------------------------- Shnaks обнулил влияние
#define SHIP_STAT_RANGE_SPECIAL 0.2						//оценочный
#define SHIP_STAT_RANGE_MAX 0.0						    //
#define SHIP_STAT_RANGE_REQUEST 0.0				       //заказные корабли на верфи
#define SHIP_STAT_RANGE_DRAFT 0.0					  //топовые чертежные корабли на верфи
#define SHIP_STAT_RANGE_GENERATION 0.0			     //случайные корабли в море
#define SHIP_STAT_RANGE_GENERATION_SHIPYARD 0.00	//корабли на верфи
//<----------------------------------------------------------------------------- Shnaks обнулил влияние

#define SHIP_STAT_RANGE_GENERATION_BONUS0 0.00   // <------------------------------------Бонусы для атрибутов от 1 до 20%
#define SHIP_STAT_RANGE_GENERATION_BONUS01 0.01  // 
#define SHIP_STAT_RANGE_GENERATION_BONUS02 0.02  //
#define SHIP_STAT_RANGE_GENERATION_BONUS03 0.03  //
#define SHIP_STAT_RANGE_GENERATION_BONUS04 0.04  //
#define SHIP_STAT_RANGE_GENERATION_BONUS05 0.05  //
#define SHIP_STAT_RANGE_GENERATION_BONUS06 0.06  //
#define SHIP_STAT_RANGE_GENERATION_BONUS07 0.07  //
#define SHIP_STAT_RANGE_GENERATION_BONUS08 0.08  //
#define SHIP_STAT_RANGE_GENERATION_BONUS09 0.09  //
#define SHIP_STAT_RANGE_GENERATION_BONUS10 0.10  //
#define SHIP_STAT_RANGE_GENERATION_BONUS11 0.11  //
#define SHIP_STAT_RANGE_GENERATION_BONUS12 0.12  //
#define SHIP_STAT_RANGE_GENERATION_BONUS13 0.13  //
#define SHIP_STAT_RANGE_GENERATION_BONUS14 0.14  //
#define SHIP_STAT_RANGE_GENERATION_BONUS15 0.15  //
#define SHIP_STAT_RANGE_GENERATION_BONUS16 0.16  //
#define SHIP_STAT_RANGE_GENERATION_BONUS17 0.17  //
#define SHIP_STAT_RANGE_GENERATION_BONUS18 0.18  //
#define SHIP_STAT_RANGE_GENERATION_BONUS19 0.19  //
#define SHIP_STAT_RANGE_GENERATION_BONUS20 0.20  // <------------------------------------

#define SHIP_STAT_UPGRADE_BERMUDE 0.03				//процент апгрейда бермудками // Shnaks (в рамках нового метода генерации статов рекомендую 3-5%)
#define SHIP_STAT_UPGRADE_FLYING_DUTCHMAN 0.05
#define SHIP_HP_PENALTY_THRESH 65.0
#define SHIP_SP_PENALTY_THRESH 85.0

ref GetRealShip(int iType)
{
	if(iType >= REAL_SHIPS_QUANTITY || iType < 0) // LEO: -1 читал память до массива (краш загрузки порта 25.09.2026)
	{
		trace ("--- Wrong Ship Index. iType is " + iType);
		Log_TestInfo("--- Wrong Ship Index. iType is " + iType);
		return &RealShipDummy; // для отлова
	}
	return &RealShips[iType];
}

float GenerateShipStatF(float value, float fRandRate)
{
	float randRange = makefloat(value * fRandRate);
	return makefloat(value + randRange); 
}

int GenerateShipStatI(int value, float fRandRate)
{
	float randRange = makefloat(value * fRandRate);
	return round(value + randRange); 
}

bool HasDefaultShipMinCrew(ref rShip)
{
	// LEO: правило 20% относится к инитной норме (OptCrew), а не к MaxCrew с перегрузом.
	if (CheckAttribute(rShip, "QuestShip") || CheckAttribute(rShip, "isFort")) return false;
	if (!CheckAttribute(rShip, "BaseType") || !CheckAttribute(rShip, "MinCrew")) return false;
	int iBase = sti(rShip.BaseType);
	if (iBase < 0 || iBase >= GetArraySize(&ShipsTypes)) return false;
	ref rBaseShip = GetShipByType(iBase);
	if (CheckAttribute(rBaseShip, "QuestShip") || CheckAttribute(rBaseShip, "isFort")) return false;
	if (!CheckAttribute(rBaseShip, "OptCrew") || !CheckAttribute(rBaseShip, "MinCrew")) return false;
	if (sti(rBaseShip.OptCrew) <= 0) return false;
	if (stf(rBaseShip.MinCrew) != makeint(sti(rBaseShip.OptCrew) * 0.2 + 0.5)) return false;
	return stf(rShip.MinCrew) == stf(rBaseShip.MinCrew);
}

int GenerateShipMinCrew(ref rShip, float fStatRange)
{
	if (!HasDefaultShipMinCrew(rShip)) return GenerateShipStatI(sti(rShip.MinCrew), fStatRange);
	// LEO: OptCrew уже получил бонусы и разброс; отдельный бросок MinCrew нарушал их соотношение.
	rShip.CrewMinRatioVersion = 1;
	return makeint(sti(rShip.OptCrew) * 0.2 + 0.5);
}

void RepairDefaultShipMinCrew()
{
	// LEO: только узнаваемая старая генерация; квестовые ТТХ и бонусы минимума не выравниваем.
	if (SHIP_STAT_RANGE_GENERATION != 0.0 || SHIP_STAT_RANGE_GENERATION_SHIPYARD != 0.0) return;
	int iShips = GetArraySize(&RealShips);
	for (int i = 0; i < iShips; i++)
	{
		ref rShip = &RealShips[i];
		if (CheckAttribute(rShip, "CrewMinRatioVersion")) continue;
		if (!HasDefaultShipMinCrew(rShip) || !CheckAttribute(rShip, "OptCrew")) continue;
		if (CheckAttribute(rShip, "Untuned.MinCrew") || CheckAttribute(rShip, "Untuned.OptCrew")) continue;
		if (CheckAttribute(rShip, "Tuning.MinCrew") || CheckAttribute(rShip, "Tuning.OptCrew")) continue;
		if (CheckAttribute(rShip, "Upgrade.SUP_BRANDER")) continue;
		if (!CheckAttribute(rShip, "savesBonus") || !CheckAttribute(rShip, "savemBonus")) continue;
		int iSpawnBonus = sti(rShip.savesBonus);
		int iMaterialBonus = sti(rShip.savemBonus);
		if (iSpawnBonus < 0 || iSpawnBonus > 12 || iMaterialBonus < 0 || iMaterialBonus > 7) continue;
		ref rBaseShip = GetShipByType(sti(rShip.BaseType));
		int iOptCrew = GenerateShipStatI(sti(rBaseShip.OptCrew), SelectShipsBonuses(rBaseShip, "bonusOptCrew", iSpawnBonus, iMaterialBonus));
		// LEO: несовпадение с сохранёнными бонусами означает неизвестное изменение нормы - пропускаем.
		if (stf(rShip.OptCrew) != iOptCrew) continue;
		rShip.MinCrew = GenerateShipMinCrew(rShip, 0.0);
	}
}

// isStolen это признак ворованности (те цены на верфи)
// если 1 - ворованный, цена копеешная, 0 - честно купленный - можно перепродать.
int GenerateShip(int iBaseType, bool isStolen)
{
	int iShip = CreateBaseShip(iBaseType);

	// LEO: CreateBaseShip при нехватке слотов отдаёт SHIP_NOTUSED, а не -1 - дальше писали характеристики
	//   в запасную запись ShipsTypes через GetRealShip(1000) и портили её
  	if (iShip == -1 || iShip == SHIP_NOTUSED) return SHIP_NOTUSED;

	ref rRealShip = GetRealShip(iShip);

	SetShipsBonuses(rRealShip);

    if(CheckAttribute(rRealShip, "hullNums")) 
	{
        int nHulls = sti(rRealShip.hullNums);
        nHulls -= 1;
        if(nHulls < 0) nHulls = 0;
        rRealShip.ship.upgrades.hull = 1 + rand(nHulls);
    }
    else {rRealShip.ship.upgrades.hull = 1 + rand(2);}
	rRealShip.ship.upgrades.sails = 1 + rand(2);  // только визуальная разница

	rRealShip.MastMultiplier = stf(stf(rRealShip.MastMultiplier)-0.3+makefloat(0.02*rand(30)));

	if (!CheckAttribute(rRealShip, "isFort"))
	{
	    rRealShip.SpeedRate	       = GenerateShipStatF(stf(rRealShip.SpeedRate), SHIP_STAT_RANGE_GENERATION);
	    rRealShip.TurnRate         = GenerateShipStatF(stf(rRealShip.TurnRate), SHIP_STAT_RANGE_GENERATION);
	    rRealShip.HP               = GenerateShipStatI(sti(rRealShip.HP), SHIP_STAT_RANGE_GENERATION);
	    rRealShip.WindAgainstSpeed = GenerateShipStatF(stf(rRealShip.WindAgainstSpeed), SHIP_STAT_RANGE_GENERATION);
	}
    rRealShip.Capacity        = GenerateShipStatI(sti(rRealShip.Capacity), SHIP_STAT_RANGE_GENERATION);
    rRealShip.OptCrew         = GenerateShipStatI(sti(rRealShip.OptCrew), SHIP_STAT_RANGE_GENERATION);
    rRealShip.MaxCrew         = makeint(sti(rRealShip.OptCrew) * 1.25 + 0.5);  // 25 процентов перегруза
    rRealShip.MinCrew         = GenerateShipMinCrew(rRealShip, SHIP_STAT_RANGE_GENERATION);

	rRealShip.Price	= GetShipPriceByTTH(iShip, &NullCharacter);
	if (sti(rRealShip.Price) <= 0) rRealShip.Price = 100;

	rRealShip.Stolen = isStolen;  // ворованность

	return iShip;
}

int GenerateShipExt(int iBaseType, bool isStolen, ref chr)
{
	string  attr;
	int 	i;
	aref 	refShip;
	float	fStatRange = SHIP_STAT_RANGE_GENERATION;
	bool	isShipyard = false;

	int iShip = CreateBaseShip(iBaseType);

	if (iShip == -1 || iShip == SHIP_NOTUSED) return SHIP_NOTUSED; // LEO: см. GenerateShip

	ref rRealShip = GetRealShip(iShip);

	SetShipsBonuses(rRealShip);

    if(CheckAttribute(rRealShip, "hullNums")) 
	{
        int nHulls = sti(rRealShip.hullNums);
        nHulls -= 1;
        if(nHulls < 0) nHulls = 0;
        rRealShip.ship.upgrades.hull = 1 + rand(nHulls);
    }
    else {rRealShip.ship.upgrades.hull = 1 + rand(2);}
	rRealShip.ship.upgrades.sails = 1 + rand(2);  // только визуальная разница

	if (!CheckAttribute(rRealShip, "QuestShip"))
		rRealShip.MastMultiplier = stf(stf(rRealShip.MastMultiplier)-0.3+makefloat(0.02*rand(30)));

	if (CheckAttribute(chr, "City"))// ugeen --> если кораблик генерится на верфи, разброс статов более узкий
	{
		string  sCity = chr.City;
		if(CheckCharacterID(chr, sCity + "_shipyarder"))
		{
			fStatRange 	= SHIP_STAT_RANGE_GENERATION_SHIPYARD;
			isShipyard 	= true;
		}
	}

	if (!CheckAttribute(rRealShip, "isFort"))
	{
		int iCannonDiff = 0;
		if (!isShipyard) iCannonDiff = rand(sti(rRealShip.MaxCannonDiff));

		// собственно сам рэндом стволов
		makearef(refShip, chr.Ship);
		ResetShipCannonsDamages(chr);

		if (!CheckAttribute(refShip,"soiling")) refShip.soiling = 0;

		// куда-нить запишем максимально возможное кол-во орудий (потом нужно будет, если захотим проапгрейдиться на этот счет)
		rRealShip.CannonsQuantityMax = sti(rRealShip.CannonsQuantity);
		// принципиальный момент ! нужно нулить обязательно левые стволы !
		for (i = 0; i < sti(rRealShip.rcannon); i++)
		{
			attr = "c" + i;
			rRealShip.Cannons.Borts.cannonr.damages.(attr) = 1.0;
			rRealShip.Cannons.Borts.cannonl.damages.(attr) = 1.0;
			rRealShip.Cannons.Borts.rcannon.damages.(attr) = 1.0;
			rRealShip.Cannons.Borts.lcannon.damages.(attr) = 1.0;

			refShip.Cannons.Borts.cannonr.damages.(attr) = 1.0;
			refShip.Cannons.Borts.cannonl.damages.(attr) = 1.0;
			refShip.Cannons.Borts.rcannon.damages.(attr) = 1.0;
			refShip.Cannons.Borts.lcannon.damages.(attr) = 1.0;
		}

		rRealShip.rcannon = sti(rRealShip.rcannon) - iCannonDiff;
		rRealShip.lcannon = sti(rRealShip.lcannon) - iCannonDiff;

		rRealShip.cannonr = sti(rRealShip.rcannon);
		rRealShip.cannonl = sti(rRealShip.lcannon);
		rRealShip.cannonf = sti(rRealShip.fcannon);
		rRealShip.cannonb = sti(rRealShip.bcannon);

		rRealShip.Cannons = sti(rRealShip.CannonsQuantityMax) - iCannonDiff * 2;
		rRealShip.CannonsQuantity = sti(rRealShip.Cannons);

		//а теперь вертаем стволы обратно в потребном нам количестве
		for (i = 0; i < sti(rRealShip.rcannon); i++)
		{
			attr = "c" + i;
			rRealShip.Cannons.borts.cannonr.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.cannonl.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.rcannon.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.lcannon.damages.(attr) = 0.0;

			refShip.Cannons.borts.cannonr.damages.(attr) = 0.0;
			refShip.Cannons.borts.cannonl.damages.(attr) = 0.0;
			refShip.Cannons.borts.rcannon.damages.(attr) = 0.0;
			refShip.Cannons.borts.lcannon.damages.(attr) = 0.0;
		}

		for (i = 0; i < sti(rRealShip.fcannon); i++)
		{
			attr = "c" + i;
			rRealShip.Cannons.borts.cannonf.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.fcannon.damages.(attr) = 0.0;
			refShip.Cannons.borts.cannonf.damages.(attr) = 0.0;
			refShip.Cannons.borts.fcannon.damages.(attr) = 0.0;
		}

		for (i = 0; i < sti(rRealShip.bcannon); i++)
		{
			attr = "c" + i;
			rRealShip.Cannons.borts.cannonb.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.bcannon.damages.(attr) = 0.0;
			refShip.Cannons.borts.cannonb.damages.(attr) = 0.0;
			refShip.Cannons.borts.bcannon.damages.(attr) = 0.0;
		}

		refShip.Cannons.Borts.cannonl = sti(rRealShip.lcannon);
		refShip.Cannons.Borts.cannonr = sti(rRealShip.rcannon);
		refShip.Cannons.Borts.cannonf = sti(rRealShip.fcannon);
		refShip.Cannons.Borts.cannonb = sti(rRealShip.bcannon);

		refShip.Cannons.Borts.lcannon = sti(rRealShip.lcannon);
		refShip.Cannons.Borts.rcannon = sti(rRealShip.rcannon);
		refShip.Cannons.Borts.fcannon = sti(rRealShip.fcannon);
		refShip.Cannons.Borts.bcannon = sti(rRealShip.bcannon);

		refShip.Cannons = sti(rRealShip.Cannons);
		// <-- рэндом стволов

		if(!CheckAttribute(rRealShip, "QuestShip"))
		{
			rRealShip.SpeedRate	       = GenerateShipStatF(stf(rRealShip.SpeedRate), fStatRange);
			rRealShip.TurnRate         = GenerateShipStatF(stf(rRealShip.TurnRate), fStatRange);
			rRealShip.HP               = GenerateShipStatI(sti(rRealShip.HP), fStatRange);
			rRealShip.WindAgainstSpeed = GenerateShipStatF(stf(rRealShip.WindAgainstSpeed), fStatRange);
		}
		else
		{
			switch (sti(rRealShip.Class))//уникам максимальную броню, чтоб игроки не сливали
			{
				case 7: rRealShip.HullArmor = 5; break;
				case 6: rRealShip.HullArmor = 9; break;
				case 5: rRealShip.HullArmor = 13; break;
				case 4: rRealShip.HullArmor = 18; break;
				case 3: rRealShip.HullArmor = 24; break;
				case 2: rRealShip.HullArmor = 30; break;
				case 1: rRealShip.HullArmor = 36; break;
			}
		}
	}

	if(!CheckAttribute(rRealShip, "QuestShip"))
	{
		rRealShip.Capacity        = GenerateShipStatI(sti(rRealShip.Capacity), fStatRange);
		rRealShip.OptCrew         = GenerateShipStatI(sti(rRealShip.OptCrew), fStatRange);
		rRealShip.MaxCrew         = makeint(sti(rRealShip.OptCrew) * 1.25 + 0.5);  // 25 процентов перегруза
		rRealShip.MinCrew         = GenerateShipMinCrew(rRealShip, fStatRange);
	}

	if (!CheckAttribute(rRealShip, "QuestShip"))
		SetCabinTypeEx(rRealShip, sti(rRealShip.Class)); //Выдача случайной каюты по классу не квестовым - Gregg

	rRealShip.Price	= GetShipPriceByTTH(iShip, chr); 
	if (CheckAttribute(rRealShip, "QuestShip")) rRealShip.Price	= sti(rRealShip.Price) * 2;
	if (sti(rRealShip.Price) <= 0) rRealShip.Price = 100;

	rRealShip.Stolen = isStolen;  // ворованность
	rRealShip.soiling = 0;
	if (rand(4)==0 && sti(rRealShip.Class) < 7 && !CheckAttribute(rRealShip, "QuestShip")) GetRandomSpecialUpgrade(rRealShip);
	else if (rand(99) < 3 && sti(rRealShip.Class) < 7) GetPreset(rRealShip);

	SetRandomNameToShip(chr);

	return iShip;
}
// -> ugeen

void GetRandomSpecialUpgrade(ref rRealShip)
{
	string shiptype = "";
	if (CheckAttribute(rRealShip,"QuestShip")) return;
	if (rRealShip.type.merchant == true && rRealShip.type.war == true) shiptype = "Universal";
	else if (rRealShip.type.merchant == true) shiptype = "Merchant";
	else if (rRealShip.type.war == true) shiptype = "War";
	int z = 1 + rand(2);
	int upnum = 0;
	while (upnum < z)
	{
		string tempstring = ShipsUpgrades[rand(17)];
		if (HasSubStr(GetConvertStr(tempstring+"_TYPE","ShipsUpgrades.txt"),shiptype) && tempstring != "SUP_BRANDER")
		{
			CheckForShipStatsChange(rRealShip,tempstring);
			rRealShip.Upgrade.(tempstring) = true;
			upnum++;
		}
	}
}

void GetPreset(ref rRealShip)
{
	string shiptype = "";
	string uptypes = "";
	if (CheckAttribute(rRealShip,"QuestShip")) return;
	if (rRealShip.type.merchant == true && rRealShip.type.war == true) 
	{
		shiptype = "Universal";
		uptypes = "SUP_SAIL|SUP_HULLKRENG|SUP_FALCONETS|SUP_REINFORCEDCONSTRUCTION";
	}
	else if (rRealShip.type.merchant == true) 
	{
		shiptype = "Merchant";
		uptypes = "SUP_TRADEARMOUR|SUP_HULLKRENG|SUP_CAPACITY|SUP_HAMMOCKS";
	}
	else if (rRealShip.type.war == true) 
	{
		shiptype = "War";
		uptypes = "SUP_GUNS|SUP_HULLKRENG|SUP_BIGSIDES|SUP_BARRICADES";
	}
	for (int i = 0; i < 18; i++)
	{
		string tempstring = ShipsUpgrades[i];
		if (HasSubStr(GetConvertStr(tempstring+"_TYPE","ShipsUpgrades.txt"),shiptype) && HasSubStr(uptypes,tempstring))
		{
			CheckForShipStatsChange(rRealShip,tempstring);
			rRealShip.Upgrade.(tempstring) = true;
		}
	}
}

void SetCabinTypeEx(ref rRealShip, int shipclass) //Выдача случайной каюты по классу не квестовым - Gregg
{
	if (shipclass == 7) {rRealShip.CabinType = "New_Cabin3"; return;}

	if (shipclass == 6)
	{
		switch (rand(2))
		{
			case 0: rRealShip.CabinType = "Cabin_Small"; return;
			break;
			case 1: rRealShip.CabinType = "Cabin_Small2"; return;
			break;
			case 2: rRealShip.CabinType = "New_Cabin1"; return;
			break;
		}
	}
	if (shipclass == 5)
	{
		switch (rand(2))
		{
			case 0: rRealShip.CabinType = "Cabin_Small2"; return;
			break;
			case 1: rRealShip.CabinType = "New_Cabin1"; return;
			break;
			case 2: rRealShip.CabinType = "Cabin_Medium1"; return;
			break;
		}
	}
	if (shipclass == 4)
	{
		switch (rand(2))
		{
			case 0: rRealShip.CabinType = "Cabin_Medium1"; return;
			break;
			case 1: rRealShip.CabinType = "Cabin_Medium"; return;
			break;
			case 2: rRealShip.CabinType = "Cabin_Medium2"; return;
			break;
		}
	}
	if (shipclass == 3)
	{
		switch (rand(2))
		{
			case 0: rRealShip.CabinType = "Cabin_Medium"; return;
			break;
			case 1: rRealShip.CabinType = "Cabin_Medium2"; return;
			break;
			case 2: rRealShip.CabinType = "New_Cabin2"; return;
			break;
		}
	}
	if (shipclass == 2)
	{
		switch (rand(2))
		{
			case 0: rRealShip.CabinType = "New_Cabin2"; return;
			break;
			case 1: rRealShip.CabinType = "Cabin_Quest"; return;
			break;
			case 2: rRealShip.CabinType = "Cabin"; return;
			break;
		}
	}
	if (shipclass == 1)
	{
		switch (rand(2))
		{
			case 0: rRealShip.CabinType = "Cabin"; return;
			break;
			case 1: rRealShip.CabinType = "Cabin_Huge"; return;
			break;
			case 2: rRealShip.CabinType = "Cabin_Quest"; return;
			break;
		}
	}
}

int CreateBaseShip(int iBaseType)
{
	int iShip = FindFirstEmptyShip();
	if (iShip == -1)
	{
		RealShips_CollectGarbage();
		iShip = FindFirstEmptyShip();
	}

	if(iShip == -1)
	{
        trace("Can not find empty ship slot");
		if (g_shipDumpDone == 0) // LEO: раскладка, кем заняты все слоты - для разбора переполнения
		{
			g_shipDumpDone = 1;
			RealShips_DumpStats();
		}
		return SHIP_NOTUSED;
	}

	ref rBaseShip = GetShipByType(iBaseType);
	ref rRealShip = GetRealShip(iShip);
	DeleteAttribute(rRealShip, "");
	CopyAttributes(rRealShip, rBaseShip);
    rRealShip.index = iShip;
    rRealShip.BaseType = iBaseType;
	rRealShip.Price = GetPriceByTTH(rRealShip, &NullCharacter); // LEO: цена сразу по ТТХ - в ините её больше нет
    if (!CheckAttribute(rRealShip, "ship.upgrades.hull")) // если задано в базе, то квестовый тип, не перебивать
    {
    	rRealShip.ship.upgrades.hull = 1;  //признак корабля теперь
    }
	rRealShip.ship.upgrades.sails = 1;

	int hullarmor;//реворк брони корпуса
	switch (rand(1))
	{
		case 0: hullarmor = 1; break;
		case 1: hullarmor = -1 break;
	}
	switch (sti(rRealShip.Class))
	{
		case 7: rRealShip.HullArmor = 4+(rand(1)*hullarmor); break;
		case 6: rRealShip.HullArmor = 8+(rand(1)*hullarmor); break;
		case 5: rRealShip.HullArmor = 12+(rand(1)*hullarmor); break;
		case 4: rRealShip.HullArmor = 16+(rand(2)*hullarmor); break;
		case 3: rRealShip.HullArmor = 22+(rand(2)*hullarmor); break;
		case 2: rRealShip.HullArmor = 28+(rand(2)*hullarmor); break;
		case 1: rRealShip.HullArmor = 34+(rand(2)*hullarmor); break;
	}

    rRealShip.BaseName = rRealShip.name; // запоминалка для нужд, тк далее идёт GetConvertStrWithReplace("Variable_ShipsUtilites_1", "Names.txt", "#space#", " ")
	if (rRealShip.name != "Fort" && rRealShip.name != "Boat") // не знаю зачем :(
	{
		rRealShip.name = rRealShip.name + "1"; // без этого вылет, но почему!!!!!!!!!!!!!!
		// выяснил - папка с моделью имеет на конце 1, вот и ответ
	}
    rRealShip.Stolen = 0;  // ворованность - честный

	if (iArcadeSails == 0) // момент инерции ниже для тактики
	{
	    rRealShip.InertiaAccelerationY = stf(rRealShip.InertiaAccelerationY) / 2.0;
	}

	return iShip;
}

void ResetShipCannonsDamages(ref refCharacter)
{
	aref refShip;

	makearef(refShip, refCharacter.Ship);
	refShip.Cannons.Borts = "";

	if (CheckAttribute(refShip, "Cannons.Borts.cannonf.damages")) { DeleteAttribute(refShip, "Cannons.Borts.cannonf.damages"); }
	if (CheckAttribute(refShip, "Cannons.Borts.cannonb.damages")) { DeleteAttribute(refShip, "Cannons.Borts.cannonb.damages"); }
	if (CheckAttribute(refShip, "Cannons.Borts.cannonl.damages")) { DeleteAttribute(refShip, "Cannons.Borts.cannonl.damages"); }
	if (CheckAttribute(refShip, "Cannons.Borts.cannonr.damages")) { DeleteAttribute(refShip, "Cannons.Borts.cannonr.damages"); }

	if (CheckAttribute(refShip, "Cannons.Borts.fcannon.damages")) { DeleteAttribute(refShip, "Cannons.Borts.fcannon.damages"); }
	if (CheckAttribute(refShip, "Cannons.Borts.bcannon.damages")) { DeleteAttribute(refShip, "Cannons.Borts.bcannon.damages"); }
	if (CheckAttribute(refShip, "Cannons.Borts.lcannon.damages")) { DeleteAttribute(refShip, "Cannons.Borts.lcannon.damages"); }
	if (CheckAttribute(refShip, "Cannons.Borts.rcannon.damages")) { DeleteAttribute(refShip, "Cannons.Borts.rcannon.damages"); }
}

// Генерация корабля для верфи
int GenerateStoreShip(int iBaseType)
{
	int iShip = GenerateShip(iBaseType, 0); // честный

	if (iShip == -1 || iShip == SHIP_NOTUSED) // LEO: GenerateShip при нехватке слотов отдаёт SHIP_NOTUSED
	{
		return SHIP_NOTUSED;
	}

	ref rRealShip = GetRealShip(iShip);
    rRealShip.StoreShip = true;  // корабль на верфи, трется отдельным методом

	return iShip;
}

// переработка метода 26.05.06 boal

// -> ugeen 25.01.09 -> для верфи
int GenerateStoreShipExt(int iBaseType, ref chr)
{
	int iShip = GenerateShipExt(iBaseType, 0, chr); // честный

	if (iShip == -1 || iShip == SHIP_NOTUSED) // LEO: GenerateShipExt при нехватке слотов отдаёт SHIP_NOTUSED
	{
		return SHIP_NOTUSED;
	}

	ref rRealShip = GetRealShip(iShip);
	rRealShip.StoreShip = false;  // корабль на верфи, трется отдельным методом

	return iShip;
}
// <- ugeen

// идея: трем все корабли, где нет НПС-владельца, все галки пофиг
void EmptyAllFantomShips()
{
	int  i;
	ref  chr;
	// оптимальный код - два прохода, а не вложенный цикл. В начале всем сносим флаг удаляемости, потом проставляем, где есть НПС
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
        DeleteAttribute(&RealShips[i], "lock");
	}
	// лодку ставим, тк он имеет индекс НПС за пределами MAX_CHARACTERS
	// теперь НПС-кэпы
	// LEO: фикс утечки кораблей ПУ/аукциона (подробно в OnLoadReinit.c ClearAllFantomShips). Было `i < MAX_CHARACTERS` -
	//   пробел [MAX_CHARACTERS, 1700) не сканился, сторожа теряли lock, GC стирал их корабль. Сканим [0, FANTOM_CHARACTERS); id!="0".
	for (i = 0; i < FANTOM_CHARACTERS; i++)
	{
        chr = &Characters[i];
        if (chr.id != "0" && CheckAttribute(chr, "ship.type") && chr.ship.type != SHIP_NOTUSED)
        {
            RealShips[sti(chr.ship.type)].lock = true;
        }
        // очистка для верфи
		if (CheckAttribute(chr, "shipyard"))
		{
		    FreeShipFromShipyard(chr);
		}
	}
	for (i = FANTOM_CHARACTERS; i < TOTAL_CHARACTERS; i++) // фантомы в море
	{
        chr = &Characters[i];
        if (CheckAttribute(chr, "ship.type") && chr.ship.type != SHIP_NOTUSED)
        {
            RealShips[sti(chr.ship.type)].lock = true;
        }
		else
		{
			if (CheckAttribute(chr,"lifeday") && sti(chr.lifeday) == 0) InitCharacter(&Characters[i], i);
		}
	}
	RealShips_LockShipyardShips(); // LEO: корабли верфей держит ссылка из NPChar.shipyard, а не флаг StoreShip
	// теперь сборка мусора
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
        if (!CheckAttribute(&RealShips[i], "lock") && !CheckAttribute(&RealShips[i], "HiredPrize"))  // LEO: HiredPrize - слот приза наёмного пирата (капер-однодневка умирает, слот иначе стирался -> корабль без имени пропадал)
        {
			DeleteAttribute(&RealShips[i], "");
		}
	}
}

// LEO: верфь удерживает слот реальной ссылкой; один забытый StoreShip не мешает сборке мусора.
void RealShips_LockShipyardShips()
{
	int i, j, iNum, iShip;
	aref arDest, arImt;
	for (i = 0; i < FANTOM_CHARACTERS; i++)
	{
		if (!CheckAttribute(&Characters[i], "shipyard")) continue;
		makearef(arDest, Characters[i].shipyard);
		iNum = GetAttributesNum(arDest);
		for (j = 0; j < iNum; j++)
		{
			arImt = GetAttributeN(arDest, j);
			if (!CheckAttribute(arImt, "Type")) continue;
			iShip = sti(arImt.Type);
			if (iShip >= 0 && iShip < REAL_SHIPS_QUANTITY) RealShips[iShip].lock = true;
		}
	}
}

// LEO: диагностика - кем заняты слоты RealShips. Зовётся из CreateBaseShip, когда даже после сборки мусора
//   свободного слота нет. Пишет в compile.log раскладку по категориям и список владельцев.
void RealShips_DumpStats()
{
	int i, j, iNum, iType;
	int nFree = 0;
	int nYard = 0;
	int nStoreLost = 0;
	int nPrize = 0;
	int nStatic = 0;
	int nFantom = 0;
	int nEmptyChr = 0;
	int nOrphan = 0;
	int nYards = 0;
	string sLine = "";
	ref chr;
	aref arDest, arImt;

	for (i = 0; i < REAL_SHIPS_QUANTITY; i++) DeleteAttribute(&RealShips[i], "diag");
	for (i = 0; i < TOTAL_CHARACTERS; i++)
	{
		chr = &Characters[i];
		if (CheckAttribute(chr, "shipyard"))
		{
			nYards++;
			makearef(arDest, chr.shipyard);
			iNum = GetAttributesNum(arDest);
			for (j = 0; j < iNum; j++)
			{
				arImt = GetAttributeN(arDest, j);
				if (!CheckAttribute(arImt, "Type")) continue;
				iType = sti(arImt.Type);
				if (iType >= 0 && iType < REAL_SHIPS_QUANTITY) RealShips[iType].diag.yard = chr.id;
				else trace("RealShips diag: shipyard " + chr.id + " entry " + GetAttributeName(arImt) + " has bad Type " + iType);
			}
		}
		if (!CheckAttribute(chr, "ship.type")) continue;
		iType = sti(chr.ship.type);
		if (iType < 0 || iType >= REAL_SHIPS_QUANTITY) continue;
		if (CheckAttribute(&RealShips[iType], "diag.owner")) continue;
		RealShips[iType].diag.owner = i;
	}
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
		if (!CheckAttribute(&RealShips[i], "name")) { nFree++; continue; }
		if (CheckAttribute(&RealShips[i], "diag.yard")) { nYard++; continue; }
		if (CheckAttribute(&RealShips[i], "HiredPrize")) { nPrize++; continue; }
		if (CheckAttribute(&RealShips[i], "diag.owner"))
		{
			j = sti(RealShips[i].diag.owner);
			if (Characters[j].id == "0") nEmptyChr++;
			else
			{
				if (j >= FANTOM_CHARACTERS) nFantom++;
				else
				{
					nStatic++;
					sLine = sLine + " " + Characters[j].id;
					if (strlen(sLine) > 200) { trace("RealShips diag owners:" + sLine); sLine = ""; }
				}
			}
			continue;
		}
		if (CheckAttribute(&RealShips[i], "StoreShip"))
		{
			nStoreLost++;
			trace("RealShips diag: lost StoreShip slot " + i + " " + RealShips[i].BaseName);
			continue;
		}
		nOrphan++;
	}
	if (sLine != "") trace("RealShips diag owners:" + sLine);
	sLine = "RealShips diag: free " + nFree + ", shipyard " + nYard + " in " + nYards + " yards";
	sLine = sLine + ", lost StoreShip " + nStoreLost + ", HiredPrize " + nPrize;
	sLine = sLine + ", static chars " + nStatic + ", fantoms " + nFantom;
	sLine = sLine + ", empty chr slots " + nEmptyChr + ", orphan " + nOrphan;
	trace(sLine);
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++) DeleteAttribute(&RealShips[i], "diag");
}

// LEO: сборка мусора RealShips без побочек: верфи и фантомов не трогаем, только стираем слоты, на которые не
//   ссылается ни один персонаж. Штатная EmptyAllFantomShips идёт лишь на переходах с морем, и долгое путешествие
//   по суше забивало все 900 слотов: новые корабли получали битый тип, загрузка порта падала (краш 25.09.2026).
//   Зовётся из CreateBaseShip, когда свободного слота нет.
void RealShips_CollectGarbage()
{
	int i, iType;
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
		DeleteAttribute(&RealShips[i], "lock");
	}
	for (i = 0; i < TOTAL_CHARACTERS; i++)
	{
		if (!CheckAttribute(&Characters[i], "ship.type")) continue;
		if (Characters[i].id == "0") continue; // пустой слот со старым ship.type держал бы чужой корабль (как в EmptyAllFantomShips)
		iType = sti(Characters[i].ship.type);
		if (iType >= 0 && iType < REAL_SHIPS_QUANTITY) RealShips[iType].lock = true;
	}
	RealShips_LockShipyardShips(); // LEO: корабли верфей - по ссылке из NPChar.shipyard (см. там)
	int iFreed = 0;
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
		if (CheckAttribute(&RealShips[i], "lock")) continue;
		if (CheckAttribute(&RealShips[i], "HiredPrize")) continue;
		if (!CheckAttribute(&RealShips[i], "name")) continue;
		DeleteAttribute(&RealShips[i], "");
		iFreed++;
	}
	trace("RealShips_CollectGarbage: freed " + iFreed);
	if (iFreed > 0) g_shipDumpDone = 0;
}

int GetRealShipsCount()
{ // метод для отладки
	int result = 0;

	for (int i= 0; i< REAL_SHIPS_QUANTITY; i++)
	{
		if (checkAttribute(&RealShips[i], "name"))
		{
			result++;
		}
	}

	return result;
}

// поиск пустого слота
int FindFirstEmptyShip()
{
	// LEO: скан с роллинг-хинта (последний выданный+1) и с ОБОРОТОМ — вместо O(900) от нуля на каждый спавн.
	//   Гарантирует нахождение любого свободного слота (обходит все 900), но обычно свободный сразу за хинтом.
	int i, k;
	for(k = 0; k < REAL_SHIPS_QUANTITY; k++)
	{
		i = g_shipHint + k;
		if (i >= REAL_SHIPS_QUANTITY) i = i - REAL_SHIPS_QUANTITY; // оборот
		if(!checkAttribute(&RealShips[i], "name"))
		{
			//#20170912-02 Fix for RealShips/Character sails
		    DeleteAttribute(&RealShips[i], "ship.upgrades");
		    DeleteAttribute(&RealShips[i], "EmblemedSails");
		    DeleteAttribute(&RealShips[i], "GeraldSails");
			g_shipHint = i + 1;
			if (g_shipHint >= REAL_SHIPS_QUANTITY) g_shipHint = 0;
			return i;
		}
	}

	return -1;
}

//Boyer add for new game #20170301-6
void ResetRealShipArray()
{
	for(int i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
		DeleteAttribute(&RealShips[i], "name");
	}
	g_shipHint = 0; // LEO: все слоты свободны -> хинт в начало
	return;
}

//================//
float ShipSpeedBonusFromWeight(ref _refCharacter, ref rShip)
{	// от загрузки трюма
    if(!CheckAttribute(_refCharacter, "Cargo")) RecalculateCargoLoad(_refCharacter);

	float fCapacity = stf(rShip.Capacity);
	float fLoad = Clampf((GetCargoLoad(_refCharacter) - 0.7 * fCapacity) / fCapacity);

	float fSRFromWeight = stf(rShip.SpeedDependWeight);
	if(fSRFromWeight > 0.2){fSRFromWeight = 0.2;}//убрать при след ребалансе
	fSRFromWeight = Clampf(1 - fSRFromWeight * fLoad / (1- 0.7));
	return fSRFromWeight;
}
float ShipSpeedBonusFromHP(ref _refCharacter, ref rShip)
{	// от повреждения корпуса
	float fShipHp = stf(_refCharacter.ship.hp);
	float fShipMaxHp = stf(rShip.HP);
	// Доп. помпы снимают штраф к скорости в критической зоне (корпус < 20%) — в той же точке, где корабль перестаёт тонуть (см. AIShip.c)
	if (CheckForUpgrade(rShip,"SUP_PUMPS") && (fShipHp/fShipMaxHp < 0.2)) return 1.0;
	return Bring2Range(0.8, 1.0, 0.0, SHIP_HP_PENALTY_THRESH*0.01, fShipHp/fShipMaxHp);
}
float ShipSpeedBonusFromPeople(ref _refCharacter, ref rShip)	
{	// от команды
	float fCrewMax = stf(rShip.MaxCrew);
	float fCrewOpt = stf(rShip.OptCrew);
	float fCrewCur = stf(_refCharacter.Ship.Crew.Quantity);
	if (fCrewCur > fCrewMax) fCrewCur = fCrewMax;
	float fSRFromPeople = Clampf(0.85 + ((GetCrewExp(_refCharacter, "Sailors") * fCrewCur) / (fCrewOpt * GetCrewExpRate())) * 0.25);
	return fSRFromPeople;
}
float ShipSpeedBonusFromSails(ref _refCharacter)
{	// от повреждения парусов
	float fSRFromSailDamage = pow(Bring2Range(0.00001, 1.0, 0.001, SHIP_SP_PENALTY_THRESH, stf(_refCharacter.ship.sp)), 0.66);//0.3
	//симулируем запас защищающий от 1 залпа - 5%
	//считаем мощность парусов - скорость ^1/3
	//учитываем часть парусов которую нельзя повредить - 35-40%, добавляем условные 35-50% к скорости от не-повреждаемой части.
	//высчитываем степень Х по среднему значению из высчитанной области
	return fSRFromSailDamage;
}
float ShipSpeedBonusFromSoiling(ref _refCharacter)
{	// от загрязнения дна
	float fSoiling = (100-(stf(_refCharacter.ship.soiling)*0.3))/100;
	return fSoiling;
}
float ShipSpeedBonusFromUpgrades(ref _refCharacter, ref rShip)
{	// от апгрейдов
	float fSRFromUpgrades = 1.0;
	return fSRFromUpgrades;
}
//скорость корабля от навыка. shiptype = SHIP_NOTUSED - корабль персонажа
float SpeedBySkill(aref refCharacter, int nShipType)
{
	float fSkill = GetSummonSkillFromName(refCharacter, SKILL_SAILING);

	int shipClass;
	if (nShipType == SHIP_NOTUSED) {
		shipClass = GetCharacterShipClass(refCharacter);
	}
	else {
		shipClass = sti(RealShips[nShipType].Class);
	}
	float fNeedSkillMin, fNeedSkillMax, fSkillMax;

	fNeedSkillMin = GetShipClassNavySkill(shipClass);
	if (fNeedSkillMin == SKILL_MAX)
	{
		fNeedSkillMax = SKILL_MAX + 1;
		fSkillMax = SKILL_MAX + 1;
	}
	else
	{
		if (fNeedSkillMin < 1) fNeedSkillMin = 1;
		fNeedSkillMax = fNeedSkillMin + 10;
		if (fNeedSkillMax > SKILL_MAX) fNeedSkillMax = SKILL_MAX;
		fSkillMax = SKILL_MAX;
	}

	float fSRFromSkill = 0.6 +
		Bring2Range(0.0, 0.004 * fNeedSkillMin, 0.0, fNeedSkillMin, fSkill) +
		Bring2Range(0.0, 0.3 - 0.003 * fNeedSkillMin, fNeedSkillMin, fNeedSkillMax, fSkill) +
		Bring2Range(0.0, 0.1 - 0.001 * fNeedSkillMin, fNeedSkillMin, fSkillMax, fSkill);

    float fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "ShipSpeedUp"), 1.0, 1.15);   //slib
    fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "SailingProfessional"), fSpeedPerk, 1.20);
	fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "SeaWolf"), fSpeedPerk, fSpeedPerk + 0.05);
	if (CheckAttribute(refCharacter,"GhostShipTuning")) fSpeedPerk *= 1.05;

	return fSRFromSkill * fSpeedPerk / 1.2;
}

float FindShipSpeedBonus(ref refCharacter, ref rShip)
{
	if(!CheckAttribute(refCharacter, "Ship.type"))
	{
		trace("Character " + refCharacter.id + " have no ship!");
		return 0.0;
	}
	int nShipType = sti(refCharacter.ship.type);
	if(nShipType == SHIP_NOTUSED)
	{
		trace("Character " + refCharacter.id + " have invalid ship!");
		return 0.0;
	}
	float fBonus = 1.0;
	fBonus *= AIShip_isPerksUse(CheckCharacterPerk(refCharacter, "MapMaker"), 1.0, 1.05);
	fBonus *= ShipSpeedBonusFromWeight(refCharacter, rShip);
	fBonus *= ShipSpeedBonusFromHP(refCharacter, rShip);
	fBonus *= ShipSpeedBonusFromPeople(refCharacter, rShip);
	fBonus *= ShipSpeedBonusFromSails(refCharacter);
	fBonus *= ShipSpeedBonusFromSoiling(refCharacter);
	fBonus *= ShipSpeedBonusFromUpgrades(refCharacter, rShip);
	fBonus *= SpeedBySkill(refCharacter, nShipType);
	return fBonus;
}

float FindShipSpeed(aref refCharacter, ref rShip)
{
	float fBonus = FindShipSpeedBonus(refCharacter, rShip);
	float fMaxSpeedZ = stf(rShip.SpeedRate);  // базовая скорость	
	return fBonus * fMaxSpeedZ;
}

//================//
float ShipTurnRateBonusFromWeight(ref _refCharacter, ref rShip)
{	// от загрузки трюма
    if(!CheckAttribute(_refCharacter, "Cargo")) RecalculateCargoLoad(_refCharacter);

	float fCapacity = stf(rShip.Capacity);
	float fLoad = Clampf((GetCargoLoad(_refCharacter) - 0.5 * fCapacity) / fCapacity);//штраф начинается при 50% дедвейта

	float fTRFromWeight = stf(rShip.TurnDependWeight);
	if(fTRFromWeight > 0.2){fTRFromWeight = 0.2;}//убрать при след ребалансе
	fTRFromWeight = Clampf(1 - fTRFromWeight * fLoad / (1 - 0.5));
	return fTRFromWeight;
}
float ShipTurnRateBonusFromHP(ref _refCharacter, ref rShip)
{	// от повреждения корпуса
	float fShipHp = stf(_refCharacter.Ship.hp);
	float fShipMaxHp = stf(rShip.HP);
	float fSpeedFromHp = fShipHp/fShipMaxHp;
	fSpeedFromHp = 0.7 + fSpeedFromHp * 0.3;
	return fSpeedFromHp;
}
float ShipTurnRateBonusFromPeopleUncapped(ref _refCharacter, float fCrewCur, ref rShip)
{	// от команды
	float fCrewMax = stf(rShip.MaxCrew);
	float fCrewOpt = stf(rShip.OptCrew);
	if (fCrewCur > fCrewMax) fCrewCur = fCrewMax;
	float  fTRFromPeople;

	fTRFromPeople = 0.3 + ((GetCrewExp(_refCharacter, "Sailors") * fCrewCur) / (fCrewOpt * GetCrewExpRate())) * 1.0;
	return fTRFromPeople;
}
float ShipTurnRateBonusFromPeople(ref _refCharacter, ref rShip)
{	// от команды
	float fCrewCur = stf(_refCharacter.Ship.Crew.Quantity);
	float fTRFromPeople = ShipTurnRateBonusFromPeopleUncapped(_refCharacter, fCrewCur, rShip);
	if (fTRFromPeople > 1) fTRFromPeople = 1;
	return fTRFromPeople;
}
float ShipTurnRateBonusFromSails(ref _refCharacter)
{	// от повреждения парусов
	float fTRFromSailDamage = pow(Bring2Range(0.00001, 1.0, 0.001, SHIP_SP_PENALTY_THRESH, stf(_refCharacter.ship.sp)), 0.66);
	//см. ShipSpeedBonusFromSails
	return fTRFromSailDamage;
}
float ShipTurnRateBonusFromSoiling(ref _refCharacter)
{	// от загрязнения дня
	float fSoiling = (100-(stf(_refCharacter.ship.soiling)*0.15))/100;
	return fSoiling;
}
float ShipTurnRateBonusFromUpgrades(ref _refCharacter, ref rShip)
{	// от апгрейдов
	float fTRFromUpgrades = 1.0;
	return fTRFromUpgrades;
}

float TurnBySkill(aref refCharacter, int nShipType)
{
	float fSkill = GetSummonSkillFromName(refCharacter, SKILL_SAILING);

	int shipClass;
	if (nShipType == SHIP_NOTUSED) {
		shipClass = GetCharacterShipClass(refCharacter);
	}
	else {
		shipClass = sti(RealShips[nShipType].Class);
	}
	if (refCharacter.ship.type == SHIP_NOTUSED)
	{
		return 0.0;
	}
	float fNeedSkillMin, fNeedSkillMax, fSkillMax;

	fNeedSkillMin = GetShipClassNavySkill(shipClass);
	if (fNeedSkillMin == SKILL_MAX)
	{
		fNeedSkillMax = SKILL_MAX + 1;
		fSkillMax = SKILL_MAX + 1;
	}
	else
	{
		if (fNeedSkillMin < 1) fNeedSkillMin = 1;
		fNeedSkillMax = fNeedSkillMin + 10;
		if (fNeedSkillMax > SKILL_MAX) fNeedSkillMax = SKILL_MAX;
		fSkillMax = SKILL_MAX;
	}

	float fTRFromSKill;
	if (iArcadeSails == 1)
	{
		fTRFromSKill = 0.5 +
			Bring2Range(0.0, 0.005 * fNeedSkillMin, 0.0, fNeedSkillMin, fSkill) +
			Bring2Range(0.0, 0.35 - 0.0035 * fNeedSkillMin, fNeedSkillMin, fNeedSkillMax, fSkill) +
			Bring2Range(0.0, 0.15 - 0.0015 * fNeedSkillMin, fNeedSkillMin, fSkillMax, fSkill);
	}
	else
	{
		fTRFromSKill = 0.3 +
			Bring2Range(0.0, 0.007 * fNeedSkillMin, 0.0, fNeedSkillMin, fSkill) +
			Bring2Range(0.0, 0.4 - 0.004 * fNeedSkillMin, fNeedSkillMin, fNeedSkillMax, fSkill) +
			Bring2Range(0.0, 0.3 - 0.003 * fNeedSkillMin, fNeedSkillMin, fSkillMax, fSkill);
	}

    float fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "ShipTurnRateUp"), 1.0, 1.15);   //slib
    fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "SailingProfessional"), fSpeedPerk, 1.20);
	fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "SeaWolf"), fSpeedPerk, fSpeedPerk+0.05);
    float fFastTurn180 = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "Turn180"), 1.0, 1.4);
	if (CheckAttribute(refCharacter,"GhostShipTuning")) fSpeedPerk *= 1.05;

	return fTRFromSKill * fSpeedPerk * fFastTurn180 / 1.2;
}

float FindShipTurnRateBonus(aref refCharacter, ref rShip)
{
	if(!CheckAttribute(refCharacter, "Ship.type"))
	{
		trace("Character " + refCharacter.id + " have no ship!");
		return 0.0;
	}
	int nShipType = sti(refCharacter.ship.type);
	if(nShipType == SHIP_NOTUSED)
	{
		trace("Character " + refCharacter.id + " have invalid ship!");
		return 0.0;
	}

	float fBonus = 1.0;
	fBonus *= ShipTurnRateBonusFromWeight(refCharacter, rShip);
	fBonus *= ShipTurnRateBonusFromHP(refCharacter, rShip);
	fBonus *= ShipTurnRateBonusFromPeople(refCharacter, rShip);
	fBonus *= ShipTurnRateBonusFromSails(refCharacter);
	fBonus *= ShipTurnRateBonusFromSoiling(refCharacter);
	fBonus *= ShipTurnRateBonusFromUpgrades(refCharacter, rShip)
	fBonus *= TurnBySkill(refCharacter, nShipType);
	return fBonus;
}

float FindShipTurnRate(aref refCharacter, ref rShip)
{
	return FindShipTurnRateBonus(refCharacter, rShip);
}

//================//

// calculate recharge time for cannon
float Cannon_GetRechargeTimeValue(ref aCharacter)
{
	if(!CheckAttribute(aCharacter, "Ship.type"))	{ return 0.0; }
	int nShipType = sti(aCharacter.ship.type);
	if(nShipType == SHIP_NOTUSED)	{ return 0.0; }

	int		iCannonType = sti(aCharacter.Ship.Cannons.Type);
	ref		rCannon = GetCannonByType(iCannonType);
	float	fReloadTime = GetCannonReloadTime(rCannon); // boal 28.01.03 - 09.02.05
	float	fCannonSkill = 1.0 + stf(aCharacter.TmpSkill.Cannons);
	if (iCannonType >=  CANNON_TYPE_CANNON_LBS8)//пушка или кулеврина
	{
		fCannonSkill = fCannonSkill - 0.1 * (MakeFloat(iCannonType) + 1 - CANNON_TYPE_CANNON_LBS8);// <1 штраф, >1 бонус
	}
	else
	{
		fCannonSkill = fCannonSkill - 0.1 * (MakeFloat(iCannonType) + 1);
	}
	if (fCannonSkill > 1.5) { fCannonSkill = 1.5; }
	if (fCannonSkill < 0.65) { fCannonSkill = 0.65; }
	float fMultiply = 1.0;
	// boal -->
	if (CheckAttribute(aCharacter, "TmpPerks")) {
		fMultiply = AIShip_isPerksUse(aCharacter.TmpPerks.FastReload, 1.0, 0.9);
		fMultiply = AIShip_isPerksUse(aCharacter.TmpPerks.ImmediateReload, fMultiply, 0.7);
	}
	else {
		fMultiply = AIShip_isPerksUse(CheckOfficersPerk(aCharacter, "FastReload"), 1.0, 0.9);
		fMultiply = AIShip_isPerksUse(CheckOfficersPerk(aCharacter, "ImmediateReload"), fMultiply, 0.7);	
	}

	fMultiply *= (1+CheckOfficersPerk(aCharacter,"InstantRepair"));//х2 времени при активной быстрой починке

	ref refBaseShip = GetRealShip(sti(aCharacter.ship.Type));
	if (CheckForUpgrade(refBaseShip,"SUP_GUNS")) fMultiply *= 1.1;
	if (sti(refBaseShip.BaseType) != SHIP_FORT)
	{
        // boal 060804 для компа поблажки
		//Boyer remove reload speed boost for enemies

		// boal <--
		fReloadTime = Cannon_GetRechagreTimeBonusFromPeople(aCharacter, fReloadTime);
    // boal  корректный учет команды <--
    }
	return  fMultiply * fReloadTime / fCannonSkill;
}

int GetCannonsNum(aref chr)
{
	int nShipType = sti(chr.ship.type);
	if(nShipType == SHIP_NOTUSED)
	{
		trace("Character " +chr.id + " have invalid ship!");
		return 0;
	}
    ref refBaseShip = GetRealShip(nShipType);
	int iCannons = GetBortIntactCannonsNum(chr, "fcannon", sti(refBaseShip.fcannon)) + GetBortIntactCannonsNum(chr, "bcannon", sti(refBaseShip.bcannon)) + GetBortIntactCannonsNum(chr, "lcannon", sti(refBaseShip.lcannon)) + GetBortIntactCannonsNum(chr, "rcannon", sti(refBaseShip.rcannon));
	return iCannons;
}

// ugeen  02.02.09
int GetCannonsNumMax(aref chr)
{
	int nShipType = sti(chr.ship.type);
	ref refBaseShip = GetRealShip(nShipType);

	int iCannons = sti(refBaseShip.CannonsQuantityMax);

	return iCannons;
}
// ugeen

// boal
int GetBortCannonsQty(aref chr, string  sType)
{
	int nShipType = sti(chr.ship.type);
	if(nShipType == SHIP_NOTUSED)
	{
		trace("Character " +chr.id + " have invalid ship!");
		return 0;
	}
    ref refBaseShip = GetRealShip(nShipType);

	int iCannons = GetBortIntactCannonsNum(chr, sType, sti(refBaseShip.(sType)));

	return iCannons;
}

int GetBortCannonsMaxQty(aref chr, string  sType)
{
	int nShipType = sti(chr.ship.type);
	if(nShipType == SHIP_NOTUSED)
	{
		trace("Character " +chr.id + " have invalid ship!");
		return 0;
	}
    ref refBaseShip = GetRealShip(nShipType);

	int iCannons = sti(refBaseShip.(sType));

	return iCannons;
}

int GetBortCannonsBaseQty(ref chr, string sBort)
{
	int nShipType = sti(chr.ship.type);
	ref rRealShip = GetRealShip(nShipType);
	ref rBaseShip = GetShipByType(sti(rRealShip.BaseType));

	int iCannons = sti(rBaseShip.(sBort));

	return iCannons;
}
///////////////////////  ВЕРФИ //////////////////////
// Gregg: имя атрибута нации в ShipsTypes[].nation.* по номеру нации — было 5 копий switch по нации.
// Динамический доступ .(sNat) заменяет пятикейсовый switch одним циклом.
string GetShipsTypeNationAttr(int iNation)
{
	switch (iNation)
	{
		case ENGLAND: return "england"; break;
		case FRANCE:  return "france";  break;
		case SPAIN:   return "spain";   break;
		case HOLLAND: return "holland"; break;
		case PIRATE:  return "pirate";  break;
	}
	return "";
}

//Lipsar учет нации на верфях-->
void SetShipyardStore(ref NPChar)
{
    int    iTest_ship, i;
	string attrName;

	if (CheckAttribute(NPChar, "shipyard")) return; // ещё есть корабли с того раза

	SaveCurrentNpcQuestDateParam(npchar, "shipyardDate"); // дата заполнения верфи

    if (bBettaTestMode)
    {
        // LEO: полный отладочный каталог держим только у текущей верфи, иначе исчерпываются слоты.
        for (i = 0; i < FANTOM_CHARACTERS; i++)
        {
            if (CheckAttribute(&Characters[i], "shipyard")) RemoveShipFromShipyard(&Characters[i]);
        }
        RealShips_CollectGarbage();
        for (i = 1; i <=SHIP_TYPES_QUANTITY; i++)
        {
            attrName = "ship" + i;
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(i-1, NPChar), attrName);
        }
        return;
    }

	FillShipParamShipyard(NPChar, GenerateStoreShipExt(SHIP_LEUDO, NPChar), "ship1"); // обязательный на верфи

	string shipyarder_nation=NPChar.nation;//учет нации Lipsar
	// 0-england 1-france 2-spain 3-holland 4-pirate;
	int storeArray[SHIP_TYPES_QUANTITY];
	aref rShip;
	int j=0;
	string sNat = GetShipsTypeNationAttr(sti(shipyarder_nation));
	for (i=0;i<116;i++)//fix 116 для учета только НЕ квестовых кораблей
	{
		makearef(rShip,ShipsTypes[i].nation.(sNat));
		if (rShip==true)
		{
			storeArray[j]= i;
			j++;
		}
	}
	int j1,j2,j3,j4,j5,j6=0;
	for (i=0;i<j;i++)
		{
			j6=i;
			if (sti(ShipsTypes[storeArray[i]].Class)==6) break;//ищем начало 6 класса
		}
	iTest_ship=rand(j6-1);
	FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship2");
	iTest_ship=rand(j6-1);
	FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship3");//2 корабля 7 класса

	int iRankReq = 0;
	if (bRankRequirement) iRankReq = 4;//При опции требования ранга - корабли на верфи на 4 ранга позже

	for (i=j6+1;i<j;i++)
		{
			j5=i;
			if (sti(ShipsTypes[storeArray[i]].Class)==5) break;//ищем начало 5 класса
		}

	iTest_ship=j6+rand(j5-j6-1);
	FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship4");
	iTest_ship=j6+rand(j5-j6-1);
	FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship5");//2 корабля 6 класса

	for (i=j5+1;i<j;i++)
	{
		j4=i;
		if (sti(ShipsTypes[storeArray[i]].Class)==4) break;//ищем начало 4 класса
	}
	if (sti(PChar.rank)>=(iRankReq-7))//снижаю требования, 5 класс всегда доступен
		{
			iTest_ship=j5+rand(j4-j5-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship6");

			iTest_ship=j5+rand(j4-j5-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship7");

			iTest_ship=j5+rand(j4-j5-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship8");//3 корабля 5 класса
		}

	for (i=j4+1;i<j;i++)
		{
			j3=i;
			if (sti(ShipsTypes[storeArray[i]].Class)==3) break;//ищем начало 3 класса
		}
	if (sti(PChar.rank)>=(iRankReq-2))//снижаю требования, 4 класс со второго ранга ГГ, если повышенные требования из-за опции
		{
			iTest_ship=j4+rand(j3-j4-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship9");

			iTest_ship=j4+rand(j3-j4-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship10");

			iTest_ship=j4+rand(j3-j4-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship11");//3 корабля 4 класса
		}
	for (i=j3+1;i<j;i++)
		{
			j2=i;
			if (sti(ShipsTypes[storeArray[i]].Class)==2) break;//ищем начало 2 класса
		}
	if (sti(PChar.rank)>=(iRankReq+3))
		{
			iTest_ship=j3+rand(j2-j3-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship12");

			iTest_ship=j3+rand(j2-j3-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship13");//2 корабля 3 класса
		}
	for (i=j2+1;i<j;i++)
		{
			j1=i;
			if (sti(ShipsTypes[storeArray[i]].Class)==1) break;//ищем начало 1 класса
		}
	if (sti(PChar.rank)>=(iRankReq+9))
		{
			iTest_ship=j2+rand(j1-j2-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship14");//корабль 2 класса
		}
	if (sti(PChar.rank)>=(iRankReq+17))//на 1 ранг позже, чем через заказ на верфи
		{
			iTest_ship=j1+rand(j-j1-1);
			FillShipParamShipyard(NPChar, GenerateStoreShipExt(storeArray[iTest_ship], NPChar), "ship15");//корабль 1 класса
		}
	// LEO: ни одного корабля не сгенерилось - пустую верфь не запоминаем, чтобы при следующем визите попробовать заново
	if (CheckAttribute(NPChar, "shipyard"))
	{
		aref arYard;
		makearef(arYard, NPChar.shipyard);
		if (GetAttributesNum(arYard) == 0) DeleteAttribute(NPChar, "shipyard");
	}
}
//Lipsar учет нации на верфях<--
void FreeShipFromShipyard(ref NPChar)
{
	if (GetNpcQuestPastDayParam(NPChar, "shipyardDate") > (3 + rand(6)))
	{
        aref   arDest, arImt;
		string sAttr;
		int    iNum, i, iShip;
		makearef(arDest, NPChar.shipyard);
		iNum = GetAttributesNum(arDest);
		for (i = 0; i < iNum; i++)
		{
	    	arImt = GetAttributeN(arDest, i);
		iShip = sti(arImt.Type);
		if (iShip >= 0 && iShip < REAL_SHIPS_QUANTITY) DeleteAttribute(&RealShips[iShip], "StoreShip"); // можно тереть
	    }
        DeleteAttribute(NPChar, "shipyard");
	}
}

void RemoveCannonsFromBortShipyard(ref chr, string sBort)
{
	int     maxQty = GetBortCannonsBaseQty(chr, sBort);
	int     i;
	string  attr;
	string  sBort_real = sBort;

	if(sBort == "rcannon") sBort_real = "cannonr";
	if(sBort == "lcannon") sBort_real = "cannonl";
	if(sBort == "fcannon") sBort_real = "cannonf";
	if(sBort == "bcannon") sBort_real = "cannonb";

	// нулим кол-во пушек на борту
	for (i = 0; i < maxQty; i++)
	{
		attr = "c" + i;
		// поломана на 100 процентов, не палит, те нет её
		chr.Ship.Cannons.borts.(sBort).damages.(attr) = 1.0;
		chr.Ship.Cannons.borts.(sBort_real).damages.(attr) = 1.0;
	}

	RecalculateCargoLoad(chr);  // пересчет, тк пушки снялись
}

void RemoveAllCannonsShipyardShip(ref NPChar)
{
    RemoveCannonsFromBortShipyard(NPChar, "fcannon");
    RemoveCannonsFromBortShipyard(NPChar, "bcannon");
    RemoveCannonsFromBortShipyard(NPChar, "rcannon");
    RemoveCannonsFromBortShipyard(NPChar, "lcannon");
}

void FillShipParamShipyard(ref NPChar, int _iType, string _sShipNum)
{
    aref    arTo, arFrom;

	// LEO: слотов RealShips не хватило - не пишем в верфь запись с типом 1000. Раньше она висела там до
	//   очистки верфи (3-9 дней + выход в море), таблица верфи была пустой, а FreeShipFromShipyard лез в RealShips[1000].
	if (_iType < 0 || _iType >= REAL_SHIPS_QUANTITY)
	{
		trace("FillShipParamShipyard: no ship for " + NPChar.id + " " + _sShipNum);
		return;
	}

	DeleteAttribute(NPChar, "ship");
    NPChar.Ship.Type = _iType;

	SetRandomNameToShip(NPChar);

    SetBaseShipData(NPChar);
    SetCrewQuantity(NPChar, 0);
    if (sti(NPChar.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
    {
    	NPChar.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS8; // to_do переделать на продажу без орудий вообще
    }
	RemoveAllCannonsShipyardShip(NPChar);	//нулим пушки .. сами покупайте какие надо
    DeleteAttribute(NPChar, "Ship.Cargo");  //пустой трюм
    SetGoodsInitNull(NPChar);
    RecalculateCargoLoad(NPChar);

    FillShipyardShipBack(NPChar, _sShipNum);
}

void FillShipyardShip(ref NPChar, string _sShipNum)
{
    aref    arTo, arFrom;

	DeleteAttribute(NPChar, "ship");

    makearef(arFrom,   NPChar.shipyard.(_sShipNum));
    NPChar.Ship = "";
	makearef(arTo, NPChar.Ship);
	CopyAttributes(arTo, arFrom);
}

void FillShipyardShipBack(ref NPChar, string _sShipNum)
{
    aref    arTo, arFrom;

  	DeleteAttribute(NPChar, "shipyard." + _sShipNum);
    NPChar.shipyard.(_sShipNum) = "";
    makearef(arTo,   NPChar.shipyard.(_sShipNum));
	makearef(arFrom, NPChar.Ship);
	CopyAttributes(arTo, arFrom);
}

int GetSailsTuningPrice(ref _chr, ref _shipyard, float _rate)
{
	int st  = GetCharacterShipType(_chr);
	ref shref;

	if (st==SHIP_NOTUSED) return 0;
	shref = GetRealShip(st);

	return makeint(stf(_shipyard.ShipCostRate) * sti(shref.Price) * _rate / 10) * 10; // кратно 10
}

// Warship 26.07.09 Вынес сюда
// int st - GetCharacterShipType(_character)
// ref _shipyard - референс на верфиста
int GetShipPriceByType(int st, ref _shipyard)
{
	if(st==SHIP_NOTUSED) return 0;
	ref shref = GetRealShip(st);

	return makeint(stf(_shipyard.ShipCostRate) * sti(shref.Price));
}

// Метод вернёт цену продажи корабля персонажа с учетом всех скилов (для верфи)
int GetShipSellPrice(ref _chr, ref _shipyard)
{
	int st = GetCharacterShipType(_chr);
	int price = GetShipPriceByType(st, _shipyard);
	price = makeint(price - 1.5*GetSailRepairCost(st, GetSailDamagePercent(_chr), _shipyard));
	price = makeint(price - 1.5*GetHullRepairCost(st, GetHullDamagePercent(_chr), _shipyard));

	float nCommerce   = GetSummonSkillFromNameToOld(GetMainCharacter(), SKILL_COMMERCE) + 0.001;

	if(CheckOfficersPerk(pchar,"Trader")) { nCommerce += 2; }
	if(CheckOfficersPerk(pchar,"AdvancedCommerce"))	{ nCommerce += 4; }
	else
	{
		if(CheckOfficersPerk(pchar,"BasicCommerce"))	{ nCommerce += 2; }
	}

	price = price - price / (nCommerce*10.5);

	ref rRealShip = GetRealShip(st);

	if (sti(rRealShip.Stolen) == true) //проверка на ворованный
	{

		int iRep = sti(pchar.reputation);
		int iRepMultiplier = makeint((iRep - REPUTATION_NEUTRAL) / 250);
		int iDifficultyMultiplier = makeint(MOD_SKILL_ENEMY_RATE * 3 / 100);
		price = makeint(price / 10) + iRepMultiplier * price - iDifficultyMultiplier * price; // LEO: ворованный задёшево (10%) - быстрый сброс хлама на верфи; нормальную цену даёт аукцион в ПУ
    }
    if (price < 0 && sti(_chr.Ship.Type) != SHIP_NOTUSED)
	{
	   price = 0;
	}

	return price;
}

// Метод вернёт цену покупки корабля персонажа с учетом всех скилов (для верфи)
int GetShipBuyPrice(int iType, ref _shipyard)
{
	// boal учет скилов торговли 22.01.2004 -->
	float nCommerce   = GetSummonSkillFromNameToOld(GetMainCharacter(), SKILL_COMMERCE);

	if(CheckOfficersPerk(pchar,"Trader")) { nCommerce += 2; }
    if(CheckOfficersPerk(pchar,"AdvancedCommerce"))	{ nCommerce += 4; }
	else
	{
		if(CheckOfficersPerk(pchar,"BasicCommerce"))	{ nCommerce += 2; }
	}

    return makeint(GetShipPriceByType(iType, _shipyard) + GetShipPriceByType(iType, _shipyard)/(nCommerce*10));
    // boal 22.01.2004 <--
}

int GetSailRepairCost(int shipType, int repairPercent, ref _shipyard)
{
	float fConstanta = 2.0;
	float fSkillDepend = fConstanta + (SKILL_TO_OLD*2.0) - (GetSummonSkillFromNameToOld(pchar, SKILL_REPAIR) + GetSummonSkillFromNameToOld(pchar, SKILL_COMMERCE));
	fSkillDepend *= 0.1 + (MOD_SKILL_ENEMY_RATE*3 * 0.01);
	float SailRepairCoeff = 1.0 * fSkillDepend;
	int shipPrice = GetShipPriceByType(shipType, _shipyard);
	if(shipPrice<=0) return 0;
	ref shref = GetRealShip(shipType);
	float scost = SailRepairCoeff * (shipPrice*SAIL_COST_PERCENT+99.0)/100.0;
	if (CheckAttribute(shref, "HullArmor") && shref.HullArmor == "S") scost *= 3; // СФ
	return makeint((scost*repairPercent+99.0)/100.0);
}

int GetHullRepairCost(int shipType,int repairPercent, ref _shipyard)
{
	float fConstanta = 4.0;
	float fSkillDepend = fConstanta + (SKILL_TO_OLD*1.75) - (GetSummonSkillFromNameToOld(pchar, SKILL_REPAIR) + GetSummonSkillFromNameToOld(pchar, SKILL_COMMERCE));
	fSkillDepend *= 0.1 + (MOD_SKILL_ENEMY_RATE*3 * 0.01);
	float HullRepairCoeff = 1.0 * fSkillDepend;
	int shipPrice = GetShipPriceByType(shipType, _shipyard);
	if(shipPrice<=0) return 0;
	ref shref = GetRealShip(shipType);
	float scost = HullRepairCoeff * (shipPrice*HULL_COST_PERCENT+99.0)/100.0;
	if (CheckAttribute(shref, "HullArmor") && shref.HullArmor == "S") scost *= 3; // СФ
	return makeint((scost*repairPercent+99.0)/100.0);
}

int GetSailDamagePercent(ref _chr)
{
	return 100 - MakeInt(GetSailPercent(_chr));
}

int GetHullDamagePercent(ref _chr)
{
	return 100 - MakeInt(GetHullPercent(_chr));
}
///////////////////////////////////////////////////////////////   ВЕРФИ  <--
// палуба от типа корабля
string GetShip_deck(ref chr, bool map2sea)
{
	string ret, add;
	int    iType = sti(chr.Ship.Type);
	if (map2sea)
	{
	    ret = "Ship_deck_";
	}
	else
	{
	    ret = "Deck_Near_Ship_";
	}
	add = "Low"; // если нет типа
	if (iType != SHIP_NOTUSED)
	{
        ref rRealShip = GetRealShip(iType);

		add = rRealShip.DeckType;
	}

	return ret + add;
}

void MakeCloneShipDeck(ref chr, bool map2sea)
{
	ref rOrg, rClone;
	int iOrg, iClone;
    string locId, toLocId;

    locId = GetShip_deck(chr, map2sea);
    if (map2sea)
	{
	    toLocId = "Ship_deck";
	}
	else
	{
	    toLocId = "Deck_Near_Ship";
	}

	iOrg = FindLocation(locId);
	iClone = FindLocation(toLocId);

	if(iOrg < 0 || iClone < 0) {
        Trace("MakeCloneShipDeck error: iOrg " + iOrg + ", iClone " + iClone);
        return;
	}

	makeref(rOrg, Locations[iOrg]);
	makeref(rClone, Locations[iClone]);

	DeleteAttribute(rClone, "");
	CopyAttributes(rClone, rOrg);
	rClone.id = toLocId;
	rClone.index = iClone;
}

void MakeCloneFortBoarding(string fromLocId)
{
	ref rOrg, rClone;
	int iOrg, iClone;
    string toLocId;

    toLocId = "BOARDING_FORT";

	iOrg = FindLocation(fromLocId);
	iClone = FindLocation(toLocId);

	if(iOrg < 0 || iClone < 0) {
        Trace("MakeCloneFortBoarding error: iOrg " + iOrg + ", iClone " + iClone);
        return;
	}

	makeref(rOrg, Locations[iOrg]);
	makeref(rClone, Locations[iClone]);

	DeleteAttribute(rClone, "");
	CopyAttributes(rClone, rOrg);
	rClone.id = toLocId;
	rClone.index = iClone;

    rClone.type = "fort_attack";
	LAi_LocationFantomsGen(rClone, false);
	DeleteAttribute(rClone, "reload");
	//это подкручивание другого файла локаторов. там нет goto, soldiers и пр. есть rld и дополн.сундуки
	if (rClone.models.always.locators == "fortV_locators")
	{
		rClone.models.always.locators = "fortV_lAttack";
		rClone.models.always.fortV = "fortV_attack";
	}
	else
	{
		rClone.models.always.locators = "fortVRight_lAttack";
		rClone.models.always.fortV = "fortVRight_attack";
	}
	rClone.boarding.locatorNum = 35;
	rClone.boarding = "false";
	rClone.boarding.nextdeck = "Boarding_fortyard"; // Jason
	rClone.image = "loading\jonny_load\outside\FortVRight.dds";
}

int GetPortManPrice(int Price, ref NPChar)
{
    if (!CheckAttribute(NPChar, "PortManPrice"))
	{
		NPChar.PortManPrice = (0.06 + frnd()*0.1);
	}
	return makeint(Price * stf(NPChar.PortManPrice) /10 ) * 10;
}

// ugeen --> альтернативный расчет стоянки корабля в ПУ
int GetPortManPriceExt(ref NPChar, ref chref)
{
	float fLeadership 	= 1.8 - GetSummonSkillFromName(pchar, SKILL_LEADERSHIP)/120.0; 	// учитываем авторитет
	float fCommerce 	= 1.8 - GetSummonSkillFromName(pchar, SKILL_COMMERCE)/120.0; 	// учитываем торговлю
	float fRelation 	= 1.0; 															// учитываем  - родная нация или нет

	if(sti(NPChar.nation) == GetBaseHeroNation()) fRelation = 0.5; 						// если нация родная- снижаем цену в два раза
	int price = makeint(100 * MOD_SKILL_ENEMY_RATE*3 * 1.56 * sqr(6.7 - sti(RealShips[sti(chref.Ship.Type)].Class)) * (2 + 0.5 * GetNationRelation2MainCharacter(sti(NPChar.nation))) * fRelation * fLeadership * fCommerce);
	price = price + sti(chref.Ship.Crew.Quantity) * 10; // LEO: за каждого матроса платим по 10 золота

	return price;
}
//<-- альтернативный расчет стоянки корабля в ПУ

// Стоянка кораблей в ПУ: транзакции, общие для диалога Common_Portman и интерфейса portstock -->
int PortStock_GetStorePrice(ref rPortman, ref chref) // цена месяца стоянки, 0 - бесплатная стоянка на Каймане
{
	if (pchar.location == "Caiman_PortOffice") return 0;
	return GetPortManPriceExt(rPortman, chref);
}

int PortStock_GetReturnDebt(ref chref) // долг за хранение при возврате
{
	if (CheckAttribute(chref, "ShipInStockMan.Unlimited")) return 0; // бессрочная стоянка уже оплачена
	return GetNpcQuestPastMonthParam(chref, "ShipInStockMan.Date") * sti(chref.ShipInStockMan.MoneyForShip);
}

void PortStock_RefreshSkills(ref _compref) // аналог DelBakSkill из диалоговых файлов
{
	DelBakSkillAttr(pchar);
	ClearCharacterExpRate(pchar);
	RefreshCharacterSkillExpRate(pchar);
	SetEnergyToCharacter(pchar);

	DelBakSkillAttr(_compref);
	ClearCharacterExpRate(_compref);
	RefreshCharacterSkillExpRate(_compref);
	SetEnergyToCharacter(_compref);
}

// сдать корабль компаньона на стоянку; iCompanionCharIdx - индекс в Characters
bool PortStock_StoreShip(ref rPortman, int iCompanionCharIdx, bool bWithOfficer, bool bUnlimited)
{
	ref chref, compref;
	aref arTo, arFrom;

	compref = GetCharacter(iCompanionCharIdx);
	if (SeaTowOperations_Block(compref)) return false;
	if (AICapturedTrap_IsShipInfected(compref)) return false; // LEO: страховка - заражённый корабль на стоянку не принимают (лазейка от мора)
	int iPrice = PortStock_GetStorePrice(rPortman, compref);
	int iPayment = iPrice;
	if (bUnlimited) iPayment = iPrice * 6; // бессрочно - шестикратная цена разово
	if (sti(pchar.Money) < iPayment) return false;
	AddMoneyToCharacter(pchar, -iPayment);

	Wounded_ClearToInfirmary(compref); // LEO: раненых нельзя оставить на сдаваемом в ПУ корабле - в городской лазарет (+подсказка в лог)

	if (bWithOfficer)
	{
		chref = compref;
		chref.ShipInStockMan = rPortman.id;
		chref.ShipInStockMan.MoneyForShip = iPrice;
		chref.ShipInStockMan.AltDate = GetQuestBookDataDigit(); // для печати
		SaveCurrentNpcQuestDateParam(chref, "ShipInStockMan.Date"); // для расчёта
		RemoveCharacterCompanion(pchar, chref);
	}
	else
	{
		chref = GetCharacter(NPC_GenerateCharacter("ShipInStockMan", "PKM_rab_"+(rand(3)+1), "man", "man", rand(5)+1, rPortman.nation, -1, false));
		chref.id = chref.id + "_" + chref.index; // меняем ID на оригинальный
		chref.reputation = 10 + rand(80);
		chref.lastname = chref.name;
		if (LanguageGetLanguage() == "russian") chref.name = "сторож";
		else chref.name = "the watchman";
		DeleteAttribute(chref, "ship");
		chref.ship = "";

		chref.ShipInStockMan = rPortman.id;
		chref.ShipInStockMan.MoneyForShip = iPrice;
		chref.ShipInStockMan.AltDate = GetQuestBookDataDigit(); // для печати
		SaveCurrentNpcQuestDateParam(chref, "ShipInStockMan.Date"); // для расчёта

		makearef(arTo, chref.ship);
		makearef(arFrom, compref.Ship);
		CopyAttributes(arTo, arFrom);

		compref.ship.type = SHIP_NOTUSED;
		RemoveCharacterCompanion(pchar, compref);
		AddPassenger(pchar, compref, false);
		PortStock_RefreshSkills(compref);
	}

	if (bUnlimited) chref.ShipInStockMan.Unlimited = true; // плата при возврате не берётся

	chref.location = "";
	chref.location.group = "";
	chref.location.locator = "";
	if (!CheckAttribute(rPortman, "Portman")) rPortman.Portman = 0;
	rPortman.Portman = sti(rPortman.Portman) + 1;
	if (!CheckAttribute(pchar, "ShipInStock")) pchar.ShipInStock = 0;
	pchar.ShipInStock = sti(pchar.ShipInStock) + 1;
	return true;
}

// забрать корабль со стоянки; iStockCharIdx - индекс хранителя (офицер или сторож) в Characters,
// iNewCaptainCharIdx - индекс пассажира-капитана (для корабля со сторожем), для офицерского корабля игнорируется
bool PortStock_ReturnShip(ref rPortman, int iStockCharIdx, int iNewCaptainCharIdx)
{
	ref chref, compref;
	aref arTo, arFrom;

	if (GetCompanionQuantity(pchar) >= COMPANION_MAX) return false;
	chref = GetCharacter(iStockCharIdx);
	int iDebt = PortStock_GetReturnDebt(chref);
	if (sti(pchar.Money) < iDebt) return false;

	bool bWatchman = HasSubStr(chref.id, "ShipInStockMan_");
	if (bWatchman && iNewCaptainCharIdx < 0) return false; // кораблю со сторожем нужен новый капитан

	AddMoneyToCharacter(pchar, -iDebt);
	DownCrewExp(chref, GetNpcQuestPastDayParam(chref, "ShipInStockMan.Date"));

	if (bWatchman)
	{
		compref = GetCharacter(iNewCaptainCharIdx);
		DeleteAttribute(compref, "ship");
		compref.ship = "";

		makearef(arTo, compref.ship);
		makearef(arFrom, chref.Ship);
		CopyAttributes(arTo, arFrom);

		CheckForReleaseOfficer(iNewCaptainCharIdx); // увольнение с должности, если был не просто пассажиром
		RemovePassenger(pchar, compref);
		SetCompanionIndex(pchar, -1, iNewCaptainCharIdx);
		PortStock_RefreshSkills(compref);

		// чистим сторожа, чтобы слот массива снова можно было занять
		DeleteAttribute(chref, "ShipInStockMan");
		chref.id = "ShipInStockMan";
		DeleteAttribute(chref, "ship");
		chref.ship = "";
		LAi_SetCurHP(chref, 0.0);
	}
	else
	{
		DeleteAttribute(chref, "ShipInStockMan");
		SetCompanionIndex(pchar, -1, iStockCharIdx);
	}

	rPortman.Portman = sti(rPortman.Portman) - 1;
	pchar.ShipInStock = sti(pchar.ShipInStock) - 1;
	return true;
}
//<-- стоянка кораблей в ПУ

//--> LEO: АУКЦИОН кораблей в ПУ (продажа за 30-50% с задержкой по классу; деньги копятся у портмана колонии)
// честная цена корабля по СОСТОЯНИЮ (как на верфи), но БЕЗ скидки за ворованность - база аукциона
int PortAuction_GetShipValue(ref chref, ref rPortman)
{
	if (!CheckAttribute(chref, "Ship.Type")) return 0;
	int st = sti(chref.Ship.Type);
	if (st == SHIP_NOTUSED) return 0;
	ref rRealShip = GetRealShip(st);
	int iWasStolen = sti(rRealShip.Stolen);
	rRealShip.Stolen = 0;                          // временно честная цена (без 10%-штрафа за ворованность)
	// портман не верфист - подставляем нейтральный ShipCostRate (у верфистов 0.8-1.2), иначе цена выйдет 0
	bool bHadRate = CheckAttribute(rPortman, "ShipCostRate");
	float fOldRate = 0.0;
	if (bHadRate) fOldRate = stf(rPortman.ShipCostRate);
	rPortman.ShipCostRate = 1.0;
	int price = GetShipSellPrice(chref, rPortman);
	if (bHadRate) rPortman.ShipCostRate = fOldRate;
	else DeleteAttribute(rPortman, "ShipCostRate");
	rRealShip.Stolen = iWasStolen;
	if (price < 1) price = 1;
	return price;
}

// срок аукциона по классу (dRand) с сокращением чистой удачей (-4%/очко, макс -40% при LUCK 10)
int PortAuction_RollDays(int shipClass, int iSlot)
{
	int dMin, dMax;
	if (shipClass >= 5) { dMin = 7;  dMax = 14; }         // 6-5 класс: неделя-две
	else
	{
		if (shipClass >= 3) { dMin = 14; dMax = 30; }    // 4-3 класс: две недели - месяц
		else                { dMin = 30; dMax = 90; }    // 2-1 класс: месяц - три
	}
	int days = dMin + (dRand(9999) + iSlot * 131) % (dMax - dMin + 1);
	int fort = GetCharacterSkillNoBonus(pchar, SKILL_FORTUNE); // навык ВЕЗЕНИЕ (чистый/белый, 1-100)
	if (fort > 100) fort = 100;
	days = days - days * fort / 250;            // сокращение срока: макс -40% при Везении 100
	if (days < 3) days = 3;
	return days;
}

// (пере)ролл лота: целевая цена 30-50% от базы + срок (при провале лот запускается заново)
void PortAuction_RollLot(ref keeper)
{
	int iBase = sti(keeper.ShipAtAuction.Base);
	int iPct = 30 + (dRand1(9999) + sti(keeper.index) * 97) % 21; // 30..50
	int iTarget = iBase * iPct / 100;
	if (iTarget < 1) iTarget = 1;
	keeper.ShipAtAuction.Target = iTarget;
	keeper.ShipAtAuction.Days   = PortAuction_RollDays(sti(keeper.ShipAtAuction.Class), sti(keeper.index));
}

// LEO: перенос команды с выставляемого корабля - компактная версия логики верфи (SendCrewTo* в shipyard.c интерфейс-локальны, отсюда недоступны).
// добивает приёмник до его предела (МАКС либо ОПТ при bOpt), взвешенно усредняя опыт/мораль, возвращает нераспределённый остаток.
int PortAuction_PourCrew(ref chrefFrom, ref chrTo, int crewnum, bool bOpt)
{
	int cap = GetMaxCrewQuantity(chrTo);
	if (bOpt) cap = GetOptCrewQuantity(chrTo);
	// LEO: вычитаем РАНЕНЫХ (занимают места) - иначе команда льётся поверх буфера раненых и актив+раненые > Max (перебафф).
	int freeSpace = cap - GetCrewQuantity(chrTo) - GetWoundedQuantity(chrTo);
	if (freeSpace <= 0) return crewnum;
	int take = crewnum;
	if (take > freeSpace) take = freeSpace;
	float fDen = makefloat(GetCrewQuantity(chrTo) + take);
	if (fDen > 0.0)
	{
		chrTo.Ship.Crew.Exp.Sailors   = (stf(chrTo.Ship.Crew.Exp.Sailors)  *GetCrewQuantity(chrTo) + stf(chrefFrom.Ship.Crew.Exp.Sailors)  *take) / fDen;
		chrTo.Ship.Crew.Exp.Cannoners = (stf(chrTo.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(chrTo) + stf(chrefFrom.Ship.Crew.Exp.Cannoners)*take) / fDen;
		chrTo.Ship.Crew.Exp.Soldiers  = (stf(chrTo.Ship.Crew.Exp.Soldiers) *GetCrewQuantity(chrTo) + stf(chrefFrom.Ship.Crew.Exp.Soldiers) *take) / fDen;
		chrTo.Ship.Crew.morale        = (stf(chrTo.Ship.Crew.morale)       *GetCrewQuantity(chrTo) + stf(chrefFrom.Ship.Crew.morale)       *take) / fDen;
	}
	SetCrewQuantity(chrTo, GetCrewQuantity(chrTo) + take);
	return crewnum - take;
}

// LEO: остаток команды - в гарнизон/таверну города (взвешенно), как SendCrewToTavern на верфи
void PortAuction_CrewToTavern(ref chrefFrom, int crewn)
{
	if (crewn <= 0) return;
	ref refTown = GetColonyByIndex(FindColony(loadedLocation.fastreload));
	if (!CheckAttribute(refTown, "index")) return;
	float fDen = makefloat(GetCrewQuantity(refTown) + crewn);
	if (fDen > 0.0)
	{
		refTown.Ship.Crew.Exp.Sailors   = (stf(refTown.Ship.Crew.Exp.Sailors)  *GetCrewQuantity(refTown) + stf(chrefFrom.Ship.Crew.Exp.Sailors)  *crewn) / fDen;
		refTown.Ship.Crew.Exp.Cannoners = (stf(refTown.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refTown) + stf(chrefFrom.Ship.Crew.Exp.Cannoners)*crewn) / fDen;
		refTown.Ship.Crew.Exp.Soldiers  = (stf(refTown.Ship.Crew.Exp.Soldiers) *GetCrewQuantity(refTown) + stf(chrefFrom.Ship.Crew.Exp.Soldiers) *crewn) / fDen;
		refTown.Ship.Crew.Morale        = (stf(refTown.Ship.Crew.Morale)       *GetCrewQuantity(refTown) + stf(chrefFrom.Ship.Crew.Morale)       *crewn) / fDen;
	}
	refTown.Ship.Crew.Quantity = GetCrewQuantity(refTown) + crewn;
}

// LEO: распределить команду продаваемого корабля по выбранному режиму (как радиокнопки верфи):
// "Max" - ГГ и компаньоны до МАКСИМУМА; "Opt" - до ОПТИМУМА; "Fire" - всех сразу в таверну. Остаток всегда в таверну.
void PortAuction_DistributeCrew(ref chref, string sMode)
{
	Wounded_ClearToInfirmary(chref); // LEO: раненых - в лазарет (+подсказка)
	int crew = GetCrewQuantity(chref);
	if (crew <= 0) return;
	if (sMode == "Fire")
	{
		PortAuction_CrewToTavern(chref, crew);
		return;
	}
	bool bOpt = (sMode == "Opt");
	crew = PortAuction_PourCrew(chref, pchar, crew, bOpt);
	int i, cn;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		if (crew <= 0) break;
		cn = GetCompanionIndex(pchar, i);
		if (cn <= 0) continue;
		ref comp = GetCharacter(cn);
		if (sti(comp.index) == sti(chref.index)) continue; // не сам выставляемый корабль
		crew = PortAuction_PourCrew(chref, comp, crew, bOpt);
	}
	if (crew > 0) PortAuction_CrewToTavern(chref, crew);
}

// LEO: Как на верфи, отсутствие ствола хранится как damage=1 в обоих написаниях борта.
// LEO: Берём реальные порты корабля, а не базовую модель: улучшенные порты тоже должны опустеть.
void PortAuction_RemoveSoldCannons(ref chref)
{
	string sBort, sReal, attr;
	for (int b = 0; b < 4; b++)
	{
		switch (b)
		{
			case 0: sBort = "fcannon"; sReal = "cannonf"; break;
			case 1: sBort = "bcannon"; sReal = "cannonb"; break;
			case 2: sBort = "lcannon"; sReal = "cannonl"; break;
			case 3: sBort = "rcannon"; sReal = "cannonr"; break;
		}
		int maxQty = GetBortCannonsMaxQty(chref, sBort);
		for (int i = 0; i < maxQty; i++)
		{
			attr = "c" + i;
			chref.Ship.Cannons.Borts.(sBort).damages.(attr) = 1.0;
			chref.Ship.Cannons.Borts.(sReal).damages.(attr) = 1.0;
		}
	}
	RecalculateCargoLoad(chref);
}

// LEO: снять с выставляемого корабля активы (как продажа на верфи): пушки -> проданы в лавку пушечным товаром,
// груз -> продан в лавку (контрабанда уходит ГГ), команда -> по режиму sCrewMode (Max/Opt/Fire).
void PortAuction_RecoverShipAssets(ref chref, string sCrewMode)
{
	ref Colony = GetColonyByIndex(FindColony(loadedLocation.fastreload));
	bool bStore = CheckAttribute(Colony, "StoreNum");
	ref trader;
	if (bStore) trader = &stores[sti(Colony.StoreNum)];
	int moneyback = 0;

	// --- ПУШКИ: снимаются и продаются как пушечный товар (на верфи их теряют, тут возвращаем стоимость) ---
	int iCannons = GetCannonsNum(chref);
	if (iCannons > 0 && bStore)
	{
		int cidx = GetCannonGoodsIdxByType(sti(chref.Ship.Cannons.Type));
		if (cidx != -1)
		{
			int cUnits    = Goods[cidx].Units;
			int cStoreQty = GetStoreGoodsQuantity(trader, cidx);
			int cPrice    = GetStoreGoodsPrice(trader, cidx, PRICE_TYPE_SELL, pchar, 1);
			PortAuction_RemoveSoldCannons(chref); // LEO: Сначала списать стволы, затем товар и деньги; копия лота уже без них.
			SetStoreGoods(trader, cidx, cStoreQty + iCannons);
			moneyback += makeint(cPrice * iCannons / cUnits + 0.5);
		}
	}

	// --- ГРУЗ: продаётся в лавку колонии, контрабанда уходит ГГ (как в DoSellShip) ---
	if (bStore)
	{
		aref refGoods;
		int iGoodIndex;
		for (iGoodIndex = 0; iGoodIndex < GOODS_QUANTITY; iGoodIndex++)
		{
			int ShipQ = GetCargoGoods(chref, iGoodIndex);
			if (ShipQ <= 0) continue;
			string goodname = Goods[iGoodIndex].Name;
			makearef(refGoods, trader.Goods.(goodname));
			if (sti(refGoods.TradeType) == TRADE_TYPE_CONTRABAND)
			{
				AddCharacterGoods(pchar, iGoodIndex, ShipQ);
				RemoveCharacterGoods(chref, iGoodIndex, ShipQ);
				continue;
			}
			int iUnits      = Goods[iGoodIndex].Units;
			int iStoreQty   = GetStoreGoodsQuantity(trader, iGoodIndex);
			int iStorePrice = GetStoreGoodsPrice(trader, iGoodIndex, PRICE_TYPE_SELL, pchar, 1);
			RemoveCharacterGoods(chref, iGoodIndex, ShipQ);
			SetStoreGoods(trader, iGoodIndex, iStoreQty + ShipQ);
			moneyback += makeint(iStorePrice * ShipQ / iUnits + 0.5);
		}
	}

	if (moneyback > 0)
	{
		AddMoneyToCharacter(pchar, moneyback);
		Statistic_AddValue(pchar, "Money_get", moneyback);
		AddCharacterExpToSkill(pchar, "Commerce", moneyback / 2600.0);
		Log_Info(XI_ConvertString("PortAuction_Recovered") + " " + moneyback);
	}

	// --- КОМАНДА: по выбранному в окне режиму (макс/опт по кораблям либо всех в таверну) ---
	PortAuction_DistributeCrew(chref, sCrewMode);
}

// выставить корабль компаньона на аукцион: корабль уходит из эскадры на держателя-аукциониста, офицер - в пассажиры
// sCrewMode - выбор в окне подтверждения: "Max"/"Opt"/"Fire" (как радиокнопки верфи)
bool PortAuction_SubmitShip(ref rPortman, int iCompanionCharIdx, string sCrewMode)
{
	if (iCompanionCharIdx < 0 || iCompanionCharIdx >= TOTAL_CHARACTERS) return false;
	// LEO: Повторно проверяем владельца при подтверждении, не доверяя устаревшей строке интерфейса.
	if (GetCompanionNumber(pchar, iCompanionCharIdx) <= 0) return false;
	ref compref = GetCharacter(iCompanionCharIdx);
	if (SeaTowOperations_Block(compref)) return false;
	if (!GetRemovable(compref)) return false;
	if (PGG_IsCompanionOwnShip(compref)) return false;
	if (!CheckAttribute(compref, "Ship.Type")) return false;
	if (sti(compref.Ship.Type) == SHIP_NOTUSED) return false;
	if (AICapturedTrap_IsShipInfected(compref)) return false; // LEO: заражённый мором корабль на аукцион не выставить (лазейка)

	// LEO: Резервируем держателя ДО продажи активов: при нехватке персонажей корабль и деньги не меняются.
	int iKeeper = NPC_GenerateCharacter("ShipAtAuction", "PKM_rab_"+(rand(3)+1), "man", "man", rand(5)+1, rPortman.nation, -1, false);
	if (iKeeper < 0) return false;
	ref keeper = GetCharacter(iKeeper);

	// LEO: снять с корабля пушки/груз/команду (как при продаже на верфи) ДО копирования на держателя - иначе активы пропадут
	PortAuction_RecoverShipAssets(compref, sCrewMode);

	int iBase = PortAuction_GetShipValue(compref, rPortman);
	int iShipClass = GetCharacterShipClass(compref);

	keeper.id = keeper.id + "_" + keeper.index; // уникальный ID (слот переиспользуется после продажи)
	keeper.lastname = keeper.name;
	if (LanguageGetLanguage() == "russian") keeper.name = "распорядитель торгов";
	else keeper.name = "the auctioneer";
	DeleteAttribute(keeper, "ship");
	keeper.ship = "";

	aref arTo, arFrom;
	makearef(arTo, keeper.ship);
	makearef(arFrom, compref.Ship);
	CopyAttributes(arTo, arFrom);

	keeper.ShipAtAuction          = rPortman.id;   // у какого портмана выставлено (туда же выручка)
	keeper.ShipAtAuction.Base     = iBase;
	keeper.ShipAtAuction.Class    = iShipClass;
	keeper.ShipAtAuction.ShipName = compref.Ship.Name;
	PortAuction_RollLot(keeper);                    // первичный ролл цены и срока

	keeper.location = ""; keeper.location.group = ""; keeper.location.locator = "";

	compref.ship.type = SHIP_NOTUSED;
	RemoveCharacterCompanion(pchar, compref);
	AddPassenger(pchar, compref, false);
	ReturnAutoOfficers(); // LEO: ex-капитан занимал должности до капитанства - вернуть его на них (как при продаже на верфи)
	PortStock_RefreshSkills(compref);
	return true;
}

// суточная обработка аукционов (зов из QuestActions): отсчёт срока, по истечении - ролл успеха (удача влияет)
void PortAuction_Daily()
{
	int i;
	for (i = 1; i < MAX_CHARACTERS; i++)
	{
		ref c = &Characters[i];
		if (!CheckAttribute(c, "ShipAtAuction")) continue;
		if (!CheckAttribute(c, "ShipAtAuction.Days")) continue;
		int d = sti(c.ShipAtAuction.Days) - 1;
		if (d > 0) { c.ShipAtAuction.Days = d; continue; }

		// срок вышел - продаём с шансом (база 60% + УДАЧА (SPECIAL), макс 95%)
		int luck = makeint(GetCharacterSPECIALSimple(pchar, SPECIAL_L)); // характеристика Удача (1-10)
		int chance = 60 + luck * 4;               // +до 40% за Удачу 10 (с потолком 95)
		if (chance > 95) chance = 95;
		if (((dRand2(9999) + i * 7) % 100) < chance)
		{
			ref rPortman = CharacterFromID(c.ShipAtAuction);
			int iGain = sti(c.ShipAtAuction.Target);
			string sShipName = c.ShipAtAuction.ShipName;
			string sPlace = "";
			if (CheckAttribute(rPortman, "id"))
			{
				if (!CheckAttribute(rPortman, "AuctionVault")) rPortman.AuctionVault = 0;
				rPortman.AuctionVault = sti(rPortman.AuctionVault) + iGain;
				if (CheckAttribute(rPortman, "City") && rPortman.City != "")
				{
					string sCity = rPortman.City;
					string sIsland = GetIslandByCityName(sCity);
					sPlace = XI_ConvertString("Colony" + sCity);
					if (sPlace != "")
					{
						sPlace = ": " + sPlace;
						if (sIsland != "" && sIsland != "Mein" && sIsland != sCity)
						{
							string sIslandName = XI_ConvertString(sIsland);
							if (sIslandName != "") sPlace = sPlace + ", " + sIslandName;
						}
					}
				}
			}
			Log_InfoColored("green", XI_ConvertString("PortAuction_Sold") + " '" + sShipName + "' (" + iGain + ")" + sPlace); // LEO: продажа корабля на торгах - зелёным (деньги получены)
			// LEO: в «Кораблей продано» шла ТОЛЬКО верфь (shipyard.c), и всё ушедшее через ПУ пропадало
			//   из статистики. Считаем в момент РЕАЛЬНОЙ продажи лота: выставление можно отозвать,
			//   а неудачные торги перезапускаются (PortAuction_RollLot ниже).
			Statistic_AddValue(pchar, "SellShip", 1);
			PlayStereoSound("interface\bort_jurnal.wav"); // LEO: звуковое оповещение о продаже на аукционе
			// чистим держатель + освобождаем слот массива
			DeleteAttribute(c, "ShipAtAuction");
			c.id = "ShipAtAuction";
			DeleteAttribute(c, "ship");
			c.ship = "";
			LAi_SetCurHP(c, 0.0);
		}
		else
		{
			PortAuction_RollLot(c); // провал - аукцион запускается заново (новая цена и срок)
		}
	}
}

// забрать накопленную выручку у портмана колонии
int PortAuction_WithdrawMoney(ref rPortman)
{
	if (!CheckAttribute(rPortman, "AuctionVault")) return 0;
	int iSum = sti(rPortman.AuctionVault);
	if (iSum > 0)
	{
		AddMoneyToCharacter(pchar, iSum);
		Statistic_AddValue(pchar, "Money_get", iSum); // выручка с торгов тоже деньги - в денежную статистику
	}
	rPortman.AuctionVault = 0;
	return iSum;
}

// LEO: ВЕРНУТЬ корабль с торгов в эскадру (передумал продавать), пока не продан. Бесплатно.
// Корабль был стрипнут при выставлении (пушки/груз/команда сняты как при продаже) - возвращается голый корпус,
// которому нужен новый капитан из пассажиров (ex-капитан стал пассажиром при выставлении). Зеркало PortStock_ReturnShip.
bool PortAuction_RecallShip(ref rPortman, int iKeeperCharIdx, int iNewCaptainCharIdx)
{
	if (iKeeperCharIdx < 0 || iKeeperCharIdx >= TOTAL_CHARACTERS) return false;
	if (iNewCaptainCharIdx < 0 || iNewCaptainCharIdx >= TOTAL_CHARACTERS) return false;
	if (GetCompanionQuantity(pchar) >= COMPANION_MAX) return false;
	ref keeper = GetCharacter(iKeeperCharIdx);
	if (!CheckAttribute(keeper, "ShipAtAuction")) return false;     // не лот аукциона
	if (keeper.ShipAtAuction != rPortman.id) return false; // LEO: Чужой порт не вправе возвращать этот лот.
	if (!CheckAttribute(keeper, "Ship.Type")) return false;
	if (sti(keeper.Ship.Type) == SHIP_NOTUSED) return false;
	if (GetPassengerNumber(pchar, iNewCaptainCharIdx) < 0) return false;
	if (GetCompanionNumber(pchar, iNewCaptainCharIdx) >= 0) return false;
	ref compref = GetCharacter(iNewCaptainCharIdx);
	if (!CheckAttribute(compref, "index")) return false;
	if (!GetRemovable(compref)) return false;
	if (CheckAttribute(compref, "Ship.Type"))
	{
		if (sti(compref.Ship.Type) != SHIP_NOTUSED) return false; // LEO: Не перезаписываем уже принадлежащий капитану корабль.
	}
	if (CheckCompanionIndex(pchar, -1, iNewCaptainCharIdx) < 0) return false;

	aref arTo, arFrom;
	object oldShip;
	bool bHadShip = CheckAttribute(compref, "Ship");
	if (bHadShip)
	{
		makearef(arFrom, compref.ship);
		CopyAttributes(&oldShip, arFrom);
	}

	DeleteAttribute(compref, "ship");
	compref.ship = "";
	makearef(arTo, compref.ship);
	makearef(arFrom, keeper.ship);
	CopyAttributes(arTo, arFrom);

	// LEO: Держателя очищаем и офицера снимаем с должности только после успешного назначения в эскадру.
	if (SetCompanionIndex(pchar, -1, iNewCaptainCharIdx) != iNewCaptainCharIdx)
	{
		DeleteAttribute(compref, "ship");
		if (bHadShip)
		{
			compref.ship = "";
			makearef(arTo, compref.ship);
			CopyAttributes(arTo, &oldShip);
		}
		return false;
	}
	CheckForReleaseOfficer(iNewCaptainCharIdx); // снять с должности, если был не просто пассажиром
	RemovePassenger(pchar, compref);
	PortStock_RefreshSkills(compref);

	// чистим держателя-аукциониста + освобождаем слот массива (как при продаже/возврате со стоянки)
	DeleteAttribute(keeper, "ShipAtAuction");
	keeper.id = "ShipAtAuction";
	DeleteAttribute(keeper, "ship");
	keeper.ship = "";
	LAi_SetCurHP(keeper, 0.0);
	return true;
}

// LEO: УТРАТА ВЫРУЧКИ аукциона ГГ в городе при ШТУРМЕ врагами (захват/рейд). ТОЛЬКО касса ПУ -
// корабли на торгах и на стоянке НЕ трогаем (решение юзера). Зовётся рядом с SetNull2Deposit.
void PortAssets_NullOnSiege(string sCity)
{
	ref rPm = CharacterFromID(sCity + "_PortMan");
	if (!CheckAttribute(rPm, "AuctionVault")) return;
	if (sti(rPm.AuctionVault) <= 0) return;
	rPm.AuctionVault = 0;
	// красный (без анимации) + звук, единообразно с пропажей вкладов
	Log_InfoColored("red", XI_ConvertString("PortAuction_VaultLost") + " " + GetCityName(sCity));
	PlayStereoSound("interface\bort_jurnal.wav");
}

// LEO: Панаму не штурмуют (нет порта, носят ТОЛЬКО вклады), поэтому вклад у её ростовщика не теряется
// при захвате. Даём 5% МЕСЯЧНЫЙ шанс утраты (ростовщик сбежал/ограблен). Зов из суточного QuestActions.
void PanamaDeposit_MonthlyRoll()
{
	if (!CheckAttribute(pchar, "quest.Deposits.Panama")) return; // нет вклада - нечего терять
	int iCurMonth = getDataYear() * 12 + getDataMonth();
	int iLast = -1;
	if (CheckAttribute(pchar, "questTemp.PanamaUsurerMonth")) iLast = sti(pchar.questTemp.PanamaUsurerMonth);
	if (iLast == iCurMonth) return;                      // в этом месяце уже катали
	pchar.questTemp.PanamaUsurerMonth = iCurMonth;
	if (iLast < 0) return;                               // первый учёт месяца - без ролла (не наказываем сразу)
	if (dRand(99) < 5) SetNull2Deposit("Panama");        // 5% дневной drand (на день смены месяца не сейв-скамится) - вклад пропал
}
//<-- аукцион кораблей в ПУ

// Warship 31.05.09 Установка на корабль персонажа определённого паруса
// Путь относительно папки Textures
bool SetShipSailsFromFile(ref _char, String _sailTexturePath)
{
	ref realShip;
	int shipType = sti(_char.Ship.Type);

	if(shipType == SHIP_NOTUSED) // Есть ли корабль вообще
	{
		return false;
	}

	realShip = GetRealShip(shipType);

	realShip.EmblemedSails.normalTex = _sailTexturePath;

	return true;
}

string GetLocalShipAttrib(ref ship, string attrib)
{
	if (CheckAttribute(ship, attrib)) return ship.(attrib);
	return "";
}

bool CheckShipAttribute(ref ship, string attrib)
{
	if(CheckAttribute(ship, attrib)) return true;
	return false;
}

string GetBaseShipParamFromType(int iType, string _param)
{
	object rBaseShip = GetShipByType(iType);
	return rBaseShip.(_param);
}

int GetShipPriceByTTH_HP(int shipClass, ref rRealShip)
{
	//корпус
	int hp_price = 1;
	switch(shipClass)
	{
		case 7: hp_price = 1; break;
		case 6: hp_price = 3; break;
		case 5: hp_price = 5; break;
		case 4: hp_price = 8; break;
		case 3: hp_price = 10; break;
		case 2: hp_price = 12; break;
		case 1: hp_price = 15; break;
	}
	return sti(rRealShip.HP) * hp_price;
}
int GetShipPriceByTTH_Capacity(int shipClass, ref rRealShip)
{
	//трюм
	int capacity_price = 1;
	switch(shipClass)
	{
		case 7: capacity_price = 1; break;
		case 6: capacity_price = 3; break;
		case 5: capacity_price = 5; break;
		case 4: capacity_price = 8; break;
		case 3: capacity_price = 10; break;
		case 2: capacity_price = 12; break;
		case 1: capacity_price = 15; break;
	}
	return sti(rRealShip.Capacity) * capacity_price;
}
int GetShipPriceByTTH_MaxCrew(int shipClass, ref rRealShip)
{
	//команда
	int crew_price = 1;
	switch(shipClass)
	{
		case 7: crew_price = 2; break;
		case 6: crew_price = 10; break;
		case 5: crew_price = 15; break;
		case 4: crew_price = 20; break;
		case 3: crew_price = 30; break;
		case 2: crew_price = 35; break;
		case 1: crew_price = 40; break;
	}
	return sti(rRealShip.MaxCrew) * crew_price;
}
int GetShipPriceByTTH_MaxCaliber(int shipClass, ref rRealShip)
{
	//калибр
	int caliber_price = 5000;
	switch(sti(rRealShip.MaxCaliber))
	{
		case 8: caliber_price = 8000; break;
		case 10: caliber_price = 15000; break;
		case 12: caliber_price = 25000; break;
		case 16: caliber_price = 50000; break;
		case 20: caliber_price = 75000; break;
		case 24: caliber_price = 100000; break;
		case 28: caliber_price = 150000; break;
		case 32: caliber_price = 200000; break;
		case 36: caliber_price = 250000; break;
		case 42: caliber_price = 300000; break;
		case 48: caliber_price = 650000; break;
	}
	return caliber_price;
}
int GetShipPriceByTTH_CannonsQuantity(int shipClass, ref rChar, ref rRealShip)
{
	//количество пушек
	int cannon_price = 100;
	if (!CheckAttribute(rRealShip, "CannonsQuantity")) {
		trace("GetShipPriceByTTH: у " + rRealShip.BaseName + " отсутствует атрибут CannonsQuantity");
		return 0;
	}
	int iCannonsQuantity = sti(rRealShip.CannonsQuantity);
	if (iCannonsQuantity <= 0) {
		return 0;
	}
	if(!CheckAttribute(rChar, "Ship.Cannons.Type") || sti(rChar.Ship.Cannons.Type) == CANNON_TYPE_NONECANNON)
	{
		return cannon_price = iCannonsQuantity * 1500; //универсальное значение для NullCharacter
	}
	ref rCannon = GetCannonByType(sti(rChar.Ship.Cannons.Type));
	if (!CheckAttribute(rCannon,"Cost")) {
		cannon_price = 0; //меняю цену с 1500 на 0, чтобы цена корабля без пушек при заказе считалась правильно.
		if (CurrentInterface != INTERFACE_SHIPYARD1 && CurrentInterface != INTERFACE_SHIPYARD2){
			log_info(GetConvertStrWithReplace("Variable_ShipsUtilites_1", "Logs.txt", "#space#", " "));// лог для отслеживания, что эта ветка кода только при заказе корабля используется и исправление ничего не сломало
		}
	}
	else {
		cannon_price = sti(rCannon.Cost);
	}
	return sti(rRealShip.CannonsQuantity) * cannon_price; //2
}
int GetShipPriceByTTH_WindAgainstSpeed(int shipClass, ref rRealShip)
{
	//бейдевинд
	int was_price = 10;
	switch(shipClass)
	{
		case 7: was_price = 6; break;
		case 6: was_price = 125; break;
		case 5: was_price = 312; break;
		case 4: was_price = 2500; break;
		case 3: was_price = 6250; break;
		case 2: was_price = 12500; break;
		case 1: was_price = 25000; break;
	}
	return makeint(stf(rRealShip.WindAgainstSpeed) * was_price);
}
int GetShipPriceByTTH_MastMultiplier(int shipClass, ref rRealShip)
{
	//мачты
	int mast_price = 10;
	switch(shipClass)
	{
		case 7: mast_price = 6; break;
		case 6: mast_price = 125; break;
		case 5: mast_price = 312; break;
		case 4: mast_price = 2500; break;
		case 3: mast_price = 6250; break;
		case 2: mast_price = 12500; break;
		case 1: mast_price = 25000; break;
	}
	return makeint(mast_price/(stf(rRealShip.MastMultiplier)/2));
}
int GetShipPriceByTTH_SpeedRate(int shipClass, ref rRealShip)
{
	//скорость
	int speed_price = 10;
	switch(shipClass)
	{
		case 7: speed_price = 8; break;
		case 6: speed_price = 50; break;
		case 5: speed_price = 100; break;
		case 4: speed_price = 250; break;
		case 3: speed_price = 1250; break;
		case 2: speed_price = 2500; break;
		case 1: speed_price = 10000; break;
	}
	return MakeInt(stf(rRealShip.SpeedRate) * speed_price);
}
int GetShipPriceByTTH_TurnRate(int shipClass, ref rRealShip)
{
	//маневренность
	int turn_price = 10;
	switch(shipClass)
	{
		case 7: turn_price = 2; break;
		case 6: turn_price = 12; break;
		case 5: turn_price = 50; break;
		case 4: turn_price = 125; break;
		case 3: turn_price = 750; break;
		case 2: turn_price = 1500; break;
		case 1: turn_price = 6000; break;
	}
	return MakeInt(stf(rRealShip.TurnRate) * turn_price);
}
//Mett: --> формула стоимости кораблей по ТТХ
// LEO: считаем по ЛЮБОМУ описанию корабля - и по сгенерированному экземпляру (RealShips), и по
//   эталону типа (ShipsTypes). Раньше функция умела только первое, поэтому ПГГ и ГВИК читали
//   рудиментарный refShip.Price из инита и сравнивали его с ценой по ТТХ - две разные шкалы.
int GetPriceByTTH(ref rShip, ref rChar)
{
	int shipClass = sti(rShip.class);

	int iSumm = GetShipPriceByTTH_HP(shipClass, rShip);
	iSumm += GetShipPriceByTTH_Capacity(shipClass, rShip);
	iSumm += GetShipPriceByTTH_MaxCrew(shipClass, rShip);
	iSumm += GetShipPriceByTTH_MaxCaliber(shipClass, rShip);
	iSumm += GetShipPriceByTTH_CannonsQuantity(shipClass, rChar, rShip);
	iSumm += GetShipPriceByTTH_WindAgainstSpeed(shipClass, rShip);
	iSumm += GetShipPriceByTTH_MastMultiplier(shipClass, rShip);
	iSumm += GetShipPriceByTTH_SpeedRate(shipClass, rShip);
	iSumm += GetShipPriceByTTH_TurnRate(shipClass, rShip);
	if (iSumm <= 0) iSumm = 100;
	return iSumm;
}

int GetShipPriceByTTH(int iType, ref rChar)
{
	ref rRealShip = GetRealShip(iType);
	return GetPriceByTTH(rRealShip, rChar);
}

// LEO: цена ЭТАЛОНА типа - для прикидок «что этот корабль стоит вообще» (выбор корабля ПГГ,
//   заказ ГВИК, справка в интерфейсе). Пушки идут по универсальной ставке, как у NullCharacter:
//   у типа орудий ещё нет, их выбирают уже на конкретном корабле.
int GetShipTypePriceByTTH(int iShipType)
{
	ref rShip = GetShipByType(iShipType);
	return GetPriceByTTH(rShip, &NullCharacter);
}
//Mett: <-- формула стоимости кораблей по ТТХ

//<---Lipsar корабли по квесту Предложение Пирата
void CreatePGG_War(ref ch, int iNation, ref shipOwner)
{
	aref rShip;
	ref rMerch;
	int iType, cl, cl1, i, j;
	ref refShip;
	refShip = GetRealShip(sti(shipOwner.ship.Type));
	int storeArray1[SHIP_TYPES_QUANTITY];
	j = 0;
	switch(sti(refShip.Class))
	{
		case 7:
			cl = 3;
			cl1 = 15;
		break;
		case 6:
			cl = 3;
			cl1 = 15;
		break;
		case 5:
			cl = 15;
			cl1 = 28;
		break;
		case 4:
			cl = 28;
			cl1 = 52;
		break;
		case 3:
			if (MOD_SKILL_ENEMY_RATE == 3)
			{
				cl = 52;
				cl1 = 84;
			}
			else
			{
				cl = 28;
				cl1 = 52;
			}
		break;
		case 2:
			if (MOD_SKILL_ENEMY_RATE == 3)
			{
				cl = 84;
				cl1 = 105;
			}
			else
			{
				cl = 52;
				cl1 = 84;
			}
		break;
		case 1:
			if (MOD_SKILL_ENEMY_RATE == 3)
			{
				cl = 105;
				cl1 = 125;
			}
			else
			{
				cl = 84;
				cl1 = 105;
			}
		break;
	}
	iType = GetShipTypeByBand(cl, cl1, "War", iNation); // LEO: мемоизированный подбор (кэш) вместо скана банда каждый раз
	ch.Ship.Type = GenerateShipExt(iType, 1, ch);
}
void CreatePGG_Trade(ref ch, int iNation, ref shipOwner)
{
	aref rShip;
	ref rMerch;
	int iType, cl, cl1, i, j;
	ref refShip;
	refShip = GetRealShip(sti(shipOwner.ship.Type));
	int storeArray2[SHIP_TYPES_QUANTITY];
	j = 0;

	switch(sti(refShip.Class))
	{
		case 7:
			cl = 3;
			cl1 = 15;
		break;
		case 6:
			cl = 3;
			cl1 = 15;
		break;
		case 5:
			cl = 15;
			cl1 = 28;
		break;
		case 4:
			cl = 28;
			cl1 = 52;
		break;
		case 3:
			cl = 52;
			cl1 = 84;
		break;
		case 2:
			cl = 52;
			cl1 = 84;
		break;
		case 1:
			cl = 52;
			cl1 = 84;
		break;
	}
	i = cl;
	string sNat = GetShipsTypeNationAttr(iNation);
	iType = GetShipTypeByBand(cl, cl1, "Merchant", iNation); // LEO: мемоизированный подбор (кэш) вместо скана банда каждый раз
	ch.Ship.Type = GenerateShipExt(iType, 1, ch);
	ch.Ship.Mode = "trade";
}
//--->Lipsar корабли по квесту Предложение Пирата
void CreateFortDefenders(ref ch, int iNation)
{
	aref rShip;
	ref rMerch;
	int iType, cl, cl1, i, j;
	cl1 = 116;
	cl  = 84;
	int storeArray1[SHIP_TYPES_QUANTITY];
	j = 0;
	string sNat = GetShipsTypeNationAttr(iNation);
	for (i=cl;i<cl1;i++)
	{
		makearef(rShip,ShipsTypes[i].nation.(sNat));
		if (rShip == true)
		{
			makeref(rMerch,ShipsTypes[i]);
			if(rMerch.Type.War == true)
			{
				storeArray1[j]= i;
				j++;
			}
		}
	}
	iType = storeArray1[rand(j-1)];
	ch.Ship.Type = GenerateShipExt(iType, 1, ch);
}
void CreateFortMerchants(ref ch, int iNation)
{
	aref rShip;
	ref rMerch;
	int iType, cl, cl1, i, j;
	int storeArray2[SHIP_TYPES_QUANTITY];
	j = 0;
	cl = 3;
	cl1 = 52;
	switch (iNation)
	{
		case "0":
			for (i=cl;i<cl1;i++)
			{
				makearef(rShip,ShipsTypes[i].nation.england);
				if (rShip == true)
				{
					makeref(rMerch,ShipsTypes[i]);
					if(rMerch.Type.Merchant == true)
					{
						storeArray2[j]= i;
						j++;
					}
				}
			}
		break;
		case "1":
			for (i=cl;i<cl1;i++)
			{
				makearef(rShip,ShipsTypes[i].nation.france);
				if (rShip == true)
				{
					makeref(rMerch,ShipsTypes[i]);
					if(rMerch.Type.Merchant == true)
					{
						storeArray2[j]= i;
						j++;
					}
				}
			}
		break;
		case "2":
			for (i;i<cl1;i++)
			{
				makearef(rShip,ShipsTypes[i].nation.spain);
				if (rShip == true)
				{
					makeref(rMerch,ShipsTypes[i]);
					if(rMerch.Type.Merchant == true)
					{
						storeArray2[j]= i;
						j++;
					}
				}
			}
		break;
		case "3":
			for (i=cl;i<cl1;i++)
			{
				makearef(rShip,ShipsTypes[i].nation.holland);
				if (rShip == true)
				{
					makeref(rMerch,ShipsTypes[i]);
					if(rMerch.Type.Merchant == true)
					{
						storeArray2[j]= i;
						j++;
					}
				}
			}
		break;
		case "4":
			for (i=cl;i<cl1;i++)
			{
				makearef(rShip,ShipsTypes[i].nation.pirate);
				if (rShip == true)
				{
					makeref(rMerch,ShipsTypes[i]);
					if(rMerch.Type.Merchant == true)
					{
						storeArray2[j]= i;
						j++;
					}
				}
			}
		break;
	}
	iType = storeArray2[rand(j-1)];
	ch.Ship.Type = GenerateShipExt(iType, 1, ch);
}

//Генерация топового корабля
int GenerateShipTop(int iBaseType, bool isStolen, ref chr)
{
	string  attr;
	int 	i;
	aref 	refShip;

	int iShip = CreateBaseShip(iBaseType);

	if (iShip == -1 || iShip == SHIP_NOTUSED) return SHIP_NOTUSED;

	ref rRealShip = GetRealShip(iShip);
	ref rBaseShip = GetShipByType(sti(rRealShip.BaseType));

	rRealShip.QuestShip = true;

    if(CheckAttribute(rRealShip, "hullNums")) 
	{
        int nHulls = sti(rRealShip.hullNums);
        nHulls -= 1;
        if(nHulls < 0) nHulls = 0;
        rRealShip.ship.upgrades.hull = 1 + rand(nHulls);
    }
    else {rRealShip.ship.upgrades.hull = 1 + rand(2);}
	rRealShip.ship.upgrades.sails = 1 + rand(2);  // только визуальная разница

	switch (sti(rRealShip.Class))
	{
		case 7: rRealShip.HullArmor = 5; break;
		case 6: rRealShip.HullArmor = 9; break;
		case 5: rRealShip.HullArmor = 13; break;
		case 4: rRealShip.HullArmor = 18; break;
		case 3: rRealShip.HullArmor = 24; break;
		case 2: rRealShip.HullArmor = 30; break;
		case 1: rRealShip.HullArmor = 36; break;
	}

	if (!CheckAttribute(rRealShip, "isFort"))
	{
		makearef(refShip, chr.Ship);
		ResetShipCannonsDamages(chr);

		// куда-нить запишем максимально возможное кол-во орудий (потом нужно будет, если захотим проапгрейдиться на этот счет)
		rRealShip.CannonsQuantityMax = sti(rRealShip.CannonsQuantity);
		// принципиальный момент ! нужно нулить обязательно левые стволы !
		for (i = 0; i < sti(rRealShip.rcannon); i++)
		{
			attr = "c" + i;
			rRealShip.Cannons.Borts.cannonr.damages.(attr) = 1.0;
			rRealShip.Cannons.Borts.cannonl.damages.(attr) = 1.0;
			rRealShip.Cannons.Borts.rcannon.damages.(attr) = 1.0;
			rRealShip.Cannons.Borts.lcannon.damages.(attr) = 1.0;

			refShip.Cannons.Borts.cannonr.damages.(attr) = 1.0;
			refShip.Cannons.Borts.cannonl.damages.(attr) = 1.0;
			refShip.Cannons.Borts.rcannon.damages.(attr) = 1.0;
			refShip.Cannons.Borts.lcannon.damages.(attr) = 1.0;
		}

		rRealShip.rcannon = sti(rRealShip.rcannon);
		rRealShip.lcannon = sti(rRealShip.lcannon);

		rRealShip.cannonr = sti(rRealShip.rcannon);
		rRealShip.cannonl = sti(rRealShip.lcannon);
		rRealShip.cannonf = sti(rRealShip.fcannon);
		rRealShip.cannonb = sti(rRealShip.bcannon);

		rRealShip.Cannons = sti(rRealShip.CannonsQuantityMax);
		rRealShip.CannonsQuantity = sti(rRealShip.Cannons);

		//а теперь вертаем стволы обратно в потребном нам количестве
		for (i = 0; i < sti(rRealShip.rcannon); i++)
		{
			attr = "c" + i;
			rRealShip.Cannons.borts.cannonr.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.cannonl.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.rcannon.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.lcannon.damages.(attr) = 0.0;

			refShip.Cannons.borts.cannonr.damages.(attr) = 0.0;
			refShip.Cannons.borts.cannonl.damages.(attr) = 0.0;
			refShip.Cannons.borts.rcannon.damages.(attr) = 0.0;
			refShip.Cannons.borts.lcannon.damages.(attr) = 0.0;
		}

		for (i = 0; i < sti(rRealShip.fcannon); i++)
		{
			attr = "c" + i;
			rRealShip.Cannons.borts.cannonf.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.fcannon.damages.(attr) = 0.0;
			refShip.Cannons.borts.cannonf.damages.(attr) = 0.0;
			refShip.Cannons.borts.fcannon.damages.(attr) = 0.0;
		}

		for (i = 0; i < sti(rRealShip.bcannon); i++)
		{
			attr = "c" + i;
			rRealShip.Cannons.borts.cannonb.damages.(attr) = 0.0;
			rRealShip.Cannons.borts.bcannon.damages.(attr) = 0.0;
			refShip.Cannons.borts.cannonb.damages.(attr) = 0.0;
			refShip.Cannons.borts.bcannon.damages.(attr) = 0.0;
		}

		refShip.Cannons.Borts.cannonl = sti(rRealShip.lcannon);
		refShip.Cannons.Borts.cannonr = sti(rRealShip.rcannon);
		refShip.Cannons.Borts.cannonf = sti(rRealShip.fcannon);
		refShip.Cannons.Borts.cannonb = sti(rRealShip.bcannon);

		refShip.Cannons.Borts.lcannon = sti(rRealShip.lcannon);
		refShip.Cannons.Borts.rcannon = sti(rRealShip.rcannon);
		refShip.Cannons.Borts.fcannon = sti(rRealShip.fcannon);
		refShip.Cannons.Borts.bcannon = sti(rRealShip.bcannon);

		refShip.Cannons = sti(rRealShip.Cannons);

		if (!CheckAttribute(rRealShip, "QuestShip"))
		{
			rRealShip.Capacity		= makeint(sti(rRealShip.Capacity)*(1.0 + SHIP_STAT_RANGE_DRAFT));
			rRealShip.HP			= makeint(sti(rRealShip.HP)*(1.0 + SHIP_STAT_RANGE_DRAFT));
			rRealShip.SpeedRate		= stf(rRealShip.SpeedRate)*(1.0 + SHIP_STAT_RANGE_DRAFT);
			rRealShip.TurnRate		= stf(rRealShip.TurnRate)*(1.0 + SHIP_STAT_RANGE_DRAFT);
			rRealShip.WindAgainstSpeed 	= stf(rRealShip.WindAgainstSpeed)*(1.0 + SHIP_STAT_RANGE_DRAFT);
			rRealShip.MastMultiplier = stf(rBaseShip.MastMultiplier) - 0.3;//фикс нестандартных мачт
		}
	}

	if (!CheckAttribute(rRealShip, "QuestShip"))
	{
		rRealShip.OptCrew       = makeint(sti(rRealShip.OptCrew)*(1.0 + SHIP_STAT_RANGE_DRAFT));
		rRealShip.MaxCrew       = makeint(sti(rRealShip.OptCrew)*1.25);
	}
	// to_do del -->
	rRealShip.BoardingCrew  = 0;
	rRealShip.GunnerCrew    = 0;
	rRealShip.CannonerCrew  = 0;
	// to_do del <--

	if (!CheckAttribute(rRealShip, "QuestShip")) 
	{
		rRealShip.Blueprint = true;
		SetCabinTypeEx(rRealShip, sti(rRealShip.Class)); //Выдача случайной каюты по классу не квестовым - Gregg
	}

	rRealShip.Price	= GetShipPriceByTTH(iShip, chr);//фикс - такая же цена как при заказе на верфи

	if (sti(rRealShip.Price) <= 0) rRealShip.Price = 100;

	rRealShip.Stolen = isStolen;  // ворованность
	rRealShip.soiling = 0;

	return iShip;
}
// -> ugeen

int GetShootDistance(ref chref, string ball)
{
	float distance = 0.0;
	float fKoefBallType = 1.0;
	int p = GetCharacterSPECIALSimple(chref, SPECIAL_P)-3;
	if (p < 0) p = 0;

	ref	rCannon = GetCannonByType(sti(chref.Ship.Cannons.Type));
	if (!CheckAttribute(rCannon, "FireRange"))
	{
		distance = Goods[GOOD_CANNON_24].FireRange;
	}
	else {
		distance = stf(rCannon.FireRange);
	}
	bool bOk = chref.index == GetMainCharacterIndex() && CheckOfficersPerk(chref, "LongRangeShoot");
	if (CheckAttribute(chref, "perks.list.LongRangeShoot") || bOk)
	{
		distance = distance * (1.15+p*0.03);
	}
	else
	{
		distance = distance * (1.0+p*0.03);
	}
	switch(ball)//зависит от скорости из инита квадратично
	{
		case "ball":	fKoefBallType = stf(Goods[GOOD_BALLS].SpeedV0); break;
		case "grape":	fKoefBallType = stf(Goods[GOOD_GRAPES].SpeedV0); break;		//0,55*0,55	=0,3025
		case "knippel":	fKoefBallType = stf(Goods[GOOD_KNIPPELS].SpeedV0); break;		//0,90*0,90	=0,81
		case "bomb":	fKoefBallType = stf(Goods[GOOD_BOMBS].SpeedV0); break;		//0,65*0,65	=0,4225
	}
	distance = distance * fKoefBallType * fKoefBallType;
	return makeint(distance+0.5);
}

int GetShipTypeExt(int iClassMin, int iClassMax, string sShipType, int iNation)//нация -1 допускает любые нации, иначе только одну указанную		//sShipType = "merchant"/"war",	"" - любой тип
{
	int iShips[SHIP_TYPES_QUANTITY];
	int iShipsNum = 0;
	aref aNation;
	string sNation;
	bool bOk;
	int iClass;
	string sNatKey, sClsKey, sTypKey, sPath, sRk, sM; // LEO: кэш подбора типа
	int cn, m;                                        // LEO: кэш подбора типа

	// LEO: КЭШ подбора типа. Ключ ВЛОЖЕННЫЙ (нация -> диапазон классов -> тип), НЕ плоский!
	// Поиск атрибута в движке ЛИНЕЙНЫЙ по числу детей узла со сравнением строк
	// (ATTRIBUTES::GetAttributeClass, engine core/src/attributes.cpp). Плоский ключ копил ТЫСЯЧИ детей в
	// ОДНОМ узле -> каждое обращение дорожало по мере плавания => прогрессирующие фризы глобалки (кэш
	// становился медленнее скана 143 типов, который он заменял). Вложенность держит на каждом уровне
	// единицы-десятки детей, все домены конечные => стоимость лукапа ПОСТОЯННА и не растёт со временем.
	if (iNation < 0) sNatKey = "n_any"; else sNatKey = "n" + iNation;
	sClsKey = "c" + iClassMin + "_" + iClassMax;
	sTypKey = "t" + sShipType;
	sPath = "k." + sNatKey + "." + sClsKey + "." + sTypKey;
	if (CheckAttribute(&g_ShipTypeCache, sPath + ".num"))
	{
		cn = sti(g_ShipTypeCache.k.(sNatKey).(sClsKey).(sTypKey).num);
		if (cn <= 0) return INVALID_SHIP_TYPE;
		sRk = "" + rand(cn - 1);
		return sti(g_ShipTypeCache.k.(sNatKey).(sClsKey).(sTypKey).(sRk));
	}

	for (int i=0; i<=SHIP_TYPES_QUANTITY; i++)
	{
		object rShip = GetShipByType(i);
		if (!checkAttribute(rship, "class")) trace ("bad ship is: " + rship.name);

		if (checkAttribute(rship, "QuestShip")) continue; //квестовые корабли Игнорим

		iClass = MakeInt(rShip.Class);
		if (iClass > iClassMin) { continue; }
		if (iClass < iClassMax) { continue; }//по классу Игнорим

		if (sti(rShip.CanEncounter) != true) { continue; }
		if (sShipType != "" && sti(rShip.Type.(sShipType)) != true) { continue; }//по типу игнорим
		bOk = false;
		if(iNation == -1 || !CheckAttribute(rShip, "nation"))
		{
			iShips[iShipsNum] = i;
			iShipsNum++;
			continue;
		}
		sNation = GetNationNameByType(iNation);
		if(rShip.nation.(sNation) != true ) { continue; }//по нации игнорим
		iShips[iShipsNum] = i;
		iShipsNum++;
	}
	// LEO: сохранить результат скана в кэш (в т.ч. пустой: num=0 -> следующие хиты сразу вернут INVALID без скана).
	// num пишем ПЕРВЫМ: движок перебирает детей в порядке создания, поэтому .num находится с первого сравнения.
	g_ShipTypeCache.k.(sNatKey).(sClsKey).(sTypKey).num = iShipsNum;
	for (m = 0; m < iShipsNum; m++)
	{
		sM = "" + m;
		g_ShipTypeCache.k.(sNatKey).(sClsKey).(sTypKey).(sM) = iShips[m];
	}

	if (iShipsNum==0)
	{
		Trace("Can't find ship type '" + sShipType + "' with ClassMin = " + iClassMin + " and ClassMax = " + iClassMax + " iNation = " + iNation);
		return INVALID_SHIP_TYPE;
	}

	return iShips[rand(iShipsNum - 1)];
}

// LEO: тот же подбор, что и GetShipTypeExt, но БЕЗ внутреннего rand - отдаёт элемент пула по ИНДЕКСУ
// (по модулю размера пула). Нужен там, где состав эскадры должен быть детерминирован дневным dRand и не
// переролиться сейв-лоадом: например перехватчики ДУ фиксируются днём взятия фрахта.
// Пул тот же самый (кэш g_ShipTypeCache), поэтому классы/нации/типы отбираются один в один.
int GetShipTypeExtByIdx(int iClassMin, int iClassMax, string sShipType, int iNation, int idx)
{
	// прогреваем кэш штатной функцией - она же и заполнит пул, если его ещё нет
	if (GetShipTypeExt(iClassMin, iClassMax, sShipType, iNation) == INVALID_SHIP_TYPE) return INVALID_SHIP_TYPE;

	string sNatKey, sClsKey, sTypKey, sRk;
	if (iNation < 0) sNatKey = "n_any"; else sNatKey = "n" + iNation;
	sClsKey = "c" + iClassMin + "_" + iClassMax;
	sTypKey = "t" + sShipType;

	int cn = sti(g_ShipTypeCache.k.(sNatKey).(sClsKey).(sTypKey).num);
	if (cn <= 0) return INVALID_SHIP_TYPE;
	if (idx < 0) idx = -idx;
	idx = idx % cn;
	sRk = "" + idx;
	return sti(g_ShipTypeCache.k.(sNatKey).(sClsKey).(sTypKey).(sRk));
}

// LEO: мемоизированный подбор типа корабля по БАНДУ класса [cl,cl1) + нация + тип ("War"/"Merchant") — для свиты
//   ПГГ (CreatePGG_War/Trade/CreateFortDefenders). Тот же кэш g_ShipTypeCache, ключ с префиксом "b" (не пересекается
//   с "k" у GetShipTypeExt). Хит = O(1) вместо скана банда с makearef/makeref на КАЖДЫЙ корабль свиты (снижает бурст).
int GetShipTypeByBand(int cl, int cl1, string sType, int iNation)
{
	int bShips[SHIP_TYPES_QUANTITY];
	int bNum = 0;
	aref rShip;
	ref rMerch;
	string sNatKey, sClsKey, sTypKey, sPath, sNat, sRk, sM;
	int cn, m, i;

	// LEO: ключ ВЛОЖЕННЫЙ (см. подробный коммент в GetShipTypeExt): плоский копил тысячи детей в одном узле,
	// а поиск атрибута в движке линейный => кэш деградировал по мере плавания. Ветка "b" отделена от "k".
	if (iNation < 0) sNatKey = "n_any"; else sNatKey = "n" + iNation;
	sClsKey = "c" + cl + "_" + cl1;
	sTypKey = "t" + sType;
	sPath = "b." + sNatKey + "." + sClsKey + "." + sTypKey;
	if (CheckAttribute(&g_ShipTypeCache, sPath + ".num"))
	{
		cn = sti(g_ShipTypeCache.b.(sNatKey).(sClsKey).(sTypKey).num);
		if (cn <= 0) return INVALID_SHIP_TYPE;
		sRk = "" + rand(cn - 1);
		return sti(g_ShipTypeCache.b.(sNatKey).(sClsKey).(sTypKey).(sRk));
	}
	sNat = GetShipsTypeNationAttr(iNation);
	for (i = cl; i < cl1; i++)
	{
		makearef(rShip, ShipsTypes[i].nation.(sNat));
		if (rShip == true)
		{
			makeref(rMerch, ShipsTypes[i]);
			if (rMerch.Type.(sType) == true)
			{
				bShips[bNum] = i;
				bNum++;
			}
		}
	}
	g_ShipTypeCache.b.(sNatKey).(sClsKey).(sTypKey).num = bNum; // num первым - находится с первого сравнения
	for (m = 0; m < bNum; m++)
	{
		sM = "" + m;
		g_ShipTypeCache.b.(sNatKey).(sClsKey).(sTypKey).(sM) = bShips[m];
	}
	if (bNum <= 0) return INVALID_SHIP_TYPE;
	return bShips[rand(bNum - 1)];
}

int GetShipTypeExtNotNation(int iClassMin, int iClassMax, string sShipType, int iNation)//нация - исключает выбор указанной нации		//sShipType = "merchant"/"war",	"" - любой тип
{
	int iShips[SHIP_TYPES_QUANTITY];
	int iShipsNum = 0;
	aref aNation;
	string sNation;
	bool bOk;
	int iClass;

	for (int i=0; i<=SHIP_TYPES_QUANTITY; i++)
	{
		object rShip = GetShipByType(i);
		if (!checkAttribute(rship, "class")) trace ("bad ship is: " + rship.name);

		if (checkAttribute(rship, "QuestShip")) continue; //квестовые корабли Игнорим

		iClass = MakeInt(rShip.Class);
		if (iClass > iClassMin) { continue; }
		if (iClass < iClassMax) { continue; }//по классу Игнорим

		if (sti(rShip.CanEncounter) != true) { continue; }
		if (sShipType != "" && sti(rShip.Type.(sShipType)) != true) { continue; }//по типу игнорим
		bOk = false;
		if (CheckAttribute(rShip, "nation"))
		{
			sNation = GetNationNameByType(iNation); 
			if (rShip.nation.(sNation) == true ) { continue; }//игнорим, если есть указанная нация. Специально для квестов по поиску кораблей, чтоб нельзя было в том же городе заказать
		}
		iShips[iShipsNum] = i;
		iShipsNum++;
	}
	if (iShipsNum==0)
	{
		Trace("Can't find ship type '" + sShipType + "' with ClassMin = " + iClassMin + " and ClassMax = " + iClassMax + " iNation = " + iNation);
		return INVALID_SHIP_TYPE;
	}

	return iShips[rand(iShipsNum - 1)];
}

void SetShipsBonuses(ref rRealShip)
{
	// Gregg: числовые таблицы бонусов держит SelectShipsBonuses (единый источник);
	// здесь только выбираем индексы и пишем в сейв-атрибуты. -1 = бонус неактивен
	// (в SelectShipsBonuses ни один case не совпадёт -> 0), эквивалент «switch не выполнялся».
	if (CheckAttribute(rRealShip, "QuestShip")) return;

	int tipsBonus = -1;
	int tipmBonus = -1;

	if(CheckAttribute(rRealShip, "spawnBonus"))
	{
		if(sti(rRealShip.spawnBonus) == true)
		{
			tipsBonus = rand(12);
			rRealShip.savesBonus = tipsBonus;
		}
	}
	if(CheckAttribute(rRealShip, "materialBonus"))
	{
		if(sti(rRealShip.materialBonus) == true)
		{
			tipmBonus = rand(7);
			rRealShip.savemBonus = tipmBonus;
		}
	}

	//генерим статы со всеми бонусами (значения — из единого SelectShipsBonuses)
	rRealShip.HP               = GenerateShipStatI(sti(rRealShip.HP), SelectShipsBonuses(rRealShip, "bonusHP", tipsBonus, tipmBonus));
	rRealShip.SpeedRate        = GenerateShipStatF(stf(rRealShip.SpeedRate), SelectShipsBonuses(rRealShip, "bonusSpeed", tipsBonus, tipmBonus));
	rRealShip.TurnRate         = GenerateShipStatF(stf(rRealShip.TurnRate), SelectShipsBonuses(rRealShip, "bonusTurnRate", tipsBonus, tipmBonus));
	rRealShip.WindAgainstSpeed = GenerateShipStatF(stf(rRealShip.WindAgainstSpeed), SelectShipsBonuses(rRealShip, "bonusWindAgainstSpeed", tipsBonus, tipmBonus));
	rRealShip.Capacity         = GenerateShipStatI(sti(rRealShip.Capacity), SelectShipsBonuses(rRealShip, "bonusCapacity", tipsBonus, tipmBonus));
	rRealShip.OptCrew          = GenerateShipStatI(sti(rRealShip.OptCrew), SelectShipsBonuses(rRealShip, "bonusOptCrew", tipsBonus, tipmBonus));
}

float SelectShipsBonuses(ref rRealShip, string stattype, int tipsBonus, int tipmBonus)
{
	float bonusHP = 0.0;
    float bonusSpeed = 0.0;
    float bonusTurnRate = 0.0;
    float bonusWindAgainstSpeed = 0.0;
    float bonusCapacity = 0.0;
    float bonusOptCrew = 0.0;

	switch (tipsBonus)
	{
		case 0:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS12;
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS04;
			break; // Basster refit
		case 1: 
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;				
			bonusWindAgainstSpeed    -= SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			break; // Kingstown refit
		case 2:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS07;
			break; // Nassau refit
		case 3:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS10;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS08;
			break; // Marigo refit
		case 4:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS04;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS06;
			break; // Gavana refit
		case 5:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS02;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS02;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS09;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS02;
			break; // Trinidad refit
		case 6:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS08;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			break; // Cartagena refit
		case 7:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS03;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS10;
			break; // Amsterdam refit
		case 8:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS08;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS04;
			break; // Scheepswerf refit
		case 9:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS08;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // Portsmouth refit
		case 10:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS0;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS05;
			break; // Brest refit
		case 11:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS10;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // Rochester refit
		case 12:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS0;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS03;
			break; // Boston refit 
	}

	switch (tipmBonus)
	{
		case 0: 
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS06;
			break; // teak
		case 1:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS02;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS02;
			bonusWindAgainstSpeed    -= SHIP_STAT_RANGE_GENERATION_BONUS10;	
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // White oak
		case 2:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS15;	
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS04;  
			break; // fir
		case 3:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS06;	
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			break; // Mahagony
		case 4:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS05;	
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // Sabicu	
		case 5:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusWindAgainstSpeed    -= SHIP_STAT_RANGE_GENERATION_BONUS04;	
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // Cedr 
		case 6:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS05;	
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS06;
			break; // larch 
		case 7:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusWindAgainstSpeed    -= SHIP_STAT_RANGE_GENERATION_BONUS07;	
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS07;
			break; // Swamp oak
	}

	//генерим статы со всеми бонусами
	if (stattype == "bonusHP") return bonusHP;
	if (stattype == "bonusSpeed") return bonusSpeed;
	if (stattype == "bonusTurnRate") return bonusTurnRate;
	if (stattype == "bonusWindAgainstSpeed") return bonusWindAgainstSpeed;
	if (stattype == "bonusCapacity") return bonusCapacity;
	if (stattype == "bonusOptCrew") return bonusOptCrew;
	return 0.0;
}

void CheckForShipStatsChange(ref rship, string uptype) //метод для изменения статов корабля от апгрейдов
{
	float multi, multi2;
	int oldHP = sti(rship.HP);
	switch (uptype)
	{
		case "SUP_TRADEARMOUR": //Защита трюма
			rship.HullArmor = makeint(stf(rship.HullArmor) * 0.9 + 0.5);
		break;
		case "SUP_SAIL": //Усиленные паруса
			if (sti(rship.class) == 6 || sti(rship.class) == 5) multi = 1.1;
			else if (sti(rship.class) == 4 || sti(rship.class) == 3) multi = 1.15;
			else if (sti(rship.class) == 2 || sti(rship.class) == 1) multi = 1.2;
			rship.SpeedRate = stf(rship.SpeedRate) * multi;
		break;
		case "SUP_HULLKRENG": //Медное днище
			if (sti(rship.class) == 6 || sti(rship.class) == 5) multi = 1.03;
			else if (sti(rship.class) == 4 || sti(rship.class) == 3) multi = 1.05;
			else if (sti(rship.class) == 2 || sti(rship.class) == 1) multi = 1.07;
			rship.SpeedRate = stf(rship.SpeedRate) * multi;
		break;
		case "SUP_CAPACITY": //Увеличенный трюм
			if (sti(rship.class) == 6 || sti(rship.class) == 5) {multi = 1.2; multi2 = 0.9;}
			else if (sti(rship.class) == 4 || sti(rship.class) == 3) {multi = 1.15; multi2 = 0.93;}
			else if (sti(rship.class) == 2 || sti(rship.class) == 1) {multi = 1.1; multi2 = 0.95;}
			rship.Capacity = makeint(stf(rship.Capacity) * multi + 0.5);
			rship.HullArmor = makeint(stf(rship.HullArmor) * multi2 + 0.5);
		break;
		case "SUP_HAMMOCKS": //Гамаки
			if (sti(rship.class) == 6 || sti(rship.class) == 5) {multi = 1.25; multi2 = 0.87;}
			else if (sti(rship.class) == 4 || sti(rship.class) == 3) {multi = 1.18; multi2 = 0.91;}
			else if (sti(rship.class) == 2 || sti(rship.class) == 1) {multi = 1.11; multi2 = 0.95;}
			rship.MaxCrew = makeint(stf(rship.MaxCrew) * multi + 0.5);
			rship.Capacity = makeint(stf(rship.Capacity) * multi2 + 0.5);
		break;
		case "SUP_HELM": //Манёвренный штурвал
			if (sti(rship.class) == 6 || sti(rship.class) == 5) multi = 1.1;
			else if (sti(rship.class) == 4 || sti(rship.class) == 3) multi = 1.15;
			else if (sti(rship.class) == 2 || sti(rship.class) == 1) multi = 1.2;
			rship.TurnRate = stf(rship.TurnRate) * multi + 0.5;
		break;
		case "SUP_BRANDER": //Брандер
			rship.HP = makeint(stf(rship.HP) * 1.4 + 0.5);
			rship.SpeedRate = stf(rship.SpeedRate) * 1.4;
			rship.TurnRate = stf(rship.TurnRate) * 0.5 + 0.5;
			rship.MinCrew = makeint(stf(rship.MinCrew) * 0.2 + 0.5);
			rship.OptCrew = makeint(stf(rship.OptCrew) * 0.2 + 0.5);
			rship.MaxCrew = makeint(stf(rship.MaxCrew) * 0.2 + 0.5);
			rship.Capacity = makeint(stf(rship.Capacity) * 0.75 + 0.5);
		break;
		case "SUP_ADDITIONALARSENAL": //Дополнительный арсенал
			rship.HP = makeint(stf(rship.HP) * 0.8 + 0.5);
			rship.Capacity = makeint(stf(rship.Capacity) * 1.35 + 0.5);
		break;
		case "SUP_REINFORCEDCONSTRUCTION": //Укрепление трюма и конструкции
			if (CheckShipType(rship,"War")) rship.HP = makeint(stf(rship.HP) * 1.7 + 0.5);
			else rship.HP = makeint(stf(rship.HP) * 1.4 + 0.5);
			rship.Capacity = makeint(stf(rship.Capacity) * 0.8 + 0.5);
		break;
	}
	if (sti(rship.HP) != oldHP && CheckAttribute(rship, "index") && CheckAttribute(pchar, "Ship.Type") && sti(pchar.Ship.Type) == sti(rship.index))
	{
		pchar.ship.hp = GetCharacterShipHP(pchar);
	}
	rship.Price = makeint(sti(rship.Price) * 1.05);
}

void UpdateShipStatsFromUpgrades(ref rship)
{
	if (!CheckAttribute(rship, "Upgrade")) return;

	aref arUpgrades, arCur;
	makearef(arUpgrades, rship.Upgrade);
	int num = GetAttributesNum(arUpgrades);
	for (int i = 0; i < num; i++)
	{
		arCur = GetAttributeN(arUpgrades, i);
		CheckForShipStatsChange(rship, GetAttributeName(arCur));
	}
}

bool CheckShipType(ref rship, string shiptype)
{
	string type = "";
	if (rship.type.merchant == true && rship.type.war == true) type = "Universal";
	else if (rship.type.war == true && rship.type.merchant != true ) type = "War";
	else if (rship.type.merchant == true && rship.type.war != true) type = "Merchant";
	if (HasSubStr(shiptype,type)) return true;
	return false;
}

bool CheckForUpgrade(ref rship, string uptype)
{
	if (CheckAttribute(rship,"Upgrade."+uptype)) return true;
	return false;
}

int GetShipMastsQuantity(ref chr)
{
	if (!CheckAttribute(chr, "ship.type")) return 0;    
	int iShip = sti(chr.ship.type);
	if (iShip == SHIP_NOTUSED) return 0;
	if (CheckAttribute(chr,"Ship.MastsQty")) return sti(chr.Ship.MastsQty);
	else
	{
		if (CheckAttribute(chr,"Ship.Masts"))
		{
			aref arMasts;
			makearef(arMasts, chr.Ship.Masts);
			return GetAttributesNum(arMasts);			
		}
	}
	return 0;
}

int GetShipFallMastsQuantity(ref chr)
{
	int ret = 0;
	if (!CheckAttribute(chr, "ship.type")) return 0;    
	int iShip = sti(chr.ship.type);
	if (iShip == SHIP_NOTUSED) return 0;
	if (CheckAttribute(chr,"Ship.Masts"))
	{
		aref arMasts;
		makearef(arMasts, chr.Ship.Masts);
		int nMasts = GetAttributesNum(arMasts);
		for(int i = 0; i < nMasts; i++)
		{
			string sMast = GetAttributeName(GetAttributeN(arMasts, i));
			if(chr.Ship.Masts.(sMast) == 1) ret++;
		}
	}
	return ret;
}
