
//добавить или удалить товары в магазине города.
void AddRemoveGoodsInStore(string _town, int _tradeGoods, int _quantityGoods, bool toAdd)
{
	int iTest;
	ref rStore;
	string sGoodName;

	iTest = FindColony(_town);
	if (iTest != -1)
	{
		rStore = GetColonyByIndex(iTest);
		iTest = sti(rStore.StoreNum);
		makeref(rStore, stores[iTest]);

		sGoodName = Goods[_tradeGoods].Name;
		iTest = sti(rStore.Goods.(sGoodName).Quantity);
		if (toAdd)
		{
			iTest += _quantityGoods;
		}
		else
		{
			iTest -= _quantityGoods;
		}
		if (iTest < 0) iTest = 0;
		rStore.Goods.(sGoodName).Quantity = iTest;
		//TODO: сделать пересчет цены на товар, т.к. его либо мало, либо много.
	}
}

//==================================================
// Locations
//==================================================
//Mett: найти город назначения для персонажа, дописал под новый расчет дистанции между колониями
string PGG_FindTargetTown(ref chr)
{
    string sCurTown = chr.PGGAi.location.town;

    int i = 1;
    string sTargetIsland = GetRandomIslandId();    //получаем рандомный остров
    while (GetCityNameByIsland(sTargetIsland) == "None" || GetCityNameByIsland(sTargetIsland) == "Caiman" || GetCityNameByIsland(sTargetIsland) == "LostShipsCity") //ищем есть ли на острове колония
    {
        i++;
        sTargetIsland = GetRandomIslandId();    //ищем повторно
    }
    string sTargetTown = FindTownOnIsland(sTargetIsland); //присваеваем колонию, в которую направляемся

    chr.PGGAi.Task.Target = sTargetTown;
    chr.PGGAi.Task.Target.days = CalculateColonyDistance(sCurTown, sTargetTown) + rand(2);    //новый расчет дней на дорогу

    Log_TestInfo("PGG_FindTargetTown: для " + chr.id + GetConvertStrWithReplace("Variable_PsHero_11", "Logs.txt", "#space#", " ") + sTargetTown + GetConvertStrWithReplace("Variable_PsHero_12", "Logs.txt", "#space#", " ") + i);

    return sTargetTown;
}

// LEO: цель рейса на турнир. Арена стоит на НЕОБИТАЕМОЙ Доминике (FencingTown), колонии там нет, поэтому
//   PGG_FindTargetTown её не вернёт - он перебирает только Colonies. Цель ставим вручную, а срок пути
//   считаем от ближайшей колонии-соседа: точность тут не нужна, прибытие в системе ПГГ и так условное
//   (счётчик дней), физически корабль идёт энкаунтером по глобалке.
void PGG_SetTournamentTarget(ref chr)
{
	chr.PGGAi.Task.Target = PGG_ARENA_ISLAND;
	int iDays = 3 + rand(3);
	if (CheckAttribute(chr, "PGGAi.location.town"))
	{
		int iCalc = CalculateColonyDistance(chr.PGGAi.location.town, "Marigo"); // Марго - ближайшая к Доминике колония
		if (iCalc > 0) iDays = iCalc + rand(2);
	}
	chr.PGGAi.Task.Target.days = iDays;
}

//получить ID случайного города по нации. Будет для раскидывания ПГГ в начале игры по миру.
string PGG_FindRandomTownByNation(int _nation)
{
	int i, n;
	string sTowns[MAX_COLONIES];
	n = 0;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		if (sti(Colonies[i].nation) == _nation && Colonies[i].from_sea != "" && Colonies[i].id != "LostShipsCity" && Colonies[i].id != "Caiman") //в сухопутных не будут. //18.06.07 fix ГПК
		{
			sTowns[n] = Colonies[i].id;
			n++;
		}
	}

	if (n == 0)
	{
		trace("ОШИБКА: <PsHero.c>: Не могу найти ни одного города для нации: " + _nation + " Использовать по умолчанию.");
		return "Bridgetown";
	}
	return sTowns[rand(n-1)];
}

//получить ID случайного города по нации. Будет для раскидывания ПГГ в начале игры по миру.
string PGG_FindRandomTown(ref chr)
{

	string town = "";
	if(chr.sex == "skeleton")
	{
		switch (rand(3))
		{
			case 0:
				town = "PortRoyal";
			break;
			case 1:
				town = "BasTer";
			break;
			case 2:
				town = "Havana";
			break;
			case 3:
				town = "Villemstad";
			break;
		}
		return town;
	}
	else
	{
		return PGG_FindRandomTownByNation(sti(chr.nation));
	}
}

