// Gregg: ==> квест «Две сестры» - глава 10 сюжетной линии К:ПС «Проклятая судьба».
//
// Кендарт просит найти для Пуэрто-Рико четырёх девиц лёгкого поведения: без борделя
// на острове зреет бунт. Три независимые ветки, сходящиеся в финале:
//   Вики Хендерсон (Мартиника)  - согласна уехать, если убить её сестру Диану;
//   сёстры Лус (Тортуга)        - младшую похитили сатанисты, и это самая длинная нить:
//                                 катакомбы, засада в Белизе, плен на «Церебусе», Мухачи;
//   Натали Ремис (Бермуды)      - её держит при себе пират Малкин.
//
// Отличия от оригинала К:ПС - осознанные, не потери переноса:
//   * плен на «Церебусе» больше НЕ обирает игрока. В К:ПС там выгребали инвентарь,
//     деньги, компаньонов и подменяли корабль; здесь снимается только экипировка
//     (RemoveCharacterEquip в CSP слоты чистит, предметы оставляет), а меню персонажа
//     на время плена закрыто. Вещи при тебе, но связанному ими не воспользоваться;
//   * «Церебус» после захвата - трофей в эскадру, а не подмена корабля игрока.
//     Поэтому не нужны ни Денни Хьюлм с возвратом корабля, ни правка CabinType;
//   * дверь трюма вскрывается штатной мини-игрой CSP (lockpicking), запуск - из
//     разговора ГГ с самим собой в командном меню. Экран CreateKey из К:ПС не переносим;
//   * бессмертных NPC нет там, где игрок может до них дотянуться;
//   * награда Мухачи - винтовальный штуцер вместо развилки «250 тысяч или мушкетон».
//
// Хелперы BO_SetEncounters и BO_SetNPCDeadFunction взяты из blackorchid.c: это общие
// утилиты переноса К:ПС, дублировать их третий раз смысла нет.

#define TS_DIANA_RANK_UP		10		// насколько Диана Хендерсон выше ГГ по рангу
#define TS_DIANA_HP				100.0	// сколько здоровья теряет Диана между двумя сходками
#define TS_SKEL_QTY				15		// скелетов в катакомбах Тортуги (в К:ПС было 25 в двух точках)
#define TS_SKEL_RANK_DOWN		10		// насколько скелеты слабее ГГ
#define TS_ROOM_SLEEP_DAYS		3		// сколько ГГ отлёживается после драки в доме сестёр
#define TS_HOLD_DAYS			1		// сколько «Церебус» идёт до первой стоянки
#define TS_HOLD_WAIT_DAYS		3		// сколько ГГ сидит в трюме до второй стоянки
#define TS_PATROL_RANK_UP		5		// насколько патруль анимистов выше ГГ
#define TS_MUHACHI_QTY			14		// индейцев в лагере Мухачи
#define TS_ANIMIST_QTY			19		// сатанистов в лагере
#define TS_KRENO_MONEY			150000	// сколько при себе держал Мерил Крено
#define TS_NEXT_DAYS			45		// от конца главы 10 до следующей части линии

// Gregg: запись в журнал одной строкой - имя квеста повторяется двадцать раз.
// AddQuestUserData цепляется к ПОСЛЕДНЕЙ добавленной записи (quests.c:404 берёт
// GetAttributesNum(...)-1), а не к квесту целиком, поэтому род ГГ подставляем после
// КАЖДОЙ записи. Один вызов при старте оставлял в поздних записях дырку вместо слова.
// sSex - обычное окончание, sSex1 - возвратная форма, sSex2 - слово целиком.
void TS_Record(string sNum)
{
	AddQuestRecord("TwoSisters", sNum);
	AddQuestUserData("TwoSisters", "sSex", GetSexPhrase("", "а"));
	AddQuestUserData("TwoSisters", "sSex1", GetSexPhrase("ся", "ась"));
	AddQuestUserData("TwoSisters", "sSex2", GetSexPhrase("нашёл", "нашла"));
}

// Gregg: «Церебус» - именной бриг, а не первый попавшийся из генератора. Правки идут
// по ЭКЗЕМПЛЯРУ в RealShips (GenerateShip возвращает индекс копии, CreateBaseShip
// клонирует базовый тип), поэтому SHIP_BRIG и все прочие бриги в игре не меняются.
// Атрибут QuestShip намеренно не ставим: по нему экран корабля лезет за видеокартинкой
// SHIP_Brig (ship.c:605), а описание корпуса становится «неопознанным»
// (interface_utils.c:2143). Уникальность держится на характеристиках и улучшениях.
void TS_MakeCerebus(ref chr)
{
	chr.Ship.Type = GenerateShip(SHIP_BRIG, 0);
	chr.Ship.Name = iTxt("TS_CerebusName");

	ref rShip = GetRealShip(sti(chr.Ship.Type));
	rShip.HP			= 3600;		// базовый бриг - 3000
	rShip.SpeedRate		= 17.6;		// 16.8
	rShip.TurnRate		= 21.0;		// 19.0
	rShip.MaxCaliber	= 24;		// 20, на класс выше обычного брига
	rShip.MaxCrew		= 200;		// 180, трюм под пленников
	rShip.Price			= 160000;	// 125000

	// Три готовых улучшения - все слоты неквестового корпуса заняты, снять их игрок может.
	CheckForShipStatsChange(rShip, "SUP_HULLKRENG");
	rShip.Upgrade.SUP_HULLKRENG = true;
	CheckForShipStatsChange(rShip, "SUP_REINFORCEDCONSTRUCTION");
	rShip.Upgrade.SUP_REINFORCEDCONSTRUCTION = true;
	CheckForShipStatsChange(rShip, "SUP_SAIL");
	rShip.Upgrade.SUP_SAIL = true;

	SetBaseShipData(chr);
}

// Gregg: снять с ГГ экипировку и запомнить, что было надето. Предметы остаются
// в инвентаре: RemoveCharacterEquip в CSP чистит только слот (CharacterUtilite.c:2605).
void TS_StripEquip()
{
	string sGroup;
	string sItem;
	int iN;
	for (iN = 0; iN < 5; iN++)
	{
		switch (iN)
		{
			case 0: sGroup = GUN_ITEM_TYPE; break;
			case 1: sGroup = BLADE_ITEM_TYPE; break;
			case 2: sGroup = SPYGLASS_ITEM_TYPE; break;
			case 3: sGroup = CIRASS_ITEM_TYPE; break;
			case 4: sGroup = BACKPACK_ITEM_TYPE; break;
		}
		sItem = GetCharacterEquipByGroup(PChar, sGroup);
		if (sItem != "")
		{
			PChar.TwoSisters.Equip.(sGroup) = sItem;
		}
		RemoveCharacterEquip(PChar, sGroup);
	}
}

// Gregg: вернуть то, что сняли. Предмет мог уйти из инвентаря по ходу дела, поэтому
// перед экипировкой проверяем, что он вообще на руках.
void TS_ReturnEquip()
{
	if (!CheckAttribute(PChar, "TwoSisters.Equip")) return;

	string sGroup;
	string sItem;
	string sKey;
	int iN;
	for (iN = 0; iN < 5; iN++)
	{
		switch (iN)
		{
			case 0: sGroup = GUN_ITEM_TYPE; break;
			case 1: sGroup = BLADE_ITEM_TYPE; break;
			case 2: sGroup = SPYGLASS_ITEM_TYPE; break;
			case 3: sGroup = CIRASS_ITEM_TYPE; break;
			case 4: sGroup = BACKPACK_ITEM_TYPE; break;
		}
		sKey = "TwoSisters.Equip." + sGroup;
		if (!CheckAttribute(PChar, sKey)) continue;
		sItem = PChar.TwoSisters.Equip.(sGroup);
		if (GetCharacterItem(PChar, sItem) > 0)
		{
			EquipCharacterByItem(PChar, sItem);
		}
	}
	DeleteAttribute(PChar, "TwoSisters.Equip");
}

// Gregg: конец главы 9 взводит этот таймер - см. FindAndDestroyEnd в blackorchid.c.
void TwoSistersTimeEnd(string qName)
{
	PChar.TwoSisters = "1";

	ref chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Kendart.c";
	chr.Dialog.CurrentNode = "First time";
}

void TwoSistersStart()
{
	PChar.TwoSisters = "2";

	SetQuestHeader("TwoSisters");
	TS_Record("1");

	ref chr;
	string sID;
	string sModel;
	string sName;
	string sLastName;
	string sLocation;
	string sLocator;
	string sLocatorGroup;
	string sGroup;
	int iN;

	for (iN = 0; iN < 2; iN++)
	{
		switch (iN)
		{
			case 0:
				sID = "Viki Henderson";
				sModel = "horse02";
				sName = "Вики";
				sLastName = "Хендерсон";
				sLocation = "FortFrance_Brothel";
				sLocator = "quest1";
				sLocatorGroup = "quest";
				sGroup = "FRANCE_CITIZENS";
			break;

			case 1:
				sID = "Natali Remis";
				sModel = "horse04";
				sName = "Натали";
				sLastName = "Ремис";
				sLocation = "Pirates_town";
				sLocator = "goto1";
				sLocatorGroup = "goto";
				sGroup = "PIRATE_CITIZENS";
			break;
		}

		chr = GetCharacter(NPC_GenerateCharacter(sID, sModel, "woman", "towngirl", 1, PIRATE, -1, false));
		chr.rank = 1;
		chr.name = sName;
		chr.lastname = sLastName;
		if (iN == 0)
		{
			chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Viki.c";
			chr.Dialog.CurrentNode = "1";
		}
		else
		{
			// Gregg: у Натали и Малкина общий файл, и узел «1» принадлежит ему.
			// До сцены на улице у неё свой нейтральный вход.
			chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Natali.c";
			chr.Dialog.CurrentNode = "First time";
		}
		chr.greeting = "whore";
		// Gregg: девицы стоят в людном месте и в квесте не дерутся ни разу - бессмертие
		// тут не «неуязвимый противник», а защита сюжетного NPC от шальной драки в городе.
		LAi_SetImmortal(chr, true);
		chr.NotRemoved = true;
		LAi_SetCitizenType(chr);
		LAi_group_MoveCharacter(chr, sGroup);
		ChangeCharacterAddressGroup(chr, sLocation, sLocatorGroup, sLocator);
	}

	SetQuestFunction("TwoSistersLusQuestStart", "TwoSistersLusQuestStart", "l", "Tortuga_Brothel");
	SetQuestFunction("TwoSistersRemisStart", "TwoSistersRemisStart", "l", "Pirates_town");
}

//////////////////////////////////////////////////////////////////////////////////////
// Ветка Вики Хендерсон: Мартиника - Пуэрто-Принсипе
//////////////////////////////////////////////////////////////////////////////////////
void TwoSistersVikiQuestStart()
{
	TS_Record("2");

	BO_SetEncounters("PuertoPrincipe_ExitTown", true);
	SetQuestFunction("TwoSistersVikiToPuertoPrincipe", "TwoSistersVikiToPuertoPrincipe", "l", "PuertoPrincipe_ExitTown");
}

void TwoSistersVikiToPuertoPrincipe(string qName)
{
	chrDisableReloadToLocation = true;

	ChangeCharacterAddressGroup(PChar, "PuertoPrincipe_ExitTown", "quest", "quest1");
	LAi_SetActorType(PChar);

	DoQuestFunctionDelay("TwoSistersVikiInPuertoPrincipe", 0.1);
}

