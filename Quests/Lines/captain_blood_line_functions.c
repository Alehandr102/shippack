// Gregg: вынесено из reaction_functions.c (сплит god-файла) — секция «Линейка капитана Блада» (главный сюжет, линия капитана Блада).
// #include на исходное место — видимость и поведение идентичны.

void Blood_StartGame(string qName)
{
    ref sld;
	Pchar.questTemp.CapBloodLine = true;
    Pchar.questTemp.CapBloodLine.stat = "Begining";
	pchar.Cirgnore = true; //атрибут, блокирующий смену модели кирасной логикой
    Pchar.BaseNation = ENGLAND;
    //fix перков флагов
    DeleteAttribute(Pchar, "perks.list.FlagFra");
    DeleteAttribute(Pchar, "perks.list.FlagSpa");
    DeleteAttribute(Pchar, "perks.list.FlagHol");
    SetCharacterPerk(Pchar, "FlagEng");
    Flag_Change(ENGLAND);
    //Подготовка Барбадоса
    int n = FindLocation("Bridgetown_town");
    DeleteAttribute(&locations[n], "questSeekCap");
    DeleteAttribute(&locations[n], "houseEnc");
    locations[n].notCloseCommonHouse = true;
    n = FindLocation("Bridgetown_tavern"); //homo fix
    DeleteAttribute(&locations[n], "habitues");
	colonies[FindColony("Bridgetown")].DontSetShipInPort = true;
    //Обираем до нитки
    RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
    RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
    RemoveCharacterEquip(pchar, SPYGLASS_ITEM_TYPE);
    RemoveCharacterEquip(pchar, PATENT_ITEM_TYPE);
	RemoveCharacterEquip(pchar, CIRASS_ITEM_TYPE);
	RemoveCharacterEquip(pchar, MAPS_ITEM_TYPE);
    DeleteAttribute(Pchar, "items");

    DeleteAttribute(Pchar, "ship");
    DeleteAttribute(Pchar, "ShipSails.gerald_name");
    Pchar.ship.type = SHIP_NOTUSED;

    TakeNItems(Pchar, "potion3", 1);
    TakeNItems(Pchar, "potion2", 3);
    TakeNItems(Pchar, "potion1", 4);
    SetCharacterPerk(Pchar, "Doctor1");

	//Эндрю Бэйнс
    sld = GetCharacter(NPC_GenerateCharacter("Beyns", "trader_8", "man", "man", 7, ENGLAND, 1, false));
    sld.name = GetConvertStrWithReplace("Variable_reaction_functions_153", "Names.txt", "#space#", " ");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_154", "Names.txt", "#space#", " ");
	sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
	sld.dialog.currentnode = "First time";
	sld.greeting = "cit_common";
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
	LAi_group_MoveCharacter(sld, sTemp);
    LAi_SetStayTypeNoGroup(sld);
   	LAi_SetImmortal(sld, true);
	sld.talker = 10;
	ChangeCharacterAddressGroup(sld, "Estate", "goto", "goto12");

    //==============//Джереми Питт//==============
   	sld = GetCharacter(NPC_GenerateCharacter("Pitt", "PBLine_Pitt_0", "man", "man", 1, ENGLAND, -1, false)); //TO_DO анимацию вернуть по готовности модельки
    sld.name 	= GetConvertStr("Variable_reaction_functions_0.8", "Names.txt");
    sld.lastname 	= GetConvertStr("Variable_reaction_functions_0.8.1", "Names.txt");
    sld.Dialog.CurrentNode = "First time";
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.greeting = "Gr_YoungMan";
//                  P I R A T  E S
    SetSPECIAL(sld, 5,9,7,5,10,7,4);
    sld.rank = 12;
    sld.OfficerWantToGo.DontGo = true; //не пытаться уйти
	sld.HalfImmortal = true;  // Контузия
	sld.HeroModel = "PBLine_Pitt_0,PBLine_Pitt_1,PBLine_Pitt_2,PBLine_Pitt_3,PBLine_Pitt_4,PBLine_Pitt_5";
	sld.loyality = MAX_LOYALITY;
    InitStartParam(sld);

    SetSelfSkill(sld, 40, 35, 20, 40, 25);
    //int _ld, int _cr, int _ac, int _cn, int _sl, int _re, int _gr, int _de, int _sn
    SetShipSkill(sld, 40, 30, 25, 25, 50, 25, 25, 25, 30);

	SetCharacterPerk(sld, "ShipSpeedUp");
	SetCharacterPerk(sld, "ShipTurnRateUp");
	SetCharacterPerk(sld, "StormProfessional");
	SetCharacterPerk(sld, "WindCatcher");
	SetCharacterPerk(sld, "SailsMan");

    SetFantomHP(sld);
    SetEnergyToCharacter(sld);
   	LAi_SetImmortal(sld, true);
    LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", 2.0, 0);
	ChangeCharacterAddressGroup(sld, "Estate", "goto", "goto1");

	pchar.quest.CureLordMovie.win_condition.l1 = "locator";
	pchar.quest.CureLordMovie.win_condition.l1.location = "EstateBadroom1";
	pchar.quest.CureLordMovie.win_condition.l1.locator_group = "quest";
	pchar.quest.CureLordMovie.win_condition.l1.locator = "quest1";
	pchar.quest.CureLordMovie.function = "CureLord";

	if (bQuestsRank)	//Если стоит галочка, то убрать требования рангов у квестов
	{
		DoQuestFunctionDelay("UbratTrebovanieRangov", 0.1);
	}
}

void CureLord(string qName)
{
    ref sld;
    sld = &characters[GetCharacterIndex("Beyns")];
    ChangeCharacterAddressGroup(sld, "EstateBadroom1", "reload", "reload1");
    sld.dialog.currentnode = "EBStep_1";
    LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "",  -1.0, 0);

}

void CureLordMovie()
{
    InterfaceStates.Buttons.Save.enable = false;
	StartQuestMovie(true, true, true);
	ChangeShowIntarface();
    LAi_SetActorTypeNoGroup(Pchar);
    LAi_ActorGoToLocator(Pchar, "goto", "goto6", "CureLordMovie_Con1", 10.0);
}

void CureLordMovie_Con1()
{
    LAi_ActorTurnToLocator(Pchar, "goto", "goto7");
	LAi_ActorAnimation(Pchar, "Barman_idle", "CureLordMovie_Con2", 5);
}

void CureLordMovie_Con2()
{
    LAi_Fade("", "");
	DoQuestCheckDelay("CureLordMovie_Con3", 0.5);
}

void CureLordMovie_Con3()
{

    ref sld;
    sld = &characters[GetCharacterIndex("Beyns")];
    sld.dialog.currentnode = "EBStep_2";

    ChangeCharacterAddressGroup(sld, "EstateOffice", "goto", "goto6");
    LAi_SetStayTypeNoGroup(sld);
    DoReloadCharacterToLocation("EstateOffice", "goto", "goto3");
    EndQuestMovie();
   	ChangeShowIntarface();
    LAi_SetPlayerType(Pchar);
    InterfaceStates.Buttons.Save.enable = true;

}

void BloodGetBlade(string qName)
{
    LAi_SetCheckMinHP(Pchar, 1, true, "CapBlood_CheckMinHP");
   	LAi_SetImmortal(pchar, false);
}

void DragunInvansion()
{
    ref sld;
    sld = &characters[GetCharacterIndex("Pitt")];
    sld.dialog.currentnode = "PStep_5";
    ChangeCharacterAddressGroup(sld, "EstateOffice", "reload", "reload1");
    LAi_SetActorTypeNoGroup(sld);
    LAi_ActorDialog(sld, pchar, "",  -1, 0);
}

void DragunInvansion2()
{
    pchar.quest.BloodGetBlade.win_condition.l1 = "item";
    pchar.quest.BloodGetBlade.win_condition.l1.item= "pistol1";
    pchar.quest.BloodGetBlade.function = "BloodGetBlade";
    pchar.quest.DragunInvansion.win_condition.l1          = "location";
    pchar.quest.DragunInvansion.win_condition.l1.location = "Estate";
    pchar.quest.DragunInvansion.function                  = "_DragunInvansion3";

}

void _DragunInvansion3(string qName)
{
    chrDisableReloadToLocation = true; // закрыть выход из локации
    DoQuestFunctionDelay("DragunInvansion3", 60.0);
}

void DragunInvansion3(string qName)
{
    ref ch, sld;
    LAi_LocationFightDisable(loadedLocation, true);
    LAi_SetFightMode(Pchar, false);
    LAi_LockFightMode(pchar, true);
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
    LAi_group_SetHearRadius("TmpEnemy", 3000.0); //fix

	ch = GetCharacter(NPC_GenerateCharacter("CapGobart", "Hobart", "man", "man", 10, ENGLAND, 0, false));
    ch.name 	= GetConvertStr("Variable_reaction_functions_0.9", "Names.txt");
    ch.lastname 	= GetConvertStr("Variable_reaction_functions_0.9.1", "Names.txt");
    ch.Dialog.CurrentNode = "First time";
    ch.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    ch.greeting = "soldier_arest";
    ch.location = "none";
    FantomMakeCoolFighter(ch, 25, 70, 70, "blade22", "pistol2", 20);
    LAi_SetImmortal(ch, true);
    ch.BreakTmplAndFightGroup = true; //fix
    LAi_group_MoveCharacter(ch, "TmpEnemy");

    ChangeCharacterAddressGroup(ch, "Estate", "goto", "goto1");
    LAi_SetActorTypeNoGroup(ch);
	LAi_ActorDialog(ch, pchar, "", 7, 0);

    for (int i=0 ; i<=4; i++)
    {
    	// генерим c сопровождение
		sTemp = "SoldierEnglish_1";
		if (i == 1) sTemp = "Dragoon_1";
        sld = GetCharacter(NPC_GenerateCharacter(("Dragun_"+i), sTemp, "man", "man", 2, ENGLAND, 0, false));
        sld.location = "none";
        sld.BreakTmplAndFightGroup = true;
        SetSPECIAL(sld, 8,10,9,3,6,10,4);
        InitStartParam(sld);
        SetEnergyToCharacter(sld);
        LAi_SetImmortal(sld, true);
        GiveItem2Character(sld, "blade4");
    	EquipCharacterByItem(sld, "blade4");
	    sld.Dialog.CurrentNode = "Draguns_0";
        sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
        LAi_group_MoveCharacter(sld, "TmpEnemy");

    	ChangeCharacterAddressGroup(sld, "Estate", "reload", "reload1");
        LAi_SetActorTypeNoGroup(sld);
    	if(i < 3)
    	{
        	if( i > 1 )
        	{
            	GiveItem2Character(sld, "pistol2");
            	EquipCharacterByItem(sld, "pistol2");
            	TakeNItems(sld, "bullet", 30);
				AddItems(sld, "GunPowder", 30);
            }
            LAi_ActorFollow(sld, ch, "", -1);
        }
        if(i == 3)LAi_ActorRunToLocation(sld, "reload", "reload3", "none", "", "", "", 20);
        if(i == 4)LAi_ActorRunToLocation(sld, "reload", "reload5", "none", "", "", "", 20);
     }

}

