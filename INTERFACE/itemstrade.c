#include "interface\itemstrade_equip.c"

int	nCurScrollNum = 0;
int iCharCapacity;
int iTotalSpace;
float fCharWeight, fStoreWeight;
int iMaxGoodsStore = 50000;
bool nocheck = true;

bool bTradeLog = true;
bool bFilteredGroup = false;
string sFilteredGroup;
string sLastTable = "";
int iLastRow = 0;

int  BuyOrSell = 0; // 1-buy -1 sell
string sChrId;
ref refCharacter, refStoreChar;	// текущий ГГ (оф) и торгаш
int iCharQty, iStoreQty, iCharPrice, iStorePrice;
float fWeight;
string sCurItem;
int chestsnum;
aref box1;
aref box2;
aref box3;
aref box4;

void InitInterface_R(string iniName, ref pTrader)
{
	refCharacter = pchar;
	refStoreChar = pTrader;
	StartAboveForm(true);
	FillCharactersScroll();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	CreateString(true,"CharName","",FONT_NORMAL,COLOR_NORMAL, 400,106,SCRIPT_ALIGN_CENTER,1.0);
	SetCharWeight();
	SetDescription();
	FillCharacterInfo();

	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);// Центрировать текст вертикально

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessBreakExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);

	SetEventHandler("OnTableClick", "OnTableClick", 0);
	SetEventHandler("MouseRClickUP","EndTooltip",0);
	SetEventHandler("TableSelectChange", "CS_TableSelectChange", 0);
	SetEventHandler("Autotrade_All", "Autotrade_All", 0);
	SetEventHandler("Autotrade_This", "Autotrade_This", 0);
	SetEventHandler("TransactionOK", "TransactionOK", 0);
	SetEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT", 0);
	SetEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON", 0);
	SetEventHandler("ADD_BUTTON","ADD_BUTTON",0);
	SetEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON", 0);
	SetEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON", 0);
	SetEventHandler("deactivate_log", "deactivate_log", 0);
	SetEventHandler("dblclickTable", "dblclickTable", 0);

	SetEventHandler("frame","ProcessFrame",1);
	SetEventHandler("ShowEquipWindow", "ShowEquipWindow", 0);
	SetEventHandler("ExitEquipWindow", "ExitEquipWindow", 0);
	SetEventHandler("ShowBladeEquipInfo", "ShowBladeEquipInfo", 0);
	SetEventHandler("ShowGunEquipInfo", "ShowGunEquipInfo", 0);
	SetEventHandler("ShowSpyglassEquipInfo", "ShowSpyglassEquipInfo", 0);
	SetEventHandler("ShowCirassEquipInfo", "ShowCirassEquipInfo", 0);
	SetEventHandler("ShowBackPackEquipInfo", "ShowBackPackEquipInfo", 0);
	SetEventHandler("ShowTalismanEquipInfo", "ShowTalismanEquipInfo", 0);
	SetEventHandler("ShowBookEquipInfo", "ShowBookEquipInfo", 0);
	SetEventHandler("Show1EquipInfo", "Show1EquipInfo", 0);
	SetEventHandler("Show2EquipInfo", "Show2EquipInfo", 0);
	SetEventHandler("Show3EquipInfo", "Show3EquipInfo", 0);
	SetEventHandler("Show4EquipInfo", "Show4EquipInfo", 0);
	SetEventHandler("Show5EquipInfo", "Show5EquipInfo", 0);

	ResetTableHeader("TABLE_LOG");
	deactivate_log();//изначально активен, выключаем
	SetNodeUsing("TRADE_LOG",  false);

	SetFormatedText("STORECAPTION1", XI_ConvertString("titleItemsTrade"));
	SetNewPicture("OTHER_PICTURE", "interfaces\portraits\face_" + its(refStoreChar.FaceId) + ".tga");
	if (!IsPCharHaveTreasurer()) {SetNodeUsing("Autotrade_All",  false); SetNodeUsing("Autotrade_This",  false);}//скрываем автозакупку без казначея

	switch (refCharacter.sex)
	{
		case "man": SetNewPicture("EQUIP_BIG_PICTURE", "interfaces\sith\CharEquip_Man.dds");
		break;
		case "woman": SetNewPicture("EQUIP_BIG_PICTURE", "interfaces\sith\CharEquip_Woman.dds");
		break;
		case "skeleton": SetNewPicture("EQUIP_BIG_PICTURE", "interfaces\sith\CharEquip_Skeleton.dds");
		break;
	}
	LAi_SetActorTypeNoGroup(Pchar);
}

void ProcessBreakExit()
{
	if (bTradeLog) {deactivate_log(); return;}
	if (sti(GameInterface.qty_edit.str) != 0) {GameInterface.qty_edit.str = 0; return;} //по Esc сбрасываем введенное число предметов, и не закрываем интерфейс сразу
	if (bFilteredGroup) {bFilteredGroup = false; AddToTable();return;}//второй Esc - сбрасываем фильтрацию вместо выхода
	IDoExit( RC_INTERFACE_FOOD_INFO_EXIT );//третий Ecs - закрываем интерфейс
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void IDoExit(int exitCode)
{
	CloseTooltip();
	LAi_SetPlayerType(PChar);
	// барыга: после закрытия окна торговли снова начинает бродить по городу
	if (CheckAttribute(refStoreChar, "isFence"))
	{
		refStoreChar.talker = 0;          // значок остаётся, но сам не заговаривает
		LAi_SetCitizenType(refStoreChar); // возвращаем режим горожанина (бродит)
	}
	CheckAndSetOverloadMode(GetMainCharacter());// boal проверка на перегруз 21.01.2004
	RefreshEquippedMaps(GetMainCharacter()); // рефрешим карты, если купили

	int i, cn;
	for(i=1;i<=MAX_NUM_FIGHTERS;i++)	// раздайте патроны, порутчик Голицын <--
	{
		cn = GetOfficersIndex(pchar, i);
		if (cn!=-1)
		{
			refCharacter = GetCharacter(cn);
			CheckAndSetOverloadMode(refCharacter);
			if (CheckAttribute(refCharacter, "skill.Fencing")) //boal fix иначе не берут саблю при перегрузе
			{
				EquipBestByGroup(refCharacter, BLADE_ITEM_TYPE);
				EquipBestByGroup(refCharacter, BACKPACK_ITEM_TYPE);
				EquipBestByGroup(refCharacter, TALISMAN_ITEM_TYPE);
			}
			if (CheckAttribute(refCharacter, "skill.Pistol") && GetCharacterSkill(refCharacter,"Pistol") > 0.1 )
			{
				EquipBestByGroup(refCharacter, GUN_ITEM_TYPE);
			}
		}
	}
	EndAboveForm(true);
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessBreakExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("OnTableClick", "OnTableClick");
	DelEventHandler("MouseRClickUP","EndTooltip");
	DelEventHandler("TableSelectChange", "CS_TableSelectChange");
	DelEventHandler("frame","ProcessFrame");
	DelEventHandler("TransactionOK", "TransactionOK");
	DelEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT");
	DelEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON");
	DelEventHandler("ADD_BUTTON","ADD_BUTTON");
	DelEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON");
	DelEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON");
	DelEventHandler("ShowEquipWindow", "ShowEquipWindow");
	DelEventHandler("ExitEquipWindow", "ExitEquipWindow");
	DelEventHandler("ShowBladeEquipInfo", "ShowBladeEquipInfo");
	DelEventHandler("ShowGunEquipInfo", "ShowGunEquipInfo");
	DelEventHandler("ShowSpyglassEquipInfo", "ShowSpyglassEquipInfo");
	DelEventHandler("ShowCirassEquipInfo", "ShowCirassEquipInfo");
	DelEventHandler("ShowBackPackEquipInfo", "ShowBackPackEquipInfo");
	DelEventHandler("ShowTalismanEquipInfo", "ShowTalismanEquipInfo");
	DelEventHandler("ShowBookEquipInfo", "ShowBookEquipInfo");
	DelEventHandler("Show1EquipInfo", "Show1EquipInfo");
	DelEventHandler("Show2EquipInfo", "Show2EquipInfo");
	DelEventHandler("Show3EquipInfo", "Show3EquipInfo");
	DelEventHandler("Show4EquipInfo", "Show4EquipInfo");
	DelEventHandler("Show5EquipInfo", "Show5EquipInfo");
	DelEventHandler("Autotrade_All", "Autotrade_All");
	DelEventHandler("Autotrade_This", "Autotrade_This");
	DelEventHandler("deactivate_log", "deactivate_log");
	DelEventHandler("dblclickTable", "dblclickTable");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
		case "QTY_OK_BUTTON":
			if(comName=="leftstep") ADD_BUTTON();
			if(comName=="rightstep") REMOVE_BUTTON();
			if(comName=="speedleft") ADD_ALL_BUTTON();
			if(comName=="speedright") REMOVE_ALL_BUTTON();
		break;

		case "QTY_CANCEL_BUTTON":
			if(comName=="leftstep") ADD_BUTTON();
			if(comName=="rightstep") REMOVE_BUTTON();
			if(comName=="speedleft") ADD_ALL_BUTTON();
			if(comName=="speedright") REMOVE_ALL_BUTTON();
		break;

		case "TABLE_LIST":
			if(comName=="leftstep") ADD_BUTTON();
			if(comName=="rightstep") REMOVE_BUTTON();
			if(comName=="speedleft") ADD_ALL_BUTTON();
			if(comName=="speedright") REMOVE_ALL_BUTTON();
		break;
		case "TABLE_LIST_TRADER":
			if(comName=="leftstep") ADD_BUTTON();
			if(comName=="rightstep") REMOVE_BUTTON();
			if(comName=="speedleft") ADD_ALL_BUTTON();
			if(comName=="speedright") REMOVE_ALL_BUTTON();
		break;
	}
}

void OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();
	string sRow = "tr" + (iRow + 1);

	if (!SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sControl, 1 )) SortTable(sControl, iColumn);

	Table_UpdateWindow(sControl);
}

void AddToTable()
{
	int n, i, iTemp;
	string row, sItem;
	int idLngFile;
	int leftQty, rightQty;
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	Table_Clear("TABLE_LIST", true, true, false);
	Table_Clear("TABLE_LIST_TRADER", true, true, false);
	ResetTableHeader("TABLE_LIST");
	ResetTableHeader("TABLE_LIST_TRADER");

	object allItems;
	aref arInventory, arInventory2, arItem;
	ref rItem;

	makearef(arInventory, refCharacter.Items);
	n = GetAttributesNum(arInventory);
	for (i = 0; i < n; i++)
	{
		arItem = GetAttributeN(arInventory, i);
		sItem = GetAttributeName(arItem);
		row = DigitsToString(FindItem(sItem), 4);
		if (IsGenerableItem(sItem))
			row = "a" + row + sItem;// Сначала уникальные предметы
		else row = "b" + row;
		allItems.(row) = sItem;
	}
	makearef(arInventory, allItems);
	sort(arInventory);//по порядку предметы игрока

	n = 1;
	for (i = 0; i < GetAttributesNum(arInventory); i++)
	{
		arItem = GetAttributeN(arInventory, i);
		sItem = GetAttributeValue(arItem);
		rItem = ItemsFromID(sItem);

		if (IsQuestUsedItem(sItem)) continue;//квестовые пропускаем
		leftQty  = GetCharacterFreeItem(refCharacter, sItem);
		if (leftQty == 0) continue;//экипированные пропускаем
		if (bFilteredGroup) 
		{
			if (CheckAttribute(rItem, "groupID"))
			{
				if (rItem.groupID != sFilteredGroup) continue;	//экипировка, карты, сундуки
			}
			else
			{
				if (CheckAttribute(rItem, "sortIndex") && (rItem.sortIndex == sFilteredGroup) ) {;} else {continue;} //зелья и еда
			}
		}

		row = "tr" + n;
		GameInterface.TABLE_LIST.(row).id = sItem;
		GameInterface.TABLE_LIST.(row).index = n;
		GameInterface.TABLE_LIST.(row).td2.str = leftQty;
		GameInterface.TABLE_LIST.(row).td2.fontidx = 5;
		GameInterface.TABLE_LIST.(row).td3.str = FloatToString(GetItemWeight(sItem) * leftQty, 1);
		GameInterface.TABLE_LIST.(row).td4.str = GetTradeItemPrice(sItem, PRICE_TYPE_SELL);
		GameInterface.TABLE_LIST.(row).td1.icon.group = rItem.picTexture;
		GameInterface.TABLE_LIST.(row).td1.icon.image = "itm" + rItem.picIndex;
		GameInterface.TABLE_LIST.(row).td1.icon.offset = "-2, 0";
		GameInterface.TABLE_LIST.(row).td1.icon.width = 30;
		GameInterface.TABLE_LIST.(row).td1.icon.height = 30;
		GameInterface.TABLE_LIST.(row).td1.textoffset = "30,0";
		GameInterface.TABLE_LIST.(row).td1.str = LanguageConvertString(idLngFile, rItem.name);
		GameInterface.TABLE_LIST.(row).td1.scale = 0.9;
		iTemp = CheckItemInSets(refCharacter, sItem);
		if (iTemp > 0)
		{
			GameInterface.TABLE_LIST.(row).td1.str = GameInterface.TABLE_LIST.(row).td1.str + GetConvertStrWithReplace("Variable_itemstrade_6", "Interface.txt", "#space#", " ") + iTemp + ")";
			GameInterface.TABLE_LIST.(row).td1.color = argb(255,196,196,255);//подцвечиваем предметы из комплектов
			GameInterface.TABLE_LIST.(row).td2.color = argb(255,196,196,255);
		}
		n++;
	}

	makearef(arInventory2, refStoreChar.Items);
	n = GetAttributesNum(arInventory2);
	for (i = 0; i < n; i++)
	{
		arItem = GetAttributeN(arInventory2, i);
		sItem = GetAttributeName(arItem);
		row = DigitsToString(FindItem(sItem), 4);
		if (IsGenerableItem(sItem))
			row = "a" + row + sItem;// Сначала уникальные предметы
		else row = "b" + row;
		allItems.(row) = sItem;
	}
	makearef(arInventory2, allItems);
	sort(arInventory2);//по порядку предметы торговца

	n = 1;
	for (i = 0; i < GetAttributesNum(arInventory2); i++)
	{
		arItem = GetAttributeN(arInventory2, i);
		sItem = GetAttributeValue(arItem);
		rItem = ItemsFromID(sItem);

		if (IsQuestUsedItem(sItem)) continue;//квестовые пропускаем
		rightQty  = GetCharacterFreeItem(refStoreChar, sItem);
		if (rightQty == 0) continue;//экипированные пропускаем
		if (bFilteredGroup) 
		{
			if (CheckAttribute(rItem, "groupID"))
			{
				if (rItem.groupID != sFilteredGroup) continue;	//экипировка, карты, сундуки
			}
			else
			{
				if (CheckAttribute(rItem, "sortIndex") && (rItem.sortIndex == sFilteredGroup) ) {;} else {continue;} //зелья и еда
			}
		}

		row = "tr" + n;
		GameInterface.TABLE_LIST_TRADER.(row).id = sItem;
		GameInterface.TABLE_LIST_TRADER.(row).index = n;
		GameInterface.TABLE_LIST_TRADER.(row).td1.icon.group = rItem.picTexture;
		GameInterface.TABLE_LIST_TRADER.(row).td1.icon.image = "itm" + rItem.picIndex;
		GameInterface.TABLE_LIST_TRADER.(row).td1.icon.offset = "-2, 0";
		GameInterface.TABLE_LIST_TRADER.(row).td1.icon.width = 30;
		GameInterface.TABLE_LIST_TRADER.(row).td1.icon.height = 30;
		GameInterface.TABLE_LIST_TRADER.(row).td1.textoffset = "30,0";
		GameInterface.TABLE_LIST_TRADER.(row).td1.str = LanguageConvertString(idLngFile, rItem.name);
		GameInterface.TABLE_LIST_TRADER.(row).td1.scale = 0.9;
		GameInterface.TABLE_LIST_TRADER.(row).td2.str = rightQty;
		GameInterface.TABLE_LIST_TRADER.(row).td3.str = FloatToString(GetItemWeight(sItem) * rightQty, 1);
		GameInterface.TABLE_LIST_TRADER.(row).td4.str = GetTradeItemPrice(sItem, PRICE_TYPE_BUY);

		iTemp = CheckItemInSets(refStoreChar, sItem);
		n++;
	}
	Table_UpdateWindow("TABLE_LIST");
	Table_UpdateWindow("TABLE_LIST_TRADER");

	LanguageCloseFile(idLngFile);
}

