
native string LanguageGetLanguage();
native int LanguageOpenFile(string sFileName);
native void LanguageCloseFile(int idLngFile);
native string LanguageConvertString(int userLngFileID, string inStr);
native void LanguageSetLanguage(string sLanguageName);
native string XI_ConvertString(string inStr); // длинные переменные замедляют метод
native int GlobalLngFileID();
native string LanguageGetFaderPic(string faderPicName);
native string DialogAssembleStr(string idStr, string paramStr);
native string DialogAddParamToStr(string oldParamStr, string paramID, string paramValue);


native void XI_SetColorCorrection(float fContrast, float fGamma, float fBrightness);
native void XI_SetMouseSensitivity( float fMouseXSens, float fMouseYSens );
native void XI_ControlMakeInvert(string sControlName, int bInvertState);
native void XI_MakeNode(string sIniFileName, string sNodeType, string sNodeName, int nPriority);
native void XI_DeleteNode(string sNodeName);
native bool XI_IsWindowEnable(string sWindowName);
native void XI_WindowShow(string sWindowName, int bShowStatus);
native void XI_WindowDisable(string sWindowName, int bDisableStatus);
native void XI_WindowAddNode(string sWindowName, string sNodeName);
native bool XI_CreateFolder(string sFolderName);
native bool XI_CheckFolder(string sFolderName);
native bool XI_DeleteFolder(string sFolderName);
native bool XI_FindFolders(string sFindTemplate,aref arFoldersList);
native int XI_StoreNodeLocksWithOff();
native void XI_RestoreNodeLocks(int nStoreCode);
native bool XI_IsKeyPressed(string key_name); // key_name = {"shift","control","alt"}
native void XI_RegistryExitKey(string key_name);
native void XI_IsVirtualKeyPressed(string key_name);

#libriary "script_interface_functions"

void CreateImage(string AttrName,string imgListName,string imgName,int left,int top,int right,int bottom)
{
	GameInterface.pictures.(AttrName).tex = imgListName;
    GameInterface.pictures.(AttrName).pic = imgName;
    SendMessage(&GameInterface,"lslllll",MSG_INTERFACE_PLACE_IMAGE,AttrName,true,left,top,right,bottom);
}

void CreateAbsoluteImage(string AttrName,string imgListName,string imgName,int left,int top,int right,int bottom)
{
	GameInterface.pictures.(AttrName).tex = imgListName;
    GameInterface.pictures.(AttrName).pic = imgName;
    SendMessage(&GameInterface,"lslllll",MSG_INTERFACE_PLACE_IMAGE,AttrName,false,left,top,right,bottom);
}

void SetPictureBlind(string picName,bool blindFlag,int minCol,int maxCol)
{
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_SET_BLIND,picName,blindFlag,minCol,maxCol);
}

void CreateString(int enable,string strName, string strData, string fontName, int color, int x, int y, int alignment, float scale)
{
    GameInterface.strings.(strName) = strData;
    SendMessage(&GameInterface,"lssllllf",MSG_INTERFACE_CREATE_STRING, strName, fontName, color, x,y,alignment,scale);
    if(enable==FALSE)
    {
        SendMessage(&GameInterface,"ls", MSG_INTERFACE_DISABLE_STRING, strName);
    }
}

void DisableString(string strName)
{
    SendMessage(&GameInterface,"ls", MSG_INTERFACE_DISABLE_STRING, strName);
}

void EnableString(string strName)
{
    SendMessage(&GameInterface,"ls", MSG_INTERFACE_ENABLE_STRING, strName);
}

void ChangeStringColor(string strName, int newCol)
{
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_CHANGE_STR_COLOR,strName,newCol);
}

void SetNodeUsing(string nodeName,int bUsing)
{
    SendMessage(&GameInterface,"lsl", MSG_INTERFACE_SET_NODE_USING, nodeName, bUsing);
}

void SetCurrentNode(string nodeName)
{
    SendMessage(&GameInterface,"ls", MSG_INTERFACE_NEW_CURRENT_NODE,nodeName);
}
string GetCurrentNode()
{
	string retVal;
	SendMessage(&GameInterface,"le", MSG_INTERFACE_GET_CURRENT_NODE,&retVal);
	return retVal;
}

void ShowHelpString(string helpString)
{
    SendMessage(&GameInterface,"ls", MSG_INTERFACE_SETHELPSTRING, helpString);
}

string MakeMoneyShow(int moneyQuantity,string sign,string deliver)
{
	string moneyStr;
	int i,j;
	moneyStr="";
	if (moneyQuantity < 0) {sign = sign + "-"; moneyQuantity = -moneyQuantity;}//фикс - показ отрицательных сумм
	while(moneyQuantity>999)
	{
		i = moneyQuantity/1000;
		j = moneyQuantity-i*1000;
		if(j>99)
		{
			moneyStr = deliver + j + moneyStr;
		}
		else
		{
			if(j>9)
			{
				moneyStr = deliver+"0" + j + moneyStr;
			}
			else
			{
				moneyStr = deliver+"00" + j + moneyStr;
			}
		}
		moneyQuantity = i;
	}
	moneyStr = sign+moneyQuantity+moneyStr;
	return moneyStr;
}

void SetNewPicture(string _NodeName,string _NewPictureFileName)
{
	SendMessage(&GameInterface,"lslls", MSG_INTERFACE_MSG_TO_NODE,_NodeName,2, false, _NewPictureFileName);
}

void SetNewGroupPicture(string sNodeName,string sGroup, string sName)
{
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE, sNodeName ,6, sGroup, sName);
}

void SetNewVideoPicture(string _NodeName,string _NewVideoPicFileName)
{
	SendMessage(&GameInterface,"lslls", MSG_INTERFACE_MSG_TO_NODE,_NodeName,2, true, _NewVideoPicFileName);
}

void SetSelectable(string _NodeName,int _Selectable)
{
	SendMessage(&GameInterface,"lsl", MSG_INTERFACE_SET_SELECTABLE, _NodeName, _Selectable);
}
int GetSelectable(string _NodeName)
{
	return SendMessage(&GameInterface,"ls", MSG_INTERFACE_GET_SELECTABLE, _NodeName);
}

void SetClickable(string _NodeName, bool ClickStatus)
{
	SendMessage(&GameInterface,"lslll", MSG_INTERFACE_MSG_TO_NODE,_NodeName,-1,1, ClickStatus);
}
bool GetClickable(string _NodeName)
{
	return SendMessage(&GameInterface,"lsll", MSG_INTERFACE_MSG_TO_NODE,_NodeName,-1,2);
}

void DelFromScroll(string nodeName,int pnum)
{
	int qn = sti(GameInterface.(nodeName).ListSize)-1;
	aref inar,outar;
	string attrName;
	for(int i=pnum; i<qn; i++)
	{
		attrName = "pic"+(i+1);
		makearef(inar,GameInterface.(nodeName).(attrName));
		attrName = "pic"+(i+2);
		makearef(outar,GameInterface.(nodeName).(attrName));
		CopyAttributes(inar,outar);
	}
	attrName = "pic"+(i+1);
	DeleteAttribute(&GameInterface,nodeName+"."+attrName);
	if(qn>=0) GameInterface.(nodeName).ListSize = qn;
	SendMessage(&GameInterface,"lsl", MSG_INTERFACE_DEL_SCROLLIMAGE, nodeName, pnum);
}

void AddToScroll(string nodeName,int pnum)
{
	SendMessage(&GameInterface,"lsl", MSG_INTERFACE_ADD_SCROLLIMAGE, nodeName, pnum);
}

void XI_SetQuestTitles(string nodName,aref questInfo,int topNum)
{
	SendMessage(&GameInterface,"lsal",MSG_INTERFACE_SET_TITLE_STRINGS,nodName,questInfo,topNum);
}

int GetShipTexture(string shipType)
{
	if(shipType=="Not Used") return -1;
	if(shipType=="Galeoth1") return 1;
	if(shipType=="Galeoth2") return 1;
	if(shipType=="Fearless") return 1;
	if(shipType=="GhostShip") return 1;
	return 0;
}

string GetShipTextureName(string shipType)
{
	if(shipType=="Not Used") return "";
	if(shipType=="Galeoth1") return "SHIPS1";
	if(shipType=="Galeoth2") return "SHIPS1";
	if(shipType=="Fearless") return "SHIPS1";
	if(shipType=="GhostShip") return "SHIPS1";
	return "SHIPS16";
}

string GetFaceGroupName(int charIdx)
{
	if(charIdx<0) {return "EMPTYFACE";}
	return "FACE128_"+Characters[charIdx].FaceID;
}

string GetMinifiedFaceGroupName(int charIdx)
{
    if(charIdx<0) {return "EMPTYFACE";}
    return "%_"+Characters[charIdx].FaceID;
}

