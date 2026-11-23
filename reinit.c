//Boyer fix #20170301-6

#include "STORE\initGoods.c"
#include "ITEMS\initItems.c"
#include "CANNONS\Cannons_init.c"
#include "characters\Characters_tables.c"
#include "Ships\Ships_init.c"
#include "Ships\Sails_init.c"
#include "Weather\WhrInit.c"
#include "encounters\Encounters_init.c"
#include "STORE\store_init.c"
#include "nations\nations_init.c"
#include "colonies\Colonies_init.c"
#include "INTERFACE\perks\perks_init.c"
void initReInit(bool bAll)
{
Trace("Reinitializing");

	SetArraySize(&Cannon,CANNON_TYPES_QUANTITY)
	InitCannons();
	SetArraySize(&Goods,GOODS_QUANTITY);
	InitGoods();
	SetArraySize(&EncountersTypes,MAX_ENCOUNTER_TYPES);
	InitEncounters();
	SetArraySize(&SailsColors,SAILS_COLOR_QUANTITY);
	InitSailsColors();


if (bAll) {
	SetArraySize(&randItemModels,MAX_LOADED_RANDITEMS);
	SetArraySize(&RandItems, RANDITEMS_QUANTITY);
	SetArraySize(&Items,ITEMS_QUANTITY);
	InitItems();
	SetArraySize(&stores,STORE_QUANTITY);
	StoreInit();
	InitCharactersTables();
	SetArraySize(&ShipsTypes, SHIP_TYPES_QUANTITY_WITH_FORT);
	InitShips();
	SetArraySize(&Weathers,MAX_WEATHERS);
	InitWeather();
	InitNations();
	SetArraySize(&Locations,MAX_LOCATIONS);
	InitLocations();
	InitColonies();
	CreateColonyCommanders();
	SetGovenourToCity(ENGLAND, "PortRoyal");
	SetGovenourToCity(SPAIN, "Havana");
	SetGovenourToCity(FRANCE, "Tortuga");
	SetGovenourToCity(HOLLAND, "Villemstad");
	extrnInitPerks();

	ClearAllFantomShips();
	}
}