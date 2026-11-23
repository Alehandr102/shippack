// Gregg: вынесено из reaction_functions.c (сплит god-файла) — секция «Миниквесты».
// #include на исходное место — видимость и поведение идентичны.

void WaitressFack_null(string qName) //нулим квест официантки
{
	LocatorReloadEnterDisable(pchar.questTemp.different.FackWaitress.City + "_tavern", "reload2_back", true);
	pchar.questTemp.different = "free";
	pchar.quest.WaitressFack_inRoom.over = "yes";
	DeleteAttribute(pchar, "questTemp.different.FackWaitress");
}

void WaitressFack_inRoom(string qName)
{
	chrDisableReloadToLocation = true;
	LocatorReloadEnterDisable(locations[reload_location_index].fastreload + "_tavern", "reload2_back", true);
	DoQuestFunctionDelay("WaitressFack_Enter", 10.0 + frand(10.0));
}

void WaitressFack_Enter(string qName)
{
	LAi_group_Delete("EnemyFight");
	LAi_SetFightMode(pchar, false);
	LAi_LockFightMode(pchar, true);
	if (pchar.questTemp.different.FackWaitress.Kick == "0")
	{	//подстава
		iTemp = sti(pchar.rank) + rand(MOD_SKILL_ENEMY_RATE);
		sld = GetCharacter(NPC_GenerateCharacter("BerglarWairessQuest", "officer_"+(rand(42)+1), "man", "man", iTemp, PIRATE, -1, true));
		FantomMakeCoolFighter(sld, iTemp, 80, 80, "topor2", "pistol6", 50);
		sld.dialog.Filename = "Quest\ForAll_dialog.c";
		sld.dialog.currentnode = "WaitressBerglar";
		sld.greeting = "Enc_Raiders";
 		//меняем обличие официантки
		rCharacter = characterFromId(locations[reload_location_index].fastreload + "_waitress");
		rCharacter.model = "girl_" + (rand(7)+1);
		SetRandomNameToCharacter(rCharacter);
		FaceMaker(rCharacter);
	}
	else
	{
        sld = characterFromId("WairessQuest");
	}
	ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
	LAi_SetActorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	LAi_ActorDialog(sld, pchar, "", 2.0, 0);
}

void WaitressFack_outRoom()
{
	// ==> Забираем клинки, пистоли и деньги.
	RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
    while (FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE) != "")
    {
        TakeItemFromCharacter(pchar, FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE));
    }
    while (FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE) != "")
    {
        TakeItemFromCharacter(pchar, FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE));
    }
    pchar.money = 0;
    // <== Забираем клинки, пистоли и деньги.
	AddCharacterExpToSkill(pchar, "Pistol", 100);
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", "reload1_back", "none", "", "", "OpenTheDoors", -1.0);
	DoQuestCheckDelay("OpenTheDoors", 10.0);
	pchar.questTemp.different = "FackWaitress_noMoney"; //флаг ограбили
}

void WaitressFack_fight()
{
	AddCharacterExpToSkill(pchar, "FencingLight", 30);
	AddCharacterExpToSkill(pchar, "Fencing", 30);
	AddCharacterExpToSkill(pchar, "FencingHeavy", 30);
	LAi_SetFightMode(pchar, true);
	pchar.questTemp.different = "FackWaitress_fighted"; //флаг пришлось подраться
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	pchar.quest.WaitressFack_afterFight.win_condition.l1 = "NPC_Death";
	pchar.quest.WaitressFack_afterFight.win_condition.l1.character = "BerglarWairessQuest";
	pchar.quest.WaitressFack_afterFight.win_condition = "OpenTheDoors";
}

void WaitressFack_fack()
{
	pchar.questTemp.HorseQty = sti(pchar.questTemp.HorseQty) + 1;

	DoQuestCheckDelay("PlaySex_1", 1.0);
	pchar.questTemp.different = "FackWaitress_facking";
}

// ----------------- Пожертвование хозяйки борделя ---------------------
void HostessChurch_null(string qName) //нулим квест
{
	//если квест ещё взят, то деньги считай прикарманены
	if (characters[GetCharacterIndex(pchar.questTemp.different.HostessChurch.city + "_Hostess")].questChurch == "taken")
	{
		characters[GetCharacterIndex(pchar.questTemp.different.HostessChurch.city + "_Hostess")].questChurch = "baster";
		if (CheckAttribute(pchar,"HellSpawnLocation")) LocatorReloadEnterDisable(pchar.HellSpawnLocation, "reload7_back", false);
	}
	pchar.questTemp.different = "free";
	DeleteAttribute(pchar, "questTemp.different.HostessChurch");
}

// ----------------- Украсть чертежи на верфи ---------------------
void ShipyardsMap_returnCFN(string qName)
{
	string  sName;
	aref	arDis, arRld;
	int location_index = FindLocation(pchar.questTemp.different.ShipyardsMap.city + "_town");
    makearef(arRld, Locations[location_index].reload);
    int		Qty = GetAttributesNum(arRld);
    for (int a=0; a<Qty; a++)
    {
    	arDis = GetAttributeN(arRld, a);
		sName = arDis.go;
    	if (findsubstr(sName, "_shipyard" , 0) != -1)
    	{
			arDis.close_for_night = true;
			break;
    	}
    }
	LocatorReloadEnterDisable(pchar.questTemp.different.ShipyardsMap.city + "_shipyard", "reload2", false);
}

// ----------------- Найти кольцо мэра в борделе ---------------------
void TakeMayorsRing_null(string qName) //нулим квест
{
	pchar.questTemp.different = "free";
	if (CheckCharacterItem(pchar, "MayorsRing"))
    {
		TakeItemFromCharacter(pchar, "MayorsRing");
		AddQuestRecord("SeekMayorsRing", "4");
		AddQuestUserData("SeekMayorsRing", "sSex", GetSexPhrase("","а"));
		AddQuestUserData("SeekMayorsRing", "sCity", XI_ConvertString("Colony" + pchar.questTemp.different.TakeMayorsRing.city + "Gen"));
		CloseQuestHeader("SeekMayorsRing");
		ChangeCharacterReputation(pchar, -3);
	}
	else
	{
		if (pchar.questTemp.different.TakeMayorsRing == "toBrothel")
		{
			AddQuestRecord("SeekMayorsRing", "5");
			AddQuestUserData("SeekMayorsRing", "sSex", GetSexPhrase("ел","ла"));
			AddQuestUserData("SeekMayorsRing", "sCity", XI_ConvertString("Colony" + pchar.questTemp.different.TakeMayorsRing.city + "Gen"));
			CloseQuestHeader("SeekMayorsRing");
			ChangeCharacterReputation(pchar, -5);
		}
	}
	DeleteAttribute(pchar, "questTemp.different.TakeMayorsRing");
}

// ----------------- Спихнуть судовые бумаги ---------------------
void GiveShipLetters_null(string qName) //нулим квест
{
	pchar.questTemp.different = "free";
	DeleteAttribute(pchar, "questTemp.different.GiveShipLetters");
}

void CheckShipLetters(string qName)
{
	if (!CheckCharacterItem(pchar, "CaptainBook"))
	{
		Log_QuestInfo(GetConvertStrWithReplace("Variable_reaction_functions_6", "Logs.txt", "#space#", " "));
		sld = ItemsFromID("CaptainBook");
		sld.shown = false;
		pchar.questTemp.different = "free";
		pchar.questTemp.different.GiveShipLetters.over = "yes"; //снимаем таймер
		DeleteAttribute(pchar, "questTemp.different.GiveShipLetters");
	}
}
// ----------------- Спихнуть судовые бумаги ---------------------

// ----------------- Развод на секс хозяйки борделя ---------------------
void SexWithHostess_null(string qName) //нулим квест
{
	if (pchar.questTemp.different.HostessSex == "toRoom")
	{
		sld = characterFromId(pchar.questTemp.different.HostessSex.city + "_Hostess");
		ChangeCharacterAddressGroup(sld, pchar.questTemp.different.HostessSex.city + "_SecBrRoom", "goto", "goto8");
		LocatorReloadEnterDisable(pchar.questTemp.different.HostessSex.city + "_Brothel", "reload2_back", true);
	}
	LAi_SetOwnerTypeNoGroup(sld);
	sld.dialog.currentnode = "First time";
	sld.quest.NotGoneToSex = true; //не пришёл. Секса больше не будет
	DeleteAttribute(pchar, "questTemp.different.HostessSex");
	pchar.questTemp.different = "free";
}

void SexWithHostess_goToRoom()
{
	chrDisableReloadToLocation = true;
	sld = characterFromId(pchar.questTemp.different.HostessSex.city + "_Hostess");
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorGoToLocation(sld, "reload", "reload1", pchar.questTemp.different.HostessSex.city + "_Brothel_room", "goto", "goto3", "OpenTheDoors", -1);
	pchar.quest.SexWithHostess_inRoom.win_condition.l1 = "location";
	pchar.quest.SexWithHostess_inRoom.win_condition.l1.location = pchar.questTemp.different.HostessSex.city + "_Brothel_room";
	pchar.quest.SexWithHostess_inRoom.function = "SexWithHostess_inRoom";
	LocatorReloadEnterDisable(pchar.questTemp.different.HostessSex.city + "_Brothel", "reload2_back", false);
}

void SexWithHostess_inRoom(string qName)
{
	DoQuestFunctionDelay("SexWithHostess_inRoom_2", 1.5);
	LocatorReloadEnterDisable(pchar.questTemp.different.HostessSex.city + "_Brothel", "reload2_back", true);
}

void SexWithHostess_inRoom_2(string qName)
{
	sld = characterFromId(pchar.questTemp.different.HostessSex.city + "_Hostess");
	sld.dialog.currentnode = "Hostess_inSexRoom";
	LAi_SetActorTypeNoGroup(sld);
	LAi_ActorDialog(sld, pchar, "", 0, 0);
}

void SexWithHostess_fack()
{
	pchar.questTemp.HorseQty = sti(pchar.questTemp.HorseQty) + 1;
	DoQuestCheckDelay("PlaySex_1", 1.0);
	pchar.questTemp.different = "HostessSex";
}

// ----------------- Миниквесты портмана  ---------------------
// Gregg: общие куски шести функций «кэп уходит в море/на карту» (Portman/Robber/Citiz/Mushket/Danielle/BlackBeard).
// Хвост постановки кэпа в морскую группу (у BlackBeard нет Task/Lock — его группа собирается вручную)
void CapSeaGroupTail(ref rCap, string sGroup)
{
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Group_AddCharacter(sGroup, rCap.id);
	Group_SetGroupCommander(sGroup, rCap.id);
	SetRandGeraldSail(rCap, sti(rCap.Nation));
}

// Перезанос сроков проверки по Id кэпа в базе нпс-кэпов
void CapUpdateCheckTime(ref rCap, int iDays)
{
	string sCapId = rCap.id;
	NullCharacter.capitainBase.(sCapId).checkTime = iDays + 2;
	NullCharacter.capitainBase.(sCapId).checkTime.control_day = GetDataDay();
	NullCharacter.capitainBase.(sCapId).checkTime.control_month = GetDataMonth();
	NullCharacter.capitainBase.(sCapId).checkTime.control_year = GetDataYear();
}

