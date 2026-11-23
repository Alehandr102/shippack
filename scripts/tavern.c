void TavernWaitDate(string date)
{
	int iTime, iAddTime, iTmp;
	iTime = sti(environment.time);
	if (date == "wait_day")
	{
		if (iTime >= 21) iAddTime = 24 - iTime + 7;
		if (iTime < 7) iAddTime = 7 - iTime;
		// boal 03.03.05 -->
		if (iTime >= 7 && iTime < 21) iAddTime = 24  + 7 - iTime;
		// boal <--
		StoreDayUpdate();
	}
	else
	{
		if (iTime < 24) iAddtime = 24 - iTime;
	}
	LAi_Fade("", "");
	WaitDate("",0,0,0,iAddtime,5);
	//navy --> сон снимает алкоголь... 2 часа = кружка
	if (CheckAttribute(pchar, "questTemp.Rum") && CheckAttribute(pchar, "chr_ai.drunk"))
	{
		iTmp = sti(pchar.chr_ai.drunk);
		iTmp -= (iAddtime * 900);
		if (iTmp < 0) LAi_SetAlcoholNormal(pchar);
		else pchar.chr_ai.drunk = iTmp;
	}
	//navy <--
	RecalculateJumpTable();
	// boal -->
	RefreshWeather();
	RefreshLandTime();
	// boal <--
}

//Boyer add
void WasteTime(int baseHours)
{
	int nMinutes, nHours;
	nMinutes = rand(15);
	nHours = func_max(baseHours, 1);
	if(rand(1)) nMinutes = -1 * nMinutes;

	LAi_Fade("", "");
	WaitDate("",0,0,0,nHours,nMinutes);
	RefreshWeather();
	RefreshLandTime();
}
//End Boyer add

void TavernWaitDate_LSC(string date)
{
	int iTime, iAddTime, iTmp;
	iTime = sti(environment.time);
	if (date == "wait_day")
	{
		if (iTime >= 21) iAddTime = 24 - iTime + 7;
		if (iTime < 7) iAddTime = 7 - iTime;
		// boal 03.03.05 -->
		if (iTime >= 7 && iTime < 21) iAddTime = 24  + 7 - iTime;
		// boal <--
		StoreDayUpdate();
	}
	else
	{
		if (iTime < 24) iAddtime = 23 - iTime;
	}
	WaitDate("",0,0,0,iAddtime,5);
	//navy --> сон снимает алкоголь... 2 часа = кружка
	if (CheckAttribute(pchar, "questTemp.Rum") && CheckAttribute(pchar, "chr_ai.drunk"))
	{
		iTmp = sti(pchar.chr_ai.drunk);
		iTmp -= (iAddtime * 900);
		if (iTmp < 0) LAi_SetAlcoholNormal(pchar);
		else pchar.chr_ai.drunk = iTmp;
	}
	//navy <--
	DoReloadCharacterToLocation("FleuronTavern",  "quest", "bed");
	RecalculateJumpTable();
	// boal -->
	RefreshWeather();
	RefreshLandTime();
	// boal <--
}

// LEO: РАЗБОРКИ ОФИЦЕРОВ В ТАВЕРНЕ ============================================================
// Наёмный офицер из отряда изредка сам цепляется к завсегдатаю и лезет в драку. Дерутся только эти
// двое: обоих уводим в личные группы, враждебные между собой и мирные ко всем остальным - город и
// охрана не сбегаются, ГГ смотрит. Смертей нет: у каждого стоит порог HP, ниже которого движок
// делает бойца бессмертным и зовёт наш обработчик - тот разводит драчунов и возвращает им мирные
// типы. После драки офицеру подставляется нода с объяснением, за что он полез.
#define TAVERNBRAWL_CHANCE			5	// шанс драки на заходе в таверну, %
#define TAVERNBRAWL_SURRENDER_PCT	25	// на скольких % от максимума HP боец выдыхается, %
#define TAVERNBRAWL_REASONS			10	// сколько заготовлено объяснений (ноды Brawl_1..N)
#define TAVERNBRAWL_FINE			500	// сколько хозяин требует за беспорядок в зале
#define TAVERNBRAWL_GROUP_OFF		"TAVERNBRAWL_OFF"
#define TAVERNBRAWL_GROUP_HAB		"TAVERNBRAWL_HAB"
#define TAVERNBRAWL_TEST_MODE		0	// LEO: ТЕСТ: 0=боевой, 1=драка на каждом заходе в таверну
#define TAVERNBRAWL_ENABLED			0	// LEO: ВЫКЛЮЧАТЕЛЬ фичи. 0 = драк нет (игрокам не зашло, механику ещё думаем), 1 = работает