// LEO: ПГГ реально в том городе, где сейчас игрок. ⭐findsubstr на ПУСТОЙ подстроке возвращает 0,
//      то есть «найдено» - поэтому ПГГ с пустым городом раньше считался присутствующим В ЛЮБОЙ
//      локации: садился в таверну каждого города и выдёргивался к игроку через полкарты.
bool PGG_IsInPlayerTown(ref chr)
{
	if (!CheckAttribute(chr, "PGGAi.IsPGG")) return false;
	if (!sti(chr.PGGAi.IsPGG)) return false;   // нанят игроком - в мире его быть не должно
	if (!CheckAttribute(chr, "PGGAi.location.town")) return false;
	string sTown = chr.PGGAi.location.town;
	if (sTown == "") return false;
	if (sTown == "none") return false;
	if (findsubstr(pchar.location, sTown, 0) == -1) return false;
	return true;
}

// LEO: город, где ПГГ остаётся после расставания. GetCurrentTown пуст везде, кроме колонии, а
//      расстаются обычно в каюте или в море - пустой город ломает ПГГ (см. PGG_IsInPlayerTown).
string PGG_TownForRelease(ref chr)
{
	string sTown = GetCurrentTown();
	if (sTown != "") return sTown;
	return PGG_FindRandomTownByNation(sti(chr.nation));
}
// LEO: хвосты службы у ГГ. Штатное увольнение офицера (LandEnc_OfficerFired) их снимает, оба
//      ПГГ-пути не снимали: Payment висел, а RestorePost ушедшему уже не пригодится - копился в сейве.
void PGG_ClearOfficerLeftovers(ref chr)
{
	DeleteAttribute(chr, "Payment");
	DeleteAttribute(chr, "RestorePost");
}

// LEO: ПГГ уходит из офицеров ДРАКОЙ. Полный PGG_FireOfficer звать нельзя - он выдернет бойца из
//      локации посреди схватки. Возвращаем только статус жителя мира: иначе ПГГ навсегда остаётся с
//      офицерским диалогом и служебным городом «Офицер», то есть выпадает из игры.
void PGG_ReleaseOfficerFlags(ref chr)
{
	if (!CheckAttribute(chr, "PGGAi")) return;
	chr.PGGAi.IsPGG = true;
	chr.RebirthPhantom = true;
	chr.PGGAi.location.town = PGG_TownForRelease(chr);
	chr.Dialog.FileName = "PGG_Dialog.c";
	chr.Dialog.CurrentNode = "Second time";
	chr.Dialog.TempNode = "Second time";
	PGG_ChangeRelation2MainCharacter(chr, -40); // LEO: расстались дракой - столько же, сколько за уход после боя (LeaveBattle.c)
	PGG_ClearOfficerLeftovers(chr);
}



//грузить или нет ПГГ в таверну.
void PGG_TavernCheckIsPGGHere()
{
	ref chr;
	int i;
	for (i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);

		if (PGG_IsInPlayerTown(chr) && !CheckAttribute(chr, "willDie") && !LAi_IsDead(chr) && chr.PGGAi.location != "Dead" && !CheckAttribute(chr, "PGGAi.Task.SetSail"))
		{
			if (!CheckAttribute(pchar, "GenQuest.PGG_Quest") && PGG_CheckForQuestOffer(chr)) continue;
			//в таверне или нет.
			if (rand(1) == 1 && chr.sex != "skeleton")
			{
				PGG_PlaceCharacter2Tavern(chr, true);
				PGG_DebugLog("PGG " + chr.id + " in tavern");
			}
			else
			{
				PGG_PlaceCharacter2Tavern(chr, false);
			}
		}
	}
}

