//Обработка ПГГ.
int PsHeroQty = 0;

#define PGG_TASK_NOTASK			"NoTask"
#define PGG_TASK_WAITINTAVERN	"WaitInTavern"
#define PGG_TASK_WORKONMAYOR	"WorkOnMayor"
#define PGG_TASK_SAILTOISLAND	"SailToIsland"
#define PGG_TASK_GOTOWAR		"GoToWar"
#define PGG_TASK_WORKCONVOY		"WorkConvoy"
#define PGG_TASK_WORKONSTORE	"WorkOnStore"
#define PGG_TASK_WORKWITHCONTRA	"WorkWithContra"
#define PGG_TASK_LOCKSERVICE	"LockService"
// LEO: ТУРНИР АРЕНЫ. Задача морская, но цель не колония: арена стоит на необитаемой Доминике, и
//   PGG_FindTargetTown её не вернёт (он перебирает только Colonies).
#define PGG_TASK_ARENA			"Arena"
#define PGG_ARENA_ISLAND		"Dominica"   // остров с ареной (для отображения цели)
// ⚠️ маршрут строится ДО ЛОКАТОРА, имя острова точкой маршрута не является. У Доминики роль порта играет
//   берег Shore27 - так же её обходит CalculateColonyDistance в colonies.c.
#define PGG_ARENA_LOCATOR		"Shore27"
// ⚠️ стоянка: в МОРСКОЙ локации корабельные точки лежат в IslandShips1/2 (ship_1..ship_6), а Reload_* -
//   это входы в акваторию. Названия групп НЕ совпадают с деревом узлов модели.
#define PGG_ARENA_BAY1			"IslandShips1"
#define PGG_ARENA_BAY2			"IslandShips2"
#define PGG_ARENA_BAY_PLACES	6
#define PGG_ARENA_MAX_FIGHTERS	7            // сколько ПГГ максимум едет на один турнир
// отдых после турнира: роспуск и новый отбор идут в одной генерации, а снятый стоит у самой Доминики -
//   без паузы его звали бы снова, и бухта не пустела бы. Цикл турнира 20-25 дней, 45 = пропуск двух.
#define PGG_ARENA_REST_DAYS		45

// LEO: ОХРАНА КАЮТЫ. Достаток конкурента = его личная гвардия при абордаже. ⚠️ Деньги НЕ тратятся,
//   миллион здесь - ПОРОГ, а не расход: разбогател - значит нанял, обеднел - охрана разошлась.
#define PGG_CABIN_GUARD_PRICE	1000000
#define PGG_CABIN_GUARDS_MAX	5            // потолок по замыслу; свободных мест в каюте 6 (aloc0..aloc6 минус место капитана)
#define PGG_WORLD_MAP_MAX_AT_SEA	10

//уменьшаю кач до 30 процентов от СВМЛ... т.к. пока не ясно чего оно так быстро качается.
#define EXP_MODIFIER  0.25

#include "scripts\PGG\PGG_config.h"
#include "scripts\PGG\PGG_init.c"
#include "scripts\PGG\PGG_update.c"
#include "scripts\PGG\PGG_tasks.c"
#include "scripts\PGG\PGG_progress.c"
#include "scripts\PGG\PGG_world.c"
#include "scripts\PGG\PGG_relations.c"
#include "scripts\PGG\PGG_rumours.c"
#include "scripts\PGG\PGG_quest_pp.c"
