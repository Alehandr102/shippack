#define PRICE_TYPE_BUY	0
#define PRICE_TYPE_SELL	1

#define CONTRA_SELL		1
#define CONTRA_BUY		2

#event_handler("NextDay","StoreDayUpdateStart");
#event_handler("EvStoreDayUpdate","StoreDayUpdate");

// Lugger -->
void SetStoreGoodsEdition(ref _refStore,int _Goods,int _Quantity, int iAddQuantity, bool bDelete)
{
	string tmpstr = Goods[_Goods].name;
	_refStore.Goods.(tmpstr).Quantity = _Quantity;

	if(CheckAttribute(_refStore, "StoreHouse"))
	{
		int iWeight = GetGoodWeightByType(_Goods, iAddQuantity);

		if(bDelete)
		{
			_refStore.current_weight = makeint(_refStore.current_weight) - iWeight;
		}
		else
		{
			_refStore.current_weight = makeint(_refStore.current_weight) + iWeight;
		}

		// Пределы, что не больше и не меньше
		if(makeint(_refStore.current_weight) >= makeint(_refStore.max_weight))
		{
			_refStore.current_weight = makeint(_refStore.max_weight);
		}

		if(makeint(_refStore.current_weight) < 0)
		{
			_refStore.current_weight = 0;
		}

	}
}
// Lugger <--

void SetStoreGoods(ref _refStore,int _Goods,int _Quantity)
{
	string tmpstr = Goods[_Goods].name;
	_refStore.Goods.(tmpstr).Quantity = _Quantity;
}

void AddStoreGoods(ref _refStore,int _Goods,int _Quantity)
{
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	makearef(refGoods,_refStore.Goods.(tmpstr));
	int q = 0;
	if( CheckAttribute(refGoods,"Quantity") ) {q = sti(refGoods.Quantity);}
	refGoods.Quantity = q +_Quantity;
}

void RemoveStoreGoods(ref _refStore,int _Goods,int _Quantity)
{
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	makearef(refGoods,_refStore.Goods.(tmpstr));
	int q = 0;
	if( CheckAttribute(refGoods,"Quantity") ) q = sti(refGoods.Quantity);
	if(q<_Quantity) {refGoods.Quantity = 0;}
	else {refGoods.Quantity = q - _Quantity;}
}

int GetStoreGoodsQuantity(ref _refStore,int _Goods)
{
	string tmpstr = Goods[_Goods].name;
	int q = 0;
	if( CheckAttribute(_refStore,"Goods."+tmpstr) )
		q = sti(_refStore.Goods.(tmpstr).Quantity);
	return q;
}

int GetContrabandGoods(ref _refStore, int _Goods)
{
	string tmpstr = Goods[_Goods].name;
	if( CheckAttribute(_refStore,"Goods."+tmpstr+".canbecontraband") )
	{
		return sti(_refStore.Goods.(tmpstr).canbecontraband); // 1 or 2
	}
	return 0;
}

// Gregg: единый скилл-модификатор торговой цены (было 2 копии здесь + копия в shipyard.c GetCannonPrice).
// rSkillChar — чей навык Коммерции и перки Advanced/BasicCommerce; rTraderChar — чей перк Trader
// (в GetStoreGoodsPrice исторически это разные персонажи: pchar и chref — сохранено).
float GetTradeSkillModify(ref rSkillChar, ref rTraderChar, int _PriceType)
{
	float _TradeSkill = GetSummonSkillFromNameToOld(rSkillChar, SKILL_COMMERCE); // 0..10.0
	float skillModify;
	if(_PriceType==PRICE_TYPE_BUY)
	{
		skillModify = 1.46 - _TradeSkill*0.019; // boal 20.01.2004
		if(CheckOfficersPerk(rSkillChar,"AdvancedCommerce"))	{ skillModify -= 0.2; }
		else
		{
			if(CheckOfficersPerk(rSkillChar,"BasicCommerce"))	{ skillModify -= 0.1; }
		}
		if(CheckOfficersPerk(rTraderChar,"Trader"))	{ skillModify -= 0.05; }
	}
	else
	{
		skillModify = 0.69 + _TradeSkill*0.019; // boal
		if(CheckOfficersPerk(rSkillChar,"AdvancedCommerce"))	skillModify += 0.05;
		if(CheckOfficersPerk(rTraderChar,"Trader"))	{ skillModify += 0.05; }
	}
	return skillModify;
}

// Gregg: фрахтовый бонус коммерции (было 3 копии в блоках фрахта/конвоя/эскорта)
int GetCargoCommerceBonus()
{
	return 9 + sti(pchar.Skill.Commerce) / 20;
}

// Gregg: сумма с набежавшими месячными процентами: Sum + (Sum/100)*Interest*iMonths
// (было 6 копий формулы в Usurer_dialog ru/en; у кредита месяцы идут как iPastMonths+1, у вклада — iPastMonths)
int CalcUsurerSumWithInterest(int iSum, int iInterest, int iMonths)
{
	return iSum + ((iSum/100)*iInterest)*iMonths;
}

int GetStoreGoodsPrice(ref _refStore, int _Goods, int _PriceType, ref chref, int _qty)
{
	if(CheckAttribute(_refStore, "StoreHouse"))
	{
		return 0;
	}
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	int basePrice = MakeInt(Goods[_Goods].Cost);
		if (!CheckAttribute(_refStore,"Goods."+tmpstr) ) return 0;
	makearef(refGoods,_refStore.Goods.(tmpstr));
 	int tradeType = MakeInt(refGoods.TradeType);
	float tradeModify = 1.0;

	bool isPirateColony = false;
	bool isPCharOwner = false;
	if (_refStore.Colony != "none")
	{
		if (sti(Colonies[FindColony(_refStore.Colony)].Nation) == PIRATE) isPirateColony = true;
		if (sti(Colonies[FindColony(_refStore.Colony)].HeroOwn) == true) isPCharOwner = true;
	}

	switch (tradeType)
	{
		case TRADE_TYPE_EXPORT:
			tradeModify = TRADE_MOD_EXPORT + stf(refGoods.RndPriceModify);		//+r0.08
			break;
		case TRADE_TYPE_NORMAL:
			tradeModify = TRADE_MOD_NORMAL + stf(refGoods.RndPriceModify);		//+r0.1
			break;
		case TRADE_TYPE_IMPORT:
			tradeModify = TRADE_MOD_IMPORT + stf(refGoods.RndPriceModify);		//+r0.12
			break;
		case TRADE_TYPE_AGGRESSIVE:
			tradeModify = TRADE_MOD_AGGRESSIVE + stf(refGoods.RndPriceModify);	//+r0.16
			break;
		case TRADE_TYPE_CONTRABAND:
			if (!isPirateColony || isPCharOwner || (_PriceType == PRICE_TYPE_BUY))
				tradeModify = TRADE_MOD_CONTRABAND + stf(refGoods.RndPriceModify);	//+r0.2
			else
				// В пиратских поселениях, которые не принадлежат игроку - цена продажи небольшая,
				// потому что много желающих продать, а покупают только пираты.
				tradeModify = TRADE_MOD_CONTRA_PIRATE + stf(refGoods.RndPriceModify);
			break;
		case TRADE_TYPE_AMMUNITION:
			//делаю всё тоже, что и для нормального товара, а тип нужен, чтоб на корабле не скупали лишнее.
			tradeModify = TRADE_MOD_NORMAL + stf(refGoods.RndPriceModify);		//+r0.1
			break;
		case TRADE_TYPE_CANNONS:
			tradeModify = TRADE_MOD_NORMAL + stf(refGoods.RndPriceModify);		//+r0.1
			break;
	}

	float skillModify = GetTradeSkillModify(pchar, chref, _PriceType);
	if(tradeType == TRADE_TYPE_CANNONS)
	{
		//цены пушек меняем после применения перков, иначе непропорционально их влияние на итог
		if(_PriceType==PRICE_TYPE_BUY) skillModify *= TRADE_MOD_CANNONS_BUY;
		else skillModify /= TRADE_MOD_CANNONS_SELL_DIV;
	}

	// boal 23.01.2004 -->
	if (MakeInt(basePrice*tradeModify*skillModify + 0.5) < 1) return 1;
	// boal 23.01.2004 <--
	if(CheckAttribute(pchar,"Goods.Store.Contraband")) return MakeInt(basePrice*tradeModify*skillModify*_qty  + 0.5);

    return MakeInt(basePrice*tradeModify*skillModify*_qty  + 0.5);
}

// обратное преобразование цены в RndPriceModify
float GetStoreGoodsRndPriceModify(ref _refStore, int _Goods, int _PriceType, ref chref, int _price)
{
	if(CheckAttribute(_refStore, "StoreHouse"))
	{
		return 0.0;
	}
    float skillModify;
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	int basePrice = MakeInt(Goods[_Goods].Cost);
	if (!CheckAttribute(_refStore,"Goods."+tmpstr) ) return 0;
	makearef(refGoods,_refStore.Goods.(tmpstr));
 	int tradeType = MakeInt(refGoods.TradeType);

	float tradeModify = 1.0;

    skillModify = GetTradeSkillModify(chref, chref, _PriceType);
	tradeModify = makefloat(_price) / (basePrice*skillModify);

	switch (tradeType)
	{
		case TRADE_TYPE_EXPORT:
			tradeModify = tradeModify - 0.55; //0.55;
			break;
		case TRADE_TYPE_NORMAL:
			tradeModify = tradeModify - 0.85;
			break;
		case TRADE_TYPE_IMPORT:
			tradeModify = tradeModify - 1.2; //1.2;
			break;
		case TRADE_TYPE_AGGRESSIVE:
			tradeModify = tradeModify - 1.5;//почему не -1.5 было, а  просто -0.0???
		break;
		case TRADE_TYPE_CONTRABAND:
			tradeModify = tradeModify - 1.8; //2.4;
			break;
		case TRADE_TYPE_AMMUNITION:
			tradeModify = tradeModify - 0.85;
			break;
		case TRADE_TYPE_CANNONS:
			tradeModify = tradeModify - 0.85;
			break;
	}
	 // может быть минусом, а что делать :(
    return tradeModify;
}
// <--
string GetStoreGoodsType(ref _refStore,int _Goods)
{
	string tmpstr = Goods[_Goods].name;
	int tradeType = TRADE_TYPE_NORMAL;
	if( CheckAttribute(_refStore,"Goods."+tmpstr) ) {
		tradeType = sti(_refStore.Goods.(tmpstr).TradeType);
	}

	switch(tradeType)
	{
	case TRADE_TYPE_NORMAL:
		return TRADE_TYPE_NORMAL_NAME;
		break;
	case TRADE_TYPE_EXPORT:
		return TRADE_TYPE_EXPORT_NAME;
		break;
	case TRADE_TYPE_IMPORT:
		return TRADE_TYPE_IMPORT_NAME;
		break;
	case TRADE_TYPE_CONTRABAND:
		return TRADE_TYPE_CONTRABAND_NAME;
		break;
	case TRADE_TYPE_AMMUNITION:
		return TRADE_TYPE_AMMUNITION_NAME;
		break;
	case TRADE_TYPE_CANNONS:   //new boal
		return TRADE_TYPE_CANNONS_NAME;
		break;
	case TRADE_TYPE_AGGRESSIVE:
		return TRADE_TYPE_AGGRESSIVE_NAME;
		break;
	}
	Trace("Missing trade type");
	return TRADE_TYPE_NORMAL_NAME;
}

int storeDayUpdateCnt = -1;
void StoreDayUpdateStart()
{
	if(storeDayUpdateCnt >= 0) return;
	storeDayUpdateCnt = 0;
	PostEvent("EvStoreDayUpdate", 30);

	Event("EvSituationsUpdate", "l", 0);   // вызов размазаных вычислений на Нехт?ай
}

void StoreDayUpdate()
{
	if(storeDayUpdateCnt >= 0)
	{
		UpdateStore(&Stores[storeDayUpdateCnt]);
		storeDayUpdateCnt++;
		if(storeDayUpdateCnt >= STORE_QUANTITY) storeDayUpdateCnt = -1;
		PostEvent("EvStoreDayUpdate", 30);
	}
}

void UpdateStore(ref pStore)
{
	if(CheckAttribute(pStore, "StoreHouse"))
	{
		return;
	}
	aref gref, curref;
	makearef(gref, pStore.Goods);
	int delta, oldQty;
	string tmpstr, nsale;
    float rateInc = 0.0;

	for(int i=0; i<GOODS_QUANTITY; i++)
	{
		tmpstr = Goods[i].name;
		if (!CheckAttribute(gref,tmpstr) ) continue;
		makearef(curref, gref.(tmpstr));
        // пересмотр системы 24.01.08. Нова§ - "круги на воде"
        oldQty = sti(curref.Quantity);
        delta = makeint((oldQty - sti(curref.Norm))/7);
        curref.Quantity = oldQty - delta + (rand(2) - 1)*rand(sti(sti(curref.Norm)/100));

		if( sti(pStore.Goods.(tmpstr).TradeType) == TRADE_TYPE_CANNONS )
		{
			curref.Quantity = oldQty - delta + (rand(2) - 1)*rand(sti(sti(curref.Norm) * 3/100));
			if(sti(Goods[i].NotSale) == 1) // 1.2.5 --> старшие калибры не продаем !!!
			{
				curref.Quantity = 0;
				continue;
			}
		}

        if (sti(curref.Quantity) < 0) curref.Quantity = 0;
		if(stf(curref.Norm) > 0.0)
		{
			rateInc = stf(curref.Quantity) / stf(curref.Norm);
		}

		if (abs(rateInc - 1) < 0.015 )
		{  // нормализаци§ врем§ от времени
		    curref.RndPriceModify = curref.NormPriceModify;
		}
		else
		{
			curref.RndPriceModify = AddPriceModify(stf(curref.RndPriceModify), sti(curref.TradeType), rateInc);
		}
	}
}

