//=====================================================================================================================================
// Неустойка торговой гильдии (кидалово фрахта ИГРОКОМ).
// Игрок присвоил фрахтовый груз (провалил доставку и не сдал её за 3 дня после провала) -> гильдия:
//  - вносит в чёрные списки (фрахты недоступны у ВСЕХ лавочников, пока висит долг, но не дольше 30-60 дней),
//  - добивается награды за голову у нации торговца (нац-репутация опускается до НЗГ-уровня, дельта запоминается),
//  - долг = стоимость товара (базовая цена) + 25% неустойки; гасится в ЛЮБОМ магазине нации деньгами
//    либо возвратом товара + 25% деньгами. Вернул - прощён (блок и награда снимаются сразу).
//  - снял награду у дипломата, не вернув долг - на следующие сутки гильдия добивается её снова (суточный чекер).
// Журнал: квест GuildDebt ("Неустойка торговой гильдии"). Этап 2: взыскатели гильдии (волны). Этап 3: слово капитану.
//=====================================================================================================================================

// LEO: ПЕРСОНАЛЬНАЯ ЛОЯЛЬНОСТЬ ТОРГОВЦА. Каждый ВЫПОЛНЕННЫЙ у конкретного работодателя фрахт даёт +1% к его будущей
// премии (потолок 60%). Счётчик хранится на pchar по id торговца, читается с кэпом. Всё считается при ОТКРЫТИИ журнала
// заказов / взятии / сдаче - НЕ в покадровом цикле, на перф не влияет.
int Freight_LoyaltyPct(ref trader)
{
	if (!CheckAttribute(trader, "id")) return 0;
	string a = "FreightLoyalty." + trader.id;
	if (!CheckAttribute(pchar, a)) return 0;
	int n = sti(pchar.(a));
	if (n > 60) n = 60;   // потолок надбавки 60%
	return n;
}

// +1 к счётчику лояльности ВЫДАВШЕГО работодателя (GiveTraderID) при выполнении фрахта. Выше 60 не растим (потолок).
void Freight_AddLoyalty()
{
	if (!CheckAttribute(pchar, "CargoQuest.GiveTraderID")) return;
	string a = "FreightLoyalty." + pchar.CargoQuest.GiveTraderID;
	int n = 0;
	if (CheckAttribute(pchar, a)) n = sti(pchar.(a));
	if (n < 60) pchar.(a) = n + 1;
}

// LEO: модификатор ШАНСА КИДАЛОВА (%), складывается с базой ролла (контрафрахт / подкуп клерка / барыга).
// Плохая репутация повышает шанс, удача снижает. rep 100 -> +0; rep 0 (кровавый убийца) -> +25.
// Каждое очко Удачи выше 3 (базовый минимум) -> -5%. Может быть отрицательным - итоговый порог клампится сравнением.
int Freight_ScamChanceMod()
{
	int rep = sti(pchar.reputation);
	if (rep < 0)   rep = 0;
	if (rep > 100) rep = 100;
	int mod = 25 * (100 - rep) / 100;                 // 0..+25 по репутации
	int luck = GetCharacterSPECIALSimple(pchar, SPECIAL_L);
	if (luck > 3) mod = mod - (luck - 3) * 5;          // -5% за очко Удачи выше 3
	return mod;
}

// навесить/перенавесить награду за голову: опустить отношение нации до НЗГ-уровня (порог ОЗГ: -10, см. bountyhunters.c),
// дельту КОПИМ в GuildDebt.RepPenalty - возвращаем ровно её, чужие изменения репутации не трогаем
void GuildDebt_ApplyBounty()
{
	int iNation = sti(pchar.GuildDebt.nation);
	int iCur = ChangeCharacterNationReputation(pchar, iNation, 0);
	int iDlt = 5;                          // всегда хоть небольшой штраф
	if (iCur > -15) iDlt = iCur + 15;      // опускаем до -15: гарантированное НЗГ
	ChangeCharacterNationReputation(pchar, iNation, -iDlt);
	pchar.GuildDebt.RepPenalty = sti(pchar.GuildDebt.RepPenalty) + iDlt;
}

// вернуть невозвращённую дельту нац-репутации (погашение долга / слово капитану взыскателей)
void GuildDebt_RestoreBounty()
{
	if (!CheckAttribute(pchar, "GuildDebt.RepPenalty")) return;
	int iDlt = sti(pchar.GuildDebt.RepPenalty);
	if (iDlt > 0) ChangeCharacterNationReputation(pchar, sti(pchar.GuildDebt.nation), iDlt);
	pchar.GuildDebt.RepPenalty = 0;
}

