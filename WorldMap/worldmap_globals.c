

#define WDM_MAP_TO_SEA_SCALE				50
#define WDM_MAP_ENCOUNTERS_TO_SEA_SCALE		25	// boal	18.02.06 80 - это много, нужно обязательнй бой, когда догнали	// 80 VANO был тут, чисто для того чтобы корабли ближе появлялись в море, а то они на горизонте тусуются

#define WDM_NONE_ISLAND				""

#define WDM_ETYPE_MERCHANT	0
#define WDM_ETYPE_FOLLOW	1
#define WDM_ETYPE_WARRING	2
#define WDM_ETYPE_SPECIAL	3

object worldMap;

// Gregg: единая точка учёта числа кораблей на глобалке (было 8 копий инкремента + 6 декрементов)
void Map_IncShipCounter()
{
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		pchar.worldmap.shipcounter = 0;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) + 1;
}

void Map_DecShipCounter()
{
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
}

// Gregg: generic-конструктор простого квест-энкаунтера (общее тело 5 близнецов Map_Create*)
void Map_CreateQuestEncounter(string beginLocator, string characterID, int TimeOut, string sType)
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID);
	encField.type = sType;
	encField.characterID = characterID;
	encField.beginlocator = beginlocator;
	encField.TimeOut = TimeOut;
	worldMap.addQuestEncounters = "updateQuest";
	Map_IncShipCounter();
}

//=========================================================================================
//Quest functions  - Квестовые энкаунтеры. Все эвенты приходят с параметром characterID
//=========================================================================================

//военный (догоняющий). TimeOut в днях, если -1, то неиспользуеться.
//торговец - в случае успешного достижения пункта назначения придёт эвент Map_TraderSucces
void Map_CreateTrader(string beginlocator, string endLocator, string characterID, int TimeOut)  //boal
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID); //boal fix
	encField.type = "trader";
	encField.characterID = characterID;
	encField.beginlocator = beginlocator;
	encField.endLocator = endLocator;
	encField.TimeOut = TimeOut; //boal
	worldMap.addQuestEncounters = "updateQuest";
	Map_IncShipCounter();
}

// boal 04/10/06
//военный (догоняющий). TimeOut в днях, если -1, то неиспользуеться.
//торговец - в случае успешного достижения пункта назначения придёт эвент Map_TraderSucces
void Map_CreateTraderXZ(float x1, float z1, float x2, float z2, string characterID, int TimeOut)
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID);
	if(characterID == "Head_of_Gold_Squadron" || characterID == "Head_of_Silver_Squadron") encField.type = "goldFleet";
	else encField.type = "trader";
	encField.XZGoto = true;
	encField.characterID = characterID;
	encField.x1 = x1;
	encField.z1 = z1;
	encField.x2 = x2;
	encField.z2 = z2;
	encField.TimeOut = TimeOut; //boal
	worldMap.addQuestEncounters = "updateQuest";
	Map_IncShipCounter();
}

//Lipsar--->

void Map_CreateFastWarrior(string beginLocator, string characterID, int TimeOut) { Map_CreateQuestEncounter(beginLocator, characterID, TimeOut, "coolwarrior"); }

void Map_CreateFastTradeHunter(string beginLocator, string characterID, int TimeOut) { Map_CreateQuestEncounter(beginLocator, characterID, TimeOut, "tradehunter"); }

void Map_CreateSlowMerch(string beginLocator, string characterID, int TimeOut) { Map_CreateQuestEncounter(beginLocator, characterID, TimeOut, "shipwrecked"); }

void Map_CreateHunter(string beginLocator, string characterID, int TimeOut) { Map_CreateQuestEncounter(beginLocator, characterID, TimeOut, "hunter"); }

// LEO: дружественный энкаунтер возврата компаньона - догоняет игрока, на сближении исчезает и шлёт
//   WorldMap_CompanionMet -> вступление в эскадру. Модель на глобалке - ranger (зелёный).
void Map_CreateCompanionMeet(string beginLocator, string characterID, int TimeOut) { Map_CreateQuestEncounter(beginLocator, characterID, TimeOut, "companionmeet"); }

// LEO: дружественный энкаунтер рейса ТУДА - компаньон мирно плывёт к городу-складу (цель = XZ острова
//   города). Не дерётся (isEnemy=false, наезд игрока не даёт стычку). По достижении шлёт
//   WorldMap_CompanionArrived -> работа со складом. Модель ranger.
void Map_CreateCompanionSail(string beginLocator, string characterID, string toCity, int TimeOut)
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID);
	encField.type = "companionsail";
	encField.characterID = characterID;
	encField.beginlocator = beginLocator;
	encField.sailToCity = toCity; // движок наведётся на ПОРТ города через GetQuestLocator (не центр острова)
	encField.TimeOut = TimeOut;
	worldMap.addQuestEncounters = "updateQuest";
	Map_IncShipCounter();
}

//<---Lipsar

//военный (догоняющий). TimeOut в днях, если -1, то неиспользуеться.
//При таймауте придёт эвент Map_WarriorEnd
void Map_CreateWarrior(string beginLocator, string characterID, int TimeOut) { Map_CreateQuestEncounter(beginLocator, characterID, TimeOut, "warrior"); }

