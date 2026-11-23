

#define SAVEIMAGE_UPDATE_TIME	0

bool bThisSave;
bool bIsGameProcessNow;
aref scrshot;
object emptyscrshot;

int g_nLablesFileID = -1;
string currentProfile;

int g_nCurrentSaveIndex = 0;
int g_nFirstSaveIndex = -1;
int g_nSaveQuantity = 0;

// LEO: список записей - ОДНА колонка: строка = миниатюра плюс подпись в три строки, справа
//   карточка выбранной записи с крупным превью. Последняя строка лежит НИЖЕ области списка
//   и служит буфером плавной прокрутки: она выезжает снизу уже заполненной.
#define SLOTS_IN_LINE			1  // записей в строке списка
#define SAVE_LINES				12 // всего строк: 11 видимых + строка-буфер
#define MAX_SAVE_SLOTS			12 // SLOTS_IN_LINE * SAVE_LINES
// сколько записей держит кэш снимков в движке (SS_CACHE_MAX в scr_shoter.cpp) - дальше он вытесняет
// старые, и прогревать смысла нет. Снимки теперь 1280x720, поэтому кэш там короткий.
#define SAVE_PRECACHE_MAX		16
#define SAVE_LOAD_PER_TICK		3  // строк показываем за заход
#define SAVE_PRECACHE_PER_TICK	4  // записей прогреваем в кэш за заход

object g_oSaveList[MAX_SAVE_SLOTS];
object g_oSaveContainer;

bool isMainMenuChecker = false;

void InitInterface_BB(string iniName, bool isSave, bool isMainMenu)
{
	if( CheckAttribute(&PlayerProfile,"name") ) {
		PlayerProfile.old_name = PlayerProfile.name;
	}

	isMainMenuChecker = isMainMenu;
	bThisSave = isSave;
	bIsGameProcessNow = true;
	if(bThisSave) GameInterface.title = "titleSave";
	else GameInterface.title = "titleLoad";

	g_nLablesFileID = LanguageOpenFile("LocLables.txt");
	g_nInterfaceFileID = LanguageOpenFile("interface_strings.txt");

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	if( bThisSave ) {
		SetSelectable("BTN_PROFILE",false);
		SendMessage( &GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"BTN_SAVELOAD", 0, "Save" );
	}

	// by default first save is selected
	SendMessage( &GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, "FRAME1", 0, argb(255,255,210,120) );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVETXT1", 8, -1, argb(255,255,255,255) );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVELOC1", 8, -1, argb(255,255,255,255) );


	FillProfileList();
	FindScrshotClass();
	InitSaveObjList();

	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("eventBtnAction","procBtnAction",0);
	SetEventHandler("eventSelecterActivate","procSelecterActivate",0);
	SetEventHandler("eventSelecterMove","procSelecterMove",0);
	SetEventHandler("eventSaveClick","procSaveClick",0);
	SetEventHandler("evLoadOneSaveInfo","procLoadOneSaveInfo",0);
	SetEventHandler("eProfileBtnAction","procProfileBtnAction",0);
	SetEventHandler("eventConfirm","procConfirm",0);
	SetEventHandler("RibbonTopChange","RibbonTopChange",0);
	SetEventHandler("LoadProfile","LoadProfile",0);
	SetEventHandler("SaveLoad","SaveLoad",0);
	SetEventHandler("eventSaveCustom","ProcessCustomSaveAction",0);
	SetEventHandler("CheckButtonChange", "CheckButtonChange", 0);

	PostEvent( "evLoadOneSaveInfo",1 );

	if(isSave == false && isMainMenu == true && sti(PlayerProfile.profilesQuantity) > 1)
	{
		ProcChooseProfileFromList();
	}
	else
	{
		SetCurrentProfile( GetCurrentProfile() );
	}


}

void SetCurrentProfile( string sProfileName )
{
	currentProfile = sProfileName;
	PlayerProfile.name = sProfileName;
	DeleteAttribute( &g_oSaveContainer,"" );
	int nSaveNum = 0;
	string saveName;
	int nSaveSize;
	string attr;
	GameInterface.SavePath = "SAVE\" + currentProfile;
	// LEO: список собираем сразу по выбранному фильтру (Все / Ручные / Быстрые / Авто)
	int nFilter = GetSaveFilter();
	ShowSaveFilter(nFilter);
	int file_cnt = 0;
	while( SendMessage(&GameInterface,"llee",MSG_INTERFACE_SAVE_FILE_FIND,file_cnt,&saveName,&nSaveSize)!=0 )
	{
		if( IsSaveInFilter(saveName, nFilter) )
		{
			attr = "s" + nSaveNum;
			g_oSaveContainer.(attr).savename = saveName;
			g_oSaveContainer.(attr).savesize = nSaveSize;
			nSaveNum++;
		}
		file_cnt++;
	}
	g_oSaveContainer.listsize = nSaveNum;
	g_nSaveQuantity = nSaveNum;
	g_nFirstSaveIndex = -1;
	FillSaveList( (g_nCurrentSaveIndex/MAX_SAVE_SLOTS) * MAX_SAVE_SLOTS );
	if( bThisSave ) {
		setInitSelection( g_nSaveQuantity );
	} else {
		setInitSelection( 0 );
	}
	UpdateSaveRibbonRows();
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVEINFO", 1, 1, "#"+XI_ConvertString("ProfileName")+": " + currentProfile );
	LoadGameOptions();
}

void setInitSelection(int _sel)
{
	if( _sel < 0 || _sel > g_nSaveQuantity ) {
		_sel = 0;
	}

	// LEO: ряд нужного слота подводим ЛЕНТОЙ (раньше данные подменялись под неподвижным списком -
	//   с плавной прокруткой это давало рассинхрон картинки и содержимого). При сохранении сюда
	//   приходит g_nSaveQuantity, то есть ПЕРВЫЙ ПУСТОЙ слот.
	int nVisibleLines = SAVE_LINES - 1;
	int nLine = _sel / SLOTS_IN_LINE;
	int nTop = nLine - nVisibleLines + 1;
	if( nTop < 0 ) nTop = 0;

	SetRibbonTop(nTop);
	FillSaveList(nTop * SLOTS_IN_LINE);

	int imgIdx = _sel - nTop * SLOTS_IN_LINE + 1;
	if( imgIdx < 1 ) imgIdx = 1;
	if( imgIdx > MAX_SAVE_SLOTS ) imgIdx = MAX_SAVE_SLOTS;
	if( imgIdx != 1 ) {
		SetSelecting(0, false);
	}
	PostEvent("eventSaveClick", 100, "ls", 0, "SAVEIMG" + imgIdx);
}

void ProcessCancelExit()
{
	DelEventHandler("RibbonTopChange","RibbonTopChange");
	DelEventHandler("eventConfirm","procConfirm");
	DelEventHandler("evLoadOneSaveInfo","procLoadOneSaveInfo");
	DelEventHandler("eProfileBtnAction","procProfileBtnAction");
	DelEventHandler("eventSaveClick","procSaveClick");
	DelEventHandler("eventSelecterActivate","procSelecterActivate");
	DelEventHandler("eventSelecterMove","procSelecterMove");
	DelEventHandler("eventBtnAction","procBtnAction");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("LoadProfile","LoadProfile");
	DelEventHandler("SaveLoad","SaveLoad");
	DelEventHandler("eventSaveCustom","ProcessCustomSaveAction");
	DelEventHandler("CheckButtonChange", "CheckButtonChange");

	// LEO: список записей держит и разобранные описания (ветка parsed на каждую запись) - при выходе
	//   он ни к чему, а при следующем открытии всё равно строится заново.
	DeleteAttribute( &g_oSaveContainer, "" );

	if( CheckAttribute(&PlayerProfile,"old_name") ) {
		PlayerProfile.name = PlayerProfile.old_name;
	}

	if( CheckAttribute(&InterfaceStates,"showGameMenuOnExit") && sti(InterfaceStates.showGameMenuOnExit) == true)
	{
		IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU);
		return;
	}

	IDoExit(RC_INTERFACE_SAVELOAD_EXIT);

	if( !CheckAttribute(&InterfaceStates,"InstantExit") || sti(InterfaceStates.InstantExit)==false )
	{
		ReturnToMainMenu();
	}
}

void IDoExit(int exitCode)
{
	DeleteAttribute(&PlayerProfile,"old_name");

	GameInterface.SavePath = "SAVE";
	LanguageCloseFile(g_nLablesFileID);
	LanguageCloseFile(g_nInterfaceFileID);

    interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"InstantExit") && sti(InterfaceStates.InstantExit)==true ) {
		EndCancelInterface(false);
	} else {
		EndCancelInterface(true);
	}
}