void ProcessFrame()
{
	if(sti(GameInterface.CHARACTERS_SCROLL.current) != nCurScrollNum && GetCurrentNode() == "CHARACTERS_SCROLL")
	{
		nCurScrollNum = sti(GameInterface.CHARACTERS_SCROLL.current);
		string sAttr = "pic" + (nCurScrollNum + 1);
		refCharacter = &characters[sti(GameInterface.CHARACTERS_SCROLL.(sAttr).character)];
		switch (refCharacter.sex)
		{
			case "man": SetNewPicture("EQUIP_BIG_PICTURE", "interfaces\sith\CharEquip_Man.dds");
			break;
			case "woman": SetNewPicture("EQUIP_BIG_PICTURE", "interfaces\sith\CharEquip_Woman.dds");
			break;
			case "skeleton": SetNewPicture("EQUIP_BIG_PICTURE", "interfaces\sith\CharEquip_Skeleton.dds");
			break;
		}
		if (HasSubStr(GameInterface.CHARACTERS_SCROLL.(sAttr).character,"box"))
		{
			switch (GameInterface.CHARACTERS_SCROLL.(sAttr).character)
			{
				case "box1": makeref(refCharacter,box1) break;
				case "box2": makeref(refCharacter,box2) break;
				case "box3": makeref(refCharacter,box3) break;
				case "box4": makeref(refCharacter,box4) break;
			}
		}
		SetDescription();
		if (sLastTable == "TABLE_LIST") {iLastRow = 1;}
		GameInterface.TABLE_LIST.select = 1;
		GameInterface.TABLE_LIST.top = 0;
		ShowGoodsInfo();
		FillCharacterInfo();
	}
}

void SetVariable()//вес предметов и кол-во золота
{
	string sText, sTextSecond;

	iCharCapacity = GetMaxItemsWeight(refCharacter);
	sText  = FloatToString(fCharWeight, 1) + " / " + iCharCapacity;
	SetFormatedText("CAPACITY", XI_ConvertString("Weight") + ":\n" + sText);

	iTotalSpace = iMaxGoodsStore;
	string sMaxGoodsStore;
	sMaxGoodsStore = GetConvertStrWithReplace("Variable_itemstrade_12", "Interface.txt", "#space#", " ");
	SetFormatedText("STORE_CAPACITY", sMaxGoodsStore);

	sText = XI_ConvertString("OurMoney") + " " + FindRussianMoneyString(sti(pchar.money));
	SetFormatedText("OUR_GOLD", sText);
}

void SetDescription()
{
 	if (GetCurrentNode() == "CHARACTERS_SCROLL")
	{
		string attributeName = attributeName = "pic" + (nCurScrollNum+1);
		if(CheckAttribute(&GameInterface, "CHARACTERS_SCROLL." + attributeName)) SetCharWeight();
	}
	AddToTable();
	SetVariable();//вес предметов и кол-во золота
}

void SetCharWeight()
{
	fCharWeight  = GetItemsWeight(refCharacter);
	fStoreWeight = 0;
}

void ShowGoodsInfo()// инфа о предмете
{
	string sRow = "tr" + (iLastRow);
	if (checkattribute(GameInterface, sLastTable + "." + sRow + ".id" )) 
	{
		sCurItem = GameInterface.(sLastTable).(sRow).id; 
	}
	else
	{
		if (sLastTable == "TABLE_LIST") sLastTable = "TABLE_LIST_TRADER"; else sLastTable = "TABLE_LIST"; 
		iLastRow = sti(GameInterface.(sLastTable).select) - 1;
	}

	// LEO: ветка else выше НЕ переустанавливает sCurItem - в строке таблицы не оказалось id, и здесь
	// оставался предмет от прошлого показа (или пусто). FindItem тогда возвращает -1, Items[-1] давал
	// "invalid index -1" и валил всю функцию по стеку. Показывать нечего - молча выходим.
	int iItemIdx = FindItem(sCurItem);
	if (iItemIdx < 0) return;
	ref	arItm = &Items[iItemIdx];
	string GoodName = arItm.name;
	int	lngFileID = LanguageOpenFile("ItemsDescribe.txt");
	string sHeader = LanguageConvertString(lngFileID, GoodName);

	string describeStr = "";

	if (bBettaTestMode)
	{
		describeStr += " id = " + sCurItem + NewStr();
		if(CheckAttribute(arItm, "dmg_min.old") && CheckAttribute(arItm, "dmg_max.old"))
		{
			describeStr += "OldDmgMin = " + sti(arItm.dmg_min.old) + ", OldDmgMax = "+sti(arItm.dmg_max.old) + newStr();
		}
	}
	describeStr += GetItemDescribe(sCurItem);

	fWeight = GetItemWeight(sCurItem);

	BuyOrSell = 0;
	SetFormatedText("QTY_TypeOperation", "");
	SetFormatedText("QTY_Result", "");
	GameInterface.qty_edit.str = "0";

	SetNewGroupPicture("QTY_GOODS_PICTURE", arItm.picTexture, "itm" + arItm.picIndex);
	SetFormatedText("QTY_CAPTION", sHeader);
	SetFormatedText("QTY_GOODS_INFO", describeStr);
	LanguageCloseFile(lngFileID);

	iCharQty = GetCharacterFreeItem(refCharacter, sCurItem);
	iStoreQty = GetCharacterFreeItem(refStoreChar, sCurItem);

	SetFormatedText("QTY_INFO_STORE_QTY", its(iStoreQty));
	SetFormatedText("QTY_INFO_SHIP_QTY", its(iCharQty));
	BuyOrSell = 0;

	iStorePrice = GetTradeItemPrice(sCurItem, PRICE_TYPE_SELL);
	SetFormatedText("QTY_INFO_STORE_PRICE",XI_ConvertString("Cost") + NewStr() + its(iStorePrice));

	iCharPrice = GetTradeItemPrice(sCurItem, PRICE_TYPE_BUY);
	SetFormatedText("QTY_INFO_SHIP_PRICE", XI_ConvertString("Cost") + NewStr() + its(iCharPrice));
}

int FindTradeTableRow(string sTable, string sItem)
{
	string sRow;
	aref aRows;
	makearef(aRows, GameInterface.(sTable));
	int iLinesMax = GetAttributesNum(aRows);
	for (int n = 0; n < iLinesMax; n++)
	{
		sRow = GetAttributeName(GetAttributeN(aRows, n));
		if (!HasSubStr(sRow, "tr")) continue;
		if (!CheckAttribute(GameInterface, sTable + "." + sRow + ".id")) continue;
		if (GameInterface.(sTable).(sRow).id == sItem)
		{
			return makeint(strcut(sRow, 2, strlen(sRow) - 1));
		}
	}
	return 0;
}

