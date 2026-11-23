#include "ITEMS\initItems.c"
#include "STORE\initGoods.c"

string totalInfo;
int idLngFile = -1;
int remInt = 0;
string descF1;
string descF2;
string descF3;
string descF4;
string descF5;
string descF6;
string descF7;
string descF8;
string descF9;
string descF10;
string descF11;
string descF12;
string descF13;
string descF14;
string descF15;
string descF16;
string descF17;
string descF18;
string descF19;
string descF20;
string descF21;
string descF22;
string descF23;
string descF24;
string descF25;
string descF26;
string descF27;
string descF28;
string descF29;
string descF30;
string descF31;
string descF32;
string descF33;
string descF34;
string descF35;
string descF36;
string descF37;
string descF38;
string descF39;
string descF40;
string descF41;
string descF42;
string descF43;
string descF44;
string descF45;
string descF46;
string descF47;
string descF48;
string descF49;
string descF50;
string descF51;
string descF52;
string descF53;
string descF54;
string descF55;
string descF56;
string descF57;
string descF58;
string descF59;
string descF60;
string descF61;
string descF62;
string descF63;
string descF64;
string descF65;
string descF66;
string descF67;
string descF68;
string descF69;
string descF70;
string descF71;
string descF72;
string descF73;
string descF74;
string descF75;
string descF76;
string descF77;
string descF78;
string descF79;
string descF80;
string descF81;
string descF82;
string descF83;
string descF84;
string descF85;
string descF86;
string descF87;
string descF88;
string descF89;
string descF90;
string descF91;
string descF92;
string descF93;
string descF94;
string descF95;
string descF96;
string descF97;
string descF98;
string descF99;
string descF100;

void InitInterface(string iniName)
{
 	StartAboveForm(true);
	InitDesc();

	GameInterface.title = "titleBoal_Debug";

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	CalculateInfoData();

	CalculateCheatsInfo(); // Warship. Статистика - сколько читов юзали

	SetFormatedText("INFO_TEXT", totalInfo);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);


	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("SetScrollerPos","SetScrollerPos",0);
	SetEventHandler("ScrollPosChange","ProcScrollPosChange",0);
	SetEventHandler("ScrollTopChange","ProcScrollChange",0);

    GameInterface.reload_edit.str = "Pirates_Shipyard, reload, reload1";

	// LEO: радиус снимка вокруг корабля правится в самой строке чита и живёт в настройках, а не в сейве.
	if(!CheckAttribute(&InterfaceStates, "DbgIslandMapRadius")) { InterfaceStates.DbgIslandMapRadius = 14000; }
	GameInterface.radius_edit.str = InterfaceStates.DbgIslandMapRadius;
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_QUICK_SAVE );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_QUICK_SAVE );
}

void IDoExit(int exitCode)
{
    EndAboveForm(true);

	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("ievnt_command","ProcCommand");
 	DelEventHandler("SetScrollerPos","SetScrollerPos");
	DelEventHandler("ScrollPosChange","ProcScrollPosChange");
	DelEventHandler("ScrollTopChange","ProcScrollChange");

	if(bSeaActive)
    {
        RefreshBattleInterface();
    }
	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}


void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void CalculateInfoData()
{
	// LEO: список читов рисует лента - у каждой строки своя кнопка и своё описание,
	//   поэтому описания раскладываем по узлам, а в поле вывода остаётся одна шапка.
	totalInfo = GetConvertStrWithReplace("Variable_Debuger_2", "Interface.txt", "#space#", " ");
	SetFormatedText("D_F1", "- " + descF1);
	SetFormatedText("D_F2", "- " + descF2);
	SetFormatedText("D_F3", "- " + descF3);
	SetFormatedText("D_F4", "- " + descF4);
	SetFormatedText("D_F5", "- " + descF5);
	SetFormatedText("D_F6", "- " + descF6);
	SetFormatedText("D_F7", "- " + descF7);
	SetFormatedText("D_F8", "- " + descF8);
	SetFormatedText("D_F9", "- " + descF9);
	SetFormatedText("D_F10", "- " + descF10);
	SetFormatedText("D_F11", "- " + descF11);
	SetFormatedText("D_F12", "- " + descF12);
	SetFormatedText("D_F13", "- " + descF13);
	SetFormatedText("D_F14", "- " + descF14);
	SetFormatedText("D_F15", "- " + descF15);
	SetFormatedText("D_F16", "- " + descF16);
	SetFormatedText("D_F17", "- " + descF17);
	SetFormatedText("D_F18", "- " + descF18);
	SetFormatedText("D_F19", "- " + descF19);
	SetFormatedText("D_F20", "- " + descF20);
	SetFormatedText("D_F21", "- " + descF21);
	SetFormatedText("D_F22", "- " + descF22);
	SetFormatedText("D_F23", "- " + descF23);
	SetFormatedText("D_F24", "- " + descF24);
	SetFormatedText("D_F25", "- " + descF25);
	SetFormatedText("D_F26", "- " + descF26);
	SetFormatedText("D_F27", "- " + descF27);
	SetFormatedText("D_F28", "- " + descF28);
	SetFormatedText("D_F29", "- " + descF29);
	SetFormatedText("D_F30", "- " + descF30);
	SetFormatedText("D_F31", "- " + descF31);
	SetFormatedText("D_F32", "- " + descF32);
	SetFormatedText("D_F33", "- " + descF33);
	SetFormatedText("D_F34", "- " + descF34);
	SetFormatedText("D_F35", "- " + descF35);
	SetFormatedText("D_F36", "- " + descF36);
	SetFormatedText("D_F37", "- " + descF37);
	SetFormatedText("D_F38", "- " + descF38);
	SetFormatedText("D_F39", "- " + descF39);
	SetFormatedText("D_F40", "- " + descF40);
	SetFormatedText("D_F41", "- " + descF41);
	SetFormatedText("D_F42", "- " + descF42);
	SetFormatedText("D_F43", "- " + descF43);
	SetFormatedText("D_F44", "- " + descF44);
	SetFormatedText("D_F45", "- " + descF45);
	SetFormatedText("D_F46", "- " + descF46);
	SetFormatedText("D_F47", "- " + descF47);
	SetFormatedText("D_F48", "- " + descF48);
	SetFormatedText("D_F49", "- " + descF49);
	SetFormatedText("D_F50", "- " + descF50);
	SetFormatedText("D_F51", "- " + descF51);
	SetFormatedText("D_F52", "- " + descF52);
	SetFormatedText("D_F53", "- " + descF53);
	SetFormatedText("D_F54", "- " + descF54);
	SetFormatedText("D_F55", "- " + descF55);
	SetFormatedText("D_F56", "- " + descF56);
	SetFormatedText("D_F57", "- " + descF57);
	SetFormatedText("D_F58", "- " + descF58);
	SetFormatedText("D_F59", "- " + descF59);
	SetFormatedText("D_F60", "- " + descF60);
	SetFormatedText("D_F61", "- " + descF61);
	SetFormatedText("D_F62", "- " + descF62);
	SetFormatedText("D_F63", "- " + descF63);
	SetFormatedText("D_F64", "- " + descF64);
	SetFormatedText("D_F65", "- " + descF65);
	SetFormatedText("D_F66", "- " + descF66);
	SetFormatedText("D_F67", "- " + descF67);
	SetFormatedText("D_F68", "- " + descF68);
	SetFormatedText("D_F69", "- " + descF69);
	SetFormatedText("D_F70", "- " + descF70);
	SetFormatedText("D_F71", "- " + descF71);
	SetFormatedText("D_F72", "- " + descF72);
	SetFormatedText("D_F73", "- " + descF73);
	SetFormatedText("D_F74", "- " + descF74);
	SetFormatedText("D_F75", "- " + descF75);
	SetFormatedText("D_F76", "- " + descF76);
	SetFormatedText("D_F77", "- " + descF77);
	SetFormatedText("D_F78", "- " + descF78);
	SetFormatedText("D_F79", "- " + descF79);
	SetFormatedText("D_F80", "- " + descF80);
	SetFormatedText("D_F81", "- " + descF81);
	SetFormatedText("D_F82", "- " + descF82);
	SetFormatedText("D_F83", "- " + descF83);
	SetFormatedText("D_F84", "- " + descF84);
	SetFormatedText("D_F85", "- " + descF85);
	SetFormatedText("D_F86", "- " + descF86);
	SetFormatedText("D_F87", "- " + descF87);
	SetFormatedText("D_F88", "- " + descF88);
	SetFormatedText("D_F89", "- " + descF89);
	SetFormatedText("D_F90", "- " + descF90);
	SetFormatedText("D_F91", "- " + descF91);
	SetFormatedText("D_F92", "- " + descF92);
	SetFormatedText("D_F93", "- " + descF93);
	SetFormatedText("D_F94", "- " + descF94);
	SetFormatedText("D_F95", "- " + descF95);
	SetFormatedText("D_F96", "- " + descF96);
	SetFormatedText("D_F97", "- " + descF97);
	SetFormatedText("D_F98", "- " + descF98);
	SetFormatedText("D_F99", "- " + descF99);
	SetFormatedText("D_F100", "- " + descF100);
}
void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
	    case "B_F1":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF1();
		  }
	    break;
	    case "B_F2":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF2();
		  }
	    break;
	    case "B_F3":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF3();
		  }
	    break;
	    case "B_F4":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF4();
		  }
	    break;
	    case "B_F5":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF5();
		  }
	    break;

	    case "B_F6":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF6();
		  }
	    break;

	    case "B_F7":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF7();
		  }
	    break;

	    case "B_F8":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF8();
		  }
	    break;

	    case "B_F9":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF9();
		  }
	    break;

	    case "B_F10":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF10();
		  }
	    break;

	    case "B_F11":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF11();
		  }
	    break;

	    case "B_F12":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF12();
		  }
	    break;

	    case "B_F13":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF13();
		  }
	    break;

	    case "B_F14":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF14();
		  }
	    break;

	    case "B_F15":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF15();
		  }
	    break;

	    case "B_F16":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF16();
		  }
	    break;

	    case "B_F17":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF17();
		  }
	    break;

	    case "B_F18":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF18();
		  }
	    break;

	    case "B_F19":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF19();
		  }
	    break;

        case "B_F20":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF20();
		  }
	    break;

	    case "B_F21":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF21();
		  }
	    break;
	    case "B_F22":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF22();
		  }
	    break;
	    case "B_F23":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF23();
		  }
	    break;
	    case "B_F24":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF24();
		  }
	    break;
	    case "B_F25":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF25();
		  }
	    break;
	    case "B_F26":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF26();
		  }
	    break;
	    case "B_F27":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF27();
		  }
	    break;
	    case "B_F28":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF28();
		  }
	    break;
	    case "B_F29":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF29();
		  }
	    break;
	    case "B_F30":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF30();
		  }
	    break;
	    case "B_F31":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF31();
		}
	    break;
	    case "B_F32":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF32();
		}
	    break;
		case "B_F33":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF33();
		}
	    break;
		case "B_F34":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF34();
		}
	    break;
		case "B_F35":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF35();
		}
	    break;
		case "B_F36":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF36();
		}
	    break;
		case "B_F37":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF37();
		}
	    break;
		case "B_F38":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF38();
		}
	    break;
		case "B_F39":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF39();
		}
	    break;
		case "B_F40":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF40();
		}
	    break;
		case "B_F41":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF41();
		}
	    break;
		case "B_F42":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF42();
		}
	    break;
		case "B_F43":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF43();
		}
	    break;
		case "B_F44":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF44();
		}
	    break;
	    case "B_F45":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF45();
		  }
	    break;
	    case "B_F46":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF46();
		  }
	    break;
	    case "B_F47":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF47();
		  }
	    break;
	    case "B_F48":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF48();
		  }
	    break;
	    case "B_F49":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF49();
		  }
	    break;

	    case "B_F50":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF50();
		  }
	    break;

	    case "B_F51":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF51();
		  }
	    break;

	    case "B_F52":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF52();
		  }
	    break;

	    case "B_F53":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF53();
		  }
	    break;

	    case "B_F54":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF54();
		  }
	    break;

	    case "B_F55":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF55();
		  }
	    break;

	    case "B_F56":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF56();
		  }
	    break;

	    case "B_F57":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF57();
		  }
	    break;

	    case "B_F58":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF58();
		  }
	    break;

	    case "B_F59":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF59();
		  }
	    break;

	    case "B_F60":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF60();
		  }
	    break;

	    case "B_F61":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF61();
		  }
	    break;

	    case "B_F62":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF62();
		  }
	    break;

	    case "B_F63":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF63();
		  }
	    break;

        case "B_F64":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF64();
		  }
	    break;

	    case "B_F65":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF65();
		  }
	    break;
	    case "B_F66":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF66();
		  }
	    break;
	    case "B_F67":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF67();
		  }
	    break;
	    case "B_F68":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF68();
		  }
	    break;
	    case "B_F69":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF69();
		  }
	    break;
	    case "B_F70":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF70();
		  }
	    break;
	    case "B_F71":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF71();
		  }
	    break;
	    case "B_F72":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF72();
		  }
	    break;
	    case "B_F73":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF73();
		  }
	    break;
	    case "B_F74":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF74();
		  }
	    break;
	    case "B_F75":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF75();
		}
	    break;
	    case "B_F76":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF76();
		}
	    break;
		case "B_F77":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF77();
		}
	    break;
		case "B_F78":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF78();
		}
	    break;
		case "B_F79":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF79();
		}
	    break;
		case "B_F80":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF80();
		}
	    break;
		case "B_F81":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF81();
		}
	    break;
		case "B_F82":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF82();
		}
	    break;
		case "B_F83":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF83();
		}
	    break;
		case "B_F84":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF84();
		}
	    break;
		case "B_F85":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF85();
		}
	    break;
		case "B_F86":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF86();
		}
	    break;
		case "B_F87":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF87();
		}
	    break;
		case "B_F88":
		if(comName=="activate" || comName=="click")
		{
			CalculateInfoDataF88();
		}
	    break;
	    case "B_F89":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F90":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F91":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F92":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F93":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F94":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F95":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F96":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F97":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F98":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F99":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_F100":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataFree();
		  }
	    break;
	    case "B_GOODS":
	      if(comName=="activate" || comName=="click")
	      {
	          IDoExit(RC_INTERFACE_DO_GOODS_SPAWN);
	      }
	    break;
	    case "B_SHIP":
	      if(comName=="activate" || comName=="click")
	      {
	          IDoExit(RC_INTERFACE_DO_SHIP_CHANGE);
	      }
	    break;
	    case "B_BETA":
		  if(comName=="activate" || comName=="click")
		  {
              IDoExit(RC_INTERFACE_DO_BOAL_BETA);
		  }
	    break;
	    case "B_RADIUS_OK":
		  if(comName=="activate" || comName=="click")
		  {
		      ApplyMapRadius();
		  }
	    break;
	    case "B_RELOAD":
		  if(comName=="activate" || comName=="click")
		  {
              ReloadByStr();
		  }
	    break;
	}

	CalculateCheatsInfo(); // Статистика по читам
}

////////////////////////////////////////////////////////////////////////

// LEO: кнопки 89-100 держим про запас - под них вешаются новые читы.
// LEO: радиус снимка карты - зажимаем введённое в поле строки 71 и кладём в InterfaceStates
//   (файл настроек, не сейв игрока), чтобы значение пережило закрытие окна.
int GetMapRadius()
{
	int nMapRadius = sti(stripblank(GameInterface.radius_edit.str));
	if(nMapRadius < 100) { nMapRadius = 100; }
	if(nMapRadius > 100000) { nMapRadius = 100000; }
	InterfaceStates.DbgIslandMapRadius = nMapRadius;
	GameInterface.radius_edit.str = nMapRadius;
	return nMapRadius;
}

void ApplyMapRadius()
{
	int nMapRadius = GetMapRadius();
	totalInfo = "Радиус снимка вокруг корабля: " + nMapRadius + " м.";
	SetFormatedText("INFO_TEXT", totalInfo);
}

////////////////////////////////////////////////////////////////////////

void CalculateInfoDataFree()
{
	totalInfo = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	totalInfo = totalInfo + NewStr() + NewStr() +
	            GetConvertStrWithReplace("Variable_Debuger_165", "Interface.txt", "#space#", " ");
	SetFormatedText("INFO_TEXT", totalInfo);
}

////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF1()
{
	totalInfo = descF1;
	Pchar.money = sti(Pchar.money) + 500000;
	totalInfo = totalInfo + GetAssembledString(" Денег у #sName#а теперь #dmoney#",	Pchar);
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_3", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	Statistic_AddValue(PChar, "Cheats.F1", 1);
}

