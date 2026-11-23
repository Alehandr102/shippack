/*
	Programm interface:

	Init world map in start
		void wdmInitWorldMap();
	Create world map object
		void wdmCreateMap();
	Release world map object
		void wdmReleaseMap();


	SPI (Script program interface):
	Получение данных от карты
	(const) поля только для прочтения
	(var) поля только для записи и прочтения, запись в них приводит к обновления остальных полей,
	      принадлежащих этой группе

	Шторма
	worldMap.storm.num - текущие количество штормов в карте (const)
	worldMap.storm.cur - индекс текущего шторма 0..num-1, надо перезаписывать для обновления полей (var)
	worldMap.storm.x - позиция шторма по x (const)
	worldMap.storm.z - позиция шторма по z (const)
	worldMap.storm.time - оставшееся время жизни в секундах (const)

	Кораблики событий
	worldMap.encounter.num - текущие количество корабликов в карте (const)
	worldMap.encounter.cur - индекс текущего кораблика в карте (var)
	worldMap.encounter.x - позиция кораблика по x (const)
	worldMap.encounter.z - позиция кораблика по z (const)
	worldMap.encounter.time - оставшееся время жизни в секундах (const)
	worldMap.encounter.type - тип события, обозначаемый этим корабликом (define в таблице типов энкоунтеров)
	worldMap.encounter.attack - индекс кораблика, который атакуем (стоя на месте)
	worldMap.encounter.id - название описания энкоунтера: worldMap.encounters.(worldMap.encounter.id)...

	Разная информация
	worldMap.info.playerInStorm - если 1 то плеер находиться в шторме
	worldMap.info.updateinfo - апдейт информации


	//Внутреняя информация - !!!Осторожно, ни каких проверок!!!
	worldMap.enemyshipViewDistMin;		//Растояние на котором корабль начинает исчезать
	worldMap.enemyshipViewDistMax;		//Растояние на котором корабль исчезает полностью
	worldMap.enemyshipDistKill;			//Расстояние на котором убиваем корабль
	worldMap.enemyshipBrnDistMin;		//Минимальное растояние на котором рожается корабль
	worldMap.enemyshipBrnDistMax;		//Максимальное растояние на котором рожается корабль

	worldMap.stormViewDistMin;			//Растояние на котором шторм начинает исчезать
	worldMap.stormViewDistMax;			//Растояние на котором шторм исчезает полностью
	worldMap.stormDistKill;				//Расстояние на котором убиваем шторм
	worldMap.stormBrnDistMin;			//Минимальное растояние на котором рожается шторм
	worldMap.stormBrnDistMax;			//Максимальное растояние на котором рожается шторм

*/

#include "worldmap\worldmap_globals.c"
#include "worldmap\worldmap_events.c"
#include "worldmap\worldmap_reload.c"
#include "worldmap\worldmap_encgen.c"
#include "worldmap\worldmap_coords.c"


//=========================================================================================


#event_handler("WorldMap_EncounterCreate", "wdmEvent_EncounterCreate");
#event_handler("WorldMap_PlayerInStorm", "wdmEvent_PlayerInStorm");
#event_handler("WorldMap_ShipEncounter", "wdmEvent_ShipEncounter");
#event_handler("WorldMap_UpdateDate", "wdmEvent_UpdateDate");
#event_handler("ExitFromWorldMap", "wdmReloadToSea");
//#event_handler("NextDay", "wdmNextDayUpdate");

#event_handler("EventTimeUpdate", "wdmTimeUpdate");

// LEO ФИКС РАЗМНОЖЕНИЯ ТИКА: ПОКОЛЕНИЕ цепочки EventTimeUpdate. Живёт только САМОЕ СВЕЖЕЕ поколение,
// все прежние цепочки гаснут на своём же тике (не перепощиваются). Событие несёт номер поколения
// (PostEvent ... "l", gen -> GetEventData()). Тик, забирая право работать, СРАЗУ поднимает поколение —
// это гасит и дубли ОДНОГО события, если обработчик оказался подписан многократно.
// Замер до фикса (compile.log 18.07): 14 входов на карту -> ~230 живых цепочек -> 46 тиков/с вместо 0.2.
int g_wdmTickGen    = 0;  // рантайм, сбрасывается при старте игры


//=========================================================================================
// Programm interface
//=========================================================================================


void wdmCreateMap(float x, float z, float ay)
{
	//Координаты острова с которого уплыли
	float zeroX = MakeFloat(worldMap.zeroX);
	float zeroZ = MakeFloat(worldMap.zeroZ);
	float ShipX, ShipZ;
	//Находим новыве координаты на карте
	int scale = WDM_MAP_TO_SEA_SCALE;
	if (worldMap.island == "Cuba1" || worldMap.island == "Cuba2" || worldMap.island == "Beliz" || worldMap.island == "SantaCatalina"
		|| worldMap.island == "PortoBello" || worldMap.island == "Cartahena" || worldMap.island == "Maracaibo"
		|| worldMap.island == "Caracas" || worldMap.island == "Cumana")
	{
		scale = 25;
	}
	shipX = (x/scale) + zeroX;
	shipZ = (z/scale) + zeroZ;
	worldMap.playerShipX = (x/scale) + zeroX;
	worldMap.playerShipZ = (z/scale) + zeroZ;
	worldMap.playerShipAY = ay;
	//Загружаем карту
	wdmCreateWorldMap();
}