void DragunInvansion4()
{
 	StartQuestMovie(true, true, true);
	locCameraToPos(18.241, 13, 0.35, false);
	ref ch, sld;
    LAi_LockFightMode(pchar, false);
    LAi_LocationFightDisable(loadedLocation, false);
    ch = &characters[GetCharacterIndex("Pitt")];
    ChangeCharacterAddressGroup(ch, "Estate", "reload", "reload5");
    LAi_SetActorType(ch);
    LAi_ActorGoToLocator(ch, "goto", "goto5", "", -1);

    sld = &characters[GetCharacterIndex("Beyns")];
    ChangeCharacterAddressGroup(sld, "Estate", "reload", "reload5");
    LAi_SetActorType(sld);
    LAi_ActorFollow(sld, ch, "", -1);

    for (i=1; i<5; i++)
    {
        sld = &characters[GetCharacterIndex("Dragun_"+i)];

        if(i == 3 || i == 4)
        {
            ChangeCharacterAddressGroup(sld, "Estate", "reload", "reload5");
            LAi_SetActorTypeNoGroup(sld);
            LAi_ActorFollow(sld, ch, "", -1);
        }
        else
        {

        }
    }

}

void CapGobartAttack(string qName)
{
    ref sld;
    int numdead = 0;
    for (i=1; i<=2; i++)
    {
        sld = characterFromID("Dragun_"+i);
        if (CharacterIsDead(sld))
        {
            numdead++;
        }
    }
    if(numdead == 2)
    {
        sld = characterFromID("CapGobart");
        LAi_SetWarriorTypeNoGroup(sld);
        LAi_group_Attack(sld, Pchar);
    }
}
/////////////////////////////////////////////////////////////////////////
void Blood_StartGame_End(string qName)
{
    ref sld;
    chrDisableReloadToLocation = true;
    InterfaceStates.Launched = false;
    InterfaceStates.Buttons.Save.enable = true;
    sld = characterFromID("Dragun_0");
    LAi_RemoveCheckMinHP(sld);
    pchar.quest.CapGobartAttack_1.over = "yes";
    pchar.quest.CapGobartAttack_2.over = "yes";

	CreateEntity(&LAi_QuestFader, "fader");
	SendMessage(&LAi_QuestFader, "lfl", FADER_OUT, 0.5, false);
	LAi_QuestFader.oldSaveState = InterfaceStates.Buttons.Save.enable;
	InterfaceStates.Buttons.Save.enable = 0;

    Pchar.model="PGG_Blad_6";
	DoQuestFunctionDelay("CapBloodLineInit", 0.1);
}

void CapBloodLineInit(string qName)
{
    //Возвращаем все после плена
    LAi_SetCurHPMax(Pchar);
    LAi_SetPlayerType(Pchar);
    //Рабство
    LAi_SetImmortal(Pchar, false);
    DeleteAttribute(Pchar, "items");
    DeleteAttribute(Pchar, "equip");

    DeleteAttribute(Pchar, "ship");
    DeleteAttribute(Pchar, "ShipSails.gerald_name");
    Pchar.ship.type = SHIP_NOTUSED;
    Pchar.money = 0;
    // Cтид
    sld = &characters[GetCharacterIndex("Bridgetown_Mayor")];
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.name = GetConvertStr("Variable_reaction_functions_155", "Names.txt");
    sld.lastname = GetConvertStr("Variable_reaction_functions_155.1", "Names.txt");
    // Первый квест
	Environment.date.month = 4;
    SetCurrentTime(23, 1);
    pchar.quest.onPlantation.win_condition.l1          = "location";
    pchar.quest.onPlantation.win_condition.l1.location = "BridgeTown_Plantation";
    pchar.quest.onPlantation.function                  = "CapBloodLine_q1";

    int n= FindLocation("Bridgetown_town");  // не забыть вернуть
    locations[n].reload.l3.close_for_night = false;
    //для энкаунтеров в джанглах
    bDisableLandEncounters = true;

	PostVideoAndQuest("slave", 100, "move_slave_2");
}

void CapBloodLine_firstEnterHome(string qName)
{
	sld = characterFromID("Pitt");
	sld.Dialog.CurrentNode = "PStep_7";
	sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
	LAi_group_MoveCharacter(sld, sTemp);
	ChangeCharacterAddressGroup(sld, "Plantation_S1", "reload", "reload1");
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", 3, 0);
	Pchar.questTemp.CapBloodLine.statcrew = "";
}

void CapBloodLine_q1(string qName)
{
    ref sld;
    LAi_LocationFightDisable(loadedLocation, true);
    LAi_SetFightMode(Pchar, false);
    LAi_LockFightMode(pchar, true);
    chrDisableReloadToLocation = true; // закрыть выход из локации
    bDisableFastReload = true; // закрыть переходы.
    Pchar.questTemp.CapBUnarmed = true; //ГГ сабля не положена

    //Бишоп на плантациях
   	sld = GetCharacter(NPC_GenerateCharacter("Bishop", "PBLine_Bishop", "man", "man", 1, ENGLAND, 3, false));
    sld.name 	= GetConvertStr("Variable_reaction_functions_0.10", "Names.txt");
    sld.lastname 	= GetConvertStr("Variable_reaction_functions_0.10.1", "Names.txt");
    sld.Dialog.CurrentNode = "First Bishop";
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.greeting = "Gr_Bishop";

    SetSPECIAL(sld, 9,8,10,3,6,10,4);
    InitStartParam(sld);
    SetEnergyToCharacter(sld);

    GiveItem2Character(sld, "blade22");
	EquipCharacterByItem(sld, "blade22");
	GiveItem2Character(sld, "pistol2");
	EquipCharacterByItem(sld, "pistol2");
	LAi_SetImmortal(sld, true);

    ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "reload", "houseSp1");
    LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);

    //Джереми Питт
    sld = characterFromID("Pitt");
    sld.Dialog.CurrentNode = "First time";
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    LAi_SetCitizenType(sld);
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
	LAi_group_MoveCharacter(sld, sTemp);
    ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "officers", "houseS1_3");

    //Оружейник
	sld = GetCharacter(NPC_GenerateCharacter("Griffin", "shipowner_11", "man", "man", 10, ENGLAND, 3, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_156", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_157", "Names.txt", "#space#", " ");
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.greeting = "Gr_Master";
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
    LAi_SetOwnerType(sld);
    LAi_group_MoveCharacter(sld, sTemp);
	ChangeCharacterAddressGroup(sld, "CommonFlamHouse", "goto","goto1");

	LAi_SetImmortal(sld, true);
    //Рыбак
	sld = GetCharacter(NPC_GenerateCharacter("Hells", "shipowner_9", "man", "man", 10, PIRATE, 3, false));
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_158", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_159", "Names.txt", "#space#", " ");
    sld.SaveItemsForDead = true; // сохранять на трупе вещи
    sld.DontClearDead = true;
	sld.greeting = "Gr_Fisher";
	AddMoneyToCharacter(sld, 2000);
	FantomMakeCoolFighter(sld, 7, 25, 50, "topor1", "", 10);
    LAi_SetOwnerTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, "CommonRoom_MH2", "goto","goto4");

	//Контра
	sld = GetCharacter(NPC_GenerateCharacter("Quest_Smuggler", "Pirate_4", "man", "man", 10, ENGLAND, 3, false));
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.greeting = "Gr_Smuggler Agent";
	LAi_SetSitTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, "Bridgetown_tavern", "sit","sit2");

    //алкаши в таверне
    int i=rand(2)
	sld = GetCharacter(NPC_GenerateCharacter("Quest_Habitue", "citiz_"+(rand(11)+1), "man", "man", 10, ENGLAND, 3, false));
	sld.greeting = "Gr_Tavern_Mate";
    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
    sld.Default           = "Bridgetown_tavern";
    sld.Default.group     = "sit";
    sld.Default.ToLocator = "sit_front" + (i+1);
    LAi_SetCitizenType(sld);
    LAi_SetSitTypeNoGroup(sld);
    ChangeCharacterAddressGroup(sld, "Bridgetown_tavern", "sit","sit_base"+(1+i));

    //==============//Волверстон//==============
   	sld = GetCharacter(NPC_GenerateCharacter("Volverston", "PBLine_Volverston_0", "man", "man", 12, ENGLAND, -1, false));
    sld.name 	= GetConvertStr("Variable_reaction_functions_0.11", "Names.txt");
    sld.lastname 	= "";
    sld.Dialog.CurrentNode = "First time";
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.greeting = "Gr_questOfficer";
   	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
    LAi_group_MoveCharacter(sld, sTemp);
    LAi_SetCitizenType(sld);
    SetRandSPECIAL(sld);
    InitStartParam(sld);
    sld.rank = 10;
	sld.OfficerWantToGo.DontGo = true; //не пытаться уйти
	sld.HalfImmortal = true;  // Контузия
	sld.loyality = MAX_LOYALITY;
	sld.HeroModel = "PBLine_Volverston_0,PBLine_Volverston_1,PBLine_Volverston_2,PBLine_Volverston_3,PBLine_Volverston_4,PBLine_Volverston_5";
