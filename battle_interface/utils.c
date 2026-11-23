native float funcGetSailSpeed(int holeQ,int holeMax,float sailPow);
native float GetSailSpeed(int holeQ, int holeMax, float maxSpeed, float fSailHoleDepend);
native int RandomHole2Sail(int chrIdx, string reyName, int groupNum, int maxHole, int holeData, int addHoleQ);
native int DeleteOneSailHole(int chrIdx, string groupName, string reyName, int holeMask, int deleteHoleQ );
native string GetAssembledString(string formatStr, aref arDataBase);
native int ShipSailState(int chrIdx);

#libriary "script_rigging_files"

//==========================================================
//   Секция починки при использовании специальных умений.
//==========================================================
#define BI_SLOW_REPAIR_PERCENT	1.5
#define BI_SLOW_REPAIR_PERIOD	3000
#define BI_SLOW_REPAIR_CAP		20.0	// LightRepair (лёгкий ремонт) чинит не выше 20%
#define BI_FAST_REPAIR_PERCENT	1.0
#define BI_FAST_REPAIR_SAIL		3.0
#define BI_FAST_REPAIR_PERIOD	50
// LEO рефактор быстрой починки (InstantRepair): именованные константы вместо магических чисел
#define BI_FAST_SKILL_STEP		20		// каждые 20 ед. навыка Ремонт (начиная с 41) дают +1 к темпу
#define BI_FAST_CARPENTER_MUL	1.1		// перк Плотник: +10% к эффекту починки
#define BI_FAST_REPAIR_HULLMAT	1.0		// расход ДОСОК за списание (корпус); дробные значения ОК
#define BI_FAST_REPAIR_SAILMAT	3.0		// расход ПАРУСИНЫ за списание (паруса), отдельно от досок; дробные значения ОК
#define BI_FAST_BUILDER_MAT		0.9		// перк Кораблестроитель: множитель расхода материала (0.9 = −10%)
#define BI_FAST_SF_DIV			3.0		// Стальной Флот: эффект починки /3 (и расход материала ×2)
#define BI_FAST_SAIL_EPS		0.000003 // порог «починка парусов реально прошла за тик» (float-шум движка)

#event_handler("evntActionRepair","procActionRepair");

void ActivateSpecRepair(ref chref,int repairType)
{
	if(repairType==0)
	{	PostEvent("evntActionRepair",BI_SLOW_REPAIR_PERIOD,"ll",sti(chref.index),0);
	}
	else
	{	chref.ship.RepairS = 0.0; // LEO: свежий старт накопителя парусов на каждую активацию (без переноса/банковки между активациями)
		PostEvent("evntActionRepair",BI_FAST_REPAIR_PERIOD,"llff",sti(chref.index),1, 0.0,0.0);
	}
}

void procActionRepair()
{
	int chrIdx = GetEventData();
	int eRepType = GetEventData();
	float fMaterialH = 0.0;
	float fMaterialS = 0.0;
	if (eRepType != 0)
	{
		fMaterialH = GetEventData(); // вестигиальные параметры события (всегда 0) — читаем для выравнивания потока
		fMaterialS = GetEventData();
	}

	if (chrIdx < 0) return;
	if (!bSeaActive) return;
	ref chref = GetCharacter(chrIdx);

	// LEO: гейт IsCompanion снят — авто-перки чинят и ВРАЖЕСКИЙ ИИ, и компаньонов. Событие шлёт только
	//   хоткей ГГ или авто-перк ИИ (Ship_AI_AutoUsePerks), поэтому чужие/случайные корабли не чинятся.
	if (LAi_IsDead(chref)) return;

	// во время абордажа (пауза боя) — просто переотправить событие, ничего не чиня
	if (bAbordageStarted)
	{
		if (eRepType == 0)
			PostEvent("evntActionRepair", BI_FAST_REPAIR_PERIOD, "ll", chrIdx, 0);
		else
			PostEvent("evntActionRepair", BI_FAST_REPAIR_PERIOD, "llff", chrIdx, eRepType, fMaterialH, fMaterialS);
		return;
	}

	if (eRepType == 0)
		SlowRepair_Tick(chrIdx); // LightRepair — лёгкий ремонт
	else
		FastRepair_Tick(chrIdx); // InstantRepair — быстрая починка
}