void TwoSistersVikiInPuertoPrincipe(string _tmp)
{
	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, 6.5, 2.0, 4.5, false);

	ref chr;

	chr = GetCharacter(NPC_GenerateCharacter("Diana Henderson Killer", "pirate_10", "man", "man", 1, PIRATE, 0, true));
	ChangeCharacterAddressGroup(chr, "PuertoPrincipe_ExitTown", "quest", "quest2");
	LAi_SetHP(chr, 50.0, 50.0);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	int iRank = GetRank(PChar, 0) + TS_DIANA_RANK_UP;
	// Gregg: у модели Danielle свой набор анимаций (Danielle / Danielle_fast / _swim),
	// CSP её так и заводит - см. MagicCity.c. С общей woman теряется бег и плавание.
	chr = GetCharacter(NPC_GenerateCharacter("Diana Henderson", "Danielle", "woman", "Danielle", iRank, PIRATE, 0, true));
	chr.name = "Диана";
	chr.lastname = "Хендерсон";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Viki.c";
	chr.Dialog.CurrentNode = "17";
	chr.greeting = "Gr_Danielle";
	LAi_NPC_Equip(chr, iRank, true, false);
	FantomMakeCoolFighter(chr, iRank, 50, 50, "blade7", "", 100);
	// Gregg: в К:ПС Диана до разговора была бессмертной. У нас она просто в группе игрока
	// и в драку с наёмником не лезет - убить её раньше времени игроку нечем и незачем.
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	ChangeCharacterAddressGroup(chr, "PuertoPrincipe_ExitTown", "quest", "quest3");
	LAi_ActorTurnToCharacter(PChar, chr);

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.TwoSistersVikiTalkWithDiana.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersVikiTalkWithDiana.win_condition.l1.character = "Diana Henderson Killer";
	PChar.quest.TwoSistersVikiTalkWithDiana.function = "TwoSistersVikiTalkWithDiana";

	// Gregg: Диана смертна с самого начала, и в свалке её может задеть сам игрок.
	// Раз бессмертия нет, у любого исхода должен быть выход: кольцо снимается с тела,
	// чем бы бой ни кончился. Условие висит с этой минуты и до конца ветки.
	PChar.quest.TwoSistersVikiNextFightWithDianaWin.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersVikiNextFightWithDianaWin.win_condition.l1.character = "Diana Henderson";
	PChar.quest.TwoSistersVikiNextFightWithDianaWin.function = "TwoSistersVikiNextFightWithDianaWin";
}

void TwoSistersVikiTalkWithDiana(string qName)
{
	locCameraFollow();

	LAi_Fade("", "");
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("Diana Henderson");
	SetDialogCharacter2Character(PChar, chr, 20.0, false);
}

// Первая сходка: Диана уступает, но не умирает - её останавливает гейт по здоровью.
void TwoSistersVikiFirstFightWithDiana()
{
	ref chr = CharacterFromID("Diana Henderson");

	SetFoodToCharacter(chr, 2, 15);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	LAi_SetCheckMinHP(chr, 1.0, true, "TwoSistersVikiFirstFightWithDianaWin");
	PChar.quest.TwoSistersVikiFirstFightWithDianaWin.function = "TwoSistersVikiFirstFightWithDianaWin";
}

void TwoSistersVikiFirstFightWithDianaWin(string qName)
{
	ref chr = CharacterFromID("Diana Henderson");
	chr.Dialog.CurrentNode = "28";
	SetDialogCharacter2Character(PChar, chr, 1.0, true);
	LAi_RemoveCheckMinHP(chr);
	LAi_UseAtidoteBottle(chr);
}

// Вторая сходка - насмерть.
void TwoSistersVikiNextFightWithDiana()
{
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("Diana Henderson");

	float fHp = LAi_GetCharacterMaxHP(chr) - TS_DIANA_HP;
	if (fHp < TS_DIANA_HP) { fHp = TS_DIANA_HP; }

	LAi_UseAtidoteBottle(chr);
	LAi_SetHP(chr, fHp, fHp);
	LAi_SetImmortal(chr, false);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.TwoSistersVikiNextFightWithDianaWin.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersVikiNextFightWithDianaWin.win_condition.l1.character = "Diana Henderson";
	PChar.quest.TwoSistersVikiNextFightWithDianaWin.function = "TwoSistersVikiNextFightWithDianaWin";
}

void TwoSistersVikiNextFightWithDianaWin(string qName)
{
	// Gregg: сюда приходят оба пути - и штатный второй бой, и случайный удар в свалке
	// с наёмником. Гейт первой сходки снимаем, иначе он останется висеть на трупе.
	ref chrDiana = CharacterFromID("Diana Henderson");
	LAi_RemoveCheckMinHP(chrDiana);
	DeleteAttribute(PChar, "quest.TwoSistersVikiFirstFightWithDianaWin");
	DeleteAttribute(PChar, "quest.TwoSistersVikiTalkWithDiana");

	chrDisableReloadToLocation = false;
	LAi_SetPlayerType(PChar);
	TakeNItems(PChar, "TwoSisters_Ring", 1);

	BO_SetEncounters("PuertoPrincipe_ExitTown", false);
	TS_Record("3");

	ref chr = CharacterFromID("Viki Henderson");
	chr.Dialog.CurrentNode = "33";
}

