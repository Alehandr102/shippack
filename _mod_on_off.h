#define VERSION_NUMBER1       "Corsairs Ship Pack  "
#define VERSION_NUMBER2       "v2.4.8.17 (23.11.2026)"
#define VERSION_NUMBER3       ""
#define VERSION_NUMBER4       ""
#define VERSION_MAJOR       2
#define VERSION_MINOR       4
#define VERSION_PATH        8
#define VERSION_FIX        17
#define VERSION_DATE        "23.11.2026"
#define PROVIDER_TXT            "LEOPARD's team"
#define VERSION_NUM      99911  // текущий номер
// Gregg: 99911 - отказ от подсистемы миграций (UpdateGameDataOnLoad). Предметы и локации
// перенесённых квестов К:ПС заводятся только штатными инитами, а те на загрузке не зовутся:
// каталоги приходят из сейва позиционно. Сейвы прежних версий несовместимы, нужна новая игра.
#define VERSION_NUM_PRE  99911  // последний совместмый

#define VERSION_WWW     ""
#define BI_COMPARE_HEIGHT 900
#define MOD_BETTATESTMODE			"Off"  // On -DEV version; Test - TST ver; Off

// глобальная строковая переменная
string sGlobalTemp;

//Времянка типа для сдачи
int surrenderenabled = 1;
//Lipsar для альт переходов
bool bAltInfo = false;
// запрет всех случаек на суше (офицеры и клады тоже)
bool    bDisableLandEncounters = false;
// запрет жать ф2
bool    bDisableCharacterMenu  = false;
// флаг - игрок читер
bool    bPlayerIsCheater =  false;  // на перспективу, патчем
// режим супер читов
bool    bBettaTestMode = false;
// показывать quest_reaction Log
bool    bQuestLogShow  = false;
// показывать инфу по балансу
bool    bExpLogShow    = false;
// всегда сдаются капитаны - отладка
bool    TestRansackCaptain = false;
// показ локаторов
bool    bLocatorShow  = false;
// Не жрать, нет крыс
bool    bNoEatNoRats  = false;
// Живой мир на паузе: губер не переезжает от времени, нации не объявляют войну и мир, эскадры не берут город себе, город нельзя взять себе (даже с патентом)
bool    bWorldAlivePause      = true;
// пауза метро
bool bPauseContrabandMetro = false;
// лог для ПГГ
bool bPGGLogShow = false;
// флаг генерации монстров
bool bMonstersGen = false;
// флаг генерации сухопутных энкаунтеров
bool bLandEncountersGen = true;
// флаг в сундуке
bool bMainCharacterInBox = false;
// флаг в огне
bool bMainCharacterInFire = false;
// главное меню
bool bMainMenu = false;
//загрузка начала игры
bool startGameWeather = false;
//движок проверяет эту переменную, должна равной #define ENGINE_SCRIPT_VERSION в core.h
int iScriptVersion = 8675309;       //54128  --  15650

// Генератор фантомов - альтернатива
bool    bNewFantomGenerator       	= true;  // резерв "новый фехт"
bool    bRechargePistolOnLine  		= true;  // пистолеты перезаряжаются без сабли
// упрощение невозможного
bool    bNewCodeOn                	= false;  // упрощение абордажа
// нет сайвов, токо в церкви есть
bool    bHardcoreGame             	= false;
// раздел добычи
bool 	bPartitionSet 				= true;
// вес матросов
bool 	bSailorsWeight 				= true;
// новые паруса
bool 	bNewSails 					= true;
// хардкорные анимации
bool 	bHardAnimations				= true;
// Усложнённые боссы 15.12.2021
bool 	bHardBoss					= true;
// -10 корабельного порога
bool 	bHigherShipRate				= true;
// Маркировка квестовых
bool 	bQuestMark 					= false;
// -10 личного порога
bool 	bHigherSelfRate 			= true;
// Требование ранга для навигации
bool 	bRankRequirement 			= false;
// влияние сложности на переносимый вес
bool 	bDifficultyWeight 			= false;
// Упрощённое фехтование
float 	bModDamage 					= 1.0;
// Найм полубессмертного ПГГ
bool 	bHalfImmortalPGG 			= false;
// Сбор денег за швартовку
bool 	bPortPermission 			= false;
//Срез скиллов от офицеров
bool   bNoBonusSkillOff             = false;
//Взятки стражникам
bool   bBribeSoldiers               = true;
//система скрытности
int 	iStealthSystem = 1;
//заполнение энциклопедии
bool 	bFillEncyShips              = false;
//альтернативный баланс
bool 	bAltBalance                 = false;
bool 	bAltBalanceTimeSlow         = false;
bool 	bAltBalanceOffTopPerk       = false;
bool 	bAltBalanceProHits          = false;
//альтернативный баланс
bool 	bShootOnlyEnemy				= true;
//Квесты без требований к рангу
bool 	bQuestsRank					= false;

