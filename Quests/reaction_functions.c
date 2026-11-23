
ref		sld, rCharacter;
int     iTemp, i; // нужно для вычислений любых целых (нации)
float   locx, locy, locz;
string  sTemp; // любые строки для вычислений
bool    bOk;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  разное с оптимизации   начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//ПГГ инфо
void OpenPGGInformation()
{
	LaunchPsHeroScreen();
}

//==> Дело жизни и смерти
// Gregg: общий хвост спавна берглара — SuperShooter + HP/логин/тип-гражданина + переезд в нац-группу.
// Полный табличный SpawnBerglar НЕ делаю: у 6 блоков ~20 разных полей + переменные перки,
// хелпер на 20 позиционных параметров был бы хуже и опаснее самих блоков (квест-спавн).
void BerglarFinalize(ref sld, string sCitizenGroup)
{
	sld.SuperShooter  = true;
	LAi_SetHP(sld, 1.0, 1.0);
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, sCitizenGroup);
}

void Birglars_Spawn(string qName)
{
	// Бандиты сразу экипировывуют оружие при выдаче (Это из-за новой логики экипировки.), поэтому теперь оно выдается и экипировывуется прямо в диалоге Бергларов.
	ref sld;
	pchar.questTemp.tugs.berglarState = 1; //счётчик
	//============> грабитель в Сент-Джонсе
	pchar.questTemp.tugs.berglarSentJons = "SentJons_TownCave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarSentJons.hp = 160; //сколько HP
	pchar.questTemp.tugs.berglarSentJons.locator = "basement1"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarSentJons", "citiz_12", "man", "man", 22, ENGLAND, -1, false));
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.1", "Names.txt");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_1", "Names.txt", "#space#", " ");
	sld.rank = 25;
	sld.city = "SentJons";
	sld.location	= "SentJons_town";
	sld.location.group = "goto";
	sld.location.locator = "goto4";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	TakeNItems(sld, "jewelry1", 30+rand(10));
	TakeNItems(sld, "bullet", 51);
	TakeNItems(sld, "GunPowder", 51);
	sld.money = 10460;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 10, 100, 70, 100, 70);
	SetCharacterPerk(sld, "SwordplayProfessional");
	SetCharacterPerk(sld, "Sliding");
	SetCharacterPerk(sld, "HardHitter");
	BerglarFinalize(sld, "ENGLAND_CITIZENS");
	//=============> грабитель на Мартинике
	pchar.questTemp.tugs.berglarFortFrance = "FortFrance_Dungeon"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarFortFrance.hp = 180; //сколько HP
	pchar.questTemp.tugs.berglarFortFrance.locator = "basement1"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarFortFrance", "citiz_11", "man", "man", 23, FRANCE, -1, false));
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.2", "Names.txt");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_2", "Names.txt", "#space#", " ");
	sld.rank = 25;
	sld.city = "FortFrance";
	sld.location	= "FortFrance_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	TakeNItems(sld, "bullet", 51);
	TakeNItems(sld, "GunPowder", 51);
	TakeNItems(sld, "jewelry2", 25+rand(10));
	sld.money = 12670;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 10, 100, 70, 50, 60);
	SetCharacterPerk(sld, "BasicDefense");
	BerglarFinalize(sld, "FRANCE_CITIZENS");
	//============> грабитель в Мариго
	pchar.questTemp.tugs.berglarMarigo = "Marigo_Cave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarMarigo.hp = 200; //сколько HP
	pchar.questTemp.tugs.berglarMarigo.locator = "gate_back"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarMarigo", "citiz_1", "man", "man", 21, HOLLAND, -1, false));
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.3", "Names.txt");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_3", "Names.txt", "#space#", " ");
	sld.rank = 25;
	sld.city = "Marigo";
	sld.location	= "Marigo_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	TakeNItems(sld, "jewelry3", 25+rand(10));
	TakeNItems(sld, "bullet", 51);
	TakeNItems(sld, "GunPowder", 51);
	sld.money = 15480;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 60, 10, 100, 100, 40);
	SetCharacterPerk(sld, "CriticalHit");
	SetCharacterPerk(sld, "Sliding");
	SetCharacterPerk(sld, "HardHitter");
	BerglarFinalize(sld, "HOLLAND_CITIZENS");
	//============> грабитель в Панаме
	pchar.questTemp.tugs.berglarPanama = "Panama_Cave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarPanama.hp = 200; //сколько HP
	pchar.questTemp.tugs.berglarPanama.locator = "gate_back"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarPanama", "citiz_4", "man", "man", 27, SPAIN, -1, false));
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.4", "Names.txt");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_4", "Names.txt", "#space#", " ");
	sld.rank = 27;
	sld.city = "Panama";
	sld.location	= "Panama_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	TakeNItems(sld, "jewelry2", 30+rand(5));
	TakeNItems(sld, "bullet", 51);
	TakeNItems(sld, "GunPowder", 51);
	sld.money = 15180;
	sld.talker = 8; //начать диалог
	SetSelfSkill(sld, 100, 10, 50, 50, 70);
	SetCharacterPerk(sld, "AdvancedDefense");
	BerglarFinalize(sld, "SPAIN_CITIZENS");
	//============> грабитель в Картахене
	pchar.questTemp.tugs.berglarCartahena = "Cartahena_Cave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarCartahena.hp = 170; //сколько HP
	pchar.questTemp.tugs.berglarCartahena.locator = "gate_back"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarCartahena", "citiz_7", "man", "man", 22, SPAIN, -1, false));
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.5", "Names.txt");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_5", "Names.txt", "#space#", " ");
	sld.rank = 22;
	sld.city = "Cartahena";
	sld.location	= "Cartahena_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	TakeNItems(sld, "jewelry1", 40+rand(10));
	TakeNItems(sld, "bullet", 51);
	TakeNItems(sld, "GunPowder", 51);
	sld.money = 15980;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 100, 10, 70, 100, 50);
	SetCharacterPerk(sld, "Sliding");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "CriticalHit");
	BerglarFinalize(sld, "SPAIN_CITIZENS");
	//============> грабитель в Санта Каталине
	pchar.questTemp.tugs.berglarSantaCatalina = "SantaCatalina_Cave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarSantaCatalina.hp = 230; //сколько HP
	pchar.questTemp.tugs.berglarSantaCatalina.locator = "gate_back"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarSantaCatalina", "citiz_9", "man", "man", 25, SPAIN, -1, false));
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.6", "Names.txt");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_6", "Names.txt", "#space#", " ");
	sld.rank = 25;
	sld.city = "SantaCatalina";
	sld.location	= "SantaCatalina_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	TakeNItems(sld, "jewelry3", 25+rand(10));
	TakeNItems(sld, "bullet", 51);
	TakeNItems(sld, "GunPowder", 51);
	sld.money = 16450;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 10, 10, 100, 50, 80);
	SetCharacterPerk(sld, "CriticalHit");
	BerglarFinalize(sld, "SPAIN_CITIZENS");
}
void Birglars_fight(string qName)
{
	if (GetCharacterIndex("Berglar" + pchar.quest.(qName).city) != -1)
	{
		LAi_SetFightMode(pchar, false);
		LAi_LockFightMode(pchar, true);
		sTemp = "berglar" + pchar.quest.(qName).city;
		pchar.questTemp.tugs.(sTemp) = "over";
		sld = characterFromId("Berglar" + pchar.quest.(qName).city);
		LAi_SetActorType(sld);
		LAi_ActorDialog(sld, pchar, "", 2.0, 0);
	}
}
//--> ОЗГ - пират
// Gregg: единый расчёт ранга вражеского капитана из ранга ГГ (было 6 копий блока в reaction_functions/Common_Mayor)
int GetQuestEnemyRank()
{
	int iRank;
	if (sti(pchar.rank) > 6)
	{
		if (sti(pchar.rank) > 20) iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*3);
		else iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*3/2);
	}
	else
	{
		if (sti(pchar.rank) > 3) iRank = sti(pchar.rank);
		else iRank = 1;
	}
	return iRank;
}

void TakePirateship_CreateShip(string qName)
{
	Log_Info("Пиратское судно на горизонте!");
	DoQuestCheckDelay("see_sails", 1.3);
	pchar.quest.AllMayorsQuests_Late.over = "yes"; //снимаем общий таймер
	int iRank = GetQuestEnemyRank();
	string sShipName = pchar.GenQuest.TakePirateship.ShipName;
	Group_FindOrCreateGroup("ContraCureer");
	Group_SetType("ContraCureer", "pirate");//тип группы
	sld = GetCharacter(NPC_GenerateCharacter("ContraCureerCap", "pirate_"+1+rand(9), "man", "man", iRank, PIRATE, -1, true));
	SetCaptanModelByEncType(sld, "pirate");
	DeleteAttribute(sld, "Ship");
	SetShipToFantom(sld, "pirate", true);
	Fantom_SetCannons(sld, "war");
	SetCrewQuantityFull(sld);
    Fantom_SetBalls(sld, "war");
	Fantom_SetGoods(sld, "war");
	sld.name = pchar.GenQuest.TakePirateship.Name;
	sld.lastname = "";
	DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");			   
	sld.DontRansackCaptain = true;
	sld.AnalizeShips = true;
	sld.WatchFort = true;
	sld.AlwaysEnemy = true;
	sld.Ship.Mode = "pirate";
	sld.Ship.name = sShipName;
	SetCharacterPerk(sld, "MusketsShoot");
	Group_AddCharacter("ContraCureer", "ContraCureerCap");
	Group_SetType("ContraCureer", "pirate");
	Group_SetGroupCommander("ContraCureer", "ContraCureerCap");
	Group_SetAddress("ContraCureer", pchar.GenQuest.TakePirateship.island, "quest_ships", "quest_ship_10");
	Group_SetTaskAttack("ContraCureer", PLAYER_GROUP);
	pchar.quest.ContraCureer_AfterBattle.win_condition.l1 = "Group_Death";
	pchar.quest.ContraCureer_AfterBattle.win_condition.l1.group = "ContraCureer";
	pchar.quest.ContraCureer_AfterBattle.function = "ContraPirate_AfterBattle";
	pchar.quest.ContraCureer_DieHard.win_condition.l1 = "MapEnter";
	pchar.quest.ContraCureer_DieHard.function = "ContraPirate_DieHard";
}
void ContraPirate_AfterBattle(string qName)//победили
{
	pchar.quest.AllMayorsQuests_Late.over = "yes";
	pchar.quest.ContraCureer_DieHard.over = "yes";
	Group_DeleteGroup("ContraCureer");
	ChangeOfficersLoyality("good_all", 1);
	Island_SetReloadEnableGlobal(pchar.GenQuest.TakePirateship.Island, true);
	AddComplexSeaExpToScill(200, 150, 150, 150, 150, 150, 0);
	AddQuestRecord("MayorsQuestsList", "23-1");
	pchar.GenQuest.TakePirateship = "Execute"; //флаг выполнил успешно
}

void ContraPirate_DieHard(string qName)//сбежали
{
	pchar.quest.ContraCureer_AfterBattle.over = "yes";
	Group_DeleteGroup("ContraCureer");
	Island_SetReloadEnableGlobal(pchar.GenQuest.TakePirateship.Island, true);
	ChangeOfficersLoyality("bad_all", 1);
	AddQuestRecord("MayorsQuestsList", "23-2");
	pchar.GenQuest.TakePirateship = "Found"; //флаг провалил
	pchar.quest.AllMayorsQuests_Late.over = "yes";
}
//--> Контркурьер
void TakePostcureer_CreateShip(string qName)
{
	int iNation = sti(pchar.GenQuest.TakePostcureer.Nation);
	int iDays = sti(pchar.GenQuest.TakePostcureer.Terms2);
	int iRank = GetQuestEnemyRank();
	string sShipName = pchar.GenQuest.TakePostcureer.ShipName;
	string sCapId = "ContraCureerCap";
    Group_FindOrCreateGroup("ContraCureer");//создать группу
	Group_SetType("ContraCureer", "war");//тип группы
	sld = GetCharacter(NPC_GenerateCharacter("ContraCureerCap", "nationmodel", "man", "man", iRank, iNation, iDays, true));
	SetCaptanModelByEncType(sld, "war");
	DeleteAttribute(sld, "Ship");
	SetShipToCureer(sld, iNation, true);
	Fantom_SetCannons(sld, "war");
	SetCrewQuantityFull(sld);
    Fantom_SetBalls(sld, "war");
	Fantom_SetGoods(sld, "war");
	sld.DontRansackCaptain = true;
	sld.AnalizeShips = true;
	sld.Ship.name = sShipName;
	sld.Ship.Mode = "war";
	DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");
	int iGoods = GOOD_COFFEE + drand(sti(GOOD_PAPRIKA - GOOD_COFFEE));
	int iSpace = GetCharacterFreeSpace(sld, iGoods);
	iSpace = makeint(iSpace/4);
	AddCharacterGoods(sld, iGoods, iSpace);
	sld.mapEnc.type = "trade";
	sld.mapEnc.worldMapShip = "quest_ship";
    sld.mapEnc.Name = "'" + sShipName + " '";
    Group_AddCharacter("ContraCureer", sld.id);
	Group_SetGroupCommander("ContraCureer", sld.id);
    Group_SetTaskAttackInMap("ContraCureer", PLAYER_GROUP);
    Group_LockTask("ContraCureer");
	Map_CreateTrader(pchar.GenQuest.TakePostcureer.City, pchar.GenQuest.TakePostcureer.CityA, sld.id, iDays);
	pchar.quest.ContraCureer_Abordage.win_condition.l1 = "Group_Death";
	pchar.quest.ContraCureer_Abordage.win_condition.l1.group = "ContraCureer";
	pchar.quest.ContraCureer_Abordage.function = "ContraCureer_AfterBattle";//взяли на абордаж
}

void ContraCureer_AfterBattle(string qName)//победили
{
	pchar.quest.AllMayorsQuests_Late.over = "yes";	
	Group_DeleteGroup("ContraCureer");
	pchar.quest.ContraCureer_DieHard.over = "yes";
	Island_SetReloadEnableGlobal(pchar.GenQuest.TakePostcureer.Island, true);
	if (CheckCharacterItem(pchar, "sicretLetter"))
	{
		AddComplexSeaExpToScill(150, 100, 100, 100, 100, 100, 0);
		ChangeOfficersLoyality("good_all", 1);
		AddQuestRecord("MayorsQuestsList", "24-1");
		pchar.GenQuest.TakePostcureer = "Execute"; //флаг выполнил успешно
	}
	else
	{
		ChangeOfficersLoyality("bad_all", 1);
		AddQuestRecord("MayorsQuestsList", "24-2");
		pchar.GenQuest.TakePostcureer = "Found"; //флаг провалил
	}
}

void TakeArsenalship_CreateShip(string qName)
{
    int iNation = sti(pchar.GenQuest.TakeArsenalship.Nation);
    int iDays = sti(pchar.GenQuest.TakeArsenalship.Terms2);
    int iRank;
    ref sld, sld2;

    iRank = GetQuestEnemyRank();

    string sShipName = pchar.GenQuest.TakeArsenalship.ShipName;
    string sCapId = "ContraArsCap";      // Кэп арсенала
    string sEscortId = "ContraArsEscort"; // Кэп конвоира

    sld = GetCharacter(NPC_GenerateCharacter(sCapId, "nationmodel", "man", "man", iRank, iNation, -1, true));
    SetCaptanModelByEncType(sld, "war");
    DeleteAttribute(sld, "Ship");
    SetShipToFantom(sld, "war", true);
    Fantom_SetCannons(sld, "war");
    SetCrewQuantityFull(sld);
    Fantom_SetBalls(sld, "war");
    sld.ship.name = sShipName;
    sld.AlwaysEnemy = true;
    sld.DontRansackCaptain = true;
    sld.Ship.Mode = "war";
    int iGoods = GOOD_BALLS + drand(makeint(GOOD_BOMBS - GOOD_BALLS));
    int iSpace = GetCharacterFreeSpace(sld, iGoods);
    iSpace = makeint(iSpace/4);
    AddCharacterGoods(sld, iGoods, iSpace);
    AddCharacterGoods(sld, GOOD_POWDER, iSpace);
    AddCharacterGoods(sld, GOOD_WEAPON, 1000+rand(1000));
    AddCharacterGoods(sld, GOOD_BALLS, 500);
    AddCharacterGoods(sld, GOOD_GRAPES, 1000);
    AddCharacterGoods(sld, GOOD_KNIPPELS, 500);
    AddCharacterGoods(sld, GOOD_BOMBS, 500);
    sld2 = GetCharacter(NPC_GenerateCharacter(sEscortId, "nationmodel", "man", "man", iRank, iNation, -1, true)); 
    SetCaptanModelByEncType(sld2, "war");
    DeleteAttribute(sld2, "Ship");
    SetShipToFantom(sld2, "trade", true);
    Fantom_SetCannons(sld2, "war");
    SetCrewQuantityFull(sld2);
    Fantom_SetBalls(sld2, "war");
    sld2.AlwaysEnemy = true;
    sld2.Ship.Mode = "war";
    sld2.mapEnc.Name = "'" + sShipName + "', а также корабль охраны"; 
    Group_DeleteGroup("ContraArsenal");
    Group_FindOrCreateGroup("ContraArsenal");
    Group_AddCharacter("ContraArsenal", sEscortId); 
    Group_AddCharacter("ContraArsenal", sCapId);    
    Group_SetGroupCommander("ContraArsenal", sEscortId);     
    SetCompanionIndex(sld2, -1, sti(sld.index)); 

    // Настройки карты (теперь привязаны к sld2)
    sld2.mapEnc.type = "trade";
    sld2.mapEnc.worldMapShip = "quest_ship";

    sld.Quests.Group = "ContraArsenal";
    sld2.Quests.Group = "ContraArsenal";

    Group_SetTaskAttack("ContraArsenal", PLAYER_GROUP);
    Group_SetAddress("ContraArsenal", pchar.GenQuest.TakeArsenalship.Island, "Quest_Ships", "Quest_Ship_"+(3+rand(4)));
    Group_LockTask("ContraArsenal");

    // Создаем торговца по ID ОХРАННИКА
    Map_CreateTrader(pchar.GenQuest.TakeArsenalship.City, pchar.GenQuest.TakeArsenalship.CityA, sEscortId, iDays);

    // Условие победы всё равно вешаем на корабль с ГРУЗОМ (sld)
    pchar.quest.ContraArs_AfterBattle.win_condition.l1 = "NPC_Death";
    pchar.quest.ContraArs_AfterBattle.win_condition.l1.character = sCapId;
    pchar.quest.ContraArs_AfterBattle.function = "ContraArs_AfterBattle";
}

void ContraArs_AfterBattle(string qName)//победили
{
		pchar.quest.AllMayorsQuests_Late.over = "yes";
	pchar.quest.ContraArs_DieHard.over = "yes";
	Island_SetReloadEnableGlobal(pchar.GenQuest.TakeArsenalship.Island, true);
	AddComplexSeaExpToScill(150, 100, 100, 100, 100, 100, 0);
	ChangeOfficersLoyality("good_all", 1);
	AddQuestRecord("MayorsQuestsList", "25-1");
	pchar.GenQuest.TakeArsenalship = "Execute"; //флаг выполнил успешно
}

void TakePassenger_CreateShip(string qName)
{
	int iNation = sti(pchar.GenQuest.TakePassenger.Nation);
	int iDays = sti(pchar.GenQuest.TakePassenger.Terms2);
	int iRank = GetQuestEnemyRank();
	string sShipName = pchar.GenQuest.TakePassenger.ShipName;
	string sCapId = "ContraPassCap";
    string sGroup = "Sea_" + sCapId + "1";
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	sld = GetCharacter(NPC_GenerateCharacter("ContraPassCap", "nationmodel", "man", "man", iRank, iNation, iDays, true));
	SetCaptanModelByEncType(sld, "war");
	DeleteAttribute(sld, "Ship");
	SetShipToFantom(sld, iNation, true);
	Fantom_SetCannons(sld, "war");
	SetCrewQuantityFull(sld);
    Fantom_SetBalls(sld, "war");
	Fantom_SetGoods(sld, "war");
	sld.DontRansackCaptain = true;
	sld.AnalizeShips = true;
	sld.Ship.name = sShipName;
	sld.Ship.Mode = "war";
	DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");
	int iGoods = GOOD_COFFEE + drand(sti(GOOD_PAPRIKA - GOOD_COFFEE));
	int iSpace = GetCharacterFreeSpace(sld, iGoods);
	iSpace = makeint(iSpace/4);
	AddCharacterGoods(sld, iGoods, iSpace);
	sld.mapEnc.type = "trade";
	sld.mapEnc.worldMapShip = "quest_ship";
    sld.mapEnc.Name = "'" + sShipName + " '";
    Group_AddCharacter(sGroup, sld.id);
	Group_SetGroupCommander(sGroup, sld.id);
    Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);
	Map_CreateTrader(pchar.GenQuest.TakePassenger.City, pchar.GenQuest.TakePassenger.CityA, sld.id, iDays);
	pchar.quest.ContraCureer_Abordage.win_condition.l1 = "Character_Capture";
	pchar.quest.ContraCureer_Abordage.win_condition.l1.character = "ContraPassCap";
	pchar.quest.ContraCureer_Abordage.function = "ContraPass_Abordage";//взяли на абордаж
	pchar.quest.ContraCureer_Sink.win_condition.l1 = "Character_sink";
	pchar.quest.ContraCureer_Sink.win_condition.l1.character = "ContraPassCap";
	pchar.quest.ContraCureer_Sink.function = "ContraPass_Sink";//потопили
}
void ContraPass_Abordage(string qName)//победили
{
	pchar.quest.AllMayorsQuests_Late.over = "yes"; //снимаем общий таймер
	pchar.quest.ContraCureer_Sink.over = "yes";
	Group_DeleteGroup("Sea_ContraPassCap1");
	AddComplexSeaExpToScill(150, 100, 100, 100, 100, 100, 0);
	ChangeOfficersLoyality("good_all", 1);
	AddQuestRecord("MayorsQuestsList", "26-1");
	pchar.GenQuest.TakePassenger = "Execute"; //флаг выполнил успешно
}

void ContraPass_Sink(string qName)//провалили
{
	pchar.quest.AllMayorsQuests_Late.over = "yes"; //снимаем общий таймер
	pchar.quest.ContraCureer_Abordage.over = "yes";
	Group_DeleteGroup("Sea_ContraPassCap1");
	ChangeOfficersLoyality("bad_all", 1);
	AddQuestRecord("MayorsQuestsList", "26-2");
	pchar.GenQuest.TakePassenger = "Found"; //флаг провалил
}
void Birglars_Time(string qName) // удаление квеста Остина и всех остинцев в случае неявки на встречу или отказа в диалоге
{
	DeleteAttribute(pchar, "questTemp.tugs"); // удаляем базу квеста
	pchar.questTemp.tugs.berglarState = 0; // обнуляем счётчик во избежание глюков
	// --> удаляем всех остинцев, без разницы, кто жив, кто погиб, так как встречи с ними могут быть в произвольном порядке
	sld = characterFromId("BerglarSentJons");
	sld.lifeday = 0;
	sld = characterFromId("BerglarFortFrance");
	sld.lifeday = 0;
	sld = characterFromId("BerglarMarigo");
	sld.lifeday = 0;
	sld = characterFromId("BerglarPanama");
	sld.lifeday = 0;
	sld = characterFromId("BerglarCartahena");
	sld.lifeday = 0;
	sld = characterFromId("BerglarSantaCatalina");
	sld.lifeday = 0;
	// <-- удаляем всех остинцев, без разницы, кто жив, кто погиб, так как встречи с ними могут быть в произвольном порядке
	if (CheckAttribute(pchar, "questTemp.OstinDelete"))
	{
		CloseQuestHeader("BerglarsGang"); // квест в архив
		pchar.questdata.Ostin = 1;
	}
	else
	{
		AddQuestRecord("BerglarsGang", "15"); // запись на случай провала квеста при неявке на встречу
		AddQuestUserData("BerglarsGang", "sSex", GetSexPhrase("ся","ась"));
		CloseQuestHeader("BerglarsGang"); // квест в архив
		pchar.questdata.Ostin = 1;
	}
}
//<== Дело жизни и смерти

//освободить флаг на дачу миниквестов
void SmallQuests_free(string qName)
{
	pchar.questTemp.different = "free";
}
//вернуть тип губернаторов обратно в sit после боевки
void MayorSitBack(string qName)
{
	iTemp = GetCharacterIndex(Pchar.quest.MayorSitBack.mayorId)
	if (iTemp > 0)
	{
		sld = &characters[iTemp];
		sld.location = Pchar.quest.MayorSitBack.locationName;
		sld.location.group = "sit";
		sld.location.locator = Pchar.quest.MayorSitBack.locatorName;
		LAi_SetHuberTypeNoGroup(sld);
		RemoveCharacterEquip(sld, BLADE_ITEM_TYPE);
		RemoveCharacterEquip(sld, GUN_ITEM_TYPE);
	}
}