string PrepareMinifiedFaceString(string facestr)
{
    string preparedFacestr = facestr;

    while(HasSubStr(preparedFacestr, "%")) {
        preparedFacestr = strreplace(preparedFacestr, "%", "FACE128");
    }

    return preparedFacestr;
}

string GetFacePictureName(int charIdx)
{
	if(charIdx<0) return "emptyface";
	return "face";
}
//-------------------------------------------------------------------------------------------------------------
// FormatedText
//-------------------------------------------------------------------------------------------------------------
void SetFormatedText(string sNodeName, string sText)
{
	SendMessage(&GameInterface,"lss",MSG_INTERFACE_SET_FORMATEDTEXT,sNodeName,sText);
}
//-------------------------------------------------------------------------------------------------------------
void AddLineToFormatedText(string sNodeName, string sText)
{
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE, sNodeName, 0, &sText);
}
//-------------------------------------------------------------------------------------------------------------
void SelectLineInFormatedText(string sNodeName, int line)
{
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,sNodeName, 1, line);
}
//-------------------------------------------------------------------------------------------------------------
int SelectedLineInFormatedText(string sNodeName)
{
	return SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,sNodeName, 11);
}

int SetVAligmentFormatedText(string sNodeName)  // boal нарыл в ядре, что можно
{
	return SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,sNodeName, 5);
}

int SetAlignmentFormatedText(string nodeName, int alignment)
{
    return SendMessage(&GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, nodeName, 14, alignment);
}
//-------------------------------------------------------------------------------------------------------------
// Warship -->
void SetFormattedTextLastLineColor(string _sNodeName, int _iColor)
{
	SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, _sNodeName, 8, -1, _iColor); // -1 = последняя лини, как я понял
}
// <--
//-------------------------------------------------------------------------------------------------------------

string FloatToString(float fl,int nDigAfterPoint)
{

	fl= fl + 0.5/pow(10.0,nDigAfterPoint); // округление
	int p1 = makeint(fl); // целая часть
	string str = p1 + "."; // строка
	for(int n=0; n<nDigAfterPoint; n++)
	{
		fl = (fl - p1)*10.0;
		p1 = makeint(fl);
		str = str + p1;
	}
	return str;
}
int GetStringWidth(string str, string fontID, float fScale)
{
	return SendMessage(&GameInterface, "lssf", MSG_INTERFACE_GET_STRWIDTH, str, fontID, fScale);
}

string GetItemPictureName(string itemName)
{
	aref arItm;
	if( Items_FindItem(itemName,&arItm)>=0 )
	{
		if( CheckAttribute(arItm,"picIndex") )
		{
			return	"itm"+sti(arItm.picIndex);
		}
	}
	return "cannon back";
}

string GetItemTextureName(string itemName)
{
	aref arItm;
	if( Items_FindItem(itemName,&arItm)>=0 )
	{
		if( CheckAttribute(arItm,"picTexture") )
		{
			return	arItm.picTexture;
		}
	}
	return "ICONS";
}

int GetItemPictureTexture(string accessString, string itemName)
{
	aref arItm,arGroup;
	makearef(arGroup,GameInterface.(accessString));
	string sTexName = "ICONS";

	if( Items_FindItem(itemName,&arItm)>=0 )
	{	if( CheckAttribute(arItm,"picTexture") )
		{	sTexName = arItm.picTexture;
		}
	}
	int q = GetAttributesNum(arGroup);
	for(int i=0; i<q; i++)
	{	if( GetAttributeValue(GetAttributeN(arGroup,i)) == sTexName )
		{	return i;
		}
	}
	return -1;
}

void FillImagesGroupForItems(aref arImgGrp)
{
	int i,n,q;
	string stmp;
	object objtmp;
	for(i=0; i<ITEMS_QUANTITY; i++)
	{
		if( CheckAttribute( &Items[i], "picTexture" ) && Items[i].picTexture!="" )
		{
			stmp = Items[i].picTexture;
			objtmp.(stmp) = true;
		}
	}
	q = GetAttributesNum(&objtmp);
	n = GetAttributesNum(arImgGrp);
	for(i=0; i<q; i++)
	{
		stmp = "t"+n;
		arImgGrp.(stmp) = GetAttributeName(GetAttributeN(&objtmp,i));
		n++;
	}
}

string GetFacePicName(ref chref)
{
	return "face";
}

void FillFaceList(string strAccess, ref chref, int fillCode)
{
	aref ar;
	makearef(ar, GameInterface.(strAccess));

	int n, cn, q;

	AddFaceGroup(strAccess,"FACE128_"+chref.faceID);

	if(fillCode==0) // officers
	{
		//officers
		if (chref.Fellows.Passengers.navigator == "-1")
		{
			AddFaceGroup(strAccess,"FACE128_NAVIGATOR");
		}
		else
		{
			AddFaceGroup(strAccess,"FACE128_"+Characters[sti(chref.Fellows.Passengers.navigator)].faceID);
		}

		if (chref.Fellows.Passengers.boatswain == "-1")
		{
			AddFaceGroup(strAccess,"FACE128_BOATSWAIN");
		}
		else
		{
			AddFaceGroup(strAccess,"FACE128_"+Characters[sti(chref.Fellows.Passengers.boatswain)].faceID);
		}

		if (chref.Fellows.Passengers.cannoner == "-1")
		{
			AddFaceGroup(strAccess,"FACE128_cannoner");
		}
		else
		{
			AddFaceGroup(strAccess,"FACE128_"+Characters[sti(chref.Fellows.Passengers.cannoner)].faceID);
		}

		if (chref.Fellows.Passengers.doctor == "-1")
		{
			AddFaceGroup(strAccess,"FACE128_doctor");
		}
		else
		{
			AddFaceGroup(strAccess,"FACE128_"+Characters[sti(chref.Fellows.Passengers.doctor)].faceID);
		}

		if (chref.Fellows.Passengers.treasurer == "-1")
		{
			AddFaceGroup(strAccess,"FACE128_treasurer");
		}
		else
		{
			AddFaceGroup(strAccess,"FACE128_"+Characters[sti(chref.Fellows.Passengers.treasurer)].faceID);
		}

		if (chref.Fellows.Passengers.carpenter == "-1")
		{
			AddFaceGroup(strAccess,"FACE128_carpenter");
		}
		else
		{
			AddFaceGroup(strAccess,"FACE128_"+Characters[sti(chref.Fellows.Passengers.carpenter)].faceID);
		}

		//fighters
		for(n=1; n<=MAX_NUM_FIGHTERS; n++)
		{
			cn = GetOfficersIndex(chref,n);
			if( cn>=0 ) {
				AddFaceGroup(strAccess,"FACE128_"+Characters[cn].faceID);
			}
			else
			{
				AddFaceGroup(strAccess,"FACE128_fighter");
			}
		}
	}

	if(fillCode==1) // companions
	{
		for(n=1; n<COMPANION_MAX; n++)
		{
			cn = GetCompanionIndex(chref,n);
			if( cn>=0 ) {
				AddFaceGroup(strAccess,"FACE128_"+Characters[cn].faceID);
			}
		}
	}

	if(fillCode==2) // passengers
	{
		q = GetPassengersQuantity(chref);
		for(n=0; n<q; n++)
		{
			cn = GetPassenger(chref,n);
			if( cn>=0)
			{
				AddFaceGroup(strAccess,"FACE128_"+Characters[cn].faceID);
			}
		}
	}

	if(fillCode==3) // officer for hire
	{

	}
	if(fillCode==4) // free passengers
	{
		q = GetPassengersQuantity(chref);
		for(n=0; n<q; n++)
		{
			cn = GetPassenger(chref,n);
			if( cn>=0)
			{
				if (!CheckAttribute(&characters[cn], "prisoned") || sti(characters[cn].prisoned) != true)
				{
					if(!CheckAttribute(&characters[cn], "isfree"))
					{
						if(!CheckAttribute(&characters[cn], "isquest")) // to_do
						{
							AddFaceGroup(strAccess,"FACE128_"+Characters[cn].faceID);
						}
					}
				}
			}
		}
	}
}

