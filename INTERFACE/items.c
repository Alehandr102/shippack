/// BOAL меню предметов
#include "interface\character_all.h"

int nCurScrollOfficerNum;
int iEQUIP_SET = 0;
string sMapDescribe;//переменная для текста описания карты
bool bMapCross, isPassenger;
float fOffsetX, fOffsetY;
int iCurTab;
int ItemsV[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool OfficersSwitch = true;
bool PrisonersSwitch = true;
// LEO: по какой рамке боеприпаса кликнули: "pistol" - ITEM_14, "mushket" - ITEM_16, "" - живой ствол (как было).
//   Сбрасывается в FillItemsTable при любом другом режиме таблицы.
string sAmmoTarget = "";

// LEO: Инвентарный обмен использует ту же статичную картинку, что ItemsChange, без hover и замены старого DDS.
void Items_SetExchangeIcon()
{
	if(CommandIcons_Enabled())
	{
		SetNewPicture("EXCHANGE",CommandIcons_StaticTexture("LandCommands"));
		SendMessage(&GameInterface,"lslffff",MSG_INTERFACE_MSG_TO_NODE,"EXCHANGE",1,3.0/16.0,4.0/16.0,1.0/4.0,2.0/4.0);
	}
	else
	{
		SetNewPicture("EXCHANGE","interfaces\Exchange.dds");
		SendMessage(&GameInterface,"lslffff",MSG_INTERFACE_MSG_TO_NODE,"EXCHANGE",1,0.0,1.0,0.0,1.0);
	}
}

void InitInterface(string iniName)
{
	GetXYWindowOffset(&fOffsetX, &fOffsetY);
	InterfaceStack.SelectMenu_node = "LaunchItems"; // запоминаем, что звать по Ф2
	GameInterface.title = "titleItems";
	xi_refCharacter = pchar;
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) xi_refCharacter = GetCharacter(sti(pchar.SystemInfo.CheckNpcIdx)); // LEO: бета-чеккер — показываем предметы проверяемого NPC (read-only)
	isPassenger = 0;
	FillCharactersScroll();
	FillFreePassengerScroll();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	Items_SetExchangeIcon();

	// LEO: бета-чеккер NPC — прячем партийный скролл и не-чеккерные вкладки (Журнал/Нации/Торговля); остаются Персонаж/Корабль/Предметы (метод ship.c).
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx"))
	{
		SetNodeUsing("CHARACTERS_SCROLL", false);
		SetNodeUsing("CHARACTERS_SCROLL_FRAME", false);
		SetNodeUsing("CHARACTERS_BACK", false);    // серая подложка скролла
		SetNodeUsing("CHARACTERS_BACK_HI", false); // подсветка выбранного
		SetNodeUsing("UP_BUTTON", false);
		SetNodeUsing("DOWN_BUTTON", false);
		SetNodeUsing("I_QUESTBOOK", false);
		SetNodeUsing("I_QUESTBOOK_2", false);
		SetNodeUsing("I_NATIONS", false);
		SetNodeUsing("I_NATIONS_2", false);
		SetNodeUsing("I_TRADEBOOK", false);
		SetNodeUsing("I_TRADEBOOK_2", false);
		SetNodeUsing("STR_BUTTONS", false); // LEO: ВСЕ тексты верхнего меню одним узлом (как в ship.c) — остаются только иконки Персонаж/Корабль/Предметы
	}

	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("frame","ProcessFrame",1);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("ShowMapDesc","ShowMapDesc",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);

	SetEventHandler("ExitOfficerMenu","ExitOfficerMenu",0);
	SetEventHandler("OfficerChange","OfficerChange",0);
	SetEventHandler("acceptaddofficer","AcceptAddOfficer",0);
	SetEventHandler("ExitRemoveOfficerMenu","ExitRemoveOfficerMenu",0);
	SetEventHandler("AcceptRemoveOfficer","AcceptRemoveOfficer",0);

	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("eTabControlPress","procTabChange",0);
	SetEventHandler("ExitMapWindow","ExitMapWindow",0);
	SetEventHandler("ExitItemFromCharacterWindow","ExitItemFromCharacterWindow",0);

	SetEventHandler("ShowItemFromCharacterWindow","ShowItemFromCharacterWindow",0);
	SetEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT", 0);

	SetEventHandler("ShowBladeEquipInfo", "ShowBladeEquipInfo", 0);
	SetEventHandler("ShowGunEquipInfo", "ShowGunEquipInfo", 0);
	SetEventHandler("ShowSpyglassEquipInfo", "ShowSpyglassEquipInfo", 0);
	SetEventHandler("ShowCirassEquipInfo", "ShowCirassEquipInfo", 0);
	SetEventHandler("ShowSuitEquipInfo", "ShowSuitEquipInfo", 0);
	SetEventHandler("ShowBackPackEquipInfo", "ShowBackPackEquipInfo", 0);
	SetEventHandler("ShowTalismanEquipInfo", "ShowTalismanEquipInfo", 0);
	SetEventHandler("ShowBookEquipInfo", "ShowBookEquipInfo", 0);
	SetEventHandler("Show1EquipInfo", "Show1EquipInfo", 0);
	SetEventHandler("Show2EquipInfo", "Show2EquipInfo", 0);
	SetEventHandler("Show3EquipInfo", "Show3EquipInfo", 0);
	SetEventHandler("Show4EquipInfo", "Show4EquipInfo", 0);
	SetEventHandler("Show5EquipInfo", "Show5EquipInfo", 0);
	SetEventHandler("ShowAmmoEquipInfo", "ShowAmmoEquipInfo", 0);
	SetEventHandler("ShowMushAmmoEquipInfo", "ShowMushAmmoEquipInfo", 0);
	SetEventHandler("ClickAmmo", "ClickAmmo", 0);
	SetEventHandler("ClickMushAmmo", "ClickMushAmmo", 0);
	SetEventHandler("ClickShow1", "ClickShow1", 0);
	SetEventHandler("ClickShow2", "ClickShow2", 0);
	SetEventHandler("ClickShow3", "ClickShow3", 0);
	SetEventHandler("ClickShow4", "ClickShow4", 0);
	SetEventHandler("ClickShow5", "ClickShow5", 0);
	SetEventHandler("ClickBook", "ClickBook", 0);
	SetEventHandler("ClickTalisman", "ClickTalisman", 0);
	SetEventHandler("ClickBackPack", "ClickBackPack", 0);
	SetEventHandler("ClickMushket", "ClickMushket", 0);
	SetEventHandler("ClickCirass", "ClickCirass", 0);
	SetEventHandler("ClickSuit", "ClickSuit", 0);
	SetEventHandler("ClickSpyGlass", "ClickSpyGlass", 0);
	SetEventHandler("ClickGun", "ClickGun", 0);
	SetEventHandler("ClickSaber", "ClickSaber", 0);
	SetEventHandler("CheckButtonChange", "CheckButtonChange", 0);
	SetEventHandler("SaveSETName", "SaveSETName", 0);
	SetEventHandler("ShowEditBox", "ShowEditBox", 0);
	SetEventHandler("HideEditBox", "HideEditBox", 0);

	XI_RegistryExitKey("IExit_F2");
	SetVariable();
	HideEditBox();
	SetNewGroupPicture("Weight_PIC", "ICONS_CHAR", "weight");
	SetNewGroupPicture("Money_PIC", "ICONS_CHAR", "Money");
	SetNodeUsing("EXCHANGE", false);
	CheckForAvailableItems();
}

void CheckForAvailableItems()
{
	int i, n;
	for (i = 1; i < 16; i++) SetNodeUsing("ITEM_"+i+"_V",false);
	for (i = 0; i < 14; i++) ItemsV[i] = 0;
	object allItems;
	aref rootItems, arItem;
	ref rItem;
	string row, sGood, sItem;
	makearef(rootItems, xi_refCharacter.Items);
	n = GetAttributesNum(rootItems);
	for (i = 0; i < n; i++)
	{
		arItem = GetAttributeN(rootItems, i);
		sGood = GetAttributeName(arItem);
		rItem = ItemsFromID(sGood);
		if (ItemsV[0] == 0)
		{
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,"blade"))
			{
				SetNodeUsing("ITEM_1_V",true);
				ItemsV[0] = 1;
			}
		}
		if (ItemsV[1] == 0)
		{
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,"gun"))
			{
				SetNodeUsing("ITEM_2_V",true);
				ItemsV[1] = 1;
			}
		}
		if (ItemsV[2] == 0)
		{
			if (HasSubStr(rItem.id,"spyglass"))
			{
				SetNodeUsing("ITEM_3_V",true);
				ItemsV[2] = 1;
			}
		}
		if (ItemsV[3] == 0)
		{
			if (HasSubStr(rItem.id,"cirass"))
			{
				SetNodeUsing("ITEM_4_V",true);
				ItemsV[3] = 1;
			}
		}
		if (ItemsV[4] == 0)
		{
			if (HasSubStr(rItem.id,"mushket"))
			{
				SetNodeUsing("ITEM_5_V",true);
				ItemsV[4] = 1;
			}
		}
		if (ItemsV[5] == 0)
		{
			if (HasSubStr(rItem.id,"backpack"))
			{
				SetNodeUsing("ITEM_6_V",true);
				ItemsV[5] = 1;
			}
		}
		if (ItemsV[6] == 0)
		{
			if (HasSubStr(rItem.id,"talisman"))
			{
				SetNodeUsing("ITEM_7_V",true);
				ItemsV[6] = 1;
			}
		}
		if (ItemsV[7] == 0)
		{
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,"indian_center"))
			{
				SetNodeUsing("ITEM_8_V",true);
				ItemsV[7] = 1;
			}
		}
		if (ItemsV[8] == 0)
		{
			if (CheckAttribute(rItem,"skill"))
			{
				SetNodeUsing("ITEM_9_V",true);
				ItemsV[8] = 1;
			}
		}
		if (ItemsV[9] == 0)
		{
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,"jewelry_indian_left"))
			{
				SetNodeUsing("ITEM_10_V",true);
				ItemsV[9] = 1;
			}
		}
		if (ItemsV[10] == 0)
		{
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,"jewelry_indian_right"))
			{
				SetNodeUsing("ITEM_11_V",true);
				ItemsV[10] = 1;
			}
		}
		if (ItemsV[11] == 0)
		{
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,"idols_left"))
			{
				SetNodeUsing("ITEM_12_V",true);
				ItemsV[11] = 1;
			}
		}
		if (ItemsV[12] == 0)
		{
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,"idols_right"))
			{
				SetNodeUsing("ITEM_13_V",true);
				ItemsV[12] = 1;
			}
		}
		if (ItemsV[13] == 0)
		{
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,"suit"))
			{
				SetNodeUsing("ITEM_15_V",true);
				ItemsV[13] = 1;
			}
		}
	}
}

void FillFreePassengerScroll()
{
	int i, howWork, _curCharIdx;
	string faceName, attributeName, PsgAttrName, sTemp;
	ref _refCurChar;
	aref pRef;
	bool ok;

	DeleteAttribute(&GameInterface, "PASSENGERSLIST");

	nCurScrollOfficerNum = -1;
	GameInterface.PASSENGERSLIST.current = 0;
	GameInterface.PASSENGERSLIST.NotUsed = 6;
	GameInterface.PASSENGERSLIST.ImagesGroup.t0 = "EMPTYFACE";
	FillFaceList("PASSENGERSLIST.ImagesGroup", pchar, 2); // passengers
	GameInterface.PASSENGERSLIST.BadTex1 = 0;
	GameInterface.PASSENGERSLIST.BadPic1 = "emptyface";

	makearef(pRef,pchar.Fellows.Passengers);
	int nListSize = GetPassengersQuantity(pchar);
	int m = 0;
	for(i=0; i<nListSize; i++)
	{
		attributeName = "pic" + (m+1);
		PsgAttrName = "id"+(i+1);

		_curCharIdx = sti(pRef.(PsgAttrName));

		if (_curCharIdx!=-1)
		{
			ok = CheckAttribute(&characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true;
			if (!ok && GetRemovable(&characters[_curCharIdx]))
			{
				// совместители должностей -->
				howWork = 1;
				if (CheckCharacterPerk(&characters[_curCharIdx], "ByWorker")) howWork = 2;
				if (CheckCharacterPerk(&characters[_curCharIdx], "ByWorker2")) howWork = 3;
				// LEO: пересчёт лечит и старые сейвы - счётчик мог уехать вверх и намертво прятать человека
				ok = SyncOfficerIsFree(&characters[_curCharIdx]) < howWork;
				PsgAttrName = GetOfficerTypeByNum(nCurScrollNum);
				// совместители должностей <--
				if (ok && !CheckAttribute(&characters[_curCharIdx], PsgAttrName))
				{
					GameInterface.PASSENGERSLIST.(attributeName).character = _curCharIdx;
					GameInterface.PASSENGERSLIST.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
					GameInterface.PASSENGERSLIST.(attributeName).tex1 = FindFaceGroupNum("PASSENGERSLIST.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);

					//специализация первой
					sTemp = "#" + GetIconFromFont(GetSpeciality(characters[_curCharIdx]));
					//должности	 - не оптимально, но не должно быть проблемой
					if (isOfficer(characters[_curCharIdx])) sTemp += GetIconFromFont("fighter");
					if (sti(pchar.Fellows.Passengers.navigator) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("navigator");
					if (sti(pchar.Fellows.Passengers.boatswain) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("boatswain");
					if (sti(pchar.Fellows.Passengers.cannoner) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("cannoner");
					if (sti(pchar.Fellows.Passengers.doctor) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("doctor");
					if (sti(pchar.Fellows.Passengers.carpenter) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("carpenter");
					if (sti(pchar.Fellows.Passengers.treasurer) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("treasurer");
					GameInterface.PASSENGERSLIST.(attributeName).str1 = sTemp; 

					m++;
				}
			}
		}
	}
	GameInterface.PASSENGERSLIST.ListSize = m + 2; // не знаю зачем, но для совместимости с GetConvertStrWithReplace("Variable_character_all_64", "Interface.txt", "#space#", " ")
}

void ExitOfficerMenu()
{
	XI_WindowShow("OFFICERS_WINDOW", false);
	XI_WindowDisable("OFFICERS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CHARACTERS_SCROLL");
}

void OfficerChange()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не переключать/назначать персонажей NPC
	string attributeName = "pic" + (nCurScrollNum+1);

	if(GameInterface.CHARACTERS_SCROLL.(attributeName).character != "0")
	{
		int iCharacter = sti(GameInterface.CHARACTERS_SCROLL.(attributeName).character);
		xi_refCharacter = &characters[iCharacter];

		if (CheckAttribute(xi_refCharacter, "prisoned") && xi_refCharacter.prisoned == true) {;}
		else {if (GetRemovable(xi_refCharacter)) isPassenger = 0; else isPassenger = 1;}

		if (isOfficerInShip(xi_refCharacter, true) && xi_refCharacter.id != pchar.id)
		{
			XI_WindowShow("REMOVE_OFFICER_WINDOW", true);
			XI_WindowDisable("REMOVE_OFFICER_WINDOW", false);
			XI_WindowDisable("MAIN_WINDOW", true);

			SetCurrentNode("REMOVE_CANCEL_OFFICER");
		}
	}
	else
	{
		//Boyer mod
		if (nCurScrollNum <= 6 + MAX_NUM_FIGHTERS && nCurScrollNum != 0)
		{
			FillFreePassengerScroll();
			SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"PASSENGERSLIST",-1);
			SetCurrentNode("PASSENGERSLIST");
			ProcessFrame();
			SetOfficersSkills();

			XI_WindowShow("OFFICERS_WINDOW", true);
			XI_WindowDisable("OFFICERS_WINDOW", false);
			XI_WindowDisable("MAIN_WINDOW", true);
		}
	}
	CheckForAvailableItems();
}

void SetOfficersSkills()
{
	string sCharacter = "pic"+(sti(GameInterface.PASSENGERSLIST.current)+1);
	if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter))
	{
		if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter + ".character"))
		{
			sCharacter = GameInterface.PASSENGERSLIST.(sCharacter).character;
			ref otherchr = &characters[sti(sCharacter)];
			SetSPECIALMiniTable("TABLE_SMALLSKILL", otherchr);
			SetOTHERMiniTable("TABLE_SMALLOTHER", otherchr);
			SetFormatedText("OFFICER_NAME", GetFullName(otherchr));
			SetSelectable("ACCEPT_ADD_OFFICER", true);
		}
		else
		{
			Table_Clear("TABLE_SMALLSKILL", false, true, true);
			Table_Clear("TABLE_SMALLOTHER", false, true, true);
			SetFormatedText("OFFICER_NAME", "");
			SetSelectable("ACCEPT_ADD_OFFICER", false);
		}
	}
	else
	{
		Table_Clear("TABLE_SMALLSKILL", false, true, true);
		Table_Clear("TABLE_SMALLOTHER", false, true, true);
		SetFormatedText("OFFICER_NAME", "");
		SetSelectable("ACCEPT_ADD_OFFICER", false);
	}
}

void AcceptAddOfficer()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не назначать офицеров в чеккере NPC
	int iCurrentNode = nCurScrollNum;
	bool bNeedFollow = false;

	string attributeName2 = "pic"+(nCurScrollOfficerNum+1);
	int iChar = sti(GameInterface.PASSENGERSLIST.(attributeName2).character);
	// LEO: раненого (контузия) нельзя ставить НИ НА ОДНУ должность — дождись выздоровления (дыра items-меню, зеркало character_all.c).
	//   БЕЗ Log_info (под меню интерфейса лог не виден) — молча отклоняем, меню оставляем открытым.
	if (CheckAttribute(&characters[iChar], "HPminusDaysNeedtoRestore"))
	{
		return;
	}
	sld = &characters[iChar];
	if(sld.id != "pet_crab" || (nCurScrollNum > 6))
	{
		if (checkAttribute(GameInterface, "PASSENGERSLIST."+attributeName2 + ".character"))
		{
			iChar = sti(GameInterface.PASSENGERSLIST.(attributeName2).character);

			switch (nCurScrollNum)
			{
				case 1:
					pchar.Fellows.Passengers.navigator = iChar;
				break;

				case 2:
					pchar.Fellows.Passengers.boatswain = iChar;
				break;

				case 3:
					pchar.Fellows.Passengers.cannoner = iChar;
				break;

				case 4:
					pchar.Fellows.Passengers.doctor = iChar;
				break;

				case 5:
					pchar.Fellows.Passengers.treasurer = iChar;
				break;

				case 6:
					pchar.Fellows.Passengers.carpenter = iChar;
				break;
				// LEO: хвостовой блок без метки - он и работает как default: строки абордажников.
					SetOfficersIndex(pchar, -1, iChar);
					bNeedFollow = true;
				break;
			}
			attributeName2 = GetOfficerTypeByNum(nCurScrollNum);
			characters[iChar].(attributeName2) = true; // совместитель дожности
			SyncOfficerIsFree(&characters[iChar]); // LEO: счётчик занятых должностей - по факту
			if (bNeedFollow) // только для офов
			{

				if (IsEntity(loadedLocation) && loadedLocation.type == "tavern")
				{   //  пусть всегда будут появляться
					PlaceCharacter(&Characters[iChar], "goto", "random_must_be_near");
				}
				LAi_tmpl_SetFollow(&Characters[iChar], GetMainCharacter(), -1.0);
			}
			FillCharactersScroll();
			GameInterface.CHARACTERS_SCROLL.current = iCurrentNode;
		}
	}
	ExitOfficerMenu();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"CHARACTERS_SCROLL",-1);
	ProcessFrame();
	SetVariable();
}

