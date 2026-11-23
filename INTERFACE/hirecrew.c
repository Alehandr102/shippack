////    boal 30.07.06 найм матросов
int	nCurScrollNum = 0;
ref refCharacter;
ref refTown;

int  BuyOrSell = 0; // 1-buy -1 sell
int  iPriceSailor;
int	 QtyMax = 0;
int	 CrewQty = 0;
bool bHireAllPending = false; // LEO: превью «Нанять на все» активно (ждёт ОК); сброс по скроллу/Отмене/одиночной операции
bool bHireAllOptimal = false; // LEO: последний режим превью: true=оптимум (ПКМ), false=максимум (ЛКМ)
void InitInterface(string iniName)
{
 	StartAboveForm(true);

	refCharacter = pchar;

	// город, где ГГ
	refTown = GetColonyByIndex(FindColony(loadedLocation.fastreload));
    FillShipsScroll();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	CreateString(true,"ShipName","",FONT_NORMAL,COLOR_MONEY, 405,180,SCRIPT_ALIGN_CENTER,1.0);

	SetDescription();

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);

	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("frame","ProcessFrame",1);

	SetEventHandler("TransactionOK", "TransactionOK", 0);
	SetEventHandler("TransactionCancel", "TransactionCancel", 0);
	SetEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT", 0);
	SetEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON", 0);
	SetEventHandler("ADD_BUTTON","ADD_BUTTON",0);
	SetEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON", 0);
	SetEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON", 0);

	SetNewGroupPicture("CREW_PICTURE", "SHIP_STATE_ICONS", "Crew");
	SetNewGroupPicture("CREW_PICTURE2", "SHIP_STATE_ICONS", "Crew");
	SetBackupQty();
	SetCurrentNode("SHIPS_SCROLL");
	GameInterface.qty_edit.str = 0;
	QtyMax = GetCargoFreeSpace(refCharacter);

	if(CheckAttribute(refTown, "AdditionalCrew"))
	{
		if(!IsEquipCharacterByArtefact(pchar, "talisman11"))
		{
			CrewQty = GetCrewQuantity(refTown) - sti(refTown.AdditionalCrew);
			if(CrewQty < 0) CrewQty = 0;
			SetCrewQuantity(refTown, CrewQty);
		}
	}
	else
	{
		if(IsEquipCharacterByArtefact(pchar, "talisman11"))
		{
			CrewQty = GetCrewQuantity(refTown);						
			refTown.AdditionalCrew = makeint(CrewQty * 0.5);			
			CrewQty += sti(refTown.AdditionalCrew);
			SetCrewQuantity(refTown, CrewQty);
		}
	}
	LAi_SetActorTypeNoGroup(Pchar);
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_FOOD_INFO_EXIT );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_FOOD_INFO_EXIT );
}

