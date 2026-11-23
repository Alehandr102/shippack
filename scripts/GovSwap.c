// =============================================================================
//  ПОДМЕНА ГРУЗА - сюжетная развилка ГУБЕРНАТОРСКОГО фрахта (тип 6).
//  В порту-отправителе (25%) к ГГ подбегает делец и предлагает подменить казённый
//  груз дешёвкой: настоящий товар - ему, в резиденцию сдать филькину грамоту под той
//  же накладной. При сдаче губернатору, если подмена была - мэр зовёт ТАМОЖЕННИКА
//  (5% запечённый шанс заметить). Инспектор подходит к ГГ, сверяет груз, уходит;
//  итог объявляет МЭР при повторном разговоре.
//  Роллы (GovSwapOffer 25%, GovSwapCaught 5%) запекаются ПРИ ВЗЯТИИ фрахта
//  (quests_reaction.c trade_quest_open) на дневном dRand - не сейв-скамятся.
//  Связано с [[cargo-delivery-quests]].
// =============================================================================

// доля дельца за подмену: 60% базовой стоимости груза (платит сразу, наличными)
int GovSwap_Bonus()
{
	int g   = sti(pchar.CargoQuest.iTradeGoods);
	int qty = sti(pchar.CargoQuest.iQuantityGoods);
	int pct = 50;
	if (CheckAttribute(pchar, "CargoQuest.GovSwapBonusPct")) pct = sti(pchar.CargoQuest.GovSwapBonusPct); // 40-60%, запечён при взятии
	return makeint(stf(Goods[g].Cost) * qty * pct / 100);
}

// самый дешёвый товар грузового диапазона (какао..кирпич), кроме исходного - на него меняем при подмене
int GovSwap_CheapestGood(int exceptGood)
{
	int best = -1;
	int bestCost = 0;
	int g, c;
	for (g = GOOD_CHOCOLATE; g <= GOOD_BRICK; g++)
	{
		if (g == exceptGood) continue;
		c = sti(Goods[g].Cost);
		if (best < 0) { best = g; bestCost = c; }
		else { if (c < bestCost) { best = g; bestCost = c; } }
	}
	if (best < 0) best = GOOD_CHOCOLATE; // фоллбек
	return best;
}

// какой товар реально в трюме по этому фрахту: подменённая дешёвка (если была подмена) либо исходный груз
int GovSwap_HoldGood()
{
	if (CheckAttribute(pchar, "CargoQuest.GovSwapFakeGood")) return sti(pchar.CargoQuest.GovSwapFakeGood);
	return sti(pchar.CargoQuest.iTradeGoods);
}

// категория товара - чтобы подменять на правдоподобно-похожее (дерево на дерево, ткань на ткань и т.д.)
int GovSwap_GoodCategory(int g)
{
	if (g == GOOD_EBONY) return 1;     // дерево
	if (g == GOOD_MAHOGANY) return 1;
	if (g == GOOD_SANDAL) return 1;
	if (g == GOOD_PLANKS) return 1;
	if (g == GOOD_COTTON) return 2;    // ткань/мануфактура
	if (g == GOOD_LINEN) return 2;
	if (g == GOOD_LEATHER) return 2;
	if (g == GOOD_SILK) return 2;
	if (g == GOOD_CLOTHES) return 2;
	if (g == GOOD_WINE) return 3;      // напитки
	if (g == GOOD_ALE) return 3;
	if (g == GOOD_RUM) return 3;
	if (g == GOOD_CHOCOLATE) return 4; // съестное/колониальные
	if (g == GOOD_COFFEE) return 4;
	if (g == GOOD_SUGAR) return 4;
	if (g == GOOD_WHEAT) return 4;
	if (g == GOOD_CINNAMON) return 4;
	if (g == GOOD_COPRA) return 4;
	if (g == GOOD_PAPRIKA) return 4;
	if (g == GOOD_FRUITS) return 4;
	if (g == GOOD_OIL) return 4;
	if (g == GOOD_TOBACCO) return 4;
	return 0; // прочее (кирпич и т.п.) - категории нет
}