int GetTradeTableRowsCount(string sTable)
{
	int iCount = 0;
	string sRow;
	aref aRows;
	makearef(aRows, GameInterface.(sTable));
	int iLinesMax = GetAttributesNum(aRows);
	for (int n = 0; n < iLinesMax; n++)
	{
		sRow = GetAttributeName(GetAttributeN(aRows, n));
		if (HasSubStr(sRow, "tr")) iCount++;
	}
	return iCount;
}

void UpdateTradeTableRow(string sTable, int iRow, string sItem, int iQty, int iPriceType)
{
	string sRow = "tr" + iRow;
	GameInterface.(sTable).(sRow).td2.str = iQty;
	GameInterface.(sTable).(sRow).td3.str = FloatToString(GetItemWeight(sItem) * iQty, 1);
	GameInterface.(sTable).(sRow).td4.str = GetTradeItemPrice(sItem, iPriceType);
}

bool TryUpdateCurrentTradeRows(string sItem)
{
	int leftQty = GetCharacterFreeItem(refCharacter, sItem);
	int rightQty = GetCharacterFreeItem(refStoreChar, sItem);
	int leftRow = FindTradeTableRow("TABLE_LIST", sItem);
	int rightRow = FindTradeTableRow("TABLE_LIST_TRADER", sItem);
	bool bLeftRowMustDisappear = leftQty == 0 && leftRow > 0;
	bool bRightRowMustDisappear = rightQty == 0 && rightRow > 0;
	bool bLeftRowMustAppear = leftQty > 0 && leftRow == 0;
	bool bRightRowMustAppear = rightQty > 0 && rightRow == 0;

	if (bLeftRowMustDisappear || bRightRowMustDisappear) return false;
	if (bLeftRowMustAppear || bRightRowMustAppear) return false;

	if (leftRow > 0) UpdateTradeTableRow("TABLE_LIST", leftRow, sItem, leftQty, PRICE_TYPE_SELL);
	if (rightRow > 0) UpdateTradeTableRow("TABLE_LIST_TRADER", rightRow, sItem, rightQty, PRICE_TYPE_BUY);

	Table_UpdateWindow("TABLE_LIST");
	Table_UpdateWindow("TABLE_LIST_TRADER");
	return true;
}

void RefreshCurrentGoodsNumbers()
{
	iCharQty = GetCharacterFreeItem(refCharacter, sCurItem);
	iStoreQty = GetCharacterFreeItem(refStoreChar, sCurItem);

	SetFormatedText("QTY_INFO_STORE_QTY", its(iStoreQty));
	SetFormatedText("QTY_INFO_SHIP_QTY", its(iCharQty));

	iStorePrice = GetTradeItemPrice(sCurItem, PRICE_TYPE_SELL);
	iCharPrice = GetTradeItemPrice(sCurItem, PRICE_TYPE_BUY);
	SetFormatedText("QTY_INFO_STORE_PRICE", XI_ConvertString("Cost") + NewStr() + its(iStorePrice));
	SetFormatedText("QTY_INFO_SHIP_PRICE", XI_ConvertString("Cost") + NewStr() + its(iCharPrice));

	GameInterface.qty_edit.str = 0;
	BuyOrSell = 0;
	SetFormatedText("QTY_TypeOperation", "");
	SetFormatedText("QTY_Result", "");
	SetCharWeight();
	SetVariable();
}

void TransactionOK()
{
	bool bFastRowsUpdate;
	int iSelectedRow;
	int nTradeQuantity, moneyback;
	ChangeQTY_EDIT();
	nTradeQuantity = sti(GameInterface.qty_edit.str);
	if (BuyOrSell == 0)
	{
		EndTooltip();
		return;
	}

 	if (BuyOrSell == 1) // BUY
	{
		if (loadedLocation.id == "EsmeraldaStoreBig" && sCurItem == "map_LSC") bDisableFastReload = false;
		moneyback = makeint(iCharPrice*stf(GameInterface.qty_edit.str) + 0.5);
		TakeNItems(refCharacter, sCurItem, nTradeQuantity);
		TakeNItems(refStoreChar, sCurItem, -nTradeQuantity);
		AddTradeLog(sCurItem, nTradeQuantity, -moneyback);
		pchar.money = sti(pchar.money) - moneyback;
		Statistic_AddValue(Pchar, "Money_spend", moneyback);
		AddCharacterExpToSkill(pchar, "Commerce", moneyback / 700.0);// boal  check skill
		WaitDate("",0,0,0, 0, 1);
	}
 	else
	{ // SELL
		moneyback = makeint(iStorePrice*stf(GameInterface.qty_edit.str) + 0.5);
		TakeNItems(refCharacter, sCurItem, -nTradeQuantity);
		TakeNItems(refStoreChar, sCurItem, nTradeQuantity);
		AddTradeLog(sCurItem, -nTradeQuantity, moneyback);
  		pchar.money = sti(pchar.money) + moneyback;
		Statistic_AddValue(Pchar, "Money_get", moneyback);
		AddCharacterExpToSkill(pchar, "Commerce", moneyback / 1000.0);// boal  check skill
		WaitDate("",0,0,0, 0, 1);
	}
	bFastRowsUpdate = TryUpdateCurrentTradeRows(sCurItem);
	if (!bFastRowsUpdate)
	{
		AddToTable();
		// LEO: пересборка могла укоротить список - движок сам ставит выделение на новую
		//   последнюю строку и пишет её номер в "select". Прежний ручной ACTION_UPSTEP был тут
		//   лишним и уводил выделение на ДВЕ строки вверх. Номер строки берём обратно у движка:
		//   сдвиг выделения при клампе события TableSelectChange НЕ шлёт, и без этого
		//   ShowGoodsInfo смотрел в исчезнувшую строку.
		if (CheckAttribute(&GameInterface, sLastTable + ".select"))
		{
			iSelectedRow = sti(GameInterface.(sLastTable).select);
			if (iSelectedRow > 0) iLastRow = iSelectedRow;
		}
	}
	Table_UpdateWindow("TABLE_LOG");
	EndTooltip();
	if (bFastRowsUpdate) RefreshCurrentGoodsNumbers();
	else
	{
		GameInterface.qty_edit.str = 0;
		ChangeQTY_EDIT();
		ShowGoodsInfo();
	}
}

void confirmChangeQTY_EDIT()
{
	ChangeQTY_EDIT();
	SetCurrentNode("QTY_OK_BUTTON");
}

