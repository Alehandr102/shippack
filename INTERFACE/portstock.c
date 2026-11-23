// Стоянка кораблей в портовом управлении.
// Перенос механики хранения из диалога Common_Portman (узлы ShipStock_*) в интерфейс.
// Транзакции - PortStock_* в scripts\ShipsUtilites.c, образец вёрстки и кода - storage.c

// LEO: фильтр по классам кораблей (механика взята с верфи - радиокнопка CLASS_ALL)
#define FIS_ALL		0
#define FIS_1		1
#define FIS_2		2
#define FIS_3		3
#define FIS_4		4
#define FIS_5		5
#define FIS_6		6
#define FIS_7		7
int FIS_FilterState = FIS_ALL;

ref refPortman;
string curRow = "tr1";
int TableSelect = 0;
bool bShowCaptWin = false;
bool bOfficerMode = true;	// оставить корабль вместе с офицером
int nCaptQty = 0;			// кандидаты в капитаны при возврате корабля со сторожем
int nCurCaptScrollNum = -1;
int iTakeCharIdx = -1;		// сторож, чей корабль забираем через окно выбора капитана
bool ok;

void InitInterface_R(string iniName, ref pPortman)
{
	StartAboveForm(true);
	refPortman = pPortman;
	curRow = "tr1";
	TableSelect = 1;
	bShowCaptWin = false;
	bOfficerMode = true;
	iTakeCharIdx = -1;
	FIS_FilterState = FIS_ALL; // LEO: при открытии фильтр сброшен на "Все классы"

	GameInterface.TABLE_LIST.hr.td1.str = XI_ConvertString("PortStock_hdrType");
	GameInterface.TABLE_LIST.hr.td1.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td2.str = XI_ConvertString("PortStock_hdrName");
	GameInterface.TABLE_LIST.hr.td2.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td3.str = XI_ConvertString("PortStock_hdrClass");
	GameInterface.TABLE_LIST.hr.td3.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td4.str = XI_ConvertString("PortStock_hdrCapt");
	GameInterface.TABLE_LIST.hr.td4.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td5.str = XI_ConvertString("PortStock_hdrStatus");
	GameInterface.TABLE_LIST.hr.td5.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td6.str = XI_ConvertString("PortStock_hdrPrice");
	GameInterface.TABLE_LIST.hr.td6.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td1.sorttype = "";
	GameInterface.TABLE_LIST.hr.td2.sorttype = "";
	GameInterface.TABLE_LIST.hr.td3.sorttype = "";
	GameInterface.TABLE_LIST.hr.td4.sorttype = "";
	GameInterface.TABLE_LIST.hr.td5.sorttype = "";
	GameInterface.TABLE_LIST.hr.td6.sorttype = "";

	AddToTable();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("TableSelectChange", "CS_TableSelectChange", 0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
	SetEventHandler("MouseRClickUP","CloseCaptWin",0);
	SetEventHandler("CaptOK", "CaptOK", 0);
	SetEventHandler("frame","ProcessFrame",1);

	SetFormatedText("PORTSTOCKCAPTION", XI_ConvertString("Colony" + refPortman.City) + ": " + XI_ConvertString("titlePortStock"));
	SetFormatedText("CAPT_CAPTION", XI_ConvertString("PortStock_ChooseCapt"));
	UpdateOfficerBtn();
	UpdateInfoForSelection();
	LAi_SetActorTypeNoGroup(Pchar);
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_PORTSTOCK_EXIT );
}

void ProcessCancelExit()
{
	if (bShowCaptWin)
	{
		CloseCaptWin();
	}
	else
	{
		IDoExit( RC_INTERFACE_PORTSTOCK_EXIT );
	}
}

void IDoExit(int exitCode)
{
	EndAboveForm(true);
	LAi_SetPlayerType(PChar);

	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("TableSelectChange", "CS_TableSelectChange");
	DelEventHandler("OnTableClick", "OnTableClick");
	DelEventHandler("MouseRClickUP","CloseCaptWin");
	DelEventHandler("CaptOK", "CaptOK");
	DelEventHandler("frame","ProcessFrame");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	if (bShowCaptWin) return; // пока открыто окно выбора капитана, нижние кнопки не работают

	switch(nodName)
	{
		case "BTN_STORE_MONTH":
			DoStore(false);
		break;
		case "BTN_STORE_UNLIM":
			DoStore(true);
		break;
		case "BTN_TAKE":
			DoTake();
		break;
		case "BTN_OFFICER_ON":
			bOfficerMode = false;
			UpdateOfficerBtn();
		break;
		case "BTN_OFFICER_OFF":
			bOfficerMode = true;
			UpdateOfficerBtn();
		break;
		case "CLASS_ALL": // LEO: клик по радиокнопке фильтра классов
			ProcessFilter();
		break;
	}
}

