int nCurScrollNum = -1;
ref refCharacter = PChar;
string sMessageMode;

int iChangeCount = 2;

// LEO: открыта ли форма смены корабля. У формы установки такой признак уже есть - Guardians.InstallShip
// (0 = закрыта), а у этой не было. Нужен для Esc: он должен закрывать форму, а не весь интерфейс.
bool bChangeFrameOpen = false;

void InitInterface(string iniName)
{
    	GameInterface.title = "titleColonyGuarding";

    	FillShipsScroll();

    	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	PChar.Colony.Guardians.InstallShip = 0;
	PChar.Colony.Guardians.InstallShip.Character = 0;

	SetInformation();

	SetNodeUsing("FRAME_SHIP_INSTALL", false);
	SetNodeUsing("INSTALL_OK", false);
	SetNodeUsing("INSTALL_CANCEL", false);
	SetNodeUsing("SHIPS_SCROLL", false);
	SetNodeUsing("LEFTSCROLLBUTTON", false);
	SetNodeUsing("RIGHTSCROLLBUTTON", false);
	SetNodeUsing("TABLE_SHIP_INSTALL", false);
	SetNodeUsing("BORDERS_SHIP_INSTALL", false);
	SetNodeUsing("BORDERS_SHIP_INSTALL_1", false);

	SetNodeUsing("CHANGE_OK", false);
	SetNodeUsing("CHANGE_CANCEL", false);
	SetNodeUsing("CHANGE_LEFT", false);
	SetNodeUsing("CHANGE_RIGHT", false);
	SetNodeUsing("BORDERS_SHIP_CHANGE_LEFT", false);
	SetNodeUsing("BORDERS_SHIP_CHANGE_LEFT_1", false);
	SetNodeUsing("BORDERS_SHIP_CHANGE_RIGHT", false);
	SetNodeUsing("BORDERS_SHIP_CHANGE_RIGHT_1", false);
	SetNodeUsing("FRAME_SHIP_CHANGE", false);
	sMessageMode = "";

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("SetInstallInformation","SetInstallInformation",0);
	SetEventHandler("RefreshInstallInformationRight","RefreshInstallInformationRight",0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("ShipOneName","ShipOneName",0);
	SetEventHandler("ShipTwoName","ShipTwoName",0);
	SetEventHandler("ShipThreeName","ShipThreeName",0);
	SetEventHandler("ShipFourName","ShipFourName",0);
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;

	sPicture = "-1";
	switch (sCurrentNode)
	{
		case "SHIPS_SCROLL":
			string attributeName = GetPictureNumber();
			int iCharacter = sti(GameInterface.SHIPS_SCROLL.(attributeName).companionIndex);
			if(iCharacter != 0)
			{
				ref chr = &Characters[iCharacter];
				SetSPECIALMiniTable("TABLE_SMALLSKILL", chr);
				SetOTHERMiniTable("TABLE_SMALLOTHER", chr);
				SetFormatedText("OFFICER_NAME", GetFullName(chr));
				SetNewPicture("CHARACTER_BIG_PICTURE", "interfaces\portraits\face_" + chr.faceId + ".tga");
				SetNewPicture("CHARACTER_FRAME_PICTURE", "interfaces\Frame3.dds");

				XI_WindowShow("RPG_WINDOW", true);
				XI_WindowDisable("RPG_WINDOW", false);
				sMessageMode = "RPG_Hint";
			}
			break;
	}
}

void HideInfoWindow()
{
	CloseTooltip();
	if (sMessageMode == "RPG_Hint")
	{
		XI_WindowShow("RPG_WINDOW", false);
		XI_WindowDisable("RPG_WINDOW", true);
		sMessageMode = "";
	}
}

// LEO: подгонка таблицы ТТХ под УЗКИЕ панели слотов. SetShipOTHERTable рассчитан на просторные
//   строки экрана корабля: иконка 24x24 и шрифт 0.8. Здесь строка всего 20 пикселей на 220 в
//   ширину - иконка вылезала за строку, а длинные значения вроде "24 фунт / 26" упирались в
//   рамку. Зовётся ПОСЛЕ заполнения таблицы и ДО Table_UpdateWindow.
void CG_FitTableToPanel(string sTable, int iIconSize, float fScale)
{
	string sRow;
	int i;
	for (i = 1; i <= 10; i++)
	{
		sRow = "tr" + i;
		GameInterface.(sTable).(sRow).td1.icon.width = iIconSize;
		GameInterface.(sTable).(sRow).td1.icon.height = iIconSize;
		GameInterface.(sTable).(sRow).td2.scale = fScale;
		GameInterface.(sTable).(sRow).td3.scale = fScale;
	}
}

// LEO: суффикс группы портретов по стилю интерфейса. При включённой опционалке PotC картинки лежат в
//   отдельной папке, а группы в pictures.ini называются "SHIPS_<тип>_POTC". Без суффикса группа не
//   находится, и вместо корабля показывается заглушка - ровно этим и болел весь экран.
string CG_ShipPicSuffix()
{
	if (InterfaceStates.InterfaceShipString == "interfaces\ships_PotC\") return "_POTC";
	return "";
}

void FillShipsScroll()
{
	nCurScrollNum = -1;

	string sNodeName = "SHIPS_SCROLL";

	int iNotUsed = 6;

	DeleteAttribute(&GameInterface, sNodeName);
	GameInterface.(sNodeName).current = -1;

	GameInterface.(sNodeName).ImagesGroup.t0 = "BLANK_SHIP2";
	GameInterface.(sNodeName).BadTex1 = 0;
	GameInterface.(sNodeName).BadPic1 = "Not Used2";

	FillShipList(sNodeName + ".ImagesGroup", pchar);

	string attributeName, shipName;
	int iShipType, cn;
	int iListSize = 0;

	// LEO: суффикс стиля интерфейса. FillShipList выше добавляет группы как "SHIPS_<тип>_POTC", когда
	//   включена опционалка PotC (портреты лежат в отдельной папке SHIPS_PotC), а искали тут всегда без
	//   суффикса - группа не находилась, и ВСЕ миниатюры выходили серыми заглушками. Так делают все
	//   остальные экраны с этим скроллом (ship.c, shipyard.c, store.c) - здесь суффикс просто забыли.
	string potc = CG_ShipPicSuffix();

	for(int i= 0; i< COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);

		if(cn!= -1 && cn != nMainCharacterIndex && !CheckAttribute(&Characters[cn], "PGGAi"))
		{
			iShipType = sti(characters[cn].ship.type);
			if(iShipType != SHIP_NOTUSED)
			{
				iShipType = sti(RealShips[iShipType].basetype);

				if (iShipType!= SHIP_NOTUSED)
				{
					shipName = ShipsTypes[iShipType].Name;
					attributeName = "pic" + (iListSize+1);

					GameInterface.(sNodeName).(attributeName).companionIndex = cn;
					GameInterface.(sNodeName).(attributeName).img1 = "ship";
					GameInterface.(sNodeName).(attributeName).tex1 = FindFaceGroupNum(sNodeName + ".ImagesGroup","SHIPS_"+shipName+potc);

					iListSize++;
				}
			}
		}
	}

	GameInterface.(sNodeName).ListSize = iListSize;
	GameInterface.SHIPS_SCROLL.NotUsed = iNotUsed - iListSize + 1;

	SendMessage(&GameInterface, "lsl", MSG_INTERFACE_SCROLL_CHANGE, sNodeName, -1);

	if(!CheckAttribute(&GameInterface,"SHIPS_SCROLL.current"))
	{
		GameInterface.SHIPS_SCROLL.current = 0;
	}

}

void ShipOneName()
{
	Characters[sti(PChar.Colony.Guardians.Ship1)].ship.name = GameInterface.SHIP1_NAME.str;
	SetShipInformationOne();
}

void ShipTwoName()
{
	Characters[sti(PChar.Colony.Guardians.Ship2)].ship.name = GameInterface.SHIP2_NAME.str;
	SetShipInformationTwo();
}

void ShipThreeName()
{
	Characters[sti(PChar.Colony.Guardians.Ship3)].ship.name = GameInterface.SHIP3_NAME.str;
	SetShipInformationThree();
}

void ShipFourName()
{
	Characters[sti(PChar.Colony.Guardians.Ship4)].ship.name = GameInterface.SHIP4_NAME.str;
	SetShipInformationFour();
}

bool GetCompanionShipQuantity()
{
	ref PChar = GetMainCharacter();

	int cn, iShipType;

	if(GetCompanionQuantity(PChar) <= 1)
	{
		return false;
	}

	for(int i=0;i<COMPANION_MAX;i++)
	{
		cn = GetCompanionIndex(PChar, i);
		if(cn!= -1 && cn != nMainCharacterIndex && !CheckAttribute(&Characters[cn], "PGGAi"))
		{
			iShipType = sti(characters[cn].ship.type);
			if(iShipType != SHIP_NOTUSED)
			{
				return true;
			}
		}
	}

	return false;
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_SALARY_EXIT );
}

// LEO: Esc сперва закрывает ОТКРЫТУЮ ФОРМУ - ровно как кнопка "Отмена" - и только на голом главном
//   экране выходит из интерфейса. Решаем здесь, а не привязками в .ini: движок шлёт deactivate в
//   АКТИВНЫЙ узел, а при открытой форме активной остаётся кнопка главного экрана, которой форму и
//   открыли. По её привязке Esc закрывал весь интерфейс мимо формы.
void ProcessCancelExit()
{
	if (sti(PChar.Colony.Guardians.InstallShip) != 0) // открыта форма выбора корабля
	{
		PChar.Colony.Guardians.InstallShip = 0;
		PChar.Colony.Guardians.InstallShip.Character = 0;
		SetInstallFrame(true, false);
		SetInformation();
		return;
	}
	if (bChangeFrameOpen) // открыта форма смены корабля
	{
		SetChangeFrame(true, false);
		SetInformation();
		return;
	}
	IDoExit( RC_INTERFACE_SALARY_EXIT );
}

