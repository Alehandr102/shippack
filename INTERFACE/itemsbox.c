// Warship 06.09.08 Переделка интерфейса обыска трупов, обмена с офами и сундука
// Большая часть интерфейса - это интерфейс покупки/продажи предметов

// Gregg: РЕЖИМ КОРОБКИ. Был магической строкой, которую sGetInterfaceType() пересчитывал из sFaceID
// на каждый чих (13 вызовов), а результат клали в ЛОКАЛЬНУЮ sInterfaceType, затенявшую одноимённый
// глобал (в IDoExit жили обе сразу). Теперь режим считается ОДИН раз в InitInterface_RS.
// Вся разница между режимами собрана в функциях BoxMode_* ниже - больше её нигде искать не надо.
#define BOXMODE_EXCHANGE	0	// обмен с офицером/пассажиром
#define BOXMODE_CHEST		1	// сундук: локации, каюты (box1..4), банковская ячейка, мародёрка (LootCollector)
#define BOXMODE_DEADMAN		2	// обыск трупа
#define BOXMODE_BARREL		3	// бочка

#define INTERFACE_ITEMSBOX_CHAR_ARROYSIZE	10

int iBoxMode = BOXMODE_EXCHANGE;

int nCurScrollNum = 0;
int iCharCapacity, iCharQty, iStoreQty;
float fCharWeight, fStoreWeight, fWeight;
bool bShowChangeWin = false;
bool DelLogin = false;
int  BuyOrSell = 0;
string sFaceID, sCurItem;
ref refCharacter;
aref refToChar, arChest, arDeadChar;
String sCharactersArroy[INTERFACE_ITEMSBOX_CHAR_ARROYSIZE] = {"", "", "", "", "", "", "", "", "", ""};

int iTableAddAllBtnX = 144;
int iTableAddAllBtnY = 160;
int iTableRemoveAllBtnX = 612;
int iTableRemoveAllBtnY = 160;

int iCurrentTabMode = 3; // Текущий режим сортировки

int iLinesCount = 0;
bool bLeoDirectXfer = false; // LEO: идёт прямая перекладка стрелками - AddToTable пропускает легаси Warship-fix сдвига select (иначе на последней строке выделение/стрелки уезжают вверх)

int chestsnum;
aref box1;
aref box2;
aref box3;
aref box4;

void InitInterface_RS(string iniName, ref itemsRef, string faceID)
{
	ReplaceTreasureMapPartCopies(itemsRef, PChar);
	sFaceID = faceID;

	// Gregg: режим определяется ОДИН раз здесь (раньше sGetInterfaceType() перепарсивал sFaceID в 13 местах)
	iBoxMode = BOXMODE_EXCHANGE;
	if(faceID == "")       iBoxMode = BOXMODE_CHEST;   // сундук
	if(faceID == "Barrel") iBoxMode = BOXMODE_BARREL;  // бочонок
	if(faceID == "666")    iBoxMode = BOXMODE_DEADMAN; // мертвец

	String Tab3Caption;
	refCharacter = GetMainCharacter();
	makearef(refToChar, itemsRef);

	if(CheckAttribute(itemsRef, "Money"))
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_itemsbox_1", "Interface.txt", "#space#", " ")+itemsRef.money);
	}

	LAi_SetActorTypeNoGroup(PChar); // Тип актера - чтоб с нами могли побазарить, если увидели в сундуке

	Tab3Caption = BoxMode_Init(itemsRef); // вся режимная инициализация (пауза, арефы, снятие шмота) + подпись 3-й вкладки

	GameInterface.TABLE_LIST.hr.td1.str = GetConvertStrWithReplace("Variable_itemsbox_3", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td1.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td2.str = GetConvertStrWithReplace("Variable_itemsbox_3.1", "Interface.txt", "#space#", " ")+"\n"+GetConvertStrWithReplace("Variable_itemsbox_3.2", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td2.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td2.textoffset = "0, 2";
	GameInterface.TABLE_LIST.hr.td3.str = GetConvertStrWithReplace("Variable_itemsbox_4", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td3.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td4.str = GetConvertStrWithReplace("Variable_itemsbox_3.1", "Interface.txt", "#space#", " ")+"\n"+GetConvertStrWithReplace("Variable_itemsbox_3.2", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td4.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td4.textoffset = "0, 2";
	GameInterface.TABLE_LIST.hr.td5.str = GetConvertStrWithReplace("Variable_itemsbox_5", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td5.scale = 0.9;
//--> mod tablesort
	GameInterface.TABLE_LIST.hr.td1.sorttype = "";//числа - любое значение кроме "string"
	GameInterface.TABLE_LIST.hr.td2.sorttype = "";
	GameInterface.TABLE_LIST.hr.td3.sorttype = "string";
	GameInterface.TABLE_LIST.hr.td4.sorttype = "";
	GameInterface.TABLE_LIST.hr.td5.sorttype = "";
//<-- mod tablesort

	FillCharactersScroll();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	if (checkattribute(pchar, "iCurrentTabMode") && iBoxMode == BOXMODE_EXCHANGE) iCurrentTabMode = sti(pchar.iCurrentTabMode); //запоминание вкладки только если обмен
	SetControlsTabMode(iCurrentTabMode);

	CreateString(true, "CharName", "", FONT_NORMAL, COLOR_MONEY, 405, 108, SCRIPT_ALIGN_CENTER, 1.0);

	BoxMode_Buttons();

	// Тут позиционим кнопки взятия на таблице
	MoveTableArrows(1);

	if(iBoxMode == BOXMODE_EXCHANGE) // Установим строки с именами
	{
		GameInterface.strings.CharName = GetFullName(refToChar);
	}
	else
	{
		GameInterface.strings.CharName = GetFullName(refCharacter);
		if (iBoxMode == BOXMODE_DEADMAN)
		{
			CreateString(true, "CharName2", "", FONT_NORMAL, COLOR_MONEY, 645, 108, SCRIPT_ALIGN_RIGHT, 1.0);
			GameInterface.strings.CharName2 = GetFullName(refToChar);//имя трупа показываем под картинкой черепа
		}
	}

	SetCharWeight();
	SetVariable();

	SetCharacterMoneyToGold(refCharacter); // Переводим деньги ГГ в золото
	SetCharacterMoneyToGold(itemsRef);
	iSetCharIDToCharactersArroy(refCharacter); // Заносим в список ГГ

	AddToTable(itemsRef);
	ShowGoodsInfo(GameInterface.TABLE_LIST.tr1.id);
	BoxMode_Pictures();

	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
	SetEventHandler("MouseRClickUP","EndTooltip",0);
	SetEventHandler("ShowHelpHint", "ShowHelpHint", 0);
	SetEventHandler("ShowItemInfo", "ShowItemInfo", 0);
	SetEventHandler("TableSelectChange", "CS_TableSelectChange", 0);
	SetEventHandler("TransactionOK", "TransactionOK", 0);
	SetEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT", 0);
	SetEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON", 0);
	SetEventHandler("ADD_BUTTON","ADD_BUTTON",0);
	SetEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON", 0);
	SetEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON", 0);
	SetEventHandler("frame","ProcessFrame",1);
	SetEventHandler("eTabControlPress", "procTabChange", 0); // Событие смены закладки

	if (csmCA(pchar, "CSM.LootCollector.Run"))
		SetFormatedText("STORECAPTION1", GetConvertStrWithReplace("Variable_itemsbox_9", "Interface.txt", "#space#", " "));
	else
		SetFormatedText("STORECAPTION1", XI_ConvertString(BoxMode_TitleKey()));

	if (csmCA(pchar, "CSM.LootCollector.Run") && !csmCA(pchar, "CSM.LootCollector.Tutor"))
	{
		string sHeader, sText1, sText2, sText3;

		// 2do: потом добавить -->
		string sPicture = "none";
		string sGroup = "none";
		string sGroupPicture = "none";
		// <--

		sHeader = GetConvertStrWithReplace("Variable_itemsbox_10", "Interface.txt", "#space#", " ");
		sText1 = GetConvertStrWithReplace("Variable_itemsbox_10.1", "Interface.txt", "#space#", " ") + newStr() + newStr() +
				 GetConvertStrWithReplace("Variable_itemsbox_10.2", "Interface.txt", "#space#", " ") + newStr() +
				 GetConvertStrWithReplace("Variable_itemsbox_11", "Interface.txt", "#space#", " ");
		sText2 = GetConvertStrWithReplace("Variable_itemsbox_10.3", "Interface.txt", "#space#", " ") + newStr() +
				 GetConvertStrWithReplace("Variable_itemsbox_10.4", "Interface.txt", "#space#", " ") + newStr() +
				 GetConvertStrWithReplace("Variable_itemsbox_12", "Interface.txt", "#space#", " ");
		sText3 = GetConvertStrWithReplace("Variable_itemsbox_13", "Interface.txt", "#space#", " ");

		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,255,130), sText3, argb(255,175,255,175), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
	}

	CreateString(true, "TabCaption1", GetConvertStrWithReplace("Variable_itemsbox_14", "Interface.txt", "#space#", " "), FONT_NORMAL, COLOR_NORMAL, 130, 525, SCRIPT_ALIGN_CENTER, 0.8);
	CreateString(true, "TabCaption2", GetConvertStrWithReplace("Variable_itemsbox_15", "Interface.txt", "#space#", " "), FONT_NORMAL, COLOR_NORMAL, 310, 525, SCRIPT_ALIGN_CENTER, 0.8);
	CreateString(true, "TabCaption3", Tab3Caption, FONT_NORMAL, COLOR_NORMAL, 490, 525, SCRIPT_ALIGN_CENTER, 0.8);
	CreateString(true, "TabCaption4", GetConvertStrWithReplace("Variable_itemsbox_16", "Interface.txt", "#space#", " "), FONT_NORMAL, COLOR_NORMAL, 670, 525, SCRIPT_ALIGN_CENTER, 0.8);
	
	LAi_SetActorTypeNoGroup(Pchar);
}

// Gregg: ШАГ 4 рефакторинга — вся режимная инициализация собрана здесь.
// Пауза игры, арефы на сундук/труп, сброс DelLogin, снятие шмота, квест-перехваты.
// Возвращает подпись третьей вкладки (Tab3Caption).
string BoxMode_Init(ref itemsRef)
{
	string sTab3 = "";
	switch(iBoxMode)
	{
		case BOXMODE_CHEST:
			StartAboveForm(false); // Warship fix 17.04.09 не паузить игру. Нужно для обысков сундуков - кражи
			if(!LAi_boarding_process)
			{
				QuestCheckTakeBoxes(itemsRef); // Перехват
			}
			makearef(arChest, itemsRef);
			ShipSituation_SetQuestSituation(ShipSituation_0); // проверка квестовых ситуаций в каюте кэпа
			sTab3 = GetConvertStrWithReplace("Variable_itemsbox_6", "Interface.txt", "#space#", " ");
		break;

		case BOXMODE_BARREL:
			StartAboveForm(true);
			if(!LAi_boarding_process)
			{
				QuestCheckTakeBoxes(itemsRef); // Перехват
			}
			makearef(arChest, itemsRef);
			sTab3 = GetConvertStrWithReplace("Variable_itemsbox_7", "Interface.txt", "#space#", " ");
		break;

		case BOXMODE_DEADMAN:
			StartAboveForm(false);
			makearef(arDeadChar, itemsRef);
			DelLogin = false; // LEO: сброс флага "не обыскивать" на старте обыска (иначе залипает с прошлого трупа → ложный серый крестик)
			iSetCharIDToCharactersArroy(itemsRef); // Не нужно это, но и не помешает
			StripLowRankEquip(itemsRef);
			ShipSituation_SetQuestSituation(ShipSituation_0); // проверка квестовых ситуаций в каюте кэпа
			sTab3 = GetConvertStrWithReplace("Variable_itemsbox_2", "Interface.txt", "#space#", " ");
		break;

		case BOXMODE_EXCHANGE:
			StartAboveForm(false);
			iSetCharIDToCharactersArroy(itemsRef); // Не нужно это, но и не помешает
			StripLowRankEquip(itemsRef);
			sTab3 = GetConvertStrWithReplace("Variable_itemsbox_8", "Interface.txt", "#space#", " ");
		break;
	}
	return sTab3;
}

// boal: снять с офа/трупа оружие и броню, если его ранг ниже нашего (кроме HoldEquip)
void StripLowRankEquip(ref itemsRef)
{
	if(!CheckAttribute(itemsRef, "HoldEquip") && CheckAttribute(itemsRef, "rank"))
	{
		if(sti(itemsRef.rank) < sti(refCharacter.rank)) // только, если ранг больше
		{
			RemoveCharacterEquip(itemsRef, GUN_ITEM_TYPE);
			if(!IsEquipCharacterByItem(itemsRef, "unarmed"))
			{
				RemoveCharacterEquip(itemsRef, BLADE_ITEM_TYPE);
			}

			RemoveCharacterEquip(itemsRef, CIRASS_ITEM_TYPE); //броня 081004 boal
			RemoveCharacterEquip(itemsRef, BACKPACK_ITEM_TYPE); //броня 081004 boal
		}
	}
}

void BoxMode_Buttons() // бывш. InterfaceInitButtons: набор кнопок по режиму
{
	SetNodeUsing("GETALL_BUTTON", false);
	SetNodeUsing("DELBODY_BUTTON", false);
	SetNodeUsing("AUTOPICK", false);
	SetNodeUsing("PUT_MATERIALS", false);
	SetNodeUsing("PUT_JEWELRY", false);
	SetNodeUsing("PUT_IDOLS", false);

	switch(iBoxMode)
	{
		case BOXMODE_EXCHANGE:
			SetNodeUsing("GETALL_BUTTON", true);
		break;

		case BOXMODE_DEADMAN:
			SetNodeUsing("GETALL_BUTTON", true);
			SetNodeUsing("DELBODY_BUTTON", true);
		break;

		case BOXMODE_CHEST:
			// LEO: «Взять всё» в сундуке доступна всегда - раньше её открывал только активный сборщик
			//   лута, хотя сама кнопка и её обработчик были на месте. Остальные кнопки сборщика
			//   (материалы, ценности, идолы, автоподбор) под его гейтом и остаются.
			//   Исключение - сундук каюты: он хранилище, а не добыча, выгребать его целиком незачем.
			SetNodeUsing("GETALL_BUTTON", loadedLocation.id != Get_My_Cabin());
			if (csmCA(pchar, "CSM.LootCollector.Run"))
			{
				SetNodeUsing("PUT_MATERIALS", true);
				SetNodeUsing("PUT_JEWELRY", true);
				SetNodeUsing("PUT_IDOLS", true);
				SetNodeUsing("AUTOPICK", true);
				SendMessage(&GameInterface,"lslllll", MSG_INTERFACE_MSG_TO_NODE, "GETALL_BUTTON", 1, 740, 14, 830, 39);
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"PUT_MATERIALS",0, "#"+XI_ConvertString("GetMaterials"));
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"PUT_JEWELRY",0, "#"+XI_ConvertString("GetJewelry"));
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"PUT_IDOLS",0, "#"+XI_ConvertString("GetIdols"));
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"AUTOPICK",0, "#"+XI_ConvertString("Autopick2"));
			}
			if (loadedLocation.id == Get_My_Cabin())
			{
				SetNodeUsing("PUT_MATERIALS", true);
				SetNodeUsing("PUT_JEWELRY", true);
				SetNodeUsing("PUT_IDOLS", true);
				SetNodeUsing("AUTOPICK", true);
			}
		break;

		case BOXMODE_BARREL:

		break;
	}
}

