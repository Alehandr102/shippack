/// BOAL меню корабль
int nCurScrollNum;
ref xi_refCharacter;
int shipIndex;
bool bPartShow = true;
// LEO: панель команды показывает ЗДОРОВЫХ (false) или РАНЕНЫХ (true), стрелки под баром морали.
// Флаг на весь экран, а не на корабль: листая эскадру, удобно сравнивать раненых по кораблям.
bool bShipWoundedView = false;

string CurTable, CurRow;
int iSelected; // курсор в таблице

// для выкидывания
int iShipQty, iUnits, iCurGoodsIdx;

string sMessageMode;
void InitInterface_R(string iniName, ref _chr) // _chr нужно для читового просмотра НПС в море
{
    InterfaceStack.SelectMenu_node = "LaunchShipState"; // запоминаем, что звать по Ф2
	GameInterface.title = "titleShip";

	xi_refCharacter = _chr;
	FillShipsScroll();
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
    SetEventHandler("ievnt_command","ProcessCommandExecute",0);
    SetEventHandler("frame","ProcessFrame",1);
    SetEventHandler("confirmShipChangeName","confirmShipChangeName",0);
	SetEventHandler("CheckForRename","CheckForRename",0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ShowItemInfo", "ShowItemInfo", 0);
	SetEventHandler("GoodsExitCancel", "GoodsExitCancel", 0);
	SetEventHandler("TransactionOK", "TransactionOK", 0);
	SetEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT", 0);
	SetEventHandler("ShipChange", "ShipChange", 0);
	SetEventHandler("ExitShipChangeMenu", "ExitShipChangeMenu", 0);
	SetEventHandler("ShowOtherClick", "ShowOtherClick", 0);
	SetEventHandler("GoToShipChange", "GoToShipChange", 0);
	SetEventHandler("ExitCannonsMenu", "ExitCannonsMenu", 0);
	SetEventHandler("HideUpgradesInfo", "HideUpgradesInfo", 0);

	SetEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON", 0);
	SetEventHandler("ADD_BUTTON","ADD_BUTTON",0);
	SetEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON", 0);
	SetEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON", 0);
	SetEventHandler("ExitPartitionWindow", "ExitPartitionWindow", 0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
    XI_RegistryExitKey("IExit_F2");
    // LEO: в режиме чеккера NPC прячем лишние вкладки меню + ВСЕ тексты меню (оставляем только иконки Персонаж/Корабль)
    if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx"))
    {
        SetNodeUsing("I_QUESTBOOK", false);  SetNodeUsing("I_QUESTBOOK_2", false);
        SetNodeUsing("I_TRADEBOOK", false);  SetNodeUsing("I_TRADEBOOK_2", false);
        SetNodeUsing("I_NATIONS", false);    SetNodeUsing("I_NATIONS_2", false);
        SetNodeUsing("I_ITEMS", false);      SetNodeUsing("I_ITEMS_2", false);
        SetNodeUsing("STR_BUTTONS", false);  // LEO: тексты меню одним узлом - прячем все (остаются иконки Персонаж/Корабль)
    }
    //////////////////
    EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",156,78,366,313); // tak from SHIP_BIG_PICTURE
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 161,284,361,1, 4);
    EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",20,85,134,199);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,225,147,1, 4);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,204,147,1, 4);

    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 657,214,789,1, 4);
    SetNewGroupPicture("CREW_PICTURE", "SHIP_STATE_ICONS", "Crew");

    SetNewGroupPicture("EXTRAHULLON", "SHIP_UPGRADES", "EXTRAHULLON");
    SetNewGroupPicture("EXTRASAILON", "SHIP_UPGRADES", "EXTRASAILON");
    SetNewGroupPicture("EXTRAGUNSON", "SHIP_UPGRADES", "EXTRAGUNSON");
    SetNewGroupPicture("EXTRAHULLKRENGON", "SHIP_UPGRADES", "EXTRAHULLKRENGON");
    SetNewGroupPicture("EXTRACAPACITYON", "SHIP_UPGRADES", "EXTRACAPACITYON");
    SetNewGroupPicture("EXTRABIGSIDESON", "SHIP_UPGRADES", "EXTRABIGSIDESON");

    SetFormatedText("Money_TEXT", MakeMoneyShow(sti(pchar.Money), MONEY_SIGN,MONEY_DELIVER));

    GameInterface.TABLE_LIST.hr.td1.str = GetConvertStrWithReplace("Variable_ship_1", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td1.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td2.str = GetConvertStrWithReplace("Variable_ship_2", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td2.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td3.str = GetConvertStrWithReplace("Variable_ship_3", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td3.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td4.str = GetConvertStrWithReplace("Variable_ship_4", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td4.scale = 0.9;
	GameInterface.TABLE_LIST.hr.td5.str = GetConvertStrWithReplace("Variable_ship_5", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_LIST.hr.td5.scale = 0.8;
	GameInterface.TABLE_LIST.select = 0;
//--> mod tablesort
	GameInterface.TABLE_LIST.hr.td1.sorttype = "string";
	GameInterface.TABLE_LIST.hr.td2.sorttype = "";
	GameInterface.TABLE_LIST.hr.td3.sorttype = "";
	GameInterface.TABLE_LIST.hr.td4.sorttype = "";
	GameInterface.TABLE_LIST.hr.td5.sorttype = "";
//<-- mod tablesort
	FillPartitionTables();

	SetCurrentNode("SHIPS_SCROLL");
	OnShipScrollChange();
	sMessageMode = "";
	SetFormatedText("SHIPS_WEIGHTS", GetShipsWeights());
	SetNodeUsing("Soil_TEXT", true);
	SetNodeUsing("SoilS_TEXT", true);
	SetNodeUsing("SoilM_TEXT", true);
}

string GetShipsWeights()
{
	int unno = 0;
	int twaa = 0;
	int tree = 0;
	int cn, iShipType;
	for(int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(xi_refCharacter, i);
		if(cn != -1)
		{
			iShipType = sti(characters[cn].ship.type);
			if(iShipType != SHIP_NOTUSED)
			{
				unno += makeint(GetCargoMaxSpace(characters[cn])-GetCargoLoad(characters[cn]));
				twaa += makeint(GetCargoLoad(characters[cn]));
				tree += makeint(GetCargoMaxSpace(characters[cn]));
			}
		}
	}
	return GetConvertStrWithReplace("Variable_ship_6", "Interface.txt", "#space#", " ") + unno + GetConvertStrWithReplace("Variable_ship_6.1","Interface.txt", "#space#", " ") + twaa + GetConvertStrWithReplace("Variable_ship_6.2","Interface.txt", "#space#", " ") + tree + GetConvertStrWithReplace("Variable_ship_7", "Interface.txt", "#space#", " ");
}

void ProcessExitCancel()
{
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
    DelEventHandler("ievnt_command","ProcessCommandExecute");
    DelEventHandler("frame","ProcessFrame");
    DelEventHandler("confirmShipChangeName","confirmShipChangeName");
	DelEventHandler("CheckForRename","CheckForRename");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ShowItemInfo", "ShowItemInfo");
	DelEventHandler("GoodsExitCancel", "GoodsExitCancel");
	DelEventHandler("TransactionOK", "TransactionOK");
	DelEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT");
	DelEventHandler("ShipChange", "ShipChange");
	DelEventHandler("ExitShipChangeMenu", "ExitShipChangeMenu");
	DelEventHandler("ShowOtherClick", "ShowOtherClick");
	DelEventHandler("GoToShipChange", "GoToShipChange");
	DelEventHandler("ExitCannonsMenu", "ExitCannonsMenu");
	DelEventHandler("HideUpgradesInfo", "HideUpgradesInfo");

	DelEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON");
	DelEventHandler("ADD_BUTTON","ADD_BUTTON");
	DelEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON");
	DelEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON");
	DelEventHandler("ExitPartitionWindow", "ExitPartitionWindow");
	DelEventHandler("OnTableClick", "OnTableClick");
	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"ReloadMenuExit"))
	{
        DeleteAttribute(&InterfaceStates,"ReloadMenuExit");
		EndCancelInterface(false);
	}
	else
	{
		DeleteAttribute(pchar, "SystemInfo.CheckNpcIdx"); // LEO: чеккер закрыт полностью - сброс проверяемого NPC
		EndCancelInterface(true);
	}
	//#20200329-01
	if(bSeaActive) RefreshBattleInterface();
}
void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	// LEO: read-only режим чеккера NPC - пропускаем только навигацию по меню (вкладки), любые действия блокируем
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx") && nodName != "I_CHARACTER_2" && nodName != "I_SHIP_2" && nodName != "I_CHARACTER" && nodName != "I_SHIP") return; // LEO: в чеккере пускаем только Персонаж/Корабль (иконки И клик-зоны)
    switch(nodName)
	{
		/////  пушки -->
        case "CANNONS_OK":
			if(comName=="click")
			{
				HideCannonsMenu(); // применить уже сущ изменения
				OnShipScrollChange();
				Ship_ResearchCannons(xi_refCharacter);
			}
		break;

		case "CANNONS_REMOVE_ALL":
			if(comName=="click")
			{
			    CanonsRemoveAll();
			}
		break;

		case "PARTITION_SHOW":
			if(comName=="click")
			{
			    PARTITION_SHOW();
			}
		break;

		case "CANNONS_SET_ALL":
			if(comName=="click")
			{
			    CanonsSetAll(xi_refCharacter);
			}
		break;

		case "CANNONS_LEFT_F":
			if(comName=="click")
			{
				ChangeCannonNum("fcannon", -1);
			}
		break;
		case "CANNONS_RIGHT_F":
			if(comName=="click")
			{
    			ChangeCannonNum("fcannon", 1);
			}
		break;

		case "CANNONS_LEFT_B":
			if(comName=="click")
			{
                ChangeCannonNum("bcannon", -1);
			}
		break;
		case "CANNONS_RIGHT_B":
			if(comName=="click")
			{
                ChangeCannonNum("bcannon", 1);
			}
		break;

		case "CANNONS_LEFT_R":
			if(comName=="click")
			{
                ChangeCannonNum("rcannon", -1);
			}
		break;
		case "CANNONS_RIGHT_R":
			if(comName=="click")
			{
                ChangeCannonNum("rcannon", 1);
			}
		break;

		case "CANNONS_LEFT_L":
			if(comName=="click")
			{
                ChangeCannonNum("lcannon", -1);
			}
		break;
		case "CANNONS_RIGHT_L":
			if(comName=="click")
			{
                ChangeCannonNum("lcannon", 1);
			}
		break;
		/////  пушки <--
		case "SHIPSUP_BUTTON_TEMP_GOODS":
			if(comName=="click")
			{
			    ShowUpgradesInfo();
			}
		break;
		
		case "SHIPSUP_EXIT_BTN":
			if(comName=="click")
			{
			    HideUpgradesInfo();
			}
		break;
		
		case "CREW_MORALE_BUTTON":
			if(comName=="click")
			{
			    ShowMoraleMessage();
			}
		break;

		// LEO: обе стрелки крутят один и тот же переключатель - режимов всего два
		case "CREW_MODE_LEFT":
			if(comName=="click" || comName=="activate")
			{
			    ToggleShipCrewView();
			}
		break;

		case "CREW_MODE_RIGHT":
			if(comName=="click" || comName=="activate")
			{
			    ToggleShipCrewView();
			}
		break;

		case "PARTITION_OK":
			if(comName=="click")
			{
			    DoPartitionPay();
			}
		break;

		case "CREW_PARTITION":
			if(comName=="click")
			{
			    ShowPartitionWindow();
			}
		break;

		case "RESORT":
			if(comName=="click")
			{
				SortCompanionsBy("speedrate");
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
		// мины 07.07.07
		case "QTY_BOMB_BUTTON":
			if(comName=="click")
			{
			    SetMineFree(xi_refCharacter, 1);
			    iShipQty = GetCargoGoods(xi_refCharacter, GOOD_POWDER);
			    OnShipScrollChange();
			}
		break;
		/////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////////////
void FillShipsScroll()
{
	nCurScrollNum = -1;
	if(!CheckAttribute(&GameInterface,"SHIPS_SCROLL.current"))
	{
		GameInterface.SHIPS_SCROLL.current = 0;
	}

	string attributeName;
	string shipName;
	string potc = "";
	int iShipType, cn;
	GameInterface.SHIPS_SCROLL.ImagesGroup.t0 = "BLANK_SHIP2";

	FillShipList("SHIPS_SCROLL.ImagesGroup", xi_refCharacter);
	GameInterface.SHIPS_SCROLL.BadTex1 = 0;
	GameInterface.SHIPS_SCROLL.BadPic1 = "Not Used2";
	if (InterfaceStates.InterfaceShipString == "interfaces\ships_PotC\") potc = "_POTC";

	int m = 0;
	for(int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(xi_refCharacter, i);
		if(cn != -1)
		{
			iShipType = sti(characters[cn].ship.type);
			if(iShipType != SHIP_NOTUSED)
			{
				attributeName = "pic" + (m+1);
				//Boyer change #20170430-03

				ref rBaseShip = GetRealShip(iShipType);
				iShipType = rBaseShip.basetype;
                //#20170430-03 Companion ships are appended with a "1"
				shipName = rBaseShip.BaseName;
				string shipClass = rBaseShip.Class;

				GameInterface.SHIPS_SCROLL.(attributeName).character = cn;
				//Boyer change #20170430-03
				GameInterface.SHIPS_SCROLL.(attributeName).str1 = "#"+GetConvertStrWithReplace("Variable_ship_8", "Interface.txt", "#space#", " ")+ShipsTypes[iShipType].Class;
				GameInterface.SHIPS_SCROLL.(attributeName).str2 = shipName;
				if (sti(rBaseShip.Type.Merchant)) GameInterface.SHIPS_SCROLL.(attributeName).str3 = "#"+"Í";
				if (sti(rBaseShip.Type.War)) GameInterface.SHIPS_SCROLL.(attributeName).str4 = "#"+"É";
				GameInterface.SHIPS_SCROLL.(attributeName).img1 = "ship";
				GameInterface.SHIPS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("SHIPS_SCROLL.ImagesGroup","SHIPS_"+shipName+potc);
				m++;
			}
			else
			{
				attributeName = "pic" + (m+1);
				GameInterface.SHIPS_SCROLL.(attributeName).character = cn;
				GameInterface.SHIPS_SCROLL.(attributeName).str1 = "#";
				GameInterface.SHIPS_SCROLL.(attributeName).str2 = "NoneBoat";
				GameInterface.SHIPS_SCROLL.(attributeName).img1 = "Not Used2";
				GameInterface.SHIPS_SCROLL.(attributeName).tex1 = "BLANK_SHIP2";
				m++;
			}
		}
	}

	GameInterface.SHIPS_SCROLL.ListSize = m;
	GameInterface.SHIPS_SCROLL.NotUsed = COMPANION_MAX - m + 1;
}

void ProcessFrame()
{
	string attributeName;
	int iCharacter;

	string sNode = GetCurrentNode();
	if(sNode == "SHIPS_SCROLL")
	{
		if(sti(GameInterface.SHIPS_SCROLL.current)!= nCurScrollNum)
		{
			DeleteAttribute(&GameInterface, "TABLE_LIST.BackUp");
			nCurScrollNum = sti(GameInterface.SHIPS_SCROLL.current);

			attributeName = "pic" + (nCurScrollNum+1);

			iCharacter = sti(GameInterface.SHIPS_SCROLL.(attributeName).character);

			if(iCharacter > 0)
			{
				string sChrId = characters[iCharacter].id;

				xi_refCharacter = characterFromID(sChrId);
				shipIndex= nCurScrollNum+ 1;
				if (shipIndex< 0)
				{
					shipIndex= 0;
				}
				if (sti(xi_refCharacter.ship.type) == SHIP_NOTUSED)
				{
					shipIndex= -1;
				}
				// boal оптимизация скилов -->
			    DelBakSkillAttr(xi_refCharacter);
			    RefreshCharacterSkillExpRate(xi_refCharacter);

			    SetEnergyToCharacter(xi_refCharacter);
			    // boal оптимизация скилов <--
				OnShipScrollChange();
			}
			else
			{
				shipIndex= -1;
			}
			return;
		}
	}
}

// LEO: подпись и стрелки переключателя «Здоровые <-> Раненые». bShow=false - слот пустой, корабля нет.
void SetShipCrewModeLabel(bool bShow)
{
	SetNodeUsing("CREW_MODE_LEFT", bShow);
	SetNodeUsing("CREW_MODE_RIGHT", bShow);
	SetNodeUsing("CREW_BACK6", bShow);
	if (!bShow)
	{
		SetFormatedText("CREW_MODE_TEXT", "");
		return;
	}
	if (bShipWoundedView) SetFormatedText("CREW_MODE_TEXT", XI_ConvertString("CrewView_wounded"));
	else                  SetFormatedText("CREW_MODE_TEXT", XI_ConvertString("CrewView_healthy"));
}

void ToggleShipCrewView()
{
	bShipWoundedView = !bShipWoundedView;
	OnShipScrollChange();
}

void OnShipScrollChange()
{
	SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\portraits\face_" + xi_refCharacter.FaceId + ".tga");
	SetFormatedText("HERO_NAME", GetFullName(xi_refCharacter));
	SetFormatedText("HERO_RANK",xi_refCharacter.Rank);
	// нулим все формы, тк корабля может не быть
	SetFormatedText("SHIP_RANK","");
	SetFormatedText("CREW_QTY","");
	SetFormatedText("FOOD", "");
	SetFormatedText("FOOD_SHIP", "");
	SetFormatedText("MONEY_SHIP", "");
	SetFormatedText("CREW_MORALE_TEXT", "");
	SetNewGroupPicture("CREW_MORALE_PIC", "MORALE_SMALL", GetMoraleGroupPicture(1));
	SetSelectable("CREW_MORALE_BUTTON", false);
	Table_Clear("TABLE_LIST", false, true, false);
	Table_Clear("TABLE_OTHER", false, true, false);
	Table_Clear("TABLE_CREW", false, true, false);
	//Ship info window
	SetFormatedText("Money_TEXT", MakeMoneyShow(sti(pchar.Money), MONEY_SIGN,MONEY_DELIVER));
	SetFormatedText("SQUADRON_POWER", GetConvertStrWithReplace("Variable_ship_72", "Interface.txt", "#space#", " ") + " - " + CalculateSquadronPowerPchar());
	int iShip = sti(xi_refCharacter.ship.type);

	SetNodeUsing("CREW_PARTITION", false);
	SetNodeUsing("RESORT", false);
	if (iShip != SHIP_NOTUSED)
	{
		// оптимизация скилов -->
	    DelBakSkillAttr(xi_refCharacter);
	    RefreshCharacterSkillExpRate(xi_refCharacter);
	    // оптимизация скилов <--

		if (CheckAttribute(xi_refCharacter, "Ship.Cargo.RecalculateCargoLoad") && sti(xi_refCharacter.Ship.Cargo.RecalculateCargoLoad))
		{
			RecalculateCargoLoad(xi_refCharacter);
			xi_refCharacter.Ship.Cargo.RecalculateCargoLoad = 0;
		}
		ref refShip = GetRealShip(iShip);
		string sShip = refShip.BaseName;
		SetNewPicture("SHIP_BIG_PICTURE", InterfaceStates.InterfaceShipString + sShip + ".dds");
		if (!CheckAttribute(refShip,"QuestShip")) SetNodeUsing("SHIP_BIG_PICTURE_VIDEO",false);
		else {SetNodeUsing("SHIP_BIG_PICTURE_VIDEO",true); SetNewVideoPicture("SHIP_BIG_PICTURE_VIDEO","SHIP_"+sShip);}

		GameInterface.edit_box.str = xi_refCharacter.ship.name;
		SetFormatedText("SHIP_RANK", refShip.Class);
		SetFormatedText("CLASS_ARMOR", refShip.HullArmor);
		if (!CheckAttribute(refShip,"Tuning.HullArmor")) SetNewGroupPicture("CLASS_ARMOR_ICON", "ICONS_SPEC", "Normal");
		else SetNewGroupPicture("CLASS_ARMOR_ICON", "ICONS_SPEC", "Upgraded");
		FillGoodsTable();

		SetShipOTHERTable("TABLE_OTHER", xi_refCharacter);
		SetShipQualityTable(xi_refCharacter, "BAR_HP", "BAR_Mast", "BAR_Speed", "BAR_TurnRate", "BAR_WAS", "BAR_Capacity", "BAR_Crew");
		// еда -->
		int iColor, iFood;
		string sText;
		// на одном корабле
		SetFoodShipInfo(xi_refCharacter, "FOOD_SHIP");
		// в эскадре
		if (GetCompanionQuantity(pchar) > 1) SetSquadronFoodShipInfo(xi_refCharacter, "FOOD_SHIP"); // больше 1 ГГ
		// еда <--
		SetRumShipInfo(xi_refCharacter,"FOOD");
		if (GetRemovable(xi_refCharacter)) // считаем только своих, а то вских сопровождаемых кормить!!!
		{
		    // для каждого корабля учитываем класс и считаем отдельно
		    SetFormatedText("MONEY_SHIP", GetConvertStrWithReplace("Variable_ship_9", "Interface.txt", "#space#", " ") + NewStr() + FindRussianMoneyString(GetSalaryForShip(xi_refCharacter)));
		}
		// LEO: у раненых нет ни оптимума, ни минимума - красная подсветка «мало/много» неуместна
		if (bShipWoundedView)
		{
			SetFormatedText("CREW_QTY", ""+GetWoundedQuantity(xi_refCharacter));
			iColor = argb(255,255,255,255);
		}
		else
		{
		SetFormatedText("CREW_QTY", ""+GetCrewQuantity(xi_refCharacter));
		if (GetCrewQuantity(xi_refCharacter) > GetOptCrewQuantity(xi_refCharacter) || GetCrewQuantity(xi_refCharacter) < GetMinCrewQuantity(xi_refCharacter))
		{
			iColor = argb(255,255,64,64);
		}
		else
		{
			iColor = argb(255,255,255,255);
		}
		}
		SetShipCrewModeLabel(true);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CREW_QTY", 8,-1,iColor);
		// LEO: в режиме раненых флаг, текст и бар морали - из БУФЕРА лазарета
		float fPanelMorale = stf(xi_refCharacter.ship.crew.morale);
		if (bShipWoundedView) fPanelMorale = GetWoundedMorale(xi_refCharacter);
		SetNewGroupPicture("CREW_MORALE_PIC", "MORALE_SMALL", GetMoraleGroupPicture(fPanelMorale));
		SetFormatedText("CREW_MORALE_TEXT", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(makeint(fPanelMorale))));
		// «Поднять мораль» - только по активной команде: платить за боевой дух лежачих незачем
		if (sti(xi_refCharacter.Ship.crew.morale) < MORALE_MAX && GetCrewQuantity(xi_refCharacter) > 0 && !bShipWoundedView)
		{
			SetSelectable("CREW_MORALE_BUTTON", true);
		}
		SetNodeUsing("BAR_Sailors", true);
		SetNodeUsing("BAR_Cannoners", true);
		SetNodeUsing("BAR_Soldiers", true);
		SetNodeUsing("BAR_CrewMoral", true);
		SetNodeUsing("BAR_ShipsWeights", true);
		SetCrewExpTableEx(xi_refCharacter, "TABLE_CREW", "BAR_Sailors", "BAR_Cannoners", "BAR_Soldiers", bShipWoundedView);
		//BAR_CrewMoral
		GameInterface.StatusLine.BAR_CrewMoral.Max   = 100;
		GameInterface.StatusLine.BAR_CrewMoral.Min   = 0;
		GameInterface.StatusLine.BAR_CrewMoral.Value = makeint(fPanelMorale);
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_CrewMoral", 0);
		int unno = 0;
		int twaa = 0;
		int tree = 0;
		int cn, iShipType;
		for(int i = 0; i < COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(pchar, i);
			if(cn != -1)
			{
				iShipType = sti(characters[cn].ship.type);
				if(iShipType != SHIP_NOTUSED)
				{
					twaa += makeint(GetCargoLoad(characters[cn]));
					tree += makeint(GetCargoMaxSpace(characters[cn]));
				}
			}
		}
		GameInterface.StatusLine.BAR_ShipsWeights.Max   = tree;
		GameInterface.StatusLine.BAR_ShipsWeights.Min   = 0;
		GameInterface.StatusLine.BAR_ShipsWeights.Value = twaa;
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_ShipsWeights", 0);
		if (xi_refCharacter.id == pchar.id)
		{
			SetNodeUsing("CREW_PARTITION", true);
			SetNodeUsing("RESORT", true);
    	}
		
		if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"UpgradePrepared")) SetNodeUsing("SHIPSUP_BUTTON_TEMP_GOODS",true);
		else SetNodeUsing("SHIPSUP_BUTTON_TEMP_GOODS",false);
		
		if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Upgrade")) SetUpgradesNodes(RealShips[sti(xi_refCharacter.Ship.Type)],RealShips[sti(xi_refCharacter.Ship.Type)],false);
		else DisableUpgradeNodes(RealShips[sti(xi_refCharacter.Ship.Type)],RealShips[sti(xi_refCharacter.Ship.Type)],false);
		float SoilS, SoilM;
		if( !CheckAttribute(xi_refCharacter, "ship.soiling")) { xi_refCharacter.ship.soiling = 0;}
		SoilS = 100*(1 - ShipSpeedBonusFromSoiling(xi_refCharacter));
		SoilM = 100*(1 - ShipTurnRateBonusFromSoiling(xi_refCharacter));
		string sSoil = xi_refCharacter.ship.soiling +"%";

		sText = " " + GetConvertStrWithReplace("Variable_ship_10", "Interface.txt", "#space#", " ") + sSoil;
		SetFormatedText("Soil_TEXT", sText);
		// LEO: было fts(...,3) - обрезка по 3 СИМВОЛАМ, у «30.0» выходило «30.» -> на экране «30.%»
		if (SoilS < 0.1) sSoil = "0%"; else sSoil = FloatToString(SoilS, 1) + "%";
		sText = "  " + GetConvertStrWithReplace("Variable_ship_11", "Interface.txt", "#space#", " ") + sSoil;
		SetFormatedText("SoilS_TEXT", sText);
		if (SoilM < 0.1) sSoil = "0%"; else sSoil = FloatToString(SoilM, 1) + "%";
		sText = " " + GetConvertStrWithReplace("Variable_ship_12", "Interface.txt", "#space#", " ") + sSoil;
		SetFormatedText("SoilM_TEXT", sText);
	}
	else
	{
		SetNewPicture("SHIP_BIG_PICTURE", "interfaces\blank_ship2.dds");
		GameInterface.edit_box.str = XI_Convertstring("NoneBoat");
		SetNodeUsing("BAR_Sailors", false);
		SetNodeUsing("BAR_Cannoners", false);
		SetNodeUsing("BAR_Soldiers", false);
		SetNodeUsing("BAR_CrewMoral", false);
		SetNodeUsing("SHIPSUP_BUTTON_TEMP_GOODS", false);
		SetShipCrewModeLabel(false); // LEO: корабля в слоте нет - прятать и переключатель
	}
	Table_UpdateWindow("TABLE_LIST");
	Table_UpdateWindow("TABLE_OTHER");
	Table_UpdateWindow("TABLE_CREW");
}

void confirmShipChangeName()
{
	if (shipIndex== -1) return;
	xi_refcharacter.ship.name = GameInterface.edit_box.str;
	GameInterface.edit_box.str = xi_refcharacter.ship.name;
	if(GameInterface.EDIT_BOX.lastkey != " ") SetCurrentNode("SHIPS_SCROLL");
}

void CheckForRename()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер - не переименовывать корабль NPC
	if (GetShipRemovable(xi_refCharacter) == true && shipIndex!= -1)
	{
		SetCurrentNode("EDIT_BOX");
	}
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sText4, sPicture;
	string sGroup, sGroupPicture;
	int iItem;
	int xx = 96;
	int yy = 96;

	//Boyer fix #20170401-01 Can't load texture -1.tx log errors
	sPicture = "";
	string sAttributeName;
	int nChooseNum = -1;
	int iShip = sti(xi_refCharacter.ship.type);
	ref refBaseShip = GetRealShip(iShip);

	bool  bShowHint = true;
	if (HasSubStr(sCurrentNode,"SHIPSUP_")) {SetUpgradeTooltip(sCurrentNode); return;}
	switch (sCurrentNode)
	{
		case "SHIP_BIG_PICTURE":
			if (shipIndex != -1)
			{
				sHeader = XI_ConvertString(refBaseShip.BaseName);
				sText1 = GetConvertStr(refBaseShip.BaseName, "ShipsDescribe.txt");

				//--> показ каюты по ПКМ на портрете корабля
				int nLoc = FindLocation(refBaseShip.CabinType);
				if (nLoc >= 0 && CheckAttribute(&Locations[nLoc],"image")) 
				{
					sPicture = Locations[nLoc].image;
					xx = 512;//размер картинки
					yy = 288;
				}
				//<-- показ каюты
			}
			else
			{
				sHeader = XI_Convertstring("NoneBoat");
				sText1  = GetConvertStr("NoneBoat", "ShipsDescribe.txt");
			}
		break;

		case "SHIPS_SCROLL":
			if (shipIndex != -1)
			{
				sHeader = XI_ConvertString(refBaseShip.BaseName);
				sText1 = GetConvertStr(refBaseShip.BaseName, "ShipsDescribe.txt");
				sText2 = GetRPGText("shipChoose_hint");
			}
			else
			{
				sHeader = XI_Convertstring("NoneBoat");
				sText1  = GetConvertStr("NoneBoat", "ShipsDescribe.txt");
			}
		break;

		case "MAIN_CHARACTER_PICTURE":
			// отдельная форма
			bShowHint = false;
			ShowRPGHint();
		break;

		case "TABLE_LIST":
			sHeader = XI_Convertstring("Goods");
			sText1  = GetRPGText("GoodsCargo_hint");
		break;

		case "SQUADRON_POWER":
			sHeader = XI_Convertstring("SQUADRON_POWER");
			sText1  = XI_ConvertString("SQUADRON_POWER_descr");
		break;

		case "TABLE_CREW":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
			sText3 = GameInterface.TABLE_OTHER.tr8.UserData.ID2;
		break;

		case "CLASS_ARMOR_ICON":
			sHeader = GetConvertStr("Class_Armor", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Class_Armor_hint", "ShipsDescribe.txt");
		break;

		case "TABLE_OTHER":
			sHeader = XI_ConvertString(GameInterface.(CurTable).(CurRow).UserData.ID);
		    sText1  = GetConvertStr(GameInterface.(CurTable).(CurRow).UserData.ID, "ShipsDescribe.txt");
		    if (sti(xi_refCharacter.ship.type) != SHIP_NOTUSED) {
				sText2 = GameInterface.(CurTable).(CurRow).UserData.ID1;
				sText3 = GameInterface.(CurTable).(CurRow).UserData.ID2;
				if (GameInterface.(CurTable).(CurRow).UserData.ID == "Hull") {
					sText4 = AddTextAboutHull(xi_refCharacter);
				}
			}
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType" && sti(xi_refCharacter.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
			{
				sGroup = "GOODS";
				sGroupPicture = GetCannonType(sti(xi_refCharacter.Ship.Cannons.Type)) + "_" + GetCannonCaliber(sti(xi_refCharacter.Ship.Cannons.Type));
				xx = 128;
				yy = 128;
			}
		break;
		case "CREW_PARTITION":
			sHeader = XI_Convertstring("PartitionCaption");
			sText1  = GetRPGText("Partition_hint");
		break;
		case "RESORT":
			sHeader = XI_Convertstring("RESORTCaption");
			sText1  = GetRPGText("Resort_hint");
		break;
		case "Soil_TEXT":
			sHeader = XI_ConvertString("Soil_Tooltip");
			sText1 = XI_ConvertString("Soil_descr_Tooltip");
		break;
		case "SoilS_TEXT":
			sHeader = XI_ConvertString("SoilS_Tooltip");
			sText1 = XI_ConvertString("SoilS_descr_Tooltip");
		break;
		case "SoilM_TEXT":
			sHeader = XI_ConvertString("SoilM_Tooltip");
			sText1 = XI_ConvertString("SoilM_descr_Tooltip");
		break;
	}
	if (bShowHint)
	{
		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), sText4, argb(255,255,255,255), sPicture, sGroup, sGroupPicture, xx, yy);
	}
}

void HideInfoWindow()
{
	CloseTooltip();
	ExitRPGHint();
}

void TableSelectChange()
{
	string sControl = GetEventData();
	iSelected = GetEventData();
    CurTable = sControl;
    CurRow   =  "tr" + (iSelected);
	if (sControl == "PARTITION_TABLE_MAIN") PARTITION_SHOW_Details(sti(GameInterface.PARTITION_TABLE_MAIN.(CurRow).td1.str));
    NullSelectTable("TABLE_LIST");
    NullSelectTable("TABLE_OTHER");
    NullSelectTable("TABLE_CREW");
}

void ExitRPGHint()
{
	if (sMessageMode == "RPG_Hint")
	{
		XI_WindowShow("RPG_WINDOW", false);
		XI_WindowDisable("RPG_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", false);
		sMessageMode = "";
	}
}

void ShowRPGHint()
{
	SetSPECIALMiniTable("TABLE_SMALLSKILL", xi_refCharacter);
    SetOTHERMiniTable("TABLE_SMALLOTHER", xi_refCharacter);
    SetFormatedText("OFFICER_NAME", GetFullName(xi_refCharacter));

	XI_WindowShow("RPG_WINDOW", true);
	XI_WindowDisable("RPG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "RPG_Hint";
}

void FillGoodsTable()
{
	int n, i, qty;
	string row;
	ref rShip;
	string sGood;
	aref refGoods;

	n = 1;
	if (!CheckAttribute(&GameInterface, "TABLE_LIST.BackUp"))
	{
		GameInterface.TABLE_LIST.select = 0;
		GameInterface.TABLE_LIST.top    = 0;
		GameInterface.TABLE_LIST.BackUp = true;
	}
    for (i = 0; i< GOODS_QUANTITY; i++)
	{
        row = "tr" + n;
		sGood = Goods[i].name;
		qty = GetCargoGoods(xi_refCharacter, i);
		if (qty <= 0) continue; // только не нули

		GameInterface.TABLE_LIST.(row).index = i;

		GameInterface.TABLE_LIST.(row).td2.str = qty;
		GameInterface.TABLE_LIST.(row).td3.str = GetGoodWeightByType(i, qty);
		GameInterface.TABLE_LIST.(row).td4.str = Goods[i].Units;
		GameInterface.TABLE_LIST.(row).td5.str = Goods[i].Weight;

        GameInterface.TABLE_LIST.(row).td1.icon.group = "GOODS";
		GameInterface.TABLE_LIST.(row).td1.icon.image = sGood;
		GameInterface.TABLE_LIST.(row).td1.icon.offset = "0, 0";
		GameInterface.TABLE_LIST.(row).td1.icon.width = 20;
		GameInterface.TABLE_LIST.(row).td1.icon.height = 20;
		GameInterface.TABLE_LIST.(row).td1.textoffset = "10,0";
		GameInterface.TABLE_LIST.(row).td1.str = XI_ConvertString(sGood);
		GameInterface.TABLE_LIST.(row).td1.color = argb(255,255,255,255); // обычный цвет названия
		GameInterface.TABLE_LIST.(row).td1.scale = 0.88;
		// подменная дешёвка (GovSwapFakeGood) - красным + "(подмена)", но ТОЛЬКО пока губ.фрахт активен
		// (закрыл любым путём - клерк/барыга/неустойка/сдача - пометка пропадает)
		if (CheckAttribute(pchar, "CargoQuest.GovSwapFakeGood"))
		{
			if (i == sti(pchar.CargoQuest.GovSwapFakeGood))
			{
				if (CheckAttribute(pchar, "quest.generate_trade_quest_progress"))
				{
					if (pchar.quest.generate_trade_quest_progress == "begin" || pchar.quest.generate_trade_quest_progress == "failed")
					{
						if (LanguageGetLanguage() == "russian") GameInterface.TABLE_LIST.(row).td1.str = XI_ConvertString(sGood) + " (подмена)";
						else GameInterface.TABLE_LIST.(row).td1.str = XI_ConvertString(sGood) + " (swapped)";
						GameInterface.TABLE_LIST.(row).td1.color = argb(255,255,115,115); // красный
					}
				}
			}
		}
		n++;
	}
}

void NullSelectTable(string sControl)
{
	if (sControl != CurTable)
	{
	    GameInterface.(sControl).select = 0;
	    Table_UpdateWindow(sControl);
	}
}

void ShowItemInfo()
{
	if (CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".index")) // нет товара вообще
	{
	    ShowGoodsInfo(sti(GameInterface.(CurTable).(CurRow).index));
		XI_WindowDisable("QTY_WINDOW", false);
		XI_WindowShow("QTY_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		SetCurrentNode("QTY_CANCEL_BUTTON");
		if (GetRemovable(xi_refCharacter))
		{
		    SetSelectable("QTY_OK_BUTTON", true);
		}
		else
		{
		    SetSelectable("QTY_OK_BUTTON", false);
		}
		// мины
	    if (sti(GameInterface.(CurTable).(CurRow).index) == GOOD_POWDER)
	    {
	        SetNodeUsing("QTY_BOMB_BUTTON", true);
	        if (bSeaActive && !bAbordageStarted && GetRemovable(xi_refCharacter))
	        {
	            SetSelectable("QTY_BOMB_BUTTON", true);
	        }
	        else
	        {
	            SetSelectable("QTY_BOMB_BUTTON", false);
	        }
		}
		else
		{
		    SetNodeUsing("QTY_BOMB_BUTTON", false);
		}
	}
}

void GoodsExitCancel()
{

	XI_WindowDisable("QTY_WINDOW", true);
	XI_WindowShow("QTY_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", false);
	SetCurrentNode("TABLE_LIST");
	// пересчитаем новые пареметры от веса
	if (iShipQty != GetCargoGoods(xi_refCharacter, iCurGoodsIdx))
	{
		SetCharacterGoods(xi_refCharacter, iCurGoodsIdx, iShipQty); // вернем обратно
		OnShipScrollChange();
	}
}

void ShowGoodsInfo(int iGoodIndex)
{
	string GoodName = goods[iGoodIndex].name;

	int lngFileID = LanguageOpenFile("GoodsDescribe.txt");
	string sHeader = XI_ConvertString(GoodName);

    iCurGoodsIdx = iGoodIndex;
	string goodsDescr = GetAssembledString( LanguageConvertString(lngFileID,goodName+"_descr"), &Goods[iGoodIndex]);
    goodsDescr += newStr() + XI_ConvertString("weight") + " " + Goods[iGoodIndex].weight + " " + XI_ConvertString("cwt") +
	              GetConvertStrWithReplace("Variable_ship_26", "Interface.txt", "#space#", " ") + Goods[iGoodIndex].Units + " " + XI_ConvertString("units");

	iUnits  = sti(Goods[iGoodIndex].Units);
	if(checkAttribute(pchar, "ship.cargo.goods." + GoodName + ".isquest"))
	{
		string sTradeQ = pchar.ship.cargo.goods.(GoodName).isquest.quantity;
		string sColony = pchar.ship.cargo.goods.(GoodName).isquest.colony;
		goodsDescr += NewStr() + XI_ConvertString("YouNeedToDelivery") + sTradeQ + XI_ConvertString("QuantityOfGoodsToColony") + XI_ConvertString("Colony"+sColony) + ".";
	}

    SetFormatedText("QTY_TypeOperation", GetConvertStrWithReplace("Variable_ship_27", "Interface.txt", "#space#", " "));
    SetFormatedText("QTY_Result", "");
    GameInterface.qty_edit.str = "0";

	SetNewGroupPicture("QTY_GOODS_PICTURE", "GOODS", GoodName);
    SetFormatedText("QTY_CAPTION", sHeader);
    SetFormatedText("QTY_GOODS_INFO", goodsDescr);
	LanguageCloseFile(lngFileID);

	iShipQty = GetCargoGoods(xi_refCharacter, iGoodIndex);
    SetFormatedText("QTY_INFO_SHIP_QTY", its(iShipQty))
    SetFormatedText("QTY_INFO_STORE_QTY", its(GetGoodWeightByType(iGoodIndex, iShipQty)));

    SetFormatedText("QTY_INFO_STORE_PRICE", GetConvertStrWithReplace("Variable_ship_28", "Interface.txt", "#space#", " "));
    SetFormatedText("QTY_INFO_SHIP_PRICE",  GetConvertStr("Variable_ship_28.1", "Interface.txt"));
}

void TransactionOK()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер - не двигать/выкидывать товары NPC
	int nTradeQuantity;
	confirmChangeQTY_EDIT();

	nTradeQuantity = sti(GameInterface.qty_edit.str);

    SetCharacterGoods(xi_refCharacter, iCurGoodsIdx, iShipQty);

    if (!GetRemovable(xi_refCharacter)) return;
	if (nTradeQuantity > 0)
	{
		DropGoodsToSeaFromInterface(iCurGoodsIdx, nTradeQuantity); // остаётся плавать в море
		iShipQty = GetCargoGoods(xi_refCharacter, iCurGoodsIdx);
	}
	GoodsExitCancel();
	SetFormatedText("SHIPS_WEIGHTS", GetShipsWeights());
}

void confirmChangeQTY_EDIT()
{
	ChangeQTY_EDIT();
    SetCurrentNode("QTY_OK_BUTTON");
}

void ChangeQTY_EDIT()
{
	int  iWeight;
	GameInterface.qty_edit.str = abs(sti(GameInterface.qty_edit.str));  // приведение к числу

	string GoodName = goods[iCurGoodsIdx].name;

    // проверка на колво доступное -->
    if (sti(GameInterface.qty_edit.str) > iShipQty)
    {
        GameInterface.qty_edit.str = iShipQty;
    }
    SetFormatedText("QTY_INFO_SHIP_QTY", its(iShipQty - sti(GameInterface.qty_edit.str)))
	SetFormatedText("QTY_INFO_STORE_QTY", its(GetGoodWeightByType(iCurGoodsIdx, (iShipQty - sti(GameInterface.qty_edit.str)))));
	// прикидываем место в трюме
	iWeight = sti(GameInterface.qty_edit.str);
	SetCharacterGoods(xi_refCharacter, iCurGoodsIdx, iShipQty);
	if (iWeight > 0)
	{
		RemoveCharacterGoods(xi_refCharacter, iCurGoodsIdx, iWeight);
	}
	OnShipScrollChange();
}

void REMOVE_ALL_BUTTON()  // продать все
{
	if (!GetRemovable(xi_refCharacter)) return;
	GameInterface.qty_edit.str = iShipQty;
	ChangeQTY_EDIT();
}

void ADD_ALL_BUTTON()  // купить все
{
	if (!GetRemovable(xi_refCharacter)) return;
	GameInterface.qty_edit.str = 0;
	ChangeQTY_EDIT();
}

void REMOVE_BUTTON()  // продать
{
	if (!GetRemovable(xi_refCharacter)) return;
	GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) + iUnits);
	ChangeQTY_EDIT();
}

