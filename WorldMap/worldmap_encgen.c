

//--------------------------------------------------------------------------------------
//Генерация энкоунтеров
//--------------------------------------------------------------------------------------

//Частота штормов в секунду
#define WDM_STORM_RATE   0.0001
#define WDM_ROLL_SCALE   1001
//Частота торговцев в секунду
#define WDM_MERCHANTS_RATE		0.09
//Частота воюищих кораблей в секунду
#define WDM_WARRING_RATE		0.015
//Частота нападающих кораблей в секунду
#define WDM_FOLLOW_RATE  0.025
//Частота специальных событий  (бочка или шлюпка) в секунду
#define WDM_SPECIAL_RATE  		0.002

//MAX, это поменял Я!!!!!!! Шуршунчик.
// Boal - учите мат. часть, г-н Шуршунчик. не работает это до начала новой игры, всегда по уполчанию идёт. Дефайн правильно, тем более  iEncountersRate далее работает
//float WDM_FOLLOW_RATE = 0.025 * iEncountersRate;
//float WDM_STORM_RATE = 0.0001 * iEncountersRate;


//--------------------------------------------------------------------------------------


float wdmTimeOfLastStorm = 0.0;
float wdmTimeOfLastMerchant = 0.0;
float wdmTimeOfLastWarring = 0.0;
float wdmTimeOfLastFollow = 0.0;
float wdmTimeOfLastSpecial = 0.0;


void wdmReset()
{
	wdmTimeOfLastStorm = 0.0;
	wdmTimeOfLastMerchant = 0.0;
	wdmTimeOfLastWarring = 0.0;
	wdmTimeOfLastFollow = 0.0;
	wdmTimeOfLastSpecial = 0.0;
}

//Storm
void wdmStormGen(float dltTime, float playerShipX, float playerShipZ, float playerShipAY)
{
	bool encoff = false;
	if(CheckAttribute(pchar,"worldmapencountersoff") == 1)
	{
		if(sti(pchar.worldmapencountersoff)) return;
	}
	int numStorms = wdmGetNumberStorms();
	if(numStorms < 1)
	{
		wdmTimeOfLastStorm = wdmTimeOfLastStorm + dltTime*WDM_STORM_RATE*1000.0*iEncountersRate;
		if(rand(WDM_ROLL_SCALE) < wdmTimeOfLastStorm)
		{
			wdmCreateStorm();
			wdmTimeOfLastStorm = 0.0;
		}
	}else{
		wdmTimeOfLastStorm = 0.0;
	}
}

//Random ships
void wdmShipEncounter(float dltTime, float playerShipX, float playerShipZ, float playerShipAY)
{
	int numShips = wdmGetNumberShipEncounters();
	//if( CheckAttribute(pchar,"worldmap.shipcounter") ) {
		//numShips = numShips - sti(pchar.worldmap.shipcounter);
	//}
	// LEO: ПГГ НЕ ЗАНИМАЮТ слоты обычных энкаунтеров. Движковый worldMap.encounter.num считает ВСЕХ, поэтому
	//   до 10 охотников (PGG_WORLD_MAP_MAX_AT_SEA) выжирали лимит целиком и на карте не оставалось торговцев/
	//   военных (жалоба игроков: "кроме ПГГ никого нет"). Вычитаем их ТОЧНО (было: грубый бонус +1..+3 к лимиту).
	numShips = numShips - WdmGetPGGEncountersNum();
	if (numShips < 0) numShips = 0; // до штатной проверки ниже - чтобы вычитание не сыпало её warning-трейс
	if( numShips < 0 )
	{
		trace("Warning! World map ship quantity < 0 : numShips = " + numShips);
		trace("pchar.worldmap.shipcounter = " + pchar.worldmap.shipcounter);
		numShips = 0;
	}
	int shipgen = 8;
	switch (iEncountersCountRate)
	{
		case 1: shipgen=6; break;
		case 2: shipgen=12; break;
		case 3: shipgen=30; break;
	}
	if (!CheckAttribute(pchar, "PGGsAtSea"))
	{
		pchar.PGGsAtSea = 0;
	}
	// LEO: бонус к лимиту за ПГГ больше НЕ нужен - они вычтены из numShips выше (точно, а не потолком +3).
	if(numShips < shipgen)
	{
		//Вероятности появления
		wdmTimeOfLastMerchant = wdmTimeOfLastMerchant + dltTime*WDM_MERCHANTS_RATE*1000.0*iEncountersRate;
		wdmTimeOfLastWarring = wdmTimeOfLastWarring + dltTime*WDM_WARRING_RATE*1000.0*iEncountersRate;
		wdmTimeOfLastFollow = wdmTimeOfLastFollow + dltTime*WDM_FOLLOW_RATE*1000.0*iEncountersRate;
		wdmTimeOfLastSpecial = wdmTimeOfLastSpecial + dltTime*WDM_SPECIAL_RATE*1000.0*iEncountersRate;
		//Вероятность от количества созданных
		float nump = 1.0 - numShips*0.15;
		//Выбираем
		if(rand(WDM_ROLL_SCALE) + 1 < wdmTimeOfLastMerchant)
		{
			wdmTimeOfLastMerchant = 0.0;
			wdmCreateMerchantShip(0.8 + rand(10)*0.03);
		}
		else
		{
			bool encoff = false;
			if(CheckAttribute(pchar,"worldmapencountersoff") == 1)
			{
				encoff = sti(pchar.worldmapencountersoff);
			}
			if(encoff == false)
			{
				if(rand(WDM_ROLL_SCALE) + 1 < wdmTimeOfLastWarring)
				{
					wdmTimeOfLastWarring = 0.0;
					wdmCreateWarringShips();
				}
				else
				{
					if(rand(WDM_ROLL_SCALE) + 1 < wdmTimeOfLastFollow)
					{
						wdmTimeOfLastFollow = 0.0;
						if(!IsStopMapFollowEncounters())
						{
							wdmCreateFollowShip(0.8 + rand(10)*0.05);
						}
					}
				}
				if(rand(WDM_ROLL_SCALE) + 1 < wdmTimeOfLastSpecial)
				{
					wdmTimeOfLastSpecial = 0.0;
					wdmCreateSpecial(0.05 + rand(10)*0.02);
				}
			}
		}
	}
	else
	{
		wdmTimeOfLastMerchant = 0.0;
		wdmTimeOfLastWarring = 0.0;
		wdmTimeOfLastFollow = 0.0;
		wdmTimeOfLastSpecial = 0.0;
	}
}

