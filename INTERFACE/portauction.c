// LEO: Аукцион (продажа корабля) в портовом управлении. Образец вёрстки/кода - portstock.c.
// Транзакции - PortAuction_* в scripts\ShipsUtilites.c. Выручка копится у портмана колонии (AuctionVault).

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
bool ok;
string SellState = "Max";    // LEO: выбор судьбы команды в окне подтверждения (Max/Opt/Fire) - как на верфи
int g_iPendingSubmitIdx = -1; // LEO: индекс корабля, ожидающего подтверждения в окне
// LEO: окно выбора капитана при ВОЗВРАТЕ корабля с торгов в эскадру (передумал продавать)
bool bShowCaptWin = false;
int nCaptQty = 0;
int nCurCaptScrollNum = -1;
int iRecallKeeperIdx = -1;   // держатель-лот, чей корабль возвращаем

void InitInterface_R(string iniName, ref pPortman)
{
	StartAboveForm(true);
	refPortman = pPortman;
	curRow = "tr1";
	TableSelect = 1;
	bShowCaptWin = false;
	iRecallKeeperIdx = -1;
	FIS_FilterState = FIS_ALL; // LEO: при открытии фильтр сброшен на "Все классы"

	GameInterface.TABLE_LIST.hr.td1.str = XI_ConvertString("PortStock_hdrType");
	GameInterface.TABLE_LIST.hr.td1.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td2.str = XI_ConvertString("PortStock_hdrName");
	GameInterface.TABLE_LIST.hr.td2.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td3.str = XI_ConvertString("PortStock_hdrClass");
	GameInterface.TABLE_LIST.hr.td3.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td4.str = XI_ConvertString("PortStock_hdrCapt");
	GameInterface.TABLE_LIST.hr.td4.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td5.str = XI_ConvertString("PortAuction_hdrCity");
	GameInterface.TABLE_LIST.hr.td5.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td6.str = XI_ConvertString("PortStock_hdrStatus");
	GameInterface.TABLE_LIST.hr.td6.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td7.str = XI_ConvertString("PortAuction_hdrGain");
	GameInterface.TABLE_LIST.hr.td7.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td1.sorttype = "";
	GameInterface.TABLE_LIST.hr.td2.sorttype = "";
	GameInterface.TABLE_LIST.hr.td3.sorttype = "";
	GameInterface.TABLE_LIST.hr.td4.sorttype = "";
	GameInterface.TABLE_LIST.hr.td5.sorttype = "";
	GameInterface.TABLE_LIST.hr.td6.sorttype = "";
	GameInterface.TABLE_LIST.hr.td7.sorttype = "";

	AddToTable();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("TableSelectChange", "CS_TableSelectChange", 0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
	SetEventHandler("CheckButtonChange", "procCheckBoxChange", 0); // LEO: радиокнопки выбора команды в окне
	SetEventHandler("ExitMsgMenu", "ExitMsgMenu", 0);              // LEO: закрытие окна подтверждения
	SetEventHandler("MouseRClickUP","CloseCaptWin",0);            // LEO: ПКМ закрывает окно выбора капитана
	SetEventHandler("CaptOK", "CaptOK", 0);                       // LEO: подтверждение выбора капитана при возврате
	SetEventHandler("frame","ProcessFrame",1);                   // LEO: обновление имени капитана в скролле

	SetFormatedText("PORTAUCTIONCAPTION", XI_ConvertString("Colony" + refPortman.City) + ": " + XI_ConvertString("titlePortAuction"));
	SetFormatedText("CAPT_CAPTION", XI_ConvertString("PortStock_ChooseCapt"));
	UpdateInfoForSelection();
	LAi_SetActorTypeNoGroup(Pchar);
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_PORTAUCTION_EXIT );
}