// LEO: Акватория = ближайший к игроку остров. Его локализованное имя пишем в coord-текст HUD глобалки
//   (движок рисует worldMap.coordinate на плашке coord.tga.tx). Обновляется раз в 5с из wdmTimeUpdate.
void wdmUpdateAquatoria()
{
	if (!IsEntity(worldMap)) return;
	float px = stf(worldMap.playerShipX);
	float pz = stf(worldMap.playerShipZ);
	float bestDist = 1000000000000.0;
	string bestId = "";
	ref rIsl;
	string id;
	float ix, iz, d;
	int i;
	for (i = 0; i <= iNumIslands; i++)
	{
		if (Islands[i].visible == false) continue;
		rIsl = GetIslandByIndex(i);
		id = rIsl.id;
		if (!CheckAttribute(worldMap, "islands." + id + ".position.x")) continue;
		ix = stf(worldMap.islands.(id).position.x);
		iz = stf(worldMap.islands.(id).position.z);
		d = GetDistance2DRel(px, pz, ix, iz);
		if (d < bestDist)
		{
			bestDist = d;
			bestId = id;
		}
	}
	if (bestId == "") return;
	if (CheckAttribute(worldMap, "aquatoriaId"))
	{
		if (worldMap.aquatoriaId == bestId) return; // акватория не сменилась — не спамим сообщением
	}
	worldMap.aquatoriaId = bestId;
	// LEO: имя акватории — как в механике доплытия: "возле <острова в род. падеже>" (near + <id>Gen)
	SendMessage(&worldMap, "ls", MSG_WORLDMAP_SET_COORDINATES, XI_ConvertString("near") + XI_ConvertString(bestId + "Gen"));
}

// LEO: Ф2 — данные для отталкивания энкаунтеров от ВРАЖДЕБНЫХ им фортов (антиабуз: враги не лезут под чужой форт).
//   Кладём в worldMap: форты (позиция+нация из town-меток) + матрицу отношений наций + gen-счётчик.
//   Движок (WdmEnemyShip::FindFortForce) читает и отталкивает корабль от фортов, враждебных его нации.
//   Строится 1 раз при входе на глобалку (нации/владельцы там почти не меняются) — движок кэширует по gen.
void wdmBuildFortRepelData()
{
	if (!IsEntity(worldMap)) return;
	if (!CheckAttribute(worldMap, "labels")) return;
	if (CheckAttribute(worldMap, "wdmForts")) DeleteAttribute(&worldMap, "wdmForts");
	if (CheckAttribute(worldMap, "wdmNatRel")) DeleteAttribute(&worldMap, "wdmNatRel");
	// матрица отношений наций: n1 враждебна n2 -> ставим 1 (иначе ключа нет -> движок читает 0)
	int n1, n2;
	string sN1, sN2;
	for (n1 = 0; n1 < MAX_NATIONS; n1++)
	{
		sN1 = "" + n1;
		for (n2 = 0; n2 < MAX_NATIONS; n2++)
		{
			sN2 = "" + n2;
			if (GetNationRelation(n1, n2) == RELATION_ENEMY) worldMap.wdmNatRel.(sN1).(sN2) = 1;
		}
	}
	// LEO: пираты враждебны ВСЕМ нациям и наоборот. GetNationRelation для пар с PIRATE не всегда даёт RELATION_ENEMY
	// (пираты в игре — особый случай, везде обходятся отдельно), из-за чего пиратские энкаунтеры не отталкивались от
	// фортов наций и лезли прямо под форт. Форсируем вражду с пиратами явно.
	string sPir = "" + PIRATE;
	for (n1 = 0; n1 < MAX_NATIONS; n1++)
	{
		if (n1 == PIRATE) continue;
		sN1 = "" + n1;
		worldMap.wdmNatRel.(sN1).(sPir) = 1; // форт нации n1 отпугивает пиратский корабль
		worldMap.wdmNatRel.(sPir).(sN1) = 1; // пиратский форт отпугивает корабль нации n1
	}
	worldMap.wdmNatRelNum = MAX_NATIONS;
	// форты = town-метки: позицию движок уже посчитал (labels.(x).position), нация = иконка метки
	aref aLabels;
	makearef(aLabels, worldMap.labels);
	int cnt = GetAttributesNum(aLabels);
	int i, k, fn;
	string sK;
	aref lb;
	k = 0;
	for (i = 0; i < cnt; i++)
	{
		lb = GetAttributeN(aLabels, i);
		if (!CheckAttribute(lb, "type")) continue;
		if (lb.type != "Town") continue;
		if (!CheckAttribute(lb, "icon")) continue;
		if (!CheckAttribute(lb, "position.x")) continue;
		fn = sti(lb.icon);
		if (fn < 0) continue;
		if (fn >= MAX_NATIONS) continue;
		sK = "" + k;
		worldMap.wdmForts.(sK).x = stf(lb.position.x);
		worldMap.wdmForts.(sK).z = stf(lb.position.z);
		worldMap.wdmForts.(sK).nation = fn;
		k++;
	}
	worldMap.wdmFortsNum = k;
	// тунаблы (радиус/сила отталкивания) — правятся БЕЗ пересборки движка
	worldMap.wdmFortRadius = 120.0; // LEO: радиус действия форта (сила спадает линейно от форта к этой границе). Меньше = компактнее зона.
	worldMap.wdmFortForce = 1.0;   // LEO: величина силы У ФОРТА (на границе радиуса = 0). Вклад у форта = 1.5(вес)×1.0 = 1.5 > погони ~1.0, чтобы гонящийся за ГГ не подлез под форт. Больше = держит дальше от форта.
	// бамп поколения -> движок перечитает кэш
	int gen = 0;
	if (CheckAttribute(worldMap, "wdmFortsGen")) gen = sti(worldMap.wdmFortsGen);
	worldMap.wdmFortsGen = gen + 1;
}

