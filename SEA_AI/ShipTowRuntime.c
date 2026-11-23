#include "sea_ai\ShipTow.c"

#define SEA_TOW_ATTACH_DISTANCE 120.0
// LEO: EA — Н; демпфирование — доля критического; пределы ускорений — м/с² и рад/с².
#define SEA_TOW_AXIAL_RIGIDITY 20000000.0
#define SEA_TOW_DAMPING_RATIO 0.9
#define SEA_TOW_ACCELERATION 8.0
#define SEA_TOW_ANGULAR_ACCELERATION 0.25
#define SEA_TOW_ANGULAR_DAMPING 0.4
#define SEA_TOW_ADDED_MASS 1.2
// LEO: Угловое затухание — 1/с; AddedMass учитывает присоединённую воду. Предел скорости не навязывается.
#define SEA_TOW_ROPE_COLOR 4290617713
#define SEA_TOW_ROPE_SAG 0.06
// LEO: Запас длины и разнос призов — метры; крепления — доли габарита корпуса, не мачт.
#define SEA_TOW_ROPE_CLEARANCE 55.0
#define SEA_TOW_ROPE_EXTRA_SLACK 20.0
#define SEA_TOW_ANCHOR_ALONG 0.43
#define SEA_TOW_ANCHOR_HEIGHT 0.15
#define SEA_TOW_ANCHOR_MIN_HEIGHT 0.8
#define SEA_TOW_ANCHOR_MAX_HEIGHT 6.0
#define SEA_TOW_ROPE_MAX_SAG 5.0
#define SEA_TOW_ROPE_MIN_SAG 0.05
#define SEA_TOW_ROPE_SEGMENTS 24
#define SEA_TOW_SAILTO_SAMPLES 16
#define SEA_TOW_TICK_MS 500
#define SEA_TOW_TELEMETRY_INTERVAL 0.25

int SeaTowTickGeneration;
#event_handler("Control Activation", "SeaTow_Control");

// LEO: 0 на палубе выбирает ближайший дрейфующий приз в радиусе зацепа; повторное нажатие снимает его буксир.
void SeaTow_Control()
{
	string control = GetEventData();
	if (control != "SeaTowToggle") return;
	SeaTow_TogglePlayer();
}

// LEO: После абордажа entid персонажа принадлежал наземному актёру; корабль ищем по его нативному владельцу.
bool SeaTow_HasSeaShip(ref chr)
{
	if (!CheckAttribute(chr, "id") || !CheckAttribute(chr, "index")) return false;
	int index = sti(chr.index);
	if (index < 0 || index >= TOTAL_CHARACTERS) return false;
	aref ship;
	if (!FindEntity(&ship, "ship")) return false;
	while (true)
	{
		int owner = -1;
		SendMessage(ship, "le", MSG_SHIP_GET_CHARACTER_INDEX, &owner);
		if (owner == index) return true;
		if (!FindEntityNext(&ship)) return false;
	}
	return false;
}

// LEO: Только вход и итог на нажатие; таймеры и кадровые обработчики сюда не обращаются.
void SeaTow_InputTrace(string reason, int target, float distance)
{
	string camera = "";
	string id = "";
	int binding = -1;
	int code = -1;
	int launched = -1;
	int heroEntity = -1;
	int prizeEntity = -1;
	if (bSeaActive) heroEntity = IsEntity(pchar);
	if (CheckAttribute(&SeaCameras, "Camera")) camera = SeaCameras.Camera;
	if (CheckAttribute(&objControlsState, "boundkeys.SeaTowToggle")) binding = sti(objControlsState.boundkeys.SeaTowToggle);
	if (CheckAttribute(&objControlsState, "map.controls.SeaTowToggle")) code = sti(objControlsState.map.controls.SeaTowToggle);
	if (CheckAttribute(&InterfaceStates, "Launched")) launched = sti(InterfaceStates.Launched);
	if (target >= 0 && target < TOTAL_CHARACTERS)
	{
		ref chr = GetCharacter(target);
		if (CheckAttribute(chr, "id")) id = chr.id;
		prizeEntity = IsEntity(chr);
	}
	Trace("[SEA_TOW_INPUT] fn=SeaTow_TogglePlayer reason=" + reason + " group=" + curKeyGroupName +
		" camera=" + camera + " binding=" + binding + " code=" + code + " target=" + target +
		" id=" + id + " distance=" + distance + " ui=" + launched + " sea=" + bSeaActive + " reload=" + bSeaReloadStarted + " abordage=" + bAbordageStarted +
		" heroEntity=" + heroEntity + " prizeEntity=" + prizeEntity);
}