void FindScrshotClass()
{
	string layerName;
	if(bSeaActive && !bAbordageStarted) layerName = SEA_REALIZE;
	else layerName = "realize";

	if( !GetEntity(&scrshot, "scrshoter") ) {
		makearef(scrshot,emptyscrshot);
	}
}

string GetCurrentProfile()
{
	if( CheckAttribute(&PlayerProfile, "name") && PlayerProfile.name!="" ) {
		return PlayerProfile.name;
	}
	trace("error (save_load.c) : PlayerProfile.name attribute not found.");

	object FolderList;
	XI_FindFoldersWithoutNetsave("SAVE\*", &FolderList);

	int num = GetAttributesNum(&FolderList);
	if( num > 0 )
		return GetAttributeValue( GetAttributeN(&FolderList, num - 1) );
	return "";
}

void FillProfileList()
{
	PlayerProfile.profilesQuantity = 0;

	object FolderList;
	XI_FindFoldersWithoutNetsave("SAVE\*", &FolderList);

	int num = GetAttributesNum(&FolderList);
	string folderName;

	int i = 0;
	string attr;
	for(i=0; i<num; i++) {
		attr = "profile_" + i;
		PlayerProfile.(attr) = GetAttributeValue( GetAttributeN(&FolderList,i) );
	}

	PlayerProfile.profilesQuantity = num;
}

void procSelecterActivate()
{
	SaveLoadCurrentIntoSlot();
}

void procBtnAction()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( iComIndex == ACTION_RIGHTSTEP ) {
		if( sNodName == "BTN_PROFILE" ) {
			if( GetSelectable("BTN_SAVELOAD") ) {
				SetCurrentNode("BTN_SAVELOAD");
			} else {
				sNodName = "BTN_SAVELOAD";
			}
		}
		if( sNodName == "BTN_SAVELOAD" ) {
			if( GetSelectable("BTN_DELETE") ) {
				SetCurrentNode("BTN_DELETE");
			} else {
				sNodName = "BTN_DELETE";
			}
		}
		if( sNodName == "BTN_DELETE" ) {
			SetCurrentNode("BTN_EXIT");
		}
		return;
	}

	if( iComIndex == ACTION_LEFTSTEP ) {
		if( sNodName == "BTN_EXIT" ) {
			if( GetSelectable("BTN_DELETE") ) {
				SetCurrentNode("BTN_DELETE");
			} else {
				sNodName = "BTN_DELETE";
			}
		}
		if( sNodName == "BTN_DELETE" ) {
			if( GetSelectable("BTN_SAVELOAD") ) {
				SetCurrentNode("BTN_SAVELOAD");
			} else {
				sNodName = "BTN_SAVELOAD";
			}
		}
		if( sNodName == "BTN_SAVELOAD" && !bThisSave ) {
			SetCurrentNode("BTN_PROFILE");
		}
		return;
	}

	if( iComIndex == ACTION_ACTIVATE || iComIndex == ACTION_MOUSECLICK ) {
		if( sNodName == "BTN_PROFILE" ) {
			ProcChooseProfileFromList();
		}
		if( sNodName == "BTN_SAVELOAD" ) {
			SaveLoadCurrentIntoSlot();
		}
		if( sNodName == "BTN_DELETE" ) {
			DoConfirm(CONFIRMMODE_SAVE_DELETE);
		}
	}
}

void SaveLoadCurrentIntoSlot()
{
	if( bThisSave ) {
		// запись
		if( GetCurSaveName()=="" ) { // не перезапись старого файла
			ProcessSave();
		} else {
			DoConfirm(CONFIRMMODE_SAVE_OVERWRITE);
		}
	} else {
		// чтение
		if( g_nCurrentSaveIndex<0 || g_nCurrentSaveIndex>=g_nSaveQuantity ) { // нет такой ¤чейки с записью
			return;
		}
		if( bIsGameProcessNow ) { // в данный момент уже идёт игра?
			DoConfirm(CONFIRMMODE_LOAD_GAME);
		} else {
			ProcessLoad();
		}
	}
}

void ProcChooseProfileFromList()
{
	// disable all windows
	XI_WindowDisable( "MAIN_WINDOW", true );
	// enable profile window
	XI_WindowDisable( "PROFILE_WINDOW", false );
	XI_WindowShow( "PROFILE_WINDOW", true );
	// set current node to profile list
	FillProfileListIntoTable();
	SetCurrentNode( "PROFILE_WINDOW_LIST" );
}

void FillProfileListIntoTable()
{
	// Fill Profile List
	int n, q, nSel;
	string src_attr, dst_attr, sCurProfile;
	q = sti(PlayerProfile.profilesQuantity);
	nSel = 1;
	if( CheckAttribute(&PlayerProfile,"name") ) {
		sCurProfile = PlayerProfile.name;
	} else {
		sCurProfile = "";
	}
	DeleteAttribute( &GameInterface, "profile_window_list" );
	for( n=0; n<q; n++ )
	{
		src_attr = "profile_" + n;
		dst_attr = "tr" + (n+1);
		GameInterface.profile_window_list.(dst_attr).td1.str = PlayerProfile.(src_attr);
		if( PlayerProfile.(src_attr) == sCurProfile ) {nSel = n+1;}
	}
	GameInterface.profile_window_list.select = nSel;
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "PROFILE_WINDOW_LIST", 0 );
}

void ProcExitProfile()
{
	// disable all windows
	XI_WindowShow( "PROFILE_WINDOW", false );
	// enable main window
	XI_WindowDisable( "MAIN_WINDOW", false );
	// set current node to button for start profile choosing
	SetCurrentNode( "BTN_PROFILE" );
}

void ProcChooseProfile()
{
	string attr = "profile_" + (sti(GameInterface.profile_window_list.select)-1);
	if( CheckAttribute(&PlayerProfile,attr) ) {
		SetCurrentProfile( PlayerProfile.(attr) );
	}

	ProcExitProfile();
}

void ProcDeleteProfile()
{
	int nProfileIdx = sti(GameInterface.profile_window_list.select) - 1;
	// deleting process:
	string attr = "profile_" + nProfileIdx;
	string sThisProfile = PlayerProfile.(attr);
	DeleteProfile( PlayerProfile.(attr) );

	// shifting profile list
	int nProfilesQ = sti(PlayerProfile.profilesQuantity);
	nProfilesQ--;
	string attrSrc, attrDst;
	for( int n=nProfileIdx; n<nProfilesQ; n++ )
	{
		attrDst = "profile_" + n;
		attrSrc = "profile_" + (n+1);
		PlayerProfile.(attrDst) = PlayerProfile.(attrSrc);
	}
	DeleteAttribute( &PlayerProfile, "profile_"+nProfilesQ );
	PlayerProfile.profilesQuantity = nProfilesQ;
	FillProfileListIntoTable();

	if( sThisProfile == currentProfile )
	{
		SetCurrentProfile("");
	}
}

void DeleteProfile(string profileName)
{
	string oldpath = "";

	// Warship 08.07.09 fix - ошибка езка об отсутствии атрибута
	if(CheckAttribute(GameInterface, "SavePath"))
	{
		oldpath = GameInterface.SavePath;
	}
    else
        oldpath = "SAVE";
	string sProfilePath = "SAVE\" + profileName;
	GameInterface.SavePath = sProfilePath;
	// deleting all files from profile folder
	int nSaveNum= 0;
	string saveName;
	int nSaveSize;
	while( SendMessage(&GameInterface,"llee",MSG_INTERFACE_SAVE_FILE_FIND,nSaveNum,&saveName,&nSaveSize)!=0 )
	{
		nSaveNum++;
		SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,saveName);
	}
	// LEO: путь сбрасываем ДО сноса каталога, и сносим по строке, а не по атрибуту SavePath.
	//   Движок заводит папку SavePath заново при первом же поиске сейвов, поэтому от удалённого
	//   профиля оставалась пустая директория.
	if( oldpath == sProfilePath ) oldpath = "SAVE";
	GameInterface.SavePath = oldpath;
	XI_DeleteFolder( sProfilePath );
	// LEO: вместе с профилем уходят и его снимки рангов для зала славы - иначе папка копилась бы
	//   мёртвым грузом, а новый профиль с тем же именем получил бы чужие кадры на стендах.
	XI_DeleteFolder( FinalGame_GetProfileShotsDir(profileName) );
}