void FillShipList(string strAccess, ref chref)
{
	string sShip;
	string potc = "";
	int n, cn;
	int iShipType;
	if (InterfaceStates.InterfaceShipString == "interfaces\ships_PotC\") potc = "_POTC";

	for(n=0; n<COMPANION_MAX; n++)
	{
		cn = GetCompanionIndex(Pchar, n);
		if(cn != -1)
		{
			iShipType = GetCharacterShipType(characters[cn]);
			if (iShipType > REAL_SHIPS_QUANTITY)
			{
				continue;
			}
			iShipType = sti(RealShips[iShipType].basetype);
			// LEO: проверять надо ТОТ ЖЕ элемент, который потом читаем. Здесь стояло ShipsTypes[n], где
			//   n - счётчик компаньонов, а не тип корабля: у 4-го компаньона проверялся 4-й тип судна.
			//   Когда проверка не проходила, имя выходило пустым и группа получалась "SHIPS_" (или
			//   "SHIPS__POTC") - портрет не находился ни на одном экране со скроллом кораблей.
			if (CheckAttribute(ShipsTypes[iShipType], "name")) sShip = ShipsTypes[iShipType].name;	else sShip = "";
			AddFaceGroup(strAccess,"SHIPS_"+sShip+potc);
		}
	}
}

void FillItemsPicturesList(string strAccess, ref chref)  // to_do del
{
	AddFaceGroup(strAccess, "WEAPONS");
	return;
}
void FillUpgradesList(string strAccess, string smode, int iCannonMaterial)  // to_do del
{
	aref ar;
	makearef(ar, GameInterface.(strAccess));

	int iMax;
	if(smode == "cannons")
	{
		iMax = CANNON_TYPES_QUANTITY + 1;
	}
	else
	{
		iMax = 4;
	}
	if(smode == "cannons")
	{
		AddFaceGroup(strAccess,smode + "_UPGRADE_"+iCannonMaterial);
	}
	else
	{
		for(int i = 1; i < iMax; i++)
		{
			AddFaceGroup(strAccess,smode + "_UPGRADE_"+i);
		}
	}
}

void AddFaceGroup(string strAccess, string groupName)
{
	if( FindFaceGroupNum(strAccess,groupName)<0 )
	{
		aref ar;
		makearef(ar, GameInterface.(strAccess));
		string attrName = "t"+GetAttributesNum(ar);
		ar.(attrName) = groupName;
	}
}

int FindFaceGroupNum(string strAccess, string groupName)
{
	int n, q;
	aref aRoot, aCur;
	makearef(aRoot, GameInterface.(strAccess));

	q = GetAttributesNum(aRoot);
	for(n=0; n<q; n++)
	{
		aCur = GetAttributeN(aRoot,n);
		if( GetAttributeValue(aCur) == groupName ) return n;
	}

	return -1;
}

string GlobalStringConvert(string strID)
{
	return LanguageConvertString( GlobalLngFileID(), strID );
}

void EnumerateIcons(string sDirectory, string sFileMask, string sControlName, int iAddListSize)
{
	object fileFinder;
	fileFinder.dir = sDirectory;
	fileFinder.mask = sFileMask;
	CreateEntity(&fileFinder, "FINDFILESINTODIRECTORY");
	aref arFileList;
	makearef(arFileList, fileFinder.filelist);
	int iNumFiles = GetAttributesNum(arFileList);
	for (int i = 0; i < iNumFiles; i++)
	{
		string sFile = "id" + i;
		string attrName = "pic" + (i + 1);

		GameInterface.(sControlName).(attrName).name1 = sDirectory + "\" + arFileList.(sFile);
		GameInterface.(sControlName).(attrName).FileName = arFileList.(sFile);
	}
	GameInterface.(sControlName).ListSize = iNumFiles + iAddListSize;
}

void CreateTooltip(string header, string text1, int color1, string text2, int color2, string text3, int color3, string text4, int color4, string picTexture, string picGroup, string picImage, int nPicWidth, int nPicHeight)
{
	// LEO: тултип поверх тултипа терял бы первый набор замков насовсем - узлы остались бы
	//   заперты, и окно перестало бы отзываться. Закрываем прошлый, если он ещё висит.
	CloseTooltip();
	// сохранить состояния окон и элементов
	int nSaveNodeState = -1;
	nSaveNodeState = XI_StoreNodeLocksWithOff();
	InterfaceStates.tooltip.savestate = nSaveNodeState;
	InterfaceStates.tooltip.oldcurrentnode = GetCurrentNode();
	// создать элементы тултипа:
	XI_MakeNode( "", "TOOLTIP_FADER", "tooltip_fader", 30000 ); // fader
	XI_MakeNode( "", "TOOLTIP_FRAME", "tooltip_frame", 30000 ); // окно
	XI_MakeNode( "", "TOOLTIP_TITLERECT", "tooltip_titlerect", 30001 ); // заголовок
	XI_MakeNode( "", "TOOLTIP_PICTURE", "tooltip_picture", 30001 ); // картинка
	XI_MakeNode( "", "TOOLTIP_TEXTRECT", "tooltip_textborder2", 30001 ); // окно для техт2
	XI_MakeNode( "", "TOOLTIP_TEXTRECT", "tooltip_textborder4", 30001 ); // окно для техт4
	XI_MakeNode( "", "TOOLTIP_TITLE", "tooltip_titile", 30002 ); // заголовок
	XI_MakeNode( "", "TOOLTIP_TEXT1", "tooltip_text1", 30002 ); //
	XI_MakeNode( "", "TOOLTIP_TEXT2", "tooltip_text2", 30002 ); //
	XI_MakeNode( "", "TOOLTIP_TEXT3", "tooltip_text3", 30002 ); //
	XI_MakeNode( "", "TOOLTIP_TEXT4", "tooltip_text4", 30002 ); //
	SendMessage(&GameInterface,"lsslslslslsssll",MSG_INTERFACE_SET_TOOLTIP, header, text1,color1, text2,color2, text3,color3, text4,color4, picTexture, picGroup, picImage, nPicWidth, nPicHeight );
}

void CloseTooltip()
{
	// удалить элементы тултипа:
	XI_DeleteNode( "tooltip_fader" ); // окно
	XI_DeleteNode( "tooltip_frame" ); // окно
	XI_DeleteNode( "tooltip_titlerect" ); // заголовок
	XI_DeleteNode( "tooltip_picture" ); // картинка
	XI_DeleteNode( "tooltip_textborder2" ); // окно
	XI_DeleteNode( "tooltip_textborder4" ); // окно
	XI_DeleteNode( "tooltip_titile" ); // заголовок
	XI_DeleteNode( "tooltip_text1" ); //
	XI_DeleteNode( "tooltip_text2" ); //
	XI_DeleteNode( "tooltip_text3" ); //
	XI_DeleteNode( "tooltip_text4" ); //
	// LEO: восстанавливаем состояние ТОЛЬКО если тултип действительно был открыт, и сразу
	//   забываем запомненный узел. Раньше oldcurrentnode не чистился, а CloseTooltip дёргают
	//   не только по закрытию: в торговле предметами его зовёт каждый отпуск правой кнопки и
	//   подтверждение сделки. После осмотра вещи в окне экипировки офицера фокус на первой же
	//   продаже уезжал на узел этого УЖЕ ЗАКРЫТОГО окна, и управление хоткеями умирало.
	int nSaveNodeState = -1;
	if( CheckAttribute(&InterfaceStates,"tooltip.savestate") ) {
		nSaveNodeState = sti(InterfaceStates.tooltip.savestate);
	}
	if( nSaveNodeState!=-1 ) {
		if( CheckAttribute(&InterfaceStates,"tooltip.oldcurrentnode") ) {
			SetCurrentNode(InterfaceStates.tooltip.oldcurrentnode);
			DeleteAttribute(&InterfaceStates,"tooltip.oldcurrentnode");
		}
		XI_RestoreNodeLocks(nSaveNodeState);
		InterfaceStates.tooltip.savestate = -1;
	}
}

string GetMoraleGroupPicture(float fMoraleValue)
{
	if( fMoraleValue >= 75.0 ) {return "high morale";}
	if( fMoraleValue >= 40.0 ) {return "medium morale";}
	return "low morale";
}

bool XI_FindFoldersWithoutNetsave(string sFindTemplate,aref arFoldersList)
{
		bool bRetVal = XI_FindFolders(sFindTemplate,arFoldersList);

	int num = GetAttributesNum(arFoldersList);
	int i = 0;
	for(i=0; i<num; i++) {
		if( "NetSaves" == GetAttributeValue(GetAttributeN(arFoldersList,i)) )
		{
			DeleteAttribute( arFoldersList, GetAttributeName(GetAttributeN(arFoldersList,i)) );
			return bRetVal;
		}
	}

	return bRetVal;
}
// boal -->
void ReadSavedOptionsEx(ref gopt)
{
	string sFileName = "options_start";
	SendMessage(&GameInterface, "lsa", MSG_INTERFACE_LOADOPTIONS, sFileName, gopt);
}

void SaveSavedOptionsEx(ref gopt)
{
	string sFileName = "options_start";
	SendMessage(&GameInterface, "lsa", MSG_INTERFACE_SAVEOPTIONS, sFileName, gopt);
}

void SaveStartGameParam()
{
    object gopt;

	ReadSavedOptionsEx(&gopt);

	ref optref = &gopt;

    optref.StartGameParam.PlayerProfile    = GameInterface.PROFILE_NAME.str;//PlayerProfile.name;
    optref.StartGameParam.MOD_SKILL_ENEMY_RATE   = MOD_SKILL_ENEMY_RATE;
	optref.StartGameParam.MOD_EXP_RATE   = MOD_EXP_RATE;
	optref.StartGameParam.MOD_DEAD_CLEAR_TIME   = MOD_DEAD_CLEAR_TIME;
	optref.StartGameParam.MOD_DEFENDERS_RATE   = MOD_DEFENDERS_RATE;
	optref.StartGameParam.MainChAnim   = MainChAnim;
    optref.StartGameParam.bHardcoreGame          = bHardcoreGame;
	optref.StartGameParam.bPartitionSet          = bPartitionSet;
	optref.StartGameParam.bSailorsWeight         = bSailorsWeight;
	optref.StartGameParam.bNewSails         	 = bNewSails;
	optref.StartGameParam.bHardAnimations  		 = bHardAnimations;
	optref.StartGameParam.bHardBoss  		 	 = bHardBoss;
	optref.StartGameParam.bHigherShipRate  		 = bHigherShipRate;
	optref.StartGameParam.bHigherSelfRate  		 = bHigherSelfRate;
	optref.StartGameParam.bRankRequirement  	 = bRankRequirement;
	optref.StartGameParam.bHalfImmortalPGG  	 = bHalfImmortalPGG;
	optref.StartGameParam.bPortPermission  		 = bPortPermission;
	optref.StartGameParam.bNoBonusSkillOff  	 = bNoBonusSkillOff;
    optref.StartGameParam.bRechargePistolOnLine  = bRechargePistolOnLine;
    optref.StartGameParam.iEncountersRate        = iEncountersRate;
	optref.StartGameParam.iEncountersCountRate   = iEncountersCountRate;
	optref.StartGameParam.iArcadeSails           = iArcadeSails;
	optref.StartGameParam.bAltBalance          	 = bAltBalance;
	optref.StartGameParam.bAltBalanceTimeSlow    = bAltBalanceTimeSlow;
	optref.StartGameParam.bAltBalanceOffTopPerk  = bAltBalanceOffTopPerk;
	optref.StartGameParam.bAltBalanceProHits     = bAltBalanceProHits;
	optref.StartGameParam.bFillEncyShips         = bFillEncyShips;
	optref.StartGameParam.bDifficultyWeight      = bDifficultyWeight;
	optref.StartGameParam.bModDamage      		 = bModDamage;
	optref.StartGameParam.bShootOnlyEnemy		 = bShootOnlyEnemy;
	optref.StartGameParam.iStealthSystem         = iStealthSystem;
    // иначе сброс галки может быть 
	optref.StartGameParam.bWorldAlivePause       = bWorldAlivePause;
	optref.StartGameParam.bQuestsRank            = bQuestsRank;

    optref.StartGameParam.HeroType         = NullCharacter.HeroParam.HeroType;
    optref.StartGameParam.Nation           = NullCharacter.HeroParam.Nation;
    optref.StartGameParam.CurHeroNum       = startHeroType;
	
	int  heroQty   = sti(GetNewMainCharacterParam("ps_hero_qty"));
	for (int n=1; n<=heroQty; n++)
	{
		string sBlockPGG = "PGG"+n;
		if (CheckAttribute(pchar,"RemovePGG." + sBlockPGG) && sti(pchar.RemovePGG.(sBlockPGG)) == 1)
		{
			optref.StartGameParam.DisabledPGGs.(sBlockPGG) = 1;
		}
		else optref.StartGameParam.DisabledPGGs.(sBlockPGG) = 0;
	}
	

	SaveSavedOptionsEx(&gopt);
}
void LoadStartGameParam()
{
    object gopt;

	ReadSavedOptionsEx(&gopt);

	ref optref = &gopt;

	if (CheckAttribute(optref, "StartGameParam.PlayerProfile"))
	{
    	GameInterface.PROFILE_NAME.str = optref.StartGameParam.PlayerProfile;
    }

    if (CheckAttribute(optref, "StartGameParam.MOD_SKILL_ENEMY_RATE"))
	{
    	MOD_SKILL_ENEMY_RATE = sti(optref.StartGameParam.MOD_SKILL_ENEMY_RATE);
    }
	if (CheckAttribute(optref, "StartGameParam.MOD_EXP_RATE"))
	{
    	MOD_EXP_RATE = sti(optref.StartGameParam.MOD_EXP_RATE);
    }
	if (CheckAttribute(optref, "StartGameParam.MOD_DEAD_CLEAR_TIME"))
	{
    	MOD_DEAD_CLEAR_TIME = sti(optref.StartGameParam.MOD_DEAD_CLEAR_TIME);
    }
	if (CheckAttribute(optref, "StartGameParam.MOD_DEFENDERS_RATE"))
	{
    	MOD_DEFENDERS_RATE = sti(optref.StartGameParam.MOD_DEFENDERS_RATE);
    }
	if (CheckAttribute(optref, "StartGameParam.MainChAnim"))
	{
		MainChAnim = optref.StartGameParam.MainChAnim;
    }
    if (CheckAttribute(optref, "StartGameParam.bHardcoreGame"))
	{
    	bHardcoreGame = sti(optref.StartGameParam.bHardcoreGame);
    }
	if (CheckAttribute(optref, "StartGameParam.bPartitionSet"))
	{
    	bPartitionSet = sti(optref.StartGameParam.bPartitionSet);
    }
	if (CheckAttribute(optref, "StartGameParam.bSailorsWeight"))
	{
    	bSailorsWeight = sti(optref.StartGameParam.bSailorsWeight);
    }
	if (CheckAttribute(optref, "StartGameParam.bNewSails"))
	{
    	bNewSails = sti(optref.StartGameParam.bNewSails);
    }
	if (CheckAttribute(optref, "StartGameParam.bHardAnimations"))
	{
    	bHardAnimations = sti(optref.StartGameParam.bHardAnimations);
    }
	if (CheckAttribute(optref, "StartGameParam.bHardBoss"))
	{
    	bHardBoss = sti(optref.StartGameParam.bHardBoss);
    }
	if (CheckAttribute(optref, "StartGameParam.bHigherShipRate"))
	{
    	bHigherShipRate = sti(optref.StartGameParam.bHigherShipRate);
    }
	if (CheckAttribute(optref, "StartGameParam.bHigherSelfRate"))
	{
    	bHigherSelfRate = sti(optref.StartGameParam.bHigherSelfRate);
    }
	if (CheckAttribute(optref, "StartGameParam.bRankRequirement"))
	{
    	bRankRequirement = sti(optref.StartGameParam.bRankRequirement);
    }
	if (CheckAttribute(optref, "StartGameParam.bHalfImmortalPGG"))
	{
    	bHalfImmortalPGG = sti(optref.StartGameParam.bHalfImmortalPGG);
    }
	if (CheckAttribute(optref, "StartGameParam.bNoBonusSkillOff"))
	{
    	bNoBonusSkillOff = sti(optref.StartGameParam.bNoBonusSkillOff);
    }
	if (CheckAttribute(optref, "StartGameParam.bPortPermission"))
	{
    	bPortPermission = sti(optref.StartGameParam.bPortPermission);
    }
    if (CheckAttribute(optref, "StartGameParam.bRechargePistolOnLine"))
	{
    	bRechargePistolOnLine = sti(optref.StartGameParam.bRechargePistolOnLine);
    }
    if (CheckAttribute(optref, "StartGameParam.iEncountersRate"))
	{
    	iEncountersRate = sti(optref.StartGameParam.iEncountersRate);
    }
	if (CheckAttribute(optref, "StartGameParam.iEncountersCountRate"))
	{
    	iEncountersCountRate = sti(optref.StartGameParam.iEncountersCountRate);
    }
    if (CheckAttribute(optref, "StartGameParam.iArcadeSails"))
	{
    	iArcadeSails = sti(optref.StartGameParam.iArcadeSails);
    }
    if (CheckAttribute(optref, "StartGameParam.bWorldAlivePause"))
	{
    	bWorldAlivePause = sti(optref.StartGameParam.bWorldAlivePause);
    }
    if (CheckAttribute(optref, "StartGameParam.bQuestsRank"))
    {
        bQuestsRank = sti(optref.StartGameParam.bQuestsRank);
    }
    //////
    if (CheckAttribute(optref, "StartGameParam.HeroType"))
	{
    	NullCharacter.HeroParam.HeroType = optref.StartGameParam.HeroType;
    }
    if (CheckAttribute(optref, "StartGameParam.Nation"))
	{
    	NullCharacter.HeroParam.Nation = optref.StartGameParam.Nation;
    }
    if (CheckAttribute(optref, "StartGameParam.CurHeroNum"))
	{
		startHeroType = sti(optref.StartGameParam.CurHeroNum);
	}
	
	if (CheckAttribute(optref, "StartGameParam.bAltBalance"))
	{
    	bAltBalance = sti(optref.StartGameParam.bAltBalance);
    }
	if (CheckAttribute(optref, "StartGameParam.bAltBalanceTimeSlow"))
	{
    	bAltBalanceTimeSlow = sti(optref.StartGameParam.bAltBalanceTimeSlow);
    }
	if (CheckAttribute(optref, "StartGameParam.bAltBalanceOffTopPerk"))
	{
    	bAltBalanceOffTopPerk = sti(optref.StartGameParam.bAltBalanceOffTopPerk);
    }
	if (CheckAttribute(optref, "StartGameParam.bAltBalanceProHits"))
	{
    	bAltBalanceProHits = sti(optref.StartGameParam.bAltBalanceProHits);
    }
	if (CheckAttribute(optref, "StartGameParam.bFillEncyShips"))
	{
    	bFillEncyShips = sti(optref.StartGameParam.bFillEncyShips);
    }
	if (CheckAttribute(optref, "StartGameParam.bDifficultyWeight"))
	{
    	bDifficultyWeight = sti(optref.StartGameParam.bDifficultyWeight);
    }
	if (CheckAttribute(optref, "StartGameParam.bModDamage"))
	{
    	bModDamage = stf(optref.StartGameParam.bModDamage);
    }
	if (CheckAttribute(optref, "StartGameParam.iStealthSystem"))
	{
    	iStealthSystem = sti(optref.StartGameParam.iStealthSystem);
    }
	if (CheckAttribute(optref, "StartGameParam.bShootOnlyEnemy"))
	{
    	bShootOnlyEnemy = sti(optref.StartGameParam.bShootOnlyEnemy);
    }
	int  heroQty   = sti(GetNewMainCharacterParam("ps_hero_qty"));
	for (int n=1; n<=heroQty; n++)
	{
		string sBlockPGG = "PGG"+n;
		if (CheckAttribute(optref,"StartGameParam.DisabledPGGs." + sBlockPGG) && sti(optref.StartGameParam.DisabledPGGs.(sBlockPGG)) == 1)
		{
			pchar.RemovePGG.(sBlockPGG) = 1;
		}
		else pchar.RemovePGG.(sBlockPGG) = 0;
	}
}
void LoadPlayerProfileDefault()
{
    object gopt;

	ReadSavedOptionsEx(&gopt);

	ref optref = &gopt;

	if (CheckAttribute(optref, "StartGameParam.PlayerProfile"))
	{
    	PlayerProfile.name = optref.StartGameParam.PlayerProfile;
    }
}

// boal сложность -->
string GetLevelComplexity(int _Level_Complexity)
{
	switch (_Level_Complexity)
	{
	    case 1 :
            return XI_ConvertString("m_Complexity_1"); // Матрос;
            break;
        case 2 :
            return XI_ConvertString("m_Complexity_2"); // Капитан;
            break;
        case 3 :
            return XI_ConvertString("m_Complexity_3");// Адмирал;
            break;
    }
}

// GetAchievementName / GetAchievementDescrible / GetAchievementIcon (+ варианты *H
// с переиспользуемым хендлом) перенесены в Program\Achievements.c.

// LEO: цвета бонусов предмета. Инлайн-теги понимают и FORMATEDTEXT (инвентарь, сундук, магазин), и
//   TABLE (энциклопедия) - там тег в начале строки красит её целиком.
//   ⚠️ Теги держим В ЛОКАЛИЗАЦИИ, а не литералом в скрипте: компилятор Storm портит в литерале знак
//   '=', в кадр уезжало «<color-...>» с дефисом. А без '=' разметка молча пропадает и не красит:
//   GetLineNext вырезает её по шести символам «<color», но цвет ставит MakeTagChecking по «<color=».
//   Значение кэшируем - энциклопедия строит описания сразу для всех предметов, и открывать под каждый
//   файл локализации накладно.
string sItemBonusColorGood = "";
string sItemBonusColorBad = "";

string ItemBonusColorGood()
{
	if (sItemBonusColorGood == "") sItemBonusColorGood = GetConvertStr("Variable_ColorGood", "Interface.txt");
	return sItemBonusColorGood;
}

string ItemBonusColorBad()
{
	if (sItemBonusColorBad == "") sItemBonusColorBad = GetConvertStr("Variable_ColorBad", "Interface.txt");
	return sItemBonusColorBad;
}

// LEO: сами строки-бонусы размечены тегами прямо в ItemsDescribe.txt (как в HeroDescribe.txt на экране
//   выбора персонажа). Разбирать их в скрипте нечем: в Storm НЕТ escape-последовательностей, "\n" в
//   коде - это два символа, а не перевод строки, и findsubstr по нему всегда возвращал -1.
string GetItemDescribe(string sItemID)
{
	int iGoodIndex = Items_FindItemIdx(sItemID);
	ref    arItm = &Items[iGoodIndex];
	int    lngFileID = LanguageOpenFile("ItemsDescribe.txt");
    string describeStr = "";
	string ammotype = "";

	if (CheckAttribute(arItm, "groupID"))
	{
		//aw013 -->
		//aw013 <--

		if(arItm.groupID == GUN_ITEM_TYPE)
		{
			if(CheckAttribute(arItm,"type.t1") && !CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
			{
				ammotype = LanguageConvertString(lngFileID, "itmname_"+arItm.type.t1.bullet) + ".";
				describeStr += GetAssembledString(
				LanguageConvertString(lngFileID,"weapon gun parameters"),
				arItm) + "."  + NewStr() + GetConvertStrWithReplace("Variable_utilite_3", "Interface.txt", "#space#", " ") + ammotype + newStr();
			}
			if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
			{
				ammotype = LanguageConvertString(lngFileID, "itmname_"+arItm.type.t1.bullet) + "/" + LanguageConvertString(lngFileID, "itmname_"+arItm.type.t2.bullet) + ".";
				int dmg21m = arItm.type.t1.DmgMin_NC;
				int dmg21ma = arItm.type.t1.DmgMax_NC;
				int dmg21a = arItm.type.t1.Accuracy;
				int dmg21c = arItm.type.t1.ChargeSpeed;
				int dmg22m = arItm.type.t2.DmgMin_NC;
				int dmg22ma = arItm.type.t2.DmgMax_NC;
				int dmg22a = arItm.type.t2.Accuracy;
				int dmg22c = arItm.type.t2.ChargeSpeed;
				float ammoMult1 = LAi_AmmoDamageMultiplier(arItm.type.t1.bullet);
				float ammoMult2 = LAi_AmmoDamageMultiplier(arItm.type.t2.bullet);
				dmg21m = MakeInt(dmg21m * ammoMult1);
				dmg21ma = MakeInt(dmg21ma * ammoMult1);
				dmg22m = MakeInt(dmg22m * ammoMult2);
				dmg22ma = MakeInt(dmg22ma * ammoMult2);
				string sTemp;
				if (dmg21a != dmg22a) sTemp =  dmg21a+"%/"+dmg22a+"%";
				else sTemp = dmg21a+"%";
				string sTemp1;
				if(dmg21m == dmg22m || dmg21ma == dmg22ma ) sTemp1 = GetConvertStrWithReplace("Variable_utilite_28", "Interface.txt", "#space#", " ") + dmg21m+"/"+dmg21ma;
				else sTemp1 = GetConvertStrWithReplace("Variable_utilite_4", "Interface.txt", "#space#", " ")+dmg21m+"/"+dmg22m + GetConvertStrWithReplace("Variable_utilite_5", "Interface.txt", "#space#", " ") + dmg21ma+"/"+dmg22ma;
				string sTemp2;
				if (dmg21c == dmg22c) sTemp2 = dmg21c;
				else sTemp2 = dmg21c+"/"+dmg22c;
				describeStr += GetConvertStrWithReplace("Variable_utilite_6", "Interface.txt", "#space#", " ") + sTemp1 +  NewStr()+GetConvertStrWithReplace("Variable_utilite_7", "Interface.txt", "#space#", " ")+ sTemp2 +GetConvertStrWithReplace("Variable_utilite_8", "Interface.txt", "#space#", " ")+ sTemp  + NewStr() + GetConvertStrWithReplace("Variable_utilite_9", "Interface.txt", "#enter#", "\n") + ammotype + NewStr();
			}
			if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && CheckAttribute(arItm,"type.t3"))
			{
				ammotype = LanguageConvertString(lngFileID, "itmname_"+arItm.type.t1.bullet) + "/" + LanguageConvertString(lngFileID, "itmname_"+arItm.type.t2.bullet) + "/" + LanguageConvertString(lngFileID, "itmname_"+arItm.type.t3.bullet) + ".";
				int dmg31m = arItm.type.t1.DmgMin_NC;
				int dmg31ma = arItm.type.t1.DmgMax_NC;
				int dmg31a = arItm.type.t1.Accuracy;
				int dmg31c = arItm.type.t1.ChargeSpeed;
				int dmg32m = arItm.type.t2.DmgMin_NC;
				int dmg32ma = arItm.type.t2.DmgMax_NC;
				int dmg32a = arItm.type.t2.Accuracy;
				int dmg32c = arItm.type.t2.ChargeSpeed;
				int dmg33m = arItm.type.t3.DmgMin_NC;
				int dmg33ma = arItm.type.t3.DmgMax_NC;
				int dmg33a = arItm.type.t3.Accuracy;
				int dmg33c = arItm.type.t3.ChargeSpeed;
				if (dmg31a == dmg32a && dmg32a == dmg33a) sTemp = dmg31a+"%";
				else  sTemp =  dmg31a+"%/"+dmg32a+"%/"+dmg33a+"%";
				if(dmg31m == dmg32m && dmg33m == dmg32m) sTemp1 = GetConvertStrWithReplace("Variable_utilite_28", "Interface.txt", "#space#", " ") + dmg31m+"/"+dmg31ma;
				else sTemp1 = GetConvertStrWithReplace("Variable_utilite_10", "Interface.txt", "#space#", " ")+dmg31m+"/"+dmg32m+"/"+dmg33m+GetConvertStrWithReplace("Variable_utilite_11", "Interface.txt", "#space#", " ")+dmg31ma+"/"+dmg32ma+"/"+dmg33ma;
				if (dmg31c == dmg32c && dmg32c == dmg33c) sTemp2 = dmg31c;
				else sTemp2 = dmg31c+"/"+dmg32c+"/"+dmg33c;
				describeStr += GetConvertStrWithReplace("Variable_utilite_12", "Interface.txt", "#space#", " ")+sTemp1+","+ NewStr()+GetConvertStrWithReplace("Variable_utilite_13", "Interface.txt", "#space#", " ")+sTemp2+GetConvertStrWithReplace("Variable_utilite_14", "Interface.txt", "#space#", " ")+sTemp + NewStr() + GetConvertStrWithReplace("Variable_utilite_15", "Interface.txt", "#enter#", "\n") + ammotype + NewStr();
			}
			if (CheckAttribute(arItm,"ReqPerk"))
			{
				if (arItm.ReqPerk == "Gunman") describeStr += GetConvertStrWithReplace("Variable_utilite_16", "Interface.txt", "#space#", " ")+ NewStr();
				if (arItm.ReqPerk == "GunProfessional") describeStr += GetConvertStrWithReplace("Variable_utilite_17", "Interface.txt", "#space#", " ")+ NewStr();
			}
			else describeStr += GetConvertStrWithReplace("Variable_utilite_18", "Interface.txt", "#space#", " ")+ NewStr();
		}
		if(arItm.groupID==BLADE_ITEM_TYPE)
		{
			float dmg_min, dmg_max, weight;
			GetBladeParams(sItemID, &dmg_min, &dmg_max, &weight);
			int price = CalculateBladePrice(arItm.FencingType, dmg_min, dmg_max, weight);

			// Собираем фейковый айтем, так как генерируемые параметры не содержатся в изначальном айтеме
			object tempObj;
			aref arTemp;
			makearef(arTemp, tempObj);
			CopyAttributes(arTemp, arItm);
			tempObj.dmg_min = dmg_min;
			tempObj.dmg_max = dmg_max;
			tempObj.weight = weight;
			tempObj.price = price;

			describeStr += GetAssembledString(
				LanguageConvertString(lngFileID,"weapon blade parameters"),
				arTemp) + newStr();
			
			if (CheckAttribute(arItm, "FencingType"))
			{
    			arItm.FencingTypeName = XI_ConvertString(arItm.FencingType);
    			describeStr += GetAssembledString( LanguageConvertString(lngFileID,"weapon blade type"), arItm) + newStr();
			}
			else
			{
                describeStr += "ERROR" + newStr();
			}
			describeStr += newStr() + GetOtherBladeInfo(arItm) + newStr(); // EvgAnat
		}
	}

	// eddy. грамота ГВИК (HolTradeLicence): текст-вставку #sUniNote# в её описании определяем
	// по ТЕКУЩЕМУ статусу прямо при показе - иначе на старых сейвах (где атрибут не выставлен)
	// или после перезагрузки плейсхолдер заменяется пустотой. arItm == &Items[iGoodIndex].
	if (sItemID == "HolTradeLicence")
	{
		arItm.UniNote = GetUniLicenceNote(IsUniLicenceActive());
	}

	describeStr = describeStr + GetAssembledString(LanguageConvertString(lngFileID, Items[iGoodIndex].describe), &Items[iGoodIndex]);

	// LEO: бафы, которые считает код, идут ПОСЛЕ описания - сперва что это за вещь, потом что она даёт.
	//   Перенос литералом: NewStr() здесь отдаёт пустоту, заново открывая уже открытый ItemsDescribe.txt.
	if (CheckAttribute(arItm, "groupID"))
	{
		// подсумок: прибавка к переносимому весу
		if (arItm.groupID == BACKPACK_ITEM_TYPE)
		{
			describeStr += "\n" + ItemBonusColorGood() +
				GetAssembledString(LanguageConvertString(lngFileID, "BackPackWeight"), arItm) + "</color>";
		}
		// книга: сколько даст опыта. Берём из BookBonus - оттуда же, откуда начисляет TryReadBook,
		//   чтобы текст не разъезжался с балансом.
		if (arItm.groupID == BOOK_ITEM_TYPE)
		{
			if (CheckAttribute(arItm, "skill"))
			{
				int iBookExp = BookBonus(sItemID);
				if (iBookExp > 0)
				{
					describeStr += "\n" + ItemBonusColorGood() + "+" + iBookExp + " " +
						GetConvertStrWithReplace("Variable_utilite_BookExp", "Interface.txt", "#space#", " ") +
						" " + XI_ConvertString(arItm.skill) + "</color>";
				}
			}
		}
	}
	if (CheckAttribute(arItm, "potion"))
	{
		if (CheckAttribute(arItm, "potion.health"))
		{
			// LEO: восстановление здоровья - баф, потеря - дебаф; красим по знаку, как бонусы предметов
			describeStr += "\n";
			if (stf(arItm.potion.health) >= 0)
			{
				describeStr += ItemBonusColorGood() + LanguageConvertString(lngFileID, "Potion parameters") +
					": HP + " + sti(arItm.potion.health) + "</color>";
			}
			else
			{
				describeStr += ItemBonusColorBad() + LanguageConvertString(lngFileID, "Potion parameters") +
					": HP" + sti(arItm.potion.health) + "</color>";
			}
		}
		// LEO: снятие отравления - тоже баф, и игрок о нём нигде не узнавал. Признак - potion.antidote,
		//   по нему же лечит UseItem (items_utilite.c): противоядие, микстура и бутылка рома.
		if (CheckAttribute(arItm, "potion.antidote"))
		{
			describeStr += "\n" + ItemBonusColorGood() +
				GetConvertStrWithReplace("Variable_utilite_Antidote", "Interface.txt", "#space#", " ") + "</color>";
		}
	}
	if (CheckAttribute(arItm, "Food"))
	{
		if (CheckAttribute(arItm, "Food.energy"))
		{
			// LEO: прибавка энергии - баф, убыль - дебаф (см. блок зелий выше)
			describeStr += "\n";
			if (stf(arItm.Food.energy) >= 0)
			{
				describeStr += ItemBonusColorGood() + LanguageConvertString(lngFileID, "Energy parameters") +
					": + " + sti(arItm.Food.energy) + "</color>";
			}
			else
			{
				describeStr += ItemBonusColorBad() + LanguageConvertString(lngFileID, "Energy parameters") +
					": " + sti(arItm.Food.energy) + "</color>";
			}
		}
	}

	describeStr += "\n" +GetConvertStrWithReplace("Variable_utilite_26", "Interface.txt", "#space#", " ")+ GetItemPrice(sItemID) + " " + GetConvertStr("Variable_utilite_27", "Interface.txt") + " " + FloatToString(GetItemWeight(sItemID), 2) + newStr();
	if (CheckAttribute(arItm, "groupID"))//Книги, процент прочитанности - Gregg
	{
		if (arItm.groupID == BOOK_ITEM_TYPE)
		{
			string sId = arItm.id;
			if (CheckAttribute(pchar, "books." + sId))
			{
				int readtime = BookReadTime(sId);
				float value = 100 * (1.0 - stf(pchar.books.(sId)) / readtime);
				string text = FloatToString(value, 1);
				describeStr += GetConvertStrWithReplace("Variable_utilite_19", "Interface.txt", "#space#", " ") + text + "%.";
			}
		}
	}
	//aw013 <--

	LanguageCloseFile(lngFileID);

	return describeStr;
}

string GetGunDamage(ref arItm)
{
	if (arItm.groupID != GUN_ITEM_TYPE) return "-------";
	if(CheckAttribute(arItm,"type.t1") && !CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
	{
		int dmg11m = arItm.type.t1.DmgMin_NC;
		int dmg11ma = arItm.type.t1.DmgMax_NC;
		return GetConvertStrWithReplace("Variable_utilite_20", "Interface.txt", "#space#", " ")+dmg11m+"\n"+GetConvertStrWithReplace("Variable_utilite_21", "Interface.txt", "#space#", " ")+dmg11ma;
	}
	if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
	{
		int dmg21m = arItm.type.t1.DmgMin_NC;
		int dmg21ma = arItm.type.t1.DmgMax_NC;
		int dmg22m = arItm.type.t2.DmgMin_NC;
		int dmg22ma = arItm.type.t2.DmgMax_NC;
		float ammoMult1 = LAi_AmmoDamageMultiplier(arItm.type.t1.bullet);
		float ammoMult2 = LAi_AmmoDamageMultiplier(arItm.type.t2.bullet);
		dmg21m = MakeInt(dmg21m * ammoMult1);
		dmg21ma = MakeInt(dmg21ma * ammoMult1);
		dmg22m = MakeInt(dmg22m * ammoMult2);
		dmg22ma = MakeInt(dmg22ma * ammoMult2);
		string sTemp2;
		if(dmg21m == dmg22m || dmg21ma == dmg22ma ) sTemp2 =  GetConvertStrWithReplace("Variable_utilite_22", "Interface.txt", "#space#", " ")+dmg21m+"\n" + GetConvertStrWithReplace("Variable_utilite_21", "Interface.txt", "#space#", " ") +dmg21ma;
		else sTemp2 = GetConvertStrWithReplace("Variable_utilite_22", "Interface.txt", "#space#", " ")+dmg21m+"/"+dmg22m + "\n" + GetConvertStrWithReplace("Variable_utilite_21", "Interface.txt", "#space#", " ") + dmg21ma+"/"+dmg22ma;
		return sTemp2;
	}
	if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && CheckAttribute(arItm,"type.t3"))
	{
		int dmg31m = arItm.type.t1.DmgMin_NC;
		int dmg31ma = arItm.type.t1.DmgMax_NC;
		int dmg32m = arItm.type.t2.DmgMin_NC;
		int dmg32ma = arItm.type.t2.DmgMax_NC;
		int dmg33m = arItm.type.t3.DmgMin_NC;
		int dmg33ma = arItm.type.t3.DmgMax_NC;
		string sTemp3;
		if(dmg31m == dmg32m && dmg33m == dmg32m) sTemp3 = GetConvertStrWithReplace("Variable_utilite_22", "Interface.txt", "#space#", " ")+dmg31m+"\n" + GetConvertStrWithReplace("Variable_utilite_21", "Interface.txt", "#space#", " ")+dmg31ma;
		else sTemp3 = GetConvertStrWithReplace("Variable_utilite_22", "Interface.txt", "#space#", " ")+dmg31m+"/"+dmg32m+"/"+dmg33m+"\n" + GetConvertStrWithReplace("Variable_utilite_21", "Interface.txt", "#space#", " ")+dmg31ma+"/"+dmg32ma+"/"+dmg33ma;
		return sTemp3;
	}
}

string GetAmmoTypes(ref arItm)
{
	int lngFileID = LanguageOpenFile("ItemsDescribe.txt");
	if (arItm.groupID != GUN_ITEM_TYPE) return "-------";
	if(CheckAttribute(arItm,"type.t1") && !CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
	{
		return LanguageConvertString(lngFileID, "itmname_"+arItm.type.t1.bullet);
	}
	if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
	{
		return LanguageConvertString(lngFileID, "itmname_"+arItm.type.t1.bullet) + "/" + LanguageConvertString(lngFileID, "itmname_"+arItm.type.t2.bullet);
	}
	if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && CheckAttribute(arItm,"type.t3"))
	{
		return LanguageConvertString(lngFileID, "itmname_"+arItm.type.t1.bullet) + "/" + LanguageConvertString(lngFileID, "itmname_"+arItm.type.t2.bullet) + "/" + LanguageConvertString(lngFileID, "itmname_"+arItm.type.t3.bullet);
	}
	LanguageCloseFile(lngFileID);
}

string GetAccuracy(ref arItm)
{
	if (arItm.groupID != GUN_ITEM_TYPE) return "-------";
	if(CheckAttribute(arItm,"type.t1") && !CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
	{
		return arItm.type.t1.Accuracy+"%";
	}
	if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
	{
		int dmg21a = arItm.type.t1.Accuracy;
		int dmg22a = arItm.type.t2.Accuracy;
		if (dmg21a != dmg22a) return dmg21a+"%/"+dmg22a+"%";
		else return dmg21a+"%";
	}
	if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && CheckAttribute(arItm,"type.t3"))
	{
		int dmg31a = arItm.type.t1.Accuracy;
		int dmg32a = arItm.type.t2.Accuracy;
		int dmg33a = arItm.type.t3.Accuracy;
		if (dmg31a == dmg32a && dmg32a == dmg33a) return dmg31a+"%";
		else  return dmg31a+"%/"+dmg32a+"%/"+dmg33a+"%";
	}
}

string GetRecharge(ref arItm)
{
	if (arItm.groupID != GUN_ITEM_TYPE) return "-------";
	if(CheckAttribute(arItm,"type.t1") && !CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
	{
		return arItm.type.t1.ChargeSpeed;
	}
	if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && !CheckAttribute(arItm,"type.t3"))
	{
		int dmg21a = arItm.type.t1.ChargeSpeed;
		int dmg22a = arItm.type.t2.ChargeSpeed;
		if (dmg21a != dmg22a) return dmg21a+"/"+dmg22a;
		else return its(dmg21a);
	}
	if(CheckAttribute(arItm,"type.t1") && CheckAttribute(arItm,"type.t2") && CheckAttribute(arItm,"type.t3"))
	{
		int dmg31a = arItm.type.t1.ChargeSpeed;
		int dmg32a = arItm.type.t2.ChargeSpeed;
		int dmg33a = arItm.type.t3.ChargeSpeed;
		if (dmg31a == dmg32a && dmg32a == dmg33a) return its(dmg31a);
		else return dmg31a+"/"+dmg32a+"/"+dmg33a;
	}
}

string MsgIS(string _str)
{
	return GetConvertStr(_str, "interface_strings.txt");
}

void Table_UpdateWindow(string sTableControl)
{
	SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, sTableControl, 0);
}

