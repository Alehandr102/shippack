void PGG_DailyUpdateSeaReload()
{
	PGG_DailyUpdateRange("PGG_DailyUpdateSeaReload", 1, MakeInt(PsHeroQty/3 - 1));
}
void PGG_DailyUpdateWorldMapReload()
{
	PGG_DailyUpdateRange("PGG_DailyUpdateWorldMapReload", MakeInt(PsHeroQty/3), MakeInt(PsHeroQty * 2 / 3 - 1));
}
void PGG_DailyUpdateReload()
{
	PGG_DailyUpdateRange("PGG_DailyUpdateReload", MakeInt(PsHeroQty * 2 / 3), PsHeroQty);
}

// LEO: сколько ПГГ обрабатывать дневным ИИ за ОДИН шаг апдейта. 1 = «по одному за тик» — убирает пачку
//   тяжёлых спавнов свиты/рефитов за кадр (главный источник фриза глобалки). Тунабл: если ИИ ПГГ кажется
//   вялым (медленно берут новые задания) — поднять до 2-3; если ещё хитчит — оставить 1.
#define PGG_UPDATES_PER_STEP 2

void PGG_Updates(int i)
{
	// LEO: РАНЬШЕ резало PsHeroQty на 10 частей -> ~6-7 ПГГ ЗА КАДР -> пачка NPC_GenerateCharacter + рефитов
	//   (PGG_SelectTask->SetUpForTask/UpdateStats) синхронно за кадр = ФРИЗ. ТЕПЕРЬ роллинг-курсор: по
	//   PGG_UPDATES_PER_STEP ПГГ за вызов, персистентный курсор в pchar (переживает смену суток), равномерная
	//   ротация всех ПГГ. Дневная СТАТ-прокачка (PGG_UpdateStatsDaily) идёт отдельно на релоадах — НЕ задета,
	//   поэтому прокачка остаётся равномерной по времени. i (номер шага) больше не нужен — курсор сам ведёт обход.
	int cur = 0;
	if (CheckAttribute(pchar, "PGG_UpdateCursor")) cur = sti(pchar.PGG_UpdateCursor);
	int n;
	for (n = 0; n < PGG_UPDATES_PER_STEP; n++)
	{
		cur++;
		if (cur > PsHeroQty) cur = 1;
		PGG_DailyUpdateEx(cur);
	}
	pchar.PGG_UpdateCursor = cur;
}

void PGG_PostWorldMapDailyUpdate()
{
	DelEventHandler("PGG_WorldMapDailyUpdate", "PGG_WorldMapDailyUpdateEvent");
	SetEventHandler("PGG_WorldMapDailyUpdate", "PGG_WorldMapDailyUpdateEvent", 0);
	PostEvent("PGG_WorldMapDailyUpdate", 100);
}

void PGG_WorldMapDailyUpdateEvent()
{
	DelEventHandler("PGG_WorldMapDailyUpdate", "PGG_WorldMapDailyUpdateEvent");
	PGG_DailyUpdateWorldMapReload();
}

void PGG_DailyUpdateRange(string sQuestDate, int iStart, int iEnd)
{
	int j, iDays;

	if(actLoadFlag) return;

	iDays = GetQuestPastDayParam(sQuestDate);
	if (iDays < 1) return;

	if (iStart < 1) iStart = 1;
	if (iEnd > PsHeroQty) iEnd = PsHeroQty;

	for (j = iStart; j <= iEnd; j++)
	{
		PGG_UpdateStatsDaily(CharacterFromID("PsHero_" + j), iDays);
	}
	SaveCurrentQuestDateParam(sQuestDate);
}


