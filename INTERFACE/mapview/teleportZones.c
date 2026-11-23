// Gregg: подсветка зон телепорта на карте острова в атласе
// LEO: Чернильная обводка не меняет область ПКМ. Посещённые места отмечены
// цельным штрихом, непосещённые - бледным разомкнутым.
// LEO: Для каждой точки oMapTeleport текущей карты квадрат TZ_SIZE начинается
// в Pos.X/Pos.Y и совпадает с проверкой CC в DoTeleport. Это координаты мыши:
// дополнительное смещение не нужно.
// LEO: Метки видны, только если телепорт с текущей карты сейчас разрешён.
// pchar.AtlasVisited записывается в LoadLocation; непосещённые места остаются
// недоступными по проверке DoTeleport.

#define TZ_MAX   24   // узлов TP_ZONE_n в mapview.ini
#define TZ_SIZE  30   // = gridsize в DoTeleport

// LEO: В define только литералы; оттенки чернил без свечения и заливки.
#define TZ_OPEN_INK       4113834516 // LEO: argb(245,52,34,20)
#define TZ_CLOSED_INK     3443798046 // LEO: argb(205,68,48,30)

// Был ли герой в локации (любое посещение)
bool TZ_IsVisited(string locid)
{
	return CheckAttribute(pchar, "AtlasVisited." + locid);
}

// Можно ли сейчас телепортироваться по атласу (общие условия, без учёта карты)
bool TeleportAllowed()
{
	bool IsEnemyTown = false;
	int Nation = PIRATE;
	if (CheckAttribute(loadedLocation, "fastreload") && loadedLocation.fastreload != "")
	{
		int colony = FindColony(loadedLocation.fastreload);
		if (colony < 0) return false;
		Nation = sti(Colonies[colony].nation);
	}
	bool Enemy = GetNationRelation2MainCharacter(Nation) == RELATION_ENEMY || GetRelation2BaseNation(Nation) == RELATION_ENEMY;
	if (Enemy && Nation != PIRATE && !CheckNationLicence(Nation))
	{
		if (loadedLocation.type == "town" || loadedLocation.type == "residence" || loadedLocation.type == "tavern" || loadedLocation.type == "house" || loadedLocation.type == "shop" || loadedLocation.type == "shipyard" || loadedLocation.type == "church") IsEnemyTown = true;
	}

	// во время штурма/осады форта телепорт по карте атласа запрещён - иначе им можно выйти
	//   из захвата города посреди боя (флаг живёт до AfterTownBattle, colony.c).
	bool bTownAssault = CheckAttribute(PChar, "GenQuestFort.StartAttack");

	if (chrDisableReloadToLocation) return false;
	if (LAi_group_IsActivePlayerAlarm()) return false;
	if (CheckAttribute(PChar, "MapBestTeleportDisable")) return false;
	if (IsEnemyTown || bTownAssault) return false;
	return true;
}

void HideTeleportZones()
{
	for (int i = 1; i <= TZ_MAX; i++)
	{
		SetNodeUsing("TP_ZONE_" + i, false);
	}
}

void UpdateTeleportZones(string mapid)
{
	aref mapname, shorename, pos;
	string shore_name;
	int i, j, k, x, y;
	bool bOpen;

	HideTeleportZones();
	if (!CheckAttribute(&oMapTeleport, mapid)) return;
	makearef(mapname, oMapTeleport.(mapid));
	// те же условия, что в DoTeleport: телепорт работает только по карте текущего острова
	if (!CheckAttribute(loadedLocation, "islandId") || !CheckAttribute(mapname, "island")) return;
	if (mapname.island != loadedLocation.islandId) return;
	if (!TeleportAllowed()) return;

	k = 0;
	for (i = 0; i < GetAttributesNum(mapname); i++)
	{
		shorename = GetAttributeN(mapname, i);
		shore_name = GetAttributeName(shorename);
		if (shore_name == loadedLocation.id) continue; // в текущую локацию телепорт не срабатывает
		bOpen = TZ_IsVisited(shore_name);
		for (j = 0; j < GetAttributesNum(shorename); j++)
		{
			if (k >= TZ_MAX) return;
			pos = GetAttributeN(shorename, j);
			if (!CheckAttribute(pos, "X") || !CheckAttribute(pos, "Y")) continue;
			x = sti(pos.X);
			y = sti(pos.Y);
			k++;
			SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, "TP_ZONE_" + k, -1, 4, x, y, x + TZ_SIZE, y + TZ_SIZE);
			SetNodeUsing("TP_ZONE_" + k, true);
			// LEO: Четыре постоянных варианта; смена карты не расходует игровой RNG.
			int variant = (x + y) % 4;
			float left = variant * 0.25;
			float top = 0.5;
			int ink = TZ_CLOSED_INK;
			if (bOpen)
			{
				top = 0.0;
				ink = TZ_OPEN_INK;
			}
			SendMessage(&GameInterface, "lslffff", MSG_INTERFACE_MSG_TO_NODE, "TP_ZONE_" + k, 1, left, left + 0.25, top, top + 0.5);
			SendMessage(&GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, "TP_ZONE_" + k, 4, ink);
		}
	}
}
