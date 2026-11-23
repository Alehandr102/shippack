#include "Addons\Korsar_Maxim_func.c"		//Функции Korsar-а Maxim-а
#include "Addons\LORD_func.c"	        	//Функции LORD-а
#include "Addons\Academy.c"		// Академия и тренировки
#include "Addons\Arena.c"		// Поселение фехтовальщиков и множество фич с ними
#include "Addons\Colony.c"		// Постройка и жизнь колонии
#include "Addons\ColonyGuarding.c"	// Организация охраны колонии в море
#include "Addons\Plantation.c"		// Постройка и жизнь плантации
#include "Addons\Mines.c"		// Постройка и жизнь рудников
#include "Addons\Quests.c"		// Квесты
#include "Addons\GenerateQuests.c"	// Генерируемые квесты
int sixth[12] = {3,4,5,6,7,8,9,10,11,12,13,14};
int fifth[13] = {15,16,17,18,19,20,21,22,23,24,25,26,27};
int fourth[24] = {28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
int third[32] = {52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83};
int second[21] = {84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104};
int first[20] = {105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124};

void SetDamnedDestinyVariable()
{
	PChar.SelectMainCharacter = false;

	GenerateColonyVariable();
	ReloadProgressUpdate();
	InitPlantation();
	ReloadProgressUpdate();
	SetStartColonyGuarding();
	ReloadProgressUpdate();
	//----------------------------------------------------
	PChar.Dozor = "0";
	PChar.Dozor.Riddle.CurType = 1;
	PChar.Dozor.Riddle.CanInterface = 0;
	DozorPrepare();
	//----------------------------------------------------
	// Gregg: «Исчезновение людей» (перенос из К:ПС) - взводится сразу, ждёт 10 ранга
	DisappearPeoplePrepare();
	//----------------------------------------------------
	// Gregg: «Корабль обезьян» (перенос из К:ПС) - Аллен стоит у церкви Бас-Тера
	// с начала игры, но заговорит о своей беде только с 13 ранга
	CaptainMonkeyPrepare();
	//----------------------------------------------------
	PChar.MysteriousIsland = "0";
	PChar.MysteriousIsland.FirstNoneMoney = false;
	PChar.MysteriousIsland.TerksHide = false;
	PChar.MysteriousIsland.Action = false;
	PChar.MysteriousIsland.DisableBoxOpen = false;
	PChar.MysteriousIsland.BlackOrchidAction = false;
	//----------------------------------------------------
	PChar.GenerateShipWreck.Block = false;
	PChar.GenerateShipWreck.ShipInMap = "";
	PChar.GenerateShipWreck.State = "none";
	PChar.GenerateShipWreck.MaxGoods = 500;
	PChar.GenerateShipWreck.MaxGoods.Planks = 0;
	PChar.GenerateShipWreck.MaxGoods.SailCloth = 0;
	PChar.GenerateShipWreck.GoodsChange = false;
	PChar.GenerateShipWreck.GoodsChange.Yes = false;
	PChar.GenerateShipWreck.AddBandaShip = false;
	PChar.GenerateShipWreck.ValodyaToMoney = false;

	PChar.GenerateQuestDuel.Block = false;

	PChar.TmpModel = "Hero1";
	PChar.ToMusketeer.Model = "hero1";
	PChar.ToMusketeer.Ani = "man";

	PChar.SuccessPrice.ReadNotice = false;
	PChar.SuccessPrice.CreateCharacter = false;
	PChar.SuccessPrice.CreateCharacter.Model = "Hero1";

	GenerateArena();
	AcademyLandInit();

	ReloadProgressUpdate();
}
/////////////////////////////////////////////////////////////////////////////////
// Начало игры и выбор главного героя
/////////////////////////////////////////////////////////////////////////////////
void SelectMainCharacterInCabine(string sQuest)
{
	ref PChar = GetMainCharacter();

	bDisableCharacterMenu = true;
	bDisableQuestInterface = true;

	Locations[FindLocation(PChar.location)].items.randitem1 = "";

	ChangeCharacterAddressGroup(PChar, PChar.location, "goto", "SelectCharacter");

	PChar.model = "";
	SetNewModelToChar(PChar);

    SetCharacterTask_None(GetMainCharacter());
	LAi_SetActorTypeNoGroup(PChar);
	LAi_ActorTurnToLocator(PChar, "reload", "reload1");

	DoQuestFunctionDelay("LaunchCreateCharacterInterface", 1.0);
}

void LaunchCreateCharacterInterface(string _tmp)
{
	LaunchCreateCharacter();
}

void EndSelectMainCharacter(ref chr)
{
	bDisableCharacterMenu = false;
	bDisableQuestInterface = false;
	SetTimerFunction("StartGameOpenArena", 0, 0, 30);
	pchar.starttype = startHeroType;
	LaunchSelectCharacter();
}


void NoFreeze()
{
	SetLaunchFrameFormParam(GetConvertStr("Variable_utils_1", "Frames.txt"), "", 0.1, 2.0);
	LaunchFrameForm();
}

void StartGameOpenArena(string qName)
{
	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.label = "Arena";
	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = 1;
	Locations[FindLocation("FencingTown_ExitTown")].reload.l4.label = "FencingTown";
	Locations[FindLocation("FencingTown_ExitTown")].reload.l4.disable = 0;

	Locations[FindLocation("FencingTown_Arena")].id.label = "Arena";
	Locations[FindLocation("FencingTown_Fort")].id.label = "FencingTown";
	AddSimpleRumourToAllNations(GetConvertStrWithReplace("Variable_utils_1", "AddRumours.txt", "#space#", " "), 30, 1);
}

void BuildMalteseOrder(string qName)
{
	Locations[FindLocation("DeckWithReefs")].reload.l2.label = "Reefs_Chapter";
	Locations[FindLocation("DeckWithReefs")].reload.l2.disable = 0;
	Locations[FindLocation("Reefs_Chapter")].id.label = "Reefs_Chapter";

	ref ch = GetCharacter(NPC_GenerateCharacter("MalteseOrderCharacter", "priest_sp"+(rand(2)+1), "man", "man", 30, PIRATE, 1, true));
	ch.greeting = "MalteseOrderMan";
	ch.name 	= GetConvertStr("Variable_utils_0", "Names.txt");
    ch.lastname = GetConvertStr("Variable_utils_1", "Names.txt");
	ch.City = "Reefs";
	ch.location	= "Reefs_Chapter";
	ch.location.group = "sit";
	ch.location.locator = "sit1";
	ch.Dialog.Filename = "MalteseOrder.c";
	LAi_SetHuberType(ch);
	LAi_group_MoveCharacter(ch, "PIRATE_CITIZENS");
	LAi_SetImmortal(ch, true);
	DeleteAttribute(ch,"lifeDay");

	Log_info(GetConvertStrWithReplace("Variable_utils_2", "Logs.txt", "#space#", " "));
}

void RefreshSlavesLimit(string qName)
{
	ref sld = CharacterFromID("PlantRuler");
	sld.SlavesLimit = 250+rand(750);
}

void RefreshGWIK(string qName)
{
	DeleteAttribute(pchar,"GwikTimer");
}

// ГВИК: подписка на сводки цен, закупочные контракты и аукцион конфиската -->
void GWIK_RefreshAllPrices()
{
	for (int i = 0; i < MAX_COLONIES; i++)
	{
		ref rcolony = &Colonies[i];
		if (rcolony.id != "Caiman" && rcolony.id != "Panama" && rcolony.nation != "none") SetPriceListByStoreMan(rcolony);
	}
}

void GWIK_SubscriptionTick(string qName)
{
	if (!CheckAttribute(pchar, "GWIK.Subscription")) return;
	GWIK_RefreshAllPrices();
	pchar.GWIK.Subscription = sti(pchar.GWIK.Subscription) - 5;
	if (sti(pchar.GWIK.Subscription) > 0)
	{
		if (LanguageGetLanguage() == "russian") log_info("Курьер ГВИК доставил свежую сводку цен по архипелагу.");
		else log_info("A GWIC courier has delivered a fresh price report for the archipelago.");
		SetTimerFunction("GWIK_SubscriptionTick", 0, 0, 5);
		DeleteAttribute(pchar, "quest.GWIK_SubscriptionTick.over"); // OnQuestComplete уже пометил квест к удалению - иначе перевзвод не выживет
	}
	else
	{
		DeleteAttribute(pchar, "GWIK.Subscription");
		if (LanguageGetLanguage() == "russian") log_info("Подписка на торговые сводки ГВИК истекла.");
		else log_info("Your GWIC trade report subscription has expired.");
	}
}

void GWIK_DividendTick(string qName) // ежемесячные дивиденды пайщика
{
	if (!CheckAttribute(pchar, "GWIK.Share")) return;
	int iRate = 2;                                                        // % в месяц: Пайщик
	int iLvl = 0;
	if (CheckAttribute(pchar, "GWIK.Share.Level")) iLvl = sti(pchar.GWIK.Share.Level);
	if (iLvl == 2) iRate = 3;                                            // Партнёр
	if (iLvl >= 3) iRate = 4;                                            // Компаньон
	if (CheckAttribute(pchar, "GWIK.Jamaica.Founder")) iRate = iRate + 1; // Gregg: соучредитель конторы Форт-Оранжа - +1% (Компаньон снова 5%)
	int iAdd = makeint(sti(pchar.GWIK.Share.Invested) * iRate / 100);
	if (!CheckAttribute(pchar, "GWIK.Share.Pool")) pchar.GWIK.Share.Pool = 0;
	pchar.GWIK.Share.Pool = sti(pchar.GWIK.Share.Pool) + iAdd;
	if (LanguageGetLanguage() == "russian")
	{
		log_info("ГВИК начислила дивиденды: " + FindRussianMoneyString(iAdd) + ".");
		log_info("Получить их можно в любой конторе Компании.");
	}
	else
	{
		log_info("The GWIC has credited your dividends: " + FindRussianMoneyString(iAdd) + ".");
		log_info("Collect them at any Company office.");
	}
	SetTimerFunction("GWIK_DividendTick", 0, 1, 0); // раз в месяц
	DeleteAttribute(pchar, "quest.GWIK_DividendTick.over"); // OnQuestComplete уже пометил квест к удалению - иначе перевзвод не выживет
}

int GWIK_VolumeMult() // множитель объёма партий (контракт/конфискат) в %, растёт с уровнем ГГ и уровнем пая
{
	int shareLvl = 0;
	if (CheckAttribute(pchar, "GWIK.Share.Level")) shareLvl = sti(pchar.GWIK.Share.Level);
	int m = 100 + sti(pchar.rank) * 4 + shareLvl * 25; // +4% за уровень ГГ, +25% за уровень пая
	if (m > 300) m = 300;                               // потолок - втрое от базы
	return m;
}

// Gregg: репутация в Компании (pchar.GWIK.Rep) - копится с делами, отдельно от пая (тот про деньги, репутация - про доверие).
// Гейтит выкуп трофеев, особые поручения и лучшие лоты. Очки -> уровень 0..4.
int GWIK_RepLevel()
{
	int r = 0;
	if (CheckAttribute(pchar, "GWIK.Rep")) r = sti(pchar.GWIK.Rep);
	if (r >= 700) return 4;
	if (r >= 350) return 3;
	if (r >= 150) return 2;
	if (r >= 50)  return 1;
	return 0;
}

string GWIK_RepLevelName(int iLvl) // Gregg: название ранга доверия - для лога и интерфейса (в диалогах свои тексты по локалям)
{
	if (LanguageGetLanguage() == "russian")
	{
		if (iLvl >= 4) return "столп Компании";
		if (iLvl == 3) return "друг Компании";
		if (iLvl == 2) return "доверенное лицо";
		if (iLvl == 1) return "знакомый Компании";
		return "случайный клиент";
	}
	if (iLvl >= 4) return "pillar of the Company";
	if (iLvl == 3) return "friend of the Company";
	if (iLvl == 2) return "trusted hand";
	if (iLvl == 1) return "Company acquaintance";
	return "casual client";
}

int GWIK_RepNextTarget() // Gregg: порог следующего ранга доверия; -1 - ранг уже высший
{
	int iLvl = GWIK_RepLevel();
	if (iLvl == 0) return 50;
	if (iLvl == 1) return 150;
	if (iLvl == 2) return 350;
	if (iLvl == 3) return 700;
	return -1;
}

void GWIK_ArmRankLetter(int iTier) // Gregg: взвести письмо за новый ранг. Отдельной функцией - зовётся из GWIK_AddRep, который стоит ВЫШЕ по файлу
{
	int iSent = 0;
	if (CheckAttribute(pchar, "GWIK.RankLetter.Sent")) iSent = sti(pchar.GWIK.RankLetter.Sent);
	if (iTier <= iSent) return; // за этот ранг письмо уже уходило
	pchar.GWIK.RankLetter.Pending = iTier;
	DeleteAttribute(pchar, "quest.GWIK_RankLetter.over"); // иначе перевзвод таймера мёртв (over от прошлого письма)
	SetTimerFunction("GWIK_RankLetter", 0, 0, 3); // письмо идёт три дня, как письмо Семнадцати Господ
}

string GWIK_RankGift(int iTier) // Gregg: подарок к письму за новый ранг доверия. Порядок: карта сокровищ -> книга -> деньги.
// Возвращает готовую строку «что пришло» для письма (обе локали).
// ВАЖНО: map_full - ОДИН глобальный предмет, весь клад (остров/тайник/содержимое) лежит в его шаблоне.
// Поэтому новую карту генерим ТОЛЬКО когда целой карты у игрока нет: иначе затрём неоткопанный клад
{
	bool bRus = (LanguageGetLanguage() == "russian");
	ref rMap;
	string sPirate = "";
	string sBook = "";
	int iHalf1 = GetCharacterItem(pchar, "map_part1");
	int iHalf2 = GetCharacterItem(pchar, "map_part2");
	if (GetCharacterItem(pchar, "map_full") < 1)
	{
		// половинка есть ровно одна - шлём недостающую: движок сам склеит их в карту клада 2 уровня при открытии инвентаря
		if (iHalf1 > 0 && iHalf2 < 1)
		{
			GiveItem2Character(pchar, "map_part2");
			if (bRus) return "недостающая половина карты сокровищ"; else return "the missing half of a treasure map";
		}
		if (iHalf2 > 0 && iHalf1 < 1)
		{
			GiveItem2Character(pchar, "map_part1");
			if (bRus) return "недостающая половина карты сокровищ"; else return "the missing half of a treasure map";
		}
		rMap = ItemsFromID("map_full");
		DeleteAttribute(rMap, "MapIslId"); // старые данные от прошлого (уже откопанного) клада - иначе карта придёт с чужим тайником
		DeleteAttribute(rMap, "uniquep");  // и без пометки «клад легендарного пирата» от прошлой именной карты
		if (iTier >= 4)
		{
			sPirate = GetUniquePirateName(); // пул именных кладов: пусто - значит все четыре уже разыграны
			if (sPirate != "")
			{
				FillMapForUniqueTreasure(rMap, sPirate);
				GiveItem2Character(pchar, "map_full");
				if (bRus) return "карта клада легендарного пирата"; else return "a chart to a legendary pirate's hoard";
			}
		}
		if (iTier >= 3) pchar.GenQuest.TreasureBuild = true; // ветка «из кусочков» = клад 2 уровня (супер-вещи); флаг гасится внутри FillMapForTreasure
		FillMapForTreasure(rMap);
		if (sti(rMap.MapTypeIdx) == -1)
		{
			rMap.MapTypeIdx = rand(2); // Gregg: подарок Компании не может оказаться фальшивкой - глушим встроенный шанс подделки
			ArmTreasureMapCondition(rMap); // LEO: у фальшивки условие не ставилось - без него клад бы не появился
		}
		GiveItem2Character(pchar, "map_full");
		if (iTier >= 3)
		{
			if (bRus) return "карта богатого клада"; else return "a chart to a rich hoard";
		}
		if (bRus) return "карта сокровищ"; else return "a treasure map";
	}
	// карта на руках - клад не трогаем, шлём книгу нужного тира (RegenerateBook сам ищет НЕ читанную, до пяти попыток)
	sBook = RegenerateBook(iTier);
	if (sBook != "" && !CheckForBookReady(sBook))
	{
		GiveItem2Character(pchar, sBook);
		// название предмета живёт в ItemsDescribe.txt, а не в общем файле интерфейса (идиома из GetRusNameNationLicence)
		string sBookName = LanguageConvertString(LanguageOpenFile("ItemsDescribe.txt"), Items[GetItemIndex(sBook)].name);
		if (bRus) return "книга «" + sBookName + "»"; else return "the book '" + sBookName + "'";
	}
	// и книги все читаны - остаётся денежная премия
	AddMoneyToCharacter(pchar, 125000 * iTier);
	if (bRus) return "денежная премия в " + FindRussianMoneyString(125000 * iTier); // Gregg: слово «пиастров» дописывает сам FindRussianMoneyString - второй раз не надо
	return "a cash bonus of " + FindRussianMoneyString(125000 * iTier);
}

void GWIK_RankLetter(string qName) // Gregg: письмо из конторы за новый ранг доверия, приходит через три дня (образец - GWIK_FoundersGift)
{
	int iTier = 0;
	if (CheckAttribute(pchar, "GWIK.RankLetter.Pending")) iTier = sti(pchar.GWIK.RankLetter.Pending);
	DeleteAttribute(pchar, "GWIK.RankLetter.Pending");
	if (iTier < 1) return;
	if (iTier > 4) iTier = 4;
	int iSent = 0;
	if (CheckAttribute(pchar, "GWIK.RankLetter.Sent")) iSent = sti(pchar.GWIK.RankLetter.Sent);
	if (iTier <= iSent) return; // за этот ранг письмо уже уходило: доверие может качнуться вниз и снова вверх, письмо - один раз
	pchar.GWIK.RankLetter.Sent = iTier;
	string sGift = GWIK_RankGift(iTier);
	// Gregg: КОРОТКИМИ строками и по одной на Log_Info - длинная строка обрезается по ширине экрана (первое письмо не влезло).
	// Держать до ~70 знаков; в Журнал игры каждая строка ложится отдельной записью с датой
	if (LanguageGetLanguage() == "russian")
	{
		Log_Info("Письмо из конторы Компании.");
		if (iTier == 1) Log_Info("Вы внесены в книги как знакомый Компании.");
		if (iTier == 2) Log_Info("Отныне вы значитесь у нас доверенным лицом.");
		if (iTier == 3) Log_Info("Правление сочло вас другом Компании.");
		if (iTier == 4) Log_Info("Семнадцать Господ признали вас столпом Компании.");
		Log_Info("С письмом пришло: " + sGift + ".");
	}
	else
	{
		Log_Info("A letter from the Company office.");
		if (iTier == 1) Log_Info("You are entered in our books as an acquaintance of the Company.");
		if (iTier == 2) Log_Info("From now on you stand in our books as a trusted hand.");
		if (iTier == 3) Log_Info("The board has judged you a friend of the Company.");
		if (iTier == 4) Log_Info("The Seventeen Gentlemen name you a pillar of the Company.");
		Log_Info("Enclosed: " + sGift + ".");
	}
	PlayStereoSound("notebook");
}

void GWIK_AddRep(int n)
{
	int r = 0;
	if (CheckAttribute(pchar, "GWIK.Rep")) r = sti(pchar.GWIK.Rep);
	int iLvlWas = GWIK_RepLevel(); // ранг ДО начисления - чтобы поймать переход
	r = r + n;
	if (r < 0) r = 0;
	pchar.GWIK.Rep = r;
	if (n == 0) return;
	// Gregg: видимое сообщение (Log_Info пишет и в Журнал игры с датой) - иначе доверие Компании росло молча:
	// понять, что и на сколько его двинуло, было нельзя
	int iLvlNow = GWIK_RepLevel();
	int iNext = GWIK_RepNextTarget();
	string sSign = "+";
	if (n < 0) sSign = "";
	if (LanguageGetLanguage() == "russian")
	{
		if (iNext > 0) Log_Info("Доверие ГВИК: " + sSign + n + " (стало " + r + " из " + iNext + ")");
		else Log_Info("Доверие ГВИК: " + sSign + n + " (стало " + r + ", высший ранг)");
		if (iLvlNow > iLvlWas) Log_Info("Компания повысила ваш ранг: " + GWIK_RepLevelName(iLvlNow));
		if (iLvlNow < iLvlWas) Log_Info("Компания понизила ваш ранг: " + GWIK_RepLevelName(iLvlNow));
		if (iLvlNow > iLvlWas) GWIK_ArmRankLetter(iLvlNow); // повышение - через три дня придёт письмо с подарком
	}
	else
	{
		if (iNext > 0) Log_Info("WIC standing: " + sSign + n + " (now " + r + " of " + iNext + ")");
		else Log_Info("WIC standing: " + sSign + n + " (now " + r + ", highest rank)");
		if (iLvlNow > iLvlWas) Log_Info("The Company has raised your rank: " + GWIK_RepLevelName(iLvlNow));
		if (iLvlNow < iLvlWas) Log_Info("The Company has lowered your rank: " + GWIK_RepLevelName(iLvlNow));
		if (iLvlNow > iLvlWas) GWIK_ArmRankLetter(iLvlNow); // повышение - через три дня придёт письмо с подарком
	}
}

// Gregg: реестр контор ГВИК (Кюрасао / Сан Мартин / Форт-Оранж). Форт-Оранж (idx 2) считается конторой только после открытия квестом (GWIK.Jamaica.Done).
int GWIK_OfficeCount()
{
	// Gregg: третью контору учитываем только когда квест выставил флаг И представитель реально заспавнен,
	// иначе GWIK_PartnerId направит почту/вексель на несуществующий NPC (поручение станет невыполнимым).
	if (CheckAttribute(pchar, "GWIK.Jamaica.Done") && GetCharacterIndex("FortOrange_GWIK_char") != -1) return 3;
	return 2;
}
string GWIK_OfficeIdByIdx(int i)
{
	if (i == 1) return "GVIK_char";
	if (i == 2) return "FortOrange_GWIK_char";
	return "GWIK_char";
}
int GWIK_OfficeIdxById(string sId)
{
	if (sId == "GVIK_char") return 1;
	if (sId == "FortOrange_GWIK_char") return 2;
	return 0;
}
string GWIK_OfficeCityById(string sId)
{
	if (sId == "GVIK_char") return "Marigo";
	if (sId == "FortOrange_GWIK_char") return "FortOrange";
	return "Villemstad";
}
string GWIK_PartnerId(string sSelf) // контора-партнёр (почта/вексель) - следующая ОТКРЫТАЯ контора по кольцу
{
	int cnt = GWIK_OfficeCount();
	int p = GWIK_OfficeIdxById(sSelf) + 1;
	if (p >= cnt) p = 0;
	return GWIK_OfficeIdByIdx(p);
}

// Gregg: локации перенесённых квестов К:ПС заводятся штатно, в InitLocations. Раньше
// здесь жил набор *_Ensure*-функций, дописывавших их в хвост массива на уже начатой
// партии: Locations сохраняется в сейв целиком, и правки инита до старого прохождения
// не доезжали. С отказом от миграций надобность отпала - мод рассчитан на новую игру.

// LEO: контора Форт-Оранжа ночью закрыта, как в Мариго (close_for_night у дверей) и в Виллемстаде (вход
//   через банк). Зовётся при открытии конторы и из OnLoad - для партий, где она открыта раньше (репорт 24.09.2026).
void GWIK_FortOrangeDoorNightClose()
{
	if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) return;
	int loc = FindLocation("FortOrange_town");
	if (loc < 0) return;
	if (!CheckAttribute(&Locations[loc], "reload.l24.go")) return;
	if (Locations[loc].reload.l24.go != "FortOrange_GWIK") return;
	Locations[loc].reload.l24.close_for_night = 1;
}

