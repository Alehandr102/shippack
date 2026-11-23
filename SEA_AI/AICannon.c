#define MAX_CANNON_DAMAGE_DISTANCE	2.0 //дальность расчета урона по пушке, если снаряд попал ближе этого числа(в метрах?), то расчёт идёт

// LEO: 1 = высоты вертикального прицела (Y/DY по типам снарядов) берём АВТО из геометрии корабля (движок
//      считает при загрузке и пишет в aEnemy.Ship.AutoHeight.*; книппели по высоте парусов и т.д.).
//      0 = старое поведение, ручные значения из refShip.Height.* в Ships_init.c.
#define AUTO_FIRE_HEIGHT	1

// LEO: сужение ВЕРТИКАЛЬНОГО разброса прицела корпусных снарядов (ядра/бомбы/картечь) — против перелётов
//      на некоторых кораблях. Множитель к DY в Cannon_GetFireHeight (1.0 = как было; меньше = уже разлёт).
//      Книппели НЕ трогаем — им нужен разброс по всей высоте парусов. Тюнится без пересборки движка.
#define HULL_FIRE_DY_MULT	0.3

void DeleteCannonsEnvironment()
{
	DelEventHandler(CANNON_GET_RECHARGE_TIME, "Cannon_GetRechargeTime");
	DelEventHandler(CANNON_GET_FIRE_TIME, "Cannon_GetFireTime");
	DelEventHandler(CANNON_GET_FIRE_HEIGHT, "Cannon_GetFireHeight");
	DelEventHandler(CANNON_FIRE, "Cannon_FireCannon");
	DelEventHandler(CANNON_UNLOAD, "Cannon_UnloadBall");
	DelEventHandler(CANNON_LOAD, "Cannon_LoadBall");
	DelEventHandler(CANNON_RECALCULATE_PARAMETERS, "Cannon_RecalculateParametersEvent");
	DelEventHandler(CANNON_DAMAGE, "Cannon_DamageEvent");
}

void CreateCannonsEnvironment()
{
	SetEventHandler(CANNON_GET_RECHARGE_TIME, "Cannon_GetRechargeTime", 0);
	SetEventHandler(CANNON_GET_FIRE_TIME, "Cannon_GetFireTime", 0);
	SetEventHandler(CANNON_GET_FIRE_HEIGHT, "Cannon_GetFireHeight", 0);
	SetEventHandler(CANNON_FIRE, "Cannon_FireCannon", 0);
	SetEventHandler(CANNON_UNLOAD, "Cannon_UnloadBall", 0);
	SetEventHandler(CANNON_LOAD, "Cannon_LoadBall", 0);
	SetEventHandler(CANNON_RECALCULATE_PARAMETERS, "Cannon_RecalculateParametersEvent", 0);
	SetEventHandler(CANNON_DAMAGE, "Cannon_DamageEvent", 0);

	SendMessage(&AISea, "lff", AI_MESSAGE_CANNONS_PARAMS, MAX_CANNON_DAMAGE_DISTANCE, 1.0);	// fMaxCannonDamageDistance, fMaxCannonDamageRadiusPoint
}

bool Cannon_LoadBall()
{
	aref	aCharacter = GetEventData();

	int iBallType = sti(aCharacter.Ship.Cannons.Charge.Type);
	int iNumBalls = GetCargoGoods(aCharacter, iBallType);
 	// boal -->
	int iNumPowder = GetCargoGoods(aCharacter, GOOD_POWDER);
	if (iNumBalls > 0 && iNumPowder > 0)
	{
		AddCharacterGoodsCannon(aCharacter, iBallType, -1);
		AddCharacterGoodsCannon(aCharacter, GOOD_POWDER, -1);
		if (IsMainCharacter(aCharacter)) BI_UpdateLoadedProjectiles();
		// boal <--
		return true;
	}
	return false;
}

void Cannon_UnloadBall()
{
	aref aCharacter = GetEventData();
	if(CheckAttribute(aCharacter, "Ship.Cannons"))
	{
		AddCharacterGoodsCannon(aCharacter, sti(aCharacter.Ship.Cannons.Charge.Type), 1);
	}
	// boal -->
	AddCharacterGoodsCannon(aCharacter, GOOD_POWDER, 1);
	// boal <--
}