//=====================================================
// LightRepair (лёгкий ремонт): медленно чинит корпус/паруса/мачты до 20%, без материалов.
//=====================================================
void SlowRepair_Tick(int chrIdx)
{
	ref chref = GetCharacter(chrIdx);
	float hpp = GetHullPercent(chref);
	float spp = GetSailPercent(chref);
	float fRepair;
	MastLightRepairPerSec(chref); // чиним и мачты
	if (hpp < BI_SLOW_REPAIR_CAP)
	{
		fRepair = BI_SLOW_REPAIR_CAP - hpp;
		if (fRepair > BI_SLOW_REPAIR_PERCENT) fRepair = BI_SLOW_REPAIR_PERCENT;
		ProcessHullRepair(chref, fRepair);
	}
	if (spp < BI_SLOW_REPAIR_CAP)
	{
		fRepair = BI_SLOW_REPAIR_CAP - spp;
		if (fRepair > BI_SLOW_REPAIR_PERCENT) fRepair = BI_SLOW_REPAIR_PERCENT;
		ProcessSailRepair(chref, fRepair);
	}
	if (CheckOfficersPerk(chref, "LightRepair"))
		PostEvent("evntActionRepair", BI_SLOW_REPAIR_PERIOD, "ll", chrIdx, 0);
}

//======================================================
// InstantRepair (быстрая починка): корпус (доски) и паруса (парусина) до InstantRepairRATE %.
// Один тик; пока перк активен и есть что чинить — переотправляет себя каждые BI_FAST_REPAIR_PERIOD.
//======================================================
void FastRepair_Tick(int chrIdx)
{
	ref chref = GetCharacter(chrIdx);
	// FPS-компенсация: событие ждёт период, но срабатывает на ближайшем кадре -> фактический интервал = период + ~1 кадр.
	// Масштаб (кадр+период)/период возвращает целевой объём починки независимо от FPS (идея камрада).
	float fpsScale = makefloat(GetDeltaTime() + BI_FAST_REPAIR_PERIOD) / makefloat(BI_FAST_REPAIR_PERIOD);
	bool sf = S_armor(chref); // Стальной Флот
	int rate = FastRepair_SkillRate(chref);

	// значения ДО починки этого тика — по ним решаем, продолжать ли цикл (как в исходнике)
	float hpp = GetHullPercent(chref);
	float spp = GetSailPercent(chref);

	// LEO: гейт по ПЛАТЁЖЕСПОСОБНОСТИ, а не по «есть хоть что-то в трюме»: на остатке меньше цены тика
	//   починка шла бы даром. Этими же флагами решаем, перезапускать ли цикл, - иначе с одной доской
	//   в трюме событие перепосылалось бы вечно вхолостую.
	bool bBuilder = CheckOfficersPerk(chref, "Builder");
	bool bDoHull = false;
	if (hpp < InstantRepairRATE) bDoHull = FastRepair_CanPay(chref, true, bBuilder, sf);
	bool bDoSail = false;
	if (spp < InstantRepairRATE) bDoSail = FastRepair_CanPay(chref, false, bBuilder, sf);
	if (bDoHull) FastRepair_Hull(chref, rate, fpsScale, sf);
	if (bDoSail) FastRepair_Sail(chref, rate, fpsScale, sf);

	// LEO: опыт за быструю починку (перк InstantRepair) — ВСЕМ, кто юзает перк (ГГ, компаньоны, вражеский ИИ),
	// LEO: пропорционально фактически починенному за тик (шкала обычного ремонта). Получатель — плотник ЭТОГО
	// LEO: корабля (если есть в свите — фактически только у ГГ), иначе капитан корабля.
	// LEO: тики копятся в одну строку exp-лога (AddCharacterExpToSkill суммирует по навыку) -> не спамит.
	float dHull = GetHullPercent(chref) - hpp;
	float dSail = GetSailPercent(chref) - spp;
	int iCls = GetCharacterShipClass(chref);
	float fExp = 0.0;
	if (dHull > 0.0) fExp = fExp + dHull * (7-iCls) / 2.5;
	if (dSail > 0.0) fExp = fExp + dSail * (7-iCls) / 3.5;
	if (fExp > 0.0)
	{
		// LEO: опыт напрямую персонажу корабля (на корабле ГГ — самому ГГ). Плотник получит свою долю
		//      через перк «обмен опытом», поэтому на него отдельно не роутим.
		AddCharacterExpToSkill(chref, "Repair", fExp);
	}

	// продолжать, пока перк активен, есть что чинить и ЧЕМ чинить (по состоянию ДО этого тика)
	if (CheckOfficersPerk(chref, "InstantRepair"))
	{
		bool again = false;
		if (bDoHull) again = true;
		if (bDoSail) again = true;
		if (again)
			PostEvent("evntActionRepair", BI_FAST_REPAIR_PERIOD, "llff", chrIdx, 1, 0.0, 0.0);
	}
}

