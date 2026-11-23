// LEO: В сейве хранятся роли, фаза, траектория и состояние бросков на разрубание.
#define BOARDING_PULL_MIN 5.0
#define BOARDING_PULL_MAX 15.0
#define BOARDING_HOOK_INTERVAL 0.32
#define BOARDING_HOOK_FLIGHT 0.90
#define BOARDING_PULL_INTERVAL 2.0
#define BOARDING_PULL_STROKE 1.75
#define BOARDING_METERS_PER_TUG_MIN 6.0
#define BOARDING_METERS_PER_TUG_MAX 12.0
#define BOARDING_PULL_MOMENTUM_TIME 2.5
#define BOARDING_PULL_GLIDE_SPEED_LIMIT 12.0
#define BOARDING_HULL_GAP 0.5
#define BOARDING_RETRY_SECONDS 30
#define BOARDING_FIGHT_MIN 12.0
#define BOARDING_FIGHT_MAX 30.0
#define BOARDING_CUT_MIN_CREW 8
#define BOARDING_CUT_CREW_FACTOR 3
#define BOARDING_CUT_RANGE 30.0
#define BOARDING_CUT_NEAR_SCALE 0.65
#define BOARDING_CUT_FAR_SCALE 1.35
#define BOARDING_CUT_BASE_ATTEMPTS 10
#define BOARDING_CUT_DEFENCE_EXP 250.0
#define BOARDING_CUT_AI_INTERVAL 0.5
#define BOARDING_CUT_LUCK_BASE 3
#define BOARDING_CUT_LUCK_MAX 10
#define BOARDING_PULL_LOSS_PER_ROPE 0.14
#define BOARDING_PULLING 2
#define BOARDING_FIGHTING 3

bool Boarding_Busy(ref chr)
{
	return CheckAttribute(chr, "SeaAI.Boarding.Active");
}

int Boarding_CutBudget(ref defender)
{
	int luck = GetCharacterSPECIAL(defender, SPECIAL_L);
	if (luck < BOARDING_CUT_LUCK_BASE) luck = BOARDING_CUT_LUCK_BASE;
	if (luck > BOARDING_CUT_LUCK_MAX) luck = BOARDING_CUT_LUCK_MAX;
	return BOARDING_CUT_BASE_ATTEMPTS + luck - BOARDING_CUT_LUCK_BASE;
}

// LEO: Общий допуск ГГ и ИИ; потери и сдачу проверяем также во время сцепки.
bool Boarding_CanCut(ref attacker, ref defender)
{
	int crewA = GetCrewQuantity(attacker);
	int crewB = GetCrewQuantity(defender);
	return crewB >= BOARDING_CUT_MIN_CREW && crewB * BOARDING_CUT_CREW_FACTOR >= crewA && !CheckAttribute(defender, "surrendered");
}

bool Boarding_CutCrewReady(ref attacker, ref defender)
{
	if (!sti(attacker.SeaAI.Boarding.CanCut)) return false;
	if (!Boarding_CanCut(attacker, defender))
	{
		attacker.SeaAI.Boarding.CanCut = 0;
		return false;
	}
	if (!CheckAttribute(attacker, "SeaAI.Boarding.CutCrewCapacity"))
	{
		// LEO: Старая сцепка получает штатную численность без сброса канатов, попыток и seed.
		// LEO: Ждём обновления движкового процента, чтобы не дать первый бросок по старому полному шансу.
		attacker.SeaAI.Boarding.CutCrewCapacity = GetOptCrewQuantity(defender);
		return false;
	}
	if (!CheckAttribute(attacker, "SeaAI.Boarding.CutCrewCapacityApplied")) return false;
	if (stf(attacker.SeaAI.Boarding.CutCrewCapacityApplied) != stf(attacker.SeaAI.Boarding.CutCrewCapacity)) return false;
	return true;
}