void ChangeQTY_EDIT()
{
	float iWeight = 0.0;
	SetCharWeight();
	GameInterface.qty_edit.str = sti(GameInterface.qty_edit.str);
	if (sti(GameInterface.qty_edit.str) == 0)
	{
		SetFormatedText("QTY_TypeOperation", "");
		SetFormatedText("QTY_Result", "");
		BuyOrSell = 0;
	}
	else
	{
		if (sti(GameInterface.qty_edit.str) < 0 || BuyOrSell == -1)
		{
			if (BuyOrSell != -1)
			{
				GameInterface.qty_edit.str = -sti(GameInterface.qty_edit.str);
			}
			BuyOrSell = -1;
			// проверка на колво доступное -->
			if (sti(GameInterface.qty_edit.str) > iCharQty)
			{
				GameInterface.qty_edit.str = iCharQty;
			}
			iWeight = (fWeight * sti(GameInterface.qty_edit.str));
			if ((fStoreWeight + iWeight) > iTotalSpace)
			{
				iWeight = iTotalSpace - fStoreWeight - 0.01; // чуть меньше
				GameInterface.qty_edit.str = makeint(iWeight / fWeight );
				iWeight = fWeight * sti(GameInterface.qty_edit.str);
				GameInterface.qty_edit.str = makeint(iWeight / fWeight ); // округдение
			}
			// проверка на колво доступное <--
			// квестовые не продать -->
			if (IsQuestUsedItem(sCurItem))
			{
				GameInterface.qty_edit.str = 0;
			}
			// квестовые не продать <--
			SetFormatedText("QTY_TypeOperation", GetConvertStrWithReplace("Variable_itemstrade_13", "Interface.txt", "#space#", " "));
			SetFormatedText("QTY_Result", GetConvertStrWithReplace("Variable_itemstrade_14", "Interface.txt", "#space#", " ") + makeint(iStorePrice*stf(GameInterface.qty_edit.str) + 0.5) +
							GetConvertStrWithReplace("Variable_itemstrade_15", "Interface.txt", "#space#", " ") + FloatToString(iWeight, 1) );
		}
		else
		{
			BuyOrSell = 1;
			// проверка на колво доступное -->
			if (sti(GameInterface.qty_edit.str) > iStoreQty)
			{
				GameInterface.qty_edit.str = iStoreQty;
			}
			iWeight = (fWeight * sti(GameInterface.qty_edit.str));
			if ((fCharWeight + iWeight) > iCharCapacity)
			{
				iWeight = iCharCapacity - fCharWeight - 0.01; // чуть меньше
				if (iWeight < 0) iWeight = 0;
				GameInterface.qty_edit.str = makeint(iWeight / fWeight );
				iWeight = fWeight * sti(GameInterface.qty_edit.str);
				GameInterface.qty_edit.str = makeint(iWeight / fWeight ); // округдение
			}
			if (makeint(iCharPrice*stf(GameInterface.qty_edit.str) + 0.5) > sti(pchar.money))
			{
				GameInterface.qty_edit.str = makeint(sti(pchar.money) / iCharPrice);
				iWeight = fWeight * sti(GameInterface.qty_edit.str);
			}
			// проверка на колво доступное <--

			SetFormatedText("QTY_TypeOperation", GetConvertStrWithReplace("Variable_itemstrade_16", "Interface.txt", "#space#", " "));
			SetFormatedText("QTY_Result", GetConvertStrWithReplace("Variable_itemstrade_17", "Interface.txt", "#space#", " ") + makeint(iCharPrice*stf(GameInterface.qty_edit.str) + 0.5) +
							GetConvertStrWithReplace("Variable_itemstrade_18", "Interface.txt", "#space#", " ") + FloatToString(iWeight, 1) );
		}
	}
	// если получили ноль
	if (sti(GameInterface.qty_edit.str) == 0)
	{
		SetFormatedText("QTY_TypeOperation", "");
		SetFormatedText("QTY_Result", "");
		BuyOrSell = 0;
	}
	SetFormatedText("QTY_INFO_STORE_QTY", its(iStoreQty - BuyOrSell*sti(GameInterface.qty_edit.str)));
	SetFormatedText("QTY_INFO_SHIP_QTY", its(iCharQty + BuyOrSell*sti(GameInterface.qty_edit.str)));
	fCharWeight  = fCharWeight  + BuyOrSell * iWeight;
	fStoreWeight = fStoreWeight - BuyOrSell * iWeight;
	SetVariable();//вес предметов и кол-во золота
}

void REMOVE_ALL_BUTTON()  // продать все
{
	GameInterface.qty_edit.str = -iCharQty;
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void ADD_ALL_BUTTON()  // купить все
{
	GameInterface.qty_edit.str = iStoreQty;
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void REMOVE_BUTTON()  // продать
{
	if (BuyOrSell == 0)
	{
		GameInterface.qty_edit.str = -1;
	}
	else
	{
		if (BuyOrSell == -1)
		{
			GameInterface.qty_edit.str = -(sti(GameInterface.qty_edit.str) + 1);
		}
		else
		{
			GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) - 1);
		}
		BuyOrSell = 0;
	}
	ChangeQTY_EDIT();
}

void ADD_BUTTON()  // купить
{
	if (BuyOrSell == 0) GameInterface.qty_edit.str = 1;
	else
	{
  		if (BuyOrSell == 1) GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) + 1);
			else GameInterface.qty_edit.str = -(sti(GameInterface.qty_edit.str) - 1);
		BuyOrSell = 0;
	}
	ChangeQTY_EDIT();
}

void CS_TableSelectChange()
{
	string sTable = GetEventData();
	int iRow = GetEventData();

	if (sTable == "TABLE_LOG") return;//с этой таблицей ничего не делаем
	if (iLastRow==iRow && sLastTable==sTable) return;
	iLastRow = iRow; 
	sLastTable = sTable;

	ShowGoodsInfo();
}

void SwitchTradeItemsTable()
{
	string sActiveTable = "";
	string sNextTable = "";
	int iSelect = 0;
	int iRowsCount = 0;

	if (GetCurrentNode() == "TABLE_LIST" || GetCurrentNode() == "SCROLL_LIST") sActiveTable = "TABLE_LIST";
	else if (GetCurrentNode() == "TABLE_LIST_TRADER" || GetCurrentNode() == "SCROLL_LIST_TRADER") sActiveTable = "TABLE_LIST_TRADER";
	else if (sLastTable == "TABLE_LIST" || sLastTable == "TABLE_LIST_TRADER") sActiveTable = sLastTable;
	else sActiveTable = "TABLE_LIST";

	if (sActiveTable == "TABLE_LIST") sNextTable = "TABLE_LIST_TRADER";
	else sNextTable = "TABLE_LIST";

	iRowsCount = GetTradeTableRowsCount(sNextTable);
	if (iRowsCount <= 0) return;

	if (!CheckAttribute(&GameInterface, sNextTable + ".select")) GameInterface.(sNextTable).select = 0;
	iSelect = sti(GameInterface.(sNextTable).select);
	if (iSelect < 1) iSelect = 1;
	if (iSelect > iRowsCount) iSelect = iRowsCount;

	GameInterface.(sNextTable).select = iSelect;
	if (sti(GameInterface.(sNextTable).top) > (iSelect - 1)) GameInterface.(sNextTable).top = iSelect - 1;
	if (sti(GameInterface.(sNextTable).top) < (iSelect - 15)) GameInterface.(sNextTable).top = iSelect - 15;

	sLastTable = sNextTable;
	iLastRow = iSelect;
	SetCurrentNode(sNextTable);
	ShowGoodsInfo();
}