// Маршрут «из бухты своего города в бухту целевого» (цель выбирает вызывающий)
void CapPrepareShoreRoute(ref rCap, string sTargetCity)
{
	rCap.quest = "InMap"; //личный флаг искомого кэпа
	rCap.cityShore = GetIslandRandomShoreId(GetArealByCityName(rCap.city));
	rCap.quest.targetCity = sTargetCity;
	rCap.quest.targetShore = GetIslandRandomShoreId(GetArealByCityName(rCap.quest.targetCity));
}

// Выпуск кэпа на глобалку квестовым корабликом до целевой бухты (после CapPrepareShoreRoute)
void CapSailToTargetShore(ref rCap, string sShipTag, string sNameVar)
{
	rCap.mapEnc.type = "trade";
	//выбор типа кораблика на карте
	rCap.mapEnc.worldMapShip = sShipTag;
	rCap.mapEnc.Name = GetConvertStrWithReplace(sNameVar, "Names.txt", "#space#", " ");
	int daysQty = GetMaxDaysFromIsland2Island(GetArealByCityName(rCap.quest.targetCity), GetArealByCityName(rCap.city)) + 1; //дней доехать даем с запасом
	Map_CreateTrader(rCap.cityShore, rCap.quest.targetShore, rCap.id, daysQty);
	CapUpdateCheckTime(rCap, daysQty);
}

void SetCapitainFromCityToSea(string qName) //помещаем в море кэпа, который сейчас ошивается в городе
{
	if (!CheckAttribute(pchar, "quest." + qName + ".CapId")) return;
	int capIndex = GetCharacterIndex(pchar.quest.(qName).CapId)
	if (capIndex != -1)
	{
		sld = &characters[capIndex];
		if (LAi_IsDead(sld)) return;
		sld.location	= "none";
		sld.location.group = "";
		sld.location.locator = "";
		//в морскую группу кэпа
		string sGroup = "PorpmansShip_" + sld.index;
		group_DeleteGroup(sGroup);
		Group_FindOrCreateGroup(sGroup);
		Group_SetType(sGroup,"trade");
		CapSeaGroupTail(sld, sGroup);
		//записываем данные в структуру кэпа
		sld.quest = "InMap"; //личный флаг рассеянного кэпа
		sld.quest.targetCity = SelectNotEnemyColony(sld); //определим колонию, куда отправится кэп
		sld.quest.stepsQty = sti(sld.quest.stepsQty) + 1; //количество выходов в море
		Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_7", "Logs.txt", "#space#", " ") + sld.id + GetConvertStrWithReplace("Variable_reaction_functions_8", "Logs.txt", "#space#", " ") + sld.city + GetConvertStrWithReplace("Variable_reaction_functions_9", "Logs.txt", "#space#", " ") + sld.quest.targetCity);
		//определим бухту, куда ставить энкаунтер. чтобы сразу не генерился перед ГГ у города
		sTemp = GetArealByCityName(sld.city);
		sld.quest.baseShore = GetIslandRandomShoreId(sTemp);
		//на карту
		iTemp = GetMaxDaysFromIsland2Island(sTemp, GetArealByCityName(sld.quest.targetCity)) + 1; //дней доехать даем с запасом
		Map_CreateTrader(sld.quest.baseShore, sld.quest.targetCity, sld.id, iTemp);
		//даем общий слух, что кэп ушёл в другой город
		AddSimpleRumourEx(LinkRandPhrase(GetConvertStrWithReplace("Variable_reaction_functions_6", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_7", "AddRumours.txt", "#space#", " ") + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_8", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ".",
			GetConvertStrWithReplace("Variable_reaction_functions_9", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_10", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ".",
			GetConvertStrWithReplace("Variable_reaction_functions_11", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_12", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ". " + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_13", "AddRumours.txt", "#space#", " ")),
			sld.city, iTemp, 1, "PortmansBook_DeliveryToCap", sld.id);
		//--> запись инфы по кэпу в базу местного портмана
		sTemp = sld.id; //Id кэпа, который оставил отметку
		rCharacter = &characters[GetCharacterIndex(sld.City + "_PortMan")];
		rCharacter.quest.capitainsList.(sTemp) = sld.quest.targetCity; //куда отправился
		rCharacter.quest.capitainsList.(sTemp).date = GetDateString(); //запишем дату, когда отправился
		//ВНИМАНИЕ. в квестбук должна заносится типовая строка по примеру   PortmansBook_Delivery    #TEXT   5
		//в список портмана заносим тайтл, заголовок и номер строки из quest_text.txt
		rCharacter.quest.capitainsList.(sTemp).QBString1 = characters[GetCharacterIndex(sld.quest.firstCity + "_PortMan")].id + "PortmansBook_Delivery";
		rCharacter.quest.capitainsList.(sTemp).QBString2 = "PortmansBook_Delivery";
		rCharacter.quest.capitainsList.(sTemp).QBQty = 5;	//номер квестовой записи об инфе от портмана
		//перезаносим время в базу кэпов
		CapUpdateCheckTime(sld, iTemp);
	}
}

void SetRobberFromSeaToMap(string qName) //помещаем в море кэпа-вора, который счас стоит на рейде в порту
{
	if (!CheckAttribute(pchar, "quest." + qName + ".CapId")) return;
	int capIndex = GetCharacterIndex(pchar.quest.(qName).CapId)
	if (capIndex != -1)
	{
		sld = &characters[capIndex];
		if (LAi_IsDead(sld)) return;
		//в морскую группу кэпа
		string sGroup = "SeekCapShip_" + sld.index;
		group_DeleteGroup(sGroup);
		Group_FindOrCreateGroup(sGroup);
		CapSeaGroupTail(sld, sGroup);
		//записываем данные в структуру кэпа
		sld.quest = "InMap"; //личный флаг рассеянного кэпа
		sld.quest.targetCity = SelectAnyColony2(sld.city, sld.quest.cribCity); //определим колонию, куда отправится кэп
		Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_10", "Logs.txt", "#space#", " ") + sld.id + GetConvertStrWithReplace("Variable_reaction_functions_11", "Logs.txt", "#space#", " ") + sld.city + GetConvertStrWithReplace("Variable_reaction_functions_12", "Logs.txt", "#space#", " ") + sld.quest.targetCity);
		//на карту
		iTemp = GetMaxDaysFromIsland2Island(GetArealByCityName(sld.quest.targetCity), GetArealByCityName(sld.city)) + 1; //дней доехать даем с запасом
		Map_CreateTrader(sld.city, sld.quest.targetCity, sld.id, iTemp);
		//даем общий слух, что кэп ушёл в другой город
		AddSimpleRumourEx(LinkRandPhrase(GetConvertStrWithReplace("Variable_reaction_functions_14", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_15", "AddRumours.txt", "#space#", " ") + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_16", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ".",
			GetConvertStrWithReplace("Variable_reaction_functions_17", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_18", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ".",
			GetConvertStrWithReplace("Variable_reaction_functions_19", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_20", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ". " + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_21", "AddRumours.txt", "#space#", " ")),
			sld.city, iTemp, 1, "Portmans_SeekShip_rum", sld.id);
		//--> запись инфы по кэпу в базу местного портмана
		sTemp = sld.id; //Id кэпа, который оставил отметку
		rCharacter = &characters[GetCharacterIndex(sld.City + "_PortMan")];
		rCharacter.quest.capitainsList.(sTemp) = sld.quest.targetCity; //куда отправился
		rCharacter.quest.capitainsList.(sTemp).date = GetDateString(); //запишем дату, когда отправился
		rCharacter.quest.capitainsList.(sTemp).QBString1 = characters[GetCharacterIndex(sld.quest.cribCity + "_PortMan")].id + "Portmans_SeekShip";
		rCharacter.quest.capitainsList.(sTemp).QBString2 = "Portmans_SeekShip";
		rCharacter.quest.capitainsList.(sTemp).QBQty = 2;	//номер квестовой записи об инфе от портмана
		//перезаносим время в базу кэпов
		CapUpdateCheckTime(sld, iTemp);
	}
}

void SeekShip_checkAbordage(string qName) //кэп-вор успешно абордирован
{
	// LEO: проверяем возвращаемый корпус, а не смерть его прежнего капитана.
	sld = characterFromId(pchar.quest.(qName).CapId);
	rCharacter = &characters[GetCharacterIndex(sld.quest.cribCity + "_PortMan")];
	bOk = false;
	for (i=0; i<=COMPANION_MAX; i++)
	{
		iTemp = GetCompanionIndex(pchar, i);
		if(iTemp != -1)
		{
			sld = &characters[iTemp];
			if (sld.ship.name == rCharacter.quest.PortmansSeekShip.shipName &&
				RealShips[sti(sld.ship.type)].BaseName == rCharacter.quest.PortmansSeekShip.shipTapeName &&
				RealShips[sti(sld.Ship.Type)].basetype == rCharacter.quest.PortmansSeekShip.shipTape)
			{
				bOk = true;
			}
		}
	}
	//решение по результатам проверки
	sld = characterFromId(pchar.quest.(qName).CapId);
	if (!bOk)
	{
		// LEO: захваченный корпус можно подобрать из дрейфа; сдача всё равно требует его в эскадре.
		int driftIndex = GetCharacterIndex("_DriftCap_" + sld.id);
		if (driftIndex != -1)
		{
			ref drift = &characters[driftIndex];
			if (CheckAttribute(drift, "Ship.Type") && CheckAttribute(drift, "Ship.HP") && !CheckAttribute(drift, "Ship.Sink"))
			{
				bOk = sti(drift.Ship.Type) != SHIP_NOTUSED && sti(drift.Ship.Type) == sti(sld.Ship.Type) && stf(drift.Ship.HP) > 0.0;
			}
		}
	}
	sTemp = "SeekShip_checkSink" + rCharacter.index;
	pchar.quest.(sTemp).over = "yes"; //снимаем прерывание на утонул
	if (bOk)
	{
		rCharacter.quest = "SeekShip_success"; //флаг успешного взятия на абордаж
		sTemp = rCharacter.id + "Portmans_SeekShip";
		AddQuestRecordEx(sTemp, "Portmans_SeekShip", "5");
	}
	else
	{
		rCharacter.quest = "SeekShip_sink"; //флаг утопления судна, провал квеста
		sTemp = rCharacter.id + "Portmans_SeekShip";
		AddQuestRecordEx(sTemp, "Portmans_SeekShip", "9");
	}
}

void SeekShip_checkSink(string qName)
{
	sld = characterFromId(pchar.quest.(qName).CapId);
	rCharacter = &characters[GetCharacterIndex(sld.quest.cribCity + "_PortMan")];
	rCharacter.quest = "SeekShip_sink"; //флаг утопления судна, провал квеста
	sTemp = rCharacter.id + "Portmans_SeekShip";
	AddQuestRecordEx(sTemp, "Portmans_SeekShip", "8");
	sTemp = "SeekShip_checkAbordage" + rCharacter.index;
	pchar.quest.(sTemp).over = "yes"; //снимаем прерывание на абордаж
}

// ----------------- ночной сторож в церкви ---------------------
void ChurchNightGuard_exitFromLoc(string qName)
{
	DeleteAttribute(pchar, "PostEventQuest.questDelay.ChurchNightGuard_timerInChurch"); //снимаем таймер на отработку действия
	if (sti(pchar.questTemp.different.Church_NightGuard.NightQty) == 0) pchar.questTemp.different.Church_NightGuard.NightQty = 1;
	else pchar.questTemp.different.Church_NightGuard.NightQty = sti(pchar.questTemp.different.Church_NightGuard.NightQty) + GetQuestPastDayParam("questTemp.different.Church_NightGuard");
	Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_13", "Logs.txt", "#space#", " ") + pchar.questTemp.different.Church_NightGuard.NightQty);
	if (sti(pchar.questTemp.different.Church_NightGuard.NightQty) >= 3)
	{
		sTemp = pchar.questTemp.different.Church_NightGuard + "Church_NightGuard";
		AddQuestRecordEx(sTemp, "Church_NightGuard", "4");
		AddQuestUserData(sTemp, "sSex", GetSexPhrase("","а"));
		else
		{
			if (GetTime() > 6.0 && GetTime() < 23.99)
			{
				SaveCurrentQuestDateParam("questTemp.different.Church_NightGuard"); //перезапишем дату
			}
			else
			{
				sTemp = pchar.questTemp.different.Church_NightGuard + "Church_NightGuard";
				AddQuestRecordEx(sTemp, "Church_NightGuard", "3");
				AddQuestUserData(sTemp, "sSex", GetSexPhrase("","а"));
				AddQuestUserData(sTemp, "sSex1", GetSexPhrase("ен","на"));
				CloseQuestHeader(sTemp);
				DeleteAttribute(pchar, "questTemp.different.Church_NightGuard"); //снимаем флаг
				ChangeCharacterReputation(pchar, -20);
			}
		}
	}
}

void ChurchNightGuard_timerInChurch(string qName)
{
	int iRank, num;
	if (pchar.questTemp.different.Church_NightGuard.chance == pchar.questTemp.different.Church_NightGuard.NightQty)
	{	//грабители лезут
		pchar.quest.ChurchNightGuard_exitFromLoc.over = "yes"; //снимаем прерывание на выход из локации
		pchar.questTemp.different.Church_NightGuard.NightQty = 3; //присвоим максимум дней дежурства
		pchar.questTemp.different.Church_NightGuard.fight = true; //флаг боевки
        chrDisableReloadToLocation = true; // закрыть выход из локации.
		sTemp = pchar.questTemp.different.Church_NightGuard + "Church_NightGuard";
		AddQuestRecordEx(sTemp, "Church_NightGuard", "5");
        LAi_group_Delete("EnemyFight");
		//--> генерим ранг
		num = rand(1)+2;
		if (sti(pchar.rank) > 6)
		{
			if (sti(pchar.rank) > 20) iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*2.5/num);
			else iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*1.6/num);
		}
		else
		{	//казуалам зеленый свет на начало игры
			if (sti(pchar.rank) > 3) iRank = sti(pchar.rank);
			else
			{
				iRank = 1;
				num = 1;
			}
		}
		//<-- генерим ранг
		for (i=1; i<=num; i++)
        {
            sld = GetCharacter(NPC_GenerateCharacter("Bandit"+i, "officer_"+(i+10), "man", "man", iRank, PIRATE, 0, true));
			SetFantomParamFromRank(sld, iRank, true);
			LAi_SetWarriorType(sld);
			LAi_warrior_SetStay(sld, true);
			LAi_warrior_DialogEnable(sld, false);
            LAi_group_MoveCharacter(sld, "EnemyFight");
            ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload1");
        }
		sld.Dialog.Filename = "Quest\ForAll_dialog.c";
		sld.dialog.currentnode = "ChurchBerglar";
		GiveItem2Character(sld, "Chest");
		sld.SaveItemsForDead  = true;
		sld.DontClearDead = true;
        LAi_SetActorTypeNoGroup(sld);
		LAi_ActorDialog(sld, pchar, "", 1.0, 0);
	}
	else
	{	//нету никого
        SetLaunchFrameFormParam(GetConvertStr("Variable_reaction_functions_1", "Frames.txt"), "ChurchNightGuard_timerInChurch_2", 0, 3);
        LaunchFrameForm();
        RecalculateJumpTable();
	}
}
// ----------------- поиск кэпов, дача квеста горожанином ---------------------
void SCQ_seekCapIsDeath(string qName)
{
	string sTitle;
	sld = characterFromId(pchar.quest.(qName).CapId); //капитан
	rCharacter = &characters[GetCharacterIndex("QuestCitiz_" + sld.quest.cribCity)]; //горожанин-квестодатель
	//чистим базу нпс-кэпов  -->
	aref forName;
	makearef(forName, NullCharacter.capitainBase);
	DeleteAttribute(forName, sld.id);
	//<-- чистим базу нпс-кэпов
	if (rCharacter.quest.SeekCap == "manSlave")
	{
		sTitle = sld.quest.cribCity + "SCQ_" + rCharacter.quest.SeekCap;
		AddQuestRecordEx(sTitle, "SCQ_" + rCharacter.quest.SeekCap, "4");
		AddQuestUserData(sTitle, "sCapName", GetFullName(sld));
		AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + rCharacter.city + "Acc"));
		rCharacter.quest.SeekCap = rCharacter.quest.SeekCap + "over"; //дополняем флаг квест до 'выполнен'
	}
	if (sld.quest.SeekCap == "womanRevengeFight") //проверим флаг боя на улице, уже можно валить
	{
		sTitle = sld.quest.cribCity + "SCQ_" + rCharacter.quest.SeekCap;
		AddQuestRecordEx(sTitle, "SCQ_" + rCharacter.quest.SeekCap, "5");
		AddQuestUserData(sTitle, "sCapName", GetFullName(sld));
		AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + rCharacter.city + "Acc"));
		rCharacter.quest.SeekCap = rCharacter.quest.SeekCap + "over"; //дополняем флаг квест до 'выполнен'
		sTemp = "SecondTimer_" + sld.id;
		pchar.quest.(sTemp).over = "yes"; //снимаем возможно установленный таймер
		sTemp = "SCQ_" + rCharacter.index;
		pchar.quest.(sTemp).over = "yes"; //снимаем прерывание смерть квестодателя
	}
	if (sld.quest.SeekCap == "womanHasband" || rCharacter.quest.SeekCap == "manRapeWife" || sld.quest.SeekCap == "manFriend" || sld.quest.SeekCap == "womanRevenge" || rCharacter.quest.SeekCap == "womanPirates")
	{
		sTitle = sld.quest.cribCity + "SCQ_" + sld.quest.SeekCap;
		AddQuestRecordEx(sTitle, "SCQ_" + sld.quest.SeekCap, "4");
		sld.lifeDay = 0;
		Map_ReleaseQuestEncounter(sld.id);
		string sGroup = "SeekCapShip_" + sld.index;
		group_DeleteGroup(sGroup);
		sTemp = "SecondTimer_" + sld.id;
		pchar.quest.(sTemp).over = "yes"; //снимаем возможно установленный таймер
		sTemp = "SCQ_" + rCharacter.index;
		pchar.quest.(sTemp).over = "yes"; //снимаем прерывание смерть квестодателя
		sTitle = sld.quest.cribCity + "SCQ_" + sld.quest.SeekCap;
		CloseQuestHeader(sTitle);
		//снимаем горожанку-маньячку
		if (sld.quest.SeekCap == "womanRevenge")
		{
			rCharacter.lifeDay = 0;
		}
	}
}
//смерть квестодателя закрывает и чистит квест
void SCQ_CitizenIsDeath(string qName)
{
	sld = characterFromId(pchar.quest.(qName).CapId); //капитан
	sld.lifeDay = 0;
	rCharacter = characterFromId(pchar.quest.(qName).CitizenId); //горожанин-квестодатель
	//чистим базу нпс-кэпов  -->
	aref forName;
	makearef(forName, NullCharacter.capitainBase);
	DeleteAttribute(forName, sld.id);
	//<-- чистим базу нпс-кэпов
	Map_ReleaseQuestEncounter(sld.id);
	string sGroup = "SeekCapShip_" + sld.index;
	group_DeleteGroup(sGroup);
	sTemp = "SecondTimer_" + sld.id;
	pchar.quest.(sTemp).over = "yes"; //снимаем возможно установленный таймер
	sTemp = "SCQ_" + sld.index;
	pchar.quest.(sTemp).over = "yes"; //снимаем прерывание смерть кэпа
	string sTitle = sld.quest.cribCity + "SCQ_" + rCharacter.quest.SeekCap;
	CloseQuestHeader(sTitle);
}

