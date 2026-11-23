string GetIslandForTreasure()
{
	int iNum, m;
	ref Itm;
	aref arDest, arImt;
	string sAttr;

	m = 0;
	string sCurIsland = GetCharacterCurrentIslandId(pchar);
	makearef(arDest, NullCharacter.TravelMap.Islands);
	iNum = GetAttributesNum(arDest);

	string sArray[50];// динамические массивы в Шторме не организуешь :(
	for (int i = 0; i<iNum; i++)
	{
		arImt = GetAttributeN(arDest, i);
		sAttr = GetAttributeName(arImt);
		if(CheckAttribute(&NullCharacter,"TravelMap.Islands." + sAttr + ".Treasure"))
		{
			if(sCurIsland == sAttr || CheckTreasureMaps(sAttr)) continue;
			else
			{
				sArray[m] = sAttr;
				m++;
			}
		}
	}
	m = rand(m-1);
	return sArray[m];
}

bool CheckTreasureMaps(string sIsland)
{
	ref Itm;

	if(GetCharacterItem(pchar,"mapQuest") > 0)
	{
		itm = ItemsFromID("mapQuest");
		if(CheckAttribute(itm, "MapIslId") && itm.MapIslId == sIsland) return true;
	}
	if(GetCharacterItem(pchar,"map_full") > 0)
	{
		itm = ItemsFromID("map_full");
		if(CheckAttribute(itm, "MapIslId") && itm.MapIslId == sIsland) return true;
	}
	return false;
}

string GetLocationForTreasure(string island)
{
    int iNum;
	aref arDest, arImt;
	string sAttr;

	makearef(arDest, NullCharacter.TravelMap.Islands.(island).Treasure);
	iNum = GetAttributesNum(arDest);
    iNum = rand(iNum-1);

    arImt = GetAttributeN(arDest, iNum);
	Log_TestInfo(GetConvertStrWithReplace("Variable_Treasure_1", "Logs.txt", "#space#", " ")+GetAttributeName(arImt));
	return GetAttributeName(arImt);
}

string GetBoxForTreasure(string island, string location)
{
    int iNum;
	aref arDest, arImt;
	string sAttr;

	makearef(arDest, NullCharacter.TravelMap.Islands.(island).Treasure.(location));
	iNum = GetAttributesNum(arDest);
    iNum = rand(iNum-1);

    arImt = GetAttributeN(arDest, iNum);
	Log_TestInfo(GetConvertStrWithReplace("Variable_Treasure_2", "Logs.txt", "#space#", " ")+GetAttributeValue(arImt));
	return GetAttributeValue(arImt);// тут не атрибут, а значение
}

string GetFileMapForTreasure(string island)
{
    int n;

    for (n = 0; n < ITEMS_QUANTITY; n++)
	{
		if (CheckAttribute(&Items[n], "MapIsland") )
		{
            if (Items[n].MapIsland == island)
            {
                return Items[n].imageTga;
            }
		}
    }
    return "";
}

void AddIslandsMaps(ref item, int iProbability1, int iProbability2)//Assainisateur: на заметку. Карты разбиты на две категории, с раздельными шансами выдачи
                                                                   //чем выше входной параметр, тем НИЖЕ шанс выдачи (оригинальный дизайн)
{
	// карты по аргументу iProbability1:
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_antigua")) item.BoxTreasure.map_antigua = 1;//       Карта Антигуа
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_Bahames")) item.BoxTreasure.map_Bahames = 1;//       Карта Багам
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_barbados")) item.BoxTreasure.map_barbados = 1;//     Карта Барбадоса
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_beliz")) item.BoxTreasure.map_beliz = 1;//           Карта Белиза
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_bermudas")) item.BoxTreasure.map_bermudas = 1;//     Карта Бермуды
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_guad")) item.BoxTreasure.map_guad = 1;//             Карта Гваделупы
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_cuba")) item.BoxTreasure.map_cuba = 1;//             Карта Кубы
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_cumana")) item.BoxTreasure.map_cumana = 1;//         Карта Куманы
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_Curacao")) item.BoxTreasure.map_Curacao = 1;//       Карта Кюрасао
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_martiniqua")) item.BoxTreasure.map_martiniqua = 1;// Карта Мартиники
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_nevis")) item.BoxTreasure.map_nevis = 1;//           Карта Невиса
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_panama")) item.BoxTreasure.map_panama = 1;//         Карта Панамы
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_puerto")) item.BoxTreasure.map_puerto = 1;//         Карта Пуэрто-Рико
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_sm")) item.BoxTreasure.map_sm = 1;//                 Карта Сан Мартина
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_santa")) item.BoxTreasure.map_santa = 1;//           Карта Санта-Каталины
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_TORTUGA")) item.BoxTreasure.map_TORTUGA = 1;//       Карта Тортуги
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_trinidad")) item.BoxTreasure.map_trinidad = 1;//     Карта Тринидада
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_hisp")) item.BoxTreasure.map_hisp = 1;//             Карта Эспаньолы
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_jam")) item.BoxTreasure.map_jam = 1;//               Карта Ямайки
	if(rand(iProbability1) == 1 && !CheckMainHeroMap("map_maracaibo")) item.BoxTreasure.map_maracaibo = 1;//   Карта центра Мэйна
	// карты по аргументу iProbability2:
	if(rand(iProbability2) == 1 && !CheckMainHeroMap("map_dominica")) item.BoxTreasure.map_dominica = 1;//     Карта Доминики
	if(rand(iProbability2) == 1 && !CheckMainHeroMap("map_cayman")) item.BoxTreasure.map_cayman = 1;//         Карта Каймана
	if(rand(iProbability2) == 1 && !CheckMainHeroMap("map_SentLuchia")) item.BoxTreasure.map_SentLuchia = 1;// Карта Санта-Люсии
	if(rand(iProbability2) == 1 && !CheckMainHeroMap("map_terks")) item.BoxTreasure.map_terks = 1;//           Карта Теркса
	if(rand(iProbability2) == 1 && !CheckMainHeroMap("map_Pearl")) item.BoxTreasure.map_Pearl = 1;//           Карта жемчужных промыслов
	if(rand(iProbability2) == 1 && !CheckMainHeroMap("map_maine_1")) item.BoxTreasure.map_maine_1 = 1;//       Карта запад Мэйна
	if(rand(iProbability2) == 1 && !CheckMainHeroMap("map_maine_2")) item.BoxTreasure.map_maine_2 = 1;//       Карта юг Мэйна
}

// LEO: условие «ГГ пришёл в локацию клада» - по нему quests_reaction зовёт SetTreasureBoxFromMap(),
//   которая и раскладывает сундук. БЕЗ него карта ведёт в пустую пещеру и не исчезает.
void ArmTreasureMapCondition(ref item)
{
	Pchar.quest.SetTreasureFromMap.win_condition.l1          = "location";
	Pchar.quest.SetTreasureFromMap.win_condition.l1.location = item.MapLocId;
	Pchar.quest.SetTreasureFromMap.win_condition             = "SetTreasureFromMap";
}

// LEO: выдать игроку целую карту клада. map_full - ОДИН глобальный предмет: остров, тайник и
//   содержимое лежат в его шаблоне. Раздавать предмет БЕЗ генерации нельзя: карта унаследует чужой
//   тайник и, главное, не получит условия прихода в локацию - клад не появится, а карта останется
//   в инвентаре навсегда. Метку именного клада чистим руками: FillMapForTreasure её не трогает,
//   и карта пришла бы с чужой подписью «клад легендарного пирата». Целую карту на руках не трогаем -
//   иначе затрём ещё не откопанный клад.
void GiveTreasureMapToPlayer()
{
	ref rMap;
	if (GetCharacterItem(pchar, "map_full") < 1)
	{
		rMap = ItemsFromID("map_full");
		DeleteAttribute(rMap, "uniquep");
		FillMapForTreasure(rMap);
	}
	GiveItem2Character(pchar, "map_full");
}

void FillMapForTreasure(ref item)// генерация карты сокровищ начинается здесь
{
	item.MapIslId   = GetIslandForTreasure();
	item.MapLocId   = GetLocationForTreasure(item.MapIslId);
	item.MapBoxId   = GetBoxForTreasure(item.MapIslId, item.MapLocId);
	item.MapTypeIdx = drand(2);
	
	// генерим клад
	DeleteAttribute(item, "BoxTreasure");
	
	if (GetCharacterSPECIALSimple(pchar, SPECIAL_L) > (rand(6)+3)) FillBoxForTreasure(item, rand(1));
	else FillBoxForTreasure(item, rand(3));
	FillBoxForTreasureAddition(item);
	Xzibit(item);
	if (rand(99)==1) BookAdd(1, item);// книги в клад 1 уровня, шанс 1%
	
	if (!CheckAttribute(Pchar, "GenQuest.TreasureBuild"))
	{
		if (rand(12+GetCharacterSPECIALSimple(pchar, SPECIAL_L)) == 1) item.MapTypeIdx = -1;
		pchar.PGGTreasureStolen = true;
	}
	else
	{// из кусочков даем супер вещи
		FillBoxForTreasureSuper(item);
		if (GetCharacterSPECIALSimple(pchar, SPECIAL_L) > (rand(7)+2)) FillBoxForTreasureSuper(item);
		if (rand(99)==1) item.BoxTreasure.mushket_Shtuzer = 1;// Винтовальный штуцер, шанс 1%
		if (rand(99)==1) BookAdd(2, item);// книги в клад 2 уровня, шанс 1%
	}	
	DeleteAttribute(Pchar, "GenQuest.TreasureBuild");// сборный
	
	if (sti(item.MapTypeIdx) != -1)
	{
		ArmTreasureMapCondition(item);
	}
}

void FillMapForUniqueTreasure(ref item, string type)// генерация именных карт сокровищ
{
	item.MapIslId   = GetIslandForTreasure();
	item.MapLocId   = GetLocationForTreasure(item.MapIslId);
	item.MapBoxId   = GetBoxForTreasure(item.MapIslId, item.MapLocId);
	item.MapTypeIdx = 0;
	item.uniquep = type;

	// генерим клад
	DeleteAttribute(item, "BoxTreasure");

	FillBoxForTreasure(item, rand(1));
	FillBoxForTreasureAddition(item);
	Xzibit(item);
	FillBoxForTreasureSuper(item);
	FillBoxForTreasureSuper(item);
	BookAdd(1, item);// книги в клад 1 уровня
	BookAdd(2, item);// книги в клад 2 уровня
	Log_TestInfo("treasure "+type);

	switch (type)
	{
		case "Drake"://                                  Дрейк
			item.BoxTreasure.gold = 400000+rand(600000);
			item.BoxTreasure.mushket_revolver = 1;//     Четырёхкамерный мушкет
			item.BoxTreasure.Map_Best = 1;//             Отличная карта
		break;
		case "Barbarigo"://                              Барбариго
			item.BoxTreasure.gold = 400000+rand(600000);
			item.BoxTreasure.blade41 = 1;//              Сторта Барбариго
			item.BoxTreasure.cirass5 = 1;//              Наградная кираса
		break;
		case "BlackBeard"://                             Чёрная Борода
			item.BoxTreasure.gold = 800000+rand(200000);
			item.BoxTreasure.jewelry5 = 900+rand(100);// Золотой слиток
			item.BoxTreasure.howdah = 1;//               Гауда
		break;
		case "Levasser"://                               Левассер
			item.BoxTreasure.gold = 600000+rand(400000);
			item.BoxTreasure.jewelry1 = 250+rand(150);// Сапфир
			item.BoxTreasure.jewelry2 = 250+rand(150);// Алмаз
			item.BoxTreasure.jewelry3 = 250+rand(150);// Рубин
			item.BoxTreasure.jewelry4 = 250+rand(150);// Изумруд
			item.BoxTreasure.grape_mushket = 1;//        Склопетта
		break;
	}

	ArmTreasureMapCondition(item);
}

string GetUniquePirateName()
{
	int count = -1;
	string name[4];
	if (!CheckAttribute(pchar,"questTemp.UniquePirate.Drake"))
	{
		count++;
		name[count]="Drake";
	}
	if (!CheckAttribute(pchar,"questTemp.UniquePirate.Barbarigo"))
	{
		count++;
		name[count]="Barbarigo";
	}
	if (!CheckAttribute(pchar,"questTemp.UniquePirate.BlackBeard"))
	{
		count++;
		name[count]="BlackBeard";
	}
	if (!CheckAttribute(pchar,"questTemp.UniquePirate.Levasser"))
	{
		count++;
		name[count]="Levasser";
	}
	if (count == -1) return "";
	else
	{
		string resultname = name[rand(count)];
		pchar.questTemp.UniquePirate.(resultname) = 1;
		return resultname;
	}

	return "";
}

string GetUniquePirateNameString(string type)
{
	switch (type)
	{
		case "Drake": return "Френсиса Дрейка"; break;
		case "Barbarigo": return "Агостино Барбариго"; break;
		case "BlackBeard": return "Чёрной Бороды"; break;
		case "Levasser": return "Левассера"; break;
	}
	return "";
}

void BookAdd(int value, ref item)//книги - Gregg, Assainisateur
{
	string book = GetRandomBookbyTier(value+2);
	if (CheckForBookReady(book)) book = RegenerateBook(value+2);
	item.BoxTreasure.(book) = 1;
}

void Xzibit(ref item)// сундучный метод
{
	switch (rand(3))
	{
		case 0: item.BoxTreasure.Chest_ammo = 1; break;
		case 1: item.BoxTreasure.Chest_treasure = 1; break;
		case 2: item.BoxTreasure.Chest_quest = 1; break;
		case 3: item.BoxTreasure.Chest_Craftsmans = 1; break;
	}
}