////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF2()
{
	totalInfo = descF2;
	CreateSiege("");
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_4", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	ProcessCancelExit();
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F2", 1);
}
////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF3()
{
	ref chref;
	int i, iKilled, iSkipped;
	totalInfo = descF3;
	iKilled = 0;
	iSkipped = 0;
	// Только персонажи текущей локации.
	for (i = 0; i < TOTAL_CHARACTERS; i++)
	{
		makeref(chref, Characters[i]);
		if (i == GetMainCharacterIndex()) continue;
		if (!CheckAttribute(chref, "location")) continue;
		if (chref.location != pchar.location) continue;
		if (!CheckAttribute(chref, "chr_ai.group")) continue;   // не заведён в ИИ локации - не боец
		if (LAi_IsDead(chref)) continue;
		if (!LAi_group_IsEnemy(pchar, chref)) continue;         // свои, офицеры и нейтралы не трогаются
		// LAi_KillCharacter снимает бессмертие; квестовых бессмертных пропускаем.
		if (LAi_IsImmortal(chref))
		{
			iSkipped++;
			continue;
		}
		LAi_KillCharacter(chref);
		iKilled++;
	}
	SetFormatedText("INFO_TEXT", totalInfo + "\nУбито врагов: " + iKilled + ", пропущено неуязвимых: " + iSkipped);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F3", 1);
}
////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF4()
{
	totalInfo = descF4;
    ChangeCrewExp(pchar, "Sailors", 10);
	ChangeCrewExp(pchar, "Cannoners", 10);
	ChangeCrewExp(pchar, "Soldiers", 10);

    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_6", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F4", 1);
}
////////////////////////////////////////////////////////////////////////
int BOAL_debug_num = 1;
void CalculateInfoDataF5()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF5;
    pchar.Skill.FreeSkill = sti(pchar.Skill.FreeSkill) + 1000;
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
    GetConvertStrWithReplace("Variable_Debuger_8", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F5", 1);
}
////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF6()
{
	ref mc;
	string sEquipItem;
	totalInfo = descF6;
	mc = GetMainCharacter();
	mc.rank = 33;

	SetRandSelfSkill(mc, 100, 100);
	SetRandShipSkill(mc, 100, 100);
	LAi_SetHP(mc, 60000.0, 60000.0);

	mc.Ship.Type = GenerateShipExt(SHIP_SANTISIMA, true, mc);

	SetBaseShipData(mc);
	ref VeryRealShip = GetRealShip(sti(PChar.Ship.Type));
    DeleteAttribute(VeryRealShip,"EmblemedSails");
    DeleteAttribute(VeryRealShip,"shipsails");
    VeryRealShip.ship.upgrades.sails = 3;

	mc.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS42;
	SetCrewQuantityFull(mc);
	AddCrewMorale(mc, 100);
	ChangeCrewExp(mc, "Sailors", 100);
	ChangeCrewExp(mc, "Cannoners", 100);
	ChangeCrewExp(mc, "Soldiers", 100);

	SetCharacterGoods(mc, GOOD_BALLS, 1500);
	SetCharacterGoods(mc, GOOD_GRAPES, 1500);
	SetCharacterGoods(mc, GOOD_KNIPPELS, 1500);
	SetCharacterGoods(mc, GOOD_BOMBS, 1500);;
	SetCharacterGoods(mc, GOOD_FOOD, 1000);
	SetCharacterGoods(mc, GOOD_MEDICAMENT, 1000);
	SetCharacterGoods(mc, GOOD_POWDER, 3000);
	SetCharacterGoods(mc, GOOD_WEAPON, 1500);

	sEquipItem = GetCharacterEquipByGroup(mc, BLADE_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, GUN_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, SPYGLASS_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, CIRASS_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);

	AddMoneyToCharacter(mc, 1000000);
	AddItems(mc, "spyglass5", 1);
	AddItems(mc, "pistol7", 1);
	AddItems(mc, "Map_Best", 1);

	sEquipItem = GetGeneratedItem("blade32"); 				// Фламберж			(ЛО)
	AddItems(mc, sEquipItem, 1);

	AddItems(mc, "cirass5", 1);

	EquipCharacterbyItem(mc, "spyglass5");
	EquipCharacterbyItem(mc, "pistol7");
	GiveGunAmmunition(mc,"pistol7");
 	EquipCharacterbyItem(mc, sEquipItem);
 	EquipCharacterbyItem(mc, "cirass5");

 	SetHalfPerksToChar(mc, false);

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_10", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F6", 1);
}
////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF7()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF7;

	SetRandSelfSkill(PChar, 1, 1);

    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_12", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F7", 1);
}
////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF8()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF8;
	ref mc, ch;
	int n, idx;
	mc = GetMainCharacter();
    idx = GetCharIDXByParam("TavernMan", "location", mc.location);
    bool ok = true;
    while (ok)
    {
        for(n=0; n<MAX_CHARACTERS; n++)
    	{
    		makeref(ch,Characters[n]);
    		if (CheckAttribute(ch, "TavernMan"))
    		{
                if (ch.location == "none") continue; // фикс для новых, невидимых до поры островов
                if (n > idx)
                {
                    ok = false;
                    if (GetCityFrom_Sea(ch.City) != "")
                    {
						setCharacterShipLocation(mc, GetCityFrom_Sea(ch.City));
                        setWDMPointXZ(GetCityFrom_Sea(ch.City));
                    }
                    DoQuestReloadToLocation(ch.location,"reload","reload1", "");
                    Log_Info(ch.location);
                    break;
                }
    		}
        }
        idx = -1;
    }
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_14", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F8", 1);
}

void CalculateInfoDataF9()
{
	SetCharacterGoods(pchar, GOOD_BALLS, 2000);
	SetCharacterGoods(pchar, GOOD_GRAPES, 2000);
	SetCharacterGoods(pchar, GOOD_KNIPPELS, 2000);
	SetCharacterGoods(pchar, GOOD_BOMBS, 2000);;
	SetCharacterGoods(pchar, GOOD_FOOD, 6000);
	SetCharacterGoods(pchar, GOOD_POWDER, 3000);
	SetCharacterGoods(pchar, GOOD_SLAVES, 3000);
	SetCharacterGoods(pchar, GOOD_BRICK, 2500);
	SetCharacterGoods(pchar, GOOD_MAHOGANY, 1500);
	SetCharacterGoods(pchar, GOOD_EBONY, 1500);
	SetCharacterGoods(pchar, GOOD_PLANKS, 3000);
	SetCharacterGoods(pchar, GOOD_TOOLS, 1500);
	SetCharacterGoods(pchar, GOOD_MEDICAMENT, 10000);

	totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_16", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F19", 1);
}

////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF10()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF10;
	ref mc;
	mc = GetMainCharacter();
			if(LAi_IsImmortal(mc))
			{
				LAi_SetImmortal(mc, false);
				Log_SetStringToLog("God mode OFF");
			}else{
				LAi_SetImmortal(mc, true);
				Log_SetStringToLog("God mode ON");
			}
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_17", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F10", 1);
}
////////////////////////////////////////////////////////////////////////

void CalculateInfoDataF11()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	totalInfo = descF11;

    ref mc = GetMainCharacter();
	mc.reputation = sti(mc.reputation) + 10;
	if (sti(mc.reputation) > REPUTATION_MAX) mc.reputation = REPUTATION_MAX;

    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_19", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F11", 1);
}
////////////////////////////////////////////////////////////////////////
void CalculateInfoDataF12()
{
    totalInfo = descF12;

	pchar.reputation = sti(pchar.reputation) - 10;
	if (sti(pchar.reputation) < REPUTATION_MIN) pchar.reputation = REPUTATION_MIN;

    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_21", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F12", 1);
}

void CalculateInfoDataF13()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF13;

    ref mc;
	mc = GetMainCharacter();
    if(CheckAttribute(mc,"worldmapencountersoff") == 0)
	{
		mc.worldmapencountersoff = "1";
		Log_SetStringToLog("Worldmap encounters OFF");
	}
	else
	{
		if(mc.worldmapencountersoff == "1")
		{
			mc.worldmapencountersoff = "0";
			Log_SetStringToLog("Worldmap encounters ON");
		}
		else
		{
			mc.worldmapencountersoff = "1";
			Log_SetStringToLog("Worldmap encounters OFF");
		}
	}
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_22", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F13", 1);
}

void CalculateInfoDataF14()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF14;
    if(globalSGMode != false)
	{
		globalSGMode = false;
		Log_SetStringToLog("Shotgun mode OFF");
	}else{
		globalSGMode = true;
		Log_SetStringToLog("Shotgun mode ON");
	}
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно! Qfreze=";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F14", 1);
}

void CalculateInfoDataF15()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF15;

    DoQuestCheckDelay("Nation_Legend_Map", 2.0);
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_24", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
    ProcessCancelExit();
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F15", 1);
}

void CalculateInfoDataF16()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF16;

    LAi_Fade("", "");
    for (int i = 1; i<=5; i++)
    {
		AddDataToCurrent(0, 0, 1);
	}
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_25", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F16", 1);
}

void CalculateInfoDataF17()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF17;

    aref  curItem;
	aref  arItem;
	aref rootItems;
	int i, j, n;
	string attributeName;
	ref ch;


    makearef(rootItems, pchar.quest);

    trace("=================================================");
    DumpAttributes(rootItems);
    trace("=================================================");
    Log_Info("DumpAttributes");
    remInt++;
    totalInfo = totalInfo + " page " + remInt;
    for(i=0; i<GetAttributesNum(rootItems); i++)
    {
        if (i > remInt*60 || i < (remInt-1)*60) continue;

        curItem = GetAttributeN(rootItems, i);
		j = sti(GetAttributeValue(curItem));
		totalInfo = totalInfo+"   "+i+"= "+
                    GetAttributeName(curItem);

    }
    if (i <= remInt*60) remInt = 0;
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_26", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F17", 1);
}

void CalculateInfoDataF18()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF18;

	for(i=1; i<11; i++)
    {
		ref sld = GetCharacter(NPC_GenerateCharacter("DebugOfficer_"+i, "officer_"+i, "man", "man", 30, PIRATE, -1, true));
		sld.greeting = "Gr_questOfficer";
		sld.Dialog.Filename = "Enc_Officer_dialog.c";
		sld.quest.meeting = true;
		SetSelfSkill(sld, 100, 100, 100, 100, 100);
		SetShipSkill(sld, 100, 100, 100, 100, 100, 100, 100, 100, 100);
		SetSPECIAL(sld, 6, 10, 9, 10, 10, 8, 10);
		SetCharacterPerk(sld, "Energaiser"); // скрытый перк даёт 1.5 к приросту энергии, дается ГГ и боссам уровней
		SetCharacterPerk(sld, "BasicBattleState");
		SetCharacterPerk(sld, "AdvancedBattleState");
		SetCharacterPerk(sld, "ShipDefenseProfessional");
		SetCharacterPerk(sld, "LightRepair");
		SetCharacterPerk(sld, "InstantRepair");
		SetCharacterPerk(sld, "ShipTurnRateUp");
		SetCharacterPerk(sld, "ShipSpeedUp");
		SetCharacterPerk(sld, "StormProfessional");
		SetCharacterPerk(sld, "Turn180");
		SetCharacterPerk(sld, "SailingProfessional");
		SetCharacterPerk(sld, "Carpenter");
		SetCharacterPerk(sld, "Builder");
		SetCharacterPerk(sld, "WindCatcher");
		SetCharacterPerk(sld, "SailsMan");
		SetCharacterPerk(sld, "Doctor1");
		SetCharacterPerk(sld, "Doctor2");
		sld.quest.OfficerPrice = sti(pchar.rank)*100;
		Pchar.questTemp.HiringOfficerIDX = GetCharacterIndex(sld.id);
		sld.OfficerWantToGo.DontGo = true; //не пытаться уйти
		sld.loyality = MAX_LOYALITY;
		AddPassenger(pchar, sld, false);
		sld.location = "None";
		sld.Dialog.CurrentNode = "hired";
		sld.Payment = true;
		sld.DontClearDead = true;
		DeleteAttribute(Pchar, "questTemp.HiringOfficerIDX");
		DeleteAttribute(sld, "LifeDay")
		SaveCurrentNpcQuestDateParam(sld, "HiredDate"); // дата найма
	}
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_28", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F18", 1);
}

void CalculateInfoDataF19()
{
    totalInfo = descF19;
    if (sti(pchar.Fellows.Passengers.navigator) != -1)
	{
	    ref chr = GetCharacter(sti(pchar.Fellows.Passengers.navigator));

    	chr.Skill.FreeSkill  = sti(chr.Skill.FreeSkill) + 35;
    }
    else
    {
        totalInfo = GetConvertStrWithReplace("Variable_Debuger_29", "Interface.txt", "#space#", " ");
    }
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_30", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F19", 1);
}

void CalculateInfoDataF20()
{
    totalInfo = descF20;
	Sea_CreateLighthouse("Jamaica");
	Sea_CreateLighthouse("Cuba1");
	Sea_CreateLighthouse("Cuba2");
	SendMessage(&lighthouseLightModel, "lffffffffffff", MSG_MODEL_SET_POSITION,-116.0, 55.0, -1568.5, 1, 0, 0, 0, 1, 0, 0, 0, 1);
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_32", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F20", 1);
}

void CalculateInfoDataF21()
{
    totalInfo = descF21;
	locCameraLockNearHero(-5.0, 2.0, -5.0, 200, true);

    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_34", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F21", 1);
}

void CalculateInfoDataF22()
{
    totalInfo = descF22;
    {
		totalInfo = GetConvertStrWithReplace("Variable_Debuger_36", "Interface.txt", "#space#", " ") + pchar.questTemp.Ascold.TraderId + NewStr() +
					GetConvertStrWithReplace("Variable_Debuger_37", "Interface.txt", "#space#", " ") + pchar.questTemp.Ascold.MerchantColony + NewStr() +
					GetConvertStrWithReplace("Variable_Debuger_38", "Interface.txt", "#space#", " ") + pchar.questTemp.Ascold.ShipyarderId + NewStr()+
					GetConvertStrWithReplace("Variable_Debuger_39", "Interface.txt", "#space#", " ") + Items[GetItemIndex("Rock_letter")].startLocation + NewStr()+
					GetConvertStrWithReplace("Variable_Debuger_40", "Interface.txt", "#space#", " ") + Items[GetItemIndex("Rock_letter")].startLocator;
					if (pchar.questTemp.BlueBird.City != "")
					{
						totalInfo = totalInfo + NewStr()+ GetConvertStrWithReplace("Variable_Debuger_41", "Interface.txt", "#space#", " ") + pchar.questTemp.BlueBird.City;
					}
					else
					{
						totalInfo = totalInfo + NewStr()+ GetConvertStrWithReplace("Variable_Debuger_42", "Interface.txt", "#space#", " ");
					}
    }
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_43", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F22", 1);
}

void CalculateInfoDataF23()
{
    totalInfo = descF23;
    DumpAttributes(&Camera);
	totalInfo += "\n\nCamera.pos.x == " + Camera.pos.x +
		"\nCamera.pos.y = " + Camera.pos.y +
		"\nCamera.pos.z == " + Camera.pos.z +
		"\n\nCamera.ang.x == " + Camera.ang.x +
		"\nCamera.ang.y == " + Camera.ang.y +
		"\nCamera.ang.z == " + Camera.ang.z + "\n\n";
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_45", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F23", 1);
}

void CalculateInfoDataF24()
{
    totalInfo = descF24;
    GiveUniLicence(30); // универсальная торговая лицензия ГВИК (отладка)
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_47", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F24", 1);
}

void CalculateInfoDataF25()
{
    totalInfo = descF25;
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_49", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

    ProcessCancelExit();
    if( bSeaActive && !bAbordageStarted )
    {
    	Sea_ReloadStart();
	}
	else
	{
	    DoQuestReloadToLocation(pchar.location, pchar.location.group, pchar.location.locator, "");
	}

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F25", 1);
}

void CalculateInfoDataF26()
{
	totalInfo = descF26;
    ref characterRef = GetMainCharacter();

	trace("==================== " + characterRef.ID + " ====================");
	DumpAttributes(characterRef);
	trace("========================================");

	ref realShip;
	int shipType = sti(characterRef.Ship.Type);

	if(shipType != SHIP_NOTUSED) // Есть ли корабль вообще
	{
		realShip = GetRealShip(shipType);
		DumpAttributes(realShip);
	}

	Log_Info(GetConvertStrWithReplace("Variable_Debuger_51", "Interface.txt", "#space#", " "));
	totalInfo = totalInfo + NewStr() + NewStr() +
		GetConvertStrWithReplace("Variable_Debuger_52", "Interface.txt", "#space#", " ");
	SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.26", 1);
}

void CalculateInfoDataF27()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	totalInfo = descF27;
    for (int j=0; j< MAX_NATIONS; j++)
	{
        ChangeCharacterNationReputation(pchar, j, -50);
    }
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                GetConvertStrWithReplace("Variable_Debuger_53", "Interface.txt", "#space#", " ");
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F27", 1);
}

void CalculateInfoDataF28()
{
    totalInfo = descF28;
	GhostShipOnMap();
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_55", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F28", 1);
}

void CalculateInfoDataF29()
{
    totalInfo = descF29;
	for (int j=0; j< MAX_NATIONS; j++)
	{
        ChangeCharacterNationReputation(pchar, j, 50);
    }
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_57", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F29", 1);
}

void CalculateInfoDataF30()
{
    totalInfo = descF30;
	LAi_Fade("", "");
	AddDataToCurrent(0, 0, 20);
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_58", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F30", 1);
}