void TwoSistersVikiQuestComplete()
{
	chrDisableReloadToLocation = false;
	bDisableCharacterMenu = false;

	TS_Record("4");

	PChar.TwoSisters.Viki = true;
	TwoSistersCheckComplete();

	ref chr = CharacterFromID("Viki Henderson");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
// Ветка сестёр Лус: Тортуга
//////////////////////////////////////////////////////////////////////////////////////
void TwoSistersLusQuestStart(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Rebekka Lus", "horse06", "woman", "towngirl", 1, PIRATE, -1, false));
	chr.name = "Ребекка";
	chr.lastname = "Лус";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Rebekka.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Enc_RapersGirl_1";
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;
	LAi_SetCitizenType(chr);
	LAi_group_MoveCharacter(chr, "FRANCE_CITIZENS");
	ChangeCharacterAddressGroup(chr, "Tortuga_Brothel", "quest", "quest1");

	SetDialogCharacter2Character(PChar, chr, 15.0, false);
}

void TwoSistersLusQuestToDungeon()
{
	chrDisableReloadToLocation = false;
	// Gregg: задание от Ребекки в журнал попадало только после боя в катакомбах -
	// игрок выходил из борделя без единой строчки о том, куда идти.
	TS_Record("23");

	ref chr = CharacterFromID("Rebekka Lus");
	LAi_SetCitizenType(chr);

	ref jew = ItemsFromID("TwoSisters_Medallion");
	jew.shown = true;
	jew.startLocation = "Tortuga_Cave";
	jew.startLocator = "medallion";

	Locations[FindLocation("Tortuga_Cave")].locators_radius.item.medallion = 2.0;
	SetQuestFunction("TwoSistersLusQuestInDungeon", "TwoSistersLusQuestInDungeon", "l", "Tortuga_Cave");
}

// Gregg: в К:ПС все двадцать пять скелетов вставали на два локатора - monster20 и
// monster30, то есть двумя грудами тел в одной точке. Раскидываем по настоящим точкам
// подземелья, а число режем: в CSP бой тяжелее, и двадцать пять - это уже не «нелегко»,
// а стена. Штатную генерацию монстров на время сцены глушим, иначе к нашим добавятся
// местные и подземелье превратится в свалку.
void TwoSistersLusQuestInDungeon(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_LocationDisableMonstersGen("Tortuga_Cave", true);

	string sLocator;
	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) - TS_SKEL_RANK_DOWN;
	if (iRank < 1) { iRank = 1; }

	ref chr;
	int iN;
	for (iN = 1; iN <= TS_SKEL_QTY; iN++)
	{
		switch (iN)
		{
			case 1: sLocator = "monster5"; break;
			case 2: sLocator = "monster6"; break;
			case 3: sLocator = "monster8"; break;
			case 4: sLocator = "monster9"; break;
			case 5: sLocator = "monster10"; break;
			case 6: sLocator = "monster11"; break;
			case 7: sLocator = "monster13"; break;
			case 8: sLocator = "monster15"; break;
			case 9: sLocator = "monster17"; break;
			case 10: sLocator = "monster18"; break;
			case 11: sLocator = "monster19"; break;
			case 12: sLocator = "monster20"; break;
			case 13: sLocator = "monster22"; break;
			case 14: sLocator = "monster24"; break;
			case 15: sLocator = "monster25"; break;
		}

		chr = GetCharacter(NPC_GenerateCharacter("TwoSistersLusQuest Skel " + iN, "Skel" + (rand(2) + 1), "skeleton", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "Tortuga_Cave", "monsters", sLocator);
		LAi_NPC_Equip(chr, iRank, true, true);
		SetFoodToCharacter(chr, 5, 20);

		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("TwoSistersLusQuestSkeletonsDead", "TwoSistersLusQuestSkeletonsDead", "TwoSistersLusQuest Skel ", TS_SKEL_QTY);
}

void TwoSistersLusQuestSkeletonsDead(string qName)
{
	LAi_LocationDisableMonstersGen("Tortuga_Cave", false);
	TS_Record("5");

	// Gregg: ловим ЛОКАТОР медальона, а не появление вещи в инвентаре: условие "item"
	// опрашивается только на прогоне QuestsCheck и срабатывает с большим опозданием.
	PChar.quest.TwoSistersLusQuestFindMedallion.win_condition.l1 = "locator";
	PChar.quest.TwoSistersLusQuestFindMedallion.win_condition.l1.location = "Tortuga_Cave";
	PChar.quest.TwoSistersLusQuestFindMedallion.win_condition.l1.locator_group = "item";
	PChar.quest.TwoSistersLusQuestFindMedallion.win_condition.l1.locator = "medallion";
	PChar.quest.TwoSistersLusQuestFindMedallion.function = "TwoSistersLusQuestFindMedallion";
}

void TwoSistersLusQuestFindMedallion(string qName)
{
	chrDisableReloadToLocation = false;
	TS_Record("6");

	ref jew = ItemsFromID("TwoSisters_Medallion");
	jew.startLocation = "";
	jew.startLocator = "";

	Locations[FindLocation("Tortuga_Cave")].locators_radius.item.medallion = 0.1;

	ref chr = CharacterFromID("Rebekka Lus");
	chr.Dialog.CurrentNode = "15";
}

void TwoSistersLusQuestToTavernRoom()
{
	TS_Record("7");

	ref chr = CharacterFromID("Rebekka Lus");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	SetQuestFunction("TwoSistersLusQuestInTavernRoom", "TwoSistersLusQuestInTavernRoom", "l", "Tortuga_tavern_upstairs");
}

void TwoSistersLusQuestInTavernRoom(string qName)
{
	chrDisableReloadToLocation = false;

	bDisableCharacterMenu = true;
	bDisableQuestInterface = true;

	LAi_SetActorType(PChar);
	DoQuestFunctionDelay("TwoSistersLusQuestSetFirstForm", 3.0);
}

void TwoSistersLusQuestSetFirstForm(string _tmp)
{
	// Gregg: в К:ПС продолжение вешалось на квест-имя таблички и ловилось кейсом
	// в свитче quests_reaction.c. В CSP для этого есть штатная пара Run_Function
	// плюс SetLaunchFrameRunFunctionParam - свитч трогать не нужно.
	SetLaunchFrameFormParam(iTxt("TS_ThreeDaysSleep"), "Run_Function", 3.0, 3.0);
	SetLaunchFrameRunFunctionParam("TwoSistersLusDialogInRoom", 1.0);
	LaunchFrameForm();
}

void TwoSistersLusDialogInRoom()
{
	WaitDate("", 0, 0, TS_ROOM_SLEEP_DAYS, 0, 0);
	SetCurrentTime(13, 10);
	RecalculateJumpTable();
	RefreshWeather();
	RefreshLandTime();

	ref chr = CharacterFromID("Rebekka Lus");
	chr.greeting = "Gr_Woman_Citizen";
	ChangeCharacterAddressGroup(chr, "Tortuga_tavern_upstairs", "reload", "reload1_back");

	chr.Dialog.CurrentNode = "24";
	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

// Gregg: дверь дома сестёр в CSP уже есть - houseS1 ведёт в типовой CommonCobHouse.
// Перецепляем её на квестовый дом и в финале вернём обратно. Слот тот же, что и
// в оригинале К:ПС, так что дом стоит на своём месте: за таверной, вверх по улице.
void TwoSistersLusToTortugaTown()
{
	LAi_SetPlayerType(PChar);
	chrDisableReloadToLocation = false;

	bDisableCharacterMenu = false;
	bDisableQuestInterface = false;

	ref chr = CharacterFromID("Rebekka Lus");
	ChangeCharacterAddressGroup(chr, "Tortuga_town", "reload", "reload4");

	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	ref loc = LocFromID("Tortuga_town");
	loc.reload.l22.go = "HouseSistersLus";
	loc.reload.l22.emerge = "reload1";
	loc.reload.l22.label = "House Sisters Lus";
	loc.reload.l22.disable = 1;

	Locations[FindLocation("Tortuga_town")].locators_radius.reload.houseS1 = 3.0;
	SetQuestFunction("TwoSistersLusInTortugaTown", "TwoSistersLusInTortugaTown", "l", "Tortuga_town");
}

void TwoSistersLusInTortugaTown(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Rebekka Lus");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocator(chr, "goto", "goto6", "", -1);

	string sModel;
	string sAni;
	string sSex;
	string sLocator;
	int iN;
	for (iN = 1; iN <= 7; iN++)
	{
		switch (iN)
		{
			case 1:
				sModel = "citiz_11";
				sSex = "man";
			break;

			case 2:
				sModel = "barmen_5";
				sSex = "man";
			break;

			case 3:
				sModel = "shipowner_8";
				sSex = "man";
			break;

			case 4:
				sModel = "girl_2";
				sSex = "woman";
			break;

			case 5:
				sModel = "girl_8";
				sSex = "woman";
			break;

			case 6:
				sModel = "citiz_5";
				sSex = "man";
			break;

			case 7:
				sModel = "citiz_3";
				sSex = "man";
			break;
		}
		sAni = "man";
		if (sSex == "woman") { sAni = "towngirl"; }
		sLocator = "people" + iN;

		chr = GetCharacter(NPC_GenerateCharacter("People At Rebekka Lus House " + iN, sModel, sSex, sAni, 1, PIRATE, -1, false));
		chr.greeting = "Gr_Crazy";
		chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Rebekka.c";
		chr.Dialog.CurrentNode = "28";
		ChangeCharacterAddressGroup(chr, "Tortuga_town", "quest", sLocator);
		LAi_SetActorType(chr);
		LAi_ActorTurnToLocator(chr, "reload", "houseS1");
	}

	PChar.TwoSisters.CanCitizenTurn = true;

	// Gregg: SetQuestLocatorFunction из К:ПС в CSP нет, условие пишем явно.
	PChar.quest.TwoSistersLusInTortugaTownTalk.win_condition.l1 = "locator";
	PChar.quest.TwoSistersLusInTortugaTownTalk.win_condition.l1.location = "Tortuga_town";
	PChar.quest.TwoSistersLusInTortugaTownTalk.win_condition.l1.locator_group = "reload";
	PChar.quest.TwoSistersLusInTortugaTownTalk.win_condition.l1.locator = "houseS1";
	PChar.quest.TwoSistersLusInTortugaTownTalk.function = "TwoSistersLusInTortugaTownTalk";
}

// Gregg: зевак разворачивает к двери на каждой загрузке города - иначе после
// возвращения в локацию толпа стоит спиной к дому. Зовётся из LAi_LocationLoaded.
void TwoSistersLusCitizensTurnToDoor(ref loc)
{
	if (!CheckAttribute(loc, "id")) return;
	if (!CheckAttribute(PChar, "TwoSisters.CanCitizenTurn")) return;
	if (sti(PChar.TwoSisters.CanCitizenTurn) == 0) return;
	if (loc.id != "Tortuga_town") return;

	string sCharacter;
	ref chr;
	int iN;
	for (iN = 1; iN <= 8; iN++)
	{
		sCharacter = "People At Rebekka Lus House " + iN;
		if (iN == 8) { sCharacter = "Rebekka Lus"; }

		if (GetCharacterIndex(sCharacter) == -1) continue;

		chr = CharacterFromID(sCharacter);
		if (sCharacter == "Rebekka Lus")
		{
			if (chr.location.locator == "reload4") continue;
		}

		LAi_SetActorType(chr);
		LAi_ActorTurnToLocator(chr, "reload", "houseS1");
	}
}

void TwoSistersLusInTortugaTownTalk(string qName)
{
	Locations[FindLocation("Tortuga_town")].locators_radius.reload.houseS1 = 1.0;

	ref chr = CharacterFromID("People At Rebekka Lus House 3");
	SetDialogCharacter2Character(PChar, chr, 10.0, true);

	chr = CharacterFromID("Rebekka Lus");
	ChangeCharacterAddressGroup(chr, "Tortuga_town", "goto", "goto6");
	LAi_type_actor_Reset(chr);
	LAi_ActorTurnToLocator(chr, "reload", "houseS1");
}

void TwoSistersLusQuestToHouse()
{
	chrDisableReloadToLocation = false;
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("People At Rebekka Lus House 3");
	LAi_SetActorType(chr);
	LAi_ActorGoToLocator(chr, "quest", "people3", "", -1);
	LAi_ActorTurnToLocator(chr, "reload", "houseS1");

	ref loc = LocFromID("Tortuga_town");
	loc.reload.l22.disable = 0;

	SetQuestFunction("TwoSistersLusQuestInHouse", "TwoSistersLusQuestInHouse", "l", "HouseSistersLus");
}

void TwoSistersLusQuestInHouse(string qName)
{
	ref chr = CharacterFromID("Rebekka Lus");
	ChangeCharacterAddress(chr, "None", "");

	PChar.TwoSisters.CanCitizenTurn = false;

	string sID;
	int iN;
	for (iN = 1; iN <= 7; iN++)
	{
		sID = "People At Rebekka Lus House " + iN;
		chr = CharacterFromID(sID);
		chr.LifeDay = 0;
	}

	SetQuestFunction("TwoSistersLusQuestToRoom", "TwoSistersLusQuestToRoom", "l", "HouseSistersLusRoom");
}

void TwoSistersLusQuestToRoom(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr;
	string sModel;
	string sLocator;
	string sGroup;
	int iN;
	int iB;
	for (iN = 1; iN <= 3; iN++)
	{
		switch (iN)
		{
			case 1:
				sModel = "Animists2";
				sLocator = "bar2";
				sGroup = "barmen";
			break;

			case 2:
				sModel = "officer_18";
				sLocator = "bar1";
				sGroup = "barmen";
			break;

			case 3:
				sModel = "officer_6";
				sLocator = "goto1";
				sGroup = "goto";
			break;
		}

		// Gregg: анимация man2 в CSP отсутствует - везде, где К:ПС ставил её, берём man.
		chr = GetCharacter(NPC_GenerateCharacter("Man In Rebekka Lus House " + iN, sModel, "man", "man", 15, PIRATE, 0, true));
		chr.greeting = "Gr_questOfficer";
		chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Rebekka.c";
		chr.Dialog.CurrentNode = "30";
		ChangeCharacterAddressGroup(chr, "HouseSistersLusRoom", sGroup, sLocator);
		LAi_SetImmortal(chr, true);

		if (iN == 3)
		{
			chr.name = "Ханно";
			chr.lastname = "Гроот";
			SetDialogCharacter2Character(PChar, chr, 2.5, false);
		}
		else
		{
			for (iB = 0; iB <= 4; iB++)
			{
				LaunchBlood(chr, 7.0, true);
			}
			LAi_SetActorType(chr);
			LAi_ActorSetLayMode(chr);
		}
	}

	ChangeCharacterAddressGroup(PChar, "HouseSistersLusRoom", "barmen", "stay");
}

void TwoSistersLusQuestToRoomSetAnimist()
{
	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, 3.0, 1.5, 3.0, false);

	ref chr = GetCharacter(NPC_GenerateCharacter("Animist in Lus House", "Animists2", "man", "man", 25, PIRATE, 0, true));
	FantomMakeCoolFighter(chr, 35, 100, 100, "", "", 200);
	LAi_NPC_Equip(chr, sti(chr.rank), true, true);
	SetFoodToCharacter(chr, 5, 30);
	ChangeCharacterAddressGroup(chr, "HouseSistersLusRoom", "reload", "reload1");

	LAi_SetActorType(chr);
	LAi_ActorTurnToCharacter(chr, PChar);
}

// Gregg: сцену ГГ проигрывает - так задумано в оригинале, дальше он приходит в себя
// через три дня. Но бессмертным сатаниста НЕ делаем: если игрок его всё-таки уложит,
// это должно что-то значить, а не упираться в стену. Гейт стоит на самом ГГ.
void TwoSistersLusQuestRoomFightWithAnimist()
{
	LAi_SetPlayerType(PChar);
	locCameraFollow();

	ref chr = CharacterFromID("Man In Rebekka Lus House 3");
	LAi_SetImmortal(chr, false);
	LAi_SetHP(chr, 50.0, 50.0);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	chr = CharacterFromID("Animist in Lus House");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	// Gregg: если сатаниста всё-таки убили - сцена всё равно кончается обмороком,
	// но по своей ветке: тело исчезает, и это ровно та загадка, которую потом
	// подтверждает Мухачи.
	PChar.quest.TwoSistersLusQuestAnimistDead.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersLusQuestAnimistDead.win_condition.l1.character = "Animist in Lus House";
	PChar.quest.TwoSistersLusQuestAnimistDead.function = "TwoSistersLusQuestAnimistDead";

	PChar.quest.TwoSistersLusQuestRoomFightWithAnimistLoose.function = "TwoSistersLusQuestRoomFightWithAnimistLoose";
	LAi_SetImmortal(PChar, false);
	LAi_SetCheckMinHP(PChar, 5.0, true, "TwoSistersLusQuestRoomFightWithAnimistLoose");
}

void TwoSistersLusQuestAnimistDead(string qName)
{
	PChar.TwoSisters.AnimistKilled = true;
	DeleteAttribute(PChar, "quest.TwoSistersLusQuestRoomFightWithAnimistLoose");
	LAi_RemoveCheckMinHP(PChar);
	// Gregg: удар в спину от второго - тот самый, после которого ГГ падает. Именно
	// SetHP, а НЕ ApplyCharacterDamage: гейт по здоровью только что снят, и настоящий
	// урон в упор увёл бы игрока в Game Over вместо сюжетного обморока.
	LAi_SetHP(PChar, 5.0, LAi_GetCharacterMaxHP(PChar));
	LaunchBlood(PChar, 7.0, true);
	DoQuestFunctionDelay("TwoSistersLusQuestRoomFightWithAnimistLoose", 0.5);
}

void TwoSistersLusQuestRoomFightWithAnimistLoose(string qName)
{
	LAi_SetActorType(PChar);
	LAi_ActorAnimation(PChar, "death_" + rand(2), "", -1);

	DoQuestFunctionDelay("TwoSistersLusQuestRoomFightLooseFall", 1.5);
}

void TwoSistersLusQuestRoomFightLooseFall(string _tmp)
{
	LAi_Fade("", "");

	LAi_type_actor_Reset(PChar);
	LAi_ActorSetLayMode(PChar);

	DoQuestFunctionDelay("TwoSistersLusQuestRoomReload", 2.5);
}

void TwoSistersLusQuestRoomReload(string _tmp)
{
	bDisableCharacterMenu = true;
	bDisableQuestInterface = true;

	WaitDate("", 0, 0, TS_ROOM_SLEEP_DAYS, 0, 0);
	SetCurrentTime(6, 30);
	RecalculateJumpTable();

	SetLaunchFrameFormParam(iTxt("TS_ComeToSenses"), "Run_Function", 0.1, 3.0);
	SetLaunchFrameRunFunctionParam("TwoSistersLusQuestRoomReloadStart", 1.0);
	LaunchFrameForm();
}

void TwoSistersLusQuestRoomReloadStart()
{
	DoFunctionReloadToLocation("HouseSistersLus", "goto", "goto4", "TwoSistersLusQuestRoomReloadEnd");
}

void TwoSistersLusQuestRoomReloadEnd()
{
	bDisableCharacterMenu = false;
	bDisableQuestInterface = false;

	LAi_RemoveCheckMinHP(PChar);
	LAi_UseAtidoteBottle(PChar);
	LAi_SetImmortal(PChar, false);
	LAi_SetCurHPMax(PChar);
	LAi_SetPlayerType(PChar);

	// Gregg: сатанисты ушли, но медальон остался на полу - улика, с которой ГГ едет
	// к Кендарту.
	TakeNItems(PChar, "TwoSisters_AnimistMedallion", 1);

	ref chr = CharacterFromID("Rebekka Lus");
	ChangeCharacterAddressGroup(chr, "HouseSistersLus", "reload", "reload1");

	chr.Dialog.CurrentNode = "34";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void TwoSistersLusQuestNeedMedallionInfo()
{
	chrDisableReloadToLocation = false;

	PChar.TwoSisters = "3";
	TS_Record("8");
	// Gregg: постоянное здоровье в CSP идёт по шкале до 60 (AddCharacterMaxHealth
	// режет потолок), отрастает по +1 за день отдыха. Штраф за пять дней в беспамятстве
	// держим на уровне тяжёлой драки: в моде максимум встречается -30, обычная цена -10.
	AddCharacterHealth(PChar, -10);

	ref chr = CharacterFromID("Rebekka Lus");
	ChangeCharacterAddressGroup(chr, "HouseSistersLus", "goto", "goto1");
	LAi_SetCitizenType(chr);
}

//////////////////////////////////////////////////////////////////////////////////////
// Белиз: Мерил Крено и ловушка в бухте Четумаль
//////////////////////////////////////////////////////////////////////////////////////
// Gregg: дверь houseSp1 в CSP ведёт в типовой дом; на время квеста перецепляем её
// на дом Крено, как это делал оригинал со своим слотом.
void TwoSistersLusQuestToBeliz()
{
	PChar.TwoSisters = "4";
	TS_Record("9");

	ref loc = LocFromID("Beliz_town");
	loc.reload.l25.go = "HouseMerilKreno";
	loc.reload.l25.emerge = "reload1";
	loc.reload.l25.label = "House Meril Kreno";
	loc.reload.l25.disable = 0;

	SetQuestFunction("TwoSistersLusQuestInBeliz", "TwoSistersLusQuestInBeliz", "l", "HouseMerilKreno");
}

void TwoSistersLusQuestInBeliz(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Meril Kreno", "usurer_3", "man", "man", 30, PIRATE, -1, false));
	FantomMakeCoolFighter(chr, 30, 100, 100, "blade37", "pistol5", 200);
	chr.name = "Мерил";
	chr.lastname = "Крено";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Rebekka.c";
	chr.Dialog.CurrentNode = "56";
	chr.greeting = "Gr_QuestMan";
	// Gregg: в своём доме Крено недосягаем - разговор начинается сразу и кончается
	// уходом в бухту. Бессмертие снимается на берегу, где до него уже можно дотянуться.
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;

	ChangeCharacterAddressGroup(chr, "HouseMerilKreno", "barmen", "bar1");
	SetDialogCharacter2Character(PChar, chr, 15.0, false);
}

void TwoSistersLusQuestSetCasanderToKrenoHouse()
{
	ref chr = GetCharacter(NPC_GenerateCharacter("Captain Casander", "officer_20", "man", "man", 1, PIRATE, -1, false));
	chr.greeting = "Gr_QuestMan";
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;

	LAi_SetActorType(chr);
	ChangeCharacterAddressGroup(chr, "HouseMerilKreno", "reload", "reload1");
}

void TwoSistersLusQuestToBelizShore()
{
	chrDisableReloadToLocation = false;
	// Gregg: сцена «Касандер входит в дом» держала камеру на нём - отпускаем.
	locCameraTarget(PChar);
	locCameraFollow();

	ref chr = CharacterFromID("Meril Kreno");
	LAi_SetStayType(chr);

	chr = CharacterFromID("Captain Casander");
	TS_MakeCerebus(chr);
	SetGeraldDetails(chr, 2, 2, "", "pir_" + (rand(4) + 1));
	SetCharacterShipLocation(chr, "Shore8");

	LAi_SetActorType(chr);
	LAi_ActorGoToLocator(chr, "goto", "goto1", "", -1);

	ref loc = LocFromID("Beliz_town");
	loc.reload.l25.disable = 1;

	TS_Record("10");
	BO_SetEncounters("Shore8", true);
	SetQuestFunction("TwoSistersLusQuestInBelizShore", "TwoSistersLusQuestInBelizShore", "l", "Shore8");
}

void TwoSistersLusQuestInBelizShore(string qName)
{
	chrDisableReloadToLocation = true;

	// Gregg: дом Крено больше не нужен - дверь возвращаем типовому интерьеру сразу,
	// чтобы не тащить перецепленный слот через весь остаток квеста.
	ref loc = LocFromID("Beliz_town");
	loc.reload.l25.go = "CommonDoubleflourHouse_2";
	loc.reload.l25.emerge = "reload1";
	loc.reload.l25.label = "House";
	DeleteAttribute(loc, "reload.l25.disable");

	string sID;
	ref chr;
	int iN;
	for (iN = 1; iN <= 2; iN++)
	{
		switch (iN)
		{
			case 1: sID = "Meril Kreno"; break;
			case 2: sID = "Captain Casander"; break;
		}

		chr = CharacterFromID(sID);
		chr.Dialog.CurrentNode = "67";
		ChangeCharacterAddressGroup(chr, "Shore8", "quest", "quest" + iN);

		if (iN == 2)
		{
			LAi_NPC_Equip(chr, 30, true, true);
			FantomMakeCoolFighter(chr, 30, 100, 100, "blade21", "pistol6", 200);
		}

		LAi_SetActorType(chr);
		LAi_ActorTurnToLocator(chr, "goto", "goto7");

		if (iN == 1)
		{
			LAi_SetStayType(chr);
		}
	}
}

void TwoSistersLusQuestCasanderShotPrepare()
{
	bDisableCharacterMenu = true;
	bDisableQuestInterface = true;

	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, 5.5, 3.5, 51.5, false);

	ref chr = CharacterFromID("Captain Casander");
	LAi_SetActorType(chr);

	LAi_SetActorType(PChar);
	DoQuestFunctionDelay("TwoSistersLusQuestCasanderShot", 1.0);
}

void TwoSistersLusQuestCasanderShot(string _tmp)
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Captain Casander");
	LAi_ActorTurnToCharacter(chr, PChar);
	LAi_ActorAnimation(chr, "Shot", "", -1);

	DoQuestFunctionDelay("TwoSistersLusQuestCasanderShotComplete", 1.5);
}

void TwoSistersLusQuestCasanderShotComplete(string _tmp)
{
	ref chr = CharacterFromID("Captain Casander");
	LAi_SetStayType(chr);

	LAi_ApplyCharacterDamage(PChar, 1);
	LaunchBlood(PChar, 7.0, true);

	float fHp = LAi_GetCharacterMaxHP(PChar);
	LAi_SetHP(PChar, 5.0, fHp);

	DoQuestFunctionDelay("TwoSistersLusQuestCasanderShotEnd", 0.5);
}

void TwoSistersLusQuestCasanderShotEnd(string _tmp)
{
	LAi_type_actor_Reset(PChar);
	LAi_ActorSetLayMode(PChar);

	DoQuestFunctionDelay("TwoSistersLusQuestToHoldPrepare", 2.5);
}

void TwoSistersLusQuestToHoldPrepare(string _tmp)
{
	WaitDate("", 0, 0, TS_HOLD_DAYS, 0, 0);
	SetCurrentTime(1, 30);
	RecalculateJumpTable();

	ref chr = CharacterFromID("Captain Casander");
	ChangeCharacterAddress(chr, "None", "");

	chr = CharacterFromID("Meril Kreno");
	ChangeCharacterAddress(chr, "None", "");

	SetLaunchFrameFormParam(iTxt("TS_WhereAmI"), "Run_Function", 0.1, 3.0);
	SetLaunchFrameRunFunctionParam("TwoSistersLusQuestToHold", 1.0);
	LaunchFrameForm();
}

// Gregg: ключевая развилка с оригиналом. В К:ПС здесь выгребали ВЕСЬ инвентарь, деньги
// и компаньонов, а корабль подменяли на SHIP_NOTUSED. Ничего этого нет: снимаем только
// экипировку (слоты, не предметы) и закрываем меню персонажа. Корабль остаётся стоять
// в бухте Четумаль - команда ждёт капитана, как и в оригинале ждала.
void TwoSistersLusQuestToHold()
{
	TS_StripEquip();

	SetCharacterShipLocation(PChar, "Shore8");
	// карта: экран откроется, но телепорт откажет - иначе игрок сбежит из плена по метке
	PChar.MapBestTeleportDisable = true;
	bQuestDisableMapEnter = true;

	// свита в трюме пленника взяться ниоткуда не может
	if (!CheckAttribute(PChar, "TwoSisters.NoOfficers"))
	{
		PChar.TwoSisters.NoOfficers = true;
		DeleteAllOfficersFromLocation();
		bDisableOfficers = true;
	}

	SetQuestBoxItems("Cerebus_Hold", "TwoSisters_Hairpin", 1);
	DoFunctionReloadToLocation("Cerebus_Hold", "goto", "goto1", "TwoSistersLusQuestToHoldComplete");
}

void TwoSistersLusQuestToHoldComplete()
{
	BO_SetEncounters("Shore8", false);
	// Gregg: chrDisableReloadToLocation здесь НЕ ставим. Дверь трюма и так заперта
	// своим disable, а флаг заодно гасит в командном меню пункт «поговорить с собой»
	// (LandInterface.c:2059) - то есть запасной вход во взлом.
	bDisableQuestInterface = false;

	LAi_SetImmortal(PChar, false);
	LAi_SetCurHPMax(PChar);
	LAi_SetGroundSitType(PChar);

	ref chr = CharacterFromID("Meril Kreno");
	chr.greeting = "Gr_UnpleasantPerson";
	chr.Dialog.CurrentNode = "68";
	ChangeCharacterAddressGroup(chr, "Cerebus_Hold", "reload", "reload1");
	SetDialogCharacter2Character(PChar, chr, 20.0, false);
}

void TwoSistersLusQuestMainHeroStandUpPrepare()
{
	ref chr = CharacterFromID("Meril Kreno");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "", -1);

	DoQuestFunctionDelay("TwoSistersLusQuestMainHeroStandUp", 10.0);
}