void PGG_DailyUpdateEx(int i)
{
	string sTask;
	ref chr;
	int iDays;


	chr = CharacterFromID("PsHero_" + i);

	// LEO: ПГГ гонит пирата ИЛИ уже в стычке (PGG_ChasingPirate) -> его энкаунтер на глобалке ЖИВОЙ (или
	//   заменён боем). НЕ трогаем его в часовом апдейте: перетаск/пересоздание (PGG_FindTargetTown) удалит
	//   живой энкаунтер «не по правилам» -> висячий saveAttribute -> краш; а во время стычки не даём задвоить
	//   ПГГ (он и в бою, и «плывёт» в другой город). Флаг снимет диверт (погоню бросил/стычка кончилась).
	if (CheckAttribute(chr, "PGG_ChasingPirate")) return;

	if (CheckAttribute(chr, "willDie")) return;

	// LEO: подчистка зависшего PGG_warrior для старых сейвов - раньше teardown охоты его не снимал,
	//   и ПГГ навсегда оставался пиратом-догоняющим. Активную охоту не трогаем: у неё есть PGG_Hunter,
	//   а warrior без него и есть зависший хвост.
	if (CheckAttribute(chr, "PGG_warrior") && !CheckAttribute(chr, "PGG_Hunter")) DeleteAttribute(chr, "PGG_warrior");

	if (CheckAttribute(chr,"PGGAi.DontUpdate"))
	{
		// LEO: квест забрал ПГГ себе, но он ещё числится на турнире арены - там он неприкасаем, и в
		//   сухопутной сцене его нельзя убить. Новые партии закрывает гейт в SelectRandomPGG, а эта
		//   ветка лечит уже начатые: на турнире ему всё равно делать нечего, он занят квестом.
		if (CheckAttribute(chr, "PGG_Tournament")) PGG_ReleaseFromTournamentForQuest(chr);
		Log_TestInfo(GetFullName(chr)+ GetConvertStrWithReplace("Variable_PsHero_1", "Logs.txt", "#space#", " "));
		if(!CheckAttribute(chr,"PGGAi.Task.SetSail")) chr.PGGAi.Task.SetSail = true;
		if(!CheckAttribute(chr,"LambiniAsoleda") && GetQuestPastDayParam("PGGAlmostDied" + i) >= PGG_REBIRTH_WAIT_DAYS)
		{
			DeleteAttribute(chr,"PGGAi.DontUpdate");
			DeleteAttribute(chr,"PGGAi.Task");
		}
		return;
	}

	if (CheckAttribute(chr, "PGGAi.SeenToday"))
	{
		DeleteAttribute(chr, "PGGAi.SeenToday")
	}

	//лояльность компаньона
	if (IsCompanion(chr))
	{
		//раз в месяц проверка.
		if (GetNpcQuestPastDayParam(chr, "Companion.CheckRelation") > PGG_COMPANION_RELCHECK_DAYS)
		{
			//тут вожможно стоит дописать ещё и общую лояльность и мораль матросов.
			if (PGG_ChangeRelation2MainCharacter(chr, 0) < PGG_REL_COMPANION_LEAVE && sti(chr.Ship.Type) != SHIP_NOTUSED)
			{
				pchar.Quest.PGG_Companion_Leave.win_condition.l1 = "Location_Type";
				pchar.Quest.PGG_Companion_Leave.win_condition.l1.location_type = "tavern";
				pchar.Quest.PGG_Companion_Leave.function = "PGG_CompanionLeave";
				pchar.questTemp.PGG_Companion_Leave.index = chr.index;
			}
			SaveCurrentNpcQuestDateParam(chr, "Companion.CheckRelation");
		}
	}

	//помер, нефиг мертвых качать.
	//временно либо офицер, либо компаньон... не работаем с ним.
	if (LAi_IsDead(chr) || !sti(chr.PGGAi.IsPGG))
	{
		if (LAi_IsDead(chr))
		{
			if (CheckAttribute(chr, "PGGAi.Rebirth") && sti(chr.PGGAi.Rebirth))
			{
				Train_PPG(chr, true, true);
				LAi_SetCurHPMax(chr);
				DeleteAttribute(chr, "PGGAi.Rebirth");
				DeleteAttribute(chr, "Ship");
				PGG_ClearCompanions(chr); // LEO: возрождение сбрасывает Ship, но компаньоны тащились -> чистим, чтобы протухший торговец-эскорт не всплыл в эскадре после «почти-смерти»
				chr.Ship.Type = SHIP_NOTUSED;
				chr.Dialog.FileName = "PGG_dialog.c";
				chr.Dialog.CurrentNode = "Second Time";

				chr.PGGAi.DontUpdate = true;
				chr.PGGAi.Task.SetSail = true;
				SaveCurrentQuestDateParam("PGGAlmostDied"+i);
			}
			else
			{
				PGG_CheckDead(chr);
			}
		}
		return;
	}

	//у нас есть задание
	if (CheckAttribute (chr, "PGGAi.Task") && chr.PGGAi.Task != PGG_TASK_NOTASK)
	{
		sTask = chr.PGGAi.Task;

		//лок, работу у мэра снимаем через сутки.
		if (sTask == PGG_TASK_LOCKSERVICE || sTask == PGG_TASK_WORKONMAYOR)
		{
			DeleteAttribute(chr, "PGGAi.LockService");
			PGG_SelectTask(chr);
		}
		//контру снимаем через 3 суток.
		if (sTask == PGG_TASK_WORKWITHCONTRA)
		{
			chr.PGGAi.Task.DaysCounter = sti(chr.PGGAi.Task.DaysCounter) + 1;
			if (sti(chr.PGGAi.Task.DaysCounter) > PGG_CONTRA_DAYS)
			{
				if (CheckAttribute(pchar, "questTemp.PGGContra.Know"))
				{
					RemoveSmugglersFromShore();
					pchar.Quest.Rand_Smuggling.Over = "yes";
				}
				DeleteAttribute(chr, "PGGAi.LockService");
				DeleteAttribute(chr, "PGGAi.Task.DaysCounter");
				PGG_SelectTask(chr);
			}
			else PGG_DebugLog(chr.id + " " + chr.PGGAi.Task + " " + chr.PGGAi.Task.DaysCounter);
		}
		//без денег и корабля...
		if (sTask == PGG_TASK_WAITINTAVERN)
		{
			//денег на кораблик нет.
			if (sti(chr.Money) < PGG_MONEY_FOR_SHIP)
			{
				//можем взять истребление банд, чтобы просто не бездельничать
				if (rand(100) < 60)
				{
					chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
					PGG_UpdateStats(chr, "Task");
				}
				else
				{
					PGG_DebugLog(chr.id + " stayed in tavern.");
				}
			}
			//есть деньги на корбаль... валим, кто не успел, тот опоздал
			else
			{
				PGG_UpdateStats(chr, "");
				PGG_SelectTask(chr);
			}
		}
		// LEO: РЕЙС НА ТУРНИР. Прибытие как у обычных морских задач (дни вышли И рейсового энкаунтера уже
		//   нет), но город НЕ переписываем: Доминика не колония, а PGGAi.location.town читают много где.
		//   Товар в магазин тоже не сваливаем - магазина там нет. В конце предохранитель от зависания.
		if (sTask == PGG_TASK_ARENA)
		{
			int iPast = GetNpcQuestPastDayParam(chr, "PGGAi.Task.days");
			int iWay  = 0;
			if (CheckAttribute(chr, "PGGAi.Task.Target.days")) iWay = sti(chr.PGGAi.Task.Target.days);
			// LEO: прибытие. Обычно его проводит приход корабля в точку (PGG_CompleteWorldMapSail), и пока
			//   кораблик жив, мы его НЕ трогаем - пусть дойдёт своим ходом, это и есть «видно, как приплыл».
			//   Но энкаунтер двигается ТОЛЬКО пока игрок на глобалке: если тот сидит на суше, рейс заморожен
			//   и сам не дойдёт никогда. Поэтому по хорошему запасу времени проводим прибытие абстрактно -
			//   строго когда игрока на карте нет, чтобы не гасить кораблик у него на глазах.
			bool bForceArrive = false;
			if (iPast > iWay + 5 && !IsEntity(worldMap)) bForceArrive = true;
			if (bForceArrive && CheckExistQuestEncounter(chr.id))
			{
				Map_ReleaseQuestEncounter(chr.id);
				PGG_DebugLog(chr.id + " прибытие на турнир проведено абстрактно (игрок не на глобалке)");
			}
			if (iWay <= iPast && !CheckExistQuestEncounter(chr.id))
			{
				if (!CheckAttribute(chr, "PGG_Tournament.Arrived"))
				{
					// LEO: свиту распускаем и рейсовый энкаунтер снимаем, а вот группу корабля НЕ трогаем -
					//   она нужна живой: на неё вешается стоянка в бухте (PGG_ParkAtTournament).
					PGG_Disband_Fleet(chr);
					Map_ReleaseQuestEncounter(chr.id);
					DeleteAttribute(chr, "PGGAi.ActiveQuest");
					chr.PGGAi.location = "land";
					chr.PGG_Tournament.Arrived = true;
					PGG_ParkAtTournament(chr);
					PGG_DebugLog(chr.id + " прибыл на турнир, дней в пути " + iPast);
				}
			}
			if (iPast > iWay + 30)   // турнир идёт максимум пару десятков дней - значит завис
			{
				PGG_ClearTournament(chr);
				PGG_SelectTask(chr);
			}
		}
		// LEO: СТРАХОВКА от бессмертного корабля в бухте. Стоянку снимает PGG_ClearTournament, но если
		//   признак участия пропал мимо него (гибель, чужая правка, кривой сейв), неуязвимость осталась бы
		//   навсегда. ⚠️ Гейт именно по ВНЕШНЕМУ PGG_ParkedAtArena: внутренний PGG_Tournament.Parked лежит
		//   в самой ветке турнира и исчезает вместе с ней, поэтому проверять его тут бессмысленно.
		if (CheckAttribute(chr, "PGG_ParkedAtArena"))
		{
			if (!CheckAttribute(chr, "PGG_Tournament"))
			{
				PGG_UnparkFromTournament(chr);
				PGG_DebugLog(chr.id + " снят со стоянки страховкой (потерял признак турнира)");
			}
		}
		//морские задания...
		if (sTask == PGG_TASK_SAILTOISLAND || sTask == PGG_TASK_WORKCONVOY || sTask == PGG_TASK_WORKONSTORE)
		{
			if (CheckAttribute(chr, "PGGAi.Task.Target.days") && sti(chr.PGGAi.Task.Target.days) <= GetNpcQuestPastDayParam(chr, "PGGAi.Task.days") && !CheckExistQuestEncounter(chr.id))
			{
				// LEO: финализируем прибытие по дням ТОЛЬКО если рейсового энкаунтера на карте УЖЕ НЕТ (доплыл ->
				//   позиционное Map_TraderSucces/PGG_CompleteWorldMapSail, либо off-map livetime снёс). Если кораблик
				//   ЖИВ и ещё плывёт — НЕ сносим его по дням (иначе пропадал посреди моря + телепорт в порт), ждём
				//   доплытия до пункта. Запас livetime рейсу увеличен (PGG_tasks.c) -> позиция выигрывает гонку.
				PGG_Disband_Fleet(chr);
				Map_ReleaseQuestEncounter(chr.id);
				group_DeleteGroup(chr.id+"_Group");
				DeleteAttribute(chr, "PGGAi.ActiveQuest");
				chr.PGGAi.location = "land";
				chr.PGGAi.location.town = chr.PGGAi.Task.Target;
				//а это заполнение магазина товаром, привезенным ПГГ.
				AddRemoveGoodsInStore(chr.PGGAi.location.town, sti(chr.PGGAi.Task.Target.Goods), sti(chr.PGGAi.Task.Target.Goods.Qty), true);
				DeleteAttribute(chr, "PGGAi.Task");
				PGG_SelectTask(chr);
			}
			else
			{
				if (CheckAttribute(chr, "PGGAi.Task.Target.days"))
				{
					iDays = sti(chr.PGGAi.Task.Target.days);
					PGG_DebugLog(chr.id + " " + chr.PGGAi.Task + " Target: " + chr.PGGAi.Task.Target + " days: " + iDays);
				}
			}
		}
	}
	//задания нет
	else
	{
		PGG_SelectTask(chr);
	}
	if (chr.PGGAi.location == "land" && sti(chr.Ship.Type) != SHIP_NOTUSED)
	{
		chr.location.from_sea = chr.PGGAi.location.town + "_town";
	}
	else
	{
		chr.location.from_sea = "";
	}

	if (CheckAttribute(chr, "nation_backup"))
	{
		if (!CheckAttribute(chr, "PGG_warrior"))
		{
			if (!CheckAttribute(chr, "PGG_Hunter")) // LEO: активный охотник держит нацию PIRATE до конца охоты (флаг глобалки + форт бьёт как пирата); teardown удаляет PGG_Hunter -> нация возвращается
			{
				chr.nation = chr.nation_backup;
				DeleteAttribute(chr, "nation_backup")
			}
		}
	}
}

