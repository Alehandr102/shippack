// PGG_GiveAndEquip: give an item to a PGG and equip it
void PGG_GiveAndEquip(ref chr, string sItem)
{
	GiveItem2Character(chr, sItem);
	EquipCharacterByItem(chr, sItem);
}

void PGG_UpdateStats(ref chr, string sExpType)
{
	float fMod;
	int iMoney = 0;
	int i, iRnd;
	string sHeroType = chr.HeroParam.HeroType;
	bool bDailyUpdate = false;
	if (sExpType == "Daily") bDailyUpdate = true;
	if (sExpType == "DailyFast") bDailyUpdate = true;

	if (CheckAttribute(chr,"PGGAi.IsPGG") && chr.PGGAi.IsPGG == false) return;
	if (CheckAttribute(chr, "willDie") || LAi_IsDead(chr)) return;
	if (!CheckAttribute(chr, "PGGAi.OldRank")) chr.PGGAi.OldRank = chr.Rank;
	if (bDailyUpdate)
	{
		if (chr.PGGAi.location == "land")
		{
			//время кача в часах

			fMod = MOD_SKILL_ENEMY_RATE*3 + rand(MOD_SKILL_ENEMY_RATE*3 + 12);
			fMod = fMod*EXP_MODIFIER;

			AddCharacterExpToSkill(chr, GetEquipedBladeType(chr), fMod*261);
			AddCharacterExpToSkill(chr, SKILL_PISTOL, fMod*34.444);
			AddCharacterExpToSkill(chr, SKILL_FORTUNE, fMod*11.369);
			AddCharacterExpToSkill(chr, SKILL_LEADERSHIP, fMod*11.759);
			AddCharacterExpToSkill(chr, SKILL_DEFENCE, fMod*11.5);
			//время кача в часах
			fMod = MOD_SKILL_ENEMY_RATE*3 + rand(12);
			fMod = fMod*EXP_MODIFIER;
			AddCharacterExpToSkill(chr, SKILL_COMMERCE, fMod*25.2);
			if(GetCharacterShipClass(chr) > GetCharacterShipClass(PChar) || rand(9) == 0)
			{
				PGG_AddMoneyToCharacter(chr, PGG_AddShipsBattleExp(chr, 1));
			}
		}
		else
		{
			//20 процентов шанс на бой в море за день... для торговца.
			iRnd = 20;
			//для остальных чаще, т.к. основной доход.
			if (sHeroType != "Merchant")
			{
				iRnd += 15;
			}

			if (GetCharacterShipClass(chr) > GetCharacterShipClass(PChar) || rand(100) < iRnd)
			{
				//даю меньше от 1 до 6 кораблей...
				PGG_AddMoneyToCharacter(chr, PGG_AddShipsBattleExp(chr, 1 + rand(makeint(MOD_SKILL_ENEMY_RATE*3*1.5))));
			}
		}
	}
	if (sExpType == "Task")
	{
		switch (chr.PGGAi.Task)
		{
		case PGG_TASK_WORKONMAYOR:
            AddCharacterExpToSkill(chr, SKILL_LEADERSHIP, 20 + MOD_SKILL_ENEMY_RATE*3 + rand(MOD_SKILL_ENEMY_RATE*3 + 40));
		    AddCharacterExpToSkill(chr, SKILL_COMMERCE, 3 + MOD_SKILL_ENEMY_RATE*3);
			iMoney = Makeint(sti(chr.rank)*200 + (GetSummonSkillFromNameToOld(chr, SKILL_LEADERSHIP)*30 + 700 + rand(20)*100));
			// + Кач за боевки
			iRnd = rand(100);
			//убить пирата
			if (sti(chr.Ship.Type) != SHIP_NOTUSED && iRnd > 67)
			{
				iMoney = PGG_AddShipsBattleExp(chr, 1);
			}
			//банды, контра
			else
			{
				//TODO: статистика? а нужна она вообще?
			}
			PGG_AddMoneyToCharacter(chr, iMoney);
			break;
		}
		PGG_DebugLog(chr.id + " " + chr.PGGAi.Task + " money: " + iMoney);
	}

	if (sExpType == "DailyFast") return;

	PGG_UpdateStatsMaintenance(chr);
}