void Ascold_fightInsideTemple(string qName)
{
	if (MOD_SKILL_ENEMY_RATE > 5) chrDisableReloadToLocation = true;
	characters[GetCharacterIndex("Ascold")].dialog.currentnode = "Ascold_KilledTemple";
	AddQuestRecord("Ascold", "11");
	AddQuestUserData("Ascold", "sSex", GetSexPhrase("","а"));
    //--------------------------- испанцы ------------------------------
	LAi_group_Delete("EnemyFight");
    int Rank = sti(pchar.rank) - 5 + MOD_SKILL_ENEMY_RATE;
	if (Rank < 1) Rank = 1;
 	sld = GetCharacter(NPC_GenerateCharacter("SpaEnemy_off", "SpaOfficer3", "man", "man", Rank, SPAIN, -1, true));
    FantomMakeCoolFighter(sld, sti(pchar.rank), 90, 90, "blade26", "pistol4", 120);
	LAi_SetActorType(sld);
    LAi_group_MoveCharacter(sld, "EnemyFight");
	ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto9");
	for (i=1; i<=8; i++)
    {
        sTemp = "sold_spa_"+(rand(7)+1);
		if (i==12) sTemp = "off_spa_1";
 		sld = GetCharacter(NPC_GenerateCharacter("SpaEnemy_"+i, sTemp, "man", "man", Rank, SPAIN, -1, true));
        FantomMakeCoolFighter(sld, sti(pchar.rank), 90, 90, "blade26", "pistol3", 50);
		DeleteAttribute(sld, "SuperShooter");
		sld.BreakTmplAndFightGroup = true;
		LAi_SetActorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto"+i);
    }
    for (i=10; i<=13; i++)
    {
        sld = GetCharacter(NPC_GenerateMusketeer("SpaMush_"+i, "spa_rifle_"+(rand(2)+1), "man", Rank, SPAIN, -1, false));
		if (MOD_SKILL_ENEMY_RATE > 5) TakeNItems(sld,"potion1", rand(MOD_SKILL_ENEMY_RATE)+1);
		sld.MusketerDistance = 0;
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto"+i);
	}
	//стравливаем
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, false);
	LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
	DoQuestFunctionDelay("Ascold_actorSetWarrior", 3.5);
}

void Ascold_actorSetWarrior(string qName)
{
	iTemp = GetCharacterIndex("SpaEnemy_off");
	if (iTemp != -1 && !LAi_IsDead(&characters[iTemp])) LAi_SetWarriorTypeNoGroup(&characters[iTemp]);
	for (i=1; i<=8; i++)
    {
		iTemp = GetCharacterIndex("SpaEnemy_"+i);
		if (iTemp != -1 && !LAi_IsDead(&characters[iTemp])) LAi_SetWarriorTypeNoGroup(&characters[iTemp]);
	}
}

void Ascold_LeifIsDead(string qName)
{
	LocatorReloadEnterDisable("Guadeloupe_Cave", "reload3_back", false);
	LAi_LocationDisableToughSkeletonTimer("Guadeloupe_Cave", -1);
	// Gregg: поднятых при переселении души добиваем вместе с ним
	int i, idx;
	for (i = 0; i < 2; i++)
	{
		idx = GetCharacterIndex("LeifDraugr_"+i);
		if (idx != -1) LAi_KillCharacter(&characters[idx]);
	}
}

// Gregg: 1/2 здоровья - переселение души. В диалоге Аззи сказано, что Лейф выучил у ацтеков перенос
// души между телами: тело подводит, но он поднимается в другом. Уходит он при этом НЕ к игроку, а
// назад к своей гробнице - туда, где лежало первое тело, и держит её.
//
// Локаторы задаём ИМЕНАМИ, а не поиском. cavernBig1 - две доли, разделённые стеной, но близкие в
// мировых координатах: LAi_FindNearestFreeLocator меряет по прямой и уводил Лейфа с драуграми за
// стену, в соседнюю долю. Ни "random", ни "random_must_be_near" здесь применять нельзя.
// locator1 - гробница (там Лейф стоит изначально), goto1/goto2/goto3 - рядом с ней, проверено в
// игре отладочным показом локаторов. goto3 держим свободным - под третьего драугра или отход.
void Leif_SoulJump()
{
	int idx = GetCharacterIndex("LeifEricson");
	if (idx < 0) return;
	ref rLeif = GetCharacter(idx);
	ref rDraugr;
	int i;
	string sLoc;
	Log_Info(GetConvertStrWithReplace("Variable_leif_souljump", "Logs.txt", "#space#", " "));
	LAi_CharacterPlaySound(rLeif, "Gr_LeifEricson");
	ChangeCharacterAddressGroup(rLeif, "Guadeloupe_Cave", "goto", "locator1");
	// SetStay влияет ТОЛЬКО на поведение без цели (LAi_warrior.c:302): стоять или бродить в поисках.
	// Увидев игрока, Лейф всё равно пойдёт в атаку - и это нормально, он берсерк, а не турель.
	// Бег через всю пещеру прекращает сам телепорт, а не этот флаг.
	LAi_warrior_SetStay(rLeif, true);
	// Новое тело - целое: восстанавливаем до 3/4. Не до полного намеренно: срабатывает прыжок на
	// половине, так что игроку пришлось бы снести 50% + 100% = полтора пула. При 3/4 выходит 1.25
	// пула - тело заметно свежее, но бой не удваивается. Хочешь сильнее - множитель ниже.
	float fMaxHP = LAi_GetCharacterMaxHP(rLeif);
	float fNewHP = fMaxHP * 0.75;
	LAi_SetHP(rLeif, fNewHP, fMaxHP);
	// Показываем цифру над ним - тем же способом, каким показывается вампирский хил Юм Самиля
	SendMessage(rLeif, "lfff", MSG_CHARACTER_VIEWDAMAGE, fNewHP, fNewHP, fMaxHP);
	// С ним поднимаются двое драугров. Группа EnemyFight - та же, что у Лейфа: внутри одной группы
	// урон не проходит, значит стравить их на него нельзя (LAi_ApplyCharacterAttackDamage).
	for (i = 0; i < 2; i++)
	{
		rDraugr = GetCharacter(NPC_GenerateCharacter("LeifDraugr_"+i, "Skel"+(rand(3)+1), "skeleton", "skeleton", sti(pchar.rank), PIRATE, 0, true));
		FantomMakeCoolFighter(rDraugr, sti(pchar.rank), 85, 85, "blade24", "", 100);
		LAi_SetHP(rDraugr, 250, 250); // ПОСЛЕ FantomMakeCoolFighter - тот прибавляет своё
		LAi_SetCurHPMax(rDraugr);
		LAi_SetWarriorType(rDraugr);
		LAi_group_MoveCharacter(rDraugr, "EnemyFight");
		sLoc = "goto1";
		if (i == 1) sLoc = "goto2";
		ChangeCharacterAddressGroup(rDraugr, "Guadeloupe_Cave", "goto", sLoc);
		LAi_SetFightMode(rDraugr, true);
	}
	LAi_SetCheckMinHP(rLeif, fMaxHP * 0.4, false, "Leif_Berserk");
}

// Gregg: 40% здоровья - берсерк. В отличие от Миктлантекутли, который в финале раскрывается,
// Лейф ранг не роняет: он не открывается, а звереет. Пропуск флинча + сквозное пробитие блока,
// травмы и пролом кирасы с 15 (от blade28) до 30, плюс ПОЛНОЕ восстановление энергии.
// Энергия здесь не косметика: от неё зависит темп атак и блоков (Lai_CharacterChangeEnergy,
// GetEnergyBladeDrain), и к этому моменту он её уже просадил. Полный бак = реальный всплеск
// натиска, а не только цифры в атрибутах.
void Leif_Berserk()
{
	int idx = GetCharacterIndex("LeifEricson");
	if (idx < 0) return;
	ref rLeif = GetCharacter(idx);
	Log_Info(GetConvertStrWithReplace("Variable_leif_berserk", "Logs.txt", "#space#", " "));
	LAi_CharacterPlaySound(rLeif, "Gr_LeifEricson");
	rLeif.chr_ai.energy = LAi_GetCharacterMaxEnergy(rLeif);
	SetBossDefense(rLeif, 0.0, 0.33, 100, true);
	SetBossOffense(rLeif, 20, 0, 30, 0);
	// valueT (травмы) хелпер не покрывает - обычно приходит от оружия, задаём напрямую
	rLeif.chr_ai.special.valueT = 30;
}

 void Ascold_OpenTheGrave(string qName)
{
	DoReloadCharacterToLocation("Guadeloupe_Cave", "reload", "reload5");
}

void DelivLettTortuga_Digress(string qName)
{
	LAi_ActorAnimation(characterFromId("AntonioDeBarras"), "attack_fast_1", "DelivLettTortuga_2KickPhar", 0.8);
}

void SeekIsabella_exitTown(string qName)
{
    DeleteAttribute(&locations[FindLocation("Beliz_ExitTown")], "DisableEncounters"); //энкаутеры вернем
	DoQuestCheckDelay("TalkSelf_Quest", 0.1); //диалог сам-на-сам
}

void Fr2Letter_shore22(string qName)
{
	if (characters[GetCharacterIndex("Bandit_1")].location == "Shore22")
	{
		for (i=1; i<=2; i++)
		{
			sld = characterFromId("Bandit_"+i);
			LAi_SetActorType(sld);
			LAi_ActorTurnToCharacter(sld, pchar);
		}
		LAi_SetActorType(pchar);
		SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.0);
	}
}

void Isabella_widding(string qName)
{
	pchar.quest.Romantic_Widding_Cancel.over = "yes"; //убираем таймер на вовремя не явлился
	pchar.RomanticQuest = "Widding";
	sld = characterFromID("Isabella");
	LAi_SetActorType(pchar);
    SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
	LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.0);
}

void FrLine_HoverGoOnPrison()
{
	pchar.questTemp.jailCanMove = true; //даем возможность выйти из тюрьмы
	LAi_SetPlayerType(pchar);
	pchar.questTemp.State = "Fr2Letter_SeekProblems";
	AddQuestRecord("Fra_Line_2_DelivLetter", "3");

	RestoreTempRemoveParam(pchar, "items");
	pchar.money = pchar.questTemp.money;
	DeleteAttribute(pchar, "questTemp.money");
	RefreshEquippedMaps(pchar);
	//==> странных типов помещаем в таверну
    LAi_group_Delete("PeaceGroup");
	for (i=1; i<=2; i++)
    {
		sTemp = "sit6";
		if (i==2) sTemp = "sit7";
		sld = GetCharacter(NPC_GenerateCharacter("Bandit_"+i, "pirate_"+i, "man", "man", 20, FRANCE, 5, true));
		FantomMakeCoolFighter(sld, 20, 70, 40, BLADE_NORMAL, "pistol3", 70);
		sld.Dialog.Filename = "Quest\FraLineNpc_1.c";
		LAi_SetSitType(sld);
		LAi_group_MoveCharacter(sld, "PeaceGroup");
		LAi_SetCheckMinHP(sld, sti(sld.chr_ai.hp)-1, false, "Fr2Letter_FightAfterShot");
		ChangeCharacterAddressGroup(sld, "Villemstad_tavern", "sit", sTemp);
	}
	sld = characterFromID("GoverFantom");
	LAi_SetActorType(sld);
    LAi_ActorGoToLocation(sld, "reload", "reload1", "none", "", "", "", 2.0);
	DoQuestFunctionDelay("FrLine_HoverGoOnPrison_2", 2.5);
}

void FrLine_HoverGoOnPrison_2(string qName)
{
	DoReloadCharacterToLocation("Villemstad_prison",  "goto", "goto12");
}

void HL5_portManGiveFleuts()
{
    pchar.questTemp.State = "ThreeFleutes_toLaVega";
    for (int j=1; j<=3; j++)
    {
        sld = GetCharacter(NPC_GenerateCharacter("Captain_"+j, "trader_"+(rand(15)+1), "man", "man", 15, HOLLAND, -1, true));
        FantomMakeCoolSailor(sld, SHIP_FLEUT, "", CANNON_TYPE_CANNON_LBS16, 30, 15, 15);
        FantomMakeCoolFighter(sld, 15, 10, 10, BLADE_SHORT, "pistol1", 0);
        sld.Ship.Cargo.Goods.Balls = 18;
        sld.Ship.Cargo.Goods.Grapes = 18;
        sld.Ship.Cargo.Goods.Knippels = 18;
        sld.Ship.Cargo.Goods.Bombs = 18;
        sld.Ship.Cargo.Goods.Weapon = 0;
        sld.Ship.Cargo.Goods.Powder = 50;
		sld.CompanionEnemyEnable = false;
		sld.Abordage.Enable = false; // НЕЛЬЗЯ!
		SetShipRemovable(sld, false);
		AddCharacterGoods(sld, GOOD_CLOTHES, 5000);
        AddCharacterGoods(sld, GOOD_FRUITS, 200);
        AddCharacterGoods(sld, GOOD_RUM, 900);
        SetCompanionIndex(pchar, -1, sti(sld.index));
        pchar.GenQuest.StoreGoods.StoreIdx = LaVega_STORE;
        pchar.GenQuest.StoreGoods.HL5_ThreeFleutes = true;
        Pchar.quest.ThreeFleutes_TalkIntoLaVega.win_condition.l1 = "location";
        Pchar.quest.ThreeFleutes_TalkIntoLaVega.win_condition.l1.location = "LaVega_town";
        Pchar.quest.ThreeFleutes_TalkIntoLaVega.win_condition = "ThreeFleutes_TalkIntoLaVega";
    }
}

void SeekShip_Stay(string qName)
{
	sld = &characters[sti(pchar.quest.(qName).Idx)];
	LAi_SetStayType(sld);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  разное с оптимизации   конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  шебека Синяя Птица   начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gregg: секция «шебека Синяя Птица» вынесена в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\reaction_functions\bluebird.c"
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  шебека Синяя Птица    конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Генератор жемчужного промысла Sharp    начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gregg: секция «Генератор жемчужного промысла Sharp» вынесена в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\reaction_functions\sharp.c"
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Генератор жемчужного промысла Sharp   конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gregg: семейство «Пиратская линейка» вынесено в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\Lines\pirates_line_functions.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////   Миниквесты        начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ----------------- Переспать с официанткой ---------------------
// Gregg: секция «Миниквесты» вынесена в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\reaction_functions\miniquests.c"
#include "quests\reaction_functions\inquisition.c" // Gregg: "Помощь святой инквизиции", перенос из К:ПС
#include "quests\reaction_functions\moonstone.c" // Gregg: "Легенда о призрачном свете", перенос из К:ПС
#include "quests\reaction_functions\ddprologue.c" // Gregg: альтернативный пролог «Необитаемый остров», перенос из К:ПС
#include "quests\reaction_functions\uneasydelivery.c" // Gregg: "Товар для Ричарда Тилльманна", перенос из К:ПС
#include "quests\reaction_functions\grifosajfus.c" // Gregg: "Предательство Грифо Сайфуса", перенос из К:ПС
#include "quests\reaction_functions\disappearpeople.c" // Gregg: "Исчезновение людей", перенос из К:ПС
#include "quests\reaction_functions\captainmonkey.c" // Gregg: "Корабль обезьян", перенос из К:ПС
#include "quests\reaction_functions\blackorchid.c" // Gregg: трилогия "Чёрная Орхидея", перенос из К:ПС
#include "quests\reaction_functions\twosisters.c" // Gregg: "Две сестры", глава 10 линии К:ПС

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////   Миниквесты        конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Линейка ГПК           начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gregg: секция «Линейка ГПК» вынесена в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\Lines\gpk_line_functions.c"
//<<<<<<-------- квест скелета Лейтона Декстера ----------------

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Линейка ГПК           конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Линейка Теночтитлана          начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//перец, что сваливает из Тено
// Gregg: секция «Линейка Теночтитлана» вынесена в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\reaction_functions\tenochtitlan.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Линейка Теночтитлана         конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Зачарованный город        начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gregg: секция «Зачарованный город» вынесена в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\reaction_functions\enchanted.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Зачарованный город     eddy     конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////   -- Линейка капитана Блада --     начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gregg: секция «Линейка капитана Блада» вынесена в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\Lines\captain_blood_line_functions.c"

// Gregg: семейство Slavetrader вынесено в отдельный файл (сплит god-файла reaction_functions.c).
#include "quests\reaction_functions\slavetrader.c"
//-->малява, грабеж торгашей
void Deliver_CreateTraderShips(string qName)//создание торговых кораблей
{
	int i, ShipType, Rank, iShipRank, iCannonType;
	int iGoods, iSpace;
	ref sld;
	string sTemp1, sTemp2, sNation;

	sTemp1 = pchar.questTemp.jailCanMove.Deliver.ShipName1;//имя корабля
	sTemp2 = pchar.questTemp.jailCanMove.Deliver.ShipName2;//имя корабля
	sNation = pchar.questTemp.jailCanMove.Deliver.Nation;
    Island_SetReloadEnableGlobal(pchar.questTemp.jailCanMove.Deliver.Island, false);
    Group_FindOrCreateGroup("Trade_Attack");//создать группу
	Group_SetType("Trade_Attack", "trade");//тип группы
	for (i=1; i<=2; i++)
	{
		if(makeint(pchar.rank) >= 20) { iShipRank = 3; }
		if(makeint(pchar.rank) >= 15 && makeint(pchar.rank) < 20) { iShipRank = rand(1)+2; }
		if(makeint(pchar.rank) >= 10 && makeint(pchar.rank) < 15) { iShipRank = rand(1)+1; }
		if(makeint(pchar.rank) >= 4 && makeint(pchar.rank) < 10) { iShipRank = rand(1); }
		if(makeint(pchar.rank) < 4) { iShipRank = 0; }
		switch (iShipRank)
		{
			case 0:
				ShipType = SHIP_BARQUE;
				Rank = 12 + rand(5);
				iCannonType = CANNON_TYPE_CANNON_LBS16;
			break;
			case 1:
				ShipType = SHIP_FLEUT;
				Rank = 18 + rand(5);
				iCannonType = CANNON_TYPE_CANNON_LBS16;
			break;
			case 2:
				ShipType = SHIP_PINNACE;
				Rank = 25 + rand(5);
				iCannonType = CANNON_TYPE_CANNON_LBS24;
			break;
			case 3:
				ShipType = SHIP_GALEON_H;
				Rank = 30 + rand(5);
				iCannonType = CANNON_TYPE_CANNON_LBS24;
			break;
		}
		sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, "trader_"+(rand(15)+1), "man", "man", Rank, sNation, 3, true));//создание кэпа
		if (i == 1)
		{
		FantomMakeCoolSailor(sld, ShipType, sTemp1, iCannonType, 48, 35, 35);//создание кораблей
		}
		if (i == 2)
		{
		FantomMakeCoolSailor(sld, ShipType, sTemp2, iCannonType, 44, 30, 30);//создание кораблей
		}
		sld.Ship.Mode = "trade";
		iGoods = sti(pchar.questTemp.jailCanMove.Deliver.Goods);
		iSpace = GetCharacterFreeSpace(sld, iGoods);
		iSpace = makeint(iSpace/2 + rand(iSpace/2));
		SetCharacterGoods(sld, iGoods, iSpace);
		Group_AddCharacter("Trade_Attack", "CaptainAttack_"+i);//добавление в группу
		sld.AlwaysEnemy = true;
		sld.ship.Crew.Morale = 60;
		ChangeCrewExp(sld, "Sailors", 50);
		ChangeCrewExp(sld, "Cannoners", 50);
		ChangeCrewExp(sld, "Soldiers", 100);
    }
    // ==> стравливание
	Group_SetGroupCommander("Trade_Attack", "CaptainAttack_1");
	Group_SetTaskAttack("Trade_Attack", PLAYER_GROUP);
	Group_SetPursuitGroup("Trade_Attack", PLAYER_GROUP);
	Group_SetAddress("Trade_Attack", pchar.questTemp.jailCanMove.Deliver.Island, "Quest_Ships", "Quest_Ship_" + (3+ rand(4)));
	Group_LockTask("Trade_Attack");

    // ==> прерывание на убиение эскадры
    pchar.quest.jailCanMoveDeliver_AfterBattle.win_condition.l1 = "Group_Death";//условие победы
	pchar.quest.jailCanMoveDeliver_AfterBattle.win_condition.l1.group = "Trade_Attack";//какую группу уничтожить
	pchar.quest.jailCanMoveDeliver_AfterBattle.function = "Deliver_AfterBattle";//это победа
	pchar.quest.jailCanMoveDeliver_DieHard.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.jailCanMoveDeliver_DieHard.function = "Deliver_DieHard";//это слинял
	pchar.quest.Deliver_TraderShipsOver.over = "yes";
	AddQuestRecord("GivePrisonFree", "13_1");
	AddQuestUserData("GivePrisonFree", "sSex", GetSexPhrase("ел","ла"));
	AddQuestUserData("GivePrisonFree", "sShipName1", pchar.questTemp.jailCanMove.Deliver.ShipName1);
	AddQuestUserData("GivePrisonFree", "sShipName2", pchar.questTemp.jailCanMove.Deliver.ShipName2);
}

void Deliver_AfterBattle(string qName)//реакция на победу
{
	Group_DeleteGroup("Trade_Attack");
	Island_SetReloadEnableGlobal(pchar.questTemp.jailCanMove.Deliver.Island, true);
    pchar.quest.jailCanMoveDeliver_DieHard.over = "yes";
	DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
	CloseQuestHeader("GivePrisonFree");
}

void Deliver_DieHard(string qName)//реакция на поражение
{
	Group_DeleteGroup("Trade_Attack");
    Island_SetReloadEnableGlobal(pchar.questTemp.jailCanMove.Deliver.Island, true);
    pchar.quest.jailCanMoveDeliver_AfterBattle.over = "yes";
	DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
	CloseQuestHeader("GivePrisonFree");
}