void Table_Clear(string sTableControl, bool bClearHeader, bool bClearContent, bool bUpdateWindow)
{
	if (bClearHeader)
	{
		DeleteAttribute(&GameInterface, sTableControl + ".hr");
	}

	if (bClearContent)
	{
		for (int i=1; i<2000; i++)
		{
			string sA = sTableControl + "." + "tr" + i;
			if (!CheckAttribute(&GameInterface, sA)) { break; }
			DeleteAttribute(&GameInterface, sA);
		}
	}

	if (bUpdateWindow)
	{
		Table_UpdateWindow(sTableControl);
	}
}
void Button_SetText(string sButtonControl, string sText)
{
	SendMessage( &GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, sButtonControl, 0, sText);
}

void Button_SetEnable(string sButtonControl, bool bEnable)
{
	SetSelectable(sButtonControl, bEnable);
}

void CheckButton_SetDisable(string sControl, int iControlIndex, bool bDisableState)
{
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, sControl, 5, iControlIndex, bDisableState);
}

int CheckButton_GetState(string sControl, int iControlIndex)
{
	return SendMessage(&GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, sControl, 3, iControlIndex);
}

void CheckButton_SetState(string sControl, int iControlIndex, bool bState)
{
	SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, sControl, 2, iControlIndex, bState);
}

