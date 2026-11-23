// Warship 17.07.08 Свободное плавание компаньонов

// LEO: движок шлёт это, когда энкаунтер-компаньон догнал игрока на глобалке (WdmCompanionShip)
#event_handler("WorldMap_CompanionMet", "CompanionTravel_OnCompanionMet");
// LEO: ...и когда рейс ТУДА дошёл до города-склада
#event_handler("WorldMap_CompanionArrived", "CompanionTravel_OnCompanionArrived");

// LEO: Каюта и кают-компания говорят через клона; индекс капитана важнее совпадающих имён.
int CompanionTravel_DepartureIndex(ref speaker)
{
	int index = -1;
	if (CheckAttribute(speaker, "realcompanionidx")) index = sti(speaker.realcompanionidx);
	else
	{
		if (CheckAttribute(speaker, "IsCompanionClone"))
		{
			if (sti(speaker.IsCompanionClone) != 0) return -1;
		}
		if (CheckAttribute(speaker, "index")) index = sti(speaker.index);
	}
	if (index < 0 || index >= TOTAL_CHARACTERS) return -1;
	ref captain = GetCharacter(index);
	if (!CheckAttribute(captain, "id")) return -1;
	if (CheckAttribute(speaker, "IsCompanionClone"))
	{
		if (sti(speaker.IsCompanionClone) != 0)
		{
			if (!CheckAttribute(speaker, "index")) return -1;
			if (!CheckAttribute(captain, "Tasks.Clone")) return -1;
			if (sti(captain.Tasks.Clone) != sti(speaker.index)) return -1;
		}
	}
	return index;
}

// LEO: Проверяем живые сцепки до оплаты и назначения рейса.
bool CompanionTravel_CanDepart(ref speaker)
{
	int index = CompanionTravel_DepartureIndex(speaker);
	if (index < 0) return false;
	ref captain = GetCharacter(index);
	return !SeaTowOperations_IsLinked(captain);
}

string CompanionTravel_DepartureBlockedText()
{
	return XI_ConvertString("SeaTowOperationsDetach");
}

bool CompanionTravel_CheckDeparture(ref speaker)
{
	if (CompanionTravel_CanDepart(speaker)) return true;
	SeaTowOperations_Warn();
	return false;
}

// LEO: Отложенное закрытие окна и прямой вызов торговли повторно проверяют того же капитана.
bool CompanionTravel_CheckDepartureContext(string field)
{
	if (!CheckAttribute(PChar, "CompanionTravel." + field)) return true;
	int index = sti(PChar.CompanionTravel.(field));
	if (index < 0 || index >= TOTAL_CHARACTERS) return false;
	if (CheckAttribute(PChar, "CompanionTravel.PendingSendNpc"))
	{
		int sender = sti(PChar.CompanionTravel.PendingSendNpc);
		if (sender < 0 || sender >= TOTAL_CHARACTERS) return false;
		if (CompanionTravel_DepartureIndex(GetCharacter(sender)) != index) return false;
	}
	return CompanionTravel_CheckDeparture(GetCharacter(index));
}

// LEO: Отказанное окно не оставляет отложенную отправку для следующего магазина; постоянные рейсы не трогаем.
void CompanionTravel_ClearDepartureContext(string field)
{
	if (field == "BuyChrIdx")
	{
		DeleteAttribute(PChar, "CompanionTravel.BuyChrIdx");
		DeleteAttribute(PChar, "CompanionTravel.BuyStoreNum");
		DeleteAttribute(PChar, "CompanionTravel.BuySnap");
		DeleteAttribute(PChar, "CompanionTravel.BuyMoneySnap");
	}
	else
	{
		if (field != "StorageChrIdx") return;
		DeleteAttribute(PChar, "CompanionTravel.StorageChrIdx");
		DeleteAttribute(PChar, "CompanionTravel.Snap");
	}
	DeleteAttribute(PChar, "CompanionTravel.PendingSendNpc");
}

// LEO: Проверяем и меню, и финальные/динамические ноды: старый сохранённый диалог не обходит запрет.
bool CompanionTravel_IsDepartureNode(string node)
{
	// LEO: Старые ссылки используют Exit_Fire вместо exit_fire; увольнение и назначения тоже выводят капитана.
	node = GetStrSmallRegister(node);
	if (node == "companiontravel") return true;
	if (node == "companiontravel_start") return true;
	if (node == "companiontravel_storagecargo") return true;
	if (findSubStr(node, "companiontravel_preparestart", 0) == 0) return true;
	if (findSubStr(node, "companiontravelto", 0) == 0) return true;
	if (findSubStr(node, "travel_", 0) == 0) return true;
	if (findSubStr(node, "travelto", 0) == 0) return true;
	if (findSubStr(node, "commerce_", 0) == 0) return true;
	if (findSubStr(node, "auction_", 0) == 0) return true;
	if (findSubStr(node, "companion_buycargo", 0) == 0) return true;
	if (findSubStr(node, "officeroutfit_", 0) == 0) return true;
	if (findSubStr(node, "exit_fire", 0) == 0) return true;
	if (findSubStr(node, "get_out_", 0) == 0) return true;
	if (findSubStr(node, "return_items_", 0) == 0) return true;
	if (findSubStr(node, "im_kind_", 0) == 0) return true;
	if (node == "asyouwish") return true;
	if (node == "wanttogo_free") return true;
	if (findSubStr(node, "cancelcontract", 0) == 0) return true;
	if (node == "boal_stay") return true;
	if (node == "gover_hire" || node == "exit_gover_hire") return true;
	if (node == "colonybuilding_hovernor_1" || node == "colonybuilding_hovernor_2") return true;
	if (node == "colonyguarding_hovernor_1" || node == "colonyguarding_hovernor_2") return true;
	if (node == "minescommander_hire" || node == "minescommander_hired") return true;
	if (node == "plantationcommander_hire" || node == "plantationcommander_hired") return true;
	return false;
}

// LEO: текстовый id колонии для XI_ConvertString. Большинство колоний именуются "Colony<id><падеж>",
//   но четыре острова - без префикса. Отдельными if, без цепочки || (сторм-гоча на составных условиях).
string CompanionTravel_ColonyTextId(string sColonyId, string sCase)
{
	if (sColonyId == "Caiman")   return sColonyId + sCase;
	if (sColonyId == "Dominica") return sColonyId + sCase;
	if (sColonyId == "StLucia")  return sColonyId + sCase;
	if (sColonyId == "Terks")    return sColonyId + sCase;
	return "Colony" + sColonyId + sCase;
}

// LEO: идёт ли корабль сейчас ЖИВЫМ рейсом (туда на склад / обратно к эскадре). Пока идёт - он не
//   останавливается: не ведёт переговоров в море и не открывает командный диалог. Два флага раздельно
//   (сторм-гоча на составных условиях - без цепочки ||).
bool CompanionTravel_IsOnLiveRoute(ref rChar)
{
	if (CheckAttribute(rChar, "CompanionTravel.OutboundLive")) return true;
	if (CheckAttribute(rChar, "CompanionTravel.Meeting"))      return true;
	return false;
}

// LEO: арендован ли у нас склад в этом городе. Факт аренды живёт НА ЛАВОЧНИКЕ (<colony>_trader):
//   Common_Store при аренде пишет NPChar.Storage.Activate, при просрочке - удаляет. На store-объект
//   этот флаг лишь ЗЕРКАЛИТСЯ, и то лениво - только когда игрок зайдёт к лавочнику. Поэтому проверяем
//   первоисточник (лавочника), а зеркало store - как запасной путь на случай нестандартного id.
bool CompanionTravel_HasStorage(string sColonyId)
{
	int iTrader = GetCharacterIndex(sColonyId + "_trader");
	if (iTrader >= 0 && CheckAttribute(&characters[iTrader], "Storage.Activate")) return true;
	int iStore = FindStore(sColonyId);
	if (iStore >= 0 && CheckAttribute(&stores[iStore], "Storage.Activate")) return true;
	return false;
}

// LEO: есть ли у нас хоть один арендованный склад - нужно, чтобы не показывать пустой пункт в диалоге
bool CompanionTravel_HasAnyStorage()
{
	int n;
	for (n = 0; n < MAX_COLONIES; n++)
	{
		if (colonies[n].nation == "none") continue;
		if (colonies[n].id == "Panama") continue;
		if (CompanionTravel_HasStorage(colonies[n].id)) return true;
	}
	return false;
}

// ПЕРЕТАРКА ПЕРЕД РЕЙСОМ: снимаем снимок склада ДО, после закрытия окна считаем разницу.
//   склад пополнился -> отвезти: товар обратно в трюм компаньона; убыл -> забрать: снять с трюма,
//   на складе НЕ возвращаем (это резерв, не продастся, пока компаньон в пути). Манифест - на компаньоне.

void CompanionTravel_StorageSnapshot(int iStore)
{
	if (!CompanionTravel_CheckDepartureContext("StorageChrIdx")) return;
	int i;
	string sKey;
	DeleteAttribute(PChar, "CompanionTravel.Snap");
	for (i = 0; i < GOODS_QUANTITY; i++)
	{
		sKey = "" + i; // ключ - строковая ПЕРЕМЕННАЯ: .(выражение) сторм не понимает
		PChar.CompanionTravel.Snap.(sKey) = GetStorageGoodsQuantity(&stores[iStore], i);
	}
}

void CompanionTravel_StorageCommit(ref rCompanion, int iStore)
{
	if (!CompanionTravel_CheckDeparture(rCompanion)) return;
	int i, iWas, iNow, d;
	string sKey;
	DeleteAttribute(rCompanion, "CompanionTravel.Deliver");
	DeleteAttribute(rCompanion, "CompanionTravel.Pickup");
	for (i = 0; i < GOODS_QUANTITY; i++)
	{
		sKey = "" + i;
		iWas = 0;
		if (CheckAttribute(PChar, "CompanionTravel.Snap." + sKey)) iWas = sti(PChar.CompanionTravel.Snap.(sKey));
		iNow = sti(GetStorageGoodsQuantity(&stores[iStore], i));
		d = iNow - iWas;
		if (d > 0)
		{
			SetStorageGoods(&stores[iStore], i, iWas);   // склад возвращаем как был
			AddCharacterGoodsSimple(rCompanion, i, d);   // груз обратно в трюм - повезёт сам
			rCompanion.CompanionTravel.Deliver.(sKey) = d;
		}
		if (d < 0)
		{
			RemoveCharacterGoodsSelf(rCompanion, i, -d); // из трюма убрать: заберёт по прибытии
			rCompanion.CompanionTravel.Pickup.(sKey) = -d; // со склада НЕ возвращаем - это резерв
		}
	}
	DeleteAttribute(PChar, "CompanionTravel.Snap");
}

// ==================== ТОРГОВЫЙ РЕЙС ====================
// LEO: компаньон везёт свой корабль в город на торг (окно магазина, гейт по казначею). Закупка - оплата СРАЗУ при
//   отправке, товар в трюме; продажа - продаёт в городе, выручку ГГ получает ПРИ ВСТРЕЧЕ. Цены отправки запоминаются
//   -> по прибытии сверяется дрейф (доплата/возврат по закупке, выручка по цене города). Опыт Commerce - компаньону.

// снапшот трюма компаньона + кошелька ГГ до магазина (BuyCommit сверит: манифест + реально списанное)
void CompanionTravel_BuySnapshot(int iChr)
{
	if (iChr < 0 || iChr >= TOTAL_CHARACTERS) return;
	if (!CompanionTravel_CheckDeparture(GetCharacter(iChr))) return;
	int i;
	string sKey;
	DeleteAttribute(PChar, "CompanionTravel.BuySnap");
	PChar.CompanionTravel.BuyMoneySnap = sti(PChar.money); // кошелёк до торговли
	for (i = 0; i < GOODS_QUANTITY; i++)
	{
		sKey = "" + i;
		PChar.CompanionTravel.BuySnap.(sKey) = GetCargoGoods(&characters[iChr], i);
	}
}

// магазин закрыт (store.c IDoExit): по итогу торговли определяем тип рейса (TradeKind) и расчёт. Берём РЕАЛЬНУЮ дельту
//   кошелька (iSpent, store.c посчитал верно); цены отправки пишем в TradePrice - для сверки дрейфа по прибытии.
void CompanionTravel_BuyCommit()
{
	if (!CheckAttribute(PChar, "CompanionTravel.BuyChrIdx")) return;
	if (!CompanionTravel_CheckDepartureContext("BuyChrIdx"))
	{
		CompanionTravel_ClearDepartureContext("BuyChrIdx");
		return;
	}
	int iChr = sti(PChar.CompanionTravel.BuyChrIdx);
	if (iChr < 0 || iChr >= TOTAL_CHARACTERS) return;
	ref sld = &characters[iChr];
	int iSend = iChr;
	if (CheckAttribute(PChar, "CompanionTravel.PendingSendNpc")) iSend = sti(PChar.CompanionTravel.PendingSendNpc);
	int iStore = sti(PChar.CompanionTravel.BuyStoreNum); // стор города - для цен ОТПРАВКИ (по прибытии сверим дрейф)
	int iMoneyWas = 0;
	if (CheckAttribute(PChar, "CompanionTravel.BuyMoneySnap")) iMoneyWas = sti(PChar.CompanionTravel.BuyMoneySnap);
	int iSpent = iMoneyWas - sti(PChar.money); // РЕАЛЬНАЯ дельта кошелька: >0 закупка (потрачено), <0 продажа (выручено)
	int i;
	string sKey;
	bool bAnyBuy = false;
	bool bAnySell = false;
	DeleteAttribute(sld, "CompanionTravel.TradeList");
	DeleteAttribute(sld, "CompanionTravel.TradePrice");
	DeleteAttribute(sld, "CompanionTravel.SellItem");
	DeleteAttribute(sld, "CompanionTravel.TradeKind");
	// тип: докуплено -> закупка (товар в трюме), продано -> продажа. Цены ОТПРАВКИ пишем в TradePrice - для сверки дрейфа.
	for (i = 0; i < GOODS_QUANTITY; i++)
	{
		sKey = "" + i;
		int iWas = 0;
		if (CheckAttribute(PChar, "CompanionTravel.BuySnap." + sKey)) iWas = sti(PChar.CompanionTravel.BuySnap.(sKey));
		int iDelta = GetCargoGoods(sld, i) - iWas;
		if (iDelta > 0)
		{
			bAnyBuy = true;
			sld.CompanionTravel.TradeList.(sKey)  = iDelta; // манифест закупки (товар уже в трюме - для сверки дрейфа)
			sld.CompanionTravel.TradePrice.(sKey) = GetStoreGoodsPrice(&stores[iStore], i, PRICE_TYPE_BUY, PChar, 1);
		}
		else if (iDelta < 0) { bAnySell = true; sld.CompanionTravel.SellItem.(sKey) = -iDelta; }
	}
	DeleteAttribute(PChar, "CompanionTravel.BuySnap");
	DeleteAttribute(PChar, "CompanionTravel.BuyMoneySnap");
	DeleteAttribute(PChar, "CompanionTravel.BuyChrIdx");
	DeleteAttribute(PChar, "CompanionTravel.BuyStoreNum");
	DeleteAttribute(PChar, "CompanionTravel.PendingSendNpc");
	// откат денег окна (ТИХО) - видимый расчёт покажем сами в нужный момент (закупка - сейчас, продажа - при встрече)
	setShowMoneyLogValue(false);
	AddMoneyToCharacter(PChar, iSpent);
	if (!bAnyBuy && !bAnySell) // ничего / перекуп-в-ноль: рейса нет (SetTraveller не звался, деньги уже вернули)
	{
		DeleteAttribute(sld, "CompanionTravel"); // чистим весь остаток рейса (ToColonyID/Days/Return/Task), как складская отмена
		Log_Info("Торговых операций нет - отправка отменена.");
		return;
	}
	int iVol = iSpent;
	if (iVol < 0) iVol = -iVol;
	if (iVol > 0) AddCharacterExpToSkill(sld, "Commerce", iVol / 2600.0); // опыт компаньону по обороту
	sld.CompanionTravel.TradeSpent = iSpent; // сумма ОТПРАВКИ для записи СЖ (по прибытии пересчитается по дрейфу)
	sld.CompanionTravel.Task = "buy";
	if (bAnyBuy)
	{
		// ЗАКУПКА: платим сразу (видимо), товар в трюме. Манифест+цены отправки оставляем - дрейф сверим по прибытии.
		DeleteAttribute(sld, "CompanionTravel.SellItem"); // бартер-продажи движок провёл вживую - не откладываем
		AddMoneyToCharacter(PChar, -iSpent); // оплата закупки сейчас
		sld.CompanionTravel.TradeKind = "buy";
		Log_Info("Казначей оплатил закупку компаньону.");
		CompanionTravel_SetTraveller(&characters[iSend]);
		return;
	}
	// ПРОДАЖА: докупок нет. Товар назад в трюм (повезёт), продаст В ГОРОДЕ по ТАМОШНЕЙ цене; выручку ГГ получит ПРИ ВСТРЕЧЕ.
	for (i = 0; i < GOODS_QUANTITY; i++)
	{
		sKey = "" + i;
		if (!CheckAttribute(sld, "CompanionTravel.SellItem." + sKey)) continue;
		int iSold = sti(sld.CompanionTravel.SellItem.(sKey));
		AddCharacterGoods(sld, i, iSold);              // товар назад в трюм
		sld.CompanionTravel.TradeList.(sKey) = -iSold; // минус = продать в городе (TradeAtArrival)
	}
	DeleteAttribute(sld, "CompanionTravel.SellItem");
	sld.CompanionTravel.TradeKind = "sale";
	Log_Info("Товар отправлен на продажу с компаньоном - расчёт при встрече.");
	CompanionTravel_SetTraveller(&characters[iSend]);
}