//                     P  I  E  A  T  R  S
	SetSPECIAL(sld, 10, 7, 9, 8, 8, 4, 8);

    SetSelfSkill(sld, 20, 30, 45, 30, 40);//
    //int            ld,  cr, ac, cn, sl, re, gr, de, sn
    SetShipSkill(sld, 45, 30, 15, 15, 10, 40, 45, 30, 20);
   	SetCharacterPerk(sld, "AdvancedDefense");
	SetCharacterPerk(sld, "CriticalHit");

	SetCharacterPerk(sld, "Rush");
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "IronWill");
	SetCharacterPerk(sld, "LongRangeGrappling");
	SetCharacterPerk(sld, "GrapplingProfessional");
    SetEnergyToCharacter(sld);
    SetFantomHP(sld);
   	LAi_SetImmortal(sld, true);
	ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "officers", "houseF2_1");

    //==============//Натаниэль   Хагторп//==============
   	sld = GetCharacter(NPC_GenerateCharacter("Hugtorp", "PBLine_Hagtorp_0", "man", "man", 10, ENGLAND, -1, false));
    sld.name 	= GetConvertStr("Variable_reaction_functions_0.12", "Names.txt");
    sld.lastname 	= GetConvertStr("Variable_reaction_functions_0.12.1", "Names.txt");
    sld.Dialog.CurrentNode = "First time";
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.greeting = "Gr_Officer";
   	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
    LAi_group_MoveCharacter(sld, sTemp);
    LAi_SetCitizenType(sld);
    SetRandSPECIAL(sld);
    InitStartParam(sld);
    sld.rank = 10;
	sld.OfficerWantToGo.DontGo = true; //не пытаться уйти
	sld.HalfImmortal = true;  // Контузия
	sld.HeroModel = "PBLine_Hagtorp_0,PBLine_Hagtorp_1,PBLine_Hagtorp_2,PBLine_Hagtorp_3,PBLine_Hagtorp_4,PBLine_Hagtorp_5";
	sld.loyality = MAX_LOYALITY;
//                    P  I  E  A  T  R  S
	SetSPECIAL(sld, 8, 9, 8, 8, 9, 8, 7);
    //int _fl, int _f, int _fh, int _p, int _fr
    SetSelfSkill(sld, 20, 40, 35, 30, 30);
    //int _ld, int _cr,  ac, cn, sl, re, gr, de, sn
    SetShipSkill(sld, 45, 30, 25, 25, 20, 25, 35, 30, 20);
   	SetCharacterPerk(sld, "LongRangeGrappling");
	SetCharacterPerk(sld, "Brander");
	SetCharacterPerk(sld, "Troopers");
	SetCharacterPerk(sld, "AdvancedDefense");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "Gunman");
    SetEnergyToCharacter(sld);
    SetFantomHP(sld);
   	LAi_SetImmortal(sld, true);
   	ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "officers", "houseS2_2");

    //==============//Николас Дайк//==============
   	sld = GetCharacter(NPC_GenerateCharacter("Dieke", "PBLine_Daik_0", "man", "man", 10, PIRATE, -1, false));
    sld.name 	= GetConvertStr("Variable_reaction_functions_0.13", "Names.txt");
    sld.lastname 	= GetConvertStr("Variable_reaction_functions_0.13.1", "Names.txt");
    sld.Dialog.CurrentNode = "First time";
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.greeting = "Gr_Officer";
   	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
    LAi_group_MoveCharacter(sld, sTemp);
    LAi_SetCitizenType(sld);
    SetRandSPECIAL(sld);
    InitStartParam(sld);
    sld.rank = 10;
	sld.OfficerWantToGo.DontGo = true; //не пытаться уйти
	sld.HalfImmortal = true;  // Контузия
	sld.HeroModel = "PBLine_Daik_0,PBLine_Daik_1,PBLine_Daik_2,PBLine_Daik_3,PBLine_Daik_4,PBLine_Daik_5";
	sld.loyality = MAX_LOYALITY;
//                    P  I  E  A  T  R  S
	SetSPECIAL(sld, 6, 8, 7, 5, 9, 9, 7);
    //int _fl, int _f, int _fh, int _p, int _fr
    SetSelfSkill(sld, 40, 35, 20, 30, 30);
    //                ld, cr, ac, cn, sl, re, gr, de, sn
    SetShipSkill(sld, 30, 35, 20, 20, 20, 45, 25, 35, 20);
   	SetCharacterPerk(sld, "BasicCommerce");
	SetCharacterPerk(sld, "LightRepair");
	SetCharacterPerk(sld, "Carpenter");
	SetCharacterPerk(sld, "BasicDefense");
	SetCharacterPerk(sld, "Sliding");
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "EnergyPlus");
    SetEnergyToCharacter(sld);
    SetFantomHP(sld);
   	LAi_SetImmortal(sld, true);
	ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "officers", "houseF1_1");

    //==============//Нед Огл//==============
   	sld = GetCharacter(NPC_GenerateCharacter("Ogl", "PBLine_NedOgl_0", "man", "man", 10, ENGLAND, -1, false));
    sld.name 	= GetConvertStr("Variable_reaction_functions_0.14", "Names.txt");
    sld.lastname 	= GetConvertStr("Variable_reaction_functions_0.14.1", "Names.txt");
    sld.Dialog.CurrentNode = "First time";
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.greeting = "Gr_Ogl";
   	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
    LAi_group_MoveCharacter(sld, sTemp);
    LAi_SetCitizenType(sld);
    LAi_SetStayTypeNoGroup(sld);
    SetRandSPECIAL(sld);
    InitStartParam(sld);
    sld.rank = 10;
    sld.OfficerWantToGo.DontGo = true; //не пытаться уйти
	sld.HalfImmortal = true;  // Контузия
	sld.loyality = MAX_LOYALITY;
	sld.HeroModel = "PBLine_NedOgl_0,PBLine_NedOgl_1,PBLine_NedOgl_2,PBLine_NedOgl_3,PBLine_NedOgl_4,PBLine_NedOgl_5";
	SetSPECIAL(sld, 8, 9, 5, 5, 9, 5, 7);
    //int           _fl,  f,  fh, p,  fr
    SetSelfSkill(sld, 20, 30, 20, 35, 30);
    //int            ld,  cr, ac, cn, sl, re, gr, de, sn
    SetShipSkill(sld, 20, 10, 50, 40, 15, 20, 20, 20, 20);
   	SetCharacterPerk(sld, "FastReload");
	SetCharacterPerk(sld, "HullDamageUp");
	SetCharacterPerk(sld, "CrewDamageUp");
	SetCharacterPerk(sld, "SailsDamageUp");
	SetCharacterPerk(sld, "LongRangeShoot");
	SetCharacterPerk(sld, "BasicDefense");
	SetCharacterPerk(sld, "GunProfessional");
	SetCharacterPerk(sld, "SharedExperience");
    SetEnergyToCharacter(sld);
    SetFantomHP(sld);
   	LAi_SetImmortal(sld, true);
	ChangeCharacterAddressGroup(sld, "Plantation_S2", "barmen", "stay");

    //==============//Кинжал для Блада//==============
    sld = GetCharacter(NPC_GenerateCharacter("Slave_Quest", "shipowner_6", "man", "man", 7, ENGLAND, 2, false));
    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
    sld.Dialog.CurrentNode = "SLQStep_0";
    sld.greeting = "Gr_slave";
    sld.talker = 10;
    sld.CityType = "citizen";
	sld.city = "BridgeTown";
	LAi_SetLoginTime(sld, 6.0, 22.99);
    ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "goto", "goto12");
	LAi_SetCitizenType(sld);
    LAi_group_MoveCharacter(sld, sTemp);

    //раскидаем квестовых нпс по локациям
	SetQuestsCharacters();

    // первый заход в город
    pchar.quest.CapBloodLine_q1_1.win_condition.l1          = "location";
    pchar.quest.CapBloodLine_q1_1.win_condition.l1.location = "Bridgetown_town";
    pchar.quest.CapBloodLine_q1_1.function                  = "_PrepareBridgeTown";
}

void ChangePIRATES()
{
	pchar.SystemInfo.ChangePIRATES = true;
	LaunchCharacter(pchar);
}

void _PrepareBridgeTown(string qName)
{
    DoQuestFunctionDelay("PrepareBridgeTown", 1.0);
}
void PrepareBridgeTown(string qName)
{
    trace("PrepareBridgeTown");
    //меняем город под рабство
    ChangeBridgeTownDialogs(false);
    //солдат на страже у резиденции
    ref sld = &characters[GetCharacterbyLocation("Bridgetown_town", "soldiers", "soldier2")];
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.Dialog.CurrentNode = "First Guard";
    sld.protector = true;
    sld.protector.CheckAlways = 1 ;
    LAi_RemoveLoginTime(sld);
    LoginCharacter(sld, "BridgeTown_town");
    //Нэтталл
    LoginNettl("");
    //закроем резиденцию до поры
    LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", true); //fix
    LocatorReloadEnterDisable("BridgeTown_fort", "reload2", true);
    sld = &characters[GetCharacterIndex("Bridgetown_Smuggler")]
    LoginCharacter(sld, "Bridgetown_tavern");

}

void DetectBlades()
{
    if (CheckAttribute(Pchar, "questTemp.CapBloodLine") && Pchar.questTemp.CapBloodLine == true && Pchar.questTemp.CapBUnarmed == true)
    {
        string sweapon = FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE);

        bool bblades = ( sweapon != "unarmed") && !HasSubStr(sweapon, "blade5") && (sweapon!= "");
        bool bguns = (FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE) != "");
        if(bblades || bguns)
        {
            pchar.quest.CapBloodDetectBlades1.win_condition.l1          = "location";
            pchar.quest.CapBloodDetectBlades1.win_condition.l1.location = "Bridgetown_town";
            pchar.quest.CapBloodDetectBlades1.function                  = "TakeBlades";

            pchar.quest.CapBloodDetectBlades2.win_condition.l1          = "location";
            pchar.quest.CapBloodDetectBlades2.win_condition.l1.location = "BridgeTown_Plantation";
            pchar.quest.CapBloodDetectBlades2.function                  = "TakeBlades";
        }
        if(sweapon == "")
        {

        }
        else
        {
            EquipCharacterByItem(Pchar,GetCharacterEquipByGroup(Pchar, BLADE_ITEM_TYPE));
        }
    }
}

void TakeBlades(string qName)
{
    sld = GetCharacter(NPC_GenerateCharacter("SolderTakeBlades", "sold_eng_"+(rand(7)+1), "man", "man", 10, ENGLAND, 0, false));
	sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
	sld.dialog.currentnode = "First time";
	sld.city = "Bridgetown_town";
    sld.greeting = "soldier_arest";
    sld.location = "none";
    sld.BreakTmplAndFight = true;

    SetFantomParamFromRank(sld, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE, true);
	LAi_SetCitizenType(sld);
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
	LAi_group_MoveCharacter(sld, sTemp);
    PlaceCharacter(sld, "goto", "random_must_be_near"))
    LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "",  -1.0, 0);
	chrDisableReloadToLocation = true;
    pchar.quest.SolderTakeBladesDie.win_condition.l1 = "NPC_death";
    pchar.quest.SolderTakeBladesDie.win_condition.l1.character = "SolderTakeBlades";
   	pchar.quest.SolderTakeBladesDie.function = "SolderTakeBladesDie";

}

