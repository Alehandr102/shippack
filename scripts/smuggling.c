// SLiB Новый файл для ВМЛ, все от К3 потерто

int ChangeContrabandRelation(ref pchar, int _val)
{
   int prevVal = makeint(pchar.questTemp.Contraband.relation);
   pchar.questTemp.Contraband.relation = prevVal + _val;
   if (makeint(pchar.questTemp.Contraband.relation) > 99) pchar.questTemp.Contraband.relation = 99;
   if (makeint(pchar.questTemp.Contraband.relation) < 0) pchar.questTemp.Contraband.relation = 0;

   int newVal = makeint(pchar.questTemp.Contraband.relation);

   if (sti(pchar.index) == GetMainCharacterIndex()) LogContrabandRelationChange(newVal - prevVal, newVal); // лог только про ГГ

   return newVal;
}

// работа с контрабандой
//Jason -- > функция выберет из рандомного интервала в массиве товаров контрабандный товар в текущем магазине, первый в интервале. Если такового не окажется, то вернёт -1.
int SelectContrabandGoods(ref _refCharacter)
{
	int i;
	int curStoreIdx = GetCharacterCurrentStore(_refCharacter);
	if(curStoreIdx>=0)
	{
		for(i=rand(30); i<31; i++)
		{
			if (GetStoreGoodsType(&Stores[curStoreIdx],i) == TRADE_TYPE_CONTRABAND_NAME) return i;
			else return -1;
		}
	}
}
//<-- выбор рандомного контрабандного товара

void SetContraGoods(ref _refCharacter, int _Goods, int _Quantity)
{
	string goodsName = Goods[_Goods].name;
	_refCharacter.Goods.(goodsName) = _Quantity;
}

void AddContraGoods(ref _refCharacter, int _Goods, int _Quantity)
{
	string goodsName = Goods[_Goods].name;
	if(CheckAttribute(_refCharacter,"Goods."+goodsName))
	{
		_refCharacter.Goods.(goodsName) = sti(_refCharacter.Goods.(goodsName)) + _Quantity;
	}
	else
	{
		_refCharacter.Goods.(goodsName) = _Quantity;
	}
}

void RemoveContraGoods(ref _refCharacter, int _Goods, int _Quantity)
{
	int    curQuantity;
	string goodsName = Goods[_Goods].name;

	curQuantity = sti( _refCharacter.Goods.(goodsName));
	if (curQuantity >= _Quantity)
	{
		_refCharacter.Goods.(goodsName) = curQuantity - _Quantity;
	}
	else
	{
		_refCharacter.Goods.(goodsName) = 0;
	}
}

int GetContraGoodsQuantity(ref _refCharacter, int _Goods)
{
	string tmpstr = Goods[_Goods].name;
	int q = 0;
	if( CheckAttribute(_refCharacter,"Goods."+tmpstr) )
		q = sti(_refCharacter.Goods.(tmpstr));
	return q;
}

// установить агенту контриков товары, опираясь на наличие контрабандного товара в соответсвующем магазине
void SetAllContraGoods(ref _refStore, ref _refCharacter)
{
    string 	tmpstr;
	int		qty;

	for (int i = 0; i < GOODS_QUANTITY; i++)
	{
		tmpstr = Goods[i].name;
		if( CheckAttribute(_refStore,"Goods."+tmpstr+".canbecontraband") )
		{
			qty = sti(_refStore.Goods.(tmpstr).Quantity);
			if(sti(_refStore.Goods.(tmpstr).canbecontraband) == CONTRA_SELL)  	// можем продать товар  контрикам
			{
				SetContraGoods(_refCharacter, i, 0);
			}
			if(sti(_refStore.Goods.(tmpstr).canbecontraband) == CONTRA_BUY)		// можем купить товар у контриков
			{
				SetContraGoods(_refCharacter, i, makeint(qty/4) + rand(200));
			}
		}
	}
	SetSpecialGoods(_refCharacter);
}

void RemoveAllContraGoods(ref _refCharacter)
{
	DeleteAttribute(_refCharacter,"Goods");
}

int FindContrabandGoods(ref _refCharacter)
{
	int i, j = 0;
	int curStoreIdx = GetCharacterCurrentStore(_refCharacter);
	if(curStoreIdx >= 0)
	{
		_refCharacter.FindContrabandGoods.StoreIdx = curStoreIdx;
		for(i=0; i<GOODS_QUANTITY; i++)
		{
			if( GetStoreGoodsType(&Stores[curStoreIdx],i) == TRADE_TYPE_CONTRABAND_NAME || GetContrabandGoods(&Stores[curStoreIdx],i) == CONTRA_SELL )
			{
				if( GetSquadronGoods(_refCharacter,i) > 0 )
				{
					_refCharacter.FindContrabandGoods.GoodsIdx = i;
					j++;
				}
			}
		}
		if(j == 0)
		{
			DeleteAttribute(_refCharacter,"FindContrabandGoods");
			return -1;
		}
	}
	return j;
}