void FillBoxForTreasure(ref item, int i)
{
	// определяем тип клада
	switch (i)
	{
		// good
		case 0:
			if (rand(1) == 1) item.BoxTreasure.jewelry2 = 2 + rand(30);//    Алмаз
			else item.BoxTreasure.jewelry3 = 10 + rand(3);//                 Рубин
			if (rand(1) == 1) item.BoxTreasure.jewelry3 = 1 + rand(30);//    Рубин
			if (rand(1) == 1) item.BoxTreasure.jewelry5 = 5 + rand(60);//    Золотой слиток
			if (rand(1) == 1) item.BoxTreasure.jewelry7 = 15 + rand(100);//  Золотое кольцо с изумрудом
			if (rand(2) == 1) item.BoxTreasure.jewelry10 = 15 + rand(100);// Золотое кольцо с сапфиром
			if (rand(2) == 1) item.BoxTreasure.jewelry13 = 5 + rand(100);//  Камея
			if (rand(2) == 1) item.BoxTreasure.jewelry14 = 5 + rand(100);//  Золотая брошь
			if (rand(2) == 1) item.BoxTreasure.jewelry17 = 5 + rand(100);//  Серебряный слиток
			if (rand(2) == 1) item.BoxTreasure.jewelry18 = 5 + rand(100);//  Золотое кольцо с рубином
			if (rand(2) == 1) item.BoxTreasure.mineral2 = 5 + rand(100);//   Лютня
			if (rand(2) == 1) item.BoxTreasure.mineral5 = 5 + rand(100);//   Старое ведро
			if (rand(4) == 1) item.BoxTreasure.suit_2 = 1;//                 Костюм негоцианта
			if (rand(4) == 1) item.BoxTreasure.suit_1 = 1;// Обноски
			                                              // Assainisateur: шанс получить Обноски в плохом кладе меньше 6,1% даже в "лучшем" случае (Удача 3)
			                                              // для персонажей с более высокой удачей этот шанс СНИЖАЕТСЯ почти к нулю (0,758% при Удаче 10)
			                                              // поэтому добавил Обноски сюда со стандартным шансом выдачи (10%)
			AddIslandsMaps(item, 32, 65);
		break;
		// best
		case 1:
			if (rand(2) == 1) item.BoxTreasure.chest = 1 + rand(14);//       Сундук
			else item.BoxTreasure.jewelry12 = 10 + rand(3);//                Маленькая жемчужина
			if (rand(2) == 1) item.BoxTreasure.icollection = 1 + rand(4);//  Сокровища
			if (rand(1) == 1) item.BoxTreasure.jewelry1 = 15 + rand(100);//  Сапфир
			if (rand(1) == 1) item.BoxTreasure.jewelry6 = 5 + rand(100);//   Серебряное кольцо с сапфиром
			if (rand(1) == 1) item.BoxTreasure.jewelry11 = 15 + rand(100);// Большая жемчужина
			if (rand(1) == 1) item.BoxTreasure.jewelry12 = 15 + rand(100);// Маленькая жемчужина
			if (rand(4) == 1) item.BoxTreasure.suit_3 = 1;//                 Дворянское платье
		break;
		// bad
		case 2:
			if (rand(2) == 1) item.BoxTreasure.rabble = 5 + rand(100);//     Кочерга
			else item.BoxTreasure.blade3 = 5 + rand(100);//                  Катлас
			if (rand(2) == 1) item.BoxTreasure.blade3 = 5 + rand(100);//     Катлас
			if (rand(2) == 1) item.BoxTreasure.blade5 = 5 + rand(100);//     Дага
			if (rand(2) == 1) item.BoxTreasure.blade5 = 1;//                 Дага
			if (rand(2) == 1) item.BoxTreasure.mineral4 = 5 + rand(100);//   Баклан
			if (rand(2) == 1) item.BoxTreasure.mineral6 = 5 + rand(200);//   Коралл
			if (rand(2) == 1) item.BoxTreasure.mineral7 = 5 + rand(100);//   Трубка
			if (rand(2) == 1) item.BoxTreasure.mineral9 = 5 + rand(100);//   Кружка
			if (rand(2) == 1) item.BoxTreasure.mineral10 = 25 + rand(300);// Мешочек соли
			if (rand(2) == 1) item.BoxTreasure.indian4 = 15 + rand(300);//   Глиняный кувшин
			if (rand(2) == 1) item.BoxTreasure.indian8 = 5 + rand(100);//    Серебряный кубок
			if (rand(2) == 1) item.BoxTreasure.indian9 = 5 + rand(100);//    Алебастровый сосуд
			if (rand(2) == 1) item.BoxTreasure.indian13 = 5 + rand(100);//   Древняя курительница
			if (rand(2) == 1) item.BoxTreasure.indian16 = 5 + rand(200);//   Раскрашенный сосуд
			if (rand(2) == 1) item.BoxTreasure.jewelry16 = 5 + rand(100);//  Ожерелье
			if (rand(2) == 1) item.BoxTreasure.suit_1 = 1;//                 Обноски
		break;
		// рекурсивная переадресация на good
		case 3:
			FillBoxForTreasure(item, 0);
		break;
	}
}

void FillBoxForTreasureAddition(ref item)
{
	float nLuck = GetCharacterSkillToOld(Pchar, SKILL_FORTUNE);
	
	// выдача куска карты (с проверкой на скилл "Везение")
	if (10*nLuck > rand(110))// гладкий скейл шанса выдачи от скилла. максимальный шанс выдачи ограничен 90% даже при 100 скилла (оригинальный дизайн, без изменений)
	{
		if (GetCharacterItem(Pchar, "map_part1") == 0)
		{
			item.BoxTreasure.map_part1 = 1;
		}
		else
		{
			if (GetCharacterItem(Pchar, "map_part2") == 0)
			{
				item.BoxTreasure.map_part2 = 1;
			}
		}
	}
	
	// +1 вещи
	switch (rand(40))//Assainisateur: на заметку. шанс 14/41 (~34,1%, ~1/3) не получить ничего, шанс 27/41 (~65,9%, ~2/3) получить что-то одно из списка далее (оригинальный дизайн, без изменений)
	{
		case 1: item.BoxTreasure.sculMa1 = 1; break;//   Белый хрустальный череп
		case 2: item.BoxTreasure.sculMa2 = 1; break;//   Розовый хрустальный череп
		case 3: item.BoxTreasure.sculMa3 = 1; break;//   Голубой хрустальный череп
		case 4: item.BoxTreasure.Coins = 1; break;//     Проклятые жемчужины
		case 5: item.BoxTreasure.jewelry4 = 1; break;//  Изумруд
		case 6: item.BoxTreasure.jewelry8 = 1; break;//  Бронзовое кольцо
		case 7: item.BoxTreasure.jewelry9 = 1; break;//  Бронзовый крестик
		case 8: item.BoxTreasure.jewelry15 = 1; break;// Изумрудные подвески
		case 9: item.BoxTreasure.STATUE1 = 1; break;//   Статуэтка Шочипилли
		case 10: item.BoxTreasure.indian1 = 1; break;//  Оберег Тлальчитонатиу
		case 11: item.BoxTreasure.indian2 = 1; break;//  Пугающая фигурка
		case 12: item.BoxTreasure.indian3 = 1; break;//  Нефритовая маска
		case 13: item.BoxTreasure.indian5 = 1; break;//  Двойная маска
		case 14: item.BoxTreasure.indian6 = 1; break;//  Амулет Шиукоатля
		case 15: item.BoxTreasure.indian7 = 1; break;//  Идол Великой Матери
		case 16: item.BoxTreasure.indian10 = 1; break;// Оберег Эхекатля
		case 17: item.BoxTreasure.indian12 = 1; break;// Кубок-тотем Тепейоллотля
		case 18: item.BoxTreasure.indian14 = 1; break;// Чаша Ололиуки
		case 19: item.BoxTreasure.indian15 = 1; break;// Базальтовая голова
		case 20: item.BoxTreasure.indian17 = 1; break;// Тельная ладанка
		case 21: item.BoxTreasure.indian18 = 1; break;// Идол Атлауа
		case 22: item.BoxTreasure.indian19 = 1; break;// Статуэтка Тлалока
		case 23: item.BoxTreasure.indian20 = 1; break;// Церемониальный нож
		case 24: item.BoxTreasure.indian21 = 1; break;// Церемониальный сосуд
		case 25: item.BoxTreasure.indian22 = 1; break;// Голова воина племени майя
		case 26: item.BoxTreasure.Mineral3 = 1; break;// Свечи
		case 27: item.BoxTreasure.Mineral8 = 1; break;// Башмак
	}
	AddIslandsMaps(item, 9, 19);
	//добавим тотемы
	// Assainisateur: выдача только недостающих тотемов.
	if (rand(19) == 1)// сначала выясняем, будет ли ВООБЩЕ возможность получить тотем (бросок на "натуральную 20-ку" как в DnD, шанс 5%). если повезло, то едем дальше:
	{
		string sMissingTotems[10];// в оригинальном коде прописано 10 тотемов для возможной выдачи (без изменений)
		string sTotemName;
		int iTotemCounter = -1;// счётчик тотемов доступных к выдаче игроку
		
		for (int i = 1; i < 11; i++)// перебираем 10 тотемов по порядку
		{
			sTotemName = "Totem_" + i;
			if (!CheckMainHeroTotem(sTotemName))
			{//                                               если данный тотем прошёл проверку, то:
				iTotemCounter++;//                            1. увеличиваем счётчик
				sMissingTotems[iTotemCounter] = sTotemName;// 2. добавляем тотем в список на выдачу
			}
		}
		
		if (iTotemCounter > -1)
		{//                                                     если в список на выдачу добавлен хотя бы один тотем, то:
			sTotemName = sMissingTotems[rand(iTotemCounter)];// 1. выбираем случайный тотем из добавленных в список
			item.BoxTreasure.(sTotemName) = 1;//                2. кладём выбранный тотем в клад
		}
	}
}

void FillBoxForTreasureSuper(ref item)
{
	float nLuck = GetCharacterSkillToOld(Pchar, SKILL_FORTUNE);
	
	// выдача крутого предмета (с проверкой на скилл "Везение")
	if (10*nLuck > rand(70))// гладкий скейл шанса выдачи от скилла. 100% шанс выдачи достигается при 71 скилла и выше (оригинальный дизайн, без изменений)
	{
		// Assainisateur: выдача только недостающих предметов.
		string sMissingItems[35];// в оригинальном коде прописано 35 предметов для возможной выдачи (без изменений)
		string sItemName;
		int iItemCounter = -1;// счётчик предметов доступных к выдаче игроку
		bool bPerformCheck = true;
		bool bSkipThisOne;
		
		while (iItemCounter < 0)
		{
			for (int i = 1; i < 36; i++)// перебираем все 35 предметов (список отсортирован по типам предметов для удобства восприятия кода)
			{
				switch (i)
				{
					case 1: sItemName = "Map_Best"; break;//                  _Отличная карта
					case 2: sItemName = "spyglass4"; break;//                  Отличная подзорная труба
					case 3: sItemName = "spyglass5"; break;//                 _Эксклюзивная подзорная труба
					case 4: sItemName = "BackPack5"; break;//                 _Эксклюзивный подсумок
					case 5: sItemName = "talisman1"; break;//                  Оберег 'Пернатый Змей'
					case 6: sItemName = "talisman5"; break;//                  Оберег 'Гонтер'
					case 7: sItemName = "talisman8"; break;//                  Оберег 'Скорпион'
					case 8: sItemName = "talisman10"; break;//                 Талисман 'Упырь'
					case 9: sItemName = "talisman12"; break;//                 Талисман 'Аргузин'
					case 10: sItemName = "talisman13"; break;//                Талисман 'Повелитель огня'
					case 11: sItemName = "talisman14"; break;//                Талисман 'Творец вдов'
					case 12: sItemName = "talisman15"; break;//                Талисман 'Сын ягуара'
					case 13: sItemName = "talisman16"; break;//                Талисман 'Спиногрыз'
					case 14: sItemName = "talisman17"; break;//               _Талисман 'Клеврет'
					case 15: sItemName = "cirass3"; break;//                   Гвардейская кираса
					case 16: sItemName = "cirass4"; break;//                  _Офицерская кираса
					case 17: sItemName = "pistol9"; break;//                   Двузарядный пистолет
					case 18: sItemName = "pistol10"; break;//                  Трёхствольный пистолет
					case 19: sItemName = "pistol4"; break;//                  _Четырёхствольный пистолет
					case 20: sItemName = "mushket"; break;//                   Кремневый мушкет
					case 21: sItemName = "flint_mushketon"; break;//           Кремневый Мушкетон
					case 22: sItemName = "mushket_SHZ"; break;//               Колониальный мушкет
					case 23: sItemName = "mushket_mushketon1"; break;//       _Колониальный Мушкетон
					case 24: sItemName = "topor_emperor"; break;//             ТО: Императорский топор
					case 25: sItemName = GetGeneratedItem("blade13"); break;// ТО: Скьявона
					case 26: sItemName = GetGeneratedItem("blade20"); break;// ТО: Клейбэг
					case 27: sItemName = GetGeneratedItem("blade33"); break;// ТО: Валлонская шпага
					case 28: sItemName = GetGeneratedItem("blade24"); break;// СО: Офицерский клеванг
					case 29: sItemName = GetGeneratedItem("blade25"); break;// СО: Морская сабля
					case 30: sItemName = GetGeneratedItem("blade47"); break;// СО: Персидская сабля
					case 31: sItemName = GetGeneratedItem("blade23"); break;// ЛО: Бретта
					case 32: sItemName = GetGeneratedItem("blade26"); break;// ЛО: Паппенхаймер
					case 33: sItemName = GetGeneratedItem("blade30"); break;//_ЛО: Нобиль
					case 34: sItemName = "indian11"; break;//                  Крысиный бог. внимание прикол: Крысобог присутствует в этом списке дважды. оригинальный дизайн, без изменений
					case 35: sItemName = "indian11"; break;//                  хотели таким образом удвоить шанс выпадения Крысобога в кладе? или просто не заметили повтора?
				}
				
				bSkipThisOne = bPerformCheck && GetCharacterItem(Pchar, sItemName) > 0;// проверяем, не лежит ли УЖЕ данный предмет в инвентаре игрока – только на первой итерации цикла while
				                                                                       // во вторую итерацию while попадаем только в двух КОСМИЧЕСКИ редких ситуациях
				                                                                       // в этих редких ситуациях во второй итерации while проверка отключается, чтобы выдать игроку любой случайный предмет из списка
				// проверяем, не лежит ли УЖЕ данный предмет в кладе - это исключает совпадение выдаваемых
				// предметов при двойном вызове метода (именные клады и, если повезло, клады 2-го уровня)
				// LEO: запись в кладе заводится только когда предмет туда положен (ниже по коду), поэтому у
				// ещё не выданного её нет - голое чтение сыпало "missed attribute: <предмет>" + "no rAP data"
				// на каждый перебор списка. Плюс Storm не сокращает вычисление ||, так что читалось даже
				// тогда, когда предмет уже отсеян проверкой инвентаря выше.
				if (!bSkipThisOne)
				{
					if (CheckAttribute(item, "BoxTreasure." + sItemName))
					{
						bSkipThisOne = sti(item.BoxTreasure.(sItemName)) > 0;
					}
				}
				if (!bSkipThisOne)
				{//                                            если данный предмет прошёл все проверки, то:
					iItemCounter++;//                          1. увеличиваем счётчик
					sMissingItems[iItemCounter] = sItemName;// 2. добавляем предмет в список на выдачу
				}
			}
			
			bPerformCheck = false;// отключаем проверку наличия предмета в инвентаре игрока для теоретически возможного второго цикла while
		}
		
		sItemName = sMissingItems[rand(iItemCounter)];// 1. выбираем случайный предмет из добавленных в список
		item.BoxTreasure.(sItemName) = 1;//              2. кладём выбранный предмет в клад
	}
}

