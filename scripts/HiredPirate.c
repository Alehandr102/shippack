// ===========================================================================================
// LEO: Квест "Наёмный пират" (hired privateer / caper).
// Поток:
//   1) В таверне по шансу спавнится пират-капер с ЖЁЛТЫМ "?" над головой (как барыга/делец GovSwap).
//   2) В диалоге он за сумму (50% сейчас) берётся захватить корабль. Предупреждает: за призом приплыть
//      с ЗАПАСНЫМ офицером - его посадим капитаном.
//   3) Через 10-20 дней ("захватывает") ЛЮБОЙ бармен по запросу даёт наводку на КОНКРЕТНУЮ бухту.
//   4) Плывёшь в бухту (корабль-приз стоит там), платишь вторые 50%, назначаешь офицера - корабль в эскадре.
//   5) Дедлайн: не приплыл за неделю после наводки бармена -> запись в СЖ закрывается, задаток сгорает.
// Пространство имён: pchar.GenQuest.HiredPirate.*  (генераторный квест, один за раз, чистится в конце).
// СЖ: AddQuestRecord("HiredPirate", "N")  (тексты - QuestBook1_Generic.txt).
// ===========================================================================================

// --- ТЮНИНГ ---
#define HIREDPIRATE_READY_MIN      10    // мин. дней "захвата" до наводки бармена
#define HIREDPIRATE_READY_SPAN     11    // + rand(0..10) -> 10..20 дней
#define HIREDPIRATE_DEADLINE_DAYS  7     // дней на прибытие в бухту после наводки бармена
#define HIREDPIRATE_COOLDOWN_DAYS  180   // КД повторного спавна в таверне после ПОЛУЧЕНИЯ приза (~полгода)
#define HIREDPIRATE_TEST_ALWAYS    0     // LEO: 1=тест 100% спавна (пропуск ролла шанса, только отладка), 0=штатная формула

// цена услуги = ПОЛНАЯ стоимость приза по ТТХ (как на верфи), 50% при найме + 50% в бухте.
// 🐛 НЕ звать GetShipPriceByTTH(baseType): она читает RealShips[baseType] (ДРУГОЙ массив - инстансы), по базовому
// индексу там чужой корабль -> Барк = 15 млн. Считаем по БАЗОВОМУ шаблону (GetShipByType) тем же набором под-функций.
int HiredPirate_FeeByShipType(int baseType)
{
	if (baseType < 0) return 15000; // тип не подобран
	ref rs = GetShipByType(baseType);
	int c = sti(rs.class);
	int fee = GetShipPriceByTTH_HP(c, rs);
	fee += GetShipPriceByTTH_Capacity(c, rs);
	fee += GetShipPriceByTTH_MaxCrew(c, rs);
	fee += GetShipPriceByTTH_MaxCaliber(c, rs);
	fee += GetShipPriceByTTH_CannonsQuantity(c, &NullCharacter, rs);
	fee += GetShipPriceByTTH_WindAgainstSpeed(c, rs);
	fee += GetShipPriceByTTH_MastMultiplier(c, rs);
	fee += GetShipPriceByTTH_SpeedRate(c, rs);
	fee += GetShipPriceByTTH_TurnRate(c, rs);
	if (fee < 5000) fee = 5000;
	return fee;
}

// класс приза по рангу ГГ (та же таблица, что SetShipToFantom в AIFantom.c) - выбираем ОДИН класс из диапазона.
int HiredPirate_PickShipClass()
{
	int Rank = sti(pchar.rank);
	int iClassMin = 6;
	int iClassMax = 6;
	if (Rank >= 1  && Rank <= 5)  { iClassMin = 6; iClassMax = 6; }
	if (Rank >= 5  && Rank <= 10) { iClassMin = 6; iClassMax = 5; }
	if (Rank >= 10 && Rank <= 15) { iClassMin = 5; iClassMax = 4; }
	if (Rank >= 15 && Rank <= 20) { iClassMin = 4; iClassMax = 3; }
	if (Rank >= 20 && Rank <= 30) { iClassMin = 3; iClassMax = 2; }
	if (Rank > 30)                { iClassMin = 2; iClassMax = 2; } // LEO: класс 1 исключён (был iClassMax=1)
	// iClassMin >= iClassMax по номеру (6 хуже 1); выбираем случайный класс между ними
	int lo = iClassMax; // меньший НОМЕР
	int hi = iClassMin; // больший НОМЕР
	return lo + rand(hi - lo);
}