// LEO: чтение состояния радиокнопки CLASS_ALL -> FIS_FilterState -> перезаполнение списка (механика верфи)
void ProcessFilter()
{
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 1)) { FIS_FilterState = FIS_ALL; AddToTable(); return; }
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 2)) { FIS_FilterState = FIS_7;   AddToTable(); return; }
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 3)) { FIS_FilterState = FIS_6;   AddToTable(); return; }
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 4)) { FIS_FilterState = FIS_5;   AddToTable(); return; }
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 5)) { FIS_FilterState = FIS_4;   AddToTable(); return; }
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 6)) { FIS_FilterState = FIS_3;   AddToTable(); return; }
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 7)) { FIS_FilterState = FIS_2;   AddToTable(); return; }
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 8)) { FIS_FilterState = FIS_1;   AddToTable(); return; }
}

void UpdateOfficerBtn()
{
	if (bOfficerMode)
	{
		SetNodeUsing("BTN_OFFICER_ON", true);
		SetNodeUsing("BTN_OFFICER_OFF", false);
	}
	else
	{
		SetNodeUsing("BTN_OFFICER_ON", false);
		SetNodeUsing("BTN_OFFICER_OFF", true);
	}
}

// ----------------------- таблица -----------------------

int GetRowAttr(string sRow, string sAttr)
{
	if (!CheckAttribute(&GameInterface, "TABLE_LIST." + sRow + "." + sAttr)) return -1;
	return sti(GameInterface.TABLE_LIST.(sRow).(sAttr));
}

void AddToTable()
{
	int n, i, cn, iPrice, iMonths;
	string row, sStatus, sPrice;
	ref chref;
	ref rKeeper;
	int iColor;

	n = 1;
	Table_Clear("TABLE_LIST", false, true, false);

	// эскадра в порту (флагман сдать нельзя, как и в диалоге)
	for (i=1; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(PChar, i);
		if (cn <= 0) continue;
		chref = GetCharacter(cn);
		if (!GetRemovable(chref)) continue;
		if (RealShips[sti(chref.Ship.Type)].BaseName == SHIP_SOLEYRU) continue; // отрубаем хитрость на получение Солея
		if (PGG_IsCompanionOwnShip(chref)) continue;    // LEO: корабль конкурента, а не наш - сдать его нельзя
		if (FIS_FilterState != FIS_ALL && sti(RealShips[sti(chref.Ship.Type)].Class) != sti(FIS_FilterState)) continue; // LEO: фильтр по классу

		row = "tr" + n;
		iPrice = PortStock_GetStorePrice(refPortman, chref);
		FillShipRow(row, chref, GetFullName(chref), XI_ConvertString("PortStock_InSquadron"), "" + iPrice, argb(255,255,255,255)); // в ячейке без слова "пиастров", иначе перенос строки
		GameInterface.TABLE_LIST.(row).chridx = cn;
		GameInterface.TABLE_LIST.(row).rtype = 0;
		n++;
	}

	// корабли на хранении: сначала у этого портмана, затем в других портах (только просмотр)
	for (i=1; i<MAX_CHARACTERS; i++)
	{
		chref = &Characters[i];
		if (!CheckAttribute(chref, "ShipInStockMan")) continue;
		if (chref.ShipInStockMan == "") continue;
		if (FIS_FilterState != FIS_ALL && sti(RealShips[sti(chref.Ship.Type)].Class) != sti(FIS_FilterState)) continue; // LEO: фильтр по классу

		row = "tr" + n;
		if (chref.ShipInStockMan == refPortman.id)
		{
			if (CheckAttribute(chref, "ShipInStockMan.Unlimited"))
			{
				sStatus = XI_ConvertString("PortStock_StoredUnlim");
				sPrice = "-";
			}
			else
			{
				iMonths = GetNpcQuestPastMonthParam(chref, "ShipInStockMan.Date");
				sStatus = "" + iMonths + " " + XI_ConvertString("PortStock_Months"); // коротко, иначе перенос в ячейке
				sPrice = "" + PortStock_GetReturnDebt(chref);
			}
			FillShipRow(row, chref, GetKeeperName(chref), sStatus, sPrice, argb(255,255,225,160));
			GameInterface.TABLE_LIST.(row).chridx = i;
			GameInterface.TABLE_LIST.(row).rtype = 1;
			n++;
		}
		else
		{
			rKeeper = CharacterFromID(chref.ShipInStockMan);
			sStatus = XI_ConvertString("Colony" + rKeeper.City);
			FillShipRow(row, chref, GetKeeperName(chref), sStatus, "-", argb(255,150,150,150));
			GameInterface.TABLE_LIST.(row).chridx = i;
			GameInterface.TABLE_LIST.(row).rtype = 2;
			n++;
		}
	}

	NextFrameRefreshTable();
}