//воюющий. TimeOut в днях. При таймауте придёт эвент Map_BattleEnd
//если же игрок вошёл в этот энкаунтер до истечения таймаута удалиться энкоунтер из карты
void Map_CreateBattle(string characterID, int iEnemyNation, int TimeOut)
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID);
	encField.type = "battle";
	encField.characterID = characterID;
	encField.iEnemyNation = iEnemyNation;
	encField.TimeOut = TimeOut;
	worldMap.addQuestEncounters = "updateQuest";
	Map_IncShipCounter();
}

//Проверка наличия квестового энкоунтера на глобалке
bool CheckExistQuestEncounter(string characterID)
{
	aref encs;
	makearef(encs, worldMap.encounters);
	int num = GetAttributesNum(encs);
	for(i = 0; i < num; i++)
	{
		aref enc = GetAttributeN(encs, i);
		if(CheckAttribute(enc, "quest.chrID") == 0) continue;
		if(enc.quest.chrID == characterID)
		{
			return true;
		}
	}
	return false;
}

//Удалить квестового энкоунтера
void Map_ReleaseQuestEncounter(string characterID)
{
	//Просматриваем список создаваемых
	aref encs, at;
	string aname;
	makearef(encs, worldMap.addQuestEncounters);
	int num = GetAttributesNum(encs);
	for(int cnt = 0; cnt < num; cnt++)
	{
		for(int i = 0; i < num; i++)
		{
			at = GetAttributeN(encs, i);
			aname = GetAttributeName(at);
			if (CheckAttribute (worldMap, "addQuestEncounters." + aname + ".characterID"))
			{
				aname = "addQuestEncounters." + aname + ".characterID";

				if(worldMap.(aname) == characterID)
				{
					DeleteAttribute(&worldMap, aname);
					break;
				}
			}
		}
		if(i >= num)
		{
			break;
		}

	}
	//Просматриваем список созданных
	makearef(encs, worldMap.encounters);
	num = GetAttributesNum(encs);
	for(i = 0; i < num; i++)
	{
		aref enc = GetAttributeN(encs, i);
		if(CheckAttribute(enc, "quest.chrID") == 0) continue;
		if(enc.quest.chrID == characterID)
		{
			wdmDeleteLoginEncounter(GetAttributeName(enc));
			break;
		}
	}
	worldMap.deleteUpdate = "";
}

//=========================================================================================
//Util functions
//=========================================================================================

//Storm functions

int wdmGetNumberStorms()
{
	return MakeInt(worldMap.storm.num);
}

//Ship encounter functions

int wdmGetNumberShipEncounters()
{
	return MakeInt(worldMap.encounter.num);
}

bool wdmSetCurrentShipData(int shipIndex)
{
	worldMap.encounter.cur = shipIndex;
	int i = MakeInt(worldMap.encounter.cur);
	if(i < 0 || i != shipIndex) return 0;
	return 1;
}

//Создание энкоунтеров...

// Специальный энкоунтер (сундук или кораблекрушенец)
bool wdmCreateSpecial(float kSpeed)
{
	//Получим описание энкоунтера в море
	int i1 = -1;
	int i2 = -1;
	//Создаём энкоунтера
	if(GenerateMapEncounter(WDM_ETYPE_SPECIAL, worldMap.island, &i1, &i2) == false) return false;
	string encID = "";
	bool res = wdmCreateSpecialByIndex(kSpeed, i1, &encID, "", "", 5+rand(5)); //boal new //homo 07/10/06
	//Очищаем массив энкоунтеров
	ReleaseMapEncounters();
	return res;
}

//Бочонок или кораблекрушенец
bool wdmCreateSpecialByIndex(float kSpeed, int index, ref encID, string from, string to, int timeOutInDays)  // boal
{
	//Нация энкоунтера (моделька)
	string nationShipName = wdmEncounterModelName(index);
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	// boal правки в ядре -->
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec);
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = SendMessage(&worldMap, "lsssff", MSG_WORLDMAP_CREATEENC_MER, nationShipName, from, to, kSpeed, timeOutInSec);
	// boal <--
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	//Вернём идентификатор
	encID = worldMap.EncounterID1;
	return res;
}

//Торговец
bool wdmCreateMerchantShip(float kSpeed)
{
	//Получим описание энкоунтера в море
	int i1 = -1;
	int i2 = -1;
	if(GenerateMapEncounter(WDM_ETYPE_MERCHANT, worldMap.island, &i1, &i2) == false) return false;
	//Создаём энкоунтера
	string encID = "";
	bool res = wdmCreateMerchantShipByIndex(kSpeed, i1, &encID, "", "", 5+rand(5)); //boal new //homo 07/10/06
	//Очищаем массив энкоунтеров
	ReleaseMapEncounters();
	return res;
}

//Торговец
bool wdmCreateMerchantShipByIndex(float kSpeed, int index, ref encID, string from, string to, int timeOutInDays)  // boal
{
	//Нация энкоунтера (моделька)
	string nationShipName = wdmEncounterModelName(index);
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	// boal правки в ядре -->
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec);
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = SendMessage(&worldMap, "lsssff", MSG_WORLDMAP_CREATEENC_MER, nationShipName, from, to, kSpeed, timeOutInSec);
	// boal <--
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	//Вернём идентификатор
	encID = worldMap.EncounterID1;
	return res;
}
// boal 04/10/06
bool wdmCreateMerchantShipXZByIndex(float kSpeed, int index, ref encID, float x1, float z1, float x2, float z2, int timeOutInDays)
{
	//Нация энкоунтера (моделька)
	string nationShipName = wdmEncounterModelName(index);
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec);
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = SendMessage(&worldMap, "lsffffff", MSG_WORLDMAP_CREATEENC_MER_XZ, nationShipName, x1, z1, x2, z2, kSpeed, timeOutInSec);
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	//Вернём идентификатор
	encID = worldMap.EncounterID1;
	return res;
}