void SetTreasureBoxFromMap()
{
    aref   item;
    ref    loc;
    string box;
    aref   arToBox;
    aref   arFromBox;

    if (GetCharacterItem(Pchar, "map_full")>0 )
    {
        Log_Info(GetConvertStrWithReplace("Variable_Treasure_3", "Logs.txt", "#space#", " "));
		//Запрет спавна проклятого кэпа в этой локации
		ref locLoad = &locations[reload_location_index];
		pchar.treasurelocation = locLoad.id;

        PlaySound("interface\notebook.wav");
		pchar.treasurelocation = locLoad.id;
		Statistic_AddValue(Pchar, "Treasure", 1);
        if( CheckAttribute(Pchar,"location.from_sea") )
        {
            if (rand(2) == 1) //33 процента
            {
                Pchar.quest.SetTreasureHunter.win_condition.l1          = "location";
                Pchar.quest.SetTreasureHunter.win_condition.l1.location = Pchar.location.from_sea;
                Pchar.quest.SetTreasureHunter.win_condition             = "";
				if (rand(1) == 1 && CheckAttribute(pchar, "PGGTreasureStolen")) //50 процентов что это будет ПГГ
				{
					int heroSelected = 1;
					string chosenHero = rand(PsHeroQty);
					for (i = 0; i < 25; i++)
					{
						if (sti(chosenHero) != 0)
						{
							sld = CharacterFromID("PsHero_"+chosenHero);
							// LEO: занятый турниром арены не годится - он стоит на якоре у Доминики и на время
							//   турнира неприкасаем (PGG_ParkAtTournament ставит immortal). Выдернутый в сцену
							//   клада, он не получал урона ни саблей, ни пулей.
							if (sld.PGGAi.location == "Dead" || IsCompanion(sld) || IsOfficer(sld) || sld.dialog.filename == "Enc_Officer_dialog.c" || CheckAttribute(sld, "PGG_Tournament"))
							{
								chosenHero = rand(PsHeroQty);
							}
							else
							{
								heroSelected = 0;
								pchar.chosenTreasureHero = sld.id;
								PGG_Disband_Fleet(sld);
								break;   // LEO: кандидат найден. Без выхода цикл крутил оставшиеся итерации на том же
								         //   ПГГ и звал роспуск его эскадры по два десятка раз подряд.
							}
						}
						else
						{
							chosenHero = rand(PsHeroQty);
						}
					}
					if(heroSelected == 0)
					{
						Pchar.quest.SetTreasureHunter.function    = "SetPGGTreasureHunter";
						Log_TestInfo(GetConvertStrWithReplace("Variable_Treasure_4", "Logs.txt", "#space#", " ") + pchar.chosenTreasureHero);
					}
					else
					{
						Pchar.quest.SetTreasureHunter.function    = "SetTreasureHunter";
					}
				}
				else
				{
					Pchar.quest.SetTreasureHunter.function    = "SetTreasureHunter";
				}
            }
        }
		DeleteAttribute(pchar, "PGGTreasureStolen");
        Items_FindItem("map_full", &item);

        box = item.MapBoxId;

        loc = &locations[FindLocation(item.MapLocId)];
        loc.(box).items = "";

        makearef(arToBox, loc.(box).items);
        makearef(arFromBox, item.BoxTreasure);
        CopyAttributes(arToBox, arFromBox);

        loc.(box) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
        loc.(box).Treasure =  true; // признак сокровища в сундуке

        DeleteAttribute(item, "MapIslId");
		DeleteAttribute(item, "uniquep");
        TakeNItems(Pchar, "map_full", -1);
    }
}

// погодня за ГГ на карте
void  TraderHunterOnMap()
{
    ref  sld;
    int  i;
    int  iDUClass;
    int  iDUSeed = dRandSlot(4, 97); // LEO: дневной сдвиг по пулу кораблей - см. цикл ниже

    string sCapId = "TraderHunter";
    string sGroup = "Sea_" + sCapId + "1";

	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
    for (i = 1; i <= GetCompanionQuantity(pchar); i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_hol_2", "man", "man", sti(PChar.rank) + 5, PIRATE, 15, true));
        SetShipHunter(sld);
        // LEO: РАЗНЫЕ корабли в эскадре, а не клоны. Класс берём тот, что выдал SetShipHunter по рангу ГГ,
        //   с прежним потолком в 2 класс (без мановаров) - меняем только КОНКРЕТНЫЙ тип внутри класса,
        //   поэтому мощь перехвата не едет. Индекс в пуле = дневной dRand + номер: состав фиксируется днём
        //   взятия фрахта, а соседние номера дают заведомо разные корабли.
        //   Потолок класса вкатан СЮДА вместо отдельного HunterCapToClass2: тот пересоздавал корабль ещё
        //   раз, а каждый GenerateShipExt занимает слот RealShips - лишний расход на 6-12 перехватчиков.
        //   Пересадка идёт ДО SetFantomParamHunter - навыки и ранг капитана считаются от финального корабля.
        iDUClass = sti(RealShips[sti(sld.Ship.Type)].Class);
        if (iDUClass < 2) iDUClass = 2;
        HunterApplyShipType(sld, GetShipTypeExtByIdx(iDUClass, iDUClass, "war", PIRATE, iDUSeed + i));
        SetFantomParamHunter(sld);
        SetCaptanModelByEncType(sld, "war");
        sld.AlwaysEnemy = true;
        sld.DontRansackCaptain = false;     // можно брать на абордаж - капитан сдаётся в каюте
        sld.GentlemenHunter = true;         // диалог сдачи в каюте (плен/убийство, без опыта при пощаде) - ВСЕМ капитанам эскадры
        sld.NoOfficerHire = true;           // пленного джентльмена удачи нельзя нанять офицером в трюме (Ransack_captain_dialog)
        sld.mapEnc.type = "war";
        sld.mapEnc.Name = GetConvertStrWithReplace("Variable_Treasure_1", "Names.txt", "#space#", " ");
		sld.hunter = "pirate";
		// быстрый ПИРАТСКИЙ корабль на глобальной карте (по образцу скоростных ОЗГ) - чтобы перехватчик догонял ГГ
		if(makeint(pchar.rank) < 7)
		{
			sld.mapEnc.worldMapShip = "pirates_shooner";
		}
		else
		{
			if(makeint(pchar.rank) < 11)
			{
				sld.mapEnc.worldMapShip = "pirates_bark";
			}
			else
			{
				if(makeint(pchar.rank) < 16)
				{
					sld.mapEnc.worldMapShip = "pirates_frigate";
				}
				else
				{
					sld.mapEnc.worldMapShip = "pirates_manowar";
				}
			}
		}
        Group_AddCharacter(sGroup, sCapId + i);
    }

    Group_SetGroupCommander(sGroup, sCapId+ "1");
    Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);
	Map_CreateFastWarrior("", sCapId + "1", 8); // быстрее обычного перехватчика (coolwarrior, kSpeed 1.7 вместо 1.2 у tradehunter) - джентльмены удачи прыткие
}

// перехватчик комендантского (боеприпасного) фрахта - каперы нации, воюющей с нацией города отправления
void FortHunterOnMap()
{
	ref  sld;
	int  i;

	// LEO: нация отправителя - у КОЛОНИИ, а не у флага NPC: после смены владельца города NPC до своей
	//   перегенерации носит прежнюю нацию (тот же урок уже учтён в FortAgentOnMap ниже)
	int iGiveIdx = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
	if (iGiveIdx < 0) return;
	int iFromNation = -1;
	int iFromCol = FindColony(characters[iGiveIdx].city);
	if (iFromCol >= 0) iFromNation = sti(Colonies[iFromCol].nation);
	if (iFromNation < 0) iFromNation = sti(characters[iGiveIdx].nation);

	int iEnemyNation = FindEnemyNation2NationWithoutPirates(iFromNation);
	// LEO: войны нет - каперов слать некому. Раньше тут стоял голый return, и половина рейсов (перехват
	//   ролится 50/50 с посыльным-агентом) шла вообще без риска. Про конвой уже пущен слух - идут пираты.
	if (iEnemyNation == -1) { TraderHunterOnMap(); return; }

	string sCapId = "FortAmmoHunter";
	string sGroup = "Sea_" + sCapId + "1";

	// национальная модель на глобальной карте, размер по рангу ГГ (как у ОЗГ)
	string sMapShip = "british";
	if (iEnemyNation == FRANCE)  sMapShip = "french";
	if (iEnemyNation == SPAIN)   sMapShip = "spanish";
	if (iEnemyNation == HOLLAND) sMapShip = "dutch";
	if(makeint(pchar.rank) < 7)
	{
		sMapShip = sMapShip + "_lugger";
	}
	else
	{
		if(makeint(pchar.rank) < 11)
		{
			sMapShip = sMapShip + "_brig";
		}
		else
		{
			if(makeint(pchar.rank) < 16)
			{
				sMapShip = sMapShip + "_frigate";
			}
			else
			{
				sMapShip = sMapShip + "_battleship";
			}
		}
	}

	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	for (i = 1; i <= GetCompanionQuantity(pchar); i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_" + NationShortName(iEnemyNation) + "_" + (rand(1) + 1), "man", "man", sti(PChar.rank) + 5, iEnemyNation, 15, true));
		SetShipHunter(sld);
		HunterCapToClass2(sld); // LEO: каперы не мощнее 2 класса (без линкоров 1 класса); ДО SetFantomParamHunter - навыки/ранг капитана от корабля 2 класса
		SetFantomParamHunter(sld);
		SetCaptanModelByEncType(sld, "war");
		sld.AlwaysEnemy = true;
		sld.DontRansackCaptain = false; // можно брать на абордаж - капитан сдаётся в каюте
		sld.FortHunter = true;          // диалог сдачи в каюте (плен/убийство, бонус за пощаду как у ContraHunter) - ВСЕМ капитанам эскадры
		sld.NoOfficerHire = true;       // пленного капера-перехватчика нельзя нанять офицером в трюме (Ransack_captain_dialog)
		sld.mapEnc.type = "war";
		sld.mapEnc.Name = GetConvertStrWithReplace("Variable_Treasure_8", "Names.txt", "#space#", " ");
		sld.hunter = "hunter";
		sld.mapEnc.worldMapShip = sMapShip;
		Group_AddCharacter(sGroup, sCapId + i);
	}

	Group_SetGroupCommander(sGroup, sCapId + "1");
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Map_CreateFastWarrior("", sCapId + "1", 8);

	// СЖ-записи о бое: запись о нападении пишет wdmEnterSeaQuest (worldmap_events.c, по образцу ОЗГ -
	// прямой хук входа в бой с квестовым энкаунтером). Здесь - вахта на КОНЕЦ боя (вся группа потоплена/
	// взята на абордаж): многоразовая (quest.again - без него после срабатывания остаётся over="yes",
	// и повторный взвод поверх НЕ работает), реакция сама снимает её, когда запись сделана.
	// FortHunt_ClearWatch чистит всё при закрытии заказа.
	DeleteAttribute(pchar, "questTemp.FortHuntAttacked");
	DeleteAttribute(pchar, "quest.FortHuntAttack"); // от прошлых версий/заказов мог остаться мёртвый (over) квест
	DeleteAttribute(pchar, "quest.FortHuntOver");
	pchar.quest.FortHuntOver.again = true;
	pchar.quest.FortHuntOver.win_condition.l1 = "Group_Death";
	pchar.quest.FortHuntOver.win_condition.l1.group = sGroup;
	pchar.quest.FortHuntOver.win_condition = "FortHunt_BattleOver";
}

// ============================================================================
//  КОМЕНДАНТСКИЙ ФРАХТ: ПОСЫЛЬНЫЙ-АГЕНТ ВРАЖЕСКОЙ НАЦИИ (другие 50% перехвата).
//  Нейтральный быстрый НЕСКИПАЕМЫЙ энкаунтер: на карте атакует (кнопка "пропустить"
//  заблокирована), в море нейтрализуется (wdmEnterSeaQuest) - высылаешь шлюпку и
//  говоришь на палубе (FortAgent_dialog.c). Предлагает увести казённый груз себе либо
//  поручить ГГ доставить его в форт своей нации.
// ============================================================================

// форт-город заданной нации (цель самовоза). "" - нет подходящего.
// Выбор города - на ДНЕВНОМ drand (стабилен в течение игрового дня, не переролить сейв-лоадом).
string FortAgent_FindNationCity(int iNation)
{
	string aCities[40];
	int n = 0;
	int i;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		if (Colonies[i].id == "none") continue;
		if (sti(Colonies[i].nation) != iNation) continue;
		if (CheckAttribute(&Colonies[i], "HasNoFort")) continue;
		if (Colonies[i].id == "Panama") continue; // LEO: груз сдаётся с кораблей на рейде, а к Панаме морем не подойти
		if (n < 40) { aCities[n] = Colonies[i].id; n = n + 1; }
	}
	if (n == 0) return "";
	if (n == 1) return aCities[0];
	int iPick = dRandSlotLow(2, n - 1); // дневной drand, отдельный поток (декорреляция); индекс массива - через переменную (гоча Storm)
	return aCities[iPick];
}