void FillShipRow(string row, ref chref, string sCaptain, string sStatus, string sPrice, int iColor)
{
	ref rBaseShip = GetRealShip(sti(chref.Ship.Type));
	string potc = "";
	if (InterfaceStates.InterfaceShipString == "interfaces\ships_PotC\") potc = "_POTC";

	GameInterface.TABLE_LIST.(row).td1.icon.group = "SHIPS_" + ShipsTypes[sti(rBaseShip.basetype)].Name + potc;
	GameInterface.TABLE_LIST.(row).td1.icon.image = "ship";
	GameInterface.TABLE_LIST.(row).td1.icon.width = 41;
	GameInterface.TABLE_LIST.(row).td1.icon.height = 41;
	GameInterface.TABLE_LIST.(row).td1.icon.offset = "-2, 0";
	GameInterface.TABLE_LIST.(row).td1.textoffset = "42,0";
	GameInterface.TABLE_LIST.(row).td1.str = XI_ConvertString(RealShips[sti(chref.Ship.Type)].BaseName);
	GameInterface.TABLE_LIST.(row).td1.color = iColor;
	GameInterface.TABLE_LIST.(row).td2.str = chref.Ship.Name;
	GameInterface.TABLE_LIST.(row).td2.color = iColor;
	GameInterface.TABLE_LIST.(row).td3.str = RealShips[sti(chref.Ship.Type)].Class;
	GameInterface.TABLE_LIST.(row).td3.color = iColor;
	if (CheckAttribute(chref, "FaceID")) // портрет капитана/сторожа
	{
		GameInterface.TABLE_LIST.(row).td4.icon.group = "FACE128_" + chref.FaceID;
		GameInterface.TABLE_LIST.(row).td4.icon.image = "face";
		GameInterface.TABLE_LIST.(row).td4.icon.width = 41;
		GameInterface.TABLE_LIST.(row).td4.icon.height = 41;
		GameInterface.TABLE_LIST.(row).td4.icon.offset = "-2, 0";
		GameInterface.TABLE_LIST.(row).td4.textoffset = "42,0";
	}
	GameInterface.TABLE_LIST.(row).td4.str = sCaptain;
	GameInterface.TABLE_LIST.(row).td4.color = iColor;
	GameInterface.TABLE_LIST.(row).td5.str = sStatus;
	GameInterface.TABLE_LIST.(row).td5.color = iColor;
	GameInterface.TABLE_LIST.(row).td6.str = sPrice;
	GameInterface.TABLE_LIST.(row).td6.color = iColor;
}

string GetKeeperName(ref chref)
{
	if (HasSubStr(chref.id, "ShipInStockMan_")) return chref.name; // сторож
	return GetFullName(chref);
}

void NextFrameRefreshTable()
{
	SetEventHandler("frame", "RefreshTableByFrameEvent", 0);
}

void RefreshTableByFrameEvent()
{
	DelEventHandler("frame", "RefreshTableByFrameEvent");
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABLE_LIST", 0 );
}

void CS_TableSelectChange()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();
	TableSelect = iSelected;
	curRow = "tr" + iSelected;
	UpdateInfoForSelection();
}

void OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();
	Table_UpdateWindow(sControl);
}

// ----------------------- инфострока и кнопки -----------------------