void SeaTow_InputReject(string reason, int target, float distance, bool visible)
{
	SeaTow_InputTrace(reason, target, distance);
	if (visible) Log_Info(XI_ConvertString("SeaTowFail" + reason));
}

// LEO: Причина отказа для клавиши зацепа ГГ.
string SeaTow_PlayerBeginReason(ref prize)
{
	if (!CheckAttribute(&AISea, "TowingVersion")) return "Engine";
	if (sti(AISea.TowingVersion) < 2) return "Engine";
	if (!SeaTow_CanTug(pchar)) return "CannotTug";
	if (!CheckAttribute(pchar, "id") || !CheckAttribute(pchar, "index") || !CheckAttribute(prize, "index")) return "Endpoint";
	if (!SeaTow_HasSeaShip(pchar) || !SeaTow_HasSeaShip(prize)) return "NoEntity";
	if (LAi_IsDead(pchar) || LAi_IsDead(prize)) return "Dead";
	if (!CheckAttribute(prize, "id")) return "NoDrift";
	if (!HasSubStr(prize.id, "_DriftCap_")) return "NoDrift";
	if (Boarding_Busy(pchar) || Boarding_Busy(prize)) return "Busy";
	if (Ship_GetDistance2D(pchar, prize) > SEA_TOW_ATTACH_DISTANCE) return "Distance";
	int tugHull = SeaTow_Hull(pchar);
	int prizeHull = SeaTow_Hull(prize);
	if (tugHull < 0 || prizeHull < 0 || tugHull == prizeHull || pchar.index == prize.index) return "InvalidHull";
	string key = "prize" + prize.index;
	if (CheckAttribute(pchar, "SeaTow.Links." + key)) return "NotOwner";
	if (SeaTow_IsTowed(pchar, pchar) || SeaTow_Count(pchar, prize) > 0) return "Chain";
	if (SeaTow_Count(pchar, pchar) >= SEA_TOW_MAX_PER_TUG) return "Limit";
	return "";
}

void SeaTow_TogglePlayer()
{
	SeaTow_InputTrace("Entry", -1, -1.0);
	if (!bSeaActive) { SeaTow_InputReject("NoSea", -1, -1.0, false); return; }
	if (bSeaReloadStarted) { SeaTow_InputReject("Reload", -1, -1.0, false); return; }
	if (bAbordageStarted) { SeaTow_InputReject("Abordage", -1, -1.0, false); return; }
	if (SeaCameras.Camera != "SeaDeckCamera") { SeaTow_InputReject("Camera", -1, -1.0, false); return; }
	if (sti(InterfaceStates.Launched) != 0) { SeaTow_InputReject("Interface", -1, -1.0, false); return; }
	int nearest = -1;
	float best = SEA_TOW_ATTACH_DISTANCE;
	int distant = -1;
	float distantBest = 1000000000.0;
	int other = -1;
	float otherBest = SEA_TOW_ATTACH_DISTANCE;
	int dead = -1;
	float deadBest = SEA_TOW_ATTACH_DISTANCE;
	for (int i = 0; i < iNumShips; i++)
	{
		int index = Ships[i];
		if (index < 0 || index >= TOTAL_CHARACTERS || index == GetMainCharacterIndex()) continue;
		ref prize = GetCharacter(index);
		if (!CheckAttribute(prize, "id")) continue;
		float distance = Ship_GetDistance2D(pchar, prize);
		if (!HasSubStr(prize.id, "_DriftCap_"))
		{
			if (!LAi_IsDead(prize) && distance <= otherBest) { otherBest = distance; other = index; }
			continue;
		}
		if (LAi_IsDead(prize))
		{
			if (distance <= deadBest) { deadBest = distance; dead = index; }
			continue;
		}
		if (distance <= distantBest) { distantBest = distance; distant = index; }
		if (distance > best) continue;
		best = distance;
		nearest = index;
	}
	if (nearest < 0)
	{
		if (distant >= 0) SeaTow_InputReject("Distance", distant, distantBest, true);
		else
		{
			if (dead >= 0) SeaTow_InputReject("Dead", dead, deadBest, true);
			else
			{
				if (other >= 0) SeaTow_InputReject("NoDrift", other, otherBest, true);
				else SeaTow_InputReject("NoTarget", -1, -1.0, true);
			}
		}
		return;
	}
	prize = GetCharacter(nearest);
	if (SeaTow_IsTowed(pchar, prize))
	{
		if (!SeaTow_End(pchar, prize)) { SeaTow_InputReject("NotOwner", nearest, best, true); return; }
		SeaTow_InputTrace("Detached", nearest, best);
	}
	else
	{
		string reason = SeaTow_PlayerBeginReason(prize);
		if (reason != "") { SeaTow_InputReject(reason, nearest, best, true); return; }
		if (!SeaTow_Begin(pchar, prize)) { SeaTow_InputReject("Group", nearest, best, true); return; }
		SeaTow_InputTrace("Attached", nearest, best);
	}
	PlaySound("interface\ok.wav");
}