void Tavern_OfficerBrawlCheck()
{
	int i, idx, iQty, iChance, iOff, iHab;

	Tavern_BrawlSafetyCheck(); // хвост от прошлого раза (сейв-лоад посреди драки)

	// LEO: фича отключена выключателем. Уборщик выше НАМЕРЕННО остаётся до выхода: на партиях,
	//   где драка началась ещё при включённой фиче, он разведёт драчунов при смене локации.
	if (TAVERNBRAWL_ENABLED == 0) return;

	if (LAi_IsCapturedLocation) return;
	if (chrDisableReloadToLocation) return;             // офицер уже занят другой сценой
	if (CheckAttribute(pchar, "questTemp.TavernBrawl")) return;
	if (TAVERNBRAWL_TEST_MODE == 0)
	{
		if (!CheckNPCQuestDate(pchar, "TavernBrawlDate")) return; // не чаще раза в день
	}

	iChance = TAVERNBRAWL_CHANCE;
	if (TAVERNBRAWL_TEST_MODE == 1) iChance = 100;
	if (drand(99) >= iChance) return;

	// зачинщик - случайный наёмный офицер отряда, квестовых не трогаем (у них своя ветка диалога)
	iQty = 0;
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		if (Tavern_BrawlIsGoodOfficer(GetOfficersIndex(pchar, i))) iQty++;
	}
	if (iQty < 1) return;

	iQty = drand(iQty - 1);
	iOff = -1;
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		idx = GetOfficersIndex(pchar, i);
		if (!Tavern_BrawlIsGoodOfficer(idx)) continue;
		if (iQty == 0) { iOff = idx; break; }
		iQty--;
	}
	if (iOff < 0) return;

	// жертва - случайный завсегдатай этой же таверны
	iQty = 0;
	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		if (Tavern_BrawlIsGoodHabitue(i)) iQty++;
	}
	if (iQty < 1) return;

	iQty = drand(iQty - 1);
	iHab = -1;
	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		if (!Tavern_BrawlIsGoodHabitue(i)) continue;
		if (iQty == 0) { iHab = i; break; }
		iQty--;
	}
	if (iHab < 0) return;

	SetNPCQuestDate(pchar, "TavernBrawlDate");
	pchar.questTemp.TavernBrawl.off = iOff;
	pchar.questTemp.TavernBrawl.hab = iHab;
	pchar.questTemp.TavernBrawl.loc = pchar.location;
	// причина - на обычном rand: drand внутри одних суток даёт одно и то же, и офицер всякий раз
	// оправдывался одной фразой. Сейв-скамить тут нечего, это чистая болтовня.
	pchar.questTemp.TavernBrawl.reason = rand(TAVERNBRAWL_REASONS - 1) + 1;

	// не на самом входе: даём игроку осмотреться, а офицеру - дойти до стола
	DoQuestFunctionDelay("Tavern_BrawlStart", 3.0);
}