void StringCollection_SetText(string sControl, int iControlIndex, string sText)
{
	SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, sControl, 1, iControlIndex, sText);
}

void StringCollection_SetTextValue(string sControl, int iControlIndex, int iValue)
{
	string sValue = "#" + iValue;
	SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, sControl, 1, iControlIndex, sValue);
}

void ImageCollection_ChangeTextureGroup(string sControl, string sNewTextureGroup)
{
	SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, sControl, 1, sNewTextureGroup);
}

void ImageCollection_SetColor(string sControl, int iCIndex, int Color)
{
	SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, sControl, 3, iCIndex, Color);
}

void ScrollImage_SetPosition(string sControl, int iPosition)
{
	SendMessage(&GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, sControl, 1, iPosition);
}

// boal <--

string GetOtherBladeInfo(ref arItm) // EvgAnat - добавление иконок спецэффектов ХО в инфошки
{
	aref arSpecial, arProp;
	makearef(arSpecial, arItm.special);
	string sName, sVal, sSymb, sInfo;
	int n = GetAttributesNum(arSpecial);
	int i;
	sInfo = "";
	for(i=0; i<n; i++)
	{
		arProp = GetAttributeN(arSpecial, i);
		sName = GetAttributeName(arProp);
		sVal = GetAttributeValue(arProp);
		switch(sName)
		{
			case "valueBB":		sSymb = "‡";	break; 
			case "valueCrB":	sSymb = "„";	break;
			case "valueCB":		sSymb = "ƒ";	break;
			case "valueSS":		sSymb = "…";	break;
			case "valueStS":	sSymb = "†";	break;
			case "valueT":		sSymb = "Š";	break;
			case "valueB":		sSymb = "‰";	break;
			case "valueP":		sSymb = "ˆ";	break;
			case "valueV":		sSymb = "‹"; sVal = "50";	break; 
		}
		sInfo += sSymb + " " + sVal + "%   ";
	}
	if (sInfo != "")
		sInfo = strcut(sInfo, 0, strlen(sInfo)-4);
	return sInfo;
}