// LEO: Реестр живёт на pchar, а не на морской сущности; в нём только индексы, строки и числа.
bool SeaTow_HasLinks()
{
	if (!CheckAttribute(pchar, "SeaTow.Links")) return false;
	aref links;
	makearef(links, pchar.SeaTow.Links);
	if (GetAttributesNum(links) > 0) return true;
	DeleteAttribute(pchar, "SeaTow.Links");
	return false;
}

bool SeaTow_ValidEndpoint(aref endpoint)
{
	if (!CheckAttribute(endpoint, "Index") || !CheckAttribute(endpoint, "Id")) return false;
	if (!CheckAttribute(endpoint, "Hull") || !CheckAttribute(endpoint, "Stamp")) return false;
	int index = sti(endpoint.Index);
	if (index < 0 || index >= TOTAL_CHARACTERS) return false;
	return SeaTow_Matches(endpoint, GetCharacter(index));
}

float SeaTow_Mass(ref chr)
{
	int hull = SeaTow_Hull(chr);
	if (hull < 0) return 0.0;
	float mass = stf(RealShips[hull].Weight);
	float cargo = GetCargoLoad(chr);
	if (cargo > 0.0) mass += cargo;
	if (mass < 1.0) return 1.0;
	return mass;
}

// LEO: Имя группы переживает снятие буксира и смену stamp связи; проверяем владельца и единственного участника.
bool SeaTow_GroupOwned(string name, aref endpoint)
{
	if (!CheckAttribute(endpoint, "Index") || !CheckAttribute(endpoint, "Id")) return false;
	if (!CheckAttribute(endpoint, "Hull") || !CheckAttribute(endpoint, "Stamp")) return false;
	int index = sti(endpoint.Index);
	if (index < 0 || index >= TOTAL_CHARACTERS || sti(endpoint.Hull) < 0) return false;
	int groupIndex = Group_FindGroup(name);
	if (groupIndex < 0) return false;
	ref group = &AIGroups[groupIndex];
	if (!CheckAttribute(group, "MainCharacter")) return false;
	if (group.MainCharacter != endpoint.Id || Group_GetCharactersNumR(group) != 1) return false;
	string member = "id_" + index;
	if (!CheckAttribute(group, "Quest." + member + ".index")) return false;
	if (group.Quest.(member) != endpoint.Id || sti(group.Quest.(member).index) != index) return false;
	if (!CheckAttribute(group, "SeaTow"))
	{
		// LEO: Старый сейв можно принять только по точному stamp живого endpoint, не по префиксу имени.
		return name == "SeaTow_" + endpoint.Stamp && SeaTow_ValidEndpoint(endpoint);
	}
	if (!CheckAttribute(group, "SeaTow.GroupStamp") || !CheckAttribute(group, "SeaTow.PrizeId")) return false;
	if (!CheckAttribute(group, "SeaTow.PrizeIndex") || !CheckAttribute(group, "SeaTow.PrizeHull")) return false;
	if (sti(group.SeaTow.GroupStamp) <= 0 || name != "SeaTow_" + group.SeaTow.GroupStamp) return false;
	return group.SeaTow.PrizeId == endpoint.Id && sti(group.SeaTow.PrizeIndex) == index &&
		sti(group.SeaTow.PrizeHull) == sti(endpoint.Hull);
}