void SetCharacterMoneyToGold(aref rChar) // В интерфейсе оперируем с деньгами как с предметом "gold"
{
	bool bOk = iBoxMode == BOXMODE_CHEST || iBoxMode == BOXMODE_BARREL;
	if(bOk && !CheckAttribute(rChar, "ID"))
	{
		if(!CheckAttribute(rChar, "Items.Gold")) rChar.Items.Gold = 0;
		rChar.items.gold = sti(rChar.items.gold)+sti(GetAttrValue(rChar, "money")); // По квестам ложится сразу золото, а не деньги
	}
	else
	{
		rChar.items.gold = sti(GetAttrValue(rChar, "money"));
	}
}

int iSetCharIDToCharactersArroy(ref _rChar)
{
	string sCurArroyID;
	string sCharID = _rChar.id;
	for(int i=0; i<INTERFACE_ITEMSBOX_CHAR_ARROYSIZE; i++)
	{
		sCurArroyID = sCharactersArroy[i];
		if(sCurArroyID == sCharID) return i;

		if(sCurArroyID == "")
		{
			sCharactersArroy[i] = sCharID;
			return i;
		}
	}

	// Не знаю, возможна ли нехватка свободных, ведь define INTERFACE_ITEMSBOX_CHAR_ARROYSIZE 10
	sCharactersArroy[0] = sCharID; // Свободных нету, но поставить-то нужно
	return 0;
}

void MakeMoneyFromGoldWithowtChest()
{
	string sCurArroyID;
	ref arCurChar;
	for(int i=0; i<INTERFACE_ITEMSBOX_CHAR_ARROYSIZE, i++)
	{
		sCurArroyID = sCharactersArroy[i];
		if(sCurArroyID != "")
		{
			arCurChar = CharacterFromID(sCurArroyID);
			arCurChar.money = GetCharacterItem(arCurChar, "Gold");
			DeleteAttribute(arCurChar, "items.gold");
		}
	}
}

// Gregg: были две посимвольно одинаковые функции (WithChest/WithDeadMan) — слиты в одну
void MakeMoneyFromGoldWithBox(aref arBox)
{
	arBox.money = GetCharacterItem(arBox, "Gold");
	DeleteAttribute(arBox, "items.gold");
	MakeMoneyFromGoldWithowtChest();
}

