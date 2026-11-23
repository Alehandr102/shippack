#define ISG_CHARGE_BALLS		0
#define ISG_CHARGE_GRAPES		1
#define ISG_CHARGE_KNIPPELS		2
#define ISG_CHARGE_BOMBS		3

object objISpyGlass;

void InterfaceSpyGlassInit(bool bNet)
{
	AltIntIcons = sti(InterfaceStates.AltIntIcons) == 1; // LEO: альт-иконки НЕЗАВИСИМЫ от выбора интерфейса — накладываются поверх любого (Круглый/Квадратный/POTC)
	FillISpyGlassParameters();
	CreateEntity(&objISpyGlass,"ispyglass");
	DeleteAttribute(&objISpyGlass,"parameters");
	TelescopeInitParameters();
	// Gregg: обработчик CTRL нужен в любом режиме. Раньше он ставился только для сети, а в одиночке - один раз
	//   в SeaLoad (sea.c). InterfaceSpyGlassRelease его снимает, поэтому после любого пересоздания боевого
	//   интерфейса в том же море (каюта, скрытие/показ интерфейса, абордаж, ролик) CTRL больше не поднимал трубу.
	DelEventHandler("MSG_TELESCOPE_REQUEST", "Telescope_Request");
	SetEventHandler("MSG_TELESCOPE_REQUEST", "Telescope_Request", 0);
	if( bNet ) {
		objISpyGlass.bNet = 1;
	} else {
		objISpyGlass.bNet = 0;
		LayerAddObject(SEA_EXECUTE,&objISpyGlass,-1);
		LayerAddObject(SEA_REALIZE,&objISpyGlass,-1);
		SetEventHandler("SetTelescopeInfo","SetSpyGlassData",0);
	}
	SetEventHandler("ReleaseTelescopeInfo","ResetSpyGlassData",0);
	SetEventHandler("BI_VISIBLE","CheckInterfaceVisible",0);
	SetEventHandler("NewHUD_RefreshSpyglass","Spyglass_ReloadNewHud",0);
}

void InterfaceSpyGlassRelease()
{
	DelEventHandler("NewHUD_RefreshSpyglass","Spyglass_ReloadNewHud");
	DelEventHandler("BI_VISIBLE","CheckInterfaceVisible");
	DelEventHandler("ReleaseTelescopeInfo","ResetSpyGlassData");
	DelEventHandler("MSG_TELESCOPE_REQUEST", "Telescope_Request");
	DelEventHandler("SetTelescopeInfo","SetSpyGlassData");
	DeleteClass(&objISpyGlass);
}

void CheckInterfaceVisible()
{
	int vtype = GetEventData();
	if(vtype!=0) ResetSpyGlassData();
}

