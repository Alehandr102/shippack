#include "storm-engine\battle_interface\msg_control.h"
#include "storm-engine\sea_ai\script_defines.h"
#include "battle_interface\NewHUD.c"
#include "battle_interface\HUDLayout.c"
#include "battle_interface\NewSeaHUD.c"
#include "battle_interface\CommandIcons.c"
#include "battle_interface\landinterface.c"
#include "battle_interface\ispyglass.c"
#include "battle_interface\reload_tables.c"
#include "battle_interface\utils.c"
#include "battle_interface\ActivePerksShow.c"
#include "battle_interface\backgroundtask.c"
#include "battle_interface\WmInterface.c"
#include "battle_interface\BattleInterfaceUpdates.c"
#include "battle_interface\ShipInfoBars.c"
#include "battle_interface\ShipTowHUD.c"

#define BI_ICONS_SHIPS_TEXTURE_NAME "battle_interface\icons.dds"

#define BI_ICONS_ST_NONE		0
#define BI_ICONS_ST_MYSHIPS		1
#define BI_ICONS_ST_COMMAND		2
#define BI_ICONS_ST_TARGETING		3

#define BI_ICONS_TEXTURE_COMMAND	0
#define BI_ICONS_TEXTURE_SHIP1		1
#define BI_ICONS_TEXTURE_ABILITY	2

#define SAILDIR_POS_DIF             10000.0
#define FLAG_ENSIGN    "E"
#define FLAG_SPECIAL   "S"
#define FLAG_NONE      "N"

#define SHIP_FLAG      -2
#define SHIP_SFLAG     -3

#define KRUGLYI		0
#define KVADRAT		1
#define POTC		2

#event_handler("BI_GetCmdListOrderPriority", "BI_GetCmdListOrderPriority");

int bi_icons_ShowType;
int bi_icons_CommandMode;

int bi_idxSelectedPicture;

object BattleInterface;

bool bDisableSailTo = false;

bool bDisableMapEnter = false;

bool bReloadCanBe = false;
bool bMapEnter = false;
bool bSailTo=false;
bool bAttack=false;
bool bAbordage = false;
bool bDefend=false;

bool bCanSpeak = false;

bool bEnableIslandSailTo = false;

int BI_intRetValue;
int BI_retComValue;
int BI_ChargeState[5];
float BI_g_fRetVal;

int BI_intNRetValue[8];

object BI_objRetValue;
object objShipPointer;
bool boal_soundOn = true;
int compIdx = 0;
bool bCompPointers = true;
int CurrentShip = 0;
int CurrentFlag = 0;
int ShipFlagsQuantity = -1;
bool AltIntIcons = false;
#event_handler("evntRandomSailDmg","ProcessRandomSailDmg");
#event_handler("GetSailTextureData","procGetSailTextureData");
#event_handler("GetRiggingData","procGetRiggingData");
#event_handler("EventMastFall","procMastFall");
#event_handler("evntGetSmallFlagData","procGetSmallFlagData");
#event_handler("evntGetCaptainFace","procGetCaptainFace");
#event_handler(SHIP_CREATE,"BI_CreateShip");
#event_handler("evntGetSailStatus","procGetSailStatus");

void BI_InitExperienceLog()
{
	// LEO: Морское окружение переживает пересоздание HUD, но не переход в другой морской сеанс.
	if(!CheckAttribute(&AISea,"expLogStarted"))
	{
		DeleteAttribute(pchar,"BIEXP");
		AISea.expLogStarted=1;
	}
	ExpLog_Apply(&BattleInterface,true);
}

void InitBattleInterface()
{
	AltIntIcons = sti(InterfaceStates.AltIntIcons) == 1; // LEO: Альтернативные иконки не зависят от выбранной формы интерфейса.
	BI_InitializeCommands();
	bi_icons_CommandMode = 0;
	BattleInterface.comstate = 0;
	BattleInterface.SelCharacter = -1;
	bi_idxSelectedPicture = -1;
	BattleInterface.boardRadius = 50;
	BattleInterface.freeDistance = MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER;

	SetParameterData();

	int nCAdel;
	for (nCAdel = 1; nCAdel < COMPANION_MAX; nCAdel++)
	{
		DeleteAttribute(&BattleInterface, "textinfo.CompAction1" + nCAdel);
		DeleteAttribute(&BattleInterface, "textinfo.CompAction2" + nCAdel);
	}

	BI_InitExperienceLog();
	BattleInterface.blindSpeed = 0.003;
	BattleInterface.MaxWind = 30.0;
	BattleInterface.MaxShipSpeed = 20.0;
	if (iArcadeSails == 1)
	{
		BattleInterface.ShipSpeedScaler = (1.0 / 2.5);
	}
	else
	{
		BattleInterface.ShipSpeedScaler = (1.0 / 1.0);
	}
	BattleInterface.ShowNavigator = InterfaceStates.BattleShow.Navigator;
	BattleInterface.ShowCommands = InterfaceStates.BattleShow.Command;
	BattleInterface.MainChrIndex = nMainCharacterIndex;
	ShipTowHUD_Build(&BattleInterface);
	CreateEntity(&BattleInterface,"battle_interface");
	DeleteAttribute(&BattleInterface,"CommandTextures");
	DeleteAttribute(&BattleInterface,"CommandShowParam");
	LayerAddObject(SEA_EXECUTE,&BattleInterface,-1);
	LayerAddObject(SEA_REALIZE,&BattleInterface,-1);

	SetEventHandler(SHIP_DELETE,"BI_DeleteShip",0);
	SetEventHandler(SHIP_DEAD,"BI_DeadShip",0);
	SetEventHandler("BI_CommandEndChecking","BI_CommandEndChecking",0);
	SetEventHandler("BI_LaunchCommand","BI_LaunchCommand",0);
	SetEventHandler("BI_GetChargeQuantity","BI_GetChargeQuantity",0);
	SetEventHandler("BI_SetPossibleCommands","BI_SetPossibleCommands",0);
	SetEventHandler("GetCurrentCharge","GetCurrentCharge",0);
	SetEventHandler("evntGetCharacterShipClass","biGetCharacterShipClass",0);
	SetEventHandler(BI_EVENT_SET_VISIBLE,"BI_CommandVisible",0);
	SetEventHandler(BI_EVENT_SET_SEA_STATE,"BI_SetSeaState",0);
	SetEventHandler(BI_EVENT_GET_DATA,"BI_GetData",0);
	SetEventHandler(BI_EVENT_CHECK_SHIPSTATE_SHOW,"BI_GetSSShow",0);
	SetEventHandler("evntGetLandData","BI_GetLandData",0);
	SetEventHandler(BI_EVENT_GET_FORT_RELATION,"BI_GetFortRelation",0);
	SetEventHandler(EVENT_CHANGE_COMPANIONS,"RefreshBattleInterface",0);
	SetEventHandler("BI_CallUpdateShip","BI_UpdateShip",0);
	SetEventHandler("SeaAimReadout","BI_SeaAimReadout",0);
	SetEventHandler("frame","BI_Frame",1);
	SetEventHandler("BI_DirSailCheck","BI_DirSailCheck",0);
	PostEvent("BI_DirSailCheck", 100);
	SetEventHandler("evntPerkAgainUsable","BI_PerkAgainUsable",0);
	SetEventHandler("evntSetUsingAbility","procSetUsingAbility",0);
	SetEventHandler("evntCheckEnableLocator","procCheckEnableLocator",0);
	SetEventHandler("evntCheckEnableShip","procCheckEnableShip",0);
	SetEventHandler("evntGetSRollSpeed","procGetSRollSpeed",0);
	SetEventHandler("DoSailHole","ProcessSailDamage",0);
	SetEventHandler("evntBISelectShip","procBISelectShip",0);
	SetEventHandler("BI_CallUpdateShipsRollSpeed","BI_UpdateShipsRollSpeed", 0);

	procLoadIntoNew();
	SetEventHandler("Control Activation","BI_ProcessControlPress",0);
	CreateILogAndActions(LOG_FOR_SEA);
	Log_SetActiveAction("Nothing");
	ResetTimeScale();
	BI_UpdateLoadedProjectiles();
	BI_UpdateCannons();
	BI_UpdateShipsRollSpeed();
	ShipTowHUD_Start();
}

void GetSailStatus(int chrIdx)
{
	object objSail;
	if( !GetEntity(&objSail,"sail") )
	{
		return;
	}
	if(chrIdx < 0) return;
	ref chref = GetCharacter(chrIdx);
	if(CheckAttribute(chref,"ship.sailstatus")) DeleteAttribute(chref,"ship.sailstatus"));
	SendMessage(&objSail,"lsl", MSG_SAIL_SCRIPT_PROCESSING,"GetSailStatus", chrIdx);
}

void procGetSailStatus()
{
	int chrIdx 		= GetEventData();
	string nodeName = GetEventData();
	int grNum 		= GetEventData();
	float fSPow 	= GetEventData();
	int hc 			= GetEventData();
	int mhc 		= GetEventData();

	if(chrIdx < 0) return;
	ref chref = GetCharacter(chrIdx);

	aref arSail;
	string groupName = ""+grNum;
	chref.ship.sailstatus = sti(chref.ship.sailstatus) + 1;

	makearef(arSail,chref.ship.sailstatus.(nodeName).(groupName));

	arSail.mhc 	= mhc;
	arSail.hc 	= hc;
	arSail.sp 	= fSPow;
}

ref BI_GetFortRelation()
{
	BI_intRetValue = BI_RELATION_NEUTRAL;
	aref arLoc = GetEventData();
	int chrIdx = Fort_FindCharacter(AISea.Island,"reload",arLoc.name);
	if(chrIdx>=0)
	{
		switch( SeaAI_GetRelation(chrIdx,nMainCharacterIndex) )
		{
		case RELATION_FRIEND:	BI_intRetValue = BI_RELATION_FRIEND;	break;
		case RELATION_NEUTRAL:	BI_intRetValue = BI_RELATION_NEUTRAL;	break;
		case RELATION_ENEMY:	BI_intRetValue = BI_RELATION_ENEMY;		break;
		}
	}
	return &BI_intRetValue;
}

ref BI_GetSSShow()
{
	BI_intRetValue = false;
	int charIdx = GetEventData();
	switch( GetCharacterEquipByGroup(pchar, SPYGLASS_ITEM_TYPE) )
	{
	case CHEAP_SPYGLASS:	BI_intRetValue = false;	break;
	case COMMON_SPYGLASS:	BI_intRetValue = true;	break;
	case GOOD_SPYGLASS:		BI_intRetValue = true;	break;
	case SUPERIOR_SPYGLASS:	BI_intRetValue = true;	break;
	}
	return &BI_intRetValue;
}

void BI_CommandVisible()
{
	int visibleFlag = GetEventData();
	SendMessage(&BattleInterface,"ll",BI_SET_VISIBLE,visibleFlag);

	if (visibleFlag == 0)
	{
		compIdx = 0;
		if (CheckAttribute(&objShipPointer, "textures"))
			SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, -1, 0, 0.005, 1.0, 1.0, 1.0, 5.0);
	}
}

void BI_SeaAimReadout()
{
	int distance=GetEventData();
	int spreadWidth=GetEventData();
	int spreadLength=GetEventData();
	NewSeaHUD_UpdateAimReadout(&BattleInterface,distance,spreadWidth,spreadLength);
}

int bi_nReloadTarget=-1;
bool biold_bCanEnterToLand = false;
bool biold_bAbordageShipCanBe = false;
bool biold_bAbordageFortCanBe = false;
bool biold_bReloadCanBe = false;
bool biold_bMapEnter = false;
bool bOldNotEnoughBalls = false;

int biLastMainShipType = -999; // LEO: Объявление нужно старым сейвам; без него загрузка падает с "variable not found".
string g_DirSailTarget = ""; // LEO: Цель обновляется таймером BI_DirSailCheck, читается в GetBILocationName.
bool bDirSailRingOnce = true; // Gregg: гард однократного звука/лога при появлении доплытия

#define DIRSAIL_CHECK_MS	400
void BI_DirSailCheck()
{
	g_DirSailTarget = "";
	if(bSeaActive && sti(InterfaceStates.DirectSail)) g_DirSailTarget = CheckForPotentialIslandChange();
	if(g_DirSailTarget != "")
	{
		if(bDirSailRingOnce)
		{
			bDirSailRingOnce = false;
			PlaySound("interface\dyn_dyn.ogg");
			Log_SetStringToLog(XI_ConvertString("DirectSailReady") + XI_ConvertString("near") + XI_ConvertString(g_DirSailTarget + "Gen"));
		}
	}
	else
	{
		bDirSailRingOnce = true;
	}
	PostEvent("BI_DirSailCheck", DIRSAIL_CHECK_MS);
}