// Gregg: ключ заголовка формы по режиму (бывший sGetInterfaceTypeStr с четырьмя параметрами)
string BoxMode_TitleKey()
{
	string sKey = "titleExchangeItems";
	switch(iBoxMode)
	{
		case BOXMODE_CHEST:
			sKey = "titleItemsBox";
		break;

		case BOXMODE_BARREL:
			sKey = "titleBarrel";
		break;

		case BOXMODE_DEADMAN:
			sKey = "titleDeadItems";
		break;
	}
	return sKey;
}

void BoxMode_MoneyOnExit() // Выставим деньги: перевод предмета "gold" обратно в money (бывш. SetCharactersMoneyOnExit)
{
	switch(iBoxMode)
	{
		case BOXMODE_EXCHANGE:
			MakeMoneyFromGoldWithowtChest();
		break;

		case BOXMODE_CHEST:
			MakeMoneyFromGoldWithBox(arChest);
		break;

		case BOXMODE_BARREL:
			MakeMoneyFromGoldWithBox(arChest);
		break;

		case BOXMODE_DEADMAN:
			MakeMoneyFromGoldWithBox(arDeadChar);
		break;
	}
}

// Gregg: ШАГ 2 рефакторинга — обе кнопки-стрелки на строку iLine таблицы (1 = базовая позиция).
// Эта пара SendMessage дословно повторялась в 8 местах.
void MoveTableArrows(int iLine)
{
	SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE, "TABLE_ADD_ALL_BUTTON", 0, iTableAddAllBtnX, iTableAddAllBtnY + 30 * (iLine - 1), iTableAddAllBtnX + 30, iTableAddAllBtnY + 30 * (iLine - 1) + 30, 0);
	SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE, "TABLE_REMOVE_ALL_BUTTON", 0, iTableRemoveAllBtnX, iTableRemoveAllBtnY + 30 * (iLine - 1), iTableRemoveAllBtnX + 30, iTableRemoveAllBtnY + 30 * (iLine - 1) + 30, 0);
}

// Gregg: единый «хвост обновления» после переносов (был скопирован в 6 местах с плавающим порядком).
// bResetSelection: сброс на первую строку (там всегда Gold) + инфо-панель + стрелки на базу.
// select сбрасывается ДО AddToTable, чтобы не заходить в его легаси-сдвиг выделения.
void RefreshBoxTable(bool bResetSelection)
{
	SetCharWeight();
	SetVariable();
	if(bResetSelection)
	{
		GameInterface.TABLE_LIST.top = 0;
		GameInterface.TABLE_LIST.select = 1;
	}
	AddToTable(refToChar);
	if(bResetSelection)
	{
		sCurItem = GameInterface.TABLE_LIST.tr1.id;
		ShowGoodsInfo(sCurItem);
		MoveTableArrows(1);
	}
}

void ShowItemsWeight()
{
	CreateString(true, "ToCharacterWeightStr", "", FONT_NORMAL, COLOR_NORMAL, 698, 110, SCRIPT_ALIGN_CENTER, 0.9);

	if(iBoxMode != BOXMODE_CHEST) GameInterface.strings.ToCharacterWeightStr =  XI_ConvertString("Weight") + ": " + FloatToString(GetItemsWeight(refToChar), 1) + " / " + GetMaxItemsWeight(refToChar);
	else GameInterface.strings.ToCharacterWeightStr =  XI_ConvertString("Weight") + ": " + FloatToString(GetItemsWeight(refToChar), 1) + " / 999999";

	CreateString(true, "MyCharacterWeightStr", XI_ConvertString("Weight") + ": " + FloatToString(GetItemsWeight(refCharacter), 1) + " / " + GetMaxItemsWeight(refCharacter), FONT_NORMAL, COLOR_NORMAL, 108, 110, SCRIPT_ALIGN_CENTER, 0.9);
}

void ProcessBreakExit()
{
	IDoExit(RC_INTERFACE_FOOD_INFO_EXIT);
}

void ProcessCancelExit()
{
	if(bShowChangeWin)
	{
		EndTooltip();
	}
	else
	{
		IDoExit(RC_INTERFACE_FOOD_INFO_EXIT);
	}
}

void IDoExit(int exitCode)
{
	if (iBoxMode == BOXMODE_EXCHANGE) pchar.iCurrentTabMode = iCurrentTabMode;

	ref arCurChar;
	string sCurArroyID;
	BoxMode_MoneyOnExit(); // Переводим монеты "gold" в деньги
	RefreshEquippedMaps(GetMainCharacter()); // рефрешим карты, если купили
	ref sld;

	bool bBox = iBoxMode == BOXMODE_CHEST || iBoxMode == BOXMODE_BARREL; // бывший глобальный флаг bBoxUsed
	if(!bBox)
	{
		for(int i=0; i<INTERFACE_ITEMSBOX_CHAR_ARROYSIZE, i++)
		{
			sCurArroyID = sCharactersArroy[i];
			if(sCurArroyID != "")
			{
				arCurChar = CharacterFromID(sCurArroyID);
				if(IsMainCharacter(arCurChar)) continue;

				if(CheckAttribute(arCurChar, "skill.Fencing")) //boal fix иначе не берут саблю при перегрузе
				{
					EquipBestByGroup(arCurChar, BLADE_ITEM_TYPE);
				}

				if(CheckAttribute(arCurChar, "skill.Pistol") && GetCharacterSkill(arCurChar,"Pistol") > 0.1 )
				{
					EquipBestByGroup(arCurChar, GUN_ITEM_TYPE);
				}

				if(CheckAttribute(arCurChar, "RealCompanionIdx")) // 1.2.4 обмен с компаньоном
				{
					sld = GetCharacter(sti(arCurChar.RealCompanionIdx));
					ChangeAttributesFromCharacter(sld, arCurChar, false); //тут предметы и деньги
				}
			}
		}
	}
	else
	{
		bMainCharacterInBox = false; // снимаем флаг 'в боксе'
		// Gregg: флаг читает LandInterface.c именно у pchar, поэтому ставим прямо на него.
		// Было Characters[sti(refCharacter.index)] — а refCharacter в скролле уезжает: на офицера
		// (флаг ложился на него, pchar его не видел, радар молча не пересчитывался) либо на сундук
		// каюты (makeref(refCharacter,boxN) ниже) — у сундука нет .index, отсюда RUNTIME ERROR.
		pchar.radarPOIDirty = 1;
	}

	EndAboveForm(true);
	LAi_SetPlayerType(PChar);

	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

	DelEventHandler("OnTableClick", "OnTableClick");
	DelEventHandler("MouseRClickUP","EndTooltip");
	DelEventHandler("ShowHelpHint", "ShowHelpHint");
	DelEventHandler("ShowItemInfo", "ShowItemInfo");
	DelEventHandler("TableSelectChange", "CS_TableSelectChange");
	DelEventHandler("frame","ProcessFrame");
	DelEventHandler("TransactionOK", "TransactionOK");
	DelEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT");
	DelEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON");
	DelEventHandler("ADD_BUTTON","ADD_BUTTON");
	DelEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON");
	DelEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON");

	DelEventHandler("eTabControlPress", "procTabChange"); // Событие смены закладки

	interfaceResultCommand = exitCode;

	if (iBoxMode == BOXMODE_DEADMAN)
	{
		// LEO: индекс ловим ДО Dead_DelLoginedCharacter (оно инвалидирует refToChar)
		int dlIdx = -1;
		if (CheckAttribute(refToChar, "index")) dlIdx = sti(refToChar.index);

		if (DelLogin)
		{
			Dead_DelLoginedCharacter(refToChar);
		}
		// LEO: радар — облутан = ТОТ ЖЕ критерий, что "не обыскивать" (DelLogin), чтобы совпадало с кнопкой
		if (DelLogin) { if (dlIdx >= 0) Characters[dlIdx].radarlooted = 1; }
	}

	EndCancelInterface(true);

	LAi_SetPlayerType(PChar); // Возвращаем тип игрока

	// boal проверка на перегруз 21.01.2004 -->
	CheckAndSetOverloadMode(GetMainCharacter());
	// boal 21.01.2004 <--

	// CSM -->
	if (csmCA(pchar, "CSM.LootCollector.Run"))
	{
		csmHideLootCollectorBox(false);

		if (!csmCA(pchar, "CSM.LootCollector.Tutor"))
		{
			pchar.CSM.LootCollector.Tutor = true;
			ClearAllLogStrings();
			Log_Info(GetConvertStrWithReplace("Variable_itemsbox_17.1", "Interface.txt", "#space#", " "));
			Log_Info(GetConvertStrWithReplace("Variable_itemsbox_18", "Interface.txt", "#space#", " "));
			PlaySound("interface\notebook.wav");
		}
	}

	csmDA(pchar, "CSM.LootCollector.Run");
	// CSM <--
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
		case "TABLE_ADD_ALL_BUTTON":
			if(comName == "activate" || comName == "click")
			{
				ADD_ALL_BUTTON();
				TransactionOK();
			}
		break;

		case "TABLE_REMOVE_ALL_BUTTON":
			if(comName == "activate" || comName == "click")
			{
				REMOVE_ALL_BUTTON();
				TransactionOK();
			}
		break;

		case "QTY_OK_BUTTON":
			if(comName=="leftstep")
			{
				ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
				REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
				ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
				REMOVE_ALL_BUTTON();
			}
		break;

		case "QTY_CANCEL_BUTTON":
			if(comName=="leftstep")
			{
				ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
				REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
				ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
				REMOVE_ALL_BUTTON();
			}
		break;

		case "TABLE_LIST":
			if(comName=="leftstep")
			{
				ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
				REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
				ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
				REMOVE_ALL_BUTTON();
			}
		break;

		case "GETALL_BUTTON":
			if(comName == "activate" || comName == "click")
			{
				onGetAllBtnClick();
			}
		break;
		
		case "PUT_MATERIALS":
			if(comName == "activate" || comName == "click")
			{
				if (csmCA(pchar, "CSM.LootCollector.Run")) GetStuff("material");
				else PutStuff("material");
			}
		break;
		
		case "PUT_IDOLS":
			if(comName == "activate" || comName == "click")
			{
				if (csmCA(pchar, "CSM.LootCollector.Run")) GetStuff("idol");
				else PutStuff("idol");
			}
		break;
		
		case "PUT_JEWELRY":
			if(comName == "activate" || comName == "click")
			{
				if (csmCA(pchar, "CSM.LootCollector.Run")) GetStuff("jewelry");
				else PutStuff("jewelry");
			}
		break;
		
		case "AUTOPICK":
			if(comName == "activate" || comName == "click")
			{
				int iOfficer = -1;
				ref _chr;
				// Gregg: инвариант цикла — спрашивали заново на каждого офицера
				bool bFromChest = false;
				if (csmCA(pchar, "CSM.LootCollector.Run")) bFromChest = true;
				// Gregg: держим файлы строк открытыми на весь клик. LanguageOpenFile рефкаунтится, а снос блока
				// идёт только при nref==0, поэтому вложенные GetConvertStrWithReplace/LanguageOpenFile внутри
				// Autoequip* теперь лишь дёргают nref вместо перечитки Interface.txt и ItemsDescribe.txt с диска.
				int idIfaceFile = LanguageOpenFile("Interface.txt");
				int idItemsFile = LanguageOpenFile("ItemsDescribe.txt");
				// Gregg: было MAX_NUM_FIGHTERS+6 — GetOfficersIndex на всё сверх MAX_NUM_FIGHTERS отдаёт -1
				for(int z = 0; z <= MAX_NUM_FIGHTERS; z++)
				{
					iOfficer = GetOfficersIndex(pchar, z);
					if(iOfficer != -1)
					{
						_chr = &characters[iOfficer];
						if (bFromChest) AutoequipFromChest(_chr,refToChar);
						else AutoequipFromCabin(_chr);
					}
				}
				LanguageCloseFile(idItemsFile);
				LanguageCloseFile(idIfaceFile);
				// Gregg: AUTOPICK единственный из переносов не обновлял вес — висело старое значение до смены
				// персонажа в скролле; RefreshBoxTable чинит (пересчёт веса + перерисовка + таблица).
				RefreshBoxTable(false);
			}
		break;

		//navy --> Убрать тело ;)
		case "DELBODY_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				Dead_DelLoginedCharacter(refToChar);
				ProcessCancelExit();
			}
		break;
		//navy <--
	}
}

