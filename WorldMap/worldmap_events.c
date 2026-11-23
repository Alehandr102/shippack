
//=========================================================================================
//Events
//=========================================================================================

void wdmEvent_EncounterCreate()
{
	float dltTime = GetEventData();
	float playerShipX = GetEventData();
	float playerShipZ = GetEventData();
	float playerShipAY = GetEventData();
	//Skip encounters
	if(CheckAttribute(worldMap, "noenc") != 0)
	{
		if(worldMap.noenc == "true") return;
	}
	//Save player ship position
	worldMap.playerShipX = playerShipX;
	worldMap.playerShipZ = playerShipZ;
	worldMap.playerShipAY = playerShipAY;

	wdmStormGen(dltTime, playerShipX, playerShipZ, playerShipAY);
	wdmShipEncounter(dltTime, playerShipX, playerShipZ, playerShipAY);
}

void wdmEvent_PlayerInStorm()
{
	float playerShipX = GetEventData();
	float playerShipZ = GetEventData();
	float playerShipAY = GetEventData();
	int stormIndex = GetEventData();
	if(!IsEquipCharacterByArtefact(pchar, "talisman2")) wdmReloadToSea();
}

void wdmEvent_ShipEncounter()
{
	float playerShipX = GetEventData();
	float playerShipZ = GetEventData();
	float playerShipAY = GetEventData();
	int eshipIndex = GetEventData();
	if (CheckAttribute(pchar, "SkipEshipIndex") && pchar.SkipEshipIndex == eshipIndex) return; // boal
	pchar.eshipIndex = eshipIndex;
	LaunchMapScreen();

}

void wdmEvent_UpdateDate()
{
	Environment.date.day = worldMap.date.day;
	Environment.date.month = worldMap.date.month;
	Environment.date.year = worldMap.date.year;
	Environment.date.hour = worldMap.date.hour;
	Environment.date.min = worldMap.date.min;
	Environment.date.sec = worldMap.date.sec;
	Environment.time = GetEventData();
}

//Добавляем по запросу квестовых энкоунтеров
// LEO: неудачный рейс компаньона повторяется отдельно, не пересоздавая успешные корабли.
void wdmRetryQuestEncounter(aref encDescription)
{
	aref retry = wdmCreateNewQuestEncDescription();
	CopyAttributes(retry, encDescription);
	worldMap.addQuestEncounters = "updateQuest";
}

