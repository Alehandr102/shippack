// Gregg: вынесено из GenQuests.c (сплит god-файла) — квест «Каторжник» (Convict).
// #include на исходное место — file-scope глобалы/дефайны остаются в GenQuests.c выше include; поведение идентично.

void Convict_LocExit(string qName)
{
	ref sld;
	int i;
	if(pchar.GenQuest.Convict == "close")
	{
		for(i = 0; i < sti(pchar.GenQuest.Convict.ConvictQty); i++)
		{
			sld = CharacterFromID("Convict_" + i);
			sld.LifeDay = 0;
		}
		LAi_group_Delete("ConvictGroup");
		DeleteAttribute(pchar, "GenQuest.Convict");
		return;
	}
	if(pchar.GenQuest.Convict == "fight")
	{
		ChangeCharacterReputation(pchar, -2);
		OfficersReaction("bad");
		DeleteAttribute(pchar, "GenQuest.Convict");
		return;
	}
	if(pchar.GenQuest.Convict == "ToShore" || pchar.GenQuest.Convict == "GetShip" || pchar.GenQuest.Convict == "ToMayak")
	{
		for(i = 0; i < sti(pchar.GenQuest.Convict.ConvictQty); i++)
		{
			sld = CharacterFromID("Convict_" + i);
			ChangeCharacterAddress(sld, "none", "");
		}
	}
	if(pchar.GenQuest.Convict == "FightInShore" || pchar.GenQuest.Convict == "ShoreFight" || pchar.GenQuest.Convict == "MayakFight")
	{
		if(pchar.GenQuest.Convict == "FightInShore")
		{
			AddQuestRecord("Convict", "6");
			AddQuestUserData("Convict", "sSex", GetSexPhrase("слово джентльмена","'держать своё слово'"));
			AddQuestUserData("Convict", "sSex1", GetSexPhrase("","а"));
			AddQuestUserData("Convict", "sShoreName", XI_ConvertString("Shore" + pchar.GenQuest.Convict.ShoreLocation + "Acc"));
		}
		if(pchar.GenQuest.Convict == "ShoreFight")
		{
			AddQuestRecord("Convict", "9");
			AddQuestUserData("Convict", "sSex", GetSexPhrase("ся","ась"));
		}
		if(pchar.GenQuest.Convict == "MayakFight")
		{
			AddQuestRecord("Convict", "13");
			AddQuestUserData("Convict", "sSex", GetSexPhrase("слово джентльмена","'держать своё слово'"));
			AddQuestUserData("Convict", "sSex1", GetSexPhrase("","а"));
			AddQuestUserData("Convict", "sMayak", XI_ConvertString(pchar.GenQuest.Convict.Mayak + "Dat"));
		}

		CloseQuestHeader("Convict");
		ChangeCharacterReputation(pchar, 2);
		OfficersReaction("bad");
		DeleteAttribute(pchar, "GenQuest.Convict");
	}
}

void Convict_GetMineType()
{
	pchar.GenQuest.Convict.MineType = drand(2) + 1;
}

void Convict_MeetInShore(string qName)
{
	ref sld;
	int i;
	pchar.quest.Convict_SetTimer.over = "yes";
	pchar.quest.Convict_MapEnter.over = "yes";

	DeleteAttribute(&Locations[FindLocation(pchar.GenQuest.Convict.ShoreLocation)], "DisableEncounters");

	if(pchar.Location == pchar.location.from_sea) // пришёл с моря
	{
		chrDisableReloadToLocation = true;
		bDisableFastReload = true;
		for(i = 0; i < sti(pchar.GenQuest.Convict.ConvictQty); i++)
		{
			sld = CharacterFromID("Convict_" + i);
			PlaceCharacter(sld, "goto", pchar.location);
			if(i == 0)
			{
				LAi_CharacterEnableDialog(sld);
				if(pchar.GenQuest.Convict == "GetShip")
				{
					sld.dialog.currentnode = "Convict_15";
				}
				if(pchar.GenQuest.Convict == "ToShore")
				{
					sld.dialog.currentnode = "Convict_16";
				}
				if(pchar.GenQuest.Convict == "ToMayak")
				{
					sld.dialog.currentnode = "Convict_16";
				}
				LAi_SetActorType(sld);
				LAi_ActorDialog(sld, pchar, "", -1, 0);
			}
		}
	}
	else // пришёл с суши --> закрываем квест
	{
		AddQuestRecord("Convict", "3");
		AddQuestUserData("Convict", "sSex", GetSexPhrase("","а"));
		CloseQuestHeader("Convict");
		for(i = 0; i < sti(pchar.GenQuest.Convict.ConvictQty); i++)
		{
			sld = CharacterFromID("Convict_" + i);
			sld.LifeDay = 0;
		}
		LAi_group_Delete("ConvictGroup");
		DeleteAttribute(pchar, "GenQuest.Convict");
	}
}

