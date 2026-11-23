#define SEA_TOW_ORDER_TICK_MS 500
#define SEA_TOW_ORDER_POSITION_EPSILON 0.05

object SeaTowOrders;
int SeaTowOrdersGeneration;
int SeaTowOrdersSerial;

// LEO: Заявки не входят в постоянный реестр связей; переход отменяет только незавершённые подходы.
bool SeaTowOrders_HasPending()
{
	if (!CheckAttribute(&SeaTowOrders, "Orders")) return false;
	aref orders;
	makearef(orders, SeaTowOrders.Orders);
	if (GetAttributesNum(orders) > 0) return true;
	DeleteAttribute(&SeaTowOrders, "Orders");
	return false;
}

bool SeaTowOrders_SeaReady()
{
	if (!bSeaActive || bSeaReloadStarted || bAbordageStarted) return false;
	if (!CheckAttribute(pchar, "id") || !CheckAttribute(pchar, "index")) return false;
	if (DialogRun || questMovieProcess != 0 || LAi_IsDead(pchar)) return false;
	return CheckAttribute(&AISea, "TowingVersion") && CheckAttribute(pchar, "location");
}

// LEO: Отдельная метка корпуса отличает его повторное использование, не изменяя метки настоящего буксира.
void SeaTowOrders_Snapshot(aref endpoint, ref chr)
{
	int hull = SeaTow_Hull(chr);
	if (hull < 0 || !CheckAttribute(chr, "id") || !CheckAttribute(chr, "index")) return;
	if (!CheckAttribute(&RealShips[hull], "SeaTowOrderStamp"))
	{
		SeaTowOrdersSerial++;
		RealShips[hull].SeaTowOrderStamp = SeaTowOrdersSerial;
	}
	endpoint.Id = chr.id;
	endpoint.Index = chr.index;
	endpoint.Hull = hull;
	endpoint.Stamp = RealShips[hull].SeaTowOrderStamp;
}

bool SeaTowOrders_EndpointValid(aref endpoint)
{
	if (!CheckAttribute(endpoint, "Index") || !CheckAttribute(endpoint, "Id")) return false;
	if (!CheckAttribute(endpoint, "Hull") || !CheckAttribute(endpoint, "Stamp")) return false;
	int index = sti(endpoint.Index);
	if (index < 0 || index >= TOTAL_CHARACTERS) return false;
	ref chr = GetCharacter(index);
	if (!CheckAttribute(chr, "id") || !CheckAttribute(chr, "index")) return false;
	if (chr.id != endpoint.Id || sti(chr.index) != index) return false;
	int hull = SeaTow_Hull(chr);
	if (hull < 0 || hull != sti(endpoint.Hull)) return false;
	if (!CheckAttribute(&RealShips[hull], "SeaTowOrderStamp")) return false;
	return sti(RealShips[hull].SeaTowOrderStamp) == sti(endpoint.Stamp);
}

bool SeaTowOrders_CanRequest(ref tug, ref prize)
{
	if (!SeaTowOrders_SeaReady()) return false;
	if (!CheckAttribute(tug, "index") || !CheckAttribute(prize, "index")) return false;
	int tugIndex = sti(tug.index);
	int prizeIndex = sti(prize.index);
	if (tugIndex < 0 || tugIndex >= TOTAL_CHARACTERS || prizeIndex < 0 || prizeIndex >= TOTAL_CHARACTERS) return false;
	if (!CheckAttribute(tug, "id") || !CheckAttribute(prize, "id")) return false;
	if (!SeaTow_CanTug(tug) || IsMainCharacter(tug) || IsCompanion(prize) || IsMainCharacter(prize)) return false;
	if (!IsEntity(tug) || !IsEntity(prize) || LAi_IsDead(tug) || LAi_IsDead(prize)) return false;
	if (Boarding_Busy(tug) || Boarding_Busy(prize)) return false;
	if (!HasSubStr(prize.id, "_DriftCap_")) return false;
	int tugHull = SeaTow_Hull(tug);
	int prizeHull = SeaTow_Hull(prize);
	if (tugHull < 0 || prizeHull < 0 || tugHull == prizeHull || tugIndex == prizeIndex) return false;
	if (!CheckAttribute(prize, "Ship.Pos.x") || !CheckAttribute(prize, "Ship.Pos.z")) return false;
	string prizeKey = "prize" + prizeIndex;
	if (CheckAttribute(pchar, "SeaTow.Links." + prizeKey)) return false;
	if (SeaTow_IsTowed(pchar, tug) || SeaTow_Count(pchar, prize) > 0) return false;
	if (SeaTow_Count(pchar, tug) >= SEA_TOW_MAX_PER_TUG) return false;
	if (!SeaTowOrders_HasPending()) return true;
	aref orders, order, endpoint;
	makearef(orders, SeaTowOrders.Orders);
	for (int i = 0; i < GetAttributesNum(orders); i++)
	{
		order = GetAttributeN(orders, i);
		if (!CheckAttribute(order, "Prize") || !CheckAttribute(order, "Tug")) continue;
		makearef(endpoint, order.Prize);
		if (!SeaTowOrders_EndpointValid(endpoint)) continue;
		if (sti(endpoint.Index) != prizeIndex && sti(endpoint.Hull) != prizeHull) continue;
		makearef(endpoint, order.Tug);
		if (!SeaTowOrders_EndpointValid(endpoint) || sti(endpoint.Index) != tugIndex) return false;
	}
	return true;
}