//Преследователь
bool wdmCreateFollowShip(float kSpeed)
{
	//Получим описание энкоунтера в море
	int i1 = -1;
	int i2 = -1;
	if(GenerateMapEncounter(WDM_ETYPE_FOLLOW, worldMap.island, &i1, &i2) == false) return false;
	//Создаём энкоунтера
	string encID = "";
	bool res = wdmCreateFollowShipByIndex(kSpeed, i1, &encID, 2+rand(3)); //homo 07/10/06
	//Очищаем массив энкоунтеров
	ReleaseMapEncounters();
	return res;
}

//Преследователь
bool wdmCreateFollowShipByIndex(float kSpeed, int index, ref encID, int timeOutInDays)
{
	//Нация энкоунтера
	string nationShipName = wdmEncounterModelName(index);
	//проверим нацию
	ref n = GetMapEncounterRef(index);
	int iNation = PIRATE;
	if(CheckAttribute(n, "nation"))
	{
		iNation = sti(n.Nation);
	}
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	//Время жизни в секундах
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec); // Boal супер бага была - делили на 24. Математику в школе прогуливали?
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = false;
	bool sdps = false;

	aref arShips;
	makearef(arShips, mapEncSlotRef.shiptypes);
	int enemyclasspower = CalculateSquadronPowerOther(arShips);
	int playerclasspower = CalculateSquadronPowerPchar();

	if (CheckAttribute(pchar,"SeaDogProfessionalSwitch") && pchar.SeaDogProfessionalSwitch == 1) sdps = true;
	bool klas = (sdps) && (GetCharacterShipClass(pchar) < 5);
	if (klas || (GetNationRelation2MainCharacter(iNation) != RELATION_ENEMY) || (playerclasspower > enemyclasspower) || (rand(100) < (GetCharacterSPECIAL(pchar, "LUCK") * 3)))
	{
		res = SendMessage(&worldMap, "lsssff", MSG_WORLDMAP_CREATEENC_MER, nationShipName, "", "", kSpeed, timeOutInSec); // boal new
	}
	else
	{   // boal - код этот полный абзац, я Вам, господа акеловцы, аплодирую - метод Следовать даёт убегание - и так весь код. плакаль
		// погано, что метод имеет право быть, но название должно быть иным, нужно чтоб мирные не доставали патрулями.
		res = SendMessage(&worldMap, "lsff", MSG_WORLDMAP_CREATEENC_FLW, nationShipName, kSpeed, timeOutInSec);
	}
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	encID = worldMap.EncounterID1;
	return res;
}

int ClassPowerCalculate(int class)//сила относительно класса каждого конкретного корабля
{
	switch (class)
	{
		case 7: return 1; break;
		case 6: return 4; break;
		case 5: return 7; break;
		case 4: return 13; break;
		case 3: return 25; break;
		case 2: return 50; break;
		case 1: return 100; break;
	}
	return 0;
}

int CalculateSquadronPowerPchar()
{
	int playerclasspower = 0;
	for (int j=0; j<COMPANION_MAX; j++) // ГГ+
	{
		int cn = GetCompanionIndex(pchar, j);
		if (cn>0)
		{
			ref officer = GetCharacter(cn);
			playerclasspower += ClassPowerCalculate(GetCharacterShipClass(officer));
		}
	}
	return playerclasspower;
}

int CalculateSquadronPowerOther(aref arShips)
{
	int iNumShips = GetAttributesNum(arShips);
	int enemyclasspower = 0;
	int iShipType = 0;
	for(int iS=0; iS<iNumShips; iS++)//энка
	{
		iShipType = GetAttributeValue(GetAttributeN(arShips, iS));
		enemyclasspower += ClassPowerCalculate(sti(shipstypes[iShipType].Class));
	}
	return enemyclasspower;
}

//Преследователь НАСТОЯЩИЙ
bool wdmCreateRealFollowShipByIndex(float kSpeed, int index, ref encID, int timeOutInDays)
{
	//Нация энкоунтера
	string nationShipName = wdmEncounterModelName(index);
	//проверим нацию
	ref n = GetMapEncounterRef(index);
	int iNation = PIRATE;
	if(CheckAttribute(n, "nation"))
	{
		iNation = sti(n.Nation);
	}
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	//Время жизни в секундах
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec); // Boal супер бага была - делили на 24. Математику в школе прогуливали?
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = false;
	// boal: мирная ветка (CREATEENC_MER для не-врагов) выпилена - метод «Следовать» давал убегание,
	//       поэтому энкаунтер всегда создаётся через CREATEENC_FLW.
	res = SendMessage(&worldMap, "lsff", MSG_WORLDMAP_CREATEENC_FLW, nationShipName, kSpeed, timeOutInSec);
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	encID = worldMap.EncounterID1;
	return res;
}