void TwoSistersLusQuestMainHeroStandUp(string _tmp)
{
	LAi_Fade("", "");
	LAi_SetPlayerType(PChar);

	// Gregg: пока ГГ в плену, меню персонажа закрыто, а с ним и журнал - подсказки
	// идут на экран логом. В журнал плен ляжет одной записью, когда дверь откроется.
	Log_Info(iTxt("TS_LogHold"));

	// Gregg: шпильку ловим по локатору ящика, а не по появлению вещи в инвентаре -
	// условие "item" опрашивается редко и срабатывает с заметной задержкой.
	PChar.quest.TwoSistersLusQuestFindHairpin.win_condition.l1 = "item";
	PChar.quest.TwoSistersLusQuestFindHairpin.win_condition.l1.item = "TwoSisters_Hairpin";
	PChar.quest.TwoSistersLusQuestFindHairpin.function = "TwoSistersLusQuestFindHairpin";
}

void TwoSistersLusQuestFindHairpin(string qName)
{
	ClearQuestBoxStay("Cerebus_Hold");
	Log_Info(iTxt("TS_LogHairpin"));

	PChar.quest.TwoSistersLusQuestToHoldDoor.win_condition.l1 = "locator";
	PChar.quest.TwoSistersLusQuestToHoldDoor.win_condition.l1.location = "Cerebus_Hold";
	PChar.quest.TwoSistersLusQuestToHoldDoor.win_condition.l1.locator_group = "reload";
	PChar.quest.TwoSistersLusQuestToHoldDoor.win_condition.l1.locator = "reload1";
	PChar.quest.TwoSistersLusQuestToHoldDoor.function = "TwoSistersLusQuestToHoldDoor";
}

