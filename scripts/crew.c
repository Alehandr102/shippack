// файл по методам экипажа, переделка для ВМЛ 29.07.06
void UpdateCrewExp()
{
	int cn;
	ref chr;
	for (int i = 0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if(cn != -1)
		{
			chr = &characters[cn];
			if (bSeaActive || IsEntity(worldMap))  // море или карта
			{
				ChangeCrewExp(chr, "Sailors", 1);
			}
			else
			{
				ChangeCrewExp(chr, "Sailors", -1);
				ChangeCrewExp(chr, "Cannoners", -1);
				ChangeCrewExp(chr, "Soldiers", -1);
			}
		}
	}

}
string GetExpName(int iExp)
{
	string sExp = "Exp 1";

	if(iExp >= 12)
	{
		sExp = "Exp 2";
	}
	if(iExp >= 24)
	{
		sExp = "Exp 3";
	}
	if(iExp >= 35)
	{
		sExp = "Exp 4";
	}
	if(iExp >= 46)
	{
		sExp = "Exp 5";
	}
	if(iExp >= 57)
	{
		sExp = "Exp 6";
	}
	if(iExp >= 68)
	{
		sExp = "Exp 7";
	}
	if(iExp >= 80)
	{
		sExp = "Exp 8";
	}
	if(iExp >= 90)
	{
		sExp = "Exp 9";
	}

	return sExp;
}

// boal новый учет зп 16.01.04 -->
int GetMoneyForOfficer(ref Npchar)
{
    if (CheckAttribute(Npchar, "Payment") && makeint(Npchar.Payment) == true)
    {
	    int i, sum;
	    sum = 0;
	    for (i=1; i<15; i++)
	    {
	        sum += GetSkillValue(Npchar, SKILL_TYPE, GetSkillNameByIdx(i));
	    }
		if (!CheckAttribute(Npchar, "OfficerWantToGo.DontGo"))// aw013 за постоянство надо платить
		{
			return MOD_SKILL_ENEMY_RATE*3*4*sum;
		}
		else
		{
			return MOD_SKILL_ENEMY_RATE*3*8*sum;
		}
    }
    return 0;
}
int GetMoneyForOfficerFull(ref Npchar)
{
    float nLeaderShip = GetSummonSkillFromNameToOld(pchar, SKILL_LEADERSHIP);
	float nCommerce   = GetSummonSkillFromNameToOld(pchar, SKILL_COMMERCE);

	return makeint(GetMoneyForOfficer(Npchar)*2/(nLeaderShip + nCommerce) );
}

float SalaryCoeff_GetSetting(string _param)
{
	return stf(GetConvertStr(_param, KVL_MODS_FILE));
}

// LEO: множитель жалованья с клампом - ОДНОЙ точкой. Кламп жил только внутри GetSalaryForShip,
//   и окно персонажа на ПКМ показывало оплату офицера без него - при штатном 0.5 цифра врала вдвое.
float SalaryCoeff_Get()
{
	float fCoeff = SalaryCoeff_GetSetting("SalaryComplex_Coeff");
	if (fCoeff <= 0.0) fCoeff = 1.0; // нет настройки или мусор в ней
	if (fCoeff <= 0.5) fCoeff = 0.5; // нижняя граница из описания настройки
	return fCoeff;
}