void PGG_GraveyardCheckIsPGGHere(ref location)
{
	if(HasSubStr(location.id,"Graveyard"))
	{
		ref chr;
		int i;
		bool hasPGG = false;
		for (i = 1; i <= PsHeroQty; i++)
		{
			chr = CharacterFromID("PsHero_" + i);
			if (PGG_IsInPlayerTown(chr) && !CheckAttribute(chr, "willDie") && !LAi_IsDead(chr) && chr.PGGAi.location != "Dead")
			{
				//в таверне или нет.
				if (rand(1) == 1 && !CheckAttribute(chr, "PGGAi.Task.SetSail") && chr.sex == "skeleton")
				{
					PGG_PlaceCharacter2Graveyard(chr, true);
					PGG_DebugLog("PGG " + chr.id + " in graveyard");
					LAi_LocationFightDisable(location, true);
					hasPGG = true;
				}
				else
				{
					PGG_PlaceCharacter2Graveyard(chr, false);
				}
			}
		}
		if (!hasPGG)
		{
			LAi_LocationFightDisable(location, false);
		}
	}
}

void PGG_PlaceCharacter2Graveyard(ref chr, bool _bSet)
{
	int n = 0;
	string sTemp;

	if (_bSet)
	{
		sTemp = PlaceCharacter(chr, "goto", "random_free");
		LAi_SetWarriorType(chr);
	}
	else
	{
		ChangeCharacterAddressGroup(chr, "None", "", "");
		LAi_SetWarriorType(chr);
	}
}
//посадить или убрать ПГГ из таверны.
void PGG_PlaceCharacter2Tavern(ref chr, bool _bSet)
{
	int n = 0;
	string sTemp;

	if (_bSet)
	{
		sTemp = PlaceCharacter(chr, "sit", "random_free");
		n = 0;
		while (findsubstr(sTemp, "sit_front" , 0) != -1 && n < 50)
		{    // чтоб не занимали место напротив
			sTemp = PlaceCharacter(chr, "sit", "random_free");
			n++;
		}
		if (findsubstr(sTemp, "_bar" , 0) != -1) LAi_SetBarmanType(chr);
		else
		{
			if (findsubstr(sTemp, "sit" , 0) == -1)
			{
				PlaceCharacter(chr, "tables", "random_free");
				switch (rand(2))
				{
					case 0:
						LAi_SetStayType(chr);
					break;
					case 1:
						LAi_SetWarriorType(chr);
						if(chr.model.animation == "man")
						{
							LAi_SetGroundSitType(chr);
						}
					break;
					case 2:
						LAi_SetWarriorType(chr);
					break;
				}
			}
			else
			{
				LAi_SetSitType(chr);
			}
		}
	}
	else
	{
		if (!CheckAttribute(chr, "PGGAi.DontUpdate"))
		{
		ChangeCharacterAddressGroup(chr, "None", "", "");
		LAi_SetWarriorType(chr);
		}
	}
}
//==================================================
// Verifications
//==================================================
//доступны ли услуги NPC, если нет, то возвращает ID ПГГ занявшего NPC
int CheckFreeServiceForNPC(ref NPChar, string ServiceType)
{
	ref chr;
	for (int i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);
		if (chr.PGGAi.location == "land" && chr.PGGAi.location.town == NPChar.City)
		{
			if (CheckAttribute(chr, "PGGAi.LockService") && chr.PGGAi.LockService == ServiceType) return sti(chr.index);
		}
		if (CheckAttribute(chr, "PGGAi.location.town.back") && chr.PGGAi.location.town.back == NPChar.City)
		{
			if (CheckAttribute(chr, "PGGAi.LockService") && chr.PGGAi.LockService == ServiceType) return sti(chr.index);
		}
	}
	return -1;
}

//можно ли взять задание у NPC, если нет, то возвращает ID ПГГ взявшего задание
int CheckAvailableTaskForNPC(ref NPChar, string PGGTask)
{
	ref chr;
	for (int i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);
		if (chr.PGGAi.location == "land" && chr.PGGAi.location.town == NPChar.City)
		{
			if (CheckAttribute (chr, "PGGAi.Task") && chr.PGGAi.Task == PGGTask) return sti(chr.index);
		}
	}
	return -1;
}

//получить доступный класс корабля для персонажа.
int PGG_GetAvailableShipClass(ref chr)
{
	int retVal = 7;
	int iSkill = sti(chr.skill.Sailing);

	if (iSkill > 0) retVal = 6;
	if (iSkill > 24) retVal = 5;
	if (iSkill > 39) retVal = 4;
	if (iSkill > 64) retVal = 3;
	if (iSkill > 79) retVal = 2;
	if (iSkill > 94) retVal = 1;

	return retVal;
}