void CitizCapFromSeaToMap(string qName) //помещаем на карту кэпа, разыскиваемого горожанами
{
	if (!CheckAttribute(pchar, "quest." + qName + ".CapId")) return;
	int capIndex = GetCharacterIndex(pchar.quest.(qName).CapId)
	if (capIndex != -1)
	{
		sld = &characters[capIndex];
		if (LAi_IsDead(sld)) return;
		sld.nation = sld.quest.nation;
		//в морскую группу кэпа
		string sGroup = "SeekCapShip_" + sld.index;
		group_DeleteGroup(sGroup);
		Group_FindOrCreateGroup(sGroup);
		CapSeaGroupTail(sld, sGroup);
		//записываем данные в структуру кэпа
		sld.quest = "InMap"; //личный флаг искомого кэпа
		sld.quest.targetCity = SelectAnyColony2(sld.city, sld.quest.cribCity); //определим колонию, куда отправится кэп
		Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_14", "Logs.txt", "#space#", " ") + sld.id + GetConvertStrWithReplace("Variable_reaction_functions_15", "Logs.txt", "#space#", " ") + sld.city + GetConvertStrWithReplace("Variable_reaction_functions_16", "Logs.txt", "#space#", " ") + sld.quest.targetCity);
		//на карту
		iTemp = GetMaxDaysFromIsland2Island(GetArealByCityName(sld.quest.targetCity), GetArealByCityName(sld.city)) + 1; //дней доехать даем с запасом
		Map_CreateTrader(sld.city, sld.quest.targetCity, sld.id, iTemp);
		//даем общий слух, что кэп ушёл в другой город
		AddSimpleRumourEx(LinkRandPhrase(GetConvertStrWithReplace("Variable_reaction_functions_22", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_23", "AddRumours.txt", "#space#", " ") + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_24", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ".",
			GetConvertStrWithReplace("Variable_reaction_functions_25", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_26", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ".",
			GetConvertStrWithReplace("Variable_reaction_functions_27", "AddRumours.txt", "#space#", " ") + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + GetConvertStrWithReplace("Variable_reaction_functions_28", "AddRumours.txt", "#space#", " ") + XI_ConvertString("Colony"+sld.quest.targetCity+"Acc") + ". " + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_29", "AddRumours.txt", "#space#", " ")),
			sld.city, iTemp, 1, "Citiz_SeekCap_rum", sld.id);
		//--> запись инфы по кэпу в базу местного портмана
		sTemp = sld.id; //Id кэпа, который оставил отметку
		rCharacter = &characters[GetCharacterIndex(sld.City + "_PortMan")];
		rCharacter.quest.capitainsList.(sTemp) = sld.quest.targetCity; //куда отправился
		rCharacter.quest.capitainsList.(sTemp).date = GetDateString(); //запишем дату, когда отправился
		rCharacter.quest.capitainsList.(sTemp).QBString1 = sld.quest.cribCity + "SCQ_" + characters[GetCharacterIndex("QuestCitiz_"+sld.quest.cribCity)].quest.SeekCap;
		rCharacter.quest.capitainsList.(sTemp).QBString2 = "SCQ_" + characters[GetCharacterIndex("QuestCitiz_"+sld.quest.cribCity)].quest.SeekCap;
		rCharacter.quest.capitainsList.(sTemp).QBQty = 2;
		//меняем сроки проверки по Id кэпа в базе нпс-кэпов
		CapUpdateCheckTime(sld, iTemp);
	}
}

//после боевки в каюте. для всех квестов по поисков кэпов ситезанами
void CitizSeekCap_afterCabinFight(string qName)
{
    //#20191003-01
    LAi_group_SetAlarm(LAI_GROUP_BRDENEMY, LAI_GROUP_PLAYER, 0.0);
	sld = GetCharacter(NPC_GenerateCharacter(pchar.quest.(qName).label + "_" + pchar.quest.(qName).WifeCity, pchar.quest.(qName).model, "woman", "towngirl", 5, pchar.quest.(qName).nation, -1, false));
	sld.name = pchar.quest.(qName).WifeName;
	sld.lastname = pchar.quest.(qName).WifeLastname;
	sld.dialog.filename   = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode = pchar.quest.(qName).label + "_Board";
	sld.quest.SeekCap = pchar.quest.(qName).label;
	sld.quest.cribCity = pchar.quest.(qName).WifeCity;
	LAi_SetStayType(sld);
	GetCharacterPos(pchar, &locx, &locy, &locz);
	ChangeCharacterAddressGroup(sld, pchar.location, "rld", LAi_FindFarLocator("rld", locx, locy, locz));
	LAi_SetActorType(pchar);
    LAi_SetActorType(sld);
    SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
	LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
}

//кораблик с капитаном, который имеет мушкет несчастного мушкетера
void SetMushketFromSeaToMap(string qName)
{
	sld = characterFromId("MushketCap");
	//в морскую группу кэпа
	string sGroup = "MushketCapShip";
	Group_FindOrCreateGroup(sGroup);
	CapSeaGroupTail(sld, sGroup);
	CapPrepareShoreRoute(sld, SelectAnyColony(sld.city)); //определим колонию, в бухту которой он придет
	Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_17", "Logs.txt", "#space#", " ") + sld.city + GetConvertStrWithReplace("Variable_reaction_functions_18", "Logs.txt", "#space#", " ") + sld.quest.targetShore);
	//==> на карту
	CapSailToTargetShore(sld, "quest_ship", "Variable_reaction_functions_21");
}

//новый галеон Даниэль
void SetDanielleFromSeaToMap(string qName)
{
	sld = characterFromId("Danielle");
	//в морскую группу кэпа
	string sGroup = "DanielleGroup";
	Group_FindOrCreateGroup(sGroup);
	CapSeaGroupTail(sld, sGroup);
	CapPrepareShoreRoute(sld, SelectAnyColony(sld.city)); //определим колонию, в бухту которой он придет
	Log_TestInfo("Кэп фрегата Queen вышел из: " + sld.city + GetConvertStrWithReplace("Variable_reaction_functions_19", "Logs.txt", "#space#", " ") + sld.quest.targetShore);
	//==> на карту
	CapSailToTargetShore(sld, "quest_ship", "Variable_reaction_functions_22");
}

//Чёрная Борода
void SetBlackBeardFromSeaToMap(string qName)
{
	sld = characterFromId("BlackBeardNPC");
	if (LAi_IsDead(sld)) return;
	//в морскую группу кэпа
	string sGroup = "BlackBeardGroup";
	Group_FindOrCreateGroup(sGroup);
	Group_AddCharacter(sGroup, sld.id);
	Group_SetGroupCommander(sGroup, sld.id);
	SetRandGeraldSail(sld, sti(sld.Nation));
	CapPrepareShoreRoute(sld, GetRandomPirateCity()); //определим колонию, в бухту которой он придет
	Log_TestInfo(GetConvertStrWithReplace("Variable_reaction_functions_20", "Logs.txt", "#space#", " ") + sld.city + GetConvertStrWithReplace("Variable_reaction_functions_21", "Logs.txt", "#space#", " ") + sld.quest.targetShore);
	//==> на карту
	CapSailToTargetShore(sld, "Tich_MKA", "Variable_reaction_functions_23");
}

bool BlackBeard_IsGoodRaidIsland(string sIsland)
{
	if (FindIsland(sIsland) == -1) return false;
	if (GetIslandRandomShoreId(sIsland) == "") return false;
	if (sIsland == "Dominica") return true;
	if (sIsland == "Terks") return true;
	if (sIsland == "StLucia") return true;
	return false;
}

string BlackBeard_GetRandomUninhabitedIsland()
{
	int iIsland = rand(2);

	if (iIsland == 0) return "Dominica";
	if (iIsland == 1) return "Terks";
	return "StLucia";
}

string BlackBeard_GetRaidIslandName(string sIsland)
{
	return GetConvertStr(sIsland, "LocLables.txt");
}

void BlackBeardRaid_CreateSquadron(string sIsland)
{
	ref sld;
	string sGroup = "BlackBeardRaidGroup";
	string sShipName;
	int iChar, i;

	group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup, "trade");

	for (i = 0; i < 4; i++)
	{
		sShipName = "BlackBeardRaid_" + i;
		iChar = NPC_GenerateCharacter(sShipName, "off_spa_2", "man", "man", 20 + rand(10), SPAIN, 15, true);
		makeref(sld, Characters[iChar]);
		SetRandomNameToCharacter(sld);
		sld.AlwaysEnemy = true;
		sld.AlwaysSandbankManeuver = true;
		sld.DontRansackCaptain = true;
		sld.dialog.filename = "Capitans_dialog.c";
		sld.dialog.currentnode = "GoldSquadron";
		sld.DeckDialogNode = "GoldSquadron";

		if (i < 2)
		{
			FantomMakeCoolSailor(sld, SHIP_WASA, "", CANNON_TYPE_CANNON_LBS16, 60, 60, 60);
			Fantom_SetCannons(sld, "war");
			Fantom_SetBalls(sld, "pirate");
			AddCharacterGoods(sld, GOOD_POWDER, 5000);
			AddCharacterGoods(sld, GOOD_SAILCLOTH, 500);
			AddCharacterGoods(sld, GOOD_PLANKS, 500);
			AddCharacterGoods(sld, GOOD_GOLD, 1000);
		}
		else
		{
			FantomMakeCoolSailor(sld, SHIP_SANTIAGO, "", CANNON_TYPE_CANNON_LBS12, 50, 50, 50);
			Fantom_SetCannons(sld, "war");
			Fantom_SetBalls(sld, "war");
		}

		SetCaptanModelByEncType(sld, "war");
		SetFantomParamHunter(sld);
		LAi_group_MoveCharacter(sld, "BlackBeardRaidGroup");
		Group_AddCharacter(sGroup, sld.id);

		if (i == 0)
		{
			Group_SetGroupCommander(sGroup, sld.id);
			sld.mapEnc.type = "trade";
			sld.mapEnc.worldMapShip = "Manowar_gold";
		}
		SetRandomNameToShip(sld);
	}

	Group_SetAddress(sGroup, sIsland, "Quest_Ships", "Quest_Ship_" + (9 + rand(1)));
	Group_SetTaskAttack(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);

	pchar.quest.BlackBeardRaid_GroupDead.win_condition.l1 = "Group_Death";
	pchar.quest.BlackBeardRaid_GroupDead.win_condition.l1.Group = sGroup;
	pchar.quest.BlackBeardRaid_GroupDead.function = "BlackBeardRaid_GroupDead";
}

void BlackBeardRaid_EnableFailEscape(string qName)
{
	if (!CheckAttribute(pchar, "questTemp.BlackBeardRaidStarted")) return;
	pchar.quest.BlackBeardRaid_FailEscape.win_condition.l1 = "MapEnter";
	pchar.quest.BlackBeardRaid_FailEscape.function = "BlackBeardRaid_FailEscape";
}

void BlackBeard_StartGoldRaid()
{
	ref bb = characterFromId("BlackBeardNPC");
	string sStartIsland, sStartShore;
	int iTry;

	if (LAi_IsDead(bb)) return;
	if (GetCompanionQuantity(pchar) >= COMPANION_MAX)
	{
		Log_Info("Нужен свободный слот в эскадре для Чёрной Бороды.");
		return;
	}

	pchar.quest.BlackBeardTimer.over = "yes";
	DeleteAttribute(pchar, "quest.BlackBeardRaid_EnableFailEscape");
	DeleteAttribute(pchar, "quest.BlackBeardRaid_GroupDead");
	DeleteAttribute(pchar, "quest.BlackBeardRaid_FailEscape");
	Map_ReleaseQuestEncounter(bb.id);
	Group_DeleteGroup("BlackBeardGroup");
	Group_DeleteGroup("Sea_" + bb.id);
	DeleteAttribute(&NullCharacter, "capitainBase.BlackBeardNPC");
	DeleteAttribute(bb, "quest");
	DeleteAttribute(bb, "mapEnc");
	bb.PGGAi.RemoveEncounter = true;

	pchar.questTemp.BlackBeardRaidStarted = true;
	if (CheckAttribute(pchar, "questTemp.BlackBeardRaidStartIsland") && BlackBeard_IsGoodRaidIsland(pchar.questTemp.BlackBeardRaidStartIsland))
	{
		sStartIsland = pchar.questTemp.BlackBeardRaidStartIsland;
	}
	else
	{
		sStartIsland = BlackBeard_GetRandomUninhabitedIsland();
		pchar.questTemp.BlackBeardRaidStartIsland = sStartIsland;
	}

	sStartShore = GetIslandRandomShoreId(sStartIsland);
	iTry = 0;
	while (sStartShore == "" && iTry < 20)
	{
		sStartIsland = BlackBeard_GetRandomUninhabitedIsland();
		sStartShore = GetIslandRandomShoreId(sStartIsland);
		iTry++;
	}
	if (sStartShore == "")
	{
		DeleteAttribute(pchar, "questTemp.BlackBeardRaidStarted");
		return;
	}
	pchar.questTemp.BlackBeardRaidStartIsland = sStartIsland;

	SetCharacterRelationBoth(sti(bb.index), GetMainCharacterIndex(), RELATION_FRIEND);
	SetRandGeraldSail(bb, sti(bb.Nation));
	bb.CompanionEnemyEnable = true;
	bb.PGGAi.IsPGG = false;
	bb.PGGAi.location.town = "none";
	DeleteAttribute(bb, "PGGAi.Task");
	DeleteAttribute(bb, "PGGAi.LockService");
	DeleteAttribute(bb, "AlwaysFriend");
	SetCompanionIndex(PChar, -1, sti(bb.index));
	SetCharacterRemovable(bb, false);
	SetBaseShipData(bb);
	DeleteAttribute(bb, "ship.sails");
	RepairMasts(bb);
	DeleteAttribute(bb, "ship.blots");
	Fantom_SetCannons(bb, "pirate");
	Fantom_SetBalls(bb, "pirate");
	bb.items.talisman3 = 1;
	EquipCharacterByItem(bb, "talisman3");

	BlackBeardRaid_CreateSquadron(sStartIsland);

	pchar.quest.BlackBeardRaid_EnableFailEscape.win_condition.l1 = "location";
	pchar.quest.BlackBeardRaid_EnableFailEscape.win_condition.l1.location = sStartIsland;
	pchar.quest.BlackBeardRaid_EnableFailEscape.function = "BlackBeardRaid_EnableFailEscape";

	pchar.quest.BlackBeardRaidTimer.win_condition.l1            = "Timer";
	pchar.quest.BlackBeardRaidTimer.win_condition.l1.date.day   = GetAddingDataDay(0, 0, 7);
	pchar.quest.BlackBeardRaidTimer.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 7);
	pchar.quest.BlackBeardRaidTimer.win_condition.l1.date.year  = GetAddingDataYear(0, 0, 7);
	pchar.quest.BlackBeardRaidTimer.function                    = "BlackBeardRaid_TimerExpired";

	Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_56", "Logs.txt", "#space#", " ") + BlackBeard_GetRaidIslandName(sStartIsland) + GetConvertStr("Variable_reaction_functions_60", "Logs.txt"));
}

void BlackBeardRaid_GroupDead(string qName)
{
	ref bb = characterFromId("BlackBeardNPC");

	if (LAi_IsDead(bb)) return;

	pchar.quest.BlackBeardRaidTimer.over = "yes";
	bb.AlwaysEnemy = true;
	DeleteAttribute(bb, "AlwaysFriend");
	bb.ship.hp = GetCharacterShipHP(bb);
	bb.ship.SP = GetCharacterShipSP(bb);
	DeleteAttribute(bb, "ship.sails");
	DeleteAttribute(bb, "ship.blots");
	RepairMasts(bb);
	SetCrewQuantityFull(bb);
	SeaAI_SetCompanionEnemy(bb);
	pchar.Titch_seabattle = true;
	DeleteAttribute(pchar, "questTemp.BlackBeardRaidStarted");
	DeleteAttribute(pchar, "questTemp.BlackBeardRaidStartIsland");
	pchar.quest.BlackBeardRaid_FailEscape.over = "yes";
	Log_Info(GetConvertStr("Variable_reaction_functions_57", "Logs.txt"));
}

//==> Gregg: генератор ростовщика "Долг капитана" (Gen_LoanCapDebt)
// Морская часть переиспользует штатную машинерию розыска кэпов: базу
// NullCharacter.capitainBase, список портового управления и Map_CreateTrader.
// Своего тут только генерация должника, развязки и уборка.
//
// СОСТОЯНИЕ КВЕСТА - ровно три изменяемых поля, всё остальное неизменно с момента
// генерации. Прошлая схема (State + Paid + Force + Lied + Revealed + Proof + Where)
// кодировала одни и те же факты по нескольку раз, и ветки разъезжались.
//
//   pchar.GenQuest.LoanCap.Stage - offered | seek | fighting | paid | forced | spared | promised
//   pchar.GenQuest.LoanCap.Knows - no | word | proof   (что ГГ знает о завышении долга)
//   pchar.GenQuest.LoanCap.Held  - сколько песо ГГ держит ДЛЯ ростовщика (0 = не собирал)
//
// Held материализован намеренно. Раньше факт сбора денег УГАДЫВАЛСЯ по состоянию,
// причём каждый узел ростовщика угадывал заново - отсюда и реплика "тело долга
// кладите сюда" при нулевом движении денег. Теперь и списание, и гейт кошелька,
// и текст реплики берут одно и то же число.
//
// Сцена (палуба/каюта) в состоянии квеста НЕ живёт: это свойство текущего разговора,
// а не квеста. Она живёт на самом собеседнике (npchar.LoanCapScene) и читается только
// через CheckAttribute: отсутствующий атрибут в Storm - рантайм-ошибка, а не "ложь".
//
// Факт "ростовщик завысил долг" отдельным полем не хранится - он выводится
// однозначно: RealSum < Sum. Отдельный флаг мог бы только разойтись с суммами.

// Gregg: создаём должника-капитана. На карту он ещё НЕ выходит — это делает
// LoanCap_Launch по факту согласия ГГ. Разделено, чтобы ростовщик мог назвать
// имя должника и корабль до того, как игрок возьмётся за дело.
void LoanCap_CreateDebtor(ref npchar, int iNation)
{
	int iRank = sti(pchar.rank) + 3;
	if (iRank > 30) iRank = 30;

	ref rCap = GetCharacter(NPC_GenerateCharacter("LoanCap_" + npchar.index, "", "man", "man", iRank, iNation, -1, true));
	SetCaptanModelByEncType(rCap, "trade");
	SetShipToFantom(rCap, "trade", true);
	rCap.Ship.Mode = "trade";
	rCap.dialog.filename = "Quest\ForAll_dialog.c";
	rCap.dialog.currentnode = "LoanCap";
	rCap.DeckDialogNode = "LoanCap_inDeck";

	DeleteAttribute(rCap, "SinkTenPercent");
	DeleteAttribute(rCap, "SaveItemsForDead");
	DeleteAttribute(rCap, "DontClearDead");
	DeleteAttribute(rCap, "AboardToFinalDeck");
	DeleteAttribute(rCap, "DontRansackCaptain");
	DeleteAttribute(rCap, "Killer"); // Gregg: слот мог быть переиспользован, статус смерти чужой
	DeleteAttribute(rCap, "DontDeskTalk"); // Gregg: мирная развязка прошлого экземпляра затыкала палубный диалог
	rCap.AlwaysSandbankManeuver = true;
	rCap.AnalizeShips = true;
	rCap.DontRansackCaptain = true; // Gregg: не сдаётся по общему диалогу, разговор только квестовый

	SetCharacterPerk(rCap, "FastReload");
	SetCharacterPerk(rCap, "ShipSpeedUp");
	SetCharacterPerk(rCap, "ShipTurnRateUp");
	SetCharacterPerk(rCap, "StormProfessional");
	SetCharacterPerk(rCap, "SwordplayProfessional");
	SetCharacterPerk(rCap, "AdvancedDefense");
	SetCharacterPerk(rCap, "Tireless");

	rCap.quest = "offered";
	rCap.quest.nation = iNation;
	rCap.quest.cribCity = npchar.city;
	// Gregg: должник брал заём здесь и отсюда же снялся с якоря — иначе запись
	// в списке ПУ ложится в случайный порт и игроку негде взять наводку
	rCap.city = npchar.city;
	rCap.quest.targetCity = SelectAnyColony(npchar.city);

	pchar.GenQuest.LoanCap.CapId = rCap.id;
	pchar.GenQuest.LoanCap.CapName = GetFullName(rCap);
	pchar.GenQuest.LoanCap.ShipName = rCap.Ship.name;
	pchar.GenQuest.LoanCap.ShipType = GetStrSmallRegister(XI_ConvertString(RealShips[sti(rCap.Ship.Type)].BaseName));

	// Gregg: примерно треть должников - разорившиеся честные люди, а не жулики
	// (открывает ветку "закрыть долг за свой счёт"), примерно четверть ростовщиков
	// завышает сумму долга в свою пользу.
	pchar.GenQuest.LoanCap.Honest = "no";
	if (rand(2) == 0) pchar.GenQuest.LoanCap.Honest = "yes";
	bool bLied = false;
	if (rand(3) == 0) bLied = true;

	// Gregg: бета-меню ростовщика задаёт вариант принудительно, минуя рандом.
	// Флаг разовый - стираем сразу, как GWIK.BetaForce* в конторе ГВИК.
	if (CheckAttribute(pchar, "questTemp.LoanCapBeta"))
	{
		sTemp = pchar.questTemp.LoanCapBeta;
		DeleteAttribute(pchar, "questTemp.LoanCapBeta");
		pchar.GenQuest.LoanCap.Honest = "no";
		bLied = false;
		if (sTemp == "honest") pchar.GenQuest.LoanCap.Honest = "yes";
		if (sTemp == "lied") bLied = true;
		if (sTemp == "both")
		{
			pchar.GenQuest.LoanCap.Honest = "yes";
			bLied = true;
		}
	}

	// Gregg: настоящий долг считаем ПОСЛЕ решения флагов - и рандом, и бета сюда сходятся.
	// RealSum < Sum и есть признак завышения, отдельного поля под него нет.
	pchar.GenQuest.LoanCap.RealSum = pchar.GenQuest.LoanCap.Sum;
	if (bLied)
	{
		iTemp = makeint(sti(pchar.GenQuest.LoanCap.Sum) * (65 + rand(15)) / 100);
		pchar.GenQuest.LoanCap.RealSum = iTemp;
	}

	// Gregg: атрибуты состояния должны СУЩЕСТВОВАТЬ к моменту, когда о квесте узнает
	// кто-либо ещё: короткого замыкания в Storm нет, и связка CheckAttribute && значение
	// всё равно читает отсутствующий атрибут. Stage пишется последним - он же признак
	// того, что экземпляр собран целиком.
	pchar.GenQuest.LoanCap.Knows = "no";
	pchar.GenQuest.LoanCap.Held = 0;

	// Gregg: бета-меню зовёт Clear() и MakeOffer() в одном кадре. Clear ставит
	// over = "yes", а QuestsCheck в конце кадра сносит ВСЁ, у чего есть over -
	// свежие узлы погибли бы вместе со старыми. Пишем начисто.
	DeleteAttribute(pchar, "quest.LoanCap_UsurerIsDeath");
	DeleteAttribute(pchar, "quest.LoanCap_Deadline");
	DeleteAttribute(pchar, "quest.LoanCap_ReportDue");

	// Gregg: прерывание на гибель ростовщика вешаем уже здесь, иначе его смерть
	// между предложением и согласием оставит висеть GenQuest.LoanCap навсегда
	pchar.quest.LoanCap_UsurerIsDeath.win_condition.l1 = "NPC_Death";
	pchar.quest.LoanCap_UsurerIsDeath.win_condition.l1.character = npchar.id;
	pchar.quest.LoanCap_UsurerIsDeath.function = "LoanCap_UsurerIsDeath";

	// Gregg: срок протухания непринятого предложения. Гейт выдачи глобальный
	// ("уже есть GenQuest.LoanCap - не предлагать"), поэтому если игрок просто уйдёт
	// из диалога, не сказав ни да ни нет, генератор встанет на всю игру.
	// LoanCap_Launch перезапишет этот таймер настоящим сроком исполнения.
	LoanCap_ArmTimer("LoanCap_Deadline", 7, "LoanCap_Deadline");
}

// Gregg: собрать предложение ростовщика. Общая точка для обычного ролла и бета-меню,
// чтобы условия задания не разъезжались между ними.
void LoanCap_MakeOffer(ref npchar)
{
	pchar.GenQuest.LoanCap.Sum        = (12 + cRand(sti(PChar.rank))) * 1500 * GetCharacterSPECIALSimple(PChar, SPECIAL_L) + cRand(20) * 300;
	pchar.GenQuest.LoanCap.Percent    = makeint(sti(pchar.GenQuest.LoanCap.Sum) / (3 + rand(2)));
	pchar.GenQuest.LoanCap.Days       = 30 + rand(15);
	pchar.GenQuest.LoanCap.UsurerId   = npchar.id;
	pchar.GenQuest.LoanCap.UsurerCity = npchar.city;
	// Gregg: имя ростовщика кэшируем. Должник поминает его посреди абордажа, а лезть
	// за живым персонажем оттуда незачем - имя за время квеста не меняется.
	pchar.GenQuest.LoanCap.UsurerName = GetFullName(npchar);
	LoanCap_CreateDebtor(npchar, sti(npchar.nation));
	pchar.GenQuest.LoanCap.Stage      = "offered";
	npchar.quest.LoanCap = "offered";
}

// Gregg: ГГ согласился — выпускаем должника на карту и вешаем прерывания
void LoanCap_Launch(ref npchar)
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.CapId")) return;
	int capIndex = GetCharacterIndex(pchar.GenQuest.LoanCap.CapId);
	if (capIndex == -1) return;

	ref rCap = &characters[capIndex];

	string sGroup = "LoanCapShip_" + rCap.index;
	group_DeleteGroup(sGroup); // Gregg: Group_FindOrCreateGroup переиспользует чужой состав
	Group_FindOrCreateGroup(sGroup);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Group_AddCharacter(sGroup, rCap.id);
	Group_SetGroupCommander(sGroup, rCap.id);
	SetRandGeraldSail(rCap, sti(rCap.Nation));

	rCap.quest = "InMap";
	rCap.mapEnc.type = "trade";
	rCap.mapEnc.worldMapShip = "Galleon_red";
	rCap.mapEnc.Name = XI_ConvertString(RealShips[sti(rCap.Ship.Type)].BaseName) + " '" + rCap.Ship.name + "'";

	int iDays = GetMaxDaysFromIsland2Island(GetArealByCityName(rCap.city), GetArealByCityName(rCap.quest.targetCity)) + 1;
	Map_CreateTrader(rCap.city, rCap.quest.targetCity, rCap.id, iDays);
	LoanCap_WriteToPortman(rCap, iDays);

	// Gregg: прерывание на гибель должника
	sTemp = "LoanCap_" + rCap.id;
	DeleteAttribute(pchar, "quest." + sTemp); // Gregg: стереть протухшее over от прошлого экземпляра
	pchar.quest.(sTemp).win_condition.l1 = "NPC_Death";
	pchar.quest.(sTemp).win_condition.l1.character = rCap.id;
	pchar.quest.(sTemp).function = "LoanCap_CapIsDeath";
	pchar.quest.(sTemp).CapId = rCap.id;

	// Gregg: срок исполнения поверх срока протухания предложения
	LoanCap_ArmTimer("LoanCap_Deadline", sti(pchar.GenQuest.LoanCap.Days), "LoanCap_Deadline");

	// Gregg: регистрация в базе нпс-кэпов, чтобы полугодовая чистка не потеряла должника
	sTemp = rCap.id;
	NullCharacter.capitainBase.(sTemp).quest      = "loancap";
	NullCharacter.capitainBase.(sTemp).questGiver = npchar.id;
	NullCharacter.capitainBase.(sTemp).Tilte1     = "Gen_LoanCapDebt";
	NullCharacter.capitainBase.(sTemp).Tilte2     = "Gen_LoanCapDebt";
	NullCharacter.capitainBase.(sTemp).checkTime  = iDays + 2;
	NullCharacter.capitainBase.(sTemp).checkTime.control_day   = GetDataDay();
	NullCharacter.capitainBase.(sTemp).checkTime.control_month = GetDataMonth();
	NullCharacter.capitainBase.(sTemp).checkTime.control_year  = GetDataYear();
}

