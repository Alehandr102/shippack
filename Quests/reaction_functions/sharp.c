// Gregg: вынесено из reaction_functions.c (сплит god-файла) — секция «Генератор жемчужного промысла Sharp».
// #include на исходное место — видимость и поведение идентичны.

bool Sharp_IsQuestCaptain(ref captain)
{
	if(!CheckAttribute(captain, "CaptanId")) return false;
	if(captain.CaptanId != "Sharp") return false;
	if(!CheckAttribute(pchar, "questTemp.Sharp.SeekSpy")) return false;
	string stage = pchar.questTemp.Sharp.SeekSpy;
	if(stage != "inDelNorte" && stage != "alwaysKnow") return false;
	if(pchar.questTemp.Sharp == "toSharp_going") return false;
	int index = GetCharacterIndex("Sharp");
	if(index < 0) return false;
	ref sharpCaptain = GetCharacter(index);
	if(!CheckAttribute(sharpCaptain, "mapEnc.type")) return false;
	if(sharpCaptain.mapEnc.type != "war") return false;
	if(CheckAttribute(sharpCaptain, "SharpRecruitEncounter")) return true;
	// LEO: Сохраняем квестовую встречу в сейвах до появления явной метки.
	return CheckAttribute(sharpCaptain, "AlwaysEnemy") && CheckAttribute(sharpCaptain, "DontRansackCaptain");
}

bool Sharp_CanRecruit(ref captain)
{
	if(CheckAttribute(pchar, "SharpRecruited") || GetCharacterIndex("SharpOf") >= 0) return false;
	return GetPassengersQuantity(pchar) < PASSENGERS_MAX && Sharp_IsQuestCaptain(captain);
}

bool Sharp_IsPrisoner(ref prisoner)
{
	if(prisoner.id == "Sharp") return true;
	// LEO: Старые пленники Шарпа хранятся под id prisoner_N.
	return CheckAttribute(prisoner, "model") && prisoner.model == "Sharp";
}

void Sharp_loginNearIsland(string qName)
{
	if (GetQuestPastMinutesParam("questTemp.Sharp") < 10)
	{
		sld = &characters[GetCharacterIndex("Sharp")];
		DeleteAttribute(sld, "SharpRecruitEncounter");
		LAi_SetCurHPMax(sld);
		sld.nation = pchar.nation; //нация Шарпа всегда как у ГГ
		SetCrewQuantityFull(sld);

		DeleteAttribute(sld, "ship.sails");// убрать дыры на парусах
		DeleteAttribute(sld, "ship.blots");
		RepairMasts(sld);// вернуть сбитые мачты

		SetBaseShipData(sld);
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
		Group_AddCharacter("Sharp_Group", "Sharp");
		Group_SetGroupCommander("Sharp_Group", "Sharp");
		Group_SetAddress("Sharp_Group", pchar.questTemp.Sharp.Island, "quest_ships", "quest_ship_"+(rand(1)+1));
		Group_SetTaskRunaway("Sharp_Group", PLAYER_GROUP);
		pchar.Quest.Sharp_landOver1.win_condition.l1 = "Location_Type";
		pchar.Quest.Sharp_landOver1.win_condition.l1.location_type = "town";
		pchar.Quest.Sharp_landOver1.function = "Sharp_Over";
		pchar.Quest.Sharp_landOver2.win_condition.l1 = "Location_Type";
		pchar.Quest.Sharp_landOver2.win_condition.l1.location_type = "seashore";
		pchar.quest.Sharp_landOver2.function = "Sharp_Over";
	    Pchar.quest.Sharp_mapOver.win_condition.l1 = "MapEnter";
		pchar.quest.Sharp_mapOver.function = "Sharp_Over";
	}
	else
	{
		AddQuestRecord("SharpPearl", "12");
		AddQuestUserData("SharpPearl", "sSex", GetSexPhrase("","а"));
		CloseQuestHeader("SharpPearl");
		pchar.questTemp.Sharp = "seekSharp"; //опять начинаем с борделей
		pchar.questTemp.Sharp.brothelChance = rand(10);
	}
}

void Sharp_Over(string qName)
{
	group_DeleteGroup("Sharp_Group");
	pchar.quest.Sharp_landOver1.over = "yes";
	pchar.quest.Sharp_landOver2.over = "yes";
	pchar.quest.Sharp_mapOver.over = "yes";
	//даем запись в СЖ, если не нашёл Шарпа
	if (pchar.questTemp.Sharp == "toSharp_going")
	{
		AddQuestRecord("SharpPearl", "4");
		CloseQuestHeader("SharpPearl");
		pchar.questTemp.Sharp = "seekSharp"; //опять начинаем с борделей
		pchar.questTemp.Sharp.brothelChance = rand(10);
	}
}

//Удаление потерявшегося контрактника
void Remove_Contract_Officer(string qName)
{
	ref chr = CharacterFromID(qName);
	if (CheckAttribute(chr,"ImmortalOfficer"))
	{
		if (!IsCompanion(chr) && !isOfficerInShip(chr, true))
		{
			pchar.Option_ImmortalOfficers = sti(sti(pchar.Option_ImmortalOfficers) - 1);
		}
	}
}