void CalculateInfoDataF31()
{
	ref mc;
	string sEquipItem;
	totalInfo = descF31;
	mc = GetMainCharacter();
	SetRandSelfSkill(mc, 100, 100);
	SetRandShipSkill(mc, 100, 100);
	LAi_SetHP(mc, 380.0, 380.0);
	mc.rank = 25;
	mc.Ship.Type = GenerateShipExt(SHIP_ARABELLA, true, mc);
	SetBaseShipData(mc);
	int iShip = pchar.ship.type;
	mc.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS32;
	SetCrewQuantityFull(mc);
	AddCrewMorale(mc, 100);
	ChangeCrewExp(mc, "Sailors", 100);
	ChangeCrewExp(mc, "Cannoners", 100);
	ChangeCrewExp(mc, "Soldiers", 100);

	SetCharacterGoods(mc, GOOD_BALLS, 1000);
	SetCharacterGoods(mc, GOOD_GRAPES, 1000);
	SetCharacterGoods(mc, GOOD_KNIPPELS, 1000);
	SetCharacterGoods(mc, GOOD_BOMBS, 1000);;
	SetCharacterGoods(mc, GOOD_FOOD, 6000);
	SetCharacterGoods(mc, GOOD_MEDICAMENT, 1000);
	SetCharacterGoods(mc, GOOD_POWDER, 4000);
	SetCharacterGoods(mc, GOOD_WEAPON, 500);
	AddAchievementPoints(10000);

	sEquipItem = GetCharacterEquipByGroup(mc, BLADE_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, GUN_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, SPYGLASS_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, CIRASS_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, BACKPACK_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);

	AddMoneyToCharacter(mc, 10000000);
	AddItems(mc, "spyglass5", 1);
	AddItems(mc, "pistol5", 1);
	AddItems(mc, "Mushket", 1);
	AddItems(mc, "BackPack5", 1);


	sEquipItem = GetGeneratedItem("blade32");
	AddItems(mc, sEquipItem, 1);

	AddItems(mc, "bullet", 50);
	AddItems(mc, "GunPowder", 50);
	AddItems(mc, "Map_Best", 1);
	AddItems(mc, "cirass5", 1);

	EquipCharacterbyItem(mc, "spyglass5");
	EquipCharacterbyItem(mc, "pistol5");
 	EquipCharacterbyItem(mc, sEquipItem);
 	EquipCharacterbyItem(mc, "cirass5");
	EquipCharacterbyItem(mc, "BackPack5");

 	SelAllPerksToChar(mc, false);

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_59", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F31", 1);
}

void CalculateInfoDataF32()
{
	ref mc;
	string sEquipItem;
	totalInfo = descF32;
	mc = GetMainCharacter();
	SetRandSelfSkill(mc, 40, 75);
	SetRandShipSkill(mc, 40, 75);
	mc.rank = 10;
	mc.Ship.Type = GenerateShipExt(SHIP_BRIGSW, true, mc);
	SetBaseShipData(mc);
	mc.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS24;
	SetCrewQuantityFull(mc);
	AddCrewMorale(mc, 80);
	ChangeCrewExp(mc, "Sailors", 50);
	ChangeCrewExp(mc, "Cannoners", 50);
	ChangeCrewExp(mc, "Soldiers", 50);

	SetCharacterGoods(mc, GOOD_BALLS, 700);
	SetCharacterGoods(mc, GOOD_GRAPES, 700);
	SetCharacterGoods(mc, GOOD_KNIPPELS, 700);
	SetCharacterGoods(mc, GOOD_BOMBS, 700);;
	SetCharacterGoods(mc, GOOD_FOOD, 3000);
	SetCharacterGoods(mc, GOOD_MEDICAMENT, 500);
	SetCharacterGoods(mc, GOOD_POWDER, 2000);
	SetCharacterGoods(mc, GOOD_WEAPON, 500);

	sEquipItem = GetCharacterEquipByGroup(mc, BLADE_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, GUN_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, SPYGLASS_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);
	sEquipItem = GetCharacterEquipByGroup(mc, CIRASS_ITEM_TYPE);
	RemoveItems(mc, sEquipItem, 1);

	AddMoneyToCharacter(mc, 1000000);
	AddItems(mc, "spyglass3", 1);
	AddItems(mc, "pistol5", 1);

	sEquipItem = GetGeneratedItem("blade27");
	AddItems(mc, sEquipItem, 1);

	AddItems(mc, "bullet", 50);
	AddItems(mc, "GunPowder", 50);
	AddItems(mc, "cirass4", 1);
	AddItems(mc, "talisman1", 1);
	AddItems(mc, "talisman2", 1);
	AddItems(mc, "talisman3", 1);
	AddItems(mc, "talisman4", 1);
	AddItems(mc, "talisman6", 1);
	AddItems(mc, "talisman7", 1);
	AddItems(mc, "talisman8", 1);

	EquipCharacterbyItem(mc, "spyglass3");
	EquipCharacterbyItem(mc, "pistol5");
 	EquipCharacterbyItem(mc, sEquipItem);
 	EquipCharacterbyItem(mc, "cirass4");

 	SetHalfPerksToChar(mc, false);

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_61", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F32", 1);
}

void CalculateInfoDataF33()
{
 	ref mc;
	totalInfo = descF33;
	mc = GetMainCharacter();
 	mc.perks.FreePoints_self = sti(mc.perks.FreePoints_self) + 1;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_62", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F33", 1);
}

void CalculateInfoDataF34()
{
 	ref mc;
	totalInfo = descF34;
	mc = GetMainCharacter();
 	mc.perks.FreePoints_ship = sti(mc.perks.FreePoints_ship) + 1;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_63", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F34", 1);
}

void CalculateInfoDataF35()
{
 	ref mc;
	totalInfo = descF35;
	mc = GetMainCharacter();
 	mc.rank_exp = 99;
	ApplayNewSkill(mc, "", 1);

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_64", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F35", 1);
}

void CalculateInfoDataF36()
{
	totalInfo = descF36;
 	LoginDummy();
	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_66", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F36", 1);
}

void CalculateInfoDataF37()
{
	CreateSmallColonyCaiman();
	PChar.ColonyBuilding.Stage.FirstStage = "3";
	Characters[GetCharacterIndex("Builder")].Dialog.CurrentNode = "ColonyBuilding_7";
	PlayStereoSound("notebook");
	Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_68", "Interface.txt", "#space#", " "));
	LAi_SetCitizenType(CharacterFromID("Builder"));
	LAi_group_MoveCharacter(&Characters[GetCharacterIndex("Builder")], LAI_GROUP_PLAYER);
	ChangeCharacterAddressGroup(CharacterFromID("Builder"), "Shore17", "goto", "goto13");
	PChar.quest.ColonyBuilding_2_1.win_condition.l1 = "location";
	PChar.quest.ColonyBuilding_2_1.win_condition.l1.location = "Shore17";
	PChar.quest.ColonyBuilding_2_1.win_condition = "ColonyBuilding_2_1";
	Pchar.GenQuest.Hunter2Pause = true;

	DoQuestReloadToLocation("Shore17","reload","reload1", "");
	setCharacterShipLocation(pchar, "Shore17");
	setWDMPointXZ("Shore17");

	totalInfo = descF37;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_69", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F37", 1);
}

void CalculateInfoDataF38()
{
	PChar.ColonyBuilding.Stage.SecondStage = "3";
	LAi_SetCitizenType(CharacterFromID("Builder"));
	LAi_group_MoveCharacter(&Characters[GetCharacterIndex("Builder")], LAI_GROUP_PLAYER);
	Characters[GetCharacterIndex("Builder")].Dialog.CurrentNode = "ColonyModification_4";
	ChangeCharacterAddressGroup(CharacterFromID("Builder"), "Caiman_town", "goto", "goto8");
	LAi_SetActorType(CharacterFromID("Builder"));
	LAi_ActorDialog(CharacterFromID("Builder"), PChar, "", 15.0, 1.0);
	PlayStereoSound("notebook");
	Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_70", "Interface.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Debuger_71", "Interface.txt", "#space#", " "));
	CreateModificyColonyCaiman();
	LAi_QuestDelay("LiveOfTheColonyUptade_1", 0.5);
	LAi_QuestDelay("CheckDefenceColonyUptade_1", 0.5);
	Pchar.GenQuest.Hunter2Pause = true;
	
	DoQuestReloadToLocation("Caiman_town","reload","reload1", "");
	setCharacterShipLocation(pchar, "Caiman_town");
	setWDMPointXZ("Caiman_town");

	PChar.quest.Store_build_1.win_condition.l1 = "location";
	PChar.quest.Store_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.Store_build_1.win_condition = "Store_build_1";
	PChar.quest.Shipyard_build_1.win_condition.l1 = "location";
	PChar.quest.Shipyard_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.Shipyard_build_1.win_condition = "Shipyard_build_1";
	PChar.quest.Tavern_build_1.win_condition.l1 = "location";
	PChar.quest.Tavern_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.Tavern_build_1.win_condition = "Tavern_build_1";
	PChar.quest.Church_build_1.win_condition.l1 = "location";
	PChar.quest.Church_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.Church_build_1.win_condition = "Church_build_1";
	PChar.quest.Bank_build_1.win_condition.l1 = "location";
	PChar.quest.Bank_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.Bank_build_1.win_condition = "Bank_build_1";
	PChar.quest.HeadPort_build_1.win_condition.l1 = "location";
	PChar.quest.HeadPort_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.HeadPort_build_1.win_condition = "HeadPort_build_1";
	PChar.quest.StoreHouse_build_1.win_condition.l1 = "location";
	PChar.quest.StoreHouse_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.StoreHouse_build_1.win_condition = "StoreHouse_build_1";
	PChar.quest.StoreHouse_resize_1.win_condition.l1 = "location";
	PChar.quest.StoreHouse_resize_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.StoreHouse_resize_1.win_condition = "StoreHouse_resize_1";

	PChar.quest.Expidition_build_1.win_condition.l1 = "location";
	PChar.quest.Expidition_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.Expidition_build_1.win_condition = "Expidition_build_1";

	PChar.quest.Plantation_build_1.win_condition.l1 = "location";
	PChar.quest.Plantation_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.Plantation_build_1.win_condition = "Plantation_build_1";
	PChar.quest.Plantation_resize_1.win_condition.l1 = "location";
	PChar.quest.Plantation_resize_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.Plantation_resize_1.win_condition = "Plantation_resize_1";

	PChar.quest.GoldMine_build_1.win_condition.l1 = "location";
	PChar.quest.GoldMine_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.GoldMine_build_1.win_condition = "GoldMine_build_1";
	PChar.quest.SilverMine_build_1.win_condition.l1 = "location";
	PChar.quest.SilverMine_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.SilverMine_build_1.win_condition = "SilverMine_build_1";
	PChar.quest.IronMine_build_1.win_condition.l1 = "location";
	PChar.quest.IronMine_build_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.IronMine_build_1.win_condition = "IronMine_build_1";
	PChar.quest.GoldMine_resize_1.win_condition.l1 = "location";
	PChar.quest.GoldMine_resize_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.GoldMine_resize_1.win_condition = "GoldMine_resize_1";
	PChar.quest.SilverMine_resize_1.win_condition.l1 = "location";
	PChar.quest.SilverMine_resize_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.SilverMine_resize_1.win_condition = "SilverMine_resize_1";
	PChar.quest.IronMine_resize_1.win_condition.l1 = "location";
	PChar.quest.IronMine_resize_1.win_condition.l1.location = "Caiman_town";
	PChar.quest.IronMine_resize_1.win_condition = "IronMine_resize_1";

	totalInfo = descF38;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_72", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F38", 1);
}

void CalculateInfoDataF39()
{
	PChar.ColonyBuilding.Stage.SecondStage = "3";
	LAi_SetCitizenType(CharacterFromID("Builder"));
	LAi_group_MoveCharacter(&Characters[GetCharacterIndex("Builder")], LAI_GROUP_PLAYER);
	Characters[GetCharacterIndex("Builder")].Dialog.CurrentNode = "ColonyFortBuilding_6";
	ChangeCharacterAddressGroup(CharacterFromID("Builder"), "LandGuardingPort", "goto", "goto1");
	LAi_SetActorType(CharacterFromID("Builder"));
	LAi_ActorDialog(CharacterFromID("Builder"), PChar, "", 15.0, 1.0);
	PlayStereoSound("notebook");
	Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_74", "Interface.txt", "#space#", " ") + PChar.ColonyBuilding.ColonyName + GetConvertStrWithReplace("Variable_Debuger_75", "Interface.txt", "#space#", " "));
	CreateModificyColonyCaiman();
	LAi_QuestDelay("LiveOfTheColonyUptade_1", 0.5);
	LAi_QuestDelay("CheckDefenceColonyUptade_1", 0.5);
	Pchar.GenQuest.Hunter2Pause = true;
	
	DoQuestReloadToLocation("LandGuardingPort","reload","reload1", "");
	setCharacterShipLocation(pchar, "Caiman_town");
	setWDMPointXZ("Caiman_town");

	PChar.quest.Store_build_1.win_condition.l1 = "location";
	PChar.quest.Store_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.Store_build_1.win_condition = "Store_build_1";
	PChar.quest.Shipyard_build_1.win_condition.l1 = "location";
	PChar.quest.Shipyard_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.Shipyard_build_1.win_condition = "Shipyard_build_1";
	PChar.quest.Tavern_build_1.win_condition.l1 = "location";
	PChar.quest.Tavern_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.Tavern_build_1.win_condition = "Tavern_build_1";
	PChar.quest.Church_build_1.win_condition.l1 = "location";
	PChar.quest.Church_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.Church_build_1.win_condition = "Church_build_1";
	PChar.quest.Bank_build_1.win_condition.l1 = "location";
	PChar.quest.Bank_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.Bank_build_1.win_condition = "Bank_build_1";
	PChar.quest.HeadPort_build_1.win_condition.l1 = "location";
	PChar.quest.HeadPort_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.HeadPort_build_1.win_condition = "HeadPort_build_1";
	PChar.quest.StoreHouse_build_1.win_condition.l1 = "location";
	PChar.quest.StoreHouse_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.StoreHouse_build_1.win_condition = "StoreHouse_build_1";
	PChar.quest.StoreHouse_resize_1.win_condition.l1 = "location";
	PChar.quest.StoreHouse_resize_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.StoreHouse_resize_1.win_condition = "StoreHouse_resize_1";

	PChar.quest.Expidition_build_1.win_condition.l1 = "location";
	PChar.quest.Expidition_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.Expidition_build_1.win_condition = "Expidition_build_1";

	PChar.quest.Plantation_build_1.win_condition.l1 = "location";
	PChar.quest.Plantation_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.Plantation_build_1.win_condition = "Plantation_build_1";
	PChar.quest.Plantation_resize_1.win_condition.l1 = "location";
	PChar.quest.Plantation_resize_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.Plantation_resize_1.win_condition = "Plantation_resize_1";

	PChar.quest.GoldMine_build_1.win_condition.l1 = "location";
	PChar.quest.GoldMine_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.GoldMine_build_1.win_condition = "GoldMine_build_1";
	PChar.quest.SilverMine_build_1.win_condition.l1 = "location";
	PChar.quest.SilverMine_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.SilverMine_build_1.win_condition = "SilverMine_build_1";
	PChar.quest.IronMine_build_1.win_condition.l1 = "location";
	PChar.quest.IronMine_build_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.IronMine_build_1.win_condition = "IronMine_build_1";
	PChar.quest.GoldMine_resize_1.win_condition.l1 = "location";
	PChar.quest.GoldMine_resize_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.GoldMine_resize_1.win_condition = "GoldMine_resize_1";
	PChar.quest.SilverMine_resize_1.win_condition.l1 = "location";
	PChar.quest.SilverMine_resize_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.SilverMine_resize_1.win_condition = "SilverMine_resize_1";
	PChar.quest.IronMine_resize_1.win_condition.l1 = "location";
	PChar.quest.IronMine_resize_1.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.IronMine_resize_1.win_condition = "IronMine_resize_1";

	PChar.quest.ColonyFortBuilding_6.win_condition.l1 = "location";
	PChar.quest.ColonyFortBuilding_6.win_condition.l1.location = "LandGuardingPort";
	PChar.quest.ColonyFortBuilding_6.win_condition = "ColonyFortBuilding_6";

	totalInfo = descF39;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_76", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F39", 1);
}

void CalculateInfoDataF40()
{
	totalInfo = descF40;
 	ref mc;
	mc = GetMainCharacter();
	mc.Ship.Type = GenerateShipExt(SHIP_XebekVML, true, mc);
	SetBaseShipData(mc);
	int iShip = pchar.ship.type;
	mc.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS24;
	SetCrewQuantityFull(mc);
	AddCrewMorale(mc, 100);
	ChangeCrewExp(mc, "Sailors", 100);
	ChangeCrewExp(mc, "Cannoners", 100);
	ChangeCrewExp(mc, "Soldiers", 100);

	SetCharacterGoods(mc, GOOD_BALLS, 1000);
	SetCharacterGoods(mc, GOOD_GRAPES, 1000);
	SetCharacterGoods(mc, GOOD_KNIPPELS, 1000);
	SetCharacterGoods(mc, GOOD_BOMBS, 1000);;
	SetCharacterGoods(mc, GOOD_FOOD, 2000);
	SetCharacterGoods(mc, GOOD_MEDICAMENT, 1000);
	SetCharacterGoods(mc, GOOD_POWDER, 2000);
	SetCharacterGoods(mc, GOOD_WEAPON, 300);
	ref refBaseShip = GetRealShip(iShip);
	refBaseShip.WindAgainstSpeed = 30.0;
	refBaseShip.SpeedRate = 200.0;
	refBaseShip.TurnRate = 90.0;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_78", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F40", 1);
}