void SeaTow_BindGroup(string name, aref endpoint)
{
	ref group = Group_GetGroupByID(name);
	if (!CheckAttribute(group, "SeaTow.GroupStamp")) group.SeaTow.GroupStamp = endpoint.Stamp;
	group.SeaTow.PrizeId = endpoint.Id;
	group.SeaTow.PrizeIndex = endpoint.Index;
	group.SeaTow.PrizeHull = endpoint.Hull;
}

void SeaTow_Forget(string key)
{
	if (!CheckAttribute(pchar, "SeaTow.Links." + key)) return;
	aref link, target, tugInfo;
	makearef(link, pchar.SeaTow.Links.(key));
	makearef(target, link.Prize);
	makearef(tugInfo, link.Tug);
	int tugIndex = -1;
	int tugStamp = -1;
	int prizeIndex = -1;
	int prizeStamp = -1;
	if (CheckAttribute(tugInfo, "Index")) tugIndex = sti(tugInfo.Index);
	if (CheckAttribute(tugInfo, "Stamp")) tugStamp = sti(tugInfo.Stamp);
	if (CheckAttribute(target, "Index")) prizeIndex = sti(target.Index);
	if (CheckAttribute(target, "Stamp")) prizeStamp = sti(target.Stamp);
	if (CheckAttribute(link, "Group"))
	{
		string group = link.Group;
		if (SeaTow_GroupOwned(group, target))
		{
			SeaTow_BindGroup(group, target);
			Group_DeleteAtEnd(group);
		}
	}
	if (prizeIndex >= 0 && prizeIndex < TOTAL_CHARACTERS)
	{
		ref prize = GetCharacter(prizeIndex);
		if (CheckAttribute(prize, "Ship.SeaTow.PrizeStamp"))
		{
			if (sti(prize.Ship.SeaTow.PrizeStamp) == prizeStamp) DeleteAttribute(prize, "Ship.SeaTow");
		}
	}
	if (SeaTow_ValidEndpoint(target))
	{
		prize = GetCharacter(prizeIndex);
		DeleteAttribute(prize, "Ship.SeaTow");
		if (CheckAttribute(link, "LifeDay")) prize.LifeDay = link.LifeDay;
	}
	SeaTow_Remove(pchar, key);
	if (tugIndex >= 0 && tugIndex < TOTAL_CHARACTERS)
	{
		ref tug = GetCharacter(tugIndex);
		if (SeaTow_Count(pchar, tug) == 0 && CheckAttribute(tug, "Ship.SeaTow.LoadStamp"))
		{
			if (sti(tug.Ship.SeaTow.LoadStamp) == tugStamp) DeleteAttribute(tug, "Ship.SeaTow");
		}
	}
}

// LEO: Временный ПГГ остаётся независимым капитаном; нанятый постоянным офицером может буксировать.
bool SeaTow_CanTug(ref tug)
{
	if (IsMainCharacter(tug)) return true;
	if (!IsCompanion(tug)) return false;
	if (!CheckAttribute(tug, "PGGAi")) return true;
	if (!CheckAttribute(tug, "PGGAi.Officer")) return false;
	return sti(tug.PGGAi.Officer) != 0;
}

bool SeaTow_CanBegin(ref tug, ref prize)
{
	if (!bSeaActive || bSeaReloadStarted || bAbordageStarted) return false;
	if (!CheckAttribute(&AISea, "TowingVersion")) return false;
	if (sti(AISea.TowingVersion) < 2) return false;
	if (!SeaTow_CanTug(tug)) return false;
	if (!SeaTow_HasSeaShip(tug) || !SeaTow_HasSeaShip(prize)) return false;
	if (LAi_IsDead(tug) || LAi_IsDead(prize)) return false;
	if (!CheckAttribute(prize, "id")) return false;
	if (!HasSubStr(prize.id, "_DriftCap_")) return false;
	if (Boarding_Busy(tug) || Boarding_Busy(prize)) return false;
	if (Ship_GetDistance2D(tug, prize) > SEA_TOW_ATTACH_DISTANCE) return false;
	return true;
}

