int iTypeCount = 0;
int g_pD, g_pM, g_pY; // Gregg: разобранная дата планировщика (день/месяц/год)
bool g_bSchedLoaded = false; // Gregg: планировщик уже подтянут в строки за эту сессию интерфейса

void InitInterface(string iniName)
{
    	GameInterface.title = "titleCreateNotice";
    	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetBaseInterfaceData();

	HideButtons("_DONE", false, false);
	CheckCreateNotice();
	SetNoticeTypeInfo();
	g_bSchedLoaded = false; // Gregg: при каждом открытии интерфейса планировщик грузим заново
	SchedulerUpdateMode(); // Gregg: показать/скрыть поля даты по текущему типу записи

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("IsCorrectNotice","IsCorrectNotice",0);
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_SALARY_EXIT );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_SALARY_EXIT );
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("IsCorrectNotice","IsCorrectNotice");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void IDoExitTwo(int exitCode, bool bCode)
{
	interfaceResultCommand = exitCode;
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("IsCorrectNotice","IsCorrectNotice");
	EndCancelInterface(bCode);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	if(nodName == "STRING1_RESET" || nodName == "STRING2_RESET" || nodName == "STRING3_RESET" || nodName == "STRING4_RESET"
	|| nodName == "STRING5_RESET" || nodName == "STRING6_RESET" || nodName == "STRING7_RESET" || nodName == "STRING8_RESET"
	|| nodName == "STRING9_RESET" || nodName == "STRING10_RESET")
	{
		if(comName=="activate" || comName=="click")
		{
			ResetString(nodName);
		}
	}

	if(nodName == "STRING1_DONE" || nodName == "STRING2_DONE" || nodName == "STRING3_DONE" || nodName == "STRING4_DONE"
	|| nodName == "STRING5_DONE" || nodName == "STRING6_DONE" || nodName == "STRING7_DONE" || nodName == "STRING8_DONE"
	|| nodName == "STRING9_DONE" || nodName == "STRING10_DONE"
	|| nodName == "STRING1_DONE_OFF" || nodName == "STRING2_DONE_OFF" || nodName == "STRING3_DONE_OFF" || nodName == "STRING4_DONE_OFF"
	|| nodName == "STRING5_DONE_OFF" || nodName == "STRING6_DONE_OFF" || nodName == "STRING7_DONE_OFF" || nodName == "STRING8_DONE_OFF"
 	|| nodName == "STRING9_DONE_OFF"|| nodName == "STRING10_DONE_OFF")
	{
		if(comName=="activate" || comName=="click")
		{
			ChangeOffOrOn(nodName);
		}
	}

	switch(nodName)
	{
		case "EXIT":
			if(comName=="activate" || comName=="click")
			{
				PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_SALARY_EXIT);
			}
		break;

		case "DONE":
			if(comName=="activate" || comName=="click")
			{
				CreateNotice();
			}
		break;

		case "RESET":
			if(comName=="activate" || comName=="click")
			{
				SetBaseInterfaceData();
			}
		break;

		case "ALL_DONE":
			if(comName=="activate" || comName=="click")
			{
				OffOnStrings(true);
			}
		break;

		case "ALL_RESET":
			if(comName=="activate" || comName=="click")
			{
				OffOnStrings(false);
			}
		break;

		case "DELETE_ALL_NOTICES":
			if(comName=="activate" || comName=="click")
			{
				if(iTypeCount == 2) // Gregg: режим планировщика - чистим планировщик
				{
					DeleteAttribute(pchar, "Scheduler");
					DeleteQuestHeader("MyScheduler");
					SchedulerLoadToRows();
				}
				else
				{
					DeleteQuestHeader("MyNotices");
				}
			}
		break;

		case "JOURNAL_BUTTON":
			if(comName=="click")
			{
				IDoExitTwo(RC_INTERFACE_TO_LOGBOOK, true);
				return;
			}
		break;

		case "NOTICE_TYPE_LEFT":
			if(comName=="activate" || comName=="click")
			{
				SelectNoticeType(true);
			}
		break;

		case "NOTICE_TYPE_RIGHT":
			if(comName=="activate" || comName=="click")
			{
				SelectNoticeType(false);
			}
		break;
	}
}