void ExitRemoveOfficerMenu()
{
	XI_WindowShow("REMOVE_OFFICER_WINDOW", false);
	XI_WindowDisable("REMOVE_OFFICER_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CHARACTERS_SCROLL");
}

void AcceptRemoveOfficer()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не снимать офицеров в чеккере NPC
	int iCurrentNode = nCurScrollNum;
	string attributeName2 = "pic"+(nCurScrollNum+1);
	string sKeepLoc;

	int iChar = sti(GameInterface.CHARACTERS_SCROLL.(attributeName2).character);

	switch (nCurScrollNum)
	{
		case 1:
			pchar.Fellows.Passengers.navigator = -1;
		break;

		case 2:
			pchar.Fellows.Passengers.boatswain = -1;
		break;

		case 3:
			pchar.Fellows.Passengers.cannoner = -1;
		break;

		case 4:
			pchar.Fellows.Passengers.doctor = -1;
		break;

		case 5:
			pchar.Fellows.Passengers.treasurer = -1;
		break;

		case 6:
			pchar.Fellows.Passengers.carpenter = -1;
		break;
		// LEO: хвостовой блок без метки - он и работает как default: строки абордажников.
			// LEO: снятие со слота абордажника не должно выбрасывать человека из мира: внутри
			//   SetOfficersIndex бывшему бойцу ставят адрес "none" - это путь УХОДА из команды.
			//   Прочие должности (случаи 1-6) адрес не трогают вовсе - выравниваем поведение.
			sKeepLoc = "none";
			if (CheckAttribute(&characters[iChar], "location")) sKeepLoc = characters[iChar].location;
			RemoveOfficersIndex(pchar, iChar);
			characters[iChar].location = sKeepLoc;
		break;
	}
	attributeName2 = GetOfficerTypeByNum(nCurScrollNum);
	DeleteAttribute(&characters[iChar], attributeName2); // совместитель дожности
	SyncOfficerIsFree(&characters[iChar]); // LEO: счётчик занятых должностей - по факту, а не вслепую -1

	FillCharactersScroll();
	GameInterface.CHARACTERS_SCROLL.current = iCurrentNode;
	ExitRemoveOfficerMenu();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"CHARACTERS_SCROLL",-1);
	SetVariable();
}

void ProcessExitCancel()
{
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	CloseTooltip();
	if (!CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) SaveEquipSet(); // LEO: в чеккере не сохраняем комплект экипировки на проверяемого NPC
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("frame","ProcessFrame");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("ShowMapDesc","ShowMapDesc");
	DelEventHandler("MouseRClickUp","HideInfoWindow");

	DelEventHandler("ExitOfficerMenu","ExitOfficerMenu");
	DelEventHandler("OfficerChange","OfficerChange");
	DelEventHandler("acceptaddofficer","AcceptAddOfficer");
	DelEventHandler("ExitRemoveOfficerMenu","ExitRemoveOfficerMenu");
	DelEventHandler("AcceptRemoveOfficer","AcceptRemoveOfficer");

	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("eTabControlPress","procTabChange");
	DelEventHandler("ExitMapWindow","ExitMapWindow");
	DelEventHandler("ExitItemFromCharacterWindow","ExitItemFromCharacterWindow");
	DelEventHandler("ShowItemFromCharacterWindow","ShowItemFromCharacterWindow");

	DelEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT");

	DelEventHandler("ShowBladeEquipInfo", "ShowBladeEquipInfo");
	DelEventHandler("ShowGunEquipInfo", "ShowGunEquipInfo");
	DelEventHandler("ShowSpyglassEquipInfo", "ShowSpyglassEquipInfo");
	DelEventHandler("ShowCirassEquipInfo", "ShowCirassEquipInfo");
	DelEventHandler("ShowSuitEquipInfo", "ShowSuitEquipInfo");
	DelEventHandler("ShowBackPackEquipInfo", "ShowBackPackEquipInfo");
	DelEventHandler("ShowTalismanEquipInfo", "ShowTalismanEquipInfo");
	DelEventHandler("ShowBookEquipInfo", "ShowBookEquipInfo");
	DelEventHandler("Show1EquipInfo", "Show1EquipInfo");
	DelEventHandler("Show2EquipInfo", "Show2EquipInfo");
	DelEventHandler("Show3EquipInfo", "Show3EquipInfo");
	DelEventHandler("Show4EquipInfo", "Show4EquipInfo");
	DelEventHandler("Show5EquipInfo", "Show5EquipInfo");
	DelEventHandler("ShowAmmoEquipInfo", "ShowAmmoEquipInfo");
	DelEventHandler("ShowMushAmmoEquipInfo", "ShowMushAmmoEquipInfo");
	DelEventHandler("ClickAmmo", "ClickAmmo");
	DelEventHandler("ClickMushAmmo", "ClickMushAmmo");
	DelEventHandler("ClickShow1", "ClickShow1");
	DelEventHandler("ClickShow2", "ClickShow2");
	DelEventHandler("ClickShow3", "ClickShow3");
	DelEventHandler("ClickShow4", "ClickShow4");
	DelEventHandler("ClickShow5", "ClickShow5");
	DelEventHandler("ClickBook", "ClickBook");
	DelEventHandler("ClickTalisman", "ClickTalisman");
	DelEventHandler("ClickBackPack", "ClickBackPack");
	DelEventHandler("ClickMushket", "ClickMushket");
	DelEventHandler("ClickCirass", "ClickCirass");
	DelEventHandler("ClickSuit", "ClickSuit");
	DelEventHandler("ClickSpyGlass", "ClickSpyGlass");
	DelEventHandler("ClickGun", "ClickGun");
	DelEventHandler("ClickSaber", "ClickSaber");
	DelEventHandler("CheckButtonChange", "CheckButtonChange");
	DelEventHandler("SaveSETName", "SaveSETName");
	DelEventHandler("ShowEditBox", "ShowEditBox");
	DelEventHandler("HideEditBox", "HideEditBox");

	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"ReloadMenuExit"))
	{
		DeleteAttribute(&InterfaceStates,"ReloadMenuExit");
		EndCancelInterface(false);
	}
	else
	{
		DeleteAttribute(pchar, "SystemInfo.CheckNpcIdx"); // LEO: полный выход (Esc) из меню предметов — чисто завершаем бета-чеккер NPC (зеркало character.c:101)
		EndCancelInterface(true);
	}
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	ref itm;
	bool Block = bAbordageStarted && !bCabinStarted && !bDeckBoatStarted;
	// LEO: чеккер NPC — не уводить из чеккера на вкладки-данные ГГ (Журнал/Нации/Торговля). Персонаж/Корабль/Предметы роутятся на NPC и остаются рабочими.
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx"))
	{
		if (nodName == "I_QUESTBOOK" || nodName == "I_QUESTBOOK_2" || nodName == "I_NATIONS" || nodName == "I_NATIONS_2" || nodName == "I_TRADEBOOK" || nodName == "I_TRADEBOOK_2") return;
	}
	switch(nodName)
	{
		case "EQUIP_BUTTON":
			if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — экипировка недоступна для NPC
			if (comName=="activate" || comName=="click")
			{
				EquipPress();
			}
		break;
		case "EXCHANGE":
			if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — обмен товарами с NPC недоступен
			if (comName=="activate" || comName=="click")
			{
				PostEvent("OpenExchange", 150, "a", xi_refCharacter);
				ProcessExitCancel();
			}
		break;
		case "DISCARD_BTN":
			if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не выбрасывать предметы NPC
			if (comName=="activate" || comName=="click")
			{
				DiscardTreasureMap();
			}
		break;

		case "I_CHARACTER_2":
			if (Block) return;
			if(comName=="click")
			{
				nodName = "I_CHARACTER";
				InterfaceStates.ReloadMenuExit = true;
				IDoExit(RC_INTERFACE_ANY_EXIT);
				PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
				return;
			}
		break;
		case "I_SHIP_2":
			if (Block) return;
			if(comName=="click")
			{
				nodName = "I_SHIP";
				InterfaceStates.ReloadMenuExit = true;
				IDoExit(RC_INTERFACE_ANY_EXIT);
				PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
			}
		break;
		case "I_QUESTBOOK_2":
			if (Block) return;
			if(comName=="click")
			{
				nodName = "I_QUESTBOOK";
				InterfaceStates.ReloadMenuExit = true;
				IDoExit(RC_INTERFACE_ANY_EXIT);
				PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
			}
		break;
		case "I_TRADEBOOK_2":
			if (Block) return;
			if(comName=="click")
			{
				nodName = "I_TRADEBOOK";
				InterfaceStates.ReloadMenuExit = true;
				IDoExit(RC_INTERFACE_ANY_EXIT);
				PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
			}
		break;
		case "I_NATIONS_2":
			if (Block) return;
			if(comName=="click")
			{
				nodName = "I_NATIONS";
				InterfaceStates.ReloadMenuExit = true;
				IDoExit(RC_INTERFACE_ANY_EXIT);
				PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
			}
		break;
		case "I_ITEMS_2":
			if (Block) return;
			if(comName=="click")
			{
				nodName = "I_ITEMS";
				InterfaceStates.ReloadMenuExit = true;
				IDoExit(RC_INTERFACE_ANY_EXIT);
				PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
			}
		break;
		case "QTY_ADD_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				ADD_BUTTON(GameInterface.(CurTable).(CurRow).id);
			}
		break;

		case "QTY_ADD_ALL_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				ADD_ALL_BUTTON(GameInterface.(CurTable).(CurRow).id);
			}
		break;

		case "QTY_REMOVE_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				REMOVE_BUTTON(GameInterface.(CurTable).(CurRow).id);
			}
		break;

		case "QTY_REMOVE_ALL_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				REMOVE_ALL_BUTTON(GameInterface.(CurTable).(CurRow).id);
			}
		break;

		case "ITEM_FROM_CHARACTER_OK_BUTTON":
			RemoveItemsQuantity();
		break;
		// boal new menu 31.12.04 -->
		if (nodName == "I_CHARACTER" || nodName == "I_SHIP" ||
			nodName == "I_QUESTBOOK" || nodName == "I_TRADEBOOK" ||
			nodName == "I_NATIONS" || nodName == "I_ITEMS")
		{
			if (Block) return;
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
}

void ProcessFrame()
{
	if (GetCurrentNode() != "EDIT_BOX1") HideEditBox();

	if (sti(GameInterface.CHARACTERS_SCROLL.current)!=nCurScrollNum && GetCurrentNode() == "CHARACTERS_SCROLL")
	{
		nCurScrollNum = sti(GameInterface.CHARACTERS_SCROLL.current);
		SetButtonsState();
		CheckForAvailableItems();
		return;
	}
	if (sti(GameInterface.PASSENGERSLIST.current)!= nCurScrollOfficerNum && GetCurrentNode() == "PASSENGERSLIST")
	{
		nCurScrollOfficerNum = sti(GameInterface.PASSENGERSLIST.current);
		SetOfficersSkills();
		return;
	}
}