int Boarding_RetryLeft(ref chr)
{
	if (!CheckAttribute(chr, "SeaAI.BoardingRetry")) return 0;
	return makeint(retMax(0, sti(chr.SeaAI.BoardingRetry) - GetSeaTime()));
}

bool Boarding_CanStart(ref attacker, ref defender)
{
	if (!bSeaActive || bSeaReloadStarted || bAbordageStarted) return false;
	if (sti(attacker.index) == sti(defender.index) || LAi_IsDead(attacker) || LAi_IsDead(defender)) return false;
	if (Boarding_Busy(attacker) || Boarding_Busy(defender)) return false;
	if (Boarding_RetryLeft(attacker) > 0) return false;
	if (CheckAttribute(attacker, "SeaAI.BoardingRetry")) DeleteAttribute(attacker, "SeaAI.BoardingRetry");
	if (!Character_IsAbordageEnable(defender)) return false;
	if (CheckAttribute(attacker, "CantBoardEnemies") || CheckAttribute(attacker, "surrendered")) return false;
	if (CheckAttribute(attacker, "Tasks.CanBoarding"))
	{
		if (!sti(attacker.Tasks.CanBoarding)) return false;
	}
	return GetCrewQuantity(attacker) > 0;
}

float Boarding_PullMetersPerTug(ref attacker)
{
	// LEO: Свежий эффективный абордаж: боцман, вещи и штраф корабля, без сохранённых TmpSkill/BakSkill.
	int boarding = GetSummonSkillFromNameSimple(attacker, SKILL_GRAPPLING);
	boarding = ApplayNavyPenaltyToSkill(attacker, SKILL_GRAPPLING, boarding);
	float skill = retMin(1.0, retMax(0.0, MakeFloat(boarding) / SKILL_MAX));
	return BOARDING_METERS_PER_TUG_MIN + (BOARDING_METERS_PER_TUG_MAX - BOARDING_METERS_PER_TUG_MIN) * skill;
}