// работа с контрабандой
int FindFirstContrabandGoods(ref _refCharacter)
{
	if (sti(_refCharacter.Ship.Type) == SHIP_NOTUSED) return -1;

	int i;
	int curStoreIdx = GetCharacterCurrentStore(_refCharacter);
	if(curStoreIdx>=0)
	{
		_refCharacter.FindContrabandGoods.StoreIdx = curStoreIdx;
		for(i=0; i<GOODS_QUANTITY; i++)
		{
			if( GetStoreGoodsType(&Stores[curStoreIdx],i) == TRADE_TYPE_CONTRABAND_NAME )
			{
				if( GetSquadronGoods(_refCharacter,i)>0 )
				{
					_refCharacter.FindContrabandGoods.GoodsIdx = i;
					return i;
				}
			}
			if( GetStoreGoodsType(&Stores[curStoreIdx],i) == TRADE_TYPE_CANNONS_NAME )
			{
				if( GetSquadronGoods(_refCharacter,i)>0 )
				{
					_refCharacter.FindContrabandGoods.GoodsIdx = i;
					return i;
				}
			}
		}
		DeleteAttribute(_refCharacter,"FindContrabandGoods");
		return -1;
	}
	int curLocIdx = FindLocation(_refCharacter.location);
	if(curLocIdx<0) return -1;
	int curIslandIdx = GetIslandIdxByLocationIdx(curLocIdx);
	if(curIslandIdx<0) return -1;
	aref islRef; makearef(islRef,Islands[curIslandIdx].Trade.Contraband);
	int n, iMax;
	iMax   = GetAttributesNum(islRef); //fix
	for(i=0; i<iMax; i++)
	{
		n = sti(GetAttributeValue(GetAttributeN(islRef,0)));
		if( GetSquadronGoods(_refCharacter,n)>0 )
		{
			_refCharacter.FindContrabandGoods.IslandIdx = curIslandIdx;
			_refCharacter.FindContrabandGoods.GoodsIdx = i;
			return n;
		}
	}
	return -1;
}

int FindNextContrabandGoods(ref _refCharacter)
{
	int i,n;
	if(CheckAttribute(_refCharacter,"FindContrabandGoods.StoreIdx"))
	{
		if(CheckAttribute(_refCharacter,"FindContrabandGoods.GoodsIdx"))
		{
			int curStoreIdx = sti(_refCharacter.FindContrabandGoods.StoreIdx);
			if(curStoreIdx>=0)
			{
				for(i=sti(_refCharacter.FindContrabandGoods.GoodsIdx)+1; i<GOODS_QUANTITY; i++)
				{
					if( GetStoreGoodsType(&Stores[curStoreIdx],i) == TRADE_TYPE_CONTRABAND_NAME )
					{
						if( GetSquadronGoods(_refCharacter,i)>0 )
						{
							_refCharacter.FindContrabandGoods.GoodsIdx = i;
							return i;
						}
					}
				}
			}
		}
	}
	else
	{
		if(CheckAttribute(_refCharacter,"FindContrabandGoods.IslandIdx"))
		{
			if(CheckAttribute(_refCharacter,"FindContrabandGoods.GoodsIdx"))
			{
				int curIslandIdx = sti(_refCharacter.FindContrabandGoods.IslandIdx);
				if(curIslandIdx>=0)
				{
					int curGoodsNum = sti(_refCharacter.FindContrabandGoods.GoodsIdx) + 1;
					aref islRef; makearef(islRef,Islands[curIslandIdx].Trade.Contraband);
					for(i=curGoodsNum; i<GetAttributesNum(islRef); i++)      // это мне сомнительно, походу тут баг, но эта ветка не работает, тк магазин есть, тк разговор в таверне идёт
					{
						n = GetAttributeValue(GetAttributeN(islRef,i));
						if( GetSquadronGoods(_refCharacter,n)>0 )
						{
							_refCharacter.FindContrabandGoods.GoodsIdx = i;
							return n;
						}
					}
				}
			}
		}
	}
	DeleteAttribute(_refCharacter,"FindContrabandGoods");
	return -1;
}

int GetCharacterCurrentStore(ref _refCharacter)
{
	bool bOk2 = CheckAttribute(Pchar, "quest.Contraband.Active") && (sti(Pchar.quest.Contraband.Active) == true); // в сделке, значит магазин уже ясен
	if (bOk2 && CheckAttribute(Pchar, "GenQuest.Contraband.StoreIdx"))
	{
		return sti(Pchar.GenQuest.Contraband.StoreIdx);
	}
	int i, idx;
	int curLocNum = FindLocation(_refCharacter.location);
	if(curLocNum<0) return -1;

    idx = -1;
	if (CheckAttribute(&Locations[curLocNum], "fastreload")) // в таверне должно быть всегда
	{
		string locGroupId = Locations[curLocNum].fastreload;
		for (i=0; i<STORE_QUANTITY; i++)
		{
			if (Stores[i].group == locGroupId)
			{
				idx = i;
				break;
			}
		}
	}
	else
	{
        idx = SHIP_STORE; // нет контры там, всегда пошлют, если вдруг нет fastreload в таверне
	}

    Pchar.GenQuest.Contraband.StoreIdx = idx; // запомним, для бухты, когда в сделку пойдем
	return  idx;
}
// to_do
int GetCurrentIslandGoodsPrice(int GoodsType)
{
	int curStoreIdx = GetCharacterCurrentStore(GetMainCharacter());   // вернёт в бухте из запоминалки
	if(curStoreIdx<0) return 0;
	return GetStoreGoodsPrice(&Stores[curStoreIdx],GoodsType,PRICE_TYPE_SELL,GetMainCharacter(), 1);
}
// выбор куда идти
string SelectSmugglingLocation()
{
    ref CurIsland;
	int n;
	string TargetLocation

	n = GetCharacterCurrentIsland(Pchar);
	if (n < 0)
	{
		TargetLocation = "None";
	}
	else
	{
		CurIsland = GetIslandByIndex(n);
		TargetLocation = GetIslandRandomShoreId(CurIsland.id);
    }
    Pchar.Quest.SelectedSmugglingLocation = TargetLocation;
    return TargetLocation;
}