bool CanChooseSuitPriority()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return false;
	if (!CheckAttribute(xi_refCharacter, "HeroModel") || !CheckAttribute(xi_refCharacter, "equip.suit")) return false;
	if (xi_refCharacter.model == "protocusto" || CheckAttribute(xi_refCharacter, "Cirgnore")) return false;
	return xi_refCharacter.equip.suit != "";
}

void CheckForSuitCheckbox()
{
	bool show = CanChooseSuitPriority();
	SetNodeUsing("CHECK_SUIT", show);
	if (show) CheckButton_SetState("CHECK_SUIT", 1, CharacterPrefersSuit(xi_refCharacter));
}

void SetButtonsState()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не переключать вид на члена команды, держим проверяемого NPC
	string attributeName = "pic" + (nCurScrollNum+1);
	if(GameInterface.CHARACTERS_SCROLL.(attributeName).character != "0")
	{
		int iCharacter = sti(GameInterface.CHARACTERS_SCROLL.(attributeName).character);
		SaveEquipSet();
		xi_refCharacter = &characters[iCharacter];

		if (CheckAttribute(xi_refCharacter, "prisoned") && xi_refCharacter.prisoned == true) {;}
		else {if (GetRemovable(xi_refCharacter)) isPassenger = 0; else isPassenger = 1;}

		SetVariable();
	}
	else
	{
		if (xi_refCharacter.id != pchar.id)
		{
			xi_refCharacter = pchar;
			isPassenger = 0;
			SetVariable();
		}
	}
}

void SetVariable()
{
	switch (xi_refCharacter.sex)
	{
		case "man": SetNewPicture("SETUP_BIG_PICTURE", "interfaces\sith\CharEquip_Man.dds");
		break;
		case "woman": SetNewPicture("SETUP_BIG_PICTURE", "interfaces\sith\CharEquip_Woman.dds");
		break;
		case "skeleton": SetNewPicture("SETUP_BIG_PICTURE", "interfaces\sith\CharEquip_Skeleton.dds");
		break;
	}

	if (xi_refCharacter.index != nMainCharacterIndex)
	{
		if (CheckForExchangeAllowed(xi_refCharacter)) SetNodeUsing("EXCHANGE", true);
		else SetNodeUsing("EXCHANGE", false);
	}
	else SetNodeUsing("EXCHANGE", false);

	SetFormatedText("SETUP_FRAME_CAPTION", XI_ConvertString("Equipment") + ": " + GetFullName(xi_refCharacter));
	SetControlsTabMode(1);
	for (int i=1;i<7;i++) {FillSetName(i);}//Заполняем имена комплектов снаряжения
	HideItemInfo();
	SetFormatedText("Weight_TEXT", XI_ConvertString("weight") + ": " + FloatToString(GetItemsWeight(xi_refCharacter), 1) + "/"+GetMaxItemsWeight(xi_refCharacter));
	SetFormatedText("Money_TEXT", MakeMoneyShow(sti(xi_refCharacter.Money), MONEY_SIGN,MONEY_DELIVER));

	if (!checkattribute(xi_refCharacter, "selectedSet")) xi_refCharacter.selectedSet = 1;//первое открытие

	int selectedSet = sti(xi_refCharacter.selectedSet);
	bool setChanged = iEQUIP_SET != selectedSet;
	iEQUIP_SET = selectedSet;
	CheckButton_SetState("CHECK_EQUIP_SET", iEQUIP_SET, true);
	ReEquipCharacter(false);
	if (setChanged) FillItemsTable(iCurTab);
	SetNodeUsing("EQUIP_BUTTON" , true);
	SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Equip that"));
}

bool FilterNonGroups(ref rItem, int mode, string itemtype)
{
	switch (mode)
	{
		case 7:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
		case 8:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
		case 9:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
		case 10:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
		case 11:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
		case 12:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
		case 13:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
		case 18:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
		case 19:
			if (CheckAttribute(rItem,"groupID") && HasSubStr(rItem.groupID,itemtype)) return true;
		break;
	}
	return false;
}

void Items_AddEquippedToTable(ref allItems, string sItem)
{
	if (sItem == "") return;
	int iItem = FindItem(sItem);
	if (iItem < 0) return;
	string sKey = "00" + DigitsToString(iItem, 4) + sItem;
	allItems.(sKey) = sItem;
}