void BI_Frame()
{
	ShipInfoBars_Update(); // LEO: Мир/бой и смена опции применяются без пересоздания HUD.
	bool bYesUpdateCommand = false;
	if(bNotEnoughBalls!=bOldNotEnoughBalls)
	{
		bOldNotEnoughBalls = bNotEnoughBalls;
		if(bNotEnoughBalls)
		{
			SendMessage(&BattleInterface,"ll",BI_MSG_NOT_ENOUGH_BALLS_FLAG,true);
		}
		else
		{
			SendMessage(&BattleInterface,"ll",BI_MSG_NOT_ENOUGH_BALLS_FLAG,false);
		}
	}
	if(biold_bAbordageShipCanBe!=bAbordageShipCanBe)
	{
		bYesUpdateCommand = true;
		biold_bAbordageShipCanBe = bAbordageShipCanBe;
	}
	if(biold_bAbordageFortCanBe!=bAbordageFortCanBe)
	{
		bYesUpdateCommand = true;
		biold_bAbordageFortCanBe = bAbordageFortCanBe;
	}
	if(biold_bCanEnterToLand!=bCanEnterToLand)
	{
		bYesUpdateCommand = true;
		biold_bCanEnterToLand = bCanEnterToLand;
	}
	if(biold_bReloadCanBe!=bReloadCanBe)
	{
		bYesUpdateCommand = true;
		biold_bReloadCanBe = bReloadCanBe;
	}
	if(biold_bMapEnter!=bMapEnter)
	{
		bYesUpdateCommand = true;
		biold_bMapEnter = bMapEnter;
	}
	if(bYesUpdateCommand)
	{
		BI_SetCommandMode(-1,-1,-1,-1);
	}
	if(bAbordageShipCanBe)
	{
		Log_SetActiveAction("Board");
		if (boal_soundOn)
		{
		    boal_soundOn = false;
            PlayVoice("interface\"+LanguageGetLanguage()+"_GTBoard"+rand(1)+".wav");
		}

		return;
	}
	if(bAbordageFortCanBe)
	{
		Log_SetActiveAction("LandTroops");

		if (boal_soundOn)
		{
		    boal_soundOn = false;
            PlayVoice("interface\"+LanguageGetLanguage()+"_Abandon0.wav");
		}

		return;
	}
	if(bReloadCanBe)
	{
		Log_SetActiveAction("Reload");
		return;
	}
	if(bCanEnterToLand)
	{
		Log_SetActiveAction("Moor");

		if (boal_soundOn)
		{
		    boal_soundOn = false;
            PlaySound("interface\_Yakordrop.wav");
		}

		return;
	}

	boal_soundOn = true;

	if(g_DirSailTarget != "") // Gregg: доплытие как быстрое действие (приоритет выше выхода на карту)
	{
		Log_SetActiveAction("DirSail");
		return;
	}
	if(bMapEnter)
	{
		Log_SetActiveAction("Map");
		return;
	}
	Log_SetActiveAction("Nothing");
}

void StartBattleInterface()
{
	bi_nReloadTarget = -1;
	bi_icons_ShowType = BI_ICONS_ST_MYSHIPS;
	BI_SetCommandMode(BI_COMMODE_MY_SHIP_SELECT,-1,-1,-1);
	BI_SetIslandData();
	InterfaceSpyGlassInit(false);
	objShipPointer.textures.friend = "battle_interface\Frendly.dds";
	objShipPointer.textures.enemy = "battle_interface\Enemy.dds";
	CreateEntity(&objShipPointer,"shippointer");
	LayerAddObject(SEA_EXECUTE,&objShipPointer,222222);
	LayerAddObject(SEA_REALIZE,&objShipPointer,-1);
	DoQuestFunctionDelay("ActiveShowAfterReload", 0.2);
}

void RefreshBattleInterface()
{
	BI_SetCommandMode(0,-1,-1,-1);
	SendMessage(&BattleInterface,"l",BI_MSG_REFRESH);
	BI_SetCommandMode(BI_COMMODE_MY_SHIP_SELECT,-1,-1,-1);

	UpdateCompanionsStates();
}

// LEO: отметить выбранный боеприпас (тип 0-3): подсветка цвета самой иконки + стрелка-маркер под ней.
// Позиции берём готовыми - rect иконки из imageslist и rect слота из ammomark, оба посчитаны при построении ряда.
void BI_UpdateAmmoHighlight()
{
	if(!CheckAttribute(pchar, "Ship.Cannons.Charge.Type")) { BattleInterface.ammohighlight.used = 0; BattleInterface.ammomark.used = 0; return; }
	int t = sti(pchar.Ship.Cannons.Charge.Type);
	if(t < 0 || t > 3) { BattleInterface.ammohighlight.used = 0; BattleInterface.ammomark.used = 0; return; }
	string nm = "ammoicon" + t;

	if(!CheckAttribute(BattleInterface, "imageslist." + nm + ".pos")) { BattleInterface.ammohighlight.used = 0; BattleInterface.ammomark.used = 0; return; }
	BattleInterface.ammohighlight.icon = nm;
	BattleInterface.ammohighlight.pos = BattleInterface.imageslist.(nm).pos; // базовая позиция иконки (для шейка)
	BattleInterface.ammohighlight.used = 1;

	string amSl = "slot" + t;
	if(!CheckAttribute(BattleInterface, "ammomark." + amSl)) { BattleInterface.ammomark.used = 0; return; }
	BattleInterface.ammomark.pos = BattleInterface.ammomark.(amSl);
	BattleInterface.ammomark.used = 1;
}

ref BI_GetCmdListOrderPriority()
{
  int curItem = GetEventData();
  BI_intRetValue = 0;

  if(curItem >= 0 && curItem < 14) {
    int i = 0;
    int cmdList[14];
    cmdList[i] = BI_COMMODE_FRIEND_FORT_SELECT; i++;
    cmdList[i] = BI_COMMODE_NEUTRAL_FORT_SELECT; i++;
    cmdList[i] = BI_COMMODE_ENEMY_FORT_SELECT; i++;
    cmdList[i] = BI_COMMODE_NOTDISEASED_TOWN; i++;
    cmdList[i] = BI_COMMODE_DISEASED_TOWN; i++;
    cmdList[i] = BI_COMMODE_ENEMY_TOWN; i++;
    cmdList[i] = BI_COMMODE_LAND_SELECT; i++;
    cmdList[i] = BI_COMMODE_MY_SHIP_SELECT; i++;
    cmdList[i] = BI_COMMODE_NEUTRAL_SHIP_SELECT; i++;
    cmdList[i] = BI_COMMODE_FRIEND_SHIP_SELECT; i++;
    cmdList[i] = BI_COMMODE_ENEMY_SHIP_SELECT; i++;
    cmdList[i] = BI_COMMODE_COMMAND_SELECT; i++;
    cmdList[i] = BI_COMMODE_CANNON_CHARGE; i++;
    cmdList[i] = BI_COMMODE_USER_ICONS; i++;

    BI_intRetValue = cmdList[curItem];
  }

  return &BI_intRetValue;
}

void DeleteBattleInterface()
{
	ShipTowHUD_Stop();
    ResetTimeScale();

	Log_SetActiveAction("Nothing");
	InterfaceSpyGlassRelease();

    DeleteClass(&BattleInterface);
	DeleteClass(&objShipPointer);

	DelEventHandler(SHIP_DELETE, "BI_DeleteShip");
	DelEventHandler(SHIP_DEAD,"BI_DeadShip");
	DelEventHandler("BI_CommandEndChecking", "BI_CommandEndChecking");
	DelEventHandler("BI_LaunchCommand", "BI_LaunchCommand");
	DelEventHandler("BI_GetChargeQuantity","BI_GetChargeQuantity");
	DelEventHandler("BI_SetPossibleCommands","BI_SetPossibleCommands");
	DelEventHandler("GetCurrentCharge","GetCurrentCharge");
	DelEventHandler("evntGetCharacterShipClass","biGetCharacterShipClass");
	DelEventHandler(BI_EVENT_SET_VISIBLE,"BI_CommandVisible");
	DelEventHandler(BI_EVENT_SET_SEA_STATE,"BI_SetSeaState");
	DelEventHandler(BI_EVENT_GET_DATA,"BI_GetData");
	DelEventHandler(BI_EVENT_CHECK_SHIPSTATE_SHOW,"BI_GetSSShow");
	DelEventHandler("evntGetLandData","BI_GetLandData");
	DelEventHandler(BI_EVENT_GET_FORT_RELATION,"BI_GetFortRelation");
	DelEventHandler(EVENT_CHANGE_COMPANIONS,"RefreshBattleInterface");
	DelEventHandler("BI_CallUpdateShip","BI_UpdateShip");
	DelEventHandler("SeaAimReadout","BI_SeaAimReadout");
	DelEventHandler("frame","BI_Frame");
	DelEventHandler("BI_DirSailCheck","BI_DirSailCheck");
	DelEventHandler("evntPerkAgainUsable","BI_PerkAgainUsable");
	DelEventHandler("evntSetUsingAbility","procSetUsingAbility");
	DelEventHandler("evntCheckEnableLocator","procCheckEnableLocator");
	DelEventHandler("evntCheckEnableShip","procCheckEnableShip");
	DelEventHandler("evntGetSRollSpeed","procGetSRollSpeed");
	DelEventHandler("Control Activation","BI_ProcessControlPress");
	DelEventHandler("DoSailHole","ProcessSailDamage");
	DelEventHandler("evntBISelectShip","procBISelectShip");
	DelEventHandler("BI_CallUpdateShipsRollSpeed","BI_UpdateShipsRollSpeed");
}

ref BI_CommandEndChecking()
{
	BI_retComValue = 0;
	string comName = GetEventData();

	switch(comName)
	{

	case "BI_Cabin":
		BI_retComValue = 0;
		break;

	case "BI_Boat":
		BI_retComValue = BI_COMMODE_NEUTRAL_SHIP_SELECT+BI_COMMODE_FRIEND_SHIP_SELECT;
        BattleInterface.Commands.Boat.EffectRadius = DistanceToShipTalk;

		if (CheckAttribute(pchar, "GenQuest.DestroyPirate"))
		{
			BattleInterface.Commands.Boat.EffectRadius = 150;
		}

		break;
	case "BI_Tow":
		BI_retComValue = BI_COMMODE_NEUTRAL_SHIP_SELECT+BI_COMMODE_FRIEND_SHIP_SELECT+BI_COMMODE_ENEMY_SHIP_SELECT;
		BattleInterface.Commands.Tow.EffectRadius = -1.0;
		break;
	case "BI_Bomb":
		BI_retComValue = 0;
		break;
	case "cancel":
		BI_retComValue = -1;
		break;
	case "BI_Moor":
		BI_retComValue = 0;
		break;
	case "BI_SailTo":
		int iIsland = FindIsland(pchar.location);
		if(iIsland > 0 && !sti(Islands[iIsland].reload_enable))
		{
			bEnableIslandSailTo	= false;
		}
		if(bEnableIslandSailTo) {
			BI_retComValue = BI_COMMODE_MY_SHIP_SELECT+BI_COMMODE_NEUTRAL_SHIP_SELECT+BI_COMMODE_FRIEND_SHIP_SELECT+BI_COMMODE_ENEMY_SHIP_SELECT+BI_COMMODE_FRIEND_FORT_SELECT+BI_COMMODE_NEUTRAL_FORT_SELECT+BI_COMMODE_ENEMY_FORT_SELECT+BI_COMMODE_DISEASED_TOWN+BI_COMMODE_NOTDISEASED_TOWN+BI_COMMODE_LAND_SELECT;
		} else {
			BI_retComValue = BI_COMMODE_MY_SHIP_SELECT+BI_COMMODE_NEUTRAL_SHIP_SELECT+BI_COMMODE_FRIEND_SHIP_SELECT+BI_COMMODE_ENEMY_SHIP_SELECT;
		}
		if (bBettaTestMode)
		{
		    BattleInterface.Commands.SailTo.EffectRadius	= 900000;
		}
  		else
  		{
			string sSpyGlass = GetCharacterEquipByGroup(pchar, SPYGLASS_ITEM_TYPE);
			if(sSpyGlass != "")
			{
				ref rItm = ItemsFromID(sSpyGlass);
				BattleInterface.Commands.SailTo.EffectRadius	= 5000 + sti(rItm.radius);
			}
			else BattleInterface.Commands.SailTo.EffectRadius	= 5000;
		}
		break;
	case "BI_Board":
		BI_retComValue = 0;
		break;
	case "BI_LandTroops":
		BI_retComValue = 0;
		break;
	case "BI_Map":
		BI_retComValue = 0;
		break;
	case "BI_Attack":
		BI_retComValue = BI_COMMODE_ENEMY_SHIP_SELECT+BI_COMMODE_ENEMY_FORT_SELECT+BI_COMMODE_ALLLOCATOR_SELECT;
        BattleInterface.Commands.Attack.EffectRadius	= 2000;
		break;
	case "BI_Abordage":
		BI_retComValue = BI_COMMODE_ENEMY_SHIP_SELECT;
		BattleInterface.Commands.Abordage.EffectRadius	= 2000;
		break;
	case "BI_Defend":
		BI_retComValue = BI_COMMODE_MY_SHIP_SELECT+BI_COMMODE_FRIEND_SHIP_SELECT+BI_COMMODE_FRIEND_FORT_SELECT+BI_COMMODE_ALLLOCATOR_SELECT;
		BattleInterface.Commands.Defend.EffectRadius	= 2000;
		break;
	case "BI_SailAway":
		BI_retComValue = 0;
		break;
	case "BI_DirSail":
		BI_retComValue = 0;
		break;
    case "BI_SailDir":
	    BI_SailDirIconSet();
		BI_retComValue = BI_COMMODE_USER_ICONS;
		break;
	case "BI_HeaveToDrift":
		BI_retComValue = 0;
		break;
	case "BI_Reload":
		BI_retComValue = 0;
		break;
	case "BI_Charge":
		BI_retComValue = BI_COMMODE_CANNON_CHARGE;
		break;
	case "BI_Speed":
		BI_SailSpeedIconSet();
		BI_retComValue = BI_COMMODE_USER_ICONS;
		break;
	case "BI_CompanionCommand":
		BI_retComValue = BI_COMMODE_MY_SHIP_SELECT+BI_COMMODE_ALLLOCATOR_SELECT;
		break;
	case "BI_Brander":
		BI_retComValue = BI_COMMODE_ENEMY_SHIP_SELECT+BI_COMMODE_ALLLOCATOR_SELECT;
		break;
	case "BI_ImmediateReload":
		BI_retComValue = 0;
		break;
	case "BI_LightRepair":
		BI_retComValue = 0;
		break;
	case "BI_InstantRepair":
		BI_retComValue = 0;
		break;
	case "BI_Turn180":
		BI_retComValue = 0;
		break;
	case "BI_SandbankManeuver":
		BI_retComValue = 0;
		break;
	case "BI_Ability":
		BI_retComValue = BI_COMMODE_ABILITY_ICONS;
		break;
	case "BI_ImmDeath":
		BI_retComValue = BI_COMMODE_NEUTRAL_SHIP_SELECT+BI_COMMODE_FRIEND_SHIP_SELECT+BI_COMMODE_ENEMY_SHIP_SELECT+BI_COMMODE_FRIEND_FORT_SELECT+BI_COMMODE_NEUTRAL_FORT_SELECT+BI_COMMODE_ENEMY_FORT_SELECT;
		BattleInterface.Commands.ImmediateDeath.EffectRadius	= 900000;
	break;
	case "BI_InstantBoarding":
		BI_retComValue = BI_COMMODE_NEUTRAL_SHIP_SELECT+BI_COMMODE_FRIEND_SHIP_SELECT+BI_COMMODE_ENEMY_SHIP_SELECT;
	break;
	case "BI_Speak":
		BI_retComValue = BI_COMMODE_NEUTRAL_SHIP_SELECT+BI_COMMODE_FRIEND_SHIP_SELECT+BI_COMMODE_ENEMY_SHIP_SELECT;
		BattleInterface.Commands.Speak.EffectRadius	= 500;
	break;

	}

	return &BI_retComValue;
}
void BI_SailDirIconSet()
{
    BattleInterface.UserIcons.ui1.enable = false;
    BattleInterface.UserIcons.ui2.enable = false;
    BattleInterface.UserIcons.ui3.enable = false;

    BattleInterface.UserIcons.ui4.enable = true;
    BattleInterface.UserIcons.ui5.enable = true;
    BattleInterface.UserIcons.ui6.enable = true;
    BattleInterface.UserIcons.ui7.enable = true;
    BattleInterface.UserIcons.ui8.enable = true;
    BattleInterface.UserIcons.ui9.enable = true;
    BattleInterface.UserIcons.ui10.enable = true;
    BattleInterface.UserIcons.ui11.enable = true;
}

void BI_SailSpeedIconSet()
{
    BattleInterface.UserIcons.ui1.enable = true;
    BattleInterface.UserIcons.ui2.enable = true;
    BattleInterface.UserIcons.ui3.enable = true;

    BattleInterface.UserIcons.ui4.enable = false;
    BattleInterface.UserIcons.ui5.enable = false;
    BattleInterface.UserIcons.ui6.enable = false;
    BattleInterface.UserIcons.ui7.enable = false;
    BattleInterface.UserIcons.ui8.enable = false;
    BattleInterface.UserIcons.ui9.enable = false;
    BattleInterface.UserIcons.ui10.enable = false;
    BattleInterface.UserIcons.ui11.enable = false;
}

void BI_LaunchCommand()
{
	bool bOk;
	int charIdx = GetEventData();
	string commandName = GetEventData();
	int targetNum = GetEventData();
	string locName = GetEventData();

	compIdx = 0;
	if (CheckAttribute(&objShipPointer, "textures"))
		SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, -1, 0, 0.005, 1.0, 1.0, 1.0, 5.0);
	if (charIdx < 0 || charIdx >= TOTAL_CHARACTERS) return;
	ref chRef = GetCharacter(charIdx);
	if( LAi_IsDead(chRef) ) return;

	aref arFader;
	if( GetEntity(arFader,"fader") ) {return;}

	if(targetNum==-1 && locName=="cancel") {
		SendMessage(&BattleInterface,"ls",MSG_BATTLE_LAND_MAKE_COMMAND,"cancel");
		return;
	}
	if(commandName=="cancel") {
		SendMessage(&BattleInterface,"ls",MSG_BATTLE_LAND_MAKE_COMMAND,"cancel");
		return;
	}

	string alternativeCommand;
	if( CheckAttribute( &BattleInterface, "AbilityIcons."+commandName+".quest" ) )
	{
		alternativeCommand = commandName;
		commandName = "BI_UseItemAbilitie";
	}

	// LEO: Открытие подменю и отмена выбора не отменяют подход; исполняемый новый приказ отменяет только заявку.
	if (commandName != "BI_Tow")
	{
		DeleteAttribute(chRef, "CompanionAutoEngage");
		if (commandName != "BI_CompanionCommand") SeaTowOrders_Cancel(chRef);
	}

	switch(commandName)
	{
    case "BI_Tow":
		bool towAccepted = false;
		if (targetNum >= 0 && targetNum < TOTAL_CHARACTERS)
			towAccepted = SeaTowOrders_Toggle(chRef, GetCharacter(targetNum));
		if (towAccepted) PlayVoice("INTERFACE\SignalCompanion.wav");
		else PlaySound("interface\knock.wav");
		break;
    case "BI_Cabin":

		if (Sea_BlockedByBalls()) break;
        Sea_CabinStartNow();
		break;
	case "BI_Boat":

		if (Sea_BlockedByBalls()) break;
		if (!CheckAttribute(Characters[targetNum], "LockBoat") && !HasSubStr(Characters[targetNum].id, "_DriftCap_"))
		{

			if(Characters[targetNum].id != "MaryCelesteCapitan" || PChar.QuestTemp.MaryCeleste != "OnDeck")
			{
				Sea_DeckBoatLoad(targetNum);
			}
			else
			{
				PlaySound("interface\knock.wav");
			}
		}
		else
		{
			PlaySound("interface\knock.wav");
		}
    break;
	 case "BI_Bomb":
        if (GetCargoGoods(chRef, GOOD_POWDER) >= 200 && GetRemovable(chRef)) SetMineFree(chRef, 1); // fix ugeen 21.12.13
		else PlaySound("interface\knock.wav");
		break;
	case "BI_Charge":
		int chargeType=GOOD_BALLS;
		switch(targetNum)
		{
		case 1:
			PlayVoice("interface\"+LanguageGetLanguage()+"_balls.wav");
			chargeType=GOOD_BALLS;
			break;
		case 2:
			PlayVoice("interface\"+LanguageGetLanguage()+"_grapes.wav");
			chargeType=GOOD_GRAPES;
			break;
		case 3:
			PlayVoice("interface\"+LanguageGetLanguage()+"_chain.wav");
			chargeType=GOOD_KNIPPELS;
			break;
		case 4:
			PlayVoice("interface\"+LanguageGetLanguage()+"_bombs.wav");
			chargeType=GOOD_BOMBS;
			break;
		}
		Ship_ChangeCharge(chRef, chargeType);

		chRef.ShipCannonChargeType = chargeType;
		break;
	case "BI_Map":
		if (bMapEnter)
        {
			Sea_MapLoad();
        }
        else
        {
            Log_SetStringToLog(GetConvertStrWithReplace("Variable_BattleInterface_1", "Battle_Interface.txt", "#space#", " "));
        }
		break;
	case "BI_Moor":
		Sea_LandLoad();
		break;
	case "BI_Board":
		Sea_AbordageLoad(SHIP_ABORDAGE,true);
		break;
	case "BI_LandTroops":
		Sea_AbordageLoad(FORT_ABORDAGE,true);
		break;
	case "BI_SailAway":
		if (CheckAttribute(&characters[charIdx], "SeaAI.Task.Target"))  //fix
		{
			Ship_SetTaskRunAway(SECONDARY_TASK, charIdx, sti(characters[charIdx].SeaAI.Task.Target));
			PlayVoice("INTERFACE\SignalCompanion.wav");
		}
		else
		{
			Ship_SetTaskRunAway(SECONDARY_TASK, charIdx, GetMainCharacterIndex());
			PlayVoice("INTERFACE\SignalCompanion.wav");
		}
	break;
	case "BI_HeaveToDrift":
		Ship_SetTaskDrift(SECONDARY_TASK,charIdx);
		PlayVoice("INTERFACE\"+LanguageGetLanguage()+"_strike_down.wav");
		break;
	case "BI_Defend":
		Ship_SetTaskDefend(SECONDARY_TASK,charIdx,GetTargChrIndex(targetNum,locName));
		PlayVoice("INTERFACE\SignalCompanion.wav");
		DeleteAttribute(chRef, "ShipCannonChargeType"); // флаг офов, чем пулять постоянно, ставится игроком командами, до след приказа Атака или конца снарядов
		break;
	case "BI_Attack":
		Ship_SetTaskAttack(SECONDARY_TASK,charIdx,GetTargChrIndex(targetNum,locName));
		PlayVoice("INTERFACE\SignalCompanion.wav");
		DeleteAttribute(chRef, "ShipCannonChargeType"); // флаг офов, чем пулять постоянно, ставится игроком командами, до след приказа Атака или конца снарядов
	break;
	case "BI_Abordage":
		Ship_SetTaskAbordage(SECONDARY_TASK,charIdx,GetTargChrIndex(targetNum,locName));
	break;
	case "BI_Reload":
		if(bi_nReloadTarget!=-1)
		{
			LaunchTransferMain(chRef, GetCharacter(bi_nReloadTarget), "Transfer");
		}
		break;
	case "BI_SailTo":
	    if (bDisableMapEnter && !bBettaTestMode)
	    {
           PlaySound("interface\knock.wav");
	       break;
	    }
	    if (!CheckEnemyCompanionDistance2GoAway(false) && !bBettaTestMode)  // компаньон в беде
	    {
           PlaySound("interface\knock.wav");
	       break;
	    }

		if (targetNum == -1)
		{ // приплыть в локатор с именем locName
            bOk = true;

			if (MOD_SKILL_ENEMY_RATE >= 2 && !bBettaTestMode)
            {
                targetNum = Fort_FindCharacter(AISea.Island,"reload",locName);
                if (targetNum == -1)
                {

                    targetNum = Fort_FindCharacterByPort(AISea.Island, locName);
                }
            }
			if (targetNum >= 0)
			{
				if (GetRelation(sti(pchar.index), targetNum) == RELATION_ENEMY && !CheckAttribute(&Characters[targetNum], "CanBeSailTo"))
				{
                  	if (sti(Characters[targetNum].Fort.Mode) == FORT_NORMAL)
					{
						bOk = false;
					}
				}
			}
			if (bOk)
   			{
    			if( !IsEntity(&SailToFader) ) {trace("SailToLocator : " + locName); SeaAI_SailToLocator(locName); }
			}
			else
			{
                PlaySound("interface\knock.wav");
			}
		}
		else
		{ // догнать перса с индексом targetNum

            if (GetRelation(sti(pchar.index), targetNum) != RELATION_ENEMY || CheckAttribute(&Characters[targetNum], "CanBeSailTo") || bBettaTestMode)
            {
			    if( !IsEntity(&SailToFader) ) {SeaAI_SailToCharacter(targetNum); }
			}
			else
			{
                PlaySound("interface\knock.wav");
			}

		}
		break;
	case "BI_Speed":
		switch(locName)
		{
		case "sail_none":
			Ship_SetSailState(charIdx,0.0);
		break;
		case "sail_midi":
			Ship_SetSailState(charIdx,0.5);
		break;
		case "sail_fast":
			Ship_SetSailState(charIdx,1.0);
		break;
		}
		break;
	case "BI_DirSail":
		if (sti(InterfaceStates.DirectSail) && bSeaActive && !bAbordageStarted) CheckIslandChange(1); //доплытие по кнопке (Gregg: гейт опцией DirectSail)
		break;
	case "BI_SailDir":
        float posX = stf(Characters[charIdx].Ship.Pos.x);
        float posZ = stf(Characters[charIdx].Ship.Pos.z);
        float fAng;
        switch(locName)
		{
			case "dir_n":
				posZ += SAILDIR_POS_DIF;
				Ship_SetTaskMove(SECONDARY_TASK, charIdx, posX, posZ);
				PlayVoice("INTERFACE\SignalCompanion.wav");
			break;
			case "dir_ne":
				fAng = PIm2 * 0.125;
				posX = posX + sin(fAng) * SAILDIR_POS_DIF;
				posZ = posZ + cos(fAng) * SAILDIR_POS_DIF;
				Ship_SetTaskMove(SECONDARY_TASK, charIdx, posX, posZ);
				PlayVoice("INTERFACE\SignalCompanion.wav");
			break;
			case "dir_e":
				posX += SAILDIR_POS_DIF;
				Ship_SetTaskMove(SECONDARY_TASK, charIdx, posX, posZ);
				PlayVoice("INTERFACE\SignalCompanion.wav");
			break;
			case "dir_se":
				fAng = PIm2 * 0.375;
				posX = posX + sin(fAng) * SAILDIR_POS_DIF;
				posZ = posZ + cos(fAng) * SAILDIR_POS_DIF;
				Ship_SetTaskMove(SECONDARY_TASK, charIdx, posX, posZ);
				PlayVoice("INTERFACE\SignalCompanion.wav");
			break;
			case "dir_s":
				posZ -= SAILDIR_POS_DIF;
				Ship_SetTaskMove(SECONDARY_TASK, charIdx, posX, posZ);
				PlayVoice("INTERFACE\SignalCompanion.wav");
			break;
			case "dir_sw":
				fAng = PIm2 * 0.625;
				posX = posX + sin(fAng) * SAILDIR_POS_DIF;
				posZ = posZ + cos(fAng) * SAILDIR_POS_DIF;
				Ship_SetTaskMove(SECONDARY_TASK, charIdx, posX, posZ);
				PlayVoice("INTERFACE\SignalCompanion.wav");
			break;
			case "dir_w":
				posX -= SAILDIR_POS_DIF;
				Ship_SetTaskMove(SECONDARY_TASK, charIdx, posX, posZ);
				PlayVoice("INTERFACE\SignalCompanion.wav");
			break;
			case "dir_nw":
				fAng = PIm2 * 0.875;
				posX = posX + sin(fAng) * SAILDIR_POS_DIF;
				posZ = posZ + cos(fAng) * SAILDIR_POS_DIF;
				Ship_SetTaskMove(SECONDARY_TASK, charIdx, posX, posZ);
				PlayVoice("INTERFACE\SignalCompanion.wav");
			break;
		}
        break;
	case "BI_CompanionCommand":
		BI_SetSpecCommandMode(BI_COMMODE_COMMAND_SELECT,-1,-1,targetNum,1);
		return;
		break;
	case "BI_ImmDeath":
		if(targetNum==-1)
		{ // смерть форта
			targetNum = Fort_FindCharacter(AISea.Island,"reload",locName);
			if(targetNum>=0)
			{
				Fort_SetAbordageMode(pchar, GetCharacter(targetNum));
			}
		}
		else
		{
			if (SeaTowOperations_Block(GetCharacter(targetNum))) return;
			ShipDead(targetNum,KILL_BY_BALL,nMainCharacterIndex);
		}
		break;
	case "BI_InstantBoarding":
  		ActivateCharacterPerk(GetCharacter(charIdx),"InstantBoarding");
		CharacterPerkOff(GetCharacter(charIdx),"InstantBoarding");
		if( CheckSuccesfullBoard(GetCharacter(charIdx),GetCharacter(targetNum)) ) {
			Sea_AbordageStartNow(SHIP_ABORDAGE,targetNum,true,true);
		} else {
			Log_SetStringToLog( XI_ConvertString("failed to board") );
			Event("BI_LaunchCommand","lsls", charIdx, "BI_SailTo", targetNum, locName);
			return;
		}
		break;
	case "BI_Brander":
		if (SeaTowOperations_Block(GetCharacter(charIdx))) return;
		Ship_SetTaskBrander(SECONDARY_TASK, charIdx, targetNum);
		PlayVoice("INTERFACE\SignalCompanion.wav");
		break;
	case "BI_ImmediateReload":
		ActivateCharacterPerk(GetCharacter(charIdx),"ImmediateReload");
		PlayVoice("INTERFACE\SignalCompanion.wav");
		break;
	case "BI_LightRepair":
		ActivateCharacterPerk(GetCharacter(charIdx),"LightRepair");
		ActivateSpecRepair(GetCharacter(charIdx),0);
		PlayVoice("INTERFACE\SignalCompanion.wav");
		break;
	case "BI_InstantRepair":
		ActivateCharacterPerk(GetCharacter(charIdx),"InstantRepair");
		ActivateSpecRepair(GetCharacter(charIdx),1);
		PlayVoice("INTERFACE\SignalCompanion.wav");
		break;
	case "BI_Turn180":
		ActivateCharacterPerk(GetCharacter(charIdx),"Turn180");
		PlayVoice("INTERFACE\SignalCompanion.wav");
		break;
	case "BI_SandbankManeuver":
		ActivateCharacterPerk(GetCharacter(charIdx),"SandbankManeuver");
		PlayVoice("INTERFACE\SignalCompanion.wav");
		break;
	case "BI_UseItemAbilitie":
		CompleteQuestName( BattleInterface.AbilityIcons.(alternativeCommand).quest, "");
	break;
	case "BI_Speak":

		if (CompanionTravel_IsOnLiveRoute(GetCharacter(targetNum)))
		{
			Log_Info(GetFullName(GetCharacter(targetNum)) + GetConvertStrWithReplace("Variable_BattleInterface_OnRouteNoSpeak", "Logs.txt", "#space#", " "));
			break;
		}

		if (HasSubStr(Characters[targetNum].id, "_DriftCap_"))
		{
			Log_Info(GetConvertStrWithReplace("Variable_BattleInterface_DriftNoSpeak", "Logs.txt", "#space#", " "));
			break;
		}

		if (CheckAttribute(&Characters[targetNum], "FiveArmies"))
		{
			Log_Info(GetConvertStrWithReplace("Variable_BattleInterface_FiveArmiesNoSpeak", "Logs.txt", "#space#", " "));
			break;
		}
		if(LAi_IsDead(GetCharacter(targetNum)) == false)
		{
			Sea_Speak(GetCharacter(targetNum), nMainCharacterIndex, -1);
		}
	break;
	}
	BI_SetSpecCommandMode(BI_COMMODE_MY_SHIP_SELECT,-1,-1,-1, 0);
}

void BI_SetIslandData()
{
    if( !CheckAttribute(AISea,"Island"))
	{
		trace("BI_SetIslandData - has bug");
		return;
	}
	int isln = FindIsland(AISea.Island);
	if(isln==-1) return;

	ref atmp;
	atmp = GetIslandByIndex(isln);
	SendMessage(&BattleInterface,"la",BI_MSG_SET_ISLAND,atmp);
}

void BI_UpdateShip()
{
	int charIndex = GetEventData();
	AddShipToInterface(charIndex);
}

void BI_CreateShip()
{
	int charIndex = GetEventData();
	if( IsEntity(&BattleInterface) ) {
		AddShipToInterface(charIndex);
	}
}

void AddShipToInterface(int charIndex)
{
	if(charIndex==-1)
	{
		Trace("ERROR: Invalid character index for create ship");
		return;
	}
	ref chRef = GetCharacter(charIndex);
	int st = GetCharacterShipType(chRef);
	if( st==SHIP_NOTUSED )
	{
		Trace("WARNING!!! Character id = "+chRef.id+" hav`t ship.");
		return;
	}
	ref shipRef = GetRealShip(st);
	ref mainChRef = GetMainCharacter();
	int idx, i;

	if(CheckAttribute(chRef,"curshipnum"))
	{
		idx = sti(chRef.curshipnum);
	}
	else
	{
		for(i = 0; i < MAX_SHIPS_ON_SEA; i++)
		{
			if(Ships[i] == charIndex)
			{
				idx = i;
				chRef.curshipnum = i;
			}
		}
	}
	if (idx < 0)
	{
		int iCompSlotIdx;
		for(i = 1; i < COMPANION_MAX; i++)
		{

			iCompSlotIdx = GetCompanionIndex(mainChRef, i);
			if (iCompSlotIdx < 0) continue;
			if (!GetRemovable(GetCharacter(iCompSlotIdx))) continue;
			if (iCompSlotIdx == charIndex)
			{
				idx = i;
				chRef.curshipnum = i;
			}
		}
	}
	int myShip = false;
	int transferableShip = false;
	int shipRelation = BI_RELATION_NEUTRAL;
	switch( SeaAI_GetRelation(charIndex,nMainCharacterIndex) )
	{
	case RELATION_FRIEND:
		shipRelation = BI_RELATION_FRIEND;
		break;
	case RELATION_ENEMY:
		shipRelation = BI_RELATION_ENEMY;
		break;
	}

	for(i=0; i<COMPANION_MAX; i++)
	{
		if(GetCompanionIndex(pchar,i) == charIndex)
		{
			myShip = true;
		}
	}
	transferableShip = myShip;
	if (HasSubStr(chRef.id, "_DriftCap_") && !LAi_IsDead(chRef)) transferableShip = true;

	if( CharacterIsDead(chRef) )
	{
		if( !CheckAttribute(chRef,"ship.shipsink") || sti(chRef.ship.shipsink)==false )
			return;
	}

	SendMessage(&BattleInterface,"llaallll",BI_IN_CREATE_SHIP,charIndex,chRef,shipRef,myShip,shipRelation,0,transferableShip);
}

void BI_DeleteShip()
{
	int charIndex = GetEventData();
	if(charIndex==-1)
	{
		Trace("ERROR: Invalid character index");
		return;
	}
	if( IsCompanion(GetCharacter(charIndex)) )
	{
		RemoveCharacterCompanion(pchar,GetCharacter(charIndex));
	}
	DeleteAttribute(GetCharacter(charIndex),"ship.shipsink");
	RefreshBattleInterface();
}

void BI_DeadShip()
{
	int charIndex = GetEventData();
	if(charIndex==-1)
	{
		Trace("ERROR: Invalid character index");
		return;
	}
	if( IsCompanion(GetCharacter(charIndex)) )
	{
		RemoveCharacterCompanion(pchar,GetCharacter(charIndex));
	}
	Characters[charIndex].ship.shipsink = true;
	if(charIndex==GetMainCharacterIndex()) NewSeaHUD_UpdateAimReadout(&BattleInterface,0,0,0);

	SendMessage(&BattleInterface, "ll", BI_IN_DEAD_SHIP, charIndex);
	RefreshBattleInterface();
}

void BI_SetCommandMode(int commode, int texNum, int picNum, int chIdx)
{
	bi_icons_CommandMode = commode;
	SendMessage(&BattleInterface,"llllll",BI_IN_SET_COMMAND_MODE,commode,texNum,picNum,chIdx,-1);
}

void BI_SetSpecCommandMode(int commode, int texNum, int picNum, int chIdx, int comState)
{
	bi_icons_CommandMode = commode;
	SendMessage(&BattleInterface,"llllll",BI_IN_SET_COMMAND_MODE,commode,texNum,picNum,chIdx,comState);
}

void BI_SetPossibleCommands()
{
	int chIdx = GetEventData();
	int mainIdx = sti(pchar.index);
	compIdx = chIdx;

	bool bNoCmd = false;
	if( chIdx<0 ) bNoCmd = true;
	else if( CharacterIsDead(GetCharacter(chIdx)) ) bNoCmd = true;
	if( bNoCmd )
	{
		aref aroot,arcur;
		makearef(aroot,BattleInterface.Commands);
		int q = GetAttributesNum(aroot);
		for(int i=0; i<q; i++)
		{
			arcur = GetAttributeN(aroot,i);
			arcur.enable = false;
		}
		return;
	}

	if(mainIdx==chIdx)
	{
		BattleInterface.Commands.ImmediateDeath.enable	= bBettaTestMode;
		BattleInterface.Commands.InstantBoarding.enable	= bBettaTestMode;

		BattleInterface.Commands.Moor.enable			= bCanEnterToLand;
		BattleInterface.Commands.Board.enable			= bAbordageShipCanBe;
		BattleInterface.Commands.SailAway.enable		= false;
		BattleInterface.Commands.HeaveToDrift.enable	= false;
		BattleInterface.Commands.Charge.enable			= CheckAttribute(pchar,"Ship.Cannons.Charge.Type");
		BattleInterface.Commands.LandTroops.enable		= bAbordageFortCanBe;
		BattleInterface.Commands.Attack.enable			= false;
		BattleInterface.Commands.Defend.enable			= false;
		BattleInterface.Commands.Tow.enable				= false;
		BattleInterface.Commands.Reload.enable			= bReloadCanBe;
		BattleInterface.Commands.Abordage.enable		= false;
		BattleInterface.Commands.SailTo.enable			= !bDisableSailTo && bSailTo;
		BattleInterface.Commands.SailDir.enable			= false;
		BattleInterface.Commands.Map.enable				= bMapEnter;
		BattleInterface.Commands.Speed.enable			= true;
		BattleInterface.Commands.Speak.enable			= true; // вернуть губы: ветка спутника гасит их, тут надо включать обратно

		if (sti(RealShips[sti(pchar.Ship.Type)].BaseType) >= SHIP_LEUDO)
        {
            BattleInterface.Commands.Cabin.enable		= true;
        }
        BattleInterface.Commands.Boat.enable           = true;
	}

	else
	{
		BattleInterface.Commands.Moor.enable			= false;
		BattleInterface.Commands.Board.enable			= false;
		BattleInterface.Commands.SailAway.enable		= true;
		BattleInterface.Commands.HeaveToDrift.enable	= true;
		BattleInterface.Commands.Charge.enable			= CheckAttribute(GetCharacter(chIdx),"Ship.Cannons.Charge.Type");
		BattleInterface.Commands.LandTroops.enable		= false;
		BattleInterface.Commands.Attack.enable			= bAttack;
		BattleInterface.Commands.Abordage.enable		= bAbordage;
		BattleInterface.Commands.Defend.enable			= bDefend;
		BattleInterface.Commands.Tow.enable				= SeaTowOrders_HasTarget(GetCharacter(chIdx));
		BattleInterface.Commands.Reload.enable			= false;
		BattleInterface.Commands.SailTo.enable			= false;
		BattleInterface.Commands.SailDir.enable			= true;
		BattleInterface.Commands.Map.enable				= false;
		BattleInterface.Commands.Speed.enable			= false;
		BattleInterface.Commands.Speak.enable			= false;
		BattleInterface.Commands.ImmediateDeath.enable  = false;
		BattleInterface.Commands.InstantBoarding.enable  = false;
		BattleInterface.Commands.Cabin.enable			= false;
        BattleInterface.Commands.Boat.enable			= false;
	}
    Event("evntSetUsingAbility","l", chIdx);
}

void BI_InitializeCommands()
{
	int idLngFile = LanguageOpenFile("commands_name.txt");

	DeleteAttribute(&BattleInterface,"Commands");
	DeleteAttribute(&BattleInterface,"AbilityIcons");

	BattleInterface.Commands.Cancel.enable			= false;
	BattleInterface.Commands.Cancel.picNum			= 1;
	BattleInterface.Commands.Cancel.selPicNum		= 0;
	BattleInterface.Commands.Cancel.texNum			= 2;
	BattleInterface.Commands.Cancel.event			= "Cancel";
	BattleInterface.Commands.Cancel.note			= LanguageConvertString(idLngFile, "sea_Cancel");

	BattleInterface.Commands.Moor.enable			= false;
	BattleInterface.Commands.Moor.picNum			= 29;
	BattleInterface.Commands.Moor.selPicNum			= 13;
	BattleInterface.Commands.Moor.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Moor.event				= "BI_Moor";
	BattleInterface.Commands.Moor.note				= LanguageConvertString(idLngFile, "sea_Moor");

	BattleInterface.Commands.SailTo.enable			= false;
	BattleInterface.Commands.SailTo.picNum			= 16;
	BattleInterface.Commands.SailTo.selPicNum		= 0;
	BattleInterface.Commands.SailTo.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.SailTo.event			= "BI_SailTo";
	BattleInterface.Commands.SailTo.note			= LanguageConvertString(idLngFile, "sea_SailTo");
	BattleInterface.FillSailToIcons = "town";

	BattleInterface.Commands.Board.enable			= false;
	BattleInterface.Commands.Board.picNum			= 30;
	BattleInterface.Commands.Board.selPicNum		= 14;
	BattleInterface.Commands.Board.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Board.event			= "BI_Board";
	BattleInterface.Commands.Board.note				= LanguageConvertString(idLngFile, "sea_Board");

	BattleInterface.Commands.LandTroops.enable		= false;
	BattleInterface.Commands.LandTroops.picNum		= 42;
	BattleInterface.Commands.LandTroops.selPicNum	= 45;
	BattleInterface.Commands.LandTroops.texNum		= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.LandTroops.event		= "BI_LandTroops";
	BattleInterface.Commands.LandTroops.note		= LanguageConvertString(idLngFile, "sea_LandTroops");

	BattleInterface.Commands.Map.enable				= false;
	BattleInterface.Commands.Map.picNum				= 28;
	BattleInterface.Commands.Map.selPicNum			= 12;
	BattleInterface.Commands.Map.texNum				= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Map.event				= "BI_Map";
	BattleInterface.Commands.Map.note				= LanguageConvertString(idLngFile, "sea_Map");

	BattleInterface.Commands.Speak.enable		= true;
	BattleInterface.Commands.Speak.picNum		= 69;
	BattleInterface.Commands.Speak.selPicNum	= 68;
	BattleInterface.Commands.Speak.event		= "BI_Speak";
	BattleInterface.Commands.Speak.note         = LanguageConvertString(idLngFile, "sea_Speak");

	BattleInterface.Commands.Attack.enable			= false;
	BattleInterface.Commands.Attack.picNum			= 49;
	BattleInterface.Commands.Attack.selPicNum		= 33;
	BattleInterface.Commands.Attack.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Attack.event			= "BI_Attack";
	BattleInterface.Commands.Attack.note			= LanguageConvertString(idLngFile, "sea_Attack");

	BattleInterface.Commands.Abordage.enable		= false;
	BattleInterface.Commands.Abordage.picNum		= 30;
	BattleInterface.Commands.Abordage.selPicNum		= 14;
	BattleInterface.Commands.Abordage.texNum		= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Abordage.event			= "BI_Abordage";
	BattleInterface.Commands.Abordage.note			= LanguageConvertString(idLngFile, "sea_Abordage");

	BattleInterface.Commands.Defend.enable			= false;
	BattleInterface.Commands.Defend.picNum			= 51;
	BattleInterface.Commands.Defend.selPicNum		= 35;
	BattleInterface.Commands.Defend.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Defend.event			= "BI_Defend";
	BattleInterface.Commands.Defend.note			= LanguageConvertString(idLngFile, "sea_Defend");

	// LEO: Отдельная текстура буксировки сохраняет шлюпку и работает без нового HUD.
	BattleInterface.Commands.Tow.enable = false;
	BattleInterface.Commands.Tow.picNum = 0;
	BattleInterface.Commands.Tow.selPicNum = 1;
	BattleInterface.Commands.Tow.texNum = 4;
	BattleInterface.Commands.Tow.event = "BI_Tow";
	BattleInterface.Commands.Tow.note = XI_ConvertString("SeaTowOrder");

	BattleInterface.Commands.SailAway.enable		= false;
	BattleInterface.Commands.SailAway.picNum		= 50;
	BattleInterface.Commands.SailAway.selPicNum		= 34;
	BattleInterface.Commands.SailAway.texNum		= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.SailAway.event			= "BI_SailAway";
	BattleInterface.Commands.SailAway.note			= LanguageConvertString(idLngFile, "sea_SailAway");

	BattleInterface.Commands.HeaveToDrift.enable	= false;
	BattleInterface.Commands.HeaveToDrift.picNum	= 25;
	BattleInterface.Commands.HeaveToDrift.selPicNum	= 9;
	BattleInterface.Commands.HeaveToDrift.texNum	= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.HeaveToDrift.event		= "BI_HeaveToDrift";
	BattleInterface.Commands.HeaveToDrift.note		= LanguageConvertString(idLngFile, "sea_HeaveToDrift");

	BattleInterface.Commands.Reload.enable			= false;
	BattleInterface.Commands.Reload.picNum			= 48;
	BattleInterface.Commands.Reload.selPicNum		= 32;
	BattleInterface.Commands.Reload.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Reload.event			= "BI_Reload";
	BattleInterface.Commands.Reload.note			= LanguageConvertString(idLngFile, "sea_Reload");

	BattleInterface.Commands.Charge.enable			= false;
	BattleInterface.Commands.Charge.picNum			= 17;
	BattleInterface.Commands.Charge.selPicNum		= 1;
	BattleInterface.Commands.Charge.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Charge.event			= "BI_Charge";
	BattleInterface.Commands.Charge.note			= LanguageConvertString(idLngFile, "sea_Charge");

	BattleInterface.Commands.Speed.enable			= false;
	BattleInterface.Commands.Speed.picNum			= 22;
	BattleInterface.Commands.Speed.selPicNum		= 6;
	BattleInterface.Commands.Speed.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Speed.event			= "BI_Speed";
	BattleInterface.Commands.Speed.note				= LanguageConvertString(idLngFile, "sea_Speed");

	BattleInterface.Commands.Ability.enable			= false;
	BattleInterface.Commands.Ability.picNum			= 27;
	BattleInterface.Commands.Ability.selPicNum		= 11;
	BattleInterface.Commands.Ability.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Ability.event			= "BI_Ability";
	BattleInterface.Commands.Ability.note			= LanguageConvertString(idLngFile, "sea_Ability");

	BattleInterface.Commands.Boat.enable	= false;
	BattleInterface.Commands.Boat.picNum	= 26;
	BattleInterface.Commands.Boat.selPicNum	= 10;
	BattleInterface.Commands.Boat.texNum	= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Boat.event	= "BI_Boat";
	BattleInterface.Commands.Boat.note	= GetConvertStrWithReplace("Variable_BattleInterface_2", "Battle_Interface.txt", "#space#", " ");

	BattleInterface.Commands.Cabin.enable	= false;
	BattleInterface.Commands.Cabin.picNum	= 27;
	BattleInterface.Commands.Cabin.selPicNum	= 11;
	BattleInterface.Commands.Cabin.texNum	= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Cabin.event	= "BI_Cabin";
	BattleInterface.Commands.Cabin.note	    = GetConvertStrWithReplace("Variable_BattleInterface_3", "Battle_Interface.txt", "#space#", " ");
	BattleInterface.Commands.Bomb.enable	= false;
	BattleInterface.Commands.Bomb.picNum	= 67;
	BattleInterface.Commands.Bomb.selPicNum	= 66;
	BattleInterface.Commands.Bomb.texNum	= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Bomb.event		= "BI_Bomb";
	BattleInterface.Commands.Bomb.note	    = GetConvertStrWithReplace("Variable_BattleInterface_4", "Battle_Interface.txt", "#space#", " ");

	BattleInterface.Commands.ImmediateDeath.enable	= false;
	BattleInterface.Commands.ImmediateDeath.picNum	= 65;    // это чит
	BattleInterface.Commands.ImmediateDeath.selPicNum	= 64;
	BattleInterface.Commands.ImmediateDeath.texNum	= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.ImmediateDeath.event	= "BI_ImmDeath";
	BattleInterface.Commands.ImmediateDeath.note	= LanguageConvertString(idLngFile, "sea_ImmediateDeath");

	BattleInterface.Commands.Brander.enable			= false;
	BattleInterface.Commands.Brander.picNum			= 52;
	BattleInterface.Commands.Brander.selPicNum		= 36;
	BattleInterface.Commands.Brander.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Brander.event			= "BI_Brander";
    BattleInterface.Commands.Brander.note	= GetConvertStr("Brander", "AbilityDescribe.txt");

	BattleInterface.Commands.ImmediateReload.enable			= false;
	BattleInterface.Commands.ImmediateReload.picNum			= 53;
	BattleInterface.Commands.ImmediateReload.selPicNum		= 37;
	BattleInterface.Commands.ImmediateReload.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.ImmediateReload.event			= "BI_ImmediateReload";
    BattleInterface.Commands.ImmediateReload.note	= GetConvertStr("ImmediateReload", "AbilityDescribe.txt");

	BattleInterface.Commands.InstantBoarding.enable			= false;
	BattleInterface.Commands.InstantBoarding.picNum			= 30;   // это чит
	BattleInterface.Commands.InstantBoarding.selPicNum		= 14;
	BattleInterface.Commands.InstantBoarding.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.InstantBoarding.event			= "BI_InstantBoarding";
    BattleInterface.Commands.InstantBoarding.note	= GetConvertStr("InstantBoarding", "AbilityDescribe.txt");

	BattleInterface.Commands.LightRepair.enable			= false;
	BattleInterface.Commands.LightRepair.picNum			= 54;
	BattleInterface.Commands.LightRepair.selPicNum		= 38;
	BattleInterface.Commands.LightRepair.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.LightRepair.event			= "BI_LightRepair";
	BattleInterface.Commands.LightRepair.note	= GetConvertStr("LightRepair", "AbilityDescribe.txt");

	BattleInterface.Commands.InstantRepair.enable		= false;
	BattleInterface.Commands.InstantRepair.picNum		= 55;
	BattleInterface.Commands.InstantRepair.selPicNum	= 39;
	BattleInterface.Commands.InstantRepair.texNum		= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.InstantRepair.event		= "BI_InstantRepair";
	BattleInterface.Commands.InstantRepair.note	= GetConvertStr("InstantRepair", "AbilityDescribe.txt");

	BattleInterface.Commands.Turn180.enable				= false;
	BattleInterface.Commands.Turn180.picNum				= 56;
	BattleInterface.Commands.Turn180.selPicNum			= 40;
	BattleInterface.Commands.Turn180.texNum				= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.Turn180.event				= "BI_Turn180";
	BattleInterface.Commands.Turn180.note	= GetConvertStr("Turn180", "AbilityDescribe.txt");

	BattleInterface.Commands.SandbankManeuver.enable	= false;
	BattleInterface.Commands.SandbankManeuver.picNum	= 57;
	BattleInterface.Commands.SandbankManeuver.selPicNum	= 41;
	BattleInterface.Commands.SandbankManeuver.texNum	= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.SandbankManeuver.event		= "BI_SandbankManeuver";
	BattleInterface.Commands.SandbankManeuver.note	= GetConvertStr("SandbankManeuver", "AbilityDescribe.txt");

	BattleInterface.Commands.DirSail.enable		= false;
	BattleInterface.Commands.DirSail.picNum		= 60;
	BattleInterface.Commands.DirSail.selPicNum	= 60;
	BattleInterface.Commands.DirSail.event		= "BI_DirSail";
	BattleInterface.Commands.DirSail.note         = LanguageConvertString(idLngFile, "sea_DirSail");

	BattleInterface.Commands.SailDir.enable			= false;
	BattleInterface.Commands.SailDir.picNum			= 71;
	BattleInterface.Commands.SailDir.selPicNum		= 70;
	BattleInterface.Commands.SailDir.texNum			= BI_ICONS_TEXTURE_COMMAND;
	BattleInterface.Commands.SailDir.event			= "BI_SailDir";
	BattleInterface.Commands.SailDir.note			= LanguageConvertString(idLngFile, "sea_SailDir");

	LanguageCloseFile(idLngFile);
}

ref BI_GetChargeQuantity()
{
	int chIdx = GetEventData();
	ref chr = GetCharacter(chIdx);
	BI_ChargeState[0] = 4;
	BI_ChargeState[1] = GetCargoGoods(chr,GOOD_BALLS);
	BI_ChargeState[2] = GetCargoGoods(chr,GOOD_GRAPES);
	BI_ChargeState[3] = GetCargoGoods(chr,GOOD_KNIPPELS);
	BI_ChargeState[4] = GetCargoGoods(chr,GOOD_BOMBS);

	return &BI_ChargeState;
}

ref GetCurrentCharge() // вызывается каждый кадр
{
	BI_intNRetValue[0] = BI_icons_ProjectilesUpdate;

	float fState = Ship_GetSailState(pchar);
	if( fState < 0.33 ) {
		BI_intNRetValue[1] = 20;
	} else {
		if( fState < 0.66 ) {
			BI_intNRetValue[1] = 19;
		} else {
			BI_intNRetValue[1] = 18;
		}
	}
	BI_intNRetValue[2] = 61;

  	BI_intNRetValue[3] = 53;
	BI_intNRetValue[4] = sti(pchar.ship.cargo.goods.powder == 0);

	BI_intNRetValue[5] = sti(pchar.ship.cargo.goods.planks == 0);
	BI_intNRetValue[6] = sti(pchar.ship.cargo.goods.sailcloth == 0);

	BI_intNRetValue[7] = BI_CannonsUpdate;

	int aSelAmmo = sti(pchar.Ship.Cannons.Charge.Type); // 0..3 = GOOD_BALLS..GOOD_BOMBS
	string sSelIcon;
	BattleInterface.textinfo.ammocount0.text = "" + GetCargoGoods(pchar, GOOD_BALLS);
	BattleInterface.textinfo.ammocount1.text = "" + GetCargoGoods(pchar, GOOD_GRAPES);
	BattleInterface.textinfo.ammocount2.text = "" + GetCargoGoods(pchar, GOOD_KNIPPELS);
	BattleInterface.textinfo.ammocount3.text = "" + GetCargoGoods(pchar, GOOD_BOMBS);
	BattleInterface.textinfo.ammocount4.text = "" + GetCargoGoods(pchar, GOOD_POWDER);
	BattleInterface.textinfo.ammocount5.text = "" + GetCargoGoods(pchar, GOOD_WEAPON);
	BattleInterface.textinfo.ammocount6.text = "" + GetCargoGoods(pchar, GOOD_MEDICAMENT);
	BattleInterface.textinfo.ammocount7.text = "" + GetCargoGoods(pchar, GOOD_PLANKS);
	BattleInterface.textinfo.ammocount8.text = "" + GetCargoGoods(pchar, GOOD_SAILCLOTH);

	BattleInterface.imageslist.ammoicon0.empty = 0;
	BattleInterface.imageslist.ammoicon1.empty = 0;
	BattleInterface.imageslist.ammoicon2.empty = 0;
	BattleInterface.imageslist.ammoicon3.empty = 0;
	BattleInterface.imageslist.ammoicon4.empty = 0;
	BattleInterface.imageslist.ammoicon5.empty = 0;
	BattleInterface.imageslist.ammoicon6.empty = 0;
	BattleInterface.imageslist.ammoicon7.empty = 0;
	BattleInterface.imageslist.ammoicon8.empty = 0;

	int nNeedBI = 0;
	int nShipTypeBI = sti(pchar.ship.type);
	if (nShipTypeBI != SHIP_NOTUSED)
	{
		ref rBaseShipBI = GetRealShip(nShipTypeBI);
		nNeedBI = GetBortIntactCannonsNum(pchar, "fcannon", sti(rBaseShipBI.fcannon));
		nNeedBI = nNeedBI + GetBortIntactCannonsNum(pchar, "bcannon", sti(rBaseShipBI.bcannon));
		nNeedBI = nNeedBI + GetBortIntactCannonsNum(pchar, "lcannon", sti(rBaseShipBI.lcannon));
		nNeedBI = nNeedBI + GetBortIntactCannonsNum(pchar, "rcannon", sti(rBaseShipBI.rcannon));
	}

	if (nNeedBI > 0)
	{
		BattleInterface.imageslist.ammoicon4.empty = sti(GetCargoGoods(pchar, GOOD_POWDER) < nNeedBI);
		if (aSelAmmo >= GOOD_BALLS)
		{
			if (aSelAmmo <= GOOD_BOMBS)
			{
				sSelIcon = "ammoicon" + aSelAmmo;
				BattleInterface.imageslist.(sSelIcon).empty = sti(GetCargoGoods(pchar, aSelAmmo) < nNeedBI);
			}
		}
	}

	return &BI_intNRetValue;
}

ref biGetCharacterShipClass()
{
	int nChrIdx = GetEventData();
	BI_intRetValue = 1;
	if( nChrIdx>=0 && nChrIdx<TOTAL_CHARACTERS ) {
		BI_intRetValue = GetCharacterShipClass( &Characters[nChrIdx] );
	}
	return &BI_intRetValue;
}

bool BI_SetSeaState()
{
	bool bTmp;
	bSailTo = GetEventData();
	bTmp = GetEventData();
	bMapEnter = GetEventData();
	bAttack = GetEventData();
	bAbordage = bAttack;

	bDefend = GetEventData();
	bReloadCanBe = GetEventData();
	bi_nReloadTarget = GetEventData();
	bEnableIslandSailTo = bMapEnter;
	if(bDisableMapEnter)	bMapEnter = false;
	if(SeaTow_HasLinks()) bMapEnter = false;

    if(bMapEnter) seaAlarmed = false;
	if(CheckAttribute(pchar,"GhostCap.Fight")) bMapEnter = false;
    return bMapEnter;
}

void UpdateCompanionsStates()
{
	int i, cn;
	ref chref;

	if (CheckAttribute(pchar, "Ship.CmdActionAmmo")) DeleteAttribute(pchar, "Ship.CmdActionAmmo");
	if (CheckAttribute(pchar, "Ship.CmdActionTask")) DeleteAttribute(pchar, "Ship.CmdActionTask");
	bool bShow = (sti(InterfaceStates.ShipCapShowActionsPlus) == 1);
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar,i);
		if(cn>=0)
		{
			chref = GetCharacter(cn);
			if(bShow)
			{
				chref.Ship.CmdActionAmmo = GetChargedIcon(sti(chref.Ship.Cannons.Charge.Type));
				chref.Ship.CmdActionTask = getShipIconFromSeaAITask(sti(chref.SeaAI.Task));
			}
			else
			{
				chref.Ship.CmdActionAmmo = "";
				chref.Ship.CmdActionTask = "";
			}
		}
	}

	if (bSeaActive)
	{
		bool bCap = (sti(InterfaceStates.ShipCapShowActions) == 1);

		SendMessage(pchar, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");
		for (i = 1; i < COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(pchar, i);
			if (cn < 0) continue;
			chref = GetCharacter(cn);
			if (bCap)
			{
				string sCap = Ship_BuildActionCaption(chref, sti(chref.SeaAI.Task), sti(chref.Ship.Cannons.Charge.Type));
				SendMessage(chref, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, sCap);
			}
			else
			{
				SendMessage(chref, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");
			}
		}
	}
}

void SetShipPictureDataByShipTypeName(int N)
{
	BI_intNRetValue[0] = (N%16)*2 + (N/16)*32; BI_intNRetValue[1] = BI_intNRetValue[0] + 1;
	BI_intNRetValue[2] = BI_ICONS_TEXTURE_SHIP1;
	BI_intNRetValue[3] = false;
}

void SetShipPictureDataByShipType(int st)
{
	if(st>=0 && st<SHIP_TYPES_QUANTITY)
	{
		SetShipPictureDataByShipTypeName( st );
	}
}

ref BI_GetData()
{
	int dataType = GetEventData();
	int chrIdx;
	ref chRef;

	switch(dataType)
	{

	case BIDT_SHIPPICTURE:
		chRef = GetCharacter(GetEventData());

		if( CharacterIsDead(chRef) && sti(chRef.index) != GetMainCharacterIndex())
		{
            BI_intNRetValue[0] = 65;
            BI_intNRetValue[1] = 64; // выбранная черепушка
            BI_intNRetValue[2] = BI_ICONS_TEXTURE_COMMAND;
            BI_intNRetValue[3] = true;

			return &BI_intNRetValue;
			break;
		}
		int st = sti(chRef.Ship.Type);
		if (!CheckAttribute(&RealShips[st], "basetype")) Log_TestInfo("BIDT_SHIPPICTURE нет basetype у корабля НПС ID=" + chRef.id);
		st = sti(RealShips[st].basetype);
		SetShipPictureDataByShipType( st );
		break;
	case BIDT_GERALD_DATA:
		{
			chrIdx = GetEventData();
			if(chrIdx<0) break;
			chRef = GetCharacter(chrIdx);
			BI_intNRetValue[0] = 8;
			switch( SeaAI_GetRelation(chrIdx,nMainCharacterIndex) )
			{
				case RELATION_FRIEND:
					BI_intNRetValue[1] = 7;
				break;
				case RELATION_NEUTRAL:
					BI_intNRetValue[1] = -1;
				break;
				case RELATION_ENEMY:
					BI_intNRetValue[1] = 6;
				break;
			}
			BI_intNRetValue[2] = 8;
			switch( sti(chRef.nation) )
			{
				case ENGLAND:
					BI_intNRetValue[3] = 3;
				break;
				case FRANCE:
					BI_intNRetValue[3] = 2;
				break;
				case SPAIN:
					BI_intNRetValue[3] = 0;
				break;
				case HOLLAND:
					BI_intNRetValue[3] = 4;
				break;
				case PIRATE:
					BI_intNRetValue[3] = 5;
				break;
			}
		}
		break;
	}
	return &BI_intNRetValue;
}

void BI_LayoutExperience(aref owner,float ratio)
{
	int count=ExpLog_LineCount(true);
	int height=RecalculateVIcon(makeint(20*ratio*count));
	int y=RecalculateVIcon(makeint(290*ratio));
	if(y+height>sti(showWindow.bottom)) y=sti(showWindow.bottom)-height;
	if(y<sti(showWindow.top)) y=sti(showWindow.top);
	int x=sti(showWindow.right)-RecalculateHIcon(makeint(72*ratio));
	owner.expLogLines=count;
	// LEO: Все слоты доступны при перестроении HUD; скрытые строки имеют пустой текст.
	for(int i=1;i<=ExpLog_MaxLines();i++)
	{
		string line="EXP"+i;
		owner.textinfo.(line).font="interface_normal";
		owner.textinfo.(line).scale=1.2*ratio;
		owner.textinfo.(line).align="right";
		owner.textinfo.(line).pos.x=x;
		owner.textinfo.(line).pos.y=y+RecalculateVIcon(makeint(20*ratio*(i-1)));
		owner.textinfo.(line).refreshable=true;
	}
}

void SetParameterData()
{
	HUDLayout_Init();
	aref seaHUDOwner;
	makearef(seaHUDOwner,BattleInterface);
	NewSeaHUD_Clear(seaHUDOwner);
	float fHtRatio = stf(Render.screen_y) / screenscaling;
	int idLngFile = LanguageOpenFile("commands_name.txt");
	if(AltIntIcons) BattleInterface.CommandTextures.list.t0.name = "battle_interface\List_icons_Konshud.dds";
	else BattleInterface.CommandTextures.list.t0.name = "battle_interface\list_icons.dds";
	BattleInterface.CommandTextures.list.t0.xsize = 16;
	BattleInterface.CommandTextures.list.t0.ysize = 8;

	if (sti(InterfaceStates.InterfaceSelection)==POTC)
	{
		BattleInterface.CommandTextures.list.t1.name = "battle_interface\ship_icons1_PotC.dds";
	}
	else
	{
		if(AltIntIcons)
		{
			BattleInterface.CommandTextures.list.t1.name = "battle_interface\ship_icons1_Konshud.dds";
		}
		else
		{
			BattleInterface.CommandTextures.list.t1.name = "battle_interface\ship_icons1.dds";
		}
	}

	BattleInterface.CommandTextures.list.t1.xsize = 32;
	BattleInterface.CommandTextures.list.t1.ysize = 16;

	if(AltIntIcons) BattleInterface.CommandTextures.list.t2.name = "battle_interface\cancel_Konshud.dds";
	else BattleInterface.CommandTextures.list.t2.name = "battle_interface\cancel.dds";
	BattleInterface.CommandTextures.list.t2.xsize = 2;
	BattleInterface.CommandTextures.list.t2.ysize = 1;

	BattleInterface.CommandTextures.list.t3.name = "battle_interface\small_nations.dds";
	BattleInterface.CommandTextures.list.t3.xsize = 8;
	BattleInterface.CommandTextures.list.t3.ysize = 1;

	// LEO: Свой слот буксировки; исходные четыре текстуры команд остаются на прежних местах.
	string towIconStyle = "Standard";
	if(AltIntIcons) towIconStyle = "AltIcons";
	BattleInterface.CommandTextures.list.t4.name = "battle_interface\NewHUD\CommandIcons\Tow\"+towIconStyle+"\Legacy.dds";
	BattleInterface.CommandTextures.list.t4.xsize = 2;
	BattleInterface.CommandTextures.list.t4.ysize = 1;

	BattleInterface.charge.charge1.picNum = 19; // balls
	BattleInterface.charge.charge1.selPicNum = 3;
	BattleInterface.charge.charge2.picNum = 18; // grapes
	BattleInterface.charge.charge2.selPicNum = 2;
	BattleInterface.charge.charge3.picNum = 21; // "Knippels"
	BattleInterface.charge.charge3.selPicNum = 5;
	BattleInterface.charge.charge4.picNum = 20; // bombs
	BattleInterface.charge.charge4.selPicNum = 4;

	BattleInterface.CommandTextures.ChargeTexNum = 0;
	BattleInterface.CommandTextures.CommandTexNum = 0;
	aref commandSkinOwner;
	makearef(commandSkinOwner,BattleInterface);
	CommandIcons_Setup(commandSkinOwner,1);

	BattleInterface.CommandShowParam.maxShowQuantity = 10;
	BattleInterface.CommandShowParam.iconDistance = makeint(4 * fHtRatio);
	BattleInterface.CommandShowParam.iconWidth = RecalculateHIcon(makeint(64 * fHtRatio));
	BattleInterface.CommandShowParam.iconHeight = RecalculateVIcon(makeint(64 * fHtRatio));
	BattleInterface.CommandShowParam.leftIconsOffset = sti(showWindow.left)+RecalculateHIcon(makeint(16 * fHtRatio));
	BattleInterface.CommandShowParam.downIconsOffset = sti(showWindow.bottom)-RecalculateVIcon(makeint(80 * fHtRatio));
	BattleInterface.CommandShowParam.buttonWidth = RecalculateHIcon(makeint(8 * fHtRatio));
	BattleInterface.CommandShowParam.buttonHeight = RecalculateVIcon(makeint(64 * fHtRatio));
	BattleInterface.CommandShowParam.buttonOffset = RecalculateHIcon(makeint(4 * fHtRatio));
	BattleInterface.CommandShowParam.buttonTexture = "battle_interface\lr_buttons.dds";
	BattleInterface.CommandShowParam.shipStateWidth = RecalculateHIcon(makeint(64 * fHtRatio));
	BattleInterface.CommandShowParam.shipStateHeight = RecalculateVIcon(makeint(5 * fHtRatio)); // LEO: высота ОДНОГО бара (3 бара = 3×), 5px компактно
	BattleInterface.CommandShowParam.shipStateTexture = "battle_interface\indicators.dds";
	BattleInterface.CommandShowParam.shipStateOffset = RecalculateVIcon(0);

	BattleInterface.CommandShowParam.shipStateColor = argb(255,128,128,128); // цвет-модулятор (как у иконок)
	BattleInterface.CommandShowParam.shipStateBackUV = "0.0,0.5,1.0,0.666";  // LEO: ОДИН серый трек (низ текстуры = 3 одинаковых трека по 0.166), иначе под каждый бар лезут все три
	BattleInterface.CommandShowParam.shipStateHullUV = "0.0,0.333,1.0,0.5";  // корпус
	BattleInterface.CommandShowParam.shipStateSailUV = "0.0,0.167,1.0,0.333";// паруса
	BattleInterface.CommandShowParam.shipStateCrewUV = "0.0,0.0,1.0,0.166";  // команда

	BattleInterface.CommandShowParam.captainFaceShow = 1;
	BattleInterface.CommandShowParam.captainFaceSize = "" + RecalculateHIcon(makeint(48 * fHtRatio)) + "," + RecalculateVIcon(makeint(48 * fHtRatio));
	BattleInterface.CommandShowParam.captainFaceOffset = "0," + RecalculateVIcon(makeint(22 * fHtRatio)); // ниже флажка нации
	BattleInterface.CommandShowParam.captainFaceColor = argb(255,128,128,128);
	BattleInterface.CommandShowParam.captainFaceUV = "0.0,0.0,1.0,1.0";

	if( BI_UseRoundFaces() )
	{
		BattleInterface.CommandShowParam.captainFaceBorder = "battle_interface\Ship_Border_circle.dds";
	}
	else
	{
		BattleInterface.CommandShowParam.captainFaceBorder = "battle_interface\Ship_Border.dds";
	}
	BattleInterface.CommandShowParam.captainFaceBorderSize = "" + RecalculateHIcon(makeint(53 * fHtRatio)) + "," + RecalculateVIcon(makeint(53 * fHtRatio));
	BattleInterface.CommandShowParam.captainFaceBorderOffset = "0,0"; // подгонка рамки к портрету
	BattleInterface.CommandShowParam.captainFaceBorderColor = argb(255,128,128,128);
	BattleInterface.CommandShowParam.captainFaceBorderUV = "0.0,0.0,1.0,1.0";
	BattleInterface.CommandShowParam.GeraldWidth = RecalculateHIcon(makeint(32 * fHtRatio));
	BattleInterface.CommandShowParam.GeraldHeight = RecalculateVIcon(makeint(32 * fHtRatio));
	BattleInterface.CommandShowParam.commandFont = "bold_numbers";
	BattleInterface.CommandShowParam.printXOffset = RecalculateHIcon(makeint(32 * fHtRatio));
	BattleInterface.CommandShowParam.printYOffset = RecalculateVIcon(makeint(-26 * fHtRatio));
	BattleInterface.CommandShowParam.commandNoteFont = "interface_normal";
	BattleInterface.CommandShowParam.noteXOffset = RecalculateHIcon(0);
	BattleInterface.CommandShowParam.noteYOffset = RecalculateVIcon(makeint(-30 * fHtRatio));
	BattleInterface.CommandShowParam.argbTFactorColor = argb(256,64,64,64);

	BattleInterface.UserIcons.ui1.enable = true;
	BattleInterface.UserIcons.ui1.pic = 25;
	BattleInterface.UserIcons.ui1.selpic = 9;
	BattleInterface.UserIcons.ui1.tex = 0;
	BattleInterface.UserIcons.ui1.name = "sail_none";

	BattleInterface.UserIcons.ui2.enable = true;
	BattleInterface.UserIcons.ui2.pic = 24;
	BattleInterface.UserIcons.ui2.selpic = 8;
	BattleInterface.UserIcons.ui2.tex = 0;
	BattleInterface.UserIcons.ui2.name = "sail_midi";

	BattleInterface.UserIcons.ui3.enable = true;
	BattleInterface.UserIcons.ui3.pic = 23;
	BattleInterface.UserIcons.ui3.selpic = 7;
	BattleInterface.UserIcons.ui3.tex = 0;
	BattleInterface.UserIcons.ui3.name = "sail_fast";
	BattleInterface.UserIcons.ui4.enable = true;
	BattleInterface.UserIcons.ui4.selpic = 96;
	BattleInterface.UserIcons.ui4.pic = 112;
	BattleInterface.UserIcons.ui4.tex = BI_ICONS_TEXTURE_COMMAND;;
	BattleInterface.UserIcons.ui4.name = "dir_n";
	BattleInterface.UserIcons.ui4.note = LanguageConvertString(idLngFile, "sea_SailN");

	BattleInterface.UserIcons.ui5.enable = true;
	BattleInterface.UserIcons.ui5.selpic = 97;
	BattleInterface.UserIcons.ui5.pic = 113;
	BattleInterface.UserIcons.ui5.tex = BI_ICONS_TEXTURE_COMMAND;;
	BattleInterface.UserIcons.ui5.name = "dir_ne";
	BattleInterface.UserIcons.ui5.note = LanguageConvertString(idLngFile, "sea_SailNE");

	BattleInterface.UserIcons.ui6.enable = true;
	BattleInterface.UserIcons.ui6.selpic = 98;
	BattleInterface.UserIcons.ui6.pic = 114;
	BattleInterface.UserIcons.ui6.tex = BI_ICONS_TEXTURE_COMMAND;;
	BattleInterface.UserIcons.ui6.name = "dir_e";
	BattleInterface.UserIcons.ui6.note = LanguageConvertString(idLngFile, "sea_SailE");

	BattleInterface.UserIcons.ui7.enable = true;
	BattleInterface.UserIcons.ui7.selpic = 99;
	BattleInterface.UserIcons.ui7.pic = 115;
	BattleInterface.UserIcons.ui7.tex = BI_ICONS_TEXTURE_COMMAND;;
	BattleInterface.UserIcons.ui7.name = "dir_se";
	BattleInterface.UserIcons.ui7.note = LanguageConvertString(idLngFile, "sea_SailSE");

	BattleInterface.UserIcons.ui8.enable = true;
	BattleInterface.UserIcons.ui8.selpic = 100;
	BattleInterface.UserIcons.ui8.pic = 116;
	BattleInterface.UserIcons.ui8.tex = BI_ICONS_TEXTURE_COMMAND;;
	BattleInterface.UserIcons.ui8.name = "dir_s";
	BattleInterface.UserIcons.ui8.note = LanguageConvertString(idLngFile, "sea_SailS");

	BattleInterface.UserIcons.ui9.enable = true;
	BattleInterface.UserIcons.ui9.selpic = 101;
	BattleInterface.UserIcons.ui9.pic = 117;
	BattleInterface.UserIcons.ui9.tex = BI_ICONS_TEXTURE_COMMAND;;
	BattleInterface.UserIcons.ui9.name = "dir_sw";
	BattleInterface.UserIcons.ui9.note = LanguageConvertString(idLngFile, "sea_SailSW");

    BattleInterface.UserIcons.ui10.enable = true;
	BattleInterface.UserIcons.ui10.selpic = 102;
	BattleInterface.UserIcons.ui10.pic = 118;
	BattleInterface.UserIcons.ui10.tex = BI_ICONS_TEXTURE_COMMAND;;
	BattleInterface.UserIcons.ui10.name = "dir_w";
	BattleInterface.UserIcons.ui10.note = LanguageConvertString(idLngFile, "sea_SailW");

	BattleInterface.UserIcons.ui11.enable = true;
	BattleInterface.UserIcons.ui11.selpic = 103;
	BattleInterface.UserIcons.ui11.pic = 119;
	BattleInterface.UserIcons.ui11.tex = BI_ICONS_TEXTURE_COMMAND;;
	BattleInterface.UserIcons.ui11.name = "dir_nw";
	BattleInterface.UserIcons.ui11.note = LanguageConvertString(idLngFile, "sea_SailNW");

	BattleInterface.MessageIcons.IconWidth = RecalculateHIcon(makeint(64 * fHtRatio));
	BattleInterface.MessageIcons.IconHeight = RecalculateVIcon(makeint(24 * fHtRatio));
	BattleInterface.MessageIcons.IconDist = RecalculateVIcon(2);
	BattleInterface.MessageIcons.IconBottom = sti(showWindow.bottom)-RecalculateVIcon(makeint((80+20) * fHtRatio));
	BattleInterface.MessageIcons.IconMaxQuantity = 4;
	BattleInterface.MessageIcons.BlendTime = 3.0;
	BattleInterface.MessageIcons.FallSpeed = 22.0;
	BattleInterface.MessageIcons.argbHighBlind = argb(255,128,128,128);
	BattleInterface.MessageIcons.argbLowBlind = argb(255,68,68,68);
	BattleInterface.MessageIcons.BlindUpTime = 0.5;
	BattleInterface.MessageIcons.BlindDownTime = 1.0;
	BattleInterface.MessageIcons.texture = "battle_interface\MessageIcons.dds";
	BattleInterface.MessageIcons.TexHSize = 2;
	BattleInterface.MessageIcons.TexVSize = 2;

	BattleInterface.navigation.aspectRatio				= showWindow.aspectRatio;
	BattleInterface.navigation.navigatorWidth			= RecalculateHIcon(makeint(216 * fHtRatio));
	BattleInterface.navigation.navigatorHeight			= RecalculateVIcon(makeint(216 * fHtRatio));
	BattleInterface.navigation.rightPos					= sti(showWindow.right) + RecalculateHIcon(makeint(-10 * fHtRatio));
	BattleInterface.navigation.topPos					= sti(showWindow.top) - RecalculateVIcon(makeint(24 * fHtRatio));
	BattleInterface.navigation.speedShowFont			= "bold_numbers";
	BattleInterface.navigation.speedOutYOffset			= RecalculateVIcon(makeint(180 * fHtRatio));
	BattleInterface.navigation.shipSpeedXOffset			= RecalculateHIcon(makeint(25 * fHtRatio));
	BattleInterface.navigation.windSpeedXOffset			= RecalculateHIcon(makeint(-25 * fHtRatio));
	BattleInterface.navigation.fontScale				= 0.7 * fHtRatio;
	BattleInterface.navigation.windWidth				= makeint(32 * fHtRatio);
	BattleInterface.navigation.windHeight				= makeint(128 * fHtRatio);

	BattleInterface.navigation.compasTexture			= "battle_interface\compass.dds";
	BattleInterface.navigation.cannonsTexture			= "battle_interface\indicators_cannons_reload.dds";
	BattleInterface.navigation.emptyTexture				= "battle_interface\indicators_dark_and_center_ship.dds";
	BattleInterface.navigation.windTexture				= "battle_interface\wind_pointer.dds";

	BattleInterface.navigation.leftChargeBegAngle		= 214;
	BattleInterface.navigation.leftChargeEndAngle		= 326;
	BattleInterface.navigation.rightChargeBegAngle		= 146;
	BattleInterface.navigation.rightChargeEndAngle		= 34;
	BattleInterface.navigation.forwardChargeBegAngle	= 343;
	BattleInterface.navigation.forwardChargeEndAngle	= 378;
	BattleInterface.navigation.backwardChargeBegAngle	= 198;
	BattleInterface.navigation.backwardChargeEndAngle	= 162;
	BattleInterface.navigation.mapRadius				= makeint(48 * fHtRatio);
	BattleInterface.navigation.horizontRadius			= 400;
	BattleInterface.navigation.minScale					= 0.5;
	BattleInterface.navigation.maxScale					= 8;   // LEO: было 4 — шире отдаление миникарты
	BattleInterface.navigation.curScale					= 2;
	BattleInterface.navigation.scaleStep				= 2.5; // LEO: теперь единиц/сек (зум × dt, не зависит от FPS); меньше = медленнее/мельче
	BattleInterface.navigation.scaleSmooth				= 8.0; // LEO: скорость плавного зума (больше = быстрее догоняет цель)
	BattleInterface.navigation.argbReadyCannonColor		= argb(255,0,155,0);
	BattleInterface.navigation.argbChargeCannonColor	= argb(255,0,255,0);
	BattleInterface.navigation.argbDamageCannonColor	= argb(0,190,50,10);
	BattleInterface.navigation.argbSeaColor				= argb(255,10,30,80);
	BattleInterface.navigation.argbFireZoneColor		= argb(60,250,250,250);
	BattleInterface.navigation.argbEnemyShipColor		= argb(255,255,0,0);
	BattleInterface.navigation.argbFrendShipColor		= argb(255,0,255,0);
	BattleInterface.navigation.argbNeutralShipColor		= argb(255,128,128,128);
	BattleInterface.navigation.argbDeadShipColor		= argb(255,0,0,255);
	BattleInterface.navigation.argbBackMaxColor			= argb(255,0,0,128);
	BattleInterface.navigation.argbBackMinColor			= argb(0,0,0,128);
	BattleInterface.navigation.shipShowRadius			= 8.0;

	BattleInterface.navigation.sailstateTexture = CommandIcons_Texture("LIST_ICON2");
	BattleInterface.navigation.sailstateTextureGreed	= "8,8";
	BattleInterface.navigation.sailstatePos				= RecalculateHIcon(makeint(25* fHtRatio))+","+RecalculateVIcon(makeint(160* fHtRatio));
	BattleInterface.navigation.sailstatePictureSize		= RecalculateHIcon(makeint(40* fHtRatio))+","+RecalculateVIcon(makeint(40* fHtRatio));

	BattleInterface.navigation.windStateTexture = CommandIcons_Texture("LIST_ICON2");
	BattleInterface.navigation.windTextureGreed			= "8,8";
	BattleInterface.navigation.windPos					= RecalculateHIcon(makeint(-25* fHtRatio))+","+RecalculateVIcon(makeint(160* fHtRatio));
	BattleInterface.navigation.windPictureSize			= RecalculateHIcon(makeint(40* fHtRatio))+","+RecalculateVIcon(makeint(40* fHtRatio));

	BattleInterface.textinfo.Date.font = "interface_button";
	BattleInterface.textinfo.Date.scale = 1.0 * fHtRatio;
	BattleInterface.textinfo.Date.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(120* fHtRatio));
	BattleInterface.textinfo.Date.pos.y = RecalculateVIcon(makeint(170* fHtRatio));
	BattleInterface.textinfo.Date.text = XI_convertString("Date:")+GetQuestBookData();
	BattleInterface.textinfo.Date.refreshable = true;

	BattleInterface.textinfo.Location.font = "interface_button";
	BattleInterface.textinfo.Location.scale = 1.0* fHtRatio;
	BattleInterface.textinfo.Location.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(120* fHtRatio));
	BattleInterface.textinfo.Location.pos.y = RecalculateVIcon(makeint(190* fHtRatio));
	BattleInterface.textinfo.Location.text = GetBILocationName();
	BattleInterface.textinfo.Location.refreshable = true;

	BattleInterface.textinfo.Speed.font = "interface_button";
	BattleInterface.textinfo.Speed.scale = 1.0 * fHtRatio;
	BattleInterface.textinfo.Speed.color = argb(255,255,255,255);
	BattleInterface.textinfo.Speed.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(138* fHtRatio));
	BattleInterface.textinfo.Speed.pos.y = RecalculateVIcon(makeint(337* fHtRatio));
	BattleInterface.textinfo.Speed.text = "";

	BI_LayoutExperience(&BattleInterface,fHtRatio);

	bool bHorzBars = false;
	if(CheckAttribute(&InterfaceStates,"ShipBarsHorizontal")) { if(sti(InterfaceStates.ShipBarsHorizontal)==1) bHorzBars = true; }

	int iCompStride = 110;
	int iCompX = 132;
	int iPortBaseY = 68;
	if(sti(InterfaceStates.InterfaceSelection)==KRUGLYI)
	{
		iCompStride = 112;
		iPortBaseY = 66;
	}
	else
	{
		if(bHorzBars) { iCompStride = 105; iCompX = 123; }
	}
	int iGlyphDX = iCompX - 70;                      // 70 = ShipIcon.iconoffset*.x (X центра портрета)
	int iAmmoDY  = 148 - iCompStride - iPortBaseY;   // -30 (верт/круг), -25 (гориз-бары)
	int iTaskDY  = 186 - iCompStride - iPortBaseY;   // +8 / +13
	int iGlyphAbsX = sti(showWindow.left) + RecalculateHIcon(makeint(iGlyphDX * fHtRatio));
	int iAmmoAbsY  = sti(showWindow.top)  + RecalculateVIcon(makeint(iAmmoDY * fHtRatio));
	int iTaskAbsY  = sti(showWindow.top)  + RecalculateVIcon(makeint(iTaskDY * fHtRatio));
	BattleInterface.ShipIcon.actionfontid    = "interface_normal";
	BattleInterface.ShipIcon.actionfontcolor = argb(255,255,255,255);
	BattleInterface.ShipIcon.actionfontscale = 1.2 * fHtRatio;
	BattleInterface.ShipIcon.actionammooffset = iGlyphAbsX + "," + iAmmoAbsY;
	BattleInterface.ShipIcon.actiontaskoffset = iGlyphAbsX + "," + iTaskAbsY;

	string ammoTex = CommandIcons_Texture("LIST_ICON2");
	int ammoPic[9];
	ammoPic[0]=32; ammoPic[1]=33; ammoPic[2]=34; ammoPic[3]=35; ammoPic[4]=53; ammoPic[5]=36; ammoPic[6]=37; ammoPic[7]=38; ammoPic[8]=39;
	int aiW = RecalculateHIcon(makeint(40 * fHtRatio));
	int aiH = RecalculateVIcon(makeint(40 * fHtRatio));
	int aiGap = RecalculateHIcon(makeint(8 * fHtRatio));
	int aiStride = aiW + aiGap;
	int aiRowW = 9*aiW + 8*aiGap;
	int aiX0 = (sti(showWindow.left)+sti(showWindow.right))/2 - aiRowW/2;
	int aiY0 = sti(showWindow.bottom) - aiH - RecalculateVIcon(makeint(14 * fHtRatio));
	aiX0 = aiX0 + HUDLayout_GetOffset("sea","resources","x");
	aiY0 = aiY0 + HUDLayout_GetOffset("sea","resources","y");
	int aiI; int aiPic; int aiCol; int aiRow; string aiUV; int aiX; string aiNm; string aiCnt;

	int amW = RecalculateHIcon(makeint(18 * fHtRatio));
	int amH = RecalculateVIcon(makeint(11 * fHtRatio));
	int amY = aiY0 + aiH + RecalculateVIcon(makeint(1 * fHtRatio));
	int amX0; int amX1; string amSlot;
	int amColor = argb(255,255,216,128); // LEO: цвет выделения - общий для подсвеченной иконки и стрелки
	for(aiI=0; aiI<9; aiI++)
	{
		aiPic = ammoPic[aiI];
		aiCol = aiPic % 8;
		aiRow = aiPic / 8;
		aiUV = FloatToString(stf(aiCol)/8.0,3)+","+FloatToString(stf(aiRow)/8.0,3)+","+FloatToString(stf(aiCol+1)/8.0,3)+","+FloatToString(stf(aiRow+1)/8.0,3);
		aiX = aiX0 + aiI*aiStride;
		aiNm = "ammoicon"+aiI;
		BattleInterface.imageslist.(aiNm).texture = ammoTex;
		BattleInterface.imageslist.(aiNm).color = argb(255,128,128,128);
		BattleInterface.imageslist.(aiNm).uv = aiUV;
		BattleInterface.imageslist.(aiNm).pos = ""+aiX+","+aiY0+","+(aiX+aiW)+","+(aiY0+aiH);
		if (aiI < 4)
		{
			amX0 = aiX + aiW/2 - amW/2;
			amX1 = amX0 + amW;
			amSlot = "slot" + aiI;
			BattleInterface.ammomark.(amSlot) = ""+amX0+","+amY+","+amX1+","+(amY+amH);
		}

		aiCnt = "ammocount"+aiI;
		BattleInterface.textinfo.(aiCnt).font = "interface_normal";
		BattleInterface.textinfo.(aiCnt).scale = 1.0 * fHtRatio;
		BattleInterface.textinfo.(aiCnt).color = argb(255,255,255,255);
		BattleInterface.textinfo.(aiCnt).pos.x = aiX + aiW/2;
		BattleInterface.textinfo.(aiCnt).pos.y = aiY0 - RecalculateVIcon(makeint(15 * fHtRatio));
		BattleInterface.textinfo.(aiCnt).refreshable = true;
		BattleInterface.textinfo.(aiCnt).text = "";
	}

	BattleInterface.ammohighlight.normalcolor = argb(255,128,128,128); // обычная иконка (как у остальных)
	BattleInterface.ammohighlight.flashcolor = argb(255,255,255,255);  // вспышка при выборе
	BattleInterface.ammohighlight.highcolor = amColor;                 // выделенная: тёплое золото (от обычных 128,128,128 отличается заметно)
	BattleInterface.ammohighlight.flashtime = 0.12;                    // длит. вспышки, сек
	BattleInterface.ammohighlight.settletime = 0.55;                   // длит. затухания вспышка->выделенный, сек (подольше)
	BattleInterface.ammohighlight.shake = 6;                           // амплитуда шейка во вспышке, px (0 = выкл)
	BattleInterface.ammohighlight.used = 0;

	BattleInterface.imageslist.ammomark.texture = "\battle_interface\arrowly.dds";
	BattleInterface.imageslist.ammomark.uv = "1.000,1.000,0.000,0.000";
	BattleInterface.imageslist.ammomark.color = amColor;

	amSlot = "slot0";
	BattleInterface.imageslist.ammomark.pos = BattleInterface.ammomark.(amSlot);
	BattleInterface.ammomark.icon = "ammomark";
	BattleInterface.ammomark.color = amColor;
	BattleInterface.ammomark.used = 0;
	BI_UpdateAmmoHighlight();

	BattleInterface.textinfo.CannonsNumL.font = "bold_numbers";
	BattleInterface.textinfo.CannonsNumL.scale = 0.55 * fHtRatio;
	BattleInterface.textinfo.CannonsNumL.color = argb(255,255,255,255);
	BattleInterface.textinfo.CannonsNumL.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(212* fHtRatio));
	BattleInterface.textinfo.CannonsNumL.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(80* fHtRatio));;
	BattleInterface.textinfo.CannonsNumL.refreshable = true;

	BattleInterface.textinfo.CannonsNumR.font = "bold_numbers";
	BattleInterface.textinfo.CannonsNumR.scale = 0.55 * fHtRatio;
	BattleInterface.textinfo.CannonsNumR.color = argb(255,255,255,255);
	BattleInterface.textinfo.CannonsNumR.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(26* fHtRatio));
	BattleInterface.textinfo.CannonsNumR.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(80* fHtRatio));;
	BattleInterface.textinfo.CannonsNumR.refreshable = true;

	BattleInterface.textinfo.CannonsNumB.font = "bold_numbers";
	BattleInterface.textinfo.CannonsNumB.scale = 0.55 * fHtRatio;
	BattleInterface.textinfo.CannonsNumB.color = argb(255,255,255,255);
	BattleInterface.textinfo.CannonsNumB.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(120* fHtRatio));
	BattleInterface.textinfo.CannonsNumB.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(158* fHtRatio));;
	BattleInterface.textinfo.CannonsNumB.refreshable = true;

	BattleInterface.textinfo.CannonsNumF.font = "bold_numbers";
	BattleInterface.textinfo.CannonsNumF.scale = 0.55 * fHtRatio;
	BattleInterface.textinfo.CannonsNumF.color = argb(255,255,255,255);
	BattleInterface.textinfo.CannonsNumF.pos.x = sti(showWindow.right) - RecalculateHIcon(makeint(120* fHtRatio));
	BattleInterface.textinfo.CannonsNumF.pos.y = sti(showWindow.top) + RecalculateVIcon(makeint(0* fHtRatio));;
	BattleInterface.textinfo.CannonsNumF.refreshable = true;

	BattleInterface.LineInfo.speed.color = "";
	BattleInterface.LineInfo.speed.begin.x = "";
	BattleInterface.LineInfo.speed.begin.y = "";
	BattleInterface.LineInfo.speed.end.x = "";
	BattleInterface.LineInfo.speed.end.y = "";

	BattleInterface.LineInfo.Ammo.color = "";
	BattleInterface.LineInfo.Ammo.begin.x = "";
	BattleInterface.LineInfo.Ammo.begin.y = "";
	BattleInterface.LineInfo.Ammo.end.x = "";
	BattleInterface.LineInfo.Ammo.end.y = "";

	if( CheckAttribute(&InterfaceStates,"ShowBattleMode") ) {
		BattleInterface.battleborder.used = InterfaceStates.ShowBattleMode;
	} else {
		BattleInterface.battleborder.used = false;
	}
	BattleInterface.battleborder.color1 = argb(255,255,255,255);
	BattleInterface.battleborder.color2 = argb(155,255,255,255);
	BattleInterface.battleborder.extpos = "0,0," + ShowWindow.right + "," + ShowWindow.bottom;
	BattleInterface.battleborder.intpos1 =
		RecalculateHIcon(20) + "," +
		RecalculateVIcon(20) + "," +
		(sti(ShowWindow.right)-RecalculateHIcon(20)) + "," +
		(sti(ShowWindow.bottom)-RecalculateVIcon(20));
	BattleInterface.battleborder.intpos2 =
		RecalculateHIcon(10) + "," +
		RecalculateVIcon(10) + "," +
		(sti(ShowWindow.right)-RecalculateHIcon(10)) + "," +
		(sti(ShowWindow.bottom)-RecalculateVIcon(10));
	BattleInterface.battleborder.speed = 1.5;
	BattleInterface.battleborder.texture = "battle_interface\battleborder.dds";

	if (sti(InterfaceStates.InterfaceSelection)==KVADRAT)
	{

		BattleInterface.ShipIcon.backtexturename		= "battle_interface\Ship_Border.dds"; // Основа
		BattleInterface.ShipIcon.backcolor				= argb(255,128,128,128);
		BattleInterface.ShipIcon.backuv					= "0.0,0.0,1.0,1.0";
		BattleInterface.ShipIcon.backoffset				= "0,0";
		BattleInterface.ShipIcon.backiconsize			= RecalculateHIcon(makeint(80* fHtRatio))+","+RecalculateVIcon(makeint(80* fHtRatio));

		if(bHorzBars)
		{

			BattleInterface.ShipIcon.sailorfontid			= "bold_numbers";
			BattleInterface.ShipIcon.sailorfontcolor		= argb(255,255,255,255); // Цвет количества матросов
			BattleInterface.ShipIcon.sailorfontscale		= 0.45 * fHtRatio;
			BattleInterface.ShipIcon.sailorfontoffset		= RecalculateHIcon(makeint(53* fHtRatio))+","+RecalculateVIcon(makeint(-49* fHtRatio));

			BattleInterface.ShipIcon.shipnamefontid			= "interface_normal";
			BattleInterface.ShipIcon.shipnamefontcolor		= argb(255,255,255,255); // Цвет имени корабля
			BattleInterface.ShipIcon.shipnamefontscale		= 1.0 * fHtRatio;
			BattleInterface.ShipIcon.shipnamefontoffset		= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(-69* fHtRatio));
		}
		else
		{

			BattleInterface.ShipIcon.sailorfontid			= "bold_numbers";
			BattleInterface.ShipIcon.sailorfontcolor		= argb(255,255,255,255); // Цвет количества матросов
			BattleInterface.ShipIcon.sailorfontscale		= 0.65 * fHtRatio;
			BattleInterface.ShipIcon.sailorfontoffset		= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(42* fHtRatio));

			BattleInterface.ShipIcon.shipnamefontid			= "interface_normal";
			BattleInterface.ShipIcon.shipnamefontcolor		= argb(255,255,255,255); // Цвет имени корабля
			BattleInterface.ShipIcon.shipnamefontscale		= 1.0 * fHtRatio;
			BattleInterface.ShipIcon.shipnamefontoffset		= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(-56* fHtRatio));
		}

		if(bHorzBars)
		{

			BattleInterface.ShipIcon.shipstatetexturename	= "battle_interface\indicators.dds";
			BattleInterface.ShipIcon.shipstatecolor			= argb(255,128,128,128);
			BattleInterface.ShipIcon.shipstatehorizontal	= 1;
			BattleInterface.ShipIcon.shipstatebackuv		= "0.0,0.5,1.0,0.666";    // LEO: ОДИН серый трек (низ текстуры = 3 одинаковых трека по 0.166, берём первый), иначе под бар лезут все три
			BattleInterface.ShipIcon.shipstatebackcolor		= argb(180,0,0,0);        // чёрный + альфа
			BattleInterface.ShipIcon.shiphpuv				= "0.0,0.333,1.0,0.5";   // корпус (верх)
			BattleInterface.ShipIcon.shiphpoffset			= RecalculateHIcon(0.0* fHtRatio)+","+RecalculateVIcon(-49* fHtRatio);
			BattleInterface.ShipIcon.shiphpiconsize			= RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(6* fHtRatio));
			BattleInterface.ShipIcon.shipspuv				= "0.0,0.167,1.0,0.333"; // паруса (середина)
			BattleInterface.ShipIcon.shipspoffset			= RecalculateHIcon(0.0* fHtRatio)+","+RecalculateVIcon(-44* fHtRatio);
			BattleInterface.ShipIcon.shipspiconsize			= RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(6* fHtRatio));
			BattleInterface.ShipIcon.shipcrewuv				= "0.0,0.0,1.0,0.166";   // команда (низ, у иконки)
			BattleInterface.ShipIcon.shipcrewoffset			= RecalculateHIcon(0.0* fHtRatio)+","+RecalculateVIcon(-39* fHtRatio);
			BattleInterface.ShipIcon.shipcrewiconsize		= RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(6* fHtRatio));
		}
		else
		{
			BattleInterface.ShipIcon.shipstatetexturename	= "battle_interface\ShipHP_SP.dds"; // Полоски HP/SP
			BattleInterface.ShipIcon.shipstatecolor			= argb(255,128,128,128);
			BattleInterface.ShipIcon.shipstatehorizontal	= 0;
			BattleInterface.ShipIcon.shiphpuv				= "0.0,0.0,0.5,1.0";
			BattleInterface.ShipIcon.shiphpoffset			= RecalculateHIcon(-43.0* fHtRatio)+","+RecalculateVIcon(0.0* fHtRatio);
			BattleInterface.ShipIcon.shiphpiconsize			= RecalculateHIcon(makeint(6* fHtRatio))+","+RecalculateVIcon(makeint(80* fHtRatio));
			BattleInterface.ShipIcon.shipspuv				= "0.5,0.0,1.0,1.0";
			BattleInterface.ShipIcon.shipspoffset			= RecalculateHIcon(43.0* fHtRatio)+","+RecalculateVIcon(0.0* fHtRatio);
			BattleInterface.ShipIcon.shipspiconsize			= RecalculateHIcon(makeint(6* fHtRatio))+","+RecalculateVIcon(makeint(80* fHtRatio));
			BattleInterface.ShipIcon.shipcrewiconsize		= "0,0"; // команда-бар скрыт в вертик. режиме
		}

		BattleInterface.ShipIcon.shipclassiconsize		= "0,0";
		BattleInterface.ShipIcon.gunchargeprogress		= "0.0625, 0.219, 0.359, 0.5, 0.641, 0.781, 0.983";

		BattleInterface.ShipIcon.shiptexturename		= "battle_interface\ship_icons2.dds"; // Иконка корабля
		BattleInterface.ShipIcon.shipcolor				= argb(255,128,128,128);
		BattleInterface.ShipIcon.shipoffset				= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(0* fHtRatio));
		BattleInterface.ShipIcon.shipiconsize			= RecalculateHIcon(makeint(80* fHtRatio))+","+RecalculateVIcon(makeint(80* fHtRatio));
		BattleInterface.ShipIcon.xsize = 32;
		BattleInterface.ShipIcon.ysize = 32;

		BattleInterface.ShipIcon.commandlistverticaloffset =  makeint(-33 * fHtRatio);

		if(bHorzBars)
		{

			BattleInterface.ShipIcon.shipiconsize = RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(74* fHtRatio));
			BattleInterface.ShipIcon.backiconsize = RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(74* fHtRatio));
			BattleInterface.ShipIcon.iconoffset1 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(68* fHtRatio));
			BattleInterface.ShipIcon.iconoffset2 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(173* fHtRatio));
			BattleInterface.ShipIcon.iconoffset3 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(278* fHtRatio));
			BattleInterface.ShipIcon.iconoffset4 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(383* fHtRatio));
			BattleInterface.ShipIcon.iconoffset5 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(488* fHtRatio));
			BattleInterface.ShipIcon.iconoffset6 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(593* fHtRatio));
			BattleInterface.ShipIcon.iconoffset7 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(698* fHtRatio));
			BattleInterface.ShipIcon.iconoffset8 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(803* fHtRatio));
		}
		else
		{
			BattleInterface.ShipIcon.iconoffset1 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(68* fHtRatio));
			BattleInterface.ShipIcon.iconoffset2 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(178* fHtRatio));
			BattleInterface.ShipIcon.iconoffset3 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(288* fHtRatio));
			BattleInterface.ShipIcon.iconoffset4 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(398* fHtRatio));
			BattleInterface.ShipIcon.iconoffset5 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(508* fHtRatio));
			BattleInterface.ShipIcon.iconoffset6 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(618* fHtRatio));
			BattleInterface.ShipIcon.iconoffset7 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(728* fHtRatio));
			BattleInterface.ShipIcon.iconoffset8 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(838* fHtRatio));
		}

		BattleInterface.CommandList.CommandMaxIconQuantity = 12;
		BattleInterface.CommandList.CommandIconSpace = 1;
		BattleInterface.CommandList.CommandIconLeft = makeint(150 * fHtRatio);
		BattleInterface.CommandList.CommandIconWidth = RecalculateHIcon(makeint(64 * fHtRatio));
		BattleInterface.CommandList.CommandIconHeight = RecalculateVIcon(makeint(64* fHtRatio));

		BattleInterface.CommandList.CommandNoteFont = "interface_button";
		BattleInterface.CommandList.CommandNoteColor = argb(255,255,255,255);
		BattleInterface.CommandList.CommandNoteScale = 1.2 * fHtRatio;
		BattleInterface.CommandList.CommandNoteOffset = RecalculateHIcon(makeint(0* fHtRatio)) + "," + RecalculateVIcon(makeint(-68* fHtRatio));

		BattleInterface.CommandList.UDArrow_Texture = "battle_interface\arrowly.dds";
		BattleInterface.CommandList.UDArrow_UV_Up = "0.0,1.0,1.0,0.0";
		BattleInterface.CommandList.UDArrow_UV_Down = "0.0,0.0,1.0,1.0";
		BattleInterface.CommandList.UDArrow_Size = RecalculateHIcon(makeint(32* fHtRatio)) + "," + RecalculateVIcon(makeint(32* fHtRatio));
		BattleInterface.CommandList.UDArrow_Offset_Up = RecalculateHIcon(makeint(-36* fHtRatio)) + "," + RecalculateVIcon(makeint(-37* fHtRatio));
		BattleInterface.CommandList.UDArrow_Offset_Down = RecalculateHIcon(makeint(-36* fHtRatio)) + "," + RecalculateVIcon(makeint(60* fHtRatio));
	}

	else if (sti(InterfaceStates.InterfaceSelection)==POTC)
	{

		BattleInterface.ShipIcon.backtexturename		= "battle_interface\Ship_Border_PotC.dds"; // Основа
		BattleInterface.ShipIcon.backcolor				= argb(255,128,128,128);
		BattleInterface.ShipIcon.backuv					= "0.0,0.0,1.0,1.0";
		BattleInterface.ShipIcon.backoffset				= "0,0";
		BattleInterface.ShipIcon.backiconsize			= RecalculateHIcon(makeint(80* fHtRatio))+","+RecalculateVIcon(makeint(80* fHtRatio));

		if(bHorzBars)
		{

			BattleInterface.ShipIcon.sailorfontid			= "bold_numbers";
			BattleInterface.ShipIcon.sailorfontcolor		= argb(255,255,255,255); // Цвет количества матросов
			BattleInterface.ShipIcon.sailorfontscale		= 0.45 * fHtRatio;
			BattleInterface.ShipIcon.sailorfontoffset		= RecalculateHIcon(makeint(53* fHtRatio))+","+RecalculateVIcon(makeint(-49* fHtRatio));

			BattleInterface.ShipIcon.shipnamefontid			= "interface_normal";
			BattleInterface.ShipIcon.shipnamefontcolor		= argb(255,255,255,255); // Цвет имени корабля
			BattleInterface.ShipIcon.shipnamefontscale		= 1.0 * fHtRatio;
			BattleInterface.ShipIcon.shipnamefontoffset		= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(-69* fHtRatio));
		}
		else
		{

			BattleInterface.ShipIcon.sailorfontid			= "bold_numbers";
			BattleInterface.ShipIcon.sailorfontcolor		= argb(255,255,255,255); // Цвет количества матросов
			BattleInterface.ShipIcon.sailorfontscale		= 0.65 * fHtRatio;
			BattleInterface.ShipIcon.sailorfontoffset		= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(42* fHtRatio));

			BattleInterface.ShipIcon.shipnamefontid			= "interface_normal";
			BattleInterface.ShipIcon.shipnamefontcolor		= argb(255,255,255,255); // Цвет имени корабля
			BattleInterface.ShipIcon.shipnamefontscale		= 1.0 * fHtRatio;
			BattleInterface.ShipIcon.shipnamefontoffset		= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(-56* fHtRatio));
		}

		if(bHorzBars)
		{

			BattleInterface.ShipIcon.shipstatetexturename	= "battle_interface\indicators.dds";
			BattleInterface.ShipIcon.shipstatecolor			= argb(255,128,128,128);
			BattleInterface.ShipIcon.shipstatehorizontal	= 1;
			BattleInterface.ShipIcon.shipstatebackuv		= "0.0,0.5,1.0,0.666";    // LEO: ОДИН серый трек (низ текстуры = 3 одинаковых трека по 0.166, берём первый), иначе под бар лезут все три
			BattleInterface.ShipIcon.shipstatebackcolor		= argb(180,0,0,0);        // чёрный + альфа
			BattleInterface.ShipIcon.shiphpuv				= "0.0,0.333,1.0,0.5";   // корпус (верх)
			BattleInterface.ShipIcon.shiphpoffset			= RecalculateHIcon(0.0* fHtRatio)+","+RecalculateVIcon(-49* fHtRatio);
			BattleInterface.ShipIcon.shiphpiconsize			= RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(6* fHtRatio));
			BattleInterface.ShipIcon.shipspuv				= "0.0,0.167,1.0,0.333"; // паруса (середина)
			BattleInterface.ShipIcon.shipspoffset			= RecalculateHIcon(0.0* fHtRatio)+","+RecalculateVIcon(-44* fHtRatio);
			BattleInterface.ShipIcon.shipspiconsize			= RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(6* fHtRatio));
			BattleInterface.ShipIcon.shipcrewuv				= "0.0,0.0,1.0,0.166";   // команда (низ, у иконки)
			BattleInterface.ShipIcon.shipcrewoffset			= RecalculateHIcon(0.0* fHtRatio)+","+RecalculateVIcon(-39* fHtRatio);
			BattleInterface.ShipIcon.shipcrewiconsize		= RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(6* fHtRatio));
		}
		else
		{
			BattleInterface.ShipIcon.shipstatetexturename	= "battle_interface\ShipHP_SP.dds"; // Полоски HP/SP
			BattleInterface.ShipIcon.shipstatecolor			= argb(255,128,128,128);
			BattleInterface.ShipIcon.shipstatehorizontal	= 0;
			BattleInterface.ShipIcon.shiphpuv				= "0.0,0.0,0.5,1.0";
			BattleInterface.ShipIcon.shiphpoffset			= RecalculateHIcon(-43.0* fHtRatio)+","+RecalculateVIcon(0.0* fHtRatio);
			BattleInterface.ShipIcon.shiphpiconsize			= RecalculateHIcon(makeint(6* fHtRatio))+","+RecalculateVIcon(makeint(80* fHtRatio));
			BattleInterface.ShipIcon.shipspuv				= "0.5,0.0,1.0,1.0";
			BattleInterface.ShipIcon.shipspoffset			= RecalculateHIcon(43.0* fHtRatio)+","+RecalculateVIcon(0.0* fHtRatio);
			BattleInterface.ShipIcon.shipspiconsize			= RecalculateHIcon(makeint(6* fHtRatio))+","+RecalculateVIcon(makeint(80* fHtRatio));
			BattleInterface.ShipIcon.shipcrewiconsize		= "0,0"; // команда-бар скрыт в вертик. режиме
		}

		BattleInterface.ShipIcon.shipclassiconsize		= "0,0";
		BattleInterface.ShipIcon.gunchargeprogress		= "0.0625, 0.219, 0.359, 0.5, 0.641, 0.781, 0.983";

		BattleInterface.ShipIcon.shiptexturename		= "battle_interface\ship_icons2_PotC.dds"; // Иконка корабля
		BattleInterface.ShipIcon.shipcolor				= argb(255,128,128,128);
		BattleInterface.ShipIcon.shipoffset				= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(0* fHtRatio));
		BattleInterface.ShipIcon.shipiconsize			= RecalculateHIcon(makeint(80* fHtRatio))+","+RecalculateVIcon(makeint(80* fHtRatio));
		BattleInterface.ShipIcon.xsize = 32;
		BattleInterface.ShipIcon.ysize = 32;

		BattleInterface.ShipIcon.commandlistverticaloffset =  makeint(-33 * fHtRatio);

		if(bHorzBars)
		{

			BattleInterface.ShipIcon.shipiconsize = RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(74* fHtRatio));
			BattleInterface.ShipIcon.backiconsize = RecalculateHIcon(makeint(74* fHtRatio))+","+RecalculateVIcon(makeint(74* fHtRatio));
			BattleInterface.ShipIcon.iconoffset1 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(68* fHtRatio));
			BattleInterface.ShipIcon.iconoffset2 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(173* fHtRatio));
			BattleInterface.ShipIcon.iconoffset3 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(278* fHtRatio));
			BattleInterface.ShipIcon.iconoffset4 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(383* fHtRatio));
			BattleInterface.ShipIcon.iconoffset5 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(488* fHtRatio));
			BattleInterface.ShipIcon.iconoffset6 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(593* fHtRatio));
			BattleInterface.ShipIcon.iconoffset7 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(698* fHtRatio));
			BattleInterface.ShipIcon.iconoffset8 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(803* fHtRatio));
		}
		else
		{
			BattleInterface.ShipIcon.iconoffset1 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(68* fHtRatio));
			BattleInterface.ShipIcon.iconoffset2 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(178* fHtRatio));
			BattleInterface.ShipIcon.iconoffset3 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(288* fHtRatio));
			BattleInterface.ShipIcon.iconoffset4 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(398* fHtRatio));
			BattleInterface.ShipIcon.iconoffset5 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(508* fHtRatio));
			BattleInterface.ShipIcon.iconoffset6 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(618* fHtRatio));
			BattleInterface.ShipIcon.iconoffset7 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(728* fHtRatio));
			BattleInterface.ShipIcon.iconoffset8 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(838* fHtRatio));
		}

		BattleInterface.CommandList.CommandMaxIconQuantity = 12;
		BattleInterface.CommandList.CommandIconSpace = 1;
		BattleInterface.CommandList.CommandIconLeft = makeint(150 * fHtRatio);
		BattleInterface.CommandList.CommandIconWidth = RecalculateHIcon(makeint(64 * fHtRatio));
		BattleInterface.CommandList.CommandIconHeight = RecalculateVIcon(makeint(64* fHtRatio));

		BattleInterface.CommandList.CommandNoteFont = "interface_button";
		BattleInterface.CommandList.CommandNoteColor = argb(255,255,255,255);
		BattleInterface.CommandList.CommandNoteScale = 1.2 * fHtRatio;
		BattleInterface.CommandList.CommandNoteOffset = RecalculateHIcon(makeint(0* fHtRatio)) + "," + RecalculateVIcon(makeint(-68* fHtRatio));

		BattleInterface.CommandList.UDArrow_Texture = "battle_interface\arrowly.dds";
		BattleInterface.CommandList.UDArrow_UV_Up = "0.0,1.0,1.0,0.0";
		BattleInterface.CommandList.UDArrow_UV_Down = "0.0,0.0,1.0,1.0";
		BattleInterface.CommandList.UDArrow_Size = RecalculateHIcon(makeint(32* fHtRatio)) + "," + RecalculateVIcon(makeint(32* fHtRatio));
		BattleInterface.CommandList.UDArrow_Offset_Up = RecalculateHIcon(makeint(-36* fHtRatio)) + "," + RecalculateVIcon(makeint(-37* fHtRatio));
		BattleInterface.CommandList.UDArrow_Offset_Down = RecalculateHIcon(makeint(-36* fHtRatio)) + "," + RecalculateVIcon(makeint(60* fHtRatio));
	}
	else if (sti(InterfaceStates.InterfaceSelection)==KRUGLYI)
	{

		BattleInterface.ShipIcon.backtexturename		= "battle_interface\ShipBackIcon.dds"; // Основа
		BattleInterface.ShipIcon.backcolor				= argb(255,128,128,128);
		BattleInterface.ShipIcon.backuv					= "0.0,0.0,1.0,1.0";
		BattleInterface.ShipIcon.backoffset				= "-1,0";
		BattleInterface.ShipIcon.backiconsize			= RecalculateHIcon(makeint(110* fHtRatio))+","+RecalculateVIcon(makeint(110* fHtRatio));

		BattleInterface.ShipIcon.sailorfontid			= "bold_numbers";
		BattleInterface.ShipIcon.sailorfontcolor		= argb(255,255,255,255); // Цвет количества матросов
		BattleInterface.ShipIcon.sailorfontscale		= 0.6 * fHtRatio;
		BattleInterface.ShipIcon.sailorfontoffset		= RecalculateHIcon(makeint(-2* fHtRatio))+","+RecalculateVIcon(makeint(31* fHtRatio));

		BattleInterface.ShipIcon.shipnamefontid			= "interface_button";
		BattleInterface.ShipIcon.shipnamefontcolor		= argb(255,255,255,255); // Цвет имени корабля
		BattleInterface.ShipIcon.shipnamefontscale		= 1.15 * fHtRatio;
		BattleInterface.ShipIcon.shipnamefontoffset		= RecalculateHIcon(makeint(0* fHtRatio))+","+RecalculateVIcon(makeint(-66* fHtRatio));
		BattleInterface.ShipIcon.shipnamefont.align 	= "left";

		BattleInterface.ShipIcon.shipstatetexturename	= "battle_interface\ShipState.dds"; // Полоски HP/SP
		BattleInterface.ShipIcon.shipstatecolor			= argb(255,128,128,128);
		BattleInterface.ShipIcon.shiphpuv				= "0.0,0.153,0.68,0.815";
		BattleInterface.ShipIcon.shiphpoffset			= RecalculateHIcon(-17* fHtRatio)+","+RecalculateVIcon(-2* fHtRatio);
		BattleInterface.ShipIcon.shiphpiconsize			= RecalculateHIcon(makeint(78* fHtRatio))+","+RecalculateVIcon(makeint(75* fHtRatio));

		BattleInterface.ShipIcon.shipspuv				= "0.32,0.149,1.0,0.816";
		BattleInterface.ShipIcon.shipspoffset			= RecalculateHIcon(16* fHtRatio)+","+RecalculateVIcon(-2* fHtRatio);
		BattleInterface.ShipIcon.shipspiconsize			= RecalculateHIcon(makeint(78* fHtRatio))+","+RecalculateVIcon(makeint(75* fHtRatio));

		BattleInterface.ShipIcon.shipstatehorizontal	= 0;
		BattleInterface.ShipIcon.shipcrewiconsize		= "0,0";

		BattleInterface.ShipIcon.shipclassiconsize		= "0,0";
		BattleInterface.ShipIcon.gunchargeprogress		= "0.0625, 0.219, 0.359, 0.5, 0.641, 0.781, 0.983";

		BattleInterface.ShipIcon.shiptexturename		= "battle_interface\ship_icons2_johnny88hud.dds"; // Иконка корабля
		BattleInterface.ShipIcon.shipcolor				= argb(255,128,128,128);
		BattleInterface.ShipIcon.shipoffset				= RecalculateHIcon(makeint(-1* fHtRatio))+","+RecalculateVIcon(makeint(-5* fHtRatio));
		BattleInterface.ShipIcon.shipiconsize			= RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(70* fHtRatio));
		BattleInterface.ShipIcon.xsize = 32;
		BattleInterface.ShipIcon.ysize = 32;

		BattleInterface.ShipIcon.commandlistverticaloffset = makeint(-32 * fHtRatio);

		BattleInterface.ShipIcon.iconoffset1 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(66* fHtRatio));
		BattleInterface.ShipIcon.iconoffset2 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(178* fHtRatio));
		BattleInterface.ShipIcon.iconoffset3 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(290* fHtRatio));
		BattleInterface.ShipIcon.iconoffset4 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(402* fHtRatio));
		BattleInterface.ShipIcon.iconoffset5 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(514* fHtRatio));
		BattleInterface.ShipIcon.iconoffset6 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(626* fHtRatio));
		BattleInterface.ShipIcon.iconoffset7 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(738* fHtRatio));
		BattleInterface.ShipIcon.iconoffset8 = RecalculateHIcon(makeint(70* fHtRatio))+","+RecalculateVIcon(makeint(850* fHtRatio));

		BattleInterface.CommandList.CommandMaxIconQuantity = 12;
		BattleInterface.CommandList.CommandIconSpace = 1;
		BattleInterface.CommandList.CommandIconLeft = makeint(150 * fHtRatio);
		BattleInterface.CommandList.CommandIconWidth = RecalculateHIcon(makeint(64 * fHtRatio));
		BattleInterface.CommandList.CommandIconHeight = RecalculateVIcon(makeint(64 * fHtRatio));

		BattleInterface.CommandList.CommandNoteFont = "interface_button";
		BattleInterface.CommandList.CommandNoteColor = argb(255,255,255,255);
		BattleInterface.CommandList.CommandNoteScale = 1.2 * fHtRatio;
		BattleInterface.CommandList.CommandNoteOffset = RecalculateHIcon(makeint(0* fHtRatio)) + "," + RecalculateVIcon(makeint(-68* fHtRatio));

		BattleInterface.CommandList.UDArrow_Texture = "battle_interface\arrowly.dds";
		BattleInterface.CommandList.UDArrow_UV_Up = "0.0,1.0,1.0,0.0";
		BattleInterface.CommandList.UDArrow_UV_Down = "0.0,0.0,1.0,1.0";
		BattleInterface.CommandList.UDArrow_Size = RecalculateHIcon(makeint(32 * fHtRatio)) + "," + RecalculateVIcon(makeint(32 * fHtRatio));
		BattleInterface.CommandList.UDArrow_Offset_Up = RecalculateHIcon(makeint(-36 * fHtRatio)) + "," + RecalculateVIcon(makeint(-37 * fHtRatio));
		BattleInterface.CommandList.UDArrow_Offset_Down = RecalculateHIcon(makeint(-36 * fHtRatio)) + "," + RecalculateVIcon(makeint(66 * fHtRatio));
	}

	aref thinShipIcon;
	float thinShipDiameter = 80.0 * fHtRatio;
	if(sti(InterfaceStates.InterfaceSelection) == KRUGLYI) thinShipDiameter = 70.0 * fHtRatio;
	if(sti(InterfaceStates.InterfaceSelection) == KVADRAT)
	{
		if(bHorzBars) thinShipDiameter = 74.0 * fHtRatio;
	}
	makearef(thinShipIcon, BattleInterface.ShipIcon);
	thinShipDiameter=NewHUD_SetSeaLayout(thinShipIcon,sti(InterfaceStates.InterfaceSelection),bHorzBars,COMPANION_MAX,fHtRatio,stf(Render.screen_y),thinShipDiameter);
	NewHUD_Setup(thinShipIcon, sti(InterfaceStates.InterfaceSelection), GetMainCharacterIndex(), true, bHorzBars, thinShipDiameter, fHtRatio);
	if(sti(InterfaceStates.InterfaceSelection)==KRUGLYI && NewHUD_GetOption("NewHUDEnabled")!=0)
		BattleInterface.ShipIcon.shipnamefontid="interface_normal";

	HUDLayout_ApplySigns(thinShipIcon,"sea",COMPANION_MAX,true);
	aref layoutCommands;
	makearef(layoutCommands,BattleInterface.CommandList);
	HUDLayout_SetCommands(layoutCommands,"sea",GetMainCharacterIndex());
	aref layoutOwner;
	makearef(layoutOwner,BattleInterface);
	HUDLayout_ApplyNavigator(layoutOwner);
	NewSeaHUD_SetParameters(layoutOwner,sti(Render.screen_x),sti(Render.screen_y),fHtRatio);
	NewSeaHUD_SetAimReadout(layoutOwner,sti(Render.screen_x),sti(Render.screen_y),fHtRatio);

	PerkTimers_CreateSlots(&BattleInterface, true);
	if(NewHUD_GetOption("NewHUDEnabled")!=0) BI_UpdateAmmoHighlight();
	BattleInterface.ShipInfoImages.RelationTexture = "battle_interface\ship_arrows1.dds";
	BattleInterface.ShipInfoImages.RelationOffset.x = 0.0;
	BattleInterface.ShipInfoImages.RelationOffset.y = 0.3;
	BattleInterface.ShipInfoImages.RelationOffset.z = 0.0;
	BattleInterface.ShipInfoImages.RelationSize = "0.75,0.5";

	BattleInterface.ShipInfoImages.RelationUV1 = "0.0,0.0,0.125,1.0";   // friend (зелёный)
	BattleInterface.ShipInfoImages.RelationUV2 = "0.125,0.0,0.25,1.0";  // enemy (красный)
	BattleInterface.ShipInfoImages.RelationUV3 = "0.25,0.0,0.375,1.0";  // neutral (бежевый)
	BattleInterface.ShipInfoImages.RelationUV4 = "0.375,0.0,0.5,1.0";   // компаньон (синий)
	BattleInterface.ShipInfoImages.RelationUV5 = "0.5,0.0,0.625,1.0";   // ГГ (чёрный)

	BattleInterface.ShipInfoImages.StarTexture = "quest_signs\QM_MiniBoss.tga"; // движок допишет .tx -> QM_MiniBoss.tga.tx
	BattleInterface.ShipInfoImages.StarOffset.x = 0.0;
	BattleInterface.ShipInfoImages.StarOffset.y = 0.95; // чуть выше корпуса (спущена ближе к барам)
	BattleInterface.ShipInfoImages.StarOffset.z = 0.0;
	BattleInterface.ShipInfoImages.StarSize = "0.5,0.25"; // вдвое меньше (~квадрат на экране); тюнить тут
	BattleInterface.ShipInfoImages.StarUV = "0.0,0.0,1.0,1.0"; // вся текстура

	SetShipBarsParameters(); // LEO: настройка баров над кораблями (вынесена для смены текстуры на лету)

	BattleInterface.ShifInfoVisible = InterfaceStates.enabledshipmarks;
	LanguageCloseFile(idLngFile);
}