string GetIconFromFont(string _name)
{
	string sTemp = "_";
	switch (_name)
	{											//https://scriptsource.org/cms/scripts/page.php?item_id=character_list&key=0080
		case "fighter": sTemp = "ò";break;		//00F2
		case "navigator": sTemp = "ó";break;
		case "boatswain": sTemp = "ô";break;
		case "cannoner": sTemp = "õ";break;
		case "doctor": sTemp = "ö";break;
		case "treasurer": sTemp = "÷";break;
		case "carpenter": sTemp = "ø";break;
		case "SeaWolf": sTemp = "ù";break;
		case "Trader": sTemp = "ú";break;
		case "Adventurer": sTemp = "û";break;
		case "Agent": sTemp = "ü";break;
		case "Buccaneer": sTemp = "ý";break;
		case "Fencer": sTemp = "þ";break;
		case "Grunt": sTemp = "ÿ";break;		//00FF
	}
	return sTemp;
}

void SetUpgradesNodes(ref shTo, ref shTo2, bool transfer)
{
	string Up = "";
	string Up2 = "";
	aref aroot, arcur;
	int i, num;
	int j = 0
	int z = 3;
	if (CheckAttribute(shTo,"QuestShip")) z = 6;
	else if (CheckAttribute(shTo,"Blueprint")) z = 6;	// Gregg: чертёжные корабли — 6 слотов
	if (CheckAttribute(shTo,"Upgrade"))
	{
		makearef(aroot,shTo.Upgrade);
		num = GetAttributesNum(aroot);
		for (i = 0; i < num; i++)
		{
			arcur = GetAttributeN(aroot,i);
			Up = GetAttributeName(arcur);
			j = i+1;
			SetNewGroupPicture("SHIPSUP_"+j, "SHIP_UPGRADES_MAX", GetConvertStr(Up+"_ICON","ShipsUpgrades.txt")+"On");
			SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_"+j, 4, argb(255,128,128,128));
			Up2 = "SHIPSUP_"+j;
			GameInterface.Upgrade.(Up2) = Up;
		}
	}
	for (i = j+1; i <= 6; i++)
	{
		SetNewGroupPicture("SHIPSUP_"+i, "SHIP_UPGRADES_MAX", "Dummy");
		if (i <= z) SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_"+i, 4, argb(255,128,128,128));
		else SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_"+i, 4,argb(255,55,55,55));
	}
	if (transfer)
	{
		j = 0;
		z = 3;
		if (CheckAttribute(shTo2,"QuestShip")) z = 6;
		else if (CheckAttribute(shTo2,"Blueprint")) z = 6;	// Gregg: чертёжные корабли — 6 слотов
		if (CheckAttribute(shTo2,"Upgrade"))
		{
			makearef(aroot,shTo2.Upgrade);
			num = GetAttributesNum(aroot);
			for (i = 0; i < num; i++)
			{
				arcur = GetAttributeN(aroot,i);
				Up = GetAttributeName(arcur);
				j = i+1;
				SetNewGroupPicture("SHIPSUP2_"+j, "SHIP_UPGRADES_MAX", GetConvertStr(Up+"_ICON","ShipsUpgrades.txt")+"On");
				SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP2_"+j, 4, argb(255,128,128,128));
				Up2 = "SHIPSUP2_"+j;
				GameInterface.Upgrade.(Up2) = Up;
			}
		}
		for (i = j+1; i <= 6; i++)
		{
			SetNewGroupPicture("SHIPSUP2_"+i, "SHIP_UPGRADES_MAX", "Dummy");
			if (i <= z) SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP2_"+i, 4, argb(255,128,128,128));
			else SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP2_"+i, 4,argb(255,55,55,55));
			
		}
	}
}