void AddCharacterGoodsCannon(aref aCharacter, int iGood, int iQuantity)
{
	string sGoodName = Goods[iGood].name;
	aCharacter.Ship.Cargo.Goods.(sGoodName) = sti(aCharacter.Ship.Cargo.Goods.(sGoodName)) + iQuantity;
	aCharacter.Ship.Cargo.RecalculateCargoLoad = true;
}

void Cannon_RecalculateParametersEvent()
{
	int iCharacterIndex = GetEventData();
	Cannon_RecalculateParameters(iCharacterIndex);
}

//отрабатывает каждый кадр для каждого корабля?!
void Cannon_RecalculateParameters(int iCharacterIndex)
{
	// LEO: движок шлёт CANNON_RECALCULATE_PARAMETERS per-frame по кораблям, в т.ч. на мёртвые/невалидные (-1)
	//      -> Characters[-1] + GetCharacter(-1) = спам "Incorrect Index" при стрельбе/выбивании пушек. Гвард -1.
	if (iCharacterIndex < 0) return;
	if (sti(Characters[iCharacterIndex].Ship.Cannons.Type) == CANNON_TYPE_NONECANNON) { return; }

	ref	rCharacter = GetCharacter(iCharacterIndex);
	ref	rCannon = GetCannonByType(sti(rCharacter.Ship.Cannons.Type));
	ref	rBall = GetGoodByType(sti(rCharacter.Ship.Cannons.Charge.Type));
	ref rShip = GetRealShip(sti(rCharacter.Ship.Type));
	int p = GetCharacterSPECIALSimple(rCharacter, SPECIAL_P)-3;
	if (p < 0) p = 0;
	if (CheckAttribute(rCharacter, "TmpPerks.LongRangeShoot"))
	{
		rCharacter.Ship.Cannons.SpeedV0 = stf(rCannon.SpeedV0) * stf(rBall.SpeedV0) * sqrt(AIShip_isPerksUse(rCharacter.TmpPerks.LongRangeShoot, 1.0, 1.15) + p * 0.03); //slib
	}
	else
	{
	    rCharacter.Ship.Cannons.SpeedV0 = stf(rCannon.SpeedV0) * stf(rBall.SpeedV0) * sqrt(1.0 + p * 0.03);
	}
	if (HasSubStr(rCharacter.id, " Fort Commander")) rCharacter.Ship.Cannons.SpeedV0 = stf(rCharacter.Ship.Cannons.SpeedV0)*1.5;
	float fFireAng = stf(rCannon.FireAngMax);
	rShip.Cannons.borts.cannonl.FireAngMax = fFireAng;
	rShip.Cannons.borts.cannonr.FireAngMax = fFireAng;
	rShip.Cannons.borts.cannonf.FireAngMax = fFireAng;
	rShip.Cannons.borts.cannonb.FireAngMax = fFireAng;
	// нужно при первой инициализации моря и если вдруг не сбросится
	rCharacter.Ship.Cannons.Borts.cannonl.FiredCount = 0;
	rCharacter.Ship.Cannons.Borts.cannonr.FiredCount = 0;
	rCharacter.Ship.Cannons.Borts.cannonf.FiredCount = 0;
	rCharacter.Ship.Cannons.Borts.cannonb.FiredCount = 0;		
}

float Cannon_GetFireHeight()
{
	aref aCharacter = GetEventData();
	aref aEnemy = GetEventData();

	ref rBallType = GetGoodByType(sti(aCharacter.Ship.Cannons.Charge.Type));
	string sBallName = rBallType.Name;

	float Y, DY;
	// LEO: предпочитаем АВТО-высоты из реальной геометрии (движок пишет в инстанс aEnemy.Ship.AutoHeight.*
	//      при загрузке: книппели по высоте парусов, ядра/бомбы по корпусу). Нет авто или тоггл выключен —
	//      ручные значения из типа корабля (refShip.Height.* в Ships_init.c).
	bool bUseAuto = false;
	if (AUTO_FIRE_HEIGHT)
	{
		if (CheckAttribute(aEnemy, "Ship.AutoHeight." + sBallName + ".Y")) bUseAuto = true;
	}
	if (bUseAuto)
	{
		Y  = stf(aEnemy.Ship.AutoHeight.(sBallName).Y);
		DY = stf(aEnemy.Ship.AutoHeight.(sBallName).DY);
	}
	else
	{
		ref rEnemyShip = GetRealShip(sti(aEnemy.Ship.Type));
		Y  = stf(rEnemyShip.Height.(sBallName).Y);
		DY = stf(rEnemyShip.Height.(sBallName).DY);
	}

	// LEO: корпусные снаряды (ядра/бомбы/картечь) — узкий вертикальный разброс против перелётов;
	//      книппели идут полным разбросом (нужна вся высота парусов).
	if (sti(aCharacter.Ship.Cannons.Charge.Type) != GOOD_KNIPPELS) DY = DY * HULL_FIRE_DY_MULT;

	return Y + (frnd()-0.5) * DY;
}