void FillCharactersScroll()
{
	int i;
	string faceName;
	string attributeName;
	string PsgAttrName;
	int _curCharIdx;
	ref _refCurChar;
	aref pRef, pRef2;
	bool bOk;
	DeleteAttribute(&GameInterface, "CHARACTERS_SCROLL");

	nCurScrollNum = 0;
	GameInterface.CHARACTERS_SCROLL.current = 0;
	makearef(pRef,pchar.Fellows.Passengers);

	int nListSize = GetPassengersQuantity(pchar);
	int nListSizeFree = GetNotQuestPassengersQuantity(pchar);

	GameInterface.CHARACTERS_SCROLL.NotUsed = 6;
	GameInterface.CHARACTERS_SCROLL.ListSize = nListSizeFree + 3;

	GameInterface.CHARACTERS_SCROLL.ImagesGroup.t0 = "EMPTYFACE";
	GameInterface.CHARACTERS_SCROLL.ImagesGroup.t1 = "BOX_IMAGE";

	FillFaceList("CHARACTERS_SCROLL.ImagesGroup", pchar, 2); // passengers

	GameInterface.CHARACTERS_SCROLL.BadTex1 = 0;
	GameInterface.CHARACTERS_SCROLL.BadPic1 = "emptyface";

	int m = 0;

	int iTest;
	iTest = FindColony(refStoreChar.City); // город магазина
	ref rColony, chref;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
	}
	
	bool ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea) || (Pchar.location == "Caiman_Store");
	if (ok)
	{
	    string sNation = Colonies[FindColony(loadedLocation.fastreload)].nation;
		if (sNation != "none")
		{
			i = sti(sNation);
			ok = (GetNationRelation2MainCharacter(i) == RELATION_ENEMY) || GetRelation2BaseNation(i) == RELATION_ENEMY;
			if (CheckNationLicence(i) && GetNationRelation2MainCharacter(i) != RELATION_ENEMY) ok = false;
			if (ok && (i != PIRATE)) ok = false;
			else ok = true;
		}
	}

	if (ok && Pchar.SystemInfo.CabinType != "" && sti(Pchar.Ship.Type) != SHIP_NOTUSED)//фикс - ошибки в логе, если ГГ без корабля и каюты
	{
		ref cabinloc = &locations[FindLocation(Pchar.SystemInfo.CabinType)];
		chestsnum = 0;
		if (CheckAttribute(cabinloc,"box1")) {chestsnum++; makearef(box1,cabinloc.box1);}
		if (CheckAttribute(cabinloc,"box2")) {chestsnum++; makearef(box2,cabinloc.box2);}
		if (CheckAttribute(cabinloc,"box3")) {chestsnum++; makearef(box3,cabinloc.box3);}
		if (CheckAttribute(cabinloc,"box4")) {chestsnum++; makearef(box4,cabinloc.box4);}
		for (i = 1;i <= chestsnum;i++)
		{
			attributeName = "pic" + (m + 1);
			GameInterface.CHARACTERS_SCROLL.(attributeName).character = "box"+i;
			GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = "BoxImage";
			GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","BOX_IMAGE");
			m++;
		}
		GameInterface.CHARACTERS_SCROLL.current = m;
		nCurScrollNum = m;
	}

	attributeName = "pic" + (m + 1);
	GameInterface.CHARACTERS_SCROLL.(attributeName).character = nMainCharacterIndex;
	GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(pchar);
	GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_" + pchar.FaceID);
	m++;

	for(i = 0; i < nListSize; i++)
	{
		_curCharIdx = GetPassenger(pchar, i);

		bOk = CheckAttribute(&characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true;
		if(_curCharIdx != -1  && !CheckAttribute(&characters[_curCharIdx], "isquest") && !bOk && !CheckAttribute(&characters[_curCharIdx],"nonremovable"))
		{
			if (IsOfficer(&characters[_curCharIdx]) || ok)  // boal	//если в союзном городе, то не только абордагов, а всех пассажиров
			{
				attributeName = "pic" + (m + 1);
				GameInterface.CHARACTERS_SCROLL.(attributeName).character = _curCharIdx;
				GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
				GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
				m++;
			}
		}
	}
}

void FillCharacterInfo()
{
	if (CheckAttribute(refCharacter,"faceid")) SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\portraits\face_" + refCharacter.FaceId + ".tga");
	else SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\BoxImage.dds");
	string sAttr = "pic" + (nCurScrollNum + 1);
	int iCurCharImageIndex = sti(GameInterface.CHARACTERS_SCROLL.(sAttr).character);
	string sFullName;
	if (!HasSubStr(GameInterface.CHARACTERS_SCROLL.(sAttr).character,"box")) sFullName = GetFullName(&characters[iCurCharImageIndex]);
	else
	{
		switch (GameInterface.CHARACTERS_SCROLL.(sAttr).character)
		{
			case "box1": sFullName = GetConvertStrWithReplace("Variable_itemsbox_31", "Interface.txt", "#space#", " ") break;
			case "box2": sFullName = GetConvertStrWithReplace("Variable_itemsbox_32", "Interface.txt", "#space#", " ") break;
			case "box3": sFullName = GetConvertStrWithReplace("Variable_itemsbox_33", "Interface.txt", "#space#", " ") break;
			case "box4": sFullName = GetConvertStrWithReplace("Variable_itemsbox_34", "Interface.txt", "#space#", " ") break;
		}
	}

	string sTemp = "";
	if (!isMainCharacter(&characters[iCurCharImageIndex]) && !HasSubStr(GameInterface.CHARACTERS_SCROLL.(sAttr).character,"box"))
	{
		if (isOfficer(characters[iCurCharImageIndex])) sTemp += GetIconFromFont("fighter");
		if (sti(pchar.Fellows.Passengers.navigator) == sti(characters[iCurCharImageIndex].index)) sTemp += GetIconFromFont("navigator");
		if (sti(pchar.Fellows.Passengers.boatswain) == sti(characters[iCurCharImageIndex].index)) sTemp += GetIconFromFont("boatswain");
		if (sti(pchar.Fellows.Passengers.cannoner) == sti(characters[iCurCharImageIndex].index)) sTemp += GetIconFromFont("cannoner");
		if (sti(pchar.Fellows.Passengers.doctor) == sti(characters[iCurCharImageIndex].index)) sTemp += GetIconFromFont("doctor");
		if (sti(pchar.Fellows.Passengers.carpenter) == sti(characters[iCurCharImageIndex].index)) sTemp += GetIconFromFont("carpenter");
		if (sti(pchar.Fellows.Passengers.treasurer) == sti(characters[iCurCharImageIndex].index)) sTemp += GetIconFromFont("treasurer");
	}

	GameInterface.strings.CharName = sTemp + "ð" + sFullName;
}

//описание торговли устарело - для старого интерфейса делалось
void ShowHelpHint()
{
	string sHeader;
	string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
	sPicture = "none";
	sGroup = "none";
	sGroupPicture = "none";
	string sNode = GetCurrentNode();

	sHeader = XI_ConvertString("titleItemsTrade");
	sText1 = GetConvertStr("Variable_guide_trade", "Interface.txt");
	sText3 = GetConvertStrWithReplace("Variable_itemstrade_11", "Interface.txt", "#space#", " ");
	sText2 = GetConvertStr("Variable_quick_sell", "Interface.txt");

	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,192,192,192), sText3, argb(255,255,255,255), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
}

void EndTooltip()
{
	CloseTooltip();
}

// Автозакупка по настройкам казначея в первый сундук текущей каюты капитана.
// Gregg: тело переехало в ITEMS\items_utilite.c (Trade_AutobuyChest) - его же зовёт фоновый рейд казначея в порту
void AutotradeCabinChest()
{
	object atcStat;
	ref rStat; makeref(rStat, atcStat);
	Trade_AutobuyChest(refStoreChar, false, rStat);
}

void Autotrade_All()
{
	if (!IsPCharHaveTreasurer()) {Log_Info(GetConvertStrWithReplace("Variable_itemstrade_19", "Interface.txt", "#space#", " ")); return;}
	Fastsale_trash();
	int iOfficer = -1;
	ref _chr;

	for(int z = 0; z < MAX_NUM_FIGHTERS+6; z++)
	{
		iOfficer = GetOfficersIndex(pchar, z);
		if(iOfficer != -1)
		{
			_chr = &characters[iOfficer];
			SellExcessConsume(_chr);
		}
	}
	for(z = 0; z < MAX_NUM_FIGHTERS+6; z++)
	{
		iOfficer = GetOfficersIndex(pchar, z);
		if(iOfficer != -1)
		{
			_chr = &characters[iOfficer];
			BuyConsume(_chr);
		}
	}
	AutotradeCabinChest(); // и закупка в первый сундук каюты, если настроена
	Table_UpdateWindow("TABLE_LOG");
//двумя циклами сначала продавать со всех, а потом покупать - это позволит GetConvertStrWithReplace("Variable_itemstrade_20", "Interface.txt", "#space#", " ") излишки через торговца. Но так будут теряться монеты на разнице цен.
//TODO - с одной стороны - это плата за удобство, с другой - нелепость. Позже заменить функцией равномерного распредления припасов между абордагами???

	AddToTable();
	EndTooltip();
}