// LEO: сколько кораблей на карте - это энкаунтеры ПГГ (рейсы/охотники за ГГ + борта их стычек с пиратами).
//   Вычитается из занятых слотов в wdmShipEncounter, чтобы ПГГ не вытесняли обычные энкаунтеры.
//   Обход READ-ONLY (идиома PGG_HasSkirmishEncounter, PGG_world.c:347) - в сейв ничего не пишем, структура
//   сохраняемых атрибутов не меняется => старые сейвы совместимы.
int WdmGetPGGEncountersNum()
{
	int num, i, idx;
	int cnt = 0;
	aref encs, enc;
	string sChrId;
	if (!IsEntity(worldMap)) return 0;
	makearef(encs, worldMap.encounters);
	num = GetAttributesNum(encs);
	for (i = 0; i < num; i++)
	{
		enc = GetAttributeN(encs, i);
		if (CheckAttribute(enc, "PGG_WarEnc")) // борт стычки ПГГ <-> пират (их сразу два)
		{
			cnt++;
			continue;
		}
		if (!CheckAttribute(enc, "quest.chrID")) continue;
		sChrId = enc.quest.chrID;
		idx = GetCharacterIndex(sChrId); // НЕ CharacterFromID - тот сыплет "не найден НПС" в лог для снесённого
		if (idx < 0) continue;
		if (CheckAttribute(&characters[idx], "PGGAi")) cnt++; // маркер персонажа-ПГГ (PGG_tasks.c:444)
	}
	return cnt;
}


#event_handler("Map_TraderSucces", "Map_TraderSucces");
#event_handler("Map_WarriorEnd", "Map_WarriorEnd");
// to_do -->
void Map_WarriorEnd()
{
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		return;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
	if(sti(pchar.worldmap.shipcounter) < 0)
	{
		sti(pchar.worldmap.shipcounter) = 0;
	}
	string sChar = GetEventData();
	PGG_CompleteWorldMapSail(sChar);
}
void Map_TraderSucces()
{
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		return;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
	if(sti(pchar.worldmap.shipcounter) < 0)
	{
		pchar.worldmap.shipcounter = 0;
	}
	string sChar = GetEventData();
	if (PGG_CompleteWorldMapSail(sChar)) return;

	Map_TraderSucces_quest(sChar); //обработка квестов по поиску кэпов

	//homo 03/08/06 Наводка на купца
	if (findsubstr(sChar, "_QuestMerchant" , 0) != -1)
	{
        Map_ReleaseQuestEncounter(sChar);
        Group_DeleteGroup("Sea_"+sChar);
        CloseQuestHeader("MerchantOnMap");
	}
    // homo 07/10/06 GoldFleet
    if (sChar == "Head_of_Gold_Squadron")
	{
        RouteGoldFleet();
	}

	if (findsubstr(sChar, "SiegeCap_" , 0) != -1)
	{
        SiegeProgress();
	}
	/*if(GetCharacterIndex(sChar) != -1)  // типа пример
	{
		if(CheckAttribute(&characters[GetCharacterIndex(sChar)], "hovernor"))
		{
			if(sti(characters[GetCharacterIndex(sChar)].hovernor) == 1)
			{
				//  to_do MakeNewCapital(sChar);
				return;
			}
		}
	}
	else
	{
		trace("Can't find character with character id " + sChar);
		return;
	}

	if(sChar == "Head of Gold Squadron")
	{
		PrepareContinueGoldConvoy();
		return;
	}
	if(CheckAttribute(&characters[GetCharacterIndex(sChar)], "mapEnc.quest"))
	{
		int iQuest = sti(characters[GetCharacterIndex(sChar)].mapEnc.quest);
		string sQuest = "sink_ship_character_" + sChar + "_wait_before_travel";
		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 2);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 2);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 2);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "sink_ship_travel";
		pchar.quest.(sQuest).quest = iQuest;

		return;
	}  */
}