void GWIK_OpenFortOrangeOffice() // открыть контору Форт-Оранжа: локация + дверь reload_H3 + представитель + флаг (идемпотентно)
{
	if (FindLocation("FortOrange_GWIK") < 0) return; // локацию создать не удалось - не вешаем битую дверь
	pchar.GWIK.Jamaica.Done = true;
	int loc = FindLocation("FortOrange_town"); // дверь reload_H3 (reload.l24) -> контора
	if (loc >= 0)
	{
		Locations[loc].reload.l24.go = "FortOrange_GWIK";
		Locations[loc].reload.l24.label = "GWIK";
		// Gregg: снять залипший disable, оставленный суточным рандомайзером дверей-заглушек (reload.c:614 ставит rand(1) каждой двери, у которой в .go есть "Common").
		// До открытия конторы reload_H3 вела в CommonRoom_MH9 и перекатывалась каждые сутки; после смены .go подстроки "Common" нет - рандомайзер её больше не тронет,
		// и выпавшая единица заперла бы контору НАВСЕГДА (chrCheckReload -> "Closed" и для двери, и для быстрого перехода).
		if (CheckAttribute(&Locations[loc], "reload.l24.disable"))  DeleteAttribute(&Locations[loc], "reload.l24.disable");
		if (CheckAttribute(&Locations[loc], "reload.l24.canEnter")) DeleteAttribute(&Locations[loc], "reload.l24.canEnter"); // мусор того же рандомайзера
	}
	GWIK_FortOrangeDoorNightClose();
	// Gregg: сбросить кэш путей быстрого перехода - иначе новая дверь конторы не попадёт в граф, если контору открыли, стоя в той же группе локаций (сценарий квеста в Форт-Оранже). Граф перестроится при следующем входе в локацию.
	if (CheckAttribute(&objFastReloadTable, "Paths")) DeleteAttribute(&objFastReloadTable, "Paths");
	if (GetCharacterIndex("FortOrange_GWIK_char") == -1) // представитель конторы (по образцу GWIK_char)
	{
		string sMdl = "trader_11";
		if (CheckAttribute(pchar, "GWIK.Jamaica.FactorModel")) sMdl = pchar.GWIK.Jamaica.FactorModel; // Gregg: управляющий - тот самый фактор, лицо должно совпадать
		ref ch = GetCharacter(NPC_GenerateCharacter("FortOrange_GWIK_char", sMdl, "man", "man", 10, HOLLAND, -1, false));
		if (CheckAttribute(pchar, "GWIK.Jamaica.FactorFName")) // Gregg: управляющим становится ТОТ САМЫЙ фактор, которого игрок привёз (имя переносится)
		{
			ch.name = pchar.GWIK.Jamaica.FactorFName;
			ch.lastname = pchar.GWIK.Jamaica.FactorLName;
		}
		else SetRandomNameToCharacter(ch); // бета-открытие без квеста
		ch.City = "FortOrange";
		ch.location = "FortOrange_GWIK";
		ch.location.group = "sit";
		ch.location.locator = "sit1";
		ch.Dialog.Filename = "GWIK.c";
		ch.nation = HOLLAND;
		ch.NotRemoved = true;
		ch.watchBoxes = true; // Gregg: паритет с GWIK_char (Кюрасао)
		LAi_SetSitTypeNoGroup(ch);
		LAi_group_MoveCharacter(ch, "HOLLAND_CITIZENS");
		LAi_SetHP(ch, 200.0, 200.0);
		LAi_SetImmortal(ch, true);
	}
}

// Gregg: особый квест «Контора на Ямайке» (открытие 3-й конторы в Форт-Оранже). Гейт: Компаньон + репутация >=3 + голландский Форт-Оранж, не начат и не пройден.
bool GWIK_JamaicaEligible()
{
	if (CheckAttribute(pchar, "GWIK.Jamaica.Done")) return false;
	if (CheckAttribute(pchar, "GWIK.Jamaica.Stage")) return false; // уже в работе
	int sl = 0;
	if (CheckAttribute(pchar, "GWIK.Share.Level")) sl = sti(pchar.GWIK.Share.Level);
	if (sl < 3) return false;                                   // только Компаньон
	if (GWIK_RepLevel() < 3) return false;                      // друг Компании и выше
	if (GetCityNation("FortOrange") != HOLLAND) return false;   // Форт-Оранж должен быть голландским
	return true;
}

void GWIK_JamaicaSpawnEnvoy() // эмиссар Компании в ратуше Форт-Оранжа (ведёт квест на месте)
{
	if (GetCharacterIndex("GWIK_Envoy") != -1) return; // уже есть
	ref ev = GetCharacter(NPC_GenerateCharacter("GWIK_Envoy", "trader_4", "man", "man", 10, HOLLAND, -1, false));
	SetRandomNameToCharacter(ev);
	ev.City = "FortOrange";
	ev.location = "FortOrange_townhall";
	ev.location.group = "goto";
	ev.location.locator = "goto10"; // в ратуше свободен goto10 (sit1 занят мэром, sit2 нет)
	ev.Dialog.Filename = "GWIK_Jamaica.c";
	ev.nation = HOLLAND;
	ev.NotRemoved = true;
	LAi_SetOwnerType(ev); // стоячий «хозяин» (как лавочник) - стоит на месте и разговаривает; Actor-тип без команды не интерактивен
	LAi_group_MoveCharacter(ev, "HOLLAND_CITIZENS");
	LAi_SetHP(ev, 100.0, 100.0);
	LAi_SetImmortal(ev, true);
}

void GWIK_StartJamaicaQuest() // принять квест: участие в открытии конторы Компании в Форт-Оранже
{
	pchar.GWIK.Jamaica.Stage = 1;
	GWIK_JamaicaSpawnEnvoy();
	PlayStereoSound("notebook");
	ReOpenQuestHeader("GWIK_JamaicaJob");
	AddQuestRecord("GWIK_JamaicaJob", "1");
	if (LanguageGetLanguage() == "russian") log_info("Новое дело Компании: контора на Ямайке.");
	else log_info("A new Company affair: an office on Jamaica.");
}

bool GWIK_JamaicaSpawnCourier() // Gregg: английский курьер с доносом - квестовый энкаунтер МИРОВОЙ КАРТЫ (по образцу SetJornalCapParam): идёт из вод Ямайки в английский порт, перехватить до прибытия (этап 2). Дошёл - см. ветку GWIK_JamCourier в Map_TraderSucces_quest (донос ушёл -> откуп)
{
	// fixed id (без суффикса) - его ждёт Map_CreateTrader / win_condition / Map_TraderSucces_quest
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_JamCourier", "", "man", "man", sti(pchar.rank), ENGLAND, -1, true));
	SetCaptanModelByEncType(sld, "trade");
	DeleteAttribute(sld, "Ship");
	SetShipToFantom(sld, "trade", true);
	sld.Ship.Type = GenerateShipExt(SHIP_CORVETTELIGHT, true, sld); // лёгкий корвет - правдоподобный курьер (а не случайный галеон)
	SetBaseShipData(sld);
	SetCrewQuantityFull(sld);
	Fantom_SetCannons(sld, "trade"); // Gregg: пересчитать калибр под фактический корвет - SetShipToFantom выставил его под СЛУЧАЙНЫЙ корабль по рангу игрока (SetBaseShipData калибр не перезаписывает - отсюда «какой попало калибр»)
	Fantom_SetBalls(sld, "trade"); // Gregg: и боезапас/порох под фактический корпус (тем же корнем был занижен - считался по числу пушек мелкого случайного корабля)
	// Gregg: личность курьера - имя капитана и корабля видны в трубу/на карте и упоминаются в текстах
	if (LanguageGetLanguage() == "russian") { sld.name = "Джереми"; sld.lastname = "Пейдж"; sld.Ship.Name = "Стриж"; }
	else { sld.name = "Jeremy"; sld.lastname = "Page"; sld.Ship.Name = "Swift"; }
	sld.Ship.Mode = "trade";
	sld.AlwaysSandbankManeuver = true;
	sld.DontRansackCaptain = true;
	SetRandGeraldSail(sld, sti(sld.Nation));
	Group_FindOrCreateGroup("GWIK_JamCourierGroup");
	Group_SetType("GWIK_JamCourierGroup", "trade");
	Group_AddCharacter("GWIK_JamCourierGroup", sld.id);
	Group_SetGroupCommander("GWIK_JamCourierGroup", sld.id);
	pchar.GWIK.Jamaica.CourierId = sld.id;
	// маршрут на мировой карте: берег Ямайки -> английский (не враждебный Англии) порт
	string sToCity = SelectNotEnemyColony(sld);
	if (sToCity == "") return false; // некуда вести - не загоняем квест в тупик
	string sFromAreal = GetArealByCityName("FortOrange"); // Ямайка
	string sFromShore = GetIslandRandomShoreId(sFromAreal);
	sld.mapEnc.type = "trade";
	sld.mapEnc.worldMapShip = "Galleon_red";
	sld.mapEnc.Name = "Курьер 'Стриж'";
	if (LanguageGetLanguage() != "russian") sld.mapEnc.Name = "Courier 'Swift'";
	int iDays = GetMaxDaysFromIsland2Island(sFromAreal, GetArealByCityName(sToCity)) + 2;
	Map_CreateTrader(sFromShore, sToCity, sld.id, iDays);
	pchar.quest.GWIK_JamCourierDead.win_condition.l1 = "NPC_Death";
	pchar.quest.GWIK_JamCourierDead.win_condition.l1.character = sld.id;
	pchar.quest.GWIK_JamCourierDead.win_condition = "GWIK_JamCourierDead";
	pchar.quest.GWIK_JamCourierDead.function = "GWIK_JamCourierDead";
	return true;
}

void GWIK_JamCourierDead(string qName) // курьер потоплен - доложить эмиссару
{
	if (!CheckAttribute(pchar, "GWIK.Jamaica.Stage")) return;
	if (sti(pchar.GWIK.Jamaica.Stage) != 2) return;
	pchar.GWIK.Jamaica.CourierDone = true;
	Map_ReleaseQuestEncounter("GWIK_JamCourier"); // снять карт-энкаунтер (курьер потоплен)
	Group_DeleteGroup("GWIK_JamCourierGroup");
	AddQuestRecord("GWIK_JamaicaJob", "3");
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Английский курьер с доносом потоплен.");
		log_info("Вернитесь к эмиссару Компании в Форт-Оранже.");
	}
	else
	{
		log_info("The English courier with the denunciation is sunk.");
		log_info("Return to the Company envoy in Fort Orange.");
	}
}

void GWIK_JamaicaSpawnFactorInOffice() // Gregg: этап 4 - фактор входит в контору с улицы и САМ подходит представиться (паттерн конвойного пассажира); диалог GWIK_Factor.c
{
	if (CheckAttribute(pchar, "GWIK.Jamaica.FactorId")) return; // уже представлен (стоит в конторе или на борту)
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_Factor", "trader_" + (rand(3) + 1), "man", "man", 5 + rand(10), HOLLAND, 0, false));
	sld.id = sld.id + "_" + sld.index;
	SetRandomNameToCharacter(sld); // у фактора есть имя и лицо - он же станет управляющим новой конторы
	pchar.GWIK.Jamaica.FactorFName = sld.name;
	pchar.GWIK.Jamaica.FactorLName = sld.lastname;
	pchar.GWIK.Jamaica.FactorModel = sld.model; // модель тоже переносится на управляющего
	pchar.GWIK.Jamaica.FactorId = sld.id;
	DeleteAttribute(sld, "ship");
	sld.ship = "";
	sld.Dialog.Filename = "GWIK_Factor.c";
	sld.nation = HOLLAND;
	sld.NotRemoved = true;
	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
	LAi_SetHP(sld, 80.0, 80.0);
	LAi_SetImmortal(sld, true);
	ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1"); // появляется у двери конторы (reload1 есть в GWIK/GVIK/клоне)
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0); // сам подходит и заговаривает - представляется без посредника
}

void GWIK_JamaicaFactorBoard() // Gregg: фактор поднимается на борт (зовётся из ЕГО диалога, узел Factor_Go) и уходит из конторы «на пристань»
{
	int fi = -1;
	if (CheckAttribute(pchar, "GWIK.Jamaica.FactorId")) fi = GetCharacterIndex(pchar.GWIK.Jamaica.FactorId);
	if (fi == -1) return;
	ref sld = GetCharacter(fi);
	AddPassenger(pchar, sld, false);
	SetCharacterRemovable(sld, false); // квест-пассажир: нельзя ставить офицером/ссаживать
	pchar.GWIK.Jamaica.FactorAboard = true;
	GWIK_JamaicaSpawnAmbush(); // конкуренты уже ждут у Ямайки - обещанная засада на фактора
	PlayStereoSound("notebook");
	AddQuestRecord("GWIK_JamaicaJob", "6");
	AddQuestUserData("GWIK_JamaicaJob", "sFactor", GetFullName(sld)); // имя фактора в журнал (строго после AddQuestRecord)
	// уходит из конторы через дверь и исчезает («понёс вещи на пристань»)
	LAi_SetActorType(sld);
	LAi_ActorGoToLocation(sld, "reload", "reload1", "none", "", "", "", 20.0);
}

bool GWIK_JamaicaSpawnAmbush() // Gregg: засада на фактора (этап 4) - перехватчик конкурентов ждёт у Форт-Оранжа; для прохождения НЕобязателен: потопить или просто оторваться
{
	string sAreal = GetArealByCityName("FortOrange");
	string sLocator = "";
	int iIsl = FindIsland(sAreal);
	if (iIsl >= 0 && CheckAttribute(&Islands[iIsl], "Quest_ships.Quest_ship_7")) sLocator = "Quest_ship_7";
	if (sLocator == "") sLocator = GWIK_PickQuestShipLocator(sAreal);
	if (sLocator == "") return false; // некуда ставить - засады просто не будет, квест не страдает
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_JamAmbush", "", "man", "man", sti(pchar.rank), ENGLAND, -1, true));
	sld.id = sld.id + "_" + sld.index;
	SetCaptanModelByEncType(sld, "trade");
	DeleteAttribute(sld, "Ship");
	SetShipToFantom(sld, "trade", true);
	sld.Ship.Type = GenerateShipExt(SHIP_CORVETTELIGHT, true, sld); // лёгкий перехватчик
	SetBaseShipData(sld);
	SetCrewQuantityFull(sld);
	Fantom_SetCannons(sld, "war"); // Gregg: калибр под фактический корвет-перехватчик (иначе остаётся от случайного корабля SetShipToFantom)
	Fantom_SetBalls(sld, "war"); // Gregg: и боезапас/порох под фактический корпус (тем же корнем был занижен)
	if (LanguageGetLanguage() == "russian") { sld.name = "Сайлас"; sld.lastname = "Крейн"; sld.Ship.Name = "Тень"; }
	else { sld.name = "Silas"; sld.lastname = "Crane"; sld.Ship.Name = "Shade"; }
	sld.Ship.Mode = "War";
	sld.AlwaysSandbankManeuver = true;
	sld.DontRansackCaptain = true;
	sld.AlwaysEnemy = true; // нападает сам - обещание «конкуренты не дремлют» сбывается
	Group_FindOrCreateGroup("GWIK_JamAmbushGroup");
	Group_SetType("GWIK_JamAmbushGroup", "war");
	Group_AddCharacter("GWIK_JamAmbushGroup", sld.id);
	Group_SetGroupCommander("GWIK_JamAmbushGroup", sld.id);
	Group_SetAddress("GWIK_JamAmbushGroup", sAreal, "Quest_ships", sLocator);
	Group_LockTask("GWIK_JamAmbushGroup");
	pchar.GWIK.Jamaica.AmbushId = sld.id;
	pchar.quest.GWIK_JamAmbushDead.win_condition.l1 = "NPC_Death";
	pchar.quest.GWIK_JamAmbushDead.win_condition.l1.character = sld.id;
	pchar.quest.GWIK_JamAmbushDead.win_condition = "GWIK_JamAmbushDead";
	pchar.quest.GWIK_JamAmbushDead.function = "GWIK_JamAmbushDead";
	return true;
}

void GWIK_JamAmbushDead(string qName) // перехватчик потоплен - фактор спасён с боем
{
	if (!CheckAttribute(pchar, "GWIK.Jamaica.AmbushId")) return;
	DeleteAttribute(pchar, "GWIK.Jamaica.AmbushId");
	Group_DeleteGroup("GWIK_JamAmbushGroup");
	GWIK_AddRep(10); // Компания ценит защиту своих людей
	AddQuestRecord("GWIK_JamaicaJob", "13");
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Перехватчик конкурентов - 'Тень' Сайласа Крейна - на дне.");
		log_info("Путь к Форт-Оранжу свободен.");
	}
	else
	{
		log_info("The rivals' interceptor - Silas Crane's 'Shade' - is sunk.");
		log_info("The way to Fort Orange is clear.");
	}
}

void GWIK_JamaicaClearAmbush() // фактор доставлен: если от перехватчика просто оторвались - тихо убрать его
{
	pchar.quest.GWIK_JamAmbushDead.over = "yes"; // снять прерывание на его смерть
	if (CheckAttribute(pchar, "GWIK.Jamaica.AmbushId"))
	{
		int i = GetCharacterIndex(pchar.GWIK.Jamaica.AmbushId);
		if (i != -1) LAi_SetCurHP(&Characters[i], 0.0); // освобождаем слот
		DeleteAttribute(pchar, "GWIK.Jamaica.AmbushId");
	}
	Group_DeleteGroup("GWIK_JamAmbushGroup");
}

// --- этап 3: снабжение конторы через ЭСКОРТ конвоя из Кюрасао (2 галеона-компаньона + 2 корвета-преследователя) ---

void GWIK_JamaicaMakePursuerShip(ref sld) // Gregg: настроить один корвет эскадры-преследователя (корвет, война, агрессия + поля мир.карты для лидера-энкаунтера)
{
	SetCaptanModelByEncType(sld, "trade");
	DeleteAttribute(sld, "Ship");
	SetShipToFantom(sld, "war", true);
	sld.Ship.Type = GenerateShipExt(SHIP_CORVETTE, true, sld);
	SetBaseShipData(sld);
	SetCrewQuantityFull(sld);
	Fantom_SetCannons(sld, "war"); // Gregg: калибр под фактический корвет (иначе остаётся от случайного корабля SetShipToFantom)
	Fantom_SetBalls(sld, "war"); // Gregg: и боезапас/порох под фактический корпус (тем же корнем был занижен)
	sld.Ship.Name = generateRandomNameToShip(ENGLAND);
	sld.Ship.Mode = "War";
	sld.AlwaysSandbankManeuver = true;
	sld.AlwaysEnemy = true;        // агрессивен в морском бою (нац.отношения Англии мало)
	sld.DontRansackCaptain = true;
	SetRandGeraldSail(sld, sti(sld.Nation));
	sld.mapEnc.type = "war";       // для лидера ОБЯЗАТЕЛЬНО (иначе GenerateMapEncounter_Alone не спавнит энкаунтер)
	sld.mapEnc.worldMapShip = "quest_ship";
	if (LanguageGetLanguage() == "russian") sld.mapEnc.Name = "Корветы конкурентов";
	else sld.mapEnc.Name = "Rival corvettes";
}

void GWIK_JamaicaSpawnPursuer() // Gregg: 3 корвета конкурентов ДОГОНЯЮТ конвой от Кюрасао (эскадра). КЛЮЧ: группа зовётся "Sea_"+id лидера - тогда в бой грузится ВСЯ группа (DirSailEnc.c грузит группу Sea_<charID>), как Золотой флот/осадные отряды
{
	int r = sti(pchar.rank);
	string sGroup = "Sea_GWIK_JamPursuer0"; // "Sea_" + id лидера (GWIK_JamPursuer0) - обязательное имя
	Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup, "war");
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP); // догонять и атаковать игрока на карте
	Group_LockTask(sGroup);
	ref p;                                   // рефы/счётчик - ВНЕ цикла (Storm не любит объявление ref в теле for)
	int k;
	string sid;
	for (k = 0; k < 3; k++)
	{
		sid = "GWIK_JamPursuer" + k;          // фикс id "GWIK_JamPursuer0/1/2" (0 - лидер-энкаунтер)
		p = GetCharacter(NPC_GenerateCharacter(sid, "", "man", "man", r, ENGLAND, -1, true));
		GWIK_JamaicaMakePursuerShip(p);
		Group_AddCharacter(sGroup, sid);
		if (k == 0) Group_SetGroupCommander(sGroup, sid);
	}
	Map_CreateWarrior(GetIslandRandomShoreId(GetArealByCityName("Villemstad")), "GWIK_JamPursuer0", 20); // догоняющий от Кюрасао; лидер = блип, группа подтянется в бой
}

void GWIK_JamaicaConvoyCleanup(bool bRemoveComps) // снять условия конвоя и убрать корветы-перехватчики (+опц. вывести галеоны из эскадры)
{
	pchar.quest.GWIK_JamConvoy1Dead.over = "yes";
	pchar.quest.GWIK_JamConvoy2Dead.over = "yes";
	pchar.quest.GWIK_JamConvoyArrive.over = "yes";
	pchar.quest.GWIK_JamaicaConvoyTimeout.over = "yes";
	Map_ReleaseQuestEncounter("GWIK_JamPursuer0"); // снять карт-энкаунтер лидера, если ещё догоняет
	int k;
	int pi;
	for (k = 0; k < 3; k++) // убрать уцелевшие корветы эскадры (фикс id)
	{
		pi = GetCharacterIndex("GWIK_JamPursuer" + k);
		if (pi >= 0) LAi_SetCurHP(&Characters[pi], 0.0);
	}
	Group_DeleteGroup("Sea_GWIK_JamPursuer0");
	if (bRemoveComps)
	{
		ref c1 = CharacterFromID("GWIK_JamConvoy1");
		if (IsCompanion(c1)) RemoveCharacterCompanion(pchar, c1);
		ref c2 = CharacterFromID("GWIK_JamConvoy2");
		if (IsCompanion(c2)) RemoveCharacterCompanion(pchar, c2);
	}
}

