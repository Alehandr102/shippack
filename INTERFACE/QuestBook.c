#include "interface\modelviewer\modelviewer.c"

int curQuestTop;
string CurTable, CurRow;
int iMaxGoodsStore = 50000;
string sMessageMode;
ref chrefsp;
int FState_SHIP_N_L = 0;
int FState_SHIP_L = 8;
int FState_SHIP_N_R = 0;
int FState_SHIP_R = 8;
int FState_BLADE = 0;
int FState_BLADEQ = 0;
int FState_GunQ = 0;
int FState_BONUS = 0;
int FState_COMPLETE = 0;
int FState_ACTIVE = 0;        // 0 - все, 1 - только активные
int FState_AVAILABLE = 0;     // 0 - все, 1 - только доступные
int FState_UNAVAILABLE = 0;   // 0 - все, 1 - только недоступные по рангу
int Last_Left_Ship = 1;//запоминаем выбранный в таблице корабль
int Last_Right_Ship = 1;
int rownumberach;

string GunTypeQ[4]={"poor","ordinary","good","excellent"};//фильтр оружия по качеству
string BladeTypeQ[4]={"poor","ordinary","good","excellent"};//фильтр оружия по качеству
string BladeType[3]={"FencingLight","Fencing","FencingHeavy"};//фильтр оружия по тяжести

string BonusType[7]={"backpack","Totem","indian","jewelry","cirass","talisman","mineral"};//фильтр предметов по типам
string ShipNation[5]={"england","france","holland","spain","pirate"};

void InitInterface(string iniName)
{
	InterfaceStack.SelectMenu_node = "LaunchQuestBook"; // запоминаем, что звать по Ф2
	GameInterface.title = "titleQuestBook";

	if(sti(Pchar.Ship.Type) != SHIP_NOTUSED && sti(RealShips[sti(Pchar.Ship.Type)].BaseType) <= SHIP_OCEAN)//не исследуем квестовые, и лодку с фортом
	{	
		sTemp = RealShips[sti(Pchar.Ship.Type)].BaseName;
		Pchar.Encyclopedia.(sTemp) = "1";//тут не нужно это. Нужно было при входе на корабль вставлять такое, а не только при абордаже
	}
	if(bFillEncyShips) 
	{
		aref aShips;
		makearef(aShips, Pchar.Encyclopedia);
		int Sum = GetAttributesNum(aShips);
		if (Sum != sti(pchar.questTemp.shipsearchcount)) 
		{
			pchar.questTemp.shipsearchcount = Sum;
			log_info(GetConvertStrWithReplace("Variable_QuestBook_1", "Interface.txt", "#space#", " ") + XI_Convertstring(sTemp) + GetConvertStrWithReplace("Variable_QuestBook_2", "Interface.txt", "#space#", " ") + pchar.questTemp.shipsearchcount + GetConvertStrWithReplace("Variable_QuestBook_3", "Interface.txt", "#space#", " "));
			CheckAndUnlock("AchShipSearch", Sum);
		}
	}
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	selectJournal(1); // первый режим журнала, только активные

	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);

	SetEventHandler("eTabControlPress","procTabChange",0);
	SetEventHandler("SetScrollerPos","SetScrollerPos",0);
	SetEventHandler("ScrollPosChange","ProcScrollPosChange",0);
	SetEventHandler("RibbonTopChange","ProcRibbonTopChange",0); // LEO: прокрутка ленты - пересчитать прогресс-бары достижений
	SetEventHandler("ScrollTopChange","ProcScrollChange",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("QuestTopChange","QuestTopChange",0);
	SetEventHandler("BackToTitle","BackToTitle",0);
	SetEventHandler("QuestActivate","XI_QuestActivate",0);
	SetEventHandler("QuestDeActivate","QuestDeActivate",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("CheckButtonChange", "ProcessFilter", 0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ShowPGGInfo","ShowPGGInfo",0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("ShowShipsWindow","ShowShipsWindow",0);
	SetEventHandler("ShowBladeWindow","ShowBladeWindow",0);
	SetEventHandler("ShowGunWindow","ShowGunWindow",0);
	SetEventHandler("ShowFoodWindow","ShowFoodWindow",0);
	SetEventHandler("ShowBonusItemWindow","ShowBonusItemWindow",0);
	SetEventHandler("ShowQuestsWindow","ShowQuestsWindow",0);
	SetEventHandler("PointsEX","PointsEX",0);
	SetEventHandler("HideEncWindows","HideEncWindows",0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
	SetEventHandler("ievntCreateNotice","LaunchCreateNoticeFromBook",0); // Gregg: кнопка "Заметка в журнал"

	InitModelViewer("MODEL_VIEWER");

	sMessageMode = "";
	string sDate = " ";
	sDate = GetDateString();
	string sTime;
	sTime = GetTimeString();
	SetFormatedText("CURRENT_DATE_CAPTION", sDate + " "+ sTime);
	XI_RegistryExitKey("IExit_F2");
	SetControlsTabMode(1);

	InitTableHeader();
	SetNodeUsing("EXIT_ENC_BTN",false);
	SetNodeUsing("SHIP_PICTURE_VL", false);
	SetNodeUsing("SHIP_PICTURE_VR", false);
	SetNodeUsing("SHIP_BIG_PICTURE_VIDEO",false);
	SetNodeUsing("MODEL_VIEWER_VIDEO_BG",false);
	ShowStr(true);
	if (checkattribute(NullCharacter, "saveFState_COMPLETE")) FState_COMPLETE = sti(NullCharacter.saveFState_COMPLETE);//QUESTS_CLASS
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"QUESTS_CLASS", 2, 1, FState_COMPLETE);

	if (checkattribute(NullCharacter, "saveFState_ACTIVE")) FState_ACTIVE = sti(NullCharacter.saveFState_ACTIVE);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"QUESTS_CLASS", 2, 2, FState_ACTIVE);

	if (checkattribute(NullCharacter, "saveFState_AVAILABLE")) FState_AVAILABLE = sti(NullCharacter.saveFState_AVAILABLE);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"QUESTS_CLASS", 2, 3, FState_AVAILABLE);

	if (checkattribute(NullCharacter, "saveFState_UNAVAILABLE")) FState_UNAVAILABLE = sti(NullCharacter.saveFState_UNAVAILABLE);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"QUESTS_CLASS", 2, 4, FState_UNAVAILABLE);
}

void ShowStr(bool istrue)
{
	if (istrue)
	{
		SetNodeUsing("TABBTN_QUEST",true);
		SetNodeUsing("TABBTN_QUESTARJ",true);
		SetNodeUsing("TABBTN_INFO",true);
		SetNodeUsing("TABBTN_CASHBOOK",true);
		SetNodeUsing("TABBTN_SHIP_PLACE",true);
		SetNodeUsing("TABBTN_STATISTIC",true);
		SetNodeUsing("TABBTN_STOREBOOK",true);
		if (InterfaceStates.AltFont == "0")
		{
			SetNodeUsing("TABSTR_QUEST",true);
			SetNodeUsing("TABSTR_QUESTARJ",true);
			SetNodeUsing("TABSTR_INFO",true);
			SetNodeUsing("TABSTR_CASHBOOK",true);
			SetNodeUsing("TABSTR_SHIP_PLACE",true);
			SetNodeUsing("TABSTR_STATISTIC",true);
			SetNodeUsing("TABSTR_STOREBOOK",true);
			SetNodeUsing("TABSTR_QUEST_ALT",false);
			SetNodeUsing("TABSTR_QUESTARJ_ALT",false);
			SetNodeUsing("TABSTR_INFO_ALT",false);
			SetNodeUsing("TABSTR_CASHBOOK_ALT",false);
			SetNodeUsing("TABSTR_SHIP_PLACE_ALT",false);
			SetNodeUsing("TABSTR_STATISTIC_ALT",false);
			SetNodeUsing("TABSTR_STOREBOOK_ALT",false);
		}
		else
		{
			SetNodeUsing("TABSTR_QUEST",false);
			SetNodeUsing("TABSTR_QUESTARJ",false);
			SetNodeUsing("TABSTR_INFO",false);
			SetNodeUsing("TABSTR_CASHBOOK",false);
			SetNodeUsing("TABSTR_SHIP_PLACE",false);
			SetNodeUsing("TABSTR_STATISTIC",false);
			SetNodeUsing("TABSTR_STOREBOOK",false);
			SetNodeUsing("TABSTR_QUEST_ALT",true);
			SetNodeUsing("TABSTR_QUESTARJ_ALT",true);
			SetNodeUsing("TABSTR_INFO_ALT",true);
			SetNodeUsing("TABSTR_CASHBOOK_ALT",true);
			SetNodeUsing("TABSTR_SHIP_PLACE_ALT",true);
			SetNodeUsing("TABSTR_STATISTIC_ALT",true);
			SetNodeUsing("TABSTR_STOREBOOK_ALT",true);
		}
	}
	else
	{
		SetNodeUsing("TABBTN_QUEST",false);
		SetNodeUsing("TABBTN_QUESTARJ",false);
		SetNodeUsing("TABBTN_INFO",false);
		SetNodeUsing("TABBTN_CASHBOOK",false);
		SetNodeUsing("TABBTN_SHIP_PLACE",false);
		SetNodeUsing("TABBTN_STATISTIC",false);
		SetNodeUsing("TABBTN_STOREBOOK",false);
		SetNodeUsing("TABSTR_QUEST",false);
		SetNodeUsing("TABSTR_QUESTARJ",false);
		SetNodeUsing("TABSTR_INFO",false);
		SetNodeUsing("TABSTR_CASHBOOK",false);
		SetNodeUsing("TABSTR_SHIP_PLACE",false);
		SetNodeUsing("TABSTR_STATISTIC",false);
		SetNodeUsing("TABSTR_STOREBOOK",false);
		SetNodeUsing("TABSTR_QUEST_ALT",false);
		SetNodeUsing("TABSTR_QUESTARJ_ALT",false);
		SetNodeUsing("TABSTR_INFO_ALT",false);
		SetNodeUsing("TABSTR_CASHBOOK_ALT",false);
		SetNodeUsing("TABSTR_SHIP_PLACE_ALT",false);
		SetNodeUsing("TABSTR_STATISTIC_ALT",false);
		SetNodeUsing("TABSTR_STOREBOOK_ALT",false);
	}
}

void XI_SetQuestData(bool qtitle)
{
	aref arefTmp;
	makearef(arefTmp,pchar.TmpQuestInfo);

	if (InterfaceStates.AltFont == "0") XI_SetQuestTitles("QUEST_TITLE_ALT",arefTmp,curQuestTop);
	else XI_SetQuestTitles("QUEST_TITLE",arefTmp,curQuestTop);
	SetNodeUsing("QUEST_TITLE_ALT",false);
	SetNodeUsing("QUEST_TITLE",false);
	SetNodeUsing("QUEST_TEXT_ALT_NORMAL",false);
	SetNodeUsing("QUEST_TEXT",false);
	SetNodeUsing("QUEST_TEXT_ALT",false);

	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",true);
	else SetNodeUsing("QUEST_TITLE",true);
	switch (sti(InterfaceStates.AltFont))
	{
		case 0: SetNodeUsing("QUEST_TEXT_ALT_NORMAL",!qtitle); break;
		case 1: SetNodeUsing("QUEST_TEXT",!qtitle); break;
		case 2: SetNodeUsing("QUEST_TEXT_ALT",!qtitle); break;
	}
	SetNodeUsing("QUESTSCROLL",true);
	SetNodeUsing("QUESTSCROLL2",true); // LEO: правый скролл текста квеста - показываем во вкладке "Журнал" (в других прячет HideQuests)

	ShowButtons();

	if(qtitle == true)
	{
		if (InterfaceStates.AltFont == "0") SetCurrentNode("QUEST_TITLE_ALT");
		else SetCurrentNode("QUEST_TITLE");
	}
	else
	{
		switch (sti(InterfaceStates.AltFont))
		{
			case 0: SetCurrentNode("QUEST_TEXT_ALT_NORMAL"); break;
			case 1: SetCurrentNode("QUEST_TEXT"); break;
			case 2: SetCurrentNode("QUEST_TEXT_ALT"); break;
		}
	}
}

void ShowButtons()
{

}

void HideEncWindows()
{
	XI_WindowShow("ENC_INFO_SHIP",false);
	XI_WindowShow("ENC_INFO_BLADE",false);
	XI_WindowShow("ENC_INFO_GUN",false);
	XI_WindowShow("ENC_INFO_FOOD",false);
	XI_WindowShow("ENC_INFO_BONUSITEM",false);
	XI_WindowShow("ENC_INFO_ACHIEVEMENTS",false);
	XI_WindowShow("ENC_INFO_QUESTS",false);

	XI_WindowDisable("ENC_INFO_SHIP",true);
	XI_WindowDisable("ENC_INFO_BLADE",true);
	XI_WindowDisable("ENC_INFO_GUN",true);
	XI_WindowDisable("ENC_INFO_FOOD",true);
	XI_WindowDisable("ENC_INFO_BONUSITEM",true);
	XI_WindowDisable("ENC_INFO_ACHIEVEMENTS",true);
	XI_WindowDisable("ENC_INFO_QUESTS",true);

	ShowStr(true);

	SetNodeUsing("I_CHARACTER_2",true);
	SetNodeUsing("I_CHARACTER",true);
	SetNodeUsing("I_SHIP_2",true);
	SetNodeUsing("I_SHIP",true);
	SetNodeUsing("I_NATIONS_2",true);
	SetNodeUsing("I_NATIONS",true);
	SetNodeUsing("I_TRADEBOOK_2",true);
	SetNodeUsing("I_TRADEBOOK",true);
	SetNodeUsing("I_ITEMS_2",true);
	SetNodeUsing("I_ITEMS",true);

	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",true);
	else SetNodeUsing("QUEST_TITLE",true);
	SetNodeUsing("SHIP_TABLE_LIST_LEFT",false);
	SetNodeUsing("SHIP_TABLE_LIST_RIGHT",false);
	HideAchievements();
	SetNodeUsing("ACHIEVEMENTS_STR",false);
	SetNodeUsing("ACHIEVEMENTS_INFO_FRAME_CAPTION",false);
	SetNodeUsing("ACHIEVEMENTS_INFO_FADER",false);
	SetNodeUsing("ACHIEVEMENTS_INFO_DUST",false);
	SetNodeUsing("QUESTS_INFO_FADER",false);
	SetNodeUsing("QUESTS_INFO_DUST",false);
	SetNodeUsing("POINTS_EXCHANGE",false);
	SetControlsTabMode( 3 );
	XI_WindowShow("ENCYCLOPEDIA_INFO_BUTTONS", true);
	XI_WindowDisable("ENCYCLOPEDIA_INFO_BUTTONS", false);

	SetNodeUsing("EXIT_ENC_BTN",false);
	SetNodeUsing("EXIT_BTN",true);
}

//инфа о кораблях
void ShowShipsWindow()
{
	XI_WindowShow("ENC_INFO_SHIP",true);
	XI_WindowDisable("ENC_INFO_SHIP",false);
	XI_WindowShow("ENC_INFO_BLADE",false);
	XI_WindowShow("ENC_INFO_GUN",false);
	XI_WindowShow("ENC_INFO_FOOD",false);
	XI_WindowShow("ENC_INFO_BONUSITEM",false);
	XI_WindowShow("ENC_INFO_ACHIEVEMENTS",false);
	XI_WindowShow("ENC_INFO_QUESTS",false);

	ShowStr(false);
	HideQuestTabs();
	HideAchievements();

	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",false);
	else SetNodeUsing("QUEST_TITLE",false);
	SetNodeUsing("SHIP_TABLE_LIST_LEFT",true);
	SetNodeUsing("SHIP_TABLE_LIST_RIGHT",true);
	FillShipInfoEncy("SHIP_TABLE_LIST_LEFT");
	FillShipInfoEncy("SHIP_TABLE_LIST_RIGHT");
	SetNodeUsing("EXIT_ENC_BTN",true);
	SetNodeUsing("EXIT_BTN",false);
}

void ShowBladeWindow()
{
	XI_WindowShow("ENC_INFO_SHIP",false);
	XI_WindowShow("ENC_INFO_BLADE",true);
	XI_WindowDisable("ENC_INFO_BLADE",false);
	XI_WindowShow("ENC_INFO_GUN",false);
	XI_WindowShow("ENC_INFO_FOOD",false);
	XI_WindowShow("ENC_INFO_BONUSITEM",false);
	XI_WindowShow("ENC_INFO_ACHIEVEMENTS",false);
	XI_WindowShow("ENC_INFO_QUESTS",false);

	ShowStr(false);
	HideQuestTabs();
	HideAchievements();

	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",false);
	else SetNodeUsing("QUEST_TITLE",false);
	AddToTable("BLADE_TABLE_LIST", "blade");
	SetNodeUsing("EXIT_ENC_BTN",true);
	SetNodeUsing("EXIT_BTN",false);
}

void ShowGunWindow()
{
	XI_WindowShow("ENC_INFO_SHIP",false);
	XI_WindowShow("ENC_INFO_BLADE",false);
	XI_WindowShow("ENC_INFO_GUN",true);
	XI_WindowDisable("ENC_INFO_GUN",false);
	XI_WindowShow("ENC_INFO_FOOD",false);
	XI_WindowShow("ENC_INFO_BONUSITEM",false);
	XI_WindowShow("ENC_INFO_ACHIEVEMENTS",false);
	XI_WindowShow("ENC_INFO_QUESTS",false);

	ShowStr(false);
	HideQuestTabs();
	HideAchievements();

	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",false);
	else SetNodeUsing("QUEST_TITLE",false);
	AddToTable("GUN_TABLE_LIST", "gun");
	SetNodeUsing("EXIT_ENC_BTN",true);
	SetNodeUsing("EXIT_BTN",false);
}

void ShowFoodWindow()
{
	XI_WindowShow("ENC_INFO_SHIP",false);
	XI_WindowShow("ENC_INFO_BLADE",false);
	XI_WindowShow("ENC_INFO_GUN",false);
	XI_WindowShow("ENC_INFO_FOOD",true);
	XI_WindowDisable("ENC_INFO_FOOD",false);
	XI_WindowShow("ENC_INFO_BONUSITEM",false);
	XI_WindowShow("ENC_INFO_ACHIEVEMENTS",false);
	XI_WindowShow("ENC_INFO_QUESTS",false);

	ShowStr(false);
	HideQuestTabs();
	HideAchievements();

	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",false);
	else SetNodeUsing("QUEST_TITLE",false);
	AddToTable("FOOD_TABLE_LIST", "food");
	SetNodeUsing("EXIT_ENC_BTN",true);
	SetNodeUsing("EXIT_BTN",false);
}

void ShowBonusItemWindow()
{
	XI_WindowShow("ENC_INFO_SHIP",false);
	XI_WindowShow("ENC_INFO_BLADE",false);
	XI_WindowShow("ENC_INFO_GUN",false);
	XI_WindowShow("ENC_INFO_FOOD",false);
	XI_WindowShow("ENC_INFO_BONUSITEM",true);
	XI_WindowDisable("ENC_INFO_BONUSITEM",false);
	XI_WindowShow("ENC_INFO_ACHIEVEMENTS",false);
	XI_WindowShow("ENC_INFO_QUESTS",false);

	ShowStr(false);
	HideQuestTabs();
	HideAchievements();

	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",false);
	else SetNodeUsing("QUEST_TITLE",false);
	AddToTable("BONUSITEM_TABLE_LIST", "BonusItem");
	SetNodeUsing("EXIT_ENC_BTN",true);
	SetNodeUsing("EXIT_BTN",false);
}

void ShowQuestsWindow()
{
	XI_WindowShow("ENC_INFO_SHIP",false);
	XI_WindowShow("ENC_INFO_BLADE",false);
	XI_WindowShow("ENC_INFO_GUN",false);
	XI_WindowShow("ENC_INFO_FOOD",false);
	XI_WindowShow("ENC_INFO_BONUSITEM",false);
	XI_WindowShow("ENC_INFO_ACHIEVEMENTS",false);
	XI_WindowShow("ENC_INFO_QUESTS",true);
	XI_WindowDisable("ENC_INFO_QUESTS",false);

	ShowStr(false);
	HideQuestTabs();
	HideAchievements();

	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",false);
	else SetNodeUsing("QUEST_TITLE",false);
	ShowInfoWindowEncyQuests();
	FillQuestsHeader();
	FillQuests();
	SetNodeUsing("EXIT_ENC_BTN",true);
	SetNodeUsing("EXIT_BTN",false);
}

void HideQuestTabs()
{
	SetNodeUsing("I_CHARACTER_2",false);
	SetNodeUsing("I_CHARACTER",false);
	SetNodeUsing("I_SHIP_2",false);
	SetNodeUsing("I_SHIP",false);
	SetNodeUsing("I_NATIONS_2",false);
	SetNodeUsing("I_NATIONS",false);
	SetNodeUsing("I_TRADEBOOK_2",false);
	SetNodeUsing("I_TRADEBOOK",false);
	SetNodeUsing("I_ITEMS_2",false);
	SetNodeUsing("I_ITEMS",false);
}

void HideQuests()
{
	if (InterfaceStates.AltFont == "0") SetNodeUsing("QUEST_TITLE_ALT",false);
	else SetNodeUsing("QUEST_TITLE",false);
	switch (sti(InterfaceStates.AltFont))
	{
		case 0: SetNodeUsing("QUEST_TEXT_ALT_NORMAL",false); break;
		case 1: SetNodeUsing("QUEST_TEXT",false); break;
		case 2: SetNodeUsing("QUEST_TEXT_ALT",false); break;
	}
	SetNodeUsing("QUESTSCROLL",false);
	SetNodeUsing("QUESTSCROLL2",false); // LEO: второй скролл тоже прятать в других вкладках
}

void HideStatistic()
{
	SetNodeUsing("TABLE_SHIPCLASS",false);
	SetNodeUsing("TABLE_HUMAN",false);
	SetNodeUsing("TABLE_NATION",false);
	SetNodeUsing("TABLE_OTHER",false);
	SetNodeUsing("TABLE_MONEY",false);
	CreateString(true, "LevelComplexity", "", "INTERFACE_ULTRASMALL", COLOR_NORMAL, 0, 0, SCRIPT_ALIGN_RIGHT, 1);
}

void HideCashBook()
{
	SetNodeUsing("SCROLL_DEBIT",false);
	SetNodeUsing("TABLE_DEBIT",false);
	SetNodeUsing("SCROLL_CREDIT",false);
	SetNodeUsing("TABLE_CREDIT",false);
}

void HideShipPlace()
{
	SetNodeUsing("TABLE_SHIP_PLACE",false);
	SetNodeUsing("SCROLL_SHIP_PLACE",false);
	SetNodeUsing("SHIPAUCTION_TOTAL",false); // LEO: строка-итог аукциона
}

void HideStoreBook()
{
	SetNodeUsing("TABLE_CITY",false);
	SetNodeUsing("SCROLL_CITY",false);
	SetNodeUsing("TABLE_GOODS",false);
	SetNodeUsing("SCROLL_GOODS",false);
}

void ProcessCancelExit()
{
	switch (sti(InterfaceStates.AltFont))
	{
		case 0:
			if( GetSelectable("QUEST_TEXT_ALT_NORMAL") )
			{
				XI_SetQuestData(true);
				return;
			}
		break;
		case 1:
			if( GetSelectable("QUEST_TEXT") )
			{
				XI_SetQuestData(true);
				return;
			}
		break;
		case 2:
			if( GetSelectable("QUEST_TEXT_ALT") )
			{
				XI_SetQuestData(true);
				return;
			}
		break;
	}
	if (CheckForEnabledWindows())
	{
		HideEncWindows();
		return;
	}
	if(currentWindow != "")
	{
		QuestBookExitModelViewer();
	} else {
		IDoExit(RC_INTERFACE_ANY_EXIT);
	}
}

bool CheckForEnabledWindows()
{
	if (XI_IsWindowEnable("ENC_INFO_SHIP") || XI_IsWindowEnable("ENC_INFO_BLADE") || XI_IsWindowEnable("ENC_INFO_GUN") || XI_IsWindowEnable("ENC_INFO_QUESTS")
	|| XI_IsWindowEnable("ENC_INFO_BONUSITEM") || XI_IsWindowEnable("ENC_INFO_FOOD") || XI_IsWindowEnable("ENC_INFO_ACHIEVEMENTS")) return true;
	return false;
}

void QuestTopChange()
{
	if( GetSelectable("QUEST_TITLE") || GetSelectable("QUEST_TITLE_ALT"))
	{
		int newTop = curQuestTop+GetEventData();

		aref arefTmp;
		makearef(arefTmp,pchar.TmpQuestInfo);
		int maxVal = GetAttributesNum(arefTmp);
		if(newTop>=maxVal)
		{
			newTop=maxVal-1;
		}
		if(newTop<0)
		{
			newTop=0;
		}

		if(newTop!=curQuestTop)
		{
			curQuestTop=newTop;
			if (InterfaceStates.AltFont == "0") XI_SetQuestTitles("QUEST_TITLE_ALT",arefTmp,curQuestTop);
			else XI_SetQuestTitles("QUEST_TITLE",arefTmp,curQuestTop);
			XI_SetScroller(MakeFloat(newTop)/MakeFloat(maxVal));
		}
	}
}

void SetQTextShow(aref pA,int qnum)
{
	// boal Покраска, выбрали 22.06.07 -->
	if (qnum >= GetAttributesNum(pA)) {
		trace("ERROR: SetQTextShow qnum more than pA length")
		return;
	}
	aref arTopic = GetAttributeN(pA, qnum);
	DeleteQuestHeaderColor(GetAttributeName(arTopic));
	// boal <--
	switch (sti(InterfaceStates.AltFont))
	{
		case 0:
			SendMessage(&GameInterface,"lsal",MSG_INTERFACE_INIT_QTEXT_SHOW,"QUEST_TEXT_ALT_NORMAL",pA,qnum);
			SetCurrentNode("QUEST_TEXT_ALT_NORMAL");
		break;
		case 1:
			SendMessage(&GameInterface,"lsal",MSG_INTERFACE_INIT_QTEXT_SHOW,"QUEST_TEXT",pA,qnum);
			SetCurrentNode("QUEST_TEXT");
		break;
		case 2:
			SendMessage(&GameInterface,"lsal",MSG_INTERFACE_INIT_QTEXT_SHOW,"QUEST_TEXT_ALT",pA,qnum);
			SetCurrentNode("QUEST_TEXT_ALT");
		break;
	}
}

void BackToTitle()
{
	XI_SetQuestData(true);
}

void XI_QuestActivate()
{
	int aq = curQuestTop+GetEventData();
	aref pA;
	makearef(pA,pchar.TmpQuestInfo);
	XI_SetQuestData(false);
	SetQTextShow(pA,aq);
	switch (sti(InterfaceStates.AltFont))
	{
		case 0: SetCurrentNode("QUEST_TEXT_ALT_NORMAL"); break;
		case 1: SetCurrentNode("QUEST_TEXT"); break;
		case 2: SetCurrentNode("QUEST_TEXT_ALT"); break;
	}
}

void XI_SetScroller(float pos)
{
	if (GetCurrentNode()=="QUEST_TITLE" || GetCurrentNode() == "QUEST_TITLE_ALT") SendMessage(&GameInterface,"lsf",MSG_INTERFACE_SET_SCROLLER,"QUESTSCROLL",pos);
    else SendMessage(&GameInterface,"lsf",MSG_INTERFACE_SET_SCROLLER,"QUESTSCROLL2",pos);
}

void SetScrollerPos()
{
	string nodName = GetEventData();
	float pos = GetEventData();
	XI_SetScroller(pos);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("QuestTopChange","QuestTopChange");
	DelEventHandler("BackToTitle","BackToTitle");
	DelEventHandler("QuestActivate","XI_QuestActivate");
	DelEventHandler("SetScrollerPos","SetScrollerPos");
	DelEventHandler("ScrollPosChange","ProcScrollPosChange");
	DelEventHandler("RibbonTopChange","ProcRibbonTopChange"); // LEO: снимаем вместе с окном - иначе накапливается при каждом открытии
	DelEventHandler("ScrollTopChange","ProcScrollChange");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("eTabControlPress","procTabChange");
	DelEventHandler("CheckButtonChange", "ProcessFilter");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
 	DelEventHandler("QuestDeActivate","QuestDeActivate");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ShowPGGInfo","ShowPGGInfo");
	DelEventHandler("ShowShipsWindow","ShowShipsWindow");
	DelEventHandler("ShowBladeWindow","ShowBladeWindow");
	DelEventHandler("ShowGunWindow","ShowGunWindow");
	DelEventHandler("ShowFoodWindow","ShowFoodWindow");
	DelEventHandler("ShowBonusItemWindow","ShowBonusItemWindow");
	DelEventHandler("ShowQuestsWindow","ShowQuestsWindow");
	DelEventHandler("PointsEX", "PointsEX");
	DelEventHandler("HideEncWindows", "HideEncWindows");
	DelEventHandler("ievntCreateNotice","LaunchCreateNoticeFromBook"); // Gregg
	SetEventHandler("OnTableClick", "OnTableClick", 0);

	QuestBookExitModelViewer();

	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"ReloadMenuExit"))
	{
		DeleteAttribute(&InterfaceStates,"ReloadMenuExit");
		EndCancelInterface(false);
	}
	else
	{
		EndCancelInterface(true);
	}
}
void QuestDeActivate()
{
	XI_SetQuestData(true);
}