// Скейлинг интерфейса
int screenscaling = BI_COMPARE_HEIGHT;

// Lugger: Режим дополнения -->
bool bAddonMode = true;
bool bAddonLine = true;
bool bNationLines = true;
bool bEnglandOrPirates = false;
bool bDisableQuestInterface = false;
bool bDisableOfficers = false;
bool bSanJuanIsDamned = false;
// Lugger: Режим дополнения <--

///////////////////////////////////////////////////
//          complexity
//  Next Level rate
int MOD_EXP_RATE           = 20; // это база... + (сложность)
//int Level_Complexity       = 5; // by default только настройка
//  complexity of fantom skill
int MOD_SKILL_ENEMY_RATE   = 2; // 1-easy; 2-medium; 3-hard (please, do not set more 3 or less 1 it mays crash game
// LEO: боевой радиус коллизии в ближнем бою. Движок читает через MSG "SetFightRadius" (шлёт characters.c).
//   0.8..1.0 = тесно (стиль M&B), 1.5 = старое раздутое. #define (не глобалка) в сейв не пишется. Тюнинг: число + рестарт.
#define MOD_FIGHT_COLLISION_RADIUS  1.5
#define MOD_Complexity_1_DMG  3.0
// LEO: дымный след пули на суше (LaunchGunTrail, LAi_utils.c). Скрипт считает только дуло и точку,
//   где пуля закончила путь, а полоску рисует движок по MSG "ShotTrail" (location_effects.cpp).
//   Партикловая схема не нужна - движок берёт готовую текстуру порохового дыма LocEfx\sgsmoke.tga.
//   Здесь геометрия линии; вид клубов (размер, плотность, время жизни) - константы LFX_TRAIL_* в движке.
#define GUNTRAIL_ENABLE       1  // 0 = выключить след совсем
#define GUNTRAIL_MIN_DIST     1.5  // ближе этого след не ставим - его закрывает дульный дым
#define GUNTRAIL_FREE_DIST    15.0 // как далеко уходит след при стрельбе без цели (ведём по развороту стрелка)
// LEO: попадание пули в кирасу - искры по броне, а при рикошете ещё и дымный отскок пули в сторону.
#define CIRASS_SPARK_FWD      0.35 // сдвиг искр навстречу стрелку, на переднюю часть кирасы
#define CIRASS_SPARK_Y        1.15 // высота искр над ногами цели (грудь)
#define GUNTRAIL_RICO_DIST    5.0  // как далеко улетает срикошетившая пуля
#define GUNTRAIL_RICO_ANGMIN  1.1  // минимальный отворот отскока от линии выстрела, радианы
#define GUNTRAIL_RICO_ANGADD  0.9  // случайная добавка к отвороту
#define GUNTRAIL_RICO_UP      0.7  // насколько отскок задирается вверх
// LEO: картечь и ежовые снаряды - не пуля, а горсть дроби: рисуем веер следов вместо одного.
//   Шаг клубов у них пореже одиночной пули: следов много, иначе залп забьёт буфер движка.
#define GUNTRAIL_BUCKSHOT_N      5    // сколько следов дробин на выстрел
#define GUNTRAIL_BUCKSHOT_SPREAD 0.10 // разброс дробин от линии выстрела, радианы
#define GUNTRAIL_BUCKSHOT_LEN    0.25 // разброс длины дробин (доля дистанции)
#define GUNTRAIL_BUCKSHOT_STEP   0.7  // шаг клубов для дробин
// LEO: залповое оружие - картечница ("Гусиная Лапка") бьёт всеми стволами, двустволки дают дуплет.
//   Каждый ствол шлёт свой заряд, поэтому следы умножаем на число стволов и держим под потолком.
#define GUNTRAIL_MAX_TRAILS      16   // потолок следов на один выстрел
#define GUNTRAIL_BARREL_SPREAD   0.35 // множитель разброса для пуль: стволы бьют почти параллельно, это не дробь
// LEO: граната и петарда - осколки из точки разрыва во все стороны, с задиром вверх.
#define GUNTRAIL_FRAG_GRENADE    12    // осколков у гранаты
#define GUNTRAIL_FRAG_PELLET     6    // осколков у петарды
#define GUNTRAIL_FRAG_DIST       10.0  // как далеко улетают осколки
#define GUNTRAIL_FRAG_STEP       0.45 // шаг клубов для осколков
#define GUNTRAIL_FRAG_UPMIN      0.2  // минимальный задир осколка вверх
#define GUNTRAIL_FRAG_UPADD      1.4  // случайная добавка к задиру
// LEO: расчленение взрывом. Движок режет меш персонажа по костям (голова/руки/ноги/торс)
//   и разбрасывает куски. 0 = выключить, иначе сила разлёта.
#define CHOP_BLAST_POWER      3.0
#define GUNTRAIL_MUZZLE_FWD   0.5  // вынос старта следа вперёд от центра стрелка
#define GUNTRAIL_MUZZLE_Y     1.25 // высота дула над ногами стрелка
#define GUNTRAIL_TARGET_Y     1.15 // высота точки попадания над ногами цели
#define GUNTRAIL_MISS_SIDE    0.7  // на сколько след уводит вбок при промахе
#define GUNTRAIL_MISS_OVER    2.0  // на сколько след проскакивает за цель при промахе
//officers count
int MOD_OFFICERS_RATE   = 3;
//количество кораблей охраны
int MOD_DEFENDERS_RATE   = 3;
// Killed body clear
#define MOD_DEAD_CLEAR           "On"
int MOD_DEAD_CLEAR_TIME = 500; // in sec. не используется, заменено на 500 в коде
string MainChAnim = "";
int chest_ammo[99];
int chest_treasure[99];
int chest_quest[99];
int chest_craftsmans[99];
int chest_ammo2[99];
int chest_treasure2[99];
int chest_quest2[99];
int chest_craftsmans2[99];