// заменитель для подмены: более дешёвый товар ТОЙ ЖЕ категории, вариативно по dRand.
// Нет похожего дешевле - фоллбек на самый дешёвый. slot - поток дневного рандома.
int GovSwap_SubstituteGood(int realGood, int slot)
{
	int cat = GovSwap_GoodCategory(realGood);
	if (cat == 0) return GovSwap_CheapestGood(realGood); // у груза нет категории - просто самый дешёвый
	int aCand[24];
	int n = 0;
	int realCost = sti(Goods[realGood].Cost);
	int g;
	for (g = 0; g < GOODS_QUANTITY; g++)
	{
		if (g == realGood) continue;
		if (GovSwap_GoodCategory(g) != cat) continue;     // только та же категория
		if (sti(Goods[g].Cost) >= realCost) continue;     // только дешевле исходного
		if (n < 24) { aCand[n] = g; n = n + 1; }
	}
	if (n == 0) return GovSwap_CheapestGood(realGood);    // в категории нет дешевле - фоллбек
	int j = dRandSlotLow(slot, n - 1);
	return aCand[j];
}

// делец подбегает к ГГ в порту/городе-отправителе (спавн у портового локатора reload1_back). Зовётся на входе
// в локацию (GovSwap_TryDealerOnEnter) - надёжный хук, не гейтится горожанами.
void TrySpawnGovSwapDealer(aref loc, int iColony, int iNation)
{
	if (!CheckAttribute(pchar, "CargoQuest.iOrderType")) return;
	if (sti(pchar.CargoQuest.iOrderType) != 6) return;                // только губернаторский фрахт
	if (loc.type != "town" && loc.type != "port") return;             // город/порт
	if (iNation == PIRATE) return;                                    // не пиратские поселения
	if (!CheckAttribute(pchar, "CargoQuest.GovSwapOffer")) return;    // ролл 25% при взятии
	if (CheckAttribute(pchar, "CargoQuest.GovSwapSpawned")) return;   // предложение одноразовое
	if (CheckAttribute(pchar, "CargoQuest.GovSwapDone")) return;      // уже согласился
	if (!CheckQuestAttribute("generate_trade_quest_progress", "begin")) return; // фрахт активен
	if (!CheckAttribute(pchar, "CargoQuest.GiveTraderID")) return;
	int iGiver = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
	if (iGiver < 0) return;
	string sOriginCity = characters[iGiver].city;
	if (Colonies[iColony].id != sOriginCity) return;                  // только в городе-отправителе

	pchar.CargoQuest.GovSwapSpawned = true; // пропустил дельца - повезёшь дальше, больше не появится

	int iChar = NPC_GeneratePhantomCharacter("citizen", iNation, MAN, 1); // LifeDay=1 - исчезнет на след. сутки
	ref chr = &characters[iChar];
	SetNPCModelUniq(chr, "citizen", MAN);
	chr.City     = Colonies[iColony].id;
	chr.CityType = "citizen";
	LAi_SetLoginTime(chr, 0.0, 23.99);
	ChangeCharacterAddressGroup(chr, pchar.location, "reload", "reload1_back"); // спавн у портового локатора (точка прибытия ГГ)
	chr.dialog.filename    = "GovSwap_dialog.c";
	chr.dialog.currentnode = "First time";
	chr.greeting           = "";
	chr.talker             = 1;
	if (InterfaceStates.EnabledQuestsMarks) // жёлтый "?" над дельцом (снимается после разговора в GovSwap_RemoveNpc)
	{
		chr.quest.questflag.model = "questionmarkY";
		chr.quest.questflag.technique = GetTechNameForSign(); // БЕЗ technique значок не рендерится!
	}
	pchar.CargoQuest.GovSwapDealerId = chr.id;
	LAi_SetActorTypeNoGroup(chr);
	LAi_ActorDialog(chr, pchar, "", 25.0, 0); // подбегает и сам заводит разговор
}