void FillItemsTable(int _mode) // 1 - все 2 - оружие 3 - остальное
{
	if (_mode != 18) sAmmoTarget = ""; // не таблица боеприпасов - выбор снова идёт в живой ствол
	int n, i, iTemp;
	string row, sGood, sItem;
	int  idLngFile;
	bool ok, ok2 = true;
	aref rootItems, arItem;
	ref rItem;

	GameInterface.TABLE_ITEMS.hr.td1.str = GetConvertStrWithReplace("Variable_items_2", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ITEMS.hr.td1.scale = 0.8;
	GameInterface.TABLE_ITEMS.hr.td2.str = GetConvertStrWithReplace("Variable_items_3", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ITEMS.hr.td2.scale = 0.8;
	GameInterface.TABLE_ITEMS.hr.td3.str = GetConvertStrWithReplace("Variable_items_4", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ITEMS.hr.td3.scale = 0.8;
	GameInterface.TABLE_ITEMS.hr.td4.str = GetConvertStrWithReplace("Variable_items_5", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ITEMS.hr.td4.scale = 0.8;
	GameInterface.TABLE_ITEMS.select = 0;
	GameInterface.TABLE_ITEMS.top = 0;
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	Table_Clear("TABLE_ITEMS", false, true, false);

	object allItems;

    makearef(rootItems, xi_refCharacter.equip); // экипировка
    n = GetAttributesNum(rootItems);
    for(i = 0; i < n; i++)
    {
        arItem = GetAttributeN(rootItems, i);
        Items_AddEquippedToTable(&allItems, GetAttributeValue(arItem));
    }

	Items_AddEquippedToTable(&allItems, GetSelectedMusket(xi_refCharacter));
	Items_AddEquippedToTable(&allItems, GetParkedPistol(xi_refCharacter));

	makearef(rootItems, xi_refCharacter.Items);
	n = GetAttributesNum(rootItems);
	for (i = 0; i < n; i++)
	{
		arItem = GetAttributeN(rootItems, i);
		sItem = GetAttributeName(arItem);
		if (IsEquipCharacterByItem(xi_refCharacter, sItem)) continue;
		iTemp = FindItem(sItem);
		if (iTemp < 0) continue;
		sGood = DigitsToString(iTemp, 4) + sItem;

		if (CheckAttribute(Items[iTemp], "SortIndex"))
		{
			sGood = DigitsToString(sti(Items[iTemp].SortIndex) + 1, 2) + sGood;
		}
		else
		{
			sGood = "99" + sGood;
		}
		allItems.(sGood) = sItem;
	}

	makearef(rootItems, allItems);
	sort(rootItems);

	n = 1;
	for (i = 0; i < GetAttributesNum(rootItems); i++)
	{
		arItem = GetAttributeN(rootItems, i);
		sGood = GetAttributeValue(arItem);
		rItem = ItemsFromID(sGood);
		if (_mode > 4)
		{
			if (_mode == 5 && !CheckAttribute(rItem,"block")) continue;
			if (_mode == 6)
			{
				if (!CheckAttribute(rItem,"chargeQ")) continue;
				if (isMushket(sGood)) continue;
			}
			if (_mode == 7 && !FilterNonGroups(rItem,_mode,"cirass")) continue;
			if (_mode == 8 && !FilterNonGroups(rItem,_mode,"talisman")) continue;

			if (_mode == 9 && !FilterNonGroups(rItem,_mode,"jewelry_indian_left")) continue;
			if (_mode == 10 && !FilterNonGroups(rItem,_mode,"jewelry_indian_right")) continue;
			if (_mode == 11 && !FilterNonGroups(rItem,_mode,"indian_center")) continue;
			if (_mode == 12 && !FilterNonGroups(rItem,_mode,"idols_left")) continue;
			if (_mode == 13 && !FilterNonGroups(rItem,_mode,"idols_right")) continue;
			if (_mode == 14 && !CheckAttribute(rItem,"scope")) continue;
			if (_mode == 15 && !isMushket(sGood)) continue;
			if (_mode == 16 && !CheckAttribute(rItem,"BackPackWeight")) continue;
			if (_mode == 17 && !CheckAttribute(rItem,"skill")) continue;
			if (_mode == 18 && !FilterNonGroups(rItem,_mode,"ammo")) continue;
			if (_mode == 19 && !FilterNonGroups(rItem,_mode,"suit")) continue;
		}

		row = "tr" + n;
		ok = rItem.ItemType == "WEAPON" || rItem.ItemType == "SUPPORT";
		if (CheckAttribute(rItem,"kind")) ok = true;
		if (CheckAttribute(rItem,"mapSpecial"))
		{
			ok2 = false;
		}
		else
		{
			ok2 = true;
		}
		if (_mode == 1 && rItem.ItemType == "MAP" && ok2) continue;
		if (_mode == 2 && !ok && ok2) continue;
		ok = ok || rItem.ItemType == "MAP";
		if (_mode == 3 && ok && ok2) continue;
		if (_mode == 4 && rItem.ItemType != "MAP" && rItem.id != "MapsAtlas") continue;

		GameInterface.TABLE_ITEMS.(row).id = sGood;

		GameInterface.TABLE_ITEMS.(row).td1.icon.group = rItem.picTexture;
		GameInterface.TABLE_ITEMS.(row).td1.icon.image = "itm" + rItem.picIndex;
		GameInterface.TABLE_ITEMS.(row).td1.icon.offset = "-2, 0";
		GameInterface.TABLE_ITEMS.(row).td1.icon.width = 32;
		GameInterface.TABLE_ITEMS.(row).td1.icon.height = 32;
		GameInterface.TABLE_ITEMS.(row).td1.textoffset = "31,0";
		GameInterface.TABLE_ITEMS.(row).td1.str = LanguageConvertString(idLngFile, rItem.name);

		iTemp = CheckItemInSets(xi_refCharacter, sGood);
		if (iTemp > 0)
		{
			GameInterface.TABLE_ITEMS.(row).td1.str = GameInterface.TABLE_ITEMS.(row).td1.str + GetConvertStrWithReplace("Variable_items_6", "Interface.txt", "#space#", " ") + iTemp + ")";
			GameInterface.TABLE_ITEMS.(row).td1.color = argb(255,196,196,255);//подцвечиваем предметы из комплектов
		}
		if (IsEquipCharacterByItem(xi_refCharacter, sGood)) GameInterface.TABLE_ITEMS.(row).td1.color = argb(255,196,196,255);//на этом интерфейсе надо подцвечивать и текущий комплект???

		GameInterface.TABLE_ITEMS.(row).td1.scale = 0.85;
		GameInterface.TABLE_ITEMS.(row).td2.str   = FloatToString(GetItemWeight(sGood), 1);
		GameInterface.TABLE_ITEMS.(row).td2.scale = 0.9;
		GameInterface.TABLE_ITEMS.(row).td3.str   = GetCharacterItem(xi_refCharacter, sGood);
		GameInterface.TABLE_ITEMS.(row).td3.scale = 0.9;
		GameInterface.TABLE_ITEMS.(row).td4.str   = FloatToString(GetItemWeight(sGood) * sti(GameInterface.TABLE_ITEMS.(row).td3.str), 1);
		GameInterface.TABLE_ITEMS.(row).td4.scale = 0.9;
		n++;
	}

	Table_UpdateWindow("TABLE_ITEMS");
	LanguageCloseFile(idLngFile);
	if (_mode == 1)
	{
		FillItemsSelected();
	}
}

void FillItemsSelected()
{
	CheckForSuitCheckbox();
	int	i;
	string sGood;
	ref item;

	for(i = 0; i < 17; i++) // до ITEM_16 включительно - рамка боеприпаса мушкета
	{
		SetNewGroupPicture("ITEM_"+i, "", "");
	}

	aref arEquip;
	makearef(arEquip, xi_refCharacter.equip);
	int q = GetAttributesNum(arEquip);
	for(i = 0; i < q; i++)
	{
		sGood = GetAttributeValue(GetAttributeN(arEquip, i));
		if (sGood == "") continue;

		item = ItemsFromID(sGood);

		switch (item.groupID)
		{
			case BLADE_ITEM_TYPE:
				SetNewGroupPicture("ITEM_1", item.picTexture, "itm" + item.picIndex);
			break;
			case SPYGLASS_ITEM_TYPE:
				SetNewGroupPicture("ITEM_3", item.picTexture, "itm" + item.picIndex);
			break;
			case CIRASS_ITEM_TYPE:
				SetNewGroupPicture("ITEM_4", item.picTexture, "itm" + item.picIndex);
			break;
			case SUIT_ITEM_TYPE:
				SetNewGroupPicture("ITEM_15", item.picTexture, "itm" + item.picIndex);
			break;
			case BACKPACK_ITEM_TYPE:
				SetNewGroupPicture("ITEM_6", item.picTexture, "itm" + item.picIndex);
			break;
			case TALISMAN_ITEM_TYPE:
				SetNewGroupPicture("ITEM_7", item.picTexture, "itm" + item.picIndex);
			break;
			case INDIAN_CENTER_ITEM_TYPE:
				SetNewGroupPicture("ITEM_8", item.picTexture, "itm" + item.picIndex);
			break;
			case BOOK_ITEM_TYPE:
				SetNewGroupPicture("ITEM_9", item.picTexture, "itm" + item.picIndex);
			break;
			case JEWELRY_INDIAN_LEFT_ITEM_TYPE:
				SetNewGroupPicture("ITEM_10", item.picTexture, "itm" + item.picIndex);
			break;
			case JEWELRY_INDIAN_RIGHT_ITEM_TYPE:
				SetNewGroupPicture("ITEM_11", item.picTexture, "itm" + item.picIndex);
			break;
			case IDOLS_LEFT_ITEM_TYPE:
				SetNewGroupPicture("ITEM_12", item.picTexture, "itm" + item.picIndex);
			break;
			case IDOLS_RIGHT_ITEM_TYPE:
				SetNewGroupPicture("ITEM_13", item.picTexture, "itm" + item.picIndex);
			break;
		}
	}
	SetAmmoSlotPicture("ITEM_2", GetFirearmInSlot(xi_refCharacter, false));
	SetAmmoSlotPicture("ITEM_5", GetFirearmInSlot(xi_refCharacter, true));
	// LEO: у пистоля и мушкета отдельные рамки боеприпасов.
	SetAmmoSlotPicture("ITEM_14", GetAmmoSlotAmmo(xi_refCharacter, false));
	SetAmmoSlotPicture("ITEM_16", GetAmmoSlotAmmo(xi_refCharacter, true));
	SetFormatedText("Weight_TEXT", XI_ConvertString("weight") + ": " + FloatToString(GetItemsWeight(xi_refCharacter), 1) + "/"+GetMaxItemsWeight(xi_refCharacter));
}

//espkk. Returns attack coefficient by weapon. Writes perk multiplier in kPerk
float _GetAttackFactor(string sBladeID, ref rBlade, string sType, ref kPerk)
{
	float min, max, weight;
	GetBladeParams(sBladeID, &min, &max, &weight);

	kPerk = 1;
	float kAttackDmg = 1.0;

	float HeavyW = 1.0;//модификатор урона для тяжа
	float fgtlevel = 0.0;
	fgtlevel = GetCharacterSkill(xi_refCharacter, rBlade.FencingType); //определение скила по типу меча
	fgtlevel = fgtlevel/SKILL_MAX;//вычисление флоата

	float bladeDmg = min + (max - min)*fgtlevel;

	switch(sType)
	{
		case "fast":
		kAttackDmg = 0.7;
		if (bAltBalanceProHits && rBlade.FencingType != "Fencing") kAttackDmg *= 0.7;
		break;

		case "force":
		kAttackDmg = 1.0;
		if (bAltBalanceProHits && rBlade.FencingType != "FencingLight") kAttackDmg *= 0.7;
		break;

		case "round":
		kAttackDmg = 0.6;
		if(CheckCharacterPerk(xi_refCharacter, "BladeDancer"))
		{
			kAttackDmg = kAttackDmg * 1.3;
		}
		if (bAltBalanceProHits && rBlade.FencingType != "Fencing") kAttackDmg *= 0.7;
		break;

		case "break":
		kAttackDmg = 3.0;
		if (bAltBalanceProHits && rBlade.FencingType != "FencingHeavy") kAttackDmg *= 0.7;
		break;

		case "fient":
		kAttackDmg = 0.5;
		if(CheckCharacterPerk(xi_refCharacter, "Agent")) kAttackDmg = kAttackDmg * 2;
		if (bAltBalanceProHits && rBlade.FencingType != "FencingLight") kAttackDmg *= 0.7;
		break;
	}
	float dmg = bladeDmg * kAttackDmg;

	return dmg;
}

string CheckForSpecial(string type)
{
	string weaponID = GetCharacterEquipByGroup(xi_refCharacter, BLADE_ITEM_TYPE);
	string sFencingType = LAi_GetBladeFencingType(xi_refCharacter);
	string sText = "";
	int iFencingType = 1 * (sFencingType == "Fencing") + 2 * (sFencingType == "FencingHeavy");
	float fSkill = 0.01 * GetCharacterSkill(xi_refCharacter, sFencingType);
	int critvalue = 0;//это... критический эффект.
	bool bAttackHardHitter = CheckCharacterPerk(xi_refCharacter, "HardHitter");
	float coeff = 0.0;
	if (weaponID != "")
	{
		aref weapon;
		Items_FindItem(weaponID, &weapon);
		bool bSpecial = CheckAttribute(weapon, "special."+type);//чек сразу нужного?
		switch (type)
		{
			case "valueB":
				critvalue = LAi_CalcBloodingChance(xi_refCharacter, iFencingType, fSkill, 0);
				sText = critvalue + "%";
				if (critvalue) {
					sText += " (" + BLOODING_MIN_TIME + "-" + LAi_CalcBloodingDuration(xi_refCharacter, iFencingType, fSkill, 0) + " " + GetConvertStrWithReplace("Variable_character_all_35", "Interface.txt", "#space#", " ") + ")";
				}
			break;
			case "valueCrB"://crit
				if (bSpecial) {
					critvalue = sti(weapon.special.valueCrB);
				}
				sText = LAi_CalcCritChance(xi_refCharacter, 0, iFencingType, critvalue)+"% (x"+LAi_CalcCritDamageMult(xi_refCharacter)+")";
			break;
			case "valueSS":
				critvalue = LAi_CalcSwiftChance(xi_refCharacter, iFencingType, fSkill);
				sText = critvalue + "%";
				if (critvalue) {
					coeff = LAi_CalcSwiftDuration(xi_refCharacter, iFencingType, fSkill, 0);
					sText += " (" + coeff + "-" + (coeff + SWIFT_TIME_RAND) + GetConvertStrWithReplace("Variable_character_all_28", "Interface.txt", "#space#", " ") + ")";
				}
			break;
			case "valueBB"://break
				sText = LAi_CalcBlockBreakChance(xi_refCharacter, sFencingType, fSkill) + "% / " + LAi_CalcCuirassBreakChance(xi_refCharacter, iFencingType, bAttackHardHitter, fSkill) + "%";
			break;
			case "valueStS":
				critvalue = xi_refCharacter.chr_ai.special.(type);
				sText = critvalue + "%";
				if (critvalue) {
					sText += " (" + CalcStunDuration(0) + "-" + (CalcStunDuration(0) + STUN_TIME_RAND) + GetConvertStrWithReplace("Variable_character_all_28", "Interface.txt", "#space#", " ") + ")";
				}
			break;
			case "valueT":
				critvalue = xi_refCharacter.chr_ai.special.(type);
				sText = critvalue + "%";
				if (critvalue) {
					sText += " (" + CalcTraumaDuration(0) + "-" + (CalcTraumaDuration(0) + TRAUMA_TIME_RAND) + GetConvertStrWithReplace("Variable_character_all_28", "Interface.txt", "#space#", " ") + ")";
				}
			break;
			case "valueP":
				critvalue = LAi_CalcPoisonChance(xi_refCharacter);
				sText = critvalue + "%";
				if (critvalue) {
					sText += " (" + LAi_CalcPoisonDuration(0) + "-" + (LAi_CalcPoisonDuration(0) + 2 * POISON_TIME_RAND) + GetConvertStrWithReplace("Variable_character_all_28", "Interface.txt", "#space#", " ") + ")";
				}
			break;
		}
	}
	else//а нужно ли?
	{
		if (type == "valueCrB") {
			return LAi_CalcCritChance(xi_refCharacter, 0, 0, 0)+"% (x"+LAi_CalcCritDamageMult(xi_refCharacter)+")";
		}
		else {
			return 0+"%";
		}
	}
	return sText;
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sPicture;
	string sGroup, sGroupPicture;
	int iText2Color = 0;

	//Boyer fix #20170401-01 Can't load texture -1.tx log errors
	sPicture = "";
	string sAttributeName;
	int nChooseNum = -1;
	switch (sCurrentNode)
	{
	}
	sHeader = GetConvertStrWithReplace("Variable_items_7", "Interface.txt", "#space#", " ");
	sText1  = GetConvertStrWithReplace("Variable_items_8", "Interface.txt", "#space#", " ")+
			  GetConvertStrWithReplace("Variable_items_9", "Interface.txt", "#space#", " ")+
			  GetConvertStrWithReplace("Variable_items_10", "Interface.txt", "#space#", " ");

	// LEO: у ПУСТОЙ строки нет атрибута id, а его читали не глядя: ItemsFromID возвращал ничего, rBlade
	//   оставался неинициализированным, и подсказка валилась пачкой RUNTIME ERROR. Всплывало после
	//   подбора квестового предмета - таблица перестраивается, а выбор остаётся за её концом.
	//   Флагом, а не ранним return: дойти до CreateTooltip обязательно, иначе подсказка залипнет.
	bool bRowHasItem = false;
	if (sCurrentNode == "TABLE_ITEMS")
	{
		if (CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".id")) bRowHasItem = true;
	}
	if (bRowHasItem)
	{
		string sItemID = GameInterface.(CurTable).(CurRow).id;
		ref rBlade = ItemsFromID(sItemID);
		if (ThisItemCanBeEquip(sItemID))
		{
			if(rBlade.groupID == BLADE_ITEM_TYPE)
			{
				sHeader = GetConvertStrWithReplace("Variable_items_11", "Interface.txt", "#space#", " ")
				sText1 = "";

				int ControlsLngFile = LanguageOpenFile("ControlsNames.txt");
				string sEquippedWeapon = GetCharacterEquipByGroup(xi_refCharacter, BLADE_ITEM_TYPE);
				if (sItemID != sEquippedWeapon) {
					EquipCharacterByItem(xi_refCharacter, sItemID);//прицениваемся к оружию
				}
				float aSkill = LAi_GetCharacterFightLevel(xi_refCharacter);
				float kAttackDmg, kPerk;
				string sFencingType = LAi_GetBladeFencingType(xi_refCharacter);
				int iFencingType = 1 * (sFencingType == "Fencing") + 2 * (sFencingType == "FencingHeavy");
				int iCritical;
				string sAttackTypes[5];
				sAttackTypes[0] = "fast";
				sAttackTypes[1] = "force";
				sAttackTypes[2] = "round";
				sAttackTypes[3] = "break";
				sAttackTypes[4] = "fient"; //feint = fient in lang file

				// столбцы корректированных эффектов
				// TODO: переписать под func, связав значок, название метода, аттрибута, значение и доп параметр. разделить пробитие блока и кирасы в "персонаже"
				aref arSpecial; makearef(arSpecial, xi_refCharacter.chr_ai.special);
				aref arProp;
				string sName, sVal, sVal2, sVal3, sSymb;
				sText2 = "";
				for (int j = 0; j < GetAttributesNum(arSpecial); j++)
				{
					arProp = GetAttributeN(arSpecial, j);
					sName = GetAttributeName(arProp);
					sVal = CheckForSpecial(sName);
					switch(sName)
					{
						case "valueBB":
							sSymb = "Variable_character_all_47.1";
							sVal2 = "‡ƒ" + sVal;
						break; 
						case "valueCrB":
							sSymb = "Variable_character_all_47.2";
							sVal2 = "„" + sVal;
						break;
						case "valueSS":
							sSymb = "Variable_character_all_47";
							sVal2 = "…" + sVal;
						break;
						case "valueStS":
							sSymb = "Variable_character_all_48";
							sVal2 = "†" + sVal;
						break;
						case "valueT":
							sSymb = "Variable_character_all_49";
							sVal2 = "Š" + sVal;
						break;
						case "valueB":
							sSymb = "Variable_character_all_46";
							sVal2 = "‰" + sVal;
						break;
						case "valueP":
							sSymb = "Variable_character_all_50";
							sVal2 = "ˆ" + sVal;
						break;
					}
					if (sVal != "" && sVal != "0%") {
						sText2 += GetConvertStrWithReplace(sSymb, "Interface.txt", "#space#", " ") + " | ";
						sVal3 += sVal2 + " | ";
					}
				}
				sText2 += "\n" + sVal3;
				sSymb = "";
				sText2 += "\n---------------------------------------------------------------------------------\n";
				sText2 += GetConvertStrWithReplace("Variable_character_all_36", "Interface.txt", "#space#", " ") + " - " + GetConvertStrWithReplace("Variable_ship_37", "Interface.txt", "#space#", " ") + ", ";
				sText2 += XI_ConvertString("Energy") + ":\n";
				for (int index = 0; index < 5; index++)
				{
					kAttackDmg = LAi_CalcDamageForBlade(xi_refCharacter, aSkill, index, rBlade.FencingType, false, false, false);

					sText2 += "\n" + LanguageConvertString(ControlsLngFile, "ChrAttack" + sAttackTypes[index]) + ":\n";// + GetConvertStrWithReplace("Variable_ship_37", "Interface.txt", "#space#", " ") + " - ";
					sText2 += MakeInt(kAttackDmg)+"Â";
					if (index == 4) { sAttackTypes[index] = "feintc";}
					// энергия
					sText2 += "   " + FloatToString(LAi_CalcUseEnergyForBlade(xi_refCharacter, sAttackTypes[index], aSkill), 1)+"Î";
					// крит
					sText2 += "\n";
					sVal3 = "";
					for (j = 0; j < GetAttributesNum(arSpecial); j++)
					{
						arProp = GetAttributeN(arSpecial, j);
						sName = GetAttributeName(arProp);
						sVal = CheckForSpecial(sName);
						sVal2 = "";	
						switch(sName)
						{
							case "valueCrB":
								sVal2 = LAi_CalcCritChance(xi_refCharacter, index, sFencingType, sti(xi_refCharacter.chr_ai.special.valueCrB)) + "% (x" + LAi_CalcCritDamageMult(xi_refCharacter) + ")";
								sSymb = "„";
							break;
							case "valueB":
								iCritical = LAi_CalcBloodingChance(xi_refCharacter, iFencingType, aSkill, index);
								sVal2 = iCritical + "%";
								if (iCritical) {
									sVal2 += " (" + BLOODING_MIN_TIME + "-" + LAi_CalcBloodingDuration(xi_refCharacter, iFencingType, aSkill, 0) + " " + GetConvertStrWithReplace("Variable_character_all_35", "Interface.txt", "#space#", " ") + ")";
								}
								sSymb = "‰";
							break;
						}
						if (sVal != "" && sVal2 != "" && sVal != "0%" && sVal != sVal2) {
							sVal3 += " " + sSymb + sVal2 + " ";
						}
					}
					if (sVal3 != "") {
						sText2 += GetConvertStrWithReplace("Variable_quest_table_spec", "Interface.txt", "#space#", " ") + ": " + sVal3 + "\n";
					}
				}
				if (sItemID != sEquippedWeapon) {
					EquipCharacterByItem(xi_refCharacter, sEquippedWeapon);//возвращаем старое
				}
				sGroup = rBlade.picTexture;
				sGroupPicture = "itm" + rBlade.picIndex;

				LanguageCloseFile(ControlsLngFile);
			}
		}
	}

	if (sCurrentNode == "CHECK_EQUIP_SET")
	{
		sHeader = GetConvertStrWithReplace("Variable_items_12", "Interface.txt", "#space#", " ");
		sText1  = XI_ConvertString("KitHint");
	}
	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,255,255), "", 0, "", 0, "-1", sGroup, sGroupPicture, 64, 64);
}
void HideInfoWindow()
{
	CloseTooltip();
}

void TableSelectChange()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не выбирать/двигать товары NPC
	string sControl = GetEventData();
	iSelected = GetEventData();
	CurTable = sControl;
	CurRow   =  "tr" + (iSelected);
	// отрисовка инфы
	SetItemInfo();
}

void SetItemInfo()
{
	string sItemID = GameInterface.(CurTable).(CurRow).id;
	if (sItemID == "letter_LSC" && CheckAttribute(pchar, "questTemp.LSC.poorName"))
	{
        AddQuestRecord("ISS_PoorsMurder", "11");
        AddQuestUserData("ISS_PoorsMurder", "sSex", GetSexPhrase(GetConvertStrWithReplace("Variable_items_13", "Interface.txt", "#space#", " "),GetConvertStrWithReplace("Variable_items_14", "Interface.txt", "#space#", " ")));
        AddQuestUserData("ISS_PoorsMurder", "sName", pchar.questTemp.LSC.poorName);
        pchar.questTemp.LSC = "readyGoLSC";
        DeleteAttribute(pchar, "questTemp.LSC.poorName");
        int n = FindIsland("LostShipsCity");
        Islands[n].visible = true;
        Islands[n].reload_enable = true;
        Islands[n].alwaysStorm = true; //живём в штормах
        Islands[n].MaxSeaHeight = 2.0;
        Islands[n].storm = true;
        Islands[n].tornado = true;
	}
	// Gregg: «Чёрная Орхидея» - шифр от книги Брайана Джорджа выбит на монете из его же
	// сундука. Механика та же, что у письма выше: осмотрел предмет - сработало. В К:ПС
	// вместо этого был отдельный экран-кодовый замок (UnlockBook), которого у нас нет.
	// Проверки раздельные: короткого замыкания в Storm нет, а атрибута может не быть вовсе.
	if (sItemID == "MysteriousArtefact_Coin")
	{
		if (CheckAttribute(pchar, "MysteriousArtefact"))
		{
			// "15" - Ланвен уже попросил найти комбинацию, но книга ещё заперта
			if (pchar.MysteriousArtefact == "15")
			{
				// Gregg: по имени, а не напрямую - сегмент интерфейса компилируется
				// отдельно, и связывать его с blackorchid.c на этапе компиляции незачем
				DoQuestFunctionDelay("MysteriousArtefactOpenBook", 0.1);
			}
		}
	}
	ref item = ItemsFromID(sItemID);
	SetFormatedText("INFO_TEXT", GetItemDescribe(sItemID));
	SetNewGroupPicture("INFO_PIC", item.picTexture, "itm" + item.picIndex);
	SetNodeUsing("INFO_TEXT", true);
	SetNodeUsing("INFO_PIC", true);
	SetVAligmentFormatedText("INFO_TEXT");

	SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Equip that"));
	SetSelectable("EQUIP_BUTTON",ThisItemCanBeEquip(sItemID));
}

