#define BRDLT_SHIP	0
#define BRDLT_FORT	1

#define LOCINITBRD_DEFAULTLOCATION "BOARDING_U_H_CS_T12"
#define LOCINITBRD_DEFAULTLOCATION_FORT "Any_fort_1"

// =====================================================================================
//  LEO: ТЮНИНГ-БЛОК АБОРДАЖА/ШТУРМА ФОРТА — все балансные «ручки» в одном месте.
//  Стадия 1 рефактора: значения 1:1 с прежним кодом (баланс НЕ менялся). Зона: [П]алуба / [Ф]орт / [О]ба.
//  MOD_SKILL_ENEMY_RATE — внешний глобальный множитель сложности, входит во многие формулы (тут не дублируется).
// =====================================================================================

// --- Базовые лимиты сцены ---
#define MAX_GROUP_SIZE          13      // [О] базовый maxcrew на локацию (если у локации нет boarding.locatorNum)
#define MAX_MUSKETEER_SIZE      12      // [О] макс. мушкетёров на сторону
#define PLAYER_BOARDING_COEFF   0.75    // [О] база перевеса/потерь («25% не в бою»)
#define BRD_START_HP_PLAYER     40      // [О] стартовое ХП абордажника ГГ (до бонусов)
#define BRD_START_HP_ENEMY      40      // [О] стартовое ХП абордажника врага
#define BRD_START_ERANK         10      // [О] дефолтный ранг врага
#define BRD_OFFICER_ATT_RANGE   100.0   // [О] радиус автоагра абордажников на палубе

// --- Мгновенная сдача при заходе на абордаж (только корабль) ---
#define BRD_SURRENDER_ENEMY_MORALE       5    // [П] мораль врага после захвата
#define BRD_SURRENDER_PLAYER_MORALE_GAIN 3    // [П] +мораль ГГ за принятую сдачу
#define BRD_CAPTAIN_RESURRECT_CREW_RAND  20   // [П] rand(N): воскрешение части команды при сдаче кэпа (0..N-1)
#define BRD_CAPTAIN_RESURRECT_CREW_MULT  0.01 // [П] множитель доли воскрешения

// --- Опыт солдат за исход (AIShip_isPerksUse: без перка SeaWolf / с перком) ---
#define BRD_SOLDIER_EXP_SURRENDER_OFF   1.0  // [О] сдался сразу
#define BRD_SOLDIER_EXP_SURRENDER_ON    2.0
#define BRD_SOLDIER_EXP_CAPTIVE_OFF     4.0  // [П] капитан в плен
#define BRD_SOLDIER_EXP_CAPTIVE_ON      8.0
#define BRD_SOLDIER_EXP_SHIP_OFF        5.0  // [П] корабль захвачен
#define BRD_SOLDIER_EXP_SHIP_ON         10.0
#define BRD_SOLDIER_EXP_FORT_OFF        7.0  // [Ф] форт захвачен
#define BRD_SOLDIER_EXP_FORT_ON         14.0

// --- Мушкетный залп ПЕРЕД боем (только корабль, перк MusketsShoot) ---
#define BRD_MUSKET_VOLLEY_FRACTION  0.25 // [П] базовый урон залпа = доля команды
#define BRD_MUSKET_SURGEON_OFF      1.0  // [П] множитель урона по цели без её Экстренного хирурга
#define BRD_MUSKET_SURGEON_ON       0.75 // [П] ...с Экстренным хирургом у цели
#define BRD_MUSKET_DEFENCE_BASE     2.1  // [П] модификатор от Защиты цели: (BASE - def/SKILL_MAX) / DIV
#define BRD_MUSKET_DEFENCE_DIV      2.0
#define BRD_MUSKET_FALCONETS_MULT   1.2  // [П] усиленные фальконеты (SUP_FALCONETS)
#define BRD_MUSKET_BIGSIDES_MULT    0.6  // [П] укреплённый фальшборт цели (SUP_BIGSIDES)
#define BRD_MUSKET_ARMOR_DIV        3    // [П] бронепалуба цели (S_armor)
#define BRD_MUSKET_SOUND_MAX        7    // [П] rand(N) вариантов звука выстрела

// --- Кэпы ХП/ранг-бонусов и автолевел (зависят от MOD_SKILL_ENEMY_RATE) ---
#define BRD_PLAYER_HP_CAP_BASE       600 // [О] кэп ХП игрока = BASE / MOD
#define BRD_ENEMY_HP_CAP_BASE        250 // [О] кэп ХП врага  = BASE * MOD
#define BRD_PLAYER_RANK_CAP_BASE     60  // [О] кэп m_rank_bonus = BASE / MOD
#define BRD_ENEMY_RANK_CAP_BASE      15  // [О] кэп e_rank_bonus = BASE * MOD
#define BRD_RANK_AUTOLEVEL_DIV       3   // [О] автолевел: (разница рангов)/DIV
#define BRD_RANK_AUTOLEVEL_THRESHOLD 15  // [О] автолевел врага начинается с ранга ГГ > THRESHOLD
#define BRD_OFFICER_BONUS_K          2   // [О] бонус врагу от офицеров ГГ: avg*off/(K*MAX_NUM_FIGHTERS*(MODBASE-MOD))
#define BRD_OFFICER_BONUS_MODBASE    5

// --- Потери и медицина (финал боя) ---
#define BRD_CASUALTY_DEFENCE_BASE   0.7   // [О] делитель потерь: BASE + def/SKILL_MAX (0.77..1.7)
#define BRD_CASUALTY_SURGEON_OFF    0.0   // [О] Экстренный хирург: без перка
#define BRD_CASUALTY_SURGEON_ON     0.325 // [О] ...с перком (бонус выживанию)
#define BRD_MEDICINE_PER_WOUNDED    3     // [О] единиц лекарства на раненого
#define BRD_MEDICINE_LOW_WARN_MULT  2     // [О] порог предупреждения о нехватке лекарств
#define BRD_CASUALTY_SKILL_EXP_DIV  3     // [О] опыт Защиты/Абордажа = потери/DIV
// Мораль считается от доли УБИТЫХ, а не от всех задетых. База стала втрое меньше прежней (раненые
// исключены), поэтому все три числа отмасштабированы на те же ~3: типовой абордаж даёт прежнюю
// просадку, но двигают её только смерти - Защита и перки врача теперь реально её гасят.
#define BRD_MORALE_LOSS_THRESHOLD   0.05  // [О] «незаметные потери» (капитан выжил)
#define BRD_MORALE_LEADERSHIP_DIV   150.0 // [О] Лидерство снижает падение морали (0.7..6.7% убитых прощается)
#define BRD_MORALE_LOSS_SCALE       180   // [О] масштаб падения морали (типовой размен ~-15, мясорубка ~-45)

// --- Зелья капитана (по сложности MOD_SKILL_ENEMY_RATE) ---
#define BRD_CAPTAIN_POTION1_BASE    0.6   // [О] potion1: (BASE + RAND*rnd) * hp_пул / DIV, кэп CAP*MOD
#define BRD_CAPTAIN_POTION1_RAND    0.3
#define BRD_CAPTAIN_POTION1_DIV     40
#define BRD_CAPTAIN_POTION1_CAP     4
#define BRD_CAPTAIN_POTION2_BASE    0.65  // [О] potion2: ... кэп CAP*(MOD-1)
#define BRD_CAPTAIN_POTION2_RAND    0.35
#define BRD_CAPTAIN_POTION2_DIV     150
#define BRD_CAPTAIN_POTION2_CAP     3

// --- Кираса капитана / мушкетёры ---
#define BRD_CUIRASS_DIV             10  // [О] уровень кирасы = (MOD*3*2 + ранг)/DIV
#define BRD_CUIRASS_CAP             5   // [О] макс. уровень кирасы
#define BRD_MUSKETEER_RANK_MOD_MULT 3   // [О] ранг мушкетёра = ранг ГГ + MOD*MULT

// --- Форт ---
#define BRD_FORT_SLAVES_BASE    10   // [Ф] рабы (остатки выживших) = BASE + rand(RAND)
#define BRD_FORT_SLAVES_RAND    350

// --- Сдача в бою CheckForSurrender (только корабль) ---
#define BRD_SURR_RATE_NORMAL     0.3  // [П] базовая склонность к сдаче (обычный) — LEO: снижено 0.5->0.3 (реже сдаются)
#define BRD_SURR_RATE_PIRATE     0.12 // [П] ...пират — LEO: снижено 0.2->0.12
#define BRD_SURR_RATE_TRADE      0.7  // [П] ...торговец — LEO: снижено 0.9->0.7
#define BRD_SURR_RATE_MOD_NORMAL 0.02 // [П] вычет от сложности: RATE - COEF*MOD*MULT
#define BRD_SURR_RATE_MOD_PIRATE 0.01
#define BRD_SURR_RATE_MOD_TRADE  0.03
#define BRD_SURR_RATE_MOD_MULT   3
#define BRD_SURR_SKILL_MULT      5.0  // [П] вес навыка/опыта/морали в силе сдачи
#define BRD_SURR_DECK_MULT       0.25 // [П] вес номера палубы (1 + MULT*deck) — LEO: снижено 0.5->0.25 (меньше сдач ДО боя на deck1)
#define BRD_SURR_SEADOG_MULT     2.0  // [П] перк SeaDogProfessional ×сила сдачи
#define BRD_SURR_SHIP_BONUS_DIV  10.0 // [П] бонус за лишний корабль эскадры (+1/DIV каждый)
#define BRD_SURR_ROLL_MAX        1000 // [П] знаменатель ролла rand(MAX)

bool   IsFort = false;
int    boarding_location = -1;
int    boarding_location_type = -1;
object boarding_fader;


int   boarding_player_crew = 0;
int   boarding_officers    = 0;
float boarding_player_base_crew    = 0.0; //boal
float boarding_player_crew_per_chr = 1.0;
int   boarding_player_crew_start   = 0; //sd
float boarding_player_dmg_crew     = 0.0; // LEO: накопленный НАНЕСЁННЫЙ урон по матросам-абордажникам ГГ, переведённый в единицы команды (потери). Хилки зельями не занижают: урон учитывается в момент нанесения

int   boarding_enemy_crew = 0;
float boarding_enemy_base_crew    = 0.0;
float boarding_enemy_crew_per_chr = 1.0;
int   boarding_enemy_crew_start   = 0; //sd
int   boarding_escadra_crew = 0;   // LEO: бойцов-десанта эскадры (отдельная модель) осталось заспавнить на штурме форта
int   boarding_escadra_nation = 0; // LEO: нация десанта эскадры (для модели sold_<nat>_N)
int   boarding_escadra_raw = 0;    // LEO: сырая уцелевшая команда эскадры (учтена в силе штурмующих mcrew на форте)
int   boarding_escadra_flag = 0;   // LEO: текущий спавнящийся боец — десант эскадры (для пометки chr.BoardEscadra)
int   boarding_quest_crew = 0;

ref    boarding_enemy;
object boarding_adr[10];
float  boarding_player_hp = BRD_START_HP_PLAYER;
float  boarding_enemy_hp = BRD_START_HP_ENEMY;
int    boarding_echr_index = -1;
int    boarding_rank_diff = 0;         // LEO: ранг ГГ минус ранг вражьего капитана — для пересчёта бонусов на каждой локации
int    boarding_offic_rank_bonus = 0;  // LEO: надбавка врагу за офицеров ГГ, считается один раз на входе
int    boarding_erank = BRD_START_ERANK;
bool   LAi_boarding_process = false;
bool   Surrendered = false; // сдача в плен

//Процес абордажа
bool LAi_IsBoardingProcess()
{
	return LAi_boarding_process;
}

//Получить картинку для перехода в абордаж
string LAi_GetBoardingImage(ref echr, bool isMCAttack)
{
	ref mchr = GetMainCharacter();
	string deckID = "";
	isMCAttack   = true;// boal 110804 fix всегда герой
	if(isMCAttack)
	{
		deckID = GetShipLocationID(echr);
	}else{
		deckID = GetShipLocationID(mchr);
	}
	int locID = -1;
	if(deckID != "")
	{
		locID = FindLocation(deckID);
		if(locID < 0)
		{
			Trace("Boarding: ship location not found <" + deckID + ">, set default");
			deckID = "";
		}
	}
	if(locID >= 0)
	{
		if(CheckAttribute(&Locations[locID], "image"))
		{
			if(Locations[locID].image != "")
			{
				return Locations[locID].image;
			}
		}
	}
	return "loading\jonny_load\abordage\abord_"+rand(2)+".dds";
}

// =====================================================================================
//  LEO: Диспетчеры разделения ФОРТ / ПАЛУБА (Стадия 2 рефактора).
//  Форт-специфичная логика вынесена сюда из функций-монстров; каркас абордажа общий.
//  Поведение 1:1 с прежним инлайн-кодом. Читают глобал IsFort (приватен этому файлу).
// =====================================================================================

// Дефолтная стартовая локация: форт vs корабль.
string Boarding_GetDefaultLocation(int locType)
{
	if(locType == BRDLT_FORT) return LOCINITBRD_DEFAULTLOCATION_FORT;
	return LOCINITBRD_DEFAULTLOCATION;
}