////////////////////////////////////////////////////////////////////////////////
//                Контрабандисты НАЧАЛО
////////////////////////////////////////////////////////////////////////////////
void PlaceSmugglersOnShore(string LocationId)
{
	ref Smuggler, player;
	int RandCounter;
	int i;

	player = GetMainCharacter();

	string Model;

	player.GenQuest.Smugglers_Group = "Smugglers_1";
    LAi_group_Register(player.GenQuest.Smugglers_Group);
	DeleteAttribute(player, "quest.Contraband.ThanksGot"); // новая высадка - благодарность за прошлый бой с патрулём больше не закрывает диалоги
	DeleteAttribute(player, "GenQuest.contraTravel.PatrolBribed"); // и мирный исход прошлого патруля тоже не в счёт
	DeleteAttribute(player, "quest.SmugRevengeKilled"); // вахта возмездия прошлой высадки - id Rand_Smug0X реюзаются, чужой бой не в счёт

	for (i = 1; i <= 3+makeint(MOD_SKILL_ENEMY_RATE*3/2); i++)
    {
        Model = "pirate_" + (rand(9)+1);
		Smuggler = GetCharacter(NPC_GenerateCharacter("Rand_Smug0" + i, Model, "man", "man", 5, PIRATE, 4, true)); // 4 дня, потом сами пропадут
		SetFantomParam(Smuggler);
		LAi_SetWarriorTypeNoGroup(Smuggler);
		LAi_warrior_DialogEnable(Smuggler, true);
		LAi_warrior_SetStay(Smuggler, true);
		Smuggler.Dialog.Filename = "Smuggler_OnShore_dialog.c";
		Smuggler.Dialog.CurrentNode = "First time";
		Smuggler.greeting = "Gr_Smuggler_OnShore";
		LAi_group_MoveCharacter(Smuggler, player.GenQuest.Smugglers_Group);
		if (i < 3) ChangeCharacterAddressGroup(Smuggler, LocationID, "Smugglers", "Smuggler0" + i);
		else
		{
			int rando = rand(3);
			switch (rando)
			{
				case 0: rando = 1; break;
				case 1: rando = 1; break;
				case 2: rando = 2; break;
				case 3: rando = 3; break;
			}
			ChangeCharacterAddressGroup(Smuggler, LocationID, "Smugglers", "Smuggler0"+rando);
		}
    }

	AddGeometryToLocation(LocationID, "smg");

	LAi_group_SetRelation(LAI_GROUP_PLAYER, player.GenQuest.Smugglers_Group, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(player.GenQuest.Smugglers_Group, "CoastalGuards", LAI_GROUP_NEITRAL); // патруль на берегу

	SetTimerCondition("Rand_Smuggling", 0, 0, 4, false);  // через 4 дня всех убрать
	// задание на завал контры
	if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers == "" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(pchar.location)]))
	{
		pchar.quest.KillSmugglers.win_condition.l1 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l1.character = "Rand_Smug01";
		pchar.quest.KillSmugglers.win_condition.l2 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l2.character = "Rand_Smug02";
		pchar.quest.KillSmugglers.win_condition.l3 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l3.character = "Rand_Smug03";
		pchar.quest.KillSmugglers.win_condition = "KillSmugglers_after";
	}
}


void RemoveSmugglersFromShore()
{
	ref Smuggler;
	int RandCounter, i, cn;
	string sLoc;

	DeleteAttribute(pchar, "quest.Contraband.Active");
	// контрабандный ФРАХТ (заказ из журнала торговца): журнал "Сбыт контрабанды" не открывался - закрывать нечего,
	// иначе в логе появляется лишнее "Была закрыта запись (Сбыт контрабанды)". Флаг quest.SmuggleFreight чистится в диалоге ПОСЛЕ этого вызова.
	if (!CheckAttribute(pchar, "quest.SmuggleFreight")) CloseQuestHeader("Gen_Contraband");

	if (CheckAttribute(pchar, "Quest.SelectedSmugglingLocation"))
	{
		sLoc = pchar.Quest.SelectedSmugglingLocation;
	}
//navy --> PGG
	else
	{
		if (CheckAttribute(pchar, "questTemp.PGGContra"))
		{
			sLoc = pchar.questTemp.PGGContra.Loc;
			Smuggler = &Characters[sti(pchar.questTemp.PGGContra)];
			ChangeCharacterAddressGroup(Smuggler, "None", "", "");
			Smuggler.location.from_sea = "";
			Smuggler.PGGAi.location.town = Smuggler.PGGAi.location.town.back;
			DeleteAttribute(pchar, "questTemp.PGGContra");
			DeleteAttribute(Smuggler, "PGGAi.location.town.back");
			CloseQuestHeader("Gen_StrangeInfo");
			pchar.Quest.PGG_WithSmugglers_Dead.Over = "yes";
		}
	}
//navy <--
	RemoveGeometryFromLocation(sLoc, "smg");

	pchar.quest.KillSmugglers_outShore.win_condition.l1 = "ExitFromLocation";
	pchar.quest.KillSmugglers_outShore.win_condition.l1.location = pchar.location;
	pchar.quest.KillSmugglers_outShore.win_condition = "KillSmugglers_outShore";
	for (i=1; i<=3+makeint(MOD_SKILL_ENEMY_RATE*3/2); i++) //eddy
	{
		cn = GetCharacterIndex("Rand_Smug0"+i);
		if (cn != -1)
		{
			characters[cn].lifeDay = 0;
		}
	}
	LAi_group_SetRelation(LAI_GROUP_PLAYER, "CoastalGuards", LAI_GROUP_NEITRAL);
}