void CalculateInfoDataF41()
{
	totalInfo = descF41;
	int iChar;

	string sColony;

	int i;
	float x, y, z;  // boal

	for (i=0; i<MAX_COLONIES; i++)
	{
		if (colonies[i].nation == "none") continue; // необитайки
		// зададим базовых мэров городов
		iChar = GetCharacterIndex(colonies[i].id + "_Mayor");
		if (iChar != -1)
		{   // мэр есть
        	Log_Info("M: " + characters[iChar].id + " L:" + characters[iChar].location +
        	" " + characters[iChar].City + "  " + characters[iChar].nation);
        	trace("M: " + characters[iChar].id + " L:" + characters[iChar].location +
        	" " + characters[iChar].City + "  " + characters[iChar].nation);
        }

		// добавить проверку на пиратов, у них нет фортов, нафиг им коммандер?
		if (CheckAttribute(&colonies[i], "HasNoFort"))
		{
			continue;
		}
		iChar = GetCharacterIndex(colonies[i].id + " Fort Commander");
		Log_Info("F: " + characters[iChar].id + " L:" + characters[iChar].location + " g " + characters[iChar].location.group + " r " + characters[iChar].location.locator +
  		" " + characters[iChar].City + "  " + characters[iChar].nation);
  		trace("F: " + characters[iChar].id + " L:" + characters[iChar].location + " g " + characters[iChar].location.group + " r " + characters[iChar].location.locator +
  		" " + characters[iChar].City + "  " + characters[iChar].nation);
	}

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_80", "Interface.txt", "#space#", " ");
	SetFormatedText("INFO_TEXT", totalInfo);
	Statistic_AddValue(PChar, "Cheats.F40", 1);
}

void CalculateInfoDataF42()
{
	totalInfo = descF42;
	int i, j, idx;
	float fMaxCost;

	for (i = 0; i< GOODS_QUANTITY; i++)
	{
		SetCharacterGoods(pchar, i, sti(Goods[i].Units));
	}
	trace("======= TestGoodsLiquidity ======== start ");
	for (j = 0; j< GOODS_QUANTITY; j++)
	{
		fMaxCost = 0;
		idx = -1;
		for (i = 0; i< GOODS_QUANTITY; i++)
		{
			if (GetCargoGoods(pchar, i) > 0)
			{
				if (fMaxCost < stf(Goods[i].Cost)/stf(Goods[i].Weight))
				{
					fMaxCost = stf(Goods[i].Cost)/stf(Goods[i].Weight);
					idx = i;
				}
			}
		}
		if (fMaxCost > 0)
		{
			SetCharacterGoods(pchar, idx, 0);
			trace(Goods[idx].Name + " = " + fMaxCost);
			Log_info(Goods[idx].Name + " = " + fMaxCost);
		}
	}
	trace("======= TestGoodsLiquidity ======== end ");

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_82", "Interface.txt", "#space#", " ");
	SetFormatedText("INFO_TEXT", totalInfo);
	Statistic_AddValue(PChar, "Cheats.F40", 1);
}

void CalculateInfoDataF43()
{
	totalInfo = descF43;

	Log_TestInfo("start GoldFleet");
	AddTemplRumour("Start_GoldFleet", id_counter+1);
	string sQuest = "EndOfGoldFleet";
	SetTimerCondition(sQuest, 0, 0, 29, false);
	pchar.quest.(sQuest).win_condition = "EndOfGoldFleet";
	pchar.quest.(sQuest).function= "EndOfGoldFleet";
	GoldFleet();
	int pbx = worldMap.islands.PortoBello.PortoBello_town.position.x;
	int pbz = worldMap.islands.PortoBello.PortoBello_town.position.z;
	Map_CreateTraderXZ(pbx, pbz, -858.089, 897.072, "Head_of_Gold_Squadron", 22);

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_84", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F43", 1);
}

void CalculateInfoDataF44()
{
	totalInfo = descF44;

	totalInfo = totalInfo + NewStr() +  + "pchar.Ship.Ang.y _ " + stf(pchar.Ship.Ang.y) + NewStr() + "pchar.Ship.Pos.x _ " + stf(pchar.Ship.Pos.x) + NewStr() + "pchar.Ship.Pos.z _ " + stf(pchar.Ship.Pos.z) + NewStr() + GetConvertStrWithReplace("Variable_Debuger_86", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	trace("reload,reload_fort1," + cos(-stf(pchar.Ship.Ang.y)) + ",0,"	+ sin(-stf(pchar.Ship.Ang.y)) + ",0,1,0," + sin(stf(pchar.Ship.Ang.y)) + ",0," + cos(stf(pchar.Ship.Ang.y)) + "," + stf(pchar.Ship.Pos.x) + ",1.364242E-12," + stf(pchar.Ship.Pos.z) + ",1,0,0,0,0,0,0,0,0");

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F44", 1);
}

void CalculateInfoDataF45()
{
	totalInfo = descF45;

    InitItems();
    initGoods();
    ShipsInit();
    IslandsInit();
    LocationInit();
    ColoniesInit();
    CreateColonyCommanders();

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_88", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F45", 1);
}

void CalculateInfoDataF46()
{
	float locx0, locy0, locz0, locAy0;
	GetCharacterPos(pchar, &locx0, &locy0, &locz0);
	GetCharacterAy(pchar, &locAy0);

	totalInfo = descF46;

 	totalInfo = totalInfo + "locX: " + locx0 + " | locY: " + locy0 + " | locZ: " + locz0 + " | locAy: " + locAy0 + GetConvertStrWithReplace("Variable_Debuger_90", "Interface.txt", "#space#", " ");

	trace("reload,reload1," + cos(-stf(locAy0)) + ",0," + sin(-stf(locAy0)) + ",0,1,0," + sin(stf(locAy0)) + ",0," + cos(stf(locAy0)) + "," + locx0 + "," + locy0 + "," + locz0 + ",1,0,0,0,0,0,0,0,0,");
	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F46", 1);
}

void CalculateInfoDataF47()
{
	totalInfo = descF47;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_92", "Interface.txt", "#space#", " ");

	AddItems(pchar, "chest_ammo", 5);
	AddItems(pchar, "chest_treasure", 5);
	AddItems(pchar, "chest_quest", 5);
	AddItems(pchar, "chest_craftsmans", 5);
	AddItems(pchar, "lockpick", 20);

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F47", 1);
}

void CalculateInfoDataF48()
{
	totalInfo = descF48;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_94", "Interface.txt", "#space#", " ");

	float locx, locy, locz;
	aref aroot, arcur;
	string attrname;
	makearef(aroot,loadedLocation.reload);
	int num = GetAttributesNum(aroot);
	for(int i=0; i<num; i++)
	{
		arcur = GetAttributeN(aroot,i);
		attrname = GetAttributeName(arcur);
		DeleteAttribute(loadedLocation, "reload."+attrname+".disable");
	}

	chrDisableReloadToLocation = false;
	InterfaceStates.DisFastTravel = false
	bDisableCharacterMenu = false;
	bDisableFastReload = false;

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F48", 1);
}

void CalculateInfoDataF49()
{
	totalInfo = descF49;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_96", "Interface.txt", "#space#", " ");

	pchar.GiantEvilSkeleton = true;
	pchar.MalteseInfo = true;
	pchar.MalteseOrderOnHold = true;
	pchar.MalteseOrder = true;
	BuildMalteseOrder("");
	DoQuestReloadToLocation("Reefs_Chapter","reload","reload1","");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F49", 1);
}

void CalculateInfoDataF50()
{
	totalInfo = descF50;

	SetArraySize(&rand_arr,5); //должно быть равно самой большой цифре индекса массива + 1
	rand_arr[0] = GetConvertStrWithReplace("Variable_Debuger_97", "Interface.txt", "#space#", " ");
	rand_arr[1] = GetConvertStrWithReplace("Variable_Debuger_98", "Interface.txt", "#space#", " ");
	rand_arr[2] = GetConvertStrWithReplace("Variable_Debuger_99", "Interface.txt", "#space#", " ");
	rand_arr[3] = GetConvertStrWithReplace("Variable_Debuger_100", "Interface.txt", "#space#", " ");
	rand_arr[4] = GetConvertStrWithReplace("Variable_Debuger_101", "Interface.txt", "#space#", " ");
	string result = LinkRandArrayPhrase(4); //должно быть равно самой большой цифре индекса массива

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_102", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo+"\n"+result);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F50", 1);
}

void CalculateInfoDataF51()
{
	totalInfo = descF51;
	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_104", "Interface.txt", "#space#", " ");
	int ass = 0;
	for (i=0; i<TOTAL_CHARACTERS; i++)
	{
		ref chref;
		makeref(chref, Characters[i]);
		if (CheckAttribute(chref, "location") && chref.location == pchar.location)
		{
			ass++;
		}
	}

	SetFormatedText("INFO_TEXT", totalInfo+"\nКоличество персонажей в локации: "+ass);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F51", 1);
}

void CalculateInfoDataF52()
{
	totalInfo = descF52;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_106", "Interface.txt", "#space#", " ");
	
	DumpAttributes(loadedLocation);

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F52", 1);
}

void CalculateInfoDataF53()
{
	totalInfo = descF53;
	if (checkattribute(pchar, "cheatGlobalMapView"))
			{deleteattribute(pchar, "cheatGlobalMapView"); totalInfo += ". => ВЫКЛЮЧЕНО";}
		else {pchar.cheatGlobalMapView = true; totalInfo += ". => ВКЛЮЧЕНО";}
	ApplayNewSkill(pchar, "", 0);
	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_107", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F53", 1);
}

void CalculateInfoDataF54()
{
	totalInfo = descF54;

	sld = GetCharacter(NPC_GenerateCharacter("LoL", "ozg_green", "man", "man", 5, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 5, 30, 30, "blade30", "pistol2", 5);
	FantomMakeCoolSailor(sld, SHIP_POSEIDON, GetConvertStrWithReplace("Variable_Debuger_109", "Interface.txt", "#space#", " "), CANNON_TYPE_CANNON_LBS42, 50, 60, 70); //КОРАБЛЬ

	sld.ship.Crew.Morale = 50;								//Мораль
	ChangeCrewExp(sld, "Sailors", 50);						//Матросы
	ChangeCrewExp(sld, "Cannoners", 60);					//Канониры
	ChangeCrewExp(sld, "Soldiers", 70);						//Солдаты
	sld.DontRansackCaptain = true; 							//Квестовые не сдаются
	sld.AlwaysSandbankManeuver = true;
	sld.AlwaysEnemy = true;
	Group_FindOrCreateGroup("Test_Attack");
	Group_SetType("Test_Attack", "war");
	Group_AddCharacter("Test_Attack", "LoL");

	Group_SetGroupCommander("Test_Attack", "LoL");
	Group_SetTaskAttack("Test_Attack", PLAYER_GROUP);
	Group_SetAddress("Test_Attack", "Reefs", "quest_ships", "Quest_ship_3");
	Group_LockTask("Test_Attack");
	QuestToSeaLogin_Launch();
	QuestToSeaLogin_PrepareLoc("Reefs", "Quest_Ships", "Quest_ship_1", true);

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_129", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F54", 1);
}

void CalculateInfoDataF55()
{
	totalInfo = descF55;

	TakeNItems(PChar, "jewelry11", 50);
	TakeNItems(PChar, "chest", 12);

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_131", "Interface.txt", "#space#", " ");
	
	for (int z = 0; z < ITEMS_QUANTITY;z++)
	{
		if (CheckAttribute(Items[z],"id") && HasSubStr(Items[z].id,"map_"))
		{
			GiveItem2Character(pchar,Items[z].id);
		}
	}
	RefreshEquippedMaps(GetMainCharacter());

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F55", 1);
}

void CalculateInfoDataF56()
{
	totalInfo = descF56;

	ref mc = GetMainCharacter();
	mc.UnlimitedWeight = true;
	for (int i = 0; i < ITEMS_QUANTITY; i++) 
	{
		ref itm = &Items[i];

		if (!CheckAttribute(itm, "id")) continue;
		if (!CheckAttribute(itm, "Weight")) continue;
		if (CheckAttribute(itm, "ItemType")) {
			if (itm.ItemType == "QUESTITEMS") continue;
		}

		AddItems(mc, itm.id, 1);
	}

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_133", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F56", 1);
}

void CalculateInfoDataF57()
{
	totalInfo = descF57;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_135", "Interface.txt", "#space#", " ");
	
	int nation1, nation2, i, j;
	for (i = 0; i <= 4; i++)
	{
		totalInfo = totalInfo + NewStr() + GetConvertStrWithReplace("Variable_check_relations_1", "Interface.txt", "#space#", " ") + GetNationNameByType(i) + GetConvertStrWithReplace("Variable_check_relations_2", "Interface.txt", "#space#", " ") + GetRelationName(GetNationRelation2MainCharacter(i));
		totalInfo = totalInfo + NewStr() + GetConvertStrWithReplace("Variable_check_relations_3", "Interface.txt", "#space#", " ") + GetNationNameByType(i) + GetConvertStrWithReplace("Variable_check_relations_2", "Interface.txt", "#space#", " ") + GetRelationName(GetRelation2BaseNation(i));
		
		for (j = 0; j <= 4; j++)
		{
			if (i != j)	totalInfo = totalInfo + NewStr() + GetConvertStrWithReplace("Variable_check_relations_4", "Interface.txt", "#space#", " ") + GetNationNameByType(i) + GetConvertStrWithReplace("Variable_NationLegend_1", "Interface.txt", "#space#", " ") + GetNationNameByType(j) + GetConvertStrWithReplace("Variable_check_relations_2", "Interface.txt", "#space#", " ") + GetRelationName(GetNationRelation(i,j));
		}
	}
	SetNationRelations();

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F57", 1);
}

	// Десять пленных для проверки бунта.
void CalculateInfoDataF58()
{
	int i, idx;
	ref sld;
	for (i = 0; i < 10; i++)
	{
		idx = NPC_GenerateCharacter("DbgPrisoner" + i, "off_" + NationShortName(rand(3)) + "_2", "man", "man", 10, rand(3), 0, true);
		if (idx < 0) continue;
		sld = GetCharacter(idx);
		sld.ship.type = SHIP_NOTUSED;
		SetCharToPrisoner(sld);   // копирует в новый слот пленника (removable + в пассажиры)
		sld.LifeDay = 0;          // временный донор-шаблон уберётся при чистке фантомов
	}
	totalInfo = "Создано 10 пленников. Всего в трюме: " + GetPrisonerQty();
	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F58", 1);
}

void CalculateInfoDataF59()
{
	// Gregg: remove ALL (non-hidden) perks from the main character and refund ability points
	ref chr = GetMainCharacter();

	aref arList; makearef(arList, ChrPerksList.list);
	int q = GetAttributesNum(arList);
	string pname, bt;
	int refunded = 0;
	int i;
	for (i = 0; i < q; i++)
	{
		pname = GetAttributeName(GetAttributeN(arList, i));
		if (CheckAttribute(&ChrPerksList, "list." + pname + ".Hiden")) continue;
		if (!CheckCharacterPerk(chr, pname)) continue;
		bt = "self";
		if (CheckAttribute(&ChrPerksList, "list." + pname + ".BaseType")) bt = ChrPerksList.list.(pname).BaseType;
		if (bt == "ship") chr.perks.FreePoints_ship = sti(chr.perks.FreePoints_ship) + 1;
		else              chr.perks.FreePoints_self = sti(chr.perks.FreePoints_self) + 1;
		DeleteAttribute(chr, "perks.list." + pname);
		refunded++;
	}

	ClearActiveChrPerks(chr);
	Event("eSwitchPerks", "l", GetMainCharacterIndex());

	totalInfo = descF59 + NewStr() + NewStr() + GetConvertStrWithReplace("DbgPerksRemoved", "Interface.txt", "#space#", " ") + refunded;
	SetFormatedText("INFO_TEXT", totalInfo);

	Statistic_AddValue(PChar, "Cheats.F59", 1);
}

	// Семь укомплектованных капитанов для проверки штурма форта.