// Темп быстрой починки: каждые BI_FAST_SKILL_STEP ед. навыка Ремонт (с 41) +1, +1 за плотника-пассажира, минимум 1.
int FastRepair_SkillRate(ref chref)
{
	int rate = GetSummonSkillFromName(chref, SKILL_REPAIR) - 1;
	rate /= BI_FAST_SKILL_STEP;
	if (rate == 0) rate++;
	if (chref.Fellows.Passengers.carpenter > 0) rate++;
	return rate;
}

// Починка КОРПУСА за тик (доски). Эффект: темп × Плотник(+10%) ÷ СФ(3). Материал — через
// FastRepair_SpendMaterial; что досок хватает на весь тик, проверил FastRepair_Tick.
void FastRepair_Hull(ref chref, int rate, float fpsScale, bool sf)
{
	bool bBuilder = CheckOfficersPerk(chref, "Builder");
	float fRepair = makefloat(rate * BI_FAST_REPAIR_PERCENT) * fpsScale;
	if (CheckOfficersPerk(chref, "Carpenter")) fRepair *= BI_FAST_CARPENTER_MUL;
	if (sf) fRepair /= BI_FAST_SF_DIV;
	FastRepair_SpendMaterial(chref, true, bBuilder, sf);
	ProcessHullRepairDigital(chref, fRepair);
}

// Починка ПАРУСОВ за тик (парусина). Темп ÷ класс корабля (1-4 кл. медленнее) ÷ макс.SP, × Плотник, ÷ СФ.
// У крупных судов прирост бюджета за тик МАЛ относительно урона одного паруса -> нужен НАКОПИТЕЛЬ
// (иначе целую дыру за тик не залатать). Копим ЧИСТЫЙ бюджет в ship.RepairS; возврат ProcessSailRepairFast
// НЕ переносим — он с корректировками (может уйти в минус и отравить накопитель). Прогресс детектим по
// реальному росту ship.sp (ProcessSailRepairFast пересчитывает его, см. CalculateShipSP в конце функции).
// Парусина списывается за ЗАЛАТАННУЮ ДЫРУ, а не за тик; что её хватит, проверил FastRepair_Tick.
void FastRepair_Sail(ref chref, int rate, float fpsScale, bool sf)
{
	bool bBuilder = CheckOfficersPerk(chref, "Builder");
	ref rBaseShip = &ShipsTypes[sti(RealShips[sti(chref.Ship.Type)])];
	float classSlow = makefloat(GetCharacterShipClass(chref));
	if (classSlow < 5.0) classSlow = 6.0 - classSlow; else classSlow = 1.0; // 1-й класс в 5 раз медленнее

	if(!CheckAttribute(chref, "chref.ship.RepairS"))
		chref.ship.RepairS = 0.01;

	// прирост бюджета починки за тик
	float inc = (BI_FAST_REPAIR_PERCENT * rate * BI_FAST_REPAIR_SAIL * (1 + 0.1*CheckOfficersPerk(chref, "Carpenter")) * fpsScale) / (stf(rBaseShip.SP) * classSlow);
	if (sf) inc /= BI_FAST_SF_DIV;

	// накопленный бюджет (чистый, без отравления возвратом); clamp >=0 на всякий случай
	float budget = 0.0;
	if (CheckAttribute(chref, "ship.RepairS")) budget = stf(chref.ship.RepairS);
	if (budget < 0.0) budget = 0.0;
	budget += inc;
	if (budget <= 0.0) return;

	float spBefore = 0.0;
	if (CheckAttribute(chref, "ship.sp")) spBefore = stf(chref.ship.sp);
	ProcessSailRepairFast(chref, budget); // возврат игнорируем (ненадёжен)
	float spAfter = stf(chref.ship.sp);   // пересчитан внутри ProcessSailRepairFast

	if (spAfter > spBefore) // дыра(ы) реально починены за этот тик
	{
		chref.ship.RepairS = 0.0; // бюджет израсходован -> сброс накопителя
		FastRepair_SpendMaterial(chref, false, bBuilder, sf);
	}
	else
		chref.ship.RepairS = budget; // прогресса нет -> копим дальше до целой дыры
}