// LEO: настройка баров над кораблями (текстура/UV/размеры/ghost). Вынесена из SetParameterData,
// чтобы переключать новые CSP <-> старые HD на лету (ReloadShipBars шлёт движку BI_MSG_RELOAD_SHIPBARS).
void SetShipBarsParameters()
{
	ShipInfoBars_Init(); // LEO: Однократная миграция и состояние при перезагрузке текстуры полос.
	BattleInterface.ShipInfoImages.ProgressTexture = "battle_interface\indicators_CSP.dds"; // LEO: текстура баров над кораблями (512x256): красный y4-37 / синий y47-80 / белый y90-122 / 3 рамки-подложки y128-256
	BattleInterface.ShipInfoImages.ProgressSize = "1.994,0.09"; // ОБЩАЯ ширина баров (1.994 = вписаны в рамки, расширены ещё +1px с краёв); .y = деф высота (корпус)

	BattleInterface.ShipInfoImages.ProgressBackOffset.x = 0.0;
	BattleInterface.ShipInfoImages.ProgressBackOffset.y = 0.65;
	BattleInterface.ShipInfoImages.ProgressBackOffset.z = 0.0;
	BattleInterface.ShipInfoImages.ProgressBackSize = "2.0,0.3";
	BattleInterface.ShipInfoImages.ProgressBackUV = "0.0,0.5,1.0,1.0"; // LEO: 3 рамки-подложки (y128..256)

	BattleInterface.ShipInfoImages.HullOffset.x = 0.0;
	BattleInterface.ShipInfoImages.HullOffset.y = 0.75;
	BattleInterface.ShipInfoImages.HullOffset.z = 0.0;
	BattleInterface.ShipInfoImages.HullBarH = 0.09;                      // высота красного бара
	BattleInterface.ShipInfoImages.HullUV = "0.0,0.016,1.0,0.145"; // LEO: красная заливка y4..37

	BattleInterface.ShipInfoImages.SailOffset.x = 0.0;
	BattleInterface.ShipInfoImages.SailOffset.y = 0.6502;
	BattleInterface.ShipInfoImages.SailOffset.z = 0.0;
	BattleInterface.ShipInfoImages.SailBarH = 0.0923;                    // высота синего бара (+1px сверху)
	BattleInterface.ShipInfoImages.SailUV = "0.0,0.184,1.0,0.313"; // LEO: синяя заливка y47..80

	BattleInterface.ShipInfoImages.CrewOffset.x = 0.0;
	BattleInterface.ShipInfoImages.CrewOffset.y = 0.5514;
	BattleInterface.ShipInfoImages.CrewOffset.z = 0.0;
	BattleInterface.ShipInfoImages.CrewBarH = 0.0853;                    // высота белого бара (−2px снизу)
	BattleInterface.ShipInfoImages.CrewUV = "0.0,0.352,1.0,0.477"; // LEO: белая заливка y90..122

	if (CheckAttribute(&InterfaceStates, "OldShipBars"))
	{
		if (sti(InterfaceStates.OldShipBars) == 1)
		{
			BattleInterface.ShipInfoImages.ProgressTexture = "battle_interface\indicators_HD.dds";
			BattleInterface.ShipInfoImages.ProgressSize = "2.0,0.1"; // плотная заливка на всю ширину (без инсета рамок)
			BattleInterface.ShipInfoImages.ProgressBackUV = "0.0,0.5,1.0,1.0";
			BattleInterface.ShipInfoImages.HullUV = "0.0,0.333,1.0,0.5";
			BattleInterface.ShipInfoImages.SailUV = "0.0,0.167,1.0,0.333";
			BattleInterface.ShipInfoImages.CrewUV = "0.0,0.0,1.0,0.166";
		}
	}

	BattleInterface.ShipInfoImages.GhostOn = 1;
	aref seaGhostParams;
	makearef(seaGhostParams,BattleInterface.ShipInfoImages);
	NewHUD_SetGhostParameters(seaGhostParams,true);
	BattleInterface.ShipInfoImages.GhostHeight = 1.0;                     // базовая высота хвоста = во весь бар; per-bar края подрезаются ниже

	BattleInterface.ShipInfoImages.GhostTopHull = 0.0023; // красный: верх −1px
	BattleInterface.ShipInfoImages.GhostBotHull = 0.0023; // красный: низ −1px
	BattleInterface.ShipInfoImages.GhostTopSail = 0.0023; // синий: верх −1px
	BattleInterface.ShipInfoImages.GhostBotSail = 0.0023; // синий: низ −1px
	BattleInterface.ShipInfoImages.GhostTopCrew = 0.0023; // белый: верх −1px
	BattleInterface.ShipInfoImages.GhostBotCrew = 0.0;    // белый: низ без изменений
	BattleInterface.ShipInfoImages.FortYOffset = 30.0;         // LEO: подъём бара над фортом по миру (над башнями; позиция = центр пушек)
	BattleInterface.ShipInfoImages.FortWidth = 4.0;            // ширина баров форта (шире корабельных ~1.994)
	BattleInterface.ShipInfoImages.FortBarH = 0.09;           // высота одного бара форта
	BattleInterface.ShipInfoImages.FortBackH = 0.12;          // высота подложки (1 слот под ХП)
	BattleInterface.ShipInfoImages.FortHullOffsetY = 0.60;    // Y ХП-бара (корпус, красный)
	BattleInterface.ShipInfoImages.FortGarrisonOffsetY = 0.61;// Y бара гарнизона (белый), вплотную под ХП
	BattleInterface.ShipInfoImages.FortBackOffsetY = 0.60;    // Y подложки (по ХП-бару)
	BattleInterface.ShipInfoImages.FortBackUV = "0.0,0.5,1.0,0.667"; // 1 слот (только ХП)
	BattleInterface.ShipInfoImages.FortShowGarrison = 0;      // гарнизон выкл (только ХП); 1 = вернуть
	BattleInterface.ShipInfoImages.FortShowDist = 1200.0;    // дальность показа бара форта (ед.) // LEO: подъём Y бара над фортом (тунабл; у форта pos.y=0)
}