void SetSpyGlassData()
{
	bGameLogSkip = 0; // LEO: страховка Журнала игры — сброс флага пропуска на случай, если betatest-дамп трубы ниже застрял в 1 (ошибка внутри дампа → bGameLogSkip=0 не выполнялся → журнал молчал)
	int chrIdx = GetEventData();
	if(chrIdx<0) return;

	string sTextureName;
	if (sti(InterfaceStates.InterfaceSelection) == KVADRAT) sTextureName = "battle_interface\ship_icons2.dds";
	else if (sti(InterfaceStates.InterfaceSelection) == POTC) sTextureName = "battle_interface\ship_icons2_PotC.dds";
	else sTextureName = "battle_interface\ship_icons2_johnny88hud.dds";
	float uvLeft = 0;
	float uvTop = 0;
	float uvRight = 0.125;
	float uvBottom = 0.125;

	int	shipHull = -1;
	int shipSail = -1;
	int shipCrew = -1;
	int shipCannons = -1;
	int shipMaxCannons = -1;
	int shipCharge = -1;
	int shipNation = -1;
	int shipClass = 1;
	float shipSpeed = -1.0;
	string shipName = "";
	string shipType = "";
	bool isFort;
	object tmpobj;
	aref arScopeItm;
	int tmpCharge;
	ref chref = GetCharacter(chrIdx);

	if (Items_FindItem(GetCharacterEquipByGroup(pchar, SPYGLASS_ITEM_TYPE), &arScopeItm) < 0)
	{
		makearef(arScopeItm,tmpobj);
	}

	bool isDriftShip = HasSubStr(chref.id, "_DriftCap_");

	if (CheckAttribute(arScopeItm, "scope.show.nation") && (sti(arScopeItm.scope.show.nation) != 0) && !isDriftShip)
	{
		shipNation = TranslateNationCode(sti(chref.nation));
	}

	int nSailState = 1;
	float fSailSt = Ship_GetSailState(chref);
	if( fSailSt<0.3 ) {nSailState = 0;}
	else {
		if( fSailSt<0.6 ) {nSailState = 1;}
		else {nSailState = 2;}
	}

	int nFace = 65535;
	string sCaptainName = "                ";
	int nDefenceSkill = -1;
	int nGrapplingSkill = -1;
	int nCannonsSkill = -1;
	int nAccuracySkill = -1;
	int nSailingSkill = -1;

	if (!isDriftShip)
	{
		nFace = sti(chref.faceID);
		sCaptainName = GetCharacterFullName(chref.id);
	}

	if (CheckAttribute(arScopeItm, "scope.show.captain_skills") && (sti(arScopeItm.scope.show.captain_skills) != 0) && !isDriftShip)
	{
		nDefenceSkill = GetCharacterSkill(chref,SKILL_DEFENCE);
		nGrapplingSkill = GetCharacterSkill(chref,SKILL_GRAPPLING);
		nCannonsSkill = GetCharacterSkill(chref,SKILL_CANNONS);
		nAccuracySkill = GetCharacterSkill(chref,SKILL_ACCURACY);
		nSailingSkill = GetCharacterSkill(chref,SKILL_SAILING);
	}

	if(CheckAttribute(chref, "CanDropSails") && !sti(chref.CanDropSails))
	{
		nSailState = 2;
	}

	if((chref.ID == "MaryCelesteCapitan") || (CheckAttribute(pchar,"GenQuest.ShipSituation.Explosion")))
	{

		nDefenceSkill = 0;
		nGrapplingSkill = 0;
		nCannonsSkill = 0;
		nAccuracySkill = 0;
		nSailingSkill = 0;
	}

	if(CheckAttribute(chref,"Ship.Type"))
	{
		int iShip = sti(chref.Ship.Type);
		if (iShip != SHIP_NOTUSED)
		{
			iShip = sti(RealShips[iShip].basetype);
			if(ShipsTypes[iShip].name == "fort")
			{
				isFort = true;
				if (CheckAttribute(arScopeItm,"scope.show.ship_type") && sti(arScopeItm.scope.show.ship_type)!=0 )
				{
					shipType = XI_ConvertString("Fort_Name");
				}

				if (CheckAttribute(arScopeItm,"scope.show.ship_name") && sti(arScopeItm.scope.show.ship_name)!=0 )
				{
					shipName =  chref.Ship.Name;
				}

				if (CheckAttribute(arScopeItm,"scope.show.cannons") && sti(arScopeItm.scope.show.cannons)!=0 ) {
					shipCannons = Fort_GetCannonsQuantity(chref);
					if( !CheckAttribute(chref, "fort.Cannons.Quantity") ) {shipMaxCannons=0;}
					else {shipMaxCannons = sti(chref.Fort.Cannons.Quantity);}
				}
				if(CheckAttribute(arScopeItm,"scope.show.crew") && sti(arScopeItm.scope.show.crew)!=0 )
					{	shipCrew = GetCrewQuantity(chref);	}
				if( !CheckAttribute(arScopeItm,"scope.show.hull") || sti(arScopeItm.scope.show.hull)!=0 ) {
					if( CheckAttribute(chref,"ship.hp") && CheckAttribute(chref,"fort.hp") && stf(chref.Fort.hp)>0.0 ) {
						shipHull = makeint( stf(chref.ship.hp)/stf(chref.Fort.hp) * 100.0 );
					} else {
						shipHull = 100;
					}
				}
				if (CheckAttribute(arScopeItm,"scope.show.charge") && sti(arScopeItm.scope.show.charge)!=0 )
				{
					tmpCharge = sti(chref.Ship.Cannons.Charge.Type);
					switch(tmpCharge)
					{
					case GOOD_BALLS: shipCharge=0; break;
					case GOOD_GRAPES: shipCharge=3; break;
					case GOOD_KNIPPELS: shipCharge=2; break;
					case GOOD_BOMBS: shipCharge=1; break;
					}
				}
				Spyglass_GetFortIcon(&sTextureName, &uvLeft, &uvTop, &uvRight, &uvBottom);
				nSailState = -1;
			}

			else
			{
				isFort = false;
				GetTextureUVForShip(iShip, &uvLeft, &uvTop, &uvRight, &uvBottom);

				shipClass = GetCharacterShipClass(chref);
				if (CheckAttribute(arScopeItm,"scope.show.cannons") && sti(arScopeItm.scope.show.cannons)!=0 )
				{
					shipCannons = GetIntactCannonQuantity(chref);
					shipMaxCannons = GetCannonQuantity(chref);
				}
				if (CheckAttribute(arScopeItm,"scope.show.ship_name") && sti(arScopeItm.scope.show.ship_name)!=0 )
				{
					shipName =  chref.Ship.Name;
				}
				if (CheckAttribute(arScopeItm,"scope.show.ship_type") && sti(arScopeItm.scope.show.ship_type)!=0 )
				{
					shipType = XI_ConvertString(GetShipTypeName(chref));
				}

				if (!CheckAttribute(chref, "SpyglassCheckedFull"))
				{
					bool bScopeFullChk = false;
					if (CheckAttribute(arScopeItm, "scope.show.actions") && sti(arScopeItm.scope.show.actions) != 0) bScopeFullChk = true;
					if (CheckAttribute(arScopeItm, "scope.show.captain_skills") && sti(arScopeItm.scope.show.captain_skills) != 0) bScopeFullChk = true;
					if (bScopeFullChk)
					{
						chref.SpyglassCheckedFull = true;

						if (CheckAttribute(chref, "SpyglassChecked")) ShipShowNameSingle(chref);
					}
				}
				if (!CheckAttribute(chref,"SpyglassChecked"))
				{
					chref.SpyglassChecked = true;
					ShipShowNameSingle(chref);
				}
				if (CheckAttribute(arScopeItm,"scope.show.hull") && sti(arScopeItm.scope.show.hull) == true )
				{
					shipHull = makeint(GetHullPercent(chref));
				}
				if (CheckAttribute(arScopeItm,"scope.show.sail") && sti(arScopeItm.scope.show.sail) == true )
				{
					shipSail = makeint(GetSailPercent(chref));
				}
				if (CheckAttribute(arScopeItm,"scope.show.crew") && sti(arScopeItm.scope.show.crew) == true )
				{
					shipCrew = GetCrewQuantity(chref);
				}
				if (CheckAttribute(arScopeItm,"scope.show.speed") && sti(arScopeItm.scope.show.speed) == true)
				{
					shipSpeed = stf(chref.Ship.speed.z) * stf(BattleInterface.ShipSpeedScaler);
				}
				if (CheckAttribute(arScopeItm,"scope.show.charge") && sti(arScopeItm.scope.show.charge) == true )
				{
					tmpCharge = sti(chref.Ship.Cannons.Charge.Type);
					switch(tmpCharge)
					{
					case GOOD_BALLS:     shipCharge=0; break;
					case GOOD_GRAPES:    shipCharge=3; break;
					case GOOD_KNIPPELS:  shipCharge=2; break;
					case GOOD_BOMBS:     shipCharge=1; break;
					}
				}
			}
		}
	}
	if (CheckAttribute(arScopeItm,"scope.show.cannontype") && sti(arScopeItm.scope.show.cannontype)== true)
	{
		string CannonTypeName = "";
		switch (sti(chref.Ship.Cannons.Type))
		{
            case CANNON_TYPE_CANNON_LBS8:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_2", "Battle_Interface.txt", "#space#", " ");break;
            case CANNON_TYPE_CANNON_LBS10:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_3", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CANNON_LBS12:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_4", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CANNON_LBS16:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_5", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CANNON_LBS20:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_6", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CANNON_LBS24:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_7", "Battle_Interface.txt", "#space#", " ");break;
            case CANNON_TYPE_CANNON_LBS28:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_8", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CANNON_LBS32:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_9", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CANNON_LBS36:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_10", "Battle_Interface.txt", "#space#", " ");break;
		    case CANNON_TYPE_CANNON_LBS42:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_11", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CANNON_LBS48:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_12", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CULVERINE_LBS8:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_13", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CULVERINE_LBS10:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_14", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CULVERINE_LBS12:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_15", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CULVERINE_LBS16:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_16", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CULVERINE_LBS20:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_17", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CULVERINE_LBS24:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_18", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CULVERINE_LBS28:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_19", "Battle_Interface.txt", "#space#", " ");break;
		    case CANNON_TYPE_CULVERINE_LBS32:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_20", "Battle_Interface.txt", "#space#", " ");break;
			case CANNON_TYPE_CULVERINE_LBS36:CannonTypeName = GetConvertStrWithReplace("Variable_ispyglass_21", "Battle_Interface.txt", "#space#", " ");break;
		}
	}

	sCaptainName = CannonTypeName + "  " + sCaptainName;
    float fDistance = stf(FloatToString(Ship_GetDistance2D(GetMainCharacter(), chref), 1));
	Spyglass_SetTargetNewHud(chref, shipCrew);
	Spyglass_SetTargetLayout(isFort);
	SendMessage(&objISpyGlass,"lsslllfflllllllllllssl",MSG_ISG_UPDATE, shipName,shipType,
		shipHull,shipSail,shipCrew,	shipSpeed, fDistance,
		shipCannons,shipMaxCannons,
		shipCharge,shipNation, nSailState,nFace,
		nDefenceSkill,nGrapplingSkill,nCannonsSkill,nAccuracySkill,nSailingSkill,
		sCaptainName,"",shipClass);
	SendMessage(&objISpyGlass,"lsffff",MSG_ISG_SET_SHIPICON, sTextureName, uvLeft,uvTop,uvRight,uvBottom);
	SendShipUpgradesToSpyglass(chref, arScopeItm); // LEO: ряд иконок апгрейдов цели (только если труба показывает апгрейды)
	SendMessage(&objISpyGlass,"ll",MSG_ISG_VISIBLE,true);

	if (bBettaTestMode)
	{
		bGameLogSkip = 1; // LEO: отладочный дамп подзорной трубы НЕ писать в Журнал игры
		ClearAllLogStrings();
		if (CheckAttribute(chref, "SeaAI.Task"))
		{
		    Log_SetStringToLog("shipType=" + GetCharacterShipType(chref) +
		                               " SeaAI.Task=" + chref.SeaAI.Task);
			if (CheckAttribute(chref, "SeaAI.Task.Target"))
			{
		        sCaptainName = "none";
				if (sti(chref.SeaAI.Task.Target) != -1)  sCaptainName = Characters[sti(chref.SeaAI.Task.Target)].id;

				Log_Info("SeaAI.Task.Target = " + chref.SeaAI.Task.Target + " id npc = " + sCaptainName);
			}
		}
		if (TestRansackCaptain && !bQuestLogShow && rand(20) == 4) // иначе не отжать кнопку
		{
		    pchar.SystemInfo.OnlyShowCharacter = true;
		    pchar.SystemInfo.CheckNpcIdx = sti(chref.index); // LEO: проверяемый NPC - вкладки меню (персонаж/корабль) роутятся на него
		    LaunchCharacter(chref);
		}
		if (TestRansackCaptain && bQuestLogShow && rand(20) == 4) // иначе не отжать кнопку
		{
		    pchar.SystemInfo.CheckNpcIdx = sti(chref.index); // LEO: проверяемый NPC для вкладок чеккера
		    LaunchShipStateNPC(chref);
		}
		Log_SetStringToLog("Cap_id = " + chref.id +" hp= " + makeint(chref.chr_ai.hp) + " rank= "+ chref.rank +
		                           " Moral= " + chref.ship.crew.morale);
		Log_SetStringToLog("Ship.MaxCaliber=  " + RealShips[sti(chref.Ship.Type)].MaxCaliber +
		                           " Ship.Capacity =" + RealShips[sti(chref.Ship.Type)].Capacity  +
		                           " Ship.MaxCrew= " + RealShips[sti(chref.Ship.Type)].MaxCrew);

		Log_SetStringToLog("Ship.SpeedRate= " + RealShips[sti(chref.Ship.Type)].SpeedRate +
		                           " Ship.TurnRate =" + RealShips[sti(chref.Ship.Type)].TurnRate  +
		                           " Ship.HP=" + RealShips[sti(chref.Ship.Type)].HP);
		Log_Info("CheckForSurrender = " + CheckForSurrender(pchar, chref, 1));
		Log_Info("CheckLockTask = " + CheckAttribute(chref, "ShipTaskLock"));
		Log_Info("CheckWatchFort = " + CheckAttribute(chref, "WatchFort"));
		if (CheckAttribute(chref, "EncType")) Log_Info("EncType = " + chref.EncType);
		if (CheckAttribute(chref, "Ship.Mode")) Log_Info("Ship.Mode = " + chref.Ship.Mode);
		if (CheckAttribute(chref, "Ship.LastBallCharacter")) Log_Info("LastBallCharacter = " + chref.Ship.LastBallCharacter);
		if (CheckAttribute(arScopeItm,"scope.show.mushketshot") && sti(arScopeItm.scope.show.mushketshot) != 0  && CheckOfficersPerk(chref, "MusketsShoot")) Log_Info("MushketShot enabled");
		if (CheckAttribute(chref,"Situation")) Log_Info("Cap. Situation : " + chref.situation +" type :" + chref.situation.type);
		ActiveF12Controll(sti(chref.index));
		bGameLogSkip = 0; // LEO: конец отладочного дампа трубы
	}

}