bool Boarding_Begin(ref attacker, ref defender, bool manualScene, float strengthA, float strengthB)
{
	if (!Boarding_CanStart(attacker, defender)) return false;
	bool playerAttacks = IsMainCharacter(attacker);
	bool playerDefends = IsMainCharacter(defender);
	if (manualScene)
	{
		if (!playerAttacks && !playerDefends) return false;
		if (CheckAttribute(pchar, "abordage_active") || CheckAttribute(pchar, "abordage_active_count")) return false;
	}
	float grappling = retMin(1.0, retMax(0.0, stf(attacker.TmpSkill.Grappling)));
	float defence = retMin(1.0, retMax(0.0, stf(defender.TmpSkill.Defence)));
	float chance = 0.05 + 0.50 * defence / (defence + grappling + 0.04);
	bool canCut = Boarding_CanCut(attacker, defender);
	attacker.SeaAI.Boarding.Active = 1;
	attacker.SeaAI.Boarding.Owner = 1;
	attacker.SeaAI.Boarding.Partner = sti(defender.index);
	attacker.SeaAI.Boarding.Manual = manualScene;
	attacker.SeaAI.Boarding.PullSeconds = BOARDING_PULL_MAX - (BOARDING_PULL_MAX - BOARDING_PULL_MIN) * grappling;
	attacker.SeaAI.Boarding.PullInterval = BOARDING_PULL_INTERVAL;
	attacker.SeaAI.Boarding.PullStroke = BOARDING_PULL_STROKE;
	attacker.SeaAI.Boarding.MetersPerTug = Boarding_PullMetersPerTug(attacker);
	attacker.SeaAI.Boarding.PullMomentumTime = BOARDING_PULL_MOMENTUM_TIME;
	attacker.SeaAI.Boarding.PullGlideSpeedLimit = BOARDING_PULL_GLIDE_SPEED_LIMIT;
	attacker.SeaAI.Boarding.LastPullSound = 0;
	attacker.SeaAI.Boarding.HookInterval = BOARDING_HOOK_INTERVAL;
	attacker.SeaAI.Boarding.HookFlight = BOARDING_HOOK_FLIGHT;
	attacker.SeaAI.Boarding.HullGap = BOARDING_HULL_GAP;
	attacker.SeaAI.Boarding.Seed = rand(32767);
	attacker.SeaAI.Boarding.CanCut = canCut;
	attacker.SeaAI.Boarding.CutChance = chance;
	attacker.SeaAI.Boarding.CutCrewCapacity = GetOptCrewQuantity(defender); // LEO: Движок читает живую команду относительно штатной оптимальной.
	attacker.SeaAI.Boarding.CutRange = BOARDING_CUT_RANGE;
	attacker.SeaAI.Boarding.CutNearScale = BOARDING_CUT_NEAR_SCALE;
	attacker.SeaAI.Boarding.CutFarScale = BOARDING_CUT_FAR_SCALE;
	attacker.SeaAI.Boarding.StrengthA = strengthA;
	attacker.SeaAI.Boarding.StrengthB = strengthB;
	defender.SeaAI.Boarding.Active = 1;
	defender.SeaAI.Boarding.Owner = 0;
	defender.SeaAI.Boarding.Partner = sti(attacker.index);
	if (canCut)
	{
		attacker.SeaAI.Boarding.PullLossPerRope = BOARDING_PULL_LOSS_PER_ROPE;
		defender.SeaAI.Boarding.CutSeed = rand(65535);
		defender.SeaAI.Boarding.CutTapsLeft = Boarding_CutBudget(defender);
		defender.SeaAI.Boarding.CutRopes = 0;
	}

	bool accepted = SendMessage(&AISea, "laa", AI_MESSAGE_SHIP_BOARDING_BEGIN, attacker, defender);
	if (!accepted)
	{
		DeleteAttribute(attacker, "SeaAI.Boarding");
		DeleteAttribute(defender, "SeaAI.Boarding");
		return false;
	}
	Trace("[BOARDING] start " + attacker.index + " -> " + defender.index + " player=" + manualScene);
	if (manualScene) PlayStereoSound("Interface\abordage.wav");
	else Play3DSound("Interface\abordage.wav", stf(attacker.Ship.Pos.x), stf(attacker.Ship.Pos.y), stf(attacker.Ship.Pos.z));
	if (manualScene)
	{
		bAbordageShipCanBe = false;
		bAbordageFortCanBe = false;
		bCanEnterToLand = false;
		bDisableMapEnter = true;
		bDisableSailTo = true;
		RefreshBattleInterface();
	}
	return true;
}

void Boarding_EndByCut(ref attacker, ref defender)
{
	bool cut = SendMessage(&AISea, "la", AI_MESSAGE_SHIP_BOARDING_CUT, defender);
	if (!cut) return;
	attacker.SeaAI.BoardingRetry = GetSeaTime() + BOARDING_RETRY_SECONDS;
	if (IsMainCharacter(attacker))
	{
		Log_SetStringToLog(GetConvertStr("BoardingRetry", "SeaBoarding.txt"));
		Boarding_RefreshCooldown();
	}
	if (IsMainCharacter(defender))
	{
		AddCharacterExpToSkill(defender, SKILL_DEFENCE, BOARDING_CUT_DEFENCE_EXP);
		Log_SetStringToLog(GetConvertStr("BoardingCutSuccess", "SeaBoarding.txt"));
	}
}