void procSelecterMove()
{
	int iComIndex = GetEventData();

	int nLeft,nTop,nRight,nBottom;
	bool bMakeMove = false;

	switch( iComIndex )
	{
	case ACTION_RIGHTSTEP:
		bMakeMove = GetMoveToOtherSave( g_nCurrentSaveIndex+1, &nLeft,&nTop,&nRight,&nBottom );
	break;
	case ACTION_LEFTSTEP:
		bMakeMove = GetMoveToOtherSave( g_nCurrentSaveIndex-1, &nLeft,&nTop,&nRight,&nBottom );
	break;
	case ACTION_UPSTEP:
		bMakeMove = GetMoveToOtherSave( g_nCurrentSaveIndex-SLOTS_IN_LINE, &nLeft,&nTop,&nRight,&nBottom );
	break;
	case ACTION_DOWNSTEP:
		bMakeMove = GetMoveToOtherSave( g_nCurrentSaveIndex+SLOTS_IN_LINE, &nLeft,&nTop,&nRight,&nBottom );
	break;
	}

	if( bMakeMove ) {
		SendMessage( &GameInterface, "lslllll", MSG_INTERFACE_MSG_TO_NODE,"SAVE_SELECTER",0, nLeft,nTop,nRight,nBottom );
	}
}

// LEO: какая ячейка сейчас подсвечена - номер держим, чтобы не гадать по состоянию узлов.
int g_nHiliteSlot = -1;

// LEO: после перезаливки слоты показывают ДРУГИЕ записи, поэтому подсветку пересчитываем от
//   выбранного сохранения. Гасим ВСЕ строки и зажигаем нужную заново, не полагаясь на то, где
//   рамка горела раньше: перенос строки цвет не переносит, и состояние разъезжалось - то рамок было
//   две, то не появлялось ни одной. Это не кадр прокрутки, двадцать смен цвета тут не в тягость.
void RefreshHilite()
{
	int nSlot = g_nCurrentSaveIndex - g_nFirstSaveIndex;
	if( nSlot < 0 || nSlot >= MAX_SAVE_SLOTS ) nSlot = -1;

	for( int i=0; i<MAX_SAVE_SLOTS; i++ ) {
		SetSelecting(i, i == nSlot);
	}
	g_nHiliteSlot = nSlot;
}

void ClearAllSelecting()
{
	for( int i=0; i<MAX_SAVE_SLOTS; i++ ) {
		SetSelecting(i, false);
	}
	g_nHiliteSlot = -1;
}

void SetSelecting(int nSlot,bool bSelect)
{
	string sNodeName;
	int nColor, nFrameColor;
	sNodeName = "SAVEIMG"+(nSlot+1);
	if( bSelect ) {
		nColor = argb(255,255,255,255);
		nFrameColor = argb(255,255,210,120);   // золотистая рамка выбранной строки
	} else {
		nColor = argb(255,178,178,178);   // как в ini: до первого клика строки красит именно он
		nFrameColor = argb(255,128,128,128);   // дефолт FRAME из defaultnode.ini
	}
	// LEO: выделение - ЯРКАЯ РАМКА строки и белая подпись. Раньше выбранное превью мигало само
	//   (blind у PICTURE): это читалось как грязное коричневое пятно.
	SendMessage( &GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, "FRAME"+(nSlot+1), 0, nFrameColor );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVETXT"+(nSlot+1), 8, -1, nColor );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVELOC"+(nSlot+1), 8, -1, nColor );
}

bool GetMoveToOtherSave( int nNewSaveIndex, ref rLeft, ref rTop, ref rRight, ref rBottom )
{
	if( nNewSaveIndex < 0 ) return false;
	if( g_nCurrentSaveIndex == nNewSaveIndex ) return false;

	int nMaxQ = (g_nSaveQuantity + makeint(SLOTS_IN_LINE-1))/ SLOTS_IN_LINE * SLOTS_IN_LINE + SLOTS_IN_LINE;
	if( nNewSaveIndex > nMaxQ ) return false;

	int nOldIdx = g_nCurrentSaveIndex - g_nFirstSaveIndex;
	int nNewIdx = nNewSaveIndex - g_nFirstSaveIndex;

	// LEO: ушли стрелками за край видимой части - листаем ЛЕНТОЙ, данные подтянутся за ней
	if( nNewIdx < 0 ) {
		if( g_nFirstSaveIndex > 0 ) {
			SetRibbonTop(g_nFirstSaveIndex / SLOTS_IN_LINE - 1);
			FillSaveList(g_nFirstSaveIndex - SLOTS_IN_LINE);
		}
		nNewIdx = nNewSaveIndex - g_nFirstSaveIndex;
	}
	if( nNewIdx > makeint(SLOTS_IN_LINE*(SAVE_LINES-1)-1) ) {
		SetRibbonTop(g_nFirstSaveIndex / SLOTS_IN_LINE + 1);
		FillSaveList(g_nFirstSaveIndex + SLOTS_IN_LINE);
		nNewIdx = nNewSaveIndex - g_nFirstSaveIndex;
	}

	bool bEmptySlot = true;
	if( nNewIdx >= 0 && nNewIdx < MAX_SAVE_SLOTS ) {
		if( bThisSave ) bEmptySlot = false;                              // при записи годится и пустая
		if( sti(g_oSaveList[nNewIdx].saveidx) >= 0 ) bEmptySlot = false; // за строкой есть запись
	}
	if( nNewIdx>0 && bEmptySlot ) {
		nNewIdx = g_nCurrentSaveIndex - g_nFirstSaveIndex;
		nNewSaveIndex = g_nCurrentSaveIndex;
		if( nNewIdx == nOldIdx ) {
			ReloadSaveInfo();
			return false; // нельзя перейти на новый элемент (на первый всегда можно)
		}
	}

	g_nCurrentSaveIndex = nNewSaveIndex;
	RefreshHilite();
	ReloadSaveInfo();
	
	if(g_nSaveQuantity > SLOTS_IN_LINE)
	{	
		int nLineQ = g_nSaveQuantity / SLOTS_IN_LINE;
		int nLineCur = (g_nCurrentSaveIndex + 1) / SLOTS_IN_LINE;
		// LEO: ползунок ведёт сама лента (ownedControl) - руками его двигать нельзя, иначе он спорит с ней
	}

	int nLeft = 0;
	int nTop = 0;
	int nRight = 0;
	int nBottom = 0;

	rLeft = nLeft;
	rTop = nTop;
	rRight = nRight;
	rBottom = nBottom;
	return true;
}

void procSaveClick()
{
	int iComIdx = GetEventData();
	string sNodName = GetEventData();

	// LEO: номер строки берём из имени узла - раньше тут стояла лесенка из сорока сравнений,
	//   и любая правка сетки означала переписывание всей лесенки. Кликабельна ВСЯ строка,
	//   поэтому имя приходит от миниатюры, подписи или рамки - отрезаем известный префикс.
	int nCut = -1;
	if( findSubStr(sNodName, "SAVEIMG", 0) == 0 ) nCut = 7;
	if( findSubStr(sNodName, "SAVETXT", 0) == 0 ) nCut = 7;
	if( findSubStr(sNodName, "SAVELOC", 0) == 0 ) nCut = 7;
	if( findSubStr(sNodName, "FRAME", 0) == 0 ) nCut = 5;
	if( nCut < 0 ) return;
	int nSlot = sti(strcut(sNodName, nCut, strlen(sNodName)-1)) - 1;
	if( nSlot < 0 || nSlot >= MAX_SAVE_SLOTS ) return;
	SelectSaveImage( g_nFirstSaveIndex + nSlot );
}

void SelectSaveImage( int nSaveIndex )
{
	int nLeft,nTop,nRight,nBottom;
	bool bMakeMove = GetMoveToOtherSave( nSaveIndex, &nLeft,&nTop,&nRight,&nBottom );
	if( bMakeMove ) {
		SendMessage( &GameInterface, "lslllll", MSG_INTERFACE_MSG_TO_NODE,"SAVE_SELECTER",0, nLeft,nTop,nRight,nBottom );
	}
	SetCurrentNode( "SAVE_SELECTER" );
}

// LEO: лента прокручивается по СТРОКАМ-ДАННЫМ, поэтому держим их ровно столько, сколько записей.
//   Содержимое строк пустое: строку рисуют узлы, привязанные местом (slotnodes).
void SetRibbonTop(int nTop)
{
	if( nTop < 0 ) nTop = 0;
	GameInterface.SAVE_RIBBON.top = nTop;
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "SAVE_RIBBON", 0 ); // перезалить строки
}