bool SeaTowOrders_OwnTow(ref tug, ref prize)
{
	if (!CheckAttribute(tug, "index") || !CheckAttribute(prize, "index")) return false;
	int tugIndex = sti(tug.index);
	int prizeIndex = sti(prize.index);
	if (tugIndex < 0 || tugIndex >= TOTAL_CHARACTERS || prizeIndex < 0 || prizeIndex >= TOTAL_CHARACTERS) return false;
	string key = "prize" + prizeIndex;
	if (!CheckAttribute(pchar, "SeaTow.Links." + key + ".Tug")) return false;
	if (!CheckAttribute(pchar, "SeaTow.Links." + key + ".Prize")) return false;
	aref endpoint;
	makearef(endpoint, pchar.SeaTow.Links.(key).Prize);
	if (!SeaTow_ValidEndpoint(endpoint) || !SeaTow_Matches(endpoint, prize)) return false;
	makearef(endpoint, pchar.SeaTow.Links.(key).Tug);
	return SeaTow_ValidEndpoint(endpoint) && SeaTow_Matches(endpoint, tug);
}

// LEO: Выбор своего зацепленного приза разрешён и при полном лимите; строгая проверка подхода не меняется.
bool SeaTowOrders_CanSelect(ref tug, ref prize)
{
	if (!SeaTowOrders_SeaReady()) return false;
	if (!SeaTowOrders_OwnTow(tug, prize)) return SeaTowOrders_CanRequest(tug, prize);
	if (!SeaTow_CanTug(tug) || IsMainCharacter(tug)) return false;
	if (!IsEntity(tug) || !IsEntity(prize) || LAi_IsDead(tug) || LAi_IsDead(prize)) return false;
	return !Boarding_Busy(tug) && !Boarding_Busy(prize);
}

bool SeaTowOrders_HasTarget(ref tug)
{
	for (int i = 0; i < iNumShips; i++)
	{
		int index = Ships[i];
		if (index < 0 || index >= TOTAL_CHARACTERS) continue;
		if (SeaTowOrders_CanSelect(tug, GetCharacter(index))) return true;
	}
	return false;
}

bool SeaTowOrders_OwnsTask(aref order)
{
	if (!CheckAttribute(order, "Tug") || !CheckAttribute(order, "Prize.Index")) return false;
	if (!CheckAttribute(order, "MoveX") || !CheckAttribute(order, "MoveZ")) return false;
	aref endpoint;
	makearef(endpoint, order.Tug);
	if (!SeaTowOrders_EndpointValid(endpoint)) return false;
	ref tug = GetCharacter(sti(endpoint.Index));
	if (!CheckAttribute(tug, "SeaAI.Task") || !CheckAttribute(tug, "SeaAI.Task.Target")) return false;
	if (sti(tug.SeaAI.Task) != AITASK_MOVE || sti(tug.SeaAI.Task.Target) != sti(order.Prize.Index)) return false;
	if (!CheckAttribute(tug, "SeaAI.Task.Pos.x") || !CheckAttribute(tug, "SeaAI.Task.Pos.z")) return false;
	// LEO: Движок меняет внутреннюю точку обхода, не SeaAI.Task.Pos; допуск покрывает строковый обмен float.
	return abs(stf(tug.SeaAI.Task.Pos.x) - stf(order.MoveX)) <= SEA_TOW_ORDER_POSITION_EPSILON &&
		abs(stf(tug.SeaAI.Task.Pos.z) - stf(order.MoveZ)) <= SEA_TOW_ORDER_POSITION_EPSILON;
}