// LEO: Общая точка зацепа для ГГ, командлиста компаньонов и стратегического режима.
bool SeaTow_Begin(ref tug, ref prize)
{
	if (!SeaTow_CanBegin(tug, prize)) return false;
	if (!SeaTow_Attach(pchar, tug, prize)) return false;
	string key = "prize" + prize.index;
	aref link;
	makearef(link, pchar.SeaTow.Links.(key));
	if (CheckAttribute(prize, "LifeDay"))
	{
		link.LifeDay = prize.LifeDay;
		DeleteAttribute(prize, "LifeDay");
	}
	string group = "SeaTow_" + link.Prize.Stamp;
	aref endpoint;
	makearef(endpoint, link.Prize);
	if (CheckAttribute(prize, "SeaAI.Group.Name"))
	{
		if (SeaTow_GroupOwned(prize.SeaAI.Group.Name, endpoint)) group = prize.SeaAI.Group.Name;
	}
	int groupIndex = Group_FindGroup(group);
	if (groupIndex >= 0)
	{
		if (!SeaTow_GroupOwned(group, endpoint))
		{
			SeaTow_Forget(key);
			return false;
		}
	}
	else groupIndex = Group_CreateGroup(group);
	if (groupIndex < 0)
	{
		SeaTow_Forget(key);
		return false;
	}
	link.Group = group;
	SeaTow_BindGroup(group, endpoint);
	DeleteAttribute(&AIGroups[groupIndex], "DelAtEnd");
	Group_SetGroupCommander(group, prize.id);
	Group_ChangeCharacter(group, prize.id);
	Group_SetAddress(group, "none", "", "");
	ChangeCharacterShipGroup(prize, group);
	Group_SetTaskNone(group);
	Ship_SetTaskDrift(PRIMARY_TASK, sti(prize.index));
	Ship_SetTaskDrift(SECONDARY_TASK, sti(prize.index));
	SeaTow_Refresh();
	SeaTow_Schedule();
	SeaTowOperations_Attached(tug, prize);
	return true;
}

bool SeaTow_End(ref tug, ref prize)
{
	if (!SeaTow_IsTowed(pchar, prize)) return false;
	string key = "prize" + prize.index;
	aref endpoint;
	makearef(endpoint, pchar.SeaTow.Links.(key).Tug);
	if (!SeaTow_Matches(endpoint, tug)) return false;
	SeaTow_Forget(key);
	SeaTow_Refresh();
	return true;
}

float SeaTow_HullLength(ref chr)
{
	if (CheckAttribute(chr, "Ship.TowHull.Size.z")) return stf(chr.Ship.TowHull.Size.z);
	return stf(chr.Ship.BoxSize.z);
}

// LEO: Общая локальная точка для картинки и физики. Здесь можно настраивать крепления отдельных моделей.
void SeaTow_LocalAnchor(ref chr, bool stern, ref anchor)
{
	anchor.x = 0.0;
	anchor.y = SEA_TOW_ANCHOR_MIN_HEIGHT;
	anchor.z = 0.0;
	if (CheckAttribute(chr, "Ship.TowHull.Center.x")) anchor.x = chr.Ship.TowHull.Center.x;
	if (CheckAttribute(chr, "Ship.TowHull.Center.z")) anchor.z = chr.Ship.TowHull.Center.z;
	if (CheckAttribute(chr, "Ship.TowHull.Center.y"))
	{
		anchor.y = stf(chr.Ship.TowHull.Center.y) + stf(chr.Ship.TowHull.Size.y) * SEA_TOW_ANCHOR_HEIGHT;
		if (stf(anchor.y) < SEA_TOW_ANCHOR_MIN_HEIGHT) anchor.y = SEA_TOW_ANCHOR_MIN_HEIGHT;
		if (stf(anchor.y) > SEA_TOW_ANCHOR_MAX_HEIGHT) anchor.y = SEA_TOW_ANCHOR_MAX_HEIGHT;
	}
	float along = SeaTow_HullLength(chr) * SEA_TOW_ANCHOR_ALONG;
	if (stern) along = -along;
	anchor.z = stf(anchor.z) + along;
}