void UpdateSaveRibbonRows()
{
	// LEO: за последней записью всегда держим пустую строку - в режиме записи туда сохраняются
	//   новые игры, в режиме чтения он просто закрывает хвост списка, как в прежнем окне.
	int nRows = (g_nSaveQuantity + SLOTS_IN_LINE - 1) / SLOTS_IN_LINE + 1;
	if( nRows < 1 ) nRows = 1;

	string sRow;
	Table_Clear("SAVE_RIBBON", false, true, false);
	for( int i=1; i<=nRows; i++ ) {
		sRow = "tr" + i;   // имя строки - только через переменную: выражение прямо в .() Storm не берёт
		GameInterface.SAVE_RIBBON.(sRow).td1.str = "";
	}
	Table_UpdateWindow("SAVE_RIBBON");
	SetClickable("SAVESCROLL", nRows > SAVE_LINES-1);
}

void FillSaveList(int nFirstSaveIndex)
{
	if( nFirstSaveIndex<0 ) nFirstSaveIndex = 0;
	nFirstSaveIndex = nFirstSaveIndex / SLOTS_IN_LINE;
	nFirstSaveIndex = nFirstSaveIndex * SLOTS_IN_LINE;

	bool bNoRebuildSaveList = (g_nFirstSaveIndex>=0);
	int nDelta = nFirstSaveIndex - g_nFirstSaveIndex;
	g_nFirstSaveIndex = nFirstSaveIndex;
	int i;

	// LEO: шаг ровно на ряд (а при плавной прокрутке он всегда такой) - переносим уже загруженные
	//   ряды и догружаем только новый край. Раньше на каждую величину сдвига была своя ветка с
	//   ручным перечислением линий, и добавление ряда требовало править их все.
	if( bNoRebuildSaveList && nDelta==SLOTS_IN_LINE )
	{
		for( i=1; i<SAVE_LINES; i++ ) {
			MoveSaveLine(i, i-1);
		}
		FillSaveLine(SAVE_LINES-1, nFirstSaveIndex + (SAVE_LINES-1)*SLOTS_IN_LINE);
		RefreshHilite();
		// LEO: правую панель (портреты офицеров, корабль, флаг) при прокрутке НЕ трогаем - выбранное
		//   сохранение не меняется, а перерисовка двух десятков узлов на каждый ряд била по кадру.
		return;
	}
	if( bNoRebuildSaveList && nDelta==-SLOTS_IN_LINE )
	{
		for( i=SAVE_LINES-1; i>0; i-- ) {
			MoveSaveLine(i-1, i);
		}
		FillSaveLine(0, nFirstSaveIndex);
		RefreshHilite();
		return;
	}

	// прыжок ползунком или открытие окна - заливаем все ряды
	for( i=0; i<SAVE_LINES; i++ ) {
		FillSaveLine(i, nFirstSaveIndex + i*SLOTS_IN_LINE);
	}
	RefreshHilite();
	ReloadSaveInfo();
}

void FillSaveLine(int nLine, int nBegIndex)
{
	int nBeg = nLine * SLOTS_IN_LINE;
	int i,n;
	string attr;
	g_nSaveQuantity;
	for(i=0; i<SLOTS_IN_LINE; i++)
	{
		n = nBegIndex + i;
		attr = "s" + n;
		if( CheckAttribute(&g_oSaveContainer,attr) ) {
			LoadInfo(nBeg+i, n, g_oSaveContainer.(attr).savename);
		} else {
			FillEmptySaveSlot(nBeg+i);
		}
	}
}

void FillEmptySaveSlot(int nSlot)
{
	ClearSaveInfoByIndex( nSlot );
	g_oSaveList[nSlot].saveidx = -1;
	g_oSaveList[nSlot].savefile = "";
	g_oSaveList[nSlot].loaded = 1;
	ShowDataForSave(nSlot,"empty",0,"");
}