// LEO: атлас иконок апгрейдов ShipUpgradesMax.dds (2048x2048, иконки "On" 256x256). По индексу idx (0..17) возвращает id апгрейда (return) и его UV-прямоугольник (через float&)
string GetUpgradeAtlasUV(int idx, ref uvL, ref uvT, ref uvR, ref uvB)
{
	int px = -1;
	int py = -1;
	string supId = "";
	switch (idx)
	{
	case  0: supId = "SUP_TRADEARMOUR";            px = 0;    py = 256;  break;
	case  1: supId = "SUP_SAIL";                   px = 256;  py = 256;  break;
	case  2: supId = "SUP_GUNS";                   px = 512;  py = 256;  break;
	case  3: supId = "SUP_HULLKRENG";              px = 768;  py = 256;  break;
	case  4: supId = "SUP_CAPACITY";               px = 1024; py = 256;  break;
	case  5: supId = "SUP_BIGSIDES";               px = 1280; py = 256;  break;
	case  6: supId = "SUP_MASTS";                  px = 1536; py = 256;  break;
	case  7: supId = "SUP_HAMMOCKS";               px = 1792; py = 256;  break;
	case  8: supId = "SUP_HELM";                   px = 0;    py = 768;  break;
	case  9: supId = "SUP_BARRICADES";             px = 256;  py = 768;  break;
	case 10: supId = "SUP_PUMPS";                  px = 512;  py = 768;  break;
	case 11: supId = "SUP_LAFETTES";               px = 768;  py = 768;  break;
	case 12: supId = "SUP_FALCONETS";              px = 1024; py = 768;  break;
	case 13: supId = "SUP_FLANGES";                px = 1280; py = 768;  break;
	case 14: supId = "SUP_BRANDER";                px = 1536; py = 768;  break;
	case 15: supId = "SUP_COMBATARSENAL";          px = 1792; py = 768;  break;
	case 16: supId = "SUP_ADDITIONALARSENAL";      px = 0;    py = 1280; break;
	case 17: supId = "SUP_REINFORCEDCONSTRUCTION"; px = 256;  py = 1280; break;
	}
	uvL = makefloat(px) / 2048.0;
	uvT = makefloat(py) / 2048.0;
	uvR = makefloat(px + 256) / 2048.0;
	uvB = makefloat(py + 256) / 2048.0;
	return supId;
}

// LEO: шлёт в трубу ряд иконок апгрейдов корабля цели (до 6). Незанятые слоты гасит. Гейт: scope.show.upgrades (отличная труба+) и не форт
void SendShipUpgradesToSpyglass(ref chref, aref arScopeItm)
{
	int slot = 0;
	bool bAllow = false;

	if (CheckAttribute(arScopeItm, "scope.show.upgrades") && sti(arScopeItm.scope.show.upgrades) != 0) bAllow = true;

	if (CheckAttribute(arScopeItm, "scope.show.captain_skills") && sti(arScopeItm.scope.show.captain_skills) != 0) bAllow = true;
	if (bAllow && CheckAttribute(chref, "Ship.Type"))
	{
		int shipType = sti(chref.Ship.Type);
		if (shipType != SHIP_NOTUSED)
		{
			int baseT = sti(RealShips[shipType].basetype);
			if (ShipsTypes[baseT].name != "fort")
			{
				int i;
				for (i = 0; (i < 18) && (slot < 6); i++)
				{

					string supId = ShipsUpgrades[i];
					if (CheckForUpgrade(&RealShips[shipType], supId))
					{
						float uvL = 0.0;
						float uvT = 0.0;
						float uvR = 0.0;
						float uvB = 0.0;
						GetUpgradeAtlasUV(i, &uvL, &uvT, &uvR, &uvB);
						SendMessage(&objISpyGlass, "lllffffs", MSG_ISG_SET_UPGRADE, slot, 1, uvL, uvT, uvR, uvB, "interfaces\ShipUpgradesMax.dds");
						slot = slot + 1;
					}
				}

				if (slot < 7 && !LAi_IsDead(chref) && CheckCharacterPerk(chref, "MusketsShoot"))
				{
					SendMessage(&objISpyGlass, "lllffffs", MSG_ISG_SET_UPGRADE, slot, 1, 0.625, 0.375, 0.75, 0.5, "interfaces\PERKS\PERK_ENABLE.dds"); // LEO: UV ячейки MusketsShoot 640,384,768,512 / 1024 (текстура 1024x1024)
					slot = slot + 1;
				}
			}
		}
	}

	int j;
	for (j = slot; j < 7; j++)
		SendMessage(&objISpyGlass, "lllffffs", MSG_ISG_SET_UPGRADE, j, 0, 0.0, 0.0, 0.0, 0.0, "");
}

