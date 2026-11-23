// Gregg: Debug — создание товаров на корабль.
// Клон грида «Закупка товаров казначеем» (GoodsTransfer.c), но «OK» кладёт товар
// прямо в трюм выбранного корабля БЕСПЛАТНО (без казначея и без списания денег).
// Запускается из меню отладчика (F11) кнопкой «Создать товары».

int  nCurScrollNumS = 0;   // текущий индекс в скролле кораблей
int  iCurCompanion  = 0;   // индекс персонажа-владельца выбранного корабля
int  iCurGoodIndex  = 0;   // выбранный товар (индекс в Goods[])
string sCurGoodRow;        // строка таблицы выбранного товара

void InitInterface(string iniName)
{
	FillShipsScroll();
	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);
	FillGoodsTable();

	GameInterface.title = "titleGoodsSpawn";

	SetEventHandler("exitCancel", "ProcessCancelExit", 0);
	SetEventHandler("evntDoPostExit", "DoPostExit", 0);
	SetEventHandler("ievnt_command", "ProcCommand", 0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ShowItemInfo", "ShowItemInfo", 0);
	SetEventHandler("frame", "ProcessFrame", 1);
	SetEventHandler("UnShowWindow", "UnShowWindow", 0);
}

void IDoExit(int exitCode)
{
	DelEventHandler("exitCancel", "ProcessCancelExit");
	DelEventHandler("evntDoPostExit", "DoPostExit");
	DelEventHandler("ievnt_command", "ProcCommand");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ShowItemInfo", "ShowItemInfo");
	DelEventHandler("frame", "ProcessFrame");
	DelEventHandler("UnShowWindow", "UnShowWindow");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcessCancelExit()
{
	IDoExit(RC_INTERFACE_QUICK_SAVE);   // назад в игру
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

// -------- служебное --------

void FillShipsScroll()
{
	FillScrollImageWithCompanionShips("SHIPS_SCROLL", COMPANION_MAX);
	if(!CheckAttribute(&GameInterface, "SHIPS_SCROLL.current")) GameInterface.SHIPS_SCROLL.current = 0;
}

void RefreshCargoInfo(ref refCharacter)
{
	SetFormatedText("GOODS_SET_SUM", GetConvertStrWithReplace("GoodsSpawn_Hold", "Interface.txt", "#space#", " ") + GetCargoLoad(refCharacter) + " / " + GetCargoMaxSpace(refCharacter));
}

void FillGoodsTable()
{
	string sGood, row;
	int i, n;
	string attributeName = "pic" + (nCurScrollNumS + 1);
	iCurCompanion = GameInterface.SHIPS_SCROLL.(attributeName).companionIndex;
	String companionId = Characters[iCurCompanion].Id;
	ref refCharacter = characterFromID(companionId);

	SetNewPicture("CHARACTER_PICTURE", "interfaces\portraits\face_" + refCharacter.FaceId + ".tga");
	SetFormatedText("SHIP_NAME", XI_ConvertString(RealShips[sti(refCharacter.Ship.Type)].BaseName) + " '" + refCharacter.Ship.Name + "'");
	SetFormatedText("COMP_NAME", GetFullName(refCharacter));

	Table_Clear("GOODS_TABLE_LIST", false, true, false);
	GameInterface.GOODS_TABLE_LIST.hr.td1.str = GetConvertStrWithReplace("GoodsSpawn_ColGood", "Interface.txt", "#space#", " ");
	GameInterface.GOODS_TABLE_LIST.hr.td2.str = GetConvertStrWithReplace("GoodsSpawn_ColOnboard", "Interface.txt", "#space#", " ");
	GameInterface.GOODS_TABLE_LIST.hr.td3.str = GetConvertStrWithReplace("GoodsSpawn_ColWeight", "Interface.txt", "#enter#", "\n");
	GameInterface.GOODS_TABLE_LIST.hr.td3.scale = 0.8;

	for (i = 0, n = 1; i < GOODS_QUANTITY; i++)
	{
		sGood = Goods[i].name;
		if (CheckAttribute(Goods[i], "CannonIdx")) continue;   // пушки/кулеврины пропускаем

		row = "tr" + n;
		GameInterface.GOODS_TABLE_LIST.(row).index = i;
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.group = "GOODS";
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.image = sGood;
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.offset = "-3, -1";
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.width = 42;
		GameInterface.GOODS_TABLE_LIST.(row).td1.icon.height = 42;
		GameInterface.GOODS_TABLE_LIST.(row).td1.textoffset = "39,0";
		GameInterface.GOODS_TABLE_LIST.(row).td1.str = XI_ConvertString(sGood);
		GameInterface.GOODS_TABLE_LIST.(row).td1.align = "left";
		GameInterface.GOODS_TABLE_LIST.(row).td2.str = GetCargoGoods(refCharacter, i);
		GameInterface.GOODS_TABLE_LIST.(row).td3.str = Goods[i].Weight + " / " + Goods[i].Units;
		n++;
	}
	GameInterface.GOODS_TABLE_LIST.select = 0;
	Table_UpdateWindow("GOODS_TABLE_LIST");

	RefreshCargoInfo(refCharacter);
}

// -------- окно ввода количества (общий TG-попап) --------

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
	string sGood = Goods[iCurGoodIndex].name;
	SendMessage(&GameInterface, "lslss", MSG_INTERFACE_MSG_TO_NODE, "TG_GOODS_PICTURE", 6, "GOODS", sGood);
	SetFormatedText("TG_GOODS_CAPTION", XI_ConvertString(sGood));
	SetFormatedText("TG_GOODS_INFO", GetAssembledString(GetConvertStr(sGood + "_descr", "GoodsDescribe.txt"), &Goods[iCurGoodIndex]));

	String companionId = Characters[iCurCompanion].Id;
	ref refCharacter = characterFromID(companionId);
	GameInterface.TG_EDIT.str = GetCargoGoods(refCharacter, iCurGoodIndex);
}

void ShowItemInfo()
{
	ShowWindow(1);
	ShowTransferGoods();
}

void OnAddBtnClick(int _add)
{
	int iNum = MakeInt(GameInterface.TG_EDIT.str);
	ref rGood = &Goods[iCurGoodIndex];
	int _Units = sti(rGood.Units);
	_add = _add * abs(_add);
	iNum = iNum + _add * _Units;
	if (iNum < 0) iNum = 0;
	if (iNum > 99999) iNum = 99999;
	GameInterface.TG_EDIT.str = iNum;
}

// -------- команды --------

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	int iMult = 1;
	if (comName == "rclick") iMult = 5;

	String companionId = Characters[iCurCompanion].Id;
	ref refCharacter = characterFromID(companionId);
	int iNum;

	switch(nodName)
	{
		case "TG_ADD_ALL_BUTTON":    OnAddBtnClick(10 * iMult);  break;
		case "TG_ADD_BUTTON":        OnAddBtnClick(1 * iMult);   break;
		case "TG_REMOVE_ALL_BUTTON": OnAddBtnClick(-10 * iMult); break;
		case "TG_REMOVE_BUTTON":     OnAddBtnClick(-1 * iMult);  break;

		case "TG_CANCEL_BUTTON":
			if(comName == "click" || comName == "activate") UnShowWindow();
			break;

		case "TG_OK_BUTTON":
			if(comName == "click" || comName == "activate")
			{
				iNum = sti(GameInterface.TG_EDIT.str);
				if (iNum > 99999) iNum = 99999;
				if (iNum < 0) iNum = 0;
				SetCharacterGoods(refCharacter, iCurGoodIndex, iNum);   // абсолютная установка + пересчёт трюма
				FillGoodsTable();
				UnShowWindow();
			}
			break;

		case "FILL_HOLD":
			if(comName == "click" || comName == "activate")
			{
				AddCharacterGoodsSimple(refCharacter, iCurGoodIndex, 999999);  // добить выбранный товар под завязку
				FillGoodsTable();
			}
			break;

		case "CLEAR_CARGO":
			if(comName == "click" || comName == "activate")
			{
				DeleteAttribute(refCharacter, "Ship.Cargo.Goods");
				RecalculateCargoLoad(refCharacter);
				FillGoodsTable();
			}
			break;
	}
}

void TableSelectChange()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();
	string sRow = "tr" + (iSelected);

	if (sControl == "GOODS_TABLE_LIST")
	{
		iCurGoodIndex = sti(GameInterface.GOODS_TABLE_LIST.(sRow).index);
		sCurGoodRow = sRow;
	}
}

void ProcessFrame()
{
	if (GetCurrentNode() == "SHIPS_SCROLL")
	{
		if (sti(GameInterface.SHIPS_SCROLL.current) != nCurScrollNumS)
		{
			nCurScrollNumS = sti(GameInterface.SHIPS_SCROLL.current);
			FillGoodsTable();
		}
	}
}