void SetCoastalGuardPursuit()
{
	ref Smuggler;
	int i;
	int iNation = sti(pchar.GenQuest.Contraband.GuardNation);// Нация патруля
	string Model;
	if (CheckCharacterPerk(pchar, "UnlimitedContra") && GetCompanionQuantity(pchar) == 1 && sti(RealShips[sti(pchar.ship.type)].Class) > 2)
	{
		Log_TestInfo("Џерк спас от патрулЯ");
	}
	else
	{
		if (CheckAttribute(pchar, "SmugglingFlag")) Flag_Change(PIRATE);
		pchar.ContraInterruptWaiting = true;
		for (i = 1; i <= 3; i++)
		{
			Model = "off_" + NationShortName(iNation) + "_" + (rand(1) + 1);
			Smuggler = GetCharacter(NPC_GenerateCharacter("Coastal_Captain0" + i, Model, "man", "man", 5, iNation, 3, true)); // 3 дня, потом сами пропадут
			SelectCoastalGuardShip(Smuggler);
			SetFantomParamHunter(Smuggler);
			DeleteAttribute(Smuggler,"OZG");
			// 1.2.3 хардкор! Smuggler.Coastal_Captain = true; // если по нему палить, НЗГ не будет
			Smuggler.AlwaysEnemy = true;
			Smuggler.DontRansackCaptain = true;
			Smuggler.AlwaysSandbankManeuver = true;
			Group_addCharacter("Coastal_Guards", Smuggler.id);
			SetCharacterRelationBoth(sti(Smuggler.index), GetMainCharacterIndex(), RELATION_ENEMY);
			if (makeint(Pchar.rank) < 6 && i == 1 && GetCompanionQuantity(Pchar) == 1) break;
			if (makeint(Pchar.rank) < 9 && i == 2 && GetCompanionQuantity(Pchar) < 3) break;
		}
		Group_SetGroupCommander("Coastal_Guards", "Coastal_Captain01");

		Group_SetPursuitGroup("Coastal_Guards", PLAYER_GROUP);
		Group_SetAddress("Coastal_Guards", Islands[GetCharacterCurrentIsland(Pchar)].id, "", "");
		Group_SetTaskAttack("Coastal_Guards", PLAYER_GROUP);
		Group_LockTask("Coastal_Guards");
	}
}


// Морская опасность для КОНТРАБАНДНОГО ФРАХТА (заказ из журнала торговца).
// По ролу: либо патруль нации отправления, либо пираты-охотники за контрабандистами.
// Реюзит группу "Coastal_Guards" и её очистку (StopCoastalGuardPursuit), завершается на MapEnter.
void ArmContraFreightPursuit()
{
	// перк "Заядлый контрабандист": на одиночном малом корабле (класс > 2) уходим без морской погони
	if (CheckCharacterPerk(pchar, "UnlimitedContra") && GetCompanionQuantity(pchar) == 1 && sti(RealShips[sti(pchar.ship.type)].Class) > 2) return;

	ref hunter;
	int i;
	bool bPirate = (rand(1) == 1); // 50/50: патруль или пираты-охотники
	int iNation;
	if (bPirate) iNation = PIRATE;
	else iNation = sti(pchar.GenQuest.Contraband.GuardNation);
	string Model;

	pchar.ContraInterruptWaiting = true;
	for (i = 1; i <= 3; i++)
	{
		if (bPirate) Model = "pirate_" + (rand(9) + 1);
		else         Model = "off_" + NationShortName(iNation) + "_" + (rand(1) + 1);
		hunter = GetCharacter(NPC_GenerateCharacter("Coastal_Captain0" + i, Model, "man", "man", 5, iNation, 3, true)); // 3 дня, потом сами пропадут
		SelectCoastalGuardShip(hunter);
		SetFantomParamHunter(hunter);
		DeleteAttribute(hunter, "OZG");
		hunter.AlwaysEnemy = true;
		hunter.AlwaysSandbankManeuver = true;
		if (bPirate)
		{
			hunter.DontRansackCaptain = false;       // пиратского капитана можно взять на абордаж и поговорить
			if (i == 1) { hunter.ContraHunter = true; hunter.NoOfficerHire = true; }  // особая реплика охотника + запрет найма пленного (LEO) - ТОЛЬКО у флагмана группы
		}
		else
		{
			hunter.DontRansackCaptain = true;
			hunter.ContraPatrol = true; // нац. патруль контрафрахта: квестовых сцен нет - в Speak можно откупиться (Speak_IsTaxOnly)
		}
		Group_addCharacter("Coastal_Guards", hunter.id);
		SetCharacterRelationBoth(sti(hunter.index), GetMainCharacterIndex(), RELATION_ENEMY);
		if (makeint(Pchar.rank) < 6 && i == 1 && GetCompanionQuantity(Pchar) == 1) break;
		if (makeint(Pchar.rank) < 9 && i == 2 && GetCompanionQuantity(Pchar) < 3) break;
	}
	Group_SetGroupCommander("Coastal_Guards", "Coastal_Captain01");
	Group_SetPursuitGroup("Coastal_Guards", PLAYER_GROUP);
	Group_SetAddress("Coastal_Guards", Islands[GetCharacterCurrentIsland(Pchar)].id, "", "");
	Group_SetTaskAttack("Coastal_Guards", PLAYER_GROUP);
	Group_LockTask("Coastal_Guards");
}

// жив ли персонаж с данным id (для шайки кидал и их подмоги)
bool SmugScam_IsAlive(string sId)
{
	if (GetCharacterIndex(sId) == -1) return false;
	if (LAi_isDead(CharacterFromID(sId))) return false;
	return true;
}

// повесить "сдачу последнего выжившего" на всех живых участников шайки (смуглеры + подмога):
// при нуле HP каждый замирает бессмертным на 1 HP и стреляет реакцию SmugScam_LastStand.
// Метка SmugScamLastStand нужна реакции: замороженных ищем по бессмертию+метке, а НЕ по HP -
// смуглер может выпить зелье уже после заморозки (HP подскочит, а бессмертие останется).
void SmugScam_ArmLastStand()
{
	ref m;
	int i;
	int iMax = 3 + makeint(MOD_SKILL_ENEMY_RATE * 3 / 2);
	for (i = 1; i <= iMax; i++)
	{
		if (SmugScam_IsAlive("Rand_Smug0" + i))
		{
			m = CharacterFromID("Rand_Smug0" + i);
			m.SmugScamLastStand = true;
			LAi_SetCheckMinHP(m, 1, true, "SmugScam_LastStand");
		}
		if (SmugScam_IsAlive("SmugScamHelp0" + i))
		{
			m = CharacterFromID("SmugScamHelp0" + i);
			m.SmugScamLastStand = true;
			LAi_SetCheckMinHP(m, 1, true, "SmugScam_LastStand");
		}
	}
}