void PGG_UpdateStatsDaily(ref chr, int iDays)
{
	int i;

	if (iDays <= 1)
	{
		PGG_UpdateStats(chr, "Daily");
		return;
	}

	for (i = 0; i < iDays; i++)
	{
		PGG_UpdateStats(chr, "DailyFast");
	}
	PGG_UpdateStatsMaintenance(chr);
}

void PGG_UpdateStatsMaintenance(ref chr)
{
	if (CheckAttribute(chr,"PGGAi.IsPGG") && chr.PGGAi.IsPGG == false) return;
	if (CheckAttribute(chr, "willDie") || LAi_IsDead(chr)) return;
	if (!CheckAttribute(chr, "PGGAi.OldRank")) chr.PGGAi.OldRank = chr.Rank;

	PGG_UpdatePerks(chr);
	PGG_UpdateEquip(chr);
	PGG_UpdateShipEquip(chr);
	//оптимизация.
	if (chr.PGGAi.OldRank != chr.Rank && rand(100) < 40)
	{
		chr.PGGAi.OldRank = chr.Rank;
		DelBakSkillAttr(chr);
		ClearCharacterExpRate(chr);
		RefreshCharacterSkillExpRate(chr);
	}
}

//кач персонажа за бой с кораблями. Возвращает стоимость добычи.
int PGG_AddShipsBattleExp(ref chr, int _shipsNum)
{
	string sDebugStr;
	ref rShip;
	int i, iMoney;
	float fTmp = 0.0;
	float fMod = MOD_SKILL_ENEMY_RATE*3 * EXP_MODIFIER * _shipsNum;

	AddCharacterExpToSkill(chr, SKILL_ACCURACY, fMod*29.4 + FRAND(100.0));
	AddCharacterExpToSkill(chr, SKILL_SAILING, fMod*14.0);
	AddCharacterExpToSkill(chr, SKILL_CANNONS, makefloat((12+rand(80))*12.5));

	sDebugStr = chr.id + " fight with " + _shipsNum + " ships.";
	iMoney = 0;
	//абордаж
	if (rand(100) < 30 && sti(chr.Ship.Type) != SHIP_NOTUSED)
	{
		//взял себе
		rShip = GetRealShip(sti(chr.Ship.Type));
		if (rand(2) == 1)
		{
			fTmp = 1.0;
			//примерная цена за корабль
			iMoney = sti(rShip.Price)*0.2 + rand(sti(sti(rShip.Price)*0.5));
			sDebugStr += " Took them.";
		}
		//утопил
		else
		{
			AddCharacterExpToSkill(chr, SKILL_ACCURACY, fMod * 40);
			AddCharacterExpToSkill(chr, SKILL_SAILING, fMod * 140);
			AddCharacterExpToSkill(chr, SKILL_CANNONS, fMod * 40);
			AddCharacterExpToSkill(chr, SKILL_SAILING, fMod * 200); // бонус: намеренно повышенный кач парусности за потопление (не копипаст, оставлено сознательно)
			sDebugStr += " Destroyed them.";
		}

		i = sti(chr.ship.crew.quantity);
		AddCharacterExpToSkill(chr, GetEquipedBladeType(chr), fMod*FRAND(233.4));
		AddCharacterExpToSkill(chr, SKILL_PISTOL, fMod*3.48);
		AddCharacterExpToSkill(chr, SKILL_FORTUNE, fMod*(2.4 + fTmp*20));
		AddCharacterExpToSkill(chr, SKILL_LEADERSHIP, fMod*(2.4 + fTmp*25));
		AddCharacterExpToSkill(chr, SKILL_DEFENCE, fMod*(1.0 + fTmp*40));
		AddCharacterExpToSkill(chr, SKILL_COMMERCE, fMod * 50 * fTmp);
		AddCharacterExpToSkill(chr, SKILL_REPAIR, fMod * 60 * fTmp);

		AddCharacterExpToSkill(chr, SKILL_GRAPPLING, ((1+i-rand(i))/3+0.5+110) + (fMod * 120 * fTmp));
		AddCharacterExpToSkill(chr, SKILL_DEFENCE, (1+i-rand(i))/3+0.5+60);
		AddCharacterExpToSkill(chr, SKILL_SAILING, ((1+i-rand(i))/3+0.5+110) + (fMod * 120 * fTmp));
		AddCharacterExpToSkill(chr, SKILL_FORTUNE, (1+i-rand(i))/3+0.5+60);

		//TODO: Что там по деньгам в каюте? Сколько вешать в граммах? :)
		iMoney += 1000 + rand(2000);
		//примерная стоимость товара.
		i = 1;
		//TODO: большой куш!!!! сколько макс можно взять?
		if (rand(100) < 5)
		{
			PGG_DebugLog("PGG BIG LUCK");
			i = 5;
		}
		iMoney += rand(100)*10*i;
	}
	PGG_DebugLog(sDebugStr);
	return iMoney * _shipsNum * 4;
}