// Отложенный старт: заводим обоих драчунов и стравливаем.
// Параметр обязателен: отложенные функции движок зовёт с именем квеста, без него ломается стек.
void Tavern_BrawlStart(string qName)
{
	if (!CheckAttribute(pchar, "questTemp.TavernBrawl")) return;
	if (pchar.questTemp.TavernBrawl.loc != pchar.location) { DeleteAttribute(pchar, "questTemp.TavernBrawl"); return; }

	ref rOff = GetCharacter(sti(pchar.questTemp.TavernBrawl.off));
	ref rHab = GetCharacter(sti(pchar.questTemp.TavernBrawl.hab));
	if (LAi_IsDead(rOff) || LAi_IsDead(rHab)) { DeleteAttribute(pchar, "questTemp.TavernBrawl"); return; }

	// Завсегдатая надо ПОДНЯТЬ ИЗ-ЗА СТОЛА: он приписан к локатору "sit", и одной смены типа мало -
	// модель остаётся на лавке, драться с ней нельзя (боец рядом бесконечно перезапускает удар).
	// Переставляем его на проходной локатор рядом с игроком, запомнив место за столом: после драки
	// сядет ровно туда же, а искать свободное наугад ненадёжно - их разбирают соседи.
	pchar.questTemp.TavernBrawl.sitgroup = rHab.location.group;
	pchar.questTemp.TavernBrawl.sitlocator = rHab.location.locator;
	if (PlaceCharacter(rHab, "goto", "random_must_be_near") == "")
	{
		DeleteAttribute(pchar, "questTemp.TavernBrawl"); // некуда встать - обойдётся без драки
		return;
	}
	pchar.questTemp.TavernBrawl.started = true;

	// В мирном зале драки запрещены на уровне локации, и движок не пускает бойцов в боевой режим
	// (LAi_tmpl_fight_IsFight -> LAi_LocationCanFight). На время потасовки снимаем запрет, после
	// неё возвращаем - иначе таверна так и осталась бы местом, где можно махать саблей.
	if (!LAi_LocationCanFight())
	{
		pchar.questTemp.TavernBrawl.nofight = true;
		LAi_LocationFightDisable(loadedLocation, false);
	}

	Tavern_BrawlSetFighter(rOff, TAVERNBRAWL_GROUP_OFF, "TavernBrawl_OfficerDown");
	Tavern_BrawlSetFighter(rHab, TAVERNBRAWL_GROUP_HAB, "TavernBrawl_HabitueDown");

	// Ссора строго между двумя группами, со всем остальным залом обе стороны в мире.
	LAi_group_SetRelation(TAVERNBRAWL_GROUP_OFF, TAVERNBRAWL_GROUP_HAB, LAI_GROUP_ENEMY);
	Tavern_BrawlPeaceWithAll();

	// Запрет боя снят со ВСЕГО зала, поэтому клинки могли бы достать и капитан, и остальной отряд -
	// вышла бы резня вместо потасовки двоих. Запираем боевой режим всем, кроме зачинщика.
	Tavern_BrawlLockBystanders(true);

	// LEO: фраза начинается с завсегдатая, имя офицера уходит в конец - иначе строка требовала бы
	//   родовой формы глагола, и у женщин-офицеров выходило «сцепился».
	Log_InfoColored("yellow", GetConvertStrWithReplace("Variable_tavern_Brawl", "Logs.txt", "#space#", " ") + GetFullName(rOff) + "!");
}

// Помирить обе стороны драки со всеми, кто в зале: отрядом капитана, горожанами любой нации и
// безгруппными (служанки, лоточники). Одной дружбы мало - движок держит для пары ещё и отношение
// "по тревоге", и у friend оно по умолчанию враждебное (MsgSetRelation: actState = rs_enemy).
// Без этого случайно задетая служанка поднимала весь зал, и таверна резала друг друга.
void Tavern_BrawlPeaceWithAll()
{
	int i;

	Tavern_BrawlPeaceWith(LAI_GROUP_PLAYER);
	Tavern_BrawlPeaceWith(LAI_GROUP_PLAYER_OWN);
	Tavern_BrawlPeaceWith(LAI_DEFAULT_GROUP);
	for (i = 0; i < MAX_NATIONS; i++)
	{
		Tavern_BrawlPeaceWith(GetNationNameByType(i) + "_citizens");
	}
}