void TwoSistersLusQuestToHoldDoor(string qName)
{
	DoQuestFunctionDelay("TwoSistersLusQuestWaitTimeInHold", 5.0);
}

void TwoSistersLusQuestWaitTimeInHold(string _tmp)
{
	SetLaunchFrameFormParam(iTxt("TS_DaysInHold"), "Run_Function", 3.0, 3.0);
	SetLaunchFrameRunFunctionParam("TwoSistersLusQuestWaitTimeInHoldEnd", 1.0);
	LaunchFrameForm();
}

void TwoSistersLusQuestWaitTimeInHoldEnd()
{
	WaitDate("", 0, 0, TS_HOLD_WAIT_DAYS, 0, 0);
	SetCurrentTime(8, 20);
	RecalculateJumpTable();
	RefreshWeather();
	RefreshLandTime();

	LAi_SetGroundSitType(PChar);

	ref chr = CharacterFromID("Meril Kreno");
	chr.Dialog.CurrentNode = "89";
	ChangeCharacterAddressGroup(chr, "Cerebus_Hold", "reload", "reload1");
	SetDialogCharacter2Character(PChar, chr, 20.0, false);
}

void TwoSistersLusQuestKrenoExitFromShip()
{
	ref chr = CharacterFromID("Meril Kreno");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "TwoSistersLusQuestCanPickDoor", -1);
	PChar.quest.TwoSistersLusQuestCanPickDoor.function = "TwoSistersLusQuestCanPickDoor";
}

// Gregg: вместо экрана CreateKey из К:ПС - штатная мини-игра взлома CSP. Разрешаем
// ветку в разговоре ГГ с самим собой (командное меню, «поговорить с собой»), оттуда
// и запускается lockpicking. Меню персонажа при этом закрыто, экран предметов
// недоступен, так что запуск через вещь в инвентаре тут не годится.
void TwoSistersLusQuestCanPickDoor(string qName)
{
	LAi_SetPlayerType(PChar);
	Log_Info(iTxt("TS_LogPickDoor"));
	PChar.TwoSisters.CanPickDoor = true;

	TS_ArmPickDoor();
}

// Gregg: основной вход во взлом - подход к самой двери со шпилькой в руках. Ветка
// «поговорить с собой» осталась запасной: экран предметов в плену закрыт, и щёлкнуть
// шпилькой из инвентаря нельзя, а к двери игрок подходит сам.
void TS_ArmPickDoor()
{
	PChar.quest.TwoSistersLusQuestPickDoorAtLocator.win_condition.l1 = "locator";
	PChar.quest.TwoSistersLusQuestPickDoorAtLocator.win_condition.l1.location = "Cerebus_Hold";
	PChar.quest.TwoSistersLusQuestPickDoorAtLocator.win_condition.l1.locator_group = "reload";
	PChar.quest.TwoSistersLusQuestPickDoorAtLocator.win_condition.l1.locator = "reload1";
	PChar.quest.TwoSistersLusQuestPickDoorAtLocator.function = "TwoSistersLusQuestPickDoorAtLocator";
}

void TwoSistersLusQuestPickDoorAtLocator(string qName)
{
	if (!CheckAttribute(PChar, "TwoSisters.CanPickDoor")) return;
	// Gregg: без шпильки к двери можно подходить сколько угодно - просто перевзводим
	// условие и ждём, пока игрок обыщет ящик.
	if (GetCharacterItem(PChar, "TwoSisters_Hairpin") < 1)
	{
		DoQuestFunctionDelay("TS_RearmPickDoor", 2.0);
		return;
	}
	TwoSistersLusQuestPickDoorStart();
}

// Перевзвод условия: срабатывает и после отказа от взлома, и когда шпильки ещё нет.
// Пять секунд - чтобы экран не открывался повторно, пока игрок стоит у двери.
void TS_RearmPickDoor(string _tmp)
{
	if (!CheckAttribute(PChar, "TwoSisters.CanPickDoor")) return;
	TS_ArmPickDoor();
}

// Зовётся из разговора ГГ с самим собой (MainHero_dialog.c, узел TwoSisters_PickDoor).
// Запускать мини-игру прямо из диалога нельзя: procEnableInterfaceLaunch отказывает,
// пока DialogRun не сброшен, поэтому уходим через отложенный вызов.
void TwoSistersLusQuestPickDoorStart()
{
	PChar.questTemp.lockpicking.ID = "TwoSisters_HoldDoor";
	DoQuestFunctionDelay("TwoSistersLusQuestPickDoorLaunch", 0.5);
}

void TwoSistersLusQuestPickDoorLaunch(string _tmp)
{
	LaunchLockPicking();
	// Gregg: если игрок закроет экран, не вскрыв замок, условие на двери должно
	// ожить заново - иначе второй попытки не будет вовсе. Пауза нужна, чтобы окно
	// не открылось повторно прямо под ногами у стоящего вплотную игрока.
	DoQuestFunctionDelay("TS_RearmPickDoor", 5.0);
}

// Зовётся из lockpicking.c, когда замок поддался.
void TwoSistersLusQuestDoorOpened()
{
	DeleteAttribute(PChar, "TwoSisters.CanPickDoor");
	// Gregg: снимаем условие на двери руками - иначе отложенный TS_RearmPickDoor
	// повесит его снова уже после того, как замок сдался.
	DeleteAttribute(PChar, "quest.TwoSistersLusQuestPickDoorAtLocator");
	TakeNItems(PChar, "TwoSisters_Hairpin", -1);

	chrDisableReloadToLocation = false;
	bDisableCharacterMenu = false;
	TS_ReturnEquip();

	ref loc = LocFromID("Cerebus_Hold");
	DeleteAttribute(loc, "reload.l1.disable");

	// Gregg: сабля лежит в ящике кают-компании - в К:ПС она была на столе, на локаторе
	// dagger, которого в модели CSP нет.
	SetQuestBoxItems("Cerebus_WardRoom", "blade7", 1);
	TS_Record("11");

	SetQuestFunction("TwoSistersLusQuestToCamp", "TwoSistersLusQuestToCamp", "l", "Cerebus_WardRoom");
}

void TwoSistersLusQuestToCamp(string qName)
{
	SetQuestFunction("TwoSistersLusQuestInCannonDeck", "TwoSistersLusQuestInCannonDeck", "l", "Cerebus_CannonDeck");
}

// Gregg: в CSP-модели орудийной палубы группы goto нет вовсе - только rld и box.
// Патруль ставим на абордажные точки loc3 и loc5, они для того и размечены.
void TwoSistersLusQuestInCannonDeck(string qName)
{
	chrDisableReloadToLocation = true;
	ClearQuestBoxStay("Cerebus_WardRoom");

	ref chr;
	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + TS_PATROL_RANK_UP;
	string sLocator;
	int iN;
	for (iN = 1; iN <= 2; iN++)
	{
		sLocator = "loc3";
		if (iN == 2) { sLocator = "loc5"; }

		chr = GetCharacter(NPC_GenerateCharacter("Cerebus CannonDeck Patrol " + iN, "Animists2", "man", "man", iRank, PIRATE, 0, true));
		ChangeCharacterAddressGroup(chr, "Cerebus_CannonDeck", "rld", sLocator);
		LAi_NPC_Equip(chr, iRank, true, true);
		SetFoodToCharacter(chr, 2, 20);
		LAi_SetWarriorType(chr);
	}

	// Gregg: в К:ПС патруль ждал 7,5 секунды и только потом бросался - на экране это
	// читается как зависшие болванчики. Сразу отдаём их в бой, как обычную охрану.
	TwoSistersLusQuestFightInCannonDeck("");
}