void AddQuestRecordInterface(string idQuest,string idReferenceQuest,string idText, string sText)
{
	if(CheckAttribute(pchar,"QuestInfo."+idQuest)==false)
	{
		SetQuestHeaderEx(idQuest,idReferenceQuest);
	}

	string sTextLbl = "l0";
	if( CheckAttribute(pchar,"QuestInfo."+idQuest+".Text") )
	{
		aref arTmp; makearef( arTmp, pchar.QuestInfo.(idQuest) );
		makearef(arTmp,pchar.QuestInfo.(idQuest).Text);
		sTextLbl = "l" + GetAttributesNum(arTmp);
	}
	if( sTextLbl != "" )
	{
		pchar.QuestInfo.(idQuest).Text.(sTextLbl) = "@"+idReferenceQuest+"@"+GetQuestBookData() + NewStr();
		pchar.QuestInfo.(idQuest).Text.(sTextLbl) = pchar.QuestInfo.(idQuest).Text.(sTextLbl) + sText;
		Log_SetStringToLog(XI_ConvertString("Logbook Update"));
		AddMsgToCharacter(pchar,MSGICON_LOGBOOK);
		PlayStereoSound("notebook");
	}

	SetQuestHeaderColor(idQuest, argb(255,255,128,255));
}

int IsCorrectNotice()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	int n,nLen;
	string str, sName;

	sName = GameInterface.(nodName).str;
	string sFullString;

	nLen = strlen(sName);
	for( n=0; n<nLen; n++ )
	{
		str = GetSymbol(&sName,n);
		sFullString += str;

		if(sFullString==GetConvertStrWithReplace("Variable_CreateNotice_1", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_2", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_3", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_4", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_5", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_6", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_7", "Interface.txt", "#space#", " ") || str==GetConvertStrWithReplace("Variable_CreateNotice_8", "Interface.txt", "#space#", " ")
		|| sFullString==GetConvertStrWithReplace("Variable_CreateNotice_9", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_10", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_11", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_12", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_13", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_14", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_15", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_16", "Interface.txt", "#space#", " ")
		|| sFullString==GetConvertStrWithReplace("Variable_CreateNotice_17", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_18", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_19", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_20", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_21", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_22", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_23", "Interface.txt", "#space#", " ") || str==GetConvertStrWithReplace("Variable_CreateNotice_24", "Interface.txt", "#space#", " ")
		|| sFullString==GetConvertStrWithReplace("Variable_CreateNotice_25", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_26", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_27", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_28", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_29", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_30", "Interface.txt", "#space#", " ") || str==GetConvertStrWithReplace("Variable_CreateNotice_31", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_32", "Interface.txt", "#space#", " ")
		|| sFullString==GetConvertStrWithReplace("Variable_CreateNotice_33", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_34", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_35", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_36", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_37", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_38", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_39", "Interface.txt", "#space#", " ") || sFullString==GetConvertStrWithReplace("Variable_CreateNotice_40", "Interface.txt", "#space#", " "))
		{
			GameInterface.(nodName).str = GetConvertStrWithReplace("Variable_CreateNotice_41", "Interface.txt", "#space#", " ");
			return -1;
		}
	}

	return -1;
}

void SetBaseInterfaceData()
{
	CreateString(true,"StringsHeader", GetConvertStrWithReplace("Variable_CreateNotice_42", "Interface.txt", "#space#", " "), FONT_CAPTION, argb(255,255,255,156), 335,40,SCRIPT_ALIGN_CENTER,1.5);
	CreateString(true,"Sbros_Done", "Вкл. / Выкл.", FONT_CAPTION, argb(255,255,255,156), 635,44,SCRIPT_ALIGN_CENTER,1.2);

	string sNode, sDateNode;
	for(int s=1; s <= 10; s++)
	{
		sNode = "NOTICE_WINDOW_" + s;
		GameInterface.(sNode).str = GetConvertStrWithReplace("Variable_CreateNotice_43", "Interface.txt", "#space#", " ");
		sDateNode = "NOTICE_DATE_" + s; // Gregg: поля даты планировщика по умолчанию = текущая дата
		GameInterface.(sDateNode).str = Sch_CurDateStr();
	}

	SetNewGroupPicture("CREATE_NOTICE_PICTURE", "CREATE_NOTICE_PAPER", "paper");
}

void SetNoticeTypeInfo()
{
	string sType = "";

	if(iTypeCount == 0)
	{
		sType = GetConvertStrWithReplace("Variable_CreateNotice_44", "Interface.txt", "#space#", " ");
	}
	else
	{
		if(iTypeCount == 1)
		{
			sType = GetConvertStrWithReplace("Variable_CreateNotice_45", "Interface.txt", "#space#", " ");
		}
		else
		{
			sType = GetConvertStrWithReplace("Variable_CreateNotice_48", "Interface.txt", "#space#", " "); // Gregg: Планировщик
		}
	}

	CreateString(true,"NoticeTypeStr", sType, FONT_CAPTION, argb(255,255,255,255), 350,520,SCRIPT_ALIGN_CENTER,1.0); // Gregg: селектор типа перенесён в центр-низ (в узкой правой панели длинное "Планировщик" не влезало между стрелками)
}

void OffOnStrings(bool bOn)
{
	string sButton;

	if(bOn)
	{
		sButton = "_DONE";
		HideButtons(sButton, true, false);

		sButton = "_DONE_OFF";
		HideButtons(sButton, false, false);
	}
	else
	{
		sButton = "_DONE_OFF";
		HideButtons(sButton, true, false);

		sButton = "_DONE";
		HideButtons(sButton, false, false);
	}

	CheckCreateNotice();
}

void HideButtons(string sButton, bool bOpen, bool bButton)
{
	string sNode;

	if(bButton)
	{
		SetNodeUsing(sButton, bOpen);
	}
	else
	{
		for(int b=1; b<=10; b++)
		{
			sNode = "STRING" + b;
			sNode += sButton;

			SetNodeUsing(sNode, bOpen);
			SetSelectable(sNode, bOpen);
		}
	}
}

void ResetString(string sNode)
{
	if(sNode == "STRING1_RESET") 		{ sNode = "NOTICE_WINDOW_1"; }
	if(sNode == "STRING2_RESET") 		{ sNode = "NOTICE_WINDOW_2"; }
	if(sNode == "STRING3_RESET") 		{ sNode = "NOTICE_WINDOW_3"; }
	if(sNode == "STRING4_RESET") 		{ sNode = "NOTICE_WINDOW_4"; }
	if(sNode == "STRING5_RESET") 		{ sNode = "NOTICE_WINDOW_5"; }
	if(sNode == "STRING6_RESET") 		{ sNode = "NOTICE_WINDOW_6"; }
	if(sNode == "STRING7_RESET") 		{ sNode = "NOTICE_WINDOW_7"; }
	if(sNode == "STRING8_RESET") 		{ sNode = "NOTICE_WINDOW_8"; }
	if(sNode == "STRING9_RESET") 		{ sNode = "NOTICE_WINDOW_9"; }
	if(sNode == "STRING10_RESET") 		{ sNode = "NOTICE_WINDOW_10"; }

	GameInterface.(sNode).str = GetConvertStrWithReplace("Variable_CreateNotice_46", "Interface.txt", "#space#", " ");
}

void ChangeOffOrOn(string sNode)
{
	string sResultNode;

	if(sNode == "STRING1_DONE" || sNode == "STRING2_DONE" || sNode == "STRING3_DONE" || sNode == "STRING4_DONE"
	|| sNode == "STRING5_DONE" || sNode == "STRING6_DONE" || sNode == "STRING7_DONE" || sNode == "STRING8_DONE"
	|| sNode == "STRING9_DONE" || sNode == "STRING10_DONE")
	{
		SetNodeUsing(sNode, false);
		SetSelectable(sNode, false);

		sResultNode = sNode + "_OFF";
		SetNodeUsing(sResultNode, true);
		SetSelectable(sResultNode, true);
	}
	else
	{
		if(sNode == "STRING1_DONE_OFF" || sNode == "STRING2_DONE_OFF" || sNode == "STRING3_DONE_OFF" || sNode == "STRING4_DONE_OFF"
		|| sNode == "STRING5_DONE_OFF" || sNode == "STRING6_DONE_OFF" || sNode == "STRING7_DONE_OFF" || sNode == "STRING8_DONE_OFF"
		|| sNode == "STRING9_DONE_OFF" || sNode == "STRING10_DONE_OFF")
		{
			SetNodeUsing(sNode, false);
			SetSelectable(sNode, false);

			switch(sNode)
			{
				case "STRING1_DONE_OFF": sResultNode = "STRING1_DONE"; break;
				case "STRING2_DONE_OFF": sResultNode = "STRING2_DONE"; break;
				case "STRING3_DONE_OFF": sResultNode = "STRING3_DONE"; break;
				case "STRING4_DONE_OFF": sResultNode = "STRING4_DONE"; break;
				case "STRING5_DONE_OFF": sResultNode = "STRING5_DONE"; break;
				case "STRING6_DONE_OFF": sResultNode = "STRING6_DONE"; break;
				case "STRING7_DONE_OFF": sResultNode = "STRING7_DONE"; break;
				case "STRING8_DONE_OFF": sResultNode = "STRING8_DONE"; break;
				case "STRING9_DONE_OFF": sResultNode = "STRING9_DONE"; break;
				case "STRING10_DONE_OFF": sResultNode = "STRING10_DONE"; break;
			}

			SetNodeUsing(sResultNode, true);
			SetSelectable(sResultNode, true);
		}
	}

	CheckCreateNotice();
}

void CreateNotice()
{
	if(iTypeCount == 2) // Gregg: режим планировщика - сохраняем записи-будильники
	{
		SchedulerSave();
		SchedulerLoadToRows();
		return;
	}

	string sText, sNode, sButtonNode, sNodeText, sType;

	sText = "";

	if(iTypeCount == 1)
	{
		sType = "2";
	}
	else
	{
		sType = "1";
	}

	AddQuestRecordEx("MyNotices", "MyNotices", sType);

	for(int i=1; i<=10; i++)
	{
		sNode = "NOTICE_WINDOW_" + i;
		sButtonNode = "STRING" + i + "_DONE";

		if(GetSelectable(sButtonNode) == 0)
		{
			continue;
		}

		sNodeText = GameInterface.(sNode).str;

		if(sNodeText != "") // if(sNodeText != GetConvertStrWithReplace("Variable_CreateNotice_47", "Interface.txt", "#space#", " ") && sNodeText != "")
		{
			AddQuestUserData("MyNotices", "sTextNum" + i, sNodeText);
		}
	}

	//AddQuestRecordInterface("MyNotices", "MyNotices", "1", sText);

	HideButtons("_DONE", false, false);
	HideButtons("_DONE_OFF", true, false);
	CheckCreateNotice();

	for(int s=1; s<=10; s++)
	{
		ResetString("STRING" + s + "_RESET");
	}
}

void CheckCreateNotice()
{
	if(iTypeCount == 2) // Gregg: планировщик - всегда даём сохранить (в т.ч. чтобы выключить пункты)
	{
		SetSelectable("DONE", true);
		return;
	}

	int iCount = 0;
	string sButtonNode;

	for(int i=1; i <= 10; i++)
	{
		sButtonNode = "STRING" + i + "_DONE";

		if(GetSelectable(sButtonNode) == 0)
		{
			iCount++;
			continue;
		}

	}

	if(iCount == 10)
	{
		SetSelectable("DONE", false);
	}
	else
	{
		SetSelectable("DONE", true);
	}
}

void SelectNoticeType(bool bLeft)
{
	if(bLeft)
	{
		iTypeCount -= 1;
	}
	else
	{
		iTypeCount += 1;
	}

	if(iTypeCount > 2) // Gregg: 0=Строка, 1=Столбец, 2=Планировщик
	{
		iTypeCount = 0;
	}

	if(iTypeCount < 0)
	{
		iTypeCount = 2;
	}

	SetNoticeTypeInfo();
	SchedulerUpdateMode(); // Gregg: переключить видимость полей даты / загрузить планировщик
}

//============================ Gregg: ПЛАНИРОВЩИК ============================

string Sch_Pad2(int v) // двузначное число с ведущим нулём
{
	if(v < 10) return "0" + v;
	return "" + v;
}

string Sch_DateStr(int d, int m, int y) // "ДД.ММ.ГГГГ"
{
	return Sch_Pad2(d) + "." + Sch_Pad2(m) + "." + y;
}

string Sch_CurDateStr() // текущая игровая дата как "ДД.ММ.ГГГГ"
{
	return Sch_DateStr(GetDataDay(), GetDataMonth(), GetDataYear());
}

// Разбор строки "ДД.ММ.ГГГГ" -> g_pD/g_pM/g_pY. true если корректно.
bool Sch_ParseDate(string sDate)
{
	int n = strlen(sDate);
	string a = "";
	string b = "";
	string c = "";
	int part = 0;
	string ch;
	for(int i=0; i<n; i++)
	{
		ch = GetSymbol(&sDate, i);
		if(ch == ".")
		{
			part += 1;
			if(part > 2) return false;
		}
		else
		{
			if(part == 0) a += ch;
			if(part == 1) b += ch;
			if(part == 2) c += ch;
		}
	}
	if(a == "") return false;
	if(b == "") return false;
	if(c == "") return false;
	g_pD = sti(a);
	g_pM = sti(b);
	g_pY = sti(c);
	if(g_pD < 1)  return false;
	if(g_pD > 31) return false;
	if(g_pM < 1)  return false;
	if(g_pM > 12) return false;
	if(g_pY < 1)  return false;
	return true;
}

// Показать/скрыть поля даты по текущему типу записи; в планировщике подтянуть сохранённые записи
void SchedulerUpdateMode()
{
	bool bPlanner = (iTypeCount == 2);
	string sDate;
	for(int i=1; i<=10; i++)
	{
		sDate = "NOTICE_DATE_" + i;
		SetNodeUsing(sDate, bPlanner);
		SetSelectable(sDate, bPlanner);
	}

	if(bPlanner)
	{
		CreateString(true, "SchedulerDateHdr", GetConvertStrWithReplace("Variable_CreateNotice_49", "Interface.txt", "#space#", " "), FONT_CAPTION, argb(255,255,255,156), 155, 108, SCRIPT_ALIGN_CENTER, 0.8);
		if(!g_bSchedLoaded) // Gregg: грузим записи в строки лишь раз за сессию, чтобы не затирать несохранённый ввод при переключении типа
		{
			SchedulerLoadToRows();
			g_bSchedLoaded = true;
		}
	}
	else
	{
		CreateString(true, "SchedulerDateHdr", "", FONT_CAPTION, argb(255,255,255,156), 155, 108, SCRIPT_ALIGN_CENTER, 0.8);
	}
}

// Загрузка сохранённых записей планировщика в строки интерфейса
void SchedulerLoadToRows()
{
	string sI, sWin, sDate, sDone, sOff;
	int d, m, y;
	bool act;
	for(int i=1; i<=10; i++)
	{
		sI   = "i" + i;
		sWin = "NOTICE_WINDOW_" + i;
		sDate= "NOTICE_DATE_" + i;
		sDone= "STRING" + i + "_DONE";
		sOff = "STRING" + i + "_DONE_OFF";

		if(CheckAttribute(pchar, "Scheduler." + sI + ".text"))
		{
			GameInterface.(sWin).str = pchar.Scheduler.(sI).text;
			d = sti(pchar.Scheduler.(sI).day);
			m = sti(pchar.Scheduler.(sI).month);
			y = sti(pchar.Scheduler.(sI).year);
			GameInterface.(sDate).str = Sch_DateStr(d, m, y);
			act = (sti(pchar.Scheduler.(sI).active) == 1);
			SetNodeUsing(sDone, act);
			SetSelectable(sDone, act);
			SetNodeUsing(sOff, !act);
			SetSelectable(sOff, !act);
		}
		else
		{
			GameInterface.(sWin).str = GetConvertStrWithReplace("Variable_CreateNotice_43", "Interface.txt", "#space#", " ");
			GameInterface.(sDate).str = Sch_CurDateStr();
			SetNodeUsing(sDone, false);
			SetSelectable(sDone, false);
			SetNodeUsing(sOff, true);
			SetSelectable(sOff, true);
		}
	}
	CheckCreateNotice();
}

// Сохранение строк интерфейса в pchar.Scheduler + обновление записи в судовом журнале
void SchedulerSave()
{
	string sI, sWin, sDate, sDone, sText, sDateStr, sEmpty;
	bool act;
	sEmpty = GetConvertStrWithReplace("Variable_CreateNotice_43", "Interface.txt", "#space#", " ");

	DeleteAttribute(pchar, "Scheduler");
	pchar.Scheduler = "";

	for(int i=1; i<=10; i++)
	{
		sWin = "NOTICE_WINDOW_" + i;
		sDate= "NOTICE_DATE_" + i;
		sDone= "STRING" + i + "_DONE";
		sI   = "i" + i;

		sText = GameInterface.(sWin).str;
		if(sText == "")     continue;
		if(sText == sEmpty) continue;

		sDateStr = GameInterface.(sDate).str;
		if(!Sch_ParseDate(sDateStr)) continue; // некорректная дата - пропускаем строку

		act = (GetSelectable(sDone) == 1);

		pchar.Scheduler.(sI).text  = sText;
		pchar.Scheduler.(sI).day   = g_pD;
		pchar.Scheduler.(sI).month = g_pM;
		pchar.Scheduler.(sI).year  = g_pY;
		if(act) pchar.Scheduler.(sI).active = 1;
		else    pchar.Scheduler.(sI).active = 0;
	}

	SchedulerRefreshJournal();
	PlayStereoSound("notebook");
}

// Перестроить запись MyScheduler в судовом журнале из pchar.Scheduler (только активные пункты)
void SchedulerRefreshJournal()
{
	string sI, sLine, sVal;
	int i, d, m, y, cnt;

	DeleteQuestHeader("MyScheduler");

	cnt = 0;
	for(i=1; i<=10; i++)
	{
		sI = "i" + i;
		if(CheckAttribute(pchar, "Scheduler." + sI + ".text"))
		{
			if(sti(pchar.Scheduler.(sI).active) == 1) cnt += 1;
		}
	}
	if(cnt == 0) return;

	AddQuestRecordEx("MyScheduler", "MyScheduler", "1");

	for(i=1; i<=10; i++)
	{
		sI   = "i" + i;
		sLine= "sLine" + i;
		sVal = "";
		if(CheckAttribute(pchar, "Scheduler." + sI + ".text"))
		{
			if(sti(pchar.Scheduler.(sI).active) == 1)
			{
				d = sti(pchar.Scheduler.(sI).day);
				m = sti(pchar.Scheduler.(sI).month);
				y = sti(pchar.Scheduler.(sI).year);
				sVal = "[" + Sch_DateStr(d, m, y) + "] " + pchar.Scheduler.(sI).text;
			}
		}
		AddQuestUserData("MyScheduler", sLine, sVal);
	}
}
