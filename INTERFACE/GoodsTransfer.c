// Интерфейс закупки товара казначеем
int	nCurScrollNumS = 0;
int iCurCompanion = -1;
int iCurGoodIndex;
string sCurGoodRow;

int	nCurScrollNumC = 0;
int iCurFighter = 0;
string sCurItemRow, sCurItemId;

int iITEMS_SET = 1;
int iGOODS_SET = 1;
string sPresetPreviewReturnNode = "";
bool bGoodRuleClickHandled;
bool bDoubleGoodClick;
int iLastGoodClickRow = -1;
int iLastGoodClickColumn = -1;

bool isGoodTable;
float curBuyGoodsWeight;
float curBuyItemsWeight;

bool bChestSel = false;

// Офицеры хранят свои списки на персонаже. Общий список первого сундука всех кают принадлежит капитану.
ref TI_SettingsOwner()
{
	if (bChestSel) return pchar;
	return &Characters[iCurFighter];
}

string TI_SettingsPath()
{
	if (bChestSel) return "CabinChestTrade.TransferItems";
	return "TransferItems";
}

int TI_GetCount(string sItem)
{
	ref holder = TI_SettingsOwner();
	string sPath = TI_SettingsPath() + ".I." + sItem;
	if (CheckAttribute(holder, sPath)) return sti(holder.(sPath));
	return 0;
}

void TI_SetCount(string sItem, int iNum)
{
	ref holder = TI_SettingsOwner();
	string sPath = TI_SettingsPath() + ".I." + sItem;
	if (iNum == 0) DeleteAttribute(holder, sPath);
	else holder.(sPath) = iNum;
}

bool TI_CheckFlag(string sFlag)
{
	ref holder = TI_SettingsOwner();
	return CheckAttribute(holder, TI_SettingsPath() + "." + sFlag);
}

void TI_SetFlag(string sFlag, bool bOn)
{
	ref holder = TI_SettingsOwner();
	string sPath = TI_SettingsPath() + "." + sFlag;
	if (bOn == CheckAttribute(holder, sPath)) return;
	if (bOn) holder.(sPath) = true;
	else DeleteAttribute(holder, sPath);
}

void TI_ClearAll()
{
	ref holder = TI_SettingsOwner();
	string sPath = TI_SettingsPath();
	DeleteAttribute(holder, sPath);
	// Пустой список после сброса сохраняет выбор игрока при переходе в старую каюту.
	if (bChestSel) holder.(sPath) = true;
}

void InitInterface(string iniName)
{
	FillShipsScroll();
	FillCharactersScroll();
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);//если выше ставить, не показываются портретики офицеров
	FillItemsTable();
	FillGoodsTable();
	SetCheckButtonsStates();
	RefreshPresetColors();
	CheckButton_SetState("CHECK_GOODS_SET", iGOODS_SET, true);
	CheckButton_SetState("CHECK_ITEMS_SET", iITEMS_SET, true);

	SetEventHandler("exitCancel", "ProcessCancelExit", 0);
	SetEventHandler("evntDoPostExit", "DoPostExit", 0);
	SetEventHandler("ievnt_command", "ProcCommand", 0);
	SetEventHandler("OnTableClick", "TradeGoodClick", 0);
	SetEventHandler("RibbonQuantityClick", "TradeQuantityClick", 0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("MouseRClickUP","HideInfo", 0);
	SetEventHandler("PreviewGoodsSet", "PreviewGoodsSet", 0);
	SetEventHandler("PreviewItemsSet", "PreviewItemsSet", 0);
	SetEventHandler("HidePresetPreview", "HidePresetPreview", 0);

	SetEventHandler("CheckButtonChange", "ProcessCheckBox", 0);
	SetEventHandler("ShowItemInfo", "ShowItemInfo", 0);
	SetEventHandler("frame","ProcessFrame",1);
	SetEventHandler("UnShowWindow", "UnShowWindow", 0);
	SetEventHandler("confirmChangeQTY_EDIT", "ConfirmTradeQuantity", 0);
	SetEventHandler("SaveGoodsSet", "SaveGoodsSet", 0);
	SetEventHandler("LoadGoodsSet", "LoadGoodsSet", 0);
	SetEventHandler("SaveItemsSet", "SaveItemsSet", 0);
	SetEventHandler("LoadItemsSet", "LoadItemsSet", 0);
}