float AddPriceModify(float curModify, int tradeType, float rateInc)
{
	float fMaxModify = 0.5;
	float fMinModify = -0.3;
	float delta = rand(7)/1000.0;

	switch(tradeType)
	{
		case TRADE_TYPE_IMPORT:
		     fMinModify = -0.4;
             break;

		case TRADE_TYPE_CONTRABAND:
		     fMaxModify = 0.3;
		     fMinModify = -1.5;
             break;
		case TRADE_TYPE_AGGRESSIVE:
			fMinModify = -0.85;
			fMaxModify = 0.15;
			break;
	}
	if (rateInc <= 0.965)
	{
		curModify += delta;
	}
	else
	{
		if (rateInc >= 1.035)
        {
			curModify -= delta;
		}
	}

	if(curModify < fMinModify)
	    curModify = fMinModify;

	if(curModify > fMaxModify)
        curModify = fMaxModify;

	return curModify;
}

void FillShipStore( ref chr)
{
	ref pRef = &stores[SHIP_STORE];
	// boal все лишнее убрал, просто сбросить RndPriceModify   в 1 и все, тк иначе затаривание будет
	int iQuantity = 0;
	string goodName;

	for(int i = 0; i<GOODS_QUANTITY; i++)
	{
		iQuantity = GetCargoGoods(chr, i);
		SetStoreGoods(pref, i, iQuantity);

		goodName = Goods[i].name;
		pRef.Goods.(goodName).RndPriceModify = 1.0;
	}
}

int FindStore(string sColony)
{
	for(int i = 0; i < STORE_QUANTITY; i++)
	{
		if (Stores[i].colony == sColony)
		{
			return i;
		}
	}
	return -1;
}

void CheckForGoodsSpoil(aref chr)
{
	int iGoodsQ = 0;
	for(int i = 0; i < GOODS_QUANTITY; i++)
	{
		iGoodsQ = GetCargoGoods(chr, i);
		if(iGoodsQ > 0)
		{
			if(CheckAttribute(&Goods[i], "AfraidWater"))
			{
				if(sti(Goods[i].AfraidWater) > 0)
				{
					iGoodsQ = iGoodsQ - sti(Goods[i].AfraidWater);
					if(iGoodsQ < 0)
					{
						iGoodsQ = 0;
					}
					SetCharacterGoods(chr, i, iGoodsQ);
				}
			}
		}
	}
}

int GetStoreFreeSpace(object refStore)
{
	int iGoodsQ = 0;
	int iWeight = 0;
	for(int i = 0; i < GOODS_QUANTITY; i++)
	{
		iGoodsQ = GetStoreGoodsQuantity(refStore, i);
		iWeight = iWeight + iGoodsQ * stf(Goods[i].weight);
	}
	return iWeight;
}

// boal -->
string GetGoodsNameAlt(int idx)
{
    string ret;
    int iSeaGoods = LanguageOpenFile("ShipEatGood.txt");
    ret = LanguageConvertString(iSeaGoods, "seg_" + Goods[idx].Name);
    LanguageCloseFile(iSeaGoods);

    return ret;
}
// запоминаем цены в ГГ
void SetPriceListByStoreMan(ref rchar)   //rchar - это колония
{
    ref refStore, nulChr;
    string attr1, sGoods;
    int i, tradeType;
    string tmpstr;
    aref   refGoods;

    nulChr = &NullCharacter;

    if (sti(rchar.StoreNum) >= 0)
    {
        refStore = &stores[sti(rchar.StoreNum)];

	if(CheckAttribute(refStore, "StoreHouse"))
	{
		return;
	}
        attr1 = rchar.id; // ветка, где храним цены
        for (i = 0; i < GOODS_QUANTITY; i++)
        {
            sGoods = "Gidx" + i;

        	tmpstr = Goods[i].name;
        	tradeType = 0;
        	if (CheckAttribute(refStore,"Goods."+tmpstr))
        	{
        	   makearef(refGoods, refStore.Goods.(tmpstr));
         	   tradeType = MakeInt(refGoods.TradeType);
         	}
            nulChr.PriceList.(attr1).(sGoods).tradeType = tradeType; // тип торговли
            nulChr.PriceList.(attr1).(sGoods).Buy  = GetStoreGoodsPrice(refStore, i, PRICE_TYPE_BUY, pchar, 1);
            if (tradeType == TRADE_TYPE_CONTRABAND && !bBettaTestMode)
            {
                nulChr.PriceList.(attr1).(sGoods).Buy  = "???";
            }
            nulChr.PriceList.(attr1).(sGoods).Sell = GetStoreGoodsPrice(refStore, i, PRICE_TYPE_SELL, pchar, 1);
            if (tradeType == TRADE_TYPE_CONTRABAND && !bBettaTestMode)
            {
                nulChr.PriceList.(attr1).(sGoods).Sell  = "???";
            }
            nulChr.PriceList.(attr1).(sGoods).Qty  = GetStoreGoodsQuantity(refStore, i);
            if (tradeType == TRADE_TYPE_CONTRABAND && !bBettaTestMode)
            {
                nulChr.PriceList.(attr1).(sGoods).Qty  = "???";
            }
        }
        nulChr.PriceList.(attr1).AltDate = GetQuestBookDataDigit();
    }
}
// нулим магазин при захвате города эскадрой
void SetNull2StoreMan(ref rchar)
{
    ref refStore;
    int i;
    string tmpstr

    if (sti(rchar.StoreNum) >= 0)
    {
        refStore = &stores[sti(rchar.StoreNum)];

        for (i = 0; i < GOODS_QUANTITY; i++)
        {
            tmpstr = Goods[i].name;
        	if( CheckAttribute(refStore,"Goods."+tmpstr) )
        	{
        		refStore.Goods.(tmpstr).Quantity = 0;
            }
        }
    }
}
// делим колво товара - остаток дл§ грабежа rchar - это колони§
void SetNull2StoreManPart(ref rchar, float part)
{
    ref refStore;
    int i;
    string tmpstr

    if (sti(rchar.StoreNum) >= 0)
    {
        refStore = &stores[sti(rchar.StoreNum)];

        for (i = 0; i < GOODS_QUANTITY; i++)
        {
            tmpstr = Goods[i].name;
        	if (CheckAttribute(refStore,"Goods."+tmpstr) )
        	{
        		refStore.Goods.(tmpstr).Quantity = makeint(sti(refStore.Goods.(tmpstr).Quantity) / part);
            }
        }
    }
}

// нулим ростовщиков
void SetNull2Deposit(string _city)
{
    int iSum;

    if (CheckAttribute(Pchar, "quest.Deposits." + _city))
    {
        // LEO: сообщать о пропаже ТОЛЬКО при реально существующем вкладе.
        //   Жалоба игрока: вкладов нигде нет, а сообщение "все вклады в Порто Белло пропали" пришло.
        //   Причина: ветка quest.Deposits.<город> создаётся САМА при разговоре с ростовщиком —
        //   Usurer_dialog.c:1483 и :1535 пишут `Result = 0` как "fix" на отсутствующий атрибут.
        //   Достаточно зайти в диалог о вкладе и НЕ вложить ничего: контейнер уже есть, .Sum нет.
        //   Проверка ниже смотрит на САМ вклад, а не на существование контейнера.
        iSum = 0;
        if (CheckAttribute(Pchar, "quest.Deposits." + _city + ".Sum"))
        {
            iSum = sti(Pchar.quest.Deposits.(_city).Sum);
        }

        if (iSum <= 0)
        {
            // пустышка от разговора — молча убираем, чтобы не копилась в сейве и не всплывала снова
            DeleteAttribute(Pchar, "quest.Deposits." + _city);
            return;
        }

        // LEO: пропажа вкладов - КРАСНЫМ (без анимации) + звук, заметное событие потери
        Log_InfoColored("red", GetConvertStrWithReplace("Variable_StoreUtilite_5", "Logs.txt", "#space#", " ") + GetCityName(_city) + GetConvertStrWithReplace("Variable_StoreUtilite_6", "Logs.txt", "#space#", " "));
        PlayStereoSound("interface\bort_jurnal.wav");
        DeleteAttribute(Pchar, "quest.Deposits." + _city);
    }
}

// утилиты работы со складом --> ugeen  28.01.10
void SetStorageGoods(ref _refStore,int _Goods,int _Quantity)
{
	string tmpstr = Goods[_Goods].name;
	_refStore.Storage.Goods.(tmpstr).Quantity = _Quantity;
}

void AddStorageGoods(ref _refStore,int _Goods,int _Quantity)
{
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	makearef(refGoods,_refStore.Storage.Goods.(tmpstr));
	int q = 0;
	if( CheckAttribute(refGoods,"Quantity") ) {q = sti(refGoods.Quantity);}
	refGoods.Quantity = q +_Quantity;
}

void RemoveStorageGoods(ref _refStore,int _Goods,int _Quantity)
{
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	makearef(refGoods,_refStore.Storage.Goods.(tmpstr));
	int q = 0;
	if( CheckAttribute(refGoods,"Quantity") ) q = sti(refGoods.Quantity);
	if(q<_Quantity) {refGoods.Quantity = 0;}
	else {refGoods.Quantity = q - _Quantity;}
}

int GetStorageGoodsQuantity(ref _refStore,int _Goods)
{
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	makearef(refGoods,_refStore.Storage.Goods.(tmpstr));
	int q = 0;
	if( CheckAttribute(_refStore,"Storage.Goods."+tmpstr) && CheckAttribute(refGoods,"Quantity"))
	{
		q = sti(_refStore.Storage.Goods.(tmpstr).Quantity);
	}
	return q;
}

int GetStorageUsedWeight(object refStore)
{
	int iGoodsQ = 0;
	int iWeight = 0;
	for(int i = 0; i < GOODS_QUANTITY; i++)
	{
		iGoodsQ = GetStorageGoodsQuantity(refStore, i);
		iWeight = iWeight + GetGoodWeightByType(i, iGoodsQ)
	}
	return iWeight;
}

// Gregg: GetStorage(sColony) удалён — был байт-в-байт дубликатом FindStore(sColony)

void UpdateSmugglers()
{
	int max_gold = 500000;
	int increment = 50000;
	if (CheckCharacterPerk(pchar, "UnlimitedContra"))
	{
		max_gold = 1000000
		increment = 100000;
	}
	for(int n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].nation != "none" && sti(colonies[n].nation) != PIRATE && colonies[n].id != "Panama")
		{
			sld = CharacterFromID(colonies[n].id + "_Smuggler");
			if (!CheckAttribute(sld, "SmugglingMoney"))
			{
				sld.SmugglingMoney = max_gold + rand(increment);
				SaveCurrentQuestDateParam("SmugglingMoney"+n);
			}
			else
			{
				if(GetQuestPastDayParam("SmugglingMoney" + n) >= (5 + rand(2)) && sti(sld.SmugglingMoney) < max_gold)
				{
					sld.SmugglingMoney = sti(sld.SmugglingMoney) + increment + rand (increment);
					SaveCurrentQuestDateParam("SmugglingMoney"+n);
				}
			}
		}
	}
}

void SetSpecialGoods(ref chref)
{
	int amount, rank;
	rank = sti(pchar.rank);
	for (int i = 0; i < 4; i++)
	{
		if (rank < 5) amount = 20 + rand(40);
		else if (rank > 4 && rank < 10) amount = 40 + rand(80);
		else if (rank > 9 && rank < 15) amount = 100 + rand(200);
		else if (rank > 14 && rank < 20) amount = 200 + rand(400);
		else if (rank > 19 && rank < 25) amount = 350 + rand(800);
		else if (rank > 24) amount = 500 + rand(1000);
		switch (i)
		{
			case 0: SetContraGoods(chref, 37, amount);
			break;
			case 1: SetContraGoods(chref, 38, amount);
			break;
			case 2: SetContraGoods(chref, 39, amount);
			break;
			case 3: SetContraGoods(chref, 40, amount);
			break;
		}
	}
}