void GWIK_JamaicaLoadGalleon(ref g) // Gregg: загрузить галеон конвоя товаром до ~80% свободного трюма (красное дерево + сандал + шёлк)
{
	int budget = makeint(GetCargoFreeSpace(g) * 80 / 100);
	if (budget < 3) return;
	int per = budget / 3; // поровну на три товара
	AddCharacterGoods(g, GOOD_MAHOGANY, GetGoodQuantityByWeight(GOOD_MAHOGANY, per));
	AddCharacterGoods(g, GOOD_SANDAL,   GetGoodQuantityByWeight(GOOD_SANDAL, per));
	AddCharacterGoods(g, GOOD_SILK,     GetGoodQuantityByWeight(GOOD_SILK, per));
}

bool GWIK_JamaicaSpawnConvoy() // этап 3: 2 галеона Компании (древесина+шёлк) входят в эскадру компаньонами; довести до Форт-Оранжа. Образец - generate_convoy_quest
{
	if (GetCompanionQuantity(pchar) > COMPANION_MAX - 2) return false; // нужно 2 свободных слота эскадры
	int lvl = makeint((sti(pchar.rank) + 60) / 4);
	// галеон 1
	ref g1 = GetCharacter(NPC_GenerateCharacter("GWIK_JamConvoy1", "", "man", "man", lvl, HOLLAND, -1, true)); // фикс id - для win_condition/IsCompanion
	SetCaptanModelByEncType(g1, "trade");
	DeleteAttribute(g1, "Ship");
	SetShipToFantom(g1, "trade", true);
	g1.Ship.Type = GenerateShipExt(SHIP_GALEON, true, g1);
	SetBaseShipData(g1);
	SetCrewQuantityFull(g1);
	Fantom_SetCannons(g1, "trade"); // Gregg: калибр под фактический галеон (иначе остаётся от случайного корабля SetShipToFantom)
	Fantom_SetBalls(g1, "trade"); // Gregg: и боезапас/порох под фактический корпус (до GWIK_JamaicaLoadGalleon - ядра резервируются раньше товара)
	GWIK_JamaicaLoadGalleon(g1); // трюм под древесину и шёлк
	g1.Ship.Name = generateRandomNameToShip(HOLLAND);
	g1.BakNation = g1.nation;
	g1.CompanionEnemyEnable = true;
	SetCompanionIndex(pchar, -1, sti(g1.index)); // Gregg: галеон - прямо в эскадру (обёртка CharacterIntoCompanionAndGoOut ломалась: требует актёра + общий слот CallFunctionParam на 2 вызова подряд = второй затирал первый)
	SetCharacterRemovable(g1, false);
	DeleteAttribute(pchar, "quest.GWIK_JamConvoy1Dead.over");
	pchar.quest.GWIK_JamConvoy1Dead.win_condition.l1 = "NPC_Death";
	pchar.quest.GWIK_JamConvoy1Dead.win_condition.l1.character = "GWIK_JamConvoy1";
	pchar.quest.GWIK_JamConvoy1Dead.win_condition = "GWIK_JamConvoyShipDead";
	pchar.quest.GWIK_JamConvoy1Dead.function = "GWIK_JamConvoyShipDead";
	// галеон 2
	ref g2 = GetCharacter(NPC_GenerateCharacter("GWIK_JamConvoy2", "", "man", "man", lvl, HOLLAND, -1, true));
	SetCaptanModelByEncType(g2, "trade");
	DeleteAttribute(g2, "Ship");
	SetShipToFantom(g2, "trade", true);
	g2.Ship.Type = GenerateShipExt(SHIP_GALEON, true, g2);
	SetBaseShipData(g2);
	SetCrewQuantityFull(g2);
	Fantom_SetCannons(g2, "trade"); // Gregg: калибр под фактический галеон (иначе остаётся от случайного корабля SetShipToFantom)
	Fantom_SetBalls(g2, "trade"); // Gregg: и боезапас/порох под фактический корпус (до GWIK_JamaicaLoadGalleon - ядра резервируются раньше товара)
	GWIK_JamaicaLoadGalleon(g2); // трюм под древесину и шёлк
	g2.Ship.Name = generateRandomNameToShip(HOLLAND);
	g2.BakNation = g2.nation;
	g2.CompanionEnemyEnable = true;
	SetCompanionIndex(pchar, -1, sti(g2.index));
	SetCharacterRemovable(g2, false);
	DeleteAttribute(pchar, "quest.GWIK_JamConvoy2Dead.over");
	pchar.quest.GWIK_JamConvoy2Dead.win_condition.l1 = "NPC_Death";
	pchar.quest.GWIK_JamConvoy2Dead.win_condition.l1.character = "GWIK_JamConvoy2";
	pchar.quest.GWIK_JamConvoy2Dead.win_condition = "GWIK_JamConvoyShipDead";
	pchar.quest.GWIK_JamConvoy2Dead.function = "GWIK_JamConvoyShipDead";
	// прибытие в Форт-Оранж = снабжение доставлено (Location порта)
	DeleteAttribute(pchar, "quest.GWIK_JamConvoyArrive.over");
	pchar.quest.GWIK_JamConvoyArrive.win_condition.l1 = "Location";
	pchar.quest.GWIK_JamConvoyArrive.win_condition.l1.location = GetPortByCityName("FortOrange"); // "FortOrange_port"
	pchar.quest.GWIK_JamConvoyArrive.win_condition = "GWIK_JamaicaConvoyArrive";
	pchar.quest.GWIK_JamConvoyArrive.function = "GWIK_JamaicaConvoyArrive";
	GWIK_JamaicaSpawnPursuer(); // 2 корвета конкурентов вдогонку
	pchar.GWIK.Jamaica.Convoy = "escort";
	DeleteAttribute(pchar, "quest.GWIK_JamaicaConvoyTimeout.over");
	SetTimerFunction("GWIK_JamaicaConvoyTimeout", 0, 1, 0); // месяц - подстраховка от зависания
	return true;
}

void GWIK_JamaicaConvoyFail(string sRecord) // конвой потерян - этап 3 провален, можно перезапросить конвой в Кюрасао
{
	GWIK_JamaicaConvoyCleanup(true);
	DeleteAttribute(pchar, "GWIK.Jamaica.Convoy");
	GWIK_AddRep(-5); // Компания недовольна потерей груза (клампится 0)
	AddQuestRecord("GWIK_JamaicaJob", sRecord);
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Конвой Компании потерян.");
		log_info("Загляните в контору Кюрасао - снарядят новый.");
	}
	else
	{
		log_info("The Company convoy is lost.");
		log_info("See the Curacao office - they will fit out another.");
	}
}

void GWIK_JamaicaConvoyArrive(string qName) // конвой дошёл до Форт-Оранжа - снабжение доставлено
{
	if (!CheckAttribute(pchar, "GWIK.Jamaica.Convoy")) return;
	ref c1 = CharacterFromID("GWIK_JamConvoy1");
	ref c2 = CharacterFromID("GWIK_JamConvoy2");
	int alive = 0;
	if (IsCompanion(c1)) alive++;
	if (IsCompanion(c2)) alive++;
	if (alive == 0) { GWIK_JamaicaConvoyFail("17"); return; } // оба потеряны - провал
	GWIK_JamaicaConvoyCleanup(true); // уцелевшие галеоны швартуются у конторы (выходят из эскадры)
	DeleteAttribute(pchar, "GWIK.Jamaica.Convoy");
	pchar.GWIK.Jamaica.ConvoyArrived = alive; // сколько галеонов дошло (1 или 2) - для реплики эмиссара
	AddQuestRecord("GWIK_JamaicaJob", "16");
	if (LanguageGetLanguage() == "russian") log_info("Конвой доведён до Форт-Оранжа. Загляните к эмиссару Компании.");
	else log_info("The convoy has reached Fort Orange. See the Company envoy.");
}

void GWIK_JamConvoyShipDead(string qName) // один из галеонов потоплен: оба - провал, один уцелел - продолжаем
{
	if (!CheckAttribute(pchar, "GWIK.Jamaica.Convoy")) return;
	ref c1 = CharacterFromID("GWIK_JamConvoy1");
	ref c2 = CharacterFromID("GWIK_JamConvoy2");
	if (!IsCompanion(c1) && !IsCompanion(c2)) { GWIK_JamaicaConvoyFail("17"); return; } // оба погибли
	AddQuestRecord("GWIK_JamaicaJob", "18"); // один галеон потерян - доведите второй
	if (LanguageGetLanguage() == "russian") log_info("Один из галеонов конвоя потоплен. Доведите второй до Форт-Оранжа.");
	else log_info("One of the convoy galleons is sunk. Get the other one to Fort Orange.");
}

void GWIK_JamaicaConvoyTimeout(string qName) // подстраховка: конвой не доведён за месяц - провал
{
	if (!CheckAttribute(pchar, "GWIK.Jamaica.Convoy")) return;
	GWIK_JamaicaConvoyFail("17");
}

bool GWIK_JamaicaSpawnFinal() // этап 5 (путь силы): английский приватир пытается сорвать открытие конторы
{
	string sAreal = GetArealByCityName("FortOrange");
	// Gregg: приватир - строго на Quest_ship_7 (спот подобран вручную);
	// пикер вернул бы ПЕРВЫЙ Quest-локатор острова - это бухта Портленд, туда не надо
	string sLocator = "";
	int iIsl = FindIsland(sAreal);
	if (iIsl >= 0 && CheckAttribute(&Islands[iIsl], "Quest_ships.Quest_ship_7")) sLocator = "Quest_ship_7";
	if (sLocator == "") sLocator = GWIK_PickQuestShipLocator(sAreal); // фолбэк, если локатора нет в модели
	if (sLocator == "") return false;
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_JamFinal", "", "man", "man", sti(pchar.rank) + 2, ENGLAND, -1, true));
	sld.id = sld.id + "_" + sld.index;
	SetCaptanModelByEncType(sld, "trade");
	DeleteAttribute(sld, "Ship");
	SetShipToFantom(sld, "trade", true);
	sld.Ship.Type = GenerateShipExt(SHIP_CORVETTE, true, sld); // Gregg: приватир - корвет (а не случайный галеон)
	SetBaseShipData(sld);
	SetCrewQuantityFull(sld);
	Fantom_SetCannons(sld, "war"); // Gregg: калибр под фактический корвет (иначе остаётся от случайного корабля SetShipToFantom)
	Fantom_SetBalls(sld, "war"); // Gregg: и боезапас/порох под фактический корпус (тем же корнем был занижен)
	// Gregg: личность приватира - о нём говорят эмиссар и управляющий («капер Джеффри Гейл на 'Гончей'»)
	if (LanguageGetLanguage() == "russian") { sld.name = "Джеффри"; sld.lastname = "Гейл"; sld.Ship.Name = "Гончая"; }
	else { sld.name = "Jeffrey"; sld.lastname = "Gale"; sld.Ship.Name = "Greyhound"; }
	sld.Ship.Mode = "War";
	sld.AlwaysSandbankManeuver = true;
	sld.DontRansackCaptain = true;
	sld.AlwaysEnemy = true; // Gregg: агрессивен - пытается сорвать открытие конторы
	Group_FindOrCreateGroup("GWIK_JamFinalGroup");
	Group_SetType("GWIK_JamFinalGroup", "war");
	Group_AddCharacter("GWIK_JamFinalGroup", sld.id);
	Group_SetGroupCommander("GWIK_JamFinalGroup", sld.id);
	Group_SetAddress("GWIK_JamFinalGroup", sAreal, "Quest_ships", sLocator);
	Group_LockTask("GWIK_JamFinalGroup");
	pchar.GWIK.Jamaica.FinalId = sld.id;
	pchar.quest.GWIK_JamFinalDead.win_condition.l1 = "NPC_Death";
	pchar.quest.GWIK_JamFinalDead.win_condition.l1.character = sld.id;
	pchar.quest.GWIK_JamFinalDead.win_condition = "GWIK_JamFinalDead";
	pchar.quest.GWIK_JamFinalDead.function = "GWIK_JamFinalDead";
	return true;
}

void GWIK_JamFinalDead(string qName) // приватир потоплен - вернуться к эмиссару открывать контору
{
	if (!CheckAttribute(pchar, "GWIK.Jamaica.Stage")) return;
	if (sti(pchar.GWIK.Jamaica.Stage) != 5) return;
	pchar.GWIK.Jamaica.FinalDone = true;
	Group_DeleteGroup("GWIK_JamFinalGroup");
	AddQuestRecord("GWIK_JamaicaJob", "9");
	if (LanguageGetLanguage() == "russian") log_info("Английский приватир потоплен. Возвращайтесь к эмиссару - пора открывать контору.");
	else log_info("The English privateer is sunk. Return to the envoy - time to open the office.");
}

void GWIK_FinishJamaicaQuest(int iEnglandHit) // финал: открыть контору, наградить, прибрать временные NPC
{
	// эмиссара НЕ трогаем здесь: финал зовётся из его же диалога, убивать собеседника на месте нельзя. Остаётся в ратуше, его реплика после открытия укажет на постоянного представителя.
	GWIK_OpenFortOrangeOffice();          // дверь + представитель FortOrange_GWIK_char + флаг Jamaica.Done + сброс кэша быстрого перехода
	if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) // контору открыть не удалось (нет локации) - НЕ разрушаем квест и НЕ платим, иначе награду можно фармить
	{
		if (LanguageGetLanguage() == "russian") log_info("Не удалось открыть контору в Форт-Оранже. Загляните к эмиссару чуть позже.");
		else log_info("The Fort Orange office could not be opened. See the envoy again a little later.");
		return;
	}
	int fi = -1;
	if (CheckAttribute(pchar, "GWIK.Jamaica.FactorId")) fi = GetCharacterIndex(pchar.GWIK.Jamaica.FactorId);
	if (fi != -1) // фактор устроился в новой конторе
	{
		RemovePassenger(pchar, &Characters[fi]);
		Characters[fi].location = "";
		LAi_SetCurHP(&Characters[fi], 0.0);
	}
	GWIK_JamaicaClearAmbush();            // страховка: если от перехватчика оторвались и не убрали на сдаче фактора
	pchar.GWIK.Jamaica.Founder = true;    // +1% к дивидендам как соучредителю
	int iReward = 120000 + sti(pchar.rank) * 3000;
	AddMoneyToCharacter(pchar, iReward);
	GWIK_AddRep(65);
	AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 300); // Gregg: капстоун линии Ямайки - венец за открытие конторы (все пути), крупный опыт лидерства
	AddCharacterExpToSkill(pchar, SKILL_COMMERCE, 200);   // Gregg: и торговли - основание торгового представительства Компании
	if (iEnglandHit > 0) ChangeCharacterNationReputation(pchar, ENGLAND, -iEnglandHit); // путь силы - англичане в обиде
	PlayStereoSound("notebook");
	AddQuestRecord("GWIK_JamaicaJob", "10");
	AddQuestUserData("GWIK_JamaicaJob", "sSex", GetSexPhrase("", "а")); // форма по полу для «стал@<sSex>» (строго после AddQuestRecord)
	CloseQuestHeader("GWIK_JamaicaJob");
	SetTimerFunction("GWIK_FoundersGift", 0, 0, 7); // через неделю - письмо Семнадцати Господ (разовое)
	// чистка временных флагов квеста (Founder и Done оставляем; Tone тоже - от него зависит приветствие управляющего)
	DeleteAttribute(pchar, "GWIK.Jamaica.Stage");
	DeleteAttribute(pchar, "GWIK.Jamaica.Offered");
	DeleteAttribute(pchar, "GWIK.Jamaica.CourierId");
	DeleteAttribute(pchar, "GWIK.Jamaica.FactorId");
	DeleteAttribute(pchar, "GWIK.Jamaica.FinalId");
	DeleteAttribute(pchar, "GWIK.Jamaica.FactorAboard");
	DeleteAttribute(pchar, "GWIK.Jamaica.FinalDone");
	DeleteAttribute(pchar, "GWIK.Jamaica.FactorFName"); // имя и модель уже перенесены на управляющего
	DeleteAttribute(pchar, "GWIK.Jamaica.FactorLName");
	DeleteAttribute(pchar, "GWIK.Jamaica.FactorModel");
	DeleteAttribute(pchar, "GWIK.Jamaica.MayorOk");
	DeleteAttribute(pchar, "GWIK.Jamaica.Convoy");        // Gregg: страховка - в норме уже снят на этапе 3
	DeleteAttribute(pchar, "GWIK.Jamaica.ConvoyArrived");
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Контора Компании в Форт-Оранже открыта!");
		log_info("Награда - " + FindRussianMoneyString(iReward) + ".");
	}
	else
	{
		log_info("The Company office in Fort Orange is open!");
		log_info("Reward: " + FindRussianMoneyString(iReward) + ".");
	}
}

void GWIK_FoundersGift(string qName) // Gregg: через неделю после открытия - письмо Семнадцати Господ с дарами (разовый таймер из финала)
{
	if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) return;
	GWIK_AddRep(15);
	string sPrint = "Ship_Print_3";                    // дар Компании: чертёж корабля 3 класса...
	if (sti(pchar.rank) > 19) sPrint = "Ship_Print_2"; // ...а бывалым капитанам - 2 класса
	GiveItem2Character(pchar, sPrint);
	GiveItem2Character(pchar, "sculMa2");              // розовый хрустальный череп
	PlayStereoSound("notebook");
	ReOpenQuestHeader("GWIK_JamaicaJob"); // письмо ложится в судовой журнал отдельной записью
	AddQuestRecord("GWIK_JamaicaJob", "14");
	CloseQuestHeader("GWIK_JamaicaJob");
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Курьер доставил письмо Семнадцати Господ.");
		log_info("Дары Компании: чертёж корабля и розовый хрустальный череп.");
	}
	else
	{
		log_info("A courier has delivered a letter from the Heeren XVII.");
		log_info("The Company's gifts: a ship blueprint and a pink crystal skull.");
	}
}

int GWIK_TrophyPrice(ref rComp) // Gregg: цена выкупа Компанией трофейного корабля эскадры - до 40% базовой цены, растёт с репутацией и состоянием судна
{
	ref rShip = GetRealShip(sti(rComp.Ship.Type)); // Storm не даёт GetRealShip(...).Price инлайн - сначала в ref (см. companions.c:48)
	int basePrice = sti(rShip.Price);
	int pct = 25;                       // rep<2 (на узел и так пускаем от rep>=2 - это страховка)
	int rl = GWIK_RepLevel();
	if (rl == 2) pct = 30;
	if (rl == 3) pct = 35;
	if (rl >= 4) pct = 40;
	int state = makeint((GetHullPercent(rComp) + GetSailPercent(rComp)) / 2.0); // среднее состояние корпуса/парусов, 0..100
	int price = makeint(basePrice * pct / 100 * state / 100); // порядок делений - против переполнения int на дорогих судах
	if (price < 100) price = 100;
	return price;
}

// Gregg: продажа трофейного корабля эскадры Компании без потерь. Переиспользуем порт логики верфи из ShipsUtilites.c
// (сами SendCrewTo* в shipyard.c интерфейс-локальны и отсюда недоступны). Порядок действий - как в PortAuction_SubmitShip.
bool GWIK_SellTrophyShip(ref rShip)
{
	if (SeaTowOperations_Block(rShip)) return false;
	PortAuction_RecoverShipAssets(rShip, "Max"); // груз и пушки -> лавка колонии (контрабанда в трюм ГГ); команда -> ГГ и компаньоны до максимума, избыток -> таверна
	rShip.ship.type = SHIP_NOTUSED;
	RemoveCharacterCompanion(pchar, rShip);
	AddPassenger(pchar, rShip, false);           // призовой капитан-офицер уходит пассажиром, а не гибнет в пустоту
	ReturnAutoOfficers();                        // если экс-капитан занимал должности до капитанства - вернуть его на них
	PortStock_RefreshSkills(rShip);              // сбросить капитанские бонусы навыков (аналог DelBakSkill на верфи)
	Statistic_AddValue(pchar, "SellShip", 1);    // LEO: выкуп Компанией - та же продажа, что на верфи и аукционе ПУ
	return true;
}

void GWIK_CheckContract(ref rGwik) // ротация закупочного контракта конторы
{
	bool bNeed = !CheckAttribute(rGwik, "GWIK.Contract");
	if (!bNeed)
	{
		if (sti(rGwik.GWIK.Contract.Qty) <= 0)
		{
			if (GetNpcQuestPastDayParam(rGwik, "GWIK.ContractDate") >= 3) bNeed = true; // выполненный - новый через 3 дня
		}
		else
		{
			if (GetNpcQuestPastDayParam(rGwik, "GWIK.ContractDate") >= 7) bNeed = true; // невыкупленный - через 7 дней
		}
	}
	if (!bNeed) return;
	int iGood = 10 + drand(23); // колониальные товары: GOOD_RUM..GOOD_TOOLS (drand - дневной детерминированный рандом, неуязвим к F9; свой поток, чтобы не совпадал с товаром аукциона)
	rGwik.GWIK.Contract.Good = iGood;
	rGwik.GWIK.Contract.Qty = makeint((5 + drand2(15)) * 20 * GWIK_VolumeMult() / 100); // базово 100..400 ед., масштаб от уровня ГГ и пая
	// Gregg: ставка контракта зависит от уровня пая - без пая почти по рынку, привилегия растёт со статусом
	int cBase = 115; int cSpread = 5;          // без пая:   115..120% Cost
	int sLvl = 0;
	if (CheckAttribute(pchar, "GWIK.Share.Level")) sLvl = sti(pchar.GWIK.Share.Level);
	if (sLvl == 1) { cBase = 120; cSpread = 20; } // Пайщик:    120..140%
	if (sLvl == 2) { cBase = 130; cSpread = 20; } // Партнёр:   130..150%
	if (sLvl >= 3) { cBase = 140; cSpread = 20; } // Компаньон: 140..160%
	rGwik.GWIK.Contract.Price = makeint(sti(Goods[iGood].Cost) * (cBase + drand3(cSpread)) / 100);
	SaveCurrentNpcQuestDateParam(rGwik, "GWIK.ContractDate");
}

