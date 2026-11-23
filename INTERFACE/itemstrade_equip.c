//дополнение к обмену предметами
// Lugger: Equip Info -->
void ExitEquipWindow()
{
	XI_WindowShow("EQUIP_WINDOW", false);
	XI_WindowDisable("EQUIP_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);
	SetCurrentNode(sLastTable);//глобалка в itemstrade.c
}

void ShowEquipWindow()
{
	string sAttr = "pic" + (nCurScrollNum + 1);
	if (HasSubStr(GameInterface.CHARACTERS_SCROLL.(sAttr).character,"box")) return;//у сундуков нет экипировки

	XI_WindowShow("EQUIP_WINDOW", true);
	XI_WindowDisable("EQUIP_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	SetCurrentNode("EQUIP_FRAME");

	string sBlade = GetCharacterEquipByGroup(refCharacter, "blade");
	string sGun = GetCharacterEquipByGroup(refCharacter, "gun");
	string sSpyglass = GetCharacterEquipByGroup(refCharacter, "spyglass");
	string sCirass = GetCharacterEquipByGroup(refCharacter, "cirass");
	string sBackPack = GetCharacterEquipByGroup(refCharacter, "BackPack");
	string sTalisman = GetCharacterEquipByGroup(refCharacter, "talisman");
	string sBook = GetCharacterEquipByGroup(refCharacter, "book");
	string sShow1 = GetCharacterEquipByGroup(refCharacter, "jewelry_indian_left");
	string sShow2 = GetCharacterEquipByGroup(refCharacter, "jewelry_indian_right");
	string sShow3 = GetCharacterEquipByGroup(refCharacter, "indian_center");
	string sShow4 = GetCharacterEquipByGroup(refCharacter, "idols_left");
	string sShow5 = GetCharacterEquipByGroup(refCharacter, "idols_right");

	if(CheckAttribute(refCharacter, "equip.blade") && refCharacter.equip.blade != "")
	{
		SetNewGroupPicture("EQUIP_BLADE_PIC", Items[FindItem(sBlade)].picTexture, "itm" + Items[FindItem(sBlade)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_BLADE_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.gun") && refCharacter.equip.gun != "")
	{
		SetNewGroupPicture("EQUIP_GUN_PIC", Items[FindItem(sGun)].picTexture, "itm" + Items[FindItem(sGun)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_GUN_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.spyglass") && refCharacter.equip.spyglass != "")
	{
		SetNewGroupPicture("EQUIP_SPYGLASS_PIC", Items[FindItem(sSpyglass)].picTexture, "itm" + Items[FindItem(sSpyglass)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_SPYGLASS_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.cirass") && refCharacter.equip.cirass != "")
	{
		SetNewGroupPicture("EQUIP_CIRASS_PIC", Items[FindItem(sCirass)].picTexture, "itm" + Items[FindItem(sCirass)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_CIRASS_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.BackPack") && refCharacter.equip.BackPack != "")
	{
		SetNewGroupPicture("EQUIP_BACKPACK_PIC", Items[FindItem(sBackPack)].picTexture, "itm" + Items[FindItem(sBackPack)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_BACKPACK_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.talisman") && refCharacter.equip.talisman != "")
	{
		SetNewGroupPicture("EQUIP_TALISMAN_PIC", Items[FindItem(sTalisman)].picTexture, "itm" + Items[FindItem(sTalisman)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_TALISMAN_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.book") && refCharacter.equip.book != "")
	{
		SetNewGroupPicture("EQUIP_BOOK_PIC", Items[FindItem(sBook)].picTexture, "itm" + Items[FindItem(sBook)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_BOOK_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.jewelry_indian_left") && refCharacter.equip.jewelry_indian_left != "")
	{
		SetNewGroupPicture("EQUIP_SHOW1_PIC", Items[FindItem(sShow1)].picTexture, "itm" + Items[FindItem(sShow1)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_SHOW1_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.jewelry_indian_right") && refCharacter.equip.jewelry_indian_right != "")
	{
		SetNewGroupPicture("EQUIP_SHOW2_PIC", Items[FindItem(sShow2)].picTexture, "itm" + Items[FindItem(sShow2)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_SHOW2_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.indian_center") && refCharacter.equip.indian_center != "")
	{
		SetNewGroupPicture("EQUIP_SHOW3_PIC", Items[FindItem(sShow3)].picTexture, "itm" + Items[FindItem(sShow3)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_SHOW3_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.idols_left") && refCharacter.equip.idols_left != "")
	{
		SetNewGroupPicture("EQUIP_SHOW4_PIC", Items[FindItem(sShow4)].picTexture, "itm" + Items[FindItem(sShow4)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_SHOW4_PIC", "none", "none");
	}

	if(CheckAttribute(refCharacter, "equip.idols_right") && refCharacter.equip.idols_right != "")
	{
		SetNewGroupPicture("EQUIP_SHOW5_PIC", Items[FindItem(sShow5)].picTexture, "itm" + Items[FindItem(sShow5)].picIndex);
	}
	else
	{
		SetNewGroupPicture("EQUIP_SHOW5_PIC", "none", "none");
	}

	SetFormatedText("EQUIP_CAPTION", XI_ConvertString("Equipment") + ": " + refCharacter.name + " " + refCharacter.lastname);
}

void ShowBladeEquipInfo() {ShowEquipInfo("blade");}
void ShowGunEquipInfo() {ShowEquipInfo("gun");}
void ShowSpyglassEquipInfo() {ShowEquipInfo("spyglass");}
void ShowBackPackEquipInfo() {ShowEquipInfo("BackPack");}
void ShowTalismanEquipInfo() {ShowEquipInfo("talisman");}
void ShowCirassEquipInfo()	{ShowEquipInfo("cirass");}
void ShowBookEquipInfo() {ShowEquipInfo("book");}
void Show1EquipInfo()	{ShowEquipInfo("jewelry_indian_left");}
void Show2EquipInfo()	{ShowEquipInfo("jewelry_indian_right");}
void Show3EquipInfo()	{ShowEquipInfo("indian_center");}
void Show4EquipInfo()	{ShowEquipInfo("idols_left");}
void Show5EquipInfo()	{ShowEquipInfo("idols_right");}

void ShowEquipInfo(string sItemType)
{
	if (!CheckAttribute(refCharacter, "equip." + sItemType)) {return;}
	if (refCharacter.equip.(sItemType) == "") {return;}

	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;

	string sItem = GetCharacterEquipByGroup(refCharacter, sItemType);
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

void ClickSaber()	{ClickItem("BLADE");}
void ClickGun()		{ClickItem("GUN");}
void ClickSpyGlass() {ClickItem("SPYGLASS");}
void ClickCirass()	{ClickItem("CIRASS");}
void ClickBackPack() {ClickItem("BACKPACK");}
void ClickTalisman() {ClickItem("TALISMAN");}
void ClickBook()	{ClickItem("BOOK");}
void ClickShow1()	{ClickItem("SHOW1");}
void ClickShow2()	{ClickItem("SHOW2");}
void ClickShow3()	{ClickItem("SHOW3");}
void ClickShow4()	{ClickItem("SHOW4");}
void ClickShow5()	{ClickItem("SHOW5");}

void ClickItem(string sItem)
{
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BLADE_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_GUN_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_CIRASS_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_SPYGLASS_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BACKPACK_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_TALISMAN_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_BOOK_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_SHOW1_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_SHOW2_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_SHOW3_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_SHOW4_PIC", 5, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EQUIP_SHOW5_PIC", 5, 0);

	string sNode = "EQUIP_" + sItem + "_PIC";
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,sNode, 5, 1);
}
// Lugger: Equip Info <--