void SeaTowOrders_ReleaseStamp(int hull, int stamp)
{
	if (hull < 0 || hull >= REAL_SHIPS_QUANTITY) return;
	if (!CheckAttribute(&RealShips[hull], "SeaTowOrderStamp")) return;
	if (sti(RealShips[hull].SeaTowOrderStamp) == stamp) DeleteAttribute(&RealShips[hull], "SeaTowOrderStamp");
}

// LEO: Чужой новый приказ и переиспользованного капитана не трогаем; свой подход заменяем защитой флагмана.
void SeaTowOrders_Remove(string key, bool stopApproach)
{
	if (key == "") return;
	if (!CheckAttribute(&SeaTowOrders, "Orders." + key)) return;
	aref order;
	makearef(order, SeaTowOrders.Orders.(key));
	if (stopApproach && SeaTowOrders_OwnsTask(order))
	{
		ref tug = GetCharacter(sti(order.Tug.Index));
		if (IsCompanion(tug) && !LAi_IsDead(tug) && !Boarding_Busy(tug))
		{
			DeleteAttribute(tug, "TacOrderTarget");
			if (IsEntity(tug) && IsEntity(pchar) && !LAi_IsDead(pchar))
				Ship_SetTaskDefend(SECONDARY_TASK, sti(tug.index), GetMainCharacterIndex());
		}
	}
	int tugHull = -1;
	int tugStamp = -1;
	int prizeHull = -1;
	int prizeStamp = -1;
	if (CheckAttribute(order, "Tug.Hull") && CheckAttribute(order, "Tug.Stamp"))
	{
		tugHull = sti(order.Tug.Hull);
		tugStamp = sti(order.Tug.Stamp);
	}
	if (CheckAttribute(order, "Prize.Hull") && CheckAttribute(order, "Prize.Stamp"))
	{
		prizeHull = sti(order.Prize.Hull);
		prizeStamp = sti(order.Prize.Stamp);
	}
	DeleteAttribute(&SeaTowOrders, "Orders." + key);
	SeaTowOrders_ReleaseStamp(tugHull, tugStamp);
	SeaTowOrders_ReleaseStamp(prizeHull, prizeStamp);
	SeaTowOrders_HasPending();
}

void SeaTowOrders_Cancel(ref tug)
{
	if (!CheckAttribute(tug, "index")) return;
	if (sti(tug.index) < 0 || sti(tug.index) >= TOTAL_CHARACTERS) return;
	string key = "tug" + tug.index;
	SeaTowOrders_Remove(key, true);
	if (!SeaTowOrders_HasPending())
	{
		SeaTowOrdersGeneration++;
		DelEventHandler("SeaTowOrdersTick", "SeaTowOrders_Tick");
	}
}

void SeaTowOrders_Move(aref order, ref tug, ref prize)
{
	order.MoveX = prize.Ship.Pos.x;
	order.MoveZ = prize.Ship.Pos.z;
	Ship_SetTaskMove(SECONDARY_TASK, sti(tug.index), stf(order.MoveX), stf(order.MoveZ));
	tug.SeaAI.Task.Target = prize.index;
	tug.TacOrderTarget = prize.index;
	tug.TacOrderDefend = 1;
}

// LEO: Один подход на компаньона; отказ не уничтожает прежнюю заявку и не выдаёт звуков из таймера.
bool SeaTowOrders_Request(ref tug, ref prize)
{
	if (!SeaTowOrders_CanRequest(tug, prize)) return false;
	if (Ship_GetDistance2D(tug, prize) <= SEA_TOW_ATTACH_DISTANCE)
	{
		if (!SeaTow_Begin(tug, prize)) return false;
		SeaTowOrders_Cancel(tug);
		DeleteAttribute(tug, "CompanionAutoEngage");
		Ship_SetTaskDefend(SECONDARY_TASK, sti(tug.index), GetMainCharacterIndex());
		return true;
	}
	SeaTowOrders_Cancel(tug);
	string key = "tug" + tug.index;
	aref order, endpoint;
	makearef(order, SeaTowOrders.Orders.(key));
	makearef(endpoint, order.Tug);
	SeaTowOrders_Snapshot(endpoint, tug);
	makearef(endpoint, order.Prize);
	SeaTowOrders_Snapshot(endpoint, prize);
	order.Scene = pchar.location;
	order.Owner = pchar.id;
	order.OwnerIndex = pchar.index;
	DeleteAttribute(tug, "CompanionAutoEngage");
	SeaTowOrders_Move(order, tug, prize);
	SeaTowOrders_Schedule();
	return true;
}