void FortAgentOnMap()
{
	// LEO: нация, воюющая с нацией АДРЕСАТА (форт-получатель боеприпасов), а не отправителя - посыльный
	//      перехватывает груз, идущий ЕГО врагу («не место такому грузу в арсенале наших врагов»). Нет войны - агента слать некому (вместо него - каперы)
	//      Нация берётся у КОЛОНИИ: форт принадлежит ей, а флаг NPC-торговца после смены владельца
	//      города может остаться старым - тогда агент заявлял вражду с фортом, с которым в мире.
	int iFortNation = -1;
	int iFortCol = FindColony(pchar.CargoQuest.iTradeColony);
	if (iFortCol >= 0) iFortNation = sti(Colonies[iFortCol].nation);
	if (iFortNation < 0) iFortNation = sti(characters[GetCharacterIndex(pchar.CargoQuest.TraderID)].nation);
	int iEnemyNation = FindEnemyNation2NationWithoutPirates(iFortNation);
	if (iEnemyNation == -1) { FortHunterOnMap(); return; }
	// у вражеской нации нет форта-адресата (все города захвачены/пиратские) - самовоз привёл бы в никуда (софтлок).
	// Тогда агента не шлём, а ставим обычных каперов-перехватчиков
	if (FortAgent_FindNationCity(iEnemyNation) == "") { FortHunterOnMap(); return; }

	DeleteAttribute(pchar, "FortAgentResolved"); // новый энкаунтер - предложения снова доступны (на случай след. фрахта)

	string sCapId = "FortAgent_Cap";
	string sGroup = "Sea_FortAgent";

	string sMapShip = "british";
	if (iEnemyNation == FRANCE)  sMapShip = "french";
	if (iEnemyNation == SPAIN)   sMapShip = "spanish";
	if (iEnemyNation == HOLLAND) sMapShip = "dutch";
	if (makeint(pchar.rank) < 7)       sMapShip = sMapShip + "_lugger";
	else if (makeint(pchar.rank) < 11) sMapShip = sMapShip + "_brig";
	else if (makeint(pchar.rank) < 16) sMapShip = sMapShip + "_frigate";
	else                               sMapShip = sMapShip + "_battleship";

	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);

	ref sld = GetCharacter(NPC_GenerateCharacter(sCapId, "off_" + NationShortName(iEnemyNation) + "_" + (rand(1) + 1), "man", "man", sti(pchar.rank) + 3, iEnemyNation, 8, true));
	// посыльный-агент ВДВОЕ сильнее ГГ: кулсейлор (корабль на класс мощнее, полный экипаж, макс корабельные навыки/перки)
	// + кулфайтер (ранг x2, фехтование/HP/боевые перки). Серьёзная угроза - казённый груз так просто не отбить.
	int iAgClass = GetCharacterShipClass(pchar) - 1; // на класс мощнее текущего корабля ГГ
	if (iAgClass < 2) iAgClass = 2; // но не выше 2 класса - на мановарах (1 класс) агент не рассекает
	SetFantomParamHunter(sld);           // база (AI/деньги/экипаж/лечилки) - ВАЖНО до cool: внутри Fantom_SetRandomSkills перетирает навыки, поэтому cool ставим ПОСЛЕ
	FantomMakeCoolSailor(sld, GetShipTypeExt(iAgClass, iAgClass, "war", iEnemyNation), "", CANNON_TYPE_CANNON_LBS24, 95, 90, 95);
	FantomMakeCoolFighter(sld, makeint(pchar.rank) * 2, 95, 90, "", "", 150);
	SetCaptanModelByEncType(sld, "war"); // военный вид капитана (кулфайтер перерисовал модель - возвращаем)
	sld.AlwaysEnemy = true;        // на карте гонится и блокирует "пропустить"; в море нейтрализуем (wdmEnterSeaQuest)
	sld.DontRansackCaptain = true; // разговор на палубе через шлюпку, не абордаж
	sld.FortAgent = true;          // распознаём при входе в море
	sld.mapEnc.type = "war";
	sld.mapEnc.Name = GetConvertStrWithReplace("Variable_Treasure_10", "Names.txt", "#space#", " "); // "Посыльный вражеской нации"
	sld.hunter = "hunter";
	sld.mapEnc.worldMapShip = sMapShip;
	sld.Dialog.Filename = "FortAgent_dialog.c";
	sld.Dialog.CurrentNode = "First time";
	sld.DeckDialogNode = "First time"; // палубный визит шлюпкой держит свой диалог (иначе Cabin.c форсит Capitans_dialog.c)
	sld.greeting = "Gr_Officer";
	Group_AddCharacter(sGroup, sCapId);

	Group_SetGroupCommander(sGroup, sCapId);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Map_CreateFastWarrior("", sCapId, 8);
}

// раскассирование конвоя + уход агента выполняем НА ВЫХОДЕ из палубного диалога:
// despawn/удаление кораблей во время активного диалога вешало выход (софтлок). Обёртки по id агента.
void FortAgent_HandOver_OnExit()
{
	int cn = GetCharacterIndex("FortAgent_Cap");
	if (cn < 0) return;
	FortAgent_HandOver(GetCharacter(cn));
}
void FortAgent_Depart_OnExit()
{
	int cn = GetCharacterIndex("FortAgent_Cap");
	if (cn < 0) return;
	FortAgent_Depart(GetCharacter(cn));
}

// ОТКАЗ от предложений: НА ВЫХОДЕ из диалога делаем РЕАЛЬНОГО агента (FortAgent_Cap) враждебным.
// (в диалоге собеседник мог быть клоном - правка на нём не действовала на корабль в море, отсюда "не могу атаковать").
// Снимаем DontRansackCaptain - теперь его можно взять на абордаж, в каюте он торгуется (FortAgentCabin).
void FortAgent_Refuse_OnExit()
{
	int cn = GetCharacterIndex("FortAgent_Cap");
	if (cn < 0) return;
	ref a = GetCharacter(cn);
	DeleteAttribute(a, "AlwaysFriend");
	DeleteAttribute(a, "ShipEnemyDisable");
	a.AlwaysEnemy = true;
	a.DontRansackCaptain = false;   // абордаж разрешён - сдача в каюте
	a.FortAgentRefused = true;      // метка каютной сцены (SetQuestAboardCabinDialog по CaptanId)
	a.Ship.LastBallCharacter = -1;
	SetCharacterRelationBoth(cn, GetMainCharacterIndex(), RELATION_ENEMY);
	a.nation = PIRATE; // отказ - агент берёт груз силой, нация Y его дезавуирует: убийство НЕ вешает НЗГ нации Y.
	                   // За провал самого фрахта карает нация-ЗАКАЗЧИК (X) через долг гильдии (GuildDebt_Start по GiveTraderID).
	pchar.FortAgentResolved = true; // решение принято (отказ) - палубный диалог "First time" больше не предлагать; дальше бой/абордаж
	// ПРИКАЗ АТАКОВАТЬ ставим НЕ здесь: палубный диалог идёт при выгруженном море (bSeaActive=false,
	// палуба - отдельная LoadLocation), а Group_SetTaskAttack запускает живой приказ через
	// AIAttack_GroupAttack только при bSeaActive=true. Отсюда был баг "враждебен, но не атакует":
	// приказ ложился в атрибут группы, но до корабля не доходил, а лок мешал периодике переставить
	// (она к тому же гейтится на вражду НАЦИИ - нация агента игроку не враг).
	// Просим движок начать бой ПРИ ВОЗВРАТЕ в море (Cabin_ReloadEndFadeAfter), как штатный StartBattleAfterDeck.
	pchar.FortAgentFightAfterDeck = true;
}

// передать агенту команду (он откупается за конвой): нужно = оптимум ЕГО корабля; берём ИЗЛИШЕК
// сверх оптимума с кораблей ЭСКАДРЫ ГГ (не конвоя), ни один не опускаем ниже его оптимума
void FortAgent_GiveCrewToAgent(ref agent)
{
	int iNeed = GetOptCrewQuantity(agent);
	int iGot = 0;
	int k, cn, surplus, take;
	for (k = 0; k < COMPANION_MAX; k++)
	{
		if (iGot >= iNeed) break;
		cn = GetCompanionIndex(pchar, k);
		if (cn < 0) continue;
		ref s = &Characters[cn];
		if (sti(s.ship.type) == SHIP_NOTUSED) continue;
		if (HasSubStr(s.id, "FortConvoy_Cap")) continue; // суда конвоя - не источник
		surplus = GetCrewQuantity(s) - GetOptCrewQuantity(s);
		if (surplus <= 0) continue;
		take = iNeed - iGot;
		if (take > surplus) take = surplus;
		s.Ship.Crew.Quantity = sti(s.Ship.Crew.Quantity) - take;
		iGot = iGot + take;
	}
	// собранную команду РЕАЛЬНО зачисляем агенту (раньше iGot лишь считался, на корабль агента не попадал - "часть команды не передавалась")
	if (iGot > 0) agent.Ship.Crew.Quantity = GetCrewQuantity(agent) + iGot;
}

// агент откупился в каюте (после абордажа отказника): платит x5, забирает конвой и команду, уходит на СВОЁМ корабле. Без опыта/репутации.
// КЛЮЧЕВОЕ: корабль агента НЕ становится трофеем. Любой абордаж корабля движок завершает формой захвата
// (LaunchRansackMain в LAi_ReloadEndFade). Чтобы её обойти - помечаем "каютную ситуацию" (как эпидемия/взрыв):
// флаг GenQuest.FortAgentBuyOut -> CheckShipSituation_GenQuest() -> выход через Return2SeaAfterCabin (без захвата).
// Корабль агента освобождаем в море ПРИ ВОЗВРАТЕ (Cabin_ReloadEndFadeAfter, bSeaActive=true) - FortAgent_BuyOutRelease.
void FortAgent_CabinDeal()
{
	AddMoneyToCharacter(pchar, sti(pchar.CargoQuest.iMoney) * 5);
	int cn = GetCharacterIndex("FortAgent_Cap");
	if (cn >= 0) FortAgent_GiveCrewToAgent(GetCharacter(cn)); // команда агенту ДО релиза (клон копирует корабль агента вместе с ней)
	FortConvoy_Disband(); // конвой уходит к агенту
	pchar.quest.generate_trade_quest_progress = "";
	pchar.quest.generate_trade_quest.over = "yes";
	CloseQuestHeader("DELIVERY_TRADE_QUEST");
	FortHunt_ClearWatch();

	pchar.GenQuest.FortAgentBuyOut = true; // -> CheckShipSituation_GenQuest() true -> каютный выход = Return2SeaAfterCabin (без формы захвата)
	pchar.FortAgentResolved = true;        // сделка заключена - блокируем повторные предложения (узел "First time")

	// восстановить управление ГГ (как QuestAboardCabinDialogNotBattle), иначе ГГ застывал в каюте без выхода
	Lai_SetPlayerType(pchar);
	if (CheckAttribute(pchar, "GenQuest.QuestAboardCabinDialogIdx"))
	{
		ref sld = &Characters[sti(pchar.GenQuest.QuestAboardCabinDialogIdx)];
		LAi_RemoveCheckMinHP(sld);
		LAi_SetImmortal(sld, false);
		LAi_SetWarriorType(sld);
	}
	LAi_LocationFightDisable(loadedLocation, false);
	// АВТО-выход в море без формы захвата (ручной reload через локатор не срабатывал - ГГ застревал).
	// Реакция FortAgent_ExitToSea = Return2SeaAfterCabin (quests_reaction.c), по образцу LAi_ReloadBoarding.
	DoQuestCheckDelay("FortAgent_ExitToSea", 1.0);
}

// корабль агента уходит в море свободным (без захвата). Зовётся из Cabin_ReloadEndFadeAfter при ВОЗВРАТЕ в море (bSeaActive=true),
// иначе SeaAI_SetCaptainFree отработал бы вхолостую. Тот же механизм, что освобождает кэпа в ситуациях "эпидемия/взрыв".
void FortAgent_BuyOutRelease()
{
	DeleteAttribute(pchar, "GenQuest.FortAgentBuyOut");
	int cn = GetCharacterIndex("FortAgent_Cap");
	if (cn >= 0) SetCaptainFreeQuestAboardSituation(GetCharacter(cn)); // клон агента уходит дружелюбным; CheckQuestAboardCabinSituation тут no-op (нет Epidemy/Explosion)
	// клон "FortAgent_Cap_free" копирует атрибуты агента (вкл. диалог) - иммунитет + запрет повторного разговора
	int cf = GetCharacterIndex("FortAgent_Cap_free");
	if (cf >= 0)
	{
		ref f = GetCharacter(cf);
		f.DontDeskTalk = true;             // нельзя заново вызвать палубный диалог и переиграть (issue 3)
		SpeakTruce_ApplyShip(f);           // иммунитет: ГГ/компаньоны не могут его добить (issue 2)
	}
	Group_DeleteGroup("Sea_FortAgent");
}

// убрать энкаунтер посыльного с глобалки/моря (фрахт уже сдан/провален - агенту ловить больше нечего)
void FortAgent_Cleanup()
{
	int cn = GetCharacterIndex("FortAgent_Cap");
	if (cn >= 0)
	{
		ref a = GetCharacter(cn);
		DeleteAttribute(a, "AlwaysEnemy");
		SetCharacterRelationBoth(cn, GetMainCharacterIndex(), RELATION_NEUTRAL);
		if (CheckAttribute(a, "SeaAI.Group.Name"))
		{
			Group_UnlockTask(a.SeaAI.Group.Name);
			Group_SetTaskNone(a.SeaAI.Group.Name);
		}
		a.lifeDay = 0; // уйдёт при ближайшей чистке фантомов / смене локации
	}
	Group_DeleteGroup("Sea_FortAgent");
}

// агент уходит с миром (приняли любое из его предложений): нейтрал, не стреляет, despawn
void FortAgent_Depart(ref agent)
{
	DeleteAttribute(agent, "AlwaysEnemy");
	agent.AlwaysFriend = true;
	agent.ShipEnemyDisable = true;
	agent.DontDeskTalk = true; // сделка заключена - больше не пускаем на палубу (иначе можно переиграть выбор до despawn'а)
	SetCharacterRelationBoth(sti(agent.index), GetMainCharacterIndex(), RELATION_NEUTRAL);
	if (CheckAttribute(agent, "SeaAI.Group.Name"))
	{
		Group_UnlockTask(agent.SeaAI.Group.Name);
		Group_SetTaskNone(agent.SeaAI.Group.Name);
	}
	pchar.FortAgentImmunize = true; // на возврате в море (Cabin_ReloadEndFadeAfter) повесим иммунитет SpeakTruce - ГГ не добьёт уходящего
	pchar.FortAgentResolved = true; // сделка заключена - блокируем повторные предложения в палубном диалоге (узел "First time")
	agent.lifeDay = 0; // уйдёт при ближайшей чистке фантомов / смене локации
}

// LEO: предательство наказывается один раз ценой доверенной поставки, независимо от выплаты агента.
bool FortAgent_TakeSide(ref agent)
{
	if (!CheckAttribute(pchar, "CargoQuest.GiveTraderID")) return false;
	if (sti(pchar.CargoQuest.iOrderType) != 7) return false;
	if (!CheckQuestAttribute("generate_trade_quest_progress", "begin")) return false;
	if (CheckAttribute(pchar, "CargoQuest.SideTaken")) return false;
	if (CheckAttribute(pchar, "FortAgentResolved")) return false;
	int iGive = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
	if (iGive < 0) return false;
	int iAg = sti(agent.nation);
	int iTr = -1;
	if (CheckAttribute(pchar, "CargoQuest.iTradeNation")) iTr = sti(pchar.CargoQuest.iTradeNation);
	int iFortCol = FindColony(pchar.CargoQuest.iTradeColony);
	// LEO: агент враждебен форту-получателю, но может быть соотечественником отправителя.
	if (iFortCol >= 0)
	{
		if (CheckAttribute(&Colonies[iFortCol], "nation"))
		{
			if (Colonies[iFortCol].nation != "none" && Colonies[iFortCol].nation != "") iTr = sti(Colonies[iFortCol].nation);
		}
	}
	if (iTr < 0 || iTr >= PIRATE) return false;
	if (iAg == iTr) return false;
	int value = FreightConvoy_CargoValue();
	pchar.CargoQuest.SideTaken = true;
	pchar.FortAgentResolved = true;
	ChangeCharacterNationReputation(pchar, iAg, 5);
	ChangeCharacterNationReputation(pchar, iTr, -FreightConvoy_NationPenalty(value));
	ChangeCharacterReputation(pchar, -FreightConvoy_HonourPenalty(value));
	return true;
}