void IDoExit(int exitCode)
{
	DelEventHandler("exitCancel", "ProcessCancelExit");
	DelEventHandler("evntDoPostExit", "DoPostExit");
	DelEventHandler("ievnt_command", "ProcCommand");
	DelEventHandler("OnTableClick", "TradeGoodClick");
	DelEventHandler("RibbonQuantityClick", "TradeQuantityClick");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("MouseRClickUP","HideInfo");
	DelEventHandler("PreviewGoodsSet", "PreviewGoodsSet");
	DelEventHandler("PreviewItemsSet", "PreviewItemsSet");
	DelEventHandler("HidePresetPreview", "HidePresetPreview");

	DelEventHandler("CheckButtonChange", "ProcessCheckBox");
	DelEventHandler("ShowItemInfo", "ShowItemInfo");
	DelEventHandler("UnShowWindow", "UnShowWindow");
	DelEventHandler("confirmChangeQTY_EDIT", "ConfirmTradeQuantity");
	DelEventHandler("frame","ProcessFrame");
	DelEventHandler("SaveGoodsSet", "SaveGoodsSet");
	DelEventHandler("LoadGoodsSet", "LoadGoodsSet");
	DelEventHandler("SaveItemsSet", "SaveItemsSet");
	DelEventHandler("LoadItemsSet", "LoadItemsSet");
	HideInfo();
	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcessCancelExit()
{
	if (sPresetPreviewReturnNode != "")
	{
		HidePresetPreview();
		return;
	}
	IDoExit(RC_INTERFACE_GOODS_TRANSFER);
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void HideInfo()
{
	CloseTooltip();
}

void SetCheckButtonsStates()
{
	bool hasShip = iCurCompanion >= 0;
	SetNodeUsing("CHECK_BUYCONTRABAND", hasShip);
	SetNodeUsing("CHECK_RESTRICTSELL", hasShip);
	SetNodeUsing("CLEAR_GOODS_SET", hasShip);
	SetNodeUsing("SAVE_GOODS", hasShip);
	SetNodeUsing("LOAD_GOODS", hasShip);
	if (hasShip)
	{
		CheckButton_SetState("CHECK_BUYCONTRABAND", 1, CheckAttribute(&Characters[iCurCompanion], "TransferGoods.BuyContraband"));
		CheckButton_SetState("CHECK_RESTRICTSELL", 1, CheckAttribute(&Characters[iCurCompanion], "TransferGoods.SellRestriction"));
	}

	CheckButton_SetState("CHECK_RESTRICTSELL_I", 1, TI_CheckFlag("SellRestriction"));
	CheckButton_SetState("CHECK_RESTRICTSELL_G", 1, TI_CheckFlag("SellGarbageRestriction"));
	CheckButton_SetState("CHECK_SELL_LOGS", 1, CheckAttribute(&InterfaceStates, "AutoSellLogs"));

	SetNodeUsing("REPORT_BTN", CheckAttribute(pchar, "TreasurerReport")); // Gregg: отчёта нет - и кнопку не показываем
}

void ShowWindow(int WindowNum)
{
	switch (WindowNum)
	{
	case 0:
		XI_WindowDisable("MAIN_WINDOW", false);
		XI_WindowDisable("GOODS_TRANSFER_WINDOW", true);
		XI_WindowShow("GOODS_TRANSFER_WINDOW", false);
		SetCurrentNode("MAIN_BACK");
		break;
	case 1:
		XI_WindowDisable("MAIN_WINDOW", true);
		XI_WindowDisable("GOODS_TRANSFER_WINDOW", false);
		XI_WindowShow("GOODS_TRANSFER_WINDOW", true);
		SetCurrentNode("TG_CANCEL_BUTTON");
		break;
	}
}

void UnShowWindow()
{
	ShowWindow(0);
}

void ShowTransferGoods()
{
	string describeStr = "";
	string sHeader;
	int buyCount;

	if (isGoodTable)
	{	//показ товара левой таблицы
		String sGood = Goods[iCurGoodIndex].name;
		SendMessage(&GameInterface, "lslss", MSG_INTERFACE_MSG_TO_NODE, "TG_GOODS_PICTURE", 6, "GOODS", sGood);//картинка
		sHeader = XI_ConvertString(sGood);
		describeStr = GetAssembledString(GetConvertStr(sGood + "_descr", "GoodsDescribe.txt"), &Goods[iCurGoodIndex]);

		if (CheckAttribute(Characters[iCurCompanion], "TransferGoods.G." + sGood)) buyCount = sti(Characters[iCurCompanion].TransferGoods.G.(sGood)); else buyCount = 0;
	}
	else
	{	//показ предмета правой таблицы
		ref item = ItemsFromID(sCurItemId);
		SetNewGroupPicture("TG_GOODS_PICTURE", item.picTexture, "itm" + item.picIndex);//картинка
		sHeader = GetConvertStr(item.name, "ItemsDescribe.txt");
		describeStr = GetItemDescribe(sCurItemId);
		buyCount = TI_GetCount(sCurItemId);
	}

	SetFormatedText("TG_GOODS_CAPTION", sHeader);//заголовок
	SetFormatedText("TG_GOODS_INFO", describeStr);//описание
	GameInterface.TG_EDIT.str = buyCount;
}

void OnAddBtnClick(int _add)
{
	int iNum = MakeInt(GameInterface.TG_EDIT.str);

	if (isGoodTable)
	{
		ref rGood = &Goods[iCurGoodIndex];
		int _Units = sti(rGood.Units);
		_add = _add * abs(_add);

		iNum = iNum + _add * _Units;
		if (iNum < 0) iNum = 0;
		// максимум 15000 грузоподъемности, хотя и 10000 много было бы
		if (iNum > 15000 / sti(rGood.Weight) * _Units) iNum = 15000 / sti(rGood.Weight) * _Units;
	}
	else
	{
		iNum = iNum + _add;
		if (iNum < 0) iNum = 0;

	}

	GameInterface.TG_EDIT.str = iNum;
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	if (nodName != "GOODS_TABLE_LIST")
	{
		if (comName == "click" || comName == "dblclick") iLastGoodClickRow = -1;
	}

	if (comName == "rclick" || comName == "dblrclick")
	{
		bool goodsArrow = nodName == "SAVE_GOODS" || nodName == "LOAD_GOODS";
		bool itemsArrow = nodName == "SAVE_ITEMS" || nodName == "LOAD_ITEMS";
		if (goodsArrow || itemsArrow)
		{
			int slot = iITEMS_SET;
			if (goodsArrow) slot = iGOODS_SET;
			string action = "Load";
			if (nodName == "SAVE_GOODS" || nodName == "SAVE_ITEMS") action = "Save";
			string title = XI_ConvertString(action) + " · " + XI_ConvertString("Kit" + slot);
			CreateTooltip("#" + title, XI_ConvertString("TradeHint_" + nodName), argb(255,255,255,255),
				"", 0, "", 0, "", 0, "", "", "", 0, 0);
			return;
		}
	}

	int iMult = 1;//правой кнопкой множитель x5
	if (comName == "rclick") iMult=5;
	switch(nodName)
	{
		case "GOODS_TABLE_LIST":
			if (comName == "click_down") bGoodRuleClickHandled = false;
			if (comName == "dblclick" && !SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, nodName, 3))
			{
				// Native double clicks use a timer shared by all cells. Hit-test this press too.
				bDoubleGoodClick = true;
				SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, "GOODS_TABLE_LIST", -1, 3, "click");
				bDoubleGoodClick = false;
			}
			break;

		case "CONSUME_TABLE_LIST":
			if (comName == "dblclick" && !SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, nodName, 3))
			{
				if (SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, nodName, 1) <= 0) break;
				SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, nodName, -1, 3, "click");
				SetCurrentNode(nodName);
				ShowItemInfo();
			}
			break;

		case "CLEAR_GOODS_SET":
			if (iCurCompanion < 0) break;
			DeleteAttribute(&Characters[iCurCompanion],"TransferGoods");
			FillGoodsTable();
			SetCheckButtonsStates();
			break;

		case "CLEAR_ITEMS_SET":
			TI_ClearAll(); // офицер либо сундук каюты
			FillItemsTable();
			SetCheckButtonsStates();
			break;

		// Gregg: отчёт о последнем рейде казначея по городу (данные - в pchar.TreasurerReport)
		case "REPORT_BTN":
			if(comName == "click" || comName == "activate")
			{
				IDoExit(RC_INTERFACE_ANY_EXIT);
				PostEvent("LaunchIAfterFrame", 1, "sl", "I_TREASURER_REPORT", 2);
				return;
			}
			break;

		case "TG_ADD_ALL_BUTTON":
			OnAddBtnClick(10*iMult);
			break;

		case "TG_ADD_BUTTON":
			OnAddBtnClick(1*iMult);
			break;

		case "TG_REMOVE_ALL_BUTTON":
			OnAddBtnClick(-10*iMult);
			break;

		case "TG_REMOVE_BUTTON":
			OnAddBtnClick(-1*iMult);
			break;

		case "TG_CANCEL_BUTTON":
			if(comName == "click" || comName == "activate")
			{
				UnShowWindow();
			}
			break;


		case "TG_OK_BUTTON":
			if (comName == "click" || comName == "activate") ConfirmTradeQuantity();
			break;
	}
}

