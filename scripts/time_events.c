
void ActivateTimeEvents()
{

}

//////////////////////// boal SLiB ////////////////////////////////
void SalaryNextDayUpdate()
{
	if (sti(NullCharacter.SalayPayMonth) != GetDataMonth() && !bDisableMapEnter) // boal
	{
		Log_QuestInfo("WorldSituationsUpdate SalaryNextDayUpdate");
        // проверка на наличие кому платить -->
        int nPaymentQ = 0;
        int i, cn;
        ref chref;

		for (i=0; i<COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(pchar, i);
			if (cn >= 0)
			{
				chref = GetCharacter(cn);
				if (GetRemovable(chref)) // считаем только своих, а то вских сопровождаемых кормить!!!
				{
				    nPaymentQ += GetSalaryForShip(chref);
				}
			}
		}
		nPaymentQ += CompanionTravel_GetTravellersSalary(); // LEO: + жалованье компаньонов на поручении/рейде (они вышли из эскадры, штатный цикл их не видит)
		// проверка на наличие кому платить <--
		NullCharacter.SalayPayMonth = GetDataMonth(); // boal
		if (nPaymentQ > 0)
		{
			if( CheckAttribute(pchar,"CrewPayment") )
			{
				nPaymentQ += makeint(pchar.CrewPayment); // а тут помним все до копейки!
			}
			if( CheckAttribute(pchar,"Partition.MonthPart") )
			{
				nPaymentQ += makeint(pchar.Partition.MonthPart); // доля за месяц
				DeleteAttribute(pchar,"Partition.MonthPart");
			}

			pchar.CrewPayment = nPaymentQ;
			LaunchMoneyGraphCollect();

			if (!dialogRun && !bQuestCheckProcessFreeze && !bAbordageStarted) // можно показать
			{
	      		LaunchSalaryScreen("");
			}
			else
			{
		        Pchar.quest.LaunchSalaryScreen.win_condition.l1 = "MapEnter";
		        PChar.quest.LaunchSalaryScreen.win_condition    = "LaunchSalaryScreen";
		        PChar.quest.LaunchSalaryScreen.function         = "LaunchSalaryScreen";
	        }
        }
	}
}

void ShipYarderSkillNextDayUpdate()
{
	int iCharIndex; 
	if(!CheckAttribute(pchar,"ShipYarderSkill")) pchar.ShipYarderSkill = GetDataMonth();
	if(pchar.ShipYarderSkill != GetDataMonth())
	{
		Log_QuestInfo("WorldSituationsUpdate ShipYarderSkillNextDayUpdate");
		pchar.ShipYarderSkill = GetDataMonth();

		for(int n=0; n<MAX_COLONIES; n++)
		{
			if (!checkattribute(colonies[n], "HasNoFort"))//нет форта - нет и верфи, панама и бермуды всё равно исключения
			{
				iCharIndex = GetCharacterIndex(colonies[n].id + "_shipyarder"); 
				if (iCharIndex != -1)
				{
					characters[iCharIndex].reputation = sti(characters[iCharIndex].reputation) + rand(1);
					//То есть в среднем +0,25 навыка раз в месяц. +3 за год
				}
			}
		}
	}
}