// НАДЁЖНЫЙ триггер: зовётся на КАЖДОМ входе в локацию (LAi_login.c), в отличие от CreateCitizens
// он НЕ гейтится isLocationHasCitizens - поэтому делец появляется и при выходе из магазина в город/порт.
void GovSwap_TryDealerOnEnter(aref loc)
{
	if (!CheckAttribute(pchar, "CargoQuest.iOrderType")) return;
	if (sti(pchar.CargoQuest.iOrderType) != 6) return;       // только губернаторский - чтобы не молотить каждый вход
	if (!CheckAttribute(loc, "fastreload")) return;          // не колониальная локация (каюта/палуба/etc)
	int iColony = FindColony(loc.fastreload);
	if (iColony < 0) return;
	int iNation = sti(Colonies[iColony].nation);
	TrySpawnGovSwapDealer(loc, iColony, iNation);
}

// игрок согласился на подмену: делец ЗАБИРАЕТ настоящий груз и выдаёт самый дешёвый товар (тот же объём),
// его и повезём губернатору. Бонус считается от НАСТОЯЩЕГО груза.
void GovSwap_DoSwap()
{
	int realGood = sti(pchar.CargoQuest.iTradeGoods);
	int qty = sti(pchar.CargoQuest.iQuantityGoods);
	int bonus = GovSwap_Bonus();
	int fakeGood = GovSwap_SubstituteGood(realGood, 2); // дешевле + та же категория, вариативно по dRand
	RemoveCharacterGoods(pchar, realGood, qty); // делец забрал настоящий груз
	AddCharacterGoods(pchar, fakeGood, qty);    // взамен - дешёвка под казённой пломбой
	pchar.CargoQuest.GovSwapFakeGood = fakeGood;
	AddMoneyToCharacter(pchar, bonus);
	pchar.CargoQuest.GovSwapDone = true;
	ReOpenQuestHeader("DELIVERY_TRADE_QUEST");
	AddQuestRecord("DELIVERY_TRADE_QUEST", "gov3");
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sSex", GetSexPhrase("ся", "ась"));
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sMoney", FindRussianMoneyString(bonus));
}

// мэр зовёт таможенника: спавн в резиденции (локации ГГ), подходит к ГГ. iNation = нация мэра.
// LEO: дверь, у которой появляется досмотрщик, - первый переход самой локации. Жёстко заданного reload1_back
//   нет, например, в резиденции Мариго (там reload1): ChangeCharacterAddressGroup молча не ставил NPC, а id уже
//   был записан - и досмотрщик не появлялся больше никогда (репорт 25.09.2026).
string GovSwap_DoorLocator()
{
	int iLoc = FindLocation(pchar.location);
	if (iLoc >= 0)
	{
		if (CheckAttribute(&Locations[iLoc], "reload.l1.name")) return Locations[iLoc].reload.l1.name;
	}
	return "reload1_back";
}

void GovSwap_SpawnInspector(int iNation)
{
	// уже идёт к ГГ в этой же комнате - второго не зовём
	if (CheckAttribute(pchar, "CargoQuest.GovSwapInspectorId"))
	{
		int iOld = GetCharacterIndex(pchar.CargoQuest.GovSwapInspectorId);
		if (iOld >= 0)
		{
			if (characters[iOld].location == pchar.location && !LAi_IsDead(&characters[iOld])) return;
		}
	}
	int iChar = NPC_GeneratePhantomCharacter("citizen", iNation, MAN, 1);
	ref chr = &characters[iChar];
	SetNPCModelUniq(chr, "citizen", MAN);
	chr.CityType = "citizen";
	LAi_SetLoginTime(chr, 0.0, 23.99);
	ChangeCharacterAddressGroup(chr, pchar.location, "reload", GovSwap_DoorLocator()); // появляется у входа
	chr.dialog.filename    = "GovSwap_dialog.c";
	chr.dialog.currentnode = "Inspector_Check";
	chr.greeting           = "";
	chr.talker             = 1;
	pchar.CargoQuest.GovSwapInspectorId = chr.id;
	LAi_SetActorTypeNoGroup(chr);
	LAi_ActorDialog(chr, pchar, "", 25.0, 0); // подходит и сам заводит досмотр
}