void IDoExit(int exitCode)
{
    EndAboveForm(true);
	LAi_SetPlayerType(pchar);
	RecalculateCargoLoad(refCharacter);
    GetBackupQty(); // если неожиданно вышли
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("frame","ProcessFrame");

	DelEventHandler("TransactionOK", "TransactionOK");
	DelEventHandler("TransactionCancel", "TransactionCancel");
	DelEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT");
	DelEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON");
	DelEventHandler("ADD_BUTTON","ADD_BUTTON");
	DelEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON");
	DelEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON");

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
		case "QTY_ADD_BUTTON":
			if(comName=="rclick")
			{
				ADD_MIN_BUTTON();
			}
		break;
		case "QTY_ADD_ALL_BUTTON":
			if(comName=="rclick")
			{
				ADD_OPT_BUTTON();
			}
		break;

		case "HIRE_ALL_BUTTON":
			if(comName=="click" || comName=="activate")
			{
				HireAllPreview(false); // ЛКМ — превью найма на МАКСИМУМ (коммит по ОК)
			}
			if(comName=="rclick")
			{
				HireAllPreview(true);  // ПКМ — превью ОПТИМАЛЬНОГО экипажа (коммит по ОК)
			}
		break;
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void FillShipsScroll()
{
	nCurScrollNum = -1;
	FillScrollImageWithCompanionShips("SHIPS_SCROLL", COMPANION_MAX);

	if(!CheckAttribute(&GameInterface,"SHIPS_SCROLL.current"))
	{
		GameInterface.SHIPS_SCROLL.current = 0;
	}
}

void SetVariable()
{
	string sText, sTextSecond;
	int iColor;
	int nShipType = sti(refCharacter.ship.type);

	if (nShipType == SHIP_NOTUSED)
	{
        GameInterface.strings.shipname = "";
		return;
	}

	QtyMax = GetCargoFreeSpace(refCharacter);
	ref refBaseShip = GetRealShip(nShipType);

    SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\portraits\face_" + its(refCharacter.FaceId) + ".tga");

	SetFormatedText("CAPACITY", GetFullName(refCharacter));

	sText = XI_ConvertString("OurMoney") + NewStr() + FindRussianMoneyString(sti(pchar.money));
	SetFormatedText("OUR_GOLD", sText);

	if (CheckAttribute(refCharacter, "ship.name"))
	{
		GameInterface.strings.shipname = refCharacter.ship.name;
	}
	else
	{
	    GameInterface.strings.shipname = "";
	}
	SetCrewExpTable(refCharacter, "TABLE_CREW", "BAR_Sailors", "BAR_Cannoners", "BAR_Soldiers");

	SetFormatedText("CREW_QTY", ""+GetCrewQuantity(refCharacter));
	if (GetCrewQuantity(refCharacter) > GetOptCrewQuantity(refCharacter) || GetCrewQuantity(refCharacter) < GetMinCrewQuantity(refCharacter))
	{
		iColor = argb(255,255,64,64);
	}
	else
	{
		iColor = argb(255,255,255,255);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CREW_QTY", 8,-1,iColor);
	SetNewGroupPicture("CREW_MORALE_PIC", "MORALE_SMALL", GetMoraleGroupPicture(stf(refCharacter.ship.crew.morale)));
	SetFormatedText("CREW_MORALE_TEXT", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(sti(refCharacter.Ship.crew.morale))));

	// на одном корабле
	SetFoodShipInfo(refCharacter, "FOOD_SHIP");

	SetFormatedText("INFO_SHIP", XI_ConvertString(refBaseShip.BaseName) + GetConvertStrWithReplace("Variable_hirecrew_1", "Interface.txt", "#space#", " ") + refBaseShip.Class +GetConvertStrWithReplace("Variable_hirecrew_2", "Interface.txt", "#space#", " ")+GetMinCrewQuantity(refCharacter) + GetConvertStrWithReplace("Variable_hirecrew_3", "Interface.txt", "#space#", " ") + GetOptCrewQuantity(refCharacter));
	SetFormatedText("MONEY_SHIP", GetConvertStrWithReplace("Variable_hirecrew_4", "Interface.txt", "#space#", " ") + NewStr() + FindRussianMoneyString(GetSalaryForShip(refCharacter)));
	////  заполнялка города
	SetCrewExpTable(refTown, "TABLE_CREW2", "BAR_Sailors2", "BAR_Cannoners2", "BAR_Soldiers2");

	SetFormatedText("CREW_QTY2", ""+GetCrewQuantity(refTown));
	SetNewGroupPicture("CREW_MORALE_PIC2", "MORALE_SMALL", GetMoraleGroupPicture(stf(refTown.ship.crew.morale)));
	SetFormatedText("CREW_MORALE_TEXT2", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(sti(refTown.Ship.crew.morale))));
	iPriceSailor = GetCrewPriceForTavern(refTown.id);
	if(IsEquipCharacterByArtefact(pchar, "talisman11")) iPriceSailor = makeint(iPriceSailor/2);
	SetFormatedText("TAVERN_PRICE", GetConvertStrWithReplace("Variable_hirecrew_5", "Interface.txt", "#space#", " ") + FindRussianMoneyString(iPriceSailor));
	
	GameInterface.StatusLine.BAR_CrewMorale.Max   = 100;
	GameInterface.StatusLine.BAR_CrewMorale.Min   = 0;
	GameInterface.StatusLine.BAR_CrewMorale.Value = sti(refCharacter.Ship.crew.morale);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_CrewMorale", 0);
	GameInterface.StatusLine.BAR_CrewMorale2.Max   = 100;
	GameInterface.StatusLine.BAR_CrewMorale2.Min   = 0;
	GameInterface.StatusLine.BAR_CrewMorale2.Value = sti(refTown.Ship.crew.morale);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_CrewMorale2", 0);
}