void Deliver_TraderShipsOver(string qName)//просроченный таймер торговых кораблей и кидалова - 2 в 1
{
	pchar.quest.jailCanMoveDeliver_ShipsAttack.over = "yes";
	AddQuestRecord("GivePrisonFree", "13_2");
	AddQuestUserData("GivePrisonFree", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("GivePrisonFree", "sShipName1", pchar.questTemp.jailCanMove.Deliver.ShipName1);
	AddQuestUserData("GivePrisonFree", "sShipName2", pchar.questTemp.jailCanMove.Deliver.ShipName2);
	DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
	CloseQuestHeader("GivePrisonFree");
}
//<--малява, грабеж торгашей

//-->малява, перехват курьера
void Deliver_CreateCureerShips(string qName)//создание курьерского корабля
{
	int i, ShipType, Rank, iShipRank, iCannonType;
	int iNation = sti(pchar.questTemp.jailCanMove.Deliver.Nation);
	ref sld;
	string Blade, sTemp, sNation;

	sTemp = pchar.questTemp.jailCanMove.Deliver.ShipName;//имя корабля
	sNation = pchar.questTemp.jailCanMove.Deliver.Nation;
    Island_SetReloadEnableGlobal(pchar.questTemp.jailCanMove.Deliver.Island, false);
    Group_FindOrCreateGroup("Cureer_Attack");//создать группу
	Group_SetType("Cureer_Attack", "war");//тип группы
		if(makeint(pchar.rank) >= 20) { iShipRank = 3; }
		if(makeint(pchar.rank) >= 12 && makeint(pchar.rank) < 20) { iShipRank = 2; }
		if(makeint(pchar.rank) >= 5 && makeint(pchar.rank) < 12) { iShipRank = 1; }
		if(makeint(pchar.rank) < 5) { iShipRank = 0; }
		switch (iShipRank)
		{
			case 0:
				ShipType = SHIP_SCHOONER;
				Rank = 12 + rand(5);
                Blade = "blade15";
				iCannonType = CANNON_TYPE_CANNON_LBS24;
			break;
			case 1:
				ShipType = SHIP_BRIGANTINE;
				Rank = 17 + rand(5);
                Blade = "blade34";
				iCannonType = CANNON_TYPE_CANNON_LBS24;
			break;
			case 2:
				ShipType = SHIP_CORVETTE;
				Rank = 22 + rand(5);
                Blade = "blade26";
				iCannonType = CANNON_TYPE_CANNON_LBS32;
			break;
			case 3:
				ShipType = SHIP_FRIGATE;
				Rank = 30 + rand(5);
                Blade = "blade28";
				iCannonType = CANNON_TYPE_CANNON_LBS32;
			break;
		}
		sld = GetCharacter(NPC_GenerateCharacter("CureerAttack", "off_" + NationShortName(iNation) + "_" + (rand(1) + 1), "man", "man", Rank, sNation, 3, true));//создание кэпа
		FantomMakeCoolSailor(sld, ShipType, sTemp, iCannonType, 75, 50, 50);//создание кораблей
		FantomMakeCoolFighter(sld, Rank, 40, 40, Blade, "pistol3", 40);//создание фантома кэпа
		Group_AddCharacter("Cureer_Attack", "CureerAttack");//добавление в группу
		DeleteAttribute(sld, "SaveItemsForDead");
		DeleteAttribute(sld, "DontClearDead");
		SetCharacterPerk(sld, "MusketsShoot");
		sld.AlwaysEnemy = true;

    // ==> стравливание
	Group_SetGroupCommander("Cureer_Attack", "CureerAttack");
	Group_SetTaskAttack("Cureer_Attack", PLAYER_GROUP);
	Group_SetPursuitGroup("Cureer_Attack", PLAYER_GROUP);
	Group_SetAddress("Cureer_Attack", pchar.questTemp.jailCanMove.Deliver.Island, "Quest_Ships", "Quest_Ship_" + (3+ rand(4)));
	Group_LockTask("Cureer_Attack");

    // ==> прерывание на убиение эскадры
    pchar.quest.jailCanMoveDeliver_AfterBattleC.win_condition.l1 = "Group_Death";//условие победы
	pchar.quest.jailCanMoveDeliver_AfterBattleC.win_condition.l1.group = "Cureer_Attack";//какую группу уничтожить
	pchar.quest.jailCanMoveDeliver_AfterBattleC.function = "DeliverC_AfterBattle";//это победа
	pchar.quest.jailCanMoveDeliver_DieHardC.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.jailCanMoveDeliver_DieHardC.function = "DeliverC_DieHard";//это слинял
	pchar.quest.Deliver_CureerShipsOver.over = "yes";
	AddQuestRecord("GivePrisonFree", "14_1");
	AddQuestUserData("GivePrisonFree", "sSex", GetSexPhrase("ел","ла"));
	AddQuestUserData("GivePrisonFree", "sShipName", pchar.questTemp.jailCanMove.Deliver.ShipName);
}

void DeliverC_AfterBattle(string qName)//реакция на победу
{
	Group_DeleteGroup("Cureer_Attack");
	Island_SetReloadEnableGlobal(pchar.questTemp.jailCanMove.Deliver.Island, true);
    pchar.quest.jailCanMoveDeliver_DieHardC.over = "yes";
	DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
	CloseQuestHeader("GivePrisonFree");
}

void DeliverC_DieHard(string qName)//реакция на поражение
{
	Group_DeleteGroup("Cureer_Attack");
    Island_SetReloadEnableGlobal(pchar.questTemp.jailCanMove.Deliver.Island, true);
    pchar.quest.jailCanMoveDeliver_AfterBattleC.over = "yes";
	DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
	CloseQuestHeader("GivePrisonFree");
}

void Deliver_CureerShipsOver(string qName)//просроченный таймер курьерского корабля
{
	pchar.quest.jailCanMoveDeliver_ShipsCAttack.over = "yes";
	AddQuestRecord("GivePrisonFree", "14_2");
	AddQuestUserData("GivePrisonFree", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("GivePrisonFree", "sShipName", pchar.questTemp.jailCanMove.Deliver.ShipName);
	DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
	CloseQuestHeader("GivePrisonFree");
}
//<--малява, перехват курьера

//-->малява, кидалово
void Deliver_lay(string qName)//пришли - а нету никого
{
	pchar.quest.Deliver_TraderShipsOver.over = "yes";
	AddQuestRecord("GivePrisonFree", "15");
	AddQuestUserData("GivePrisonFree", "sSex", GetSexPhrase("ел","ла"));
	AddQuestUserData("GivePrisonFree", "sSex1", GetSexPhrase("","а"));
	AddQuestUserData("GivePrisonFree", "sShipName1", pchar.questTemp.jailCanMove.Deliver.ShipName1);
	AddQuestUserData("GivePrisonFree", "sShipName2", pchar.questTemp.jailCanMove.Deliver.ShipName2);
	DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
	CloseQuestHeader("GivePrisonFree");
}
//<--малява, кидалово

/* Nathaniel (12.03.21): генераторный квест "Поиски корабля" ------------------------------------ */
void Findship_Over(string qName)//просрочили найти корабль на верфь
{
	AddQuestRecord("Findship", "2");
	CloseQuestHeader("Findship");
	DeleteAttribute(pchar, "GenQuest.Findship.Shipyarder");
}
/* ---------------------------------------------------------------------------------------------- */
//zagolski - побег офицера
void mOfficer_fc(string qName)
{
	if(GetCharacterIndex(Pchar.questTemp.MunityOfficerID) != -1) sld = CharacterFromID(Pchar.questTemp.MunityOfficerID);
	else return;
	if (IsEntity(worldMap))
	{
		if(sti(sld.Payment) == true)
		{
			if (sti(sld.ship.type) != SHIP_NOTUSED)
			{
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_reaction_functions_24", "Logs.txt", "#space#", " ") + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_25", "Logs.txt", "#space#", " ") + sld.ship.name + "");
				Pchar.questTemp.MunityOfficerID.begin = "0";
			}
			else
			{
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_reaction_functions_26", "Logs.txt", "#space#", " ") + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_27", "Logs.txt", "#space#", " ") + pchar.ship.name + "");
				if (GetCrewQuantity(pchar) > 0 && !IsEquipCharacterByArtefact(pchar, "talisman16"))
				{
					AddCharacterExpToSkill(PChar, "Leadership", 200);
					MunityOnShip("ShipMunity");
				}
				else Pchar.questTemp.MunityOfficerID.begin = "0";
			}
			sld.LifeDay = 0;
			RemovePassenger(Pchar, sld);
		}
		else Pchar.questTemp.MunityOfficerID.begin = "0";
	}
	else
	{
		pchar.quest.mOfficer_fc2.win_condition.l1 = "MapEnter";
		pchar.quest.mOfficer_fc2.function    = "mOfficer_fc2";
	}
}

void mOfficer_fc2(string qName)
{
	if(GetCharacterIndex(Pchar.questTemp.MunityOfficerID) != -1) sld = CharacterFromID(Pchar.questTemp.MunityOfficerID);
	else return;
	if(sti(sld.Payment) == true)
	{
		if (sti(sld.ship.type) != SHIP_NOTUSED)
		{
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_reaction_functions_28", "Logs.txt", "#space#", " ") + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_29", "Logs.txt", "#space#", " ") + sld.ship.name + "");
			Pchar.questTemp.MunityOfficerID.begin = "0";
		}
		else
		{
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_reaction_functions_30", "Logs.txt", "#space#", " ") + GetFullName(sld) + GetConvertStrWithReplace("Variable_reaction_functions_31", "Logs.txt", "#space#", " ") + pchar.ship.name + "");
			if (GetCrewQuantity(pchar) > 0 && !IsEquipCharacterByArtefact(pchar, "talisman16"))
			{
				AddCharacterExpToSkill(PChar, "Leadership", 200);
				MunityOnShip("ShipMunity");
			}
			else Pchar.questTemp.MunityOfficerID.begin = "0";
		}
		sld.LifeDay = 0;
		RemovePassenger(Pchar, sld);
	}
	else Pchar.questTemp.MunityOfficerID.begin = "0";
}

//zagolski. битва в бухте по квесту шарпа
void sharp_pre(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_group_Delete("EnemyFight");
    for (i=1; i<=5; i++)
    {
		sld = GetCharacter(NPC_GenerateCharacter("sh_pre_"+i, "pirate_"+i, "man", "man", 15, PIRATE, 0, true));
		FantomMakeCoolFighter(sld, 15, 70, 70, RandPhraseSimple("blade21","blade31"), RandPhraseSimple("pistol3", "pistol6"), 20);
		LAi_SetWarriorType(sld);
		LAi_group_MoveCharacter(sld, "EnemyFight");
		ChangeCharacterAddressGroup(sld, locations[FindLoadedLocation()].id, "goto", "goto4");
		LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
		LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
		LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
	}
}

// zagolski - опасный груз
void zpq_sld1_fc(string qName)
{
	LAi_group_Delete("EnemyFight");
	LAi_LocationFightDisable(loadedLocation, true);

    	for (i=1; i<=3; i++)
    	{
		sld = GetCharacter(NPC_GenerateCharacter("qp2_"+i, "pirate_"+i, "man", "man", 20, PIRATE, 0, true));
		sld.Dialog.Filename = "Quest\zpq_dialog.c";
		sld.dialog.currentnode = "zpq_sld2";
		sld.greeting = "Enc_Waiker";
		LAi_SetActorType(sld);
		LAi_group_MoveCharacter(sld, "EnemyFight");
		ChangeCharacterAddressGroup(sld, "Cumana_ExitTown", "goto", "goto5");
		if (i == 1)
		{
			FantomMakeCoolFighter(sld, 20, 90, 90, "blade31", "pistol6", 50);
			LAi_ActorDialog(sld, pchar, "", 3, 0);
			sld.cirassId = Items_FindItemIdx("cirass2");
			GiveItem2Character(sld, "jewelry4");
			AddMoneyToCharacter(sld, 1000+rand(1000));
			sld.SaveItemsForDead  = true;
			sld.DontClearDead = true;
			if (GetCharacterItem(pchar, "map_full") == 0)
			{
				if (GetCharacterItem(pchar, "map_part1") == 0) GiveItem2Character(sld, "map_part1");
				else
				{
					if (GetCharacterItem(pchar, "map_part2") == 0) GiveItem2Character(sld, "map_part2");
				}
			}
			else GiveItem2Character(sld, "map_normal");
		}
		else
		{
			FantomMakeCoolFighter(sld, 15, 55, 55, "blade7", "pistol3", 20);
			LAi_ActorFollow(sld, characterFromId("qp2_1"), "", -1);
		}
	}
}

void zpq_seaBattle(string qName)
{
	LAi_group_Delete("EnemyFight");
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("zpqCaptain", "Lil_Jim_0", "man", "man_fast", 40, PIRATE, 1, true));
	else sld = GetCharacter(NPC_GenerateCharacter("zpqCaptain", "Lil_Jim_0", "man", "man", 40, PIRATE, 1, true));
	FantomMakeCoolFighter(sld, 40, 105, 105, "blade23", "pistol3", 75*MOD_SKILL_ENEMY_RATE);
	FantomMakeCoolestSailor(sld, SHIP_HERCULES, GetConvertStr("Variable_reaction_functions_6", "ShipNames.txt"), CANNON_TYPE_CULVERINE_LBS20,  80, 80, 80);
    sld.name 	= GetConvertStr("Variable_reaction_functions_0.17", "Names.txt");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_177", "Names.txt", "#space#", " ");
	sld.id		= "zpqCaptain";
	sld.AlwaysSandbankManeuver = true;
	sld.DontRansackCaptain = true;
	sld.AnalizeShips = true;
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	sld.HeroModel = "Lil_Jim_0,Lil_Jim_1,Lil_Jim_2,Lil_Jim_3,Lil_Jim_4,Lil_Jim_5";
	SetCharacterPerk(sld, "MusketsShoot");

	sld.ship.Crew.Morale = 100;
	ChangeCrewExp(sld, "Sailors", 100);
	ChangeCrewExp(sld, "Cannoners", 100);
	ChangeCrewExp(sld, "Soldiers", 100);
	AddItems(sld, "talisman13", 1);
	sld.SaveItemsForDead = true;
	sld.DontClearDead = true;
	LAi_group_MoveCharacter(sld, "EnemyFight");
	Group_AddCharacter("zpq_Group", "zpqCaptain");
	Group_SetType("zpq_Group", "Pirate");
	Group_SetGroupCommander("zpq_Group", "zpqCaptain");
	Group_SetAddress("zpq_Group", "Cumana", "quest_ships", "quest_ship_4");
	Group_SetTaskAttack("zpq_Group", PLAYER_GROUP);
	Group_LockTask("zpq_Group");
}

//zagolski. начальный квест пирата
void pirateStartQuest_Smuggler_fc(string qName)
{
	if (sti(pchar.rank)<5 && sti(pchar.Nation) == PIRATE)
	{
	AddQuestRecord("pSQ", "4");
	chrDisableReloadToLocation = true;
	int iRank = makeint(sti(pchar.rank)+MOD_SKILL_ENEMY_RATE/2);

    	for (i=1; i<=3; i++)
    	{
		sld = GetCharacter(NPC_GenerateCharacter("psq1_"+i, "sold_"+NationShortName(sti(pchar.questTemp.pirateStartQuest.City2Nation))+"_"+(rand(4)+1), "man", "man", iRank, sti(pchar.questTemp.pirateStartQuest.City2Nation), 0, true));
		SetFantomParamFromRank(sld, iRank, true);
		LAi_SetWarriorType(sld);
		LAi_group_MoveCharacter(sld, "pirateStartQuest_Smuggler_group");
		ChangeCharacterAddressGroup(sld, pchar.questTemp.pirateStartQuest.City2 +"_ExitTown", "goto", LAi_FindNearestFreeLocator2Pchar("goto"));
	}
	LAi_group_SetRelation("pirateStartQuest_Smuggler_group", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("pirateStartQuest_Smuggler_group", LAI_GROUP_PLAYER, true);
	LAi_group_SetCheck("pirateStartQuest_Smuggler_group", "OpenTheDoors");
	}
}

void pirateStartQuest_Smuggler_fc2(string qName)
{
    	LAi_group_SetRelation("pirateStartQuest_Smuggler_group", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	int iRank = makeint(sti(pchar.rank)+MOD_SKILL_ENEMY_RATE/2);

    	for (i=1; i<=3; i++)
    	{
		sld = GetCharacter(NPC_GenerateCharacter("psq2_"+i, "pirate_"+i, "man", "man", iRank, PIRATE, -1, true));
		SetFantomParamFromRank(sld, iRank, true);
		sld.Dialog.Filename = "Quest\pSQ_Smuggler_dialog.c";
		sld.dialog.currentnode = "First time";
		sld.greeting = "Gr_Smuggler_OnShore";
		LAi_SetImmortal(sld, true);
		LAi_SetGuardianType(sld);
		LAi_group_MoveCharacter(sld, "pirateStartQuest_Smuggler_group");
		ChangeCharacterAddressGroup(sld, pchar.questTemp.pirateStartQuest.Shore, "Smugglers", "Smuggler0" + i);
	}
}

void pirateStartQuest_Lether_fc(string qName)
{
	if (GetCharacterItem(pchar, "letter_open") == 0)
	{
		GiveItem2Character(pchar, "letter_open");
		ChangeItemDescribe("letter_open", "itmdescr_letter_open_pirateStartQuest");
	}
	AddQuestRecord("pSQ", "6");
	AddQuestUserData("pSQ", "sSex", GetSexPhrase("ел","ла"));
	AddQuestUserData("pSQ", "sName", pchar.questTemp.pirateStartQuest.Name);
	SetTimerFunction("pirateStartQuest_Bahus_fc", 0, 0, 1);
}

void pirateStartQuest_Bahus_fc(string qName)
{
	pchar.quest.pirateStartQuest_Bahus_seaBattle.win_condition.l1 = "location";
	pchar.quest.pirateStartQuest_Bahus_seaBattle.win_condition.l1.location = pchar.questTemp.pirateStartQuest.Areal;
	pchar.quest.pirateStartQuest_Bahus_seaBattle.function = "pirateStartQuest_Bahus_seaBattle";
}

void pirateStartQuest_Bahus_seaBattle(string qName)
{
	sld = GetCharacter(NPC_GenerateCharacter("pSQB_Captain", "pirate_3", "man", "man", 6, PIRATE, 1, true));
	FantomMakeCoolFighter(sld, 6, 30, 30, "blade6", "pistol1", 30);
	FantomMakeCoolSailor(sld, SHIP_FLEUT, GetConvertStr("Variable_reaction_functions_5", "ShipNames.txt"), CANNON_TYPE_CANNON_LBS16, 50, 30, 30);
        sld.name 	= GetConvertStr("Variable_reaction_functions_0.18", "Names.txt");
        sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_178", "Names.txt", "#space#", " ");
	SetCharacterGoods(sld, GOOD_SLAVES, 300);
	SetCrewQuantity(sld, 129);
	sld.ship.Crew.Morale = 20;
	ChangeCrewExp(sld, "Sailors", 50);
	ChangeCrewExp(sld, "Cannoners", 30);
	ChangeCrewExp(sld, "Soldiers", 20);

	Group_AddCharacter("pSQB_Group", "pSQB_Captain");
	Group_SetType("pSQB_Group", "Pirate");
	Group_SetGroupCommander("pSQB_Group", "pSQB_Captain");
	Group_SetAddress("pSQB_Group", pchar.questTemp.pirateStartQuest.Areal, "quest_ships", "quest_ship_7");
	Group_LockTask("pSQB_Group");
}

void pirateStartQuest_seaBattle(string qName)
{
	LAi_group_SetRelation("pirateStartQuest_Smuggler_group", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	sld = GetCharacter(NPC_GenerateCharacter("pSQ_Captain", "pirate_6", "man", "man", 8, PIRATE, -1, true));
	FantomMakeCoolFighter(sld, 8, 40, 40, "blade9", "pistol3", 25*MOD_SKILL_ENEMY_RATE);
	FantomMakeCoolSailor(sld, SHIP_LUGGERQUEST, GetConvertStr("Variable_reaction_functions_4", "ShipNames.txt"), CANNON_TYPE_CANNON_LBS12, 40, 40, 40);
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.19", "Names.txt");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_179", "Names.txt", "#space#", " ");
	sld.AlwaysSandbankManeuver = true;
	sld.DontRansackCaptain = true;
	sld.AnalizeShips = true;

	sld.ship.Crew.Morale = 50;
	ChangeCrewExp(sld, "Sailors", 50);
	ChangeCrewExp(sld, "Cannoners", 50);
	ChangeCrewExp(sld, "Soldiers", 50);

	GiveItem2Character(sld, "cirass1");
	GiveItem2Character(sld, "jewelry9");
	sld.SaveItemsForDead = true;
	sld.DontClearDead = true;

	LAi_group_MoveCharacter(sld, "pirateStartQuest_Smuggler_group");
	Group_AddCharacter("pSQ_Group", "pSQ_Captain");
	Group_SetType("pSQ_Group", "Pirate");
	Group_SetGroupCommander("pSQ_Group", "pSQ_Captain");
	Group_SetAddress("pSQ_Group", pchar.questTemp.pirateStartQuest.Areal, "quest_ships", "quest_ship_4");
	Group_SetTaskAttack("pSQ_Group", PLAYER_GROUP);
	Group_LockTask("pSQ_Group");

	pchar.quest.pirateStartQuest_seaBattle_over.win_condition.l1 = "NPC_Death";
	pchar.quest.pirateStartQuest_seaBattle_over.win_condition.l1.character = "pSQ_Captain";
	pchar.quest.pirateStartQuest_seaBattle_over.function = "pirateStartQuest_seaBattle_over";
}

void pirateStartQuest_seaBattle_over(string qName)
{
	pchar.questTemp.pirateStartQuest = "5";
	AddQuestRecord("pSQ", "8");
	AddQuestUserData("pSQ", "sName", pchar.questTemp.pirateStartQuest.Name);
}

//--> ОЗГ, Карлос Кассир
void SelectCarlosCity() //выбираем город
{
	switch (rand(6))
	{
		case 0:
			pchar.questTemp.Headhunter.City = "Bridgetown";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_180", "Names.txt", "#space#", " ");
			break;
		case 1:
			pchar.questTemp.Headhunter.City = "Charles";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_181", "Names.txt", "#space#", " ");
			break;
		case 2:
			pchar.questTemp.Headhunter.City = "Tortuga";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_182", "Names.txt", "#space#", " ");
			break;
		case 3:
			pchar.questTemp.Headhunter.City = "Santodomingo";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_183", "Names.txt", "#space#", " ");
			break;
		case 4:
			pchar.questTemp.Headhunter.City = "Panama";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_184", "Names.txt", "#space#", " ");
			break;
		case 5:
			pchar.questTemp.Headhunter.City = "Portroyal";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_185", "Names.txt", "#space#", " ");
			break;
		case 6:
			pchar.questTemp.Headhunter.City = "Marigo";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_186", "Names.txt", "#space#", " ");
			break;
	}
}

void CreateCarlos()//создаем Карлоса
{
    ref sld, ch;
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Carlos", "ozg_carlos", "man", "man_fast", 20, PIRATE, -1, false));
	else sld = GetCharacter(NPC_GenerateCharacter("Carlos", "ozg_carlos", "man", "man", 20, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_187", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_188", "Names.txt", "#space#", " ");
    sld.dialog.filename = "Quest\Other_quests_NPC.c";
	sld.dialog.currentnode = "Carlos";
    sld.greeting = "pirat_common";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	sld.HeroModel = "ozg_carlos,ozg_carlos_1,ozg_carlos_2,ozg_carlos_3,ozg_carlos_4,ozg_carlos_5";
    FantomMakeCoolFighter(sld, 20, 70, 70, "blade21", "pistol3", 50);
    sld.DontClearDead = true;
    LAi_SetCitizenType(sld);

    if(pchar.questTemp.Headhunter.Chance == 1)
    {
   	    LAi_SetSitTypeNoGroup(sld);
        ChangeCharacterAddressGroup(sld, pchar.questTemp.Headhunter.City + "_tavern", "sit","sit2");
    }
    else
    {
   	    LAi_SetStayTypeNoGroup(sld);
        ChangeCharacterAddressGroup(sld, pchar.questTemp.Headhunter.City + "_Brothel_room", "goto","goto3");

		ch = GetCharacter(NPC_GenerateCharacter("Qhorse", "horse05", "woman", "towngirl", 10, ENGLAND, 1, false));
        LAi_SetActorTypeNoGroup(ch);
		ChangeCharacterAddressGroup(ch, pchar.questTemp.Headhunter.City + "_Brothel_room", "goto","goto1");
        int n = FindLocation(pchar.questTemp.Headhunter.City + "_Brothel");
        locations[n].reload.l2.disable = false;
    }
}

void CarlosDuel(string qName)
{
	pchar.quest.CarlosPlace.win_condition.l1 = "Location";
	pchar.quest.CarlosPlace.win_condition.l1.location = pchar.questTemp.Headhunter.City + "_ExitTown";
	pchar.quest.CarlosPlace.function = "CarlosPlace";
}

void CarlosDuelOver(string qName)//не пришли на стрелку
{
	pchar.quest.CarlosPlace.over = "yes";
	LAi_LocationDisableOfficersGen(pchar.questTemp.Headhunter.City + "_ExitTown", false);//офицеров пускать
	locations[FindLocation(pchar.questTemp.Headhunter.City + "_ExitTown")].DisableEncounters = false; //энкаутеры откроем
	sld = characterFromId("Carlos");
	sld.lifeDay = 0;
	AddQuestRecord("Headhunt", "2_2");
	AddQuestUserData("Headhunt", "sSex", GetSexPhrase("","ла"));
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	pchar.questTemp.Headhunter = "end_quest";
}

void CarlosPlace(string qName)
{
	pchar.quest.CarlosDuelOver.over = "yes";
	LAi_SetPlayerType(pchar);
	LAi_group_Delete("EnemyFight");
	iTemp = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE + 5; //это ранг от сложности
	for (i=2; i<=4; i++)
    {
		if (i == 4)
        {
			sld = GetCharacter(NPC_GenerateMusketeer("Carloshelper_"+i, "mushketer_1", "man", iTemp, PIRATE, -1, true));
            FantomMakeCoolFighter(sld, iTemp, 20, 20, "", "mushket", 50);
		}
		else
        {
			sld = GetCharacter(NPC_GenerateCharacter("Carloshelper_"+i, "Pirate_"+i, "man", "man", iTemp, PIRATE, -1, true));
			FantomMakeCoolFighter(sld, iTemp, 40, 40, "blade16", "pistol2", 50);
		}
		LAi_SetWarriorType(sld);
		LAi_warrior_DialogEnable(sld, false);
        LAi_group_MoveCharacter(sld, "EnemyFight");
        ChangeCharacterAddressGroup(sld, pchar.questTemp.Headhunter.City + "_ExitTown", "goto", "goto6");
    }
    sld = characterFromID("Carlos");
	sld.Dialog.Filename = "Quest\Other_Quests_NPC.c"; //диалоговый файл, из которого говорить будет
	sld.dialog.currentnode = "talk_off_town_Carlos"; // диалоговый узел
	if (CheckAttribute(sld, "CityType"))
	{
		DeleteAttribute(sld, "City"); // чтоб не было ругани с нацией
		DeleteAttribute(sld, "CityType");
	}
	ChangeCharacterAddressGroup(sld, pchar.questTemp.Headhunter.City + "_ExitTown", "goto", "goto1");
	LAi_SetActorType(pchar);
    LAi_SetActorType(sld);
    LAi_ActorTurnToCharacter(sld, pchar);
    SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
	LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
}
//<-- ОЗГ, Карлос Кассир

void SelectHoumCity()//выберем колонию для Холма
{
	switch (sti(pchar.questTemp.Headhunter.Chance))
	{
		case 0:
			pchar.questTemp.Headhunter.City = "Bridgetown";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_189", "Names.txt", "#space#", " ");
			pchar.questTemp.Headhunter.Island = "Barbados";
		break;
		case 1:
			pchar.questTemp.Headhunter.City = "Charles";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_190", "Names.txt", "#space#", " ");
			pchar.questTemp.Headhunter.Island = "Nevis";
		break;
		case 2:
			pchar.questTemp.Headhunter.City = "Sentjons";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_191", "Names.txt", "#space#", " ");
			pchar.questTemp.Headhunter.Island = "Antigua";
		break;
		case 3:
			pchar.questTemp.Headhunter.City = "Portspein";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_192", "Names.txt", "#space#", " ");
			pchar.questTemp.Headhunter.Island = "Trinidad";
		break;
		case 4:
			pchar.questTemp.Headhunter.City = "Portroyal";
			pchar.questTemp.Headhunter.Cityname = GetConvertStrWithReplace("Variable_reaction_functions_193", "Names.txt", "#space#", " ");
			pchar.questTemp.Headhunter.Island = "Jamaica";
		break;
	}
}

void CreateHoum()//создадим Холма
{
    ref sld;
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Houm", "ozg_horn", "man", "man_fast", 25, ENGLAND, -1, false));
	else sld = GetCharacter(NPC_GenerateCharacter("Houm", "ozg_horn", "man", "man", 25, ENGLAND, -1, false));
	FantomMakeCoolFighter(sld, 25, 50, 50, "blade19", "pistol6", 70);
	FantomMakeCoolSailor(sld, SHIP_GALEON_H, GetConvertStr("Variable_reaction_functions_7", "ShipNames.txt"), CANNON_TYPE_CANNON_LBS32, 70, 70, 70);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_194", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_195", "Names.txt", "#space#", " ");
    sld.dialog.filename = "Quest\Other_quests_NPC.c";
	sld.dialog.currentnode = "Houm";
    sld.greeting = "Gr_HeadPirates";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	sld.Ship.Mode = "pirate";
    DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");
    sld.AlwaysSandbankManeuver = true;
	sld.AnalizeShips = true;  //анализировать вражеские корабли при выборе таска
	sld.AlwaysEnemy = true;
	sld.HeroModel = "ozg_horn,ozg_horn_1,ozg_horn_2,ozg_horn_3,ozg_horn_4,ozg_horn_5";
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "GunProfessional");
	SetCharacterPerk(sld, "MusketsShoot");
	SetCharacterGoods(sld, GOOD_SILVER, 556);//положить в трюм серебро
	LAi_SetCitizenType(sld);
	LAi_SetImmortal(sld, true);
	sld.ship.Crew.Morale = 70;
	ChangeCrewExp(sld, "Sailors", 70);
	ChangeCrewExp(sld, "Cannoners", 70);
	ChangeCrewExp(sld, "Soldiers", 70);

    if(pchar.questTemp.Headhunter.Chance1 == 0)
    {
   	    LAi_SetSitTypeNoGroup(sld);
		FreeSitLocator(pchar.questTemp.Headhunter.City + "_tavern", "sit1");
        ChangeCharacterAddressGroup(sld, pchar.questTemp.Headhunter.City + "_tavern", "sit", "sit1");
    }
    else
    {
		LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
		ChangeCharacterAddressGroup(sld, pchar.questTemp.Headhunter.City + "_town", "quest", "quest1");
    }
}

void Headhunter_CreateHoumShips(string qName)//cоздание кораблей Хоума
{
	ref sld;
    Island_SetReloadEnableGlobal(pchar.questTemp.Headhunter.Island, false);
    Group_FindOrCreateGroup("Houm_Attack");//создать группу
	Group_SetType("Houm_Attack", "war");//тип группы

		sld = CharacterFromID("Houm");
		LAi_SetImmortal(sld, false);
		Group_AddCharacter("Houm_Attack", "Houm");//добавим Хоума в группу

		if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Houm_helper", "pirate_1", "man", "man_fast", 35, ENGLAND, -1, true));//создание кэпа-помощника
		else sld = GetCharacter(NPC_GenerateCharacter("Houm_helper", "pirate_1", "man", "man", 35, ENGLAND, -1, true));//создание кэпа-помощника
		sld.name = GetConvertStrWithReplace("Variable_reaction_functions_196", "Names.txt", "#space#", " ");
		sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_197", "Names.txt", "#space#", " ");
		FantomMakeCoolSailor(sld, SHIP_BRIG, "", CANNON_TYPE_CANNON_LBS16, 70, 70, 70);//создание брига
		FantomMakeCoolFighter(sld, 35, 60, 60, "blade15", "pistol5", 90);//создание фантома кэпа
		if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
		DeleteAttribute(sld, "SaveItemsForDead");
		DeleteAttribute(sld, "DontClearDead");
		sld.Ship.Mode = "pirate";
		sld.AlwaysEnemy = true;
		sld.Abordage.Enable = true;
		Group_AddCharacter("Houm_Attack", "Houm_helper");//добавление в группу

    // ==> стравливание
	Group_SetGroupCommander("Houm_Attack", "Houm");
	Group_SetTaskAttack("Houm_Attack", PLAYER_GROUP);
	Group_SetPursuitGroup("Houm_Attack", PLAYER_GROUP);
	Group_SetAddress("Houm_Attack", pchar.questTemp.Headhunter.Island, "quest_ships", "quest_ship_6");
	Group_LockTask("Houm_Attack");

    // ==> прерывание на убиение эскадры
    pchar.quest.Headhunter_AfterBattle.win_condition.l1 = "Group_Death";//условие победы
	pchar.quest.Headhunter_AfterBattle.win_condition.l1.group = "Houm_Attack";//какую группу уничтожить
	pchar.quest.Headhunter_AfterBattle.function = "Headhunter_HoumAttack_AfterBattle";//это победа
	pchar.quest.Headhunter_DieHard.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.Headhunter_DieHard.function = "Headhunter_HoumAttack_DieHard";//это слинял
}

void Headhunter_HoumAttack_AfterBattle(string qName)//реакция на победу
{
	Group_DeleteGroup("Houm_Attack");
	Island_SetReloadEnableGlobal(pchar.questTemp.Headhunter.Island, true);
    pchar.quest.Headhunter_DieHard.over = "yes";
	AddQuestRecord("Headhunt", "7");
	pchar.questTemp.Headhunter = "hunt_houm_yes";
}

void Headhunter_HoumAttack_DieHard(string qName)//реакция на поражение
{
	Group_DeleteGroup("Houm_Attack");
    Island_SetReloadEnableGlobal(pchar.questTemp.Headhunter.Island, true);
    pchar.quest.Headhunter_AfterBattle.over = "yes";
	AddQuestRecord("Headhunt", "8");
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	pchar.questTemp.Headhunter = "end_quest";
	ChangeOfficersLoyality("bad_all", 1);
}

void Headhunter_PinasseOver(string qName)//не сообразили, куда идти за 30 дней
{
	pchar.quest.Headhunter_Rattortuga.over = "yes";
	sld = characterFromId("RatOfficer");
	sld.lifeDay = 0;
	AddQuestRecord("Headhunt", "10");
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	pchar.questTemp.Headhunter = "end_quest";
}

void Headhunter_CreateRatpinasse(string qName)//создание пинаса Аделаида с перпомом
{
	pchar.quest.Headhunter_PinasseOver.over = "yes";
	ref sld;
	Group_FindOrCreateGroup("PinasseC");//создать группу
	Group_SetType("PinasseC", "war");//тип группы

	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("RatSeaOfficer", "officer_15", "man", "man_fast", 35, FRANCE, -1, true));//создание кэпа
	else sld = GetCharacter(NPC_GenerateCharacter("RatSeaOfficer", "officer_15", "man", "man", 35, FRANCE, -1, true));//создание кэпа
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_198", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_199", "Names.txt", "#space#", " ");
	FantomMakeCoolSailor(sld, SHIP_EASTINDIAMAN, GetConvertStr("Variable_reaction_functions_8", "ShipNames.txt"), CANNON_TYPE_CANNON_LBS16, 100, 100, 100);//создание кораблей
	FantomMakeCoolFighter(sld, 35, 80, 80, "blade15", "pistol2", 90);//создание фантома кэпа
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	sld.DontRansackCaptain = true;
	DeleteAttribute(sld, "AlwaysFriend");
	DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");
	sld.Ship.Mode = "pirate";
	Group_AddCharacter("PinasseC", "RatSeaOfficer");//добавление в группу
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "GunProfessional");
	SetCharacterPerk(sld, "MusketsShoot");
	sld.ship.Crew.Morale = 100;
	ChangeCrewExp(sld, "Sailors", 100);
	ChangeCrewExp(sld, "Cannoners", 100);
	ChangeCrewExp(sld, "Soldiers", 100);

	Group_SetGroupCommander("PinasseC", "RatSeaOfficer");
	Group_SetTaskAttack("PinasseC", PLAYER_GROUP);
	Group_SetAddress("PinasseC", "Tortuga", "quest_ships", "quest_ship_1");
	Group_LockTask("PinasseC");
	AddQuestRecord("Headhunt", "15");

    // ==> прерывание на убиение эскадры
    pchar.quest.HeadhunterC_AfterBattle.win_condition.l1 = "Group_Death";
	pchar.quest.HeadhunterC_AfterBattle.win_condition.l1.group = "PinasseC";
	pchar.quest.HeadhunterC_AfterBattle.function = "PinasseC_AfterBattle";
}