void wdmTimeUpdate()
{
	// LEO ФИКС РАЗМНОЖЕНИЯ: пропускаем дальше ТОЛЬКО цепочку текущего поколения. Устаревшая (лишняя) просто
	// возвращается, НЕ перепощивая себя, — и тем самым гаснет. Затем сразу поднимаем поколение: право работать
	// «забрано», поэтому дубли этого же события (многократная подписка обработчика) тоже не пройдут.
	int wdmGen = GetEventData();
	if (wdmGen != g_wdmTickGen) return;
	g_wdmTickGen = g_wdmTickGen + 1;
	// boal -->
	// LEO: QuestsCheck и диверт ПГГ->пират РАЗВЕДЕНЫ по ЧЁТНЫМ/НЕЧЁТНЫМ тикам — каждая раз в 10с, пики НИКОГДА
	//   не совпадают (раньше QuestsCheck был каждый тик и на chase-тике складывался с ним в двойной спайк).
	//   Безопасно: на глобалке поллинг квестов критичен лишь Timer(hour) с часовым допуском; дневные Timer ловит
	//   NextDay, событийные — свои хендлеры (evntQuestsCheck/SHIP_DEAD/SEA_LOGIN/LOCATION_LOAD). ChaseFlags внутри
	//   уже гейтится PGGsAtSea>0. Первичный чек при входе на карту сохранён (wdmCreateMap->QuestsCheck).
	int wdmCT = 0;
	if (CheckAttribute(pchar, "wdmChaseTick")) wdmCT = sti(pchar.wdmChaseTick);
	if (wdmCT == 0) QuestsCheck();       // тики 0,2,4… — раз в 10с
	else PGG_UpdateChaseFlags();         // тики 1,3,5… — раз в 10с (детект+старт погони + сверка окончания стычки)
	pchar.wdmChaseTick = 1 - wdmCT;
	wdmUpdateAquatoria();    // LEO: обновить название акватории (ближайший остров) в HUD глобалки
	PostEvent("EventTimeUpdate", 5000, "l", g_wdmTickGen); // продолжаем СВОИМ (уже поднятым) поколением
	// boal <--
}

// LEO: ЖИВОЕ применение опции «Регионы на глобальной карте» (текстура моря sea/sea_regions). Движок меняет текстуру
//   на лету по AttributeChanged("seaTexture") -> sea->SetTexture. Гард IsEntity: опции открываются и из главного меню
//   (глобалки нет). Раньше опция читалась только при входе на глобалку (wdmCreateWorldMap ниже) -> не применялась на лету.
void RegionsWorldmap_ApplyLive()
{
	if (!IsEntity(worldMap)) return;
	if (sti(InterfaceStates.RegionsWorldmap) == 1) worldMap.seaTexture = "sea_regions";
	else worldMap.seaTexture = "sea";
}