// LEO: переключить бары над кораблями на лету — обновить атрибуты и пересоздать сущность в движке
void ReloadShipBars()
{
	if (!bSeaActive) return; // вне моря боевого интерфейса нет — применится при входе
	SetShipBarsParameters();
	SendMessage(&BattleInterface, "l", BI_MSG_RELOAD_SHIPBARS);
}

ref ProcessSailDamage()
{

	int shootIdx = GetEventData();

	int chrIdx = GetEventData();

	string sMastName = GetEventData();

	string reyName = GetEventData();
	int groupNum = GetEventData();

	int holeCount = GetEventData();
	int holeData = GetEventData();
	int maxHoleCount = GetEventData();

	float sailPower = GetEventData();

	ref chref = GetCharacter(chrIdx);

	string groupName = ""+groupNum;
	aref arSail;
	makearef(arSail,chref.ship.sails.(reyName).(groupName));

	if (LAi_IsImmortal(chref))
	{
		arSail.hd = DeleteOneSailHole(sti(chref.index), groupName, reyName, holeData, 1);//имитирует защиту залатыванием дырки в месте попадания
		BI_g_fRetVal = 0;
		return &BI_g_fRetVal;
	}

	float sailDmg = 0.0;
	float sailDmgMax = GetCharacterShipSP(chref) * sailPower;
	if(!CheckAttribute(arSail, "oldhd"))
	{
		arSail.oldhd = 0;
	}
	if(!CheckAttribute(arSail, "olddmg"))
	{
		arSail.olddmg = 0;
	}
	if(!CheckAttribute(arSail, "oldhc"))
	{
		arSail.oldhc = 0;
	}
	if( !CheckAttribute(arSail,"dmg") )	{ sailDmg = 0.0; }

	if(sMastName=="*")
	{
		if(sailDmg>sailDmgMax)	{ sailDmg = sailDmgMax; }
		int needHole = GetNeedHoleFromDmg(sailDmg,sailDmgMax,maxHoleCount);
		if(holeCount!=needHole)
		{
			if(holeCount<needHole)
			{
				holeData = RandomHole2Sail(chrIdx,reyName,groupNum, maxHoleCount, holeData, needHole-holeCount);
				holeCount = needHole;
			}
			else
			{
				sailDmg = GetNeedDmgFromHole(holeCount,sailDmgMax,maxHoleCount);
			}
		}
		sailDmg = GetNeedDmgFromHole(holeCount,sailDmgMax,maxHoleCount);
	}
	else
	{
		if(sMastName!="#")	{ arSail.mastFall = sMastName; }
		sailDmg = sailDmgMax;
	}

	bool bOk2 = S_armor(chref) && rand(2)<2 && holedata != sti(arSail.oldhd); // СФ
	if(bOk2)
	{

		if(sailDmg - stf(arSail.olddmg) >=1)
		{
			arSail.hd = DeleteOneSailHole(sti(chref.index), groupName, reyName, holeData, 1);
			arSail.oldhd = sti(arSail.hd);
			if(!CheckAttribute(arSail, "defended"))
			{
				arSail.defended = 1;
			}
			arSail.defended = sti(arSail.defended)+1;
			arSail.hc = sti(arSail.oldhc);
			arSail.oldhc = sti(arSail.hc);
			arSail.dmg = stf(arSail.olddmg);
			arSail.olddmg = stf(arSail.dmg);
			chref.ship.SP = CalculateShipSP(chref);
			BI_g_fRetVal = 0.0;
			return &BI_g_fRetVal;
		}
	}
	else
	{
		arSail.dmg = sailDmg;
		arSail.hd = holeData;
		arSail.hc = holeCount;
		arSail.oldhd = holeData;
	}

	arSail.mhc = maxHoleCount;
	arSail.sp = sailPower;

	chref.ship.SP = CalculateShipSP(chref);
	BI_g_fRetVal = sailDmg;
	return &BI_g_fRetVal;
}