//------------ жемчужный промысел ----------------
void SeaPearl_login(string qName)
{
	characters[GetCharacterIndex("Sharp")].DeckDialogNode = "NewDeckSharp"; //новая нода на палубу Шарпу
	pchar.quest.SeaPearl_Late.over = "yes";
	LAi_group_Delete("EnemyFight");
	group_DeleteGroup("PearlGroup");
	iTemp = sti(pchar.questTemp.Sharp.PearlAreal.qtyShips);
	Group_FindOrCreateGroup("PearlGroup");
	Group_SetType("PearlGroup", "trade");
	for (int j = 1; j <= iTemp; j++)
	{
		sTemp = "PearlCapitain_" + j;
		sld = GetCharacter(NPC_GenerateCharacter(sTemp, "trader_"+(rand(15)+1), "man", "man", 10, PIRATE, 30, true));
		sld.Ship.Type = GenerateShip(SHIP_LEUDO, false);
		SetRandomNameToShip(sld);
		SetBaseShipData(sld);
		SetCrewQuantityFull(sld);
		Fantom_SetBalls(sld, "pirate");
		sld.ShipTaskLock = true;
		sld.Abordage.Enable = false;
		sld.SinkTenPercent = true; //экономия в АИ шипов
		sld.PearlTartane = true; //тартана ловцов жемчуга, можно грабить
		LAi_group_MoveCharacter(sld, "EnemyFight");
		Group_AddCharacter("PearlGroup", sTemp);
	}
    // ==> стравливание
	Group_SetGroupCommander("PearlGroup", "PearlCapitain_1");
	Group_SetAddress("PearlGroup", pchar.questTemp.Sharp.PearlAreal.Island, "quest_ships", pchar.questTemp.Sharp.PearlAreal.locator);
	Group_SetTaskRunaway("PearlGroup", PLAYER_GROUP);
	Group_LockTask("PearlGroup");
	pchar.quest.SeaPearl_DieHard1.win_condition.l1 = "ExitFromLocation";
	pchar.quest.SeaPearl_DieHard1.win_condition.l1.location = pchar.questTemp.Sharp.PearlAreal.Island;
	pchar.quest.SeaPearl_DieHard1.function = "SeaPearl_DieHard";
	Pchar.quest.SeaPearl_DieHard2.win_condition.l1 = "MapEnter";
	pchar.quest.SeaPearl_DieHard2.function = "SeaPearl_DieHard";
}