bool HiredPirate_IsActive()
{
	return CheckAttribute(pchar, "GenQuest.HiredPirate");
}

// прошло ли достаточно дней "захвата" -> бармен готов дать наводку
bool HiredPirate_IsReadyForLead()
{
	if (!HiredPirate_IsActive()) return false;
	if (pchar.GenQuest.HiredPirate.Stage != "hired") return false;
	int need = sti(pchar.GenQuest.HiredPirate.ReadyDays);
	if (GetQuestPastDayParam("HiredPirate_HireDate") >= need) return true;
	return false;
}

// ---------------------------------------------------------------------------------------------
// СПАВН В ТАВЕРНЕ (хук на вход в локацию, зовётся из LAi_login.c). Образец - GovSwap/барыга.
// ---------------------------------------------------------------------------------------------
// уже есть живой капер в мире (не наняли, не истёк) - чтобы не плодить дубль при повторном входе в тот же день
bool HiredPirate_CaperExists()
{
	if (!CheckAttribute(pchar, "GenQuest.HiredPirate_TavernNpc")) return false;
	int idx = GetCharacterIndex(pchar.GenQuest.HiredPirate_TavernNpc);
	if (idx < 0) return false;
	if (!CheckAttribute(&characters[idx], "isHiredCaper")) return false; // сняли флаг = уже не капер
	return true;
}

void HiredPirate_TrySpawn(aref loc)
{
	// БУХТА: капер ждёт с призом (stage revealed). Спавним при заходе в бухту - основной, надёжный путь
	// (не зависит от win_condition и его регистра). Сравнение id локации регистронезависимое (FindLocation).
	if (HiredPirate_IsActive())
	{
		if (CheckAttribute(pchar, "GenQuest.HiredPirate.Stage") && pchar.GenQuest.HiredPirate.Stage == "revealed")
		{
			if (CheckAttribute(loc, "id") && CheckAttribute(pchar, "GenQuest.HiredPirate.BayLocation"))
			{
				if (FindLocation(loc.id) == FindLocation(pchar.GenQuest.HiredPirate.BayLocation)) HiredPirate_SpawnBayCaper();
			}
		}
		return;                                         // квест активен - тавернского капера не спавним
	}
	if (!CheckAttribute(loc, "habitues")) return;       // только ТАВЕРНА (атрибут habitues)
	if (HiredPirate_CaperExists()) return;              // уже есть живой капер - не плодим
	if (LAi_IsCapturedLocation) return;
	if (!CheckAttribute(loc, "fastreload")) return;
	int iColony = FindColony(loc.fastreload);
	if (iColony < 0) return;
	int iNation = sti(Colonies[iColony].nation);
	if (iNation == PIRATE) return;                      // не пиратские поселения

	// LEO: КД после ПОЛУЧЕНИЯ приза - полгода не появляется в тавернах. Штамп ставится в HiredPirate_GetShip.
	// Правила шанса ниже НЕ трогаем - просто не доходим до них, пока КД не истёк.
	if (CheckAttribute(pchar, "HiredPirate_CooldownDate") && GetQuestPastDayParam("HiredPirate_CooldownDate") < HIREDPIRATE_COOLDOWN_DAYS) return;

	// шанс 1-в-1 как у картоторговца (LAi_utilites.c): чем выше Удача - тем чаще, у Авантюриста заметно выше.
	// НО на dRand (день + колония фикс) вместо rand - вход в таверну нельзя переспамить, не сейв-скамится.
	int iLuck = makeint(GetCharacterSPECIALSimple(pchar, SPECIAL_L) / 3);
	int iRoll = dRand(9999) + iColony * 37;
	// LEO: ролл шанса пропускается в тест-режиме (HIREDPIRATE_TEST_ALWAYS 1) — капер спавнится ВСЕГДА (для отладки квеста).
	if (HIREDPIRATE_TEST_ALWAYS == 0)
	{
		if (IsCharacterPerkOn(pchar, "Adventurer"))
		{
			if (iRoll % (10 - iLuck) != 0) return;   // авантюрист: ~1/(10 - Удача/3)
		}
		else
		{
			if (iRoll % (16 - iLuck) != 3) return;   // обычный: ~1/(16 - Удача/3)
		}
	}

	// капер СИДИТ (как контрабандист/завсегдатаи) и имеет ПИРАТСКУЮ модель
	string sSit = LAi_FindFreeRandomLocator("sit");
	if (sSit == "") return;                             // нет свободных мест в таверне - не спавним

	int iChar = NPC_GeneratePhantomCharacter("pirate", iNation, MAN, 1); // LifeDay=1 - исчезнет назавтра
	ref chr = &characters[iChar];
	SetNPCModelUniq(chr, "pirate", MAN);
	chr.City     = Colonies[iColony].id;
	chr.CityType = "citizen";                           // фантом-многодневка: этим его чистит система горожан (LifeDay=1)
	chr.isHiredCaper = true;                            // флаг жёлтого "?" (LAi_sit.c)
	LAi_SetLoginTime(chr, 0.0, 23.99);
	// LEO: НЕ звать тут FreeSitLocator! Локатор уже свободен (его вернул LAi_FindFreeRandomLocator),
	// а функция проходит по ВСЕМУ массиву персонажей и в ветке else разлогинивает каждого, у кого
	// location == "none" - это состояние динамически подсаженных посетителей таверны. Итог: спавн капера
	// вычищал таверну целиком, оставляя лишь статиков (бармен, служанка) и контрабандистов.
	ChangeCharacterAddressGroup(chr, loc.id, "sit", sSit);
	LAi_group_MoveCharacter(chr, GetNationNameByType(iNation) + "_citizens");
	LAi_SetSitType(chr);
	chr.dialog.filename    = "HiredPirate_Tavern.c";
	chr.dialog.currentnode = "First time";
	chr.greeting           = "pirat_common";
	chr.talker             = 1;
	// Фиксируем класс+БАЗОВЫЙ тип (GetShipTypeExt отсекает QuestShip, класс 1 исключён в PickShipClass). Приз тут НЕ
	// генерим - GenerateShipTop выделял бы RealShips-слот, а он утёк бы у ненанятых каперов-однодневок. Цена по базовому
	// шаблону = цене выданного корабля (GenerateShipTop ставит QuestShip=true -> макс-статы пропускаются -> базовые ТТХ).
	int cls = HiredPirate_PickShipClass();
	int baseType = GetShipTypeExt(cls, cls, "", -1);
	chr.hpClass    = cls;
	chr.hpShipType = baseType;                          // базовый тип (ShipsTypes) - по нему генерим приз при найме
	chr.hpFee      = HiredPirate_FeeByShipType(baseType);
	pchar.GenQuest.HiredPirate_TavernNpc = chr.id;      // временно, до найма/ухода из таверны
}