///  статы персонажа в близи
void ActiveF12Controll(int chr)
{

    int res = chr;
    if (res != -1)
    {
        ref findCh = GetCharacter(res);
        res = findCh.chr_ai.hp;
        Log_SetStringToLog("" + GetFullName(findCh) +
                           " "+XI_ConvertString("Rank")+" " + findCh.rank + " "+XI_ConvertString("Health")+" "+res + "/" + LAi_GetCharacterMaxHP(findCh));

		if (bBettaTestMode)
        {
            Log_SetStringToLog("Id= " + findCh.id);
            Log_SetStringToLog("Idx= " + findCh.index);
            if(CheckAttribute(findCh, "LifeDay"))
		Log_Info("LifeDay - " + findCh.LifeDay);

	    if(CheckAttribute(findCh, "City"))
	    	Log_Info("City - " + findCh.City);
            if (CheckAttribute(findCh, "equip.blade"))
            {
                Log_SetStringToLog("Blade= " + findCh.equip.blade);
            }
            if (CheckAttribute(findCh, "equip.pistol"))
            {
                Log_SetStringToLog("Pistol= " + findCh.equip.pistol);
            }
            Log_SetStringToLog("model= " + findCh.model);
            Log_SetStringToLog("face= " + findCh.faceId);

            if (CheckAttribute(findCh, "chr_ai.type")) Log_SetStringToLog("chr_ai.type= " + findCh.chr_ai.type);
            else Log_SetStringToLog("chr_ai.type= - (не сухопутный ИИ)");
            if (CheckAttribute(findCh, "location.group"))
                Log_SetStringToLog("Group= " + findCh.location.group + " locator= " + findCh.location.locator);
            else Log_SetStringToLog("Group= - (не на сухопутной локации)");

            int iRelChk = GetRelation(sti(findCh.index), GetMainCharacterIndex());
            string sRelChk = "нейтрал";
            if (iRelChk == RELATION_ENEMY) sRelChk = "ВРАГ";
            if (iRelChk == RELATION_FRIEND) sRelChk = "друг";
            if (CheckAttribute(findCh, "AlwaysEnemy")) sRelChk = sRelChk + " (AlwaysEnemy)";
            Log_SetStringToLog("Отношение к ГГ= " + sRelChk);

            if (CheckAttribute(findCh, "SeaAI.Group.Name")) Log_SetStringToLog("SeaAI.Group= " + findCh.SeaAI.Group.Name);
            if (CheckAttribute(findCh, "SeaAI.Task")) Log_SetStringToLog("SeaAI.Task= " + findCh.SeaAI.Task);
            if (CheckAttribute(findCh, "Ship.Mode")) Log_SetStringToLog("Ship.Mode= " + findCh.Ship.Mode);
            if (CheckAttribute(findCh, "cirassId"))
            {
                Log_SetStringToLog("cirassId= " + sti(findCh.cirassId));
            }
            else
            {
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_ispyglass_22", "Battle_Interface.txt", "#space#", " "));
            }
            if (CheckAttribute(findCh, "chr_ai.FencingType"))
            {
                Log_SetStringToLog("chr_ai.FencingType= " + findCh.chr_ai.FencingType));
            }
            else
            {
                Log_SetStringToLog("Нет FencingType - error");
            }
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_ispyglass_23", "Battle_Interface.txt", "#space#", " ") + findCh.nation);
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_ispyglass_24", "Battle_Interface.txt", "#space#", " ") + findCh.sex);

			if (CheckAttribute(findCh, "chr_ai.group"))
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_ispyglass_25", "Battle_Interface.txt", "#space#", " ") + findCh.chr_ai.group);
			if (CheckAttribute(findCh, "chr_ai.tmpl"))
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_ispyglass_26", "Battle_Interface.txt", "#space#", " ") + findCh.chr_ai.tmpl);
			if (CheckAttribute(findCh, "chr_ai.tmpl.state"))
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_ispyglass_27", "Battle_Interface.txt", "#space#", " ") + findCh.chr_ai.tmpl.state);
			dumpattributes(findCh);

            pchar.SystemInfo.OnlyShowCharacter = true;
            pchar.SystemInfo.CheckNpcIdx = sti(findCh.index); // LEO: проверяемый NPC - для кнопки "Корабль" в чеккере
			LaunchCharacter(findCh);
        }
        else
        {
            if (MOD_BETTATESTMODE == "Test")
	        {
	            Log_SetStringToLog("Id= " + findCh.id);
	        }
        }
    }
}

void ResetSpyGlassData()
{
	SendMessage(&objISpyGlass,"ll",MSG_ISG_VISIBLE,false);
}

int TranslateNationCode(int nationCode)
{
	switch(nationCode)
	{
		case ENGLAND:
			return 0;
		break;
		case FRANCE:
			return 1;
		break;
		case SPAIN:
			return 2;
		break;
		case HOLLAND:
			return 3;
		break;
		case PIRATE:
			return 4;
		break;
	}
	return -1;
}