void ProcessFrame()
{
	if(GetCurrentNode() == "SHIPS_SCROLL")
	{
		if(sti(GameInterface.SHIPS_SCROLL.current)!=nCurScrollNum)
		{
			GetBackupQty(); // если неожиданно вышли
			bHireAllPending = false; // LEO: смена корабля сбрасывает превью найма на все
			nCurScrollNum = sti(GameInterface.SHIPS_SCROLL.current);
			SetDescription();
			GameInterface.qty_edit.str = 0;
			SetFormatedText("QTY_TypeOperation", "");
		    SetFormatedText("QTY_Result", "");
		}
	}
}

void SetDescription()
{
 	string sChrId;

	if(GetCurrentNode() == "SHIPS_SCROLL")
	{
		string attributeName = attributeName = "pic" + (nCurScrollNum+1);
		if(CheckAttribute(&GameInterface, "SHIPS_SCROLL." + attributeName))
		{
			int iCharacter = GameInterface.SHIPS_SCROLL.(attributeName).companionIndex;
			sChrId = characters[iCharacter].id;
			refCharacter = characterFromID(sChrId);
			QtyMax = GetCargoFreeSpace(refCharacter);
			SetBackupQty();
			if (GetRemovable(refCharacter))
			{
			    SetSelectable("QTY_OK_BUTTON", true);
			}
			else
			{
			    SetSelectable("QTY_OK_BUTTON", false);
			}
		}
	}
	SetVariable();
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;
	int iItem;

	//Boyer fix #20170401-01 Can't load texture -1.tx log errors
	sPicture = "";
	string sAttributeName;
	int nChooseNum = -1;
	int iShip;
	ref refBaseShip;

	bool  bShowHint = true;
	switch (sCurrentNode)
	{
		case "TABLE_CREW":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
		break;
		case "TABLE_CREW2":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
		break;
	}
	if (bShowHint)
	{
		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
	}
}

void HideInfoWindow()
{
	CloseTooltip();
}
// бакап значений, до применения
void SetBackupQty()
{
	aref    arTo, arFrom;
	NullCharacter.TavernBak.Hero = "";
	NullCharacter.TavernBak.Tavern = "";

	makearef(arTo,   NullCharacter.TavernBak.Hero);
	makearef(arFrom, refCharacter.Ship.Crew);
	CopyAttributes(arTo, arFrom);

	makearef(arTo,   NullCharacter.TavernBak.Tavern);
	makearef(arFrom, refTown.Ship.Crew);
	CopyAttributes(arTo, arFrom);
}

void GetBackupQty()
{
	aref    arTo, arFrom;

	makearef(arFrom,   NullCharacter.TavernBak.Hero);
	makearef(arTo, refCharacter.Ship.Crew);
	CopyAttributes(arTo, arFrom);

	makearef(arFrom,   NullCharacter.TavernBak.Tavern);
	makearef(arTo, refTown.Ship.Crew);
	CopyAttributes(arTo, arFrom);
}

void TransactionCancel()
{
	if (bHireAllPending) // LEO: отмена превью найма на все — сброс без выхода из интерфейса
	{
		bHireAllPending = false;
		GameInterface.qty_edit.str = 0;
		SetFormatedText("QTY_TypeOperation", "");
		SetFormatedText("QTY_Result", "");
		SetVariable();
		return;
	}
	if (sti(GameInterface.qty_edit.str) == 0)
	{   // выход
		ProcessCancelExit();
	}
	else
	{
		CancelQty();
    }
}