#event_handler("WorldMap_AddQuestEncounters", "wdmEvent_AddQuestEncounters");
void wdmEvent_AddQuestEncounters()
{
	int idx = -1;
	string encID = "";
	string encPath = "";
	aref encs, at;
	makearef(encs, worldMap.addQuestEncounters);
	// LEO: снимок забирает текущую очередь; новые заявки во время создания не теряются.
	object pendingRequests;
	CopyAttributes(&pendingRequests, encs);
	DeleteAttribute(&worldMap, "addQuestEncounters");
	worldMap.addQuestEncounters = "";
	makearef(encs, pendingRequests);
	int num = GetAttributesNum(encs);
	for(int i = 0; i < num; i++)
	{
		at = GetAttributeN(encs, i);
		if (!CheckAttribute(at, "type")) continue;

		if (CheckAttribute(at, "characterID")) // boal fix 14.09.06
		{
			if(at.type == "trader")
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					PostEvent("Map_TraderSucces", 100, "s", at.characterID);
					continue;
				}
				//Создаём в карте энкоунтера
				encID = "";
				if (CheckAttribute(at, "XZGoto")) // покоординатный boal 04/10/06
				{
					if(!wdmCreateMerchantShipXZByIndex(1.0, idx, &encID, stf(at.x1), stf(at.z1), stf(at.x2), stf(at.z2), sti(at.TimeOut)))
					{
						PostEvent("Map_TraderSucces", 100, "s", at.characterID);
						continue;
					}
				}
				else
				{
					float traderspeed = 1.0;
					if (at.characterID == "BSBons" && CheckAttribute(Pchar, "BonsFlintRunning")) traderspeed = 1.35; // квест ЧП
					if(!wdmCreateMerchantShipByIndex(traderspeed, idx, &encID, at.beginlocator, at.endLocator, sti(at.TimeOut)))
					{
						PostEvent("Map_TraderSucces", 100, "s", at.characterID);
						continue;
					}
				}
				//Путь до энкоунтера
				encPath = "encounters." + encID;
				//Сохраняем принадлежность к квестовым энкоунтеров
				worldMap.(encPath).quest.event = "Map_TraderSucces";
				worldMap.(encPath).quest.chrID = at.characterID;
			}

			if(at.type == "warrior")
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				//Создаём в карте энкоунтера
				encID = "";
				if(!wdmCreateRealFollowShipByIndex(1.0, idx, &encID, sti(at.TimeOut)))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				//Путь до энкоунтера
				encPath = "encounters." + encID;
				//Сохраняем принадлежность к квестовым энкоунтеров
				worldMap.(encPath).quest.event = "Map_WarriorEnd";
				worldMap.(encPath).quest.chrID = at.characterID;
			}

			if(at.type == "coolwarrior")//быстрый энкаутер Lipsar взял из ккс
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				//Создаём в карте энкоунтера
				encID = "";
				if(!wdmCreateRealFollowShipByIndex(1.7, idx, &encID, sti(at.TimeOut)))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				//Путь до энкоунтера
				encPath = "encounters." + encID;
				//Сохраняем принадлежность к квестовым энкоунтеров
				worldMap.(encPath).quest.event = "Map_WarriorEnd";
				worldMap.(encPath).quest.chrID = at.characterID;
			}

			//быстрый энкаутер по фрахту Lipsar
			//на самом деле, тот же энкаунтер у охотников за кладами, по квесту Изабеллы х2, конвой пассажира, доставка сундуков ростовщика, конвой торговца, пираты на необитайке, карта сокровищ
			if(at.type == "tradehunter")
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				encID = "";
				int iPrise = 0;
				if (checkattribute(pchar, "cargoquest") && sti(pchar.CargoQuest.iMoney) > 50000)
				{iPrise = Makeint(sti(pchar.CargoQuest.iMoney) / 50000)* 0.1;}

				if(!wdmCreateRealFollowShipByIndex(1.2 + iPrise, idx, &encID, sti(at.TimeOut)))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				encPath = "encounters." + encID;
				worldMap.(encPath).quest.event = "Map_WarriorEnd";
				worldMap.(encPath).quest.chrID = at.characterID;
			}

			if(at.type == "shipwrecked")//медленный энкаутер торговца Lipsar
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				encID = "";
				if(!wdmCreateMerchantShipByIndex(0.2, idx, &encID,"","", sti(at.TimeOut)))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				encPath = "encounters." + encID;
				worldMap.(encPath).quest.event = "Map_WarriorEnd";
				worldMap.(encPath).quest.chrID = at.characterID;
			}

			if(at.type == "hunter")//энка ОЗГ Lipsar
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				//Создаём в карте энкоунтера
				encID = "";
				ref refChar = CharacterFromID(at.characterID);
				string sRep = NationShortName(sti(refChar.nation))+"hunter";
				if(!wdmCreateRealFollowShipByIndex(1.1 + sti(pchar.reputation.(sRep))/100, idx, &encID, sti(at.TimeOut)))
				{
					PostEvent("Map_WarriorEnd", 100, "s", at.characterID);
					continue;
				}
				//Путь до энкоунтера
				encPath = "encounters." + encID;
				//Сохраняем принадлежность к квестовым энкоунтеров

				worldMap.(encPath).quest.event = "Map_WarriorEnd";
				worldMap.(encPath).quest.chrID = at.characterID;
			}

			// LEO: дружественный энкаунтер возврата компаньона (модель ranger). Наводится на игрока,
			//   на сближении исчезает и движок шлёт WorldMap_CompanionMet -> вступление в эскадру.
			//   Боевого quest.event НЕТ: это своя, а не вражина.
			if(at.type == "companionmeet")
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					wdmRetryQuestEncounter(at);
					continue;
				}
				encID = "";
				// LEO: скорость возврата - по атрибуту компаньона CompanionTravel.WdmSpeed (сбор с торгов = 1.5x), иначе база 1.1
				float fMeetSpd = 1.1;
				ref rMeetChr = CharacterFromID(at.characterID);
				if (CheckAttribute(rMeetChr, "CompanionTravel.WdmSpeed")) fMeetSpd = stf(rMeetChr.CompanionTravel.WdmSpeed);
				if(!wdmCreateCompanionShipByIndex(fMeetSpd, idx, &encID, sti(at.TimeOut), at.characterID, at.beginlocator))
				{
					wdmRetryQuestEncounter(at);
					continue;
				}
				// LEO: пометка .quest - чтобы энкаунтер ПЕРЕЖИЛ заход игрока на него (WdmAddEncountersData
				//   не ставит needDelete квестовым). Иначе после выхода из энкаунтера компаньон пропадал.
				//   Пересоздаётся при выходе, как ОЗГ. Боевого quest.event НЕ ставим - это своя, не вражина.
				encPath = "encounters." + encID;
				worldMap.(encPath).quest.chrID = at.characterID;
			}

			// LEO: рейс ТУДА - компаньон мирно плывёт к городу-складу (цель = XZ, sailToX/Z на энкаунтере).
			//   По достижении движок шлёт WorldMap_CompanionArrived -> работа со складом.
			if(at.type == "companionsail")
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					wdmRetryQuestEncounter(at);
					continue;
				}
				encID = "";
				// LEO: скорость энкаунтера - по атрибуту компаньона CompanionTravel.WdmSpeed (сбор с торгов = 1.5x), иначе база 1.0
				float fSailSpd = 1.0;
				ref rSailChr = CharacterFromID(at.characterID);
				if (CheckAttribute(rSailChr, "CompanionTravel.WdmSpeed")) fSailSpd = stf(rSailChr.CompanionTravel.WdmSpeed);
				if(!wdmCreateCompanionShipByIndex(fSailSpd, idx, &encID, sti(at.TimeOut), at.characterID, at.beginlocator))
				{
					wdmRetryQuestEncounter(at);
					continue;
				}
				// цель-город: имя на энкаунтер (движок берёт позицию порта через GetQuestLocator)
				encPath = "encounters." + encID;
				worldMap.(encPath).sailToCity = at.sailToCity;
				// LEO: .quest - чтобы энкаунтер пережил заход игрока на него (см. companionmeet выше)
				worldMap.(encPath).quest.chrID = at.characterID;
			}

			if(at.type == "goldFleet")//энка ЗФ Lipsar
			{
				if(!GenerateMapEncounter_Alone(at.characterID, &idx))
				{
					PostEvent("Map_TraderSucces", 100, "s", at.characterID);
					continue;
				}
				//Создаём в карте энкоунтера
				encID = "";
				if(!wdmCreateMerchantShipXZByIndex(0.42, idx, &encID, stf(at.x1), stf(at.z1), stf(at.x2), stf(at.z2), sti(at.TimeOut)))
				{
					PostEvent("Map_TraderSucces", 100, "s", at.characterID);
					continue;
				}
				//Путь до энкоунтера
				encPath = "encounters." + encID;
				//Сохраняем принадлежность к квестовым энкоунтеров
				worldMap.(encPath).quest.event = "Map_TraderSucces";
				worldMap.(encPath).quest.chrID = at.characterID;
			}

		}
	}
	//Очищаем массив энкоунтеров
	ReleaseMapEncounters();
}