// убрать временного NPC из локации (по id)
void GovSwap_RemoveNpc(string sId)
{
	int idx = GetCharacterIndex(sId);
	if (idx < 0) return;
	ref m = &characters[idx];
	UnmarkCharacter(m); // снять "?" после разговора (канон: чистит model/technique, ПОТОМ атрибут - иначе билборд остаётся висеть)
	LAi_RemoveCheckMinHP(m);
	DeleteAttribute(m, "items");
	m.money   = 0;
	m.lifeDay = 0; // удалится при перезагрузке локации
	LAi_SetActorType(m);
	LAi_ActorRunToLocation(m, "reload", LAi_FindNearestFreeLocator2Pchar("reload"), "none", "", "", "", -1); // уходит из локации
}

// делец уходит после разговора (вешается на выход из диалога)
void GovSwap_DealerLeave()
{
	if (!CheckAttribute(pchar, "CargoQuest.GovSwapDealerId")) return;
	GovSwap_RemoveNpc(pchar.CargoQuest.GovSwapDealerId);
	DeleteAttribute(pchar, "CargoQuest.GovSwapDealerId");
}

// таможенник уходит после досмотра (вешается на выход из диалога)
void GovSwap_InspectorLeave()
{
	if (!CheckAttribute(pchar, "CargoQuest.GovSwapInspectorId")) return;
	GovSwap_RemoveNpc(pchar.CargoQuest.GovSwapInspectorId);
	DeleteAttribute(pchar, "CargoQuest.GovSwapInspectorId");
}

// чистка всех флагов подмены (при завершении фрахта)
void GovSwap_Clear()
{
	DeleteAttribute(pchar, "CargoQuest.GovSwapOffer");
	DeleteAttribute(pchar, "CargoQuest.GovSwapDone");
	DeleteAttribute(pchar, "CargoQuest.GovSwapCaught");
	DeleteAttribute(pchar, "CargoQuest.GovSwapSpawned");
	DeleteAttribute(pchar, "CargoQuest.GovSwapInspected");
	DeleteAttribute(pchar, "CargoQuest.GovSwapDealerId");
	DeleteAttribute(pchar, "CargoQuest.GovSwapInspectorId");
	DeleteAttribute(pchar, "CargoQuest.GovSwapFinePct");
	DeleteAttribute(pchar, "CargoQuest.GovSwapBonusPct");
	DeleteAttribute(pchar, "CargoQuest.GovSwapFakeGood");
	DeleteAttribute(pchar, "CargoQuest.GovSubtype");
	// ЭСКОРТ: на любом закрытии фрахта (сдача/провал/лазейка) распускаем казённые суда эскорта
	if (CheckAttribute(pchar, "CargoQuest.GovEscortActive"))
	{
		GovEscort_Disband();
		DeleteAttribute(pchar, "CargoQuest.GovEscortActive");
	}
	DeleteAttribute(pchar, "CargoQuest.GovEscortShips");
	// ЭСКОРТ, вариант агента: убрать посыльного, разоружить засаду у острова, снять флаги перенаправления
	GovEscortAgent_Cleanup();
	GovEscortAmbush_Disarm();
	DeleteAttribute(pchar, "CargoQuest.GovEscortAgentZ");
	DeleteAttribute(pchar, "CargoQuest.GovEscortAgentMult");
	DeleteAttribute(pchar, "CargoQuest.GovEscortFought");
	DeleteAttribute(pchar, "GovEscortAgentResolved");
	// GovSwapMayorId чистит GovSwap_MayorReseat (на выходе из диалога вердикта); GovFineDebt.<city> НЕ трогаем - копится до погашения
}