void Boarding_TryCut(ref attacker, ref defender)
{
	if (GetDeltaTime() == 0) return;
	if (!Boarding_CutCrewReady(attacker, defender)) return;
	if (!CheckAttribute(attacker, "SeaAI.Boarding.CutChancePercent") ||
		!CheckAttribute(attacker, "SeaAI.Boarding.CutRopesRequired")) return;
	int required = sti(attacker.SeaAI.Boarding.CutRopesRequired);
	if (required <= 0) return;

	// LEO: В старой сцепке сохраняем уже обрубленные канаты, остаток попыток и броски.
	if (!CheckAttribute(defender, "SeaAI.Boarding.CutTapsLeft"))
		defender.SeaAI.Boarding.CutTapsLeft = Boarding_CutBudget(defender);
	if (!CheckAttribute(defender, "SeaAI.Boarding.CutSeed"))
		defender.SeaAI.Boarding.CutSeed = sti(attacker.SeaAI.Boarding.Seed);
	int tapsLeft = sti(defender.SeaAI.Boarding.CutTapsLeft);
	if (tapsLeft <= 0) return;
	tapsLeft--;
	defender.SeaAI.Boarding.CutTapsLeft = tapsLeft;

	int seed = (sti(defender.SeaAI.Boarding.CutSeed) * 25173 + 13849) % 65536;
	defender.SeaAI.Boarding.CutSeed = seed;
	if (seed * 100 < sti(attacker.SeaAI.Boarding.CutChancePercent) * 65536)
	{
		int severed = 1;
		if (CheckAttribute(defender, "SeaAI.Boarding.CutRopes"))
			severed += sti(defender.SeaAI.Boarding.CutRopes);
		defender.SeaAI.Boarding.CutRopes = severed;
		if (severed >= required)
		{
			Boarding_EndByCut(attacker, defender);
			return;
		}
	}
	if (tapsLeft > 0) return;
	attacker.SeaAI.Boarding.CanCut = 0;
	if (IsMainCharacter(defender)) Log_SetStringToLog(GetConvertStr("BoardingCutFailed", "SeaBoarding.txt"));
}

bool Boarding_PlayerCut()
{
	if (!Boarding_Busy(pchar)) return false;
	if (sti(pchar.SeaAI.Boarding.Owner)) return false;
	ref attacker = GetCharacter(sti(pchar.SeaAI.Boarding.Partner));
	if (!Boarding_Busy(attacker)) return false;
	// LEO: Пока ГГ защищается, клавиша не передаёт команду смены заряда эскадре.
	if (sti(attacker.SeaAI.Boarding.Phase) == BOARDING_PULLING) Boarding_TryCut(attacker, pchar);
	return true;
}

void Boarding_RefreshCooldown()
{
	if (!bSeaActive || bAbordageStarted) return;
	if (!IsEntity(&objActivePerkShower)) return;
	int left = Boarding_RetryLeft(pchar);
	if (left <= 0)
	{
		if (CheckAttribute(pchar, "SeaAI.BoardingRetry")) DeleteAttribute(pchar, "SeaAI.BoardingRetry");
		if (isPerkInActiveList("BoardingRetry")) DelPerkFromActiveList("BoardingRetry");
		return;
	}
	if (!isPerkInActiveList("BoardingRetry")) AddPerkToActiveList("BoardingRetry");
	PerkFx_Update("BoardingRetry", makefloat(left) / BOARDING_RETRY_SECONDS, false, false);
	PerkTimers_Refresh();
}

void Boarding_ApplyLoss(ref chr, int initial, int finalCrew, float progress)
{
	int desired = initial - makeint((initial - finalCrew) * progress + 0.5);
	int current = GetCrewQuantity(chr);
	if (current <= desired) return;
	int lost = current - desired;
	Crew_SkimWounded(chr, lost);
	SetCrewQuantity(chr, desired);
	chr.SeaAI.Boarding.AppliedLoss = sti(chr.SeaAI.Boarding.AppliedLoss) + lost;
}