// Gregg: единая постановка квестового таймера. Узел всегда пишется НАЧИСТО: over = "yes"
// не отключает узел, а помечает его на удаление в конце кадра, и дозапись поверх такого
// узла умерла бы вместе с ним.
void LoanCap_ArmTimer(string sNode, int iDays, string sFunc)
{
	DeleteAttribute(pchar, "quest." + sNode);
	pchar.quest.(sNode).win_condition.l1            = "Timer";
	pchar.quest.(sNode).win_condition.l1.date.day   = GetAddingDataDay(0, 0, iDays);
	pchar.quest.(sNode).win_condition.l1.date.month = GetAddingDataMonth(0, 0, iDays);
	pchar.quest.(sNode).win_condition.l1.date.year  = GetAddingDataYear(0, 0, iDays);
	pchar.quest.(sNode).function                    = sFunc;
}

// Gregg: запись о должнике в список портового управления города, откуда он вышел
void LoanCap_WriteToPortman(ref rCap, int iDays)
{
	int portIndex = GetCharacterIndex(rCap.City + "_PortMan");
	if (portIndex == -1) return;

	string sCapId = rCap.id;
	rCharacter = &characters[portIndex];
	rCharacter.quest.capitainsList.(sCapId)            = rCap.quest.targetCity;
	rCharacter.quest.capitainsList.(sCapId).date       = GetDateString();
	rCharacter.quest.capitainsList.(sCapId).QBString1  = "Gen_LoanCapDebt";
	rCharacter.quest.capitainsList.(sCapId).QBString2  = "Gen_LoanCapDebt";
	rCharacter.quest.capitainsList.(sCapId).QBQty      = 2;
}