void Map_TraderSucces_quest(string sChar)
{
	ref character = CharacterFromID(sChar);
	// LEO: рейс компаньона к складу идёт дружественным энкаунтером (companionsail), прибытие приходит
	//   отдельным событием WorldMap_CompanionArrived - не через Map_TraderSucces. Здесь ничего не делаем.

	//пиратка, квест №7
	if (sChar == "LeonCapitain")
	{
		pchar.questTemp.piratesLine = "Soukins_LeonNotFound";
		QuestSetCurrentNode("Henry Morgan", "PL_Q7_LeonNotFound"); //нода, ничего не узнал
		AddQuestRecord("Pir_Line_7_Soukins", "14");
		Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_1", "Logs.txt", "#space#", " "));
		Map_ReleaseQuestEncounter("LeonCapitain");
		group_DeleteGroup("Leon_Group");
	}
	//пиратка, квест №7
	if (sChar == "QuestCap_PL7")
	{
		pchar.questTemp.piratesLine = "Soukins_battlshipNotFound";
		AddQuestRecord("Pir_Line_7_Soukins", "12");
		AddQuestUserData("Pir_Line_7_Soukins", "sSex", GetSexPhrase("","а"));
		Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_2", "Logs.txt", "#space#", " "));
		Map_ReleaseQuestEncounter("QuestCap_PL7");
		group_DeleteGroup("Quest_Ship");
	}

	// Gregg: «Чёрная Орхидея», глава 9 - «Искатель» дошёл до второй стоянки. Обработчика
	// прибытия у этого энкаунтера не было ни в оригинале, ни в порте, и корабль просто
	// пропадал с карты: игрок доходил до бухты и никого там не заставал. Разворачиваем.
	if (sChar == "Brus Fisher")
	{
		BrusEncounterArrived();
	}

	// Gregg: курьер ГВИК (квест «Контора на Ямайке») дошёл до порта - донос ушёл, эмиссар предложит откуп
	if (sChar == "GWIK_JamCourier")
	{
		Map_ReleaseQuestEncounter("GWIK_JamCourier");
		group_DeleteGroup("GWIK_JamCourierGroup");
		if (CheckAttribute(pchar, "GWIK.Jamaica.Stage") && sti(pchar.GWIK.Jamaica.Stage) == 2 && !CheckAttribute(pchar, "GWIK.Jamaica.CourierDone"))
		{
			pchar.GWIK.Jamaica.CourierEscaped = true;
			pchar.quest.GWIK_JamCourierDead.over = "yes"; // курьера уже нет - снять прерывание на его смерть
			AddQuestRecord("GWIK_JamaicaJob", "11");
			if (LanguageGetLanguage() == "russian") log_info("Английский курьер ушёл в порт - донос дошёл. Загляните к эмиссару Компании в Форт-Оранже.");
			else log_info("The English courier reached port - the denunciation got through. See the Company envoy in Fort Orange.");
		}
	}

	// Gregg: пиратский корабль с пленником (выкуп капитана ГВИК) дошёл до порта - НЕ провал: он патрулирует, перезапускаем отрезок. Реальный провал - потопление (капитан гибнет) или месячный таймаут GWIK_RansomTimeout
	if (sChar == "GWIK_RansomShip")
	{
		if (CheckAttribute(pchar, "GWIK.Job") && pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.Force") && !CheckAttribute(pchar, "GWIK.Job.Rescued"))
			GWIK_RansomDispatchLeg();
		else
			Map_ReleaseQuestEncounter("GWIK_RansomShip"); // задание закрыто - просто снять энкаунтер
	}

	// Gregg: судно конкурента (морской вариант «удар по конкурентам») дошло до порта - НЕ провал: конкурент патрулирует воды своей колонии, перезапускаем очередной отрезок. Реальный провал - только месячный таймаут GWIK_RivalGone
	if (sChar == "GWIK_RivalShip")
	{
		if (CheckAttribute(pchar, "GWIK.Job") && pchar.GWIK.Job == "rival" && !CheckAttribute(pchar, "GWIK.Job.Done") && CheckAttribute(pchar, "GWIK.Job.RivalMoving"))
			GWIK_RivalDispatchLeg();
		else
			Map_ReleaseQuestEncounter("GWIK_RivalShip"); // задание уже закрыто - просто снять энкаунтер
	}

	// Warship 08.07.09 Пасхалка с бригантиной Мэри Селест
	if(sChar == "MaryCelesteCapitan")
	{
		Map_ReleaseQuestEncounter("MaryCelesteCapitan");

		// Для палубы, при Map_ReleaseQuestEncounter() рассылалось событие, а ставить ещё раз не нужно
		if(PChar.QuestTemp.MaryCeleste != "OnDeck")
		{
			character.fromCity = character.toCity; // Колония, из бухты которой выйдет
			character.fromShore = character.toShore;
			character.toCity = SelectAnyColony(character.fromCity); // Колония, в бухту которой придет
			character.toShore = GetIslandRandomShoreId(GetArealByCityName(character.toCity));

			Map_CreateTrader(character.fromShore, character.toShore, "MaryCelesteCapitan", GetMaxDaysFromIsland2Island(GetArealByCityName(character.toCity), GetArealByCityName(character.fromCity)));

			Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_3", "Logs.txt", "#space#", " ") + character.fromCity + GetConvertStrWithReplace("Variable_worldmap_encgen_4", "Logs.txt", "#space#", " ") + character.toShore);
		}
	}

	// ugeen --> генератор "Повод для спешки"
	if(sChar == "PirateCapt")
	{
		Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_7", "Logs.txt", "#space#", " "));
		Map_ReleaseQuestEncounter(sChar);
		AddQuestRecord("ReasonToFast","26");
		CloseQuestHeader("ReasonToFast");
		DeleteAttribute(pchar,"questTemp.ReasonToFast");
	}

	//розыск и отдача кэпу судового журнала
	if (findsubstr(sChar, "PortmansCap_" , 0) != -1 && characters[GetCharacterIndex(sChar)].quest == "InMap")
	{
		SetCapitainFromSeaToCity(sChar);
		Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_8", "Logs.txt", "#space#", " ") + sChar + GetConvertStrWithReplace("Variable_worldmap_encgen_9", "Logs.txt", "#space#", " "));
	}
	//поиски кэпа-вора
	if (findsubstr(sChar, "SeekCap_" , 0) != -1 && characters[GetCharacterIndex(sChar)].quest == "InMap")
	{
		SetRobberFromMapToSea(sChar);
		Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_10", "Logs.txt", "#space#", " ") + sChar + GetConvertStrWithReplace("Variable_worldmap_encgen_11", "Logs.txt", "#space#", " "));
	}
	//поиски кэпа, квест дают горожане
	if (findsubstr(sChar, "SeekCitizCap_" , 0) != -1)
	{
		int iChar = GetCharacterIndex(sChar);
		if (characters[iChar].quest == "InMap")
		{
			CitizCapFromMapToCity(sChar);
			Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_12", "Logs.txt", "#space#", " ") + sChar + GetConvertStrWithReplace("Variable_worldmap_encgen_13", "Logs.txt", "#space#", " "));
		}
		if (characters[iChar].quest == "outMap")
		{
			string sTemp = "SCQ_" + characters[iChar].index;
			pchar.quest.(sTemp).over = "yes"; //снимаем прерывание смерть кэпа
			characters[iChar].lifeDay = 0;
		}
	}
	// Gregg: должник-капитан из генератора ростовщика дошёл до порта
	if (findsubstr(sChar, "LoanCap_" , 0) != -1)
	{
		int iLoanCap = GetCharacterIndex(sChar);
		// Gregg: должник мог быть вычищен раньше, чем снят энкаунтер с карты
		if (iLoanCap != -1)
		{
			if (characters[iLoanCap].quest == "InMap")
			{
				LoanCap_FromMapToCity(sChar);
			}
		}
	}
	//поиски бригантины с мушкетом
	if (sChar == "MushketCap" && characters[GetCharacterIndex(sChar)].quest == "InMap")
	{
		SetMushketFromMapToSea();
		Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_14", "Logs.txt", "#space#", " "));
	}
	//поиски брига Королева
	if (sChar == "Danielle" && characters[GetCharacterIndex(sChar)].quest == "InMap")
	{
		SetDanielleFromMapToSea();
		Log_TestInfo("Энканутер кэпа брига Queen дошёл до места назначения.");
	}
	//поиски Чёрной Бороды
	if (sChar == "BlackBeardNPC" && characters[GetCharacterIndex(sChar)].quest == "InMap")
	{
		SetBlackBeardFromMapToSea();
		Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_encgen_15", "Logs.txt", "#space#", " "));
	}
}