void procTabChange()
{
	int comIndex = GetEventData();
	String nodName = GetEventData();
	SetCurrentNode("TABLE_LIST");

	// Сами закладки оказались почему-то ненажимаемыми - проверка на кнопки
	switch(nodName)
	{
		case "TABSTR_1":
			SetControlsTabMode(1)
		break;

		case "TABSTR_2":
			SetControlsTabMode(2)
		break;

		case "TABSTR_3":
			SetControlsTabMode(3)
		break;

		case "TABSTR_4":
			SetControlsTabMode(4)
		break;
	}
}

void SetControlsTabMode(int _mode)
{
	String Pic1 = "TabSelected";
	String Pic2 = Pic1;
	String Pic3 = Pic1;
	String Pic4 = Pic1;

	switch(_mode)
	{
		case 1:
			Pic1 = "TabDeSelected";
		break;

		case 2:
			Pic2 = "TabDeSelected";
		break;

		case 3:
			Pic3 = "TabDeSelected";
		break;

		case 4:
			Pic4 = "TabDeSelected";
		break;
	}

	iCurrentTabMode = _mode;

	// Выставим таблицу в начало
	GameInterface.TABLE_LIST.select = 1;
	GameInterface.TABLE_LIST.top = 0;

	// Тут идёт рефрешь предметов
	AddToTable(refToChar);

	// Сбросим позицию стрелок
	MoveTableArrows(1);

	SetNewGroupPicture("TABBTN_1", "TABS", Pic1);
	SetNewGroupPicture("TABBTN_2", "TABS", Pic2);
	SetNewGroupPicture("TABBTN_3", "TABS", Pic3);
	SetNewGroupPicture("TABBTN_4", "TABS", Pic4);
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
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

	nCurScrollNum = -1;
	GameInterface.CHARACTERS_SCROLL.current = 0;
	makearef(pRef,pchar.Fellows.Passengers);

	int nListSize = GetPassengersQuantity(pchar);
	int nListSizeFree = GetNotQuestPassengersQuantity(pchar);

	GameInterface.CHARACTERS_SCROLL.NotUsed = 6;
	GameInterface.CHARACTERS_SCROLL.ListSize = nListSizeFree + 2;

	GameInterface.CHARACTERS_SCROLL.ImagesGroup.t0 = "EMPTYFACE";
	GameInterface.CHARACTERS_SCROLL.ImagesGroup.t1 = "BOX_IMAGE";

	FillFaceList("CHARACTERS_SCROLL.ImagesGroup", pchar, 2); // passengers

	GameInterface.CHARACTERS_SCROLL.BadTex1 = 0;
	GameInterface.CHARACTERS_SCROLL.BadPic1 = "emptyface";

	string sCharID = "";
	int iCharIDX;

	int m = 0;
	bool cabin = loadedLocation.id == Get_My_Cabin();
	// Gregg: гейт по режиму — arChest инициализирован только у сундука/бочки (BoxMode_Init). Раньше при
	// ОБМЕНЕ с офицером в локации банка arChest.id разыменовывал пустой aref: две строки RUNTIME ERROR
	// (uninitialized aref + zero AClass root) в error.log на каждое открытие, а bank получался false
	// лишь потому, что подставная строка ошибки не содержит "_usurer". Ячейка ростовщика всегда
	// открывается как сундук (LaunchItemsBox, faceID=""), так что поведение фичи не меняется.
	bool bank = false;
	if (iBoxMode == BOXMODE_CHEST) bank = HasSubStr(loadedLocation.id,"_bank") && HasSubStr(arChest.id,"_usurer") && Pchar.SystemInfo.CabinType != "";
	if (cabin || bank)//фикс - ошибки в логе, если ГГ без корабля и каюты
	{
		ref cabinloc = &locations[FindLocation(Pchar.SystemInfo.CabinType)];
		chestsnum = 0;
		string ass = "";
		if (bank) pchar.boxx = "ass";
		if (CheckAttribute(cabinloc,"box1") && pchar.boxx != "box1") {chestsnum++; makearef(box1,cabinloc.box1); ass = ass+"1"}
		if (CheckAttribute(cabinloc,"box2") && pchar.boxx != "box2") {chestsnum++; makearef(box2,cabinloc.box2); ass = ass+"2"}
		if (CheckAttribute(cabinloc,"box3") && pchar.boxx != "box3") {chestsnum++; makearef(box3,cabinloc.box3); ass = ass+"3"}
		if (CheckAttribute(cabinloc,"box4") && pchar.boxx != "box4") {chestsnum++; makearef(box4,cabinloc.box4); ass = ass+"4"}
		for (i = 1;i <= 4;i++)
		{
			if (HasSubStr(ass,its(i)))
			{
				attributeName = "pic" + (m + 1);
				GameInterface.CHARACTERS_SCROLL.(attributeName).character = "box"+i;
				GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = "BoxImage";
				GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","BOX_IMAGE");
				m++;
			}
		}
		GameInterface.CHARACTERS_SCROLL.current = m;
	}
	if(iBoxMode != BOXMODE_EXCHANGE)
	{
		attributeName = "pic" + (m + 1);
		GameInterface.CHARACTERS_SCROLL.(attributeName).character = nMainCharacterIndex;
		GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(pchar);
		GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_" + pchar.FaceID);
		m++;
	}
	else // При обмене предметами с офом первая картинка - собсно офф
	{
		sCharID = refToChar.ID;
		iCharIDX = GetCharacterIndex(sCharID);
		attributeName = "pic" + (m + 1);

		SetCharacterMoneyToGold(refToChar);
		iSetCharIDToCharactersArroy(refToChar); // Пометим его для удаления золота и дачи денег

		GameInterface.CHARACTERS_SCROLL.(attributeName).character = iCharIDX;
		GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(refToChar);
		GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_" + refToChar.FaceID);
		m++;
	}

	for(i = 0; i < nListSize; i++)
	{
		_curCharIdx = GetPassenger(pchar, i);

		bOk = CheckAttribute(&characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true;
		if(_curCharIdx != -1  && !CheckAttribute(&characters[_curCharIdx], "isquest") && !bOk)
		{
			if(IsOfficer(&characters[_curCharIdx]) && PChar.location == Characters[_curCharIdx].location)  // boal // fix - ГГ и офы должны быть в одной локации
			{
				if(sCharID == characters[_curCharIdx].ID) continue;

				ReplaceTreasureMapPartCopies(GetCharacter(_curCharIdx), PChar); // на всякий случай
				SetCharacterMoneyToGold(GetCharacter(_curCharIdx));
				iSetCharIDToCharactersArroy(GetCharacter(_curCharIdx)); // Пометим его для удаления золота и дачи денег
				attributeName = "pic" + (m + 1);
				GameInterface.CHARACTERS_SCROLL.(attributeName).character = _curCharIdx;
				GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
				GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
				m++;
				continue;
			}
			bool EnemyCabinOrBank = HasSubStr(pchar.location,"_bank") || CheckAttribute(loadedLocation, "CabinType");
			if(IsOfficer(&characters[_curCharIdx]) && PChar.location != Characters[_curCharIdx].location && EnemyCabinOrBank)  // fix - ячейка банковская и аборды
			{
				if(sCharID == characters[_curCharIdx].ID) continue;

				SetCharacterMoneyToGold(GetCharacter(_curCharIdx));
				iSetCharIDToCharactersArroy(GetCharacter(_curCharIdx)); // Пометим его для удаления золота и дачи денег
				attributeName = "pic" + (m + 1);
				GameInterface.CHARACTERS_SCROLL.(attributeName).character = _curCharIdx;
				GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
				GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
				m++;
			}
		}
	}
}

void AddToTable(ref rChar)
{
	// Gregg: пин файлов строк на всю переборку. GetConvertStr/GetConvertStrWithReplace — это
	// LanguageOpenFile+Convert+LanguageCloseFile, а Close при nref==0 сносит блок целиком, и следующий
	// Open читает файл с диска и парсит ДВАЖДЫ (172 КБ ItemsDescribe + 132 КБ Interface). Ниже они зовутся
	// НА КАЖДУЮ строку таблицы -> замер VM-профайлером: 47% из 147 мс на вызов AddToTable уходило сюда.
	// Внешний Open держит nref>0, поэтому вложенные вызовы становятся инкрементом счётчика.
	// Ранних return в функции нет (единственный выход в конце), так что пин не утечёт.
	int idItemsFile = LanguageOpenFile("ItemsDescribe.txt");
	int idIfaceFile = LanguageOpenFile("Interface.txt");
	// Gregg: пометки комплектов инвариантны — незачем дёргать их на каждый предмет
	string sInSetLeft  = GetConvertStrWithReplace("Variable_itemsbox_26", "Interface.txt", "#space#", " ");
	string sInSetRight = GetConvertStrWithReplace("Variable_itemsbox_27", "Interface.txt", "#space#", " ");

	string sList, sItem;
	int n, i, iLeftQty, iRightQty, iSelLine, iTemp;
	ref rItem;
	bool ok;
	bool ok1 = false;
	n = 1;
	int iRightQtyTotal=0;

	// Warship fix 31.05.09 - слетала текущая полоска + кнопочки забрать/отдать всё
	// Садо-мазо получилось ещё то
	ok = iCurrentTabMode == 2 || iCurrentTabMode == 3; // Долбаный шторм не понимает И и ИЛИ в одном выражении, даж со скобками
	// LEO: при ПРЯМОЙ перекладке стрелками предмет остаётся на той же строке - этот select-сдвиг (рассчитан на
	//   оконный поток, где строки пропадают) не нужен и уводит выделение/стрелки вверх на последней строке.
	if(ok && iLinesCount > 2 && !bLeoDirectXfer)
	{
		if(sti(GameInterface.TABLE_LIST.select) + 1 == iLinesCount)
		{
			iLinesCount -= 2;
			GameInterface.TABLE_LIST.select = iLinesCount;

			iSelLine = iLinesCount;

			if(CheckAttribute(&GameInterface, "TABLE_LIST.top"))
			{
				if(sti(GameInterface.TABLE_LIST.top) > 0)
				{
					GameInterface.TABLE_LIST.top = sti(GameInterface.TABLE_LIST.top) - 1;
				}

				iSelLine = iLinesCount - sti(GameInterface.TABLE_LIST.top);
			}

			MoveTableArrows(iSelLine);

			sList = "tr" + sti(GameInterface.TABLE_LIST.select);
			sCurItem = GameInterface.TABLE_LIST.(sList).id;
			ShowGoodsInfo(sCurItem);
		}
		else
		{
			GameInterface.TABLE_LIST.select = sti(GameInterface.TABLE_LIST.select) + 1;

			ok1 = true; // См. ниже проверку if(ok1) - бред-бредом конечно, но по-другому не получается
		}
	}

	iLinesCount = 1;

	Table_Clear("TABLE_LIST", false, true, false);

	iLeftQty  = GetCharacterFreeItem(refCharacter, "Gold");
	iRightQty = GetCharacterFreeItem(rChar, "Gold");
	if (iRightQty>0) iRightQtyTotal += 1;

	// Фикс - должно быть всегда, иначе у нищих офов слетает стрелочки
		sList = "tr" + n;
		rItem = &Items[FindItem("Gold")];
		GameInterface.TABLE_LIST.(sList).id = "Gold";
		GameInterface.TABLE_LIST.(sList).index = n;
		GameInterface.TABLE_LIST.(sList).td1.str = iLeftQty;
		GameInterface.TABLE_LIST.(sList).td1.scale = 0.8;
		GameInterface.TABLE_LIST.(sList).td2.str = "-";
		GameInterface.TABLE_LIST.(sList).td3.icon.group = rItem.picTexture;
		GameInterface.TABLE_LIST.(sList).td3.icon.image = "itm" + rItem.picIndex;
		GameInterface.TABLE_LIST.(sList).td3.icon.offset = "30, 0";
		GameInterface.TABLE_LIST.(sList).td3.icon.width = 30;
		GameInterface.TABLE_LIST.(sList).td3.icon.height = 30;
		GameInterface.TABLE_LIST.(sList).td3.textoffset = "32, 0";
		GameInterface.TABLE_LIST.(sList).td3.str = GetConvertStr(rItem.name, "ItemsDescribe.txt");
		GameInterface.TABLE_LIST.(sList).td3.scale = 1.0;
		GameInterface.TABLE_LIST.(sList).td4.str = "-";
		GameInterface.TABLE_LIST.(sList).td5.str = iRightQty;
		GameInterface.TABLE_LIST.(sList).td5.scale = 1.0;
		n++;

		iLinesCount++;

	object allItems;
	aref arInventory, arItem;
	int inventorySize;

	if (iCurrentTabMode != 3) // Сортировка не GetConvertStrWithReplace("Variable_itemsbox_21", "Interface.txt", "#space#", " ")
	{
		makearef(arInventory, refCharacter.Items);
		inventorySize = GetAttributesNum(arInventory);
		for (i = 0; i < inventorySize; i++)
		{
			arItem = GetAttributeN(arInventory, i);
			sItem = GetAttributeName(arItem);
			sList = DigitsToString(FindItem(sItem), 4);

			if (IsGenerableItem(sItem))
			{
				// Сначала заполняем рандомящимся оружием
				sList = "a" + sList + sItem;
			}
			else
			{
				sList = "b" + sList;
			}

			allItems.(sList) = sItem;
		}
	}

	if (iCurrentTabMode != 2) // Сортировка не GetConvertStrWithReplace("Variable_itemsbox_22", "Interface.txt", "#space#", " ")
	{
		makearef(arInventory, rChar.Items);
		inventorySize = GetAttributesNum(arInventory);
		for (i = 0; i < inventorySize; i++)
		{
			arItem = GetAttributeN(arInventory, i);
			sItem = GetAttributeName(arItem);
			sList = DigitsToString(FindItem(sItem), 4);

			if (IsGenerableItem(sItem))
			{
				sList = "a" + sList + sItem;
			}
			else
			{
				sList = "b" + sList;
			}

			allItems.(sList) = sItem;
		}
	}

	makearef(arInventory, allItems);
	sort(arInventory);

	inventorySize = GetAttributesNum(arInventory);
	for (i = 0; i < inventorySize; i++)
	{
		arItem = GetAttributeN(arInventory, i);
		sItem = GetAttributeValue(arItem);
		rItem = ItemsFromID(sItem);

		if (sItem == "Gold") continue; // Золото выше

		iLeftQty  = GetCharacterFreeItem(refCharacter, sItem);
		iRightQty = GetCharacterFreeItem(rChar, sItem);
		if (iRightQty > 0) iRightQtyTotal += 1;

		if ((iLeftQty == 0) && (iRightQty == 0)) continue;
		if (iCurrentTabMode == 2 && iLeftQty < 1) continue; // Сортировка GetConvertStrWithReplace("Variable_itemsbox_23", "Interface.txt", "#space#", " ")
		if (iCurrentTabMode == 3 && iRightQty < 1) continue; // Сортировка GetConvertStrWithReplace("Variable_itemsbox_24", "Interface.txt", "#space#", " ")

		if (iCurrentTabMode == 4) // Сортировка GetConvertStrWithReplace("Variable_itemsbox_25", "Interface.txt", "#space#", " ")
		{
			if (CheckAttribute(rItem, "groupID"))
			{
				// Проверка на оружие
				if (rItem.groupID != BLADE_ITEM_TYPE && rItem.groupID != GUN_ITEM_TYPE && rItem.groupID != TALISMAN_ITEM_TYPE && rItem.groupID != AMMO_ITEM_TYPE)
				{
					continue;
				}
			}
			else
			{
				// Проверка на зелья
				if(!HasSubStr(sItem, "potion"))
				{
					continue;
				}
			}
		}

		sList = "tr" + n;
		GameInterface.TABLE_LIST.(sList).id = sItem;
		GameInterface.TABLE_LIST.(sList).td1.str = iLeftQty;
		GameInterface.TABLE_LIST.(sList).td2.str = FloatToString(GetItemWeight(sItem) * iLeftQty, 1);
		GameInterface.TABLE_LIST.(sList).td3.icon.group = rItem.picTexture;
		GameInterface.TABLE_LIST.(sList).td3.icon.image = "itm" + rItem.picIndex;
		GameInterface.TABLE_LIST.(sList).td3.icon.offset = "30, 0";
		GameInterface.TABLE_LIST.(sList).td3.icon.width = 30;
		GameInterface.TABLE_LIST.(sList).td3.icon.height = 30;
		GameInterface.TABLE_LIST.(sList).td3.textoffset = "32, 0";
		GameInterface.TABLE_LIST.(sList).td3.str = GetConvertStr(rItem.name, "ItemsDescribe.txt");
		GameInterface.TABLE_LIST.(sList).td3.scale = 1.0;
		GameInterface.TABLE_LIST.(sList).td4.str = FloatToString(GetItemWeight(sItem) * iRightQty, 1);
		GameInterface.TABLE_LIST.(sList).td5.str = iRightQty;

		iTemp = CheckItemInSets(refCharacter, sItem);
		if (iTemp > 0)
		{
			GameInterface.TABLE_LIST.(sList).td3.str = GameInterface.TABLE_LIST.(sList).td3.str + sInSetLeft + iTemp + ")";
			GameInterface.TABLE_LIST.(sList).td3.color = argb(255,196,196,255);//подцвечиваем предметы из комплектов
			GameInterface.TABLE_LIST.(sList).td1.color = argb(255,196,196,255);//в комплекте у левого персонажа
		}
		iTemp = CheckItemInSets(rChar, sItem);
		if (iTemp > 0)
		{
			GameInterface.TABLE_LIST.(sList).td3.str = GameInterface.TABLE_LIST.(sList).td3.str + sInSetRight + iTemp + ")";
			GameInterface.TABLE_LIST.(sList).td3.color = argb(255,196,196,255);
			GameInterface.TABLE_LIST.(sList).td5.color = argb(255,196,196,255);//в комплекте у правого персонажа
		}
		n++;

		iLinesCount++;
	}

	if(ok1)
	{
		GameInterface.TABLE_LIST.select = sti(GameInterface.TABLE_LIST.select) - 1;
		sList = "tr" + sti(GameInterface.TABLE_LIST.select);

		// Warship 10.07.09 fix - Могло не быть, при сбрасывании в начало при переключении персонажей
		if(CheckAttribute(GameInterface, "TABLE_LIST." + sList + ".id"))
		{
			sCurItem = GameInterface.TABLE_LIST.(sList).id;
			ShowGoodsInfo(sCurItem);
		}
	}

	// Gregg: универсальный кламп выделения ПОСЛЕ пересборки. Если выделенная строка исчезла целиком
	// (забрали последнюю строку прямым переносом стрелками, AUTOPICK выгреб несколько строк и т.п.),
	// select указывал за конец таблицы: подсвечивалась пустая строка, sCurItem застревал на пустом
	// предмете (SyncCurItemToSelection выходит по гварду), стрелки висели на пустоте. Легаси-сдвиг
	// выше чинит только оконный поток и ровно одну исчезнувшую строку — а этот кламп добивает остальное.
	iTemp = iLinesCount - 1; // строк сейчас в таблице (tr1..trN)
	if(iTemp >= 1 && sti(GameInterface.TABLE_LIST.select) > iTemp)
	{
		GameInterface.TABLE_LIST.select = iTemp;

		iSelLine = iTemp;
		if(CheckAttribute(&GameInterface, "TABLE_LIST.top"))
		{
			// окно могло остаться проскролленным ниже последней строки
			if(sti(GameInterface.TABLE_LIST.top) >= iTemp) GameInterface.TABLE_LIST.top = iTemp - 1;
			iSelLine = iTemp - sti(GameInterface.TABLE_LIST.top);
		}
		MoveTableArrows(iSelLine);

		sList = "tr" + iTemp;
		sCurItem = GameInterface.TABLE_LIST.(sList).id;
		ShowGoodsInfo(sCurItem);
	}

	Table_UpdateWindow("TABLE_LIST");
	SetEventHandler("frame", "RefreshTableByFrameEvent", 0);
	if (iBoxMode == BOXMODE_DEADMAN)
	{
		if (iRightQtyTotal == 0)
		{
			DelLogin = true;
		}
	}

	// Gregg: снимаем пин (парно к Open в начале функции)
	LanguageCloseFile(idIfaceFile);
	LanguageCloseFile(idItemsFile);
}

void RefreshTableByFrameEvent()
{
	DelEventHandler("frame", "RefreshTableByFrameEvent");
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABLE_LIST", 0 );
}

void OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();

//--> mod tablesort
	if (!SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sControl, 1 )) SortTable(sControl, iColumn);