// ---------------------------------------------------------------------------------------------
// НАЁМ (зовётся из диалога тавернского пирата после согласия). Списываем 50%, заводим квест + СЖ.
// ---------------------------------------------------------------------------------------------
void HiredPirate_Hire(ref npc)
{
	// класс/цену/базовый тип берём зафиксированными при спавне
	int cls = HiredPirate_PickShipClass();
	int fee = 15000;
	int baseType = -1;
	if (CheckAttribute(npc, "hpClass"))    cls = sti(npc.hpClass);
	if (CheckAttribute(npc, "hpFee"))      fee = sti(npc.hpFee);
	if (CheckAttribute(npc, "hpShipType")) baseType = sti(npc.hpShipType);
	if (baseType < 0) baseType = GetShipTypeExt(cls, cls, "", -1); // фолбэк (старый капер без hpShipType)
	int deposit = fee / 2;

	AddMoneyToCharacter(pchar, -deposit);              // 50% сейчас

	pchar.GenQuest.HiredPirate.Stage    = "hired";
	pchar.GenQuest.HiredPirate.ShipClass = cls;
	pchar.GenQuest.HiredPirate.Rest     = fee - deposit; // остаток к оплате в бухте
	pchar.GenQuest.HiredPirate.ReadyDays = HIREDPIRATE_READY_MIN + rand(HIREDPIRATE_READY_SPAN - 1);
	pchar.GenQuest.HiredPirate.FromIsland = Islands[GetCharacterCurrentIsland(pchar)].id;
	// LEO: ПЕРСИСТЕНТНЫЙ капер для бухты (механизм ПГГ/PsHero): NPC_GenerateCharacter = именованный по фикс-id + конкретной
	//   моделью, LifeDay=-1 => не рециклится как фантом. Модель ставится при входе в локацию -> в бухте просто ПЕРЕНОСИМ его
	//   (ChangeCharacterAddressGroup), та же модель/лицо, без заморозок/рефрешей. Тавернский фантом умрёт как habitue.
	int iBayCap = NPC_GenerateCharacter("HiredPirateCaper", npc.model, "man", "man", 5, PIRATE, -1, true);
	if (iBayCap >= 0)
	{
		ref bayCap = &Characters[iBayCap];
		if (CheckAttribute(npc, "model.animation")) bayCap.model.animation = npc.model.animation;
		if (CheckAttribute(npc, "FaceId"))   bayCap.FaceId  = npc.FaceId;   // ТА ЖЕ внешность, что в таверне
		if (CheckAttribute(npc, "name"))     bayCap.name     = npc.name;
		if (CheckAttribute(npc, "lastname")) bayCap.lastname = npc.lastname;
	}
	// фолбэк-внешность (на случай потери именованного персонажа): в бухте пересоздаём по этим атрибутам.
	pchar.GenQuest.HiredPirate.CaperModel = npc.model;
	if (CheckAttribute(npc, "model.animation")) pchar.GenQuest.HiredPirate.CaperModelAni = npc.model.animation;
	if (CheckAttribute(npc, "FaceId"))   pchar.GenQuest.HiredPirate.CaperFace = npc.FaceId;
	if (CheckAttribute(npc, "name"))     pchar.GenQuest.HiredPirate.CaperName = npc.name;
	if (CheckAttribute(npc, "lastname")) pchar.GenQuest.HiredPirate.CaperLastName = npc.lastname;

	// ГЕНЕРИМ+МОРОЗИМ приз тут (на найме, не на спавне - иначе RealShips-слот утекал бы у ненанятых каперов). Тип
	// (hpShipType) зафиксирован на спавне -> reload до найма даёт тот же тип; всё замораживаем в PrizeShip (не перекатать).
	// Чертёжный: GenerateShipTop ставит QuestShip=true + принудительно Blueprint=true.
	npc.Ship.Type = GenerateShipTop(baseType, false, npc);         // false = НЕ ворованый (Stolen=0)
	RealShips[sti(npc.Ship.Type)].Blueprint = true;
	// LEO: ЗАЩИТА СЛОТА приза от сборщика мусора. Капер = однодневка, за 10-20 дней ожидания умирает, его слот RealShips
	//   никто не держит -> EmptyAllFantomShips/ClearAllFantomShips стирали слот целиком (вкл. имя модели) -> при передаче
	//   Ship_Add2Sea падал ("SHIP N have no name") и приз пропадал. Флаг щадит слот (см. GC в ShipsUtilites/OnLoadReinit);
	//   снимается при передаче/провале (Cleanup). Регенерацию слота на передаче НЕ делаем (рероллит апгрейд корпуса = слив).
	RealShips[sti(npc.Ship.Type)].HiredPrize = true;
	SetRandomNameToShip(npc);
	SetBaseShipData(npc);
	ResetShipCannonsDamages(npc);
	int iMax = GetMaxCrewQuantity(npc);
	SetCrewQuantity(npc, iMax / 3 + rand(iMax / 3));               // случайная команда
	npc.ship.HP = makeint(stf(npc.ship.HP) * (0.4 + frnd() * 0.6)); // 40..100% корпуса
	npc.ship.SP = makeint(stf(npc.ship.SP) * (0.4 + frnd() * 0.6)); // 40..100% парусов (отдельный ролл - независимо от корпуса)
	// ТРЮМ: рандомный ТОРГОВЫЙ груз тем же методом, что энкаунтеры (трофей = купец). Клампится по вместимости, не locked
	// (у капера нет situation). Пишется в npc.Ship.Cargo.Goods -> в заморозку. ДО freeze и ПОСЛЕ SetBaseShipData.
	Fantom_SetGoods(npc, "trade");
	aref arTo, arFrom;
	makearef(arTo, pchar.GenQuest.HiredPirate.PrizeShip);
	makearef(arFrom, npc.ship);
	CopyAttributes(arTo, arFrom);                                  // ЗАМОРОЗКА приза в квест (в бухте офицеру дают ЭТОТ корабль)

	SaveCurrentQuestDateParam("HiredPirate_HireDate");  // штамп даты найма (для отсчёта дней "захвата")

	SetQuestHeader("HiredPirate");
	AddQuestRecord("HiredPirate", "1");                 // "нанял капера, ждать наводку бармена"
	AddQuestUserData("HiredPirate", "sSex", GetSexPhrase("","а")); // LEO: род ГГ - "нанял"/"наняла"
	AddQuestUserData("HiredPirate", "sMoney", FindRussianMoneyString(deposit));
	AddQuestUserData("HiredPirate", "sRest", FindRussianMoneyString(fee - deposit));

	// Снять метку у собеседника + сделать инертным. Внешность/имя уже сохранены выше - в бухте будет ТОТ ЖЕ пират
	// (переиспользуем по CaperChrId, пока жив, иначе фолбэк-пересоздание по CaperModel/Face/Name).
	DeleteAttribute(npc, "isHiredCaper");
	if (CheckAttribute(npc, "quest.questflag")) DeleteAttribute(npc, "quest.questflag");
	DeleteAttribute(npc, "talker");                     // инертен: без дефолтной "!" и не переспросить
	// Тавернский фантом (npc) НЕ трогаем сверх инертности - умрёт как обычный завсегдатай (LifeDay=1). Капер для бухты -
	// отдельный ПЕРСИСТЕНТНЫЙ "HiredPirateCaper" (создан выше), его и переносим в бухту.
	pchar.GenQuest.HiredPirate.CaperChrId = "HiredPirateCaper"; // фикс-id персистентного капера (в бухту переносим ЕГО)
	DeleteAttribute(pchar, "GenQuest.HiredPirate_TavernNpc");
}