void SolderTakeBladesDie(string qName)
{
    chrDisableReloadToLocation = false;
}

//Меняет всем дилоги
void ChangeBridgeTownDialogs(bool restore)
{
    ref sld;
    int num, cur;
    num = 0;
    cur = rand(3)+1;
    for(int i=0;i<MAX_CHARACTERS;i++)
	{
        sld = &characters[i];
        //солдаты
        if (CheckAttribute(sld, "city") && sld.city == "Bridgetown")
        {
            if(CheckAttribute(sld, "CityType") && sld.CityType == "soldier")
            {
                if (!restore)
                {
                    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
                }
                else
                {
                    sld.dialog.filename = "Common_Soldier.c";
                }
                sld.lifeDay = 3;
            }
            //горожане
            if(CheckAttribute(sld, "CityType") && sld.CityType == "citizen")
            {
                if (!restore)
                {
                    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
                    if(sld.location.group == "merchant")
                    {   //торговцы
                        num++;
                        if(num == cur) // для квеста по добычи оружия метим одного продовца
                        {
                            sld.quest.bGoodMerch = true;
                        }

                    }
                }
                else
                {
                    if(sld.location.group == "merchant")
                    {   //торговцы
                        sld.dialog.filename = "Common_ItemTrader.c";
                    }
                    else
                    {   //обычные
                        sld.dialog.filename = "Common_citizen.c";
                    }
                }
                sld.lifeDay = 3;
            }
            //нищие
            if(sld.id == "Bridgetown_Poorman")
            {
                if (!restore)
                {
                    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
                }
                else
                {
                    sld.dialog.filename = "Common_poorman.c";
                }
            }

            //квестовые
            if(sld.id == "QuestCitiz_Bridgetown")
            {
                if (!restore)
                {
                    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
                    sld.Dialog.CurrentNode = "First time";
                }
                else
                {
                    sld.dialog.filename = "Quest\ForAll_dialog.c";
                }
            }
        }

	}
}

void LoginNettl(string qName)
{

        Pchar.questTemp.CapBloodLine.TalkWithNettl = false;          //10//3
		sld = GetCharacter(NPC_GenerateCharacter("Nettl", "shipowner_2", "man", "man", 7, ENGLAND, 3, false));
        sld.name = GetConvertStrWithReplace("Variable_reaction_functions_160", "Names.txt", "#space#", " ");
        sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_161", "Names.txt", "#space#", " ");
		sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
		sld.dialog.currentnode = "First time";
		sld.city = "Bridgetown_town";
		sld.greeting = "Gr_Tavern_Mate";

		LAi_SetCitizenType(sld);
		sTemp = GetNationNameByType(ENGLAND) + "_citizens";
		LAi_group_MoveCharacter(sld, sTemp);
		sld.talker = 10;
		PlaceCharacter(sld, "goto", "random");
}

void CapBloodLine_q1_Late(string qName)
{
    InterfaceStates.Buttons.Save.enable = false;

    GameOver("blood");
}

void CapBloodLine_q1_End(string qName)
{
   AddQuestRecord("CapBloodLine_q1", "6");
   Pchar.questTemp.CapBloodLine.stat = "CureMisStid_Complite";
}

void CapBloodLine_q1_Complited(string qName)
{
    ref sld
    trace("CapBloodLine_q1_Complited!!!");
    pchar.quest.CapBloodLineTimer_1.over = "yes";
    sld = &characters[GetCharacterIndex("MisStid")];

    sld.location = "none";
    sld = &characters[GetCharacterIndex("Bridgetown_Mayor")];
    sld.Dialog.CurrentNode = "First time";

    int n= FindLocation("Bridgetown_town");
    locations[n].reload.l3.close_for_night = true;//закрываем резиденцию
    AddQuestRecord("CapBloodLine_q1", "7");
    CloseQuestHeader("CapBloodLine_q1");

    bQuestCheckProcessFreeze = true;
    WaitDate("",0,0,0, 12, 0); // прошло время
    SetCurrentTime(7, 0); // должно быть 2-е число 7 часов
    bQuestCheckProcessFreeze = false;
    SetLaunchFrameFormParam(GetConvertStr("Variable_reaction_functions_3", "Frames.txt"), "Reload_To_Location", 0.1, 2.0);
    SetLaunchFrameReloadLocationParam("Bridgetown_tavern", "reload","reload2", "");
	LaunchFrameForm();
	CapBloodLine_q2();
}

void CapBloodLine_q2()
{
    ref sld;
    sld = &characters[GetCharacterIndex("Waker")];
    sld.dialog.currentnode = "First time";
    Pchar.questTemp.CapBloodLine.stat = "WakerOffer";
    ChangeCharacterAddressGroup(sld, "Bridgetown_tavern_upstairs", "goto","goto1");
    LAi_SetActorTypeNoGroup(sld);
    LAi_ActorDialog(sld, pchar, "", 2.0, 0);
    AddQuestRecord("CapBloodLine_q2", "2");
    AddQuestRecord("CapBloodLine_q2", "3");

    //сроки 10 часов
    PChar.quest.CapBloodLineTimer_2.win_condition.l1            = "Timer";
    PChar.quest.CapBloodLineTimer_2.win_condition.l1.date.hour  = 23;
    PChar.quest.CapBloodLineTimer_2.win_condition.l1.date.day   = GetAddingDataDay(0, 0, 0);
    PChar.quest.CapBloodLineTimer_2.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 0);
    PChar.quest.CapBloodLineTimer_2.win_condition.l1.date.year  = GetAddingDataYear(0, 0, 0);
    PChar.quest.CapBloodLineTimer_2.function = "CapBloodLine_q2_Late";

}

void Spain_spyDie(string qName)
{
    ref sld;
    sld = &characters[GetCharacterIndex("Spain_spy")];
    LAi_KillCharacter(sld);
}

void LoginSpain_spy(string qName)
{
    ref sld;
    sld = GetCharacter(NPC_GenerateCharacter("Spain_spy", "shipowner_13", "man", "man", 7, SPAIN, 2, false));
    sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
    sld.name = GetConvertStrWithReplace("Variable_reaction_functions_162", "Names.txt", "#space#", " ");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_163", "Names.txt", "#space#", " ");
	sld.dialog.currentnode = "First time";
	sld.city = "Bridgetown_town";
    sld.greeting = "Gr_HUNTER";
    sld.location = "none";

    FantomMakeCoolFighter(sld, 7, 30, 50, "blade6", "", 20);
	LAi_SetCitizenType(sld);
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
	LAi_group_MoveCharacter(sld, sTemp);
    PlaceCharacter(sld, "goto", "random_must_be_near"))
    LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "",  -1.0, 0);
	chrDisableReloadToLocation = true;
}

void SpainSpyAttack(string qName)
{
    LAi_Fade("", "");
    bQuestCheckProcessFreeze = true;
    WaitDate("",0,0,0, 0, 10); // прошло время
    bQuestCheckProcessFreeze = false;
    AddQuestRecord("WeaponsForEscape", "8");
    LocatorReloadEnterDisable("BridgeTown_town", "houseSp2", false);
}

void _DeadSolder(string qName)
{
    PlaySound("People Fight\Death_NPC_04.WAV");
    DoQuestFunctionDelay("DeadSolder", 1.0);
}

void DeadSolder(string qName)
{
    ref sld;
    sld = &characters[GetCharacterIndex("CPBQuest_Solder")];
    TakeItemFromCharacter(sld, FindCharacterItemByGroup(sld, BLADE_ITEM_TYPE));
    GiveItem2Character(sld, "blade6");
    LAi_KillCharacter(sld);
}

void FindMsStid_ring(string qName)
{
    AddQuestRecord("WeaponsForEscape", "14");
}

void LoginWinterwood()
{
    ref sld, ch, itm;
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Winterwood", "officer_20", "man", "man_fast", 10, PIRATE, 3, false));
	else sld = GetCharacter(NPC_GenerateCharacter("Winterwood", "officer_20", "man", "man", 10, PIRATE, 3, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_164", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_165", "Names.txt", "#space#", " ");
    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
    sld.greeting = "pirat_common";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
    FantomMakeCoolFighter(sld, 7, 25, 20, "blade10", "pistol3", 10);
    GiveItem2Character(sld, "DOjeronRing"); //палец to_do
    itm = ItemsFromID("DOjeronRing");
	itm.picIndex = 15;
    itm.picTexture = "ITEMS_13";
    ChangeItemDescribe("DOjeronRing", "itmdescr_HugtorpRing");
    ChangeItemName("DOjeronRing", "itmname_HugtorpRing");
    AddMoneyToCharacter(sld, 1000);
    sld.SaveItemsForDead = true; // сохранять на трупе вещи
    sld.DontClearDead = true;
    LAi_SetCitizenType(sld);

    string sQuest = "WinterwoodDuel";
    pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
    pchar.quest.(sQuest).win_condition.l1.character = sld.id;
    pchar.quest.(sQuest).function= "WinterwoodDie";

    if(!rand(1))
    {
   	    LAi_SetSitTypeNoGroup(sld);
        ChangeCharacterAddressGroup(sld, "Bridgetown_tavern", "sit","sit4");
    }
    else
    {
   	    LAi_SetStayTypeNoGroup(sld);
        ChangeCharacterAddressGroup(sld, "Bridgetown_Brothel_room", "goto","goto3");

		ch = GetCharacter(NPC_GenerateCharacter("Qhorse", "horse07", "woman", "towngirl", 10, ENGLAND, 1, false));
		sTemp = GetNationNameByType(ENGLAND) + "_citizens";
        LAi_group_MoveCharacter(ch, sTemp);
        LAi_SetActorTypeNoGroup(ch);
		LAi_ActorAfraid(ch, Pchar, false);
		ChangeCharacterAddressGroup(ch, "Bridgetown_Brothel_room", "goto","goto1");

        int n= FindLocation("Bridgetown_Brothel");
        locations[n].reload.l2.disable = false;

    }
}

void WinterwoodDuel(string qName)
{
	pchar.quest.Winterwood2Place.win_condition.l1 = "Location";
	pchar.quest.Winterwood2Place.win_condition.l1.location = "Bridgetown_ExitTown";
	pchar.quest.Winterwood2Place.function = "Winterwood2Place";
}

void Winterwood2Place(string qName)
{
    ref sld;
    sld = characterFromID("Winterwood");
   	sld.Dialog.CurrentNode = "talk_off_town";
    if (CheckAttribute(sld, "CityType"))
	{
		DeleteAttribute(sld, "City"); // чтоб не было ругани с нацией
		DeleteAttribute(sld, "CityType");
	}
   	ChangeCharacterAddressGroup(sld, "Bridgetown_ExitTown", "goto","goto1");
   	LAi_SetStayTypeNoGroup(sld);
    LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "",  -1.0, 0);
	chrDisableReloadToLocation = true;
}