//проверка на смерть ПГГ
void PGG_CheckDead(ref chr)
{
    //#20181013-02
    if(!CheckAttribute(chr, "PGGAi.location")) {
        if(bSeaActive)
            chr.PGGAi.location = "sea";
        else
            chr.PGGAi.location = "land";
    }
	if (chr.PGGAi.location == "Dead") return;
	if (IsCompanion(chr) && GetRemovable(chr) && CheckOfficersPerk(chr, "ShipEscape")) return;

	//убираем из локаций на выходе.
	PChar.questTemp.Chr2Remove = chr.id;
	if (!bSeaActive)
	{
		PChar.Quest.RemovePGG.win_condition.l1 = "ExitFromLocation";
		PChar.Quest.RemovePGG.win_condition.l1.Location = PChar.location;
	}
	else
	{
		PChar.Quest.RemovePGG.win_condition.l1 = "ExitFromSea";
	}
	PChar.Quest.RemovePGG.win_condition = "RemoveCharacterFromLocation";

	int pggStage = 0;

	if(!CheckAttribute(Pchar,"GenQuest.PGG_Quest.Stage"))
	{
		pggStage = -10;
	}
	else
	{
		pggStage = sti(Pchar.GenQuest.PGG_Quest.Stage);
	}

	if(pggStage > 3 || CheckAttribute(PChar,"PGG_Enemy") || CheckAttribute(PChar,"PGG_EnemyPP"))
	{
		if (!CheckAttribute(chr, "PGG_Hunter") && !CheckAttribute(chr, "AlwaysEnemy"))
		{
			if(MakeInt(chr.reputation) > PGG_REP_HIGH)
			{
				ChangeCharacterNationReputation(PChar, sti(chr.Nation), -15);
			}
			if(MakeInt(chr.reputation) < PGG_REP_MID)
			{
				ChangeCharacterNationReputation(PChar, sti(chr.Nation), -5);
			}
			if (MakeInt(chr.reputation) >= PGG_REP_MID && MakeInt(chr.reputation) <= PGG_REP_HIGH)
			{
				ChangeCharacterNationReputation(PChar, sti(chr.Nation), -10);
			}
		}
		DeleteAttribute(PChar,"PGG_Enemy");
	}

	PGG_Disband_Fleet(chr);

	if (!CheckAttribute(chr, "KilledTimes"))	chr.KilledTimes = 1;
	else	chr.KilledTimes = sti(chr.KilledTimes)+1;
	int respawnChance = 25 + sti(chr.KilledTimes) * MOD_SKILL_ENEMY_RATE*3;

	//выжил или нет
			if (sti(chr.PGGAi.IsPGG) && rand(100) > respawnChance && chr.id != "PsHero_2")
			{
				Log_TestInfo(chr.id + GetConvertStrWithReplace("Variable_PsHero_3", "Logs.txt", "#space#", " "));
			}
			else
			{
				if (IsOfficer(chr) || IsCompanion(chr))
				{
					Log_TestInfo(chr.id + GetConvertStrWithReplace("Variable_PsHero_4", "Logs.txt", "#space#", " "));
				}
				else
				{
					if (CheckAttribute(chr, "willDie"))
					{
						Log_TestInfo(chr.id + GetConvertStrWithReplace("Variable_PsHero_5", "Logs.txt", "#space#", " "));
					}
					else
					{
						chr.PGGAi.Rebirth = true;
						Log_TestInfo(chr.id + GetConvertStrWithReplace("Variable_PsHero_6", "Logs.txt", "#space#", " "));
						if (CheckAttribute(chr, "PGGAi.location.town.back"))
						{
							chr.PGGAi.location.town = chr.PGGAi.location.town.back;
						}
						if (CheckAttribute(chr, "nation_backup"))
						{
							chr.PGGAi.location.town = PGG_FindRandomTownByNation(sti(chr.nation_backup));
						}
						else
						{
							chr.PGGAi.location.town = PGG_FindRandomTown(chr);
						}
						PGG_ChangeRelation2MainCharacter(chr, -30);
						DeleteAttribute(chr, "PGGAi.Task");
						DeleteAttribute(chr, "PGGAi.LockService");
						DeleteAttribute(chr, "AlwaysFriend");
						// LEO: сворачиваем охоту ПОЛНОСТЬЮ - обратной операцией к PGG_MakeHunter. Раньше снимался
						//   только PGG_Hunter, а AlwaysEnemy, hunter, DontRansackCaptain, mapEnc.type=war и нация
						//   PIRATE висели дальше: ПГГ бил игрока вечно, и помириться было нечем.
						PGG_MakePeace(chr);
						chr.PGGAi.RemoveEncounter = true;
						wdmEmptyAllDeadQuestEncounter();
						DeleteAttribute(chr, "PGGAi.RemoveEncounter");
						DeleteAttribute(PChar, "GenQuest.PGG_Quest.PGG_Enemy");
						return;
					}
				}
			}

	if (!CheckAttribute(chr, "DontCountDeath"))
	{
		if (!CheckAttribute(pchar, "PGG_killed"))
		{
			pchar.PGG_killed = 1;
		}
		else
		{
			pchar.PGG_killed = sti(sti(pchar.PGG_killed)+1);
		}
		Log_TestInfo(GetConvertStrWithReplace("Variable_PsHero_7", "Logs.txt", "#space#", " ")+ pchar.PGG_killed);
	}
	else
	{
		if (!CheckAttribute(pchar, "PGG_NotKilled"))
		{
			pchar.PGG_NotKilled = 1;
		}
		else
		{
			pchar.PGG_NotKilled = sti(sti(pchar.PGG_NotKilled)+1);
		}
	}
	chr.chr_ai.hp = 0.0;

	// LEO: погиб по дороге на турнир или во время него - снимаем участие СРАЗУ. Иначе признак висел бы до
	//   предохранителя (месяц): в таблице «участвует в турнире» у покойника, место в сетке занято мертвецом,
	//   а корабль остался бы стоять в бухте бессмертным.
	if (CheckAttribute(chr, "PGG_Tournament"))
	{
		PGG_FreeTournamentSeat(chr);
		PGG_ClearTournament(chr);
	}

	DeleteAttribute(chr, "PGGAi.Task");
	DeleteAttribute(chr, "PGGAi.LockService");

	chr.PGGAi.location = "Dead";
	chr.PGGAi.location.town = "None";
}