// Gregg: выход из судового журнала в интерфейс заметок/планировщика (кнопка "Заметка в журнал")
void LaunchCreateNoticeFromBook()
{
	IDoExit(RC_INTERFACE_TO_CREATENOTICE);
}

// LEO: лента прокрутилась - у достижений прогресс-бары привязаны к МЕСТУ в списке, а не к строке,
//   поэтому их значения надо пересчитать под новую верхнюю строку. Раньше это делалось только при
//   смене выделения, и при прокрутке колесом бары оставались от прежних строк.
void ProcRibbonTopChange()
{
	string sNode = GetEventData();
	int nTop = GetEventData();
	if (sNode == "TABLE_ACHIEVEMENTS") fill_bars();
}

void ProcScrollPosChange()
{
	float newPos = GetEventData();
	string scrollName = GetEventData();

	if( GetSelectable("QUEST_TITLE") || GetSelectable("QUEST_TITLE_ALT"))
	{
		aref arefTmp;
		makearef(arefTmp,pchar.TmpQuestInfo);
		int maxVal = GetAttributesNum(arefTmp);
		int newTop = makeint(newPos*maxVal);

		if(newTop!=curQuestTop && scrollName == "QUESTSCROLL")
		{
			curQuestTop=newTop;
			if (InterfaceStates.AltFont == "0")	XI_SetQuestTitles("QUEST_TITLE_ALT",arefTmp,curQuestTop);
			else XI_SetQuestTitles("QUEST_TITLE",arefTmp,curQuestTop);
		}
	}
	else
	{
		switch (sti(InterfaceStates.AltFont))
		{
			case 0: SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"QUEST_TEXT_ALT_NORMAL", 1,newPos); break;
			case 1: SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"QUEST_TEXT", 1,newPos); break;
			case 2: SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"QUEST_TEXT_ALT", 1,newPos); break;
		}
	}
}

void ProcScrollChange()
{
	int changeNum = GetEventData();
	if(changeNum==0) return;
	string controlNode = "";
	if (InterfaceStates.AltFont == "0")
	{
		if( GetSelectable("QUEST_TITLE_ALT") ) controlNode = "QUEST_TITLE_ALT";
	}
	else
	{
		if( GetSelectable("QUEST_TITLE") ) controlNode = "QUEST_TITLE";
	}
	switch (sti(InterfaceStates.AltFont))
	{
		case 0: if( GetSelectable("QUEST_TEXT_ALT_NORMAL") ) controlNode = "QUEST_TEXT_ALT_NORMAL"; break;
		case 1: if( GetSelectable("QUEST_TEXT") ) controlNode = "QUEST_TEXT"; break;
		case 2: if( GetSelectable("QUEST_TEXT_ALT") ) controlNode = "QUEST_TEXT_ALT"; break;
	}

	if(controlNode!="")
	{
		if(changeNum>0) {
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,controlNode,-1, 0,ACTION_DOWNSTEP);
		} else {
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,controlNode,-1, 0,ACTION_UPSTEP);
		}
	}
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	switch(nodName)
	{
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
		case "SHIP_TABLE_LIST_LEFT":
			if(comName=="dblclick")
			{
				QuestBookShowModelViewer("ENC_INFO_SHIP", "LEFT");
			}
		break;
		case "SHIP_TABLE_LIST_RIGHT":
			if(comName=="dblclick")
			{
				QuestBookShowModelViewer("ENC_INFO_SHIP", "RIGHT");
			}
		break;
		case "BLADE_TABLE_LIST":
			if(comName=="dblclick")
			{
				QuestBookShowModelViewer("ENC_INFO_BLADE", "");
			}
		break;
		case "GUN_TABLE_LIST":
			if(comName=="dblclick")
			{
				QuestBookShowModelViewer("ENC_INFO_GUN", "");
			}
		break;
		case "MODEL_VIEWER_BUTTON_CANCEL":
			if(comName=="click")
			{
				QuestBookExitModelViewer();
			}
		break;
		case "BONUSITEM_TABLE_LIST":
			if(comName=="dblclick" && bBettaTestMode)
			{
				AddItems(pchar, Items[sti(GameInterface.(CurTable).(CurRow).index)].id, 1);
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

void selectJournal(int iMode)
{
	//Boyer fix for .complete sometimes missing
	string sAttComplete = "";

	HideStatistic();
	HideCashBook();
	HideShipPlace();
	HideStoreBook();
	HideInfoWindowEncy();
	HideAchievements();

	aref arQuestInfo, arTmp, newAttr;
	int  i, j;
	string attributeName;
	bool   ok;

	makearef(arQuestInfo, pchar.QuestInfo);
	DeleteAttribute(pchar, "TmpQuestInfo");
	pchar.TmpQuestInfo = "";

	int iQNum = GetAttributesNum(arQuestInfo);
	int iQIdx;

	// буфер под сортировку: имя записи и её вес
	string qNames[500];
	int qScores[500];
	int qCount = 0;

	for(i = 0; i < iQNum; i++)
	{
		iQIdx = i;
		if (iMode == 2) iQIdx = iQNum - 1 - i; // архив: берём с конца - последний закрытый идёт вверх списка

		arTmp = GetAttributeN(arQuestInfo, iQIdx);
		attributeName = GetAttributeName(arTmp);

		sAttComplete = "QuestInfo." + attributeName + ".Complete";
		ok = false;
		switch (iMode)
		{
			case 1:
				if (!CheckAttribute(pchar, "QuestInfo." + attributeName + ".InfoType") && CheckAttribute(pchar, sAttComplete) && sti(pchar.QuestInfo.(attributeName).Complete) == false)
				{
					ok = true;
				}
			break;

			case 2:
				if (!CheckAttribute(pchar, "QuestInfo." + attributeName + ".InfoType") && CheckAttribute(pchar, sAttComplete) && sti(pchar.QuestInfo.(attributeName).Complete) == true)
				{
					ok = true;
					// LEO: метки сортировки активного журнала архивной записи не нужны - снимает их
					//   CloseQuestHeader, а здесь чистим партии, где они успели налипнуть раньше.
					DeleteAttribute(pchar, "QuestInfo." + attributeName + ".SortIndex");
					DeleteAttribute(pchar, "QuestInfo." + attributeName + ".ColorStamped");
				}
			break;

			case 3:
				if (CheckAttribute(pchar, "QuestInfo." + attributeName + ".InfoType"))
				{
					ok = true;
				}
			break;
		}

		// копим отобранные записи, сортировка ниже
		if (ok && qCount < 500)
		{ 
			// базовый вес - ванильный порядок: чем новее запись, тем выше
			int score = 1000 - i; 

			if (iMode == 1) // порядок правим только в журнале активных
			{
				if (CheckAttribute(arTmp, "color")) 
				{
					// запись светится (непрочитана) - выдаём ей свежий номер очереди
					if (!CheckAttribute(arTmp, "ColorStamped")) 
					{
						if (!CheckAttribute(pchar, "QuestSortCounter")) pchar.QuestSortCounter = 10000;
						pchar.QuestSortCounter = sti(pchar.QuestSortCounter) + 1;
						pchar.QuestInfo.(attributeName).SortIndex = pchar.QuestSortCounter;
						pchar.QuestInfo.(attributeName).ColorStamped = true;
					}
					// непрочитанные всегда выше прочитанных
					score = 1000000 + sti(pchar.QuestInfo.(attributeName).SortIndex);
				}
				else 
				{
					// прочитали - снимаем отметку, чтобы следующий засвет выдал новый номер
					if (CheckAttribute(pchar, "QuestInfo." + attributeName + ".ColorStamped")) 
					{
						DeleteAttribute(pchar, "QuestInfo." + attributeName + ".ColorStamped");
					}

					// номер очереди оставляем: порядок недавних записей не скачет
					if (CheckAttribute(arTmp, "SortIndex")) 
					{
						score = sti(arTmp.SortIndex);
					}
				}
			}

			qNames[qCount] = attributeName;
			qScores[qCount] = score;
			qCount++;
		}
	}

	// сортировка по убыванию веса. LEO: с выходом по «обменов не было» - в журнале без непрочитанных
	//   записей порядок уже верный, и проход получается один вместо квадрата от числа записей.
	bool bSwapped = true;
	for (i = 0; i < qCount - 1; i++)
	{
		if (!bSwapped) break;
		bSwapped = false;
		for (j = 0; j < qCount - i - 1; j++)
		{
			if (qScores[j] < qScores[j + 1])
			{
				int tmpScore = qScores[j];
				qScores[j] = qScores[j + 1];
				qScores[j + 1] = tmpScore;

				string tmpName = qNames[j];
				qNames[j] = qNames[j + 1];
				qNames[j + 1] = tmpName;
				bSwapped = true;
			}
		}
	}

	// готовый порядок - во временную ветку, из неё рисует движок
	for (i = 0; i < qCount; i++)
	{
		attributeName = qNames[i];
		pchar.TmpQuestInfo.(attributeName) = "";
		makearef(newAttr, pchar.TmpQuestInfo.(attributeName));
		aref srcAttr; makearef(srcAttr, pchar.QuestInfo.(attributeName));
		CopyAttributes(newAttr, srcAttr);
	}

	XI_SetQuestData(true);
}

void selectStatistic()
{
	SetNodeUsing("TABLE_SHIPCLASS",true);
	SetNodeUsing("TABLE_HUMAN",true);
	SetNodeUsing("TABLE_NATION",true);
	SetNodeUsing("TABLE_OTHER",true);
	SetNodeUsing("TABLE_MONEY",true);
	HideCashBook();
	HideQuests();
	HideShipPlace();
	HideStoreBook();
	HideAchievements();
	ShowButtons();
	CreateString(true, "LevelComplexity", XI_ConvertString("m_Complexity") + " : " + GetLevelComplexity(MOD_SKILL_ENEMY_RATE), "interface_ultrasmall", COLOR_NORMAL, 440, 338, SCRIPT_ALIGN_CENTER, 1.5);
}

void selectCashBook()
{
	HideStatistic();
	HideQuests();
	HideShipPlace();
	HideStoreBook();
	HideAchievements();
	if (g_iCashBookMode == 1)
	{
		// LEO: режим "Выручка с торгов" - левая таблица показывает выручку аукциона по городам, правая (вклады) скрыта
		FillAuctionVaultTable();
		SetNodeUsing("SCROLL_CREDIT",true);
		SetNodeUsing("TABLE_CREDIT",true);
		SetNodeUsing("SCROLL_DEBIT",false);
		SetNodeUsing("TABLE_DEBIT",false);
	}
	else
	{
		FillCreditTable(); // вернуть кредиты (могли быть затёрты режимом выручки)
		SetNodeUsing("SCROLL_DEBIT",true);
		SetNodeUsing("TABLE_DEBIT",true);
		SetNodeUsing("SCROLL_CREDIT",true);
		SetNodeUsing("TABLE_CREDIT",true);
	}
	ShowButtons();
}

void selectAchievements() // Отображаем окно достижений
{
	XI_WindowDisable("ENC_INFO_ACHIEVEMENTS",false);
	SetNodeUsing("I_CHARACTER_2",false);
	SetNodeUsing("I_CHARACTER",false);
	SetNodeUsing("I_SHIP_2",false);
	SetNodeUsing("I_SHIP",false);
	SetNodeUsing("I_NATIONS_2",false);
	SetNodeUsing("I_NATIONS",false);
	SetNodeUsing("I_TRADEBOOK_2",false);
	SetNodeUsing("I_TRADEBOOK",false);
	SetNodeUsing("I_ITEMS_2",false);
	SetNodeUsing("I_ITEMS",false);

	rownumberach = 0;
	HideCashBook();
	HideQuests();
	HideStatistic();
	HideQuestTabs();
	ShowStr(false);

	CheckAndUnlock("money", sti(pchar.Money));

	SetNodeUsing("ACHIEVEMENTS_INFO_FADER",true);
	SetNodeUsing("ACHIEVEMENTS_INFO_DUST",true);
	SetNodeUsing("ACHIEVEMENTS_INFO_FRAME",true);
	SetNodeUsing("TABLE_ACHIEVEMENTS",true);
	SetNodeUsing("ACH_COMPLETE_SUM",true);
	SetNodeUsing("SCROLL_ACHIEVEMENTS",true);
	SetNodeUsing("BAR_str1",true);
	SetNodeUsing("BAR_str2",true);
	SetNodeUsing("BAR_str3",true);
	SetNodeUsing("BAR_str4",true);
	SetNodeUsing("BAR_str5",true);
	SetNodeUsing("BAR_str6",true);
	SetNodeUsing("BAR_str7",true);

	SetNodeUsing("SHIP_INFO_FADER",false);
	SetNodeUsing("SHIP_INFO_DUST",false);
	SetNodeUsing("SHIP_INFO_FRAME",false);
	SetNodeUsing("SHIP_INFO_FRAME_CAPTION",false);
	SetNodeUsing("SHIP_TABLE_LIST_LEFT",false);
	SetNodeUsing("SHIP_TABLE_LIST_RIGHT",false);
	SetNodeUsing("SHIP_TABLE_SCROLL_LEFT",false);
	SetNodeUsing("SHIP_TABLE_SCROLL_RIGHT",false);
	SetNodeUsing("SHIP_TABLE_OTHER_LEFT",false);
	SetNodeUsing("SHIP_TABLE_OTHER_RIGHT",false);
	SetNodeUsing("SHIP_INFO_FRAME_TEXT_LEFT",false);
	SetNodeUsing("SHIP_INFO_FRAME_TEXT_RIGHT",false);
	SetNodeUsing("SHIP_INFO_TEXT_LEFT",false);
	SetNodeUsing("SHIP_INFO_TEXT_RIGHT",false);
	SetNodeUsing("SHIP_CLASS_LEFT",false);
	SetNodeUsing("SHIP_NATION_LEFT",false);
	SetNodeUsing("SHIP_CLASS_RIGHT",false);
	SetNodeUsing("SHIP_NATION_RIGHT",false);
	SetNewPicture("S_NATION_E_L", "");
	SetNewPicture("S_NATION_F_L", "");
	SetNewPicture("S_NATION_H_L", "");
	SetNewPicture("S_NATION_S_L", "");
	SetNewPicture("S_NATION_P_L", "");
	SetNewPicture("S_NATION_E_R", "");
	SetNewPicture("S_NATION_F_R", "");
	SetNewPicture("S_NATION_H_R", "");
	SetNewPicture("S_NATION_S_R", "");
	SetNewPicture("S_NATION_P_R", "");
	SetNodeUsing("BLADE_INFO_FADER",false);
	SetNodeUsing("BLADE_INFO_DUST",false);
	SetNodeUsing("BLADE_INFO_FRAME",false);
	SetNodeUsing("BLADE_INFO_FRAME_CAPTION",false);
	SetNodeUsing("BLADE_TABLE_LIST_SCROLL",false);
	SetNodeUsing("BLADE_TABLE_LIST",false);
	SetNodeUsing("BLADE_TYPE_CLASS",false);
	SetNodeUsing("BLADE_TYPE_Quality",false);
	SetNodeUsing("GUN_INFO_FADER",false);
	SetNodeUsing("GUN_INFO_DUST",false);
	SetNodeUsing("GUN_INFO_FRAME",false);
	SetNodeUsing("GUN_INFO_FRAME_CAPTION",false);
	SetNodeUsing("GUN_TABLE_LIST_SCROLL",false);
	SetNodeUsing("GUN_TABLE_LIST",false);
	SetNodeUsing("GUN_TYPE_QUALITY",false);
	SetNodeUsing("FOOD_INFO_FADER",false);
	SetNodeUsing("FOOD_INFO_DUST",false);
	SetNodeUsing("FOOD_INFO_FRAME",false);
	SetNodeUsing("FOOD_INFO_FRAME_CAPTION",false);
	SetNodeUsing("FOOD_TABLE_LIST_SCROLL",false);
	SetNodeUsing("FOOD_TABLE_LIST",false);
	SetNodeUsing("BONUSITEM_INFO_FADER",false);
	SetNodeUsing("BONUSITEM_INFO_DUST",false);
	SetNodeUsing("BONUSITEM_INFO_FRAME",false);
	SetNodeUsing("BONUSITEM_INFO_FRAME_CAPTION",false);
	SetNodeUsing("BONUSITEM_TABLE_LIST_SCROLL",false);
	SetNodeUsing("BONUSITEM_TABLE_LIST",false);
	SetNodeUsing("BONUSITEM_CLASS",false);
	SetNodeUsing("QUESTS_INFO_FADER",false);
	SetNodeUsing("QUESTS_INFO_DUST",false);

	FillATableHeader();
	FillATableInfo();

	SetNodeUsing("ACHIEVEMENTS_STR",true);
	SetNodeUsing("POINTS_EXCHANGE",true);
	SetNodeUsing("ACHIEVEMENTS_INFO_FRAME_CAPTION",true);
	SetNodeUsing("ACHIEVEMENTS_INFO_FADER",true);
	SetNodeUsing("ACHIEVEMENTS_INFO_DUST",true);

	SetFormatedText("ACHIEVEMENTS_INFO_FRAME_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_4", "Interface.txt", "#space#", " "));
	SetFormatedText("ACHIEVEMENTS_STR", GetConvertStrWithReplace("Variable_QuestBook_5", "Interface.txt", "#space#", " ") + sti(pchar.ach_points)); // Очки достижений

	SetNodeUsing("EXIT_ENC_BTN",true);
	SetNodeUsing("EXIT_BTN",false);
}

void selectShipPlace()
{
	SetNodeUsing("TABLE_SHIP_PLACE",true);
	SetNodeUsing("SCROLL_SHIP_PLACE",true);
	HideStatistic();
	HideQuests();
	HideCashBook();
	HideStoreBook();
	ShowButtons();
}

void selectStoreBook()
{
	SetNodeUsing("TABLE_CITY",true);
	SetNodeUsing("SCROLL_CITY",true);
	SetNodeUsing("TABLE_GOODS",true);
	SetNodeUsing("SCROLL_GOODS",true);
	HideStatistic();
	HideQuests();
	HideCashBook();
	HideShipPlace();
	ShowButtons();
}

void HideInfoWindow()
{
	SetNewPicture("SHIP_PICTURE_L", "None");
	SetNewPicture("SHIP_PICTURE_R", "None");
	SetNodeUsing("SHIP_PICTURE_VL",false);
	SetNodeUsing("SHIP_PICTURE_VR",false);
	curselectedshipl = 0;
	curselectedshipr = 0;
	CloseTooltip();
	ExitRPGHint();
}

bool opened = false;

void ShowPGGInfo()
{
	if (opened) return;
	if (CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".index") && GameInterface.(CurTable).(CurRow).td4.str != GetConvertStrWithReplace("Variable_QuestBook_6", "Interface.txt", "#space#", " "))
	{ // нет ПГГ в списке
		ref chrefspp = CharacterFromID(GameInterface.(CurTable).(CurRow).index);
		ref refBaseShip = GetRealShip(sti(chrefspp.ship.type));
		string shipTexture = refBaseShip.BaseName;
		SetFormatedText("OFFICERS_WINDOW_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_7", "Interface.txt", "#space#", " "));
		SetFormatedText("SHIP_NAME", XI_ConvertString(RealShips[sti(chrefspp.Ship.Type)].BaseName) + " '" + chrefspp.Ship.Name + "'");
		SetFormatedText("SHIP_WINDOW_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_8", "Interface.txt", "#space#", " "));
		SetFormatedText("PORT_INFO_PLACE", "Остров/колония:\n"+GameInterface.(CurTable).(CurRow).td7.str+" / "+GameInterface.(CurTable).(CurRow).td6.str);
		if (g_iShipPlaceMode == 0) // СТОЯНКА
		{
			SetFormatedText("SHIP_INFO_PLACE", GetConvertStrWithReplace("Variable_QuestBook_9", "Interface.txt", "#space#", " "));
			SetFormatedText("DATE_INFO_PLACE", "Дата стоянки:\n"+GameInterface.(CurTable).(CurRow).td8.str);
			SetFormatedText("MONEY_INFO_PLACE", "Цена в месяц:\n"+GameInterface.(CurTable).(CurRow).td9.str+GetConvertStrWithReplace("Variable_QuestBook_10", "Interface.txt", "#space#", " "));
		}
		else // АУКЦИОН - другие подписи (LEO: не "стоянка"/"цена в месяц")
		{
			SetFormatedText("SHIP_INFO_PLACE", XI_ConvertString("PortAuction_InfoHdr"));
			SetFormatedText("DATE_INFO_PLACE", XI_ConvertString("PortAuction_InfoDays")+"\n"+GameInterface.(CurTable).(CurRow).td8.str);
			SetFormatedText("MONEY_INFO_PLACE", XI_ConvertString("PortAuction_InfoGain")+"\n"+GameInterface.(CurTable).(CurRow).td9.str+GetConvertStrWithReplace("Variable_QuestBook_10", "Interface.txt", "#space#", " "));
		}
		SetFormatedText("SHIP_RANK", refBaseShip.Class);
		SetFormatedText("CLASS_ARMOR", refBaseShip.HullArmor);
		if (!CheckAttribute(refBaseShip,"Tuning.HullArmor")) SetNewGroupPicture("CLASS_ARMOR_ICON", "ICONS_SPEC", "Normal");
		else SetNewGroupPicture("CLASS_ARMOR_ICON", "ICONS_SPEC", "Upgraded");

		SetNodeUsing("SHIP_BIG_PICTURE_VIDEO",false);
		SetNewPicture("SHIP_BIG_PICTURE", InterfaceStates.InterfaceShipString + shipTexture + ".dds");

		opened = true;
		SetNewPicture("SHIP_FRAME_PICTURE", "interfaces\Frame1.dds");
		string texturedata;
		SetNewPicture("CHARACTER_PROFESSION", "INTERFACES\Sith\Char_"+GetSpeciality(chrefspp)+".dds");
		string portpic;
		switch (GameInterface.(CurTable).(CurRow).td6.icon.image)
		{
			case "France": portpic = "loading\enc_fra.dds";
			break;
			case "England": portpic = "loading\enc_eng.dds";
			break;
			case "Spain": portpic = "loading\enc_spa.dds";
			break;
			case "Holland": portpic = "loading\enc_hol.dds";
			break;
			case "Pirate": portpic = "loading\enc_pir.dds";
			break;
		}
		SetNewPicture("NATION_PORT_PICTURE", portpic);
		SetShipOTHERTable("TABLE_OTHERS",chrefspp);
		SetShipQualityTable(chrefspp, "BAR_HP", "BAR_Mast", "BAR_Speed", "BAR_TurnRate", "BAR_WAS", "BAR_Capacity", "BAR_Crew");
		SetSPECIALMiniTable("TABLE_SMALLSKILL", chrefspp);
		SetOTHERMiniTable("TABLE_SMALLOTHER", chrefspp);
		string offtype;
		if (!CheckAttribute(chrefspp,"quest.officertype")) offtype = "\nСпециальность: Универсал";
		else offtype = "\nСпециальность: "+XI_ConvertString(chrefspp.quest.officertype);
		SetFormatedText("OFFICER_NAME", GetFullName(chrefspp)+offtype);
		SetNewPicture("CHARACTER_BIG_PICTURE", "interfaces\portraits\face_" + chrefspp.faceId + ".tga");
		SetNewPicture("CHARACTER_FRAME_PICTURE", "interfaces\Frame2.dds");

		if (CheckAttribute(RealShips[sti(chrefspp.Ship.Type)],"Upgrade")) SetUpgradesNodes(RealShips[sti(chrefspp.Ship.Type)],RealShips[sti(chrefspp.Ship.Type)],false);
		else DisableUpgradeNodes(RealShips[sti(chrefspp.Ship.Type)],RealShips[sti(chrefspp.Ship.Type)],false);

		if (bHalfImmortalPGG)
		{
			if (CheckAttribute(chrefspp, "ImmortalOfficer"))
			{
				SetNodeUsing("CONTRACT",true);
				SetNewPicture("CONTRACT", "interfaces\FaqPictures\CONTRACT.dds");
				SetNodeUsing("CONTRACT_TEXT",true);
				SetFormatedText("CONTRACT_TEXT",GetConvertStrWithReplace("Variable_QuestBook_11", "Interface.txt", "#space#", " "));
			}
			else
			{
				SetNodeUsing("CONTRACT",false);
				SetFormatedText("CONTRACT_TEXT","");
			}
		}
		else
		{
			if (CheckAttribute(chrefspp, "halfimmortal"))
			{
				SetNodeUsing("CONTRACT",true);
				SetNewPicture("CONTRACT", "interfaces\FaqPictures\CONTRACT.dds");
				SetNodeUsing("CONTRACT_TEXT",true);
				SetFormatedText("CONTRACT_TEXT",GetConvertStrWithReplace("Variable_QuestBook_12", "Interface.txt", "#space#", " "));
			}
			else
			{
				SetNodeUsing("CONTRACT",false);
				SetFormatedText("CONTRACT_TEXT","");
			}
		}

		XI_WindowShow("SHOW_ADV_INFO", true);
		XI_WindowDisable("SHOW_ADV_INFO", false);
		sMessageMode = "RPG_Hint";
	}
	if (CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".index") && GameInterface.(CurTable).(CurRow).td4.str == GetConvertStrWithReplace("Variable_QuestBook_13", "Interface.txt", "#space#", " "))
	{
		ref chrefspp1 = CharacterFromID(GameInterface.(CurTable).(CurRow).index);
		SetFormatedText("OFFICERS_WINDOW_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_14", "Interface.txt", "#space#", " "));
		SetNewPicture("CHARACTER_PROFESSION", "INTERFACES\Sith\Char_"+GetSpeciality(chrefspp1)+".dds");
		SetSPECIALMiniTable("TABLE_SMALLSKILL", chrefspp1);
		SetOTHERMiniTable("TABLE_SMALLOTHER", chrefspp1);
		string offtype1;
		if (!CheckAttribute(chrefspp1,"quest.officertype")) offtype1 = "\nСпециальность: Универсал";
		else offtype1 = "\nСпециальность: "+XI_ConvertString(chrefspp1.quest.officertype);
		SetFormatedText("OFFICER_NAME", GetFullName(chrefspp1)+offtype1);
		SetNewPicture("CHARACTER_BIG_PICTURE", "interfaces\portraits\face_" + chrefspp1.faceId + ".tga");
		SetNewPicture("CHARACTER_FRAME_PICTURE", "interfaces\Frame2.dds");
		if (bHalfImmortalPGG)
		{
			if (CheckAttribute(chrefspp1, "ImmortalOfficer"))
			{
				SetNodeUsing("CONTRACT",true);
				SetNewPicture("CONTRACT", "interfaces\FaqPictures\CONTRACT.dds");
				SetNodeUsing("CONTRACT_TEXT",true);
				SetFormatedText("CONTRACT_TEXT",GetConvertStrWithReplace("Variable_QuestBook_15", "Interface.txt", "#space#", " "));
			}
			else
			{
				SetNodeUsing("CONTRACT",false);
				SetFormatedText("CONTRACT_TEXT","");
			}
		}
		else
		{
			if (CheckAttribute(chrefspp1, "halfimmortal"))
			{
				SetNodeUsing("CONTRACT",true);
				SetNewPicture("CONTRACT", "interfaces\FaqPictures\CONTRACT.dds");
				SetNodeUsing("CONTRACT_TEXT",true);
				SetFormatedText("CONTRACT_TEXT",GetConvertStrWithReplace("Variable_QuestBook_16", "Interface.txt", "#space#", " "));
			}
			else
			{
				SetNodeUsing("CONTRACT",false);
				SetFormatedText("CONTRACT_TEXT","");
			}
		}

		XI_WindowShow("SHOW_ADV_INFO_SHORT", true);
		XI_WindowDisable("SHOW_ADV_INFO_SHORT", false);
		sMessageMode = "RPG_Hint";
	}
}