void ConfirmTradeQuantity()
{
	string sRow = sCurItemRow;
	if (isGoodTable) sRow = sCurGoodRow;
	ApplyTradeQuantity(isGoodTable, sRow, MakeInt(GameInterface.TG_EDIT.str));
	UnShowWindow();
}

int TradeQuantityStep(bool bGoods, bool bRightClick, bool bShift)
{
	if (bGoods)
	{
		if (bRightClick)
		{
			if (bShift) return 1000;
			return 500;
		}
		if (bShift) return 100;
		return 50;
	}
	if (bRightClick)
	{
		if (bShift) return 100;
		return 10;
	}
	if (bShift) return 5;
	return 1;
}

void TradeQuantityClick()
{
	string sTable = GetEventData();
	int iRow = GetEventData();
	int iDirection = GetEventData();
	bool bRightClick = GetEventData() != 0;
	if (sTable != "GOODS_TABLE_LIST" && sTable != "CONSUME_TABLE_LIST") return;
	iLastGoodClickRow = -1;
	string sRow = "tr" + iRow;
	if (!CheckAttribute(&GameInterface, sTable + "." + sRow + ".td2.str")) return;

	bool bGoods = sTable == "GOODS_TABLE_LIST";
	int iCount = sti(GameInterface.(sTable).(sRow).td2.str);
	ApplyTradeQuantity(bGoods, sRow, iCount + iDirection * TradeQuantityStep(bGoods, bRightClick, XI_IsKeyPressed("shift")));
}

void SetTradeGoodsQuantityCell(string sRow, int iCount)
{
	GameInterface.GOODS_TABLE_LIST.(sRow).td2.str = iCount;
	// Reserve room for both arrows even when all six digits are present.
	float fScale = 0.82;
	if (iCount >= 100000) fScale = 0.68;
	GameInterface.GOODS_TABLE_LIST.(sRow).td2.scale = fScale;
}

void ApplyTradeQuantity(bool bGoods, string sRow, int iCount)
{
	if (bGoods)
	{
		if (!CheckAttribute(&GameInterface, "GOODS_TABLE_LIST." + sRow + ".index")) return;
		ref rGood = &Goods[sti(GameInterface.GOODS_TABLE_LIST.(sRow).index)];
		ref rShip = &Characters[iCurCompanion];
		string sGood = rGood.name;
		string sPath = "TransferGoods.G." + sGood;
		int iPrevious = 0;
		if (CheckAttribute(rShip, sPath)) iPrevious = sti(rShip.(sPath));
		iCount = Clamp(iCount, 0, 999999);
		if (iCount == 0) DeleteAttribute(rShip, sPath);
		else rShip.(sPath) = iCount;
		curBuyGoodsWeight += (iCount - iPrevious) * stf(rGood.Weight) / sti(rGood.Units);
		SetTradeGoodsQuantityCell(sRow, iCount);
		Table_UpdateWindow("GOODS_TABLE_LIST");
		SetFormatedText("GOODS_SET_SUM", GetConvertStrWithReplace("Variable_GoodsTransfer_1", "Interface.txt", "#space#", " ") + MakeInt(curBuyGoodsWeight) + GetConvertStrWithReplace("Variable_GoodsTransfer_2", "Interface.txt", "#space#", " "));
	}
	else
	{
		if (!CheckAttribute(&GameInterface, "CONSUME_TABLE_LIST." + sRow + ".id")) return;
		string sItem = GameInterface.CONSUME_TABLE_LIST.(sRow).id;
		iCount = Clamp(iCount, 0, 9999);
		curBuyItemsWeight += (iCount - TI_GetCount(sItem)) * GetItemWeight(sItem);
		TI_SetCount(sItem, iCount);
		GameInterface.CONSUME_TABLE_LIST.(sRow).td2.str = iCount;
		Table_UpdateWindow("CONSUME_TABLE_LIST");
		SetFormatedText("ITEMS_SET_SUM", GetConvertStrWithReplace("Variable_GoodsTransfer_3", "Interface.txt", "#space#", " ") + FloatToString(curBuyItemsWeight, 1));
	}
}

void ProcessFrame()
{
	string sNode = GetCurrentNode();
	if (sNode == "SHIPS_SCROLL")
	{
		if (sti(GameInterface.SHIPS_SCROLL.current) != nCurScrollNumS)
		{
			nCurScrollNumS = sti(GameInterface.SHIPS_SCROLL.current);
			FillGoodsTable();
			SetCheckButtonsStates();
		}
	}

	if (sNode == "CHARACTERS_SCROLL")
	{
		if (sti(GameInterface.CHARACTERS_SCROLL.current) != nCurScrollNumC)
		{
			nCurScrollNumC = sti(GameInterface.CHARACTERS_SCROLL.current);
			FillItemsTable();
			SetCheckButtonsStates();
		}
	}
}