// ---------------------------------------------------------------------------------------------
// ПОЛНАЯ ОЧИСТКА квеста (успех/провал/отказ). Снимает флаги, дедлайн, распускает группу приза.
// ---------------------------------------------------------------------------------------------
void HiredPirate_Cleanup()
{
	pchar.quest.HiredPirate_Deadline.over = "yes";     // снять таймер-дедлайн, если стоит
	// убрать капера (тот же персонаж в таверне/бухте = CaperChrId). Инертен: снять флаг+метку, talker (иначе
	// citizen-тик повесит дефолтный "!"), dialog (иначе кликнешь снова). lifeDay=0 - тело уйдёт при перезагрузке.
	int bidx = -1;
	if (CheckAttribute(pchar, "GenQuest.HiredPirate.CaperChrId")) bidx = GetCharacterIndex(pchar.GenQuest.HiredPirate.CaperChrId);
	if (bidx < 0 && CheckAttribute(pchar, "GenQuest.HiredPirate.BayCaperId")) bidx = GetCharacterIndex(pchar.GenQuest.HiredPirate.BayCaperId);
	if (bidx >= 0)
	{
		ref bm = &characters[bidx];
		DeleteAttribute(bm, "isHiredCaper");
		if (CheckAttribute(bm, "quest.questflag")) DeleteAttribute(bm, "quest.questflag");
		DeleteAttribute(bm, "talker");
		DeleteAttribute(bm, "dialog");
		bm.lifeDay = 0;
	}
	// LEO: снять спец-защиту слота приза от GC. После передачи слот держит офицер-компаньон (штатный lock);
	//   при провале дедлайна слот больше не нужен -> пусть GC его штатно стирает. Без снятия = вечный флаг = утечка слота.
	if (CheckAttribute(pchar, "GenQuest.HiredPirate.PrizeShip.Type"))
	{
		DeleteAttribute(&RealShips[sti(pchar.GenQuest.HiredPirate.PrizeShip.Type)], "HiredPrize");
	}
	DeleteAttribute(pchar, "GenQuest.HiredPirate");
	DeleteAttribute(pchar, "GenQuest.HiredPirate_TavernNpc");
	DeleteAttribute(pchar, "HiredPirate_HireDate"); // штамп даты найма (SaveCurrentQuestDateParam)
}