int GetSalaryForShip(ref chref)
{
    int i, cn, iMax;
    ref mchref, offref;
    int nPaymentQ = 0;
    float fExp;
    mchref = GetMainCharacter();
	float SalaryCoeff = 1.0;

	float nLeaderShip = GetSummonSkillFromNameToOld(mchref,SKILL_LEADERSHIP);
	float nCommerce   = GetSummonSkillFromNameToOld(mchref,SKILL_COMMERCE);

	float shClass = GetCharacterShipClass(chref);
	if (shClass   < 1) shClass   =7;
	if (!GetRemovable(chref) && sti(chref.index) != GetMainCharacterIndex()) return 0; // считаем только своих, а то вских сопровождаемых кормить!!!

	SalaryCoeff = SalaryCoeff_Get();

	// экипаж
	fExp = (GetCrewExp(chref, "Sailors") + GetCrewExp(chref, "Cannoners") + GetCrewExp(chref, "Soldiers")) / 100.00; // средний коэф опыта 0..3
	nPaymentQ += makeint( SalaryCoeff * fExp * stf((0.5 + MOD_SKILL_ENEMY_RATE*3/5.0)*200*GetCrewQuantity(chref))/stf(shClass) * (1.05 - (nLeaderShip + nCommerce)/ 40.0) );

    // теперь самого капитана и его офицеров (тут  главный герой не считается) так что пассажиров и оффицеров ниже
    if(sti(chref.index) != GetMainCharacterIndex())
    {
        nPaymentQ += makeint(SalaryCoeff * GetMoneyForOfficer(chref)*2/(nLeaderShip + nCommerce) );
        // офицеры
        for(i = 1; i <=MAX_NUM_FIGHTERS; i++)  // в к3 нет офов у компаньона :(
	    {
	        cn = GetOfficersIndex(chref, i);
		    if( cn > 0 )
		    {
			    offref = GetCharacter(cn);
			    if (GetRemovable(offref)) // считаем только своих, а то вских сопровождаемых кормить!!!
			    {
			        nPaymentQ += makeint(SalaryCoeff * GetMoneyForOfficerFull(offref));
			    }
			}
		}
	}
	if(sti(chref.index) == GetMainCharacterIndex()) // все пассажиры и офицеры для гл героя
	{
        iMax = GetPassengersQuantity(mchref);
		for(i=0; i < iMax; i++)
        {
            cn = GetPassenger(mchref,i);
            if(cn != -1)
            {
                if(!IsCompanion(GetCharacter(cn)))
                {
                    offref = GetCharacter(cn);
                    if (GetRemovable(offref)) // считаем только своих, а то вских сопровождаемых кормить!!!
			        {
                        if(CheckAttribute(offref,"prisoned"))
    		            {
    			            if(sti(offref.prisoned)==true) continue;
    		            }
    			        nPaymentQ += makeint(SalaryCoeff * GetMoneyForOfficerFull(offref));
			        }
                }
            }
        }
    }
    return nPaymentQ;
}
// boal новый учет зп <--

int AddCrewMorale(ref chr, int add)
{
    int morale = MORALE_NORMAL;
	if (CheckAttribute(chr, "Ship.Crew.Morale")) morale = sti(chr.Ship.Crew.Morale);
    morale += add;
	if(morale < MORALE_MIN)	morale = MORALE_MIN;
	if(morale > MORALE_MAX)	morale = MORALE_MAX;
	chr.Ship.Crew.Morale = morale;

	return morale;
}

int GetCharacterRaiseCrewMoraleMoney(ref chr)
{
	float nLeaderShip = GetSummonSkillFromNameToOld(GetMainCharacter(),SKILL_LEADERSHIP);
	float nCommerce   = GetSummonSkillFromNameToOld(GetMainCharacter(),SKILL_COMMERCE); // boal
	int nPaymentQ = 15 + GetCrewQuantity(chr)*(16 + MOD_SKILL_ENEMY_RATE*3*5 - nLeaderShip - nCommerce); // boal
	float fExp = (GetCrewExp(chr, "Sailors") + GetCrewExp(chr, "Cannoners") + GetCrewExp(chr, "Soldiers")) / 100.00; // средний коэф опыта 0..3
	nPaymentQ = makeint(nPaymentQ * fExp + 0.5);
	if (nPaymentQ < 5) nPaymentQ = 5;
	return nPaymentQ;
}

void DownCrewExp(ref chr, int days)
{
	ChangeCrewExp(chr, "Sailors", -makeint(days/10));
	ChangeCrewExp(chr, "Cannoners", -makeint(days/10));
	ChangeCrewExp(chr, "Soldiers", -makeint(days/10));
}

