// ============================================================================
// Gregg: метка текущего местонахождения героя на карте острова
// ----------------------------------------------------------------------------
// oHeroMarks.<id карты>.<id локации>.u/v - точка на картинке карты в пикселях
// текстуры 1024x1024 (как в графическом редакторе). Размечаются уличные
// локации: город (метка на якоре порта), выход из города, джунгли, развилки,
// бухты, пещеры. Для локаций без точки (таверна, дома, магазин и т.п.) метка
// ставится на ближайшую размеченную локацию по переходам reload.
// Точки размечены в heroMarkerPoints.c (InitHeroMarks).
// ============================================================================

object oHeroMarks; // точки метки героя: oHeroMarks.<карта>.<локация>.u/v

#include "interface\mapview\heroMarkerPoints.c"

#define HM_TEX_SIZE     1024.0  // размер текстуры карты
#define HM_MAP_LEFT     -15.0   // положение узла MAP в mapview.ini
#define HM_MAP_TOP      42.0
#define HM_MAP_WIDTH    615.0
#define HM_MAP_HEIGHT   544.0
#define HM_MARK_SIZE    14      // LEO: половина прежнего диаметра; привязка по центру
#define HM_MAX_DEPTH    3       // глубина поиска по переходам
#define HM_QUEUE_SIZE   64      // очередь поиска (= размер массивов q/d в HM_FindMarkLoc)

// Ищет ближайшую к герою локацию, у которой есть точка на карте mapid
string HM_FindMarkLoc(string mapid)
{
	aref arMap, rl, r;
	string q[64];
	int d[64];
	int head, tail, depth, li, i, k, n;
	string cur, nxt;
	bool seen;

	if (!CheckAttribute(&oHeroMarks, mapid)) return "";
	if (IsEntity(worldMap)) return "";
	if (!CheckAttribute(loadedLocation, "id")) return "";
	makearef(arMap, oHeroMarks.(mapid));

	head = 0;
	tail = 1;
	q[0] = loadedLocation.id;
	d[0] = 0;
	while (head < tail)
	{
		cur = q[head];
		depth = d[head];
		head++;

		if (CheckAttribute(arMap, cur)) return cur;
		if (depth >= HM_MAX_DEPTH) continue;

		li = FindLocation(cur);
		if (li < 0) continue;
		if (!CheckAttribute(&Locations[li], "reload")) continue;

		makearef(rl, Locations[li].reload);
		n = GetAttributesNum(rl);
		for (i = 0; i < n; i++)
		{
			r = GetAttributeN(rl, i);
			if (!CheckAttribute(r, "go")) continue;
			nxt = r.go;
			seen = false;
			for (k = 0; k < tail; k++)
			{
				if (q[k] == nxt) { seen = true; break; }
			}
			if (seen || tail >= HM_QUEUE_SIZE) continue;
			q[tail] = nxt;
			d[tail] = depth + 1;
			tail++;
		}
	}
	return "";
}

void UpdateHeroMarker(string mapid)
{
	string locid = HM_FindMarkLoc(mapid);
	if (locid == "")
	{
		SetNodeUsing("HERO_MARKER", false);
		return;
	}

	aref p;
	makearef(p, oHeroMarks.(mapid).(locid));
	// позиции из ini движок сам сдвигает под широкий экран, а позицию из скрипта - нет
	float offX = 0.0;
	float offY = 0.0;
	GetXYWindowOffset(&offX, &offY);

	int x = makeint(offX + HM_MAP_LEFT + stf(p.u) * HM_MAP_WIDTH / HM_TEX_SIZE);
	int y = makeint(offY + HM_MAP_TOP + stf(p.v) * HM_MAP_HEIGHT / HM_TEX_SIZE);
	int left = x - HM_MARK_SIZE / 2;
	int top = y - HM_MARK_SIZE / 2;

	SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, "HERO_MARKER", -1, 4,
		left, top, left + HM_MARK_SIZE, top + HM_MARK_SIZE);
	SetNodeUsing("HERO_MARKER", true);
}

// Gregg: карта для открытия атласа. Если текущая локация героя размечена прямо на табличной
// карте острова (tableMap) - оставляем её; иначе ищем среди карт героя ту, где она размечена.
string HM_PreferMap(string tableMap)
{
	aref root, m;
	int i, n;
	string locid, mapid;

	if (IsEntity(worldMap)) return tableMap;
	if (!CheckAttribute(loadedLocation, "id")) return tableMap;
	locid = loadedLocation.id;
	if (tableMap != "" && CheckAttribute(&oHeroMarks, tableMap + "." + locid)) return tableMap;

	makearef(root, oHeroMarks);
	n = GetAttributesNum(root);
	for (i = 0; i < n; i++)
	{
		m = GetAttributeN(root, i);
		mapid = GetAttributeName(m);
		if (CheckAttribute(m, locid) && GetCharacterItem(pchar, mapid) > 0) return mapid;
	}
	return tableMap;
}
