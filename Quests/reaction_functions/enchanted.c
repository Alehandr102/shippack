// Gregg: вынесено из reaction_functions.c (сплит god-файла) — секция «Зачарованный город».
// #include на исходное место — видимость и поведение идентичны.

void MC_startInCaracasRang(string qName)
{
	pchar.quest.MC_startInCaracas.win_condition.l1 = "location";
	pchar.quest.MC_startInCaracas.win_condition.l1.location = "Caracas_town";
	pchar.quest.MC_startInCaracas.function = "MC_startInCaracas";
}

void MC_startInCaracas(string qName)
{
	if (GetHour() > 9.0 && GetHour() < 20.0 && !LAi_IsCapturedLocation)
	{
		pchar.questTemp.MC = "toCaracas"; //флаг квеста
		bDisableFastReload = true;
		iTemp = FindLocation("Caracas_town");
		DeleteAttribute(&locations[iTemp], "citizens"); //уберем горожан
		DeleteAttribute(&locations[iTemp], "carrier");	//уберем грузчиков
		DeleteAttribute(&locations[iTemp], "questSeekCap");	//уберем квестодателя
		//закроем выходы из города
		locations[iTemp].reload.l1.disable = true;
		locations[iTemp].reload.l2.disable = true;
		locations[iTemp].reload.l3.disable = true;
		locations[iTemp].reload.l4.disable = true;
		locations[iTemp].reload.l41.disable = true;
		locations[iTemp].reload.l5.disable = true;
		locations[iTemp].reload.l6.disable = true;
		locations[iTemp].reload.l7.disable = true;
		locations[iTemp].reload.l8.disable = true;
		locations[iTemp].reload.l10.disable = true;
		locations[iTemp].reload.gate.disable = true;
		//очистим таверну
		iTemp = FindLocation("Caracas_tavern");
		DeleteAttribute(&locations[iTemp], "habitues"); //уберем в таверне наполнение нпс
		locations[iTemp].reload.l1.disable = true;
		locations[iTemp].reload.l4.disable = true;
		sld = characterFromId("Caracas_waitress"); //официантку тоже
		ChangeCharacterAddress(sld, "none", "");
		//ставим горожан
		for (i=1; i<=12; i++)
		{
			sld = GetCharacter(NPC_GenerateCharacter("CaracasCit_"+i, "citiz_"+i, "man", "man", 10, SPAIN, -1, true));
			sld.dialog.filename = "Quest\MagicCity.c";
			sld.dialog.currentnode = "CaracasMan";
			sld.greeting = "cit_common";
			sld.BreakTmplAndFightGroup = true;
			LAi_SetActorType(sld);
			LAi_ActorWaitDialog(sld, pchar);
			LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
			ChangeCharacterAddressGroup(sld, "Caracas_town", "quest", "MCquest"+i);
		}
	}
	else
	{
		pchar.quest.MC_startAgain.win_condition.l1 = "MapEnter";
		pchar.quest.MC_startAgain.function = "MC_startAgain";
	}
}

void MC_startAgain(string qName)
{
	pchar.quest.MC_startInCaracas.win_condition.l1 = "location";
	pchar.quest.MC_startInCaracas.win_condition.l1.location = "Caracas_town";
	pchar.quest.MC_startInCaracas.function = "MC_startInCaracas";
}