void IDoExit(int exitCode)
{
	// LEO: чистим теневые g3/g4 из старых сейвов. Это была копия слотов 3 и 4, которую функции ОТРИСОВКИ
	//   применяли обратно в Colony.Guardians.Ship3/Ship4 на каждой перерисовке. InstallShip ставил корабль,
	//   а следующий же SetInformation откатывал его к устаревшему значению - отсюда «в форт ничего не
	//   выставляется», и только в слотах 3 и 4: у первых двух такого дубля не было. Само состояние живёт
	//   в Colony.Guardians.*, дублировать его не нужно.
	DeleteAttribute(pchar, "g3");
	DeleteAttribute(pchar, "g4");
	DelEventHandler("SetInstallInformation","SetInstallInformation");
	DelEventHandler("RefreshInstallInformationRight","RefreshInstallInformationRight");
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("ShipOneName","ShipOneName");
	DelEventHandler("ShipTwoName","ShipTwoName");
	DelEventHandler("ShipThreeName","ShipThreeName");
	DelEventHandler("ShipFourName","ShipFourName");

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
		case "B_OK":
			if(comName=="activate" || comName=="click")
			{
				PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_SALARY_EXIT);
			}
		break;

		case "REPAIR_SHIP1":
			if(comName=="activate" || comName=="click")
			{
				RepairShip(sti(PChar.Colony.Guardians.Ship1));
			}
		break;

		case "REPAIR_SHIP2":
			if(comName=="activate" || comName=="click")
			{
				RepairShip(sti(PChar.Colony.Guardians.Ship2));
			}
		break;

		case "REPAIR_SHIP3":
			if(comName=="activate" || comName=="click")
			{
				RepairShip(sti(PChar.Colony.Guardians.Ship3));
			}
		break;

		case "REPAIR_SHIP4":
			if(comName=="activate" || comName=="click")
			{
				RepairShip(sti(PChar.Colony.Guardians.Ship4));
			}
		break;

		case "GUARD_YES_SHIP2":
			if(comName=="activate" || comName=="click")
			{
				GuardianInizialization(sti(PChar.Colony.Guardians.Ship2), 2, true);
			}
		break;

		case "GUARD_NOT_SHIP2":
			if(comName=="activate" || comName=="click")
			{
				GuardianInizialization(sti(PChar.Colony.Guardians.Ship2), 2, false);
			}
		break;

		case "GUARD_YES_SHIP3":
			if(comName=="activate" || comName=="click")
			{
				GuardianInizialization(sti(PChar.Colony.Guardians.Ship3), 3, true);
			}
		break;

		case "GUARD_NOT_SHIP3":
			if(comName=="activate" || comName=="click")
			{
				GuardianInizialization(sti(PChar.Colony.Guardians.Ship3), 3, false);
			}
		break;

		case "GUARD_YES_SHIP4":
			if(comName=="activate" || comName=="click")
			{
				GuardianInizialization(sti(PChar.Colony.Guardians.Ship4), 4, true);
			}
		break;

		case "GUARD_NOT_SHIP4":
			if(comName=="activate" || comName=="click")
			{
				GuardianInizialization(sti(PChar.Colony.Guardians.Ship4), 4, false);
			}
		break;

		case "INSTALL_SHIP1":
			if(comName=="activate" || comName=="click")
			{
				PChar.Colony.Guardians.InstallShip = 1;
				InstallShipFrame();
			}
		break;

		case "INSTALL_SHIP2":
			if(comName=="activate" || comName=="click")
			{
				PChar.Colony.Guardians.InstallShip = 2;
				InstallShipFrame();
			}
		break;

		case "INSTALL_SHIP3":
			if(comName=="activate" || comName=="click")
			{
				PChar.Colony.Guardians.InstallShip = 3;
				InstallShipFrame();
			}
		break;

		case "INSTALL_SHIP4":
			if(comName=="activate" || comName=="click")
			{
				PChar.Colony.Guardians.InstallShip = 4;
				InstallShipFrame();
			}
		break;

		case "REMOVE_SHIP1":
			if(comName=="activate" || comName=="click")
			{
				RemoveGuardian(1, sti(PChar.Colony.Guardians.Ship1));
			}
		break;

		case "REMOVE_SHIP2":
			if(comName=="activate" || comName=="click")
			{
				RemoveGuardian(2, sti(PChar.Colony.Guardians.Ship2));
			}
		break;

		case "REMOVE_SHIP3":
			if(comName=="activate" || comName=="click")
			{
				RemoveGuardian(3, sti(PChar.Colony.Guardians.Ship3));
			}
		break;

		case "REMOVE_SHIP4":
			if(comName=="activate" || comName=="click")
			{
				RemoveGuardian(4, sti(PChar.Colony.Guardians.Ship4));
			}
		break;

		case "LEFTSCROLLBUTTON":
			if(comName=="activate" || comName=="click")
			{
				CheckInstallInformation(true);
				PostEvent("Event_SwapScrollShip", 400);
			}
		break;

		case "RIGHTSCROLLBUTTON":
			if(comName=="activate" || comName=="click")
			{
				CheckInstallInformation(false);
				PostEvent("Event_SwapScrollShip", 400);
			}
		break;

		case "SHIPS_SCROLL":
			if(comName=="activate" || comName=="click")
			{
				SetInstallInformation(false);
				PostEvent("Event_SwapScrollShip", 400);
			}
		break;

		case "INSTALL_OK":
			if(comName=="activate" || comName=="click")
			{
				// LEO: страховка от установки "пустого" выбора. Индекс 0 - это САМ ГГ, а не корабль из
				//   списка: так проявлялся баг с невыставляемым кораблём. Молча уводить в форт главного
				//   героя нельзя ни при каких обстоятельствах, поэтому просто ничего не делаем.
				int iInstChr = sti(PChar.Colony.Guardians.InstallShip.Character);
				if (iInstChr > 0)
				{
					InstallShip(sti(PChar.Colony.Guardians.InstallShip), iInstChr);
				}
			}
		break;

		case "INSTALL_CANCEL":
			if(comName=="activate" || comName=="click")
			{
				PChar.Colony.Guardians.InstallShip = 0;
				PChar.Colony.Guardians.InstallShip.Character = 0;
				SetInstallFrame(true, false);
				SetInformation();
			}
		break;

		case "GUARD_CHANGE":
			if(comName=="activate" || comName=="click")
			{
				SetChangeFrame(false, true);
			}
		break;

		case "CHANGE_CANCEL":
			if(comName=="activate" || comName=="click")
			{
				SetChangeFrame(true, false);
				SetInformation();
			}
		break;

		case "CHANGE_LEFT":
			if(comName=="activate" || comName=="click")
			{
				CheckChangeShips(true);
			}
		break;

		case "CHANGE_RIGHT":
			if(comName=="activate" || comName=="click")
			{
				CheckChangeShips(false);
			}
		break;

		case "CHANGE_OK":
			if(comName=="activate" || comName=="click")
			{
				ChangeShipsOK();
			}
		break;
	}
}

#event_handler("Event_SwapScrollShip", "SwapScrollShip");
void SwapScrollShip()
{
	SetInstallInformation(false);
}

void RepairShip(int iCharacter)
{
	if(iCharacter == -1)
	{
		return;
	}

	ref chr = &Characters[iCharacter];

	chr.ship.hp = GetCharacterShipHP(chr);
	DeleteAttribute(chr, "ship.blots");

	chr.ship.sp = GetCharacterShipSP(chr);
	DeleteAttribute(chr, "ship.sails");
	RepairMasts(chr);

	SetInformation();
}

void RemoveGuardian(int iNum, int iCharacter)
{
	ref chr = &Characters[iCharacter];
	chr.nation = sti(chr.nation.pre);

	string sShip = "Ship" + iNum;
	PChar.Colony.Guardians.(sShip) = -1;
	PChar.Colony.Guardians.(sShip).Active = false;
	DeleteAttribute(pchar,"g"+iNum);
	GuardianInizialization(iCharacter, iNum, false);

	SetCharacterShipLocation(chr, "None");
	SetCompanionIndex(PChar, -1, iCharacter);

	if(CheckAttribute(chr, "AlwaysFriend")) 	{ DeleteAttribute(chr, "AlwaysFriend"); }
	if(CheckAttribute(chr, "ShipEnemyDisable")) 	{ DeleteAttribute(chr, "ShipEnemyDisable"); }
	if(CheckAttribute(chr, "DontCheckFlag")) 	{ DeleteAttribute(chr, "DontCheckFlag"); }

	ClearShipInfo(1);
	ClearShipInfo(2);
	ClearShipInfo(3);
	ClearShipInfo(4);

	string sGroup = "InterfaceColonyGuardians_" + iNum;
	if(Group_FindGroup(sGroup) != -1)
	{
		Group_DeleteGroup(sGroup);
	}

	RefreshShipsScroll();
	SetInformation();
}

void GuardianInizialization(int iCharacter, int iShip, bool bYes)
{
	if(iCharacter == -1)
	{
		return;
	}

	ref chr = &Characters[iCharacter];

	string sNodeNot, sNodeYes;
	string sShip = "Ship" + iShip;
	string sGroup = "InterfaceColonyGuardians_" + iShip;
	if(bYes)
	{
		if(Group_FindGroup(sGroup) != -1)
		{
			Group_SetAddress(sGroup, "Caiman", "quest_ships", "quest_ship_" + iShip);
		}

		sNodeNot = "GUARD_YES_SHIP" + iShip;
		sNodeYes = "GUARD_NOT_SHIP" + iShip;

		PChar.Colony.Guardians.(sShip).Active = true;
	}
	else
	{
		if(Group_FindGroup(sGroup) != -1)
		{
			Group_SetAddress(sGroup, "none", "none", "none");
		}

		sNodeNot = "GUARD_NOT_SHIP" + iShip;
		sNodeYes = "GUARD_YES_SHIP" + iShip;

		PChar.Colony.Guardians.(sShip).Active = false;

	}

	SetNodeUsing(sNodeNot, false);
	SetNodeUsing(sNodeYes, true);

}