// Gregg: должник по ходу маршрута отмечается в списке КАЖДОГО порта отправления.
// Common_Portman.c при показе списка разыменовывает персонажа (GetFullName, Ship.Type),
// поэтому перед удалением должника записи надо вымести отовсюду.
void LoanCap_ClearPortmanRecords(string sCapId)
{
	int n;
	int portIndex;
	aref arList;
	for (n = 0; n < MAX_COLONIES; n++)
	{
		portIndex = GetCharacterIndex(Colonies[n].id + "_PortMan");
		if (portIndex == -1) continue;
		if (!CheckAttribute(&characters[portIndex], "quest.capitainsList")) continue;
		makearef(arList, characters[portIndex].quest.capitainsList);
		DeleteAttribute(arList, sCapId);
	}
}

// Gregg: полная уборка квеста. Безопасна при мёртвом/несуществующем кэпе.
void LoanCap_Clear()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap")) return;

	string sCapId = "";
	if (CheckAttribute(pchar, "GenQuest.LoanCap.CapId")) sCapId = pchar.GenQuest.LoanCap.CapId;

	if (sCapId != "")
	{
		sTemp = "LoanCap_" + sCapId;
		pchar.quest.(sTemp).over = "yes";
		sTemp = "SecondTimer_" + sCapId;
		pchar.quest.(sTemp).over = "yes";

		if (CheckAttribute(NullCharacter, "capitainBase"))
		{
			aref arBase;
			makearef(arBase, NullCharacter.capitainBase);
			DeleteAttribute(arBase, sCapId);
		}

		Map_ReleaseQuestEncounter(sCapId);
		LoanCap_ClearPortmanRecords(sCapId);

		int capIndex = GetCharacterIndex(sCapId);
		if (capIndex != -1)
		{
			sTemp = "LoanCapShip_" + capIndex;
			group_DeleteGroup(sTemp);
			characters[capIndex].lifeDay = 0;
		}
	}

	// Gregg: освобождаем личный флаг ростовщика, чтобы он снова мог выдавать работу
	if (CheckAttribute(pchar, "GenQuest.LoanCap.UsurerId"))
	{
		int usurerIndex = GetCharacterIndex(pchar.GenQuest.LoanCap.UsurerId);
		if (usurerIndex != -1) DeleteAttribute(&characters[usurerIndex], "quest.LoanCap");
	}

	pchar.quest.LoanCap_Deadline.over = "yes";
	pchar.quest.LoanCap_ReportDue.over = "yes";
	pchar.quest.LoanCap_UsurerIsDeath.over = "yes";
	// Gregg: пленник, оставшийся на руках, перестаёт быть квестовым - иначе его
	// нельзя ни ссадить, ни как-то от него избавиться, и он висит в трюме вечно.
	LoanCap_FreeCaptive();
	DeleteAttribute(pchar, "GenQuest.LoanCap");
}