void PinasseC_AfterBattle(string qName)//реакция на досрочное уничтожение пинаса
{
	sld = characterFromId("RatOfficer");
	sld.lifeDay = 0;
	AddQuestRecord("Headhunt", "16");
	AddQuestUserData("Headhunt", "sSex", GetSexPhrase("","а"));
	ChangeCharacterHunterScore(PChar, "frahunter", 30); //начислить НЗГ
	pchar.questTemp.Headhunter = "end_quest";
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	Group_DeleteGroup("PinasseC");
}

void CreateRatOfficer()//посадим в таверну самого Крысу - не будем модель прятать
{
    ref sld;
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("RatOfficer", "ozg_piRat", "man", "man_fast", 10, PIRATE, -1, false));
	else sld = GetCharacter(NPC_GenerateCharacter("RatOfficer", "ozg_piRat", "man", "man", 10, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_200", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_201", "Names.txt", "#space#", " ");
    sld.dialog.filename = "Quest\Other_quests_NPC.c";
	sld.dialog.currentnode = "RatOfficer";
    sld.greeting = "pirat_common";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	sld.HeroModel = "ozg_piRat,ozg_piRat_1,ozg_piRat_2,ozg_piRat_3,ozg_piRat_4,ozg_piRat_5";
    GiveItem2Character(sld, "blade15");
	EquipCharacterbyItem(sld, "blade15");
    DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");
    LAi_SetCitizenType(sld);
   	LAi_SetSitTypeNoGroup(sld);
    ChangeCharacterAddressGroup(sld, "Tortuga_tavern", "sit","sit2");
}

void Headhunter_Ratgulf(string qName)//высадились в заливе
{
	pchar.quest.HeadhunterC_AfterBattle.over = "yes";
	Group_DeleteGroup("PinasseC");
	pchar.quest.Headhunter_RatgulfOver.over = "yes";
	if (GetCompanionQuantity(pchar) > 1 || 3-sti(RealShips[sti(pchar.ship.type)].Class) > 0)
	{
		AddQuestRecord("Headhunt", "19_1");
		AddQuestUserData("Headhunt", "sSex", GetSexPhrase("ся","ась"));
		CloseQuestHeader("Headhunt");
		pchar.questdata.Kondotier = 1;
		pchar.questTemp.Headhunter = "end_quest";
	}
	else
	{
		AddQuestRecord("Headhunt", "19");
		pchar.quest.HeadhunterRatgulfTimer.win_condition.l1 = "Timer";
		pchar.quest.HeadhunterRatgulfTimer.win_condition.l1.date.hour  = sti(GetTime() + 24);
		pchar.quest.HeadhunterRatgulfTimer.win_condition.l1.date.day   = GetAddingDataDay(0, 0, rand(1));
		pchar.quest.HeadhunterRatgulfTimer.win_condition.l1.date.month = GetAddingDataMonth(0, 0, rand(1));
		pchar.quest.HeadhunterRatgulfTimer.win_condition.l1.date.year  = GetAddingDataYear(0, 0, rand(1));
		pchar.quest.HeadhunterRatgulfTimer.function = "Headhunter_CreateRatTruepinasse_prepare";
		SetFunctionTimerCondition("Headhunter_Ratgulf2Over", 0, 0, 5, false);
	}
}

void Headhunter_CreateRatTruepinasse_prepare(string qName)
{
	pchar.quest.Headhunter_RatgulfPinasse.win_condition.l1 = "location";
    pchar.quest.Headhunter_RatgulfPinasse.win_condition.l1.location = "Beliz";//отправляем в локацию
    pchar.quest.Headhunter_RatgulfPinasse.function = "Headhunter_CreateRatTruepinasse";
}

void Headhunter_RatOfficerOver(string qName)//просрочили поговорить с Крысой
{
	pchar.quest.HeadhunterC_AfterBattle.over = "yes";
	Group_DeleteGroup("PinasseC");
	sld = characterFromId("RatOfficer");
	sld.dialog.filename = "Quest\Other_quests_NPC.c";
	sld.dialog.currentnode = "RatOfficer_5";
	sld.lifeDay = 0;
	AddQuestRecord("Headhunt", "17_1");
	AddQuestUserData("Headhunt", "sSex", GetSexPhrase("ся","ась"));
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	pchar.questTemp.Headhunter = "end_quest";
}

void Headhunter_RatgulfOver(string qName)//просрочили дойти до залива
{
	pchar.quest.HeadhunterC_AfterBattle.over = "yes";
	pchar.quest.Headhunter_Ratgulf.over = "yes";
	Group_DeleteGroup("PinasseC");
	AddQuestRecord("Headhunt", "20");
	AddQuestUserData("Headhunt", "sSex", GetSexPhrase("ся","лась"));
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	pchar.questTemp.Headhunter = "end_quest";
}

void Headhunter_Ratgulf2Over(string qName)//страдали фигней вместо поисков после высадки в заливе
{
	pchar.quest.Headhunter_RatgulfPinasse.over = "yes";
	AddQuestRecord("Headhunt", "20_1");
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	pchar.questTemp.Headhunter = "end_quest";
}

void Headhunter_CreateRatTruepinasse(string qName)//создание пинаса Аделаида с Крысой
{
	pchar.quest.Headhunter_Ratgulf2Over.over = "yes";
	ref sld;
	int hcrew;
	Group_FindOrCreateGroup("PinasseT");//создать группу
	Group_SetType("PinasseT", "war");//тип группы
	Island_SetReloadEnableGlobal("Beliz", false);

	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("RatCaptain", "ozg_piRat", "man", "man_fast", 25, PIRATE, -1, true)); // LEO: Превозмогаторам страдать 08.12.2021
	else sld = GetCharacter(NPC_GenerateCharacter("RatCaptain", "ozg_piRat", "man", "man", 25, PIRATE, -1, true));//создание кэпа
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_202", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_203", "Names.txt", "#space#", " ");
	sld.DontRansackCaptain = true;
	sld.HeroModel = "ozg_piRat,ozg_piRat_1,ozg_piRat_2,ozg_piRat_3,ozg_piRat_4,ozg_piRat_5";
	FantomMakeCoolSailor(sld, SHIP_EASTINDIAMAN, GetConvertStr("Variable_reaction_functions_9", "ShipNames.txt"), CANNON_TYPE_CANNON_LBS16, 80, 80, 80);//создание кораблей
	FantomMakeCoolFighter(sld, 25, 70, 70, "blade15", "pistol6", 100);//создание фантома кэпа
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");
	sld.Ship.Mode = "pirate";
	Group_AddCharacter("PinasseT", "RatCaptain");//добавление в группу
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "GunProfessional");
	SetCharacterPerk(sld, "MusketsShoot");
	hcrew = GetMaxCrewQuantity(sld);
	SetCrewQuantity(sld, hcrew);

	sld.ship.Crew.Morale = 100;
	ChangeCrewExp(sld, "Sailors", 100);
	ChangeCrewExp(sld, "Cannoners", 100);
	ChangeCrewExp(sld, "Soldiers", 100);

	// ==> стравливание
	Group_SetGroupCommander("PinasseT", "RatCaptain");
	Group_SetTaskAttack("PinasseT", PLAYER_GROUP);
	Group_SetAddress("PinasseT", "Beliz", "quest_ships", "Quest_ship_8");
	Group_LockTask("PinasseT");
	AddQuestRecord("Headhunt", "21");

    // ==> прерывание на убиение эскадры
    pchar.quest.Headhunter_AfterBattle.win_condition.l1 = "Group_Death";
	pchar.quest.Headhunter_AfterBattle.win_condition.l1.group = "PinasseT";
	pchar.quest.Headhunter_AfterBattle.function = "Headhunter_Pinassetrue_AfterBattle";
	pchar.quest.Headhunter_DieHard.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.Headhunter_DieHard.function = "Headhunter_Pinassetrue_DieHard";//это слинял
}

void Headhunter_Pinassetrue_DieHard(string qName)//реакция на поражение
{
	Group_DeleteGroup("PinasseT");
    Island_SetReloadEnableGlobal("Beliz", true);
    pchar.quest.Headhunter_AfterBattle.over = "yes";
	AddQuestRecord("Headhunt", "22");
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	pchar.questTemp.Headhunter = "end_quest";
	ChangeOfficersLoyality("bad_all", 1);
}

void Headhunter_Pinassetrue_AfterBattle(string qName)//реакция на победу
{
	if (!CheckAttribute(pchar, "GenQuest.LastQuestPrisonerIdx"))
	{
		Group_DeleteGroup("PinasseT");
		Island_SetReloadEnableGlobal("Beliz", true);
		pchar.quest.Headhunter_DieHard.over = "yes";
		AddQuestRecord("Headhunt", "24");
		pchar.questTemp.Headhunter = "hunt_rat_yes2";
	}
	else
	{
		characters[sti(pchar.GenQuest.LastQuestPrisonerIdx)].id = "RatCaptain"; //базар в каюте, проверка ИД кэпа
	}
}

void Headhunter_HalenOver(string qName)//опоздали за 10 дней до Кюрасао
{
	AddQuestRecord("Headhunt", "26_1");
	AddQuestUserData("Headhunt", "sSex", GetSexPhrase("","а"));
	CloseQuestHeader("Headhunt");
	pchar.questdata.Kondotier = 1;
	pchar.questTemp.Headhunter = "end_quest";
}

void RatHunters_inRoom(string qName)//идем в комнату
{
	chrDisableReloadToLocation = true;
	LocatorReloadEnterDisable(locations[reload_location_index].fastreload + "_tavern", "reload2_back", true);
	DoQuestCheckDelay("TalkSelf_Quest", 5.0);
}

void RatHunters_Sleep()//спим в комнате
{
	SetLaunchFrameFormParam(GetConvertStr("Variable_reaction_functions_5", "Frames.txt"), "", 0, 6);
    LaunchFrameForm();
    WaitDate("", 0, 0, 0, 8, 10); //крутим время
    RecalculateJumpTable();
	DoQuestFunctionDelay("RatHunters_Enter", 0.5);
}

void RatHunters_Enter(string qName)//пришли киллеры
{
	LAi_SetFightMode(pchar, false);
	LAi_LockFightMode(pchar, true);
	int iTemp = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE;
	for (i=1; i<=2; i++)
	{
		if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)ref sld = GetCharacter(NPC_GenerateCharacter("RatHunters"+i, "officer_7", "man", "man_fast", iTemp, PIRATE, -1, true)); // LEO: Превозмогаторам страдать 08.12.2021
		else sld = GetCharacter(NPC_GenerateCharacter("RatHunters"+i, "officer_7", "man", "man", iTemp, PIRATE, -1, true));
		FantomMakeCoolFighter(sld, iTemp, 50, 50, "topor2", "pistol5", 50);
		LAi_SetActorType(sld);
		if (i == 1)
		{
			LAi_CharacterDisableDialog(sld);
         	ChangeCharacterAddressGroup(sld, "Villemstad_tavern_upstairs", "goto", "goto2");
		}
		else
		{
			sld.dialog.Filename = "Quest\Other_quests_NPC.c";
			sld.dialog.currentnode = "RatHunters";
			sld.greeting = "Gr_HUNTER";
			if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
         	ChangeCharacterAddressGroup(sld, "Villemstad_tavern_upstairs", "goto", "goto3");
			LAi_ActorDialog(sld, pchar, "", 2.0, 0);
		}
		LAi_group_MoveCharacter(sld, "EnemyFight");
	}
	DeleteAttribute(sld, "SaveItemsForDead");
	DeleteAttribute(sld, "DontClearDead");
	LAi_group_MoveCharacter(sld, "EnemyFight");
}

void CreateHalen()//создаем Халена и ближайшее окружение
{
    ref sld;
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Halen", "ozg_martin", "man", "man_fast", 30, HOLLAND, -1, false)); // LEO: Превозмогаторам страдать 08.12.2021
	else sld = GetCharacter(NPC_GenerateCharacter("Halen", "ozg_martin", "man", "man", 30, HOLLAND, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_204", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_205", "Names.txt", "#space#", " ");
    sld.dialog.filename = "Quest\Other_quests_NPC.c";
	sld.dialog.currentnode = "Halen";
    sld.greeting = "pirat_common";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
    FantomMakeCoolFighter(sld, 30, 70, 70, "blade34", "pistol6", 100);
    ChangeCharacterAddressGroup(sld, "Villemstad_town", "reload", "reload3");
	sld.SaveItemsForDead = true;
	sld.DontClearDead = true;
	sld.HeroModel = "ozg_martin,ozg_martin_1,ozg_martin_2,ozg_martin_3,ozg_martin_4,ozg_martin_5";
	GiveItem2Character(sld, "cirass5");
	EquipCharacterbyItem(sld, "cirass5");
	// LEO: у чекера СВОЁ имя. Раньше и он, и location-условие ниже звались Headhunter_FightInPort,
	//   а QuestComplete при пустом qname сперва смотрит pchar.quest.<имя>.function - и удар по Халену
	//   уходил в Headhunter_SeabattleInPort (корвет в море) вместо кейса, который отпирает ворота.
	LAi_SetCheckMinHP(sld, (LAi_GetCharacterHP(sld) - 1), false, "Headhunter_HalenHit");//сработает на атаку Халена
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", "reload1_back", "none", "", "", "Headhunter_HalenOut", -1);//сработает, если Хален уйдет
	LAi_Actor2WaitDialog(sld, pchar);
	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
	LocatorReloadEnterDisable("Villemstad_town", "gate_back", true);
	LocatorReloadEnterDisable("Villemstad_town", "reload1_back", true);
	LocatorReloadEnterDisable("Villemstad_town", "reload2_back", true);
	for (i=1; i<=10; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("AddSolder"+i, "Sold_hol_"+(rand(7)+1), "man", "man_fast", 20, HOLLAND, 1, true));
		sld.Dialog.Filename = "Common_Soldier.c";
		sld.city = "Villemstad";
		sld.citytype = "soldier";
		FantomMakeCoolFighter(sld, 20, 50, 50, "blade16", "pistol3", 50);
        LAi_SetPatrolType(sld);
		int iLocatorNum = i + 10;
		ChangeCharacterAddressGroup(sld, "Villemstad_town", "patrol", "patrol"+iLocatorNum);
		LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
	}
	LAi_group_SetLookRadius("HOLLAND_CITIZENS", 100);
	LAi_group_SetHearRadius("HOLLAND_CITIZENS", 100);
	pchar.quest.Headhunter_FightInPort.win_condition.l1 = "location";
	pchar.quest.Headhunter_FightInPort.win_condition.l1.location = "Curacao";
	pchar.quest.Headhunter_FightInPort.function = "Headhunter_SeabattleInPort";
}

// LEO: CreateHalen запирает три выхода Виллемстада; отпирали только удар по Халену и его уход. Уйдя в обход
//   (картой-телепортом) или до фикса 10.09.2026, квест заканчивали с городом, запертым навсегда.
void Headhunter_ReleaseVillemstadExits()
{
	LocatorReloadEnterDisable("Villemstad_town", "gate_back", false);
	LocatorReloadEnterDisable("Villemstad_town", "reload1_back", false);
	LocatorReloadEnterDisable("Villemstad_town", "reload2_back", false);
}

// LEO: лечение начатых партий (зовётся из OnLoad). Отпираем, только если заперт именно набор Халена, а
//   сцены, которой он принадлежит, уже нет. Тот же набор ставит поиск шпиона губернатора - его не трогаем.
void Headhunter_HealVillemstadExits()
{
	int iTown = FindLocation("Villemstad_town");
	if (iTown < 0) return;
	string sKey = UD_FindReloadKeyByName("Villemstad_town", "reload1_back");
	if (sKey == "") return;
	if (!CheckAttribute(&Locations[iTown], "reload." + sKey + ".disable")) return;
	if (sti(Locations[iTown].reload.(sKey).disable) == 0) return;

	int iHalen = GetCharacterIndex("Halen");
	if (iHalen != -1)
	{
		if (!LAi_IsDead(&Characters[iHalen]) && Characters[iHalen].location == "Villemstad_town") return;
	}
	if (CheckAttribute(pchar, "GenQuest.SeekSpy"))
	{
		if (pchar.GenQuest.SeekSpy == "FoundSpy") return;
	}
	Headhunter_ReleaseVillemstadExits();
}

void Headhunter_SeabattleInPort(string qName)//создание корвета Халена
{
	ref sld;
	Headhunter_ReleaseVillemstadExits(); // в море - значит город покинут, сцена в нём окончена
	Group_FindOrCreateGroup("Halen_frigate");//создать группу
	Group_SetType("Halen_frigate", "war");//тип группы

	if (GetCharacterIndex("Halen") == -1)
	{
		if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Halen1", "officer_17", "man", "man_fast", 25, HOLLAND, -1, true)); // LEO: Превозмогаторам страдать 08.12.2021
		else sld = GetCharacter(NPC_GenerateCharacter("Halen1", "officer_17", "man", "man", 25, HOLLAND, -1, true));
		if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	}
	else
	{
		if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Halen1", "ozg_martin", "man", "man_fast", 25, HOLLAND, -1, true)); // LEO: Превозмогаторам страдать 08.12.2021
		else sld = GetCharacter(NPC_GenerateCharacter("Halen1", "ozg_martin", "man", "man", 25, HOLLAND, -1, true));//создание кэпа
		sld.name = GetConvertStrWithReplace("Variable_reaction_functions_206", "Names.txt", "#space#", " ");
		sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_207", "Names.txt", "#space#", " ");
		sld.HeroModel = "ozg_martin,ozg_martin_1,ozg_martin_2,ozg_martin_3,ozg_martin_4,ozg_martin_5";
		if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	}
	sld.DontRansackCaptain = true;
	FantomMakeCoolSailor(sld, SHIP_CORVETTE, GetConvertStr("Variable_reaction_functions_10", "ShipNames.txt"), CANNON_TYPE_CANNON_LBS32, 70, 70, 70);//создание кораблей
	FantomMakeCoolFighter(sld, 30, 70, 70, "blade34", "pistol6", 100);//создание фантома кэпа
	sld.Ship.Mode = "war";
	Group_AddCharacter("Halen_frigate", "Halen1");//добавление в группу
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "GunProfessional");
	SetCharacterPerk(sld, "MusketsShoot");
	if (GetCharacterIndex("Halen") != -1)
	{
		GiveItem2Character(sld, "cirass5");
	}
	sld.ship.Crew.Morale = 80;
	ChangeCrewExp(sld, "Sailors", 80);
	ChangeCrewExp(sld, "Cannoners", 80);
	ChangeCrewExp(sld, "Soldiers", 80);
	// LEO: БАГФИКС - ставили только ЗАДАЧУ атаковать, но не ОТНОШЕНИЕ, а весь морской бой гейтится именно им:
	//   голландцы без войны с ГГ не стреляли и не шли на абордаж. Снимается в Headhunter_ClearHalenEnmity.
	sld.AlwaysEnemy = true;
	SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);

	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Halen_helper", "off_hol_2", "man", "man_fast", 25, HOLLAND, -1, true)); // LEO: Превозмогаторам страдать 08.12.2021
	else sld = GetCharacter(NPC_GenerateCharacter("Halen_helper", "off_hol_2", "man", "man", 25, HOLLAND, -1, true));//создание кэпа
	sld.DontRansackCaptain = true;
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	FantomMakeCoolSailor(sld, SHIP_FRIGATE, "", CANNON_TYPE_CANNON_LBS32, 50, 50, 50);//призовем на помощь фрегат
	FantomMakeCoolFighter(sld, 35, 60, 60, "blade15", "pistol2", 90);//создание фантома кэпа
	sld.Ship.Mode = "war";
	sld.AlwaysEnemy = true; // LEO: та же вражда, что и флагману - иначе помощник просто не воевал
	SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
	Group_AddCharacter("Halen_frigate", "Halen_helper");//добавление в группу

	// ==> стравливание
	Group_SetGroupCommander("Halen_frigate", "Halen1");
	Group_SetTaskAttack("Halen_frigate", PLAYER_GROUP);
	Group_SetAddress("Halen_frigate", "Curacao", "quest_ships", "Quest_ship_2");
	Group_LockTask("Halen_frigate");

    // ==> прерывание на убиение эскадры
    pchar.quest.Headhunter_AfterBattle.win_condition.l1 = "Group_Death";
	pchar.quest.Headhunter_AfterBattle.win_condition.l1.group = "Halen_frigate";
	pchar.quest.Headhunter_AfterBattle.function = "Headhunter_Halen_frigate_AfterBattle";
	pchar.quest.Headhunter_DieHard.win_condition.l1 = "MapEnter";//условие на слинять
    pchar.quest.Headhunter_DieHard.function = "Headhunter_Halen_frigate_DieHard";//это слинял
}


