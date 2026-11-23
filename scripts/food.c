#define I_MIN_MORALE	10

// boal -->
#define FOOD_BY_CREW       10.0
#define FOOD_BY_SLAVES     20.0
#define FOOD_BY_PASSENGERS 10.0
#define RUM_BY_CREW        20.0
// boal <--

// вернёт число дней на сколько есть еда на всех кораблях
int CalculateFood()
{
	int chrIndex;
	int iCrewQuantity = 0;
	float foodNeeded = 0;
	int iSlavesQuantity = 0;
	int iFoodQuantity = 0;

	for (int i=0; i<COMPANION_MAX; i++)
	{
		chrIndex = GetCompanionIndex(pchar, i);
		if (chrIndex != -1)
		{
            if (!GetRemovable(&characters[chrIndex])) continue;
			if (characters[chrIndex].ship.type != SHIP_NOTUSED)
			{
				// LEO: раненые едят наравне со здоровыми, плюс офицерская добавка на КАЖДЫЙ корабль -
				//   расход считается по кораблям (в сумматоре она целая, в списании 5.1).
				iCrewQuantity   += GetCrewQuantity(&characters[chrIndex]) + GetWoundedQuantity(&characters[chrIndex]) + 5;
				iFoodQuantity   += makeint(GetCargoGoods(&characters[chrIndex], GOOD_FOOD) / isEquippedArtefactUse(&characters[chrIndex], "talisman6", 1.0, 0.75));
				iSlavesQuantity += GetCargoGoods(&characters[chrIndex], GOOD_SLAVES);
			}
		}
	}
	int iPassQuantity = GetPassengersQuantity(pchar);

	foodNeeded = makefloat(iCrewQuantity/FOOD_BY_CREW + iPassQuantity/FOOD_BY_PASSENGERS + iSlavesQuantity/FOOD_BY_SLAVES);

	if (foodNeeded < 1)
	{
		foodNeeded = 1;
	}

	iFoodQuantity = makeint(iFoodQuantity/foodNeeded + 0.2);

	return iFoodQuantity;
}

// еды на одном корабле
int CalculateShipFood(ref _chr)
{
	int iCrewQuantity = 0;
	float foodNeeded = 0;
	int iSlavesQuantity = 0;
	int iFoodQuantity = 0;
	int iPassQuantity = 0;

	// LEO: раненые тоже едят, и суточный расход добавляет 5.1 на офицеров - см. формулу списания
	//   ниже по файлу. Показ считал только здоровых и без этой добавки, поэтому в меню корабля
	//   дней было больше, чем на глобалке, где предупреждение идёт от реального расхода.
	iCrewQuantity   =  GetCrewQuantity(_chr) + GetWoundedQuantity(_chr);
	iFoodQuantity   =  GetCargoGoods(_chr, GOOD_FOOD);
	iSlavesQuantity =  GetCargoGoods(_chr, GOOD_SLAVES);
	if (_chr.id == pchar.id)
	{
		iPassQuantity = GetPassengersQuantity(pchar);
	}

	foodNeeded = isEquippedArtefactUse(_chr, "talisman6", 1.0, 0.75) * makefloat((iCrewQuantity + 5.1)/FOOD_BY_CREW + iPassQuantity/FOOD_BY_PASSENGERS + iSlavesQuantity/FOOD_BY_SLAVES);

	if (foodNeeded < 1)
	{
		foodNeeded = 1;
	}

	iFoodQuantity = makeint(iFoodQuantity/foodNeeded + 0.2);

	return iFoodQuantity;
}

// Warship 11.07.09 Вернёт кол-во дней, на сколько хватит рому на одном корабле
int CalculateShipRum(ref _character)
{
	// LEO: раненые тоже пьют - как и в суточном списании рома ниже по файлу
	int iCrewQty = GetCrewQuantity(_character) + GetWoundedQuantity(_character);
	int cn;
	iCrewQty = makeint(isEquippedArtefactUse(_character, "talisman4", 1.0, 0.2) *((iCrewQty+5.1) / RUM_BY_CREW)); // eat ratio
	if (iCrewQty == 0) iCrewQty = 1;
	int rumCount = GetCargoGoods(_character, GOOD_RUM);
	int iRumDays = 0;
	iRumDays = makeint(rumCount/iCrewQty);
	return iRumDays;
}

// boal 21.04.04 крысы на корабле -->
//#20200226-03
int CalculateRum()
{
	// LEO: раненые тоже пьют - как и в суточном списании рома ниже по файлу
	int iCrewQty = GetCrewQuantity(pchar) + GetWoundedQuantity(pchar);
	int cn;
	iCrewQty = makeint(isEquippedArtefactUse(pchar, "talisman4", 1.0, 0.2) *((iCrewQty+5.1) / RUM_BY_CREW)); // eat ratio
	if (iCrewQty == 0) iCrewQty = 1;
	int rumCount = GetCargoGoods(pchar, GOOD_RUM);
	int iRumDays = 0;
	iRumDays = makeint(rumCount/iCrewQty);
	return iRumDays;
}