void InstallShip(int iGuardian, int iCharacter)
{
	string sShip = "Ship" + iGuardian;
	PChar.Colony.Guardians.(sShip) = iCharacter;

	ref chr = &Characters[iCharacter];
	RemoveCharacterCompanion(PChar, chr);

	chr.nation.pre = sti(chr.nation);
	chr.nation = PIRATE;
	chr.ShipEnemyDisable = true;
	chr.CaimanGuardian = true;
	chr.DontCheckFlag = true;
	chr.DontRefresh = true;

	//14.05.2021 evil_bars: удаляем аттрибут, т.к. он проверяется в Addons/ColonyGuarding.c/CheckDeadColonyGuard() и удаляет корабли
	if(CheckAttribute(chr,"ship.shipsink")) { DeleteAttribute(chr,"ship.shipsink"); }

	chr.AlwaysFriend = true;
	chr.Ship.Mode = "war";

	SetCharacterRelationBoth(iCharacter, GetMainCharacterIndex(), RELATION_FRIEND);
	SetCharacterShipLocation(chr, "LandGuardingPort");

	string sGroup = "InterfaceColonyGuardians_" + iGuardian;
	Group_FindOrCreateGroup(sGroup);

	Group_SetType(sGroup, "war");
	Group_AddCharacter(sGroup, chr.id);
	Group_SetGroupCommander(sGroup, chr.id);
	GuardianInizialization(iCharacter, iGuardian, true);

	UpdateRelations();

	PChar.Colony.Guardians.InstallShip = 0;
	PChar.Colony.Guardians.InstallShip.Character = 0;

	RefreshShipsScroll();

	SetInstallFrame(true, false);
	SetInformation();
}

void SetInformation()
{
	CreateString(true,"ShipHeaderOne",GetConvertStrWithReplace("Variable_ColonyGuardiang_1", "Interface.txt", "#space#", " "), FONT_CAPTION, COLOR_NORMAL, 190,35,SCRIPT_ALIGN_CENTER,1.5);
	CreateString(true,"ShipHeaderTwo",GetConvertStrWithReplace("Variable_ColonyGuardiang_2", "Interface.txt", "#space#", " "), FONT_CAPTION, COLOR_NORMAL, 190,317,SCRIPT_ALIGN_CENTER,1.5);
	CreateString(true,"ShipHeaderThree",GetConvertStrWithReplace("Variable_ColonyGuardiang_3", "Interface.txt", "#space#", " "), FONT_CAPTION, COLOR_NORMAL, 595,35,SCRIPT_ALIGN_CENTER,1.5);
	CreateString(true,"ShipHeaderFour",GetConvertStrWithReplace("Variable_ColonyGuardiang_4", "Interface.txt", "#space#", " "), FONT_CAPTION, COLOR_NORMAL, 595,317,SCRIPT_ALIGN_CENTER,1.5);

	GameInterface.strings.ShipInstallFrameHeader = "";
	SetNodeUsing("FRAME_SHIP_INSTALL", false);
	SetNodeUsing("INSTALL_OK", false);
	SetNodeUsing("INSTALL_CANCEL", false);
	SetNodeUsing("GUARD_CHANGE", true);

	SetShipInformationOne();
	SetShipInformationTwo();
	SetShipInformationThree();
	SetShipInformationFour();

	int iShipOne = sti(PChar.Colony.Guardians.Ship1);
	int iShipTwo = sti(PChar.Colony.Guardians.Ship2);
	int iShipThree = sti(PChar.Colony.Guardians.Ship3);
	int iShipFour = sti(PChar.Colony.Guardians.Ship4);

	if(iShipOne == -1 && iShipTwo == -1 && iShipThree == -1 && iShipFour == -1)
	{
		SetSelectable("GUARD_CHANGE", false);
	}
	else
	{
		if(iShipOne != -1 && iShipTwo != -1)
		{
			SetSelectable("GUARD_CHANGE", true);
		}
		else
		{
			SetSelectable("GUARD_CHANGE", false);
		}
	}
}

void SetShipInformationOne()
{
	int iShipType = sti(PChar.Colony.Guardians.Ship1);

	if(iShipType == -1)
	{
		CreateImage("ShipType1", "BLANK_SHIP2", "Not Used2", 130,100,258,228);
		CreateString(true,"ShipOneNot",GetConvertStrWithReplace("Variable_ColonyGuardiang_5", "Interface.txt", "#space#", " "), FONT_NORMAL, COLOR_WARNING, 195,240,SCRIPT_ALIGN_CENTER,1.0);
		SetNodeUsing("REMOVE_SHIP1", false);
		SetNodeUsing("GUARD_NOT_SHIP1", false);
		SetNodeUsing("BORDERS_SHIP_1", false);
		SetNodeUsing("BORDERS_SHIP_1_1", false);
		SetNodeUsing("TABLE_SHIP_1", false);
		SetNodeUsing("SHIP1_NAME", false);
		SetSelectable("REPAIR_SHIP1", false);
		SetSelectable("GUARD_YES_SHIP1", false);

		SetNodeUsing("INSTALL_SHIP1", true);

		bool bCanInstall = GetCompanionShipQuantity();
		if(bCanInstall)
		{
			SetSelectable("INSTALL_SHIP1", true);
		}
		else
		{
			SetSelectable("INSTALL_SHIP1", false);
		}
	}
	else
	{
		int iCharacter = iShipType;
		ref chr = &Characters[iCharacter];

		ref rShip = GetRealShip(sti(chr.ship.type));
		string sShipType = ShipsTypes[sti(rShip.basetype)].name;
		string sShipName = chr.Ship.name;

		GameInterface.SHIP1_NAME.str = chr.Ship.name;

		int iShipClass = sti(rShip.class);

		CreateImage("ShipType1", "SHIPS_" + sShipType + CG_ShipPicSuffix(), "ship", 20,105,160,245);

		GameInterface.strings.ShipHeaderOne = XI_ConvertString(sShipType) + " - " + sShipName;

		CreateString(true,"ShipType1", XI_ConvertString(sShipType), FONT_CAPTION, COLOR_NORMAL, 90,249,SCRIPT_ALIGN_CENTER,1.0);
		CreateString(true,"ShipClass1", iShipClass, FONT_BOLD_NUMBERS, COLOR_NORMAL, 140,220,SCRIPT_ALIGN_LEFT,1.0);

		// LEO: ТТХ - СТАНДАРТНОЙ таблицей (SetShipOTHERTable), как на экране корабля.
		//   Раньше тут была ручная вёрстка: шесть строк по CreateImage и паре CreateString
		//   с прибитыми пикселями. Строк стало десять - прибавились мачты, бейдевинд, дедвейт.
		Table_Clear("TABLE_SHIP_1", false, true, false);
		SetShipOTHERTable("TABLE_SHIP_1", chr);
		CG_FitTableToPanel("TABLE_SHIP_1", 18, 0.8);
		Table_UpdateWindow("TABLE_SHIP_1");

		SetNodeUsing("INSTALL_SHIP1", false);
		SetNodeUsing("GUARD_NOT_SHIP1", false);
		SetSelectable("REPAIR_SHIP1", true);
		SetSelectable("GUARD_YES_SHIP1", true);

		if(sti(PChar.Colony.Guardians.Ship2) != -1 || GetCompanionQuantity(PChar) >= COMPANION_MAX)
		{
			SetSelectable("REMOVE_SHIP1", false);
		}
		else
		{
			SetSelectable("REMOVE_SHIP1", true);
		}

		if(PChar.Colony.Guardians.Ship1.Active == true)
		{
			SetNodeUsing("GUARD_NOT_SHIP1", true);
			SetNodeUsing("GUARD_YES_SHIP1", false);
		}
		else
		{
			SetNodeUsing("GUARD_NOT_SHIP1", false);
			SetNodeUsing("GUARD_YES_SHIP1", true);
		}
	}
}

