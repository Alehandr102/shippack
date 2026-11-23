
#define STORE_QUANTITY	33

#define Bridgetown_STORE		0
#define SanJuan_STORE			1
#define Santiago_STORE			2
#define Villemstad_STORE		3
#define Tortuga_STORE			4
#define Marigo_STORE			5
#define PortSpein_STORE			6
#define Charles_STORE			7
#define PortoBello_STORE		8
#define SentJons_STORE			9
#define Havana_STORE			10
#define BasTer_STORE			11
#define FortFrance_STORE		12
#define LeFransua_STORE			13
#define LaVega_STORE			14
#define SantoDomingo_STORE		15
#define PortPax_STORE			16
#define Pirates_STORE			17
#define PuertoPrincipe_STORE    18
#define PortRoyal_STORE         19
#define FortOrange_STORE        20
#define Panama_STORE          	21
#define Cartahena_STORE         22
#define Maracaibo_STORE         23
#define Caracas_STORE          	24
#define Cumana_STORE          	25
#define SantaCatalina_STORE     26
#define Beliz_STORE          	27
#define Caiman_STORE		28
#define STORE_HOUSE			29
#define Nassau_STORE          	30
#define DesMoines_STORE          	31

#define SHIP_STORE 				32

#define TRADE_TYPE_NORMAL		0
#define TRADE_TYPE_EXPORT		1
#define TRADE_TYPE_IMPORT		2
#define TRADE_TYPE_CONTRABAND	3
#define TRADE_TYPE_AMMUNITION	4
#define TRADE_TYPE_CANNONS 		5
#define TRADE_TYPE_AGGRESSIVE	6

#define TRADE_TYPE_NORMAL_NAME		"normal"
#define TRADE_TYPE_EXPORT_NAME		"export"
#define TRADE_TYPE_IMPORT_NAME		"import"
#define TRADE_TYPE_CONTRABAND_NAME	"contraband"
#define TRADE_TYPE_AMMUNITION_NAME	"ammunition"
#define TRADE_TYPE_CANNONS_NAME     "cannons"
#define TRADE_TYPE_AGGRESSIVE_NAME  "aggressive"

// Gregg: базовые множители цены по типу торговли — единая точка баланса
// (используются в GetStoreGoodsPrice и GetCargoTradePrice; обратная GetStoreGoodsRndPriceModify
// исторически вычитает ДРУГИЕ константы 0.55/0.85/1.2/1.5/1.8 — не смешивать)
#define TRADE_MOD_EXPORT			0.76
#define TRADE_MOD_NORMAL			0.95
#define TRADE_MOD_IMPORT			1.14
#define TRADE_MOD_AGGRESSIVE		1.42
#define TRADE_MOD_CONTRABAND		1.8
#define TRADE_MOD_CONTRA_PIRATE		0.75	// продажа контрабанды в чужой пиратской колонии
#define TRADE_MOD_CANNONS_BUY		4.3		// множитель цены пушек при покупке (после перков)
#define TRADE_MOD_CANNONS_SELL_DIV	1.3		// делитель цены пушек при продаже

object  stores[STORE_QUANTITY];