// LEO: Halen1/Halen_helper - ИМЕНОВАННЫЕ слоты с LifeDay=-1: не истекают, так что вражда осела бы в сейве
//   навсегда. Слоты при этом НЕ освобождать - DieHard ниже решает исход по GetCharacterIndex("Halen1").
void Headhunter_ClearHalenEnmity()
{
	ref chr;
	if (GetCharacterIndex("Halen1") >= 0)
	{
		chr = CharacterFromID("Halen1");
		SetCharacterRelationBoth(sti(chr.index), GetMainCharacterIndex(), RELATION_NEUTRAL);
		DeleteAttribute(chr, "AlwaysEnemy");
	}
	if (GetCharacterIndex("Halen_helper") >= 0)
	{
		chr = CharacterFromID("Halen_helper");
		SetCharacterRelationBoth(sti(chr.index), GetMainCharacterIndex(), RELATION_NEUTRAL);
		DeleteAttribute(chr, "AlwaysEnemy");
	}
}

void Headhunter_Halen_frigate_DieHard(string qName)//ушли на карту
{
	Headhunter_ClearHalenEnmity(); // LEO: убрать за собой вражду
	Group_DeleteGroup("Halen_frigate");
    Island_SetReloadEnableGlobal("Curacao", true);
    pchar.quest.Headhunter_AfterBattle.over = "yes";
	if (GetCharacterIndex("Halen") == -1 || GetCharacterIndex("Halen1") == -1)
	{
		AddQuestRecord("Headhunt", "31");
		pchar.questTemp.Headhunter = "hunt_halen_yes";
		ChangeCharacterHunterScore(PChar, "holhunter", 30);
	}
	else
	{
		AddQuestRecord("Headhunt", "30");
		CloseQuestHeader("Headhunt");
		pchar.questdata.Kondotier = 1;
		pchar.questTemp.Headhunter = "end_quest";
		ChangeOfficersLoyality("bad_all",1);
	}
	characters[GetCharacterIndex("Halen")].lifeDay = 0;
}

void Headhunter_Halen_frigate_AfterBattle(string qName)//потопили корвет
{
	Headhunter_ClearHalenEnmity(); // LEO: убрать за собой вражду
	Group_DeleteGroup("Halen_frigate");
	Island_SetReloadEnableGlobal("Curacao", true);
    pchar.quest.Headhunter_DieHard.over = "yes";
	AddQuestRecord("Headhunt", "31");
	pchar.questTemp.Headhunter = "hunt_halen_yes";
	ChangeCharacterHunterScore(PChar, "holhunter", 30);
	characters[GetCharacterIndex("Halen")].lifeDay = 0;
}

void CreateMiko()//создадим Мико Пистольеро
{
    ref sld;
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("Miko", "ozg_miko", "man", "man_fast", 40, SPAIN, -1, false)); // LEO: Превозмогаторам страдать 08.12.2021
	else sld = GetCharacter(NPC_GenerateCharacter("Miko", "ozg_miko", "man", "man", 40, SPAIN, -1, false));
	FantomMakeCoolFighter(sld, 40, 50, 100, "blade22", "pistol4", 150);
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_208", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_209", "Names.txt", "#space#", " ");
    sld.dialog.filename = "Quest\Other_quests_NPC.c";
	sld.dialog.currentnode = "Miko";
    sld.greeting = "pirat_common";
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	sld.HeroModel = "ozg_miko,ozg_miko_1,ozg_miko_2,ozg_miko_3,ozg_miko_4,ozg_miko_5";
	sld.SaveItemsForDead = true;
	sld.DontClearDead = true;
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "GunProfessional");
	GiveItem2Character(sld, "indian10");
	GiveItem2Character(sld, "Totem_5");
	GiveItem2Character(sld, "sculMa2");
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "Pearlgroup_2");
	ChangeCharacterAddressGroup(sld, "Pearl_Town_2", "goto", "goto1");
	for (i=2; i<=5; i++)//добавим охраны в поселение
	{
		if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter("AddPearlguard"+i, "Pirate_"+(rand(24)+1), "man", "man_fast", 25, SPAIN, -1, true)); // LEO: Превозмогаторам страдать 08.12.2021
		else sld = GetCharacter(NPC_GenerateCharacter("AddPearlguard"+i, "Pirate_"+(rand(24)+1), "man", "man", 25, SPAIN, -1, true));
		sld.Dialog.Filename = "Pearl_dialog.c";
		sld.dialog.currentnode = "PearlMan";
		sld.city = "Pearl_Town_2";
		sld.citytype = "soldier";
		sld.greeting = "pirat_common";
		if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
		FantomMakeCoolFighter(sld, 25, 50, 50, "blade16", "pistol3", 100);
        LAi_SetPatrolType(sld);
		ChangeCharacterAddressGroup(sld, "Pearl_Town_2", "goto", "goto"+i);
		LAi_group_MoveCharacter(sld, "Pearlgroup_2");
	}
	LAi_group_SetLookRadius("Pearl_Town_2", 50);
	LAi_group_SetHearRadius("Pearl_Town_2", 50);
}

void Miko_die(string qName)
{
	chrDisableReloadToLocation = false;
	pchar.quest.Headhunter_Jahunters.win_condition.l1 = "location";
	pchar.quest.Headhunter_Jahunters.win_condition.l1.location = "shore55";
	pchar.quest.Headhunter_Jahunters.function = "Headhunter_Jahunters";
}

void Headhunter_Jahunters(string qName)//наймиты Джа в бухте
{
	int iTemp = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE + 5;
	chrDisableReloadToLocation = true;
	for (i=1; i<=6; i++)
	{
		if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)ref sld = GetCharacter(NPC_GenerateCharacter("JaHunters"+i, "officer_7", "man", "man_fast", iTemp, PIRATE, -1, true)); // LEO: Превозмогаторам страдать 08.12.2021
		else sld = GetCharacter(NPC_GenerateCharacter("JaHunters"+i, "officer_7", "man", "man", iTemp, PIRATE, -1, true));
		FantomMakeCoolFighter(sld, 25, 60, 60, "topor2", "pistol6", 70);
		LAi_SetActorType(sld);
		if (i == 1)
		{
			sld.dialog.Filename = "Quest\Other_quests_NPC.c";
			sld.dialog.currentnode = "JaHunters";
			sld.greeting = "Gr_HUNTER";
			if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
			ChangeCharacterAddressGroup(sld, "shore55", "goto", "goto4");
			LAi_ActorDialog(sld, pchar, "", 10.0, 0);
			LAi_SetImmortal(sld, true);
		}
		else
		{
			LAi_CharacterDisableDialog(sld);
			ChangeCharacterAddressGroup(sld, "shore55", "goto", "goto1");
		}
		LAi_group_MoveCharacter(sld, "EnemyFight");
	}
	sld = GetCharacter(NPC_GenerateMusketeer("JaMush", "mushketer_1", "man", iTemp, PIRATE, -1, true));
    FantomMakeCoolFighter(sld, iTemp, 80, 80, "", "mushket", 100);
	ChangeCharacterAddressGroup(sld, "shore55", "goto", "goto2");
	LAi_group_MoveCharacter(sld, "EnemyFight");
}

void Create_Ja(string qName)//cоздаем Фрегат "Ахерон" с Джа
{
	int hcrew;
	Group_FindOrCreateGroup("BlueBird_Group2");
	bQuestDisableMapEnter = true;
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("Jafar", "QuestCaptain", "man", "man_fast", 28, PIRATE, -1, true)); // LEO: Превозмогаторам - страдать 01.12.2021
	else sld = GetCharacter(NPC_GenerateCharacter("Jafar", "QuestCaptain", "man", "man", 28, PIRATE, -1, true));
	sld.name 	= GetConvertStr("Variable_reaction_functions_0.20", "Names.txt");
    sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_210", "Names.txt", "#space#", " ");
	sld.dialog.filename = "Quest\Other_quests_NPC.c";
	sld.dialog.currentnode = "Ja_talk";
	sld.greeting = "CapSinkShip";
	// Прописка всех моделей для кирас. -->
	sld.HeroModel = "QuestCaptain,QuestCaptain_1,QuestCaptain_2,QuestCaptain_3,QuestCaptain_4,QuestCaptain_5,QuestCaptain,QuestCaptain,QuestCaptain";
	// Прописка всех моделей для кирас. <--
	FantomMakeCoolFighter(sld, 35, 75, 75, "blade14", "pistol6", 100);
	FantomMakeCoolestSailor(sld, SHIP_CONSTITUTION, "Ахерон", CANNON_TYPE_CANNON_LBS32, 80, 80, 80); // LEO: убрал оверапы калибров
	DeleteAttribute(sld, "SinkTenPercent");
	DeleteAttribute(sld, "AboardToFinalDeck");
	sld.AlwaysSandbankManeuver = true;
	sld.SaveItemsForDead = true;
	sld.DontClearDead = true;
	sld.AnalizeShips = true;  //анализировать вражеские корабли при выборе таска
	sld.AlwaysEnemy = true;
	sld.DontRansackCaptain = true;
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	SetCharacterPerk(sld, "FastReload");
	SetCharacterPerk(sld, "HullDamageUp");
	SetCharacterPerk(sld, "SailsDamageUp");
	SetCharacterPerk(sld, "CrewDamageUp");
	SetCharacterPerk(sld, "CriticalShoot");
	SetCharacterPerk(sld, "LongRangeShoot");
	SetCharacterPerk(sld, "CannonProfessional");
	SetCharacterPerk(sld, "ShipDefenseProfessional");
	SetCharacterPerk(sld, "SwordplayProfessional");
	SetCharacterPerk(sld, "AdvancedDefense");
	SetCharacterPerk(sld, "CriticalHit");
	SetCharacterPerk(sld, "Sliding");
	SetCharacterPerk(sld, "MusketsShoot");
	SetCharacterPerk(sld, "ShipSpeedUp");
	SetCharacterPerk(sld, "ShipTurnRateUp");
	GiveItem2Character(sld, "indian5");
	GiveItem2Character(sld, "jewelry15");
	GiveItem2Character(sld, "icollection");
	GiveItem2Character(sld, "spyglass4");
	EquipCharacterbyItem(sld, "spyglass4");
	EquipCharacterByItem(sld,"talisman14");
	hcrew = GetMaxCrewQuantity(sld);
	SetCrewQuantity(sld, hcrew);

	sld.ship.Crew.Morale = 100;
	ChangeCrewExp(sld, "Sailors", 100);
	ChangeCrewExp(sld, "Cannoners", 100);
	ChangeCrewExp(sld, "Soldiers", 100);
	Group_AddCharacter("BlueBird_Group2", "Jafar");

	Group_SetGroupCommander("BlueBird_Group2", "Jafar");
	Group_SetAddress("BlueBird_Group2", "SantaCatalina", "quest_ships", "Quest_ship_9");
	Group_SetTaskAttack("BlueBird_Group2", PLAYER_GROUP);
	Group_LockTask("BlueBird_Group2");
	AddQuestRecord("Headhunt", "36");

	pchar.quest.Headhunter_AfterBattleJa.win_condition.l1 = "Group_Death";
	pchar.quest.Headhunter_AfterBattleJa.win_condition.l1.group = "BlueBird_Group2";
	pchar.quest.Headhunter_AfterBattleJa.function = "Ja_over";
}

void Ja_over(string qName)//уничтожили Джа
{
	Group_DeleteGroup("BlueBird_Group2");
	bQuestDisableMapEnter = false;
	AddQuestRecord("Headhunt", "37");
	AddQuestUserData("Headhunt", "sSex", GetSexPhrase("","а"));
	pchar.questTemp.Headhunter = "hunt_ja_yes";
}

void Pascal_Escape()
{
}

//ККС

// --> тайна Санта-Люсии
void LSC_RingOver(string qName) // чистка квеста
{
	sld = characterFromId("LSC_Rishard");
	sld.dialog.currentnode = "First time";
	ChangeCharacterAddressGroup(sld, "FleuronTavern", "sit", "sit2");
	LAi_SetSitType(sld);
	LAi_group_MoveCharacter(sld, "LSC_CITIZEN");
	if (!CheckAttribute(sld, "quest.ring_final")) AddQuestRecord("LSC_Ring", "2");

	sld = characterFromId("pet_crab");
	ChangeCharacterAddressGroup(sld, "ExternalRingDeck", "goto", "goto5");
	LAi_SetActorTypeNoGroup(sld);
	LAi_type_actor_Reset(sld);
	LAi_ActorDialog(sld, pchar, "", 20.0, 0);

	CloseQuestHeader("LSC_Ring");
	CloseQuestHeader("RingCapBook");


}

void LSC_RingStart(string qName) // готовы
{
	sld = characterFromId("LSC_Rishard");
	sld.dialog.currentnode = "ring_10";
	LAi_RemoveLoginTime(sld);
	ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "quest", "ring");
	LAi_SetStayType(sld);
	LAi_group_MoveCharacter(sld, "TMP_FRIEND");
}

void LSC_RingEnter(string qName) // входим
{
	pchar.quest.LSC_Ring_Over.over = "yes"; //снять прерывание
	SetLaunchFrameFormParam(GetConvertStr("Variable_reaction_functions_6", "Frames.txt")+ NewStr() +GetConvertStr("Variable_reaction_functions_7", "Frames.txt"), "", 0, 6);//табличка
	LaunchFrameForm();
	WaitDate("", 0, 0, 0, 1, 10); //крутим время
	RecalculateJumpTable();
	DoQuestFunctionDelay("LSC_RingReload", 5.5);
}

void LSC_RingReload(string qName) // телепорт
{
	DoQuestReloadToLocation("ExternalRingInside", "reload", "reload1", "LSC_RingEnterInside");
}

void LSC_RingRishardTalk(string node) // говорилка Ришарда
{
	sld = characterFromId("LSC_Rishard");
	sld.dialog.currentnode = node;
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}

void LSC_RingDeleteItemsBoxes(string qName)
{
	ref loc;
	aref arBox;
	string sName;
	for(int i=0; i<MAX_LOCATIONS; i++)
	{
		loc = &locations[i];
		if (loc.id == "ExternalRingDeck" || loc.id == "ExternalRingInside" || loc.id == "ExternalRingCabin1")
		{
			for(int n=1; n<=MAX_HANDLED_BOXES; n++)
			{
				sName = "private" + n;
				if (CheckAttribute(loc, sName))
				{
					makearef(arBox, loc.(sName));
					DeleteAttribute(arBox, "items");
					arBox.items = "";
				}
				else break;
			}
		}
	}
	pchar.GenQuestBox.ExternalRingInside = true;
	pchar.GenQuestBox.ExternalRingInside.box1.items.jewelry5 = 1;
}
// <-- тайна Санта-Люсии