// LEO: деньги за партию по-магазинному. Цена товара (и Goods[].Cost, и цены ГВИК) - за ПАЧКУ
//   в Goods[].Units штук, а количество везде считается в штуках: лавка платит price*qty/Units
//   (store.c). ГВИК же брал цену пачки за каждую штуку - конфискат выходил в разы дороже лавки
//   (одежда по 18 за штуку против 30 за десяток), а по контракту Компания столько же переплачивала.
int GWIK_GoodsMoney(int iGood, int iQty, int iPrice)
{
	int iUnits;
	int iMoney;
	if (iQty <= 0) return 0;
	iUnits = sti(Goods[iGood].Units);
	if (iUnits < 1) iUnits = 1;
	iMoney = makeint(makefloat(iPrice) * iQty / iUnits + 0.5);
	if (iMoney < 1) iMoney = 1; // как в лавке: даром не отдаём
	return iMoney;
}

// LEO: сколько ШТУК товара потянем на заданные деньги при цене за пачку
int GWIK_GoodsAffordable(int iGood, int iMoney, int iPrice)
{
	int iUnits;
	if (iPrice < 1) return 0;
	iUnits = sti(Goods[iGood].Units);
	if (iUnits < 1) iUnits = 1;
	return makeint(makefloat(iMoney) * iUnits / iPrice);
}

bool GWIK_IsAuctionItem(int i) // годится ли предмет в лот конфиската: трубы и карты
{
	if (!CheckAttribute(&Items[i], "id")) return false;
	if (!CheckAttribute(&Items[i], "groupID")) return false;
	if (Items[i].id == "map_LSC") return false; // квестовая карта
	if (Items[i].id == "map_full") return false;  // Gregg: эксклюзив компаньонов ГВИК (особый лот, не в общем пуле)
	if (Items[i].id == "spyglass5") return false; // Gregg: эксклюзив компаньонов ГВИК (особый лот, не в общем пуле)
	if (Items[i].groupID == SPYGLASS_ITEM_TYPE) return (Items[i].id != "spyglass1");
	if (Items[i].groupID == MAPS_ITEM_TYPE)
	{
		if (!CheckAttribute(&Items[i], "ItemType")) return false;
		if (Items[i].ItemType != "MAP") return false;
		if (!CheckAttribute(&Items[i], "price")) return false;
		return (sti(Items[i].price) > 0);
	}
	return false;
}

int GWIK_LoadGoodsSquadron(ref chr, int iGood, int iQty) // грузит товар: сначала трюм ГГ, остаток - по своим кораблям эскадры; возвращает фактически погруженное количество
{
	int loaded = AddCharacterGoodsSimple(chr, iGood, iQty);
	int i, cn;
	ref chref;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(chr, i);
		if (cn != -1 && loaded < iQty && GetRemovable(&Characters[cn])) // грузим токо своим
		{
			chref = GetCharacter(cn);
			loaded += AddCharacterGoodsSimple(chref, iGood, iQty - loaded);
		}
	}
	return loaded;
}

int GWIK_RemoveGoodsSquadron(ref chr, int iGood, int iQty) // Gregg: снимает товар - сначала с флагмана, остаток с кораблей эскадры (как считает GetSquadronGoods); возвращает фактически снятое
{
	int removed = 0;
	int have = GetCargoGoods(chr, iGood);
	int take = iQty;
	if (take > have) take = have;
	if (take > 0) { RemoveCharacterGoodsSelf(chr, iGood, take); removed = removed + take; }
	int i, cn, h2, t2;
	ref chref;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(chr, i);
		if (cn != -1 && removed < iQty && GetRemovable(&Characters[cn])) // токо свои корабли
		{
			chref = GetCharacter(cn);
			if (GetShipRemovableEx(chref))
			{
				h2 = GetCargoGoods(chref, iGood);
				t2 = iQty - removed;
				if (t2 > h2) t2 = h2;
				if (t2 > 0) { RemoveCharacterGoodsSelf(chref, iGood, t2); removed = removed + t2; }
			}
		}
	}
	return removed;
}

string GWIK_PickAuctionItem()
{
	int i, iCount;
	iCount = 0;
	for (i = 0; i < ITEMS_QUANTITY; i++)
	{
		if (GWIK_IsAuctionItem(i)) iCount++;
	}
	if (iCount == 0) return "";
	int iPick = drand1(iCount - 1);
	for (i = 0; i < ITEMS_QUANTITY; i++)
	{
		if (GWIK_IsAuctionItem(i))
		{
			if (iPick == 0) return Items[i].id;
			iPick--;
		}
	}
	return "";
}

void GWIK_MakeAuctionExtra(ref rGwik, bool bItem) // второй лот конфиската для пайщиков (Партнёр и выше), тип - противоположный основному
{
	string sItem = "";
	if (bItem) sItem = GWIK_PickAuctionItem();
	if (sItem != "")
	{
		ref rItem = ItemsFromID(sItem);
		rGwik.GWIK.AuctionExtra = "item";
		rGwik.GWIK.AuctionExtra.Item = sItem;
		rGwik.GWIK.AuctionExtra.Price = makeint(sti(rItem.price) * (55 + drand3(15)) / 100);
	}
	else
	{
		int iGood = 10 + drand1(23);
		rGwik.GWIK.AuctionExtra = "goods";
		rGwik.GWIK.AuctionExtra.Good = iGood;
		rGwik.GWIK.AuctionExtra.Qty = makeint((10 + drand2(20)) * 10 * GWIK_VolumeMult() / 100);
		rGwik.GWIK.AuctionExtra.Price = makeint(sti(Goods[iGood].Cost) * (55 + drand3(15)) / 100);
	}
}

void GWIK_CheckAuction(ref rGwik) // ротация лота конфиската конторы
{
	int shareLvl = 0;
	if (CheckAttribute(pchar, "GWIK.Share.Level")) shareLvl = sti(pchar.GWIK.Share.Level);
	bool bExtraItem;

	bool bNeed = !CheckAttribute(rGwik, "GWIK.Auction");
	if (!bNeed && GetNpcQuestPastDayParam(rGwik, "GWIK.AuctionDate") >= 7) bNeed = true;
	if (bNeed)
	{
		DeleteAttribute(rGwik, "GWIK.Auction");
		DeleteAttribute(rGwik, "GWIK.AuctionExtra"); // второй лот ротируется вместе с основным

		string sItem = "";
		if (drand(2) == 0) sItem = GWIK_PickAuctionItem(); // примерно треть лотов - приборы и карты (drand - дневной рандом: одинаков при F9 в тот же день)
		if (sItem != "")
		{
			ref rItem = ItemsFromID(sItem);
			rGwik.GWIK.Auction = "item";
			rGwik.GWIK.Auction.Item = sItem;
			rGwik.GWIK.Auction.Price = makeint(sti(rItem.price) * (55 + drand3(15)) / 100); // 55..70% цены
		}
		else
		{
			int iGood = 10 + drand1(23);
			rGwik.GWIK.Auction = "goods";
			rGwik.GWIK.Auction.Good = iGood;
			rGwik.GWIK.Auction.Qty = makeint((10 + drand2(20)) * 10 * GWIK_VolumeMult() / 100); // базово 100..300 ед., масштаб от уровня ГГ и пая
			rGwik.GWIK.Auction.Price = makeint(sti(Goods[iGood].Cost) * (55 + drand3(15)) / 100);
		}
		// пайщикам от Партнёра - сразу второй лот противоположного типа
		if (shareLvl >= 2)
		{
			bExtraItem = (sItem == ""); // основной - товар -> доп. предмет, и наоборот
			GWIK_MakeAuctionExtra(rGwik, bExtraItem);
		}
		SaveCurrentNpcQuestDateParam(rGwik, "GWIK.AuctionDate");
		return;
	}
	// пай повышен в середине цикла (или старый сейв) - второй лот ещё не создан
	if (shareLvl >= 2 && !CheckAttribute(rGwik, "GWIK.AuctionExtra"))
	{
		bExtraItem = (rGwik.GWIK.Auction != "item");
		GWIK_MakeAuctionExtra(rGwik, bExtraItem);
	}
}
// поручения контор ГВИК -->
string GWIK_PickQuestShipLocator(string sAreal) // локатор Quest_ships у острова
{
	int islandIdx = FindIsland(sAreal);
	if (islandIdx < 0) return "";
	if (!CheckAttribute(&Islands[islandIdx], "Quest_ships")) return "";
	aref arQuestLoc, arLocator;
	makearef(arQuestLoc, Islands[islandIdx].Quest_ships);
	int iTemp = GetAttributesNum(arQuestLoc);
	string locatorName;
	for (int i = 0; i < iTemp; i++)
	{
		arLocator = GetAttributeN(arQuestLoc, i);
		locatorName = GetAttributeName(arLocator);
		if (findsubstr(locatorName, "Quest", 0) != -1) return locatorName;
	}
	return "";
}

string GWIK_RivalShipStr() // Gregg: корабль конкурента для журнала/диалога - «тип 'Имя'»; старый сейв без имени -> только тип
{
	string s = "";
	if (CheckAttribute(pchar, "GWIK.Job.ShipType")) s = GetStrSmallRegister(pchar.GWIK.Job.ShipType);
	if (CheckAttribute(pchar, "GWIK.Job.ShipName") && pchar.GWIK.Job.ShipName != "") s = s + " '" + pchar.GWIK.Job.ShipName + "'";
	return s;
}

string GWIK_JobPlaceStr() // Gregg: место конкурента для диалога - «в водах <города>, что на <острове>». Было «у берегов <города>»: берега бывают у ОСТРОВА, а не у города
{
	if (!CheckAttribute(pchar, "GWIK.Job.City")) return "";
	string sCity = pchar.GWIK.Job.City;
	string sIsland = GetIslandByCityName(sCity);
	string sAreal = "";
	// остров не опознан или называется как сам город (Тортуга) - без уточнения, иначе «в водах Тортуги, что на Тортуге»
	if (sIsland != "" && sIsland != sCity) sAreal = strreplace(iTxt("GWIK_JobPlaceAreal"), "#island#", XI_ConvertString(sIsland + "Voc"));
	// Gregg: морской конкурент первые двое суток догружается в порту (GWIK_RivalPutToSea) - и в конторе о нём говорят соответственно,
	// иначе игрока посылали «в воды» за судном, которого там ещё нет
	if (CheckAttribute(pchar, "GWIK.Job.RivalInPort"))
	{
		return iTxt("GWIK_JobPlacePort") + XI_ConvertString("Colony" + sCity + "Gen") + sAreal + iTxt("GWIK_JobPlacePortTail");
	}
	return iTxt("GWIK_JobPlace") + XI_ConvertString("Colony" + sCity + "Gen") + sAreal;
}

string GWIK_PickPatrolPartner(string sCity) // Gregg: ближайшая ДРУГАЯ англо/франц. колония к sCity - второй конец морского патруля (открытый коридор между двумя портами, а не круги у одного)
{
	int best = -1;
	int bestDist = 2000000000;
	int n;
	int d;
	for (n = 0; n < MAX_COLONIES; n++)
	{
		if (colonies[n].id == sCity) continue;
		if (colonies[n].nation == "none") continue;
		if (colonies[n].id == "Panama") continue;
		if (sti(colonies[n].nation) != ENGLAND && sti(colonies[n].nation) != FRANCE) continue;
		d = GetMaxDaysFromIsland2Island(GetArealByCityName(sCity), GetArealByCityName(colonies[n].id)); // дистанция-прокси (город->ареал, как в движке), -1 если нет
		if (d >= 0 && d < bestDist) { bestDist = d; best = n; }
	}
	if (best < 0) return "";
	return colonies[best].id;
}

void GWIK_RivalDispatchLeg() // Gregg: очередной отрезок ПАТРУЛЯ морского конкурента - открытый коридор между его колонией и ближайшей союзной, направление чередуется (перехват в открытом море посередине). Вызов при спавне и при каждом «прибытии», пока не убит/не месячный таймаут. НЕ односторонний рейс - иначе «доходил» и проваливал раньше, чем игрок доплывёт
{
	if (!CheckAttribute(pchar, "GWIK.Job.City")) return;
	if (!CheckAttribute(pchar, "GWIK.Job.RivalId")) return;
	string cA = pchar.GWIK.Job.City;
	string cB = cA;
	if (CheckAttribute(pchar, "GWIK.Job.RivalCity2") && pchar.GWIK.Job.RivalCity2 != "") cB = pchar.GWIK.Job.RivalCity2;
	string fromCity = cA;
	string toCity = cB;
	if (CheckAttribute(pchar, "GWIK.Job.RivalLeg") && sti(pchar.GWIK.Job.RivalLeg) == 1) { fromCity = cB; toCity = cA; pchar.GWIK.Job.RivalLeg = 0; } // чередуем A->B / B->A
	else pchar.GWIK.Job.RivalLeg = 1;
	string sShore = GetIslandRandomShoreId(GetArealByCityName(fromCity));
	if (sShore == "") sShore = GetIslandRandomShoreId(GetArealByCityName(cA)); // фолбэк на берег своей колонии
	if (sShore == "") return; // без берега маршрута нет - разрулит месячный таймаут GWIK_RivalGone
	int days = GetMaxDaysFromIsland2Island(GetArealByCityName(fromCity), GetArealByCityName(toCity)) + 3;
	if (days < 6) days = 6; // отрезок минимум ~6 дней, чтобы конкурент шёл открытым морем, а не наматывал у порта
	Map_CreateTrader(sShore, toCity, pchar.GWIK.Job.RivalId, days);
}

string GWIK_PickLandRivalCity() // Gregg: колония для НАЗЕМНОГО варианта. Свой подбор, а не findRivalCity_GWIK: тому годятся только англо-французские
// колонии, и при войне с Англией и Францией (обычное дело) наземный вариант не выдавался НИКОГДА - молча откатывался в море.
// Требования: в город можно войти (не война), есть таверна и окраина под дуэль, не текущий остров. Сначала ищем конкурентов
// у англичан/французов (флавор), если таких нет - годится любая невраждебная неписьменная колония: агент торгового дома
// сидит там, где ему удобно, нация колонии его не определяет
{
	int n;
	int cityArray[MAX_COLONIES];
	int howCity = 0;
	int iNat = 0;
	int iPass = 0;
	string sSelfIsland = "";
	string sTown = GetCurrentTown();
	if (sTown != "") sSelfIsland = GetIslandByCityName(sTown);
	for (iPass = 0; iPass < 2; iPass++) // проход 0 - только Англия/Франция, проход 1 - любая нация кроме пиратов
	{
		howCity = 0;
		for (n = 0; n < MAX_COLONIES; n++)
		{
			if (colonies[n].id == "Panama") continue;
			if (colonies[n].nation == "none") continue;
			if (sSelfIsland != "" && colonies[n].islandLable == sSelfIsland) continue; // не на том же острове, где игрок сейчас
			iNat = sti(colonies[n].nation);
			if (iNat == PIRATE) continue;
			if (GetNationRelation2MainCharacter(iNat) == RELATION_ENEMY) continue; // война - в город не войти, поручение стало бы невыполнимым
			if (iPass == 0)
			{
				if (iNat != ENGLAND && iNat != FRANCE) continue;
			}
			if (FindLocation(colonies[n].id + "_tavern") < 0) continue;   // сидеть негде
			if (FindLocation(colonies[n].id + "_ExitTown") < 0) continue; // дуэли за воротами быть негде
			cityArray[howCity] = n;
			howCity++;
		}
		if (howCity > 0) return colonies[cityArray[cRand(howCity-1)]].id;
	}
	return "";
}

bool GWIK_SpawnRivalLand(string sCity) // Gregg: НАЗЕМНЫЙ вариант - представитель конкурентов в море не выходит, сидит в таверне колонии. Убийство целиком на штатном дуэльном фреймворке (DIALOGS\<язык>\Common_Duel.c + scripts\duel.c): оскорбить -> вызвать -> за городские ворота либо тут же в таверне
{
	string sTavern = sCity + "_tavern";
	if (FindLocation(sTavern) < 0) return false; // нет таверны - вариант не годится, вызывающий откатится на морской
	if (FindLocation(sCity + "_ExitTown") < 0) return false; // нет окраины - дуэль за воротами была бы тихо недоступна, а это молчаливый тупик. Тоже откат в море
	int iNation = GetCityNation(sCity); // цель - местный житель: та же нация, что у колонии (и городская группа совпадёт)
	if (iNation == PIRATE) return false;
	if (GetNationRelation2MainCharacter(iNation) == RELATION_ENEMY) return false; // Gregg: с этой нацией война - в город не войти, поручение стало бы невыполнимым. Откат в море
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_RivalMan", "", "man", "man", sti(pchar.rank), iNation, -1, false));
	sld.id = sld.id + "_" + sld.index; // уникальный id, чтобы мёртвые дубли не путались под ногами
	SetCaptanModelByEncType(sld, "trade"); // человек торговой компании, а не пират
	SetRandomNameToCharacter(sld);
	DeleteAttribute(sld, "Ship"); // в море не выходит - корабля у него нет (и морская дуэль сама собой не предложится)
	SetFantomParamHunter(sld); // навыки + клинок/пистолет по рангу + HP: та же идиома, которой дуэльный фреймворк вооружает бойцов
	sld.City = sCity;          // БЕЗ CityType - иначе fight_off_town_prep начнёт крутить ему LifeDay как городскому фантому
	sld.Dialog.Filename = "GWIK_RivalAgent.c";
	LAi_SetLoginTime(sld, 0.0, 24.0); // в таверне круглые сутки
	pchar.GWIK.Job.RivalId = sld.id;
	pchar.GWIK.Job.RivalLand = true;
	pchar.GWIK.Job.City = sCity;
	pchar.GWIK.Job.RivalName = GetFullName(sld);
	pchar.GWIK.Job.RivalTavern = sTavern;

	// Gregg: на руках у него грамота ГВИК - месячная универсальная лицензия. Мотив поручения (перебивает Компании цены по ЕЁ же
	// лицензии) и приз тому, кто обыщет тело. SaveItemsForDead - иначе движок вещи с трупа снимет.
	// ВАЖНО: HolTradeLicence - ОДИН предмет на весь мод, срок хранится в атрибутах ШАБЛОНА предмета (Action_date/Validity),
	// а не у владельца. Поэтому срок и флаг универсальности выставляем ТОЛЬКО если у игрока своей грамоты нет:
	// иначе затёрли бы срок его собственной лицензии, а флаг подарил бы ей универсальность даром.
	// Флаг сам по себе безвреден: IsUniLicenceActive требует И флаг, И предмет на руках, а покупка обычной
	// голландской грамоты у губернатора этот флаг штатно снимает (GiveNationLicence, ветка HOLLAND)
	sld.SaveItemsForDead = true;
	GiveItem2Character(sld, "HolTradeLicence");
	if (!CheckCharacterItem(pchar, "HolTradeLicence"))
	{
		ref rLic = ItemsFromID("HolTradeLicence");
		SaveCurrentNpcQuestDateParam(rLic, "Action_date"); // отсчёт срока - со дня выдачи поручения (месяц, как и таймаут самого поручения)
		rLic.Action_date = GetCurrentDate();
		rLic.Validity = FindRussianDaysString(30); // строка для описания предмета
		rLic.Validity.QtyDays = 30;                // дни для расчётов
		rLic.Days_left = 30;
		rLic.UniNote = GetUniLicenceNote(true);    // в описании - пометка «универсальная» (плейсхолдер #sUniNote#)
		pchar.UniLicence = true;
		pchar.GWIK.Job.RivalLicence = true;        // для реплики конторы: намекнуть, что бумаги стоит обыскать
	}

	// Gregg: сброс маркеров прошлого rival-поручения (иначе перевзвод прерываний мёртв - over="yes" остаётся)
	DeleteAttribute(pchar, "quest.GWIK_RivalDead.over");
	DeleteAttribute(pchar, "quest.GWIK_RivalGone.over");
	DeleteAttribute(pchar, "quest.GWIK_RivalLand.over");
	DeleteAttribute(pchar, "quest.GWIK_RivalDuelStart.over");

	// прерывание на смерть цели - то же самое, что у морских вариантов (NPC_Death = CharacterIsDead, работает и на суше)
	pchar.quest.GWIK_RivalDead.win_condition.l1 = "NPC_Death";
	pchar.quest.GWIK_RivalDead.win_condition.l1.character = sld.id;
	pchar.quest.GWIK_RivalDead.win_condition = "GWIK_RivalDead";
	pchar.quest.GWIK_RivalDead.function = "GWIK_RivalDead";

	// сажаем его ЛЕНИВО, на входе игрока в таверну: сит-локаторы у каждой таверны свои (sit2/sit3/sit11...), гвоздями брать нельзя - плохой локатор движок карает удалением персонажа
	pchar.quest.GWIK_RivalLand.win_condition.l1 = "Location";
	pchar.quest.GWIK_RivalLand.win_condition.l1.location = sTavern;
	pchar.quest.GWIK_RivalLand.win_condition = "GWIK_RivalLandArrive";
	pchar.quest.GWIK_RivalLand.function = "GWIK_RivalLandArrive";

	SetTimerFunction("GWIK_RivalGone", 0, 1, 0); // таймаут-бэкстоп: через месяц конкурент уедет
	return true;
}

void GWIK_RivalLandArrive(string qName) // Gregg: игрок вошёл в таверну - сажаем человека конкурентов на ближайшее свободное место за столом (паттерн агента почты: локатор ищем в загруженной локации, а не угадываем)
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "rival") return;
	if (!CheckAttribute(pchar, "GWIK.Job.RivalLand")) return;
	if (CheckAttribute(pchar, "GWIK.Job.Done")) return;
	if (!CheckAttribute(pchar, "GWIK.Job.RivalId")) return;
	int i = GetCharacterIndex(pchar.GWIK.Job.RivalId);
	if (i == -1) return;
	ref sld = GetCharacter(i);
	if (CharacterIsDead(sld)) return;
	// локатор ищем САМИ, а не через PlaceCharacter: та при отсутствии свободного места зовёт ChangeCharacterAddressGroup с ПУСТЫМ локатором, а за это движок удаляет персонажа («error teleportation»)
	string sGrp2 = "sit";
	string sLoc = LAi_FindNearestFreeLocator2Pchar("sit"); // сит-локаторы у каждой таверны свои (sit2/sit3/sit11...) - берём свободный в уже загруженной локации
	if (sLoc == "")
	{
		sGrp2 = "goto"; // все места за столами заняты - пусть стоит
		sLoc = LAi_FindNearestFreeLocator2Pchar("goto");
	}
	if (sLoc == "") return; // совсем некуда - over не ставим, посадим при следующем входе в таверну
	ChangeCharacterAddressGroup(sld, pchar.location, sGrp2, sLoc);
	if (sGrp2 == "sit") LAi_SetSitTypeNoGroup(sld);
	else LAi_SetOwnerType(sld); // стоячий «хозяин»: стоит и разговаривает (актёр без команды неинтерактивен)
	string sGrp = "ENGLAND_CITIZENS"; // группа по нации (колония могла и сменить флаг, пока игрок плыл)
	if (sti(sld.Nation) == FRANCE) sGrp = "FRANCE_CITIZENS";
	if (sti(sld.Nation) == HOLLAND) sGrp = "HOLLAND_CITIZENS";
	if (sti(sld.Nation) == SPAIN) sGrp = "SPAIN_CITIZENS";
	LAi_group_MoveCharacter(sld, sGrp); // мирный горожанин, пока его не оскорбили; дуэль сама переведёт его в DUEL_FIGHTER
	pchar.quest.GWIK_RivalLand.over = "yes";
}