float ChangeCrewExp(ref chr, string sType, float fNewExp)
{
	if (!CheckAttribute(chr, "Ship.Crew.Exp." + sType)) chr.Ship.Crew.Exp.(sType) = (1 + rand(50));
	float fCrewExp = stf(chr.Ship.Crew.Exp.(sType));
	if (stf(chr.Ship.Crew.Quantity) == 0) return fCrewExp;//фикс ошибки деления на ноль

	if (fNewExp > 0.0)
	{
		switch(sType)
		{
			case "Cannoners" 	:
				fNewExp = fNewExp * isEquippedArtefactUse(chr, "talisman13", 1.0, 2.0);
			break;
			case "Sailors" 		:
				fNewExp = fNewExp * isEquippedArtefactUse(chr, "talisman12", 1.0, 2.0);
			break;
			case "Soldiers" 	:
				fNewExp = fNewExp * isEquippedArtefactUse(chr, "talisman14", 1.0, 2.0);
			break;
		}
	}
	fCrewExp = fCrewExp + fNewExp * (100/stf(chr.Ship.Crew.Quantity));//не все матросы учатся разом
	if (fCrewExp > 100) { fCrewExp = 100; }
	if (fCrewExp < 1) { fCrewExp = 1; }
	chr.Ship.Crew.Exp.(sType) = fCrewExp;
	return fCrewExp;
}

float GetCrewExp(ref chr, string sType)
{
	if (!CheckAttribute(chr, "Ship.Crew.Exp." + sType)) chr.Ship.Crew.Exp.(sType) = 10;
	return stf(chr.Ship.Crew.Exp.(sType));
}

float GetCrewExpRate()
{
	return makefloat(50 + MOD_SKILL_ENEMY_RATE*3);
}

int GetCharacterCrewMorale(ref chr)
{
	if(!CheckAttribute(chr, "ship.crew.morale"))
	{
		chr.ship.crew.morale = MORALE_NORMAL;
	}

	return sti(chr.ship.crew.morale);
}

// пересчет наемников в городах
void UpdateCrewInColonies()
{
	int nNeedCrew = GetCurCrewEscadr(); // всего матросов
	int ableCrew = GetMaxCrewAble();   // допустимое число
	ref rTown;
	int nPastQ, nPastM;
	int eSailors, eCannoners, eSoldiers;

	for(int i = 0; i < MAX_COLONIES; i++)
	{
		rTown = &colonies[i];
	    if (rTown.nation == "none") continue;

		if(CheckAttribute(rTown, "AdditionalCrew")) DeleteAttribute(rTown, "AdditionalCrew");

	    if (GetNpcQuestPastDayParam(rTown, "CrewDate") >= (2+rand(2)) || !CheckAttribute(rTown, "CrewDate.control_year"))
	    {
			SaveCurrentNpcQuestDateParam(rTown, "CrewDate");
			nPastQ = 0;
			if (CheckAttribute(rTown,"ship.crew.quantity"))	nPastQ = sti(rTown.ship.crew.quantity);

			if (nNeedCrew >= ableCrew )
		    {
		        nNeedCrew = 1+rand(20);
		    }
		    else
		    {
		        nNeedCrew = ableCrew - nNeedCrew - rand(makeint((ableCrew - nNeedCrew)/2.0));
				if (nNeedCrew < 1) nNeedCrew = 1+rand(20);
		    }

/*			nNeedCrew = makeint(abs(REPUTATION_NEUTRAL - sti(pchar.reputation))/MOD_SKILL_ENEMY_RATE + sti(pchar.rank)/MOD_SKILL_ENEMY_RATE + rand(sti(pchar.rank)/2) + drand(20 + MOD_SKILL_ENEMY_RATE));
			
			if(MOD_SKILL_ENEMY_RATE == 2) nNeedCrew *= 3;
			if(MOD_SKILL_ENEMY_RATE == 4) nNeedCrew *= 2;
*/

			if (nPastQ > nNeedCrew)
			{	nPastM = MORALE_NORMAL/3 + rand(MORALE_MAX-MORALE_NORMAL/3);
			}
			else
			{	nPastM = MORALE_NORMAL/5 + rand(makeint(MORALE_NORMAL*1.5));
			}
			rTown.Ship.crew.quantity = nNeedCrew;
			rTown.Ship.crew.morale   = nPastM;
			// пороги опыта от нации
			switch (sti(rTown.nation))
			{
				case ENGLAND:
					eSailors   = 45;
					eCannoners = 5;
					eSoldiers  = 20;
				break;
				case FRANCE:
					eSailors   = 20;
					eCannoners = 45;
					eSoldiers  = 5;
				break;
				case SPAIN:
					eSailors   = 5;
					eCannoners = 20;
					eSoldiers  = 45;
				break;
				case PIRATE:
					eSailors   = 25;
					eCannoners = 25;
					eSoldiers  = 45;
				break;
				case HOLLAND:
					eSailors   = 30;
					eCannoners = 30;
					eSoldiers  = 10;
				break;
			}
			rTown.Ship.Crew.Exp.Sailors   = eSailors   + rand(2*eSailors)   + rand(10);
			rTown.Ship.Crew.Exp.Cannoners = eCannoners + rand(2*eCannoners) + rand(10);
			rTown.Ship.Crew.Exp.Soldiers  = eSoldiers  + rand(2*eSoldiers)  + rand(10);
			ChangeCrewExp(rTown, "Sailors", 0);  // приведение к 1-100
			ChangeCrewExp(rTown, "Cannoners", 0);
			ChangeCrewExp(rTown, "Soldiers", 0);
		}
	}
}

