#include "storm-engine\battle_interface\log_msg.h"

#define LOG_FOR_SEA			1
#define LOG_FOR_LAND		2
#define LOG_FOR_WORLDMAP	3

object showWindow;
object ILogAndActions;
object IBoardingStatus;
bool   bYesBoardStatus;
string g_ActiveActionName;

#event_handler("blieGetMsgIconRoot","BI_GetMsgIconRoot");
void InitLogInterface()
{
	SetEventHandler("SetWindowSize","LI_SetWindowSize",0);
	CreateEntity(&ILogAndActions,"ILogAndActions");
	DelEventHandler("SetWindowSize","LI_SetWindowSize");
	DeleteAttribute(&ILogAndActions,"");
	ILogAndActions.type = "";
	CreateLogEnvironment();
	SendMessage(&ILogAndActions,"lll",LOG_AND_ACTIONS_INIT,sti(InterfaceStates.BattleShow.FastCommand),sti(InterfaceStates.BattleShow.LogString));
	SetEventHandler(EVENT_LOCATION_LOAD,"LI_LocationLoad",0);
	SetEventHandler("BI_FastCommand","BI_FastCommand",0);
	LayerAddObject(EXECUTE,&ILogAndActions,-257);
	LayerAddObject(REALIZE,&ILogAndActions,-1);
	LayerAddObject(SEA_EXECUTE,&ILogAndActions,-257);
	LayerAddObject(SEA_REALIZE,&ILogAndActions,-1);

	InitActivePerkShower();
}

void Log_RemoveFromSingleLayers()
{
	LayerDelObject(EXECUTE,&ILogAndActions);
	LayerDelObject(REALIZE,&ILogAndActions);
	LayerDelObject(SEA_EXECUTE,&ILogAndActions);
	LayerDelObject(SEA_REALIZE,&ILogAndActions);
}

void Log_MoveToSingleLayers()
{
	LayerAddObject(EXECUTE,&ILogAndActions,-257);
	LayerAddObject(REALIZE,&ILogAndActions,-1);
	LayerAddObject(SEA_EXECUTE,&ILogAndActions,-257);
	LayerAddObject(SEA_REALIZE,&ILogAndActions,-1);
}

void LI_SetWindowSize()
{
	int w = GetEventData();
	int h = GetEventData();
	int bTVused = GetEventData();
	if(bTVused)	SetShowWindowParameters(bTVused,w,h,40,24,w-40,h-24);
	else	SetShowWindowParameters(bTVused,w,h,0,0,w,h);
}

// LEO: показать/скрыть весь лог. Нужно катсцене: сообщения вроде «В энциклопедию добавлена
//   новая запись» висели поверх чёрных полос кино-режима вместе с HUD.
void Log_SetVisible(bool bVisible)
{
	if(!IsEntity(&ILogAndActions)) return;
	SendMessage(&ILogAndActions,"ll",LI_SET_VISIBLE,bVisible);
}

void LI_LocationLoad()
{
	CreateILogAndActions(LOG_FOR_LAND);

	if(questMovieProcess > 0) Log_SetVisible(false);
}

void InitBattleInterfacesParameters()
{
	DeleteClass(&IBoardingStatus);
	DeleteAttribute(&IBoardingStatus,"");
	bYesBoardStatus = false;
}

void CreateILogAndActions(int loadType)
{
	DeleteAttribute(&ILogAndActions,"ActiveActions");
	if(loadType==LOG_FOR_SEA)
	{
		CreateSeaActionsEnvironment();
	}
	if(loadType==LOG_FOR_LAND)
	{
		CreateLandActionsEnvironment();
	}
	if(loadType==LOG_FOR_WORLDMAP)
	{
		CreateWorldMapActionsEnvironment();
	}
	SendMessage(&ILogAndActions,"lll",LOG_AND_ACTIONS_CHANGE,sti(InterfaceStates.BattleShow.FastCommand),sti(InterfaceStates.BattleShow.LogString));
	Log_SetActiveAction(g_ActiveActionName);
}