void SetShipInformationTwo()
{
	int iShipType = sti(PChar.Colony.Guardians.Ship2);

	if(iShipType == -1)
	{
		CreateImage("ShipType2", "BLANK_SHIP2", "Not Used2", 130,383,258,511);
		CreateString(true,"ShipTwoNot",GetConvertStrWithReplace("Variable_ColonyGuardiang_11", "Interface.txt", "#space#", " "), FONT_NORMAL, COLOR_WARNING, 195,523,SCRIPT_ALIGN_CENTER,1.0);
		SetNodeUsing("REMOVE_SHIP2", false);
		SetNodeUsing("GUARD_NOT_SHIP2", false);
		SetNodeUsing("BORDERS_SHIP_2", false);
		SetNodeUsing("BORDERS_SHIP_2_1", false);
		SetNodeUsing("TABLE_SHIP_2", false);
		SetNodeUsing("SHIP2_NAME", false);
		SetSelectable("REPAIR_SHIP2", false);
		SetSelectable("GUARD_YES_SHIP2", false);

		SetNodeUsing("INSTALL_SHIP2", true);

		bool bCanInstall = GetCompanionShipQuantity();
		if(bCanInstall && sti(PChar.Colony.Guardians.Ship1) != -1)
		{
			SetSelectable("INSTALL_SHIP2", true);
		}
		else
		{
			SetSelectable("INSTALL_SHIP2", false);
		}
	}
	else
	{
		int iCharacter = iShipType;
		ref chr = &Characters[iCharacter];

		ref rShip = GetRealShip(sti(chr.ship.type));
		string sShipType = ShipsTypes[sti(rShip.basetype)].name;
		string sShipName = chr.Ship.name;

		GameInterface.SHIP2_NAME.str = chr.Ship.name;

		int iShipClass = sti(rShip.class);

		CreateImage("ShipType2", "SHIPS_" + sShipType + CG_ShipPicSuffix(), "ship", 20,388,160,528);

		GameInterface.strings.ShipHeaderTwo = XI_ConvertString(sShipType) + " - " + sShipName;

		CreateString(true,"ShipType2", XI_ConvertString(sShipType), FONT_CAPTION, COLOR_NORMAL, 90,532,SCRIPT_ALIGN_CENTER,1.0);
		CreateString(true,"ShipClass2", iShipClass, FONT_BOLD_NUMBERS, COLOR_NORMAL, 140,503,SCRIPT_ALIGN_LEFT,1.0);

		// LEO: ТТХ - СТАНДАРТНОЙ таблицей (SetShipOTHERTable), как на экране корабля.
		//   Раньше тут была ручная вёрстка: шесть строк по CreateImage и паре CreateString
		//   с прибитыми пикселями. Строк стало десять - прибавились мачты, бейдевинд, дедвейт.
		Table_Clear("TABLE_SHIP_2", false, true, false);
		SetShipOTHERTable("TABLE_SHIP_2", chr);
		CG_FitTableToPanel("TABLE_SHIP_2", 18, 0.8);
		Table_UpdateWindow("TABLE_SHIP_2");

		SetNodeUsing("INSTALL_SHIP2", false);
		SetNodeUsing("GUARD_NOT_SHIP2", false);
		SetSelectable("REPAIR_SHIP2", true);
		SetSelectable("GUARD_YES_SHIP2", true);

		if(sti(PChar.Colony.Guardians.Ship3) != -1 || GetCompanionQuantity(PChar) >= COMPANION_MAX)
		{
			SetSelectable("REMOVE_SHIP2", false);
		}
		else
		{
			SetSelectable("REMOVE_SHIP2", true);
		}

		if(PChar.Colony.Guardians.Ship2.Active == true)
		{
			SetNodeUsing("GUARD_NOT_SHIP2", true);
			SetNodeUsing("GUARD_YES_SHIP2", false);
		}
		else
		{
			SetNodeUsing("GUARD_NOT_SHIP2", false);
			SetNodeUsing("GUARD_YES_SHIP2", true);
		}
	}
}

void SetShipInformationThree()
{
	int iShipType = sti(PChar.Colony.Guardians.Ship3);
	if(iShipType == -1)
	{
		CreateImage("ShipType3", "BLANK_SHIP2", "Not Used2", 525,100,653,228);
		CreateString(true,"ShipThreeNot",GetConvertStrWithReplace("Variable_ColonyGuardiang_17", "Interface.txt", "#space#", " "), FONT_NORMAL, COLOR_WARNING, 590,240,SCRIPT_ALIGN_CENTER,1.0);
		SetNodeUsing("REMOVE_SHIP3", false);
		SetNodeUsing("GUARD_NOT_SHIP3", false);
		SetNodeUsing("BORDERS_SHIP_3", false);
		SetNodeUsing("BORDERS_SHIP_3_1", false);
		SetNodeUsing("TABLE_SHIP_3", false);
		SetNodeUsing("SHIP3_NAME", false);
		SetSelectable("REPAIR_SHIP3", false);
		SetSelectable("GUARD_YES_SHIP3", false);

		SetNodeUsing("INSTALL_SHIP3", true);

		bool bCanInstall = GetCompanionShipQuantity();
		if(bCanInstall && sti(PChar.Colony.Guardians.Ship2) != -1)
		{
			SetSelectable("INSTALL_SHIP3", true);
		}
		else
		{
			SetSelectable("INSTALL_SHIP3", false);
		}
	}
	else
	{
		int iCharacter = iShipType;
		ref chr = &Characters[iCharacter];

		ref rShip = GetRealShip(sti(chr.ship.type));
		string sShipType = ShipsTypes[sti(rShip.basetype)].name;
		string sShipName = chr.Ship.name;

		GameInterface.SHIP3_NAME.str = chr.Ship.name;

		int iShipClass = sti(rShip.class);

		CreateImage("ShipType3", "SHIPS_" + sShipType + CG_ShipPicSuffix(), "ship", 415,105,555,245);

		GameInterface.strings.ShipHeaderThree = XI_ConvertString(sShipType) + " - " + sShipName;

		CreateString(true,"ShipType3", XI_ConvertString(sShipType), FONT_CAPTION, COLOR_NORMAL, 485,249,SCRIPT_ALIGN_CENTER,1.0);
		CreateString(true,"ShipClass3", iShipClass, FONT_BOLD_NUMBERS, COLOR_NORMAL, 535,220,SCRIPT_ALIGN_LEFT,1.0);

		// LEO: ТТХ - СТАНДАРТНОЙ таблицей (SetShipOTHERTable), как на экране корабля.
		//   Раньше тут была ручная вёрстка: шесть строк по CreateImage и паре CreateString
		//   с прибитыми пикселями. Строк стало десять - прибавились мачты, бейдевинд, дедвейт.
		Table_Clear("TABLE_SHIP_3", false, true, false);
		SetShipOTHERTable("TABLE_SHIP_3", chr);
		CG_FitTableToPanel("TABLE_SHIP_3", 18, 0.8);
		Table_UpdateWindow("TABLE_SHIP_3");

		SetNodeUsing("INSTALL_SHIP3", false);
		SetNodeUsing("GUARD_NOT_SHIP3", false);
		SetSelectable("REPAIR_SHIP3", true);
		SetSelectable("GUARD_YES_SHIP3", true);

		if(sti(PChar.Colony.Guardians.Ship4) != -1 || GetCompanionQuantity(PChar) >= COMPANION_MAX)
		{
			SetSelectable("REMOVE_SHIP3", false);
		}
		else
		{
			SetSelectable("REMOVE_SHIP3", true);
		}

		if(PChar.Colony.Guardians.Ship3.Active == true)
		{
			SetNodeUsing("GUARD_NOT_SHIP3", true);
			SetNodeUsing("GUARD_YES_SHIP3", false);
		}
		else
		{
			SetNodeUsing("GUARD_NOT_SHIP3", false);
			SetNodeUsing("GUARD_YES_SHIP3", true);
		}
	}
}

void SetShipInformationFour()
{
	int iShipType = sti(PChar.Colony.Guardians.Ship4);
	if(iShipType == -1)
	{
		CreateImage("ShipType4", "BLANK_SHIP2", "Not Used2", 525,383,653,511);
		CreateString(true,"ShipFourNot",GetConvertStrWithReplace("Variable_ColonyGuardiang_23", "Interface.txt", "#space#", " "), FONT_NORMAL, COLOR_WARNING, 590,523,SCRIPT_ALIGN_CENTER,1.0);
		SetNodeUsing("REMOVE_SHIP4", false);
		SetNodeUsing("GUARD_NOT_SHIP4", false);
		SetNodeUsing("BORDERS_SHIP_4", false);
		SetNodeUsing("BORDERS_SHIP_4_1", false);
		SetNodeUsing("TABLE_SHIP_4", false);
		SetNodeUsing("SHIP4_NAME", false);
		SetSelectable("REPAIR_SHIP4", false);
		SetSelectable("GUARD_YES_SHIP4", false);

		SetNodeUsing("INSTALL_SHIP4", true);

		bool bCanInstall = GetCompanionShipQuantity();
		if(bCanInstall && sti(PChar.Colony.Guardians.Ship3) != -1)
		{
			SetSelectable("INSTALL_SHIP4", true);
		}
		else
		{
			SetSelectable("INSTALL_SHIP4", false);
		}
	}
	else
	{
		int iCharacter = iShipType;
		ref chr = &Characters[iCharacter];

		ref rShip = GetRealShip(sti(chr.ship.type));
		string sShipType = ShipsTypes[sti(rShip.basetype)].name;
		string sShipName = chr.Ship.name;

		GameInterface.SHIP4_NAME.str = chr.Ship.name;

		int iShipClass = sti(rShip.class);

		CreateImage("ShipType4", "SHIPS_" + sShipType + CG_ShipPicSuffix(), "ship", 415,388,555,528);

		GameInterface.strings.ShipHeaderFour = XI_ConvertString(sShipType) + " - " + sShipName;

		CreateString(true,"ShipType4", XI_ConvertString(sShipType), FONT_CAPTION, COLOR_NORMAL, 485,532,SCRIPT_ALIGN_CENTER,1.0);
		CreateString(true,"ShipClass4", iShipClass, FONT_BOLD_NUMBERS, COLOR_NORMAL, 535,503,SCRIPT_ALIGN_LEFT,1.0);

		// LEO: ТТХ - СТАНДАРТНОЙ таблицей (SetShipOTHERTable), как на экране корабля.
		//   Раньше тут была ручная вёрстка: шесть строк по CreateImage и паре CreateString
		//   с прибитыми пикселями. Строк стало десять - прибавились мачты, бейдевинд, дедвейт.
		Table_Clear("TABLE_SHIP_4", false, true, false);
		SetShipOTHERTable("TABLE_SHIP_4", chr);
		CG_FitTableToPanel("TABLE_SHIP_4", 18, 0.8);
		Table_UpdateWindow("TABLE_SHIP_4");

		SetNodeUsing("INSTALL_SHIP4", false);
		SetNodeUsing("GUARD_NOT_SHIP4", false);
		SetSelectable("REPAIR_SHIP4", true);
		SetSelectable("GUARD_YES_SHIP4", true);

		if(GetCompanionQuantity(PChar) >= COMPANION_MAX)
		{
			SetSelectable("REMOVE_SHIP4", false);
		}
		else
		{
			SetSelectable("REMOVE_SHIP4", true);
		}

		if(PChar.Colony.Guardians.Ship4.Active == true)
		{
			SetNodeUsing("GUARD_NOT_SHIP4", true);
			SetNodeUsing("GUARD_YES_SHIP4", false);
		}
		else
		{
			SetNodeUsing("GUARD_NOT_SHIP4", false);
			SetNodeUsing("GUARD_YES_SHIP4", true);
		}
	}
}