//<-- mod tablesort
	Table_UpdateWindow(sControl);
}

void ShowHelpHint()
{
	string sHeader;
	string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
	sPicture = "none";
	sGroup = "none";
	sGroupPicture = "none";

	if(!bShowChangeWin) // покажем помощь по работе с формой
	{
		sHeader = XI_ConvertString("titleItemsTrade");
		sText1 = GetConvertStr("Variable_guide_trade", "Interface.txt");

		sText3 = GetConvertStrWithReplace("Variable_itemsbox_30", "Interface.txt", "#space#", " ");
		sText2 = GetConvertStr("Variable_fast_give_away", "Interface.txt");

		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,192,192,192), sText3, argb(255,255,255,255), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
	}
}

void EndTooltip()
{
	CloseTooltip(); // всегда убирать, если был
	GameInterface.qty_edit.str = 0;
	SetCharWeight();
	SetVariable();
	SetCurrentNode("TABLE_LIST");
 	XI_WindowDisable("QTY_WINDOW", true);
	XI_WindowShow("QTY_WINDOW", false);
	bShowChangeWin = false;
}

void ShowItemInfo()
{
	if(bShowChangeWin) // жмем окей, когда курсор на таблице
	{
		TransactionOK();
	}
	else
	{
		GameInterface.qty_edit.str = "0";
		BuyOrSell = 0;
		ChangeQTY_EDIT();

		XI_WindowDisable("QTY_WINDOW", false);
		XI_WindowShow("QTY_WINDOW", true);
		bShowChangeWin = true;
	}
}