int shipspool[32];
string skillz[14];
string rand_arr[2];
string ShipsUpgrades[18] = {"SUP_TRADEARMOUR","SUP_SAIL","SUP_GUNS","SUP_HULLKRENG","SUP_CAPACITY","SUP_BIGSIDES","SUP_MASTS","SUP_HAMMOCKS","SUP_HELM","SUP_BARRICADES","SUP_PUMPS","SUP_LAFETTES","SUP_FALCONETS","SUP_FLANGES","SUP_BRANDER","SUP_COMBATARSENAL","SUP_ADDITIONALARSENAL","SUP_REINFORCEDCONSTRUCTION"};

#define CHAR_ITEMS_WEIGHT        70

#define InstantRepairRATE        65.0

// дистанция до корабля для разговора
#define DistanceToShipTalk        300.0

// boal 23.01.2004 доработка 091004 сколь угодно героев -->
int    startHeroType = 1;
// boal 23.01.2004 <--

int ItemDeadStartCount = 0; // заполняется в itemInit порог для заполения трупов (оптимизация)

#define STARTGAME_YEAR       1665
#define STARTGAME_MONTH      1
#define STARTGAME_DAY        1

// пасхалка Кукурузина
#define FUNY_SHIP_NAME    "Кукурузина"
#define KEY_ENG_LINE	  "uFRklw7Ghww19ywWQ"

int TimeScaleCounter = 0;
// запрет перехода
bool chrDisableReloadToLocation = false;
// для видения форта в АИ шипов
bool bIsFortAtIsland = false;
float fFort_x, fFort_z;
int iFortNation, iFortCommander;
//начало и конец предметов для локаторов item
int ItemsForLocators_start, ItemsForLocators_end, Totems_start, Totems_end;