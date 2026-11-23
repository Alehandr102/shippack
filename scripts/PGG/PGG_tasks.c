// PGG_MakeHunter: turn a PGG into a hostile pirate hunter; returns sail duration in days
int PGG_MakeHunter(ref chr)
{
	chr.AlwaysEnemy = true;
	chr.PGG_Hunter = true;
	chr.DontRansackCaptain = true;
	chr.mapEnc.type = "war";
	chr.hunter = "pirate";
	chr.PGG_warrior = true;
	chr.nation_backup = chr.nation;
	chr.nation = PIRATE;
	DeleteAttribute(chr, "Abordage.Enable");
	DeleteAttribute(chr, "AlwaysFriend");
	return 15;
}

// LEO: снять следы охоты - обратная к PGG_MakeHunter для флагов, которые конец рейса не снимал:
//   PGG_Hunter/PGG_warrior гасились и раньше, а AlwaysEnemy, hunter и DontRansackCaptain висели вечно,
//   и побывавший охотником ПГГ до конца игры встречал ГГ залпом - даже когда сам звал в свою каюту.
//   Звать ДО удаления PGG_Hunter: по нему отличаем охотника. Нацию вернёт PGG_DailyUpdateEx по nation_backup,
//   обида живёт в PGGAi.relations, а не в вечном боевом флаге.
void PGG_EndHuntTraces(ref chr)
{
	if (!CheckAttribute(chr, "PGG_Hunter")) return;
	DeleteAttribute(chr, "AlwaysEnemy");
	DeleteAttribute(chr, "hunter");
	DeleteAttribute(chr, "DontRansackCaptain");
	if (CheckAttribute(chr, "mapEnc.type"))
	{
		if (chr.mapEnc.type == "war") DeleteAttribute(chr, "mapEnc.type");
	}
}