// LEO: гейт по силе — ПГГ гонит пиратов, только если достаточно силён (иначе одиночка на мелком корабле
//   полезет на кучу пиратов = нелогично). Достаточно: эскадра из 2+ кораблей ИЛИ крупный корабль (класс <= 3).
bool PGG_CanHuntPirates(ref chr)
{
	int nsh = 0;
	int j;
	for (j = 0; j < COMPANION_MAX; j++)
	{
		if (GetCompanionIndex(chr, j) > 0) nsh++;
	}
	if (nsh >= 2) return true;
	if (GetCharacterShipClass(chr) <= 3) return true;
	return false;
}

// ============================================================================================
// LEO: ОППОРТУНИСТИЧНЫЙ ДОГОН ПГГ->ПИРАТ. Погоню ВЕДЁТ ДВИЖОК сам (WdmMerchantShip::UpdateChase — читает позицию
//   пирата из ДЕРЕВА энкаунтеров, БЕЗ перебора корабликов и висячих указателей). Скрипт только:
//   - ДИВЕРТ (раз в 5с из wdmTimeUpdate): свободный ПЛЫВУЩИЙ нац.ПГГ рядом с пиратом -> задаёт цель ОДИН раз
//     (пишет имя пирата на энкаунтер ПГГ: wdmChaseTgt). Состояние на ПЕРСОНАЖА (PGG_ChasingPirate + PGG_ChaseTgt).
//   - СВЕРКА (тот же диверт): движок бросил погоню (wdmChaseTgt опустел) -> снять флаги; стычка кончилась
//     (нет её энкаунтера) -> снять PGG_ChasingPirate (ПГГ возвращается к делам).
//   - СТЫЧКА: по событию WorldMap_PGGCaughtPirate (движок догнал) -> спавн реального боя ПГГ vs пират.
//   ГЛАВНОЕ: НЕТ пер-тик записи worldMap-атрибутов -> не дёргаем хрупкий движковый AttributeChanged (он крашил
//   проходом по родителям на удалении энкаунтера). Цель задаётся один раз за погоню.
// ============================================================================================
#define PGG_CHASE_VISION 120.0  // «заметил пирата по пути» (мир.координаты глобалки). Контакт/потеря — в движке.

// дистанция между энкаунтерами по текущим позициям (x/z). -1 если позиций нет (кто-то не заспавнен).
float PGG_EncDist(string encA, string encB)
{
	float ax, az, bx, bz, dx, dz;
	if (!CheckAttribute(worldMap, "encounters." + encA + ".x")) return -1.0;
	if (!CheckAttribute(worldMap, "encounters." + encB + ".x")) return -1.0;
	ax = stf(worldMap.encounters.(encA).x);
	az = stf(worldMap.encounters.(encA).z);
	bx = stf(worldMap.encounters.(encB).x);
	bz = stf(worldMap.encounters.(encB).z);
	dx = ax - bx;
	dz = az - bz;
	return sqrt(dx*dx + dz*dz);
}

// снять догон/стычку с персонажа полностью (цель + оба флага)
void PGG_ChaseEnd(ref chr)
{
	DeleteAttribute(chr, "PGG_ChaseTgt");
	DeleteAttribute(chr, "PGG_ChasingPirate");
}

// есть ли ещё живой энкаунтер стычки этого ПГГ (его борт помечен encdata.CharacterID = pggId)?
bool PGG_HasSkirmishEncounter(string pggId)
{
	int num, i;
	aref encs, enc;
	if (!IsEntity(worldMap)) return false;
	makearef(encs, worldMap.encounters);
	num = GetAttributesNum(encs);
	for (i = 0; i < num; i++)
	{
		enc = GetAttributeN(encs, i);
		if (!CheckAttribute(enc, "PGG_WarEnc")) continue;          // ТОЛЬКО боевой борт стычки (у рейсового энка тоже encdata.CharacterID==pggId -> иначе пересозданный рейс самозаблокирует снятие флага)
		if (!CheckAttribute(enc, "encdata.CharacterID")) continue;
		if (enc.encdata.CharacterID == pggId) return true;
	}
	return false;
}