void Tavern_BrawlPeaceWith(string sGroup)
{
	LAi_group_SetRelation(TAVERNBRAWL_GROUP_OFF, sGroup, LAI_GROUP_FRIEND);
	LAi_group_SetRelation(TAVERNBRAWL_GROUP_HAB, sGroup, LAI_GROUP_FRIEND);
	LAi_group_SetAlarmReaction(TAVERNBRAWL_GROUP_OFF, sGroup, LAI_GROUP_FRIEND, LAI_GROUP_FRIEND);
	LAi_group_SetAlarmReaction(TAVERNBRAWL_GROUP_HAB, sGroup, LAI_GROUP_FRIEND, LAI_GROUP_FRIEND);
}

// Запереть/отпустить боевой режим зрителей: капитан и его отряд, кроме зачинщика драки.
void Tavern_BrawlLockBystanders(bool bLock)
{
	int i, idx, iOff;

	iOff = -1;
	if (CheckAttribute(pchar, "questTemp.TavernBrawl.off")) iOff = sti(pchar.questTemp.TavernBrawl.off);

	LAi_LockFightMode(pchar, bLock);
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		idx = GetOfficersIndex(pchar, i);
		if (idx < 0 || idx == iOff) continue;
		LAi_LockFightMode(GetCharacter(idx), bLock);
	}
}

// Кандидат в зачинщики: живой наёмный офицер отряда со штатным диалогом.
bool Tavern_BrawlIsGoodOfficer(int idx)
{
	if (idx < 0) return false;
	ref rChr = GetCharacter(idx);
	if (LAi_IsDead(rChr)) return false;
	if (!GetRemovable(rChr)) return false;
	if (CheckAttribute(rChr, "prisoned")) return false;
	if (!CheckAttribute(rChr, "location")) return false;
	if (rChr.location != pchar.location) return false;
	if (!CheckAttribute(rChr, "dialog.filename")) return false;
	return rChr.dialog.filename == "Enc_Officer_dialog.c";
}

// Кандидат в жертвы: живой завсегдатай, сидящий в этой же таверне.
bool Tavern_BrawlIsGoodHabitue(int idx)
{
	ref rChr;
	makeref(rChr, Characters[idx]);
	if (!CheckAttribute(rChr, "dialog.filename")) return false;
	if (rChr.dialog.filename != "Habitue_dialog.c") return false;
	if (!CheckAttribute(rChr, "location")) return false;
	if (rChr.location != pchar.location) return false;
	return !LAi_IsDead(rChr);
}

// Поднять бойца в драку: личная группа, порог сдачи и никаких разговоров посреди потасовки.
void Tavern_BrawlSetFighter(ref rChr, string sGroup, string sQuestDown)
{
	// клинок у завсегдатая с собой есть, но не надет - сидя в таверне его убирают. Пока он
	// не в руке, движок не пускает бойца в боевой режим, и тот стоит перед врагом столбом.
	// Помечаем выданный клинок, чтобы после драки убрать его обратно в ножны.
	if (Tavern_BrawlEquipBlade(rChr)) rChr.questTemp.BrawlBlade = true;
	LAi_group_Register(sGroup);
	LAi_SetWarriorTypeNoGroup(rChr);
	LAi_warrior_SetStay(rChr, false);
	LAi_warrior_DialogEnable(rChr, false);
	LAi_group_MoveCharacter(rChr, sGroup);
	LAi_SetCheckMinHP(rChr, LAi_GetCharacterMaxHP(rChr) * TAVERNBRAWL_SURRENDER_PCT / 100.0, true, sQuestDown);
	// боевой режим мог быть заперт (в мирном зале оружие не достают) - снимаем замок и включаем сами,
	// строго ПОСЛЕ клинка: без него движок в боевой режим не пустит
	SendMessage(&rChr, "lsl", MSG_CHARACTER_EX_MSG, "LockFightMode", false);
	LAi_SetFightMode(rChr, true);
}