void Autotrade_This()
{
	if (!IsPCharHaveTreasurer()) {Log_Info(GetConvertStrWithReplace("Variable_itemstrade_21", "Interface.txt", "#space#", " ")); return;}
	// в скролле выбран сундук каюты (у бокса нет index): автозакупка по списку сундука (только первый)
	if (!CheckAttribute(refCharacter, "index"))
	{
		string saTT = "pic" + (nCurScrollNum + 1);
		if (GameInterface.CHARACTERS_SCROLL.(saTT).character == "box1") AutotradeCabinChest();
		Table_UpdateWindow("TABLE_LOG");
		AddToTable();
		EndTooltip();
		return;
	}
	Fastsale_trash_single(refCharacter);
	SellExcessConsume(refCharacter);
	BuyConsume(refCharacter);
	Table_UpdateWindow("TABLE_LOG");
	AddToTable();
	EndTooltip();
}

void BuyConsume(ref chr)
{
	object bcStat;
	ref rStat; makeref(rStat, bcStat);
	Trade_BuyConsume(chr, refStoreChar, false, rStat);
}

void SellExcessConsume(ref chr)
{
	object secStat;
	ref rStat; makeref(rStat, secStat);
	Trade_SellExcessConsume(chr, refStoreChar, false, rStat);
}

void FastSaleCharacter(ref rChar, ref rSaleProps)
{
	aref arInventory, arItem;
	string sItem;
	ref rItem;
	int sellQty, sellcost, i;

	makearef(arInventory, rChar.Items);
	for (i = 0; i < GetAttributesNum(arInventory); i++)
	{
		arItem = GetAttributeN(arInventory, i);
		sItem = GetAttributeName(arItem);
		rItem = ItemsFromID(sItem);

		sellQty = GetCharacterFreeItem(rChar, sItem);
		if (sellQty == 0) continue;

		if ((sItem == "slave_01") || (sItem == "pistol3")) continue;

		if (CheckAttribute(rItem, "quality"))
		{
			if ((rItem.quality == "poor") || (rItem.quality == "ordinary"))
			{
				if (CheckAttribute(rItem, "groupID") && (rItem.groupID == GUN_ITEM_TYPE))
					rSaleProps.GunCount = sti(rSaleProps.GunCount) + sellQty;
				else
					rSaleProps.BladeCount = sti(rSaleProps.BladeCount) + sellQty;
			}
			else continue;
		}
		else
		{
			// mineral4 (Баклан) — они не мусор
			// jewelry11, jewelry12 — теперь нужны
			bool isRubbish =
				(sItem == "mineral2") ||	// Лютня
				(sItem == "mineral3") ||    // Свечи
				(sItem == "mineral5") ||    // Старое ведро
				(sItem == "mineral6") ||    // Коралл
				(sItem == "mineral7") ||    // Трубка
				(sItem == "mineral8") ||    // Башмак
				(sItem == "mineral9") ||    // Кружка
				(sItem == "mineral10") ||   // Мешочек соли
				(sItem == "spyglass1") ||   // Дешевая подзорная труба
				(sItem == "spyglass2") ||   // Обычная подзорная труба
				(sItem == "jewelry6") ||    // Серебряное кольцо с сапфиром
				(sItem == "jewelry7") ||    // Золотое кольцо с изумрудом
				(sItem == "jewelry10") ||   // Золотое кольцо с сапфиром
				(sItem == "jewelry13") ||   // Камея
				(sItem == "jewelry16") ||   // Ожерелье
				(sItem == "jewelry18") ||   // Золотое кольцо с рубином
				(sItem == "indian2") ||     // Пугающая фигурка
				(sItem == "indian8") ||     // Серебряный кубок
				(sItem == "indian9") ||     // Алебастровый сосуд
				(sItem == "indian13") ||    // Древняя курительница
				(sItem == "indian16");      // Раскрашенный сосуд
			bool isTalisman =               
				(sItem == "indian17") ||	// Тельная ладанка
				(sItem == "indian3") ||     // Нефритовая маска
				(sItem == "indian5") ||     // Двойная маска
				(sItem == "indian7") ||     // Идол Великой Матери
				(sItem == "indian14") ||    // Чаша Ололиуки
				(sItem == "indian12") ||    // Кубок-тотем Тепейоллотля
				(sItem == "indian10") ||    // Оберег Эхекатля
				(sItem == "indian1");       // Оберег Тлальчитонатиу
			if (isRubbish)
			{
				rSaleProps.RubbishCount = sti(rSaleProps.RubbishCount) + sellQty;
			}
			else
			{
				if (isTalisman)
				{
					sellQty -= 1;
					rSaleProps.TalismanCount = sti(rSaleProps.TalismanCount) + sellQty;
				}
				else continue;
			}
		}

		rSaleProps.Items.(sItem) = sellQty;
	}

	makearef(arInventory, rSaleProps.Items);
	for (i = 0; i < GetAttributesNum(arInventory); i++)
	{
		arItem = GetAttributeN(arInventory, i);
		sItem = GetAttributeName(arItem);
		sellQty = sti(GetAttributeValue(arItem));

		sellcost = GetTradeItemPrice(sItem, PRICE_TYPE_SELL) * sellQty;
		TakeNItems(rChar, sItem, -sellQty);
		TakeNItems(refStoreChar, sItem, sellQty);
		AddTradeLog(sItem, -sellQty, sellcost);
		rSaleProps.money = sti(rSaleProps.money) + sellcost;
	}
	Table_UpdateWindow("TABLE_LOG");
	DeleteAttribute(rSaleProps, "Items");
}

void Fastsale_trash()
{
	int iOfficer = -1;

	object sellProps;
	ref rSaleProps;
	makeref(rSaleProps, sellProps);

	rSaleProps.money = 0;
	rSaleProps.BladeCount = 0;
	rSaleProps.GunCount = 0;
	rSaleProps.RubbishCount = 0;
	rSaleProps.TalismanCount = 0;

	for (int z = 0; z < 6 + MAX_NUM_FIGHTERS; z++)
	{
		if (z == 0) iOfficer = GetMainCharacterIndex();
		else iOfficer = GetOfficersIndex(pchar, z);
		if (iOfficer != -1 && CheckAttribute(&characters[iOfficer],"TransferItems.SellGarbageRestriction"))
		{
			if (CheckAttribute(&characters[iOfficer],"fighter") || z == 0)
			{
				FastSaleCharacter(&characters[iOfficer], rSaleProps);
			}
		}
	}

	if (rSaleProps.money != 0) { // если хоть что-то продали
		if (CheckAttribute(&InterfaceStates, "AutoSellLogs"))
		{
			Log_TestInfo(GetConvertStrWithReplace("Variable_itemstrade_42", "Interface.txt", "#space#", " ") + rSaleProps.BladeCount + GetConvertStrWithReplace("Variable_itemstrade_43", "Interface.txt", "#space#", " ") + rSaleProps.GunCount +
				GetConvertStrWithReplace("Variable_itemstrade_44", "Interface.txt", "#space#", " ") + rSaleProps.TalismanCount + GetConvertStrWithReplace("Variable_itemstrade_45", "Interface.txt", "#space#", " ") + rSaleProps.RubbishCount);
		}
		AddmoneyToCharacter(PChar, sti(rSaleProps.money));
		ref rTreasurer = GetPCharTreasurerRef(); //Казначей. Ему даем экспу
		int iExpAdd = MakeInt(abs(sti(rSaleProps.money) * 0.5) / 1000) + 1;
		AddCharacterExpToSkill(rTreasurer, "Commerce", iExpAdd);
		AddCharacterExpToSkill(pchar, "Commerce", iExpAdd);
	}
}