void GWIK_RivalDuelOfficersOff() // Gregg: дуэль ОДИН НА ОДИН - свору (офицеров, абордажников) на бой не берём: тестер поймал, что на дуэль
// приходит вся команда. Рецепт LEO из дуэли Анжелики (Enc_OfficerGirl.c): DeleteAllOfficersFromLocation снимает их сейчас,
// глобальный bDisableOfficers держит снятыми и при следующих заходах в локации (проверяется в characters_login)
{
	if (CheckAttribute(pchar, "GWIK.DuelNoOfficers")) return; // уже сняты этой же дуэлью
	pchar.GWIK.DuelNoOfficers = true; // маркер «свору снимали МЫ» - под GWIK, а не под GWIK.Job: тот сносится при завершении поручения
	DeleteAllOfficersFromLocation();
	bDisableOfficers = true;
	DeleteAttribute(pchar, "quest.GWIK_RivalDuelOfficers.over");
	SetTimerFunction("GWIK_RivalDuelOfficers", 0, 0, 1); // страховка: если дуэль сорвётся (сбежал из боя, бросил поручение) - свора вернётся сама через сутки
}

void GWIK_RivalDuelOfficersOn() // вернуть свору
{
	if (!CheckAttribute(pchar, "GWIK.DuelNoOfficers")) return; // снимали не мы - не нам и возвращать (флаг общий на весь мод)
	DeleteAttribute(pchar, "GWIK.DuelNoOfficers");
	bDisableOfficers = false;
	pchar.quest.GWIK_RivalDuelOfficers.over = "yes"; // страховочный таймер больше не нужен
	pchar.quest.GWIK_RivalDuelLeft.over = "yes";
}

void GWIK_RivalDuelOfficers(string qName) // страховочный таймер (сутки)
{
	GWIK_RivalDuelOfficersOn();
}

void GWIK_RivalDuelLeft(string qName) // игрок ушёл с места дуэли - свора возвращается
{
	GWIK_RivalDuelOfficersOn();
}

bool GWIK_RivalDuelOutTown() // Gregg: дуэль ЗА ГОРОДОМ - своя, без «договорной» машинерии фреймворка (fight_off_town_prep: ожидание N часов, приход к воротам, разговор перед боем, «я передумал», трусость, возврат дуэлянта). Слишком много состояний, каждое ломало логику и позволяло подвесить поручение. Здесь: телепорт за город и бой сразу. Зовётся из диалога через AddDialogExitQuestFunction
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return false;
	if (pchar.GWIK.Job != "rival") return false;
	if (!CheckAttribute(pchar, "GWIK.Job.RivalId")) return false;
	if (!CheckAttribute(pchar, "GWIK.Job.City")) return false;
	int iTgt = GetCharacterIndex(pchar.GWIK.Job.RivalId);
	if (iTgt == -1) return false;
	ref sld = GetCharacter(iTgt);
	if (CharacterIsDead(sld)) return false;

	string sCity = pchar.GWIK.Job.City;
	string sPlace = sCity + "_ExitTown";
	int iPlace = FindLocation(sPlace);
	if (iPlace < 0) return false;

	// локатор прибытия: берём тот, которым локация возвращает в город (у каждой окраины он свой - reload3/reload4/...), а не гвоздями
	ref rPlace = &Locations[iPlace];
	aref arRel, arNode;
	string sLocName = "";
	string sTownLoc = sCity + "_town";
	int iN = 0;
	int k = 0;
	makearef(arRel, rPlace.reload);
	iN = GetAttributesNum(arRel);
	for (k = 0; k < iN; k++)
	{
		arNode = GetAttributeN(arRel, k);
		if (!CheckAttribute(arNode, "go")) continue;
		if (arNode.go != sTownLoc) continue;
		if (!CheckAttribute(arNode, "name")) continue;
		sLocName = arNode.name;
		break;
	}
	if (sLocName == "")
	{
		if (!CheckAttribute(rPlace, "reload.l1.name")) return false; // не нашли, куда ставить игрока - дуэли за городом не будет
		sLocName = rPlace.reload.l1.name;
	}

	DeleteAttribute(sld, "City"); // чтоб не было ругани с нацией за убийство её горожанина (так же делает Duel_Prepare_Fight)
	pchar.quest.GWIK_RivalLand.over = "yes";  // сажать его в таверну больше не нужно - он ждёт за городом
	pchar.GWIK.Job.RivalOutTown = true;       // для реплики в конторе: «ждёт вас за воротами»

	DeleteAttribute(pchar, "quest.GWIK_RivalDuelStart.over");
	pchar.quest.GWIK_RivalDuelStart.win_condition.l1 = "Location";
	pchar.quest.GWIK_RivalDuelStart.win_condition.l1.location = sPlace;
	pchar.quest.GWIK_RivalDuelStart.win_condition = "GWIK_RivalDuelStart";
	pchar.quest.GWIK_RivalDuelStart.function = "GWIK_RivalDuelStart";

	GWIK_RivalDuelOfficersOff(); // за город идёт ТОЛЬКО ГГ - свору снимаем ДО телепорта, иначе она едет с ним
	DoQuestReloadToLocation(sPlace, "reload", sLocName, ""); // телепорт игрока за город; противника поставим по прибытии, когда локация загружена
	return true;
}

void GWIK_RivalDuelStart(string qName) // Gregg: игрок за городом - ставим противника рядом и НЕМЕДЛЕННО начинаем бой (никаких разговоров и путей отказаться)
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "rival") return;
	if (!CheckAttribute(pchar, "GWIK.Job.RivalId")) return;
	int iTgt = GetCharacterIndex(pchar.GWIK.Job.RivalId);
	if (iTgt == -1) return;
	ref sld = GetCharacter(iTgt);
	if (CharacterIsDead(sld)) return;
	float locx, locy, locz;
	GetCharacterPos(pchar, &locx, &locy, &locz);
	string sLoc = LAi_FindNearestFreeLocator2Pchar("goto");
	if (sLoc == "") sLoc = LAi_FindNearestLocator("goto", locx, locy, locz); // свободных нет - берём занятый, лишь бы не пустой (пустой = движок удалит персонажа)
	if (sLoc == "") return; // локаторов "goto" нет вообще: бой не начнём, но и не сломаемся - цель добьётся месячным таймаутом
	ChangeCharacterAddressGroup(sld, pchar.location, "goto", sLoc);
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "GWIK_RivalDuel");
	LAi_group_SetRelation("GWIK_RivalDuel", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("GWIK_RivalDuel", LAI_GROUP_PLAYER, true);
	LAi_SetFightMode(pchar, true); // клинок наголо
	DeleteAllOfficersFromLocation(); // Gregg: страховка - если кто-то из своры всё же залогинился на окраине, снимаем уже на месте
	// ушёл с места дуэли (сбежал из боя, не добив) - свора возвращается, не ждать суточной страховки
	DeleteAttribute(pchar, "quest.GWIK_RivalDuelLeft.over");
	pchar.quest.GWIK_RivalDuelLeft.win_condition.l1 = "ExitFromLocation";
	pchar.quest.GWIK_RivalDuelLeft.win_condition.l1.location = pchar.location;
	pchar.quest.GWIK_RivalDuelLeft.win_condition = "GWIK_RivalDuelLeft";
	pchar.quest.GWIK_RivalDuelLeft.function = "GWIK_RivalDuelLeft";
	pchar.quest.GWIK_RivalDuelStart.over = "yes";
	if (LanguageGetLanguage() == "russian") log_info("Он уже ждёт с клинком в руке.");
	else log_info("He is already waiting, blade in hand.");
}

void GWIK_RivalTavernTalk(string qName) // Gregg: резня в таверне не остаётся тайной - через несколько дней весть расходится по архипелагу и бьёт по личной репутации (дуэль за городскими воротами такого следа не оставляет)
{
	ChangeCharacterReputation(pchar, -10); // подбираемое: «немного» по шкале мода (отказ картоторговцу -15, обчистить дуэлянта -25)
	bool bClose = true;
	if (CheckAttribute(pchar, "GWIK.Job"))
	{
		if (pchar.GWIK.Job == "rival") bClose = false; // поручение ещё не сдано в конторе - заголовок закроет сдача, а не слухи
	}
	ReOpenQuestHeader("GWIK_RivalJob"); // дописать в закрытый заголовок можно (как письмо Семнадцати Господ)
	AddQuestRecord("GWIK_RivalJob", "8");
	if (bClose) CloseQuestHeader("GWIK_RivalJob");
	if (LanguageGetLanguage() == "russian") log_info("Об убийстве в таверне уже болтают по всему архипелагу.");
	else log_info("Word of the tavern killing has spread across the archipelago.");
}

void GWIK_RivalDuelCleanup() // Gregg: снять НЕЗАКОНЧЕННУЮ дуэль с нашей целью (игрок бросил поручение / цель ушла): у фреймворка свои прерывания и таймер, без уборки они висят и роняют следующую дуэль
{
	if (!CheckAttribute(pchar, "questTemp.duel.enemy")) return;
	if (!CheckAttribute(pchar, "GWIK.Job.RivalId")) return;
	if (pchar.questTemp.duel.enemy != pchar.GWIK.Job.RivalId) return; // дуэль с кем-то другим - не наше дело
	if (CheckAttribute(pchar, "questTemp.duel.place"))
	{
		int iLoc = FindLocation(pchar.questTemp.duel.place);
		if (iLoc >= 0) Locations[iLoc].DisableEncounters = false; // вернуть встречи за воротами (гасит fight_off_town_prep, восстанавливает Duel_Move_OpponentBack, которого уже не будет)
	}
	pchar.quest.duel_move_opponent2place.over = "yes";
	pchar.quest.duel_move_opponentBack.over = "yes";
	pchar.quest.Duel_CheckSituation.over = "yes";
	pchar.quest.Duel_Talk_Off_Town.over = "yes";
	pchar.quest.Duel_Fight_Right_Now.over = "yes";
	pchar.quest.Duel_Clear_Enemy.over = "yes";
	DeleteAttribute(pchar, "questTemp.Duel");
}

bool GWIK_SpawnRival(string sCity) // судно конкурентов у колонии sCity; разновидность (drand1): 0 стоит у острова, 1 курсирует (патрулирует) в море - перехват на глобалке, 2 наземный (в таверне)
{
	string sAreal = GetArealByCityName(sCity);
	string sLocator = "";
	string sToCity = "";
	string sBlip = "";
	int iDays = 0;
	int iNation = ENGLAND;
	ref sld;

	int iVar = 0;
	bool bMoving = false;
	bool bLand = false;
	if (CheckAttribute(pchar, "GWIK.BetaForceRival")) // Gregg: бета-оверрайд варианта (тестовое меню), одноразовый - минует drand
	{
		if (pchar.GWIK.BetaForceRival == "moving") bMoving = true;
		if (pchar.GWIK.BetaForceRival == "land") bLand = true;
		DeleteAttribute(pchar, "GWIK.BetaForceRival");
	}
	else
	{
		iVar = drand1(2); // Gregg: разновидность через drand1 - СВОЙ поток! тип поручения в Job_Ask катится drand(3) (поток 0); тем же drand(1) вариант залипал бы, т.к. D%2==(D%4)%2 => на всех днях «конкурента» одно и то же значение. drand1 независим. Как фрахты у контракта, F9-стойко. 0 стоит у острова, 1 курсирует в море, 2 наземный (таверна)
		if (iVar == 1) bMoving = true;
		if (iVar == 2) bLand = true;
	}
	if (bLand)
	{
		// Gregg: город для наземного варианта подбираем СВОИМ пикером, а не берём sCity: тот пришёл из findRivalCity_GWIK
		// (только Англия/Франция) и при войне с обеими нациями наземный вариант не выдавался никогда - молча уходил в море
		string sLandCity = GWIK_PickLandRivalCity();
		if (sLandCity != "")
		{
			if (GWIK_SpawnRivalLand(sLandCity)) return true; // цель в таверне колонии - дальше всё делает дуэльный фреймворк
		}
		if (LanguageGetLanguage() == "russian")
		{
			Log_TestInfo("ГВИК: наземный вариант конкурента не выдался, откат в море.");
			Log_TestInfo("Нет колонии без войны, с таверной и окраиной.");
		}
		else
		{
			Log_TestInfo("GWIK: land rival variant unavailable, falling back to sea.");
			Log_TestInfo("No colony at peace with a tavern and outskirts.");
		}
		bMoving = true; // подходящей колонии нет - откатываемся в море, поручение не заваливаем
	}
	DeleteAttribute(pchar, "GWIK.Job.RivalLand"); // сброс наземной разновидности от прошлого поручения

	if (!bMoving)
	{
		sLocator = GWIK_PickQuestShipLocator(sAreal);
		if (sLocator == "") bMoving = true; // у острова нет квест-локатора - уходим в морской вариант, не заваливаем поручение
	}

	if (rand(1) == 1) iNation = FRANCE;
	if (bMoving)
	{
		sld = GetCharacter(NPC_GenerateCharacter("GWIK_RivalShip", "", "man", "man", sti(pchar.rank), iNation, -1, true)); // фикс id (без суффикса) - его ждут Map_CreateTrader / win_condition / Map_TraderSucces_quest
	}
	else
	{
		sld = GetCharacter(NPC_GenerateCharacter("GWIK_Rival", "", "man", "man", sti(pchar.rank), iNation, -1, true));
		sld.id = sld.id + "_" + sld.index; // уникальный id, чтобы мёртвые дубли не путались под ногами
	}
	SetCaptanModelByEncType(sld, "trade");
	DeleteAttribute(sld, "Ship");
	SetShipToFantom(sld, "trade", true);
	sld.Ship.Mode = "Trade";
	sld.AlwaysSandbankManeuver = true;
	sld.DontRansackCaptain = true;

	Group_FindOrCreateGroup("GWIK_RivalGroup");
	Group_SetType("GWIK_RivalGroup", "trade");
	Group_AddCharacter("GWIK_RivalGroup", sld.id);
	Group_SetGroupCommander("GWIK_RivalGroup", sld.id);

	pchar.GWIK.Job.RivalId = sld.id;
	pchar.GWIK.Job.City = sCity;
	pchar.GWIK.Job.ShipType = XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName);
	pchar.GWIK.Job.ShipName = sld.Ship.Name; // Gregg: имя корабля (случайное, из SetShipToFantom) - чтобы журнал/диалог показывали «тип 'Имя'», а не только тип
	DeleteAttribute(pchar, "GWIK.Job.RivalMoving"); // сброс разновидности от прошлого поручения

	// Gregg: сброс маркеров завершения от прошлого rival-поручения, иначе перевзвод win_condition/таймера мёртв (over="yes" остаётся)
	DeleteAttribute(pchar, "quest.GWIK_RivalDead.over");
	DeleteAttribute(pchar, "quest.GWIK_RivalGone.over");

	// прерывание на смерть конкурента (обе разновидности)
	pchar.quest.GWIK_RivalDead.win_condition.l1 = "NPC_Death";
	pchar.quest.GWIK_RivalDead.win_condition.l1.character = sld.id;
	pchar.quest.GWIK_RivalDead.win_condition = "GWIK_RivalDead";
	pchar.quest.GWIK_RivalDead.function = "GWIK_RivalDead";

	if (bMoving)
	{
		// морской вариант: конкурент ПАТРУЛИРУЕТ воды своей колонии (перехват на глобалке). Перезапуск отрезков в GWIK_RivalDispatchLeg, реальный провал - только месячный таймаут GWIK_RivalGone
		if (GetIslandRandomShoreId(sAreal) == "")
		{
			sLocator = GWIK_PickQuestShipLocator(sAreal); // у острова нет берега для маршрута - откатываемся на статичный, если есть локатор
			if (sLocator == "") { GWIK_DropRivalJob(); return false; }
			bMoving = false;
		}
		else
		{
			pchar.GWIK.Job.RivalMoving = true;
			if (LanguageGetLanguage() == "russian") sBlip = "Судно конкурентов"; else sBlip = "Rivals' ship";
			if (sld.Ship.Name != "") sBlip = sBlip + " '" + sld.Ship.Name + "'";
			sld.mapEnc.type = "trade";
			sld.mapEnc.worldMapShip = "Galleon_red";
			sld.mapEnc.Name = sBlip;
			DeleteAttribute(pchar, "GWIK.Job.RivalLeg"); // старт с A->B
			sToCity = GWIK_PickPatrolPartner(sCity); // второй конец коридора (переиспользуем локал sToCity)
			if (sToCity != "") pchar.GWIK.Job.RivalCity2 = sToCity;
			// Gregg: в море НЕ сразу - двое суток судно догружается в порту своей колонии. Иначе поручение выдавалось на судно,
			// которое уже мотается по коридору где-то далеко: игрок выходил из конторы, а перехватывать было нечего и негде.
			// Теперь у него есть форы дойти до нужных вод, а выход в море сопровождается сообщением
			pchar.GWIK.Job.RivalInPort = true;
			DeleteAttribute(pchar, "quest.GWIK_RivalPutToSea.over"); // иначе перевзвод таймера мёртв (over от прошлого поручения)
			SetTimerFunction("GWIK_RivalPutToSea", 0, 0, 2);
		}
	}
	if (!bMoving)
	{
		Group_SetAddress("GWIK_RivalGroup", sAreal, "Quest_ships", sLocator);
		Group_LockTask("GWIK_RivalGroup");
	}

	// таймаут-бэкстоп: через месяц судно уходит (обе разновидности)
	SetTimerFunction("GWIK_RivalGone", 0, 1, 0);
	return true;
}

void GWIK_RivalPutToSea(string qName) // Gregg: двое суток истекли - конкурент выходит из порта и начинает патруль между двумя портами
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "rival") return;
	if (CheckAttribute(pchar, "GWIK.Job.Done")) return;
	if (!CheckAttribute(pchar, "GWIK.Job.RivalMoving")) return; // не морская разновидность (или поручение уже другое) - таймер пустой
	if (!CheckAttribute(pchar, "GWIK.Job.RivalInPort")) return; // уже в море
	DeleteAttribute(pchar, "GWIK.Job.RivalInPort");
	GWIK_RivalDispatchLeg(); // первый отрезок патруля (дальше перезапуск при каждом «прибытии»)
	string sPort = "";
	if (CheckAttribute(pchar, "GWIK.Job.City")) sPort = XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Gen");
	if (LanguageGetLanguage() == "russian") Log_Info("Судно конкурентов вышло из порта " + sPort + ".");
	else Log_Info("The rivals' ship has left the port of " + sPort + ".");
}

void GWIK_RivalDead(string qName)
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "rival") return;
	pchar.GWIK.Job.Done = true;
	GWIK_AddRep(17); // Gregg: репутация за устранение конкурента Компании
	pchar.quest.GWIK_RivalGone.over = "yes"; // снимаем таймаут
	GWIK_RivalDuelOfficersOn(); // Gregg: дуэль окончена - свора возвращается
	if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) // Gregg: наземный вариант - цель убита в дуэли (или прирезана в таверне)
	{
		pchar.quest.GWIK_RivalLand.over = "yes"; // сажать его больше некуда
		pchar.quest.GWIK_RivalDuelStart.over = "yes"; // и дуэль за городом больше не начинать
		ref rLoc = &Locations[FindLoadedLocation()];
		bool bTavern = false;
		if (CheckAttribute(pchar, "questTemp.LocationClone")) bTavern = true; // дрались в клоне таверны (его ставит LocationMakeClone)
		if (CheckAttribute(rLoc, "type"))
		{
			if (rLoc.type == "tavern") bTavern = true; // либо прирезали прямо в таверне, без всяких дуэлей
		}
		if (bTavern)
		{
			DeleteAttribute(pchar, "quest.GWIK_RivalTavernTalk.over"); // иначе перевзвод таймера мёртв
			SetTimerFunction("GWIK_RivalTavernTalk", 0, 0, 2); // через два дня весть разойдётся - минус личная репутация (за воротами свидетелей нет, там чисто)
		}
		AddQuestRecord("GWIK_RivalJob", "7"); // данные - строго после записи
		if (CheckAttribute(pchar, "GWIK.Job.RivalName")) AddQuestUserData("GWIK_RivalJob", "sName", pchar.GWIK.Job.RivalName);
		if (CheckAttribute(pchar, "GWIK.Job.Pay")) AddQuestUserData("GWIK_RivalJob", "sMoney", FindRussianMoneyString(sti(pchar.GWIK.Job.Pay)));
		if (LanguageGetLanguage() == "russian") log_info("Человек конкурентов Компании мёртв. Доложите в контору ГВИК.");
		else log_info("The Company's rival agent is dead. Report to a GWIC office.");
		return;
	}
	if (CheckAttribute(pchar, "GWIK.Job.RivalMoving")) Map_ReleaseQuestEncounter("GWIK_RivalShip"); // Gregg: снять карт-энкаунтер (морской вариант)
	Group_DeleteGroup("GWIK_RivalGroup");
	AddQuestRecord("GWIK_RivalJob", "2"); // данные - строго после записи: AddQuestUserData вешает их на последнюю запись
	AddQuestUserData("GWIK_RivalJob", "sShip", GWIK_RivalShipStr()); // Gregg: тип + имя корабля
	if (CheckAttribute(pchar, "GWIK.Job.Pay")) AddQuestUserData("GWIK_RivalJob", "sMoney", FindRussianMoneyString(sti(pchar.GWIK.Job.Pay)));
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Судно конкурентов больше не выйдет в море.");
		log_info("Доложите в контору ГВИК.");
	}
	else
	{
		log_info("The Company's rivals have lost their ship.");
		log_info("Report to a GWIC office.");
	}
}