void UpdateInfoForSelection()
{
	int rtype = GetRowAttr(curRow, "rtype");
	int chridx = GetRowAttr(curRow, "chridx");
	string sInfo = XI_ConvertString("PortStock_Money") + " " + FindRussianMoneyString(sti(pchar.Money)) + "\n";
	ref chref;
	int iPrice;

	SetSelectable("BTN_STORE_MONTH", false);
	SetSelectable("BTN_STORE_UNLIM", false);
	SetSelectable("BTN_TAKE", false);

	if (rtype == 0 && chridx > 0)
	{
		chref = GetCharacter(chridx);
		iPrice = PortStock_GetStorePrice(refPortman, chref);
		if (iPrice > 0)
		{
			sInfo = sInfo + FindRussianMoneyString(iPrice) + " " + XI_ConvertString("PortStock_PerMonth") + " " + XI_ConvertString("PortStock_OrUnlim") + " " + FindRussianMoneyString(iPrice*6) + ".";
			SetSelectable("BTN_STORE_UNLIM", true);
		}
		else
		{
			sInfo = sInfo + XI_ConvertString("PortStock_Free"); // Кайман
		}
		SetSelectable("BTN_STORE_MONTH", true);
		int iWnd0 = GetWoundedQuantity(chref); // LEO: предупреждение - раненые на сдаваемом корабле уйдут в лазарет
		if (iWnd0 > 0)
		{
			if (LanguageGetLanguage() == "russian") sInfo = sInfo + "\nРаненые (" + iWnd0 + ") отправятся в городской лазарет.";
			else sInfo = sInfo + "\nThe wounded (" + iWnd0 + ") will be sent to the town infirmary.";
		}
	}
	if (rtype == 1 && chridx > 0)
	{
		chref = GetCharacter(chridx);
		if (CheckAttribute(chref, "ShipInStockMan.Unlimited"))
		{
			sInfo = sInfo + XI_ConvertString("PortStock_NoDebt");
		}
		else
		{
			iPrice = PortStock_GetReturnDebt(chref);
			sInfo = sInfo + XI_ConvertString("PortStock_DebtIs") + " " + FindRussianMoneyString(iPrice) + ".";
		}
		SetSelectable("BTN_TAKE", true);
	}
	if (rtype == 2)
	{
		sInfo = sInfo + XI_ConvertString("PortStock_OtherPort");
	}
	SetFormatedText("INFO_TEXT", sInfo);
}

void ShowMessage(string sMsg)
{
	SetFormatedText("INFO_TEXT", XI_ConvertString("PortStock_Money") + " " + FindRussianMoneyString(sti(pchar.Money)) + "\n" + sMsg);
}

// ----------------------- действия -----------------------

void DoStore(bool bUnlimited)
{
	int rtype = GetRowAttr(curRow, "rtype");
	int chridx = GetRowAttr(curRow, "chridx");
	if (rtype != 0 || chridx <= 0) return;

	ref chref = GetCharacter(chridx);
	if (SeaTowOperations_Block(chref)) return;
	if (AICapturedTrap_IsShipInfected(chref)) // LEO: заражённый корабль на стоянку не принимают (лазейка от мора)
	{
		ShowMessage(XI_ConvertString("PortStock_EpidemyRefuse"));
		return;
	}
	int iPrice = PortStock_GetStorePrice(refPortman, chref);
	int iPayment = iPrice;
	if (bUnlimited) iPayment = iPrice * 6;
	if (sti(pchar.Money) < iPayment)
	{
		ShowMessage(XI_ConvertString("PortStock_NoMoney"));
		return;
	}

	PortStock_StoreShip(refPortman, chridx, bOfficerMode, bUnlimited);
	curRow = "tr1";
	TableSelect = 1;
	AddToTable();
	UpdateInfoForSelection();
}

void DoTake()
{
	int rtype = GetRowAttr(curRow, "rtype");
	int chridx = GetRowAttr(curRow, "chridx");
	if (rtype != 1 || chridx <= 0) return;

	ref chref = GetCharacter(chridx);
	if (GetCompanionQuantity(pchar) >= COMPANION_MAX)
	{
		ShowMessage(XI_ConvertString("PortStock_SquadFull"));
		return;
	}
	if (sti(pchar.Money) < PortStock_GetReturnDebt(chref))
	{
		ShowMessage(XI_ConvertString("PortStock_NoMoney"));
		return;
	}

	if (HasSubStr(chref.id, "ShipInStockMan_"))
	{
		// корабль со сторожем - нужен новый капитан из пассажиров
		FillCaptScroll();
		if (nCaptQty == 0)
		{
			ShowMessage(XI_ConvertString("PortStock_NoCapt"));
			return;
		}
		iTakeCharIdx = chridx;
		bShowCaptWin = true;
		XI_WindowDisable("CAPT_WINDOW", false);
		XI_WindowShow("CAPT_WINDOW", true);
		SetCurrentNode("CAPT_SCROLL");
		UpdateCaptDisplay();
	}
	else
	{
		PortStock_ReturnShip(refPortman, chridx, -1);
		curRow = "tr1";
		TableSelect = 1;
		AddToTable();
		UpdateInfoForSelection();
	}
}

// ----------------------- выбор капитана -----------------------