void ADD_BUTTON()  // купить
{
	if (!GetRemovable(xi_refCharacter)) return;
	GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) - iUnits);
	if (sti(GameInterface.qty_edit.str) < 0) GameInterface.qty_edit.str = 0;
	ChangeQTY_EDIT();
}

void DropGoodsToSeaFromInterface(int iGoodIndex, int iQuantity)
{
	ref rGood;
	string sGood;

	sGood = Goods[iGoodIndex].name;

	if (CheckAttribute(xi_refCharacter, "Ship.Cargo.Goods."+sGood))
	{
		RemoveCharacterGoods(xi_refCharacter, iGoodIndex, iQuantity);
		if (bSeaActive && !bAbordageStarted)  // море, но не каюта
		{
			iQuantity = iQuantity / sti(Goods[iGoodIndex].Units);

			int iSwimQuantity = makeint(iQuantity/3 + rand(makeint(iQuantity/3)) + rand(makeint(iQuantity/3)));

			if(iSwimQuantity < 1)
			{
				iSwimQuantity = 1;
			}

			RecalculateCargoLoad(xi_refCharacter);

			if (CheckAttribute(&Goods[iGoodIndex], "Swim"))
			{
				float fTime = stf(Goods[iGoodIndex].Swim.Time);
				string sModel = Goods[iGoodIndex].Swim.Model;

				fTime = fTime + (frnd() - 0.5) * fTime / 2.0;

				AISeaGoods_AddGood(xi_refCharacter, sGood, sModel, fTime * 60.0, iSwimQuantity);
			}
			PlaySound("Ships\jakor_002.wav");
		}
	}
}