void FillISpyGlassParameters()
{

	string sTexture = "spyglass1";
	if (CheckAttribute(pchar, "equip.spyglass"))
	{
	    sTexture = pchar.equip.spyglass;
	}
	int nHCenter = sti(showWindow.right)/2;

	if(InterfaceStates.SpyglassTextures) objISpyGlass.lens.texture = "battle_interface\spyglassEHO\" + sTexture + ".dds";
	else objISpyGlass.lens.texture = "battle_interface\spyglass\" + sTexture + ".dds";

	objISpyGlass.lens.pos = (nHCenter-sti(showWindow.bottom))+","+(sti(showWindow.top)-1)+","+(nHCenter+sti(showWindow.bottom))+","+showWindow.bottom;

	int ntop = sti(showWindow.bottom) - RecalculateVIconScaled(114);
	int nbottom = ntop + RecalculateVIconScaled(128);

	if (sti(InterfaceStates.InterfaceSelection) == KVADRAT)
	{
		objISpyGlass.shipsign.ship.texture = "battle_interface\ship_icons2.dds";
		objISpyGlass.shipsign.ship.pos = RecalculateHIconScaled(8)+","+(ntop+RecalculateVIconScaled(-2)) + "," + RecalculateHIconScaled(100)+","+(ntop+RecalculateVIconScaled(92));
		objISpyGlass.shipsign.ship.uv = "0,0,0.125,0.125";
		objISpyGlass.shipsign.back.texture = "battle_interface\Ship_Border.dds";
		objISpyGlass.shipsign.back.pos = RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(0)) + "," + RecalculateHIconScaled(100)+","+(nbottom+RecalculateVIconScaled(-38));

		objISpyGlass.shipsign.hp.texture = "battle_interface\ShipHP_SP.dds";
		objISpyGlass.shipsign.hp.pos = RecalculateHIconScaled(4)+","+(ntop+RecalculateVIconScaled(0)) + "," + RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(90));
		objISpyGlass.shipsign.hp.uv = "0.0,0.0,0.5,1.0";
		objISpyGlass.shipsign.sp.texture = "battle_interface\ShipHP_SP.dds";
		objISpyGlass.shipsign.sp.pos = RecalculateHIconScaled(100)+","+(ntop+RecalculateVIconScaled(0)) + "," + RecalculateHIconScaled(106)+","+(ntop+RecalculateVIconScaled(90));
		objISpyGlass.shipsign.sp.uv = "0.5,0.0,1.0,1.0";
		objISpyGlass.shipsign.class.texture = "";
		objISpyGlass.shipsign.class.pos = RecalculateHIconScaled(28)+","+(ntop+RecalculateVIconScaled(1)) + "," + RecalculateHIconScaled(90)+","+(ntop+RecalculateVIconScaled(15));
	}
	else if (sti(InterfaceStates.InterfaceSelection) == POTC)
	{
		objISpyGlass.shipsign.ship.texture = "battle_interface\ship_icons2_PotC.dds";
		objISpyGlass.shipsign.ship.pos = RecalculateHIconScaled(11)+","+(ntop+RecalculateVIconScaled(1)) + "," + RecalculateHIconScaled(98)+","+(ntop+RecalculateVIconScaled(89));
		objISpyGlass.shipsign.ship.uv = "0,0,0.125,0.125";
		objISpyGlass.shipsign.back.texture = "battle_interface\Ship_Border_PotC.dds";
		objISpyGlass.shipsign.back.pos = RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(0)) + "," + RecalculateHIconScaled(100)+","+(nbottom+RecalculateVIconScaled(-38));

		objISpyGlass.shipsign.hp.texture = "battle_interface\ShipHP_SP.dds";
		objISpyGlass.shipsign.hp.pos = RecalculateHIconScaled(4)+","+(ntop+RecalculateVIconScaled(0)) + "," + RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(90));
		objISpyGlass.shipsign.hp.uv = "0.0,0.0,0.5,1.0";
		objISpyGlass.shipsign.sp.texture = "battle_interface\ShipHP_SP.dds";
		objISpyGlass.shipsign.sp.pos = RecalculateHIconScaled(100)+","+(ntop+RecalculateVIconScaled(0)) + "," + RecalculateHIconScaled(106)+","+(ntop+RecalculateVIconScaled(90));
		objISpyGlass.shipsign.sp.uv = "0.5,0.0,1.0,1.0";
		objISpyGlass.shipsign.class.texture = "";
		objISpyGlass.shipsign.class.pos = RecalculateHIconScaled(28)+","+(ntop+RecalculateVIconScaled(1)) + "," + RecalculateHIconScaled(90)+","+(ntop+RecalculateVIconScaled(15));
	}
	else
	{
		objISpyGlass.shipsign.ship.texture = "battle_interface\ship_icons2.dds";
		objISpyGlass.shipsign.ship.pos = RecalculateHIconScaled(20)+","+(ntop+RecalculateVIconScaled(6)) + "," + RecalculateHIconScaled(106)+","+(ntop+RecalculateVIconScaled(90));
		objISpyGlass.shipsign.ship.uv = "0,0,0.125,0.125";
		objISpyGlass.shipsign.back.texture = "battle_interface\shipbackicon.dds";
		objISpyGlass.shipsign.back.pos = RecalculateHIconScaled(0)+","+(ntop+RecalculateVIconScaled(-10)) + "," + RecalculateHIconScaled(128)+","+(nbottom+RecalculateVIconScaled(-10));
		objISpyGlass.shipsign.hp.texture = "battle_interface\ShipState.dds";
		objISpyGlass.shipsign.hp.pos = RecalculateHIconScaled(0)+","+(ntop+RecalculateVIconScaled(4)) + "," + RecalculateHIconScaled(64)+","+(ntop+RecalculateVIconScaled(93));
		objISpyGlass.shipsign.hp.uv = "0.0,0.109,0.5,0.79";
		objISpyGlass.shipsign.sp.texture = "battle_interface\ShipState.dds";
		objISpyGlass.shipsign.sp.pos = RecalculateHIconScaled(64)+","+(ntop+RecalculateVIconScaled(4)) + "," + RecalculateHIconScaled(128)+","+(ntop+RecalculateVIconScaled(93));
		objISpyGlass.shipsign.sp.uv = "0.5,0.109,1.0,0.79";
		objISpyGlass.shipsign.class.texture = "";
		objISpyGlass.shipsign.class.pos = RecalculateHIconScaled(28)+","+(ntop+RecalculateVIconScaled(1)) + "," + RecalculateHIconScaled(90)+","+(ntop+RecalculateVIconScaled(15));
	}

	objISpyGlass.info.nation.texture = "nations.dds"; // LEO: новая текстура наций (1024x128, 6 флагов по 128px: Испания/Пираты/Голландия/Франция/Англия/Контрабандисты)

	if (sti(InterfaceStates.InterfaceSelection) != KRUGLYI) objISpyGlass.info.nation.pos = RecalculateHIconScaled(116)+","+(ntop+RecalculateVIconScaled(8)) + "," + RecalculateHIconScaled(188)+","+(ntop+RecalculateVIconScaled(80));
	else objISpyGlass.info.nation.pos = RecalculateHIconScaled(133)+","+(ntop+RecalculateVIconScaled(18)) + "," + RecalculateHIconScaled(205)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.info.nation.uv = "0,0,0.125,1.0";

	objISpyGlass.info.cannon.texture = CommandIcons_Texture("List_icons");
	objISpyGlass.info.cannon.pos = RecalculateHIconScaled(212)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(260)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.info.cannon.uv = "0.625,0.375,0.6875,0.5";

	objISpyGlass.info.speed.texture = CommandIcons_Texture("LIST_ICON2");
	objISpyGlass.info.speed.pos = RecalculateHIconScaled(284)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(332)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.info.speed.uv = "0.25,0.25,0.375,0.375";
	objISpyGlass.info.sailto.texture = CommandIcons_Texture("List_icons");
	objISpyGlass.info.sailto.pos = RecalculateHIconScaled(428)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(476)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.info.sailto.uv = "0.75,0.25,0.8125,0.375";

	objISpyGlass.info.charge.texture = CommandIcons_Texture("LIST_ICON2");
	objISpyGlass.info.charge.pos = RecalculateHIconScaled(356)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(404)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.info.charge.uv = "0.125,0.5,0.25,0.625";

	objISpyGlass.captain.back.texture = "battle_interface\shipbackiconA.dds";
	objISpyGlass.captain.back.pos = RecalculateHIconScaled(906)+","+ntop + "," + RecalculateHIconScaled(1034)+","+nbottom;
	objISpyGlass.captain.face.texture = "battle_interface\portraits\face_0.tga";
	objISpyGlass.captain.face.pos = RecalculateHIconScaled(924)+","+(ntop+RecalculateVIconScaled(20)) + "," + RecalculateHIconScaled(988)+","+(ntop+RecalculateVIconScaled(84));

	objISpyGlass.captain.fencing.texture = "interfaces\icons_spec.dds";
	objISpyGlass.captain.fencing.pos = RecalculateHIconScaled(830)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(878)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.captain.fencing.uv = "0.875,0.125,1.0,0.25";   // защита
	objISpyGlass.captain.cannon.texture = "interfaces\icons_spec.dds";
	objISpyGlass.captain.cannon.pos = RecalculateHIconScaled(758)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(806)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.captain.cannon.uv = "0.75,0.125,0.875,0.25";  // абордаж
	objISpyGlass.captain.accuracy.texture = "interfaces\icons_spec.dds";
	objISpyGlass.captain.accuracy.pos = RecalculateHIconScaled(686)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(734)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.captain.accuracy.uv = "0.375,0.125,0.5,0.25";  // орудия
	objISpyGlass.captain.navigation.texture = "interfaces\icons_spec.dds";
	objISpyGlass.captain.navigation.pos = RecalculateHIconScaled(614)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(662)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.captain.navigation.uv = "0.25,0.125,0.375,0.25"; // меткость
	objISpyGlass.captain.boarding.texture = "interfaces\icons_spec.dds";
	objISpyGlass.captain.boarding.pos = RecalculateHIconScaled(542)+","+(ntop+RecalculateVIconScaled(42)) + "," + RecalculateHIconScaled(590)+","+(ntop+RecalculateVIconScaled(90));
	objISpyGlass.captain.boarding.uv = "0.5,0.125,0.625,0.25";  // навигация

	float fBaseScale = stf(showWindow.width) / 1310.0;
	if (sti(InterfaceStates.InterfaceSelection) != KRUGLYI)
	{
		objISpyGlass.text.shiptype.font = "interface_normal"; // Тип корабля
		objISpyGlass.text.shiptype.pos = RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(-28));
		objISpyGlass.text.shiptype.color = argb(255,255,255,168);
		objISpyGlass.text.shiptype.scale = fBaseScale * 1.1;

		objISpyGlass.text.shipname.font = "interface_normal"; // Имя корабля
		objISpyGlass.text.shipname.pos = RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(-15));
		objISpyGlass.text.shipname.color = argb(255,255,255,168);
		objISpyGlass.text.shipname.scale = fBaseScale * 1.1;
	}
	else
	{
		objISpyGlass.text.shiptype.font = "interface_normal"; // Тип корабля
		objISpyGlass.text.shiptype.pos = RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(-28));
		objISpyGlass.text.shiptype.color = argb(255,255,255,168);
		objISpyGlass.text.shiptype.scale = fBaseScale * 1.1;

		objISpyGlass.text.shipname.font = "interface_normal"; // Имя корабля
		objISpyGlass.text.shipname.pos = RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(-15));
		objISpyGlass.text.shipname.color = argb(255,255,255,168);
		objISpyGlass.text.shipname.scale = fBaseScale * 1.1;
	}

	objISpyGlass.text.cannons.font = "interface_normal";
	objISpyGlass.text.cannons.pos = RecalculateHIconScaled(236)+","+(ntop+RecalculateVIconScaled(26));
	objISpyGlass.text.cannons.align = "center";
	objISpyGlass.text.cannons.color = argb(255,255,255,168);
	objISpyGlass.text.cannons.scale = fBaseScale * 1.2;

	objISpyGlass.text.speed.font = "interface_normal";
	objISpyGlass.text.speed.pos = RecalculateHIconScaled(308)+","+(ntop+RecalculateVIconScaled(26));
	objISpyGlass.text.speed.align = "center";
	objISpyGlass.text.speed.color = argb(255,255,255,168);
	objISpyGlass.text.speed.scale = fBaseScale * 1.2;

	objISpyGlass.text.sailto.font = "interface_normal";
	objISpyGlass.text.sailto.pos = RecalculateHIconScaled(452)+","+(ntop+RecalculateVIconScaled(26));
	objISpyGlass.text.sailto.align = "center";
	objISpyGlass.text.sailto.color = argb(255,255,255,168);
	objISpyGlass.text.sailto.scale = fBaseScale * 1.2;

	if(sti(InterfaceStates.InterfaceSelection) != KRUGLYI)
	{
		objISpyGlass.text.crew.font = "bold_numbers";
		objISpyGlass.text.crew.pos = RecalculateHIconScaled(55)+","+(ntop+RecalculateVIconScaled(95));
		objISpyGlass.text.crew.align = "center";
		objISpyGlass.text.crew.color = argb(255,255,255,168);
		objISpyGlass.text.crew.scale = fBaseScale * 1.0;
	}
	else
	{
		objISpyGlass.text.crew.font = "bold_numbers";
		objISpyGlass.text.crew.pos = RecalculateHIconScaled(64)+","+(ntop+RecalculateVIconScaled(90));
		objISpyGlass.text.crew.align = "center";
		objISpyGlass.text.crew.color = argb(255,255,255,168);
		objISpyGlass.text.crew.scale = fBaseScale * 1.0;
	}

	objISpyGlass.captext.capname.font = "interface_normal";
	objISpyGlass.captext.capname.pos = RecalculateHIconScaled(1000)+","+(ntop-RecalculateVIconScaled(16));
	objISpyGlass.captext.capname.align = "right";
	objISpyGlass.captext.capname.color = argb(255,255,255,168);
	objISpyGlass.captext.capname.scale = fBaseScale * 1.6;

	objISpyGlass.captext.fencing.font = "interface_normal";
	objISpyGlass.captext.fencing.pos = RecalculateHIconScaled(856)+","+(ntop+RecalculateVIconScaled(26));
	objISpyGlass.captext.fencing.align = "center";
	objISpyGlass.captext.fencing.color = argb(255,255,255,168);
	objISpyGlass.captext.fencing.scale = fBaseScale * 1.4;

	objISpyGlass.captext.cannon.font = "interface_normal";
	objISpyGlass.captext.cannon.pos = RecalculateHIconScaled(784)+","+(ntop+RecalculateVIconScaled(26));
	objISpyGlass.captext.cannon.align = "center";
	objISpyGlass.captext.cannon.color = argb(255,255,255,168);
	objISpyGlass.captext.cannon.scale = fBaseScale * 1.4;

	objISpyGlass.captext.accuracy.font = "interface_normal";
	objISpyGlass.captext.accuracy.pos = RecalculateHIconScaled(712)+","+(ntop+RecalculateVIconScaled(26));
	objISpyGlass.captext.accuracy.align = "center";
	objISpyGlass.captext.accuracy.color = argb(255,255,255,168);
	objISpyGlass.captext.accuracy.scale = fBaseScale * 1.4;

	objISpyGlass.captext.navigation.font = "interface_normal";
	objISpyGlass.captext.navigation.pos = RecalculateHIconScaled(640)+","+(ntop+RecalculateVIconScaled(26));
	objISpyGlass.captext.navigation.align = "center";
	objISpyGlass.captext.navigation.color = argb(255,255,255,168);
	objISpyGlass.captext.navigation.scale = fBaseScale * 1.4;

	objISpyGlass.captext.boarding.font = "interface_normal";
	objISpyGlass.captext.boarding.pos = RecalculateHIconScaled(566)+","+(ntop+RecalculateVIconScaled(26));
	objISpyGlass.captext.boarding.align = "center";
	objISpyGlass.captext.boarding.color = argb(255,255,255,168);
	objISpyGlass.captext.boarding.scale = fBaseScale * 1.4;

	int upUpBaseX = 212;  // левый край ряда — вровень с рядом стат-иконок (cannon@212)
	int upUpPitch = 44;   // LEO: шаг между иконками (сжато с 50 под 7 слотов: 6 апгрейдов + мушкетный залп)
	int upUpSize  = 40;   // размер иконки
	int upUpTop = ntop - RecalculateVIconScaled(36); // над числами статов (числа@ntop+26, иконки@ntop+42)
	int upUpBot = ntop + RecalculateVIconScaled(4);
	int iUpIco;
	for (iUpIco = 0; iUpIco < 7; iUpIco++)  // LEO: 7 слотов (6 апгрейдов + слот мушкетного залпа)
	{
		string sUpIco = "icon" + iUpIco;
		objISpyGlass.upgrades.(sUpIco).texture = "interfaces\ShipUpgradesMax.dds";
		objISpyGlass.upgrades.(sUpIco).color = argb(255,128,128,128); // LEO: рендер трубы множит яркость x2 — 0x80 даёт нормальный цвет (0xFF = выжженый)
		objISpyGlass.upgrades.(sUpIco).pos = RecalculateHIconScaled(upUpBaseX + iUpIco*upUpPitch)+","+upUpTop+","+RecalculateHIconScaled(upUpBaseX + iUpIco*upUpPitch + upUpSize)+","+upUpBot;
	}

	objISpyGlass.nationuvarray.uv0 = "0.5,0,0.625,1.0"; // LEO: england (5-й слот текстуры)
	objISpyGlass.nationuvarray.uv1 = "0.375,0,0.5,1.0"; // LEO: france (4-й слот)
	objISpyGlass.nationuvarray.uv2 = "0,0,0.125,1.0"; // LEO: spain (1-й слот)
	objISpyGlass.nationuvarray.uv3 = "0.25,0,0.375,1.0"; // LEO: holland (3-й слот)
	objISpyGlass.nationuvarray.uv4 = "0.125,0,0.25,1.0"; // LEO: pirate (2-й слот)
	objISpyGlass.nationuvarray.uv5 = "0.625,0,0.75,1.0"; // LEO: smuggler (6-й слот)

	objISpyGlass.chargeuvarray.uv0 = "0.0,0.5,0.125,0.625"; // тип 0 = ядра
	objISpyGlass.chargeuvarray.uv1 = "0.375,0.5,0.5,0.625"; // LEO ФИКС: тип 1 = КАРТЕЧЬ (ячейка grapes; было ошибочно на ячейке бомбы)
	objISpyGlass.chargeuvarray.uv2 = "0.25,0.5,0.375,0.625"; // тип 2 = книппели
	objISpyGlass.chargeuvarray.uv3 = "0.125,0.5,0.25,0.625"; // LEO ФИКС: тип 3 = БОМБЫ (ячейка bomb; было ошибочно на ячейке картечи) -> форты/бомбы больше не показываются "картечью"

	objISpyGlass.sailuvarray.uv0 = "0.5,0.25,0.625,0.375"; // down
	objISpyGlass.sailuvarray.uv1 = "0.375,0.25,0.5,0.375"; // middle
	objISpyGlass.sailuvarray.uv2 = "0.25,0.25,0.375,0.375"; // up
	Spyglass_SetNewHudParameters();
}