float Cannon_GetRechagreTimeBonusFromPeople(ref aCharacter, float fReloadTime)
{
	float crewQty  = GetCrewQuantity(aCharacter);
	float OptCrew  = GetOptCrewQuantity(aCharacter);
	float MaxCrew  = GetMaxCrewQuantity(aCharacter);
	float fMorale  = GetCharacterCrewMorale(aCharacter);

	if (crewQty > MaxCrew){ crewQty = MaxCrew;}// fix 14/03/05
	if (OptCrew > 0)
	{
		float  fExp;
		fExp = 0.3 + (GetCrewExp(aCharacter, "Cannoners") * crewQty) / (OptCrew * GetCrewExpRate()) * 1.0;

		if (fExp > 1){ fExp = 1;}
		fReloadTime = fReloadTime / fExp;
	}
	fReloadTime = fReloadTime * (1.0 + (1.0 - fMorale / MORALE_NORMAL) * 0.1);
	return fReloadTime;
}

// calculate recharge time for cannon
float Cannon_GetRechargeTime()
{
	aref	aCharacter = GetEventData();
	return Cannon_GetRechargeTimeValue(aCharacter);
}

// calculate delay before fire - в 10 раз быстрее чем addball
float Cannon_GetFireTime()
{
	aref aCharacter = GetEventData();
	ref refBaseShip = GetRealShip(sti(aCharacter.ship.Type));
	// make 10 seconds random delay between fire from fort cannons
	if (refBaseShip.Name == ShipsTypes[SHIP_FORT].Name) {
		return frnd() * 15.0 * stf(aCharacter.fort.cannons.quantity) / 75.0;// 5-10с при 75 пушках
	}

	string sFiredBort;
	if (!CheckAttribute(aCharacter, "Ship.firedbort")) {
		aCharacter.Ship.firedbort = "cannonl";
	}
	sFiredBort = aCharacter.Ship.firedbort;// время залпа считается после назначения этого аттрибута
	if (sFiredBort != "" && stf(aCharacter.ship.cannons.borts.(sFiredBort).ChargeRatio) < 1.0) {
		return 0.0;
	}
	float fDamageRatio = 1;
	if (CheckAttribute(aCharacter, "ship.cannons.borts."+sFiredBort+".DamageRatio")) {//it just works
		fDamageRatio = 1 - stf(aCharacter.ship.cannons.borts.(sFiredBort).DamageRatio);
	}
	aCharacter.Ship.Cannons.Borts.(sFiredBort).FiredCount = sti(aCharacter.Ship.Cannons.Borts.(sFiredBort).FiredCount) - 1;
	if (aCharacter.Ship.Cannons.Borts.(sFiredBort).FiredCount <= 0 || !CheckAttribute(aCharacter, "Ship.Cannons.Borts." + sFiredBort + ".FiredCount")) {// после конца расчета залпа становится 0
		aref arGunsnum;
		makearef(arGunsnum, aCharacter.ship.cannons.borts.(sFiredBort).damages);// TODO оптимизировать

		fDamageRatio = MakeInt(0.5 + makefloat(GetAttributesNum(arGunsnum)) * fDamageRatio);
		aCharacter.Ship.Cannons.Borts.(sFiredBort).FiredCount = MakeInt(fDamageRatio);
		aCharacter.Ship.Cannons.Borts.(sFiredBort).NumIntactCannons = MakeInt(fDamageRatio);
	}

	float fFireTimeMult = CalcShipCannonFireTimeMult(aCharacter, refBaseShip);
	float fRandomMult = 50.0 * fFireTimeMult;
	if(sti(aCharacter.Ship.Cannons.Borts.(sFiredBort).FiredCount) == sti(aCharacter.Ship.Cannons.Borts.(sFiredBort).NumIntactCannons))
	{
		// последняя пушка - от нее считается упреждение
		// от слишком больших значений будут баги 
		fRandomMult = 0.0;
		fFireTimeMult *= 0.15;
	}
	int iBortFired = (sFiredBort == "cannonr") || (sFiredBort == "cannonl");
	if (iBortFired) { // TODO оптимизировать
		float fShipCannonsQuantityMult = ShipCannonFireTimeBonusFromBaseShip(aCharacter, refBaseShip);
		fFireTimeMult *= fShipCannonsQuantityMult;// если дек больше двух то наоборот ускоряем залп
	}
	return fFireTimeMult * (stf(aCharacter.Ship.Cannons.Borts.(sFiredBort).FiredCount) + fRandomMult * iBortFired * (Random() - 0.5));

}