void ProcessCheckBox()
{
	string sControl = GetEventData();
	int iSelectedCB = GetEventData();
	int iNewState = GetEventData();
	if (sControl == "CHECK_GOODS_SET")	{iGOODS_SET = iSelectedCB;	return;	}
	if (sControl == "CHECK_ITEMS_SET")	{iITEMS_SET = iSelectedCB;	return;	}
	if (sControl == "CHECK_BUYCONTRABAND")
	{
		if (iCurCompanion < 0) return;
		if (iNewState) Characters[iCurCompanion].TransferGoods.BuyContraband = true;
		else DeleteAttribute(&Characters[iCurCompanion], "TransferGoods.BuyContraband");
		return;
	}
	if (sControl == "CHECK_RESTRICTSELL")
	{
		if (iCurCompanion < 0) return;
		if (iNewState) Characters[iCurCompanion].TransferGoods.SellRestriction = true;
		else DeleteAttribute(&Characters[iCurCompanion], "TransferGoods.SellRestriction");
		return;
	}
	if (sControl == "CHECK_RESTRICTSELL_I")
	{
		TI_SetFlag("SellRestriction", iNewState); // офицер либо сундук каюты
		return;
	}
	if (sControl == "CHECK_RESTRICTSELL_G")
	{
		TI_SetFlag("SellGarbageRestriction", iNewState);
		return;
	}
	if (sControl == "CHECK_SELL_LOGS")
	{
		if (iNewState) InterfaceStates.AutoSellLogs = true;
		else DeleteAttribute(&InterfaceStates, "AutoSellLogs");
		return;
	}
}

void ShowItemInfo()
{
	string sNode = GetCurrentNode();
	if (sNode != "GOODS_TABLE_LIST" && sNode != "CONSUME_TABLE_LIST") return;
	isGoodTable = sNode == "GOODS_TABLE_LIST";
	if (isGoodTable)
	{
		if (!CheckAttribute(&GameInterface, "GOODS_TABLE_LIST." + sCurGoodRow + ".index")) return;
	}
	else if (sCurItemId == "") return;
	ShowWindow(1);
	ShowTransferGoods();
}

void FillShipsScroll()
{
	FillScrollImageWithCompanionShips("SHIPS_SCROLL", COMPANION_MAX);
	nCurScrollNumS = 0;
	GameInterface.SHIPS_SCROLL.current = 0;
}

void FillCharactersScroll()
{
	int i;
	string attributeName;
	int _curCharIdx;
	bool bOk;
	DeleteAttribute(&GameInterface, "CHARACTERS_SCROLL");

	GameInterface.CHARACTERS_SCROLL.current = 0;

	int nListSize = GetPassengersQuantity(pchar);

	GameInterface.CHARACTERS_SCROLL.NotUsed = 6;

	GameInterface.CHARACTERS_SCROLL.ImagesGroup.t0 = "EMPTYFACE";
	GameInterface.CHARACTERS_SCROLL.ImagesGroup.t1 = "BOX_IMAGE";

	FillFaceList("CHARACTERS_SCROLL.ImagesGroup", pchar, 2); // passengers

	GameInterface.CHARACTERS_SCROLL.BadTex1 = 0;
	GameInterface.CHARACTERS_SCROLL.BadPic1 = "emptyface";

	int m = 0;
	// первый сундук каюты - СЛЕВА от ГГ: настройка автозакупки у торговцев предметами прямо в сундук
	if (Trade_GetCabinChestLocation() >= 0)
	{
		attributeName = "pic" + (m + 1);
		GameInterface.CHARACTERS_SCROLL.(attributeName).character = "box1";
		GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = "BoxImage";
		GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","BOX_IMAGE");
		m++;
	}
	attributeName = "pic" + (m + 1);
	GameInterface.CHARACTERS_SCROLL.(attributeName).character = nMainCharacterIndex;
	GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(pchar);
	GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_" + pchar.FaceID);
	GameInterface.CHARACTERS_SCROLL.current = m; // по умолчанию выбран ГГ (сундук - слева)
	nCurScrollNumC = m;
	m++;

	for(i = 0; i < nListSize; i++)
	{
		_curCharIdx = GetPassenger(pchar, i);
		if (_curCharIdx < 0) continue;

		bOk = false;
		if (CheckAttribute(&characters[_curCharIdx], "prisoned")) bOk = sti(characters[_curCharIdx].prisoned) != 0;
		if (!CheckAttribute(&characters[_curCharIdx], "isquest") && !bOk)
		{
			if (IsOfficer(&characters[_curCharIdx]))  // boal
			{
				attributeName = "pic" + (m + 1);
				GameInterface.CHARACTERS_SCROLL.(attributeName).character = _curCharIdx;
				GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
				GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
				m++;
			}
		}
	}
	GameInterface.CHARACTERS_SCROLL.ListSize = m;
}