// LEO: Труба использует тот же профиль компаньона; размеры и переключатели остаются в скрипте.
void Spyglass_SetNewHudParameters()
{
	aref owner;
	makearef(owner,objISpyGlass);
	float diameter = RecalculateVIconScaled(84);
	float heightRatio = diameter / 84.0;
	NewHUD_Setup(owner,sti(InterfaceStates.InterfaceSelection),-1,true,false,diameter,heightRatio);
	objISpyGlass.newhud.companion.numbers = NewHUD_GetOption("NewHUDHeroNumbers");
	objISpyGlass.info.cannon.texture = CommandIcons_Texture("List_icons");
	objISpyGlass.info.sailto.texture = CommandIcons_Texture("List_icons");
	objISpyGlass.info.speed.texture = CommandIcons_Texture("LIST_ICON2");
	objISpyGlass.info.charge.texture = CommandIcons_Texture("LIST_ICON2");
	objISpyGlass.info.sailto.uv = "0.75,0.25,0.8125,0.375";
	objISpyGlass.captain.back.texture = "battle_interface\shipbackiconA.dds";
	objISpyGlass.newhud.crewbadge.enabled = 0;

	objISpyGlass.targetlayout.ship.profile = objISpyGlass.newhud.companionprofile;
	objISpyGlass.targetlayout.ship.position = RecalculateHIconScaled(64)+","+(sti(showWindow.bottom)-RecalculateVIconScaled(68));
	objISpyGlass.targetlayout.ship.diameter = diameter;
	objISpyGlass.targetlayout.ship.portraitfullcell = sti(InterfaceStates.InterfaceSelection) == POTC;
	objISpyGlass.targetlayout.fort.profile = "sea-companion-square";
	if(sti(InterfaceStates.InterfaceSelection) == KRUGLYI) objISpyGlass.targetlayout.fort.profile = "sea-companion-round";
	objISpyGlass.targetlayout.fort.position = RecalculateHIconScaled(64)+","+(sti(showWindow.bottom)-RecalculateVIconScaled(68));
	objISpyGlass.targetlayout.fort.diameter = RecalculateVIconScaled(84);
	objISpyGlass.targetlayout.fort.portraitfullcell = 1;
	objISpyGlass.targetlayout.fort.texture = "battle_interface\list_icon2.dds";
	if(sti(InterfaceStates.InterfaceSelection) == KRUGLYI) objISpyGlass.targetlayout.fort.texture = "battle_interface\list_icon2_Konshud.dds";
	objISpyGlass.targetlayout.fort.uvleft = 0.01;
	objISpyGlass.targetlayout.fort.uvtop = 0.01;
	objISpyGlass.targetlayout.fort.uvright = 0.25;
	objISpyGlass.targetlayout.fort.uvbottom = 0.25;
	bool fortTarget = false;
	if(CheckAttribute(&objISpyGlass,"targetlayout.activefort")) fortTarget = sti(objISpyGlass.targetlayout.activefort) != 0;
	int ntop = sti(showWindow.bottom) - RecalculateVIconScaled(114);
	int nationLeft = 116;
	int nationTop = 8;
	if(sti(InterfaceStates.InterfaceSelection) == KRUGLYI) { nationLeft = 133; nationTop = 18; }
	int titleOffset = 0;
	if(NewHUD_GetOption("NewHUDEnabled") != 0)
	{

		objISpyGlass.targetlayout.fort.texture = "battle_interface\NewHUD\Spyglass\FortPortraitSquare.dds";
		if(sti(InterfaceStates.InterfaceSelection) == KRUGLYI) objISpyGlass.targetlayout.fort.texture = "battle_interface\NewHUD\Spyglass\FortPortraitRound.dds";
		objISpyGlass.captain.back.texture = "battle_interface\NewHUD\Spyglass\CaptainFrame.dds";
		objISpyGlass.targetlayout.fort.uvleft = 0.0;
		objISpyGlass.targetlayout.fort.uvtop = 0.0;
		objISpyGlass.targetlayout.fort.uvright = 1.0;
		objISpyGlass.targetlayout.fort.uvbottom = 1.0;

		if(CommandIcons_Enabled())
		{
			objISpyGlass.info.sailto.texture = CommandIcons_Texture("LIST_ICON2");
			objISpyGlass.info.sailto.uv = "0.75,0.875,0.875,1";
		}
		titleOffset = -4;
		if(sti(InterfaceStates.InterfaceSelection) == KRUGLYI) nationLeft = 125;
		else nationLeft = 122;
	}
	objISpyGlass.info.nation.pos = RecalculateHIconScaled(nationLeft)+","+(ntop+RecalculateVIconScaled(nationTop))+","+RecalculateHIconScaled(nationLeft+72)+","+(ntop+RecalculateVIconScaled(nationTop+72));
	objISpyGlass.text.shiptype.pos = RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(-28+titleOffset));
	objISpyGlass.text.shipname.pos = RecalculateHIconScaled(10)+","+(ntop+RecalculateVIconScaled(-15+titleOffset));
	if(sti(InterfaceStates.InterfaceSelection) != KRUGLYI)
	{

		objISpyGlass.newhud.companion.crewcount = 0;
		objISpyGlass.newhud.crewbadge.enabled = NewHUD_GetOption("NewHUDCrewCount");
		objISpyGlass.newhud.crewbadge.texture = "battle_interface\NewHUD\sea-companion-round\Details.dds";
		objISpyGlass.newhud.crewbadge.uv = "0.0078125,0.015625,0.734375,0.6171875";
		int badgeLeft = RecalculateHIconScaled(38);
		int badgeRight = RecalculateHIconScaled(90);
		int badgeTop = sti(showWindow.bottom)-RecalculateVIconScaled(27);
		int badgeBottom = sti(showWindow.bottom)-RecalculateVIconScaled(5);
		objISpyGlass.newhud.crewbadge.pos = badgeLeft+","+badgeTop+","+badgeRight+","+badgeBottom;
		int crewFontSize = makeint(12.0*heightRatio);
		if(crewFontSize < 8) crewFontSize = 8;
		if(crewFontSize > 40) crewFontSize = 40;

		objISpyGlass.targetlayout.crewlabel.centerx = 0.5;
		objISpyGlass.targetlayout.crewlabel.centery = 0.666667;
		int crewX = badgeLeft+makeint((badgeRight-badgeLeft)*stf(objISpyGlass.targetlayout.crewlabel.centerx));
		int crewY = badgeTop+makeint((badgeBottom-badgeTop)*stf(objISpyGlass.targetlayout.crewlabel.centery)-crewFontSize*0.5);
		objISpyGlass.newhud.crewlabel.pos = crewX+","+crewY;
		objISpyGlass.newhud.crewlabel.font = "newhud_numbers_"+crewFontSize;
		objISpyGlass.newhud.crewlabel.align = "center";
		objISpyGlass.newhud.crewlabel.color = argb(255,255,255,255);
		objISpyGlass.newhud.crewlabel.scale = 1.0;
	}
	Spyglass_ApplyTargetLayout(fortTarget);
}