// LEO: дружественный follow компаньона на глобалке (модель ranger, зелёный). Как real-follow, но шлёт
//   MSG_WORLDMAP_CREATEENC_COMPANION -> движок создаёт WdmCompanionShip (isEnemy=false, при контакте не бой).
//   Имя компаньона кладём на saveAttribute энкаунтера ("companionChrID") - движок вернёт его в событии встречи.
bool wdmCreateCompanionShipByIndex(float kSpeed, int index, ref encID, int timeOutInDays, string chrID, string sFromPort)
{
	string modelName = "ranger"; // глобалочная модель энкаунтера-компаньона
	ref mapEncSlotRef = GetMapEncounterRef(index);
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec);
	float timeOutInSec = daysPerSec*timeOutInDays;
	// sFromPort - порт отправления (движок стартует ОТ него через GetQuestLocator, как merchant/ПГГ)
	bool res = SendMessage(&worldMap, "lssff", MSG_WORLDMAP_CREATEENC_COMPANION, modelName, sFromPort, kSpeed, timeOutInSec);
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	encID = worldMap.EncounterID1;
	worldMap.encounters.(encID).companionChrID = chrID; // движок читает с saveAttribute при контакте
	return res;
}

//Воюющие корабли
bool wdmCreateWarringShips()
{
	//Получим описание энкоунтера в море
	int i1 = -1;
	int i2 = -1;
	if(GenerateMapEncounter(WDM_ETYPE_WARRING, worldMap.island, &i1, &i2) == false)
	{
		ReleaseMapEncounters();
		return false;
	}
	//Создаём энкоунтера
	string encID1 = "";
	string encID2 = "";
	bool res = wdmCreateWarringShipsByIndex(i1, i2, &encID1, &encID2, 5+rand(5));  //homo 07/10/06
	//Очищаем массив энкоунтеров
	ReleaseMapEncounters();
	return res;
}

// LEO: % шанс добавить 3-ю (пиратскую) сторону к войне нация-vs-нация. Частота самих войн НЕ меняется.
#define WDM_THREESIDE_CHANCE 40

//Воюющие корабли
bool wdmCreateWarringShipsByIndex(int index1, int index2, ref encID1, ref encID2, int timeOutInDays)
{
	//Нации энкоунтера
	string nationShipName1 = wdmEncounterModelName(index1);
	string nationShipName2 = wdmEncounterModelName(index2);
	//Создадим ссылки на атрибуты
	ref mapEncSlotRef1 = GetMapEncounterRef(index1);
	ref mapEncSlotRef2 = GetMapEncounterRef(index2);
	//Время жизни в секундах
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec); // Boal супер бага была - делили на 24. Математику в школе прогуливали?
	float timeOutInSec = daysPerSec*timeOutInDays;

	// LEO: 3-СТОРОННЯЯ СТЫЧКА — с шансом добавить ПИРАТСКИЙ 3-й борт. Только если ОБЕ базовые стороны — нации
	//   (не пираты), иначе две пиратские стороны слились бы в одну (дружат). Слот форсим в пираты (нация/тип/
	//   модель/корабли). Движок спавнит 3-й борт вершиной треугольника (WAR3); вражду даёт матрица наций
	//   (пират ENEMY всем), 3-сторонний замес разрулит ИИ (целит ближайшего врага).
	int index3 = -1;
	int iPirEnc = 0;  // LEO: тип пиратского борта и ранг ГГ - объявление В ШАПКЕ:
	int iPirRank = 0; //      Storm не принимает объявление внутри вложенного блока
	string pirateShipName = "";
	ref mapEncSlotRef3;
	if (sti(mapEncSlotRef1.Nation) != PIRATE && sti(mapEncSlotRef2.Nation) != PIRATE && rand(99) < WDM_THREESIDE_CHANCE)
	{
		if (GenerateMapEncounter_War(worldMap.island, &index3, -1))
		{
			mapEncSlotRef3 = GetMapEncounterRef(index3);
			// LEO: КРИТ — GenerateMapEncounter_War(-1) НЕ ставит GroupName -> в бою группа грузится как "error"
			//   с неинициализированными кораблями (нет Ship.Name/Cannons) -> краш sea.c. Ставим как у нацсторон:
			//   GroupName = ENCOUNTER_GROUP("egroup__") + slot; RndFormation как у 2-сторонней войны (форма строя).
			mapEncSlotRef3.GroupName = "egroup__" + index3;
			mapEncSlotRef3.RndFormation = rand(5);
			mapEncSlotRef3.Nation = PIRATE;
			// LEO: тип пиратского борта ПО РАНГУ ГГ, штатными порогами MinRank из Encounters_init.c
			//   (SMALL с 1, MEDIUM с 3, LARGE с 5, SQUADRON с 7). Раньше тут был захардкожен SQUADRON:
			//   у него MinRank 7 и классы 2-4 НА ЛЮБОМ ранге, поэтому новичок получал пятёрку линкоров
			//   рядом с торговцами 5-6 класса.
			iPirEnc = ENCOUNTER_TYPE_PIRATE_SMALL;
			iPirRank = sti(pchar.rank);
			if (iPirRank >= 3) iPirEnc = ENCOUNTER_TYPE_PIRATE_MEDIUM;
			if (iPirRank >= 5) iPirEnc = ENCOUNTER_TYPE_PIRATE_LARGE;
			if (iPirRank >= 7) iPirEnc = ENCOUNTER_TYPE_PIRATE_SQUADRON;
			mapEncSlotRef3.RealEncounterType = iPirEnc;
			mapEncSlotRef3.Type = "pirate";
			GenerateMapEncounter_WriteNumShips(mapEncSlotRef3, iPirEnc, 8);
			GenerateMapEncounter_SetMapShipModel(mapEncSlotRef3);
			pirateShipName = wdmEncounterModelName(index3);
		}
	}

	bool res;
	if (index3 >= 0 && pirateShipName != "")
	{
		// 3-сторонняя: WAR3 с тремя моделями (model1, model2, model3=пираты)
		res = SendMessage(&worldMap, "lsssfff", MSG_WORLDMAP_CREATEENC_WAR3, nationShipName2, nationShipName1, pirateShipName, timeOutInSec, 0.0, 0.0);
		WdmCopyEncounterData(mapEncSlotRef1, worldMap.EncounterID1);
		WdmCopyEncounterData(mapEncSlotRef2, worldMap.EncounterID2);
		WdmCopyEncounterData(mapEncSlotRef3, worldMap.EncounterID3);
	}
	else
	{
		// обычная 2-сторонняя (+fX,fZ 0,0 = рандомная позиция как раньше)
		res = SendMessage(&worldMap, "lssfff", MSG_WORLDMAP_CREATEENC_WAR, nationShipName2, nationShipName1, timeOutInSec, 0.0, 0.0);
		WdmCopyEncounterData(mapEncSlotRef1, worldMap.EncounterID1);
		WdmCopyEncounterData(mapEncSlotRef2, worldMap.EncounterID2);
	}
	// LEO: EDIT 1 суффиксировал имена групп encID'ом -> перекрёстная ссылка сторон войны протухла (Task.Target = старое
	//   "egroup__<slot>", а группа противника теперь суффиксирована). Восстанавливаем: Task.Target каждой стороны = РЕАЛЬНОЕ
	//   (уже суффиксированное) имя группы ПРОТИВНИКА — читаем, не гадаем суффикс -> стороны целят друг друга с t=0, а не
	//   перецепляются через нацматрицу. Сторона1<->сторона2 (EncounterID1/2); пиратский 3-й борт (EncounterID3) Task.Target не имеет.
	string p1t; p1t = "encounters." + worldMap.EncounterID1 + ".encdata";
	string p2t; p2t = "encounters." + worldMap.EncounterID2 + ".encdata";
	if (CheckAttribute(&worldMap, p1t + ".Task.Target"))
	{
		if (CheckAttribute(&worldMap, p2t + ".GroupName")) worldMap.(p1t).Task.Target = worldMap.(p2t).GroupName;
	}
	if (CheckAttribute(&worldMap, p2t + ".Task.Target"))
	{
		if (CheckAttribute(&worldMap, p1t + ".GroupName")) worldMap.(p2t).Task.Target = worldMap.(p1t).GroupName;
	}
	encID1 = worldMap.EncounterID1;
	encID2 = worldMap.EncounterID2;
	return res;
}

