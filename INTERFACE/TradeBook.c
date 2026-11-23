/// BOAL меню торговли
string CurTable, CurRow, sLastTABLE_GOODS;
string sMessageMode;
int iCurSummaryGood = -1;

void InitInterface(string iniName)
{
    InterfaceStack.SelectMenu_node = "LaunchTradeBook"; // запоминаем, что звать по Ф2
	GameInterface.title = "titleTradeBook";
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
    SetEventHandler("ievnt_command","ProcessCommandExecute",0);
    SetEventHandler("TableSelectChange", "TableSelectChange", 0);
    SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
    SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("CheckButtonChange", "CheckButtonChange", 0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
	SetEventHandler("EventGoodsDblClick", "OnGoodsDblClick", 0);

    XI_RegistryExitKey("IExit_F2");
    // шапка -->
    GameInterface.TABLE_CITY.hr.td1.str = GetConvertStrWithReplace("Variable_TradeBook_1", "Interface.txt", "#space#", " ");
    GameInterface.TABLE_CITY.hr.td1.scale = 0.8
	GameInterface.TABLE_CITY.hr.td2.str = GetConvertStrWithReplace("Variable_TradeBook_2", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_CITY.hr.td2.scale = 0.8;
	GameInterface.TABLE_CITY.hr.td3.str = GetConvertStrWithReplace("Variable_TradeBook_3", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_CITY.hr.td3.scale = 0.8;
	GameInterface.TABLE_CITY.hr.td4.str = GetConvertStrWithReplace("Variable_TradeBook_4", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_CITY.hr.td4.scale = 0.8;
    // <--
//--> mod tablesort
	GameInterface.TABLE_CITY.hr.td1.sorttype = "string"; // LEO: включена сортировка по нации (id наций одноразрядные -> строковая = верный порядок)
	GameInterface.TABLE_CITY.hr.td2.sorttype = "string";
	GameInterface.TABLE_CITY.hr.td3.sorttype = "string";
	GameInterface.TABLE_CITY.hr.td4.sorttype = "string";
	GameInterface.TABLE_CITY.hr.td4.sortdir = "dec";//по уменьшению
//<-- mod tablesort

    // шапка -->
    GameInterface.TABLE_GOODS.hr.td1.str = XI_ConvertString("Good name");
    GameInterface.TABLE_GOODS.hr.td1.scale = 0.8;
    GameInterface.TABLE_GOODS.hr.td2.str = GetConvertStrWithReplace("Variable_TradeBook_5", "Interface.txt", "#space#", " ");
    GameInterface.TABLE_GOODS.hr.td2.scale = 0.8;
	GameInterface.TABLE_GOODS.hr.td3.str = XI_ConvertString("Price sell");
	GameInterface.TABLE_GOODS.hr.td3.scale = 0.8;
	GameInterface.TABLE_GOODS.hr.td4.str = XI_ConvertString("Price buy");
	GameInterface.TABLE_GOODS.hr.td4.scale = 0.8;
	GameInterface.TABLE_GOODS.hr.td5.str = XI_ConvertString("In the store");
	GameInterface.TABLE_GOODS.hr.td5.scale = 0.8;
	GameInterface.TABLE_GOODS.hr.td6.str = GetConvertStrWithReplace("Variable_TradeBook_6", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_GOODS.hr.td6.scale = 0.8;
	GameInterface.TABLE_GOODS.hr.td7.str = GetConvertStrWithReplace("Variable_TradeBook_7", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_GOODS.hr.td7.scale = 0.8;
	GameInterface.TABLE_GOODS.hr.td8.str = GetConvertStrWithReplace("Variable_TradeBook_8", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_GOODS.hr.td8.scale = 0.8;
//--> mod tablesort
	GameInterface.TABLE_GOODS.hr.td1.sorttype = "string";
	GameInterface.TABLE_GOODS.hr.td2.sorttype = "";
	GameInterface.TABLE_GOODS.hr.td3.sorttype = "";
	GameInterface.TABLE_GOODS.hr.td4.sorttype = "";
	GameInterface.TABLE_GOODS.hr.td5.sorttype = "";
	GameInterface.TABLE_GOODS.hr.td6.sorttype = "";
	GameInterface.TABLE_GOODS.hr.td7.sorttype = "";
	GameInterface.TABLE_GOODS.hr.td8.sorttype = "";
	GameInterface.TABLE_GOODS.hr.td8.sortdir = "dec";
//<-- mod tablesort
	LoadTradeBookFilters();
	GameInterface.TABLE_SUMMARY.hr.td1.str = GetConvertStrWithReplace("Variable_TradeBook_1", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SUMMARY.hr.td1.scale = 0.68;
	GameInterface.TABLE_SUMMARY.hr.td2.str = GetConvertStrWithReplace("Variable_TradeBook_2", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SUMMARY.hr.td2.scale = 0.68;
	GameInterface.TABLE_SUMMARY.hr.td3.str = GetConvertStrWithReplace("Variable_TradeBook_14", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SUMMARY.hr.td3.scale = 0.68;
	GameInterface.TABLE_SUMMARY.hr.td4.str = GetConvertStrWithReplace("Variable_TradeBook_15", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SUMMARY.hr.td4.scale = 0.68;
	GameInterface.TABLE_SUMMARY.hr.td5.str = GetConvertStrWithReplace("Variable_TradeBook_16", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SUMMARY.hr.td5.scale = 0.68;
	
	GameInterface.TABLE_SUMMARY.hr.td1.sorttype = "string";
	GameInterface.TABLE_SUMMARY.hr.td2.sorttype = "string";
	GameInterface.TABLE_SUMMARY.hr.td3.sorttype = "";
	GameInterface.TABLE_SUMMARY.hr.td4.sorttype = "";
	GameInterface.TABLE_SUMMARY.hr.td5.sorttype = "";
	sMessageMode = "";
	iCurSummaryGood = -1;   // LEO: журнал могли закрыть прямо со сводкой - иначе подсказка покажет прошлый товар
	string sDate = " ";
	sDate = GetDateString();
	string sTime;
	sTime = GetTimeString();
	SetFormatedText("CURRENT_DATE_CAPTION", sDate + " "+ sTime);
	GameInterface.TABLE_CITY.hr.sortedColumn = 1; // LEO: журнал торговли по умолчанию сортируется по нациям (td1 = номер нации)
  FillPriceListTown("TABLE_CITY");
	XI_WindowShow("SUMMARY_WINDOW", false);
	XI_WindowDisable("SUMMARY_WINDOW", true);
}

void ProcessExitCancel()
{
	// ESC при открытой сводке закрывает её, а не весь журнал
	if (sMessageMode == "SummaryWindow")
	{
		HideGoodSummaryTable();
		return;
	}
	
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
    DelEventHandler("ievnt_command","ProcessCommandExecute");
    DelEventHandler("TableSelectChange", "TableSelectChange");
    DelEventHandler("ShowInfoWindow","ShowInfoWindow");
    DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("CheckButtonChange", "CheckButtonChange");
	DelEventHandler("OnTableClick", "OnTableClick");
	DelEventHandler("EventGoodsDblClick", "OnGoodsDblClick");

	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"ReloadMenuExit"))
	{
        DeleteAttribute(&InterfaceStates,"ReloadMenuExit");
		EndCancelInterface(false);
	}
	else
	{
		EndCancelInterface(true);
	}
}
void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	if (sMessageMode == "SummaryWindow")
	{
		// команды фоновых таблиц гасим: под сводкой они прокручиваться не должны
		if (nodName == "TABLE_CITY" || nodName == "TABLE_GOODS" || nodName == "SCROLL_CITY" || nodName == "SCROLL_GOODS")
		{
			if (comName == "upstep" || comName == "downstep" || comName == "click" || comName == "action") return;
		}
	}

	if (nodName == "BTN_SUMMARY_CLOSE")
	{
		if (comName == "click")
		{
			HideGoodSummaryTable();
			return;
		}
	}
    switch(nodName)
	{
		case "I_CHARACTER_2":
			if(comName=="click")
			{
			    nodName = "I_CHARACTER";
			}
		break;
		case "I_SHIP_2":
			if(comName=="click")
			{
			    nodName = "I_SHIP";
			}
		break;
		case "I_QUESTBOOK_2":
			if(comName=="click")
			{
			    nodName = "I_QUESTBOOK";
			}
		break;
		case "I_TRADEBOOK_2":
			if(comName=="click")
			{
			    nodName = "I_TRADEBOOK";
			}
		break;
		case "I_NATIONS_2":
			if(comName=="click")
			{
			    nodName = "I_NATIONS";
			}
		break;
		case "I_ITEMS_2":
			if(comName=="click")
			{
			    nodName = "I_ITEMS";
			}
		break;
	}
	// boal new menu 31.12.04 -->
	if (nodName == "I_CHARACTER" || nodName == "I_SHIP" ||
	    nodName == "I_QUESTBOOK" || nodName == "I_TRADEBOOK" ||
		nodName == "I_NATIONS" || nodName == "I_ITEMS")
	{
		if(comName=="click")
		{
            InterfaceStates.ReloadMenuExit = true;
			IDoExit(RC_INTERFACE_ANY_EXIT);
			PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
			return;
		}
	}
	// boal new menu 31.12.04 -->
}
//  таблица: город, местоположение, актуальность
void FillPriceListTown(string _tabName)
{
	string  cityId, attr2, firstId, sAltD;
    int     i, cn, n, iTemp;
    ref     nulChr = &NullCharacter;
    string  row;
    aref    rootItems;
    aref    curItem;
    ref     rCity;
    
    makearef(rootItems, nulChr.PriceList);  // тут живут ИД города и служ. инфа.
    n = 1;
    firstId = "";
    for (i=0; i<GetAttributesNum(rootItems); i++)
    {
        row = "tr" + n;
		curItem = GetAttributeN(rootItems, i);
		cityId = GetAttributeName(curItem);
		cn = FindColony(cityId);
		if (cn != -1)
		{
			rCity = GetColonyByIndex(cn);
			if (n == 1) firstId = cityId;
			GameInterface.(_tabName).(row).index = i;//<-- mod tablesort
			GameInterface.(_tabName).(row).UserData.CityID  = cityId;
			GameInterface.(_tabName).(row).UserData.CityIDX = cn;
			GameInterface.(_tabName).(row).td1.icon.group  = "NATIONS";
			GameInterface.(_tabName).(row).td1.icon.image  = Nations[sti(rCity.nation)].Name;
			GameInterface.(_tabName).(row).td1.icon.width  = 40;
		    GameInterface.(_tabName).(row).td1.icon.height = 40;
		    GameInterface.(_tabName).(row).td1.icon.offset = "-5, 0";
			GameInterface.(_tabName).(row).td1.str = rCity.nation;//<-- mod tablesort
			GameInterface.(_tabName).(row).td1.scale = 0.0;//невидимая цифра для сортировки
			GameInterface.(_tabName).(row).td2.str = GetConvertStr(cityId + " Town", "LocLables.txt");
			GameInterface.(_tabName).(row).td2.scale = 0.8;
			GameInterface.(_tabName).(row).td3.str = GetConvertStr(rCity.islandLable, "LocLables.txt");
			GameInterface.(_tabName).(row).td3.scale = 0.8;
			GameInterface.(_tabName).(row).td4.scale = 0.8;
			if (CheckAttribute(nulChr, "PriceList." + cityId + ".AltDate"))
		    {
		        sAltD = nulChr.PriceList.(cityId).AltDate; // хранимый формат "ГГГГ.ММ.ДД ЧЧ:ММ" не трогаем (по нему парсит GetPastDayFromAltDate)
		        // LEO: отображение - сверху ЧАСЫ:МИНУТЫ, снизу день.месяц.год (ячейка переносит по пробелу). Хранимый формат
		        //   "ГГГГ.ММ.ДД ЧЧ:ММ": час=strcut(11,12), мин=strcut(14,15). Раньше стояли наоборот (мин:час -> "46:11" при 11:46).
		        GameInterface.(_tabName).(row).td4.str = strcut(sAltD,11,12) + ":" + strcut(sAltD,14,15) + " " + strcut(sAltD,8,9) + "." + strcut(sAltD,5,6) + "." + strcut(sAltD,0,3);
				iTemp = GetPastDayFromAltDate(sAltD);
				if (iTemp > 192) iTemp = 192;//больше 192 дней изменений нет
				iTemp = iTemp/3;
				if (iTemp > 32) GameInterface.(_tabName).(row).td4.color = argb(255,255,255-8*(iTemp-33),255-8*(iTemp-33)); 
				if (iTemp < 32) GameInterface.(_tabName).(row).td4.color = argb(255,255-8*(31-iTemp),255,255-8*(33-iTemp)); 
		    }
		    else
		    {
		        GameInterface.(_tabName).(row).td4.str = "??.??.????";
		    }
			n++;
		}
	}
	GameInterface.(_tabName).select = 0;
	// LEO: применить дефолтную/текущую сортировку (по нации при открытии) — как в FillPriceList для TABLE_GOODS
	if (checkattribute(&GameInterface, _tabName + ".hr.sortedColumn"))
	{
		int nColumnTown = sti(GameInterface.(_tabName).hr.sortedColumn);
		DeleteAttribute(&GameInterface, _tabName + ".hr.sortedColumn");
		SortTable(_tabName, nColumnTown);
	}
	Table_UpdateWindow(_tabName);
	FillPriceList("TABLE_GOODS", firstId);
}

void FillPriceList(string _tabName, string attr1)// картинка, название, картинка экспорта, продажа, покупка, колво, пачка, вес пачки
{
    Table_Clear(_tabName, false, true, false);
	sLastTABLE_GOODS = attr1;
    string  sGoods, sTemp;
    int     i, n, iTradeType;
    ref     nulChr = &NullCharacter;
    string  row;
	if (attr1 != "")
	{
	    n = 1;
	    for (i = 0; i < GOODS_QUANTITY; i++)
	    {
	        row = "tr" + n;
	        sGoods = "Gidx" + i;
			iTradeType = sti(nulChr.PriceList.(attr1).(sGoods).TradeType);
			if (iTradeType == TRADE_TYPE_CANNONS) continue;//пушки не показываем
			sTemp = iTradeType;
			if (iTradeType == 6) sTemp = "4";
			if (iTradeType == 4 || iTradeType == 5) sTemp = "0";//расходники имеют тип 4 и сортируются между контрой и агрессивным, портя восприятие

			if (sti(nulChr.SaveTradeBookFilters.(sTemp)) == false) continue; // выключен фильтр этого типа

			GameInterface.(_tabName).(row).index = i;//<-- mod tablesort
            GameInterface.(_tabName).(row).UserData.ID = Goods[i].name;
            GameInterface.(_tabName).(row).UserData.IDX = i;

	        GameInterface.(_tabName).(row).td1.icon.group = "GOODS";
			GameInterface.(_tabName).(row).td1.icon.image = Goods[i].name;
			GameInterface.(_tabName).(row).td1.icon.offset = "0, 0";
			GameInterface.(_tabName).(row).td1.icon.width = 20;
			GameInterface.(_tabName).(row).td1.icon.height = 20;
			GameInterface.(_tabName).(row).td1.textoffset = "20,0";
			GameInterface.(_tabName).(row).td1.str = XI_ConvertString(Goods[i].name);
			GameInterface.(_tabName).(row).td1.scale = 0.85;

	        GameInterface.(_tabName).(row).td2.icon.group = "TRADE_TYPE";
			GameInterface.(_tabName).(row).td2.icon.image = "ico_" + iTradeType;
			GameInterface.(_tabName).(row).td2.icon.offset = "1, 0";
			GameInterface.(_tabName).(row).td2.icon.width = 18;
			GameInterface.(_tabName).(row).td2.icon.height = 18;

			GameInterface.(_tabName).(row).td2.str = sTemp;//<-- mod tablesort
			GameInterface.(_tabName).(row).td2.scale = 0.0;//невидимая цифра для сортировки
	        if (CheckAttribute(nulChr, "PriceList." + attr1 + "." + sGoods + ".Buy"))
	        {
	            GameInterface.(_tabName).(row).td3.str = nulChr.PriceList.(attr1).(sGoods).Buy;
	        }
	        else
	        {
	            GameInterface.(_tabName).(row).td3.str = "???";
	        }
	        if (CheckAttribute(nulChr, "PriceList." + attr1 + "." + sGoods + ".Sell"))
	        {
	            GameInterface.(_tabName).(row).td4.str = nulChr.PriceList.(attr1).(sGoods).Sell;
	        }
	        else
	        {
	            GameInterface.(_tabName).(row).td4.str = "???";
	        }
	        if (CheckAttribute(nulChr, "PriceList." + attr1 + "." + sGoods + ".Qty"))
	        {
	            GameInterface.(_tabName).(row).td5.str = nulChr.PriceList.(attr1).(sGoods).Qty;
	        }
	        else
	        {
	            GameInterface.(_tabName).(row).td5.str = "????";
	        }
	        GameInterface.(_tabName).(row).td6.str = Goods[i].Units;
			GameInterface.(_tabName).(row).td7.str = Goods[i].Weight;
			GameInterface.(_tabName).(row).td8.str = GetGoodWeightByType(i, sti(nulChr.PriceList.(attr1).(sGoods).Qty));
	        n++;
	    }
    }
	GameInterface.(_tabName).select = 0;
	if (checkattribute(&GameInterface, _tabName + ".hr.sortedColumn"))//таблица уже сортировалось ранее
	{
		int nColumn = sti(GameInterface.(_tabName).hr.sortedColumn);
		DeleteAttribute(&GameInterface, _tabName + ".hr.sortedColumn");//стираем старую сортировку, чтобы запустить именно сортировку, а не реверс
		SortTable(_tabName, nColumn);//сохраняем сортировку
	}
    Table_UpdateWindow(_tabName);
}

void TableSelectChange()
{
//не присылает номер колонки, а номер строки с единицы начинает, на заголовке не срабатывает
	string sControl = GetEventData();
	int iRow = GetEventData();
    CurTable = sControl;
    CurRow   = "tr" + (iRow);
	
    if (CurTable == "TABLE_CITY" && sLastTABLE_GOODS != GameInterface.(CurTable).(CurRow).UserData.CityID)//перерисуем прайс
    {
		Table_Clear("TABLE_GOODS", false, true, false);
		FillPriceList("TABLE_GOODS", GameInterface.(CurTable).(CurRow).UserData.CityID);
    }
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;
	int iItem;

	sPicture = "";
	string sAttributeName;
	int nChooseNum = -1;
	switch (sCurrentNode)
	{
		case "TABLE_GOODS":
		    sGroup = "GOODS";
		    sGroupPicture = GameInterface.(CurTable).(CurRow).UserData.ID;
		    sHeader = XI_ConvertString(GameInterface.(CurTable).(CurRow).UserData.ID);
		    iItem = sti(GameInterface.(CurTable).(CurRow).UserData.IDX);
		    sText1  = GetAssembledString(GetConvertStr(GameInterface.(CurTable).(CurRow).UserData.ID + "_descr", "GoodsDescribe.txt"), &Goods[iItem]);
			sText2 = GetConvertStrWithReplace("Variable_TradeBook_9", "Interface.txt", "#space#", " ") + newStr() +
				 GetConvertStrWithReplace("Variable_TradeBook_10", "Interface.txt", "#space#", " ") + newStr() +
				 GetConvertStrWithReplace("Variable_TradeBook_11", "Interface.txt", "#space#", " ") + newStr() +
				 GetConvertStrWithReplace("Variable_TradeBook_12", "Interface.txt", "#space#", " ") + newStr() +
				 GetConvertStrWithReplace("Variable_TradeBook_13", "Interface.txt", "#space#", " ");
		break;
		case "TABLE_SUMMARY":
			if (iCurSummaryGood != -1)
			{
				iItem = iCurSummaryGood;
				sGroup = "GOODS";
				sGroupPicture = Goods[iItem].name;
				sHeader = XI_ConvertString(Goods[iItem].name);
				sText1 = GetAssembledString(GetConvertStr(Goods[iItem].name + "_descr", "GoodsDescribe.txt"), &Goods[iItem]);
				sText2 = GetConvertStrWithReplace("Variable_TradeBook_18", "Interface.txt", "#space#", " ") + newStr() +
					 GetConvertStrWithReplace("Variable_TradeBook_19", "Interface.txt", "#space#", " ") + newStr() +
					 GetConvertStrWithReplace("Variable_TradeBook_20", "Interface.txt", "#space#", " ") + newStr() +
					 GetConvertStrWithReplace("Variable_TradeBook_21", "Interface.txt", "#space#", " ");
			}
		break;
	}
	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,220,220,220), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);

}

void HideInfoWindow()
{
	CloseTooltip();
}

void OnTableClick()
{
//TableSelectChange срабатывает раньше OnTableClick
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();

	if (!SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sControl, 1 )) 
	{
		SortTable(sControl, iColumn);//<-- mod tablesort
		Table_UpdateWindow(sControl);
	}
}

void CheckButtonChange()
{
	string sControl = GetEventData();
	int iSelectedCB = GetEventData();
	int iNewState = GetEventData();
//========================================//
	if (sControl == "CHECK_GOODS_TYPE")	//тут чисто визуал, фильтры храню в атрибуте и при заполнении его проверяю, а не статус элемента интерфейса
	{
		ref nulChr = &NullCharacter;
		aref aNC;
		string sTemp = iSelectedCB - 1;
		if (sti(nulChr.SaveTradeBookFilters.(sTemp)) != iNewState) 
		{
			nulChr.SaveTradeBookFilters.(sTemp) = iNewState;
			FillPriceList("TABLE_GOODS", sLastTABLE_GOODS);	
		}
		return;
	}
}
void LoadTradeBookFilters()
{
	ref nulChr = &NullCharacter;
	if(!checkattribute(nulChr, "SaveTradeBookFilters")) //При первом использовании - все виды товаров включены
	{
		nulChr.SaveTradeBookFilters.0 = 1;
		nulChr.SaveTradeBookFilters.1 = 1;
		nulChr.SaveTradeBookFilters.2 = 1;
		nulChr.SaveTradeBookFilters.3 = 1;
		nulChr.SaveTradeBookFilters.4 = 1;
	}
	for (int i=0;i<5;i++)
	{
		iTemp = sti(nulChr.SaveTradeBookFilters.(i));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_GOODS_TYPE", 2, i+1, iTemp);//вызывает CheckButtonChange, к сожалению
	}
}

int GetPastDayFromAltDate(string AltDate)
{
	int year = sti(strcut(AltDate, 0, 3));
	int month = sti(strcut(AltDate, 5, 6));
	int day = sti(strcut(AltDate, 8, 9));
	return GetPastTime("day", year, month, day, 0,GetDataYear(), GetDataMonth(), GetDataDay(), 0);
}


void OnGoodsDblClick()
{
	// сводка уже открыта - двойные клики по фону игнорируем
	if (sMessageMode == "SummaryWindow") return; 

	if (CurRow != "" && CurTable == "TABLE_GOODS")
	{
		string sAttr = CurTable + "." + CurRow + ".UserData.IDX";
		if (CheckAttribute(&GameInterface, sAttr))
		{
			int iGoodIdx = sti(GameInterface.(CurTable).(CurRow).UserData.IDX);
			ShowGoodSummaryTable(iGoodIdx);
		}
	}
}


void ShowGoodSummaryTable(int iGoodIdx)
{
	iCurSummaryGood = iGoodIdx;
	string  cityId;
	int     i, cn, n;
	ref     nulChr = &NullCharacter;
	string  row;
	aref    rootItems, curItem;
	ref     rCity;
	
	string sGoods = "Gidx" + iGoodIdx;
	string sTabName = "TABLE_SUMMARY";

	Table_Clear(sTabName, false, true, false);
	sMessageMode = "SummaryWindow"; 

	string sGoodName = XI_ConvertString(Goods[iGoodIdx].name);
	string sSummaryTitle = GetConvertStrWithReplace("Variable_TradeBook_17", "Interface.txt", "#space#", " ");
	SetFormatedText("SUMMARY_GOOD_NAME", sSummaryTitle + sGoodName);
	SetNewGroupPicture("SUMMARY_GOOD_ICON", "GOODS", Goods[iGoodIdx].name);
	
	makearef(rootItems, nulChr.PriceList);
	
	int maxSellPrice = 0;
	int minBuyPrice = 999999; 
	int currentSellPrice = 0;
	int currentBuyPrice = 0;
	
	for (i = 0; i < GetAttributesNum(rootItems); i++)
	{
		curItem = GetAttributeN(rootItems, i);
		cityId = GetAttributeName(curItem);
		
		if (CheckAttribute(nulChr, "PriceList." + cityId + "." + sGoods + ".Sell"))
		{
			currentSellPrice = sti(nulChr.PriceList.(cityId).(sGoods).Sell);
			if (currentSellPrice > maxSellPrice) maxSellPrice = currentSellPrice;
		}
		if (CheckAttribute(nulChr, "PriceList." + cityId + "." + sGoods + ".Buy"))
		{
			currentBuyPrice = sti(nulChr.PriceList.(cityId).(sGoods).Buy);
			if (currentBuyPrice > 0 && currentBuyPrice < minBuyPrice) minBuyPrice = currentBuyPrice;
		}
	}
	
	n = 1;
	for (i = 0; i < GetAttributesNum(rootItems); i++)
	{
		curItem = GetAttributeN(rootItems, i);
		cityId = GetAttributeName(curItem);
		cn = FindColony(cityId);
		
		if (cn != -1)
		{
			rCity = GetColonyByIndex(cn);
			row = "tr" + n;
			
			GameInterface.(sTabName).(row).td1.icon.group = "NATIONS";
			GameInterface.(sTabName).(row).td1.icon.image = Nations[sti(rCity.nation)].Name;
			// LEO: строка сводки - 23 пикселя, поэтому щит 30x30 выпирал за неё и лез на рамку слева.
			//   Размер под высоту строки, offset по X центрирует в колонке (45 - 20)/2, а valign
			//   доверяем движку: он сам считает вертикаль от реальной высоты строки.
			GameInterface.(sTabName).(row).td1.icon.width = 20;
			GameInterface.(sTabName).(row).td1.icon.height = 20;
			GameInterface.(sTabName).(row).td1.icon.offset = "12, 0";
			GameInterface.(sTabName).(row).td1.icon.valign = "center";
			GameInterface.(sTabName).(row).td1.str = rCity.nation;
			GameInterface.(sTabName).(row).td1.scale = 0.0;
			
			GameInterface.(sTabName).(row).td2.str = GetConvertStr(cityId + " Town", "LocLables.txt");
			GameInterface.(sTabName).(row).td2.scale = 0.85;
			
			int buyP = 0;
			int sellP = 0;
			if (CheckAttribute(nulChr, "PriceList." + cityId + "." + sGoods + ".Buy")) buyP = sti(nulChr.PriceList.(cityId).(sGoods).Buy);
			if (CheckAttribute(nulChr, "PriceList." + cityId + "." + sGoods + ".Sell")) sellP = sti(nulChr.PriceList.(cityId).(sGoods).Sell);

			if (buyP > 0)
			{
				GameInterface.(sTabName).(row).td3.str = buyP;
				if (minBuyPrice < 999999 && buyP <= (minBuyPrice * 105 / 100))
				{
					GameInterface.(sTabName).(row).td3.color = argb(255, 50, 255, 50); 
				}
				else
				{
					GameInterface.(sTabName).(row).td3.color = argb(255, 255, 255, 255); 
				}
			}
			else
			{
				GameInterface.(sTabName).(row).td3.str = "???";
				GameInterface.(sTabName).(row).td3.color = argb(255, 130, 130, 130); 
			}
				
			if (sellP > 0)
			{
				GameInterface.(sTabName).(row).td4.str = sellP;
				if (maxSellPrice > 0 && sellP >= (maxSellPrice * 95 / 100))
				{
					GameInterface.(sTabName).(row).td4.color = argb(255, 255, 160, 0); 
				}
				else
				{
					GameInterface.(sTabName).(row).td4.color = argb(255, 255, 255, 255); 
				}
			}
			else
			{
				GameInterface.(sTabName).(row).td4.str = "???";
				GameInterface.(sTabName).(row).td4.color = argb(255, 130, 130, 130); 
			}
			
			if (buyP > 0 || sellP > 0)
			{
				if (CheckAttribute(nulChr, "PriceList." + cityId + "." + sGoods + ".Qty"))
				{
					GameInterface.(sTabName).(row).td5.str = nulChr.PriceList.(cityId).(sGoods).Qty;
				}
				else
				{
					GameInterface.(sTabName).(row).td5.str = "????";
					GameInterface.(sTabName).(row).td5.color = argb(255, 130, 130, 130); 
				}
			}
			else
			{
				GameInterface.(sTabName).(row).td5.str = "???";
				GameInterface.(sTabName).(row).td5.color = argb(255, 130, 130, 130);
			}

			n++;
		}
	}
	
	XI_WindowShow("SUMMARY_WINDOW", true);
	XI_WindowDisable("SUMMARY_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	
	SetNodeUsing("TABLE_GOODS", false);
	SetNodeUsing("TABLE_CITY", false);
	SetNodeUsing("SCROLL_GOODS", false);
	SetNodeUsing("SCROLL_CITY", false);
	SetNodeUsing("CHECK_GOODS_TYPE", false);
	
	SetCurrentNode("TABLE_SUMMARY"); 
	Table_UpdateWindow(sTabName);
}


void HideGoodSummaryTable()
{
	iCurSummaryGood = -1;
	sMessageMode = ""; 
	XI_WindowShow("SUMMARY_WINDOW", false);
	XI_WindowDisable("SUMMARY_WINDOW", true);
	
	XI_WindowDisable("MAIN_WINDOW", false); 
	
	// и возвращаем фоновые таблицы на экран
	SetNodeUsing("TABLE_GOODS", true);
	SetNodeUsing("TABLE_CITY", true);
	SetNodeUsing("SCROLL_GOODS", true);
	SetNodeUsing("SCROLL_CITY", true);
	SetNodeUsing("CHECK_GOODS_TYPE", true);
	
	SetCurrentNode("TABLE_GOODS"); 
}