// материковые "острова" (Мейн): в Islands_init.c помечены комментом (Mein). Юзер хочет только НАСТОЯЩИЕ острова -
// приз в бухте Мейна не отгоняем (у их берегов есть имя в LocLables, потому фильтр названного берега их не ловит).
bool HiredPirate_IsMainland(string sIsl)
{
	if (sIsl == "PortoBello")    return true;
	if (sIsl == "Cartahena")     return true;
	if (sIsl == "Maracaibo")     return true;
	if (sIsl == "Caracas")       return true;
	if (sIsl == "Cumana")        return true;
	if (sIsl == "SantaCatalina") return true;
	if (sIsl == "Beliz")         return true;
	if (sIsl == "Panama")        return true;
	return false;
}

// ---------------------------------------------------------------------------------------------
// НАВОДКА БАРМЕНА: зовётся из бармен-диалога, когда квест активен, срок "захвата" вышел и бухта ещё не открыта.
// Выбирает ближний остров + бухту (берег), ставит дедлайн неделю, взводит win_condition на вход в бухту (там
// заспавнится капер с призом), пишет запись в СЖ.
// ---------------------------------------------------------------------------------------------
void HiredPirate_BarmanLead()
{
	// Выбор острова+бухты. dRand (не rand!) - бухту нельзя перекатать перезагрузкой (день-фикс). dRand в ЦИКЛЕ
	// вернул бы одно и то же - берём dRand-СТАРТ индекса и детерминированно обходим по кругу.
	// Требуем НАЗВАННЫЙ реальный НЕ-маяк берег: это исключает МЕЙН/Панаму (у их берегов нет имени в LocLables)
	// и недостижимые точки. Остров без такого берега пропускаем целиком.
	// все объявления - в начале функции (Storm: область видимости = вся функция; в цикле объявлять рискованно)
	string sFrom = "";
	aref arIsl, arShore;
	int nIsl, nShore, startI, startS, ii, jj, li, lf;
	string sIsland, sBay, sBayName, sCandI, sAttr, sCandS, sReal, nm;

	if (CheckAttribute(pchar, "GenQuest.HiredPirate.FromIsland")) sFrom = pchar.GenQuest.HiredPirate.FromIsland;
	makearef(arIsl, NullCharacter.TravelMap.Islands);
	nIsl = GetAttributesNum(arIsl);
	startI = 0;
	if (nIsl > 0) startI = (dRand1(9999) + 37) % nIsl;

	sIsland = "";
	sBay = "";
	sBayName = "";
	for (ii = 0; ii < nIsl; ii++)
	{
		sCandI = GetAttributeName(GetAttributeN(arIsl, (startI + ii) % nIsl));
		if (sCandI == sFrom) continue;
		if (HiredPirate_IsMainland(sCandI)) continue;          // весь Мейн (Порто Белло, Панама и пр.) - не остров
		if (GetCityNameByIsland(sCandI) == "None") continue;   // без колонии
		makearef(arShore, NullCharacter.TravelMap.Islands.(sCandI).Shore);
		nShore = GetAttributesNum(arShore);
		if (nShore < 1) continue;
		startS = (dRand3(9999) + 53) % nShore;                 // другой слот = декорреляция от острова
		for (jj = 0; jj < nShore; jj++)
		{
			sAttr = "t" + ((startS + jj) % nShore);
			sCandS = arShore.(sAttr);
			if (findsubstr(sCandS, "Mayak", 0) != -1) continue; // маяк - не берег
			li = FindLocation(sCandS);
			if (li < 0) continue;                               // локации нет - недостижимо
			sReal = locations[li].id;                           // нормализованный регистр (shore21 -> Shore21)
			nm = GetConvertStr(sReal, "LocLables.txt");
			if (nm == "" || nm == sReal || nm == sCandS) continue; // нет читаемого имени = мейн/безымянный
			sIsland = sCandI; sBay = sReal; sBayName = nm;
			break;
		}
		if (sBay != "") break;
	}
	if (sBay == "") // фолбэк (крайне маловероятно): любой НЕ-материковый остров с колонией и берегом
	{
		for (ii = 0; ii < nIsl; ii++)
		{
			sCandI = GetAttributeName(GetAttributeN(arIsl, (startI + ii) % nIsl));
			if (HiredPirate_IsMainland(sCandI)) continue;
			if (GetCityNameByIsland(sCandI) == "None") continue;
			sBay = GetIslandRandomShoreId(sCandI);
			if (sBay == "") continue;
			if (findsubstr(sBay, "Mayak", 0) != -1) continue;
			lf = FindLocation(sBay);
			if (lf < 0) continue;
			sIsland = sCandI;
			sBay = locations[lf].id;
			sBayName = GetConvertStr(sBay, "LocLables.txt");
			break;
		}
	}

	pchar.GenQuest.HiredPirate.Stage       = "revealed";
	pchar.GenQuest.HiredPirate.BayIsland    = sIsland;
	pchar.GenQuest.HiredPirate.BayLocation  = sBay;
	pchar.GenQuest.HiredPirate.BayName      = sBayName;

	// капер заспавнится при заходе игрока в бухту - основной путь через хук входа HiredPirate_TrySpawn,
	// win_condition оставляем дублирующим (на случай, если хук не отработает)
	pchar.quest.HiredPirate_ArriveBay.win_condition.l1 = "Location";
	pchar.quest.HiredPirate_ArriveBay.win_condition.l1.location = sBay;
	pchar.quest.HiredPirate_ArriveBay.function = "HiredPirate_ArriveBay";

	// дедлайн неделя (до конца текущих суток + N)
	SetFunctionTimerCondition("HiredPirate_Deadline", 0, 0, HIREDPIRATE_DEADLINE_DAYS, false);

	AddQuestRecord("HiredPirate", "2");
	AddQuestUserData("HiredPirate", "sBay", pchar.GenQuest.HiredPirate.BayName);       // ТОЧНАЯ бухта по имени
	AddQuestUserData("HiredPirate", "sIsland", XI_ConvertString(sIsland + "Gen"));     // остров
	AddQuestUserData("HiredPirate", "sRest", FindRussianMoneyString(sti(pchar.GenQuest.HiredPirate.Rest)));
}