void Boarding_SetFight(ref attacker, ref defender)
{
	float a = retMax(0.1, stf(attacker.SeaAI.Boarding.StrengthA));
	float b = retMax(0.1, stf(attacker.SeaAI.Boarding.StrengthB));
	float weaker = retMin(a, b) / retMax(a, b);
	int crewA = GetCrewQuantity(attacker);
	int crewB = GetCrewQuantity(defender);
	attacker.SeaAI.Boarding.InitialCrew = crewA;
	defender.SeaAI.Boarding.InitialCrew = crewB;
	attacker.SeaAI.Boarding.PlannedFinalCrew = 0;
	defender.SeaAI.Boarding.PlannedFinalCrew = 0;
	attacker.SeaAI.Boarding.AppliedLoss = 0;
	defender.SeaAI.Boarding.AppliedLoss = 0;
	if (a >= b)
	{
		attacker.SeaAI.Boarding.PlannedFinalCrew = crewA - makeint(crewA * weaker / 1.8);
	}
	else
	{
		defender.SeaAI.Boarding.PlannedFinalCrew = crewB - makeint(crewB * weaker / 1.8);
	}
	attacker.SeaAI.Boarding.FightDuration = BOARDING_FIGHT_MIN + (BOARDING_FIGHT_MAX - BOARDING_FIGHT_MIN) * weaker;
}

void Boarding_FinalizeAI(ref attacker, ref defender)
{
	int a = GetCrewQuantity(attacker);
	int b = GetCrewQuantity(defender);
	int lostA = sti(attacker.SeaAI.Boarding.AppliedLoss);
	int lostB = sti(defender.SeaAI.Boarding.AppliedLoss);
	SendMessage(&AISea, "la", AI_MESSAGE_SHIP_BOARDING_END, attacker);
	if (a == 0 && b == 0)
	{
		ShipDead(sti(attacker.index), KILL_BY_ABORDAGE, -1);
		ShipDead(sti(defender.index), KILL_BY_ABORDAGE, -1);
		return;
	}
	if (b == 0)
	{
		AutoAbord_ResolveBoarding(attacker, defender, lostA, lostB,
			GetConvertStrWithReplace("Variable_companions_3", "Logs.txt", "#space#", " "),
			GetConvertStrWithReplace("Variable_companions_4", "Logs.txt", "#space#", " "));
	}
	else
	{
		AutoAbord_ResolveBoarding(defender, attacker, lostB, lostA,
			GetConvertStrWithReplace("Variable_companions_8", "Logs.txt", "#space#", " "),
			GetConvertStrWithReplace("Variable_companions_9", "Logs.txt", "#space#", " "));
	}
}

