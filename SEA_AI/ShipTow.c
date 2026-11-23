#define SEA_TOW_MAX_PER_TUG 3

int SeaTow_Hull(ref ship)
{
	if (!CheckAttribute(ship, "Ship.Type")) return -1;
	int hull = sti(ship.Ship.Type);
	if (hull < 0 || hull >= REAL_SHIPS_QUANTITY) return -1;
	if (!CheckAttribute(&RealShips[hull], "name")) return -1;
	return hull;
}

int SeaTow_Stamp(ref owner, int hull)
{
	if (!CheckAttribute(&RealShips[hull], "SeaTowStamp"))
	{
		if (!CheckAttribute(owner, "SeaTow.Serial")) owner.SeaTow.Serial = 0;
		owner.SeaTow.Serial = sti(owner.SeaTow.Serial) + 1;
		RealShips[hull].SeaTowStamp = owner.SeaTow.Serial;
	}
	return sti(RealShips[hull].SeaTowStamp);
}

// LEO: Индекс капитана и индекс корпуса переиспользуются. Метка корпуса не даёт воскресить чужой буксир.
bool SeaTow_Matches(aref endpoint, ref ship)
{
	if (!CheckAttribute(endpoint, "Id") || !CheckAttribute(endpoint, "Index")) return false;
	if (!CheckAttribute(endpoint, "Hull") || !CheckAttribute(endpoint, "Stamp")) return false;
	if (!CheckAttribute(ship, "id") || !CheckAttribute(ship, "index")) return false;
	if (ship.id != endpoint.Id || sti(ship.index) != sti(endpoint.Index)) return false;
	int hull = SeaTow_Hull(ship);
	if (hull < 0 || hull != sti(endpoint.Hull)) return false;
	if (!CheckAttribute(&RealShips[hull], "SeaTowStamp")) return false;
	return sti(RealShips[hull].SeaTowStamp) == sti(endpoint.Stamp);
}

int SeaTow_Count(ref owner, ref tug)
{
	if (!CheckAttribute(owner, "SeaTow.Links")) return 0;
	aref links, link, endpoint;
	makearef(links, owner.SeaTow.Links);
	int count = 0;
	for (int i = 0; i < GetAttributesNum(links); i++)
	{
		link = GetAttributeN(links, i);
		makearef(endpoint, link.Tug);
		if (SeaTow_Matches(endpoint, tug)) count++;
	}
	return count;
}

bool SeaTow_IsTowed(ref owner, ref ship)
{
	if (!CheckAttribute(owner, "SeaTow.Links") || !CheckAttribute(ship, "index")) return false;
	string key = "prize" + ship.index;
	if (!CheckAttribute(owner, "SeaTow.Links." + key)) return false;
	aref prize;
	makearef(prize, owner.SeaTow.Links.(key).Prize);
	return SeaTow_Matches(prize, ship);
}

bool SeaTow_Attach(ref owner, ref tug, ref ship)
{
	if (!CheckAttribute(tug, "id") || !CheckAttribute(ship, "id")) return false;
	if (!CheckAttribute(tug, "index") || !CheckAttribute(ship, "index")) return false;
	int tugHull = SeaTow_Hull(tug);
	int prizeHull = SeaTow_Hull(ship);
	if (tugHull < 0 || prizeHull < 0 || tugHull == prizeHull || tug.index == ship.index) return false;
	string key = "prize" + ship.index;
	if (CheckAttribute(owner, "SeaTow.Links." + key)) return false;
	if (SeaTow_IsTowed(owner, tug) || SeaTow_Count(owner, ship) > 0) return false;
	if (SeaTow_Count(owner, tug) >= SEA_TOW_MAX_PER_TUG) return false;
	int tugStamp = SeaTow_Stamp(owner, tugHull);
	int prizeStamp = SeaTow_Stamp(owner, prizeHull);
	owner.SeaTow.Links.(key).Tug.Id = tug.id;
	owner.SeaTow.Links.(key).Tug.Index = tug.index;
	owner.SeaTow.Links.(key).Tug.Hull = tugHull;
	owner.SeaTow.Links.(key).Tug.Stamp = tugStamp;
	owner.SeaTow.Links.(key).Prize.Id = ship.id;
	owner.SeaTow.Links.(key).Prize.Index = ship.index;
	owner.SeaTow.Links.(key).Prize.Hull = prizeHull;
	owner.SeaTow.Links.(key).Prize.Stamp = prizeStamp;
	return true;
}