void CS_TableSelectChange()
{
	String sControl = GetEventData();
	int iSelected = GetEventData();
	int iCol = GetEventData();

	// Warship 26.04.09 fix - при прокрутке можго вылазить за экран, неправильно определялась текущая линия
	int iSelLine = iSelected;

	if(CheckAttribute(&GameInterface, "TABLE_LIST.top"))
	{
		iSelLine = iSelected - sti(GameInterface.TABLE_LIST.top);
	}

	MoveTableArrows(iSelLine);

	String sList = "tr" + iSelected;
	// Gregg: тут был SetCharWeight() + SetVariable() — 4 полных обхода инвентаря на КАЖДЫЙ шаг скролла
	// (замер VM-профайлером: 7.9 из 9.9 мс на шаг, ~530 вызовов ItemsFromID). Смена выделения строки
	// не трогает ни refCharacter, ни refToChar, ни их инвентари — вес измениться не может, а все точки,
	// где он реально меняется (переносы, смена персонажа в скролле), пересчитывают его сами.
	ShowGoodsInfo(GameInterface.TABLE_LIST.(sList).id);
}

void SetVariable()
{
	// Gregg: iCharCapacity — глобал, его читает кламп веса в ChangeQTY_EDIT
	iCharCapacity = GetMaxItemsWeight(refCharacter);
	ShowItemsWeight(); // Переделка
}