void Fastsale_trash_single(ref character)
{
	if (!CheckAttribute(character,"TransferItems.SellGarbageRestriction")) return; //нет разрешения продавать мусор

	object sellProps;
	ref rSaleProps;
	makeref(rSaleProps, sellProps);

	rSaleProps.money = 0;
	rSaleProps.BladeCount = 0;
	rSaleProps.GunCount = 0;
	rSaleProps.RubbishCount = 0;
	rSaleProps.TalismanCount = 0;

	FastSaleCharacter(character, rSaleProps);

	if (rSaleProps.money != 0) { // если хоть что-то продали
		if(CheckAttribute(&InterfaceStates, "AutoSellLogs"))
		{
			Log_TestInfo(GetConvertStrWithReplace("Variable_itemstrade_46", "Interface.txt", "#space#", " ") + rSaleProps.BladeCount + GetConvertStrWithReplace("Variable_itemstrade_47", "Interface.txt", "#space#", " ") + rSaleProps.GunCount +
				GetConvertStrWithReplace("Variable_itemstrade_48", "Interface.txt", "#space#", " ") + rSaleProps.TalismanCount + GetConvertStrWithReplace("Variable_itemstrade_49", "Interface.txt", "#space#", " ") + rSaleProps.RubbishCount);
		}
		AddmoneyToCharacter(PChar, sti(rSaleProps.money));
		ref rTreasurer = GetPCharTreasurerRef(); //Казначей. Ему даем экспу
		int iExpAdd = MakeInt(abs(sti(rSaleProps.money) * 0.5) / 1000) + 1;
		AddCharacterExpToSkill(rTreasurer, "Commerce", iExpAdd);
		AddCharacterExpToSkill(pchar, "Commerce", iExpAdd);
	}
}

void AddTradeLog(string sItem, int sellQty, int sellcost)
{
	SetNodeUsing("TRADE_LOG",  true);
	GameInterface.TABLE_LOG.hr.td3.str = sti(GameInterface.TABLE_LOG.hr.td3.str) + sellcost;//общая сумма в заголовке
	bool isEnded = true;
	string row;
	ref rItem = ItemsFromID(sItem);

	for (int n=1;isEnded;n++)
	{
		row = "tr" + n;
		if (!CheckAttribute(GameInterface, "TABLE_LOG." + row + ".id")) //таблица кончилась, предмет не нашли
		{
			isEnded = false;
			GameInterface.TABLE_LOG.(row).id = sItem;
			GameInterface.TABLE_LOG.(row).index = n;
			int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
			GameInterface.TABLE_LOG.(row).td1.str = LanguageConvertString(idLngFile, rItem.name);
			LanguageCloseFile(idLngFile);
			GameInterface.TABLE_LOG.(row).td2.str = sellQty;
			GameInterface.TABLE_LOG.(row).td3.str = sellcost;
		}
		else
		{
			if (GameInterface.TABLE_LOG.(row).id == sItem)//предмет уже был
			{
				GameInterface.TABLE_LOG.(row).td2.str = sti(GameInterface.TABLE_LOG.(row).td2.str) + sellQty;
				GameInterface.TABLE_LOG.(row).td3.str = sti(GameInterface.TABLE_LOG.(row).td3.str) + sellcost;
				isEnded = false;
			}
		}
	}
}

void deactivate_log()
{
	if (bTradeLog)
	{
		SetNodeUsing("TABLE_LOG",  false);
		SetNodeUsing("SCROLL_LOG",  false);
		SetNodeUsing("LOG_Win_fix",  false);
		SetCurrentNode(sLastTable);
		bTradeLog = false;
	}
	else
	{
		SetNodeUsing("TABLE_LOG",  true);
		SetNodeUsing("SCROLL_LOG",  true);
		SetNodeUsing("LOG_Win_fix",  true);
		SetCurrentNode("TABLE_LOG");
		bTradeLog = true;
	}
}

void dblclickTable()
{
	if (bFilteredGroup) return;	//уже фильтровано, надо сначала отменить фильтрацию

	string sRow = "tr" + (iLastRow);
	if (!checkattribute(GameInterface, sLastTable + "." + sRow + ".id")) return;
	ref rItem = ItemsFromID(GameInterface.(sLastTable).(sRow).id)
	if (CheckAttribute(rItem, "groupID"))
	{
		bFilteredGroup = true;
		sFilteredGroup = rItem.groupID;
		AddToTable();
	}
	else
	{
		if (CheckAttribute(rItem, "sortIndex") )
		{
			bFilteredGroup = true;
			sFilteredGroup = rItem.sortIndex;// =2 для еды и зелий
			AddToTable();
		}
		else sFilteredGroup = "";
	}
}

void ResetTableHeader(string sTable)
{
	if (sTable == "TABLE_LIST")
	{
		GameInterface.TABLE_LIST.hr.td1.str = GetConvertStrWithReplace("Variable_itemstrade_3", "Interface.txt", "#space#", " ");
		GameInterface.TABLE_LIST.hr.td1.scale = 0.9;
		GameInterface.TABLE_LIST.hr.td2.str = GetConvertStrWithReplace("Variable_itemstrade_1", "Interface.txt", "#space#", " ");
		GameInterface.TABLE_LIST.hr.td2.scale = 0.9;
		GameInterface.TABLE_LIST.hr.td3.str = GetConvertStrWithReplace("Variable_itemstrade_2", "Interface.txt", "#space#", " ");
		GameInterface.TABLE_LIST.hr.td3.scale = 0.9;
		GameInterface.TABLE_LIST.hr.td4.str = XI_ConvertString("Price sell");
		GameInterface.TABLE_LIST.hr.td4.scale = 0.9;
		GameInterface.TABLE_LIST.hr.td1.sorttype = "string";
		GameInterface.TABLE_LIST.hr.td2.sorttype = "";
		GameInterface.TABLE_LIST.hr.td3.sorttype = "";
		GameInterface.TABLE_LIST.hr.td4.sorttype = "";
	}

	if (sTable == "TABLE_LIST_TRADER")
	{
		GameInterface.TABLE_LIST_TRADER.hr.td1.str = GetConvertStrWithReplace("Variable_itemstrade_3", "Interface.txt", "#space#", " ");
		GameInterface.TABLE_LIST_TRADER.hr.td1.scale = 0.9;
		GameInterface.TABLE_LIST_TRADER.hr.td2.str = GetConvertStrWithReplace("Variable_itemstrade_1", "Interface.txt", "#space#", " ");
		GameInterface.TABLE_LIST_TRADER.hr.td2.scale = 0.9;
		GameInterface.TABLE_LIST_TRADER.hr.td3.str = GetConvertStrWithReplace("Variable_itemstrade_5", "Interface.txt", "#space#", " ");
		GameInterface.TABLE_LIST_TRADER.hr.td3.scale = 0.9;
		GameInterface.TABLE_LIST_TRADER.hr.td4.str = XI_ConvertString("Price sell");
		GameInterface.TABLE_LIST_TRADER.hr.td4.scale = 0.9;
	 	GameInterface.TABLE_LIST_TRADER.hr.td1.sorttype = "string";
		GameInterface.TABLE_LIST_TRADER.hr.td2.sorttype = "";
		GameInterface.TABLE_LIST_TRADER.hr.td3.sorttype = "";
		GameInterface.TABLE_LIST_TRADER.hr.td4.sorttype = "";
	}

	if (sTable == "TABLE_LOG")
	{
		GameInterface.TABLE_LOG.hr.td1.str = GetConvertStr("itmname_gold", "ItemsDescribe.txt");
		GameInterface.TABLE_LOG.hr.td1.icon.group = "ITEMS_4";
		GameInterface.TABLE_LOG.hr.td1.icon.image = "itm2";
		GameInterface.TABLE_LOG.hr.td1.icon.offset = "-2, 0";
		GameInterface.TABLE_LOG.hr.td1.icon.width = 30;
		GameInterface.TABLE_LOG.hr.td1.icon.height = 30;
		GameInterface.TABLE_LOG.hr.td1.textoffset = "30, 0";
		GameInterface.TABLE_LOG.hr.td2.str = "-";
		GameInterface.TABLE_LOG.hr.td3.str = "0";
		GameInterface.TABLE_LOG.hr.td1.sorttype = "string";
		GameInterface.TABLE_LOG.hr.td2.sorttype = "";
		GameInterface.TABLE_LOG.hr.td3.sorttype = "";
	}
}