// LEO: Длина хранится в связи: снятие соседнего приза не дёргает оставшиеся канаты.
float SeaTow_Length(ref tug, ref prize)
{
	string key = "prize" + prize.index;
	aref current, links, link, endpoint;
	makearef(current, pchar.SeaTow.Links.(key));
	if (CheckAttribute(current, "Length")) return stf(current.Length);
	float ropeLength = (SeaTow_HullLength(tug) + SeaTow_HullLength(prize)) * 0.5 + SEA_TOW_ROPE_CLEARANCE;
	makearef(links, pchar.SeaTow.Links);
	for (int i = 0; i < GetAttributesNum(links); i++)
	{
		link = GetAttributeN(links, i);
		if (GetAttributeName(link) == key || !CheckAttribute(link, "Length")) continue;
		makearef(endpoint, link.Tug);
		if (!SeaTow_Matches(endpoint, tug)) continue;
		makearef(endpoint, link.Prize);
		if (!SeaTow_ValidEndpoint(endpoint)) continue;
		ref other = GetCharacter(sti(endpoint.Index));
		float behind = stf(link.Length) + (SeaTow_HullLength(other) + SeaTow_HullLength(prize)) * 0.5 + SEA_TOW_ROPE_CLEARANCE;
		if (behind > ropeLength) ropeLength = behind;
	}
	current.Length = ropeLength;
	return ropeLength;
}