// <-- ugeen
void ChangeImport()
{
	string goodName;
	ref pRef;
	int iType;

	//Границы перегода на другой тип
	int gModifierExport,gModifierImport;

	for(int i=0; i<STORE_QUANTITY; i++)
    {
		if(GetQuestPastDayParam("ChangeImport"+i) >= (5 + rand(2)))
		{
			pRef = &Stores[i];
			if(pRef.Colony != "Pirates" && pRef.Colony != "none" && pRef.Colony != "Panama")
			{
				int contraband = 0;
				int export = 0;
				int import = 0;
				int aggress = 0;
				for (int j = 0; j < GOODS_QUANTITY; j++)
				{
					goodName = Goods[j].Name;
					if(sti(pRef.Goods.(goodName).TradeType) == 3)
					{
						contraband++;
					}
					if(sti(pRef.Goods.(goodName).TradeType) == 1)
					{
						export++;
					}
					if (pRef.Goods.(goodName).TradeType == "2" || pRef.Goods.(goodName).TradeType == "3" || pRef.Goods.(goodName).TradeType == "6")
					{
						import++;
					}
					if (pRef.Goods.(goodName).TradeType == "6")
					{
						aggress++;
					}
				}
				for (j = 0; j < GOODS_QUANTITY; j++)
				{
					if(j >= GOOD_CANNON_8 && j <= GOOD_CANNON_48) continue;//пушки динамическую торговлю игнорируют? 
					if (j == GOOD_SLAVES) continue;//рабы динамическую торговлю игнорируют? 
					if(j >= GOOD_SHIPSILK && j <= GOOD_RESIN) continue;//убрать страт товары
					goodName = Goods[j].Name;

					gModifierExport = sti(sti(Goods[j].Norm)*0.6);
					gModifierImport = sti(sti(Goods[j].Norm)*0.3);

					switch(sti(pRef.Goods.(goodName).TradeType))
					{
						case TRADE_TYPE_NORMAL:
							if (rand(1) == 0)
							{
								pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*1.2);
							}
							else
							{
								pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*0.8);
							}
						break;
						case TRADE_TYPE_EXPORT:
							pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*0.9);
						break;
						case TRADE_TYPE_IMPORT:
							pRef.Goods.(goodName).Quantity =  sti(sti(pRef.Goods.(goodName).Quantity)*1.1);
						break;
						case TRADE_TYPE_CONTRABAND:
							pRef.Goods.(goodName).Quantity =  sti(sti(pRef.Goods.(goodName).Quantity)*1.3 + rand(50));
						break;
						case TRADE_TYPE_AGGRESSIVE:
							pRef.Goods.(goodName).Quantity =  sti(sti(pRef.Goods.(goodName).Quantity)*1.2);
						break;
					}
					if (j <= GOOD_RUM)
					{//Расходники восстанавливаются чуток быстрее
						if (sti(pRef.Goods.(goodName).Quantity) < 250 * sti(Goods[j].Units))
						{
							pRef.Goods.(goodName).Quantity = sti(pRef.Goods.(goodName).Quantity) + rand(100 * sti(Goods[j].Units));
						}
					}
					int goodsQty = sti(pRef.Goods.(goodName).Quantity);   // LEO: была лишняя закрывающая скобка

					if(i == 3 || i == 4 || i == 10 || i == 19)//номера магазинов столиц		3=Villemstad_STORE	10=Havana_STORE	19=PortRoyal_STORE
					{
						iType = rand(2);
						if (aggress == 0)
						{
							aggress = -2;
						}
						if (i == 4 && CheckAttribute(pchar,"Whisper.Contraband") &&  j == GOOD_EBONY)// для особого задания Виспер?   //4=Tortuga_STORE
						{
							continue;
						}
					}

					else
					{
						iType = rand(1);
					}
					//импорт
					if (j > 10 && goodsQty < gModifierImport && pRef.Goods.(goodName).TradeType != "2" && pRef.Goods.(goodName).TradeType != "3" && pRef.Goods.(goodName).TradeType != "6")
					{
						if (sti(pRef.Goods.(goodName).TradeType) != 1)
						{
							if (import < 9 || goodsQty < gModifierImport/3)
							{
								import++;
								if(j == GOOD_GOLD || j == GOOD_SILVER)//золото и серебро не меняется на экспорт?  не очень понял в этих кодах. 
								{
									pRef.Goods.(goodName).TradeType = 2;
									pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*0.2);
								}
								else
								{
									if(iType == 0)
									{
										if (aggress < 1)
										{
											pRef.Goods.(goodName).TradeType = 6;
											aggress++;
										}
										else
										{
											if(contraband > 3)
											{
												pRef.Goods.(goodName).TradeType = 2;
											}
											else 
											{
												if(pRef.Colony != "Caiman")//неконтрабандность Каймана
												{
													pRef.Goods.(goodName).TradeType = 3;
													contraband++;
												}
											}
										}
									}
									if(iType == 1)
									{
										if(contraband > 3)
										{
											if (aggress < 1)
											{
												pRef.Goods.(goodName).TradeType = 6;
												aggress++;
											}
											else
											{
												pRef.Goods.(goodName).TradeType = 2;
											}
										}
										else
										{
											if(pRef.Colony != "Caiman")//неконтрабандность Каймана
											{
												pRef.Goods.(goodName).TradeType = 3;
												contraband++;
											}
										}
									}
									pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*0.3);
								}
							}
							else
							{
								pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*1.2);
								pRef.Goods.(goodName).TradeType = 0;
							}
						}
						else
						{
							pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*1.5);
							pRef.Goods.(goodName).TradeType = 0;
						}

					}
					//норма
					if (goodsQty > gModifierImport && goodsQty < gModifierExport)
					{
						if (pRef.Goods.(goodName).TradeType == "2" || pRef.Goods.(goodName).TradeType == "3" || pRef.Goods.(goodName).TradeType == "6")
						{
							if(goodsQty > gModifierImport*1.3)
							{
								pRef.Goods.(goodName).TradeType = 0;
								import--;
							}
						}
						if (pRef.Goods.(goodName).TradeType == "1")
						{
							pRef.Goods.(goodName).TradeType = 0;
							if (sti(pRef.Goods.(goodName).Quantity) < gModifierImport*1.5)
							{
								pRef.Goods.(goodName).Quantity = gModifierExport/2;
							}
							export--;
						}
					}
					//экспорт
					if (goodsQty > gModifierExport)
					{
						if (sti(pRef.Goods.(goodName).TradeType) == 0)
						{
							if (export < 5)
							{//если не много экспортов
								pRef.Goods.(goodName).TradeType = 1;
								pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*2 + rand(sti(sti(pRef.Goods.(goodName).Quantity))));
								export++;
							}
							else
							{
								if (goodsQty > gModifierExport * 1.7)
								{//если экспортов достаточно, но товара слишком много, чтоб игнорить
									pRef.Goods.(goodName).TradeType = 1;
									pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*2 + rand(sti(sti(pRef.Goods.(goodName).Quantity))));
									export++;
								}
								else
								{
									pRef.Goods.(goodName).Quantity = sti(sti(pRef.Goods.(goodName).Quantity)*0.9);
								}
							}
						}
						else
						{
							if (sti(pRef.Goods.(goodName).TradeType) != 1)
							{
								pRef.Goods.(goodName).Quantity = gModifierExport * 0.9;
							}
						}
					}

					switch(sti(pRef.Goods.(goodName).TradeType))
					{
						case TRADE_TYPE_NORMAL:
							pRef.Goods.(goodName).RndPriceModify = frnd() * 0.15;
						break;
						case TRADE_TYPE_EXPORT:
							pRef.Goods.(goodName).RndPriceModify = frnd() * 0.1;
						break;
						case TRADE_TYPE_IMPORT:
							pRef.Goods.(goodName).RndPriceModify = frnd() * 0.20;
						break;
						case TRADE_TYPE_CONTRABAND:
							pRef.Goods.(goodName).RndPriceModify = frnd() * 0.35;
						break;
						case TRADE_TYPE_AGGRESSIVE:
							pRef.Goods.(goodName).RndPriceModify = frnd() * 0.25;
						break;
					}
				}
			}
			SaveCurrentQuestDateParam("ChangeImport"+i);
		}
	}
}

// =====================================================================================
//  Генераторные квесты "доставка груза" (фрахт у торговца в магазине).
//  Здесь — ОБЩАЯ (рус/англ) логика типов заказа: какой тип, сколько дней и денег, объём.
//  Тексты диалога и сборка заказов — в DIALOGS/{russian,english}/Common_Store.c.
//
//  ЧТОБЫ ДОБАВИТЬ НОВЫЙ ТИП ЗАКАЗА: завести CARGO_*, и учесть его в RollCargoOrderType,
//  GetCargoOrderDays, GetCargoOrderMoney, GetCargoOrderQty, GetCargoOrderGood и GetCargoOrderName.
// =====================================================================================
#define CARGO_SIMPLE    0   // простой: много времени на дорогу, оплата ниже
#define CARGO_STANDARD  1   // обычный: средние сроки и оплата
#define CARGO_URGENT    2   // срочный: мало времени, оплата выше
#define CARGO_PERISH    3   // скоропортящийся: очень мало времени, оплата выше
#define CARGO_PREMIUM   4   // премиальный (редкий): большой объём, оплата x1.8
#define CARGO_SMUGGLE   5   // контрабанда (редкий): сдаётся смуглер-агенту в городе назначения, оплата максимальная
#define CARGO_GOVERNOR  6   // губернаторский (редкий): госзаказ в обход таможни, сдаётся губернатору/мэру города, груз - в магазин города
#define CARGO_FORTAMMO  7   // комендантский (редкий): боеприпасы гарнизону, сдаётся коменданту форта (начальнику тюрьмы Common_Prison.c), груз расходуется

// --- дневной рандом для заказов ---
// dRand/dRand1/dRand2/dRand3 (NumberUtilities.c) — штатный ДНЕВНОЙ рандом: одно значение за игровые
// сутки, хранится в PChar.DayRandom* и СОХРАНЯЕТСЯ в сейве (сбрасывается только при смене суток),
// поэтому заказы стабильны весь день и НЕ меняются при перезагрузке. Один поток на слот заказа.
int dRandSlot(int slot, int maxv)
{
	if (slot <= 1) return dRand(maxv);
	if (slot == 2) return dRand1(maxv);
	if (slot == 3) return dRand2(maxv);
	return dRand3(maxv);
}

// контрабандный фрахт: смуглеры работают только с неприметной эскадрой -
// не больше 3 кораблей, и ни один не мощнее 3 класса (класс 1 - самый сильный, 7 - самый слабый).
// Перк "Заядлый контрабандист" снимает ограничение полностью (штатное поведение перка:
// в ванильной сделке в бухте проверка эскадры тоже скипается по этому перку).
bool CheckSmuggleFreightShips()
{
	if (CheckCharacterPerk(pchar, "UnlimitedContra")) return true; // заядлому контрабандисту верят с любой эскадрой
	if (GetCompanionQuantity(pchar) > 3) return false;
	int cn;
	int iShipType;
	for (int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if (cn < 0) continue;
		if (!CheckAttribute(&Characters[cn], "ship.type")) continue;
		iShipType = sti(Characters[cn].ship.type);
		if (iShipType == SHIP_NOTUSED) continue;
		if (sti(RealShips[iShipType].Class) < 3) return false; // 1-2 класс - слишком мощный, засекут
	}
	return true;
}

// тип заказа; slot задаёт дневной поток рандома.
// bAllowSpecial=false (напр. в пиратских поселениях) - не выдаём контрабанду/губернаторский, только базовые+премиум.
int RollCargoOrderType(int slot, bool bAllowSpecial)
{
	int r = dRandSlot(slot, 9); // 0..9
	if (r == 0) return CARGO_PREMIUM;  // ~10%
	if (bAllowSpecial)
	{
		if (r == 1) return CARGO_SMUGGLE;  // ~10% контрабанда
		if (r == 2) return CARGO_GOVERNOR; // ~10% губернаторский
		if (r == 3 && FreightConvoy_CanTake(CARGO_FORTAMMO)) return CARGO_FORTAMMO; // LEO: военный конвой только подходящему капитану.
	}
	return dRandSlot(slot, 3); // 0..3 = простой / обычный / срочный / скоропортящийся
}

// срок доставки (в днях) по типу заказа. maxDays — минимальное время на дорогу.
int GetCargoOrderDays(int otype, int maxDays, int slot)
{
	if (maxDays < 1) maxDays = 1;
	int days;
	if (otype == CARGO_SIMPLE)        days = maxDays * 2 + 3 + dRandSlot(slot, maxDays);                 // вагон времени
	else if (otype == CARGO_STANDARD) days = makeint(maxDays * 1.5) + 2 + dRandSlot(slot, maxDays / 2);  // с запасом
	else if (otype == CARGO_PREMIUM)  days = makeint(maxDays * 1.3) + 2 + dRandSlot(slot, maxDays / 2);  // объём важнее срока
	else if (otype == CARGO_SMUGGLE)  days = makeint(maxDays * 1.5) + 2 + dRandSlot(slot, maxDays / 2);  // контрабанда - с запасом (нужно успеть к смуглерам)
	else if (otype == CARGO_GOVERNOR) days = makeint(maxDays * 1.4) + 2 + dRandSlot(slot, maxDays / 2);  // губернаторский - с разумным запасом
	else if (otype == CARGO_FORTAMMO) days = makeint(maxDays * 1.4) + 2 + dRandSlot(slot, maxDays / 2);  // комендантский - с разумным запасом
	else if (otype == CARGO_URGENT)   days = makeint(maxDays * 0.9) + 1 + dRandSlot(slot, maxDays / 4);  // впритык
	else                              days = makeint(maxDays * 0.7) + 1 + dRandSlot(slot, maxDays / 5);  // скоропортящийся — гонка
	if (days < 3) days = 3;
	return days;
}

// объём груза (шт.) по типу заказа и свободному месту в трюме эскадры
int GetCargoOrderQty(int otype, int freeSpace, int slot)
{
	int qty;
	if (otype == CARGO_PREMIUM || CheckOfficersPerk(pchar, "Trader")) qty = freeSpace; // премиум/торговец — под завязку
	else qty = freeSpace - dRandSlot(slot, freeSpace / 4 - dRandSlot(slot, freeSpace / 5));
	if (otype == CARGO_SIMPLE) qty = makeint(qty * 0.7); // простой заказ — груза поменьше
	if (qty < 0) qty = 0;
	return qty;
}

// младшие разряды дневного float потока слота: 0..maxv, практически независимы от старших разрядов.
// Нужно для товара: тип заказа берётся из СТАРШИХ разрядов того же float (dRandSlot(slot,9)),
// поэтому прямой dRandSlot с малым max дал бы товар, жёстко привязанный к типу
// (скоропортящийся - всегда верх диапазона: паприка/фрукты/масло/кирпич).
int dRandSlotLow(int slot, int maxv)
{
	int big = dRandSlot(slot, 9999);
	int q = makeint(big / (maxv + 1));
	int r = big - q * (maxv + 1);
	if (r < 0) r = r + maxv + 1;   // страховка, если makeint округляет вверх
	if (r > maxv) r = r - maxv - 1;
	return r;
}

// продовольственный (скоропортящийся) ли товар - фильтр для живого выбора и ролла PERISH
bool CargoGoodPerishable(int iGoods)
{
	if (iGoods == GOOD_CHOCOLATE) return true;
	if (iGoods == GOOD_COFFEE)    return true;
	if (iGoods == GOOD_SUGAR)     return true;
	if (iGoods == GOOD_WHEAT)     return true;
	if (iGoods == GOOD_WINE)      return true;
	if (iGoods == GOOD_ALE)       return true;
	if (iGoods == GOOD_CINNAMON)  return true;
	if (iGoods == GOOD_COPRA)     return true;
	if (iGoods == GOOD_PAPRIKA)   return true;
	if (iGoods == GOOD_FRUITS)    return true;
	return false;
}

// =====================================================================================
//  ФАЗА 2 реворка: ЖИВАЯ ЭКОНОМИКА фрахта (только базовые типы 0..4).
//  Возим то, что выгодно торговцу: дёшево у отправителя (экспорт), дорого у адресата (импорт).
//  Доля ГГ = 25% от прибыли перевозки по живым ценам магазинов. Корабли ГГ - "баржа".
// =====================================================================================
ref GetCityStoreRef(string sCity)
{
	// по шагам: вложенное выражение в индексе массива компилятор Storm не переваривает
	ref rCol = GetColonyByIndex(FindColony(sCity));
	int iStoreNum = sti(rCol.StoreNum);
	return &Stores[iStoreNum];
}