void CalculateInfoDataF60()
{
	ref mc = GetMainCharacter();
	ref sld;
	int i;
	int aShips[7];
	aShips[0] = SHIP_MANOWAR;
	aShips[1] = SHIP_VICTORY;
	aShips[2] = SHIP_SANFELIPE;
	aShips[3] = SHIP_SOLEY;
	aShips[4] = SHIP_OCEAN;
	aShips[5] = SHIP_SANTAANNA;
	aShips[6] = SHIP_MANOWAR;

	for (i = 0; i < 7; i++)
	{
		// модель берём штатным подборщиком — он сам разбирается с нациями и пиратами
		sld = GetCharacter(NPC_GenerateCharacter("CheatSiegeComp_" + i,
			GetCaptainModelByNation(sti(mc.nation), "war"),
			"man", "man", 60, sti(mc.nation), -1, true));
		FantomMakeCoolSailor(sld, aShips[i], "", CANNON_TYPE_CULVERINE_LBS32, 100, 100, 100);
		FantomMakeCoolFighter(sld, 100, 100, 100, "blade28", "pistol5", 50000);
		// Тестовые капитаны должны сдаваться, тонуть и уходить без квестовой защиты.
		DeleteAttribute(sld, "DontRansackCaptain");
		DeleteAttribute(sld, "AboardToFinalDeck");
		DeleteAttribute(sld, "DontClearDead");
		DeleteAttribute(sld, "SaveItemsForDead");
		DeleteAttribute(sld, "AlwaysSandbankManeuver");
		sld.SinkTenPercent = true;
		// навыки и ВСЕ перки — штатными хелперами, как на кнопке 31
		SetRandSelfSkill(sld, 100, 100);
		SetRandShipSkill(sld, 100, 100);
		SelAllPerksToChar(sld, false);
		SetCrewQuantityFull(sld);                   // ещё раз: перки могли поднять потолок команды
		AddCrewMorale(sld, 100);
		ChangeCrewExp(sld, "Sailors", 100);
		ChangeCrewExp(sld, "Cannoners", 100);
		ChangeCrewExp(sld, "Soldiers", 100);

		SetCharacterGoods(sld, GOOD_MEDICAMENT, 3000);  // раненых на штурме много, лечение упирается в них
		SetCharacterGoods(sld, GOOD_BALLS,      3000);
		SetCharacterGoods(sld, GOOD_GRAPES,     2000);
		SetCharacterGoods(sld, GOOD_KNIPPELS,   2000);
		SetCharacterGoods(sld, GOOD_BOMBS,      2000);
		SetCharacterGoods(sld, GOOD_POWDER,     5000);
		SetCharacterGoods(sld, GOOD_FOOD,       5000);
		SetCharacterGoods(sld, GOOD_RUM,        1000);
		SetCharacterGoods(sld, GOOD_WEAPON,      500);
		SetCharacterGoods(sld, GOOD_PLANKS,      500);
		SetCharacterGoods(sld, GOOD_SAILCLOTH,   500);

		sld.CompanionEnemyEnable = false;
		SetCompanionIndex(mc, -1, sti(sld.index));
	}

	totalInfo = descF60 + NewStr() + NewStr() + "Готово: 7 компаньонов в эскадре.";
	SetFormatedText("INFO_TEXT", totalInfo);
	Statistic_AddValue(PChar, "Cheats.F60", 1);
}

void CalculateInfoDataF61()
{
	totalInfo = descF61;
	ref mc;
	mc = GetMainCharacter();

	float locx, locy, locz, locay;
	if (bSeaActive && !bAbordageStarted)
	{
	    if (CheckAttribute(mc, "Ship.pos.x"))
		{
		    totalInfo = totalInfo + NewStr() + mc.location + "     ship(x,z)= "+mc.Ship.pos.x +", "+mc.Ship.pos.z;
			totalInfo = totalInfo + NewStr() + " Sea_Coord.X " + Sea_GetRealCoordX(makefloat(mc.Ship.pos.x)) + " Sea_Coord.Z " + Sea_GetRealCoordZ(makefloat(mc.Ship.pos.z));
		    if (false)
	        {
	            string sTemp = mc.curIslandId;
	            float r1 = stf(worldMap.islands.(sTemp).GoMapRadius);

				r1 *= WDM_MAP_TO_SEA_SCALE;
	            float d2 = GetDistance2D(stf(mc.Ship.Pos.x),
	                              stf(mc.Ship.Pos.z),
	                              stf(worldMap.islands.(sTemp).position.rx),
	                              stf(worldMap.islands.(sTemp).position.rz));

	            totalInfo = totalInfo + "         Координаты для карты radius= " + r1 + "   GetDistance2D= " + d2 + "      ";
	        }
		}
	}
	else
	{
  		if (IsEntity(loadedLocation))
  		{
  		    float xx,zz,xz,zx;

			GetCharacterPos(GetMainCharacter(), &locx, &locy, &locz);
			GetCharacterAy(GetMainCharacter(), &locay);
			xx = -sin(locay - PId2);
            zz = xx;
            xz = -cos(locay - PId2);
            zx = -xz;
			totalInfo = totalInfo + "id = " + mc.location + " (x,y,z, xx,zz)= "+locx + ", " +locy + ", "+locz + ", "+xx + ", "+xz;
			totalInfo = totalInfo + NewStr() + "filespath.models = " + loadedLocation.filespath.models;
			totalInfo = totalInfo + NewStr() + "image = " + loadedLocation.image;
			totalInfo = totalInfo + NewStr() + "models.locators = " + loadedLocation.models.always.locators;

			if(CheckAttribute(LoadedLocation, "islandId"))
			{
				totalInfo = totalInfo + NewStr() + "islandId = " + LoadedLocation.islandId;
			}

			if(CheckAttribute(LoadedLocation, "townsack"))
			{
				totalInfo = totalInfo + NewStr() + "townsack = " + LoadedLocation.townsack;
			}
		}
	}
	totalInfo = totalInfo + NewStr() + " MapShipX " +	worldMap.playerShipX + " MapShipZ " + worldMap.playerShipZ + " X " + worldMap.island.x + " Z " + worldMap.island.z;

	totalInfo = totalInfo + NewStr() + " Map_Coord.X " + Map_GetRealCoordX(makefloat(worldMap.playerShipX)) + " Map_Coord.Z " + Map_GetRealCoordZ(makefloat(worldMap.playerShipZ));

	totalInfo = totalInfo + NewStr() + " Zero_Point_X " + makefloat(worldMap.zeroX) + " Zero_Point_Z " + makefloat(worldMap.zeroZ);
    totalInfo = totalInfo + NewStr() + NewStr() +
                GetConvertStrWithReplace("Variable_Debuger_5", "Interface.txt", "#space#", " ");
    SetFormatedText("INFO_TEXT",totalInfo);

	GetRealCoordsObjects();

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F61", 1);
}

	// Ищем владельцев слотов и крупные деревья: рост MAX_CHARACTERS удорожает ежедневную чистку.
void CalculateInfoDataF62()
{
	ref chref;
	int i;
	int iBusy, iDead, iRebirth, iLife, iForever, iComp;
	int iZone;
	int j, k, n, iWorst;
	int iFvGen, iFvQuest, iFvOfficer, iFvCitizen, iFvPGG, iFvOther;
	int iDeadStuck;
	int iFvQuestWin;
	string sKind;
	string sQuestSample;
	string sHeavyBranch;
	string sQtBranch;
	aref arBr;
	aref arSub;
	int iBrQty, iBrSub, iBrWorst, iBrMax;
	// Имена Storm нечувствительны к регистру; не пересекаемся с нативными функциями.
	string sChrId;
	int aBusy[5];
	int aForever[5];
	int iTopIdx[8];
	int iTopNum[8];

	totalInfo = descF62;
	iBusy = 0; iDead = 0; iRebirth = 0; iLife = 0; iForever = 0; iComp = 0;
	iFvGen = 0; iFvQuest = 0; iFvOfficer = 0; iFvCitizen = 0; iFvPGG = 0; iFvOther = 0;
	iDeadStuck = 0;
	iFvQuestWin = 0;
	sQuestSample = "";
	for (i = 0; i < 5; i++)
	{
		aBusy[i] = 0;
		aForever[i] = 0;
	}
	for (i = 0; i < 8; i++)
	{
		iTopIdx[i] = -1;
		iTopNum[i] = 0;
	}

	for (i = 0; i < TOTAL_CHARACTERS; i++)
	{
		makeref(chref, Characters[i]);
		if (!CheckAttribute(chref, "id")) continue;
		if (chref.id == "0") continue;
		iBusy++;
		// зона слота
		iZone = 1;
		if (i < GlobalCharacters) iZone = 0;
		if (i >= MAX_CHARACTERS) iZone = 2;
		if (i >= LOC_FANTOM_CHARACTERS) iZone = 3;
		if (i >= FANTOM_CHARACTERS) iZone = 4;
		aBusy[iZone] = aBusy[iZone] + 1;

		if (LAi_IsDead(chref))
		{
			iDead++;
			// мёртвый без возрождения и без срока - слот держится мёртвым грузом
			if (!CheckAttribute(chref, "RebirthPhantom"))
			{
				if (!CheckAttribute(chref, "LifeDay")) iDeadStuck++;
			}
		}
		if (CheckAttribute(chref, "RebirthPhantom")) iRebirth++;
		if (CheckAttribute(chref, "LifeDay")) iLife++;
		if (CheckCompanionInCharacter(pchar, chref.id)) iComp++;
		if (CheckPassengerInCharacter(pchar, chref.id)) iComp++;
		// «вечный» - живой, без срока жизни и без права на возрождение: такой слот не освободит никто
		if (!LAi_IsDead(chref))
		{
			if (!CheckAttribute(chref, "LifeDay"))
			{
				if (i >= GlobalCharacters)
				{
					iForever++;
					aForever[iZone] = aForever[iZone] + 1;
					// LEO: кто именно занимает слот навсегда - по этому видно, кого учить сроку жизни
					sKind = chref.id;
					if (findsubstr(sKind, "PsHero", 0) == 0) iFvPGG++;
					else
					{
						if (CheckAttribute(chref, "quest"))
						{
							iFvQuest++;
							// LEO: отделяем НАСТОЯЩИХ квестовых (у них есть условие победы) от тех,
							//   кому квестовая система просто что-то записала в ветку
							if (CheckAttribute(chref, "quest.win_condition")) iFvQuestWin++;
							else
							{
								if (strlen(sQuestSample) < 90) sQuestSample = sQuestSample + chref.id + " ";
							}
						}
						else
						{
							if (CheckAttribute(chref, "OfficerType")) iFvOfficer++;
							else
							{
								if (findsubstr(sKind, "Citizen", 0) >= 0) iFvCitizen++;
								else
								{
									if (findsubstr(sKind, "Gen", 0) == 0) iFvGen++;
									else iFvOther++;
								}
							}
						}
					}
				}
			}
		}
		// самые «толстые» по числу веток атрибутов
		n = GetAttributesNum(chref);
		iWorst = -1;
		for (j = 0; j < 8; j++)
		{
			if (n > iTopNum[j])
			{
				iWorst = j;
				j = 8;
			}
		}
		if (iWorst >= 0)
		{
			for (k = 7; k > iWorst; k--)
			{
				iTopNum[k] = iTopNum[k-1];
				iTopIdx[k] = iTopIdx[k-1];
			}
			iTopNum[iWorst] = n;
			iTopIdx[iWorst] = i;
		}
	}

	totalInfo = totalInfo + NewStr() + NewStr();
	totalInfo = totalInfo + "Планка NPC (MAX_CHARACTERS): " + MAX_CHARACTERS + ",  постоянных: " + GlobalCharacters + NewStr();
	totalInfo = totalInfo + "Занято слотов всего: " + iBusy + " из " + TOTAL_CHARACTERS + NewStr();
	totalInfo = totalInfo + "  постоянные 0.." + GlobalCharacters + ": " + aBusy[0] + NewStr();
	totalInfo = totalInfo + "  генерённые " + GlobalCharacters + ".." + MAX_CHARACTERS + ": " + aBusy[1] + ",  из них вечных: " + aForever[1] + NewStr();
	totalInfo = totalInfo + "  вне планки " + MAX_CHARACTERS + ".." + LOC_FANTOM_CHARACTERS + ": " + aBusy[2] + ",  из них вечных: " + aForever[2] + NewStr();
	totalInfo = totalInfo + "  локационные " + LOC_FANTOM_CHARACTERS + ".." + FANTOM_CHARACTERS + ": " + aBusy[3] + ",  из них вечных: " + aForever[3] + NewStr();
	totalInfo = totalInfo + "  морские " + FANTOM_CHARACTERS + ".." + TOTAL_CHARACTERS + ": " + aBusy[4] + ",  из них вечных: " + aForever[4] + NewStr();
	totalInfo = totalInfo + NewStr();
	totalInfo = totalInfo + "Мёртвых: " + iDead + ",  с RebirthPhantom: " + iRebirth + ",  со сроком LifeDay: " + iLife + NewStr();
	totalInfo = totalInfo + "  из них БЕЗ возрождения и БЕЗ срока (слот не вернётся): " + iDeadStuck + NewStr();
	totalInfo = totalInfo + NewStr() + "Вечные по типам: ПГГ " + iFvPGG + ",  квестовые " + iFvQuest + ",  офицеры " + iFvOfficer + NewStr();
	totalInfo = totalInfo + "                 горожане " + iFvCitizen + ",  генерённые " + iFvGen + ",  прочие " + iFvOther + NewStr();
	totalInfo = totalInfo + "  из «квестовых» с условием победы: " + iFvQuestWin + ",  без него: " + (iFvQuest - iFvQuestWin) + NewStr();
	totalInfo = totalInfo + "  примеры без условия: " + sQuestSample + NewStr();

	// LEO: со временем толстеет не число персонажей, а ИХ ДЕРЕВЬЯ. Смотрим у ГГ, какие ветки
	//   первого уровня самые крупные - именно они и растут от партии к партии.
	sHeavyBranch = "";
	iBrQty = GetAttributesNum(pchar); // ⚠️ по ref НАПРЯМУЮ: makearef(x, pchar) даёт пустой перебор
	for (n = 0; n < 6; n++)
	{
		iBrMax = -1; iBrWorst = -1;
		for (j = 0; j < iBrQty; j++)
		{
			arSub = GetAttributeN(pchar, j); // ⚠️ именно присваивание: makearef такой возврат не принимает
			iBrSub = GetAttributesNum(arSub);
			if (iBrSub > iBrMax)
			{
				if (findsubstr(sHeavyBranch, GetAttributeName(GetAttributeN(pchar, j)), 0) == -1)
				{
					iBrMax = iBrSub;
					iBrWorst = j;
				}
			}
		}
		if (iBrWorst < 0) break;
		sHeavyBranch = sHeavyBranch + GetAttributeName(GetAttributeN(pchar, iBrWorst)) + "=" + iBrMax + "  ";
	}
	totalInfo = totalInfo + NewStr() + "Самые толстые ветки ГГ: " + sHeavyBranch + NewStr();

	// LEO: questTemp не чистится НИГДЕ - смотрим, чем именно она набита
	sQtBranch = "";
	if (CheckAttribute(pchar, "questTemp"))
	{
		makearef(arBr, pchar.questTemp);
		iBrQty = GetAttributesNum(arBr);
		for (n = 0; n < 8; n++)
		{
			iBrMax = -1; iBrWorst = -1;
			for (j = 0; j < iBrQty; j++)
			{
				arSub = GetAttributeN(arBr, j);
				iBrSub = GetAttributesNum(arSub) + 1;
				if (iBrSub > iBrMax)
				{
					if (findsubstr(sQtBranch, GetAttributeName(arSub), 0) == -1)
					{
						iBrMax = iBrSub;
						iBrWorst = j;
					}
				}
			}
			if (iBrWorst < 0) break;
			sQtBranch = sQtBranch + GetAttributeName(GetAttributeN(arBr, iBrWorst)) + "=" + iBrMax + " ";
		}
		totalInfo = totalInfo + "questTemp (" + iBrQty + " веток): " + sQtBranch + NewStr();
	}
	totalInfo = totalInfo + "В команде (компаньоны и пассажиры): " + iComp + NewStr();
	totalInfo = totalInfo + "ВЕЧНЫХ (живой, без LifeDay, слот не освободится): " + iForever + NewStr();
	totalInfo = totalInfo + NewStr() + "Самые тяжёлые по числу веток атрибутов:" + NewStr();
	for (j = 0; j < 8; j++)
	{
		if (iTopIdx[j] < 0) continue;
		makeref(chref, Characters[iTopIdx[j]]);
		sChrId = chref.id;
		totalInfo = totalInfo + "  " + iTopIdx[j] + ": " + sChrId + " - " + iTopNum[j] + NewStr();
	}

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F62", 1);
}

// Gregg: ==> готовность к квестам сюжетной линии К:ПС (F63-F70).
// Линия идёт цепочкой: конец главы взводит таймер, тот ставит Кендарту нужный диалог.
// Кнопки выставляют состояние ПРЕДЫДУЩИХ глав и снимают ожидание, чтобы завязку можно
// было проверить, не проходя всю линию заново. Порог ранга поднимается только вверх:
// у кого он уже выше, тому кнопка ничего не портит.

void DBG_KPS_Rank(int iRank)
{
	if (sti(PChar.rank) < iRank) PChar.rank = iRank;
}