// LEO: ПАСХАЛКА "БИТВА ПЯТИ ВОИНСТВ" - стычка на пять сторон одной кучей на глобалке.
//   Слоты энкаунтеров заполняем вручную (штатный GenerateMapEncounter_War дал бы случайные корабли,
//   а тут нужны именно тартаны и люггер Сарумана), после чего шлём движку одно сообщение WARN -
//   он расставит все пять бортов вокруг общей точки и свяжет их кольцом attack. При входе игрока
//   ExitFromMap по этому кольцу утащит в море всю пятёрку.
//   Порядок слотов EncounterID у движка исторический (борт0 -> ID2, борт1 -> ID1, дальше по номеру),
//   поэтому копируем данные по таблице соответствия, а не по индексу.
#define FIVEARMIES_SIDES		5	// сколько воинств
#define FIVEARMIES_SHIPS		3	// кораблей в каждом
#define FIVEARMIES_TIMEOUT		10	// сколько дней стычка держится на карте, потом гаснет навсегда

bool wdmCreateFiveArmiesBattle()
{
	int    i, j, iSlot, iShipType;
	int    aSlots[FIVEARMIES_SIDES];
	string aModels[FIVEARMIES_SIDES];
	string aEncIDs[FIVEARMIES_SIDES];
	string sAttr, sEncPath, sNextPath;
	ref    rEncounter;
	float  fTimeOutSec = FIVEARMIES_TIMEOUT * 24.0 / stf(worldMap.date.hourPerSec);

	for (i = 0; i < FIVEARMIES_SIDES; i++)
	{
		iSlot = FindFreeMapEncounterSlot();
		if (iSlot < 0)
		{
			ReleaseMapEncounters();
			return false;
		}
		ManualReleaseMapEncounter(iSlot);
		aSlots[i] = iSlot;

		rEncounter = &MapEncounters[iSlot];
		rEncounter.Nation = FiveArmies_SideNation(i);
		rEncounter.RealEncounterType = ENCOUNTER_TYPE_PATROL_SMALL;
		rEncounter.Type = "war";
		if (sti(rEncounter.Nation) == PIRATE)
		{
			rEncounter.RealEncounterType = ENCOUNTER_TYPE_PIRATE_SMALL;
			rEncounter.Type = "pirate";
		}
		rEncounter.GroupName = ENCOUNTER_GROUP + iSlot; // WdmCopyEncounterData допишет уникальный encID
		rEncounter.RndFormation = rand(5);
		rEncounter.NumWarShips = FIVEARMIES_SHIPS;
		rEncounter.FiveArmies = i; // метка воинства: по ней инфо-окно подписывает сторону (map.c)
		rEncounter.bUse = true;

		// корабли задаём сами: тартаны, и только под Саруманом люггер
		for (j = 0; j < FIVEARMIES_SHIPS; j++)
		{
			sAttr = j;
			iShipType = SHIP_TARTANE;
			if (i == FIVEARMIES_SIDES - 1 && j == 0) iShipType = SHIP_LUGGER;
			rEncounter.shiptypes.(sAttr) = iShipType;
			rEncounter.shipmodes.(sAttr) = "war";
		}
		rEncounter.v2 = true; // корабли уже расписаны - не перегенерировать по классам

		GenerateMapEncounter_SetMapShipModel(rEncounter);
		aModels[i] = wdmEncounterModelName(iSlot);
	}

	// слоты EncounterID общие на весь модуль - чистим, чтобы не принять за свои хвосты прошлой стычки
	for (i = 1; i <= FIVEARMIES_SIDES; i++)
	{
		DeleteAttribute(&worldMap, "EncounterID" + i);
	}

	if (!SendMessage(&worldMap, "lsssssfff", MSG_WORLDMAP_CREATEENC_WARN, aModels[0], aModels[1], aModels[2], aModels[3], aModels[4], fTimeOutSec, 0.0, 0.0))
	{
		ReleaseMapEncounters();
		return false;
	}

	// борт0 движок кладёт в EncounterID2, борт1 - в EncounterID1, остальные по своему номеру.
	// Слот пуст, если модель борта не загрузилась - такую сторону просто пропускаем.
	aEncIDs[0] = ""; if (CheckAttribute(&worldMap, "EncounterID2")) aEncIDs[0] = worldMap.EncounterID2;
	aEncIDs[1] = ""; if (CheckAttribute(&worldMap, "EncounterID1")) aEncIDs[1] = worldMap.EncounterID1;
	aEncIDs[2] = ""; if (CheckAttribute(&worldMap, "EncounterID3")) aEncIDs[2] = worldMap.EncounterID3;
	aEncIDs[3] = ""; if (CheckAttribute(&worldMap, "EncounterID4")) aEncIDs[3] = worldMap.EncounterID4;
	aEncIDs[4] = ""; if (CheckAttribute(&worldMap, "EncounterID5")) aEncIDs[4] = worldMap.EncounterID5;

	for (i = 0; i < FIVEARMIES_SIDES; i++)
	{
		if (aEncIDs[i] == "") continue;
		WdmCopyEncounterData(GetMapEncounterRef(aSlots[i]), aEncIDs[i]);
	}
	ReleaseMapEncounters();

	// каждое воинство целит следующее по кругу; имя группы читаем УЖЕ суффиксированным
	for (i = 0; i < FIVEARMIES_SIDES; i++)
	{
		j = i + 1;
		if (j >= FIVEARMIES_SIDES) j = 0;
		if (aEncIDs[i] == "") continue;
		if (aEncIDs[j] == "") continue;
		sEncPath  = "encounters." + aEncIDs[i] + ".encdata";
		sNextPath = "encounters." + aEncIDs[j] + ".encdata";
		if (!CheckAttribute(&worldMap, sNextPath + ".GroupName")) continue;

		worldMap.(sEncPath).Task = AITASK_ATTACK;
		worldMap.(sEncPath).Task.Target = worldMap.(sNextPath).GroupName;
		// имя группы понадобится в море: по нему пасхалка находит свои корабли (см. FiveArmies_SeaCheck)
		sAttr = i;
		pchar.questTemp.FiveArmies.grp.(sAttr) = worldMap.(sEncPath).GroupName;
	}
	return true;
}