float ShipCannonFireTimeBonusFromSkill(aref aCharacter, ref refBaseShip)
{
	int iCannonType = sti(aCharacter.Ship.Cannons.Type);
	float fFireTimeMult = 1.5 - 1.5 * stf(aCharacter.TmpSkill.Cannons);// дефолт при 10 навыка и 10-м калибре с грузом меньше лайтвейта
	if (iCannonType >= CANNON_TYPE_CANNON_LBS8) { //пушка или кулеврина 
		iCannonType -= CANNON_TYPE_CANNON_LBS8;
	}
	fFireTimeMult += 0.1 * (MakeFloat(iCannonType) + 3.0);//штраф
	return fFireTimeMult;
}

// от команды
float ShipCannonFireTimeBonusFromPeople(aref aCharacter, ref refBaseShip)
{
	float fCrewQuality = GetCrewExp(aCharacter, "Cannoners") / GetCrewExpRate();
	fCrewQuality *= GetCrewQuantity(aCharacter) / stf(refBaseShip.OptCrew);
	fCrewQuality += 0.05; 
	if (fCrewQuality > 1.0) fCrewQuality = 1.0;
	return (2.0 - fCrewQuality);
}

//от свободного места на корабле
float ShipCannonFireTimeBonusFromCapacity(aref aCharacter, ref refBaseShip)
{
	float fCapacity = stf(refBaseShip.Capacity);
	fCapacity = 0.3 + (fCapacity - stf(aCharacter.Ship.Cargo.Load)) / fCapacity;//ошибки от отсутствия аттрибута нужны
	if (fCapacity < 1.0) fCapacity = 1.0;
	return fCapacity;
}

//от количества пушек на типе корабля
float ShipCannonFireTimeBonusFromBaseShip(aref aCharacter, ref refBaseShip)
{
	// Gregg: у безоружных типов (Boat, Leudo — CannonsQuantity=0 в Ships_init) деление роняло
	// «Divide by zero» из интерфейсной подсказки. Clampf режет в [0,1], поэтому при любом
	// количестве стволов <= 60 результат и так 1.0 — возвращаем его напрямую.
	float fCannonsQty = stf(refBaseShip.cannonsquantity);
	if (fCannonsQty < 1.0) return 1.0;
	return Clampf(60.0 / fCannonsQty);
}

//общий расчёт
float CalcShipCannonFireTimeMult(aref aCharacter, ref refBaseShip)
{
	float fFireTimeMult = ShipCannonFireTimeBonusFromSkill(aCharacter, refBaseShip);// дефолт при 10 навыка и 10-м калибре с грузом меньше лайтвейта

	if (fFireTimeMult < 1.0) {
		fFireTimeMult = 1.0;
	}

	fFireTimeMult *= ShipCannonFireTimeBonusFromPeople(aCharacter, refBaseShip);// мало влияния, в худшем случае 1.7, реалистично - 1.0-1.3
	fFireTimeMult /= ShipCannonFireTimeBonusFromCapacity(aCharacter, refBaseShip);// упрощенный расчет отдачи пушки

	if (fFireTimeMult > 2.15) {
		fFireTimeMult = 2.15;
	} // больше этого множителя - нежелательно
	return 0.15 * fFireTimeMult;
}

void Cannon_FireCannon()
{
	float fX, fY, fZ, fSpeedV0, fFireDirection, fFireHeightAngle, fCannonDirAng, fMaxFireDistance, fAngle;

	aref aCharacter = GetEventData();

	DeleteAttribute(aCharacter, "Sea_AI.Cannon.Charge"); //должно располагаться здесь, а не выше

	fX = GetEventData();
	fY = GetEventData();
	fZ = GetEventData();
	fSpeedV0 = GetEventData();
	fFireDirection = GetEventData();
	fFireHeightAngle = GetEventData();
	fCannonDirAng = GetEventData();
	fMaxFireDistance = GetEventData();
	fAngle = GetEventData();
	int iDeckShot = GetEventData();
	 // boal навел порядок по оптимизации
	Ball_AddBall(aCharacter, fX, fY, fZ, fSpeedV0, fFireDirection, fFireHeightAngle, fCannonDirAng, fMaxFireDistance, fAngle, iDeckShot);
}