void RepairDefendersNextDayUpdate()
{
	// В море откладываем ремонт, не расходуя отметку месяца.
	if(bSeaActive) return;
	if(!CheckAttribute(pchar,"repairDefenders")) pchar.repairDefenders = GetDataMonth();
	if(pchar.repairDefenders != GetDataMonth())
	{
		Log_QuestInfo("WorldSituationsUpdate RepairDefendersNextDayUpdate");
		pchar.repairDefenders = GetDataMonth();
		ref _rChar;//Lipsar ---> Ремонт сторожей
		for (int j=GlobalCharacters; j<MAX_CHARACTERS; j++)
		{
			if (CheckAttribute(&characters[j], "fortDefender") && CheckAttribute(&characters[j], "IslandShips"))
			{
				_rChar = &characters[j];
				if(GetCharacterShipType(_rChar) != SHIP_NOTUSED)
				{
					RepairMasts(_rChar);
					DeleteAttribute(_rChar, "ship.sails");
					// Чиним штатные стволы; отсутствующие слоты модели не добавляем.
					ref rShip = GetRealShip(sti(_rChar.Ship.Type));
					string sBort, sReal, sCannon;
					for(int i = 0; i < 4; i++)
					{
						switch(i)
						{
							case 0: sBort = "cannonl"; sReal = "lcannon"; break;
							case 1: sBort = "cannonr"; sReal = "rcannon"; break;
							case 2: sBort = "cannonf"; sReal = "fcannon"; break;
							case 3: sBort = "cannonb"; sReal = "bcannon"; break;
						}
						for(int k = 0; k < sti(rShip.(sReal)); k++)
						{
							sCannon = "c" + k;
							_rChar.Ship.Cannons.Borts.(sBort).damages.(sCannon) = 0.0;
							_rChar.Ship.Cannons.Borts.(sReal).damages.(sCannon) = 0.0;
						}
					}

					_rChar.Ship.HP = GetCharacterShipHP(_rChar);
					_rChar.Ship.SP = 100;
					SetCrewQuantityFull(_rChar);
					SetCharacterGoods(_rChar, GOOD_BALLS, 2000);
					SetCharacterGoods(_rChar, GOOD_GRAPES, 2000);
					SetCharacterGoods(_rChar, GOOD_KNIPPELS, 2000);
					SetCharacterGoods(_rChar, GOOD_BOMBS, 2000);
					SetCharacterGoods(_rChar, GOOD_POWDER, 8000);
					SetCharacterGoods(_rChar, GOOD_FOOD, 2000);
					SetCharacterGoods(_rChar, GOOD_WEAPON, 1500);
					SetCharacterGoods(_rChar, GOOD_MEDICAMENT, 1000);
					SetCharacterGoods(_rChar, GOOD_PLANKS, 800);
					SetCharacterGoods(_rChar, GOOD_SAILCLOTH, 800);
				}
			}
		}
	}
}


#event_handler("EvSituationsUpdate","WorldSituationsUpdate");
void WorldSituationsUpdate()
{
	int iStep = GetEventData();
	float dayRandom;
	float dayRandom1;
	float dayRandom2;
	float dayRandom3;

	// LEO: раньше только шаги 0..9 (слайс PsHeroQty/10 = ~6-7 ПГГ/шаг -> фриз). Теперь роллинг-курсор внутри
	//   PGG_Updates (по PGG_UPDATES_PER_STEP за вызов) -> зовём на ВСЕХ шагах, чтобы курсор обходил всех ПГГ
	//   быстрее (шаги 12..36 — почти пустой падинг, добавка 1 ПГГ дёшева).
	PGG_Updates(iStep);

	switch(iStep)
	{
		case 0:
            DeleteAttribute(pchar, "SkipEshipIndex");// boal

			DailyEatCrewUpdate(); // boal
			Log_QuestInfo("WorldSituationsUpdate DailyEatCrewUpdate");
			Barrel_RatsCheck(); // LEO: крысы из выловленной бочки доели свой срок
			FiveArmies_TryCreate(); // LEO: пасхалка "битва пяти воинств"

			dayRandom = Random();
			PChar.DayRandom = dayRandom;
			Log_TestInfo("dayRandom == " + dayRandom);

			dayRandom1 = Random();
			PChar.DayRandom1 = dayRandom1;
			Log_TestInfo("dayRandom1 == " + dayRandom1);

			dayRandom2 = Random();
			PChar.DayRandom2 = dayRandom2;
			Log_TestInfo("dayRandom2 == " + dayRandom2);
			
			dayRandom3 = Random();
			PChar.DayRandom3 = dayRandom3;
			Log_TestInfo("dayRandom3 == " + dayRandom3);

			CheckOfficersHPMinus(); // ежедневное обновление дней для выздоровления офов
			CheckBook(); //Книги - Gregg
			Scheduler_CheckDaily(); //Планировщик (записи-будильники по дате) - Gregg
			CheckTrauma();//Тяжёлая травма
			TimerOnStartSilverFleet(); // СФ
		break;

		case 1:
            SalaryNextDayUpdate();  // зарплата
			ShipYarderSkillNextDayUpdate();
		break;

		case 2:
			// ушло в переходы локаций PGG_DailyUpdate(); // navy
			RepairDefendersNextDayUpdate();
			ProcessDayRepair();
		break;

		case 3:
			Group_FreeAllDead();
			Group_DropErasedMembers(); // LEO: и точечно - протухшие записи внутри ЖИВЫХ групп (лечит старые сейвы)
		break;

		case 4:
			QuestActions(); //eddy
		break;

		case 5:
			wdmEmptyAllOldEncounter();// homo чистка энкоутеров
		break;

		case 6:
			UpdateCrewExp();  // изменение опыта команды
			CheckTraining();//треня оффов
		break;

		case 7:
			UpdateCrewInColonies(); // пересчет наемников в городах
		break;

		case 8:
			if(IsEntity(worldMap))
			{
				EmptyAllFantomCharacter(); // трем НПС
				wdmEmptyAllDeadQuestEncounter();
			}
		break;

		case 9:

            UpdateFame();   // это теперь известность репутации
			GenerateRumour() //homo 05/07/06
		break;
		//добавляю до 36, но можно любое число, если ещё сократить время милисекунд в ивенте
		case 10:
			ChangeImport();
		break;

		case 11:
			UpdateSmugglers();
		break;
	}

	iStep++;
	InterfaceStates.WorldSituationUpdateStep = iStep;

	if(iStep <= 36)
	{
		PostEvent("EvSituationsUpdate", 1000, "l", iStep);
	}
}