// после диалога досмотрщика - бесшовно передаём слово ГУБЕРНАТОРУ (без повторного клика).
// Вешается на выход из диалога досмотрщика (AddDialogExitQuestFunction в Inspector_Check).
void GovSwap_AfterInspector()
{
	// убрать таможенника
	if (CheckAttribute(pchar, "CargoQuest.GovSwapInspectorId"))
	{
		GovSwap_RemoveNpc(pchar.CargoQuest.GovSwapInspectorId);
		DeleteAttribute(pchar, "CargoQuest.GovSwapInspectorId");
	}
	// мгновенно открыть диалог мэра на узле вердикта (LAi_ActorDialogNow = dialogcan, без подбегания)
	if (!CheckAttribute(pchar, "CargoQuest.GovSwapMayorId")) return;
	int mi = GetCharacterIndex(pchar.CargoQuest.GovSwapMayorId);
	if (mi < 0) return;
	ref m = &characters[mi];
	m.dialog.filename = "Common_Mayor.c";       // гарантируем, что узел вердикта найдётся
	m.dialog.currentnode = "GovFreight_Deliver"; // переоткрываем узел сдачи - там сработает ветка вердикта (caught/passed)
	LAi_SetActorType(m);
	LAi_ActorDialogNow(m, pchar, "", -1);
}

// вернуть мэру ИСХОДНЫЙ тип после диалога вердикта (его актёризовали для LAi_ActorDialogNow).
// Тип снят снапшотом до актёризации (GovSwapMayorType): мэры сидят ("sit"), губернаторы столиц стоят ("huber" и пр.).
void GovSwap_MayorReseat()
{
	if (!CheckAttribute(pchar, "CargoQuest.GovSwapMayorId")) return;
	int mi = GetCharacterIndex(pchar.CargoQuest.GovSwapMayorId);
	if (mi >= 0)
	{
		string t = "";
		if (CheckAttribute(pchar, "CargoQuest.GovSwapMayorType")) t = pchar.CargoQuest.GovSwapMayorType;
		ref m = &characters[mi];
		if (t == "sit") LAi_SetSitTypeNoGroup(m);
		else if (t == "huberstay") LAi_SetHuberStayTypeNoGroup(m);
		else if (t == "stay") LAi_SetStayTypeNoGroup(m);
		else LAi_SetHuberTypeNoGroup(m); // "huber" и фоллбек - стоячий губернатор
	}
	DeleteAttribute(pchar, "CargoQuest.GovSwapMayorId");
	DeleteAttribute(pchar, "CargoQuest.GovSwapMayorType");
}

// записать неустойку губернатору города (когда ГГ не смог оплатить штраф за подмену деньгами)
void GovSwap_AddFineDebt(string sCity, int amount)
{
	if (amount <= 0) return;
	int cur = 0;
	if (CheckAttribute(pchar, "GovFineDebt." + sCity)) cur = sti(pchar.GovFineDebt.(sCity));
	pchar.GovFineDebt.(sCity) = cur + amount;
}

// удержать неустойку из премии при доставке губ.фрахта в этот город. Возвращает урезанную премию.
int GovSwap_WithholdDebt(int iReward, string sCity)
{
	if (!CheckAttribute(pchar, "GovFineDebt." + sCity)) return iReward;
	int debt = sti(pchar.GovFineDebt.(sCity));
	if (debt <= 0) { DeleteAttribute(pchar, "GovFineDebt." + sCity); return iReward; }
	int withhold = debt;
	if (withhold > iReward) withhold = iReward;
	debt = debt - withhold;
	if (debt <= 0) DeleteAttribute(pchar, "GovFineDebt." + sCity);
	else pchar.GovFineDebt.(sCity) = debt;
	return iReward - withhold;
}

// штраф размером пакета (одинаков в тексте узла и здесь): % запечён при взятии, от базовой стоимости груза
int GovSwap_FineAmount()
{
	int g   = sti(pchar.CargoQuest.iTradeGoods);
	int qty = sti(pchar.CargoQuest.iQuantityGoods);
	int pct = 30;
	if (CheckAttribute(pchar, "CargoQuest.GovSwapFinePct")) pct = sti(pchar.CargoQuest.GovSwapFinePct);
	return makeint(GetCargoBaseCost(g, qty) * pct / 100);
}

