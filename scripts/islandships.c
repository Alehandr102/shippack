////  файл для кораблей в порту, море и сторожевиков
int iCount;
void GenerateIslandShips(string sIslandID)
{
	if (FindIsland(sIslandID) == -1) return;
	int iColonyQuantity = sti(Islands[FindIsland(sIslandID)].colonyquantity);
	int iNation;
	int iShipsQuantity;
	int iChar;
	int iType = 0;
	ref chr;
	string sTemp;
	int defendersCount = MOD_DEFENDERS_RATE;

	int MainDefenderChar = -1;

	for (int i = 0; i< MAX_COLONIES; i++)
	{
		iCount = 1;
		MainDefenderChar = -1;
		if (Colonies[i].island == sIslandID)
		{
			if (colonies[i].nation != "none" && colonies[i].id != "Caiman" && !CheckAttribute(&colonies[i], "HasNoFort"))
			{
				// LEO: сторожевики ОБОРОНЯЮТ форт при осаде (Siege) — даже с DontSetShipInPort. Без осады, но с блокировкой
				//   порта (квесты — Гавана/золотой флот, Бриджтаун и т.п.) — не спавним. Торгаши — только когда порт совсем свободен.
				bool bDefendersAllowed = true;
				if (CheckAttribute(&colonies[i], "DontSetShipInPort")) { if (!CheckAttribute(&colonies[i], "Siege")) bDefendersAllowed = false; }
				bool bTradersAllowed = true;
				if (CheckAttribute(&colonies[i], "Siege")) bTradersAllowed = false;
				if (CheckAttribute(&colonies[i], "DontSetShipInPort")) bTradersAllowed = false;
				bool FortDefender;
				ref FortChref = GetFortCommander(colonies[i].id);
				if (CheckAttribute(&colonies[i], "AlreadyGen")) //охрана
				{
					int check = 0;
					for (int j=GlobalCharacters; j<MAX_CHARACTERS; j++)
					{
						if (CheckAttribute(&characters[j], "fortDefender") && CheckAttribute(&characters[j], "IslandShips") && characters[j].IslandShips == colonies[i].id)
						{
							check++;
						}
					}
					if (check == 0) DeleteAttribute(&colonies[i],"AlreadyGen"); // сторожевики выбиты -> разрешаем перегенерацию
				}
				if (bDefendersAllowed && !CheckAttribute(&colonies[i], "AlreadyGen")) // генерируем охрану один раз
				{
					iNation = sti(Colonies[i].nation);

					FortDefender = false;

					// бюджет сторожевиков — ОБЩИЙ на остров (defendersCount объявлен вверху = MOD_DEFENDERS_RATE).
					// НЕ сбрасываем на колонию: иначе многоколониальные острова плодят десятки крупных линейников
					// у портов -> зависание в виде с палубы (движок рендерит их такелаж вблизи).
					if (defendersCount != 0) // фикс: раньше стояла ; после if -> блок выполнялся всегда
					{
						// форт пал/в осаде или наш (пиратский) город -> охрану (и торгашей ниже) не спавним
						// NB: Storm не парсит A && (B||C||D) в одном выражении -> вложенные if
						if (CheckAttribute(FortChref, "Fort.Mode"))
						{
							if (sti(FortChref.Fort.Mode) == FORT_ABORDAGE || sti(FortChref.Fort.Mode) == FORT_DEAD || iNation == PIRATE) continue;
						}
						colonies[i].AlreadyGen = true;
						for (j = 0; defendersCount > 0; j++) // тратим ОБЩИЙ бюджет острова (декремент в конце тела)
						{
							sTemp = "off_" + NationShortName(iNation) + "_" + (rand(1) + 1);
							iChar = NPC_GenerateCharacter("FortDefender" + i + j, sTemp, "man", "spy", 30, iNation, -1, true);
							if (MainDefenderChar == -1) MainDefenderChar = iChar;
							chr = GetCharacter(iChar);

							CreateFortDefenders(chr, iNation);
							PlaceCharacterShip(iChar, iNation, sIslandID, i, true, MainDefenderChar);
							characters[iChar].IslandShips = Colonies[i].id;

							// отношение к игроку — ЕДИНОЕ для всех сторожевиков колонии (фикс "один враг, остальные нет")
							if (iNation == PIRATE) // наш город
							{
								characters[iChar].AlwaysFriend = true;
								SetCharacterRelationBoth(iChar, GetMainCharacterIndex(), RELATION_FRIEND);
							}
							else
							{
								SetCharacterRelationBoth(iChar, GetMainCharacterIndex(), GetNationRelation2MainCharacter(iNation));
							}

							characters[iChar].Ship.Mode = "war";
							if (rand(4) == 1 || GetCharacterShipClass(&characters[iChar]) == 1) SetRandGeraldSail(chr, iNation);
							characters[iChar].AlwaysSandbankManeuver = true; // запрет тонуть об берег
							characters[iChar].AnalizeShips = true;           // анализить вражеские корабли
							characters[iChar].location.from_sea = colonies[i].from_sea;

							SetCaptanModelByEncType(chr, "war");
							SetRandomNameToShip(chr);
							SetBaseShipData(chr);
							Fantom_SetCannons(chr, "war");
							Fantom_SetBalls(chr, "war");
							AddItems(chr, "potion2", 10);

							Ship_SetTaskDefend(SECONDARY_TASK, iChar, GetFortCommanderIdx(Colonies[i].id));
							characters[iChar].Task.Lock = true;
							SetCrewQuantityFull(chr);
							Ship_FlagRefresh(chr);

							// снаряга и навыки сторожевика
							SelAllPerksToNotPchar(chr);
							if (CheckAttribute(chr, "perks.list.ShipEscape")) DeleteAttribute(chr, "perks.list.ShipEscape"); // сторож не сбегает
							switch (rand(2))
							{
								case 0: GiveItem2Character(chr, "blade32"); EquipCharacterByItem(chr, "blade32"); break;
								case 1: GiveItem2Character(chr, "blade28"); EquipCharacterByItem(chr, "blade28"); break;
								case 2: GiveItem2Character(chr, "blade26"); EquipCharacterByItem(chr, "blade26"); break;
							}
							SetSelfSkill(chr, 100, 100, 100, 100, 90);
							SetShipSkill(chr, 90, 90, 90, 90, 100, 90, 90, 90, 90);
							defendersCount = defendersCount - 1; // потратили единицу общего бюджета острова
						}
					}

				}
				if (!bTradersAllowed) continue; // LEO: при осаде/блокировке порта торгашей не спавним (сторожа выше уже отработали)
				if (!CheckAttribute(&colonies[i], "GenShipDate") || GetNpcQuestPastDayParam(&colonies[i], "GenShipDate") > 0) //торгаши
				{
                    SaveCurrentNpcQuestDateParam(&colonies[i], "GenShipDate"); // дата заполнения
					iNation = sti(Colonies[i].nation);

					FortDefender = false;

					iShipsQuantity = rand(2);
					if (CheckAttribute(FortChref,"Fort.Mode"))
					{
						if(sti(FortChref.Fort.Mode) == FORT_ABORDAGE || sti(FortChref.Fort.Mode) == FORT_DEAD || iNation == PIRATE)
						{
							iShipsQuantity = 0;
							continue;
						}
					}
					for (j = 0; j < iShipsQuantity; j++)
					{
						iChar = NPC_GenerateCharacter("ColonyTrader" + i +j, "trade", "man", "man", MOD_SKILL_ENEMY_RATE*3, iNation, -1, true);
						chr = GetCharacter(iChar);
						chr.AlwaysSandbankManeuver = true;
						CreateFortMerchants(chr, iNation);
						PlaceCharacterShip(iChar, iNation, sIslandID, i, FortDefender, MainDefenderChar);
						characters[iChar].IslandShips = Colonies[i].id;

						// отношение к игроку — единое по нации (консистентный флаг/реакция)
						if (iNation == PIRATE) // наш город
						{
							characters[iChar].AlwaysFriend = true;
							SetCharacterRelationBoth(iChar, GetMainCharacterIndex(), RELATION_FRIEND);
						}
						else
						{
							SetCharacterRelationBoth(iChar, GetMainCharacterIndex(), GetNationRelation2MainCharacter(iNation));
						}

						characters[iChar].Ship.Mode = "trade";

						SetRandomNameToShip(chr);
						SetBaseShipData(chr);
						SetCrewQuantity(chr, GetOptCrewQuantity(chr));

						if (rand(4) == 1 || GetCharacterShipClass(&characters[iChar]) == 1) SetRandGeraldSail(chr, sti(characters[iChar].Nation));
						characters[iChar].AlwaysSandbankManeuver = true;
						characters[iChar].AnalizeShips = true; //анализить вражеские корабли
						characters[iChar].location.from_sea = colonies[i].from_sea;

						SetCaptanModelByEncType(chr, characters[iChar].Ship.Mode);//

						Fantom_SetCannons(chr, characters[iChar].Ship.Mode);
						Fantom_SetBalls(chr, characters[iChar].Ship.Mode);
						Fantom_SetGoods(chr, characters[iChar].Ship.Mode);
						SetSeaFantomParam(chr, characters[iChar].Ship.Mode);


						Ship_FlagRefresh(chr);
					}
				}
			}
		}
	}
}
void PlaceCharacterShip(int iChar, int iNation, string sIslandID, int iColonyIdx, bool isFortDefender, int defendersGroup)
{
	int iColonyNum = sti(colonies[iColonyIdx].num);
	string sColonyID = colonies[iColonyIdx].id;

	ref chr = GetCharacter(iChar);
	ref FortChref = GetFortCommander(colonies[iColonyIdx].id);
	string sGroup1 = "Fort_"+colonies[iColonyIdx].id;
	Group_AddCharacter(sGroup1, FortChref.id);
	Group_SetGroupCommander(sGroup1, FortChref.id);

	string sGroup = "IslandGroup" + iChar;
	string sLocatorGroup = "IslandShips" + iColonyNum;
	string sLocator = "Ship_"+(rand(5)+1);
	if(isFortDefender)
	{
		sLocator = "Ship_" + iCount;
		iCount++;
	}

	Group_AddCharacter(sGroup, characters[iChar].id);

	if(isFortDefender)
	{
		Group_SetGroupCommander(sGroup, characters[defendersGroup].id); // все сторожевики колонии — в группе главного сторожевика
	}
	else
	{
		Group_SetGroupCommander(sGroup, characters[iChar].id);
	}

	Group_SetAddress(sGroup, sIslandID, sLocatorGroup, sLocator);

	int iTask = rand(1);
	float x, z;
	if(isFortDefender)
	{
		characters[iChar].WatchFort = true;
		characters[iChar].fortDefender = 1;
		chr.SeaAI.Task = AITASK_DEFEND;
		chr.SeaAI.Task.Target = GetCharacterIndex(FortChref.id);
	}
	else
	{
		if (iTask == 0)
		{
			x = rand(1000000);
			z = rand(1000000);
			Group_SetTaskMove(sGroup, x, z);
		}
		else
		{
			Ship_SetTaskDrift(SECONDARY_TASK, iChar);
			Group_SetTaskNone(sGroup);
		}
	}
}
// очистить порт
void ClearIslandShips(string _city)
{
	if (!bSeaActive)
	{
		for (int i=GlobalCharacters; i<MAX_CHARACTERS; i++)
		{
			// Gregg: неудаляемых квестовых персонажей колониальная зачистка не трогает.
			// Без этого сторожевик, поставленный квестом, стирался InitCharacter на первой
			// же осаде - и приходилось не ставить ему IslandShips, из-за чего ИИ обороны
			// форта читал пустой атрибут.
			if (CheckAttribute(&characters[i], "NotRemoved")) continue;
			// отдельный код зачистки boal -->
			if (CheckAttribute(&characters[i], "IslandShips") && characters[i].IslandShips == _city)
			{
				InitCharacter(&characters[i], i);
			}
			if (CheckAttribute(&characters[i], "fortDefender") && sti(characters[i].fortDefender) == 1 && characters[i].IslandShips == _city)
			{
				InitCharacter(&characters[i], i);
			}
		}
		// потрем пустые группы
		Group_FreeAllDead();
	}
}