void ShowDataForSave(int nSlot, string picname, int picpointer, string strdata)
{
	string nodname = "SAVEIMG" + (nSlot+1);
	// LEO: в режиме записи кликабельны все строки (включая пустые - туда и пишем), в режиме чтения
	//   те, за которыми стоит запись. Раньше флаг зависел от того, успел ли подгрузиться снимок, и
	//   при переносе строки приезжал от соседа - строка могла остаться невыбираемой навсегда.
	bool bClickable = bThisSave;
	if( sti(g_oSaveList[nSlot].saveidx) >= 0 ) bClickable = true;
	if( picname!="" ) {
		if( picname == "empty" ) {
			SendMessage( &GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, nodname, 2, 0,"interfaces\SaveIcons\SaveEmpty.dds" );
		}
		if( picname == "loading" ) {
			SendMessage( &GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, nodname, 2, 0,"interfaces\SaveIcons\SaveLoading.dds" );
		}
		if( picname == "corrupted" ) {
			SendMessage( &GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, nodname, 2, 0,"interfaces\SaveIcons\SaveCorrupted.dds" );
		}
	}
	if( picpointer!=0 ) {
		SendMessage( &GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, nodname, 7, picpointer );
		bClickable = true;
		// LEO: тот же снимок показывает карточка справа - запоминаем указатель в записи,
		//   чтобы не читать его с диска второй раз. Перенос строки тащит поле с собой.
		g_oSaveList[nSlot].texture = picpointer;
		if( g_nFirstSaveIndex + nSlot == g_nCurrentSaveIndex ) ShowBigPreview(nSlot);
	} else {
		if( picname == "empty" || picname == "corrupted" ) DeleteAttribute( &g_oSaveList[nSlot], "texture" );
	}
	SetSelectable( nodname, bClickable );

	string sSystemTimeString = "No Time";
	string sTimeAttr = "s" + g_oSaveList[nSlot].saveidx;   // "s-1" у пустой строки - атрибутов нет
	// LEO: время файла спрашиваем у системы ОДИН раз на запись и запоминаем: раньше это делалось при
	//   каждом показе строки, то есть по обращению к диску на каждую пройденную строку.
	if( CheckAttribute(&g_oSaveContainer, sTimeAttr + ".filetime") ) {
		sSystemTimeString = g_oSaveContainer.(sTimeAttr).filetime;
	}
	else {
		if( CheckAttribute(&g_oSaveList[nSlot], "savefile") && g_oSaveList[nSlot].savefile != "" ) {
			string fileSystemTime = "";
			string fileSystemDate = "";
			SendMessage(&GameInterface,"lsee",MSG_INTERFACE_GETTIME, "SAVE\"+currentProfile+"\"+g_oSaveList[nSlot].savefile, &fileSystemTime, &fileSystemDate);
			sSystemTimeString = GetSystemTimeString( fileSystemTime, fileSystemDate );
			if( CheckAttribute(&g_oSaveContainer, sTimeAttr) ) {
				g_oSaveContainer.(sTimeAttr).filetime = sSystemTimeString;
			}
		}
	}
	string sCapLoc = "Unknown";
	string sCapTime = "No Time";
	// LEO: запись уже разобрана при прогреве - берём готовое, тяжёлый ParseSaveData не зовём.
	//   ⚠️ Поля переносим ПОИМЁННО: CopyAttributes чистит приёмник, и служебные saveidx/savefile/
	//   loaded слетали - слот после этого считался пустым и показывал «битый диск».
	bool bFromCache = false;
	if( CheckAttribute(&g_oSaveContainer, sTimeAttr + ".parsed") ) {
		aref arParsed;
		makearef(arParsed, g_oSaveContainer.(sTimeAttr).parsed);
		sCapLoc = arParsed.locName;
		sCapTime = arParsed.timeStr;
		g_oSaveList[nSlot].locName = arParsed.locName;
		g_oSaveList[nSlot].timeStr = arParsed.timeStr;
		g_oSaveList[nSlot].faceinfo = arParsed.faceinfo;
		g_oSaveList[nSlot].playtime = arParsed.playtime;
		g_oSaveList[nSlot].curship = arParsed.curship;
		g_oSaveList[nSlot].nation = arParsed.nation;
		g_oSaveList[nSlot].difficulty = arParsed.difficulty;
		g_oSaveList[nSlot].money = arParsed.money;
		g_oSaveList[nSlot].rank = arParsed.rank;
		strdata = "";
		bFromCache = true;
	}

	if( strdata != "" )
	{
		string facestr, locName, timeStr, language, playtime, curship, nation, difficulty, money, rank;
		if( ParseSaveData(strdata, &facestr, &locName, &timeStr, &language, &playtime, &curship, &nation, &difficulty, &money, &rank) ) {
			sCapLoc = locName;
			sCapTime = timeStr;
			g_oSaveList[nSlot].locName = locName;
			g_oSaveList[nSlot].timeStr = timeStr;
			g_oSaveList[nSlot].faceinfo = PrepareMinifiedFaceString(facestr);
			g_oSaveList[nSlot].playtime = playtime;
			g_oSaveList[nSlot].curship = curship;
			g_oSaveList[nSlot].nation = nation;
			g_oSaveList[nSlot].difficulty = difficulty;
			g_oSaveList[nSlot].money = money;
			g_oSaveList[nSlot].rank = rank;
		} else {
			g_oSaveList[nSlot].faceinfo = "";
			g_oSaveList[nSlot].playtime = "";
		}
	} else {
		if( !bFromCache ) {   // данные из кэша уже лежат в слоте - затирать их нечем и незачем
			g_oSaveList[nSlot].faceinfo = "";
			g_oSaveList[nSlot].playtime = "";
		}
	}
	SetSlotCaption(nSlot, sCapLoc, sCapTime, sSystemTimeString);

	if( (g_nCurrentSaveIndex-g_nFirstSaveIndex) == nSlot ) {
		ReloadSaveInfo();
	}
}

// LEO: до правки interface.c игровое время писалось в сейв через ДВА пробела - в уже записанных
//   строках это читается как дыра, поэтому схлопываем при показе.
string SqueezeSpaces(string sText)
{
	int nLen = strlen(sText);
	int nPos = findSubStr(sText, "  ", 0);
	while( nPos >= 0 )
	{
		string sTail = "";   // хвоста может и не быть: strcut за концом строки пишет ошибку в лог
		if( nPos+2 < nLen ) sTail = strcut(sText, nPos+2, nLen-1);
		sText = strcut(sText, 0, nPos) + sTail;
		nLen = strlen(sText);
		nPos = findSubStr(sText, "  ", 0);
	}
	return sText;
}

// LEO: подпись строки разнесена по двум узлам: даты стоят у миниатюры, название локации - справа
//   по центру ячейки. Раньше подписи списка жили одной коллекцией строк с абсолютными координатами,
//   и каждая правка сетки требовала пересчёта индексов вида nSlot*3+2. Части храним у слота: перенос
//   строки при прокрутке копирует их вместе с остальными данными.
void SetSlotCaption(int nSlot, string sLoc, string sGameTime, string sFileTime)
{
	sGameTime = SqueezeSpaces(sGameTime);
	g_oSaveList[nSlot].capLoc = sLoc;
	g_oSaveList[nSlot].capGame = sGameTime;
	g_oSaveList[nSlot].capFile = sFileTime;
	SetFormatedText("SAVETXT"+(nSlot+1), sGameTime + NewStr() + sFileTime);
	SetFormatedText("SAVELOC"+(nSlot+1), sLoc);
}

string GetSystemTimeString( string systemTime, string systemDate )
{
	// LEO: движок отдаёт время с секундами - в списке они лишние, режем до "чч:мм"
	if( strlen(systemTime) > 5 ) systemTime = strcut(systemTime, 0, 4);
	return systemTime + " " + systemDate;
}

void MoveSaveLine(int nSrcLine, int nDstLine)
{
	int nSrc = nSrcLine * SLOTS_IN_LINE;
	int nDst = nDstLine * SLOTS_IN_LINE;
	for( int i=0; i<SLOTS_IN_LINE; i++ )
	{
		MoveSaveInfo(nSrc+i,nDst+i);
	}
}

void InitSaveObjList()
{
	int i;
	for(i = 0; i< MAX_SAVE_SLOTS; i++)
	{
		g_oSaveList[i].saveidx = -1;
	}
}

int FindSaveInfoIndex(int iSaveIdx)
{
	int i;
	for(i = 0; i< MAX_SAVE_SLOTS; i++)
	{
		if( g_oSaveList[i].saveidx == iSaveIdx ) {
			return i;
		}
	}
	return -1;
}

void ClearSaveInfoByIndex(int i)
{
	if( g_oSaveList[i].saveidx == -1 ) {return;}
	DeleteAttribute( &g_oSaveList[i], "" );
	g_oSaveList[i].saveidx = -1;
}

void MoveSaveInfo(int nSrc, int nDst)
{
	CopyAttributes( &g_oSaveList[nDst], &g_oSaveList[nSrc] );
	DeleteAttribute( &g_oSaveList[nSrc], "" );
	g_oSaveList[nSrc].saveidx = -1;

	// copy info from src control to dst control
	string sDstImgNod = "SAVEIMG"+(nDst+1);
	string sSrcImgNod = "SAVEIMG"+(nSrc+1);
	// LEO: кликабельность ставим ПО ДАННЫМ приёмника, а не копируем у источника. В конце списка
	//   ряд-источник бывает пустым (флаг выключен), и приехавший на его место ряд с записями
	//   переставал отзываться на клик - ровно то «верхний ряд не кликается».
	bool bDstClickable = bThisSave;
	if( sti(g_oSaveList[nDst].saveidx) >= 0 ) bDstClickable = true;
	SetSelectable( sDstImgNod, bDstClickable );
	SendMessage( &GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, sDstImgNod, 8, sSrcImgNod );
	if( CheckAttribute(&g_oSaveList[nDst], "capLoc") ) {
		SetSlotCaption( nDst, g_oSaveList[nDst].capLoc, g_oSaveList[nDst].capGame, g_oSaveList[nDst].capFile );
	}
	// LEO: источник в «пусто» НЕ гасим - следующий перенос (или заливка нового края) перезапишет
	//   его тем же кадром, а лишняя установка текстуры на каждую из двадцати ячеек била по кадру.
}

void LoadInfo(int nInfoIdx, int nSaveIdx, string sSaveName)
{
	ClearSaveInfoByIndex( nInfoIdx );
	g_oSaveList[nInfoIdx].saveidx = nSaveIdx;
	g_oSaveList[nInfoIdx].savefile = sSaveName;
	g_oSaveList[nInfoIdx].loaded = 0;
	ShowDataForSave(nInfoIdx,"loading",0,"");
}

void procLoadOneSaveInfo()
{
	int nBudget = SAVE_LOAD_PER_TICK;
	int i;
	int pTex;
	string strdata;
	bool bYesScrShoter = IsEntity(scrshot);
	for(i = 0; i < MAX_SAVE_SLOTS; i++)
	{
		if( g_oSaveList[i].saveidx != "-1" )
		{
			if( !CheckAttribute(&g_oSaveList[i],"loaded") || g_oSaveList[i].loaded=="0" )
			{
				g_oSaveList[i].loaded = 1;
				strdata = "";
				if( bYesScrShoter ) {
					pTex = SendMessage( scrshot, "lsse", MSG_SCRSHOT_READ, "SAVE\" + currentProfile, g_oSaveList[i].savefile, &strdata );
				}
				if( strdata == "" ) {
					ShowDataForSave(i,"corrupted",0,"");
				} else {
					ShowDataForSave(i,"",pTex,strdata);
				}
				// LEO: за заход обрабатываем целый ряд, а не одну ячейку: раньше ряд наполнялся
				//   пять кадров подряд и было видно, как он догружается уже выехавшим. Снимок
				//   после показа НЕ отпускаем - он живёт в кэше движка (RELEASE там теперь значит
				//   «забыть запись» и шлётся только при удалении или перезаписи файла).
				nBudget--;
				if( nBudget <= 0 ) {
					PostEvent( "evLoadOneSaveInfo",SAVEIMAGE_UPDATE_TIME );
					return;
				}
			}
		}
	}

	// LEO: видимое загружено - фоном прогреваем ОСТАЛЬНЫЕ записи в кэш движка. Так весь список
	//   оказывается в памяти через секунду-другую после открытия окна, и прокрутка не лезет на
	//   диск вовсе. Разом при открытии грузить нельзя - на сотне сохранений это залипание.
	PrecacheNextSave();
	PostEvent( "evLoadOneSaveInfo",SAVEIMAGE_UPDATE_TIME );
}

void PrecacheNextSave()
{
	if( !IsEntity(scrshot) ) return;

	int nMax = g_nSaveQuantity;
	if( nMax > SAVE_PRECACHE_MAX ) nMax = SAVE_PRECACHE_MAX;

	string attr, strdata;
	int nBudget = SAVE_PRECACHE_PER_TICK;
	for( int i=0; i<nMax; i++ )
	{
		attr = "s" + i;
		if( !CheckAttribute(&g_oSaveContainer,attr) ) continue;
		if( CheckAttribute(&g_oSaveContainer,attr+".precached") ) continue;

		g_oSaveContainer.(attr).precached = 1;
		SendMessage( scrshot, "lsse", MSG_SCRSHOT_READ, "SAVE\" + currentProfile, g_oSaveContainer.(attr).savename, &strdata );
		// LEO: описание разбираем ЗДЕСЬ, один раз на запись, и складываем ВСЕ поля в неё. Показ
		//   ячейки при прокрутке потом ничего не парсит - просто копирует готовое.
		if( strdata != "" ) {
			string facestr, locName, timeStr, language, playtime, curship, nation, difficulty, money, rank;
			if( ParseSaveData(strdata, &facestr, &locName, &timeStr, &language, &playtime, &curship, &nation, &difficulty, &money, &rank) ) {
				g_oSaveContainer.(attr).parsed.locName = locName;
				g_oSaveContainer.(attr).parsed.timeStr = timeStr;
				g_oSaveContainer.(attr).parsed.faceinfo = PrepareMinifiedFaceString(facestr);
				g_oSaveContainer.(attr).parsed.playtime = playtime;
				g_oSaveContainer.(attr).parsed.curship = curship;
				g_oSaveContainer.(attr).parsed.nation = nation;
				g_oSaveContainer.(attr).parsed.difficulty = difficulty;
				g_oSaveContainer.(attr).parsed.money = money;
				g_oSaveContainer.(attr).parsed.rank = rank;
			}
		}
		nBudget--;
		if( nBudget <= 0 ) return;   // остальное дочитаем на следующих заходах
	}
}

void procProfileBtnAction()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( iComIndex == ACTION_ACTIVATE || iComIndex == ACTION_MOUSECLICK ) {
		if( sNodName == "PROFILE_WINDOW_BTN_EXIT" ) {
			if(isMainMenuChecker == true && g_nSaveQuantity == 0)
			{
				SetCurrentProfile("");
				ProcExitProfile();
			}
			else
			{
				ProcExitProfile();
			}
		}
		if( sNodName == "PROFILE_WINDOW_BTN_CHOOSE" ) {
			ProcChooseProfile();
		}
		if( sNodName == "PROFILE_WINDOW_BTN_DELETE" ) {
			DoConfirm( CONFIRMMODE_PROFILE_DELETE );
		}
		return;
	}
}

bool ParseSaveData(string fullSaveData, ref facestr, ref locationStr, ref timeStr, ref languageID, ref playtime, ref curship, ref nation, ref difficulty, ref money, ref rank)
{
	string lastStr;
	if( !GetNextSubStr(fullSaveData, locationStr, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, facestr, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, timeStr, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, playtime, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, curship, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, nation, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, difficulty, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, money, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, rank, &lastStr, "@") ) return false;
	GetNextSubStr(lastStr, languageID, &lastStr, "@");
	return true;
}

bool GetNextSubStr(string inStr, ref outStr, ref lastStr, string separator)
{
	if(inStr=="")
	{
		outStr="";
		lastStr="";
		return false;
	}
	int strSize = strlen(inStr)-1;
	if(strSize<=0)
	{
		outStr="";
		lastStr="";
		return false;
	}
	int sympos = findsubstr(inStr,separator,0);
	if(sympos==-1)
	{
		outStr = inStr;
		lastStr = "";
		return true;
	}
	if(sympos>0)	outStr = strcut(inStr,0,sympos-1);
	else	outStr = "";
	if(strSize>sympos+1) lastStr = strcut(inStr,sympos+1,strSize);
	else lastStr = "";
	return true;
}

void ProcessDeleteSaveFile()
{
	string curSave = GetCurSaveName();
	if( curSave=="" ) { return; }
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,curSave);
	if( IsEntity(scrshot) ) {
		SendMessage( scrshot, "ls", MSG_SCRSHOT_RELEASE, curSave ); // забыть снимок удалённой записи
		ForgetSaveTextures();
	}

	// LEO: список пересобираем ЦЕЛИКОМ, как при открытии окна. Раньше записи и ячейки сдвигались
	//   вручную, и после удаления разъезжались три вещи сразу: номер записи в слоте, разобранные
	//   данные и снимок - ячейки оставались пустыми. Снимки уцелевших записей лежат в кэше движка,
	//   так что пересборка стоит недорого.
	int nWanted = g_nCurrentSaveIndex;
	SetCurrentProfile( currentProfile );
	if( nWanted >= g_nSaveQuantity ) nWanted = g_nSaveQuantity - 1;
	if( nWanted < 0 ) nWanted = 0;
	setInitSelection( nWanted );
}

void ProcessLoad()
{
	string sCurSave = GetCurSaveName();
	PlayerProfile.name = currentProfile;

	IDoExit(RC_INTERFACE_SAVELOAD_EXIT);
	ResetSound();

	SetEventHandler("evntLoad","LoadGame",1);
	PostEvent( "evntLoad", 0, "s", "SAVE\" + PlayerProfile.name+ "\" +sCurSave );
	Event("evntPreLoad");
}

void ProcessSave()
{
	string curSave = GetCurSaveName();
	if( curSave!="" ) {
		SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,curSave);
	}
	LaunchCustomSaveGame();
}

void LaunchCustomSaveGame()
{
	string curLocName = GetCurLocationName();
	string saveName;
	saveName = GetClampedSaveName(currentProfile+" "+curLocName,0);
	int idx = 0;
	while( SendMessage(&GameInterface,"ls", MSG_INTERFACE_NEW_SAVE_FILE_NAME, saveName) == 1)
	{
		saveName = GetClampedSaveName(currentProfile+" "+curLocName,idx+1);
		idx++;
	}

	XI_WindowDisable("MAIN_WINDOW", true);
	XI_WindowDisable("CUSTOM_SAVE_WINDOW", false);
	XI_WindowShow("CUSTOM_SAVE_WINDOW", true);

	GameInterface.CUSTOM_SAVE_NAME.str = saveName;
	SetFormatedText("CUSTOM_SAVE_MSG", GetConvertStrWithReplace("Variable_save_load_1", "Interface.txt", "#space#", " "));
}

void ExitSaveCustomGame()
{
	XI_WindowShow( "CUSTOM_SAVE_WINDOW", false );
	XI_WindowDisable( "MAIN_WINDOW", false );
	// set current node to button for start profile choosing
	SetCurrentNode( "BTN_PROFILE" );
}

void ProcessCustomSaveAction()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( iComIndex == ACTION_ACTIVATE || iComIndex == ACTION_MOUSECLICK ) {
		if( sNodName == "CUSTOM_SAVE_BTN_OK" ) {

			string saveName = GameInterface.CUSTOM_SAVE_NAME.str;
			string sSaveDescriber = GetSaveDataString(saveName);
			if( IsEntity(scrshot) ) {
				SendMessage( scrshot, "ls", MSG_SCRSHOT_RELEASE, saveName ); // пишем поверх - забыть прежний снимок
				ForgetSaveTextures();
			}
			SetEventHandler("evntSave","SaveGame",1);
			PostEvent("evntSave",0,"ss", "SAVE\"+PlayerProfile.name+"\"+saveName, sSaveDescriber);
            // Warship Fix свечени¤
			ExitSaveCustomGame();
			IDoExit(RC_INTERFACE_SAVELOAD_EXIT);
		}
		if( sNodName == "CUSTOM_SAVE_BTN_CANCEL" ) {
			ExitSaveCustomGame();
		}
	}
}

string GetClampedSaveName(string sInName, int nNumber)
{
	if(nNumber>0) {return sInName + " " + nNumber;}
	else {return sInName;}
}

string GetCurSaveName()
{
	if( g_nCurrentSaveIndex<0 || g_nCurrentSaveIndex>=g_nSaveQuantity ) return "";
	string attr = "s" + g_nCurrentSaveIndex;
	if( CheckAttribute(&g_oSaveContainer,attr+".savename") ) {
		return g_oSaveContainer.(attr).savename;
	}
	return "";
}


void procConfirm()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( iComIndex==ACTION_ACTIVATE || iComIndex==ACTION_MOUSECLICK ) {
		if( sNodName=="CONFIRM_YES" ) {
			UndoConfirm(true);
		} else {
			UndoConfirm(false);
		}
	}
	if( iComIndex==ACTION_DEACTIVATE ) {
		UndoConfirm(false);
	}
}