// LEO: F3 не имеет hover; используем отдельную совместимую сетку статичных состояний.
string Log_CommandIconsTexture()
{
	if(!CheckAttribute(&ILogAndActions,"type")) return "";
	if(ILogAndActions.type=="land") return CommandIcons_StaticTexture("LandCommands");
	if(ILogAndActions.type=="sea") return CommandIcons_StaticTexture("List_icons");
	if(ILogAndActions.type=="map") return CommandIcons_StaticTexture("WorldMapCommands");
	return "";
}

void Log_RefreshCommandIcons()
{
	if(!IsEntity(&ILogAndActions)) return;
	string texture=Log_CommandIconsTexture();
	if(texture=="") return;
	if(ILogAndActions.type=="sea") CreateSeaActionsEnvironment();
	if(ILogAndActions.type=="land") CreateLandActionsEnvironment();
	ILogAndActions.ActiveActions.TextureName=texture;
	SendMessage(&ILogAndActions,"lll",LOG_AND_ACTIONS_CHANGE,sti(InterfaceStates.BattleShow.FastCommand),sti(InterfaceStates.BattleShow.LogString));
	Log_SetActiveAction(g_ActiveActionName);
}

void Log_SetActiveAction(string actionName)
{
	if( ILogAndActions.type=="sea" && g_ActiveActionName!=actionName ) {

		if (actionName != "Moor")
			pchar.MoorName = " ";
		RefreshBattleInterface();
	}
	g_ActiveActionName = actionName;

	if( CheckAttribute(&ILogAndActions,"ActiveActions."+actionName+".Text") ) {
		ILogAndActions.ActiveActions.text2.text = ILogAndActions.ActiveActions.(actionName).Text;
	} else {
		ILogAndActions.ActiveActions.text2.text = XI_ConvertString("for_quick_action");
	}
	SendMessage(&ILogAndActions,"ls",LOG_SET_ACTIVE_ACTION,actionName);
}

void Log_SetStringToLog(string strLog)
{
	SendMessage(&ILogAndActions,"lls",LOG_ADD_STRING, false, strLog);
	if (bGameLogSkip == 0) GameLog_Record(strLog); // LEO: в Журнал игры — всё, кроме Log_TestInfo (он ставит bGameLogSkip)
}

// LEO: строка лога заданным цветом (color — RGB через argb(0,r,g,b); alpha лог проставляет сам)
void Log_SetStringToLogColor(int color, string strLog)
{
	SendMessage(&ILogAndActions,"lllls",LOG_ADD_STRING_COLOR, false, color, 0, strLog);
	if (bGameLogSkip == 0) GameLog_Record(strLog); // LEO: цветные строки тоже в Журнал игры
}

// LEO: цвет + анимация (белая вспышка -> основной цвет -> затухание в тёмный, с шейком на вспышке) — для денежных сумм
void Log_SetStringToLogColorAnim(int color, string strLog)
{
	SendMessage(&ILogAndActions,"lllls",LOG_ADD_STRING_COLOR, false, color, 1, strLog);
	if (bGameLogSkip == 0) GameLog_Record(strLog); // LEO: деньги (приход/расход) в Журнал игры
}

// LEO: удобный лог цветной строки ПО ИМЕНИ ЦВЕТА — для любых скриптов: Log_InfoColored("green", "текст").
// Имена: green/red/yellow/blue/violet/orange/white. anim=true -> с анимацией (вспышка+шейк), как у денег.
int GameLogColorByName(string sColor)
{
	if (sColor == "green")  return argb(0, 90, 220, 90);
	if (sColor == "red")    return argb(0, 230, 90, 90);
	if (sColor == "yellow") return argb(0, 251, 237, 68);
	if (sColor == "blue")   return argb(0, 0, 255, 255);
	if (sColor == "violet") return argb(0, 255, 20, 147);
	if (sColor == "orange") return argb(0, 255, 160, 60);
	return argb(0, 255, 255, 255); // white по умолчанию
}

void Log_InfoColored(string sColor, string strLog)
{
	Log_SetStringToLogColor(GameLogColorByName(sColor), strLog);
}

void Log_InfoColoredAnim(string sColor, string strLog)
{
	Log_SetStringToLogColorAnim(GameLogColorByName(sColor), strLog);
}

void Log_SetEternalString(string strLog)
{
	SendMessage(&ILogAndActions,"lls",LOG_ADD_STRING, true, strLog);
}