void DailyRatsEatGoodsUpdate(ref chref)
{
	if (GetCharacterItem(chref, "indian11") > 0) return;

    int iGoods = GOOD_POWDER + rand(GOOD_OIL - GOOD_POWDER);
    int iQuantity = GetCargoGoods(chref, iGoods);
    int iSeaGoods = LanguageOpenFile("ShipEatGood.txt");
	int qty = 30;
	int randv = 4;
	if(IsCharacterPerkOn(chref, "Trader"))
	{
		qty = 60;
		randv = 1;
	}
    if (iQuantity > qty && rand(randv) != 0) // шанс не жрать, если весь спектр
    {
        float fSkill = GetSummonSkillFromNameToOld(chref, SKILL_REPAIR) + GetSummonSkillFromNameToOld(chref,SKILL_FORTUNE);

        iQuantity = 1+ rand(makeint(iQuantity / (10+fSkill)));
        RemoveCharacterGoodsSelf(chref, iGoods, iQuantity);
		string sOf = "";
		if (LanguageGetLanguage() != "russian") sOf = "of ";
                Log_SetStringToLog(RandSwear() + GetConvertStrWithReplace("Variable_food_1", "Logs.txt", "#space#", " ")+ XI_ConvertString(RealShips[sti(chref.Ship.Type)].BaseName+"Voc") + " '" + chref.Ship.Name + "'" + LinkRandPhrase(GetConvertStrWithReplace("Variable_food_2", "Logs.txt", "#space#", " "), GetConvertStrWithReplace("Variable_food_3", "Logs.txt", "#space#", " "), GetConvertStrWithReplace("Variable_food_4", "Logs.txt", "#space#", " ")) +
                           iQuantity + GetConvertStrWithReplace("Variable_pcs_with_spaces", "Logs.txt", "#space#", " ") + sOf + LanguageConvertString(iSeaGoods, "seg_" + Goods[iGoods].Name));

        Statistic_AddValue(pchar, "RatsEatGoods", iQuantity);

		if (iQuantity > 400) iQuantity = 400;

		AddCharacterExpToSkill(chref, SKILL_REPAIR, iQuantity);
        AddCharacterExpToSkill(chref, SKILL_FORTUNE, iQuantity/10);
    }
    LanguageCloseFile(iSeaGoods);
}

// boal food for crew 20.01.2004 -->
void DailyEatCrewUpdate()   // сюда пихаю всё что в 1 день
{
    ref mainCh = GetMainCharacter();
    int i, cn, crew, morale;
    ref chref;
    int nMoraleDecreaseQ;

    // LEO: тавернский врач оплачен -> лечим ВСЕХ раненых флота разом (на смену суток), снимаем флаг
    if(CheckAttribute(mainCh, "Ship.Crew.woundedTavernHeal"))
    {
        if(Wounded_CanRecover())
        {
            DeleteAttribute(mainCh, "Ship.Crew.woundedTavernHeal");
            Wounded_TavernHealFleet(mainCh);
        }
    }
    DeleteAttribute(mainCh, "woundedDocPending"); // LEO: фолбэк-сброс гейта вызова врача (если врача вызвали, но не поговорили - опция вернётся на след. сутки)

    mainCh.questTemp.abordage = 0; // fix квест потопить пирата второй абордаж

    SetNewDayHealth(); // здоровье за день
    // >>>>>======== квест Аззи, подсчет контрольныйх сумм по неуязвимости =================
    AzzyCheckSumControl();
    // <<<<<======== квест Аззи, подсчет контрольныйх сумм по неуязвимости =================
	// ОЗГи -->
    DeleteAttribute(mainCh, "GenQuest.Hunter2Pause");  // boal бойня в форте кончилась - ОЗГи вернулись
    // ОЗГи <--

	////////////////      ЕДА     /////////////////
	if (bNoEatNoRats) return; // betatest
    if (sti(mainCh.Ship.Type) == SHIP_NOTUSED ) return;

	// снижение лояльности от долга 02.02.08 -->
	if (CheckAttribute(pchar, "CrewPayment")) // Долг
	{
        cn = makeint(pchar.CrewPayment);
        if (cn > 32000) cn = 32000;
		if (rand(cn) > 1000)
		{
            morale = 5 + CheckOfficersPerk(pchar, "IronWill");   // true = 1
			for (i = 0; i<GetPassengersQuantity(pchar); i++)
			{   // любой пассажир у кого есть пристрастие может свалить
				cn = GetPassenger(pchar, i);
				if (cn != -1)
				{
		            chref = &Characters[cn];
					if (CheckAttribute(chref, "loyality") && !CheckAttribute(chref, "OfficerWantToGo.DontGo") && rand(morale) == 2 && !IsEquipCharacterByArtefact(chref, "talisman17"))
					{
		    			chref.loyality = makeint(chref.loyality) - 1;
					}
				}
			}
		}
	}
	// снижение лояльности от долга 02.02.08 <--

	for(i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(mainCh,i);
		if( cn>=0 )
		{
			chref = GetCharacter(cn);

			if(sti(chref.Ship.soiling)<100)
			{
				if (CheckForUpgrade(&RealShips[sti(chref.Ship.Type)],"SUP_HULLKRENG")) //Медное днище
				{
					if (rand(2)<1) chref.Ship.soiling = sti(chref.Ship.soiling)+1;
				}
				else chref.Ship.soiling= sti(chref.Ship.soiling)+1;
				if (S_armor(chref) && rand(2)<1) chref.Ship.soiling = sti(chref.Ship.soiling) - 1; // СФ
			}

			if (!GetRemovable(chref)) continue;

			// RATS -->
			DailyRatsEatGoodsUpdate(chref);
			// RATS <--
			DailyEatCrewUpdateForShip(chref, false);

		}
	}
}

// boal 20.01.2004 <--