// ---------------------------------------------------------------------------------------------
// СПАВН капера с "?" в бухте (текущая локация). Диалог = забор приза. Гард от дублей по BayCaperId.
// ---------------------------------------------------------------------------------------------
void HiredPirate_SpawnBayCaper()
{
	if (!HiredPirate_IsActive()) return;
	// Берём ПЕРСИСТЕНТНОГО именованного капера (создан на найме через NPC_GenerateCharacter). Модель/лицо у него уже
	// заданы и применятся при постановке в локацию - НИКАКИХ заморозок/рефрешей.
	int idx = -1;
	if (CheckAttribute(pchar, "GenQuest.HiredPirate.CaperChrId")) idx = GetCharacterIndex(pchar.GenQuest.HiredPirate.CaperChrId);
	// Фолбэк (не должно случаться - персонаж именованный/персистентный): пересоздаём тем же id по фолбэк-внешности.
	if (idx < 0)
	{
		string sModel = "pirate_1";
		if (CheckAttribute(pchar, "GenQuest.HiredPirate.CaperModel")) sModel = pchar.GenQuest.HiredPirate.CaperModel;
		idx = NPC_GenerateCharacter("HiredPirateCaper", sModel, "man", "man", 5, PIRATE, -1, true);
		if (idx >= 0)
		{
			ref cr = &Characters[idx];
			if (CheckAttribute(pchar, "GenQuest.HiredPirate.CaperModelAni")) cr.model.animation = pchar.GenQuest.HiredPirate.CaperModelAni;
			if (CheckAttribute(pchar, "GenQuest.HiredPirate.CaperFace"))     cr.FaceId  = pchar.GenQuest.HiredPirate.CaperFace;
			if (CheckAttribute(pchar, "GenQuest.HiredPirate.CaperName"))     cr.name     = pchar.GenQuest.HiredPirate.CaperName;
			if (CheckAttribute(pchar, "GenQuest.HiredPirate.CaperLastName")) cr.lastname = pchar.GenQuest.HiredPirate.CaperLastName;
		}
		pchar.GenQuest.HiredPirate.CaperChrId = "HiredPirateCaper";
	}
	if (idx < 0) return;
	ref chr = &characters[idx];
	if (CheckAttribute(chr, "isHiredCaper")) return; // уже поставлен в бухте
	// ПЕРЕНОСИМ капера в текущую локацию (бухту) на goto-локатор рядом с ГГ + диалог забора. talker=1 + citizen: стоит с "?".
	chr.isHiredCaper = true;
	LAi_SetLoginTime(chr, 0.0, 24.0);
	string sLoc = LAi_FindNearestFreeLocator2Pchar("goto");
	if (sLoc != "") { ChangeCharacterAddressGroup(chr, pchar.location, "goto", sLoc); }
	else { PlaceCharacter(chr, "goto", "random_free"); }
	LAi_SetCitizenType(chr);
	chr.dialog.filename    = "HiredPirate_Bay.c";
	chr.dialog.currentnode = "First time";
	chr.greeting           = "pirat_common";
	chr.talker             = 1;
	pchar.GenQuest.HiredPirate.BayCaperId = chr.id;
}