// УСПЕШНАЯ доставка губ.фрахта (обычная ИЛИ удачная подмена): деньги/репутация/опыт/груз в магазин/закрытие.
// Удерживает неустойку прошлой подмены из премии. Возвращает выплаченную (урезанную) сумму.
int GovSwap_DeliverReward(ref mayor)
{
	int g   = GovSwap_HoldGood(); // что реально в трюме: дешёвка (если подмена) либо настоящий груз
	int qty = sti(pchar.CargoQuest.iQuantityGoods);
	int reward = sti(pchar.CargoQuest.iMoney);
	bool late = CheckQuestAttribute("generate_trade_quest_progress", "failed");
	if (late) reward = makeint(reward / 2);
	else { if (GetQuestPastDayParam("CargoQuest") * 2 < sti(pchar.CargoQuest.iDaysExpired)) reward = makeint(reward * 1.2); }
	string city = mayor.city;
	reward = GovSwap_WithholdDebt(reward, city); // удержать неустойку прошлой подмены, если числится за этим городом
	AddMoneyToCharacter(pchar, reward);
	Freight_AddLoyalty(); // LEO: +1% к будущей премии выдавшего работодателя за выполненный губернаторский фрахт
	ChangeCharacterNationReputation(pchar, sti(mayor.nation), 5);
	ChangeCharacterReputation(pchar, 1);
	int daysMul = GetQuestPastDayParam("CargoQuest") + 1;
	if (late) daysMul = 1;
	AddCharacterExpToSkill(pchar, "Sailing", daysMul * sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_I));
	AddCharacterExpToSkill(pchar, "Leadership", daysMul * sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_C));
	AddCharacterExpToSkill(pchar, "Commerce", daysMul * sti(pchar.rank) * (GetCharacterSPECIALSimple(pchar, SPECIAL_C) + GetCharacterSPECIALSimple(pchar, SPECIAL_I)));
	RemoveCharacterGoods(pchar, g, qty);
	ref st = GetColonyByIndex(FindColony(city));
	AddStoreGoods(&stores[sti(st.StoreNum)], g, qty);
	pchar.quest.generate_trade_quest_progress = "";
	pchar.quest.generate_trade_quest.over = "yes";
	ReOpenQuestHeader("DELIVERY_TRADE_QUEST");
	AddQuestRecord("DELIVERY_TRADE_QUEST", "gov2");
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(g));
	GovSwap_Clear();
	CloseQuestHeader("DELIVERY_TRADE_QUEST");
	AchievementsCounter_genquests(1);
	return reward;
}

// РАСКРЫТАЯ подмена: штраф 20-40% (деньгами; не хватило - неустойка на губернаторе), -репа, БЕЗ опыта, груз конфискуют.
void GovSwap_CaughtPenalty(ref mayor)
{
	int g   = GovSwap_HoldGood(); // конфискуют то, что в трюме (дешёвку); штраф же считается от НАСТОЯЩЕГО груза (GovSwap_FineAmount)
	int qty = sti(pchar.CargoQuest.iQuantityGoods);
	int fine = GovSwap_FineAmount();
	int money = makeint(pchar.money);
	int paid = fine;
	if (paid > money) paid = money;
	if (paid > 0) AddMoneyToCharacter(pchar, -paid);
	int rest = fine - paid;
	string city = mayor.city;
	if (rest > 0) GovSwap_AddFineDebt(city, rest); // денег не хватило (спрятал в сундук) - повесить неустойку на губернатора (без блокировки)
	ChangeCharacterReputation(pchar, -3);
	RemoveCharacterGoods(pchar, g, qty); // подделку конфискуют
	pchar.quest.generate_trade_quest_progress = "";
	pchar.quest.generate_trade_quest.over = "yes";
	ReOpenQuestHeader("DELIVERY_TRADE_QUEST");
	AddQuestRecord("DELIVERY_TRADE_QUEST", "gov4");
	GovSwap_Clear();
	CloseQuestHeader("DELIVERY_TRADE_QUEST");
	// опыт НЕ даём - по деньгам ГГ всё равно в плюсе (премия дельца больше штрафа)
}