// LEO: миникарта глобалки — дефолты user-настроек в InterfaceStates (персистентны в сейв-опциях; при 1-м запуске/отсутствии).
void WorldRadar_InitDefaults()
{
	if (!CheckAttribute(&InterfaceStates, "WorldRadarOn"))        InterfaceStates.WorldRadarOn = 1;
	if (!CheckAttribute(&InterfaceStates, "WorldRadarShape"))     InterfaceStates.WorldRadarShape = 0;     // 0=квадрат,1=круг
	if (!CheckAttribute(&InterfaceStates, "WorldRadarRange"))     InterfaceStates.WorldRadarRange = 1.0;   // МНОЖИТЕЛЬ зума (охват = enemyshipViewDistMax * множитель)
	if (stf(InterfaceStates.WorldRadarRange) > 3.0)              InterfaceStates.WorldRadarRange = 1.0;   // >3 = стухшее (старый абсолют ~500 / вне диапазона) -> дефолт-множитель
	if (stf(InterfaceStates.WorldRadarRange) < 0.3)             InterfaceStates.WorldRadarRange = 0.3;   // ниже минимума ползунка
	if (!CheckAttribute(&InterfaceStates, "WorldRadarSize"))      InterfaceStates.WorldRadarSize = 220.0;  // размер на экране, px
	if (!CheckAttribute(&InterfaceStates, "WorldRadarShowEnc"))   InterfaceStates.WorldRadarShowEnc = 1;
	if (!CheckAttribute(&InterfaceStates, "WorldRadarShowStorm")) InterfaceStates.WorldRadarShowStorm = 1;
	if (!CheckAttribute(&InterfaceStates, "WorldRadarShowForts")) InterfaceStates.WorldRadarShowForts = 1;
	if (!CheckAttribute(&InterfaceStates, "WorldRadarBlip"))      InterfaceStates.WorldRadarBlip = 1.5;    // размер объектов (0.2..5.0)
	if (!CheckAttribute(&InterfaceStates, "WorldRadarMapRes"))    InterfaceStates.WorldRadarMapRes = 1024; // качество RT-карты (1024/2048/4096)
}

// LEO: живое применение user-настроек миникарты (опции/клавиши) на активной глобалке — движок перечитает по minimapUpdate.
void WorldRadar_ApplyLive()
{
	if (!IsEntity(worldMap)) return;
	worldMap.minimap.enable    = sti(InterfaceStates.WorldRadarOn);
	worldMap.minimap.shape     = sti(InterfaceStates.WorldRadarShape);
	// LEO: скилл-зависимый охват — базовая дальность = enemyshipViewDistMax (Восприятие+труба, задаёт ApplayNewSkill) × множитель зума (Б/Ю)
	float wrVD = 200.0;
	if (CheckAttribute(worldMap, "enemyshipViewDistMax")) wrVD = stf(worldMap.enemyshipViewDistMax);
	worldMap.minimap.range     = wrVD * stf(InterfaceStates.WorldRadarRange);
	worldMap.minimap.radius    = stf(InterfaceStates.WorldRadarSize);
	worldMap.minimap.showEnc   = sti(InterfaceStates.WorldRadarShowEnc);
	worldMap.minimap.showStorm = sti(InterfaceStates.WorldRadarShowStorm);
	worldMap.minimap.showForts = sti(InterfaceStates.WorldRadarShowForts);
	worldMap.minimap.blipScale = stf(InterfaceStates.WorldRadarBlip);    // размер объектов
	worldMap.minimap.quality   = sti(InterfaceStates.WorldRadarMapRes);  // качество RT (пересборка RT в движке при смене)
	worldMap.minimapUpdate = "1"; // триггер ReadMinimapConfig в движке
	if(IsEntity(&objActivePerkShower)) InitActivePerkShower();
}

// LEO: клавиша P — вкл/выкл миникарту глобалки на лету (как радар на суше).
void WorldRadar_ToggleLive()
{
	WorldRadar_InitDefaults();
	InterfaceStates.WorldRadarOn = 1 - sti(InterfaceStates.WorldRadarOn);
	WorldRadar_ApplyLive();
}

// LEO: клавиши Б/Ю — зум миникарты глобалки: множитель поверх скилл-базы enemyshipViewDistMax. mult<1 ближе, >1 дальше.
void WorldRadar_AdjustRange(float mult)
{
	WorldRadar_InitDefaults();
	float f = stf(InterfaceStates.WorldRadarRange) * mult;
	if (f < 0.3) f = 0.3;
	if (f > 3.0) f = 3.0;
	// LEO: снап к шагу ползунка настроек 0.05 — чтобы Б/Ю на карте и слайдер «Охват» показывали одно значение
	int k = makeint((f - 0.3) / 0.05 + 0.5);
	f = 0.3 + k * 0.05;
	InterfaceStates.WorldRadarRange = f;
	WorldRadar_ApplyLive();
}