// Gregg: сейв мог быть сделан на прошлой схеме состояния (State/Paid/Force/Revealed/
// Proof/Where). Переносить оттуда нечего - квест не выходил за пределы ветки, - но
// бросить экземпляр нельзя: гейт выдачи глобальный, и генератор встал бы у всех
// ростовщиков архипелага навсегда. Поэтому старый экземпляр просто закрываем.
void LoanCap_Migrate()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap")) return;
	if (CheckAttribute(pchar, "GenQuest.LoanCap.Stage")) return;

	CloseQuestHeader("Gen_LoanCapDebt");
	LoanCap_Clear();
}

// Gregg: --> производные величины. Все три - ФУНКЦИИ, а не атрибуты: хранимая копия
// рано или поздно разъезжается с тем, из чего её посчитали.

// Gregg: сколько снимаем с должника. ВСЕГДА полная названная сумма: должник обязан
// ростовщику по расписке, и торг об её размере - разговор не с ним, а с ростовщиком.
int LoanCap_Collect()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Sum")) return 0;
	return sti(pchar.GenQuest.LoanCap.Sum);
}

// Gregg: гонорар ГГ. Не пересчитывается ни при каких обстоятельствах - он оговорён
// при выдаче поручения, и вскрытие подлога его не касается.
int LoanCap_Fee()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Percent")) return 0;
	return sti(pchar.GenQuest.LoanCap.Percent);
}

// Gregg: настоящий долг капитана. Ровно то, что он реально брал.
int LoanCap_RealSum()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.RealSum")) return 0;
	return sti(pchar.GenQuest.LoanCap.RealSum);
}

// Gregg: сколько ростовщик имеет право оставить себе. Узнал ГГ настоящую цифру -
// ростовщик забирает её, а разницу возвращает: излишек он выманивал у ГГ, не у должника.
int LoanCap_Demand()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Sum")) return 0;
	// Gregg: "yes" - ГГ знает настоящую цифру, но ростовщику её ещё не предъявил;
	// "shown" - предъявил и тот признал. Пересчёт идёт только по второму.
	if (pchar.GenQuest.LoanCap.Knows == "shown") return sti(pchar.GenQuest.LoanCap.RealSum);
	return sti(pchar.GenQuest.LoanCap.Sum);
}