void SetInstallFrame(bool bInstall, bool bContinue)
{
	SetNodeUsing("FRAME_SHIP_1", bInstall);
	SetNodeUsing("FRAME_SHIP_2", bInstall);
	SetNodeUsing("FRAME_SHIP_3", bInstall);
	SetNodeUsing("FRAME_SHIP_4", bInstall);

	SetNodeUsing("TABLE_SHIP_1", bInstall);
	SetNodeUsing("INSTALL_SHIP1", bInstall);
	SetNodeUsing("REMOVE_SHIP1", bInstall);
	SetNodeUsing("REPAIR_SHIP1", bInstall);
	SetNodeUsing("GUARD_NOT_SHIP1", bInstall);
	SetNodeUsing("GUARD_YES_SHIP1", bInstall);
	SetNodeUsing("BORDERS_SHIP_1", bInstall);
	SetNodeUsing("BORDERS_SHIP_1_1", bInstall);
	SetNodeUsing("SHIP1_NAME", bInstall);

	SetNodeUsing("TABLE_SHIP_2", bInstall);
	SetNodeUsing("INSTALL_SHIP2", bInstall);
	SetNodeUsing("REMOVE_SHIP2", bInstall);
	SetNodeUsing("REPAIR_SHIP2", bInstall);
	SetNodeUsing("GUARD_NOT_SHIP2", bInstall);
	SetNodeUsing("GUARD_YES_SHIP2", bInstall);
	SetNodeUsing("BORDERS_SHIP_2", bInstall);
	SetNodeUsing("BORDERS_SHIP_2_1", bInstall);
	SetNodeUsing("SHIP2_NAME", bInstall);

	SetNodeUsing("TABLE_SHIP_3", bInstall);
	SetNodeUsing("INSTALL_SHIP3", bInstall);
	SetNodeUsing("REMOVE_SHIP3", bInstall);
	SetNodeUsing("REPAIR_SHIP3", bInstall);
	SetNodeUsing("GUARD_NOT_SHIP3", bInstall);
	SetNodeUsing("GUARD_YES_SHIP3", bInstall);
	SetNodeUsing("BORDERS_SHIP_3", bInstall);
	SetNodeUsing("BORDERS_SHIP_3_1", bInstall);
	SetNodeUsing("SHIP3_NAME", bInstall);

	SetNodeUsing("TABLE_SHIP_4", bInstall);
	SetNodeUsing("INSTALL_SHIP4", bInstall);
	SetNodeUsing("REMOVE_SHIP4", bInstall);
	SetNodeUsing("REPAIR_SHIP4", bInstall);
	SetNodeUsing("GUARD_NOT_SHIP4", bInstall);
	SetNodeUsing("GUARD_YES_SHIP4", bInstall);
	SetNodeUsing("BORDERS_SHIP_4", bInstall);
	SetNodeUsing("BORDERS_SHIP_4_1", bInstall);
	SetNodeUsing("SHIP4_NAME", bInstall);

	if(!bContinue)
	{

		GameInterface.pictures.ShipTypeInstall.pic = "";
		GameInterface.pictures.BackgroundShipTypeInstall.pic = "";
		GameInterface.pictures.BackgroundShipNameInstall.pic = "";
		GameInterface.pictures.ShipHpInstall.pic = "";
		GameInterface.pictures.ShipSpInstall.pic = "";
		GameInterface.pictures.ShipSrInstall.pic = "";
		GameInterface.pictures.ShipTrInstall.pic = "";
		GameInterface.pictures.ShipCrewInstall.pic = "";
		GameInterface.pictures.ShipCannonsInstall.pic = "";
		GameInterface.pictures.ShipCannonsTypeInstall.pic = "";

		GameInterface.strings.ShipNameInstall = "";
		GameInterface.strings.ShipTypeInstall = "";
		GameInterface.strings.ShipClassInstall = "";
		GameInterface.strings.ShipHpInstall = "";
		GameInterface.strings.ShipHpQuantityInstall = "";
		GameInterface.strings.ShipSpInstall = "";
		GameInterface.strings.ShipSpQuantityInstall = "";
		GameInterface.strings.ShipSrInstall = "";
		GameInterface.strings.ShipSrQuantityInstall = "";
		GameInterface.strings.ShipTrInstall = "";
		GameInterface.strings.ShipTrQuantityInstall = "";
		GameInterface.strings.ShipCrewInstall = "";
		GameInterface.strings.ShipCrewQuantityInstall = "";
		GameInterface.strings.ShipCannonsInstall = "";
		GameInterface.strings.ShipCannonsQuantityInstall = "";
		GameInterface.strings.ShipCannonsQuantityInstall = "";
		GameInterface.strings.ShipCannonsTypeInstall = "";
		GameInterface.strings.ShipCannonsTypeInstall1 = "";

		SetNodeUsing("FRAME_SHIP_INSTALL", false);
		SetNodeUsing("INSTALL_OK", false);
		SetNodeUsing("INSTALL_CANCEL", false);
		SetNodeUsing("SHIPS_SCROLL", false);
		SetNodeUsing("LEFTSCROLLBUTTON", false);
		SetNodeUsing("RIGHTSCROLLBUTTON", false);
		SetNodeUsing("TABLE_SHIP_INSTALL", false);
		SetNodeUsing("BORDERS_SHIP_INSTALL", false);
		SetNodeUsing("BORDERS_SHIP_INSTALL_1", false);
	}

	if(bContinue)
	{
		GameInterface.pictures.ShipType1.pic = "";
		GameInterface.pictures.BackgroundShipName1.pic = "";
		GameInterface.pictures.BackgroundShipType1.pic = "";
		GameInterface.pictures.ShipHp1.pic = "";
		GameInterface.pictures.ShipSp1.pic = "";
		GameInterface.pictures.ShipSr1.pic = "";
		GameInterface.pictures.ShipTr1.pic = "";
		GameInterface.pictures.ShipCrew1.pic = "";
		GameInterface.pictures.ShipCannons1.pic = "";

		GameInterface.strings.ShipOneNot = "";
		GameInterface.strings.ShipHeaderOne = "";
		GameInterface.strings.ShipName1 = "";
		GameInterface.strings.ShipType1 = "";
		GameInterface.strings.ShipClass1 = "";
		GameInterface.strings.ShipHp1 = "";
		GameInterface.strings.ShipHpQuantity1 = "";
		GameInterface.strings.ShipSp1 = "";
		GameInterface.strings.ShipSpQuantity1 = "";
		GameInterface.strings.ShipSr1 = "";
		GameInterface.strings.ShipSrQuantity1 = "";
		GameInterface.strings.ShipTr1 = "";
		GameInterface.strings.ShipTrQuantity1 = "";
		GameInterface.strings.ShipCrew1 = "";
		GameInterface.strings.ShipCrewQuantity1 = "";
		GameInterface.strings.ShipCannons1 = "";
		GameInterface.strings.ShipCannonsQuantity1 = "";

		GameInterface.pictures.ShipType2.pic = "";
		GameInterface.pictures.BackgroundShipName2.pic = "";
		GameInterface.pictures.BackgroundShipType2.pic = "";
		GameInterface.pictures.ShipHp2.pic = "";
		GameInterface.pictures.ShipSp2.pic = "";
		GameInterface.pictures.ShipSr2.pic = "";
		GameInterface.pictures.ShipTr2.pic = "";
		GameInterface.pictures.ShipCrew2.pic = "";
		GameInterface.pictures.ShipCannons2.pic = "";

		GameInterface.strings.ShipTwoNot = "";
		GameInterface.strings.ShipHeaderTwo = "";
		GameInterface.strings.ShipName2 = "";
		GameInterface.strings.ShipType2 = "";
		GameInterface.strings.ShipClass2 = "";
		GameInterface.strings.ShipHp2 = "";
		GameInterface.strings.ShipHpQuantity2 = "";
		GameInterface.strings.ShipSp2 = "";
		GameInterface.strings.ShipSpQuantity2 = "";
		GameInterface.strings.ShipSr2 = "";
		GameInterface.strings.ShipSrQuantity2 = "";
		GameInterface.strings.ShipTr2 = "";
		GameInterface.strings.ShipTrQuantity2 = "";
		GameInterface.strings.ShipCrew2 = "";
		GameInterface.strings.ShipCrewQuantity2 = "";
		GameInterface.strings.ShipCannons2 = "";
		GameInterface.strings.ShipCannonsQuantity2 = "";

		GameInterface.pictures.ShipType3.pic = "";
		GameInterface.pictures.BackgroundShipName3.pic = "";
		GameInterface.pictures.BackgroundShipType3.pic = "";
		GameInterface.pictures.ShipHp3.pic = "";
		GameInterface.pictures.ShipSp3.pic = "";
		GameInterface.pictures.ShipSr3.pic = "";
		GameInterface.pictures.ShipTr3.pic = "";
		GameInterface.pictures.ShipCrew3.pic = "";
		GameInterface.pictures.ShipCannons3.pic = "";

		GameInterface.strings.ShipThreeNot = "";
		GameInterface.strings.ShipHeaderThree = "";
		GameInterface.strings.ShipName3 = "";
		GameInterface.strings.ShipType3 = "";
		GameInterface.strings.ShipClass3 = "";
		GameInterface.strings.ShipHp3 = "";
		GameInterface.strings.ShipHpQuantity3 = "";
		GameInterface.strings.ShipSp3 = "";
		GameInterface.strings.ShipSpQuantity3 = "";
		GameInterface.strings.ShipSr3 = "";
		GameInterface.strings.ShipSrQuantity3 = "";
		GameInterface.strings.ShipTr3 = "";
		GameInterface.strings.ShipTrQuantity3 = "";
		GameInterface.strings.ShipCrew3 = "";
		GameInterface.strings.ShipCrewQuantity3 = "";
		GameInterface.strings.ShipCannons3 = "";
		GameInterface.strings.ShipCannonsQuantity3 = "";

		GameInterface.pictures.ShipType4.pic = "";
		GameInterface.pictures.BackgroundShipName4.pic = "";
		GameInterface.pictures.BackgroundShipType4.pic = "";
		GameInterface.pictures.ShipHp4.pic = "";
		GameInterface.pictures.ShipSp4.pic = "";
		GameInterface.pictures.ShipSr4.pic = "";
		GameInterface.pictures.ShipTr4.pic = "";
		GameInterface.pictures.ShipCrew4.pic = "";
		GameInterface.pictures.ShipCannons4.pic = "";

		GameInterface.strings.ShipFourNot = "";
		GameInterface.strings.ShipHeaderFour = "";
		GameInterface.strings.ShipName4 = "";
		GameInterface.strings.ShipType4 = "";
		GameInterface.strings.ShipClass4 = "";
		GameInterface.strings.ShipHp4 = "";
		GameInterface.strings.ShipHpQuantity4 = "";
		GameInterface.strings.ShipSp4 = "";
		GameInterface.strings.ShipSpQuantity4 = "";
		GameInterface.strings.ShipSr4 = "";
		GameInterface.strings.ShipSrQuantity4 = "";
		GameInterface.strings.ShipTr4 = "";
		GameInterface.strings.ShipTrQuantity4 = "";
		GameInterface.strings.ShipCrew4 = "";
		GameInterface.strings.ShipCrewQuantity4 = "";
		GameInterface.strings.ShipCannons4 = "";
		GameInterface.strings.ShipCannonsQuantity4 = "";

		CreateString(true,"ShipInstallFrameHeader",GetConvertStrWithReplace("Variable_ColonyGuardiang_29", "Interface.txt", "#space#", " "), FONT_CAPTION, COLOR_NORMAL, 395,78,SCRIPT_ALIGN_CENTER,1.5);
		SetNodeUsing("FRAME_SHIP_INSTALL", true);
		SetNodeUsing("INSTALL_OK", true);
		SetNodeUsing("INSTALL_CANCEL", true);
		SetSelectable("INSTALL_OK", true);

	}
}