// ВАРИАНТ А: передать суда конвоя агенту за x2 выплаты (платит сразу на палубе)
void FortAgent_HandOver(ref agent)
{
	if (!FortAgent_TakeSide(agent)) return;
	AddMoneyToCharacter(pchar, sti(pchar.CargoQuest.iMoney) * 2);
	AddCharacterExpToSkill(pchar, "Leadership", 100);
	AddCharacterExpToSkill(pchar, "Sneak", 100);
	FortConvoy_Disband(); // суда конвоя уходят к агенту

	AddQuestRecord("DELIVERY_TRADE_QUEST", "fortagent2");
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sSex", GetSexPhrase("","а"));
	pchar.quest.generate_trade_quest_progress = "";
	pchar.quest.generate_trade_quest.over = "yes";
	CloseQuestHeader("DELIVERY_TRADE_QUEST");
	FortHunt_ClearWatch();
	FortAgent_Depart(agent);
}

// льгота прохода к форту нации-заказчика при самовозе: форт под нужным флагом не разоблачает
// игрока ВЕСЬ срок доставки (от сделки; после срока - чекает как обычно). Зовётся из проверки флага форта (AIShip.c)
bool FortAgent_FortGrace(ref rFort)
{
	return FortAgent_DeliverGraceNation(sti(rFort.nation));
}

// общий грейс самовоза для нации iNation: и форт в море (FortAgent_FortGrace), и стража на суше (FortAgent_LandGrace).
// Окно = срок доставки + 2 дня (бэкстоп; реальная граница - флаг FortAgentDeliver.active, снимается чисткой/провалом/сдачей+1день).
bool FortAgent_DeliverGraceNation(int iNation)
{
	if (!CheckAttribute(pchar, "FortAgentDeliver.active")) return false;
	if (iNation != sti(pchar.FortAgentDeliver.nation)) return false;
	int iGraceDays = 6;
	if (CheckAttribute(pchar, "FortAgentDeliver.days")) iGraceDays = sti(pchar.FortAgentDeliver.days);
	if (GetNpcQuestPastDayWOInit(pchar, "FortAgentGraceDate") > iGraceDays + 2) return false;
	return true;
}

// грейс на СУШЕ: стража города нации-адресата пропускает ГГ (самовоз груза по сделке с агентом). Зовётся из проверки патруля (LAi_patrol*.c)
bool FortAgent_LandGrace(int iNation)
{
	return FortAgent_DeliverGraceNation(iNation);
}

// LEO: тот же грейс, но по НОМЕРУ СЛОТА НПС - зовут и из диалогов (ref), и из шаблонов ИИ (aref).
//   Нация города надёжнее личного флага: стража перегенерируется по таймеру и после смены владельца
//   какое-то время носит прежнюю нацию, из-за чего ГГ с законным самовозом не пускали в форт.
//   Проверка строго расширяющая: не совпало по колонии - пробуем личный флаг (наёмники бывают чужой нации).
bool FortAgent_LandGraceIdx(int idx)
{
	// зовётся на каждую встречу со стражей - отсекаем одним атрибутом, иначе впустую крутится FindColony
	if (!CheckAttribute(pchar, "FortAgentDeliver.active")) return false;
	if (idx < 0) return false;
	if (CheckAttribute(&Characters[idx], "city"))
	{
		if (FindColony(Characters[idx].city) >= 0)
		{
			if (FortAgent_LandGrace(GetCityNation(Characters[idx].city))) return true;
		}
	}
	return FortAgent_LandGrace(sti(Characters[idx].nation));
}

// окно безопасного ухода после сдачи самовоза истекло (+1 день): снимаем грейс
void FortAgentDeliver_Cleanup()
{
	DeleteAttribute(pchar, "FortAgentDeliver");
	FreightAmbush_Disarm(); // самовоз завершён - засада больше не нужна
}

// ============================================================================
//  ЗАСАДА ПИРАТОВ НА САМОВОЗ КОМЕНДАНТСКОГО КОНВОЯ: пираты прознали про казённый порох
//  и поджидают У ОСТРОВА-АДРЕСАТА. Армируется при ПРИНЯТИИ самовоза (FortAgent_StartDeliver):
//  ставим условие на ЛОКАЦИЮ острова (SetFunctionLocationCondition) - при входе в его море
//  спавним пиратскую эскадру через Group_SetAddress(остров,"","") (грузится ВМЕСТЕ с морем острова).
//  Образец - квест сопровождения флейта "Орион" (Andre_Abel_Quest_Battle_With_Pirates_Squadron).
// ============================================================================

// взвести засаду у острова-адресата (зовётся при принятии самовоза; sIsland - остров форта-адресата)
void FreightAmbush_Arm(string sIsland)
{
	DeleteAttribute(pchar, "quest.FreightAmbush_OnIsland"); // сброс прошлого арма
	DeleteAttribute(pchar, "FreightAmbushIsland");
	if (sIsland == "") return;
	// LEO: засада ВСЕГДА (было ~50% на дневном drand). Сделка с посыльным - это увод казённого пороха
	//   вражескому коменданту: о таком грузе пираты узнают наверняка, и ждут его гарантированно.
	pchar.FreightAmbushIsland = sIsland;
	SetFunctionLocationCondition("FreightAmbush_OnIsland", sIsland, false); // при входе в море острова -> FreightAmbush_OnIsland
}

// снять засаду (если самовоз сдан/провален до срабатывания)
void FreightAmbush_Disarm()
{
	DeleteAttribute(pchar, "quest.FreightAmbush_OnIsland");
	DeleteAttribute(pchar, "FreightAmbushIsland");
	Group_DeleteGroup("FreightAmbushPirates");
}

// спавн пиратской засады в море острова (вызов по .function из win_condition location)
void FreightAmbush_OnIsland(string sQuest)
{
	// одноразово: SetFunctionLocationCondition(...,false) -> OnQuestComplete сам ставит over="yes"
	if (!CheckAttribute(pchar, "FortAgentDeliver.active")) return; // только активный самовоз
	if (!CheckAttribute(pchar, "FreightAmbushIsland")) return;
	string sIsland = pchar.FreightAmbushIsland;

	int iShipClass = sti(RealShips[sti(pchar.Ship.Type)].Class);
	if (iShipClass > 5) iShipClass = 5; // не громадим эскадру против мелочи
	if (iShipClass < 2) iShipClass = 2; // LEO: ДУ-засада не мощнее 2 класса (без мановаров 1 класса)

	ref sld;
	int i;
	// LEO: размер засады = размер эскадры ГГ (сам ГГ + компаньоны). Пираты знают, за чем идут, и
	// снаряжают ровно столько кораблей, сколько у конвоя - в одиночку не задавят, всей эскадрой не сомнут.
	int iAmbushShips = GetCompanionQuantity(pchar);
	if (iAmbushShips < 1) iAmbushShips = 1;
	if (iAmbushShips > COMPANION_MAX) iAmbushShips = COMPANION_MAX;
	Group_DeleteGroup("FreightAmbushPirates");
	Group_FindOrCreateGroup("FreightAmbushPirates");
	for (i = 1; i <= iAmbushShips; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("FreightAmbushPirate" + i, "pirate_" + (rand(10) + 1), "man", "man", 15, PIRATE, -1, true));
		FantomMakeCoolFighter(sld, makeint(pchar.rank) + rand(8) + 3, 60, 50, "blade24", "pistol3", 30);
		FantomMakeCoolSailor(sld, GetShipTypeExt(iShipClass, iShipClass, "war", -1), GetConvertStrWithReplace("Variable_Treasure_7", "Names.txt", "#space#", " "), CANNON_TYPE_CANNON_LBS24, 75, 70, 65);
		Fantom_SetCannons(sld, "pirate"); // пушки под корабль
		sld.AlwaysEnemy = true;
		sld.DontRansackCaptain = false; // можно взять на абордаж
		Group_AddCharacter("FreightAmbushPirates", "FreightAmbushPirate" + i);
		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
	}
	Group_SetType("FreightAmbushPirates", "war");
	Group_SetGroupCommander("FreightAmbushPirates", "FreightAmbushPirate1");
	Group_SetTaskAttack("FreightAmbushPirates", PLAYER_GROUP);
	Group_LockTask("FreightAmbushPirates");
	Group_SetAddress("FreightAmbushPirates", sIsland, "", ""); // спавн в море острова-адресата
	Group_SetPursuitGroup("FreightAmbushPirates", PLAYER_GROUP);
	if (LanguageGetLanguage() == "russian") Log_Info("Засада! Пираты прознали про казённый конвой и поджидали у острова.");
	else Log_Info("Ambush! Pirates learned of the state convoy and lay in wait by the island.");
}

// ============================================================================
//  ЭСКОРТ (подтип 2 губернаторского, [[gov-escort-fork]]): МОРСКОЕ СОБЫТИЕ В ПУТИ.
//  Армируется при взятии эскорта (trade_quest_open). 50/50: ПЕРЕХВАТЧИКИ "Джентльменов
//  Удачи" (гонятся по глобалке) ИЛИ агентура-перенаправление (согласие агенту = ЗАСАДА
//  у острова-адресата, как Орион/комендантский FreightAmbush).
// ============================================================================

// ПЕРЕХВАТЧИКИ ДУ: 6-12 ОБЫЧНЫХ (не cool) пиратских судов КЛАССОМ НИЖЕ флагмана ГГ. Гонятся по глобалке за казённым грузом.
// Каждый капитан сдаётся в каюте (GentlemenHunter); за пощаду даёт ПОЛОВИНУ награды +50 скрытность/+50 авторитет (флаг GovEscortHalfReward; засада - полную +100/+100).
// КЛЮЧЕВОЕ (образец пасхалки "Sea_Devs0", GoldFleet.c): имя группы = "Sea_" + id КОМАНДИРА. Тогда движок
// при входе в бой (ALONE-энкаунтер -> Sea_LoginGroup) находит ГОТОВУЮ группу и грузит ВСЕХ её членов,
// а не одного командира. Иначе (имя группы != "Sea_"+id) в бой попадал только флагман.
void GovEscort_DUInterceptors()
{
	pchar.CargoQuest.GovEscortFought = true; // LEO: метка «бой был» для доклада губернатору
	ref sld;
	int i;

	int iClass = GetCharacterShipClass(pchar) + 1; // на класс СЛАБЕЕ флагмана (1 сильный ... 7 слабый)
	if (iClass > 7) iClass = 7;
	if (iClass < 2) iClass = 2; // LEO: ДУ не мощнее 2 класса (даже если флагман ГГ - мановар 1 класса)
	// LEO: тип брался ОДИН на всю эскадру - отсюда ряд близнецов. Теперь свой на каждый корабль: индекс
	//   в пуле = дневной dRand + номер. Класс прежний, мощь не меняется.
	int duSeed = dRandSlot(4, 97);

	int n = 6 + dRandSlotLow(1, 6); // 6..12 кораблей (дневной drand; лимит локации MAX_SHIPS_IN_LOCATION=32 - хватает)

	string sGroup = "Sea_GovEscortDU0"; // = "Sea_" + id командира (GovEscortDU0) - условие полной загрузки группы в бой
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);

	for (i = 0; i < n; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("GovEscortDU" + i, "pirate_" + (rand(24) + 1), "man", "man", sti(pchar.rank) + 3, PIRATE, 8, true));
		sld.Ship.Type = GenerateShipExt(GetShipTypeExtByIdx(iClass, iClass, "war", -1, duSeed + i), true, sld); // класс НИЖЕ флагмана, тип - свой у каждого
		SetRandomNameToShip(sld);
		SetFantomParamHunter(sld); // ОБЫЧНЫЕ навыки/AI/HP/деньги по классу корабля (Fantom_SetRandomSkills - НЕ cool)
		ResetShipCannonsDamages(sld);
		SetBaseShipData(sld);
		SetCrewQuantityFull(sld);
		Fantom_SetCannons(sld, "pirate"); // пушки под корабль
		Fantom_SetBalls(sld, "war");
		SetCaptanModelByEncType(sld, "war");
		sld.AlwaysEnemy = true;
		sld.DontRansackCaptain = false; // абордаж: капитан сдаётся в каюте
		sld.GentlemenHunter = true;       // каютная сдача (как TraderHunter)
		sld.NoOfficerHire = true;         // пленного ДУ нельзя нанять офицером в трюме
		sld.GovEscortHalfReward = true;   // перехватчики (слабый рой) - за пощаду в каюте ПОЛОВИНА: +50 скрытность/+50 авторитет (засада даёт +100/+100)
		sld.mapEnc.type = "war";
		sld.mapEnc.Name = GetConvertStrWithReplace("Variable_Treasure_1", "Names.txt", "#space#", " ");
		sld.hunter = "pirate";
		// модель корабля на глобалке (виден только командир) - быстрый пиратский
		if (makeint(pchar.rank) < 7)       sld.mapEnc.worldMapShip = "pirates_shooner";
		else if (makeint(pchar.rank) < 11) sld.mapEnc.worldMapShip = "pirates_bark";
		else if (makeint(pchar.rank) < 16) sld.mapEnc.worldMapShip = "pirates_frigate";
		else                               sld.mapEnc.worldMapShip = "pirates_manowar";
		LAi_group_MoveCharacter(sld, "GovEscortDU"); // боевая LAi-группа (как у Sea_Devs0)
		Group_AddCharacter(sGroup, sld.id);
		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
		if (i == 0)
		{
			Group_SetGroupCommander(sGroup, sld.id);
			Map_CreateFastWarrior("", sld.id, 8); // быстрый перехватчик на глобалке (командир тянет всю группу в бой)
		}
	}
}

// диспетчер морского события эскорта: 50/50 перехватчики ДУ / союзный посыльный (перенаправление+засада).
void GovEscort_MarineEvent()
{
	if (dRandSlotLow(2, 99) < 50) GovEscort_DUInterceptors(); // 50% - перехватчики в пути
	else GovEscortAgentOnMap();                               // 50% - союзный посыльный (перенаправление/засада)
}