// LEO: бонусы за расклад сил (ХП и уровень матросов). Считалось единожды на входе, из-за чего к
// последним локациям бонус безнадёжно устаревал: пулы сторон приводятся к СУММЕ лимитов всех локаций,
// и соотношение по ходу боя уходит далеко от стартового. Теперь зовётся перед каждой расстановкой.
// Сжатие пулов к maxcrew сюда НЕ входит — оно однократное, иначе пулы поедут.
void Boarding_ApplyStrengthBonuses(int mcrew, int ecrew)
{
	if (mcrew < 1) mcrew = 1;
	if (ecrew < 1) ecrew = 1;
	boarding_player_hp = 0.0;
	boarding_enemy_hp  = 0.0;
	m_rank_bonus = 0;
	e_rank_bonus = 0;
	if (mcrew > ecrew) {
		boarding_player_hp = boarding_player_hp + GetBoarding_player_hp_Bonus(mcrew, ecrew);
		GetBoarding_player_rank_Bonus(mcrew, ecrew);
		if (boarding_player_hp > BRD_PLAYER_HP_CAP_BASE/MOD_SKILL_ENEMY_RATE) boarding_player_hp = BRD_PLAYER_HP_CAP_BASE/MOD_SKILL_ENEMY_RATE;
	}
	else {
		boarding_enemy_hp = boarding_enemy_hp + GetBoarding_enemy_hp_Bonus(mcrew, ecrew);
		GetBoarding_enemy_rank_Bonus(mcrew, ecrew);
		if (boarding_enemy_hp > BRD_ENEMY_HP_CAP_BASE*MOD_SKILL_ENEMY_RATE) boarding_enemy_hp = BRD_ENEMY_HP_CAP_BASE*MOD_SKILL_ENEMY_RATE;
	}
	//небольшой автолевел
	e_rank_bonus = e_rank_bonus + MakeInt(boarding_rank_diff / BRD_RANK_AUTOLEVEL_DIV);
	float moraleHP = GetCrewBonusMoraleHP(pchar);
	if (IsFort && CheckAttribute(pchar, "FortLanding.Total") && sti(pchar.FortLanding.Total) > 0)
	{
		aref party;
		makearef(party, pchar.FortLanding);
		m_rank_bonus += makeint(7 * (0.1 + Landing_Average(party, "Soldiers") / GetCrewExpRate() - 0.65));
		moraleHP = (Landing_Average(party, "Morale") - MORALE_NORMAL) / (MORALE_MAX - MORALE_MIN) * 0.4 - 0.1;
		if (moraleHP < -0.15) moraleHP = -0.15;
		if (moraleHP > 0.1) moraleHP = 0.1;
	}
	else m_rank_bonus += MakeInt(GetCrewBonusRank(pchar));
	boarding_player_hp += MakeInt(moraleHP * LAi_GetCharacterMaxHP(pchar));
	if (m_rank_bonus > BRD_PLAYER_RANK_CAP_BASE/MOD_SKILL_ENEMY_RATE) {m_rank_bonus = BRD_PLAYER_RANK_CAP_BASE/MOD_SKILL_ENEMY_RATE;}
	if (e_rank_bonus > BRD_ENEMY_RANK_CAP_BASE*MOD_SKILL_ENEMY_RATE) {e_rank_bonus = BRD_ENEMY_RANK_CAP_BASE*MOD_SKILL_ENEMY_RATE;}
	if (sti(pchar.rank) > BRD_RANK_AUTOLEVEL_THRESHOLD) {
		e_rank_bonus = e_rank_bonus + MakeInt((sti(pchar.rank)-BRD_RANK_AUTOLEVEL_THRESHOLD) / BRD_RANK_AUTOLEVEL_DIV);//автолевел
	}
	if (e_rank_bonus < 0) {
		m_rank_bonus -= e_rank_bonus;
		e_rank_bonus = 0;
	}
	e_rank_bonus += boarding_offic_rank_bonus; // надбавка за офицеров ГГ (считается один раз на входе)
}

// Максимум бойцов на сцене: форт суммирует бастионы, корабль — доп. палубы по классу.
int Boarding_ComputeMaxCrew(int locID, int eclass)
{
	int maxcrew = MAX_GROUP_SIZE;
	int idecknum;
	if(CheckAttribute(&Locations[locID], "boarding.locatorNum")) {
		maxcrew = sti(Locations[locID].boarding.locatorNum);
	}
	//TODO: убрать этот костыль и разобраться как выставлять размер массива
	if (IsFort) {
		idecknum = FindLocation(Locations[locID].boarding.nextdeck);
		if(CheckAttribute(&Locations[idecknum], "boarding.locatorNum")) {
			maxcrew += sti(Locations[idecknum].boarding.locatorNum);
		}
		idecknum = FindLocation("Boarding_bastion2");//fixme: у boarding_bastion1 неправильное число локаторов, boarding_bastion не юзается.
		if(CheckAttribute(&Locations[idecknum], "boarding.locatorNum")) {
			maxcrew += sti(Locations[idecknum].boarding.locatorNum);
		}
	}
	else if (eclass <= 2) {
		idecknum = FindLocation("Boarding_GunDeck");//nextdeck не прописан в LocationInitBoarding
		if(CheckAttribute(&Locations[idecknum], "boarding.locatorNum")) {
			maxcrew += sti(Locations[idecknum].boarding.locatorNum) - 1;//плюсует лишнюю тушку
		}
		idecknum = FindLocation("Boarding_Cargohold");
		if(CheckAttribute(&Locations[idecknum], "boarding.locatorNum")) {
			maxcrew += sti(Locations[idecknum].boarding.locatorNum) - 1;
		}
	}
	else if (eclass <= 4) {
		idecknum = FindLocation("Boarding_Campus");
		if(CheckAttribute(&Locations[idecknum], "boarding.locatorNum")) {
			maxcrew += sti(Locations[idecknum].boarding.locatorNum) - 1;
		}
	}
	else if (eclass == 5) {
		idecknum = FindLocation("Boarding_Cargohold");
		if(CheckAttribute(&Locations[idecknum], "boarding.locatorNum")) {
			maxcrew += sti(Locations[idecknum].boarding.locatorNum) - 1;
		}
	}
	return maxcrew;
}

// Финал захвата ФОРТА (общий для аварийного исхода в старте и штатного финала): событие, рабы, передача управления.
void Boarding_FortCaptureFinalize(ref echr)
{
	Event(FORT_CAPTURED, "l", sti(echr.index));
	echr.Ship.Crew.Quantity = BRD_FORT_SLAVES_BASE + rand(BRD_FORT_SLAVES_RAND); // рабы (остатки выживших)
	LaunchFortCapture(echr);
}

int Boarding_FortLandingStart(ref captain, int flagship, int allies)
{
	int total = Landing_Start(captain, "FortLanding", flagship);
	boarding_player_base_crew = sti(captain.FortLanding.Total);
	boarding_player_dmg_crew = 0.0;
	captain.FortLanding.Support = total - sti(captain.FortLanding.Total) + allies;
	return total + allies;
}

void Boarding_SetFortLandingModels(ref captain, int models)
{
	int owned = sti(captain.FortLanding.Total);
	int support = sti(captain.FortLanding.Support);
	int virtualModels = Landing_SupportModels(models, owned, support);
	boarding_escadra_crew = Landing_SupportModels(models, owned + support - boarding_escadra_raw, boarding_escadra_raw);
	boarding_quest_crew = virtualModels - boarding_escadra_crew;
	boarding_player_crew_per_chr = 0.0;
	if (models > virtualModels) boarding_player_crew_per_chr = makefloat(owned) / (models - virtualModels);
}

void Boarding_CancelFortLanding()
{
	if (!CheckAttribute(pchar, "FortLanding")) return;
	// LEO: на следующем ярусе учитываем уже полученный урон, но не выдаём награду за захват. Сцену этот расчёт не восстанавливает.
	if (boarding_player_dmg_crew > 0.0) Boarding_SettlePlayerCrew(pchar, false, false);
	else Landing_Resolve(pchar, "FortLanding", 0, 0, 0.0, 0);
	DeleteAttribute(pchar, "EnemyRank");
	DelEventHandler("LAi_event_GroupKill", "LAi_BoardingGroupKill");
	boarding_location = -1;
	LAi_boarding_process = false;
	bQuestCheckProcessFreeze = false;
}

// Раздача выживших матросов после боя: форт — десантом по компаньонам эскадры, корабль — в команду ГГ.
void Boarding_DistributeSurvivingCrew(float crew)
{
	if(IsFort) {
		int cn, iFree, iGive;
		ref officer;
		for (int j=1; j<COMPANION_MAX; j++) {
			cn = GetCompanionIndex(pchar, j);
			if (cn <= 0) {
				continue;
			}
			officer = GetCharacter(cn);
			if (!GetRemovable(officer)) {
				continue;
			}

			// LEO: доливаем по СВОБОДНЫМ местам (Max - актив - раненые). Раньше ставили ровно Max, и
			// лазарет оказывался сверх вместимости: после штурма форта корабль показывал полную команду
			// И столько же раненых. Остаток уменьшаем ровно на розданное - прежняя строка вычитала
			// Max + MinCrew, то есть больше выданного, и следующему кораблю доставался МИНУС.
			iFree = GetFreeCrewQuantity(officer);
			if (iFree <= 0) continue;
			iGive = makeint(crew);
			if (iGive > iFree) iGive = iFree;
			AddCharacterCrew(officer, iGive);
			crew -= iGive;
			if (crew <= 0) break;
		}
	}
	else {
		SetCrewQuantity(GetMainCharacter(),Makeint(crew + 0.3));
	}
}

// Мушкетный залп ПЕРЕД боем: потери ЦЕЛИ от залпа СТРЕЛКА (перк MusketsShoot, только корабль).
// Зеркально ГГ↔враг — формула едина, на месте вызова отличаются лог/статистика. Возвращает убитых у цели.
int Boarding_MusketVolley(int iShooterCrew, int iTargetCrew, ref rShooter, ref rTarget)
{
	float tmpDefence;
	int cas;
	tmpDefence = MakeFloat(GetSummonSkillFromName(rTarget, SKILL_DEFENCE)) / SKILL_MAX;
	cas = makeint(iShooterCrew * BRD_MUSKET_VOLLEY_FRACTION * AIShip_isPerksUse(CheckOfficersPerk(rTarget, "EmergentSurgeon"), BRD_MUSKET_SURGEON_OFF, BRD_MUSKET_SURGEON_ON));//aw013
	cas = makeint(cas * (BRD_MUSKET_DEFENCE_BASE - tmpDefence) / BRD_MUSKET_DEFENCE_DIV);
	if (cas > iTargetCrew) cas = iTargetCrew;//aw013
	PlaySound("INTERFACE\_musketshot_" + rand(BRD_MUSKET_SOUND_MAX) + ".wav");
	if (CheckForUpgrade(&RealShips[sti(rShooter.Ship.Type)],"SUP_FALCONETS")) cas = makeint(makefloat(cas)*BRD_MUSKET_FALCONETS_MULT+0.5); //Усиленные фальконеты
	if (CheckForUpgrade(&RealShips[sti(rTarget.Ship.Type)],"SUP_BIGSIDES")) cas = makeint(makefloat(cas)*BRD_MUSKET_BIGSIDES_MULT+0.5);  //Укрепленный фальшборт
	if (S_armor(rTarget)) cas = makeint(cas/BRD_MUSKET_ARMOR_DIV); // СФ
	// Gregg: оберег Мухачи («Две сестры») режет и потери от залпа - ровно те же -10%,
	// что и от ядер с картечью в море.
	if (GetCharacterItem(rTarget, "MuhachiAmulet") > 0) cas = makeint(makefloat(cas) * 0.9 + 0.5);
	// LEO: потери не бывают ни отрицательными, ни больше самой команды. Множитель Защиты
	//   (BRD_MUSKET_DEFENCE_BASE - def/SKILL_MAX) уходит в МИНУС, когда Защита цели со всеми
	//   бонусами переваливает за 210: залп «убивал» -2 человек, то есть ПОПОЛНЯЛ команду.
	//   Границы ставим ПОСЛЕ всех множителей - фальконеты умеют задирать их выше численности.
	if (cas < 0) cas = 0;
	if (cas > iTargetCrew) cas = iTargetCrew;
	return cas;
}