// История старой дружбы Lipsar
string RandomCityAllien()
{
	if (CheckAttribute(&colonies[1], "nation"))
	{
		string City;
		int n, nation;
		int storeArray[27];
		int howStore = 0;
		string NATION1=pchar.BaseNation;
		switch (NATION1)
		{
			case "0":
				NATION1="ENGLAND";
			break;
			case "1":
				NATION1="FRANCE";
			break;
			case "2":
				NATION1="SPAIN";
			break;
			case "3":
				NATION1="HOLLAND";
			break;
			case "4":
				NATION1="PIRATE";
			break;
		}
		for (int i = 0; i < 27; i++)
		{
			if (colonies[i].nation != "none" && colonies[i].id != "Havana" && colonies[i].id != "Panama" &&  colonies[i].nation == NATION1 && colonies[i].id != "PortRoyal")
			{
				storeArray[howStore] = i;
				howStore++;
			}
		}
	n = rand(howStore - 1);
	nation = storeArray[n];
	City = colonies[nation].id;
	}
	Log_Info(City);
	return City;
}
string RandomCityEnemy()
{
	string City;
	int n, n1, n2, n3 = 0;
	int nation = 0;
	int storeArray[27];
	int howStore = 0;
	string Patent = pchar.PatentNation;
	switch (Patent)
	{
	case "eng":
		n = 0;
		n1 = 1;
		n2 = 0;
		n3 = 0;
	break;
	case "fra":
		n = 1;
		n1 = 0;
		n2 = 3;
		n3 = 1;
	break;
	case "spa":
		n = 2;
		n1 = 3;
		n2 = 2;
		n3 = 2;
	break;
	case "hol":
		n = 3;
		n1 = 1;
		n2 = 2;
		n3 = 3;
	break;
	}
	for (int i = 0; i < 27; i++)
	{
		if (colonies[i].nation != "none" && sti(colonies[i].nation) != PIRATE && colonies[i].id != "Havana" && colonies[i].id != "Panama" && colonies[i].nation != n && colonies[i].nation != n1 && colonies[i].nation != n2 && colonies[i].nation != n3)
		{
			storeArray[howStore] = i;
			howStore++;
		}
	}
	n = rand(howStore - 1);
	nation = storeArray[n];
	City = colonies[nation].id;
	return City;
}
void SpawnMaks(string spawn_max)
{
	sld = GetCharacter(NPC_GenerateCharacter("Maks", "pirate_15", "man", "man", 30, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_215", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_216", "Names.txt", "#space#", " ");
	sld.City = spawn_max - "_tavern";
	FantomMakeCoolFighter(sld, 30, 90, 90, "blade28", "pistol5", 300);
	FreeSitLocator(spawn_max, "sit1");
	SetSelfSkill(sld, 100, 80, 80, 100, 80);
	SetShipSkill(sld, 70, 70, 70, 70, 70, 70, 70, 70, 70);
	ChangeCharacterAddressGroup(sld, spawn_max, "sit", "sit1");
	sld.Dialog.filename = "Quest\SilencePrice\Maks.c";
	LAi_SetImmortal(sld, true);
	LAi_SetSitType(sld);
	LAi_SetLoginTime(sld, 0.0, 24.0);
	sld.NextDiag.TempNode = "First_Time";
}
string RandomHouse(ref npchar)
{
	aref arCommon, arRld, arRld2, arDis;
	int i, n, Qty, Qty2;
	string LocId;
	string storeArray[50];
	int howStore = 0;
	makearef(arRld, Locations[FindLocation(npchar.city + "_town")].reload);
	Qty = GetAttributesNum(arRld);
	for (i = 0; i < Qty; i++)
	{
		arCommon = GetAttributeN(arRld, i);
		LocId = arCommon.go;
		if (findsubstr(LocId, "Common", 0) != -1)
		{
			storeArray[howStore] = LocId;
			howStore++;
		}

		if (arCommon.label != "Sea")
		{
			makearef(arRld2, Locations[FindLocation(LocId)].reload);
			Qty2 = GetAttributesNum(arRld2);
			for (n = 0; n < Qty2; n++)
			{
				arCommon = GetAttributeN(arRld2, n);
				LocId = arCommon.go;
				if (findsubstr(LocId, "Common", 0) != -1)
				{
					storeArray[howStore] = LocId;
					howStore++;
				}
			}
		}
	}
	LocId = storeArray[rand(howStore - 1)];
	Log_TestInfo(LocId);
	Pchar.Luke.City.Loc = LocID;
	SetOpenDoorCommonLoc(Pchar.Luke.City, LocID);
	return LocId;
}
void GetSpawnPirate(string qName)
{
	chrDisableReloadToLocation = true;
	sld = GetCharacter(NPC_GenerateCharacter("Anri", "officer_12", "man", "man", 30, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_217", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_218", "Names.txt", "#space#", " ");
	SelAllPerksToNotPchar(sld);
	FantomMakeCoolFighter(sld, 30, 90, 90, "blade32", "pistol5", 300);
	sld.Dialog.filename = "Quest\SilencePrice\Anri.c";
	sld.SaveItemsForDead = true;
	LAi_SetImmortal(sld,true);
	SetSelfSkill(sld, 100, 80, 80, 100, 80);
	SetShipSkill(sld, 70, 70, 70, 70, 70, 70, 70, 70, 70);
	SelAllPerksToNotPchar(sld);
	sld.NextDiag.TempNode = "First_Time";
	ChangeCharacterAddressGroup(sld, "LaVega_town", "officers", LAi_FindNearestFreeLocator2Pchar("officers"));
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, Pchar, "", 20.0, 5.0);
	bDisableFastReload = false;
}
void PirateComeToIsabella(string qName)
{
	sld = CharacterFromID("Anri");
	sld.NextDiag.CurrentNode = "Isabella_1";
	LAi_SetStayType(sld);
	ChangeCharacterAddressGroup(sld, "Shore32", "smugglers", "smuggler01");
	Locations[FindLocation("Shore32")].DisableEncounters = true;
	LAi_SetActorType(sld);
	LAi_LocationDisableOfficersGen("Shore32", true);
	LAi_ActorDialog(sld, Pchar, "", 20.0, 20.0);
}
void PirateComeToIsabellaLoose(string qName)
{
	sld = CharacterFromID("Anri");
	ChangeCharacterAddressGroup(sld, "none", "none", "none");
	AddQuestRecord("Silence_Price", "5_1");
	CloseQuestHeader("Silence_Price");
	// Gregg: questdata здесь НЕ ставим. Единица рисует в списке квестов иконку complete
	// (QuestBook.c:1290), а это провал: игрок не пришёл на Исабелу до трёх ночи и потерял
	// след Люка. Образец - MoonStoneStartFailed, там при провале флаг тоже не трогают.
}
void MaksCome(string qName)
{
	sld = CharacterFromID("Maks");
	LAi_SetImmortal(sld, false);
	ChangeCharacterAddressGroup(sld, Pchar.Luke.City.Loc, "reload", "reload1");
	SelAllPerksToNotPchar(sld);
	LAi_group_MoveCharacter(sld, "Maks");
	LAi_SetPlayerType(pchar);
	LAi_group_FightGroups("Maks", LAI_GROUP_PLAYER, true);
	LAi_group_SetCheck("Maks","AddGuardians");
	EndLookAfterCharacter();
	LAi_SetFightMode(pchar, true);
	sld.SaveItemsForDead = true;
	GiveItem2Character(sld,"book3_5");
	GiveItem2Character(sld,"book3_12");
	GiveItem2Character(sld,"cirass5");
	EquipCharacterByItem(sld, "cirass5");
	RemoveItems(sld, "cirass5", 1);
	sld.DontClearDead = true;
}
void LukesEscape(string qName)
{
	AddQuestRecord("Silence_Price", "6");
	Flag_Change(PIRATE);
	AddCharacterHealth(pchar, -30);
	Pchar.Luke.BadPoison = "1";
}
void IDD_Pochta_1(string qName)
{
	chrDisableReloadToLocation = true;
	sld = GetCharacter(NPC_GenerateCharacter("IDD_Pochta_1", "officer_4", "man", "man", 30, PIRATE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_219", "Names.txt", "#space#", " ");
	sld.lastname = "";
	FantomMakeCoolFighter(sld, 10, 40, 40, "blade6", "pistol1", 50);
	sld.Dialog.filename = "Quest\SilencePrice\Shpionskie_Igry.c";
	LAi_SetImmortal(sld,true);
	ChangeCharacterAddressGroup(sld, PChar.location, "goto", "goto25");
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, Pchar, "", 20.0, 0.0);
}
void IDD_SI_Zovet_Boss(string qName)
{
	sld = CharacterFromID("IDD_Lamb_Ohr2")
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_220", "Names.txt", "#space#", " ");
	sld.lastname = "";
	sld.Dialog.filename = "Quest\SilencePrice\Shpionskie_Igry.c";
	sld.dialog.currentnode   = "Dokladivaet_Ohr2_1";
	LAi_CharacterEnableDialog(sld);
	ChangeCharacterAddressGroup(sld, "FortFrance_town", "reload", "houseH2");
	LAi_ActorDialog(sld, pchar, "", 0.3, 0);
}
//Korsar Maxim --> мой код
void SetPortAlarm(string qName) //Тревога в городе, если причалил под вражеским флагом
{
	Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_32", "Logs.txt", "#space#", " "));

	string sCity = FindStringBeforeChar(PChar.location, "_town");

	int iNation = GetCityNation(sCity);

	string sNation = GetNationNameByType(iNation);

	LAi_group_FightGroups(GetNationNameByType(iNation)+"_citizens", LAI_GROUP_PLAYER, false);
	if (rand(3) != 1) SetNationRelation2MainCharacter(iNation, RELATION_ENEMY);
}
//Korsar Maxim <-- мой код

//Sinistra Проклятый идол -->
void PDM_Callow_RodjerProdolg(string qName)
{
	chrDisableReloadToLocation = true;
	ClearAllLogStrings();
	Log_info(GetConvertStrWithReplace("Variable_reaction_functions_33", "Logs.txt", "#space#", " "));
	
	sld = CharacterFromID("PDM_LeFransua_Mayor_Klon")
	ChangeCharacterAddressGroup(sld, "LeFransua_town", "none", "");

	sld = CharacterFromID("LeFransua_Mayor")
	ChangeCharacterAddressGroup(sld, "LeFransua_townhall", "sit", "sit1");
}
void PDM_PI_Skelety_v_more(string qName)
{
	if (CheckAttribute(pchar, "questTemp.PDM_PI_Skelety_v_more"))
	{
		if (IsEntity(worldMap))
		{
			DoQuestFunctionDelay("PDM_PI_Skelety_v_more_paluba", 0.0);
		}
		else
		{
			PChar.quest.PDM_Ne_Spryacheshsy.win_condition.l1 = "MapEnter";
			PChar.quest.PDM_Ne_Spryacheshsy.function = "PDM_PI_Skelety_v_more";
		}
	}
}
void PDM_PI_Skelety_v_more_paluba(string qName)
{
	MakeCloneShipDeck(pchar, true); // подмена палубы
	DoQuestReloadToLocation("Ship_deck", "goto", "goto10", "PDM_PI_Skelety_on_Ship");
}
void PDM_PI_Vykl_Music(string qName)
{
	SetMusic("none");
	PlayVoice("Kopcapkz\Voices\Skeletons\Skeleton_hit_14.ogg");
	PlayVoice("Kopcapkz\Voices\Skeletons\Skeleton_hit_16.ogg");
	PlayVoice("Kopcapkz\Voices\Skeletons\Skeleton_hit_23.ogg");
	SetMusic("Deck_Nekro");
}
void PDM_PI_Vernut_Gorod(string qName)
{
	SetLocationCapturedState("LeFransua_town", false);
	AddSimpleRumourToAllNations(GetConvertStrWithReplace("Variable_reaction_functions_75", "AddRumours.txt", "#space#", " "), 7, 1);
}
//Sinistra Проклятый идол <--

//Sinistra Золото не тонет -->
void PDM_Zoloto_ne_tonet_BITVA_na_sushe(string qName)
{
	chrDisableReloadToLocation = true;
	Pchar.GenQuest.Hunter2Pause = true;
	LAi_SetPlayerType(pchar);
	LAi_SetFightMode(pchar, true);
	PlaySound("Kopcapkz\Voices\PDM\Witch\piple.wav");
	PlaySound("Kopcapkz\Voices\PDM\Ispantsi.wav");

	int Rank = sti(pchar.rank) - 3 + MOD_SKILL_ENEMY_RATE;
	if (Rank < 1) Rank = 1;
//------------------------испанцы-------------------
	for (i=1; i<=9; i++)
    {
        sTemp = "sold_spa_"+(rand(7)+1);  //количество
		if (i==6) sTemp = "off_spa_2";
 		sld = GetCharacter(NPC_GenerateCharacter("SraFriend_"+i, sTemp, "man", "man", Rank, SPAIN, -1, true));
        FantomMakeCoolFighter(sld, sti(pchar.rank), 40, 40, BLADE_NORMAL, "", 10);
		sld.greeting = "GR_Spainguard";
		LAi_SetWarriorType(sld);
		sld.lifeday = 0;
		sld.dialog.filename   = "Quest\PDM\Novaya_Rodina.c";
		sld.dialog.currentnode   = "Novoe_Zadanie_ZaIspaniu";
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
        ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto13");
	}
	for (i=10; i<=18; i++)
    {
        sTemp = "sold_spa_"+(rand(7)+1);  //количество
		if (i==15) sTemp = "off_spa_2";
 		sld = GetCharacter(NPC_GenerateCharacter("SraFriend_"+i, sTemp, "man", "man", Rank, SPAIN, -1, true));
        FantomMakeCoolFighter(sld, sti(pchar.rank), 40, 40, BLADE_NORMAL, "", 10);
		sld.greeting = "GR_Spainguard";
		LAi_SetWarriorType(sld);
		sld.lifeday = 0;
		sld.dialog.filename   = "Quest\PDM\Novaya_Rodina.c";
		sld.dialog.currentnode   = "Novoe_Zadanie_ZaIspaniu";
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
        ChangeCharacterAddressGroup(sld, pchar.location, "item",  "item10");
	}
	for (i=19; i<=22; i++)
    {
        sld = GetCharacter(NPC_GenerateMusketeer("SpaMush_"+i, "spa_rifle_"+(rand(2)+1), "man", Rank, SPAIN, -1, false));
		sld.MusketerDistance = 0;
		sld.greeting = "GR_Spainguard";
		LAi_SetWarriorType(sld);
		sld.lifeday = 0;
		sld.dialog.filename   = "Quest\PDM\Novaya_Rodina.c";
		sld.dialog.currentnode   = "Novoe_Zadanie_ZaIspaniu";
        LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
        ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto10");
	}
//----------------------англичане-------------------------
	for (i=23; i<=43; i++)
    {
        sTemp = "sold_eng_"+(rand(7)+1);  //количество
		if (i==40) sTemp = "off_eng_1";
 		sld = GetCharacter(NPC_GenerateCharacter("EngEnemy_"+i, sTemp, "man", "man", Rank, ENGLAND, -1, true));
        FantomMakeCoolFighter(sld, sti(pchar.rank), 50, 50, BLADE_NORMAL, "", 20);
		LAi_SetWarriorType(sld);
		LAi_group_MoveCharacter(sld, "PDM_ENGenemy");
        ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto16");
	}
	for (i=44; i<=47; i++)
    {
        sld = GetCharacter(NPC_GenerateMusketeer("EngMush_"+i, "eng_rifle_"+(rand(2)+1), "man", Rank, ENGLAND, -1, false));
		sld.MusketerDistance = 0;
		LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "PDM_ENGenemy");
        ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto26");
	}
	LAi_group_SetRelation("PDM_ENGenemy", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);		//стравливаем
	LAi_group_FightGroups("PDM_ENGenemy", LAI_GROUP_PLAYER, false);
	LAi_group_SetCheck("PDM_ENGenemy", "PDM_ZolNeTon_PobNaSush");
}
//Sinistra Золото не тонет <--

//Sinistra Охота на ведьму -->
void PDM_ONV_NaRabotu(string qName)
{
	LAi_SetPlayerType(pchar);
	StartInstantDialogNoType("Beliz_Mayor", "RazgovorSGuberom_7", "Quest\PDM\Ohota_na_vedmu.c");
}
//Sinistra Охота на ведьму <--

//Sinistra Клан Ламбрини -->
void PDM_CL_Ubrat_Lodku(string qName)
{
	int Rank = sti(pchar.rank) - 4 + MOD_SKILL_ENEMY_RATE;
	if (Rank < 1) Rank = 1;
	int Sila = 70 + MOD_SKILL_ENEMY_RATE*3;
	int DopHP = 60 + MOD_SKILL_ENEMY_RATE * 12;

	sld = CharacterFromID("PDM_CL_Anto2")
	ChangeCharacterAddressGroup(sld, "none", "", "");
	Group_SetAddress("PDM_el_tib", "none", "", "");

	sld = GetCharacter(NPC_GenerateCharacter("PDM_CL_Antonio3", "SpaOfficer2", "man", "man", Rank, SPAIN, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_221", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_222", "Names.txt", "#space#", " ");
	sld.greeting = "GR_Spainguard";
	FantomMakeCoolFighter(sld, Rank, Sila, Sila, "blade34", "pistol2", DopHP);
	sld.nonTable = true;
	LAi_SetSitType(sld);
	sld.Dialog.Filename = "Quest\PDM\Clan_Lambrini.c";
	sld.dialog.currentnode   = "Antonio_2_1";
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	sld.city = "Maracaibo";
	ChangeCharacterAddressGroup(sld, "Maracaibo_church", "sit", "sit16");
	LAi_SetLoginTime(sld, 9.0, 14.00);
}
void PDM_CL_Sadis_Pokupatel(string qName)
{
	sld = CharacterFromID(pchar.LambriniPGG)
	ChangeCharacterAddressGroup(sld, "Maracaibo_tavern", "sit", "sit_base2");
	LAi_SetSitType(sld);
}
void PDM_CL_Sadis_Octavio(string qName)
{
	sld = CharacterFromID("PDM_Octavio_Lambrini")
	ChangeCharacterAddressGroup(sld, "Maracaibo_tavern", "sit", "sit_front2");
	LAi_SetSitType(sld);
}
void PDM_CL_PlayerType(string qName)
{
	LAi_SetPlayerType(pchar);
}
void PDM_CL_Octavio_Speech(string qName)
{
	sld = CharacterFromID("PDM_Octavio_Lambrini");
	LAi_tmpl_SetDialog(sld, pchar, -1.0);
}
void PDM_CL_Pokupatel_Speech(string qName)
{
	sld = CharacterFromID(pchar.LambriniPGG);
	LAi_tmpl_SetDialog(sld, pchar, -1.0);
}
void PDM_CL_Pokupatel_Uhodit(string qName)
{
	sld = characterFromID(pchar.LambriniPGG);
	LAi_SetActorType(sld);
	ChangeCharacterAddressGroup(sld, "Maracaibo_tavern", "tables", "stay3");
	LAi_ActorGoToLocation(sld, "reload", "reload1_back", "none", "", "", "", -1);
}

void LambriniPGG_RemoveFromShore(string qName)
{
	sld = characterFromID(pchar.LambriniPGG);
	ChangeCharacterAddressGroup(sld, "none", "none", "none");
}

void LambriniPGG_Tavern(string qName)
{
	sld = CharacterFromID(pchar.LambriniPGG);
	sld.dialog.currentnode   = "LambriniPGG_2_3";
	pchar.InstantDialog = sld.id;
	DoQuestFunctionDelay("InstantDialog", 0);
}
//Sinistra Клан Ламбрини <--

//Sinistra "Странные вещи творятся на архипелаге" -->

void PKM_SvtvA_Malta_na_pomosh(string qName)
{	
	sld = GetCharacter(NPC_GenerateCharacter("Maltese", "Maltese", "man", "man", sti(pchar.rank), SPAIN, -1, true));
	FantomMakeCoolFighter(sld, sti(pchar.rank), 30, 30, "blade16", "pistol1", 400);
	SetSPECIAL(sld, 10, 6, 8, 10, 8, 7, 6);
	SetSelfSkill(sld, 18, 21, 70, 3, 40);
	SetShipSkill(sld, 60, 12, 18, 17, 32, 41, 60, 31, 26);
	SetCharacterPerk(sld, "Energaiser");
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_223", "Names.txt", "#space#", " ");
	sld.lastname = GetConvertStrWithReplace("Variable_reaction_functions_224", "Names.txt", "#space#", " ");
	sld.loyality = 20;
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
	ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload1_back");
	
	Log_info(GetConvertStrWithReplace("Variable_reaction_functions_34", "Logs.txt", "#space#", " "));
	
	PChar.quest.PKM_SvtvA_MalteseMertv.win_condition.l1 = "NPC_Death";
	PChar.quest.PKM_SvtvA_MalteseMertv.win_condition.l1.character = "Maltese";
	PChar.quest.PKM_SvtvA_MalteseMertv.win_condition = "PKM_SvtvA_MalteseMertv";
	sld = CharacterFromID("Satanist_Krisa");
	LAi_SetImmortal(sld, false);
}
void PKM_SvtvA_Malta_na_pomosh_2(string qName)
{	
	LAi_SetPlayerType(pchar);
	sld = CharacterFromID("FortFrance_Priest");
	LAi_SetActorType(sld);
	sld.dialog.filename = "Quest\PKM\Strannie_veshi_tvorytsya_v_arhipelage.c";
	sld.dialog.currentnode = "Победа_в Церкви";
	LAi_ActorDialogNow(sld, Pchar, "", -1);
}
void PKM_SvtvA_Gospod_nakazal(string qName)
{	
	LAi_SetActorType(pchar);
}
void PKM_SvtvA_Uchitel_smert(string qName)
{	
	sld = CharacterFromID("Satanist_Uchitel");
	LAi_ActorAnimation(sld, "lay_2", "", 2.0);
}
void PKM_SvtvA_SatanistyAnimation(string qName)
{	
	sld = CharacterFromID("Satanist_Kuhnya_5");
	LAi_SetActorType(sld);
	LAi_ActorTurnToLocator(sld, "sit", "sit35");
	LAi_ActorAnimation(sld, "dialog_stay14", "", 2.0);
	
	sld = CharacterFromID("Satanist_Oderjim_1");
	LAi_SetActorType(sld);
	LAi_ActorAnimation(sld, "worship_2", "", 2.4);
	
	sld = CharacterFromID("Satanist_Oderjim_2");
	LAi_SetActorType(sld);
	LAi_ActorAnimation(sld, "worship_2", "", 2.4);
	
	sld = CharacterFromID("Satanist_Oderjim_3");
	LAi_SetActorType(sld);
	LAi_ActorAnimation(sld, "worship_2", "", 2.4);
	
	sld = CharacterFromID("Satanist_Oderjim_4");
	LAi_SetActorType(sld);
	LAi_ActorAnimation(sld, "worship_2", "", 2.4);
	
	sld = CharacterFromID("Satanist_Oderjim_5");
	LAi_SetActorType(sld);
	LAi_ActorAnimation(sld, "worship_2", "", 2.4);
	
	sld = CharacterFromID("Chernoe_Solntse");
	LAi_SetActorType(sld);
	LAi_ActorSetHuberMode(sld);
}
void PKM_SvtvA_Temnota(string qName)
{	
	LAi_Fade("", "");
}
void PKM_SvtvA_RazgovorZKnyazem(string qName)
{	
	StartQuestMovie(true, false, true);
	LAi_SetActorType(pchar);
	DoQuestFunctionDelay("PKM_SvtvA_Temnota", 1.5);
	DoQuestFunctionDelay("PKM_SvtvA_RazgovorZKnyazem2", 2.0);
	SetMusic("music_DeckWithReefs");
}
void PKM_SvtvA_RazgovorZKnyazem2(string qName)
{	
	locCameraFromToPos(-2.00, 4.40, 8.00, true, -4.00, 1.74, 3.70);
	DoQuestFunctionDelay("PKM_SvtvA_Temnota", 3.0);
	DoQuestFunctionDelay("PKM_SvtvA_RazgovorZKnyazem3", 3.5);
}
void PKM_SvtvA_RazgovorZKnyazem3(string qName)
{	
	locCameraFromToPos(-9.30, 8.00, -0.70, true, 13.00, -20.0, 0.00);
	DoQuestFunctionDelay("PKM_SvtvA_Temnota", 3.7);
	DoQuestFunctionDelay("PKM_SvtvA_RazgovorZKnyazem4", 4.2);
}
void PKM_SvtvA_RazgovorZKnyazem4(string qName)
{	
	locCameraFromToPos(-8.00, 2.90, -1.00, true, -30.00, 3.50, 3.70);
	DoQuestFunctionDelay("PKM_SvtvA_RazgovorZKnyazem5", 1.5);
}
void PKM_SvtvA_RazgovorZKnyazem5(string qName)
{	
	LAi_SetPlayerType(pchar);
	sld = CharacterFromID("Chernoe_Solntse");
	sld.dialog.filename = "Quest\PKM\Strannie_veshi_tvorytsya_v_arhipelage.c";
	sld.dialog.currentnode = "Лорд_Хаоса_1";
	LAi_ActorDialogNow(sld, Pchar, "", -1);
}

//Sinistra "Странные вещи творятся на архипелаге" <--

//Sinistra Мэри и Шарль -->
bool SharleMaryIsHere()	//Проверка на Мэри абордажника
{
	if (CheckAttribute(pchar, "SharleMaryId"))
	{
		sld = CharacterFromID(pchar.SharleMaryId);
		if (sld.location == pchar.location && !LAi_IsDead(sld))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void Mary_SexReady(string qName) // Мэри снова готова к сексу
{
	sld = characterFromId("SharleMary");	
	DeleteAttribute(sld, "quest.daily_sex");
	pchar.quest.Mary_giveme_sex.win_condition.l1 = "Timer";
	pchar.quest.Mary_giveme_sex.win_condition.l1.date.hour  = sti(GetTime());
	pchar.quest.Mary_giveme_sex.win_condition.l1.date.day   = GetAddingDataDay(0, 0, 14);
	pchar.quest.Mary_giveme_sex.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 14);
	pchar.quest.Mary_giveme_sex.win_condition.l1.date.year  = GetAddingDataYear(0, 0, 14);
	pchar.quest.Mary_giveme_sex.function = "Mary_GiveMeSex";
}

void Mary_GiveMeSex(string qName)	// требует секса, если давно не давал. Пока только в таверне
{
	pchar.quest.Mary_giveme_sex1.win_condition.l1 = "Location_Type";
	pchar.quest.Mary_giveme_sex1.win_condition.l1.location_type = "tavern";
	pchar.quest.Mary_giveme_sex1.function = "Mary_GetTalk";
	sld = characterFromId("SharleMary");
	sld.quest.iwantsex = true;
}

void Mary_GetTalk(string qName) // говорилка Мэри
{
	// Фикс софтлока Мэри когда она хочет интима с Шариком -->
	if (!SharleMaryIsHere())
    {
        SetFunctionTimerCondition("Mary_SexReady", 0, 0, 5, false);
        return;
    }
	// Фикс софтлока Мэри когда она хочет интима с Шариком <--
	chrDisableReloadToLocation = true;//закрыть локацию
	sld = characterFromId("SharleMary");
	if (CheckAttribute(sld, "quest.iwantsex")) sld.dialog.currentnode = "Mary_givemesex";
	GetCharacterPos(pchar, &locx, &locy, &locz);
	ChangeCharacterAddressGroup(sld, pchar.location, "reload", LAi_FindNearestLocator("reload", locx, locy, locz));
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0);
}
void Mary_Pomogaet_v_Kautah(string q)
{
	sld = CharacterFromID(pchar.SharleMaryId);
	ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
}
//Sinistra Мери и Шарль <--

void StartInstantDialogNoType(string id, string node, string fileName)
{
	DialogExit();
	sld = CharacterFromId(id);
	sld.Dialog.Filename = fileName;
	sld.dialog.currentnode = node;
	pchar.InstantDialog = sld.id;
	DoQuestFunctionDelay("InstantDialogNoType", 0);
}

void InstantDialogNoType(string qName)
{
	sld = CharacterFromID(pchar.InstantDialog); // LEO: восстановить sld из pchar.InstantDialog — глобал sld мог перезаписаться за кадр отложенного вызова (DoQuestFunctionDelay=PostEvent), тогда NoType-диалог не запускался (напр. Вейн в "Неуловимой Урке" молчал). Как в InstantDialog.
	sld.chr_ai.tmpl.dialog = "";
	sld.chr_ai.tmpl.time = "0";
	sld.chr_ai.tmpl.dlgtime = "-1";
	sld.chr_ai.tmpl.state = "wait";
	DeleteAttribute(sld, "chr_ai.tmpl.poklon");
	LAi_tmpl_dialog_StartDialog(sld, pchar, -1.0);
}

void StartInstantDialog(string id, string node, string fileName)
{
	DialogExit();
	sld = CharacterFromId(id);
	sld.Dialog.Filename = fileName;
	sld.dialog.currentnode = node;
	pchar.InstantDialog = sld.id;
	DoQuestFunctionDelay("InstantDialog", 0);
}

void InstantDialog(string qName)
{
	sld = CharacterFromID(pchar.InstantDialog);
	LAi_SetStayTypeNoGroup(sld);
	LAi_tmpl_SetDialog(sld, pchar, -1.0);
}

void SaveOldDialog(ref chr)
{
	chr.Dialog.FilenameBackup = chr.Dialog.Filename;
	chr.dialog.currentnodeBackup   = chr.dialog.currentnode;
}

void RestoreOldDialog(ref chr)
{
	chr.Dialog.Filename = chr.Dialog.FilenameBackup;
	chr.dialog.currentnode   = chr.dialog.currentnodeBackup;
}

//Blackthorn Викинг
void PirateVikingQuest_Captain_Is_Dead(string qName)
{
	AddQuestRecord("pirateVikingQuest", "7");
	pchar.questTemp.pirateVikingQuest = "7";
}
//Blackthorn Викинг

//Тичингиту
void FreeTichingituOver(string qName)//удаляем Тичингиту
{
	DeleteAttribute(pchar, "questTemp.Tichingitu");
	if (CheckCharacterItem(pchar, "letter_1")) RemoveItems(pchar, "letter_A", 1); // 170712
	sld = CharacterFromID("Tichingitu");
	sld.lifeday = 0;
	AddQuestRecord("Ascold", "3_6");
}

void SetTichingituJail()//ставим Тичингиту
{
	sld = GetCharacter(NPC_GenerateCharacter("Tichingitu", "Tichingitu_0", "man", "man", 10, FRANCE, -1, false));
	sld.name = GetConvertStrWithReplace("Variable_reaction_functions_225", "Names.txt", "#space#", " "); // 270912
	sld.lastname = "";
	sld.greeting = "GR_Tynichgitu";
    sld.Dialog.Filename = "Quest\Tichingitu.c";
	sld.dialog.currentnode = "Tichingitu";
	sld.rank = 12;
	sld.HeroModel = "Tichingitu_0,Tichingitu_1,Tichingitu_2,Tichingitu_3,Tichingitu_4,Tichingitu_5";
	SetFantomHP(sld); // LEO: ХП по силе, выносливости и рангу; растущую жизнь докладывает Sync внутри
	SetSPECIAL(sld, 4, 9, 5, 5, 10, 8, 8);
	SetSelfSkill(sld, 30, 30, 30, 50, 20);
    SetShipSkill(sld, 5, 5, 2, 5, 1, 2, 1, 1, 10);
	SetCharacterPerk(sld, "Energaiser");
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "BasicDefense");
	SetCharacterPerk(sld, "CriticalHit");
	SetCharacterPerk(sld, "Gunman");
	GiveItem2Character(sld, "unarmed");
	sld.equip.blade = "unarmed";
	sld.equip.gun = "";
	ChangeCharacterAddressGroup(sld, "BasTer_prison", "goto", "goto9");
	LAi_SetStayType(sld);
	LAi_SetImmortal(sld, true); // 170712
}
//
// Нежданное наследство
void UnexpectedInheritance(string qName)
{
	if (GetCharacterIndex("UI_girl") != -1) return;
	
	sld = GetCharacter(NPC_GenerateCharacter("UI_girl", "girl_1", "woman", "woman", 1, FRANCE, -1, false));
	// LEO: девушка ждала ГГ на улице круглосуточно - ночью горожанке там делать нечего. Дневное окно,
	//   как у прочих городских: придёт игрок ночью - её на месте не будет, днём встретит как прежде.
	LAi_SetLoginTime(sld, 8.0, 21.0);
	LAI_SetStayType(sld);
	sld.talker = 10;
	LAi_group_MoveCharacter(sld, "France_citizens");
	sld.Dialog.Filename = "Quest\UnexpectedInheritance.c";
	sld.dialog.currentnode = "Girl";
	ChangeCharacterAddressGroup(sld, "BasTer_town", "patrol", "patrol12");
}

void UnexpectedInheritanceGetPartTwo(string qName)
{
	Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_35", "Logs.txt", "#space#", " "));
	PlaySound("interface\important_item.wav");
	AddQuestRecord("UnexpectedInheritance", "3");
	AddQuestUserData("UnexpectedInheritance", "sSex", GetSexPhrase("ёл","ла"));
	ref locLoad = &locations[reload_location_index];
	locLoad.box1.items.indian22 = 1;

	pchar.UnexpectedInheritance = "part2";
}
void UnexpectedInheritanceGetPartThree(string qName)
{
	Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_36", "Logs.txt", "#space#", " "));
	PlaySound("interface\important_item.wav");
	AddQuestRecord("UnexpectedInheritance", "5");
	ref locLoad = &locations[reload_location_index];
	locLoad.private1.items.indian20 = 1;

	pchar.UnexpectedInheritance = "part3";
}
void UnexpectedInheritanceGetPartFour(string qName)
{
	chrDisableReloadToLocation = false;
	Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_37", "Logs.txt", "#space#", " "));
	PlaySound("interface\important_item.wav");
	AddQuestRecord("UnexpectedInheritance", "7");

	pchar.UnexpectedInheritance = "part4";
}
void UnexpectedInheritanceGetPartFive(string qName)
{
	Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_38", "Logs.txt", "#space#", " "));
	AddQuestRecord("UnexpectedInheritance", "9");

	pchar.UnexpectedInheritanceFort = true;
}
void UnexpectedInheritanceEnd(string qName)
{
	InterfaceStates.Buttons.Save.enable = true;
	chrDisableReloadToLocation = false;
	AddQuestRecord("UnexpectedInheritance", "11");
	CloseQuestHeader("UnexpectedInheritance");
	pchar.questdata.Nasledstvo = 1;
	UnlockAchievement("AchNasledstvo", 3);
}
void UnexpectedInheritanceSeaBattle()
{
	sld = GetCharacter(NPC_GenerateCharacter("UICap", "officer_17", "man", "man", 25, PIRATE, -1, true));
	sld.Ship.Type = GenerateShipExt(SHIP_POSTILLIONEN, true, sld);
	SetRandomNameToShip(sld);
	sld.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS16;

	SetCrewQuantityFull(sld);
	int hcrew = GetMaxCrewQuantity(sld);
	SetCrewQuantity(sld, hcrew);//Full

	sld.ship.Crew.Morale = 100;
	ChangeCrewExp(sld, "Sailors", 5 + 5 * MOD_SKILL_ENEMY_RATE);
	ChangeCrewExp(sld, "Cannoners", 5 + 5 * MOD_SKILL_ENEMY_RATE);
	ChangeCrewExp(sld, "Soldiers", 5 + 5 * MOD_SKILL_ENEMY_RATE);

	sld.money = 50000;
	sld.SaveItemsForDead = true;
	SetSelfSkill(sld, 75, 75, 75, 75, 75);
	SetShipSkill(sld, 75, 75, 75, 75, 75, 75, 75, 75, 75);
	sld.rank = 15;
	if (MOD_SKILL_ENEMY_RATE > 2)
	{
		SetHalfPerksToChar(sld, true);
	}
	SetCharacterPerk(sld, "MusketsShoot");
	SetCharacterPerk(sld, "CannonProfessional");
	if (MOD_SKILL_ENEMY_RATE == 3)
	{
		sld.rank = 25;
		SetSelfSkill(sld, 90, 90, 90, 90, 90);
		SetShipSkill(sld, 100, 100, 100, 100, 100, 100, 100, 100, 100);
	}
	SetSPECIAL(sld, 10, 10, 10, 10, 10, 10, 10);

    SetCharacterGoods(sld,GOOD_FOOD,500);
    SetCharacterGoods(sld,GOOD_BALLS,1000);
    SetCharacterGoods(sld,GOOD_GRAPES,1000);
    SetCharacterGoods(sld,GOOD_KNIPPELS,1000);
    SetCharacterGoods(sld,GOOD_BOMBS,1000);
    SetCharacterGoods(sld,GOOD_POWDER,3000);
    SetCharacterGoods(sld,GOOD_PLANKS,150);
    SetCharacterGoods(sld,GOOD_SAILCLOTH,150);
    SetCharacterGoods(sld,GOOD_RUM,200);
    SetCharacterGoods(sld,GOOD_WEAPON,600);
	SetCharacterGoods(sld,GOOD_MEDICAMENT,300);

	sld.AlwaysEnemy = true;
	sld.DontRansackCaptain = true;
	sld.AlwaysSandbankManeuver = true;
	Group_FindOrCreateGroup("UI_seabattle");
	Group_SetType("UI_seabattle", "pirate");
	Group_AddCharacter("UI_seabattle", sld.id);
	Group_SetGroupCommander("UI_seabattle", sld.id);
	SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);

	LAi_SetImmortal(sld, false);
	Group_SetPursuitGroup("UI_seabattle", PLAYER_GROUP);
	Group_SetTaskAttack("UI_seabattle", PLAYER_GROUP);
	Group_LockTask("UI_seabattle");
	Group_SetAddress("UI_seabattle", "Dominica", "Quest_ships", "quest_ship_7");
}
void UnexpectedInheritanceGrottoPirates(string qName)
{
	chrDisableReloadToLocation = true;
	Group_FindOrCreateGroup("UIPirates");
	for (int i = 1; i <= MOD_SKILL_ENEMY_RATE; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("UI_pirate"+i, "pirate_"+(rand(20)+1), "man", "man", 10, PIRATE, -1, true));
		if (i == 1)
		{
			sld.model = "officer_25";
			LAI_SetStayType(sld);
			sld.talker = 10;
			sld.Dialog.Filename = "Quest\UnexpectedInheritance.c";
			sld.dialog.currentnode = "GrottoPirate";
			ChangeCharacterAddressGroup(sld, "Dominica_Grot", "goto", "goto1");
			DeleteAttribute(sld, "items");
			TakeNItems(sld, "chest", 2);
			sld.SaveItemsForDead = true;
			pchar.quest.UnexpectedInheritanceGetPartFour.win_condition.l1 = "NPC_Death";
			pchar.quest.UnexpectedInheritanceGetPartFour.win_condition.l1.character ="UI_pirate1";
			PChar.quest.UnexpectedInheritanceGetPartFour.function = "UnexpectedInheritanceGetPartFour";
		}
		else
		{
			ChangeCharacterAddressGroup(sld, "Dominica_Grot", "monsters", "monster1");
			LAI_SetWarriorType(sld);
		}
		LAi_group_MoveCharacter(sld, "UIPirates");
	}
}
void UnexpectedInheritanceTerks(string part)
{
	chrDisableReloadToLocation = true;
	Group_FindOrCreateGroup("UISkeletons");
	for (int i = 1; i <= MOD_SKILL_ENEMY_RATE; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("UI_skel"+i, "Skel"+(rand(3)+1), "skeleton", "skeleton", 12, PIRATE, -1, true));
		if (i == 1)
		{
			LAi_SetImmortal(sld, true);
			sld.model = "BSUnd5";
			FantomMakeCoolFighter(sld, 15, 60, 60, LinkRandPhrase(RandPhraseSimple("blade23","blade25"), RandPhraseSimple("blade30","blade26"), RandPhraseSimple("blade24","blade13")), "pistol8", MOD_SKILL_ENEMY_RATE*4);
			LAi_SetHP(sld, 800, 800);
			sld.SaveItemsForDead = true;
			sld.cirassId = Items_FindItemIdx("cirass1");  // предмета нет, но влияение есть
			LAI_SetStayType(sld);
			sld.talker = 10;
			sld.Dialog.Filename = "Quest\UnexpectedInheritance.c";
			sld.dialog.currentnode = "GrottoSkeleton";
			sld.HeroModel = "BSUnd5,BSUnd5_1,BSUnd5_2,BSUnd5_3,BSUnd5_4,BSUnd5_5";
			ChangeCharacterAddressGroup(sld, "Terks_Grot", "monsters", "monster3");
			sld.SaveItemsForDead = true;
			pchar.quest.UnexpectedInheritanceEnd.win_condition.l1 = "NPC_Death";
			pchar.quest.UnexpectedInheritanceEnd.win_condition.l1.character ="UI_skel1";
			PChar.quest.UnexpectedInheritanceEnd.function = "UnexpectedInheritanceEnd";
		}
		else
		{
			ChangeCharacterAddressGroup(sld, "Terks_Grot", "monsters", "monster1");
			LAi_KillCharacter(sld);
		}
		LAi_group_MoveCharacter(sld, "UIPirates");
	}
}
void UnexpectedInheritanceTranslatePart(string part)
{
	if (part == "part1")
	{
		AddQuestRecord("UnexpectedInheritance", "2");
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1 = "locator";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.location = "Mayak9";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.locator_group = "box";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.locator = "box1";
		pchar.quest.UnexpectedInheritanceGetPartTwo.function = "UnexpectedInheritanceGetPartTwo";
	}
	if (part == "part2")
	{
		AddQuestRecord("UnexpectedInheritance", "4");
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1 = "locator";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.location = "FortOrange_Grot";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.locator_group = "box";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.locator = "box1";
		pchar.quest.UnexpectedInheritanceGetPartTwo.function = "UnexpectedInheritanceGetPartThree";
	}
	if (part == "part3")
	{
		AddQuestRecord("UnexpectedInheritance", "6");
		UnexpectedInheritanceSeaBattle();

		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1 = "location";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.location = "Dominica_Grot";
		pchar.quest.UnexpectedInheritanceGetPartTwo.function = "UnexpectedInheritanceGrottoPirates";
	}
	if (part == "part4")
	{
		AddQuestRecord("UnexpectedInheritance", "8");
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1 = "locator";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.location = "FortFrance_fort";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.locator_group = "box";
		pchar.quest.UnexpectedInheritanceGetPartTwo.win_condition.l1.locator = "box1";
		pchar.quest.UnexpectedInheritanceGetPartTwo.function = "UnexpectedInheritanceGetPartFive";
	}
}