void ShipChange()
{
	if (SeaTowOperations_BlockSwap(pchar, xi_refCharacter)) return;
	bool bOk;
	if (xi_refCharacter.id != pchar.id && sti(pchar.ship.type) != SHIP_NOTUSED)
	{
		SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("companionship"));
		SetFormatedText("REMOVE_WINDOW_TEXT", GetConvertStrWithReplace("Variable_ship_29", "Interface.txt", "#space#", " "));
		SetSelectable("REMOVE_ACCEPT_OFFICER", true);
		bOk = !bSeaActive && LAi_grp_alarmactive;
		if (bDisableMapEnter || bOk || chrDisableReloadToLocation)
		{
			SetFormatedText("REMOVE_WINDOW_TEXT", GetConvertStrWithReplace("Variable_ship_30", "Interface.txt", "#space#", " "));
			SetSelectable("REMOVE_ACCEPT_OFFICER", false);
		}
		if (bSeaActive && GetShipEscape())
		{
			SetFormatedText("REMOVE_WINDOW_TEXT", GetConvertStrWithReplace("Variable_ship_31", "Interface.txt", "#space#", " "));
			SetSelectable("REMOVE_ACCEPT_OFFICER", true);
		}
		if (!GetRemovable(xi_refCharacter))
		{
			SetFormatedText("REMOVE_WINDOW_TEXT", GetConvertStrWithReplace("Variable_ship_32", "Interface.txt", "#space#", " "));
			SetSelectable("REMOVE_ACCEPT_OFFICER", false);
		}
		if (sti(pchar.ship.type) == SHIP_NOTUSED)
		{
			SetFormatedText("REMOVE_WINDOW_TEXT", GetConvertStrWithReplace("Variable_ship_33", "Interface.txt", "#space#", " "));
			SetSelectable("REMOVE_ACCEPT_OFFICER", false);
		}
		sMessageMode = "ShipChange";
		ShowShipChangeMenu();
	}
}