void ProcessCancelExit()
{
	if (bShowCaptWin) { CloseCaptWin(); return; } // LEO: закрыть окно выбора капитана, не выходя из интерфейса
	IDoExit( RC_INTERFACE_PORTAUCTION_EXIT );
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
	DelEventHandler("CheckButtonChange", "procCheckBoxChange");
	DelEventHandler("ExitMsgMenu", "ExitMsgMenu");
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
	switch(nodName)
	{
		case "BTN_SUBMIT":
			DoSubmit();
		break;
		case "BTN_WITHDRAW":
			DoWithdraw();
		break;
		case "BTN_RECALL":
			DoRecall();
		break;
		case "MSG_OK":
			MessageOk();
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

// ----------------------- таблица -----------------------

int GetRowAttr(string sRow, string sAttr)
{
	if (!CheckAttribute(&GameInterface, "TABLE_LIST." + sRow + "." + sAttr)) return -1;
	return sti(GameInterface.TABLE_LIST.(sRow).(sAttr));
}

void AddToTable()
{
	int n, i, cn;
	string row, sStatus, sGain;
	ref chref, rKeeper;

	n = 1;
	Table_Clear("TABLE_LIST", false, true, false);

	// эскадра в порту (флагман продать нельзя)
	for (i=1; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(PChar, i);
		if (cn <= 0) continue;
		chref = GetCharacter(cn);
		if (!GetRemovable(chref)) continue;
		if (RealShips[sti(chref.Ship.Type)].BaseName == SHIP_SOLEYRU) continue; // отрубаем хитрость на получение Солея
		if (PGG_IsCompanionOwnShip(chref)) continue;    // LEO: корабль конкурента, а не наш - выставить его нельзя
		if (FIS_FilterState != FIS_ALL && sti(RealShips[sti(chref.Ship.Type)].Class) != sti(FIS_FilterState)) continue; // LEO: фильтр по классу

		row = "tr" + n;
		sGain = "~" + PortAuction_GetShipValue(chref, refPortman); // ориентир: полная стоимость (выручка будет 30-50%)
		FillShipRow(row, chref, "-", XI_ConvertString("PortStock_InSquadron"), sGain, argb(255,255,255,255));
		GameInterface.TABLE_LIST.(row).chridx = cn;
		GameInterface.TABLE_LIST.(row).rtype = 0;
		n++;
	}

	// корабли на аукционе: сначала у этого портмана, затем в других портах (только просмотр)
	for (i=1; i<MAX_CHARACTERS; i++)
	{
		chref = &Characters[i];
		if (!CheckAttribute(chref, "ShipAtAuction")) continue;
		if (chref.ShipAtAuction == "") continue;
		if (FIS_FilterState != FIS_ALL && sti(RealShips[sti(chref.Ship.Type)].Class) != sti(FIS_FilterState)) continue; // LEO: фильтр по классу

		row = "tr" + n;
		sStatus = "" + sti(chref.ShipAtAuction.Days) + " " + XI_ConvertString("PortAuction_DaysShort");
		sGain = "" + sti(chref.ShipAtAuction.Target);
		if (chref.ShipAtAuction == refPortman.id)
		{
			FillShipRow(row, chref, XI_ConvertString("Colony" + refPortman.City), sStatus, sGain, argb(255,255,225,160));
			GameInterface.TABLE_LIST.(row).chridx = i;
			GameInterface.TABLE_LIST.(row).rtype = 1;
			n++;
		}
		else
		{
			rKeeper = CharacterFromID(chref.ShipAtAuction);
			FillShipRow(row, chref, XI_ConvertString("Colony" + rKeeper.City), sStatus, sGain, argb(255,150,150,150));
			GameInterface.TABLE_LIST.(row).chridx = i;
			GameInterface.TABLE_LIST.(row).rtype = 2;
			n++;
		}
	}

	NextFrameRefreshTable();
}

void FillShipRow(string row, ref chref, string sCity, string sStatus, string sPrice, int iColor)
{
	ref rBaseShip = GetRealShip(sti(chref.Ship.Type));
	string potc = "";
	if (InterfaceStates.InterfaceShipString == "interfaces\ships_PotC\") potc = "_POTC";

	GameInterface.TABLE_LIST.(row).td1.icon.group = "SHIPS_" + ShipsTypes[sti(rBaseShip.basetype)].Name + potc;
	GameInterface.TABLE_LIST.(row).td1.icon.image = "ship";
	GameInterface.TABLE_LIST.(row).td1.icon.width = 57;
	GameInterface.TABLE_LIST.(row).td1.icon.height = 57;
	GameInterface.TABLE_LIST.(row).td1.icon.offset = "-2, 0";
	GameInterface.TABLE_LIST.(row).td1.textoffset = "42,0";
	GameInterface.TABLE_LIST.(row).td1.str = XI_ConvertString(RealShips[sti(chref.Ship.Type)].BaseName);
	GameInterface.TABLE_LIST.(row).td1.color = iColor;
	GameInterface.TABLE_LIST.(row).td1.align = "right";
	GameInterface.TABLE_LIST.(row).td2.str = chref.Ship.Name;
	GameInterface.TABLE_LIST.(row).td2.color = iColor;
	GameInterface.TABLE_LIST.(row).td3.str = RealShips[sti(chref.Ship.Type)].Class;
	GameInterface.TABLE_LIST.(row).td3.color = iColor;
	if (CheckAttribute(chref, "FaceID")) // портрет капитана/аукциониста
	{
		GameInterface.TABLE_LIST.(row).td4.icon.group = "FACE128_" + chref.FaceID;
		GameInterface.TABLE_LIST.(row).td4.icon.image = "face";
		GameInterface.TABLE_LIST.(row).td4.icon.width = 57;
		GameInterface.TABLE_LIST.(row).td4.icon.height = 57;
		GameInterface.TABLE_LIST.(row).td4.icon.offset = "-2, 0";
		GameInterface.TABLE_LIST.(row).td4.textoffset = "42,0";
	}
	GameInterface.TABLE_LIST.(row).td4.str = ""; // имя капитана убрано - только фото
	GameInterface.TABLE_LIST.(row).td4.color = iColor;
	GameInterface.TABLE_LIST.(row).td5.str = sCity;
	GameInterface.TABLE_LIST.(row).td5.color = iColor;
	GameInterface.TABLE_LIST.(row).td6.str = sStatus;
	GameInterface.TABLE_LIST.(row).td6.color = iColor;
	GameInterface.TABLE_LIST.(row).td7.str = sPrice;
	GameInterface.TABLE_LIST.(row).td7.color = iColor;
}

string GetKeeperName(ref chref)
{
	if (HasSubStr(chref.id, "ShipAtAuction_")) return chref.name; // аукционист
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
	int iVault = 0;
	if (CheckAttribute(refPortman, "AuctionVault")) iVault = sti(refPortman.AuctionVault);

	string sInfo = XI_ConvertString("PortStock_Money") + " " + FindRussianMoneyString(sti(pchar.Money)) + "    " + XI_ConvertString("PortAuction_Vault") + " " + FindRussianMoneyString(iVault) + "\n";

	SetSelectable("BTN_SUBMIT", false);
	SetSelectable("BTN_RECALL", false);
	SetSelectable("BTN_WITHDRAW", iVault > 0);

	if (rtype == 0 && chridx > 0)
	{
		sInfo = sInfo + XI_ConvertString("PortAuction_HintSubmit");
		SetSelectable("BTN_SUBMIT", true);
	}
	if (rtype == 1 && chridx > 0)
	{
		ref chref = GetCharacter(chridx);
		sInfo = sInfo + XI_ConvertString("PortAuction_HintAtSale") + " " + sti(chref.ShipAtAuction.Days) + " " + XI_ConvertString("PortAuction_DaysShort") + ".";
		SetSelectable("BTN_RECALL", true); // LEO: свой лот - можно вернуть в эскадру, пока не продан
	}
	if (rtype == 2)
	{
		sInfo = sInfo + XI_ConvertString("PortStock_OtherPort");
	}
	SetFormatedText("INFO_TEXT", sInfo);
}

void ShowMessage(string sMsg)
{
	SetFormatedText("INFO_TEXT", sMsg);
}

// ----------------------- действия -----------------------

void DoSubmit()
{
	int rtype = GetRowAttr(curRow, "rtype");
	int chridx = GetRowAttr(curRow, "chridx");
	if (rtype != 0 || chridx <= 0) return;

	ref chref = GetCharacter(chridx);
	if (SeaTowOperations_Block(chref)) return;
	if (RealShips[sti(chref.Ship.Type)].BaseName == SHIP_SOLEYRU) return;
	if (AICapturedTrap_IsShipInfected(chref)) // LEO: заражённый мором корабль на аукцион не выставить
	{
		ShowMessage(XI_ConvertString("PortStock_EpidemyRefuse"));
		return;
	}

	// LEO: окно подтверждения (как на верфи) - выбор судьбы команды + предупреждение о невозврате.
	// SellState НЕ сбрасываем: переключатель сохраняет визуальный выбор между открытиями (старт - Max из глобала)
	g_iPendingSubmitIdx = chridx;
	SetFormatedText("MSG_WINDOW_CAPTION", XI_ConvertString("titlePortAuction"));
	SetFormatedText("MSG_WINDOW_TEXT",  XI_ConvertString("PortAuction_ConfirmTitle"));
	SetFormatedText("MSG_WINDOW_TEXT2", GetConvertStrWithReplace("Variable_shipyard_37", "Interface.txt", "#space#", " ")); // "Как поступить с командой?"
	string sAucHint = XI_ConvertString("PortAuction_SellHint");
	int iWndA = GetWoundedQuantity(chref); // LEO: предупреждение - раненые на выставляемом корабле уйдут в лазарет
	if (iWndA > 0)
	{
		if (LanguageGetLanguage() == "russian") sAucHint = sAucHint + "\nРаненые (" + iWndA + ") отправятся в городской лазарет.";
		else sAucHint = sAucHint + "\nThe wounded (" + iWndA + ") will be sent to the town infirmary.";
	}
	SetFormatedText("MSG_WINDOW_TEXT4", sAucHint);
	ShowMsgMenu();
}

// показать окно подтверждения
void ShowMsgMenu()
{
	XI_WindowShow("MSG_WINDOW", true);
	XI_WindowDisable("MSG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	SetNodeUsing("MSG_CREW_CHECKBOX", true);
	SetCurrentNode("MSG_CANCEL");
}

// закрыть окно подтверждения (без действия)
void ExitMsgMenu()
{
	XI_WindowShow("MSG_WINDOW", false);
	XI_WindowDisable("MSG_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);
	g_iPendingSubmitIdx = -1;
	SetCurrentNode("TABLE_LIST");
}

// OK в окне - реально выставляем корабль с выбранным режимом команды
void MessageOk()
{
	int idx = g_iPendingSubmitIdx;
	ExitMsgMenu();
	if (idx <= 0) return;
	PortAuction_SubmitShip(refPortman, idx, SellState);
	curRow = "tr1";
	TableSelect = 1;
	AddToTable();
	UpdateInfoForSelection();
}

// смена радиокнопки выбора судьбы команды
void procCheckBoxChange()
{
	string sNodName = GetEventData();
	int nBtnIndex = GetEventData();
	int bBtnState = GetEventData();
	if (sNodName == "MSG_CREW_CHECKBOX")
	{
		if (bBtnState == true)
		{
			switch (nBtnIndex)
			{
			case 1: SellState = "Max"; break;
			case 2: SellState = "Opt"; break;
			case 3: SellState = "Fire"; break;
			}
		}
	}
}

void DoWithdraw()
{
	PortAuction_WithdrawMoney(refPortman);
	AddToTable();
	UpdateInfoForSelection();
}

// ----------------------- возврат корабля с торгов (передумал продавать) -----------------------

void DoRecall()
{
	int rtype = GetRowAttr(curRow, "rtype");
	int chridx = GetRowAttr(curRow, "chridx");
	if (rtype != 1 || chridx <= 0) return; // только СВОЙ лот аукциона (rtype==1)
	if (GetCompanionQuantity(pchar) >= COMPANION_MAX)
	{
		ShowMessage(XI_ConvertString("PortStock_SquadFull"));
		return;
	}
	// корабль был стрипнут при выставлении - нужен новый капитан из пассажиров (как возврат со сторожем на стоянке)
	FillCaptScroll();
	if (nCaptQty == 0)
	{
		ShowMessage(XI_ConvertString("PortStock_NoCapt"));
		return;
	}
	iRecallKeeperIdx = chridx;
	bShowCaptWin = true;
	XI_WindowDisable("CAPT_WINDOW", false);
	XI_WindowShow("CAPT_WINDOW", true);
	SetCurrentNode("CAPT_SCROLL");
	UpdateCaptDisplay();
}

// скролл портретов пассажиров-кандидатов в капитаны (1:1 с portstock.c)
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
	GameInterface.CAPT_SCROLL.ListSize = m + 2;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"CAPT_SCROLL",-1);
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
	PortAuction_RecallShip(refPortman, iRecallKeeperIdx, iCapt);
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
	iRecallKeeperIdx = -1;
	XI_WindowDisable("CAPT_WINDOW", true);
	XI_WindowShow("CAPT_WINDOW", false);
	SetCurrentNode("TABLE_LIST");
}