void SeaTow_SetRuntime(ref tug, ref prize)
{
	if (!SeaTow_HasSeaShip(tug) || !SeaTow_HasSeaShip(prize)) return;
	string key = "prize" + prize.index;
	aref link;
	makearef(link, pchar.SeaTow.Links.(key));
	float centerLength = SeaTow_Length(tug, prize);
	object tugAnchor, prizeAnchor;
	SeaTow_LocalAnchor(tug, true, &tugAnchor);
	SeaTow_LocalAnchor(prize, false, &prizeAnchor);
	bool migrate = true;
	if (CheckAttribute(link, "RopeLayoutVersion")) migrate = sti(link.RopeLayoutVersion) != 1;
	if (migrate)
	{
		float ta = stf(tug.Ship.Ang.y);
		float pa = stf(prize.Ship.Ang.y);
		float dx = stf(tug.Ship.Pos.x) + cos(ta) * stf(tugAnchor.x) + sin(ta) * stf(tugAnchor.z);
		float dz = stf(tug.Ship.Pos.z) - sin(ta) * stf(tugAnchor.x) + cos(ta) * stf(tugAnchor.z);
		dx -= stf(prize.Ship.Pos.x) + cos(pa) * stf(prizeAnchor.x) + sin(pa) * stf(prizeAnchor.z);
		dz -= stf(prize.Ship.Pos.z) - sin(pa) * stf(prizeAnchor.x) + cos(pa) * stf(prizeAnchor.z);
		float restLength = sqrt(dx * dx + dz * dz) + SEA_TOW_ROPE_EXTRA_SLACK;
		float minimum = centerLength + stf(tugAnchor.z) - stf(prizeAnchor.z);
		if (restLength < minimum) restLength = minimum;
		if (CheckAttribute(link, "RopeLength"))
		{
			minimum = stf(link.RopeLength) + SEA_TOW_ROPE_EXTRA_SLACK;
			if (restLength < minimum) restLength = minimum;
		}
		// LEO: Переход на корпусные крепления и запас длины выполняется один раз, включая старые сейвы.
		link.RopeLength = restLength;
		link.Length = restLength - stf(tugAnchor.z) + stf(prizeAnchor.z);
		link.RopeLayoutVersion = 1;
		DeleteAttribute(prize, "Ship.SeaTow.TelemetryTime");
		DeleteAttribute(prize, "Ship.SeaTow.TensionKN");
		DeleteAttribute(prize, "Ship.SeaTow.SlackMeters");
	}
	prize.Ship.SeaTow.TugAnchor.x = tugAnchor.x;
	prize.Ship.SeaTow.TugAnchor.y = tugAnchor.y;
	prize.Ship.SeaTow.TugAnchor.z = tugAnchor.z;
	prize.Ship.SeaTow.PrizeAnchor.x = prizeAnchor.x;
	prize.Ship.SeaTow.PrizeAnchor.y = prizeAnchor.y;
	prize.Ship.SeaTow.PrizeAnchor.z = prizeAnchor.z;
	prize.Ship.SeaTow.PrizeId = prize.id;
	prize.Ship.SeaTow.PrizeHull = prize.Ship.Type;
	prize.Ship.SeaTow.PrizeStamp = pchar.SeaTow.Links.(key).Prize.Stamp;
	prize.Ship.SeaTow.TugIndex = tug.index;
	prize.Ship.SeaTow.TugId = tug.id;
	prize.Ship.SeaTow.TugHull = tug.Ship.Type;
	prize.Ship.SeaTow.PhysicsVersion = 2;
	prize.Ship.SeaTow.Length = link.RopeLength;
	// LEO: Вес из игровых CWT переводим в кг (1 CWT = 50 кг); опыт команды уже учтён парусной тягой.
	prize.Ship.SeaTow.TugMass = SeaTow_Mass(tug) * 50.0;
	prize.Ship.SeaTow.PrizeMass = SeaTow_Mass(prize) * 50.0;
	prize.Ship.SeaTow.AxialRigidity = SEA_TOW_AXIAL_RIGIDITY;
	prize.Ship.SeaTow.DampingRatio = SEA_TOW_DAMPING_RATIO;
	prize.Ship.SeaTow.MaxAcceleration = SEA_TOW_ACCELERATION;
	prize.Ship.SeaTow.MaxAngularAcceleration = SEA_TOW_ANGULAR_ACCELERATION;
	prize.Ship.SeaTow.AngularDamping = SEA_TOW_ANGULAR_DAMPING;
	prize.Ship.SeaTow.AddedMass = SEA_TOW_ADDED_MASS;
	prize.Ship.SeaTow.TelemetryInterval = SEA_TOW_TELEMETRY_INTERVAL;
	DeleteAttribute(prize, "Ship.SeaTow.Spring");
	DeleteAttribute(prize, "Ship.SeaTow.Damping");
	DeleteAttribute(prize, "Ship.SeaTow.MaxSpeed");
	prize.Ship.SeaTow.RopeColor = SEA_TOW_ROPE_COLOR;
	prize.Ship.SeaTow.RopeSag = SEA_TOW_ROPE_SAG;
	prize.Ship.SeaTow.RopeMaxSag = SEA_TOW_ROPE_MAX_SAG;
	prize.Ship.SeaTow.RopeMinSag = SEA_TOW_ROPE_MIN_SAG;
	prize.Ship.SeaTow.RopeSegments = SEA_TOW_ROPE_SEGMENTS;
	prize.Ship.SeaTow.SailToSlack = SEA_TOW_ROPE_EXTRA_SLACK;
	prize.Ship.SeaTow.SailToSamples = SEA_TOW_SAILTO_SAMPLES;
}