#event_handler("WorldMap_IsSkipEnable", "wdmIsSkipEnable");
bool wdmSkipReturnBool = false;
bool wdmIsSkipEnable()
{
	wdmSkipReturnBool = false;
	if(IsCharacterPerkOn(pchar, "SailingProfessional")) //to_do del
	{
		if(rand(100) <= 50)
		{
			wdmSkipReturnBool = true;
		}
	}
	return wdmSkipReturnBool;
}

void wdmDeleteLoginEncounter(string encID)
{
	Event("WorldMap_EncounterDelete", "s", encID);
}

#event_handler("WorldMap_EncounterDelete", "wdmEncounterDelete");
ref wdmEncounterDelete()
{
	BI_intRetValue = true;

	string encID = GetEventData();
	string encPath = "encounters." + encID;
	if(CheckAttribute(&worldMap, encPath) == 0)
	{
		return &BI_intRetValue;
	}
	aref enc;
	makearef(enc, worldMap.(encPath));
	//Сохраняем событие
	bool needEvent = false;
	string eventName = "";
	string chrID = "";
	if(CheckAttribute(enc, "quest.chrID"))
	{
    //homo 14/04/07 не ясно если энкоутер ещё не доплыл до пункта назанчения значит его нельзя тереть?
    //Даже если его трет программист?
    	if(CheckAttribute(&enc, "Gotox") && CheckAttribute(&enc, "Gotoz"))
		{

			float fDeltaX = (stf(enc.x) - stf(enc.Gotox));
			float fDeltaZ = (stf(enc.z) - stf(enc.Gotoz));
			float fRadSqr = fDeltaX*fDeltaX + fDeltaZ*fDeltaZ;
			if( fRadSqr > 100.0 && stf(enc.livetime) > 1.0)
			{
				BI_intRetValue = false;

			}
		}

		if(CheckAttribute(enc, "quest.event") != 0)
		{
			eventName = enc.quest.event;
			chrID = enc.quest.chrID;
			needEvent = true;
		}

	}
	//Отмечаем, что удалён
	enc.needDelete = "wdmEncounterDelete";
	//Удаляем квестовую пометку
	DeleteAttribute(&enc, "quest");
	if (!IsEntity(worldMap))
	{
	   //Трем сам энкаутер сразу homo 10/04/07
        DeleteAttribute(&worldMap, encPath);
    }
	//Отправляем квестовый эвент, если надо
	if(needEvent)
	{
		PostEvent(eventName, 100, "s", chrID);
	}
	return &BI_intRetValue;
}