// по прибытии + сверка ДРЕЙФА цен. Закупка (TradeList>0): товар в трюме, считаем разницу цен (сейчас - отправка).
//   Продажа (TradeList<0): продаём здесь по тамошней цене. Итог -> TradeSpent (доплата/возврат / -выручка); деньги при встрече.
void CompanionTravel_TradeAtArrival(ref sld)
{
	if (!CheckAttribute(sld, "CompanionTravel.TradeList")) return;
	string sColony = "";
	if (CheckAttribute(sld, "CompanionTravel.ToColonyID")) sColony = sld.CompanionTravel.ToColonyID;
	int iCol = FindColony(sColony);
	int iStore = -1;
	if (iCol >= 0) iStore = sti(colonies[iCol].StoreNum); // стор колонии напрямую (пираты-безопасно)
	int i;
	int iDrift = 0;
	int iProceeds = 0;
	string sKey;
	for (i = 0; i < GOODS_QUANTITY; i++)
	{
		sKey = "" + i;
		if (!CheckAttribute(sld, "CompanionTravel.TradeList." + sKey)) continue;
		int iDelta = sti(sld.CompanionTravel.TradeList.(sKey));
		if (iDelta > 0) // ЗАКУПКА: товар в трюме, сверяем дрейф цены за рейс
		{
			if (iStore >= 0 && CheckAttribute(sld, "CompanionTravel.TradePrice." + sKey))
			{
				int iPaid = sti(sld.CompanionTravel.TradePrice.(sKey));
				int iNow  = GetStoreGoodsPrice(&stores[iStore], i, PRICE_TYPE_BUY, PChar, 1);
				iDrift += (iNow - iPaid) * iDelta / sti(Goods[i].Units); // подорожало -> доплата (>0), подешевело -> возврат
			}
		}
		else // ПРОДАЖА: продаём здесь по ТАМОШНЕЙ цене
		{
			int q = -iDelta;
			int have = GetCargoGoods(sld, i);
			if (q > have) q = have; // подстраховка (трюм мог измениться в пути)
			if (q > 0)
			{
				RemoveCharacterGoods(sld, i, q);
				if (iStore >= 0) iProceeds += GetStoreGoodsPrice(&stores[iStore], i, PRICE_TYPE_SELL, PChar, 1) * q / sti(Goods[i].Units);
			}
		}
	}
	sld.CompanionTravel.TradeSpent = iDrift - iProceeds; // закупка: доплата (>0)/возврат (<0); продажа: -выручка
	DeleteAttribute(sld, "CompanionTravel.TradeList");
	DeleteAttribute(sld, "CompanionTravel.TradePrice");
}

// расчёт при ФИЗИЧЕСКОЙ встрече с ГГ (возврат/забор в порту), без телепорта: продажа - выручка; закупка - дрейф цены
//   (база оплачена при отправке). Итог уже в TradeSpent. Одноразово (гейт по TradeKind).
void CompanionTravel_SettleTrade(ref sld)
{
	// сбор с торгов: компаньон привёз выручку - отдаём ГГ вживую (одноразово)
	if (CheckAttribute(sld, "CompanionTravel.AuctionMoney"))
	{
		int m = sti(sld.CompanionTravel.AuctionMoney);
		DeleteAttribute(sld, "CompanionTravel.AuctionMoney");
		if (m > 0)
		{
			AddMoneyToCharacter(PChar, m);
			Log_Info("Компаньон привёз выручку с наших торгов.");
			PlayStereoSound("interface\bort_jurnal.wav");
		}
	}
	// снаряжение офицера: сдача с выданного бюджета - отдаём ГГ вживую (одноразово)
	if (CheckAttribute(sld, "CompanionTravel.OutfitChange"))
	{
		int mo = sti(sld.CompanionTravel.OutfitChange);
		DeleteAttribute(sld, "CompanionTravel.OutfitChange");
		if (mo > 0)
		{
			AddMoneyToCharacter(PChar, mo);
			Log_Info("Офицер вернул сдачу с выданного на снаряжение.");
			PlayStereoSound("interface\bort_jurnal.wav");
		}
	}
	if (!CheckAttribute(sld, "CompanionTravel.TradeKind")) return; // не торговый рейс / уже рассчитан
	string sKind = sld.CompanionTravel.TradeKind;
	int iSettle = 0;
	if (CheckAttribute(sld, "CompanionTravel.TradeSpent")) iSettle = sti(sld.CompanionTravel.TradeSpent);
	DeleteAttribute(sld, "CompanionTravel.TradeSpent");
	DeleteAttribute(sld, "CompanionTravel.TradeKind");
	if (iSettle == 0) return; // дрейфа нет / выручки нет - ничего не двигаем
	AddMoneyToCharacter(PChar, -iSettle); // продажа (iSettle<0) -> выручка ГГ; закупка-дрейф (>0 доплата / <0 возврат)
	if (sKind == "sale")
	{
		Log_Info("Компаньон передал вам выручку с торговых операций.");
		PlayStereoSound("interface\bort_jurnal.wav");
	}
	else if (iSettle > 0) Log_Info("Цены за рейс подросли - доплата за закупку компаньона.");
	else                  Log_Info("Цены за рейс упали - возврат за закупку компаньона.");
}

// текст знаковой суммы торгового рейса для СЖ: "(+12 345)" продажа / "(-12 345)" закупка / "" если по нулям
string CompanionTravel_TradeSumText(ref sld)
{
	int iSpent = 0;
	if (CheckAttribute(sld, "CompanionTravel.TradeSpent")) iSpent = sti(sld.CompanionTravel.TradeSpent);
	int iNet = -iSpent; // + продажа (выручка) / - закупка (траты)
	if (iNet == 0) return "";
	if (iNet > 0)  return " на сумму (+" + FindRussianMoneyString(iNet) + ")";
	return " на сумму (-" + FindRussianMoneyString(-iNet) + ")";
}

// ==================== СБОР ВЫРУЧКИ С ТОРГОВ ====================
// LEO: компаньон объезжает города с накопленной выручкой торгов (портман.AuctionVault>0) ЖАДНЫМ маршрутом ближайшего
//   соседа (следующий порт ищется на каждой остановке - учитывает лоты, проданные за рейс), в каждом забирает выручку
//   В СЕБЯ (AuctionMoney, не телепорт) и возвращается к ГГ. Деньги отдаёт при физической встрече (SettleTrade).
//   Скорость 1.5x (дни /450 при базе /300 + визуал WdmSpeed=1.5). Travel-механика общая (Task="auction").

// есть ли где-то готовая выручка торгов (гейт команды в диалоге)
bool CompanionTravel_HasAuctionMoney()
{
	int i;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		int iPm = GetCharacterIndex(colonies[i].id + "_PortMan"); // GetCharacterIndex не спамит в лог (в отличие от CharacterFromID) для городов без портмана
		if (iPm < 0) continue;
		ref rPm = &characters[iPm];
		if (!CheckAttribute(rPm, "AuctionVault")) continue;
		if (sti(rPm.AuctionVault) > 0) return true;
	}
	return false;
}

// дистанция город-город по мировым координатам (сдвиг/отражение _GetDistanceToColony2D её сохраняют)
float CompanionTravel_ColonyDist(string sA, string sB)
{
	int cia = FindColony(sA);
	int cib = FindColony(sB);
	if (cia < 0) return 999999.0;
	if (cib < 0) return 999999.0;
	string sIslA = colonies[cia].Island; // ключи .() требуют ПРОСТУЮ строку-переменную (как в MapBest.c), не выражение
	string sIslB = colonies[cib].Island;
	string sTownA = sA + "_town";
	string sTownB = sB + "_town";
	float ax = makefloat(worldMap.islands.(sIslA).(sTownA).position.x);
	float az = makefloat(worldMap.islands.(sIslA).(sTownA).position.z);
	float bx = makefloat(worldMap.islands.(sIslB).(sTownB).position.x);
	float bz = makefloat(worldMap.islands.(sIslB).(sTownB).position.z);
	return GetDistance2D(ax, az, bx, bz);
}

// дни перехода город-город с учётом скорости 1.5x (базовый рейс = дист/300; быстрее в 1.5 раза => дист/450)
int CompanionTravel_LegDays(string sA, string sB)
{
	int d = makeint(CompanionTravel_ColonyDist(sA, sB) / 450 + 1.0);
	if (d < 1) d = 1;
	return d;
}

// ближайший к sFrom порт с ГОТОВОЙ выручкой торгов (sFrom="" -> считаем от ГГ). sExclude пропускаем. "" если нет.
string CompanionTravel_NearestAuctionPort(string sFrom, string sExclude)
{
	int i;
	string sBest = "";
	float fBestD = 0.0;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		string sCid = colonies[i].id;
		if (sCid == sExclude) continue;
		int iPm = GetCharacterIndex(sCid + "_PortMan"); // GetCharacterIndex не спамит в лог для городов без портмана
		if (iPm < 0) continue;
		ref rPm = &characters[iPm];
		if (!CheckAttribute(rPm, "AuctionVault")) continue;
		if (sti(rPm.AuctionVault) <= 0) continue;
		float d;
		if (sFrom == "") d = makefloat(GetDistanceToColony2D(sCid));
		else             d = CompanionTravel_ColonyDist(sFrom, sCid);
		if (sBest == "") { sBest = sCid; fBestD = d; }
		else if (d < fBestD) { sBest = sCid; fBestD = d; }
	}
	return sBest;
}

// старт сбора: ближайший к ГГ порт с выручкой -> отправка. Маршрут ЖАДНЫЙ (следующий порт ищется на КАЖДОЙ остановке
//   в AuctionArrive) - это ближайший сосед + ДИНАМИКА: подбирает лоты, проданные уже во время рейса.
void CompanionTravel_StartAuctionRun(ref _NPChar, bool bReturn)
{
	if (!CompanionTravel_CheckDeparture(_NPChar)) return;
	ref sld = GetCharacter(CompanionTravel_DepartureIndex(_NPChar));
	string sFirst = CompanionTravel_NearestAuctionPort("", ""); // ближайший к ГГ порт с готовой выручкой
	if (sFirst == "") return; // нечего собирать (гейт диалога и так не пустит)
	sld.CompanionTravel.AuctionMoney = 0;
	sld.CompanionTravel.AuctionStops = 0;   // счётчик остановок - кап от бесконечного объезда (лоты могут докапливаться)
	sld.CompanionTravel.Task = "auction";
	sld.CompanionTravel.WdmSpeed = 1.5;     // визуальная скорость энкаунтера (читается в worldmap_events.c)
	if (bReturn) sld.CompanionTravel.Return = true;
	else DeleteAttribute(sld, "CompanionTravel.Return");
	sld.CompanionTravel.ToColonyID = sFirst;
	int iDays = makeint(GetDistanceToColony2D(sFirst) / 450 + 1.0); // скорость 1.5x (база /300)
	if (iDays < 1) iDays = 1;
	sld.CompanionTravel.Days = iDays;
	CompanionTravel_SetTraveller(_NPChar);
}

// прибытие в очередной порт торгов: забираем выручку В СЕБЯ, дальше ЖАДНО к ближайшему следующему порту с выручкой
//   (динамика - подберёт лоты, проданные уже во время рейса) ИЛИ домой/ждать. Зов из обеих веток прибытия.
void CompanionTravel_AuctionArrive(string sCompanion, string sID)
{
	ref sld = CharacterFromID(sID);
	string sColony = sld.CompanionTravel.ToColonyID;
	ref rPm = CharacterFromID(sColony + "_PortMan"); // забрать выручку текущего порта В СЕБЯ (не телепорт ГГ)
	if (CheckAttribute(rPm, "AuctionVault"))
	{
		int v = sti(rPm.AuctionVault);
		if (v > 0)
		{
			int cur = 0;
			if (CheckAttribute(sld, "CompanionTravel.AuctionMoney")) cur = sti(sld.CompanionTravel.AuctionMoney);
			sld.CompanionTravel.AuctionMoney = cur + v;
			rPm.AuctionVault = 0;
		}
	}
	int stops = 1;
	if (CheckAttribute(sld, "CompanionTravel.AuctionStops")) stops = sti(sld.CompanionTravel.AuctionStops) + 1;
	sld.CompanionTravel.AuctionStops = stops;
	AddQuestRecord("CompanionTravel", "3");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
	// следующий порт с выручкой (ДИНАМИЧЕСКИ: учтёт лоты, проданные за рейс). Кап stops<30 - защита от бесконечного объезда.
	string sNext = "";
	if (stops < 30) sNext = CompanionTravel_NearestAuctionPort(sColony, sColony);
	if (sNext != "")
	{
		int legDays = CompanionTravel_LegDays(sColony, sNext);
		sld.CompanionTravel.ToColonyID = sNext;
		if (sCompanion != "") PChar.CompanionTravel.(sCompanion).Days = legDays;
		AddQuestUserData("CompanionTravel", "sReason", "забрал"+ NPCharSexPhrase(sld,"","а") +" выручку с торгов в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и идёт дальше");
		sld.CompanionTravel.FromPort = sColony; // старт следующей ноги ИЗ этого порта (не у ГГ)
		CompanionTravel_LaunchOutbound(sld, legDays);
		return;
	}
	// маршрут пройден - сумма собранного в запись СЖ
	int iGot = 0;
	if (CheckAttribute(sld, "CompanionTravel.AuctionMoney")) iGot = sti(sld.CompanionTravel.AuctionMoney);
	string sAuSum = "";
	if (iGot > 0) sAuSum = " на сумму " + FindRussianMoneyString(iGot);
	if (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true)
	{
		AddQuestUserData("CompanionTravel", "sReason", "собрал"+ NPCharSexPhrase(sld,"","а") +" выручку за проданные корабли" + sAuSum + " и идёт обратно к нам");
		sld.CompanionTravel.Phase = "back";
		CompanionTravel_SpawnMeetEncounter(sCompanion, sID, sColony);
		return;
	}
	AddQuestUserData("CompanionTravel", "sReason", "собрал"+ NPCharSexPhrase(sld,"","а") +" выручку за проданные корабли" + sAuSum + " и ждёт нас в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")));
	if (sCompanion != "") PChar.CompanionTravel.(sCompanion).Days = 0;
	CompanionTravel_SetCompanionToColony(sColony, "CompanionTravel_" + sCompanion, sID);
}

// ==================== СНАРЯЖЕНИЕ ОФИЦЕРА КОРАБЛЁМ ====================
// LEO: обратный рейс - безлошадному офицеру-файтеру команда "снаряди себе корабль по классу". Идёт в БЛИЖАЙШИЙ город
//   с верфью, покупает корпус+пушки (из воздуха - верфи регенерят, недостаток бюджета покрываем), нанимает РЕАЛЬНУЮ
//   команду из пула таверны и закупает РЕАЛЬНЫЙ груз в лавке (нет в наличии - НЕ берём из воздуха), потом живым
//   энкаунтером догоняет эскадру и входит компаньоном-капитаном (ReturnToSquadron->SetCompanionIndex). Бюджет списываем
//   СРАЗУ (антиабуз), остаток отдаёт при встрече (SettleTrade, без телепорта). Опыт Commerce - офицеру за оборот.
//   Дорога ДО города = абстрактный ТАЙМЕР (корабля нет - нет энкаунтера туда), маркер OutfitTravel. Task="outfit".