void FillGoodsTable()
{
	string sGood, row;
	int i, n, buyCount;
	iLastGoodClickRow = -1;
	string attributeName = "pic" + (nCurScrollNumS+1);
	if (!CheckAttribute(&GameInterface, "SHIPS_SCROLL." + attributeName + ".companionIndex"))
	{
		iCurCompanion = -1;
		sCurGoodRow = "";
		Table_Clear("GOODS_TABLE_LIST", false, true, true);
		return;
	}
	iCurCompanion = sti(GameInterface.SHIPS_SCROLL.(attributeName).companionIndex);
	ref refCharacter = &Characters[iCurCompanion];
	SetNewPicture("CHARACTER_PICTURE", "interfaces\portraits\face_" + refCharacter.FaceId + ".tga");
	SetFormatedText("SHIP_NAME", XI_ConvertString(RealShips[sti(refCharacter.Ship.Type)].BaseName) + " '" + refCharacter.Ship.Name + "'");
	SetFormatedText("COMP_NAME", GetFullName(refCharacter));
	Table_Clear("GOODS_TABLE_LIST", true, true, false);
	GameInterface.GOODS_TABLE_LIST.hr.td1.str = GetConvertStrWithReplace("Variable_GoodsTransfer_4", "Interface.txt", "#space#", " ");
	GameInterface.GOODS_TABLE_LIST.hr.td2.str = GetConvertStrWithReplace("Variable_GoodsTransfer_5", "Interface.txt", "#space#", " ");
	GameInterface.GOODS_TABLE_LIST.hr.td3.str = GetConvertStrWithReplace("Variable_GoodsTransfer_5.2", "Interface.txt", "#enter#", "");
	GameInterface.GOODS_TABLE_LIST.hr.td4.str = XI_ConvertString("TradeRuleBuy");
	GameInterface.GOODS_TABLE_LIST.hr.td5.str = XI_ConvertString("TradeRuleSell");
	curBuyGoodsWeight = 0;

	for (i = 0, n = 1; i< GOODS_QUANTITY; i++)
	{
		row = "tr" + n;
		sGood = Goods[i].name;
		if (checkattribute(Goods[i],"CannonIdx")) continue;//без пушек???

		GameInterface.GOODS_TABLE_LIST.(row).index = i;
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.group = "GOODS";
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.image = sGood;
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.offset = "-2,0";
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.width = 34;
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.height = 34;
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.valign = "center";
		GameInterface.GOODS_TABLE_LIST.(row).td1.textoffset = "39,0";
		GameInterface.GOODS_TABLE_LIST.(row).td1.str = XI_ConvertString(sGood);
		GameInterface.GOODS_TABLE_LIST.(row).td1.align = "left";
		aref arGoodRow;
		makearef(arGoodRow, GameInterface.GOODS_TABLE_LIST.(row));
		SetTradeRuleIcon(arGoodRow, "td4", CheckAttribute(pchar, "TradeGoodsRules.NoBuy." + sGood), 22);
		SetTradeRuleIcon(arGoodRow, "td5", CheckAttribute(pchar, "TradeGoodsRules.NoSell." + sGood), 22);
		if (CheckAttribute(Characters[iCurCompanion], "TransferGoods.G." + sGood)) buyCount = sti(Characters[iCurCompanion].TransferGoods.G.(sGood)); else buyCount = 0;
		SetTradeGoodsQuantityCell(row, buyCount);
		GameInterface.GOODS_TABLE_LIST.(row).td3.str = Goods[i].Weight + " / " + Goods[i].Units;//вес/пачка
		curBuyGoodsWeight = curBuyGoodsWeight + buyCount*stf(Goods[i].Weight)/sti(Goods[i].Units);
		n++;
	}
	sCurGoodRow = "";
	if (n > 1)
	{
		sCurGoodRow = "tr1";
		iCurGoodIndex = sti(GameInterface.GOODS_TABLE_LIST.tr1.index);
	}
	GameInterface.GOODS_TABLE_LIST.top = 0;
	GameInterface.GOODS_TABLE_LIST.select = (n > 1);
	Table_UpdateWindow("GOODS_TABLE_LIST");
	SetFormatedText("GOODS_SET_SUM", GetConvertStrWithReplace("Variable_GoodsTransfer_6", "Interface.txt", "#space#", " ") + MakeInt(curBuyGoodsWeight) + GetConvertStrWithReplace("Variable_GoodsTransfer_7", "Interface.txt", "#space#", " "));
}

void FillItemsTable()
{
	string sItem, row;
	int i, n, buyCount;
	string attributeName = "pic" + (nCurScrollNumC+1);
	if (!CheckAttribute(&GameInterface, "CHARACTERS_SCROLL." + attributeName + ".character")) return;
	// выбран сундук каюты ЛИБО персонаж
	bChestSel = false;
	if (GameInterface.CHARACTERS_SCROLL.(attributeName).character == "box1")
	{
		if (Trade_GetCabinChestLocation() >= 0) bChestSel = true;
		iCurFighter = nMainCharacterIndex; // безопасный дефолт для "персонажных" обращений (данные идут через TI_* по bChestSel)
		SetFormatedText("CHAR_NAME", GetConvertStrWithReplace("Variable_itemsbox_31", "Interface.txt", "#space#", " "));
	}
	else
	{
		iCurFighter = sti(GameInterface.CHARACTERS_SCROLL.(attributeName).character);
		SetFormatedText("CHAR_NAME", GetFullName(Characters[iCurFighter]));
	}

	Table_Clear("CONSUME_TABLE_LIST", false, true, false);
	GameInterface.CONSUME_TABLE_LIST.hr.td1.str = GetConvertStrWithReplace("Variable_GoodsTransfer_8", "Interface.txt", "#space#", " ");
	GameInterface.CONSUME_TABLE_LIST.hr.td2.str = GetConvertStrWithReplace("Variable_GoodsTransfer_9", "Interface.txt", "#space#", " ");
	GameInterface.CONSUME_TABLE_LIST.hr.td3.str = GetConvertStrWithReplace("Variable_GoodsTransfer_10", "Interface.txt", "#space#", " ");
	curBuyItemsWeight = 0;
	int iLang = LanguageOpenFile("ItemsDescribe.txt");
	for(i = 0, n = 1; i < ITEMS_QUANTITY; i++)
	{
		sItem = Items[i].id;

		if (IsGenerableItemIndex(i)) continue;
		if (!CheckAttribute(Items[i],"sortIndex"))
		{
			if (!HasSubStr(sItem,"CompCraft")) continue;//в списке только расходники
		}
		else
		{
			if (Items[i].SortIndex == 1 || Items[i].SortIndex == 2 || Items[i].ItemType == "CRAFTCOMPONENTS") {}
			else continue; //в списке расходники и крафт

		}
		if (sItem == "Lockpick") continue;//отмычки пропускаем
		if (sItem == "CompCraft_Tools" || sItem == "CompCraft_Locksmith" || sItem == "CompCraft_Puleleyka") continue; //исключение выбранных Шахом штук
		if (sItem == "powder_pellet" || sItem == "grenade" || sItem == "harpoon" || sItem == "GunCap_colt" || sItem == "shotgun_cartridge" || sItem == "GunEchin" || sItem == "shotgun_bullet" || sItem == "12_gauge") continue; //не встречается в продаже
		row = "tr" + n;

		buyCount = TI_GetCount(sItem); // офицер либо сундук каюты

		GameInterface.CONSUME_TABLE_LIST.(row).id = sItem;
		GameInterface.CONSUME_TABLE_LIST.(row).td1.icon.group = Items[i].picTexture;
		GameInterface.CONSUME_TABLE_LIST.(row).td1.icon.image = "itm" + Items[i].picIndex;
		GameInterface.CONSUME_TABLE_LIST.(row).td1.icon.offset = "-2,0";
		GameInterface.CONSUME_TABLE_LIST.(row).td1.icon.width = 37;
		GameInterface.CONSUME_TABLE_LIST.(row).td1.icon.height = 37;
		GameInterface.CONSUME_TABLE_LIST.(row).td1.icon.valign = "center";
		GameInterface.CONSUME_TABLE_LIST.(row).td1.textoffset = "40,0";
		GameInterface.CONSUME_TABLE_LIST.(row).td1.align = "left";
		GameInterface.CONSUME_TABLE_LIST.(row).td1.str = Items[i].name;
		if (iLang >= 0) GameInterface.CONSUME_TABLE_LIST.(row).td1.str = LanguageConvertString(iLang, Items[i].name);
		GameInterface.CONSUME_TABLE_LIST.(row).td2.str = buyCount;
		float fWeight = 0.0;
		if (CheckAttribute(&Items[i], "weight")) fWeight = stf(Items[i].weight);
		GameInterface.CONSUME_TABLE_LIST.(row).td3.str = FloatToString(fWeight, 1);
		curBuyItemsWeight += buyCount * fWeight;
		n++;
	}
	if (iLang >= 0) LanguageCloseFile(iLang);
	sCurItemRow = "tr1";
	sCurItemId = "";
	if (n > 1) sCurItemId = GameInterface.CONSUME_TABLE_LIST.tr1.id;
	GameInterface.CONSUME_TABLE_LIST.top = 0;
	GameInterface.CONSUME_TABLE_LIST.select = (n > 1);
	Table_UpdateWindow("CONSUME_TABLE_LIST");
	SetFormatedText("ITEMS_SET_SUM", GetConvertStrWithReplace("Variable_GoodsTransfer_11", "Interface.txt", "#space#", " ") + FloatToString(curBuyItemsWeight,1));
}