// Сколько единиц материала стоит ЭТОТ тик: корпус=доски (BI_FAST_REPAIR_HULLMAT) / паруса=парусина
// (BI_FAST_REPAIR_SAILMAT). Дробное накопление «долга» в ship.MatDelta(S) -> списываем целые единицы.
// Кораблестроитель множит расход на BI_FAST_BUILDER_MAT, Стальной Флот удваивает.
// bCommit=false - только посчитать, долг не трогаем (нужно для проверки платёжеспособности).
int FastRepair_MaterialUnits(ref chref, bool isHull, bool builder, bool sf, bool bCommit)
{
	float matBase = BI_FAST_REPAIR_SAILMAT;
	if (isHull) matBase = BI_FAST_REPAIR_HULLMAT;
	if (builder) matBase *= BI_FAST_BUILDER_MAT; // Кораблестроитель -10%

	float debt = 0.0;
	if (isHull) { if (CheckAttribute(chref, "ship.MatDelta"))  debt = stf(chref.ship.MatDelta);  }
	else        { if (CheckAttribute(chref, "ship.MatDeltaS")) debt = stf(chref.ship.MatDeltaS); }
	debt += matBase;
	int units = 0;
	if (debt >= 1.0) { units = makeint(debt); debt -= units; }
	if (bCommit)
	{
		if (isHull) chref.ship.MatDelta = debt; else chref.ship.MatDeltaS = debt;
	}
	if (sf) units *= 2;
	return units;
}

// LEO: хватит ли материала оплатить этот тик ЦЕЛИКОМ. Бесплатной починки нет: не оплатил - не чинишься.
//   Ноль единиц = в этот тик списывать нечего (дробный долг ещё копится) - это не отказ.
bool FastRepair_CanPay(ref chref, bool isHull, bool builder, bool sf)
{
	int units = FastRepair_MaterialUnits(chref, isHull, builder, sf, false);
	if (units <= 0) return true;
	int goodId = GOOD_PLANKS;
	if (!isHull) goodId = GOOD_SAILCLOTH;
	return GetCargoGoods(chref, goodId) >= units;
}

// Списать материал за тик. Возвращает, сколько единиц реально ушло из трюма.
int FastRepair_SpendMaterial(ref chref, bool isHull, bool builder, bool sf)
{
	int units = FastRepair_MaterialUnits(chref, isHull, builder, sf, true);
	if (units <= 0) return 0;
	int goodId = GOOD_PLANKS;
	if (!isHull) goodId = GOOD_SAILCLOTH;
	int have = GetCargoGoods(chref, goodId);
	// LEO: страховка от минуса - запись шла без ограничения снизу, и счётчик уходил в отрицательные
	//   значения (репорт: парусина -2), а вместе с ним врал и вес груза. Платёжеспособность проверяет
	//   FastRepair_CanPay ДО починки, так что сюда мы попадаем уже с оплаченным тиком.
	if (units > have) units = have;
	if (units <= 0) return 0;
	string gname = Goods[goodId].Name;
	chref.Ship.Cargo.Goods.(gname) = have - units;
	chref.Ship.Cargo.Load = sti(chref.Ship.Cargo.Load) - sti(Goods[goodId].Weight)*units;
	return units;
}
//============================================================

float GetAllSailsDamagePercent(ref chref)
{
	float SailPow = 0.0;
	float SailDmg = 0.0;
	
	if(!IsEntity(&worldMap)) GetSailStatus(GetCharacterIndex(chref.id));
	
	aref arRoot, arGroup, arSail;
	int i,j,rq,gq;
	
	if(IsEntity(&worldMap))
	{
		if(CheckAttribute(chref,"ship.sailstatus.sailpow")) return makefloat(chref.ship.sailstatus.sailpow);
		else 												return 100.0;
	}
	
	if(!CheckAttribute(chref,"ship.sailstatus")) 
	{
		SailPow = 100.0;
		chref.ship.sailstatus.sailpow = SailPow;
		chref.ship.sailstatus.saildmg = SailDmg;
		return SailPow = 0.0;
	}	
	makearef(arRoot, chref.ship.sailstatus);
	rq = GetAttributesNum(arRoot);
	for(i=0; i<rq; i++)
	{
		arGroup = GetAttributeN(arRoot, i);
		gq = GetAttributesNum(arGroup);
		for(j=0; j<gq; j++)
		{
			arSail = GetAttributeN(arGroup, j);
			if (CheckAttribute(arSail, "sp") && CheckAttribute(arSail, "hc") && CheckAttribute(arSail, "mhc")) 
			{
				float fSailPow = GetCharacterShipSP(chref) * stf(arSail.sp);
				float fSailDmg = GetNeedDmgFromHole(sti(arSail.hc),fSailPow, sti(arSail.mhc));
				SailPow += fSailPow;
				SailDmg += fSailDmg;
			}
		}
	}
	if (SailPow > 100.0) SailPow = 100.0;
	chref.ship.sailstatus.sailpow = SailPow;
	chref.ship.sailstatus.saildmg = SailDmg;
		
	return SailPow;
}