#event_handler("WorldMap_GetMoral", "wdmGetMoral");
float wdmGetMoral()
{
	// LEO: мораль КОМАНДЫ КОРАБЛЯ ГГ (тот же источник, что в меню корабля: ship.crew.morale),
	//      а не средняя по всей эскадре — иначе бар на глобалке не сходится с реальной моралью команды.
	return stf(GetCharacterCrewMorale(pchar));
}

#event_handler("WorldMap_GetFood", "wdmGetFood");
float wdmGetFood()
{
	return makefloat(CalculateFood());
}
//#20200226-03
#event_handler("WorldMap_GetRum", "wdmGetRum");
float wdmGetRum()
{
	return makefloat(CalculateRum());
}
// LEO: лекарства для HUD глобалки — суммарное кол-во GOOD_MEDICAMENT в эскадре. Движок рисует число, только если в worldmap.c задан worldMap.medText.
#event_handler("WorldMap_GetMedicine", "wdmGetMedicine");
float wdmGetMedicine()
{
	return makefloat(GetSquadronGoods(pchar, GOOD_MEDICAMENT));
}
//  квестовый отлов входа в море по начилию НПС в случайке
void wdmEnterSeaQuest(string _chrId)
{
	ref NPChar = characterFromID(_chrId);

	if (findsubstr(_chrId, "Hunter0" , 0) != -1) // ОЗГи
	{
		AddQuestRecord("HeadHunter", "HeadHunter_Sea");
		AddQuestUserData("HeadHunter", "sSex", GetSexPhrase("ся","ась"));
		AddQuestUserData("HeadHunter", "sNation", NationNameGenitive(sti(NPChar.Nation)));
		CloseQuestHeader("HeadHunter");
	}
	// посыльный-агент вражеской нации (комендантский конвой): на карте гнался как враг (блок "пропустить"),
	// в море - НЕЙТРАЛИТЕТ для палубных переговоров. Высылаешь шлюпку -> FortAgent_dialog.c
	if (findsubstr(_chrId, "FortAgent" , 0) != -1)
	{
		DeleteAttribute(NPChar, "AlwaysEnemy");
		NPChar.AlwaysFriend     = true;  // периодика ИИ держит мир, не стреляет и не агрит
		NPChar.ShipEnemyDisable = true;
		SetCharacterRelationBoth(GetCharacterIndex(_chrId), GetMainCharacterIndex(), RELATION_NEUTRAL);
		if (CheckAttribute(NPChar, "SeaAI.Group.Name")) // снять залоченную карто-задачу атаки, иначе продолжит идти на таран
		{
			Group_UnlockTask(NPChar.SeaAI.Group.Name);
			Group_SetTaskNone(NPChar.SeaAI.Group.Name);
		}
		if (CheckAttribute(pchar, "CargoQuest.GiveTraderID") && !CheckAttribute(pchar, "questTemp.FortAgentMet"))
		{
			pchar.questTemp.FortAgentMet = true; // запись о встрече - один раз
			AddQuestRecord("DELIVERY_TRADE_QUEST", "fortagent1");
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sHunterNation", NationNameGenitive(sti(NPChar.Nation)));
		}
	}
	// союзный посыльный-агент эскорта: на карте гнался как враг (блок "пропустить"),
	// в море - НЕЙТРАЛИТЕТ для палубных переговоров. Высылаешь шлюпку -> GovEscortAgent_dialog.c
	if (findsubstr(_chrId, "GovEscortAgent" , 0) != -1)
	{
		DeleteAttribute(NPChar, "AlwaysEnemy");
		NPChar.AlwaysFriend     = true;
		NPChar.ShipEnemyDisable = true;
		SetCharacterRelationBoth(GetCharacterIndex(_chrId), GetMainCharacterIndex(), RELATION_NEUTRAL);
		if (CheckAttribute(NPChar, "SeaAI.Group.Name"))
		{
			Group_UnlockTask(NPChar.SeaAI.Group.Name);
			Group_SetTaskNone(NPChar.SeaAI.Group.Name);
		}
	}
	// каперы-перехватчики комендантского конвоя: запись о нападении (по образцу ОЗГ)
	if (findsubstr(_chrId, "FortAmmoHunter" , 0) != -1)
	{
		if (!CheckAttribute(pchar, "questTemp.FortHuntAttacked") && CheckAttribute(pchar, "CargoQuest.GiveTraderID"))
		{
			pchar.questTemp.FortHuntAttacked = true; // запись одна на заказ; он же гард для записи о конце боя
			int fhTraderIdx = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
			AddQuestRecord("DELIVERY_TRADE_QUEST", "fort5");
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sHunterNation", NationNameGenitive(sti(NPChar.Nation)));
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sTraderNation", NationNameAblative(sti(characters[fhTraderIdx].nation)));
		}
	}
		// homo 19/07/06 GlodFleet
	if (_chrId == "Head_of_Gold_Squadron")
	{
        string sQuest = "GoldFleetEncounter";
        pchar.quest.(sQuest).win_condition.l1 = "EnterToSea";  // fix homo 20/01/07
        pchar.quest.(sQuest).win_condition = "GoldFleetEncounter";
        pchar.quest.(sQuest).function = "GoldFleetEncounter";
	}

	if (_chrId == "Head_of_Silver_Squadron" && CheckAttribute(PChar, "questTemp.silverfleet") && pchar.questTemp.silverfleet != "over") // СФ
	{
		sQuest = "SilverFleetEncounter";
		pchar.quest.(sQuest).win_condition.l1 = "EnterToSea";
		pchar.quest.(sQuest).win_condition = "SilverFleetQuestDeathCheck";
		pchar.quest.(sQuest).function = "SilverFleetQuestDeathCheck";
    	}
	//homo 27/09/06
	if (findsubstr(_chrId, "SiegeCap_1" , 0) != -1)// Осаждающая эскадра
	{
        SiegeSquadronOnMap(_chrId);
	}
	//пиратка, квест №7. фрегат Леон
	if (_chrId == "LeonCapitain")
	{
		AddQuestRecord("Pir_Line_7_Soukins", "15");
	}
	//пиратка, квест №7. бэттлшип найден
	if (_chrId == "QuestCap_PL7")
	{
		AddQuestRecord("Pir_Line_7_Soukins", "11");
		pchar.questTemp.piratesLine = "Soukins_battlshipFound";

		pchar.quest.PQ7_BattleShip_notFound.over = "yes";
        pchar.quest.PQ7_winBattleShip.win_condition.l1 = "Group_Death";
		pchar.quest.PQ7_winBattleShip.win_condition.l1.group = "Quest_Ship";
		pchar.quest.PQ7_winBattleShip.function = "PQ7_winBattleShip";
	}
	//жемчужный генератор Шарпа
	if (_chrId == "Sharp")
	{
		characters[GetCharacterIndex(_chrId)].nation = pchar.nation;
	}
	if (_chrId == "Flint")
	{
		if (pchar.nation == Pirate)
		{
			DeleteAttribute (characters[GetCharacterIndex(_chrId)], "AlwaysEnemy");
			characters[GetCharacterIndex(_chrId)].AlwaysFriend = true;
		}
		else
		{
			DeleteAttribute (characters[GetCharacterIndex(_chrId)], "AlwaysFriend");
			characters[GetCharacterIndex(_chrId)].AlwaysEnemy = true;
		}
	}
	if (_chrId == "BSBons" && CheckAttribute(pchar, "BonsFlintRunning"))
	{
		sQuest = "BS_BonsFlintRunning";
        pchar.quest.(sQuest).win_condition.l1 = "EnterToSea";  // fix homo 20/01/07
        pchar.quest.(sQuest).win_condition = "BS_BonsFlintRunning_AddRecord";
        pchar.quest.(sQuest).function = "BS_BonsFlintRunning_AddRecord";
	}
	if(findsubstr(NPChar.id, "PsHero_" , 0) != -1 && NPChar.nation == PIRATE && PGG_ChangeRelation2MainCharacter(NPChar, 0) > 40 && !CheckAttribute(NPChar, "PGG_warrior"))
	{
		NPChar.nation_backup = NPChar.nation;
		NPChar.nation = pchar.nation;
		characters[GetCharacterIndex(_chrId)].nation = pchar.nation;
	}
}
