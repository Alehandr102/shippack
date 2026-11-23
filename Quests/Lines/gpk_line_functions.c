// Gregg: вынесено из reaction_functions.c (сплит god-файла) — секция «Линейка ГПК» (главный сюжет City of Lost Ships).
// #include на исходное место — видимость и поведение идентичны.

void ISS_SpawnTrust(string qName)
{
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("PoorKillSponsor", "smuggler_boss", "man", "man_fast", 30, PIRATE, -1, false)); // LEO: Страдать превозмогаторам 07.12.2021
	else sld = GetCharacter(NPC_GenerateCharacter("PoorKillSponsor", "smuggler_boss", "man", "man", 30, PIRATE, -1, false));
	sld.name = GetConvertStr("Variable_LSC_Q2Utilite_5", "Names.txt");
	sld.lastname = GetConvertStr("Variable_LSC_Q2Utilite_6", "Names.txt");;
	sld.talker = 8; //начать диалог
	LAi_SetLoginTime(sld, 9.0, 21.99);
	sld.City = "PortRoyal";
	sld.nation = ENGLAND;
	sld.Dialog.Filename = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode = "Begin_PoorKill";
	sld.greeting = "Gr_OliverTrast";
	LAi_SetCitizenType(sld);
	LAi_SetImmortal(sld, true);
	LAi_SetHP(sld, 100.0, 100.0);
	sld.greeting = "Gr_OliverTrast";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	ChangeCharacterAddressGroup(sld, "PortRoyal_town", "goto", "goto2");
}

void ISS_MurderSignExecute(string qName)
{
	pchar.questTemp.LSC.killCost = sti(pchar.questTemp.LSC.killCost)-1;
	pchar.questTemp.LSC = "PoorMurderBegin";
	if (sti(pchar.questTemp.LSC.killCost) <= 0)
	{
		pchar.questTemp.LSC = "PoorMurderExecute";
		DeleteAttribute(pchar, "questTemp.LSC.killCost");
		SetQuestHeader("ISS_PoorsMurder");
		AddQuestRecord("ISS_PoorsMurder", "2");
	}
}

void LSC_enterStuvesantBox(string qName)
{
	AddQuestRecord("ISS_PoorsMurder", "5");
	AddQuestUserData("ISS_PoorsMurder", "sSex", GetSexPhrase("ел","ла"));
	pchar.questTemp.LSC = "toVillemstadTavern";
	//посадим воришку в таверну
	FreeSitLocator("Villemstad_tavern", "sit1");
	sld = GetCharacter(NPC_GenerateCharacter("GiveKeyMan", "shipowner_10", "man", "man", 10, HOLLAND, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_24", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_25", "Names.txt", "#space#", " ");
	sld.city = "Villemstad";
	sld.location	= "Villemstad_tavern";
	sld.location.group = "sit";
	sld.location.locator = "sit1";
	sld.dialog.filename   = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode   = "GiveKeyMan";
	sld.greeting = "Gr_Master";
	LAi_SetSitType(sld);
	LAi_RemoveLoginTime(sld);
	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
}

void LSC_openMasterHouse(string qName)
{
	LocatorReloadEnterDisable("Villemstad_town", "houseSp5", false);
	//делаем видимым ключ
	sld = ItemsFromID("keyQuestLSC");
	sld.shown = true;
	//Boyer change #20170418-01
	pchar.restoreLSCKeymasterKey = "keyQuestLSC";
	pchar.quest.LSC_enterMasterHouse.win_condition.l1 = "location";
	pchar.quest.LSC_enterMasterHouse.win_condition.l1.location = "Villemstad_houseSp5";
	pchar.quest.LSC_enterMasterHouse.function = "LSC_enterMasterHouse";
}

void LSC_enterMasterHouse(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_LocationFightDisable(&Locations[FindLocation("Villemstad_houseSp5")], true);
	//ставим Мастера Ключей
	sld = characterFromId("GiveKeyMan");
	sld.dialog.currentnode = "GiveKeyManInside";
	LAi_SetActorType(sld);
	ChangeCharacterAddressGroup(sld, "Villemstad_houseSp5", "barmen", "bar1");
    LAi_SetActorType(pchar);
    LAi_SetActorType(sld);
    SetActorDialogAny2Pchar("GiveKeyMan", "", 0.0, 1.0);
	LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
}

void LSC_enterSoldiers()
{
	LAi_SetPlayerType(pchar);
	LAi_group_Delete("EnemyFight");
	pchar.questTemp.LSC = "CanFoundStuvesantKey";
	sld = characterFromId("GiveKeyMan");
	LAi_CharacterDisableDialog(sld);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, LAI_GROUP_TmpEnemy);
	LAi_group_SetRelation(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	iTemp = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE - 2;
	//int iNum = sti((2+MOD_SKILL_ENEMY_RATE)/2);
	//Boyer mod #20170318-33
	int iNum = makeint(MOD_SKILL_ENEMY_RATE) + 1;
	for (i=1; i<=iNum; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter("MKSold"+i, "sold_hol_"+(rand(7)+1), "man", "man", iTemp, HOLLAND, 0, true));
		SetFantomParamFromRank(sld, iTemp, true);
		LAi_SetWarriorType(sld);
		//LAi_warrior_SetStay(sld, true);
		LAi_warrior_DialogEnable(sld, false);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto3");
    }
    sld = GetCharacter(NPC_GenerateCharacter("MKOfficer", "off_hol_2", "man", "man", iTemp, HOLLAND, 0, true));
	SetFantomParamFromRank(sld, iTemp, true);
	sld.Dialog.Filename = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode = "MK_HollOfficer";
	sld.greeting = "soldier_arest";
    LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload1");
	LAi_ActorDialog(sld, pchar, "", 1.0, 0);
}