void GWIK_DropRivalJob() // снять задание и вычистить судно (или человека) конкурентов
{
	pchar.quest.GWIK_RivalDead.over = "yes";
	pchar.quest.GWIK_RivalGone.over = "yes";
	pchar.quest.GWIK_RivalLand.over = "yes"; // Gregg: наземный вариант - отменить посадку цели в таверне
	pchar.quest.GWIK_RivalDuelStart.over = "yes"; // и начало дуэли за городом
	pchar.quest.GWIK_RivalPutToSea.over = "yes"; // Gregg: и выход из порта, если судно ещё догружалось
	GWIK_RivalDuelOfficersOn(); // Gregg: бросили поручение посреди дуэли - вернуть свору
	GWIK_RivalDuelCleanup(); // Gregg: и снять недоигранную дуэль с ней, иначе прерывания/таймер фреймворка висят
	if (CheckAttribute(pchar, "GWIK.Job.RivalMoving")) Map_ReleaseQuestEncounter("GWIK_RivalShip"); // Gregg: снять карт-энкаунтер морского варианта
	Group_DeleteGroup("GWIK_RivalGroup");
	if (CheckAttribute(pchar, "GWIK.Job.RivalId"))
	{
		int i = GetCharacterIndex(pchar.GWIK.Job.RivalId);
		if (i != -1)
		{
			if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) Characters[i].location = ""; // Gregg: наземную цель сначала УБИРАЕМ из таверны, иначе на глазах игрока останется труп ни с того ни с сего
			LAi_SetCurHP(&Characters[i], 0.0); // освобождаем слот
		}
	}
	DeleteAttribute(pchar, "GWIK.Job");
}

void GWIK_RivalGetaway() // Gregg: конкурент ушёл (таймаут ИЛИ дошёл до порта в морском варианте) - поручение провалено
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "rival") return;
	if (CheckAttribute(pchar, "GWIK.Job.Done")) return;
	bool bLand = CheckAttribute(pchar, "GWIK.Job.RivalLand"); // читаем ДО уборки: GWIK_DropRivalJob сносит GWIK.Job целиком
	if (bLand) AddQuestRecord("GWIK_RivalJob", "9"); // наземный: конкурент уехал из города
	else AddQuestRecord("GWIK_RivalJob", "4");
	CloseQuestHeader("GWIK_RivalJob");
	GWIK_DropRivalJob();
	SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
	if (bLand)
	{
		if (LanguageGetLanguage() == "russian")
		{
			log_info("Человек конкурентов покинул колонию.");
			log_info("Поручение Компании провалено.");
		}
		else
		{
			log_info("The rivals' agent has left the colony.");
			log_info("The Company's assignment has failed.");
		}
		return;
	}
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Судно конкурентов ушло из вод архипелага.");
		log_info("Поручение Компании провалено.");
	}
	else
	{
		log_info("The rivals' ship has left these waters.");
		log_info("The Company's assignment has failed.");
	}
}

void GWIK_RivalGone(string qName) // таймаут: конкурент ушёл (бэкстоп)
{
	GWIK_RivalGetaway();
}

bool GWIK_ConvoySpawnAmbush(string sDestCity) // Gregg: засада на эскорт - 1 пират ДОГОНЯЕТ эскадру ГГ на глобалке от острова назначения (Map_CreateWarrior + Group_SetTaskAttackInMap, как ямайские преследователи) и навязывает бой. Редкая и лёгкая (решение: эскорт и так ловит случайные встречи карты, не дублировать). Статичный Quest_ships легко обходился кругом
{
	string sAreal = GetArealByCityName(sDestCity);
	string sShore = GetIslandRandomShoreId(sAreal);
	if (sShore == "") return false; // нет берега у острова - засады не будет, эскорт не страдает
	int r = sti(pchar.rank);
	string sGroup = "Sea_GWIK_ConvoyAmbush0"; // "Sea_" + id лидера - обязательно, иначе в бой войдёт ТОЛЬКО лидер (движок грузит группу Sea_<charID>)
	Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup, "war");
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP); // догонять и атаковать эскадру ГГ на карте - обойти нельзя
	Group_LockTask(sGroup);
	ref p;      // реф/счётчики - ВНЕ цикла (Storm не любит ref в теле for; без динамических атрибут-ключей)
	int k;
	string sid;
	int iEClass;
	int iEType;
	for (k = 0; k < 1; k++) // Gregg: 1 пират (засада редкая и лёгкая, чтоб не дублировать случайные встречи карты); группа Sea_+лидер оставлена на случай расширения
	{
		sid = "GWIK_ConvoyAmbush" + k; // фикс id (0 - лидер-блип)
		p = GetCharacter(NPC_GenerateCharacter(sid, "", "man", "man", r, PIRATE, -1, true));
		SetCaptanModelByEncType(p, "trade");
		DeleteAttribute(p, "Ship");
		SetShipToFantom(p, "war", true);
		// Gregg: тип по КЛАССУ корабля ГГ (тот же алгоритм, что у пирата выкупа GWIK_RansomForceSpawn), а не гвоздями SHIP_CORVETTE: на класс слабее ГГ, но не слабее 6; случайный тип этого класса
		iEClass = GetCharacterShipClass(pchar) + 1;
		if (iEClass > 6) iEClass = 6;
		iEType = GWIK_RandShipTypeByClass(iEClass);
		if (iEType < 0) iEType = SHIP_SLOOP; // фолбэк, если класс пуст
		p.Ship.Type = GenerateShipExt(iEType, true, p);
		SetBaseShipData(p);
		SetCrewQuantityFull(p);
		Fantom_SetCannons(p, "war");
		Fantom_SetBalls(p, "war");
		SetRandomNameToCharacter(p);
		p.Ship.Name = generateRandomNameToShip(PIRATE);
		p.Ship.Mode = "War";
		p.AlwaysSandbankManeuver = true;
		p.AlwaysEnemy = true; // нападает сам
		p.DontRansackCaptain = true;
		SetRandGeraldSail(p, sti(p.Nation));
		p.mapEnc.type = "war";                 // для лидера ОБЯЗАТЕЛЬНО (иначе GenerateMapEncounter_Alone не спавнит энкаунтер)
		p.mapEnc.worldMapShip = "quest_ship";
		if (LanguageGetLanguage() == "russian") p.mapEnc.Name = "Пираты-перехватчики"; else p.mapEnc.Name = "Raiding pirates";
		Group_AddCharacter(sGroup, sid);
		if (k == 0) Group_SetGroupCommander(sGroup, sid);
	}
	Map_CreateWarrior(sShore, "GWIK_ConvoyAmbush0", 20); // догоняющий от острова назначения; лидер = блип, группа подтянется в бой
	pchar.GWIK.ConvoyAmbush = "yes"; // маркер для уборки
	return true;
}

void GWIK_ConvoyAmbushCleanup() // Gregg: убрать засаду при завершении эскорта (любой исход) - иначе пираты вечно догоняют игрока
{
	if (!CheckAttribute(pchar, "GWIK.ConvoyAmbush")) return;
	Map_ReleaseQuestEncounter("GWIK_ConvoyAmbush0"); // снять карт-энкаунтер лидера, если ещё догоняет
	int k;
	int pi;
	for (k = 0; k < 2; k++)
	{
		pi = GetCharacterIndex("GWIK_ConvoyAmbush" + k);
		if (pi >= 0) LAi_SetCurHP(&Characters[pi], 0.0);
	}
	Group_DeleteGroup("Sea_GWIK_ConvoyAmbush0");
	DeleteAttribute(pchar, "GWIK.ConvoyAmbush");
}

void GWIK_MailAgentArrive(string qName) // Gregg: игрок вошёл в город конторы с «горячим» пакетом - агент конкурентов подходит на улице (win_condition "Location")
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "mail") return;
	if (!CheckAttribute(pchar, "GWIK.Job.Hot")) return;
	if (CheckAttribute(pchar, "GWIK.Job.HotResolved")) return;
	if (CheckAttribute(pchar, "GWIK.Job.MailAgentId")) return; // уже стоит
	if (!CheckAttribute(pchar, "GWIK.Job.HotTown")) return;
	string sTown = pchar.GWIK.Job.HotTown;
	// стартовый локатор по городу (подтверждённые квестами: Villemstad reload3, Marigo goto12); агент всё равно САМ идёт к ГГ (ActorDialog), локатор нужен лишь валидный
	string sGrp = "goto";
	string sLoc = "goto1";
	if (sTown == "Villemstad_town") { sGrp = "reload"; sLoc = "reload3"; }
	if (sTown == "Marigo_town")     { sGrp = "goto";   sLoc = "goto12"; }
	if (sTown == "FortOrange_town") { sGrp = "reload"; sLoc = "reload1"; }
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_MailAgent", "pirate_" + (rand(9) + 1), "man", "man", 10, PIRATE, -1, false));
	sld.id = sld.id + "_" + sld.index;
	pchar.GWIK.Job.MailAgentId = sld.id;
	sld.Dialog.Filename = "GWIK_MailAgent.c";
	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS"); // нейтральная городская группа (как фактор) - не втянуть в бой
	LAi_SetHP(sld, 100.0, 100.0);
	ChangeCharacterAddressGroup(sld, sTown, sGrp, sLoc);
	LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", -1, 0); // сам подходит и заговаривает
	pchar.quest.GWIK_MailAgent.over = "yes"; // этот заход отработан; следующий перевзведёт GWIK_MailAgentLeft
	// Gregg: игрок может уйти из локации, не дослушав (тестер поймал). Ловим выход и убираем агента - иначе он остаётся
	// с зависшей актёрской командой и при возвращении бежит через полгорода как заведённый
	DeleteAttribute(pchar, "quest.GWIK_MailAgentLeft.over");
	pchar.quest.GWIK_MailAgentLeft.win_condition.l1 = "ExitFromLocation";
	pchar.quest.GWIK_MailAgentLeft.win_condition.l1.location = sTown;
	pchar.quest.GWIK_MailAgentLeft.win_condition = "GWIK_MailAgentLeft";
	pchar.quest.GWIK_MailAgentLeft.function = "GWIK_MailAgentLeft";
}

void GWIK_MailAgentLeft(string qName) // Gregg: ушли из города, не поговорив - агента убираем и перевзводим перехват на следующий заход.
// Пакет остаётся горячим: конкуренты своего не бросят, но подходить будут заново и с чистого места, а не догонять зависшим актёром
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "mail") return;
	if (CheckAttribute(pchar, "GWIK.Job.HotResolved")) return; // разговор уже состоялся - перехват отработан
	if (CheckAttribute(pchar, "GWIK.Job.HotFight")) return;    // идёт драка: выход заблокирован, сюда не попасть (страховка)
	if (!CheckAttribute(pchar, "GWIK.Job.HotTown")) return;
	if (CheckAttribute(pchar, "GWIK.Job.MailAgentId"))
	{
		int i = GetCharacterIndex(pchar.GWIK.Job.MailAgentId);
		if (i != -1)
		{
			Characters[i].location = ""; // вне диалога локацию обнулять безопасно (в диалоге - нельзя, ломает exit)
			LAi_SetCurHP(&Characters[i], 0.0);
		}
		DeleteAttribute(pchar, "GWIK.Job.MailAgentId");
	}
	int iDodge = 0;
	if (CheckAttribute(pchar, "GWIK.Job.HotDodge")) iDodge = sti(pchar.GWIK.Job.HotDodge);
	pchar.GWIK.Job.HotDodge = iDodge + 1; // сколько раз разминулись - агент это припомнит в приветствии
	// Gregg: перехват переезжает в ГОРОД НАЗНАЧЕНИЯ. Иначе корабль отменял ветку насовсем: перехват висел только на городе выдачи,
	// и уплывший игрок сдавал пакет, ни разу не увидев агента. Конкуренты знают адрес доставки, а контора сидит внутри здания -
	// в город получателя войти всё равно придётся, так что встреча состоится
	string sNext = pchar.GWIK.Job.HotTown;
	if (CheckAttribute(pchar, "GWIK.Job.Target"))
	{
		string sCity = GWIK_OfficeCityById(pchar.GWIK.Job.Target);
		if (sCity != "") sNext = sCity + "_town";
	}
	pchar.GWIK.Job.HotTown = sNext;
	// перевзвести перехват: при следующем входе в этот город агент подойдёт заново, с чистого места
	DeleteAttribute(pchar, "quest.GWIK_MailAgent.over");
	pchar.quest.GWIK_MailAgent.win_condition.l1 = "Location";
	pchar.quest.GWIK_MailAgent.win_condition.l1.location = sNext;
	pchar.quest.GWIK_MailAgent.win_condition = "GWIK_MailAgentArrive";
	pchar.quest.GWIK_MailAgent.function = "GWIK_MailAgentArrive";
	pchar.quest.GWIK_MailAgentLeft.over = "yes";
}

void GWIK_MailAgentDismiss(ref agent) // Gregg: агент уходит после выбора; зовётся из GWIK_MailAgent.c
{
	// НЕ agent.location="" - обнуление локации у СОБЕСЕДНИКА посреди его же диалога ломает закрытие (диалог застревал на "exit").
	// Паттерн фактора: актёр сам уходит к воротам и исчезает - персонаж остаётся валиден, exit срабатывает. Плохой локатор просто не проложит путь (не застревание). MailAgentId НЕ чистим - GWIK_MailAgentCleanup добьёт при сдаче/сбросе.
	LAi_SetActorType(agent);
	LAi_ActorGoToLocation(agent, "reload", "reload1", "none", "", "", "", 20.0);
}

void GWIK_MailFightRelease(bool bKillLive) // Gregg: снять блок выхода из локации после драки за пакет; bKillLive - добить недобитых (аварийный путь)
{
	chrDisableReloadToLocation = false; // из локации снова можно уйти
	DelEventHandler("LAi_event_GroupKill", "GWIK_MailThugsKilled");
	pchar.quest.GWIK_MailFightTimeout.over = "yes";
	DeleteAttribute(pchar, "GWIK.Job.HotFight");
	if (!bKillLive) return;
	int k;
	int i;
	for (k = 0; k < 8; k++) // фикс-id, счётчики и индексы ВНЕ рефов: динамических ключей и ref в теле цикла не заводим
	{
		i = GetCharacterIndex("GWIK_MailThug" + k);
		if (i != -1) LAi_SetCurHP(&Characters[i], 0.0);
	}
}

void GWIK_MailThugsKilled() // Gregg: вся ватага мертва - пакет отбит, выход свободен
{
	string sGroup = GetEventData();
	if (sGroup != "GWIK_MailThugs") return; // чужая группа - не наше дело
	GWIK_MailFightRelease(false);
	GWIK_AddRep(5); // Компания ценит, что её бумаги не отдали
	AddQuestRecord("GWIK_MailJob", "8");
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Нападавшие мертвы, пакет Компании при вас.");
		log_info("Из города можно уходить.");
	}
	else
	{
		log_info("The attackers are dead, the Company packet is safe.");
		log_info("You are free to leave the town.");
	}
}

void GWIK_MailFightTimeout(string qName) // Gregg: страховка от залипшего блока выхода (кто-то из ватаги застрял и не умирает)
{
	if (!CheckAttribute(pchar, "GWIK.Job.HotFight")) return;
	GWIK_MailFightRelease(true);
}

void GWIK_MailAgentFightStart() // Gregg: отказ отдать пакет - агент свистит подмогу и берёт силой. Зовётся отложенно (AddDialogExitQuestFunction),
// чтобы бой начался ПОСЛЕ закрытия диалога. Из локации не выйти, пока вся ватага не перебита
{
	if (!CheckAttribute(pchar, "GWIK.Job.MailAgentId")) return;
	int iAgent = GetCharacterIndex(pchar.GWIK.Job.MailAgentId);
	if (iAgent == -1) return;
	ref agent = GetCharacter(iAgent);
	int iQty = 1 + GetOfficersQuantity(pchar); // подмога по размеру своры ГГ: один агент на капитана с офицерами не полезет
	string sGroup = "GWIK_MailThugs";
	ref p;        // реф и счётчики ВНЕ цикла - Storm не любит ref в теле for
	int k;
	int idx;
	string sLoc;
	float locx, locy, locz;
	int r = sti(pchar.rank);
	GetCharacterPos(pchar, &locx, &locy, &locz);
	for (k = 0; k < iQty; k++)
	{
		idx = NPC_GenerateCharacter("GWIK_MailThug" + k, "pirate_" + (rand(9) + 1), "man", "man", r, PIRATE, 1, false); // фикс-id: по ним же убираем
		if (idx == -1) continue;
		p = GetCharacter(idx);
		SetFantomParamHunter(p); // клинок, пистолет и навыки по рангу ГГ
		sLoc = LAi_FindNearestFreeLocator2Pchar("goto");
		if (sLoc == "") sLoc = LAi_FindNearestLocator("goto", locx, locy, locz); // свободных нет - занятый лучше пустой строки (на пустой движок удаляет персонажа)
		if (sLoc == "") continue;
		ChangeCharacterAddressGroup(p, pchar.location, "goto", sLoc);
		LAi_SetWarriorType(p);
		LAi_group_MoveCharacter(p, sGroup);
	}
	SetFantomParamHunter(agent); // сам агент дрался бы голыми руками - вооружаем
	LAi_SetWarriorType(agent);
	LAi_group_MoveCharacter(agent, sGroup); // из городской группы - в свою, иначе горожане полезут разнимать
	LAi_group_SetRelation(sGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups(sGroup, LAI_GROUP_PLAYER, true);
	LAi_SetFightMode(pchar, true);
	pchar.GWIK.Job.HotFight = true;
	chrDisableReloadToLocation = true; // пока живы - из локации не уйти
	SetEventHandler("LAi_event_GroupKill", "GWIK_MailThugsKilled", 0);
	LAi_group_SetCheckEvent(sGroup);
	DeleteAttribute(pchar, "quest.GWIK_MailFightTimeout.over");
	SetTimerFunction("GWIK_MailFightTimeout", 0, 0, 1); // сутки - страховка от залипшего блока выхода
	if (LanguageGetLanguage() == "russian") log_info("Из подворотен выходят люди агента. Уйти не дадут.");
	else log_info("The agent's men step out of the alleys. There is no walking away.");
}

void GWIK_MailAgentCleanup() // Gregg: полная уборка агента при завершении/сбросе почты (если игрок не поговорил и агент завис)
{
	if (CheckAttribute(pchar, "GWIK.Job.HotFight")) GWIK_MailFightRelease(true); // поручение кончилось посреди драки - снять блок выхода и убрать ватагу
	pchar.quest.GWIK_MailAgent.over = "yes";
	pchar.quest.GWIK_MailAgentLeft.over = "yes"; // и вотч «ушёл из города, не поговорив» - иначе остаётся взведённым на пустом месте
	if (CheckAttribute(pchar, "GWIK.Job.MailAgentId"))
	{
		int i = GetCharacterIndex(pchar.GWIK.Job.MailAgentId);
		if (i != -1) { Characters[i].location = ""; LAi_SetCurHP(&Characters[i], 0.0); }
		DeleteAttribute(pchar, "GWIK.Job.MailAgentId");
	}
}

void GWIK_SnapshotMutineer(ref sld) // Gregg: снимок личности «посредника» в таверне - если он окажется беглым капитаном (бунт), перехваченный/доставленный будет ТЕМ ЖЕ человеком (лицо+имя+модель+ранг)
{
	pchar.GWIK.Job.MutinyName     = sld.name;
	pchar.GWIK.Job.MutinyLastName = sld.lastname;
	if (CheckAttribute(sld, "middlename")) pchar.GWIK.Job.MutinyMidName = sld.middlename;
	pchar.GWIK.Job.MutinySex      = sld.sex;
	pchar.GWIK.Job.MutinyModel    = sld.model;              // модель уже разрешена FaceMaker'ом внутри NPC_GenerateCharacter
	pchar.GWIK.Job.MutinyAni      = sld.model.animation;
	pchar.GWIK.Job.MutinyFaceId   = sld.FaceId;             // портрет = FaceId (модель на экране портрет НЕ задаёт - копировать обязательно)
	if (CheckAttribute(sld, "HeroModel")) pchar.GWIK.Job.MutinyHeroModel = sld.HeroModel;
	pchar.GWIK.Job.MutinyRank     = sld.rank;               // капитан в таверне - ранг 10
}

void GWIK_StampMutineer(ref sld, bool bCopyRank) // Gregg: наложить снимок личности беглеца на цель (доставленный пленник силового пути / капитан перехваченного корабля), чтобы это был один и тот же человек
{
	if (!CheckAttribute(pchar, "GWIK.Job.MutinyModel")) return; // нет снимка (старый сейв / не тот путь) - оставить случайную личность
	sld.name            = pchar.GWIK.Job.MutinyName;
	sld.lastname        = pchar.GWIK.Job.MutinyLastName;
	if (CheckAttribute(pchar, "GWIK.Job.MutinyMidName")) sld.middlename = pchar.GWIK.Job.MutinyMidName;
	else DeleteAttribute(sld, "middlename");
	sld.sex             = pchar.GWIK.Job.MutinySex;
	sld.model           = pchar.GWIK.Job.MutinyModel;
	sld.model.animation = pchar.GWIK.Job.MutinyAni;
	sld.FaceId          = pchar.GWIK.Job.MutinyFaceId;      // портрет - копировать обязательно (иначе останется лицо случайной модели)
	if (CheckAttribute(pchar, "GWIK.Job.MutinyHeroModel")) sld.HeroModel = pchar.GWIK.Job.MutinyHeroModel;
	else DeleteAttribute(sld, "HeroModel");                 // снять кирас-сабмодель случайной trader-модели
	DeleteAttribute(sld, "VISUAL_CIRASS");                  // guard-флаг FaceMaker; FaceId задан вручную, повторный прогон не нужен
	if (bCopyRank) sld.rank = pchar.GWIK.Job.MutinyRank;    // доставленный пленник виден на экране - ранг таверны (10); у капитана шлюпа боевой уровень оставляем масштабированным под ГГ
}

void GWIK_MutineerDepart() // Gregg: беглец согласился сам вернуться в Компанию (успешное убеждение) - покидает таверну. Deferred через AddDialogExitQuestFunction: обнуление локации собеседника безопасно ТОЛЬКО после закрытия диалога (образец LandEnc_OfficerHired)
{
	if (!CheckAttribute(pchar, "GWIK.Job.AgentId")) return;
	int i = GetCharacterIndex(pchar.GWIK.Job.AgentId);
	if (i == -1) return;
	Characters[i].location = "";
	Characters[i].location.group = "";
	Characters[i].location.locator = "";
}

void GWIK_StartRansomJob() // посредник пиратов в таверне на Бермудах
{
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_PirateAgent", "pirate_" + (rand(9) + 1), "man", "man", 10, PIRATE, -1, false));
	sld.id = sld.id + "_" + sld.index;
	pchar.GWIK.Job.AgentId = sld.id;
	GWIK_SnapshotMutineer(sld); // Gregg: снимок личности на случай бунта - тогда во всех исходах беглец будет ТЕМ ЖЕ человеком, что сидит здесь
	sld.City = "Pirates";
	sld.location = "Pirates_tavern";
	sld.location.group = "sit";
	sld.location.locator = "sit4";
	sld.Dialog.Filename = "GWIK_Ransom.c";
	LAi_SetSitTypeNoGroup(sld);
	LAi_SetLoginTime(sld, 0.0, 24.0);
	LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
	LAi_SetHP(sld, 80.0, 80.0);
}

void GWIK_RansomPay() // выкуп заплачен: капитан Компании становится пассажиром
{
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_Captive", "trader_" + (rand(3) + 1), "man", "man", 5 + rand(10), HOLLAND, -1, false));
	sld.id = sld.id + "_" + sld.index;
	pchar.GWIK.Job.CaptiveId = sld.id;
	if (CheckAttribute(pchar, "GWIK.Job.Mutiny")) GWIK_StampMutineer(sld, true); // Gregg: бунт - доставленный пленник = беглец из таверны (силовой путь: тот, кого взяли на абордаж), иначе случайный голландец
	DeleteAttribute(sld, "ship");
	sld.ship = "";
	sld.location = "";
	sld.location.group = "";
	sld.location.locator = "";
	AddPassenger(pchar, sld, false);
	SetCharacterRemovable(sld, false); // Gregg: квестовый пассажир - нельзя ставить на должность/капитаном эскадры, продавать или ссаживать
	pchar.GWIK.Job.Rescued = true;
}

void GWIK_RansomForceCleanup(bool bKillLive) // Gregg: снять прерывания захвата/потопления/таймаута и убрать пиратский шлюп силового пути
{
	pchar.quest.GWIK_RansomBoarded.over = "yes";
	pchar.quest.GWIK_RansomSunk.over = "yes";
	pchar.quest.GWIK_RansomTimeout.over = "yes";
	pchar.quest.GWIK_RansomPutToSea.over = "yes"; // Gregg: и выход из порта, если судно ещё догружалось
	if (CheckAttribute(pchar, "GWIK.Job.ForceShipId"))
	{
		if (bKillLive) // шлюп ещё в пути (отказ от задания на полпути) - снять карт-энкаунтер и освободить слот; после абордажа/потопления/побега НЕ трогаем
		{
			Map_ReleaseQuestEncounter("GWIK_RansomShip");
			int i = GetCharacterIndex("GWIK_RansomShip");
			if (i != -1) LAi_SetCurHP(&Characters[i], 0.0);
		}
		Group_DeleteGroup("GWIK_RansomShipGroup");
		DeleteAttribute(pchar, "GWIK.Job.ForceShipId");
	}
}

void GWIK_RansomForceFail(string sRecord) // Gregg: общий провал силового пути - капитан потерян, позор, посредник ушёл
{
	int i;
	GWIK_RansomForceCleanup(false);
	GWIK_AddRep(-10); // позор: Компания потеряла капитана по вине наёмника (GWIK_AddRep не уходит ниже 0)
	AddQuestRecord("GWIK_RansomJob", sRecord);
	if (CheckAttribute(pchar, "GWIK.Job.ForceShipName")) AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName); // имя корабля в журнал (строго после AddQuestRecord)
	CloseQuestHeader("GWIK_RansomJob");
	if (CheckAttribute(pchar, "GWIK.Job.AgentId")) // посредник больше не нужен
	{
		i = GetCharacterIndex(pchar.GWIK.Job.AgentId);
		if (i != -1) LAi_SetCurHP(&Characters[i], 0.0);
	}
	DeleteAttribute(pchar, "GWIK.Job");
	SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
}