// LEO: Одна команда зацепляет свободный приз либо отцепляет свой; чужой буксир и его заявки не меняются.
bool SeaTowOrders_Toggle(ref tug, ref prize)
{
	if (!SeaTowOrders_CanSelect(tug, prize)) return false;
	if (SeaTowOrders_OwnTow(tug, prize))
	{
		if (!SeaTow_End(tug, prize)) return false;
		SeaTowOrders_Cancel(tug);
		return true;
	}
	return SeaTowOrders_Request(tug, prize);
}

void SeaTowOrders_Suspend()
{
	SeaTowOrdersGeneration++;
	DelEventHandler("SeaTowOrdersTick", "SeaTowOrders_Tick");
	DeleteAttribute(&SeaTowOrders, "StrategyHandledTarget");
	if (!SeaTowOrders_HasPending()) return;
	aref orders, order;
	makearef(orders, SeaTowOrders.Orders);
	for (int i = GetAttributesNum(orders) - 1; i >= 0; i--)
	{
		order = GetAttributeN(orders, i);
		string key = GetAttributeName(order);
		SeaTowOrders_Remove(key, true);
	}
}

void SeaTowOrders_Update()
{
	if (!SeaTowOrders_HasPending()) return;
	if (!SeaTowOrders_SeaReady()) { SeaTowOrders_Suspend(); return; }
	aref orders, order, endpoint;
	makearef(orders, SeaTowOrders.Orders);
	for (int i = GetAttributesNum(orders) - 1; i >= 0; i--)
	{
		order = GetAttributeN(orders, i);
		string key = GetAttributeName(order);
		if (!CheckAttribute(order, "Prize") || !CheckAttribute(order, "Scene") ||
			!CheckAttribute(order, "Owner") || !CheckAttribute(order, "OwnerIndex"))
		{
			SeaTowOrders_Remove(key, true);
			continue;
		}
		makearef(endpoint, order.Prize);
		bool valid = SeaTowOrders_EndpointValid(endpoint) && SeaTowOrders_OwnsTask(order);
		if (order.Scene != pchar.location || order.Owner != pchar.id || sti(order.OwnerIndex) != sti(pchar.index)) valid = false;
		if (valid)
		{
			ref tug = GetCharacter(sti(order.Tug.Index));
			ref prize = GetCharacter(sti(order.Prize.Index));
			valid = SeaTowOrders_CanRequest(tug, prize);
		}
		if (!valid) { SeaTowOrders_Remove(key, true); continue; }
		if (Ship_GetDistance2D(tug, prize) <= SEA_TOW_ATTACH_DISTANCE)
		{
			SeaTow_Begin(tug, prize);
			SeaTowOrders_Remove(key, true);
		}
		else SeaTowOrders_Move(order, tug, prize);
	}
}

// LEO: Независимый таймер нужен до первого зацепа: штатный таймер связей без буксира не запускается.
void SeaTowOrders_Schedule()
{
	SeaTowOrdersGeneration++;
	DelEventHandler("SeaTowOrdersTick", "SeaTowOrders_Tick");
	if (!SeaTowOrders_HasPending()) return;
	SetEventHandler("SeaTowOrdersTick", "SeaTowOrders_Tick", 0);
	PostEvent("SeaTowOrdersTick", SEA_TOW_ORDER_TICK_MS, "l", SeaTowOrdersGeneration);
}

void SeaTowOrders_Tick()
{
	int generation = GetEventData();
	if (generation != SeaTowOrdersGeneration) return;
	DelEventHandler("SeaTowOrdersTick", "SeaTowOrders_Tick");
	SeaTowOrders_Update();
	SeaTowOrders_Schedule();
}
