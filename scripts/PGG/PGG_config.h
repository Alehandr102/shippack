// ====================================================================
//  ПГГ: общие константы (config)
//  Подключается из scripts\PGG\PsHero.c (основная компиляция).
//  Таблица #define в Storm глобальна, поэтому эти константы видны и в
//  сегменте диалога DIALOGS\<lang>\PGG\... — отдельный include там не нужен.
// ====================================================================

// --- Отношение ПГГ к ГГ (0..100, нейтрально 50) ---
#define PGG_REL_FRIENDLY              50   // порог дружелюбного общения/согласия
#define PGG_REL_HOSTILE               41   // ниже — ПГГ может начать преследовать
#define PGG_REL_HIRE_MIN              70   // минимум для найма без откупа
#define PGG_REL_COMPANION_LEAVE       5    // ниже — компаньон уходит

// --- Репутация ПГГ (0..100) ---
#define PGG_REP_LOW                   20
#define PGG_REP_MID                   30
#define PGG_REP_GOOD                  40   // граница good/bad (alignment) и не-жадности
#define PGG_REP_HIGH                  70

// --- Кулдауны Пиратского Предложения (дни) ---
#define PGG_PP_COOLDOWN_SEA           7
#define PGG_PP_COOLDOWN_TOWN          14
#define PGG_PP_OFFER_CD_BASE          25
#define PGG_PP_OFFER_CD_RAND          20
#define PGG_PP_OFFER_CHANCE           10

// --- Деньги ---
#define PGG_MONEY_CAP_BASE            100000  // кэп денег ПГГ = base + rank*perrank
#define PGG_MONEY_CAP_PER_RANK        10000
#define PGG_MONEY_FOR_SHIP            16000   // порог "хватит на кораблик"
#define PGG_MONEY_START_BASE          8000    // стартовые деньги при генерации
#define PGG_HIRE_FORCE_PRICE_PER_RANK 100000  // откуп при отказе наняться = rank*...

// --- Счётчики дней задач ---
#define PGG_CONTRA_DAYS               3
#define PGG_AFTERSAIL_TASK_LIMIT      5
#define PGG_REBIRTH_WAIT_DAYS         20
#define PGG_COMPANION_RELCHECK_DAYS   30

// --- Пополнение склада города ---
#define PGG_RESTOCK_MULT              1.4