void scareOfficers(int minSkill)
{
	int i, idx;
	for(i = 1; i <= MAX_NUM_FIGHTERS; i++ )
	{
		if (CheckOfficersPerk(pchar, "IronWill"))
	    {
			minSkill = minSkill * 0.7;
		}
		idx = GetOfficersIndex(pchar,i);
		if( idx == -1 ) continue;
		sld = GetCharacter(idx);
		if (sld.location == pchar.location )
		{
			if(!CheckAttribute(sld, "HasNoFear"))
			{
				if(minSkill + drand(30+i) > sti(GetSummonSkillFromName(sld, SKILL_LEADERSHIP)))
				{
					ChangeCharacterAddressGroup(sld, Get_My_Cabin(), "goto",  "goto1");
					Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_39", "Logs.txt", "#space#", " ")+GetFullName(sld)+GetConvertStrWithReplace("Variable_reaction_functions_40", "Logs.txt", "#space#", " ")+NPCharSexPhrase(sld,"",GetConvertStrWithReplace("Variable_reaction_functions_41", "Logs.txt", "#space#", " "))+ GetConvertStrWithReplace("Variable_reaction_functions_42", "Logs.txt", "#space#", " "));
				}
				else
				{
					Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_43", "Logs.txt", "#space#", " ")+GetFullName(sld)+GetConvertStrWithReplace("Variable_reaction_functions_44", "Logs.txt", "#space#", " ")+NPCharSexPhrase(sld,"",GetConvertStrWithReplace("Variable_reaction_functions_45", "Logs.txt", "#space#", " "))+ GetConvertStrWithReplace("Variable_reaction_functions_46", "Logs.txt", "#space#", " "));
				}
			}
			else
			{
				Log_Info(GetFullName(sld)+GetConvertStrWithReplace("Variable_reaction_functions_47", "Logs.txt", "#space#", " ")+NPCharSexPhrase(sld,GetConvertStrWithReplace("Variable_reaction_functions_48", "Logs.txt", "#space#", " "),GetConvertStrWithReplace("Variable_reaction_functions_49", "Logs.txt", "#space#", " "))+ GetConvertStrWithReplace("Variable_reaction_functions_50", "Logs.txt", "#space#", " "));
			}
		}
	}
}

//Квест. Проигравшийся игрок. -->
void LooserGenerator_sart_Magazin(string s)		//Украл владелец магазина
{
	ChangeCharacterReputation(pchar, -5);

	ref chr = CharacterFromID(pchar.HOTP_CasinoQuest.npcharID);
	chr.dialog.filename = "Quest\PDM\looser.c";
	chr.dialog.currentnode = "Jdet_1";
	DeleteAttribute(chr, "lifeday");

	ReOpenQuestHeader("KIP_Looser");
	AddQuestRecord("KIP_Looser", "1.1");
	AddQuestUserData("KIP_Looser", "sSex", GetSexPhrase("ся","ась"));
	AddQuestUserData("KIP_Looser", "sName", GetFullName(chr));

	pchar.HOTP_CasinoQuest.store = chr.city + "_store";

	DoQuestFunctionDelay("LooserGenerator_SetStoreLocation", 3.0);

	// Таймер на провал квеста = 1 день
	SetTimerFunction("LooserGenerator_TimeFailed", 0, 0, 1);
}
void LooserGenerator_sart_Verf(string s)	//Украл владелец верфи
{
	ChangeCharacterReputation(pchar, -5);

	ref chr = CharacterFromID(pchar.HOTP_CasinoQuest.npcharID);
	chr.dialog.filename = "Quest\PDM\looser.c";
	chr.dialog.currentnode = "Jdet_1";
	DeleteAttribute(chr, "lifeday");

	ReOpenQuestHeader("KIP_Looser");
	AddQuestRecord("KIP_Looser", "1.2");
	AddQuestUserData("KIP_Looser", "sSex", GetSexPhrase("ся","ась"));
	AddQuestUserData("KIP_Looser", "sName", GetFullName(chr));

	pchar.HOTP_CasinoQuest.shipyard = chr.city + "_shipyard";

	DoQuestFunctionDelay("LooserGenerator_SetShipyardLocation", 3.0);

	// Таймер на провал квеста = 1 день
	SetTimerFunction("LooserGenerator_TimeFailed", 0, 0, 1);
}
void LooserGenerator_SetStoreLocation(string s)	// Выставлялка прерывания на локации
{
	pchar.quest.LooserGeneratorSart.win_condition.l1 = "location";
	pchar.quest.LooserGeneratorSart.win_condition.l1.location = "CommonPackhouse_1";
	pchar.quest.LooserGeneratorSart.function = "LooserGenerator_InStore";
}

void LooserGenerator_SetShipyardLocation(string s)
{
	pchar.quest.LooserGeneratorSart2.win_condition.l1 = "location";
	pchar.quest.LooserGeneratorSart2.win_condition.l1.location = "CommonPackhouse_2";
	pchar.quest.LooserGeneratorSart2.function = "LooserGenerator_InShipyard";
}
void LooserGenerator_InStore(string s)		// Заходим в магазин
{
	int n = FindLocation(pchar.location);
	int i;
	string str;

	if(CheckAttribute(pchar, "HOTP_CasinoQuest.store") && locations[n].reload.l1.go == pchar.HOTP_CasinoQuest.store)
	{
		DeleteAttribute(pchar, "HOTP_CasinoQuest.store");
		Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_51", "Logs.txt", "#space#", " "));
		LAi_LocationFightDisable(loadedLocation, true);
		pchar.KIP_PI_SleditZaNami = "KIP_PI_SleditZaNami";
		chrDisableReloadTolocation = true;
		InterfaceStates.Buttons.Save.enable = false;

		int qty = 0;
        int iFlag [8];
        int iBagsQty;
        int iSumm;
        int iRnd;
        string sBox;
		// LEO: РОВНО 7 кошельков по 6 сундукам (box1..box6), макс. 2 в сундук, в случайные.
		// Прошлая версия ЗАВИСАЛА НАМЕРТВО при переходе на склад: rand(6)=0..6 давал box7 (которого нет),
		// ветка iSumm>7 не увеличивала qty, и при исчерпании сундуков while(qty<7) крутился вечно.
		int iCount[7];   // кошельков в сундуке, индексы 1..6 (Storm инициализирует нулями)
		int iGuard = 0;
		int iBox;
		while (qty < 7)
		{
			iRnd = 1 + rand(5);          // 1..6 (rand инклюзивный)
			if (iCount[iRnd] < 2)        // не больше 2 кошельков в один сундук
			{
				iCount[iRnd] = iCount[iRnd] + 1;
				qty++;
			}
			iGuard++;
			if (iGuard > 1000) break;    // подстраховка от вечного цикла
		}
		for (iBox = 1; iBox <= 6; iBox++)
		{
			if (iCount[iBox] > 0)
			{
				sBox = "box" + iBox;
				pchar.GenQuestBox.CommonPackhouse_1.(sBox).items.Bag_with_money = iCount[iBox];
			}
		}

		return;
	}

	if(CheckAttribute(pchar, "HOTP_CasinoQuest.store"))	// Если вдруг решили зайти в другой город
	{
		DoQuestFunctionDelay("LooserGenerator_SetStoreLocation", 3.0);
	}
}
void LooserGenerator_InShipyard(string s)		// Заходим на верфь
{
	int n = FindLocation(pchar.location);
	int i;
	string str;

	if(CheckAttribute(pchar, "HOTP_CasinoQuest.shipyard") && locations[n].reload.l1.go == pchar.HOTP_CasinoQuest.shipyard)
	{
		DeleteAttribute(pchar, "HOTP_CasinoQuest.shipyard");
		Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_52", "Logs.txt", "#space#", " "));
		LAi_LocationFightDisable(loadedLocation, true);
		pchar.KIP_PI_SleditZaNami = "KIP_PI_SleditZaNami";
		chrDisableReloadTolocation = true;
		InterfaceStates.Buttons.Save.enable = false;

		int qty = 0;
        int iFlag [8];
        int iBagsQty;
        int iSumm;
        int iRnd;
        string sBox;
		// LEO: РОВНО 7 кошельков по 6 сундукам (box1..box6), макс. 2 в сундук, в случайные.
		// Прошлая версия ЗАВИСАЛА НАМЕРТВО при переходе на склад: rand(6)=0..6 давал box7 (которого нет),
		// ветка iSumm>7 не увеличивала qty, и при исчерпании сундуков while(qty<7) крутился вечно.
		int iCount[7];   // кошельков в сундуке, индексы 1..6 (Storm инициализирует нулями)
		int iGuard = 0;
		int iBox;
		while (qty < 7)
		{
			iRnd = 1 + rand(5);          // 1..6 (rand инклюзивный)
			if (iCount[iRnd] < 2)        // не больше 2 кошельков в один сундук
			{
				iCount[iRnd] = iCount[iRnd] + 1;
				qty++;
			}
			iGuard++;
			if (iGuard > 1000) break;    // подстраховка от вечного цикла
		}
		for (iBox = 1; iBox <= 6; iBox++)
		{
			if (iCount[iBox] > 0)
			{
				sBox = "box" + iBox;
				pchar.GenQuestBox.CommonPackhouse_2.(sBox).items.Bag_with_money = iCount[iBox];
			}
		}

		return;
	}

	if(CheckAttribute(pchar, "HOTP_CasinoQuest.shipyard"))	// Если вдруг решили зайти в другой город
	{
		DoQuestFunctionDelay("LooserGenerator_SetShipyardLocation", 3.0);
	}
}
void LooserGenerator_OpenLocation(string s)		// Открываем локацию
{
	chrDisableReloadTolocation = false;
	LAi_LocationFightDisable(loadedLocation, false);
	InterfaceStates.Buttons.Save.enable = true;

	DeleteAttribute(&locations[FindLocation(pchar.location)], "NotSpawnItem");

	ref chr = CharacterFromID(pchar.HOTP_CasinoQuest.npcharID);
	chr.CasinoQuest_ok = true;
	chr.dialog.filename = "Quest\PDM\looser.c";
	chr.dialog.currentnode = "quest";

	DeleteQuestCheck("LooserGeneratorFailed");

}
void LooserGenerator_Complette()		// Успешное завершение квеста
{
	DeleteAttribute(pchar, "items.Bag_with_money");
	DeleteAttribute(pchar, "KIP_PI_SleditZaNami");
	PChar.quest.LooserGenerator_TimeFailed.over = "yes";

	ref chr = CharacterFromID(pchar.HOTP_CasinoQuest.npcharID);
	chr.lifeday = 0;
	CloseQuestHeader("KIP_Looser");
	pchar.questdata.Looser = 1;
}
void LooserGenerator_TimeFailed(string s)		// Провал квеста по времени
{
	DeleteQuestCheck("LooserGeneratorSart");
	DeleteQuestCheck("LooserGeneratorSart2");

	DeleteAttribute(pchar, "items.Bag_with_money");
	DeleteAttribute(pchar, "KIP_PI_SleditZaNami");

	Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_53", "Logs.txt", "#space#", " "));
	AddQuestRecord("KIP_Looser", "2");
	CloseQuestHeader("KIP_Looser");
	pchar.questdata.Looser = 1;

	ref chr = CharacterFromID(pchar.HOTP_CasinoQuest.npcharID);
	chr.lifeday = 0;
	chr.dialog.filename = "Quest\PDM\looser.c";
	chr.dialog.currentnode = "first time";
}
void LooserGenerator_FailedByEnc(string s)		// Поймали с поличным
{
	chrDisableReloadTolocation = false;
	LAi_LocationFightDisable(loadedLocation, false);
	InterfaceStates.Buttons.Save.enable = true;

	DeleteAttribute(&locations[FindLocation(pchar.location)], "NotSpawnItem");

	ref chr = CharacterFromID(pchar.HOTP_CasinoQuest.npcharID);
	chr.lifeday = 0;
	chr.CasinoQuest_ok = true;
	chr.dialog.filename = "Quest\PDM\looser.c";
	chr.dialog.currentnode = "first time";

	DeleteQuestCheck("LooserGeneratorSart");
	DeleteQuestCheck("LooserGeneratorSart2");
	DeleteAttribute(pchar, "items.Bag_with_money");
	pchar.KIP_PI_ZapisVSJ = "KIP_PI_ZapisVSJ";
	DeleteAttribute(pchar, "KIP_PI_SleditZaNami");
	DeleteAttribute(pchar, "HOTP_CasinoQuest");
	PChar.quest.LooserGenerator_TimeFailed.over = "yes";

	Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_54", "Logs.txt", "#space#", " "));
	AddQuestRecord("KIP_Looser", "2");
	CloseQuestHeader("KIP_Looser");
	pchar.questdata.Looser = 1;
}
void LooserGenerator_DopProverka(string s)		// Дополнительная проверка на наличие 7+ кошельков
{
	if (GetCharacterItem(pchar,"Bag_with_money") >= 7 && !CheckAttribute(pchar, "KIP_PI_ZapisVSJ"))
	{
		AddQuestRecord("KIP_Looser", "3");
		AddQuestUserData("KIP_Looser", "sSex", GetSexPhrase("ёл","ла"));
		DoQuestFunctionDelay("LooserGenerator_OpenLocation", 0.1);
		pchar.KIP_PI_ZapisVSJ = "KIP_PI_ZapisVSJ";
	}
}
//<-- Квест. Проигравшийся игрок.

//Квест. Чудесное спасение на рифах. -->
void KSM_Snr_Nashli_Ship(string qName)
{
	// LEO: в старом сейве Аллока может отсутствовать; не запускаем сцену с NullCharacter.
	if (GetCharacterIndex("KSM_Alloka") < 0)
	{
		if (pchar.location != "WreckedShip") return;
		if (!CheckAttribute(pchar, "QuestInfo.KSM_Spasenie_na_rifah.complete")) return;
		if (sti(pchar.QuestInfo.KSM_Spasenie_na_rifah.complete) != 0) return;
		if (CheckAttribute(pchar, "questdata.SpasenieNaRifah"))
		{
			if (sti(pchar.questdata.SpasenieNaRifah) != 0) return;
		}
		int iAlloka = NPC_GenerateCharacter("KSM_Alloka", "trader_1", "man", "man", 10, ENGLAND, -1, false);
		if (iAlloka < 0) return;
		sld = GetCharacter(iAlloka);
		sld.name = GetConvertStr("Variable_quests_reaction_0.13.1", "Names.txt");
		sld.lastname = GetConvertStr("Variable_quests_reaction_0.14.1", "Names.txt");
		LAi_SetImmortal(sld, true);
		SetCharacterRemovable(sld, false);
	}
	bDisableFastReload = true;
	chrDisableReloadToLocation = true;
	StartQuestMovie(true, false, true);
	DoQuestFunctionDelay("KSM_Snr_Nashli_Ship_2", 3.0);
	DoQuestFunctionDelay("KSM_Snr_Nashli_Ship_Cam", 0.1);
	ChangeCharacterAddressGroup(pchar, "WreckedShip", "rld", "loc2");
	DoQuestFunctionDelay("UbratPortret", 0.1);
	LAi_SetActorType(pchar);
	LAi_ActorGoToLocator(pchar, "goto", "goto4", "", -1);
	
	sld = CharacterFromID("KSM_Alloka")
	FantomMakeCoolFighter(sld, sti(pchar.rank) + 5, 15 + MOD_SKILL_ENEMY_RATE * 4, 15 + MOD_SKILL_ENEMY_RATE * 4, "blade46", "pistol11", 50 + MOD_SKILL_ENEMY_RATE * 4);
	sld.SaveItemsForDead = true;
	sld.DontChangeGun = true;
	TakeNItems(sld, "potionwine", 2);
	AddMoneyToCharacter(sld, 2000);
	ChangeCharacterAddressGroup(sld, "WreckedShip", "rld", "aloc14");
	LAi_SetActorType(sld);
	sld.Dialog.Filename = "Quest\KSM\Spasenie_na_rifah.c";
	sld.dialog.currentnode   = "REEFS";
	
	//Наши матросы
	for (i=1; i<=2; i++)
	{
		if (pchar.sex != "skeleton")
		{
			sTemp = "shipowner_"+(rand(28)+1);
		}
		else
		{
			sTemp = "Skel_" + (rand(4)+1);
		}
		sld = GetCharacter(NPC_GenerateCharacter("KSM_Snr_Matrosiki_"+i, sTemp, "man", "man", sti(pchar.rank), PIRATE, -1, true));
		ChangeCharacterAddressGroup(sld, "WreckedShip", "goto", "goto1");
		LAi_SetActorType(sld);
		LAi_ActorFollow(sld, pchar, "", -1);
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
	}
	if (pchar.sex != "skeleton")
	{
		sTemp = "shipowner_"+(rand(28)+1);
	}
	else
	{
		sTemp = "Skel_" + (rand(4)+1);
	}
	sld = GetCharacter(NPC_GenerateCharacter("KSM_Snr_Matrosiki_3", sTemp, "man", "man", sti(pchar.rank), PIRATE, -1, true));
	ChangeCharacterAddressGroup(sld, "WreckedShip", "goto", "goto1");
	LAi_SetActorType(sld);
	LAi_ActorGoToLocator(sld, "rld", "aloc9", "", -1);
	LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
	//Вражеские мушкетёры
	sTemp = "Mushketer_"+(rand(24)+1);
	sld = GetCharacter(NPC_GenerateMusketeer("KSM_Snr_Mushkety_1", sTemp, "man", sti(pchar.rank), PIRATE, -1, false));
	sld.MusketerDistance = 0;
	ChangeCharacterAddressGroup(sld, "WreckedShip", "rld", "aloc12");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	
	sTemp = "Mushketer_"+(rand(24)+1);
	sld = GetCharacter(NPC_GenerateMusketeer("KSM_Snr_Mushkety_2", sTemp, "man", sti(pchar.rank), PIRATE, -1, false));
	sld.MusketerDistance = 0;
	ChangeCharacterAddressGroup(sld, "WreckedShip", "rld", "aloc13");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	
	sTemp = "Mushketer_"+(rand(24)+1);
	sld = GetCharacter(NPC_GenerateMusketeer("KSM_Snr_Mushkety_3", sTemp, "man", sti(pchar.rank), PIRATE, -1, false));
	sld.MusketerDistance = 0;
	ChangeCharacterAddressGroup(sld, "WreckedShip", "rld", "aloc15");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	
}
void KSM_Snr_Nashli_Ship_Cam(string qName)
{
	locCameraFromToPos(9.50, 16.00, -19.00, false, -10.00, 8.00, -20.00);
	Locations[FindLocation(pchar.location)].box1.items.grenade = 5;
	Locations[FindLocation(pchar.location)].box1.items.grapeshot = 15;
	Locations[FindLocation(pchar.location)].box1.items.bullet = 20;
	Locations[FindLocation(pchar.location)].box1.items.GunPowder = 25;
	Locations[FindLocation(pchar.location)].box1.items.Mushket_english = 1;
	Locations[FindLocation(pchar.location)].box1.items.compcraft_sulfur = 20;
	Locations[FindLocation(pchar.location)].box1.items.compcraft_powdermixture = 20;
	
	Locations[FindLocation(pchar.location)].box2.items.chest = 1;
	Locations[FindLocation(pchar.location)].box2.items.jewelry1 = rand(10)+10;
	Locations[FindLocation(pchar.location)].box2.items.jewelry2 = rand(10)+10;
	Locations[FindLocation(pchar.location)].box2.items.jewelry3 = rand(10)+10;
	Locations[FindLocation(pchar.location)].box2.items.jewelry4 = rand(10)+10;
	Locations[FindLocation(pchar.location)].box2.items.jewelry17 = rand(20)+10;
	
	Locations[FindLocation(pchar.location)].box3.items.chest = 1;
	Locations[FindLocation(pchar.location)].box3.items.jewelry5 = rand(20)+10;
	Locations[FindLocation(pchar.location)].box3.items.jewelry17 = rand(10)+10;
	Locations[FindLocation(pchar.location)].box3.items.potionrum = rand(2)+2;
}
void KSM_Snr_Nashli_Ship_2(string qName)
{
	locCameraFromToPos(-8.50, 16.00, -30.00, true, 10.00, 13.00, 5.00);
	DoQuestFunctionDelay("KSM_Snr_Nashli_Ship_3", 5.0);
	ChangeCharacterAddressGroup(pchar, "WreckedShip", "rld", "loc12");
	LAi_SetActorType(pchar);
	LAi_ActorGoToLocator(pchar, "goto", "goto4", "", -1);
}
void KSM_Snr_Nashli_Ship_3(string qName)
{
	locCameraFromToPos(-5.50, 16.00, -10.00, false, 3.00, 11.30, 5.00);
	DoQuestFunctionDelay("KSM_Snr_Razgovor_1", 4.0);
}
void KSM_Snr_Razgovor_1(string qName)
{
	LAi_SetPlayerType(pchar);
	sld = CharacterFromID("KSM_Alloka")
	LAi_ActorDialogNow(sld, Pchar, "", -1);
}
void UbratPortret(string qName)
{
	ChangeShowIntarface();
}
//<-- Квест. Чудесное спасение на рифах.