void Winterwood_Prepare_Fight()
{
	int i, idx;
	ref npchar, rLoc;

	npchar = characterFromID("Winterwood");
	makeref(rLoc, Locations[FindLoadedLocation()]);

	if (CheckAttribute(rLoc, "type") && rLoc.type == "tavern")
	{
		if (PChar.chr_ai.type == LAI_TYPE_SIT)
		{
			LAi_SetPlayerType(pchar);
			PlaceCharacter(pchar, "tables", pchar.location);
		}
		PChar.Quest.Duel_Fight_Right_Now.win_condition.l1 = "Location";
		PChar.Quest.Duel_Fight_Right_Now.win_condition.l1.Location = "Clone_location";
		PChar.Quest.Duel_Fight_Right_Now.function = "Winterwood_Fight_Right_Now";

		LocationMakeClone(pchar.location);
		Locations[FindLocation("Clone_location")].image = "loading\jonny_load\inside\fight_taverna_"+rand(1)+".dds";
		DoReloadCharacterToLocation("Clone_location", pchar.location.group, pchar.location.locator);
		PlaceCharacter(npchar, "goto", "Clone_location")
	}
	else
	{
		DoQuestFunctionDelay("Winterwood_Fight_Right_Now", 0.5);
	}
	LAi_SetWarriorType(npchar);
	LAi_group_MoveCharacter(npchar, "DUEL_FIGHTER");
}

void Winterwood_Fight_Right_Now(string qName)
{
	bool bOk = false;
	ref sld = &Locations[FindLocation(pchar.location)];

	//запоминаем запрет на оружие в локации
	if (CheckAttribute(sld, "noFight"))
	{
		bOk = sti(sld.noFight);
	}
	if (bOk)
	{
		LAi_LocationFightDisable(sld, !bOk);
		pchar.questTemp.LocFightEnable = bOk;
	}
	//достать саблю
	LAi_SetFightMode(pchar, true);
	LAi_group_SetRelation("DUEL_FIGHTER", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("DUEL_FIGHTER", LAI_GROUP_PLAYER, true);
	chrDisableReloadToLocation = true;
}

void WinterwoodDie(string qName)
{
    if (CheckAttribute(PChar, "quest.Winterwood2Place")) Pchar.quest.Winterwood2Place.over = "yes";
    if (CheckAttribute(PChar, "quest.CapBloodLineTimer_3")) Pchar.quest.CapBloodLineTimer_3.over = "yes";

	//восстанавливаем запрет на оружие, если был
	if (CheckAttribute(pchar, "questTemp.LocFightEnable") && sti(pchar.questTemp.LocFightEnable))
	{
		sld = &Locations[FindLocation(pchar.location)];
		LAi_LocationFightDisable(sld, true);
		DeleteAttribute(pchar, "questTemp.LocFightEnable");
	}

	//если в локации клоне, то объясняем куда все делись
	if (CheckAttribute(pchar, "questTemp.LocationClone") && sti(pchar.questTemp.LocationClone))
	{
		DoQuestCheckDelay("TalkSelf_Start", 0.2);
	}

	//убрать оружие
	DoQuestCheckDelay("hide_weapon", 2.0);
	chrDisableReloadToLocation = false;
    AddQuestRecord("HugtorpTrouble", "3");
}

void MoneyForDieke(string qName)
{
    sld = characterFromID("Dieke");
    sld.Dialog.CurrentNode = "DKStep_3";
    AddQuestRecord("DiekeQuest", "4")
}

void CapBloodLine_q2_Late(string qName)
{
    InterfaceStates.Buttons.Save.enable = false;
    GameOver("blood");
}

void CapBloodLine_q2_Complited()
{
    ref sld;
    Pchar.quest.CapBloodLineTimer_2.over = "yes";
    AddQuestRecord("CapBloodLine_q2", "6");
    int n= FindLocation("Bridgetown_tavern");
    locations[n].reload.l2.disable = false;
    sld = &characters[GetCharacterIndex("Waker")];
    sld.Dialog.CurrentNode = "WStep_11";
    LAi_SetStayTypeNoGroup(sld);

    bQuestCheckProcessFreeze = true;
    Environment.date.day = 3;
    SetCurrentTime(7, 0); // должно быть 3-е число 7 часов
    bQuestCheckProcessFreeze = false;

    //Шпион
    sld = GetCharacter(NPC_GenerateCharacter("MoneySpy", "shipowner_5", "man", "man", 10, ENGLAND, 1, false));
    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
    FantomMakeCoolFighter(sld, 10, 25, 50, "blade9", "", 10);
    sld.quest.meeting = 0;
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
    LAi_SetOwnerType(sld);
    LAi_group_MoveCharacter(sld, sTemp);
    sld.talker = 10; //fix
   	ChangeCharacterAddressGroup(sld, "CommonDoubleFlourHouse_1", "barmen", "stay");
    //#20190120-02
    LocatorReloadEnterDisable("BridgeTown_town", "houseS1", false);
    //Властелин рыб
    sld = GetCharacter(NPC_GenerateCharacter("Fisherman", "shipowner_8", "man", "man", 10, ENGLAND, 1, false));
    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
    sld.name = GetConvertStrWithReplace("Variable_reaction_functions_166", "Names.txt", "#space#", " ");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_167", "Names.txt", "#space#", " ");
	sld.greeting = "Gr_Fisher";
    sld.Default           = "Bridgetown_tavern";
    sld.Default.group     = "sit";
    sld.Default.ToLocator = "sit_front4";//fix
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
	LAi_SetSitType(sld);
    LAi_group_MoveCharacter(sld, sTemp);
    ChangeCharacterAddressGroup(sld, "Bridgetown_tavern", "sit","sit_base4"); //fix

    //Жак Соловей
    sld = GetCharacter(NPC_GenerateCharacter("Jack", "DSolovey", "man", "man_kvl2", 10, ENGLAND, 1, false));
    sld.name = GetConvertStrWithReplace("Variable_reaction_functions_168", "Names.txt", "#space#", " ");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_169", "Names.txt", "#space#", " ");
    sld.greeting = "Gr_Solovey";
    sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
    FantomMakeCoolFighter(sld, 10, 25, 50, "blade7", "", 10);
    sld.dialog.currentnode = "JSTStep_0";
    sld.talker = 10;
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";
	LAi_SetStayType(sld);
    LAi_group_MoveCharacter(sld, sTemp);

   	ChangeCharacterAddressGroup(sld, "Bridgetown_Store", "goto", "goto1");
   	sld = characterFromID("Bridgetown_trader");
   	LAi_SetLoginTime(sld, 0.0, 1.0);

    //сроки
    PChar.quest.CapBloodLineTimer_2.win_condition.l1            = "Timer";
    PChar.quest.CapBloodLineTimer_2.win_condition.l1.date.hour  = 15;
    PChar.quest.CapBloodLineTimer_2.win_condition.l1.date.day   = GetAddingDataDay(0, 0, 0);
    PChar.quest.CapBloodLineTimer_2.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 0);
    PChar.quest.CapBloodLineTimer_2.win_condition.l1.date.year  = GetAddingDataYear(0, 0, 0);
    PChar.quest.CapBloodLineTimer_2.function = "CapBloodLine_q2_Late";

    SetLaunchFrameFormParam(GetConvertStr("Variable_reaction_functions_4", "Frames.txt"), "Reload_To_Location", 0.1, 2.0);
    SetLaunchFrameReloadLocationParam("Plantation_S1", "goto", "goto1", "");
   	LaunchFrameForm();
}

void QUsurerLate(string qName)
{
    ChangeCharacterHunterScore(PChar, "enghunter", 30);//fix было -30;
}

void LoginMainer()
{

        Pchar.questTemp.CapBloodLine.ShipForJack = false;
		sld = GetCharacter(NPC_GenerateCharacter("Mainer", "officer_1", "man", "man", 7, ENGLAND, 3, false));
        sld.name = GetConvertStrWithReplace("Variable_reaction_functions_170", "Names.txt", "#space#", " ");
        sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_171", "Names.txt", "#space#", " ");
		sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
		sld.dialog.currentnode = "MNStep_0";
		sld.city = "Bridgetown_town";

		LAi_SetCitizenType(sld);
		sTemp = GetNationNameByType(ENGLAND) + "_citizens";
		LAi_group_MoveCharacter(sld, sTemp);

        ChangeCharacterAddressGroup(sld, "Bridgetown_town", "reload", "reload1");
}

void CapBloodLine_q3_Complited()
{
    Pchar.quest.CapBloodLineTimer_2.over = "yes";
    pchar.quest.CapBloodEscape1.win_condition.l1          = "location";
    pchar.quest.CapBloodEscape1.win_condition.l1.location = "Bridgetown_town";
    pchar.quest.CapBloodEscape1.function                  = "ReturnToPlantation1";

}

void ReturnToPlantation1(string qName)
{
    ref sld;
    sld = GetCharacter(NPC_GenerateCharacter("SolderTakeBlades", "sold_eng_"+(rand(7)+1), "man", "man", 10, ENGLAND, 0, false));
	sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
	sld.dialog.currentnode = "STBStep_2";
	sld.city = "Bridgetown_town";
    sld.greeting = "Gr_HUNTER";
    sld.location = "none";
    sld.BreakTmplAndFightGroup = true;
    SetFantomParamHunter(sld); //крутые парни
	LAi_SetCitizenType(sld);
	sTemp = GetNationNameByType(ENGLAND) + "_citizens";

    LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, sTemp);
    PlaceCharacter(sld, "goto", "random_must_be_near"));
	LAi_ActorDialog(sld, pchar, "",  -1.0, 0);
	chrDisableReloadToLocation = true;

}

void ReturnToPlantation2(string qName)
{
    ref sld;
    sld = characterFromID("Hugtorp");
    LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "",  -1.0, 0);

    sld = characterFromID("Pitt");
    ChangeCharacterAddressGroup(sld, "None", "", "");
	chrDisableReloadToLocation = true;
}