void SetCharacterName()
{
	string sAttr = "pic" + (nCurScrollNum + 1);
	int iCurCharImageIndex = sti(GameInterface.CHARACTERS_SCROLL.(sAttr).character);

	string sFullName = GetFullName(&characters[iCurCharImageIndex]);
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

	GameInterface.strings.CharName = sFullName;
}

void ProcessFrame()
{
	aref arCurCharFromImage;
	if(sti(GameInterface.CHARACTERS_SCROLL.current) != nCurScrollNum && GetCurrentNode() == "CHARACTERS_SCROLL")
	{
		XI_WindowDisable("QTY_WINDOW", true);
		XI_WindowShow("QTY_WINDOW", false);
		nCurScrollNum = sti(GameInterface.CHARACTERS_SCROLL.current);
		string sAttr = "pic" + (nCurScrollNum + 1);
		int iCurCharImageIndex = sti(GameInterface.CHARACTERS_SCROLL.(sAttr).character);

		if(iBoxMode == BOXMODE_EXCHANGE) // скролл переключает ПРАВОГО (с кем меняемся)
		{
			if (!HasSubStr(GameInterface.CHARACTERS_SCROLL.(sAttr).character,"box"))
			{
				makearef(refToChar, characters[iCurCharImageIndex]);
			}
			else makearef(refToChar, pchar);
		}
		else // сундук/бочка/труп: скролл переключает ЛЕВОГО (кто копается) — было три одинаковых case
		{
			makeref(refCharacter, characters[iCurCharImageIndex]);
		}

		SetVariable();
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
		
		AddToTable(refToChar);
		BoxMode_Pictures();
		// Gregg: refCharacter только что сменился — пересчитываем вес ЗДЕСЬ, где смена и произошла.
		// Раньше fCharWeight чинился побочно, из CS_TableSelectChange на первом же шаге скролла;
		// его читает кламп окна количества (проверка "влезет ли"), так что стухший вес = чужой лимит.
		SetCharWeight();
		SetVariable();
		GameInterface.TABLE_LIST.select = 1;
		GameInterface.TABLE_LIST.top = 0;
		ShowGoodsInfo(GameInterface.TABLE_LIST.tr1.id); // LEO: обновляем количество золота для выбранного источника, даже если строка та же
		SetCharacterName();

		// Стрелочки на первую позицию
		MoveTableArrows(1);
	}
}

void BoxMode_Pictures() // портреты по режиму (бывш. FillCharactersImages)
{
	if (CheckAttribute(refCharacter,"faceid")) SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\portraits\face_" + refCharacter.FaceId + ".tga");
	else SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\BoxImage.dds");

	switch(iBoxMode)
	{
		case BOXMODE_EXCHANGE:
			SetNewPicture("OTHER_PICTURE", "interfaces\portraits\face_" + refToChar.FaceId + ".tga");
		break;

		case BOXMODE_CHEST:
			SetNewGroupPicture("OTHER_PICTURE", "BOX_IMAGE", "BoxImage");
		break;

		case BOXMODE_BARREL:
			SetNewGroupPicture("OTHER_PICTURE", "BARREL_IMAGE", "BarrelImage");
		break;

		case BOXMODE_DEADMAN:
			SetNewPicture("OTHER_PICTURE", "interfaces\portraits\face_666.dds");
		break;
	}
}

void SetCharWeight()
{
    fCharWeight  = GetItemsWeight(refCharacter);
    fStoreWeight = GetItemsWeight(refToChar);
}

// Gregg: ШАГ 3 рефакторинга — единый массовый перенос предметов. Схлопнул три копии одного
// цикла (onGetAllBtnClick / GetStuff / PutStuff, ~200 строк на троих).
// bWeightLimit — клампить по свободному весу ПОЛУЧАТЕЛЯ (невесомые предметы идут без лимита;
//   в сундук/труп кладём без лимита — они бездонны);
// bKeepSetItem — оставить у источника по одному предмету, занятому в комплектах (логика PutStuff).
// Двухфазность (сначала AddItems получателю, потом общий RemoveItems у источника) сохранена:
// удаление предметов внутри перебора атрибутов инвентаря ломает итерацию.
// Возвращает true, если перенесён хоть один ВЕСОМЫЙ предмет — семантика старого bOk:
// только он тратит WaitDate-минуту в onGetAllBtnClick, невесомые (золото) — нет.
// Инкрементальные правки fCharWeight/fStoreWeight из старых циклов убраны: внутри цикла их
// никто не читает (кламп берёт GetItemsWeight напрямую), а хвост RefreshBoxTable пересчитывает
// оба веса заново через SetCharWeight.
bool MoveItemsBulk(ref rFrom, ref rTo, string sFilter, bool bWeightLimit, bool bKeepSetItem)
{
	int i, iItemsQty, maxItemsToAdd;
	String itemID;
	aref arInventory, arItem;
	object itemsToTake;
	float itemWeight;
	bool bOk = false;

	makearef(arInventory, rFrom.Items);
	for (i = 0; i < GetAttributesNum(arInventory); i++)
	{
		arItem = GetAttributeN(arInventory, i);
		itemID = GetAttributeName(arItem);

		iItemsQty = GetCharacterFreeItem(rFrom, itemID);
		if(bKeepSetItem)
		{
			if (CheckItemInSets(rFrom, itemID) > 0) iItemsQty--; //один комплектный предмет оставляем в кармане. если их больше одного, лишние выкидываем
//УТОЧНИТЬ, можно же два одинаковых предмета в комплекте сохранить? Тогда надо проверять это дополнительно
		}
		if(iItemsQty < 1) continue;
		if(!ItemMatchesStuffFilter(itemID, sFilter)) continue;

		if(bWeightLimit)
		{
			itemWeight = GetItemWeight(itemID);
			if(itemWeight > 0)
			{
				// Учет перегруза: сколько ещё влезет получателю
				maxItemsToAdd = makeint((GetMaxItemsWeight(rTo) - GetItemsWeight(rTo)) / itemWeight);
				if(maxItemsToAdd < iItemsQty) iItemsQty = maxItemsToAdd;
				if(iItemsQty < 1) continue;
				bOk = true;
			}
		}
		else
		{
			bOk = true;
		}

		itemsToTake.(itemID) = iItemsQty;
		AddItems(rTo, itemID, iItemsQty);
	}

	makearef(arInventory, itemsToTake);
	for (i = 0; i < GetAttributesNum(arInventory); i++)
	{
		arItem = GetAttributeN(arInventory, i);
		itemID = GetAttributeName(arItem);
		iItemsQty = sti(GetAttributeValue(arItem));

		RemoveItems(rFrom, itemID, iItemsQty);
	}

	return bOk;
}

// Gregg: фильтр «что за stuff» для кнопок Материалы/Идолы/Драгоценности ("" = любой предмет)
bool ItemMatchesStuffFilter(string itemID, string sFilter)
{
	if(sFilter == "") return true;
	if(sFilter == "material") return HasSubStr(itemID, "CompCraft");
	if(sFilter == "idol")
	{
		if(HasSubStr(itemID, "indian11")) return false;
		return HasSubStr(itemID, "indian");
	}
	if(sFilter == "jewelry")
	{
		if(HasSubStr(itemID, "jewelry")) return true;
		if(HasSubStr(itemID, "mineral")) return true;
		if(itemID == "Chest") return true;
		if(itemID == "icollection") return true;
		return false;
	}
	return true;
}

// Жмакнули по кнопке «Забрать всё»
void onGetAllBtnClick()
{
	if(MoveItemsBulk(refToChar, refCharacter, "", true, false)) WaitDate("", 0, 0, 0, 0, 1);
	RefreshBoxTable(true);
}

void PutStuff(string itmtype) // сложить в сундук материалы/идолы/драгоценности (по одному комплектному оставляем)
{
	MoveItemsBulk(refCharacter, refToChar, itmtype, false, true); // в сундук — без лимита веса
	RefreshBoxTable(true);
}

void GetStuff(string itmtype) // забрать из сундука материалы/идолы/драгоценности (LootCollector)
{
	MoveItemsBulk(refToChar, refCharacter, itmtype, true, false);
	RefreshBoxTable(true);
}

// инфа о предмете
void ShowGoodsInfo(string sItem)
{
	int itemIndex = FindItem(sItem);
	string GoodName = Items[itemIndex].name;
	int    lngFileID = LanguageOpenFile("ItemsDescribe.txt");
	string sHeader = LanguageConvertString(lngFileID, GoodName);

	sCurItem = sItem;
	string describeStr = "";

	if(bBettaTestMode)
	{
		describeStr += "id = " + sItem + NewStr();
	}
	describeStr += GetItemDescribe(sItem);

	fWeight = GetItemWeight(sItem);

	BuyOrSell = 0;
	SetFormatedText("QTY_TypeOperation", "");
	SetFormatedText("QTY_Result", "");
	GameInterface.qty_edit.str = "0";

	SetNewGroupPicture("QTY_GOODS_PICTURE", Items[itemIndex].picTexture, "itm" + Items[itemIndex].picIndex);
	SetFormatedText("QTY_CAPTION", sHeader);
	SetFormatedText("QTY_GOODS_INFO", describeStr);
	LanguageCloseFile(lngFileID);

	iCharQty = GetCharacterFreeItem(refCharacter, sItem);
	iStoreQty = GetCharacterFreeItem(refToChar, sItem);

	SetFormatedText("QTY_INFO_STORE_QTY", its(iStoreQty));
	SetFormatedText("QTY_INFO_SHIP_QTY", its(iCharQty));
	BuyOrSell = 0;
}