//Начать абордаж с главным персонажем
void LAi_StartBoarding(int locType, ref echr, bool isMCAttack)
{
    SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
	ResetSound(); // new
	PauseAllSounds();

	if(sti(RealShips[sti(echr.Ship.Type)].BaseType) <= SHIP_OCEAN) {//не исследуем квестовые, и лодку с фортом
		sTemp = RealShips[sti(echr.Ship.Type)].BaseName;
		Pchar.Encyclopedia.(sTemp) = "1";
	}
	if(bFillEncyShips) {
		aref aShips;
		makearef(aShips, Pchar.Encyclopedia);
		int Sum = GetAttributesNum(aShips);
		if (Sum != sti(pchar.questTemp.shipsearchcount)) {
			pchar.questTemp.shipsearchcount = Sum;
			log_info(GetConvertStrWithReplace("Variable_LAi_boarding_1", "Logs.txt", "#space#", " ") + XI_Convertstring(sTemp) + GetConvertStrWithReplace("Variable_LAi_boarding_2", "Logs.txt", "#space#", " ") + pchar.questTemp.shipsearchcount + GetConvertStrWithReplace("Variable_LAi_boarding_3", "Logs.txt", "#space#", " "));
			CheckAndUnlock("AchShipSearch", Sum);
		}
	}

	bQuestCheckProcessFreeze = true;//fix

	EmptyAbordageCharacters();
	// NK -->
    isMCAttack = (locType == BRDLT_FORT);
    IsFort = isMCAttack;
	// NK <--
	//Установим обработчик на убийство группы
	SetEventHandler("LAi_event_GroupKill", "LAi_BoardingGroupKill", 1);
	//Настроим интерфейс
	DeleteBattleInterface();
	InitBattleLandInterface();
	//Сохраним индекс врага
	boarding_echr_index = sti(echr.index);
	//Параметры сражающихся сторон
	ref mchr = GetMainCharacter();
	Landing_Resolve(mchr, "FortLanding", 0, 0, 0.0, 0);
	boarding_escadra_raw = 0;
	int mclass = GetCharacterShipClass(mchr);
	int mcrew = GetCrewQuantity(mchr);
	int mcrewBak;
	// Saving enemy captain rank for future use in CalculateAppropriateSkills (Gray 12.02.2005)
	mchr.EnemyRank = echr.rank

	DeleteAttribute(pchar, "abordage_active");
	// boal учет оружия 21.01.2004 -->
	Log_TestInfo(GetConvertStrWithReplace("Variable_LAi_boarding_4", "Logs.txt", "#space#", " ") + mcrew);
	mcrew = GetWeaponCrew(mchr, mcrew);
	int flagshipLanding = mcrew;

	// boal 21.01.2004 <--
	int eclass = GetCharacterShipClass(echr);
	int ecrew = GetCrewQuantity(echr);
	int ecrewBak;

    Log_TestInfo(GetConvertStrWithReplace("Variable_LAi_boarding_5", "Logs.txt", "#space#", " ") + mcrew + GetConvertStrWithReplace("Variable_LAi_boarding_6", "Logs.txt", "#space#", " ") + ecrew);

    boarding_location_type = locType;
	// Сдача в плен -->
	//квестовые не сдаются
    if (!CheckAttribute(echr, "DontRansackCaptain") && boarding_location_type != BRDLT_FORT) {
		if (CheckForSurrender(mchr, echr, 1) || TestRansackCaptain) {// 1 - это учет первый раз, до битвы на палубе
			echr.ship.crew.morale = BRD_SURRENDER_ENEMY_MORALE;// после захвата у них мораль такая

			AddCrewMorale(mchr, BRD_SURRENDER_PLAYER_MORALE_GAIN);

			SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
			//Следующей локации нет
			DelEventHandler("LAi_event_GroupKill", "LAi_BoardingGroupKill");
			boarding_location = -1;

			// Saved value is not needed anymore... (Gray 14.02.2005)
			DeleteAttribute(GetMainCharacter(), "EnemyRank");
			bQuestCheckProcessFreeze = false;//fix

			//Настроим интерфейс
			Log_SetActiveAction("Nothing");
			EndBattleLandInterface();
			//Выгружаемся в интерфейс

			ChangeCrewExp(pchar, "Soldiers", AIShip_isPerksUse(CheckOfficersPerk(pchar, "SeaWolf"),BRD_SOLDIER_EXP_SURRENDER_OFF, BRD_SOLDIER_EXP_SURRENDER_ON));
			LaunchRansackMain(pchar, echr, "crew");
			// на деле параметры LaunchRansackMain не важны совсем - все определеятеся от реалий - жив кэп и сколько у него матросов - их и обрабатываем
			// но они используются в сообщениях  crew - это сдался сразу
			LAi_boarding_process = false;
			Event(SHIP_CAPTURED, "l", sti(echr.index));
			return;
		}
	}
	// Сдача в плен <--

	// MusketsShoot -->
    if (CheckOfficersPerk(mchr, "MusketsShoot") && IsFort == false)
    {
		ecrewBak = Boarding_MusketVolley(mcrew, ecrew, mchr, echr);
		ecrew = ecrew - ecrewBak;
		Log_SetStringToLog(GetConvertStrWithReplace("Variable_LAi_boarding_7", "Logs.txt", "#space#", " ") + ecrewBak + GetConvertStrWithReplace("Variable_LAi_boarding_8", "Logs.txt", "#space#", " "));
	}
	if (CheckOfficersPerk(echr, "MusketsShoot") && IsFort == false)
	{
		mcrewBak = Boarding_MusketVolley(ecrew, mcrew, echr, mchr);
		mcrew = mcrew - mcrewBak;
		Log_SetStringToLog(GetConvertStrWithReplace("Variable_LAi_boarding_9", "Logs.txt", "#space#", " ") + mcrewBak + GetConvertStrWithReplace("Variable_LAi_boarding_10", "Logs.txt", "#space#", " "));
		Statistic_AddValue(mchr, "DeadCrewBoard", mcrewBak);
    }
    // MusketsShoot <--

	if(ecrew < 1) ecrew = 1;
	if(mcrew < 1) mcrew = 1;
	boarding_erank = sti(echr.rank);

	// база для расчета
	boarding_enemy_base_crew = ecrew;
	boarding_player_base_crew = mcrew;

	//бонус хп игроку при опыте+морали солдат выше среднего
	GetBoardingHP(mchr, echr, &boarding_player_hp, &boarding_enemy_hp);

	//Определяем цепь локаций для абордажа
	boarding_location = -1;

	string deckID = "";
	isMCAttack   = true;// boal 110804 fix всегда герой
	if(isMCAttack) {
		deckID = GetShipLocationID(echr);
	}
	else {
		deckID = GetShipLocationID(mchr);
	}
	int locID = -1;
	if(deckID != "") {
		locID = FindLocation(deckID);
		if(locID < 0) {
			Trace("Boarding: ship location not found <" + deckID + ">, set default");
			deckID = "";
		}
	}
	if(deckID == "") {
		deckID = Boarding_GetDefaultLocation(locType);
		locID = FindLocation(deckID);
	}
	if (locID < 0) {   // исключительная ситуация
		Trace("Boarding: ship location not found <" + deckID + ">, no start boarding");
		// Saved value is not needed anymore... (Gray 14.02.2005)
		DeleteAttribute(GetMainCharacter(), "EnemyRank");
	    bQuestCheckProcessFreeze = false;//fix

		if(boarding_location_type == BRDLT_SHIP) {
            ChangeCrewExp(pchar, "Soldiers", AIShip_isPerksUse(CheckOfficersPerk(pchar, "SeaWolf"),BRD_SOLDIER_EXP_SURRENDER_OFF, BRD_SOLDIER_EXP_SURRENDER_ON));
            SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
			LaunchRansackMain(GetMainCharacter(), echr, "captain");	  // на деле параметры LaunchRansackMain не важны совсем - все определеятеся от реалий
            LAi_boarding_process = false;
			Event(SHIP_CAPTURED, "l", sti(echr.index)); // to_do can be harmfull
		}
		else {
			if (boarding_location_type == BRDLT_FORT) {
                ChangeCrewExp(pchar, "Soldiers", AIShip_isPerksUse(CheckOfficersPerk(pchar, "SeaWolf"),BRD_SOLDIER_EXP_SURRENDER_OFF, BRD_SOLDIER_EXP_SURRENDER_ON));
                SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
				Boarding_FortCaptureFinalize(echr);
                LAi_boarding_process = false;
			}
			else {
				Trace("Boarding: unknow boarding location type");
			}
		}
		return;
	}

	pchar.abordage = 0;
	//Определяем размеры команд
	boarding_enemy = echr;

	//Максимальное количество человек на корабле
	int maxcrew = Boarding_ComputeMaxCrew(locID, eclass);
	// на тартане каюты нет
	if (boarding_location_type == BRDLT_SHIP && eclass != 7) {
    	boarding_enemy.ShipCabinLocationId = GetShipCabinID(echr);
	}

	//Отношение сторон

	// fort -->
	// boal 21.01.2004 -->
    if(IsFort) {
        // LEO: помощь эскадре — её уцелевшая команда штурмует вместе с ГГ, учитываем в силе штурмующих (иначе враг получает бонус за перевес, будто ГГ штурмует один)
        if (CheckQuestAttribute("Union_with_Escadra", "Yes") && CheckAttribute(NullCharacter, "Siege.Colony") && CheckAttribute(echr, "City") && echr.City == NullCharacter.Siege.Colony)
        {
            boarding_escadra_nation = sti(NullCharacter.Siege.nation);
            boarding_escadra_raw = SiegeSurvivingCrewTotal();
        }
        mcrew = Boarding_FortLandingStart(mchr, flagshipLanding, boarding_escadra_raw);
        //Максимальное количество человек
        Log_TestInfo("Итого: До расчета mcrew = "+mcrew+ " ecrew = "+ ecrew + " boarding_enemy_hp = "+ boarding_enemy_hp + " boarding_player_hp = "+boarding_player_hp);
    }
    // boal 21.01.2004 <--
    // fort <--
	float rel;
	float reldiv;
	boarding_rank_diff = sti(pchar.rank) - sti(echr.rank); // запоминаем для пересчёта на следующих локациях
	boarding_offic_rank_bonus = 0;                         // офицеров пересчитает блок ниже, старое значение не тащим
	Boarding_ApplyStrengthBonuses(mcrew, ecrew);           // бонусы ХП/уровня по стартовому раскладу
	if(mcrew > ecrew) {
		rel = Makefloat(mcrew)/makefloat(maxcrew);
		mcrew = maxcrew;
		ecrew = MakeInt(ecrew / (rel*PLAYER_BOARDING_COEFF) + 0.5);
		if (ecrew < 1) ecrew = 1;
		reldiv = Makefloat(mcrew)/Makefloat(ecrew);//половина преимущества - в тушках
		reldiv = sqrt(reldiv);
		ecrew = MakeInt(ecrew*reldiv);
		if (ecrew > maxcrew) {ecrew = maxcrew;}
	}
	else {
		rel = Makefloat(ecrew)/makefloat(maxcrew);
		ecrew = maxcrew;
		mcrew = MakeInt(mcrew*PLAYER_BOARDING_COEFF / rel + 0.5);
		if (mcrew < 1) mcrew = 1;
		reldiv = Makefloat(ecrew)/Makefloat(mcrew);//половина преимущества - в тушках
		reldiv = sqrt(reldiv);
		mcrew = MakeInt(mcrew*reldiv);
	}
	if(mcrew < 1) mcrew = 1;
	if(ecrew < 1) ecrew = 1;
	Log_TestInfo("После приведения mcrew = "+mcrew+ " ecrew = "+ ecrew + " boarding_enemy_hp = "+ boarding_enemy_hp + " boarding_player_hp = "+boarding_player_hp+" e_rank_bonus="+e_rank_bonus+" m_rank_bonus="+m_rank_bonus);
	boarding_enemy_crew        = ecrew;
	boarding_enemy_crew_start  = ecrew;

	// START MOD Code by Stone-D : 30/07/2003
	boarding_enemy_crew_per_chr = makefloat(boarding_enemy_base_crew / makefloat(ecrew)); // Stone-D : For calculating final crew numbers
	// END MOD Code by Stone-D : 30/07/2003

	// impact from officers
	boarding_officers = 0;
	int iOfficer = -1;
	int avg_officers_rank = 0;
	for(int i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		iOfficer = GetOfficersIndex(pchar, i);
		if (iOfficer != -1)
		{
			sld = &characters[iOfficer];
			if (!CheckAttribute(sld, "AboardRestriction"))
			{
				boarding_officers = boarding_officers + 1;
				if (CheckAttribute(sld, "rank")) avg_officers_rank += sti(sld.rank);
				LAi_SetOfficerType(sld);
			}
		}
		if (i == MAX_NUM_FIGHTERS && boarding_officers > 0) avg_officers_rank /= boarding_officers;
	}
	// LEO: надбавку врагу за офицеров ГГ считаем ОДИН раз (состав офицеров за абордаж не меняется), но
	//   держим отдельно — пересчёт бонусов на каждой локации обнуляет e_rank_bonus и добавляет её заново.
	boarding_offic_rank_bonus = avg_officers_rank*boarding_officers/(BRD_OFFICER_BONUS_K*MAX_NUM_FIGHTERS*(BRD_OFFICER_BONUS_MODBASE-MOD_SKILL_ENEMY_RATE));
	e_rank_bonus += boarding_offic_rank_bonus;

    boarding_player_crew_per_chr = makefloat((boarding_player_base_crew)/ makefloat(mcrew)); //приведение типа
	boarding_player_crew       = mcrew;
	boarding_player_crew_start = mcrew;
	boarding_player_dmg_crew   = 0.0; // LEO: сброс накопленного урона на старте абордажа (потери считаем по нанесённому урону)
	boarding_escadra_crew = 0;
	boarding_quest_crew = 0;
	if (IsFort) Boarding_SetFortLandingModels(mchr, mcrew);
	//Выставим игроку и офицерам максимальную жизнь и запомним адреса
	LAi_SetCurHPMax(mchr);
	boarding_adr[0].location = mchr.location;
	boarding_adr[0].group    = mchr.location.group;
	boarding_adr[0].locator  = mchr.location.locator;
	for(i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		int idx = GetOfficersIndex(GetMainCharacter(), i);
		//Boyer mod
		int k = i % 3;
		if(k==0) k = 3;
		if(idx < 0)
		{
			//Boyer mod
			boarding_adr[k].location = "";
			boarding_adr[k].group	= "";
			boarding_adr[k].locator  = "";
			continue;
		}
		LAi_SetCurHPMax(&Characters[idx]);
		boarding_adr[k].location = Characters[idx].location;
		boarding_adr[k].group	= Characters[idx].location.group;
		boarding_adr[k].locator  = Characters[idx].location.locator;
	}
	//Стартуем
	LAi_boarding_process = true;
	LAi_LoadLocation(deckID, locType);
	if (!LAi_boarding_process) return;
	CreateEntity(&boarding_fader, "fader");
	SendMessage(&boarding_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);
}