void ExitShipChangeMenu()
{
	XI_WindowShow("REMOVE_OFFICER_WINDOW", false);
	XI_WindowDisable("REMOVE_OFFICER_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("SHIPS_SCROLL");
	sMessageMode = "";
}

void ShowShipChangeMenu()
{
	XI_WindowShow("REMOVE_OFFICER_WINDOW", true);
	XI_WindowDisable("REMOVE_OFFICER_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetCurrentNode("REMOVE_CANCEL_OFFICER");
}

bool GetShipEscape()
{
    if (CheckCharacterPerk(PChar, "ShipEscape") && GetHullPercent(PChar) <= 15.0)
    {
        return true;
    }
    return false;
}

void ShowOtherClick()
{
	if (CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".UserData.ID") && GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType")
	{
		// казённый корабль (квестовый конвой и т.п.): орудия опечатаны - снять и распродать их нельзя
		if (!GetRemovable(xi_refCharacter))
		{
			if (LanguageGetLanguage() == "russian") Log_Info("Казённое имущество: орудия этого корабля опечатаны.");
			else Log_Info("State property: this ship's guns are sealed.");
			return;
		}
		ShowCannonsMenu();
	}
}

void GoToShipChange()
{
	if (sMessageMode == "MoraleMessage") // мораль
	{
		AddMoneyToCharacter(pchar, -GetCharacterRaiseCrewMoraleMoney(xi_refCharacter));
		Statistic_AddValue(pchar, "PartitionPay", GetCharacterRaiseCrewMoraleMoney(xi_refCharacter));
		AddCrewMorale(xi_refCharacter, 10);
		ExitShipChangeMenu();
		OnShipScrollChange();
	}
	if (sMessageMode == "ShipChange") // обмен с кораблём
	{
		if (SeaTowOperations_BlockSwap(pchar, xi_refCharacter)) return;
		pchar.TransferChar = xi_refCharacter.index;
		InterfaceStates.ReloadMenuExit = true;
		IDoExit(RC_INTERFACE_ANY_EXIT);
		PostEvent("LaunchIAfterFrame",1,"sl", "TransferMain", 2);
	}
}

void ShowUpgradesInfo()
{
	XI_WindowShow("SHIPSUP_TEMP_GOODS_WINDOW", true);
	XI_WindowDisable("SHIPSUP_TEMP_GOODS_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	SetSelectable("SHIPSUP_BUTTON_TEMP_GOODS", false);
	Table_Clear("SHIPSUP_TABLE_GOODS", false, true, false);
	ref shTo = &RealShips[sti(xi_refCharacter.Ship.Type)];
	string sGood = "";
	aref aroot, arcur;
	makearef(aroot,shTo.GoodsForUpgrade);
	int num = GetAttributesNum(aroot);
	int gooda = 0;
	int m = 1;
	for (int i = 0; i < num; i++)
	{
		arcur = GetAttributeN(aroot,i);
		sGood = GetAttributeName(arcur);
		gooda = sti(shTo.GoodsForUpgrade.(sGood));
		if (gooda > 0)
		{
			string row = "tr" + (m);
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.image = sGood;
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.group = "GOODS";
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.width = 32;
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.height = 32;
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.offset = "-2, -1";
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.str = "";
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.align = "center";
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td2.str = XI_ConvertString(sGood);
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td2.scale = 1.0;
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td3.scale = 1.0;
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td3.str = shTo.GoodsForUpgrade.(sGood);
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td4.scale = 1.0;
			GameInterface.SHIPSUP_TABLE_GOODS.(row).td4.str = ""+GetSquadronGoods(pchar,FindGood(sGood));
			if (sti(shTo.GoodsForUpgrade.(sGood)) <= GetSquadronGoods(pchar,FindGood(sGood))) GameInterface.SHIPSUP_TABLE_GOODS.(row).td4.color = argb(255,115,255,115);
			else GameInterface.SHIPSUP_TABLE_GOODS.(row).td4.color = argb(255,255,115,115);
			m++;
		}
	}
	string city = "Colony"+Characters[sti(shTo.Shipyarder)].City+"Gen";
	SetFormatedText("SHIPSUP_CAPTION", XI_ConvertString("GoodsRemaining")+" "+XI_ConvertString(city)+":");
	Table_UpdateWindow("SHIPSUP_TABLE_GOODS");
	SetCurrentNode("SHIPSUP_TABLE_GOODS");
}

void HideUpgradesInfo()
{
	XI_WindowShow("SHIPSUP_TEMP_GOODS_WINDOW", false);
	XI_WindowDisable("SHIPSUP_TEMP_GOODS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);
	SetSelectable("SHIPSUP_BUTTON_TEMP_GOODS", true);
	SetCurrentNode("SHIPS_SCROLL");
}

void ShowMoraleMessage()
{
	int  sum = GetCharacterRaiseCrewMoraleMoney(xi_refCharacter);
	SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("RaiseMorale"));
	SetFormatedText("REMOVE_WINDOW_TEXT", XI_ConvertString("RaiseMoraleMoney") + FindRussianMoneyString(sum));

	if (sum <= sti(pchar.money))
	{
		SetSelectable("REMOVE_ACCEPT_OFFICER", true);
	}
	else
	{
		SetSelectable("REMOVE_ACCEPT_OFFICER", false);
	}
	sMessageMode = "MoraleMessage";
	ShowShipChangeMenu();
}

void ShowCannonsMenu()
{
	XI_WindowShow("CANNONS_WINDOW", true);
	XI_WindowDisable("CANNONS_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetCurrentNode("CANNONS_CANCEL");

    SetBackupCannons(); // запомнить, для отката

    bool bOk  = !bSeaActive && LAi_grp_alarmactive;
    bool bOk2 = bAbordageStarted && !bCabinStarted; // в абордаже не жать пушки
	if (bDisableMapEnter || bOk || bOk2)
    { // идёт бой, запрет смены орудий
    	SetSelectable("CANNONS_OK", false);
	}
	else
	{
		SetSelectable("CANNONS_OK", true);
	}
	if (!GetRemovable(xi_refCharacter))
	{
		SetSelectable("CANNONS_OK", false);
	}
    DeleteAttribute(&GameInterface, "CANNONS_TABLE.BackUp")
    CannonsMenuRefresh();
}
void CannonsMenuRefresh()
{
	int idx = GetCannonGoodsIdxByType(sti(xi_refCharacter.Ship.Cannons.Type));
	if (idx != -1)
	{
	    SetNewGroupPicture("CANNONS_PIC", "GOODS", Goods[idx].Name);
		SetFormatedText("CANNONS_TEXT", XI_ConvertString(Goods[idx].Name));

		SetFormatedText("CANNONS_QTY_F", its(GetBortCannonsQty(xi_refCharacter, "fcannon")));
		SetFormatedText("CANNONS_QTY_B", its(GetBortCannonsQty(xi_refCharacter, "bcannon")));
		SetFormatedText("CANNONS_QTY_R", its(GetBortCannonsQty(xi_refCharacter, "rcannon")));
		SetFormatedText("CANNONS_QTY_L", its(GetBortCannonsQty(xi_refCharacter, "lcannon")));
		/// всего GetCannonsNum(xi_refCharacter)
	}
	else
	{
		SetNewGroupPicture("CANNONS_PIC", "GOODS", "");
		SetFormatedText("CANNONS_TEXT", "");
		SetFormatedText("CANNONS_QTY_F", "0");
		SetFormatedText("CANNONS_QTY_B", "0");
		SetFormatedText("CANNONS_QTY_R", "0");
		SetFormatedText("CANNONS_QTY_L", "0");
	}
	FillCannonsTable();
	if (IsMainCharacter(xi_refCharacter)) BI_UpdateCannons();
}
void ExitCannonsMenu()
{
	HideCannonsMenu();

	GetBackupCannons(); // отмена
	OnShipScrollChange();
}

void HideCannonsMenu()
{
	XI_WindowShow("CANNONS_WINDOW", false);
	XI_WindowDisable("CANNONS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("TABLE_OTHER");
	sMessageMode = "";
}

/// установить орудия по борту (сперва расчитать дельту было стало - лишнее в запасы)
void SetCannonsToBort(ref chr, string sBort, int iQty)
{
	int     curQty = GetBortCannonsQty(chr, sBort);
	int     maxQty = GetBortCannonsMaxQty(chr, sBort);
	int     i, delta;
	string  attr;
	int     center, left, right; // счетчики орудий для распределения
	bool    bLeft; // направление хода
	string  sBort_real;

	if(sBort == "rcannon") sBort_real = "cannonr";
	if(sBort == "lcannon") sBort_real = "cannonl";
	if(sBort == "fcannon") sBort_real = "cannonf";
	if(sBort == "bcannon") sBort_real = "cannonb";

	if (iQty > maxQty) iQty = maxQty;
	if (iQty < 0) iQty = 0;

	int idx = GetCannonGoodsIdxByType(sti(chr.Ship.Cannons.Type));
    delta = iQty - curQty;
    if (delta > 0)
    {
    	if (GetCargoGoods(chr, idx) < delta) iQty = curQty + GetCargoGoods(chr, idx);
    }
	if (iQty > curQty)
	{ // списать со склада
		RemoveCharacterGoodsSelf(chr, idx, (iQty - curQty));
	}
	else
	{
		if (iQty < curQty)
		{// лишние на склад
			SetCharacterGoods(chr, idx, GetCargoGoods(chr, idx) + (curQty - iQty)); // этот метод, тк перегруз может быть, а  AddCharacterGoodsSimple режет перегруз
		}
	}
	// нулим колво пушек на борту и распределяем заново от центра (как они на модели по номерам не знаю, допуск, что подряд)
	for (i = 0; i < maxQty; i++)
	{
		attr = "c" + i;
		chr.Ship.Cannons.borts.(sBort).damages.(attr) = 1.0; // поломана на 100 процентов, не палит, те нет её
		chr.Ship.Cannons.borts.(sBort_real).damages.(attr) = 1.0; // поломана на 100 процентов, не палит, те нет её
	}
	// распределяем
	if (iQty > 0)
	{
		center = makeint(maxQty / 2); // целочисленное деление
		left   = center - 1;
		right  = center;
		i = 0; // сколько распределили уже
		bLeft = true;
		while (i < iQty)
		{
			if (bLeft)
			{
				if (left >= 0)
				{
					attr = "c" + left;
					left--;
				}
				else
				{
					attr = "c" + right;
					right++;
				}
				if (right < maxQty) bLeft = false;
			}
			else
			{
				if (right < maxQty)
				{
					attr = "c" + right;
					right++;
				}
				else
				{
					attr = "c" + left;
					left--;
				}
				if (left >= 0) bLeft = true;
			}
			chr.Ship.Cannons.borts.(sBort).damages.(attr) = 0.0; // новая, не битая
			chr.Ship.Cannons.borts.(sBort_real).damages.(attr) = 0.0; // новая, не битая
			i++;
		}
	}
	RecalculateCargoLoad(chr);  // пересчет, тк пушки снялись
}

void CanonsRemoveAll()
{
    if (!GetRemovable(xi_refCharacter)) return; // казённый корабль: орудия опечатаны (подстраховка к гварду открытия окна)
    SetCannonsToBort(xi_refCharacter, "fcannon", 0);
    SetCannonsToBort(xi_refCharacter, "bcannon", 0);
    SetCannonsToBort(xi_refCharacter, "rcannon", 0);
    SetCannonsToBort(xi_refCharacter, "lcannon", 0);
    OnShipScrollChange();
    CannonsMenuRefresh();
}

// бакап значений, до применения
void SetBackupCannons()
{
	aref    arTo, arFrom;
	NullCharacter.ShipBak.Cannons = "";
	NullCharacter.ShipBak.Cargo   = "";

	makearef(arTo,   NullCharacter.ShipBak.Cannons);
	makearef(arFrom, xi_refCharacter.Ship.Cannons);
	CopyAttributes(arTo, arFrom);

	makearef(arTo,   NullCharacter.ShipBak.Cargo);
	makearef(arFrom, xi_refCharacter.Ship.Cargo);
	CopyAttributes(arTo, arFrom);
}

void GetBackupCannons()
{
	aref    arTo, arFrom;

	makearef(arFrom,   NullCharacter.ShipBak.Cannons);
	makearef(arTo, xi_refCharacter.Ship.Cannons);
	CopyAttributes(arTo, arFrom);

	makearef(arFrom,   NullCharacter.ShipBak.Cargo);
	makearef(arTo, xi_refCharacter.Ship.Cargo);
	CopyAttributes(arTo, arFrom);
}

void CanonsSetAll(ref chr)
{
    if (GetCannonQuantity(chr) <= 0) return;
	// сначала все убрать
    if (GetCannonsNum(chr) > 0)
    {
		SetCannonsToBort(chr, "fcannon", 0);
	    SetCannonsToBort(chr, "bcannon", 0);
	    SetCannonsToBort(chr, "rcannon", 0);
	    SetCannonsToBort(chr, "lcannon", 0);
    }
    //новый калибр назначить
    if (CurTable == "CANNONS_TABLE" && CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".index"))
    {
		chr.Ship.Cannons.Type = sti(Goods[sti(GameInterface.(CurTable).(CurRow).index)].CannonIdx);
	}
    // потом все выставить раскидав по бортам
    int idx = GetCannonGoodsIdxByType(sti(chr.Ship.Cannons.Type));
    int fb, lb, rb, bb;
    int qty;

    if (idx != -1)
    {
    	qty = GetCargoGoods(chr, idx);

    	rb = GetBortCannonsMaxQty(chr, "rcannon");

    	if (rb  > (qty / 2)) rb = qty / 2;
    	qty = qty - rb;
    	if (qty < 0) qty = 0;

    	lb = GetBortCannonsMaxQty(chr, "lcannon");
    	if (lb > qty) lb = qty;
    	qty = qty - lb;
    	if (qty < 0) qty = 0;

    	bb = GetBortCannonsMaxQty(chr, "bcannon");
    	if (bb > qty) bb = qty;
    	qty = qty - bb;
    	if (qty < 0) qty = 0;

    	fb = GetBortCannonsMaxQty(chr, "fcannon");
    	if (fb > qty) fb = qty;

    	SetCannonsToBort(chr, "fcannon", fb);
	    SetCannonsToBort(chr, "bcannon", bb);
	    SetCannonsToBort(chr, "rcannon", rb);
	    SetCannonsToBort(chr, "lcannon", lb);
    }
    // рефреш
    OnShipScrollChange();
    CannonsMenuRefresh();
}

void ChangeCannonNum(string sBort, int add)
{
    if (!GetRemovable(xi_refCharacter)) return; // казённый корабль: орудия опечатаны (подстраховка к гварду открытия окна)
    SetCannonsToBort(xi_refCharacter, sBort, GetBortCannonsQty(xi_refCharacter, sBort) + add);
    OnShipScrollChange();
	CannonsMenuRefresh();
}

void FillCannonsTable()
{
	int n, i, qty;
	string row;
	ref rShip;
	string sGood;
	aref refGoods;

	n = 1;
	Table_Clear("CANNONS_TABLE", false, true, false);
	GameInterface.CANNONS_TABLE.hr.td1.str = GetConvertStrWithReplace("Variable_ship_34", "Interface.txt", "#space#", " ");
	GameInterface.CANNONS_TABLE.hr.td1.scale = 0.8;
	GameInterface.CANNONS_TABLE.hr.td2.str = GetConvertStrWithReplace("Variable_ship_35", "Interface.txt", "#space#", " ");
	GameInterface.CANNONS_TABLE.hr.td2.scale = 0.8;
	GameInterface.CANNONS_TABLE.hr.td3.str = GetConvertStrWithReplace("Variable_ship_36", "Interface.txt", "#space#", " ");
	GameInterface.CANNONS_TABLE.hr.td3.scale = 0.8;
	GameInterface.CANNONS_TABLE.hr.td4.str = GetConvertStrWithReplace("Variable_ship_37", "Interface.txt", "#space#", " ");
	GameInterface.CANNONS_TABLE.hr.td4.scale = 0.8;
	GameInterface.CANNONS_TABLE.hr.td5.str = GetConvertStrWithReplace("Variable_ship_38", "Interface.txt", "#space#", " ");
	GameInterface.CANNONS_TABLE.hr.td5.scale = 0.8;

	if (!CheckAttribute(&GameInterface, "CANNONS_TABLE.BackUp"))
	{
		GameInterface.CANNONS_TABLE.select = 0;
		GameInterface.CANNONS_TABLE.top    = 0;
		GameInterface.CANNONS_TABLE.BackUp = true;
	}
	if (GetCannonQuantity(xi_refCharacter) > 0)
	{
	    for (i = 0; i< GOODS_QUANTITY; i++)
		{
	        row = "tr" + n;
			sGood = Goods[i].name;
			qty = GetCargoGoods(xi_refCharacter, i);
			if (qty <= 0) continue; // только не нули

	        if (!CheckAttribute(&Goods[i], "type")) continue;
	        if (sti(Goods[i].type) != TRADE_TYPE_CANNONS) continue;

	        if (GetCannonCaliber(sti(Goods[i].CannonIdx)) > GetMaximumCaliber(xi_refCharacter)) continue;
			GameInterface.CANNONS_TABLE.(row).index = i;

			GameInterface.CANNONS_TABLE.(row).td2.str = qty;
			GameInterface.CANNONS_TABLE.(row).td2.scale = 0.85;
			GameInterface.CANNONS_TABLE.(row).td3.str = sti(Goods[i].FireRange);
			GameInterface.CANNONS_TABLE.(row).td3.scale = 0.85;
			GameInterface.CANNONS_TABLE.(row).td4.str = "x"+FloatToString(stf(Goods[i].DamageMultiply), 1);
			GameInterface.CANNONS_TABLE.(row).td4.scale = 0.85;
			GameInterface.CANNONS_TABLE.(row).td5.str = sti(Goods[i].ReloadTime);
			GameInterface.CANNONS_TABLE.(row).td5.scale = 0.85;

	        GameInterface.CANNONS_TABLE.(row).td1.icon.group = "GOODS";
			GameInterface.CANNONS_TABLE.(row).td1.icon.image = sGood;
			GameInterface.CANNONS_TABLE.(row).td1.icon.offset = "0, 0";
			GameInterface.CANNONS_TABLE.(row).td1.icon.width = 32;
			GameInterface.CANNONS_TABLE.(row).td1.icon.height = 32;
			GameInterface.CANNONS_TABLE.(row).td1.textoffset = "28,0";
			GameInterface.CANNONS_TABLE.(row).td1.str = XI_ConvertString(sGood);
			GameInterface.CANNONS_TABLE.(row).td1.scale = 0.75;
			n++;
		}
	}
	Table_UpdateWindow("CANNONS_TABLE");
}

void ShowPartitionWindow()
{
    string str;
	string sTitul = "";
	if (isMainCharacterPatented())
	{
		sTitul = GetAddress_FormTitle(sti(Items[sti(pchar.EquipedPatentId)].Nation), sti(Items[sti(pchar.EquipedPatentId)].TitulCur))
	}

	str = GetConvertStrWithReplace("Variable_ship_39", "Interface.txt", "#space#", " ") + GetDateString() + " " + GetTimeString() + NewStr();
	if(CheckAttribute(pchar, "paymentdate"))
	{
		str += GetConvertStrWithReplace("Variable_ship_40", "Interface.txt", "#space#", " ") + pchar.paymentdate + NewStr();
	}
	str += sTitul + " " + GetFullName(pchar) + NewStr() + XI_ConvertString("Rank") + ": " + sti(pchar.rank) + NewStr();
	str += XI_ConvertString("m_Complexity") + ": " + GetLevelComplexity(MOD_SKILL_ENEMY_RATE) + NewStr() +
			XI_ConvertString("OurMoney") + FindRussianMoneyString(sti(pchar.money)) + NewStr() + "*****" + NewStr();
    str += GetConvertStrWithReplace("Variable_ship_41", "Interface.txt", "#space#", " ") + NewStr() + GetConvertStrWithReplace("Variable_ship_42", "Interface.txt", "#space#", " ") + GetPartitionAmount("Partition.MonthPart.Hero") + NewStr() +
		  GetConvertStrWithReplace("Variable_ship_43", "Interface.txt", "#space#", " ") + GetPartitionAmount("Partition.MonthPart.Officers") + NewStr() +
		  GetConvertStrWithReplace("Variable_ship_44", "Interface.txt", "#space#", " ") + GetPartitionAmount("Partition.MonthPart.Crew") + NewStr() + // + "-----" + NewStr() +
		  GetConvertStrWithReplace("Variable_ship_45", "Interface.txt", "#space#", " ") + GetPartitionAmount("Partition.MonthPart") + NewStr();
	str += "*****" + NewStr() + GetConvertStrWithReplace("Variable_ship_46", "Interface.txt", "#space#", " ") + GetPartitionAmount("CrewPayment");

	SetFormatedText("PARTITION_WINDOW_TEXT", str);
    SetSelectable("PARTITION_OK", false);
	if (GetPartitionAmount("Partition.MonthPart") > 0 || GetPartitionAmount("CrewPayment") > 0)
	{
		if (sti(Pchar.Money) > 0)
		{
			SetSelectable("PARTITION_OK", true);
		}
	}
	XI_WindowShow("PARTITION_WINDOW", true);
	XI_WindowDisable("PARTITION_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	PARTITION_SHOW();

	SetCurrentNode("PARTITION_CANCEL");
}

void PARTITION_SHOW()
{
	bPartShow = !bPartShow;
	SetNodeUsing("PARTITION_TABLE_MAIN", bPartShow);
	SetNodeUsing("PARTITION_SCROLL_MAIN", bPartShow);
	SetNodeUsing("PARTITION_REMEMBER_TIME", bPartShow);
	if (bPartShow)
		SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"PARTITION_SHOW", 2, "MAIN_ICONS", "upbutton");//меняем направление стрелки-картинки при каждом клике
		else SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"PARTITION_SHOW", 2, "MAIN_ICONS", "downbutton");

	if (!bPartShow)
	{
		XI_WindowShow("PARTITION_WINDOW2", false);//скрываем список "разделов добычи"
		XI_WindowDisable("PARTITION_WINDOW2", true);

		GameInterface.PARTITION_TABLE_MAIN.select = 0;//сбрасываем выделенную строку, при следующем открытии детализация всё равно не будет открыта
		Table_UpdateWindow("PARTITION_TABLE_MAIN");
	}
}

void ExitPartitionWindow()
{
	XI_WindowShow("PARTITION_WINDOW", false);
	XI_WindowDisable("PARTITION_WINDOW", true);

	XI_WindowShow("PARTITION_WINDOW2", false);
	XI_WindowDisable("PARTITION_WINDOW2", true);

	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("SHIPS_SCROLL");
	sMessageMode = "";
}

void DoPartitionPay()
{
	int sum = 0;
	if (GetPartitionAmount("Partition.MonthPart") > 0 || GetPartitionAmount("CrewPayment") > 0)
	if (GetPartitionAmount("CrewPayment") > 0)	//Это долг с прошлого месяца
	{
	    sum = GetPartitionAmount("CrewPayment");
	    if (sti(Pchar.Money) < sum) sum = sti(Pchar.Money);
        Pchar.CrewPayment = sti(Pchar.CrewPayment) - sum;
        if (sti(Pchar.CrewPayment) <= 0) DeleteAttribute(Pchar, "CrewPayment");
	}
	else
	{
		if (GetPartitionAmount("Partition.MonthPart") > 0)		//MonthPart - Это доли текущего месяца
		{
		    sum = GetPartitionAmount("Partition.MonthPart");
		    if (sti(Pchar.Money) < sum) sum = sti(Pchar.Money);
	        Pchar.Partition.MonthPart = sti(Pchar.Partition.MonthPart) - sum;
	        AddCrewMorale(xi_refCharacter, 2);
			if (GetPartitionAmount("Partition.MonthPart") < 1) 
			{
				ClearPartitionHistory();//очищаем историю разделов добычи
				Table_Clear("PARTITION_TABLE_MAIN", false, true, false);
			}
		}
	}
	pchar.paymentdate = GetDateString() + " " + GetTimeString();
	AddMoneyToCharacter(Pchar, -sum);
	Statistic_AddValue(pchar, "PartitionPay", sum);

	OnShipScrollChange();
	ExitPartitionWindow();
}

int GetPartitionAmount(string _param)
{
    if (!CheckAttribute(Pchar, _param)) return 0;
    return sti(Pchar.(_param));
}

void SortCompanionsBy(string attrName) // сортируем по реальной скорости
{
    int ids[7];
    int ids_num = 0;

    int cq = GetCompanionQuantity(pchar);
    if (cq <= 1) return;

    for(int i = 1; i < cq; i++) {
        int cn = GetCompanionIndex(pchar, i);
        if (cn == -1) {
            continue;
        }

        if(sti(characters[cn].ship.type) == SHIP_NOTUSED) {
            ids[ids_num] = cn;
            ids_num++;
            continue;
        }

        ref rBaseShip = GetRealShip(sti(characters[cn].ship.type));
		if (!CheckAttribute(rBaseShip, attrName)) {
			trace(GetConvertStrWithReplace("Variable_ship_47", "Interface.txt", "#space#", " ") + attrName + GetConvertStrWithReplace("Variable_ship_48", "Interface.txt", "#space#", " ") + rBaseShip.Name + "'");
            return; // чтобы ничего не сломать
		}

        {
			// insert
            int j;
            for(j = 0; j < ids_num; j++) {
                bool insert = false;
                int shipTypeLhs = sti(characters[ids[j]].ship.type);
                if(shipTypeLhs == SHIP_NOTUSED) {
                    // no ship
                    insert = true;
                } else {
                    // check if greater or (equal+type is greater)
                    ref rBaseShipLhs = GetRealShip(shipTypeLhs);

                    int rhsVal = makeint(FindShipSpeed(characters[cn], rBaseShip)*100);		//берём реальную скорость с учетом бонусов
                    int lhsVal = makeint(FindShipSpeed(characters[ids[j]], rBaseShipLhs)*100);

                    bool valueLess = lhsVal < rhsVal;
                    bool typeLess = sti(characters[ids[j]].ship.type) < sti(characters[cn].ship.type);
                    bool valueEqTypeLess = rhsVal == lhsVal && typeLess;

                    insert = valueLess || valueEqTypeLess;
                }
                if(insert) {
                    // shift
                    for(int k=ids_num; k>j; k--) {
                        ids[k] = ids[k-1];
                    }
                    break;
                }
            }
            ids[j] = cn;
            ids_num++;
		}
	}

    for(i = 0; i < ids_num; i++)
	{
        string id = "id"+(i+1);
	    pchar.Fellows.Companions.(id) = ids[i];
	}

    Event(EVENT_CHANGE_COMPANIONS,"");
    IDoExit(RC_INTERFACE_TO_SHIP);
}

void OnTableClick()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер - не открывать окно операций с товаром NPC
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();

//--> mod tablesort
	if (sControl == "TABLE_LIST" || sControl == "PARTITION_TABLE_MAIN")
	{
		if (!SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sControl, 1 )) SortTable(sControl, iColumn);
		Table_UpdateWindow(sControl);
	}
//<-- mod tablesort
}

void FillPartitionTables()
{
	int TotalBooty;
	string row;

	GameInterface.PARTITION_TABLE_MAIN.hr.td1.sorttype = "";
	GameInterface.PARTITION_TABLE_MAIN.hr.td2.sorttype = "string";
	GameInterface.PARTITION_TABLE_MAIN.hr.td3.sorttype = "string";
	GameInterface.PARTITION_TABLE_MAIN.hr.td4.sorttype = "";
	GameInterface.PARTITION_TABLE_MAIN.hr.td5.sorttype = "";
	GameInterface.PARTITION_TABLE_MAIN.hr.td6.sorttype = "";

	GameInterface.PARTITION_TABLE_MAIN.hr.td1.str = " # ";
	GameInterface.PARTITION_TABLE_MAIN.hr.td2.str = GetConvertStrWithReplace("Variable_ship_67", "Interface.txt", "#space#", " ");
	GameInterface.PARTITION_TABLE_MAIN.hr.td3.str = GetConvertStrWithReplace("Variable_ship_68", "Interface.txt", "#space#", " ");
	GameInterface.PARTITION_TABLE_MAIN.hr.td4.str = GetConvertStrWithReplace("Variable_ship_69", "Interface.txt", "#space#", " ");
	GameInterface.PARTITION_TABLE_MAIN.hr.td5.str = GetConvertStrWithReplace("Variable_ship_70", "Interface.txt", "#space#", " ");
	GameInterface.PARTITION_TABLE_MAIN.hr.td6.str = GetConvertStrWithReplace("Variable_ship_71", "Interface.txt", "#space#", " ");

	aref arCurPartition, arCurPart;
	makearef(arCurPartition, NullCharacter.Partition);
	int num = GetAttributesNum(arCurPartition);
	for (int i=0; i < num; i++)
	{
		row = "tr" + (i+1);
		arCurPart = GetAttributeN(arCurPartition, i);
		GameInterface.PARTITION_TABLE_MAIN.(row).td1.str = GetAttributeName(arCurPart);
		// LEO: MSum (стоимость эскадры на момент снимка) пишется в crew.c и только при полном пересчёте -
		// у части записей раздела его нет, и голое чтение сыпало "missed attribute: msum" в лог.
		// В самом crew.c этот же атрибут читается уже под CheckAttribute. Нет записи = 0, как и было.
		int iMSumBefore = 0;
		if (CheckAttribute(arCurPart, "before.MSum")) iMSumBefore = sti(arCurPart.before.MSum);
		GameInterface.PARTITION_TABLE_MAIN.(row).td2.str = arCurPart.before;
		GameInterface.PARTITION_TABLE_MAIN.(row).td4.str = MakeMoneyShow(iMSumBefore, MONEY_SIGN,MONEY_DELIVER);
		if (checkAttribute(arCurPart, "after"))
		{
			int iMSumAfter = 0;
			if (CheckAttribute(arCurPart, "after.MSum")) iMSumAfter = sti(arCurPart.after.MSum);
			GameInterface.PARTITION_TABLE_MAIN.(row).td3.str = arCurPart.after;
			GameInterface.PARTITION_TABLE_MAIN.(row).td5.str = MakeMoneyShow(iMSumAfter, MONEY_SIGN,MONEY_DELIVER);
			TotalBooty = iMSumAfter - iMSumBefore;
			if (TotalBooty < 0)
				GameInterface.PARTITION_TABLE_MAIN.(row).td6.str = "0(" + MakeMoneyShow(TotalBooty, MONEY_SIGN,MONEY_DELIVER) + ")";
			else GameInterface.PARTITION_TABLE_MAIN.(row).td6.str = MakeMoneyShow(TotalBooty, MONEY_SIGN,MONEY_DELIVER) + ")";
			GameInterface.PARTITION_TABLE_MAIN.(row).td6.scale = 0.8;
		}
		else
		{
			GameInterface.PARTITION_TABLE_MAIN.(row).td3.str = "???";
			GameInterface.PARTITION_TABLE_MAIN.(row).td5.str = "???";
			GameInterface.PARTITION_TABLE_MAIN.(row).td6.str = "???";
		}
		GameInterface.PARTITION_TABLE_MAIN.(row).td6.textoffset = "-5,0";//смещение из-за скролла
	}
	GameInterface.PARTITION_TABLE_MAIN.select = 0;
	Table_UpdateWindow("PARTITION_TABLE_MAIN");
}

void PARTITION_SHOW_Details(int iCount)
{
	aref arCurPart;
	makearef(arCurPart, NullCharacter.Partition.(iCount));

	GameInterface.PARTITION_TABLE_BEFORE2.hr.td1.str = arCurPart.before;//Дата
	GameInterface.PARTITION_TABLE_BEFORE2.hr.td2.str = GetConvertStrWithReplace("Variable_ship_49", "Interface.txt", "#space#", " ");//Доля
	GameInterface.PARTITION_TABLE_BEFORE2.tr1.td1.str = GetConvertStrWithReplace("Variable_ship_50", "Interface.txt", "#space#", " ") + arCurPart.before.HowCrew;//матросов
	GameInterface.PARTITION_TABLE_BEFORE2.tr2.td1.str = GetConvertStrWithReplace("Variable_ship_51", "Interface.txt", "#space#", " ") + arCurPart.before.HowOff;//офицеров
	GameInterface.PARTITION_TABLE_BEFORE2.tr3.td1.str = GetConvertStrWithReplace("Variable_ship_52", "Interface.txt", "#space#", " ") + arCurPart.before.HowComp;//компаньонов:
	GameInterface.PARTITION_TABLE_BEFORE2.tr4.td1.str = GetFullName(pchar) + ": 1";
	GameInterface.PARTITION_TABLE_BEFORE2.tr5.td1.str = GetConvertStrWithReplace("Variable_ship_53", "Interface.txt", "#space#", " ") + (sti(arCurPart.before.HowComp)+1);//содерж. кораблей:
	GameInterface.PARTITION_TABLE_BEFORE2.tr1.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE2.tr2.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE2.tr3.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE2.tr4.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE2.tr5.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE2.tr1.td2.str = FloatToString(100*stf(arCurPart.before.pCrew),1) + "%";
	GameInterface.PARTITION_TABLE_BEFORE2.tr2.td2.str = FloatToString(100*stf(arCurPart.before.pOfficer),1) + "%";
	GameInterface.PARTITION_TABLE_BEFORE2.tr3.td2.str = FloatToString(100*stf(arCurPart.before.pCompanion),1) + "%";
	GameInterface.PARTITION_TABLE_BEFORE2.tr4.td2.str = FloatToString(100*stf(arCurPart.before.pHeroPart),1) + "%";
	GameInterface.PARTITION_TABLE_BEFORE2.tr5.td2.str = FloatToString(100*stf(arCurPart.before.pHeroShips),1) + "%";
	GameInterface.PARTITION_TABLE_BEFORE2.select = 0;
	Table_UpdateWindow("PARTITION_TABLE_BEFORE2");

	GameInterface.PARTITION_TABLE_BEFORE.hr.td1.str = GetConvertStrWithReplace("Variable_ship_54", "Interface.txt", "#space#", " ");//Имущество
	GameInterface.PARTITION_TABLE_BEFORE.hr.td2.str = GetConvertStrWithReplace("Variable_ship_55", "Interface.txt", "#space#", " ");//Стоимость
	GameInterface.PARTITION_TABLE_BEFORE.tr1.td1.str = GetConvertStrWithReplace("Variable_ship_56", "Interface.txt", "#space#", " ");//золото
	GameInterface.PARTITION_TABLE_BEFORE.tr1.td2.str = MakeMoneyShow(sti(arCurPart.before.Money), MONEY_SIGN,MONEY_DELIVER);
	GameInterface.PARTITION_TABLE_BEFORE.tr2.td1.str = GetConvertStrWithReplace("Variable_ship_57", "Interface.txt", "#space#", " ");//100 * stf(GetConvertStr("PriceShips", "PartitionSettings.txt"))
	GameInterface.PARTITION_TABLE_BEFORE.tr2.td2.str = MakeMoneyShow(sti(arCurPart.before.Ships), MONEY_SIGN,MONEY_DELIVER);
	GameInterface.PARTITION_TABLE_BEFORE.tr3.td1.str = GetConvertStrWithReplace("Variable_ship_58", "Interface.txt", "#space#", " ");//100 * stf(GetConvertStr("PriceCannons", "PartitionSettings.txt")
	GameInterface.PARTITION_TABLE_BEFORE.tr3.td2.str = MakeMoneyShow(sti(arCurPart.before.Cannons), MONEY_SIGN,MONEY_DELIVER);
	GameInterface.PARTITION_TABLE_BEFORE.tr4.td1.str = GetConvertStrWithReplace("Variable_ship_59", "Interface.txt", "#space#", " ");//100 * stf(GetConvertStr("PriceGoods", "PartitionSettings.txt")
	GameInterface.PARTITION_TABLE_BEFORE.tr4.td2.str = MakeMoneyShow(sti(arCurPart.before.Goods), MONEY_SIGN,MONEY_DELIVER);
	GameInterface.PARTITION_TABLE_BEFORE.tr5.td1.str = GetConvertStrWithReplace("Variable_ship_60", "Interface.txt", "#space#", " ");//100 * stf(GetConvertStr("PriceCannons", "PartitionSettings.txt")
	GameInterface.PARTITION_TABLE_BEFORE.tr5.td2.str = MakeMoneyShow(sti(arCurPart.before.GCannons), MONEY_SIGN,MONEY_DELIVER);
	GameInterface.PARTITION_TABLE_BEFORE.tr1.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE.tr2.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE.tr3.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE.tr4.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE.tr5.td1.align = "left";
	GameInterface.PARTITION_TABLE_BEFORE.select = 0;
	Table_UpdateWindow("PARTITION_TABLE_BEFORE");

	GameInterface.PARTITION_TABLE_AFTER.hr.td1.str = GetConvertStrWithReplace("Variable_ship_61", "Interface.txt", "#space#", " ");
	GameInterface.PARTITION_TABLE_AFTER.hr.td2.str = GetConvertStrWithReplace("Variable_ship_62", "Interface.txt", "#space#", " ");
	GameInterface.PARTITION_TABLE_AFTER2.hr.td1.str = "???";
	GameInterface.PARTITION_TABLE_AFTER2.hr.td2.str = GetConvertStrWithReplace("Variable_ship_63", "Interface.txt", "#space#", " ");
	if (checkAttribute(arCurPart, "after"))
	{
		GameInterface.PARTITION_TABLE_AFTER2.hr.td1.str = arCurPart.after;
		GameInterface.PARTITION_TABLE_AFTER2.tr1.td1.str = GetConvertStrWithReplace("Variable_ship_64", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER2.tr2.td1.str = GetConvertStrWithReplace("Variable_ship_65", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER2.tr3.td1.str = GetConvertStrWithReplace("Variable_ship_66", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER2.tr4.td1.str = GetFullName(pchar) + ":";
		GameInterface.PARTITION_TABLE_AFTER2.tr5.td1.str = GetConvertStrWithReplace("Variable_ship_53", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER2.tr1.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER2.tr2.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER2.tr3.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER2.tr4.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER2.tr5.td1.align = "left";
		float fTotalBooty = sti(arCurPart.after.MSum) - sti(arCurPart.before.MSum);
		if (fTotalBooty < 0)
			GameInterface.PARTITION_TABLE_AFTER2.tr4.td2.str = "0(" + MakeMoneyShow(fTotalBooty, MONEY_SIGN,MONEY_DELIVER) + ")";
		else GameInterface.PARTITION_TABLE_AFTER2.tr4.td2.str = MakeMoneyShow(Makeint(stf(arCurPart.before.pHeroPart) * fTotalBooty), MONEY_SIGN,MONEY_DELIVER);
		if (fTotalBooty < 0) fTotalBooty = 0;
		GameInterface.PARTITION_TABLE_AFTER2.tr1.td2.str = MakeMoneyShow(Makeint(stf(arCurPart.before.pCrew) * fTotalBooty), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER2.tr2.td2.str = MakeMoneyShow(Makeint(stf(arCurPart.before.pOfficer) * fTotalBooty), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER2.tr3.td2.str = MakeMoneyShow(Makeint(stf(arCurPart.before.pCompanion) * fTotalBooty), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER2.tr5.td2.str = MakeMoneyShow(Makeint(stf(arCurPart.before.pHeroShips) * fTotalBooty), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER2.select = 0;

		GameInterface.PARTITION_TABLE_AFTER.tr1.td1.str = GetConvertStrWithReplace("Variable_ship_56", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER.tr1.td2.str = MakeMoneyShow(sti(arCurPart.after.Money) - sti(arCurPart.before.Money), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER.tr2.td1.str = GetConvertStrWithReplace("Variable_ship_57", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER.tr2.td2.str = MakeMoneyShow(sti(arCurPart.after.Ships) - sti(arCurPart.before.Ships), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER.tr3.td1.str = GetConvertStrWithReplace("Variable_ship_58", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER.tr3.td2.str = MakeMoneyShow(sti(arCurPart.after.Cannons) - sti(arCurPart.before.Cannons), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER.tr4.td1.str = GetConvertStrWithReplace("Variable_ship_59", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER.tr4.td2.str = MakeMoneyShow(sti(arCurPart.after.Goods) - sti(arCurPart.before.Goods), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER.tr5.td1.str = GetConvertStrWithReplace("Variable_ship_60", "Interface.txt", "#space#", " ");
		GameInterface.PARTITION_TABLE_AFTER.tr5.td2.str = MakeMoneyShow(sti(arCurPart.after.GCannons) - sti(arCurPart.before.GCannons), MONEY_SIGN,MONEY_DELIVER);
		GameInterface.PARTITION_TABLE_AFTER.tr1.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER.tr2.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER.tr3.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER.tr4.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER.tr5.td1.align = "left";
		GameInterface.PARTITION_TABLE_AFTER.select = 0;
	}
	else
	{
		Table_Clear("PARTITION_TABLE_AFTER", false, true, false);//фикс - очищаем правые таблицы, если мы в море и итоговой суммы добычи ещё нет
		Table_Clear("PARTITION_TABLE_AFTER2", false, true, false);
	}
	Table_UpdateWindow("PARTITION_TABLE_AFTER2");
	Table_UpdateWindow("PARTITION_TABLE_AFTER");

	XI_WindowShow("PARTITION_WINDOW2", true);
	XI_WindowDisable("PARTITION_WINDOW2", false);
}