void SeaTow_Refresh()
{
	if (!SeaTow_HasLinks()) return;
	aref links, link, tugInfo, prizeInfo;
	makearef(links, pchar.SeaTow.Links);
	for (int i = GetAttributesNum(links) - 1; i >= 0; i--)
	{
		link = GetAttributeN(links, i);
		makearef(tugInfo, link.Tug);
		makearef(prizeInfo, link.Prize);
		bool valid = SeaTow_ValidEndpoint(tugInfo) && SeaTow_ValidEndpoint(prizeInfo);
		if (!CheckAttribute(link, "Group")) valid = false;
		else
		{
			// LEO: Имя могло перейти к квестовой группе после снятия старой сцепки.
			if (Group_FindGroup(link.Group) >= 0 && !SeaTow_GroupOwned(link.Group, prizeInfo)) valid = false;
		}
		if (valid)
		{
			ref tug = GetCharacter(sti(tugInfo.Index));
			ref prize = GetCharacter(sti(prizeInfo.Index));
			valid = !LAi_IsDead(tug) && !LAi_IsDead(prize);
			if (!SeaTow_CanTug(tug)) valid = false;
			if (!HasSubStr(prize.id, "_DriftCap_")) valid = false;
		}
		if (!valid)
		{
			string key = GetAttributeName(link);
			SeaTow_Forget(key);
		}
	}
	if (!SeaTow_HasLinks()) return;
	object prepared;
	for (i = 0; i < GetAttributesNum(links); i++)
	{
		link = GetAttributeN(links, i);
		tug = GetCharacter(sti(link.Tug.Index));
		prize = GetCharacter(sti(link.Prize.Index));
		string tugKey = "t" + tug.index;
		if (!CheckAttribute(&prepared, tugKey))
		{
			// LEO: Убираем сохранённый штраф старой версии, нагрузку теперь создаёт только натянутый трос.
			DeleteAttribute(tug, "Ship.SeaTow");
			prepared.(tugKey) = true;
		}
		if (SeaTow_HasSeaShip(tug) && SeaTow_HasSeaShip(prize)) SeaTow_SetRuntime(tug, prize);
	}
}

// LEO: Не ищем корабли в кадре: обновление реестра дважды в секунду, только пока есть буксир.
void SeaTow_Schedule()
{
	SeaTowTickGeneration++;
	DelEventHandler("SeaTowTick", "SeaTow_Tick");
	if (!bSeaActive || bSeaReloadStarted || bAbordageStarted || !SeaTow_HasLinks()) return;
	SetEventHandler("SeaTowTick", "SeaTow_Tick", 0);
	PostEvent("SeaTowTick", SEA_TOW_TICK_MS, "l", SeaTowTickGeneration);
}

void SeaTow_Tick()
{
	int generation = GetEventData();
	if (generation != SeaTowTickGeneration) return;
	DelEventHandler("SeaTowTick", "SeaTow_Tick");
	if (!bSeaActive || bSeaReloadStarted || bAbordageStarted) return;
	SeaTow_Refresh();
	SeaTow_Schedule();
}

void SeaTow_Suspend()
{
	SeaTowTickGeneration++;
	DelEventHandler("SeaTowTick", "SeaTow_Tick");
}

// LEO: В новом море призы создаются отдельно от энкаунтеров; при загрузке морского сейва сущности уже есть.
void SeaTow_Resume()
{
	if (!bSeaActive || bSeaReloadStarted || bAbordageStarted || !SeaTow_HasLinks()) return;
	if (!CheckAttribute(&AISea, "TowingVersion")) return;
	if (sti(AISea.TowingVersion) < 2) return;
	SeaTow_Refresh();
	if (!SeaTow_HasLinks()) return;
	aref links, link;
	makearef(links, pchar.SeaTow.Links);
	for (int i = 0; i < GetAttributesNum(links); i++)
	{
		link = GetAttributeN(links, i);
		ref tug = GetCharacter(sti(link.Tug.Index));
		ref prize = GetCharacter(sti(link.Prize.Index));
		if (!SeaTow_HasSeaShip(tug) || SeaTow_HasSeaShip(prize)) continue;
		if (iNumShips >= MAX_SHIPS_ON_SEA) continue;
		string group = link.Group;
		Group_SetGroupCommander(group, prize.id);
		if (Group_FindGroup(group) < 0) continue;
		Group_ChangeCharacter(group, prize.id);
		aref endpoint;
		makearef(endpoint, link.Prize);
		SeaTow_BindGroup(group, endpoint);
		float angle = stf(tug.Ship.Ang.y);
		float distance = SeaTow_Length(tug, prize);
		Group_SetXZ_AY(group, stf(tug.Ship.Pos.x) - sin(angle) * distance, stf(tug.Ship.Pos.z) - cos(angle) * distance, angle);
		Group_SetAddress(group, "none", "", "");
		prize.SeaAI.Group.Name = group;
		Ship_Add2Sea(sti(prize.index), true, "");
		Ship_SetTaskDrift(PRIMARY_TASK, sti(prize.index));
		Ship_SetTaskDrift(SECONDARY_TASK, sti(prize.index));
	}
	SeaTow_Refresh();
	SeaTow_Schedule();
}