void HideItemInfo()
{
	SetNodeUsing("INFO_TEXT", false);
	SetNodeUsing("INFO_PIC", false);
	SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Equip that"));
	SetSelectable("EQUIP_BUTTON",false);
}

void procTabChange()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();
	SetCurrentNode("TABLE_ITEMS");
	HideItemInfo();
	if( sNodName == "TABBTN_1" )
	{
		SetControlsTabMode( 1 );
		return;
	}
	if( sNodName == "TABBTN_2" )
	{
		SetControlsTabMode( 2 );
		return;
	}
	if( sNodName == "TABBTN_3" )
	{
		SetControlsTabMode( 3 );
		return;
	}
	if( sNodName == "TABBTN_4" )
	{
		SetControlsTabMode( 4 );
		return;
	}
}

void SetControlsTabMode(int nMode)
{
	iCurTab = nMode;
	int nColor1 = argb(255,196,196,196);
	int nColor2 = nColor1;
	int nColor3 = nColor1;
	int nColor4 = nColor1;

	string sPic1 = "TabSelected";
	string sPic2 = sPic1;
	string sPic3 = sPic1;
	string sPic4 = sPic1;

	switch (nMode)
	{
		case 1: //
			sPic1 = "TabDeSelected";
			nColor1 = argb(255,255,255,255);
		break;
		case 2:
			sPic2 = "TabDeSelected";
			nColor2 = argb(255,255,255,255);
		break;
		case 3:
			sPic3 = "TabDeSelected";
			nColor3 = argb(255,255,255,255);
		break;
		case 4:
			sPic4 = "TabDeSelected";
			nColor4 = argb(255,255,255,255);
		break;
	}

	SetNewGroupPicture("TABBTN_1", "TABS", sPic1);
	SetNewGroupPicture("TABBTN_2", "TABS", sPic2);
	SetNewGroupPicture("TABBTN_3", "TABS", sPic3);
	SetNewGroupPicture("TABBTN_4", "TABS", sPic4);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_1", 8,0,nColor1);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_2", 8,0,nColor2);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_3", 8,0,nColor3);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_4", 8,0,nColor4);
	FillControlsList(nMode);
}

void FillControlsList(int nMode)
{
	if ((nMode >= 1) && (nMode <= 4))
	{
		FillItemsTable(nMode);
	}
}