void ReturnToPlantation3()
{
    ref sld;
    SetLocationCapturedState("BridgeTown_Plantation", true);
    string sTemp = "TmpEnemy";
   	LAi_group_SetHearRadius(sTemp, LAI_GROUP_GRD_HEAR - 3);
   	LAi_group_SetSayRadius(sTemp, LAI_GROUP_GRD_SAY - 1);
   	LAi_group_SetLookRadius(sTemp, LAI_GROUP_GRD_LOOK - 1);
    LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_SetAlarm(sTemp, LAI_GROUP_PLAYER, 0);

    SetShipInBridgetown(); // Готовим Бриджтаун
    Pchar.questTemp.CapBUnarmed = false;

    sld = characterFromID("Hugtorp");
    LAi_LoginInCaptureTown(sld, true);
    LAi_SetStayTypeNoGroup(sld);
    sld.talker = 10;
    sld.Dialog.CurrentNode = "HTStep_18"; //to_do
    GiveItem2Character(sld, "blade7");
    EquipCharacterByItem(sld, "blade7");
    TakeNItems(sld, "potion1", 5);
    ChangeCharacterAddressGroup(sld, "Bridgetown_town", "quest", "quest3");

    sld = characterFromID("Volverston");
    LAi_LoginInCaptureTown(sld, true);
    LAi_SetStayTypeNoGroup(sld);
    sld.Dialog.CurrentNode = "HTStep_18";  //to_do
    GiveItem2Character(sld, "blade7");
    EquipCharacterByItem(sld, "blade7");
    TakeNItems(sld, "potion1", 5);
	LAi_SetImmortal(sld, false);
    ChangeCharacterAddressGroup(sld, "Bridgetown_town", "quest", "quest4");

    sld = characterFromID("Ogl");
    LAi_LoginInCaptureTown(sld, true);
    sld.Dialog.CurrentNode = "HTStep_18";  //to_do
    LAi_SetStayTypeNoGroup(sld);
    GiveItem2Character(sld, "blade7");
    EquipCharacterByItem(sld, "blade7");
    TakeNItems(sld, "potion1", 5);
    LAi_SetImmortal(sld, false);
    ChangeCharacterAddressGroup(sld, "Bridgetown_town", "quest", "quest5");

    sld = characterFromID("Dieke");
    LAi_LoginInCaptureTown(sld, true);
    LAi_SetStayTypeNoGroup(sld);
    sld.Dialog.CurrentNode = "HTStep_18";  //to_do
    GiveItem2Character(sld, "blade7");
    EquipCharacterByItem(sld, "blade7");
    TakeNItems(sld, "potion1", 5);
    LAi_SetImmortal(sld, false);
    ChangeCharacterAddressGroup(sld, "Bridgetown_town", "quest", "quest6");

    sld = characterFromID("Pitt");
    LAi_LoginInCaptureTown(sld, true);
    LAi_SetActorType(sld);
    LAi_ActorSetLayMode(sld);
    ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "quest", "sit2");

    worldMap.date.day = 4;
    Environment.date.day = 4;
    SetCurrentTime(0, 0);

    int n = FindLocation("BridgeTown_Plantation");
    DeleteAttribute(&locations[n], "citizens");
    DeleteAttribute(&locations[n], "carrier");

    DoReloadCharacterToLocation("BridgeTown_Plantation", "reload", "reload1_back");
    SetNationRelation2MainCharacter(ENGLAND, RELATION_ENEMY);

    for (int i=0; i < 7; i++)
	{
        if(i < 2)
        {
            sld = GetCharacter(NPC_GenerateCharacter("PittGuard_"+i, "sold_eng_"+(rand(7)+1), "man", "man", 10, ENGLAND, 0, false));
            SetFantomParamFromRank(sld, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE, true); // бравые орлы
            LAi_LoginInCaptureTown(sld, true);
            LAi_SetActorType(sld);
            LAi_SetStayType(sld); // послабление
            LAi_group_MoveCharacter(sld, sTemp);

            ChangeCharacterAddressGroup(sld, "BridgeTown_Plantation", "quest", "sit"+(2*i+1));
        }
        else
        {
            int iChar = NPC_GeneratePhantomCharacter("soldier", ENGLAND, MAN, 2);
            sld = &characters[iChar];
    		SetNPCModelUniq(sld, "soldier", MAN);
    		sld.City = "Bridgetown";
            sld.CityType = "soldier";
            SetFantomParamFromRank(sld, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE, true);
            LAi_LoginInCaptureTown(sld, true);
           	LAi_SetWarriorType(sld);
    		LAi_SetPatrolType(sld);
            LAi_group_MoveCharacter(sld, sTemp);
    		PlaceCharacter(sld, "patrol", "random_free");
        }
        LAi_SetLoginTime(sld, 0.0, 24.0);
        sld.greeting = "soldier_common";
        sld.dialog.filename = "Common_Soldier.c";
        sld.dialog.currentnode = "first time";
	}

    pchar.GenQuestBox.BridgeTown_Plantation.box1.items.blade6 = 1;
    pchar.GenQuestBox.BridgeTown_Plantation.box1.items.potion1 = 15; // LEO: Упростить болванов не могу, но плюшек накину
    pchar.GenQuestBox.BridgeTown_Plantation.box1.items.Food1 = 50; // LEO: Упростить болванов не могу, но плюшек накину

   	pchar.quest.CapBloodEscape3.win_condition.l1 = "locator";
	pchar.quest.CapBloodEscape3.win_condition.l1.location = "BridgeTown_Plantation";
	pchar.quest.CapBloodEscape3.win_condition.l1.locator_group = "quest";
	pchar.quest.CapBloodEscape3.win_condition.l1.locator = "detector1";
	pchar.quest.CapBloodEscape3.function = "ReturnToPlantation4";

    chrDisableReloadToLocation = true;

}

void ReturnToPlantation4(string qName)
{
    ref sld;
    string sTemp = "TmpEnemy";
   	LAi_group_SetHearRadius(sTemp, 5); // послабление
   	LAi_group_SetSayRadius(sTemp, 6);
   	LAi_group_SetLookRadius(sTemp, 7);
    for (i = 0 ; i < 2; i++)
    {
        sld = characterFromID("PittGuard_"+i);
        LAi_SetWarriorTypeNoGroup(sld);
        LAi_group_Attack(sld, Pchar);
    }
    sld = characterFromID("Pitt");
    sld.Dialog.CurrentNode = "PStep_11";
    LAi_SetGroundSitTypeNoGroup(sld);
    SetLocationCapturedState("Bridgetown_town", true);
    pchar.quest.CapBloodEscape4.win_condition.l1          = "location";
    pchar.quest.CapBloodEscape4.win_condition.l1.location = "Bridgetown_town";
    pchar.quest.CapBloodEscape4.function                  = "StillShip";
}

void StillShip(string qName)
{

    ref chr,sld ;
    aref st;
    int iNation = SPAIN;
	string slai_group, locatorName, sType, slocator;

    slai_group = "TmpEnemy";
   	LAi_group_SetHearRadius(slai_group, LAI_GROUP_GRD_HEAR - 3);
   	LAi_group_SetSayRadius(slai_group, LAI_GROUP_GRD_SAY - 1);
   	LAi_group_SetLookRadius(slai_group, LAI_GROUP_GRD_LOOK - 1);

    LAi_group_SetAlarm(sTemp, LAI_GROUP_PLAYER, 0);

	for (i=0; i < 11; i++)
	{
        chr = GetCharacter(NPC_GenerateCharacter("SpaSolder_"+i, "sold_spa_"+(rand(7)+1), "man", "man", sti(pchar.rank)+rand(2)+1, SPAIN, 1, false));
		chr.City = "Bridgetown";
        chr.CityType = "soldier";
        SetFantomParamFromRank(chr, sti(pchar.rank)+rand(2)+1, true); // бравые орлы
		LAi_LoginInCaptureTown(chr, true);
        LAi_SetLoginTime(chr, 0.0, 24.0);
       	LAi_SetWarriorType(chr);
		LAi_SetPatrolType(chr);
		LAi_group_MoveCharacter(chr, slai_group);
		chr.greeting = "soldier_common";
		slocator = PlaceCharacter(chr, "patrol", "random_free");
		while( slocator == "patrol13")  // слишком близко к воротам
		{
            slocator = PlaceCharacter(chr, "patrol", "random_free");
		}
        chr.dialog.filename = "Common_Soldier.c";
        chr.dialog.currentnode = "first time";
	}
	sld = GetCharacter(NPC_GenerateCharacter("SpaRaider", "off_spa_2", "man", "man", 10, SPAIN, 1, false));
    FantomMakeCoolFighter(sld, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE, 25, 20, RandPhraseSimple("blade19","blade22"), "pistol2", 10);
    sld.SaveItemsForDead = true; // сохранять на трупе вещи
    sld.DontClearDead = true;
    sld.BreakTmplAndFight = true;
	LAi_LoginInCaptureTown(sld, true);
    LAi_SetLoginTime(chr, 0.0, 24.0);
    LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, slai_group);
	sld.greeting = "soldier_common";
    sld.dialog.filename = "Common_Soldier.c";
    sld.dialog.currentnode = "first time";
	sld.City = "Bridgetown";
    sld.CityType = "soldier";
    ChangeCharacterAddressGroup(sld, "Bridgetown_town", "reload", "houseF2");

    string sQuest = "SaveArabelaService";
    pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
    pchar.quest.(sQuest).win_condition.l1.character = sld.id;
    pchar.quest.(sQuest).function= "SaveArabelaService";

	chr = GetCharacter(NPC_GenerateCharacter("ArabelaService", "Girl_8", "woman", "towngirl", 10, ENGLAND, 1, false));
	chr.greeting = "Enc_RapersGirl_1";
    chr.dialog.filename = "Quest\CapBloodLine\questNPC.c";
    chr.dialog.currentnode = "ASStep_0";

    LAi_SetActorType(chr);
   	LAi_LoginInCaptureTown(chr, true);
    LAi_SetLoginTime(chr, 0.0, 24.0);
    ChangeCharacterAddressGroup(chr, "Bridgetown_town", "officers", "houseH1_2");

	LAi_ActorRunToLocator(chr, "officers", "gate1_3", "SaveArabelaServiceAfraid", 5);
    LAi_ActorFollow(sld, chr, "", -1);

   	pchar.quest.CapBloodEscape5.win_condition.l1 = "locator";
	pchar.quest.CapBloodEscape5.win_condition.l1.location = "Bridgetown_town";
	pchar.quest.CapBloodEscape5.win_condition.l1.locator_group = "quest";
	pchar.quest.CapBloodEscape5.win_condition.l1.locator = "quest11";
	pchar.quest.CapBloodEscape5.function = "ShipGuardsAlarm";

	ShipGuards();

	Pchar.questTemp.CapBloodLine.SpainInBridgetown = false;
	Pchar.questTemp.CapBloodLine.withCrew = false;

	LocatorReloadEnterDisable("BridgeTown_town", "reload5_back", true);
	LocatorReloadEnterDisable("BridgeTown_town", "reloadShip", true);

    Colonies[FindColony("Bridgetown")].DontSetShipInPort = true;
    sld = characterFromID("Bridgetown Fort Commander");
    sld.Fort.Mode = FORT_DEAD;
	// data of frot die
	sld.Fort.DieTime.Year = GetDataYear();
	sld.Fort.DieTime.Month = GetDataMonth();
	sld.Fort.DieTime.Day = GetDataDay();
	sld.Fort.DieTime.Time = GetTime();
	Event(FORT_DESTROYED, "l", sti(sld.index));

}