void SeaPearl_Late(string qName)
{
	pchar.quest.SeaPearl_login.over = "yes";
	AddQuestRecord("SharpPearl", "9");
	AddQuestUserData("SharpPearl", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("SharpPearl", "iDay", FindRussianDaysString(sti(pchar.questTemp.Sharp.PearlAreal.terms)));
	AddQuestUserData("SharpPearl", "sTarget", GetConvertStr(pchar.questTemp.Sharp.PearlAreal.Shore, "LocLables.txt"));
	pchar.questTemp.Sharp = "seekSharp"; //опять начинаем с борделей
	pchar.questTemp.Sharp.brothelChance = rand(10);
	DeleteAttribute(pchar, "questTemp.Sharp.PearlAreal");
}

void SeaPearl_DieHard(string qName)
{
	group_DeleteGroup("PearlGroup");
	pchar.quest.SeaPearl_DieHard1.over = "yes";
	pchar.quest.SeaPearl_DieHard2.over = "yes";
	if (pchar.questTemp.Sharp == "SeaPearl_success")
	{	//успешное нападение
		AddQuestRecord("SharpPearl", "7");
		AddQuestUserData("SharpPearl", "iSmall", pchar.questTemp.Sharp.PearlAreal.SmallPearlQty);
		AddQuestUserData("SharpPearl", "iBig", pchar.questTemp.Sharp.PearlAreal.BigPearlQty);
	}
	else
	{	//неудачное нападение
		AddQuestRecord("SharpPearl", "8");
		AddQuestUserData("SharpPearl", "sSex", GetSexPhrase("ел","ла"));
		AddQuestUserData("SharpPearl", "sSex1", GetSexPhrase("","а"));
	}
	pchar.questTemp.Sharp = "seekSharp"; //опять начинаем с борделей
	pchar.questTemp.Sharp.brothelChance = rand(10);
	DeleteAttribute(pchar, "questTemp.Sharp.PearlAreal");
	CloseQuestHeader("SharpPearl");
}

void SharpSeekSpy_loginSpy(string qName)
{
	if (!IsDay())
	{
		LAi_group_Delete("EnemyFight");
		AddQuestRecord("SharpPearl_SeekSpy", "3");
		sld = GetCharacter(NPC_GenerateCharacter("Emillio", "pirate_5", "man", "man", 20, SPAIN, 0, true));
		sld.name = GetConvertStrWithReplace("Variable_reaction_functions_10", "Names.txt", "#space#", " ");
		sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_11", "Names.txt", "#space#", " ");
		sld.Dialog.Filename = "Quest\ForAll_dialog.c";
		sld.dialog.currentnode = "SharpSeekSpy_Emi";
		LAi_SetActorType(sld);
		LAi_group_MoveCharacter(sld, "EnemyFight");
		ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto5");
		rCharacter = GetCharacter(NPC_GenerateCharacter("QuestMan_5", "pirate_6", "man", "man", 20, SPAIN, 0, true));
		rCharacter.Dialog.Filename = "Quest\ForAll_dialog.c";
		rCharacter.dialog.currentnode = "SharpSeekSpy_fri";
		LAi_SetActorType(rCharacter);
		LAi_group_MoveCharacter(rCharacter, "EnemyFight");
		ChangeCharacterAddressGroup(rCharacter, pchar.location, "goto", "goto6");
		DoQuestCheckDelay("CannotFightCurLocation", 1.0);
		pchar.questTemp.Sharp.SeekSpy = "inDelNorte";
		pchar.quest.SharpSeekSpy_script.win_condition.l1 = "locator";
		pchar.quest.SharpSeekSpy_script.win_condition.l1.location = "Shore55";
		pchar.quest.SharpSeekSpy_script.win_condition.l1.locator_group = "quest";
		pchar.quest.SharpSeekSpy_script.win_condition.l1.locator = "SharpSeekSpy";
		pchar.quest.SharpSeekSpy_script.function = "SharpSeekSpy_script";
		Group_DeleteGroup("Sharp_Group");
		string sGroup = "Sharp_Group";
		Group_FindOrCreateGroup(sGroup);
		sld = characterFromId("Sharp");
		LAi_SetHP(sld, 350.0, 350.0);
		// LEO: после прежнего потопления восстанавливаем и корабль, а не только здоровье Шарпа.
		SetBaseShipData(sld);
		SetHalfPerksToChar(sld, true);
		SetCharacterPerk(sld, "MusketsShoot");
		SetCharacterPerk(sld, "CannonProfessional");
		Group_AddCharacter(sGroup, sld.id);
		Group_SetGroupCommander(sGroup, sld.id);

		sld.mapEnc.worldMapShip = "quest_ship";
		sld.mapEnc.Name = GetConvertStrWithReplace("Variable_reaction_functions_12", "Names.txt", "#space#", " ");
		sld.AlwaysEnemy = true;
		sld.DontRansackCaptain = true;
		sld.SharpRecruitEncounter = true;
		sld.mapEnc.type = "war";
		sld.hunter = "pirate";
		Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
		Group_LockTask(sGroup);
		Map_CreateWarrior("", sld.id, 8);
	}
	else
	{
		SetTimerFunction("SharpSeekSpy_again", 0, 0, 3);
	}
}

void SharpSeekSpy_again(string qName)
{
	pchar.quest.SharpSeekSpy_loginSpy.win_condition.l1 = "location";
	pchar.quest.SharpSeekSpy_loginSpy.win_condition.l1.location = "Shore55";
	pchar.quest.SharpSeekSpy_loginSpy.function = "SharpSeekSpy_loginSpy";
}

void SharpSeekSpy_script(string qName)
{
	if (GetCharacterIndex("Emillio") != -1)
	{
		LAi_ActorTurnToCharacter(characterFromId("QuestMan_5"), pchar);
		sld = characterFromId("Emillio");
		sld.dialog.currentnode = "SharpSeekSpy_Emi";
		LAi_ActorDialog(sld, pchar, "", 0, 0);
	}
	else
	{
		pchar.questTemp.Sharp.SeekSpy = "over";
	}
}

void SharpSeekSpy_caveDialog()
{
	sld = characterFromId("QuestMan_5");
	sld.Dialog.Filename = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode = "SharpSeekSpy_fri";
	LAi_ActorDialog(sld, pchar, "", 0, 0);
}

void SharpSeekSpy_caveDialog_2()
{
	sld = characterFromId("Emillio");
	sld.dialog.currentnode = "SharpSeekSpy_Emi_1";
	LAi_ActorDialog(sld, pchar, "", 0, 0);
}

void SharpSeekSpy_caveDialog_3()
{
	DoQuestCheckDelay("CanFightCurLocation", 0.0);
	sld = characterFromId("Emillio");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	sld = characterFromId("QuestMan_5");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
    LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
    // Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
    LAi_SetFightMode(PChar, true);
    pchar.quest.SharpSeekSpy_afterFight.win_condition.l1 = "NPC_Death";
	pchar.quest.SharpSeekSpy_afterFight.win_condition.l1.character = "Emillio";
    pchar.quest.SharpSeekSpy_afterFight.win_condition.l2 = "NPC_Death";
	pchar.quest.SharpSeekSpy_afterFight.win_condition.l2.character = "QuestMan_5";
	pchar.quest.SharpSeekSpy_afterFight.function = "SharpSeekSpy_afterFight";
}

void SharpSeekSpy_afterFight(string qName)
{
	AddQuestRecord("SharpPearl_SeekSpy", "4");
	pchar.questTemp.Sharp.SeekSpy = "alwaysKnow";
	pchar.questTemp.Sharp = "seekSharp"; //можно снова поискать Шарпа
	CloseQuestHeader("SharpPearl");
}