// LEO: есть ли в колонии верфь (<город>_shipyarder) и магазин. Панама/безнациональные - нет.
bool CompanionTravel_IsShipyardCity(string sColonyId)
{
	if (sColonyId == "Panama") return false;
	if (GetCharacterIndex(sColonyId + "_shipyarder") < 0) return false;
	if (FindStore(sColonyId) < 0) return false;
	return true;
}

// LEO: город с верфью на ТЕКУЩЕМ регионе/острове (снаряжение идёт там, где отдан приказ - офицер не поплывёт вплавь).
//   Текущую колонию берём по точке возврата loadedLocation.fastreload (движок держит её и в порту, и в джунглях того
//   же острова). Ищем верфь на том же ВОРЛДМАП-острове (colonies[].Island): на островах = сам остров, на МЕЙНЕ каждый
//   город - свой ворлдмап-остров (у локаций islandId="Mein" общий, поэтому по islandId нельзя). "" если суши/верфи нет.
string CompanionTravel_CurrentIslandShipyard()
{
	if (IsEntity(worldMap)) return "";                  // не на суше (глобалка)
	if (!CheckAttribute(&loadedLocation, "fastreload")) return "";
	string sCurCol = loadedLocation.fastreload;
	if (sCurCol == "") return "";
	int iCur = FindColony(sCurCol);
	if (iCur < 0) return "";
	string sWmIsl = colonies[iCur].Island;              // ворлдмап-остров текущей колонии (на мейне = сам город)
	if (sWmIsl == "") return "";
	int i;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		if (colonies[i].nation == "none") continue;
		string sColIsl = colonies[i].Island;
		if (sColIsl != sWmIsl) continue;                // не наш ворлдмап-остров
		string sCid = colonies[i].id;
		if (!CompanionTravel_IsShipyardCity(sCid)) continue;
		return sCid;
	}
	return "";
}

// LEO: можно ли ЗДЕСЬ (на текущем острове) снарядить офицера - есть ли верфь. Гейт команды в диалоге.
bool CompanionTravel_CanOutfitHere()
{
	string s = CompanionTravel_CurrentIslandShipyard();
	if (s == "") return false;
	return true;
}

// LEO: минимальный ранг офицера под класс корабля (антиабуз - не заказать класс сильно выше положенного).
int CompanionTravel_ClassMinRank(int shipClass)
{
	if (shipClass <= 1) return 33;
	if (shipClass == 2) return 26;
	if (shipClass == 3) return 20;
	if (shipClass == 4) return 12;
	if (shipClass == 5) return 6;
	return 1; // класс 6 (и мельче)
}

// LEO: множитель объёма закупаемого груза: ЛИНЕЙНО - база класс 6 (=1), +1 за каждый класс выше (кл5=2, кл4=3, кл3=4, кл2=5).
int CompanionTravel_OutfitGoodsMul(int shipClass)
{
	int iMul = 7 - shipClass;
	if (iMul < 1) iMul = 1;
	return iMul;
}

// LEO: ДЕТЕРМИНИРОВАННЫЙ тип-представитель класса для СМЕТЫ (первый военный тип класса по порядку каталога). ⚠️ НЕ
//   GetShipTypeExt - тот возвращает СЛУЧАЙНЫЙ тип каждый вызов (rand внутри, даже из кэша), из-за чего смета скакала
//   при каждом открытии диалога. Фактический корабль генерим случайным (разнообразие), а смета - стабильная. -1 если нет.
int CompanionTravel_ClassRepType(int shipClass, string sType)
{
	int i;
	for (i = 0; i <= SHIP_TYPES_QUANTITY; i++)
	{
		ref rShip = GetShipByType(i);
		if (!CheckAttribute(rShip, "class")) continue;
		if (MakeInt(rShip.Class) != shipClass) continue;
		if (CheckAttribute(rShip, "QuestShip")) continue;
		if (sti(rShip.CanEncounter) != true) continue;
		if (sType != "" && sti(rShip.Type.(sType)) != true) continue; // war/merchant; "" - любой
		return i;
	}
	return -1; // нет корабля такого типа+класса -> класс не предложим для этого типа
}

// LEO: цена корпуса+пушек по ТТХ КАТАЛОЖНОГО типа (без генерации инстанса). ⚠️ Штатный GetShipPriceByTTH(iType,..) берёт
//   iType как индекс RealShips (ЖИВОЙ корабль) - для каталожного индекса он оценил бы чужой живой корабль (были дикие
//   ~15млн, одинаковые для всех классов). Считаем те же ТТХ-компоненты напрямую по каталогу (GetShipByType). rCat без
//   Ship.Cannons.Type -> пушки по универсальной ставке 1500/ствол (как для NullCharacter).
int CompanionTravel_ShipPriceByType(int catalogType)
{
	ref rCat = GetShipByType(catalogType);
	int cl = sti(rCat.class);
	int iSumm = GetShipPriceByTTH_HP(cl, rCat);
	iSumm += GetShipPriceByTTH_Capacity(cl, rCat);
	iSumm += GetShipPriceByTTH_MaxCrew(cl, rCat);
	iSumm += GetShipPriceByTTH_MaxCaliber(cl, rCat);
	iSumm += GetShipPriceByTTH_CannonsQuantity(cl, rCat, rCat);
	iSumm += GetShipPriceByTTH_WindAgainstSpeed(cl, rCat);
	iSumm += GetShipPriceByTTH_MastMultiplier(cl, rCat);
	iSumm += GetShipPriceByTTH_SpeedRate(cl, rCat);
	iSumm += GetShipPriceByTTH_TurnRate(cl, rCat);
	return iSumm;
}

// LEO: авто-смета бюджета на снаряжение класса: корпус+пушки + ~команда + груз, с запасом 15%.
//   Оценка щедрая - лишнее офицер вернёт при встрече. 0 если класс не собрать.
int CompanionTravel_EstimateOutfitBudget(int shipClass, string sType)
{
	int iType = CompanionTravel_ClassRepType(shipClass, sType);
	if (iType < 0) return 0;
	int iShip = CompanionTravel_ShipPriceByType(iType); // корпус+пушки по ТТХ каталога
	int iCrew = makeint(iShip * 0.15);             // грубо команда (растёт с кораблём)
	int iGoods = 25000 * CompanionTravel_OutfitGoodsMul(shipClass); // боевой набор (база кл6, линейно +1×база за класс выше)
	int iTotal = iShip + iCrew + iGoods;
	iTotal = makeint(iTotal * 1.15);               // запас 15%
	return iTotal;
}

// LEO: занять слот рейса под безлошадного офицера (аналог SetTraveller, но без корабля/энкаунтера и без
//   RemoveCharacterCompanion - он пассажир, не компаньон). Журнал #TEXT 4 (без sShipInfo - корабля ещё нет).
void CompanionTravel_OccupyOutfitSlot(ref sld, int iDays)
{
	if (!CompanionTravel_CheckDeparture(sld)) return;
	string attr;
	int j;
	if (!CheckAttribute(PChar, "CompanionTravel")) PChar.CompanionTravel = 0;
	PChar.CompanionTravel = sti(PChar.CompanionTravel) + 1;
	for (j = 1; j <= 3; j++)
	{
		if (Group_IsDead("CompanionTravel_Companion" + j)) attr = "Companion" + j;
	}
	sld.CompanionTravel.GroupID = "CompanionTravel_" + attr;
	PChar.CompanionTravel.(attr).ID = sld.ID;
	PChar.CompanionTravel.(attr).Days = iDays;
	PChar.CompanionTravel.(attr).ToColonyID = sld.CompanionTravel.ToColonyID;
	string attrLoc = "CompanionTravel_" + attr;
	Group_FindOrCreateGroup(attrLoc);
	Group_SetType(attrLoc, "pirate");
	Group_AddCharacter(attrLoc, PChar.CompanionTravel.(attr).ID);
	Group_SetGroupCommander(attrLoc, PChar.CompanionTravel.(attr).ID);
	Group_SetTaskRunaway(attrLoc, PLAYER_GROUP);
	Group_LockTask(attrLoc);
	ReOpenQuestHeader("CompanionTravel");
	AddQuestRecord("CompanionTravel", "4");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sReason", "отправлен"+ NPCharSexPhrase(sld,"","а") +" в " + XI_ConvertString(CompanionTravel_ColonyTextId(sld.CompanionTravel.ToColonyID, "Acc")) + " снарядить себе корабль и догнать нас (в пути " + FindRussianDaysString(iDays) + ")");
	// LEO: офицер вернётся КОМПАНЬОНОМ-капитаном - сохраняем ЧИСТЫЙ вход компаньона ("hired"), а НЕ текущую ноду
	//   офицера: она может быть служебной (напр. CompanionTravel_LastNode из Go-ноды) и после возврата дать битый
	//   текст ("встречу в ." с пустым городом - CompanionTravel уже стёрт при вступлении в эскадру).
	sld.CompanionTravel.SavedDlgFile = "Enc_Officer_dialog.c";
	sld.CompanionTravel.SavedNode    = "hired";
	sld.CompanionTravel.SavedDeck    = "hired";
	sld.dialog.filename = "Enc_Officer_dialog.c";
	sld.dialog.currentnode = "CompanionTravel_OnMission";
	sld.dialog.TempNode = "hired";
	sld.DeckDialogNode = "CompanionTravel_OnMission";
}

// LEO: команда офицеру "снаряди себе корабль". Списываем бюджет сразу, выводим из пассажиров, ставим на слот-таймер.
void CompanionTravel_StartOutfit(ref off, int shipClass, int budget, string sType)
{
	if (!CompanionTravel_CheckDeparture(off)) return;
	string sCity = CompanionTravel_CurrentIslandShipyard();
	if (sCity == "")
	{
		Log_Info("Здесь нет верфи - снаряжение невозможно.");
		return;
	}
	AddMoneyToCharacter(PChar, -budget);       // бюджет с запасом списываем сразу (остаток вернёт при встрече)
	RemovePassenger(PChar, off);               // офицер уходит снаряжаться (вернётся компаньоном-капитаном)
	off.location = "None"; off.location.group = ""; off.location.locator = ""; // ушёл к верфи - модель уберётся с причала на релоаде
	off.CompanionTravel.Task = "outfit";
	off.CompanionTravel.OutfitClass = shipClass;
	off.CompanionTravel.OutfitType = sType;    // war / merchant / "" (универсал) - какой корабль подбирать
	off.CompanionTravel.OutfitBudget = budget;
	off.CompanionTravel.ToColonyID = sCity;    // верфь на ЭТОМ же регионе/острове (снаряжается здесь)
	off.CompanionTravel.OutfitTravel = true;   // маркер шиплесс-таймера (нет корабля - нет энкаунтера туда)
	int iDays = 1 + dRand(2);                  // снаряжение 1-3 дня (dRand - не сейв-скамится)
	off.CompanionTravel.Days = iDays;
	CompanionTravel_OccupyOutfitSlot(off, iDays);
}

// LEO: диалоговый диспетчер - смета класса, проверка денег, отправка. Зовётся из нод OfficerOutfit_Go<N>.
void CompanionTravel_DispatchOutfit(ref off, int shipClass, string sType)
{
	if (!CompanionTravel_CheckDeparture(off)) return;
	DeleteAttribute(PChar, "questTemp.OutfitType"); // транзиентный temp из диалога (тип уже в sType) - чистим на всех путях
	if (sti(off.rank) < CompanionTravel_ClassMinRank(shipClass)) // антиабуз: не заказать класс сильно выше ранга офицера
	{
		Log_Info("Этот офицер ещё не дорос до корабля такого класса.");
		return;
	}
	int budget = CompanionTravel_EstimateOutfitBudget(shipClass, sType);
	if (budget <= 0) return;
	if (sti(PChar.money) < budget)
	{
		Log_Info("Не хватает золота на снаряжение этого класса.");
		return;
	}
	CompanionTravel_StartOutfit(off, shipClass, budget, sType);
}

// LEO: закупить в лавке до target единиц товара g кораблю, не дороже cap золотых. Возвращает потраченное. Только из
//   РЕАЛЬНОГО наличия стора (нет - 0, из воздуха не берём); трюм режет AddCharacterGoodsSimple.
int CompanionTravel_OutfitBuyGood(ref sld, int iStore, int g, int target, int cap)
{
	if (target <= 0) return 0;
	if (cap <= 0) return 0;
	int avail = sti(GetStoreGoodsQuantity(&stores[iStore], g));
	if (avail <= 0) return 0;
	int price1 = GetStoreGoodsPrice(&stores[iStore], g, PRICE_TYPE_BUY, PChar, 1); // за блок Units
	if (price1 < 1) return 0;
	int unit = sti(Goods[g].Units);
	if (unit < 1) unit = 1;
	int q = target;
	if (q > avail) q = avail;
	int qByMoney = makeint(makefloat(cap) * unit / price1); // сколько влезет в остаток бюджета
	if (q > qByMoney) q = qByMoney;
	if (q <= 0) return 0;
	int got = AddCharacterGoodsSimple(sld, g, q); // трюм-кап, вернёт реально взятое
	if (got <= 0) return 0;
	RemoveStoreGoods(&stores[iStore], g, got);
	return makeint(makefloat(price1) * got / unit + 0.5);
}