// ОПТОВАЯ цена товара в магазине: база x профиль колонии (экспорт/импорт), БЕЗ игроцкого скилл-спреда
// (GetStoreGoodsPrice с его наценкой/дисконтом от Коммерции делал невыгодными ПОЧТИ ВСЕ пары - торговец возит по опту).
// 0 = магазин таким не торгует ЛИБО товар здесь контрабандный (легальный фрахт контрабанду не возит - для неё тип 5)
int GetCargoTradePrice(ref rStore, int iGoods)
{
	string tmpstr = Goods[iGoods].name;
	if (!CheckAttribute(rStore, "Goods." + tmpstr)) return 0;
	aref refGoods;
	makearef(refGoods, rStore.Goods.(tmpstr));
	int tradeType = MakeInt(refGoods.TradeType);
	float tradeModify = 0.0; // 0 останется у контрабанды/пушек - их легальный фрахт не возит
	switch (tradeType)
	{
		case TRADE_TYPE_EXPORT:     tradeModify = TRADE_MOD_EXPORT + stf(refGoods.RndPriceModify); break;
		case TRADE_TYPE_NORMAL:     tradeModify = TRADE_MOD_NORMAL + stf(refGoods.RndPriceModify); break;
		case TRADE_TYPE_IMPORT:     tradeModify = TRADE_MOD_IMPORT + stf(refGoods.RndPriceModify); break;
		case TRADE_TYPE_AGGRESSIVE: tradeModify = TRADE_MOD_AGGRESSIVE + stf(refGoods.RndPriceModify); break;
		case TRADE_TYPE_AMMUNITION: tradeModify = TRADE_MOD_NORMAL + stf(refGoods.RndPriceModify); break;
	}
	if (tradeModify < 0.1) return 0;
	return MakeInt(stf(Goods[iGoods].Cost) * tradeModify + 0.5);
}

// лучший товар для перевозки по ЖИВЫМ ценам: из топ-5 пар по прибыли выбор дневным рандомом слота
// (заказ стабилен в течение суток). Возвращает -1, если выгодных пар нет (вызывающий откатится на старый ролл)
int GetCargoLiveGood(int slot, int otype, string sCityFrom, string sCityTo)
{
	ref rFrom = GetCityStoreRef(sCityFrom);
	ref rTo   = GetCityStoreRef(sCityTo);
	int aGood[5];
	int aProf[5];
	int n, g, iBuy, iSell, iProf, j, k;
	bool bIns;
	for (n = 0; n < 5; n++) { aGood[n] = -1; aProf[n] = 0; }
	for (g = GOOD_CHOCOLATE; g <= GOOD_BRICK; g++)
	{
		if (otype == CARGO_PERISH && !CargoGoodPerishable(g)) continue; // скоропортящийся возит только съестное
		iBuy  = GetCargoTradePrice(rFrom, g); // оптовые цены, без игроцкого спреда
		iSell = GetCargoTradePrice(rTo, g);
		if (iBuy <= 0) continue;  // у отправителя такого товара нет (или контрабанда)
		if (iSell <= 0) continue; // адресат таким не торгует (или контрабанда)
		iProf = iSell - iBuy;
		if (iProf <= 0) continue; // перевозка невыгодна
		// вставка в топ-5 по убыванию прибыли
		for (j = 0; j < 5; j++)
		{
			bIns = false;
			if (aGood[j] == -1) bIns = true;
			if (iProf > aProf[j]) bIns = true;
			if (bIns)
			{
				for (k = 4; k > j; k--) { aGood[k] = aGood[k-1]; aProf[k] = aProf[k-1]; }
				aGood[j] = g;
				aProf[j] = iProf;
				break;
			}
		}
	}
	n = 0;
	while (n < 5 && aGood[n] != -1) { n = n + 1; }
	if (n == 0) return -1;
	j = dRandSlotLow(slot, n - 1); // вызов функции в индексе массива Storm не переваривает - по шагам
	return aGood[j];
}

// доля ГГ: 25% от прибыли перевозки (оптовые цены) x типовой множитель. 0 = выгодной пары нет (фоллбек на старую формулу)
int GetCargoLiveMoney(int otype, int iGoods, int qty, string sCityFrom, string sCityTo)
{
	int iBuy  = GetCargoTradePrice(GetCityStoreRef(sCityFrom), iGoods);
	int iSell = GetCargoTradePrice(GetCityStoreRef(sCityTo), iGoods);
	if (iBuy <= 0) return 0;
	int iProf = iSell - iBuy;
	if (iProf <= 0) return 0;
	float payMult = 1.0;
	if (otype == CARGO_SIMPLE)  payMult = 0.8;
	if (otype == CARGO_URGENT)  payMult = 1.4;
	if (otype == CARGO_PERISH)  payMult = 1.55;
	if (otype == CARGO_PREMIUM) payMult = 1.8;
	return makeint(qty * iProf * 0.25 * payMult / sti(Goods[iGoods].Units)); // LEO: считаем по ПАЧКАМ (делим на Units) - иначе товары с большим числом штук в пачке раздували награду и дисбалансили. Запасная GetCargoOrderMoney уже делит на Units. Делим в конце, после float-множителей, чтобы целочисленное деление не обнуляло.
}

// стоимость перевозимого груза по оптовой цене магазина отправителя (показывается в тексте заказа)
int GetCargoLiveCost(int iGoods, int qty, string sCityFrom)
{
	// LEO: цена в магазине - за ПАЧКУ, а qty приходит в ШТУКАХ -> делим на Units.
	// Без этого товары с пачкой >1 (одежда - 10 шт при весе пачки 1) раздували стоимость груза
	// в заказе ровно во столько же раз: 500913 шт одежды показывались как 13 млн вместо 1.3 млн.
	// Делим В КОНЦЕ, после умножения, иначе целочисленное деление съест цену.
	return GetCargoTradePrice(GetCityStoreRef(sCityFrom), iGoods) * qty / sti(Goods[iGoods].Units);
}

// "(груз на N)" для текста заказа; пусто, если стоимость неизвестна
string GetCargoCostNote(int iCost)
{
	if (iCost <= 0) return "";
	if (LanguageGetLanguage() == "russian") return " (груз на " + FindRussianMoneyString(iCost) + ")";
	return " (cargo worth " + FindRussianMoneyString(iCost) + ")";
}

// LEO: как GetCargoCostNote, но ЧИСЛО суммы груза в цвете clr (единица - обычным)
string GetCargoCostNoteClr(int iCost, string clr)
{
	if (iCost <= 0) return "";
	if (LanguageGetLanguage() == "russian") return " (груз на " + FindRussianMoneyStringClr(iCost, clr) + ")";
	return " (cargo worth " + FindRussianMoneyStringClr(iCost, clr) + ")";
}

// топ-N самых дорогих товаров торгового диапазона (какао..кирпич) по базовому прайсу (Goods[].Cost);
// выбор внутри топа - дневным рандомом слота. Губернаторский госзаказ - топ-5, контрабандный фрахт - топ-10.
int GetCargoExpensiveGood(int slot, int topN)
{
	int aGood[10];
	int aCost[10];
	int n, g, c, j, k;
	bool bIns;
	if (topN > 10) topN = 10;
	for (n = 0; n < topN; n++) { aGood[n] = -1; aCost[n] = 0; }
	for (g = GOOD_CHOCOLATE; g <= GOOD_BRICK; g++)
	{
		c = sti(Goods[g].Cost);
		// вставка в топ по убыванию цены
		for (j = 0; j < topN; j++)
		{
			bIns = false;
			if (aGood[j] == -1) bIns = true;
			if (c > aCost[j]) bIns = true;
			if (bIns)
			{
				for (k = topN - 1; k > j; k--) { aGood[k] = aGood[k-1]; aCost[k] = aCost[k-1]; }
				aGood[j] = g;
				aCost[j] = c;
				break;
			}
		}
	}
	j = dRandSlotLow(slot, topN - 1); // вызов функции в индексе массива Storm не переваривает - по шагам
	return aGood[j];
}

// контрабандный товар острова-отправителя: только то, что РЕАЛЬНО контрабанда в этом магазине
// (TradeType CONTRABAND либо canbecontraband=CONTRA_SELL - как в FindContrabandGoods/торговле со смуглерами).
// -1 = контрабандного товара на острове нет (контра-фрахт тогда не выдаётся). Выбор - дневным рандомом слота.
int GetCargoContrabandGood(int slot, string sCityFrom)
{
	ref rFrom = GetCityStoreRef(sCityFrom);
	int aGood[21]; // какао(11)..кирпич(31)
	int n, g, j;
	bool bContra;
	n = 0;
	for (g = GOOD_CHOCOLATE; g <= GOOD_BRICK; g++)
	{
		bContra = false;
		if (GetStoreGoodsType(rFrom, g) == TRADE_TYPE_CONTRABAND_NAME) bContra = true;
		if (GetContrabandGoods(rFrom, g) == CONTRA_SELL) bContra = true;
		if (bContra)
		{
			aGood[n] = g;
			n = n + 1;
		}
	}
	if (n == 0) return -1;
	j = dRandSlotLow(slot, n - 1); // вызов функции в индексе массива Storm не переваривает - по шагам
	return aGood[j];
}

// товар агрессивного спроса (коричневый) магазина-отправителя - для губернаторского госзаказа.
// -1 = такого товара на острове нет (губ.фрахт тогда не выдаётся). Выбор - дневным рандомом слота.
int GetCargoAggressiveGood(int slot, string sCityFrom)
{
	ref rFrom = GetCityStoreRef(sCityFrom);
	int aGood[21]; // какао(11)..кирпич(31)
	int n, g, j;
	string nm;
	n = 0;
	for (g = GOOD_CHOCOLATE; g <= GOOD_BRICK; g++)
	{
		nm = Goods[g].name;
		if (!CheckAttribute(rFrom, "Goods." + nm)) continue;
		if (sti(rFrom.Goods.(nm).TradeType) == TRADE_TYPE_AGGRESSIVE)
		{
			aGood[n] = g;
			n = n + 1;
		}
	}
	if (n == 0) return -1;
	j = dRandSlotLow(slot, n - 1); // вызов функции в индексе массива Storm не переваривает - по шагам
	return aGood[j];
}

// базовая стоимость груза (Goods[].Cost x кол-во) - для пометки "(груз на N)" у контрабанды:
// GetCargoTradePrice по контрабандному товару возвращает 0, поэтому считаем по базовой цене
int GetCargoBaseCost(int iGoods, int qty)
{
	// LEO: то же, что в GetCargoLiveCost - базовая цена тоже за ПАЧКУ, а qty в штуках
	return makeint(Goods[iGoods].Cost) * qty / sti(Goods[iGoods].Units);
}

// товар по типу заказа; slot задаёт дневной поток рандома.
// обычные типы - весь торговый диапазон (какао..кирпич, без рабов/золота);
// скоропортящийся - только съестное (кирпичи не гниют); комендантский - конвой (номинал - порох);
// губернаторский/контрабандный - самые дорогие товары по прайсу.
int GetCargoOrderGood(int otype, int slot)
{
	if (otype == CARGO_FORTAMMO) return GOOD_POWDER;                     // конвой: груз смешанный (на барках), номинал заказа - порох
	if (otype == CARGO_GOVERNOR) return GetCargoExpensiveGood(slot, 5);  // госзаказ - из топ-5 самых дорогих
	if (otype == CARGO_SMUGGLE)  return GetCargoExpensiveGood(slot, 10); // контрабанда - из топ-10 самых дорогих
	if (otype == CARGO_PERISH)
	{
		int r = dRandSlotLow(slot, 9); // 0..9 - продовольственные товары
		if (r == 0) return GOOD_CHOCOLATE; // какао
		if (r == 1) return GOOD_COFFEE;
		if (r == 2) return GOOD_SUGAR;
		if (r == 3) return GOOD_WHEAT;
		if (r == 4) return GOOD_WINE;
		if (r == 5) return GOOD_ALE;
		if (r == 6) return GOOD_CINNAMON;
		if (r == 7) return GOOD_COPRA;
		if (r == 8) return GOOD_PAPRIKA;
		return GOOD_FRUITS;
	}
	return GOOD_CHOCOLATE + dRandSlotLow(slot, GOOD_BRICK - GOOD_CHOCOLATE);
}

// вознаграждение по типу заказа. base сохраняет прежний баланс, payMult задаёт разницу по типам.
int GetCargoOrderMoney(int otype, int qty, int iGoods, int maxDays, int days)
{
	if (days < 1) days = 1;
	float payMult = 1.0;
	if (otype == CARGO_SIMPLE)  payMult = 0.8;
	if (otype == CARGO_URGENT)  payMult = 1.4;
	if (otype == CARGO_PERISH)  payMult = 1.55;
	if (otype == CARGO_PREMIUM) payMult = 1.8;
	if (otype == CARGO_SMUGGLE) payMult = 2.2; // контрабанда - максимальный риск/оплата
	if (otype == CARGO_GOVERNOR) payMult = 1.8; // губернаторский - щедрый госзаказ
	if (otype == CARGO_FORTAMMO) payMult = 1.7; // комендантский - военная поставка, щедро
	int commerceBonus = GetCargoCommerceBonus();
	int base = makeint(qty / sti(Goods[iGoods].Units) * sti(Goods[iGoods].Weight) * maxDays / 2 / days) * commerceBonus;
	return makeint(base * payMult);
}

// =====================================================================================
//  КОМЕНДАНТСКИЙ КОНВОЙ (CARGO_FORTAMMO): 4 казённых судна (универсалы по классу флагмана ГГ, до 3 класса)
//  с офицерами-капитанами идут под вымпелом ГГ. Боеприпасы - ТОЛЬКО на судах конвоя, вперемешку,
//  пороха больше всего (40%): взрывоопасно, конвой нужно беречь. Погружено с запасом: к сдаче
//  требуется 50% груза - допустима потеря даже двух судов. Комендант принимает то, что дошло, плата пропорциональна.
//  Объём и плата НЕ зависят от трюма ГГ. Требование и учёт - в ЕДИНИЦАХ ВЕСА (груз смешанный).
// =====================================================================================
#define FORT_CONVOY_SHIPS 4