void UndoConfirm(bool bPositiveChoose)
{
	// disable confirm
	XI_WindowDisable( "CONFIRM_WINDOW", true );
	XI_WindowShow( "CONFIRM_WINDOW", false );
	// enable window where confirm was started
	XI_WindowDisable( g_sConfirmReturnWindow, false );

	int nTmp = g_nConfirmMode;
	if( bPositiveChoose ) {
		switch( nTmp )
		{
		case CONFIRMMODE_PROFILE_DELETE: ProcDeleteProfile(); break;
		case CONFIRMMODE_SAVE_DELETE: ProcessDeleteSaveFile(); break;
		case CONFIRMMODE_SAVE_OVERWRITE: ProcessSave(); break;
		case CONFIRMMODE_LOAD_GAME: ProcessLoad(); break;
		}
	}
	// возвращаем управление правильной контролке
	switch( nTmp )
	{
	case CONFIRMMODE_PROFILE_DELETE: SetCurrentNode("PROFILE_WINDOW_BTN_DELETE"); break;
	case CONFIRMMODE_SAVE_DELETE:
		if(GetSelectable("BTN_DELETE")) {SetCurrentNode("BTN_DELETE");}
		else {SetCurrentNode("SAVE_SELECTER");}
	break;
	case CONFIRMMODE_SAVE_OVERWRITE: SetCurrentNode("BTN_SAVELOAD"); break;
	case CONFIRMMODE_LOAD_GAME: SetCurrentNode("BTN_SAVELOAD"); break;
	}
}