void TransactionOK()
{
	int nTradeQuantity;
	String list;
	confirmChangeQTY_EDIT();
	nTradeQuantity = sti(GameInterface.qty_edit.str);
	if(BuyOrSell == 0)
	{
	    EndTooltip();
	    return;
	}

 	if(BuyOrSell == 1) // Забираем
	{
		RemoveItems(refToChar, sCurItem, nTradeQuantity);
		AddItems(refCharacter, sCurItem, nTradeQuantity);
    	WaitDate("",0,0,0, 0, 1);
	}
 	else // Отдаем
	{
		AddItems(refToChar, sCurItem, nTradeQuantity);
		RemoveItems(refCharacter, sCurItem, nTradeQuantity);
		WaitDate("",0,0,0, 0, 1);
	}

	AddToTable(refToChar);
	EndTooltip();

	ShowGoodsInfo(sCurItem); //сбросим все состояния
}

void confirmChangeQTY_EDIT()
{
	ChangeQTY_EDIT();
	SetCurrentNode("QTY_OK_BUTTON");
}

// LEO: подтянуть sCurItem и количества к ТЕКУЩЕЙ выделенной строке. Кэш обновляется по событию
// TableSelectChange, но перезаливка таблицы (AddToTable после переброса) двигает выделение молча, без
// события - кэш оставался на прежнем, уже опустевшем предмете, и стрелки «переставали работать» до клика
// мышью. Атрибут .select движок держит актуальным при ЛЮБОМ способе смены строки (UpdateSelectImage
// в xi_table.cpp пишет его туда), так что он и есть источник правды.
void SyncCurItemToSelection()
{
	string sSel = "tr" + sti(GameInterface.TABLE_LIST.select);
	if(!CheckAttribute(&GameInterface, "TABLE_LIST." + sSel + ".id")) return;
	if(GameInterface.TABLE_LIST.(sSel).id == sCurItem) return;
	ShowGoodsInfo(GameInterface.TABLE_LIST.(sSel).id); // ставит sCurItem + iCharQty/iStoreQty
}

// LEO: прямой переброс предмета БЕЗ окна количества/подтверждения (стрелки прямо в таблице сундука).
// nReqQty > 0 — забрать столько из сундука персонажу; < 0 — отдать столько в сундук. Кол-во клампится
// ChangeQTY_EDIT (по доступному/весу/квестовости), затем перемещаем напрямую и обновляем таблицу.
void DoDirectTransfer(int nReqQty)
{
	int n = 0;
	SyncCurItemToSelection();
	if(sCurItem == "") return;
	GameInterface.qty_edit.str = nReqQty;
	BuyOrSell = 0;
	ChangeQTY_EDIT(); // клампит qty_edit и ставит BuyOrSell (1 = забрать, -1 = отдать)
	n = sti(GameInterface.qty_edit.str);
	if(n != 0 && BuyOrSell != 0)
	{
		if(BuyOrSell == 1) // забрать из сундука -> персонажу
		{
			RemoveItems(refToChar, sCurItem, n);
			AddItems(refCharacter, sCurItem, n);
		}
		else // отдать у персонажа -> в сундук
		{
			AddItems(refToChar, sCurItem, n);
			RemoveItems(refCharacter, sCurItem, n);
		}
		WaitDate("",0,0,0, 0, 1);
	}
	GameInterface.qty_edit.str = 0;
	BuyOrSell = 0;
	bLeoDirectXfer = true;  // LEO: пересборка без легаси-сдвига выделения (предмет остаётся на своей строке)
	RefreshBoxTable(false);
	bLeoDirectXfer = false;
	ShowGoodsInfo(sCurItem);
}

void ChangeQTY_EDIT()
{
	float iWeight;
	float fRefCharWeight, fRefCharMaxWeight;
	SetCharWeight();
	GameInterface.qty_edit.str = sti(GameInterface.qty_edit.str);
	if(sti(GameInterface.qty_edit.str) == 0)
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

		    if(iBoxMode == BOXMODE_EXCHANGE) // Сундуки и трупы бездонны
		    {
				fRefCharWeight = GetItemsWeight(refToChar);
				fRefCharMaxWeight = GetMaxItemsWeight(refToChar);
				iWeight = (fWeight * sti(GameInterface.qty_edit.str));
				if((fRefCharWeight + iWeight) > fRefCharMaxWeight)
				{
					iWeight = fRefCharMaxWeight - fRefCharWeight - 0.01; // чуть меньше
					GameInterface.qty_edit.str = makeint(iWeight / fWeight );
					iWeight = fWeight * sti(GameInterface.qty_edit.str);
					GameInterface.qty_edit.str = makeint(iWeight / fWeight ); // округдение
				}
		    }
		    // проверка на колво доступное <--
		    // квестовые не продать -->
		    if (IsQuestUsedItem(sCurItem) && sCurItem != "Gold")
		    {
		        GameInterface.qty_edit.str = 0;
		    }
			if (sti(GameInterface.qty_edit.str) < 0) GameInterface.qty_edit.str = 0;
		    // квестовые не продать <--
		    SetFormatedText("QTY_TypeOperation", GetConvertStrWithReplace("Variable_itemsbox_38", "Interface.txt", "#space#", " "));
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
		    if((fCharWeight + iWeight) > iCharCapacity && sCurItem != "Gold")
		    {
		        iWeight = iCharCapacity - fCharWeight - 0.01; // чуть меньше

				if(iWeight < 0) iWeight = 0;

		        GameInterface.qty_edit.str = makeint(iWeight / fWeight );
		        iWeight = fWeight * sti(GameInterface.qty_edit.str);
		        GameInterface.qty_edit.str = makeint(iWeight / fWeight ); // округдение
		    }
		    // проверка на колво доступное <--

			SetFormatedText("QTY_TypeOperation", GetConvertStrWithReplace("Variable_itemsbox_39", "Interface.txt", "#space#", " "));
		}
	}
	// если получили ноль
	if (sti(GameInterface.qty_edit.str) == 0)
	{
	    SetFormatedText("QTY_TypeOperation", "");
	    BuyOrSell = 0;
	}
    SetFormatedText("QTY_INFO_STORE_QTY", its(iStoreQty - BuyOrSell*sti(GameInterface.qty_edit.str)));
    SetFormatedText("QTY_INFO_SHIP_QTY", its(iCharQty + BuyOrSell*sti(GameInterface.qty_edit.str)));
    SetCharWeight();
    fCharWeight  = fCharWeight  + BuyOrSell * iWeight;
	fStoreWeight = fStoreWeight - BuyOrSell * iWeight;
    SetVariable();
}

void REMOVE_ALL_BUTTON()  // отдать все
{
	// LEO: синхронизация ДО чтения iCharQty - оно кэшируется ShowGoodsInfo и на чужой строке даёт 0 (переброс молчит)
	if(!bShowChangeWin) { SyncCurItemToSelection(); DoDirectTransfer(-iCharQty); return; }

	ShowGoodsInfo(sCurItem);
	GameInterface.qty_edit.str = -iCharQty;
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void ADD_ALL_BUTTON()  // забрать все
{
	// LEO: синхронизация ДО чтения iStoreQty - см. REMOVE_ALL_BUTTON
	if(!bShowChangeWin) { SyncCurItemToSelection(); DoDirectTransfer(iStoreQty); return; }

	ShowGoodsInfo(sCurItem);
	GameInterface.qty_edit.str = iStoreQty;
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void REMOVE_BUTTON()  // продать / прямой переброс 1 шт. без окна
{
	if(!bShowChangeWin) { DoDirectTransfer(-1); return; } // LEO: отдать 1 шт. сразу

	if(BuyOrSell == 0)
	{
		GameInterface.qty_edit.str = -1;
	}
	else
	{
		if(BuyOrSell == -1)
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

void ADD_BUTTON()  // купить / прямой переброс 1 шт. без окна
{
	if(!bShowChangeWin) { DoDirectTransfer(1); return; } // LEO: забрать 1 шт. сразу

	if(BuyOrSell == 0)
	{
		GameInterface.qty_edit.str = 1;
	}
	else
	{
		if(BuyOrSell == 1)
		{
			GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) + 1);
		}
		else
		{
			GameInterface.qty_edit.str = -(sti(GameInterface.qty_edit.str) - 1);
		}

		BuyOrSell = 0;
	}

	ChangeQTY_EDIT();
}