void TwoSistersLusQuestFightInCannonDeck(string _tmp)
{
	ref chr;
	string sCharacter;
	int iN;
	for (iN = 1; iN <= 2; iN++)
	{
		sCharacter = "Cerebus CannonDeck Patrol " + iN;
		chr = CharacterFromID(sCharacter);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("TwoSistersLusQuestFightInCannonDeckWin", "TwoSistersLusQuestFightInCannonDeckWin", "Cerebus CannonDeck Patrol ", 2);
}

void TwoSistersLusQuestFightInCannonDeckWin(string qName)
{
	chrDisableReloadToLocation = false;
	SetQuestFunction("TwoSistersLusQuestToCerebusCabin", "TwoSistersLusQuestToCerebusCabin", "l", "Cerebus_Cabin");
}

void TwoSistersLusQuestToCerebusCabin(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Captain Casander");
	ChangeCharacterAddressGroup(chr, "Cerebus_Cabin", "rld", "loc0");
	chr.LifeDay = 0;
	chr.SaveItemsForDead = true;
	SetCharacterShipLocation(chr, "None");
	FantomMakeCoolFighter(chr, 30, 100, 100, "blade21", "pistol6", 200);
	LAi_NPC_Equip(chr, 30, true, true);
	LAi_SetImmortal(chr, false);
	DeleteAttribute(chr, "NotRemoved");
	SetFoodToCharacter(chr, 2, 25);

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.TwoSistersLusQuestFightWithCasanderWin.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersLusQuestFightWithCasanderWin.win_condition.l1.character = "Captain Casander";
	PChar.quest.TwoSistersLusQuestFightWithCasanderWin.function = "TwoSistersLusQuestFightWithCasanderWin";
}

// Gregg: корабль игрока не подменяем. «Церебус» стоит у берега Мухачи, туда же квест
// переставляет и собственное судно ГГ - команда пришла за капитаном. Сам бриг достанется
// призом позже, из разговора с уцелевшим на берегу.
void TwoSistersLusQuestFightWithCasanderWin(string qName)
{
	chrDisableReloadToLocation = false;

	ref loc = LocFromID("Cerebus_Cabin");
	loc.reload.l1.go = "Muhachi_Shore";
	loc.reload.l1.emerge = "sea";
	loc.reload.l1.label = "Shore";

	SetCharacterShipLocation(PChar, "Muhachi_Shore");
	SetQuestFunction("TwoSistersLusQuestToMuhachiShore", "TwoSistersLusQuestToMuhachiShore", "l", "Muhachi_Shore");
}

//////////////////////////////////////////////////////////////////////////////////////
// Берег Мухачи: индейцы, сатанисты и Мишель
//////////////////////////////////////////////////////////////////////////////////////
void TwoSistersLusQuestToMuhachiShore(string qName)
{
	// свита возвращается: плен кончился
	if (CheckAttribute(PChar, "TwoSisters.NoOfficers"))
	{
		DeleteAttribute(PChar, "TwoSisters.NoOfficers");
		bDisableOfficers = false;
	}
	DeleteAttribute(PChar, "MapBestTeleportDisable");
	bQuestDisableMapEnter = false;

	// Gregg: запираем шлюпку здесь, а не только в ините: локация могла приехать
	// в сейв раньше правки, и тогда выход в море остаётся открытым - игрок уходит
	// в залив Гибели без дороги назад, бросив Мишель в джунглях.
	ref locShore = LocFromID("Muhachi_Shore");
	locShore.reload.l2.disable = 1;

	TS_Record("15");
	SetQuestFunction("TwoSistersLusQuestToMuhachiJungle", "TwoSistersLusQuestToMuhachiJungle", "l", "Muhachi_Jungle_03");
}

void TwoSistersLusQuestToMuhachiJungle(string qName)
{
	chrDisableReloadToLocation = true;

	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + TS_PATROL_RANK_UP;
	ref chr;

	string sLocator;
	string sModel = "indsair2";
	string sLastModel = "";
	int iN;
	for (iN = 1; iN <= TS_MUHACHI_QTY; iN++)
	{
		if (iN == 1)
		{
			sModel = "mummy";
		}
		else
		{
			if (sLastModel == "indsair2") { sModel = "indsar1"; }
			if (sLastModel != "indsair2") { sModel = "indsair2"; }
			sLastModel = sModel;
		}

		sLocator = "citizen0" + iN;

		chr = GetCharacter(NPC_GenerateCharacter("Muhachi " + iN, sModel, "man", "man", iRank, PIRATE, 0, false));
		chr.name = "Индеец";
		chr.lastname = "";
		chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Rebekka.c";
		chr.Dialog.CurrentNode = "First time";
		ChangeCharacterAddressGroup(chr, "Muhachi_Jungle_03", "goto", sLocator);

		LAi_NPC_Equip(chr, iRank, true, true);
		LAi_SetCitizenType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

		if (iN == 1)
		{
			chr.name = "Мухачи";
			chr.Dialog.CurrentNode = "90";
			// Gregg: вождь недосягаем ровно до конца боя - он стоит в кругу своих,
			// и до разговора игроку до него не добраться.
			LAi_SetImmortal(chr, true);
			chr.NotRemoved = true;
			SetDialogCharacter2Character(PChar, chr, 45.0, false);
		}
	}
}

void TwoSistersLusQuestMessageIndsar()
{
	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + 2;
	ref chr = GetCharacter(NPC_GenerateCharacter("Muhachi 15", "indsar1", "man", "man", iRank, PIRATE, 0, true));
	chr.name = "Индеец";
	chr.lastname = "";
	ChangeCharacterAddressGroup(chr, "Muhachi_Jungle_03", "reload", "reload2_back");

	LAi_SetActorType(chr);
	LAi_ActorFollow(chr, CharacterFromID("Muhachi 1"), "", 60.0);
}

void TwoSistersLusQuestFightInJungle()
{
	// Gregg: в разговоре с Мухачи камера уходила на подбежавшего индейца - возвращаем.
	locCameraTarget(PChar);
	locCameraFollow();

	ref chr;
	string sCharacter;
	int iN;

	for (iN = 1; iN <= TS_MUHACHI_QTY + 1; iN++)
	{
		sCharacter = "Muhachi " + iN;
		if (GetCharacterIndex(sCharacter) == -1) continue;
		chr = CharacterFromID(sCharacter);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	}

	chr = GetCharacter(NPC_GenerateCharacter("Mishel Lus", "horse08", "woman", "towngirl", 1, PIRATE, -1, false));
	chr.rank = 1;
	chr.name = "Мишель";
	chr.lastname = "Лус";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Rebekka.c";
	chr.Dialog.CurrentNode = "120";
	chr.greeting = "Enc_RapersGirl_1";
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;
	ChangeCharacterAddressGroup(chr, "Muhachi_Jungle_03", "reload", "reload2_back");
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	LAi_SetActorType(chr);
	LAi_ActorAfraid(chr, PChar, true);

	// медальон анимиста получает второе описание - Мухачи объяснил, как он работает
	ref jew = ItemsFromID("TwoSisters_AnimistMedallion");
	jew.describe = "itmdescr_TwoSisters_AnimistMedallion2";

	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + TS_PATROL_RANK_UP;
	string sLocator;
	for (iN = 1; iN <= TS_ANIMIST_QTY + 1; iN++)
	{
		sLocator = "reload2_back";
		if (iN >= 10) { sLocator = "reload1_back"; }

		if (iN == 1)
		{
			chr = CharacterFromID("Meril Kreno");
			ChangeCharacterAddressGroup(chr, "Muhachi_Jungle_03", "reload", sLocator);
			chr.LifeDay = 0;
			chr.SaveItemsForDead = true;
			FantomMakeCoolFighter(chr, 30, 100, 100, "blade37", "pistol6", 300);
			LAi_NPC_Equip(chr, 35, true, true);
			LAi_SetImmortal(chr, false);
			DeleteAttribute(chr, "NotRemoved");
			SetFoodToCharacter(chr, 2, 15);
			chr.money = TS_KRENO_MONEY;
			LAi_SetWarriorType(chr);
			LAi_group_MoveCharacter(chr, "EnemyFight");
			continue;
		}

		chr = GetCharacter(NPC_GenerateCharacter("Muhachi Enemy " + (iN - 1), "Animists2", "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "Muhachi_Jungle_03", "reload", sLocator);
		chr.SaveItemsForDead = true;
		SetFoodToCharacter(chr, 2, 15);
		LAi_NPC_Equip(chr, iRank, true, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("TwoSistersLusQuestFightInJungleWin", "TwoSistersLusQuestFightInJungleWin", "Muhachi Enemy ", TS_ANIMIST_QTY);
}

void TwoSistersLusQuestFightInJungleWin(string qName)
{
	PChar.quest.TwoSistersLusQuestMerilKrenoDead.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersLusQuestMerilKrenoDead.win_condition.l1.character = "Meril Kreno";
	PChar.quest.TwoSistersLusQuestMerilKrenoDead.function = "TwoSistersLusQuestMerilKrenoDead";
}

void TwoSistersLusQuestMerilKrenoDead(string qName)
{
	ref chr = CharacterFromID("Muhachi 1");
	chr.Dialog.CurrentNode = "109";
	SetDialogCharacter2Character(PChar, chr, 45.0, false);
}

void TwoSistersLusQuestRemoveAllAmulets()
{
	TakeNItems(PChar, "TwoSisters_MuhachiAmulet", -1);

	int iQty = GetCharacterItem(PChar, "TwoSisters_AnimistMedallion");
	if (iQty > 0)
	{
		TakeNItems(PChar, "TwoSisters_AnimistMedallion", -iQty);
	}
}

void TwoSistersLusQuestWaitInJungle()
{
	ref chr;
	string sCharacter;
	int iN;

	for (iN = 1; iN <= TS_MUHACHI_QTY + 1; iN++)
	{
		sCharacter = "Muhachi " + iN;
		if (GetCharacterIndex(sCharacter) == -1) continue;

		chr = CharacterFromID(sCharacter);
		LAi_SetActorType(chr);

		if (sCharacter == "Muhachi 1")
		{
			LAi_ActorRunToLocation(chr, "reload", "reload2_back", "none", "", "", "TwoSistersLusQuestTalkWithMishel", -1);
			PChar.quest.TwoSistersLusQuestTalkWithMishel.function = "TwoSistersLusQuestTalkWithMishel";
		}
		else
		{
			LAi_ActorRunToLocation(chr, "reload", "reload2_back", "none", "", "", "", -1);
		}
	}
}

void TwoSistersLusQuestTalkWithMishel(string qName)
{
	LAi_Fade("", "");
	ref chr;
	string sCharacter;
	int iN;

	for (iN = 1; iN <= TS_MUHACHI_QTY + 1; iN++)
	{
		sCharacter = "Muhachi " + iN;
		if (GetCharacterIndex(sCharacter) == -1) continue;
		chr = CharacterFromID(sCharacter);
		DeleteAttribute(chr, "NotRemoved");
		ChangeCharacterAddress(chr, "None", "");
	}

	chr = CharacterFromID("Mishel Lus");
	LAi_type_actor_Reset(chr);
	SetDialogCharacter2Character(PChar, chr, 45.0, false);
}

void TwoSistersLusQuestReturnToCerebus()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Mishel Lus");
	LAi_type_actor_Reset(chr);
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	// Gregg: с берега шлюпка временно идёт не в море, а на «Церебус» - там разговор
	// с Мишель. Обратно в море переход вернётся в TwoSistersLusQuestReturnToBeliz.
	ref loc = LocFromID("Muhachi_Shore");
	loc.reload.l2.go = "Cerebus_Cabin";
	loc.reload.l2.emerge = "reload1";
	loc.reload.l2.label = "Ship";
	if (CheckAttribute(loc, "reload.l2.disable")) DeleteAttribute(loc, "reload.l2.disable");

	SetQuestFunction("TwoSistersLusQuestTalkWithMishelInCabin", "TwoSistersLusQuestTalkWithMishelInCabin", "l", "Cerebus_Cabin");
}

void TwoSistersLusQuestTalkWithMishelInCabin(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Mishel Lus");
	ChangeCharacterAddressGroup(chr, "Cerebus_Cabin", "rld", "aloc0");
	LAi_type_actor_Reset(chr);

	chr.Dialog.CurrentNode = "121";
	SetDialogCharacter2Character(PChar, chr, 15.0, false);
}

// Gregg: «Церебус» игроку не выдаётся подменой корабля. Бриг приводит в Белиз Терри
// Лорто - канонир, которого Касандер держал на борту гребцом; он же и предлагает
// решить судьбу трофея.
void TwoSistersLusQuestReturnToBeliz()
{
	ref chr = CharacterFromID("Mishel Lus");
	LAi_type_actor_Reset(chr);

	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);
	ChangeCharacterAddress(chr, "None", "");

	TS_Record("16");

	chrDisableReloadToLocation = false;

	// Gregg: из каюты - обратно на берег, а с берега шлюпка снова уходит в море.
	// Возвращаем обе двери в исходное состояние, чтобы «Церебус» не остался тупиком.
	ref loc = LocFromID("Cerebus_Cabin");
	loc.reload.l1.go = "Muhachi_Shore";
	loc.reload.l1.emerge = "boat";
	loc.reload.l1.label = "Shore";

	loc = LocFromID("Muhachi_Shore");
	loc.reload.l2.go = "Beliz";
	loc.reload.l2.emerge = "reload_5";
	loc.reload.l2.label = "Sea";

	SetQuestFunction("TwoSistersLusQuestReturnToBelizComplete", "TwoSistersLusQuestReturnToBelizComplete", "l", "Beliz_town");
}

void TwoSistersLusQuestReturnToBelizComplete(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Terry Lorto", "officer_14", "man", "man", 25, PIRATE, 0, true));
	chr.name = "Терри";
	chr.lastname = "Лорто";
	chr.greeting = "Gr_QuestMan";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Rebekka.c";
	chr.Dialog.CurrentNode = "133";
	ChangeCharacterAddressGroup(chr, "Beliz_town", "goto", "goto7");
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;
	SetDialogCharacter2Character(PChar, chr, 60.0, false);
}

// Трофей уходит в эскадру под Терри Лорто.
void TwoSistersLusQuestCerebusToSquadron()
{
	ref chr = CharacterFromID("Terry Lorto");
	TS_MakeCerebus(chr);
	SetCrewQuantity(chr, makeint(GetMaxCrewQuantity(chr) / 2));
	SetGeraldDetails(chr, 2, 2, "", "pir_" + (rand(4) + 1));

	SetCompanionIndex(PChar, -1, GetCharacterIndex("Terry Lorto"));
	SetCharacterRemovable(chr, true);
	SetShipRemovable(chr, true);
	LAi_SetImmortal(chr, false);
	DeleteAttribute(chr, "NotRemoved");

	TwoSistersLusQuestExitFromBeliz();
}

// Или продаётся тут же, если места в эскадре нет.
void TwoSistersLusQuestCerebusToMoney()
{
	AddMoneyToCharacter(PChar, 45000);

	ref chr = CharacterFromID("Terry Lorto");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "", -1);
	DeleteAttribute(chr, "NotRemoved");

	TwoSistersLusQuestExitFromBeliz();
}

void TwoSistersLusQuestExitFromBeliz()
{
	TS_Record("17");
	chrDisableReloadToLocation = false;

	SetQuestFunction("TwoSistersLusQuestLastDialogWithRebekka", "TwoSistersLusQuestLastDialogWithRebekka", "l", "HouseSistersLus");
}

void TwoSistersLusQuestLastDialogWithRebekka(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Mishel Lus");
	RemovePassenger(PChar, chr);
	ChangeCharacterAddressGroup(chr, "HouseSistersLus", "goto", "goto1");

	chr = CharacterFromID("Rebekka Lus");
	ChangeCharacterAddressGroup(chr, "HouseSistersLus", "goto", "goto2");

	chr.Dialog.CurrentNode = "130";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void TwoSistersLusQuestComplete()
{
	chrDisableReloadToLocation = false;
	bDisableCharacterMenu = false;

	ref chr = CharacterFromID("Mishel Lus");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "", -1);
	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);

	chr = CharacterFromID("Rebekka Lus");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "", -1);
	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);

	// дом сестёр опустел - дверь возвращаем типовому интерьеру
	ref loc = LocFromID("Tortuga_town");
	loc.reload.l22.go = "CommonCobHouse";
	loc.reload.l22.emerge = "reload1";
	loc.reload.l22.label = "House";
	DeleteAttribute(loc, "reload.l22.disable");
	Locations[FindLocation("Tortuga_town")].locators_radius.reload.houseS1 = 1.0;

	TS_Record("18");
	PChar.TwoSisters = "5";

	PChar.TwoSisters.Lus = true;
	TwoSistersCheckComplete();
}