// LEO: офицер дошёл до города - снаряжается: корпус+пушки, реальная команда, реальный груз; считает остаток бюджета
//   и живым энкаунтером идёт догонять эскадру. Остаток отдаст при встрече (SettleTrade, атрибут OutfitChange).
void CompanionTravel_DoOutfitWork(ref sld, string sColony)
{
	int shipClass = 5;
	if (CheckAttribute(sld, "CompanionTravel.OutfitClass")) shipClass = sti(sld.CompanionTravel.OutfitClass);
	string sType = "war";
	if (CheckAttribute(sld, "CompanionTravel.OutfitType")) sType = sld.CompanionTravel.OutfitType; // war/merchant/"" (универсал)
	int budget = 0;
	if (CheckAttribute(sld, "CompanionTravel.OutfitBudget")) budget = sti(sld.CompanionTravel.OutfitBudget);
	int remaining = budget;
	int iCityCol = FindColony(sColony);
	int nation = -1;
	if (iCityCol >= 0 && CheckAttribute(&colonies[iCityCol], "nation")) nation = sti(colonies[iCityCol].nation);

	// --- КОРПУС + ПУШКИ (из воздуха; недостаток бюджета покрываем - верфи регенерят) ---
	int iBaseType = GetShipTypeExt(shipClass, shipClass, sType, nation);
	if (iBaseType < 0) iBaseType = CompanionTravel_ClassRepType(shipClass, sType);
	if (iBaseType < 0) iBaseType = CompanionTravel_ClassRepType(shipClass, ""); // крайний фолбэк: любой тип класса (корабль будет всегда)
	int iShipCost = 0;
	if (iBaseType >= 0)
	{
		iShipCost = CompanionTravel_ShipPriceByType(iBaseType); // корпус+пушки по ТТХ каталога (см. гочу в ShipPriceByType)
		sld.DontRefresh = true;                                   // не набивать команду/боезапас из воздуха - добьём реальными
		sld.Ship.Type = GenerateShipExt(iBaseType, false, sld);  // false = честный (не ворованный, можно продать)
		SetRandomNameToShip(sld);
		SetBaseShipData(sld);                                     // HP/SP/апгрейды (команду/груз пропустит по DontRefresh)
		int iCal = sti(RealShips[sti(sld.Ship.Type)].MaxCaliber);
		sld.Ship.Cannons.Type = GetCannonByTypeAndCaliber("cannon", iCal); // калибр под корпус
		sld.Ship.Cannons.Charge.Type = GOOD_BALLS;
		if (!CheckAttribute(sld, "Ship.Crew.Quantity")) sld.Ship.Crew.Quantity = 0; // старт с нуля - наберём реально
		DeleteAttribute(sld, "DontRefresh");
	}
	if (iShipCost > remaining) remaining = 0;                    // недостаток - из воздуха (ГГ теряет не больше бюджета)
	else remaining -= iShipCost;

	// --- КОМАНДА: до максимума, РЕАЛЬНО из пула таверны, в пределах остатка ---
	ref rCity = GetColonyByIndex(iCityCol);
	int iHaveCrew = GetCrewQuantity(sld);
	int iTarget = GetMaxCrewQuantity(sld);
	int iPool = 0;
	if (CheckAttribute(rCity, "Ship.Crew.Quantity")) iPool = GetCrewQuantity(rCity);
	int iPrice1 = GetCrewPriceForTavern(sColony);
	if (iPrice1 < 1) iPrice1 = 1;
	int iN = iTarget - iHaveCrew;
	if (iN > iPool) iN = iPool;
	int iByMoney = remaining / iPrice1;
	if (iN > iByMoney) iN = iByMoney;
	if (iN < 0) iN = 0;
	if (iN > 0)
	{
		float fMor = 40.0;
		if (CheckAttribute(rCity, "Ship.Crew.Morale")) fMor = stf(rCity.Ship.Crew.Morale);
		float fES = 40.0;
		if (CheckAttribute(rCity, "Ship.Crew.Exp.Sailors")) fES = stf(rCity.Ship.Crew.Exp.Sailors);
		float fEC = 40.0;
		if (CheckAttribute(rCity, "Ship.Crew.Exp.Cannoners")) fEC = stf(rCity.Ship.Crew.Exp.Cannoners);
		float fESo = 40.0;
		if (CheckAttribute(rCity, "Ship.Crew.Exp.Soldiers")) fESo = stf(rCity.Ship.Crew.Exp.Soldiers);
		sld.Ship.Crew.Exp.Sailors   = fES;
		sld.Ship.Crew.Exp.Cannoners = fEC;
		sld.Ship.Crew.Exp.Soldiers  = fESo;
		sld.Ship.Crew.Morale        = fMor;
		SetCrewQuantity(sld, iHaveCrew + iN);
		rCity.Ship.Crew.Quantity = iPool - iN;
		remaining -= iN * iPrice1;
	}
	// подстраховка: даже с пустой таверной у корабля должны быть валидные опыт/мораль (иначе битый корабль)
	if (!CheckAttribute(sld, "Ship.Crew.Exp.Sailors"))
	{
		sld.Ship.Crew.Exp.Sailors = 30;
		sld.Ship.Crew.Exp.Cannoners = 30;
		sld.Ship.Crew.Exp.Soldiers = 30;
	}
	if (!CheckAttribute(sld, "Ship.Crew.Morale")) sld.Ship.Crew.Morale = 30;

	// --- ГРУЗ: стандартный боевой набор, РЕАЛЬНО из лавки (нет - пропускаем), в пределах остатка ---
	int iStore = FindStore(sColony);
	if (iStore >= 0)
	{
		int iMul = CompanionTravel_OutfitGoodsMul(shipClass); // база кл6, линейно +1×база за каждый класс выше
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_POWDER,     500 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_BALLS,      100 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_GRAPES,     200 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_KNIPPELS,   200 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_BOMBS,      100 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_FOOD,       250 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_RUM,        200 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_PLANKS,     100 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_SAILCLOTH,   100 * iMul, remaining);
		remaining -= CompanionTravel_OutfitBuyGood(sld, iStore, GOOD_MEDICAMENT,  200 * iMul, remaining);
	}

	// --- РАСЧЁТ + СНАРЯЖЁН ---
	int iSpent = budget - remaining;
	if (iSpent < 0) iSpent = 0;
	if (iSpent > 0) AddCharacterExpToSkill(sld, "Commerce", iSpent / 2600.0); // опыт офицеру за оборот
	sld.CompanionTravel.OutfitChange = remaining;   // остаток отдаст при встрече (SettleTrade)
	DeleteAttribute(sld, "CompanionTravel.OutfitTravel");
	DeleteAttribute(sld, "CompanionTravel.OutfitClass");
	DeleteAttribute(sld, "CompanionTravel.OutfitType");
	DeleteAttribute(sld, "CompanionTravel.OutfitBudget");
	CompanionTravel_SetSpecialShipAttributes(sld); // теперь есть корабль - боевые спец-флаги рейса (мирный энкаунтер)

	string sCompanion = CompanionTravel_FindSlotKey(sld.id);
	AddQuestRecord("CompanionTravel", "3");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
	string sGotSum = "";
	if (remaining > 0) sGotSum = " (сдача " + FindRussianMoneyString(remaining) + " при встрече)";
	AddQuestUserData("CompanionTravel", "sReason", "снарядил"+ NPCharSexPhrase(sld,"ся","ась") +" в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и идёт догонять нас" + sGotSum);
	sld.CompanionTravel.Phase = "back";
	CompanionTravel_SpawnMeetEncounter(sCompanion, sld.id, sColony);
}

// LEO: вместимость склада города - та же логика, что в store.c/storage.c (крупные порты и улучшение - 100k)
int CompanionTravel_StorageMax(string sColonyId)
{
	if (CheckAttribute(PChar, "ColonyBuilding.StoreHouse.Resize") && PChar.ColonyBuilding.StoreHouse.Resize == true) return 100000;
	if (sColonyId == "Tortuga")   return 100000;
	if (sColonyId == "Villemstad") return 100000;
	if (sColonyId == "PortRoyal") return 100000;
	if (sColonyId == "Havana")    return 100000;
	return 50000;
}

// LEO: компаньон дошёл до города со складом - выполняем то, ради чего шёл.
//   Deliver: везённое сгружаем на склад (сколько влезет по весу - остальное остаётся в трюме).
//   Pickup:  зарезервированное грузим в трюм (оно было снято со склада ещё при отправке;
//            что не влезло в трюм - возвращаем на склад, чтобы не пропало).
void CompanionTravel_DoStorageWork(ref rCompanion, string sColony)
{
	CompanionTravel_SettleStorageRent(sColony); // LEO: заодно гасим накопившуюся аренду склада (из своих, долгом на игрока)
	int iStore = CompanionTravel_GetStoreNum(sColony);
	if (iStore < 0) return;
	int i, q, iGot, iFreeW, iW;
	string sKey;

	if (CheckAttribute(rCompanion, "CompanionTravel.Deliver"))
	{
		for (i = 0; i < GOODS_QUANTITY; i++)
		{
			sKey = "" + i;
			if (!CheckAttribute(rCompanion, "CompanionTravel.Deliver." + sKey)) continue;
			q = sti(rCompanion.CompanionTravel.Deliver.(sKey));
			if (q > GetCargoGoods(rCompanion, i)) q = GetCargoGoods(rCompanion, i); // часть могли потерять в пути
			if (q <= 0) continue;
			iFreeW = CompanionTravel_StorageMax(sColony) - GetStorageUsedWeight(&stores[iStore]);
			iW = sti(Goods[i].Weight);
			if (iW < 1) iW = 1;
			if (q * iW / sti(Goods[i].Units) > iFreeW) q = makeint(iFreeW * sti(Goods[i].Units) / iW); // склад забит
			if (q <= 0) continue;
			SetStorageGoods(&stores[iStore], i, sti(GetStorageGoodsQuantity(&stores[iStore], i)) + q);
			RemoveCharacterGoodsSelf(rCompanion, i, q);
		}
		DeleteAttribute(rCompanion, "CompanionTravel.Deliver");
	}

	if (CheckAttribute(rCompanion, "CompanionTravel.Pickup"))
	{
		for (i = 0; i < GOODS_QUANTITY; i++)
		{
			sKey = "" + i;
			if (!CheckAttribute(rCompanion, "CompanionTravel.Pickup." + sKey)) continue;
			q = sti(rCompanion.CompanionTravel.Pickup.(sKey));
			if (q <= 0) continue;
			iGot = AddCharacterGoodsSimple(rCompanion, i, q);
			if (iGot < q) SetStorageGoods(&stores[iStore], i, sti(GetStorageGoodsQuantity(&stores[iStore], i)) + (q - iGot));
		}
		DeleteAttribute(rCompanion, "CompanionTravel.Pickup");
	}
}

// LEO: рейс сорвался (компаньон не дошёл) - зарезервированное на складе никуда не уезжало,
//   возвращаем его в оборот, иначе товар просто исчезнет из мира.
void CompanionTravel_ReleaseReserve(ref rCompanion, string sColony)
{
	if (!CheckAttribute(rCompanion, "CompanionTravel.Pickup")) return;
	int iStore = CompanionTravel_GetStoreNum(sColony);
	int i, q;
	string sKey;
	if (iStore >= 0)
	{
		for (i = 0; i < GOODS_QUANTITY; i++)
		{
			sKey = "" + i;
			if (!CheckAttribute(rCompanion, "CompanionTravel.Pickup." + sKey)) continue;
			q = sti(rCompanion.CompanionTravel.Pickup.(sKey));
			if (q <= 0) continue;
			SetStorageGoods(&stores[iStore], i, sti(GetStorageGoodsQuantity(&stores[iStore], i)) + q);
		}
	}
	DeleteAttribute(rCompanion, "CompanionTravel.Pickup");
}

// LEO: store-объект города (для чтения/записи товаров склада), -1 если города нет
int CompanionTravel_GetStoreNum(string sColonyId)
{
	return FindStore(sColonyId);
}