//Загрузить локацию абордажа
void LAi_LoadLocation(string locationID, int locType)
{
	ReloadProgressStart();
	//Ищем локацию
	int locIndex = FindLocation(locationID);
	Log_SetActiveAction("Nothing");
    Dead_Char_num = 0; // boal dead body
	if(locIndex >= 0)
	{
		//Устанавливаем главного персонажа
		ref mchr = GetMainCharacter();
		mchr.location = locationID;
		mchr.location.group = "rld";
		// boal random place to star battle 28/11/03 -->
		int locNum[20];
		int locI;
		int maxLocators = 2;

		if (CheckAttribute(&Locations[locIndex], "boarding.locatorNum"))
		{
			maxLocators = sti(Locations[locIndex].boarding.locatorNum);
		}
		locI = 0;
		locNum[locI] = rand(maxLocators-1);
		string sLocType = "loc";
		// определение стороны на палубе
		if (CheckAttribute(&Locations[locIndex], "UpDeckType"))
		{
		    sLocType = ChooseShipUpDeck(mchr, boarding_enemy);
		}

		mchr.location.locator = sLocType + locNum[locI];
		// для каюты перекрыть
		if (CheckAttribute(&Locations[locIndex], "CabinType") && CheckAttribute(&Locations[locIndex], "boarding.Loc.Hero"))
		{
		    mchr.location.locator = Locations[locIndex].boarding.Loc.Hero;
		}
		bool   okLoc;
		int iLoc, j;
		// boal random place to star battle 28/11/03 <--
		if (!CheckAttribute(&Locations[locIndex], "CabinType"))
		{ // не грузим офов, если каюту
	        //Устанавливаем офицеров, если такие есть
	        int logined = 0;
			//Boyer mod
			for(int i = 1; i <= MAX_NUM_FIGHTERS ; i++)
			{
				int idx = GetOfficersIndex(mchr, i);
				if(idx < 0) continue;
				if (logined > maxLocators) break;
				if (CheckAttribute(characters[idx],"AboardRestriction")) continue;

				LAi_ClearOfficerPost(&Characters[idx]);
				DeleteAttribute(&Characters[idx], "location");
				// boal чтоб грузились в фортах -->
				Characters[idx].location.loadcapture = true;
			    // boal чтоб грузились в фортах <--
				// boal random place to star battle 28/11/03 -->
				iLoc = rand(5);//3

				okLoc = false;
				while (!okLoc)
				{
				    for(j = 0; j <=locI; j++)
				    {
				        if (locNum[j] == iLoc)
				        {
				            iLoc = rand(5);//3
				            okLoc = false;
				            break;
				        }
				        okLoc = true;
				    }
				    //Boyer mod to fix stack error
				    okLoc = true;
				}
				locI++;
			    locNum[locI] = iLoc;
				// boal random place to star battle 28/11/03 <--
				ChangeCharacterAddressGroup(&Characters[idx], locationID, "rld", sLocType + locNum[locI]);
				logined = logined + 1;
			}
			boarding_player_crew = boarding_player_crew - logined;
		}
		//Перегружаемся в локацию
		boarding_location = locIndex;
		if(LoadLocation(&Locations[boarding_location]))
		{
			//работа с сундуком в каюте boal -->
			if (CheckAttribute(&Locations[boarding_location], "CabinType"))
			{
				FillAboardCabinBox(&Locations[boarding_location], boarding_enemy);
			}
			//Расставляем персонажей
			LAi_SetBoardingActors(locationID);
			LandRadar_RetagCombat(); // LEO: мгновенно покрасить блипы свои/враги на старте абордажа (отношения уже расставлены); дальше поддерживает поллинг BLI_UpdateObjectData
			// boal <--
			//Вытащим саблю
			SendMessage(&mchr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightMode", 1);
			//Запретим диалог
			dialogDisable = true;
		}else{
			Trace("Boarding: Boarding location not loaded, current loc <" + locationID + ">");
			Boarding_CancelFortLanding();
		}
	}else{
		Trace("Boarding: Boarding location not found, current loc <" + locationID + ">");
		Boarding_CancelFortLanding();
	}
	ReloadProgressEnd();
	SetPerspectiveSettings();
	//#20190815-01
	SetPerspective2Settings();
	PostEvent("LoadSceneSound", 500);
	pchar.OfficerAttRange = BRD_OFFICER_ATT_RANGE;//автоагр абордажников на палубе
}

//Перегрузиться в следующую локации
void LAi_ReloadBoarding()
{
	//Разрешим диалоги
	dialogDisable = false;
	//Проверим предыдущую локацию
	if(boarding_location < 0)
	{
		Trace("Boarding: No loaded current boarding location");
		Boarding_CancelFortLanding();
		LAi_boarding_process = false;
		return;
	}

	csmHideLootCollectorBox(true);

	//Установить хендлеры для обработки
	SetEventHandler("FaderEvent_StartFade", "LAi_ReloadStartFade", 0);
	SetEventHandler("FaderEvent_EndFade", "LAi_ReloadEndFade", 0);
	//Создаём фейдер и запускаем
	CreateEntity(&boarding_fader, "fader");

	string nextDeck = Locations[boarding_location].boarding.nextdeck;
	if (nextDeck != "" && boarding_enemy_crew >= 1)
	{
		int nextIndex = FindLocation(nextDeck);
		if (nextIndex >= 0 && CheckAttribute(&Locations[nextIndex], "image"))
		{
			pchar.loadscreen = Locations[nextIndex].image;
			SendMessage(&boarding_fader, "ls", FADER_PICTURE0, pchar.loadscreen);
		}
	}
	float fadeOutTime = RELOAD_TIME_FADE_OUT;
	SendMessage(&boarding_fader, "lfl", FADER_OUT, fadeOutTime, false);
	SendMessage(&boarding_fader, "l", FADER_STARTFRAME);
}

void LAi_ReloadStartFade()
{
	//Выгружаем локацию
	ResetSound(); // new
	PauseAllSounds();
	SendMessage(&Particles,"l", PS_CLEAR_CAPTURED);
	DelEventHandler("FaderEvent_StartFade", "LAi_ReloadStartFade");
	if(boarding_location >= 0) UnloadLocation(&Locations[boarding_location]);
}

// LEO: накопить НАНЕСЁННЫЙ урон по абордажной партии ГГ — ТОЛЬКО МАТРОСЫ (ни ГГ, ни офицеры) — в единицах
// команды (потери считаем по урону, а не по трупам). Зовётся из LAi_ApplyCharacterDamage. dmg уже эффективный
// (без оверкилла, обрезан по HP), maxhp — макс. HP цели. Хилки зельями не занижают потери: каждый снятый HP матроса
// учтён здесь в момент нанесения. УРОН ПО ОФИЦЕРАМ КОМАНДУ НЕ СПИСЫВАЕТ (по просьбе юзера): офицер — не «тело»
// экипажа, его ранения/гибель на палубе не должны уменьшать численность матросов корабля (живучесть офицера ×0.7
// в LAi_ApplyCharacterDamage сохранена — это отдельная механика, её НЕ трогаем).
void Boarding_AccumPlayerDamage(aref chr, float dmg, float maxhp)
{
	if (boarding_location < 0) return;                       // не идёт бой на палубе
	if (dmg <= 0.0 || maxhp <= 0.0) return;
	if (!CheckAttribute(chr, "chr_ai.group")) return;
	if (chr.chr_ai.group != LAI_GROUP_PLAYER) return;       // абордажная партия ГГ
	if (sti(chr.index) == GetMainCharacterIndex()) return;  // самого ГГ не считаем
	if (CheckAttribute(chr, "chr_ai.type") && chr.chr_ai.type == LAI_TYPE_OFFICER) return; // офицеров не считаем - их урон НЕ бьёт по команде
	if (CheckAttribute(chr, "BoardEscadra")) return; // LEO: боец десанта эскадры — его урон/гибель НЕ списывается с команды ГГ (эскадра несёт свои потери сама)
	if (CheckAttribute(chr, "AboardFantomMush")) return; // LEO: бонусные мушкетёры вне числа моделей, на которое делится экипаж.
	if (IsFort && CheckAttribute(pchar, "FortLanding"))
	{
		if (CheckAttribute(chr, "BoardQuestTrooper") || !CheckAttribute(chr, "AboardFantom")) return;
	}
	boarding_player_dmg_crew = boarding_player_dmg_crew + (dmg / maxhp) * boarding_player_crew_per_chr;
}

bool Boarding_SettlePlayerCrew(ref mchar, bool bCaptanSurrender, bool awardVictory)
{
	bool trackedFort = IsFort && CheckAttribute(mchar, "FortLanding");
	// LEO: остаток команды — по НАНЕСЁННОМУ урону (boarding_player_dmg_crew), а не по убитым болванчикам.
	// Хилки зельями уже учтены при нанесении урона, поэтому не занижают потери. Дальше идут штатные
	// модификаторы выживаемости (Защита/PLAYER_BOARDING_COEFF/Экстренный хирург/лекарства).
	float crew = boarding_player_base_crew - boarding_player_dmg_crew;
	if (crew < 0.0) crew = 0.0;
	// boal 22.01.2004 -->
	float fDefenceSkill = BRD_CASUALTY_DEFENCE_BASE + MakeFloat(GetSummonSkillFromName(mchar, SKILL_DEFENCE)) / SKILL_MAX;//0.77...1.7
	int deadCrew    = makeint((boarding_player_base_crew - crew) / (fDefenceSkill + (1/PLAYER_BOARDING_COEFF - 1) + AIShip_isPerksUse(CheckOfficersPerk(mchar, "EmergentSurgeon"), BRD_CASUALTY_SURGEON_OFF, BRD_CASUALTY_SURGEON_ON))); // бонус выжившим
	//25% не участвуют в бою, Защита -12...+42%, Экстренный хирург 20%
	int deadCrewWOMedic = makeint(boarding_player_base_crew - crew); // без бонуса
	float leaderSkill = GetSummonSkillFromNameToOld(mchar, SKILL_LEADERSHIP);
	int iTemp;
	float fTemp;
	if (deadCrew < 0) {deadCrew = 0;}
	if (trackedFort && deadCrew > sti(mchar.FortLanding.Total)) deadCrew = sti(mchar.FortLanding.Total);
	if (leaderSkill < 1) {leaderSkill = 1;} //fix
	// расчет медицины -->
	iTemp = deadCrewWOMedic - deadCrew;
	iTemp *= BRD_MEDICINE_PER_WOUNDED;
	if (iTemp > 0){
		if(CheckShipSituationDaily_GenQuest(pchar) > 1) {// если эпидемия - матросы не лечатся, лекарство не потребляется
			deadCrew += deadCrewWOMedic;
		}
		else if (GetCargoGoods(mchar, GOOD_MEDICAMENT) < iTemp) {// не хватает лекарств
			fTemp = makefloat(iTemp);
			iTemp = MakeInt(fTemp/3 * (1.0 - makefloat(GetCargoGoods(mchar, GOOD_MEDICAMENT)) / fTemp)); // умерли от ран
			RemoveCharacterGoodsSelf(mchar, GOOD_MEDICAMENT, GetCargoGoods(mchar, GOOD_MEDICAMENT)); // все нулим
			deadCrew += iTemp; // трупов больше
			Log_Info(GetConvertStrWithReplace("Variable_LAi_boarding_11", "Logs.txt", "#space#", " ") + iTemp + GetConvertStrWithReplace("Variable_LAi_boarding_12", "Logs.txt", "#space#", " "));
		}
		else {
			RemoveCharacterGoodsSelf(mchar, GOOD_MEDICAMENT, iTemp);
			if (GetCargoGoods(mchar, GOOD_MEDICAMENT) < (crew-deadCrew)*BRD_MEDICINE_LOW_WARN_MULT) {
				Log_Info(GetConvertStrWithReplace("Variable_LAi_boarding_13", "Logs.txt", "#space#", " ") + mchar.Ship.Name + GetConvertStrWithReplace("Variable_LAi_boarding_14", "Logs.txt", "#space#", " "));
			}
		}
	}
	// расчет медицины <--
	if (trackedFort && deadCrew > sti(mchar.FortLanding.Total)) deadCrew = sti(mchar.FortLanding.Total);
	// Saved value is not needed anymore... (Gray 14.02.2005)
	DeleteAttribute(mchar, "EnemyRank");
	bQuestCheckProcessFreeze = false;//fix

	RemoveCharacterGoodsSelf(mchar, GOOD_WEAPON, makeint(deadCrew + 0.5));

	crew = boarding_player_base_crew - deadCrew; // выжившие с бонусом
	// LEO: для форта разыгрываем общее число раненых как раньше, но записываем их на исходные корабли.
	int woundedCrew;
	if (trackedFort) woundedCrew = Crew_RollWounded(mchar, deadCrew);
	else woundedCrew = Crew_SkimWounded(mchar, deadCrew);
	// LEO: реально УБИТЫЕ = павшие минус те, кого вытащили в раненых. Нужны только для морали:
	// команда/статистика/опыт считаются от deadCrew как и раньше (раненые всё равно выбыли из строя).
	int killedCrew = deadCrew - woundedCrew;
	if (killedCrew < 0) {killedCrew = 0;}
	Statistic_AddValue(mchar, "Sailors_dead", deadCrew);
	Statistic_AddValue(mchar, "DeadCrewBoard", deadCrew);
	AddCharacterExpToSkill(mchar, "Defence", makeint(deadCrew / BRD_CASUALTY_SKILL_EXP_DIV + 0.5)); //качаем защиту
	AddCharacterExpToSkill(mchar, "Grappling", makeint(deadCrew / BRD_CASUALTY_SKILL_EXP_DIV + 0.5));

	// после боя падает мораль — по УБИТЫМ, не по deadCrewWOMedic (там сидел весь сырой урон: и
	// легкораненые, вернувшиеся в строй по лекарствам, и весь пул раненых)
	fTemp = 0.0;
	if (boarding_player_base_crew > 0) fTemp = makefloat(killedCrew)/boarding_player_base_crew;
	fTemp -= BRD_MORALE_LOSS_THRESHOLD;//незаметные потери + капитан выжил
	if (fTemp < 0) {fTemp = 0;}
	fTemp -= leaderSkill/BRD_MORALE_LEADERSHIP_DIV;//0.7-6.7% убитых Лидерство прощает
	if (fTemp < 0) {fTemp /= 2;}//падет быстрее чем растет
	fTemp *= (0-BRD_MORALE_LOSS_SCALE);
	iTemp = makeint(fTemp);
	if (fTemp > 0) {Log_Info(GetConvertStrWithReplace("Variable_LAi_boarding_22", "Logs.txt", "#space#", " ")+abs(iTemp)+GetConvertStrWithReplace("Variable_LAi_boarding_24", "Logs.txt", "#space#", " "));}
	else {Log_Info(GetConvertStrWithReplace("Variable_LAi_boarding_23", "Logs.txt", "#space#", " ")+abs(iTemp)+GetConvertStrWithReplace("Variable_LAi_boarding_24", "Logs.txt", "#space#", " "));}
	if (trackedFort)
	{
		float exp = 0.0;
		if (awardVictory)
		{
			bool seaWolf = CheckOfficersPerk(mchar, "SeaWolf");
			exp = AIShip_isPerksUse(seaWolf, BRD_SOLDIER_EXP_FORT_OFF, BRD_SOLDIER_EXP_FORT_ON);
			if (bCaptanSurrender) exp = AIShip_isPerksUse(seaWolf, BRD_SOLDIER_EXP_CAPTIVE_OFF, BRD_SOLDIER_EXP_CAPTIVE_ON);
		}
		Landing_Resolve(mchar, "FortLanding", deadCrew, woundedCrew, exp, iTemp);
	}
	else AddCrewMorale(mchar, iTemp);

	Log_TestInfo(GetConvertStrWithReplace("Variable_LAi_boarding_15", "Logs.txt", "#space#", " ") + crew +GetConvertStrWithReplace("Variable_LAi_boarding_16", "Logs.txt", "#space#", " "));
	// boal 22.01.2004 <--

	if (!trackedFort) Boarding_DistributeSurvivingCrew(crew);
	return trackedFort;
}

void LAi_ReloadEndFade()
{
	/* Восстанавливаем озвучку губера */
	LAi_type_huberstay_set_Boarding(false);
	bool bCaptanSurrender = false;
	//Загружаем следующую локацию
	DelEventHandler("FaderEvent_EndFade", "LAi_ReloadEndFade");
	SendMessage(&boarding_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);
	if (boarding_location < 0) return;
	if (SF_boarding_check()) return;

	if(CheckAttribute(&Locations[boarding_location], "boarding.nextdeck") && Locations[boarding_location].boarding.nextdeck != "" && boarding_enemy_crew >= 1) {
		Trace("Boarding: go to inside location");
		LAi_LoadLocation(Locations[boarding_location].boarding.nextdeck, -1);
		return; //Jason Продолжаем абордаж
	}

	if (CheckAttribute(boarding_enemy, "ShipCabinLocationId")) {//остался только кэп
		if (!CheckAttribute(boarding_enemy, "DontRansackCaptain")) {//квестовые не сдаются
			bCaptanSurrender = (Surrendered) || (TestRansackCaptain);
		}

		if (!bCaptanSurrender) {
			boarding_enemy_crew = 1;
			if(CheckAttribute(&Locations[FindLocation(boarding_enemy.ShipCabinLocationId)], "image")) {
				SendMessage(&boarding_fader, "ls", FADER_PICTURE0, Locations[FindLocation(boarding_enemy.ShipCabinLocationId)].image);
			}
			LAi_LoadLocation(boarding_enemy.ShipCabinLocationId, -1);
			DeleteAttribute(boarding_enemy, "ShipCabinLocationId"); // чтоб не зациклилось
			return;
		}
	}

	ref mchar       = GetMainCharacter();

	// квестовая сдача в плен кэпа
	if (CheckAttribute(mchar, "GenQuest.QuestAboardCaptanSurrender")) {
		DeleteAttribute(mchar, "GenQuest.QuestAboardCaptanSurrender"); // разово
		bCaptanSurrender = true;
	}
	SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
	//Следующей локации нет
	DelEventHandler("LAi_event_GroupKill", "LAi_BoardingGroupKill");
	boarding_location = -1;
	bool trackedFort = Boarding_SettlePlayerCrew(mchar, bCaptanSurrender, true);
	//Пересчитываем команду соперника
	float crew = 0;
	if (boarding_echr_index >= 0) {
		// LEO: РАНЕНЫЕ ЗАЩИТНИКОВ. Ваниль обнуляла команду приза начисто ("все трупы"), поэтому на
		// захваченном корабле раненых не было НИКОГДА и механика на нём не работала вовсе (у ГГ работала).
		// Теперь часть павших защитников спасаем в раненых по навыку Защиты ИХ капитана - достаются с призом.
		Crew_SkimWounded(&Characters[boarding_echr_index], makeint(boarding_enemy_base_crew));
		SetCrewQuantity(&Characters[boarding_echr_index], MakeInt(crew + 0.3));
		boarding_echr_index = -1;
	}

	//Настроим интерфейс
	Log_SetActiveAction("Nothing");
	EndBattleLandInterface();
	//Востановим адреса
	ref mchr = GetMainCharacter();
	mchr.location         = boarding_adr[0].location;
	mchr.location.group   = boarding_adr[0].group;
	mchr.location.locator = boarding_adr[0].locator;

	for(int i = 1; i <=MAX_NUM_FIGHTERS; i++)
	{
		int idx = GetOfficersIndex(GetMainCharacter(), i);
		//Boyer mod
		int k = i % 3;
		if(k==0) k = 3;
		if(idx < 0) continue;
		LAi_ClearOfficerPost(&Characters[idx]);
		Characters[idx].location			= boarding_adr[k].location;
		Characters[idx].location.group	= boarding_adr[k].group;
		Characters[idx].location.locator = boarding_adr[k].locator;
	}
	//Выгружаемся в интерфейс
	LAi_boarding_process = false;
	// ЭСКОРТ-засада ([[gov-escort-fork]]): помеченные капитаны не дропают лут (золото/оружие) при обыске тела -
	// клинок хороший для боя, но снять с трупа нельзя (награда - пощада в каюте, GentlemenHunter +100/+100).
	if (CheckAttribute(boarding_enemy, "GovEscortNoLoot")) { DeleteAttribute(boarding_enemy, "items"); boarding_enemy.Money = 0; }
	// START MOD Code by Stone-D : 27/07/2003
	if (bCaptanSurrender)
	{
		if (!trackedFort) ChangeCrewExp(pchar, "Soldiers", AIShip_isPerksUse(CheckOfficersPerk(pchar, "SeaWolf"), BRD_SOLDIER_EXP_CAPTIVE_OFF, BRD_SOLDIER_EXP_CAPTIVE_ON));
		LAi_SetCurHPMax(boarding_enemy);  // нужно, чтоб был живой!!!
		SetCrewQuantity(boarding_enemy, MakeInt(boarding_enemy_base_crew*(rand(BRD_CAPTAIN_RESURRECT_CREW_RAND)*BRD_CAPTAIN_RESURRECT_CREW_MULT))); // раз кэп живой, то можно раненых воскресить
		LaunchRansackMain(pchar, boarding_enemy, "captain"); //капитан в плен
		LAi_boarding_process = false;
		Event(SHIP_CAPTURED, "l", sti(boarding_enemy.index));
		return;
	}
	// END MOD Code by Stone-D : 27/07/2003
	if(boarding_location_type == BRDLT_SHIP)
	{
		ChangeCrewExp(pchar, "Soldiers", AIShip_isPerksUse(CheckOfficersPerk(pchar, "SeaWolf"),BRD_SOLDIER_EXP_SHIP_OFF, BRD_SOLDIER_EXP_SHIP_ON));
		// Смерть капитана оформляется после формы: до неё он нужен для выбора исхода.
		LaunchRansackMain(pchar, boarding_enemy, ""); //не сдался
		LAi_boarding_process = false;
		Event(SHIP_CAPTURED, "l", sti(boarding_enemy.index));
		return;
	}
	if(boarding_location_type == BRDLT_FORT)
	{
		if (!trackedFort) ChangeCrewExp(pchar, "Soldiers", AIShip_isPerksUse(CheckOfficersPerk(pchar, "SeaWolf"),BRD_SOLDIER_EXP_FORT_OFF, BRD_SOLDIER_EXP_FORT_ON));
		Boarding_FortCaptureFinalize(boarding_enemy);
		LAi_boarding_process = false;
		return;
	}
	Trace("Boarding: unknow boarding location type");
	Log_TestInfo("Boarding - unknown boarding location type : " + boarding_location_type);
}

string Boarding_FindFreeOfficerPost(aref chr, aref reserved)
{
	if (!CheckAttribute(loadedLocation, "locators.rld")) return "";
	aref locators;
	makearef(locators, loadedLocation.locators.rld);
	string best = "";
	float nearest = 0.0;
	int locatorCount = GetAttributesNum(locators);
	for (int i = 0; i < locatorCount; i++)
	{
		string name = GetAttributeName(GetAttributeN(locators, i));
		int firstDigit;
		if (findsubstr(name, "loc", 0) == 0) firstDigit = 3;
		else if (findsubstr(name, "aloc", 0) == 0) firstDigit = 4;
		else continue;
		if (strlen(name) <= firstDigit) continue;
		string suffix = strcut(name, firstDigit, strlen(name) - 1);
		string number = "" + sti(suffix);
		if (suffix != number || CheckAttribute(reserved, name)) continue;
		float distance;
		if (!GetCharacterDistByLoc(chr, "rld", name, &distance)) continue;
		if (best != "" && distance >= nearest) continue;
		if (!LAi_CheckLocatorFree("rld", name)) continue;
		best = name;
		nearest = distance;
	}
	return best;
}

void Boarding_DisperseOfficer(aref chr, aref reserved)
{
	string post = Boarding_FindFreeOfficerPost(chr, reserved);
	chr.chr_ai.OfficerAtPost = true;
	DeleteAttribute(chr, "chr_ai.LandTacCmdTarget");
	DeleteAttribute(chr, "chr_ai.LandTacHold");
	if (post != "")
	{
		reserved.(post) = true;
		LAi_SetHuberStayTypeNoGroup(chr);
		chr.chr_ai.type.main_group = "rld";
		chr.chr_ai.type.main_locator = post;
		LAi_tmpl_goto_InitTemplate(chr);
		LAi_tmpl_goto_SetLocator(chr, "rld", post, -1.0);
	}
	else
	{
		SendMessage(chr, "lsl", MSG_CHARACTER_EX_MSG, "LockFightMode", false);
		LAi_tmpl_stay_InitTemplate(chr);
	}
	LAi_SetFightMode(chr, false);
}

#event_handler("LAi_event_boarding_EnableReload", "LAi_EnableReload");
void LAi_EnableReload()
{
	if (boardM == 1)
	{
		ResetSoundScheme(); //надо гасить только Scheme, иначе партиклы звука на кострах, факелах, и прочем - не пашут
		SetSoundScheme("deck"); // обычная схема
		SetMusic("music_bitva");
		boardM = -1;
	}
	Surrendered = CheckForSurrender(GetMainCharacter(), boarding_enemy, 2); // проверка сдачи в плен, перед каютой 2 - второй учет метода

	if (csmCA(pchar, "CSM.LootCollector.Enable") && loadedLocation.type != "boarding_cabine")
	{
		pchar.CSM.LootCollector.CanBeRun = true;
		csmLootCollector();
	}
	PostEvent("csmEvent_RefreshReload", 100);
    //#20171218-01 Re-enable fast action for cabin enter during boarding
	if (loadedLocation.type != "boarding_cabine") BattleInterface.LAi_ActivateReload = true;
	SetEventHandler("Control Activation","LAi_ActivateReload",1);
	//Уберём саблю
	ref mchr = GetMainCharacter();
	SendMessage(&mchr, "lsl", MSG_CHARACTER_EX_MSG, "ChangeFightMode", 0);

	// sd -->
	Log_TestInfo("Start boarding_enemy_crew_start: " + boarding_enemy_crew_start + " boarding_enemy_crew: " + boarding_enemy_crew);
	Log_TestInfo("Start boarding_player_crew_start: " + boarding_player_crew_start + " boarding_player_crew: " + boarding_player_crew);
	// sd <--

	// LEO: Сначала занимаем штатные места команды, затем выбираем свободные места для офицеров.
	LAi_type_huberstay_set_Boarding(true);
	object reservedPosts;
	for(int i = 0; i < LAi_numloginedcharacters; i++)
	{
		int index = LAi_loginedcharacters[i];
		if(index >= 0)
		{
			if(index != GetMainCharacterIndex())
			{
				ref chr = &Characters[index];
				if(!LAi_IsDead(chr) && !CheckAttribute(chr,"AboardFantomMush")) // boal && sd не нужны офицеры для матросов
				{
					if(chr.chr_ai.group == LAI_GROUP_PLAYER)
					{
						boarding_player_crew = boarding_player_crew + 1;
						if (IsFort && CheckAttribute(pchar, "FortLanding"))
						{
							if (CheckAttribute(chr, "BoardEscadra")) boarding_escadra_crew++;
							if (CheckAttribute(chr, "BoardQuestTrooper")) boarding_quest_crew++;
						}
						if (!IsOfficer(chr))
						{
							LAi_SetHuberStayTypeNoGroup(chr);
							if (chr.location.group == "rld" && chr.location.locator != "")
							{
								string crewPost = chr.location.locator;
								reservedPosts.(crewPost) = true;
							}
						}
					}
				}
			}
		}
	}
	for(i = 0; i < LAi_numloginedcharacters; i++)
	{
		index = LAi_loginedcharacters[i];
		if(index >= 0)
		{
			if(index != GetMainCharacterIndex())
			{
				chr = &Characters[index];
				if (LAi_IsDead(chr)) continue;
				if (CheckAttribute(chr, "AboardFantomMush"))
				{
					LAi_SetHuberStayTypeNoGroup(chr);
				}
				else if (chr.chr_ai.group == LAI_GROUP_PLAYER && chr.chr_ai.type == LAI_TYPE_OFFICER && IsOfficer(chr))
				{
					Boarding_DisperseOfficer(chr, &reservedPosts);
				}
			}
		}
	}
	// Jason: перебираем классы кораблей и определяем следующую палубу
	int eclass = sti(GetCharacterShipClass(boarding_enemy)); // класс корабля противника
	if (CheckAttribute(&Locations[boarding_location], "UpDeckType"))
	{
		switch (eclass)
		{
			case 1: Locations[boarding_location].boarding.nextdeck = "Boarding_GunDeck"; break;
			case 2: Locations[boarding_location].boarding.nextdeck = "Boarding_GunDeck"; break;
			case 3: Locations[boarding_location].boarding.nextdeck = "Boarding_Campus"; break;
			case 4: Locations[boarding_location].boarding.nextdeck = "Boarding_Campus"; break;
			case 5: Locations[boarding_location].boarding.nextdeck = "Boarding_Cargohold"; break;
			case 6: Locations[boarding_location].boarding.nextdeck = ""; break;
			case 7: Locations[boarding_location].boarding.nextdeck = ""; break;
		}
	}
	if (IsFort && Locations[boarding_location].boarding.nextdeck == "Boarding_bastion")
	{
		Locations[boarding_location].boarding.nextdeck = "Boarding_bastion2";
	}
	// END MOD Code by Stone-D : 01/08/2003 -->
	Log_TestInfo("New boarding_player_crew: " + boarding_player_crew);
	Log_TestInfo("Next Location: " + Locations[boarding_location].boarding.nextdeck);
    Log_TestInfo("Enemy ship class: " + eclass);
	// END MOD Code by Stone-D : 01/08/2003 <--
}

//Активация перегрузки на следующую палубу
void LAi_ActivateReload()
{
	if(loadedLocation.type == "boarding_cabine")
	{		
		float locx, locy, locz;
		GetCharacterPos(pchar, &locx, &locy, &locz);
		string tmp_loc = LAi_FindNearestFreeLocator("reload", locx, locy, locz);
		if(tmp_loc != "" && !CheckCurLocator("reload", tmp_loc, locx, locy, locz)) return;
	}
	string controlName = "";
	controlName = GetEventData();
	if(controlName != "ChrAction") return;
	if (CheckAttribute(pchar, "GenQuest.CannotReloadBoarding")) return; // Jason
	DelEventHandler("Control Activation", "LAi_ActivateReload");
	//#20171218-01 Re-enable fast action for cabin enter during boarding
	DeleteAttribute(&BattleInterface, "LAi_ActivateReload");
	Log_SetActiveAction("Nothing");
	LAi_ReloadBoarding();
}

//Расставить персонажей для боя

void LAi_SetBoardingActors(string locID)
{
    int    limit, i, iQty;
	ref    chr;
	string model, ani, sTemp;
	int    xhp;
	int    locIndex = FindLocation(locID);
	int    mclass = GetCharacterShipClass(GetMainCharacter());
	int    eclass = GetCharacterShipClass(boarding_enemy);
	ref    mchr = GetMainCharacter(); // boal star with new loc always
    int    locMChar;

	limit = MAX_GROUP_SIZE;
	// локаторов разное число на моделях :( Переделал все на инфу из кода boal 01.01.05  +1 всегда для ГГ
	chr = &Locations[locIndex];

	if(CheckAttribute(chr, "boarding.locatorNum")) limit = sti(chr.boarding.locatorNum);

	// Бонусы считаем по исходной численности с долей выживших, не по сжатому пулу моделей.
	if (boarding_player_crew_start > 0 && boarding_enemy_crew_start > 0)
	{
		float assaultCrew = boarding_player_base_crew;
		if (IsFort && CheckAttribute(mchr, "FortLanding.Support")) assaultCrew += stf(mchr.FortLanding.Support);
		Boarding_ApplyStrengthBonuses(
			makeint(assaultCrew * boarding_player_crew / boarding_player_crew_start),
			makeint(makefloat(boarding_enemy_base_crew)  * boarding_enemy_crew  / boarding_enemy_crew_start));
	}

	Log_TestInfo("Location: " + locID + " Limit: " + limit);
	Log_TestInfo("Расклад на локации: наших " + boarding_player_crew + " врагов " + boarding_enemy_crew +
	             " -> ХП наших +" + boarding_player_hp + " врагов +" + boarding_enemy_hp +
	             ", уровень наших +" + m_rank_bonus + " врагов +" + e_rank_bonus);
	Log_TestInfo("Player: " + boarding_player_crew + GetConvertStrWithReplace("Variable_LAi_boarding_18", "Logs.txt", "#space#", " ") + boarding_player_crew_per_chr + " Enemy: " + boarding_enemy_crew + GetConvertStrWithReplace("Variable_LAi_boarding_19", "Logs.txt", "#space#", " ") + boarding_enemy_crew_per_chr);

	//Установим союзников из команды
    string sLocType = "loc";
	// определение стороны на палубе
	if (CheckAttribute(&Locations[locIndex], "UpDeckType"))
	{
	    sLocType = ChooseShipUpDeck(mchr, boarding_enemy);
	}
	int ireserve = 0;
	for(i = 1; i <=MAX_NUM_FIGHTERS; i++) {
		if (GetOfficersIndex(mchr, i) >= 0){ireserve++;}
	}
	ireserve = boarding_player_crew + ireserve - limit;
	if(ireserve > 0){
		// LEO: TODO: два офицера появляются сверх предела, причина пока не выяснена.
		Log_Info(GetConvertStrWithReplace("Variable_LAi_boarding_reserve", "Logs.txt", "#count#", "" + ireserve));
	}
	if (!CheckAttribute(&Locations[locIndex], "CabinType"))
	{ // не грузим матросов в каюту
		int questAtStart = boarding_quest_crew;
		int regularModels = boarding_player_crew - boarding_escadra_crew;
		if (regularModels < questAtStart) regularModels = questAtStart;
		int regularSpawned = 0;
		for(i = LAi_numloginedcharacters; i < limit; i++)
		{
			if(boarding_player_crew <= 0) break;
			model = LAi_GetBoardingModel(mchr, &ani);
			boarding_escadra_flag = 0;
			// LEO: первые boarding_escadra_crew бойцов на штурме форта — солдаты нации-эскадры (модель sold_<nat>_N), визуально отличимы от команды ГГ
			if (IsFort && boarding_escadra_crew > 0)
			{
				if (boarding_escadra_nation == PIRATE) model = "Pirate_" + (rand(9) + 1);
				else model = "sold_" + NationShortName(boarding_escadra_nation) + "_" + (rand(7) + 1);
				boarding_escadra_crew = boarding_escadra_crew - 1;
				boarding_escadra_flag = 1;
			}
			chr = LAi_CreateFantomCharacterEx(model, ani, "rld", sLocType + i);
			if (boarding_escadra_flag == 1) chr.BoardEscadra = 1; // LEO: пометка бойца эскадры — его гибель не списывается с команды ГГ
			else DeleteAttribute(chr, "BoardEscadra");
			DeleteAttribute(chr, "BoardQuestTrooper");
			if (IsFort && boarding_escadra_flag == 0)
			{
				if (Landing_IsSupportModel(regularSpawned, regularModels, questAtStart))
				{
					chr.BoardQuestTrooper = true;
					boarding_quest_crew--;
				}
				regularSpawned++;
			}
			// boal star with new loc always  <--
			chr.sex = SetGenderFromCap(mchr);

			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

			boarding_player_crew = boarding_player_crew - 1;
			if (!IsFort)
			{
	            SetFantomParamAbordOur(chr);
			}
			else
			{
			    SetFantomParamFortOur(chr);
			}
			SetNewModelToChar(chr); //иначе сабли не те, что реально
			chr.AboardFantom = true;
			AddCharHP(chr, boarding_player_hp); // влияение опыта и морали в НР
		}
		//ставим своих мушкетеров -->
		bool regularMusketeers = CheckOfficersPerk(mchr, "MusketsShoot");
		bool grapeMusketeer = Boarding_HasGrapeMusketeer(mchr, locIndex);
		if (regularMusketeers || grapeMusketeer)
		{
			iQty = MAX_MUSKETEER_SIZE;
			for(i=1; i<=iQty; i++)
			{
				if (LAi_CheckLocatorFree("rld", sLocType+"mush"+i))
				{
					chr = LAi_CreateBoardingMusketeer(mchr);
					SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*BRD_MUSKETEER_RANK_MOD_MULT, true);
					if (grapeMusketeer) Boarding_EquipGrapeMusketeer(chr);
					grapeMusketeer = false;
					chr.id = "GenChar_" + chr.index;
					chr.AboardFantom = true;
					chr.AboardFantomMush = true;
					chr.norun = true;
					LAi_SetWarriorType(chr);
					LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
					ChangeCharacterAddressGroup(chr, locID, "rld", sLocType+"mush"+i);
				}
				else continue;
				if (!IsFort)
				{
					SetMushketerParamAbordOur(chr);
				}
				else
				{
					SetMushketerParamFortOur(chr);
				}
				AddCharHP(chr, boarding_player_hp); // влияение опыта и морали в НР
				if (!regularMusketeers) break;
			}
		}
		//<-- ставим своих мушкетеров
	}
	//Установим врагов
	if (sLocType == "loc")
	{
	    sLocType = "aloc";
	}
	else
	{
	    sLocType = "loc";
	}
	trace("sLocType = " + sLocType);
	if (HasSubStr(boarding_enemy.model,"trader_")) boarding_enemy.Ship.Mode = "Trade";
	// LEO: сбор абордажников ГГ для офицеров-контрников убран (контрники отключены)
	for(i = 0; i < limit; i++) // <= тк loc0 .. loc4 = 5
	{
		if(boarding_enemy_crew <= 0) break;
		model = LAi_GetBoardingModel(boarding_enemy, &ani);
		if (i == 0 && CheckAttribute(&Locations[locIndex], "boarding.Loc.Capt")) //локатор каюты
		{
		    chr = LAi_CreateFantomCharacterEx(model, ani, "rld", Locations[locIndex].boarding.Loc.Capt);
	   		//оно трет банки. скопировано с LAi_equip > LAi_NPC_Equip
			//х0 на матросе, х1 на капитане, х2 на адмирале
			float fChrHpBottled = stf(chr.chr_ai.hp_max) * (MOD_SKILL_ENEMY_RATE - 1);
			//ручной_множитель * основное_значение / хп_в_банке, округляем ВНИЗ
			float fTemp = ((BRD_CAPTAIN_POTION1_BASE + BRD_CAPTAIN_POTION1_RAND * Random()) * fChrHpBottled / BRD_CAPTAIN_POTION1_DIV);
			TakeNItems(chr,"potion1", func_min(MakeInt(fTemp), BRD_CAPTAIN_POTION1_CAP * MOD_SKILL_ENEMY_RATE));
			//максимум банок в НПС - капитан 8, адмирал 12
			fTemp = ((BRD_CAPTAIN_POTION2_BASE + BRD_CAPTAIN_POTION2_RAND * Random()) * fChrHpBottled / BRD_CAPTAIN_POTION2_DIV);
			TakeNItems(chr,"potion2", func_min(MakeInt(fTemp), BRD_CAPTAIN_POTION2_CAP * (MOD_SKILL_ENEMY_RATE - 1)));
			//максимум эликов в НПС - капитан 3, адмирал 6
		}
		else
		{
			chr = LAi_CreateFantomCharacterEx(model, ani, "rld", sLocType + i);
		}
		chr.sex = SetGenderFromCap(boarding_enemy);

		LAi_group_MoveCharacter(chr, LAI_GROUP_BRDENEMY);

		boarding_enemy_crew = boarding_enemy_crew - 1;
		// boal расчет как у нормальных фантомов -->

		if (!IsFort)
		{
            SetFantomParamAbordEnemy(chr);
		}
		else
		{
            SetFantomParamFortEnemy(chr);
		}
		// капитана в рубку!!! boal
		if (i == 0 && CheckAttribute(&Locations[locIndex], "CabinType"))
		{
			ChangeAttributesFromCharacter(chr, boarding_enemy, true);
			chr.CaptanId = boarding_enemy.id; // иначе у фантома свой ИД   // to_do поправить опечатку
			boarding_enemy.CaptanId = boarding_enemy.id;
			chr.SuperShooter = true; // супер стрелок (вероятность пальбы выше, от сложности, если ещё и пистоль есть)
			if (boarding_enemy.sex == "man") chr.greeting = "CapSinkShip";
			SetCharacterPerk(chr, "Energaiser"); // скрытый перк даёт 1.5 к приросту энергии, дается ГГ и боссам уровней
			if (CheckAttribute(chr,"Situation"))
			{
				if(bSeaCanGenerateShipSituation) SetQuestAboardCabinDialogSituation(chr);
				else DeleteAttribute(chr,"Situation");
			}
			else
			{
				CaptainComission_GenerateSituation(chr);
				SetQuestAboardCabinDialog(chr); /// проверка на минНр
			}
			// если это место отработало, то кэп либо убит, либо по квестам сдается в плен
			// 1.2.3 эффект кирасы
		    xhp = makeint((MOD_SKILL_ENEMY_RATE*3*2+sti(chr.rank))/BRD_CUIRASS_DIV);

		    if (xhp > 0)
		    {
		        if (xhp >= BRD_CUIRASS_CAP) xhp = BRD_CUIRASS_CAP;
		        // путаница с 3  и 4 кирасами (меняем)
		        if (xhp == 3)
				{
					xhp = 4;
				}
				else
				{
				    if (xhp == 4) xhp = 3;
				}
				model = "cirass" + xhp;
				chr.cirassId  = Items_FindItemIdx(model);
				if (!checkattribute(chr,"heromodel")) 
				{
					DeleteAttribute(chr,"VISUAL_CIRASS");
					FaceMaker(chr);
				}
				CheckForCirass(chr);
				Log_TestInfo(GetConvertStrWithReplace("Variable_LAi_boarding_20", "Logs.txt", "#space#", " ") + model);
		    }
			if(CheckAttribute(pchar, "CabinHelp") && pchar.CabinHelp == true)	BSHangover_FlintFight_3("");
			if(CheckAttribute(pchar, "MaryCabinHelp") && pchar.MaryCabinHelp == true)	Mary_Pomogaet_v_Kautah("");
			if(sti(InterfaceStates.CharCap))
			{
				NPCShowName(sti(InterfaceStates.CharCapShowOff), sti(InterfaceStates.CharCapShowOthers), sti(InterfaceStates.CharCapTargetOfficers));
			}
		}
		SetNewModelToChar(chr); //иначе сабли не те, что реально
		string weaponID = GetCharacterEquipByGroup(chr, BLADE_ITEM_TYPE);
		aref weapon;
		Items_FindItem(weaponID, &weapon);
		chr.chr_ai.fencingtype = weapon.FencingType;
		weaponID = GetCharacterEquipByGroup(chr, GUN_ITEM_TYPE);
		if (weaponID != "")
		{
			EquipCharacterByItem(chr,weaponID);
			Items_FindItem(weaponID, &weapon);
			chr.chr_ai.charge = makefloat(weapon.chargeQ);
		}
		chr.AboardFantom = true;
		if (!CheckAttribute(&Locations[locIndex], "CabinType")) AddCharHP(chr, boarding_enemy_hp); // влияение опыта и морали в НР
		if (CheckAttribute(boarding_enemy, "id") && boarding_enemy.id == "pirateVikingQuest_Captain" && rand(4) < 2) chr.Frenzy = true;
		// LEO: офицеры-контрники со звёздами убраны
	}
	//ставим вражеских мушкетеров -->
	bool regularEnemyMusketeers = CheckCharacterPerk(boarding_enemy, "MusketsShoot") || IsFort;
	bool grapeEnemyMusketeer = Boarding_HasGrapeMusketeer(boarding_enemy, locIndex);
	if (regularEnemyMusketeers || grapeEnemyMusketeer)
	{
		if (!CheckAttribute(&Locations[locIndex], "CabinType"))
		{
			iQty = MAX_MUSKETEER_SIZE;
			for(i=1; i<=iQty; i++)
			{
				if (LAi_CheckLocatorFree("rld", sLocType+"mush"+i))
				{
					chr = LAi_CreateBoardingMusketeer(boarding_enemy);
					SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*BRD_MUSKETEER_RANK_MOD_MULT, true);
					if (grapeEnemyMusketeer)
					{
						Boarding_EquipGrapeMusketeer(chr);
						chr.AboardFantomMush = true;
						grapeEnemyMusketeer = false;
					}
					chr.id = "GenChar_" + chr.index;
					chr.AboardFantom = true;
					chr.norun = true;
					LAi_SetWarriorType(chr);
					LAi_group_MoveCharacter(chr, LAI_GROUP_BRDENEMY);
					ChangeCharacterAddressGroup(chr, locID, "rld", sLocType+"mush"+i);
				}
				else continue;
				if (!IsFort)
				{
					SetMushketerParamAbordEnemy(chr);
				}
				else
				{
					SetMushketerParamFortEnemy(chr);
				}
				AddCharHP(chr, boarding_enemy_hp); // влияение опыта и морали в НР
				if (!regularEnemyMusketeers) break;
			}
		}
	}
	//<-- ставим вражеских мушкетеров
	Boarding_SpawnPGGCabinGuards(locIndex);   // LEO: личная охрана конкурента в его каюте
	//Заставим драться эти 2 группы
	LAi_group_FightGroupsEx(LAI_GROUP_PLAYER, LAI_GROUP_BRDENEMY, true, GetMainCharacterIndex(), -1, false, false);
	LAi_group_SetCheckEvent(LAI_GROUP_BRDENEMY);
}