// ============================================================================
//  ЭСКОРТ, ВАРИАНТ АГЕНТА (вторая половина события 50/50): союзная агентура короны-заказчика
//  перехватывает в море (палубный разговор, как FortAgent), предупреждает о пиратах на маршруте
//  и предлагает увести казённый груз в ДРУГОЙ порт той же короны <Z>. Согласие -> назначение <Z>,
//  оплата x5, возможна засада у острова <Z>. Отказ -> агент
//  уходит с миром; 50% обычные перехватчики ДУ, везёшь в исходный <Y> за обычную плату.
// ============================================================================

bool GovEscortAgent_IsCity(int iColony, int iNation, string sExclude, string sExclude2)
{
	if (iColony < 0 || iColony >= MAX_COLONIES) return false;
	ref colony = &Colonies[iColony];
	if (!CheckAttribute(colony, "id")) return false;
	string sCity = colony.id;
	if (sCity == "" || sCity == "none" || sCity == "Panama") return false;
	if (sCity == sExclude || sCity == sExclude2) return false;
	// LEO: строковая нация "none" приводится к Англии; необитаемые записи не являются портами доставки.
	if (!CheckAttribute(colony, "nation")) return false;
	if (colony.nation == "" || colony.nation == "none") return false;
	if (sti(colony.nation) != iNation) return false;
	if (!CheckAttribute(colony, "from_sea")) return false;
	if (colony.from_sea == "") return false;
	return true;
}

// LEO: другой морской порт той же короны; дневной выбор исключает отправление и прежнее назначение.
string GovEscortAgent_FindCity(int iNation, string sExclude, string sExclude2)
{
	string aCities[40];
	int n = 0;
	int i;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		if (!GovEscortAgent_IsCity(i, iNation, sExclude, sExclude2)) continue;
		if (n < 40) { aCities[n] = Colonies[i].id; n = n + 1; }
	}
	if (n == 0) return "";
	if (n == 1) return aCities[0];
	int iPick = dRandSlotLow(2, n - 1); // дневной drand; индекс массива - через переменную (гоча Storm)
	return aCities[iPick];
}

string GovEscortAgent_GetCity(ref agent)
{
	if (!CheckAttribute(pchar, "CargoQuest.GovEscortActive")) return "";
	if (CheckAttribute(pchar, "GovEscortAgentResolved")) return "";
	if (!CheckAttribute(pchar, "CargoQuest.GiveTraderID")) return "";
	int iGive = GetCharacterIndex(pchar.CargoQuest.GiveTraderID);
	if (iGive < 0) return "";
	string sFrom = characters[iGive].city;
	string sZ = "";
	if (CheckAttribute(pchar, "CargoQuest.GovEscortAgentZ")) sZ = pchar.CargoQuest.GovEscortAgentZ;
	int iNation = sti(agent.nation);
	if (GovEscortAgent_IsCity(FindColony(sZ), iNation, pchar.CargoQuest.iTradeColony, sFrom)) return sZ;
	// LEO: исправляем сохранённое неверное предложение до показа диалога; допустимый выбор не перебрасываем.
	sZ = GovEscortAgent_FindCity(iNation, pchar.CargoQuest.iTradeColony, sFrom);
	pchar.CargoQuest.GovEscortAgentZ = sZ;
	return sZ;
}

// спавн агента на глобалке. Нация = корона-заказчик (GiveTraderID). На карте гонится как враг
// (блок "пропустить"), в море нейтрализуется (wdmEnterSeaQuest по подстроке id "GovEscortAgent").
void GovEscortAgentOnMap()
{
	int iNation = sti(characters[GetCharacterIndex(pchar.CargoQuest.GiveTraderID)].nation);
	// LEO: исключаем И город назначения, И город ОТПРАВЛЕНИЯ - иначе агент предлагал увести конвой
	//   ровно туда, где ты этот фрахт и взял (репорт: фрахт в Форт-де-Франсе -> предложение везти в него же).
	string sFrom = characters[GetCharacterIndex(pchar.CargoQuest.GiveTraderID)].city;
	string sZ = GovEscortAgent_FindCity(iNation, pchar.CargoQuest.iTradeColony, sFrom);
	if (sZ == "") { GovEscort_DUInterceptors(); return; } // некуда перенаправлять - просто перехватчики

	DeleteAttribute(pchar, "GovEscortAgentResolved");
	pchar.CargoQuest.GovEscortAgentZ = sZ; // куда агент предлагает увести (для текста диалога)

	string sCapId = "GovEscortAgent_Cap";
	string sGroup = "Sea_GovEscortAgent";

	string sMapShip = "british";
	if (iNation == FRANCE)  sMapShip = "french";
	if (iNation == SPAIN)   sMapShip = "spanish";
	if (iNation == HOLLAND) sMapShip = "dutch";
	if (makeint(pchar.rank) < 7)       sMapShip = sMapShip + "_lugger";
	else if (makeint(pchar.rank) < 11) sMapShip = sMapShip + "_brig";
	else if (makeint(pchar.rank) < 16) sMapShip = sMapShip + "_frigate";
	else                               sMapShip = sMapShip + "_battleship";

	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);

	ref sld = GetCharacter(NPC_GenerateCharacter(sCapId, "off_" + NationShortName(iNation) + "_" + (rand(1) + 1), "man", "man", sti(pchar.rank) + 2, iNation, 8, true));
	int agClass = GetCharacterShipClass(pchar); // корабль того же класса, что у флагмана ГГ
	if (agClass < 1) agClass = 3;
	if (agClass < 2) agClass = 2; // LEO: посыльный не мощнее 2 класса (без мановара 1 класса), как FortAgent
	sld.Ship.Type = GenerateShipExt(GetShipTypeExt(agClass, agClass, "war", iNation), true, sld); // дать РЕАЛЬНЫЙ корабль (иначе на карте "пустышка")
	SetRandomNameToShip(sld);
	SetFantomParamHunter(sld);     // обычный (не cool): агент не воюет, лишь останавливает для разговора
	ResetShipCannonsDamages(sld);
	SetBaseShipData(sld);
	SetCrewQuantityFull(sld);
	Fantom_SetCannons(sld, "war");
	Fantom_SetBalls(sld, "war");
	SetCaptanModelByEncType(sld, "war");
	sld.AlwaysEnemy = true;        // на карте гонится и блокирует "пропустить"; в море нейтрализуем
	sld.DontRansackCaptain = true; // разговор на палубе через шлюпку, не абордаж
	sld.GovEscortAgent = true;     // распознаём при входе в море (wdmEnterSeaQuest)
	sld.mapEnc.type = "war";
	sld.mapEnc.Name = GetConvertStrWithReplace("Variable_Treasure_10", "Names.txt", "#space#", " "); // "Посыльный"
	sld.hunter = "hunter";
	sld.mapEnc.worldMapShip = sMapShip;
	sld.Dialog.Filename = "GovEscortAgent_dialog.c";
	sld.Dialog.CurrentNode = "First time";
	sld.DeckDialogNode = "First time"; // палубный визит держит свой диалог (иначе Cabin.c форсит Capitans_dialog.c)
	sld.greeting = "Gr_Officer";
	Group_AddCharacter(sGroup, sCapId);

	Group_SetGroupCommander(sGroup, sCapId);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Map_CreateFastWarrior("", sCapId, 8);
}

// агент уходит с миром (после согласия/отказа): нейтрал, не стреляет, despawn. Despawn делаем НА ВЫХОДЕ
// из палубного диалога (despawn во время диалога = софтлок). Обёртка по id.
void GovEscortAgent_Depart(ref agent)
{
	DeleteAttribute(agent, "AlwaysEnemy");
	agent.AlwaysFriend = true;
	agent.ShipEnemyDisable = true;
	agent.DontDeskTalk = true; // решение принято - больше на палубу не пускаем
	SetCharacterRelationBoth(sti(agent.index), GetMainCharacterIndex(), RELATION_NEUTRAL);
	if (CheckAttribute(agent, "SeaAI.Group.Name"))
	{
		Group_UnlockTask(agent.SeaAI.Group.Name);
		Group_SetTaskNone(agent.SeaAI.Group.Name);
	}
	pchar.GovEscortAgentResolved = true;
	agent.lifeDay = 0; // уйдёт при ближайшей чистке фантомов / смене локации
}
void GovEscortAgent_Depart_OnExit()
{
	int cn = GetCharacterIndex("GovEscortAgent_Cap");
	if (cn < 0) return;
	GovEscortAgent_Depart(GetCharacter(cn));
}

// LEO: согласие меняет порт и ставит оплату x5; вероятность засады определяет GovEscortAmbush_Arm.
bool GovEscortAgent_Redirect(ref agent)
{
	string sZ = "";
	if (CheckAttribute(pchar, "CargoQuest.GovEscortAgentZ")) sZ = pchar.CargoQuest.GovEscortAgentZ;
	// LEO: после смены владельца порта или загрузки старого сейва нельзя принять уже недействительное предложение.
	if (GovEscortAgent_GetCity(agent) != sZ || sZ == "") return false;
	pchar.CargoQuest.iTradeColony = sZ;  // новое назначение - туда и сдаём губернатору
	pchar.CargoQuest.iTradeIsland = sZ;  // остров=город -> в текстах не лепится клаузула "что на <остров>"
	pchar.CargoQuest.GovEscortAgentMult = 5; // x5 при сдаче (GovEscort_Deliver)

	// новый срок: путь от ТЕКУЩЕЙ акватории ГГ до острова-адресата Z + запас (как у самовоза конвоя)
	string sFromIsl = GetCharacterCurrentIslandId(pchar);
	if (sFromIsl == "" && CheckAttribute(&AISea, "Island")) sFromIsl = AISea.Island;
	int iDays = GetMaxDaysFromIsland2Island(sFromIsl, GetArealByCityName(sZ));
	if (iDays < 1) iDays = 10; else iDays = iDays + 3; // в море маршрут не вычислить - щедрый фоллбек
	if (iDays > 20) iDays = 20;
	pchar.CargoQuest.iDaysExpired = iDays;
	SetTimerCondition("generate_trade_quest", 0, 0, iDays + 1, false); // таймер просрочки пересчитываем ОТ СДЕЛКИ с агентом

	GovEscortAmbush_Arm(GetArealByCityName(sZ));
	ChangeCharacterNationReputation(pchar, sti(agent.nation), 5); // +отношение короны-заказчика

	// ВАЖНО: AddQuestRecord СНАЧАЛА - AddQuestUserData цепляет данные к ПОСЛЕДНЕЙ записи (иначе плейсхолдеры пусты)
	AddQuestRecord("DELIVERY_TRADE_QUEST", "esc3");
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sRedirectCity", XI_ConvertString("Colony" + sZ + "Acc")); // винительный: "увести в <город>"
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sRedirectMoney", FindRussianMoneyString(sti(pchar.CargoQuest.iMoney) * 5));
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sRedirectDays", FindRussianDaysString(iDays));
	pchar.GovEscortAgentResolved = true;
	return true;
}

// ОТКАЗ агенту: агент уходит, и 50% обычные перехватчики ДУ (везём в исходный <Y> за обычную плату).
void GovEscortAgent_RefuseResolve_OnExit()
{
	GovEscortAgent_Depart_OnExit();
	if (dRandSlotLow(3, 99) < 50) GovEscort_DUInterceptors(); // 50% - перехватчики всё равно поджидают
}

// despawn агента при закрытии заказа (сдан/провален) - ловить больше нечего
void GovEscortAgent_Cleanup()
{
	int cn = GetCharacterIndex("GovEscortAgent_Cap");
	if (cn >= 0)
	{
		ref a = GetCharacter(cn);
		DeleteAttribute(a, "AlwaysEnemy");
		SetCharacterRelationBoth(cn, GetMainCharacterIndex(), RELATION_NEUTRAL);
		if (CheckAttribute(a, "SeaAI.Group.Name"))
		{
			Group_UnlockTask(a.SeaAI.Group.Name);
			Group_SetTaskNone(a.SeaAI.Group.Name);
		}
		a.lifeDay = 0;
	}
	Group_DeleteGroup("Sea_GovEscortAgent");
}

// ЗАСАДА у острова-адресата Z. Образец - FreightAmbush_*.
void GovEscortAmbush_Arm(string sIsland)
{
	DeleteAttribute(pchar, "quest.GovEscortAmbush_OnIsland");
	DeleteAttribute(pchar, "GovEscortAmbushIsland");
	if (sIsland == "") return;
	// LEO: ~50% на дневном drand (было 100%). Перенаправление груза - дело союзной агентуры, о новом
	//   маршруте пираты узнают не всегда; сейв-лоадом бросок не переролить.
	if (dRandSlotLow(4, 1) != 0) return;
	pchar.GovEscortAmbushIsland = sIsland;
	SetFunctionLocationCondition("GovEscortAmbush_OnIsland", sIsland, false); // при входе в море острова Z -> спавн засады
}
void GovEscortAmbush_Disarm()
{
	DeleteAttribute(pchar, "quest.GovEscortAmbush_OnIsland");
	DeleteAttribute(pchar, "GovEscortAmbushIsland");
	Group_DeleteGroup("GovEscortAmbushPirates");
}
void GovEscortAmbush_OnIsland(string sQuest)
{
	if (!CheckAttribute(pchar, "CargoQuest.GovEscortActive")) return; // только активный эскорт
	if (!CheckAttribute(pchar, "GovEscortAmbushIsland")) return;
	string sIsland = pchar.GovEscortAmbushIsland;
	// LEO: метка «бой был» для доклада губернатору. Перенаправление само по себе боя не значит:
	//   засаду можно проскочить, и ГГ докладывал о сражении, которого не было.
	pchar.CargoQuest.GovEscortFought = true;

	int iClass = GetCharacterShipClass(pchar); // ТОТ ЖЕ класс, что у флагмана ГГ (засада - злая, в отличие от перехватчиков)
	if (iClass > 7) iClass = 7;
	if (iClass < 2) iClass = 2; // но не мощнее 2 класса (на мановарах 1 класса не рассекать)
	int duSeed = dRandSlot(4, 97); // LEO: свой тип корабля каждому (был один на всех - эскадра из клонов)
	int n = 4 + dRandSlotLow(1, 4); // 4..8 (меньше перехватчиков, но кулсейлоры/кулфайтеры)

	ref sld;
	int i;
	Group_DeleteGroup("GovEscortAmbushPirates");
	Group_FindOrCreateGroup("GovEscortAmbushPirates");
	for (i = 0; i < n; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("GovEscortAmbushPirate" + i, "pirate_" + (rand(24) + 1), "man", "man", sti(pchar.rank) + 3, PIRATE, 8, true));
		FantomMakeCoolFighter(sld, makeint(pchar.rank) + rand(8) + 3, 60, 50, "", "", 30); // кулфайтер с ХОРОШИМ клинком (по рангу, авто), но БЕЗ пушки
		FantomMakeCoolSailor(sld, GetShipTypeExtByIdx(iClass, iClass, "war", -1, duSeed + i), "", CANNON_TYPE_CANNON_LBS24, 80, 75, 70); // кулсейлор, тот же класс, но свой тип у каждого; имя "" -> СЛУЧАЙНОЕ
		Fantom_SetCannons(sld, "pirate");
		SetCaptanModelByEncType(sld, "war");
		sld.Money = 0;              // золото с трупа не выпадает
		sld.GovEscortNoLoot = true; // и оружие не снять: стрип предметов в LAi_boarding перед обыском (хороший клинок в бою, но не нафармить). Награда - пощада в каюте (+100/+100)
		sld.AlwaysEnemy = true;
		sld.DontRansackCaptain = false; // абордаж: капитан сдаётся в каюте (+100 авторитет/+100 скрытность за пощаду - GentlemenHunter)
		sld.GentlemenHunter = true;
		sld.NoOfficerHire = true;
		sld.hunter = "pirate";
		Group_AddCharacter("GovEscortAmbushPirates", "GovEscortAmbushPirate" + i);
		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
	}
	Group_SetType("GovEscortAmbushPirates", "war");
	Group_SetGroupCommander("GovEscortAmbushPirates", "GovEscortAmbushPirate0");
	Group_SetTaskAttack("GovEscortAmbushPirates", PLAYER_GROUP);
	Group_LockTask("GovEscortAmbushPirates");
	Group_SetAddress("GovEscortAmbushPirates", sIsland, "", ""); // спавн в море острова-адресата
	// не прямо под бортом ГГ, а в ~500 от него: смещаем точку спавна группы от позиции корабля ГГ
	if (CheckAttribute(pchar, "Ship.Pos.x")) Group_SetXZ_AY("GovEscortAmbushPirates", stf(pchar.Ship.Pos.x) + 500.0, stf(pchar.Ship.Pos.z) + 200.0, 0.0);
	Group_SetPursuitGroup("GovEscortAmbushPirates", PLAYER_GROUP);
	if (LanguageGetLanguage() == "russian") Log_Info("Засада! Пираты прознали про казённый груз и поджидали у острова.");
	else Log_Info("Ambush! Pirates learned of the state cargo and lay in wait by the island.");
}