string GetPictureNumber()
{
	nCurScrollNum = sti(GameInterface.SHIPS_SCROLL.current);
	// LEO: пока список не прокрутили, current остаётся -1 (его так и выставляет FillShipsScroll, а
	//   тамошняя страховка не срабатывает - атрибут-то существует). Отсюда получался "pic0", которого
	//   нет: узлы нумеруются с pic1. companionIndex не читался, iCharacter выходил 0, и в форт уезжал
	//   персонаж №0, то есть сам ГГ - корабль не вставал. Стоило покрутить список, current становился
	//   валидным и всё работало: отсюда и "баг то есть, то нету".
	if (nCurScrollNum < 0) nCurScrollNum = 0;
	string sPicture = "pic" + (nCurScrollNum+1);

	return sPicture;
}

void CheckInstallInformation(bool left)
{
	SetInstallInformation(1);
}

void RefreshInstallInformationRight()
{
	SetInstallInformation(1);
}

void SetInstallInformation(bool bBool)
{
	string sPicture = GetPictureNumber();

	SetNodeUsing("TABLE_SHIP_INSTALL", true);
	SetNodeUsing("BORDERS_SHIP_INSTALL", true);
	SetNodeUsing("BORDERS_SHIP_INSTALL_1", true);

	int iCharacter = sti(GameInterface.SHIPS_SCROLL.(sPicture).companionIndex);

	PChar.Colony.Guardians.InstallShip.Character = iCharacter;

	ref chr = &Characters[iCharacter];
	ref rShip = GetRealShip(sti(chr.ship.type));

	string sShipType = ShipsTypes[sti(rShip.basetype)].name;
	string sShipName = chr.Ship.name;
	int iShipClass = sti(rShip.class);

	CreateImage("ShipTypeInstall", "SHIPS_" + sShipType + CG_ShipPicSuffix(), "ship", 116,284,306,474);

	CreateString(true,"ShipNameInstall", sShipName, FONT_CAPTION, COLOR_NORMAL, 210,257,SCRIPT_ALIGN_CENTER,1.2);

	CreateString(true,"ShipTypeInstall", XI_ConvertString(sShipType), FONT_CAPTION, COLOR_NORMAL, 210,477,SCRIPT_ALIGN_CENTER,1.2);
	CreateString(true,"ShipClassInstall", iShipClass, FONT_BOLD_NUMBERS, COLOR_NORMAL, 280,450,SCRIPT_ALIGN_LEFT,1.0);

	// LEO: ТТХ - СТАНДАРТНОЙ таблицей, той же, что на экране корабля и в судовом журнале. Раньше тут
	//   лежало семь строк ручной вёрстки: на каждую CreateImage и пара CreateString с прибитыми
	//   пикселями, и вся эта простыня дублировалась по экрану ещё в шести местах. Заодно строк стало
	//   десять вместо семи - добавились прочность мачт, бейдевинд и дедвейт.
	Table_Clear("TABLE_SHIP_INSTALL", false, true, false);
	SetShipOTHERTable("TABLE_SHIP_INSTALL", chr);
	Table_UpdateWindow("TABLE_SHIP_INSTALL");
}

void InstallShipFrame()
{
	SetInstallFrame(false, true);

	SetNodeUsing("FRAME_SHIP_INSTALL", true);
	SetNodeUsing("INSTALL_OK", true);
	SetNodeUsing("INSTALL_CANCEL", true);
	SetNodeUsing("SHIPS_SCROLL", true);
	SetNodeUsing("LEFTSCROLLBUTTON", true);
	SetNodeUsing("RIGHTSCROLLBUTTON", true);
	SetNodeUsing("GUARD_CHANGE", false);

	SetInstallInformation(1);
}

string GetCannonShip(int iCannonsType)
{
	string sCannon = "";

	switch(iCannonsType)
	{
		case CANNON_TYPE_CANNON_LBS8: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_38", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS10: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_39", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS12: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_40", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS16: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_41", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS20: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_42", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS24: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_43", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS28: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_44", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS32: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_45", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS36: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_46", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS42: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_47", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CANNON_LBS48: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_48", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS8: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_49", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS10: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_50", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS12: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_51", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS16: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_52", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS20: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_53", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS24: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_54", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS28: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_55", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS32: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_56", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_CULVERINE_LBS36: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_57", "Interface.txt", "#space#", " "); break;
		case CANNON_TYPE_NONECANNON: sCannon = GetConvertStrWithReplace("Variable_ColonyGuardiang_58", "Interface.txt", "#space#", " "); break;
	}
	return sCannon;
}

string GetCannonTypeCaliber(int iCannonsType)
{
	string sCaliber = "";
	switch(iCannonsType)
	{
		case CANNON_TYPE_CANNON_LBS8: sCaliber = "8."; break;
		case CANNON_TYPE_CANNON_LBS10: sCaliber = "10."; break;
		case CANNON_TYPE_CANNON_LBS12: sCaliber = "12."; break;
		case CANNON_TYPE_CANNON_LBS16: sCaliber = "16."; break;
		case CANNON_TYPE_CANNON_LBS20: sCaliber = "20."; break;
		case CANNON_TYPE_CANNON_LBS24: sCaliber = "24."; break;
		case CANNON_TYPE_CANNON_LBS28: sCaliber = "28."; break;
		case CANNON_TYPE_CANNON_LBS32: sCaliber = "32."; break;
		case CANNON_TYPE_CANNON_LBS36: sCaliber = "36."; break;
		case CANNON_TYPE_CANNON_LBS42: sCaliber = "42."; break;
		case CANNON_TYPE_CANNON_LBS48: sCaliber = "48."; break;
		case CANNON_TYPE_CULVERINE_LBS8: sCaliber = "8."; break;
		case CANNON_TYPE_CULVERINE_LBS10: sCaliber = "10."; break;
		case CANNON_TYPE_CULVERINE_LBS12: sCaliber = "12."; break;
		case CANNON_TYPE_CULVERINE_LBS16: sCaliber = "16."; break;
		case CANNON_TYPE_CULVERINE_LBS20: sCaliber = "20."; break;
		case CANNON_TYPE_CULVERINE_LBS24: sCaliber = "24."; break;
		case CANNON_TYPE_CULVERINE_LBS28: sCaliber = "28."; break;
		case CANNON_TYPE_CULVERINE_LBS32: sCaliber = "32."; break;
		case CANNON_TYPE_CULVERINE_LBS36: sCaliber = "36."; break;
		case CANNON_TYPE_NONECANNON: sCaliber = "0."; break;
	}
	return sCaliber;
}