// Один из двоих выдохся - разнимаем.
void Tavern_BrawlStop()
{
	if (!CheckAttribute(pchar, "questTemp.TavernBrawl")) return;
	if (!CheckAttribute(pchar, "questTemp.TavernBrawl.started"))
	{
		DeleteAttribute(pchar, "questTemp.TavernBrawl"); // до драки не дошло, разнимать некого
		return;
	}

	ref rOff = GetCharacter(sti(pchar.questTemp.TavernBrawl.off));
	ref rHab = GetCharacter(sti(pchar.questTemp.TavernBrawl.hab));
	int iReason = sti(pchar.questTemp.TavernBrawl.reason);
	bool bNoFight = CheckAttribute(pchar, "questTemp.TavernBrawl.nofight");
	string sSitGroup = pchar.questTemp.TavernBrawl.sitgroup;
	string sSitLocator = pchar.questTemp.TavernBrawl.sitlocator;
	// работаем по локации ДРАКИ, а не по текущей: страховка зовёт нас уже из другой локации, и по
	// pchar.location завсегдатай уехал бы следом за игроком, а запрет боя вернулся бы не в ту дверь
	string sBrawlLoc = pchar.questTemp.TavernBrawl.loc;
	int iBrawlLoc = FindLocation(sBrawlLoc);

	Tavern_BrawlLockBystanders(false); // зрителям вернуть право на оружие (пока метка ещё цела)
	DeleteAttribute(pchar, "questTemp.TavernBrawl");

	if (bNoFight && iBrawlLoc >= 0) LAi_LocationFightDisable(&Locations[iBrawlLoc], true); // зал снова мирный

	LAi_group_SetRelation(TAVERNBRAWL_GROUP_OFF, TAVERNBRAWL_GROUP_HAB, LAI_GROUP_NEITRAL);

	Tavern_BrawlCalmDown(rHab);
	if (!LAi_IsDead(rHab))
	{
		// Возвращаем за его стол перестановкой. Дойти ногами тут нельзя: до сидячих локаторов движок
		// маршрут не строит вовсе (они вне патча ходьбы) и вместо ходьбы всё равно телепортирует -
		// в штатных сценах мод такие пересадки просто прячет за фейдером.
		pchar.questTemp.TavernBrawlSit.hab = sti(rHab.index);
		ChangeCharacterAddressGroup(rHab, sBrawlLoc, sSitGroup, sSitLocator);
		LAi_SetSitType(rHab);
		// оглушённый (красный кулак) доигрывает свою анимацию, и сидячая поза поверх неё не встаёт -
		// боец так и остаётся торчать у стола. Возвращаемся к нему, когда стан отойдёт.
		DoQuestFunctionDelay("Tavern_BrawlHabitueReseat", 4.0);
	}

	Tavern_BrawlCalmDown(rOff);
	if (!LAi_IsDead(rOff))
	{
		LAi_SetOfficerType(rOff);
		rOff.dialog.currentnode = "Brawl_" + iReason;
	}

	// счёт выставят, только если капитан всё ещё в том же зале - иначе спрашивать не с кого
	if (sBrawlLoc == pchar.location) DoQuestFunctionDelay("Tavern_BrawlBarmanCome", 2.5);
}

// Повторная посадка: к этому времени стан уже отыгран.
void Tavern_BrawlHabitueReseat(string qName)
{
	if (!CheckAttribute(pchar, "questTemp.TavernBrawlSit")) return;

	ref rHab = GetCharacter(sti(pchar.questTemp.TavernBrawlSit.hab));
	DeleteAttribute(pchar, "questTemp.TavernBrawlSit");

	if (LAi_IsDead(rHab)) return;
	// тип мог перебить кто-то ещё (добили, оглушили повторно) - тогда просто усаживаем заново
	if (!CheckAttribute(rHab, "chr_ai.type") || rHab.chr_ai.type != "sit") LAi_SetSitType(rHab);
	LAi_SetDefaultSitAnimation(rHab);
}