// скролл портретов пассажиров-кандидатов, по образцу FillPassengerScroll с верфи
void FillCaptScroll()
{
	int i, _curCharIdx, m;
	string attributeName, sTemp;
	ref sld;
	bool bad;

	DeleteAttribute(&GameInterface, "CAPT_SCROLL");
	nCurCaptScrollNum = -1;
	GameInterface.CAPT_SCROLL.current = 0;
	GameInterface.CAPT_SCROLL.NotUsed = 6;
	GameInterface.CAPT_SCROLL.ImagesGroup.t0 = "EMPTYFACE";
	FillFaceList("CAPT_SCROLL.ImagesGroup", pchar, 2); // passengers
	GameInterface.CAPT_SCROLL.BadTex1 = 0;
	GameInterface.CAPT_SCROLL.BadPic1 = "emptyface";

	m = 0;
	int nListSize = GetPassengersQuantity(pchar);
	for (i=0; i<nListSize; i++)
	{
		_curCharIdx = GetPassenger(pchar, i);
		if (_curCharIdx == -1) continue;
		sld = GetCharacter(_curCharIdx);
		bad = CheckAttribute(sld, "prisoned") && sti(sld.prisoned) == true;
		if (bad) continue;
		bad = CheckAttribute(sld, "CompanionDisable") && sti(sld.CompanionDisable) == true;
		if (bad) continue;
		if (CheckAttribute(sld, "HPminusDaysNeedtoRestore")) continue;
		if (!GetRemovable(sld)) continue;
		if (sld.id == "pet_crab") continue;

		attributeName = "pic" + (m+1);
		GameInterface.CAPT_SCROLL.(attributeName).character = _curCharIdx;
		GameInterface.CAPT_SCROLL.(attributeName).img1 = GetFacePicName(sld);
		GameInterface.CAPT_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CAPT_SCROLL.ImagesGroup", "FACE128_" + sld.FaceID);
		// специализация и должности иконками под портретом
		sTemp = "#" + GetIconFromFont(GetSpeciality(sld));
		if (IsOfficer(sld)) sTemp += GetIconFromFont("fighter");
		if (sti(pchar.Fellows.Passengers.navigator) == sti(sld.index)) sTemp += GetIconFromFont("navigator");
		if (sti(pchar.Fellows.Passengers.boatswain) == sti(sld.index)) sTemp += GetIconFromFont("boatswain");
		if (sti(pchar.Fellows.Passengers.cannoner) == sti(sld.index)) sTemp += GetIconFromFont("cannoner");
		if (sti(pchar.Fellows.Passengers.doctor) == sti(sld.index)) sTemp += GetIconFromFont("doctor");
		if (sti(pchar.Fellows.Passengers.carpenter) == sti(sld.index)) sTemp += GetIconFromFont("carpenter");
		if (sti(pchar.Fellows.Passengers.treasurer) == sti(sld.index)) sTemp += GetIconFromFont("treasurer");
		GameInterface.CAPT_SCROLL.(attributeName).str1 = sTemp;
		m++;
	}
	nCaptQty = m;
	GameInterface.CAPT_SCROLL.ListSize = m + 2; // как на верфи, для совместимости
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"CAPT_SCROLL",-1); // без этого узел не перечитает список
}

int GetCaptScrollCharIdx()
{
	string sPic = "pic" + (sti(GameInterface.CAPT_SCROLL.current) + 1);
	if (!CheckAttribute(&GameInterface, "CAPT_SCROLL." + sPic + ".character")) return -1;
	return sti(GameInterface.CAPT_SCROLL.(sPic).character);
}

void UpdateCaptDisplay()
{
	int iCapt = GetCaptScrollCharIdx();
	if (iCapt < 0)
	{
		SetFormatedText("CAPT_NAME", "");
		return;
	}
	SetFormatedText("CAPT_NAME", GetFullName(GetCharacter(iCapt)));
}

void ProcessFrame()
{
	if (!bShowCaptWin) return;
	if (sti(GameInterface.CAPT_SCROLL.current) != nCurCaptScrollNum)
	{
		nCurCaptScrollNum = sti(GameInterface.CAPT_SCROLL.current);
		UpdateCaptDisplay();
	}
}

void CaptOK()
{
	if (!bShowCaptWin) return;
	int iCapt = GetCaptScrollCharIdx();
	if (iCapt < 0) return;
	PortStock_ReturnShip(refPortman, iTakeCharIdx, iCapt);
	CloseCaptWin();
	curRow = "tr1";
	TableSelect = 1;
	AddToTable();
	UpdateInfoForSelection();
}

void CloseCaptWin()
{
	if (!bShowCaptWin) return;
	bShowCaptWin = false;
	iTakeCharIdx = -1;
	XI_WindowDisable("CAPT_WINDOW", true);
	XI_WindowShow("CAPT_WINDOW", false);
	SetCurrentNode("TABLE_LIST");
}