void TableSelectChange()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();
	string sRow = "tr" + (iSelected);

	switch (sControl)
	{
		case "GOODS_TABLE_LIST":
			if (!CheckAttribute(GameInterface, "GOODS_TABLE_LIST." + sRow + ".index")) break;
			iCurGoodIndex = sti(GameInterface.GOODS_TABLE_LIST.(sRow).index);
			sCurGoodRow = sRow;
			break;
		case "CONSUME_TABLE_LIST":
			if (!CheckAttribute(GameInterface, "CONSUME_TABLE_LIST." + sRow + ".id")) break;
			sCurItemId = GameInterface.CONSUME_TABLE_LIST.(sRow).id;
			sCurItemRow = sRow;
			break;
	}
}


void SaveGoodsSet()
{
	if (iCurCompanion < 0) return;
	string sSET = "GoodsSet" + iGOODS_SET;
	DeleteAttribute(pchar, sSET);
	pchar.(sSET) = XI_ConvertString("Kit" + iGOODS_SET);

	aref arTo, arFrom;
	makearef(arTo, pchar.(sSET));
	if (CheckAttribute(Characters[iCurCompanion], "TransferGoods"))
	{
		makearef(arFrom, Characters[iCurCompanion].TransferGoods);
		CopyAttributes(arTo, arFrom);
	}

	// Фиксируем даже пустой список, чтобы загрузка могла снять все запреты.
	pchar.(sSET).TradeGoodsRules = 1;
	if (CheckAttribute(pchar, "TradeGoodsRules"))
	{
		makearef(arTo, pchar.(sSET).TradeGoodsRules);
		makearef(arFrom, pchar.TradeGoodsRules);
		CopyAttributes(arTo, arFrom);
	}
	RefreshPresetColors();
}
void LoadGoodsSet()
{
	if (iCurCompanion < 0) return;
	string sSET = "GoodsSet" + iGOODS_SET;
	if (!CheckAttribute(pchar, sSET)) return;
	ref rShip = &Characters[iCurCompanion];
	DeleteAttribute(rShip, "TransferGoods");

	aref arTo, arFrom;
	rShip.TransferGoods = "";
	makearef(arTo, rShip.TransferGoods);
	makearef(arFrom, pchar.(sSET));
	CopyAttributes(arTo, arFrom);
	DeleteAttribute(rShip, "TransferGoods.TradeGoodsRules");

	// В старых комплектах правил эскадры не было.
	if (CheckAttribute(pchar, sSET + ".TradeGoodsRules"))
	{
		DeleteAttribute(pchar, "TradeGoodsRules");
		pchar.TradeGoodsRules = 1;
		makearef(arTo, pchar.TradeGoodsRules);
		makearef(arFrom, pchar.(sSET).TradeGoodsRules);
		CopyAttributes(arTo, arFrom);
	}
	FillGoodsTable();
	SetCheckButtonsStates();
}


void SaveItemsSet()
{
	string sSET = "ItemsSet" + iITEMS_SET;
	DeleteAttribute(pchar, sSET);
	pchar.(sSET) = XI_ConvertString("Kit" + iITEMS_SET);

	aref arTo, arFrom;
	makearef(arTo, pchar.(sSET));
	ref holder = TI_SettingsOwner();
	string sPath = TI_SettingsPath();
	if (CheckAttribute(holder, sPath))
	{
		makearef(arFrom, holder.(sPath));
		CopyAttributes(arTo, arFrom);
	}
	RefreshPresetColors();
}
void LoadItemsSet()
{
	string sSET = "ItemsSet" + iITEMS_SET;
	if (!CheckAttribute(pchar, sSET)) return;
	TI_ClearAll();

	aref arTo, arFrom;
	ref holder = TI_SettingsOwner();
	string sPath = TI_SettingsPath();
	holder.(sPath) = "";
	makearef(arTo, holder.(sPath));
	makearef(arFrom, pchar.(sSET));
	CopyAttributes(arTo, arFrom);

	FillItemsTable();
	SetCheckButtonsStates();
}

void SetTradeRuleIcon(aref arRow, string sCell, bool bForbidden, int iSize)
{
	arRow.(sCell).icon.group = "CHECKBUTTON_ICON";
	if (bForbidden) arRow.(sCell).icon.image = "check_no";
	else arRow.(sCell).icon.image = "check_yes";
	arRow.(sCell).icon.width = iSize;
	arRow.(sCell).icon.height = iSize;
	arRow.(sCell).icon.halign = "center";
	arRow.(sCell).icon.valign = "center";
}

void TradeGoodClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();
	if (sControl != "GOODS_TABLE_LIST")
	{
		iLastGoodClickRow = -1;
		return;
	}
	if (bGoodRuleClickHandled) return;
	bGoodRuleClickHandled = true;
	// The header can emit OnTableClick with the previous selection.
	if (SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, sControl, 1) <= 0)
	{
		iLastGoodClickRow = -1;
		return;
	}
	bool bSameCell = iLastGoodClickRow == iRow && iLastGoodClickColumn == iColumn;
	iLastGoodClickRow = iRow;
	iLastGoodClickColumn = iColumn;
	if (iColumn == 4 || iColumn == 5)
	{
		if (!bDoubleGoodClick || !bSameCell) ToggleTradeGoodRule(iRow + 1, iColumn);
	}
	else if (bDoubleGoodClick && bSameCell) ShowItemInfo();
}