// Кендарт заводится главой 6 (BO_SGCitizens), а диалоги глав 7-10 вешаются на него.
// На скипе его может не быть вовсе - тогда CharacterFromID вернёт мусор, и функция
// главы упадёт. Заводим сами, теми же полями, и ставим в ратушу Сан-Хуана.
// bInSanJuan: где оставить новосозданного. По сюжету он заводится в гроте Теркса
// (финал главы 5), сразу после сцены уходит в «None» и всплывает там, где его ставит
// очередная глава - в главе 6 это верх таверны Ле-Франсуа. С главы 7 он уже сидит
// в ратуше Сан-Хуана, туда его и селим. Уже существующего НЕ трогаем: у честного
// прохождения своё место.
void DBG_KPS_Kendart(bool bInSanJuan)
{
	if (GetCharacterIndex("Alehon Kendart") != -1) return;
	ref chr = GetCharacter(NPC_GenerateCharacter("Alehon Kendart", "CCS_Mechanic", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_KendartName");
	chr.lastname = iTxt("BO_KendartLastName");
	chr.greeting = "Gr_Master";
	chr.NotRemoved = true;
	chr.ShipEnemyDisable = true;
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	if (bInSanJuan)
	{
		ChangeCharacterAddressGroup(chr, "SanJuan_townhall", "goto", "governor1");
	}
	else
	{
		ChangeCharacterAddress(chr, "None", "");
	}
}

// Сан-Хуан переходит к ГГ в финале главы 6 - см. SpanishGenerosityEnd в blackorchid.c.
// Кнопки поздних глав ставят то же самое, иначе состояние линии разъедется с сюжетом.
void DBG_KPS_SanJuan()
{
	int iCol = FindColony("SanJuan");
	if (iCol < 0) return;
	ref colony = &Colonies[iCol];
	// HeroOwn отмечает захват; наличие isBought само по себе не означает владение.
	if (CheckAttribute(colony, "HeroOwn"))
	{
		if (colony.HeroOwn == true) return;
	}

	// дальше повторяем финал главы 6 (SpanishGenerosityToSanJuan): испанская власть
	// из города уходит, иначе мэр остаётся сидеть в ратуше уже нашего Сан-Хуана
	if (GetCharacterIndex("SanJuan_Smuggler") != -1)
	{
		ChangeCharacterAddress(CharacterFromID("SanJuan_Smuggler"), "None", "");
	}
	if (GetCharacterIndex("SanJuan_Mayor") != -1)
	{
		ChangeCharacterAddress(CharacterFromID("SanJuan_Mayor"), "None", "");
	}

	SetCaptureTownByHero("SanJuan");
	colony.isBought = true;
	colony.DontSetShipInPort = true;
	colony.OfficerIdx = nMainCharacterIndex;

	// комендант форта переходит на нашу сторону - иначе форт бьёт по своим же
	if (GetCharacterIndex("SanJuan Fort Commander") != -1)
	{
		ref chrFort = CharacterFromID("SanJuan Fort Commander");
		chrFort.AnalizeShips = true;
		chrFort.AlwaysFriend = true;
		chrFort.ShipEnemyDisable = true;
		chrFort.DontCheckFlag = true;
	}
}

// Наместник Сан-Хуана. По сюжету садится в кресло ратуши в самом финале главы 6
// (SpanishGenerosityFinalBattleEnd), после разгрома эскадры Меркеса - до того кресло
// пустует, а наместником формально числится сам ГГ. Кнопки, пропускающие главу 6,
// обязаны посадить его сами, иначе город наш, а ратуша стоит без хозяина: налог
// собирать не с кого и диалог губернатора не открыть.
void DBG_KPS_Reksvell()
{
	if (GetCharacterIndex("Robert Reksvell") != -1) return;
	int iCol = FindColony("SanJuan");
	if (iCol < 0) return;

	ref chr = GetCharacter(NPC_GenerateCharacter("Robert Reksvell", "trader_5", "man", "man", 25, PIRATE, -1, false));
	chr.name = iTxt("BO_ReksvellName");
	chr.lastname = iTxt("BO_ReksvellLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Reksvell.c";
	chr.Dialog.CurrentNode = "First Time";
	chr.greeting = "Gr_questMan";
	chr.NotRemoved = true;
	LAi_SetHuberType(chr);
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "SanJuan_townhall", "sit", "sit1");
	SetSelfSkill(chr, 20, 20, 20, 20, 50);
	SetShipSkill(chr, 100, 100, 20, 20, 20, 50, 20, 35, 45);

	Colonies[iCol].OfficerIdx = GetCharacterIndex("Robert Reksvell");
	// без ColonyIdx его диалог не найдёт колонию и посчитает налог по заглушке
	chr.ColonyIdx = iCol;

	SaveCurrentNpcQuestDateParam(chr, "StartTaxDate");
	SaveCurrentNpcQuestDateParam(chr, "GoverTalkDate");
	LAi_LoginInCaptureTown(chr, true);
}

// Общая часть кнопок трилогии и далее: обе вводные линии закрыты, ранг не ниже 17.
void DBG_KPS_BeforeOrchid()
{
	PChar.UneasyDelivery = "END";
	PChar.TreacheryOfGrifoSajfus = "END";
	DBG_KPS_Rank(17);	// BO_RANK_REQ из blackorchid.c; числом - дефайн живёт в другом файле
}

void CalculateInfoDataF63()
{
	totalInfo = descF63;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_147", "Interface.txt", "#space#", " ");

	// «Предательство Грифо Сайфуса»: завязку даёт трактирщик пиратской таверны Бермуд,
	// но только с 5 ранга и пока квест ни разу не брали (Pirates_Tavern.c).
	DBG_KPS_Rank(5);
	if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus")) DeleteAttribute(PChar, "TreacheryOfGrifoSajfus");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F63", 1);
}

void CalculateInfoDataF64()
{
	totalInfo = descF64;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_149", "Interface.txt", "#space#", " ");

	// Орхидея, часть 1 - «Таинственный артефакт». Посыльный с Бермуд приходит сам,
	// как только гейт увидит обе закрытые линии и ранг (MysteriousArtefactCheckCan
	// зовётся из ReloadEndFade, то есть на ближайшей смене локации).
	DBG_KPS_BeforeOrchid();
	if (CheckAttribute(PChar, "MysteriousArtefact")) DeleteAttribute(PChar, "MysteriousArtefact");
	MysteriousArtefactCheckCan();

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F64", 1);
}

void CalculateInfoDataF65()
{
	totalInfo = descF65;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_151", "Interface.txt", "#space#", " ");

	// Орхидея, часть 2 - «Подарок судьбы». Начинается не разговором, а сценой: ГГ
	// приходит в себя в доме Кендарта в посёлке ловцов, поэтому зовём финал первой части.
	DBG_KPS_BeforeOrchid();
	PChar.MysteriousArtefact = "END";
	MysteriousArtefactEnd("");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F65", 1);
}

void CalculateInfoDataF66()
{
	totalInfo = descF66;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_153", "Interface.txt", "#space#", " ");

	// Орхидея, часть 3 - «Испанская щедрость». В игре её ждут 15 суток после второй
	// части, здесь ожидание снимаем.
	DBG_KPS_BeforeOrchid();
	PChar.MysteriousArtefact = "END";
	PChar.DestinyGift = "END";
	// Кендарт заводится финалом главы 5 и с этого момента нужен всем последующим:
	// SpanishGenerosityToLeFransua сажает именно его на верх таверны Ле-Франсуа.
	DBG_KPS_Kendart(false);
	SpanishGenerosityStart("");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F66", 1);
}

void CalculateInfoDataF67()
{
	totalInfo = descF67;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_155", "Interface.txt", "#space#", " ");

	// Глава 7, «Маленький ублюдок». Ждать 60 суток после третьей части не нужно,
	// зато нужен сам Кендарт и Сан-Хуан за ГГ - его берут в финале «Испанской щедрости».
	DBG_KPS_BeforeOrchid();
	PChar.MysteriousArtefact = "END";
	PChar.DestinyGift = "END";
	PChar.SpanishGenerosity = "END";
	DBG_KPS_SanJuan();
	DBG_KPS_Reksvell();
	DBG_KPS_Kendart(true);
	_SmallMongrelStart("");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F67", 1);
}

void CalculateInfoDataF68()
{
	totalInfo = descF68;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_157", "Interface.txt", "#space#", " ");

	// Глава 8, «Рауль Эсперанто».
	DBG_KPS_BeforeOrchid();
	PChar.MysteriousArtefact = "END";
	PChar.DestinyGift = "END";
	PChar.SpanishGenerosity = "END";
	PChar.SmallMongrel = "END";
	DBG_KPS_SanJuan();
	DBG_KPS_Reksvell();
	DBG_KPS_Kendart(true);
	RaulEsperantoTime("");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F68", 1);
}

void CalculateInfoDataF69()
{
	totalInfo = descF69;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_159", "Interface.txt", "#space#", " ");

	// Глава 9, «Найти и уничтожить».
	DBG_KPS_BeforeOrchid();
	PChar.MysteriousArtefact = "END";
	PChar.DestinyGift = "END";
	PChar.SpanishGenerosity = "END";
	PChar.SmallMongrel = "END";
	PChar.RaulEsperanto = "END";
	DBG_KPS_SanJuan();
	DBG_KPS_Reksvell();
	DBG_KPS_Kendart(true);
	FindAndDestroyTime("");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F69", 1);
}

void CalculateInfoDataF70()
{
	totalInfo = descF70;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_161", "Interface.txt", "#space#", " ");

	// Глава 10, «Две сестры».
	DBG_KPS_BeforeOrchid();
	PChar.MysteriousArtefact = "END";
	PChar.DestinyGift = "END";
	PChar.SpanishGenerosity = "END";
	PChar.SmallMongrel = "END";
	PChar.RaulEsperanto = "END";
	PChar.FindAndDestroy = "END";
	DBG_KPS_SanJuan();
	DBG_KPS_Reksvell();
	DBG_KPS_Kendart(true);
	if (CheckAttribute(PChar, "TwoSisters")) DeleteAttribute(PChar, "TwoSisters");
	TwoSistersTimeEnd("");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F70", 1);
}

// Gregg: <== готовность к квестам линии К:ПС

// LEO: выгрузка карты текущего острова в PNG - исходник для карт атласа. Работает В МОРЕ:
//   остров снимается ортокамерой сверху, фон прозрачный, файл кладётся в корень игры и
//   называется по самому острову. Размер снятой области в метрах пишется в system.log.
void CalculateInfoDataF71()
{
	totalInfo = descF71;

	if(IsEntity(&Island) == 0)
	{
		totalInfo = totalInfo + NewStr() + NewStr() + "Остров не загружен - нужно быть в море.";
	}
	else
	{
		int nMapRadius = GetMapRadius();
		float fMapRadius = makefloat(nMapRadius);
		int nMapSize = 4096;
		// Пустая техника = родной материал острова с лайтмапами. Чтобы вернуть плоский вид,
		// подставить "battle_island_gettexture".
		string sMapTech = "";
		// Солнце для рельефа - главные ручки внешнего вида. Азимут: 0 = с севера, 315 = с северо-запада.
		// Высота: ниже солнце - длиннее тени и резче горы. 0 гасит свет совсем (плоско, как было).
		float fSunAzim = 315.0;
		float fSunElev = 100.0;
		float fAmbient = 1.0;   // подсветка теней: меньше = контрастнее
		DumpIslandMap(nMapSize, sMapTech, fSunAzim, fSunElev, fAmbient);
		DumpIslandMapAtShip(nMapSize, fMapRadius, sMapTech, fSunAzim, fSunElev, fAmbient);
		totalInfo = totalInfo + NewStr() + NewStr() + "Карты сохранены в Docs_AND_Report\Screenshots:";
		totalInfo = totalInfo + NewStr() + "остров целиком и вокруг корабля (радиус " + nMapRadius + " м, файл с _).";
	}

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F71", 1);
}

void CalculateInfoDataF72()
{
	totalInfo = descF72;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_165", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F72", 1);
}

void CalculateInfoDataF73()
{
	totalInfo = descF73;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_167", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F73", 1);
}

void CalculateInfoDataF74()
{
	totalInfo = descF74;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_169", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F74", 1);
}

void CalculateInfoDataF75()
{
	totalInfo = descF75;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_171", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F75", 1);
}

void CalculateInfoDataF76()
{
	totalInfo = descF76;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_173", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F76", 1);
}

void CalculateInfoDataF77()
{
	totalInfo = descF77;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_175", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F77", 1);
}

void CalculateInfoDataF78()
{
	totalInfo = descF78;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_177", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F78", 1);
}

void CalculateInfoDataF79()
{
	totalInfo = descF79;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_179", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F79", 1);
}

void CalculateInfoDataF80()
{
	totalInfo = descF80;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_181", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F80", 1);
}

void CalculateInfoDataF81()
{
	totalInfo = descF81;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_183", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F81", 1);
}

void CalculateInfoDataF82()
{
	totalInfo = descF82;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_185", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F82", 1);
}

void CalculateInfoDataF83()
{
	totalInfo = descF83;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_187", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F83", 1);
}

void CalculateInfoDataF84()
{
	totalInfo = descF84;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_189", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F84", 1);
}

void CalculateInfoDataF85()
{
	totalInfo = descF85;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_191", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F85", 1);
}

void CalculateInfoDataF86()
{
	totalInfo = descF86;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_193", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F86", 1);
}

void CalculateInfoDataF87()
{
	totalInfo = descF87;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_195", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F87", 1);
}

void CalculateInfoDataF88()
{
	totalInfo = descF88;

	totalInfo = totalInfo + NewStr() + NewStr() + GetConvertStrWithReplace("Variable_Debuger_197", "Interface.txt", "#space#", " ");

	SetFormatedText("INFO_TEXT", totalInfo);

	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F88", 1);
}

void LoginDummy()
{
	LAi_group_SetRelation("teno_monsters_group", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);

	//ставим тушку
	ref sld = GetCharacter(NPC_GenerateCharacter("DummyCharacter", "mictlantecuhtli", "man", "man", 50, PIRATE, 0, true));
	sld.name = GetConvertStrWithReplace("Variable_Debuger_199", "Interface.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_Debuger_200", "Interface.txt", "#space#", " ");
	ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
	//вооружение

    FantomMakeCoolFighter(sld, 100, 100, 100, "blade28", "", 99999); 		// Танат		(ТО)
	//установить SPECIAL
	SetSPECIAL(sld, 10,10,10,10,10,10,10); // SPECIAL (Сила, Восприятие, Выносливость, Лидерство, Обучаемость, Реакция, Удача)
	//установить хп
	LAi_SetHP(sld,9999,9999);

	//установить агр
	LAi_SetWarriorType(sld);
	LAi_group_SetRelation("teno_monsters_group", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("teno_monsters_group", LAI_GROUP_PLAYER, true);

	//перки
	sld.perks.list.BasicDefense = "1";
	sld.perks.list.AdvancedDefense = "1";
	sld.perks.list.CriticalHit = "1";
	sld.perks.list.Ciras = "1";
	sld.perks.list.SwordplayProfessional = "1";
	sld.perks.list.Grus = "1";
	sld.perks.list.Rush = "1";
	sld.perks.list.Tireless = "1";
	sld.perks.list.HardHitter = "1";
	sld.perks.list.BladeDancer = "1";
	sld.perks.list.Sliding = "1";
	sld.perks.list.Gunman = "1";
	sld.perks.list.GunProfessional = "1";
	sld.perks.list.IronWill = "1";
	sld.perks.list.Medic = "1";
	sld.perks.list.HPPlus = "1";
	sld.perks.list.EnergyPlus = "1";
	sld.perks.list.Trustworthy = "1";
	sld.perks.list.ShipEscape = "1";

	sld.perks.list.FlagPir = "1";
	sld.perks.list.FlagEng = "1";
	sld.perks.list.FlagFra = "1";
	sld.perks.list.FlagSpa = "1";
	sld.perks.list.FlagHol = "1";
	sld.perks.list.FastReload = "1";
	sld.perks.list.ImmediateReload = "1";
	sld.perks.list.HullDamageUp = "1";
	sld.perks.list.SailsDamageUp = "1";
	sld.perks.list.CrewDamageUp = "1";
	sld.perks.list.CriticalShoot = "1";
	sld.perks.list.LongRangeShoot = "1";
	sld.perks.list.CannonProfessional = "1";
	sld.perks.list.LongRangeGrappling = "1";
	sld.perks.list.MusketsShoot = "1";
	sld.perks.list.GrapplingProfessional = "1";
	sld.perks.list.BasicBattleState = "1";
	sld.perks.list.AdvancedBattleState = "1";
	sld.perks.list.ShipDefenseProfessional = "1";
	sld.perks.list.LightRepair = "1";
	sld.perks.list.InstantRepair = "1";
	sld.perks.list.ShipSpeedUp = "1";
	sld.perks.list.ShipTurnRateUp = "1";
	sld.perks.list.StormProfessional = "1";
	sld.perks.list.Turn180 = "1";
	sld.perks.list.SailingProfessional = "1";
	sld.perks.list.Brander = "1";
	sld.perks.list.Troopers = "1";
	sld.perks.list.BasicCommerce = "1";
	sld.perks.list.AdvancedCommerce = "1";
	sld.perks.list.Carpenter = "1";
	sld.perks.list.Builder = "1";
	sld.perks.list.WindCatcher = "1";
	sld.perks.list.SailsMan = "1";
	sld.perks.list.Doctor1 = "1";
	sld.perks.list.Doctor2 = "1";
	sld.perks.list.EmergentSurgeon = "1";
	sld.perks.list.ByWorker = "1";
	sld.perks.list.ByWorker2 = "1";

	//установить ЛУ и КУ
	SetSelfSkill(sld, 100, 100, 100, 100, 100); //лёгкое, среднее, тяжёлое, пистолет, удача
	SetShipSkill(sld, 100, 100, 100, 100, 100, 100, 100, 100, 100); // лидерство, торговля, точность, пушки, навигация, ремонт, абордаж, защита, скрытность
}

void ReloadByStr()
{
	// разбор строки
	string loc = stripblank(GetSubStringByNum(GameInterface.reload_edit.str, 0));
	string grp = stripblank(GetSubStringByNum(GameInterface.reload_edit.str, 1));
	string ltr = stripblank(GetSubStringByNum(GameInterface.reload_edit.str, 2));

	if (loc==GetConvertStrWithReplace("Variable_Debuger_201", "Interface.txt", "#space#", " "))
	{
		grp = GetSubStringByNum(GameInterface.reload_edit.str, 1);
		for(int j = 0; j < GOODS_QUANTITY; j++)
		{
			if (grp == XI_ConvertString(Goods[j].name))
			{
				string good = Goods[j].name;
				if (sti(ltr)>-1 && sti(ltr) < 999999) pchar.Ship.Cargo.Goods.(good) = sti(ltr);
				else break;
				Log_info(GetConvertStrWithReplace("Variable_Debuger_202", "Interface.txt", "#space#", " ")+XI_ConvertString(good)+GetConvertStrWithReplace("Variable_Debuger_203", "Interface.txt", "#space#", " ")+ltr);
				break;
			}
		}
	}
	if (sti(loc) > 0 && sti(loc) < 13)
	{
		if (sti(grp) > 0 && sti(grp) < 32)
		{
			if (sti(ltr) > -1 && sti(ltr) < 25)
			{
				Environment.date.month = sti(loc);
				Environment.date.day = sti(grp);
				SetCurrentTime(sti(ltr),0);
				DoQuestReloadToLocation(loadedLocation.id, "reload", "reload1", "");
				return;
			}
		}
	}
    int i = FindLocation(loc);
    if (i != -1)
    {
        if(CheckAttribute(&locations[i],"fastreload"))
        {
			if (GetCityFrom_Sea(locations[i].fastreload) != "")
		    {
		        setCharacterShipLocation(pchar, GetCityFrom_Sea(locations[i].fastreload));
		        setWDMPointXZ(GetCityFrom_Sea(locations[i].fastreload));
		    }
	    }
	    else
	    {
	        if (locations[i].type == "seashore" || locations[i].type == "mayak")
	        {
	            setCharacterShipLocation(pchar, loc));
		        setWDMPointXZ(loc);
	        }
	    }
	    DoQuestReloadToLocation(loc, grp, ltr, "");
	    Log_Info(loc);
		// Статистика по читам
		Statistic_AddValue(PChar, "Cheats.ReloadByStr", 1);
    }
    else
    {
        Log_Info(GetConvertStrWithReplace("Variable_Debuger_204", "Interface.txt", "#space#", " "));
    }
}