//прокачка перков на полученные очки
void PGG_UpdatePerks(ref chr)
{
	//реже будет.... а то часто очень.
	if (!CheckAttribute(chr, "perks.freepoints_self")) chr.perks.freepoints_self = 0;
	if (!CheckAttribute(chr, "perks.freepoints_ship")) chr.perks.freepoints_ship = 0;

	if (sti(chr.perks.freepoints_self) > 0 || sti(chr.perks.freepoints_ship) > 0)
	{
		chr.perks.freepoints_self = 0;
		chr.perks.freepoints_ship = 0;
		LAi_NPC_EquipPerk(chr, "fantom");
		SetCharacterPerk(chr, "Energaiser"); // скрытый перк даёт 1.5 к приросту энергии
	}
}

//==================================================
// Equip
//==================================================
//экипируемся, берем другие корабли и все такое...
void PGG_UpdateEquip(ref chr)
{
	ref rShip;
	int aShips[150];
	int i, n, iClass, iType;
	bool bOk, bShipNotUsed;
	string blade;
	int prevClass = 7;
	int curPrice = 0;

//--------------------------------------------------
// Ship Section
//--------------------------------------------------
	iType = sti(chr.Ship.Type);
	bShipNotUsed = (iType == SHIP_NOTUSED);

	if (!bShipNotUsed)
	{
        rShip = GetRealShip(iType);
		prevClass = sti(rShip.Class);
		curPrice = sti(rShip.Price);
        //Boyer fix #20170410-01 for in case of missed name
        if (!CheckAttribute(chr, "Ship.Name")) SetRandomNameToShip(chr);
	}
	iClass = PGG_GetAvailableShipClass(chr);

	bOk = !bShipNotUsed && CheckAttribute(rShip, "Class") && (sti(rShip.Class) >= iClass);
	if (bShipNotUsed || bOk)
	{
		// LEO: спец/квест корабли отсекаем по атрибуту QuestShip, а не по «дороже манвара»: ценник в
		// ините был рудиментом и служил меткой (им завышали цену, чтобы ПГГ не брали Голландца и
		// Жемчужину). Метка честнее - под неё попадают и дешёвые квестовые (Валькирия, Ксебек и др.),
		// которые по цене проходили насквозь.

		// phase 1: climb CLASS - strictly better class, within the skill ceiling, not above the price ceiling
		n = 0;
		for (i = 0; i <= SHIP_OCEAN; i++)
		{
			if (!CheckAttribute(&ShipsTypes[i], "Class")) continue;
			if (sti(ShipsTypes[i].Class) < iClass) continue;       // better than skill allows - skip
			if (CheckAttribute(&ShipsTypes[i], "QuestShip")) continue;   // special/quest ship - skip
			if (prevClass > sti(ShipsTypes[i].Class))              // strictly higher class than current
			{
				aShips[n] = i;
				n++;
			}
		}

		// phase 2: no higher class to climb to -> upgrade QUALITY within the same class
		// (a strictly pricier hull, up to the Manowar ceiling). Class 1 ends on a random ~800k ship and then freezes.
		if (n == 0 && !bShipNotUsed)
		{
			for (i = 0; i <= SHIP_OCEAN; i++)
			{
				if (!CheckAttribute(&ShipsTypes[i], "Class")) continue;
				if (sti(ShipsTypes[i].Class) != prevClass) continue;   // same class only
				if (CheckAttribute(&ShipsTypes[i], "QuestShip")) continue;   // special/quest ship - skip
				if (GetShipTypePriceByTTH(i) > curPrice)               // strictly better/pricier than current
				{
					aShips[n] = i;
					n++;
				}
			}
		}

		if (n != 0)
		{
			bOk = false;
			//если корабль уже есть, то не всегда есть возможность его сменить, поэтому 5-50 процентов
			if (!bShipNotUsed)
			{
				if (MOD_SKILL_ENEMY_RATE*3 * sti(chr.rank) > rand(10*MOD_SKILL_ENEMY_RATE*3)) bOk = true;
			}
			//а нет, так всегда купит, нужда.
			else
			{
				bOk = true;
			}
			if (bOk)
			{
				i = rand(n-1);
				iType = aShips[i];
				i = GetShipTypePriceByTTH(iType);
				i = i - sti(chr.skill.Commerce) * i / 500;

				if (sti(chr.Money) > i)
				{
					PGG_DebugLog(chr.id + " Changed ship");
					chr.Ship.Type = GenerateShipExt(iType, rand(1), chr);
					SetBaseShipData(chr);
					if (!CheckAttribute(chr, "Ship.Name")) SetRandomNameToShip(chr);
					PGG_UpdateShipEquip(chr);

					PGG_AddMoneyToCharacter(chr, -i);
					chr.PGGAi.OwnShip = true;
				}
			}
		}
	}
//--------------------------------------------------
// Blade & Gun Section
//--------------------------------------------------
	if (rand(100) < 5)
	{
		if(!CheckAttribute(chr, "DontChangeGun"))	DeleteAttribute(chr, "equip");
		//трем сабли.
		blade = FindCharacterItemByGroup(chr, BLADE_ITEM_TYPE);
		while(blade != "")
		{
			TakeItemFromCharacter(chr, blade);
			blade = FindCharacterItemByGroup(chr, BLADE_ITEM_TYPE);
		}

		blade = LAi_NPC_EquipBladeSelection(sti(chr.rank),false);
		PGG_GiveAndEquip(chr, blade);

		if(rand(1000) < MOD_SKILL_ENEMY_RATE*3*100)
		{
			float fCharRankMult = stf(chr.rank) * 0.3;
			TakeNItems(chr, "potion1", rand(makeint(fCharRankMult + 0.5))); // даю меньше
			if(GetCharacterItem(chr, "potion1") > (fCharRankMult + 4.5))	chr.Items.potion1 = MakeInt(fCharRankMult + 4.5);//5-17
			fCharRankMult = 0.5 * fCharRankMult; 
			TakeNItems(chr, "potion2", rand(makeint(fCharRankMult + 0.5))); // даю меньше
			if(GetCharacterItem(chr, "potion2") > fCharRankMult + 0.5)	chr.Items.potion2 = MakeInt(fCharRankMult + 0.5);//0-6
		}

		if(rand(1000) < MOD_SKILL_ENEMY_RATE*3 * sti(chr.rank) * 8)
		{
			//трем пистолеты.
			blade = FindCharacterItemByGroup(chr, GUN_ITEM_TYPE);
			while(blade != "")
			{
				TakeItemFromCharacter(chr, blade);
				blade = FindCharacterItemByGroup(chr, GUN_ITEM_TYPE);
			}

			if(rand(100) < 50)
			{
				PGG_GiveAndEquip(chr, "pistol1");
			}
			else
			{
				if(rand(100) < 50)
				{
					PGG_GiveAndEquip(chr, "pistol2");
				}
				else
				{
					if(rand(100) < 50)
					{
						PGG_GiveAndEquip(chr, "pistol11");
					}
					else
					{
						if(rand(100) < 70)
    					{
    						PGG_GiveAndEquip(chr, "pistol5");
    					}
						else
						{
							if(rand(100) < 80)
        					{
        						PGG_GiveAndEquip(chr, "pistol6");
        					}
							else
							{
								if (sti(chr.rank) > 9)
								{
									PGG_GiveAndEquip(chr, "pistol9");
        						}
        					}
    					}
					}
				}
			}
			if (GetCharacterItem(chr, "bullet") < 10)
			{
				TakeNItems(chr, "bullet", 5+rand(15));
			}
			if(GetCharacterItem(chr, "GunPowder") < 10)
			{
				AddItems(chr, "GunPowder", 5+rand(15));
			}
		}
	}
}