//Шторм
void wdmCreateStorm()
{
    if(CheckAttribute(pchar,"worldmapencountersoff") == 1)
	{  // boal убиралка шторма
		if(sti(pchar.worldmapencountersoff)) return;
	}
	int isTornado = 0;
	if(worldMap.island == WDM_NONE_ISLAND)
	{
		int r = rand(1000);
		if(r != 0)
		{
			if(r < 450)
			{
				isTornado = 1;
			}
		}
	}
	// тест торнадор
	isTornado = 1;
	// конец теста
	SendMessage(&worldMap, "ll", MSG_WORLDMAP_CREATESTORM, isTornado);
}

//Получить модельку кораблика по индексу нации
string wdmEncounterModelName(int encIndex)
{
	ref n = GetMapEncounterRef(encIndex);
	if(CheckAttribute(n, "worldMapShip") != 0)
	{
		return n.worldMapShip;
	}
	return "ship";
}

//Скопировать данные об энкоутере
void WdmCopyEncounterData(ref mapEncSlotRef, string encStringID)
{
	string sEncID = encStringID; // LEO: сырой encID (глобально уникален, save-safe) — суффикс имени морской группы (см. ниже)
	encStringID = "encounters." + encStringID + ".encdata";
	//johnny88
	// Не даём перезаписывать encdata, если этот encID ещё занят.
	// Иначе на карте может остаться старый визуальный объект,
	// а окно встречи начнёт читать уже новые данные.
	if(CheckAttribute(&worldMap, encStringID))
	{
		return;
	}

	worldMap.(encStringID) = "";
	aref destRef;
	makearef(destRef, worldMap.(encStringID));
	CopyAttributes(destRef, mapEncSlotRef);
	// LEO: имя морской группы non-ALONE энка = "egroup__"+слот_пула, а пул чистится/переиспользуется между генерациями ->
	//   независимые энки почти всегда несут ОДИНАКОВЫЙ "egroup__0" и в море СХЛОПЫВАЮТСЯ в ОДНУ группу с ОДНИМ флагманом,
	//   теряя вражду (nations.c: общий SeaAI.Group.Name => RELATION_FRIEND). Дописываем УНИКАЛЬНЫЙ encID -> раздельные
	//   группы + свои флагманы + сохранённая вражда наций/пиратов.
	// ГОЧА (обжёгся): суффикс ТОЛЬКО пуловым. У ALONE/квестовых GenerateMapEncounter_Alone кладёт в
	//   GroupName чистый id персонажа - он уникален сам, а испорченное имя рвёт связку "Sea_"+id
	//   командира, по которой группа грузится в бой.
	if(CheckAttribute(&worldMap, encStringID + ".GroupName"))
	{
		string sGrpName = worldMap.(encStringID).GroupName;
		// литерал, а не ENCOUNTER_GROUP: тот в .c (Encounters_map.c:1), видимость зависит от порядка сборки
		if (findsubstr(sGrpName, "egroup__", 0) == 0)
		{
			worldMap.(encStringID).GroupName = sGrpName + "_" + sEncID;
		}
	}
}