// ДИВЕРТ + СВЕРКА (раз в 5с из wdmTimeUpdate).
void PGG_UpdateChaseFlags()
{
	int num, i, j, ic, pggAtSea;
	aref encs, enc, penc;
	ref rc;
	string pggEnc, pirEnc, bestPir, claimed, sGrp, warEncs;
	float bestD, d;
	if (!IsEntity(worldMap)) return;
	makearef(encs, worldMap.encounters);
	num = GetAttributesNum(encs);
	// LEO: ДЕТЕКТ погони (тяжёлый: проходы по энкаунтерам+ПГГ) — только если есть ПГГ в море, иначе гнать
	//   некому. Сверку окончания стычек (ниже) делаем ВСЕГДА (флаг PGG_ChasingPirate должен сниматься).
	pggAtSea = 0;
	if (CheckAttribute(pchar, "PGGsAtSea")) pggAtSea = sti(pchar.PGGsAtSea);
	if (pggAtSea > 0)
	{
	// LEO: собрать пиратов, за которыми УЖЕ гонятся, чтобы НЕ давать двоим гнать одного (иначе второй, кто не
	//   догнал первым, кружит у стычки). Делимитеры "|" вокруг имён -> точное совпадение через findsubstr.
	claimed = "|";
	for (i = 1; i <= PsHeroQty; i++)
	{
		rc = CharacterFromID("PsHero_" + i);
		if (CheckAttribute(rc, "PGG_ChaseTgt")) claimed = claimed + rc.PGG_ChaseTgt + "|";
	}
	// LEO: собрать энкаунтеры, УЖЕ участвующие в войне/стычке (нация vs пираты ИЛИ наша стычка) — за такими
	//   пиратами гоняться НЕЛЬЗЯ, иначе спавним бой ПОВЕРХ существующего («переинициируем» чужую стычку).
	//   Движок метит борт «Warring» атрибутом "attacked"=имя противника (CreateWarringShips) -> ловим ОБА
	//   борта: сам Warring (его имя) + его цель (значение attacked).
	warEncs = "|";
	for (i = 0; i < num; i++)
	{
		enc = GetAttributeN(encs, i);
		if (CheckAttribute(enc, "attacked")) warEncs = warEncs + GetAttributeName(enc) + "|" + enc.attacked + "|";
	}
	// --- проход по рейсовым энкаунтерам ПГГ: детект новых догонов + сверка активных погонь ---
	for (i = 0; i < num; i++)
	{
		enc = GetAttributeN(encs, i);
		if (CheckAttribute(enc, "PGG_WarEnc")) continue;      // борт уже идущей стычки — не рейс, пропустить
		if (!CheckAttribute(enc, "quest.chrID")) continue;
		ic = GetCharacterIndex(enc.quest.chrID);
		if (ic < 0) continue;
		rc = GetCharacter(ic);
		if (sti(rc.nation) == PIRATE) continue;               // ПГГ-пират сам не гонит
		pggEnc = GetAttributeName(enc);
		// уже гонит -> сверить с движком: пустой wdmChaseTgt = движок бросил погоню (пират ушёл/исчез)
		if (CheckAttribute(rc, "PGG_ChaseTgt"))
		{
			if (!CheckAttribute(enc, "wdmChaseTgt") || enc.wdmChaseTgt == "")
			{
				PGG_ChaseEnd(rc); // возвращается к делам (в след. часовой апдейт перетаскуется штатно)
			}
			continue;
		}
		if (CheckAttribute(rc, "PGG_ChasingPirate")) continue; // в стычке (без ChaseTgt) — не трогаем тут
		// свободный плывущий нац.ПГГ -> ищем ближайшего пирата в поле зрения (опортунизм: только рядом по пути)
		if (!CheckAttribute(rc, "PGGAi.Task.SetSail")) continue; // должен ПЛЫТЬ (по своим делам)
		if (CheckAttribute(rc, "PGG_Hunter")) continue;          // охотник за ГГ — не отвлекаем
		// LEO: спешит на турнир — не отвлекаем. Рейс после стычки продолжится к той же цели, но потерянные
		//   на бой дни съедают срок: опоздавшего снимает PGG_ClearTournament, и он приплывёт впустую.
		if (CheckAttribute(rc, "PGGAi.Task"))
		{
			if (rc.PGGAi.Task == PGG_TASK_ARENA) continue;
		}
		if (!PGG_CanHuntPirates(rc)) continue;                   // гейт по мощи эскадры
		if (!CheckAttribute(worldMap, "encounters." + pggEnc + ".x")) continue; // ПГГ не заспавнен (далеко)
		bestPir = "";
		bestD = PGG_CHASE_VISION;
		for (j = 0; j < num; j++)
		{
			penc = GetAttributeN(encs, j);
			if (CheckAttribute(penc, "PGG_WarEnc")) continue;            // пират уже в стычке — не гоняемся
			if (!CheckAttribute(penc, "modelName")) continue;
			if (findsubstr(penc.modelName, "pirate", 0) == -1) continue; // пират по модели
			if (CheckAttribute(penc, "needDelete")) continue;            // уже уходит/в стычке
			pirEnc = GetAttributeName(penc);
			if (findsubstr(warEncs, "|" + pirEnc + "|", 0) != -1) continue; // пират уже в войне/стычке -> не трогать (иначе переинициируем чужой бой)
			if (findsubstr(claimed, "|" + pirEnc + "|", 0) != -1) continue; // за этим пиратом уже гонятся -> мимо
			d = PGG_EncDist(pggEnc, pirEnc);
			if (d < 0.0) continue;
			if (d < bestD) { bestD = d; bestPir = pirEnc; }
		}
		if (bestPir != "")
		{
			rc.PGG_ChasingPirate = "1"; // состояние на ПЕРСОНАЖА (переживает пересоздание кораблика)
			rc.PGG_ChaseTgt = bestPir;
			worldMap.encounters.(pggEnc).wdmChaseTgt = bestPir; // задаём движку цель ОДИН раз (НЕ пер-тик!)
			claimed = claimed + bestPir + "|"; // застолбить сразу: в этом же проходе второй ПГГ его не возьмёт
		}
	}
	} // LEO: конец блока ДЕТЕКТА погони (гейт pggAtSea>0)
	// --- сверка стычек: ПГГ «в стычке» (PGG_ChasingPirate без ChaseTgt), а её энкаунтера уже нет -> бой окончен ---
	for (i = 1; i <= PsHeroQty; i++)
	{
		rc = CharacterFromID("PsHero_" + i);
		if (!CheckAttribute(rc, "PGG_ChasingPirate")) continue;
		// LEO ФИКС ЗАЛИПАНИЯ ПОГОНИ: основная сверка живёт ВЫШЕ, внутри гейта pggAtSea>0. Если гейт закрылся,
		// пока погоня была активна, снять PGG_ChaseTgt стало некому -> ПГГ НАВСЕГДА выпадал из апдейтов
		// (PGG_update.c: ранний return по PGG_ChasingPirate), а его свита/группа/слоты RealShips не
		// освобождались до конца сессии (монотонная утечка = нарастающие фризы глобалки).
		// Страховка БЕЗ гейта: цель погони пропала с карты -> погоня окончена.
		if (CheckAttribute(rc, "PGG_ChaseTgt"))
		{
			if (!CheckAttribute(worldMap, "encounters." + rc.PGG_ChaseTgt)) PGG_ChaseEnd(rc);
			continue;   // активная погоня — стычкой не считаем
		}
		if (PGG_HasSkirmishEncounter(rc.id)) continue;      // стычка ещё идёт
		// стычка кончилась -> ПГГ продолжает путь С ТОЧКИ СТЫЧКИ к своему пункту назначения (НЕ телепортом из
		//   порта). Пересоздаём рейсовый кораблик там же (координаты снял SpawnSkirmish: PGG_Resume*). Таск/цель
		//   персонажа НЕ тронуты -> дни докрутит часовой апдейт, ПГГ прибудет и возьмёт следующий рейс штатно.
		if (CheckAttribute(rc, "PGG_ResumeSet"))
		{
			sGrp = "Sea_" + rc.id;
			group_DeleteGroup(sGrp);
			Group_FindOrCreateGroup(sGrp);
			Group_SetTaskAttackInMap(sGrp, PLAYER_GROUP);
			Group_LockTask(sGrp);
			Group_AddCharacter(sGrp, rc.id);
			Group_SetGroupCommander(sGrp, rc.id);
			rc.PGGAi.Encountername = rc.id;
			Map_ReleaseQuestEncounter(rc.id);
			Map_CreateTraderXZ(stf(rc.PGG_ResumeFromX), stf(rc.PGG_ResumeFromZ), stf(rc.PGG_ResumeDestX), stf(rc.PGG_ResumeDestZ), rc.id, sti(rc.PGG_ResumeDays) + 6); // +6 запас: стартует из точки стычки, остатка дней могло не хватить доплыть (Fix 1 не даст день-снос -> деспавн по достижению цели)
			DeleteAttribute(rc, "PGG_ResumeSet");
			DeleteAttribute(rc, "PGG_ResumeFromX");
			DeleteAttribute(rc, "PGG_ResumeFromZ");
			DeleteAttribute(rc, "PGG_ResumeDestX");
			DeleteAttribute(rc, "PGG_ResumeDestZ");
			DeleteAttribute(rc, "PGG_ResumeDays");
		}
		DeleteAttribute(rc, "PGG_ChasingPirate");           // стычка кончилась -> ПГГ снова в деле
	}
}