//============================================================
//	Починка заданного количества процентов повреждения корпуса
// (возвращает число реально починеных процентов,
//  материалы не использует, пробоины в корпусе удаляет)
//============================================================
float ProcessHullRepair(ref chref,float repPercent)
{
	float dmg = 100.0 - GetHullPercent(chref);
	if(dmg==0.0) return 0.0;
	if(repPercent>dmg) repPercent=dmg;
	int blotsQuantity = GetBlotsQuantity(chref);
	int repBlots = makeint(blotsQuantity*repPercent/dmg);
	DeleteBlots(chref,repBlots);
	float baseHP = makefloat(GetCharacterShipHP(chref));
	chref.ship.HP = baseHP+(repPercent-dmg)*baseHP/100.0;
	if(sti(chref.ship.HP) > baseHP)
	{
		chref.ship.HP = baseHP;
	}
	return repPercent;
}

//	Починка заданного количества единиц повреждения корпуса - bestreducer
float ProcessHullRepairDigital(ref chref,float Digit)
{
	int baseHP = GetCharacterShipHP(chref);
	int dmg = baseHP - sti(chref.ship.HP);
	if(dmg==0.0) return 0.0;
	if(Digit>dmg) Digit=dmg;
	int blotsQuantity = GetBlotsQuantity(chref);
	int repBlots = makeint(blotsQuantity*Digit/dmg);
	DeleteBlots(chref,repBlots);
	chref.ship.HP = makefloat(baseHP+Digit-dmg);
	return Digit;
}
//============================================================
//	Починка заданного количества процентов повреждения парусов
// (возвращает число реально починеных процентов,
//  материалы не использует, дырки залатывает)
//============================================================
float ProcessSailRepair(ref chref, float repPercent)
{
	float dmg = 100.0-GetSailPercent(chref);
	if(dmg==0.0) return 0.0;
	if(repPercent>dmg) repPercent=dmg;

	float fMakeRepair = repPercent;
	int i,j,rq,gq;
	aref arRoot,arGroup,arSail;
	string tmpstr;
	makearef(arRoot, chref.ship.sails);
	rq = GetAttributesNum(arRoot);

	for(i=0; i<rq; i++)
	{
		arGroup = GetAttributeN(arRoot, i);
		gq = GetAttributesNum(arGroup);
		for(j=0; j<gq; j++)
		{
			arSail = GetAttributeN(arGroup, j);

			if( CheckAttribute(arSail,"mastFall") )
			{
				tmpstr = "ship.masts."+arSail.mastFall;
				if( CheckAttribute(chref,tmpstr) && stf(chref.(tmpstr))>=1.0 )	{continue;}
			}
			fMakeRepair -= OneSailDmgRepair(chref,arGroup,arSail,fMakeRepair);
			if(fMakeRepair<=0.0) {break;}
			if(!CheckAttribute(chref,"ship.sails")) break;
			if(GetAttributesNum(arRoot)!=rq) break;
			if(GetAttributesNum(arGroup)!=gq)
			{
				gq = GetAttributesNum(arGroup);
				j--;
			}
		}
		if(fMakeRepair<=0.0) {break;}
		if(!CheckAttribute(chref,"ship.sails")) break;
		if(GetAttributesNum(arRoot)!=rq)
		{
			rq = GetAttributesNum(arRoot);
			i--;
		}
	}
	chref.ship.sp = CalculateShipSP(chref);
	return repPercent;
}