// Обмен очков достижений
void PointsEX() {
	IDoExit(RC_INTERFACE_POINTS_EX);
}

void FillQuestsHeader() // Заполним заголовок таблицы квестов
{
    GameInterface.QUESTS_TABLE_LIST.hr.td1.str = GetConvertStr("Variable_quest_table_quest", "Interface.txt");
	GameInterface.QUESTS_TABLE_LIST.hr.td1.scale = 0.9;
	GameInterface.QUESTS_TABLE_LIST.hr.td2.str = GetConvertStr("Variable_quest_table_rank", "Interface.txt");
	GameInterface.QUESTS_TABLE_LIST.hr.td2.scale = 0.9;
	GameInterface.QUESTS_TABLE_LIST.hr.td3.str = GetConvertStr("Variable_quest_table_desc", "Interface.txt");
	GameInterface.QUESTS_TABLE_LIST.hr.td3.scale = 0.9;
	GameInterface.QUESTS_TABLE_LIST.hr.td4.str = GetConvertStr("Variable_quest_table_spec", "Interface.txt");
	GameInterface.QUESTS_TABLE_LIST.hr.td4.scale = 0.9;
	GameInterface.QUESTS_TABLE_LIST.hr.td5.str = GetConvertStr("Variable_quest_table_complete", "Interface.txt");
	GameInterface.QUESTS_TABLE_LIST.hr.td5.scale = 0.9;
	GameInterface.QUESTS_TABLE_LIST.select = 0;
//--> mod tablesort
	GameInterface.QUESTS_TABLE_LIST.hr.td1.sorttype = "string";
	GameInterface.QUESTS_TABLE_LIST.hr.td2.sorttype = "";//числа - любое значение кроме "string"
	GameInterface.QUESTS_TABLE_LIST.hr.td3.sorttype = "string";
	GameInterface.QUESTS_TABLE_LIST.hr.td4.sorttype = "";
//<-- mod tablesort
}

// LEO: ключ квеста в журнале (pchar.QuestInfo) по ключу энциклопедии (pchar.questdata) - совпадают
//   они далеко не всегда. Раньше сверялись РУССКИЕ НАЗВАНИЯ квестов: в английской версии не срабатывало
//   ни одно сравнение и фильтр «Активные» оставался пустым. Пустая строка = отдельного ключа нет,
//   хватает общей проверки по имени.
string QB_JournalKeyByQuest(string sQuest)
{
	switch (sQuest)
	{
		case "Aptekar": return "PDM_Aptekar"; break;   // Аптекарь
		case "Ascold": return "Ascold"; break;   // Аскольд
		case "CaptainMonkey": return "CaptainMonkey"; break;   // Корабль обезьян
		case "ClanLambrini": return "PDM_Clan_Lambrini"; break;   // Клан Ламбрини
		case "DengiRost": return "KSM_DVR"; break;   // Деньги в рост
		case "DisappearPeople": return "DisappearPeople"; break;   // Исчезновение людей
		case "Dozor": return "Dozor"; break;   // Дозор
		case "FleitOrion": return "Andre_Abel_Quest"; break;   // Сопровождение флейта Орион
		case "HelpSacredInquisition": return "HelpSacredInquisition"; break;   // Помощь Святой Инквизиции
		case "IstoriaDavneyDrujby": return "Silence_Price"; break;   // История давней дружбы
		case "IstoriyaIzabely": return "Romantic_Line"; break;   // История прекрасной Изабеллы
		case "KoltsoLodet": return "PDM_Poteryanoe_Koltso"; break;   // Кольцо Жозефины Лодет
		case "Kondotier": return "Headhunt"; break;   // Кондотьер
		case "KorablPrizrak": return "GhostShipQuest"; break;   // Корабль-призрак
		case "KotVMeshke": return "CSP_KvM"; break;   // Кот в мешке
		case "Kuali": return "Kuali"; break;   // Макуауитль Куали
		case "Looser": return "KIP_Looser"; break;   // Проигравшийся игрок
		case "MoonStone": return "MoonStone"; break;   // Легенда о призрачном свете
		case "NaitiMushket": return "SeekDoubleMushket"; break;   // Найти двуствольный мушкет
		case "NaitiTorgovtsa": return "SCQ_womanHasband"; break;   // Найти пропавшего торговца
		case "Nasledstvo": return "UnexpectedInheritance"; break;   // Нежданное наследство
		case "NeputyovyKaznachey": return "PDM_Neputyovy_kaznachey"; break;   // Непутёвый казначей
		case "NovayaRodina": return "PDM_Novaya_Rodina"; break;   // В поисках новой родины
		case "OhotaNaVedmu": return "PDM_Ohota_na_vedmu"; break;   // Охота на ведьму
		case "OpasnyGruz": return "zpq"; break;   // Опасный груз
		case "OskolokProshlogo": return "pirateVikingQuest"; break;   // Осколок прошлого
		case "Ostin": return "BerglarsGang"; break;   // Дело жизни и смерти
		case "PomoshElen": return "PomoshElen"; break;   // Помощь Элен
		case "ProklyatayaJara": return "PDM_Proklyataya_Jara"; break;   // Проклятая жара
		case "ProklyatyIdol": return "PDM_Cursed_Idol"; break;   // Проклятый идол
		case "Rabotorgovets": return "Slavetrader"; break;   // Работорговец
		case "Rimalie": return "Rimalie"; break;   // Рималье
		case "Satanisty": return "PKM_Animists"; break;   // Странные вещи творятся на архипелаге
		case "ShamankaChani": return "ShamanChani"; break;   // Шаманка Чани
		case "SilverFleet": return "SilverFleet"; break;   // Серебряный флот
		case "SinyaPtitsa": return "Xebeca_BlueBird"; break;   // Синяя птица
		case "SpasenieNaRifah": return "KSM_Spasenie_na_rifah"; break;   // Чудесное спасение на рифах
		case "SpastiMuja": return "SCQ_womanPirates"; break;   // Вызволение из плена мужа горожанки
		case "StartDamnedDestiny": return "StartDamnedDestiny"; break;   // Необитаемый остров
		case "StroitelstvoKolonii": return "ColonyBuilding"; break;   // Строительство колонии
		case "TohmasPetit": return "TohmasPetit"; break;   // Трусливый Томас
		case "TravlyaKrys": return "pSQ"; break;   // Травля крыс
		case "TreacheryOfGrifoSajfus": return "TreacheryOfGrifoSajfus"; break;   // Предательство Грифо Сайфуса
		case "TwoSisters": return "TwoSisters"; break;   // Две сестры
		case "ZacharovanyGorod": return "MagicCity"; break;   // Зачарованный город
		case "ZagadochnyOstrov": return "KPS_MysteriousIsland"; break;   // Загадочный остров
	}
	return "";
}

void FillQuests() // Заполним таблицу квестов информацией
{
	string sSelectedQuest = "";
	if (CheckAttribute(&GameInterface, "QUESTS_TABLE_LIST.select"))
	{
		if (sti(GameInterface.QUESTS_TABLE_LIST.select) > 0)
		{
			string sSelectedRow = "tr" + GameInterface.QUESTS_TABLE_LIST.select;
			if (CheckAttribute(&GameInterface, "QUESTS_TABLE_LIST." + sSelectedRow + ".index"))
				sSelectedQuest = GameInterface.QUESTS_TABLE_LIST.(sSelectedRow).index;
		}
	}
	Table_Clear("QUESTS_TABLE_LIST", false, true, false);

	string row, attrname, encyName, qKey, sQuestRank, sKeyAlt, sJournalKey;
	int n, num, z, q, iQuestRank, nColumn;
	aref aroot, arcur, arActiveQuests, qNode;
	bool bIsCompleted, bIsActive, bIsUnavailable, bIsAvailable, bShow, bQuestComplete;
	int iMonkeyRow = 0;
	int iCargoRow = 0;

	num = 0;
	if (CheckAttribute(pchar, "questdata"))
	{
		makearef(aroot, pchar.questdata);
		num = GetAttributesNum(aroot);
	}
	z = 1;

	for(n = 0; n < num; n++)
	{
		row = "tr" + z;
		arcur = GetAttributeN(aroot, n);
		attrname = GetAttributeName(arcur);

		// LEO: у сюжетных героев (starttype 1-8) свой пролог - квесту альтернативного тут не место
		if (attrname == "StartDamnedDestiny")
		{
			if (CheckAttribute(pchar, "starttype"))
			{
				if (sti(pchar.starttype) <= 8) continue;
			}
		}

		bIsCompleted = false;
		if (sti(pchar.questdata.(attrname)) == 1) 
		{
			bIsCompleted = true;
		}

		// фильтр «Не выполнено»: прячем пройденные
		if (FState_COMPLETE == 1)
		{
			if (bIsCompleted) continue;
		}

		// активен ли квест: ищем его незакрытую запись в журнале
		bIsActive = false;
		encyName = GetConvertStr(attrname, "Quests.txt");

		if (CheckAttribute(pchar, "questdata." + attrname))
		{
			if (sti(pchar.questdata.(attrname)) > 1)
			{
				bIsActive = true;
			}
		}

		// ключ журнала считаем ОДИН раз на квест, а не по разу на каждую запись журнала
		sJournalKey = QB_JournalKeyByQuest(attrname);

		if (bIsActive == false)
		{
			if (CheckAttribute(pchar, "QuestInfo"))
			{
				makearef(arActiveQuests, pchar.QuestInfo);
				for (q = 0; q < GetAttributesNum(arActiveQuests); q++)
				{
					qNode = GetAttributeN(arActiveQuests, q);
					qKey = GetAttributeName(qNode);

					bQuestComplete = false;
					if (CheckAttribute(qNode, "Complete"))
					{
						if (sti(qNode.Complete) == 1) 
						{
							bQuestComplete = true;
						}
					}

					if (bQuestComplete == false)
					{
						if (qKey == attrname) { bIsActive = true; break; }
						if (sJournalKey != "")
						{
							if (qKey == sJournalKey) { bIsActive = true; break; }
						}
						if (HasSubStr(attrname, "_quest"))
						{
							sKeyAlt = strreplace(attrname, "_quest", "");
							if (qKey == sKeyAlt) { bIsActive = true; break; }
						}
					}
				}
			}
		}

		// недоступен: не начат и ранг ГГ ниже требуемого
		bIsUnavailable = false;
		iQuestRank = 0;
		sQuestRank = GetConvertStr(attrname + "_Lvl", "Quests.txt");

		if (sQuestRank != "")
		{
			if (findsubstr(sQuestRank, "?", 0) == -1)
			{
				iQuestRank = sti(sQuestRank); 
			}
		}

		if (bIsCompleted == false)
		{
			if (bIsActive == false)
			{
				if (iQuestRank > 0)
				{
					if (sti(pchar.rank) < iQuestRank)
					{
						bIsUnavailable = true;
					}
				}
			}
		}

		// доступен: не пройден, не начат и по рангу проходит
		bIsAvailable = false;
		if (bIsCompleted == false)
		{
			if (bIsActive == false)
			{
				if (bIsUnavailable == false)
				{
					bIsAvailable = true;
				}
			}
		}

		if (FState_ACTIVE == 1 || FState_UNAVAILABLE == 1 || FState_AVAILABLE == 1)
		{
			bShow = false;
			if (FState_ACTIVE == 1)
			{
				if (bIsActive) bShow = true;
			}
			if (FState_AVAILABLE == 1)
			{
				if (bIsAvailable) bShow = true;
			}
			if (FState_UNAVAILABLE == 1)
			{
				if (bIsUnavailable) bShow = true;
			}

			if (bShow == false) continue;
		}

		if (attrname == "CaptainMonkey") iMonkeyRow = z;
		if (attrname == "OpasnyGruz") iCargoRow = z;
		GameInterface.QUESTS_TABLE_LIST.(row).index = attrname;
		GameInterface.QUESTS_TABLE_LIST.(row).td1.scale = 0.8;
		GameInterface.QUESTS_TABLE_LIST.(row).td2.scale = 0.9;
		GameInterface.QUESTS_TABLE_LIST.(row).td3.scale = 0.7;
		GameInterface.QUESTS_TABLE_LIST.(row).td4.scale = 0.7;

		GameInterface.QUESTS_TABLE_LIST.(row).td1.str = encyName; 
		GameInterface.QUESTS_TABLE_LIST.(row).td2.str = sQuestRank;
		GameInterface.QUESTS_TABLE_LIST.(row).td3.str = GetConvertStr(attrname+"_Desc", "Quests.txt");
		GameInterface.QUESTS_TABLE_LIST.(row).td4.str = GetConvertStr(attrname+"_Spec", "Quests.txt");

		GameInterface.QUESTS_TABLE_LIST.(row).td5.icon.width = 40;
		GameInterface.QUESTS_TABLE_LIST.(row).td5.icon.height = 40;
		GameInterface.QUESTS_TABLE_LIST.(row).td5.icon.offset = "18, -1";
		GameInterface.QUESTS_TABLE_LIST.(row).td5.icon.group = GetConvertStr(attrname+"_Icon", "Quests.txt");
		GameInterface.QUESTS_TABLE_LIST.(row).td5.icon.image = "quest_icons";

		GameInterface.QUESTS_TABLE_LIST.(row).td6.icon.width = 12;
		GameInterface.QUESTS_TABLE_LIST.(row).td6.icon.height = 12;
		GameInterface.QUESTS_TABLE_LIST.(row).td6.icon.offset = "-57, 15";
		GameInterface.QUESTS_TABLE_LIST.(row).td6.icon.group = "ICONS";

		if (bIsCompleted) 
		{
			GameInterface.QUESTS_TABLE_LIST.(row).td6.icon.image = "complete";
		}
		else 
		{
			GameInterface.QUESTS_TABLE_LIST.(row).td6.icon.image = "noncomplete";
		}

		z++;
	}

	// LEO: в старых сейвах ранг 12 записан после 13; меняем только две видимые строки, не questdata и не главы.
	if (iMonkeyRow > 0 && iCargoRow > iMonkeyRow)
	{
		object oQuestRow;
		aref aMonkeyRow, aCargoRow;
		string sMonkeyRow = "tr" + iMonkeyRow;
		string sCargoRow = "tr" + iCargoRow;
		makearef(aMonkeyRow, GameInterface.QUESTS_TABLE_LIST.(sMonkeyRow));
		makearef(aCargoRow, GameInterface.QUESTS_TABLE_LIST.(sCargoRow));
		CopyAttributes(&oQuestRow, aMonkeyRow);
		CopyAttributes(aMonkeyRow, aCargoRow);
		CopyAttributes(aCargoRow, &oQuestRow);
	}
	GameInterface.QUESTS_TABLE_LIST.select = 0;
	if (checkattribute(&GameInterface, "QUESTS_TABLE_LIST.hr.sortedColumn"))
	{
		nColumn = sti(GameInterface.QUESTS_TABLE_LIST.hr.sortedColumn);
		DeleteAttribute(&GameInterface, "QUESTS_TABLE_LIST.hr.sortedColumn");
		SortTable("QUESTS_TABLE_LIST", nColumn);
	}
	// LEO: выделение привязано к ID, а не к прежнему номеру строки; отфильтрованный квест не подменяем другим.
	if (sSelectedQuest != "")
	{
		for (n = 1; n < z; n++)
		{
			row = "tr" + n;
			if (GameInterface.QUESTS_TABLE_LIST.(row).index == sSelectedQuest)
			{
				GameInterface.QUESTS_TABLE_LIST.select = n;
				break;
			}
		}
	}
	if (CurTable == "QUESTS_TABLE_LIST")
	{
		CurRow = "";
		if (sti(GameInterface.QUESTS_TABLE_LIST.select) > 0) CurRow = "tr" + GameInterface.QUESTS_TABLE_LIST.select;
	}
	Table_UpdateWindow("QUESTS_TABLE_LIST");
}