void CreateLogEnvironment()
{

    float fHtRatio = stf(Render.screen_y) / screenscaling;
	ILogAndActions.Log.width = sti(showWindow.sw)/2;
	ILogAndActions.Log.height = sti(showWindow.sh)- makeint(80 * fHtRatio);
	ILogAndActions.Log.right = sti(showWindow.right) - RecalculateHIcon(makeint(340 * fHtRatio));
	ILogAndActions.Log.up = sti(showWindow.top) + makeint(16 * fHtRatio);
	ILogAndActions.Log.font = "interface_normal";
	ILogAndActions.Log.fontscale = 1.5 * fHtRatio;
	ILogAndActions.Log.color = argb(0,255,255,255);
	ILogAndActions.Log.offsetString = makeint(24 * fHtRatio);
	ILogAndActions.Log.speed = 0.05;

	ILogAndActions.Log.color_speed = 0.085;
	ILogAndActions.Log.hold = 10.0;
}

void CreateSeaActionsEnvironment()
{

    float fHtRatio = stf(Render.screen_y) / screenscaling;
	ILogAndActions.type = "sea";
	ILogAndActions.ActiveActions.TextureName = CommandIcons_StaticTexture("List_icons");
	ILogAndActions.ActiveActions.horzQ = 16;
	ILogAndActions.ActiveActions.vertQ = 8;
	ILogAndActions.ActiveActions.width = RecalculateHIcon(makeint(48 * fHtRatio));
	ILogAndActions.ActiveActions.height = RecalculateVIcon(makeint(48 * fHtRatio));
	ILogAndActions.ActiveActions.left = sti(showWindow.right) - RecalculateHIcon(makeint(296 * fHtRatio));
	ILogAndActions.ActiveActions.top = sti(showWindow.top) + RecalculateVIcon(makeint(32 * fHtRatio));

	ILogAndActions.ActiveActions.text1.font = "interface_normal";
	ILogAndActions.ActiveActions.text1.scale = 1.0 * fHtRatio;
	ILogAndActions.ActiveActions.text1.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(272 * fHtRatio));
	ILogAndActions.ActiveActions.text1.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(86 * fHtRatio));
	ILogAndActions.ActiveActions.text1.text = XI_ConvertString("Press_F3");
	ILogAndActions.ActiveActions.text2.font = "interface_normal";
	ILogAndActions.ActiveActions.text2.scale = 1.0 * fHtRatio;
	ILogAndActions.ActiveActions.text2.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(272 * fHtRatio));
	ILogAndActions.ActiveActions.text2.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(86 * fHtRatio));
	ILogAndActions.ActiveActions.text2.text = XI_ConvertString("for_quick_action");

	ILogAndActions.ActiveActions.Moor.IconNum		= 29;
	ILogAndActions.ActiveActions.Moor.Text			= XI_ConvertString("for_quick_action_Moor");
	ILogAndActions.ActiveActions.Board.IconNum		= 30;
	ILogAndActions.ActiveActions.Board.Text			= XI_ConvertString("for_quick_action_Board");
	ILogAndActions.ActiveActions.LandTroops.IconNum	= 42;
	ILogAndActions.ActiveActions.LandTroops.Text	= XI_ConvertString("for_quick_action_LandTroops");
	ILogAndActions.ActiveActions.Map.IconNum		= 28;
	ILogAndActions.ActiveActions.Map.Text			= XI_ConvertString("for_quick_action_Map");
	ILogAndActions.ActiveActions.Reload.IconNum		= 48;
	ILogAndActions.ActiveActions.Reload.Text		= XI_ConvertString("for_quick_action_Reload_Sea");
	ILogAndActions.ActiveActions.DirSail.IconNum	= 60; // Gregg: доплытие как быстрое действие (иконка как у команды DirSail)
	ILogAndActions.ActiveActions.DirSail.Text		= XI_ConvertString("for_quick_action_DirSail");
	ILogAndActions.ActiveActions.Nothing.IconNum	= -1;
	ILogAndActions.ActiveActions.Nothing.Text		= XI_ConvertString("for_quick_action");
	aref actionLayout;
	makearef(actionLayout,ILogAndActions.ActiveActions);
	NewSeaHUD_SetQuickAction(actionLayout,&BattleInterface,fHtRatio);
}