// ===================== LEO: РАНЕНАЯ КОМАНДА - суточное выздоровление =====================
#define WND_RECOVERY_MIN_DAYS		2		// LEO: база выздоровления при навыке Защиты 100 (дней)
#define WND_RECOVERY_MAX_DAYS		6		// LEO: база выздоровления при навыке Защиты 1 (дней)
#define WND_RECOVERY_RAND_LOW		3		// LEO: случайный хвост дней при навыке 100
#define WND_RECOVERY_RAND_HIGH		6		// LEO: случайный хвост дней при навыке 1
#define WND_DOC1_SPEED				0.20	// LEO: ускорение лечения перком Doctor1
#define WND_DOC2_SPEED				0.40	// LEO: ускорение лечения перком Doctor2
#define WND_DOC3_SPEED				0.60	// LEO: ускорение лечения перком EmergentSurgeon (не стак, берём лучший)
#define WND_PORT_SPEEDUP			0.20	// LEO: +20% к скорости лечения в порту
#define WND_MEDS_PER_HEAL			2		// LEO: лекарств на одного вылеченного раненого
#define WND_INFECTION_DEATH_FRAC	0.10	// LEO: доля раненых, гибнущих за день без лекарств (инфекция)
#define WND_TAVERN_COST_PER			50		// LEO: базовая цена лечения одного раненого у тавернского врача (× навык 0.5..1.5)

// LEO: Ночь в морской каюте не считается стоянкой на суше.
bool Wounded_CanRecover()
{
	if(bSeaActive) return false;
	if(bCabinStarted) return false;
	if(bDeckBoatStarted) return false;
	return true;
}

bool Wounded_PlayerInPort()
{
	if(IsEntity(worldMap)) return false;
	return Wounded_CanRecover();
}

// LEO: лучший (НЕ стак) перк врача на корабле -> % ускорения выздоровления (0 / 0.20 / 0.40 / 0.60).
// CheckOfficersPerk смотрит перки ВСЕХ офицеров корабля (в т.ч. ГГ-капитана) - покрывает «врач с перками ИЛИ перки ГГ».
float Wounded_DocPerkSpeed(ref rChar)
{
	if(CheckOfficersPerk(rChar, "EmergentSurgeon")) return WND_DOC3_SPEED;
	if(CheckOfficersPerk(rChar, "Doctor2")) return WND_DOC2_SPEED;
	if(CheckOfficersPerk(rChar, "Doctor1")) return WND_DOC1_SPEED;
	return 0.0;
}

// LEO: БАЗА дней выздоровления по навыку Защиты (без рандом-хвоста и бустов). Едина для тика и прогноза.
float Wounded_BaseDays(ref rChar)
{
	int defW = GetSummonSkillFromName(rChar, SKILL_DEFENCE);
	if(defW < 1)   defW = 1;
	if(defW > 100) defW = 100;
	float fLerp = makefloat(defW - 1) / 99.0; // 0 при навыке 1 .. 1 при навыке 100
	return makefloat(WND_RECOVERY_MAX_DAYS) - fLerp * makefloat(WND_RECOVERY_MAX_DAYS - WND_RECOVERY_MIN_DAYS);
}

// LEO: размах суточного рандом-хвоста по навыку Защиты. Тик катит dRand2(span) -> 0..span ВКЛЮЧИТЕЛЬНО.
int Wounded_RandSpan(ref rChar)
{
	int defW = GetSummonSkillFromName(rChar, SKILL_DEFENCE);
	if(defW < 1)   defW = 1;
	if(defW > 100) defW = 100;
	float fLerp = makefloat(defW - 1) / 99.0;
	int iSpan = WND_RECOVERY_RAND_HIGH - makeint(fLerp * makefloat(WND_RECOVERY_RAND_HIGH - WND_RECOVERY_RAND_LOW));
	if(iSpan < 0) iSpan = 0;
	return iSpan;
}

// LEO: применить бусты к дням выздоровления: перк врача (лучший, не стак) × порт. Едино для тика и прогноза.
float Wounded_ApplyBoosts(ref rChar, float fDays)
{
	fDays = fDays * (1.0 - Wounded_DocPerkSpeed(rChar));
	if(Wounded_PlayerInPort()) { fDays = fDays * (1.0 - WND_PORT_SPEEDUP); } // порт: +20% к скорости (сокращает дни)
	if(fDays < 1.0) fDays = 1.0;
	return fDays;
}

// LEO: сколько раненых встанет в строй за ОДИН день при заданной скорости.
// ceil (округл. ВВЕРХ): иначе на малом пуле раненых буст не ускоряет - всё лечится по 1/день.
int Wounded_HealPerDay(int iWnd, float fRecDays)
{
	float fHeal = makefloat(iWnd) / fRecDays;
	int iHeal = makeint(fHeal);
	if(makefloat(iHeal) < fHeal) iHeal = iHeal + 1;
	if(iHeal < 1) iHeal = 1;
	if(iHeal > iWnd) iHeal = iWnd;
	return iHeal;
}

// LEO: за сколько дней рассосётся пул раненых при заданной скорости (симуляция того же дрипа, что в тике).
int Wounded_SimDays(int iWnd, float fRecDays)
{
	int iSim = iWnd;
	int iDays = 0;
	int iGuard = 0;
	while(iSim > 0 && iGuard < 999)
	{
		iSim = iSim - Wounded_HealPerDay(iSim, fRecDays);
		iDays = iDays + 1;
		iGuard = iGuard + 1;
	}
	return iDays;
}