// Починка заданного количества дырок - bestreducer
float ProcessSailRepairFast(ref chref, float fMakeRepair)
{
	float dmg = 100.0-GetSailPercent(chref);
	int i,j,rq,gq;
	aref arRoot,arGroup,arSail;
	string tmpstr;
	makearef(arRoot, chref.ship.sails);
	rq = GetAttributesNum(arRoot);
	for(i=0; i<rq; i++)
	{
		arGroup = GetAttributeN(arRoot, i);
		gq = GetAttributesNum(arGroup);
		for(j=0; j<gq; j++)
		{
			arSail = GetAttributeN(arGroup, j);
			if( CheckAttribute(arSail,"mastFall") )
			{
				tmpstr = "ship.masts."+arSail.mastFall;
				if( CheckAttribute(chref,tmpstr) && stf(chref.(tmpstr))>=1.0 )
					{continue;}
			}
				if (!CheckAttribute(arSail, "dmg") || !CheckAttribute(arSail, "sp"))
				{
					continue; // fix boal 18.08.06
				}
				float fSailDmg = stf(arSail.dmg);
				float sailDmgMax = GetCharacterShipSP(chref) * stf(arSail.sp);
				if(fMakeRepair<=0.0)
					break;
				if (fSailDmg <= 0.0 || !CheckAttribute(arSail, "hd"))
					continue;  // fix boal 14.09.06

				if (fMakeRepair>=fSailDmg)
				{
					DeleteOneSailHole( sti(chref.index), GetAttributeName(arSail), GetAttributeName(arGroup), sti(arSail.hd), sti(arSail.hc) );
					DeleteAttribute(arSail, "dmg");
					fMakeRepair -= fSailDmg;
					continue;
				}

				fSailDmg -= fMakeRepair;
				int iAfterHole = GetNeedHoleFromDmg( fSailDmg, sailDmgMax, sti(arSail.mhc) );
				fMakeRepair += (MakeFloat(iAfterHole)/stf(arSail.mhc))*sailDmgMax - fSailDmg/stf(arSail.sp);//получение значения починки после починки движком
				if( sti(arSail.hc) > iAfterHole )
				{
					arSail.hd = DeleteOneSailHole( sti(chref.index), GetAttributeName(arSail), GetAttributeName(arGroup), sti(arSail.hd), sti(arSail.hc)-iAfterHole );
					arSail.hc = iAfterHole;
				}
				arSail.dmg = fSailDmg;
			if(fMakeRepair<=0.0) {break;}
			if(!CheckAttribute(chref,"ship.sails")) break;
			if(GetAttributesNum(arRoot)!=rq) break;
			if(GetAttributesNum(arGroup)!=gq)
			{
				gq = GetAttributesNum(arGroup);
				j--;
			}
		}
		if(fMakeRepair<=0.0) {break;}
		if(!CheckAttribute(chref,"ship.sails")) break;
		if(GetAttributesNum(arRoot)!=rq)
		{
			rq = GetAttributesNum(arRoot);
			i--;
		}
	}
	chref.ship.sp = CalculateShipSP(chref);
	return fMakeRepair;
}

//=========================================================================
//	Починка заданного количества процентов повреждения конкретного паруса
//=========================================================================
float OneSailDmgRepair(ref chref, aref arSailNode, aref arSail, float fDmgRepair)
{
	if(fDmgRepair<=0.0) return 0.0;
	if (!CheckAttribute(arSail, "dmg")) return 0.0; // fix boal 18.08.06
	float fSailDmg = stf(arSail.dmg);

	if (fSailDmg <= 0.0 || !CheckAttribute(arSail, "hd"))  return 0.0;  // fix boal 14.09.06

	if (fDmgRepair>=fSailDmg)
	{
		DeleteOneSailHole( sti(chref.index), GetAttributeName(arSail), GetAttributeName(arSailNode), sti(arSail.hd), sti(arSail.hc) );
		DeleteAttribute(arSailNode,GetAttributeName(arSail));
		if( GetAttributesNum(arSailNode)==0 )
		{
			DeleteAttribute(chref,"ship.sails."+GetAttributeName(arSailNode));
		}
		return fSailDmg;
	}

	fSailDmg -= fDmgRepair;
	float sailDmgMax = GetCharacterShipSP(chref) * stf(arSail.sp);
	int iAfterHole = GetNeedHoleFromDmg( fSailDmg, sailDmgMax, sti(arSail.mhc) );
	if( sti(arSail.hc) > iAfterHole )
	{
		arSail.hd = DeleteOneSailHole( sti(chref.index), GetAttributeName(arSail), GetAttributeName(arSailNode), sti(arSail.hd), sti(arSail.hc)-iAfterHole );
		arSail.hc = iAfterHole;
	}
	arSail.dmg = fSailDmg;
	return fDmgRepair;
}