int GetCrewPriceForTavern(string sColony)
{
	int iColony = FindColony(sColony);
	ref rTown = &colonies[iColony];

    float fExp = (GetCrewExp(rTown, "Sailors") + GetCrewExp(rTown, "Cannoners") + GetCrewExp(rTown, "Soldiers")) / 100.00; // средний коэф опыта 0..3
	float fSkill = GetSummonSkillFromNameToOld(GetMainCharacter(),SKILL_LEADERSHIP) + GetSummonSkillFromNameToOld(GetMainCharacter(),SKILL_COMMERCE); // 0-20
    int   nCrewCost = makeint((0.5 + MOD_SKILL_ENEMY_RATE*3/5.0)*30 * (1.0 - fSkill / 40.0));

	nCrewCost = makeint(fExp*nCrewCost + 0.5);
	if (nCrewCost < 5) nCrewCost = 5; // не ниже!

	return nCrewCost;
}

int GetMaxCrewAble()
{
	float nLeaderShip = 0.5 + GetSummonSkillFromNameToOld(pchar, SKILL_LEADERSHIP);
    //MOD_SKILL_ENEMY_RATE
    return makeint(nLeaderShip*(55.0 + 10*(5-MOD_SKILL_ENEMY_RATE*3) + nLeaderShip * 15.0) + 2*nLeaderShip*abs(REPUTATION_NEUTRAL - sti(pchar.reputation)));
}

int GetCurCrewEscadr()
{
    int i, cn;
    int nNeedCrew = 0;

    for(i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(GetMainCharacter(),i);
		if(cn>=0 && GetRemovable(&Characters[cn]))
		{
			nNeedCrew += GetCrewQuantity(GetCharacter(cn));
		}
	}
	return nNeedCrew;
}