bool ThisItemCanBeEquip(string sItemID)
{
	ref rItem = ItemsFromID(sItemID);
	if (CheckAttribute(xi_refCharacter,"nonremovable")) return false;
	if (HasSubStr(loadedLocation.id,"FencingTown")) return false;
	if (rItem.id == "suit_2" && CheckAttribute(pchar, "questTemp.AnjelikaTichPrologue5")) //Пролог Анжелика Тич, не даём снять юбку
    {
        SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Remove that"));
        return false;
    }
	if (HasSubStr(sItemId,"Tube"))
	{
		return true;
	}
	if (HasSubStr(sItemId,"Strange"))
	{
		if (!CheckAttribute(xi_refCharacter, "StrangeMushrooms")) xi_refCharacter.StrangeMushrooms = 0;
		if (!CheckAttribute(xi_refCharacter, "StrangePotion")) xi_refCharacter.StrangePotion = 0;

		if (sItemID ==  "StrangeMushrooms" && sti(xi_refCharacter.StrangeMushrooms) < 5) return true;
		else {if (sItemID ==  "StrangePotion" && sti(xi_refCharacter.StrangePotion) < 5) return true;}
		else {if (sItemID ==  "StrangeElixir" && !CheckAttribute(xi_refCharacter, "StrangeElixir")) return true;}
	}
	if( !CheckAttribute(rItem,"groupID") )
	{
		return false;
	}
	string sGroupID = rItem.groupID;
	if (sGroupID == AMMO_ITEM_TYPE)
	{
		if (sItemID == "GunPowder" || sItemID == "GunCap_colt" || sItemID == "shotgun_bullet" || sItemID == "12_gauge") return false;
		SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Equip that"));
	}
	if( !IsCanEquiping(xi_refCharacter, sGroupID) )
	{
		return false;
	}
	if(sItemID == "MapsAtlas")
	{
		return true;
	}

	if (sGroupID == BOOK_ITEM_TYPE)
	{
		string sId = rItem.id;
		if (!CheckAttribute(xi_refCharacter, "books." + sId))
		{
			return true;
		}
		return sti(xi_refCharacter.books.(sId)) > 0;
	}

	if (sGroupID == BLADE_ITEM_TYPE && CheckAttribute(xi_refCharacter, "DontChangeBlade")) return false;
	if (sGroupID == GUN_ITEM_TYPE && CheckAttribute(xi_refCharacter, "DontChangeGun")) return false;
	if (sGroupID == GUN_ITEM_TYPE && IsDouglasGunLocked(xi_refCharacter)) return false;

	if (sGroupID == GUN_ITEM_TYPE)
	{
		if (!isMushket(sItemID))
		{
			if (CheckAttribute(xi_refCharacter, "MusketLoadout.Drawn")) return false;
		}
		if (!CheckAttribute(rItem,"chargeQ") )
		{
			return false;
		}
		if(sItemID == "mushket2x2") return false; // Мушкет квестового офа

		if (CheckAttribute(rItem,"ReqPerk"))
		{
			if (rItem.ReqPerk == "Gunman" && !IsCharacterPerkOn(xi_refCharacter,"Gunman") && !IsCharacterPerkOn(xi_refCharacter,"GunProfessional")) return false;
			if (rItem.ReqPerk == "GunProfessional" && !IsCharacterPerkOn(xi_refCharacter,"GunProfessional")) return false;
		}

		// Нельзя экипировать мушкет в непредназначенных для этого локациях (Таверна)
	}
	if (IsEquipCharacterByItem(xi_refCharacter, sItemID))
	{
		SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Remove that"));
		if(sGroupID==PATENT_ITEM_TYPE)	//aw013
		{
			if (CheckAttribute(pchar,"questTemp.Ascold"))
			{
				if (pchar.questTemp.Ascold == "ReciveInformationManowar" || pchar.questTemp.Ascold == "Ascold_ImMummy")	return true;
			}
			return false;
		}
	}
	else
	{
		if (sGroupID == CIRASS_ITEM_TYPE && rItem.Clothes == false)
		{
			int cir = 0;
			if (IsCharacterPerkOn(xi_refCharacter,"Ciras")) cir = 5;
			else if (IsCharacterPerkOn(xi_refCharacter,"AdvancedDefense")) cir = 3;
			else if (IsCharacterPerkOn(xi_refCharacter,"BasicDefense")) cir = 1;
			if (sti(rItem.model) > cir) return false;
		}

		if (HasSubStr(sItemID, "chest")) SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Crack"));
		else SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Equip that"));
		if (sItemID == "Chest") {SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Equip that")); return false;}
	}
	if (xi_refCharacter.id == "OfMush1" || xi_refCharacter.id == "OfMush2")
	{
		if(HasSubStr(sItemID, "mushket"))
		{
			if(IsEquipCharacterByItem(xi_refCharacter, sItemID))
			{
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Remove that"));
				return false;
			}
			else
			{
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Equip that"));
				return true;
			}
		}
		if(sGroupID == BLADE_ITEM_TYPE || sGroupID == GUN_ITEM_TYPE)
		{
			return false;
		}
	}
	if (isMushket(sItemID) && !IsFixedMushketer(xi_refCharacter) &&
		sItemID != GetSelectedMusket(xi_refCharacter) && !SupportsMusketLoadout(xi_refCharacter)) return false;
	// Gregg: в скафандре не переодеваются. Кираса была закрыта и раньше, костюм - нет,
	// а он в SetEquipedItemToCharacter (CharacterUtilite.c) перезаписывает model из
	// HeroModel: скафандр слетал, анимация "armor" оставалась, и обычная модель играла
	// чужую анимацию. Сравнение строк в Storm регистронезависимо, "Protocusto" ловит
	// и "protocusto", которым модель ставит квест (items_utilite.c).
	if (xi_refCharacter.model == "Protocusto")
	{
		if (sGroupID == CIRASS_ITEM_TYPE) return false;
		if (sGroupID == SUIT_ITEM_TYPE) return false;
	}
	return true;
}

void EquipPress()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не экипировать предметы NPC
	string sItemID = GameInterface.(CurTable).(CurRow).id;
	ref itmRef = ItemsFromID(sItemID);
	string totalInfo;
	int  i;

	if (CheckAttribute(itmRef, "groupID"))
	{
		string itmGroup = itmRef.groupID;

		if (itmGroup == BLADE_ITEM_TYPE && CheckAttribute(xi_refCharacter, "DontChangeBlade")) return false;
		if (itmGroup == GUN_ITEM_TYPE && CheckAttribute(xi_refCharacter, "DontChangeGun")) return false;
		if (itmGroup == GUN_ITEM_TYPE && IsDouglasGunLocked(xi_refCharacter)) return;

		if (itmGroup == MAPS_ITEM_TYPE)
		{
			SetSelectable("DISCARD_BTN",false);
			// Warship. Отличная карта, у нее отдельный интерфейс
			if(sItemID == "Map_Best")
			{
				PChar.ShowBestMap = true; // Флаг, что смотрим из интерфейса придметов
				IDoExit(RC_INTERFACE_BEST_MAP);
				return;
			}
			// Ugeen --> интерфейс атласа карт
			if(sItemID == "MapsAtlas") // взяли атлас карт
			{
				PChar.ShowMapsAtlas = true; // Флаг, что смотрим из интерфейса предметов
				IDoExit(RC_INTERFACE_MAPVIEW);
				return;
			}
			// Ugeen --> специальная квестовая карта
			if(sItemID == "mapQuest")
			{
				totalInfo = GenQuest_GetQuestTreasureMapDescription(itmRef);
				SetNewPicture("MAP_PICTURE", "interfaces\Maps\treasure map.dds");
				SetFormatedText("MAP_TEXT", totalInfo);
				SetVAligmentFormatedText("MAP_TEXT");
				ShowMapWindow();
				return;
			}
			if (sItemID == "map_full" || sItemID == "map_part1" || sItemID == "map_part2")
			{// клады
				SetSelectable("DISCARD_BTN",true);
				SetNewPicture("MAP_PICTURE", "interfaces\Maps\map_1.dds");
				if (GetCharacterItem(pchar, "map_part1")>0  && GetCharacterItem(pchar, "map_part2")>0 && GetCharacterItem(pchar, "map_full")<=0)
				{
					TakeNItems(xi_refCharacter, "map_part1", -1);
					TakeNItems(xi_refCharacter, "map_part2", -1);
					TakeNItems(pchar, "map_full",   1);
					itmRef = ItemsFromID("map_full");
					// здесь генерация назначение и типа клада
					pchar.GenQuest.TreasureBuild = true;
					FillMapForTreasure(itmRef);
					SetVariable();
				}
				// тут применяем логику двух кусков, из них одна карта <--
				if (itmRef.mapType == "Full")
				{
					if (sti(itmRef.MapTypeIdx) == -1)
					{
						SetFormatedText("MAP_TEXT", GetConvertStr("type_fake", "MapDescribe.txt"));
						DeleteAttribute(itmRef, "MapIslId");
						TakeNItems(GetMainCharacter(), "map_full", -1);
					}
					else
					{
						// LEO: старые сейвы - карта выдавалась без генерации, условие прихода в локацию не
						//   ставилось: клад не появлялся, а карта не исчезала. Чиним при первом же взгляде.
						//   Только для УЖЕ заполненной карты: незаполненной условие поставит генерация ниже,
						//   и она же решает, не фальшивка ли это.
						if (CheckAttribute(itmRef, "MapIslId") && !CheckAttribute(pchar, "quest.SetTreasureFromMap"))
						{
							ArmTreasureMapCondition(itmRef);
						}
						if (!CheckAttribute(itmRef, "MapIslId"))
						{
							FillMapForTreasure(itmRef); //заполним если смотрим карту из сундука
						}
						i = FindLocation(itmRef.MapLocId);  // ищем ареал
						if (i != -1 && locations[i].islandId != "Mein")
						{
							totalInfo = GetConvertStr(locations[i].islandId, "LocLables.txt");
							totalInfo = GetConvertStr("type_full_" + itmRef.MapTypeIdx + "_isl", "MapDescribe.txt") + " " + totalInfo;
						}
						else
						{
							totalInfo = GetConvertStr("type_full_" + itmRef.MapTypeIdx + "_mein", "MapDescribe.txt");
						}
						itmRef.MapIslName = totalInfo;
						totalInfo = GetConvertStr(itmRef.MapLocId, "MapDescribe.txt") + ", " + GetConvertStr(itmRef.MapLocId + "_" + itmRef.MapBoxId, "MapDescribe.txt");
						itmRef.MaplocName = totalInfo;

						totalInfo = GetConvertStr("type_full_" + itmRef.MapTypeIdx, "MapDescribe.txt");

						if (CheckAttribute(itmRef,"uniquep"))
						{
							string pirname = GetUniquePirateNameString(itmRef.uniquep);
							totalInfo = "Это #sMapIslName#. На карте указано место спрятанного клада - это #sMaplocName#.\nСудя по всему - это подлинная карта сокровищ легендарного пирата "+pirname+"."
						}

						totalInfo = GetAssembledString(totalInfo, itmRef);
						sMapDescribe = totalinfo;//в глобальную переменную, чтобы можно было в другой функции показывать/скрывать текст
						SetMapCross(itmRef.MapLocId);
					}
				}
				else
				{
		   			SetFormatedText("MAP_TEXT", GetConvertStr("type_part", "MapDescribe.txt"));
				}
				SetVAligmentFormatedText("MAP_TEXT");
			}
			else
			{
				SetNewPicture("MAP_PICTURE", "interfaces\Maps\" + itmRef.imageTga + ".dds");
				SetFormatedText("MAP_TEXT", "");
			}
			ShowMapWindow();
		}
		else
		{
			if (itmGroup == SPECIAL_ITEM_TYPE) // Jason, спецпредметы
			{
				if (itmRef.id == "potion7") // слёзы Иш-Чель
				{
					if (IsMainCharacter(xi_refCharacter)) // ГГ
					{
						Log_Info(GetConvertStrWithReplace("Variable_items_15", "Interface.txt", "#space#", " "));
						PlaySound("Ambient\Tavern\glotok_001.wav");
						AddCharacterHealth(xi_refCharacter, 10);
						AddCharacterMaxHealth(xi_refCharacter, 4);
						RemoveItems(xi_refCharacter, "potion7", 1);
						FillItemsTable(iCurTab);
					}
					else PlaySound("interface\knock.wav");
					return;
				}
				if (sItemID == "RingCapBook") // СЖ пинаса 'Санта-Люсия' и дневник Колхауна
				{
					RemoveItems(pchar, sItemID, 1);
					AddTimeToCurrent(0,20);
					SetLaunchFrameFormParam(GetConvertStrWithReplace("Variable_items_16", "Interface.txt", "#space#", " "), "", 0.1, 5.0);
					LAi_Fade("","LaunchFrameForm");
					RefreshLandTime();
					AddQuestRecord("RingCapBook", "1");
					pchar.questTemp.LSC.Ring.ReadCapBook = "true";
				}

				if(HasSubStr(sItemID, "chest") && sItemID != "Chest" && xi_refCharacter.id == pchar.id)
				{
					if(CheckCharacterItem(xi_refCharacter, "Lockpick"))
					{
						pchar.questTemp.lockpicking.ID = sItemID;
						IDoExit(RC_INTERFACE_LOCKPICK);
						return;
					}
					else
					{
						Log_Info(GetConvertStrWithReplace("Variable_items_17", "Interface.txt", "#space#", " "));
						PlaySound("interface\knock.wav");
					}
				}
			}
			else
			{
				bool bCanmakeMushketer = IsMainCharacter(xi_refCharacter) || IsOfficer(xi_refCharacter) ||
					CheckAttribute(xi_refCharacter, "CanTakeMushket");
				if(isMushket(sItemID) && bCanmakeMushketer)
				{
					if (IsFixedMushketer(xi_refCharacter)) // из режима не выходят - только меняют мушкет
					{
						EquipCharacterByItem(xi_refCharacter, sItemID);
						if (GetCharacterEquipByGroup(xi_refCharacter, GUN_ITEM_TYPE) == sItemID)
						{
							xi_refCharacter.mushket = sItemID;
						}
					}
					else
					{
						if (GetSelectedMusket(xi_refCharacter) == sItemID) UnequipIntegratedMusket(xi_refCharacter);
						else SetCharacterToMushketer(xi_refCharacter, sItemID, true);
						FillItemsSelected();
						SetItemInfo();
						return;
					}
				}
				else
				{
					// LEO: боеприпас - отдельной веткой и раньше всего: EquipCharacterByItem записал бы curammo
					//   ДО проверки совместимости (и двойной else в Storm здесь работал случайно)
					if (itmGroup == AMMO_ITEM_TYPE)
					{
						EquipAmmoFromTable(sItemID, itmRef);
						return;
					}
					if(IsEquipCharacterByItem(xi_refCharacter, sItemID))
					{
						RemoveCharacterEquip(xi_refCharacter, itmGroup);
						if (CheckAttribute(itmRef, "HeadAccessory")) SetNewModelToChar(xi_refCharacter);
					}
					else
					{
						EquipCharacterByItem(xi_refCharacter, sItemID);
					}
					if (sItemID == "hatWhisper")
					{
						ChangeWhisperHeroModel();
					}
				}
			}
			if (itmGroup == GUN_ITEM_TYPE) LAi_GunSetUnload(xi_refCharacter);
			if(itmGroup == GUN_ITEM_TYPE && sti(xi_refCharacter.index) == GetMainCharacterIndex() && CheckAttribute(xi_refCharacter, "chargestage."+sItemID)) DeleteAttribute(xi_refCharacter, "chargestage."+sItemID);
			FillItemsSelected();
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BUTTON",0, "#"+XI_ConvertString("Equip that"));
			SetSelectable("EQUIP_BUTTON",ThisItemCanBeEquip(sItemID));
			SetItemInfo();
		}
	}
	if (HasSubStr(sItemID,"Strange"))
	{
		int strangeHpBonus = 5;
		int strangeEnBonus = 2;
		if (HasSubStr(xi_refCharacter.model,"Lejitos") || HasSubStr(xi_refCharacter.model,"Shicoba") || xi_refCharacter.id == "Tichingitu")
		{
			//Персонажи с индейским прошлым получают более сильный эффект
			strangeHpBonus = 10;
			strangeEnBonus = 4;
		}

		if (sItemID == "StrangeMushrooms" && sti(xi_refCharacter.StrangeMushrooms) < 5)
		{
			LAi_SetHP (xi_refCharacter,1,LAi_GetCharacterMaxHP(xi_refCharacter) + strangeHpBonus);
			xi_refCharacter.StrangeMushrooms = sti(xi_refCharacter.StrangeMushrooms)+1;
			PlaySound("interface\heartbeat.wav");
			if (IsMainCharacter(xi_refCharacter)) Log_Info(GetConvertStrWithReplace("Variable_items_19", "Interface.txt", "#space#", " ")+strangeHpBonus+"!");
			else Log_Info(GetFullName(xi_refCharacter)+GetConvertStrWithReplace("Variable_items_20", "Interface.txt", "#space#", " ")+strangeHpBonus+"!")
			ApplayNewSkill(pchar, "", 0);
			TakeNItems(xi_refCharacter, sItemID, -1);
			FillItemsTable(iCurTab);
		}
		else
		{
			if (sItemID == "StrangePotion" && sti(xi_refCharacter.StrangePotion) < 5)
			{
				LAi_SetHP (xi_refCharacter,1,LAi_GetCharacterMaxHP(xi_refCharacter));
				AddBonusEnergyToCharacter(xi_refCharacter,strangeEnBonus);
				xi_refCharacter.StrangePotion = sti(xi_refCharacter.StrangePotion)+1;
				PlaySound("interface\heartbeat.wav");
				if (IsMainCharacter(xi_refCharacter)) Log_Info(GetConvertStrWithReplace("Variable_items_21", "Interface.txt", "#space#", " ")+strangeEnBonus+"!");
				else {Log_Info(GetFullName(xi_refCharacter)+GetConvertStrWithReplace("Variable_items_22", "Interface.txt", "#space#", " ")+strangeEnBonus+"!")}
				ApplayNewSkill(pchar, "", 0);
				TakeNItems(xi_refCharacter, sItemID, -1);
				FillItemsTable(iCurTab);
			}
			else
			{
				if (sItemID == "StrangeElixir" && !CheckAttribute(xi_refCharacter, "StrangeElixir"))
				{
					xi_refCharacter.StrangeElixir = true;
					LAi_SetHP (xi_refCharacter,1,LAi_GetCharacterMaxHP(xi_refCharacter));
					PlaySound("interface\heartbeat.wav");
					if (IsMainCharacter(xi_refCharacter)) Log_Info(GetConvertStrWithReplace("Variable_items_23", "Interface.txt", "#space#", " "));
					else {Log_Info(GetFullName(xi_refCharacter)+GetConvertStrWithReplace("Variable_items_24", "Interface.txt", "#space#", " "))}
					ApplayNewSkill(pchar, "", 0);
					TakeNItems(xi_refCharacter, sItemID, -1);
					FillItemsTable(iCurTab);
				}
			}
			else PlaySound("interface\knock.wav");
		}
	}
	if (HasSubStr(sItemID,"Tube") && IsMainCharacter(xi_refCharacter))
	{
		// эффект курения вынесен в ApplyTubeEffect (items_utilite.c) - общий с быстрым меню "Покурить" (LandInterface)
		if (ApplyTubeEffect(pchar, sItemID))
		{
			TakeNItems(pchar, sItemID, -1);
			FillItemsTable(iCurTab);
		}
	}
}
// GetHigh() перенесён в ITEMS\items_utilite.c (постоянный сегмент): эффект курения трубок (ApplyTubeEffect)
// вызывается и из быстрого меню "Покурить", когда сегмент interface\items.c не загружен.
void ExitMapWindow()
{
	XI_WindowShow("MAP_WINDOW", false);
	XI_WindowDisable("MAP_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetFormatedText("MAP_TEXT", "");
	sMapDescribe = "";//обнуляем при выходе из карты, иначе после просмотра карты сокровищ можно открыть карту острова и на ней будет работать показ описания запомненной карты
	SendMessage(&GameInterface,"lslllll",MSG_INTERFACE_MSG_TO_NODE,"MAP_X_CROSS", 0, makeint(fOffsetX)+721, 25, makeint(fOffsetX)+743, 47);// Установить отметку на исходную позицию, под кнопку закрытия

	SetCurrentNode("TABLE_ITEMS");
}

void SetMapCross(string _MapLocId)
{
	int left, top;//координаты для отметки клада
	bMapCross = true;//текст описания карты не показывать сразу

	switch(_MapLocId)//список локаций беру из MapDescribe.txt
	{
	case "Antigua_Grot": left = 622; top = 235; break;	//{в пещере справа от залива Уиллогби}
	case "SentJons_TownCave": left = 601; top = 235; break;	//{в подземелье города Сент-Джонс}

	case "Beliz_Cave": left = 58; top = 181; break;	//{в пещере справа от выхода из Белиза}
	case "Beliz_Cave_2": left = 55; top = 225; break;	//{в пещере напротив выхода из Белиза}

	case "Panama_Cave": left = 229; top = 502; break;	//{в пещере слева от выхода из города Порто Белло}
	case "PortoBello_Cave": left = 195; top = 512; break;	//{в пещере справа от залива Москитос}

	case "Tortuga_Grot": left = 374; top = 53; break;	//{в пещере напротив пляжа Глаз Рыбы}
	case "Tortuga_Cave": left = 380; top = 61; break;	//{в подземелье города Тортуга}

	case "Caracas_Grot": left = 444; top = 484; break;	//{в пещере слева от выхода из Каракаса}
	case "Maracaibo_Cave": left = 369; top = 474; break;	//{в пещере напротив выхода из Маракайбо}
	case "Cartahena_Cave": left = 304; top = 510; break;	//{в пещере напротив выхода из Картахены}

	case "Pearl_Grot": left = 66; top = 345; break;	//{в пещере на западном побережье между Белизом и Санта Каталиной}
	case "SantaCatalina_Grot": left = 123; top = 445; break;	//{в пещере справа от залива Сан-Хуан дель Норте}
	case "SantaCatalina_PearlCave": left = 109; top = 466; break;	//{в пещере слева от залива Сан-Хуан дель Норте}
	case "SantaCatalina_Cave": left = 106; top = 375; break;	//{в пещере справа от мыса Перлас}
	case "Tenotchitlan_Cave": left = 55; top = 308; break;	//{в пещере на север от храма Инков}

	case "Barbados_Cave": left = 625; top = 385; break;	//{в пещере недалеко от Бриджтауна}
	case "Bermudes_Dungeon": left = 625; top = 29; break;	//{в подземелье под верьфью Шарп-Тауна}
	case "Caiman_Grot": left = 202; top = 173; break;	//{в пещере справа от пустынного пляжа}
	case "Charles_Grot": left = 550; top = 202; break;	//{в пещере справа от пляжа Бат}
	case "Cuba_Grot": left = 198; top = 44; break;	//{в пещере справа от мыса Камагуэй}
	case "Cumana_Cave": left = 530; top = 505; break;	//{в пещере справа от бухты Карупано}
	case "Curacao_Cave": left = 465; top = 435; break;	//{в пещере справа от берега Приют контрабандиста}
	case "StLucia_Grot": left = 545; top = 420; break;	//{в пещере справа от мыса Пеликана}
	case "Dominica_Grot": left = 574; top = 326; break;	//{в пещере справа от пляжа Касл Брус}
	case "Guadeloupe_Cave": left = 485; top = 269; break;	//{в пещере слева от маяка Гваделупы}
	case "Hispaniola_Cave": left = 383; top = 119; break;	//{в пещере недалеко от города Порт-о-Принс}
	case "Jamaica_Grot": left = 263; top = 287; break;	//{в пещере справа от маяка Порт-Рояля}
	case "Marigo_Cave": left = 619; top = 140; break;	//{в пещере напротив выхода из Мариго}
	case "SanJuan_Cave": left = 485; top = 153; break;	//{в пещере недалеко от Сан-Хуана}
	case "Bahames_Grot": left = 263; top = 26; break;	//{в гроте справа от бухты Приватира}
	case "Terks_Grot": left = 474; top = 33; break;	//{в пещере справа от залива Северный}
 	case "Trinidad_Grot": left = 591; top = 458; break;	//{в пещере напротив маяка Тринидада}
 	case "FortFrance_Dungeon": left = 499; top = 357; break;	//{в подземелье города Форт де Франс}
	}
	//to do - тогда уж можно выбирать ещё и карту отдельного острова для отображения
	//оставить плохую общую для карт с описанием GetConvertStrWithReplace("Variable_items_26", "Interface.txt", "#space#", " "), а качественные карты островов выбирать при описании GetConvertStrWithReplace("Variable_items_27", "Interface.txt", "#space#", " ")???

	SendMessage(&GameInterface,"lslllll",MSG_INTERFACE_MSG_TO_NODE,"MAP_X_CROSS", 0, 43+makeint(fOffsetX)+left, 50+top, 43+makeint(fOffsetX)+left+14, 50+top+14);// Установить картинку на новую позицию
}

void ShowMapDesc()
{
	if (bMapCross)
		{
		SetFormatedText("MAP_TEXT", sMapDescribe);
		SetVAligmentFormatedText("MAP_TEXT");
		bMapCross = false;
		}
	else
		{
		SetFormatedText("MAP_TEXT", "");
		bMapCross = true;
		}
}

void ShowMapWindow()
{
	XI_WindowShow("MAP_WINDOW", true);
	XI_WindowDisable("MAP_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetCurrentNode("MAP_TEXT");
	SetCurrentNode("MAP_X_CROSS");
}

void ExitItemFromCharacterWindow()
{
	XI_WindowShow("ITEM_FROM_CHARACTER_WINDOW", false);
	XI_WindowDisable("ITEM_FROM_CHARACTER_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetNodeUsing("ITEM_FROM_CHARACTER_OK_BUTTON", false);
	SetNodeUsing("ITEM_FROM_CHARACTER_CANCEL_BUTTON", false);
	SetNodeUsing("QTY_EDIT", false);
	SetNodeUsing("QTY_ADD_BUTTON", false);
	SetNodeUsing("QTY_ADD_ALL_BUTTON", false);
	SetNodeUsing("QTY_REMOVE_BUTTON", false);
	SetNodeUsing("QTY_REMOVE_ALL_BUTTON", false);

	GameInterface.qty_edit.str = 0;

	SetCurrentNode("TABLE_ITEMS");
	CheckForAvailableItems();
}

void ShowItemFromCharacterWindow()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не открывать окно операций с товаром NPC
	if (CheckAttribute(xi_refCharacter,"nonremovable")) return;
	string sItemID = GameInterface.(CurTable).(CurRow).id;

	XI_WindowShow("ITEM_FROM_CHARACTER_WINDOW", true);
	XI_WindowDisable("ITEM_FROM_CHARACTER_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetNodeUsing("ITEM_FROM_CHARACTER_OK_BUTTON", true);
	SetNodeUsing("ITEM_FROM_CHARACTER_CANCEL_BUTTON", true);
	SetNodeUsing("QTY_EDIT", true);
	SetNodeUsing("QTY_ADD_BUTTON", true);
	SetNodeUsing("QTY_ADD_ALL_BUTTON", true);
	SetNodeUsing("QTY_REMOVE_BUTTON", true);
	SetNodeUsing("QTY_REMOVE_ALL_BUTTON", true);

	SetCurrentNode("ITEM_FROM_CHARACTER_TEXT");
	SetCurrentNode("ITEM_FROM_CHARACTER_NAME");

	ref itm = ItemsFromID(sItemID);

	string sTexture = itm.picTexture;
	string sPicture = "itm" + itm.picIndex;

	SetNewGroupPicture("ITEM_FROM_CHARACTER_PICTURE", sTexture, sPicture);
	SetNewGroupPicture("ITEM_FROM_CHARACTER_WEIGHT_PICTURE", "ICONS", "weight icon");

	int lngFileID = LanguageOpenFile("ItemsDescribe.txt");
	string sGood = itm.name;
	string sGoodName = LanguageConvertString(lngFileID, sGood);

	SetFormatedText("ITEM_FROM_CHARACTER_TEXT", GetItemDescribe(sItemID));
	if(sti(xi_refCharacter.index) != nMainCharacterIndex) SetFormatedText("ITEM_FROM_CHARACTER_NAME", GetConvertStrWithReplace("Variable_items_28", "Interface.txt", "#space#", " ") + sGoodName);
	else SetFormatedText("ITEM_FROM_CHARACTER_NAME", GetConvertStrWithReplace("Variable_items_29", "Interface.txt", "#space#", " ") + sGoodName);

	int iQuantity = sti(GameInterface.qty_edit.str);
	float fItemQuantity = iQuantity * GetItemWeight(sItemID);
	float fWeight = GetItemsWeight(xi_refCharacter) - fItemQuantity;

	if(iQuantity <= 0)
	{
		fItemQuantity = GetItemsWeight(xi_refCharacter);
	}

	aref arTest;
	makearef(arTest, itm);
	string sTemp = "";
	int attrQ = GetAttributesNum(arTest);
	for(i=0; i<attrQ; i++)
	{
		sTemp = GetAttributeValue(GetAttributeN(arTest,i));
		log_testinfo(GetAttributeName(GetAttributeN(arTest,i)) + " = " + sTemp);
	}

	SetFormatedText("ITEM_FROM_CHARACTER_WEIGHT_TEXT", XI_ConvertString("weight") + ": " + FloatToString(fWeight, 1) + "/"+GetMaxItemsWeight(xi_refCharacter));
	LanguageCloseFile(lngFileID);
}

void confirmChangeQTY_EDIT()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не менять количество товара NPC
	string sItemID = GameInterface.(CurTable).(CurRow).id;
	ChangeQTY_EDIT(sItemID);
	CheckAndSetOverloadMode(GetMainCharacter());
}

void ChangeQTY_EDIT(string sItem)
{
	if(sti(GameInterface.qty_edit.str) < 0 || isPassenger)
	{
		GameInterface.qty_edit.str = 0;
	}

	if(sti(GameInterface.qty_edit.str) > GetCharacterItem(xi_refCharacter, sItem))
	{
		GameInterface.qty_edit.str = GetCharacterItem(xi_refCharacter, sItem);
	}

	if(IsEquipCharacterByItem(xi_refCharacter, sItem))
	{
		if(sti(GameInterface.qty_edit.str) >= GetCharacterItem(xi_refCharacter, sItem))
		{
			GameInterface.qty_edit.str = sti(GameInterface.qty_edit.str) - 1;
		}
	}

	if(IsQuestUsedItem(sItem))
	{
		GameInterface.qty_edit.str = 0;
	}

	int iQuantity = sti(GameInterface.qty_edit.str);
	float fItemQuantity = iQuantity * GetItemWeight(sItem);
	float fWeight = GetItemsWeight(xi_refCharacter) - fItemQuantity;

	if(iQuantity <= 0)
	{
		fItemQuantity = GetItemsWeight(xi_refCharacter);
	}

	if (sti(xi_refCharacter.index) != nMainCharacterIndex) 
	{
		if (GetItemsWeight(pchar) + (iQuantity * GetItemWeight(sItem)) >= GetMaxItemsWeight(pchar))
		{
			SetFormatedText("ITEM_FROM_CHARACTER_TEXT", GetItemDescribe(sItem)+"\n\nПревышается переносимый вес главного героя.\nЛишний вес будет выброшен.");
		}
		else SetFormatedText("ITEM_FROM_CHARACTER_TEXT", GetItemDescribe(sItem));
	}														
	SetFormatedText("ITEM_FROM_CHARACTER_WEIGHT_TEXT", XI_ConvertString("weight") + ": " + FloatToString(fWeight, 1) + "/"+GetMaxItemsWeight(xi_refCharacter));

}

void REMOVE_ALL_BUTTON(string sItem)  // выкинуть все
{
	if(IsQuestUsedItem(sItem))
	{
		return;
	}

	int iQuantity = GetCharacterItem(xi_refCharacter, sItem);

	GameInterface.qty_edit.str = iQuantity;
	ChangeQTY_EDIT(sItem);
}

void ADD_ALL_BUTTON(string sItem)  // сбросить
{
	GameInterface.qty_edit.str = 0;
	ChangeQTY_EDIT(sItem);
}

void REMOVE_BUTTON(string sItem)  // выкинуть
{
	if(IsQuestUsedItem(sItem))
	{
		return;
	}

	if(GetCharacterItem(xi_refCharacter, sItem) >= 1)
	{
		GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) + 1);
	}

	if(sti(GameInterface.qty_edit.str) > GetCharacterItem(xi_refCharacter, sItem))
	{
		GameInterface.qty_edit.str = GetCharacterItem(xi_refCharacter, sItem);
	}
	ChangeQTY_EDIT(sItem);
}

void ADD_BUTTON(string sItem)  // не выкидывать
{
	GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) - 1);

	if(sti(GameInterface.qty_edit.str) < 0)
	{
		GameInterface.qty_edit.str = 0;
	}

	ChangeQTY_EDIT(sItem);
}