void FirstLoginLostShipsCity(string qName) //первоначальная генерация нпс в ГПК
{
	//начальные настройки
	pchar.questTemp.LSC = "AdmiralIsWaiting";
	bDisableFastReload = true; //закроем фастрелоады
	//из письма делаем посление Тизера Дэна -->
	TakeItemFromCharacter(pchar, "letter_LSC");
	ChangeItemName("letter_LSC", "itmname_letter_LSC_2");
	ChangeItemDescribe("letter_LSC", "itmdescr_letter_LSC_2");
	i = FindLocation("LostShipsCity_town");
	locations[i].private11.items.letter_LSC = 1;
	//восстанавливаем флаги и пр. в игре вне ГПК
	CloseQuestHeader("ISS_PoorsMurder");
	pchar.questdata.UbitVsehNishih = 1;
	sld = characterFromId("hol_guber"); //чтобы мог переезжать
	DeleteAttribute(sld, "notMoveAble");
	DeleteAttribute(&colonies[FindColony("Villemstad")], "notCaptured"); //Виллемстад можно себе
	LAi_group_Delete("TmpEnemy"); //группа граждан ГПК
	LAi_group_Delete(LAI_GROUP_TmpEnemy); //группа граждан которых можно валить безнаказанно
	//группы кланов -->
	LAi_group_Delete("PearlGroup_1"); //для кланов заюзаем пока группы жемчужников
	LAi_group_Register("PearlGroup_1");
	LAi_group_SetRelation("PearlGroup_1", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_SetLookRadius("PearlGroup_1", 50);
	LAi_group_SetHearRadius("PearlGroup_1", 20);
	LAi_group_SetSayRadius("PearlGroup_1", 20);
	LAi_group_SetPriority(LAI_GROUP_PLAYER, 0);
	LAi_group_Delete("PearlGroup_2");
	LAi_group_Register("PearlGroup_2");
	LAi_group_SetRelation("PearlGroup_2", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_SetLookRadius("PearlGroup_2", 50);
	LAi_group_SetHearRadius("PearlGroup_2", 20);
	LAi_group_SetSayRadius("PearlGroup_2", 20);
	LAi_group_SetPriority(LAI_GROUP_PLAYER, 0);
	//<-- группы кланов
	//снять шторм с острова ГПК
	reload_cur_island_index = FindIsland("LostShipsCity"); //to_do, в релиз убрать
	DeleteAttribute(&Islands[reload_cur_island_index], "alwaysStorm");
	DeleteAttribute(&Islands[reload_cur_island_index], "storm");
	DeleteAttribute(&Islands[reload_cur_island_index], "tornado");
	DeleteAttribute(&Islands[reload_cur_island_index], "QuestlockWeather");
	DeleteAttribute(&Islands[reload_cur_island_index], "MaxSeaHeight");
	//сносим все корабли и компаньонов
	int cn;
	for (i=0; i<=COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if(cn != -1)
		{
			sld = &characters[cn];
			if (i == 0)
			{	//снимаем корабль ГГ
				DeleteAttribute(pchar, "ship");
				pchar.ship.name = "";
				pchar.ship.type = SHIP_NOTUSED;
				SetCharacterShipLocation(pchar, "");
			}
			else
			{
				RemoveCharacterCompanion(pchar, sld);
				sld.lifeDay = 0;
				LAi_KillCharacter(sld);
				i--;
			}
		}
	}
	//сносим всех пассажиров и офицеров
	for (i=0; i<=PASSENGERS_MAX; i++)
	{
		cn = GetPassenger(pchar, i);
		if(cn != -1)
		{
			sld = &characters[cn];
			RemovePassenger(pchar, sld);
			sld.lifeDay = 0;
			LAi_KillCharacter(sld);
			i--;
		}
	}
	//адмирала на стрем, чтобы ГГ долго не гулял
	//#20170629-01 Dialog/follow bug in Passenger quest
    DeleteAttribute(pchar, "tempDlgInterruptPass"));
	PChar.quest.LSC_stormTimer_1.win_condition.l1            = "Timer";
	PChar.quest.LSC_stormTimer_1.win_condition.l1.date.hour  = 23;
    PChar.quest.LSC_stormTimer_1.win_condition.l1.date.day   = GetAddingDataDay(0, 0, 1);
    PChar.quest.LSC_stormTimer_1.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 0);
    PChar.quest.LSC_stormTimer_1.win_condition.l1.date.year  = GetAddingDataYear(0, 0, 0);
    PChar.quest.LSC_stormTimer_1.function = "LSC_stormTimer_1";
	//Начальные слухи
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_30", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_31", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");

	//мэр-адмирал
	sld = GetCharacter(NPC_GenerateCharacter("LSCMayor", "capitan_2", "man", "man", 50, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 50, 90, 90, "blade33", "pistol4", 200);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_26", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_27", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location = "SanAugustineResidence";
	sld.location.group = "sit";
	sld.location.locator = "sit1";
	sld.location.baseShip = "11"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto11_0";
	sld.location.baseShip.going = false; //перемещаться между кораблями в городе
	sld.location.hours = 5; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\Mayor.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "mayor"; //тип нпс
	sld.greeting = "Gr_Kapper";
	sld.standUp = true; //вставать и нападать на врага
	sld.watchBoxes = true;
	sld.standUp.exitDisable = true;   //залочить выход в случае драки
	sld.watchBoxes.exitDisable = true; //залочить выход в случае драки
	sld.HeroModel = "capitan_2,capitan_2_1,capitan_2_2,capitan_2_3,capitan_2_4,capitan_2_5";
	TakeNItems(sld, "key_mechanic", 1);
	//sld.talker = true; //начать диалог
	LAi_SetHuberType(sld);
	LAi_RemoveLoginTime(sld);
	LAi_SetImmortal(sld, true);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// миллиционер №1
	sld = GetCharacter(NPC_GenerateCharacter("Ment_1", "officer_25", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade21", "pistol4", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_28", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_29", "Names.txt", "#space#", " ");
	sld.rank = 30;
	sld.city = "LostShipsCity";
	sld.location	= "SanAugustineResidence";
	sld.location.group = "goto";
	sld.location.locator = "goto9";
	sld.location.baseShip = "11"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto11_1";
	sld.location.baseShip.going = false; //перемещаться между кораблями в городе
	sld.location.hours = 4; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\Mentos.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "ment"; //тип нпс
	sld.greeting = "pirat_common";
	sld.talker = true; //начать диалог
	sld.watchBoxes = true;
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// миллиционер №2
	sld = GetCharacter(NPC_GenerateCharacter("Ment_2", "off_fra_1", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade25", "pistol4", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_30", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_31", "Names.txt", "#space#", " ");
	sld.rank = 30;
	sld.city = "LostShipsCity";
	sld.location	= "SanAugustineResidence";
	sld.location.group = "goto";
	sld.location.locator = "goto8";
	sld.location.baseShip = "11"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto11_2";
	sld.location.baseShip.going = false; //перемещаться между кораблями в городе
	sld.location.hours = 2; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\Mentos.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "ment"; //тип нпс
	sld.greeting = "pirat_common";
	sld.watchBoxes = true;
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// миллиционер №3
	sld = GetCharacter(NPC_GenerateCharacter("Ment_3", "sold_hol_1", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade22", "pistol4", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_32", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_33", "Names.txt", "#space#", " ");
	sld.rank = 30;
	sld.city = "LostShipsCity";
	sld.location	= "SanAugustineResidence";
	sld.location.group = "goto";
	sld.location.locator = "goto11";
	sld.location.baseShip = "11"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto11_3";
	sld.location.baseShip.going = false; //перемещаться между кораблями в городе
	sld.location.hours = 6; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\Mentos.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "ment"; //тип нпс
	sld.greeting = "pirat_common";
	sld.talker = true; //начать диалог
	sld.watchBoxes = true;
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// миллиционер №4
	sld = GetCharacter(NPC_GenerateCharacter("Ment_4", "off_spa_2", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade24", "pistol4", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_34", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_35", "Names.txt", "#space#", " ");
	sld.rank = 30;
	sld.city = "LostShipsCity";
	sld.location	= "SanAugustineResidence";
	sld.location.group = "goto";
	sld.location.locator = "goto4";
	sld.location.baseShip = "11"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto11_4";
	sld.location.baseShip.going = false; //перемещаться между кораблями в городе
	sld.location.hours = 3; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\Mentos.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "ment"; //тип нпс
	sld.greeting = "pirat_common";
	sld.watchBoxes = true;
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// миллиционер №5
	sld = GetCharacter(NPC_GenerateCharacter("Ment_5", "officer_8", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade33", "pistol4", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_36", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_37", "Names.txt", "#space#", " ");
	sld.rank = 30;
	sld.city = "LostShipsCity";
	sld.location	= "SanAugustineResidence";
	sld.location.group = "goto";
	sld.location.locator = "goto3";
	sld.location.baseShip = "11"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto11_5";
	sld.location.baseShip.going = false; //перемещаться между кораблями в городе
	sld.location.hours = 5; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\Mentos.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "ment"; //тип нпс
	sld.greeting = "pirat_common";
	sld.watchBoxes = true;
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// миллиционер №6
	sld = GetCharacter(NPC_GenerateCharacter("Ment_6", "Dragoon_1", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "topor2", "pistol4", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_38", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_39", "Names.txt", "#space#", " ");
	sld.rank = 30;
	// Прописка всех моделей для кирас. (Увы, но моделям на основе драгуна можно только 1.3.2 кирасы) -->
	sld.HeroModel = "Dragoon_1,Dragoon_2,Dragoon_3,Dragoon_4,Dragoon_5,Dragoon_6,Dragoon_1,Dragoon_1,Dragoon_1";
	// Прописка всех моделей для кирас. (Увы, но моделям на основе драгуна можно только 1.3.2 кирасы) <--
	sld.city = "LostShipsCity";
	sld.location	= "SanAugustineResidence";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.location.baseShip = "11"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto11_6";
	sld.location.baseShip.going = false; //перемещаться между кораблями в городе
	sld.location.hours = 7; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\Mentos.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "ment"; //тип нпс
	sld.talker = true; //начать диалог
	sld.watchBoxes = true;
	sld.greeting = "pirat_common";
	SetSPECIAL(sld, 10, 4, 9, 6, 8, 9, 7);
	SetSelfSkill(sld, 100, 100, 100, 30, 90);
	SetShipSkill(sld, 10, 10, 5, 20, 20, 30, 10, 10, 60);
	SetCharacterPerk(sld, "AdvancedDefense");
	SetCharacterPerk(sld, "CriticalHit");
	SetCharacterPerk(sld, "Ciras");
	//SetCharacterPerk(sld, "Grus");
	SetCharacterPerk(sld, "Rush");
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "Sliding");
	LAi_SetWarriorType(sld);
	SetFantomHP(sld); // LEO: ХП по силе, выносливости и рангу; перк растущей жизни докладывает Sync внутри
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// ------------------------ таверна ------------------------
	//тавернщик
	sld = GetCharacter(NPC_GenerateCharacter("LSCBarmen", "barmen_8", "man", "man", 20, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_40", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_41", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.dialog.filename   = "Quest\LostShipsCity\Barmen.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "tavern"; //тип нпс
	sld.greeting = "Gr_Barmen";
	LAi_SetBarmanType(sld);
	LAi_RemoveLoginTime(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	//официантка
	sld = GetCharacter(NPC_GenerateCharacter("LSCwaitress", "CitizenWom_1", "woman", "towngirl", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_42", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_43", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "waitress";
	sld.location.locator = "barmen";
	sld.dialog.filename   = "Quest\LostShipsCity\Waitress.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "tavern"; //тип нпс
	sld.greeting = "Armo_Dulin";
	LAi_SetWaitressType(sld);
	LAi_RemoveLoginTime(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// ------------------------ внешние горожане ------------------------
	// внешний горожанин Педро Хуртадо
	sld = GetCharacter(NPC_GenerateCharacter("PedroHurtado", "usurer_2", "man", "man", 10, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_44", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_45", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "LostShipsCity_town";
	sld.location.group = "goto";
	sld.location.locator = "goto06_1";
	sld.location.baseShip = "06"; //базовый корабль в ГПК
	sld.location.baseLocator = sld.location.locator;
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 2; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\PedroHurtado.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	sld.talker = true; //начать диалог
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetLSCoutType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внешний горожанин Жиль Бару
	sld = GetCharacter(NPC_GenerateCharacter("ZhilBaru", "trader_16", "man", "man", 10, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_46", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_47", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "LostShipsCity_town";
	sld.location.group = "goto";
	sld.location.locator = "goto07_1";
	sld.location.baseShip = "07"; //базовый корабль в ГПК
	sld.location.baseLocator = sld.location.locator;
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 1; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\ZhilBaru.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	sld.talker = true; //начать диалог
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetLSCoutType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внешний горожанин Тристан Борчер
	sld = GetCharacter(NPC_GenerateCharacter("TristanBorcher", "citiz_19", "man", "man", 10, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_48", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_49", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "LostShipsCity_town";
	sld.location.group = "goto";
	sld.location.locator = "goto09_1";
	sld.location.baseShip = "09"; //базовый корабль в ГПК
	sld.location.baseLocator = sld.location.locator;
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 3; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\TristanBorcher.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	sld.talker = true; //начать диалог
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetLSCoutType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внешний горожанин Ройел Хааг
	sld = GetCharacter(NPC_GenerateCharacter("RoyelHaag", "trader_11", "man", "man", 10, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_50", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_51", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "LostShipsCity_town";
	sld.location.group = "goto";
	sld.location.locator = "goto14_1";
	sld.location.baseShip = "14"; //базовый корабль в ГПК
	sld.location.baseLocator = sld.location.locator;
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 4; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\RoyelHaag.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	sld.talker = true; //начать диалог
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetLSCoutType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внешний горожанин Доминик Легро
	sld = GetCharacter(NPC_GenerateCharacter("DominicLegro", "trader_10", "man", "man", 10, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_52", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_53", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "LostShipsCity_town";
	sld.location.group = "goto";
	sld.location.locator = "goto15_1";
	sld.location.baseShip = "15"; //базовый корабль в ГПК
	sld.location.baseLocator = sld.location.locator;
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 4; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\DiminicLegro.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	sld.talker = true; //начать диалог
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetLSCoutType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внешний горожанин Джорж Стоукс
	sld = GetCharacter(NPC_GenerateCharacter("JorgeStouks", "barmen_5", "man", "man", 10, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_54", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_55", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "LostShipsCity_town";
	sld.location.group = "goto";
	sld.location.locator = "goto08_1";
	sld.location.baseShip = "08"; //базовый корабль в ГПК
	sld.location.baseLocator = sld.location.locator;
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 4; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\JorgeStouks.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "Gr_JorgeStouks";
	sld.talker = true; //начать диалог
	LAi_RemoveLoginTime(sld);
	LAi_SetStayType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");
	ChangeCharacterAddress(sld, "LostShipsCity_town", "goto16_3");

	// ------------------------ внешние горожане в таверне ------------------------
	// в таверне Генри Фолдер
	sld = GetCharacter(NPC_GenerateCharacter("HenryFolder", "barmen_14", "man", "man", 10, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_56", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_57", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "sit";
	sld.location.locator = "sit2";
	sld.location.baseShip = "03"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto03_2";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 6; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\HenryFolder.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	LAi_SetSitType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// в таверне Феликс Мендоза
	sld = GetCharacter(NPC_GenerateCharacter("FelixMendoza", "shipowner_1", "man", "man", 10, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_58", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_59", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "sit";
	sld.location.locator = "sit3";
	sld.location.baseShip = "10"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto10_2";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 5; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\FelixMendoza.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	LAi_SetSitType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// в таверне Роджер Воулер
	sld = GetCharacter(NPC_GenerateCharacter("RodgerWoyler", "usurer_5", "man", "man", 10, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_60", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_61", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "sit";
	sld.location.locator = "sit5";
	sld.location.baseShip = "12"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto12_2";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 7; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\RodgerWoyler.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	LAi_SetSitType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// в таверне Андрэ Лабор
	sld = GetCharacter(NPC_GenerateCharacter("AndreLabor", "shipowner_15", "man", "man", 15, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 15, 60, 60, "blade20", "pistol2", 10);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_62", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_63", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "sit";
	sld.location.locator = "sit9";
	sld.location.baseShip = "15"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto15_2";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 4; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\AndreLabor.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "Gr_Master";
	LAi_SetSitType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// в таверне Стефан Клейн
	sld = GetCharacter(NPC_GenerateCharacter("StefanKleyn", "shipowner_6", "man", "man", 10, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_64", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_65", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "sit";
	sld.location.locator = "sit8";
	sld.location.baseShip = "14"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto14_2";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 3; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\StefanKleyn.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	LAi_SetSitType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// в таверне Томас Фланн
	sld = GetCharacter(NPC_GenerateCharacter("ThomasFlann", "citiz_1", "man", "man", 10, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_66", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_67", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "sit";
	sld.location.locator = "sit6";
	sld.location.baseShip = "02"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto02_2";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 3; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\ThomasFlann.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	sld.talker = true; //начать диалог
	LAi_SetSitType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// в таверне Пабло Барадес
	sld = GetCharacter(NPC_GenerateCharacter("PabloBarades", "officer_20", "man", "man", 10, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_68", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_69", "Names.txt", "#space#", " ");
	sld.rank = 10;
	sld.city = "LostShipsCity";
	sld.location	= "FleuronTavern";
	sld.location.group = "sit";
	sld.location.locator = "sit14";
	sld.location.baseShip = "09"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto09_2";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 5; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\PabloBarades.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "cit_common";
	LAi_SetSitType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// ------------------------ внешние горожане в церкви ------------------------
	// падре
	sld = GetCharacter(NPC_GenerateCharacter("LSC_Priest", "priest_3", "man", "man", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_70", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_71", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "GloriaChurch";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.dialog.filename   = "Quest\LostShipsCity\Priest.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "priest"; //тип нпс
	sld.greeting = "Gr_padre";
	LAi_SetPriestType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");

	// в церкви Сандра Гомес
	sld = GetCharacter(NPC_GenerateCharacter("SandraGomes", "girl_4", "woman", "towngirl", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_72", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_73", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "GloriaChurch";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.location.baseShip = "04"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto04_3";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 4; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\SandraGomes.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "Gr_Woman_Citizen";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// в церкви Моника Райт
	sld = GetCharacter(NPC_GenerateCharacter("MonikaRight", "girl_5", "woman", "towngirl", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_74", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_75", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "GloriaChurch";
	sld.location.group = "goto";
	sld.location.locator = "goto2";
	sld.location.baseShip = "08"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto08_3";
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 5; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\MonikaRight.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "Gr_Woman_Citizen";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// ------------------------ магазин ------------------------
	sld = GetCharacter(NPC_GenerateCharacter("LSC_Trader", "trader_5", "man", "man", 20, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_76", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_77", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "EsmeraldaStoreBig";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.dialog.filename   = "Quest\LostShipsCity\Trader.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "trader"; //тип нпс
	sld.greeting = "tra_common";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetOwnerType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");

	// ------------------------ Тюрьма ------------------------
	sld = GetCharacter(NPC_GenerateCharacter("LSC_PrisonerHead", "Hobart", "man", "man", 40, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 40, 87, 87, "blade32", "pistol4", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_78", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_79", "Names.txt", "#space#", " ");
	sld.rank = 40;
	sld.city = "LostShipsCity";
	sld.location = "TartarusPrison";
	sld.location.group = "sit";
	sld.location.locator = "sit1";
	sld.dialog.filename   = "Quest\LostShipsCity\Prisoner.c";
	sld.dialog.currentnode   = "prisonerHead";
	sld.cityTape = "prisonerHead"; //тип нпс
	sld.greeting = "pirat_common";
	sld.standUp = true; //вставать и нападать на врага
	sld.watchBoxes = true;
	sld.location.days = 2; //передвижения через .. дней
	//sld.talker = true; //начать диалог
	LAi_SetHuberType(sld);
	LAi_RemoveLoginTime(sld);
	LAi_SetImmortal(sld, true);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// тюремщик №1
	sld = GetCharacter(NPC_GenerateCharacter("LSC_Prisoner1", "SoldierEnglish_1", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade21", "pistol3", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_80", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_81", "Names.txt", "#space#", " ");
	sld.rank = 30;
	sld.city = "LostShipsCity";
	sld.location	= "TartarusPrison";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.dialog.filename   = "Quest\LostShipsCity\Prisoner.c";
	sld.dialog.currentnode   = "Prisoner";
	sld.cityTape = "prisoner"; //тип нпс
	sld.greeting = "pirat_common";
	sld.standUp = true; //вставать и нападать на врага
	sld.HeroModel = "SoldierEnglish_1,SoldierEnglish_1_1,SoldierEnglish_1_2,SoldierEnglish_1_3,SoldierEnglish_1_4,SoldierEnglish_1_5";
	sld.watchBoxes = true;
	sld.location.hours = 4; //передвижения через .. часов
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// ------------------------ Банкир ------------------------
	sld = GetCharacter(NPC_GenerateCharacter("LSC_Usurer", "trader_9", "man", "man", 30, PIRATE, -1, true));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_82", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_83", "Names.txt", "#space#", " ");
	sld.rank = 30;
	sld.city = "LostShipsCity";
	sld.location	= "CarolineBank";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.dialog.filename   = "Quest\LostShipsCity\Usurer.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "usurer"; //тип нпс
	sld.greeting = "cit_common";
	LAi_SetImmortal(sld, true);
	LAi_RemoveLoginTime(sld);
	LAi_SetOwnerType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");

	// ------------------------ Механик ------------------------
	sld = GetCharacter(NPC_GenerateCharacter("Mechanic", "CCS_Mechanic", "man", "man", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_84", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_85", "Names.txt", "#space#", " ");
	sld.rank = 5;
	// Прописка всех моделей для кирас. -->
	sld.HeroModel = "CCS_Mechanic,CCS_Mechanic_1,CCS_Mechanic_2,CCS_Mechanic_3,CCS_Mechanic_4,CCS_Mechanic_5";
	// Прописка всех моделей для кирас. <--
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.dialog.filename   = "Quest\LostShipsCity\Mechanic.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "mechanic"; //тип нпс
	sld.greeting = "Gr_Master";
	TakeNItems(sld, "talisman7", 1);
	LAi_RemoveLoginTime(sld);
	LAi_SetOwnerType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");

	// ------------------------ внутренние горожане ------------------------
	// внутреняя горожанка Элис Тейлор
	sld = GetCharacter(NPC_GenerateCharacter("ElisTaylor", "girl_3", "woman", "towngirl", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_86", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_87", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "CeresSmithy";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.location.baseShip = "03"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto03_0";
	sld.location.baseLocation = "CeresSmithy"; //базовая внутреняя локация
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 4; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\ElisTaylor.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizenHome"; //тип нпс
	sld.greeting = "Gr_Woman_Citizen";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetOwnerType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внутреняя горожанка Орели Бертин
	sld = GetCharacter(NPC_GenerateCharacter("OreliBertin", "girl_6", "woman", "towngirl", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_88", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_89", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "FurieShipInside2";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.location.baseShip = "06"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto06_0";
	sld.location.baseLocation = "FurieShipInside2"; //базовая внутреняя локация
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 6; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\OreliBertin.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizenHome"; //тип нпс
	sld.greeting = "Gr_Woman_Citizen";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetOwnerType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внутреняя горожанка Леа Тоорс
	sld = GetCharacter(NPC_GenerateCharacter("LeaToors", "girl_7", "woman", "towngirl", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_90", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_91", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "PlutoStoreSmall";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.location.baseShip = "09"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto09_0";
	sld.location.baseLocation = "PlutoStoreSmall"; //базовая внутреняя локация
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 5; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\LeaToors.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizenHome"; //тип нпс
	sld.greeting = "Gr_Woman_Citizen";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetOwnerType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внутреняя горожанка Элиза Кальво
	sld = GetCharacter(NPC_GenerateCharacter("ElizaCalvo", "girl_8", "woman", "towngirl", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_92", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_93", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "SantaFlorentinaShipInside4";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.location.baseShip = "14"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto14_0";
	sld.location.baseLocation = "SantaFlorentinaShipInside4"; //базовая внутреняя локация
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 7; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\ElizaCalvo.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizenHome"; //тип нпс
	sld.greeting = "Gr_Woman_Citizen";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetOwnerType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	// внутреняя горожанка Сесиль Галард
	sld = GetCharacter(NPC_GenerateCharacter("SesilGalard", "BaynesWife", "woman", "towngirl", 5, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_94", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_95", "Names.txt", "#space#", " ");
	sld.rank = 5;
	sld.city = "LostShipsCity";
	sld.location	= "AvaShipInside3";
	sld.location.group = "barmen";
	sld.location.locator = "stay";
	sld.location.baseShip = "15"; //базовый корабль в ГПК
	sld.location.baseLocator = "goto15_0";
	sld.location.baseLocation = "AvaShipInside3"; //базовая внутреняя локация
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 5; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\SesilGalard.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizenHome"; //тип нпс
	sld.greeting = "Gr_SesilGalard";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetOwnerType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");

	//------------------- Клан Нарвал -------------------
	// нарвал главарь
	sld = GetCharacter(NPC_GenerateCharacter("Narval_head", "capitan_3", "man", "man", 35, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 35, 100, 100, "blade28", "pistol11", 200);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_96", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_97", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "VelascoShipInside1";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.dialog.filename   = "Quest\LostShipsCity\Narval.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "narval"; //тип нпс
	sld.greeting = "pirat_common";
	sld.HeroModel = "capitan_3,capitan_3_1,capitan_3_2,capitan_3_3,capitan_3_4,capitan_3_5";
	LAi_SetWarriorType(sld);
	LAi_SetImmortal(sld, true);
	LAi_group_MoveCharacter(sld, "PearlGroup_1");
	SaveCurrentNpcQuestDateParam(sld, "location"); //главе клана сделаем перемещения
	// нарвал №1
	sld = GetCharacter(NPC_GenerateCharacter("Narval_1", "officer_24", "man", "man", 20, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 25, 60, 60, "blade21", "pistol11", 10);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_98", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_99", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "VelascoShipInside1";
	sld.location.group = "goto";
	sld.location.locator = "goto2";
	sld.dialog.filename   = "Quest\LostShipsCity\Narval.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "narval"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_1");
	// нарвал №2
	sld = GetCharacter(NPC_GenerateCharacter("Narval_2", "officer_26", "man", "man", 20, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 26, 80, 80, "blade34", "pistol3", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_100", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_101", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "VelascoShipInside1";
	sld.location.group = "goto";
	sld.location.locator = "goto3";
	sld.dialog.filename   = "Quest\LostShipsCity\Narval.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "narval"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_1");
	// нарвал №3
	sld = GetCharacter(NPC_GenerateCharacter("Narval_3", "trader_2", "man", "man", 20, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 24, 70, 70, "blade25", "pistol5", 50);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_102", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_103", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "VelascoShipInside1";
	sld.location.group = "goto";
	sld.location.locator = "goto4";
	sld.dialog.filename   = "Quest\LostShipsCity\Narval.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "narval"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_1");
	// нарвал №4
	sld = GetCharacter(NPC_GenerateCharacter("Narval_4", "pirate_17", "man", "man", 25, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 25, 85, 85, "blade33", "pistol11", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_104", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_105", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "VelascoShipInside1";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\LostShipsCity\Narval.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "narval"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_1");
	// нарвал №5
	sld = GetCharacter(NPC_GenerateCharacter("Narval_5", "pirate_13", "man", "man", 20, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 27, 80, 80, "topor2", "pistol3", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_106", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_107", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "VelascoShipInside1";
	sld.location.group = "goto";
	sld.location.locator = "goto6";
	sld.dialog.filename   = "Quest\LostShipsCity\Narval.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "narval"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_1");
	// нарвал №6
	sld = GetCharacter(NPC_GenerateCharacter("Narval_6", "pirate_14", "man", "man", 22, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 22, 65, 65, "blade22", "pistol11", 10);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_108", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_109", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "VelascoShipInside1";
	sld.location.group = "goto";
	sld.location.locator = "goto7";
	sld.dialog.filename   = "Quest\LostShipsCity\Narval.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "narval"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_1");
	// нарвал №7
	sld = GetCharacter(NPC_GenerateCharacter("Narval_7", "officer_6", "man", "man", 25, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 25, 85, 85, "blade23", "pistol4", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_110", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_111", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "VelascoShipInside1";
	sld.location.group = "goto";
	sld.location.locator = "goto8";
	sld.dialog.filename   = "Quest\LostShipsCity\Narval.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "narval"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_1");

	//------------------- Клан Каспер -------------------
	// каспер главарь
	sld = GetCharacter(NPC_GenerateCharacter("Casper_head", "shipowner_24", "man", "man", 35, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 35, 100, 100, "blade28", "pistol5", 200);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_112", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_113", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto8"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_SetImmortal(sld, true);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//SaveCurrentNpcQuestDateParam(sld, "location"); //главе клана сделаем перемещения
	//каспер №1
	sld = GetCharacter(NPC_GenerateCharacter("Casper_1", "pirate_19", "man", "man", 25, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 25, 80, 80, "topor2", "pistol3", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_114", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_115", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto4"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetWarriorType(sld);
	LAi_SetImmortal(sld, true);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №2
	sld = GetCharacter(NPC_GenerateCharacter("Casper_2", "pirate_18", "man", "man", 21, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 21, 80, 80, "blade26", "pistol11", 120);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_116", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_117", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto6"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №3
	sld = GetCharacter(NPC_GenerateCharacter("Casper_3", "pirate_20", "man", "man", 25, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 27, 80, 80, "blade34", "pistol11", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_118", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_119", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto5"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №4
	sld = GetCharacter(NPC_GenerateCharacter("Casper_4", "pirate_12", "man", "man", 25, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 25, 80, 80, "blade23", "pistol3", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_120", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_121", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto7"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №5
	sld = GetCharacter(NPC_GenerateCharacter("Casper_5", "officer_20", "man", "man", 25, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 27, 80, 80, "topor2", "pistol3", 130);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_122", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_123", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto13"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №6
	sld = GetCharacter(NPC_GenerateCharacter("Casper_6", "officer_2", "man", "man", 29, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 29, 80, 80, "blade25", "pistol11", 100);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_124", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_125", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto1" //;
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №7
	sld = GetCharacter(NPC_GenerateCharacter("Casper_7", "officer_4", "man", "man", 25, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 25, 80, 80, "blade34", "pistol11", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_126", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_127", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto2"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №8
	sld = GetCharacter(NPC_GenerateCharacter("Casper_8", "officer_34", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade26", "pistol11", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_128", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_129", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto3"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №9
	sld = GetCharacter(NPC_GenerateCharacter("Casper_9", "officer_10", "man", "man", 30, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade28", "pistol11", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_130", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_131", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto10"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №10
	sld = GetCharacter(NPC_GenerateCharacter("Casper_10", "officer_6", "man", "man", 25, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 25, 80, 80, "blade28", "pistol3", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_132", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_133", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto11"; //
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	//каспер №11
	sld = GetCharacter(NPC_GenerateCharacter("Casper_11", "officer_20", "man", "man", 27, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 27, 80, 80, "blade28", "pistol11", 250);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_134", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_135", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "SanGabrielMechanic";
	sld.location.group = "goto";
	sld.location.locator = "goto12";
	sld.dialog.filename   = "Quest\LostShipsCity\Casper.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "casper"; //тип нпс
	sld.greeting = "pirat_common";
	LAi_SetImmortal(sld, true);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");

	// крабик

	sld = GetCharacter(NPC_GenerateCharacter("pet_crab", "crabSmall", "crab", "crabSmall", 25, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_136", "Names.txt", "#space#", " ");
	sld.lastname = "";
	GiveItem2Character(sld, "unarmed");
	EquipCharacterbyItem(sld, "unarmed");;
	sld.location	= "none";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.dialog.filename   = "Quest\LostShipsCity\Crab.c";
	sld.dialog.currentnode   = "First time";
	LAi_SetLSCoutType(sld);
	LAi_group_MoveCharacter(sld, "pet_crab");
    //Квестовые горожане из ККС

	// внешний горожанин Ришард Шамбон - контрабандист (Квест "Тайна Санта-Люсии")
	sld = GetCharacter(NPC_GenerateCharacter("LSC_Rishard", "Shambone", "man", "man", 17, PIRATE, -1, false));
	SetFantomParamFromRank(sld, 17, true);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_137", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_138", "Names.txt", "#space#", " ");
	sld.city = "LostShipsCity";
	sld.location	= "LostShipsCity_town";
	sld.location.group = "goto";
	sld.location.locator = "goto08_1";
	sld.location.baseShip = "08"; //базовый корабль в ГПК - Глория
	sld.location.baseLocator = sld.location.locator;
	sld.location.baseShip.going = true; //перемещаться между кораблями в городе
	sld.location.hours = 4; //передвижения через .. часов
	sld.dialog.filename   = "Quest\LostShipsCity\Rishard.c";
	sld.dialog.currentnode   = "First time";
	sld.cityTape = "citizen"; //тип нпс
	sld.greeting = "pirat_quest";
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetLSCoutType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	SaveCurrentNpcQuestDateParam(sld, "location");
}

//снять шторм
void LSC_stormTimer_1(string qName)
{
	for(i=0; i<MAX_LOCATIONS; i++)
	{
		if (CheckAttribute(&locations[i], "fastreload") && locations[i].fastreload == "LostShipsCity")
		{
			DeleteAttribute(&locations[i], "alwaysStorm");
			DeleteAttribute(&locations[i], "storm");
			DeleteAttribute(&locations[i], "tornado");
			DeleteAttribute(&locations[i], "QuestlockWeather");
			DeleteAttribute(&locations[i], "MaxWaveHeigh");
		}
	}
	//адмирал ловит ГГ, если тот ещё не явился к нему
	if (pchar.questTemp.LSC == "AdmiralIsWaiting")
	{
		pchar.quest.LSC_admiralOwnFind.win_condition.l1 = "location";
		pchar.quest.LSC_admiralOwnFind.win_condition.l1.location = "LostShipsCity_town";
		pchar.quest.LSC_admiralOwnFind.function = "LSC_admiralOwnFind";
	}
}

void LSC_admiralOwnFind(string qName)
{
	chrDisableReloadToLocation = true;
	DoQuestFunctionDelay("LSC_admiralOwnFind_1", 1+rand(2));
}

void LSC_admiralOwnFind_1(string qName)
{
	sld = characterFromId("LSCMayor");
	sld.dialog.currentnode = "FoundHero";
	PlaceCharacter(sld, "goto", "random_must_be_near");
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", 2.0, 0);
}

void LSC_admiralTakeAll()
{
	chrDisableReloadToLocation = false;
	//установим типовые ключи -->
	sld = ItemsFromID("key1");
	sld.shown = true;
	sld.startLocation = "LostShipsCity_town";
	sld.startLocator = "item1";
	//--
	sld = ItemsFromID("key2");
	sld.shown = true;
	sld.startLocation = "VelascoShipInside1";
	sld.startLocator = "item1";
	//--
	sld = ItemsFromID("key3");
	sld.shown = true;
	sld.startLocation = "EsmeraldaStoreBig";
	sld.startLocator = "item1";
	//откроем кланы
	ref rLoc = &locations[FindLocation("LostShipsCity_town")];
	DeleteAttribute(rLoc, "reload.l61.disable");
	DeleteAttribute(rLoc, "reload.l70.disable");
	DeleteAttribute(rLoc, "reload.l72.disable");
	pchar.questTemp.LSC = "AfterAdmiral";
	StoreEquippedMaps(pchar); // ugeen --> сохраним карты из атласа дабы Чад не прибрал
	RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
	RemoveCharacterEquip(pchar, SPYGLASS_ITEM_TYPE);
	RemoveCharacterEquip(pchar, PATENT_ITEM_TYPE);
	RemoveCharacterEquip(pchar, CIRASS_ITEM_TYPE);
	RemoveCharacterEquip(pchar, MAPS_ITEM_TYPE);
	pchar.MapsAtlasCount = 0;
	//сносим все предметы, кроме квестовых -->
    aref arItems;
	string sName;
    makearef(arItems, pchar.items);
    int	Qty = GetAttributesNum(arItems);
    for (int a=0; a<Qty; a++)
    {
        sName = GetAttributeName(GetAttributeN(arItems, a));
		rLoc = ItemsFromID(sName);
		if (rLoc.ItemType == "QUESTITEMS")
		{
			pchar.questTemp.items.(sName) = pchar.items.(sName);
		}
    }
	DeleteAttribute(pchar, "items");
	pchar.items = "";
	makearef(arItems, pchar.questTemp.items);
    Qty = GetAttributesNum(arItems);
    for (a=0; a<Qty; a++)
    {
        sName = GetAttributeName(GetAttributeN(arItems, a));
		TakeNItems(pchar, sName, sti(arItems.(sName)));
    }
	DeleteAttribute(pchar, "questTemp.items");
	if(CheckCharacterItem(pchar, "MapsAtlas"))
	{
		EquipCharacterByAtlas(pchar);	// экипируем атласом, если он есть
		RestoreEquippedMaps(pchar); 	// вернем карты в атлас в 1 экземпляре, все остальное Чадушке или в сундук Тизера
		RefreshEquippedMaps(pchar);
	}
	//<-- сносим все предметы, кроме квестовых
	//проверим, не залезал и ГГ в сундучок Тизера -->
	int iLoc = FindLocation("LostShipsCity_town");
	if (CheckAttribute(&locations[iLoc], "private11.equip"))
	{
		DeleteAttribute(&locations[iLoc], "private11.money"); //если залезал, то заберем денюжки, хи-хи...
	}
	//<-- проверим, не залезал и ГГ в сундучок Тизера
	AddQuestRecord("ISS_MainLine", "3");
	AddQuestUserData("ISS_MainLine", "sSex", GetSexPhrase("я гол, как сокол","у меня ничего нет"));
	AddQuestUserData("ISS_MainLine", "sSex1", GetSexPhrase("ин","ка"));
	//------->>> пул нужных слухов для наводок на квесты		AddSimpleRumourCityTip("", "LostShipsCity", 10, 1, "LSC", "");
	//освобождение мужа Элис Тейлор
	LSC_beginElisHusband();
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_32", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_rumourElisHusband");
	//квест со скелетом Лейтона Декстера
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_33", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_rumourLostDecster");
	//наводка на Доминика Легро с вином
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_34", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
}

void LSC_returnBarmen(string qName)
{
	pchar.questTemp.LSC = "toWineCellarLate";
	sld = characterFromId("LSCBarmen");
	LAi_SetBarmanTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, "FleuronTavern", "barmen",  "stay");
}

void LSC_returnBarmen_2(string qName)
{
	sld = characterFromId("LSCBarmen");
	LAi_CharacterEnableDialog(sld);
	LAi_SetBarmanTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, "FleuronTavern", "barmen",  "stay");
}

void LSC_enterToProtector(string qName)
{
    chrDisableReloadToLocation = true;
	LAi_LocationFightDisable(loadedLocation, true);
    DoQuestFunctionDelay("LSC_enterToProtector_1", 7.0);
}

void LSC_enterToProtector_1(string qName)
{
	LAi_group_Delete("EnemyFight");
    sld = GetCharacter(NPC_GenerateCharacter("LSCQuestMent", "off_hol_2", "man", "man", 25, PIRATE, 0, true));
	FantomMakeCoolFighter(sld, 25, 70, 70, "blade28", "pistol3", 50);
	sld.Dialog.Filename = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode = "LSCQuestMent";
    LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload1");
	LAi_ActorDialog(sld, pchar, "", 1.0, 0);
}

void LSC_KillOneNarval(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_LocationFightDisable(loadedLocation, true);
    DoQuestFunctionDelay("LSC_KillOneNarval_1", 3.0 + frand(4.0));
}

void LSC_KillOneNarval_1(string qName)
{
	LAi_group_Delete("EnemyFight");
    sld = GetCharacter(NPC_GenerateCharacter("LSCQuestNarval", "officer_16", "man", "man", 25, PIRATE, 0, true));
	FantomMakeCoolFighter(sld, 30, 80, 80, "blade28", "pistol4", 150);
	sld.Dialog.Filename = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode = "LSCQuestNarval";
	ChangeItemName("DOjeronRing", "itmname_DOjeronRingNarval");
	ChangeItemDescribe("DOjeronRing", "itmdescr_DOjeronRingNarval");
	ref itm;
	itm = ItemsFromID("DOjeronRing");
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_4";
	GiveItem2Character(sld, "DOjeronRing");
	sld.SaveItemsForDead   = true; // сохранять на трупе вещи
	sld.DontClearDead = true;  // не убирать труп через 200с
    LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload2");
	LAi_ActorDialog(sld, pchar, "", 1.0, 0);
}

void NavalEnterOfficers(string qName)
{
	int idxMent;
	string sOffName;
	chrDisableReloadToLocation = true;
	for (i=1 ; i<=3; i++)
	{
		idxMent = GetCharacterIndex("Ment_" + i);
		if (idxMent != -1)
		{
			ChangeCharacterAddressGroup(&characters[idxMent], "VelascoShipInside1", "officers", "officer_"+i);
			characters[idxMent].Dialog.CurrentNode = "AffterFightN";
			LAi_SetWarriorType(&characters[idxMent]);
			LAi_group_MoveCharacter(&characters[idxMent], LAI_GROUP_PLAYER);
		}
	}
	LAi_group_SetCheck("PearlGroup_1", "NarvalDestroyed");
	//#20181015-01
	idxMent = GetCharacterIndex("Narval_head");
	if (idxMent != -1) {
        sld = &characters[idxMent];
        LAi_SetImmortal(sld, false);
        LAi_group_CheckGroupQuest(sld);
	}
}

void NavalExitVelasco(string qName)
{
	int idxMent;
	string sOffName;
	for (i=1 ; i<=3; i++)
	{
		idxMent = GetCharacterIndex("Ment_" + i);
		if (idxMent != -1)
		{
			sld = &characters[idxMent];
			ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "goto", sld.location.baseLocator);
			sld.Dialog.CurrentNode = "First time";
			sld.cityTape = "ment"; //тип нпс
			LAi_SetLSCoutTypeNoGroup(sld);
		}
	}
	//распространим слухи
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_35", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_36", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_37", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_38", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_39", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_40", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_41", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_42", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_43", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_44", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
}

void LSC_scriptInterception(string qName)
{
	if (pchar.location == "FleuronTavern")
	{
		LAi_SetActorType(pchar);
		sld = characterFromId("LSCMayor");
		sld.Dialog.CurrentNode = "Interception";
		LAi_SetActorType(sld);
		ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto3");
		LAi_ActorGoToLocator(sld, "quest", "stay1", "LSC_EnterComplite_1", -1);
		sld = characterFromId("Casper_head");
		LAi_SetActorType(sld);
		ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
		LAi_ActorGoToLocator(sld, "quest", "stay2", "LSC_EnterComplite_2", -1);
	}
	else
	{
		pchar.questTemp.LSC = "InterceptionLate";
		AddQuestRecord("ISS_MainLine", "19");
		AddQuestUserData("ISS_MainLine", "sSex", GetSexPhrase("","ла"));
		AddQuestUserData("ISS_MainLine", "sSex1", GetSexPhrase("","а"));
	}
}

void LCS_EndScriptInterception()
{
    //на место камеру и пр.
	SetMainCharacterIndex(1);
    PChar = GetMainCharacter();
    LAi_SetPlayerType(PChar);
	locCameraTarget(PChar)
    locCameraFollow();
    EndQuestMovie();
	//разводим адмирала и каспера
	sld = characterFromId("LSCMayor");
	sld.Dialog.CurrentNode = "First time";
	LAi_SetActorType(sld);
	LAi_ActorGoToLocator(sld, "goto", "goto3", "", -1.0);
	sld = characterFromId("Casper_head");
	LAi_SetActorType(sld);
	LAi_ActorGoToLocator(sld, "reload", "reload1", "", -1.0);
	//прерывание на возврат адмирала и каспера на места
	pchar.quest.LCS_EndScriptInterception_2.win_condition.l1 = "ExitFromLocation";
	pchar.quest.LCS_EndScriptInterception_2.win_condition.l1.location = pchar.location;
	pchar.quest.LCS_EndScriptInterception_2.function = "LCS_EndScriptInterception_2";
	if (pchar.questTemp.LSC == "InterceptionOk")
	{
		AddQuestRecord("ISS_MainLine", "22");
		AddQuestUserData("ISS_MainLine", "sSex", GetSexPhrase("как полный кретин","как полная дура"));
		AddQuestUserData("ISS_MainLine", "sSex1", GetSexPhrase("","а"));
	}
	else
	{
		AddQuestRecord("ISS_MainLine", "20");
		AddQuestUserData("ISS_MainLine", "sSex", GetSexPhrase("ся","ась"));
		AddQuestUserData("ISS_MainLine", "sSex1", GetSexPhrase("","а"));
	}
}

void LCS_EndScriptInterception_2(string qName)
{
	sld = characterFromId("LSCMayor");
	sld.Dialog.CurrentNode = "First time";
	sld.location.going = "toTavern"; //посадим в таверну, ведь он туда пошёл
	SaveCurrentNpcQuestDateParam(sld, "location");
	LAi_SetHuberType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	ChangeCharacterAddressGroup(sld, "SanAugustineResidence", "sit", "sit1");

	sld = characterFromId("Casper_head");
	sld.Dialog.CurrentNode = "First time";
	//LAi_SetImmortal(sld, false);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "PearlGroup_2");
	ChangeCharacterAddressGroup(sld, "SanGabrielMechanic", "goto", "goto1");
}

void LSC_changeBarmen(string qName)
{
	if (pchar.location != "FleuronTavern")
	{
		pchar.questTemp.LSC = "barmenIsDead";
		sld = characterFromId("LSCBarmen");
        sld.name = GetConvertStrWithReplace("Variable_reaction_functions_139", "Names.txt", "#space#", " ");
        sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_140", "Names.txt", "#space#", " ");
		sld.model = "barmen_2";
        SendMessage(sld, "lss", MSG_CHARACTER_SETMODEL, sld.model, sld.model.animation);
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_45", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_46", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_47", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_48", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_49", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_50", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_51", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
	}
	else
	{
		pchar.quest.LSC_changeBarmen_2.win_condition.l1 = "ExitFromLocation";
		pchar.quest.LSC_changeBarmen_2.win_condition.l1.location = pchar.location;
		pchar.quest.LSC_changeBarmen_2.function = "LSC_changeBarmen_2";
	}
}

void LSC_changeBarmen_2(string qName)
{
	pchar.questTemp.LSC = "barmenIsDead";
	sld = characterFromId("LSCBarmen");
    sld.name = GetConvertStrWithReplace("Variable_reaction_functions_141", "Names.txt", "#space#", " ");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_142", "Names.txt", "#space#", " ");
	sld.model = "barmen_2";
    SendMessage(sld, "lss", MSG_CHARACTER_SETMODEL, sld.model, sld.model.animation);
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_52", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_53", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_54", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_55", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_56", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_57", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_58", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "LSC_HillBrunnerDead");
}

void LSC_SaveSesilGalard(string qName)
{
	if (pchar.location.locator == "reload1") sTemp = "goto5";
	else sTemp = "goto4";
	chrDisableReloadToLocation = true;
	LAi_LocationFightDisable(loadedLocation, true);
	sld = characterFromId("SesilGalard");
	sld.dialog.currentnode = "HelpMe";
	sld.greeting = "Enc_RapersGirl_1";
	LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
	GiveItem2Character(sld, "unarmed");
	EquipCharacterbyItem(sld, "unarmed");
	ChangeCharacterAddressGroup(sld, pchar.location, "goto",  sTemp);
	LAi_ActorDialog(sld, pchar, "", 1.0, 0);
}

void LSC_SaveSesilGalard_2()
{
	LAi_SetFightMode(pchar, true);
	LAi_LocationFightDisable(loadedLocation, false);
	//Сесилия
	sld = characterFromID("SesilGalard");
	LAi_SetActorTypeNoGroup(sld);
	if (pchar.location.locator == "reload1")
	{
		LAi_ActorRunToLocator(sld, "barmen", "bar2", "LSC_SesilAfraid", -1);
	}
	else
	{
		LAi_ActorRunToLocator(sld, "barmen", "stay", "LSC_SesilAfraid", -1);
	}
	//клановцы
	LAi_group_Delete("EnemyFight");
	if (sti(pchar.rank)<12) iTemp = sti(pchar.rank);
	else iTemp = 10+MOD_SKILL_ENEMY_RATE*2;
    for (i=1; i<=3; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter("LSCBandit"+i, "officer_"+(i+3), "man", "man", iTemp, PIRATE, 0, true));
        if (sti(pchar.rank)<12) FantomMakeCoolFighter(sld, iTemp, 50, 50, "topor3", "pistol2", 50);
		else FantomMakeCoolFighter(sld, iTemp, 80, 80, "topor2", "pistol6", 50);
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        if (i == 2)
			ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload2");
		else
			ChangeCharacterAddressGroup(sld, pchar.location, "quest",  "start"+i);
    }
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
    LAi_group_SetCheck("EnemyFight", "LSC_SesilAfterFight");
}
//вход к касперам на Сан Габриэль
void LSC_enterCasper(string qName)
{
	//если тюремщик Томас Бойл с нами
	iTemp = GetCharacterIndex("LSC_Prisoner1");
	if (iTemp != -1 && characters[iTemp].location == "SanGabrielMechanic")
	{
		characters[iTemp].reactionOnFightModeOn = true; //среагировать на обнаженку оружия
		characters[iTemp].lifeDay = 0;
		LAi_warrior_DialogEnable(&characters[iTemp], true);
		LAi_group_SetCheck("PearlGroup_2", "LSC_PrisonerAfterFight");
	}
	//касперы
	iTemp = GetCharacterIndex("Casper_head");
	if (iTemp != -1 && characters[iTemp].location == "SanGabrielMechanic")
	{
		chrDisableReloadToLocation = true;
		LAi_group_SetCheck("PearlGroup_2", "OpenTheDoors");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_59", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_60", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_61", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_62", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_63", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	}
	//откроем возможность попасть к механику.
	LocatorReloadEnterDisable("SanGabrielMechanic", "reload2", false);
	LocatorReloadEnterDisable("LostShipsCity_town", "reload55", false);
	pchar.questTemp.LSC = "toSeekMechanikCanMove";
	DeleteAttribute(pchar, "questTemp.LSC.Armo");
}
//прерывание на завал каспера от Армо
void LSC_KillOneCasper(string qName)
{
	pchar.quest.LSC_checkCasperDead.over = "yes";
	if (sti(pchar.questTemp.LSC.Armo) < 5)
	{
		iTemp = sti(pchar.quest.(qName).casperIdx);
		pchar.questTemp.LSC.Armo = sti(pchar.questTemp.LSC.Armo) + 1; //на следующего...
		pchar.questTemp.LSC.Armo.waiting = true;
		AddQuestRecord("ISS_MainLine", "36");
		AddQuestUserData("ISS_MainLine", "sName", GetFullName(&characters[iTemp]));
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_64", "AddRumours.txt", "#space#", " "), "LostShipsCity", 2, 1, "LSC", "");
		AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_65", "AddRumours.txt", "#space#", " "), "LostShipsCity", 2, 1, "LSC", "");
	}
	else
	{	//засада после третьего каспера
		DoQuestFunctionDelay("LSC_KillOneCasper_3", 4.0);
		chrDisableReloadToLocation = true;
	}
}

void LSC_KillOneCasper_3(string qName)
{
	LAi_SetFightMode(pchar, false);
	LAi_LockFightMode(pchar, true);
	GetCharacterPos(pchar, &locx, &locy, &locz);
	sld = characterFromId("Casper_6");
	sld.dialog.currentnode = "FackYou";
	LAi_SetActorTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, pchar.location, "reload", LAi_FindNearestLocator("reload", locx, locy, locz));
	LAi_ActorDialog(sld, pchar, "", -1.0, 0);
}
//таймер завален ли каспер от Армо
void LSC_checkCasperDead(string qName)
{
	pchar.quest.LSC_KillOneCasper.over = "yes";
	iTemp = sti(pchar.quest.(qName).casperIdx);
	if (!LAi_IsDead(&characters[iTemp]))
	{
		pchar.questTemp.LSC.Armo = 11; //профукал каспера
		sld = &characters[iTemp];
		sld.dialog.currentnode   = "First time";
		LAi_SetWarriorType(sld);
		LAi_group_MoveCharacter(sld, "PearlGroup_2");
		ChangeCharacterAddressGroup(sld, "SanGabrielMechanic", "goto", "goto9");
	}
}

void LSC_3CarperAttackArmo()
{
	LAi_SetFightMode(pchar, true);
	chrDisableReloadToLocation = false;
	GetCharacterPos(pchar, &locx, &locy, &locz);
	//меняем убитую Армо
	sld = characterFromId("LSCwaitress");
    sld.dialog.currentnode = "ArmoIsDead";
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_143", "Names.txt", "#space#", " ");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_144", "Names.txt", "#space#", " ");
	sld.model = "Girl_2";
    SendMessage(sld, "lss", MSG_CHARACTER_SETMODEL, sld.model, sld.model.animation);
    AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_66", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
    AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_67", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
    AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_68", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
	//первый каспер
	sld = characterFromId("Casper_6");
	sld.quest.checkCasper = true; //для обработки в АИ warrior
	LAi_SetWarriorTypeNoGroup(sld);
	//второй
	sld = characterFromId("Casper_7");
	sld.quest.checkCasper = true; //для обработки в АИ warrior
	ChangeCharacterAddressGroup(sld, pchar.location, "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
	//третий
	sld = characterFromId("Casper_8");
	sld.quest.checkCasper = true; //для обработки в АИ warrior
	ChangeCharacterAddressGroup(sld, pchar.location, "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
	AddQuestRecord("ISS_MainLine", "39");
	AddQuestUserData("ISS_MainLine", "sSex", GetSexPhrase("ся","ась"));
}

void LSC_underwaterDeathTimer(string qName)
{
	GameOver("sea");
}

//начать малый шторм
void LSC_SmallStormIsBegin()
{
	for(i=0; i<MAX_LOCATIONS; i++)
	{
		if (CheckAttribute(&locations[i], "fastreload") && locations[i].fastreload == "LostShipsCity")
		{
			locations[i].alwaysStorm = true; //живём в штормах
			if (locations[i].id == "LostShipsCity_town")
			{
				locations[i].alwaysStorm.WaveHeigh = true; //поднять уровень воды до 2.5 для низкого волнения
			}
			locations[i].storm = true;
			locations[i].tornado = true;
		}
	}
	//пустим слух о шторме
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_69", "AddRumours.txt", "#space#", " "), "LostShipsCity", 5, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_70", "AddRumours.txt", "#space#", " "), "LostShipsCity", 5, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_71", "AddRumours.txt", "#space#", " "), "LostShipsCity", 5, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_72", "AddRumours.txt", "#space#", " "), "LostShipsCity", 5, 1, "LSC", "");
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_73", "AddRumours.txt", "#space#", " "), "LostShipsCity", 5, 1, "LSC", "");
	//расставим нпс на свои места, штормовые. ограничиваем хождение по городу
	LAi_group_Delete("EnemyFight");
	for(i=0; i<MAX_CHARACTERS; i++)
	{
		makeref(sld, characters[i]);
		if (CheckAttribute(sld, "city") && sld.city == "LostShipsCity")
		{
			if (sld.cityTape == "mayor")
			{
				//LAi_SetImmortal(sld, false);
				DeleteAttribute(sld, "location.going");
				sld.cityTape = "notMove";  //не перемещаться более
				LAi_SetHuberType(sld); //садим за стол
				LAi_group_MoveCharacter(sld, "EnemyFight");
				ChangeCharacterAddressGroup(sld, "SanAugustineResidence", "sit", "sit1");
			}
			if (sld.cityTape == "ment")
			{
				if (sld.id == "Ment_6")
				{
					LAi_SetImmortal(sld, false);
					DeleteAttribute(sld, "location.going");
					sld.cityTape = "notMove";  //не перемещаться более
					LAi_SetLSCoutTypeNoGroup(sld);
					LAi_group_MoveCharacter(sld, "EnemyFight");
					ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "goto", sld.location.baseLocator);
				}
				else
				{
					sTemp = sld.id;
					sTemp = "goto" + strcut(sTemp, 5, 5);
					LAi_SetImmortal(sld, false);
					DeleteAttribute(sld, "location.going");
					sld.cityTape = "notMove";  //не перемещаться более
					LAi_SetWarriorType(sld);
					LAi_group_MoveCharacter(sld, "EnemyFight");
					ChangeCharacterAddressGroup(sld, "SanAugustineResidence", "goto", sTemp);
				}
			}
			if (sld.cityTape == "citizenHome")
			{
				DeleteAttribute(sld, "location.going");
				sld.cityTape = "notMove";  //не перемещаться более
				LAi_SetOwnerTypeNoGroup(sld);
				ChangeCharacterAddressGroup(sld, sld.location.baseLocation, "barmen", "stay");
			}
			if (sld.cityTape == "prisonerHead" || sld.cityTape == "prisoner")
			{
				LAi_SetImmortal(sld, false);
				LAi_group_MoveCharacter(sld, "EnemyFight");
			}
		}
	}
}

void LSC_afterArestMachanic(string qName)
{
	sld = &characters[sti(pchar.questTemp.LSC.lastManId)];
	LAi_SetStayTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, "SanGabrielMechanic", "barmen",  "stay");
	sld = characterFromId("Mechanic");
	ChangeCharacterAddress(sld, "none", "");
	DeleteAttribute(pchar, "questTemp.LSC.lastManId");
}

void LSC_fightEnterResidence(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_LocationFightDisable(loadedLocation, true);
	//отравляем наверх
	string sLocator;
	iTemp = GetCharacterIndex("Ment_1")
	if (iTemp != -1)
	{
		sld = &characters[iTemp];
		LAi_CharacterReincarnation(sld, false, false);
		LAi_SetLSCoutTypeNoGroup(sld);
		ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "goto", sld.location.baseLocator);
		LAi_warrior_DialogEnable(sld, false);
	}
	//------------------------
	iTemp = GetCharacterIndex("Ment_2")
	if (iTemp != -1)
	{
		sld = &characters[iTemp];
		LAi_CharacterReincarnation(sld, false, false);
		LAi_SetLSCoutTypeNoGroup(sld);
		ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "goto", sld.location.baseLocator);
		LAi_warrior_DialogEnable(sld, false);
	}
	//------------------------
	iTemp = GetCharacterIndex("Ment_3")
	if (iTemp != -1)
	{
		sld = &characters[iTemp];
		LAi_CharacterReincarnation(sld, false, false);
		LAi_SetLSCoutTypeNoGroup(sld);
		ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "goto", sld.location.baseLocator);
		LAi_warrior_DialogEnable(sld, false);
	}
	//в резиденции
	iTemp = GetCharacterIndex("Ment_4")
	if (iTemp != -1)
	{
		sld = &characters[iTemp];
		LAi_SetWarriorTypeNoGroup(sld);
		LAi_warrior_DialogEnable(sld, false);
		LAi_warrior_SetStay(sld, true);
		ChangeCharacterAddressGroup(sld, "SanAugustineResidence", "goto", "goto2");
	}
	iTemp = GetCharacterIndex("Ment_5")
	if (iTemp != -1)
	{
		sld = &characters[iTemp];
		LAi_SetWarriorTypeNoGroup(sld);
		LAi_warrior_DialogEnable(sld, false);
		LAi_warrior_SetStay(sld, true);
		ChangeCharacterAddressGroup(sld, "SanAugustineResidence", "goto", "goto3");
	}
	sld = characterFromId("LSCMayor");
	sld.dialog.currentnode = "fightTalking";
	LAi_SetImmortal(sld, false);
	LAi_SetActorTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, "SanAugustineResidence", "goto", "goto10");
	LAi_ActorDialog(sld, pchar, "", -1.0, 0);
}

void LSC_figtInResidence()
{
	sld = characterFromId("pet_crab");
	if (!CharacterIsDead(sld) && CheckAttribute(sld, "quest.answer_1"))
	{
		sld.dialog.currentnode = "crab_hire";
		sld.model	= "pet_crab";
		sld.model.animation = "crabBig";
		LAi_SetHP(sld, 1000, 1000);
		SetSPECIAL(sld, 7,7,7,7,7,7,7)
		sld.rank = 25;
		LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", 2.0, 0);
		ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "goto", "goto11_1");
	}

	sld = characterFromId("Ment_6");
	GetCharacterPos(pchar, &locx, &locy, &locz);
	LAi_SetWarriorTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, pchar.location, "reload", LAi_FindNearestFreeLocator("reload", locx, locy, locz));
    pchar.quest.LSC_figtInResidence_1.win_condition.l1 = "NPC_Death";
    pchar.quest.LSC_figtInResidence_1.win_condition.l1.character = "LSCMayor";
	iTemp = GetCharacterIndex("Ment_4")
	if (iTemp != -1)
	{
		pchar.quest.LSC_figtInResidence_1.win_condition.l2 = "NPC_Death";
		pchar.quest.LSC_figtInResidence_1.win_condition.l2.character = "Ment_4";
	}
	iTemp = GetCharacterIndex("Ment_5")
	if (iTemp != -1)
	{
		pchar.quest.LSC_figtInResidence_1.win_condition.l3 = "NPC_Death";
		pchar.quest.LSC_figtInResidence_1.win_condition.l3.character = "Ment_5";
	}
    pchar.quest.LSC_figtInResidence_1.function= "LSC_figtInResidence_1";
	LAi_SetFightMode(pchar, true);
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
}

void LSC_figtInResidence_1(string qName)
{
	sld = characterFromId("Ment_6");
	if (!CharacterIsDead(sld))
	{
		sld.dialog.currentnode = "MentOffIsLife";
		LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", -1.0, 0);
	}
	else
	{
		AddQuestRecord("ISS_MainLine", "65");
		chrDisableReloadToLocation = false;
	}
	pchar.quest.LSC_enterToPrison.win_condition.l1 = "location";
	pchar.quest.LSC_enterToPrison.win_condition.l1.location = "TartarusPrison";
	pchar.quest.LSC_enterToPrison.function = "LSC_enterToPrison";

}

void LSC_enterToPrison(string qName)
{
	pchar.quest.LSC_enterToPrison.over = "yes";//фикс, Механик в тюрьме только один раз
	chrDisableReloadToLocation = true;
	sld = characterFromId("Mechanic");
	sld.dialog.currentnode = "inPrison";
	LAi_SetImmortal(sld, true);
	LAi_SetGroundSitType(sld);
	ChangeCharacterAddressGroup(sld, "TartarusPrison", "quest", "prison3");
	sld = CharacterFromID("LSC_Prisoner1");
	if(!CharacterIsDead(sld))
	{
		ChangeCharacterAddressGroup(sld, sld.location, "goto", "goto7");
		LAi_group_MoveCharacter(sld, "TmpEnemy");
		LAi_group_FightGroups("TmpEnemy", LAI_GROUP_PLAYER, true);
	}
}

//начать большой шторм
void LSC_BigStormIsBegin()
{
	bDisableFastReload = true;
	DeleteAttribute(pchar,"JustGotIntoLSC");
	for(i=0; i<MAX_LOCATIONS; i++)
	{
		if (locations[i].id == "LostShipsCity_town")
		{	//закрываем релоады
			locations[i].notCrateFoam = true; //не рисовать пену
			DeleteAttribute(&locations[i], "reload.l2.disable"); //открываем выход с ГПК
			locations[i].reload.l231.disable = true;
			for(int n=3; n<=73; n++)
			{
				sTemp = "l" + n;
				locations[i].reload.(sTemp).disable = true;
			}
			DeleteAttribute(&locations[i], "reload.l2.disable"); //откроем выход из ГПК
			locations[i].locators_radius.reload.reload2_back = 20.0;
			pchar.location.from_sea = "LostShipsCity_town";
			//второй патч ГПК
			locations[i].models.day.charactersPatch = "LostShipsCity_patch_end";
			locations[i].models.night.charactersPatch = "LostShipsCity_patch_end";
			//погода
			DeleteAttribute(&locations[i], "alwaysStorm");
			DeleteAttribute(&locations[i], "QuestlockWeather");
			DeleteAttribute(&locations[i], "MaxWaveHeigh");
			locations[i].alwaysStorm_2 = true; //живём в штормах
			locations[i].alwaysStorm_2.WaveHeigh = true; //поднять уровень воды до 2.5 для низкого волнения
			locations[i].storm = true;
			locations[i].tornado = true;
			//#20181023-02
			if(MOD_SKILL_ENEMY_RATE==1)
                //locations[i].showLocatorGrp = "reload";
			break;
		}
	}
	//снять шторм с острова ГПК
	i = FindIsland("LostShipsCity");
	Islands[i].reload.l2.emerge = "reload2_back";
	Islands[i].reload_enable = false;
	Islands[i].visible = true;
	Islands[i].reload_enable = false;
	Islands[i].alwaysStorm = true; //живём в штормах
	Islands[i].storm = true;
	Islands[i].tornado = true;
	//прерывание на удаление острова ГПК и снятие шторма с ареала
	pchar.quest.LSC_closeLine.win_condition.l1 = "location";
	pchar.quest.LSC_closeLine.win_condition.l1.location = "LostShipsCity";
	pchar.quest.LSC_closeLine.function = "LSC_closeLine";
	SetTimerFunction("LSC_takeStormIsland", 0, 0, 10);
}

void LSC_closeLine(string qName)
{
	AddQuestRecord("ISS_MainLine", "69");
	AddQuestUserData("ISS_MainLine", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("ISS_MainLine");
	CloseQuestHeader("TakeVineToPrisoner");
	CloseQuestHeader("LSC_findDekster");
	CloseQuestHeader("ISS_ElisHusband");
	CloseQuestHeader("ISS_takeStoreMoney");
	CloseQuestHeader("ISS_takeCandles");
	pchar.questdata.GPK = 1;
	bQuestDisableMapEnter = false;
	bDisableFastReload = false;
	TakeItemFromCharacter(pchar, "keyPanama");
	//проверим наличие займа у банкира ГПК
	sld = characterFromId("LSC_Usurer");
	if (CheckAttribute(sld, "quest.loan.qtyMoney") && sti(sld.quest.loan.qtyMoney) > 0)
	{
		SetTimerFunction("LSC_climeUsurer", 0, 3 + rand(3), 0);
		pchar.questTemp.LSC_climeUsurer.qtyMoney = sti(sld.quest.loan.qtyMoney) * 5; //сколько денег к возврату
	}
	sld = ItemsFromID("key1");
	sld.price = 10;
	sld = ItemsFromID("key2");
	sld.price = 10;
	sld = ItemsFromID("key3");
	sld.price = 10;
	// Return to LSC - Gregg
	//убираем жителей ГПК из игры
	/*for(i=0; i<MAX_LOCATIONS; i++)
	{
		sld = &characters[i];
		if (CheckAttribute(sld, "city") && sld.city == "LostShipsCity")
			sld.lifeDay = 0;
	}*/
}

void LSC_takeStormIsland(string qName)
{
	i = FindIsland("LostShipsCity");
	//Islands[i].reload_enable = false;
	//Islands[i].visible = false;
	DeleteAttribute(&Islands[i], "alwaysStorm");
	DeleteAttribute(&Islands[i], "storm");
	DeleteAttribute(&Islands[i], "tornado");
	DeleteAttribute(&Islands[i], "QuestlockWeather");
	DeleteAttribute(&Islands[i], "MaxSeaHeight");

	// Return to LSC - Gregg
	pchar.ReturnToLSC = true;
	for(int xx=0; xx<MAX_CHARACTERS; xx++)
	{
		sld = &characters[xx];
		if (CheckAttribute(sld, "city") && sld.city == "LostShipsCity")
			LAi_group_MoveCharacter(sld, "");
	}
	Islands[i].reload.l2.emerge = "reload1_back";
	Islands[i].reload_enable = true;
	Islands[i].visible = true;
	sld = characterFromId("LSC_Usurer");
	sld.lifeDay = 0;
	for(i=0; i<MAX_LOCATIONS; i++)
	{
		if (locations[i].id == "LostShipsCity_town")
		{
			locations[i].reload.l1.disable = false;
			locations[i].reload.l231.disable = false;
			for(int n=3; n<=73; n++)
			{
				sTemp = "l" + n;
				locations[i].reload.(sTemp).disable = false;
			}
			DeleteAttribute(&locations[i], "reload.l2.disable"); //откроем выход из ГПК
			locations[i].locators_radius.reload.reload2_back = 10.0;
			//второй патч ГПК
			locations[i].models.day.charactersPatch = "LostShipsCity_patch_day";
			locations[i].models.night.charactersPatch = "LostShipsCity_patch_day";
			//погода
			DeleteAttribute(&locations[i], "alwaysStorm");
			DeleteAttribute(&locations[i], "QuestlockWeather");
			DeleteAttribute(&locations[i], "MaxWaveHeigh");
			DeleteAttribute(&locations[i], "alwaysStorm_2");
			DeleteAttribute(&locations[i], "alwaysStorm_2.WaveHeigh");
			DeleteAttribute(&locations[i], "storm");
			DeleteAttribute(&locations[i], "tornado");
			break;
		}
	}
	for(i=0; i<MAX_LOCATIONS; i++)
	{
		if (CheckAttribute(&locations[i], "fastreload") && locations[i].fastreload == "LostShipsCity")
		{
			DeleteAttribute(&locations[i], "alwaysStorm");
			DeleteAttribute(&locations[i], "QuestlockWeather");
			DeleteAttribute(&locations[i], "MaxWaveHeigh");
			DeleteAttribute(&locations[i], "alwaysStorm_2");
			DeleteAttribute(&locations[i], "alwaysStorm_2.WaveHeigh");
			DeleteAttribute(&locations[i], "storm");
			DeleteAttribute(&locations[i], "tornado");
		}
	}
}

//------------ посетить жилые помещения магазина ----------->>>>>>
void LCS_SetPedroHurtado(string qName)
{
	sld = characterFromId("PedroHurtado");
	sld.cityTape = "quest"; //тип нпс
	DeleteAttribute(sld, "location.going");
	LAi_SetLoginTime(sld, 10.0, 19.00);
	LAi_SetStayTypeNoGroup(sld);
	ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "goto", "goto04_6");
}

void LSC_PedroRunOpenDoor()
{
	sld = characterFromId("PedroHurtado");
	sld.BreakTmplAndFight = true;
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorRunToLocator(sld, "reload", "reload48", "LSC_PedroOpenedDoor", -1.0);
}

void LSC_Click(string qName)
{
	PlaySound("interface\key.wav");
	LocatorReloadEnterDisable("LostShipsCity_town", "reload48", false);
	chrDisableReloadToLocation = true;
}

void LSC_PedroGoWaiting()
{
	sld = characterFromId("PedroHurtado");
	LAi_RemoveLoginTime(sld);
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorTurnToLocator(sld, "goto", "goto04_6");
	chrDisableReloadToLocation = false;
	pchar.quest.LSC_EnterStoreBig.win_condition.l1 = "locator";
	pchar.quest.LSC_EnterStoreBig.win_condition.l1.location = "EsmeraldaStoreBig";
	pchar.quest.LSC_EnterStoreBig.win_condition.l1.locator_group = "reload";
	pchar.quest.LSC_EnterStoreBig.win_condition.l1.locator = "reload2";
	pchar.quest.LSC_EnterStoreBig.function = "LSC_EnterStoreBig";
}

void LSC_EnterStoreBig(string qName)
{
	//охрана
	chrDisableReloadToLocation = true;
	LAi_group_Delete("EnemyFight");
	iTemp = 8+MOD_SKILL_ENEMY_RATE*2;
    for (i=1; i<=3; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter("LSCStBandit"+i, "officer_"+(i+10), "man", "man", iTemp, PIRATE, 0, true));
        FantomMakeCoolFighter(sld, iTemp, 80, 80, "topor2", "pistol6", 50);
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
		ChangeCharacterAddressGroup(sld, pchar.location, "quest",  "quest"+i);
    }
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
    LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
	pchar.quest.LCS_HeroOutFromStory.win_condition.l1 = "ExitFromLocation";
	pchar.quest.LCS_HeroOutFromStory.win_condition.l1.location = pchar.location;
	pchar.quest.LCS_HeroOutFromStory.function = "LCS_HeroOutFromStory";
}

void LCS_HeroOutFromStory(string qName)
{
	LocatorReloadEnterDisable("LostShipsCity_town", "reload48", true);
	sld = characterFromId("PedroHurtado");
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", 1.0, 0);
	AddSimpleRumourCityTip(GetConvertStrWithReplace("Variable_reaction_functions_74", "AddRumours.txt", "#space#", " "), "LostShipsCity", 10, 1, "LSC", "");
}
//<<<<<<-------- посетить жилые помещения магазина ----------------

//------------ должок ростовщика ГПК ----------->>>>>>
void LSC_climeUsurer(string qName)
{
	pchar.quest.LSC_climeUsurer_2.win_condition.l1 = "Location_Type";
	pchar.quest.LSC_climeUsurer_2.win_condition.l1.location_type = "shop";
	pchar.quest.LSC_climeUsurer_2.function = "LSC_climeUsurer_2";
}

void LSC_climeUsurer_2(string qName)
{
	chrDisableReloadToLocation = true;
	sld = characterFromId(loadedLocation.fastreload + "_trader");
	LAi_CharacterDisableDialog(sld);
	//запишем данные для возврата в локацию
	pchar.questTemp.LSC_climeUsurer.idx = sld.id;
	pchar.questTemp.LSC_climeUsurer.location = pchar.location;
	pchar.questTemp.LSC_climeUsurer.group = pchar.location.group;
	pchar.questTemp.LSC_climeUsurer.locator = pchar.location.locator;
	DoQuestFunctionDelay("LSC_climeUsurer_3", 3.5);
}

void LSC_climeUsurer_3(string qName)
{
 	//перемещаем ГГ
	DoQuestReloadToLocation("Temple_Skulls", "goto", "goto1", "");
	DoQuestFunctionDelay("LSC_climeUsurer_4", 10.0);
	//вернем хозяину магазина возможность диалога
	sld = characterFromId(pchar.questTemp.LSC_climeUsurer.idx);
	LAi_CharacterEnableDialog(sld);
}

void LSC_climeUsurer_4(string qName)
{
	//ставим бога мертвых
	LAi_LockFightMode(Pchar, false);
	LAi_LocationFightDisable(loadedLocation, true);
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("DeadmansGod", "mictlantecuhtli", "skeleton", "man_fast", 100, PIRATE, 0, true));
	else sld = GetCharacter(NPC_GenerateCharacter("DeadmansGod", "mictlantecuhtli", "skeleton", "skeleton", 100, PIRATE, 0, true));
    FantomMakeCoolFighter(sld, 50, 100, 100, "toporAZ", "pistol4", 3000); //дадим четырехствольник
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_145", "Names.txt", "#space#", " ");
	sld.lastname = "";
	sld.dialog.filename   = "Quest\Mictlantecuhtli.c";
	sld.dialog.currentnode   = "ClimeUsurer";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	LAi_SetImmortal(sld, true);
	ChangeCharacterAddressGroup(sld, "Temple_Skulls", "goto", "goto2");
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", 5.0, 0);
}

void ClimeUsurer_haveMoney()
{
	DoQuestFunctionDelay("ClimeUsurer_haveMoney_1", 1.0);
}

void ClimeUsurer_haveMoney_1(string qName)
{
	chrDisableReloadToLocation = false;
	LAi_LocationFightDisable(&locations[FindLocation("Temple_Skulls")], false);
	DoQuestReloadToLocation(pchar.questTemp.LSC_climeUsurer.location, pchar.questTemp.LSC_climeUsurer.group, pchar.questTemp.LSC_climeUsurer.locator, "");
	DeleteAttribute(pchar, "questTemp.LSC_climeUsurer");
}

void ClimeUsurer_noMoney()
{
	LAi_LocationFightDisable(loadedLocation, false);
	sld = characterFromId("DeadmansGod");
	ChangeCharacterAddress(sld, "none", "");
	//если Аззи в дружбанах, то выручает ГГ
	if (pchar.questTemp.Azzy == "over" || pchar.questTemp.Azzy == "Azzy_Freedom")
	{
		DoQuestFunctionDelay("LSC_climeUsurer_Azzy", 30.0);
	}
}

void LSC_climeUsurer_Azzy(string qName)
{
	LAi_LockFightMode(Pchar, false);
	LAi_LocationFightDisable(loadedLocation, true);
	sld = GetCharacter(NPC_GenerateCharacter("Azzy", "Azzy", "man", "man", 22, PIRATE, 0, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_146", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_147", "Names.txt", "#space#", " ");
	sld.Dialog.Filename = "Quest\Azzy_dialog.c";
	sld.dialog.currentnode = "ClimeUsurer";
	GiveItem2Character(sld, "blade28");// сабля Моргана
	sld.equip.blade = "blade28";
	sld.rank 	= 100;
	sld.reputation = 0;
	SetSelfSkill(sld, 100, 100, 100, 100, 100);
	SetShipSkill(sld, 100, 100, 100, 100, 100, 100, 100, 100, 100);
	LAi_SetImmortal(sld, true);
	LAi_SetActorType(sld);
	LAi_SetLoginTime(sld, 0.0, 24.0);
	LAi_SetHP(sld, 10000.0, 10000.0);
	sld.greeting = "Gr_Azzy";
	ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto2");
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", 5.0, 0);
}
//<<<<<<-------- должок ростовщика ГПК ----------------

//------------ освободить мужа Элис Тейлор ----------->>>>>>
void LSC_ElisHusbandSet(string qName)
{
	sld = characterFromId("MaximTailor");
	sld.dialog.currentnode = "First time";
	sld.watchBoxes = true;
	GiveItem2Character(sld, "blade15");
	GiveItem2Character(sld, "pistol2");
	EquipCharacterbyItem(sld, "blade15");
 	EquipCharacterbyItem(sld, "pistol2");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	ChangeCharacterAddressGroup(sld, "CeresSmithy", "goto", "goto1");
}
//<<<<<<-------- освободить мужа Элис Тейлор ----------------

//------------ квест скелета Лейтона Декстера ----------->>>>>>
void LSC_admiralFoundOwnKey(string qName)
{
	sld = ItemsFromID("keyPanama");
	sld.shown = false;
	AddQuestRecord("LSC_findDekster", "11");
	AddQuestUserData("LSC_findDekster", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("LSC_findDekster");
	pchar.questTemp.LSC.lostDecster = "over";
}