//Квест. Кот в мешке. -->
void CSP_KvM_KusayuTebya_1(string qName)
{
	DoQuestFunctionDelay("CSP_KvM_KusayuTebya_2", 2.0);
	if (MOD_SKILL_ENEMY_RATE == 1) LAi_ApplyCharacterDamage(pchar, 8);
	if (MOD_SKILL_ENEMY_RATE == 2) LAi_ApplyCharacterDamage(pchar, 10);
	if (MOD_SKILL_ENEMY_RATE == 3) LAi_ApplyCharacterDamage(pchar, 11);
	LAi_CheckKillCharacter(pchar);
}
void CSP_KvM_KusayuTebya_2(string qName)
{
	DoQuestFunctionDelay("CSP_KvM_KusayuTebya_1", 2.0);
	if (MOD_SKILL_ENEMY_RATE == 1) LAi_ApplyCharacterDamage(pchar, 8);
	if (MOD_SKILL_ENEMY_RATE == 2) LAi_ApplyCharacterDamage(pchar, 10);
	if (MOD_SKILL_ENEMY_RATE == 3) LAi_ApplyCharacterDamage(pchar, 11);
	LAi_CheckKillCharacter(pchar);
}
//<-- Квест. Кот в мешке.

void FishHDS(string s)
{
	TakeItemFromCharacter(pchar, "letter_notes");
	Log_info(GetConvertStr("Variable_reaction_functions_55", "Logs.txt"));
	ChangeItemDescribe("letter_notes", "itmdescr_letter_notes");
}

void Rand_ContraFinal(string s)
{
	pchar.quest.Rand_ContraFinale.win_condition.l1 = "ExitFromLocation";
	pchar.quest.Rand_ContraFinale.win_condition.l1.location = pchar.location;
	pchar.quest.Rand_ContraFinale.win_condition.function = "Rand_Clear";
}

void Rand_Clear(string s)
{
	DeleteAttribute(pchar,"ContraInter");
}

void Pedro_Horse(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_SetActorType(CharacterFromID(pchar.PedroID));
	LAi_ActorDialog(CharacterFromID(pchar.PedroID), pchar, "", -1, 0);
}

void ActiveShowAfterReload(string qName)
{
	aref arPerksRoot,arPerk;
	makearef(arPerksRoot,pchar.perks.list);
	int perksQ = GetAttributesNum(arPerksRoot);

	string stmp;
	int idx = 0;
	for(int j=0; j<perksQ; j++)
	{
		arPerk = GetAttributeN(arPerksRoot,j);
		if( CheckAttribute(arPerk,"delay") )
		{
			if( CheckAttribute(arPerk,"active") )
			{
				string tmpStr = GetAttributeName(arPerk);
				AddAndUpdatePerkIcon(GetAttributeName(arPerk), GetPerkPictureIndex(GetAttributeName(arPerk))); // LEO: переподхват на статичной ячейке (маска-FX)
			}
		}
	}

	if(bSeaActive && !bAbordageStarted)
	{	// морская часть
		LayerAddObject(SEA_EXECUTE,&objActivePerkShower,-1);
		LayerAddObject(SEA_REALIZE,&objActivePerkShower,-1);
	}
}
void UbratTrebovanieRangov(string qName)
{
	pchar.quest.DozorPrepare.win_condition.l1 = "Rank";
	pchar.quest.DozorPrepare.win_condition.l1.value = 1;
	pchar.quest.DozorPrepare.win_condition.l1.operation = ">=";
	PChar.quest.DozorPrepare.function = "DozorPrepare_2";

	pchar.quest.MC_startInCaracasRang.win_condition.l1 = "Rank";
	pchar.quest.MC_startInCaracasRang.win_condition.l1.value = 1;
	pchar.quest.MC_startInCaracasRang.win_condition.l1.operation = ">=";
	pchar.quest.MC_startInCaracasRang.function = "MC_startInCaracasRang";

	pchar.quest.Ostin_SpawnBanditov.win_condition.l1 = "Rank";
	pchar.quest.Ostin_SpawnBanditov.win_condition.l1.value = 1;
	pchar.quest.Ostin_SpawnBanditov.win_condition.l1.operation = ">=";
	PChar.quest.Ostin_SpawnBanditov.function = "Birglars_Spawn";

	pchar.quest.PDM_NK_Nachalo.win_condition.l1 = "Rank";
	pchar.quest.PDM_NK_Nachalo.win_condition.l1.value = 1;
	pchar.quest.PDM_NK_Nachalo.win_condition.l1.operation = ">=";
	PChar.quest.PDM_NK_Nachalo.win_condition = "PDM_NK_Nachalo";

	pchar.quest.Romantic_Pirat_Spawn.win_condition.l1 = "Rank";
	pchar.quest.Romantic_Pirat_Spawn.win_condition.l1.value = 1;
	pchar.quest.Romantic_Pirat_Spawn.win_condition.l1.operation = ">=";
	PChar.quest.Romantic_Pirat_Spawn.win_condition = "Romantic_Pirat_Spawn";

	// LEO: «Проклятый идол» Нежити недоступен. Отмена стоит в Undead_Start_Graveyard_1, но она приходит позже:
	//   условие «ранг >= 1» срабатывает сразу, и Кэллоу успевал сесть в таверну Ла-Веги до отмены.
	if (startHeroType != 8)
	{
		pchar.quest.PDM_CI_SpawnJC.win_condition.l1 = "Rank";
		pchar.quest.PDM_CI_SpawnJC.win_condition.l1.value = 1;
		pchar.quest.PDM_CI_SpawnJC.win_condition.l1.operation = ">=";
		PChar.quest.PDM_CI_SpawnJC.win_condition = "PDM_CI_SpawnJC";
	}

	pchar.quest.PDM_ONV_Nachalo.win_condition.l1 = "Rank";
	pchar.quest.PDM_ONV_Nachalo.win_condition.l1.value = 1;
	pchar.quest.PDM_ONV_Nachalo.win_condition.l1.operation = ">=";
	PChar.quest.PDM_ONV_Nachalo.win_condition = "PDM_ONV_Nachalo";

	pchar.quest.PKM_SvtvA_SpawnQuest.win_condition.l1 = "Rank";
	pchar.quest.PKM_SvtvA_SpawnQuest.win_condition.l1.value = 1;
	pchar.quest.PKM_SvtvA_SpawnQuest.win_condition.l1.operation = ">=";
	PChar.quest.PKM_SvtvA_SpawnQuest.win_condition = "PKM_SvtvA_SpawnQuest";

	pchar.quest.KSM_Snr_Alloka_Spawn.win_condition.l1 = "Rank";
	pchar.quest.KSM_Snr_Alloka_Spawn.win_condition.l1.value = 1;
	pchar.quest.KSM_Snr_Alloka_Spawn.win_condition.l1.operation = ">=";
	PChar.quest.KSM_Snr_Alloka_Spawn.win_condition = "KSM_Snr_Alloka_Spawn";

	pchar.quest.KPS_ZO_JimSmit_Spawn.win_condition.l1 = "Rank";
	pchar.quest.KPS_ZO_JimSmit_Spawn.win_condition.l1.value = 1;
	pchar.quest.KPS_ZO_JimSmit_Spawn.win_condition.l1.operation = ">=";
	PChar.quest.KPS_ZO_JimSmit_Spawn.win_condition = "KPS_ZO_JimSmit_Spawn";

	pchar.quest.CSP_NN_SpawnGirl.win_condition.l1 = "Rank";
	pchar.quest.CSP_NN_SpawnGirl.win_condition.l1.value = 1;
	pchar.quest.CSP_NN_SpawnGirl.win_condition.l1.operation = ">=";
	PChar.quest.CSP_NN_SpawnGirl.function = "UnexpectedInheritance";

	pchar.quest.CSP_KvM_SpawnRibak.win_condition.l1 = "Rank";
	pchar.quest.CSP_KvM_SpawnRibak.win_condition.l1.value = 1;
	pchar.quest.CSP_KvM_SpawnRibak.win_condition.l1.operation = ">=";
	PChar.quest.CSP_KvM_SpawnRibak.win_condition = "CSP_KvM_SpawnRibak";

	pchar.quest.Kuali_Spawn.win_condition.l1 = "Rank";
	pchar.quest.Kuali_Spawn.win_condition.l1.value = 1;
	pchar.quest.Kuali_Spawn.win_condition.l1.operation = ">=";
	PChar.quest.Kuali_Spawn.win_condition = "Kuali_Spawn";

	// Борода: подстраховка для Томаса на случай "квестов без ранга"
	pchar.quest.TohmasPetit.win_condition.l1 = "Rank";
	pchar.quest.TohmasPetit.win_condition.l1.value = 1;
	pchar.quest.TohmasPetit.win_condition.l1.operation = ">=";
	PChar.quest.TohmasPetit.function = "TomasBorn";

	// Gregg: «Исчезновение людей» (перенос из К:ПС) - в норме ждёт 10 ранга.
	// Гейт «Корабля обезьян» сюда не попадает: он живёт в диалоге Аллена и читает
	// bQuestsRank напрямую, как это сделано у Тилльманна и Сайфуса.
	pchar.quest.DisappearPeoplePreStart.win_condition.l1 = "Rank";
	pchar.quest.DisappearPeoplePreStart.win_condition.l1.value = 1;
	pchar.quest.DisappearPeoplePreStart.win_condition.l1.operation = ">=";
	PChar.quest.DisappearPeoplePreStart.function = "DisappearPeoplePreStart";
}

///////////////////Тич сбежал///////////////////
void BlackBeardRaid_FailEscape(string qName)
{
	ref bb = characterFromId("BlackBeardNPC");
	if (!CheckAttribute(pchar, "questTemp.BlackBeardRaidStarted")) return;
	if (LAi_IsDead(bb)) return;

	RemoveCharacterCompanion(pchar, bb);
	Map_ReleaseQuestEncounter(bb.id);
	Group_DeleteGroup("BlackBeardRaidGroup");
	Group_DeleteGroup("Sea_" + bb.id);
	DeleteAttribute(&NullCharacter, "capitainBase.BlackBeardNPC");
	DeleteAttribute(bb, "quest");
	DeleteAttribute(bb, "mapEnc");
	bb.PGGAi.RemoveEncounter = true;
	pchar.quest.BlackBeardRaid_GroupDead.over = "yes";
	pchar.quest.BlackBeardRaidTimer.over = "yes";
	DeleteAttribute(pchar, "questTemp.BlackBeardRaidStarted");
	DeleteAttribute(pchar, "questTemp.BlackBeardRaidStartIsland");
	pchar.Titch_seabattle = true;
	Log_Info(GetConvertStr("Variable_reaction_functions_58", "Logs.txt"));
}

///////////////////Тич не дождался///////////////////
void BlackBeardRaid_TimerExpired(string qName)
{
	ref bb = characterFromId("BlackBeardNPC");
	if (!CheckAttribute(pchar, "questTemp.BlackBeardRaidStarted")) return;
	if (LAi_IsDead(bb)) return;

	RemoveCharacterCompanion(pchar, bb);
	Map_ReleaseQuestEncounter(bb.id);
	Group_DeleteGroup("BlackBeardRaidGroup");
	Group_DeleteGroup("Sea_" + bb.id);
	DeleteAttribute(&NullCharacter, "capitainBase.BlackBeardNPC");
	DeleteAttribute(bb, "quest");
	DeleteAttribute(bb, "mapEnc");
	bb.PGGAi.RemoveEncounter = true;
	pchar.quest.BlackBeardRaid_GroupDead.over = "yes";
	pchar.quest.BlackBeardRaid_FailEscape.over = "yes";
	pchar.quest.BlackBeardRaid_EnableFailEscape.over = "yes";
	DeleteAttribute(pchar, "questTemp.BlackBeardRaidStarted");
	DeleteAttribute(pchar, "questTemp.BlackBeardRaidStartIsland");
	Log_Info(GetConvertStr("Variable_reaction_functions_59", "Logs.txt"));
}

//////////////////////////////////////////////////////
///////////////////Квесты ИП//////////////////////////
//////////////////////////////////////////////////////

///////////////////Трусливый Томас///////////////////

//Спавним Томаса на 5 ранге
// Борода: сигнатура ОБЯЗАТЕЛЬНО (string qName) - функция вешается на quest.TohmasPetit.function
// и зовётся через DoQuestFunctionDelay; без параметра движок валился с "function TomasBorn stack error".
void TomasBorn(string qName)
{
	ref chr;
	int iChar;
	if (GetCharacterIndex("TohmasPetit") != -1) return; // Томас уже создан - не спавним второй раз
	if (sti(pchar.questdata.TohmasPetit) == 1) return; // квест уже пройден/закрыт - Томас больше не нужен
	iChar = NPC_GeneratePhantomCharacter("citizen", FRANCE, MAN, -1);
	chr = &characters[iChar];
	chr.id = "TohmasPetit";
	chr.name = "Томас";
	chr.lastname = "Петит";
	chr.model = "usurer_3";
	chr.dialog.filename = "Quest\IP\Thomas\thomas.c";
	chr.dialog.currentnode = "TohmasPetit_1";
	chr.talker = 10;
	FaceMaker(chr);
	LAi_RemoveLoginTime(chr);
	LAi_SetCitizenType(chr);
	LAi_group_MoveCharacter(chr, "FRANCE_CITIZENS");
	ChangeCharacterAddressGroup(chr, "BasTer_town", "goto", "goto20");	
}

void TohmasPetit_ClearCorpsesAfterReload(string qName)
{
	ref sld;
	sld = CharacterFromID("TohmasPetit");
	ChangeCharacterAddressGroup(sld, "none", "", "");

	sld = CharacterFromID("TohmasPetit_girl");
	ChangeCharacterAddressGroup(sld, "none", "", "");
}

// Борода: снять с глобалки ТОЛЬКО метку энкаунтера, сам корабль и группы не трогаем.
// Зовётся сразу как решена судьба судна (конец переговоров с капитаном): иначе метка живёт
// до закрытия квеста, и "Атлантику" успевает затянуть в чужой морской бой. Корабль при этом
// остаётся в текущем бою - абордаж после мирной развязки должен работать.
void TohmasPetit_ReleaseShipEncounter()
{
	int iShip = GetCharacterIndex("TohmasPetit_TomasShip");
	if (iShip == -1) return;
	ref sld = &characters[iShip];
	Map_ReleaseQuestEncounter(sld.id);
	DeleteAttribute(sld, "mapEnc");
	sld.PGGAi.RemoveEncounter = true;
}

// Борода: снять "Атлантику" с глобальной карты. Group_DeleteGroup мало: корабль поставлен
// через Map_CreateTraderXZ, а это отдельный quest-encounter на карте мира - без
// Map_ReleaseQuestEncounter он остаётся плавать и его можно захватить после конца квеста.
// Идиома взята из BlackBeardRaid (reaction_functions.c). Безопасна, если корабля уже нет.
void TohmasPetit_RemoveShip()
{
	int iShip = GetCharacterIndex("TohmasPetit_TomasShip");
	if (iShip == -1) return;
	ref sld = &characters[iShip];
	Map_ReleaseQuestEncounter(sld.id);
	Group_DeleteGroup("TohmasPetit_ShipGroup");
	Group_DeleteGroup("Sea_" + sld.id);
	DeleteAttribute(sld, "mapEnc");
	sld.PGGAi.RemoveEncounter = true;
}

// Борода: единая уборка временных атрибутов квеста - зовётся из КАЖДОЙ ветки закрытия.
// Все они сиблинги (TohmasPetit_xxx), а не поддерево, поэтому одним удалением не снимаются
// и каждый надо гасить поимённо. DeleteAttribute на отсутствующем атрибуте безопасен.
void TohmasPetit_ClearQuestTemp()
{
	TohmasPetit_RemoveShip(); // квест закрыт - "Атлантики" на карте быть не должно
	DeleteAttribute(pchar, "QuestTemp.TohmasPetit_Quest");
	DeleteAttribute(pchar, "QuestTemp.TohmasPetit_Search");        // фраза про Томаса у трактирщика
	DeleteAttribute(pchar, "QuestTemp.TohmasPetit_CapTalked");
	DeleteAttribute(pchar, "QuestTemp.TohmasPetit_SecuredPeacefully");
	DeleteAttribute(pchar, "QuestTemp.TohmasPetit_bloodytake");
	DeleteAttribute(pchar, "QuestTemp.TohmasPetit_KilledAnyone");
	DeleteAttribute(pchar, "QuestTemp.ArrestStorage");             // список конфиската
	DeleteAttribute(pchar, "QuestTemp.ArrestPatent");              // какой патент был надет при аресте
	DeleteAttribute(pchar, "questTemp.TohmasPetit_landId");
	DeleteAttribute(pchar, "questTemp.TohmasPetit_placeId");
	DeleteAttribute(pchar, "questTemp.TohmasPetit_ReleaseStage");
	DeleteAttribute(pchar, "questTemp.TohmasPetit_JungleDone");
	DeleteAttribute(pchar, "questTemp.Tohmas_ShipName");

	// Сработавшее условие движок снимает сам, а НЕсработавшие остаются взведёнными и после
	// закрытия могут выстрелить в чужой ветке. Гасим их через over="yes", а НЕ DeleteAttribute:
	// уборщик зовётся из реакции, которую QuestsCheck вызывает посреди обхода pchar.quest, и
	// удаление прямо в итерации роняло игру (креш в CheckAttribute). Помеченные снимет сам
	// QuestsCheck вторым проходом. Ставим только существующим - иначе атрибут создастся заново.
	// win_condition при этом СНОСИМ: основной цикл проверяет только его и про over не знает,
	// поэтому непройденная ветка успела бы выстрелить в этом же тике. Удаление дочернего
	// атрибута список веток не сдвигает - краша, как от DeleteAttribute самой ветки, не будет.
	if (CheckAttribute(pchar, "quest.TohmasPetit_ArrivedMonrlo")) { DeleteAttribute(pchar, "quest.TohmasPetit_ArrivedMonrlo.win_condition"); pchar.quest.TohmasPetit_ArrivedMonrlo.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_ArrivedSafeBay")) { DeleteAttribute(pchar, "quest.TohmasPetit_ArrivedSafeBay.win_condition"); pchar.quest.TohmasPetit_ArrivedSafeBay.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_Caught")) { DeleteAttribute(pchar, "quest.TohmasPetit_Caught.win_condition"); pchar.quest.TohmasPetit_Caught.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_ChaseTimeout")) { DeleteAttribute(pchar, "quest.TohmasPetit_ChaseTimeout.win_condition"); pchar.quest.TohmasPetit_ChaseTimeout.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_ClearCorpses")) { DeleteAttribute(pchar, "quest.TohmasPetit_ClearCorpses.win_condition"); pchar.quest.TohmasPetit_ClearCorpses.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_ClearShip_Case")) { DeleteAttribute(pchar, "quest.TohmasPetit_ClearShip_Case.win_condition"); pchar.quest.TohmasPetit_ClearShip_Case.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_EscaperLeave")) { DeleteAttribute(pchar, "quest.TohmasPetit_EscaperLeave.win_condition"); pchar.quest.TohmasPetit_EscaperLeave.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_EscaperLeave_r")) { DeleteAttribute(pchar, "quest.TohmasPetit_EscaperLeave_r.win_condition"); pchar.quest.TohmasPetit_EscaperLeave_r.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_GatesScene")) { DeleteAttribute(pchar, "quest.TohmasPetit_GatesScene.win_condition"); pchar.quest.TohmasPetit_GatesScene.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_GuardKilled")) { DeleteAttribute(pchar, "quest.TohmasPetit_GuardKilled.win_condition"); pchar.quest.TohmasPetit_GuardKilled.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_InGrot")) { DeleteAttribute(pchar, "quest.TohmasPetit_InGrot.win_condition"); pchar.quest.TohmasPetit_InGrot.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_InGrotSail")) { DeleteAttribute(pchar, "quest.TohmasPetit_InGrotSail.win_condition"); pchar.quest.TohmasPetit_InGrotSail.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_Jungle")) { DeleteAttribute(pchar, "quest.TohmasPetit_Jungle.win_condition"); pchar.quest.TohmasPetit_Jungle.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_LetThemGo")) { DeleteAttribute(pchar, "quest.TohmasPetit_LetThemGo.win_condition"); pchar.quest.TohmasPetit_LetThemGo.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_ShipSunk")) { DeleteAttribute(pchar, "quest.TohmasPetit_ShipSunk.win_condition"); pchar.quest.TohmasPetit_ShipSunk.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_TriggerPlayerStart")) { DeleteAttribute(pchar, "quest.TohmasPetit_TriggerPlayerStart.win_condition"); pchar.quest.TohmasPetit_TriggerPlayerStart.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_TriggerRastanovka")) { DeleteAttribute(pchar, "quest.TohmasPetit_TriggerRastanovka.win_condition"); pchar.quest.TohmasPetit_TriggerRastanovka.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_Uhodit")) { DeleteAttribute(pchar, "quest.TohmasPetit_Uhodit.win_condition"); pchar.quest.TohmasPetit_Uhodit.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_gate")) { DeleteAttribute(pchar, "quest.TohmasPetit_gate.win_condition"); pchar.quest.TohmasPetit_gate.over = "yes"; }
	if (CheckAttribute(pchar, "quest.TohmasPetit_srazgovor")) { DeleteAttribute(pchar, "quest.TohmasPetit_srazgovor.win_condition"); pchar.quest.TohmasPetit_srazgovor.over = "yes"; }

	// Метка «сидит в трюме» на пленниках. Читателей у неё нет, но в сейве висит вечно:
	// снималась только в одной концовке из шести. Индекс проверяем - в ранних ветках отказа
	// персонажей может уже/ещё не быть.
	int iTmp = GetCharacterIndex("TohmasPetit");
	if (iTmp != -1) DeleteAttribute(&characters[iTmp], "TohmasPetitQuestHold");
	iTmp = GetCharacterIndex("TohmasPetit_girl");
	if (iTmp != -1) DeleteAttribute(&characters[iTmp], "TohmasPetitQuestHold");
}