void ProcessDayRepair()
{
	int i, cn;
	float matQ, tmpf, repPercent;
	ref chref;
	ref mchar = GetMainCharacter();

	if (bSeaActive == false || mchar.location == mchar.SystemInfo.CabinType) // спим в каюте
	{
		for (i=0; i<COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(mchar,i);
			if(cn==-1) continue;
			chref = GetCharacter(cn);

			if (GetHullPercent(chref)<100.0 )
			{
				repPercent = GetHullRPD(chref);
				matQ = repPercent*GetHullPPP(chref);
				tmpf = GetRepairGoods(true,chref);
				if (tmpf > 0)
				{
					if (tmpf < matQ)	{ repPercent = tmpf/GetHullPPP(chref); }
					repPercent = ProcessHullRepair(chref, repPercent);
					matQ = repPercent*GetHullPPP(chref);
					RemoveRepairGoods(true,chref,matQ);

		            AddCharacterExpToSkill(chref, "Repair", matQ / 3.0);

	            }
			}

			if (GetSailPercent(chref)<100.0 )
			{
				repPercent = GetSailRPD(chref);
				matQ = repPercent*GetSailSPP(chref);
				tmpf = GetRepairGoods(false,chref);
				if (tmpf > 0)
				{
					if (tmpf < matQ)	{ repPercent = tmpf/GetSailSPP(chref); }
					repPercent = ProcessSailRepair(chref,repPercent);
					matQ = repPercent*GetSailSPP(chref);
					RemoveRepairGoods(false,chref,matQ);

		            AddCharacterExpToSkill(chref, "Repair", matQ / 4.0);

	            }
			}
		}
	}

}