void GWIK_FinishRansomJob() // сдать капитана в контору (или отказаться от задания)
{
	int i;
	GWIK_RansomForceCleanup(true); // если бросили силовой путь с живым шлюпом - убрать его
	if (CheckAttribute(pchar, "GWIK.Job.CaptiveId"))
	{
		i = GetCharacterIndex(pchar.GWIK.Job.CaptiveId);
		if (i != -1)
		{
			RemovePassenger(pchar, &Characters[i]);
			Characters[i].location = "";
			LAi_SetCurHP(&Characters[i], 0.0); // ушёл служить Компании, слот свободен
		}
	}
	if (CheckAttribute(pchar, "GWIK.Job.AgentId"))
	{
		i = GetCharacterIndex(pchar.GWIK.Job.AgentId);
		if (i != -1)
		{
			Characters[i].location = "";
			LAi_SetCurHP(&Characters[i], 0.0);
		}
	}
	DeleteAttribute(pchar, "GWIK.Job");
}

int GWIK_RandShipTypeByClass(int iClass) // Gregg: случайный обычный (не спец/квест) тип корабля заданного класса; -1 если нет (образец перебора - PGG_progress.c)
{
	int aShips[128];
	int n = 0;
	int i;
	for (i = 0; i <= SHIP_OCEAN; i++)
	{
		if (!CheckAttribute(&ShipsTypes[i], "Class")) continue;
		if (sti(ShipsTypes[i].Class) != iClass) continue;
		if (CheckAttribute(&ShipsTypes[i], "QuestShip")) continue; // LEO: спец/квест - по метке, а не по «дороже манвара» (инитный ценник выпилен)
		aShips[n] = i;
		n++;
	}
	if (n == 0) return -1;
	return aShips[rand(n - 1)];
}

void GWIK_RansomDispatchLeg() // Gregg: очередной отрезок ПАТРУЛЯ пиратского корабля с пленником - коридор между двумя портами, направление чередуется (перехват в открытом море). Вызов при спавне и при каждом «прибытии», пока не убит/потоплен/месячный таймаут. НЕ односторонний рейс - иначе «уходил в порт» и терял капитана раньше, чем игрок перехватит
{
	if (!CheckAttribute(pchar, "GWIK.Job.ForceShipId")) return;
	if (!CheckAttribute(pchar, "GWIK.Job.PirateCity")) return;
	string cA = pchar.GWIK.Job.PirateCity;
	string cB = cA;
	if (CheckAttribute(pchar, "GWIK.Job.PirateCity2") && pchar.GWIK.Job.PirateCity2 != "") cB = pchar.GWIK.Job.PirateCity2;
	string fromCity = cA;
	string toCity = cB;
	if (CheckAttribute(pchar, "GWIK.Job.PirateLeg") && sti(pchar.GWIK.Job.PirateLeg) == 1) { fromCity = cB; toCity = cA; pchar.GWIK.Job.PirateLeg = 0; } // чередуем A->B / B->A
	else pchar.GWIK.Job.PirateLeg = 1;
	string sShore = GetIslandRandomShoreId(GetArealByCityName(fromCity));
	if (sShore == "") sShore = GetIslandRandomShoreId(GetArealByCityName(cA)); // фолбэк на берег колонии-якоря
	if (sShore == "") return; // без берега маршрута нет - разрулит месячный таймаут GWIK_RansomTimeout
	int days = GetMaxDaysFromIsland2Island(GetArealByCityName(fromCity), GetArealByCityName(toCity)) + 3;
	if (days < 6) days = 6;
	Map_CreateTrader(sShore, toCity, "GWIK_RansomShip", days);
}

bool GWIK_RansomForceSpawn(string sAnchorCity) // Gregg: пиратский корабль с пленником ПАТРУЛИРУЕТ квестовым энкаунтером МИРОВОЙ КАРТЫ; перехватить и взять абордажем (потопишь - капитан гибнет, месяц не поймал - потерян). sAnchorCity - колония-якорь патруля: "Pirates" = у Бермуд (силовой путь посредника), "" = случайная колония (вариант «на борту пирата»). Образец: Portmans_SeekShip (Character_Capture/Character_sink)
{
	// фиксированный id (без суффикса) - его ждут Map_CreateTrader / win_condition / Map_TraderSucces_quest
	ref sld = GetCharacter(NPC_GenerateCharacter("GWIK_RansomShip", "", "man", "man", sti(pchar.rank), PIRATE, -1, true));
	SetCaptanModelByEncType(sld, "trade");
	DeleteAttribute(sld, "Ship");
	SetShipToFantom(sld, "war", true);
	// тип корабля противника: на класс слабее корабля ГГ, но не слабее 6 класса (кл.6 - самый мелкий «боевой»)
	int iEClass = GetCharacterShipClass(pchar) + 1;
	if (iEClass > 6) iEClass = 6;
	int iEType = GWIK_RandShipTypeByClass(iEClass);
	if (iEType < 0) iEType = SHIP_SLOOP; // фолбэк, если класс пуст
	sld.Ship.Type = GenerateShipExt(iEType, true, sld);
	SetBaseShipData(sld);
	SetCrewQuantityFull(sld);
	Fantom_SetCannons(sld, "war"); // Gregg: калибр под фактический корпус (иначе остаётся от случайного корабля SetShipToFantom)
	Fantom_SetBalls(sld, "war"); // Gregg: и боезапас/порох под фактический корпус (тем же корнем был занижен)
	SetRandomNameToCharacter(sld);                        // случайный капитан-пират
	sld.Ship.Name = generateRandomNameToShip(PIRATE);     // случайное имя корабля
	if (CheckAttribute(pchar, "GWIK.Job.Mutiny")) GWIK_StampMutineer(sld, false); // Gregg: бунт - капитан перехваченного корабля = беглец из таверны (имя+модель+портрет; боевой уровень оставляем масштабированным под ГГ)
	if (CheckAttribute(pchar, "GWIK.Job.Mutiny")) sld.DontRansackCaptain = true; // Gregg: бунт - судьбу беглеца решаем в каютном диалоге (QuestSurrender); штатная форма захвата капитана НЕ трогает, иначе получим ВТОРОГО пленника (образец RockBrasilian)
	pchar.GWIK.Job.ForceCapName = GetFullName(sld);
	sld.Ship.Mode = "War";
	sld.AlwaysSandbankManeuver = true;
	sld.AlwaysEnemy = true; // сорвётся в бой сам
	SetRandGeraldSail(sld, sti(sld.Nation));
	Group_FindOrCreateGroup("GWIK_RansomShipGroup");
	Group_SetType("GWIK_RansomShipGroup", "war");
	Group_AddCharacter("GWIK_RansomShipGroup", sld.id);
	Group_SetGroupCommander("GWIK_RansomShipGroup", sld.id);
	// патруль ПИРАТА между двумя портами (коридор), а не односторонний рейс - иначе «уходил в порт» и терял капитана раньше, чем игрок перехватит
	string sPatrolA = sAnchorCity;
	if (sPatrolA == "" || sPatrolA == "none") sPatrolA = SelectAnyColony("Pirates"); // пусто => случайная колония (вариант «на борту пирата»)
	if (sPatrolA == "none") return false; // некуда - посредник ответит фолбэком
	pchar.GWIK.Job.PirateCity = sPatrolA;
	string sPatrolB = GWIK_PickPatrolPartner(sPatrolA); // второй конец коридора (ближайшая англо/франц. колония)
	if (sPatrolB != "") pchar.GWIK.Job.PirateCity2 = sPatrolB;
	DeleteAttribute(pchar, "GWIK.Job.PirateLeg");
	sld.mapEnc.type = "trade";
	sld.mapEnc.worldMapShip = "Galleon_red";
	sld.mapEnc.Name = "Корабль '" + sld.Ship.Name + "'";
	if (LanguageGetLanguage() != "russian") sld.mapEnc.Name = "Ship '" + sld.Ship.Name + "'";
	pchar.GWIK.Job.ForceShipId = sld.id;
	pchar.GWIK.Job.ForceShipName = sld.Ship.Name;
	pchar.GWIK.Job.ForceShipType = GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName)); // Gregg: локализованный ТИП корабля для журнала (номинатив строчными, как GWIK_RivalShipStr) - аппозитив «его судно - <тип> 'имя'»; падежные суффиксы common.ini неполны (в EN Voc меньше форм), поэтому номинатив
	pchar.GWIK.Job.Force = true;
	// Gregg: вариант «пленник на борту курсирующего пирата» (якорь пустой, колония случайная) - в море НЕ сразу, как и у конкурента:
	// двое суток судно стоит в порту. Иначе поручение выдавалось на корабль, который уже мотается по коридору далеко от конторы.
	// Силовой путь посредника (якорь "Pirates", у Бермуд) уходит сразу - там по тексту «якорь уже поднимают», и игрок рядом
	if (sAnchorCity == "")
	{
		pchar.GWIK.Job.PirateInPort = true;
		DeleteAttribute(pchar, "quest.GWIK_RansomPutToSea.over"); // иначе перевзвод таймера мёртв (over от прошлого поручения)
		SetTimerFunction("GWIK_RansomPutToSea", 0, 0, 2);
	}
	else
	{
		GWIK_RansomDispatchLeg(); // первый отрезок патруля (дальше перезапуск при каждом «прибытии»)
	}
	// прерывания: захват абордажем = спасение, потопление = провал (Character_Capture/Character_sink - см. quests_check.c)
	DeleteAttribute(pchar, "quest.GWIK_RansomBoarded.over");
	DeleteAttribute(pchar, "quest.GWIK_RansomSunk.over");
	DeleteAttribute(pchar, "quest.GWIK_RansomTimeout.over");
	pchar.quest.GWIK_RansomBoarded.win_condition.l1 = "Character_Capture";
	if (CheckAttribute(pchar, "GWIK.Job.Mutiny")) pchar.quest.GWIK_RansomBoarded.win_condition.l1 = "NPC_Death"; // Gregg: бунт - капитан и есть цель; NPC_Death ловит и плен (QuestSurrender мимо формы захвата), и казнь в каюте; смерть от потопления отсекаем в хендлере (там же Character_sink = провал)
	pchar.quest.GWIK_RansomBoarded.win_condition.l1.character = sld.id;
	pchar.quest.GWIK_RansomBoarded.win_condition = "GWIK_RansomBoarded";
	pchar.quest.GWIK_RansomBoarded.function = "GWIK_RansomBoarded";
	pchar.quest.GWIK_RansomSunk.win_condition.l1 = "Character_sink";
	pchar.quest.GWIK_RansomSunk.win_condition.l1.character = sld.id;
	pchar.quest.GWIK_RansomSunk.win_condition = "GWIK_RansomSunk";
	pchar.quest.GWIK_RansomSunk.function = "GWIK_RansomSunk";
	SetTimerFunction("GWIK_RansomTimeout", 0, 1, 0); // подстраховка от зависания (месяц): если так и не перехватил
	return true;
}

void GWIK_RansomPutToSea(string qName) // Gregg: двое суток истекли - пиратское судно с пленником выходит из порта и начинает патруль (зеркало GWIK_RivalPutToSea)
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "ransom") return;
	if (CheckAttribute(pchar, "GWIK.Job.Rescued")) return; // капитан уже спасён
	if (!CheckAttribute(pchar, "GWIK.Job.PirateInPort")) return; // уже в море (или поручение другое) - таймер пустой
	DeleteAttribute(pchar, "GWIK.Job.PirateInPort");
	GWIK_RansomDispatchLeg(); // первый отрезок патруля (дальше перезапуск при каждом «прибытии»)
	string sPort = "";
	if (CheckAttribute(pchar, "GWIK.Job.PirateCity")) sPort = XI_ConvertString("Colony" + pchar.GWIK.Job.PirateCity + "Gen");
	if (LanguageGetLanguage() == "russian") Log_Info("Пиратское судно с пленником вышло из порта " + sPort + ".");
	else Log_Info("The pirate ship with the captive has left the port of " + sPort + ".");
}

void GWIK_RansomBoarded(string qName) // капитан Компании освобождён (обычный силовой путь, Character_Capture) ЛИБО судьба беглеца решена в каюте (бунт, NPC_Death)
{
	int iCap;
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "ransom") return;
	if (CheckAttribute(pchar, "GWIK.Job.Rescued")) return;
	// Gregg: в бунте хендлер висит на NPC_Death беглеца. Смерть БЕЗ каютного выбора (потоплен / погиб до каюты) - не наш случай: пусть ловит GWIK_RansomSunk. Проверяем ДО ForceCleanup, иначе снимем прерывание потопления
	if (CheckAttribute(pchar, "GWIK.Job.Mutiny") && !CheckAttribute(pchar, "GWIK.Job.MutinyFate")) return;
	GWIK_RansomForceCleanup(false); // корабль захвачен - карт-энкаунтер израсходован, не трогаем
	// бунт, «зарубить» в каюте (GWIK_MutineerCabin_Kill): пленника НЕТ, только доклад о казни в конторе
	if (CheckAttribute(pchar, "GWIK.Job.MutinyFate") && pchar.GWIK.Job.MutinyFate == "killed")
	{
		pchar.GWIK.Job.MutinyKilled = true; // доложить о казни (узел GWIK_MutineerKilledReport), пассажира нет
		AddQuestRecord("GWIK_RansomJob", "18");
		if (CheckAttribute(pchar, "GWIK.Job.ForceShipName")) AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName); // строго после AddQuestRecord
		if (CheckAttribute(pchar, "GWIK.Job.ForceShipType")) AddQuestUserData("GWIK_RansomJob", "sType", pchar.GWIK.Job.ForceShipType);
		if (LanguageGetLanguage() == "russian")
		{
			log_info("Беглый капитан оказал сопротивление и зарублен в каюте.");
			log_info("Доложите о случившемся в любой конторе ГВИК.");
		}
		else
		{
			log_info("The runaway captain resisted and was cut down in his cabin.");
			log_info("Report the matter at any GWIC office.");
		}
		return;
	}
	// бунт, «в плен» (GWIK_MutineerCabin_Prisoner -> QuestAboardCabinDialogQuestSurrender): усыновляем ТОГО САМОГО захваченного капитана (личность скопирована с GWIK_RansomShip), свежего НЕ спавним - иначе дубль пленника
	if (CheckAttribute(pchar, "GWIK.Job.Mutiny"))
	{
		iCap = -1;
		if (CheckAttribute(pchar, "GenQuest.LastQuestPrisonerIdx")) iCap = sti(pchar.GenQuest.LastQuestPrisonerIdx);
		if (iCap >= 0)
		{
			ref cap = GetCharacter(iCap);
			pchar.GWIK.Job.CaptiveId = cap.id;     // доставляем ЕГО же - тот, кого взяли на абордаж
			SetCharacterRemovable(cap, false);     // квестовый пленник (QuestSurrender уже поставил - дублируем безопасно)
			DeleteAttribute(cap, "Hold_GenQuest"); // снять возможный откупной хук (SetCharToPrisoner ставит его с шансом 1/2)
			GWIK_StampMutineer(cap, false); // Gregg: перебить лицо/имя/модель снимком беглеца - SetCharToPrisoner прогнал FaceMaker и подменил FaceId дефолтом 478 («раб в кандалах»); FaceId пинуем вручную (портрет рисуется живьём из FaceId)
			pchar.GWIK.Job.Rescued = true;
		}
		else
		{
			GWIK_RansomPay(); // фолбэк, если индекс пленника потерялся (не должно случаться) - спавним слепок под беглеца
		}
		AddQuestRecord("GWIK_RansomJob", "19");
		if (CheckAttribute(pchar, "GWIK.Job.ForceShipName")) AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName); // строго после AddQuestRecord
		if (CheckAttribute(pchar, "GWIK.Job.ForceShipType")) AddQuestUserData("GWIK_RansomJob", "sType", pchar.GWIK.Job.ForceShipType);
		if (LanguageGetLanguage() == "russian")
		{
			log_info("Беглый капитан закован в кандалы.");
			log_info("Доставьте его в любую контору ГВИК.");
		}
		else
		{
			log_info("The runaway captain is clapped in irons.");
			log_info("Deliver him to any GWIC office.");
		}
		return;
	}
	// обычный силовой путь (не бунт): освобождённый капитан Компании - свежий пассажир
	GWIK_RansomPay();
	AddQuestRecord("GWIK_RansomJob", "6");
	if (CheckAttribute(pchar, "GWIK.Job.ForceShipName")) AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName); // строго после AddQuestRecord
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Пиратский корабль взят на абордаж, капитан свободен.");
		log_info("Доставьте его в любую контору ГВИК.");
	}
	else
	{
		log_info("The pirate ship is boarded, the Company captain is free.");
		log_info("Deliver him to any GWIC office.");
	}
}

void GWIK_RansomSunk(string qName) // шлюп потоплен - капитан утонул вместе с ним, провал с позором
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "ransom") return;
	if (CheckAttribute(pchar, "GWIK.Job.Rescued")) return;
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Пиратский корабль пошёл ко дну вместе с пленником.");
		log_info("Поручение Компании провалено.");
	}
	else
	{
		log_info("The pirate ship went down with the captive aboard.");
		log_info("The Company's assignment has failed.");
	}
	GWIK_RansomForceFail("7");
}

void GWIK_RansomForceEscape() // шлюп ушёл в порт - капитана увезли вглубь пиратских вод, провал (зовётся из Map_TraderSucces_quest)
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "ransom") return;
	if (!CheckAttribute(pchar, "GWIK.Job.Force")) return;
	if (CheckAttribute(pchar, "GWIK.Job.Rescued")) return;
	Map_ReleaseQuestEncounter("GWIK_RansomShip");
	if (LanguageGetLanguage() == "russian")
	{
		log_info("Пиратский корабль с пленником ушёл в порт - вы его упустили.");
		log_info("Поручение Компании провалено.");
	}
	else
	{
		log_info("The pirate ship with the captive reached port - you lost it.");
		log_info("The Company's assignment has failed.");
	}
	GWIK_RansomForceFail("8");
}

void GWIK_RansomTimeout(string qName) // подстраховка: шлюп так и не перехвачен за месяц - капитан потерян
{
	if (!CheckAttribute(pchar, "GWIK.Job")) return;
	if (pchar.GWIK.Job != "ransom") return;
	if (!CheckAttribute(pchar, "GWIK.Job.Force")) return;
	if (CheckAttribute(pchar, "GWIK.Job.Rescued")) return;
	Map_ReleaseQuestEncounter("GWIK_RansomShip");
	if (LanguageGetLanguage() == "russian")
	{
		log_info("След пиратского корабля с пленником потерян.");
		log_info("Поручение Компании провалено.");
	}
	else
	{
		log_info("The trail of the pirate ship with the captive has gone cold.");
		log_info("The Company's assignment has failed.");
	}
	GWIK_RansomForceFail("8");
}
//<-- поручения контор ГВИК

//<-- ГВИК