// LEO: ОХРАНА КАЮТЫ КОНКУРЕНТА. Разбогатевший ПГГ встречает игрока не один: на каждый полный миллион в
// кошельке в каюте стоит телохранитель (максимум PGG_CABIN_GUARDS_MAX). Деньги при этом не тратятся -
// достаток лишь показывает, что охрану есть на что держать.
// Бойцы заметно слабее самого конкурента: их дело - связать игрока боем, а не заменить хозяина.
// Ставим их ОТДЕЛЬНО от общего цикла и НЕ списываем с boarding_enemy_crew: к каюте команда обычно уже
// перебита, а гвардия существует независимо от неё.
void Boarding_SpawnPGGCabinGuards(int locIndex)
{
	int i, iQty, iRank, iDone;
	float fHP;
	ref chr, pgg;
	aref rBlade;
	string model, ani, sLoc, sCaptLoc, sBlade;

	if (!CheckAttribute(&Locations[locIndex], "CabinType")) return;
	if (!CheckAttribute(boarding_enemy, "id")) return;
	if (findsubstr(boarding_enemy.id, "PsHero_", 0) == -1) return;   // каюта не конкурента

	pgg = CharacterFromID(boarding_enemy.id);
	iQty = PGG_GetCabinGuards(pgg);
	if (iQty < 1) return;

	sCaptLoc = "aloc2";
	if (CheckAttribute(&Locations[locIndex], "boarding.Loc.Capt")) sCaptLoc = Locations[locIndex].boarding.Loc.Capt;

	iRank = sti(pgg.rank) - 2;
	if (iRank < 1) iRank = 1;

	fHP = LAi_GetCharacterMaxHP(pgg) * 0.8;
	if (fHP < 1.0) fHP = 1.0;
	iDone = 0;
	for (i = 0; i <= 6; i++)
	{
		if (iDone >= iQty) break;
		sLoc = "aloc" + i;
		if (sLoc == sCaptLoc) continue;                  // это место хозяина каюты
		if (!LAi_CheckLocatorFree("rld", sLoc)) continue;

		// ⚠️ модель - ОБЫЧНЫЙ НАЁМНЫЙ ОФИЦЕР, тот же набор, из которого офицеров берёт себе
		//   игрок. НЕ матросская модель (это личная гвардия, а не поднятые из трюма) и НЕ мундир нации:
		//   конкурент нанимает вольных людей, а не солдат своего флага.
		//   Диапазон 1..64 - весь набор officer_N.gm (сверено с RESOURCE/MODELS/Characters, пропусков нет).
		//   Для сравнения: офицеры Флинта в финале «Чёрных парусов» берут rand(62)+2, то есть 2..64.
		model = "officer_" + (rand(63) + 1);
		ani = "man";
		// имя, лицо и пол ставит сам LAi_CreateFantomCharacterEx, а SetFantomParamFromRank их не трогает
		chr = LAi_CreateFantomCharacterEx(model, ani, "rld", sLoc);
		// ⚠️ ранг ставим ЯВНО и НЕ зовём SetFantomParamAbordEnemy: та пересчитывает бойца по «рангу врага
		//   абордажа» (от силы ГГ), из-за чего охрана выходила 83-89 уровня при 31-м у самого хозяина.
		chr.rank = iRank;
		SetFantomParamFromRank(chr, iRank, true);
		// чуть слабее хозяина: фехтование и запас здоровья урезаны, боевые перки от ранга остаются
		chr.skill.FencingLight = makeint(makefloat(sti(pgg.skill.FencingLight)) * 0.85);
		chr.skill.Fencing      = makeint(makefloat(sti(pgg.skill.Fencing))      * 0.85);
		chr.skill.FencingHeavy = makeint(makefloat(sti(pgg.skill.FencingHeavy)) * 0.85);
		LAi_SetHP(chr, fHP, fHP);
		SetNewModelToChar(chr);
		// ⚠️ БЕЗ ЭТОГО БОЕЦ ПРОСТО СТОИТ: боевой ИИ берёт манеру боя из chr_ai.fencingtype, а его
		//   выставляет не экипировка, а вот этот разбор надетого клинка (так же делает основной цикл).
		sBlade = GetCharacterEquipByGroup(chr, BLADE_ITEM_TYPE);
		if (sBlade != "")
		{
			Items_FindItem(sBlade, &rBlade);
			chr.chr_ai.fencingtype = rBlade.FencingType;
		}
		sBlade = GetCharacterEquipByGroup(chr, GUN_ITEM_TYPE);
		if (sBlade != "")
		{
			EquipCharacterByItem(chr, sBlade);
			Items_FindItem(sBlade, &rBlade);
			chr.chr_ai.charge = makefloat(rBlade.chargeQ);
		}
		chr.AboardFantom = true;
		// ⚠️ ПОРЯДОК: LAi_SetWarriorType внутри себя переносит персонажа в группу ОХРАНЫ, затирая любую
		//   выставленную до него. Поэтому сначала тип, и только потом группа абордажных врагов - иначе
		//   боец числится охраной, ни с кем не воюет и стоит столбом, пока его самого не ударят.
		//   Тот же порядок в цикле вражеских мушкетёров выше.
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_BRDENEMY);
		iDone++;
	}
	if (iDone < 1) return;
	// ⚠️ ПОДПИСИ НАД ГОЛОВОЙ расставляет NPCShowName, а зовётся он ИЗ CreateCharacter - то есть в момент
	//   создания очередного персонажа, по всем, кто уже стоит рядом. Для ПОСЛЕДНЕГО созданного бойца
	//   этого прохода уже никто не сделает, поэтому именно он всегда оставался без надписи. Прогоняем
	//   ещё раз, когда вся охрана на местах.
	if (sti(InterfaceStates.CharCap))
	{
		NPCShowName(sti(InterfaceStates.CharCapShowOff), sti(InterfaceStates.CharCapShowOthers), sti(InterfaceStates.CharCapTargetOfficers));
	}
}