//Зарезервировать место для добавления нового энкоунтера
aref wdmCreateNewQuestEncDescription()
{
	string gname, aname;
	aref encs, at;
	makearef(encs, worldMap.addQuestEncounters);
	int num = GetAttributesNum(encs);
	for(int cnt = 0; cnt < num + 10; cnt++)
	{
		gname = "e" + cnt;
		for(int i = 0; i < num; i++)
		{
			at = GetAttributeN(encs, i);
			aname = GetAttributeName(at);
			if(aname == gname)
			{
				break;
			}
		}
		if(i >= num)
		{
			break;
		}
	}
	aref retVal;
	makearef(retVal, worldMap.addQuestEncounters.(gname));
	return retVal;
}

//// boal & homosapienz
void  wdmEmptyAllDeadQuestEncounter()
{
    aref encs, enc, at;
    string sdel, aname;
    int  i, num, iChar;
    bool bKill;
    bool isWMap = IsEntity(worldMap);

    makearef(encs, worldMap.encounters);

    num = GetAttributesNum(encs);
    for (i = 0; i < num; i++)
    {
        enc = GetAttributeN(encs, i);
        if (CheckAttribute(enc, "quest.chrID"))
        {
            iChar = GetCharacterIndex(enc.quest.chrID);
            // LEO: по вложенным if - в Storm нет короткого замыкания, и прежнее условие через ||
            //      читало characters[-1] даже когда командира уже нет («Wrong Index is -1» в логе).
            bKill = false;
            if (iChar == -1) bKill = true;
            if (!bKill)
            {
                if (CharacterIsDead(&characters[iChar])) bKill = true;
            }
            if (!bKill)
            {
                if (CheckAttribute(CharacterFromID(enc.quest.chrID), "PGGAi.RemoveEncounter")) bKill = true;
            }
            if (bKill)
            {
                if (!isWMap)
                {
                    sdel = "encounters." + GetAttributeName(enc);
                    DeleteAttribute(&worldMap, sdel);
                    num--;
                    i--; // LEO: удаление сдвигает остальные - без отката индекса следующий энкаунтер ПРОПУСКАЛСЯ
                    Map_DecShipCounter();
                }
                else
                {
                    enc.livetime = 0;
                }
            }
        }
    }

    makearef(encs, worldMap.addQuestEncounters);
    num = GetAttributesNum(encs);
    for (i = 0; i < num; i++)
    {
        at = GetAttributeN(encs, i);
        sdel = GetAttributeName(at);
        aname = "addQuestEncounters." + sdel + ".characterID";
        if (CheckAttribute(&worldMap, aname))
        {
            iChar = GetCharacterIndex(worldMap.(aname));
            bKill = false;
            if (iChar == -1) bKill = true;
            if (!bKill)
            {
                if (CharacterIsDead(&characters[iChar])) bKill = true;
            }
            if (bKill)
            {
                DeleteAttribute(&worldMap, "addQuestEncounters." + sdel);
                num--;
                i--; // LEO: то же самое - иначе соседний энкаунтер оставался висеть на карте
                Map_DecShipCounter();
            }
        }
    }
}