void CancelQty()
{
	GetBackupQty();
	SetVariable();
	GameInterface.qty_edit.str = 0;
	SetFormatedText("QTY_TypeOperation", "");
    SetFormatedText("QTY_Result", "");
}
void TransactionOK()
{
	if (bHireAllPending) { HireAllCommit(); return; } // LEO: подтверждение найма на все корабли
	int nTradeQuantity, moneyback;
	confirmChangeQTY_EDIT();
	nTradeQuantity = sti(GameInterface.qty_edit.str);
	if (BuyOrSell == 0)
	{
	    CancelQty();
		return;
	}

    if (!GetRemovable(refCharacter)) return;

 	if (BuyOrSell == 1) // BUY  нанять
	{
		moneyback = makeint(iPriceSailor*stf(GameInterface.qty_edit.str));
		AddMoneyToCharacter(pchar, -moneyback); // LEO: списание через AddMoneyToCharacter -> стандартный лог денег ГГ (красным «-X золотых» + звук + стат Money_spend)
		RecalculateCargoLoad(refCharacter);
		QtyMax = GetCargoFreeSpace(refCharacter);
	}
 	else
	{ // SELL
	}
	SetBackupQty(); // применим и согласимся
	CancelQty();
}

void confirmChangeQTY_EDIT()
{
	ChangeQTY_EDIT();
    SetCurrentNode("QTY_OK_BUTTON");
}