// Gregg: сколько ростовщик возвращает ГГ сверх процентов - излишек собранного над
// тем, что он вправе оставить себе. Ноль, если ГГ снял ровно настоящий долг либо
// обман так и не вскрыт. Считается ОТ ФАКТА, а не от предполагаемой разницы сумм:
// игрок сам решает, сколько снять с должника, и вычесть надо именно снятое.
int LoanCap_Change()
{
	int iBack = LoanCap_Held() - LoanCap_Demand();
	if (iBack < 0) return 0;
	return iBack;
}

// Gregg: недостача - ГГ принёс меньше, чем ростовщик считает своим
int LoanCap_Shortage()
{
	int iShort = LoanCap_Demand() - LoanCap_Held();
	if (iShort < 0) return 0;
	return iShort;
}

// Gregg: сколько денег ГГ держит ДЛЯ ростовщика
int LoanCap_Held()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Held")) return 0;
	return sti(pchar.GenQuest.LoanCap.Held);
}

// Gregg: ростовщик завысил долг. Отдельного флага нет намеренно - признак ровно один.
bool LoanCap_IsLied()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.RealSum")) return false;
	if (sti(pchar.GenQuest.LoanCap.RealSum) < sti(pchar.GenQuest.LoanCap.Sum)) return true;
	return false;
}

// Gregg: текущая стадия; "" если квеста нет. Позволяет узлам диалога спрашивать
// состояние одной строкой, не рискуя чтением отсутствующего атрибута.
string LoanCap_Stage()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Stage")) return "";
	return pchar.GenQuest.LoanCap.Stage;
}
// Gregg: <-- производные величины

// Gregg: развязка состоялась - снимаем погоню, но квест держим открытым до отчёта.
// bDropDeathHook = false нужен ровно для плена: там оригинального кэпа добивает форма
// трофея, и прерывание на его гибель обязано дожить до неё - именно в нём мы
// усыновляем пленника (образец - GWIK_RansomBoarded).
void LoanCap_Released(bool bDropDeathHook)
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.CapId")) return;

	string sCapId = pchar.GenQuest.LoanCap.CapId;

	if (bDropDeathHook)
	{
		sTemp = "LoanCap_" + sCapId;
		pchar.quest.(sTemp).over = "yes";
	}
	sTemp = "SecondTimer_" + sCapId;
	pchar.quest.(sTemp).over = "yes";
	pchar.quest.LoanCap_Deadline.over = "yes";

	if (CheckAttribute(NullCharacter, "capitainBase"))
	{
		aref arBase;
		makearef(arBase, NullCharacter.capitainBase);
		DeleteAttribute(arBase, sCapId);
	}
	Map_ReleaseQuestEncounter(sCapId);
	LoanCap_ClearPortmanRecords(sCapId);

	int capIndex = GetCharacterIndex(sCapId);
	if (capIndex != -1)
	{
		characters[capIndex].quest = "outMap";
	}
}

// Gregg: ЕДИНСТВЕННАЯ точка фиксации развязки с должником. Все исходы идут сюда,
// поэтому деньги, репутация, запись в журнал и таймер отчёта физически не могут
// разъехаться между ветками - раньше каждая ветка делала это по-своему.
//
// iTake - сколько песо реально снято с должника (0 для развязок без денег). Считает
// его ВЫЗЫВАЮЩИЙ узел: узнав настоящий долг, ГГ вправе взять и его, и объявленный.
//   paid     - должник рассчитался (палуба или каюта)
//   forced   - ГГ добил его в каюте и забрал деньги
//   note     - должник разорён, взяли расписку
//   prisoner - должник разорён, взяли в плен (корабль и груз - трофеи)
//   promised - ГГ пообещал закрыть долг из своего кармана
//   spared   - ГГ отпустил его ни с чем
//
// Гард по стадии даёт однократность: повторная встреча сюда не проходит.
void LoanCap_Outcome(string sStage, string sRecord, int iTake)
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Stage")) return;

	string sNow = pchar.GenQuest.LoanCap.Stage;
	bool bOpen = false;
	if (sNow == "seek") bOpen = true;
	if (sNow == "fighting") bOpen = true; // Gregg: силовая развязка приходит из объявленного боя
	if (!bOpen) return;

	pchar.GenQuest.LoanCap.Stage = sStage;
	pchar.GenQuest.LoanCap.Held  = iTake;
	if (iTake > 0) AddMoneyToCharacter(pchar, iTake + LoanCap_Fee());

	// Gregg: репутация за то, что разошлись без крови - ровно здесь и ровно один раз.
	// Раньше её выдавали и палубный узел, и каютный, и узел ростовщика - выходило
	// по два начисления за одно прохождение.
	// Очко доброго дела здесь НЕ начисляется: расстаться миром и потом присвоить
	// деньги - не доброе дело. Очко даёт только доброе ЗАКРЫТИЕ квеста у ростовщика.
	if (sStage == "forced")
	{
		ChangeCharacterReputation(pchar, -2);
		OfficersReaction("bad");
	}
	// Gregg: плен - не подвиг и не злодейство: должник законно отвечает по расписке,
	// а корабль и груз ГГ уже забрал формой трофея. Репутация не двигается.
	if (sStage == "paid")
	{
		ChangeCharacterReputation(pchar, 2);
		OfficersReaction("good");
	}
	if (sStage == "note")
	{
		ChangeCharacterReputation(pchar, 2);
		OfficersReaction("good");
	}
	if (sStage == "promised")
	{
		ChangeCharacterReputation(pchar, 2);
		OfficersReaction("good");
	}
	if (sStage == "spared")
	{
		ChangeCharacterReputation(pchar, 2);
		OfficersReaction("good");
	}

	AddQuestRecord("Gen_LoanCapDebt", sRecord);

	// Gregg: прерывание на гибель кэпа держим живым только для плена - см. LoanCap_Released
	if (sStage == "prisoner")
	{
		LoanCap_Released(false);
	}
	else
	{
		LoanCap_Released(true);
	}

	// Gregg: срок отчёта. Без него "развязался и не вернулся" было бесплатным
	// присвоением, а квест висел в журнале вечно и глухо блокировал генератор.
	LoanCap_ArmTimer("LoanCap_ReportDue", 30, "LoanCap_ReportDue");
}

// Gregg: должник добит в каюте — деньги берём силой. Имя кейса приходит из
// QuestAboardCabinDialogExitWithBattle и разбирается в switch(sQuestName) в
// quests_reaction.c, по имени функции этот вызов НЕ идёт.
void LoanCap_afterCabinFight(string qName)
{
	LoanCap_Outcome("forced", "23", LoanCap_Collect());
}

// Gregg: ГГ узнал у должника настоящий размер долга. Величины не мутируем: и Demand,
// и Gap - функции от Knows, поэтому повторный заход узла безвреден и гард не нужен.
void LoanCap_LearnRealSum()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Knows")) return;
	if (pchar.GenQuest.LoanCap.Knows != "no") return;

	pchar.GenQuest.LoanCap.Knows = "yes";

	AddQuestRecord("Gen_LoanCapDebt", "14");
	AddQuestUserData("Gen_LoanCapDebt", "sRealMoney", FindRussianMoneyString(sti(pchar.GenQuest.LoanCap.RealSum)));
}

// Gregg: ГГ решил брать долг силой. Прерывание на гибель должника НЕ снимаем:
// over = "yes" не отключает, а помечает узел на удаление, после чего гибель должника
// не отследить уже никогда. Стадия fighting отделяет "бой объявлен" от "развязка
// состоялась" - без неё отступление с абордажа оставляло состояние неотличимым от
// нетронутого квеста.
void LoanCap_ArmForce()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Stage")) return;
	if (pchar.GenQuest.LoanCap.Stage != "seek") return;

	pchar.GenQuest.LoanCap.Stage = "fighting";
}

// Gregg: разорённый должник сдан ростовщику живьём. Пленника усыновляем в
// LoanCap_CapIsDeath - к моменту его вызова QuestAboardCabinDialogQuestSurrender уже
// записал GenQuest.LastQuestPrisonerIdx (образец - ГВИК, utils.c GWIK_RansomBoarded).
void LoanCap_AdoptCaptive()
{
	if (!CheckAttribute(pchar, "GenQuest.LastQuestPrisonerIdx")) return;
	int iCap = sti(pchar.GenQuest.LastQuestPrisonerIdx);
	if (iCap < 0) return;

	ref rCap = GetCharacter(iCap);
	pchar.GenQuest.LoanCap.CaptiveId = rCap.id;
	SetCharacterRemovable(rCap, false);     // Gregg: квестовый пленник - не ссадить и не продать
	DeleteAttribute(rCap, "Hold_GenQuest"); // Gregg: SetCharToPrisoner с шансом 1/2 вешает откупной хук
}

// Gregg: пленник передан ростовщику - уходит в долговую яму, слот пассажира свободен
void LoanCap_DeliverCaptive()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.CaptiveId")) return;
	int iCap = GetCharacterIndex(pchar.GenQuest.LoanCap.CaptiveId);
	if (iCap == -1) return;

	RemovePassenger(pchar, &characters[iCap]);
	characters[iCap].location = "";
	LAi_SetCurHP(&characters[iCap], 0.0);
}

// Gregg: пленник остался у ГГ на руках, а квест закрылся (срок, смерть ростовщика).
// Снимаем квестовую защиту, чтобы пассажира можно было ссадить обычным порядком.
void LoanCap_FreeCaptive()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.CaptiveId")) return;
	int iCap = GetCharacterIndex(pchar.GenQuest.LoanCap.CaptiveId);
	if (iCap == -1) return;

	SetCharacterRemovable(&characters[iCap], true);
}

// Gregg: благодарность должника - наводка на жирного купца. GenerateMerchant()
// сам ставит энкаунтер на карту и ведёт свою запись в судовом журнале.
void LoanCap_DebtorTip()
{
	// Gregg: GenerateMerchant берёт колонию из локации, а у палубы и каюты её нет -
	// вызов падал на Colonies[FindColony("")] (stack error в error.log). Наводку
	// выдаём ТОЛЬКО на суше, в городе.
	int locnum = FindLocation(pchar.location);
	if (locnum == -1) return;
	if (!CheckAttribute(&locations[locnum], "townsack")) return;

	// Gregg: слот купца один - второй вызов осиротил бы первого
	if (CheckAttribute(&MerchantParam, "active"))
	{
		if (MerchantParam.active == true) return;
	}
	GenerateMerchant();
}

// Gregg: ГГ присвоил долг — ростовщик присылает разовую засаду к воротам своего города
void LoanCap_SetAmbush()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.UsurerCity")) return;
	if (CheckAttribute(pchar, "questTemp.LoanCapAmbush")) return;

	string sCity = pchar.GenQuest.LoanCap.UsurerCity;
	pchar.questTemp.LoanCapAmbush = sCity;

	pchar.quest.LoanCap_Ambush.win_condition.l1 = "location";
	pchar.quest.LoanCap_Ambush.win_condition.l1.location = sCity + "_ExitTown";
	pchar.quest.LoanCap_Ambush.function = "LoanCap_Ambush";
}