ref procGetSailTextureData()
{
	int st,i,sq;
	ref shref;
	aref arEmbl,arSail;
	string attrName;
	int iUpgrade;
	string sUpgrade;
	string nationFileName;
	string tmpStr;

	int chrIdx = GetEventData();

	DeleteAttribute(&BI_objRetValue,"");
	if (chrIdx>=0)
	{
		st = GetCharacterShipType(GetCharacter(chrIdx));
		if (st != SHIP_NOTUSED)
		{
			sUpgrade = "common";

			shref = GetRealShip(st);

			iUpgrade = sti(shref.ship.upgrades.sails);
			switch(iUpgrade)
			{
				case 1:	sUpgrade = "common";	break; // LEO: --> Все корабли используют
				case 2: sUpgrade = "pat";		break;
				case 3:	sUpgrade = "silk";		break;
				case 4: sUpgrade = "silkblack";	break; // Sea Boss
				case 5:	sUpgrade = "usual_1";	break;
				case 6:	sUpgrade = "usual_2";	break;
				case 7:	sUpgrade = "usual_3";	break;
				case 8:	sUpgrade = "usual_4";	break;
				case 9:	sUpgrade = "usual_5";	break;
				case 10:sUpgrade = "usual_6";	break;
				case 11:sUpgrade = "usual_7";	break;
				case 12:sUpgrade = "usual_8";	break;
				case 13:sUpgrade = "usual_9";	break;
				case 14:sUpgrade = "usual_10";	break;
				case 15:sUpgrade = "usual_11";	break;
				case 16:sUpgrade = "usual_12";	break;
				case 17:sUpgrade = "usual_13";	break;
				case 18:sUpgrade = "usual_14";	break;
				case 19:sUpgrade = "usual_15";	break;
				case 20:sUpgrade = "usual_16";	break;
				case 21:sUpgrade = "usual_17";	break;
				case 22:sUpgrade = "usual_18";	break;
				case 23:sUpgrade = "usual_19";	break;
				case 24:sUpgrade = "usual_20";	break;
				case 25:sUpgrade = "usual_21";	break;
				case 26:sUpgrade = "usual_22";	break;
				case 27:sUpgrade = "usual_23";	break;
				case 28:sUpgrade = "usual_24";	break;
				case 29:sUpgrade = "usual_25";	break;
				case 30:sUpgrade = "usual_26";	break;
				case 31:sUpgrade = "usual_27";	break;
				case 32:sUpgrade = "usual_28";	break;
				case 33:sUpgrade = "usual_29";	break;
				case 34:sUpgrade = "usual_30";	break;
				case 35:sUpgrade = "usual_31";	break;
				case 36:sUpgrade = "usual_32";	break;
				case 37:sUpgrade = "usual_33";	break; // LEO: <-- Все корабли используют

				case 38:sUpgrade = "sail_torn_black_pirate_1";		break; // LEO: --> Только пираты
				case 39:sUpgrade = "sail_torn_black_pirate_2";		break;
				case 40:sUpgrade = "sail_torn_black_pirate_3";		break;
				case 41:sUpgrade = "sail_torn_black_pirate_4";		break;
				case 42:sUpgrade = "sail_torn_black_pirate_5";		break;
				case 43:sUpgrade = "sail_torn_black_pirate_6";		break;
				case 44:sUpgrade = "sail_torn_black_pirate_7";		break;
				case 45:sUpgrade = "sail_torn_black_pirate_8";		break;
				case 46:sUpgrade = "sail_torn_black_pirate_9";		break;
				case 47:sUpgrade = "sail_torn_black_pirate_10";		break;
				case 48:sUpgrade = "sail_torn_black_pirate_11";		break;
				case 49:sUpgrade = "sail_torn_black_pirate_12";		break;
				case 50:sUpgrade = "sail_torn_black_pirate_13";		break;
				case 51:sUpgrade = "sail_torn_black_pirate_14";		break;
				case 52:sUpgrade = "sail_whole_black_pirate_1";		break;
				case 53:sUpgrade = "sail_whole_black_pirate_2";		break;
				case 54:sUpgrade = "sail_whole_black_pirate_3";		break;
				case 55:sUpgrade = "sail_whole_black_pirate_4";		break;
				case 56:sUpgrade = "sail_whole_black_pirate_5";		break;
				case 57:sUpgrade = "sail_whole_black_pirate_6";		break;
				case 58:sUpgrade = "sail_whole_black_pirate_7";		break;
				case 59:sUpgrade = "sail_whole_black_pirate_8";		break;
				case 60:sUpgrade = "sail_whole_black_pirate_9";		break;
				case 61:sUpgrade = "sail_whole_black_pirate_10";	break;
				case 62:sUpgrade = "sail_whole_black_pirate_11";	break;
				case 63:sUpgrade = "sail_whole_black_pirate_12";	break;
				case 64:sUpgrade = "sail_whole_black_pirate_13";	break;
				case 65:sUpgrade = "sail_whole_black_pirate_14";	break;
				case 66:sUpgrade = "sail_whole_black_pirate_15";	break;
				case 67:sUpgrade = "sail_whole_black_pirate_16";	break;
				case 68:sUpgrade = "sail_whole_black_pirate_17";	break;
				case 69:sUpgrade = "sail_whole_white_pirate_1";		break;
				case 70:sUpgrade = "sail_whole_white_pirate_2";		break;
				case 71:sUpgrade = "sail_whole_white_pirate_3";		break;
				case 72:sUpgrade = "sail_whole_white_pirate_4";		break;
				case 73:sUpgrade = "sail_whole_white_pirate_5";		break;
				case 74:sUpgrade = "sail_whole_white_pirate_6";		break;
				case 75:sUpgrade = "sail_whole_white_pirate_7";		break;
				case 76:sUpgrade = "sail_whole_white_pirate_8";		break;
				case 77:sUpgrade = "sail_whole_white_pirate_9";		break;
				case 78:sUpgrade = "sail_whole_white_pirate_10";	break;
				case 79:sUpgrade = "sail_whole_white_pirate_11";	break;
				case 80:sUpgrade = "sail_whole_white_pirate_12";	break;
				case 81:sUpgrade = "sail_whole_white_pirate_13";	break;
				case 82:sUpgrade = "sail_whole_white_pirate_14";	break;
				case 83:sUpgrade = "sail_whole_white_pirate_15";	break;
				case 84:sUpgrade = "sail_whole_white_pirate_16";	break;
				case 85:sUpgrade = "sail_whole_white_pirate_17";	break;
				case 86:sUpgrade = "sail_whole_white_pirate_18";	break;
				case 87:sUpgrade = "sail_whole_white_pirate_19";	break;
				case 88:sUpgrade = "sail_whole_white_pirate_20";	break; // LEO: <-- Только пираты
				case 89:sUpgrade = "common_torn";					break;
				case 90:sUpgrade = "common_torn1";					break;
				case 91:sUpgrade = "common_torn2";					break;
			}
			if (iUpgrade > 37 && !bNewSails) {
				sUpgrade = "common";
			}
			nationFileName = "ships\parus_" + sUpgrade + ".dds";

			BI_objRetValue.normalTex = nationFileName;
			BI_objRetValue.geraldTex = "";
			BI_objRetValue.sailscolor = argb(255,255,255,255);  // белый парус

			if (CheckAttribute(shref, "ShipSails.SailsColor")) // 1.2.3 цвет теперь атрибут корабля, а не НПС
			{
				BI_objRetValue.sailscolor = sti(shref.ShipSails.SailsColor);
			}
			if( CheckAttribute(shref,"ShipSails.gerald_name") ) // 1.2.3 герб теперь атрибут корабля, а не НПС
			{
				BI_objRetValue.geraldTex = "Ships\Gerald\" + shref.ShipSails.gerald_name + ".tga";
			}

			BI_objRetValue.maxSP = shref.sp;

			if (CheckAttribute(shref, "EmblemedSails.normalTex")) // заднанный в типе парус
			{
				nationFileName = shref.EmblemedSails.normalTex;
				// LEO: В старых квестовых сейвах указан отсутствующий TGA; подменяем путь без изменения сохранённых парусов.
				if (nationFileName == "ships\parus_common_torn.tga") nationFileName = "ships\parus_common_torn.dds";
				BI_objRetValue.normalTex = nationFileName;
			}
			if( CheckAttribute(&Characters[chrIdx],"Features.GeraldSails") && sti(Characters[chrIdx].Features.GeraldSails)==true)
			{
				makearef(arEmbl,shref.GeraldSails);
			} else {
				makearef(arEmbl,shref.EmblemedSails);
			}
			sq = GetAttributesNum(arEmbl);
			for(i=0; i<sq; i++)
			{
				arSail = GetAttributeN(arEmbl,i);
				attrName = GetAttributeName(arSail);
				tmpStr = GetAttributeValue(arSail);
				if( CheckAttribute(arSail,"hscale") ) {
					BI_objRetValue.(attrName).hscale = arSail.hscale;
				}
				if( CheckAttribute(arSail,"vscale") ) {
					BI_objRetValue.(attrName).vscale = arSail.vscale;
				}
				if(tmpStr=="1") {
					BI_objRetValue.(attrName) = nationFileName;
				} else {
					BI_objRetValue.(attrName).Gerald = tmpStr;
				}
			}
		}
	}
	return &BI_objRetValue;
}