void Convict_SetTimer(string qName)
{
	pchar.quest.Convict_ToShore.over = "yes";
	pchar.quest.Convict_MapEnter.over = "yes";
	AddQuestRecord("Convict", "3");
	AddQuestUserData("Convict", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("Convict");
	for(int i = 0; i < sti(pchar.GenQuest.Convict.ConvictQty); i++)
	{
		sld = CharacterFromID("Convict_" + i);
		sld.LifeDay = 0;
	}
	LAi_group_Delete("ConvictGroup");
	DeleteAttribute(pchar, "GenQuest.Convict");
}

void Convict_SetTimerMeet(string qName)
{
	pchar.quest.Convict_ToShore.win_condition.l1 = "Location";
	pchar.quest.Convict_ToShore.win_condition.l1.location = pchar.GenQuest.Convict.ShoreLocation;
	pchar.quest.Convict_ToShore.function = "Convict_MeetInShore";

	SetFunctionTimerConditionParam("Convict_SetTimer", 0, 0, 0, 14, false);
}

void Convict_MapEnter(string qName)
{
	pchar.quest.Convict_SetTimer.over = "yes";
	pchar.quest.Convict_ToShore.over = "yes";
	AddQuestRecord("Convict", "3");
	AddQuestUserData("Convict", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("Convict");
	for(int i = 0; i < sti(pchar.GenQuest.Convict.ConvictQty); i++)
	{
		sld = CharacterFromID("Convict_" + i);
		sld.LifeDay = 0;
	}
	LAi_group_Delete("ConvictGroup");
	DeleteAttribute(pchar, "GenQuest.Convict");
}

void Convict_DialogDisable()
{
	ref sld;
	for(int i = 0; i < sti(pchar.GenQuest.Convict.ConvictQty); i++)
	{
		sld = CharacterFromID("Convict_" + i);
		LAi_CharacterDisableDialog(sld);
		LAi_SetStayType(sld);
	}
}

// Цена лодки, пригнанной каторжанам: по ней они и рассчитываются (ShipPrice/50 серебряных слитков).
// LEO: базой был price БАЗОВОГО ТИПА корабля, но после перевода баланса на цену по ТТХ его в инитах
//   нет вовсе (Ships.c) - чтение давало 0, и без верфевладельца рядом каторжане платили НИЧЕГО.
//   Базу берём с самого корабля (RealShips[].Price), верфь только уточняет.
int Convict_GetShipPrice(int iShipType)
{
	ref chr;
	ref rShip;
	string town = "";
	int shipyardPrice;
	int price = 0;
	int iReal = sti(pchar.GenQuest.Convict.RealShipIndex);
	rShip = GetRealShip(iReal);
	if (CheckAttribute(rShip, "Price")) price = sti(rShip.Price);
	town = FindTownOnIsland(loadedLocation.islandId);
	if (town != "")
	{
		// Верфевладельца с таким id может не быть (разговор идёт в бухте у маяка) - без него у
		// GetShipBuyPrice нет ShipCostRate, и считать по нему нечего. Тогда остаёмся на цене корабля.
		if (GetCharacterIndex(town + "_shipyarder") != -1)
		{
			chr = CharacterFromID(town + "_shipyarder");
			shipyardPrice = GetShipBuyPrice(iReal, chr);
			if (shipyardPrice > 0) price = shipyardPrice;
		}
	}
	if (price < 1) price = 100; // страховка от нулевого расчёта: пустой награды быть не должно
	return price;
}

void Convict_OnMayak(string qName)
{
	ref sld;
	if(pchar.location == pchar.location.from_sea) // пришёл с моря
	{
		pchar.quest.Convict_OnMayak.over = "yes"; // затираем прерывание
		DeleteAttribute(&Locations[FindLocation(pchar.GenQuest.Convict.Mayak)], "DisableEncounters");

		for(int i = 0; i < sti(pchar.GenQuest.Convict.ConvictQty); i++)
		{
			sld = CharacterFromID("Convict_" + i);
			ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto" + (i + 1));
			LAi_SetActorTypeNoGroup(sld);
			if(i == 0)
			{
				RemovePassenger(pchar, sld);
				LAi_CharacterEnableDialog(sld);
				sld.dialog.filename = "GenQuests_Dialog.c";
				sld.dialog.currentnode = "Convict_17";
				LAi_ActorDialog(sld, pchar, "", -1, 0);
			}
			else
			{
				LAi_ActorFollow(sld, pchar, "", -1);
			}
		}
	}
}

bool Convict_CheckShipType(int iShipType)
{
	int cn;
	ref sld;
	for (int i=1; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if(cn != -1)
		{
			sld = &characters[cn];
			if (RealShips[sti(sld.Ship.Type)].basetype == iShipType)
			{
				pchar.GenQuest.Convict.CharShipId = sld.id;
				pchar.GenQuest.Convict.RealShipIndex = sld.ship.type;
				pchar.GenQuest.Convict.ShipPrice = Convict_GetShipPrice(iShipType);
				return true;
			}
		}
	}
	return false;
}

void Convict_SetConvictToShip()
{
	ref sld, chr;
	aref arTo, arFrom;
	if(CheckAttribute(pchar,"GenQuest.Convict.CharShipId"))
	{
		sld = CharacterFromID("Convict_0");
		chr = CharacterFromID(pchar.GenQuest.Convict.CharShipId);

		DeleteAttribute(sld,"ship");
		sld.ship = "";

		makearef(arTo, sld.ship);
		makearef(arFrom, chr.Ship);
		CopyAttributes(arTo, arFrom);

		if (CheckAttribute(chr, "curshipnum"))
		{
			int curshipnum = sti(chr.curshipnum);
			sld.curshipnum = curshipnum;
			DeleteAttribute(chr, "curshipnum");
		}

		SeaAI_SetOfficer2ShipAfterAbordage(sld, chr);
		DeleteAttribute(chr,"ship"); // трем корабль
		chr.ship.type = SHIP_NOTUSED;

		RemoveCharacterCompanion(pchar, chr);
		AddPassenger(pchar, chr, false);

		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_FRIEND);

		DeleteAttribute(pchar,"GenQuest.Convict.CharShipId");

		pchar.GenQuest.Convict = "close";
		SetFunctionExitFromLocationCondition("Convict_LocExit", pchar.location, false);
	}
}