//////////////////////////////////////////////////////////////////////////////////////
// Ветка Натали Ремис: Бермуды
//////////////////////////////////////////////////////////////////////////////////////
void TwoSistersRemisStart(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Natali Remis");
	chr.greeting = "Enc_RapersGirl_1";
	LAi_SetActorType(chr);
	ChangeCharacterAddressGroup(chr, "Pirates_town", "quest", "ts_quest2");

	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + TS_DIANA_RANK_UP;
	chr = GetCharacter(NPC_GenerateCharacter("Ortodokse Malkin", "Fatman2_1", "man", "man1", iRank, PIRATE, -1, true));
	chr.name = "Ортодоксе";
	chr.lastname = "Малкин";
	chr.greeting = "Gr_UnpleasantPerson";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Natali.c";
	chr.Dialog.CurrentNode = "1";
	SetFoodToCharacter(chr, 3, 25);
	LAi_SetActorType(chr);
	LAi_ActorTurnToCharacter(chr, CharacterFromID("Natali Remis"));
	// Gregg: до разговора Малкина трогать не за что - он стоит посреди пиратского
	// посёлка и первым не лезет. Бессмертие снимается, как только начинается драка.
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;
	ChangeCharacterAddressGroup(chr, "Pirates_town", "quest", "ts_quest3");

	chr = CharacterFromID("Natali Remis");
	LAi_SetActorType(chr);
	LAi_ActorTurnToCharacter(chr, CharacterFromID("Ortodokse Malkin"));

	Locations[FindLocation("Pirates_town")].locators_radius.quest.ts_quest1 = 2.0;
	PChar.quest.TwoSistersRemisMeeting.win_condition.l1 = "locator";
	PChar.quest.TwoSistersRemisMeeting.win_condition.l1.location = "Pirates_town";
	PChar.quest.TwoSistersRemisMeeting.win_condition.l1.locator_group = "quest";
	PChar.quest.TwoSistersRemisMeeting.win_condition.l1.locator = "ts_quest1";
	PChar.quest.TwoSistersRemisMeeting.function = "TwoSistersRemisMeeting";
}

void TwoSistersRemisMeeting(string qName)
{
	Locations[FindLocation("Pirates_town")].locators_radius.quest.ts_quest1 = 1.0;

	ref chr = CharacterFromID("Ortodokse Malkin");
	SetDialogCharacter2Character(PChar, chr, 0.1, true);
}

void TwoSistersRemisFightWithMalkin()
{
	// Gregg: камеру после разговорной сцены возвращаем на ГГ. Одного locCameraFollow
	// мало: он меняет режим, но цель остаётся той, что выставил SetCameraDialogMode
	// через locCameraTarget - и камера продолжает висеть на собеседнике уже в бою.
	locCameraTarget(PChar);
	locCameraFollow();
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("Ortodokse Malkin");
	chr.LifeDay = 0;
	chr.SaveItemsForDead = true;
	LAi_SetImmortal(chr, false);
	DeleteAttribute(chr, "NotRemoved");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	chr = CharacterFromID("Natali Remis");
	LAi_SetActorType(chr);
	LAi_ActorAfraid(chr, CharacterFromID("Ortodokse Malkin"), false);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.TwoSistersRemisFightWithMalkinWin.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersRemisFightWithMalkinWin.win_condition.l1.character = "Ortodokse Malkin";
	PChar.quest.TwoSistersRemisFightWithMalkinWin.function = "TwoSistersRemisFightWithMalkinWin";
}

void TwoSistersRemisFightWithMalkinWin(string qName)
{
	LAi_SetFightMode(PChar, false);

	ref chr = CharacterFromID("Natali Remis");
	LAi_type_actor_Reset(chr);
	chr.dialog.CurrentNode = "11";
	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

// Gregg: подельники Малкина сидят на его бриге, и пока их не вычистишь, Натали
// на борт не пойдёт. В К:ПС под корабль переиспользовали палубы «Церебуса» и цепляли
// их прямо к выходу в море из посёлка; у нас ветки идут параллельно и «Церебус»
// к этому времени может быть уже в эскадре, поэтому у Малкина свои три локации.
void TwoSistersRemisToShip()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Natali Remis");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	// Gregg: выход в море из посёлка временно ведёт на бриг Малкина - ровно тот приём,
	// что был в оригинале. Возвращаем его в TwoSistersRemisQuestComplete.
	ref loc = LocFromID("Pirates_town");
	loc.reload.l1.go = "Malkin_Cabin";
	loc.reload.l1.emerge = "reload1";
	loc.reload.l1.label = "Malkin Ship";

	// Gregg: маршрут по бригу прописываем здесь, а не в ините: ход односторонний -
	// в капитанской каюте reload-локатор ровно один, наружу выпускает трюм после зачистки.
	loc = LocFromID("Malkin_Cabin");
	loc.reload.l1.name = "reload1";
	loc.reload.l1.go = "Malkin_WardRoom";
	loc.reload.l1.emerge = "reload1";
	loc.reload.l1.autoreload = "0";
	loc.reload.l1.label = "WardRoom";
	if (CheckAttribute(loc, "reload.l1.disable")) DeleteAttribute(loc, "reload.l1.disable");

	loc = LocFromID("Malkin_WardRoom");
	loc.reload.l1.name = "reload1";
	loc.reload.l1.go = "Malkin_Cabin";
	loc.reload.l1.emerge = "reload1";
	loc.reload.l1.autoreload = "0";
	loc.reload.l1.label = "Cabin";
	loc.reload.l2.name = "reload_hold";
	loc.reload.l2.go = "Malkin_Hold";
	loc.reload.l2.emerge = "reload1";
	loc.reload.l2.autoreload = "0";
	loc.reload.l2.label = "Hold";
	if (CheckAttribute(loc, "reload.l2.disable")) DeleteAttribute(loc, "reload.l2.disable");

	loc = LocFromID("Malkin_Hold");
	loc.reload.l1.name = "reload1";
	loc.reload.l1.go = "Malkin_WardRoom";
	loc.reload.l1.emerge = "reload_hold";
	loc.reload.l1.autoreload = "0";
	loc.reload.l1.label = "WardRoom";

	TS_Record("19");
	SetQuestFunction("TwoSistersRemisInShip", "TwoSistersRemisInShip", "l", "Malkin_Cabin");
}