void FillATableHeader() // Заполним заголовок таблицы достижений
{
    GameInterface.TABLE_ACHIEVEMENTS.hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_17", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ACHIEVEMENTS.hr.td1.scale = 0.9;
	GameInterface.TABLE_ACHIEVEMENTS.hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_18", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ACHIEVEMENTS.hr.td2.scale = 0.9;
	GameInterface.TABLE_ACHIEVEMENTS.hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_19", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ACHIEVEMENTS.hr.td3.scale = 0.9;
	GameInterface.TABLE_ACHIEVEMENTS.hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_20", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ACHIEVEMENTS.hr.td4.scale = 0.9;
	GameInterface.TABLE_ACHIEVEMENTS.hr.td5.str = GetConvertStrWithReplace("Variable_QuestBook_21", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_ACHIEVEMENTS.hr.td5.scale = 0.9;
	GameInterface.TABLE_ACHIEVEMENTS.select = 0;
}
void FillATableInfo() // Заполним таблицу достижений информацией
{
	Ach_EnsureMinigames();
	string row;
	int  n;
	aref aroot,arcur;
	makearef(aroot,pchar.achievements);
	int num = GetAttributesNum(aroot);
	int z = num;

	string attrname;
	int i;

    for(n = 1; n < z+1; n++)
    {
    	row = "tr" + n;

		// Размеры текста
		GameInterface.TABLE_ACHIEVEMENTS.(row).td1.scale = 1.1;
		GameInterface.TABLE_ACHIEVEMENTS.(row).td2.scale = 0.85;
		GameInterface.TABLE_ACHIEVEMENTS.(row).td3.scale = 0.75;
		GameInterface.TABLE_ACHIEVEMENTS.(row).td4.scale = 0.8;
		GameInterface.TABLE_ACHIEVEMENTS.(row).td5.scale = 0.9;

		GameInterface.TABLE_ACHIEVEMENTS.(row).td1.str = "-";
	}

	int hAch = LanguageOpenFile("AchievementsDescr.txt"); // открываем файл достижений ОДИН раз на всё заполнение
	// Сортировка по уровню выполненных достижений
	for(i=0; i<z; i++)
	{
		arcur = GetAttributeN(aroot,i);
		attrname = GetAttributeName(arcur);
		if(sti(pchar.achievements.(attrname)) == 3) SetTableRowByAchievement(hAch, attrname, sti(pchar.achievements.(attrname)));
	}
	string sTemp = rownumberach + "/";
	for(i=0; i<z; i++)
	{
		arcur = GetAttributeN(aroot,i);

		attrname = GetAttributeName(arcur);
		if(sti(pchar.achievements.(attrname)) == 2) SetTableRowByAchievement(hAch, attrname, sti(pchar.achievements.(attrname)));
	}

	for(i=0; i<z; i++)
	{
		arcur = GetAttributeN(aroot,i);

		attrname = GetAttributeName(arcur);
		if(sti(pchar.achievements.(attrname)) == 1) SetTableRowByAchievement(hAch, attrname, sti(pchar.achievements.(attrname)));
	}

	for(i=0; i<z; i++)
	{
		arcur = GetAttributeN(aroot,i);

		attrname = GetAttributeName(arcur);
		if(sti(pchar.achievements.(attrname)) == 0) SetTableRowByAchievement(hAch, attrname, sti(pchar.achievements.(attrname)));
	}
	sTemp = sTemp + rownumberach; 
	LanguageCloseFile(hAch); // закрываем файл достижений после заполнения всех строк
	SetFormatedText("ACH_COMPLETE_SUM", sTemp);
trace(sTemp);
	Table_UpdateWindow("TABLE_ACHIEVEMENTS");
	fill_bars();
}

// LEO: второй атлас и прогресс новых мини-игр не меняют оформление старых достижений.
void SetMinigameAchievementProgress(int hAch,string ach_id,int level,string row)
{
	if (ach_id != "AchThrowingChampion" && ach_id != "AchPrisonShooting") return;
	GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.group="ACHIEVEMENTS2";
	if (ach_id == "AchThrowingChampion")
	{
		if (level == 3)
		{
			GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str="\n"+GetConvertStr("Variable_QuestBook_155","Interface.txt");
			return;
		}
		int total=0;
		int wins=0;
		if (CheckAttribute(pchar,"Throwing.league.total")) total=sti(pchar.Throwing.league.total);
		if (CheckAttribute(pchar,"Throwing.league.wins")) wins=sti(pchar.Throwing.league.wins);
		if (total > 0) GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str="\n"+LanguageConvertString(hAch,"AchThrowingChampion_progress")+"\n"+wins+" / "+total;
		return;
	}
	int victories=0;
	if (CheckAttribute(pchar,"questTemp.prisonshootingwins")) victories=sti(pchar.questTemp.prisonshootingwins);
	string progress="\n"+LanguageConvertString(hAch,"AchPrisonShooting_progress")+"\n"+victories;
	if (level < 3) progress=progress+" / "+Ach_Tier(ach_id,level+1);
	GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str=progress;
}

void SetTableRowByAchievement(int hAch, string ach_id, int level)
{
	string row;
	int monstersall = sti(pchar.Statistic.Monster_s)+sti(pchar.Statistic.Monster_g);
	int killsall = sti(pchar.Statistic.Solder_s)+sti(pchar.Statistic.Citizen_s)+sti(pchar.Statistic.Warrior_s)+sti(pchar.Statistic.Monster_s)+sti(pchar.Statistic.Solder_g)+sti(pchar.Statistic.Citizen_g)+sti(pchar.Statistic.Warrior_g)+sti(pchar.Statistic.Monster_g);
	int killshipsall = sti(pchar.Statistic.KillShip_1)+sti(pchar.Statistic.KillShip_2)+sti(pchar.Statistic.KillShip_3)+sti(pchar.Statistic.KillShip_4)+sti(pchar.Statistic.KillShip_5)+sti(pchar.Statistic.KillShip_6)+sti(pchar.Statistic.KillShip_7)+sti(pchar.Statistic.AbordShip_1)+sti(pchar.Statistic.AbordShip_2)+sti(pchar.Statistic.AbordShip_3)+sti(pchar.Statistic.AbordShip_4)+sti(pchar.Statistic.AbordShip_5)+sti(pchar.Statistic.AbordShip_6)+sti(pchar.Statistic.AbordShip_7)+sti(pchar.Statistic.KillAbordShip_1)+sti(pchar.Statistic.KillAbordShip_2)+sti(pchar.Statistic.KillAbordShip_3)+sti(pchar.Statistic.KillAbordShip_4)+sti(pchar.Statistic.KillAbordShip_5)+sti(pchar.Statistic.KillAbordShip_6)+sti(pchar.Statistic.KillAbordShip_7);

	rownumberach++;
	row = "tr" + rownumberach;

	GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.width = 64;
	GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.height = 64;
	GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.offset = "-2, 0";
	GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.group = "ACHIEVEMENTS";
	GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.image = GetAchievementIconH(hAch, ach_id);
	GameInterface.TABLE_ACHIEVEMENTS.(row).td4.valign = "top";
	if(ach_id == "WhisperLine")
	{
		GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.group = "FACE128_543";
		GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.image = "face";
	}
	if(ach_id == "CapBladLine")
	{
		GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.group = "FACE128_221";
		GameInterface.TABLE_ACHIEVEMENTS.(row).td1.icon.image = "face";
	}

	if(level == 3)
	{
		GameInterface.TABLE_ACHIEVEMENTS.(row).td1.str = "";
		GameInterface.TABLE_ACHIEVEMENTS.(row).td2.icon.width = 25;
		GameInterface.TABLE_ACHIEVEMENTS.(row).td2.icon.height = 25;
		GameInterface.TABLE_ACHIEVEMENTS.(row).td2.icon.offset = "-24, 40";
		GameInterface.TABLE_ACHIEVEMENTS.(row).td2.icon.group = "ICONS";
		GameInterface.TABLE_ACHIEVEMENTS.(row).td2.icon.image = "complete";
		GameInterface.TABLE_ACHIEVEMENTS.(row).td2.str = GetAchievementNameH(hAch, ach_id, level);
		GameInterface.TABLE_ACHIEVEMENTS.(row).td3.str = GetAchievementDescribleH(hAch, ach_id, level);

		if(ach_id == "bank_money") strprogress = GetConvertStrWithReplace("Variable_QuestBook_26", "Interface.txt", "#space#", " ")  + " " + MakeMoneyShow(Ach_BankDeposits(), MONEY_SIGN,MONEY_DELIVER);
		if(ach_id == "rank") strprogress = GetConvertStrWithReplace("Variable_QuestBook_24", "Interface.txt", "#space#", " ") + " " + sti(pchar.rank);
		if(ach_id == "AchShipSearch") strprogress = GetConvertStrWithReplace("Variable_QuestBook_51", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.shipsearchcount);
		if(ach_id == "money") strprogress = GetConvertStrWithReplace("Variable_QuestBook_25", "Interface.txt", "#space#", " ") + MakeMoneyShow(sti(pchar.money), MONEY_SIGN,MONEY_DELIVER);
		if(ach_id == "taxes") strprogress = GetConvertStrWithReplace("Variable_QuestBook_33", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.taxescount);
		if(ach_id == "AchVzyatka") strprogress = GetConvertStrWithReplace("Variable_QuestBook_34", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.bribescount);
		if(ach_id == "kills") strprogress = GetConvertStrWithReplace("Variable_QuestBook_35", "Interface.txt", "#space#", " ") + " " + sti(killsall);
		if(ach_id == "ship_kills") strprogress = GetConvertStrWithReplace("Variable_QuestBook_36", "Interface.txt", "#space#", " ") + " " + sti(killshipsall);
		if(ach_id == "skel_kills") strprogress = GetConvertStrWithReplace("Variable_QuestBook_37", "Interface.txt", "#space#", " ") + " " + sti(monstersall);
		if(ach_id == "poisons") strprogress = GetConvertStrWithReplace("Variable_QuestBook_38", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.poisoncount);
		if(ach_id == "criticals") strprogress = GetConvertStrWithReplace("Variable_QuestBook_39", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.criticalcount);
		if(ach_id == "AchTreasure") strprogress = GetConvertStrWithReplace("Variable_QuestBook_27", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.treasurecount);
		if(ach_id == "AchBlood") strprogress = GetConvertStrWithReplace("Variable_QuestBook_28", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.bloodingcount);
		if(ach_id == "AchTravma") strprogress = GetConvertStrWithReplace("Variable_QuestBook_29", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.traumacount);
		if(ach_id == "AchStanS") strprogress = GetConvertStrWithReplace("Variable_QuestBook_30", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.swiftcount);
		if(ach_id == "AchStanH") strprogress = GetConvertStrWithReplace("Variable_QuestBook_31", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.stuncount);
		if(ach_id == "AchChest") strprogress = GetConvertStrWithReplace("Variable_QuestBook_32", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.chestcount);
		if(ach_id == "craft") strprogress = GetConvertStrWithReplace("Variable_QuestBook_40", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.craftcount);
		if(ach_id == "books") strprogress = GetConvertStrWithReplace("Variable_QuestBook_41", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.bookcount);
		if(ach_id == "officers") strprogress = GetConvertStrWithReplace("Variable_QuestBook_42", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.officercount);
		if(ach_id == "heal_bottles") strprogress = GetConvertStrWithReplace("Variable_QuestBook_44", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.healcount);
		if(ach_id == "AchFood") strprogress = GetConvertStrWithReplace("Variable_QuestBook_45", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.foodcount);
		if(ach_id == "gen_quests") strprogress = GetConvertStrWithReplace("Variable_QuestBook_46", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.genquestcount);
		if(ach_id == "AchBuildColony") strprogress = GetConvertStrWithReplace("Variable_QuestBook_47", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.colonystate);
		if(ach_id == "AchTurnir") strprogress = GetConvertStrWithReplace("Variable_QuestBook_48", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.tournamentcount);
		if(ach_id == "AchDuelyant") strprogress = GetConvertStrWithReplace("Variable_QuestBook_49", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.duelcount);
		if(ach_id == "AchShipOrder") strprogress = GetConvertStrWithReplace("Variable_QuestBook_50", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.shipordercount);
		if(ach_id == "AchGoldFleet") strprogress = GetConvertStrWithReplace("Variable_QuestBook_52", "Interface.txt", "#space#", " ") + " " + sti(pchar.questTemp.GoldFleetNum);
		if(ach_id == "AchSityRobbery") strprogress = GetConvertStrWithReplace("Variable_QuestBook_53", "Interface.txt", "#space#", " ") + " " + (Statistic_AddValue(PChar, "pir_GrabbingTown", 0) + Statistic_AddValue(PChar, "eng_GrabbingTown", 0) + Statistic_AddValue(PChar, "fra_GrabbingTown", 0) + Statistic_AddValue(PChar, "spa_GrabbingTown", 0) + Statistic_AddValue(PChar, "hol_GrabbingTown", 0));
		if(ach_id == "AchMapMaker") strprogress = GetConvertStrWithReplace("Variable_QuestBook_54", "Interface.txt", "#space#", " ") + " " + sti(pchar.MapsAtlasCount);

		strprogress = "\n" + strprogress + "\n";

		GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str = strprogress;
		GameInterface.TABLE_ACHIEVEMENTS.(row).td5.str = "+ 100";

		GameInterface.TABLE_ACHIEVEMENTS.(row).td2.color = argb(255,196,255,196);
		GameInterface.TABLE_ACHIEVEMENTS.(row).td3.color = argb(255,196,255,196);
		GameInterface.TABLE_ACHIEVEMENTS.(row).td4.color = argb(255,196,255,196);
		GameInterface.TABLE_ACHIEVEMENTS.(row).td5.color = argb(255,196,255,196);

	}
	else
	{
		if(ach_id == "Nation_quest_E" || ach_id == "Nation_quest_F" || ach_id == "Nation_quest_H" || ach_id == "Nation_quest_S" || ach_id == "Nation_quest_P" || ach_id == "Isabella_quest" || ach_id == "LSC_quest" || ach_id == "Teno_quest" || ach_id == "Killbeggars_quest"
		|| ach_id == "Ghostship_quest" || ach_id == "Bluebird_quest" || ach_id == "Berglarsgang_quest" || ach_id == "Mummydust_quest" || ach_id == "Enchantcity_quest"
		|| ach_id == "ships" || ach_id == "bank_money" || ach_id == "CapBladLine" || ach_id == "WhisperLine" || ach_id == "AchOrion" || ach_id == "AchRabotorg" || ach_id == "AchKondotier"
		|| ach_id == "AchTich" || ach_id == "AchRagnar" || ach_id == "AchSalazar" || ach_id == "AchKaskos" || ach_id == "AchUmSamil" || ach_id == "AchDozor" || ach_id == "AchMapMaker"
		|| ach_id == "AchZagadochniyOstrov" || ach_id == "AchNasledstvo" || ach_id == "AchStrannyeDela"
		// LEO: квесты, перенесённые из К:ПС - тоже одноуровневые (сразу 3-й уровень за прохождение)
		|| ach_id == "Inquisition_quest" || ach_id == "MoonStone_quest" || ach_id == "UneasyDelivery_quest" || ach_id == "GrifoSajfus_quest"
		|| ach_id == "DisappearPeople_quest" || ach_id == "CaptainMonkey_quest" || ach_id == "BlackOrchid_quest"
		|| ach_id == "RiddlesCaribbean_quest" || ach_id == "TwoSisters_quest" || ach_id == "AchThrowingChampion")
		{
			GameInterface.TABLE_ACHIEVEMENTS.(row).td1.str = "1";
			GameInterface.TABLE_ACHIEVEMENTS.(row).td1.fontidx = 0; // LEO Я тут лазил
			GameInterface.TABLE_ACHIEVEMENTS.(row).td1.textoffset = "38, 20";
			GameInterface.TABLE_ACHIEVEMENTS.(row).td1.scale = 0.65;
			GameInterface.TABLE_ACHIEVEMENTS.(row).td2.str = GetAchievementNameH(hAch, ach_id, 3);
			GameInterface.TABLE_ACHIEVEMENTS.(row).td3.str = GetAchievementDescribleH(hAch, ach_id, 3);
			GameInterface.TABLE_ACHIEVEMENTS.(row).td5.str = "+ 100";
		}
		else
		{
			GameInterface.TABLE_ACHIEVEMENTS.(row).td1.str = (level+1) + "";
			GameInterface.TABLE_ACHIEVEMENTS.(row).td1.fontidx = 0; // LEO Я тут лазил
			GameInterface.TABLE_ACHIEVEMENTS.(row).td1.textoffset = "38, 20";
			GameInterface.TABLE_ACHIEVEMENTS.(row).td1.scale = 0.65;
			GameInterface.TABLE_ACHIEVEMENTS.(row).td2.str = GetAchievementNameH(hAch, ach_id, (level+1));
			GameInterface.TABLE_ACHIEVEMENTS.(row).td3.str = GetAchievementDescribleH(hAch, ach_id, (level+1));
			if(level == 2) GameInterface.TABLE_ACHIEVEMENTS.(row).td5.str = "+ 100";
			if(level == 1) GameInterface.TABLE_ACHIEVEMENTS.(row).td5.str = "+ 50";
			if(level == 0) GameInterface.TABLE_ACHIEVEMENTS.(row).td5.str = "+ 25";

			if(level == 2)
			{
				GameInterface.TABLE_ACHIEVEMENTS.(row).td1.color = argb(255,9,110,255);
				GameInterface.TABLE_ACHIEVEMENTS.(row).td2.color = argb(255,179,171,255);
				GameInterface.TABLE_ACHIEVEMENTS.(row).td3.color = argb(255,179,171,255);
				GameInterface.TABLE_ACHIEVEMENTS.(row).td4.color = argb(255,179,171,255);
				GameInterface.TABLE_ACHIEVEMENTS.(row).td5.color = argb(255,179,171,255);
			}

			if(level == 1)
			{
				GameInterface.TABLE_ACHIEVEMENTS.(row).td1.color = argb(255,255,110,9);
				GameInterface.TABLE_ACHIEVEMENTS.(row).td2.color = argb(255,255,171,171);
				GameInterface.TABLE_ACHIEVEMENTS.(row).td3.color = argb(255,255,171,171)
				GameInterface.TABLE_ACHIEVEMENTS.(row).td4.color = argb(255,255,171,171)
				GameInterface.TABLE_ACHIEVEMENTS.(row).td5.color = argb(255,255,171,171)
			}
		}

		string strprogress;
		if((level == 2) || (level == 1) || (level == 0))
		{
			if(ach_id == "rank") strprogress = GetConvertStrWithReplace("Variable_QuestBook_24", "Interface.txt", "#space#", " ");
			if(ach_id == "money") strprogress = GetConvertStrWithReplace("Variable_QuestBook_25", "Interface.txt", "#space#", " ");
			if(ach_id == "bank_money") strprogress = GetConvertStrWithReplace("Variable_QuestBook_26", "Interface.txt", "#space#", " ");
			if(ach_id == "AchTreasure") strprogress = GetConvertStrWithReplace("Variable_QuestBook_27", "Interface.txt", "#space#", " ");
			if(ach_id == "AchBlood") strprogress = GetConvertStrWithReplace("Variable_QuestBook_28", "Interface.txt", "#space#", " ");
			if(ach_id == "AchTravma") strprogress = GetConvertStrWithReplace("Variable_QuestBook_29", "Interface.txt", "#space#", " ");
			if(ach_id == "AchStanS") strprogress = GetConvertStrWithReplace("Variable_QuestBook_30", "Interface.txt", "#space#", " ");
			if(ach_id == "AchStanH") strprogress = GetConvertStrWithReplace("Variable_QuestBook_31", "Interface.txt", "#space#", " ");
			if(ach_id == "AchChest") strprogress = GetConvertStrWithReplace("Variable_QuestBook_32", "Interface.txt", "#space#", " ");
			if(ach_id == "taxes") strprogress = GetConvertStrWithReplace("Variable_QuestBook_33", "Interface.txt", "#space#", " ");
			if(ach_id == "AchVzyatka") strprogress = GetConvertStrWithReplace("Variable_QuestBook_34", "Interface.txt", "#space#", " ");
			if(ach_id == "kills") strprogress = GetConvertStrWithReplace("Variable_QuestBook_35", "Interface.txt", "#space#", " ");
			if(ach_id == "ship_kills") strprogress = GetConvertStrWithReplace("Variable_QuestBook_36", "Interface.txt", "#space#", " ");
			if(ach_id == "skel_kills") strprogress = GetConvertStrWithReplace("Variable_QuestBook_37", "Interface.txt", "#space#", " ");
			if(ach_id == "poisons") strprogress = GetConvertStrWithReplace("Variable_QuestBook_38", "Interface.txt", "#space#", " ");
			if(ach_id == "criticals") strprogress = GetConvertStrWithReplace("Variable_QuestBook_39", "Interface.txt", "#space#", " ");
			if(ach_id == "craft") strprogress = GetConvertStrWithReplace("Variable_QuestBook_40", "Interface.txt", "#space#", " ");
			if(ach_id == "books") strprogress = GetConvertStrWithReplace("Variable_QuestBook_41", "Interface.txt", "#space#", " ");
			if(ach_id == "officers") strprogress = GetConvertStrWithReplace("Variable_QuestBook_42", "Interface.txt", "#space#", " ");
			if(ach_id == "ships") strprogress = GetConvertStrWithReplace("Variable_QuestBook_43", "Interface.txt", "#space#", " ");
			if(ach_id == "heal_bottles") strprogress = GetConvertStrWithReplace("Variable_QuestBook_44", "Interface.txt", "#space#", " ");
			if(ach_id == "AchFood") strprogress = GetConvertStrWithReplace("Variable_QuestBook_45", "Interface.txt", "#space#", " ");
			if(ach_id == "gen_quests") strprogress = GetConvertStrWithReplace("Variable_QuestBook_46", "Interface.txt", "#space#", " ");
			if(ach_id == "AchBuildColony") strprogress = GetConvertStrWithReplace("Variable_QuestBook_47", "Interface.txt", "#space#", " ");
			if(ach_id == "AchTurnir") strprogress = GetConvertStrWithReplace("Variable_QuestBook_48", "Interface.txt", "#space#", " ");
			if(ach_id == "AchDuelyant") strprogress = GetConvertStrWithReplace("Variable_QuestBook_49", "Interface.txt", "#space#", " ");
			if(ach_id == "AchShipOrder") strprogress = GetConvertStrWithReplace("Variable_QuestBook_50", "Interface.txt", "#space#", " ");
			if(ach_id == "AchShipSearch") strprogress = GetConvertStrWithReplace("Variable_QuestBook_51", "Interface.txt", "#space#", " ");
			if(ach_id == "AchGoldFleet") strprogress = GetConvertStrWithReplace("Variable_QuestBook_52", "Interface.txt", "#space#", " ");
			if(ach_id == "AchSityRobbery") strprogress = GetConvertStrWithReplace("Variable_QuestBook_53", "Interface.txt", "#space#", " ");
			if(ach_id == "AchMapMaker") strprogress = GetConvertStrWithReplace("Variable_QuestBook_54", "Interface.txt", "#space#", " ");

			strprogress = "\n" + strprogress + "\n";
		}

		if(level == 2)
		{
			if(ach_id == "rank") strprogress = strprogress + sti(pchar.rank) + " / 35";
			if(ach_id == "AchShipSearch") strprogress = strprogress + sti(pchar.questTemp.shipsearchcount) + " / 124";
			if(ach_id == "money") strprogress = strprogress + MakeMoneyShow(sti(pchar.money), MONEY_SIGN,MONEY_DELIVER) + " / 15 000 000";
			if(ach_id == "taxes") strprogress = strprogress + sti(pchar.questTemp.taxescount) + " / 100";
			if(ach_id == "AchVzyatka") strprogress = strprogress + sti(pchar.questTemp.bribescount) + " / 50";
			if(ach_id == "kills") strprogress = strprogress + sti(killsall) + " / 500";
			if(ach_id == "ship_kills") strprogress = strprogress + sti(killshipsall) + " / 70";
			if(ach_id == "skel_kills") strprogress = strprogress + sti(monstersall) + " / 150";
			if(ach_id == "poisons") strprogress = strprogress + sti(pchar.questTemp.poisoncount) + " / 150";
			if(ach_id == "criticals") strprogress = strprogress + sti(pchar.questTemp.criticalcount) + " / 500";
			if(ach_id == "AchTreasure") strprogress = strprogress + sti(pchar.questTemp.treasurecount) + " / 20";
			if(ach_id == "AchBlood") strprogress = strprogress + sti(pchar.questTemp.bloodingcount) + " / 300";
			if(ach_id == "AchTravma") strprogress = strprogress + sti(pchar.questTemp.traumacount) + " / 300";
			if(ach_id == "AchStanS") strprogress = strprogress + sti(pchar.questTemp.swiftcount) + " / 300";
			if(ach_id == "AchStanH") strprogress = strprogress + sti(pchar.questTemp.stuncount) + " / 300";
			if(ach_id == "AchChest") strprogress = strprogress + sti(pchar.questTemp.chestcount) + " / 30";
			if(ach_id == "craft") strprogress = strprogress + sti(pchar.questTemp.craftcount) + " / 1000";
			if(ach_id == "books") strprogress = strprogress + sti(pchar.questTemp.bookcount) + " / 10";
			if(ach_id == "officers") strprogress = strprogress + sti(pchar.questTemp.officercount) + " / 10";
			if(ach_id == "heal_bottles") strprogress = strprogress + sti(pchar.questTemp.healcount) + " / 200";
			if(ach_id == "AchFood") strprogress = strprogress + sti(pchar.questTemp.foodcount) + " / 300";
			if(ach_id == "gen_quests") strprogress = strprogress + sti(pchar.questTemp.genquestcount) + " / 40";
			if(ach_id == "AchBuildColony") strprogress = strprogress + sti(pchar.questTemp.colonystate) + " / 3";
			if(ach_id == "AchTurnir") strprogress = strprogress + sti(pchar.questTemp.tournamentcount) + " / 7";
			if(ach_id == "AchDuelyant") strprogress = strprogress + sti(pchar.questTemp.duelcount) + " / 20";
			if(ach_id == "AchShipOrder") strprogress = strprogress + sti(pchar.questTemp.shipordercount) + " / 15";
			if(ach_id == "AchGoldFleet") strprogress = strprogress + sti(pchar.questTemp.GoldFleetNum) + " / 7";
			if(ach_id == "AchSityRobbery") strprogress = strprogress + (Statistic_AddValue(PChar, "pir_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "eng_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "fra_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "spa_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "hol_GrabbingTown", 0)) + " / 10"
			GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str = strprogress;
		}

		if(level == 1)
		{
			if(ach_id == "rank") strprogress = strprogress + sti(pchar.rank) + " / 20";
			if(ach_id == "AchShipSearch") strprogress = strprogress + sti(pchar.questTemp.shipsearchcount) + " / 95";
			if(ach_id == "money") strprogress = strprogress + MakeMoneyShow(sti(pchar.money), MONEY_SIGN,MONEY_DELIVER) + " / 5 000 000";
			if(ach_id == "taxes") strprogress = strprogress + sti(pchar.questTemp.taxescount) + " / 50";
			if(ach_id == "AchVzyatka") strprogress = strprogress + sti(pchar.questTemp.bribescount) + " / 25";
			if(ach_id == "kills") strprogress = strprogress + sti(killsall) + " / 150";
			if(ach_id == "ship_kills") strprogress = strprogress + sti(killshipsall) + " / 35";
			if(ach_id == "skel_kills") strprogress = strprogress + sti(monstersall) + " / 75";
			if(ach_id == "poisons") strprogress = strprogress + sti(pchar.questTemp.poisoncount) + " / 100";
			if(ach_id == "criticals") strprogress = strprogress + sti(pchar.questTemp.criticalcount) + " / 250";
			if(ach_id == "AchTreasure") strprogress = strprogress + sti(pchar.questTemp.treasurecount) + " / 10";
			if(ach_id == "AchBlood") strprogress = strprogress + sti(pchar.questTemp.bloodingcount) + " / 150";
			if(ach_id == "AchTravma") strprogress = strprogress + sti(pchar.questTemp.traumacount) + " / 150";
			if(ach_id == "AchStanS") strprogress = strprogress + sti(pchar.questTemp.swiftcount) + " / 150";
			if(ach_id == "AchStanH") strprogress = strprogress + sti(pchar.questTemp.stuncount) + " / 150";
			if(ach_id == "AchChest") strprogress = strprogress + sti(pchar.questTemp.chestcount) + " / 15";
			if(ach_id == "craft") strprogress = strprogress + sti(pchar.questTemp.craftcount) + " / 500";
			if(ach_id == "books") strprogress = strprogress + sti(pchar.questTemp.bookcount) + " / 6";
			if(ach_id == "officers") strprogress = strprogress + sti(pchar.questTemp.officercount) + " / 6";
			if(ach_id == "heal_bottles") strprogress = strprogress + sti(pchar.questTemp.healcount) + " / 100";
			if(ach_id == "AchFood") strprogress = strprogress + sti(pchar.questTemp.foodcount) + " / 150";
			if(ach_id == "gen_quests") strprogress = strprogress + sti(pchar.questTemp.genquestcount) + " / 20";
			if(ach_id == "AchBuildColony") strprogress = strprogress + sti(pchar.questTemp.colonystate) + " / 3";
			if(ach_id == "AchTurnir") strprogress = strprogress + sti(pchar.questTemp.tournamentcount) + " / 3";
			if(ach_id == "AchDuelyant") strprogress = strprogress + sti(pchar.questTemp.duelcount) + " / 10";
			if(ach_id == "AchShipOrder") strprogress = strprogress + sti(pchar.questTemp.shipordercount) + " / 10";
			if(ach_id == "AchGoldFleet") strprogress = strprogress + sti(pchar.questTemp.GoldFleetNum) + " / 3";
			if(ach_id == "AchSityRobbery") strprogress = strprogress + (Statistic_AddValue(PChar, "pir_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "eng_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "fra_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "spa_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "hol_GrabbingTown", 0)) + " / 6"
			GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str = strprogress;
		}

		if(level == 0)
		{
			if(ach_id == "rank") strprogress = strprogress + sti(pchar.rank) + " / 10";
			if(ach_id == "money") strprogress = strprogress + MakeMoneyShow(sti(pchar.money), MONEY_SIGN,MONEY_DELIVER) + " / 1 000 000";
			int deposits = 0;
            if (CheckAttribute(pchar, "Quest.Deposits")) // не треться при возврате
            {
                aref quest;
                aref quests;
                makearef(quests,Characters[GetMainCharacterIndex()].Quest.Deposits);

                int nQuestsNum = GetAttributesNum(quests);
				int iTemp = 0;
				ref rNPChar;

                for(int n = 0; n < nQuestsNum; n++)
                {
                    quest = GetAttributeN(quests,n);

                    string sQuestName = GetAttributeName(quest);
                    if (CheckAttribute(Pchar, "Quest.Deposits."+sQuestName+".Sum"))
                    {
						int iPastMonths = GetPastTime("Month", makeint(Pchar.Quest.Deposits.(sQuestName).StartYear),makeint(Pchar.Quest.Deposits.(sQuestName).StartMonth),makeint(Pchar.Quest.Deposits.(sQuestName).StartDay), makefloat(Pchar.Quest.Deposits.(sQuestName).StartTime), getDataYear(),getDataMonth(),GetDataDay(), GetTime());

						iTemp = makeint(Pchar.Quest.Deposits.(sQuestName).Sum);
						rNPChar = CharacterFromID(sQuestName + "_usurer");
						if (iTemp > sti(rNPChar.UsurerDeposit)*1000) iTemp = sti(rNPChar.UsurerDeposit)*1000;
						iTemp = iTemp/100 * makeint(Pchar.Quest.Deposits.(sQuestName).Interest); //процент с учетом максимальной суммы вклада
						deposits += makeint(Pchar.Quest.Deposits.(sQuestName).Sum) + iTemp*iPastMonths;//фикс - теперь могут быть неснятые проценты
                    }
                }
            }
            if(ach_id == "bank_money") strprogress = strprogress + MakeMoneyShow(deposits, MONEY_SIGN,MONEY_DELIVER) + " / 50 000 000";
			if(ach_id == "AchShipSearch") strprogress = strprogress + sti(pchar.questTemp.shipsearchcount) + " / 50";
			if(ach_id == "taxes") strprogress = strprogress + sti(pchar.questTemp.taxescount) + " / 10";
			if(ach_id == "AchVzyatka") strprogress = strprogress + sti(pchar.questTemp.bribescount) + " / 10";
			if(ach_id == "kills") strprogress = strprogress + sti(killsall) + " / 50";
			if(ach_id == "ship_kills") strprogress = strprogress + sti(killshipsall) + " / 10";
			if(ach_id == "skel_kills") strprogress = strprogress + sti(monstersall) + " / 30";
			if(ach_id == "poisons") strprogress = strprogress + sti(pchar.questTemp.poisoncount) + " / 50";
			if(ach_id == "criticals") strprogress = strprogress + sti(pchar.questTemp.criticalcount) + " / 100";
			if(ach_id == "AchTreasure") strprogress = strprogress + sti(pchar.questTemp.treasurecount) + " / 5";
			if(ach_id == "AchBlood") strprogress = strprogress + sti(pchar.questTemp.bloodingcount) + " / 50";
			if(ach_id == "AchTravma") strprogress = strprogress + sti(pchar.questTemp.traumacount) + " / 50";
			if(ach_id == "AchStanS") strprogress = strprogress + sti(pchar.questTemp.swiftcount) + " / 50";
			if(ach_id == "AchStanH") strprogress = strprogress + sti(pchar.questTemp.stuncount) + " / 50";
			if(ach_id == "AchChest") strprogress = strprogress + sti(pchar.questTemp.chestcount) + " / 5";
			if(ach_id == "craft") strprogress = strprogress + sti(pchar.questTemp.craftcount) + " / 300";
			if(ach_id == "books") strprogress = strprogress + sti(pchar.questTemp.bookcount) + " / 3";
			if(ach_id == "officers") strprogress = strprogress + sti(pchar.questTemp.officercount) + " / 3";
			if(ach_id == "ships") strprogress = strprogress + sti(GetCompanionQuantity(pchar)) + " / " + (COMPANION_MAX);
			if(ach_id == "heal_bottles") strprogress = strprogress + sti(pchar.questTemp.healcount) + " / 50";
			if(ach_id == "AchFood") strprogress = strprogress + sti(pchar.questTemp.foodcount) + " / 50";
			if(ach_id == "gen_quests") strprogress = strprogress + sti(pchar.questTemp.genquestcount) + " / 10";
			if(ach_id == "AchBuildColony") strprogress = strprogress + sti(pchar.questTemp.colonystate) + " / 3";
			if(ach_id == "AchTurnir") strprogress = strprogress + sti(pchar.questTemp.tournamentcount) + " / 1";
			if(ach_id == "AchDuelyant") strprogress = strprogress + sti(pchar.questTemp.duelcount) + " / 5";
			if(ach_id == "AchShipOrder") strprogress = strprogress + sti(pchar.questTemp.shipordercount) + " / 5";
			if(ach_id == "AchGoldFleet") strprogress = strprogress + sti(pchar.questTemp.GoldFleetNum) + " / 1";
			if(ach_id == "AchSityRobbery") strprogress = strprogress + (Statistic_AddValue(PChar, "pir_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "eng_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "fra_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "spa_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "hol_GrabbingTown", 0)) + " / 3";
			GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str = strprogress;
			if(ach_id == "AchMapMaker") strprogress = strprogress + sti(pchar.MapsAtlasCount) + " / "+MAPS_IN_ATLAS;
		}

		GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str = strprogress;

		if(ach_id == "Nation_quest_E" || ach_id == "Nation_quest_F" || ach_id == "Nation_quest_H" || ach_id == "Nation_quest_S" || ach_id == "Nation_quest_P" || ach_id == "Isabella_quest" || ach_id == "LSC_quest" || ach_id == "Teno_quest" || ach_id == "Killbeggars_quest"
		|| ach_id == "Ghostship_quest" || ach_id == "Bluebird_quest" || ach_id == "Berglarsgang_quest" || ach_id == "Mummydust_quest" || ach_id == "Enchantcity_quest" || ach_id == "CapBladLine" || ach_id == "WhisperLine" || ach_id == "AchOrion" || ach_id == "AchRabotorg" || ach_id == "AchKondotier"
		|| ach_id == "AchTich" || ach_id == "AchRagnar" || ach_id == "AchSalazar" || ach_id == "AchKaskos" || ach_id == "AchUmSamil" || ach_id == "AchDozor"
		|| ach_id == "AchZagadochniyOstrov" || ach_id == "AchNasledstvo" || ach_id == "AchStrannyeDela"
		// LEO: квесты, перенесённые из К:ПС - тоже одноуровневые (сразу 3-й уровень за прохождение)
		|| ach_id == "Inquisition_quest" || ach_id == "MoonStone_quest" || ach_id == "UneasyDelivery_quest" || ach_id == "GrifoSajfus_quest"
		|| ach_id == "DisappearPeople_quest" || ach_id == "CaptainMonkey_quest" || ach_id == "BlackOrchid_quest"
		|| ach_id == "RiddlesCaribbean_quest" || ach_id == "TwoSisters_quest" || ach_id == "AchThrowingChampion")
		{
			GameInterface.TABLE_ACHIEVEMENTS.(row).td4.str = "\n" + GetConvertStr("Variable_QuestBook_156","Interface.txt");
		}
	}
	SetMinigameAchievementProgress(hAch,ach_id,level,row);
}

void HideAchievements() // Скрываем окно достижений
{
	XI_WindowDisable("ENC_INFO_ACHIEVEMENTS",true);
	SetNodeUsing("ACHIEVEMENTS_INFO_DUST",false);
	SetNodeUsing("ACHIEVEMENTS_INFO_FADER",false);
	SetNodeUsing("Achievements_STR",false);
	SetNodeUsing("TABLE_ACHIEVEMENTS",false);
	SetNodeUsing("ACH_COMPLETE_SUM",false);
	SetNodeUsing("SCROLL_ACHIEVEMENTS",false);
	SetNodeUsing("ACHIEVEMENTS_INFO_FRAME_CAPTION",false);
	SetNodeUsing("BAR_str1",false);
	SetNodeUsing("BAR_str2",false);
	SetNodeUsing("BAR_str3",false);
	SetNodeUsing("BAR_str4",false);
	SetNodeUsing("BAR_str5",false);
	SetNodeUsing("BAR_str6",false);
	SetNodeUsing("BAR_str7",false);

	SetNodeUsing("POINTS_EXCHANGE",false);
}

void ExitRPGHint()
{
	if (sMessageMode == "RPG_Hint")
	{
		XI_WindowShow("SHOW_ADV_INFO", false);
		XI_WindowDisable("SHOW_ADV_INFO", true);
		XI_WindowShow("SHOW_ADV_INFO_SHORT", false);
		XI_WindowDisable("SHOW_ADV_INFO_SHORT", true);
		SetNodeUsing("CONTRACT",false);
		SetNodeUsing("CONTRACT_TEXT",false);
		sMessageMode = "";
		opened = false;
	}
}

int curselectedshipl = 0;
int curselectedshipr = 0;

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;
	int iItem;

	sPicture = "-1";
	string sAttributeName;
	int nChooseNum = -1;
	ref	refBaseShip;
	string sShip;

	switch (sCurrentNode)
	{
		case "BLADE_TABLE_LIST":
			sHeader = XI_ConvertString("BladeEffectsLegend");
			sText1  = XI_ConvertString("BladeEffectsLegend_desc");
			CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
		break;
		case "TABLE_GOODS":
			sGroup = "GOODS";
			sGroupPicture = GameInterface.(CurTable).(CurRow).UserData.ID;
			sHeader = XI_ConvertString(GameInterface.(CurTable).(CurRow).UserData.ID);
			iItem = sti(GameInterface.(CurTable).(CurRow).UserData.IDX);
			sText1  = GetAssembledString(GetConvertStr(GameInterface.(CurTable).(CurRow).UserData.ID + "_descr", "GoodsDescribe.txt"), &Goods[iItem]);
			CreateTooltip("#" + sHeader, sText1, argb(255,255,0,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
		break;
		case "SHIP_TABLE_LIST_LEFT":
			makeref(refBaseShip,ShipsTypes[Last_Left_Ship]);
			sShip = refBaseShip.Name;
			sPicture = InterfaceStates.InterfaceShipString + sShip + ".dds";
			SetNewPicture("SHIP_PICTURE_R", sPicture);

			makeref(refBaseShip,ShipsTypes[Last_Right_Ship]);
			sShip = refBaseShip.Name;
			sPicture = InterfaceStates.InterfaceShipString + sShip + ".dds";
			SetNewPicture("SHIP_PICTURE_L", sPicture);

		break;
		case "SHIP_TABLE_LIST_RIGHT":
			makeref(refBaseShip,ShipsTypes[Last_Right_Ship]);
			sShip = refBaseShip.Name;
			sPicture = InterfaceStates.InterfaceShipString + sShip + ".dds";
			SetNewPicture("SHIP_PICTURE_L", sPicture);

			makeref(refBaseShip,ShipsTypes[Last_Left_Ship]);
			sShip = refBaseShip.Name;
			sPicture = InterfaceStates.InterfaceShipString + sShip + ".dds";
			SetNewPicture("SHIP_PICTURE_R", sPicture);
		break;
	}
}
void procTabChange()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();
	XI_WindowShow("ENCYCLOPEDIA_INFO_BUTTONS", false);
	XI_WindowDisable("ENCYCLOPEDIA_INFO_BUTTONS", true);
	if( sNodName == "TABBTN_QUEST" )
	{
		SetControlsTabMode( 1 );
		return;
	}
	if( sNodName == "TABBTN_QUESTARJ" )
	{
		SetControlsTabMode( 2 );
		return;
	}
	if( sNodName == "TABBTN_INFO" )
	{
		SetControlsTabMode( 3 );
		XI_WindowShow("ENCYCLOPEDIA_INFO_BUTTONS", true);
		XI_WindowDisable("ENCYCLOPEDIA_INFO_BUTTONS", false);
		return;
	}
	if( sNodName == "TABBTN_CASHBOOK" )
	{
		// LEO: повторный клик по уже открытой вкладке - переключение режима касса<->выручка с торгов; вход - сброс на кассу
		if (g_iQBCurTab == 4) g_iCashBookMode = 1 - g_iCashBookMode;
		else g_iCashBookMode = 0;
		SetControlsTabMode( 4 );
		return;
	}
	if( sNodName == "TABBTN_STATISTIC" )
	{
		SetControlsTabMode( 5 );
		return;
	}
	if( sNodName == "TABBTN_SHIP_PLACE" )
	{
		// LEO: повторный клик по уже открытой вкладке - переключение режима стоянка<->аукцион (V2); вход на вкладку - сброс на стоянку
		if (g_iQBCurTab == 6) g_iShipPlaceMode = 1 - g_iShipPlaceMode;
		else g_iShipPlaceMode = 0;
		FillShipPlaceTable("TABLE_SHIP_PLACE");
		Table_UpdateWindow("TABLE_SHIP_PLACE");
		SetControlsTabMode( 6 );
		return;
	}
	if( sNodName == "TABBTN_STOREBOOK" )
	{
		SetControlsTabMode( 7 );
		return;
	}
	// Достижения!
	if( sNodName == "TABBTN_Achievements" )
	{
		SetControlsTabMode( 8 );
		XI_WindowShow("ENCYCLOPEDIA_INFO_BUTTONS", true);
		XI_WindowDisable("ENCYCLOPEDIA_INFO_BUTTONS", false);
		return;
	}
}

void SetControlsTabMode(int nMode)
{
	g_iQBCurTab = nMode; // LEO: запоминаем текущую вкладку (для переключателя режима "Корабли в ПУ")
	SetNodeUsing("CREATE_NOTICE_BUTTON", (nMode == 1)); // Gregg: кнопка заметок видна только на вкладке "Журнал"
	if (nMode != 6) g_iShipPlaceMode = 0; // LEO: ушли с вкладки ПУ - сбрасываем режим на стоянку (название/цвет вернутся)
	if (nMode != 4) g_iCashBookMode = 0;  // LEO: ушли с вкладки "Кассовая книга" - сбрасываем режим выручки с торгов
	int nColor1 = argb(255,196,196,196);
	int nColor2 = nColor1;
	int nColor3 = nColor1;
	int nColor4 = nColor1;
	int nColor5 = nColor1;
	int nColor6 = nColor1;
	int nColor7 = nColor1;
	int nColor8 = nColor1;

	string sPic1 = "TabSelected";
	string sPic2 = sPic1;
	string sPic3 = sPic1;
	string sPic4 = sPic1;
	string sPic5 = sPic1;
	string sPic6 = sPic1;
	string sPic7 = sPic1;
	string sPic8 = sPic1;

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
		case 5:
			sPic5 = "TabDeSelected";
			nColor5 = argb(255,255,255,255);
		break;
		case 6:
			sPic6 = "TabDeSelected";
			nColor6 = argb(255,255,255,255);
		break;
		case 7:
			sPic7 = "TabDeSelected";
			nColor7 = argb(255,255,255,255);
		break;
		case 8:
			sPic8 = "TabDeSelected";
			nColor8 = argb(255,255,255,255);
		break;
	}
	SetNewGroupPicture("TABBTN_QUEST", "TABS", sPic1);
	SetNewGroupPicture("TABBTN_QUESTARJ", "TABS", sPic2);
	SetNewGroupPicture("TABBTN_INFO", "TABS", sPic3);
	SetNewGroupPicture("TABBTN_CASHBOOK", "TABS", sPic4);
	SetNewGroupPicture("TABBTN_STATISTIC", "TABS", sPic5);
	SetNewGroupPicture("TABBTN_SHIP_PLACE", "TABS", sPic6);
	SetNewGroupPicture("TABBTN_STOREBOOK", "TABS", sPic7);
	SetNewGroupPicture("TABBTN_Achievements", "TABS", sPic8);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_QUEST", 8,0,nColor1);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_QUESTARJ", 8,0,nColor2);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_INFO", 8,0,nColor3);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK", 8,0,nColor4);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_STATISTIC", 8,0,nColor5);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE", 8,0,nColor6);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_STOREBOOK", 8,0,nColor7);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_Achievements", 8,0,nColor8);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_QUEST_ALT", 8,0,nColor1);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_QUESTARJ_ALT", 8,0,nColor2);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_INFO_ALT", 8,0,nColor3);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK_ALT", 8,0,nColor4);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_STATISTIC_ALT", 8,0,nColor5);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE_ALT", 8,0,nColor6);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_STOREBOOK_ALT", 8,0,nColor7);

	FillControlsList(nMode);

	// LEO: вкладка "Корабли в ПУ" в режиме аукциона - подменяем название на "Аукцион" + жёлтый (как сумма золота); иначе обычное
	if (g_iShipPlaceMode == 1)
	{
		SetFormatedText("TABSTR_SHIP_PLACE", XI_ConvertString("PortAuction_TabName"));
		SetFormatedText("TABSTR_SHIP_PLACE_ALT", XI_ConvertString("PortAuction_TabName"));
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE", 5);     // LEO: вернуть верт. центрирование (SET_FORMATEDTEXT его сбрасывает)
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE_ALT", 5);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE", 8,0,argb(255,255,206,0));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE_ALT", 8,0,argb(255,255,206,0));
	}
	else
	{
		SetFormatedText("TABSTR_SHIP_PLACE", XI_ConvertString("ShipPlace"));
		SetFormatedText("TABSTR_SHIP_PLACE_ALT", XI_ConvertString("ShipPlace"));
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE", 5);     // LEO: вернуть верт. центрирование
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE_ALT", 5);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE", 8,0,nColor6);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SHIP_PLACE_ALT", 8,0,nColor6);
	}
	// LEO: вкладка "Кассовая книга" в режиме выручки с торгов - название "Выручка с торгов" + жёлтый; иначе обычное
	if (g_iCashBookMode == 1)
	{
		SetFormatedText("TABSTR_CASHBOOK", XI_ConvertString("PortAuction_VTabName"));
		SetFormatedText("TABSTR_CASHBOOK_ALT", XI_ConvertString("PortAuction_VTabName"));
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK", 5);
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK_ALT", 5);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK", 8,0,argb(255,255,206,0));
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK_ALT", 8,0,argb(255,255,206,0));
	}
	else
	{
		SetFormatedText("TABSTR_CASHBOOK", XI_ConvertString("CashBook"));
		SetFormatedText("TABSTR_CASHBOOK_ALT", XI_ConvertString("CashBook"));
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK", 5);
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK_ALT", 5);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK", 8,0,nColor4);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_CASHBOOK_ALT", 8,0,nColor4);
	}
}