// LEO: аренда склада капает помесячно на лавочнике <colony>_trader (Storage.MoneyForStorage/мес, Storage.Date;
//   как в Common_Store.c). Компаньон гасит её из своих (сброс срока), сумму вешаем долгом на игрока -> при встрече.
void CompanionTravel_SettleStorageRent(string sColony)
{
	int iTrader = GetCharacterIndex(sColony + "_trader");
	if (iTrader < 0) return;
	ref rTrader = &characters[iTrader];
	if (!CheckAttribute(rTrader, "Storage.Activate")) return;
	if (!CheckAttribute(rTrader, "Storage.MoneyForStorage")) return;
	int iMonths = GetNpcQuestPastMonthParam(rTrader, "Storage.Date");
	if (iMonths <= 0) return; // срок ещё не подошёл
	int iDue = iMonths * sti(rTrader.Storage.MoneyForStorage);
	if (iDue <= 0) return;
	SaveCurrentNpcQuestDateParam(rTrader, "Storage.Date"); // компаньон внёс плату - срок пошёл заново
	if (!CheckAttribute(PChar, "CompanionTravel.StorageDebt")) PChar.CompanionTravel.StorageDebt = 0;
	PChar.CompanionTravel.StorageDebt = sti(PChar.CompanionTravel.StorageDebt) + iDue;
	// сумму НЕ списываем сейчас (компаньон в отлучке) - копим долг и вернём при встрече/добором по суткам.
	Log_Info("Компаньон оплатил из своих аренду нашего склада в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " - " + FindRussianMoneyString(iDue) + ". Вернём при встрече.");
}

// LEO: возврат долга за аренду склада, которую компаньон оплатил из своих. Деньги с ГГ снимаем ТОЛЬКО ПРИ
//   ВСТРЕЧЕ (bAtMeeting=true: ReturnToSquadron / диалог hired в порту). Суточный тик (bAtMeeting=false) трогает
//   долг ЛИШЬ как ОСТАТОК - т.е. только после того, как встреча уже была, но денег не хватило (флаг StorageDebtDue).
//   Так до первой встречи с компаньоном ничего не списывается.
void CompanionTravel_TryPayStorageDebt(bool bAtMeeting)
{
	if (!CheckAttribute(PChar, "CompanionTravel.StorageDebt")) return;
	int iDebt = sti(PChar.CompanionTravel.StorageDebt);
	if (iDebt <= 0)
	{
		DeleteAttribute(PChar, "CompanionTravel.StorageDebt");
		DeleteAttribute(PChar, "CompanionTravel.StorageDebtDue");
		return;
	}
	// суточный добор - только по остатку уже предъявленного при встрече долга (иначе ждём встречи)
	if (!bAtMeeting && !CheckAttribute(PChar, "CompanionTravel.StorageDebtDue")) return;
	int iMoney = sti(PChar.money);
	if (iMoney <= 0) return; // нечем платить - подождём следующих суток
	int iPay = iDebt;
	if (iPay > iMoney) iPay = iMoney;
	AddMoneyToCharacter(PChar, -iPay);
	int iRest = iDebt - iPay;
	if (iRest > 0)
	{
		PChar.CompanionTravel.StorageDebt = iRest;
		PChar.CompanionTravel.StorageDebtDue = true; // встреча была, остаток добираем по суткам
		// сумму вычета НЕ пишем - её уже показывает отдельная цветная строка "-X золотых". Пишем только остаток.
		Log_Info("С вас удержано за аренду склада, оплаченную компаньоном. Остаток " + FindRussianMoneyString(iRest) + " спишем, как появятся деньги.");
	}
	else
	{
		DeleteAttribute(PChar, "CompanionTravel.StorageDebt");
		DeleteAttribute(PChar, "CompanionTravel.StorageDebtDue");
		Log_Info("С вас удержано за аренду склада, оплаченную компаньоном."); // сумму показывает цветная строка вычета
	}
}

// ==================== КОММЕРЧЕСКИЙ РЕЙС ====================
// LEO: компаньон везёт СВОЙ корабль в город и сбывает (CommerceOp): "sell" - верфь ~10% (как ворованный),
//   "auction" - лот на торги ПУ (30-50% по суткам, выручку за КОРПУС ГГ берёт сам у портмана), "store" - стоянка
//   в ПУ (сам держателем / со сторожем, плата при снятии). Пушки+товары -> лавка города (выручка КОМПАНЬОНУ,
//   копится в Proceeds), команда -> гарнизон. Безлошадный ждёт ГГ в таверне с "!"; при встрече - деньги + офицером
//   в каюту (store-keep уходит держателем в ПУ, не в таверну). Опыт Commerce компаньону. Гочи - [[companion-travel-live-feature]].

// LEO: увести команду ликвидируемого корабля в гарнизон города назначения (взвешенно по опыту/морали), как
//   SendCrewToTavern на верфи, но по УДАЛЁННОЙ колонии - рейс идёт вне текущей загруженной локации.
void CompanionTravel_CrewToColony(ref chrefFrom, ref rCol, int crewn)
{
	if (crewn <= 0) return;
	if (!CheckAttribute(rCol, "index")) return;
	float fDen = makefloat(GetCrewQuantity(rCol) + crewn);
	if (fDen > 0.0)
	{
		rCol.Ship.Crew.Exp.Sailors   = (stf(rCol.Ship.Crew.Exp.Sailors)  *GetCrewQuantity(rCol) + stf(chrefFrom.Ship.Crew.Exp.Sailors)  *crewn) / fDen;
		rCol.Ship.Crew.Exp.Cannoners = (stf(rCol.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(rCol) + stf(chrefFrom.Ship.Crew.Exp.Cannoners)*crewn) / fDen;
		rCol.Ship.Crew.Exp.Soldiers  = (stf(rCol.Ship.Crew.Exp.Soldiers) *GetCrewQuantity(rCol) + stf(chrefFrom.Ship.Crew.Exp.Soldiers) *crewn) / fDen;
		rCol.Ship.Crew.Morale        = (stf(rCol.Ship.Crew.Morale)       *GetCrewQuantity(rCol) + stf(chrefFrom.Ship.Crew.Morale)       *crewn) / fDen;
	}
	rCol.Ship.Crew.Quantity = GetCrewQuantity(rCol) + crewn;
}

// LEO: ликвидация корабля компаньона в городе назначения. Пушки+товары -> лавка города (выручка КОМПАНЬОНУ, цена
//   по ЕГО навыку Commerce), команда -> гарнизон. Корпус: bAuction -> штатный лот на торги ПУ (корпус ГГ забирает
//   у портмана сам); иначе (верфь) -> ~10% честной цены (как ворованный), тоже компаньону. Всё в Proceeds.
void CompanionTravel_LiquidateShip(ref sld, string sColony, bool bAuction)
{
	int iProceeds = 0;
	ref rCol = GetColonyByIndex(FindColony(sColony));
	bool bStore = CheckAttribute(rCol, "StoreNum");
	ref trader;
	if (bStore) trader = &stores[sti(rCol.StoreNum)];
	ref rPortman = CharacterFromID(sColony + "_PortMan");

	// --- ПУШКИ: сняты и проданы в лавку пушечным товаром ---
	int iCannons = GetCannonsNum(sld);
	if (iCannons > 0 && bStore)
	{
		int cidx = GetCannonGoodsIdxByType(sti(sld.Ship.Cannons.Type));
		if (cidx != -1)
		{
			int cUnits    = Goods[cidx].Units;
			int cStoreQty = GetStoreGoodsQuantity(trader, cidx);
			int cPrice    = GetStoreGoodsPrice(trader, cidx, PRICE_TYPE_SELL, sld, 1);
			SetStoreGoods(trader, cidx, cStoreQty + iCannons);
			iProceeds += makeint(cPrice * iCannons / cUnits + 0.5);
		}
	}

	// --- ТОВАРЫ: всё в лавку города (компаньон сбывает и контрабанду - оптом) ---
	if (bStore)
	{
		int iGoodIndex;
		for (iGoodIndex = 0; iGoodIndex < GOODS_QUANTITY; iGoodIndex++)
		{
			int ShipQ = GetCargoGoods(sld, iGoodIndex);
			if (ShipQ <= 0) continue;
			int iUnits      = Goods[iGoodIndex].Units;
			int iStoreQty   = GetStoreGoodsQuantity(trader, iGoodIndex);
			int iStorePrice = GetStoreGoodsPrice(trader, iGoodIndex, PRICE_TYPE_SELL, sld, 1);
			RemoveCharacterGoods(sld, iGoodIndex, ShipQ);
			SetStoreGoods(trader, iGoodIndex, iStoreQty + ShipQ);
			iProceeds += makeint(iStorePrice * ShipQ / iUnits + 0.5);
		}
	}

	// --- КОМАНДА: уволена в гарнизон города ---
	Wounded_ClearToInfirmary(sld); // раненых - в лазарет (штатно, как на верфи/аукционе)
	int crew = GetCrewQuantity(sld);
	if (crew > 0) CompanionTravel_CrewToColony(sld, rCol, crew);
	SetCrewQuantity(sld, 0);

	// --- КОРПУС ---
	if (bAuction)
	{
		// штатный лот на торги ПУ города назначения (продаётся по суткам PortAuction_Daily -> AuctionVault портмана)
		int iBase = PortAuction_GetShipValue(sld, rPortman);
		int iShipClass = GetCharacterShipClass(sld);
		ref keeper = GetCharacter(NPC_GenerateCharacter("ShipAtAuction", "PKM_rab_"+(rand(3)+1), "man", "man", rand(5)+1, rPortman.nation, -1, false));
		keeper.id = keeper.id + "_" + keeper.index; // уникальный ID (слот переиспользуется после продажи)
		keeper.lastname = keeper.name;
		if (LanguageGetLanguage() == "russian") keeper.name = "распорядитель торгов";
		else keeper.name = "the auctioneer";
		DeleteAttribute(keeper, "ship");
		keeper.ship = "";
		aref arTo, arFrom;
		makearef(arTo, keeper.ship);
		makearef(arFrom, sld.Ship);
		CopyAttributes(arTo, arFrom);
		keeper.ShipAtAuction          = rPortman.id;
		keeper.ShipAtAuction.Base     = iBase;
		keeper.ShipAtAuction.Class    = iShipClass;
		keeper.ShipAtAuction.ShipName = sld.Ship.Name;
		PortAuction_RollLot(keeper); // первичный ролл цены/срока (как штатное выставление)
		keeper.location = ""; keeper.location.group = ""; keeper.location.locator = "";
	}
	else
	{
		int iHull = PortAuction_GetShipValue(sld, rPortman) / 10; // верфь: быстрый сброс за ~10% (как ворованный)
		if (iHull < 1) iHull = 1;
		iProceeds += iHull;
	}

	sld.ship.type = SHIP_NOTUSED;
	sld.CompanionTravel.Proceeds = iProceeds;
	if (iProceeds > 0) AddCharacterExpToSkill(sld, "Commerce", iProceeds / 2600.0); // опыт торговли компаньону
}

// LEO: безлошадный компаньон встаёт ЖДАТЬ ГГ в таверне города с жёлтым "!". DayUpdate его не трогает (гейт WaitInTavern).
void CompanionTravel_PlaceInTavern(ref sld, string sColony)
{
	CompanionTravel_DeleteSpecialShipAttributes(sld); // снять боевые спец-флаги рейса
	if (CheckAttribute(sld, "CompanionTravel.GroupID")) Group_DeleteGroup(sld.CompanionTravel.GroupID);
	Map_ReleaseQuestEncounter(sld.id); // энкаунтер отработал - снять (иначе Direct Sail плодит фантом)
	DeleteAttribute(sld, "mapEnc");
	DeleteAttribute(sld, "CompanionTravel.OutboundLive");
	sld.CompanionTravel.WaitInTavern = sColony;
	// LEO: сажаем на сидячее место по номеру слота (sit1..3; свободу удалённо не проверить - LAi_CheckLocatorFree
	//   нужна загруженная локация). FreeSitLocator сгоняет чужого. Нет слота - стоим (goto1).
	string sTav = sColony + "_tavern";
	string sSit = "";
	string sSitKey = CompanionTravel_FindSlotKey(sld.id);
	if (sSitKey == "Companion1") sSit = "sit1";
	if (sSitKey == "Companion2") sSit = "sit2";
	if (sSitKey == "Companion3") sSit = "sit3";
	if (sSit != "")
	{
		FreeSitLocator(sTav, sSit);
		ChangeCharacterAddressGroup(sld, sTav, "sit", sSit);
		LAi_SetSitType(sld);
	}
	else
	{
		ChangeCharacterAddressGroup(sld, sTav, "goto", "goto1");
		LAi_SetStayType(sld);
	}
	LAi_SetImmortal(sld, true);
	MarkCharacter(sld, "exclamationmarkY"); // жёлтый "!" над головой (сам гейтится опцией EnabledQuestsMarks)
	sld.dialog.filename = "Enc_Officer_dialog.c";
	sld.dialog.currentnode = "CompanionCommerce_Collect";
	sld.dialog.TempNode = "CompanionCommerce_Collect"; // «нет места в каюте» -> exit: при следующем разговоре снова предложит сбор
	sld.DeckDialogNode = "CompanionCommerce_Collect";
}

// LEO: компаньон дошёл до города - выполняет коммерческую операцию и встаёт ждать в таверне.
void CompanionTravel_DoCommerceWork(ref sld, string sColony)
{
	string sOp = "sell";
	if (CheckAttribute(sld, "CompanionTravel.CommerceOp")) sOp = sld.CompanionTravel.CommerceOp;
	if (sOp == "store")
	{
		bool bWatch = (CheckAttribute(sld, "CompanionTravel.StoreMode") && sld.CompanionTravel.StoreMode == "watch");
		CompanionTravel_DoStoreWork(sld, sColony, bWatch);
		return;
	}
	bool bAuction = (sOp == "auction");
	CompanionTravel_LiquidateShip(sld, sColony, bAuction);
	CompanionTravel_PlaceInTavern(sld, sColony);
	string sReason;
	if (bAuction) sReason = "выставил"+ NPCharSexPhrase(sld,"","а") +" корабль на торги в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и ждёт меня в тамошней таверне (выручку за корпус заберу у портмана)";
	else          sReason = "продал"+ NPCharSexPhrase(sld,"","а") +" корабль на верфи в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и ждёт меня в тамошней таверне";
	AddQuestRecord("CompanionTravel", "4");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sReason", sReason);
}

// LEO: ГГ нашёл компаньона в таверне - забираем выручку и берём безлошадного офицером в каюту (пассажиром).
//   Выручку за КОРПУС с торгов тут НЕ отдаём (она у портмана, ГГ забирает сам в ПУ).
void CompanionTravel_CollectFromTavern(ref sld)
{
	string sCompanion = CompanionTravel_FindSlotKey(sld.id);
	int iProceeds = 0;
	if (CheckAttribute(sld, "CompanionTravel.Proceeds")) iProceeds = sti(sld.CompanionTravel.Proceeds);
	if (iProceeds > 0)
	{
		AddMoneyToCharacter(PChar, iProceeds); // сумму "+X золотых" движок покажет сам - в инфошке НЕ дублируем
		Statistic_AddValue(PChar, "Money_get", iProceeds);
		Log_Info("Компаньон передал вам вырученные деньги."); // без цвета (сумму пишет отдельная строка движка)
		PlayStereoSound("interface\bort_jurnal.wav");
	}
	UnmarkCharacter(sld);

	// журнал: безлошадный возвращается офицером НА БОРТ (в эскадру не встаёт - корабля нет)
	AddQuestRecord("CompanionTravel", "4");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sReason", "вернул"+ NPCharSexPhrase(sld,"ся","ась") +" ко мне на борт без корабля");

	DeleteAttribute(sld, "CompanionTravel");
	DeleteAttribute(sld, "Tasks.Clone");
	if (sCompanion != "") DeleteAttribute(PChar, "CompanionTravel." + sCompanion);
	PChar.CompanionTravel = sti(PChar.CompanionTravel) - 1;
	if (GetAttrValue(PChar, "CompanionTravel") == 0) CloseQuestHeader("CompanionTravel"); // LEO: закрывать ПОСЛЕ декремента (иначе последний компаньон не закрывал запись)

	// безлошадный - офицером (пассажиром) в каюту; штатный офицерский диалог
	// LEO: TempNode = "hired" везде, где рейс меняет диалог (как у любого нанятого офицера): узел exit делает
	//   CurrentNode = TempNode, а у офицеров линейки Блада там застревала нода их квестового диалога (HTStep_22b) -
	//   после таверны все разговоры пустые. Исключение - таверна: там TempNode держит сбор (см. PlaceInTavern).
	sld.dialog.filename = "Enc_Officer_dialog.c";
	sld.dialog.currentnode = "hired";
	sld.dialog.TempNode = "hired";
	sld.DeckDialogNode = "hired";
	AddPassenger(PChar, sld, false);

	// LEO: мягкий уход как штатный найм офицера (LandEnc_OfficerHired) - гасим location="None" БЕЗ активного
	//   ChangeCharacterAddressGroup (тот испарял модель); исчезнет при релоаде. LifeDay НЕ ставим - сотрёт из
	//   пассажиров (EmptyAllFantomCharacter/InitCharacter без гарда на пати; ср. officers.c:536).
	LAi_SetImmortal(sld, false);
	sld.location = "None";
	sld.location.group = "";
	sld.location.locator = "";
}

// LEO: счётчики ПУ (как в PortStock_StoreShip) - чтобы корабль числился на стоянке в интерфейсе "Корабли в ПУ".
void CompanionTravel_PortStockCounters(ref rPortman)
{
	if (!CheckAttribute(rPortman, "Portman")) rPortman.Portman = 0;
	rPortman.Portman = sti(rPortman.Portman) + 1;
	if (!CheckAttribute(PChar, "ShipInStock")) PChar.ShipInStock = 0;
	PChar.ShipInStock = sti(PChar.ShipInStock) + 1;
}

// LEO: завершить рейс БЕЗ таверны/эскадры (компаньон ушёл держателем в ПУ) - снять спец-флаги/группу/энкаунтер,
//   освободить слот, закрыть журнал. Диалог восстанавливать ДО этого (Saved* лежат в CompanionTravel).
void CompanionTravel_FinishRun(ref sld)
{
	string sSlot = CompanionTravel_FindSlotKey(sld.id);
	CompanionTravel_DeleteSpecialShipAttributes(sld);
	if (CheckAttribute(sld, "CompanionTravel.GroupID")) Group_DeleteGroup(sld.CompanionTravel.GroupID);
	Map_ReleaseQuestEncounter(sld.id);
	DeleteAttribute(sld, "mapEnc");
	DeleteAttribute(sld, "CompanionTravel");
	DeleteAttribute(sld, "Tasks.Clone");
	if (sSlot != "") DeleteAttribute(PChar, "CompanionTravel." + sSlot);
	PChar.CompanionTravel = sti(PChar.CompanionTravel) - 1;
	if (GetAttrValue(PChar, "CompanionTravel") == 0) CloseQuestHeader("CompanionTravel");
}

// LEO: рейс на ХРАНЕНИЕ корабля в ПУ города. bWatch=false - компаньон САМ остаётся держателем при корабле (уходит
//   в "Корабли в ПУ", забираешь его там штатно как капитана); bWatch=true - нанимаем сторожа, а компаньон безлошадным
//   ждёт в таверне (заберёшь офицером, он же годится в капитаны для снятого корабля). Плата за стоянку берётся ПРИ
//   СНЯТИИ штатным PortStock_GetReturnDebt (долг копится от Date; MoneyForShip зависит от города/нации портмана).
void CompanionTravel_DoStoreWork(ref sld, string sColony, bool bWatch)
{
	ref rPortman = CharacterFromID(sColony + "_PortMan");
	int iPrice = GetPortManPriceExt(rPortman, sld); // месячная цена стоянки (разная по городам/нациям); долг = месяцы*iPrice при снятии
	Wounded_ClearToInfirmary(sld); // раненых на сдаваемый корабль не оставляем - в лазарет города
	string sReason;
	if (bWatch)
	{
		ref holder = GetCharacter(NPC_GenerateCharacter("ShipInStockMan", "PKM_rab_"+(rand(3)+1), "man", "man", rand(5)+1, rPortman.nation, -1, false));
		holder.id = holder.id + "_" + holder.index; // уникальный ID (детект сторожа = HasSubStr id, "ShipInStockMan_")
		holder.reputation = 10 + rand(80);
		holder.lastname = holder.name;
		if (LanguageGetLanguage() == "russian") holder.name = "сторож";
		else holder.name = "the watchman";
		DeleteAttribute(holder, "ship");
		holder.ship = "";
		holder.ShipInStockMan = rPortman.id;
		holder.ShipInStockMan.MoneyForShip = iPrice;
		holder.ShipInStockMan.AltDate = GetQuestBookDataDigit();
		SaveCurrentNpcQuestDateParam(holder, "ShipInStockMan.Date"); // с этой даты копится долг - оплатишь при снятии (штатно)
		aref arTo, arFrom;
		makearef(arTo, holder.ship);
		makearef(arFrom, sld.Ship);
		CopyAttributes(arTo, arFrom);
		holder.location = ""; holder.location.group = ""; holder.location.locator = "";
		CompanionTravel_PortStockCounters(rPortman);
		sld.ship.type = SHIP_NOTUSED;
		sld.CompanionTravel.Proceeds = 0; // хранение денег не приносит - при встрече отдавать нечего
		CompanionTravel_PlaceInTavern(sld, sColony); // безлошадный ждёт в таверне (как верфь/аукцион)
		sReason = "оставил"+ NPCharSexPhrase(sld,"","а") +" корабль на стоянке в порт-управлении " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Gen")) + " под сторожем и ждёт меня в тамошней таверне";
		AddQuestRecord("CompanionTravel", "4");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sReason", sReason);
		return;
	}
	// bWatch=false: компаньон сам остаётся держателем при корабле в ПУ (не в таверну, не в эскадру)
	CompanionTravel_RestoreDialog(sld); // вернуть штатный диалог капитана ДО чистки CompanionTravel (Saved* там)
	sld.ShipInStockMan = rPortman.id;
	sld.ShipInStockMan.MoneyForShip = iPrice;
	sld.ShipInStockMan.AltDate = GetQuestBookDataDigit();
	SaveCurrentNpcQuestDateParam(sld, "ShipInStockMan.Date"); // с этой даты копится долг - оплатишь при снятии (штатно)
	sld.location = ""; sld.location.group = ""; sld.location.locator = "";
	CompanionTravel_PortStockCounters(rPortman);
	sReason = "отвёл"+ NPCharSexPhrase(sld,"","а") +" корабль на стоянку в порт-управление " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Gen")) + " и остался при нём (заберём его там же)";
	AddQuestRecord("CompanionTravel", "4");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sReason", sReason);
	CompanionTravel_FinishRun(sld); // журнал уже записан - чистим рейс/слот
}

// LEO: есть ли свободный слот для рейса? Слотов 3 (цикл ниже в SetTraveller). Без свободного отправку НЕ даём - иначе
//   SetTraveller не находит группу, GroupID выходит битый, счётчик рассинхронивается (застрявший/фантомный компаньон).
bool CompanionTravel_HasFreeSlot()
{
	int j;
	for (j = 1; j <= 3; j++)
	{
		if (Group_IsDead("CompanionTravel_Companion" + j)) return true;
	}
	return false;
}

void CompanionTravel_SetTraveller(ref _NPChar)
{
	if (!CompanionTravel_CheckDeparture(_NPChar)) return;
	ref sld;
	string sTemp, attrLoc, attr;
	sld = GetCharacter(CompanionTravel_DepartureIndex(_NPChar));

	if(!CheckAttribute(PChar, "CompanionTravel")) PChar.CompanionTravel = 0;
	PChar.CompanionTravel = sti(PChar.CompanionTravel) + 1; // Сколько компаньонов уже отправили
	for(iTemp=1; iTemp<=3; iTemp++) // Нужно, чтоб была свободная группа
	{
		if(Group_IsDead("CompanionTravel_Companion"+iTemp))
			attr = "Companion" + iTemp;
	}

	CompanionTravel_SetSpecialShipAttributes(sld);
	sld.CompanionTravel.GroupID = "CompanionTravel_" + attr; // Это группа. Её потом нужно будет потереть

	sTemp = _NPChar.ID;
	PChar.CompanionTravel.(attr).ID = sld.ID ; // Запомним ID
	PChar.CompanionTravel.(attr).Days = sti(sld.CompanionTravel.Days); // Сколько дней будет путешествовать
	PChar.CompanionTravel.(attr).ToColonyID = sld.CompanionTravel.ToColonyID; // Где с ним назначена встреча

	attrLoc = "CompanionTravel_" + attr; // Заносим в группу

	Group_FindOrCreateGroup(attrLoc);
	Group_SetType(attrLoc, "pirate");
	Group_AddCharacter(attrLoc, PChar.CompanionTravel.(attr).ID);
	Group_SetGroupCommander(attrLoc, PChar.CompanionTravel.(attr).ID);
	Group_SetTaskRunaway(attrLoc, PLAYER_GROUP); // Уходим
	Group_LockTask(attrLoc);

	// Записи в СЖ
	ReOpenQuestHeader("CompanionTravel");
	// LEO: рейс на склад - НЕ вольное плавание "жди на рейде месяц" (запись 1), а разовое поручение.
	//   Пишем универсальную запись 3 (sReason) с правильным текстом, иначе журнал противоречил приказу.
	//   НЕ через return - в конце функции обязателен RemoveCharacterCompanion.
	//   ⚠️ Все AddQuestUserData - строго ПОСЛЕ AddQuestRecord, иначе плейсхолдеры пустые ("Капитан  на ").
	bool bCommerceRun = (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "commerce");
	bool bStorageRun = (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "storage");
	bool bBuyRun = (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "buy");
	bool bAuctionRun = (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "auction");
	if (bBuyRun)
	{
		// единый торговый рейс - сумму НЕ пишем при отправке (расчёт по прибытии); знаковый итог встанет там
		bool bRet = (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true);
		string sTradeTail = " и ждать нас там на рейде";
		if (bRet) sTradeTail = " и вернуться к нам";
		// закупка оплачена сразу -> сумму пишем при отправке; продажа -> сумма (по цене прибытия) встанет по прибытии
		string sDispSum = "";
		if (CheckAttribute(sld, "CompanionTravel.TradeKind") && sld.CompanionTravel.TradeKind == "buy") sDispSum = CompanionTravel_TradeSumText(sld);
		AddQuestRecord("CompanionTravel", "3");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
		AddQuestUserData("CompanionTravel", "sReason", "отправлен"+ NPCharSexPhrase(sld,"","а") +" совершить торговые операции в " + XI_ConvertString(CompanionTravel_ColonyTextId(sld.CompanionTravel.ToColonyID, "Voc")) + sDispSum + sTradeTail + " (в пути " + FindRussianDaysString(sti(PChar.CompanionTravel.(attr).Days)) + " в один конец)");
	}
	else if (bAuctionRun)
	{
		bool bRetA = (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true);
		string sAuTail = " и ждать нас в последнем городе";
		if (bRetA) sAuTail = " и доставить её нам"; // LEO: было "вернуться к нам с выручкой" - тавтология с началом фразы
		AddQuestRecord("CompanionTravel", "3");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
		AddQuestUserData("CompanionTravel", "sReason", "отправлен"+ NPCharSexPhrase(sld,"","а") +" собрать выручку за проданные корабли" + sAuTail);
	}
	else if (bCommerceRun)
	{
		string sOpR = "sell";
		if (CheckAttribute(sld, "CompanionTravel.CommerceOp")) sOpR = sld.CompanionTravel.CommerceOp;
		string sWhat;
		if (sOpR == "auction")    sWhat = "выставить его на торги в порт-управлении";
		else if (sOpR == "store")
		{
			sWhat = "поставить его на стоянку в порт-управлении и остаться при нём";
			if (CheckAttribute(sld, "CompanionTravel.StoreMode") && sld.CompanionTravel.StoreMode == "watch")
				sWhat = "поставить его на стоянку в порт-управлении под присмотром сторожа, а самому вернуться ко мне";
		}
		else                      sWhat = "продать его на верфи";
		AddQuestRecord("CompanionTravel", "4");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sReason", "отправил"+ NPCharSexPhrase(sld,"","а") +" корабль '" + sld.Ship.name + "' к " + XI_ConvertString(CompanionTravel_ColonyTextId(sld.CompanionTravel.ToColonyID, "Dat")) + ", чтобы " + sWhat + " (в пути " + FindRussianDaysString(sti(PChar.CompanionTravel.(attr).Days)) + ")");
	}
	else if (bStorageRun)
	{
		string sTail;
		if (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true)
			sTail = " и вернуться в эскадру";
		else
			sTail = " и ждать меня там на рейде";
		AddQuestRecord("CompanionTravel", "3");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
		AddQuestUserData("CompanionTravel", "sReason", "отправил"+ NPCharSexPhrase(sld,"ся","ась") +" к нашему складу в " + XI_ConvertString(CompanionTravel_ColonyTextId(sld.CompanionTravel.ToColonyID, "Voc")) + ", чтобы разобраться с грузом" + sTail + " (в пути " + FindRussianDaysString(sti(PChar.CompanionTravel.(attr).Days)) + ")");
	}
	else
	{
		AddQuestRecord("CompanionTravel", "1");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
		AddQuestUserData("CompanionTravel", "sDays", PChar.CompanionTravel.(attr).Days);
		// LEO: текстовый id города через ColonyTextId (без-префиксные острова тоже). Тексты логов _1.._5 одинаковые - один вызов.
		AddQuestUserData("CompanionTravel", "sColony", XI_ConvertString(CompanionTravel_ColonyTextId(sld.CompanionTravel.ToColonyID, "Dat")));
		Log_Info(GetConvertStrWithReplace("Variable_CompanionTravel_5", "Logs.txt", "#space#", " ") + XI_ConvertString(CompanionTravel_ColonyTextId(sld.CompanionTravel.ToColonyID, "Gen")));
	}

	RemoveCharacterCompanion(PChar, sld);
	// Исходный диалог сохраняем для возврата в эскадру; в рейсе капитан говорит "на задании".
	if (CheckAttribute(sld, "dialog.filename"))    sld.CompanionTravel.SavedDlgFile = sld.dialog.filename;
	if (CheckAttribute(sld, "dialog.currentnode")) sld.CompanionTravel.SavedNode    = sld.dialog.currentnode; // LEO: старт-нода каютного клона
	if (CheckAttribute(sld, "DeckDialogNode"))     sld.CompanionTravel.SavedDeck    = sld.DeckDialogNode;
	sld.dialog.filename = "Enc_Officer_dialog.c";
	sld.dialog.currentnode = "CompanionTravel_OnMission";
	sld.dialog.TempNode = "hired";
	sld.DeckDialogNode = "CompanionTravel_OnMission";
	// LEO: рейс туда - и для склада, и для обычного "жди на рейде". Прогресс по ВРЕМЕНИ: счётчик Days тикает
	//   в DayUpdate каждые сутки (в городе и на глобалке), дошёл до 0 -> прибытие (AbstractArrive). Живой
	//   энкаунтер (движок) = визуал на глобалке + РАННЕЕ прибытие по контакту (OnOutboundArrive), если игрок рядом.
	CompanionTravel_LaunchOutbound(sld, sti(PChar.CompanionTravel.(attr).Days));
}

void CompanionTravel_ProcessAllTravellers() // Этот метод вызывается каждый день из QuestActions()
{
	aref arTravellers;
	if (!CheckAttribute(PChar,"CompanionTravel")) return;
	CompanionTravel_TryPayStorageDebt(false); // LEO: суточный ДОБОР ОСТАТКА долга за склад (только после встречи, если не хватило)
	makearef(arTravellers, PChar.CompanionTravel);

	string sCompanionTraveller;
	for(int i=0; i<GetAttributesNum(arTravellers); i++)
	{
		sCompanionTraveller = GetAttributeName(GetAttributeN(arTravellers, i));
		// LEO: PChar.CompanionTravel хранит и не-слоты (StorageDebt, транзиентный Snap) - их DayUpdate не касается
		//   (иначе он их же и удалит, приняв за протухший слот). Обрабатываем только слоты CompanionN.
		if (findSubStr(sCompanionTraveller, "Companion", 0) != 0) continue;
		CompanionTravel_DayUpdate(sCompanionTraveller); // LEO: убран отладочный спам "Обработка компаньона..."
	}
}

void CompanionTravel_DayUpdate(string sCompanion) // Обработка конкретного компаньона-путешественника
{
	string sID
	if (CheckAttribute(PChar, "CompanionTravel."+sCompanion+".ID")) sID = PChar.CompanionTravel.(sCompanion).ID; else {DeleteAttribute(PChar,"CompanionTravel."+sCompanion); return;}
	int iDays = sti(PChar.CompanionTravel.(sCompanion).Days);
	ref rCompanion = CharacterFromID(sID);
	// LEO: снаряжение офицера - он БЕЗ корабля (шиплесс) идёт в город абстрактным таймером. Ship-based суточную
	//   обработку (еда/поломки/RealShips[Type]) НЕ трогаем - корабля ещё нет (Type=SHIP_NOTUSED -> вылет за границы).
	//   Срок вышел -> AbstractArrive (ветка Task=="outfit" -> DoOutfitWork). Прибытие ВСЕГДА абстрактно (энкаунтера нет).
	if (CheckAttribute(rCompanion, "CompanionTravel.OutfitTravel"))
	{
		int iLeftO = sti(PChar.CompanionTravel.(sCompanion).Days) - 1;
		PChar.CompanionTravel.(sCompanion).Days = iLeftO;
		if (iLeftO <= 0) CompanionTravel_AbstractArrive(sCompanion, sID);
		return;
	}
	CompanionTravel_SetExperienceToTraveller(rCompanion); // Начисляем экспу
	if(!bNoEatNoRats)
	{
		DailyEatCrewUpdateForShip(rCompanion, true);
	}
	CompanionTravel_SetRandomEvent(rCompanion); // Случайное событие по типу покоцался корпус или паруса и т.д.
	// LEO: рейс ТУДА - прогресс по ВРЕМЕНИ (счётчик дней тикает и в городе, и на карте; живой энкаунтер = визуал
	//   + раннее прибытие по контакту OnOutboundArrive). Прибытие по времени - см. условие ниже.
	if (CheckAttribute(rCompanion, "CompanionTravel.OutboundLive"))
	{
		int iLeft = sti(PChar.CompanionTravel.(sCompanion).Days) - 1;
		PChar.CompanionTravel.(sCompanion).Days = iLeft;
		// LEO: срок вышел -> прибытие абстрактно ТОЛЬКО вне глобалки. На карте живой энкаунтер дойдёт сам
		//   (не убираем на полпути из-за обхода островов/ветра); зайдёт в город - следующий тик проведёт абстрактно.
		if (iLeft <= 0 && !IsEntity(worldMap))
		{
			CompanionTravel_AbstractArrive(sCompanion, sID);
		}
		return;
	}
	if (CheckAttribute(rCompanion, "CompanionTravel.Meeting"))
	{
		// возврат ждёт встречи с игроком; страховка на случай, если игрок надолго засел в городе
		if (CheckAttribute(rCompanion, "CompanionTravel.MeetDeadline"))
		{
			int iMd = sti(rCompanion.CompanionTravel.MeetDeadline) - 1;
			rCompanion.CompanionTravel.MeetDeadline = iMd;
			if (iMd <= 0)
			{
				DeleteAttribute(rCompanion, "CompanionTravel.Meeting");
				DeleteAttribute(rCompanion, "CompanionTravel.MeetDeadline");
				Map_ReleaseQuestEncounter(sID);
				AddQuestRecord("CompanionTravel", "3");
				AddQuestUserData("CompanionTravel", "sCapName", GetFullName(rCompanion));
				AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(rCompanion.Ship.Type)].Basename + "Voc")) + " '" + rCompanion.Ship.name + "'");
				AddQuestUserData("CompanionTravel", "sReason", "не сумел найти нас в море и ждёт в " + XI_ConvertString(CompanionTravel_ColonyTextId(rCompanion.CompanionTravel.ToColonyID, "Voc")));
				CompanionTravel_SetCompanionToColony(rCompanion.CompanionTravel.ToColonyID, "CompanionTravel_"+sCompanion, sID);
			}
		}
		return;
	}
	// LEO: коммерческий рейс завершён - компаньон безлошадным ждёт ГГ в таверне. Счётчик его не касается,
	//   уговорённого срока/припасов нет (корабля нет) - ждёт, пока ГГ сам не заберёт (CollectFromTavern).
	if (CheckAttribute(rCompanion, "CompanionTravel.WaitInTavern")) return;
	iDays--;
	PChar.CompanionTravel.(sCompanion).Days = iDays;
	if(iDays == 0) // Если дней < 0, значит он уже ждет в колонии
	{
		CompanionTravel_TimerToSetInColony(sCompanion); // Доехал - ставим в колонию или топим
	}

	// LEO: ждущий на рейде уходит, если ВЫШЕЛ УГОВОРЁННЫЙ СРОК (месяц) ИЛИ КОНЧИЛИСЬ ПРИПАСЫ (еда/ром).
	//   Days у ждущего идёт 0..-COMPANION_WAIT_GRACE. Раньше был хардкод -30 без учёта припасов.
	if (iDays < 0 && CheckAttribute(rCompanion, "CompanionTravel.WaitOnColony"))
	{
		// LEO: без составных ||-условий (сторм-гоча: `bool x = (A||B)` не компилится) - копим флаг отдельными if.
		bool bLeave = false;
		if (iDays <= -COMPANION_WAIT_GRACE)      bLeave = true; // вышел уговорённый срок (месяц)
		if (CalculateShipFood(rCompanion) <= 0)  bLeave = true; // кончилась провизия
		if (CalculateShipRum(rCompanion)  <= 0)  bLeave = true; // кончился ром
		if (bLeave)
		{
			CompanionTravel_DeleteCompanion(sID, sCompanion, true); // покидает колонию ("не дождался")
		}
	}
}