//Убийство группы
void LAi_BoardingGroupKill()
{
	string group = GetEventData();
	if(IsFort && Locations[boarding_location].id == "BOARDING_FORT")
		StopFakeFire();
	if(group != LAI_GROUP_BRDENEMY) return;
	// отключим шум абордажа (который включается при аларме в sound.c)

	if (CheckAttribute(pchar, "questTemp.Whisper.KilledPortman"))
	{
		Log_Info(GetConvertStrWithReplace("Variable_LAi_boarding_21", "Logs.txt", "#space#", " "));
		LAi_SetFightMode(PChar, false);
		PlaySound("interface\important_item.wav");
		DoQuestCheckDelay("TalkSelf_Quest", 0.8);
	}

	PostEvent("LAi_event_boarding_EnableReload", 1000);
}

//Моделька для абордажного персонажа
string LAi_GetBoardingModel(ref rCharacter, ref ani)
{
	ani = "man";
	int iNation = sti(rCharacter.nation);
	string atr;
	string model;


	if (CheckAttribute(rCharacter, "GenQuest.CrewSkelMode"))
    {
        model = GetRandSkelModel();
		ani = "man";
		return model;
    }
	if (CheckAttribute(rCharacter, "GenQuest.CrewSkelModeClassic"))
    {
        model = GetRandSkelModelClassic();
		ani = "man";
		return model;
    }
	if (CheckAttribute(rCharacter, "GenQuest.CrewSatanistMode"))
    {
        model = GetRandSatanistMode();
		ani = "man";
		return model;
    }
	// Gregg: обезьяны-нежить с «Бананового рая» («Корабль обезьян»). Модель Monkey.gm
	// ходит только под своей анимацией, "man" ей не подходит.
	if (CheckAttribute(rCharacter, "GenQuest.CrewMonkeyMode"))
    {
        model = GetRandMonkeyModel();
		ani = "monkey";
		return model;
    }
    if (CheckAttribute(rCharacter, "OZG") == true)
    {
        model = "OZG_" + (rand(8) + 1);
	    ani = "man";
		return model;
    }

	if(sti(rCharacter.index) == GetMainCharacterIndex())
	{
        // boal 290904 форма солдат -->
        if (CheckAttribute(rCharacter, "GenQuest.CrewSkelMode"))
        {
            model = GetRandSkelModel();
			ani = "man";
			return model;
        }
		if (rCharacter.sex == "skeleton")
		{
			model = GetRandSkelModelClassic();
			ani = "man";
			return model;
		}
		if (CheckAttribute(rCharacter, "OZG") == true)
        {
            model = "OZG_" + (rand(9) + 1);
			ani = "man";
			return model;
        }
        if (isMainCharacterPatented() && sti(Items[sti(rCharacter.EquipedPatentId)].TitulCur) > 1) //форма только со звания капитан
        {
            atr = "boardingModel.enemy";
            iNation = sti(Items[sti(rCharacter.EquipedPatentId)].Nation);
        }
        else
        {
            atr = "boardingModel.player";
        }
        // boal 290904 форма солдат <--
	}
	else
    {   //boal -->
		// Фикс ПГГ Роджера, когда берём его на абордаж, то у него команда - скелетоны -->
		if (rCharacter.sex == "skeleton")
        {
            model = GetRandSkelModelClassic();
            ani = "man";
            return model;
        }
		// Фикс ПГГ Роджера, когда берём его на абордаж, то у него команда - скелетоны <--
		if (CheckAttribute(rCharacter, "Ship.Mode") && rCharacter.Ship.Mode == "Trade")
		{
            atr = "boardingModel.merchant";
        }
		else
        {//boal <--
            atr = "boardingModel.enemy";
        }
	}

	if (iNation < 0) iNation = PIRATE;
	//eddy. замаскировавшися пиратов тоже надо учитывать
	if (CheckAttribute(rCharacter, "Ship.Mode") && rCharacter.Ship.Mode == "Pirate" && !IsMainCharacter(rCharacter)) iNation = PIRATE;
	Nations[iNation].boardingModel.player = "";
	Nations[iNation].boardingModel.enemy = "";
	Nations[iNation].boardingModel.merchant = "";

	aref models;
	makearef(models, Nations[iNation].(atr));
	int num = GetAttributesNum(models);
	if(num <= 0) return "pirate_1";
	int i = rand(num - 1);
	atr = GetAttributeName(GetAttributeN(models, i));
	model = models.(atr);
	atr = atr + ".ani";
	if(CheckAttribute(models, atr))
	{
		if(models.(atr) != "")
		{
			ani = models.(atr);
		}
	}
	if (GetCharacterItem(rCharacter, "SkullAztec") && rand(1)==0)
	{
		if(rand(1)==0)	model = GetRandSkelModel();
		else			model = GetRandSkelModelClassic();

		ani = "man";
		return model;
	}

	return model;
}