// тип корабля конвоя: УНИВЕРСАЛЫ (Type.Merchant + Type.War) по КЛАССУ ФЛАГМАНА ГГ, но не мощнее 3 класса.
// Детерминирован флагманом - сборка заказа, погрузка и плата считают один и тот же тип
// (между показом заказа и взятием корабль ГГ не меняется).
int GetFortConvoyShipType()
{
	int c = GetCharacterShipClass(pchar);
	if (c >= 5) return SHIP_XEBECLIGHT; // ГГ на мелочи 5-7 класса: шебека, 5 класс, ёмкость 1600
	if (c == 4) return SHIP_ROTERLOW;   // 4 класс, ёмкость 3700
	return SHIP_GALEON;                 // ГГ на 1-3 классе: галеон, 3 класс - потолок конвоя, ёмкость 5950
}

// LEO: тип корабля для ГЕНЕРАЦИИ судна конвоя/эскорта - модель ПОД НАЦИЮ работодателя
// (французский фрахт -> французские суда). Класс тот же, что у экономического эталона GetFortConvoyShipType
// (плата/порог считаются от эталона - детерминированно; GetShipTypeExt рандомный, для расчёта не годится).
// "merchant" - транспорт с трюмом под казённый груз; нет нац. купца класса -> любой нац. корабль -> эталонный универсал.
int GetFortConvoyGenShipType(int iNation)
{
	int c = GetCharacterShipClass(pchar);
	int convClass;
	if (c >= 5) convClass = 5;
	else if (c == 4) convClass = 4;
	else convClass = 3;
	int t = GetShipTypeExt(convClass, convClass, "merchant", iNation);
	if (t == INVALID_SHIP_TYPE) t = GetShipTypeExt(convClass, convClass, "", iNation); // нет нац. купца класса - любой нац. корабль того же класса
	if (t == INVALID_SHIP_TYPE) t = GetFortConvoyShipType();                            // совсем нет - откат на эталонный универсал
	return t;
}

// плановый вес боеприпасов на ОДНО судно конвоя (85% ёмкости).
// ВАЖНО: ёмкость берём из ШАБЛОНА ShipsTypes[тип]. RealShips - массив ЭКЗЕМПЛЯРОВ кораблей
// (индекс = ship.type персонажа), RealShips[16] - чужой корабль со случайной ёмкостью:
// эта грабля давала вес конвоя в миллионы и долю в 18M пиастров.
int GetFortConvoyLoadWeight()
{
	int t = GetFortConvoyShipType();
	return makeint(sti(ShipsTypes[t].Capacity) * 0.85);
}

// требуемый к сдаче ВЕС (50% погруженного: допустима потеря даже двух судов конвоя)
int GetFortConvoyRequiredWeight()
{
	return makeint(FORT_CONVOY_SHIPS * GetFortConvoyLoadWeight() * 0.50);
}

// плата конвоя: формула обычного фрахта от ПОЛНОГО веса груза конвоя (не от порога сдачи),
// множитель x3.4 - очень высокая, под стать рискам
int GetFortConvoyMoney(int maxDays, int days)
{
	if (days < 1) days = 1;
	int commerceBonus = GetCargoCommerceBonus();
	int base = makeint(FORT_CONVOY_SHIPS * GetFortConvoyLoadWeight() * maxDays / 2 / days) * commerceBonus;
	return makeint(base * 3.4);
}

// суммарная СТОИМОСТЬ боеприпасов конвоя (для пометки "груз на N" в заказе) - по той же раскладке, что грузит FortConvoy_LoadAmmo
int GetFortConvoyCargoValue()
{
	int loadW = GetFortConvoyLoadWeight();
	int v = 0;
	v = v + GetGoodQuantityByWeight(GOOD_POWDER,   makeint(loadW * 0.40)) * sti(Goods[GOOD_POWDER].Cost);
	v = v + GetGoodQuantityByWeight(GOOD_BALLS,    makeint(loadW * 0.24)) * sti(Goods[GOOD_BALLS].Cost);
	v = v + GetGoodQuantityByWeight(GOOD_GRAPES,   makeint(loadW * 0.12)) * sti(Goods[GOOD_GRAPES].Cost);
	v = v + GetGoodQuantityByWeight(GOOD_KNIPPELS, makeint(loadW * 0.12)) * sti(Goods[GOOD_KNIPPELS].Cost);
	v = v + GetGoodQuantityByWeight(GOOD_BOMBS,    makeint(loadW * 0.12)) * sti(Goods[GOOD_BOMBS].Cost);
	return v * FORT_CONVOY_SHIPS;
}

// догрузка товара конкретному барку (без распределения по эскадре)
void FortConvoy_AddGood(ref rBarque, int iGoods, int q)
{
	string sName = Goods[iGoods].name;
	if (!CheckAttribute(rBarque, "Ship.Cargo.Goods." + sName)) rBarque.Ship.Cargo.Goods.(sName) = 0;
	rBarque.Ship.Cargo.Goods.(sName) = sti(rBarque.Ship.Cargo.Goods.(sName)) + q;
	RecalculateCargoLoad(rBarque);
}

// погрузка боеприпасов на барк: вперемешку, пороха больше всего (40% веса - велик шанс подрыва, береги конвой)
void FortConvoy_LoadAmmo(ref rBarque)
{
	int loadW = GetFortConvoyLoadWeight();    // плановый вес по ШАБЛОНУ (экономика/оплата считаются от него)
	// LEO: но грузим не выше РЕАЛЬНОЙ свободной ёмкости ЭКЗЕМПЛЯРА - модификаторы верфи/материала постройки
	// уменьшают дедвейт сгенерированного судна ниже шаблона, и плановый вес перегружал трюм (Своб < 0).
	int freeW = GetCargoFreeSpace(rBarque);   // свободно с учётом груза по умолчанию + команды
	if (loadW > freeW) loadW = freeW;         // кап: не перегружаем (порог сдачи 50% планового всё равно перекрыт)
	FortConvoy_AddGood(rBarque, GOOD_POWDER,   GetGoodQuantityByWeight(GOOD_POWDER,   makeint(loadW * 0.40)));
	FortConvoy_AddGood(rBarque, GOOD_BALLS,    GetGoodQuantityByWeight(GOOD_BALLS,    makeint(loadW * 0.24)));
	FortConvoy_AddGood(rBarque, GOOD_GRAPES,   GetGoodQuantityByWeight(GOOD_GRAPES,   makeint(loadW * 0.12)));
	FortConvoy_AddGood(rBarque, GOOD_KNIPPELS, GetGoodQuantityByWeight(GOOD_KNIPPELS, makeint(loadW * 0.12)));
	FortConvoy_AddGood(rBarque, GOOD_BOMBS,    GetGoodQuantityByWeight(GOOD_BOMBS,    makeint(loadW * 0.12)));
}

// выдача конвоя при взятии заказа (trade_quest_open): 4 барка с офицерами-капитанами входят в эскадру ГГ
void FortConvoy_Spawn()
{
	ref sld;
	int i;
	int cargoValue = 0;
	int fcIdx = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
	int fcNation = sti(Characters[fcIdx].nation);
	// LEO: тип корабля выбираем ПОД НАЦИЮ работодателя per-ship (нац. суда в конвое); эталон класса/ёмкости - GetFortConvoyShipType
	for (i = 1; i <= FORT_CONVOY_SHIPS; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("FortConvoy_Cap" + i, "off_" + NationShortName(fcNation) + "_" + (rand(1) + 1), "man", "man", sti(pchar.rank), fcNation, -1, true));
		sld.Ship.Type = GenerateShipExt(GetFortConvoyGenShipType(fcNation), true, sld);
		SetRandomNameToShip(sld);
		ResetShipCannonsDamages(sld);
		SetBaseShipData(sld);
		SetCrewQuantityFull(sld);
		SetShipSkill(sld, 30, 10, 40, 40, 60, 30, 20, 40, 10);
		Fantom_SetBalls(sld, "war");
		FantomMakeCoolFighter(sld, sti(pchar.rank), 60, 60, "", "", 50);
		// LEO: было Dialog.TempNode - её dialog.c НЕ читает (ручная ячейка-заначка), поэтому нода оставалась
		//   дефолтной таверной и капитан предлагал наняться. Ставим ОБЕ ноды на квестовую (образец - ПГГ):
		//   судно казённое, идёт под нашей охраной, приказы ему отдавать нельзя - потому не "hired".
		sld.Dialog.CurrentNode = "ConvoyCap_Escort";
		sld.DeckDialogNode     = "ConvoyCap_Escort";
		sld.Dialog.FileName = "Enc_Officer_dialog.c";
		sld.quest.meeting = true;
		sld.CompanionEnemyEnable = false;
		sld.NonRemovable = true; // казённый барк: не продать/не сдать на стоянку, вне фрахтового места/еды/жалованья (GetRemovable)
		sld.OfficerWantToGo.DontGo = true;
		sld.loyality = MAX_LOYALITY;
		SetCompanionIndex(pchar, -1, sti(sld.index));
		cargoValue = cargoValue - FortConvoy_AmmoValue(sld);
		FortConvoy_LoadAmmo(sld);
		cargoValue = cargoValue + FortConvoy_AmmoValue(sld);
	}
	// LEO: стоимость именно поставки, без штатного боезапаса судов; смена флагмана её не меняет.
	pchar.CargoQuest.ConvoyCargoValue = cargoValue;
	RecalculateSquadronCargoLoad(pchar);
}

// барк жив и в эскадре ГГ (потопленные/захваченные выпадают из компаньонов)
bool FortConvoy_InSquadron(string sId)
{
	int cn = GetCharacterIndex(sId);
	if (cn == -1) return false;
	ref b = GetCharacter(cn);
	if (LAi_isDead(b)) return false;
	if (!CheckAttribute(b, "ship.type")) return false;
	if (sti(b.ship.type) == SHIP_NOTUSED) return false;
	int i;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		if (GetCompanionIndex(pchar, i) == cn) return true;
	}
	return false;
}

// число живых судов конвоя в эскадре (для развилки исходов при сдаче)
int FortConvoy_ShipsAlive()
{
	int n = 0;
	for (int i = 1; i <= FORT_CONVOY_SHIPS; i++)
	{
		if (FortConvoy_InSquadron("FortConvoy_Cap" + i)) n = n + 1;
	}
	return n;
}

// суммарный ВЕС боеприпасов (5 видов) на живых барках конвоя - столько дошло до форта
int FortConvoy_AmmoWeightAboard()
{
	int total, i, g, q, cn;
	ref b;
	total = 0;
	for (i = 1; i <= FORT_CONVOY_SHIPS; i++)
	{
		if (!FortConvoy_InSquadron("FortConvoy_Cap" + i)) continue;
		cn = GetCharacterIndex("FortConvoy_Cap" + i);
		b = GetCharacter(cn);
		for (g = GOOD_BALLS; g <= GOOD_POWDER; g++)
		{
			q = GetCargoGoods(b, g);
			total = total + GetGoodWeightByType(g, q);
		}
	}
	return total;
}

// перевести суда конвоя под флаг заданной нации (самовоз к вражескому форту: иначе суда конвоя по своей
// нации враждебны нации-адресату, и форт/сторожевики палят по ним - льгота прохода ГГ их не покрывает).
void FortConvoy_SetNation(int iNation)
{
	int i, cn;
	ref b;
	for (i = 1; i <= FORT_CONVOY_SHIPS; i++)
	{
		cn = GetCharacterIndex("FortConvoy_Cap" + i);
		if (cn < 0) continue;
		b = GetCharacter(cn);
		b.nation = iNation;
		Ship_FlagRefresh(b); // обновить отображаемый флаг под новую нацию
	}
}

// роспуск конвоя после сдачи: барки уходят к форту вместе с остатками груза
void FortConvoy_Disband()
{
	int i, k, cn;
	ref b;
	for (i = 1; i <= FORT_CONVOY_SHIPS; i++)
	{
		cn = GetCharacterIndex("FortConvoy_Cap" + i);
		if (cn == -1) continue;
		b = GetCharacter(cn);
		for (k = 1; k < COMPANION_MAX; k++)
		{
			if (GetCompanionIndex(pchar, k) == cn) SetCompanionIndex(pchar, k, -1);
		}
		DeleteAttribute(b, "NonRemovable");
		ChangeCharacterAddressGroup(b, "None", "", "");
		setCharacterShipLocation(b, "None");
		b.LifeDay = 0;
	}
	RecalculateSquadronCargoLoad(pchar);
}
//ESCORT_ANCHOR
// =============================================================================
//  ЭСКОРТ (подтип 2 губернаторского): 2-4 казённых судна с грузом фрахта.
//  Реюз машинерии комендантского конвоя (FortConvoy_*). Груз - товар губ.фрахта (не боеприпасы),
//  сдача - губернатору, частичная по уцелевшим судам. Морское событие - см. gov-escort-fork.
// =============================================================================

// LEO: пороги разделены по типу, чтобы губернаторский ранг можно было изменить независимо.
int FreightConvoy_MinRank(int otype)
{
	if (otype == CARGO_GOVERNOR) return 4;
	return 4;
}

// LEO: проверяем флагман ГГ, а не сильнейший корабль эскадры.
bool FreightConvoy_CanTake(int otype)
{
	if (sti(pchar.rank) < FreightConvoy_MinRank(otype)) return false;
	if (!CheckAttribute(pchar, "Ship.Type")) return false;
	if (GetCharacterShipType(pchar) == SHIP_NOTUSED) return false;
	int iClass = GetCharacterShipClass(pchar);
	if (iClass < 1 || iClass > 5) return false;
	return true;
}

// LEO: обычный губернаторский груз не является конвоем и новых ограничений не получает.
bool FreightConvoy_CanAccept(int otype, int subtype)
{
	if (otype == CARGO_GOVERNOR && subtype == 2) return GovEscort_CanTake();
	if (otype == CARGO_FORTAMMO)
	{
		if (!FreightConvoy_CanTake(otype)) return false;
		if (COMPANION_MAX - GetCompanionQuantity(pchar) < FORT_CONVOY_SHIPS) return false;
	}
	return true;
}

// LEO: сохранённое предложение проверяется заново до записи контракта и выхода из диалога.
bool FreightConvoy_CanConfirm(ref trader, int slot)
{
	if (CheckQuestAttribute("generate_trade_quest_progress", "begin")) return false;
	if (CheckQuestAttribute("generate_trade_quest_progress", "failed")) return false;
	string aType = "iOrderType" + slot;
	string aSub = "GovSubtype" + slot;
	if (!CheckAttribute(trader, aType)) return false;
	int subtype = 0;
	if (CheckAttribute(trader, aSub)) subtype = sti(trader.(aSub));
	return FreightConvoy_CanAccept(sti(trader.(aType)), subtype);
}