// Gregg: B — колонка нации в роль-атласах sflag/флаге (10 кол x 1 ряд), порядок rigging.ini *FlagTex
int GetNationFlagCol(int nat)
{
	int c = 0;
	switch(nat)
	{
	case HOLLAND: c = 0; break;  // DatchFlagTex
	case ENGLAND: c = 1; break;  // EnglishFlagTex
	case FRANCE:  c = 2; break;  // FrenchFlagTex
	case SPAIN:   c = 4; break;  // SpanishFlagTex
	case PIRATE:  c = 5; break;  // PirateFlagTex
	}
	return c;
}

// Gregg: B — роль корабля по char для выбора атласа sflag.
//   1=ГГ/эскадра 2=пират 3=квест 4=торговец 5=военный. Приоритет сверху вниз. БЕЗ смешанной логики &&/|| в одном if.
int GetShipRole(aref chr)
{
	if(!CheckAttribute(chr, "index")) return 5;
	if(IsMainCharacter(chr)) return 1;
	if(IsCompanion(chr)) return 1;
	if(CheckAttribute(chr, "nation"))
	{
		if(sti(chr.nation) == PIRATE) return 2;
	}
	if(!CheckAttribute(chr, "EncType"))
	{
		if(sti(chr.index) < FANTOM_CHARACTERS) return 3;
	}
	if(CheckAttribute(chr, "EncType"))
	{
		if(chr.EncType == "trade") return 4;
	}
	if(CheckAttribute(chr, "Ship.Mode"))
	{
		if(chr.Ship.Mode == "trade") return 4;
	}
	return 5;
}

ref procGetRiggingData()
{
	int n, tmp;
	int row = 0;
	int col = 0;

	string section = GetEventData(); // iniSection: FLAGS / FRTFLG / SFLAG_* / PENNANT
	tmp = GetEventData();            // triangle
	n   = GetEventData();            // nation
	tmp = GetEventData();            // isSpecialFlag

	if(section == "PENNANT")
	{

		switch(n)
		{
		case HOLLAND: row = 0; break;
		case ENGLAND: row = 1; break;
		case FRANCE:  row = 2; break;
		case SPAIN:   row = 3; break;
		case PIRATE:  row = 4; break;
		}
		BI_intNRetValue[0] = rand(2);
		BI_intNRetValue[1] = row;
		return &BI_intNRetValue;
	}

	if(section == "FLAGS")
	{

		BI_intNRetValue[0] = GetNationFlagCol(n);
		BI_intNRetValue[1] = 0;
		return &BI_intNRetValue;
	}

	aref chr = GetEventData();
	col = -1;

	if(section == "FRTFLG")
	{

		if(CheckAttribute(chr, "index"))
			col = -1;
		else
			col = GetNationFlagCol(n);
	}
	else
	{

		int role = GetShipRole(chr);
		if(section == "SFLAG_MIL")
		{
			if(role == 5) col = GetNationFlagCol(n); // военные
		}
		if(section == "SFLAG_MER")
		{
			if(role == 4) col = GetNationFlagCol(n); // торговцы
		}
		if(section == "SFLAG_PER")
		{
			if(role == 1) col = GetNationFlagCol(n); // ГГ + эскадра
		}
		if(section == "SFLAG_PIR")
		{
			if(role == 2) col = rand(6);             // пираты — случайный вариант
		}
		if(section == "SFLAG_QST")
		{
			if(role == 3) col = GetNationFlagCol(n); // квест
		}
	}

	BI_intNRetValue[0] = col;   // -1 → движок гасит флаг (роль не совпала)
	BI_intNRetValue[1] = 0;
	return &BI_intNRetValue;
}

void procBISelectShip()
{
	int chrIdx = GetEventData();
	int isMyChr = GetEventData();

	if(chrIdx==nMainCharacterIndex)	chrIdx = -1;

	bool bMenuOpen = false;
	if (CheckAttribute(&BattleInterface, "ComState")) { if (sti(BattleInterface.ComState) != 0) bMenuOpen = true; }
	if (chrIdx == -1 && compIdx != 0 && compIdx != nMainCharacterIndex && bCompPointers == true && bMenuOpen) {chrIdx = compIdx; isMyChr = 1;}
	else bCompPointers = true;

	float fShSpeed = 0.005;
	float fShAmp = 1.0;
	float fShW = 1.0;
	float fShH = 1.0;
	float fTop = 5.0;

	SendMessage(&objShipPointer,"lllfffff",MSG_SP_CHANGESHIP,chrIdx,isMyChr,fShSpeed,fShAmp,fShW,fShH,fTop);
}