// СПАВН СТЫЧКИ (реальный ПГГ vs пират, ровно в точке fX,fZ). Зовётся по событию WorldMap_PGGCaughtPirate.
void PGG_SpawnSkirmish(string pggEnc, string pirEnc, string pggId, float fX, float fZ)
{
	int iPgg;
	ref rPgg;
	string pggModel, pirModel, pggWarEnc, pirWarEnc;
	aref rEncPir, rEncPgg;
	float daysPerSec, timeOutInSec;
	if (!CheckAttribute(worldMap, "encounters." + pggEnc)) return;
	if (!CheckAttribute(worldMap, "encounters." + pirEnc)) return;
	iPgg = GetCharacterIndex(pggId);
	if (iPgg < 0) return;
	rPgg = GetCharacter(iPgg);
	rPgg.PGG_ChasingPirate = "1"; // «в стычке» — перма до конца боя (не диверт повторно)
	// LEO: новая стычка — участие игрока ещё не засчитано. Сбросить флаг помощи и наш палубный узел
	//   благодарности (ставит ShipDead, когда игрок/компаньон топит пирата этой стычки). НЕ трогаем чужой
	//   DeckDialogNode (напр. квестовый) — чистим только наш.
	DeleteAttribute(rPgg, "PGG_PlayerHelped");
	if (CheckAttribute(rPgg, "DeckDialogNode") && rPgg.DeckDialogNode == "PGG_ThanksForHelp")
		DeleteAttribute(rPgg, "DeckDialogNode");
	// LEO: данные для БЕСШОВНОГО возврата — после стычки ПГГ продолжит С ТОЧКИ СТЫЧКИ к своему пункту назначения
	//   (не «телепортом» из порта). Пункт назначения = пред-погонный gotoPos: движок сохранил его на энкаунтере
	//   как gotoBkpX/Z (снимок на 1-м тике погони, StopChase). Читаем ДО needDelete энкаунтера (ниже).
	DeleteAttribute(rPgg, "PGG_ResumeSet");
	if (CheckAttribute(worldMap, "encounters." + pggEnc + ".gotoBkpX"))
	{
		rPgg.PGG_ResumeFromX = fX;
		rPgg.PGG_ResumeFromZ = fZ;
		rPgg.PGG_ResumeDestX = worldMap.encounters.(pggEnc).gotoBkpX;
		rPgg.PGG_ResumeDestZ = worldMap.encounters.(pggEnc).gotoBkpZ;
		rPgg.PGG_ResumeDays = 4;
		if (CheckAttribute(rPgg, "PGGAi.Task.Target.days") && sti(rPgg.PGGAi.Task.Target.days) > 4)
			rPgg.PGG_ResumeDays = sti(rPgg.PGGAi.Task.Target.days);
		rPgg.PGG_ResumeSet = "1";
	}

	pggModel = worldMap.encounters.(pggEnc).modelName;
	pirModel = worldMap.encounters.(pirEnc).modelName;
	makearef(rEncPir, worldMap.encounters.(pirEnc).encdata);
	makearef(rEncPgg, worldMap.encounters.(pggEnc).encdata);
	daysPerSec = 24.0/stf(worldMap.date.hourPerSec);
	timeOutInSec = daysPerSec*(5+rand(5));
	// СТЫЧКА ровно в точке встречи (fX,fZ). Порядок: send(model1=ПГГ ->EncID2, model2=пират ->EncID1) +
	//   copy(пират->EncID1, ПГГ->EncID2). Реентрантности НЕТ: тик — скрипт-событие, не цикл движка по ships.
	SendMessage(&worldMap, "lssfff", MSG_WORLDMAP_CREATEENC_WAR, pggModel, pirModel, timeOutInSec, fX, fZ);
	WdmCopyEncounterData(rEncPir, worldMap.EncounterID1);
	WdmCopyEncounterData(rEncPgg, worldMap.EncounterID2);
	// ПГГ-борт стычки грузится РЕАЛЬНЫМ персонажем: encdata борта в ALONE + CharacterID (в обход гварда).
	//   Вход через map.c->wdmReloadToSea->SeaLogin (ветка ENCOUNTER_TYPE_ALONE) поднимет Sea_<pgg> с настоящим
	//   кораблём/командой/повреждениями. Пиратский борт (EncID1) — генерик-эскадра.
	pggWarEnc = worldMap.EncounterID2;
	worldMap.encounters.(pggWarEnc).encdata.RealEncounterType = ENCOUNTER_TYPE_ALONE;
	worldMap.encounters.(pggWarEnc).encdata.CharacterID = pggId;
	// LEO: миникарта — цвет блипа ПГГ по полу (движок читает encdata.blipColor как override поверх нации): жен розовый, муж синий
	if (CheckAttribute(rPgg, "sex") && rPgg.sex == "woman") worldMap.encounters.(pggWarEnc).encdata.blipColor = argb(255, 255, 130, 200);
	else worldMap.encounters.(pggWarEnc).encdata.blipColor = argb(255, 90, 130, 255);
	// пометить ОБА борта стычки: диверт их не считает рейсом и не гоняется за пиратом-в-бою; а сверка
	//   окончания стычки (PGG_HasSkirmishEncounter) видит бой по encdata.CharacterID = pggId.
	worldMap.encounters.(pggWarEnc).PGG_WarEnc = "1";
	pirWarEnc = worldMap.EncounterID1;
	worldMap.encounters.(pirWarEnc).PGG_WarEnc = "1";
	// LEO: борт реального ПГГ грузится как ALONE и, будучи дружественным ГГ, САМ задачи атаки не получает ->
	//   стоял бы без дела. Поэтому ПИРАТСКОМУ борту задаём атаку на группу ПГГ "Sea_"+pggId: загрузчик боя
	//   (sea.c switch -> Group_SetTaskAttack с bTaskBoth=true) поднимает в атаку ОБЕ стороны. Lock — чтобы
	//   пираты не бросали ПГГ ради ГГ. Отношение «враг» обеспечивает нация (нац.ПГГ vs PIRATE = ENEMY).
	worldMap.encounters.(pirWarEnc).encdata.Task = 1; // AITASK_ATTACK (=1; дефайн из sea_ai/Script_defines.h тут не виден)
	worldMap.encounters.(pirWarEnc).encdata.Task.Target = "Sea_" + pggId;
	worldMap.encounters.(pirWarEnc).encdata.Lock = 1;
	// убрать оба исходных энкаунтера (ПГГ+пират) — их заменил бой. Сами кораблики движок уже убрал МГНОВЕННО
	//   на контакте (killMe ПГГ + флаг wdmKillNow пирату); needDelete здесь лишь чистит осиротевшие узлы.
	worldMap.encounters.(pggEnc).needDelete = "wdmEncounterDelete";
	worldMap.encounters.(pirEnc).needDelete = "wdmEncounterDelete";
	trace("LEO PGGCAUGHT: " + pggId + " REAL-war " + pggModel + " vs " + pirModel + " at " + makeint(fX) + "," + makeint(fZ));
}