void FillControlsList(int nMode)
{
	switch (nMode)
	{
		case 1: selectJournal(1); break;  // первый режим журнала, только активные
		case 2: selectJournal(2); break;  // только закрытые
		case 3: selectJournal(3); break;  // только инфа
		case 4: selectCashBook(); break;
		case 5: selectStatistic(); break;
		case 6: selectShipPlace(); break;
		case 7: selectStoreBook(); break;
		case 8: selectAchievements(); break; // Достижения
	}
}
string GWIK_BookDate(string sStored) // GetQuestBookDataDigit даёт "ГГГГ.ММ.ДД ..." -> приводим к "ДД.ММ.ГГГГ", как у депозитов
{
	return strcut(sStored, 8, 9) + "." + strcut(sStored, 5, 6) + "." + strcut(sStored, 0, 3);
}
void InitTableHeader()
{
	int	i;
	string row;

	GameInterface.TABLE_SHIPCLASS.hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_55", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SHIPCLASS.hr.td1.scale = 0.9;
	GameInterface.TABLE_SHIPCLASS.hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_56", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SHIPCLASS.hr.td2.scale = 0.7;
	GameInterface.TABLE_SHIPCLASS.hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_57", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SHIPCLASS.hr.td3.scale = 0.7;
	GameInterface.TABLE_SHIPCLASS.hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_58", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_SHIPCLASS.hr.td4.scale = 0.7;

	for (i = 1; i< 7; i++)
	{
		row = "tr" + i;
		GameInterface.TABLE_SHIPCLASS.(row).td1.str = GetConvertStrWithReplace("Variable_QuestBook_59", "Interface.txt", "#space#", " ") + i;
		GameInterface.TABLE_SHIPCLASS.(row).td2.str = Statistic_AddValue(pchar, "KillShip_" + i, 0);
		GameInterface.TABLE_SHIPCLASS.(row).td3.str = Statistic_AddValue(pchar, "KillAbordShip_" + i, 0);
		GameInterface.TABLE_SHIPCLASS.(row).td4.str = Statistic_AddValue(pchar, "AbordShip_" + i, 0);
	}

	GameInterface.TABLE_HUMAN.hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_60", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_HUMAN.hr.td1.scale = 0.9;
	GameInterface.TABLE_HUMAN.hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_61", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_HUMAN.hr.td2.scale = 0.7;
	GameInterface.TABLE_HUMAN.hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_62", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_HUMAN.hr.td3.scale = 0.7;

	GameInterface.TABLE_HUMAN.tr1.td1.str = GetConvertStrWithReplace("Variable_QuestBook_63", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_HUMAN.tr1.td2.str = Statistic_AddValue(pchar, "Solder_s", 0);
	GameInterface.TABLE_HUMAN.tr1.td3.str = Statistic_AddValue(pchar, "Solder_g", 0);

	GameInterface.TABLE_HUMAN.tr2.td1.str = GetConvertStrWithReplace("Variable_QuestBook_64", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_HUMAN.tr2.td2.str = Statistic_AddValue(pchar, "Citizen_s", 0);
	GameInterface.TABLE_HUMAN.tr2.td3.str = Statistic_AddValue(pchar, "Citizen_g", 0);

	GameInterface.TABLE_HUMAN.tr3.td1.str = GetConvertStrWithReplace("Variable_QuestBook_65", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_HUMAN.tr3.td2.str = Statistic_AddValue(pchar, "Monster_s", 0);
	GameInterface.TABLE_HUMAN.tr3.td3.str = Statistic_AddValue(pchar, "Monster_g", 0);

	GameInterface.TABLE_HUMAN.tr4.td1.str = GetConvertStrWithReplace("Variable_QuestBook_66", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_HUMAN.tr4.td2.str = Statistic_AddValue(pchar, "Warrior_s", 0);
	GameInterface.TABLE_HUMAN.tr4.td3.str = Statistic_AddValue(pchar, "Warrior_g", 0);

	GameInterface.TABLE_HUMAN.tr5.td1.str = GetConvertStrWithReplace("Variable_QuestBook_67", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_HUMAN.tr5.td2.str = Statistic_AddValue(pchar, "Solder_s", 0) + Statistic_AddValue(pchar, "Citizen_s", 0) + Statistic_AddValue(pchar, "Monster_s", 0) + Statistic_AddValue(pchar, "Warrior_s", 0);
	GameInterface.TABLE_HUMAN.tr5.td3.str = Statistic_AddValue(pchar, "Solder_g", 0) + Statistic_AddValue(pchar, "Citizen_g", 0) + Statistic_AddValue(pchar, "Monster_g", 0) + Statistic_AddValue(pchar, "Warrior_g", 0);

	GameInterface.TABLE_OTHER.hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_68", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.hr.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_69", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.hr.td2.scale = 0.9;

	GameInterface.TABLE_OTHER.tr1.td1.str = GetConvertStrWithReplace("Variable_QuestBook_70", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr1.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr1.td2.str = Statistic_AddValue(PChar, "SellShip", 0);

	GameInterface.TABLE_OTHER.tr2.td1.str = GetConvertStrWithReplace("Variable_QuestBook_71", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr2.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr2.td2.str = Statistic_AddValue(PChar, "BuyShip", 0);

	GameInterface.TABLE_OTHER.tr3.td1.str = GetConvertStrWithReplace("Variable_QuestBook_72", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr3.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr3.td2.str = Statistic_AddValue(PChar, "ShipMunity", 0);

	GameInterface.TABLE_OTHER.tr4.td1.str = GetConvertStrWithReplace("Variable_QuestBook_73", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr4.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr4.td2.str = Statistic_AddValue(PChar, "SlavesMunity", 0);

	GameInterface.TABLE_OTHER.tr5.td1.str = GetConvertStrWithReplace("Variable_QuestBook_74", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr5.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr5.td2.str = Statistic_AddValue(PChar, "AbordShipFree", 0);

	GameInterface.TABLE_OTHER.tr6.td1.str = GetConvertStrWithReplace("Variable_QuestBook_75", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr6.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr6.td2.str = Statistic_AddValue(PChar, "RatsEatGoods", 0);

	GameInterface.TABLE_OTHER.tr7.td1.str = "Save\nLoad";
	GameInterface.TABLE_OTHER.tr7.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr7.td2.str = pchar.SystemInfo.SaveCount +"\n" + pchar.SystemInfo.LoadCount;
	GameInterface.TABLE_OTHER.tr7.td2.scale = 0.9;

	GameInterface.TABLE_OTHER.tr8.td1.str = GetConvertStrWithReplace("Variable_QuestBook_76", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr8.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr8.td2.str = sti(pchar.Health.TotalDamg);

	GameInterface.TABLE_OTHER.tr9.td1.str = GetConvertStrWithReplace("Variable_QuestBook_77", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr9.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr9.td2.str = Statistic_AddValue(PChar, "Sailors_dead", 0);

	GameInterface.TABLE_OTHER.tr10.td1.str = GetConvertStrWithReplace("Variable_QuestBook_78", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_OTHER.tr10.td1.scale = 0.9;
	GameInterface.TABLE_OTHER.tr10.td2.str = Statistic_AddValue(PChar, "Treasure", 0);

	GameInterface.TABLE_NATION.hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_79", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_NATION.hr.td1.scale = 0.9;
 	GameInterface.TABLE_NATION.hr.td2.icon.group = "NATIONS"
	GameInterface.TABLE_NATION.hr.td2.icon.image	  = Nations[0].Name;
	GameInterface.TABLE_NATION.hr.td2.icon.width = 32;
	GameInterface.TABLE_NATION.hr.td2.icon.height = 32;
	GameInterface.TABLE_NATION.hr.td2.icon.offset = "7, 2";
 	GameInterface.TABLE_NATION.hr.td3.icon.group = "NATIONS"
	GameInterface.TABLE_NATION.hr.td3.icon.image	  = Nations[1].Name;
	GameInterface.TABLE_NATION.hr.td3.icon.width = 32;
	GameInterface.TABLE_NATION.hr.td3.icon.height = 32;
	GameInterface.TABLE_NATION.hr.td3.icon.offset = "7, 2";
 	GameInterface.TABLE_NATION.hr.td4.icon.group = "NATIONS"
	GameInterface.TABLE_NATION.hr.td4.icon.image	  = Nations[2].Name;
	GameInterface.TABLE_NATION.hr.td4.icon.width = 32;
	GameInterface.TABLE_NATION.hr.td4.icon.height = 32;
	GameInterface.TABLE_NATION.hr.td4.icon.offset = "7, 2";
 	GameInterface.TABLE_NATION.hr.td5.icon.group = "NATIONS"
	GameInterface.TABLE_NATION.hr.td5.icon.image	  = Nations[3].Name;
	GameInterface.TABLE_NATION.hr.td5.icon.width = 32;
	GameInterface.TABLE_NATION.hr.td5.icon.height = 32;
	GameInterface.TABLE_NATION.hr.td5.icon.offset = "7, 2";
 	GameInterface.TABLE_NATION.hr.td6.icon.group = "NATIONS"
	GameInterface.TABLE_NATION.hr.td6.icon.image	  = Nations[4].Name;
	GameInterface.TABLE_NATION.hr.td6.icon.width = 32;
	GameInterface.TABLE_NATION.hr.td6.icon.height = 32;
	GameInterface.TABLE_NATION.hr.td6.icon.offset = "7, 2";
	GameInterface.TABLE_NATION.hr.td7.str = GetConvertStrWithReplace("Variable_QuestBook_81", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_NATION.hr.td7.scale = 0.9;

	GameInterface.TABLE_NATION.tr1.td1.str = GetConvertStrWithReplace("Variable_QuestBook_82", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_NATION.tr1.td1.scale = 0.9;
	GameInterface.TABLE_NATION.tr1.td2.str = Statistic_AddValue(PChar, "eng_KillFort", 0);
	GameInterface.TABLE_NATION.tr1.td3.str = Statistic_AddValue(PChar, "fra_KillFort", 0);
	GameInterface.TABLE_NATION.tr1.td4.str = Statistic_AddValue(PChar, "spa_KillFort", 0);
	GameInterface.TABLE_NATION.tr1.td5.str = Statistic_AddValue(PChar, "hol_KillFort", 0);
	GameInterface.TABLE_NATION.tr1.td6.str = Statistic_AddValue(PChar, "pir_KillFort", 0);
	GameInterface.TABLE_NATION.tr1.td7.str = (Statistic_AddValue(PChar, "pir_KillFort", 0) +
			 Statistic_AddValue(PChar, "eng_KillFort", 0) +
			 Statistic_AddValue(PChar, "fra_KillFort", 0) +
			 Statistic_AddValue(PChar, "spa_KillFort", 0) +
			 Statistic_AddValue(PChar, "hol_KillFort", 0));

	GameInterface.TABLE_NATION.tr2.td1.str = GetConvertStrWithReplace("Variable_QuestBook_83", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_NATION.tr2.td1.scale = 0.9;
	GameInterface.TABLE_NATION.tr2.td2.str = Statistic_AddValue(PChar, "eng_TakeTown", 0);
	GameInterface.TABLE_NATION.tr2.td3.str = Statistic_AddValue(PChar, "fra_TakeTown", 0);
	GameInterface.TABLE_NATION.tr2.td4.str = Statistic_AddValue(PChar, "spa_TakeTown", 0);
	GameInterface.TABLE_NATION.tr2.td5.str = Statistic_AddValue(PChar, "hol_TakeTown", 0);
	GameInterface.TABLE_NATION.tr2.td6.str = Statistic_AddValue(PChar, "pir_TakeTown", 0);
	GameInterface.TABLE_NATION.tr2.td7.str = (Statistic_AddValue(PChar, "pir_TakeTown", 0) +
			 Statistic_AddValue(PChar, "eng_TakeTown", 0) +
			 Statistic_AddValue(PChar, "fra_TakeTown", 0) +
			 Statistic_AddValue(PChar, "spa_TakeTown", 0) +
			 Statistic_AddValue(PChar, "hol_TakeTown", 0));

	GameInterface.TABLE_NATION.tr3.td1.str = GetConvertStrWithReplace("Variable_QuestBook_84", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_NATION.tr3.td1.scale = 0.9;
	GameInterface.TABLE_NATION.tr3.td2.str = Statistic_AddValue(PChar, "eng_GrabbingTown", 0);
	GameInterface.TABLE_NATION.tr3.td3.str = Statistic_AddValue(PChar, "fra_GrabbingTown", 0);
	GameInterface.TABLE_NATION.tr3.td4.str = Statistic_AddValue(PChar, "spa_GrabbingTown", 0);
	GameInterface.TABLE_NATION.tr3.td5.str = Statistic_AddValue(PChar, "hol_GrabbingTown", 0);
	GameInterface.TABLE_NATION.tr3.td6.str = Statistic_AddValue(PChar, "pir_GrabbingTown", 0);
	GameInterface.TABLE_NATION.tr3.td7.str = (Statistic_AddValue(PChar, "pir_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "eng_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "fra_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "spa_GrabbingTown", 0) +
			 Statistic_AddValue(PChar, "hol_GrabbingTown", 0));

	GameInterface.TABLE_NATION.tr4.td1.str = GetConvertStrWithReplace("Variable_QuestBook_85", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_NATION.tr4.td1.scale = 0.9;
	GameInterface.TABLE_NATION.tr4.td2.str = Statistic_AddValue(PChar, "eng_AbordShip", 0);
	GameInterface.TABLE_NATION.tr4.td3.str = Statistic_AddValue(PChar, "fra_AbordShip", 0);
	GameInterface.TABLE_NATION.tr4.td4.str = Statistic_AddValue(PChar, "spa_AbordShip", 0);
	GameInterface.TABLE_NATION.tr4.td5.str = Statistic_AddValue(PChar, "hol_AbordShip", 0);
	GameInterface.TABLE_NATION.tr4.td6.str = Statistic_AddValue(PChar, "pir_AbordShip", 0);
	GameInterface.TABLE_NATION.tr4.td7.str = (Statistic_AddValue(PChar, "pir_AbordShip", 0) +
			 Statistic_AddValue(PChar, "eng_AbordShip", 0) +
			 Statistic_AddValue(PChar, "fra_AbordShip", 0) +
			 Statistic_AddValue(PChar, "spa_AbordShip", 0) +
			 Statistic_AddValue(PChar, "hol_AbordShip", 0));

	GameInterface.TABLE_NATION.tr5.td1.str = GetConvertStrWithReplace("Variable_QuestBook_86", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_NATION.tr5.td1.scale = 0.9;
	GameInterface.TABLE_NATION.tr5.td2.str = Statistic_AddValue(PChar, "eng_KillShip", 0);
	GameInterface.TABLE_NATION.tr5.td3.str = Statistic_AddValue(PChar, "fra_KillShip", 0);
	GameInterface.TABLE_NATION.tr5.td4.str = Statistic_AddValue(PChar, "spa_KillShip", 0);
	GameInterface.TABLE_NATION.tr5.td5.str = Statistic_AddValue(PChar, "hol_KillShip", 0);
	GameInterface.TABLE_NATION.tr5.td6.str = Statistic_AddValue(PChar, "pir_KillShip", 0);
	GameInterface.TABLE_NATION.tr5.td7.str = (Statistic_AddValue(PChar, "pir_KillShip", 0) +
			 Statistic_AddValue(PChar, "eng_KillShip", 0) +
			 Statistic_AddValue(PChar, "fra_KillShip", 0) +
			 Statistic_AddValue(PChar, "spa_KillShip", 0) +
			 Statistic_AddValue(PChar, "hol_KillShip", 0));

	GameInterface.TABLE_MONEY.tr1.td1.str = GetConvertStrWithReplace("Variable_QuestBook_87", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_MONEY.tr1.td1.scale = 0.9;
	GameInterface.TABLE_MONEY.tr1.td2.str = Statistic_AddValue(PChar, "Money_spend", 0);
	GameInterface.TABLE_MONEY.tr1.td2.scale = 0.9;

	GameInterface.TABLE_MONEY.tr2.td1.str = GetConvertStrWithReplace("Variable_QuestBook_88", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_MONEY.tr2.td1.scale = 0.9;
	GameInterface.TABLE_MONEY.tr2.td2.str = Statistic_AddValue(PChar, "PartitionPay", 0);
	GameInterface.TABLE_MONEY.tr2.td2.scale = 0.9;

	Table_UpdateWindow("TABLE_SHIPCLASS");
	Table_UpdateWindow("TABLE_HUMAN");
	Table_UpdateWindow("TABLE_OTHER");
	Table_UpdateWindow("TABLE_NATION");
	Table_UpdateWindow("TABLE_MONEY");
	//  ростовщики
	aref quests;
	aref quest;
	int  nQuestsNum;
	int  n;
	string sQuestName;
	FillCreditTable(); // LEO: левая таблица кассы (кредиты) вынесена - чтобы refill-ить при выходе из режима "Выручка с торгов"

	GameInterface.TABLE_DEBIT.hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_93", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_DEBIT.hr.td1.scale = 0.9;
	GameInterface.TABLE_DEBIT.hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_94", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_DEBIT.hr.td2.scale = 0.9;
	GameInterface.TABLE_DEBIT.hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_95", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_DEBIT.hr.td3.scale = 0.9;
	GameInterface.TABLE_DEBIT.hr.td4.str = "%";
	GameInterface.TABLE_DEBIT.hr.td4.scale = 0.9;
	GameInterface.TABLE_DEBIT.select = 0;
	GameInterface.TABLE_DEBIT.hr.td5.str = GetConvertStrWithReplace("Variable_QuestBook_96", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_DEBIT.hr.td5.scale = 0.9;
	GameInterface.TABLE_DEBIT.select = 0;
	i = 1;
	if (CheckAttribute(pchar, "Quest.Deposits")) // не треться при возврате
	{
		makearef(quests,Characters[GetMainCharacterIndex()].Quest.Deposits);

		nQuestsNum = GetAttributesNum(quests);
		int iTemp;
		ref rNPChar;
		for(n = 0; n < nQuestsNum; n++)
		{
			quest = GetAttributeN(quests,n);

			sQuestName = GetAttributeName(quest);
			rNPChar = CharacterFromID(sQuestName + "_usurer");
			if (CheckAttribute(Pchar, "Quest.Deposits."+sQuestName+".Sum"))
			{
				row = "tr" + i;
				i++;
				GameInterface.TABLE_DEBIT.(row).td1.str = GetCityName(sQuestName);
				GameInterface.TABLE_DEBIT.(row).td1.scale = 0.85;

				iTemp = makeint(Pchar.Quest.Deposits.(sQuestName).Sum);
				if (iTemp > sti(rNPChar.UsurerDeposit)*1000) 
				{
					iTemp = sti(rNPChar.UsurerDeposit)*1000;
					GameInterface.TABLE_DEBIT.(row).td2.str = MakeMoneyShow(iTemp, MONEY_SIGN,MONEY_DELIVER) + " +(" + MakeMoneyShow(sti(Pchar.Quest.Deposits.(sQuestName).Sum) - iTemp, MONEY_SIGN,MONEY_DELIVER) + ")";//вывести лимит вклада при превышении
				}
				else GameInterface.TABLE_DEBIT.(row).td2.str = MakeMoneyShow(iTemp, MONEY_SIGN,MONEY_DELIVER);

				GameInterface.TABLE_DEBIT.(row).td2.scale = 0.75;
				GameInterface.TABLE_DEBIT.(row).td3.str = GetBookData(sti(Pchar.Quest.Deposits.(sQuestName).StartDay),
								   sti(Pchar.Quest.Deposits.(sQuestName).StartMonth),
								   sti(Pchar.Quest.Deposits.(sQuestName).StartYear));
				GameInterface.TABLE_DEBIT.(row).td3.scale = 0.8;
				GameInterface.TABLE_DEBIT.(row).td4.str = Pchar.Quest.Deposits.(sQuestName).Interest;
				GameInterface.TABLE_DEBIT.(row).td4.scale = 0.9;
				int iPastMonths = GetPastTime("Month", makeint(Pchar.Quest.Deposits.(sQuestName).StartYear),makeint(Pchar.Quest.Deposits.(sQuestName).StartMonth),makeint(Pchar.Quest.Deposits.(sQuestName).StartDay), makefloat(Pchar.Quest.Deposits.(sQuestName).StartTime), getDataYear(),getDataMonth(),GetDataDay(), GetTime());

				if (iTemp > sti(rNPChar.UsurerDeposit)*1000) iTemp = sti(rNPChar.UsurerDeposit)*1000;
				iTemp = iTemp/100 * makeint(Pchar.Quest.Deposits.(sQuestName).Interest); //процент с учетом максимальной суммы вклада
				Pchar.Quest.Deposits.(sQuestName).Result = makeint(Pchar.Quest.Deposits.(sQuestName).Sum) + iTemp*iPastMonths;//фикс - теперь могут быть неснятые проценты

				GameInterface.TABLE_DEBIT.(row).td5.str = Pchar.Quest.Deposits.(sQuestName).Result;
				GameInterface.TABLE_DEBIT.(row).td5.scale = 0.9;
			}
		}
	}
	// вексель и пай ГВИК -->
	string sGwikStr;
	if (CheckAttribute(pchar, "GWIK.Bill"))
	{
		row = "tr" + i;
		i++;
		sGwikStr = GetCityName(GWIK_OfficeCityById(pchar.GWIK.Bill.From)); // Gregg: город конторы-эмитента (вексель обналичивается в любой другой конторе)
		if (LanguageGetLanguage() == "russian") sGwikStr = "ГВИК: " + sGwikStr;
		else sGwikStr = "GWIC: " + sGwikStr;
		GameInterface.TABLE_DEBIT.(row).td1.str = sGwikStr;
		GameInterface.TABLE_DEBIT.(row).td1.scale = 0.7; // мельче, чтобы "ГВИК: Виллемстад" не переносился
		GameInterface.TABLE_DEBIT.(row).td2.str = MakeMoneyShow(sti(pchar.GWIK.Bill), MONEY_SIGN, MONEY_DELIVER);
		GameInterface.TABLE_DEBIT.(row).td2.scale = 0.75;
		if (CheckAttribute(pchar, "GWIK.Bill.Date"))
		{
			GameInterface.TABLE_DEBIT.(row).td3.str = GWIK_BookDate(pchar.GWIK.Bill.Date); // ДД.ММ.ГГГГ, как у депозитов
			GameInterface.TABLE_DEBIT.(row).td3.scale = 0.8;
		}
		GameInterface.TABLE_DEBIT.(row).td4.str = "-";
		GameInterface.TABLE_DEBIT.(row).td4.scale = 0.9;
		GameInterface.TABLE_DEBIT.(row).td5.str = MakeMoneyShow(sti(pchar.GWIK.Bill), MONEY_SIGN, MONEY_DELIVER);
		GameInterface.TABLE_DEBIT.(row).td5.scale = 0.9;
	}
	if (CheckAttribute(pchar, "GWIK.Share"))
	{
		row = "tr" + i;
		i++;
		if (LanguageGetLanguage() == "russian") sGwikStr = "Пай ГВИК " + pchar.GWIK.Share.Level; // коротко, иначе перенос в ячейке
		else sGwikStr = "GWIC share " + pchar.GWIK.Share.Level;
		GameInterface.TABLE_DEBIT.(row).td1.str = sGwikStr;
		GameInterface.TABLE_DEBIT.(row).td1.scale = 0.85;
		GameInterface.TABLE_DEBIT.(row).td2.str = MakeMoneyShow(sti(pchar.GWIK.Share.Invested), MONEY_SIGN, MONEY_DELIVER);
		GameInterface.TABLE_DEBIT.(row).td2.scale = 0.75;
		if (CheckAttribute(pchar, "GWIK.Share.Date"))
		{
			GameInterface.TABLE_DEBIT.(row).td3.str = GWIK_BookDate(pchar.GWIK.Share.Date); // ДД.ММ.ГГГГ, как у депозитов
			GameInterface.TABLE_DEBIT.(row).td3.scale = 0.8;
		}
		string sShareRate = "2";                                        // ставка дивидендов: Пайщик
		if (sti(pchar.GWIK.Share.Level) == 2) sShareRate = "3";         // Партнёр
		if (sti(pchar.GWIK.Share.Level) >= 3) sShareRate = "4";         // Компаньон
		GameInterface.TABLE_DEBIT.(row).td4.str = sShareRate;
		GameInterface.TABLE_DEBIT.(row).td4.scale = 0.9;
		GameInterface.TABLE_DEBIT.(row).td5.str = MakeMoneyShow(sti(pchar.GWIK.Share.Pool), MONEY_SIGN, MONEY_DELIVER); // накопленные дивиденды
		GameInterface.TABLE_DEBIT.(row).td5.scale = 0.9;
	}
	// вексель и пай ГВИК <--
	Table_UpdateWindow("TABLE_DEBIT");

	FillShipPlaceTable("TABLE_SHIP_PLACE"); // 1.2.3

	FillPriceListTown("TABLE_CITY");
}

int g_iShipPlaceMode = 0; // LEO: 0 = стоянка, 1 = аукцион (V2: переключатель режимов вкладки "Корабли в ПУ")
int g_iQBCurTab = 0;      // LEO: текущая вкладка журнала (для переключения режима повторным кликом по "Корабли в ПУ")
int g_iCashBookMode = 0;  // LEO: 0 = касса (кредиты/вклады), 1 = выручка с торгов по городам (тогл вкладки "Кассовая книга")

// LEO: заполнение ЛЕВОЙ таблицы кассы (кредиты/займы у ростовщиков). Вынесено из InitTableHeader,
// чтобы возвращать кредиты после режима "Выручка с торгов" (тот режим перезаписывает эту же таблицу).
void FillCreditTable()
{
	string row, sQuestName;
	aref quests, quest;
	int nQuestsNum, n, i;
	Table_Clear("TABLE_CREDIT", false, true, false);
	GameInterface.TABLE_CREDIT.hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_89", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_CREDIT.hr.td1.scale = 0.9;
	GameInterface.TABLE_CREDIT.hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_90", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_CREDIT.hr.td2.scale = 0.9;
	GameInterface.TABLE_CREDIT.hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_91", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_CREDIT.hr.td3.scale = 0.9;
	GameInterface.TABLE_CREDIT.hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_92", "Interface.txt", "#space#", " ");
	GameInterface.TABLE_CREDIT.hr.td4.scale = 0.9;
	GameInterface.TABLE_CREDIT.hr.td5.str = "%";
	GameInterface.TABLE_CREDIT.hr.td5.scale = 0.9;
	GameInterface.TABLE_CREDIT.select = 0;
	i = 1;
	if (CheckAttribute(pchar, "Quest.Loans"))
	{
		makearef(quests, pchar.Quest.Loans);
		nQuestsNum = GetAttributesNum(quests);
		for(n = 0; n < nQuestsNum; n++)
		{
			quest = GetAttributeN(quests,n);
			sQuestName = GetAttributeName(quest);
			row = "tr" + i;
			i++;
			GameInterface.TABLE_CREDIT.(row).td1.str = GetCityName(sQuestName);
			GameInterface.TABLE_CREDIT.(row).td1.scale = 0.85;
			GameInterface.TABLE_CREDIT.(row).td2.str = MakeMoneyShow(sti(Pchar.Quest.Loans.(sQuestName).Sum), MONEY_SIGN,MONEY_DELIVER);
			GameInterface.TABLE_CREDIT.(row).td2.scale = 0.75;
			GameInterface.TABLE_CREDIT.(row).td3.str = GetBookData(sti(Pchar.Quest.Loans.(sQuestName).StartDay),
							   sti(Pchar.Quest.Loans.(sQuestName).StartMonth),
							   sti(Pchar.Quest.Loans.(sQuestName).StartYear));
			GameInterface.TABLE_CREDIT.(row).td3.scale = 0.8;
			GameInterface.TABLE_CREDIT.(row).td4.str = Pchar.Quest.Loans.(sQuestName).Period;
			GameInterface.TABLE_CREDIT.(row).td4.scale = 0.9;
			GameInterface.TABLE_CREDIT.(row).td5.str = Pchar.Quest.Loans.(sQuestName).Interest;
			GameInterface.TABLE_CREDIT.(row).td5.scale = 0.9;
		}
	}
	Table_UpdateWindow("TABLE_CREDIT");
}

// LEO: режим "Выручка с торгов" - в ЛЕВОЙ таблице кассы по городам: выручка аукциона (касса ПУ) + число кораблей на торгах.
// Видно даже когда кораблей на торгах нет (выручка от уже проданных остаётся в кассе портмана города).
void FillAuctionVaultTable()
{
	int n, i, cn, iVault, iShips;
	string row;
	ref rCity, rPm, chref;
	int iAucClr = argb(255,255,225,160);
	Table_Clear("TABLE_CREDIT", false, true, false);
	GameInterface.TABLE_CREDIT.select = 0;
	GameInterface.TABLE_CREDIT.hr.td1.str = XI_ConvertString("PortAuction_hdrCity");  // Город
	GameInterface.TABLE_CREDIT.hr.td1.scale = 0.9;
	GameInterface.TABLE_CREDIT.hr.td2.str = XI_ConvertString("PortAuction_hdrGain");  // Выручка
	GameInterface.TABLE_CREDIT.hr.td2.scale = 0.9;
	GameInterface.TABLE_CREDIT.hr.td3.str = XI_ConvertString("PortAuction_VColShips"); // "На торгах кораблей:" - длинно, мельче, в 2 строки в узкой колонке
	GameInterface.TABLE_CREDIT.hr.td3.scale = 0.7;
	GameInterface.TABLE_CREDIT.hr.td4.str = "";
	GameInterface.TABLE_CREDIT.hr.td5.str = "";
	cn = 1;
	for (n = 0; n < MAX_COLONIES; n++)
	{
		rCity = &Colonies[n];
		rPm = CharacterFromID(rCity.id + "_PortMan");
		iVault = 0;
		if (CheckAttribute(rPm, "AuctionVault")) iVault = sti(rPm.AuctionVault);
		// число кораблей этого города на торгах
		iShips = 0;
		for (i = 1; i < MAX_CHARACTERS; i++)
		{
			chref = &Characters[i];
			if (!CheckAttribute(chref, "ShipAtAuction")) continue;
			if (chref.ShipAtAuction != (rCity.id + "_PortMan")) continue;
			iShips++;
		}
		if (iVault <= 0) { if (iShips <= 0) continue; } // ни выручки, ни лотов - город пропускаем
		row = "tr" + cn;
		cn++;
		GameInterface.TABLE_CREDIT.(row).td1.str = GetCityName(rCity.id);
		GameInterface.TABLE_CREDIT.(row).td1.scale = 0.85;
		GameInterface.TABLE_CREDIT.(row).td1.color = iAucClr;
		GameInterface.TABLE_CREDIT.(row).td2.str = "" + iVault;
		GameInterface.TABLE_CREDIT.(row).td2.scale = 0.8;
		GameInterface.TABLE_CREDIT.(row).td2.color = iAucClr;
		GameInterface.TABLE_CREDIT.(row).td3.str = "" + iShips;
		GameInterface.TABLE_CREDIT.(row).td3.scale = 0.8;
		GameInterface.TABLE_CREDIT.(row).td3.color = iAucClr;
	}
	Table_UpdateWindow("TABLE_CREDIT");
}

// LEO: ВАРИАНТ 2 - режим АУКЦИОНа той же вкладки "Корабли в ПУ" (подмена заголовков + только аукционные корабли, жёлтым)
void FillShipAuctionTable(string _tabName)
{
	int i, cn, n;
	string row;
	ref rCity, chref;
	int iAucClr = argb(255,255,225,160);
	int iTotalGain = 0;

	Table_Clear(_tabName, false, true, false);
	GameInterface.(_tabName).select = 0;
	GameInterface.(_tabName).hr.td1.str = "N";
	GameInterface.(_tabName).hr.td1.scale = 0.9;
	GameInterface.(_tabName).hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_97", "Interface.txt", "#space#", " "); // Фото
	GameInterface.(_tabName).hr.td2.scale = 0.9;
	GameInterface.(_tabName).hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_98", "Interface.txt", "#space#", " "); // Класс
	GameInterface.(_tabName).hr.td3.scale = 0.9;
	GameInterface.(_tabName).hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_99", "Interface.txt", "#space#", " "); // Корабль
	GameInterface.(_tabName).hr.td4.scale = 0.9;
	GameInterface.(_tabName).hr.td5.str = XI_ConvertString("PortStock_hdrStatus"); // Статус
	GameInterface.(_tabName).hr.td5.scale = 0.9;
	GameInterface.(_tabName).hr.td6.str = GetConvertStrWithReplace("Variable_QuestBook_101", "Interface.txt", "#space#", " "); // Город
	GameInterface.(_tabName).hr.td6.scale = 0.9;
	GameInterface.(_tabName).hr.td7.str = GetConvertStrWithReplace("Variable_QuestBook_102", "Interface.txt", "#space#", " "); // Локация
	GameInterface.(_tabName).hr.td7.scale = 0.9;
	GameInterface.(_tabName).hr.td8.str = XI_ConvertString("PortAuction_hdrDaysLeft"); // Дней до торгов
	GameInterface.(_tabName).hr.td8.scale = 0.9;
	GameInterface.(_tabName).hr.td9.str = XI_ConvertString("PortAuction_hdrGain"); // Выручка
	GameInterface.(_tabName).hr.td9.scale = 0.9;

	cn = 1;
	for (n=0; n<MAX_COLONIES; n++)
	{
		rCity = &Colonies[n];
		for (i=1; i<MAX_CHARACTERS; i++)
		{
			chref = &Characters[i];
			if (!CheckAttribute(chref, "ShipAtAuction")) continue;
			if (chref.ShipAtAuction != (rCity.id + "_PortMan")) continue;

			int iShipA = sti(chref.Ship.Type);
			ref refBaseShipA = GetRealShip(iShipA);
			string loclableA = "";
			if (CheckAttribute(rCity, "islandLable")) loclableA = GetConvertStr(rCity.islandLable, "LocLables.txt");
			if (rCity.Id == "Caiman" && CheckAttribute(PChar, "ColonyBuilding.ColonyName")) loclableA = PChar.ColonyBuilding.ColonyName;

			row = "tr" + cn;
			GameInterface.(_tabName).(row).index = chref.id;
			GameInterface.(_tabName).(row).td1.str = cn;
			GameInterface.(_tabName).(row).td1.scale = 0.8;
			GameInterface.(_tabName).(row).td1.color = iAucClr;
			GameInterface.(_tabName).(row).td2.icon.texture = InterfaceStates.InterfaceShipString + refBaseShipA.BaseName + ".dds";
			GameInterface.(_tabName).(row).td2.icon.uv = "0,0,1,1";
			GameInterface.(_tabName).(row).td2.icon.offset = "-2, 0";
			GameInterface.(_tabName).(row).td2.icon.width  = 40;
			GameInterface.(_tabName).(row).td2.icon.height = 40;
			GameInterface.(_tabName).(row).td3.str = refBaseShipA.Class;
			GameInterface.(_tabName).(row).td3.scale = 0.8;
			GameInterface.(_tabName).(row).td3.color = iAucClr;
			GameInterface.(_tabName).(row).td4.str = XI_ConvertString(RealShips[iShipA].BaseName) + " '" + chref.Ship.Name + "'";
			GameInterface.(_tabName).(row).td4.scale = 0.8;
			GameInterface.(_tabName).(row).td4.color = iAucClr;
			GameInterface.(_tabName).(row).td5.str = XI_ConvertString("PortAuction_AtSale");
			GameInterface.(_tabName).(row).td5.scale = 0.8;
			GameInterface.(_tabName).(row).td5.color = iAucClr;
			GameInterface.(_tabName).(row).td6.icon.group  = "NATIONS";
			GameInterface.(_tabName).(row).td6.icon.image  = Nations[sti(rCity.nation)].Name;
			GameInterface.(_tabName).(row).td6.icon.width  = 40;
			GameInterface.(_tabName).(row).td6.icon.height = 40;
			GameInterface.(_tabName).(row).td6.icon.offset = "-2, 0";
			GameInterface.(_tabName).(row).td6.str = GetConvertStr(rCity.id + " Town", "LocLables.txt");
			GameInterface.(_tabName).(row).td6.scale = 0.8;
			GameInterface.(_tabName).(row).td6.textoffset = "35, 0";
			GameInterface.(_tabName).(row).td6.color = iAucClr;
			GameInterface.(_tabName).(row).td7.str = loclableA;
			GameInterface.(_tabName).(row).td7.scale = 0.8;
			GameInterface.(_tabName).(row).td7.color = iAucClr;
			GameInterface.(_tabName).(row).td8.str = "" + sti(chref.ShipAtAuction.Days) + " " + XI_ConvertString("PortAuction_DaysShort");
			GameInterface.(_tabName).(row).td8.scale = 0.8;
			GameInterface.(_tabName).(row).td8.color = iAucClr;
			GameInterface.(_tabName).(row).td9.str = "" + sti(chref.ShipAtAuction.Target);
			GameInterface.(_tabName).(row).td9.scale = 0.8;
			GameInterface.(_tabName).(row).td9.color = iAucClr;
			iTotalGain = iTotalGain + sti(chref.ShipAtAuction.Target);
			cn++;
		}
	}
	// LEO: строка-итог под таблицей - общая ожидаемая выручка со всех текущих аукционов
	SetFormatedText("SHIPAUCTION_TOTAL", XI_ConvertString("PortAuction_TotalGain") + " " + iTotalGain);
	SetNodeUsing("SHIPAUCTION_TOTAL", true);
}

void FillShipPlaceTable(string _tabName)
{
	int	 i, cn, n;
	string  row;
	ref	 rCity, chref;

	if (g_iShipPlaceMode != 0) { FillShipAuctionTable(_tabName); return; } // LEO: режим аукциона - отдельная функция

	Table_Clear(_tabName, false, true, false); // LEO: чистим таблицу (для переключения режимов стоянка/аукцион)
	SetNodeUsing("SHIPAUCTION_TOTAL", false); // LEO: строка-итог аукциона - только в режиме аукциона
	// шапка -->
	GameInterface.(_tabName).select = 0;
	GameInterface.(_tabName).hr.td1.str = "N";
	GameInterface.(_tabName).hr.td1.scale = 0.9;
	GameInterface.(_tabName).hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_97", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td2.scale = 0.9;
	GameInterface.(_tabName).hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_98", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td3.scale = 0.9;
	GameInterface.(_tabName).hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_99", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td4.scale = 0.9;
	GameInterface.(_tabName).hr.td5.str = GetConvertStrWithReplace("Variable_QuestBook_100", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td5.scale = 0.9;
	GameInterface.(_tabName).hr.td6.str = GetConvertStrWithReplace("Variable_QuestBook_101", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td6.scale = 0.9;
	GameInterface.(_tabName).hr.td7.str = GetConvertStrWithReplace("Variable_QuestBook_102", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td7.scale = 0.9;
	GameInterface.(_tabName).hr.td8.str = GetConvertStrWithReplace("Variable_QuestBook_103", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td8.scale = 0.9;
	GameInterface.(_tabName).hr.td9.str = GetConvertStrWithReplace("Variable_QuestBook_104", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td9.scale = 0.9;
	// <--
	cn = 1;
	for (n=0; n<MAX_COLONIES; n++)
	{
		rCity = &Colonies[n];
		for (i=1; i<MAX_CHARACTERS; i++)
		{
			makeref(chrefsp, Characters[i]);
			if (CheckAttribute(chrefsp, "ShipInStockMan"))
			{
				if (chrefsp.ShipInStockMan == (rCity.id + "_PortMan"))
				{
					int iShip = chrefsp.ship.type;
					ref refBaseShip = GetRealShip(iShip);
					string shipTexture = refBaseShip.BaseName;
					string loclable = "";
					if (CheckAttribute(rCity, "islandLable")) loclable = GetConvertStr(rCity.islandLable, "LocLables.txt");
					if (rCity.Id == "Caiman" && CheckAttribute(PChar, "ColonyBuilding.ColonyName")) loclable = PChar.ColonyBuilding.ColonyName;

					row = "tr" + cn;
					GameInterface.(_tabName).(row).index = chrefsp.id;
					GameInterface.(_tabName).(row).td1.str = cn;
					GameInterface.(_tabName).(row).td1.scale = 0.8;
					GameInterface.(_tabName).(row).td2.icon.texture = InterfaceStates.InterfaceShipString + shipTexture + ".dds";
					GameInterface.(_tabName).(row).td2.icon.uv = "0,0,1,1";
					GameInterface.(_tabName).(row).td2.icon.offset = "-2, 0";
					GameInterface.(_tabName).(row).td2.icon.width  = 40;
					GameInterface.(_tabName).(row).td2.icon.height = 40;
					GameInterface.(_tabName).(row).td3.str = refBaseShip.Class;
					GameInterface.(_tabName).(row).td3.scale = 0.8;
					GameInterface.(_tabName).(row).td4.str = XI_ConvertString(RealShips[sti(chrefsp.Ship.Type)].BaseName) + " '" + chrefsp.Ship.Name + "'";
					GameInterface.(_tabName).(row).td4.scale = 0.8;

					GameInterface.(_tabName).(row).td4.textoffset = "0,6";//сдвинул текст чуть ниже, для более красивого расположения плашки, по умолчанию пустая строка IG Baron. 
					GameInterface.(_tabName).(row).td4.icon.width = 51;
					GameInterface.(_tabName).(row).td4.icon.height = 26;
					GameInterface.(_tabName).(row).td4.icon.offset = "44, -4";
					if (LanguageGetLanguage() != "russian") sTemp = "english "; else sTemp = "";
					GameInterface.(_tabName).(row).td4.icon.group = "ICONS_SPEC";
					GameInterface.(_tabName).(row).td4.icon.image = sTemp + "universal ship icon"; 
					if ( sti(refBaseShip.Type.Merchant) && !sti(refBaseShip.Type.War)) 
					GameInterface.(_tabName).(row).td4.icon.image = sTemp + "trader ship icon";
					if ( !sti(refBaseShip.Type.Merchant) && sti(refBaseShip.Type.War)) 
					GameInterface.(_tabName).(row).td4.icon.image = sTemp + "battle ship icon";//плашка для боевого типа корабля (есть ещё для быстрого fast ship icon)

					GameInterface.(_tabName).(row).td5.icon.texture = "INTERFACES\PORTRAITS\face_" + chrefsp.faceId + ".tga";
					GameInterface.(_tabName).(row).td5.icon.uv = "0,0,1,1";
					GameInterface.(_tabName).(row).td5.icon.offset = "-2, 0";
					GameInterface.(_tabName).(row).td5.icon.width  = 40;
					GameInterface.(_tabName).(row).td5.icon.height = 40;
					GameInterface.(_tabName).(row).td5.str = GetFullName(chrefsp);
					GameInterface.(_tabName).(row).td5.textoffset = "40, 0";
					GameInterface.(_tabName).(row).td5.scale = 0.8;
					GameInterface.(_tabName).(row).td6.icon.group  = "NATIONS";
					GameInterface.(_tabName).(row).td6.icon.image  = Nations[sti(rCity.nation)].Name;
					GameInterface.(_tabName).(row).td6.icon.width  = 40;
					GameInterface.(_tabName).(row).td6.icon.height = 40;
					GameInterface.(_tabName).(row).td6.icon.offset = "-2, 0";
					GameInterface.(_tabName).(row).td6.str = GetConvertStr(rCity.id + " Town", "LocLables.txt");
					GameInterface.(_tabName).(row).td6.scale = 0.8;
					GameInterface.(_tabName).(row).td6.textoffset = "35, 0";
					GameInterface.(_tabName).(row).td7.str = loclable;
					GameInterface.(_tabName).(row).td7.scale = 0.8;
					GameInterface.(_tabName).(row).td8.str = chrefsp.ShipInStockMan.AltDate;
					GameInterface.(_tabName).(row).td8.scale = 0.8;
					if (CheckAttribute(chrefsp, "ShipInStockMan.Unlimited")) // LEO: бессрочная стоянка - вместо суммы пишем "бессрочно"
						GameInterface.(_tabName).(row).td9.str = XI_ConvertString("PortStock_StoredUnlim");
					else
						GameInterface.(_tabName).(row).td9.str = chrefsp.ShipInStockMan.MoneyForShip;
					GameInterface.(_tabName).(row).td9.scale = 0.8;

					cn++;
				}
			}
		}
	}
	for (i=1; i<MAX_CHARACTERS; i++)
	{
		chref = &characters[i];
		if (CheckAttribute(chref, "OfficerInStockMan"))
		{
			row = "tr" + cn;
			GameInterface.(_tabName).(row).index = chref.id;
			GameInterface.(_tabName).(row).td1.str = "N";
			GameInterface.(_tabName).(row).td1.scale = 0.8;
			GameInterface.(_tabName).(row).td2.str = "---"
			GameInterface.(_tabName).(row).td3.str = "---";
			GameInterface.(_tabName).(row).td3.scale = 0.8;
			GameInterface.(_tabName).(row).td4.str = GetConvertStrWithReplace("Variable_QuestBook_105", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).(row).td4.scale = 0.8;
			GameInterface.(_tabName).(row).td5.icon.texture = "INTERFACES\PORTRAITS\face_" + chref.faceId + ".tga";
			GameInterface.(_tabName).(row).td5.icon.uv = "0,0,1,1";
			GameInterface.(_tabName).(row).td5.icon.offset = "-2, 0";
			GameInterface.(_tabName).(row).td5.icon.width  = 40;
			GameInterface.(_tabName).(row).td5.icon.height = 40;
			GameInterface.(_tabName).(row).td5.str = GetFullName(chref);
			GameInterface.(_tabName).(row).td5.textoffset = "40, 0";
			GameInterface.(_tabName).(row).td5.scale = 0.8;
			GameInterface.(_tabName).(row).td6.icon.group  = "NATIONS";
			if (!CheckAttribute(pchar, "PirateOrder"))GameInterface.(_tabName).(row).td6.icon.image  = "SPAIN";
			else GameInterface.(_tabName).(row).td6.icon.image  = "PIRATE";
			GameInterface.(_tabName).(row).td6.icon.width  = 40;
			GameInterface.(_tabName).(row).td6.icon.height = 40;
			GameInterface.(_tabName).(row).td6.icon.offset = "-2, 0";
			GameInterface.(_tabName).(row).td6.str = GetConvertStrWithReplace("Variable_QuestBook_106", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).(row).td6.scale = 0.8;
			GameInterface.(_tabName).(row).td6.textoffset = "35, 0";
			GameInterface.(_tabName).(row).td7.str = GetConvertStrWithReplace("Variable_QuestBook_107", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).(row).td7.scale = 0.8;
			GameInterface.(_tabName).(row).td8.str = GetConvertStrWithReplace("Variable_QuestBook_108", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).(row).td8.scale = 0.8;
			GameInterface.(_tabName).(row).td9.str = "---";
			GameInterface.(_tabName).(row).td9.scale = 0.8;

			cn++;
		}
	}
	Table_UpdateWindow(_tabName);
}

void FillPriceListTown(string _tabName)
{
	int	 i, cn, n, StoreNum;
	ref	 chref, npchar;
	string  row, firstId, firstIdX, CityId;
	aref	curItem;
	ref	 rCity;
	ref		refStorage;

	// шапка -->
	GameInterface.(_tabName).select = 0;
	GameInterface.(_tabName).hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_109", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td1.scale = 0.75;
	GameInterface.(_tabName).hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_110", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td2.scale = 0.75;
	GameInterface.(_tabName).hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_111", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td3.scale = 0.75;
	GameInterface.(_tabName).hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_112", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td4.scale = 0.75;
	GameInterface.(_tabName).hr.td5.str = GetConvertStrWithReplace("Variable_QuestBook_113", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td5.scale = 0.75;

	cn = 1;
	for (i=1; i<MAX_CHARACTERS; i++)
	{
		makeref(chref, Characters[i]);
		if (CheckAttribute(chref, "Storage.Activate"))//склады
		{
			row = "tr" + cn;
			CityId = chref.city;
			rCity = GetColonyByIndex(FindColony(CityId));
			StoreNum = FindStore(CityId);
			refStorage = &stores[StoreNum];
			if (n == 0) {firstId = rCity.id; firstIdX = 0;}
			if (rCity.id == "Caiman") rCity.islandLable = "Caiman";
			GameInterface.(_tabName).(row).UserData.CityID  = rCity.id;
			GameInterface.(_tabName).(row).UserData.CityIDX = 0;//склад
			GameInterface.(_tabName).(row).td1.icon.group  = "NATIONS";
			GameInterface.(_tabName).(row).td1.icon.image  = Nations[sti(rCity.nation)].Name;
			GameInterface.(_tabName).(row).td1.icon.width  = 26;
			GameInterface.(_tabName).(row).td1.icon.height = 26;
			GameInterface.(_tabName).(row).td1.icon.offset = "0, 3";
			GameInterface.(_tabName).(row).td2.str = GetConvertStrWithReplace("Variable_QuestBook_114", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).(row).td2.scale = 0.8;
			GameInterface.(_tabName).(row).td3.str = GetConvertStr(rCity.islandLable, "LocLables.txt") + "\n" + GetConvertStr(rCity.id + " Town", "LocLables.txt");
			GameInterface.(_tabName).(row).td3.scale = 0.65;
			GameInterface.(_tabName).(row).td4.str = GetStorageUsedWeight(refStorage) + " / " + iMaxGoodsStore;
			GameInterface.(_tabName).(row).td4.scale = 0.8;
			GameInterface.(_tabName).(row).td5.str = GetNpcQuestPastMonthParam(chref, "Storage.Date") * sti(chref.Storage.MoneyForStorage);
			GameInterface.(_tabName).(row).td5.scale = 0.8;
			cn++;
		}
		if (CheckAttribute(chref, "ShipInStockMan"))//корабли в ПУ
		{
			row = "tr" + cn;
			npchar = &characters[GetCharacterIndex(chref.ShipInStockMan)];
			CityId = npchar.city;
			rCity = GetColonyByIndex(FindColony(CityId));
			string loclable = "";
			if (CheckAttribute(rCity, "islandLable")) loclable = GetConvertStr(rCity.islandLable, "LocLables.txt");
			if (rCity.Id == "Caiman" && CheckAttribute(PChar, "ColonyBuilding.ColonyName")) loclable = PChar.ColonyBuilding.ColonyName;
			if (n == 0) {firstId = chref.id; firstIdX = 1;}
			GameInterface.(_tabName).(row).UserData.CityID  = chref.id;
			GameInterface.(_tabName).(row).UserData.CityIDX = 1;//корабль
			GameInterface.(_tabName).(row).td1.icon.group  = "NATIONS";
			GameInterface.(_tabName).(row).td1.icon.image  = Nations[sti(rCity.nation)].Name;
			GameInterface.(_tabName).(row).td1.icon.width  = 26;
			GameInterface.(_tabName).(row).td1.icon.height = 26;
			GameInterface.(_tabName).(row).td1.icon.offset = "0, 3";
			GameInterface.(_tabName).(row).td2.str = GetConvertStrWithReplace("Variable_QuestBook_115", "Interface.txt", "#space#", " ") + XI_ConvertString(RealShips[sti(chref.Ship.Type)].BaseName) + "\n'" + chref.Ship.Name + "'";
			GameInterface.(_tabName).(row).td2.scale = 0.6;
			GameInterface.(_tabName).(row).td3.str = loclable + "\n" + GetConvertStr(rCity.id + " Town", "LocLables.txt");
			GameInterface.(_tabName).(row).td3.scale = 0.65;
			GameInterface.(_tabName).(row).td4.str = RecalculateCargoLoad(chref) + " / " + GetCargoMaxSpace(chref);
			GameInterface.(_tabName).(row).td4.scale = 0.8;
			// LEO: бессрочная стоянка оплачена разово - долга нет (раньше книга считала его помесячно, репорт 24.09.2026)
			if (CheckAttribute(chref, "ShipInStockMan.Unlimited")) GameInterface.(_tabName).(row).td5.str = GetConvertStrWithReplace("Variable_QuestBook_108", "Interface.txt", "#space#", " ");
			else GameInterface.(_tabName).(row).td5.str = GetNpcQuestPastMonthParam(chref, "ShipInStockMan.Date") * sti(chref.ShipInStockMan.MoneyForShip);
			GameInterface.(_tabName).(row).td5.scale = 0.8;
			cn++;
		}
		if (CheckAttribute(chref, "StorageOpen"))//склады
		{
			row = "tr" + cn;
			CityId = chref.city;
			rCity = GetColonyByIndex(FindColony(CityId));
			if (n == 0) {firstId = rCity.id; firstIdX = 0;}
			GameInterface.(_tabName).(row).UserData.CityID  = chref.id;
			GameInterface.(_tabName).(row).UserData.CityIDX = 2;
			GameInterface.(_tabName).(row).td1.icon.group  = "NATIONS";
			GameInterface.(_tabName).(row).td1.icon.image  = Nations[sti(rCity.nation)].Name;
			GameInterface.(_tabName).(row).td1.icon.width  = 26;
			GameInterface.(_tabName).(row).td1.icon.height = 26;
			GameInterface.(_tabName).(row).td1.icon.offset = "0, 3";
			GameInterface.(_tabName).(row).td2.str = GetConvertStrWithReplace("Variable_QuestBook_116", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).(row).td2.scale = 0.65;
			GameInterface.(_tabName).(row).td3.str = GetConvertStr(rCity.islandLable, "LocLables.txt") + "\n" + GetConvertStr(rCity.id + " Town", "LocLables.txt");
			GameInterface.(_tabName).(row).td3.scale = 0.65;
			GameInterface.(_tabName).(row).td4.str = FloatToString(GetItemsWeight(chref), 1)+" / ---";
			GameInterface.(_tabName).(row).td4.scale = 0.85;
			GameInterface.(_tabName).(row).td5.str = GetNpcQuestPastMonthParam(chref, "Storage.Date") * sti(chref.StoragePrice);
			GameInterface.(_tabName).(row).td5.scale = 0.8;
			cn++;
		}
	}
	Table_UpdateWindow(_tabName);
	FillPriceList("TABLE_GOODS", firstId, firstIdX);
}

void TableSelectChange()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();
	CurTable = sControl;
	CurRow   =  "tr" + (iSelected);
	NullSelectTable("TABLE_GOODS");
	if(CurTable == "TABLE_CITY") {FillPriceList("TABLE_GOODS", GameInterface.(CurTable).(CurRow).UserData.CityID, GameInterface.(CurTable).(CurRow).UserData.CityIDX);}
	if(CurTable == "SHIP_TABLE_LIST_LEFT") {Last_Left_Ship = sti(GameInterface.(CurTable).(CurRow).index); SetShipOTHERTableInfo("SHIP_TABLE_OTHER_LEFT");}//запоминаем новый выбранный корабль и обновляем таблицу
	if(CurTable == "SHIP_TABLE_LIST_RIGHT") {Last_Right_Ship = sti(GameInterface.(CurTable).(CurRow).index); SetShipOTHERTableInfo("SHIP_TABLE_OTHER_RIGHT");}
	if(CurTable == "TABLE_ACHIEVEMENTS") fill_bars();
}

void NullSelectTable(string sControl)
{
	if (sControl != CurTable)
	{
		GameInterface.(sControl).select = 0;
		Table_UpdateWindow(sControl);
	}
}

void FillPriceList(string _tabName, string  attr1, string attr2)
{
	string  sGoods;
	int		StoreNum, iStoreQ;
	int	 i, n;
	ref	 nulChr;
	string  row;
	ref		refStorage;
	Table_Clear(_tabName, false, true, false);
	// шапка -->
	GameInterface.(_tabName).select = 0;
	GameInterface.(_tabName).hr.td1.str = XI_ConvertString("Good name");
	GameInterface.(_tabName).hr.td1.scale = 0.75;
	GameInterface.(_tabName).hr.td2.str = XI_ConvertString("In the storage");
	GameInterface.(_tabName).hr.td2.scale = 0.75;
	GameInterface.(_tabName).hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_117", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td3.scale = 0.75;
	GameInterface.(_tabName).hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_118", "Interface.txt", "#space#", " ");
	GameInterface.(_tabName).hr.td4.scale = 0.75;

	n = 1;
	if (attr1 != "")
	{
		if (attr2 == "2" && HasSubStr(attr1,"_usurer"))
		{
			ref usurer = CharacterFromID(attr1);
			aref curitem,arItem,rootItems;
			makearef(rootItems, usurer.Items);
			string sGood;
			GameInterface.(_tabName).hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_119", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_120", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_121", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_122", "Interface.txt", "#space#", " ");
			int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
			for (i = 0; i < GetAttributesNum(rootItems); i++)
			{
				curItem = GetAttributeN(rootItems, i);
				sGood = GetAttributeName(curItem);

				if (Items_FindItem(sGood, &arItem)>=0 )
				{
					row = "tr" + n;
					if (GetCharacterItem(usurer, sGood) > 0)
					{
						GameInterface.(_tabName).(row).id = sGood;
						GameInterface.(_tabName).(row).td1.icon.group = arItem.picTexture;
						GameInterface.(_tabName).(row).td1.icon.image = "itm" + arItem.picIndex;
						GameInterface.(_tabName).(row).td1.icon.offset = "-2, 0";
						GameInterface.(_tabName).(row).td1.icon.width = 32;
						GameInterface.(_tabName).(row).td1.icon.height = 32;
						GameInterface.(_tabName).(row).td1.textoffset = "31,0";
						GameInterface.(_tabName).(row).td1.str = LanguageConvertString(idLngFile, arItem.name);
						GameInterface.(_tabName).(row).td1.scale = 0.85;
						GameInterface.(_tabName).(row).td2.str   = FloatToString(GetItemWeight(sGood), 1);
						GameInterface.(_tabName).(row).td2.scale = 0.9;
						GameInterface.(_tabName).(row).td3.str   = GetCharacterItem(usurer, sGood);
						GameInterface.(_tabName).(row).td3.scale = 0.9;
						GameInterface.(_tabName).(row).td4.str   = FloatToString(GetItemWeight(sGood) * sti(GetCharacterItem(usurer, sGood)), 1);
						GameInterface.(_tabName).(row).td4.scale = 0.9;
						n++;
					}
				}
			}
			LanguageCloseFile(idLngFile);
		}
		else
		{
			if (attr2 == "1") StoreNum = GetCharacterIndex(attr1); else StoreNum = FindStore(attr1);
			if(StoreNum > -1)
			{

				if (attr2 == "1") refStorage = &characters[StoreNum]; else refStorage = &stores[StoreNum];
				for (i = 0; i < GOODS_QUANTITY; i++)
				{
					row = "tr" + n;
					if (attr2 == "1") iStoreQ = GetCargoGoods(refStorage, i); else iStoreQ = GetStorageGoodsQuantity(refStorage, i);
					if (iStoreQ == 0) continue;
					GameInterface.(_tabName).(row).UserData.ID = Goods[i].name;
					GameInterface.(_tabName).(row).UserData.IDX = i;
					GameInterface.(_tabName).(row).td1.icon.group = "GOODS";
					GameInterface.(_tabName).(row).td1.icon.image = Goods[i].name;
					GameInterface.(_tabName).(row).td1.icon.offset = "1, 0";
					GameInterface.(_tabName).(row).td1.icon.width = 32;
					GameInterface.(_tabName).(row).td1.icon.height = 32;
					GameInterface.(_tabName).(row).td1.textoffset = "30,0";
					GameInterface.(_tabName).(row).td1.str = XI_ConvertString(Goods[i].name);
					GameInterface.(_tabName).(row).td1.scale = 0.85;
					GameInterface.(_tabName).(row).td2.str = iStoreQ;
					GameInterface.(_tabName).(row).td3.str = Goods[i].Units;
					GameInterface.(_tabName).(row).td4.str = GetGoodWeightByType(i, iStoreQ);
					n++;
				}
			}
		}

	}
	Table_UpdateWindow(_tabName);
}
void FillShipInfoEncy(string _tabName)
{
	Table_Clear(_tabName, false, true, false);
	string sRow;

	GameInterface.(_tabName).hr.td1.str = GetConvertStr("Variable_QuestBook_125", "Interface.txt");
	GameInterface.(_tabName).hr.td1.scale = 0.8;
	GameInterface.(_tabName).hr.td2.str = GetConvertStr("Variable_QuestBook_124", "Interface.txt");
	GameInterface.(_tabName).hr.td2.scale = 0.8;
	GameInterface.(_tabName).hr.td3.str = GetConvertStr("Variable_shipyard_29", "Interface.txt");
	GameInterface.(_tabName).hr.td3.scale = 0.8;

	aref   arDest, arImt;
	string sAttr;
	int	iNum, iShip;
	ref	refBaseShip;
	string sShip, sTemp;
	int k = 0;
	ref chrefsp;
	makeref(chrefsp, Characters[GenerateCharacter(4,WITHOUT_SHIP,"pirate",MAN,1,WARRIOR)]);
	for (int i = 0; i < SHIP_TYPES_QUANTITY - 1;i++)
	{
	string spectypes = "";
	sRow = "tr" + (k+1);
	makeref(refBaseShip,ShipsTypes[i]);
	sShip = refBaseShip.Name;
	chrefsp.Ship.Type = sShip;
	if (i<SHIP_LUGGERQUEST && !CheckAttribute(Pchar,"Encyclopedia."+sShip) && bFillEncyShips) continue;//квестовые показываем! они же не идут в список достижения???

	int nClass = sti(refBaseShip.Class);
	string snation;
	if (_tabName == "SHIP_TABLE_LIST_LEFT" && FState_SHIP_L != 8 && FState_SHIP_L != nClass) continue;
	if (_tabName == "SHIP_TABLE_LIST_RIGHT" && FState_SHIP_R != 8 && FState_SHIP_R != nClass) continue;
	if (_tabName == "SHIP_TABLE_LIST_RIGHT" && FState_SHIP_N_R && !checkattribute(refBaseShip, "QuestShip")) //квестовые игнорируют выбор нации, всё равно показываются
		{snation = ShipNation[FState_SHIP_N_R-1]; if (refBaseShip.nation.(snation)) {} else continue;}
	if (_tabName == "SHIP_TABLE_LIST_LEFT" && FState_SHIP_N_L && !checkattribute(refBaseShip, "QuestShip"))
		{snation = ShipNation[FState_SHIP_N_L-1]; if (refBaseShip.nation.(snation)) {} else continue;}

	GameInterface.(_tabName).(sRow).index = i;
	GameInterface.(_tabName).(sRow).td1.icon.texture = InterfaceStates.InterfaceShipString + sShip + ".dds";
	GameInterface.(_tabName).(sRow).td1.icon.uv = "0,0,1,1";
	GameInterface.(_tabName).(sRow).td1.icon.offset = "-2, 0";
	GameInterface.(_tabName).(sRow).td1.icon.width  = 59;
	GameInterface.(_tabName).(sRow).td1.icon.height = 59;
	GameInterface.(_tabName).(sRow).td1.str = sti(refBaseShip.Class);
	GameInterface.(_tabName).(sRow).td1.textoffset = "48,-20";

	GameInterface.(_tabName).(sRow).td2.str = XI_Convertstring(sShip);
	GameInterface.(_tabName).(sRow).td2.textoffset = "0,6";//сдвинул текст чуть ниже, для более красивого расположения плашки, по умолчанию пустая строка IG Baron. 
	GameInterface.(_tabName).(sRow).td2.icon.width = 51;
	GameInterface.(_tabName).(sRow).td2.icon.height = 26;
	GameInterface.(_tabName).(sRow).td2.icon.offset = "0, -4";
	if (LanguageGetLanguage() != "russian") sTemp = "english "; else sTemp = "";
	GameInterface.(_tabName).(sRow).td2.icon.group = "ICONS_SPEC";
	GameInterface.(_tabName).(sRow).td2.icon.image = sTemp + "universal ship icon"; 
	if ( sti(shipstypes[i].Type.Merchant) && !sti(shipstypes[i].Type.War)) 
		GameInterface.(_tabName).(sRow).td2.icon.image = sTemp + "trader ship icon";
	if ( !sti(shipstypes[i].Type.Merchant) && sti(shipstypes[i].Type.War)) 
		GameInterface.(_tabName).(sRow).td2.icon.image = sTemp + "battle ship icon";//плашка для боевого типа корабля (есть ещё для быстрого fast ship icon)

	GameInterface.(_tabName).(sRow).td3.str = shipstypes[i].rcannon + "\n" + shipstypes[i].fcannon + "ðñ::::::::::ð" + shipstypes[i].bcannon + "\n" + shipstypes[i].lcannon;//сместить отдельные строчки?
	if (refBaseShip.Type.War == true) spectypes = GetCOnvertStr("Variable_specType_War", "Interface.txt");
	if (refBaseShip.Type.Merchant == true) spectypes = GetCOnvertStr("Variable_specType_Trade", "Interface.txt");
	if (refBaseShip.Type.Merchant == true && refBaseShip.Type.War == true) spectypes = GetCOnvertStr("Variable_specType_Universal", "Interface.txt");

	k++;
	}
	ShowInfoWindowEncyShip();
	int iselected = FindLastShip(_tabName);//если отфильтровался, то вернётся -1
	GameInterface.(_tabName).select = iselected+1;
	if (iselected < 2) GameInterface.(_tabName).top = 0; else GameInterface.(_tabName).top = iselected-2;
	if (_tabName == "SHIP_TABLE_LIST_LEFT") SetShipOTHERTableInfo("SHIP_TABLE_OTHER_LEFT"); else SetShipOTHERTableInfo("SHIP_TABLE_OTHER_RIGHT");

	Table_UpdateWindow(_tabName);
}

int FindLastShip(string _tabName)
{
	int q;
	string sRow;
	if (_tabName == "SHIP_TABLE_LIST_LEFT")
	{
		for (q = 0; q<(SHIP_TYPES_QUANTITY+1);q++)
		{
		sRow = "tr" + (q+1);

		if (!CheckAttribute(GameInterface, "SHIP_TABLE_LIST_LEFT." + sRow + ".index")) {Last_Left_Ship = -1; return -1;}//таблица кончилась, корабль не нашли, сбрасываем выделение
		if (GameInterface.(_tabName).(sRow).index == Last_Left_Ship) return q;//корабль не отфильтровался, он в строчке q+1
		}
		Last_Left_Ship = -1;
	}
	else
	{
		for (q = 0; q<(SHIP_TYPES_QUANTITY+1);q++)
		{
		sRow = "tr" + (q+1);
		if (!CheckAttribute(GameInterface, "SHIP_TABLE_LIST_RIGHT." + sRow + ".index")) {Last_Right_Ship = -1; return -1;}
		if (GameInterface.(_tabName).(sRow).index == Last_Right_Ship) return q;
		}
		Last_Right_Ship = -1;
	}
	return -1;
}

void SetShipOTHERTableInfo(string _tabName)
{
	int iShip;
	if (_tabName == "SHIP_TABLE_OTHER_LEFT") iShip = Last_Left_Ship; else iShip = Last_Right_Ship;
	Table_Clear(_tabName, false, true, false);

	if (iShip > -1)
	{
		int	 i;
		string  row;
		string sShip;
		ref refBaseShip;
		makeref(refBaseShip,ShipsTypes[iShip]);
		sShip = refBaseShip.Name;

		GameInterface.(_tabName).select = 0;
		string std1, std2, std3, std4, sleft, sright;
		std1 = "td1"; std2 = "td2"; std3 = "td3"; std4 = "td4"; sleft = "left"; sright = "right";
		if (_tabName == "SHIP_TABLE_OTHER_RIGHT")
		{std1 = "td4"; std2 = "td3"; std3 = "td2"; std4 = "td1"; sleft = "right"; sright = "left";}

		GameInterface.(_tabName).hr.(std1).str = "";
		for (i=1; i<=10; i++)
		{
			row = "tr" + i;

			GameInterface.(_tabName).(row).(std4).str = "";
			GameInterface.(_tabName).(row).(std1).icon.width = 24;
			GameInterface.(_tabName).(row).(std1).icon.height = 24;
			GameInterface.(_tabName).(row).(std1).icon.offset = "2, 1";
			GameInterface.(_tabName).(row).(std2).align = sleft;
			GameInterface.(_tabName).(row).(std2).scale = 0.8;
			GameInterface.(_tabName).(row).(std2).textoffset = "0,0";
			GameInterface.(_tabName).(row).(std3).align = sright;
			GameInterface.(_tabName).(row).(std3).scale = 0.8;
		}
		GameInterface.(_tabName).tr1.UserData.ID = "Hull";
		GameInterface.(_tabName).tr1.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr1.(std1).icon.image = "Hull";
		GameInterface.(_tabName).tr1.(std2).str = XI_ConvertString("Hull");
		GameInterface.(_tabName).tr1.(std3).str = sti(refBaseShip.hp);

		GameInterface.(_tabName).tr2.UserData.ID = "Sails";
		GameInterface.(_tabName).tr2.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr2.(std1).icon.image = "Sails";
		GameInterface.(_tabName).tr2.(std2).str = XI_ConvertString("Sails");
		GameInterface.(_tabName).tr2.(std3).str = sti(refBaseShip.sp);

		GameInterface.(_tabName).tr3.UserData.ID = "MastHP";
		GameInterface.(_tabName).tr3.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr3.(std1).icon.image = "MastHP";
		GameInterface.(_tabName).tr3.(std2).str = XI_ConvertString("MastHP");
		GameInterface.(_tabName).tr3.(std3).str = FloatToString(stf(refBaseShip.MastMultiplier),2);

		GameInterface.(_tabName).tr4.UserData.ID = "Speed";
		GameInterface.(_tabName).tr4.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr4.(std1).icon.image = "Speed";
		GameInterface.(_tabName).tr4.(std2).str = XI_ConvertString("Speed");

		GameInterface.(_tabName).tr4.(std3).str = FloatToString(stf(refBaseShip.SpeedRate),2);

		GameInterface.(_tabName).tr5.UserData.ID = "Maneuver";
		GameInterface.(_tabName).tr5.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr5.(std1).icon.image = "Maneuver";
		GameInterface.(_tabName).tr5.(std2).str = XI_ConvertString("Maneuver");

		GameInterface.(_tabName).tr5.(std3).str = FloatToString(stf(refBaseShip.TurnRate),2);

		GameInterface.(_tabName).tr6.UserData.ID = "AgainstWind";
		GameInterface.(_tabName).tr6.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr6.(std1).icon.image = "AgainstWind";
		GameInterface.(_tabName).tr6.(std2).str = XI_ConvertString("AgainstWind");
		GameInterface.(_tabName).tr6.(std3).str = FloatToString(stf(refBaseShip.WindAgainstSpeed),2);

		GameInterface.(_tabName).tr7.UserData.ID = "Capacity";
		GameInterface.(_tabName).tr7.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr7.(std1).icon.image = "Capacity";
		GameInterface.(_tabName).tr7.(std2).str = XI_ConvertString("Capacity");
		GameInterface.(_tabName).tr7.(std3).str = refBaseShip.Capacity;

		GameInterface.(_tabName).tr8.UserData.ID = "Crew";
		GameInterface.(_tabName).tr8.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr8.(std1).icon.image = "Crew";
		GameInterface.(_tabName).tr8.(std2).str = XI_ConvertString("Crew");
		GameInterface.(_tabName).tr8.(std3).str = sti(refBaseShip.MinCrew) +"/" + sti(refBaseShip.OptCrew);

		GameInterface.(_tabName).tr9.UserData.ID = "sCannons";
		GameInterface.(_tabName).tr9.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr9.(std1).icon.image = "Caliber";
		GameInterface.(_tabName).tr9.(std2).str = XI_ConvertString("sCannons");
		GameInterface.(_tabName).tr9.(std3).str = XI_ConvertString("caliber" + refBaseShip.MaxCaliber)

		GameInterface.(_tabName).tr10.UserData.ID = "CannonType";
		GameInterface.(_tabName).tr10.(std1).icon.group = "ICONS_CHAR";
		GameInterface.(_tabName).tr10.(std1).icon.image = "Cannons";
		GameInterface.(_tabName).tr10.(std2).str = XI_ConvertString("Cannon" + "s2");

		if (refBaseShip.CannonsQuantity == 0)
		{
			GameInterface.(_tabName).tr10.(std3).str = GetConvertStrWithReplace("Variable_QuestBook_130", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).tr10.(std3).align = sright;
			GameInterface.(_tabName).tr10.(std3).scale = 0.8;
		}
		else
		{
			GameInterface.(_tabName).tr10.(std3).str =  refBaseShip.CannonsQuantity;
			GameInterface.(_tabName).tr10.(std3).align = sright;
			GameInterface.(_tabName).tr10.(std3).scale = 0.8;
		}
	}
//-->показываем флаги, для обеих сторон, так как при фильтрации обе стороны скрываются
	if (Last_Left_Ship>-1)
	{
		ref refBaseShipL, refBaseShipR;
		makeref(refBaseShipL,ShipsTypes[Last_Left_Ship]);
		bool bQuest = false;
		if (checkattribute(refBaseShipL, "QuestShip")) bQuest = true; //не показываем флаги наций квестовым
		if (!bQuest && refBaseShipL.nation.england == true) SetNewPicture("S_NATION_E_L", "loading\Enc_Eng.dds"); else SetNewPicture("S_NATION_E_L", "");
		if (!bQuest && refBaseShipL.nation.france == true) SetNewPicture("S_NATION_F_L", "loading\Enc_Fra.dds"); else SetNewPicture("S_NATION_F_L", "");
		if (!bQuest && refBaseShipL.nation.holland == true) SetNewPicture("S_NATION_H_L", "loading\Enc_Hol.dds"); else SetNewPicture("S_NATION_H_L", "");
		if (!bQuest && refBaseShipL.nation.spain == true) SetNewPicture("S_NATION_S_L", "loading\Enc_Spa.dds"); else SetNewPicture("S_NATION_S_L", "");
		if (!bQuest && refBaseShipL.nation.pirate == true) SetNewPicture("S_NATION_P_L", "loading\Enc_Pir.dds"); else SetNewPicture("S_NATION_P_L", "");

		if (bBettaTestMode) {if (_tabName == "SHIP_TABLE_OTHER_LEFT") SetFormatedText("SHIP_INFO_TEXT_LEFT", GetConvertStrWithReplace("Variable_QuestBook_131", "Interface.txt", "#space#", " ")+refBaseShipL.name+"\n"+GetConvertStr(sShip, "ShipsDescribe.txt"));}
		else {if (_tabName == "SHIP_TABLE_OTHER_LEFT") SetFormatedText("SHIP_INFO_TEXT_LEFT", GetConvertStr(sShip, "ShipsDescribe.txt"));}
	}
	if (Last_Right_Ship>-1)
	{
		makeref(refBaseShipR,ShipsTypes[Last_Right_Ship]);
		bQuest = false;
		if (checkattribute(refBaseShipR, "QuestShip")) bQuest = true; //не показываем флаги наций квестовым
		if (!bQuest && refBaseShipR.nation.england == true) SetNewPicture("S_NATION_E_R", "loading\Enc_Eng.dds"); else SetNewPicture("S_NATION_E_R", "");
		if (!bQuest && refBaseShipR.nation.france == true) SetNewPicture("S_NATION_F_R", "loading\Enc_Fra.dds"); else SetNewPicture("S_NATION_F_R", "");
		if (!bQuest && refBaseShipR.nation.holland == true) SetNewPicture("S_NATION_H_R", "loading\Enc_Hol.dds"); else SetNewPicture("S_NATION_H_R", "");
		if (!bQuest && refBaseShipR.nation.spain == true) SetNewPicture("S_NATION_S_R", "loading\Enc_Spa.dds"); else SetNewPicture("S_NATION_S_R", "");
		if (!bQuest && refBaseShipR.nation.pirate == true) SetNewPicture("S_NATION_P_R", "loading\Enc_Pir.dds"); else SetNewPicture("S_NATION_P_R", "");

		if (bBettaTestMode) {if (_tabName == "SHIP_TABLE_OTHER_RIGHT") SetFormatedText("SHIP_INFO_TEXT_RIGHT", GetConvertStrWithReplace("Variable_QuestBook_132", "Interface.txt", "#space#", " ")+refBaseShipR.name+"\n"+GetConvertStr(sShip, "ShipsDescribe.txt"));}
		else {if (_tabName == "SHIP_TABLE_OTHER_RIGHT") SetFormatedText("SHIP_INFO_TEXT_RIGHT", GetConvertStr(sShip, "ShipsDescribe.txt"));}
	}
//<--показываем флаги
	Paint_ShipOTHER_Diff();
	Table_UpdateWindow("SHIP_TABLE_OTHER_LEFT");
	Table_UpdateWindow("SHIP_TABLE_OTHER_RIGHT");
}

void GiveItemToTraderByType(aref ch, string type)
{
	int i, irand;
	String itemID;
	ref itm;

	DeleteAttribute(ch, "items");
	for(i = 0; i < ITEMS_QUANTITY; i++)
	{
		itemID = Items[i].ID;
		irand = 1;
		itm = ItemsFromID(itemID);
		switch (type)
		{
			case "blade":
				if (CheckAttribute(itm,"groupID")){
					if (itm.groupID == BLADE_ITEM_TYPE) AddItems(ch, itemID, irand);
				}
			break;
			case "gun":
				if (CheckAttribute(itm,"groupID")){
					if (itm.groupID == GUN_ITEM_TYPE || itm.groupID == AMMO_ITEM_TYPE)
					{
						irand = 2;
						AddItems(ch, itemID, irand);
					}
				}
			break;
			case "food":
				if (CheckAttribute(itm,"ItemType") && !CheckAttribute(itm,"groupID")){
					if (itm.ItemType == "Support") AddItems(ch, itemID, irand);
				}
			break;
			case "bonusitem":
				if (bBettaTestMode) AddItems(ch,itemID, irand);
				else if (HasSubStr(itm.id,"backpack") || HasSubStr(itm.id,"talisman") || HasSubStr(itm.id,"cirass") || HasSubStr(itm.id,"jewelry") || HasSubStr(itm.id,"indian") || HasSubStr(itm.id,"Totem") || HasSubStr(itm.id,"mineral") || HasSubStr(itm.id,"Dozor_H") || HasSubStr(itm.id,"Dozor_Mi") || HasSubStr(itm.id,"Dozor_S") || HasSubStr(itm.id,"suit") || HasSubStr(itm.id,"Strange") || HasSubStr(itm.id,"statue") || HasSubStr(itm.id,"scul")) AddItems(ch, itemID, irand);
			break;
		}
	}
}

void AddToTable(string _tabName, string type)
{
	int n, i;
	string row, sShipGroup;
	string sGood;
	int  idLngFile;
	int leftQty, rightQty;
	n = 1;
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	Table_Clear(_tabName, false, true, false);
	GameInterface.(_tabName).top = 0;
	GameInterface.(_tabName).select = 0;
	makeref(chrefsp, Characters[GenerateCharacter(4,WITHOUT_SHIP,"pirate",MAN,1,WARRIOR)]);
	GiveItemToTraderByType(chrefsp, type);
	switch (type)
	{
		case "blade":
			ShowInfoWindowEncyBlade();
			GameInterface.(_tabName).hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_133", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td1.scale = 1.1;
			GameInterface.(_tabName).hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_133.1", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td2.scale = 1.1;
			GameInterface.(_tabName).hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_134", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td3.scale = 1.1;
			GameInterface.(_tabName).hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_135", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td4.scale = 1.1;
			GameInterface.(_tabName).hr.td5.str = GetConvertStrWithReplace("Variable_QuestBook_136", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td5.scale = 1.1;
		break;
		case "gun": ShowInfoWindowEncyGun();
			GameInterface.(_tabName).hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_137", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td1.scale = 1.1;
			GameInterface.(_tabName).hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_133.1", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td2.scale = 1.1;
			GameInterface.(_tabName).hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_138", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td3.scale = 1.1;
			GameInterface.(_tabName).hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_139", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td4.scale = 1.1;
			GameInterface.(_tabName).hr.td5.str = GetConvertStrWithReplace("Variable_QuestBook_140", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td5.scale = 1.1;
		break;
		case "food": ShowInfoWindowEncyFood();
			GameInterface.(_tabName).hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_141", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td1.scale = 1.1;
			GameInterface.(_tabName).hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_141.1", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td2.scale = 1.1;
			GameInterface.(_tabName).hr.td3.str = GetConvertStrWithReplace("Variable_QuestBook_142", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td3.scale = 1.1;
			GameInterface.(_tabName).hr.td4.str = GetConvertStrWithReplace("Variable_QuestBook_143", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td4.scale = 1.1;
		break;
		case "bonusitem": ShowInfoWindowEncyBonusItem();
			GameInterface.(_tabName).hr.td1.str = GetConvertStrWithReplace("Variable_QuestBook_144", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td1.scale = 1.1;
			GameInterface.(_tabName).hr.td2.str = GetConvertStrWithReplace("Variable_QuestBook_145", "Interface.txt", "#space#", " ");
			GameInterface.(_tabName).hr.td2.scale = 1.1;
		break;
	}

	for (i = 0; i < ITEMS_QUANTITY; i++)
	{
		row = "tr" + n;
		sGood = Items[i].id;
		// проверка на экипировку, их не продаем
		leftQty  = GetCharacterFreeItem(chrefsp, sGood);

		if (leftQty > 0)
		{
			if (type == "blade" && FState_BLADE && Items[i].FencingType != BladeType[FState_BLADE-1]) continue;//фильтрация типа легкое тяжёлое

			if (type == "blade" && !checkattribute(&Items[i],"quality")) continue;//отфильтровываю GetConvertStrWithReplace("Variable_QuestBook_146", "Interface.txt", "#space#", " ") и всякие посторонние предметы, у которых нет GetConvertStrWithReplace("Variable_QuestBook_147", "Interface.txt", "#space#", " ")
			if (type == "blade" && FState_BLADEQ && Items[i].quality != BladeTypeQ[FState_BLADEQ-1]) continue;//фильтрация клинков по качеству

			if (type == "gun" && FState_GunQ) {if (Items[i].groupID == AMMO_ITEM_TYPE || !checkattribute(&Items[i],"quality")) continue;}
			if (type == "gun" && FState_GunQ && Items[i].quality != GunTypeQ[FState_GunQ-1]) continue;//фильтрация по качеству

			if (type == "bonusitem" && FState_BONUS && !HasSubStr(sGood, BonusType[FState_BONUS-1])) continue;//фильтрация типа предметов

			GameInterface.(_tabName).(row).td1.icon.group = Items[i].picTexture;
			GameInterface.(_tabName).(row).td1.icon.image = "itm" + Items[i].picIndex;
			GameInterface.(_tabName).(row).td1.icon.offset = "-2, 0";
			GameInterface.(_tabName).(row).td1.icon.width = 60;
			GameInterface.(_tabName).(row).td1.icon.height = 60;
			GameInterface.(_tabName).(row).td1.textoffset = "60,0";
			if (bBettaTestMode) GameInterface.(_tabName).(row).td1.str = LanguageConvertString(idLngFile, Items[i].name)+"\n"+Items[i].id;
			else GameInterface.(_tabName).(row).td1.str = LanguageConvertString(idLngFile, Items[i].name);
			GameInterface.(_tabName).(row).index = i;
			GameInterface.(_tabName).(row).td1.scale = 0.9;
			switch (type)
			{
				case "blade":
				if (CheckAttribute(Items[i],"Generation")) GameInterface.(_tabName).(row).td2.str = makeint(Items[i].Generation.dmg_min.min)+"-"+makeint(Items[i].Generation.dmg_min.max)+" / "+makeint(Items[i].Generation.dmg_max.min)+"-"+makeint(Items[i].Generation.dmg_max.max);
				else GameInterface.(_tabName).(row).td2.str = makeint(Items[i].dmg_min)+"/"+makeint(Items[i].dmg_max);
				GameInterface.(_tabName).(row).td3.str = XI_ConvertString(Items[i].FencingType);
				if (CheckAttribute(Items[i],"Generation")) GameInterface.(_tabName).(row).td4.str = FloatToString(stf(Items[i].Generation.Weight.min), 1)+" - "+FloatToString(stf(Items[i].Generation.Weight.max), 1);
				else GameInterface.(_tabName).(row).td4.str = FloatToString(stf(Items[i].Weight), 1);
				GameInterface.(_tabName).(row).td5.str = GetSpecialStrings(Items[i]);
				break;
				case "gun":
				if (HasSubStr(Items[i].id,"mushket")) GameInterface.(_tabName).(row).td2.str = GetGunDamage(Items[i])+"\n"+GetConvertStrWithReplace("Variable_QuestBook_148", "Interface.txt", "#space#", " ")+sti(Items[i].melee_dmg_min)+"/"+sti(Items[i].melee_dmg_max);
				else GameInterface.(_tabName).(row).td2.str = GetGunDamage(Items[i]);
				GameInterface.(_tabName).(row).td3.scale = 0.9;
				GameInterface.(_tabName).(row).td3.str = GetAmmoTypes(Items[i])+"\n"+GetPerkInfo(Items[i]);
				GameInterface.(_tabName).(row).td4.str = GetAccuracy(Items[i]);
				GameInterface.(_tabName).(row).td5.str = GetRecharge(Items[i]);
				break;
				case "food":
				if (CheckAttribute(Items[i],"food.energy"))  GameInterface.(_tabName).(row).td2.str = makeint(Items[i].food.energy) + " EN";
				else GameInterface.(_tabName).(row).td2.str = "-------";
				if (CheckAttribute(Items[i],"potion.health"))  GameInterface.(_tabName).(row).td2.str = makeint(Items[i].potion.health) + " HP";
				if (CheckAttribute(Items[i],"potion.antidote")) GameInterface.(_tabName).(row).td3.str = GetConvertStrWithReplace("Variable_QuestBook_149", "Interface.txt", "#space#", " ");
				else GameInterface.(_tabName).(row).td3.str = "-------";
				GameInterface.(_tabName).(row).td4.str = FloatToString(stf(Items[i].Weight), 1);
				break;
				case "bonusitem":
				GameInterface.(_tabName).(row).td1.icon.width = 80;
				GameInterface.(_tabName).(row).td1.icon.height = 80;
				GameInterface.(_tabName).(row).td1.textoffset = "80,0";
				GameInterface.(_tabName).(row).td2.str = GetItemDescribe(Items[i].id);
				// LEO: масштаб задаём ЗДЕСЬ, а не в .ini: fontcellscale там лишь дефолт, атрибут scale
				//   у ячейки его перебивает. Описания выросли (бонус на строку), и длинные не влезали.
				GameInterface.(_tabName).(row).td2.scale = 0.6;
				break;
			}
			n++;
		}
	}
	LanguageCloseFile(idLngFile);
	Table_UpdateWindow(_tabName);
}

string GetPerkInfo(ref itm)
{
	if (CheckAttribute(itm,"ReqPerk"))
	{
		if (itm.ReqPerk == "Gunman") return "***\n" + GetConvertStrWithReplace("Variable_QuestBook_149.1", "Interface.txt", "#space#", " ");
		if (itm.ReqPerk == "GunProfessional") return "***\n" + GetConvertStrWithReplace("Variable_QuestBook_149.2", "Interface.txt", "#space#", " ");
	}
	else return "***\n" + GetConvertStrWithReplace("Variable_QuestBook_149.3", "Interface.txt", "#space#", " ");
	return "";
}

string GetSpecialStrings(ref itm)
{
	if (!CheckAttribute(itm,"special")) return "-------";
	string info = GetOtherBladeInfo(itm);
	return info;
}

void HideInfoWindowEncy()
{
	SetNodeUsing("SHIP_INFO_FADER",false);
	SetNodeUsing("SHIP_INFO_DUST",false);
	SetNodeUsing("SHIP_INFO_FRAME",false);
	SetNodeUsing("SHIP_INFO_FRAME_CAPTION",false);
	SetNodeUsing("SHIP_TABLE_LIST_LEFT",false);
	SetNodeUsing("SHIP_TABLE_LIST_RIGHT",false);
	SetNodeUsing("SHIP_TABLE_SCROLL_LEFT",false);
	SetNodeUsing("SHIP_TABLE_SCROLL_RIGHT",false);
	SetNodeUsing("SHIP_TABLE_OTHER_LEFT",false);
	SetNodeUsing("SHIP_TABLE_OTHER_RIGHT",false);
	SetNodeUsing("SHIP_INFO_FRAME_TEXT_LEFT",false);
	SetNodeUsing("SHIP_INFO_FRAME_TEXT_RIGHT",false);
	SetNodeUsing("SHIP_INFO_TEXT_LEFT",false);
	SetNodeUsing("SHIP_INFO_TEXT_RIGHT",false);
	SetNodeUsing("SHIP_CLASS_LEFT",false);
	SetNodeUsing("SHIP_NATION_LEFT",false);
	SetNodeUsing("SHIP_CLASS_RIGHT",false);
	SetNodeUsing("SHIP_NATION_RIGHT",false);
	SetNewPicture("S_NATION_E_L", "");
	SetNewPicture("S_NATION_F_L", "");
	SetNewPicture("S_NATION_H_L", "");
	SetNewPicture("S_NATION_S_L", "");
	SetNewPicture("S_NATION_P_L", "");
	SetNewPicture("S_NATION_E_R", "");
	SetNewPicture("S_NATION_F_R", "");
	SetNewPicture("S_NATION_H_R", "");
	SetNewPicture("S_NATION_S_R", "");
	SetNewPicture("S_NATION_P_R", "");
	SetNodeUsing("BLADE_INFO_FADER",false);
	SetNodeUsing("BLADE_INFO_DUST",false);
	SetNodeUsing("BLADE_INFO_FRAME",false);
	SetNodeUsing("BLADE_INFO_FRAME_CAPTION",false);
	SetNodeUsing("BLADE_TABLE_LIST_SCROLL",false);
	SetNodeUsing("BLADE_TABLE_LIST",false);
	SetNodeUsing("BLADE_TYPE_CLASS",false);
	SetNodeUsing("BLADE_TYPE_Quality",false);
	SetNodeUsing("GUN_INFO_FADER",false);
	SetNodeUsing("GUN_INFO_DUST",false);
	SetNodeUsing("GUN_INFO_FRAME",false);
	SetNodeUsing("GUN_INFO_FRAME_CAPTION",false);
	SetNodeUsing("GUN_TABLE_LIST_SCROLL",false);
	SetNodeUsing("GUN_TABLE_LIST",false);
	SetNodeUsing("GUN_TYPE_QUALITY",false);
	SetNodeUsing("FOOD_INFO_FADER",false);
	SetNodeUsing("FOOD_INFO_DUST",false);
	SetNodeUsing("FOOD_INFO_FRAME",false);
	SetNodeUsing("FOOD_INFO_FRAME_CAPTION",false);
	SetNodeUsing("FOOD_TABLE_LIST_SCROLL",false);
	SetNodeUsing("FOOD_TABLE_LIST",false);
	SetNodeUsing("BONUSITEM_INFO_FADER",false);
	SetNodeUsing("BONUSITEM_INFO_DUST",false);
	SetNodeUsing("BONUSITEM_INFO_FRAME",false);
	SetNodeUsing("BONUSITEM_INFO_FRAME_CAPTION",false);
	SetNodeUsing("BONUSITEM_TABLE_LIST_SCROLL",false);
	SetNodeUsing("BONUSITEM_TABLE_LIST",false);
	SetNodeUsing("BONUSITEM_CLASS",false);
	SetNodeUsing("QUESTS_INFO_FADER",false);
	SetNodeUsing("QUESTS_INFO_DUST",false);
	SetNodeUsing("QUESTS_INFO_FRAME",false);
	SetNodeUsing("QUESTS_INFO_FRAME_CAPTION",false);
	SetNodeUsing("QUESTS_TABLE_LIST_SCROLL",false);
	SetNodeUsing("QUESTS_TABLE_LIST",false);
	SetNodeUsing("QUESTS_CLASS",false);
	SetNodeUsing("QUESTS_INFO_FRAME_CAPTION",false);
	SetNodeUsing("ACHIEVEMENTS_INFO_FADER",false);
	SetNodeUsing("ACHIEVEMENTS_INFO_DUST",false);
	SetNodeUsing("ACHIEVEMENTS_INFO_FRAME",false);
	SetNodeUsing("TABLE_ACHIEVEMENTS",false);
	SetNodeUsing("ACH_COMPLETE_SUM",false);
	SetNodeUsing("SCROLL_ACHIEVEMENTS",false);
	SetNodeUsing("BAR_str1",false);
	SetNodeUsing("BAR_str2",false);
	SetNodeUsing("BAR_str3",false);
	SetNodeUsing("BAR_str4",false);
	SetNodeUsing("BAR_str5",false);
	SetNodeUsing("BAR_str6",false);
	SetNodeUsing("BAR_str7",false);

	SetNodeUsing("POINTS_EXCHANGE",false);
}
void ShowInfoWindowEncyShip()
{
	HideInfoWindowEncy();
	SetNodeUsing("SHIP_INFO_FADER",true);
	SetNodeUsing("SHIP_TABLE_LIST_LEFT",true);
	SetNodeUsing("SHIP_TABLE_LIST_RIGHT",true);
	SetNodeUsing("SHIP_TABLE_SCROLL_LEFT",true);
	SetNodeUsing("SHIP_TABLE_SCROLL_RIGHT",true);
	SetNodeUsing("SHIP_TABLE_OTHER_LEFT",true);
	SetNodeUsing("SHIP_TABLE_OTHER_RIGHT",true);
	SetNodeUsing("SHIP_INFO_DUST",true);
	SetNodeUsing("SHIP_INFO_FRAME",true);
	SetNodeUsing("SHIP_INFO_FRAME_CAPTION",true);
	SetNodeUsing("SHIP_INFO_FRAME_TEXT_LEFT",true);
	SetNodeUsing("SHIP_INFO_FRAME_TEXT_RIGHT",true);
	SetNodeUsing("SHIP_INFO_TEXT_LEFT",true);
	SetNodeUsing("SHIP_INFO_TEXT_RIGHT",true);
	SetNodeUsing("SHIP_CLASS_LEFT",true);
	SetNodeUsing("SHIP_NATION_LEFT",true);
	SetNodeUsing("SHIP_CLASS_RIGHT",true);
	SetNodeUsing("SHIP_NATION_RIGHT",true);

	SetFormatedText("SHIP_INFO_FRAME_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_150", "Interface.txt", "#space#", " "));
}

void ShowInfoWindowEncyBlade()
{
	HideInfoWindowEncy();
	SetNodeUsing("BLADE_INFO_FADER",true);
	SetNodeUsing("BLADE_INFO_DUST",true);
	SetNodeUsing("BLADE_INFO_FRAME",true);
	SetNodeUsing("BLADE_INFO_FRAME_CAPTION",true);
	SetNodeUsing("BLADE_TABLE_LIST_SCROLL",true);
	SetNodeUsing("BLADE_TABLE_LIST",true);
	SetNodeUsing("BLADE_TYPE_CLASS",true);
	SetNodeUsing("BLADE_TYPE_Quality",true);

	SetFormatedText("BLADE_INFO_FRAME_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_151", "Interface.txt", "#space#", " "));
}

void ShowInfoWindowEncyGun()
{
	HideInfoWindowEncy();
	SetNodeUsing("GUN_INFO_FADER",true);
	SetNodeUsing("GUN_INFO_DUST",true);
	SetNodeUsing("GUN_INFO_FRAME",true);
	SetNodeUsing("GUN_INFO_FRAME_CAPTION",true);
	SetNodeUsing("GUN_TABLE_LIST_SCROLL",true);
	SetNodeUsing("GUN_TABLE_LIST",true);
	SetNodeUsing("GUN_TYPE_QUALITY",true);

	SetFormatedText("GUN_INFO_FRAME_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_152", "Interface.txt", "#space#", " "));
}

void ShowInfoWindowEncyFood()
{
	HideInfoWindowEncy();
	SetNodeUsing("FOOD_INFO_FADER",true);
	SetNodeUsing("FOOD_INFO_DUST",true);
	SetNodeUsing("FOOD_INFO_FRAME",true);
	SetNodeUsing("FOOD_INFO_FRAME_CAPTION",true);
	SetNodeUsing("FOOD_TABLE_LIST_SCROLL",true);
	SetNodeUsing("FOOD_TABLE_LIST",true);

	SetFormatedText("FOOD_INFO_FRAME_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_153", "Interface.txt", "#space#", " "));
}

void ShowInfoWindowEncyBonusItem()
{
	HideInfoWindowEncy();
	SetNodeUsing("BONUSITEM_INFO_FADER",true);
	SetNodeUsing("BONUSITEM_INFO_DUST",true);
	SetNodeUsing("BONUSITEM_INFO_FRAME",true);
	SetNodeUsing("BONUSITEM_INFO_FRAME_CAPTION",true);
	SetNodeUsing("BONUSITEM_TABLE_LIST_SCROLL",true);
	SetNodeUsing("BONUSITEM_TABLE_LIST",true);
	SetNodeUsing("BONUSITEM_CLASS",true);

	SetFormatedText("BONUSITEM_INFO_FRAME_CAPTION", GetConvertStrWithReplace("Variable_QuestBook_154", "Interface.txt", "#space#", " "));
}

void ShowInfoWindowEncyQuests()
{
	HideInfoWindowEncy();
	SetNodeUsing("QUESTS_INFO_FADER",true);
	SetNodeUsing("QUESTS_INFO_DUST",true);
	SetNodeUsing("QUESTS_INFO_FRAME",true);
	SetNodeUsing("QUESTS_INFO_FRAME_CAPTION",true);
	SetNodeUsing("QUESTS_TABLE_LIST_SCROLL",true);
	SetNodeUsing("QUESTS_TABLE_LIST",true);
	SetNodeUsing("QUESTS_CLASS",true);

	SetFormatedText("QUESTS_INFO_FRAME_CAPTION", GetConvertStr("Variable_QuestBook_155", "Interface.txt"));
}

void ProcessFilter()
{
	string sControl = GetEventData();
	int iSelectedCB = GetEventData();
	int iNewState = GetEventData();
//========================================//
	if (sControl == "SHIP_CLASS_RIGHT")
	{
	FState_SHIP_R = 9 - iSelectedCB;
	FillShipInfoEncy("SHIP_TABLE_LIST_RIGHT");
	return;
	}
//========================================//
	if (sControl == "SHIP_NATION_RIGHT")
	{
	FState_SHIP_N_R = -1 + iSelectedCB;
	FillShipInfoEncy("SHIP_TABLE_LIST_RIGHT");
	return;
	}
//========================================//
	if (sControl == "SHIP_CLASS_LEFT")
	{
	FState_SHIP_L = 9 - iSelectedCB;
	FillShipInfoEncy("SHIP_TABLE_LIST_LEFT");
	return;
	}
//========================================//
	if (sControl == "SHIP_NATION_LEFT")
	{
	FState_SHIP_N_L = -1 + iSelectedCB;
	FillShipInfoEncy("SHIP_TABLE_LIST_LEFT");
	return;
	}
//========================================//
	if (sControl == "BLADE_TYPE_CLASS")
	{
	FState_BLADE = -1 + iSelectedCB;
	AddToTable("BLADE_TABLE_LIST", "blade");
	return;
	}
//========================================//
	if (sControl == "BLADE_TYPE_Quality")
	{
	FState_BLADEQ = -1 + iSelectedCB;
	AddToTable("BLADE_TABLE_LIST", "blade");
	return;
	}
//========================================//
	if (sControl == "GUN_TYPE_QUALITY")
	{
	FState_GunQ = -1 + iSelectedCB;
	AddToTable("GUN_TABLE_LIST", "gun");
	return;
	}
//========================================//
	if (sControl == "BONUSITEM_CLASS")
	{
	FState_Bonus = -1 + iSelectedCB;

	AddToTable("BONUSITEM_TABLE_LIST", "BonusItem");
	return;
	}

	if (sControl == "QUESTS_CLASS")
	{
		switch (iSelectedCB)
		{
			case 1:
				FState_COMPLETE = iNewState;
				NullCharacter.saveFState_COMPLETE = FState_COMPLETE;
			break;
			case 2:
				FState_ACTIVE = iNewState;
				NullCharacter.saveFState_ACTIVE = FState_ACTIVE;
			break;
			case 3:
				FState_AVAILABLE = iNewState;
				NullCharacter.saveFState_AVAILABLE = FState_AVAILABLE;
			break;
			case 4:
				FState_UNAVAILABLE = iNewState;
				NullCharacter.saveFState_UNAVAILABLE = FState_UNAVAILABLE;
			break;
		}
		FillQuests();
		return;
			}
//========================================//
}

void fill_bars()
{
	string sbarX, rowX, sAch_text;
	int j1, j2;
	int n_Top = 0;
	if (checkattribute(GameInterface,"TABLE_ACHIEVEMENTS.top")) n_Top = sti(GameInterface.TABLE_ACHIEVEMENTS.top);
	n_Top++;
	for (int i=0; i<8; i++)
	{
		rowX = "tr" + (n_Top + i);
		sbarX = "BAR_str" + (i+1);
		GameInterface.StatusLine.(sbarX).Min = 0;//А это нужно в инит вынести и один раз макс и мин устанавливать.
		GameInterface.StatusLine.(sbarX).Max = 100;//нужно в допинфе при заполнении таблицы достижений заполнять, наверное, всю эту фигню для удобства
		if (!CheckAttribute(GameInterface, "TABLE_ACHIEVEMENTS." + rowX + ".td4.str")) // строки нет (прокрутка за конец списка) -> пустой бар, без обращения к несуществующему узлу
		{
			GameInterface.StatusLine.(sbarX).Value = 0;
			SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sbarX, 0);
			continue;
		}
		sAch_text = GameInterface.TABLE_ACHIEVEMENTS.(rowX).td4.str;
		if (sAch_text == ("\n" + GetConvertStr("Variable_QuestBook_155","Interface.txt"))) {GameInterface.StatusLine.(sbarX).Value = 100; SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sbarX, 0); continue;}
		if (sAch_text == ("\n" + GetConvertStr("Variable_QuestBook_156","Interface.txt"))) {GameInterface.StatusLine.(sbarX).Value = 1; SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sbarX, 0); continue;}
		sAch_text = strreplace(sAch_text, " ", "");
		j2 = findsubstr(sAch_text, "/" , 0);
		if (j2 < 0) {GameInterface.StatusLine.(sbarX).Value = 100; SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sbarX, 0); continue;} // завершённое достижение без знаменателя "/" -> полный бар, без падения strcut
		GameInterface.StatusLine.(sbarX).Max = strcut(sAch_text, j2+1, strlen(sAch_text)-1);
		j1 = findsubstr(sAch_text, ":" , 0);
		GameInterface.StatusLine.(sbarX).Value = strcut(sAch_text, j1+3, j2-1);
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sbarX, 0);
	}
}

void Paint_ShipOTHER_Diff()
{
	float fAverage, fLeft, fRight;
	int k1, k2;
	string row, sTemp1, sTemp2;
	for (int i=1; i<=10; i++)
	{
		if (i == 2) continue; //паруса по 100 у всех пока что
		row = "tr" + i;
		if ( checkattribute(GameInterface, "SHIP_TABLE_OTHER_RIGHT."+row+".td2.str") && checkattribute(GameInterface, "SHIP_TABLE_OTHER_LEFT."+row+".td3.str") )
		{
			sTemp1 = GameInterface.SHIP_TABLE_OTHER_LEFT.(row).td3.str; 
			sTemp2 = GameInterface.SHIP_TABLE_OTHER_RIGHT.(row).td2.str;

			if (i == 8) 
			{
					k1 = findsubstr(sTemp1, "/" , 0);
					sTemp1 = strcut(sTemp1, k1+1, strlen(sTemp1)-1);
					k2 = findsubstr(sTemp2, "/" , 0);
					sTemp2 = strcut(sTemp2, k2+1, strlen(sTemp2)-1);
			}

			fAverage = ( stf(sTemp1) + stf(sTemp2) )/2.0;//среднее значение параметра
			if (fAverage < 0.0001) continue;//от деления на ноль
			fRight = stf(sTemp2)/fAverage - 1;
			fLeft = -fRight;
			if (fRight == 0)
			{
				GameInterface.SHIP_TABLE_OTHER_RIGHT.(row).td2.color = argb(255,255,255,255);
				GameInterface.SHIP_TABLE_OTHER_LEFT.(row).td3.color = argb(255,255,255,255);
				continue;
			}
			if (fRight > 0) {fRight	= Bring2Range(0.33, 1.0, 0.0, 1.0, fRight); fLeft = -fRight;} //задаём минимальную насыщенность цвета подкраски
				else 		{fLeft 	= Bring2Range(0.33, 1.0, 0.0, 1.0, fLeft); fRight = -fLeft;}
			if (i == 3) {fRight = -fRight; fLeft = -fLeft;}//мачты с обратной прочностью

			if (fRight > 0) //больше среднего справа
			{
				GameInterface.SHIP_TABLE_OTHER_RIGHT.(row).td2.color = argb(255,makeint(255*(1-fRight)),255,makeint(255*(1-fRight)));
			}
			else
			{
				GameInterface.SHIP_TABLE_OTHER_RIGHT.(row).td2.color = argb(255,255,makeint(255*(1+fRight)),makeint(255*(1+fRight)));
			}

			if (fLeft > 0) //больше среднего слева
			{
				GameInterface.SHIP_TABLE_OTHER_LEFT.(row).td3.color = argb(255,makeint(255*(1-fLeft)),255,makeint(255*(1-fLeft)));
			}
			else
			{
				GameInterface.SHIP_TABLE_OTHER_LEFT.(row).td3.color = argb(255,255,makeint(255*(1+fLeft)),makeint(255*(1+fLeft)));
			}
		}
		else
		{
			GameInterface.SHIP_TABLE_OTHER_RIGHT.(row).td2.color = argb(255,255,255,255);//фикс - красим белым, если одна из таблиц не заполнена
			GameInterface.SHIP_TABLE_OTHER_LEFT.(row).td3.color = argb(255,255,255,255);
		}
	}
}

void OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();

	if (sControl == "QUESTS_TABLE_LIST")
	{
		if (!SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sControl, 1 )) SortTable(sControl, iColumn);
		Table_UpdateWindow(sControl);
	}
}
