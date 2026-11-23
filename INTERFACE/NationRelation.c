/// BOAL меню наций
int curNationIdx;

void InitInterface(string iniName)
{
    InterfaceStack.SelectMenu_node = "LaunchNationRelation"; // запоминаем, что звать по Ф2
	GameInterface.title = "titleNationRelation";
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	// boal 18.12.2003 -->
	// делаем заточку под 5 или 6 наций и любую морду (64) гл. героя - универсальность залог успеха!
	// введем коэфф. для автопоcтроения матрицы
	int     i;
	int     dy=10;
    int     dx=16;
    int     sx=45;
    int     sy=120;
	int     sz=56;
	string  picName  = "pic";
	string  groupName;
	int     picCount = 1;
	int     deltaX = 170;
	int     deltaY = 240;
	int     max_nt = MAX_NATIONS;

	ref chref = GetMainCharacter();
    int j;

	if (bBettaTestMode)
	{   // со столбцом ГГ
		SetNewPicture("ICON_1", "interfaces\PORTRAITS\face_" + chref.faceId+ ".tga");
		SetNewPicture("ICON_2", "interfaces\PORTRAITS\face_" + chref.faceId+ ".tga");
		for (i=0; i< max_nt; i++)
		{
	        groupName = "NATIONS";
	        CreateImage(picName+picCount, groupName, Nations[i].Name, deltaX +dx+(dx+sz)*i, sy, deltaX + dx+ (dx+sz)*i+sz, sy+sz);
		    picCount++;
		}
		for (i=0; i< max_nt; i++)
		{
		    groupName = "NATIONS";
	        CreateImage(picName+picCount, groupName, Nations[i].Name,  sx, deltaY-10 +dy+(dy+sz)*i, sx+sz, deltaY-10 +dy+(dy+sz)*i+sz);
		    picCount++;
		}
		// Y
		for (i=0; i< max_nt; i++)
		{
	        CreateImage(picName+picCount, "relations", GetRelationName(GetNationRelation2MainCharacter(i)), deltaX -sz, deltaY +(dy+sz)*i, deltaX , deltaY +(dy+sz)*i+sz);
		    picCount++;
		}
		//X
		for (i=0; i< max_nt; i++)
		{
	        CreateImage(picName+picCount, "relations", GetRelationName(GetNationRelation2MainCharacter(i)), deltaX +dx+(dx+sz)*i, deltaY-sz-4, deltaX + dx+ (dx+sz)*i+sz, deltaY-4);
		    picCount++;
		}
		//GetNationRelation
		for (j=0; j< max_nt; j++)
		{
		    for (i=0; i< max_nt; i++)
		    {
		        if (i != j)
		        {
		            CreateImage(picName+picCount, "relations", GetRelationName(GetNationRelation(j, i)), deltaX +dx+(dx+sz)*i, deltaY +(dy+sz)*(j), deltaX + dx+ (dx+sz)*i+sz, deltaY+(dy+sz)*(j)+sz);
		            picCount++;
		        }
		    }
		}
	}
	else
	{// без столбца ГГ
     	SetNewPicture("ICON_1", "");
		SetNewPicture("ICON_2", "");
		sx=75;
    	sy=120;
		sz = 60;

		deltaX = 140;
		deltaY = 200;
		for (i=0; i< max_nt; i++)
		{
	        groupName = "NATIONS";
	        CreateImage(picName+picCount, groupName, Nations[i].Name, deltaX +dx+(dx+sz)*i, sy, deltaX + dx+ (dx+sz)*i+sz, sy+sz);
		    picCount++;
		}
		for (i=0; i< max_nt; i++)
		{
		    groupName = "NATIONS";
	        CreateImage(picName+picCount, groupName, Nations[i].Name,  sx, deltaY-10 +dy+(dy+sz)*i, sx+sz, deltaY-10 +dy+(dy+sz)*i+sz);
		    picCount++;
		}
  		for (j=0; j< max_nt; j++)
		{
		    for (i=0; i< max_nt; i++)
		    {
		        if (i != j)
		        {
		            CreateImage(picName+picCount, "relations", GetRelationName(GetNationRelation(j, i)), deltaX +dx+(dx+sz)*i, deltaY +(dy+sz)*(j), deltaX + dx+ (dx+sz)*i+sz, deltaY+(dy+sz)*(j)+sz);
		            picCount++;
		        }
		    }
		}
	}
    // boal 18.12.2003 <--
	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
    SetEventHandler("FlagsPress","FlagsProcess",0);
    SetEventHandler("ievnt_command","ProcessCommandExecute",0);
    SetEventHandler("MouseRClickUP","HideInfo",0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("CheckButtonChange","procCheckBoxChange",0);

    /////////////
    CreateString(true,"titul", XI_ConvertString("Hunter_type"),"DIALOG2",COLOR_NORMAL,589,100,SCRIPT_ALIGN_LEFT,1.0);

    // LEO: имя нации и строка отношения выровнены ВЛЕВО от общей вертикали 628 - на 6 пикселей
    //   правее начала полос (те идут с 622, NationRelation.ini). Раньше обе строки центрировались
    //   по 670, и колонка «плавала»: у длинных названий начало текста уезжало влево от полосы.
    CreateString(true,"EngH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,628,136,SCRIPT_ALIGN_LEFT,1.0);
    CreateString(true,"FraH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,628,178,SCRIPT_ALIGN_LEFT,1.0);
    CreateString(true,"SpaH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,628,220,SCRIPT_ALIGN_LEFT,1.0);
    CreateString(true,"HolH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,628,262,SCRIPT_ALIGN_LEFT,1.0);
    CreateString(true,"PirH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,628,304,SCRIPT_ALIGN_LEFT,1.0);
    CreateString(true,"SmugH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,628,346,SCRIPT_ALIGN_LEFT,1.0);
    // LEO: числа отдельной колонкой, прижатой ВПРАВО к концу полос (722) - как «35 / 50» у рейтинга
    //   ГВИК ниже. Одной строкой с подписью они не помещались: «Награда за голову 1000000» обрезалась
    //   ровно на сумме, а сумма там - самое важное. Правый край 736, а не 722 (конец полос): на строку
    //   отведено всего ~108 пикселей, и на длинной подписи вроде «Нейтральная» число налезало на буквы.
    CreateString(true,"EngV","","INTERFACE_ULTRASMALL",COLOR_NORMAL,736,151,SCRIPT_ALIGN_RIGHT,1.0);
    CreateString(true,"FraV","","INTERFACE_ULTRASMALL",COLOR_NORMAL,736,193,SCRIPT_ALIGN_RIGHT,1.0);
    CreateString(true,"SpaV","","INTERFACE_ULTRASMALL",COLOR_NORMAL,736,235,SCRIPT_ALIGN_RIGHT,1.0);
    CreateString(true,"HolV","","INTERFACE_ULTRASMALL",COLOR_NORMAL,736,277,SCRIPT_ALIGN_RIGHT,1.0);
    CreateString(true,"PirV","","INTERFACE_ULTRASMALL",COLOR_NORMAL,736,319,SCRIPT_ALIGN_RIGHT,1.0);
    CreateString(true,"SmugV","","INTERFACE_ULTRASMALL",COLOR_NORMAL,736,361,SCRIPT_ALIGN_RIGHT,1.0);
    // Gregg: блок рейтинга ГВИК - НЕ по образцу строки нации: заголовок золотом, ранг доверия слева,
    // числовой прогресс до следующего ранга справа, широкая полоса под ними (нода BAR_GWIK).
    // Числа обязательны: шкала накопительная с порогами 50/150/350/700, без них не видно, далеко ли до следующего ранга
    CreateString(true,"GwikH","","INTERFACE_ULTRASMALL",COLOR_MONEY,570,390,SCRIPT_ALIGN_LEFT,1.0);
    CreateString(true,"GwikRep","","INTERFACE_ULTRASMALL",COLOR_NORMAL,570,407,SCRIPT_ALIGN_LEFT,1.0);
    CreateString(true,"GwikVal","","INTERFACE_ULTRASMALL",COLOR_NORMAL,730,407,SCRIPT_ALIGN_RIGHT,1.0);

    CalculateHunter();

    CreateString(true,"Flag1",GetConvertStrWithReplace("Variable_NationRelation_1", "Interface.txt", "#space#", " "),"DIALOG2",COLOR_NORMAL,650,458,SCRIPT_ALIGN_CENTER,1.0);
    curNationIdx = sti(chref.nation);
    SetNewNation(0);
    XI_RegistryExitKey("IExit_F2");
	if (CheckOfficersPerk(pchar,"SeaDogProfessional"))
	{
		SetNodeUsing("SeaDogProfessional_Switch",true);
		if (!CheckAttribute(pchar,"SeaDogProfessionalSwitch")) pchar.SeaDogProfessionalSwitch = 1;
		if (pchar.SeaDogProfessionalSwitch == 1) SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SeaDogProfessional_Switch", 2, 1, true );
		else SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SeaDogProfessional_Switch", 2, 1, false );
	}
	else SetNodeUsing("SeaDogProfessional_Switch",false);
}


void procCheckBoxChange()
{
	string sNodName = GetEventData();
	int nBtnIndex = GetEventData();
	int bBtnState = GetEventData();

	if( sNodName == "SeaDogProfessional_Switch" )
	{
		{
			pchar.SeaDogProfessionalSwitch = bBtnState;
		}
	}
}
// LEO: у контрабандистов СВОЯ шкала (questTemp.Contraband.relation, 0 = хуже некуда, 99 = лучше всех),
//   а подпись брали общей GetNationReputationName под нацийские -100..100: отсюда «Нейтральная» при
//   нуле и «Хорошая» сразу после +25. Пороги - по местам, где отношение решает (Smuggler_Agent_dialog.c):
//   выше 5 контрабандисты вообще ведут дела, от 70 открываются лучшие предложения.
string NR_SmugglerRelationName(int i)
{
	if (LanguageGetLanguage() != "russian")
	{
		if (i <= 5)  return "Hostile";
		if (i <= 30) return "Neutral";
		if (i <= 69) return "Good";
		if (i <= 89) return "Excellent";
		return "Admiration";
	}
	if (i <= 5)  return "Враждебная";
	if (i <= 30) return "Нейтральная";
	if (i <= 69) return "Хорошая";
	if (i <= 89) return "Отличная";
	return "Восхищение";
}

// LEO: подпись нации БЕЗ суммы награды - сумма уехала в правую колонку чисел. Ступени берём общей
//   функцией, чтобы формулировки не разъезжались, и подменяем только ветку НЗГ. Там же подпись
//   укорочена до «За голову»: полное «Награда за голову» рядом с семизначной суммой не помещается,
//   а вместе строка читается как «За голову 1.000.000».
string NR_NationRelationName(int i)
{
	if (i > -10) return GetNationReputationName(0, i);
	if (LanguageGetLanguage() != "russian") return "Bounty";
	return "За голову";
}

// LEO: правая колонка. При награде за голову там сумма (позиция на шкале и так видна по полосе),
//   в остальных случаях - текущее значение и потолок шкалы. Сумму даём голым числом с разделителем
//   разрядов: FindRussianMoneyString дописывает «пиастров», и строка переставала помещаться, а слово
//   там ничего не добавляет - рядом стоит «За голову».
string NR_ValueText(int iValue, int iMax)
{
	int iBounty;
	if (iValue <= -10)
	{
		iBounty = -iValue * 10000;
		return MakeMoneyShow(iBounty, MONEY_SIGN, MONEY_DELIVER);
	}
	return iValue + "/" + iMax;
}
// LEO: градиент по положению на шкале: красный - жёлтый - зелёный. Считаем ОДИН раз здесь, а забирают
//   результат оба потребителя: NR_ColorTag (тег для FORMATEDTEXT) и NR_GradColor (argb для CreateString).
//   Компоненты кладём в GameInterface.NRGrad - вернуть три числа из функции Storm не умеет.
void NR_CalcGrad(int iValue, int iMin, int iMax)
{
	float fVal = iValue;
	float fLo  = iMin;
	float fHi  = iMax;
	float fPos = 0.0;
	float fK   = 0.0;
	if (fHi > fLo) fPos = (fVal - fLo) / (fHi - fLo);
	if (fPos < 0.0) fPos = 0.0;
	if (fPos > 1.0) fPos = 1.0;
	if (fPos < 0.5)
	{
		fK = fPos * 2.0;             // нижняя половина: красный -> жёлтый
		GameInterface.NRGrad.r = 235;
		GameInterface.NRGrad.g = makeint(70.0 + 165.0 * fK);
		GameInterface.NRGrad.b = makeint(70.0 + 50.0 * fK);
	}
	else
	{
		fK = (fPos - 0.5) * 2.0;     // верхняя половина: жёлтый -> зелёный
		GameInterface.NRGrad.r = makeint(235.0 - 120.0 * fK);
		GameInterface.NRGrad.g = 235;
		GameInterface.NRGrad.b = 120;
	}
}

// LEO: тег СОБИРАЕТСЯ из кусков - целым литералом его держать нельзя, компилятор Storm портит «=»
//   внутри строки, и цвет молча не применяется (см. treasurer_report.c).
string NR_ColorTag(int iValue, int iMin, int iMax)
{
	NR_CalcGrad(iValue, iMin, iMax);
	return "<color=255," + GameInterface.NRGrad.r + "," + GameInterface.NRGrad.g + "," + GameInterface.NRGrad.b + ">";
}

// LEO: тот же градиент, но числом - для CreateString, который цвет принимает параметром.
int NR_GradColor(int iValue, int iMin, int iMax)
{
	NR_CalcGrad(iValue, iMin, iMax);
	return argb(255, sti(GameInterface.NRGrad.r), sti(GameInterface.NRGrad.g), sti(GameInterface.NRGrad.b));
}

// LEO: подпись отношения, покрашенная по положению на шкале. Числа рисует отдельная строка справа.
string NR_RelationLine(string sName, int iValue, int iMin, int iMax)
{
	return NR_ColorTag(iValue, iMin, iMax) + sName + "</color>";
}
void CalculateHunter()
{

    ref mc = GetMainCharacter();

    // Gregg: шаг строк 42 (было 52), щиты 32x32 (было 40x40) - уплотнение освободило место
    // под седьмую строку внизу: репутация и пай в ГВИК. Позиции текстов/полос - в NationRelation.ini
    CreateImage("Eng",  "NATIONS", "ENGLAND",  570, 136, 607, 173);
    CreateImage("Fra",  "NATIONS", "FRANCE",   570, 178, 607, 215);
    CreateImage("Spa",  "NATIONS", "SPAIN",    570, 220, 607, 257);
    CreateImage("Hol",  "NATIONS", "HOLLAND",  570, 262, 607, 299);
    CreateImage("Pir",  "NATIONS", "Pirate",   570, 304, 607, 341);
    CreateImage("Smug", "NATIONS", "Smuggler", 570, 346, 607, 383);

    GameInterface.strings.EngH = XI_ConvertString("England");
    GameInterface.strings.FraH = XI_ConvertString("France");
    GameInterface.strings.SpaH = XI_ConvertString("Spain");
    GameInterface.strings.HolH = XI_ConvertString("Holland");
    if (LanguageGetLanguage() == "russian")
    {
        GameInterface.strings.PirH  = "Пираты";
        GameInterface.strings.SmugH = "Контрабандисты";
    }
    else
    {
        GameInterface.strings.PirH  = "Pirates";
        GameInterface.strings.SmugH = "Smugglers";
    }
    //#20200219-01
    string sText;

    // пираты - reputation.pirate (GetCharacterReputation idx 4; сейчас заглушка, геймплеем не меняется)
    // контрабандисты - questTemp.Contraband.relation (0..99, ведётся лазейками контрабанды)
    int iPir  = GetCharacterReputation(pchar, 4);
    int iSmug = 0;
    if (CheckAttribute(pchar, "questTemp.Contraband.relation")) iSmug = makeint(pchar.questTemp.Contraband.relation);
    // значения полос считаем ОДИН раз и ими же подписываем строки - цифры и заполнение полосы обязаны сходиться
    int iEng = ChangeCharacterNationReputation(pchar, ENGLAND, 0);
    int iFra = ChangeCharacterNationReputation(pchar, FRANCE,  0);
    int iSpa = ChangeCharacterNationReputation(pchar, SPAIN,   0);
    int iHol = ChangeCharacterNationReputation(pchar, HOLLAND, 0);

    SetFormatedText("Eng_TEXT",  NR_RelationLine(NR_NationRelationName(iEng),  iEng,  -100, 100));
    SetFormatedText("Fra_TEXT",  NR_RelationLine(NR_NationRelationName(iFra),  iFra,  -100, 100));
    SetFormatedText("Spa_TEXT",  NR_RelationLine(NR_NationRelationName(iSpa),  iSpa,  -100, 100));
    SetFormatedText("Hol_TEXT",  NR_RelationLine(NR_NationRelationName(iHol),  iHol,  -100, 100));
    SetFormatedText("Pir_TEXT",  NR_RelationLine(NR_NationRelationName(iPir),  iPir,  -100, 100));
    SetFormatedText("Smug_TEXT", NR_RelationLine(NR_SmugglerRelationName(iSmug), iSmug, 0, 99));

    GameInterface.strings.EngV  = NR_ValueText(iEng,  100);
    GameInterface.strings.FraV  = NR_ValueText(iFra,  100);
    GameInterface.strings.SpaV  = NR_ValueText(iSpa,  100);
    GameInterface.strings.HolV  = NR_ValueText(iHol,  100);
    GameInterface.strings.PirV  = NR_ValueText(iPir,  100);
    GameInterface.strings.SmugV = NR_ValueText(iSmug, 99);

	GameInterface.StatusLine.BAR_ENG.Max   = 100;
	GameInterface.StatusLine.BAR_ENG.Min   = -100;
	GameInterface.StatusLine.BAR_ENG.Value = iEng;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_ENG", 0);

	GameInterface.StatusLine.BAR_FRA.Max   = 100;
	GameInterface.StatusLine.BAR_FRA.Min   = -100;
	GameInterface.StatusLine.BAR_FRA.Value = iFra;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_FRA", 0);

	GameInterface.StatusLine.BAR_SPA.Max   = 100;
	GameInterface.StatusLine.BAR_SPA.Min   = -100;
	GameInterface.StatusLine.BAR_SPA.Value = iSpa;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_SPA", 0);

	GameInterface.StatusLine.BAR_HOL.Max   = 100;
	GameInterface.StatusLine.BAR_HOL.Min   = -100;
	GameInterface.StatusLine.BAR_HOL.Value = iHol;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_HOL", 0);

	GameInterface.StatusLine.BAR_PIR.Max   = 100;
	GameInterface.StatusLine.BAR_PIR.Min   = -100;
	GameInterface.StatusLine.BAR_PIR.Value = iPir;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_PIR", 0);

	GameInterface.StatusLine.BAR_SMUG.Max   = 99;
	GameInterface.StatusLine.BAR_SMUG.Min   = 0;
	GameInterface.StatusLine.BAR_SMUG.Value = iSmug;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_SMUG", 0);

	// Gregg: репутация и пай в ГВИК. Строка показывается только тем, кто уже имел дела с Компанией -
	// иначе новичок видел бы «случайный клиент» и полосу в нуле, не понимая, откуда это взялось.
	// Названия уровней синхронны узлу GWIK_Standing в DIALOGS\<язык>\GWIK.c, пороги - GWIK_RepLevel (Addons\utils.c)
	int iGwikRep  = 0;
	int iGwikLvl  = 0;
	int iGwikBase = 0; // границы текущего ранга - полоса и числа показывают путь до СЛЕДУЮЩЕГО ранга, а не общий прогресс
	int iGwikNext = 50;
	if (CheckAttribute(pchar, "GWIK"))
	{
		if (CheckAttribute(pchar, "GWIK.Rep")) iGwikRep = sti(pchar.GWIK.Rep);
		iGwikLvl = GWIK_RepLevel();
		if (iGwikLvl == 1) { iGwikBase = 50;  iGwikNext = 150; }
		if (iGwikLvl == 2) { iGwikBase = 150; iGwikNext = 350; }
		if (iGwikLvl == 3) { iGwikBase = 350; iGwikNext = 700; }
		if (LanguageGetLanguage() == "russian")
		{
			GameInterface.strings.GwikH = "Рейтинг ГВИК";
			sText = "случайный клиент";
			if (iGwikLvl == 1) sText = "знакомый Компании";
			if (iGwikLvl == 2) sText = "доверенное лицо";
			if (iGwikLvl == 3) sText = "друг Компании";
			if (iGwikLvl >= 4) sText = "столп Компании";
			GameInterface.strings.GwikRep = sText;
			GameInterface.strings.GwikVal = iGwikRep + " / " + iGwikNext;
			if (iGwikLvl >= 4) GameInterface.strings.GwikVal = iGwikRep + " - высший";
		}
		else
		{
			GameInterface.strings.GwikH = "WIC standing";
			sText = "casual client";
			if (iGwikLvl == 1) sText = "Company acquaintance";
			if (iGwikLvl == 2) sText = "trusted hand";
			if (iGwikLvl == 3) sText = "friend of the Company";
			if (iGwikLvl >= 4) sText = "pillar of the Company";
			GameInterface.strings.GwikRep = sText;
			GameInterface.strings.GwikVal = iGwikRep + " / " + iGwikNext;
			if (iGwikLvl >= 4) GameInterface.strings.GwikVal = iGwikRep + " - highest";
		}
		// LEO: красим по СТУПЕНИ (по прогрессу внутри ранга «столп Компании» краснел бы сразу после
		//   повышения). Шкала односторонняя - нулевая ступень это «дел ещё не имели», а не вражда, поэтому
		//   нижняя граница -4: ноль попадает в середину градиента, а не в красный. Повторный CreateString
		//   с тем же именем не плодит узел, а переустанавливает цвет.
		CreateString(true,"GwikRep", GameInterface.strings.GwikRep, "INTERFACE_ULTRASMALL", NR_GradColor(iGwikLvl, -4, 4), 570, 407, SCRIPT_ALIGN_LEFT, 1.0);
		GameInterface.StatusLine.BAR_GWIK.Min   = iGwikBase;
		GameInterface.StatusLine.BAR_GWIK.Max   = iGwikNext;
		GameInterface.StatusLine.BAR_GWIK.Value = iGwikRep;
		if (iGwikLvl >= 4) // высший ранг: границы сравнялись бы, полоса просто полная
		{
			GameInterface.StatusLine.BAR_GWIK.Min   = 0;
			GameInterface.StatusLine.BAR_GWIK.Max   = 700;
			GameInterface.StatusLine.BAR_GWIK.Value = 700;
		}
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "BAR_GWIK", 0);
	}
	else
	{
		SetNodeUsing("BAR_GWIK", false); // дел с Компанией не было - блока нет
	}
	DeleteAttribute(&GameInterface, "NRGrad"); // временные компоненты градиента, дальше не нужны
}

void ProcessExitCancel()
{
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
    DelEventHandler("FlagsPress","FlagsProcess");
    DelEventHandler("ievnt_command","ProcessCommandExecute");
    DelEventHandler("MouseRClickUP","HideInfo");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("CheckButtonChange","procCheckBoxChange");
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

string GwikShareName() // Gregg: название пая для подсказки по правому клику - в самом блоке рейтинга место занято рангом доверия
{
	int iShare = 0;
	if (CheckAttribute(pchar, "GWIK.Share.Level")) iShare = sti(pchar.GWIK.Share.Level);
	if (LanguageGetLanguage() == "russian")
	{
		if (iShare == 1) return "пайщик";
		if (iShare == 2) return "партнёр";
		if (iShare >= 3) return "компаньон";
		return "пая нет";
	}
	if (iShare == 1) return "shareholder";
	if (iShare == 2) return "partner";
	if (iShare >= 3) return "associate";
	return "no share";
}

void ShowInfoWindow()
{
	string sHeader = "TEST";
	string sNode = GetCurrentNode();

	string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
	int iGwikPoints = 0; // Gregg: очки доверия ГВИК для подсказки (объявление на уровне функции - Storm не любит их во вложенных блоках)
	sPicture = "none";
	sGroup = "none";
	sGroupPicture = "none";

	switch(sNode)
	{
		case "FlagPic":
			sHeader = XI_ConvertString("Nation");
			sText1 = GetRPGText("Nation_hint");
		break;

		case "INFO_CLICK":
			sHeader = XI_ConvertString("Hunter_type");
			sText1 = GetRPGText("Hunter_hint");
			if (CheckAttribute(pchar, "GWIK")) // Gregg: пай в блоке рейтинга не показан (там ранг доверия) - отдаём подсказке
			{
				if (CheckAttribute(pchar, "GWIK.Rep")) iGwikPoints = sti(pchar.GWIK.Rep);
				if (LanguageGetLanguage() == "russian") sText2 = "Голландская Вест-Индская компания: " + GwikShareName() + ", очков доверия " + iGwikPoints + ".";
				else sText2 = "Dutch West India Company: " + GwikShareName() + ", trust points " + iGwikPoints + ".";
			}
		break;
	}
	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,255,255,255), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
}

void HideInfo()
{
	CloseTooltip();
	SetCurrentNode("OK_BUTTON");
}

void FlagsProcess()
{
	// boal 04.04.2004 -->
	bool bTmpBool = true;
	int i, cn;
	ref chref;

	if (CheckAttribute(pchar, "DisableChangeFlagMode") || IsCompanion(CharacterFromID("Flint")) || CheckAttribute(pchar, "BS_permanentblackflag")) return; // нефиг менять файл за 3 секунды сразу

	if (!bBettaTestMode)
	{
    	if (bLandInterfaceStart)  bTmpBool = false; // на суше нельзя в принципе
    	if (bSeaActive && !CheckEnemyCompanionDistance2GoAway(false)) bTmpBool = false; // компаньон под ударом
        // проверка класса кораблей в эскадре -->
        // проверка класса кораблей в эскадре <--
	}
	if (bDisableMapEnter || !bTmpBool)
	{
        if (!bBettaTestMode)
        {
            PlaySound("knock");
            return;
        }
	}
	// смена флага
	Sea_ClearCheckFlag(); // сбросить всем в море проверку смотрения на флаг.
	Flag_Change(curNationIdx);
	Sea_ReconcileRelationsToFlag(); // LEO (баг 2): снять застрявшую движковую вражду боевых кораблей/осадников по нации. ТОЛЬКО игроцкий морской путь (не в Flag_Change - его дёргают квесты)
	pchar.DisableChangeFlagMode = true; //закрываем Флаг
	PChar.GenQuest.VideoAVI = GetNationNameByType(curNationIdx);
	PChar.GenQuest.VideoAfterQuest = "";
	PChar.quest.FreeChangeFlagMode.win_condition.l1 = "Timer";
	PChar.quest.FreeChangeFlagMode.win_condition.l1.date.day = GetAddingDataDay(0, 0, 0);
	PChar.quest.FreeChangeFlagMode.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 0);
	PChar.quest.FreeChangeFlagMode.win_condition.l1.date.year = GetAddingDataYear(0, 0, 0);
	PChar.quest.FreeChangeFlagMode.win_condition.l1.date.hour = GetHour() + 1;
	PChar.quest.FreeChangeFlagMode.function = "FreeChangeFlagMode";
	ProcessExitCancel();
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
    switch(nodName)
	{
        case "LEFTCHANGE_NATION":
    		if(comName=="activate" || comName=="click")
    		{
				if (!CheckAttribute(pchar, "DisableChangeFlagMode"))
				{
					SetNewNation(-1);
				}
    		}
    	break;

    	case "RIGHTCHANGE_NATION":
    		if(comName=="activate" || comName=="click")
    		{
                if (!CheckAttribute(pchar, "DisableChangeFlagMode"))
                {
					SetNewNation(1);
				}
    		}
    	break;
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
void SetNewNation(int add)
{
    ref   mchar = GetMainCharacter();
    bool  ok, ok2;

    curNationIdx = curNationIdx + add;
    if (curNationIdx < 0) curNationIdx = 4;
    if (curNationIdx > 4) curNationIdx = 0;
    SetNewGroupPicture("FlagPic", "NATIONS", GetNationNameByType(curNationIdx));

    if (IsCharacterPerkOn(mchar,"FlagPir")  ||
	    IsCharacterPerkOn(mchar,"FlagEng")  ||
		IsCharacterPerkOn(mchar,"FlagFra")  ||
		IsCharacterPerkOn(mchar,"FlagSpa")  ||
		IsCharacterPerkOn(mchar,"FlagHol"))
    {
        SetSelectable("FLAGS",true);
        ok  = !IsCharacterPerkOn(mchar,"Flag" + NationShortName(curNationIdx)) && (sti(mchar.nation) != curNationIdx);
        ok2 =  true;
        if (isMainCharacterPatented())
        {
            ok2 = (sti(Items[sti(mchar.EquipedPatentId)].Nation) != curNationIdx);
        }
        if (ok && ok2)
        {
            SetNewNation(add);
        }
        if (sti(mchar.nation) == curNationIdx)
        {
            SetSelectable("FLAGS",false);
        }
    }
    else
    {
        SetSelectable("FLAGS",false);
	    SetNodeUsing("RIGHTCHANGE_NATION",false);
	    SetNodeUsing("LEFTCHANGE_NATION",false);
    }
}