// Хозяин таверны идёт к капитану за платой: в его зале дерутся не бесплатно.
void Tavern_BrawlBarmanCome(string qName)
{
	int idx = GetCharIDXByParam("TavernMan", "location", pchar.location);
	if (idx < 0) return;                       // бармена рядом нет - и спрашивать некому
	if (chrDisableReloadToLocation) return;    // идёт другая сцена, не лезем поперёк

	ref rBarman = GetCharacter(idx);
	if (LAi_IsDead(rBarman)) return;

	// Бармен окликает капитана прямо от стойки и с места не сходит - так же, как он вмешивается,
	// когда ГГ замахивается на завсегдатая. Тип персонажа при этом не меняется, двигать его не нужно.
	// Выбора "платить или нет" не даём: есть деньги - берёт, нет - выставляет за дверь.
	string sNode = "BrawlFine_no";
	if (sti(pchar.money) >= TAVERNBRAWL_FINE) sNode = "BrawlFine_pay";
	StartInstantDialogNoType(rBarman.id, sNode, "Common_Tavern.c");
}

// Платить нечем - капитана выставляют на улицу штатным переходом этой таверны.
void Tavern_BrawlKickOut()
{
	int iLoc = FindLocation(pchar.location);
	if (iLoc < 0) return;
	if (!CheckAttribute(&Locations[iLoc], "reload.l1.go")) return;

	DoQuestReloadToLocation(Locations[iLoc].reload.l1.go, "reload", Locations[iLoc].reload.l1.emerge, "");
}

// Надеть клинок из собственной сумки, если боец безоружен. true = клинок достали ради драки.
bool Tavern_BrawlEquipBlade(ref rChr)
{
	aref arItems;
	string sName;
	ref rItem;
	int i, iItem;

	if (GetCharacterEquipByGroup(rChr, BLADE_ITEM_TYPE) != "") return false;
	if (!CheckAttribute(rChr, "items")) return false;

	makearef(arItems, rChr.items);
	for (i = 0; i < GetAttributesNum(arItems); i++)
	{
		sName = GetAttributeName(GetAttributeN(arItems, i));
		iItem = GetItemIndex(sName);   // ItemsFromID на неизвестном предмете отдаёт пустой ref
		if (iItem < 0) continue;
		rItem = &Items[iItem];
		if (!CheckAttribute(rItem, "groupID")) continue;
		if (rItem.groupID != BLADE_ITEM_TYPE) continue;
		EquipCharacterByItem(rChr, sName);
		return true;
	}
	return false;
}

// Снять боевой режим и бессмертие, выданные на время драки, и убрать клинок в ножны.
void Tavern_BrawlCalmDown(ref rChr)
{
	LAi_RemoveCheckMinHP(rChr);
	LAi_SetImmortal(rChr, false);
	// Снимаем оглушение. Иначе его таймер доработает уже ПОСЛЕ драки и по своему завершению сам
	// вернёт бойца в воина, да ещё и выхватит клинок (LAi_character.c) - усаженный за стол
	// завсегдатай через пару секунд вставал обратно. Заодно снимается замок боевого режима:
	// его ставит сам стан и после драки не снимает, без этого клинок не убрать в ножны.
	LAi_ClearStun(rChr);
	LAi_SetFightMode(rChr, false);
	// клинок доставали ради драки - иначе завсегдатай так и сядет за стол с саблей наголо
	if (CheckAttribute(rChr, "questTemp.BrawlBlade"))
	{
		DeleteAttribute(rChr, "questTemp.BrawlBlade");
		DeleteAttribute(rChr, "equip.blade");
		SetEquipedItemToCharacter(rChr, BLADE_ITEM_TYPE, "");
	}
}

// Страховка: сейв-лоад или уход из таверны посреди драки - обработчик уже не сработает.
void Tavern_BrawlSafetyCheck()
{
	// отложенная посадка могла не отработать, если игрок вышел из таверны сразу после драки
	DeleteAttribute(pchar, "questTemp.TavernBrawlSit");
	if (!CheckAttribute(pchar, "questTemp.TavernBrawl")) return;
	if (pchar.questTemp.TavernBrawl.loc == pchar.location) return;
	Tavern_BrawlStop();
}