//////////////////////////////// начало игры - туториал ////////////////////////////////
void Tut_StartGame(string sQuest)
{
    InterfaceStates.Buttons.Save.enable = false;
	StartQuestMovie(true, true, true);
    SetCharacterTask_None(GetMainCharacter());
    LAi_LockFightMode(Pchar, true);
	PChar.GenQuest.CallFunctionParam = "Tut_Continue";
	DoQuestCheckDelay("CallFunctionParam", 5.0);
	LAi_LocationFightDisable(loadedLocation, true);
	InterfaceStates.Buttons.Save.enable = false;
	DoQuestFunctionDelay("Tut_locCamera_1", 0.1);

	if (bQuestsRank)	//Если стоит галочка, то убрать требования рангов у квестов
	{
		DoQuestFunctionDelay("UbratTrebovanieRangov", 0.1);
	}
}

// LEO: старт без корабля — ГГ сразу в городе (без палубы Сандро Торна), но с возможностью построить свой билд
void Tut_StartGame_NoShip(string sQuest)
{
	InterfaceStates.Buttons.Save.enable = true;
	DoQuestFunctionDelay("Tut_StartNoShipBuild", 1.0);	// LEO: дать локации/фейду устаканиться, затем запустить мысли вслух с выбором билда
}

void Tut_StartNoShipBuild(string _tmp)
{
	// LEO: через self-диалог ГГ (механизм как у Шарпов) — узел сам откроет экран распределения характеристик
	StartActorSelfDialog("Tut_NoShipStart");
}

void Tut_locCamera_1(string _tmp)
{
    locCameraToPos(-5, 2.5, 5.6, true);
	ChangeShowIntarface();
    DoQuestFunctionDelay("Tut_locCamera_2", 10.0);
}