void CreateLandActionsEnvironment()
{

    float fHtRatio = stf(Render.screen_y) / screenscaling;
	ILogAndActions.type = "land";

	ILogAndActions.ActiveActions.TextureName = CommandIcons_StaticTexture("LandCommands");
	ILogAndActions.ActiveActions.horzQ = 16;
	ILogAndActions.ActiveActions.vertQ = 4;
	ILogAndActions.ActiveActions.width = RecalculateHIcon(makeint(48 * fHtRatio));
	ILogAndActions.ActiveActions.height = RecalculateVIcon(makeint(48 * fHtRatio));
	// LEO: Общая ось для значка и подписей; отступ считаем от панели с учётом ширины подписи.
	int hintWidth = RecalculateHIcon(makeint(96 * fHtRatio));
	int actionX = BLI_InfoPanelLeft(fHtRatio) - RecalculateHIcon(makeint(12 * fHtRatio)) - (hintWidth + 1) / 2;
	ILogAndActions.ActiveActions.left = actionX - sti(ILogAndActions.ActiveActions.width) / 2;
	ILogAndActions.ActiveActions.top = sti(showWindow.top) + RecalculateVIcon(makeint(32 * fHtRatio));

	ILogAndActions.ActiveActions.text1.font = "interface_normal";
	ILogAndActions.ActiveActions.text1.scale = 1.0 * fHtRatio;
	ILogAndActions.ActiveActions.text1.maxWidth = hintWidth;
	ILogAndActions.ActiveActions.text1.maxLines = 1;
	ILogAndActions.ActiveActions.text1.pos.x = actionX;
	ILogAndActions.ActiveActions.text1.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(86 * fHtRatio));
	ILogAndActions.ActiveActions.text1.text = XI_ConvertString("Press_F3");
	ILogAndActions.ActiveActions.text2.font = "interface_normal";
	ILogAndActions.ActiveActions.text2.scale = 1.0 * fHtRatio;
	ILogAndActions.ActiveActions.text2.maxWidth = hintWidth;
	ILogAndActions.ActiveActions.text2.maxLines = 1;
	ILogAndActions.ActiveActions.text2.pos.x = actionX;
	ILogAndActions.ActiveActions.text2.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(86 * fHtRatio));
	ILogAndActions.ActiveActions.text2.text = XI_ConvertString("for_quick_action");

	ILogAndActions.ActiveActions.ToSea.IconNum		= 13;
	ILogAndActions.ActiveActions.ToSea.Text			= XI_ConvertString("for_quick_action_ToSea");
	ILogAndActions.ActiveActions.Talk.IconNum		= 17;
	ILogAndActions.ActiveActions.Talk.Text			= XI_ConvertString("for_quick_action_Talk");
	ILogAndActions.ActiveActions.Look.IconNum		= 16;
	ILogAndActions.ActiveActions.Look.Text			= XI_ConvertString("for_quick_action_Look");
	ILogAndActions.ActiveActions.Pick.IconNum		= 16;
	ILogAndActions.ActiveActions.Pick.Text			= XI_ConvertString("for_quick_action_Pick");
	ILogAndActions.ActiveActions.Action.IconNum		= 16;
	ILogAndActions.ActiveActions.Action.Text		= XI_ConvertString("for_quick_action_Action");
	ILogAndActions.ActiveActions.OpenBox.IconNum	= 16;
	ILogAndActions.ActiveActions.OpenBox.Text		= XI_ConvertString("for_quick_action_OpenBox");
	ILogAndActions.ActiveActions.DeadBox.IconNum	= 31;
	ILogAndActions.ActiveActions.DeadBox.Text		= XI_ConvertString("for_quick_action_DeadBox");
	ILogAndActions.ActiveActions.Reload.IconNum		= 13;
	ILogAndActions.ActiveActions.Reload.Text		= XI_ConvertString("for_quick_action_Reload");
	ILogAndActions.ActiveActions.Closed.IconNum		= 29;
	ILogAndActions.ActiveActions.Closed.Text		= XI_ConvertString("for_quick_action_Closed");
	ILogAndActions.ActiveActions.Nothing.IconNum	= -1;
	ILogAndActions.ActiveActions.Nothing.Text		= XI_ConvertString("for_quick_action");
}