void MunityOnShip(string _stat)
{
	int i;
	Statistic_AddValue(pchar, _stat, 1);
    MakeCloneShipDeck(pchar, true); // подмена палубы
    i = FindLocation("Ship_deck");
    Locations[i].image = "loading\jonny_load\load\rebel.dds"; // это клоновая локация, вернётся само при перетирании другим
    DoQuestReloadToLocation("Ship_deck", "reload", "reload1", "Munity_on_Ship");
}
/* 20.01.08 Дележ добычи =======================================================================
Концепт:
Делим только награбленное, торговые, квестовые барыши не делим (выпадают сухупутные грабежи)
Для этого считаем сколько было денег до выхода в море, после моря и баталии, на карте и суше подсчет денег после
Сравниваем, если убытки, ничего не делаем, если доходы, то к дележу.
Сумма может быть не выплачена сразу, наличие суммы влияет на мораль как и ЗП.
Повышение морали погашает задолженность, если долгов нет, то плата просто так
Наличие долгов делает -1 морали каждый день, ром может спасти, а может и нет, если ещё и перегруз
Долги наследуются, даже, если все умерли. Это условность, но необходимо же как-то с ГГ стрести деньги.
Товар и корабли считаются по условно-минимальным ценам, то есть ГГ покупает их у команды и платит долю по бросовой цене.
Общая ЗП при этом сохраняется, так как доходы от торговли и контрабанды не делятся.
Грабеж города считаем условно поделенным, то есть матросы нахапали свои доли сами и не делим дополнительно (сложно делать и барышей там мало).
*/
void Partition_SetValue(string state) // state = "before" || "after" - для сравнения было-стало
{
	int cn, iMax;
	ref chref, rLoc;
	string sTemp;

	int cPART_HeroPart = 200;	//базовая доля героя
	int cPART_HeroComplex = 50;	//часть доли героя, меняющаяся из-за выбранной сложности. Удваивала долю героя при нормальной сложности
	int cPART_HeroShips = 20;	//доля героя на содержание кораблей эскадры. Домножается на класс каждого корабля. Ноль для тартан

	int cPART_Officer = 10;		//доля офицера/пассажира
	int cPART_Companion = 30;	//доля компаньона
	int cPART_Crew = 1;			//доля матроса
//наличные монеты считаются в карманах ГГ, всех офицеров, компаньонов и в каютных сундуках
//предметы не считаются
//вклады ростовщиков и склады не считаются - их изменения происходят на берегу, не влияют на стоимость эскадры в море

	int part_Money = 0;
	int part_Ships = 0;
	int part_Cannons = 0;
	int part_Goods = 0;
	int part_GCannons = 0;
	int part_Sum = 0;

	int part_HeroPart = cPART_HeroPart + (10 - MOD_SKILL_ENEMY_RATE*3)*cPART_HeroComplex;
	int part_HeroShips = 0;
	int part_Officer = 0;
	int part_Companion = 0;
	int part_Crew = 0;

	if (CheckAttribute(NullCharacter, "Partition")) cn = sti(NullCharacter.Partition); else cn = 0;
	if (state == "before") cn++;
	NullCharacter.Partition = cn;	//счетчик порядкового номера раздела добычи

	aref arCurPartition;
	makearef(arCurPartition, NullCharacter.Partition.(cn));
	arCurPartition.(state) = GetQuestBookDataDigit();	//дата текстом

	for (int i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(Pchar, i);
		if (cn >= 0)
		{
			chref = GetCharacter(cn);
			if (GetRemovable(chref)) // считаем только своих
			{
				part_Money += sti(chref.Money);	//деньги в кармане
				Part_AddShipValue(chref, &part_Ships, &part_Cannons, &part_Goods, &part_GCannons);//стоимость корабля с содержимым
				part_Companion++; // ГГ тут же
				part_Crew += GetCrewQuantity(chref);
				part_HeroShips = cPART_HeroShips * (7 - GetCharacterShipClass(chref));
			}
		}
	}

	iMax = GetPassengersQuantity(Pchar);
	for(i=0; i < iMax; i++)
	{
		cn = GetPassenger(Pchar, i);
		if(cn != -1)
		{
			chref = GetCharacter(cn);
			if (GetRemovable(chref)) // считаем только своих
			{
				if (CheckAttribute(chref, "prisoned")) {if (sti(chref.prisoned)==true) continue;}
				part_Money += sti(chref.Money);
				part_Officer++;
			}
		}
	}
	
	if (Pchar.SystemInfo.CabinType != "")	// предметы в каюте
	{
		rLoc = &locations[FindLocation(Pchar.SystemInfo.CabinType)];
		for (i = 1; i <= 5; i++)	//разве, каютных сундуков 5 может быть?
		{
			sTemp = "box" + i;
			if (CheckAttribute(rLoc, sTemp + ".money")) part_Money += sti(rLoc.(sTemp).money);
		}
	}

	arCurPartition.(state).Money = part_Money;
	arCurPartition.(state).Ships = part_Ships;
	arCurPartition.(state).Cannons = part_Cannons;
	arCurPartition.(state).Goods = part_Goods;
	arCurPartition.(state).GCannons = part_GCannons;
	arCurPartition.(state).MSum = part_Money + part_Ships + part_Cannons + part_Goods + part_GCannons;	//стоимость эскадры

	float fTotalPart = part_HeroPart + part_HeroShips + part_Officer * cPART_Officer + part_Crew * cPART_Crew + (part_Companion - 1) * cPART_Companion;
	if (state == "before")
	{
		arCurPartition.before.HowOff  = part_Officer;	//количество участников раздела добычи
		arCurPartition.before.HowComp = part_Companion - 1;
		arCurPartition.before.HowCrew = part_Crew;

		arCurPartition.before.pHeroPart = part_HeroPart / fTotalPart;	//доли участников раздела добычи в процентах уже
		arCurPartition.before.pHeroShips = part_HeroShips / fTotalPart;
		arCurPartition.before.pOfficer = part_Officer * cPART_Officer / fTotalPart;
		arCurPartition.before.pCrew = part_Crew * cPART_Crew / fTotalPart;
		arCurPartition.before.pCompanion = (part_Companion - 1) * cPART_Companion / fTotalPart;
	}

	if (state == "after" && CheckAttribute(arCurPartition, "before.MSum"))
	{
		if (sti(arCurPartition.before.MSum) < sti(arCurPartition.after.MSum))
		{
			if (bPartitionSet)	//включена опция раздела добычи
			{
				if (!CheckAttribute(Pchar, "Partition.MonthPart"))           Pchar.Partition.MonthPart = 0;
				if (!CheckAttribute(Pchar, "Partition.MonthPart.Crew"))      Pchar.Partition.MonthPart.Crew = 0;
				if (!CheckAttribute(Pchar, "Partition.MonthPart.Officers"))  Pchar.Partition.MonthPart.Officers = 0;
				if (!CheckAttribute(Pchar, "Partition.MonthPart.Hero"))      Pchar.Partition.MonthPart.Hero = 0;

				int TotalAmount = sti(arCurPartition.after.MSum) - sti(arCurPartition.before.MSum);
				Pchar.Partition.MonthPart.Crew     = sti(Pchar.Partition.MonthPart.Crew) + stf(arCurPartition.before.pCrew) * TotalAmount;
				Pchar.Partition.MonthPart.Officers = sti(Pchar.Partition.MonthPart.Officers) + 
													( stf(arCurPartition.before.pOfficer) + stf(arCurPartition.before.pCompanion) ) * TotalAmount;
				Pchar.Partition.MonthPart.Hero = sti(Pchar.Partition.MonthPart.Hero) + 
													( stf(arCurPartition.before.pHeroPart) + stf(arCurPartition.before.pHeroShips) ) * TotalAmount;
				Pchar.Partition.MonthPart = sti(Pchar.Partition.MonthPart) + 
													(1.0 - stf(arCurPartition.before.pHeroPart) - stf(arCurPartition.before.pHeroShips)) * TotalAmount;
			}
		}
	}
}