void ShowCharacterFace(int iPlace, string sFace)
{
	string nodname = "OFFICER" + (iPlace+1);
	string facegroup = sFace;
	if( sFace=="" || sFace=="*" ) {
		facegroup = "face128_fighter";
		switch( iPlace ) {
			case 11: facegroup = "face128_navigator"; break;
			case 12: facegroup = "face128_boatswain"; break;
			case 13: facegroup = "face128_cannoner"; break;
			case 14: facegroup = "face128_doctor"; break;
			case 15: facegroup = "face128_treasurer"; break;
			case 16: facegroup = "face128_carpenter"; break;
		}
	}
	SendMessage( &GameInterface, "lslss", MSG_INTERFACE_MSG_TO_NODE, nodname, 6, facegroup, "face" );
}

void ShowFaceInfo( string facestr )
{
	string sstr = facestr;
	string sdat;
	for( int i=0; i<18; i++ )
	{
		sdat = "";
		if( GetNextSubStr( sstr, &sdat, &sstr, "," ) ) {
			ShowCharacterFace(i,sdat);
		} else {
			ShowCharacterFace(i,"*");
		}
	}
}

void SSI(string shipTexture)
{
	if (shipTexture != "1000")	SetNewPicture("SHIP_ICON", InterfaceStates.InterfaceShipString + shipTexture + ".dds");
	else SetNewPicture("SHIP_ICON", "");
}

string GetFlagName(string value)
{
	switch (value)
	{
		case "England": return GetConvertStr("Variable_save_load_2", "Interface.txt");
		break;
		case "France": return GetConvertStr("Variable_save_load_3", "Interface.txt");
		break;
		case "Holland": return GetConvertStr("Variable_save_load_4", "Interface.txt");
		break;
		case "Spain": return GetConvertStr("Variable_save_load_5", "Interface.txt");
		break;
		case "Pirate": return GetConvertStr("Variable_save_load_6", "Interface.txt");
		break;
	}
	return GetConvertStrWithReplace("Variable_save_load_7", "Interface.txt", "#space#", " ");
}

// LEO: крупное превью выбранной записи. Снимок уже загружен для строки списка - берём тот же
//   указатель, второй раз с диска не читаем.
// LEO: снимок отпущен движком - указатели в записях протухли. Чистим их, иначе карточка
//   может показать освобождённую текстуру: список после этого всё равно пересобирается.
void ForgetSaveTextures()
{
	for( int i=0; i<MAX_SAVE_SLOTS; i++ )
	{
		DeleteAttribute( &g_oSaveList[i], "texture" );
	}
}

void ShowBigPreview(int nSlot)
{
	// выбранная запись уехала за пределы показанных строк - прежний кадр не трогаем
	if( nSlot < 0 || nSlot >= MAX_SAVE_SLOTS ) return;

	if( CheckAttribute(&g_oSaveList[nSlot], "texture") )
	{
		SendMessage( &GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, "BIG_PREVIEW", 7, sti(g_oSaveList[nSlot].texture) );
		return;
	}

	// LEO: заглушку ставим ТОЛЬКО у пустой ячейки. Раньше она попадала сюда и на секунду при
	//   прокрутке: строки переезжают, снимок для новой строки ещё не подгружен - и в карточке
	//   мелькал «битый диск», хотя запись на месте.
	if( sti(g_oSaveList[nSlot].saveidx) < 0 )
	{
		SendMessage( &GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, "BIG_PREVIEW", 2, 0, "interfaces\SaveIcons\SaveEmpty.dds" );
	}
}

// LEO: имя файла записи - справка, а не заголовок, поэтому идёт в хвосте строки и приглушённым:
//   глазами карточка читается по локации, а имя нужно лишь чтобы найти файл в папке сохранений.
string GetSaveFileTag(int nSlot)
{
	if( !CheckAttribute(&g_oSaveList[nSlot], "savefile") ) return "";
	if( g_oSaveList[nSlot].savefile == "" ) return "";
	return " <color=255,150,150,150>(" + g_oSaveList[nSlot].savefile + ")</color>";
}

void ReloadSaveInfo()
{
	int nSlot = g_nCurrentSaveIndex - g_nFirstSaveIndex;
	string info = "";
	string playtime = "#";
	string curship = "#";
	SetFormatedText("PLACE", GetConvertStrWithReplace("Variable_save_load_8", "Interface.txt", "#space#", " "));
	SetFormatedText("DIFFICULTY", GetConvertStrWithReplace("Variable_save_load_9", "Interface.txt", "#space#", " "));
	SetFormatedText("MONEY", "0");
	SetFormatedText("RANK", "0");
	if( nSlot>=0 && nSlot<MAX_SAVE_SLOTS && CheckAttribute(&g_oSaveList[nSlot],"faceinfo") ) {
		info = g_oSaveList[nSlot].faceinfo;
		if( g_oSaveList[nSlot].playtime != "" ) {
			playtime = "#" + XI_ConvertString("Play Time") + ": " + g_oSaveList[nSlot].playtime;
		}
	}
	ShowFaceInfo( info );
	if ( nSlot>=0 && nSlot<MAX_SAVE_SLOTS && CheckAttribute(&g_oSaveList[nSlot],"locName") ) SetFormatedText("PLACE", GetConvertStrWithReplace("Variable_save_load_10", "Interface.txt", "#space#", " ")+g_oSaveList[nSlot].locName+GetSaveFileTag(nSlot));
	SetVAligmentFormatedText("PLACE");
	if ( nSlot>=0 && nSlot<MAX_SAVE_SLOTS && CheckAttribute(&g_oSaveList[nSlot],"curship") ) SSI(g_oSaveList[nSlot].curship);
	if ( nSlot>=0 && nSlot<MAX_SAVE_SLOTS && CheckAttribute(&g_oSaveList[nSlot],"nation") ) {SetNewGroupPicture("FLAG_ICON", "NATIONS", g_oSaveList[nSlot].nation); SetFormatedText("FLAG_TEXT", GetFlagName(g_oSaveList[nSlot].nation));}
	if ( nSlot>=0 && nSlot<MAX_SAVE_SLOTS && CheckAttribute(&g_oSaveList[nSlot],"difficulty") ) SetFormatedText("DIFFICULTY", GetConvertStrWithReplace("Variable_save_load_13", "Interface.txt", "#space#", " ")+GetLevelComplexity(sti(g_oSaveList[nSlot].difficulty)));
	if ( nSlot>=0 && nSlot<MAX_SAVE_SLOTS && CheckAttribute(&g_oSaveList[nSlot],"money") ) SetFormatedText("MONEY", GetConvertStrWithReplace("Variable_save_load_14", "Interface.txt", "#space#", " ")+FindRussianMoneyString(sti(g_oSaveList[nSlot].money)));
	if ( nSlot>=0 && nSlot<MAX_SAVE_SLOTS && CheckAttribute(&g_oSaveList[nSlot],"rank") ) SetFormatedText("RANK", GetConvertStrWithReplace("Variable_save_load_15", "Interface.txt", "#space#", " ")+g_oSaveList[nSlot].rank);
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVEINFO", 1, 2, playtime );

	ShowBigPreview(nSlot);	// крупное превью выбранной записи

	if( info == "" ) {
		SetSelectable( "BTN_SAVELOAD", bThisSave );
		SetSelectable( "BTN_DELETE", false );
	} else {
		SetSelectable( "BTN_SAVELOAD", true );
		SetSelectable( "BTN_DELETE", true );
	}
}

// LEO: лента сама ведёт скроллер и колесо, а нам сообщает СМЕНУ ВЕРХНЕГО РЯДА - по ней и
//   подкачиваем данные. Прежний обработчик ScrollPosChange двигал список рывками по ползунку.
void RibbonTopChange()
{
	string sNode = GetEventData();
	int nTop = GetEventData();
	if( sNode != "SAVE_RIBBON" ) return;

	int nFirst = nTop * SLOTS_IN_LINE;
	if( nFirst == g_nFirstSaveIndex ) return;

	FillSaveList(nFirst);
}