void DisableUpgradeNodes(ref shTo, ref shTo2, bool transfer);
{
	DeleteAttribute(&GameInterface,"Upgrade");
	int i = 1;
	int z = 3;
	if (CheckAttribute(shTo,"QuestShip")) z = 6;
	else if (CheckAttribute(shTo,"Blueprint")) z = 6;	// Gregg: чертёжные корабли — 6 слотов
	for (i = 1; i <= 6; i++)
	{
		SetNewGroupPicture("SHIPSUP_"+i, "SHIP_UPGRADES_MAX", "Dummy");
		if (i > z) SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_"+i, 4,argb(255,55,55,55));
		else SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_"+i, 4, argb(255,128,128,128));
	}
	if (transfer) 
	{
		z = 3;
		if (CheckAttribute(shTo2,"QuestShip")) z = 6;
		else if (CheckAttribute(shTo2,"Blueprint")) z = 6;	// Gregg: чертёжные корабли — 6 слотов
		for (i = 1; i <= 6; i++)
		{
			SetNewGroupPicture("SHIPSUP2_"+i, "SHIP_UPGRADES_MAX", "Dummy");
			if (i > z) SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP2_"+i, 4,argb(255,55,55,55));
			else SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP2_"+i, 4, argb(255,128,128,128));
		}
	}
}