// ВАРИАНТ Б: берёмся сами довезти груз в форт нации агента за x5 (платит тот комендант при сдаче)
void FortAgent_StartDeliver(ref agent)
{
	if (!FortAgent_TakeSide(agent)) return;
	int iAg = sti(agent.nation);
	string sCity = FortAgent_FindNationCity(iAg);

	// срок доставки = время пути от текущей акватории до форта-адресата + запас.
	// В море GetCharacterCurrentIslandId часто пуст (идёт палубный визит) - тогда маршрут не вычислить, даём щедрый фоллбек.
	string sFromIsl = GetCharacterCurrentIslandId(pchar);
	if (sFromIsl == "" && CheckAttribute(&AISea, "Island")) sFromIsl = AISea.Island;
	int iDeliverDays = GetMaxDaysFromIsland2Island(sFromIsl, GetArealByCityName(sCity)); // ареал, а не GetIslandByCityName (та даёт лейбл "Mein" - нет города на острове, краш TravelMap + неверный срок)
	if (iDeliverDays < 1) iDeliverDays = 10;  // маршрут не вычислить (в море): форт может быть на другом конце архипелага - щедро
	else iDeliverDays = iDeliverDays + 4;     // запас на медленный конвой, навигацию и стычки в пути (без запаса не успеть)
	if (iDeliverDays > 20) iDeliverDays = 20; // разумный потолок

	pchar.FortAgentDeliver.active = true;
	pchar.FortAgentDeliver.nation = iAg;
	pchar.FortAgentDeliver.city   = sCity;
	pchar.FortAgentDeliver.days   = iDeliverDays;
	// форт-льгота: отсчёт с момента сделки, длится ВЕСЬ срок доставки (FortAgent_FortGrace)
	SaveCurrentNpcQuestDateParam(pchar, "FortAgentGraceDate");

	// глушим исходный комендантский квест (к нации заказчика): груз теперь везём вражескому коменданту.
	// progress="" - штатный таймер generate_trade_quest не пометит failed (нет долга гильдии); конвой и CargoQuest.* живут
	pchar.quest.generate_trade_quest_progress = "";

	// таймер провала отсчитывается ОТ СДЕЛКИ (SetTimerCondition = текущая дата + iDeliverDays), не от взятия фрахта
	SetTimerCondition("FortAgentDeliver_Fail", 0, 0, iDeliverDays, false);

	FortConvoy_SetNation(iAg); // конвой везём ПОД ФЛАГОМ нации-адресата - иначе форт/стража палят по нему как по вражескому по нации (льгота прохода ГГ суда конвоя не покрывает)

	FreightAmbush_Arm(GetArealByCityName(sCity)); // засада пиратов У ОСТРОВА(АРЕАЛА)-АДРЕСАТА (~50% дневной drand): GetArealByCityName даёт реальный id (=локация игрока в море), не лейбл "Mein"

	AddQuestRecord("DELIVERY_TRADE_QUEST", "fortagent3");
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sSex", GetSexPhrase("","а"));
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony", XI_ConvertString("Colony" + sCity + "Gen"));
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sDays", FindRussianDaysString(iDeliverDays)); // FindRussianDaysString сам даёт "N дней" (число + склонение)
	// уход агента - на ВЫХОДЕ из диалога (FortAgent_Depart_OnExit), иначе despawn во время диалога вешает выход
}

// провал самовоза (не довёз за срок): НЗГ нации-адресата + реп ГГ в ноль + конвой выходит из эскадры
void FortAgentDeliver_Fail()
{
	if (!CheckAttribute(pchar, "FortAgentDeliver.active")) return;
	int iAg = sti(pchar.FortAgentDeliver.nation);

	int iCur = ChangeCharacterNationReputation(pchar, iAg, 0);
	if (iCur > -15) ChangeCharacterNationReputation(pchar, iAg, -(iCur + 15)); // гарантированное НЗГ (порог -10)

	int iRep = sti(pchar.reputation);
	if (iRep > 0) ChangeCharacterReputation(pchar, -iRep); // личная репутация в ноль

	FortConvoy_Disband(); // конвойные компаньоны выходят из эскадры

	AddQuestRecord("DELIVERY_TRADE_QUEST", "fortagent5");
	AddQuestUserData("DELIVERY_TRADE_QUEST", "sSex", GetSexPhrase("","а"));
	pchar.quest.generate_trade_quest_progress = "";
	pchar.quest.generate_trade_quest.over = "yes";
	CloseQuestHeader("DELIVERY_TRADE_QUEST");
	FortHunt_ClearWatch();
	DeleteAttribute(pchar, "FortAgentDeliver");
	FreightAmbush_Disarm(); // самовоз провален - засада больше не нужна
}

void SetTreasureHunter(string temp)
{
    int    j, i, k;
	string sTemp, sCapId;
	ref    sld;
	bool   ok;

	if (chrDisableReloadToLocation) return; // идёт некий другой квест с запретом выхода

    Pchar.GenQuest.Hunter2Pause            = true;

    j = GetOfficersQuantity(Pchar) + 2;
	if (IsEquipCharacterByItem(pchar, "mushket6")) j += 6;
    for (i=1; i<=MAX_NUM_FIGHTERS; i++)
    {
        if (GetOfficersIndex(pchar,i) != -1)
        {
            if (IsEquipCharacterByItem(&Characters[GetOfficersIndex(pchar,i)], "mushket6")) j += 6;
        }
    }

	sCapId = "LandHunter0";
    sTemp = "LAND_HUNTER";
	ok = true;
	arrayNPCModelHow = 0;
    for (i = 1; i <= j; i++)
    {
        if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "Pirate_" + (rand(24) + 1), "man", "man_fast", sti(PChar.rank) + 5, PIRATE, 0, true));
        else sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "Pirate_" + (rand(24) + 1), "man", "man", sti(PChar.rank) + 5, PIRATE, 0, true));
        SetFantomParamHunter(sld);
        sld.Dialog.CurrentNode = "TreasureHunter";
        sld.dialog.filename = "Hunter_dialog.c";
        sld.greeting = "Gr_HUNTER";
		if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
        sld.location = "none"; // вот где порылась собака!!!!!!!!!!!


        LAi_SetActorTypeNoGroup(sld);
        LAi_SetCheckMinHP(sld, (LAi_GetCharacterHP(sld) - 1), false, "Battle_Hunters_Land");
        if (PlaceCharacter(sld, "goto", "random_must_be_near") == "" && i == 1) // fix если вдруг нет в локации
        {
            ok = false;
            break;
        }
        LAi_ActorFollow(sld, pchar, "", 8.0);
        LAi_group_MoveCharacter(sld, sTemp);
    }

	LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER_OWN, LAI_GROUP_NEITRAL);

	LAi_group_ClearAllTargets();
	LAi_SetFightModeForOfficers(false);
	if (ok)
    {
        PChar.HunterCost = makeint(sti(Pchar.money) / 5) + rand(20)*1000; //сразу генерим
        PChar.HunterCost.Qty = j;
        PChar.HunterCost.TempHunterType = "";
        sld = characterFromID(sCapId + "1");
        LAi_type_actor_Reset(sld);
        LAi_ActorDialog(sld, pchar, "", 4.0, 0);
		chrDisableReloadToLocation = true;
		DoQuestCheckDelay("OpenTheDoors", 5.0);
    }
}

void SetPGGTreasureHunter(string temp)
{
    int    j, i, k;
	string sTemp, sCapId;
	ref    sld;
	bool   ok;

	if (chrDisableReloadToLocation) return; // идёт некий другой квест с запретом выхода

    Pchar.GenQuest.Hunter2Pause            = true;

    j = GetOfficersQuantity(Pchar) + 2;

	sCapId = "LandHunter0";
    sTemp = "LAND_HUNTER";
	ok = true;
	arrayNPCModelHow = 0;
    for (i = 1; i <= j; i++)
    {
		if (i == 1)
		{
			sld = CharacterFromID(pchar.chosenTreasureHero);
			// LEO: кандидата выбирают ещё при находке карты, а сцена идёт позже - за это время ПГГ мог
			//   уехать на турнир арены и стать неприкасаемым. Снимаем участие, иначе в бою он бессмертен.
			PGG_ReleaseFromTournamentForQuest(sld);
			sld.Dialog.CurrentNode = "TreasureHunterPGG";
			sld.location = "none"; // вот где порылась собака!!!!!!!!!!!
		}
		else
		{
			if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations)sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "Pirate_" + (rand(24) + 1), "man", "man_fast", sti(PChar.rank) + 5, PIRATE, 0, true));
			else sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "Pirate_" + (rand(24) + 1), "man", "man", sti(PChar.rank) + 5, PIRATE, 0, true));
			SetFantomParamHunter(sld);
			sld.greeting = "Gr_HUNTER";
			if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
			sld.location = "none"; // вот где порылась собака!!!!!!!!!!!
			sld.dialog.filename = "Hunter_dialog.c";
			sld.Dialog.CurrentNode = "TreasureHunter";
		}

        LAi_SetActorTypeNoGroup(sld);
        LAi_SetCheckMinHP(sld, (LAi_GetCharacterHP(sld) - 1), false, "Battle_PGGHunters_Land");
        if (PlaceCharacter(sld, "goto", "random_must_be_near") == "" && i == 1) // fix если вдруг нет в локации
        {
            ok = false;
            break;
        }
        LAi_ActorFollow(sld, pchar, "", 8.0);
        LAi_group_MoveCharacter(sld, sTemp);
    }

	LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER_OWN, LAI_GROUP_NEITRAL);

	LAi_group_ClearAllTargets();
	LAi_SetFightModeForOfficers(false);
	if (ok)
    {
        PChar.HunterCost = makeint(sti(Pchar.money) / 5) + rand(20)*1000; //сразу генерим
        PChar.HunterCost.Qty = j;
        PChar.HunterCost.TempHunterType = "";
        sld = CharacterFromID(pchar.chosenTreasureHero);
        LAi_type_actor_Reset(sld);
        LAi_ActorDialog(sld, pchar, "", 4.0, 0);
		chrDisableReloadToLocation = true;
    }
}

ref SetFantomSkeletForts(string group, string locator, string enemygroup, string _type)
// _type = "GhostShipCrew"   _type = "ParamHunter"  _type = "none"
{
    string emodel;
    ref    Cr;

    emodel = GetRandSkelModel();

    Cr = LAi_CreateFantomCharacterEx(emodel, "man", group, locator);

    if (Cr.location.locator != "")
    {
		if (_type == "GhostShipCrew" || _type == "ParamHunter")
		{
			SetFantomParamHunter(Cr);
		}
		else
		{
		    SetFantomParam(Cr);
		}
		LAi_SetWarriorType(Cr);
	    LAi_group_MoveCharacter(Cr, enemygroup);
	    LAi_NoRebirthEnable(Cr); //не показывать убитых при входе в локацию
	    LAi_LoginInCaptureTown(Cr, true); // для записи игры
	    if (_type == "GhostShipCrew")
	    {
		    Cr.GhostShipCrew = true;
	    }
    }
    return Cr;
}