void MC_endCaracas()
{
	bDisableFastReload = false;
	pchar.questTemp.MC = "toMaracaibo"; //флаг квеста
	//вернем город
	iTemp = FindLocation("Caracas_town");
	locations[iTemp].citizens = true;
	locations[iTemp].carrier = true;
	locations[iTemp].questSeekCap = 8;
	DeleteAttribute(&locations[iTemp], "reload.l1.disable");
	DeleteAttribute(&locations[iTemp], "reload.l2.disable");
	DeleteAttribute(&locations[iTemp], "reload.l3.disable");
	DeleteAttribute(&locations[iTemp], "reload.l4.disable");
	DeleteAttribute(&locations[iTemp], "reload.l41.disable");
	DeleteAttribute(&locations[iTemp], "reload.l5.disable");
	DeleteAttribute(&locations[iTemp], "reload.l6.disable");
	DeleteAttribute(&locations[iTemp], "reload.l7.disable");
	DeleteAttribute(&locations[iTemp], "reload.l8.disable");
	DeleteAttribute(&locations[iTemp], "reload.l10.disable");
	DeleteAttribute(&locations[iTemp], "reload.gate.disable");
	DeleteAttribute(&locations[iTemp], "hidden_effects");
	//вернем таверну
	iTemp = FindLocation("Caracas_tavern");
	locations[iTemp].habitues = true;
	DeleteAttribute(&locations[iTemp], "reload.l1.disable");
	DeleteAttribute(&locations[iTemp], "reload.l4.disable");
	sld = characterFromId("Caracas_waitress"); //официантку тоже
	sld.location = "Caracas_tavern";
	sld.location.group = "waitress";
	sld.location.locator = "barmen";
	SetQuestHeader("MagicCity");
	AddQuestRecord("MagicCity", "1");
	pchar.quest.MC_DesMoinesInShore.win_condition.l1 = "location";
	pchar.quest.MC_DesMoinesInShore.win_condition.l1.location = "DesMoines_port";
	pchar.quest.MC_DesMoinesInShore.function = "MC_DesMoinesInShore";
	pchar.quest.MC_DesMoinesInJungle.win_condition.l1 = "location";
	pchar.quest.MC_DesMoinesInJungle.win_condition.l1.location = "DesMoines_ExitTown";
	pchar.quest.MC_DesMoinesInJungle.function = "MC_DesMoinesInShore";
	LocatorReloadEnterDisable("DesMoines_port", "gate_back", true);
	SetLocationCapturedState("DesMoines_town", true);
	locations[FindLocation("DesMoines_ExitTown")].DisableEncounters = true;
	sld = CharacterFromID("DesMoines_tavernkeeper");
	LAi_LoginInCaptureTown(sld, true);
	sld = CharacterFromID("DesMoines_trader");
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_LoginInCaptureTown(sld, true);
	sld = CharacterFromID("DesMoines_usurer");
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_LoginInCaptureTown(sld, true);
	sld = CharacterFromID("DesMoines_Priest");
	LAi_LoginInCaptureTown(sld, true);
	sld = CharacterFromID("Fernando Torres");
	sld.greeting = "pirat_common";
	LAi_LoginInCaptureTown(sld, true);
	pchar.questTemp.MCzagadka = true;
}

void MC_DesMoinesInShore(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_LocationFightDisable(loadedLocation, true);
	LAi_group_Delete("EnemyFight");
    //Boyer mod...halve number of enemies.  Their rank/skills will already outrank Hero and fight is VERY difficult...you will likely die.
    //#20181002-02
    int nNumE = round((MOD_SKILL_ENEMY_RATE*3 + 0.2) / 2);
    int nNumF = GetOfficersQuantity(pchar);
    if(nNumF > nNumE)
        nNumE = nNumF + rand(2);
    for (i=1; i<=nNumE; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter("ShoreGuard"+i, "officer_"+(i+10), "man", "man", 20, PIRATE, 0, true));
        FantomMakeCoolFighter(sld, 20, 70, 70, "topor2", "pistol3", 50);
		sld.dialog.filename = "Quest\MagicCity.c";
		sld.dialog.currentnode = "ShoreGuard";
		sld.BreakTmplAndFightGroup = true;
		sld.reactionOnFightModeOn = true;
        LAi_SetActorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
		LAi_LoginInCaptureTown(sld, true);
		PlaceCharacter(sld, "goto", "random_free");
		LAi_ActorDialog(sld, pchar, "", -1.0, 0);
    }
	PChar.quest.MC_DesMoinesInShore.over = "yes";
	PChar.quest.MC_DesMoinesInJungle.over = "yes";
}

void MC_DesMoinesVozrojdenie(string qName)
{
	//Вовзращаем город к нормальной жизни	==>
	DeleteAttribute(pchar, "questTemp.MCzagadka");
	SetLocationCapturedState("DesMoines_town", false);		
	DeleteAttribute(&locations[FindLocation("DesMoines_ExitTown")], "DisableEncounters");	
	sld = CharacterFromID("DesMoines_tavernkeeper");
	sld.model	= "skel2";				//небольшой бонус-пасхалка
	sld.model.animation = "skeleton";
	LAi_LoginInCaptureTown(sld, false);
	sld = CharacterFromID("DesMoines_trader");
	LAi_RemoveLoginTime(sld);
	LAi_LoginInCaptureTown(sld, false);
	sld = CharacterFromID("DesMoines_usurer");
	LAi_RemoveLoginTime(sld);
	LAi_LoginInCaptureTown(sld, false);
	sld = CharacterFromID("DesMoines_Priest");
	LAi_LoginInCaptureTown(sld, false);
	sld = CharacterFromID("Fernando Torres");
	sld.sex = "man";
	sld.dialog.filename = "Common_Mayor.c";
	sld.dialog.currentnode = "First time";
	sld.model	= "reservist_8";
	sld.model.animation = "man";
	sld.greeting = "spa_gov_common";
	LAi_LoginInCaptureTown(sld, false);
	//Вовзращаем город к нормальной жизни	<==
}