// LEO: круглые портреты и рамка идут круглому интерфейсу и альтернативным иконкам, квадратные -
//   остальным. Правило одно на портрет и на рамку, поэтому живёт в одной функции.
bool BI_UseRoundFaces()
{
	if( AltIntIcons ) return true;
	return sti(InterfaceStates.InterfaceSelection) == KRUGLYI;
}

// Путь к портрету капитана для командлиста. Пустая строка = портрета нет, движок ничего не рисует.
string procGetCaptainFace()
{
	int chrIdx = GetEventData();
	if( chrIdx < 0 ) return "";
	ref chr = GetCharacter(chrIdx);
	if( !CheckAttribute(chr, "FaceId") ) return "";
	if( BI_UseRoundFaces() ) return "battle_interface\portraits\face_" + chr.FaceId + ".tga";
	return "battle_interface\portraits_csp\face_" + chr.FaceId + ".tga";
}

ref procGetSmallFlagData()
{
	int chrIdx = GetEventData();
	BI_intNRetValue[0] = 3;
	BI_intNRetValue[1] = 0;
	if( chrIdx >= 0 ) {
		int iNation = sti(Characters[chrIdx].nation);
		switch( iNation )
		{
		case ENGLAND: BI_intNRetValue[1]=3; break;
		case FRANCE: BI_intNRetValue[1]=2; break;
		case SPAIN: BI_intNRetValue[1]=0; break;
		case HOLLAND: BI_intNRetValue[1]=4; break;
		case PIRATE: BI_intNRetValue[1]=5; break;
		}

		BI_intNRetValue[2] = 7;
		switch( SeaAI_GetRelation(chrIdx,nMainCharacterIndex) )
		{
			case RELATION_FRIEND:	BI_intNRetValue[2] = 7; break;
			case RELATION_NEUTRAL:	BI_intNRetValue[2] = 7; break;
			case RELATION_ENEMY:	BI_intNRetValue[2] = 6; break;
		}
	}
	return &BI_intNRetValue;
}

void procMastFall()
{
	int chrIdx = GetEventData();
	string mastName = GetEventData();
	int bFirstFall = GetEventData();

	if(chrIdx<0) return;
	int mastNum = strcut(mastName,4,strlen(mastName)-1);

	SendMessage(&Sailors, "lal", MSG_PEOPLES_ON_SHIP_MASTFALL, GetCharacter(chrIdx), mastNum);
}

bool CheckInstantRepairCondition(ref chref)
{

    if (CheckOfficersPerk(chref,"InstantRepair")) return false;  // уже в деле
    if (!GetOfficersPerkUsing(chref,"InstantRepair")) return false;  // можно включить?

	float chrShipHP = GetHullPercent(chref);
	float chrShipSP = GetSailPercent(chref);

	bool bYesHPRepair = chrShipHP < InstantRepairRATE;
	bool bYesSPRepair = chrShipSP < InstantRepairRATE;

	if( bYesHPRepair )	{ bYesHPRepair = GetCargoGoods(chref,GOOD_PLANKS)>0; }
	if( bYesSPRepair )	{ bYesSPRepair = GetCargoGoods(chref,GOOD_SAILCLOTH)>0; }

	return bYesHPRepair || bYesSPRepair;
}

void BI_PerkAgainUsable()
{
	BI_SetCommandMode(-1,-1,-1,-1);
}

float GetRepairGoods(bool bIsHull, ref chref)
{
	float fGoodsQ = 0.0;

	if( bIsHull )
	{
		fGoodsQ = GetCargoGoods(chref,GOOD_PLANKS);
		if( CheckAttribute(chref,"RepairMaterials.forHull") )   // погрешность округления списания колва досок за процент
		{
			fGoodsQ += stf(chref.RepairMaterials.forHull);
		}
	}
	else
	{
		fGoodsQ = GetCargoGoods(chref,GOOD_SAILCLOTH);
		if( CheckAttribute(chref,"RepairMaterials.forSails") )
		{
			fGoodsQ += stf(chref.RepairMaterials.forSails);
		}
	}
	return fGoodsQ;
}

void RemoveRepairGoods(bool bIsHull, ref chref, float matQ)
{
	int nGoodsQ = 0;
	float fGoodsQ = GetRepairGoods(bIsHull,chref);

	if( bIsHull )
	{
		if(fGoodsQ<=matQ)	{ DeleteAttribute(chref,"RepairMaterials.forHull"); }
		else
		{
			fGoodsQ -= matQ;
			nGoodsQ = makeint(fGoodsQ);
			chref.RepairMaterials.forHull = fGoodsQ - nGoodsQ;
		}
		SetCharacterGoods(chref,GOOD_PLANKS,nGoodsQ);
	}
	else
	{
		if(fGoodsQ<=matQ)	{ DeleteAttribute(chref,"RepairMaterials.forSails"); }
		else
		{
			fGoodsQ -= matQ;
			nGoodsQ = makeint(fGoodsQ);
			chref.RepairMaterials.forSails = fGoodsQ - nGoodsQ;
		}
		SetCharacterGoods(chref,GOOD_SAILCLOTH,nGoodsQ);
	}
}

float GetRigDamage(int shootIdx, int iBallType, ref damage_chr)
{
	ref rBall = GetGoodByType(iBallType);
	float fDistanceDamageMultiply = Bring2Range(1.2, 0.25, 0.0, stf(AIBalls.CurrentMaxBallDistance), stf(AIBalls.CurrentBallDistance));
	float fDmgRig = fDistanceDamageMultiply * stf(rBall.DamageRig);
    if (shootIdx>=0 )
	{
		ref shoot_chr = GetCharacter(shootIdx);
		if (CheckOfficersPerk(shoot_chr,"CannonProfessional") )	{ fDmgRig *= 1.3; }
		else
		{
			if( CheckOfficersPerk(shoot_chr,"SailsDamageUp") )	{ fDmgRig *= 1.15; }
		}
	}

	if (CheckOfficersPerk(damage_chr,"ShipDefenseProfessional") )	{ fDmgRig *= 0.65; } // fix
	else
	{
		if (CheckOfficersPerk(damage_chr,"AdvancedBattleState") )	{ fDmgRig *= 0.75; } // fix
		else
		{
			if (CheckOfficersPerk(damage_chr,"BasicBattleState") )	{ fDmgRig *= 0.85; } // fix
		}
	}
	return fDmgRig;
}

void MakeSailDmg(int chrIdx, float dmg)
{
	object objSail;
	if( !GetEntity(&objSail,"sail") )
	{
		return;
	}
	int iSailsUpgrade = 1;
	if(!CheckAttribute(&characters[chrIdx], "upgrades.sails"))
	{
		characters[chrIdx].upgrades.sails = 1;
	}

	iSailsUpgrade = sti(characters[chrIdx].upgrades.sails) - 1;
	dmg = dmg + ((dmg/100)*iSailsUpgrade*5);
	if (CheckForUpgrade(&RealShips[sti(characters[chrIdx].Ship.Type)],"SUP_FLANGES")) dmg *= 0.5; //Защитные шкаторины
	if (GetSailPercent(&characters[chrIdx])<=0.0) dmg = 0.0;
	SendMessage(&objSail,"lslf", MSG_SAIL_SCRIPT_PROCESSING,"RandomSailsDmg", chrIdx,dmg);
}

ref ProcessRandomSailDmg()
{
	BI_g_fRetVal = 0.0;
	int chrIdx = GetEventData();
	string nodeName = GetEventData();
	int grNum = GetEventData();
	float fDmg = GetEventData();
	float fSPow = GetEventData();
	int hc_new = GetEventData();
	int mhc = GetEventData();
	int hdat_new = GetEventData();

	if(chrIdx<0) return &BI_g_fRetVal;
	ref chref = GetCharacter(chrIdx);

	aref arSail;
	string groupName = ""+grNum;
	makearef(arSail,chref.ship.sails.(nodeName).(groupName));

	int hc_old = 0;
	if( CheckAttribute(arSail,"hc") )	hc_old = sti(arSail.hc);
	float fDmg_old = 0.0;
	if( CheckAttribute(arSail,"dmg") )	fDmg_old = stf(arSail.dmg);

	int itmp = hc_new;
	float sailDmgMax = GetCharacterShipSP(chref) * fSPow;

	float fTopDmg = sailDmgMax;
	if(itmp<mhc)	fTopDmg = GetNeedDmgFromHole(itmp+1,sailDmgMax,mhc) - 0.0001;
	if(fTopDmg>fDmg_old+fDmg)	fTopDmg = fDmg_old+fDmg;

	fDmg = fTopDmg-fDmg_old;
	if(fDmg<=0.0)	return &BI_g_fRetVal;
	chref.ship.SP = stf(chref.ship.SP)-fDmg;

	itmp = GetNeedHoleFromDmg(fTopDmg,sailDmgMax,mhc);
	if(itmp>hc_old)
	{
		BI_g_fRetVal = fDmg;
		arSail.hc = hc_new;
		arSail.hd = hdat_new;
	}
	else
	{
		BI_g_fRetVal = -fDmg;
		if( !CheckAttribute(arSail,"hc") )	arSail.hc = 0;
		if( !CheckAttribute(arSail,"hd") )	arSail.hd = 0;
	}

	arSail.mhc = mhc;
	arSail.sp = fSPow;
	arSail.dmg = fTopDmg;

	return &BI_g_fRetVal;
}

void procSetUsingAbility()
{
	int q;
	int chIdx = GetEventData();
	int mainIdx = sti(pchar.index);
	aref aroot,arcur;
	string tmpStr;

	bool bDisableAbility = false;
	if( chIdx<0 ) bDisableAbility = true;
	else if( CharacterIsDead(GetCharacter(chIdx)) ) bDisableAbility = true;
	if( bDisableAbility )
	{
		makearef(aroot,BattleInterface.AbilityIcons);
		q = GetAttributesNum(aroot);
		for(int i=0; i<q; i++)
		{
			arcur = GetAttributeN(aroot,i);
			arcur.enable = false;
		}
		return;
	}

	if(mainIdx==chIdx)
	{
        BattleInterface.Commands.Bomb.enable = true;
		BattleInterface.Commands.Brander.enable			= false;
		BattleInterface.Commands.ImmediateReload.enable	= false;
        if (!CheckOfficersPerk(pchar, "ImmediateReload") && GetOfficersPerkUsing(pchar, "ImmediateReload"))
        {
            BattleInterface.Commands.ImmediateReload.enable = true;
        }
		BattleInterface.Commands.LightRepair.enable		= false;
		if (!CheckOfficersPerk(pchar, "LightRepair") && GetOfficersPerkUsing(pchar,"LightRepair"))
		{
            if(GetHullPercent(pchar)<20.0 || GetSailPercent(pchar)<20.0 || GetMastPercent(pchar)<20.0)
			{
                BattleInterface.Commands.LightRepair.enable = true;
			}
		}
		BattleInterface.Commands.InstantRepair.enable	= CheckInstantRepairCondition(pchar);
		BattleInterface.Commands.Turn180.enable = false;
		if (!CheckOfficersPerk(pchar, "Turn180") && GetOfficersPerkUsing(pchar, "Turn180"))
        {
			BattleInterface.Commands.Turn180.enable = true;
        }
        BattleInterface.Commands.SandbankManeuver.enable = false;
        if (!CheckOfficersPerk(pchar, "SandbankManeuver") && GetOfficersPerkUsing(pchar, "SandbankManeuver"))
        {
            BattleInterface.Commands.SandbankManeuver.enable = true;
        }
		q = FindQuestUsableItem(&arcur,0);
		while( q>0 )
		{
			tmpStr = arcur.id;
			if( GetCharacterItem(pchar,arcur.id)>0 )
			{
				BattleInterface.AbilityIcons.(tmpStr).enable	= true;
				BattleInterface.AbilityIcons.(tmpStr).picNum	= arcur.quest.pic;
				BattleInterface.AbilityIcons.(tmpStr).texNum	= 9;
				BattleInterface.AbilityIcons.(tmpStr).event		= arcur.id;
				BattleInterface.AbilityIcons.(tmpStr).quest		= arcur.quest;
			}
			else
			{
				BattleInterface.AbilityIcons.(tmpStr).enable	= false;
			}
			q = FindQuestUsableItem(&arcur,q+1);
		}
	}

	else
	{
		BattleInterface.Commands.Brander.enable			= false;
		BattleInterface.Commands.Bomb.enable = true;
		if(bAttack && GetRemovable(GetCharacter(chIdx)) && GetOfficersPerkUsing(pchar,"Brander"))
		{
			BattleInterface.Commands.Brander.enable		= true;
		}
		BattleInterface.Commands.InstantRepair.enable	= false;
		BattleInterface.Commands.InstantRepair.enable	= CheckInstantRepairCondition(GetCharacter(chIdx));
		BattleInterface.Commands.ImmediateReload.enable	= GetCharacterPerkUsing(GetCharacter(chIdx), "ImmediateReload");
		BattleInterface.Commands.InstantBoarding.enable	= false;
		BattleInterface.Commands.LightRepair.enable		= GetCharacterPerkUsing(GetCharacter(chIdx), "LightRepair");
		if (GetCharacterPerkUsing(GetCharacter(chIdx),"LightRepair"))
		{
            if(GetHullPercent(GetCharacter(chIdx))<20.0 || GetSailPercent(GetCharacter(chIdx))<20.0 || GetMastPercent(GetCharacter(chIdx))<20.0)
			{
                BattleInterface.Commands.LightRepair.enable = true;
			}
		}
		BattleInterface.Commands.Turn180.enable			= GetCharacterPerkUsing(GetCharacter(chIdx), "Turn180");
	}
}

ref procCheckEnableLocator()
{
	BI_intRetValue = true;

	string comName = GetEventData();
	aref arLoc = GetEventData();

	if(comName=="BI_SailTo")
	{
		if( CheckAttribute(&AISea,"Island") && CheckAttribute(arLoc,"name") )
		{
			BI_intRetValue = Island_isGotoEnableLocal(AISea.Island,arLoc.name);
		}
	}

	return &BI_intRetValue;
}

ref procCheckEnableShip()
{
	BI_intRetValue = true;

	string comName = GetEventData();
	int cn = GetEventData();

	ref rCommander;

	if(cn>=0)
	{
		switch(comName)
		{
		case "BI_Tow":
			BI_intRetValue = false;
			if (compIdx >= 0 && compIdx < TOTAL_CHARACTERS && cn < TOTAL_CHARACTERS)
				BI_intRetValue = SeaTowOrders_CanSelect(GetCharacter(compIdx), GetCharacter(cn));
		break;
		case "BI_InstantBoarding":
			BI_intRetValue = true;
		break;

		case "BI_Speak":
			BI_intRetValue = true;

			if (IsCompanion(GetCharacter(cn))) BI_intRetValue = false;
		break;
		}
	}

	return &BI_intRetValue;
}

void BI_ProcessControlPress()
{
	string ControlName = GetEventData();
	if (ControlName == "Net_Menu" || ControlName == "DlgAction2") bCompPointers = false;
	if( sti(InterfaceStates.Launched) != 0 ) {return;}

	switch(ControlName)
	{
		case "hk_charge1":
			PlayVoice("interface\"+LanguageGetLanguage()+"_balls.wav");
			Ship_ChangeCharge(pchar, GOOD_BALLS);
		break;
		case "hk_charge2":
			PlayVoice("interface\"+LanguageGetLanguage()+"_grapes.wav");
			Ship_ChangeCharge(pchar, GOOD_GRAPES);
		break;
		case "hk_charge3":
			PlayVoice("interface\"+LanguageGetLanguage()+"_chain.wav");
			Ship_ChangeCharge(pchar, GOOD_KNIPPELS);
		break;
		case "hk_charge4":
			PlayVoice("interface\"+LanguageGetLanguage()+"_bombs.wav");
			Ship_ChangeCharge(pchar, GOOD_BOMBS);
		break;
		case "BICommandsActivate":
			PlaySound("interface\ok.wav");
		break;
	}
}

ref procGetSRollSpeed()
{
	int chrIdx = GetEventData();
	BI_g_fRetVal = 0.0;
	if(chrIdx < 0) return &BI_g_fRetVal; // LEO: движок дёргает per-ship каждый кадр; у гибнущего/удаляемого корабля (взрыв ГГ) chrIdx=-1 → был спам GetCharacter(-1)
	ref chref = GetCharacter(chrIdx);
	if(CheckAttribute(chref, "Ship.RollSpeed")) BI_g_fRetVal = chref.Ship.RollSpeed;
	return &BI_g_fRetVal;
}

ref BI_GetLandData()
{
	int iTmp;
	string attrName;
	aref arLoc, arIsl, arList;
	arLoc = GetEventData();

	BI_intNRetValue[0] = 0;
	BI_intNRetValue[1] = BI_RELATION_NEUTRAL;
	BI_intNRetValue[2] = -1;
	BI_intNRetValue[3] = -1;
	BI_intNRetValue[4] = -1;
	BI_intNRetValue[5] = -1;
	BI_intNRetValue[6] = -1;

	if( CheckAttribute(arLoc,"fort.model") )
	{
		BI_intNRetValue[0] = 1; // тип форт
		int chrIdx = Fort_FindCharacter(AISea.Island,"reload",arLoc.name);
		if(chrIdx>=0)
		{
			switch( SeaAI_GetRelation(chrIdx,nMainCharacterIndex) )
			{
			case RELATION_FRIEND:	BI_intNRetValue[1] = BI_RELATION_FRIEND; break;
			case RELATION_NEUTRAL:	BI_intNRetValue[1] = BI_RELATION_NEUTRAL; break;
			case RELATION_ENEMY:	BI_intNRetValue[1] = BI_RELATION_ENEMY; break;
			}
			BI_intNRetValue[5] = chrIdx;
		}
	}
	else
	{
		if( CheckAttribute(arLoc,"istown") && arLoc.istown=="1" )
		{
			iTmp = FindLocation( arLoc.go );
			if( iTmp>=0 ) {
				iTmp = FindColony( Locations[iTmp].fastreload );
				if( iTmp>=0 ) {
					BI_intNRetValue[0] = 2; // тип город
					switch( GetNationRelation2MainCharacter(sti(Colonies[iTmp].nation)) )
					{
					case RELATION_FRIEND:	BI_intNRetValue[1] = BI_RELATION_FRIEND; break;
					case RELATION_NEUTRAL:	BI_intNRetValue[1] = BI_RELATION_NEUTRAL; break;
					case RELATION_ENEMY:	BI_intNRetValue[1] = BI_RELATION_ENEMY; break;
					}
					BI_intNRetValue[6] = sti(Colonies[iTmp].disease);
				}
			}
		}
		else
		{// не форт, не город - бухта!
			BI_intNRetValue[0] = 3; // тип бухта
		}
	}

	int g_LocLngFileID = LanguageOpenFile("LocLables.txt");
	if( CheckAttribute(arLoc,"label") ) {
		arLoc.labelLoc = LanguageConvertString(g_LocLngFileID,arLoc.label);
		string sColonyName = PChar.ColonyBuilding.ColonyName;

		if(arLoc.label == "Caiman Port")
		{
			arLoc.labelLoc = sColonyName + GetConvertStrWithReplace("Variable_BattleInterface_5", "Battle_Interface.txt", "#space#", " ");
		}
		if (HasSubStr(arLoc.label,"mayak")) BI_intNRetValue[0] = 4; //маяк
		if( arLoc.labelLoc == "" ) {
			Trace("Warning! Language: string <"+arLoc.label+"> hav`t translation into file <LocLables.txt>");
		}
	}
	LanguageCloseFile(g_LocLngFileID);

	if( BI_intNRetValue[2]<0 || BI_intNRetValue[3]<0 )
	{
		BI_intNRetValue[2] = 0;
		switch (BI_intNRetValue[0])
		{
			case 1: // форт
				BI_intNRetValue[3] = 62;
				BI_intNRetValue[4] = 46;
			break;
			case 2: // город
				BI_intNRetValue[3] = 63;
				BI_intNRetValue[4] = 47;
            break;
            case 3: // бухта
				BI_intNRetValue[3] = 31;
				BI_intNRetValue[4] = 15;
            break;
            case 4: // маяк
				BI_intNRetValue[3] = 95;
				BI_intNRetValue[4] = 79;
            break;
		}
	}

	return &BI_intNRetValue;
}

int GetTargChrIndex(int targNum, string locName)
{
	if(targNum==-1) {
		return Fort_FindCharacter(AISea.Island,"reload",locName);
	}
	else
	{
		return targNum;
	}
}

bool CheckSuccesfullBoard(ref rBoarderChr, ref rSieger)
{
	if (bBettaTestMode) return true;
	if (LAi_IsDead(rSieger)) return false;  // fix
	float k = 0.333 + GetSummonSkillFromNameToOld(rBoarderChr, SKILL_SNEAK)*0.167;

	float topLimit = 0.41 + GetSummonSkillFromNameToOld(rBoarderChr, SKILL_GRAPPLING)*0.04;

	if( pow(rand(1000)/1000.0, k) < topLimit ) return true;
	return false;
}
// рандомный герб на парус
void SetRandGeraldSail(ref chr, int nation)
{
	string ret = "";
	int    st = GetCharacterShipType(chr);
	ref    shref;

	if (st != SHIP_NOTUSED)
	{
		shref = GetRealShip(st);
		switch (nation)
		{
			case ENGLAND:
				ret = "eng_" + (1 + rand(4));
			break;
			case FRANCE:
				ret = "fra_" + (1 + rand(5));
			break;
			case SPAIN:
				ret = "spa_" + (1 + rand(4));
			break;
			case HOLLAND:
				ret = "hol_" + (1 + rand(3));
			break;
			case PIRATE:
				ret = "pir_" + (1 + rand(5));
			break;
		}
		shref.ShipSails.Gerald_Name = ret;
	}
}

void SetSailsColor(ref chr, int col)
{
	int    st = GetCharacterShipType(chr);
	ref    shref;

	if (st != SHIP_NOTUSED)
	{
		shref = GetRealShip(st);

		shref.SailsColorIdx   = col;
		shref.ShipSails.SailsColor = SailsColors[col].color;
	}
}

bool CanSetSailsGerald(ref _char)
{
	int shipClass = GetCharacterShipClass(_char); // Если корабля нет, вернёт 7 (лодка)

	if(shipClass > 4) // Проверка на класс корабля > 4
	{
		return false;
	}

	return true;
}
string GetBILocationName()
{
	UpdateCompanionsStates(); // LEO: периодический пересинхр глифов эскадры (публикует/чистит по опции на лету)
	string Name;
	if(CheckAttribute(pchar,"MoorName"))
	{
		if (pchar.MoorName == " ")
			Name = GetCurLocationName1();
		else
			Name = pchar.MoorName;
	}
	float fHtRatio = stf(Render.screen_y) / screenscaling;

	if (sti(InterfaceStates.DirectSail) && g_DirSailTarget != "")
	{
		BattleInterface.Commands.DirSail.enable = true;
		return Name + XI_ConvertString("near") + XI_ConvertString(g_DirSailTarget+"Gen")+"          ";
	}
	else
	{
		BattleInterface.Commands.DirSail.enable = false;
		return Name;
	}
}