void Tut_locCamera_2(string _tmp)
{
    locCameraFollow();
}
void Tut_Continue()
{
    ref sld;
    LAi_LocationFightDisable(loadedLocation, false);
    LAi_SetFightMode(Pchar, false);
    LAi_LockFightMode(pchar, true);

	if(rand(95)==0)
	{
		switch (rand(1))
		{
			case 0:
				sld = GetCharacter(NPC_GenerateCharacter("Sailor_1", "Skelt", "man", "man", 1, PIRATE, 0, false));
				sld.name 	= "T-800";
				sld.lastname 	= "";
				sld.Dialog.CurrentNode = "Alt_start";
				sld.dialog.filename = "Quest\StartGame_dialog.c";
				sld.greeting = "";
			break;
			case 1:
				StopSound(0,0);
				PlayStereoOGG("CSR\Music\Sea\Deck_Nekro.ogg");
				sld = GetCharacter(NPC_GenerateCharacter("Sailor_1", "Skel_5", "man", "man", 1, PIRATE, 0, false));
				sld.name 	= GetConvertStr("Variable_time_events_0.1", "Names.txt");
				sld.lastname 	= GetConvertStr("Variable_black_sails_functions_0.1", "Names.txt");
				sld.Dialog.CurrentNode = "Alt_start2";
				sld.dialog.filename = "Quest\StartGame_dialog.c";
				sld.greeting = "";
			break;
		}
	}
	else
	{
		sld = GetCharacter(NPC_GenerateCharacter("Sailor_1", "Pirate_9", "man", "man", 1, PIRATE, 0, false));
		sld.name 	= GetConvertStr("Variable_time_events_0.2", "Names.txt");
		sld.lastname 	= GetConvertStr("Variable_black_sails_functions_0.1.1", "Names.txt");
		sld.Dialog.CurrentNode = "First time";
		sld.dialog.filename = "Quest\StartGame_dialog.c";
		sld.greeting = "Teacher_pirat";
	}

    SetSPECIAL(sld, 9,8,10,3,6,10,4);
    InitStartParam(sld);
    SetEnergyToCharacter(sld);

    LAi_SetCheckMinHP(sld, 1, true, "Tut_StartGame_CheckMinHP_1");
    GiveItem2Character(sld, "blade1");
	EquipCharacterByItem(sld, "blade1");
	// пустоля нет, лечилок нет
    ChangeCharacterAddressGroup(sld, "Ship_deck_Low", "reload", "reload1");
    LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", 5.0, 0);

	// генерим второго матроса, но пока не ставим
	sld = GetCharacter(NPC_GenerateCharacter("Sailor_2", "shipowner_2", "man", "man", 1, PIRATE, 0, false));
    sld.name 	= GetConvertStr("Variable_time_events_0.3", "Names.txt");
    sld.lastname 	= GetConvertStr("Variable_time_events_0.3.1", "Names.txt");

    SetSPECIAL(sld, 8,10,9,3,6,10,4);
    InitStartParam(sld);
    SetEnergyToCharacter(sld);

    LAi_SetCheckMinHP(sld, 1, true, "Tut_StartGame_CheckMinHP_2");
    GiveItem2Character(sld, "blade4");
	EquipCharacterByItem(sld, "blade4");
	GiveItem2Character(sld, "pistol2");
	EquipCharacterByItem(sld, "pistol2");
	TakeNItems(sld, "bullet", 30);
	AddItems(sld, "GunPowder", 30);
	// лечилок нет
    sld.location = "Ship_deck_Low";  // чтоб не терся, пока его нет
	//раскидаем квестовых нпс по локациям
	SetQuestsCharacters();
}

void Tut_RestoreState()
{
	ref sld;

	pchar.Health.Damg = 0.0; // здоровье бережем
	pchar.Health.weekDamg = 0.0;

	LAi_SetCurHPMax(pchar);
	LAi_SetCheckMinHP(pchar, 1, true, "Tut_StartGame_CheckMinHP_Hero");
	LAi_SetImmortal(pchar, false);

	sld = characterFromID("Sailor_1");
	LAi_SetCurHPMax(sld);
	LAi_SetCheckMinHP(sld, 1, true, "Tut_StartGame_CheckMinHP_1");
	LAi_SetImmortal(sld, false);

	sld = characterFromID("Sailor_2");
	LAi_SetCurHPMax(sld);
	LAi_SetCheckMinHP(sld, 1, true, "Tut_StartGame_CheckMinHP_2");
	LAi_SetImmortal(sld, false);

	DeleteAttribute(pchar, "HeroParam.Teach_beat");
}