void SetUpgradeTooltip(string upname)
{
	if (CheckAttribute(GameInterface,"Upgrade."+upname)) upname = GameInterface.Upgrade.(upname);
	else return;
	string sHeader = XI_ConvertString("UpgradeShip");
	CreateTooltip("#" + sHeader, GetConvertStr(upname+"_DESCR","ShipsUpgrades.txt"), argb(255,255,255,255), "", argb(255,255,255,255), "", argb(255,192,255,192), "", argb(255,255,255,255), "", "SHIP_UPGRADES_MAX", GetConvertStr(upname+"_ICON","ShipsUpgrades.txt")+"On", 128, 128);
}
/*!
 * \brief Возвращает иконку шрифта специальности исходя из роли.
 * Для основных персонажей (трактирщик, баникр и т.п.) не имеют специальности,
 * а она необходима для отрисовки имени над головой.
 * \warning для персонажей, специализация которых определяется PerksChars()
 * присутствует рандомная генерация после каждой загрузки локации.
 * Лучше прибить их гвоздями или проработать одинаковую генерацию/запись при генерации
 * \param id идентификатор персонажа
 * \return string спецсимвол, определяющий иконку специализации
 */
string GetIconFromFontFromIdMainChr(string id)
{
	if (hassubstr(id, "_Mayor"))
	{
		return GetIconFromFont(PerksChars());
	}
	else if (hassubstr(id, "_Priest"))
	{
		return GetIconFromFont(PerksChars());
	}
	else if (hassubstr(id, "_waitress"))
	{
		return GetIconFromFont(PerksChars());
	}
	else if (hassubstr(id, "_tavernkeeper"))
	{
		return GetIconFromFont(PerksChars());
	}
	else if (hassubstr(id, "_trader"))
	{
		return GetIconFromFont("Trader");
	}
	else if (hassubstr(id, "_shipyarder"))
	{
		return GetIconFromFont("SeaWolf");
	}
	else if (hassubstr(id, "_usurer"))
	{
		return GetIconFromFont("Trader");
	}
	else if (hassubstr(id, "_PortMan"))
	{
		return GetIconFromFont("SeaWolf");
	}
	else if (hassubstr(id, "_Hostess"))
	{
		return GetIconFromFont("Agent");
	}
	else if (hassubstr(id, "_Smuggler"))
	{
		return GetIconFromFont("Agent");
	}
	return "";
}