void CreateWorldMapActionsEnvironment()
{
	bYesBoardStatus=false;
	DeleteClass(&IBoardingStatus);
	DeleteAttribute(&IBoardingStatus,"");

    float fHtRatio = stf(Render.screen_y) / screenscaling;
	ILogAndActions.type = "map";

	ILogAndActions.ActiveActions.TextureName = CommandIcons_StaticTexture("WorldMapCommands");
	ILogAndActions.ActiveActions.horzQ = 8;
	ILogAndActions.ActiveActions.vertQ = 2;
	ILogAndActions.ActiveActions.width = RecalculateHIcon(makeint(48 * fHtRatio));
	ILogAndActions.ActiveActions.height = RecalculateVIcon(makeint(48 * fHtRatio));
	ILogAndActions.ActiveActions.left = sti(showWindow.right) - RecalculateHIcon(makeint(284 * fHtRatio));
	ILogAndActions.ActiveActions.top = sti(showWindow.top) + RecalculateVIcon(makeint(32 * fHtRatio));

	ILogAndActions.ActiveActions.text1.font = "interface_normal";
	ILogAndActions.ActiveActions.text1.scale = 1.0 * fHtRatio;
	ILogAndActions.ActiveActions.text1.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(260 * fHtRatio));
	ILogAndActions.ActiveActions.text1.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(86 * fHtRatio));
	ILogAndActions.ActiveActions.text1.text = XI_ConvertString("Press_F3");
	ILogAndActions.ActiveActions.text2.font = "interface_normal";
	ILogAndActions.ActiveActions.text2.scale = 1.0 * fHtRatio;
	ILogAndActions.ActiveActions.text2.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(260 * fHtRatio));
	ILogAndActions.ActiveActions.text2.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(86 * fHtRatio));
	ILogAndActions.ActiveActions.text2.text = XI_ConvertString("for_quick_action");

	ILogAndActions.ActiveActions.EnterToSea.IconNum	= 1;
	ILogAndActions.ActiveActions.EnterToSea.Text = XI_ConvertString("for_quick_action_EnterToSea");
	ILogAndActions.ActiveActions.EnterToIsland.IconNum = 0;
	ILogAndActions.ActiveActions.EnterToIsland.Text = XI_ConvertString("for_quick_action_EnterToIsland");
	ILogAndActions.ActiveActions.EnterToShip.IconNum = 4;
	ILogAndActions.ActiveActions.EnterToShip.Text = XI_ConvertString("for_quick_action_EnterToShip");
	ILogAndActions.ActiveActions.EnterToStorm.IconNum = 5;
	ILogAndActions.ActiveActions.EnterToStorm.Text = XI_ConvertString("for_quick_action_EnterToStorm");
	ILogAndActions.ActiveActions.EnterToAttack.IconNum = 2;
	ILogAndActions.ActiveActions.EnterToAttack.Text = XI_ConvertString("for_quick_action_EnterToAttack");
	ILogAndActions.ActiveActions.EnterToEnemy.IconNum	= 3;
	ILogAndActions.ActiveActions.EnterToEnemy.Text = XI_ConvertString("for_quick_action_EnterToEnemy");
}

// LEO: Единые цвета команд для числа и заливки; у старой шкалы сохраняем заданную прозрачность.
void Log_SetBoardingTeamColors(int enemyAlpha)
{
	IBoardingStatus.myColor=argb(255,0,255,0);
	IBoardingStatus.enemyColor=argb(enemyAlpha,255,0,0);
}

void Log_RefreshCrewBars(float ratio)
{
	if(!IsEntity(&IBoardingStatus)) return;
	if(!CheckAttribute(&IBoardingStatus,"crewHud")) return;
	if(stf(IBoardingStatus.layoutX)==LandHUD_GetOption("LandCrewBarsX") && stf(IBoardingStatus.layoutY)==LandHUD_GetOption("LandCrewBarsY")) return;
	BLI_LayoutCrewBars(ratio);
	SendMessage(&IBoardingStatus,"l",LOG_INIT_CHARCTER_HP);
}