void ToggleTradeGoodRule(int iRow, int iColumn)
{
	string sRow = "tr" + iRow;
	if (!CheckAttribute(GameInterface, "GOODS_TABLE_LIST." + sRow + ".index")) return;
	int iGood = sti(GameInterface.GOODS_TABLE_LIST.(sRow).index);
	string sRule = "TradeGoodsRules.NoBuy." + Goods[iGood].name;
	if (iColumn == 5)
	{
		sRule = "TradeGoodsRules.NoSell." + Goods[iGood].name;
	}
	bool bForbidden = !CheckAttribute(pchar, sRule);
	if (bForbidden) pchar.(sRule) = true;
	else DeleteAttribute(pchar, sRule);
	aref arRow;
	makearef(arRow, GameInterface.GOODS_TABLE_LIST.(sRow));
	SetTradeRuleIcon(arRow, "td" + iColumn, bForbidden, 22);
	Table_UpdateWindow("GOODS_TABLE_LIST");
}

void RefreshPresetColors()
{
	for (int i = 1; i <= 6; i++)
	{
		int iColor = 0;
		if (CheckAttribute(pchar, "GoodsSet" + i)) iColor = argb(255, 132, 218, 123);
		SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "CHECK_GOODS_SET", 6, i, iColor);
		iColor = 0;
		if (CheckAttribute(pchar, "ItemsSet" + i)) iColor = argb(255, 132, 218, 123);
		SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "CHECK_ITEMS_SET", 6, i, iColor);
	}
}

int FillPreviewGoodsTable(aref arPreset)
{
	Table_Clear("PRESET_GOODS_TABLE", true, true, false);
	GameInterface.PRESET_GOODS_TABLE.hr.td1.str = XI_ConvertString("Goods");
	GameInterface.PRESET_GOODS_TABLE.hr.td2.str = XI_ConvertString("QuantityShort");
	GameInterface.PRESET_GOODS_TABLE.hr.td3.str = XI_ConvertString("TradeRuleBuy");
	GameInterface.PRESET_GOODS_TABLE.hr.td4.str = XI_ConvertString("TradeRuleSell");

	bool bHasRules = CheckAttribute(arPreset, "TradeGoodsRules");
	int nRows = 0;
	for (int i = 0; i < GOODS_QUANTITY; i++)
	{
		string sGood = Goods[i].name;
		string sPath = "G." + sGood;
		int iCount = 0;
		if (CheckAttribute(arPreset, sPath)) iCount = sti(arPreset.(sPath));
		bool bNoBuy = CheckAttribute(arPreset, "TradeGoodsRules.NoBuy." + sGood);
		bool bNoSell = CheckAttribute(arPreset, "TradeGoodsRules.NoSell." + sGood);
		if (iCount <= 0 && !bNoBuy && !bNoSell) continue;

		nRows++;
		string sRow = "tr" + nRows;
		aref arRow;
		GameInterface.PRESET_GOODS_TABLE.(sRow).td2.str = iCount;
		makearef(arRow, GameInterface.PRESET_GOODS_TABLE.(sRow));
		arRow.td1.icon.group = "GOODS";
		arRow.td1.icon.image = sGood;
		arRow.td1.icon.width = 21;
		arRow.td1.icon.height = 21;
		arRow.td1.icon.valign = "center";
		arRow.td1.textoffset = "25,0";
		arRow.td1.align = "left";
		arRow.td1.str = XI_ConvertString(sGood);
		if (bHasRules)
		{
			SetTradeRuleIcon(arRow, "td3", bNoBuy, 20);
			SetTradeRuleIcon(arRow, "td4", bNoSell, 20);
		}
		else
		{
			arRow.td3.str = "—";
			arRow.td4.str = "—";
		}
	}
	GameInterface.PRESET_GOODS_TABLE.top = 0;
	GameInterface.PRESET_GOODS_TABLE.select = 0;
	return nRows;
}

int FillPreviewItemsTable(aref arPreset)
{
	Table_Clear("PRESET_ITEMS_TABLE", true, true, false);
	GameInterface.PRESET_ITEMS_TABLE.hr.td1.str = GetConvertStrWithReplace("Variable_GoodsTransfer_8", "Interface.txt", "#space#", " ");
	GameInterface.PRESET_ITEMS_TABLE.hr.td2.str = XI_ConvertString("QuantityShort");
	GameInterface.PRESET_ITEMS_TABLE.top = 0;
	GameInterface.PRESET_ITEMS_TABLE.select = 0;
	if (!CheckAttribute(arPreset, "I")) return 0;

	aref arItems, arEntry, arRow;
	makearef(arItems, arPreset.I);
	int iLang = LanguageOpenFile("ItemsDescribe.txt");
	int iRows = 0;
	for (int i = 0; i < GetAttributesNum(arItems); i++)
	{
		arEntry = GetAttributeN(arItems, i);
		int iCount = sti(GetAttributeValue(arEntry));
		if (iCount <= 0) continue;
		string sName = GetAttributeName(arEntry);
		int iItem = FindItem(sName);
		iRows++;
		string sRow = "tr" + iRows;
		GameInterface.PRESET_ITEMS_TABLE.(sRow).td2.str = iCount;
		makearef(arRow, GameInterface.PRESET_ITEMS_TABLE.(sRow));
		if (iItem >= 0)
		{
			if (iLang >= 0) sName = LanguageConvertString(iLang, Items[iItem].name);
			arRow.td1.icon.group = Items[iItem].picTexture;
			arRow.td1.icon.image = "itm" + Items[iItem].picIndex;
			arRow.td1.icon.width = 20;
			arRow.td1.icon.height = 20;
			arRow.td1.icon.valign = "center";
		}
		arRow.td1.align = "left";
		arRow.td1.textoffset = "25,0";
		arRow.td1.str = sName;
	}
	if (iLang >= 0) LanguageCloseFile(iLang);
	return iRows;
}

void PreviewGoodsSet()
{
	ShowPresetPreview("CHECK_GOODS_SET", "GoodsSet");
}

void PreviewItemsSet()
{
	ShowPresetPreview("CHECK_ITEMS_SET", "ItemsSet");
}