// Кидалово контрабандного фрахта: смуглеры призывают подмогу. Шанс (50%) отролен ПРИ ВЗЯТИИ
// фрахта (quests_reaction.c trade_quest_open, флаг CargoQuest.SmuggleScamHelp) - не пере-ролить
// ни перезагрузкой, ни сменой дня. Подмога - сильные бойцы, по силе равные скелетам поверхности
// из квеста "Упокоение умерших" (GraveyardQuest.c).
void SmuggleFreightScamHelp()
{
	if (CheckAttribute(pchar, "CargoQuest.SmuggleScamHelp")) // подмога выпала при взятии заказа
	{
		if (LanguageGetLanguage() == "russian") Log_Info("Контрабандисты призвали подмогу!");
		else Log_Info("The smugglers have called for backup!");
		ref sld;
		int iCount = 3 + makeint(MOD_SKILL_ENEMY_RATE * 3 / 2); // как численность самих смуглеров на берегу
		for (int i = 1; i <= iCount; i++)
		{
			sld = GetCharacter(NPC_GenerateCharacter("SmugScamHelp0" + i, "Pirate_" + (rand(24) + 1), "man", "man", sti(pchar.rank) + 5, PIRATE, 1, true));
			FantomMakeCoolFighter(sld, sti(pchar.rank) + 5, 90, 90, LinkRandPhrase(RandPhraseSimple("blade23","blade25"), RandPhraseSimple("blade30","blade26"), RandPhraseSimple("blade24","blade13")), RandPhraseSimple("pistol6","pistol3"), MOD_SKILL_ENEMY_RATE * 3 * 4);
			DeleteAttribute(sld, "SuperShooter");
			if (IsCharacterPerkOn(sld, "Ciras")) SetFantomWearCirass(sld);
			LAi_SetWarriorType(sld);
			LAi_group_MoveCharacter(sld, pchar.GenQuest.Smugglers_Group);
			ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1"); // подмога вбегает со входа в локацию и несётся атаковать
		}
		// группа уже дерётся с ГГ, но новичкам реляцию/задачу обновим явно
		LAi_group_SetRelation(pchar.GenQuest.Smugglers_Group, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
		LAi_group_FightGroups(LAI_GROUP_PLAYER, pchar.GenQuest.Smugglers_Group, true);
	}
	// "последний выживший сдаётся и расплачивается" - во ВСЕХ боях с шайкой, с подмогой и без:
	// каждый бой получает финал, а кидала на коленях возвращает уговор (симметрия исходов)
	DeleteAttribute(pchar, "quest.SmugScamSurrendered");
	SmugScam_ArmLastStand();
}

// ГГ напал на смуглеров ПОСЛЕ "отдал груз без оплаты" (кидалово): шайка тоже зовёт подмогу
void SmuggleFreightScamGiveAttacked()
{
	if (CheckAttribute(pchar, "quest.SmugScamGiveCalled")) return; // одна подмога на сделку
	pchar.quest.SmugScamGiveCalled = true;
	if (!CheckAttribute(pchar, "GenQuest.Smugglers_Group")) return;
	LAi_group_SetRelation(pchar.GenQuest.Smugglers_Group, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups(LAI_GROUP_PLAYER, pchar.GenQuest.Smugglers_Group, true);
	SmuggleFreightScamHelp();
}

// Последний кидала ДОБИТ (после отказа в SmugFreightPre_Kill): снимаем боевую тревогу - врагов не осталось
void SmugScamKillAlarmOff()
{
	if (!CheckAttribute(pchar, "GenQuest.Smugglers_Group")) return;
	LAi_group_SetRelation(pchar.GenQuest.Smugglers_Group, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetAlarm(pchar.GenQuest.Smugglers_Group, LAI_GROUP_PLAYER, 0.0);
	LAi_group_SetAlarm(LAI_GROUP_PLAYER, pchar.GenQuest.Smugglers_Group, 0.0);
	LAi_SetFightMode(pchar, false);     // и сразу в ножны
	LAi_SetFightModeForOfficers(false);
}

// Финализация ПОЩАЖЁННОГО выжившего (вызывается на ВЫХОДЕ из диалога сдачи, через AddDialogExitQuestFunction -
// если делать это в самой ноде, он уходил бродить, не договорив реплику): бессмертен, нем,
// ни на что не реагирует - УБЕГАЕТ из локации через reload и очищается.
void SmugScamSurvivorRetire()
{
	if (!CheckAttribute(pchar, "GenQuest.SmugScamSurvivorId")) return;
	if (GetCharacterIndex(pchar.GenQuest.SmugScamSurvivorId) == -1) return;
	ref m = CharacterFromID(pchar.GenQuest.SmugScamSurvivorId);
	DeleteAttribute(pchar, "GenQuest.SmugScamSurvivorId");
	LAi_SetImmortal(m, true); // не добить в спину на бегу
	SetCharacterRelationBoth(GetMainCharacterIndex(), sti(m.index), RELATION_NEUTRAL);
	DeleteAttribute(m, "dialog"); // без dialog.currentnode разговор не стартует (тихий отказ, без ошибки в лог)
	m.greeting = "";
	// убегает с пляжа и пропадает (штатная идиома ухода NPC: бег до reload-локатора, локация "none" = очистка)
	LAi_SetActorType(m);
	m.lifeDay = 0; // фантом подчистится при ближайшей выгрузке
	LAi_ActorRunToLocation(m, "reload", "reload1", "none", "", "", "", -1.0);
}

// ГГ напал на смуглеров в бухте ДО передачи груза: шайка зовёт подмогу (если выпала при взятии)
// и дерётся; последний выживший предложит завершить сделку (узел SmugFreightPre_Surrender).
void SmuggleFreightPreAttacked()
{
	if (!CheckAttribute(pchar, "quest.SmuggleFreight.active")) return; // передача уже состоялась/отменена - не наш случай
	if (CheckAttribute(pchar, "quest.SmugScamGiveCalled")) return;     // уже отработало
	pchar.quest.SmugScamGiveCalled = true;
	if (!CheckAttribute(pchar, "GenQuest.Smugglers_Group")) return;
	LAi_group_SetRelation(pchar.GenQuest.Smugglers_Group, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups(LAI_GROUP_PLAYER, pchar.GenQuest.Smugglers_Group, true);
	SmuggleFreightScamHelp(); // подмога (если выпала при взятии) + взвод "сдачи последнего" (он там теперь безусловный)
}

// у кого-то из шайки HP упал до нуля (он заморожен бессмертным на 1 HP):
// если он последний живой - бой окончен, сдаётся; если отпился зельем - возвращаем в бой; иначе погибает
void SmugScam_LastStandCheck()
{
	ref m;
	int i;
	int iAlive = 0;
	string sId;
	int iMax = 3 + makeint(MOD_SKILL_ENEMY_RATE * 3 / 2);
	bool bSurrendered = CheckAttribute(pchar, "quest.SmugScamSurrendered"); // сдача уже была - новых замороженных просто добиваем
	for (i = 1; i <= iMax; i++)
	{
		if (SmugScam_IsAlive("Rand_Smug0" + i)) iAlive = iAlive + 1;
		if (SmugScam_IsAlive("SmugScamHelp0" + i)) iAlive = iAlive + 1;
	}
	Log_TestInfo("SmugScam_LastStandCheck: alive=" + iAlive + " surrendered=" + bSurrendered);
	for (i = 1; i <= iMax * 2; i++)
	{
		if (i <= iMax) sId = "Rand_Smug0" + i;
		else sId = "SmugScamHelp0" + (i - iMax);
		if (!SmugScam_IsAlive(sId)) continue;
		m = CharacterFromID(sId);
		if (!CheckAttribute(m, "SmugScamLastStand")) continue; // не взведён (или уже сдался)
		if (!LAi_IsImmortal(m)) continue;                      // ещё не падал до нуля
		// замороженный найден по бессмертию+метке (НЕ по HP - мог отпиться зельем после заморозки)
		if (LAi_GetCharacterHP(m) > 5.01)
		{
			// упал, но успел хлебнуть зелья: возвращаем смертность, перевзводим чекер - пусть дерётся дальше
			Log_TestInfo("SmugScam_LastStandCheck: " + sId + " отпился зельем, перевзвод");
			LAi_SetImmortal(m, false);
			LAi_SetCheckMinHP(m, 1, true, "SmugScam_LastStand");
			continue;
		}
		if (bSurrendered || iAlive > 1)
		{
			// драться ещё есть кому (или сдача уже состоялась) - этот погибает как обычно, бессмертным не виснет
			Log_TestInfo("SmugScam_LastStandCheck: добиваем " + sId);
			LAi_SetImmortal(m, false);
			LAi_KillCharacter(m);
			iAlive = iAlive - 1;
		}
		else
		{
			// последний выживший: прекращаем бой, все сабли в ножны, идёт сдаваться и расплачиваться
			Log_TestInfo("SmugScam_LastStandCheck: последний " + sId + " сдаётся");
			pchar.quest.SmugScamSurrendered = true;
			pchar.GenQuest.SmugScamSurvivorId = m.id; // для финализации после диалога (SmugScamSurvivorRetire)
			DeleteAttribute(m, "SmugScamLastStand"); // сдавшегося последующие проверки не трогают
			LAi_group_SetRelation(pchar.GenQuest.Smugglers_Group, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
			LAi_SetFightMode(pchar, false);     // ГГ - в ножны
			LAi_SetFightModeForOfficers(false); // и офицеры тоже
			LAi_SetImmortal(m, true);           // не дать добить до разговора (он на 1 HP)
			// LEO: снимаем стан ДО смены типа - и рукопашный, и мушкетный. Раньше тут чистился только
			//   stuntime, а сбивают контрабандиста обычно выстрелом: MushketStun запирал боевой режим,
			//   и строка ниже не убирала ему клинок в ножны. Разбор - в LAi_ClearStun (LAi_utils.c).
			LAi_ClearStun(m);
			m.nodisarm = true;                  // и больше НЕ оглушается (штатный гард в LAi_Stunned_StunCharacter) - стан менял тип актора и ломал подход
			LAi_SetActorType(m);
			LAi_SetFightMode(m, false);
			m.actions.walk = "run"; // ПОДБЕГАЕТ к ГГ (follow-задача актора ходит "шагом" - мапим шаг на бег; вернём в ноде диалога)
			m.dialog.filename = "Smuggler_OnShore_dialog.c";
			// сценарий "напал ДО передачи груза" (сделка не состоялась) - своя реплика с предложением завершить сделку
			if (CheckAttribute(pchar, "quest.SmuggleFreight.active")) m.dialog.currentnode = "SmugFreightPre_Surrender";
			else m.dialog.currentnode = "SmugScam_Surrender";
			m.greeting = "";
			LAi_ActorDialog(m, pchar, "", 4.0, 0);
			return;
		}
	}
}

void StopCoastalGuardPursuit()
{
	ref rCap;
	int i, cn;

	for (i = 1; i <= 3; i++)
	{
		cn = GetCharacterIndex("Coastal_Captain0" + i);
		if (cn != -1)
		{
			rCap = GetCharacter(cn);
			if (rCap.location == Pchar.location && bSeaActive) return; // если в море погоня, то не трем, только на выход на карту.
			ChangeCharacterAddressGroup(rCap, "None", "", "");
			setCharacterShipLocation(rCap, "None");
		}
	}
	group_DeleteGroup("Coastal_Guards");
}

// === Возмездие береговых братьев ===
// ГГ откупился/отбрехался от патруля, а потом вырезал смуглеров (случай БЕЗ кидалова - сделка состоялась честно).
// Вешаем вахту на смерть ВСЕХ смуглеров текущей высадки: погибнут все - в море ждёт пиратский патруль возмездия.
// Вахта снимается при новой высадке (PlaceSmugglersOnShore) - id Rand_Smug0X реюзаются, иначе сработает на чужой бой.
void SmugRevenge_ArmDeathWatch()
{
	if (!CheckAttribute(pchar, "GenQuest.Smugglers_Group")) return;
	int i;
	string attrName;
	int iMax = 3 + makeint(MOD_SKILL_ENEMY_RATE * 3 / 2);
	for (i = 1; i <= iMax; i++)
	{
		if (!SmugScam_IsAlive("Rand_Smug0" + i)) return; // кто-то уже мёртв/убран - это не "вырезал всех после откупа"
	}
	for (i = 1; i <= iMax; i++)
	{
		attrName = "l" + i;
		pchar.quest.SmugRevengeKilled.win_condition.(attrName) = "NPC_Death";
		pchar.quest.SmugRevengeKilled.win_condition.(attrName).character = "Rand_Smug0" + i;
	}
	pchar.quest.SmugRevengeKilled.win_condition = "SmugRevenge_AllDead";
}

// Морской патруль возмездия: 3 корабля сильных пиратов высокого ранга ждут на выходе из бухты.
// Своя группа (не "Coastal_Guards") - не пересекается с штатной морской погоней за контрабанду.
// КАЖДЫЙ капитан помечен SmugRevengeHunter: при абордаже сдаётся в каюте и говорит (SetQuestAboardCabinDialog).
// Корабли штатные охотничьи (класс от ранга ГГ), но КАПИТАНЫ - асы: ранг ГГ+15 + рычаги FantomMakeCoolSailor
// (навыки 90, корабельные перки, не тонут при 10%, не выходят из боя). Саму CoolSailor НЕ зовём:
// она перегенерирует корабль и ставит DontRansackCaptain=true - убила бы сдачу в каюте.
void ArmSmugRevengePursuit()
{
	ref hunter;
	int i;
	string Model;
	StopCoastalGuardPursuit(); // заролленная морская погоня (нацпатруль/охотники) отменяется - возмездие занимает её место
	pchar.ContraInterruptWaiting = true; // сон/отдых заблокированы - погоню не пересидеть (трётся при выходе в море)
	for (i = 1; i <= 3; i++)
	{
		Model = "pirate_" + (rand(9) + 1);
		hunter = GetCharacter(NPC_GenerateCharacter("SmugRevenge_Cap0" + i, Model, "man", "man", sti(pchar.rank) + 15, PIRATE, 3, true)); // 3 дня, потом сами пропадут
		SelectCoastalGuardShip(hunter);
		SetFantomParamHunter(hunter);
		DeleteAttribute(hunter, "OZG");
		// капитан-ас: навыки и перки как у FantomMakeCoolSailor (корабль и пушки уже выставлены охотничьим штатом)
		hunter.skill.Sailing  = GetCoffDiffSkill(90, SKILL_MAX);
		hunter.skill.Accuracy = GetCoffDiffSkill(90, SKILL_MAX);
		hunter.skill.Cannons  = GetCoffDiffSkill(90, SKILL_MAX);
		hunter.SinkTenPercent = false;   // не тонет при 10% и не выходит из боя
		hunter.AboardToFinalDeck = true; // абордаж всегда до финальной палубы
		SetCharacterPerk(hunter, "FastReload");
		SetCharacterPerk(hunter, "HullDamageUp");
		SetCharacterPerk(hunter, "SailsDamageUp");
		SetCharacterPerk(hunter, "CrewDamageUp");
		SetCharacterPerk(hunter, "CriticalShoot");
		SetCharacterPerk(hunter, "LongRangeShoot");
		SetCharacterPerk(hunter, "CannonProfessional");
		SetCharacterPerk(hunter, "ShipDefenseProfessional");
		SetCharacterPerk(hunter, "ShipSpeedUp");
		SetCharacterPerk(hunter, "ShipTurnRateUp");
		hunter.AlwaysEnemy = true;
		hunter.AlwaysSandbankManeuver = true;
		hunter.DontRansackCaptain = false;  // капитана можно взять на абордаж и услышать последнее слово
		hunter.SmugRevengeHunter = true;    // реплика мстителя - у ЛЮБОГО капитана эскадры (как у GentlemenHunter)
		hunter.NoOfficerHire = true;        // LEO: пленного мстителя братства нельзя нанять офицером (как ДУ/каперы) - Ransack_captain_dialog гейтит по NoOfficerHire
		Group_addCharacter("SmugRevengeGuards", hunter.id);
		SetCharacterRelationBoth(sti(hunter.index), GetMainCharacterIndex(), RELATION_ENEMY);
	}
	Group_SetGroupCommander("SmugRevengeGuards", "SmugRevenge_Cap01");
	Group_SetPursuitGroup("SmugRevengeGuards", PLAYER_GROUP);
	Group_SetAddress("SmugRevengeGuards", Islands[GetCharacterCurrentIsland(Pchar)].id, "", "");
	Group_SetTaskAttack("SmugRevengeGuards", PLAYER_GROUP);
	Group_LockTask("SmugRevengeGuards");
	// уборка как у штатной погони: по таймеру либо на выходе на глобалку
	SetTimerCondition("SmugRevenge_AtSeaEnded", 0, 0, 2, false);
	pchar.quest.SmugRevengeAtSeaEnded_Map.win_condition.l1 = "MapEnter";
	pchar.quest.SmugRevengeAtSeaEnded_Map.win_condition = "SmugRevenge_AtSeaEnded";
}

void StopSmugRevengePursuit()
{
	ref rCap;
	int i, cn;

	for (i = 1; i <= 3; i++)
	{
		cn = GetCharacterIndex("SmugRevenge_Cap0" + i);
		if (cn != -1)
		{
			rCap = GetCharacter(cn);
			if (rCap.location == Pchar.location && bSeaActive) return; // погоня в море - не трём, уборка на выходе на карту
			ChangeCharacterAddressGroup(rCap, "None", "", "");
			setCharacterShipLocation(rCap, "None");
		}
	}
	group_DeleteGroup("SmugRevengeGuards");
}

void SelectCoastalGuardShip(ref rCharacter)
{
	SetShipHunter(rCharacter);
}

int GetContrabandGoodsPrice(ref _refStore, int _Goods, int _PriceType, ref chref, int _qty)
{
	float _TradeSkill = GetSummonSkillFromNameToOld(chref,SKILL_COMMERCE); // 0..10.0
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	int basePrice = MakeInt(Goods[_Goods].Cost);
	if (!CheckAttribute(_refStore,"Goods."+tmpstr) ) return 0;
	makearef(refGoods,_refStore.Goods.(tmpstr));
 	int tradeType = MakeInt(refGoods.TradeType);
	int Type;
	if (CheckAttribute(refGoods,"Type"))
	{
		Type = MakeInt(refGoods.Type);
	}

	float tradeModify 	= 1.0;
	float priceModify   = 1.0;
	float costModify	= 1.0;

	ref mc = GetMainCharacter();

	switch (tradeType)
	{
		case TRADE_TYPE_NORMAL:
			tradeModify = 1.00 + stf(refGoods.RndPriceModify);
			break;
		case TRADE_TYPE_EXPORT:
			tradeModify = 0.80 + stf(refGoods.RndPriceModify);
			break;
		case TRADE_TYPE_IMPORT:
			tradeModify = 1.20 + stf(refGoods.RndPriceModify);
			break;
		case TRADE_TYPE_CONTRABAND:
			tradeModify = 1.00 + stf(refGoods.RndPriceModify);
			break;
		case TRADE_TYPE_AMMUNITION:
			tradeModify = 1.00 + stf(refGoods.RndPriceModify);
			break;
		case TRADE_TYPE_CANNONS:
			tradeModify = 1.00 + stf(refGoods.RndPriceModify);
			break;
		case TRADE_TYPE_AGGRESSIVE:
			tradeModify = 1.00 + stf(refGoods.RndPriceModify);
		break;
	}

	float skillModify;
	float cModify = 1.0;

	if(_PriceType == PRICE_TYPE_BUY) // цена покупки товара игроком
	{
		skillModify = 1.325 - _TradeSkill * 0.005;
		if(tradeType == TRADE_TYPE_CANNONS) cModify = 2.0 - MOD_SKILL_ENEMY_RATE*3/20.0;
		if(CheckCharacterPerk(chref,"HT2"))
		{
			if(CheckOfficersPerk(chref,"AdvancedCommerce"))	{ skillModify -= 0.20; }
			else
			{
				if(CheckOfficersPerk(chref,"BasicCommerce"))	{ skillModify -= 0.15; }
				else skillModify -= 0.05;
			}
		}
		else
		{
			if(CheckOfficersPerk(chref,"Trader"))	{ skillModify -= 0.05; }
			if(CheckOfficersPerk(chref,"AdvancedCommerce"))	{ skillModify -= 0.20; }
			else
			{
				if(CheckOfficersPerk(chref,"BasicCommerce"))	{ skillModify -= 0.10; }
			}
		}

		costModify = 1.05;
	}
	else	// цена продажи товара игроком
	{
		skillModify = 0.675 + _TradeSkill * 0.005;
		if(tradeType == TRADE_TYPE_CANNONS) cModify = 2.0 - MOD_SKILL_ENEMY_RATE*3/20.0;
		if(CheckCharacterPerk(chref,"HT2"))
		{
			if(CheckOfficersPerk(chref,"AdvancedCommerce"))	skillModify += 0.20;
			else
			{
				if(CheckOfficersPerk(chref,"BasicCommerce"))	{ skillModify += 0.15; }
				else skillModify += 0.05;
			}
		}
		else
		{
			if(CheckOfficersPerk(chref,"Trader"))	{ skillModify += 0.05; }
			if(CheckOfficersPerk(chref,"AdvancedCommerce"))	skillModify += 0.20;
			else
			{
				if(CheckOfficersPerk(chref,"BasicCommerce"))	{ skillModify += 0.10; }
			}
		}
		costModify = 0.85;
	}

	// boal 23.01.2004 -->
	if (MakeInt(basePrice * tradeModify * skillModify * costModify + 0.5) < 1) return 1;
	// boal 23.01.2004 <--

	switch (Type)
	{
		case TRADE_TYPE_NORMAL			:
			priceModify = 1.0;
			break;
		case TRADE_TYPE_AMMUNITION		:
			priceModify = 1.0;
			break;
		case TRADE_TYPE_EXPORT			:
			priceModify = 2.0;
			break;
		case TRADE_TYPE_IMPORT			:
			priceModify = 3.0;
			break;
		case TRADE_TYPE_CANNONS 		:
			priceModify = 1.0;
			break;
	}

    return MakeInt(priceModify * basePrice * tradeModify * skillModify * _qty * costModify * cModify + 0.5);
}

void ContrabandInterruptionSetStatus(bool status) {
	for (i=1; i<=3+makeint(MOD_SKILL_ENEMY_RATE*3/2); i++) {
		ref randSmug = &characters[GetCharacterIndex("Rand_Smug0"+i)];
		if (status) {
			randSmug.ContrabandInterruption = true;
		} else {
			DeleteAttribute(randSmug, "ContrabandInterruption");
		}
	}
}