void ClearShipInfo(int iNum)
{
	string sNot = "";
	string sHeader = "";

	switch(iNum)
	{
		case 1: sNot = "ShipOneNot"; sHeader = "ShipHeaderOne"; break;
		case 2: sNot = "ShipTwoNot"; sHeader = "ShipHeaderTwo"; break;
		case 3: sNot = "ShipThreeNot"; sHeader = "ShipHeaderThree"; break;
		case 4: sNot = "ShipFourNot"; sHeader = "ShipHeaderFour"; break;
	}

	string sShipType = "ShipType" + iNum;
	string sShipName = "sShipName" + iNum;
	string sBackgroundShipName = "BackgroundShipName" + iNum;
	string sBackgroundShipType = "BackgroundShipType" + iNum;
	string sShipHp = "ShipHp" + iNum;
	string sShipSp = "ShipSp" + iNum;
	string sShipSr = "ShipSr" + iNum;
	string sShipTr = "ShipTr" + iNum;
	string sShipCrew = "ShipCrew" + iNum;
	string sShipCannons = "ShipCannons" + iNum;
	string sShipClass = "ShipClass" + iNum;
	string sShipHpQuantity = "ShipHpQuantity" + iNum;
	string sShipSpQuantity = "ShipSpQuantity" + iNum;
	string sShipSrQuantity = "ShipSrQuantity" + iNum;
	string sShipTrQuantity = "ShipTrQuantity" + iNum;
	string sShipCrewQuantity = "ShipCrewQuantity" + iNum;
	string sShipCannonsQuantity = "ShipCannonsQuantity" + iNum;

	GameInterface.pictures.(sShipType).pic = "";
	GameInterface.pictures.(sShipName).pic = "";
	GameInterface.pictures.(sBackgroundShipName).pic = "";
	GameInterface.pictures.(sBackgroundShipType).pic = "";
	GameInterface.pictures.(sShipHp).pic = "";
	GameInterface.pictures.(sShipSp).pic = "";
	GameInterface.pictures.(sShipSr).pic = "";
	GameInterface.pictures.(sShipTr).pic = "";
	GameInterface.pictures.(sShipCrew).pic = "";
	GameInterface.pictures.(sShipCannons).pic = "";

	GameInterface.strings.(sNot) = "";
	GameInterface.strings.(sHeader) = "";
	GameInterface.strings.(sShipName) = "";
	GameInterface.strings.(sShipType) = "";
	GameInterface.strings.(sShipClass) = "";
	GameInterface.strings.(sShipHp) = "";
	GameInterface.strings.(sShipHpQuantity) = "";
	GameInterface.strings.(sShipSp) = "";
	GameInterface.strings.(sShipSpQuantity) = "";
	GameInterface.strings.(sShipSr) = "";
	GameInterface.strings.(sShipSrQuantity) = "";
	GameInterface.strings.(sShipTr) = "";
	GameInterface.strings.(sShipTrQuantity) = "";
	GameInterface.strings.(sShipCrew) = "";
	GameInterface.strings.(sShipCrewQuantity) = "";
	GameInterface.strings.(sShipCannons) = "";
	GameInterface.strings.(sShipCannonsQuantity) = "";
}

void RefreshShipsScroll()
{
	FillShipsScroll();

	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"SHIPS_SCROLL",-1);
}

void SetChangeFrame(bool bInstall, bool bContinue)
{
	// LEO: одна точка учёта - bInstall=false открывает форму смены, true возвращает главный экран.
	bChangeFrameOpen = !bInstall;
	SetNodeUsing("FRAME_SHIP_1", bInstall);
	SetNodeUsing("FRAME_SHIP_2", bInstall);
	SetNodeUsing("FRAME_SHIP_3", bInstall);
	SetNodeUsing("FRAME_SHIP_4", bInstall);

	SetNodeUsing("TABLE_SHIP_1", bInstall);
	SetNodeUsing("INSTALL_SHIP1", bInstall);
	SetNodeUsing("REMOVE_SHIP1", bInstall);
	SetNodeUsing("REPAIR_SHIP1", bInstall);
	SetNodeUsing("GUARD_NOT_SHIP1", bInstall);
	SetNodeUsing("GUARD_YES_SHIP1", bInstall);
	SetNodeUsing("BORDERS_SHIP_1", bInstall);
	SetNodeUsing("BORDERS_SHIP_1_1", bInstall);
	SetNodeUsing("SHIP1_NAME", bInstall);

	SetNodeUsing("TABLE_SHIP_2", bInstall);
	SetNodeUsing("INSTALL_SHIP2", bInstall);
	SetNodeUsing("REMOVE_SHIP2", bInstall);
	SetNodeUsing("REPAIR_SHIP2", bInstall);
	SetNodeUsing("GUARD_NOT_SHIP2", bInstall);
	SetNodeUsing("GUARD_YES_SHIP2", bInstall);
	SetNodeUsing("BORDERS_SHIP_2", bInstall);
	SetNodeUsing("BORDERS_SHIP_2_1", bInstall);
	SetNodeUsing("SHIP2_NAME", bInstall);

	SetNodeUsing("TABLE_SHIP_3", bInstall);
	SetNodeUsing("INSTALL_SHIP3", bInstall);
	SetNodeUsing("REMOVE_SHIP3", bInstall);
	SetNodeUsing("REPAIR_SHIP3", bInstall);
	SetNodeUsing("GUARD_NOT_SHIP3", bInstall);
	SetNodeUsing("GUARD_YES_SHIP3", bInstall);
	SetNodeUsing("BORDERS_SHIP_3", bInstall);
	SetNodeUsing("BORDERS_SHIP_3_1", bInstall);
	SetNodeUsing("SHIP3_NAME", bInstall);

	SetNodeUsing("TABLE_SHIP_4", bInstall);
	SetNodeUsing("INSTALL_SHIP4", bInstall);
	SetNodeUsing("REMOVE_SHIP4", bInstall);
	SetNodeUsing("REPAIR_SHIP4", bInstall);
	SetNodeUsing("GUARD_NOT_SHIP4", bInstall);
	SetNodeUsing("GUARD_YES_SHIP4", bInstall);
	SetNodeUsing("BORDERS_SHIP_4", bInstall);
	SetNodeUsing("BORDERS_SHIP_4_1", bInstall);
	SetNodeUsing("SHIP4_NAME", bInstall);
	SetNodeUsing("GUARD_CHANGE", false);

	if(!bContinue)
	{

		GameInterface.pictures.ShipTypeInstall.pic = "";
		GameInterface.pictures.BackgroundShipTypeInstall.pic = "";
		GameInterface.pictures.BackgroundShipNameInstall.pic = "";
		GameInterface.pictures.ShipHpInstall.pic = "";
		GameInterface.pictures.ShipSpInstall.pic = "";
		GameInterface.pictures.ShipSrInstall.pic = "";
		GameInterface.pictures.ShipTrInstall.pic = "";
		GameInterface.pictures.ShipCrewInstall.pic = "";
		GameInterface.pictures.ShipCannonsInstall.pic = "";
		GameInterface.pictures.ShipCannonsTypeInstall.pic = "";

		GameInterface.strings.ShipChangeFrameHeader = "";
		GameInterface.strings.ShipTypeInstall = "";
		GameInterface.strings.ShipClassInstall = "";
		GameInterface.strings.ShipHpInstall = "";
		GameInterface.strings.ShipHpQuantityInstall = "";
		GameInterface.strings.ShipSpInstall = "";
		GameInterface.strings.ShipSpQuantityInstall = "";
		GameInterface.strings.ShipSrInstall = "";
		GameInterface.strings.ShipSrQuantityInstall = "";
		GameInterface.strings.ShipTrInstall = "";
		GameInterface.strings.ShipTrQuantityInstall = "";
		GameInterface.strings.ShipCrewInstall = "";
		GameInterface.strings.ShipCrewQuantityInstall = "";
		GameInterface.strings.ShipCannonsInstall = "";
		GameInterface.strings.ShipCannonsQuantityInstall = "";
		GameInterface.strings.ShipCannonsQuantityInstall = "";
		GameInterface.strings.ShipCannonsTypeInstall = "";
		GameInterface.strings.ShipCannonsTypeInstall1 = "";

		GameInterface.pictures.ShipTypeChange.tex = "";
		GameInterface.pictures.BackgroundShipTypeChange.pic = "";
		GameInterface.pictures.BackgroundShipNameChange.pic = "";
		GameInterface.strings.ShipNameChange = "";
		GameInterface.strings.ShipTypeChange = "";
		GameInterface.strings.ShipClassChange = "";

		GameInterface.pictures.ShipTypeChange2.tex = "";
		GameInterface.pictures.BackgroundShipTypeChange2.pic = "";
		GameInterface.pictures.BackgroundShipNameChange2.pic = "";
		GameInterface.strings.ShipNameChange2 = "";
		GameInterface.strings.ShipTypeChange2 = "";
		GameInterface.strings.ShipClassChange2 = "";

		SetNodeUsing("FRAME_SHIP_CHANGE", false);
		SetNodeUsing("CHANGE_OK", false);
		SetNodeUsing("CHANGE_CANCEL", false);
		SetNodeUsing("CHANGE_LEFT", false);
		SetNodeUsing("CHANGE_RIGHT", false);
		SetNodeUsing("BORDERS_SHIP_INSTALL", false);
		SetNodeUsing("BORDERS_SHIP_INSTALL_1", false);
		SetNodeUsing("BORDERS_SHIP_CHANGE_LEFT", false);
		SetNodeUsing("BORDERS_SHIP_CHANGE_LEFT_1", false);
		SetNodeUsing("BORDERS_SHIP_CHANGE_RIGHT", false);
		SetNodeUsing("BORDERS_SHIP_CHANGE_RIGHT_1", false);
	}

	if(bContinue)
	{
		GameInterface.pictures.ShipType1.pic = "";
		GameInterface.pictures.BackgroundShipName1.pic = "";
		GameInterface.pictures.BackgroundShipType1.pic = "";
		GameInterface.pictures.ShipHp1.pic = "";
		GameInterface.pictures.ShipSp1.pic = "";
		GameInterface.pictures.ShipSr1.pic = "";
		GameInterface.pictures.ShipTr1.pic = "";
		GameInterface.pictures.ShipCrew1.pic = "";
		GameInterface.pictures.ShipCannons1.pic = "";

		GameInterface.strings.ShipOneNot = "";
		GameInterface.strings.ShipHeaderOne = "";
		GameInterface.strings.ShipName1 = "";
		GameInterface.strings.ShipType1 = "";
		GameInterface.strings.ShipClass1 = "";
		GameInterface.strings.ShipHp1 = "";
		GameInterface.strings.ShipHpQuantity1 = "";
		GameInterface.strings.ShipSp1 = "";
		GameInterface.strings.ShipSpQuantity1 = "";
		GameInterface.strings.ShipSr1 = "";
		GameInterface.strings.ShipSrQuantity1 = "";
		GameInterface.strings.ShipTr1 = "";
		GameInterface.strings.ShipTrQuantity1 = "";
		GameInterface.strings.ShipCrew1 = "";
		GameInterface.strings.ShipCrewQuantity1 = "";
		GameInterface.strings.ShipCannons1 = "";
		GameInterface.strings.ShipCannonsQuantity1 = "";

		GameInterface.pictures.ShipType2.pic = "";
		GameInterface.pictures.BackgroundShipName2.pic = "";
		GameInterface.pictures.BackgroundShipType2.pic = "";
		GameInterface.pictures.ShipHp2.pic = "";
		GameInterface.pictures.ShipSp2.pic = "";
		GameInterface.pictures.ShipSr2.pic = "";
		GameInterface.pictures.ShipTr2.pic = "";
		GameInterface.pictures.ShipCrew2.pic = "";
		GameInterface.pictures.ShipCannons2.pic = "";

		GameInterface.strings.ShipTwoNot = "";
		GameInterface.strings.ShipHeaderTwo = "";
		GameInterface.strings.ShipName2 = "";
		GameInterface.strings.ShipType2 = "";
		GameInterface.strings.ShipClass2 = "";
		GameInterface.strings.ShipHp2 = "";
		GameInterface.strings.ShipHpQuantity2 = "";
		GameInterface.strings.ShipSp2 = "";
		GameInterface.strings.ShipSpQuantity2 = "";
		GameInterface.strings.ShipSr2 = "";
		GameInterface.strings.ShipSrQuantity2 = "";
		GameInterface.strings.ShipTr2 = "";
		GameInterface.strings.ShipTrQuantity2 = "";
		GameInterface.strings.ShipCrew2 = "";
		GameInterface.strings.ShipCrewQuantity2 = "";
		GameInterface.strings.ShipCannons2 = "";
		GameInterface.strings.ShipCannonsQuantity2 = "";

		GameInterface.pictures.ShipType3.pic = "";
		GameInterface.pictures.BackgroundShipName3.pic = "";
		GameInterface.pictures.BackgroundShipType3.pic = "";
		GameInterface.pictures.ShipHp3.pic = "";
		GameInterface.pictures.ShipSp3.pic = "";
		GameInterface.pictures.ShipSr3.pic = "";
		GameInterface.pictures.ShipTr3.pic = "";
		GameInterface.pictures.ShipCrew3.pic = "";
		GameInterface.pictures.ShipCannons3.pic = "";

		GameInterface.strings.ShipThreeNot = "";
		GameInterface.strings.ShipHeaderThree = "";
		GameInterface.strings.ShipName3 = "";
		GameInterface.strings.ShipType3 = "";
		GameInterface.strings.ShipClass3 = "";
		GameInterface.strings.ShipHp3 = "";
		GameInterface.strings.ShipHpQuantity3 = "";
		GameInterface.strings.ShipSp3 = "";
		GameInterface.strings.ShipSpQuantity3 = "";
		GameInterface.strings.ShipSr3 = "";
		GameInterface.strings.ShipSrQuantity3 = "";
		GameInterface.strings.ShipTr3 = "";
		GameInterface.strings.ShipTrQuantity3 = "";
		GameInterface.strings.ShipCrew3 = "";
		GameInterface.strings.ShipCrewQuantity3 = "";
		GameInterface.strings.ShipCannons3 = "";
		GameInterface.strings.ShipCannonsQuantity3 = "";

		GameInterface.pictures.ShipType4.pic = "";
		GameInterface.pictures.BackgroundShipName4.pic = "";
		GameInterface.pictures.BackgroundShipType4.pic = "";
		GameInterface.pictures.ShipHp4.pic = "";
		GameInterface.pictures.ShipSp4.pic = "";
		GameInterface.pictures.ShipSr4.pic = "";
		GameInterface.pictures.ShipTr4.pic = "";
		GameInterface.pictures.ShipCrew4.pic = "";
		GameInterface.pictures.ShipCannons4.pic = "";

		GameInterface.strings.ShipFourNot = "";
		GameInterface.strings.ShipHeaderFour = "";
		GameInterface.strings.ShipName4 = "";
		GameInterface.strings.ShipType4 = "";
		GameInterface.strings.ShipClass4 = "";
		GameInterface.strings.ShipHp4 = "";
		GameInterface.strings.ShipHpQuantity4 = "";
		GameInterface.strings.ShipSp4 = "";
		GameInterface.strings.ShipSpQuantity4 = "";
		GameInterface.strings.ShipSr4 = "";
		GameInterface.strings.ShipSrQuantity4 = "";
		GameInterface.strings.ShipTr4 = "";
		GameInterface.strings.ShipTrQuantity4 = "";
		GameInterface.strings.ShipCrew4 = "";
		GameInterface.strings.ShipCrewQuantity4 = "";
		GameInterface.strings.ShipCannons4 = "";
		GameInterface.strings.ShipCannonsQuantity4 = "";

		CreateString(true,"ShipChangeFrameHeader",GetConvertStrWithReplace("Variable_ColonyGuardiang_59", "Interface.txt", "#space#", " "), FONT_CAPTION, COLOR_NORMAL, 395,129,SCRIPT_ALIGN_CENTER,1.5);
		SetNodeUsing("FRAME_SHIP_CHANGE", true);
		SetNodeUsing("CHANGE_OK", true);
		SetNodeUsing("CHANGE_CANCEL", true);
		SetNodeUsing("CHANGE_LEFT", true);
		SetNodeUsing("CHANGE_RIGHT", true);
		SetNodeUsing("BORDERS_SHIP_CHANGE_LEFT", true);
		SetNodeUsing("BORDERS_SHIP_CHANGE_LEFT_1", true);
		SetNodeUsing("BORDERS_SHIP_CHANGE_RIGHT", true);
		SetNodeUsing("BORDERS_SHIP_CHANGE_RIGHT_1", true);
		SetSelectable("CHANGE_OK", true);
		SetChangeInformation();
	}
}

