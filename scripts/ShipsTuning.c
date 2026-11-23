//0.HP.0.MAST.Speed.Turn.WAS.Capacity.Crew.Cannon
string g_ShipBermudeTuningGoods[10] = {"","Mahogany","","Planks","Silk","Linen","Cotton","Sandal","Leather","Ebony"};
string g_ShipBermudeTuningItems[10] = {"","jewelry17","","jewelry11","jewelry2","jewelry3","jewelry4","jewelry5","jewelry1","icollection"};

float ShipTuning_OriginalStat(ref rShip, string sStat)
{
	if (CheckAttribute(rShip, "Untuned." + sStat)) return stf(rShip.Untuned.(sStat));
	if (CheckAttribute(rShip, sStat)) return stf(rShip.(sStat));
	return 0.0;
}

void RepairSpecialShipMinCrew()
{
	// LEO: исправляем только прежние минимумы двух квестовых кораблей; прочий флот и тюнинги не пересчитываем.
	int iShips = GetArraySize(&RealShips);
	for (int i = 0; i < iShips; i++)
	{
		ref rShip = &RealShips[i];
		if (!CheckAttribute(rShip, "BaseType")) continue;
		if (!CheckAttribute(rShip, "OptCrew") || !CheckAttribute(rShip, "MinCrew")) continue;
		if (CheckAttribute(rShip, "isFort") || CheckAttribute(rShip, "Untuned.MinCrew")) continue;
		int iBase = sti(rShip.BaseType);
		if (iBase == SHIP_XEBECLIGHT && sti(rShip.OptCrew) == 140 && sti(rShip.MinCrew) == 24)
		{
			// LEO: сверяем собственные статы Марлина, а не сохранённый индекс, который мог перейти другому кораблю.
			if (ShipTuning_OriginalStat(rShip, "SpeedRate") != 18.5) continue;
			if (ShipTuning_OriginalStat(rShip, "WindAgainstSpeed") != 10.0) continue;
			if (ShipTuning_OriginalStat(rShip, "Capacity") != 1850.0) continue;
			if (ShipTuning_OriginalStat(rShip, "HP") != 2000.0) continue;
			rShip.MinCrew = 28;
		}
		if (iBase == SHIP_WH_CORVETTE_QUEST && sti(rShip.OptCrew) == 300 && sti(rShip.MinCrew) == 46)
		{
			// LEO: эти признаки оставляет капитальный ремонт; базовый Пёс и явно изменённый минимум не затрагиваются.
			if (!CheckAttribute(rShip, "HullArmor")) continue;
			if (sti(rShip.HullArmor) != 30) continue;
			if (!CheckAttribute(rShip, "Untuned.HP")) continue;
			if (!CheckAttribute(rShip, "Untuned.Capacity")) continue;
			if (!CheckAttribute(rShip, "Untuned.MaxCrew")) continue;
			rShip.MinCrew = 60;
		}
	}
}

void SetShipTuningF(ref chr, string sStat, float fBonus)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	float value = stf(rShip.(sStat));
	float defaultValue = value;
	if (CheckAttribute(rShip, "Untuned." + sStat))
	{
		defaultValue = stf(rShip.Untuned.(sStat));
	}
	else
	{
		rShip.Untuned.(sStat) = defaultValue;
	}
	value = value + defaultValue * fBonus;
	rShip.(sStat) = value;
	chr.ship.(sStat) = value;
}

void SetShipTuningI(ref chr, string sStat, float fBonus)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	int value = sti(rShip.(sStat));
	int defaultValue = value;
	if (CheckAttribute(rShip, "Untuned." + sStat))
	{
		defaultValue = sti(rShip.Untuned.(sStat));
	}
	else
	{
		rShip.Untuned.(sStat) = defaultValue;
	}
	value = value + makeint(defaultValue * fBonus);
	rShip.(sStat) = value;
	chr.ship.(sStat) = value;
}

void SetShipTuningAdditiveF(ref chr, string sStat, float fBonus)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	float value = stf(rShip.(sStat));
	if (!CheckAttribute(rShip, "Untuned." + sStat))
	{
		rShip.Untuned.(sStat) = value;
	}
	value = value + fBonus;
	rShip.(sStat) = value;
	chr.ship.(sStat) = value;
}

void SetShipTuningAdditiveI(ref chr, string sStat, int fBonus)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	int value = sti(rShip.(sStat));
	if (!CheckAttribute(rShip, "Untuned." + sStat))
	{
		rShip.Untuned.(sStat) = value;
	}
	value = value + fBonus;
	rShip.(sStat) = value;
	chr.ship.(sStat) = value;
}

void SetShipTuningMastMultiplier(ref chr, float fValue)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	float value = stf(rShip.MastMultiplier);
	if (!CheckAttribute(rShip, "Untuned.MastMultiplier"))
	{
		rShip.Untuned.MastMultiplier = value;
	}
	value = value - fValue;	
	rShip.MastMultiplier = value; 
	chr.ship.MastMultiplier = value; 
}

void SetShipBermudeTuningF(ref chr, string sStat)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	if (CheckAttribute(rShip, "Tuning." + sStat)) { return; }
	rShip.Tuning.(sStat) = true;
	float fUpgradeMul = SHIP_STAT_UPGRADE_BERMUDE;
	if (sStat == "SpeedRate") {fUpgradeMul /= 2;}// sqrt(1+0.2) ~= 1+0.2/2 	
	SetShipTuningF(chr, sStat, fUpgradeMul);
}