//получить тип экипированной сабли.
string GetEquipedBladeType(ref chr)
{
	string sBlade = GetCharacterEquipByGroup(chr, BLADE_ITEM_TYPE);
	if (sBlade != "")
	{
		ref rItm = ItemsFromID(sBlade);
		return rItm.FencingType;
	}
	return SKILL_FENCING;
}

//настроить параметры корабля
void PGG_UpdateShipEquip(ref chr)
{
	if (sti(chr.Ship.Type) == SHIP_NOTUSED) return;

	ref refTown, chref;
	string curTown;

	ProcessHullRepair(chr, 100.0);
	ProcessSailRepair(chr, 100.0);
	SetCrewQuantityFull(chr);

	// LEO: чистим трюм ПЕРЕД рефитом. Иначе накопленные торговые товары забивали трюм под завязку, и
	// Fantom_SetBalls не мог добавить боеприпасы/порох (Fantom_SetCharacterGoods капается по свободному месту)
	// — ПГГ выходил в бой пустой. После чистки боеприпасы грузятся ПЕРВЫМИ (~их боевой набор), товары — в остаток.
	DeleteAttribute(chr, "Ship.Cargo.Goods");
	RecalculateCargoLoad(chr);

	if (chr.HeroParam.HeroType == "Trader")
	{
		Fantom_SetCannons(chr, "trade");
		Fantom_SetBalls(chr, "pirate");
		Fantom_SetGoods(chr, "trade");
	}
	else
	{
		Fantom_SetCannons(chr, "pirate");
		Fantom_SetBalls(chr, "pirate");
		Fantom_SetGoods(chr, "pirate");
	}

	curTown = chr.PGGAi.location.town;
	if (curTown != "")
	{
		// LEO: город ПГГ не всегда колония - бывает "none" и пиратские поселения. FindColony тогда даёт -1,
		//   а Colonies[-1] роняет в error.log «invalid index -1 / GetColonyByIndex stack error».
		if (FindColony(curTown) < 0) return;
		if (rand(5) == 2)   // нулим матросов в городе
		{
			PGG_DebugLog("PGG нулим матросов в городе " + curTown);
			refTown = GetColonyByIndex(FindColony(curTown));
			chref = CharacterFromID(curTown + "_tavernkeeper");
			SaveCurrentNpcQuestDateParam(chref, "CrewHired");
			chref.CrewHired.PGGId = chr.id;
		}
		AddRemoveGoodsInStore(curTown, GOOD_BALLS, makeint(195*PGG_RESTOCK_MULT + rand(20)), false);
		AddRemoveGoodsInStore(curTown, GOOD_BOMBS, makeint(180*PGG_RESTOCK_MULT + rand(40)), false);
		AddRemoveGoodsInStore(curTown, GOOD_KNIPPELS, makeint(95*PGG_RESTOCK_MULT + rand(20)), false);
		AddRemoveGoodsInStore(curTown, GOOD_GRAPES, makeint(75*PGG_RESTOCK_MULT + rand(20)), false);

		AddRemoveGoodsInStore(curTown, GOOD_POWDER, makeint(350*PGG_RESTOCK_MULT + rand(60)), false);
		AddRemoveGoodsInStore(curTown, GOOD_SAILCLOTH, makeint(5*PGG_RESTOCK_MULT + rand(20)), false);
		AddRemoveGoodsInStore(curTown, GOOD_PLANKS, makeint(10*PGG_RESTOCK_MULT + rand(40)), false);

		AddRemoveGoodsInStore(curTown, GOOD_FOOD, makeint(8*PGG_RESTOCK_MULT + rand(50)), false);
		AddRemoveGoodsInStore(curTown, GOOD_WEAPON, makeint(6*PGG_RESTOCK_MULT + rand(25)), false);
		AddRemoveGoodsInStore(curTown, GOOD_RUM, makeint(4*PGG_RESTOCK_MULT + rand(20)), false);
		AddRemoveGoodsInStore(curTown, GOOD_MEDICAMENT, makeint(4*PGG_RESTOCK_MULT + rand(25)), false);
	}
}