// LEO: суточное выздоровление раненых (дрип назад в строй) + инфекция без лекарств.
// Скорость = навык Защиты (навык1 -> 6+dRand дней, навык100 -> 2+dRand), × перк врача (лучший, не стак), × порт.
// Лечение тратит лекарства (WND_MEDS_PER_HEAL за раненого). Нет лекарств / эпидемия -> раненые гибнут.
void Wounded_DailyHeal(ref rChar, bool IsCompanionTraveler)
{
	int iWnd = GetWoundedQuantity(rChar);
	if(iWnd < 1) return;

	// нет лекарств / эпидемия -> ИНФЕКЦИЯ: раненые гибнут
	if(GetCargoGoods(rChar, GOOD_MEDICAMENT) < 1 || CheckShipSituationDaily_GenQuest(rChar) > 1)
	{
		int iInf = makeint(makefloat(iWnd) * WND_INFECTION_DEATH_FRAC + 0.5);
		if(iInf < 1) iInf = 1;
		if(iInf > iWnd) iInf = iWnd;
		RemoveWounded(rChar, iInf);
		Statistic_AddValue(pchar, "Sailors_dead", iInf);
		// LEO: гибель раненых - КРАСНЫМ (негативное событие), парно к зелёному логу выздоровления ниже
		if(!IsCompanionTraveler)
		{
			if(LanguageGetLanguage() == "russian") { Log_InfoColored("red", "На корабле '" + rChar.Ship.Name + "' раненые гибнут без лекарств: " + iInf + "."); }
			else { Log_InfoColored("red", "Aboard '" + rChar.Ship.Name + "' the wounded die without medicine: " + iInf + "."); }
		}
		return;
	}

	// LEO: Инфекция действует и в море, но пополнение команды возможно только вне морской сцены.
	if(!Wounded_CanRecover()) return;
	float fRecDays = Wounded_BaseDays(rChar) + makefloat(dRand2(Wounded_RandSpan(rChar)));
	fRecDays = Wounded_ApplyBoosts(rChar, fRecDays);
	int iHeal = Wounded_HealPerDay(iWnd, fRecDays);

	int iMedsMax = GetCargoGoods(rChar, GOOD_MEDICAMENT) / WND_MEDS_PER_HEAL;
	if(iHeal > iMedsMax) iHeal = iMedsMax;
	int iRoom = GetMaxCrewQuantity(rChar) - GetCrewQuantity(rChar);
	if(iHeal > iRoom) iHeal = iRoom;
	if(iHeal < 1) return; // не хватило лекарств/места на одного - ждём

	RemoveCharacterGoodsSelf(rChar, GOOD_MEDICAMENT, iHeal * WND_MEDS_PER_HEAL);
	// LEO: строго ДО RemoveWounded - вес актива берётся до пополнения. Так ветеранский опыт возвращается
	// вместе с людьми: AddCharacterCrew характеристики не трогает.
	Wounded_MergeToCrew(rChar, iHeal);
	RemoveWounded(rChar, iHeal);
	AddCharacterCrew(rChar, iHeal); // назад в строй

	// LEO: опыт Защиты капитану этого корабля за выходивших раненых СВОИМИ силами: 1 матрос = 1 очко.
	//      Тавернский врач сюда не попадает: он лечит отдельной Wounded_TavernHealFleet ДО этого тика, лазарет уже пуст.
	AddCharacterExpToSkill(rChar, SKILL_DEFENCE, makefloat(iHeal));

	// LEO: инфо о выздоровлении - ЗЕЛЁНЫМ (позитивное событие)
	if(!IsCompanionTraveler)
	{
		int iLeft = iWnd - iHeal;
		string sHeal = "";
		if(LanguageGetLanguage() == "russian")
		{
			sHeal = "На корабле '" + rChar.Ship.Name + "' вернулось в строй раненых: " + iHeal + ".";
			if(iLeft > 0) sHeal = sHeal + " В лазарете осталось: " + iLeft + ".";
		}
		else
		{
			sHeal = "Aboard '" + rChar.Ship.Name + "' back to duty: " + iHeal + ".";
			if(iLeft > 0) sHeal = sHeal + " Still in sick bay: " + iLeft + ".";
		}
		Log_InfoColored("green", sHeal);
	}
}