void ShipGuards()
{
    ref chr,sld;
    string slai_group = "EnemyFight";
    int iChar;
	ref location;
	string boxId = "box1";
	aref boxItems;
   	string model[6];

    	// ==> рабы массовка
		model[0] = "shipowner_1";
		model[1] = "shipowner_6";
		model[2] = "shipowner_9";
		model[3] = "shipowner_10";
		model[4] = "pirate_1";
		model[5] = "pirate_8";

    LAi_group_Delete(slai_group); // fix
	for (int i=0; i <= 9; i++)
	{

        if(i <= 6)
        {

            chr = GetCharacter(NPC_GenerateCharacter("ShipGuard_"+i, "sold_spa_"+(rand(7)+1), "man", "man", 10, SPAIN, 1, false));
            SetFantomParamFromRank(chr, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE, true); // бравые орлы
            LAi_SetImmortal(chr, true); //homo в одиночку ГГ не справится ;-)
    		LAi_LoginInCaptureTown(chr, true);
            LAi_SetLoginTime(chr, 0.0, 24.0);
    		LAi_SetActorTypeNoGroup(chr);
    		LAi_ActorStay(chr);
    		LAi_group_MoveCharacter(chr, slai_group);
    		ChangeCharacterAddressGroup(chr, "Bridgetown_town", "rld", "aloc"+i);
		}
		else
		{
            sld = GetCharacter(NPC_GenerateCharacter("FreeSlave_"+(i-6), model[rand(5)], "man", "man", 10, SPAIN, 1, false));
            SetFantomParamFromRank(sld, sti(pchar.rank)+makeint(2*MOD_SKILL_ENEMY_RATE/3), true); // бравые орлы
            sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
            LAi_LoginInCaptureTown(sld, true);
            LAi_SetActorType(sld);
    		LAi_ActorStay(sld);
            LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
            ChangeCharacterAddressGroup(sld, "Bridgetown_town", "quest", "quest"+i);
        }
    }
    sld = GetCharacter(NPC_GenerateCharacter("SpaFirstMate", "off_spa_1", "man", "man", 12, SPAIN, 1, false));
    FantomMakeCoolFighter(sld, 12, 30, 30, "blade30", "pistol3", 10);
	LAi_LoginInCaptureTown(sld, true);

    LAi_SetWarriorType(sld);
    ChangeCharacterAddressGroup(sld, "Cabin", "rld", "aloc0");
    pchar.quest.SpaFirstMateDie.win_condition.l1 = "NPC_death";
    pchar.quest.SpaFirstMateDie.win_condition.l1.character = "SpaFirstMate";
   	pchar.quest.SpaFirstMateDie.function = "SpaFirstMateDie";

	location = &Locations[FindLocation("Cabin")];
	makearef(boxItems, location.(boxId).items);
	boxItems.gold = 4000 * MOD_SKILL_ENEMY_RATE + rand(2000) + 250;
	boxItems.chest = 4;
	location.(boxId) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
}

void SaveArabelaService(string qName)
{
    ref sld;
    sld = characterFromID("ArabelaService");
    LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "",  -1.0, 0);
	sld = characterFromID("Hugtorp");
	sld.talker = 10;
}

void CapBloodOfficers()
{
    string offName;
    ref sld;
    Pchar.questTemp.CapBloodLine.withCrew = true;
       for (int i=1 ; i <= 7; i++)
    {

        if(i > 4) offName ="FreeSlave_"+(i-4);
        switch(i)
        {
            case 1:
                offName ="Volverston";
            break;

            case 2:
                offName ="Hugtorp";
            break;

            case 3:

                offName ="Dieke";
            break;

            case 4:

                offName ="Ogl";
            break;
        }
        sld = characterFromID(offName);
        LAi_SetImmortal(sld, false);
        sld.DontClearDead = true;  // не убирать труп
        sld.BreakTmplAndFight = true;
        LAi_SetActorType(sld);
        LAi_ActorFollow(sld, pchar, "", -1);
        LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
        sld.Dialog.CurrentNode = "HTStep_22b";  //

    }
}
void ShipGuardsDie()
{
    ref sld;
    chrDisableReloadToLocation = false;
    LocatorReloadEnterDisable("BridgeTown_town", "reloadShip", false);
    sld = characterFromID("SpaFirstMate");
    LAi_group_MoveCharacter(sld, "EnemyFight");
}

void ShipGuardsAlarm(string qName)
{
    chrDisableReloadToLocation = true; // закрыть выход из локации
    LAi_group_SetCheck("EnemyFight", "ShipGuardsDie");
    DoQuestFunctionDelay("AttakShip", 2.0);
}

void AttakShip(string qName)
{
    string offName;
    ref sld;
    int iChar;
    for (int i=0 ; i <= 9; i++)
    {
        offName = "ShipGuard_"+i;
        switch(i)
        {
            case 7:
                offName ="Volverston";
            break;

            case 8:
                offName ="Hugtorp";
            break;

            case 9:

                offName ="Dieke";
            break;

            case 10:
                offName ="Ogl";
            break;
        }
        iChar = GetCharacterIndex(offName);
        if (iChar != -1)
        {
            sld = &characters[iChar]
       	    LAi_SetWarriorTypeNoGroup(sld);
       	    if(Pchar.questTemp.CapBloodLine.withCrew) LAi_SetImmortal(sld, false);
        }
    }
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
    LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL); // Чтобы не кидались назад в город

}

void SpaFirstMateDie(string qName)
{
    string offName;
    ref sld;
    int iChar;
    for (int i=0; i < 13; i++)
	{

       offName = "SpaSolder_"+i;
        switch(i)
        {
            case 11:
                offName ="SpaRaider";
            break;

            case 12:
                offName ="ArabelaService";
            break;

        }
        iChar = GetCharacterIndex(offName);
        if (iChar != -1)
        {
            sld = &characters[iChar];
            sld.lifeDay = 0;
        }
    }
    pchar.quest.CapBloodEscape6.win_condition.l1          = "location";
    pchar.quest.CapBloodEscape6.win_condition.l1.location = "Bridgetown_town";
    pchar.quest.CapBloodEscape6.function                  = "QOfficersRecruit";
    AddQuestRecord("EscapeFormBarbados", "5");
    CloseQuestHeader("EscapeFormBarbados");
    ShipGuardsDie();
}

void QOfficersRecruit(string qName)
{
    string offName, sNode;
    ref sld;
    int iChar;
    for (int i=0 ; i <= 4; i++)
    {
        offName = "ShipGuard_"+i;
        switch(i)
        {
            case 0:
                offName ="Pitt";
                sNode = "PStep_16";
            break;

            case 1:
                offName ="Hugtorp";
                sNode = "HTStep_24";
            break;

            case 2:
                offName ="Dieke";
                sNode = "DKStep_5";
            break;

            case 3:
                offName ="Ogl";
                sNode = "OGLStep_11";
            break;

			case 4:
                offName ="Volverston";
                sNode = "VLStep_11";
            break;
        }
        iChar = GetCharacterIndex(offName);
        if (iChar != -1)
        {
            sld = &characters[iChar];
            sld.loyality = MAX_LOYALITY; //fix лояльности квестовых
            LAi_SetImmortal(sld, false);//fix бессмертного Питта
       	    ChangeCharacterAddressGroup(sld, "Bridgetown_town", "rld", "aloc"+(i+1));
            LAi_SetActorTypeNoGroup(sld);
            LAi_ActorTurnToLocator(sld, "quest", "quest11");
            sld.dialog.currentnode = sNode;
            if (offName =="Pitt") LAi_ActorDialog(sld, pchar, "",  -1.0, 0);
            else LAi_SetStayTypeNoGroup(sld);

        }
    }

}

// LEO: возвращаем только бонусы агента после пролога Блада, один раз и без переодевания героя.
void Blood_RestoreAgentBonuses()
{
	if (!CheckAttribute(PChar, "starttype")) return;
	if (sti(PChar.starttype) != 1) return;
	if (!CheckAttribute(PChar, "HeroParam.HeroType")) return;
	if (PChar.HeroParam.HeroType != "Agent") return;
	if (!CheckAttribute(PChar, "questTemp.CapBloodLine")) return;
	if (sti(PChar.questTemp.CapBloodLine) != 0) return;
	if (CheckAttribute(PChar, "BloodAgentBonusesRestored")) return;

	SetCharacterPerk(PChar, "FlagEng");
	SetCharacterPerk(PChar, "FlagFra");
	SetCharacterPerk(PChar, "FlagSpa");
	SetCharacterPerk(PChar, "FlagHol");
	if (GetCharacterItem(PChar, "suit_1") < 1)
	{
		if (!TakeNItems(PChar, "suit_1", 1)) return;
	}
	PChar.BloodAgentBonusesRestored = true;
}

void CapBloodLaspEpisode(string qName)
{
    RestoreBridgetown();
	Blood_RestoreAgentBonuses();
}

void SpaCrewAtack()
{
    int hcrew,iChar,i;
    ref chref;

    chref = characterFromID("Pitt");
    chref.Dialog.CurrentNode = "hired";
   	chref.Dialog.FileName = "Enc_Officer_dialog.c";
	chref.greeting = "Gr_questOfficer";

    string sGroup = "SpaCrew";
    Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup,"war");
    Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);

    for (i=0; i<2; i++)
    {
        iChar = NPC_GenerateCharacter("SpaBoat_"+i, "off_spa_2", "man", "man", 7, SPAIN, 1, true));
        makeref(chref, Characters[iChar]);
        chref.Ship.Mode = "war";
        chref.Ship.Type = GenerateShipExt(SHIP_LUGGER, 1, chref);
        SetRandomNameToCharacter(chref);
        SetRandomNameToShip(chref);

        SetBaseShipData(chref);

		SetCrewQuantity(chref, 150);//но не больше максимума

        DeleteAttribute(chref,"ship.sails");
        RepairMasts(chref);
        DeleteAttribute(chref,"ship.blots");

        Fantom_SetCannons(chref, "war");
        Fantom_SetBalls(chref, "war");
        SetFantomParamHunter(chref); //крутые парни
        SetCaptanModelByEncType(sld, "war");//
        Group_AddCharacter(sGroup, chref.id);
        if (i == 0) Group_SetGroupCommander(sGroup, chref.id);
    }
    Group_SetAddress(sGroup, "Barbados", "IslandShips1", "ship_3");
}