// win_condition Location (дублирующий путь к спавну бухтового капера)
void HiredPirate_ArriveBay(string qName)
{
	HiredPirate_SpawnBayCaper();
}

// ---------------------------------------------------------------------------------------------
// ЗАБОР ПРИЗА: выбранному офицеру (iOfficerIdx) генерим корабль по классу, сажаем капитаном в эскадру, платим остаток.
// ---------------------------------------------------------------------------------------------
void HiredPirate_GiveShip(int iOfficerIdx)
{
	ref off = GetCharacter(iOfficerIdx);
	int frozenHP = 0; // заморожённый (потрёпанный) корпус - объявляем тут (Storm: не в nested-блоке)
	int frozenSP = 0; // заморожённые (порванные) паруса - там же
	// передаём ЗАМОРОЖЕННЫЙ при найме корабль (не генерим заново - иначе награду сливали бы перезагрузкой)
	if (CheckAttribute(pchar, "GenQuest.HiredPirate.PrizeShip"))
	{
		DeleteAttribute(off, "ship");
		off.ship = "";
		aref arTo, arFrom;
		makearef(arTo, off.ship);
		makearef(arFrom, pchar.GenQuest.HiredPirate.PrizeShip);
		CopyAttributes(arTo, arFrom);
		// DontRefresh: SetBaseShipData без него занулил бы заморож. груз (SetGoodsInitNull) и сбросил бы HP/SP на макс
		// (корабль «новый»). Флаг гасит рефилл груза/команды/морали (сам снимается в конце). Сброс HP/SP вне гварда -
		// запоминаем потрёпанные корпус/паруса и возвращаем после.
		frozenHP = sti(off.ship.HP);
		frozenSP = sti(off.ship.SP);
		off.DontRefresh = true;
		SetBaseShipData(off);
		off.ship.HP = frozenHP;
		off.ship.SP = frozenSP;
		// LEO: флаг Blueprint/QuestShip живёт в RealShips[тип] (не в атрибутах корабля, что копируются) -
		// переставляем на выданный тип, чтобы приз ГАРАНТИРОВАННО был чертёжным (6 слотов апгрейда и т.д.).
		RealShips[sti(off.Ship.Type)].Blueprint = true;
	}
	else // фолбэк (старый сейв без заморозки): сгенерить по классу
	{
		int cls = sti(pchar.GenQuest.HiredPirate.ShipClass);
		off.Ship.Type = GenerateShipTop(GetShipTypeExt(cls, cls, "", -1), true, off); // чертёжный корабль (как основной путь)
		RealShips[sti(off.Ship.Type)].Blueprint = true;
		SetRandomNameToShip(off);
		SetBaseShipData(off);
		ResetShipCannonsDamages(off);
	}

	// оплата остатка (до пересадки, пока GenQuest ещё жив)
	AddMoneyToCharacter(pchar, -sti(pchar.GenQuest.HiredPirate.Rest));

	// офицер из свиты -> капитан корабля эскадры (ТОЧНЫЙ образец transfer_main.c AcceptAddOfficer:1644-1647).
	// CheckForReleaseOfficer ОБЯЗАТЕЛЕН - без него офицер не снимается с роли и корабль не встаёт в эскадру.
	CheckForReleaseOfficer(iOfficerIdx);
	RemovePassenger(pchar, off);
	SetCompanionIndex(pchar, -1, iOfficerIdx);
	off.loyality = MAX_LOYALITY;

	AddQuestRecord("HiredPirate", "3");
	AddQuestUserData("HiredPirate", "sShip", XI_ConvertString(RealShips[sti(off.Ship.Type)].BaseName) + " '" + off.Ship.name + "'");
	CloseQuestHeader("HiredPirate");
	// LEO: КД повторного спавна - штампуем ТОЛЬКО при успешном получении приза (не при провале дедлайна).
	// Cleanup его не трогает (чистит HireDate, не CooldownDate), так что переживёт очистку квеста.
	SaveCurrentQuestDateParam("HiredPirate_CooldownDate");
	HiredPirate_Cleanup(); // снять дедлайн, убрать бухтового капера, стереть флаги
}

// ---------------------------------------------------------------------------------------------
// ДЕДЛАЙН (win_condition Timer): не успел за неделю - капер сбыл приз, задаток сгорел.
// ---------------------------------------------------------------------------------------------
void HiredPirate_Deadline(string qName)
{
	if (!HiredPirate_IsActive()) return;
	AddQuestRecord("HiredPirate", "4");
	AddQuestUserData("HiredPirate", "sSex", GetSexPhrase("","а")); // LEO: род ГГ - "успел"/"успела"
	CloseQuestHeader("HiredPirate");
	HiredPirate_Cleanup();
}