// Damage 2 cannon from balls
float Cannon_DamageEvent()
{
	aref	aCharacter = GetEventData();
	float	fBallDamage = GetEventData();	// ball damage
	float	fCurDamage = GetEventData();	// current cannon damage (0.0 .. 1.0)
	float	fDistance = GetEventData();		// distance from ball 2 cannon
	float	x = GetEventData();				// x, y, z - cannon position
	float	y = GetEventData();
	float	z = GetEventData();
	if(CheckAttribute(aCharacter, "chr_ai.immortal") && sti(aCharacter.chr_ai.immortal) == 1)
	{
		return 0.0;
	}

	if(rand(2) == 0 && CheckAttribute(aCharacter,"perks.list.SeaWolf")) return 0.0;
	ref rShip = &RealShips[sti(aCharacter.Ship.Type)];
	if (CheckForUpgrade(&RealShips[sti(aCharacter.Ship.Type)],"SUP_LAFETTES") && rand(9)<4) return 0.0; //Усиленные лафеты
	if (IsEquipCharactersByItem(aCharacter, "talisman3")) return 0.0;
	ref rCannon = GetCannonByType(sti(aCharacter.Ship.Cannons.Type));

	float fMaxCHP = stf(rCannon.hp);
    fBallDamage *= 0.4 / fDistance;
    fCurDamage =  fCurDamage  + fBallDamage / fMaxCHP;  // TO_DO

	if (fCurDamage >= 1.0)
	{
		fCurDamage = 1.0;
		if (sti(rCannon.caliber) >= 42)
		{
			CreateBlast(x, y, z, 0.0, 0.0, "CANNONDESTR3");
		}
		else if (sti(rCannon.caliber) >= 20)
		{
			CreateBlast(x, y, z, 0.0, 0.0, "CANNONDESTR2");
		}
		else
		{
			CreateBlast(x, y, z, 0.0, 0.0, "CANNONDESTR1");
		}
		if (sti(InterfaceStates.OtherParticles))
		{
			CreateParticleSystem("blast_bomb_ptr", x, y, z, 0.0, 0.0, 0.0, 0);
			CreateParticleSystem("blast_inv_ext", x, y, z, 0.0, 0.0, 0.0, 0);
		}
		else CreateParticleSystem("blast_inv", x, y, z, 0.0, 0.0, 0.0, 0);
		Play3DSound("cannon_explosion", x, y, z);
		if (IsMainCharacter(aCharacter))
		{
		    Log_Info(XI_ConvertString("Cannon_DamageEvent"));
		    BI_UpdateCannons();
		}
		aCharacter.Ship.Cargo.RecalculateCargoLoad = true; // boal 27.07.06 пушки - груз
	}

	return fCurDamage;
}

// LEO: имя борта в атрибутах живёт в двух написаниях ("cannonr" и "rcannon"), плюс узла damages может не
// быть вовсе. Возвращает реальное имя борта или "" - записей о повреждениях у этого борта нет.
string Cannon_ResolveDamagedBort(ref rCharacter, string sBort)
{
	string sReal = sBort;
	if(sBort == "rcannon") sReal = "cannonr";
	if(sBort == "lcannon") sReal = "cannonl";
	if(sBort == "fcannon") sReal = "cannonf";
	if(sBort == "bcannon") sReal = "cannonb";
	if (CheckAttribute(rCharacter, "Ship.Cannons.Borts." + sReal + ".damages")) return sReal;
	if (CheckAttribute(rCharacter, "Ship.Cannons.Borts." + sBort + ".damages")) return sBort;
	return "";
}

// LEO: урон конкретного ствола. Запись заводит движок и ТОЛЬКО на повреждённую пушку (Cannon_DamageEvent),
// у целой её нет - поэтому голое чтение сыпало "missed attribute: cN" + "no rAP data" на каждый целый
// ствол каждый кадр, пока открыто меню корабля (в логе игрока 7300 строк). Нет записи = урона нет.
float Cannon_GetDamage(ref rCharacter, string sBortReal, string attr)
{
	if (!CheckAttribute(rCharacter, "Ship.Cannons.Borts." + sBortReal + ".damages." + attr)) return 0.0;
	return stf(rCharacter.Ship.Cannons.Borts.(sBortReal).damages.(attr));
}