void CompanionTravel_TimerToSetInColony(string sCompanion) // Таймер вышел (т.е. компаньона нужно потавить в колонию или потопить)
{
	if(!CheckAttribute(PChar, "CompanionTravel."+sCompanion+".ToColonyID")
	|| !CheckAttribute(PChar, "CompanionTravel."+sCompanion+".ID") || !CheckAttribute(PChar, "CompanionTravel."+sCompanion+".Days"))
	{
		Log_Info(GetConvertStrWithReplace("Variable_CompanionTravel_11", "Logs.txt", "#space#", " ")+sCompanion+". Немедленно сообщите Warship'у.");
		return;
	}

	string sID = PChar.CompanionTravel.(sCompanion).ID;
	string sColony = PChar.CompanionTravel.(sCompanion).ToColonyID;
	int iDays = sti(PChar.CompanionTravel.(sCompanion).Days);
	ref sld = CharacterFromID(sID);

	if(CheckAttribute(PChar, "CompanionTravel."+sCompanion+".Sink")) // Не дошёл до места назначения
	{
		CompanionTravel_DeleteCompanion(sID, sCompanion, false);
		return;
	}

	// LEO: второе плечо рейса на склад - компаньон идёт НЕ в колонию, а к нам на встречу.
	//   Без этой развилки счётчик дней снова дошёл бы до нуля и рейс зациклился бы на складе.
	if (CheckAttribute(sld, "CompanionTravel.Phase") && sld.CompanionTravel.Phase == "back")
	{
		CompanionTravel_Meet(sCompanion, sID);
		return;
	}

	// LEO: рейс на склад - по прибытии выполняем то, ради чего шли: сгружаем привезённое,
	//   забираем зарезервированное. Запись в СЖ, чтобы игрок видел, что дело сделано.
	if (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "storage")
	{
		CompanionTravel_DoStorageWork(sld, sColony);
		AddQuestRecord("CompanionTravel", "3");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
		// "Voc" в текстах колоний - предложный падеж ("в Порт Рояле"), а не звательный
		AddQuestUserData("CompanionTravel", "sReason", "провёл все операции на нашем складе в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")));

		// LEO: приказано возвращаться - в колонии не оседаем, а идём вторым плечом к ГГ.
		//   Срок обратного плеча берём тем же способом, что и прямого (от нашего текущего места).
		if (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true)
		{
			int iBack = makeint(GetDistanceToColony2D(sColony) / 300 + 1.0);
			if (iBack < 1) iBack = 1;
			sld.CompanionTravel.Phase = "back";
			PChar.CompanionTravel.(sCompanion).Days = iBack; // счётчик снова тикает вниз, теперь до встречи
			AddQuestUserData("CompanionTravel", "sReason", "провёл все операции на нашем складе в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и возвращается в эскадру, ждать его через " + FindRussianDaysString(iBack));
			return; // в колонию не ставим - он в море
		}
	}

	CompanionTravel_SetCompanionToColony(sColony, "CompanionTravel_"+sCompanion, sID); // Ставим его в колонию
}