// LEO: прогноз для подсказки ПКМ строки Команда: активные бусты (врач/перки ГГ + порт),
// лекарства на остаток раненых и оценка дней до ПОЛНОГО выздоровления (симуляция дрипа со средним рандом-хвостом).
string Wounded_ForecastText(ref rChar)
{
	int iWnd = GetWoundedQuantity(rChar);
	if(iWnd < 1) return "";
	bool bRu = (LanguageGetLanguage() == "russian");
	if(!Wounded_CanRecover())
	{
		string sAtSea;
		if(bRu) sAtSea = "\n\nЛазарет: " + iWnd + " ранен. Восстановление только на суше или глобальной карте.";
		else sAtSea = "\n\nSick bay: " + iWnd + " wounded. Recovery only ashore or on the world map.";
		if(GetCargoGoods(rChar, GOOD_MEDICAMENT) < 1)
		{
			if(bRu) return sAtSea + "\nЛекарств нет: раненые гибнут от заражения!";
			return sAtSea + "\nNo medicine: the wounded die of infection!";
		}
		if(CheckShipSituationDaily_GenQuest(rChar) > 1)
		{
			if(bRu) return sAtSea + "\nРаненые гибнут из-за эпидемии!";
			return sAtSea + "\nThe wounded are dying from the epidemic!";
		}
		return sAtSea;
	}

	// врач в таверне оплачен -> завтра все в строю (прогноз по дням не нужен)
	if(CheckAttribute(GetMainCharacter(), "Ship.Crew.woundedTavernHeal"))
	{
		if(bRu) { return "\n\nЛазарет: " + iWnd + " ранен. Лечение у городского врача ОПЛАЧЕНО - к утру все в строю."; }
		return "\n\nSick bay: " + iWnd + " wounded. Town doctor PAID - all back on duty by morning.";
	}

	// LEO: тик КАЖДЫЙ день катит dRand2 заново, поэтому точное число на завтра предсказать НЕЛЬЗЯ.
	//      Завтра -> ГРАНИЦЫ (быстрый ролл = хвост 0, медленный = хвост iRandSpan): один ролл, реальность всегда внутри.
	//      Полный срок -> оценка по СРЕДНЕМУ хвосту: за несколько дней роллы усредняются.
	//      (Раньше и завтрашнее число считалось по среднему - обещало 292, а тик давал 255.)
	float fBaseRaw = Wounded_BaseDays(rChar);
	int iRandSpan = Wounded_RandSpan(rChar);
	float fDoc = Wounded_DocPerkSpeed(rChar);
	bool bPort = Wounded_PlayerInPort();

	float fFast = Wounded_ApplyBoosts(rChar, fBaseRaw);
	float fSlow = Wounded_ApplyBoosts(rChar, fBaseRaw + makefloat(iRandSpan));
	float fAvg  = Wounded_ApplyBoosts(rChar, fBaseRaw + makefloat(iRandSpan) * 0.5);

	int iTomorrowMax = Wounded_HealPerDay(iWnd, fFast); // быстрый ролл -> в строй больше
	int iTomorrowMin = Wounded_HealPerDay(iWnd, fSlow);
	int iDays = Wounded_SimDays(iWnd, fAvg);

	string sTom = "" + iTomorrowMin;
	if(iTomorrowMax != iTomorrowMin) sTom = "" + iTomorrowMin + "-" + iTomorrowMax;

	int iMedsNeed = iWnd * WND_MEDS_PER_HEAL;
	int iMedsHave = GetCargoGoods(rChar, GOOD_MEDICAMENT);
	int iDocPct = makeint(fDoc * 100.0);
	int iPortPct = makeint(WND_PORT_SPEEDUP * 100.0);

	string s = "";
	if(bRu)
	{
		s = "\n\nЛазарет: " + iWnd + " ранен.  Не влияют на ТТХ.";
		s = s + "\nРасчётное время лечения: ";
		if(fDoc > 0.0) { s = s + "врач -" + iDocPct + "%"; } else { s = s + "врача нет"; }
		if(bPort) { s = s + ", порт -" + iPortPct + "%"; }
		s = s + "\nЛекарства: " + iMedsHave + " (по " + WND_MEDS_PER_HEAL + " на раненого разово; на всех " + iWnd + " нужно " + iMedsNeed + ")";
		if(iMedsHave < 1) { s = s + "\nЛекарств нет - раненые гибнут от заражения!"; }
		else if(iMedsHave < iMedsNeed) { s = s + "\nСейчас хватит вылечить " + (iMedsHave / WND_MEDS_PER_HEAL) + " из " + iWnd + " (остальные ждут лекарств, не гибнут)."; }
		else { s = s + "\nХватает на всех."; }
		s = s + "\nЗавтра в строй: " + sTom + " (как повезёт), полностью за ~" + iDays + " дн.";
	}
	else
	{
		s = "\n\nSick bay: " + iWnd + " wounded.  Do not affect stats.";
		s = s + "\nCalculated healing time: ";
		if(fDoc > 0.0) { s = s + "doctor -" + iDocPct + "%"; } else { s = s + "no doctor"; }
		if(bPort) { s = s + ", port -" + iPortPct + "%"; }
		s = s + "\nMedicine: " + iMedsHave + " (" + WND_MEDS_PER_HEAL + " per wounded, one-time; " + iMedsNeed + " to heal all " + iWnd + ")";
		if(iMedsHave < 1) { s = s + "\nNo medicine - the wounded die of infection!"; }
		else if(iMedsHave < iMedsNeed) { s = s + "\nEnough to heal " + (iMedsHave / WND_MEDS_PER_HEAL) + " of " + iWnd + " now (the rest wait for medicine, no deaths)."; }
		else { s = s + "\nEnough for all."; }
		s = s + "\nBack to duty tomorrow: " + sTom + " (luck of the draw), full recovery in ~" + iDays + " days.";
	}
	return s;
}

// LEO: раненых нельзя оставить на уходящем из флота корабле (продажа/аукцион/ПУ-сток) — «в лазарет» (обнулить) + подсказка.
void Wounded_ClearToInfirmary(ref chr)
{
	int iW = GetWoundedQuantity(chr);
	if(iW < 1) return;
	SetWoundedQuantity(chr, 0);
	if(LanguageGetLanguage() == "russian") { Log_Info("Раненые (" + iW + ") с корабля отправлены в городской лазарет."); }
	else { Log_Info("The wounded (" + iW + ") from the ship were sent to the town infirmary."); }
}

// LEO: всего раненых по флоту (флагман + компаньоны)
int Wounded_FleetTotal(ref chr)
{
	int iTot = 0;
	int i, cn;
	for(i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(chr, i);
		if(cn >= 0) iTot = iTot + GetWoundedQuantity(GetCharacter(cn));
	}
	return iTot;
}

// LEO: цена лечения раненых ОДНОГО корабля у тавернского врача - по числу × навыку (элитные дороже)
int Wounded_ShipHealCost(ref rChar)
{
	int iW = GetWoundedQuantity(rChar);
	if(iW < 1) return 0;
	int iExp = 50;
	if(CheckAttribute(rChar, "Ship.Crew.Exp.Sailors")) iExp = sti(rChar.Ship.Crew.Exp.Sailors);
	if(iExp < 1)   iExp = 1;
	if(iExp > 100) iExp = 100;
	float fMul = 0.5 + makefloat(iExp) / 100.0; // новобранец ×0.5 .. морской волк ×1.5
	return makeint(makefloat(iW) * makefloat(WND_TAVERN_COST_PER) * fMul);
}