void SeaTow_ReleaseStamp(ref owner, int hull, int stamp)
{
	if (hull < 0 || hull >= REAL_SHIPS_QUANTITY) return;
	if (!CheckAttribute(&RealShips[hull], "SeaTowStamp")) return;
	if (sti(RealShips[hull].SeaTowStamp) != stamp) return;
	aref links, link;
	makearef(links, owner.SeaTow.Links);
	for (int i = 0; i < GetAttributesNum(links); i++)
	{
		link = GetAttributeN(links, i);
		if (CheckAttribute(link, "Tug.Hull") && CheckAttribute(link, "Tug.Stamp"))
		{
			if (sti(link.Tug.Hull) == hull && sti(link.Tug.Stamp) == stamp) return;
		}
		if (CheckAttribute(link, "Prize.Hull") && CheckAttribute(link, "Prize.Stamp"))
		{
			if (sti(link.Prize.Hull) == hull && sti(link.Prize.Stamp) == stamp) return;
		}
	}
	DeleteAttribute(&RealShips[hull], "SeaTowStamp");
}

void SeaTow_Remove(ref owner, string key)
{
	if (!CheckAttribute(owner, "SeaTow.Links." + key)) return;
	aref links, link;
	makearef(links, owner.SeaTow.Links);
	makearef(link, links.(key));
	int tugHull = -1;
	int tugStamp = -1;
	int prizeHull = -1;
	int prizeStamp = -1;
	if (CheckAttribute(link, "Tug.Hull")) tugHull = sti(link.Tug.Hull);
	if (CheckAttribute(link, "Tug.Stamp")) tugStamp = sti(link.Tug.Stamp);
	if (CheckAttribute(link, "Prize.Hull")) prizeHull = sti(link.Prize.Hull);
	if (CheckAttribute(link, "Prize.Stamp")) prizeStamp = sti(link.Prize.Stamp);
	DeleteAttribute(links, key);
	SeaTow_ReleaseStamp(owner, tugHull, tugStamp);
	SeaTow_ReleaseStamp(owner, prizeHull, prizeStamp);
	if (GetAttributesNum(links) == 0) DeleteAttribute(owner, "SeaTow.Links");
}

bool SeaTow_Detach(ref owner, ref tug, ref ship)
{
	if (!SeaTow_IsTowed(owner, ship)) return false;
	string key = "prize" + ship.index;
	aref endpoint;
	makearef(endpoint, owner.SeaTow.Links.(key).Tug);
	if (!SeaTow_Matches(endpoint, tug)) return false;
	SeaTow_Remove(owner, key);
	return true;
}

int SeaTow_Prune(ref owner, int index, ref current)
{
	if (!CheckAttribute(owner, "SeaTow.Links")) return 0;
	aref links, link, tug, prize;
	makearef(links, owner.SeaTow.Links);
	int removed = 0;
	for (int i = GetAttributesNum(links) - 1; i >= 0; i--)
	{
		link = GetAttributeN(links, i);
		makearef(tug, link.Tug);
		makearef(prize, link.Prize);
		bool stale = sti(tug.Index) == index && !SeaTow_Matches(tug, current);
		if (sti(prize.Index) == index && !SeaTow_Matches(prize, current)) stale = true;
		if (!stale) continue;
		string key = GetAttributeName(link);
		SeaTow_Remove(owner, key);
		removed++;
	}
	return removed;
}