void RemoveItemsQuantity()
{
	string sItemID = GameInterface.(CurTable).(CurRow).id;
	ChangeQTY_EDIT(sItemID);

	int iQuantity = sti(GameInterface.qty_edit.str);
	if(iQuantity <= 0)
	{
		return;
	}

	float fItemQuantity = iQuantity * GetItemWeight(sItemID);

	TakeNItems(xi_refCharacter, sItemID, -iQuantity);
	if(sti(xi_refCharacter.index) != nMainCharacterIndex)
	{
		float fWeight = GetItemsWeight(pchar) + fItemQuantity;
		if (GetMaxItemsWeight(pchar)<=fWeight) 
		{
			if (GetItemsWeight(pchar) >= GetMaxItemsWeight(pchar)) {}
			else 
			{
				TakeNItems(pchar, sItemID, makeint(fItemQuantity - ((-1) * (GetMaxItemsWeight(pchar) - GetItemsWeight(pchar) - fItemQuantity))) / GetItemWeight(sItemID));
			}
		}
		else TakeNItems(PChar, sItemID, iQuantity);
	}											 

	ExitItemFromCharacterWindow();
	SetVariable();
}

// Gregg: generic-показ инфы об экипированном предмете по группе слота (было 13 копий Show*EquipInfo)
void ShowEquipInfoByGroup(string sGroupName)
{
	if(!CheckAttribute(xi_refCharacter, "equip." + sGroupName)) return;
	if(xi_refCharacter.equip.(sGroupName) == "") return;

	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;

	string sItem = GetCharacterEquipByGroup(xi_refCharacter, sGroupName);
	ref itm = ItemsFromID(sItem);

	sGroup = itm.picTexture;
	sGroupPicture = "itm" + itm.picIndex;

	sHeader = itm.name;
	sHeader = LanguageConvertString(idLngFile, sHeader);

	sText1 = GetItemDescribe(sItem);

	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 100, 100);
	LanguageCloseFile(idLngFile);
}

void ShowBladeEquipInfo() { ShowEquipInfoByGroup("blade"); }

void ShowGunEquipInfo() { ShowEquipInfoByGroup("gun"); }

void ShowSpyglassEquipInfo() { ShowEquipInfoByGroup("spyglass"); }

void ShowBackPackEquipInfo() { ShowEquipInfoByGroup("BackPack"); }

void ShowTalismanEquipInfo() { ShowEquipInfoByGroup("talisman"); }

void ShowCirassEquipInfo() { ShowEquipInfoByGroup("cirass"); }
// LEO: для ствола в руке берём текущий боеприпас, для запасного — сохранённый в ammostage.
string GetAmmoSlotAmmo(ref chr, bool bMushketSlot)
{
	string gun = GetFirearmInSlot(chr, bMushketSlot);
	if (gun == "" || !CheckCharacterItem(chr, gun)) return "";
	if (gun == GetCharacterEquipByGroup(chr, GUN_ITEM_TYPE))
	{
		if (CheckAttribute(chr, "chr_ai.bullet")) return chr.chr_ai.bullet;
		return "";
	}
	return LAi_FindAvailableGunAmmo(chr, gun);
}

void SetAmmoSlotPicture(string sNode, string sAmmo)
{
	if (sAmmo == "")
	{
		SetNewGroupPicture(sNode, "", "");
		return;
	}
	ref itms = ItemsFromID(sAmmo);
	SetNewGroupPicture(sNode, itms.picTexture, "itm" + itms.picIndex);
}

// LEO: выбор боеприпаса из таблицы. Куда он идёт, решает рамка, по которой кликнули (sAmmoTarget):
//   для запасного ствола меняем только его память, не разряжая активный.
void EquipAmmoFromTable(string sItemID, ref itmRef)
{
	string sGun = GetCharacterEquipByGroup(xi_refCharacter, GUN_ITEM_TYPE);
	string liveGun = sGun;
	if (sAmmoTarget == "pistol") sGun = GetFirearmInSlot(xi_refCharacter, false);
	if (sAmmoTarget == "mushket") sGun = GetFirearmInSlot(xi_refCharacter, true);
	bool bParked = sGun != liveGun;
	if (!IsGunUsesAmmo(sGun, sItemID))
	{
		PlaySound("interface\knock.wav");
		return;
	}
	if (bParked)
	{
		if (!AmmoMemoryAllowed(xi_refCharacter)) // у кого нет памяти по стволу - выбирать некуда
		{
			PlaySound("interface\knock.wav");
			return;
		}
		xi_refCharacter.ammostage.(sGun) = sItemID; // заряжается при возврате к пистолю (LAi_SetCharacterDefaultBulletType)
	}
	else
	{
		if (CheckAttribute(xi_refCharacter, "chr_ai.bullet"))
		{
			if (xi_refCharacter.chr_ai.bullet == sItemID) return; // тот же боеприпас - не разряжаем ствол зря
		}
		LAi_SetCharacterUseBullet(xi_refCharacter, sItemID);
		LAi_GunSetUnload(xi_refCharacter);
		xi_refCharacter.equip.ammo = sItemID;
	}
	log_info(GetConvertStrWithReplace("Variable_items_18", "Interface.txt", "#space#", " ")+GetConvertStr(itmRef.name, "ItemsDescribe.txt")+"");
	PlaySound("People Fight\reload1.wav");
	FillItemsSelected();
}

void ShowAmmoEquipInfo() { ShowAmmoTooltip(GetAmmoSlotAmmo(xi_refCharacter, false)); }

void ShowMushAmmoEquipInfo() { ShowAmmoTooltip(GetAmmoSlotAmmo(xi_refCharacter, true)); }

void ShowAmmoTooltip(string sItem)
{
	if(sItem == "") return;

	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;

	ref itm = ItemsFromID(sItem);

	sGroup = itm.picTexture;
	sGroupPicture = "itm" + itm.picIndex;

	sHeader = "";

	sHeader = itm.name;
	sHeader = LanguageConvertString(idLngFile, sHeader);

	sText1 = GetItemDescribe(sItem);

	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 100, 100);
	LanguageCloseFile(idLngFile);
}

void ShowSuitEquipInfo() { ShowEquipInfoByGroup("suit"); }

void ShowBookEquipInfo() { ShowEquipInfoByGroup("book"); }

void Show1EquipInfo() { ShowEquipInfoByGroup("jewelry_indian_left"); }

void Show2EquipInfo() { ShowEquipInfoByGroup("jewelry_indian_right"); }

void Show3EquipInfo() { ShowEquipInfoByGroup("indian_center"); }

void Show4EquipInfo() { ShowEquipInfoByGroup("idols_left"); }

void Show5EquipInfo() { ShowEquipInfoByGroup("idols_right"); }

void ClickSaber()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(5); ClickItem("ITEM_1", "blade", 2);} // LEO: read-only чеккер
void ClickGun()			{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(6); SelectItemRowInTable(GetFirearmInSlot(xi_refCharacter, false));} // LEO: read-only чеккер
void ClickSpyGlass()	{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(14) ClickItem("ITEM_3", "spyglass", 2);} // LEO: read-only чеккер
void ClickCirass()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(7); ClickItem("ITEM_4", "cirass", 2);} // LEO: read-only чеккер
void ClickSuit()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(19); ClickItem("ITEM_15", "suit", 2);} // LEO: read-only чеккер
void ClickMushket()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(15); SelectItemRowInTable(GetFirearmInSlot(xi_refCharacter, true));} // LEO: read-only чеккер
void ClickBackPack()	{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(16); ClickItem("ITEM_6", "BackPack", 2);} // LEO: read-only чеккер
void ClickTalisman()	{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(8); ClickItem("ITEM_7", "talisman", 3);} // LEO: read-only чеккер
void ClickBook()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(17); ClickItem("ITEM_9", "book", 3);} // LEO: read-only чеккер
void ClickShow1()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(9); ClickItem("ITEM_10", "jewelry_indian_left", 3);} // LEO: read-only чеккер
void ClickShow2()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(10); ClickItem("ITEM_11", "jewelry_indian_right", 3);} // LEO: read-only чеккер
void ClickShow3()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(11); ClickItem("ITEM_8", "indian_center", 3);} // LEO: read-only чеккер
void ClickShow4()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(12); ClickItem("ITEM_12", "idols_left", 3);} // LEO: read-only чеккер
void ClickShow5()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(13); ClickItem("ITEM_13", "idols_right", 3);} // LEO: read-only чеккер
// LEO: рамки боеприпаса пистоля и мушкета. Цель выставляем ПОСЛЕ FillItemsTable - она её сбрасывает.
//   Выделяем в таблице то, что стоит в самой рамке, а не equip.ammo: у мушкетёра это разные боеприпасы.
void ClickAmmo()		{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; FillItemsTable(18); sAmmoTarget = "pistol"; SelectItemRowInTable(GetAmmoSlotAmmo(xi_refCharacter, false));} // LEO: read-only чеккер
void ClickMushAmmo()	{if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; if (GetFirearmInSlot(xi_refCharacter, true) == "") return; FillItemsTable(18); sAmmoTarget = "mushket"; SelectItemRowInTable(GetAmmoSlotAmmo(xi_refCharacter, true));}

void ClickItem(string sItem, string sEquipType, int _tabNum)
{
	string sTemp, row;

	if(!CheckAttribute(xi_refCharacter, "equip." + sEquipType)) 
	{
		HideItemInfo();
		return;
	}
	if(xi_refCharacter.equip.(sEquipType) == "") 
	{
		HideItemInfo();
		return;
	}
	SelectItemRowInTable(GetCharacterEquipByGroup(xi_refCharacter, sEquipType));
}