void LoadProfile()
{
	ProcChooseProfile();
}

void SaveLoad()
{
	int iComIdx = GetEventData();
	string sNodName = GetEventData();

	if( sNodName == "SAVEIMG1" ) { SelectSaveImage( g_nFirstSaveIndex + 0 ) }
	if( sNodName == "SAVEIMG2" ) { SelectSaveImage( g_nFirstSaveIndex + 1 ) }
	if( sNodName == "SAVEIMG3" ) { SelectSaveImage( g_nFirstSaveIndex + 2 ) }
	if( sNodName == "SAVEIMG4" ) { SelectSaveImage( g_nFirstSaveIndex + 3 ) }
	if( sNodName == "SAVEIMG5" ) { SelectSaveImage( g_nFirstSaveIndex + 4 ) }
	if( sNodName == "SAVEIMG6" ) { SelectSaveImage( g_nFirstSaveIndex + 5 ) }
	if( sNodName == "SAVEIMG7" ) { SelectSaveImage( g_nFirstSaveIndex + 6 ) }
	if( sNodName == "SAVEIMG8" ) { SelectSaveImage( g_nFirstSaveIndex + 7 ) }
	if( sNodName == "SAVEIMG9" ) { SelectSaveImage( g_nFirstSaveIndex + 8 ) }
	if( sNodName == "SAVEIMG10" ) { SelectSaveImage( g_nFirstSaveIndex + 9 ) }

	if( sNodName == "SAVEIMG11" ) { SelectSaveImage( g_nFirstSaveIndex + 10 ) }
	if( sNodName == "SAVEIMG12" ) { SelectSaveImage( g_nFirstSaveIndex + 11 ) }
	if( sNodName == "SAVEIMG13" ) { SelectSaveImage( g_nFirstSaveIndex + 12 ) }
	if( sNodName == "SAVEIMG14" ) { SelectSaveImage( g_nFirstSaveIndex + 13 ) }
	if( sNodName == "SAVEIMG15" ) { SelectSaveImage( g_nFirstSaveIndex + 14 ) }
	if( sNodName == "SAVEIMG16" ) { SelectSaveImage( g_nFirstSaveIndex + 15 ) }
	if( sNodName == "SAVEIMG17" ) { SelectSaveImage( g_nFirstSaveIndex + 16 ) }
	if( sNodName == "SAVEIMG18" ) { SelectSaveImage( g_nFirstSaveIndex + 17 ) }
	if( sNodName == "SAVEIMG19" ) { SelectSaveImage( g_nFirstSaveIndex + 18 ) }
	if( sNodName == "SAVEIMG20" ) { SelectSaveImage( g_nFirstSaveIndex + 19 ) }

	if( sNodName == "SAVEIMG21" ) { SelectSaveImage( g_nFirstSaveIndex + 20 ) }
	if( sNodName == "SAVEIMG22" ) { SelectSaveImage( g_nFirstSaveIndex + 21 ) }
	if( sNodName == "SAVEIMG23" ) { SelectSaveImage( g_nFirstSaveIndex + 22 ) }
	if( sNodName == "SAVEIMG24" ) { SelectSaveImage( g_nFirstSaveIndex + 23 ) }
	if( sNodName == "SAVEIMG25" ) { SelectSaveImage( g_nFirstSaveIndex + 24 ) }
	if( sNodName == "SAVEIMG26" ) { SelectSaveImage( g_nFirstSaveIndex + 25 ) }
	if( sNodName == "SAVEIMG27" ) { SelectSaveImage( g_nFirstSaveIndex + 26 ) }
	if( sNodName == "SAVEIMG28" ) { SelectSaveImage( g_nFirstSaveIndex + 27 ) }
	if( sNodName == "SAVEIMG29" ) { SelectSaveImage( g_nFirstSaveIndex + 28 ) }
	if( sNodName == "SAVEIMG30" ) { SelectSaveImage( g_nFirstSaveIndex + 29 ) }

	if( sNodName == "SAVEIMG31" ) { SelectSaveImage( g_nFirstSaveIndex + 30 ) }
	if( sNodName == "SAVEIMG32" ) { SelectSaveImage( g_nFirstSaveIndex + 31 ) }
	if( sNodName == "SAVEIMG33" ) { SelectSaveImage( g_nFirstSaveIndex + 32 ) }
	if( sNodName == "SAVEIMG34" ) { SelectSaveImage( g_nFirstSaveIndex + 33 ) }
	if( sNodName == "SAVEIMG35" ) { SelectSaveImage( g_nFirstSaveIndex + 34 ) }
	if( sNodName == "SAVEIMG36" ) { SelectSaveImage( g_nFirstSaveIndex + 35 ) }
	if( sNodName == "SAVEIMG37" ) { SelectSaveImage( g_nFirstSaveIndex + 36 ) }
	if( sNodName == "SAVEIMG38" ) { SelectSaveImage( g_nFirstSaveIndex + 37 ) }
	if( sNodName == "SAVEIMG39" ) { SelectSaveImage( g_nFirstSaveIndex + 38 ) }
	if( sNodName == "SAVEIMG40" ) { SelectSaveImage( g_nFirstSaveIndex + 39 ) }

	SaveLoadCurrentIntoSlot();
}

// LEO: фильтр списка - четыре ВЗАИМОИСКЛЮЧАЮЩИХ режима (радиокнопки). Имя файла говорит о типе
//   записи: " QuickSave " ставит быстрое сохранение, " AutoSave " - автосейв по типу перехода
//   (interface.c, AutoSave_Start), всё прочее - ручное. Выбор запоминается между заходами.
#define SAVE_FILTER_ALL		0
#define SAVE_FILTER_MANUAL	1
#define SAVE_FILTER_QUICK	2
#define SAVE_FILTER_AUTO	3
#define SAVE_FILTER_NODE	"CHECK_SAVE_FILTER"

// ⚠️ Движок шлёт «CheckButtonChange» и на ПРОГРАММНУЮ установку флага, поэтому подсветка
//   кнопок сама себя вызывала бы по кругу. На время применения фильтра событие игнорируем.
bool g_bSaveFilterBusy = false;

int GetSaveFilter()
{
	if( !CheckAttribute(&InterfaceStates, "SaveListFilter") ) return SAVE_FILTER_ALL;
	int nFilter = sti(InterfaceStates.SaveListFilter);
	if( nFilter < SAVE_FILTER_ALL ) nFilter = SAVE_FILTER_ALL;
	if( nFilter > SAVE_FILTER_AUTO ) nFilter = SAVE_FILTER_ALL;
	return nFilter;
}

// Отметить в радиогруппе выбранный режим - остальные узел гасит сам (exclusiveChoose).
//   Секции идут в том же порядке, что и SAVE_FILTER_*, но нумеруются с единицы.
void ShowSaveFilter(int nFilter)
{
	CheckButton_SetState(SAVE_FILTER_NODE, nFilter + 1, true);
}

// Подходит ли запись под режим фильтра.
bool IsSaveInFilter(string sSaveName, int nFilter)
{
	if( nFilter == SAVE_FILTER_ALL ) return true;
	bool bQuick = false;
	if( findSubStr(sSaveName, " QuickSave ", 0) >= 0 ) bQuick = true;
	bool bAuto = false;
	if( findSubStr(sSaveName, " AutoSave ", 0) >= 0 ) bAuto = true;
	if( nFilter == SAVE_FILTER_QUICK ) return bQuick;
	if( nFilter == SAVE_FILTER_AUTO ) return bAuto;
	if( bQuick ) return false;   // ручные - всё, что не быстрое и не авто
	if( bAuto ) return false;
	return true;
}

// Сменить фильтр: запомнить выбор и перечитать список профиля.
void ApplySaveFilter(int nFilter)
{
	g_bSaveFilterBusy = true;
	InterfaceStates.SaveListFilter = nFilter;
	// выбор пишем в опции СРАЗУ: SetCurrentProfile в конце зовёт LoadGameOptions, и та вернула бы
	//   прежнее значение поверх нового
	SaveGameOptions();
	SetCurrentProfile(currentProfile);
	g_bSaveFilterBusy = false;
}

void CheckButtonChange()
{
	if( g_bSaveFilterBusy ) return;
	string sControl = GetEventData();
	int nSection = GetEventData();   // номер секции, с единицы
	int nOn = GetEventData();        // 1 - включена, 0 - снята
	if( sControl != SAVE_FILTER_NODE ) return;
	if( nOn == 0 ) return;           // снятие приходит парой к включению другой секции
	ApplySaveFilter(nSection - 1);
}