// Gregg: прерывание срабатывает в момент входа в локацию, когда loadedLocation
// может быть ещё не готов - LAi_FindNearestFreeLocator2Pchar вернёт пустую строку
// и засада тихо не состоится. Поэтому сам спавн уводим в отложенный вызов.
void LoanCap_Ambush(string qName)
{
	if (!CheckAttribute(pchar, "questTemp.LoanCapAmbush")) return;

	DoQuestFunctionDelay("LoanCap_AmbushSpawn", 1.0);
}

void LoanCap_AmbushSpawn(string qName)
{
	if (!CheckAttribute(pchar, "questTemp.LoanCapAmbush")) return;

	// Gregg: группы локаторов в <город>_ExitTown разнятся от локации к локации -
	// goto есть не везде. Пробуем goto, затем reload.
	string sGroup = "goto";
	string sLoc = LAi_FindNearestFreeLocator2Pchar(sGroup);
	if (sLoc == "")
	{
		sGroup = "reload";
		sLoc = LAi_FindNearestFreeLocator2Pchar(sGroup);
	}
	// Gregg: перевешивать прерывание на ту же локацию нельзя - ГГ уже в ней,
	// получим цикл. Группа reload есть везде, куда можно войти, так что сюда
	// практически не попадём; если попали - засада просто не состоялась.
	if (sLoc == "")
	{
		DeleteAttribute(pchar, "questTemp.LoanCapAmbush");
		return;
	}

	DeleteAttribute(pchar, "questTemp.LoanCapAmbush");

	int iQty = 2 + rand(1);
	int iRank = sti(pchar.rank) + 3;
	if (iRank > 30) iRank = 30;

	// Gregg: счётчик локальный, а не общий i из reaction_functions.c - внутри цикла
	// зовутся чужие функции, а глобал в Storm один на всю программу
	int iGang;
	LAi_group_Delete("LoanCapGang");
	for (iGang = 1; iGang <= iQty; iGang++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("LoanCapGang_" + iGang, "pirate_" + (rand(8) + 1), "man", "man", iRank, PIRATE, -1, false));
		FantomMakeCoolFighter(sld, iRank, 60, 50, BLADE_LONG, "pistol4", 100);
		LAi_SetStayType(sld);
		ChangeCharacterAddressGroup(sld, pchar.location, sGroup, sLoc);
		LAi_group_MoveCharacter(sld, "LoanCapGang");
		sTemp = LAi_FindNearestFreeLocator2Pchar(sGroup);
		if (sTemp != "") sLoc = sTemp;
	}
	LAi_group_SetRelation("LoanCapGang", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("LoanCapGang", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);
}

// Gregg: ставим должника на карту из города, куда он до этого пришёл
void LoanCap_FromCityToSea(string qName)
{
	if (!CheckAttribute(pchar, "quest." + qName + ".CapId")) return;
	int capIndex = GetCharacterIndex(pchar.quest.(qName).CapId);
	if (capIndex == -1) return;

	sld = &characters[capIndex];
	if (LAi_IsDead(sld)) return;

	sld.location = "none";
	sld.location.group = "";
	sld.location.locator = "";

	string sGroup = "LoanCapShip_" + sld.index;
	group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Group_AddCharacter(sGroup, sld.id);
	Group_SetGroupCommander(sGroup, sld.id);
	SetRandGeraldSail(sld, sti(sld.Nation));

	sld.quest = "InMap";
	sld.quest.targetCity = SelectAnyColony(sld.city);

	iTemp = GetMaxDaysFromIsland2Island(GetArealByCityName(sld.city), GetArealByCityName(sld.quest.targetCity)) + 1;
	Map_CreateTrader(sld.city, sld.quest.targetCity, sld.id, iTemp);

	LoanCap_WriteToPortman(sld, iTemp);
	CapUpdateCheckTime(sld, iTemp);
}

// Gregg: должник дошёл до порта — отмечается у портового управления и уходит дальше
void LoanCap_FromMapToCity(string sChar)
{
	int capIndex = GetCharacterIndex(sChar);
	if (capIndex == -1) return;

	ref rCap = &characters[capIndex];
	rCap.quest = "InCity";
	rCap.City = rCap.quest.targetCity;

	int iColony = FindColony(rCap.City);
	rCap.nation = colonies[iColony].nation;

	string sGroup = "LoanCapShip_" + rCap.index;
	group_DeleteGroup(sGroup);
	Group_AddCharacter(sGroup, rCap.id);
	Group_SetGroupCommander(sGroup, rCap.id);
	Group_SetAddress(sGroup, colonies[iColony].island, "quest_ships", "Quest_ship_" + (rand(2) + 1));
	Group_SetTaskNone(sGroup);

	// Gregg: стоянка в порту 2-3 дня, потом снова в море
	int iStay = rand(2) + 2;
	string sName = "SecondTimer_" + rCap.id;
	pchar.quest.(sName).win_condition.l1            = "Timer";
	pchar.quest.(sName).win_condition.l1.date.day   = GetAddingDataDay(0, 0, iStay);
	pchar.quest.(sName).win_condition.l1.date.month = GetAddingDataMonth(0, 0, iStay);
	pchar.quest.(sName).win_condition.l1.date.year  = GetAddingDataYear(0, 0, iStay);
	pchar.quest.(sName).function                    = "LoanCap_FromCityToSea";
	pchar.quest.(sName).CapId                       = rCap.id;

	CapUpdateCheckTime(rCap, iStay + 2);
}

// Gregg: должник погиб. NPC_Death ловит и потопление в море, и ЛЮБОЙ финал абордажа -
// движок обнуляет HP оригинала на форме трофея (в плен / убить / отпустить), поэтому
// исход различаем по Killer.status.
void LoanCap_CapIsDeath(string qName)
{
	string sNow = LoanCap_Stage();
	// Gregg: плен. Движок обнуляет HP оригинала на форме трофея, и это прерывание -
	// единственный момент, когда пленник уже создан, а индекс ещё не перетёрт чужим
	// квестом. Ровно так же ловит своего беглеца ГВИК (utils.c GWIK_RansomBoarded).
	if (sNow == "prisoner")
	{
		LoanCap_AdoptCaptive();
		LoanCap_Released(true);
		return;
	}
	// Gregg: развязка уже состоялась - гибель кэпа после неё квеста не касается.
	// Гарда по одному значению мало: живых стадий две (розыск и объявленный бой).
	bool bLive = false;
	if (sNow == "seek") bLive = true;
	if (sNow == "fighting") bLive = true;
	if (!bLive) return;

	string sRecord = "7";
	if (sNow == "fighting") sRecord = "11";
	if (CheckAttribute(pchar, "GenQuest.LoanCap.CapId"))
	{
		int capIndex = GetCharacterIndex(pchar.GenQuest.LoanCap.CapId);
		if (capIndex != -1)
		{
			if (CheckAttribute(&characters[capIndex], "Killer.status"))
			{
				if (sti(characters[capIndex].Killer.status) == KILL_BY_ABORDAGE) sRecord = "11";
			}
		}
	}

	AddQuestRecord("Gen_LoanCapDebt", sRecord);
	CloseQuestHeader("Gen_LoanCapDebt");
	LoanCap_Clear();
}

// Gregg: ростовщик умер — долг спрашивать не с кого. Собранное ГГ оставляет себе.
void LoanCap_UsurerIsDeath(string qName)
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.Stage")) return;

	// Gregg: задание так и не было принято — квестбук не трогаем, просто убираемся
	if (pchar.GenQuest.LoanCap.Stage != "offered")
	{
		AddQuestRecord("Gen_LoanCapDebt", "8");
		CloseQuestHeader("Gen_LoanCapDebt");
	}
	LoanCap_Clear();
}

// Gregg: срок розыска вышел
void LoanCap_Deadline(string qName)
{
	string sNow = LoanCap_Stage();
	if (sNow == "") return;

	// Gregg: предложение так и не приняли - убираемся молча, квестбук не трогаем
	if (sNow == "offered")
	{
		LoanCap_Clear();
		return;
	}

	// Gregg: развязка уже состоялась - срок розыска ни при чём, дальше считает
	// LoanCap_ReportDue. Живых стадий здесь две, поэтому проверяем обе.
	bool bLive = false;
	if (sNow == "seek") bLive = true;
	if (sNow == "fighting") bLive = true;
	if (!bLive) return;

	AddQuestRecord("Gen_LoanCapDebt", "6");
	CloseQuestHeader("Gen_LoanCapDebt");
	LoanCap_Clear();
}

// Gregg: срок отчёта вышел - ГГ развязался с должником и не вернулся к ростовщику.
// Один таймер на все четыре развязки, цена у каждой своя.
void LoanCap_ReportDue(string qName)
{
	string sNow = LoanCap_Stage();
	if (sNow == "") return;

	if (sNow == "promised")
	{
		// Gregg: обещал закрыть чужой долг и не пришёл. Это не просрочка розыска,
		// а нарушенное слово.
		ChangeCharacterReputation(pchar, -5);
		OfficersReaction("bad");
		AddQuestRecord("Gen_LoanCapDebt", "22");
		CloseQuestHeader("Gen_LoanCapDebt");
		LoanCap_Clear();
		return;
	}

	// Gregg: деньги собраны и не отданы - это присвоение, независимо от того,
	// сказал ли ГГ об этом ростовщику в лицо. Цена та же. Живых стадий с деньгами
	// две, поэтому проверяем обе: смешанной логики в проекте избегаем.
	bool bMoney = false;
	if (sNow == "paid") bMoney = true;
	if (sNow == "forced") bMoney = true;
	if (!bMoney)
	{
		// Gregg: отпустил, взял расписку или везёт пленника - денег ростовщика у ГГ
		// нет, обманывать некого. Просто не отчитался: ростовщик обижен и месяц
		// не даёт работы, а пленник перестаёт быть квестовым.
		LoanCap_OffendUsurer();
		AddQuestRecord("Gen_LoanCapDebt", "18");
		CloseQuestHeader("Gen_LoanCapDebt");
		LoanCap_Clear();
		return;
	}

	ChangeCharacterReputation(pchar, -12);
	OfficersReaction("bad");
	AddQuestRecord("Gen_LoanCapDebt", "21");
	CloseQuestHeader("Gen_LoanCapDebt");
	LoanCap_SetAmbush();
	LoanCap_Clear();
}

// Gregg: ростовщик обижен и месяц не даёт работы. Отдельная функция потому, что
// поводов трое: вскрытое завышение, недоплата по слову должника и молчаливый уход.
void LoanCap_OffendUsurer()
{
	if (!CheckAttribute(pchar, "GenQuest.LoanCap.UsurerId")) return;
	int usurerIndex = GetCharacterIndex(pchar.GenQuest.LoanCap.UsurerId);
	if (usurerIndex == -1) return;
	SaveCurrentNpcQuestDateParam(&characters[usurerIndex], "LoanCapOffended");
}
//<== Gregg: генератор ростовщика "Долг капитана"