void wdmCreateWorldMap()
{
    float fHtRatio = stf(Render.screen_y) / screenscaling;
	if (sti(InterfaceStates.RegionsWorldmap) == 1) worldMap.seaTexture = "sea_regions";
	else worldMap.seaTexture = "sea";
	wdmLockReload = false;
	//Уберём все сообщения для игрока
	ClearAllLogStrings();
	//
	ReloadProgressStart();
	//Настраиваем имена месяцев
	worldMap.date.monthnames.m01 = XI_ConvertString("target_month_1");
	worldMap.date.monthnames.m02 = XI_ConvertString("target_month_2");
	worldMap.date.monthnames.m03 = XI_ConvertString("target_month_3");
	worldMap.date.monthnames.m04 = XI_ConvertString("target_month_4");
	worldMap.date.monthnames.m05 = XI_ConvertString("target_month_5");
	worldMap.date.monthnames.m06 = XI_ConvertString("target_month_6");
	worldMap.date.monthnames.m07 = XI_ConvertString("target_month_7");
	worldMap.date.monthnames.m08 = XI_ConvertString("target_month_8");
	worldMap.date.monthnames.m09 = XI_ConvertString("target_month_9");
	worldMap.date.monthnames.m10 = XI_ConvertString("target_month_10");
	worldMap.date.monthnames.m11 = XI_ConvertString("target_month_11");
	worldMap.date.monthnames.m12 = XI_ConvertString("target_month_12");
	worldMap.date.font = "normal";
	//#20200226-03



	float cx = stf(showWindow.right) - (128.0) * fHtRatio - 16.0 + 64.0;
	float cy = -40.0 + 128.0 * fHtRatio;

/*	текст из сломавшего флаги PR	https://github.com/storm-devs/storm-engine/pull/440
	worldMap.sky.texture     = "WorldMap\Interfaces\sky.tga.tx";
	worldMap.sky.maskTexture = "WorldMap\Interfaces\sky_mask.tga.tx";
	worldMap.sky.leftPos = cx - 64.0 * fHtRatio;
	worldMap.sky.topPos = cy - 64.0 * fHtRatio;
	worldMap.sky.width = 128.0 * fHtRatio;
	worldMap.sky.height = 128.0 * fHtRatio;
	worldMap.sky.color = argb(128, 255, 255, 255);

	worldMap.windPointer.texture = "WorldMap\Interfaces\wind_pointer.tga.tx";
	worldMap.windPointer.leftPos = cx - 16.0 * fHtRatio;
	worldMap.windPointer.topPos = cy - 64.0 * fHtRatio;
	worldMap.windPointer.width = 32.0 * fHtRatio;
	worldMap.windPointer.height = 128.0 * fHtRatio;
	worldMap.windPointer.color = argb(255, 255, 255, 255);

	worldMap.windBar.texture     = "WorldMap\Interfaces\bar.tga.tx";
	worldMap.windBar.maskTexture = "WorldMap\Interfaces\bar_mask.tga.tx";
	worldMap.windBar.leftPos = cx - 64.0 * fHtRatio;
	worldMap.windBar.topPos = cy;
	worldMap.windBar.width = 128.0 * fHtRatio;
	worldMap.windBar.height = 128.0 * fHtRatio;
	worldMap.windBar.color = argb(255, 255, 255, 255);

	worldMap.frame.texture = "WorldMap\Interfaces\back.tga.tx";
	worldMap.frame.leftPos = cx - 64.0 * fHtRatio;
	worldMap.frame.topPos = cy - 128.0 * fHtRatio;
	worldMap.frame.width = 128.0 * fHtRatio;
	worldMap.frame.height = 256.0 * fHtRatio;
	worldMap.frame.color = argb(255, 255, 255, 255);

	worldMap.dateText.font = "normal";
	worldMap.dateText.scale = fHtRatio;
	worldMap.dateText.color = argb(255, 255, 255, 255);
	worldMap.dateText.pos.x = cx;
	worldMap.dateText.pos.y = cy + (98.0 - 16.0 * 0.5) * fHtRatio;
*/
	// Center
	cy = cy + 128.0 * fHtRatio + 32.0;

	// LEO: HUD морали/припасов глобалки. ВСЕ параметры тут -> тюнинг БЕЗ пересборки движка.
	// LEO: иконки еды/лекарств/рома запечены в morale.tga (порядок: еда, лекарства, ром) и масштабируются вместе с блоком (width/height/leftPos/topPos).
	worldMap.morale.texture     = "WorldMap\Interfaces\morale.tga.tx"; // рамка бара + рожицы + запечённые иконки еда/лекарства/ром
	worldMap.morale.barTexture  = "WorldMap\Interfaces\morale_bar.tga.tx"; // зелёная заливка бара
	worldMap.morale.maskTexture = "WorldMap\Interfaces\morale_mask.tga.tx"; // маска заливки
	// LEO: текстура MORALE.tga теперь 256x256 (квадрат) -> блок рисуем КВАДРАТНЫМ (width==height), иначе иконки/бар исказятся по вертикали.
	worldMap.morale.width = 160.0 * fHtRatio;  // «пошире»: видимый контент ~156px (было 128)
	worldMap.morale.height = 160.0 * fHtRatio; // «повыше»: бар сверху + 3 иконки снизу (в текстуре контент y=60..195 из 256)
	worldMap.morale.leftPos = cx - 80.0 * fHtRatio; // центрируем: половина width
	worldMap.morale.topPos = cy - 64.0 * fHtRatio;  // сдвиг вверх, чтобы бар остался в верхней части HUD, иконки — ниже
	worldMap.morale.color = argb(255, 255, 255, 255);
	worldMap.morale.barScroll = 0.85; // макс мораль -> полностью зелёный, мин -> полностью красный (окно на краях зажимается в чистый цвет). Оригинал был 0.28
	// LEO: заливку бара рисуем в ТОМ ЖЕ прямоугольнике, что и рамку (как в оригинале) — текстуры bar/mask
	//      сделаны 1в1 256x256 с рамкой, поэтому отдельный прямоуг. заливки НЕ задаём (движок берёт = рамке).
	//      Движковый механизм worldMap.morale.bar{LeftPos,TopPos,Width,Height} доступен как запасной рычаг,
	//      если понадобится развести заливку и рамку без пересборки.

	// LEO: 3 числа под иконками. Порядок в текстуре: ЕДА (слева, X=0.168), РОМ (центр, X=0.498), ЛЕКАРСТВА (справа, X=0.826).
	// LEO: X-позиции = leftPos + доля*width; Y — чуть ниже иконок (иконки в текстуре до y≈195/256 -> низ блока). Всё тюнится тут без пересборки.
	worldMap.foodText.font = "normal";
	worldMap.foodText.scale = fHtRatio;
	worldMap.foodText.color = argb(255, 255, 255, 255);
	worldMap.foodText.pos.x = cx - 53.0 * fHtRatio; // еда (слева)
	worldMap.foodText.pos.y = cy + 60.0 * fHtRatio;

	worldMap.rumText.font = "normal";
	worldMap.rumText.scale = fHtRatio;
	worldMap.rumText.color = argb(255, 255, 255, 255);
	worldMap.rumText.pos.x = cx; // ром (центр)
	worldMap.rumText.pos.y = cy + 60.0 * fHtRatio;

	worldMap.medText.font = "normal";
	worldMap.medText.scale = fHtRatio;
	worldMap.medText.color = argb(255, 255, 255, 255);
	worldMap.medText.pos.x = cx + 52.0 * fHtRatio; // лекарства (справа)
	worldMap.medText.pos.y = cy + 60.0 * fHtRatio;

	worldMap.nationFlag.count = 8;
	worldMap.nationFlag.texture = "WorldMap/Interfaces/WorldMapEnsigns" + iFlagAllWdm + ".dds";
	worldMap.nationFlag.height = 48 * fHtRatio;
	worldMap.nationFlag.topPos = cy + 150 * fHtRatio; // LEO: опущен ниже — освободить место под плашку акватории (было 112)
	if (iFlagAllWdm == 2) 
		{worldMap.nationFlag.width = 96 * fHtRatio; worldMap.nationFlag.leftPos = cx - 38.4 * fHtRatio;}	//какая-то херня с длиной текстуры: размер флага 204.8*128  204.8/128=1.6	именно пишу 38.4 потому что 38.4=1.6*24
	else {worldMap.nationFlag.width = 48 * fHtRatio; worldMap.nationFlag.leftPos = cx - 24 * fHtRatio;}


	// LEO: Плашка АКВАТОРИИ (coord.tga.tx) — под числами припасов. Текст = ближайший остров (wdmUpdateAquatoria -> SET_COORDINATES).
	//   Движок рисует плашку + stCoordText (лейбл) + coordText (имя) только когда worldMap.coordinate непуст.
	//   ВСЕ позиции/размеры тут -> тюнинг БЕЗ пересборки движка.
	worldMap.coord.texture = "WorldMap\Interfaces\coord.tga.tx";
	worldMap.coord.width = 128.0 * fHtRatio;
	worldMap.coord.height = 56.0 * fHtRatio;
	worldMap.coord.leftPos = cx - 64.0 * fHtRatio;
	worldMap.coord.topPos = cy + 80.0 * fHtRatio;
	worldMap.coord.color = argb(255, 255, 255, 255);

	// LEO: лейбл-слово не нужно — на плашке само НАЗВАНИЕ акватории (coordText). stCoordText делаем пустым.
	worldMap.stCoordText.text = "";

	// название акватории («возле <острова>», как в доплытии) по центру плашки
	worldMap.coordText.font = "normal";
	worldMap.coordText.scale = fHtRatio;
	worldMap.coordText.color = argb(255, 255, 255, 255);
	worldMap.coordText.pos.x = cx;
	worldMap.coordText.pos.y = cy + 100.0 * fHtRatio;

	// LEO: сброс запомненной акватории — чтобы имя пере-отправилось движку при (пере)входе на глобалку
	if (CheckAttribute(worldMap, "aquatoriaId")) DeleteAttribute(&worldMap, "aquatoriaId");





	worldMap.resizeRatio = fHtRatio;
	//Удалим все устаревшие энкаунтеры
	wdmRemoveOldEncounters();
//	Trace("Save check ---------------================--------------")
//	Trace("Save check ###########----================--------------")
	worldMap.playerInStorm = "0";
	//Сбросим счётчики генерации энкоунтеров
	wdmReset();
	// LEO: МИНИКАРТА — конфиг ставим ЗДЕСЬ (функция входа, ПЕРЕД CreateEntity), чтобы движок читал его при КАЖДОМ входе,
	//   вкл. загрузку сейва (wdmInitWorldMap init-only не зовётся -> иначе миникарта была бы только на новой игре).
	//   User-настройки в InterfaceStates (правятся опциями + клавишами P/Б/Ю). Дефолты при 1-м запуске:
	WorldRadar_InitDefaults();
	worldMap.minimap.enable     = sti(InterfaceStates.WorldRadarOn);    // вкл/выкл (клавиша P, чекбокс опции)
	worldMap.minimap.shape      = sti(InterfaceStates.WorldRadarShape); // 0=квадрат, 1=круг (опция)
	// LEO: скилл-зависимый охват — база enemyshipViewDistMax (Восприятие+труба, ApplayNewSkill) × множитель зума (Б/Ю)
	float wrVD0 = 200.0;
	if (CheckAttribute(worldMap, "enemyshipViewDistMax")) wrVD0 = stf(worldMap.enemyshipViewDistMax);
	worldMap.minimap.range      = wrVD0 * stf(InterfaceStates.WorldRadarRange); // охват (клавиши Б/Ю = множитель, опция)
	worldMap.minimap.radius     = stf(InterfaceStates.WorldRadarSize);  // размер на экране, px (опция)
	worldMap.minimap.showEnc    = sti(InterfaceStates.WorldRadarShowEnc);   // энкаунтеры
	worldMap.minimap.showStorm  = sti(InterfaceStates.WorldRadarShowStorm); // штормы
	worldMap.minimap.showForts  = sti(InterfaceStates.WorldRadarShowForts); // форты/колонии
	worldMap.minimap.quality    = sti(InterfaceStates.WorldRadarMapRes); // качество RT (опция: 1024/2048/4096)
	worldMap.minimap.blipScale  = stf(InterfaceStates.WorldRadarBlip);   // размер объектов (опция)
	// dev-тунаблы (не в опциях)
	worldMap.minimap.margin     = 50.0;   // отступ от правого-нижнего края экрана, px
	worldMap.minimap.showClouds = 1;      // облака/туман (Фаза 2)
	// LEO: обрамление миникарты — переиспользуем текстуры наземного радара. Пути+ресайз в скрипте => тюнинг БЕЗ пересборки.
	worldMap.minimap.frameCircle = "INTERFACES\Radar_Circle.dds"; // рамка круглой карты (статична)
	worldMap.minimap.frameSquare = "INTERFACES\Radar_Qoob.dds";   // рамка квадратной карты (статична)
	worldMap.minimap.frameScale  = 1.05;  // ресайз рамки относит. радиуса (полусторона квада = радиус*scale)
	// LEO: коррекция карты (тюнинг рестартом): flipX/flipZ=ОРИЕНТАЦИЯ (зеркало края текстуры), panX/panZ=ПАНОРАМА (ход карты),
	//   flipHead=разворот стрелок. Подтв.: flipZ=1 верная ориентация, panZ=0.
	worldMap.minimap.flipX      = 0;
	worldMap.minimap.flipZ      = 1;
	worldMap.minimap.panX       = 0;
	worldMap.minimap.panZ       = 0;
	worldMap.minimap.flipHead   = 0;
	// LEO: палитра цветов блипов по нации (движок читает colNation.<номер нации как строка>). Ключ .(...) требует строковую
	//   ПЕРЕМЕННУЮ (как sN1=""+n1 в wdmBuildFortRepelData); inline .(""+ENGLAND) = «Invalid access string».
	string sNatKey;
	sNatKey = "" + ENGLAND; worldMap.minimap.colNation.(sNatKey) = argb(255, 255, 255, 255); // Англия — белый
	sNatKey = "" + FRANCE;  worldMap.minimap.colNation.(sNatKey) = argb(255, 255, 240, 140); // Франция — светло-жёлтый
	sNatKey = "" + HOLLAND; worldMap.minimap.colNation.(sNatKey) = argb(255, 150, 195, 255); // Голландия — светло-синий
	sNatKey = "" + SPAIN;   worldMap.minimap.colNation.(sNatKey) = argb(255, 205, 140, 75);  // Испания — коричнево-оранжевый
	sNatKey = "" + PIRATE;  worldMap.minimap.colNation.(sNatKey) = argb(255, 95, 95, 95);    // пираты — тёмно-серый
	// цвета категорий блипов (все в скрипте — меняются без пересборки движка)
	worldMap.minimap.colHunter  = argb(255, 255, 48, 48);   // охотник за ГГ — красный
	worldMap.minimap.colBarrel  = argb(255, 255, 220, 60);  // бочонок — жёлтый
	worldMap.minimap.colBoat    = argb(255, 255, 255, 255); // лодка — белый
	worldMap.minimap.colCompanion = argb(255, 90, 220, 90); // компаньон на задании — зелёный (свой)
	worldMap.minimap.colPlayer  = argb(255, 60, 255, 90);   // ГГ — зелёный
	worldMap.minimap.colStorm   = argb(255, 230, 45, 45);   // шторм — красный круг (форма-круг отличает от красного треугольника-охотника)
	worldMap.minimap.colNeutral = argb(255, 200, 200, 200); // без нации — серый
	//Создаём карту
	CreateEntity(&worldMap,"worldmap");
	worldMap.isLoaded = "true";
	wdmUpdateAquatoria(); // LEO: сразу посчитать/показать акваторию (не ждать первого тика wdmTimeUpdate ~1с)
	wdmBuildFortRepelData(); // LEO: Ф2 — данные отталкивания энкаунтеров от враждебных им фортов
	// LEO: Ф3 — ночь на глобалке (тон суток по часу). enable=0 отключает; intensity 0..1 — сила тона (тюнинг без пересборки).
	worldMap.dayNight.enable = 1;
	worldMap.dayNight.intensity = 0.8;
	//Обновляем параметры
	worldMap.update = "";
	//Фейдер
	CreateEntity(&wdm_fader, "fader");
	if(IsEntity(wdm_fader) == 0) Trace("Fader not created!!!");
	float fadeInTime = RELOAD_TIME_FADE_IN; // Gregg: единый IN-дефайн (было 0.5)
	SendMessage(&wdm_fader, "lfl", FADER_IN, fadeInTime, true);
	SendMessage(&wdm_fader, "ls", FADER_PICTURE0, "jonny_load\sea\sea_"+rand(3)+".dds");
	//Установим звуковую схему
	SetSchemeForMap();
	//Сообщим, что загрузились
	PostEvent("EventWorldMapInit", 830); //fix boal
	ReloadProgressEnd();
	// LEO ФИКС РАЗМНОЖЕНИЯ: новое поколение -> ВСЕ ранее запущенные цепочки погаснут на своём ближайшем тике,
	// а работать продолжит только эта. Раньше тут безусловно постилась ЕЩЁ ОДНА вечная цепочка на каждый вход.
	g_wdmTickGen = g_wdmTickGen + 1;
	PostEvent("EventTimeUpdate", 1000, "l", g_wdmTickGen);
	//Создаём накопившиеся квестовые энкоунтеры
	worldMap.addQuestEncounters = "updateQuest";
	InitWmInterface();
	SendMessage(&worldMap, "ll", MSG_WORLDMAP_SET_NATION_FLAG,  sti(pchar.nation));
	StartWDMUpdateRPC();
}