void DrawCharacterHP(float myFrac,float enemyFrac,int myCount,int enemyCount)
{
	if(!bYesBoardStatus)
	{
		CreateEntity(&IBoardingStatus,"IBoardingStatus");
		LayerAddObject(REALIZE, &IBoardingStatus, -1);
		BLI_LayoutCrewBars(stf(Render.screen_y)/screenscaling);
		SendMessage(&IBoardingStatus,"l",LOG_INIT_CHARCTER_HP);
		bYesBoardStatus = true;
	}

	IBoardingStatus.myCount=myCount;
	IBoardingStatus.enemyCount=enemyCount;
	SendMessage(&IBoardingStatus,"lff",LOG_SET_CHARCTER_HP,myFrac,enemyFrac);
}

void DrawCharacterHPEx(float myHP,float enemyHP)
{
	float fHtRatio = stf(Render.screen_y) / screenscaling;
    int fTmp, fTmp2;

    if(bYesBoardStatus==false)
    {
        CreateEntity(&IBoardingStatus,"IBoardingStatus");
		LayerAddObject(REALIZE, &IBoardingStatus, 1000);
        fTmp = makeint(24.0 * fHtRatio);
        IBoardingStatus.myLeft = fTmp + "," + fTmp2;
        fTmp = makeint(86.0 * fHtRatio);
        IBoardingStatus.myTop = fTmp + "," + fTmp2;
		IBoardingStatus.enemyLeft = sti(showWindow.left)+230+73+makeint((14.0/1280.0*stf(showWindow.right)-4.0)/2.0);
		IBoardingStatus.enemyTop = -1000;
		IBoardingStatus.height = RecalculateVIcon(6)*fHtRatio;
		IBoardingStatus.width = RecalculateVIcon(132)*fHtRatio;
		Log_SetBoardingTeamColors(255);
		SendMessage(&IBoardingStatus,"l",LOG_INIT_CHARCTER_HP);
		bYesBoardStatus = true;
	}

	if(enemyHP <=0)
	{
		bYesBoardStatus=false;
		DeleteClass(&IBoardingStatus);
		DeleteAttribute(&IBoardingStatus,"");
	}

	SendMessage(&IBoardingStatus,"lff",LOG_SET_CHARCTER_HP,myHP,enemyHP);
}

void DrawCharacterHPExx(float myHP,float enemyHP)
{
	float fHtRatio = stf(Render.screen_y) / screenscaling;
    int fTmp, fTmp2;

    if(bYesBoardStatus==false)
    {
        CreateEntity(&IBoardingStatus,"IBoardingStatus");
		LayerAddObject(REALIZE, &IBoardingStatus, 1000);
        fTmp = makeint(41 * fHtRatio);
        IBoardingStatus.myLeft = fTmp + "," + fTmp2;
        IBoardingStatus.enemyLeft = fTmp + "," + fTmp2;
        fTmp = makeint(79.5 * fHtRatio);
        IBoardingStatus.myTop = fTmp + "," + fTmp2;
		IBoardingStatus.enemyTop = fTmp + "," + fTmp2;
        IBoardingStatus.height = RecalculateVIcon(10)*fHtRatio;
        IBoardingStatus.width = RecalculateVIcon(110)*fHtRatio;
        Log_SetBoardingTeamColors(55);
		SendMessage(&IBoardingStatus,"l",LOG_INIT_CHARCTER_HP);
		bYesBoardStatus = true;
	}

	if(enemyHP <=0)
	{
		bYesBoardStatus=false;
		DeleteClass(&IBoardingStatus);
		DeleteAttribute(&IBoardingStatus,"");
	}

	SendMessage(&IBoardingStatus,"lff",LOG_SET_CHARCTER_HP,myHP,enemyHP);
}