void Ship_BoardingTick()
{
	int index = GetEventData();
	if (!bSeaActive || bSeaReloadStarted || bAbordageStarted) return;
	ref attacker = GetCharacter(index);
	if (!Boarding_Busy(attacker)) return;
	ref defender = GetCharacter(sti(attacker.SeaAI.Boarding.Partner));
	if (!Boarding_Busy(defender)) return;
	int phase = sti(attacker.SeaAI.Boarding.Phase);
	float elapsed = stf(attacker.SeaAI.Boarding.Elapsed);
	if (sti(attacker.SeaAI.Boarding.Manual) && CheckAttribute(attacker, "SeaAI.Boarding.PullStep"))
	{
		int step = sti(attacker.SeaAI.Boarding.PullStep);
		if (step > sti(attacker.SeaAI.Boarding.LastPullSound))
		{
			attacker.SeaAI.Boarding.LastPullSound = step;
			if (step % 2 == 1) PlayVoice("Interface\Russian_GTBoard_7.wav");
			else PlayVoice("Interface\Russian_GTBoard_8.wav");
		}
	}
	if (phase == BOARDING_PULLING)
	{
		if (!Boarding_CutCrewReady(attacker, defender) || IsMainCharacter(defender)) return;
		float nextCut = BOARDING_CUT_AI_INTERVAL;
		if (CheckAttribute(defender, "SeaAI.Boarding.NextCutAt")) nextCut = stf(defender.SeaAI.Boarding.NextCutAt);
		if (elapsed < nextCut) return;
		defender.SeaAI.Boarding.NextCutAt = elapsed + BOARDING_CUT_AI_INTERVAL;
		// LEO: ИИ принимает бой, пока не уступает числом боеспособных матросов.
		if (GetCrewQuantity(defender) >= GetCrewQuantity(attacker)) return;
		Boarding_TryCut(attacker, defender);
		return;
	}
	if (phase != BOARDING_FIGHTING) return;
	if (sti(attacker.SeaAI.Boarding.Manual))
	{
		if (CheckAttribute(pchar, "abordage_active") || CheckAttribute(pchar, "abordage_active_count")) return;
		if (CheckAttribute(attacker, "SeaAI.Boarding.Transition")) return;
		int target = sti(defender.index);
		bool initiator = IsMainCharacter(attacker);
		if (!initiator) target = index;
		attacker.SeaAI.Boarding.Transition = 1;
		if (!Sea_AbordageLoadResolved(SHIP_ABORDAGE, initiator, target))
			SendMessage(&AISea, "la", AI_MESSAGE_SHIP_BOARDING_END, pchar);
		return;
	}
	if (!CheckAttribute(attacker, "SeaAI.Boarding.FightDuration")) Boarding_SetFight(attacker, defender);
	float progress = retMin(1.0, elapsed / stf(attacker.SeaAI.Boarding.FightDuration));
	Boarding_ApplyLoss(attacker, sti(attacker.SeaAI.Boarding.InitialCrew), sti(attacker.SeaAI.Boarding.PlannedFinalCrew), progress);
	Boarding_ApplyLoss(defender, sti(defender.SeaAI.Boarding.InitialCrew), sti(defender.SeaAI.Boarding.PlannedFinalCrew), progress);
	if (progress >= 1.0 || GetCrewQuantity(attacker) <= 0 || GetCrewQuantity(defender) <= 0)
		Boarding_FinalizeAI(attacker, defender);
}

void Boarding_OnSeaLoad()
{
	if (!CheckAttribute(pchar, "boarding_info.staged")) return;
	if (!sti(pchar.boarding_info.staged)) return;
	if (!Boarding_Busy(pchar))
	{
		// LEO: Некому возобновлять переход, если второй корабль отсутствует в сохранении.
		DeleteAttribute(pchar, "abordage_active_count");
		DeleteAttribute(pchar, "boarding_info");
		return;
	}
	ref attacker = pchar;
	if (!sti(pchar.SeaAI.Boarding.Owner)) attacker = GetCharacter(sti(pchar.SeaAI.Boarding.Partner));
	if (!CheckAttribute(attacker, "SeaAI.Boarding.Transition"))
	{
		DeleteAttribute(pchar, "abordage_active_count");
		DeleteAttribute(pchar, "boarding_info");
		return;
	}
	// LEO: Автосохранение абордажа сделано после сцепки; возобновляем переход без повторного автосейва.
	DeleteAttribute(pchar, "abordage_active_count");
	DeleteAttribute(pchar, "abordage_active");
	SetEventHandler("frame", "Continue_Sea_AbordageLoad", 1);
}

bool Boarding_DamageBlocked(ref target, int source)
{
	if (Boarding_Busy(target))
	{
		if (sti(target.SeaAI.Boarding.Partner) != source) return true;
		if (CheckAttribute(target, "SeaAI.Boarding.Phase") && sti(target.SeaAI.Boarding.Phase) == BOARDING_FIGHTING) return true;
	}
	if (source < 0 || source >= TOTAL_CHARACTERS) return false;
	ref attacker = GetCharacter(source);
	if (Boarding_Busy(attacker))
	{
		if (sti(attacker.SeaAI.Boarding.Partner) != sti(target.index)) return true;
		return CheckAttribute(attacker, "SeaAI.Boarding.Phase") && sti(attacker.SeaAI.Boarding.Phase) == BOARDING_FIGHTING;
	}
	return false;
}