// LEO: Источник и UV зависят от master; выключенный NewHUD оставляет исходный форт LIST_ICON2.
void Spyglass_GetFortIcon(ref texture, ref uvLeft, ref uvTop, ref uvRight, ref uvBottom)
{
	texture = objISpyGlass.targetlayout.fort.texture;
	uvLeft = stf(objISpyGlass.targetlayout.fort.uvleft);
	uvTop = stf(objISpyGlass.targetlayout.fort.uvtop);
	uvRight = stf(objISpyGlass.targetlayout.fort.uvright);
	uvBottom = stf(objISpyGlass.targetlayout.fort.uvbottom);
}

// LEO: Одна точка применения профиля без сообщений: инициализация не пересоздаёт ещё неготовый HUD.
void Spyglass_ApplyTargetLayout(bool fortTarget)
{
	string key = "ship";
	if(fortTarget) key = "fort";
	aref layout;
	makearef(layout,objISpyGlass.targetlayout.(key));
	objISpyGlass.newhud.companionprofile = layout.profile;
	objISpyGlass.newhud.position = layout.position;
	objISpyGlass.newhud.portraitwidth = stf(layout.diameter);
	objISpyGlass.newhud.portraitheight = stf(layout.diameter);
	objISpyGlass.newhud.companion.portraitfullcell = sti(layout.portraitfullcell);
}