void ShowPresetPreview(string sControl, string sPrefix)
{
	if (sPresetPreviewReturnNode != "") return;
	int iSlot = SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, sControl, 7);
	if (iSlot < 1 || iSlot > 6) return;
	string sSET = sPrefix + iSlot;
	if (!CheckAttribute(pchar, sSET)) return;

	aref arPreset;
	makearef(arPreset, pchar.(sSET));
	string sText = "";
	int iRows;
	bool bGoods = sPrefix == "GoodsSet";
	if (bGoods)
	{
		iRows = FillPreviewGoodsTable(arPreset);
		if (CheckAttribute(arPreset, "SellRestriction")) sText = XI_ConvertString("SellRestriction");
		else sText = XI_ConvertString("TradeShipSaleOff");
		if (CheckAttribute(arPreset, "BuyContraband")) sText += "\n" + XI_ConvertString("BuyContraband");
		if (!CheckAttribute(arPreset, "TradeGoodsRules")) sText += "\n" + XI_ConvertString("TradeLegacyRules");
		if (iRows == 0) sText = XI_ConvertString("TradePresetNoGoods") + "\n" + sText;
	}
	else
	{
		iRows = FillPreviewItemsTable(arPreset);
		if (CheckAttribute(arPreset, "SellRestriction")) sText = XI_ConvertString("SellRestriction_I");
		if (CheckAttribute(arPreset, "SellGarbageRestriction")) sText += "\n" + XI_ConvertString("SellRestriction_G");
		if (CheckAttribute(arPreset, "AutoBuyAmmo")) sText += "\n" + XI_ConvertString("TradePresetAutoAmmo");
		if (iRows == 0 && sText == "") sText = XI_ConvertString("TradePresetEmpty");
	}

	sPresetPreviewReturnNode = GetCurrentNode();
	SetFormatedText("PRESET_PREVIEW_TITLE", XI_ConvertString("Kit" + iSlot));
	SetFormatedText("PRESET_PREVIEW_TEXT", "");
	int iLines = SendMessage(&GameInterface, "lsle", MSG_INTERFACE_MSG_TO_NODE, "PRESET_PREVIEW_TEXT", 0, &sText);
	int iVisibleLines = iLines;
	if (iVisibleLines < 1) iVisibleLines = 1;
	if (iVisibleLines > 11) iVisibleLines = 11;
	float fOffsetX, fOffsetY;
	GetXYWindowOffset(&fOffsetX, &fOffsetY);
	if (CheckAttribute(&Render, "letterbox") && sti(Render.letterbox) != 0)
	{
		// Match the narrow-aspect canvas sections in interfaces.ini.
		float fAspect = stf(Render.screen_x) / stf(Render.screen_y);
		if (fAspect > 1.6 * 0.97 && fAspect <= 1.6 * 1.03) { fOffsetX = 133; fOffsetY = 33; }
		if (fAspect > 1.25 * 0.97 && fAspect <= 1.25 * 1.03) { fOffsetX = 133; fOffsetY = 126; }
		if (fAspect > 1.333333 * 0.97 && fAspect <= 1.333333 * 1.03) { fOffsetX = 133; fOffsetY = 100; }
	}
	int dx = makeint(fOffsetX);
	int dy = makeint(fOffsetY);
	int iMaxRows = 15;
	int iVisibleRows = iRows;
	if (iVisibleRows > iMaxRows) iVisibleRows = iMaxRows;
	int iTableHeight = 0;
	if (iVisibleRows > 0) iTableHeight = (iVisibleRows + 1) * 23;
	int iFooterHeight = 8;
	if (sText != "") iFooterHeight += iVisibleLines * 16 + 8;
	int iFrameHeight = 31 + iTableHeight + iFooterHeight;
	int iFrameTop = (600 - iFrameHeight) / 2;
	int iTableTop = iFrameTop + 31;
	int iTableBottom = iTableTop + iTableHeight;
	int iFrameBottom = iFrameTop + iFrameHeight;
	SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, "PRESET_PREVIEW_FRAME", -1, 4, 175 + dx, iFrameTop + dy, 625 + dx, iFrameBottom + dy);
	SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, "PRESET_PREVIEW_TITLE", -1, 4, 186 + dx, iFrameTop + 4 + dy, 614 + dx, iFrameTop + 23 + dy);
	SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, "PRESET_PREVIEW_CLOSE", -1, 4, 599 + dx, iFrameTop + 2 + dy, 619 + dx, iFrameTop + 22 + dy);
	SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, "PRESET_PREVIEW_TEXT", -1, 4, 191 + dx, iTableBottom + 4 + dy, 610 + dx, iFrameBottom - 4 + dy);

	XI_WindowDisable("MAIN_WINDOW", true);
	XI_WindowShow("PRESET_PREVIEW_WINDOW", true);
	XI_WindowDisable("PRESET_PREVIEW_WINDOW", false);
	SetNodeUsing("PRESET_GOODS_TABLE", bGoods && iRows > 0);
	SetNodeUsing("PRESET_ITEMS_TABLE", !bGoods && iRows > 0);
	SetNodeUsing("PRESET_PREVIEW_TEXT", sText != "");
	SetNodeUsing("PRESET_GOODS_SCROLL", bGoods && iRows > iVisibleRows);
	SetNodeUsing("PRESET_ITEMS_SCROLL", !bGoods && iRows > iVisibleRows);

	if (iRows > 0)
	{
		string sTable = "PRESET_GOODS_TABLE";
		string sScroll = "PRESET_GOODS_SCROLL";
		if (!bGoods)
		{
			sTable = "PRESET_ITEMS_TABLE";
			sScroll = "PRESET_ITEMS_SCROLL";
		}
		SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, sScroll, -1, 4, 606 + dx, iTableTop + dy, 619 + dx, iTableBottom + dy);
		// Message 2 adjusts the ribbon viewport while preserving the stock row heights.
		SendMessage(&GameInterface, "lsllll", MSG_INTERFACE_MSG_TO_NODE, sTable, 2, 184 + dx, iTableTop + dy, iVisibleRows + 1);
		SetCurrentNode(sTable);
	}
	else SetCurrentNode("PRESET_PREVIEW_CLOSE");
}

void HidePresetPreview()
{
	if (sPresetPreviewReturnNode == "") return;
	XI_WindowShow("PRESET_PREVIEW_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", false);
	SetCurrentNode(sPresetPreviewReturnNode);
	sPresetPreviewReturnNode = "";
}