// LEO: эскорт с четвёртого ранга, не более четырёх своих кораблей и флагман класса 1..5.
bool GovEscort_CanTake()
{
	if (!FreightConvoy_CanTake(CARGO_GOVERNOR)) return false;
	if (GetCompanionQuantity(pchar) > 4) return false;
	return true;
}

// LEO: базовая стоимость смешанных боеприпасов на конкретном судне, не премия капитана.
int FortConvoy_AmmoValue(ref ship)
{
	int value = GetCargoBaseCost(GOOD_POWDER, GetCargoGoods(ship, GOOD_POWDER));
	value = value + GetCargoBaseCost(GOOD_BALLS, GetCargoGoods(ship, GOOD_BALLS));
	value = value + GetCargoBaseCost(GOOD_GRAPES, GetCargoGoods(ship, GOOD_GRAPES));
	value = value + GetCargoBaseCost(GOOD_KNIPPELS, GetCargoGoods(ship, GOOD_KNIPPELS));
	value = value + GetCargoBaseCost(GOOD_BOMBS, GetCargoGoods(ship, GOOD_BOMBS));
	return value;
}

// LEO: новые заказы сохраняют стоимость погрузки; старые сейвы читаем без привязки к нынешнему флагману.
int FreightConvoy_CargoValue()
{
	if (CheckAttribute(pchar, "CargoQuest.ConvoyCargoValue")) return sti(pchar.CargoQuest.ConvoyCargoValue);
	if (sti(pchar.CargoQuest.iOrderType) == CARGO_GOVERNOR)
		return GetCargoBaseCost(sti(pchar.CargoQuest.iTradeGoods), sti(pchar.CargoQuest.iQuantityGoods));
	int value = 0;
	int cn;
	for (int i = 1; i <= FORT_CONVOY_SHIPS; i++)
	{
		if (!FortConvoy_InSquadron("FortConvoy_Cap" + i)) continue;
		cn = GetCharacterIndex("FortConvoy_Cap" + i);
		value = value + FortConvoy_AmmoValue(GetCharacter(cn));
	}
	return value;
}

// LEO: ограниченная монотонная шкала по цене груза: 40..200 тысяч дают 20..60 нации и 10..30 чести.
int FreightConvoy_NationPenalty(int value)
{
	if (value < 40000) value = 40000;
	if (value > 200000) value = 200000;
	return 20 + (value - 40000) / 4000;
}

int FreightConvoy_HonourPenalty(int value)
{
	return FreightConvoy_NationPenalty(value) / 2;
}

// число судов эскорта: 2..4, дневной рандом (стабильно при сейв-лоаде)
int GovEscort_ShipQty()
{
	return 2 + dRandSlotLow(1, 2); // 0..2 -> 2..4
}

// премия эскорта: ТА ЖЕ формула, что у комендантского конвоя (щедро, под стать рискам проводки) -
// вес полной загрузки N судов × срок × Коммерция × 3.4. По просьбе юзера: "сделать как в комендантском".
int GetGovEscortMoney(int maxDays, int days)
{
	if (days < 1) days = 1;
	int n = GovEscort_ShipQty();
	int commerceBonus = GetCargoCommerceBonus();
	int base = makeint(n * GetFortConvoyLoadWeight() * maxDays / 2 / days) * commerceBonus;
	return makeint(base * 3.4);
}

// ожидаемый ПОЛНЫЙ объём груза эскорта (полная загрузка N судов) - для "в количестве N" в заказе
int GetGovEscortCargoQty(int g)
{
	int n = GovEscort_ShipQty();
	int capPerShip = sti(ShipsTypes[GetFortConvoyShipType()].Capacity);
	return n * GetGoodQuantityByWeight(g, capPerShip);
}

// СТОИМОСТЬ охраняемого груза эскорта (полная загрузка N судов) - для пометки "груз на N" в заказе
int GetGovEscortCargoValue(int g)
{
	return GetCargoBaseCost(g, GetGovEscortCargoQty(g));
}

// выдача эскорта при взятии заказа: N судов с грузом губ.фрахта входят в эскадру ГГ
void GovEscort_Spawn()
{
	int n = GovEscort_ShipQty();
	pchar.CargoQuest.GovEscortShips = n; // запоминаем, сколько выдали
	int g   = sti(pchar.CargoQuest.iTradeGoods);
	int gIdx = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
	int gNation = sti(Characters[gIdx].nation);
	// LEO: тип корабля - ПОД НАЦИЮ работодателя per-ship (нац. суда в эскорте); эталон класса/ёмкости - GetFortConvoyShipType
	ref sld;
	int i, loaded;
	int total = 0; // суммарно загружено на все суда (важный груз - трюмы под завязку)
	for (i = 1; i <= n; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("GovEscort_Cap" + i, "off_" + NationShortName(gNation) + "_" + (rand(1) + 1), "man", "man", sti(pchar.rank), gNation, -1, true));
		sld.Ship.Type = GenerateShipExt(GetFortConvoyGenShipType(gNation), true, sld);
		SetRandomNameToShip(sld);
		ResetShipCannonsDamages(sld);
		SetBaseShipData(sld);
		SetCrewQuantityFull(sld);
		SetShipSkill(sld, 30, 10, 40, 40, 60, 30, 20, 40, 10);
		Fantom_SetBalls(sld, "war");
		FantomMakeCoolFighter(sld, sti(pchar.rank), 60, 60, "", "", 50);
		// LEO: было Dialog.TempNode - её dialog.c НЕ читает (ручная ячейка-заначка), поэтому нода оставалась
		//   дефолтной таверной и капитан предлагал наняться. Ставим ОБЕ ноды на квестовую (образец - ПГГ):
		//   судно казённое, идёт под нашей охраной, приказы ему отдавать нельзя - потому не "hired".
		sld.Dialog.CurrentNode = "ConvoyCap_Escort";
		sld.DeckDialogNode     = "ConvoyCap_Escort";
		sld.Dialog.FileName = "Enc_Officer_dialog.c";
		sld.quest.meeting = true;
		sld.CompanionEnemyEnable = false;
		sld.NonRemovable = true; // казённое судно: не продать/не сдать, вне фрахтового места/еды/жалованья
		sld.OfficerWantToGo.DontGo = true;
		sld.loyality = MAX_LOYALITY;
		SetCompanionIndex(pchar, -1, sti(sld.index));
		loaded = GetGoodQuantityByWeight(g, GetCargoFreeSpace(sld)); // трюм под завязку грузом фрахта
		if (loaded < 1) loaded = 1;
		FortConvoy_AddGood(sld, g, loaded);
		total = total + loaded;
	}
	pchar.CargoQuest.iQuantityGoods = total; // реальный полный объём - для пропорции сдачи и журнала (выплата от iMoney не меняется)
	pchar.CargoQuest.ConvoyCargoValue = GetCargoBaseCost(g, total);
	RecalculateSquadronCargoLoad(pchar);
}

// живых судов эскорта в эскадре
int GovEscort_ShipsAlive()
{
	int n = 0;
	int total = sti(pchar.CargoQuest.GovEscortShips);
	for (int i = 1; i <= total; i++)
	{
		if (FortConvoy_InSquadron("GovEscort_Cap" + i)) n = n + 1;
	}
	return n;
}

// суммарное кол-во груза на ЖИВЫХ судах эскорта (для частичной сдачи)
int GovEscort_CargoAboard()
{
	int total, i, cn;
	int g = sti(pchar.CargoQuest.iTradeGoods);
	int ships = sti(pchar.CargoQuest.GovEscortShips);
	ref b;
	total = 0;
	for (i = 1; i <= ships; i++)
	{
		if (!FortConvoy_InSquadron("GovEscort_Cap" + i)) continue;
		cn = GetCharacterIndex("GovEscort_Cap" + i);
		b = GetCharacter(cn);
		total = total + GetCargoGoods(b, g);
	}
	return total;
}

// роспуск эскорта после сдачи: суда уходят из эскадры
void GovEscort_Disband()
{
	int i, k, cn;
	ref b;
	int ships = sti(pchar.CargoQuest.GovEscortShips);
	for (i = 1; i <= ships; i++)
	{
		cn = GetCharacterIndex("GovEscort_Cap" + i);
		if (cn == -1) continue;
		b = GetCharacter(cn);
		for (k = 1; k < COMPANION_MAX; k++)
		{
			if (GetCompanionIndex(pchar, k) == cn) SetCompanionIndex(pchar, k, -1);
		}
		DeleteAttribute(b, "NonRemovable");
		ChangeCharacterAddressGroup(b, "None", "", "");
		setCharacterShipLocation(b, "None");
		b.LifeDay = 0;
	}
	RecalculateSquadronCargoLoad(pchar);
}

// сдача эскорта губернатору: оплата ПРОПОРЦИОНАЛЬНО уцелевшему грузу (довёз что осталось),
// репутация/опыт как у фрахта, груз в магазин, роспуск судов, закрытие. Возвращает выплату.
// fMult - множитель премии (1.0 обычная сдача в <Y>; 5.0 если согласился на перенаправление агенту).
int GovEscort_Deliver(ref mayor, float fMult)
{
	int g    = sti(pchar.CargoQuest.iTradeGoods);
	int qty  = sti(pchar.CargoQuest.iQuantityGoods);
	int aboard = GovEscort_CargoAboard();
	if (aboard > qty) aboard = qty;
	
	int reward = sti(pchar.CargoQuest.iMoney);
	bool late = CheckQuestAttribute("generate_trade_quest_progress", "failed");
	bool bEarlyBonus = false; 

	if (late) 
	{
		reward = makeint(reward / 2);
	} 
	else 
	{ 
		if (GetQuestPastDayParam("CargoQuest") * 2 < sti(pchar.CargoQuest.iDaysExpired)) 
		{
			reward = makeint(reward * 1.2);
			bEarlyBonus = true; 
		} 
	}
	reward = makeint(reward * fMult);             // x5 при перенаправлении агенту
	
	// LEO: пропорция через float - целочисленное reward*aboard переполняло int (груз эскорта в тысячах единиц)
	// и давало ОТРИЦАТЕЛЬНУЮ выплату => списание вместо платы (срабатывало даже при aboard==qty: reward*qty рвалось ДО деления)
	int iLostPenalty = 0;
	if (qty > 0)
	{
		float fRatio  = makefloat(aboard) / makefloat(qty);
		float fReward = reward * fRatio;
		iLostPenalty = reward - makeint(fReward);
		reward = makeint(fReward); // пропорция довезённого
	}

	if (late)
	{
		Log_Info(XI_ConvertString("GovEscort_LatePenalty"));
		Log_InfoColored("red", "-" + FindRussianMoneyStringLog(makeint(reward))); // Потеряна половина
	}
	else if (bEarlyBonus)
	{
		int iBonus = makeint(reward - (reward / 1.2));
		Log_Info(XI_ConvertString("GovEscort_EarlyBonus"));
		Log_InfoColored("green", "+" + FindRussianMoneyStringLog(iBonus));
	}

	if (iLostPenalty > 0)
	{
		Log_Info(XI_ConvertString("GovEscort_LostPenalty"));
		Log_InfoColored("red", "-" + FindRussianMoneyStringLog(iLostPenalty));
	}

	Log_Info(XI_ConvertString("GovEscort_TotalReward"));

	string city = mayor.city;
	reward = GovSwap_WithholdDebt(reward, city);  // удержать неустойку прошлой подмены, если числится за городом
	AddMoneyToCharacter(pchar, reward); // <- Движок сам выдаст зеленые цифры итога
	
	ChangeCharacterNationReputation(pchar, sti(mayor.nation), 5);
	ChangeCharacterReputation(pchar, 1);
	int daysMul = GetQuestPastDayParam("CargoQuest") + 1;
	if (late) daysMul = 1;
	AddCharacterExpToSkill(pchar, "Sailing", daysMul * sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_I));
	AddCharacterExpToSkill(pchar, "Leadership", daysMul * sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_C));
	if (aboard > 0)
	{
		ref st = GetColonyByIndex(FindColony(city));
		AddStoreGoods(&stores[sti(st.StoreNum)], g, aboard); // уцелевший груз - в магазин города
	}
	pchar.quest.generate_trade_quest_progress = "";
	pchar.quest.generate_trade_quest.over = "yes";
	ReOpenQuestHeader("DELIVERY_TRADE_QUEST");
	AddQuestRecord("DELIVERY_TRADE_QUEST", "esc2");
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony", XI_ConvertString("Colony" + city + "Gen")); // город сдачи в esc2 (userdata ПОСЛЕ record!)
	GovSwap_Clear(); // распускает суда эскорта (GovEscortActive) + чистит флаги
	CloseQuestHeader("DELIVERY_TRADE_QUEST");
	AchievementsCounter_genquests(1);
	return reward;
}

// Корабль ГГ пришвартован в порту города sCity (как штатная проверка лавочника в Common_Store:
// rColony.from_sea - порт колонии, pchar.location.from_sea - где причален корабль ГГ).
// Гард сдачи фрахтов: пешком/с чужого причала казённый и контрабандный груз не сдают.
bool CargoShipMooredInCity(string sCity)
{
	int iCol = FindColony(sCity);
	if (iCol < 0) return true; // колония не нашлась - не душним
	ref rColony = GetColonyByIndex(iCol);
	if (!CheckAttribute(rColony, "from_sea")) return true;
	if (rColony.from_sea == "") return true; // у города нет порта - проверять нечего
	if (!CheckAttribute(pchar, "location.from_sea")) return false; // корабль не здесь
	if (pchar.location.from_sea == rColony.from_sea) return true;
	if (pchar.location.from_sea == "Shore17") return true; // спец-случай, как у лавочника
	return false;
}

// снятие СЖ-вахт по каперам-перехватчикам (заказ закрыт любым исходом - записи о бое больше не к месту)
void FortHunt_ClearWatch()
{
	DeleteAttribute(pchar, "quest.FortHuntAttack");
	DeleteAttribute(pchar, "quest.FortHuntOver");
	DeleteAttribute(pchar, "questTemp.FortHuntAttacked");
	DeleteAttribute(pchar, "questTemp.FortHuntNation");
}