// LEO: Проверяем весь профиль, а не один fullcell: у ПКМ и форта он совпадает, но рамки независимы.
void Spyglass_SetTargetLayout(bool fortTarget)
{
	objISpyGlass.targetlayout.activefort = fortTarget;
	string key = "ship";
	if(fortTarget) key = "fort";
	aref layout;
	makearef(layout,objISpyGlass.targetlayout.(key));
	if(objISpyGlass.newhud.companionprofile == layout.profile && objISpyGlass.newhud.position == layout.position &&
		stf(objISpyGlass.newhud.portraitwidth) == stf(layout.diameter) &&
		sti(objISpyGlass.newhud.companion.portraitfullcell) == sti(layout.portraitfullcell)) return;
	Spyglass_ApplyTargetLayout(fortTarget);
	if(IsEntity(&objISpyGlass)) SendMessage(&objISpyGlass,"l",BI_MSG_RELOAD_NEWHUD);
}

// LEO: Общий HUD сообщает об изменениях без зависимости от морских объектов.
void Spyglass_ReloadNewHud()
{
	if(!IsEntity(&objISpyGlass)) return;
	Spyglass_SetNewHudParameters();
	SendMessage(&objISpyGlass,"l",BI_MSG_RELOAD_NEWHUD);

	SendMessage(&objISpyGlass,"lsffff",MSG_ISG_SET_CHARICON,objISpyGlass.captain.back.texture,0.0,0.0,1.0,1.0);

	if(CheckAttribute(&objISpyGlass,"targetlayout.activefort"))
	{
		if(sti(objISpyGlass.targetlayout.activefort) != 0)
		{
			string texture;
			float uvLeft,uvTop,uvRight,uvBottom;
			Spyglass_GetFortIcon(&texture,&uvLeft,&uvTop,&uvRight,&uvBottom);
			SendMessage(&objISpyGlass,"lsffff",MSG_ISG_SET_SHIPICON,texture,uvLeft,uvTop,uvRight,uvBottom);
		}
	}
}

// LEO: Не раскрываем скрытую трубой команду и не восстанавливаем максимум из процентов.
void Spyglass_SetTargetNewHud(ref chref, int knownCrew)
{
	DeleteAttribute(&objISpyGlass, "target");
	objISpyGlass.target.character = sti(chref.index);
	if(knownCrew < 0) return;
	int maxCrew = GetMaxCrewQuantity(chref);
	if(maxCrew <= 0) return;
	objISpyGlass.target.crewfraction = makefloat(knownCrew) / makefloat(maxCrew);
	objISpyGlass.target.woundedfraction = makefloat(GetWoundedQuantity(chref)) / makefloat(maxCrew);
}

void TelescopeInitParameters()
{
	aref arItmScope;
	if( Items_FindItem(GetCharacterEquipByGroup(pchar,SPYGLASS_ITEM_TYPE),&arItmScope)>=0 )
	{	setTelescopeInitParameters(arItmScope);
	}
	else
	{
		object tmpobj;
		setTelescopeInitParameters(&tmpobj);
	}
}

void setTelescopeInitParameters(aref arItmScope)
{
	string texName = "telescope.tga.tx";
	float fZoom = 5.5;
	int activateTime = 700; // LEO: было 500 — чуть дольше переход, чтобы плавный (smoothstep) вход/выход трубы был ощутимее
	int updateTime = 50;

	if( CheckAttribute(arItmScope,"scope.texture") )
	{	texName = arItmScope.scope.texture;
	}
	if( CheckAttribute(arItmScope,"scope.zoom") )
	{	fZoom = stf(arItmScope.scope.zoom);
	}
	if( CheckAttribute(arItmScope,"scope.time_activate") )
	{	activateTime = sti(arItmScope.scope.time_activate);
	}
	if( CheckAttribute(arItmScope,"scope.time_update") )
	{	updateTime = sti(arItmScope.scope.time_update);
	}

	SendMessage(&objISpyGlass, "lsfll", MSG_TELESCOPE_SET_TYPE, texName, fZoom, activateTime, updateTime);
}