// LEO: сколько дней ждём живую встречу возврата (MeetDeadline), прежде чем деградировать в ожидание в порту
//   (игрок засел в городе и не выходит на глобалку - энкаунтер встречи не отрабатывает).
#define COMPANION_SAIL_GRACE 20

// LEO: сколько дней компаньон ждёт нас на рейде в пункте назначения, как уговорено в диалоге ("в течение
//   месяца"). Раньше было хардкодом -30 в DayUpdate. По истечении срока ИЛИ при кончившихся припасах - уходит.
#define COMPANION_WAIT_GRACE 30

// LEO: сколько дней компаньон ЕЩЁ будет ждать на рейде = остаток уговорённого срока (месяц), но не дольше,
//   чем хватит припасов (еды/рома). Days у ждущего идёт 0..-COMPANION_WAIT_GRACE. Для подсказки в диалоге
//   и как реальный лимит стоянки. Считать по РЕАЛЬНОМУ компаньону (не клону).
int CompanionTravel_WaitDaysLeft(ref rCompanion)
{
	int iLeft = COMPANION_WAIT_GRACE;
	string sSlot = CompanionTravel_FindSlotKey(rCompanion.id);
	if (sSlot != "" && CheckAttribute(PChar, "CompanionTravel." + sSlot + ".Days"))
	{
		iLeft = COMPANION_WAIT_GRACE + sti(PChar.CompanionTravel.(sSlot).Days); // остаток месяца
	}
	if (iLeft < 0) iLeft = 0;
	int iFood = CalculateShipFood(rCompanion);
	int iRum  = CalculateShipRum(rCompanion);
	if (iFood < iLeft) iLeft = iFood; // припасы могут ограничить срок раньше месяца
	if (iRum  < iLeft) iLeft = iRum;
	if (iLeft < 0) iLeft = 0;
	return iLeft;
}

void CompanionTravel_LaunchOutbound(ref sld, int iDays)
{
	if (!CompanionTravel_CheckDeparture(sld)) return;
	sld.CompanionTravel.OutboundLive = true; // рейс идёт: прогресс по счётчику Days (DayUpdate), живой энкаунтер = визуал/раннее прибытие
	sld.mapEnc.type = "trade"; // ОБЯЗАТЕЛЬНО для GenerateMapEncounter_Alone (иначе энкаунтер не спавнится); trade = мирный
	// По умолчанию старт - ВПЛОТНУЮ К ИГРОКУ (locNameStart="" -> движок спавнит у playerShip). НЕ GetCurrentTown():
	//   в море это имя акватории -> движок телепортировал бы к ПОРТУ острова = горизонт. Но при ПЕРЕХОДЕ МЕЖДУ портами
	//   (многоногий рейс сбора) FromPort задан = текущий порт -> энкаунтер стартует ИЗ него (GetQuestLocator).
	string sFrom = "";
	if (CheckAttribute(sld, "CompanionTravel.FromPort")) sFrom = sld.CompanionTravel.FromPort;
	Map_CreateCompanionSail(sFrom, sld.id, sld.CompanionTravel.ToColonyID, iDays + 10);
	DeleteAttribute(sld, "CompanionTravel.FromPort"); // одноразово: следующий старт по умолчанию от ГГ
}

// LEO: прибытие по ВРЕМЕНИ (счётчик Days дошёл до 0 в DayUpdate) - основной путь, срабатывает и в городе,
//   и на глобалке. Снимаем живой энкаунтер, разбираем груз/аренду, пишем журнал и решаем ожидание/возврат
//   (для возврата ставим живой энкаунтер встречи - всплывёт на след. выходе на глобалку). Живое прибытие
//   по контакту (OnOutboundArrive) - лишь РАННИЙ вариант, если игрок доплыл рядом до истечения срока.
void CompanionTravel_AbstractArrive(string sCompanion, string sID)
{
	ref sld = CharacterFromID(sID);
	string sColony = sld.CompanionTravel.ToColonyID;
	DeleteAttribute(sld, "CompanionTravel.OutboundLive");
	DeleteAttribute(sld, "CompanionTravel.SailDeadline");
	Map_ReleaseQuestEncounter(sID); // если энкаунтер всё же висел - убрать

	// LEO: снаряжение офицера - выдаём корабль/команду/груз и шлём догонять эскадру (свой журнал + SpawnMeet внутри).
	if (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "outfit")
	{
		CompanionTravel_DoOutfitWork(sld, sColony);
		return;
	}

	// LEO: коммерческий рейс - ликвидация корабля + ожидание в таверне (свой журнал внутри). Не склад/не порт.
	if (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "commerce")
	{
		CompanionTravel_DoCommerceWork(sld, sColony);
		return;
	}

	// LEO: сбор выручки с торгов - забрал в текущем порту, жадно к следующему или домой (свой журнал внутри).
	if (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "auction")
	{
		CompanionTravel_AuctionArrive(sCompanion, sID);
		return;
	}

	// LEO: торговый рейс - по прибытии компаньон отрабатывает манифест ТОВАРА В ГОРОДЕ (TradeAtArrival). Выручку за
	//   продажу отдаст ПРИ ВСТРЕЧЕ (закупка оплачена ещё при отправке). Затем по флагу Return: возврат ИЛИ ждёт на рейде.
	if (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "buy")
	{
		CompanionTravel_TradeAtArrival(sld);
		string sTradeSum = ""; // по прибытии сумму пишем для продажи (выручка по цене прибытия); закупка - сумма при отправке
		if (CheckAttribute(sld, "CompanionTravel.TradeKind") && sld.CompanionTravel.TradeKind == "sale") sTradeSum = CompanionTravel_TradeSumText(sld);
		string sDid = "совершил"+ NPCharSexPhrase(sld,"","а") +" торговые операции в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + sTradeSum;
		AddQuestRecord("CompanionTravel", "3");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
		if (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true)
		{
			AddQuestUserData("CompanionTravel", "sReason", sDid + " и идёт обратно к нам");
			sld.CompanionTravel.Phase = "back";
			CompanionTravel_SpawnMeetEncounter(sCompanion, sID, sColony);
			return;
		}
		AddQuestUserData("CompanionTravel", "sReason", sDid + " и ждёт нас там на рейде");
		if (sCompanion != "") PChar.CompanionTravel.(sCompanion).Days = 0;
		CompanionTravel_SetCompanionToColony(sColony, "CompanionTravel_" + sCompanion, sID);
		return;
	}

	bool bStorage = (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "storage");
	AddQuestRecord("CompanionTravel", "3");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
	if (bStorage)
	{
		CompanionTravel_DoStorageWork(sld, sColony); // разбор груза + оплата аренды
		// LEO: приказано возвращаться - даже в АБСТРАКТНОМ прибытии не оседаем в порту, а ставим ЖИВОЙ возврат:
		//   энкаунтер companionmeet всплывёт при следующем выходе игрока на глобалку (wdmEvent_AddQuestEncounters).
		//   Если игрок так и сидит в городе - по MeetDeadline деградирует в ожидание в порту.
		if (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true)
		{
			AddQuestUserData("CompanionTravel", "sReason", "провёл все операции на нашем складе в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и идёт обратно к нам");
			sld.CompanionTravel.Phase = "back";
			CompanionTravel_SpawnMeetEncounter(sCompanion, sID, sColony);
			return;
		}
		AddQuestUserData("CompanionTravel", "sReason", "разобрал"+ NPCharSexPhrase(sld,"","а") +" груз на нашем складе в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и ждёт нас там");
	}
	else
	{
		AddQuestUserData("CompanionTravel", "sReason", "доплыл"+ NPCharSexPhrase(sld,"","а") +" до " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Gen")) + " и ждёт нас там на рейде");
	}
	if (sCompanion != "") PChar.CompanionTravel.(sCompanion).Days = 0; // прибыл - сброс счётчика (см. OnOutboundArrive)
	CompanionTravel_SetCompanionToColony(sColony, "CompanionTravel_" + sCompanion, sID);
}

// LEO: живой рейс дошёл до места назначения (Map_TraderSucces). Складской - работаем со складом и решаем
//   возврат; обычный "жди на рейде" - просто встаём ждать в порту. Запись в СЖ о прибытии в обоих случаях.
void CompanionTravel_OnOutboundArrive(string sID)
{
	ref sld = CharacterFromID(sID);
	DeleteAttribute(sld, "CompanionTravel.OutboundLive");
	DeleteAttribute(sld, "CompanionTravel.SailDeadline");
	// LEO: рейс ТУДА отработал - СНИМАЕМ квест-энкаунтер (у него quest.chrID, он переживает вход/выход и сам не
	//   исчезнет). Иначе висячая запись worldMap.encounters плодит ФАНТОМ через Direct Sail (дубль корабля).
	Map_ReleaseQuestEncounter(sID);
	DeleteAttribute(sld, "mapEnc"); // остаточная метка энкаунтера на персонаже - тоже убрать
	string sColony = sld.CompanionTravel.ToColonyID;
	string sCompanion = CompanionTravel_FindSlotKey(sID);

	// LEO: коммерческий рейс - ликвидация корабля + ожидание в таверне (свой журнал внутри). Не склад/не порт.
	if (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "commerce")
	{
		CompanionTravel_DoCommerceWork(sld, sColony);
		return;
	}

	// LEO: сбор выручки с торгов - забрал в текущем порту, жадно к следующему или домой (свой журнал внутри).
	if (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "auction")
	{
		CompanionTravel_AuctionArrive(sCompanion, sID);
		return;
	}

	// LEO: торговый рейс - по прибытии манифест ТОВАРА В ГОРОДЕ (TradeAtArrival); выручку за продажу отдаст ПРИ ВСТРЕЧЕ
	//   (закупка оплачена при отправке). Затем возврат ИЛИ ожидание на рейде.
	if (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "buy")
	{
		CompanionTravel_TradeAtArrival(sld);
		string sTradeSum = ""; // по прибытии сумму пишем для продажи (выручка по цене прибытия); закупка - сумма при отправке
		if (CheckAttribute(sld, "CompanionTravel.TradeKind") && sld.CompanionTravel.TradeKind == "sale") sTradeSum = CompanionTravel_TradeSumText(sld);
		string sDid = "совершил"+ NPCharSexPhrase(sld,"","а") +" торговые операции в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + sTradeSum;
		AddQuestRecord("CompanionTravel", "3");
		AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
		AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
		if (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true)
		{
			AddQuestUserData("CompanionTravel", "sReason", sDid + " и идёт обратно к нам");
			sld.CompanionTravel.Phase = "back";
			CompanionTravel_SpawnMeetEncounter(sCompanion, sID, sColony);
			return;
		}
		AddQuestUserData("CompanionTravel", "sReason", sDid + " и ждёт нас там на рейде");
		if (sCompanion != "") PChar.CompanionTravel.(sCompanion).Days = 0;
		CompanionTravel_SetCompanionToColony(sColony, "CompanionTravel_" + sCompanion, sID);
		return;
	}

	bool bStorage = (CheckAttribute(sld, "CompanionTravel.Task") && sld.CompanionTravel.Task == "storage");

	AddQuestRecord("CompanionTravel", "3");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");

	if (bStorage)
	{
		CompanionTravel_DoStorageWork(sld, sColony); // разбор груза + оплата аренды (SettleStorageRent)
		if (CheckAttribute(sld, "CompanionTravel.Return") && sld.CompanionTravel.Return == true)
		{
			AddQuestUserData("CompanionTravel", "sReason", "провёл все операции на нашем складе в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и идёт обратно к нам");
			sld.CompanionTravel.Phase = "back";
			CompanionTravel_SpawnMeetEncounter(sCompanion, sID, sColony); // живой возврат (companionmeet)
			return;
		}
		AddQuestUserData("CompanionTravel", "sReason", "провёл все операции на нашем складе в " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Voc")) + " и ждёт нас там на рейде");
	}
	else
	{
		AddQuestUserData("CompanionTravel", "sReason", "доплыл"+ NPCharSexPhrase(sld,"","а") +" до " + XI_ConvertString(CompanionTravel_ColonyTextId(sColony, "Gen")) + " и ждёт нас там на рейде");
	}
	// прибыл и встал ждать: сбрасываем счётчик в 0, чтобы штатная 30-дневная стоянка отсчиталась заново,
	//   а TimerToSetInColony не сработал повторно (Days был заморожен на время живого рейса).
	if (sCompanion != "") PChar.CompanionTravel.(sCompanion).Days = 0;
	CompanionTravel_SetCompanionToColony(sColony, "CompanionTravel_" + sCompanion, sID);
}

// LEO: живой возврат компаньона на глобалке. Дружественный энкаунтер (движок WdmCompanionShip, модель
//   ranger): наводится на игрока, на сближении исчезает и шлёт WorldMap_CompanionMet -> OnMeetingDone
//   (вступление в эскадру + голос). Нет берега для выхода - деградируем в ожидание в порту.
void CompanionTravel_SpawnMeetEncounter(string sCompanion, string sID, string sHome)
{
	ref sld = CharacterFromID(sID);
	sld.CompanionTravel.Meeting = true;   // на случай сейвлоада во время рейса
	sld.CompanionTravel.MeetDeadline = COMPANION_SAIL_GRACE; // страховка: игрок засел в городе -> абстрактно в порт
	sld.mapEnc.type = "trade"; // ОБЯЗАТЕЛЬНО для GenerateMapEncounter_Alone (иначе энкаунтер не спавнится)
	// Старт ОТ порта СКЛАДА (sHome), откуда компаньон отчаливает обратно (движок -> GetQuestLocator), затем к игроку.
	Map_CreateCompanionMeet(sHome, sID, 30);
}

// LEO: движок сообщил, что энкаунтер-компаньон догнал игрока (WorldMap_CompanionMet). Берём в эскадру.
void CompanionTravel_OnCompanionMet()
{
	string sID = GetEventData();
	if (GetCharacterIndex(sID) < 0) return;
	CompanionTravel_OnMeetingDone(sID);
}

// LEO: движок сообщил, что энкаунтер-компаньон дошёл до города-склада (WorldMap_CompanionArrived).
void CompanionTravel_OnCompanionArrived()
{
	string sID = GetEventData();
	if (GetCharacterIndex(sID) < 0) return;
	CompanionTravel_OnOutboundArrive(sID);
}

// LEO: ключ ветки (Companion1..3) по ID компаньона, "" если не найден
string CompanionTravel_FindSlotKey(string sID)
{
	int i;
	string attr;
	for (i = 1; i <= 3; i++)
	{
		attr = "Companion" + i;
		if (CheckAttribute(PChar, "CompanionTravel." + attr + ".ID") && PChar.CompanionTravel.(attr).ID == sID)
			return attr;
	}
	return "";
}

// LEO: суммарное МЕСЯЧНОЕ жалованье компаньонов, которые сейчас НА ПОРУЧЕНИИ / ЖДУТ НА РЕЙДЕ. Они вышли из
//   эскадры (RemoveCharacterCompanion в SetTraveller), поэтому штатный SalaryNextDayUpdate их не видит - но
//   экипаж кормить надо и в отлучке (иначе лазейка «гоняю компаньонов, чтобы не платить жалованье»). Зовётся
//   из time_events.c и добавляется к общей сумме выплаты. Только свои наёмные (GetRemovable), как в штатном.
int CompanionTravel_GetTravellersSalary()
{
	int iSum = 0;
	int i, idx;
	string attr, sCID;
	for (i = 1; i <= 3; i++)
	{
		attr = "Companion" + i;
		if (!CheckAttribute(PChar, "CompanionTravel." + attr + ".ID")) continue;
		sCID = PChar.CompanionTravel.(attr).ID;
		idx = GetCharacterIndex(sCID);
		if (idx < 0) continue;
		if (CheckAttribute(&characters[idx], "CompanionTravel.OutfitTravel")) continue; // ещё снаряжается, корабля/команды нет - не платим
		if (!GetRemovable(&characters[idx])) continue;
		iSum += GetSalaryForShip(&characters[idx]);
	}
	return iSum;
}

// LEO: вернуть исходное диалоговое состояние компаньона (сохранённое в SetTraveller). Зовётся при возврате
//   в эскадру ДО удаления класса CompanionTravel (Saved* лежат там). Иначе нода залипает на "на задании".
void CompanionTravel_RestoreDialog(ref sld)
{
	if (CheckAttribute(sld, "CompanionTravel.SavedDlgFile")) sld.dialog.filename = sld.CompanionTravel.SavedDlgFile;
	if (CheckAttribute(sld, "CompanionTravel.SavedDeck"))    sld.DeckDialogNode  = sld.CompanionTravel.SavedDeck;
	else DeleteAttribute(sld, "DeckDialogNode");
	// LEO: каютный клон (PlaceCompanionCloneNearMChr) стартует на СКОПИРОВАННОМ dialog.currentnode, а НЕ на
	//   DeckDialogNode - поэтому обязательно вернуть исходную старт-ноду, иначе клон открывается на залипшей
	//   "CompanionTravel_OnMission" (баг живого возврата: реальный чинится, но старт-нода клона остаётся).
	//   currentnode НЕ удаляем (ставим валидное) - RPGUtilite.c:3412 читает его без проверки при копире клона -> краш.
	if (CheckAttribute(sld, "CompanionTravel.SavedNode"))      sld.dialog.currentnode = sld.CompanionTravel.SavedNode;
	else if (CheckAttribute(sld, "CompanionTravel.SavedDeck")) sld.dialog.currentnode = sld.CompanionTravel.SavedDeck;
	else sld.dialog.currentnode = "hired";
	sld.dialog.TempNode = "hired";
}

// LEO: компаньон встретил нас в море - принимаем обратно в эскадру. Логика зеркалит узел диалога
//   CompanionTravel_ToSquadron (Enc_Officer_dialog.c), но без разговора - встреча на глобалке.
//   Возвращает true, если принят; false - если слота нет (тогда вызывающий шлёт его ждать в порт).
bool CompanionTravel_ReturnToSquadron(string sID)
{
	if (GetCompanionQuantity(PChar) >= COMPANION_MAX) return false;
	ref sld = CharacterFromID(sID);
	string sCompanion = CompanionTravel_FindSlotKey(sID);

	CompanionTravel_RestoreDialog(sld); // вернуть нормальную ноду (до удаления CompanionTravel c Saved*)
	CompanionTravel_DeleteSpecialShipAttributes(sld);
	if (CheckAttribute(sld, "CompanionTravel.GroupID")) Group_DeleteGroup(sld.CompanionTravel.GroupID);
	SetCompanionIndex(PChar, -1, sti(sld.index));
	PChar.CompanionTravel = sti(PChar.CompanionTravel) - 1;

	AddQuestRecord("CompanionTravel", "2");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'"); // Voc = предложный ("на бриге"), не Dat ("бригу")
	if (GetAttrValue(PChar, "CompanionTravel") == 0) CloseQuestHeader("CompanionTravel");

	CompanionTravel_SettleTrade(sld);        // LEO: ВСТРЕЧА - живой расчёт по торговому рейсу (ДО стирания CompanionTravel)
	DeleteAttribute(sld, "CompanionTravel");
	DeleteAttribute(sld, "Tasks.Clone");
	if (sCompanion != "") DeleteAttribute(PChar, "CompanionTravel." + sCompanion);
	CompanionTravel_TryPayStorageDebt(true); // LEO: ВСТРЕЧА (возврат в эскадру) - списываем долг за склад с ГГ
	return true;
}

// LEO: встреча возвращающегося компаньона состоялась (энкаунтер догнал игрока на глобалке).
//   Вызывается из Map_WarriorEnd (worldmap_encgen.c) вместо боя. Есть слот - принимаем с голосом,
//   нет - шлём ждать в порт, откуда пришёл.
void CompanionTravel_OnMeetingDone(string sID)
{
	ref sld = CharacterFromID(sID);
	string sHome = "";
	if (CheckAttribute(sld, "CompanionTravel.ToColonyID")) sHome = sld.CompanionTravel.ToColonyID;
	DeleteAttribute(sld, "CompanionTravel.Meeting");
	// LEO: встреча состоялась - СНИМАЕМ квест-энкаунтер возврата (quest.chrID, сам не исчезнет). Иначе висячая
	//   запись worldMap.encounters плодит ФАНТОМ компаньона через Direct Sail + дубль портрета в эскадре.
	Map_ReleaseQuestEncounter(sID);
	DeleteAttribute(sld, "mapEnc");

	if (CompanionTravel_ReturnToSquadron(sID))
	{
		PlayVoice("interface\"+LanguageGetLanguage()+"_EvShip1.wav"); // LEO: "У нас попутчики, кэп"
		return;
	}
	// LEO: слота нет - разворачиваем и ЖИВЫМ рейсом отправляем обратно ждать в порт, откуда изначально шёл
	//   (мы на глобалке - встреча только что была тут же). Task снимаем: это уже не склад, а просто "дойти и
	//   ждать" - иначе по прибытии повторно сработала бы складская работа/журнал. Прибытие по времени (счётчик
	//   Days) -> OnOutboundArrive/AbstractArrive (ветка "не склад") -> встанет ждать в порту.
	string sCompanion = CompanionTravel_FindSlotKey(sID);
	DeleteAttribute(sld, "CompanionTravel.Phase");
	DeleteAttribute(sld, "CompanionTravel.Task");
	AddQuestRecord("CompanionTravel", "3");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
	AddQuestUserData("CompanionTravel", "sReason", "нашёл нас в море, но места в эскадре нет - уходит ждать нас в " + XI_ConvertString(CompanionTravel_ColonyTextId(sHome, "Voc")));
	int iBack = makeint(GetDistanceToColony2D(sHome) / 300 + 1.0);
	if (iBack < 1) iBack = 1;
	if (sCompanion != "") PChar.CompanionTravel.(sCompanion).Days = iBack;
	CompanionTravel_LaunchOutbound(sld, iBack); // sld.CompanionTravel.ToColonyID == sHome -> плывёт туда живым энкаунтером
}

// LEO: второе плечо пройдено - компаньон нас нашёл. Куда он денется, зависит от обстановки:
//   нет свободного слота в эскадре -> уходит ждать в город с нашим складом;
//   мы не на глобалке (сидим в городе)  -> приходит в ЭТОТ порт и ждёт с меткой над головой;
//   мы на глобалке -> встреча живым энкаунтером (см. CompanionTravel_SpawnMeetEncounter).
void CompanionTravel_Meet(string sCompanion, string sID)
{
	ref sld = CharacterFromID(sID);
	string sHome = "";
	if (CheckAttribute(sld, "CompanionTravel.ToColonyID")) sHome = sld.CompanionTravel.ToColonyID;

	AddQuestRecord("CompanionTravel", "3");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(sld));
	AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].Basename + "Voc")) + " '" + sld.Ship.name + "'");
	DeleteAttribute(sld, "CompanionTravel.Phase");

	if (GetCompanionQuantity(PChar) >= COMPANION_MAX)
	{
		AddQuestUserData("CompanionTravel", "sReason", "вернул"+ NPCharSexPhrase(sld,"ся","ась") +", но места в эскадре нет - ждёт нас в " + XI_ConvertString(CompanionTravel_ColonyTextId(sHome, "Voc")));
		CompanionTravel_SetCompanionToColony(sHome, "CompanionTravel_"+sCompanion, sID);
		return;
	}

	if (!IsEntity(worldMap))
	{
		int iCol = -1;
		if (CheckAttribute(&loadedLocation, "fastreload")) iCol = FindColony(loadedLocation.fastreload);
		if (iCol >= 0)
		{
			sld.CompanionTravel.QuestMark = true; // жёлтый "?" над ним - чтобы игрок заметил
			AddQuestUserData("CompanionTravel", "sReason", "вернул"+ NPCharSexPhrase(sld,"ся","ась") +" и ждёт нас в " + XI_ConvertString(CompanionTravel_ColonyTextId(colonies[iCol].id, "Voc")));
			CompanionTravel_SetCompanionToColony(colonies[iCol].id, "CompanionTravel_"+sCompanion, sID);
			return;
		}
	}

	// LEO: заглушка до движкового follow - пока ждёт в порту (см. CompanionTravel_SpawnMeetEncounter).
	//   Когда класс будет готов, здесь снова будет живая встреча в море, а этот текст сменится.
	AddQuestUserData("CompanionTravel", "sReason", "вернул"+ NPCharSexPhrase(sld,"ся","ась") +" и ждёт нас в " + XI_ConvertString(CompanionTravel_ColonyTextId(sHome, "Voc")));
	CompanionTravel_SpawnMeetEncounter(sCompanion, sID, sHome);
}

