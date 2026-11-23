string totalInfo = "";
bool isSkipable = false;
bool bEncType   = false;
string  sQuestSeaCharId = "";

void InitInterface(string iniName)
{
	EngineLayersOffOn(true);
	SetTimeScale(0.0);

    bQuestCheckProcessFreeze = true;//лочим квест и карту
    bEncType = false; //обычный тип

    GameInterface.title = "title_map";

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
    SetFormatedText("MAP_CAPTION", XI_ConvertString("title_map"));
	SetFormatedText("INFO_TEXT_QUESTION", XI_ConvertString("MapWhatYouWantToDo"));
	SetCurrentNode("INFO_TEXT_QUESTION");
	SetNewPicture("INFO_PICTURE2", "loading\SeaEnc_"+rand(3)+".dds");

    wdmRecalcReloadToSea();//выношу из CalculateInfoData
	SetFormatedText("INFO_TEXT",totalInfo);
	CalculateInfoData();//проверяем возможность скипнуть энку, там же вызов 
	SetEventHandler("InterfaceBreak","ProcessBreakExit",0); // Выход на море
	SetEventHandler("exitCancel","ProcessCancelExit",0); // Выход на море по крестику или Esc
	SetEventHandler("ievnt_command","ProcCommand",0); // выход на карту только тут (по НЕТ)
	SetEventHandler("evntDoPostExit","DoPostExit",0); // выход из интерфейса

	PlayVoice("interface\"+LanguageGetLanguage()+"_EvShip"+rand(3)+".wav");
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_MAP_EXIT );
	wdmReloadToSea();
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_MAP_EXIT );
	if (GetSelectable("B_CANCEL")==0) wdmReloadToSea();
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

    SetTimeScale(1.0);
	TimeScaleCounter = 0;
	if(IsPerkIntoList("TimeSpeed"))
	{
		DelPerkFromActiveList("TimeSpeed");
	}

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
	PostEvent("StopQuestCheckProcessFreeze", 100);//boal 230804 заморозка проверки квестов
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	ref chr;
	switch(nodName)
	{
	case "B_OK":
		if(comName=="activate" || comName=="click")
		{
			if (sQuestSeaCharId != "") wdmEnterSeaQuest(sQuestSeaCharId);

			// напасть
   			IDoExit(RC_INTERFACE_MAP_EXIT);
			wdmReloadToSea();
			if(CheckAttribute(PChar, "GenerateShipWreck.ShipInMap"))
			{
				if(GetCharacterIndex(PChar.GenerateShipWreck.ShipInMap) != -1)
				{
					chr = CharacterFromID(PChar.GenerateShipWreck.ShipInMap);
					if(CheckAttribute(chr, "ShipWreck")) ShipWreckInSea(chr);
				}
			}
		}
		if(comName=="downstep")
		{
			if(GetSelectable("B_CANCEL"))	{SetCurrentNode("B_CANCEL");}
		}
	break;

	case "B_CANCEL":
		if(comName=="activate" || comName=="click")
		{
			//пропустить
			pchar.SkipEshipIndex = pchar.eshipIndex;
			IDoExit(RC_INTERFACE_MAP_EXIT);
		}
		if(comName=="upstep")
		{
			if(GetSelectable("B_OK"))	{SetCurrentNode("B_OK");}
		}
	break;
	}
}
void wdmRecalcReloadToSea()
{
	worldMap.encounter.type = "";
	totalInfo = "";
	int iRand;
	int SquadronPowerTotal = 0;

	int k = 0;//счетчик для строк таблицы с картинками корабликов
	int iShipType, n, iS, q, bCharBranch;
	int bHasModes; // LEO: есть ли у энкаунтера ветка shipmodes (иконки режима) - см. ниже
	string sCol, sRow1, sShip;
	string sShipMode;
	aref arShipModes;
	aref arAttr, arShips;//типы кораблей в энке

	int numEncounters = wdmGetNumberShipEncounters();
	int isShipEncounterType = 0;
	// LEO: стороны пасхалки "битва пяти воинств" подписываются по-своему, и у доклада своя шапка.
	// Объявляем ЗДЕСЬ: Storm не принимает объявление переменной внутри вложенного блока.
	bool bFiveArmies = false;
	bool bFiveArmiesHere = false;
	Log_TestInfo(GetConvertStrWithReplace("Variable_map_1", "Interface.txt", "#space#", " "));

	for(int i = 0; i < numEncounters; i++)
	{
		if(wdmSetCurrentShipData(i))
		{
		   if(MakeInt(worldMap.encounter.select) == 0) continue;
			isShipEncounterType++;

			string encID = worldMap.encounter.id;

			aref rEncounter;
			makearef(rEncounter, worldMap.encounters.(encID).encdata);

			// LEO: читаем с проверкой, как и соседние поля ниже. Битая/устаревшая запись энкаунтера без
			//   RealEncounterType роняла интерфейс ещё до отрисовки кораблей.
			int iRealEncounterType = -1;
			if (CheckAttribute(rEncounter, "RealEncounterType")) { iRealEncounterType = sti(rEncounter.RealEncounterType); }
			else { continue; }

			// LEO: сторона пасхалки "битва пяти воинств" - подписывается по-своему (см. ниже)
			bFiveArmies = CheckAttribute(rEncounter, "FiveArmies");
			if (bFiveArmies) bFiveArmiesHere = true;

			if (isShipEncounterType > 1 && iRealEncounterType < ENCOUNTER_TYPE_BARREL)
			{
			    totalInfo = totalInfo + "\n" + XI_ConvertString("But in the same way");
			}

			int iNumMerchantShips = 0;
			int iNumWarShips = 0;
			if(CheckAttribute(rEncounter, "NumMerchantShips"))
			{
				iNumMerchantShips = sti(rEncounter.NumMerchantShips);
			}
			if(CheckAttribute(rEncounter, "NumWarShips"))
			{
				iNumWarShips = sti(rEncounter.NumWarShips);
			}

			if (CheckAttribute(rEncounter, "CharacterID"))
			{
                iNumWarShips = GetCharacterIndex(rEncounter.CharacterID);
                if (iNumWarShips != -1)
                {
					sQuestSeaCharId = characters[iNumWarShips].id; // квестовый
					PChar.GenerateShipWreck.ShipInMap = sQuestSeaCharId;
					if (CheckAttribute(&characters[iNumWarShips], "mapEnc.Name"))
					{
						totalInfo = totalInfo + characters[iNumWarShips].mapEnc.Name;
					}
					else
					{
						totalInfo = totalInfo + "'" + characters[iNumWarShips].ship.name + "'."
					}
				}
				bEncType = true;
			}
			else
			{
				if(iRealEncounterType <= ENCOUNTER_TYPE_MERCHANT_LARGE)
				{
					totalInfo = totalInfo + GetTextOnShipsQuantity(iNumMerchantShips) + XI_ConvertString("of traders");
				}
				if(iRealEncounterType >= ENCOUNTER_TYPE_MERCHANT_GUARD_SMALL && iRealEncounterType <= ENCOUNTER_TYPE_MERCHANT_GUARD_LARGE)
				{
					totalInfo = totalInfo + GetTextOnShipsQuantity(iNumMerchantShips) + XI_ConvertString("merchants in accompaniment") + GetTextOnSecondShipsQuantity(iNumWarShips) + XI_ConvertString("guards");
				}
				if(iRealEncounterType >= ENCOUNTER_TYPE_ESCORT_SMALL && iRealEncounterType <= ENCOUNTER_TYPE_ESCORT_LARGE)
				{
					totalInfo = totalInfo + XI_ConvertString("Trade caravan") + GetTextOnShipsQuantity(iNumMerchantShips) + XI_ConvertString("merchants in accompaniment") + GetTextOnSecondShipsQuantity(iNumWarShips) + XI_ConvertString("guards");
				}

				// LEO: пасхалка "битва пяти воинств" - сторону подписываем её именем, а не патрулём нации
				if (bFiveArmies)
				{
					q = sti(rEncounter.FiveArmies) + 1;
					totalInfo = totalInfo + GetConvertStrWithReplace("Variable_FiveArmies_" + q, "Names.txt", "#space#", " ") + ": " + GetTextOnShipsQuantity(iNumWarShips);
				}

				if(!bFiveArmies && iRealEncounterType >= ENCOUNTER_TYPE_PATROL_SMALL && iRealEncounterType <= ENCOUNTER_TYPE_PATROL_LARGE)
				{
					totalInfo = totalInfo + XI_ConvertString("Patrol") + GetTextOnShipsQuantity(iNumWarShips);
				}

				if(!bFiveArmies && iRealEncounterType >= ENCOUNTER_TYPE_PIRATE_SMALL && iRealEncounterType <= ENCOUNTER_TYPE_PIRATE_SQUADRON) // увеличиваем диапазон до PIRATE_SQUADRON - johnny88
				{
					totalInfo = totalInfo + XI_ConvertString("Pirates") + GetTextOnShipsQuantity(iNumWarShips);
				}

				if(iRealEncounterType >= ENCOUNTER_TYPE_SQUADRON && iRealEncounterType <= ENCOUNTER_TYPE_ARMADA)
				{
					totalInfo = totalInfo + XI_ConvertString("Naval squadron") + GetTextOnShipsQuantity(iNumWarShips);
				}

				if(iRealEncounterType == ENCOUNTER_TYPE_PUNITIVE_SQUADRON)
				{
					totalInfo = totalInfo + XI_ConvertString("Punitive expedition") + GetTextOnShipsQuantity(iNumWarShips);
				}
				if(iRealEncounterType == ENCOUNTER_TYPE_BARREL)
				{
					totalInfo = totalInfo + XI_ConvertString("SailingItems");
				}

				if(iRealEncounterType == ENCOUNTER_TYPE_BOAT)
				{
					totalInfo = totalInfo + XI_ConvertString("ShipWreck");
				}
			}
			if(sti(rEncounter.Nation) < 0)
			{
        		totalInfo = totalInfo + GetConvertStrWithReplace("Variable_map_2", "Interface.txt", "#space#", " ");
      		}

			if(bFiveArmies)
			{
				totalInfo = totalInfo + "."; // воинство уже названо, флаг нации тут только путал бы
			}
			if(!bFiveArmies && iRealEncounterType != ENCOUNTER_TYPE_BARREL && iRealEncounterType != ENCOUNTER_TYPE_BOAT)
            {
                string nationname = "";
                switch(sti(rEncounter.Nation))
                {
                    case ENGLAND:
                        totalInfo = totalInfo + XI_ConvertString("under english flag");
                        nationname = "Eng";
                    break;
                    case FRANCE:
                        totalInfo = totalInfo + XI_ConvertString("under french flag");
                        nationname = "Fra";
                    break;
                    case SPAIN:
                        totalInfo = totalInfo + XI_ConvertString("under spanish flag");
                        nationname = "Spa";
                    break;
                    case HOLLAND:
                        totalInfo = totalInfo + XI_ConvertString("under dutch flag");
                        nationname = "Hol";
                    break;
                    case PIRATE:
                        totalInfo = totalInfo + ".";
                        nationname = "Pir";
                    break;
                }
            }

			if(GetNationRelation2MainCharacter(sti(rEncounter.Nation)) != RELATION_ENEMY)
			{
				isSkipable = true;
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"B_OK",0, "#"+XI_ConvertString("GetCloser"));
			}
			else
			{
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"B_OK",0, "#"+XI_ConvertString("map_yes"));
			}
		}

		bCharBranch = 0;
		bHasModes = 0;
		iNumShips = 0; // LEO: обнуляем ЯВНО - иначе при отсутствии данных остаётся счёт от прошлой энки в цикле
		// LEO: КРАШФИКС. У энкаунтера может остаться CharacterID уже НЕСУЩЕСТВУЮЩЕГО персонажа: игрок напал,
		//   группа испарилась, а метка на карте уцелела (репорт игрока 29.07.2026: «напал на испанца, он
		//   испарился, вижу его опять возле себя, нападаю - краш»). Выше GetCharacterIndex вернул -1, и
		//   обращение Characters[-1].SeaAI.Group.Name роняло игру. Такую энку рисуем как обычную, по shiptypes.
		if (CheckAttribute(rEncounter, "CharacterID") && iNumWarShips >= 0)
		{
			string sGroupID = Characters[iNumWarShips].SeaAI.Group.Name;
			ref rGroup = Group_FindOrCreateGroup(sGroupID);
			if (CheckAttribute(rGroup,"quest"))
			{
				makearef(arShips, rGroup.Quest);
				iNumShips = GetAttributesNum(arShips);
				bCharBranch = 1;
			}
			else
			{
				// LEO: морской группы нет (ПГГ, гоняющий пиратов, — стычка на глобалке) -> берём типы кораблей
				//   из encdata.shiptypes, иначе эскадра ПГГ не считается в общую мощь и без иконок. bCharBranch=0
				//   переключит циклы мощи ниже на чтение типа напрямую (а не через Characters[.index]).
				if (CheckAttribute(rEncounter, "shiptypes"))
				{
					makearef(arShips, rEncounter.shiptypes);
					iNumShips = GetAttributesNum(arShips);
					if (CheckAttribute(rEncounter, "shipmodes"))
					{
						makearef(arShipModes, rEncounter.shipmodes);
						bHasModes = 1;
					}
				}
				else
				{
					trace("EncCheck map-interface. Broken group: " + sGroupID);
				}
			}
		}
		else
		{
			// LEO: КРАШФИКС. Ветки shiptypes/shipmodes создаются ТОЛЬКО в цикле по числу кораблей
			//   (Encounters_map.c) - у энки без кораблей (бочка, обломки, битая/старая запись) их нет вовсе,
			//   а makearef по отсутствующей ветке оставляет aref НЕинициализированным. Дальше по функции это
			//   давало «RUNTIME ERROR: uninitialized aref» на GetAttributeN и вылет.
			if (CheckAttribute(rEncounter, "shiptypes"))
			{
				makearef(arShips, rEncounter.shiptypes);
				iNumShips = GetAttributesNum(arShips);
				if (CheckAttribute(rEncounter, "shipmodes")) //не удалось две картинки в одну ячейку запихнуть
				{
					makearef(arShipModes, rEncounter.shipmodes);
					bHasModes = 1;
				}
			}
		}
		//Заполнение таблицы картинками корабликов
		if (iNumShips>9) //Золотой флот в три строки	
		{
			int k1=0;
			for(q = 1; q < 10; q++)//фикс - если первой ячейки нет - движок не рисует всю таблицу
			{
				sCol = "td" + q;
				sRow1 = "tr" + (k + 1);
				GameInterface.TABLE_ENC.hr.(sCol).str = "";
				GameInterface.TABLE_ENC.(sRow1).(sCol).str = "";
				sRow1 = "tr" + (k + 2);
				GameInterface.TABLE_ENC.hr.(sCol).str = "";
				GameInterface.TABLE_ENC.(sRow1).(sCol).str = "";
				sRow1 = "tr" + (k + 3);
				GameInterface.TABLE_ENC.hr.(sCol).str = "";
				GameInterface.TABLE_ENC.(sRow1).(sCol).str = "";
			}
			for(iS=0; iS<iNumShips; iS++)
			{
				if (bCharBranch)//особые одиночные энкаунтеры (bCharBranch=1 корабли из группы персонажа; 0 — типы из shiptypes)
				{
					arAttr = GetAttributeN(arShips, iS);
					// LEO: в группе может висеть запись со СТЁРТОГО слота (сборщик фантомов переиничил его
					//   под другого NPC): ship.type = SHIP_NOTUSED = 1000, а RealShips всего 900 -> вылет по
					//   индексу и падение wdmRecalcReloadToSea. Такого «участника» просто не рисуем.
					if (sti(Characters[sti(arAttr.index)].ship.type) == SHIP_NOTUSED) continue;
					iShipType = sti(RealShips[sti(Characters[sti(arAttr.index)].ship.type)].basetype);
				}
				else
				{
					iShipType = GetAttributeValue(GetAttributeN(arShips, iS));
					sShipMode = ""; // LEO: нет ветки shipmodes - рисуем без иконки режима, а не падаем
					if (bHasModes) { sShipMode = GetAttributeValue(GetAttributeN(arShipModes, iS)); }
				}
//q1 - номер строки из трёх, iS - номер корабля с нуля, iNumShips - общее число кораблей, нужно определить номер колонки и общее число колонок для центрирования
//			1	4	7	10
//	ф	0	3	6	9	12
//			2	5	8	11
				n = (iS+2)/3 + 6 - (1+(iNumShips+1)/3)/2;//9 колонок - центрируем от фактического числа кораблей
				sCol = "td" + n;
				switch (iS%3)
				{
					case 0: k1 = 2; break;
					case 1: k1 = 1; break;
					case 2: k1 = 3; break;
				}
				sRow1 = "tr" + (k + k1);
				sShip = shipstypes[iShipType].Name;
				GameInterface.TABLE_ENC.(sRow1).(sCol).str = shipstypes[iShipType].Class;
				
				SquadronPowerTotal += ClassPowerCalculate(sti(shipstypes[iShipType].Class));
				
				GameInterface.TABLE_ENC.(sRow1).(sCol).textoffset = "35,-16";
				if ( (1+(iNumShips+1)/3)%2 ) GameInterface.TABLE_ENC.(sRow1).(sCol).textoffset = "-13,-16";
				// LEO: было захардкожено "interfaces\ships\" - ветка >9 кораблей (золотой флот в 3 строки) игнорировала
				//      выбор стиля интерфейса (Стиль ПКМ). Берём путь как в ветке <=9 кораблей.
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.texture = InterfaceStates.InterfaceShipString + sShip + ".dds";
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.width = 47;
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.height = 47;
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.offset = "-2,0";
				if ( (1+(iNumShips+1)/3)%2 ) GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.offset = "-26,0";
			}
			sCol = "td" + (5 - (1+(iNumShips+1)/3)/2);//iS = -1
			sRow1 = "tr" + (k + 2);//средняя строка для флага
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.group  = "NATIONS";
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.image = Nations[sti(rEncounter.Nation)].Name;//флаг эскадры сюда
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.uv = "0,0,0,0";
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.width = 48;
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.height = 48;
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.offset = "-3, -1";
			if ( (1+(iNumShips+1)/3)%2 ) GameInterface.TABLE_ENC.(sRow1).(sCol).icon.offset = "-27, -1";
			k++;
			k++;
			k++;
		}
		else
		{
			for(q = 1; q < 10; q++)//фикс - если первой ячейки нет - движок не рисует всю таблицу
			{
				sCol = "td" + q;
				sRow1 = "tr" + (k + 1);
				GameInterface.TABLE_ENC.hr.(sCol).str = "";
				GameInterface.TABLE_ENC.(sRow1).(sCol).str = "";
			}
			for(iS=0; iS<iNumShips; iS++)
			{
				if (bCharBranch)//особые одиночные энкаунтеры (bCharBranch=1 корабли из группы персонажа; 0 — типы из shiptypes)
				{
					arAttr = GetAttributeN(arShips, iS);
					// LEO: тот же гейт, что и в ветке на 3 строки выше - запись со стёртого слота даёт
					//   ship.type = SHIP_NOTUSED и вылет по индексу RealShips
					if (sti(Characters[sti(arAttr.index)].ship.type) == SHIP_NOTUSED) continue;
					iShipType = sti(RealShips[sti(Characters[sti(arAttr.index)].ship.type)].basetype);
				}
				else
				{
					iShipType = GetAttributeValue(GetAttributeN(arShips, iS));
					sShipMode = ""; // LEO: нет ветки shipmodes - рисуем без иконки режима, а не падаем
					if (bHasModes) { sShipMode = GetAttributeValue(GetAttributeN(arShipModes, iS)); }
				}
				n = iS + 6 - iNumShips/2;//9 колонок - центрируем от фактического числа кораблей
				sCol = "td" + n;
				sRow1 = "tr" + (k + 1);
				sShip = shipstypes[iShipType].Name;
				GameInterface.TABLE_ENC.(sRow1).(sCol).str = shipstypes[iShipType].Class;
				
				SquadronPowerTotal += ClassPowerCalculate(sti(shipstypes[iShipType].Class));
				
				GameInterface.TABLE_ENC.(sRow1).(sCol).textoffset = "37,-17";
				if (iNumShips%2) GameInterface.TABLE_ENC.(sRow1).(sCol).textoffset = "-11,-17";
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.texture = InterfaceStates.InterfaceShipString + sShip + ".dds";
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.width = 47;
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.height = 47;
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.offset = "-2,0";
				if (iNumShips%2) GameInterface.TABLE_ENC.(sRow1).(sCol).icon1.offset = "-26,0";

				GameInterface.TABLE_ENC.(sRow1).(sCol).icon2.group = "ICONS_SPEC";
				switch (sShipMode)
				{
					case "trade":	GameInterface.TABLE_ENC.(sRow1).(sCol).icon2.image = "commerce skill icon"; break;
					case "war": 	GameInterface.TABLE_ENC.(sRow1).(sCol).icon2.image = "cannons skill icon"; break;
					case "pirate": 	GameInterface.TABLE_ENC.(sRow1).(sCol).icon2.image = "exit button"; break;
				}
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon2.width = 14;
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon2.height = 14;
				GameInterface.TABLE_ENC.(sRow1).(sCol).icon2.offset = "-2, 0";		//its(-2-24*(iNumShips%2))+",0";
				if (iNumShips%2) GameInterface.TABLE_ENC.(sRow1).(sCol).icon2.offset = "-26,0";

			}
			sCol = "td" + (5 - iNumShips/2);//iS = -1
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.group  = "NATIONS";
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.image = Nations[sti(rEncounter.Nation)].Name;//флаг эскадры сюда
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.uv = "0,0,0,0";
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.width = 48;
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.height = 48;
			GameInterface.TABLE_ENC.(sRow1).(sCol).icon.offset = "-3, -1";
			if (iNumShips%2) GameInterface.TABLE_ENC.(sRow1).(sCol).icon.offset = "-27, -1";
			k++;
		}
	}
	Table_UpdateWindow("TABLE_ENC");

	Log_TestInfo("isShipEncounterType :" + isShipEncounterType);
	if (isShipEncounterType > 1)
    {
		// LEO: у пасхалки своя шапка - вахтенный докладывает не о сражении вообще, а о битве пяти воинств
		if (bFiveArmiesHere)
		{
			totalInfo = XI_ConvertString("NavalSignal") + GetConvertStrWithReplace("Variable_FiveArmies_Info", "Names.txt", "#space#", " ") + "\n" + totalInfo;
		}
		else
		{
			totalInfo = XI_ConvertString("NavalSignal") + XI_ConvertString("battle on course") + "\n" + totalInfo;
		}
    }
	else
	{
		if(iRealEncounterType == ENCOUNTER_TYPE_BARREL || iRealEncounterType == ENCOUNTER_TYPE_BOAT)
		{
			if(iRealEncounterType == ENCOUNTER_TYPE_BARREL)
			{
				SetNewPicture("INFO_PICTURE2", "loading\BarrelEnc_"+rand(2)+".dds");
			}
			if(iRealEncounterType == ENCOUNTER_TYPE_BOAT)
			{
				SetNewPicture("INFO_PICTURE2", "loading\BoatEnc_"+rand(5)+".dds");
			}
			totalInfo = XI_ConvertString("NavalSignal") + XI_ConvertString("SpecialSituation") + totalInfo;
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"B_OK",0, "#"+XI_ConvertString("GetItemToBort"));
		}
		else
		{
			totalInfo = XI_ConvertString("NavalSignal") + XI_ConvertString("someone sails") + "\n" + totalInfo;

		}
	}
	SetFormatedText("SQUADRON_INFO", GetConvertStrWithReplace("Variable_ship_73", "Interface.txt", "#space#", " ") + " - " + SquadronPowerTotal);
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void CalculateInfoData()
{
	if (IsCharacterPerkOn(GetMainCharacter(), "SailingProfessional") || pchar.space_press == "1")
	{
		isSkipable = true;
	}
	SetCurrentNode("B_OK");

    float iSKILL_SNEAK = GetSummonSkillFromNameToOld(pchar, SKILL_SNEAK);

    SetSelectable("B_CANCEL",true);
    if (iSKILL_SNEAK < 1) iSKILL_SNEAK = 1;
	if( (rand(100) / iSKILL_SNEAK) > GetSummonSkillFromNameToOld(pchar, SKILL_SAILING) )// сделать учет скила навигация для запрета кнопки Нет
	{
        if (!isSkipable && !bBettaTestMode) SetSelectable("B_CANCEL",false);
	}
	if (pchar.space_press == "1") bEncType = false;

	if (bEncType && !bBettaTestMode) // спец тип не пропустить
	{
        SetSelectable("B_CANCEL",false);
	}
	pchar.space_press = 0;
}