void WayBeginning(string _tmp)
{
	DeleteAttribute(pchar,"cirassID");
	DeleteAttribute(&InterfaceStates,"DisFastTravel");
	initNewMainCharacter();
	InitPerks();

	Pchar.quest.AlcoveTP.win_condition.l1 = "location";
	Pchar.quest.AlcoveTP.win_condition.l1.location = "Treasure_alcove";
	Pchar.quest.AlcoveTP.win_condition = "recharge_portals";

	Pchar.quest.Labrnt.win_condition.l1 = "location";
	Pchar.quest.Labrnt.win_condition.l1.location = "Labirint_1";
	Pchar.quest.Labrnt.win_condition = "SpookyScarySkeleton";
	Pchar.quest.Labrnt2.win_condition.l1 = "location";
	Pchar.quest.Labrnt2.win_condition.l1.location = "Labirint_2";
	Pchar.quest.Labrnt2.win_condition = "SpookyScarySkeleton";
	Pchar.quest.Labrnt3.win_condition.l1 = "location";
	Pchar.quest.Labrnt3.win_condition.l1.location = "Labirint_3";
	Pchar.quest.Labrnt3.win_condition = "SpookyScarySkeleton";

	for (int k = 0; k < 99; k++)//рандомизация сундуков НИ
	{
		chest_ammo[k] = rand(999);
		chest_treasure[k] = rand(999);
		chest_quest[k] = rand(999);
		chest_craftsmans[k] = rand(999);
		chest_ammo2[k] = rand(999);
		chest_treasure2[k] = rand(999);
		chest_quest2[k] = rand(999);
		chest_craftsmans2[k] = rand(999);
	}
	for (k = 1; k < 31; k++)
	{
		shipspool[k] = 0;
	}
	int value;

	for (k = 1; k < 31; k++)
	{
		if (k >= 1 && k < 6)
		{
			value = rand(11);
			if (sixth[value] != 0)
			{
				shipspool[k] = sixth[value];
				sixth[value] = 0;
			}
			else
			{
				while (shipspool[k] == 0)
				{
					value = rand(11);
					shipspool[k] = sixth[value];
					sixth[value] = 0;
				}
			}
		}
		if (k >= 6 && k < 11)
		{
			value = rand(12);
			if (fifth[value] != 0)
			{
				shipspool[k] = fifth[value];
				fifth[value] = 0;
			}
			else
			{
				while (shipspool[k] == 0)
				{
					value = rand(12);
					shipspool[k] = fifth[value];
					fifth[value] = 0;
				}
			}
		}
		if (k >= 11 && k < 16)
		{
			value = rand(23);
			if (fourth[value] != 0)
			{
				shipspool[k] = fourth[value];
				fourth[value] = 0;
			}
			else
			{
				while (shipspool[k] == 0)
				{
					value = rand(23);
					shipspool[k] = fourth[value];
					fourth[value] = 0;
				}
			}
		}
		if (k >= 16 && k < 21)
		{
			value = rand(31);
			if (third[value] != 0)
			{
				shipspool[k] = third[value];
				third[value] = 0;
			}
			else
			{
				while (shipspool[k] == 0)
				{
					value = rand(31);
					shipspool[k] = third[value];
					third[value] = 0;
				}
			}
		}
		if (k >= 21 && k < 26)
		{
			value = rand(20);
			if (second[value] != 0)
			{
				shipspool[k] = second[value];
				second[value] = 0;
			}
			else
			{
				while (shipspool[k] == 0)
				{
					value = rand(20);
					shipspool[k] = second[value];
					second[value] = 0;
				}
			}
		}
		if (k >= 26 && k < 31)
		{
			value = rand(19);
			if (first[value] != 0)
			{
				shipspool[k] = first[value];
				first[value] = 0;
			}
			else
			{
				while (shipspool[k] == 0)
				{
					value = rand(19);
					shipspool[k] = first[value];
					first[value] = 0;
				}
			}
		}
		Log_testinfo(k+" "+shipspool[k]);
	}
	skillz[0] = "FencingLight";
	skillz[1] = "Fencing";
	skillz[2] = "FencingHeavy";
	skillz[3] = "Pistol";
	skillz[4] = "Fortune";
	skillz[5] = "Leadership";
	skillz[6] = "Commerce";
	skillz[7] = "Accuracy";
	skillz[8] = "Cannons";
	skillz[9] = "Sailing";
	skillz[10] = "Repair";
	skillz[11] = "Grappling";
	skillz[12] = "Defence";
	skillz[13] = "Sneak";

	pchar.chest_ammo = 0;
	pchar.chest_treasure = 0;
	pchar.chest_quest = 0;
	pchar.chest_craftsmans = 0;
	pchar.chest_ammo2 = 0;
	pchar.chest_treasure2 = 0;
	pchar.chest_quest2 = 0;
	pchar.chest_craftsmans2 = 0;

	if (startHeroType == 1) //21/07/07 homo Питер Блад
    {
    	pchar.quest.Tut_start.win_condition.l1          = "location";
    	pchar.quest.Tut_start.win_condition.l1.location = "Estate";
    	pchar.quest.Tut_start.function                  = "Blood_StartGame";
		InterfaceStates.startGameWeather = FindWeather("20 Hour");
		DoQuestReloadToLocation("Estate", "reload", "reload1", "");
		return;
    }
	InitPsHeros();
	ReloadProgressUpdate();
	for (int j = 0; j< MAX_COLONIES; j++)
	{
		GenerateIslandShips(colonies[j].island);
	}
	ReloadProgressUpdate();
    if (startHeroType == 2)
    {
    	pchar.quest.Tut_start.win_condition.l1          = "location";
    	pchar.quest.Tut_start.win_condition.l1.location = "Cyberpunk2077";
    	pchar.quest.Tut_start.function                  = "Whisper_StartGame";
		DoQuestReloadToLocation("Cyberpunk2077", "reload", "reload2_back", "");
		return;
    }
	if (startHeroType == 3)		//Анжелика Тич
    {
    	pchar.quest.Tut_start.win_condition.l1          = "location";
    	pchar.quest.Tut_start.win_condition.l1.location = "LaVega_MH12";
    	pchar.quest.Tut_start.function                  = "AnjelikaTich_StartGame";
		DoQuestReloadToLocation("LaVega_MH12", "reload", "reload3", "");
		return;
    }
	if (startHeroType == 4 || startHeroType == 5)	//Дети Николаса Шарпа
    {
    	pchar.quest.Tut_start.win_condition.l1          = "location";
    	pchar.quest.Tut_start.win_condition.l1.location = "Shore57";
    	pchar.quest.Tut_start.function                  = "Sharp_StartGame";
		DoQuestReloadToLocation("Shore57", "item", "item11", "");
		return;
    }
	if (startHeroType == 6 || startHeroType == 7)	//Шарль и Мэри
    {
    	pchar.quest.Tut_start.win_condition.l1          = "location";
    	pchar.quest.Tut_start.win_condition.l1.location = "Shore_ship2";
    	pchar.quest.Tut_start.function                  = "SharleMary_StartGame";
		DoQuestReloadToLocation("Shore_ship2", "goto", "goto4", "");
		return;
    }
	if (startHeroType == 8)		//Весёлый Роджер
    {
    	pchar.quest.Tut_start.win_condition.l1          = "location";
    	pchar.quest.Tut_start.win_condition.l1.location = "Temple_skulls";
    	pchar.quest.Tut_start.function                  = "Undead_StartGame";
		DoQuestReloadToLocation("Temple_skulls", "goto", "goto2", "");
		return;
    }
	if (MOD_BETTATESTMODE == "On")
	{
		if (startHeroType == 66)		//Тестовый запуск
		{
			pchar.quest.Tut_start.win_condition.l1          = "location";
			pchar.quest.Tut_start.win_condition.l1.location = PChar.location;
			pchar.quest.Tut_start.function                  = "TEST_StartGame";
			DoQuestReloadToLocation("DesMoines_port", "reload", "reload1", "TEST_StartGame");
			return;
		}
	}
	if (startHeroType > 8)		//Все остальные
    {
    	// Gregg: альтернативный пролог «Необитаемый остров» (перенос из К:ПС) - третий вариант
    	// начала игры: ГГ приходит в себя на берегу Доминики без корабля, команды и памяти.
    	// Проверяется первым: если выбран пролог, галка «старт без корабля» не рассматривается.
    	if (sti(NullCharacter.Start.DDPrologue) == 1)
    	{
    		pchar.quest.Tut_start.win_condition.l1          = "location";
    		pchar.quest.Tut_start.win_condition.l1.location = "Shore27";
    		pchar.quest.Tut_start.function                  = "StartDamnedDestiny";
			DoQuestReloadToLocation("Shore27", "reload", "reload1", "");
			return;
    	}
    	if (sti(NullCharacter.Start.NoStartShip) == 1)	// LEO: старт без корабля — пропуск палубы Сандро Торна, ГГ сразу в город (но с раздачей характеристик)
    	{
    		pchar.quest.Tut_start.win_condition.l1          = "location";
    		pchar.quest.Tut_start.win_condition.l1.location = pchar.HeroParam.Location;
    		pchar.quest.Tut_start.function                  = "Tut_StartGame_NoShip";
			DoQuestReloadToLocation(pchar.HeroParam.Location, pchar.HeroParam.Group, pchar.HeroParam.Locator, "");
			return;
    	}
    	pchar.quest.Tut_start.win_condition.l1          = "location";
    	pchar.quest.Tut_start.win_condition.l1.location = "Ship_deck_Low";
    	pchar.quest.Tut_start.function                  = "Tut_StartGame";
		DoQuestReloadToLocation("Ship_deck_low", "goto", "goto4", "");
    }
}

/////////////////////////////////////////////////////////////////////////////////
// Раздача еды
/////////////////////////////////////////////////////////////////////////////////
void SetFoodToCharacter(ref chr, int iQuantity, int iFoodQuantity)
{
	int d = 1;
	int iSum = 0;
	string sFood = "";
	for(int f = 1; f <= iQuantity; f++)
	{
		sFood = "Food" + f;
		if(CheckAttribute(chr, "items." + sFood))
		{
			DeleteAttribute(chr, "items." + sFood);
		}

		switch(f)
		{
			case 1: d = 12; break;
			case 2: d = 14; break;
			case 3: d = 16; break;
			case 4: d = 18; break;
			case 5: d = 20; break;
		}

		iSum = makeint(iFoodQuantity / d)+1;
		TakeNItems(chr, sFood, iSum);
	}
}


void SetEnergyQty(ref chr, float fQty, float fQtyMax)
{
	chr.chr_ai.energy = fQty;
	chr.chr_ai.energyMax = fQtyMax;
}

/////////////////////////////////////////////////////////////////////////////////
// Переключение режима абордажа
/////////////////////////////////////////////////////////////////////////////////
void GetBoardModeToLocation(ref loc)
{
	if(!CheckAttribute(loc, "CheckBoardMode"))
	{
		return;
	}

	string sFile = "DeckLowVSMedium_locators";

	switch(loc.id)
	{
		case "BOARDING_SMALL_DECK":
			if(sti(InterfaceStates.ShowBoardMode) == 1)
			{
				sFile = "DeckLowVSMedium_locators_lugger";
			}
			else
			{
				sFile = "DeckLowVSMedium_locators";
			}
		break;

		case "BOARDING_MEDIUM_DECK":
			if(sti(InterfaceStates.ShowBoardMode) == 1)
			{
				sFile = "deckLowVSBig_locators_lugger";
			}
			else
			{
				sFile = "deckLowVSBig_locators";
			}
		break;

		case "BOARDING_BIG_DECK":
			if(sti(InterfaceStates.ShowBoardMode) == 1)
			{
				sFile = "deckMediumVSBig_locators_lugger";
			}
			else
			{
				sFile = "deckMediumVSBig_locators";
			}
		break;

		case "BOARDING_CRVT_FRGT_DECK":
			if(sti(InterfaceStates.ShowBoardMode) == 1)
			{
				sFile = "deckCorvetteVSFrigate_locators_lugger";
			}
			else
			{
				sFile = "deckCorvetteVSFrigate_locators";
			}
		break;

		case "BOARDING_LNSHP_FRGT_DECK":
			if(sti(InterfaceStates.ShowBoardMode) == 1)
			{
				sFile = "deckLineshipVSFrigate_locators_lugger";
			}
			else
			{
				sFile = "deckLineshipVSFrigate_locators";
			}
		break;
		
		case "Boarding_GunDeck":
			if(sti(InterfaceStates.ShowBoardMode) == 1)
			{
				sFile = "Gdeck_locators_lugger";
			}
			else
			{
				sFile = "GDeck_locators";
			}
		break;
	}

	loc.models.always.locators = sFile;

	Log_TestInfo(GetConvertStrWithReplace("Variable_utils_3", "Logs.txt", "#space#", " "));
	Log_TestInfo(GetConvertStrWithReplace("Variable_utils_4", "Logs.txt", "#space#", " ") + loc.id);
	Log_TestInfo(GetConvertStrWithReplace("Variable_utils_5", "Logs.txt", "#space#", " ") + loc.models.always.locators);
}


void Log_InfoS(string sText)
{
	PlayStereoSound("new_round");
	Log_Info(sText);
}

int GetRank(ref chr, int step)
{
	int iRank = sti(chr.rank) + step + (sti(PChar.rank)); // LEO: Какая то динамика в сложности

	if(iRank < 1)
	{
		iRank = 1;
	}

	return iRank;
}

ref PCharLoc()
{
	int iLocation = FindLocation(PChar.location);
	return &Locations[iLocation];
}

// LEO: функция обещает ФАНТОМОВ, а уводила к морю всех, кроме ГГ, офицеров и NotRemoved - вместе с
//   массовкой вымывало квестовых NPC (индеец Аптекаря). Одноразовых видно по меткам генераторов
//   (scripts/utils.c): PhantomType - городская массовка, LifeDay - временный персонаж со сроком жизни
//   (рабы Каймана, носильщики). Постоянные именные рождаются с LifeDay = -1 и теперь остаются.
void DeleteAllFantomCharactersFromLocation(ref loc)
{
	ref chr;
	for(int n=0; n<MAX_CHARACTERS; n++)
	{
		if(n == nMainCharacterIndex) continue;
		makeref(chr,Characters[n]);
		if(!CheckAttribute(chr, "id")) continue;
		if(!CheckAttribute(chr, "location")) continue;
		if(chr.location != loc.id) continue;
		if(CheckAttribute(chr, "NotRemoved")) continue;
		if(IsOfficer(chr)) continue;
		if(!CheckAttribute(chr, "PhantomType"))
		{
			if(!CheckAttribute(chr, "LifeDay")) continue; // постоянный именной NPC - не наше дело
		}
		LAi_SetActorType(chr);
		LAi_ActorGoToLocation(chr,"reload","sea","none","","","",10.0);
	}
}

void AddSimpleRumourToAllNations(string sText, int iTime, int iQty)
{
	for(int i=0; i<MAX_NATIONS; i++)
	{
		AddSimpleRumour(sText, i, iTime, iQty);
	}
}

void SetGeraldDetails(ref chr, int iHull, int iColor, string sType, string sGerald)
{
	if(!CheckAttribute(chr, "Ship.Type")) { return; }
	if(sti(chr.Ship.Type) >= REAL_SHIPS_QUANTITY) { return; }

	ref rShip = GetRealShip(sti(chr.Ship.Type));

	if(iHull != -1 && !CheckAttribute(rShip, "DisableChangedHull"))
	{
		rShip.Ship.Upgrades.Hull = iHull;
	}

	if(sType != "")
	{
		rShip.EmblemedSails.normalTex = "ships\" + sType + ".tga";
	}

	if(iColor != -1)
	{
		rShip.ship.sailscolor = SailsColors[iColor].color;
	}

	if(sGerald != "")
	{
		rShip.GeraldName = sGerald;
		rShip.ShipSails.gerald_name = "Ships\Gerald\" + sGerald + ".dds";
	}
}

// Gregg: единый источник моделей рабов/пленников (было 5 копий массива model[0..22] в Addons)
string GetSlaveModel(int idx)
{
	string model[23];
	model[0] = "prizon_1";
	model[1] = "prizon_2";
	model[2] = "prizon_3";
	model[3] = "prizon_4";
	model[4] = "prizon_5";
	model[5] = "prizon_6";
	model[6] = "prizon_7";
	model[7] = "prizon_8";
	model[8] = "pirate_1";
	model[9] = "prison_5";
	model[10] = "pirate_1";
	model[11] = "pirate_11";
	model[12] = "pirate_12";
	model[13] = "pirate_13";
	model[14] = "pirate_14";
	model[15] = "pirate_15";
	model[16] = "pirate_16";
	model[17] = "pirate_21";
	model[18] = "pirate_25";
	model[19] = "PKM_rab_1";
	model[20] = "PKM_rab_2";
	model[21] = "PKM_rab_3";
	model[22] = "PKM_rab_4";
	return model[idx];
}

// LEO: строитель колонии стоит в бухте в ГРУППЕ ИГРОКА, и любая драка на берегу (налёт охотников
//   за головами и т.п.) переводит его в warrior. Бой кончился, а он так и остался воином: молчит,
//   диалога нет, стройка встаёт намертво (репорт по Альбрехту, откаты сейвов не помогали).
//   Возвращаем его в разговорчивое состояние при входе в локацию - драка к этому моменту уже позади.
void RestoreColonyBuilder(ref loc)
{
	int idx = GetCharacterIndex("Builder");
	if (idx < 0) return;
	ref bld = GetCharacter(idx);
	if (!CheckAttribute(bld, "location")) return;
	if (bld.location != loc.id) return;
	if (LAi_IsDead(bld)) return;
	if (!CheckAttribute(bld, "chr_ai.type")) return;
	if (bld.chr_ai.type != LAI_TYPE_WARRIOR) return;
	LAi_SetCitizenType(bld);
	LAi_group_MoveCharacter(bld, LAI_GROUP_PLAYER);
}

void CreateCaimanShoreSlaves(ref loc)
{
	Log_TestInfo(PChar.location);
	if(PChar.ColonyBuilding.SlavesInShore.CurShore != loc.id) { return; }

    int iMassive;

	int iQuantity = 10;
	if(PChar.ColonyBuilding.Stage != "0")
	{
		iQuantity += 5;
	}

	ref sld;
	for(int i=1; i <= iQuantity; i++)
	{
	    iMassive = rand(22);

    	string sAni = "man";
		sld = GetCharacter(NPC_GenerateCharacter("FantomSlaveInCaimanShore_" + i, GetSlaveModel(iMassive), "man", sAni, 1, PIRATE, 0, false));
		sld.dialog.filename = "Common_Builder.c";
        sld.greeting = "Gr_slave";
		sld.dialog.currentnode = "ForSlave";
		sld.PoorSit = true;
		LAi_SetLoginTime(sld, 6.0, 22.99);
		LAi_SetCitizenTypeNoGroup(sld);
		LAi_SetImmortal(sld, true);
		LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
		PlaceCharacter(sld, "officers", "random_must_be_near");
	}
}

ref LocFromID(string sLocation)
{
	int iLocation = FindLocation(sLocation);
	return &Locations[iLocation];
}

void SetQuestBoxItems(string sLocation, string sItem, int Qty)
{
	string sBox = "box1";
	if(sItem == "SuccessPrice_Chest")
	{
		sBox = "box8";
	}

	PChar.GenQuestBox.(sLocation) = true;
	PChar.GenQuestBox.(sLocation).(sBox).items.(sItem) = Qty;
	PChar.GenQuestBox.(sLocation).stay = true;
}

void ClearQuestBoxStay(string sLocation)
{
	if(!CheckAttribute(PChar, "GenQuestBox." + sLocation))
	{
		return;
	}

	DeleteAttribute(PChar, "GenQuestBox." + sLocation);
}

void SetDialogCharacter2Character(ref chr, ref achr, float fTime, bool bActor)
{
	LAi_SetActorType(achr);

	if(bActor)
	{
		LAi_SetActorType(chr);
		LAi_ActorWaitDialog(chr, achr);
	}

	LAi_ActorDialog(achr, chr, "", fTime, 1.0);
}

void DeleteAllOfficersFromLocation()
{
	int iOfficer = 0;
	if(GetOfficersQuantity(PChar) > 0)
	{
		for(int off=0; off <= MAX_NUM_FIGHTERS; off++)
		{
			iOfficer = GetOfficersIndex(PChar, off);
			if(iOfficer != -1 && iOfficer != nMainCharacterIndex)
			{
				LogoffCharacter(&Characters[iOfficer]);
			}
		}
	}
}

int FindShipFromClass(int iClass, string sType)
{
	if(sType == "none" || sType == "")
	{
		return 0;
	}

	int iShip = 0;
	switch(sType)
	{
		case "Fast":
			switch(iClass)
			{
				case 7: return SHIP_FELUCA; break;
				case 6: return 3 + rand(11); break;
				case 5: return 15 + rand(12); break;
				case 4: return 28 + rand(23); break;
				case 3: return 52 + rand(31); break;
				case 2: return 84 + rand(20); break;
				case 1: return 84 + rand(40); break;
			}
		break;

		case "Trader":
			switch(iClass)
			{
				case 7: return SHIP_FELUCA; break;
				case 6: return 3 + rand(11); break;
				case 5: return 15 + rand(12); break;
				case 4: return 28 + rand(23); break;
				case 3: return 52 + rand(31); break;
				case 2: return 84 + rand(20); break;
				case 1: return 84 + rand(40); break;
			}
		break;

		case "All":
			switch(iClass)
			{
				case 7: return SHIP_FELUCA; break;
				case 6: return 3 + rand(11); break;
				case 5: return 15 + rand(12); break;
				case 4: return 28 + rand(23); break;
				case 3: return 52 + rand(31); break;
				case 2: return 84 + rand(20); break;
				case 1: return 84 + rand(40); break;
			}
		break;
	}
	return 0;
}

int GetMastQuantityFromShip(ref chr)
{
	if(!CheckAttribute(chr, "ship")) { return 0; }
	if(!CheckAttribute(chr, "ship.masts")) { return 0; }
	if(sti(chr.Ship.Type) >= REAL_SHIPS_QUANTITY) { return 0; }

	aref ar;
	makearef(ar, chr.ship.masts);

	return GetAttributesNum(ar);
}

void SetQuestFunction(string sQuest, string sFunction, string sType, string sName)
{
	switch(sType)
	{
		case "efl":
			PChar.quest.(sQuest).win_condition.l1 = "ExitFromLocation";
			PChar.quest.(sQuest).win_condition.l1.location = sName;
			PChar.quest.(sQuest).function = sFunction;
		break;

		case "l":
			PChar.quest.(sQuest).win_condition.l1 = "location";
			PChar.quest.(sQuest).win_condition.l1.location = sName;
			PChar.quest.(sQuest).function = sFunction;
		break;

		case "i":
			PChar.quest.(sQuest).win_condition.l1 = "item";
			PChar.quest.(sQuest).win_condition.l1.item = sName;
			PChar.quest.(sQuest).function = sFunction;
		break;
	}
}