void SetShipBermudeTuningI(ref chr, string sStat)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	if (CheckAttribute(rShip, "Tuning." + sStat)) { return; }
	rShip.Tuning.(sStat) = true;
	float fUpgradeMul = SHIP_STAT_UPGRADE_BERMUDE;
	if (sStat == "MaxCrew") {fUpgradeMul /= 2;}// sqrt(1+0.2) ~= 1+0.2/2 
	SetShipTuningI(chr, sStat, fUpgradeMul);
}

//увеличение прочности мачт
void SetShipBermudeTuningMastMultiplier(ref chr)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	if (CheckAttribute(rShip, "Tuning.MastMultiplier")) { return; }
	rShip.Tuning.MastMultiplier = true;
	SetShipTuningMastMultiplier(chr, 0.3);
}

//увеличение калибра и количества пушек
void SetShipBermudeTuningMaxCaliber(ref chr)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	if (CheckAttribute(rShip, "Tuning.MaxCaliber") || (sti(rShip.MaxCaliber) >= 36)) { return; }

	int iCaliber = sti(rShip.MaxCaliber);
	switch(iCaliber)
	{
		case 8: iCaliber = 10; break;
		case 10: iCaliber = 12; break;
		case 12: iCaliber = 16; break;
		case 16: iCaliber = 20; break;
		case 20: iCaliber = 24; break;
		case 24: iCaliber = 28; break;
		case 28: iCaliber = 32; break;
		case 32: iCaliber = 36; break;
	}
	rShip.MaxCaliber = iCaliber;
	rShip.Tuning.Cannon = true;
}

//увеличение трюма
void SetShipBermudeTuningCapacity(ref chr)
{
	SetShipBermudeTuningI(chr, "Capacity");
}

//увеличение скорости
void SetShipBermudeTuningSpeedRate(ref chr)
{
	SetShipBermudeTuningF(chr, "SpeedRate");
}

//увеличение команды
void SetShipBermudeTuningMaxCrew(ref chr)
{
	SetShipBermudeTuningI(chr, "MaxCrew");
}

//увеличение маневренности
void SetShipBermudeTuningTurnRate(ref chr)
{
	SetShipBermudeTuningF(chr, "TurnRate");
}

//увеличение ХП
void SetShipBermudeTuningHP(ref chr)
{
	SetShipBermudeTuningI(chr, "HP");
}

//увеличение бейдевинда
void SetShipBermudeTuningWindAgainstSpeed(ref chr)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	if (CheckAttribute(rShip, "Tuning.WindAgainstSpeed")) { return; }
	rShip.Tuning.WindAgainstSpeed = true;
	SetShipTuningF(chr, "WindAgainstSpeed", 0.5 / stf(rShip.Class)); // странно, но пока оставим
}

//увеличение брони
void SetShipBermudeTuningHullArmor(ref chr)
{
	ref rShip = &RealShips[sti(chr.ship.type)];
	if (CheckAttribute(rShip, "Tuning.HullArmor")) { return; }
	rShip.Tuning.HullArmor = true;
	switch (sti(rShip.Class))
	{
		case 7: rShip.HullArmor = 7; break;
		case 6: rShip.HullArmor = 11; break;
		case 5: rShip.HullArmor = 15; break;
		case 4: rShip.HullArmor = 20; break;
		case 3: rShip.HullArmor = 26; break;
		case 2: rShip.HullArmor = 32; break;
		case 1: rShip.HullArmor = 38; break;
	}
}

//генератор "отбермуженных" статов для кораблей с ручным рандомом
void SetBermudeTuningShipStatesRandom(ref chr, int iRand)
{
	int iType = GetCharacterShipType(chr);
	if(iType == SHIP_NOTUSED)
	{
		return; //нет корабля - отсекаем
	}
	ref rShip = GetRealShip(iType);

	if(rShip.class == 7) return;	//мелкие корабли отсекаем

	if(rand(iRand) == 0) SetShipBermudeTuningMaxCaliber(chr);
	if(rand(iRand) == 0) SetShipBermudeTuningMastMultiplier(chr);
	if(rand(iRand) == 0) SetShipBermudeTuningHullArmor(chr);		//увеличение брони
}

// генератор "отбермуженных" статов для кораблей
void SetBermudeTuningShipStates(ref chr)
{
	SetBermudeTuningShipStatesRandom(chr, 30);
}

//установка назначенных "отбермуженных" статов для корабля
void SetBermudeTuningStates2Ship(ref chr, bool MaxCaliber, bool Capacity, bool SpeedRate, bool MaxCrew, bool TurnRate, bool HP, bool MastMulti, bool WAS)
{
	int iType = GetCharacterShipType(chr);
	if(iType == SHIP_NOTUSED)
	{
		return; //нет корабля - отсекаем
	}

	if (MaxCaliber)	SetShipBermudeTuningMaxCaliber(chr);		//увеличение калибра
	if (Capacity)	SetShipBermudeTuningCapacity(chr);			//увеличение трюма
	if (SpeedRate)	SetShipBermudeTuningSpeedRate(chr);		//увеличение скорости
	if (MastMulti)	SetShipBermudeTuningMastMultiplier(chr);	//увеличение прочности мачт
	if (MaxCrew)	SetShipBermudeTuningMaxCrew(chr);			//увеличение команды
	if (TurnRate)	SetShipBermudeTuningTurnRate(chr);			//увеличение маневренности
	if (HP)			SetShipBermudeTuningHP(chr);				//увеличение ХП
	if (WAS)		SetShipBermudeTuningWindAgainstSpeed(chr);	//увеличение бейдевинда
}