///////////////////////////////////////////   Квест летучего голландца /////////////////////////////////
// летучий голландец
void  GhostShipOnMap()
{
    ref  sld;
    int  i;
    ref  rRealShip;

    sld = characterFromID("GhostCapt");

    SetBaseShipData(sld);
    rRealShip = GetRealShip(sti(sld.Ship.Type));
    rRealShip.ShipSails.SailsColor = argb(255, 60, 60, 60);  // 1.2.3 переделка цвета паруса от НПС

    DeleteAttribute(sld,"ship.sails");
    RepairMasts(sld);
    DeleteAttribute(sld,"ship.blots");

	sld.Ship.Name = GetConvertStrWithReplace("Variable_Treasure_2", "Names.txt", "#space#", " ");

    Ship_SetTaskNone(SECONDARY_TASK, sti(sld.index));

    if (CheckAttribute(pchar , "GenQuest.GhostShip.LastBattle"))
    {
		rRealShip.EmblemedSails.normalTex =  "ships\parus_common_torn.dds";

		sld.Ship.Type = GenerateShipExt(SHIP_FLYINGDUTCHMAN, true, sld);
		SetBaseShipData(sld);
		sld.Ship.Name = GetConvertStrWithReplace("Variable_Treasure_3", "Names.txt", "#space#", " ");

        DeleteAttribute(rRealShip, "ShipSails.SailsColor");  // белый парус

        sld.ship.Cannons.Type = CANNON_TYPE_CANNON_LBS42;

        Character_SetAbordageEnable(sld, true);
    }
    else
    {
        sld.ship.hp = 50000; // 13 процентов
    }
	sld.items.talisman3 = 1;
	EquipCharacterByItem(sld,"talisman3");
    SetCrewQuantityOverMax(sld, 666);
    sld.mapEnc.type = "war";
    sld.mapEnc.Name = GetConvertStrWithReplace("Variable_Treasure_4", "Names.txt", "#space#", " ");
	sld.mapEnc.worldMapShip = "Flying_Dutchman";
	SelAllPerksToNotPChar(sld);
	DeleteAttribute(sld,"perks.list.ShipEscape");
	// LEO: в ранние встречи (до финального боя) снимаем ЛЁГКИЙ РЕМОНТ. Корпус у ЛГ ~400к, а стартуют ему
	//   50к = 13%, то есть он сразу внутри окна автоматического ремонта (AIShip.c: LightRepair при hp<20%)
	//   и отхиливает по 20% от МАКСИМУМА, то есть 80к за раз по кулдауну — снести это не успевают ничем,
	//   включая брандеры. В финальном бою перк остаётся: там у него полный корпус и ремонт погоды не делает.
	if (!CheckAttribute(pchar, "GenQuest.GhostShip.LastBattle"))
	{
		DeleteAttribute(sld, "perks.list.LightRepair");
	}

    sld.ship.Crew.Morale = 90;
    ChangeCrewExp(sld, "Sailors", 100);
	ChangeCrewExp(sld, "Cannoners", 100);
	ChangeCrewExp(sld, "Soldiers", 100);

    Fantom_SetBalls(sld, "pirate");
    Fantom_SetCharacterGoods(sld, GOOD_BALLS,  4900 + rand(300), 0);
	Fantom_SetCharacterGoods(sld, GOOD_BOMBS,  5900 + rand(300), 0);
	Fantom_SetCharacterGoods(sld, GOOD_POWDER, 12000 + rand(300), 0);

    LAi_SetCurHPMax(sld); // если он умер
    string sGroup = "Sea_GhostCapt";  // приставка "Sea_" + ИД важна
    Group_FindOrCreateGroup(sGroup);
	Group_AddCharacter(sGroup, "GhostCapt");
    Group_SetGroupCommander(sGroup, "GhostCapt");

    SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
    Group_SetTaskAttack(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);

	Group_SetAddress(sGroup,"KhaelRoa","quest_ships","Quest_ship_9");

	Pchar.quest.GhostShip_Start.win_condition.l1 = "location";
	Pchar.quest.GhostShip_Start.win_condition.l1.location = "KhaelRoa";
	Pchar.quest.GhostShip_Start.win_condition = "GhostShip_Start";

    Pchar.quest.GhostShip_Dead.win_condition.l1 = "NPC_Death";
	Pchar.quest.GhostShip_Dead.win_condition.l1.character = "GhostCapt";
	Pchar.quest.GhostShip_Dead.win_condition = "GhostShip_Dead";
}

void Survive_In_Sea_Go2Land()
{
    string  sTemp;
    int     iDay;
    ref 	ch;
	int 	n, i, idx;
    int 	storeArray[30];
    int 	howStore = 0;

    // трем всех офов и компаньонов, не квестовых
    iDay = GetPassengersQuantity(pchar);
    i = 0;
    n = 0;
 	while (i < iDay)
	{
		i++;
		idx = GetPassenger(pchar, n); // всегда первый в массиве со смещением, если не трем
		if (idx==-1) break;
		ch = GetCharacter(idx);
		if (GetRemovable(ch)) // квестовых не трогаем
		{
		    ch.LifeDay = 1; // чтоб не сразу потерся
		    RemovePassenger(pchar, ch);
			ChangeCharacterAddress(ch,"none","");
		}
		else
		{
		    n++;
		}

	}
    for (i=0; i<COMPANION_MAX; i++)
	{
		idx = GetCompanionIndex(pchar, i);
		if (idx != -1)
		{
            ch = GetCharacter(idx);
			if (GetShipRemovable(ch) && GetRemovable(ch))  // не трогаем квестовых и ПГГ
			{
			    ch.LifeDay = 1; // чтоб не сразу потерся
			    RemoveCharacterCompanion(pchar, ch);
			}
		}
	}

	DeleteAttribute(pchar, "ship");
	pchar.ship.name = "";
	pchar.ship.type = SHIP_NOTUSED;
	pchar.money = 0;

	SetCharacterShipLocation(pchar, "");

	// выбор города близ бухты
    for (n = 0; n < MAX_COLONIES; n++)
	{
  		if (colonies[n].nation == "none") continue; // необитайки
  		if (sti(colonies[n].nation) == PIRATE) continue;
  		if (colonies[n].from_sea == "") continue; // необитайки
  		if (!CheckAttribute(&Colonies[n], "islandLable") || Colonies[n].islandLable == "Mein") continue; // только острова

        storeArray[howStore] = n;
   		howStore++;
    }

    makeref(ch,colonies[storeArray[rand(howStore-1)]]);
    setWDMPointXZ(ch.from_sea);
    pchar.location = ch.from_sea; // это порт, ниже ищем бухту

    // нулим предметы в каюте
    ref loc;
    if (Pchar.SystemInfo.CabinType != "")
    {
	    loc = &locations[FindLocation(Pchar.SystemInfo.CabinType)]; //"My_Cabin"

	    for (n = 1; n <= 4; n++)
	    {
	        sTemp = "box" + n;
			DeleteAttribute(loc, sTemp + ".items");
		    loc.(sTemp).items = "";
		    loc.(sTemp) = Items_MakeTime(0, 0, 1, 2003);
		    loc.(sTemp).money = 0;
	    }
    }
    loc = &locations[FindLocation("My_Deck")];
    for (n = 1; n <= 4; n++)
    {
        sTemp = "box" + n;
		DeleteAttribute(loc, sTemp + ".items");
	    loc.(sTemp).items = "";
	    loc.(sTemp) = Items_MakeTime(0, 0, 1, 2003);
	    loc.(sTemp).money = 0;
    }
	Pchar.GenQuest.Hunter2Pause = true;
    iDay = 5+rand(17);
    bQuestCheckProcessFreeze = true;
    WaitDate("",0,0,iDay, rand(10), 1); // прошло время
    bQuestCheckProcessFreeze = false;

	// бухта контры
    pchar.location = SelectSmugglingLocation();

    pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*40;
	Log_Info(GetConvertStrWithReplace("Variable_Treasure_5", "Logs.txt", "#space#", " "));
	if (sti(PChar.GenQuest.GhostShip.KillMe) <= 1)
	{
		AddQuestRecord("GhostShipQuest", "Survive_1");
		AddQuestUserData("GhostShipQuest", "sSex", GetSexPhrase("","а"));
	}
	else
	{
	    AddQuestRecord("GhostShipQuest", "Survive_2");
	    // добавить предмет Т102
	    if (sti(PChar.GenQuest.GhostShip.KillMe) == 2)
		{
			Log_Info(GetConvertStrWithReplace("Variable_Treasure_6", "Logs.txt", "#space#", " "));
			TakeNItems(Pchar, "term_arm", 1);
	    	pchar.QuestTemp.TakeShotgun = true;
	    }
	}
	sGlobalTemp = "afterFDsink";
	bDisableMapEnter           = false;   // мир, был бой с ЛГ

	SetLaunchFrameFormParam(GetConvertStrWithReplace("Variable_Treasure_1", "Frames.txt", "#space#", " ") + iDay + GetConvertStrWithReplace("Variable_Treasure_2", "Frames.txt", "#space#", " ") + NewStr() +
	                        XI_ConvertString("Island:") + GetConvertStr(ch.islandLable, "LocLables.txt")+","+ NewStr() +
							"" + GetConvertStr(pchar.location, "LocLables.txt") + ".",
						                        "Reload_To_Location", 0.1, 7.0);
    SetLaunchFrameReloadLocationParam(pchar.location, "Smugglers", "Smuggler01", "");
	LaunchFrameForm();
	DeleteAttribute(pchar,"GhostCap.Fight");
	n = FindIsland("KhaelRoa");
	DeleteAttribute(&Islands[n], "alwaysStorm");
	DeleteAttribute(&Islands[n], "storm");
	DeleteAttribute(&Islands[n], "tornado");
	DeleteAttribute(&Islands[n], "QuestlockWeather");
	DeleteAttribute(&Islands[n], "MaxSeaHeight");
	Pchar.Abordage.Enable = true;
	string sGroup = "Sea_GhostCapt";
	Group_SetAddressNone(sGroup);
    	Group_DeleteGroup(sGroup);
}

void GhostShipInit()
{
	ref sld;
	//============> Капитан призрака
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("GhostCapt", "PotCMobCap", "skeleton", "man_fast", 55, PIRATE, -1, true));
	else sld = GetCharacter(NPC_GenerateCharacter("GhostCapt", "PotCMobCap", "skeleton", "man", 55, PIRATE, -1, true));
	sld.dialog.filename      = "Quest\GhostShip_dialog.c";
	sld.dialog.currentnode   = "GhostCapt";
	sld.name 	= GetConvertStr("Variable_Treasure_0.1", "Names.txt");
	sld.lastname = GetConvertStrWithReplace("Variable_Treasure_5", "Names.txt", "#space#", " ");
	sld.reputation = 15;
	LAi_NoRebirthDisable(sld);
	LAi_RebirthOldName(sld);
	if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки
	sld.RebirthPhantom     = true;
	sld.AlwaysEnemy        = true;
	sld.DontHitInStorm     = true; // не ломается в шторм
	sld.DontRansackCaptain = true; //квестовые не сдаются
	sld.SinkTenPercent     = false;  // не тонуть после 10 процентах корпуса
	sld.Abordage.Enable    = false; // запрет абордажа
	sld.SaveItemsForDead   = true; // сохранять на трупе вещи
	sld.cirassId           = Items_FindItemIdx("cirass5");  // предмета нет, но влияение есть
	sld.AlwaysSandbankManeuver = true;  // тупым запрет тонуть об берег
	sld.GenQuest.CrewSkelMode  = true; // на абордаже скелеты
	LAi_SetHP(sld, 666, 666);
	SetCharacterPerk(sld, "Energaiser"); // скрытый перк даёт 1.5 к приросту энергии, дается ГГ и боссам уровней
    sld.SuperShooter  = true;
	DeleteAttribute(sld, "items");
	GiveItem2Character(sld, "topor2");
	EquipCharacterByItem(sld, "topor2");
	GiveItem2Character(sld, "pistol6");
	EquipCharacterByItem(sld, "pistol6");
	TakeNItems(sld, "bullet", 10);
	AddItems(sld, "GunPowder", 10);
	TakeNItems(sld,"potion2", 10);
	sld.money = 70000;

	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	sld.Ship.Type = CreateBaseShip(SHIP_FLYINGDUTCHMAN);
	sld.Ship.Name = GetConvertStrWithReplace("Variable_Treasure_6", "Names.txt", "#space#", " ");

	ref rRealShip = GetRealShip(sti(sld.Ship.Type));
	rRealShip.ship.upgrades.sails = 3;
	rRealShip.EmblemedSails.normalTex =  "ships\parus_common_torn.dds";
	rRealShip.ShipSails.SailsColor = argb(255,60,60,60);
	rRealShip.MaxCaliber = 48;
    rRealShip.Cannon     = CANNON_TYPE_CANNON_LBS42;
    rRealShip.MaxCrew    = 525;
    rRealShip.OptCrew    = 400;
    rRealShip.Capacity   = 5000;
    rRealShip.HP         = 400000;
    rRealShip.WindAgainstSpeed = 10.0;
    rRealShip.SpeedRate	 = 15.0;
	rRealShip.TurnRate   = 12.0;
	rRealShip.Price      = 500000;

	pchar.GenQuest.GhostShip.ShipType   = sti(sld.Ship.Type); // запомним, потом пригодится, когда парус нужно сменить на целый
}

// квест шотгана - терминатора Т-102 появление в комнате таверны
void QuestShotgunT102()
{
	ref sld;
	sld = GetCharacter(NPC_GenerateCharacter("T102", "Terminator", "Terminator_Sex", "Terminator", 55, PIRATE, 0, false));
	sld.dialog.filename      = "Quest\T102_dialog.c";
	FantomMakeCoolFighter(sld, 80, 80, 80, "katar", "", 80);

    sld.name  = "Terminator";
    sld.lastname = "";
	LAi_SetHP(sld, 1500.0, 1500.0);
	AddBonusEnergyToCharacter(sld, 200);
	SetCharacterPerk(sld, "BasicDefense");
	SetCharacterPerk(sld, "Tireless");
	SetCharacterPerk(sld, "HardHitter");
	SetCharacterPerk(sld, "CriticalHit");
	SetCharacterPerk(sld, "SwordplayProfessional");

	ChangeCharacterAddressGroup(sld, PChar.location, "reload", "reload1");
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	LAi_SetActorType(sld);
    LAi_SetActorType(pchar);

	SetActorDialogAny2Pchar(sld.id, "", 2.0, 0.0);
	LAi_ActorFollow(sld, pchar, "ActorDialog_Any2Pchar", 4.0);
    LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
}

void Map_CreateFastWarriorTreasure()
{
    ref  sld;
    int  i;

    string sCapId = "WarriorTreasure";
    string sGroup = "Sea_" + sCapId + "1";

	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
    for (i = 1; i <= 3; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_hol_2", "man", "man", sti(PChar.rank) + 5, PIRATE, 15, true));
		if(CheckAttribute(PChar,"Luke.SpawnMaks"))
		{
			CreatePGG_War(sld, PIRATE, pchar);
			if (!CheckAttribute(sld, "Ship.Cannons.Type")) Fantom_SetCannons(sld, "pirate");
			if (!CheckAttribute(sld, "Ship.Cargo.Goods")) Fantom_SetBalls(sld, "pirate");
		}
		else SetShipHunter(sld);
        SetFantomParamHunter(sld);
        SetCaptanModelByEncType(sld, "war");
        sld.AlwaysEnemy = true;
        sld.DontRansackCaptain = true;
        sld.mapEnc.type = "war";
        sld.mapEnc.Name = GetConvertStrWithReplace("Variable_Treasure_7", "Names.txt", "#space#", " ");
		sld.hunter = "pirate";
        Group_AddCharacter(sGroup, sCapId + i);
    }

    Group_SetGroupCommander(sGroup, sCapId+ "1");
    Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);
    Map_CreateFastWarrior("", sCapId + "1", 8);
}