void VolverstonAdd()
{
    ref chref;
    chref = characterFromID("Volverston");
    chref.Dialog.CurrentNode = "hired";
   	chref.Dialog.FileName = "Enc_Officer_dialog.c";
	chref.greeting = "Gr_questOfficer";
}

void HugtorpAdd()
{
    ref chref;
    chref = characterFromID("Hugtorp");
    chref.Dialog.CurrentNode = "hired";
   	chref.Dialog.FileName = "Enc_Officer_dialog.c";
	chref.greeting = "Gr_questOfficer";
}

void DiekeAdd()
{
    ref chref;
    chref = characterFromID("Dieke");
    chref.Dialog.CurrentNode = "hired";
   	chref.Dialog.FileName = "Enc_Officer_dialog.c";
	chref.greeting = "Gr_questOfficer";
}

void OglAdd()
{
    ref chref;
    chref = characterFromID("Ogl");
    chref.Dialog.CurrentNode = "hired";
   	chref.Dialog.FileName = "Enc_Officer_dialog.c";
	chref.greeting = "Gr_questOfficer";
}

////////////////////
//BlackThorn - Посланники ада
////////////////////
void HellSpawn(string qName)
{
	if (GetQuestPastDayParam("pchar.questTemp.HellSpawn") < 1  && GetCharacterEquipSuitID(pchar)!= "suit_1")
	{
		chrDisableReloadToLocation = true;
		for (int i = 1; i<15; i++)
		{
			sld = GetCharacter(NPC_GenerateCharacter("HellSpawn"+i, "Skel"+(rand(3)+1), "skeleton", "skeleton", 1, PIRATE, 1, true));
			FantomMakeCoolFighter(sld, sti(pchar.rank)+5, 90, 90, LinkRandPhrase(RandPhraseSimple("blade23","blade25"), RandPhraseSimple("blade30","blade26"), RandPhraseSimple("blade24","blade13")), RandPhraseSimple("pistol6", "pistol3"), MOD_SKILL_ENEMY_RATE*4);

			ChangeCharacterAddressGroup(sld, pchar.HellSpawnLocation, "patrol", "patrol"+i);
			sld.LifeDay = 0;

			if (i == 1)
			{
				LAi_SetActorType(sld);
				LAi_type_actor_Reset(sld);
				LAi_ActorDialog(sld, pchar, "", 20.0, 0);
				sld.dialog.filename = "Quest\HellSpawn.c";
			}
			else
			{
				LAi_SetWarriorType(sld);
			}
		}
	}
	else
	{
		ref locationHS = &locations[reload_location_index];
		LocatorReloadEnterDisable(locationHS.id, "reload7_back", false);
	}
}
void HellSpawnFinish(string qName)
{
	chrDisableReloadToLocation = false;
	pchar.HellSpawnFinished = true;
}

void HellSpawnRitualFinish(string qName)
{
	chrDisableReloadToLocation = false;
	pchar.questTemp.HellSpawn.Rebirth = true;
	DoQuestCheckDelay("TalkSelf_Quest", 1.0);
}

void HellSpawnRitual(string qName)
{
	if (GetQuestPastDayParam("pchar.questTemp.HellSpawn.Rit") < 10)
	{
		ref locationHSRitual = &locations[reload_location_index];
		chrDisableReloadToLocation = true;

		aref grp;
		makearef(grp, locationHSRitual.locators.monsters);
		pchar.Hellspawn.num = GetAttributesNum(grp)/3;
		for(int i = 1; i < sti(pchar.Hellspawn.num); i++)
		{
			sld = GetCharacter(NPC_GenerateCharacter("HellSpawnR"+i, "Skel"+(rand(3)+1), "skeleton", "skeleton", 1, PIRATE, 1, true));
			FantomMakeCoolFighter(sld, sti(pchar.rank)+5, 90, 90, LinkRandPhrase(RandPhraseSimple("blade23","blade25"), RandPhraseSimple("blade30","blade26"), RandPhraseSimple("blade24","blade13")), RandPhraseSimple("pistol6", "pistol3"), MOD_SKILL_ENEMY_RATE*4);
			ChangeCharacterAddressGroup(sld, locationHSRitual.id, "monsters", GetAttributeName(GetAttributeN(grp, i)));
			sld.LifeDay = 0;

			if (i == 1)
			{
				LAi_SetActorType(sld);
				LAi_type_actor_Reset(sld);
				LAi_ActorDialog(sld, pchar, "", 20.0, 0);
				sld.dialog.filename = "Quest\HellSpawn.c";
				sld.dialog.currentnode = "Ritual";
			}
			else
			{
				LAi_SetWarriorType(sld);
				LAi_warrior_SetStay(sld, true);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
void RestoreBridgetown()
{
        ref rColony;
        Pchar.questTemp.CapBloodLine = false;
        Pchar.model="PGG_Blad_0"; // 21.03.09 Warship fix Корректная замена модели
		DeleteAttribute(pchar,"Cirgnore");  //снятие атрибута, блокирующего смену модели кирасной логикой
        //Губернатор
        ref sld = &characters[GetCharacterIndex("Bridgetown_Mayor")];
        sld.Dialog.Filename = "Common_Mayor.c";
        sld.Dialog.CurrentNode = "First time";
        //город
        int n = FindLocation("Bridgetown_town");  // не забыть вернуть
        locations[n].reload.l3.close_for_night = true;
        locations[n].questSeekCap = 6;
        locations[n].houseEnc = true;
        //Таверна пьянь
        n = FindLocation("Bridgetown_tavern");
        locations[n].habitues = 1;
        DeleteAttribute(&locations[n], "notCloseCommonHouse");
        //Диалоги горожан
        ChangeBridgeTownDialogs(true);
        bDisableLandEncounters = false;
        chrDisableReloadToLocation = false;
        bDisableFastReload = false;
        Pchar.questTemp.CapBUnarmed = false;

        n = FindLocation("CommonStoneHouse");
        locations[n].models.day.locators = "StoneHouse_locators";
        locations[n].models.night.locators = "StoneHouse_Nlocators";
        //Плантации
        n = FindLocation("BridgeTown_Plantation");
       	locations[n].citizens = true;
    	locations[n].carrier = true;
    	//
    	SetLocationCapturedState("Bridgetown_town", false);
    	SetLocationCapturedState("BridgeTown_Plantation", false);
    	LocatorReloadEnterDisable("BridgeTown_town", "reload5_back", false);
    	LocatorReloadEnterDisable("BridgeTown_town", "reload1_back", false);
	    LocatorReloadEnterDisable("BridgeTown_fort", "reload2", false); //fix

        LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", false);//fix полностью открываем резиденцию
        LocatorReloadEnterDisable("Bridgetown_Townhall", "reload3", false);
        LocatorReloadEnterDisable("BridgeTown_town", "reloadR1", false);

    	//Торговца вернуть
    	sld = characterFromID("Bridgetown_trader");
   	    LAi_RemoveLoginTime(sld);
    	for (int j = 0; j< MAX_COLONIES; j++)
		{
			ClearIslandShips(colonies[j].id);
			DeleteAttribute(&colonies[j],"AlreadyGen");
			GenerateIslandShips(colonies[j].island);
		}
		InitPsHeros();
        makeref(rColony, Colonies[FindColony("Bridgetown")]);
        DeleteAttribute(rColony, "DontSetShipInPort"); //возвращаем жизнь
        RemoveShipFromBridgetown();
        //группы потереть
        LAi_group_Delete("EnemyFight");
        LAi_group_Delete("TmpEnemy");

        //прерывания снять
        if (CheckAttribute(PChar, "quest.CureLordMovie")) Pchar.quest.CureLordMovie.over = "yes";
        if (CheckAttribute(PChar, "quest.DragunInvansion")) Pchar.quest.DragunInvansion.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodLine_q1_1")) Pchar.quest.CapBloodLine_q1_1.over = "yes";
        if (CheckAttribute(PChar, "quest.onPlantation")) Pchar.quest.onPlantation.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodDetectBlades1")) Pchar.quest.CapBloodDetectBlades1.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodDetectBlades2")) Pchar.quest.CapBloodDetectBlades2.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodLineTimer_1")) Pchar.quest.CapBloodLineTimer_1.over = "yes";
        if (CheckAttribute(PChar, "quest.CureMisStid")) Pchar.quest.CureMisStid.over = "yes";
        if (CheckAttribute(PChar, "quest.PrepareToEscape2")) Pchar.quest.PrepareToEscape2.over = "yes";
        if (CheckAttribute(PChar, "quest.PrepareToEscape3")) Pchar.quest.PrepareToEscape3.over = "yes";
        if (CheckAttribute(PChar, "quest.WinterwoodDuel")) Pchar.quest.WinterwoodDuel.over = "yes";
        if (CheckAttribute(PChar, "quest.MoneyForDieke")) Pchar.quest.MoneyForDieke.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodLineTimer_2")) Pchar.quest.CapBloodLineTimer_2.over = "yes";
        if (CheckAttribute(PChar, "quest.QUsurer")) Pchar.quest.QUsurer.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodEscape1")) Pchar.quest.CapBloodEscape1.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodEscape2")) Pchar.quest.CapBloodEscape2.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodEscape3")) Pchar.quest.CapBloodEscape3.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodEscape4")) Pchar.quest.CapBloodEscape4.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodEscape5")) Pchar.quest.CapBloodEscape5.over = "yes";
        if (CheckAttribute(PChar, "quest.CapBloodEscape6")) Pchar.quest.CapBloodEscape6.over = "yes";
        if (CheckAttribute(PChar, "quest.SolderTakeBladesDie")) Pchar.quest.SolderTakeBladesDie.over = "yes";
        LAi_group_RemoveCheck("EnemyFight");
        //Квесты закрыть
        CloseQuestHeader("FishermanQuest");
        CloseQuestHeader("UsurerQuest");
        CloseQuestHeader("PirateQuest");

		UnlockAchievement("CapBladLine", 3);

}