// локализованная метка типа заказа (показывается в журнале торговца)
string GetCargoOrderName(int otype)
{
	bool bRus = (LanguageGetLanguage() == "russian");
	if (otype == CARGO_PREMIUM)  { if (bRus) return "Особый"; return "Special"; }
	if (otype == CARGO_SMUGGLE)  { if (bRus) return "Контрабанда"; return "Contraband"; }
	if (otype == CARGO_GOVERNOR) { if (bRus) return "Губернаторский"; return "Governor"; }
	if (otype == CARGO_FORTAMMO) { if (bRus) return "Комендантский"; return "Commandant"; }
	if (otype == CARGO_URGENT)   { if (bRus) return "Срочный"; return "Urgent"; }
	if (otype == CARGO_PERISH)   { if (bRus) return "Скоропортящийся"; return "Perishable"; }
	if (otype == CARGO_STANDARD) { if (bRus) return "Обычный"; return "Standard"; }
	if (bRus) return "Простой";
	return "Simple";
}

// Gregg: ЯДРО автозакупки товаров (переехало из interface\store.c, чтобы работало без открытого экрана магазина).
//   store.c - сегмент (LoadSegment), вне экрана его функций нет, а фоновому рейду казначея (Treasurer_PortRun)
//   ядро нужно снаружи. Отличия от прежней версии: магазин приходит параметром (был глобал refStore),
//   пиратскость колонии считается локально (был глобал bIsPirateColony), bSilent убирает прокрутку времени
//   (WaitDate при загрузке локации может перевалить полночь и дёрнуть смену суток), rStat копит сводку.
void Trade_AutobuyGoodsWithTime(ref rChar, ref refStore, bool bSilent, ref rStat, int purchaseMinutes)
{
	int i, iNeedGood, iCost, iStoreGoodQty;

	ref rGood, rTreasurer;
	string sGood;
	int iCurGoodQty, iNeedGoodsQty;
	int iMoneyQty = 0;

	int iMoved = 0; // сколько единиц товара прошло через руки (для сводки)
	rTreasurer = GetPCharTreasurerRef(); // Казначей. Ему даем экспу

	bool bIsPirateColony = false; // было глобалом интерфейса магазина
	if (refStore.Colony != "none")
	{
		int iPirCol = FindColony(refStore.Colony);
		if (iPirCol != -1)
		{
			if (sti(Colonies[iPirCol].Nation) == PIRATE) bIsPirateColony = true;
		}
	}

	for(i = 0; i < GOODS_QUANTITY; i++)
	{
		rGood = &Goods[i];
		sGood = rGood.name;

		iCurGoodQty = GetCargoGoods(rChar, i); // Сколько этого товара есть сейчас
		iNeedGoodsQty = 0;
		if (CheckAttribute(rChar, "TransferGoods.G." + sGood)) iNeedGoodsQty = sti(rChar.TransferGoods.G.(sGood)); // Сколько нужно ВСЕГО данного товара (не докупить!)

		if(iCurGoodQty == iNeedGoodsQty) continue; // ничего не нужно

		if (CheckAttribute(rChar,"TransferGoods.SellRestriction"))
		{
			if (iCurGoodQty > iNeedGoodsQty && !CheckAttribute(pchar, "TradeGoodsRules.NoSell." + sGood))
			{
				if(CheckAttribute(refStore, "goods." + sGood + ".tradetype"))
				{
					if ((refStore.goods.(sGood).tradetype == TRADE_TYPE_CONTRABAND) && !bIsPirateColony) continue;
					if (refStore.goods.(sGood).tradetype == TRADE_TYPE_CANNONS) continue;
				}
				iNeedGood = iCurGoodQty - iNeedGoodsQty; // Столько нужно продать
				if (CheckAttribute(pchar, "CargoQuest.iTradeGoods") && CheckAttribute(pchar, "CargoQuest.iQuantityGoods"))
				{
					if (sti(pchar.CargoQuest.iTradeGoods) == i)
					{
						if (CheckQuestAttribute("generate_trade_quest_progress", "begin") || CheckQuestAttribute("generate_trade_quest_progress", "failed"))
						{
							int iSquadSurplus = GetSquadronGoods(pchar, i) - GetSquadronNeededGoods(pchar, i) - sti(pchar.CargoQuest.iQuantityGoods);
							if (iSquadSurplus < iNeedGood) iNeedGood = iSquadSurplus;
						}
					}
				}
				//Тут не учитывается вариант, что автозакупку можно провести только для одного корабля, и получится излишек, если на других неполное кол-во
				//То есть получается как бы принудительная закупка этого товара для всех кораблей, даже если нажата кнопка только для автозакупок на одном корабле. Не особо важно.
				if (iNeedGood <= 0) continue;

				RemoveCharacterGoodsSelf(rChar, i, iNeedGood); // Забираем только у этого перса
				AddStoreGoods(refStore, i, iNeedGood); // Прибавляем товар в магаз

				iCost = GetStoreGoodsPrice(refStore, i, PRICE_TYPE_SELL, PChar, 1)*iNeedGood/sti(rGood.Units); // Цена товара для продажи

				if (!bSilent) WaitDate("", 0, 0, 0, 0, 1);
				if (bSilent) TradeReport_AddBuy(rStat, sGood, -iNeedGood, iCost, sti(rChar.index), true);
				iMoved += iNeedGood;
				iMoneyQty += iCost;
			}
		}

		if (iCurGoodQty < iNeedGoodsQty && !CheckAttribute(pchar, "TradeGoodsRules.NoBuy." + sGood))
		{
			if(CheckAttribute(refStore, "goods." + sGood + ".tradetype") && refStore.goods.(sGood).tradetype == TRADE_TYPE_CONTRABAND)
			{
				if(!CheckAttribute(rChar, "TransferGoods.BuyContraband")) continue;
			}

			iNeedGood = iNeedGoodsQty - iCurGoodQty; // Столько нужно купить
			iStoreGoodQty = GetStoreGoodsQuantity(refStore, i); // Сколько можем купить (скоко есть в магазе)
			if (iNeedGood > iStoreGoodQty) iNeedGood = iStoreGoodQty; // Хотим купить больше, чем есть в магазе
			int iCapacity = GetGoodQuantityByWeight(i, GetCargoFreeSpace(rChar));
			if (iNeedGood > iCapacity) iNeedGood = iCapacity;
			if (iNeedGood <= 0) continue;

			int iPrice = GetStoreGoodsPrice(refStore, i, PRICE_TYPE_BUY, PChar, 1);
			int iUnits = sti(rGood.Units);
			iCost = iPrice * iNeedGood / iUnits;
			if (sti(PChar.Money) + iMoneyQty >= iCost) // LEO: остаток С УЧЁТОМ уже набранного в этой автозакупке (iMoneyQty<0 за покупки) - не уходим в минус и не даём халявный товар
			{
				iNeedGood = AddCharacterGoodsSimple(rChar, i, iNeedGood);
				if (iNeedGood <= 0) continue;
				iCost = iPrice * iNeedGood / iUnits;
				RemoveStoreGoods(refStore, i, iNeedGood); // Изымаем из магаза
				if (!bSilent) WaitDate("", 0, 0, 0, 0, purchaseMinutes);
				if (bSilent) TradeReport_AddBuy(rStat, sGood, iNeedGood, -iCost, sti(rChar.index), true);
				iMoved += iNeedGood;
				iMoneyQty -= iCost;
			}
		}
	}

	if(iMoneyQty != 0) // Если хоть что-то продали или купили
	{
		// LEO: закупочный рейс компаньона - вспышку денег ГЛУШИМ (расчёт покажет CompanionTravel сам), опыт ГГ/казначею
		//   НЕ капает (иначе автозакупка компаньона фармит Коммерцию); опыт получит компаньон в BuyCommit.
		bool bCompBuy = CheckAttribute(PChar, "CompanionTravel.BuyChrIdx");
		if (bCompBuy) setShowMoneyLogValue(false);
		AddmoneyToCharacter(PChar, iMoneyQty);
		if (!bCompBuy)
		{
			int iExpAdd = MakeInt(abs(iMoneyQty * 0.5) / 800) + rand(1) + 1;
			AddCharacterExpToSkill(rTreasurer, "Commerce", iExpAdd);
			AddCharacterExpToSkill(pchar, "Commerce", iExpAdd);
		}
		TradeStat_Add(rStat, iMoneyQty, iMoved);
	}
}

void Trade_AutobuyGoods(ref rChar, ref refStore, bool bSilent, ref rStat)
{
	Trade_AutobuyGoodsWithTime(rChar, refStore, bSilent, rStat, 1);
}

// ============================================================================================
// Gregg: РЕЙД КАЗНАЧЕЯ ПО ПОРТУ - при заходе в город/порт казначей сам обновляет ассортимент
//   лавки и лоточников и отрабатывает настроенные списки автозакупки (раз в сутки на город).
//   Гейт - продвинутая торговля у казначея (позже вынесем в отдельный перк).
//   Точка входа: LAi_CharacterPostLogin (loc_ai\LAi_login.c), сразу после CreateCitizens -
//   лоточники локации к этому моменту уже созданы, а при загрузке сейва блок не выполняется.
// ============================================================================================

// есть ли в ветке списка хоть одна НЕнулевая позиция.
// В старых сохранениях автозакупка могла заполнить ветку нулями по всем товарам;
// наличие ветки само по себе не означает, что список настроен.
bool TreasurerRun_ListFilled(ref rHolder, string sBranch)
{
	if (!CheckAttribute(rHolder, sBranch)) return false;
	aref arList, arItem;
	makearef(arList, rHolder.(sBranch));
	for (int i = 0; i < GetAttributesNum(arList); i++)
	{
		arItem = GetAttributeN(arList, i);
		if (sti(GetAttributeValue(arItem)) > 0) return true;
	}
	return false;
}

// настроен ли хоть один список закупки (иначе рейд бессмысленный и деньги трогать не за чем)
bool TreasurerRun_HasLists()
{
	int i, cn;
	for (i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(PChar, i);
		if (cn < 0) continue;
		if (TreasurerRun_ListFilled(&Characters[cn], "TransferGoods.G")) return true;
	}
	for (i = 0; i <= MAX_NUM_FIGHTERS; i++)
	{
		cn = GetOfficersIndex(pchar, i);
		if (cn < 0) continue;
		if (TreasurerRun_ListFilled(&Characters[cn], "TransferItems.I")) return true;
	}
	if (Trade_GetCabinChestLocation() >= 0)
	{
		if (TreasurerRun_ListFilled(pchar, "CabinChestTrade.TransferItems.I")) return true;
	}
	return false;
}

// Gregg: порог «дорогой вещи» для витрины. Считаем по БАЗОВОЙ цене предмета, а не по торговой:
// торговая пляшет от Коммерции и перков, так что порог уезжал бы вместе с прокачкой.
#define TREASURER_RARE_PRICE	5000

// стоит ли предмет отдельной строки в отчёте казначея.
// Генерённые клинки/стволы приводятся к базовому предмету (ItemsFromID снимает суффикс), поэтому качество берётся у базы.
bool Trade_IsRareItem(string sItem)
{
	ref rItem = ItemsFromID(sItem);
	if (CheckAttribute(rItem, "quality"))
	{
		if (rItem.quality == "good" || rItem.quality == "excellent") return true; // жёлтые/синие/красные клинки и стволы
	}
	// Карты и книги. У лоточника они появляются только если такой карты у ГГ ещё нет (CheckMainHeroMap)
	// и книга им не прочитана (CheckForBookReady) - то есть в коробе это заведомо что-то новое.
	if (CheckAttribute(rItem, "groupID"))
	{
		if (rItem.groupID == MAPS_ITEM_TYPE) return true;
		if (rItem.groupID == BOOK_ITEM_TYPE) return true;
	}
	if (HasSubStr(sItem, "cirass")) return true;  // кирасы
	if (HasSubStr(sItem, "spyglass3")) return true;
	if (HasSubStr(sItem, "spyglass4")) return true;
	// идолы и обереги (набор как в быстрой продаже - там они помечены талисманами)
	if (sItem == "indian17" || sItem == "indian3" || sItem == "indian5" || sItem == "indian7") return true;
	if (sItem == "indian14" || sItem == "indian12" || sItem == "indian10" || sItem == "indian1") return true;
	// сеть на всё дорогое, что не описано выше: талисманы, дальние подзорные трубы, рюкзаки, крафт-инструменты.
	// GetItemPrice знает и про генерённое оружие (считает по зашитым в id параметрам), и про отсутствие цены (-1).
	if (GetItemPrice(sItem) >= TREASURER_RARE_PRICE) return true;
	return false;
}

// собрать редкости торговца в отчёт. sWhere - откуда (локализованная подпись: лавка / лоточник N)
void TreasurerReport_ScanRare(ref rTrader, string sWhere, ref rStat)
{
	if (!CheckAttribute(rTrader, "items")) return;
	aref arInv, arItem;
	makearef(arInv, rTrader.items);
	string sItem, sRow;
	int n, i, iQty;
	float dmg_min, dmg_max, weight;
	if (!CheckAttribute(rStat, "rare_n")) rStat.rare_n = 0;
	for (i = 0; i < GetAttributesNum(arInv); i++)
	{
		arItem = GetAttributeN(arInv, i);
		sItem = GetAttributeName(arItem);
		iQty = sti(GetAttributeValue(arItem));
		if (iQty < 1) continue;
		if (!Trade_IsRareItem(sItem)) continue;
		n = sti(rStat.rare_n);
		sRow = "r" + n;
		rStat.rare.(sRow).item = sItem;
		rStat.rare.(sRow).qty = iQty;
		rStat.rare.(sRow).where = sWhere;
		rStat.rare.(sRow).price = GetTradeItemPrice(sItem, PRICE_TYPE_BUY);
		if (IsGeneratedItem(sItem)) // у генерённого оружия параметры зашиты в id - вытащим для показа
		{
			GetBladeParams(sItem, &dmg_min, &dmg_max, &weight);
			rStat.rare.(sRow).dmg = makeint(dmg_min) + "-" + makeint(dmg_max); // FloatToString(x,0) оставлял хвостовую точку: "13.-53."
			rStat.rare.(sRow).weight = FloatToString(weight, 1);
		}
		rStat.rare_n = n + 1;
	}
}