//============================================================
// Получить число дырок в корпусе корабля
//============================================================
int GetBlotsQuantity(ref chref)
{
	if( !CheckAttribute(chref,"ship.blots") ) return 0;
	aref blref;
	makearef(blref,chref.ship.blots);
	return GetAttributesNum(blref);
}

//============================================================
// Удалить заданное число дырок в корпусе корабля
//============================================================
void DeleteBlots(ref chref, int repBlots)
{
	if( !CheckAttribute(chref,"ship.blots") ) return;
	aref blref,curblots;
	makearef(blref,chref.ship.blots);
	int blotsQ = GetAttributesNum(blref);
	while(repBlots>0)
	{
		if(blotsQ==0) break;
		blotsQ--;
		curblots = GetAttributeN(blref,blotsQ);
		DeleteAttribute(blref,GetAttributeName(curblots));
	}
}

//================================================================
// Получить число дырок в парусах исходя из имеющихся повреждений
//================================================================
int GetNeedHoleFromDmg(float sailDmg, float sailDmgMax, int maxHoleCount)
{
	if(sailDmgMax<=0.0) return 0;
	int holeNeed = makeint(sailDmg/sailDmgMax*maxHoleCount);
	return holeNeed;
}

//================================================================
// Надырявить конкретный парус на заданное число дырок и получить
// маску получившихся дырок
// (в параметрах координаты паруса: индекс перса, полное имя реи и
//  номер группы; затем идут максимальное возможное число дырок в
//  парусе, текущая маска дырок и число дополнительных дырок)
//================================================================

//================================================================
// Получить повреждение в парусах исходя из имеющихся дырок
//================================================================
float GetNeedDmgFromHole(int holeCount, float sailDmgMax, int maxHoleCount)
{
	if(maxHoleCount<=0) return 0.0;
	return sailDmgMax*holeCount/maxHoleCount;
}

//================================================================
// Посчитать состояние парусов
//================================================================
float CalculateShipSP(ref chref)
{
	float fSP = GetCharacterShipSP(chref);
	aref arRoot,arGroup,arSail;
	int q,n,i,j;

	makearef(arRoot, chref.ship.sails);
	q = GetAttributesNum(arRoot);
	for(i=0; i<q; i++)
	{
		arGroup = GetAttributeN(arRoot,i);
		n = GetAttributesNum(arGroup);
		for(j=0; j<n; j++)
		{
			arSail = GetAttributeN(arGroup,j);
			if(!CheckAttribute(arSail, "dmg"))
			{
				arSail.dmg = 0.0;
			}
			fSP -= stf(arSail.dmg);
		}
	}

	if(fSP<0.0) fSP = 0.0;
	return fSP;
}

//посчитать макс.количество дырок на корабле
int CalculateShipHoles(ref chref)
{
	aref arRoot,arGroup,arSail;
	int Holes = 0;
	int q,n,i,j;

	makearef(arRoot, chref.ship.sails);
	q = GetAttributesNum(arRoot);
	for(i=0; i<q; i++)
	{
		arGroup = GetAttributeN(arRoot,i);
		n = GetAttributesNum(arGroup);
		for(j=0; j<n; j++)
		{
			arSail = GetAttributeN(arGroup,j);
			Holes += 12;
		}
	}
	return Holes;
}
//===============================================
// Заполнение
//===============================================
int AddTextureToList(ref rHostObj, string texName, int hSize, int vSize)
{
	return SendMessage(rHostObj, "lsll", BI_MSG_ADD_NEWTEXTURE, texName, hSize, vSize);
}

void GetTextureUVForShip(int nShipType, ref rLeft, ref rTop, ref rRight, ref rBottom)
{
	float fLeft = 0.0;
	float fTop = 0.0;
	float fRight = 0.03125;
	float fBottom = 0.03125;//0.25;

	if( nShipType> -1 && nShipType<SHIP_TYPES_QUANTITY_WITH_FORT ) // <-- ugeen fix, для тартаны nShipType = 0 !!!!
	{
		ref rBaseShip = &ShipsTypes[nShipType];
		SetShipPictureDataByShipTypeName( nShipType );
		int nV = BI_intNRetValue[0] / 32;
		int nH = BI_intNRetValue[0] - nV * 32;
		fLeft = stf(nH) * 0.03125;
		fTop = stf(nV) * 0.03125*2;//0.25;
		fBottom = fTop + 0.03125*2;//0.25;
		fRight = fLeft + 0.03125;
	}

	rLeft = fLeft;
	rTop = fTop;
	rRight = fRight;
	rBottom = fBottom;
}