void wdmLoadSavedMap()
{
	//Загружаем карту
	wdmCreateWorldMap();
}

void wdmRemoveOldEncounters()
{
	//Пометим протухшие энкаунтеры
	wdmMarkDeleteEncounters();
	//Собираем всех желающих
	aref encs;
	makearef(encs, worldMap.encounters);
	int num = GetAttributesNum(encs);
	object forDelete;
	string attr, encID;
	int count = 0;
	for(int i = 0; i < num; i++)
	{
		aref enc = GetAttributeN(encs, i);
		if(CheckAttribute(enc, "needDelete") != 0)
		{
			attr = "del" + count;
			forDelete.(attr) = GetAttributeName(enc);
			count = count + 1;
		}
	}
	for(i = 0; i < count; i++)
	{
		attr = "del" + i;
		encID = "encounters." + forDelete.(attr);
		DeleteAttribute(&worldMap, encID);
	}
}

float wdmGetDays(int year, int month, int day, int hour)
{
	//Считаем дни по годам
	if(year < 0) year = 0;
	if(year > 3000) year = 3000;
	year = year*365;
	//Считаем целые дни
	for(int i = 1; i < month; i++)
	{
		day = day + GetMonthDays(i, year);
	}
	//Считаем полные дни
	float days = year + day + (hour/24.0);
	return days;
}