void Part_AddShipValue(ref chref, int part_Ships, int part_Cannons, int part_Goods, int part_GCannons)
{
	ref shref, rCannon;

	float cPART_PriceShips = 0.20;		//stf(GetConvertStr("PriceShips", "PartitionSettings.txt"))	//множитель цены. процент от базовой цены корабля, учитываемый в оценке стоимости добычи
	float cPART_PriceGoods = 0.70;		//множитель цены товаров в трюме
	float cPART_PriceCannons = 0.33;	//множитель цены пушек, в трюме и установленных на корабли

	int st = GetCharacterShipType(chref);
	if (st != SHIP_NOTUSED)
	{
		shref = GetRealShip(st);
		part_Ships += sti(shref.Price) * cPART_PriceShips;

		if (sti(chref.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
		{
		    rCannon = GetCannonByType(sti(chref.Ship.Cannons.Type));
		    part_Cannons += sti(rCannon.Cost) * cPART_PriceCannons * GetCannonsNum(chref);
		}
		for (int i=0; i<GOODS_QUANTITY; i++)
		{
			if (CheckAttribute(Goods[i], "CannonIdx"))
				part_GCannons += makefloat(GetCargoGoods(chref, i) * sti(Goods[i].Cost) * cPART_PriceCannons / stf(Goods[i].Units));
			else
				part_Goods 	  += makefloat(GetCargoGoods(chref, i) * sti(Goods[i].Cost) * cPART_PriceGoods / stf(Goods[i].Units));
		}
	}
}