object objTimerInterface;
#event_handler("evntTimerTimeOut","procTimerTimeOut");
void InitTimerInterface()
{
	float fHtRatio = stf(Render.screen_y) / screenscaling;
	DeleteAttribute( &objTimerInterface, "" );
	objTimerInterface.timerpos = RecalculateHIcon(makeint(100*fHtRatio))+","+RecalculateVIcon(makeint(20*fHtRatio))+","+RecalculateHIcon(makeint(228*fHtRatio))+","+RecalculateVIcon(makeint(40*fHtRatio));
	objTimerInterface.timeroffset = "0,0,0,0";
	objTimerInterface.timerbackuv = "0.0,0.0,1.0,0.5";
	objTimerInterface.timerforeuv = "0.0,0.5,1.0,1.0";
	objTimerInterface.timerbackcolor = argb(255,128,128,128);
	objTimerInterface.timerforecolor = argb(255,128,128,128);
	CreateEntity( &objTimerInterface, "BITimer" );
	LayerAddObject(SEA_REALIZE,&objTimerInterface,-1);
	LayerAddObject(REALIZE,&objTimerInterface,-1);
}

void LaunchTimerInterface(float fTime,string questtype,string questname)
{
	objTimerInterface.questname = questname;
	objTimerInterface.questtype = questtype;
	SendMessage(&objTimerInterface,"lfs",0,fTime,"evntTimerTimeOut");
}

void CloseTimerInterface()
{
	SendMessage(&objTimerInterface,"l",1);
}

void procTimerTimeOut()
{
	QuestComplete(objTimerInterface.questtype, objTimerInterface.questname);
}

void BI_ShowInterface(bool bShow)
{
	if( bShow )
	{
		if( IsEntity(&BattleInterface) )
		{
			SendMessage(&BattleInterface,"ll",BI_SET_VISIBLE,true);
		}
		if( IsEntity(&objLandInterface) )
		{
			SendMessage(&objLandInterface,"ll",MSG_BATTLE_LAND_SET_SHOW,true);
		}
	}
	else
	{
		if( IsEntity(&BattleInterface) )
		{
			SendMessage(&BattleInterface,"ll",BI_SET_VISIBLE,false);
		}
		if( IsEntity(&objLandInterface) )
		{
			SendMessage(&objLandInterface,"ll",MSG_BATTLE_LAND_SET_SHOW,false);
		}
	}
}

void MastLightRepairPerSec(ref _refCharacter)
{
	int nShipType = GetCharacterShipType(_refCharacter);
	ref rShip = GetRealShip(nShipType);
	float fRepairH;
	if (checkAttribute(rShip, "mastHP"))
	{
		aref arMasts;
		makearef(arMasts, rShip.mastHP);
		string sMastNum;
		float fMast;
		for(int iM=0; iM<GetAttributesNum(arMasts); iM++)
		{
			sMastNum = GetAttributeName(GetAttributeN(arMasts, iM));
			fMast = GetAttributeValue(GetAttributeN(arMasts, iM));
			if (checkattribute(rShip, "mastHPRep."+sMastNum)) fMast -= makefloat(rShip.mastHPRep.(sMastNum));//учитываем отложенный ремонт
			if (fMast > 0.8 && fMast < 1.0)//повреждения больше 80%, но мачта ещё не сломана
			{
				fRepairH = fMast-0.8;//вычитаем отображаемое игроку, чтобы лишнего не чинил 
				if (fRepairH > BI_SLOW_REPAIR_PERCENT/100.0) {fRepairH = BI_SLOW_REPAIR_PERCENT/100.0;}//1.5% каждые 3 сек
				if (checkattribute(rShip, "mastHPRep."+sMastNum)) rShip.mastHPRep.(sMastNum) = makefloat(rShip.mastHPRep.(sMastNum)) + fRepairH;
				else rShip.mastHPRep.(sMastNum) = fRepairH;
			}
		}
	}
}