// СТАРТ долга: реакция GuildDebt_Check (+3 дня после провала фрахта), если груз так и не сдан
void GuildDebt_Start()
{
	if (CheckAttribute(pchar, "GuildDebt")) return;                                  // долг уже висит
	if (!CheckAttribute(pchar, "CargoQuest.GiveTraderID")) return;                   // фрахта нет
	if (!CheckQuestAttribute("generate_trade_quest_progress", "failed")) return;     // успел сдать - молодец

	int g    = sti(pchar.CargoQuest.iTradeGoods);
	int qty  = sti(pchar.CargoQuest.iQuantityGoods);
	int iCost = makeint(qty * stf(Goods[g].Cost)); // стоимость товара по базовой цене (не выплата за фрахт!)

	// ЛАЗЕЙКА 2: контрабандный фрахт не довезён - смуглеры в гильдию не бегут. Вместо долга -
	// репутация у КОНТРАБАНДИСТОВ В НОЛЬ (кинул своих - больше не доверяют) + город перестаёт давать контра-фрахты. Груз остаётся игроку.
	// (Донос барыги по контра-фрахту - отдельный отложенный путь Freight_FenceBetrayDebt, он открывает уже обычный долг гильдии.)
	if (sti(pchar.CargoQuest.iOrderType) == 5)
	{
		// ДВА последствия: (1) репутация КОНТРАБАНДИСТОВ В НОЛЬ (кинул своих - доверие потеряно; бартер/сделки у смуглеров,
		// гейт relation>5, законно отваливаются - это осознанное наказание, восстанавливать репу надо отдельным путём);
		// (2) ПОЛНЫЙ блок конкретного магазина-выдатчика + неустойка. Разблок магазина репутацию контрабандистов НЕ восстанавливает.
		pchar.questTemp.Contraband.relation = 0;
		string sCnCity = characters[GetCharacterIndex(pchar.CargoQuest.GiveTraderID)].city;
		// храним СТОИМОСТЬ кинутого товара (а не просто факт): база для разблокировки города через связного в таверне (плата = стоимость +50%).
		// Город перестаёт давать контра-фрахты, т.ч. повторно его не кинуть - перезапись безопасна. CheckAttribute по-прежнему = "город заблокирован".
		pchar.NoContraCity.(sCnCity) = makeint(iCost * 1.5); // НЕУСТОЙКА магазина (стоимость×1.5); тает по мере погашения (доставки сюда / лумп у связного), при 0 блок снят
		pchar.quest.generate_trade_quest_progress = "";
		pchar.quest.generate_trade_quest.over = "yes";
		CloseQuestHeader("DELIVERY_TRADE_QUEST");
		if (CheckAttribute(pchar, "quest.SmuggleFreight"))
		{
			RemoveSmugglersFromShore();
			DeleteAttribute(pchar, "quest.SmuggleFreight");
			DeleteAttribute(pchar, "quest.Contraband.active");
		}
		return;
	}

	// сброс таймер-квестов от ПРЕДЫДУЩЕГО закрытого долга: GuildDebt_Close ставит им over="yes", а поверх over
	// новый SetTimerCondition НЕ взводится (гоча Storm с одноимёнными квестами) - иначе у ВТОРОГО долга суточный
	// чекер/волны мертвы (НЗГ после дипломата не перевешивается). Чистим перед взводом новых.
	DeleteAttribute(pchar, "quest.GuildDebt_Daily");
	DeleteAttribute(pchar, "quest.GuildDebt_Wave");
	DeleteAttribute(pchar, "quest.GuildDebt_WordTimer");

	pchar.GuildDebt.nation     = sti(characters[GetCharacterIndex(pchar.CargoQuest.GiveTraderID)].nation);
	pchar.GuildDebt.sum        = makeint(iCost * 1.25);  // деньгами: товар + 25% неустойки
	pchar.GuildDebt.GoodsType  = g;
	pchar.GuildDebt.GoodsQty   = qty;
	pchar.GuildDebt.GoodsComp  = makeint(iCost * 0.25);  // возврат товаром: доплата 25% деньгами
	pchar.GuildDebt.TraderCity = characters[GetCharacterIndex(pchar.CargoQuest.GiveTraderID)].city;
	pchar.GuildDebt.BlockDays  = 30 + rand(30);          // потолок чёрных списков (вернул - прощён раньше)
	pchar.GuildDebt.RepPenalty = 0;
	// комендантский конвой (тип 7) просрочен: казённые суда ушли из эскадры (потоплены/брошены), вернуть нечем ->
	// долг ТОЛЬКО деньгами = выплата конвоя x3 (покрывает и утраченные казённые суда, а не один лишь груз).
	if (sti(pchar.CargoQuest.iOrderType) == 7)
	{
		pchar.GuildDebt.sum       = makeint(sti(pchar.CargoQuest.iMoney) * 3);
		pchar.GuildDebt.GoodsComp = 0;
		pchar.GuildDebt.NoGoods   = true; // конвой ушёл - товаром не вернуть, гасить звонкой монетой
	}
	// эскорт (губернаторский подтип 2) просрочен: груз был на казённых судах, в руки ГГ не попадал ->
	// долг ТОЛЬКО деньгами (как конвой), а не "за присвоение товара".
	if (CheckAttribute(pchar, "CargoQuest.GovEscortActive"))
	{
		pchar.GuildDebt.sum       = makeint(sti(pchar.CargoQuest.iMoney) * 3);
		pchar.GuildDebt.GoodsComp = 0;
		pchar.GuildDebt.NoGoods   = true;
	}
	// переносим «барыга донесёт» с фрахта на долг: если потом сбыть барыге груз этого долга, исход уже предрешён (катался при взятии)
	if (CheckAttribute(pchar, "CargoQuest.FenceBetray")) pchar.GuildDebt.FenceBetray = true;
	if (CheckAttribute(pchar, "CargoQuest.ScamRoll")) pchar.GuildDebt.ScamRoll = stf(pchar.CargoQuest.ScamRoll); // дробь цены барыги - тоже на долг
	SaveCurrentNpcQuestDateParam(pchar, "GuildDebtDate");

	// гильдия списала груз: фрахт закрывается НАСОВСЕМ (сдать его за полцены больше нельзя - только долг)
	pchar.quest.generate_trade_quest_progress = "";
	pchar.quest.generate_trade_quest.over = "yes";
	CloseQuestHeader("DELIVERY_TRADE_QUEST");
	GovSwap_Clear(); // снять подмену (если был губ.фрахт): фрахт провален в долг - метка "(подмена)" больше не нужна
	FortHunt_ClearWatch(); // комендантский: вахты СЖ-записей о каперах-перехватчиках больше не к месту
	FortAgent_Cleanup();   // комендантский: посыльный-перехватчик больше не нужен (фрахт провален - долг)
	FortConvoy_Disband();  // комендантский: казённые барки уходят из эскадры (конвой потерян/брошен; самозащищён - без конвоя no-op)

	// контрабандный фрахт с назначенной бухтой: игрок забил на встречу - сворачиваем лагерь смуглеров
	// (иначе модели грузов висели бы в бухте навсегда; гард по quest.SmuggleFreight не даст закрыть "Сбыт контрабанды")
	if (CheckAttribute(pchar, "quest.SmuggleFreight"))
	{
		RemoveSmugglersFromShore();
		DeleteAttribute(pchar, "quest.SmuggleFreight");
		DeleteAttribute(pchar, "quest.Contraband.active");
	}

	// награда за голову у нации торговца
	GuildDebt_ApplyBounty();

	// суточный чекер: уладил награду у дипломата/фрахтами, не вернув долг - назавтра гильдия добьётся её снова
	SetTimerCondition("GuildDebt_Daily", 0, 0, 1, true);

	// судовой журнал
	ReOpenQuestHeader("GuildDebt");
	if (sti(pchar.CargoQuest.iOrderType) == 7) // конвой потерян: деньгами x3, товара нет
	{
		AddQuestRecord("GuildDebt", "9");
		AddQuestUserData("GuildDebt", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("GuildDebt", "sCity", XI_ConvertString("Colony" + pchar.GuildDebt.TraderCity + "Gen"));
		AddQuestUserData("GuildDebt", "sMoney", FindRussianMoneyString(sti(pchar.GuildDebt.sum)));
	}
	else
	{
		AddQuestRecord("GuildDebt", "1");
		AddQuestUserData("GuildDebt", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("GuildDebt", "sGoodGen", GetGoodsNameAlt(g));
		AddQuestUserData("GuildDebt", "sCity", XI_ConvertString("Colony" + pchar.GuildDebt.TraderCity + "Gen"));
		AddQuestUserData("GuildDebt", "sMoney", FindRussianMoneyString(sti(pchar.GuildDebt.sum)));
	}
	// взыскатели гильдии: первая волна сразу, дальше по чекеру (каждые 3 дня, когда исчезла предыдущая)
	pchar.GuildDebt.Wave = 0;
	GuildDebt_SpawnEnforcers();
	SetTimerCondition("GuildDebt_Wave", 0, 0, 3, true);
}

// суточная проверка: долг висит, слово не дано, а награда за голову снята (дипломат/фрахты подняли репутацию) -
// гильдия снова добивается награды. Чекер перевзводится сам, пока долг активен.
void GuildDebt_DailyCheck()
{
	if (!CheckAttribute(pchar, "GuildDebt")) return; // долг погашен - чекер не перевзводим
	SetTimerCondition("GuildDebt_Daily", 0, 0, 1, true);
	if (CheckAttribute(pchar, "GuildDebt.Word")) return; // дал слово капитану взыскателей - гильдия ждёт (этап 3)
	if (ChangeCharacterNationReputation(pchar, sti(pchar.GuildDebt.nation), 0) > -10)
	{
		GuildDebt_ApplyBounty();
		AddQuestRecord("GuildDebt", "2");
		AddQuestUserData("GuildDebt", "sSex", GetSexPhrase("","а"));
		// уладил награду, не вернув долг - гильдия упрямее: волны взыскателей заново (3 по 3)
		pchar.GuildDebt.Wave = 0;
		GuildDebt_SpawnEnforcers();
		SetTimerCondition("GuildDebt_Wave", 0, 0, 3, true);
	}
}

// === ВЗЫСКАТЕЛИ ГИЛЬДИИ (этап 2) ===
// Волна: 2-3 корабля нации торговца, модель глобалки galeon_red, скорость Map_CreateFastWarrior (coolwarrior, 1.7).
// Капитаны сдаются в каюте (можно дать слово - этап 3); после НАРУШЕННОГО слова - пощады нет (не сдаются, без диалога).
void GuildDebt_SpawnEnforcers()
{
	if (!CheckAttribute(pchar, "GuildDebt")) return;
	if (CheckAttribute(pchar, "GuildDebt.Word")) return;            // дал слово - гильдия ждёт
	if (GetCharacterIndex("GuildEnforcer1") != -1) return;          // прошлая волна ещё в море

	ref sld;
	int i;
	int iNation = sti(pchar.GuildDebt.nation);
	string sCapId = "GuildEnforcer";
	string sGroup = "Sea_" + sCapId + "1";
	int iCount = 2 + rand(1); // 2-3 корабля

	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	for (i = 1; i <= iCount; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_" + NationShortName(iNation) + "_" + (rand(1) + 1), "man", "man", sti(pchar.rank) + 5, iNation, 5, true)); // 5 дней - потом "скрылись от погони"
		SetShipHunter(sld);
		SetFantomParamHunter(sld);
		// атрибут OZG ОСТАВЛЕН намеренно: абордажная команда взыскателей - наёмники-головорезы (модели ОЗГ),
		// а не солдаты в форме. Взыскание долгов - как раз их работа
		SetCaptanModelByEncType(sld, "war");
		sld.AlwaysEnemy = true;
		if (CheckAttribute(pchar, "GuildDebt.WordBroken"))
		{
			sld.DontRansackCaptain = true; // слово нарушено: пощады не будет, никаких разговоров
		}
		else
		{
			sld.DontRansackCaptain = false;
			sld.GuildEnforcer = true; // сдача в каюте: можно дать слово вернуть долг (этап 3)
			sld.NoOfficerHire = true; // LEO: пленного взыскателя гильдии нельзя нанять офицером в трюме
		}
		sld.mapEnc.type = "war";
		sld.mapEnc.Name = GetConvertStrWithReplace("Variable_Treasure_9", "Names.txt", "#space#", " "); // "Взыскатели гильдии"
		sld.hunter = "hunter";
		sld.mapEnc.worldMapShip = "galeon_red"; // фирменная модель взыскателей на глобалке
		Group_AddCharacter(sGroup, sCapId + i);
	}
	Group_SetGroupCommander(sGroup, sCapId + "1");
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Map_CreateFastWarrior("", sCapId + "1", 8); // прыткие - догоняют ГГ

	pchar.GuildDebt.Wave = sti(pchar.GuildDebt.Wave) + 1;
	if (sti(pchar.GuildDebt.Wave) == 1 && !CheckAttribute(pchar, "GuildDebt.WaveRecord"))
	{
		pchar.GuildDebt.WaveRecord = true; // запись о взыскателях - один раз
		AddQuestRecord("GuildDebt", "5");
	}
}

// удалить взыскателей с моря/карты (слово/погашение): нейтралим и убираем
void GuildDebt_RemoveEnforcers()
{
	ref sld;
	int i;
	for (i = 1; i <= 3; i++)
	{
		if (GetCharacterIndex("GuildEnforcer" + i) == -1) continue;
		sld = CharacterFromID("GuildEnforcer" + i);
		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_NEUTRAL);
		DeleteAttribute(sld, "AlwaysEnemy");
		sld.lifeDay = 0; // уйдут при ближайшей чистке фантомов
	}
	Group_DeleteGroup("Sea_GuildEnforcer1");
}

// убрать НАЦ-ОЗГ (морских охотников нации) с карты/моря - при слове капитану/погашении долга.
// Иначе игроку пришлось бы их топить, а топка корабля нации снова роняет её репу -> бесконечная эскалация НЗГ.
// Гард: чистим ТОЛЬКО если нация уже НЕ враг (репа поднята восстановлением дельты) - чужие, не от долга, ОЗГ не трогаем.
void GuildDebt_RemoveNationHunters(int iNation)
{
	if (ChangeCharacterNationReputation(pchar, iNation, 0) <= -10) return; // нация всё ещё враг по своей причине - её ОЗГ законны
	string sCapId = NationShortName(iNation) + "Hunter0"; // id из SeaHunterCheck: <нация>Hunter01..07
	ref h;
	int i;
	for (i = 1; i <= 7; i++)
	{
		if (GetCharacterIndex(sCapId + i) == -1) continue;
		h = CharacterFromID(sCapId + i);
		SetCharacterRelationBoth(sti(h.index), GetMainCharacterIndex(), RELATION_NEUTRAL);
		DeleteAttribute(h, "AlwaysEnemy");
		h.lifeDay = 0; // уйдут при ближайшей чистке фантомов
	}
	Group_DeleteGroup("Sea_" + sCapId + "1");
}

// чекер волн (каждые 3 дня): следующая волна - только когда исчезла предыдущая (перебита/потоплена/скрылась).
// 3 волны за цикл, затем месяц передышки и цикл заново (для "живущих с НЗГ"). После нарушенного слова - БЕЗ передышек.
void GuildDebt_WaveCheck()
{
	if (!CheckAttribute(pchar, "GuildDebt")) return; // долг погашен - волны не перевзводим
	if (CheckAttribute(pchar, "GuildDebt.Word"))
	{
		SetTimerCondition("GuildDebt_Wave", 0, 0, 3, true); // слово дано - ждём (таймер слова разрулит)
		return;
	}
	if (GetCharacterIndex("GuildEnforcer1") != -1)
	{
		SetTimerCondition("GuildDebt_Wave", 0, 0, 3, true); // прошлая волна ещё в море
		return;
	}
	if (sti(pchar.GuildDebt.Wave) >= 3 && !CheckAttribute(pchar, "GuildDebt.WordBroken"))
	{
		// цикл из 3 волн исчерпан (накладно гонять флот без конца) - гильдия выжидает месяц и начинает заново
		pchar.GuildDebt.Wave = 0;
		SetTimerCondition("GuildDebt_Wave", 0, 0, 30, true);
		return;
	}
	GuildDebt_SpawnEnforcers();
	SetTimerCondition("GuildDebt_Wave", 0, 0, 3, true);
}

// === СЛОВО КАПИТАНУ (этап 3) === (зовётся из каютного диалога взыскателя)
// Дал слово вернуть долг за месяц: эскадра нейтрализуется и уходит, награда за голову отзывается (наша дельта),
// волны на паузе. Не вернул за месяц - слово нарушено: всё заново и БЕЗ пощады.
void GuildDebt_GiveWord()
{
	pchar.GuildDebt.Word = true;
	GuildDebt_RestoreBounty();
	GuildDebt_RemoveEnforcers();
	GuildDebt_RemoveNationHunters(sti(pchar.GuildDebt.nation)); // дал слово - убрать и нац-ОЗГ, чтоб не топить их (топка снова роняет репу)
	SetTimerCondition("GuildDebt_WordTimer", 0, 0, 30, false);
	AddQuestRecord("GuildDebt", "6");
	AddQuestUserData("GuildDebt", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("GuildDebt", "sMoney", FindRussianMoneyString(sti(pchar.GuildDebt.sum)));
}

// месяц после слова: долг так и не возвращён - слово нарушено
void GuildDebt_WordTimerCheck()
{
	if (!CheckAttribute(pchar, "GuildDebt")) return;     // долг погашен - всё в порядке
	if (!CheckAttribute(pchar, "GuildDebt.Word")) return; // слово уже снято иначе
	DeleteAttribute(pchar, "GuildDebt.Word");
	pchar.GuildDebt.WordBroken = true; // пощады не будет: взыскатели не сдаются, волны без передышек
	pchar.GuildDebt.Wave = 0;
	GuildDebt_ApplyBounty();
	AddQuestRecord("GuildDebt", "7");
	AddQuestUserData("GuildDebt", "sSex", GetSexPhrase("","а"));
	GuildDebt_SpawnEnforcers();
	SetTimerCondition("GuildDebt_Wave", 0, 0, 3, true);
}

// блок фрахтов: долг активен и потолок чёрных списков не истёк
bool GuildDebt_IsFreightBlocked()
{
	if (!CheckAttribute(pchar, "GuildDebt")) return false;
	if (GetNpcQuestPastDayWOInit(pchar, "GuildDebtDate") > sti(pchar.GuildDebt.BlockDays)) return false; // сроки вышли (долг остаётся)
	return true;
}

// ПОГАШЕНИЕ долга (зовётся из диалога лавочника ПОСЛЕ списания денег/товара и записи в журнал).
// Вернул - прощён: награда за голову отзывается, чёрные списки снимаются, чекер гаснет.
void GuildDebt_Close()
{
	GuildDebt_RestoreBounty();
	GuildDebt_RemoveEnforcers();
	GuildDebt_RemoveNationHunters(sti(pchar.GuildDebt.nation)); // долг закрыт - убрать и нац-ОЗГ (если нация перестала быть врагом)
	pchar.quest.GuildDebt_Daily.over = "yes";
	pchar.quest.GuildDebt_Wave.over = "yes";
	pchar.quest.GuildDebt_WordTimer.over = "yes";
	CloseQuestHeader("GuildDebt");
	DeleteAttribute(pchar, "GuildDebt");
}
//=====================================================================================================================================
// ЛАЗЕЙКИ КИДАЛОВА ФРАХТА (рисковые альтернативы прямому присвоению груза). Исходы/цены ЗАПЕКАЮТСЯ при взятии фрахта (анти-сейв-скам).
// Ковёрное закрытие лазейки журнал НЕ пишет (ни записи, ни звука) - прячем, не логируем; в отличие от честного погашения у лавочника (запись 3/4).
//  1) Сбыт груза барыге: 30-40% стоимости сразу, долга нет; ~10% барыга донесёт ОТЛОЖЕННО (5-15 дней) -> двойная неустойка (x2.5, NoGoods).
//  2) Контрабандный фрахт - особый случай: смуглеры не бегут в гильдию. Не довёз -> репутация КОНТРАБАНДИСТОВ в НОЛЬ
//     + город больше не даёт контра-фрахтов (разблокировка у связного-контрабандиста в таверне за стоимость+50%). Груз остаётся игроку.
//  3) Подкуп клерка гильдии у АДРЕСАТА: за 45-75% стоимости запись "теряется", долга нет, груз остаётся;
//     12-55% (по репе/удаче) клерк донесёт ОТЛОЖЕННО (5-15 дней) -> долг 2.0.
//  Комендантский конвой (тип 7) - лазеек НЕТ (груз на казённых судах); просрочка = долг деньгами iMoney x3.
//=====================================================================================================================================

// есть ли активный фрахтовый заказ, которым можно сманеврировать (взят, ещё не превратился в долг)
bool Freight_HasActive()
{
	if (CheckAttribute(pchar, "GuildDebt")) return false;                          // долг уже висит - поздно
	if (!CheckAttribute(pchar, "CargoQuest.GiveTraderID")) return false;
	if (!CheckAttribute(pchar, "quest.generate_trade_quest_progress")) return false;
	string st = pchar.quest.generate_trade_quest_progress;
	if (st != "begin" && st != "failed") return false;
	return true;
}

// груз заказа ещё в трюме эскадры?
bool Freight_CargoAboard()
{
	if (!Freight_HasActive()) return false;
	if (sti(pchar.CargoQuest.iOrderType) == 7) return false; // комендантский конвой: груз на казённых судах, не лазейка - только коменданту
	if (CheckAttribute(pchar, "CargoQuest.GovEscortActive")) return false; // LEO: губ.эскорт - груз на казённых судах конвоя, барыге не сбыть (конвой уйдёт с грузом, деньги впустую)
	return (GetSquadronGoods(pchar, sti(pchar.CargoQuest.iTradeGoods)) >= sti(pchar.CargoQuest.iQuantityGoods));
}

// стоимость груза по базовой цене (как считает долг в GuildDebt_Start)
int Freight_CargoValue()
{
	int g = sti(pchar.CargoQuest.iTradeGoods);
	return makeint(sti(pchar.CargoQuest.iQuantityGoods) * stf(Goods[g].Cost));
}

// закрыть фрахт-квест НАЧИСТО, без долга (общая часть лазеек). Груз НЕ трогаем - вызывающий сам решает
void Freight_CloseQuestNoDebt()
{
	pchar.quest.generate_trade_quest_progress = "";
	pchar.quest.generate_trade_quest.over = "yes";
	CloseQuestHeader("DELIVERY_TRADE_QUEST");
	GovSwap_Clear(); // снять подмену (если был губ.фрахт): закрыли лазейкой - метка "(подмена)" должна пропасть
	FortHunt_ClearWatch();
	if (CheckAttribute(pchar, "quest.SmuggleFreight"))
	{
		RemoveSmugglersFromShore();
		DeleteAttribute(pchar, "quest.SmuggleFreight");
		DeleteAttribute(pchar, "quest.Contraband.active");
	}
}

// --- ЛАЗЕЙКА 1: барыга ---
// сумма выплаты барыги (30-40% стоимости груза); дробь ScamRoll запечена при взятии фрахта - цену не сейв-скамить
int Freight_FencePrepare()
{
	float u = 0.5;
	if (CheckAttribute(pchar, "CargoQuest.ScamRoll")) u = stf(pchar.CargoQuest.ScamRoll);
	int iPay = makeint(Freight_CargoValue() * (0.30 + 0.10 * u)); // 30-40%
	if (iPay < 1) iPay = 1;
	pchar.FreightFencePay = iPay;
	return iPay;
}

// сбыт груза барыге: получаем деньги, груз уходит, фрахт закрыт. ~12% барыга СДАЁТ гильдии - но ОТЛОЖЕННО (5-15 дней, как клерк) -> двойная неустойка
void Freight_FenceSell()
{
	int g   = sti(pchar.CargoQuest.iTradeGoods);
	int qty = sti(pchar.CargoQuest.iQuantityGoods);
	int iPay = sti(pchar.FreightFencePay);
	if (iPay < 1) iPay = Freight_FencePrepare();

	RemoveCharacterGoods(pchar, g, qty);     // груз сбыт
	AddMoneyToCharacter(pchar, iPay);
	DeleteAttribute(pchar, "FreightFencePay");

	// барыга донесёт - но НЕ сразу: внешне сделка чиста (фрахт закрыт), долг прилетит через 5-15 дней (двойной).
	// Исход (~10%) предрешён ещё ПРИ ВЗЯТИИ фрахта (CargoQuest.FenceBetray) - сейв-лоадом сделки не переиграть.
	// Снимок данных - CargoQuest может перезаписать новый заказ. Квест закрываем в любом случае - игрок не знает, заложат ли его.
	if (CheckAttribute(pchar, "CargoQuest.FenceBetray"))
	{
		int gIdx = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
		pchar.FreightFenceBetraySnap.nation = sti(characters[gIdx].nation);
		pchar.FreightFenceBetraySnap.goods  = g;
		pchar.FreightFenceBetraySnap.qty    = qty;
		pchar.FreightFenceBetraySnap.city   = characters[gIdx].city;
		SetTimerCondition("FreightFenceBetray", 0, 0, 5 + rand(10), false);
	}
	Freight_CloseQuestNoDebt();
}

// --- ЛАЗЕЙКА 1b: сбыт барыге УЖЕ ПРОСРОЧЕННОГО фрахта (долг гильдии активен, кредиторы гоняются, груз ещё в трюме) ---
// груз долга ещё в трюме эскадры (ВЕСЬ, по накладной)?
bool Freight_DebtCargoAboard()
{
	if (!CheckAttribute(pchar, "GuildDebt")) return false;
	if (CheckAttribute(pchar, "GuildDebt.NoGoods")) return false; // товара не вернуть (сбыт барыге / конвой ушёл) - сбывать нечего, только деньги
	if (!CheckAttribute(pchar, "GuildDebt.GoodsQty")) return false;
	int g = sti(pchar.GuildDebt.GoodsType);
	return (GetSquadronGoods(pchar, g) >= sti(pchar.GuildDebt.GoodsQty));
}

// --- ЛАЗЕЙКА 2 (искупление): разблокировка кинутого контра-города через связного-контрабандиста в таверне ---
// есть ли заблокированные магазины, ещё НЕ оплаченные у связного (оплаченные ждут снятия - в списке не показываем)?
bool ContraBlock_Any()
{
	return (ContraBlock_CityByIndex(0) != "");
}

// id города по индексу среди заблокированных и ЕЩЁ НЕ оплаченных ("" если за пределами). Оплаченные (NoUnblockWait) пропускаем.
string ContraBlock_CityByIndex(int idx)
{
	if (!CheckAttribute(pchar, "NoContraCity")) return "";
	aref ar;
	makearef(ar, pchar.NoContraCity);
	int n = GetAttributesNum(ar);
	int k = 0;
	int i;
	aref ch;
	string sC;
	for (i = 0; i < n; i++)
	{
		ch = GetAttributeN(ar, i);
		sC = GetAttributeName(ch);
		if (CheckAttribute(pchar, "NoUnblockWait." + sC)) continue; // оплачен у связного, ждёт снятия - в списке не нужен
		if (k == idx) return sC;
		k = k + 1;
	}
	return "";
}

// плата за разблокировку (лумп у связного) = ОСТАТОК неустойки (хранится уже как стоимость×1.5, уменьшается погашением через доставки)
int ContraBlock_Fee(string sCity)
{
	if (!CheckAttribute(pchar, "NoContraCity." + sCity)) return 0;
	return sti(pchar.NoContraCity.(sCity));
}

// оплатить разблокировку у связного: плату списываем, но блок снимется НЕ сразу - через 1-3 дня (связной налаживает связи).
// Помечаем NoUnblockWait (магазин ещё блокирован, но из списка связного и из удержания при доставке выпадает); снимет таймер ContraUnblock_Done.
// Репутацию контрабандистов НЕ трогаем - наказание чисто магазинное.
void ContraBlock_Unblock(string sCity)
{
	AddMoneyToCharacter(pchar, -ContraBlock_Fee(sCity));
	pchar.NoUnblockWait.(sCity) = true;
	// квест-таймер одноразовый: после прошлого срабатывания на нём остаётся over="yes", поверх которого
	// новый SetTimerCondition НЕ взводится (гоча Storm) - повторная оплата другого города не разблокировалась бы.
	// Чистим перед взводом. Города, уже ждущие разблокировки, лежат в NoUnblockWait (не в квесте) - не теряются.
	DeleteAttribute(pchar, "quest.ContraUnblock_Done");
	SetTimerCondition("ContraUnblock_Done", 0, 0, 1 + rand(2), false); // 1-3 дня
}

// таймер: связной «порешал вопросы» - снимаем блок со всех оплаченных магазинов разом
void ContraBlock_FinishUnblock()
{
	if (!CheckAttribute(pchar, "NoUnblockWait")) return;
	aref ar;
	makearef(ar, pchar.NoUnblockWait);
	int n = GetAttributesNum(ar);
	int i;
	aref ch;
	string sC;
	for (i = 0; i < n; i++)
	{
		ch = GetAttributeN(ar, i);
		sC = GetAttributeName(ch);
		DeleteAttribute(pchar, "NoContraCity." + sC); // блок магазина снят
	}
	DeleteAttribute(pchar, "NoUnblockWait");
	if (LanguageGetLanguage() == "russian") Log_Info("Контрабандисты порешали вопросы.");
	else Log_Info("The smugglers have sorted things out.");
}

// груз долга есть, но НЕ весь (часть утрачена - крысы сожрали / продал / потерял): сбыть нельзя, нужен полный по накладной
bool Freight_DebtCargoPartial()
{
	if (!CheckAttribute(pchar, "GuildDebt")) return false;
	if (CheckAttribute(pchar, "GuildDebt.NoGoods")) return false; // долг деньгами-только - груза нет в принципе
	if (!CheckAttribute(pchar, "GuildDebt.GoodsQty")) return false;
	int g = sti(pchar.GuildDebt.GoodsType);
	int have = GetSquadronGoods(pchar, g);
	int need = sti(pchar.GuildDebt.GoodsQty);
	// ДВОЙНОЕ сравнение внутри return(...) Storm-парсер не переваривает ("No matching ')'") - разносим на отдельные if
	if (have <= 0) return false;    // груза нет совсем
	if (have >= need) return false; // груз ВЕСЬ - это уже Freight_DebtCargoAboard
	return true;                    // есть, но не весь
}

// сумма выплаты барыги за груз долга (30-40% стоимости груза); дробь ScamRoll перенесена на долг при его открытии
int Freight_FenceDebtPrepare()
{
	int g    = sti(pchar.GuildDebt.GoodsType);
	int iVal = makeint(sti(pchar.GuildDebt.GoodsQty) * stf(Goods[g].Cost));
	float u = 0.5;
	if (CheckAttribute(pchar, "GuildDebt.ScamRoll")) u = stf(pchar.GuildDebt.ScamRoll);
	int iPay = makeint(iVal * (0.30 + 0.10 * u)); // 30-40%
	if (iPay < 1) iPay = 1;
	pchar.FreightFencePay = iPay;
	return iPay;
}

// сбыт барыге груза просроченного фрахта (концы в воду): долг внешне ЗАКРЫТ сразу (кредиторы отстали, НЗГ снят).
// ~12% барыга всё же донесёт - но ОТЛОЖЕННО (5-15 дней): долг вернётся ДВОЙНЫМ. 88% - чисто.
void Freight_FenceSellDebt()
{
	int g    = sti(pchar.GuildDebt.GoodsType);
	int qty  = sti(pchar.GuildDebt.GoodsQty);
	int iPay = sti(pchar.FreightFencePay);
	if (iPay < 1) iPay = Freight_FenceDebtPrepare();

	RemoveCharacterGoods(pchar, g, qty); // груз сбыт
	AddMoneyToCharacter(pchar, iPay);
	DeleteAttribute(pchar, "FreightFencePay");

	// снимок данных долга на случай отложенного доноса (GuildDebt_Close ниже удалит сам долг).
	// Исход барыги предрешён при взятии фрахта и перенесён на долг (GuildDebt.FenceBetray) - не сейв-скамится.
	if (CheckAttribute(pchar, "GuildDebt.FenceBetray"))
	{
		pchar.FreightFenceBetraySnap.nation = sti(pchar.GuildDebt.nation);
		pchar.FreightFenceBetraySnap.goods  = g;
		pchar.FreightFenceBetraySnap.qty    = qty;
		pchar.FreightFenceBetraySnap.city   = pchar.GuildDebt.TraderCity;
		SetTimerCondition("FreightFenceBetray", 0, 0, 5 + rand(10), false);
	}
	GuildDebt_Close(); // долг улажен (внешне); если заложил - вернётся отложенно двойным
}

// отложенный донос барыги (5-15 дней после сбыта): открываем ДВОЙНОЙ долг гильдии по снимку (груз сбыт - товаром не вернуть)
void Freight_FenceBetrayDebt()
{
	if (!CheckAttribute(pchar, "FreightFenceBetraySnap")) return;
	if (CheckAttribute(pchar, "GuildDebt")) { DeleteAttribute(pchar, "FreightFenceBetraySnap"); return; } // долг уже висит

	int g    = sti(pchar.FreightFenceBetraySnap.goods);
	int qty  = sti(pchar.FreightFenceBetraySnap.qty);
	int iCost = makeint(qty * stf(Goods[g].Cost));

	pchar.GuildDebt.nation     = sti(pchar.FreightFenceBetraySnap.nation);
	pchar.GuildDebt.sum        = makeint(iCost * 2.5);  // барыга сдал - двойная неустойка
	pchar.GuildDebt.GoodsType  = g;
	pchar.GuildDebt.GoodsQty   = qty;
	pchar.GuildDebt.GoodsComp  = makeint(iCost * 1.5);
	pchar.GuildDebt.TraderCity = pchar.FreightFenceBetraySnap.city;
	pchar.GuildDebt.BlockDays  = 30 + rand(30);
	pchar.GuildDebt.RepPenalty = 0;
	pchar.GuildDebt.NoGoods    = true; // груз сбыт барыге - вернуть товаром нечем, гасить только деньгами (ласт колл)
	SaveCurrentNpcQuestDateParam(pchar, "GuildDebtDate");

	GuildDebt_ApplyBounty();
	SetTimerCondition("GuildDebt_Daily", 0, 0, 1, true);

	ReOpenQuestHeader("GuildDebt");
	AddQuestRecord("GuildDebt", "8"); // ласт колл: барыга донёс, долг вдвое, товара нет - только звонкой монетой
	AddQuestUserData("GuildDebt", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("GuildDebt", "sMoney", FindRussianMoneyString(sti(pchar.GuildDebt.sum)));

	pchar.GuildDebt.Wave = 0;
	GuildDebt_SpawnEnforcers();
	SetTimerCondition("GuildDebt_Wave", 0, 0, 3, true);

	DeleteAttribute(pchar, "FreightFenceBetraySnap");
}

// --- ЛАЗЕЙКА 3: подкуп клерка гильдии ---
// цена подкупа = 45-75% стоимости груза. Дробь ScamRoll (0..1) ЗАПЕКАЕТСЯ при взятии фрахта,
// чтобы цену нельзя было сейв-скамить у клерка. Запасной середняк 0.5 - для старых сейвов без флага.
// Дорого не от жадности: при удачном подкупе ВЕСЬ груз остаётся игроку, то есть лазейка платит
// сотню процентов его стоимости. При старых 20-50% выгода была вчетверо выше сбыта барыге.
// Считается заново на каждый запрос: и груз, и ScamRoll на время фрахта неизменны, поэтому цена
// повторяется точь-в-точь, а хранить её на pchar не нужно (иначе висла бы в сейве при отказе).
int Freight_BribePrepare()
{
	float u = 0.5;
	if (CheckAttribute(pchar, "CargoQuest.ScamRoll")) u = stf(pchar.CargoQuest.ScamRoll);
	int iBribe = makeint(Freight_CargoValue() * (0.45 + 0.30 * u)); // 45-75%
	if (iBribe < 1) iBribe = 1;
	return iBribe;
}

// подкуп клерка: платим, 12-55% клерк кидает (запись не убрана - долг прилетит по провалу), иначе фрахт закрыт без долга (груз остаётся)
void Freight_BribeClerk()
{
	int iBribe = Freight_BribePrepare();
	AddMoneyToCharacter(pchar, -iBribe);

	// клерк кинул: деньги взял, запись не вычеркнул. Внешне всё чисто (квест закрываем, груз остаётся),
	// но клерк донесёт гильдии через 5-15 дней -> долг прилетит тогда. Снимок данных долга (CargoQuest могут перезаписать новым заказом).
	// Исход предрешён ещё ПРИ ВЗЯТИИ фрахта (CargoQuest.ClerkBetray) - сейв-лоадом сделки не переиграть.
	if (CheckAttribute(pchar, "CargoQuest.ClerkBetray"))
	{
		int gIdx = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
		pchar.FreightClerkBetraySnap.nation = sti(characters[gIdx].nation);
		pchar.FreightClerkBetraySnap.goods  = sti(pchar.CargoQuest.iTradeGoods);
		pchar.FreightClerkBetraySnap.qty    = sti(pchar.CargoQuest.iQuantityGoods);
		pchar.FreightClerkBetraySnap.city   = characters[gIdx].city;
		SetTimerCondition("FreightClerkBetray", 0, 0, 5 + rand(10), false); // 5-15 дней - и гильдия узнаёт
		Freight_CloseQuestNoDebt(); // выглядит как успех - игрок не знает, что его сдадут
		return;
	}
	Freight_CloseQuestNoDebt(); // запись потеряна - долга не будет, груз остаётся игроку
}

// клерк донёс гильдии (через 5-15 дней после подкупа): открываем долг по снимку (неустойка 2.0, груз у игрока)
void Freight_ClerkBetrayDebt()
{
	if (!CheckAttribute(pchar, "FreightClerkBetraySnap")) return;
	if (CheckAttribute(pchar, "GuildDebt")) { DeleteAttribute(pchar, "FreightClerkBetraySnap"); return; } // долг уже висит

	int g    = sti(pchar.FreightClerkBetraySnap.goods);
	int qty  = sti(pchar.FreightClerkBetraySnap.qty);
	int iCost = makeint(qty * stf(Goods[g].Cost));

	pchar.GuildDebt.nation     = sti(pchar.FreightClerkBetraySnap.nation);
	pchar.GuildDebt.sum        = makeint(iCost * 2.0);   // подкуп чиновника - тяжелее обычной просрочки (1.25), но легче сбыта барыге (2.5)
	pchar.GuildDebt.GoodsType  = g;
	pchar.GuildDebt.GoodsQty   = qty;
	pchar.GuildDebt.GoodsComp  = makeint(iCost * 1.0);   // груз-то остался на руках: вернул товар - доплачиваешь его же стоимость
	pchar.GuildDebt.TraderCity = pchar.FreightClerkBetraySnap.city;
	pchar.GuildDebt.BlockDays  = 30 + rand(30);
	pchar.GuildDebt.RepPenalty = 0;
	SaveCurrentNpcQuestDateParam(pchar, "GuildDebtDate");

	GuildDebt_ApplyBounty();
	SetTimerCondition("GuildDebt_Daily", 0, 0, 1, true);

	ReOpenQuestHeader("GuildDebt");
	AddQuestRecord("GuildDebt", "1");
	AddQuestUserData("GuildDebt", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("GuildDebt", "sGoodGen", GetGoodsNameAlt(g));
	AddQuestUserData("GuildDebt", "sCity", XI_ConvertString("Colony" + pchar.GuildDebt.TraderCity + "Gen"));
	AddQuestUserData("GuildDebt", "sMoney", FindRussianMoneyString(sti(pchar.GuildDebt.sum)));

	pchar.GuildDebt.Wave = 0;
	GuildDebt_SpawnEnforcers();
	SetTimerCondition("GuildDebt_Wave", 0, 0, 3, true);

	DeleteAttribute(pchar, "FreightClerkBetraySnap");
}

//=====================================================================================================================================
// <-- Неустойка торговой гильдии
//=====================================================================================================================================