void ChangeQTY_EDIT()
{
	float fQty;
	bHireAllPending = false; // LEO: одиночная операция отменяет превью найма на все
	GameInterface.qty_edit.str = sti(GameInterface.qty_edit.str); // приведение к целому
	GetBackupQty();	 // обновим как было до правок
	if (sti(GameInterface.qty_edit.str) == 0)
	{
	    SetFormatedText("QTY_TypeOperation", "");
	    SetFormatedText("QTY_Result", "");
	    BuyOrSell = 0;
	}
	else
	{
		if (sti(GameInterface.qty_edit.str) < 0 || BuyOrSell == -1)
		{  // уволить
			if (BuyOrSell != -1)
			{
		    	GameInterface.qty_edit.str = -sti(GameInterface.qty_edit.str);
		    }
            BuyOrSell = -1;
		    // проверка на колво доступное -->
		    if (sti(GameInterface.qty_edit.str) > GetCrewQuantity(refCharacter))
		    {
		        GameInterface.qty_edit.str = GetCrewQuantity(refCharacter);
		    }
		    // проверка на колво доступное <--
		    SetFormatedText("QTY_TypeOperation", GetConvertStrWithReplace("Variable_hirecrew_6", "Interface.txt", "#space#", " "));
		    SetFormatedText("QTY_Result", "");
		}
		else
		{  // нанять
			BuyOrSell = 1;
         	// проверка на колво доступное -->
		    if (sti(GameInterface.qty_edit.str) > GetCrewQuantity(refTown))
		    {
		        GameInterface.qty_edit.str = GetCrewQuantity(refTown);
		    }
		    if (sti(GameInterface.qty_edit.str) > GetFreeCrewQuantity(refCharacter))
		    {
		        GameInterface.qty_edit.str = GetFreeCrewQuantity(refCharacter);
		    }

		    if (makeint(iPriceSailor*stf(GameInterface.qty_edit.str)) > sti(pchar.money))
		    {
		        GameInterface.qty_edit.str = makeint(sti(pchar.money) / iPriceSailor);
		    }
			if (bSailorsWeight)
			{
				QtyMax = GetCargoFreeSpace(refCharacter);
				if(QtyMax > 0)
				{
					if(sti(GameInterface.qty_edit.str) >= QtyMax)
					{
						GameInterface.qty_edit.str = QtyMax;
					}
				}
				else GameInterface.qty_edit.str = 0;
			}
		    // проверка на колво доступное <--

			SetFormatedText("QTY_TypeOperation", GetConvertStrWithReplace("Variable_hirecrew_7", "Interface.txt", "#space#", " "));
			SetFormatedText("QTY_Result", GetConvertStrWithReplace("Variable_hirecrew_8", "Interface.txt", "#space#", " ") + makeint(iPriceSailor*stf(GameInterface.qty_edit.str)));
		}
		// если получили ноль
		if (sti(GameInterface.qty_edit.str) == 0)
		{
		    SetFormatedText("QTY_TypeOperation", "");
		    SetFormatedText("QTY_Result", "");
		    BuyOrSell = 0;
		}
	}
	if (sti(GameInterface.qty_edit.str) > 0)
	{ // применение кол-ва

		if (BuyOrSell == 1)
		{   // найм меняет опыт и мораль корабля
			fQty = stf(GetCrewQuantity(refCharacter) + sti(GameInterface.qty_edit.str));
			refCharacter.Ship.Crew.Exp.Sailors   = (stf(refCharacter.Ship.Crew.Exp.Sailors)*GetCrewQuantity(refCharacter) +
			                                        stf(refTown.Ship.Crew.Exp.Sailors)*sti(GameInterface.qty_edit.str)) / fQty;
			refCharacter.Ship.Crew.Exp.Cannoners   = (stf(refCharacter.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refCharacter) +
			                                        stf(refTown.Ship.Crew.Exp.Cannoners)*sti(GameInterface.qty_edit.str)) / fQty;
			refCharacter.Ship.Crew.Exp.Soldiers   = (stf(refCharacter.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(refCharacter) +
			                                        stf(refTown.Ship.Crew.Exp.Soldiers)*sti(GameInterface.qty_edit.str)) / fQty;
			refCharacter.Ship.Crew.Morale   = (stf(refCharacter.Ship.Crew.Morale)*GetCrewQuantity(refCharacter) +
			                                        stf(refTown.Ship.Crew.Morale)*sti(GameInterface.qty_edit.str)) / fQty;
		}
		else
		{ // увольнение меняет таверну
			fQty = stf(GetCrewQuantity(refTown) + sti(GameInterface.qty_edit.str));
			refTown.Ship.Crew.Exp.Sailors   = (stf(refTown.Ship.Crew.Exp.Sailors)*GetCrewQuantity(refTown) +
			                                        stf(refCharacter.Ship.Crew.Exp.Sailors)*sti(GameInterface.qty_edit.str)) / fQty;
			refTown.Ship.Crew.Exp.Cannoners   = (stf(refTown.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refTown) +
			                                        stf(refCharacter.Ship.Crew.Exp.Cannoners)*sti(GameInterface.qty_edit.str)) / fQty;
			refTown.Ship.Crew.Exp.Soldiers   = (stf(refTown.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(refTown) +
			                                        stf(refCharacter.Ship.Crew.Exp.Soldiers)*sti(GameInterface.qty_edit.str)) / fQty;
			refTown.Ship.Crew.Morale   = (stf(refTown.Ship.Crew.Morale)*GetCrewQuantity(refTown) +
			                                        stf(refCharacter.Ship.Crew.Morale)*sti(GameInterface.qty_edit.str)) / fQty;
		}
		SetCrewQuantity(refCharacter, GetCrewQuantity(refCharacter) + BuyOrSell*sti(GameInterface.qty_edit.str));
		refTown.Ship.Crew.Quantity = sti(refTown.Ship.Crew.Quantity) - BuyOrSell*sti(GameInterface.qty_edit.str));
	}
    SetVariable(); // обновим экран
}

void ADD_MIN_BUTTON()  // купить все
{
	GetBackupQty();	// вернем все как было
	if (!GetRemovable(refCharacter)) return;
	if (sti(GameInterface.qty_edit.str) <= GetMinCrewQuantity(refCharacter)) GameInterface.qty_edit.str = GetMinCrewQuantity(refCharacter) - GetCrewQuantity(refCharacter);
	else GameInterface.qty_edit.str = 0;
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void ADD_OPT_BUTTON()  // купить все
{
	GetBackupQty();	// вернем все как было
	if (!GetRemovable(refCharacter)) return;
	if (sti(GameInterface.qty_edit.str) <= GetOptCrewQuantity(refCharacter)) GameInterface.qty_edit.str = GetOptCrewQuantity(refCharacter) - GetCrewQuantity(refCharacter);
	else GameInterface.qty_edit.str = 0;
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void REMOVE_ALL_BUTTON()  // продать все (уволить)
{
	GetBackupQty();	// вернем все как было
	if (!GetRemovable(refCharacter)) return;
	GameInterface.qty_edit.str = -GetCrewQuantity(refCharacter);
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void ADD_ALL_BUTTON()  // купить все
{
	GetBackupQty();	// вернем все как было
	if (!GetRemovable(refCharacter)) return;
	GameInterface.qty_edit.str = GetCrewQuantity(refTown);
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

// LEO: === «Нанять на все корабли»: превью (показ суммы) -> ОК коммитит -> скролл/Отмена/одиночка сбрасывают ===
// LEO: HireAllPreview — считает суммарную стоимость/число матросов и показывает, НИЧЕГО не меняя.
void HireAllPreview(bool bOptimal)
{
	GetBackupQty(); // сбросить незакоммиченный одиночный превью
	GameInterface.qty_edit.str = 0;
	BuyOrSell = 0;

	// LEO: пересчитать цену матроса явно (страховка от iPriceSailor=0, если SetVariable вышел раньше -> найм был бы «бесплатным»)
	iPriceSailor = GetCrewPriceForTavern(refTown.id);
	if (IsEquipCharacterByArtefact(pchar, "talisman11")) iPriceSailor = makeint(iPriceSailor/2);

	ref chrP;
	int cnP;
	int iAvailTavern = GetCrewQuantity(refTown);
	int iAvailMoney  = sti(pchar.money);
	int iTotalHire = 0;
	int iTotalCost = 0;

	for (int i = 0; i < COMPANION_MAX; i++)
	{
		cnP = GetCompanionIndex(pchar, i);
		if (cnP <= 0) continue;
		chrP = GetCharacter(cnP);
		if (!GetRemovable(chrP) && !IsMainCharacter(chrP)) continue; // флагман берём всегда, квест-корабли (NonRemovable) пропускаем
		if (sti(chrP.ship.type) == SHIP_NOTUSED) continue;

		int iTgt;
		if (bOptimal) iTgt = GetOptCrewQuantity(chrP);
		else          iTgt = GetMaxCrewQuantity(chrP);

		int iN = iTgt - GetCrewQuantity(chrP);
		if(iN > GetFreeCrewQuantity(chrP)) iN = GetFreeCrewQuantity(chrP); // LEO: не больше свободных слотов (раненые занимают места)
		if (iN <= 0) continue;
		if (iN > iAvailTavern) iN = iAvailTavern;
		if (iPriceSailor > 0 && iN * iPriceSailor > iAvailMoney) iN = makeint(iAvailMoney / iPriceSailor);
		if (bSailorsWeight)
		{
			int iFr = GetCargoFreeSpace(chrP);
			if (iN > iFr) iN = iFr;
		}
		if (iN <= 0) continue;

		iTotalHire   += iN;
		iTotalCost   += iN * iPriceSailor;
		iAvailTavern -= iN;
		iAvailMoney  -= iN * iPriceSailor;
	}

	bHireAllPending = (iTotalHire > 0);
	bHireAllOptimal = bOptimal;

	if (iTotalHire > 0)
	{
		SetFormatedText("QTY_TypeOperation", GetConvertStrWithReplace("Variable_hirecrew_7", "Interface.txt", "#space#", " ") + " " + iTotalHire);
		SetFormatedText("QTY_Result", GetConvertStrWithReplace("Variable_hirecrew_8", "Interface.txt", "#space#", " ") + iTotalCost);
		SetCurrentNode("QTY_OK_BUTTON");
	}
	else
	{
		SetFormatedText("QTY_TypeOperation", "");
		SetFormatedText("QTY_Result", "");
	}
}

// LEO: HireAllCommit — реально нанимает и списывает деньги (по кнопке ОК). Режим — из bHireAllOptimal.
void HireAllCommit()
{
	bHireAllPending = false;
	iPriceSailor = GetCrewPriceForTavern(refTown.id); // LEO: пересчёт цены (страховка от бесплатного найма)
	if (IsEquipCharacterByArtefact(pchar, "talisman11")) iPriceSailor = makeint(iPriceSailor/2);
	GetBackupQty(); // no-op при коммите (состояние чистое), но безвредно — единый путь с одиночным наймом
	ref chr;
	int cn;
	int iTavern = GetCrewQuantity(refTown);
	int iMoney  = sti(pchar.money);
	int iSpentTotal = 0;

	for (int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if (cn <= 0) continue;
		chr = GetCharacter(cn);
		if (!GetRemovable(chr) && !IsMainCharacter(chr)) continue; // флагман берём всегда, квест-корабли (NonRemovable) пропускаем
		if (sti(chr.ship.type) == SHIP_NOTUSED) continue;

		int iTarget;
		if (bHireAllOptimal) iTarget = GetOptCrewQuantity(chr);
		else          iTarget = GetMaxCrewQuantity(chr);

		int iNeed = iTarget - GetCrewQuantity(chr);
		if(iNeed > GetFreeCrewQuantity(chr)) iNeed = GetFreeCrewQuantity(chr); // LEO: не больше свободных слотов (раненые занимают места)
		if (iNeed <= 0) continue;
		if (iNeed > iTavern) iNeed = iTavern;
		if (iPriceSailor > 0 && iNeed * iPriceSailor > iMoney) iNeed = makeint(iMoney / iPriceSailor);
		if (bSailorsWeight)
		{
			int iFree = GetCargoFreeSpace(chr);
			if (iNeed > iFree) iNeed = iFree;
		}
		if (iNeed <= 0) continue;

		// найм с пересчётом опыта/морали корабля (как в ChangeQTY_EDIT)
		float fQty = stf(GetCrewQuantity(chr) + iNeed);
		chr.Ship.Crew.Exp.Sailors   = (stf(chr.Ship.Crew.Exp.Sailors)  *GetCrewQuantity(chr) + stf(refTown.Ship.Crew.Exp.Sailors)  *iNeed) / fQty;
		chr.Ship.Crew.Exp.Cannoners = (stf(chr.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(chr) + stf(refTown.Ship.Crew.Exp.Cannoners)*iNeed) / fQty;
		chr.Ship.Crew.Exp.Soldiers  = (stf(chr.Ship.Crew.Exp.Soldiers) *GetCrewQuantity(chr) + stf(refTown.Ship.Crew.Exp.Soldiers) *iNeed) / fQty;
		chr.Ship.Crew.Morale        = (stf(chr.Ship.Crew.Morale)       *GetCrewQuantity(chr) + stf(refTown.Ship.Crew.Morale)       *iNeed) / fQty;

		SetCrewQuantity(chr, GetCrewQuantity(chr) + iNeed);
		RecalculateCargoLoad(chr); // LEO: пересчёт груза этого корабля (иначе при весе матросов bSailorsWeight свободное место устаревает)
		refTown.Ship.Crew.Quantity = sti(refTown.Ship.Crew.Quantity) - iNeed;
		iSpentTotal += iNeed * iPriceSailor; // LEO: копим сумму, спишем разом в конце -> одна лог-строка «-X золотых»

		iTavern -= iNeed;
		iMoney  -= iNeed * iPriceSailor;

		if (iTavern <= 0) break;
		if (iPriceSailor > 0 && iMoney < iPriceSailor) break;
	}

	if (iSpentTotal > 0) AddMoneyToCharacter(pchar, -iSpentTotal); // LEO: списание всей суммы разом + стандартный красный лог денег ГГ «-X золотых»
	RecalculateCargoLoad(refCharacter);
	SetBackupQty(); // зафиксировать новое состояние как базовое (иначе выход/отмена откатят)
	GameInterface.qty_edit.str = 0;
	BuyOrSell = 0;
	SetFormatedText("QTY_TypeOperation", "");
	SetFormatedText("QTY_Result", "");
	SetVariable(); // обновить экран
}

void REMOVE_BUTTON()  // продать
{
	if (!GetRemovable(refCharacter)) return;
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
	if (!GetRemovable(refCharacter)) return;
	if (BuyOrSell == 0)
    {
        GameInterface.qty_edit.str = 1;
    }
    else
    {
  		if (BuyOrSell == 1)
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