// снять прейскурант города в торговую книгу (NullCharacter.PriceList) - то же, что делает выход из экрана лавки.
// Исключения повторяют штатные вызовы SetPriceListByStoreMan из диалогов: Кайман и Панама - особые города,
// колония без нации торговли не ведёт.
void TreasurerRun_TakePriceList(int iColony)
{
	ref rColony = &Colonies[iColony];
	if (!CheckAttribute(rColony, "StoreNum")) return;
	if (sti(rColony.StoreNum) < 0) return;
	if (rColony.id == "Caiman") return;
	if (rColony.id == "Panama") return;
	if (rColony.nation == "none") return;
	SetPriceListByStoreMan(rColony);

	// Лог откладываем: строка, выведенная во время загрузки локации, пропадает - CreateLandActionsEnvironment
	// пересоздаёт окружение лога уже после нас (LogInterface.c, хук EVENT_LOCATION_LOAD).
	pchar.questTemp.TreasurerPrices = rColony.id;
	DoQuestFunctionDelay("Treasurer_LogPrices", 1.0);
}

// отложенное сообщение о снятом прейскуранте (зовётся по имени - обязателен строковый параметр)
void Treasurer_LogPrices(string qName)
{
	if (!CheckAttribute(pchar, "questTemp.TreasurerPrices")) return;
	string sCity = GetCityName(pchar.questTemp.TreasurerPrices);
	DeleteAttribute(pchar, "questTemp.TreasurerPrices");
	// LEO: жёлтым с миганием - сообщение о снятом прейскуранте легко терялось в белом потоке лога
	Log_InfoColoredAnim("yellow", GetConvertStrWithReplace("Variable_treasurer_prices", "Logs.txt", "#space#", " ") + sCity);
}

// обновить ассортимент торговца, если он ещё не обновлялся сегодня.
// ВАЖНО: ключ даты у лавочника и у лоточника РАЗНЫЙ (так гейтят их диалоги) - иначе диалог
// перегенерит ассортимент заново и сотрёт то, что казначей уже разобрал.
void TreasurerRun_RefreshTrader(ref rTrader, bool bShopMan)
{
	if (bShopMan) // лавка: Common_Store.c, узел "items" - npchar.quest.item_date
	{
		if (CheckAttribute(rTrader, "quest.item_date"))
		{
			if (rTrader.quest.item_date == LastSpeakDate()) return;
		}
		GiveItemToTrader(rTrader);
		rTrader.quest.item_date = LastSpeakDate();
		return;
	}
	// лоточник: Common_ItemTrader.c, узел "Trade_exit" - QuestDate.Item_date
	if (!CheckNPCQuestDate(rTrader, "Item_date")) return;
	SetNPCQuestDate(rTrader, "Item_date");
	GiveItemToTrader(rTrader);
}

// закупка припасов у одного торговца: сначала сливаем излишки, потом добираем нехватку
void TreasurerRun_ConsumeAt(ref rTrader, ref rStat)
{
	int i, cn;
	for (i = 0; i <= MAX_NUM_FIGHTERS; i++)
	{
		cn = GetOfficersIndex(pchar, i);
		if (cn < 0) continue;
		Trade_SellExcessConsume(&Characters[cn], rTrader, true, rStat);
	}
	for (i = 0; i <= MAX_NUM_FIGHTERS; i++)
	{
		cn = GetOfficersIndex(pchar, i);
		if (cn < 0) continue;
		Trade_BuyConsume(&Characters[cn], rTrader, true, rStat);
	}
	Trade_AutobuyChest(rTrader, true, rStat);
}

void Treasurer_PortRun(ref loc)
{
	if (!CheckAttribute(loc, "type")) return;
	if (loc.type != "town")
	{
		if (loc.type != "port") return; // только город/порт колонии (без смешанных условий - парсер Storm)
	}
	if (!CheckAttribute(loc, "fastreload")) return;
	int iColony = FindColony(loc.fastreload);
	if (iColony == -1) return;
	if (LAi_IsCapturedLocation) return;                        // штурм/город трупов - не до торговли
	if (!IsPCharHaveTreasurer()) return;                       // казначея нет - некому бегать

	// LEO: рейд идёт в ТОРГОВЫЕ часы, а не по широкому логин-окну NPC (6:00-21:59): лоточники
	//   стоят на улицах с 7 до 20 (LAi_utilites.c), после этого казначею в городе делать нечего.
	float fRunHour = GetTime();
	if (fRunHour < 7.0) return;
	if (fRunHour > 20.0) return;

	// LEO: колония ГГ на Каймане торгует, только когда лавка ДОСТРОЕНА. Без этого казначей закупался
	//   на пустом месте: запись колонии существует с самого начала, а магазина и лавочников ещё нет.
	if (Colonies[iColony].id == "Caiman")
	{
		if (!CheckAttribute(pchar, "ColonyBuilding.Store")) return;
		if (sti(pchar.ColonyBuilding.Store) == 0) return;
		if (CheckAttribute(pchar, "ColonyBuilding.Store.BuildingTime"))
		{
			if (sti(pchar.ColonyBuilding.Store.BuildingTime) != 0) return; // ещё строится
		}
	}

	// БАЗОВАЯ ТОРГОВЛЯ: казначей переписывает прейскурант города в торговую книгу, не заходя в лавку.
	// LEO: тоже РАЗ В СУТКИ на город, отдельным ключом от рейда закупок. Раньше цены снимались на
	//   КАЖДОМ заходе в порт, и лог захлёбывался строкой "Казначей переписал цены" по нескольку раз
	//   за одно посещение - при каждом возврате из локации в локацию.
	if (CheckOfficersPerk(pchar, "BasicCommerce"))
	{
		if (CheckNPCQuestDate(&Colonies[iColony], "TreasurerPrices"))
		{
			SetNPCQuestDate(&Colonies[iColony], "TreasurerPrices");
			TreasurerRun_TakePriceList(iColony);
		}
	}

	// LEO: ДАЛЬШЕ - только по прямому приказу казначею (диалог офицера): обход лавки и лоточников,
	//   закупки по спискам, отчёт. Прейскурант выше приказа НЕ требует - за ним казначей никуда не
	//   ходит, это его пассивная работа при книгах. Флаг на самом офицере, а не в настройках: это
	//   распоряжение конкретному человеку, и смена казначея его не наследует.
	ref rTreasurer = GetPCharTreasurerRef();
	if (!CheckAttribute(rTreasurer, "TreasurerPortRun")) return;   // приказа не было
	if (sti(rTreasurer.TreasurerPortRun) == 0) return;             // приказ отменён

	if (!CheckOfficersPerk(pchar, "AdvancedCommerce")) return; // гейт закупок: продвинутая торговля
	Trace("LEO:TREASURER eligible colony=" + loc.fastreload + " type=" + loc.type + " money=" + pchar.money);
	// LEO: лавка общая на колонию, а лоточники появляются отдельно при входе в порт и город.
	bool bRunStore = CheckNPCQuestDate(&Colonies[iColony], "TreasurerRun");
	bool bRunMerchants = CheckNPCQuestDate(loc, "TreasurerRun");
	Trace("LEO:TREASURER schedule store=" + bRunStore + " street=" + bRunMerchants);
	if (!bRunStore && !bRunMerchants) return;

	int iNation = sti(Colonies[iColony].nation);
	if (iNation != PIRATE)
	{
		if (GetNationRelation(iNation, sti(pchar.nation)) == RELATION_ENEMY) return; // во враждебном порту казначея не пустят
	}

	// LEO: блокада «Чёрных парусов». Пока влиятельная особа запрещает вести с ГГ дела, лавки закрыты
	//   везде, кроме пиратской и своей на Каймане (Common_Store.c, узел «BlueBird == over»). Казначей
	//   этого не знал и спокойно закупался в любом городе. Обойти запрет способен только Андреас Фиклер
	//   со своими связями; с любым другим казначеем рейд по городу отменяется.
	if (CheckAttribute(pchar, "BSStart"))
	{
		if (CheckAttribute(pchar, "questTemp.BlueBird"))
		{
			if (pchar.questTemp.BlueBird == "over")
			{
				if (iNation != PIRATE)
				{
					if (Colonies[iColony].id != "Caiman")
					{
						if (rTreasurer.id != "Andreas_Fickler") return;
					}
				}
			}
		}
	}
	if (!TreasurerRun_HasLists()) return; // списки закупки не настроены

	if (bRunStore) SetNPCQuestDate(&Colonies[iColony], "TreasurerRun"); // LEO: лавку не повторяем при переходе порт-город.

	// LEO: порт и город дополняют один дневной отчёт; новый день, колония или старый отчёт без даты начинают новый.
	bool bContinueReport = false;
	if (!bRunStore)
	{
		if (CheckAttribute(pchar, "TreasurerReport.city"))
		{
			if (CheckAttribute(pchar, "TreasurerReport.date"))
			{
				if (pchar.TreasurerReport.city == Colonies[iColony].id)
				{
					if (pchar.TreasurerReport.date == environment.date.year + " " + LastSpeakDate()) bContinueReport = true;
				}
			}
		}
	}
	if (!bContinueReport) DeleteAttribute(pchar, "TreasurerReport");
	aref arRep;
	makearef(arRep, pchar.TreasurerReport);
	ref rStat; makeref(rStat, arRep);
	if (!bContinueReport)
	{
		rStat.money = 0;
		rStat.cnt = 0;
		rStat.rows = 0;
		rStat.rare_n = 0;
		rStat.city = Colonies[iColony].id;
		rStat.date = environment.date.year + " " + LastSpeakDate();
	}
	DeleteAttribute(rStat, "try"); // LEO: после нового этапа обхода снова даём фейдеру время завершиться.

	// --- лавка города: товары в трюмы + припасы у лавочника ---
	int iStoreNum = -1;
	if (CheckAttribute(&Colonies[iColony], "StoreNum")) iStoreNum = sti(Colonies[iColony].StoreNum);
	Trace("LEO:TREASURER store number=" + iStoreNum + " enabled=" + bRunStore);
	if (bRunStore && iStoreNum >= 0)
	{
		ref rStore = &Stores[iStoreNum];
		// SetQuestGoodsToStore здесь НЕ зовём: она одноразово съедает квестовые триггеры
		// (GenQuest.StoreGoods - напр. дисконт голландской линейки) и рассчитана на открытие магазина игроком.
		// Казначей торгует по обычному прейскуранту, квестовые цены применятся, когда ГГ сам зайдёт в лавку.

		int i, cn;
		for (i = 0; i < COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(PChar, i);
			if (cn < 0) continue; // i==0 - это флагман ГГ; в кнопке "автозакупка на всю эскадру" он пропущен (cn > 0), тут закупаем и его
			if (!GetRemovable(&Characters[cn])) continue;
			Trade_AutobuyGoods(&Characters[cn], rStore, true, rStat);
		}

		// лавочник как продавец предметов (id колонии + "_trader"; у квестовых городов его нет - и не надо)
		int iShop = GetCharacterIndex(Colonies[iColony].id + "_trader");
		Trace("LEO:TREASURER shop colony=" + Colonies[iColony].id + " index=" + iShop + " rows=" + rStat.rows + " money=" + pchar.money);
		if (iShop != -1)
		{
			ref rShopMan = &Characters[iShop];
			TreasurerRun_RefreshTrader(rShopMan, true);
			TreasurerRun_ConsumeAt(rShopMan, rStat);
			TreasurerReport_ScanRare(rShopMan, GetConvertStrWithReplace("Variable_treasurer_shop", "Logs.txt", "#space#", " "), rStat);
			Trace("LEO:TREASURER shop_done rows=" + rStat.rows + " money=" + pchar.money);
		}
	}

	// --- лоточники этой локации ---
	if (bRunMerchants)
	{
		SetNPCQuestDate(loc, "TreasurerRun"); // LEO: отмечаем до закупок, независимо от наличия денег и товара.
		Trace("LEO:TREASURER street rows=" + rStat.rows + " money=" + pchar.money);
		Treasurer_RunMerchants(rStat);
		Trace("LEO:TREASURER street_done rows=" + rStat.rows + " money=" + pchar.money);
	}

	if (sti(rStat.rows) == 0)
	{
		if (sti(rStat.rare_n) == 0) // ни закупок, ни редкостей - показывать нечего
		{
			DeleteAttribute(pchar, "TreasurerReport");
			return;
		}
	}

	// Экран открываем с задержкой: сейчас идёт загрузка локации (фейдер, логин персонажей),
	// интерфейс поверх неё встанет криво. Полторы секунды - и ГГ уже стоит в городе.
	DoQuestFunctionDelay("Treasurer_ShowReport", 1.5);
}

// отложенный запуск отчётного экрана. Зовётся по имени из очереди квест-функций (quests.c: call sFunction(stmp)),
// поэтому обязателен строковый параметр - как у SetPortAlarm и прочих отложенных.
// Пока не догорел фейдер входа в локацию, интерфейс не поднимется (procInterfacePrepare отказывает при живом
// фейдере), поэтому ждём его через ПАРУ имён: перевзвести себя же нельзя - очередь удаляет ветку СРАЗУ ПОСЛЕ
// вызова функции и стёрла бы свежий перевзвод.
void Treasurer_ShowReport(string qName)
{
	Treasurer_TryShowReport("Treasurer_ShowReportWait");
}

void Treasurer_ShowReportWait(string qName)
{
	Treasurer_TryShowReport("Treasurer_ShowReport");
}

void Treasurer_TryShowReport(string sNextName)
{
	if (!CheckAttribute(pchar, "TreasurerReport")) return;

	aref arFader;
	if (GetEntity(arFader, "fader")) // ещё идёт затемнение входа в локацию
	{
		int iTry = 0;
		if (CheckAttribute(pchar, "TreasurerReport.try")) iTry = sti(pchar.TreasurerReport.try);
		if (iTry > 6) return; // не дождались - отчёт останется доступным по кнопке в экране казначея
		pchar.TreasurerReport.try = iTry + 1;
		DoQuestFunctionDelay(sNextName, 1.0);
		return;
	}
	DeleteAttribute(pchar, "TreasurerReport.try");
	LaunchTreasurerReport();
}