void Tut_TeachBattle()
{
	ref sld;

	LAi_SetPlayerType(pchar);
	LAi_SetFightMode(Pchar, true);
	sld = characterFromID("Sailor_1");
	LAi_SetWarriorType(sld);
    LAi_group_MoveCharacter(sld, "TmpEnemy");

	if (sti(pchar.HeroParam.Teach_battle) == 2)
	{
        sld = characterFromID("Sailor_2");
		if (!IsEntity(sld))
        {
        	ChangeCharacterAddressGroup(sld, "Ship_deck_Low", "reload", "reload1");
        }
		LAi_SetWarriorType(sld);
	    LAi_group_MoveCharacter(sld, "TmpEnemy");
	}
	LAi_group_SetHearRadius("TmpEnemy", 100.0);
    LAi_group_FightGroupsEx("TmpEnemy", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
    LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
}

void Tut_StartDialog()
{
	ref sld = characterFromID("Sailor_1");

	if (CheckAttribute(pchar, "HeroParam.Teach_beat"))
	{ // признак, что выиграл
		if (sti(pchar.HeroParam.Teach_beat) == 3)
		{
		    sld.Dialog.CurrentNode = "Teach_battle_win_2";
		}
		else
		{
			if (sti(pchar.HeroParam.Teach_battle) == 1)
			{
				sld.Dialog.CurrentNode = "Teach_battle_win";
			}
			else
			{
			    sld.Dialog.CurrentNode = "Teach_battle_win_3";
			}
		}
	}
	else
	{
		sld.Dialog.CurrentNode = "Teach_battle_lose";
	}
	LAi_ActorWaitDialog(pchar, sld);
	LAi_ActorDialog(sld, pchar, "", 4.0, 0);
}
//поставить НПС-раздатчиков квеста в нужные города по нации ГГ
void SetQuestsCharacters()
{
	ref sld;
	string sTown;
	int iNation = sti(pchar.nation);
	switch (iNation)
	{
		case 0: sTown = "PortRoyal"; break;
		case 1: sTown = "Tortuga";	break;
		case 2:	sTown = "Havana";	break;
		case 3:	sTown = "Villemstad"; break;
		case 4:
			sTown = "PortRoyal";
			iNation = ENGLAND;
		break;
	}

}

// выздоровление офицеров после потери сознания.
//UPD Lipsar - убрал ненужные методы для пассажиров и компаньонов, так как GetNotCaptivePassengersQuantity видит и офицров и пассажиров
//пленников игнорирует. Иначе по 2 дня за 1 считало выздоровление (шло гораздо быстрее) из за пересечения метода поисков офицеров и пассажиров!
void CheckOfficersHPMinus()
{
	ref sld;
	int i, iPassenger;

	int iPassQ = GetNotCaptivePassengersQuantity(pchar);
	for(i=0; i < iPassQ; i++)
	{
		iPassenger = GetPassenger(pchar, i);
		sld = GetCharacter(iPassenger);
		if (CheckAttribute(sld, "HPminusDays")) sld.HPminusDays = sti(sld.HPminusDays) + 1;

		if(CheckAttribute(sld, "HPminusDays") && CheckAttribute(sld, "HPminusDaysNeedtoRestore"))
		{
			if(sti(sld.HPminusDays) >= sti(sld.HPminusDaysNeedtoRestore))
			{
				DeleteAttribute(sld, "HPminusDays");
				DeleteAttribute(sld, "HPminusDaysNeedtoRestore");
				DeleteAttribute(sld, "Doctored");
				Log_InfoColored("green", GetConvertStrWithReplace("Variable_time_events_3", "Logs.txt", "#space#", " ") + GetFullName(sld) + GetConvertStrWithReplace("Variable_time_events_4", "Logs.txt", "#space#", " ")); // LEO: выздоровление офицера - зелёным (без звука)
			}
			UnmarkCharacter(sld);
		}
	}
	ReturnAutoOfficers(); // выздоровевших офицеров (и освободившихся ex-капитанов) вернуть на их прежние должности
}

void CheckBook()//Проверка книги только на глобалке - Gregg
{
	if (CheckAttribute(pchar,"bookreadtoday"))//учёт чтения на суше, блок полночного чтения на глобалке
	{
		DeleteAttribute(pchar,"bookreadtoday");
		return;
	}

	if (IsEntity(&worldMap) != 0)//учёт чтения на глобалке
	{
		TryReadBook();
	}
}

void CheckTrauma() //тяжёлая травма - Lipsar и Gregg
{
	// LEO: лечение тяжёлой травмы (chr_ai.HeavyTrauma, штраф -30% к навыкам в ApplayNavyPenaltyToSkill)
	// у ОФИЦЕРОВ. Раньше блок был закомментирован -> ТТ офицеров не снималась НИГДЕ (аптекарь её не трогал,
	// бордель и CheckTrauma - только ГГ) -> вечный штраф к навыкам. Тикаем по дням, как у ГГ ниже.
	ref chr;
	int i, idx;
	for(i=0; i<GetPassengersQuantity(pchar); i++)
	{
		idx = GetPassenger(pchar, i);
		if(idx < 0) continue;
		chr = GetCharacter(idx);
		if(CheckAttribute(chr, "chr_ai.HeavyTrauma"))
		{
			chr.chr_ai.HeavyTrauma = sti(chr.chr_ai.HeavyTrauma) - 1;
			if(sti(chr.chr_ai.HeavyTrauma) <= 0)
			{
				DeleteAttribute(chr, "chr_ai.TraumaQ");
				DeleteAttribute(chr, "chr_ai.HeavyTrauma");
				Log_InfoColoredAnim("green", GetConvertStrWithReplace("Variable_time_events_7", "Logs.txt", "#space#", " ") + chr.Name + GetConvertStrWithReplace("Variable_time_events_8", "Logs.txt", "#space#", " ")); // LEO: выздоровление офицера от тяжёлой травмы — зелёный + вспышка + шейк
			}
		}
	}
	UnmarkCharacter(PChar);
	if(CheckAttribute(PChar, "chr_ai.Trauma"))//только ГГ, так как здоровье меняется только у ГГ
    {
		DeleteAttribute(PChar, "chr_ai.Trauma");
		Log_InfoColoredAnim("green", GetConvertStrWithReplace("Variable_time_events_9", "Logs.txt", "#space#", " ")); // LEO: выздоровление ГГ от травмы — зелёный + вспышка + шейк
	}
    if(CheckAttribute(PChar, "chr_ai.HeavyTrauma"))//только ГГ, так как здоровье меняется только у ГГ
    {
        PChar.chr_ai.HeavyTrauma = sti(PChar.chr_ai.HeavyTrauma) - 1;
        if(sti(PChar.chr_ai.HeavyTrauma) <= 0)
        {
			DeleteAttribute(PChar, "chr_ai.TraumaQ");
            DeleteAttribute(PChar, "chr_ai.HeavyTrauma");
            Log_InfoColoredAnim("green", GetConvertStrWithReplace("Variable_time_events_10", "Logs.txt", "#space#", " ")); // LEO: выздоровление ГГ от тяжёлой травмы — зелёный + вспышка + шейк
			CheckAndSetOverloadMode(pchar);
        }
    }
}

void CheckTraining()//треня офов
{
	if (!CheckAttribute(pchar,"MalteseOrder") && !CheckAttribute(pchar,"PirateOrder")) return;
	ref chref;
	for(i=1; i<MAX_CHARACTERS; i++)
	{
		makeref(chref, Characters[i]);
		if (CheckAttribute(chref, "OfficerInStockMan"))
		{
			string skilltype = skillz[rand(13)];
			log_testinfo(chref.name+" "+skilltype);
			AddCharacterExpToSkill(chref, skilltype, sti(chref.skill.(skilltype))+sti(chref.rank)+15+(GetCharacterSPECIALSimple(chref,SPECIAL_I)*2));
		}
	}
}

// Gregg: Планировщик - записи-будильники из интерфейса CreateNotice (pchar.Scheduler.iN)
// Текущая дата >= целевой?  Сравнение по году -> месяцу -> дню.
bool Scheduler_DateDue(int d, int m, int y)
{
	int cy = GetDataYear();
	int cm = GetDataMonth();
	int cd = GetDataDay();
	if(cy > y) return true;
	if(cy < y) return false;
	if(cm > m) return true;
	if(cm < m) return false;
	if(cd >= d) return true;
	return false;
}

// Gregg: дата как "ДД.ММ.ГГГГ" с ведущими нулями (для строки-срабатывания)
string Scheduler_DateStr(int d, int m, int y)
{
	string sd = "" + d;
	if(d < 10) sd = "0" + d;
	string sm = "" + m;
	if(m < 10) sm = "0" + m;
	return sd + "." + sm + "." + y;
}

// Ежедневная проверка планировщика. Для каждого активного и наступившего пункта - цветная строка лога
// со вспышкой + звук; запись в журнале подсвечивается. Вызывается раз в игровые сутки (case 0).
void Scheduler_CheckDaily()
{
	if(!CheckAttribute(pchar, "Scheduler")) return;

	string sI;
	int d, m, y, dueCount;
	dueCount = 0;

	for(int i=1; i<=10; i++)
	{
		sI = "i" + i;
		if(!CheckAttribute(pchar, "Scheduler." + sI + ".text")) continue;
		if(sti(pchar.Scheduler.(sI).active) != 1) continue;

		d = sti(pchar.Scheduler.(sI).day);
		m = sti(pchar.Scheduler.(sI).month);
		y = sti(pchar.Scheduler.(sI).year);
		if(!Scheduler_DateDue(d, m, y)) continue;

		// срабатывание: цветная строка со вспышкой (с датой срока)
		Log_InfoColoredAnim("yellow", GetConvertStrWithReplace("Variable_Scheduler_1", "Logs.txt", "#space#", " ") + "[" + Scheduler_DateStr(d, m, y) + "] " + pchar.Scheduler.(sI).text);
		dueCount += 1;
	}

	if(dueCount > 0)
	{
		PlayStereoSound("notebook"); // Gregg: звук напоминания (можно заменить на свой алиас)
		SetQuestHeaderColor("MyScheduler", argb(255,255,160,60)); // подсветка записи в журнале (оранжевый)
	}
}