// LEO: единый гейт «ПГГ выходит в охотники за ГГ».
// баг1: нападать только если ГГ богат И у ПГГ дурная репутация И его корабль не слабее ГГ.
// баг2: не трогать ПГГ, занятого поручением игрока (квест ПП — PChar.GenQuest.PGG_Quest.PGGid).
bool PGG_ShouldBecomeHunter(ref chr)
{
	if (CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid"))
	{
		if (PChar.GenQuest.PGG_Quest.PGGid == chr.id) return false; // этот ПГГ занят поручением игрока
	}
	if (GetCharacterShipClass(chr) > GetCharacterShipClass(PChar)) return false; // его корабль слабее — не суётся
	return true;                                                                 // деньги/репу/отношение проверяют сами ветки (как в оригинале)
}

//==================================================
// LEO: ТУРНИР АРЕНЫ — отбор и отправка участников
//==================================================
// Свободен ли ПГГ, чтобы ехать драться на арену.
bool PGG_IsFreeForTournament(ref chr)
{
	if (!CheckAttribute(chr, "PGGAi")) return false;
	if (chr.PGGAi.location == "Dead") return false;      // лимб гибели/возрождения
	if (IsCompanion(chr)) return false;
	if (IsOfficer(chr)) return false;
	if (CheckAttribute(chr, "PGG_Tournament")) return false;   // уже едет на этот турнир
	// ⚠️ только что отыгравшего НЕ звать снова: отбор идёт в той же генерации, что и роспуск, а стоит он у
	//   самой Доминики - вместо отъезда домой он просто перепарковывался, и бухта не пустела никогда.
	if (CheckAttribute(chr, "PGG_ArenaRest.control_year"))
	{
		if (GetNpcQuestPastDayParam(chr, "PGG_ArenaRest") < PGG_ARENA_REST_DAYS) return false;
	}
	if (CheckAttribute(chr, "PGG_Hunter")) return false;       // охотится за ГГ, ему не до арены
	if (CheckAttribute(chr, "PGGAi.DontUpdate")) return false;
	if (sti(chr.Ship.Type) == SHIP_NOTUSED) return false;      // без корабля до Доминики не доберётся
	if (CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid"))
	{
		if (PChar.GenQuest.PGG_Quest.PGGid == chr.id) return false; // занят поручением игрока
	}
	// LEO: назначенного охотником за кладом ГГ тоже не зовём. Кандидата выбирают при находке карты, а
	//   сцена идёт позже: уехав на турнир, он приходил за кладом неприкасаемым и не получал урона.
	if (CheckAttribute(PChar, "chosenTreasureHero"))
	{
		if (PChar.chosenTreasureHero == chr.id) return false;
	}
	return true;
}

// Отправить на турнир 1..PGG_ARENA_MAX_FIGHTERS свободных ПГГ. Возвращает, сколько реально поехало.
int PGG_SendToTournament()
{
	int i, n;
	ref chr;
	int aFree[100];
	int iFree = 0;
	pchar.PGG_TournamentSlot = 0;   // новая партия - раздаём места в бухтах с начала
	for (i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);
		if (PGG_IsFreeForTournament(chr))
		{
			aFree[iFree] = i;
			iFree++;
		}
	}
	if (iFree < 1) return 0;

	int iWant = 1 + rand(PGG_ARENA_MAX_FIGHTERS - 1);
	if (iWant > iFree) iWant = iFree;

	int iSent = 0;
	int iTry  = 0;
	while (iSent < iWant)
	{
		iTry++;
		if (iTry > 30) break;                       // страховка от зацикливания на уже выбранных
		n = rand(iFree - 1);
		chr = CharacterFromID("PsHero_" + aFree[n]);
		if (CheckAttribute(chr, "PGG_Tournament")) continue;

		chr.PGG_Tournament = true;
		chr.PGGAi.Task = PGG_TASK_ARENA;
		DeleteAttribute(chr, "PGGAi.Task.DaysCounter");
		PGG_SetUpForTask(chr);
		PGG_UpdateStats(chr, "Task");
		// PGG_SetUpForTask мог увести на сухопутную задачу (лимит ПГГ в море) - тогда он не участвует
		if (chr.PGGAi.Task != PGG_TASK_ARENA)
		{
			DeleteAttribute(chr, "PGG_Tournament");
			continue;
		}
		iSent++;
	}
	Log_TestInfo("ТУРНИР: свободных ПГГ " + iFree + ", отправлено " + iSent);
	return iSent;
}

// LEO: ПОСТАВИТЬ КОРАБЛЬ ПГГ В БУХТУ ДОМИНИКИ.
// У острова есть готовые группы стояночных локаторов Reload_1 и Reload_2, по шесть мест в каждой
// (ship_1..ship_6) - тем же способом игра расставляет квестовые эскадры и охрану колоний.
// Корабль ставится на якорь (дрейф + снятая задача) и делается неприкасаемым: пока идёт турнир, он не
// должен ни тонуть, ни драться, ни реагировать на смену флага. Всё снимает PGG_UnparkFromTournament.
void PGG_ParkAtTournament(ref chr)
{
	if (CheckAttribute(chr, "PGG_Tournament.Parked")) return;

	// раздаём места по кругу: сначала первая бухта, потом вторая
	int iSlot = 0;
	if (CheckAttribute(pchar, "PGG_TournamentSlot")) iSlot = sti(pchar.PGG_TournamentSlot);
	pchar.PGG_TournamentSlot = iSlot + 1;
	string sBay = PGG_ARENA_BAY1;
	int iPlace = iSlot;
	if (iPlace >= PGG_ARENA_BAY_PLACES)
	{
		sBay = PGG_ARENA_BAY2;
		iPlace = iPlace - PGG_ARENA_BAY_PLACES;
	}
	if (iPlace >= PGG_ARENA_BAY_PLACES) iPlace = PGG_ARENA_BAY_PLACES - 1; // мест меньше, чем гостей - жмёмся

	string sGroup = "Sea_" + chr.id;
	Group_FindOrCreateGroup(sGroup);
	Group_SetAddress(sGroup, PGG_ARENA_ISLAND, sBay, "ship_" + (iPlace + 1));
	Ship_SetTaskDrift(SECONDARY_TASK, sti(chr.index));
	Group_SetTaskNone(sGroup);
	Group_LockTask(sGroup);

	// неприкасаемость на время турнира
	LAi_SetImmortal(chr, true);            // урон по кораблю гасится в AIShip.c по этому же признаку
	chr.ShipTaskLock = true;               // задачу извне не перебить
	chr.DontRansackCaptain = true;         // не сдаётся
	chr.SinkTenPercent = false;            // не тонет и не удирает

	// ВНЕ ПОЛИТИКИ. На стоянке нация и флаг значения не имеют: гости турнира никого не трогают и их
	//   не трогают. Нейтралим к ГГ и ко всей его эскадре (иначе компаньоны сами полезут в драку),
	//   вешаем «не обижаться на выстрелы» и AlwaysFriend - он исключает корабль из пересчёта отношений
	//   по нации и флагу (AIShip.c: такие пропускаются), поэтому смена флага игроком их не поднимет.
	//   Прежнее состояние запоминаем, чтобы вернуть в точности.
	if (CheckAttribute(chr, "AlwaysEnemy")) chr.PGG_Tournament.WasEnemy = true;
	DeleteAttribute(chr, "AlwaysEnemy");
	chr.AlwaysFriend = true;
	chr.ShipEnemyDisable = true;
	Ship_MakeNeutralToPlayerFleet(sti(chr.index));

	// ...и ДРУГ С ДРУГОМ. Нации у гостей турнира разные, поэтому без этого испанец с англичанином
	//   устраивали побоище прямо на стоянке. Нейтралим с каждым, кто уже стоит в бухте; те, кто
	//   подойдёт позже, помирятся с нами сами при своей парковке.
	ref other;
	for (int i = 1; i <= PsHeroQty; i++)
	{
		other = CharacterFromID("PsHero_" + i);
		if (other.id == chr.id) continue;
		if (!CheckAttribute(other, "PGG_Tournament.Parked")) continue;
		SetCharacterRelationBoth(sti(chr.index), sti(other.index), RELATION_NEUTRAL);
	}

	chr.PGG_Tournament.Parked = true;
	// ⚠️ ДУБЛЬ признака ВНЕ ветки турнира: если ветку снесут мимо распарковки (предохранитель, гибель,
	//   чужая правка), по этому флагу страховка в дневном апдейте всё равно найдёт брошенный корабль.
	chr.PGG_ParkedAtArena = true;
	PGG_SeatInTournament(chr);   // и сразу занимает место в сетке - он успел к боям
}

// LEO: вырвать ПГГ из турнира арены перед тем, как отдать его сухопутной сцене квеста.
//   Парковка в бухте Доминики вешает на ПЕРСОНАЖА общий LAi_SetImmortal, и в наземном бою он работает
//   так же - конкурент не получал урона ни клинком, ни выстрелом (репорты по кладу и по «Клану Ламбрини»).
//   Зовётся из всех мест, где квест забирает ПГГ себе: место в сетке освобождается, корабль уходит
//   со стоянки, неприкасаемость снимается. Если ПГГ на турнире не был - остаётся только снятие immortal.
void PGG_ReleaseFromTournamentForQuest(ref chr)
{
	if (CheckAttribute(chr, "PGG_Tournament"))
	{
		PGG_FreeTournamentSeat(chr);
		PGG_ClearTournament(chr);
	}
	LAi_SetImmortal(chr, false);
}

// Снять корабль со стоянки: вернуть всё, что выставила парковка.
void PGG_UnparkFromTournament(ref chr)
{
	// гейт по ВНЕШНЕМУ признаку: внутренний PGG_Tournament.Parked исчезает вместе с веткой турнира
	if (!CheckAttribute(chr, "PGG_ParkedAtArena")) return;
	DeleteAttribute(chr, "PGG_ParkedAtArena");
	DeleteAttribute(chr, "PGG_Tournament.Parked");
	LAi_SetImmortal(chr, false);
	DeleteAttribute(chr, "ShipTaskLock");
	DeleteAttribute(chr, "DontRansackCaptain");
	chr.SinkTenPercent = true;
	// возвращаем в политику: снимаем стояночный нейтралитет и восстанавливаем прежнюю враждебность
	DeleteAttribute(chr, "ShipEnemyDisable");
	DeleteAttribute(chr, "AlwaysFriend");
	if (CheckAttribute(chr, "PGG_Tournament.WasEnemy"))
	{
		chr.AlwaysEnemy = true;
		DeleteAttribute(chr, "PGG_Tournament.WasEnemy");
	}
	DeleteAttribute(chr, "CheckFlagYet");   // пусть распознавание по флагу пройдёт заново
	string sGroup = "Sea_" + chr.id;
	Group_UnlockTask(sGroup);
	Group_SetAddressNone(sGroup);
	// ⚠️ смена адреса убирает корабль только со СЛЕДУЮЩЕЙ загрузки акватории. Если игрок сейчас в море у
	//   Доминики, снятые продолжали бы стоять у него на глазах - уводим их своим ходом и помечаем группу
	//   на удаление.
	if (!bSeaActive) return;                  // не в море - кораблей в мире нет, уводить нечего
	if (Group_FindGroup(sGroup) < 0) return;  // и группы нет: не плодить пустую через FindOrCreate
	Group_SetTaskRunaway(sGroup, PLAYER_GROUP);
	Group_DeleteAtEnd(sGroup);
}

// LEO: турнирный цикл закончился - распустить ВСЕХ участников. Зовётся, когда турнир отыгран (игроком или
// абстрактно) и при генерации нового. Иначе гости уходят лишь по 30-дневному предохранителю в дневном
// апдейте, то есть лишний месяц занимают бухту неуязвимыми кораблями.
void PGG_CloseTournamentForAll()
{
	int i;
	ref chr;
	for (i = 1; i <= PsHeroQty; i++)
	{
		if (GetCharacterIndex("PsHero_" + i) < 0) continue;
		chr = CharacterFromID("PsHero_" + i);
		if (!CheckAttribute(chr, "PGG_Tournament")) continue;
		PGG_ClearTournament(chr);
		// ⚠️ задачу НЕ отдаём случайному выбору: PGG_SelectTask может выдать сухопутное дело, и тогда
		//   конкурент числится «в городе», а с необитаемой Доминики никуда не плывёт. Отсюда только морем,
		//   рейс стартует от Shore27 (метку PGG_SailFrom поставил PGG_ClearTournament).
		if (sti(chr.Ship.Type) != SHIP_NOTUSED)
		{
			chr.PGG_ArenaReturn = true;   // поблажки гейтам выхода в море - он ОБЯЗАН уплыть с острова
			chr.PGGAi.Task = PGG_TASK_SAILTOISLAND;
			PGG_SetUpForTask(chr);
			PGG_UpdateStats(chr, "Task");
			DeleteAttribute(chr, "PGG_ArenaReturn");
		}
		else PGG_SelectTask(chr);   // без корабля уплыть не на чем - пусть решает общий выбор
	}
}

// Снять с турнира: вернуть ПГГ к обычной жизни. Зовётся и по завершении турнира, и когда не успел.
void PGG_ClearTournament(ref chr)
{
	if (!CheckAttribute(chr, "PGG_Tournament")) return;
	// ⚠️ ПОРЯДОК КРИТИЧЕН: и стоянка (.Parked), и признак прибытия (.Arrived) лежат ВНУТРИ ветки
	//   PGG_Tournament. Снесёшь ветку первой строкой - распарковка выйдет по своему гейту, и корабль
	//   останется в бухте навсегда, бессмертный. Сначала читаем и снимаем, ветку удаляем последней.
	PGG_UnparkFromTournament(chr);   // снять стоянку и неприкасаемость - ОБЯЗАТЕЛЬНО до выхода в море
	// следующий рейс начинается ОТ ДОМИНИКИ, а не из города приписки, иначе корабль возникнет у старого
	//   порта (телепорт). Метку подхватит и снимет PGG_SetUpForTask при создании энкаунтера.
	if (CheckAttribute(chr, "PGG_Tournament.Arrived")) chr.PGG_SailFrom = PGG_ARENA_LOCATOR;
	DeleteAttribute(chr, "PGG_Tournament");
	// страховка: если город всё-таки успел стать несуществующим (Доминика колонией не является),
	//   возвращаем ПГГ в нормальную колонию - иначе всё, что ищет по городу, будет спотыкаться
	if (FindColony(chr.PGGAi.location.town) == -1)
	{
		chr.PGGAi.location.town = PGG_FindRandomTown(chr);
	}
	// задачу именно УДАЛЯЕМ, как это делают все штатные завершения рейса. Если оставить строку-заглушку,
	// дневной апдейт не подберёт под неё ни одной ветки, и ПГГ зависнет без дела до следующего вмешательства.
	DeleteAttribute(chr, "PGGAi.Task");
	// отметка «был на турнире»: следующий отбор его пропустит, пока не отдохнёт PGG_ARENA_REST_DAYS дней
	SaveCurrentNpcQuestDateParam(chr, "PGG_ArenaRest");
}

//==================================================
// Tasks
//==================================================
//выбрать задание для ПГГ
void PGG_SelectTask(ref chr)
{
	ref rShip;
	int iRnd = -1;
	int iShipType = sti(chr.Ship.Type);
	//если нет корабля, то сидим в таверне, бухаем...
	if (iShipType == SHIP_NOTUSED)
	{
		if (rand(100) < 30)
		{
			chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
		}
		else
		{
			chr.PGGAi.Task = PGG_TASK_WAITINTAVERN;
		}
	}
	else iRnd = rand(6);

	switch(iRnd)
	{
	case 0:
		chr.PGGAi.Task = PGG_TASK_SAILTOISLAND;
		break;
	case 1:
		if(rand(1) == 0)
		{
			chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
			PGG_LockService(chr, "Store");
		}
		else chr.PGGAi.Task = PGG_TASK_SAILTOISLAND;
		break;
	//залочить сервис
	case 2:
		chr.PGGAi.Task = PGG_TASK_LOCKSERVICE;
		PGG_LockService(chr, "");
		break;
	//конвой
	case 3:
		chr.PGGAi.Task = PGG_TASK_WORKCONVOY;
		break;
	//на магазин
	case 4:
		chr.PGGAi.Task = PGG_TASK_WORKONSTORE;
		break;
	//контра
	case 5:
		chr.PGGAi.Task = PGG_TASK_WORKWITHCONTRA;
		chr.PGGAi.Task.DaysCounter = 1;
		PGG_LockService(chr, "Smugglers");
		break;
	//напасть на колонию (НЕДОДЕЛКА: задумывался отдельный таск "атака на колонию", сейчас это копия WorkOnMayor+Store)
	case 6:
		chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
		PGG_LockService(chr, "Store");
		break;
	}

	PGG_SetUpForTask(chr);
	PGG_UpdateStats(chr, "Task");

	//15 процентов иначе офигеем от обилия слухов :)
	if (rand(100) < 15)
	{
		PGG_AddRumour(chr, chr.PGGAi.Task);
	}
}

//вынес отдельным методом, чтобы можно было из разных мест вызывать, чаще будет.
//если передавать пустую строку, то будет рандомно выбираться...
void PGG_LockService(ref chr, string _service)
{
	if (_service != "")
	{
		chr.PGGAi.LockService = _service;
		return;
	}

	if (rand(1) == 1)
	{
		chr.PGGAi.LockService = "Shipyard";
	}
	else
	{
		chr.PGGAi.LockService = "Store";
	}
}

//Настроить параметры ПГГ под задание (корабль, команду, трюм и тд)
void PGG_SetUpForTask(ref chr)
{
	if (CheckAttribute(chr,"DeckDialogNode") && chr.DeckDialogNode == "Quest_1_Ship") return; // - Фикс "защита от дурака" когда при взятии ПП в таверне, можно было случайно выйти на глобалку, а потом уже на корабль к ПГГ - то квест рестартовался и перезапускался
	int iTradeGoods, iQuantityGoods, iMoney;
	string sTask = chr.PGGAi.Task;

	if (sTask == PGG_TASK_WORKONSTORE || sTask == PGG_TASK_SAILTOISLAND || sTask == PGG_TASK_WORKCONVOY || sTask == PGG_TASK_ARENA)
	{
		if (!CheckAttribute(pchar, "PGGsAtSea"))
		{
			pchar.PGGsAtSea = 0;
		}
		if (CheckAttribute(chr, "PGGAi.Task.SetSail"))
		{
			DeleteAttribute(chr, "PGGAi.Task.SetSail");
			DeleteAttribute(chr, "PGG_Hunter");
			DeleteAttribute(chr, "PGG_Convoy");
			DeleteAttribute(chr, "PGG_trader"); // LEO: снимаем и флаг торговца при пересетапе задачи (забыли рядом с Hunter/Convoy) -> иначе он залипает с прошлого конвоя и таблица врёт «сопровождает торговца» на след. НЕ-конвойном сейле (плывёт-то один)
		}
		if (!CheckAttribute(pchar, "LastPgg"))
		{
			pchar.LastPgg = "";
		}
		// LEO: гейты выхода в море. Турнирному рейсу прощаем «плавал в прошлый раз» и послерейсовый счётчик —
		//   иначе ПГГ почти никогда не доберётся до арены (турнир раз в 20-25 дней, а эти гейты постоянные).
		//   Лимит одновременных ПГГ в море соблюдаем всегда: он про нагрузку на глобалку.
		//   Условия разнесены во вложенные if намеренно — Storm плохо переваривает смесь && и ||.
		// LEO: рейс ОБРАТНО с турнира приравниваем к турнирному - те же поблажки. Иначе получалось так:
		//   конкурент только что завершил рейс, поэтому у него стоит послерейсовый счётчик, гейт даёт
		//   bNoSail, и вместо отплытия ему выдают сухопутное дело («новая задача WorkOnMayor/LockService»).
		//   А сидит он при этом на НЕОБИТАЕМОЙ Доминике - в город не попадёт и с острова не уплывёт никогда.
		bool bArenaTrip = false;
		if (sTask == PGG_TASK_ARENA) bArenaTrip = true;
		if (CheckAttribute(chr, "PGG_ArenaReturn")) bArenaTrip = true;

		bool bNoSail = false;
		if (bArenaTrip)
		{
			// турнирным даём отдельный запас поверх общего лимита: их максимум PGG_ARENA_MAX_FIGHTERS и
			// едут они раз в два-три десятка дней, а на общем лимите они почти всегда проигрывали гонку
			// обычным рейсам и на арену не попадал никто (репорт: «отправлено 0»)
			if (sti(pchar.PGGsAtSea) >= PGG_WORLD_MAP_MAX_AT_SEA + PGG_ARENA_MAX_FIGHTERS) bNoSail = true;
		}
		else
		{
			if (sti(pchar.PGGsAtSea) >= PGG_WORLD_MAP_MAX_AT_SEA) bNoSail = true;
		}
		if (!bArenaTrip)
		{
			if (pchar.LastPgg == chr.id) bNoSail = true;
			if (CheckAttribute(chr, "PGGAi.AfterSail.TaskCounter")) bNoSail = true;
		}
		if (bNoSail)
		{//Не пускаем в море слишком много ПГГ
			DeleteAttribute(pchar, "LastPgg");

			int iRnd = rand(3);
			switch(iRnd)
			{//Выбираем сухопутный таск
			case 0:
				chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
				chr.PGGAi.Task.DaysCounter = 2;
				PGG_LockService(chr, "Store");
				break;
			//залочить сервис
			case 1:
				chr.PGGAi.Task = PGG_TASK_LOCKSERVICE;
				chr.PGGAi.Task.DaysCounter = 2;
				PGG_LockService(chr, "");
				break;
			//контра
			case 2:
				chr.PGGAi.Task = PGG_TASK_WORKWITHCONTRA;
				chr.PGGAi.Task.DaysCounter = 2;
				PGG_LockService(chr, "Smugglers");
				break;
			//напасть на колонию (НЕДОДЕЛКА: задумывался отдельный таск "атака на колонию", сейчас это копия WorkOnMayor+Store)
			case 3:
				chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
				chr.PGGAi.Task.DaysCounter = 2;
				PGG_LockService(chr, "Store");
				break;
			}

			if (CheckAttribute(chr, "PGGAi.AfterSail.TaskCounter"))
			{
				int aftersailTasks = sti(chr.PGGAi.AfterSail.TaskCounter) + 1
				chr.PGGAi.AfterSail.TaskCounter = aftersailTasks;

				if (sti(chr.PGGAi.AfterSail.TaskCounter) >= PGG_AFTERSAIL_TASK_LIMIT)
				{
					DeleteAttribute(chr, "PGGAi.AfterSail.TaskCounter");
				}
			}
		}
		else
		{
			PGG_UpdateShipEquip(chr);
			//убираем из таверны, уплыл нафиг :)
			PGG_PlaceCharacter2Tavern(chr, false);

			iTradeGoods = rand(GOOD_SILVER);
			RecalculateSquadronCargoLoad(chr);
			iQuantityGoods = GetSquadronFreeSpace(chr, iTradeGoods);
			iQuantityGoods = iQuantityGoods - rand(makeint(iQuantityGoods/3)) - 10;
			// 1.2.3 Это лишние миллионы в ПГГ
			iMoney = makeint((iQuantityGoods / sti(Goods[iTradeGoods].Units)) * sti(Goods[iTradeGoods].Cost) * GetSummonSkillFromNameToOld(chr, SKILL_COMMERCE)/8);

			if (iQuantityGoods < 0) iQuantityGoods = 0;
			AddCharacterGoods(chr, iTradeGoods, iQuantityGoods);
			PGG_AddMoneyToCharacter(chr, iMoney);

			//а это такая засада, ;) убираем из магазина товары. -->
			AddRemoveGoodsInStore(chr.PGGAi.location.town, iTradeGoods, iQuantityGoods, false);
			//<--
			if (sTask == PGG_TASK_ARENA) PGG_SetTournamentTarget(chr); // арена на необитаемом острове, обычный поиск колонии её не вернёт
			else PGG_FindTargetTown(chr);
			chr.PGGAi.Task.Target.Goods = iTradeGoods;
			chr.PGGAi.Task.Target.Goods.Qty = iQuantityGoods;

			chr.PGGAi.Encountername = chr.id;
			Map_ReleaseQuestEncounter(chr.id);
			string  sGroup = "Sea_" + chr.id;
			group_DeleteGroup(sGroup);
			Group_FindOrCreateGroup(sGroup);
			PGG_ClearCompanions(chr); // LEO: старт нового рейса -> обнулить протухших компаньонов ПЕРЕД созданием нового трейдера (страховка для старых сейвов с уже висячим индексом; трейдер создаётся ниже, ~258)
			Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
			Group_LockTask(sGroup);
			Group_AddCharacter(sGroup, chr.id);
			Group_SetGroupCommander(sGroup, chr.id);

			if (chr.sex != "woman")
			{
				chr.mapEnc.worldMapShip = "PGGShipMan";
			}
			else
			{
				chr.mapEnc.worldMapShip = "PGGShipWoman";
			}
			chr.mapEnc.Name = GetHeroName(sti(chr.PGGAi.HeroNum));
			string sColony = chr.PGGAi.Task.Target;
			int daysQty;
			// LEO: срок пути. Обычный рейс считает его через АРЕАЛ ГОРОДА назначения, но цель турнира -
			//   Доминика, а это остров без колонии: GetArealByCityName вернёт мусор, срок выйдет кривой,
			//   и энкаунтер снесётся почти сразу - корабля на карте так и не увидишь (репорт юзера).
			//   Для турнира берём срок, посчитанный в PGG_SetTournamentTarget по расстоянию до соседней колонии.
			if (chr.PGGAi.Task == PGG_TASK_ARENA)
			{
				daysQty = sti(chr.PGGAi.Task.Target.days);
				if (daysQty < 3) daysQty = 3;
			}
			else
			{
				daysQty = 3 + GetMaxDaysFromIsland2Island(GetArealByCityName(sColony), GetArealByCityName(chr.PGGAi.location.town));
			}
			int iGoods;
			int iSpace;

			if (PGG_ShouldBecomeHunter(chr) && PGG_ChangeRelation2MainCharacter(chr, 0) <= PGG_REL_FRIENDLY && sti(chr.reputation) < PGG_REP_LOW && sti(pchar.money) >= sti(chr.rank)*20000 + 250000 && rand(100) < 5)
			{// ОРИГИНАЛ: злодей (реп<20) при отношении <=50 + богатый ГГ + корабль не слабее -> 5% ШАНС (было 1%, поднято по просьбе юзера 20.07)
				daysQty = PGG_MakeHunter(chr);
			}
			else
			{
				if (PGG_ShouldBecomeHunter(chr) && PGG_ChangeRelation2MainCharacter(chr, 0) < PGG_REL_HOSTILE)
				{// ОРИГИНАЛ: плохое отношение (<41) -> ГАРАНТИРОВАННО (репа/деньги не смотрятся; корабль не слабее — в PGG_ShouldBecomeHunter)
					daysQty = PGG_MakeHunter(chr);
				}
				else
				{
					chr.mapEnc.type = "trade";
					SetCharacterRelationBoth(sti(PChar.index), sti(chr.index), RELATION_FRIEND);
					DeleteAttribute(chr, "Abordage.Enable");
					if (chr.PGGAi.Task == PGG_TASK_WORKCONVOY && sti(chr.nation) != PIRATE && !CheckAttribute(chr, "PGG_warrior"))
					{
						sld = GetCharacter(NPC_GenerateCharacter(chr.id+"_Trader", "", "man", "man", sti(chr.rank), sti(chr.nation), daysQty, true));
						CreatePGG_Trade(sld, sti(chr.nation), chr);
						SetRandomNameToShip(sld);
						SetBaseShipData(sld);
						SetCrewQuantityFull(sld);
						Fantom_SetCannons(sld, "trade");
						Fantom_SetBalls(sld, "trade");
						SetSeaFantomParam(sld, "trade");
						SetCaptanModelByEncType(sld, "trade");
						sld.PGG_companion = true;//Чтобы не было диалога
						ResetShipCannonsDamages(sld);
						SetBaseShipData(sld);
						SetCrewQuantityFull(sld);
						Fantom_SetBalls(sld, "pirate");
						SetBaseShipData(sld);
						SetCompanionIndex(chr, -1, sti(sld.index)); // LEO: торговец-эскорт = компаньон ПГГ -> грузится 2-м кораблём в ALONE-энкаунтере (путь загрузки добирает свиту через GetCompanionIndex; иначе в бою ПГГ один, хотя в таблице «сопровождает торговца»)
						DeleteAttribute(chr, "AlwaysFriend");
						SetCharacterRelationBoth(sti(PChar.index), sti(sld.index), RELATION_FRIEND);
						//Положим товар
						iGoods = GOOD_SLAVES + rand(2);
						iSpace = GetCharacterFreeSpace(sld, iGoods);
						iSpace = MakeInt(iSpace/2 + rand(iSpace/2));
						Fantom_SetCharacterGoods(sld, iGoods, iSpace, 1);
						Group_AddCharacter(sGroup, sld.id);
						Log_TestInfo(GetHeroName(sti(chr.PGGAi.HeroNum))+GetConvertStrWithReplace("Variable_PsHero_8", "Logs.txt", "#space#", " "));
						chr.mapEnc.Name = GetConvertStrWithReplace("Variable_PsHero_1", "Names.txt", "#space#", " ") + GetHeroName(sti(chr.PGGAi.HeroNum));
						chr.PGG_Convoy = true;
						chr.PGG_trader = true;
					}
					if (PGG_ShouldBecomeHunter(chr) && sti(chr.reputation) < PGG_REP_LOW && false)
					{//баг1: грудж-конвой уходит в охотники по тем же правилам (богатый ГГ + дурная репа ПГГ)
						DeleteAttribute(chr, "AlwaysFriend");
						DeleteAttribute(chr, "Abordage.Enable");
						chr.PGG_Hunter = true;
						chr.PGG_Convoy = true;
					}
					if (false) // LEO: МЕСТЬ за убитых ПГГ ОТКЛЮЧЕНА по просьбе юзера (душнит; игра не про это). Было: PGG_killed>4 && rand>50 // LEO: ветка мести теперь гейтит репу/отношение (как ветка 288) - Линкольн rep75 больше не охотится
					{
						chr.PGG_Hunter = true;
					}
				}
			}
			if (CheckAttribute(chr, "PGG_Hunter"))
			{
				int pgg_hunters = sti(chr.rank)/15 + 1;
				int pgg_hunters_bonus = 1;
				if (MOD_SKILL_ENEMY_RATE == 3)
				{
					pgg_hunters_bonus = 2;
				}
				if (MOD_SKILL_ENEMY_RATE < 3)
				{
					pgg_hunters_bonus = 0;
				}
				chr.mapEnc.Name = chr.mapEnc.Name + GetConvertStrWithReplace("Variable_PsHero_2", "Names.txt", "#space#", " ");
				int pgg_hunters_total = pgg_hunters+pgg_hunters_bonus;
				if (!CheckAttribute(pchar, "PGG_grudge") && CheckAttribute(pchar, "PGG_killed") && sti(pchar.PGG_killed) > 4)
				{
					pgg_hunters_total = 1 + rand(pgg_hunters_total/2);
				}
				for (int i = 1; i <= pgg_hunters_total; i++)
				{
					sld = GetCharacter(NPC_GenerateCharacter(chr.id+"_hunter_"+i, "", "man", "man", sti(chr.rank), sti(chr.nation), daysQty, true));
					CreatePGG_War(sld, sti(chr.nation), chr);
					SetRandomNameToShip(sld);
					SetBaseShipData(sld);
					SetCrewQuantityFull(sld);
					Fantom_SetCannons(sld, "war"); //fix
					Fantom_SetBalls(sld, "pirate");
					Fantom_SetGoods(sld, "war");
					sld.PGG_companion = true;//Чтобы не было диалога
					SetRandomNameToShip(sld);
					SetFantomParamHunter(sld); //крутые парни
					SetCaptanModelByEncType(sld, "war");//
					// LEO баг3: свита охотника — такие же пираты-враги, как сам ПГГ (единые нация/флаг/отношение).
					// Раньше ставился RELATION_FRIEND, а AlwaysEnemy только для не-конвоя -> свита выходила «вперемешку».
					sld.nation = PIRATE;
					DeleteAttribute(sld, "AlwaysFriend");
					sld.AlwaysEnemy = true;
					SetCharacterRelationBoth(sti(PChar.index), sti(sld.index), RELATION_ENEMY);
					//Положим товар
					iGoods = GOOD_SLAVES + rand(2);
					iSpace = GetCharacterFreeSpace(sld, iGoods);
					iSpace = rand(MakeInt(iSpace/10));
					Fantom_SetCharacterGoods(sld, iGoods, iSpace, 1);

					Group_AddCharacter(sGroup, sld.id);
				}
				// LEO баг3: сам ПГГ — тоже пират-враг. Пути мести (PGG_killed>4) и грудж-конвоя не звали
				// PGG_MakeHunter, из-за чего ПГГ оставался союзником при враждебной свите.
				chr.AlwaysEnemy = true;
				if (!CheckAttribute(chr, "nation_backup")) chr.nation_backup = chr.nation;
				chr.nation = PIRATE;
				chr.mapEnc.type = "war";
				DeleteAttribute(chr, "AlwaysFriend");
				SetCharacterRelationBoth(sti(PChar.index), sti(chr.index), RELATION_ENEMY);
				// торговый спутник конвоя, если был — тоже пират-враг
				int iTraderIdx = GetCharacterIndex(chr.id + "_Trader");
				if (iTraderIdx >= 0)
				{
					ref rTrader = GetCharacter(iTraderIdx);
					rTrader.nation = PIRATE;
					DeleteAttribute(rTrader, "AlwaysFriend");
					rTrader.AlwaysEnemy = true;
					SetCharacterRelationBoth(sti(PChar.index), iTraderIdx, RELATION_ENEMY);
				}
			}
			chr.PGGAi.Task.Target.days = daysQty;
			chr.PGGAi.Task.days = daysQty;
			chr.PGGAi.ActiveQuest = true;
			chr.DeckDialogNode = "Second time";
			if (CheckAttribute(chr, "PGG_warrior"))
			{
				Map_CreateWarrior("", chr.id, daysQty);
				Log_TestInfo(GetHeroName(sti(chr.PGGAi.HeroNum))+GetConvertStrWithReplace("Variable_PsHero_9", "Logs.txt", "#space#", " "));
			}
			else
			{
				DeleteAttribute(chr, "PGG_warrior");
				// hunter without PGG_warrior (low rep / revenge) -> war encounter on map, not trade
				if (CheckAttribute(chr, "PGG_Hunter")) Map_CreateWarrior("", chr.id, daysQty);
				else
				{
					// LEO: откуда выходим. Обычно - из города приписки, но участник, вернувшийся с турнира,
					//   стоит у Доминики: он обязан ОТПЛЫТЬ ОТТУДА, иначе корабль возникнет у старого порта,
					//   то есть телепортируется. Метку ставит PGG_ClearTournament, снимаем сразу здесь.
					string sFrom = chr.PGGAi.location.town;
					if (CheckAttribute(chr, "PGG_SailFrom"))
					{
						sFrom = chr.PGG_SailFrom;
						DeleteAttribute(chr, "PGG_SailFrom");
					}
					// на турнир маршрут строим до БЕРЕГА Доминики - имя острова точкой маршрута не является
					if (chr.PGGAi.Task == PGG_TASK_ARENA)
					{
						// ⚠️ ВРЕМЯ ЖИЗНИ энкаунтера - НЕ то же, что расчётный срок пути. Расчёт идёт по
						//   расстоянию до соседней колонии, а реальный ход зависит от ветра и обхода островов,
						//   поэтому кораблики таяли на полпути к острову (репорт юзера). Даём щедрый запас:
						//   рейс всё равно завершится по ФАКТУ прихода в точку, а не по этому таймауту.
						int iLive = daysQty * 2 + 10;
						Map_CreateTrader(sFrom, PGG_ARENA_LOCATOR, chr.id, iLive);
					}
					else
					{
						// LEO: возврат с турнира - запас времени жизни такой же щедрый, как по дороге туда:
						//   стартуем от берега Доминики, а не из порта, и путь до колонии считается по чужому
						//   расстоянию. Без запаса кораблик растворялся бы на полдороге.
						if (CheckAttribute(chr, "PGG_ArenaReturn"))
						{
							DeleteAttribute(chr, "PGG_ArenaReturn");
							Map_CreateTrader(sFrom, sColony, chr.id, daysQty * 2 + 10);
						}
						else Map_CreateTrader(sFrom, sColony, chr.id, daysQty);
					}
				}
			}
			Fantom_SetBalls(chr, "pirate");
			chr.PGGAi.Task.SetSail =  true;

			int PGGsAtSeaAdded = sti(pchar.PGGsAtSea) + 1;
			pchar.PGGsAtSea = PGGsAtSeaAdded;
			chr.PGGAi.ActiveQuest =  true;
			chr.PGGAi.location = "sea";
			chr.PGGAi.location.town = "";
			SaveCurrentNpcQuestDateParam(chr, "PGGAi.Task.days");

		}
	}
}

// LEO: снять ВСЕХ компаньонов ПГГ (единственный — торговец-эскорт, SetCompanionIndex ~273). Обнуляем СЫРО, а не через
//   SetCompanionIndex(-1): если слот торговца истёк и занят квест-NPC (Портман/цель ГВИК), протухший индекс тянул чужого
//   в бой ПГГ (краш/«засчёт квеста»); сырое обнуление не трогает персонажа в слоте — не корёжит чужой квест.
void PGG_ClearCompanions(ref chr)
{
	if (!CheckAttribute(chr, "Fellows.Companions")) return;
	for (int kc = 1; kc < COMPANION_MAX; kc++)
	{
		string cid = "id" + kc;
		if (CheckAttribute(chr, "Fellows.Companions." + cid)) chr.Fellows.Companions.(cid) = -1;
	}
}

//Blackthorn - роспуск эскадры и удаление энкаунтера ПГГ
void PGG_Disband_Fleet(ref chr)
{
	if (CheckAttribute(chr, "PGGAi.Task.SetSail"))
	{
		chr.PGGAi.RemoveEncounter = true;
		wdmEmptyAllDeadQuestEncounter();
		DeleteAttribute(chr, "PGGAi.RemoveEncounter");
		DeleteAttribute(chr, "Abordage.Enable");
		DeleteAttribute(chr, "AlwaysFriend");
		DeleteAttribute(chr, "PGGAi.Task.SetSail");
		int PGGsAtSearemoved = sti(pchar.PGGsAtSea) - 1;
		pchar.PGGsAtSea = PGGsAtSearemoved;
		if (sti(pchar.PGGsAtSea) < 0) pchar.PGGsAtSea = 0;
		chr.PGGAi.AfterSail.TaskCounter = 0;
		pchar.LastPgg = chr.id;
		PGG_EndHuntTraces(chr);
		DeleteAttribute(chr, "PGG_Hunter");
		// LEO: снимаем ВМЕСТЕ с охотой. Иначе побывавший охотником ПГГ навсегда выходит на глобалку
		//   военным-догоняющим (Map_CreateWarrior) и навсегда остаётся пиратом: этот же флаг гейтит
		//   возврат nation_backup.
		DeleteAttribute(chr, "PGG_warrior");
		DeleteAttribute(chr, "PGG_Convoy");
		DeleteAttribute(chr, "PGG_trader"); // LEO: чистим и флаг торговца (иначе после конвоя он завис навсегда -> таблица конкурентов врёт «сопровождает торговца» на всех последующих плаваниях)
		PGG_ClearCompanions(chr); // LEO: снять торговца-эскорт из компаньонов (SetCompanionIndex ~273). Безусловно и надёжно, даже если фантом торговца уже истёк
	}
}

//качалка для ПГГ
bool PGG_CompleteWorldMapSail(string sChar)
{
	int iChar = GetCharacterIndex(sChar);
	if (iChar == -1) return false;

	ref chr = &characters[iChar];
	if (!CheckAttribute(chr, "PGGAi")) return false;
	if (!CheckAttribute(chr, "PGGAi.Task.SetSail")) return false;

	DeleteAttribute(chr, "Abordage.Enable");
	DeleteAttribute(chr, "AlwaysFriend");
	DeleteAttribute(chr, "PGGAi.Task.SetSail");
	DeleteAttribute(chr, "PGGAi.ActiveQuest");
	PGG_EndHuntTraces(chr);
	DeleteAttribute(chr, "PGG_Hunter");
	DeleteAttribute(chr, "PGG_warrior"); // LEO: см. блок пересетапа - без этого охота кончается только на бумаге
	DeleteAttribute(chr, "PGG_Convoy");
	DeleteAttribute(chr, "PGG_trader"); // LEO: конвой доплыл -> снять флаг торговца (тот же пропуск, что в блоке пересетапа) -> таблица не врёт «сопровождает» после завершения рейса
	PGG_ClearCompanions(chr); // LEO: ГЛАВНАЯ дыра свиты — штатное «конвой доплыл» НЕ снимало компаньона-торговца. Истёкший слот переиспользовался квест-NPC -> протухший индекс тянул чужой корабль в эскадру ПГГ (краш/засчёт квеста)

	if (!CheckAttribute(pchar, "PGGsAtSea"))
	{
		pchar.PGGsAtSea = 0;
	}
	pchar.PGGsAtSea = sti(pchar.PGGsAtSea) - 1;
	if (sti(pchar.PGGsAtSea) < 0) pchar.PGGsAtSea = 0;

	chr.PGGAi.AfterSail.TaskCounter = 0;
	pchar.LastPgg = chr.id;

	// LEO: РЕЙС НА ТУРНИР финализируем иначе. Обычный рейс пишет город назначения в location.town и сваливает
	//   груз в тамошний магазин, но цель турнира - Доминика, а это НЕ колония: town стал бы "Dominica", и
	//   дальше всё, что ищет по городу, ломалось бы (репорт: «Не найдёт НПС с characterID Dominica_tavernkeeper»
	//   из PGG_progress.c, где по town достают трактирщика). Поэтому город НЕ трогаем - ПГГ остаётся приписан
	//   к прежнему, - только помечаем прибытие. Задачу тоже не стираем: она держит его на турнире.
	if (CheckAttribute(chr, "PGG_Tournament"))
	{
		chr.PGG_Tournament.Arrived = true;
		chr.PGGAi.location = "land";
		PGG_ParkAtTournament(chr);   // корабль остаётся у острова, на стояночном локаторе бухты
		return true;
	}

	if (CheckAttribute(chr, "PGGAi.Task.Target"))
	{
		chr.PGGAi.location = "land";
		chr.PGGAi.location.town = chr.PGGAi.Task.Target;

		if (CheckAttribute(chr, "PGGAi.Task.Target.Goods") && CheckAttribute(chr, "PGGAi.Task.Target.Goods.Qty"))
		{
			AddRemoveGoodsInStore(chr.PGGAi.location.town, sti(chr.PGGAi.Task.Target.Goods), sti(chr.PGGAi.Task.Target.Goods.Qty), true);
		}
	}

	DeleteAttribute(chr, "PGGAi.Task");
	return true;
}


//==================================================
// LEO: КОРАБЛЬ КОНКУРЕНТА-КОМПАНЬОНА
//==================================================
// Конкурент может наняться вместе СО СВОИМ судном. Метка PGGAi.OwnShip ставится там, где ПГГ покупает
// корабль на свои (PGG_progress.c и диалог Whisper_PGG.c), и снимается там, где корабль ему даёт игрок:
// расчёт на верфи, покупка нового судна, назначение капитаном на приз.
// Верфь выручку хозяину отдаёт (Variable_shipyard_44), а обходные пути метку игнорировали, и корабль
// уводили даром: ПУ (стоянка/торги), приказ «отведи корабль в город», обмен/снятие капитана/снятие пушек
// в форме обмена. Гейт общий, чтобы правило жило в одном месте.
// ⚠️ В кают-компании стоит КЛОН компаньона - настоящего ищем через realcompanionidx (тем же приёмом, что
//   гейт рейсов в Enc_Officer_dialog.c), иначе метки на клоне просто нет и проверка молча пропускает.
bool PGG_IsCompanionOwnShip(ref chr)
{
	int idx;
	if (CheckAttribute(chr, "PGGAi.OwnShip")) return true;
	if (!CheckAttribute(chr, "realcompanionidx")) return false;
	idx = sti(chr.realcompanionidx);
	if (idx < 0) return false;
	return CheckAttribute(&characters[idx], "PGGAi.OwnShip");
}

//==================================================
// LEO: ОХРАНА КАЮТЫ КОНКУРЕНТА
//==================================================
// Сколько телохранителей стоит в каюте ПГГ при абордаже. Считается ПО ДОСТАТКУ и ничего не стоит самому
// конкуренту: каждый полный PGG_CABIN_GUARD_PRICE в кошельке = ещё один боец, но деньги не списываются.
// Разбогател на турнирах - встречает игрока с гвардией; обеднел - охрана разошлась сама собой.
int PGG_GetCabinGuards(ref pgg)
{
	int iQty;
	if (!CheckAttribute(pgg, "Money")) return 0;
	iQty = sti(pgg.Money) / PGG_CABIN_GUARD_PRICE;
	if (iQty > PGG_CABIN_GUARDS_MAX) iQty = PGG_CABIN_GUARDS_MAX;
	if (iQty < 0) iQty = 0;
	return iQty;
}