bool Boarding_HasGrapeMusketeer(ref captain, int locIndex)
{
	if (!IsFort && !CheckAttribute(&Locations[locIndex], "UpDeckType")) return false;
	return CheckOfficersPerk(captain, "CannonProfessional") &&
		CheckOfficersPerk(captain, "GrapplingProfessional");
}

void Boarding_EquipGrapeMusketeer(ref shooter)
{
	// LEO: Вызываем после настройки ранга, которая заново выдаёт перки.
	shooter.MushketType = "grape_mushket";
	SetCharacterPerk(shooter, "GunProfessional");
}

ref LAi_CreateBoardingMusketeer(ref captain)
{
	string model = LAi_GetBoardingMushketerModel(captain);
	string gender = SetGenderFromCap(captain);
	string animation = "man";
	if (gender == "skeleton") animation = "skeleton";
	ref shooter = GetCharacter(NPC_GenerateCharacter("GenChar_", model, gender, animation, 5, sti(captain.nation), 0, false));
	shooter.BoardingMusket = true;
	return shooter;
}

string LAi_GetBoardingMushketerModel(ref rCharacter)
{
	if (CheckAttribute(rCharacter, "GenQuest.CrewSkelMode")) return GetRandSkelModel();
	if (SetGenderFromCap(rCharacter) == "skeleton") return GetRandMushketerSkeleton();
	if (CheckAttribute(rCharacter, "GenQuest.CrewSatanistMode")) return "animists"+(1+rand(1));

	string model;
	int iNation = sti(rCharacter.nation);

	if(sti(rCharacter.index) == GetMainCharacterIndex())
	{
        if (isMainCharacterPatented() && sti(Items[sti(rCharacter.EquipedPatentId)].TitulCur) > 1) //форма только со звания капитан
        {
            iNation = sti(Items[sti(rCharacter.EquipedPatentId)].Nation);
        }
        else
        {
            iNation = PIRATE;
        }
	}
	else
    {   //boal -->
		if (CheckAttribute(rCharacter, "Ship.Mode") && rCharacter.Ship.Mode == "Trade")
		{
            iNation = PIRATE;
        }
		else
        {//boal <--
            iNation = sti(rCharacter.nation);
        }
	}

	if (iNation < 0) iNation = PIRATE;
	//eddy. замаскировавшися пиратов тоже надо учитывать
	if (CheckAttribute(rCharacter, "Ship.Mode") && rCharacter.Ship.Mode == "Pirate") iNation = PIRATE;
	if(iNation == PIRATE)
	{
		model = GetPirateMushketerModel();
	}
	else
	{
		model = NationShortName(iNation) + "_rifle_" + (rand(2)+1);
	}
	// Gregg: отдельной ветки для «Бананового рая» тут нет и не нужно - у обезьяны нет
	// ни модели с мушкетом, ни мушкетёрской анимации, а Маугли по полу скелет, так что
	// стрелков ему выдаст ветка выше. Команда всё равно вся нежить.
	return model;
}