void CompanionTravel_DeleteCompanion(string sID, string sCompanion, bool WaitInColony)
{
	ref rTraveller = CharacterFromID(sID);
	// LEO: рейс сорвался - зарезервированный под погрузку товар со склада никуда не уезжал,
	//   возвращаем его в оборот ДО того, как затрём манифест, иначе он исчезнет из мира.
	if (CheckAttribute(rTraveller, "CompanionTravel.ToColonyID"))
	{
		CompanionTravel_ReleaseReserve(rTraveller, rTraveller.CompanionTravel.ToColonyID);
	}
	DeleteAttribute(rTraveller, "CompanionTravel");
	rTraveller.LifeDay = 0;
	DeleteAttribute(PChar, "CompanionTravel."+sCompanion);
	PChar.CompanionTravel = sti(PChar.CompanionTravel) - 1; // Тут счетчик уменьшаем
	AddQuestRecord("CompanionTravel", "3");
	AddQuestUserData("CompanionTravel", "sCapName", GetFullName(rTraveller)); 
	AddQuestUserData("CompanionTravel", "sShipInfo", GetStrSmallRegister(XI_ConvertString(RealShips[sti(rTraveller.Ship.Type)].Basename + "Voc")) + " '" + rTraveller.Ship.name + "'");
	string strReason; 
	if(WaitInColony)
	{
		Log_TestInfo("Кэпа группы CompanionTravel_"+sCompanion+GetConvertStrWithReplace("Variable_CompanionTravel_12", "Logs.txt", "#space#", " "));
		strReason = "не дождался вас в колонии и покинул её.";
	}
	else
	{
		Log_TestInfo("Судно кэпа группы CompanionTravel_"+sCompanion+GetConvertStrWithReplace("Variable_CompanionTravel_13", "Logs.txt", "#space#", " "));
		strReason = "не добрался до места встречи."; // LEO: была лишняя закрывающая скобка
	}
	Log_Info(XI_ConvertString(RealShips[sti(rTraveller.Ship.Type)].Basename)+" '"+rTraveller.Ship.name+"' " + strReason); 
	AddQuestUserData("CompanionTravel", "sReason", strReason);
	if(GetAttrValue(PChar, "CompanionTravel") == 0) CloseQuestHeader("CompanionTravel");
	Group_DeleteAtEnd("CompanionTravel_"+sCompanion);
}

void CompanionTravel_SetCompanionToColony(string sColony, string sGroupID, string sID) // Поставим компаньона в колонию
{
	if (sColony == "Terks" || sColony == "Dominica" || sColony == "StLucia")
	{
		if (sColony == "Terks")	sIsland = "Terks";
		if (sColony == "StLucia")	sIsland = "StLucia";
		if (sColony == "Dominica")	sIsland = "Dominica";	//это можно удалить, если добавить Люсию в списко колоний
	}
	else string sIsland = colonies[FindColony(sColony)].Island;
	Log_TestInfo(GetConvertStrWithReplace("Variable_CompanionTravel_14", "Logs.txt", "#space#", " ") + sID + GetConvertStrWithReplace("Variable_CompanionTravel_15", "Logs.txt", "#space#", " ") + sGroupID + GetConvertStrWithReplace("Variable_CompanionTravel_16", "Logs.txt", "#space#", " ") + sColony + " (" + sIsland + ")");
	ref sld = CharacterFromID(sID);
	// LEO: страховка - встал ждать в порту, активного морского энкаунтера быть не должно. Снимаем висячий
	//   квест-энкаунтер и метку mapEnc, иначе Direct Sail спавнит фантом-дубль корабля на локалке.
	Map_ReleaseQuestEncounter(sID);
	DeleteAttribute(sld, "mapEnc");
	sld.CompanionTravel.WaitOnColony = true;
	sld.DeckDialogNode = "hired";
	Group_FindOrCreateGroup(sGroupID);
	Group_SetType(sGroupID, "pirate");
	Group_AddCharacter(sGroupID, sID);
	Group_SetGroupCommander(sGroupID, sID);
	Group_SetTaskNone(sGroupID);
	if(sIsland != "Barbados")
		Group_SetAddress(sGroupID, sIsland, "Quest_Ships", "reload_fort1_siege");
	else
		Group_SetAddress(sGroupID, sIsland, "Quest_Ships", "reload_fort1__siege");
	Group_LockTask(sGroupID);
}

float CompanionTravel_CalculateSinkChance(ref _NPC, String _sColony)
{
	Log_TestInfo("=============== Рассчитываем шанс потопления ===============");

	float fChance = 5;
	int iHPPercent = GetHullPercent(_NPC);
	int iSPPercent = GetSailPercent(_NPC);
	int iCrew = sti(_NPC.Ship.Crew.Quantity);
	// LEO: убрана мёртвая переменная iRealShipOptimalCrew = RealShips[...].SailorCrew - нигде не читалась,
	//   а у части типов кораблей атрибута SailorCrew нет => рантайм-ошибка "missed attribute: sailorcrew".
	int iSailSkill = GetSummonSkillFromNameSimple(_NPC, SKILL_SAILING);
	int iShipClass = GetCharacterShipClass(_NPC);
	int iNeedSailSkill = GetShipClassNavySkill(iShipClass);
	int iDays = makeint(GetDistanceToColony2D(_sColony)/100); // Сколько дней идти до колонии

	Log_TestInfo(GetConvertStrWithReplace("Variable_CompanionTravel_17", "Logs.txt", "#space#", " ")+fChance);
	if(iHPPercent < 50) fChance = fChance+(50-iHPPercent)*1.5; // Проценты потопления за состояние корпуса
	Log_TestInfo("База + учёт корпуса - "+fChance);

	if(iSPPercent < 50) fChance = fChance+(50-iSPPercent)*0.5; // За состояние парусов
	Log_TestInfo("База + учёт корпуса + учёт парусов - "+fChance);

	if(iSailSkill < iNeedSailSkill) fChance = fChance+(iNeedSailSkill-iSailSkill)*0.8; // Учитываем скилл навигации
	Log_TestInfo("База + учёт корпуса + учёт парусов + учёт навы - "+fChance);

	if(iDays > 7) fChance = fChance*((iDays-7)*0.1+1);
	Log_TestInfo("( База + учёт корпуса + учёт парусов + учёт навы ) * учёт дней - "+fChance);
	Log_TestInfo("=============== Шанс потопления рассчитан. Шанс - "+fChance+" ===============");

	return fChance;
}

void CompanionTravel_SetSpecialShipAttributes(ref _NPChar)
{
	_NPChar.Abordage.Enable = false;
	_NPChar.DontCheckFlag = true;
	_NPChar.AlwaysFriend        = true;
	_NPChar.ShipEnemyDisable  = true;
}

void CompanionTravel_DeleteSpecialShipAttributes(ref _NPChar)
{
	DeleteAttribute(_NPChar, "Abordage.Enable");
	DeleteAttribute(_NPChar, "DontCheckFlag");
	DeleteAttribute(_NPChar, "AlwaysFriend");
	DeleteAttribute(_NPChar, "ShipEnemyDisable");
}

void CompanionTravel_SetExperienceToTraveller(ref _NPC)
{
	int iRank = sti(_NPC.Rank);
	int iExp = (rand(1)+1)+(iRank/2);
	AddCharacterExpToSkill(_NPC, "Leadership", iExp);
	AddCharacterExpToSkill(_NPC, "Fortune", iExp);
	AddCharacterExpToSkill(_NPC, "Sailing", iExp);
	AddCharacterExpToSkill(_NPC, "Accuracy", iExp/2);
	AddCharacterExpToSkill(_NPC, "Cannons", iExp/2);
}

void CompanionTravel_SetRandomEvent(ref _NPC) // Просто случайные события для компаньона
{
	int iRand = rand(4);
	if (iRand != 0 && iRand != 2) return; // 1, 3, 4 - без происшествий
	int iShipHP = sti(_NPC.Ship.HP);
	int iShipHPMax = sti(RealShips[sti(_NPC.Ship.Type)].hp);
	int iCrew = GetCrewQuantity(_NPC);
	int iMinCrew = sti(RealShips[sti(_NPC.Ship.Type)].mincrew);

	switch(iRand)
	{
		case 0: // Покоцался корпус
			if(iShipHP <= iShipHPMax/10) break;
			_NPC.Ship.HP = sti(_NPC.Ship.HP) - (iShipHPMax/30);
		break;

		case 2: // Минусуем команду
			if(iCrew <= iMinCrew) break;
			_NPC.Ship.Crew.Quantity = iCrew-(rand(4)+2);
		break;
	}
}