void XI_SetScroller(float pos)
{
	SendMessage(&GameInterface,"lsf",MSG_INTERFACE_SET_SCROLLER,"INFO_SCROLL",pos);
}

void SetScrollerPos()
{
	string nodName = GetEventData();
	float pos = GetEventData();
	XI_SetScroller(pos);
}

void ProcScrollPosChange()
{
	float newPos = GetEventData();
    SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",2, newPos);
}

void ProcScrollChange()
{
	int changeNum = GetEventData();
	if(changeNum==0) return;
	string controlNode = "";
	if( GetSelectable("INFO_TEXT") ) controlNode = "INFO_TEXT";

	if(controlNode!="")
	{
		if(changeNum>0) {
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,controlNode,-1, 0,ACTION_DOWNSTEP);
		} else {
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,controlNode,-1, 0,ACTION_UPSTEP);
		}
	}
}

void GetRealCoordsObjects()
{
	trace("---------");
	trace("Barbados (island)					: " + Map_GetRealCoordX(946.0062) + " " + Map_GetRealCoordZ(-486.9014));
	trace(GetConvertStrWithReplace("Variable_Debuger_205", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(903.6217) + " " + Map_GetRealCoordZ(-522.2799));
	trace("Mayak2 (маяк Барбадоса)				: " + Map_GetRealCoordX(918.619) + " " + Map_GetRealCoordZ(-506.827));
	trace("Shore4 (мыс Гаррисон)				: " + Map_GetRealCoordX(919.808) + " " + Map_GetRealCoordZ(-448.061));
	trace("Shore5 (мыс Раггед Пойнт)			: " + Map_GetRealCoordX(983.787) + " " + Map_GetRealCoordZ(-518.14));
	trace("---------");
	trace("Antigua (island)						: " + Map_GetRealCoordX(903.9222) + " " + Map_GetRealCoordZ(109.0605));
	trace(GetConvertStrWithReplace("Variable_Debuger_206", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(852.4083) + " " + Map_GetRealCoordZ(128.9092));
	trace("Shore1 (мыс Бун)						: " + Map_GetRealCoordX(880.892) + " " + Map_GetRealCoordZ(148.897));
	trace("Shore2 (залив Уиллогби)				: " + Map_GetRealCoordX(931.028) + " " + Map_GetRealCoordZ(74.5353));
	trace("---------");
	trace("Curacao (island) 					: " + Map_GetRealCoordX(383.4142) + " " + Map_GetRealCoordZ(-691.11));
	trace(GetConvertStrWithReplace("Variable_Debuger_207", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(396.1) + " " + Map_GetRealCoordZ(-722.8));
	trace("Shore22 (Пальмовый берег)			: " + Map_GetRealCoordX(354.604) + " " + Map_GetRealCoordZ(-678.54));
	trace("Shore23 (Мыс Кальвинистов)			: " + Map_GetRealCoordX(347.313) + " " + Map_GetRealCoordZ(-705.112));
	trace("Shore24 (берег Приют Контрабандиста)	: " + Map_GetRealCoordX(455.427) + " " + Map_GetRealCoordZ(-740.655));
	trace("---------");
	trace("SentMartin (island)					: " + Map_GetRealCoordX(921.559) + " " + Map_GetRealCoordZ(523.596));
	trace(GetConvertStrWithReplace("Variable_Debuger_208", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(900.0994) + " " + Map_GetRealCoordZ(501.6531));
	trace("Shore40 (берег Спасения)				: " + Map_GetRealCoordX(924.442) + " " + Map_GetRealCoordZ(554.385));
	trace("Shore41 (бухта Неистового ветра)		: " + Map_GetRealCoordX(898.877) + " " + Map_GetRealCoordZ(522.703));
	trace("---------");
	trace("Guadeloupe (island)					: " + Map_GetRealCoordX(537.114) + " " + Map_GetRealCoordZ(-25.3853));
	trace(GetConvertStrWithReplace("Variable_Debuger_209", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(462.2107) + " " + Map_GetRealCoordZ(-73.0837));
	trace("Mayak4 (маяк Гваделупы)				: " + Map_GetRealCoordX(465.076) + " " + Map_GetRealCoordZ(-38.9423));
	trace("Shore28 (бухта Морн л'О)				: " + Map_GetRealCoordX(536.636) + " " + Map_GetRealCoordZ(26.1279));
	trace("Shore29 (пляж Капстер)				: " + Map_GetRealCoordX(507.669) + " " + Map_GetRealCoordZ(-75.4176));
	trace("Shore30 (бухта Гран Бур)				: " + Map_GetRealCoordX(549.807) + " " + Map_GetRealCoordZ(-69.7107));
	trace("Shore31 (мыс Недоступный)			: " + Map_GetRealCoordX(589.889) + " " + Map_GetRealCoordZ(-65.0601));
	trace("---------");
	trace("Martinique (island) 					: " + Map_GetRealCoordX(538.524) + " " + Map_GetRealCoordZ(-385.567));
	trace(GetConvertStrWithReplace("Variable_Debuger_210", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(513.51) + " " + Map_GetRealCoordZ(-397.645));
	trace(GetConvertStrWithReplace("Variable_Debuger_211", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(559.008) + " " + Map_GetRealCoordZ(-368.16));
	trace("Shore38 (пляж Ламантен)				: " + Map_GetRealCoordX(481.181) + " " + Map_GetRealCoordZ(-340.832));
	trace("Shore39 (бухта Ле Марен)				: " + Map_GetRealCoordX(557.181) + " " + Map_GetRealCoordZ(-422.987));
	trace("---------");
	trace("Bermudes (island)					: " + Map_GetRealCoordX(920.738) + " " + Map_GetRealCoordZ(920.767));
	trace(GetConvertStrWithReplace("Variable_Debuger_212", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(931.059) + " " + Map_GetRealCoordZ(928.281));
	trace("Shore3 (залив Руны)					: " + Map_GetRealCoordX(915.745) + " " + Map_GetRealCoordZ(966.469));
	trace("Shore_ship1 (бухта Разбитого Корыта) : " + Map_GetRealCoordX(968.947) + " " + Map_GetRealCoordZ(923.636));
	trace("---------");
	trace("Maracaibo (areal) 					: " + Map_GetRealCoordX(178.51) + " " + Map_GetRealCoordZ(-870.37));
	trace(GetConvertStrWithReplace("Variable_Debuger_213", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(134.82) + " " + Map_GetRealCoordZ(-939.65));
	trace(GetConvertStrWithReplace("Variable_Debuger_214", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(198.162) + " " + Map_GetRealCoordZ(-994.035));
	trace("Shore37 (бухта Гуахира)				: " + Map_GetRealCoordX(140.753) + " " + Map_GetRealCoordZ(-828.99));
	trace("---------");
	trace("Beliz (areal) 						: " + Map_GetRealCoordX(-896.084) + " " + Map_GetRealCoordZ(230.227));
	trace("Shore_ship2 (залив Гибели)			: " + Map_GetRealCoordX(-1022.94) + " " + Map_GetRealCoordZ(629.355));
	trace("Shore6 (мыс Каточе)					: " + Map_GetRealCoordX(-909.655) + " " + Map_GetRealCoordZ(629.079));
	trace("Shore7 (залив Косумель)				: " + Map_GetRealCoordX(-915.581) + " " + Map_GetRealCoordZ(409.002));
	trace("Shore8 (бухта Четумаль)				: " + Map_GetRealCoordX(-963.489) + " " + Map_GetRealCoordZ(281.628));
	trace(GetConvertStrWithReplace("Variable_Debuger_215", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-978.396) + " " + Map_GetRealCoordZ(158.751));
	trace("Shore9 (бухта Аматике)				: " + Map_GetRealCoordX(-1017.2) + " " + Map_GetRealCoordZ(-33.7293));
	trace("Shore10 (лагуна Каратаска)			: " + Map_GetRealCoordX(-818.198) + " " + Map_GetRealCoordZ(-177.208));
	trace("---------");
	trace("Jamaica (island)						: " + Map_GetRealCoordX(-298.0) + " " + Map_GetRealCoordZ(-128.0));
	trace(GetConvertStrWithReplace("Variable_Debuger_216", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-250.157) + " " + Map_GetRealCoordZ(-150.839));
	trace(GetConvertStrWithReplace("Variable_Debuger_217", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-446.951) + " " + Map_GetRealCoordZ(-107.118));
	trace("Shore36 (бухта Портленд)				: " + Map_GetRealCoordX(-281.543) + " " + Map_GetRealCoordZ(-164.245));
	trace("Mayak3 (маяк Порт Рояля)				: " + Map_GetRealCoordX(-224.127) + " " + Map_GetRealCoordZ(-163.435));
	trace("---------");
	trace("PuertoRico (island) 					: " + Map_GetRealCoordX(506.489) + " " + Map_GetRealCoordZ(460.815));
	trace(GetConvertStrWithReplace("Variable_Debuger_218", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(528.645) + " " + Map_GetRealCoordZ(484.18));
	trace("Shore44 (бухта Агуадилья)			: " + Map_GetRealCoordX(392.82) + " " + Map_GetRealCoordZ(501.808));
	trace("Shore45 (пляж Фахардо)				: " + Map_GetRealCoordX(602.049) + " " + Map_GetRealCoordZ(458.322));
	trace("---------");
	trace("Cuba1 (areal) 						: " + Map_GetRealCoordX(-358.027) + " " + Map_GetRealCoordZ(819.223));
	trace(GetConvertStrWithReplace("Variable_Debuger_219", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-339.796) + " " + Map_GetRealCoordZ(771.839));
	trace(GetConvertStrWithReplace("Variable_Debuger_220", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-282.032) + " " + Map_GetRealCoordZ(839.056));
	trace("Shore11 (мыс Камагуэй)				: " + Map_GetRealCoordX(-386.907) + " " + Map_GetRealCoordZ(883.044));
	trace("Shore12 (залив Анна Мария)			: " + Map_GetRealCoordX(-468.486) + " " + Map_GetRealCoordZ(838.497));
	trace("Mayak9 (маяк Сантьяго)				: " + Map_GetRealCoordX(-333.295) + " " + Map_GetRealCoordZ(723.486));
	trace("---------");
	trace("Cuba2 (areal) 						: " + Map_GetRealCoordX(-640.61) + " " + Map_GetRealCoordZ(870.058));
	trace(GetConvertStrWithReplace("Variable_Debuger_221", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-663.685) + " " + Map_GetRealCoordZ(913.862));
	trace("Shore13 (залив Батабано)				: " + Map_GetRealCoordX(-613.56) + " " + Map_GetRealCoordZ(886.009));
	trace("Shore14 (бухта Хувентуд)				: " + Map_GetRealCoordX(-653.269) + " " + Map_GetRealCoordZ(871.013));
	trace("Shore15 (мыс Сан Антонио)			: " + Map_GetRealCoordX(-715.129) + " " + Map_GetRealCoordZ(797.212));
	trace("Mayak10 (маяк Гаваны)				: " + Map_GetRealCoordX(-647.18) + " " + Map_GetRealCoordZ(925.046));
	trace("---------");
	trace("Tortuga (island) 					: " + Map_GetRealCoordX(102.328) + " " + Map_GetRealCoordZ(852.912));
	trace(GetConvertStrWithReplace("Variable_Debuger_222", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(116.875) + " " + Map_GetRealCoordZ(817.942));
	trace("Shore58 (пляж Глаз Рыбы)				: " + Map_GetRealCoordX(131.064) + " " + Map_GetRealCoordZ(849.44));
	trace("Mayak6 (маяк Тортуги)				: " + Map_GetRealCoordX(103.23) + " " + Map_GetRealCoordZ(812.356));
	trace("---------");
	trace("Trinidad (island)					: " + Map_GetRealCoordX(889.76) + " " + Map_GetRealCoordZ(-774.746));
	trace(GetConvertStrWithReplace("Variable_Debuger_223", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(848.282) + " " + Map_GetRealCoordZ(-822.588));
	trace("Shore59 (бухта Скарборо)				: " + Map_GetRealCoordX(937.322) + " " + Map_GetRealCoordZ(-737.478));
	trace("Shore60 (мыс Галера)					: " + Map_GetRealCoordX(906.25) + " " + Map_GetRealCoordZ(-757.928));
	trace("Mayak1 (маяк Тринидада)				: " + Map_GetRealCoordX(821.24) + " " + Map_GetRealCoordZ(-847.69));
	trace("---------");
	trace("Nevis (island)						: " + Map_GetRealCoordX(681.595) + " " + Map_GetRealCoordZ(256.013));
	trace(GetConvertStrWithReplace("Variable_Debuger_224", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(658.918) + " " + Map_GetRealCoordZ(241.972));
	trace("Shore42 (бухта Ньюкасл)				: " + Map_GetRealCoordX(690.047) + " " + Map_GetRealCoordZ(291.621));
	trace("Shore43 (пляж Бат)					: " + Map_GetRealCoordX(684.026) + " " + Map_GetRealCoordZ(212.793));
	trace("Mayak5 (маяк Невиса)					: " + Map_GetRealCoordX(663.418) + " " + Map_GetRealCoordZ(228.281));
	trace("---------");
	trace("Hispaniola1 (areal) 					: " + Map_GetRealCoordX(198.977) + " " + Map_GetRealCoordZ(588.696));
	trace(GetConvertStrWithReplace("Variable_Debuger_225", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(198.683) + " " + Map_GetRealCoordZ(547.97));
	trace(GetConvertStrWithReplace("Variable_Debuger_226", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(166.708) + " " + Map_GetRealCoordZ(662.952));
	trace("Shore32 (мыс Исабела)				: " + Map_GetRealCoordX(130.591) + " " + Map_GetRealCoordZ(682.362));
	trace("Shore33 (бухта Самана)				: " + Map_GetRealCoordX(229.081) + " " + Map_GetRealCoordZ(593.727));
	trace("Mayak8 (маяк Санто-Доминго)			: " + Map_GetRealCoordX(168.314) + " " + Map_GetRealCoordZ(520.218));
	trace("---------");
	trace("Hispaniola2 (areal) 					: " + Map_GetRealCoordX(-47.1762) + " " + Map_GetRealCoordZ(625.596));
	trace(GetConvertStrWithReplace("Variable_Debuger_227", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(22.521) + " " + Map_GetRealCoordZ(592.233));
	trace("Shore34 (бухта Гонаив)				: " + Map_GetRealCoordX(0.999614) + " " + Map_GetRealCoordZ(643.122));
	trace("Mayak7 (маяк Порт-о-Принса)			: " + Map_GetRealCoordX(-14.9756) + " " + Map_GetRealCoordZ(606.258));
	trace("---------");
	trace("Dominica 							: " + Map_GetRealCoordX(783.057) + " " + Map_GetRealCoordZ(-256.833));
	trace("Shore27 (мыс Скотс Хед)				: " + Map_GetRealCoordX(779.89) + " " + Map_GetRealCoordZ(-295.002));
	trace("Shore26 (пляж Касл Брус)				: " + Map_GetRealCoordX(803.577) + " " + Map_GetRealCoordZ(-252.617));
	trace("---------");
	trace(GetConvertStrWithReplace("Variable_Debuger_228", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(443.257) + " " + Map_GetRealCoordZ(939.102));
	trace("Shore57 (залив Южный)				: " + Map_GetRealCoordX(434.944) + " " + Map_GetRealCoordZ(923.524));
	trace("Shore56 (залив Северный)				: " + Map_GetRealCoordX(444.298) + " " + Map_GetRealCoordZ(954.975));
	trace("---------");
	trace(GetConvertStrWithReplace("Variable_Debuger_229", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-489.6) + " " + Map_GetRealCoordZ(362.455));
	trace("Shore17 (залив Эсмеральды)			: " + Map_GetRealCoordX(-505.038) + " " + Map_GetRealCoordZ(348.115));
	trace("Shore16 (Пустынный пляж)				: " + Map_GetRealCoordX(-449.837) + " " + Map_GetRealCoordZ(366.308));
	trace("---------");
	trace("Panama (areal) 						: " + Map_GetRealCoordX(-395.198) + " " + Map_GetRealCoordZ(-972.801));
	trace(GetConvertStrWithReplace("Variable_Debuger_230", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-389.965) + " " + Map_GetRealCoordZ(-949.118));
	trace("Shore63 (залив Сан Мигель) 			: " + Map_GetRealCoordX(-328.166) + " " + Map_GetRealCoordZ(-1023.72));
	trace("---------");
	trace("PortoBello (areal) 					: " + Map_GetRealCoordX(-435.73) + " " + Map_GetRealCoordZ(-895.763));
	trace(GetConvertStrWithReplace("Variable_Debuger_231", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-427.183) + " " + Map_GetRealCoordZ(-907.277));
	trace("Shore46 (лагуна Чирики)				: " + Map_GetRealCoordX(-627.474) + " " + Map_GetRealCoordZ(-940.853));
	trace("Shore47 (залив Москитос)				: " + Map_GetRealCoordX(-486.49) + " " + Map_GetRealCoordZ(-976.966));
	trace("Shore48 (Дарьенский залив)			: " + Map_GetRealCoordX(-279.316) + " " + Map_GetRealCoordZ(-1005.49));
	trace("---------");
	trace("Cartahena (areal) 					: " + Map_GetRealCoordX(-51.2104) + " " + Map_GetRealCoordZ(-851.77));
	trace(GetConvertStrWithReplace("Variable_Debuger_232", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-26.7669) + " " + Map_GetRealCoordZ(-872.535));
	trace("Shore25 (бухта Ковеньяс)				: " + Map_GetRealCoordX(-115.589) + " " + Map_GetRealCoordZ(-937.387));
	trace("Mayak11 (маяк Картахены)				: " + Map_GetRealCoordX(-65.6347) + " " + Map_GetRealCoordZ(-842.072));
	trace("---------");
	trace("Caracas (areal) 						: " + Map_GetRealCoordX(435.126) + " " + Map_GetRealCoordZ(-936.197));
	trace(GetConvertStrWithReplace("Variable_Debuger_233", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(440.684) + " " + Map_GetRealCoordZ(-960.339));
	trace("Shore21 (мыс Аруба)					: " + Map_GetRealCoordX(410.688) + " " + Map_GetRealCoordZ(-913.531));
	trace("---------");
	trace("Cumana (areal) 						: " + Map_GetRealCoordX(767.999) + " " + Map_GetRealCoordZ(-936.266));
	trace(GetConvertStrWithReplace("Variable_Debuger_234", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(691.343) + " " + Map_GetRealCoordZ(-896.619));
	trace("Shore20 (пляж Лос Текес)				: " + Map_GetRealCoordX(574.327) + " " + Map_GetRealCoordZ(-929.042));
	trace("Shore19 (бухта Карупано)				: " + Map_GetRealCoordX(768.622) + " " + Map_GetRealCoordZ(-914.469));
	trace("Shore18 (берег Бока де ла Сьерпе)	: " + Map_GetRealCoordX(924.423) + " " + Map_GetRealCoordZ(-969.921));
	trace("---------");
	trace("SantaCatalina (areal) 				: " + Map_GetRealCoordX(-742.269) + " " + Map_GetRealCoordZ(-563.205));
	trace(GetConvertStrWithReplace("Variable_Debuger_235", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-769.375) + " " + Map_GetRealCoordZ(-537.051));
	trace("Shore53 (Берег Москитов)				: " + Map_GetRealCoordX(-792.718) + " " + Map_GetRealCoordZ(-331.728));
	trace("Shore54 (мыс Перлас)					: " + Map_GetRealCoordX(-760.343) + " " + Map_GetRealCoordZ(-484.106));
	trace("Shore55 (залив Сан-Хуан дель Норте)	: " + Map_GetRealCoordX(-736.763) + " " + Map_GetRealCoordZ(-756.295));
	trace("---------");
	trace(GetConvertStrWithReplace("Variable_Debuger_236", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-938.667) + " " + Map_GetRealCoordZ(938.654));
	trace("---------");
	trace("Teno (areal)							: " + Map_GetRealCoordX(-966.062) + " " + Map_GetRealCoordZ(-294.232));
	trace(GetConvertStrWithReplace("Variable_Debuger_237", "Interface.txt", "#space#", " ") + Map_GetRealCoordX(-969.194) + " " + Map_GetRealCoordZ(-305.237));
	trace("Shore61 (Тепейакак) 					: " + Map_GetRealCoordX(-955.553) + " " + Map_GetRealCoordZ(-281.197));
	trace("Shore62 (Истапалапу)					: " + Map_GetRealCoordX(-935.811) + " " + Map_GetRealCoordZ(-316.162));
	trace("---------");
}

void InitDesc()
{
	descF1 = GetConvertStrWithReplace("Variable_Debuger_7", "Interface.txt", "#space#", " ");
	descF2 = "CreateSiege";
	descF3 = "ТЕСТ: убить всех враждебных в локации (неуязвимых не трогает)";
	descF4 = GetConvertStrWithReplace("Variable_Debuger_7.2", "Interface.txt", "#space#", " ");
	descF5 = GetConvertStrWithReplace("Variable_Debuger_7.3", "Interface.txt", "#space#", " ");
	descF6 = GetConvertStrWithReplace("Variable_Debuger_9", "Interface.txt", "#space#", " ");
	descF7 = GetConvertStrWithReplace("Variable_Debuger_11", "Interface.txt", "#space#", " ");
	descF8 = GetConvertStrWithReplace("Variable_Debuger_13", "Interface.txt", "#space#", " ");
	descF9 = GetConvertStrWithReplace("Variable_Debuger_15", "Interface.txt", "#space#", " ");
	descF10 = GetConvertStrWithReplace("Variable_Debuger_15.1", "Interface.txt", "#space#", " ");
	descF11 = GetConvertStrWithReplace("Variable_Debuger_18", "Interface.txt", "#space#", " ");
	descF12 = GetConvertStrWithReplace("Variable_Debuger_20", "Interface.txt", "#space#", " ");
	descF13 = "Worldmap encounters ON/OFF";
	descF14 = "Shotgun mode ON/OFF";
	descF15 = GetConvertStrWithReplace("Variable_Debuger_23", "Interface.txt", "#space#", " ");
	descF16 = GetConvertStrWithReplace("Variable_Debuger_23.1", "Interface.txt", "#space#", " ");
	descF17 = GetConvertStrWithReplace("Variable_Debuger_23.2", "Interface.txt", "#space#", " ");
	descF18 = GetConvertStrWithReplace("Variable_Debuger_27", "Interface.txt", "#space#", " ");
	descF19 = GetConvertStrWithReplace("Variable_Debuger_27.1", "Interface.txt", "#space#", " ");
	descF20 = GetConvertStrWithReplace("Variable_Debuger_31", "Interface.txt", "#space#", " ");
	descF21 = GetConvertStrWithReplace("Variable_Debuger_31.1", "Interface.txt", "#space#", " ");
	descF22 = GetConvertStrWithReplace("Variable_Debuger_35", "Interface.txt", "#space#", " ");
	descF23 = GetConvertStrWithReplace("Variable_Debuger_44", "Interface.txt", "#space#", " ");
	descF24 = GetConvertStrWithReplace("Variable_Debuger_46", "Interface.txt", "#space#", " ");
	descF25 = GetConvertStrWithReplace("Variable_Debuger_48", "Interface.txt", "#space#", " ");
	descF26 = GetConvertStrWithReplace("Variable_Debuger_50", "Interface.txt", "#space#", " ");
	descF27 = GetConvertStrWithReplace("Variable_Debuger_50.1", "Interface.txt", "#space#", " ");
	descF28 = GetConvertStrWithReplace("Variable_Debuger_54", "Interface.txt", "#space#", " ");
	descF29 = GetConvertStrWithReplace("Variable_Debuger_56", "Interface.txt", "#space#", " ");
	descF30 = GetConvertStrWithReplace("Variable_Debuger_56.1", "Interface.txt", "#space#", " ");
	descF31 = GetConvertStrWithReplace("Variable_Debuger_56.2", "Interface.txt", "#space#", " ");
	descF32 = GetConvertStrWithReplace("Variable_Debuger_60", "Interface.txt", "#space#", " ");
	descF33 = GetConvertStrWithReplace("Variable_Debuger_60.1", "Interface.txt", "#space#", " ");
	descF34 = GetConvertStrWithReplace("Variable_Debuger_60.2", "Interface.txt", "#space#", " ");
	descF35 = GetConvertStrWithReplace("Variable_Debuger_60.3", "Interface.txt", "#space#", " ");
	descF36 = GetConvertStrWithReplace("Variable_Debuger_65", "Interface.txt", "#space#", " ");
	descF37 = GetConvertStrWithReplace("Variable_Debuger_67", "Interface.txt", "#space#", " ");
	descF38 = GetConvertStrWithReplace("Variable_Debuger_67.1", "Interface.txt", "#space#", " ");
	descF39 = GetConvertStrWithReplace("Variable_Debuger_73", "Interface.txt", "#space#", " ");
	descF40 = GetConvertStrWithReplace("Variable_Debuger_77", "Interface.txt", "#space#", " ");
	descF41 = GetConvertStrWithReplace("Variable_Debuger_79", "Interface.txt", "#space#", " ");
	descF42 = GetConvertStrWithReplace("Variable_Debuger_81", "Interface.txt", "#space#", " "); // проверка ликвидности товара - цена-вес, что выгоднее - выводим в компил.лог список, строим заполняя трюм товаром по одной пачке
	descF43 = GetConvertStrWithReplace("Variable_Debuger_83", "Interface.txt", "#space#", " ");
	descF44 = GetConvertStrWithReplace("Variable_Debuger_85", "Interface.txt", "#space#", " ");
	descF45 = GetConvertStrWithReplace("Variable_Debuger_87", "Interface.txt", "#space#", " ");
	descF46 = GetConvertStrWithReplace("Variable_Debuger_89", "Interface.txt", "#space#", " ");
	descF47 = GetConvertStrWithReplace("Variable_Debuger_91", "Interface.txt", "#space#", " ");
	descF48 = GetConvertStrWithReplace("Variable_Debuger_93", "Interface.txt", "#space#", " ");
	descF49 = GetConvertStrWithReplace("Variable_Debuger_95", "Interface.txt", "#space#", " ");
	descF50 = GetConvertStrWithReplace("Variable_Debuger_95.1", "Interface.txt", "#space#", " ");
	descF51 = GetConvertStrWithReplace("Variable_Debuger_103", "Interface.txt", "#space#", " ");
	descF52 = GetConvertStrWithReplace("Variable_Debuger_105", "Interface.txt", "#space#", " ");
	descF53 = GetConvertStrWithReplace("Variable_Debuger_105.1", "Interface.txt", "#space#", " ");
	descF54 = GetConvertStrWithReplace("Variable_Debuger_108", "Interface.txt", "#space#", " ");
	descF55 = GetConvertStrWithReplace("Variable_Debuger_130", "Interface.txt", "#space#", " ");
	descF56 = GetConvertStrWithReplace("Variable_Debuger_132", "Interface.txt", "#space#", " ");
	descF57 = GetConvertStrWithReplace("Variable_Debuger_134", "Interface.txt", "#space#", " ");
	descF58 = "ТЕСТ: создать 10 пленников в трюм (бунт пленников)";
	descF59 = GetConvertStrWithReplace("DbgRemovePerks", "Interface.txt", "#space#", " ");
	descF60 = "7 компаньонов для теста штурма (линкоры 1 класса, полный экипаж и трюм, перки)";
	descF61 = GetConvertStrWithReplace("Variable_Debuger_7.1", "Interface.txt", "#space#", " ");
	descF62 = "ДИАГНОСТИКА: чем заняты слоты персонажей (толщина сейва)";
	descF63 = GetConvertStrWithReplace("Variable_Debuger_146", "Interface.txt", "#space#", " ");
	descF64 = GetConvertStrWithReplace("Variable_Debuger_148", "Interface.txt", "#space#", " ");
	descF65 = GetConvertStrWithReplace("Variable_Debuger_150", "Interface.txt", "#space#", " ");
	descF66 = GetConvertStrWithReplace("Variable_Debuger_152", "Interface.txt", "#space#", " ");
	descF67 = GetConvertStrWithReplace("Variable_Debuger_154", "Interface.txt", "#space#", " ");
	descF68 = GetConvertStrWithReplace("Variable_Debuger_156", "Interface.txt", "#space#", " ");
	descF69 = GetConvertStrWithReplace("Variable_Debuger_158", "Interface.txt", "#space#", " ");
	descF70 = GetConvertStrWithReplace("Variable_Debuger_160", "Interface.txt", "#space#", " ");
	descF71 = GetConvertStrWithReplace("Variable_Debuger_162", "Interface.txt", "#space#", " ");
	descF72 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF73 = GetConvertStrWithReplace("Variable_Debuger_166", "Interface.txt", "#space#", " ");
	descF74 = GetConvertStrWithReplace("Variable_Debuger_168", "Interface.txt", "#space#", " ");
	descF75 = GetConvertStrWithReplace("Variable_Debuger_170", "Interface.txt", "#space#", " ");
	descF76 = GetConvertStrWithReplace("Variable_Debuger_172", "Interface.txt", "#space#", " ");
	descF77 = GetConvertStrWithReplace("Variable_Debuger_174", "Interface.txt", "#space#", " ");
	descF78 = GetConvertStrWithReplace("Variable_Debuger_176", "Interface.txt", "#space#", " ");
	descF79 = GetConvertStrWithReplace("Variable_Debuger_178", "Interface.txt", "#space#", " ");
	descF80 = GetConvertStrWithReplace("Variable_Debuger_180", "Interface.txt", "#space#", " ");
	descF81 = GetConvertStrWithReplace("Variable_Debuger_182", "Interface.txt", "#space#", " ");
	descF82 = GetConvertStrWithReplace("Variable_Debuger_184", "Interface.txt", "#space#", " ");
	descF83 = GetConvertStrWithReplace("Variable_Debuger_186", "Interface.txt", "#space#", " ");
	descF84 = GetConvertStrWithReplace("Variable_Debuger_188", "Interface.txt", "#space#", " ");
	descF85 = GetConvertStrWithReplace("Variable_Debuger_190", "Interface.txt", "#space#", " ");
	descF86 = GetConvertStrWithReplace("Variable_Debuger_192", "Interface.txt", "#space#", " ");
	descF87 = GetConvertStrWithReplace("Variable_Debuger_194", "Interface.txt", "#space#", " ");
	descF88 = GetConvertStrWithReplace("Variable_Debuger_196", "Interface.txt", "#space#", " ");
	descF89 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF90 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF91 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF92 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF93 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF94 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF95 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF96 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF97 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF98 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF99 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");
	descF100 = GetConvertStrWithReplace("Variable_Debuger_164", "Interface.txt", "#space#", " ");	
}