string SetGenderFromCap(ref character)
{
	// Gregg: команду «Бананового рая» намеренно не выделяем в отдельный пол - у Маугли
	// sex "skeleton", и его обезьяны наследуют его же. Пол здесь ни на модель, ни на ИИ
	// не влияет (обработчики звуков по полу давно закомментированы), зато по "skeleton"
	// работает антинежитьный бонус talisman9 в LAi_fightparams - обезьяны-то мёртвые.
	if (character.sex == "skeleton") return "skeleton";
	if (CheckAttribute(character, "GenQuest.CrewSkelMode") || CheckAttribute(character, "GenQuest.CrewSkelModeClassic")) return "skeleton";
	return "man";
}

// boal 03/08/06 полностью переписал метод Stone-D от 27/07/2003
bool CheckForSurrender(ref mchr, ref echr, int _deck)
{
    if(boarding_location_type == BRDLT_FORT) return false; // Forts don't surrender.

    // LEO: корабль УЖЕ сдался в море (белый флаг по морали ИЛИ полная сдача через переговоры Speak — Ship_Surrender ставит surrendered) → при абордаже мгновенная сдача без палубного боя. Раньше флаг игнорировался и крутился вероятностный ролл, из-за чего сдавшийся корабль РЕДКО уходил в бой. Гард !DontRansackCaptain — не форсить сюжетных "не сдающихся" капитанов.
    if (CheckAttribute(echr, "surrendered") && !CheckAttribute(echr, "DontRansackCaptain")) return true;

    if (GetPrisonerQty() > PRISONER_MAX) return false; // очень много пленых

	float fEnemyRate = BRD_SURR_RATE_NORMAL - BRD_SURR_RATE_MOD_NORMAL * MOD_SKILL_ENEMY_RATE*BRD_SURR_RATE_MOD_MULT;
	if (sti(echr.Nation) == PIRATE)
	{
		fEnemyRate = BRD_SURR_RATE_PIRATE - BRD_SURR_RATE_MOD_PIRATE * MOD_SKILL_ENEMY_RATE*BRD_SURR_RATE_MOD_MULT;
	}
	else
	{
		if (CheckAttribute(echr, "Ship.Mode") && echr.Ship.Mode == "Trade") // торговцы склонны сдаться
		{
			fEnemyRate = BRD_SURR_RATE_TRADE - BRD_SURR_RATE_MOD_TRADE * MOD_SKILL_ENEMY_RATE*BRD_SURR_RATE_MOD_MULT;
		}
	}

	int eclass = GetCharacterShipClass(echr);
	int mclass = GetCharacterShipClass(mchr);

	float mcrew = stf(GetWeaponCrew(mchr, GetCrewQuantity(mchr))); // честный учет с оружием
	float ecrew = stf(GetCrewQuantity(echr));

	float fRep    = Bring2Range(0.0, 1.0, stf(REPUTATION_MIN), stf(REPUTATION_MAX), stf(mchr.reputation)); // приведение к 0..1
	float mmorale = stf(mchr.ship.crew.morale) / MORALE_MAX; // 0..1
	float emorale = stf(echr.ship.crew.morale) / MORALE_MAX; // 0..1
	float mskill  = (GetSummonSkillFromName(mchr, "Leadership") + GetSummonSkillFromName(mchr, "Grappling")) / (2.0 * SKILL_MAX_TOTAL);  // 0..1
	float eskill  = (GetCharacterSkill(echr, "Leadership") + GetCharacterSkill(echr, "Defence")) / (2.0 * SKILL_MAX_TOTAL);              // 0..1
	float mcskill = GetCrewExp(mchr, "Soldiers") / GetCrewExpRate();
	float ecskill = GetCrewExp(echr, "Soldiers") / GetCrewExpRate();

	mcrew = mcrew * (1.0 + BRD_SURR_SKILL_MULT *mskill) * (1.0 + BRD_SURR_SKILL_MULT *mcskill) * (1.0 + BRD_SURR_SKILL_MULT *mmorale) * (1.0 + BRD_SURR_DECK_MULT * _deck) * fEnemyRate * (fRep * fRep * fRep); // Если у ГГ низкая репутация - очень не хотят сдаваться
	ecrew = ecrew * (1.0 + BRD_SURR_SKILL_MULT *eskill) * (1.0 + BRD_SURR_SKILL_MULT *ecskill) * (1.0 + BRD_SURR_SKILL_MULT *emorale);

	float seaDogCoeff = 1.0;
	if (CheckOfficersPerk(mchr, "SeaDogProfessional") && CheckAttribute(pchar, "SeaDogProfessionalSwitch"))
	{
		if (pchar.SeaDogProfessionalSwitch == 1)
			seaDogCoeff = BRD_SURR_SEADOG_MULT;
	}
	mcrew = mcrew * seaDogCoeff;

	// подсчет компаньенов у сторон  -->
    int mShip = GetCompanionQuantity(mchr);
    int eShip;
    string sGroupID = Ship_GetGroupID(echr);
    if (sGroupID != "")
    {
    	eShip = Group_GetLiveCharactersNum(sGroupID);
    }
    else
    {
    	eShip = 1;
    }
    mcrew = mcrew * (1.0 + (mShip - 1) / BRD_SURR_SHIP_BONUS_DIV);
    ecrew = ecrew * (1.0 + (eShip - 1) / BRD_SURR_SHIP_BONUS_DIV);
	// <--

    Log_TestInfo("Surrender Hero = " + mcrew + "    Enemy = " + ecrew + " eShipQty = " + eShip);

    if (mcrew > ecrew)
	{
		int randval = rand(BRD_SURR_ROLL_MAX);
		Log_TestInfo("rand: " + randval);
		if (randval < BRD_SURR_ROLL_MAX * (mcrew - ecrew) / ecrew)
		{
			Log_TestInfo("Surrender!");
			return true;
		}
	}
	Log_TestInfo("Fight!");
	return false;
}

// boal 03/12/05 выбор локатора от корабля лок - маленькие, алок - большие-->
string ChooseShipUpDeck(ref _mchar, ref _enemy)
{
    string sLoc = "loc";

	if (GetCargoMaxSpace(_mchar) > GetCargoMaxSpace(_enemy))
	{
	    sLoc = "aloc";
	}

    return sLoc;
}
// boal 03/12/05 <--
// CSM -->
#event_handler("csmEvent_RefreshReload", "csmRefreshReload");
void csmRefreshReload()
{
	if (!LAi_IsBoardingProcess() || !IsEntity(&loadedLocation))
	{
		csmDA(pchar, "CSM.LootCollector.CanBeRun");
		return;
	}

	if (loadedLocation.type == "boarding_cabine" && csmCA(pchar, "CSM.LootCollector.Enable"))
		csmDA(pchar, "CSM.LootCollector.CanBeRun");
	if (loadedLocation.type == "boarding_cabine") return;

	if (g_ActiveActionName == "" || g_ActiveActionName == "Nothing")
		Log_SetActiveAction("Reload");

	PostEvent("csmEvent_RefreshReload", 250);
}
// CSM <--

// LEO: сколько защитников каюты ещё на ногах.
// ⚠️ Считаем ВСЕХ живых врагов в локации, кроме самого хозяина каюты, а не только своих подсаженных.
//    Причина: помимо личной охраны игра ставит в каюту и обычных бойцов из остатков команды (limit там
//    не единица - Boarding_ComputeMaxCrew добавляет к каюте нижние палубы). По списку «только своих»
//    такой боец не учитывался: он дрался, а хозяин уже заводил разговор.
//    Заодно это устойчиво к тому, что чекер у кого-то не сработает - счёт всегда по факту.
int Boarding_PGGCabinGuardsAlive()
{
	int i, index, iAlive, iCaptIdx;
	ref chr;
	iCaptIdx = -1;
	if (CheckAttribute(pchar, "GenQuest.QuestAboardCabinDialogIdx")) iCaptIdx = sti(pchar.GenQuest.QuestAboardCabinDialogIdx);
	iAlive = 0;
	for (i = 0; i < LAi_numloginedcharacters; i++)
	{
		index = LAi_loginedcharacters[i];
		if (index < 0) continue;
		if (index == GetMainCharacterIndex()) continue;
		if (index == iCaptIdx) continue;             // сам хозяин каюты в охрану не считается
		chr = &Characters[index];
		if (LAi_IsDead(chr)) continue;
		if (!CheckAttribute(chr, "chr_ai.group")) continue;
		if (chr.chr_ai.group != LAI_GROUP_BRDENEMY) continue;
		iAlive++;
	}
	return iAlive;
}