void BI_FastCommand()
{
	int tmpi;
	string FComName = GetEventData();

	if( LAi_IsDead(pchar) ) {return;}

	aref arFader;
	if( GetEntity(arFader,"fader") ) {return;}

	if( Sea_InTacticalView() && objControlsState.keygroups.primaryland.ChrAction == "vk_lbutton" )
	{
		Sea_ToggleTacticalPause();
		return;
	}

	bool bEC = false;

	if( ILogAndActions.type == "sea" )
	{
		switch(FComName)
		{
		case "Moor":		bEC = true; Sea_LandLoad(); break;
		case "Board":		bEC = true; Sea_AbordageLoad(SHIP_ABORDAGE,true); break;
		case "LandTroops":	bEC = true; Sea_AbordageLoad(FORT_ABORDAGE,true); break;
		case "Map":			bEC = true; Sea_MapLoad(); break;
		case "Reload":
			if(bi_nReloadTarget!=-1)
			{
				bEC = true;
				LaunchTransferMain(pchar,GetCharacter(bi_nReloadTarget),"Transfer");
			}
		break;
		case "DirSail": // Gregg: доплытие быстрым действием (F3), гейт опцией DirectSail
			if(sti(InterfaceStates.DirectSail) && bSeaActive && !bAbordageStarted)
			{
				bEC = true;
				CheckIslandChange(1);
			}
		break;
		}
	}
	if( ILogAndActions.type == "land" )
	{
		if(CheckAttribute(pchar,"Quests.quest_act"))
		{
			Event("evntQuestComplete","s",Characters[nMainCharacterIndex].Quests.quest_act);
			return;
		}
		switch(FComName)
		{
			case "OpenBox":
				if (!LAi_IsFightMode(pchar)) // в бою быстрой командой низя открыть сундук
				{
					bEC = true;
					OpenBoxProcedure();
				}
			break;
			case "Pick": bEC = true; Item_OnPickItem(); break;
			case "Action": bEC = true; Item_OnUseItem(); break;
			case "Talk":
				bEC = true;
				tmpi = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"FindDialogCharacter");
				if(tmpi>=0)	{Event("dlgReady","l",tmpi);}
			break;
			case "Reload":
				bEC = true;
				if (bCabinStarted)
	            {

	                BOAL_ReloadToLoc(chrWaitReloadRef, chrWaitReloadLocator);
	            }
	            else
					Event("Control Activation","s","ChrAction");
			break;
			case "ToSea": bEC = true; Event("Control Activation","s","ChrAction"); break;
		}
	}
	if( ILogAndActions.type == "map" )
	{
		switch(FComName)
		{
		case "EnterToSea":
			bEC = true;
			SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
			break;
		case "EnterToIsland":
			bEC = true;
			SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
			break;
		case "EnterToShip":
			bEC = true;
			SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
			break;
		case "EnterToStorm":
			bEC = true;
			SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
			break;
		case "EnterToAttack":
			bEC = true;
			SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
			break;
		case "EnterToEnemy":
			bEC = true;
			SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
			break;
		}
	}

	if( bEC )
	{
		g_ActiveActionName = "";
		SendMessage(&ILogAndActions,"ls",LOG_SET_ACTIVE_ACTION,"");
	}
}

void ClearAllLogStrings()
{
	SendMessage(&ILogAndActions,"l",LI_CLEAR_STRINGS);
}

void BI_GetMsgIconRoot()
{
	aref arTmp;
	aref pARef[4];
	int i,idx,cn;
	idx = 0;

	if(bSeaActive && !bAbordageStarted)
	{
		for(i=0; i<COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(pchar,i);
			if(cn<0) {continue;}
			Characters[cn].MessageIcons = true;
			makearef(arTmp,Characters[cn].MessageIcons);
			pARef[idx] = arTmp;
			idx++;
		}
		SendMessage(&BattleInterface,"le", BI_MSG_SET_MSG_ICONS, &pARef);
	}

	else
	{
		for(i=0; i<4; i++)
		{
			cn = GetOfficersIndex(pchar,i);
			if(cn<0) {continue;}
			Characters[cn].MessageIcons = true;
			makearef(arTmp,Characters[cn].MessageIcons);
			pARef[idx] = arTmp;
			idx++;
		}
		SendMessage(&objLandInterface,"le", MSG_BATTLE_LAND_SET_MSGICONS, &pARef);
	}
}
void LoadLIStates()
{
	pchar = GetMainCharacter();//нужно, чтобы не ругалось
	if(!CheckAttribute(pchar, "id")) return;
	int i,cn;
	ref chref;

	DeleteAttribute(pchar,"MessageIcons");
	for(i=1; i<COMPANION_MAX; i++)
	{
		cn = GetOfficersIndex(pchar,i);
		if(cn>=0)
		{
			chref = GetCharacter(cn);
			DeleteAttribute(chref,"MessageIcons");
		}
		cn = GetCompanionIndex(pchar,i);
		if(cn>=0)
		{
			chref = GetCharacter(cn);
			DeleteAttribute(chref,"MessageIcons");
		}
	}
}