// LEO: суммарная цена лечения раненых всего флота у тавернского врача
int Wounded_TavernCost(ref chr)
{
	int iCost = 0;
	int i, cn;
	for(i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(chr, i);
		if(cn >= 0) iCost = iCost + Wounded_ShipHealCost(GetCharacter(cn));
	}
	return iCost;
}

// LEO: врач оплачен - разом ставим в строй ВСЕХ раненых флота (на смену суток). Клампим по вместимости (страховка).
void Wounded_TavernHealFleet(ref chr)
{
	int i, cn, iW, iRoom;
	ref rs;
	for(i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(chr, i);
		if(cn < 0) continue;
		rs = GetCharacter(cn);
		iW = GetWoundedQuantity(rs);
		if(iW < 1) continue;
		iRoom = GetMaxCrewQuantity(rs) - GetCrewQuantity(rs);
		if(iW > iRoom) iW = iRoom;
		if(iW < 1) continue;
		Wounded_MergeToCrew(rs, iW); // LEO: вернуть опыт/мораль лазарета в активный пул (строго ДО RemoveWounded)
		RemoveWounded(rs, iW);
		AddCharacterCrew(rs, iW);
	}
	if(LanguageGetLanguage() == "russian") { Log_Info("Городской лекарь поставил ваших раненых на ноги."); }
	else { Log_Info("The town doctor has got your wounded back on their feet."); }
}
// =================== LEO: конец блока выздоровления раненых ===================