// чистка энкоутеров на карте homosapienz
void  wdmEmptyAllOldEncounter()
{
    aref encs;
    string sdel,aname;
	bool isWMap = IsEntity(worldMap);

    makearef(encs, worldMap.encounters);

    int num = GetAttributesNum(encs);
    aref enc;
    int  i;

    for (i = 0; i < num; i++)
    {
        enc = GetAttributeN(encs, i);
        if (CheckAttribute(enc, "needDelete") && enc.needDelete == "wdmEncounterDelete")
        {
	        if (CheckAttribute(enc, "quest")) Map_DecShipCounter();
	        sdel  = "encounters."+GetAttributeName(enc);
			DeleteAttribute(&worldMap, sdel);
	        num--; //fix
	        i--;

        }
    }
}
// время жизни на карте для квестовых энкоутеров
// LEO: сначала помечаем истёкшие встречи, затем перечитываем список после каждого обработчика:
// обработчик прибытия может удалить или пересоздать не только текущую встречу.
void  wdmUpdateAllEncounterLivetime()
{
    aref encs, enc;
    int ihours, num, i;
    string sdel, sEncChr;
    bool bFound;

    if (actLoadFlag) return;
    ihours = GetQuestPastTimeParam("WordMapEncounters_DailyUpdate");
    if (ihours <= 0) return;

    Log_TestInfo(GetConvertStrWithReplace("Variable_worldmap_globals_1", "Logs.txt", "#space#", " ")+makeint(ihours/24.0));
    float timeOutInSec = ihours/stf(worldMap.date.hourPerSec);
    makearef(encs, worldMap.encounters);

    // Проход 1: время жизни и пометка протухших. Список энкаунтеров здесь НЕ меняется.
    num = GetAttributesNum(encs);
    for (i = 0; i < num; i++)
    {
        enc = GetAttributeN(encs, i);          // только квестовые
        if (!CheckAttribute(enc, "livetime") || !CheckAttribute(enc, "quest")) continue;

        Log_TestInfo(enc.livetime+" - "+timeOutInSec+" : "+stf(stf(enc.livetime) - timeOutInSec));
        enc.livetime = stf(stf(enc.livetime) - timeOutInSec);
        // LEO: снимаем ТОЛЬКО событие торговца, ради него это и писалось. Было - у ЛЮБОГО
        // квестового: погоня теряла quest.event, а wdmEncounterDelete берёт владельца лишь
        // при живом event -> энкаунтер удалялся МОЛЧА, без Map_WarriorEnd. Многоразовые
        // (ОЗГ/ПГГ) переживали пересозданием, одноразовая исчезала бесследно.
        if (CheckAttribute(enc, "quest.event"))
        {
            if (enc.quest.event == "Map_TraderSucces") DeleteAttribute(enc, "quest.event");
        }
        if (sti(enc.livetime) <= 0) enc.livetimeExpired = true;
    }

    // Проход 2: разбор протухших по одному, каждый раз заново ищем первый помеченный.
    // Пометка снимается до разбора, поэтому цикл гарантированно заканчивается.
    while (true)
    {
        bFound = false;
        num = GetAttributesNum(encs);
        for (i = 0; i < num; i++)
        {
            enc = GetAttributeN(encs, i);
            if (CheckAttribute(enc, "livetimeExpired"))
            {
                bFound = true;
                break;
            }
        }
        if (!bFound) break;

        DeleteAttribute(enc, "livetimeExpired");
        sEncChr = "";
        if (CheckAttribute(enc, "quest.chrID")) sEncChr = enc.quest.chrID;
        if (sEncChr == "MaryCelesteCapitan")
        {
            // пасхалка перевзводится своим кодом, её энкаунтер тут не снимаем - фикс вылета
            Map_TraderSucces_quest(sEncChr);
            Map_DecShipCounter();
        }
        else
        {
            // LEO: отрезок дошёл до конца, пока игрок был вне карты. Шлём то же событие, что и при
            //   штатном прибытии (worldmap_encgen.c) - счётчик кораблей оно уменьшает само.
            if (sEncChr != "") PostEvent("Map_TraderSucces", 100, "s", sEncChr);
            else Map_DecShipCounter(); // адресата нет - просто снимаем протухший энкаунтер
            sdel = "encounters."+GetAttributeName(enc);
            DeleteAttribute(&worldMap, sdel);
        }
    }
}

// найти сущ случайку для НПС

aref  wdmFindOrCreateQuestEncounter(string _chrId)
{
    aref encs;
    string sdel,aname;
	bool isWMap = IsEntity(worldMap);

    makearef(encs, worldMap.encounters);

    int num = GetAttributesNum(encs);
    for(int i = 0; i < num; i++)
    {
        aref enc = GetAttributeN(encs, i);
        if (CheckAttribute(enc, "quest.chrID"))
        {
            int iChar = GetCharacterIndex(enc.quest.chrID)
            sdel  = "encounters."+GetAttributeName(enc);
            if (iChar == -1 || characters[iChar].id == _chrId)
            {
	            if (!isWMap)
				{
					DeleteAttribute(&worldMap, sdel);
	                num = GetAttributesNum(encs);
	                Map_DecShipCounter();
	            }
	            else
	            {
	            	 enc.livetime = 0;
	            }
	        }
        }
    }

   	makearef(encs, worldMap.addQuestEncounters);
    num = GetAttributesNum(encs);
	for(i = 0; i < num; i++)
	{
		aref at = GetAttributeN(encs, i);
		sdel = GetAttributeName(at);
		aname = "addQuestEncounters." + sdel + ".characterID";
		if (CheckAttribute(&worldMap, aname))
		{
	        iChar = GetCharacterIndex(worldMap.(aname));
	        if (iChar == -1 || characters[iChar].id == _chrId)
	        {
	            DeleteAttribute(&worldMap, "addQuestEncounters." + sdel);
	            num = GetAttributesNum(encs);
	            Map_DecShipCounter();
	        }
        }
    }

	return wdmCreateNewQuestEncDescription();
}