// СОБЫТИЕ ОТ ДВИЖКА: ПГГ догнал пирата (точка встречи fX,fZ) -> спавним реальный бой ПГГ vs пират ровно там.
//   Движок сам детектит контакт (WdmMerchantShip::UpdateChase) и копит событие в очередь -> шлёт его ПОСЛЕ
//   цикла кораблей (безопасно, без реентрантности). Скрипт-тика/пер-тик записей больше нет.
#event_handler("WorldMap_PGGCaughtPirate", "PGG_OnCaughtPirate");
void PGG_OnCaughtPirate()
{
	string pggEnc = GetEventData();
	string pirEnc = GetEventData();
	float fX = GetEventData();
	float fZ = GetEventData();
	string pggId;
	int ic;
	ref rc;
	if (!IsEntity(worldMap)) return;
	if (!CheckAttribute(worldMap, "encounters." + pggEnc + ".quest.chrID")) return;
	pggId = worldMap.encounters.(pggEnc).quest.chrID;
	PGG_SpawnSkirmish(pggEnc, pirEnc, pggId, fX, fZ); // ставит PGG_ChasingPirate="1" (ПГГ теперь «в стычке»)
	ic = GetCharacterIndex(pggId);
	if (ic >= 0)
	{
		rc = GetCharacter(ic);
		DeleteAttribute(rc, "PGG_ChaseTgt"); // погоня окончена; «в стычке» остаётся до конца боя
	}
}