// выделить в таблице строку предмета и прокрутить к ней ("" - снять выделение и спрятать описание)
void SelectItemRowInTable(string sCurItem)
{
	string row;
	if (sCurItem == "")
	{
		HideItemInfo();
		return;
	}
	for (int q=1;q<1000;q++)
	{
		row = "tr" + q;
		if (!checkattribute(&GameInterface,"TABLE_ITEMS." + row)) break;//кончились строки таблицы, конец цикла
		if (sCurItem == GameInterface.TABLE_ITEMS.(row).id)
		{
			Event("TableSelectChange", "sll", "TABLE_ITEMS", q, 1);//выбираем в таблице предмет снаряжения
			GameInterface.TABLE_ITEMS.select = q;
			if (q<5) q=5;
			GameInterface.TABLE_ITEMS.top = q-5;//стараемся красиво проскроллить таблицу на этот предмет
			Table_UpdateWindow("TABLE_ITEMS");
			SetCurrentNode("TABLE_ITEMS");
			break;
		}
	}
}

void CheckButtonChange()
{
	string sControl = GetEventData();
	int iSelectedCB = GetEventData();
	int iNewState = GetEventData();
	if (sControl == "CHECK_SUIT")
	{
		if (iSelectedCB != 1 || !CanChooseSuitPriority()) return;
		bool preferSuit = iNewState != 0;
		if (preferSuit == CharacterPrefersSuit(xi_refCharacter)) return;
		xi_refCharacter.Costume = preferSuit;
		string model = GetCharacterEquipmentModel(xi_refCharacter, preferSuit);
		if (model != xi_refCharacter.model)
		{
			xi_refCharacter.model = model;
			SetNewModelToChar(xi_refCharacter);
		}
		return;
	}
	// Refresh and rollback set the current radio button and can dispatch this event again.
	if (sControl != "CHECK_EQUIP_SET" || iNewState == 0 || iSelectedCB == iEQUIP_SET) return;
	if (!EquipSetChangeAllowed(xi_refCharacter, iEQUIP_SET, iSelectedCB))
	{
		CheckButton_SetState("CHECK_EQUIP_SET", iEQUIP_SET, true);
		return;
	}
	SaveEquipSet();
	iEQUIP_SET = iSelectedCB;
	ReEquipCharacter(true);
	FillItemsTable(iCurTab);
}

// LEO: в бою комплект снаряжения не меняем - ни галкой в инвентаре, ни хоткеями NUM.
//   Гейт тот же, что у путевых хоткеев, торговли и сундуков (hotkey_travel.c).
bool EquipSetChangeAllowed(ref chref, int iCurrentSet, int iTargetSet)
{
	if (iCurrentSet == iTargetSet) return true;
	if (!LAi_group_IsActivePlayerAlarm())
	{
		if (!IsIntegratedMusketDrawn(chref) || GetEquipSetGunMode(chref, iTargetSet) == "") return true;
	}
	PlaySound("knock"); // глухой стук «нельзя» - так же отвечают экран отношений и еда в бою
	return false;
}

void ReEquipCharacter(bool restoreClothes)
{
	xi_refCharacter.selectedSet = iEQUIP_SET;//запоминаем выбор сэта
	string sSET = "Set" + iEQUIP_SET;
	if (!checkattribute(xi_refCharacter, sSET)) return; //не было сохранения этого комплекта

	string sTemp, sSaved;
	for (int q=0;q<9;q++)
	{
	sTemp = sEType[q];
	// LEO: При открытии инвентаря показываем надетые вещи. Одежду из комплекта
	// применяем только при явном выборе комплекта, иначе старая запись снимает костюм.
	if (!restoreClothes)
	{
		if (sTemp == CIRASS_ITEM_TYPE || sTemp == SUIT_ITEM_TYPE) continue;
	}
	sSaved = "";
	if (CheckAttribute(xi_refCharacter, sSET + "." + sTemp)) sSaved = xi_refCharacter.(sSET).(sTemp); // LEO: в старом комплекте группы может не быть
		if(sSaved != GetCharacterEquipByGroup(xi_refCharacter, sTemp))//если другой предмет, то снимаем старый, одеваем сохранённый
		{
			// LEO: сохранённой вещи в инвентаре уже нет (выложили в сундук, продали) - текущую НЕ снимаем.
			//   Раньше снимали, надеть было нечего, и офицер оставался с пустым слотом (репорт 23.09.2026).
			if (sSaved != "")
			{
				if (GetCharacterItem(xi_refCharacter, sSaved) < 1) continue;
			}
			RemoveCharacterEquip(xi_refCharacter, sTemp);
			if (sSaved != "") {EquipCharacterByItem(xi_refCharacter, sSaved);}
		}
	}
	RestoreEquipGun(xi_refCharacter, iEQUIP_SET);//Gregg: огнестрел — отдельной веткой (мушкет/пистоль/патрон)
	ResetSummonSkillCache(xi_refCharacter); // LEO: комплект сменили - бонусы вещей пересчитать (см. RPGUtilite.c)
	FillItemsSelected();//обновляем картинки на кукле
}

void SaveEquipSet()
{
	string sSET = "Set" + iEQUIP_SET;//Set1, Set2, Set3
	xi_refCharacter.(sSET) = 1; //помечаем, что в этом комплекте что-то сохранено
	if (!checkattribute(xi_refCharacter, sSET + ".nameset")) xi_refCharacter.(sSET).nameset = GetConvertStrWithReplace("Variable_items_30", "Interface.txt", "#space#", " ") + iEQUIP_SET;//в первый раз записываем "Комплект N"
	string sTemp;
	for (int q=0;q<9;q++)
	{
	sTemp = sEType[q];
	xi_refCharacter.(sSET).(sTemp) = GetCharacterEquipByGroup(xi_refCharacter, sTemp);
	}
	//книгу не запоминаем, потому что не надо
	SaveEquipGun(xi_refCharacter, iEQUIP_SET);//Gregg: огнестрел храним отдельной веткой Set<N>.gun.*
}

/* ==================== Gregg: хранение огнестрела в комплектах ====================
   Огнестрел не входит в sEType (9 обычных групп) из-за трёх особенностей:
   1) в комплекте пистоль и мушкет хранятся отдельно; боевая стойка не меняется;
   2) выбранный тип патрона живёт на chr_ai.bullet; ствол его теперь помнит сам (ammostage.<ствол>,
      LEO 24.09.2026), а в комплекте хранится только патрон активного ствола;
   3) спец-мушкетёры и DontChangeGun — фиксированное оружие, их не трогаем.
   Заряд (chargestage.<gun>) восстанавливается самой экипировкой, в комплекте не храним.     */

// Можно ли управлять огнестрелом этого персонажа через комплекты.
bool EquipSetGunAllowed(ref chref)
{
	if (CheckAttribute(chref, "DontChangeGun")) return false; // ствол заблокирован квестом
	if (IsFixedMushketer(chref)) return false;
	return true;
}

string GetEquipSetGunMode(ref chref, int iSet)
{
	if (!EquipSetGunAllowed(chref)) return "";
	string sSET = "Set" + iSet;
	if (!CheckAttribute(chref, sSET + ".gun.mode")) return "";
	string sMode = ReadLegacyMusketKitMode(chref.(sSET).gun.mode);
	if (sMode == "pistol" || (sMode == "rifle" && IsMainCharacter(chref))) return sMode;
	return "";
}

// Снимок огнестрела активного комплекта: режим + пистолет + мушкет + выбранный патрон.
void SaveEquipGun(ref chref, int iSet)
{
	if (!EquipSetGunAllowed(chref)) return;
	// A drawn musket is temporary; do not replace an existing restorable snapshot on close.
	if (IsIntegratedMusketDrawn(chref) && GetEquipSetGunMode(chref, iSet) != "") return;
	string sSET = "Set" + iSet;
	string sGunNow = GetCharacterEquipByGroup(chref, GUN_ITEM_TYPE);
	chref.(sSET).gun.mode = "pistol";
	if (isMushket(sGunNow)) chref.(sSET).gun.mode = "rifle";
	chref.(sSET).gun.mushket = GetFirearmInSlot(chref, true);
	chref.(sSET).gun.pistol = GetFirearmInSlot(chref, false);

	// Патрон запоминаем только если реально есть ствол — иначе остаточный chr_ai.bullet даст мусор в снимке.
	if (sGunNow != "" && CheckAttribute(chref, "chr_ai.bullet"))	chref.(sSET).gun.ammo = chref.chr_ai.bullet;
	else														chref.(sSET).gun.ammo = "";
}

// Восстановление огнестрела комплекта с учётом мушкетёрской развилки и выбранного патрона.
void RestoreEquipGun(ref chref, int iSet)
{
	string sMode = GetEquipSetGunMode(chref, iSet);
	if (sMode == "" || IsIntegratedMusketDrawn(chref)) return;
	string sSET = "Set" + iSet;

	string sPistol = "";
	if (CheckAttribute(chref, sSET + ".gun.pistol")) sPistol = chref.(sSET).gun.pistol;
	string sMushket = "";
	if (CheckAttribute(chref, sSET + ".gun.mushket")) sMushket = chref.(sSET).gun.mushket;
	string sAmmo = "";
	if (CheckAttribute(chref, sSET + ".gun.ammo")) sAmmo = chref.(sSET).gun.ammo;

	bool bIsMain = IsMainCharacter(chref);

	string sCur = GetCharacterEquipByGroup(chref, GUN_ITEM_TYPE);
	// LEO: если сохранённый пистоль ушёл из инвентаря, не снимаем текущий.
	bool pistolAvailable = sPistol == "" || CheckCharacterItem(chref, sPistol);
	if (sCur != sPistol && pistolAvailable)
	{
		if (sCur != "") RemoveCharacterEquip(chref, GUN_ITEM_TYPE);
		if (sPistol != "") EquipCharacterByItem(chref, sPistol);
		if (!bIsMain && GetCharacterEquipByGroup(chref, GUN_ITEM_TYPE) != "") LAi_GunSetUnload(chref);
	}
	if (bIsMain)
	{
		string selected = GetSelectedMusket(chref);
		if (sMushket == "" && selected != "") UnequipIntegratedMusket(chref);
		else if (sMushket != selected && isMushket(sMushket) && CheckCharacterItem(chref, sMushket))
			SetCharacterToMushketer(chref, sMushket, true);
	}
	if (sMode == "rifle")
	{
		if (sAmmo != "" && CheckCharacterItem(chref, sAmmo) && IsGunUsesAmmo(sMushket, sAmmo))
			chref.ammostage.(sMushket) = sAmmo;
		return;
	}

	// (4) Восстановить выбранный патрон, если он в инвентаре и совместим с текущим стволом;
	//     иначе — дефолтный патрон ствола (LAi_SetCharacterUseBullet вернёт false при несовместимости).
	// LEO: любая переустановка патрона доходит до LAi_GunSetChargeQuant, а тот набивает СТВОЛ ПОЛНОСТЬЮ.
	//   Отсюда абуз: выстрелил - сменил комплект (или просто открыл инвентарь, там тот же
	//   ReEquipCharacter) - и оружие снова заряжено. Помним заряд до пересчёта и возвращаем после:
	//   шаг (4) обязан восстанавливать ТИП патрона, а не заряжать.
	string sOldAmmo = "";	if (CheckAttribute(chref, "chr_ai.bullet")) sOldAmmo = chref.chr_ai.bullet;
	float fKeepCharge = 0.0;
	string sKeepPrc = "";
	bool bHadCharge = CheckAttribute(chref, "chr_ai.charge");
	if (bHadCharge) fKeepCharge = stf(chref.chr_ai.charge);
	if (CheckAttribute(chref, "chr_ai.chargeprc")) sKeepPrc = chref.chr_ai.chargeprc;
	bool bAmmoOk = false;
	if (sAmmo != "" && CheckCharacterItem(chref, sAmmo))
	{
		bAmmoOk = LAi_SetCharacterUseBullet(chref, sAmmo);
	}
	if (bAmmoOk)
	{
		chref.curammo = sAmmo;
	}
	else
	{
		LAi_SetCharacterDefaultBulletType(chref);
	}
	if (bHadCharge)
	{
		chref.chr_ai.charge = fKeepCharge;
		if (sKeepPrc != "") chref.chr_ai.chargeprc = sKeepPrc;
	}
	// Сменили тип заряженного патрона на неизменном стволе — патронник пуст (как ручной путь items.c:1959-1960).
	if (bAmmoOk)
	{
		if (sOldAmmo != "")
		{
			if (sOldAmmo != sAmmo) LAi_GunSetUnload(chref);
		}
	}
}

void FillSetName(int iNum)
{
	string sSET = "Set" + iNum;
	string sName;
	if (!checkattribute(xi_refCharacter, sSET))	sName = GetConvertStrWithReplace("Variable_items_31", "Interface.txt", "#space#", " ") + iNum;	else sName = xi_refCharacter.(sSET).nameset;
	GameInterface.EDIT_BOX1.str = sName;
	SendMessage(&GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"CHECK_EQUIP_SET", 1, iNum, sName);
}

void SaveSETName()
{
	if(GameInterface.EDIT_BOX1.lastkey == " ") return;
	string sSET = "Set" + iEQUIP_SET;
	xi_refCharacter.(sSET).nameset = GameInterface.EDIT_BOX1.str;
	FillSetName(iEQUIP_SET);
	HideEditBox();
}

void ShowEditBox()
{
	string sSET = "Set" + iEQUIP_SET;
	if (!checkattribute(xi_refCharacter, sSET))	GameInterface.EDIT_BOX1.str = GetConvertStrWithReplace("Variable_items_32", "Interface.txt", "#space#", " ") + iEQUIP_SET;	else GameInterface.EDIT_BOX1.str = xi_refCharacter.(sSET).nameset;
	SetNodeUsing("EDIT_BOX1" , true);
	SetNodeUsing("EDIT_BOX1_FRAME" , true);
	SetCurrentNode("EDIT_BOX1");
}

void HideEditBox()
{
	SetNodeUsing("EDIT_BOX1" , false);
	SetNodeUsing("EDIT_BOX1_FRAME" , false);
}

void DiscardTreasureMap()
{
	if (CheckAttribute(pchar, "SystemInfo.CheckNpcIdx")) return; // LEO: read-only чеккер — не выбрасывать карту NPC
	TakeNItems(pchar, "map_full", -1);
	ref item;
	FillMapForTreasure(Items_FindItem("map_full", &item));
	ExitMapWindow();
	FillItemsTable(iCurTab);
}
/* Смена комплектов экипировки по хоткеям */
void setEquipmentComplect(int num)
{
	if(num >= 1 && num <=6 )
	{
		// The inventory index can still belong to an officer (or a freshly loaded segment).
		int iCurrentSet = 0;
		if (CheckAttribute(pchar, "selectedSet")) iCurrentSet = sti(pchar.selectedSet);
		if (!EquipSetChangeAllowed(pchar, iCurrentSet, num)) return;
		iEQUIP_SET = num;
		string sName;
		string sSET = "Set" + iEQUIP_SET;
		xi_refCharacter = pchar;
		if (!checkattribute(xi_refCharacter, sSET + ".nameset")) sName = GetConvertStrWithReplace("Variable_items_31", "Interface.txt", "#space#", " ") + iEQUIP_SET;	else sName = xi_refCharacter.(sSET).nameset;
		ReEquipCharacter(true);
		Log_Info(XI_ConvertString("Selected") + sName);
	}
}