// Warship. Вынес в отдельный метод
void DailyEatCrewUpdateForShip(ref rChar, bool IsCompanionTraveler) // IsCompanionTraveler - спец флаг для компаньонов-путешественников
{
	int iCrewQty = GetCrewQuantity(rChar);
	int cn, morale, nMoraleDecreaseQ, iDeadCrew;
	if(iCrewQty < 1 && GetCargoGoods(rChar, GOOD_SLAVES) < 1 && GetWoundedQuantity(rChar) < 1) return; // LEO: +раненые нуждаются в тике
	if(!CheckAttribute(rChar, "Ship.Crew.Morale"))
	{
		rChar.Ship.Crew.Morale = 50;
	}
	Wounded_DailyHeal(rChar, IsCompanionTraveler); // LEO: суточное выздоровление/инфекция раненых
	iCrewQty = GetCrewQuantity(rChar); // LEO: перечитать - лечение могло вернуть раненых в актив, иначе ветки смертей ниже затрут вылеченных сталым значением
	// расчет медицины -->
	if(rand(4) == 2)
	{
		// матросы
		cn = iCrewQty / 10;
		if(cn > 30) cn = 30;
		cn = rand(cn)+1;
		if(iCrewQty < cn) cn = iCrewQty;
		if(cn > 0)
		{
			if(GetCargoGoods(rChar, GOOD_MEDICAMENT) < 1)
			{
				if(!IsCompanionTraveler) Log_Info(GetConvertStrWithReplace("Variable_food_5", "Logs.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(rChar.Ship.Type)].BaseName+"Voc") + " '" + rChar.Ship.Name + GetConvertStrWithReplace("Variable_food_6", "Logs.txt", "#space#", " ") + FindRussianSailorString(cn, "No") + "!");
				iCrewQty = iCrewQty - cn;
				Statistic_AddValue(pchar, "Sailors_dead", cn);
				rChar.Ship.Crew.Quantity = iCrewQty;
				// мораль в минус
				morale = sti(rChar.Ship.Crew.Morale);

				if(CheckOfficersPerk(rChar, "IronWill")) cn /= 1.5;

				AddCrewMorale(rChar, -makeint(cn / 2)); // до 15 пунктов за раз
			}
			else
			{
				if(CheckShipSituationDaily_GenQuest(rChar) == 2) cn = cn * 2;
				if(CheckShipSituationDaily_GenQuest(rChar) == 3) cn = cn * 3;

				RemoveCharacterGoodsSelf(rChar, GOOD_MEDICAMENT, cn);
				if(GetCargoGoods(rChar, GOOD_MEDICAMENT) < 16)
				{
					if(!IsCompanionTraveler) Log_Info(GetConvertStrWithReplace("Variable_food_7", "Logs.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(rChar.Ship.Type)].BaseName+"Voc") + " '" + rChar.Ship.Name + GetConvertStrWithReplace("Variable_food_8", "Logs.txt", "#space#", " "));
				}
			}
		}
		// рабы
		cn = GetCargoGoods(rChar, GOOD_SLAVES) / 10;
		if(cn > 30) cn = 30;
		cn = rand(cn)+1;
		if(GetCargoGoods(rChar, GOOD_SLAVES) < cn) cn = GetCargoGoods(rChar, GOOD_SLAVES);
		if(cn > 0)
		{
			if(GetCargoGoods(rChar, GOOD_MEDICAMENT) < 1)
			{
				if(!IsCompanionTraveler) Log_Info(GetConvertStrWithReplace("Variable_food_9", "Logs.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(rChar.Ship.Type)].BaseName+"Voc") + " '" + rChar.Ship.Name + GetConvertStrWithReplace("Variable_food_10", "Logs.txt", "#space#", " ") + FindRussianSailorString(cn, "No") + "!");
				RemoveCharacterGoodsSelf(rChar, GOOD_SLAVES, cn);
			}
			else
			{
				cn /= 3;
				RemoveCharacterGoodsSelf(rChar, GOOD_MEDICAMENT, cn);
			}
		}
		// повторный контроль
		if(iCrewQty < 1 && GetCargoGoods(rChar, GOOD_SLAVES) < 1) return;
	}
	// расчет медицины <--

	iCrewQty = GetCrewQuantity(rChar) + GetWoundedQuantity(rChar); // LEO: раненые тоже пьют ром
	iCrewQty = makeint(isEquippedArtefactUse(rChar, "talisman4", 1.0, 0.2) *((iCrewQty+5.1) / RUM_BY_CREW)); // eat ratio
	if(iCrewQty < 1) iCrewQty = 1;
	if(iCrewQty > 0)
	{
		if(GetCargoGoods(rChar, GOOD_RUM) >= iCrewQty)
		{
			RemoveCharacterGoodsSelf(rChar, GOOD_RUM, iCrewQty);
			// проверка на остатки
			cn = makeint(GetCargoGoods(rChar, GOOD_RUM) / iCrewQty);
			if (cn < 1)
			{
				if(!IsCompanionTraveler) Log_Info(GetConvertStrWithReplace("Variable_food_11", "Logs.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(rChar.Ship.Type)].BaseName+"Voc") + " '" + rChar.Ship.Name + GetConvertStrWithReplace("Variable_food_12", "Logs.txt", "#space#", " "));
			}
			// поднимем мораль
			if(CheckShipSituationDaily_GenQuest(rChar) == 1) AddCrewMorale(rChar, 2);
		}
		else
		{
			iCrewQty = GetCargoGoods(rChar, GOOD_RUM);
			RemoveCharacterGoodsSelf(rChar, GOOD_RUM, iCrewQty);
		}
	}
	iCrewQty = GetCrewQuantity(rChar);
	// рассчет перегруза команды на мораль -->
	if(iCrewQty > GetOptCrewQuantity(rChar) && !IsEquipCharacterByArtefact(rChar, "talisman4"))
	{
		AddCrewMorale(rChar, -(1+rand(3)));
	}
	// рассчет перегруза команды на мораль <--

	// расчет долга на мораль
	if(iCrewQty > 0 && CheckAttribute(PChar, "CrewPayment"))
	{
		cn = makeint(PChar.CrewPayment);
		if(cn > 32000) cn = 32000;
		if(rand(cn) > 1000)
		{
			AddCrewMorale(rChar, -1);
			cn = 5 + CheckOfficersPerk(PChar, "IronWill");  // перк у ГГ
			if(i > 0 && rand(cn) == 2 && !CheckAttribute(rChar, "OfficerWantToGo.DontGo") && !IsEquipCharacterByArtefact(rChar, "talisman17"))
			{
				rChar.loyality = sti(rChar.loyality) - 1;
			}
		}
	}
	// расчет еды после Рома
	iCrewQty = makeint(isEquippedArtefactUse(rChar, "talisman6", 1.0, 0.75) * (iCrewQty + GetWoundedQuantity(rChar) + 5.1) / FOOD_BY_CREW + GetPassengersQuantity(rChar) / FOOD_BY_PASSENGERS); // LEO: +раненые едят
	iCrewQty = iCrewQty + makeint((GetCargoGoods(rChar, GOOD_SLAVES)+6)/ FOOD_BY_SLAVES);  // учет рабов
	if(iCrewQty == 0) iCrewQty = 1;
	if(GetCargoGoods(rChar, GOOD_FOOD) >= iCrewQty)
	{
		RemoveCharacterGoodsSelf(rChar, GOOD_FOOD, iCrewQty);
		// проверка на остатки
		cn = makeint(GetCargoGoods(rChar, GOOD_FOOD) / iCrewQty);
		if (cn < 4)
		{
			if(!IsCompanionTraveler)
			{
				Log_Info(GetConvertStrWithReplace("Variable_food_13", "Logs.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(rChar.Ship.Type)].BaseName+"Voc") + " '" + rChar.Ship.Name + GetConvertStrWithReplace("Variable_food_14", "Logs.txt", "#space#", " ") + FindRussianDaysString(cn) + "!");
				Log_Info(GetConvertStrWithReplace("Variable_food_15", "Logs.txt", "#space#", " "));
				PlaySound("interface\notebook.wav");
			}
		}
		// возможный бунт ПЛЕННИКОВ: до 5 - безопасно, каждый сверх +10% шанса, чек раз в сутки, только на глобалке.
		// Пленники добрались до арсенала и пытаются захватить корабль - в бунте участвуют все они.
		if (sti(rChar.index) == GetMainCharacterIndex() && IsEntity(worldMap) && !IsEquipCharacterByArtefact(rChar, "talisman16"))
		{
			int nPrisRiot = GetPrisonerQty();
			if (nPrisRiot > 5 && GetCrewQuantity(rChar) > 0)
			{
				if (rand(99) < (nPrisRiot - 5) * 10)
				{
					rChar.GenQuest.PrisonerMunity = true;
					if (LanguageGetLanguage() == "russian") Log_Info("Пленники добрались до сабель в арсенале и подняли бунт - они рвутся захватить корабль!");
					else Log_Info("The prisoners got to the sabres in the arsenal and rose up - they are out to seize the ship!");
					MunityOnShip("PrisonerMunity");
				}
			}
		}
		// возможный бунт рабов
		if (sti(rChar.index) == GetMainCharacterIndex() && GetCargoGoods(rChar, GOOD_SLAVES) > (GetCrewQuantity(rChar)*1.5 + sti(rChar.Ship.Crew.Morale)))
		{
			nMoraleDecreaseQ = 12 - GetSummonSkillFromNameToOld(rChar, SKILL_LEADERSHIP);
			if(CheckOfficersPerk(rChar, "IronWill")) nMoraleDecreaseQ /= 2;
			if(rand(2) == 1 && nMoraleDecreaseQ > rand(10))
			{
				if(IsEntity(worldMap))
				{
					rChar.GenQuest.SlavesMunity = true;
					Log_Info(GetConvertStrWithReplace("Variable_food_16", "Logs.txt", "#space#", " "));
					MunityOnShip("SlavesMunity");
				}
			}
		}
	}
	else
	{
		iCrewQty = GetCargoGoods(rChar, GOOD_FOOD);
		RemoveCharacterGoodsSelf(rChar, GOOD_FOOD, iCrewQty);
		PlaySound("interface\notebook.wav");

		if(!IsCompanionTraveler) Log_Info(GetConvertStrWithReplace("Variable_food_17", "Logs.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(rChar.Ship.Type)].BaseName+"Voc") + " '" + rChar.Ship.Name + GetConvertStrWithReplace("Variable_food_18", "Logs.txt", "#space#", " "));

		if(sti(rChar.index) == GetMainCharacterIndex())
		{
			AddCharacterHealth(PChar, -1);
		}

		cn = GetCrewQuantity(rChar);
		if(cn > 1)
		{
			iDeadCrew = makeint(cn/10 +0.5);
			rChar.Ship.Crew.Quantity = cn - iDeadCrew;
			Statistic_AddValue(pchar, "Sailors_dead", iDeadCrew);
			if(!IsCompanionTraveler) Log_Info(GetConvertStrWithReplace("Variable_food_19", "Logs.txt", "#space#", " "));
		}
		// LEO: раненые тоже гибнут от голода
		iDeadCrew = GetWoundedQuantity(rChar);
		if(iDeadCrew > 0)
		{
			iDeadCrew = makeint(iDeadCrew/10 + 0.5);
			if(iDeadCrew > 0)
			{
				RemoveWounded(rChar, iDeadCrew);
				Statistic_AddValue(pchar, "Sailors_dead", iDeadCrew);
			}
		}
		cn = GetCargoGoods(rChar, GOOD_SLAVES);
		if(cn > 0)
		{
			RemoveCharacterGoodsSelf(rChar, GOOD_SLAVES, makeint(cn/5 + 0.5));
			if(!IsCompanionTraveler) Log_Info(GetConvertStrWithReplace("Variable_food_20", "Logs.txt", "#space#", " "));
		}
		morale = sti(rChar.Ship.Crew.Morale);

		nMoraleDecreaseQ = 12 - GetSummonSkillFromNameToOld(rChar, SKILL_LEADERSHIP);
		if(CheckOfficersPerk(rChar, "IronWill")) nMoraleDecreaseQ /= 2;
		rChar.Ship.Crew.Morale = morale - nMoraleDecreaseQ;
		if(sti(rChar.Ship.Crew.Morale) < MORALE_MIN) rChar.Ship.Crew.Morale = MORALE_MIN;
	}

	if(sti(rChar.index) == GetMainCharacterIndex())
	{
		if(sti(rChar.Ship.Crew.Morale) <= MORALE_MIN)
		{
			if(IsEntity(worldMap) && GetCrewQuantity(rChar) > 0 && !IsCharacterEquippedArtefact(rChar, "talisman16"))
			{
				Log_Info(GetConvertStrWithReplace("Variable_food_21", "Logs.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(rChar.Ship.Type)].BaseName+"Voc") + " '" + rChar.Ship.Name + GetConvertStrWithReplace("Variable_food_22", "Logs.txt", "#space#", " "));
				MunityOnShip("ShipMunity");
			}
		}
	}
	else
	{
		if(GetShipRemovable(rChar) && !CheckAttribute(rChar, "OfficerWantToGo.DontGo") && !IsCompanionTraveler && !IsEquipCharacterByArtefact(rChar, "talisman17")) // ПГГ, квестовые оффы и компаньоны-путешественники не бунтуют
		{
			if(sti(rChar.Ship.Crew.Morale) <= MORALE_MIN || sti(rChar.loyality) <= 0) // допуск, что лояльность есть у всех офов
			{
				if(GetCrewQuantity(rChar) > 0 && !IsCharacterEquippedArtefact(rChar, "talisman16"))
				{
					Log_Info(GetConvertStrWithReplace("Variable_food_23", "Logs.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(rChar.Ship.Type)].BaseName+"Voc") + " '" + rChar.Ship.Name + GetConvertStrWithReplace("Variable_food_24", "Logs.txt", "#space#", " "));
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_food_25", "Logs.txt", "#space#", " "));
					Statistic_AddValue(PChar, "ShipMunity", 1);
					RemoveCharacterCompanion(PChar, rChar);
					//fix  ПГГ
					if(!CheckAttribute(rChar, "PGGAi"))
					{
						rChar.LifeDay = 0; // стереть при выходе
					}
					else
					{
						rChar.PGGAi.IsPGG = true;
						rChar.RebirthPhantom = true;
						rChar.PGGAi.location.town = PGG_FindRandomTownByNation(sti(rChar.nation));
						rChar.Dialog.FileName = "PGG_Dialog.c";
						rChar.Dialog.CurrentNode = "Second Time";
						PGG_ChangeRelation2MainCharacter(rChar, -20);
					}
					rChar.location = ""; // нафиг, нафиг..а то в таверне появлялся...
					rChar.location.group = "";
					rChar.location.locator = "";
				}
			}
		}
	}
}