void SetChangeInformation()
{
	int iShipOne = sti(PChar.Colony.Guardians.Ship1);

	int iCharacterOne = iShipOne;
	ref chr = &Characters[iCharacterOne];

	ref rShip = GetRealShip(sti(chr.ship.type));
	int iShipClass = sti(rShip.class);
	string sShipType = ShipsTypes[sti(rShip.basetype)].name;
	string sShipName = chr.Ship.name;

	CreateImage("ShipTypeChange", "SHIPS_" + sShipType + CG_ShipPicSuffix(), "ship", 116,224,306,414);

	CreateString(true,"ShipNameChange", sShipName, FONT_CAPTION, COLOR_NORMAL, 210,196,SCRIPT_ALIGN_CENTER,1.2);

	CreateString(true,"ShipTypeChange", XI_ConvertString(sShipType), FONT_CAPTION, COLOR_NORMAL, 210,417,SCRIPT_ALIGN_CENTER,1.2);
	CreateString(true,"ShipClassChange", iShipClass, FONT_BOLD_NUMBERS, COLOR_NORMAL, 280,390,SCRIPT_ALIGN_LEFT,1.0);

	string sShip = "Ship" + iChangeCount;
	int iShipTwo = sti(PChar.Colony.Guardians.(sShip));
	chr = &Characters[iShipTwo];

	rShip = GetRealShip(sti(chr.ship.type));
	iShipClass = sti(rShip.class);
	sShipType = ShipsTypes[sti(rShip.basetype)].name;
	sShipName = chr.Ship.name;

	CreateImage("ShipTypeChange2", "SHIPS_" + sShipType + CG_ShipPicSuffix(), "ship", 426,224,616,414);

	CreateString(true,"ShipNameChange2", sShipName, FONT_CAPTION, COLOR_NORMAL, 520,196,SCRIPT_ALIGN_CENTER,1.2);

	CreateString(true,"ShipTypeChange2", XI_ConvertString(sShipType), FONT_CAPTION, COLOR_NORMAL, 520,417,SCRIPT_ALIGN_CENTER,1.2);
	CreateString(true,"ShipClassChange2", iShipClass, FONT_BOLD_NUMBERS, COLOR_NORMAL, 590,390,SCRIPT_ALIGN_LEFT,1.0);
}

void ChangeShipsOK()
{
	string sShip = "Ship" + iChangeCount;

	int iShipOne = sti(PChar.Colony.Guardians.Ship1);
	int iShipTwo = sti(PChar.Colony.Guardians.(sShip));

	PChar.Colony.Guardians.Ship1 = iShipTwo;
	PChar.Colony.Guardians.(sShip) = iShipOne;

	string sGroup = "InterfaceColonyGuardians_1";
	if(Group_FindGroup(sGroup) != -1)
	{
		Group_DeleteGroup(sGroup);
	}

	sGroup = "InterfaceColonyGuardians_" + iChangeCount;
	if(Group_FindGroup(sGroup) != -1)
	{
		Group_DeleteGroup(sGroup);
	}

	ref chr = &Characters[iShipTwo];
	sGroup = "InterfaceColonyGuardians_1";
	Group_FindOrCreateGroup(sGroup);

	Group_SetType(sGroup, "war");
	Group_AddCharacter(sGroup, chr.id);
	Group_SetGroupCommander(sGroup, chr.id);

	chr = &Characters[iShipOne];
	sGroup = "InterfaceColonyGuardians_" + iChangeCount;
	Group_FindOrCreateGroup(sGroup);

	Group_SetType(sGroup, "war");
	Group_AddCharacter(sGroup, chr.id);
	Group_SetGroupCommander(sGroup, chr.id);

	GuardianInizialization(iShipTwo, 1, true);
	GuardianInizialization(iShipOne, iChangeCount, false);

	SetChangeFrame(true, false);
	SetInformation();
}

void CheckChangeShips(bool left)
{
	int iMax = GetMaxChangeShips();

	if(left)
	{
		iChangeCount--;
	}
	else
	{
		iChangeCount++;
	}

	if(iChangeCount < 2)
	{
		iChangeCount = iMax;
	}

	if(iChangeCount > iMax)
	{
		iChangeCount = 2;
	}

	SetChangeInformation();
}

int GetMaxChangeShips()
{
	int iMax = 0;

	string sShip;
	for(int i=1; i <= 4; i++)
	{
		sShip = "Ship" + i;

		if(PChar.Colony.Guardians.(sShip) != -1)
		{
			iMax++;
		}
	}

	return iMax;
}