void TwoSistersRemisInShip(string qName)
{
	chrDisableReloadToLocation = true;

	int iRank = GetRank(PChar, 5);
	ref chr = GetCharacter(NPC_GenerateCharacter("Malkin Sailor 1", "BigPirate", "man", "man", iRank, PIRATE, 0, true));
	chr.name = iTxt("TS_MalkinSailor");
	chr.lastname = "";
	chr.greeting = "Gr_UnpleasantPerson";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Natali.c";
	chr.Dialog.CurrentNode = "22";
	SetFoodToCharacter(chr, 3, 25);
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;
	ChangeCharacterAddressGroup(chr, "Malkin_Cabin", "rld", "loc0");
	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

void TwoSistersRemisFightWithFirstSailor()
{
	ref chr = CharacterFromID("Malkin Sailor 1");
	chr.LifeDay = 0;
	chr.SaveItemsForDead = true;
	LAi_SetImmortal(chr, false);
	DeleteAttribute(chr, "NotRemoved");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.TwoSistersRemisFightWithFirstSailorWin.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersRemisFightWithFirstSailorWin.win_condition.l1.character = "Malkin Sailor 1";
	PChar.quest.TwoSistersRemisFightWithFirstSailorWin.function = "TwoSistersRemisFightWithFirstSailorWin";
}

void TwoSistersRemisFightWithFirstSailorWin(string qName)
{
	chrDisableReloadToLocation = false;
	SetQuestFunction("TwoSistersRemisMeetingWithTwoSailor", "TwoSistersRemisMeetingWithTwoSailor", "l", "Malkin_WardRoom");
}

void TwoSistersRemisMeetingWithTwoSailor(string qName)
{
	chrDisableReloadToLocation = true;

	int iRank = GetRank(PChar, 0);
	ref chr = GetCharacter(NPC_GenerateCharacter("Malkin Sailor 2", "ManBal3", "man", "man", iRank, PIRATE, 0, true));
	chr.name = iTxt("TS_MalkinSailor2");
	chr.lastname = "";
	chr.greeting = "Gr_UnpleasantPerson";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Natali.c";
	chr.Dialog.CurrentNode = "23";
	SetFoodToCharacter(chr, 3, 25);
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;
	ChangeCharacterAddressGroup(chr, "Malkin_WardRoom", "goto", "goto1");
	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

void TwoSistersRemisFightWithSecondSailor()
{
	LAi_LocationFightDisable(&Locations[FindLocation("Malkin_WardRoom")], false);

	ref chr = CharacterFromID("Malkin Sailor 2");
	chr.LifeDay = 0;
	chr.SaveItemsForDead = true;
	LAi_SetImmortal(chr, false);
	DeleteAttribute(chr, "NotRemoved");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.TwoSistersRemisFightWithSecondSailorWin.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersRemisFightWithSecondSailorWin.win_condition.l1.character = "Malkin Sailor 2";
	PChar.quest.TwoSistersRemisFightWithSecondSailorWin.function = "TwoSistersRemisFightWithSecondSailorWin";
}

void TwoSistersRemisFightWithSecondSailorWin(string qName)
{
	chrDisableReloadToLocation = false;
	SetQuestFunction("TwoSistersRemisMeetingWithThreeSailor", "TwoSistersRemisMeetingWithThreeSailor", "l", "Malkin_Hold");
}

void TwoSistersRemisMeetingWithThreeSailor(string qName)
{
	chrDisableReloadToLocation = true;

	int iRank = GetRank(PChar, -5);
	ref chr = GetCharacter(NPC_GenerateCharacter("Malkin Sailor 3", "ManBal5", "man", "man", iRank, PIRATE, 0, true));
	chr.name = iTxt("TS_MalkinSailor3");
	chr.lastname = "";
	chr.greeting = "Gr_UnpleasantPerson";
	chr.Dialog.FileName = "Quest\KPS\TwoSisters\TS_Natali.c";
	chr.Dialog.CurrentNode = "24";
	SetFoodToCharacter(chr, 3, 25);
	LAi_SetImmortal(chr, true);
	chr.NotRemoved = true;
	ChangeCharacterAddressGroup(chr, "Malkin_Hold", "goto", "goto1");
	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

void TwoSistersRemisFightWithThirdSailor()
{
	LAi_LocationFightDisable(&Locations[FindLocation("Malkin_Hold")], false);

	ref chr = CharacterFromID("Malkin Sailor 3");
	chr.LifeDay = 0;
	chr.SaveItemsForDead = true;
	LAi_SetImmortal(chr, false);
	DeleteAttribute(chr, "NotRemoved");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.TwoSistersRemisFightWithThirdSailorWin.win_condition.l1 = "NPC_Death";
	PChar.quest.TwoSistersRemisFightWithThirdSailorWin.win_condition.l1.character = "Malkin Sailor 3";
	PChar.quest.TwoSistersRemisFightWithThirdSailorWin.function = "TwoSistersRemisFightWithThirdSailorWin";
}

void TwoSistersRemisFightWithThirdSailorWin(string qName)
{
	chrDisableReloadToLocation = false;

	// Gregg: бриг зачищен - трюм выпускает обратно в посёлок. До этой минуты выхода
	// с корабля нет вовсе: капитанская каюта ведёт только вглубь.
	ref loc = LocFromID("Malkin_Hold");
	loc.reload.l1.name = "reload1";
	loc.reload.l1.go = "Pirates_town";
	loc.reload.l1.emerge = "reload1_back";
	loc.reload.l1.autoreload = "0";
	loc.reload.l1.label = "Town";

	SetQuestFunction("TwoSistersRemisQuestComplete", "TwoSistersRemisQuestComplete", "l", "Pirates_town");
}

void TwoSistersRemisQuestComplete(string qName)
{
	// Gregg: страховка от залипших флагов - меню персонажа и переходы должны быть
	// открыты, чем бы ни кончилась ветка.
	chrDisableReloadToLocation = false;
	bDisableCharacterMenu = false;

	// выход в море из посёлка возвращаем на место
	ref loc = LocFromID("Pirates_town");
	loc.reload.l1.go = "Bermudes";
	loc.reload.l1.emerge = "reload_1";
	loc.reload.l1.label = "Sea";

	ref chr = CharacterFromID("Natali Remis");
	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);
	ChangeCharacterAddress(chr, "None", "");

	TS_Record("20");
	PChar.TwoSisters.Remis = true;
	TwoSistersCheckComplete();
}

//////////////////////////////////////////////////////////////////////////////////////
// Сведение веток и финал
//////////////////////////////////////////////////////////////////////////////////////
void TwoSistersCheckComplete()
{
	bool bViki = false;
	bool bLus = false;
	bool bRemis = false;

	if (CheckAttribute(PChar, "TwoSisters.Viki")) bViki = true;
	if (CheckAttribute(PChar, "TwoSisters.Lus")) bLus = true;
	if (CheckAttribute(PChar, "TwoSisters.Remis")) bRemis = true;

	if (!bViki) return;
	if (!bLus) return;
	if (!bRemis) return;

	PChar.TwoSisters = "6";
	TS_Record("21");
	SetQuestFunction("TwoSistersWomanExitFromShip", "TwoSistersWomanExitFromShip", "l", "SanJuan_town");
}

// Девицы сходят на берег и оседают в новом заведении.
void TwoSistersWomanExitFromShip(string qName)
{
	string sCharacter;
	string sLocator;
	ref chr;
	int iN;

	for (iN = 0; iN < 4; iN++)
	{
		switch (iN)
		{
			case 0:
				sCharacter = "Viki Henderson";
				sLocator = "goto2";
			break;

			case 1:
				sCharacter = "Mishel Lus";
				sLocator = "goto3";
			break;

			case 2:
				sCharacter = "Rebekka Lus";
				sLocator = "goto4";
			break;

			case 3:
				sCharacter = "Natali Remis";
				sLocator = "goto6";
			break;
		}

		if (GetCharacterIndex(sCharacter) == -1) continue;

		chr = CharacterFromID(sCharacter);
		RemovePassenger(PChar, chr);

		chr.City = "SanJuan";
		chr.CityType = "horse";
		chr.dialog.filename = "Common_Brothel.c";
		chr.dialog.currentnode = "Horse_talk";
		chr.greeting = "whore";
		chr.quest.price = 0;
		LAi_SetCitizenType(chr);
		LAi_group_MoveCharacter(chr, "PIRATE_CITIZENS");
		ChangeCharacterAddressGroup(chr, "SanJuan_Brothel", "goto", sLocator);
	}
}

void TwoSistersEnd()
{
	TS_Record("22");
	CloseQuestHeader("TwoSisters");
	pchar.questdata.TwoSisters = 1;	// LEO: отметка в энциклопедии
	UnlockAchievement("TwoSisters_quest", 3);
	PChar.TwoSisters = "END";

	// Gregg: бордель открывается вместо типового дома - это и есть та награда, ради
	// которой Кендарт затевал всю историю.
	ref loc = LocFromID("SanJuan_town");
	loc.reload.l27.go = "SanJuan_Brothel";
	loc.reload.l27.emerge = "reload1";
	loc.reload.l27.label = "Brothel";
	DeleteAttribute(loc, "reload.l27.disable");
	RecalculateJumpTable();

	PChar.TwoSisters.CanSetCannons = true;

	// Gregg: достижение линии здесь НЕ трогаем - третий уровень «Чёрной Орхидеи» уже
	// выдан в конце главы 9, а повторный UnlockAchievement на сейве без атрибута
	// сыплет в лог «missed attribute».
	// Gregg: главы 11+ ещё не перенесены, таймер следующей части не взводим.
}

void TwoSisters_RepairSanJuanBrothel()
{
	// LEO: таблица хранится в сейве; повторная запись того же слота не создаёт дубликатов.
	// LEO: FRP_BROTHEL=9 и FRT_BROTHEL=0 объявлены только в отдельном сегменте таблицы.
	objFastReloadTable.table.SanJuan.l11.pic = 9;
	objFastReloadTable.table.SanJuan.l11.tex = 0;
	objFastReloadTable.table.SanJuan.l11.location = "SanJuan_Brothel";

	if (!CheckAttribute(pchar, "TwoSisters")) return;
	if (pchar.TwoSisters != "END") return;

	int iTown = FindLocation("SanJuan_town");
	if (iTown < 0) return;
	ref loc = &Locations[iTown];
	if (!CheckAttribute(loc, "reload.l27.go")) return;
	if (loc.reload.l27.go != "SanJuan_Brothel") return;
	if (CheckAttribute(loc, "reload.l27.emerge"))
	{
		if (loc.reload.l27.emerge == "reload1") return;
	}

	// LEO: меняем только место появления, не открывая чужие замки и не заменяя назначение двери.
	loc.reload.l27.emerge = "reload1";
	RecalculateJumpTable();
}

// Gregg: 42-фунтовые пушки от Кендарта. В CSP лавка и «установить все» фильтруют
// по MaxCaliber корпуса, поэтому мало сменить тип пушек - нужно поднять и калибр.
// Правим через Tuning-поля, как это делает бермудский тюнинг (ShipsTuning.c), иначе
// подарок утечёт на все корабли этого типа, включая вражеские.
void TwoSistersSetCannons()
{
	DeleteAttribute(PChar, "TwoSisters.CanSetCannons");

	ref rShip = GetRealShip(sti(PChar.Ship.Type));
	rShip.MaxCaliber = 42;
	rShip.Tuning.MaxCaliber = true;
	rShip.Tuning.Cannon = true;

	int iCannonsQty = GetCannonsNum(PChar);
	int iCannonType = sti(PChar.Ship.Cannons.Type);
	int iGoodType = GetCannonGoodsIdxByType(iCannonType);

	// старые пушки складываем в трюм товаром, чтобы игрок мог их продать
	if (GetCargoGoods(PChar, iGoodType) >= 1)
	{
		iCannonsQty += GetCargoGoods(PChar, iGoodType);
	}
	if (iCannonsQty >= 1)
	{
		SetCharacterGoods(PChar, iGoodType, iCannonsQty);
	}

	iCannonsQty = GetCannonsNum(PChar);
	PChar.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS42;
	RecalculateCargoLoad(PChar);

	int iMaxCannons = GetBortCannonsMaxQty(PChar, "cannonl") + GetBortCannonsMaxQty(PChar, "cannonr") + GetBortCannonsMaxQty(PChar, "cannonb") + GetBortCannonsMaxQty(PChar, "cannonf");
	int iCannons = iMaxCannons - iCannonsQty;

	if (iCannons >= 1)
	{
		SetCharacterGoods(PChar, GOOD_CANNON_42, iCannons);
	}

	LAi_Fade("", "");
	bQuestCheckProcessFreeze = true;
	WaitDate("", 0, 0, 3, 0, 60.0);
	RecalculateJumpTable();
	bQuestCheckProcessFreeze = false;
	RefreshLandTime();

	SetCharacterShipLocation(PChar, "SanJuan_town");
}
// Gregg: <== квест «Две сестры»