int GetBortIntactCannonsNum(ref rCharacter, string sBort, int iNumCannonsOnBort)
{
	string sBort_real = Cannon_ResolveDamagedBort(rCharacter, sBort);
	if (sBort_real == "") return iNumCannonsOnBort; // повреждений на борту нет - целы все

	int iNumIntactCannons = 0;
	for (int i=0; i<iNumCannonsOnBort; i++)
	{
		if (Cannon_GetDamage(rCharacter, sBort_real, "c"+i) < 1.0) { iNumIntactCannons++; }
	}
	return iNumIntactCannons;
}

//возвращает усредненный полученный урон пушек борта
//0 - целые, 1 - отсутствие
float GetBortConditionAvg(ref rCharacter, string sBort, int iNumCannonsOnBort)
{
	string sBort_real = Cannon_ResolveDamagedBort(rCharacter, sBort);
	if (sBort_real == "") return 0.0; // LEO: повреждений нет = урон 0 (было 1.0, т.е. целый борт показывался как выбитый)

	float fDamage;
	int iNumIntactCannons = 0;
	float fCannonConditionAvg = 0;
	for (int i=0; i<iNumCannonsOnBort; i++)
	{
		fDamage = Cannon_GetDamage(rCharacter, sBort_real, "c"+i);
		if (fDamage >= 1.0) { continue; }
		iNumIntactCannons++;
		fCannonConditionAvg += fDamage;
	}
	if (iNumIntactCannons > 1) { 	fCannonConditionAvg /= iNumIntactCannons; }

	return fCannonConditionAvg;
}

//возвращает полученный урон пушки наиболее близкой к разрыву
//0 - целая, 1 - отсутствие
float GetBortConditionMin(ref rCharacter, string sBort, int iNumCannonsOnBort)
{
	string sBort_real = Cannon_ResolveDamagedBort(rCharacter, sBort);
	if (sBort_real == "") return 0.0; // LEO: повреждений нет = урон 0 (было 1.0, т.е. целый борт показывался как выбитый)

	float fDamage;
	float fCannonConditionMin = 0;
	for (int i=0; i<iNumCannonsOnBort; i++)
	{
		fDamage = Cannon_GetDamage(rCharacter, sBort_real, "c"+i);
		if (fDamage >= 1.0) { continue; }
		if (fDamage > fCannonConditionMin) { fCannonConditionMin = fDamage; }
	}
	return fCannonConditionMin;
}

//возвращает усредненный полученный урон во все пушки
//0 - целые, 1 - отсутствие
float GetAllCannonsConditionAvg(ref rCharacter)
{
    int iShip = sti(rCharacter.ship.type);
	ref refBaseShip = GetRealShip(iShip);
	float fDamage;
	string sBort;
	int iNumIntactCannons = 0;
	int	iNumCannonsOnBort = 0;
	float fCannonConditionAvg = 0;
	for (int j=0; j<4; j++) {
		switch(j)	{
			case 0:
				sBort = "cannonl";
			break;
			case 1:
				sBort = "cannonr";
			break;
			case 2:
				sBort = "cannonf";
			break;
			case 3:
				sBort = "cannonb";
			break;
		}
		iNumCannonsOnBort = sti(refBaseShip.(sBort));
		// LEO: борт мог быть записан вторым написанием ("lcannon" вместо "cannonl") - без этого урон
		// такого борта не находился и он считался целым. Нет узла вовсе - Cannon_GetDamage вернёт 0.
		string sBortReal = Cannon_ResolveDamagedBort(rCharacter, sBort);
		if (sBortReal == "") sBortReal = sBort;
		for (int i=0; i<iNumCannonsOnBort; i++)	{
			fDamage = Cannon_GetDamage(rCharacter, sBortReal, "c"+i);
			if (fDamage >= 1.0) { continue; }
			iNumIntactCannons++;
			fCannonConditionAvg += fDamage;
		}
	}
	if (iNumIntactCannons > 0) fCannonConditionAvg /= iNumIntactCannons; // LEO: гвард деления на ноль (все пушки выбиты)
	return fCannonConditionAvg;
}