void wdmMarkDeleteEncounters()
{
	//Получаем дату
	int year = sti(worldMap.date.year);
	int month = sti(worldMap.date.month);
	int day = sti(worldMap.date.day);
	int hour = sti(worldMap.date.hour);
	float days = wdmGetDays(year, month, day, hour);
	int encYear, encMonth, encDay, encHour;
	//Перебираем все энкоунтеры, помечая на удаление
	aref encs;
	makearef(encs, worldMap.encounters);
	int num = GetAttributesNum(encs);
	for(int i = 0; i < num; i++)
	{
		aref enc = GetAttributeN(encs, i);
		if(CheckAttribute(enc, "Quest") != 0)
		{
			continue;
		}
		bool deleteMe = false;
		if(CheckAttribute(enc, "year") != 0)
		{
			encYear = sti(enc.year);
		}else{
			deleteMe = true;
		}
		if(CheckAttribute(enc, "month") != 0)
		{
			encMonth = sti(enc.month);
		}else{
			deleteMe = true;
		}
		if(CheckAttribute(enc, "day") != 0)
		{
			encDay = sti(enc.day);
		}else{
			deleteMe = true;
		}
		if(CheckAttribute(enc, "hour") != 0)
		{
			encHour = sti(enc.hour);
		}else{
			deleteMe = true;
		}
		if(deleteMe != true)
		{
			float deltaDays = wdmGetDays(encYear, encMonth, encDay, encHour) - days;
			if(deltaDays < 0)
			{
				deltaDays = -deltaDays;
			}
			if(deltaDays > 1.0)
			{
				deleteMe = true;
			}
		}
		if(deleteMe != false)
		{
			enc.needDelete = "Time delete";
		}
	}
}
