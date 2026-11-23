// Gregg: ==> трилогия «Чёрная Орхидея» (главы 4-6 сюжетной линии К:ПС)
//
// Источник: К:ПС, PROGRAM\lugger\MainLine.c, строки 1582-5089:
//   4. «Чёрная орхидея - таинственный артефакт»
//   5. «Чёрная орхидея - подарок судьбы»
//   6. «Чёрная орхидея - испанская щедрость»
// Имена функций сохранены как в оригинале, чтобы можно было сверяться с источником.
//
// Пока перенесены предметы и механика самой Орхидеи. Логика глав - следующим заходом.
//
// Отличия от оригинала, о которых договорились заранее:
// - Орхидея из сюжетной вещи, дававшей ДВА МЕСЯЦА полной неуязвимости, превращена
//   в носимый оберег: -10% входящего урона плюс одна «вторая жизнь» на локацию;
// - в главе 6 испанцам продают ПОДДЕЛКУ, изготовленную Кендартом, а настоящая
//   остаётся у игрока. В оригинале продавали настоящую, испортив её до трёх дней;
// - цена сделки 5 000 000 вместо 25 000 000, колония Сан-Хуан в награде не остаётся:
//   испанцы вскрывают обман и забирают своё обратно силой (эскадра из оригинала);
// - у игрока НЕ отбирают корабль и вещи. Вместо конфискации остров запирается
//   на главу, легенда - корабль на мели и команда его чинит;
// - вход с 17 ранга.

#define BO_RANK_REQ			17		// с какого ранга приходит посыльный с Бермуд
#define BO_DMG_REDUCE		0.10	// доля, на которую оберег срезает входящий урон
#define BO_LASTSTAND_SEC	10.0	// сколько длится неуязвимость после смертельного удара
#define BO_VERSTEK_RANK		45		// Верстехольстен, телохранитель Матильды: ранг
#define BO_VERSTEK_HP		1000.0	// и здоровье. Именной боец, слабаком быть не должен

// Gregg: глава 7, «Маленький ублюдок»
#define BO_SM_START_DAYS	7		// через сколько дней после главы 6 Кендарт зовёт снова (было 60)
#define BO_SM_VISIT_PRICE	1000	// плата Баррету за один визит к Джекмену
#define BO_SM_MONTH_PRICE	15000	// плата за месячный пропуск в резиденцию
#define BO_SM_MONTH_DAYS	30		// сколько дней действует месячный пропуск
#define BO_SM_FRANK_DAYS	2		// сколько дней Фрэнк трезвеет
#define BO_SM_SHUTTLE_DAYS	11		// как часто Харпич меняет берег стоянки
#define BO_SM_WAKE_PRICE	1000	// помянуть погибшую команду Бреддокса
#define BO_SM_NEXT_DAYS		7		// от конца главы 7 до главы 8 (было 30; «Дай мне неделю»)
#define BO_SM_BARRET_HP		1200.0	// сколько держит Баррет: боец знатный, но не бессмертный

// Gregg: глава 8, «Рауль Эсперанто»
#define BO_RE_NEWMAN_DAYS	7		// сколько ждать вестей о Ньюмене
#define BO_RE_WINDOW_DAYS	1		// сколько держится окно перехвата в форте
#define BO_RE_RETRY_DAYS	30		// через сколько дней окно откроется снова
#define BO_RE_NEWMAN_HP		700.0	// здоровье Ньюмена в тюремной драке
#define BO_RE_TORTUGA_DAYS	6		// сколько ждёт корабль под Тортугой
#define BO_RE_NEXT_DAYS		1		// от конца главы 8 до главы 9 (было 45; Рауль рассказал всё и умер ночью)
#define BO_RE_BRIBE			5000	// сколько берёт Хорхе де Маранья за разговор

// Gregg: глава 9, «Найти и уничтожить»
#define BO_FD_ANNA_DAYS		3		// через сколько дней «Анна» встанет у Сан-Хуана
#define BO_FD_BRUS_DAYS		26		// как часто Брус Фишер меняет берег
#define BO_FD_MINES_MONEY	200000	// казна в сундуке кайманских шахт
#define BO_FD_WAIT_DAYS		14		// сколько ждать вестей от Кендарта
#define BO_FD_NEXT_DAYS		14		// от конца главы 9 до главы 10 (было 45; «корсары возмущаются вторую неделю»)
// Gregg: запись журнала одной строкой - и сразу род ГГ. Функции в .txt не работают,
// поэтому в тексте записи стоят плейсхолдеры, а значения приходят отсюда.
// AddQuestUserData цепляется к ПОСЛЕДНЕЙ добавленной записи (quests.c:404), а не
// к квесту целиком, поэтому подстановку повторяем после КАЖДОЙ записи.
// sSex - обычное окончание, sSex1 - возвратная форма.
void BO_MARecord(string sRec)
{
	AddQuestRecord("MysteriousArtefact", sRec);
	AddQuestUserData("MysteriousArtefact", "sSex", GetSexPhrase("", "а"));
	AddQuestUserData("MysteriousArtefact", "sSex1", GetSexPhrase("ся", "ась"));
}



// Gregg: ==> хелперы К:ПС, которых нет в CSP. Ровно тот же набор, что понадобился
// «Кораблю обезьян»; там они живут как CM_*. Держу отдельные копии сознательно:
// captainmonkey.c уже оттестирован и закоммичен, и трогать его ради общей утилиты
// смысла нет. Свести в одно место стоит, когда трилогия будет готова и проверена.

// В К:ПС это DisableLandEncounters из lugger\utils.c.
void BO_SetEncounters(string sLocation, bool bDisable)
{
	int iLoc = FindLocation(sLocation);
	if (iLoc < 0) return;
	if (bDisable)
	{
		Locations[iLoc].DisableEncounters = true;
		return;
	}
	if (CheckAttribute(&Locations[iLoc], "DisableEncounters")) DeleteAttribute(&Locations[iLoc], "DisableEncounters");
}

// В К:ПС это FreeLocator. Общий FreeSitLocator из scripts\utils.c не годится:
// он выкидывает «в никуда» ВСЕХ, кто стоит на локаторе, включая квестового NPC.
// Gregg: sExceptId - тот, кого сгонять НЕЛЬЗЯ (обычно квестовый NPC, которого мы сюда
// же и сажаем). Без этого функция находила первым его самого, увозила в «никуда»,
// возвращалась - а настоящий сосед оставался на стуле. Пустая строка = сгонять любого.
// Чистим ВСЕХ подходящих, а не первого: на одном локаторе может оказаться и двое.
void BO_FreeLocator(string sLocation, string sLocator, string sExceptId)
{
	ref chr;
	int iN;
	// Gregg: TOTAL_CHARACTERS как надмножество. MAX_CHARACTERS - подвижная планка:
	// characters_init.c:217 ставит её в globalCharacters, а FindFirstEmptyCharacter
	// (utils.c:1061) двигает вверх под каждого сгенерированного, так что городские
	// фантомы в неё попадают. Верхняя граница просто гарантирует полный обход.
	for (iN = 0; iN < TOTAL_CHARACTERS; iN++)
	{
		makeref(chr, Characters[iN]);
		if (!CheckAttribute(chr, "location")) continue;
		if (chr.location != sLocation) continue;
		if (chr.location.locator != sLocator) continue;
		if (CheckAttribute(chr, "NotRemoved")) continue;
		if (iN == GetMainCharacterIndex()) continue;
		if (sExceptId != "")
		{
			if (chr.id == sExceptId) continue;
		}
		// Gregg: мгновенная смена адреса, как в общем FreeSitLocator, а НЕ
		// LogoffCharacter: тот уводит персонажа асинхронно, локатор в этом же кадре
		// остаётся занят, и квестовый NPC садится мимо стола. Адрес стираем руками -
		// сам вызов его не присваивает (characters_login.c:169-170 возвращает false
		// раньше), а в LAi_CharacterLogoff сброс закомментирован (LAi_login.c:661).
		ChangeCharacterAddressGroup(chr, "none", "none", "none");
		chr.location = "none";
		chr.location.group = "";
		chr.location.locator = "";
	}
}

// В К:ПС это SetQuestNPCDeadFunction - условие победы на смерть нескольких NPC
// с общим префиксом имени.
void BO_SetNPCDeadFunction(string sQuest, string sFunction, string sName, int iQty)
{
	int iN;
	string sKey;
	for (iN = 1; iN <= iQty; iN++)
	{
		sKey = "l" + iN;
		PChar.quest.(sQuest).win_condition.(sKey) = "NPC_Death";
		PChar.quest.(sQuest).win_condition.(sKey).character = sName + iN;
	}
	PChar.quest.(sQuest).function = sFunction;
}

// В К:ПС это SetSeaPerksToCharacter из lugger\utils.c - пачка морских перков
// квестовому капитану.
// Gregg: провал линии. Отдельного вида «квест провален» в CSP нет - CloseQuestHeader
// помечает запись как выполненную и рисует галочку. Поэтому перед закрытием красим
// заголовок красным: единственный доступный способ отличить провал от победы.
void BO_QuestFailed(string sQuest)
{
	SetQuestHeaderColor(sQuest, argb(255, 200, 60, 60));
	CloseQuestHeader(sQuest);
}

void BO_SetSeaPerks(ref chr)
{
	SetCharacterPerk(chr, "FastReload");
	SetCharacterPerk(chr, "HullDamageUp");
	SetCharacterPerk(chr, "SailsDamageUp");
	SetCharacterPerk(chr, "CrewDamageUp");
	SetCharacterPerk(chr, "CriticalShoot");
	SetCharacterPerk(chr, "LongRangeShoot");
	SetCharacterPerk(chr, "CannonProfessional");
	SetCharacterPerk(chr, "ShipDefenseProfessional");
	SetCharacterPerk(chr, "SwordplayProfessional");
	SetCharacterPerk(chr, "AdvancedDefense");
	SetCharacterPerk(chr, "CriticalHit");
}
// Gregg: <== хелперы К:ПС

// Gregg: оберег надет именно на этом персонаже. Зовётся из LAi_ApplyCharacterDamage,
// то есть на КАЖДЫЙ полученный удар любым бойцом в локации - поэтому первым стоит
// самый дешёвый отсев по наличию атрибута, а не перебор всей экипировки.
// Второй вызов - из LAi_CheckKillCharacter, там он редкий.
bool BlackOrchid_IsWorn(aref chr)
{
	if (!CheckAttribute(chr, "equip.talisman")) return false;
	if (chr.equip.talisman != "BlackOrchid") return false;
	return true;
}

// Gregg: смертельный удар не убивает владельца, а оставляет его на ногах и делает
// неуязвимым на BO_LASTSTAND_SEC секунд - есть время добить противника или отойти.
// Заряд один; восстанавливается при переходе в новую локацию.
void BlackOrchid_LastStand(aref chr)
{
	// Gregg: заряд считаем общим на отряд, а не на носителя. Орхидея уникальна,
	// носит её почти всегда ГГ, а хранить состояние на PChar заметно надёжнее:
	// офицер может уйти из локации или вовсе быть уволен вместе со своим флагом.
	PChar.BlackOrchid.Spent = true;
	PChar.BlackOrchid.StandIdx = chr.index;
	// Gregg: поколение срабатывания. DoQuestFunctionDelay ключуется именем функции,
	// поэтому старый отложенный вызов, оставшийся в очереди, мог снять неуязвимость
	// у НОВОГО срабатывания досрочно. Конец сверяет поколение и чужой не отрабатывает.
	int iGen = 1;
	if (CheckAttribute(PChar, "BlackOrchid.StandGen")) iGen = sti(PChar.BlackOrchid.StandGen) + 1;
	PChar.BlackOrchid.StandGen = iGen;
	LAi_SetImmortal(chr, true);
	DoQuestFunctionDelay("BlackOrchid_LastStandEnd", BO_LASTSTAND_SEC);
	// Gregg: без обратной связи спасение неотличимо от «оберег не сработал» - именно на этом
	// механика и потерялась в первый раз. Индикатор над персонажем закрыт настройкой
	// CharCap, поэтому дублируем строкой в лог, она видна всегда.
	if (sti(chr.index) == GetMainCharacterIndex())
	{
		setEffectText(chr, XI_ConvertString("BlackOrchidStand"), argb(255, 200, 120, 255), GetCharCapNotifScale());
		PlaySound("interface\heartbeat.wav");
		Log_InfoColoredAnim("green", XI_ConvertString("BlackOrchidStand"));
	}
}

void BlackOrchid_LastStandEnd(string _tmp)
{
	if (!CheckAttribute(PChar, "BlackOrchid.StandIdx")) return;
	// Gregg: снимаем неуязвимость только за своё поколение. Чужое - значит нас обогнало
	// более свежее срабатывание, и гасить его рано.
	int iGen = 0;
	if (CheckAttribute(PChar, "BlackOrchid.StandGen")) iGen = sti(PChar.BlackOrchid.StandGen);
	if (CheckAttribute(PChar, "BlackOrchid.StandAck"))
	{
		if (sti(PChar.BlackOrchid.StandAck) >= iGen) return;
	}
	PChar.BlackOrchid.StandAck = iGen;
	ref chr = GetCharacter(sti(PChar.BlackOrchid.StandIdx));
	DeleteAttribute(PChar, "BlackOrchid.StandIdx");
	// Gregg: известная неточность - если за эти десять секунд бессмертие успел выдать
	// какой-нибудь квест, мы его снимем вместе со своим. Отличить одно от другого
	// движок не даёт: флаг у персонажа один и без владельца.
	LAi_SetImmortal(chr, false);
	// Gregg: окно кончилось - об этом надо сказать, иначе игрок не знает, когда снова
	// стал смертным, и это опаснее, чем не знать о самом срабатывании.
	// «reload» приходит только из BlackOrchid_Recharge, когда окно закрывают досрочно
	// на смене локации: там сообщение ни к чему, бой уже позади. Отложенный вызов
	// передаёт имя квестовой функции (quests.c, DoQuestFunctionDelay), а не эту метку.
	if (_tmp == "reload") return;
	if (sti(chr.index) != GetMainCharacterIndex()) return;
	setEffectText(chr, XI_ConvertString("BlackOrchidStandEnd"), argb(255, 190, 150, 210), GetCharCapNotifScale());
	Log_InfoColoredAnim("yellow", XI_ConvertString("BlackOrchidStandEnd"));
}

// Gregg: зовётся из ReloadEndFade на каждой смене локации.
void BlackOrchid_Recharge()
{
	if (!CheckAttribute(PChar, "BlackOrchid.Spent")) return;
	// Gregg: если предыдущее срабатывание ещё не отыграно, закрываем окно прямо здесь,
	// а не ждём отложенного вызова. Ждать нельзя: вызов может не пережить смену локации,
	// и тогда носитель останется бессмертным навсегда, а заряд не вернётся никогда
	// (у ГГ бессмертие вдобавок отключает урон по его кораблю). Несколько потерянных
	// секунд неуязвимости не жалко - из боя всё равно вышли.
	if (CheckAttribute(PChar, "BlackOrchid.StandIdx")) BlackOrchid_LastStandEnd("reload");
	DeleteAttribute(PChar, "BlackOrchid.Spent");
	LAi_SetCurHPMax(PChar);
}

// Gregg: ==> глава 4. «Чёрная орхидея - таинственный артефакт»

// Gregg: проверка готовности зовётся из ReloadEndFade, то есть на каждой смене
// локации. В К:ПС она висела в LAi_utilites.c на том же по смыслу месте. Дороговизны
// тут нет: как только квест взведён, атрибут есть и функция выходит первой строкой.
void MysteriousArtefactCheckCan()
{
	if (CheckAttribute(PChar, "MysteriousArtefact")) return;
	// Gregg: в Storm нет короткого замыкания, поэтому предыдущие главы проверяем
	// по одной - на новой игре этих атрибутов может не быть вовсе
	if (!CheckAttribute(PChar, "UneasyDelivery")) return;
	if (!CheckAttribute(PChar, "TreacheryOfGrifoSajfus")) return;
	if (PChar.UneasyDelivery != "END") return;
	if (PChar.TreacheryOfGrifoSajfus != "END") return;
	// Gregg: ранг читаем напрямую. GetRank(PChar, 0) из оригинала в CSP прибавляет
	// ранг ГГ ещё раз внутри и вернула бы удвоенное значение.
	// Общий тумблер «квесты без требований к рангу» читаем живьём, как в CM_Allen.c:
	// галочка стоит - завязка открыта с первого ранга. Здесь порог живёт в логике,
	// а не в сегменте диалога, поэтому дефайн доезжает без оговорок.
	int iRankReq = 1;
	if (!bQuestsRank) iRankReq = BO_RANK_REQ;
	if (sti(PChar.rank) < iRankReq) return;

	PChar.MysteriousArtefact = "CanTake";
	PChar.quest.MysteriousArtefactNoticeFromBermudes.win_condition.l1 = "Location_Type";
	PChar.quest.MysteriousArtefactNoticeFromBermudes.win_condition.l1.location_type = "town";
	PChar.quest.MysteriousArtefactNoticeFromBermudes.function = "MysteriousArtefactNoticeFromBermudes";
}

// В любом порту к ГГ подбегает посыльный с письмом от бермудского трактирщика.
void MysteriousArtefactNoticeFromBermudes(string qName)
{
	chrDisableReloadToLocation = true;

	string sModel = "citiz_" + (rand(9)+1);
	ref chr = GetCharacter(NPC_GenerateCharacter("MA Notice", sModel, "man", "man", 1, PIRATE, 0, false));
	chr.greeting = "cit_quest";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Messenger.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto1");
	SetDialogCharacter2Character(PChar, chr, 60.0, false);
}

// Gregg: К:ПС держала три отдельных заголовка квеста (_MysteriousArtefact_,
// _MysteriousArtefact и MysteriousArtefact), закрывая предыдущий и открывая
// следующий - так она добивалась нужного порядка записей в журнале. У нас
// заголовок один: порядком записей это не портит, а в журнале не плодит призраков.
void MysteriousArtefactFirstToBermudes()
{
	chrDisableReloadToLocation = false;

	SetQuestHeader("MysteriousArtefact");
	BO_MARecord("1");
	AddQuestUserData("MysteriousArtefact", "sBarman", GetFullName(CharacterFromID("Pirates_tavernkeeper")));

	ref chr = CharacterFromID("MA Notice");
	LAi_SetActorType(chr);
	// Gregg: таймаут -1 отключает LAi_tmpl_runto_Teleport, актёр залипает в «falure»
	LAi_ActorRunToLocation(chr, "reload", "reload4_back", "none", "", "", "", 30.0);
}

// Разговор с бермудским трактирщиком: Фредерик Левинсон ушёл в джунгли.
void MysteriousArtefactStart()
{
	BO_MARecord("2");
	PChar.MysteriousArtefact = "1";

	ref chr = GetCharacter(NPC_GenerateCharacter("Frederik Levinson", "officer_20", "man", "man", 1, PIRATE, -1, false));
	FantomMakeCoolFighter(chr, 40, 40, 40, BLADE_LONG, "pistol5", 100);
	chr.name = iTxt("BO_LevinsonName");
	chr.lastname = iTxt("BO_LevinsonLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Levinson.c";
	chr.Dialog.CurrentNode = "1";
	chr.NotRemoved = true;
	chr.greeting = "Gr_Atilla";
	LAi_SetImmortal(chr, true);

	BO_SetEncounters("Bermudes_CaveEntrance", true);
	PChar.quest.MysteriousArtefactPreToBermudesCaveEntrance.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactPreToBermudesCaveEntrance.win_condition.l1.location = "Bermudes_CaveEntrance";
	PChar.quest.MysteriousArtefactPreToBermudesCaveEntrance.function = "MysteriousArtefactPreToBermudesCaveEntrance";
}

void MysteriousArtefactPreToBermudesCaveEntrance(string qName)
{
	DoQuestFunctionDelay("MysteriousArtefactToBermudesCaveEntrance", 0.1);
}

// Левинсон у пещеры дерётся с Брайаном Джорджем - ГГ приходит к концу схватки.
void MysteriousArtefactToBermudesCaveEntrance(string _tmp)
{
	chrDisableReloadToLocation = true;

	LAi_SetActorType(PChar);

	ref chr = CharacterFromID("Frederik Levinson");
	ChangeCharacterAddressGroup(chr, "Bermudes_CaveEntrance", "monsters", "monster1");
	LAi_SetActorType(chr);

	chr = GetCharacter(NPC_GenerateCharacter("George Bryan", "officer_5", "man", "man", 1, PIRATE, 0, false));
	chr.name = iTxt("BO_BryanName");
	chr.lastname = iTxt("BO_BryanLastName");
	FantomMakeCoolFighter(chr, 15, 10, 10, BLADE_SHORT, "pistol4", 100);
	LAi_SetHP(chr, 600.0, 600.0);
	ChangeCharacterAddressGroup(chr, "Bermudes_CaveEntrance", "monsters", "monster2");
	LAi_SetActorType(chr);

	DoQuestFunctionDelay("MysteriousArtefactToBermudesCaveEntranceBattle", 2.5);
}

void MysteriousArtefactToBermudesCaveEntranceBattle(string qName)
{
	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Frederik Levinson");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	chr = CharacterFromID("George Bryan");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.MysteriousArtefactToBermudesCaveEntranceBattleWin.win_condition.l1 = "NPC_Death";
	PChar.quest.MysteriousArtefactToBermudesCaveEntranceBattleWin.win_condition.l1.character = "George Bryan";
	PChar.quest.MysteriousArtefactToBermudesCaveEntranceBattleWin.function = "MysteriousArtefactToBermudesCaveEntranceBattleWin";

	locCameraFollow();
}

void MysteriousArtefactToBermudesCaveEntranceBattleWin(string qName)
{
	LAi_Fade("", "");
	SetDialogCharacter2Character(PChar, CharacterFromID("Frederik Levinson"), 5.0, true);
}

// Продолжение разговора - в таверне Бермуд.
void MysteriousArtefactToBermudesTavern()
{
	LAi_SetPlayerType(PChar);
	PChar.MysteriousArtefact = "2";
	BO_MARecord("3");

	ref chr = CharacterFromID("Frederik Levinson");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload2_back", "none", "", "", "", 30.0);

	BO_SetEncounters("Bermudes_CaveEntrance", false);
	chrDisableReloadToLocation = false;

	PChar.quest.MysteriousArtefactInBermudesTavern.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInBermudesTavern.win_condition.l1.location = "Pirates_tavern";
	PChar.quest.MysteriousArtefactInBermudesTavern.function = "MysteriousArtefactInBermudesTavern";

	// Gregg: Левинсона сажаем в таверну заранее, как только ГГ уйдёт с пещеры -
	// иначе игрок придёт первым и увидит пустой стол
	PChar.quest.MysteriousArtefactFrederikLevinsonInBermudesTavern.win_condition.l1 = "ExitFromLocation";
	PChar.quest.MysteriousArtefactFrederikLevinsonInBermudesTavern.win_condition.l1.location = "Bermudes_CaveEntrance";
	PChar.quest.MysteriousArtefactFrederikLevinsonInBermudesTavern.function = "MysteriousArtefactFrederikLevinsonInBermudesTavern";
}

void MysteriousArtefactFrederikLevinsonInBermudesTavern(string qName)
{
	BO_FreeLocator("Pirates_tavern", "sit_base4", "Frederik Levinson");
	ref chr = CharacterFromID("Frederik Levinson");
	ChangeCharacterAddressGroup(chr, "Pirates_tavern", "sit", "sit_base4");
	LAi_SetSitType(chr);
}

void MysteriousArtefactInBermudesTavern(string qName)
{
	ref chr = CharacterFromID("Frederik Levinson");
	chr.Dialog.CurrentNode = "5";

	// Gregg: освободить стул мало - Левинсона надо ПЕРЕСАДИТЬ на него заново. Адрес ему
	// раздали, пока таверна была выгружена, и на загрузке генератор занял место своим
	// пиратом, а Левинсона отбросило рядом со столом. Здесь таверна уже загружена,
	// так что сначала убираем постороннего, потом сажаем Левинсона на его место.
	BO_FreeLocator("Pirates_tavern", "sit_base4", "Frederik Levinson");
	ChangeCharacterAddressGroup(chr, "Pirates_tavern", "sit", "sit_base4");

	BO_FreeLocator("Pirates_tavern", "sit_front4", "");
	ChangeCharacterAddressGroup(PChar, "Pirates_tavern", "sit", "sit_front4");
	LAi_SetSitType(PChar);
	LAi_SetActorType(chr);
	LAi_ActorSetSitMode(chr);
	LAi_ActorDialogNow(chr, PChar, "", -1);
}

// Левинсону нужны деньги, а его должник живёт в Порт-о-Принсе.
void MysteriousArtefactToPortPax()
{
	LAi_SetSitType(CharacterFromID("Frederik Levinson"));
	LAi_SetPlayerType(PChar);
	ChangeCharacterAddressGroup(PChar, "Pirates_tavern", "goto", "goto1");

	PChar.MysteriousArtefact = "3";
	BO_MARecord("4");

	PChar.quest.MysteriousArtefactInPortPax.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInPortPax.win_condition.l1.location = "PortPax_town";
	PChar.quest.MysteriousArtefactInPortPax.function = "MysteriousArtefactInPortPax";
}

void MysteriousArtefactInPortPax(string qName)
{
	BO_MARecord("5");
}

// Gregg: вымести городской генератор из клон-локации. DisableEncounters на него не
// влияет - это про случайные встречи, а жителей и стражу заводит CreateCitizens
// (LAi_login.c при каждой загрузке локации). Он гейтится isLocationHasCitizens:
// достаточно ОДНОГО персонажа с атрибутом CityType в локации, чтобы генератор
// пропустил её целиком. Поэтому сначала сносим всех, кто уже нагенерился, а потом
// вешаем CityType на своего бойца - и повторная загрузка локации город не заселит.
void BO_ClearCitizens(string sLocation, string sKeeperId)
{
	ref chr;
	int iN;
	// Gregg: TOTAL_CHARACTERS как надмножество. MAX_CHARACTERS - подвижная планка:
	// characters_init.c:217 ставит её в globalCharacters, а FindFirstEmptyCharacter
	// (utils.c:1061) двигает вверх под каждого сгенерированного, так что городские
	// фантомы в неё попадают. Верхняя граница просто гарантирует полный обход.
	for (iN = 0; iN < TOTAL_CHARACTERS; iN++)
	{
		makeref(chr, Characters[iN]);
		if (!CheckAttribute(chr, "CityType")) continue;
		if (!CheckAttribute(chr, "location")) continue;
		if (chr.location != sLocation) continue;
		if (iN == GetMainCharacterIndex()) continue;
		// Gregg: адрес стираем руками - см. пояснение в BO_FreeLocator. Без этого
		// выселенный остаётся приписан к локации, и LoginCharactersInLocation
		// (characters_login.c:10) возвращает его при следующей же загрузке.
		ChangeCharacterAddressGroup(chr, "none", "none", "none");
		chr.location = "none";
		chr.location.group = "";
		chr.location.locator = "";
	}
	if (sKeeperId == "") return;
	if (GetCharacterIndex(sKeeperId) < 0) return;
	chr = CharacterFromID(sKeeperId);
	chr.CityType = "soldier";
}

// Gregg: зовётся из ReloadEndFade на КАЖДОЙ смене локации. Вешать «хранителя»
// с CityType на своего бойца оказалось ненадёжно: к моменту возврата в город он
// уже убит и вычищен по LifeDay, гейт isLocationHasCitizens отпускает генератор,
// и город заселяется заново. Поэтому чистим по факту входа. Дорого не выходит:
// пока атрибута MysteriousArtefact нет, функция выходит первой же строкой.
void BlackOrchid_CloneCleanup()
{
	if (!CheckAttribute(PChar, "MysteriousArtefact")) return;
	// Gregg: именно reload_location_index, а НЕ PChar.location. ReloadEndFade бежит
	// ДО ReloadToLocation (reload.c:388), а PChar.location присваивается только
	// внутри неё (reload.c:571) - то есть здесь он ещё указывает на ПОКИДАЕМУЮ
	// локацию, и гвард отсекал всё подряд.
	if (reload_location_index < 0) return;
	string sLoc = Locations[reload_location_index].id;
	// клон-локации все с этим префиксом, кроме склада - он из цепочки убран
	if (sLoc != "MA_Santiago_town")
	{
		if (sLoc != "MA_Santiago_ExitTown")
		{
			if (sLoc != "MA_Santiago_townhall")
			{
				if (sLoc != "MA_Santiago_store") return;
			}
		}
	}
	BO_ClearCitizens(sLoc, "");
}

// LEO: клон-локации осаждённого Сантьяго. По id, а не по метке: метку старые сейвы, где клон уже
//   подготовлен, не получили бы.
bool BO_IsSantiagoClone(string sLoc)
{
	return (findsubstr(sLoc, "MA_Santiago", 0) == 0);
}

// Gregg: снять с клона флаги городского генератора. Это и есть надёжный способ:
// КАЖДАЯ ветка CreateCitizens (LAi_utilites.c) гейтится атрибутом самой локации, а
// клон приезжает через CopyAttributes со всеми флагами оригинала. Подметать следствие
// бесполезно - квестовые функции бегут из QuestsCheck (LAi_login.c:297), то есть
// на две строки РАНЬШЕ самого CreateCitizens (LAi_login.c:299).
// fantoms - отдельная беда: по нему LAi_events.c:678 воскрешает убитых, и квестовый
// бой в клоне не кончался бы никогда.
// packhouse - гейт CreateSkladInsideEncounters, того самого кладовщика на складе.
void BO_StripCloneSpawners(string sLocation)
{
	int iLoc = FindLocation(sLocation);
	if (iLoc < 0) return;
	if (CheckAttribute(&Locations[iLoc], "citizens")) DeleteAttribute(&Locations[iLoc], "citizens");
	if (CheckAttribute(&Locations[iLoc], "soldiers")) DeleteAttribute(&Locations[iLoc], "soldiers");
	if (CheckAttribute(&Locations[iLoc], "carrier")) DeleteAttribute(&Locations[iLoc], "carrier");
	if (CheckAttribute(&Locations[iLoc], "questSeekCap")) DeleteAttribute(&Locations[iLoc], "questSeekCap");
	if (CheckAttribute(&Locations[iLoc], "fantoms")) DeleteAttribute(&Locations[iLoc], "fantoms");
	if (CheckAttribute(&Locations[iLoc], "packhouse")) DeleteAttribute(&Locations[iLoc], "packhouse");
}

// Gregg: снять запрет на обнажение оружия. Флаг noFight приезжает в клон вместе
// со всеми атрибутами оригинала (CopyAttributes), а в захваченном городе драться
// нужно везде, включая лавку с телохранителем Матильды.
void BO_AllowFight(string sLocation)
{
	int iLoc = FindLocation(sLocation);
	if (iLoc < 0) return;
	if (CheckAttribute(&Locations[iLoc], "noFight")) DeleteAttribute(&Locations[iLoc], "noFight");
}

// Gregg: правки карт переходов в фабриках действуют только на новую игру - массив
// Locations целиком лежит в сейве. Поэтому те же три записи чиним в рантайме.
// Присваивания идемпотентны, звать можно сколько угодно.
void BO_FixSantiagoCloneDoors()
{
	// Gregg: город осаждён - генератор мирных жителей и стражи тут не нужен, драться
	// можно везде. noFight и DisableEncounters приезжают клоном через CopyAttributes.
	BO_SetEncounters("MA_Santiago_town", true);
	BO_SetEncounters("MA_Santiago_ExitTown", true);
	BO_SetEncounters("MA_Santiago_townhall", true);
	BO_SetEncounters("MA_Santiago_store", true);
	BO_SetEncounters("MA_Store_Packhouse", true);
	BO_AllowFight("MA_Santiago_town");
	BO_AllowFight("MA_Santiago_ExitTown");
	BO_AllowFight("MA_Santiago_townhall");
	BO_AllowFight("MA_Santiago_store");
	BO_AllowFight("MA_Store_Packhouse");
	BO_StripCloneSpawners("MA_Santiago_town");
	BO_StripCloneSpawners("MA_Santiago_ExitTown");
	BO_StripCloneSpawners("MA_Santiago_townhall");
	BO_StripCloneSpawners("MA_Santiago_store");
	BO_StripCloneSpawners("MA_Store_Packhouse");

	int iX = FindLocation("MA_Santiago_ExitTown");
	if (iX >= 0)
	{
		// ворота, а не дверь резиденции
		Locations[iX].reload.l1.emerge = "gate_back";
	}
	int iT = FindLocation("MA_Santiago_town");
	if (iT < 0) return;
	// Gregg: fastreload клон унаследовал от настоящего города, и по быстрому переходу
	// игрок уходил в НАСТОЯЩИЙ Сантьяго - там мирные жители, стража и запрет оружия
	// в лавке. Пока идёт штурм, быстрый переход гасим целиком.
	bDisableFastReload = true;
	Locations[iT].reload.l3.emerge = "reload1";
	Locations[iT].reload.l4.emerge = "reload1";
	// обратный выход из клон-города наружу, которого не было вовсе
	Locations[iT].reload.gate.name = "gate";
	Locations[iT].reload.gate.go = "MA_Santiago_ExitTown";
	Locations[iT].reload.gate.emerge = "reload3";
	Locations[iT].reload.gate.autoreload = "0";
	Locations[iT].reload.gate.label = "ExitTown";
}


void BO_RepairBryanHouseReturn()
{
	int iHouse = FindLocation("HouseGeorgeBryan");
	int iRoom = FindLocation("HouseGeorgeBryanRoom");
	if (iHouse < 0) return;
	if (iRoom < 0) return;
	if (!CheckAttribute(&Locations[iHouse], "filespath.models")) return;
	if (Locations[iHouse].filespath.models != "locations\inside\FlamHouse") return;
	if (!CheckAttribute(&Locations[iHouse], "models.day.locators")) return;
	if (Locations[iHouse].models.day.locators != "FlamHouse_locators") return;
	if (!CheckAttribute(&Locations[iHouse], "reload")) return;
	if (!CheckAttribute(&Locations[iRoom], "reload")) return;

	string sHouseDoor = UD_FindReloadKeyByName("HouseGeorgeBryan", "reload3");
	string sRoomDoor = UD_FindReloadKeyByName("HouseGeorgeBryanRoom", "reload1");
	if (sHouseDoor == "") return;
	if (sRoomDoor == "") return;
	aref arHouseDoor, arRoomDoor;
	makearef(arHouseDoor, Locations[iHouse].reload.(sHouseDoor));
	makearef(arRoomDoor, Locations[iRoom].reload.(sRoomDoor));
	if (!CheckAttribute(arHouseDoor, "go")) return;
	if (arHouseDoor.go != "HouseGeorgeBryanRoom") return;
	if (!CheckAttribute(arHouseDoor, "emerge")) return;
	if (arHouseDoor.emerge != "reload1") return;
	if (!CheckAttribute(arRoomDoor, "go")) return;
	if (arRoomDoor.go != "HouseGeorgeBryan") return;
	if (!CheckAttribute(arRoomDoor, "emerge")) return;
	if (arRoomDoor.emerge != "reload2") return;
	// LEO: исправляем только старый возврат этажом ниже, не пересоздавая сохранённый дом.
	arRoomDoor.emerge = "reload3";
}

// Gregg: дверь houseSp1 в Порт-о-Принсе в обычной игре ведёт в рандомный
// CommonDoubleflourHouse_2. Перецеплять её насовсем нельзя - обычный дом из города
// пропадёт, на этих граблях мод уже стоял. Поэтому на время квеста подменяем,
// а по его окончании возвращаем заглушку. Хелперы UD_* написаны для домов
// «Тилльманна», но они общие и переписывать их третий раз незачем.
void BO_OpenBryanHouse(bool bOpen)
{
	BO_RepairBryanHouseReturn();
	UD_TuneHouse("HouseGeorgeBryan", "BO_BryanHouse");
	UD_SetHouseDoor("PortPax_town", "houseSp1", "HouseGeorgeBryan", "CommonDoubleflourHouse_2", "BO_BryanHouse", bOpen);
	if (bOpen) return;
	// Gregg: по ходу квеста дверь ещё и запирается (см. MysteriousArtefactPortPaxWomanHelp).
	// UD_SetHouseDoor снимает disable только при ОТКРЫТИИ, поэтому на закрытии чистим сами:
	// иначе после квеста обычный дом останется запертым навсегда.
	int iTown = FindLocation("PortPax_town");
	if (iTown < 0) return;
	string sKey = UD_FindReloadKeyByName("PortPax_town", "houseSp1");
	if (sKey == "") return;
	if (CheckAttribute(&Locations[iTown], "reload." + sKey + ".disable")) DeleteAttribute(&Locations[iTown], "reload." + sKey + ".disable");
}

// Gregg: запереть/отпереть уже подменённую дверь, не трогая, куда она ведёт.
void BO_LockBryanDoor(bool bLock)
{
	int iTown = FindLocation("PortPax_town");
	if (iTown < 0) return;
	string sKey = UD_FindReloadKeyByName("PortPax_town", "houseSp1");
	if (sKey == "") return;
	if (bLock)
	{
		Locations[iTown].reload.(sKey).disable = 1;
		return;
	}
	if (CheckAttribute(&Locations[iTown], "reload." + sKey + ".disable")) DeleteAttribute(&Locations[iTown], "reload." + sKey + ".disable");
}

// Дом открывается после разговора в таверне Порт-о-Принса.
void MysteriousArtefactToHouseGeorgeBryan()
{
	BO_MARecord("6");
	PChar.MysteriousArtefact = "4";
	BO_OpenBryanHouse(true);

	PChar.quest.MysteriousArtefactInHouseGeorgeBryan.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryan.win_condition.l1.location = "HouseGeorgeBryan";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryan.function = "MysteriousArtefactInHouseGeorgeBryan";
}

// В доме сидит бродяга Жак и утверждает, что сундук заперт, а ключ хозяин потерял.
void MysteriousArtefactInHouseGeorgeBryan(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("MA Jacque", "panhandler_4", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_JacqueName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Jacque.c";
	chr.Dialog.CurrentNode = "1";
	// Gregg: Gr_Crazy из К:ПС в CSP нет ни в одном Greetings_alias.ini, а Жак попрошайка
	chr.greeting = "Gr_poorman";
	ChangeCharacterAddressGroup(chr, "HouseGeorgeBryan", "goto", "goto3");
	LAi_SetImmortal(chr, true);

	SetDialogCharacter2Character(PChar, chr, 5.0, false);
}

// Жак уходит наверх, а сундук ждёт ключа.
void MysteriousArtefactInHouseGeorgeBryanJacqueToRoom()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("MA Jacque");
	LAi_SetActorType(chr);
	// Gregg: reload3 - лестница на второй этаж, см. комментарий в DefineHouseGeorgeBryan
	LAi_ActorRunToLocation(chr, "reload", "reload3", "none", "", "", "", 30.0);

	LAi_SetPlayerType(PChar);

	// Gregg: запираем сундук через QuestClosed, а НЕ через .key: проверку ключа
	// itemLogic.c делает только для локаторов, в имени которых есть «private»
	// (itemLogic.c:587), а у нас box1 - обычный сундук. Атрибут .key на нём
	// не работал вовсе, и сундук открывался свободно. Снимаем в
	// MysteriousArtefactToHouseGeorgeBryanOpen, когда игрок уже с ключом.
	int iLoc = FindLocation("HouseGeorgeBryan");
	Locations[iLoc].box1.QuestClosed = true;

	PChar.quest.MysteriousArtefactInHouseGeorgeBryanChestClosed.win_condition.l1 = "locator";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryanChestClosed.win_condition.l1.location = "HouseGeorgeBryan";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryanChestClosed.win_condition.l1.locator_group = "box";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryanChestClosed.win_condition.l1.locator = "box1";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryanChestClosed.function = "MysteriousArtefactInHouseGeorgeBryanChestClosed";
}

// ГГ сунулся к сундуку и убедился, что тот заперт. Жак наверху, к нему можно подняться.
void MysteriousArtefactInHouseGeorgeBryanChestClosed(string qName)
{
	BO_MARecord("7");
	PChar.MysteriousArtefact = "5";

	Locations[FindLocation("HouseGeorgeBryan")].reload.l2.disable = 0;
	Locations[FindLocation("HouseGeorgeBryanRoom")].locators_radius.goto.goto1 = 1.5;

	ref chr = CharacterFromID("MA Jacque");
	chr.Dialog.CurrentNode = "8";
	ChangeCharacterAddressGroup(chr, "HouseGeorgeBryanRoom", "goto", "goto1");

	LAi_SetActorType(chr);
	LAi_ActorSetLayMode(chr);

	PChar.quest.MysteriousArtefactInHouseGeorgeBryanRoom.win_condition.l1 = "locator";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryanRoom.win_condition.l1.location = "HouseGeorgeBryanRoom";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryanRoom.win_condition.l1.locator_group = "goto";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryanRoom.win_condition.l1.locator = "goto1";
	PChar.quest.MysteriousArtefactInHouseGeorgeBryanRoom.function = "MysteriousArtefactInHouseGeorgeBryanRoom";
}

void MysteriousArtefactInHouseGeorgeBryanRoom(string qName)
{
	LAi_ActorDialogNow(CharacterFromID("MA Jacque"), PChar, "", -1);
}

// Жак вспомнил, что хозяин потерял ключ в таверне.
void MysteriousArtefactToPortPaxTavern()
{
	LAi_SetPlayerType(PChar);

	BO_MARecord("8");
	PChar.MysteriousArtefact = "6";

	// Gregg: радиус локатора возвращаем обратно, иначе разговор с Жаком будет
	// запускаться каждый раз, как игрок пройдёт мимо
	Locations[FindLocation("HouseGeorgeBryanRoom")].locators_radius.goto.goto1 = 0.5;
}

// Официантка отдаёт ключ - за деньги или иначе, смотря как с ней говорили.
void MysteriousArtefactFirstKeyGive()
{
	TakeNItems(PChar, "HouseGeorgeBryan_Key", 1);

	// Gregg: в оригинале тут switch с ТРЕМЯ одинаковыми метками case 1 - опечатка,
	// из-за которой запись в журнал всегда получалась одна и та же (а точнее, пустая
	// строка при любом значении, кроме единицы). Переписано на if/else по-честному.
	string sNotice = "9";
	int iTalk = 1;
	if (CheckAttribute(PChar, "MysteriousArtefact.WaitressTalk")) iTalk = sti(PChar.MysteriousArtefact.WaitressTalk);
	if (iTalk == 2) sNotice = "10";
	if (iTalk == 3) sNotice = "11";

	AddQuestRecord("MysteriousArtefact", sNotice);
	PChar.MysteriousArtefact = "7";

	PChar.quest.MysteriousArtefactToPortPaxTownDialog.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactToPortPaxTownDialog.win_condition.l1.location = "PortPax_town";
	PChar.quest.MysteriousArtefactToPortPaxTownDialog.function = "MysteriousArtefactToPortPaxTownDialog";
}

// На выходе из таверны ГГ останавливает незнакомка и просит о помощи.
void MysteriousArtefactToPortPaxTownDialog(string qName)
{
	chrDisableReloadToLocation = true;

	// Gregg: в К:ПС тут стоял girl_3, но у нас под этим именем лежит другая внешность -
	// избитая девушка из энкаунтера с насильниками. Матильда играет добропорядочную
	// горожанку, жену состоятельного человека, поэтому берём модель из «дамских»:
	// lady01 на Эспаньоле больше нигде не занята (см. characters\init\*.c).
	ref chr = GetCharacter(NPC_GenerateCharacter("MA Woman", "lady01", "woman", "towngirl", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_WomanName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Woman.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Enc_RapersGirl_1";
	ChangeCharacterAddressGroup(chr, "PortPax_town", "goto", "goto19");
	LAi_SetImmortal(chr, true);

	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

// ГГ соглашается помочь незнакомке - и попадает в ловушку.
void MysteriousArtefactPortPaxWomanHelp()
{
	ref chr = CharacterFromID("MA Woman");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload7", "PortPax_church", "goto", "goto1", "", 30.0);

	BO_MARecord("12");
	PChar.MysteriousArtefact = "8";

	// Gregg: дом Брайана на время этой ветки запирается. Именно ЗАПИРАЕТСЯ, а не
	// перецепляется обратно: за дверью остался наш сундук, туда ещё возвращаться
	BO_LockBryanDoor(true);

	PChar.quest.MysteriousArtefactPortPaxHouseBryanGeorge.win_condition.l1 = "locator";
	PChar.quest.MysteriousArtefactPortPaxHouseBryanGeorge.win_condition.l1.location = "PortPax_town";
	PChar.quest.MysteriousArtefactPortPaxHouseBryanGeorge.win_condition.l1.locator_group = "reload";
	PChar.quest.MysteriousArtefactPortPaxHouseBryanGeorge.win_condition.l1.locator = "houseSp1";
	PChar.quest.MysteriousArtefactPortPaxHouseBryanGeorge.function = "MysteriousArtefactPortPaxHouseBryanGeorge";

	BO_SetEncounters("Hispaniola_Jungle_06", true);
	PChar.quest.MysteriousArtefactBattleDrunkWithCaptain.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactBattleDrunkWithCaptain.win_condition.l1.location = "Hispaniola_Jungle_06";
	PChar.quest.MysteriousArtefactBattleDrunkWithCaptain.function = "MysteriousArtefactBattleDrunkWithCaptain";

	chrDisableReloadToLocation = false;
}

// Подсказка, если игрок ткнулся в запертую дверь Брайана.
void MysteriousArtefactPortPaxHouseBryanGeorge(string qName)
{
	PlayStereoSound("new_round");
	// Gregg: русских литералов в общих скриптах быть не должно - в английской игре
	// они текут в лог как есть. Тексты вынесены в Interface.txt
	Log_Info(iTxt("BO_LockedDoor_1"));
	Log_Info(iTxt("BO_LockedDoor_2"));
}

// За городом ГГ встречает "того самого типа" - и убивает его.
void MysteriousArtefactBattleDrunkWithCaptain(string qName)
{
	chrDisableReloadToLocation = true;

	// Gregg: GetRank(PChar, 10) из оригинала в CSP прибавила бы ранг ГГ дважды
	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	ref chr = GetCharacter(NPC_GenerateCharacter("Bertran Grenadie", "usurer_4", "man", "man", iRank, PIRATE, 0, false));
	chr.name = iTxt("BO_GrenadieName");
	chr.lastname = iTxt("BO_GrenadieLastName");
	FantomMakeCoolFighter(chr, 20, 20, 20, BLADE_NORMAL, "pistol6", 100);
	LAi_SetHP(chr, 700.0, 700.0);
	ChangeCharacterAddressGroup(chr, "Hispaniola_Jungle_06", "goto", "goto2");

	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.MysteriousArtefactBattleDrunkWithCaptainWin.win_condition.l1 = "NPC_Death";
	PChar.quest.MysteriousArtefactBattleDrunkWithCaptainWin.win_condition.l1.character = "Bertran Grenadie";
	PChar.quest.MysteriousArtefactBattleDrunkWithCaptainWin.function = "MysteriousArtefactBattleDrunkWithCaptainWin";
}

void MysteriousArtefactBattleDrunkWithCaptainWin(string qName)
{
	BO_SetEncounters("Hispaniola_Jungle_06", false);
	chrDisableReloadToLocation = false;

	BO_MARecord("13");
	PChar.MysteriousArtefact = "9";

	PChar.quest.MysteriousArtefactBackToPortPax.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactBackToPortPax.win_condition.l1.location = "PortPax_town";
	PChar.quest.MysteriousArtefactBackToPortPax.function = "MysteriousArtefactBackToPortPax";
}

// В городе ГГ встречает солдат: убитый оказался героем Франции.
void MysteriousArtefactBackToPortPax(string qName)
{
	chrDisableReloadToLocation = true;

	string sModel = "sold_fra_" + (rand(7)+1);
	ref chr = GetCharacter(NPC_GenerateCharacter("MA Soldier", sModel, "man", "man", 1, PIRATE, 0, false));
	chr.name = iTxt("BO_SoldierName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Soldier.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "soldier_common";
	ChangeCharacterAddressGroup(chr, "PortPax_town", "goto", "goto22");
	LAi_SetImmortal(chr, true);

	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

// Арест. Оружие отбирают на время сцены - это не конфискация из конца главы,
// а обычный обыск при задержании, всё вернётся в MysteriousArtefactReturnAll.
void MysteriousArtefactToPortPaxPrison()
{
	chrDisableReloadToLocation = false;
	LAi_Fade("", "");

	RemoveCharacterEquip(PChar, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(PChar, GUN_ITEM_TYPE);
	RemoveCharacterEquip(PChar, SPYGLASS_ITEM_TYPE);

	bDisableCharacterMenu = true;

	LAi_SetPlayerType(PChar);

	DoQuestReloadToLocation("PortPax_prison", "goto", "goto24", "");
	DoQuestFunctionDelay("MysteriousArtefactTalkInPortPaxPrison", 5.0);
}

void MysteriousArtefactTalkInPortPaxPrison(string _tmp)
{
	LAi_Fade("", "");
	ChangeCharacterAddress(CharacterFromID("MA Woman"), "none", "");

	// Gregg: начальника тюрьмы CSP заводит сама при загрузке локации, как
	// <колония>JailOff - см. CreateJail в LAi_utilites.c. Отдельно создавать не надо.
	ref chr = CharacterFromID("PortPaxJailOff");
	LAi_SetStayType(chr);
	ChangeCharacterAddressGroup(chr, "PortPax_prison", "goto", "goto23");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Soldier.c";
	chr.Dialog.CurrentNode = "3";

	SetDialogCharacter2Character(PChar, chr, 5.0, false);
}

void MysteriousArtefactInPrisonTime()
{
	ref chr = CharacterFromID("PortPaxJailOff");
	chr.Dialog.FileName = "Common_prison.c";
	chr.Dialog.CurrentNode = "First_officer";
	ChangeCharacterAddressGroup(chr, "PortPax_prison", "sit", "sit1");
	LAi_SetHuberType(chr);

	LAi_SetPlayerType(PChar);

	// Gregg: в К:ПС продолжение вешалось на квест-имя таблички (MysteriousArtefact_1..5)
	// и ловилось кейсом в свитче quests_reaction.c. В CSP для этого есть штатная пара
	// Run_Function + SetLaunchFrameRunFunctionParam - свитч трогать не нужно.
	SetLaunchFrameFormParam(iTxt("BO_ThreeDaysLater"), "Run_Function", 3.0, 1.0);
	SetLaunchFrameRunFunctionParam("MysteriousArtefactFromPrisonToPortPax", 1.0);
	LaunchFrameForm();
}

void MysteriousArtefactFromPrisonToPortPax()
{
	// Gregg: DoFunctionReloadToLocation вместо DoQuestReloadToLocation с квест-именем -
	// зовёт функцию по имени напрямую, кейс в свитче не нужен.
	DoFunctionReloadToLocation("PortPax_town", "reload", "reload1", "MysteriousArtefactFromPrisonToPortPaxEnd");
}

// Из тюрьмы выпускают под условие: выполнить поручение Стефана Ланвена на Мартинике.
void MysteriousArtefactFromPrisonToPortPaxEnd()
{
	BO_MARecord("14");
	PChar.MysteriousArtefact = "10";

	bDisableCharacterMenu = false;

	ref chr = GetCharacter(NPC_GenerateCharacter("Stefan Lanven", "usurer_8", "man", "man", 45, PIRATE, -1, false));
	SetSPECIAL(chr, 4, 9, 5, 8, 9, 3, 4);
	FantomMakeCoolFighter(chr, 45, 10, 10, BLADE_LONG, "pistol5", 100);
	SetShipSkill(chr, 100, 90, 50, 50, 60, 80, 50, 50, 90);
	SetSelfSkill(chr, 90, 85, 70, 75, 70);

	chr.rank = 45;
	chr.name = iTxt("BO_LanvenName");
	chr.lastname = iTxt("BO_LanvenLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Lanven.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_QuestMan";
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);

	// Gregg: в К:ПС Ланвен стоял в FortFrance_TownhallRoom - отдельной комнате
	// резиденции, которой у нас нет. Сажать его в CommonResidence_4 (комната слева)
	// нельзя: она общая на десять резиденций, и Ланвен появился бы во всех.
	// Поэтому ставим в саму резиденцию Фор-де-Франса, локатор goto6 в ней есть.
	// Сажать на sit1 тоже нельзя - это кресло губернатора.
	ChangeCharacterAddressGroup(chr, "FortFrance_townhall", "goto", "goto6");
	SetQuestMarkToCharacter(chr, true);

	WaitDate("", 0, 0, 3, 0, 0);
	SetCurrentTime(12, 10);
	RecalculateJumpTable();
}

// Ланвен идёт с нами: по дороге его встретит испанский патрульный отряд.
void MysteriousArtefactStefanLanvenIsPassenger()
{
	ref chr = CharacterFromID("Stefan Lanven");
	SetQuestMarkToCharacter(chr, false);
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "", 30.0);

	BO_MARecord("15");
	PChar.MysteriousArtefact = "11";

	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);

	PChar.quest.MysteriousArtefactPortPaxHouseBryanGeorge.over = "yes";

	ChangeCharacterAddress(CharacterFromID("Frederik Levinson"), "None", "");

	ref sld;
	int iN;
	// Gregg: GetRank(PChar, 10) из оригинала прибавила бы ранг ГГ дважды
	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	for (iN = 1; iN <= 3; iN++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("MA_Martinique_" + iN, "", "man", "man", iRank, SPAIN, -1, true));
		SetCaptanModelByEncType(sld, "war");
		FantomMakeCoolFighter(sld, 20, 80, 70, "blade012", "pistol3", 20);
		// Gregg: в К:ПС тут были ТРИ линкора - класс 2, 11 500 корпуса и 36 фунтов
		// каждый. На нашем семнадцатом ранге это тридцать четыре тысячи корпуса
		// второго класса за одну случайную встречу. Заменено на патрульный отряд
		// третьего класса: фрегат под флагом и два корвета сопровождения.
		// Уйти от боя всё равно можно - ветка MapEnter ниже никуда не делась.
		if (iN == 1) FantomMakeCoolSailor(sld, SHIP_FRIGATE, "", CANNON_TYPE_CULVERINE_LBS24, 70, 70, 70);
		else FantomMakeCoolSailor(sld, SHIP_CORVETTE, "", CANNON_TYPE_CULVERINE_LBS16, 65, 65, 65);
		SetGeraldDetails(sld, iN, rand(7), "", "spa_" + (rand(3)+2));
		sld.AlwaysSandbankManeuver = true;
		// Gregg: без AlwaysEnemy отношение к ГГ стирается при загрузке корабля в море -
		// оригинал ставил атрибут ровно по той же причине
		sld.AlwaysEnemy = true;
		sld.AnalizeShips = true;
		sld.DontRansackCaptain = true;
		LAi_group_MoveCharacter(sld, "EnemyFight");
		BO_SetSeaPerks(sld);
	}

	Group_FindOrCreateGroup("MA_Martinique");
	Group_AddCharacter("MA_Martinique", "MA_Martinique_1");
	Group_AddCharacter("MA_Martinique", "MA_Martinique_2");
	Group_AddCharacter("MA_Martinique", "MA_Martinique_3");
	Group_SetType("MA_Martinique", "pirate");
	Group_SetGroupCommander("MA_Martinique", "MA_Martinique_1");
	Group_SetAddress("MA_Martinique", "Martinique", "IslandShips1", "ship_6");
	Group_SetTaskAttack("MA_Martinique", PLAYER_GROUP);

	PChar.quest.MysteriousArtefactBattleMartiniqueSea.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactBattleMartiniqueSea.win_condition.l1.location = "Martinique";
	PChar.quest.MysteriousArtefactBattleMartiniqueSea.function = "MysteriousArtefactBattleMartiniqueSea";
}

// Gregg: из боя два выхода - перетопить всех либо уйти на глобальную карту.
// Оба ведут в одно и то же место, поэтому общую часть держим отдельной функцией.
void MysteriousArtefactBattleMartiniqueSea(string qName)
{
	Island_SetReloadEnableGlobal("Martinique", false);

	PChar.quest.MysteriousArtefactBattleMartiniqueSeaGood2.win_condition.l1 = "Group_Death";
	PChar.quest.MysteriousArtefactBattleMartiniqueSeaGood2.win_condition.l1.group = "MA_Martinique";
	PChar.quest.MysteriousArtefactBattleMartiniqueSeaGood2.function = "MysteriousArtefactBattleMartiniqueSeaGood2";

	PChar.quest.MysteriousArtefactBattleMartiniqueSeaGood.win_condition.l1 = "MapEnter";
	PChar.quest.MysteriousArtefactBattleMartiniqueSeaGood.function = "MysteriousArtefactBattleMartiniqueSeaGood";
}

void MysteriousArtefactMartiniqueSeaDone()
{
	Island_SetReloadEnableGlobal("Martinique", true);

	BO_MARecord("16");
	PChar.MysteriousArtefact = "12";

	Group_DeleteGroup("MA_Martinique");

	PChar.quest.MysteriousArtefactReturnToPortPax.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactReturnToPortPax.win_condition.l1.location = "PortPax_town";
	PChar.quest.MysteriousArtefactReturnToPortPax.function = "MysteriousArtefactReturnToPortPax";
}

void MysteriousArtefactBattleMartiniqueSeaGood2(string qName)
{
	PChar.quest.MysteriousArtefactBattleMartiniqueSeaGood.over = "yes";
	MysteriousArtefactMartiniqueSeaDone();
}

void MysteriousArtefactBattleMartiniqueSeaGood(string qName)
{
	PChar.quest.MysteriousArtefactBattleMartiniqueSeaGood2.over = "yes";
	MysteriousArtefactMartiniqueSeaDone();
}

// Дом Брайана теперь под охраной: без разрешения коменданта внутрь не попасть.
void MysteriousArtefactReturnToPortPax(string qName)
{
	ref chr = CharacterFromID("Stefan Lanven");
	RemovePassenger(PChar, chr);
	ChangeCharacterAddressGroup(chr, "PortPax_townhall", "goto", "goto2");
	LAi_SetStayType(chr);

	string sModel = "sold_fra_" + (rand(7)+1);
	chr = GetCharacter(NPC_GenerateCharacter("MA HouseSoldier", sModel, "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_SoldierName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Soldier.c";
	chr.Dialog.CurrentNode = "29";
	chr.greeting = "soldier_common";
	ChangeCharacterAddressGroup(chr, "PortPax_town", "reload", "houseSp1");
	LAi_SetGuardianType(chr);
	LAi_group_MoveCharacter(chr, "FRANCE_CITIZENS");
	LAi_SetImmortal(chr, true);

	PChar.quest.MysteriousArtefactDialogWithGuardianHouseGeorgeBryan.win_condition.l1 = "locator";
	PChar.quest.MysteriousArtefactDialogWithGuardianHouseGeorgeBryan.win_condition.l1.location = "PortPax_town";
	PChar.quest.MysteriousArtefactDialogWithGuardianHouseGeorgeBryan.win_condition.l1.locator_group = "reload";
	PChar.quest.MysteriousArtefactDialogWithGuardianHouseGeorgeBryan.win_condition.l1.locator = "houseSp1";
	PChar.quest.MysteriousArtefactDialogWithGuardianHouseGeorgeBryan.function = "MysteriousArtefactDialogWithGuardianHouseGeorgeBryan";
}

void MysteriousArtefactDialogWithGuardianHouseGeorgeBryan(string qName)
{
	SetDialogCharacter2Character(PChar, CharacterFromID("MA HouseSoldier"), 1.0, false);
}

// За разрешением - к коменданту в форт, а по дороге за воротами ждёт банда Тюльпана.
void MysteriousArtefactToPortPaxJailOff()
{
	BO_MARecord("17");

	ref chr = CharacterFromID("MA HouseSoldier");
	LAi_SetGuardianType(chr);
	LAi_group_MoveCharacter(chr, "FRANCE_CITIZENS");

	// Gregg: банда ждёт у выхода из города, а комендант сидит в тюрьме. Быстрый переход
	// (клавиша «Тюрьма») уводил игрока прямо к коменданту мимо выхода, и сцена с бандой
	// не срабатывала вовсе. На этот шаг гасим быстрый переход - снимаем в
	// MysteriousArtefactSeeBandaInPortPaxExit, когда встреча уже неизбежна.
	bDisableFastReload = true;

	BO_SetEncounters("PortPax_ExitTown", true);
	Locations[FindLocation("PortPax_ExitTown")].locators_radius.goto.goto2 = 3.0;
	PChar.quest.MysteriousArtefactSeeBandaInPortPaxExit.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactSeeBandaInPortPaxExit.win_condition.l1.location = "PortPax_ExitTown";
	PChar.quest.MysteriousArtefactSeeBandaInPortPaxExit.function = "MysteriousArtefactSeeBandaInPortPaxExit";
}

// Gregg: троица стоит и переговаривается, пока ГГ не подойдёт. Локаторы tulip1..3
// дописаны в модель, как и в К:ПС - тесный треугольник в паре метров друг от друга,
// шестью метрами выше по дороге от goto2, на котором сцена и срабатывает.
// Файл: RESOURCE\MODELS\Locations\Outside\Jungles\TownExitT\TownExitT_locators.gm
void MysteriousArtefactSeeBandaInPortPaxExit(string qName)
{
	bDisableOfficers = true;
	// Gregg: игрок дошёл до выхода, встреча состоится - быстрый переход возвращаем
	bDisableFastReload = false;

	// Gregg: GetRank(PChar, N) прибавила бы ранг ГГ дважды
	int iRank = sti(PChar.rank) + MOD_SKILL_ENEMY_RATE + 5;
	ref chr;
	int iN;
	string sSpot;

	Island_SetReloadEnableGlobal("Hispaniola2", false);

	for (iN = 1; iN <= 3; iN++)
	{
		string sModel = "officer_" + (16 + iN);
		chr = GetCharacter(NPC_GenerateCharacter("MA Banda " + iN, sModel, "man", "man", iRank, PIRATE, -1, true));
		chr.name = iTxt("BO_BandaName_" + iN);
		chr.lastname = "";
		chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Tulip.c";
		chr.Dialog.CurrentNode = "1";
		chr.greeting = "Gr_ArenaMember";
		LAi_SetImmortal(chr, true);
		LAi_SetActorType(chr);

		sSpot = "tulip" + iN;
		ChangeCharacterAddressGroup(chr, "PortPax_ExitTown", "goto", sSpot);
		// Gregg: SetFantomParamFromRank вместо голого LAi_NPC_Equip - он же зовёт
		// CalculateSkillsFromRank и заводит skill.*_exp. Без них начисление опыта
		// группе игрока падало в RUNTIME ERROR «missed attribute: <навык>_exp»
		// (RPGUtilite.c:2262), а бойцы выходили без навыков вовсе.
		SetFantomParamFromRank(chr, iRank, true);

		if (iN == 3)
		{
			GiveItem2Character(chr, "pistol3");
			EquipCharacterByItem(chr, "pistol3");
		}
	}

	LAi_ActorTurnToCharacter(CharacterFromID("MA Banda 1"), CharacterFromID("MA Banda 2"));
	LAi_ActorTurnToCharacter(CharacterFromID("MA Banda 2"), CharacterFromID("MA Banda 1"));
	LAi_ActorTurnToCharacter(CharacterFromID("MA Banda 3"), CharacterFromID("MA Banda 2"));

	PChar.quest.MysteriousArtefactDialogBandaInPortPaxExit.win_condition.l1 = "locator";
	PChar.quest.MysteriousArtefactDialogBandaInPortPaxExit.win_condition.l1.location = "PortPax_ExitTown";
	PChar.quest.MysteriousArtefactDialogBandaInPortPaxExit.win_condition.l1.locator_group = "goto";
	PChar.quest.MysteriousArtefactDialogBandaInPortPaxExit.win_condition.l1.locator = "goto2";
	PChar.quest.MysteriousArtefactDialogBandaInPortPaxExit.function = "MysteriousArtefactDialogBandaInPortPaxExit";
}

void MysteriousArtefactDialogBandaInPortPaxExit(string qName)
{
	LAi_SetActorType(PChar);
	LAi_ActorTurnToCharacter(PChar, CharacterFromID("MA Banda 3"));

	// Gregg: радиус возвращаем, иначе сцена будет запускаться при каждом проходе
	Locations[FindLocation("PortPax_ExitTown")].locators_radius.goto.goto2 = 0.5;

	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, -3, 1, -1, false);

	SetDialogCharacter2Character(PChar, CharacterFromID("MA Banda 1"), 0.1, true);
}

void MysteriousArtefactDialogWithBandaInPortPaxExit()
{
	LAi_Fade("", "");
	locCameraFollow();

	LAi_ActorFollow(CharacterFromID("MA Banda 2"), CharacterFromID("MA Banda 1"), "", 10.0);
	LAi_ActorFollow(CharacterFromID("MA Banda 3"), CharacterFromID("MA Banda 1"), "", 10.0);

	ref chr = CharacterFromID("MA Banda 1");
	chr.Dialog.CurrentNode = "6";
	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

// Gregg: плохая ветка - если ГГ помянет Тюльпана, третий бандит стреляет ему в спину.
// Оставлено как в оригинале: разговор с вооружёнными людьми, и в прохождении об этом
// прямо предупреждают. Кандидат на смягчение, если покажется слишком жёстким.
void MysteriousArtefactInPortPaxExitTownDead()
{
	ref chr = CharacterFromID("MA Banda 3");
	LAi_SetActorType(PChar);

	LAi_type_actor_Reset(chr);
	LAi_SetActorType(chr);

	DoQuestFunctionDelay("MysteriousArtefactInPortPaxExitTownDeadPre", 2.0);
}

void MysteriousArtefactInPortPaxExitTownDeadPre(string _tmp)
{
	ref chr = CharacterFromID("MA Banda 3");
	LAi_ActorTurnToCharacter(chr, PChar);
	LAi_ActorAnimation(chr, "Shot", "", -1);

	DoQuestFunctionDelay("MysteriousArtefactInPortPaxExitTownDeadEnd", 1.0);
}

void MysteriousArtefactInPortPaxExitTownDeadEnd(string _tmp)
{
	LAi_SetStayType(CharacterFromID("MA Banda 3"));
	LAi_SetPlayerType(PChar);
	LAi_KillCharacter(PChar);
}

// Клайв ведёт ГГ к главарю - в таверну Ла-Веги, тремя часами позже.
void MysteriousArtefactToLaVegaTavern()
{
	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Tulip", "officer_3", "man", "man", 1, HOLLAND, -1, false));
	chr.name = iTxt("BO_TulipName");
	chr.lastname = "";
	// Gregg: без AlwaysEnemy отношение стирается при загрузке корабля в море
	chr.AlwaysEnemy = true;
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Tulip.c";
	chr.Dialog.CurrentNode = "20";
	chr.greeting = "Gr_EvilPirate";
	LAi_SetSitType(chr);
	LAi_SetImmortal(chr, true);

	SetRandSPECIAL(chr);
	FantomMakeCoolFighter(chr, 30, 20, 20, BLADE_LONG, "pistol4", 100);
	SetShipSkill(chr, 100, 50, 65, 65, 70, 70, 60, 60, 40);
	SetSelfSkill(chr, 80, 75, 75, 60, 50);

	FantomMakeCoolSailor(chr, SHIP_GALEON_H, iTxt("BO_TulipShipName"), CANNON_TYPE_CULVERINE_LBS24, 70, 70, 70);
	chr.AlwaysSandbankManeuver = true;
	chr.AnalizeShips = true;
	chr.DontRansackCaptain = true;
	LAi_group_MoveCharacter(chr, "EnemyFight");
	BO_SetSeaPerks(chr);
	SetGeraldDetails(chr, 2, 3, "", "hol_" + (rand(3)+1));

	// Gregg: в К:ПС стол был sit_base4/sit_front4, а в таверне Ла-Веги (Tavern09)
	// сидячих мест только по три. Взята третья пара - она такая же «друг напротив друга»
	BO_FreeLocator("LaVega_tavern", "sit_base3", "Tulip");
	ChangeCharacterAddressGroup(chr, "LaVega_tavern", "sit", "sit_base3");

	WaitDate("", 0, 0, 0, 3, 5);
	RecalculateJumpTable();

	// Gregg: в К:ПС продолжение вешалось на квест-имя таблички (MysteriousArtefact_1..5)
	// и ловилось кейсом в свитче quests_reaction.c. В CSP для этого есть штатная пара
	// Run_Function + SetLaunchFrameRunFunctionParam - свитч трогать не нужно.
	SetLaunchFrameFormParam(iTxt("BO_ThreeHoursLaVega"), "Run_Function", 0.1, 3.0);
	SetLaunchFrameRunFunctionParam("MysteriousArtefactInLaVegaTavern", 1.0);
	LaunchFrameForm();
}

void MysteriousArtefactInLaVegaTavern()
{
	BO_FreeLocator("LaVega_tavern", "sit_front3", "");
	ChangeCharacterAddressGroup(PChar, "LaVega_tavern", "sit", "sit_front3");
	DoFunctionReloadToLocation("LaVega_tavern", "sit", "sit_front3", "MysteriousArtefactInLaVegaTavernTalkWithTulip");
}

void MysteriousArtefactInLaVegaTavernTalkWithTulip()
{
	// Gregg: пересаживаем обоих ЗАНОВО, уже после перехода. Первый раз адреса
	// раздаются ещё в Порт-о-Принсе, когда таверна Ла-Веги не загружена: освобождать
	// там нечего, а на загрузке генератор заводит своих посетителей и может занять
	// те же места. Повтор идемпотентен - BO_FreeLocator теперь видит живых NPC
	// и уводит их мгновенно.
	ref chr = CharacterFromID("Tulip");
	BO_FreeLocator("LaVega_tavern", "sit_base3", "Tulip");
	ChangeCharacterAddressGroup(chr, "LaVega_tavern", "sit", "sit_base3");
	// Gregg: стул напротив - тоже. ГГ уже сидит на нём после перехода, но генератор
	// мог посадить туда своего; BO_FreeLocator главного героя пропускает и уведёт
	// именно постороннего
	BO_FreeLocator("LaVega_tavern", "sit_front3", "");
	// Gregg: и самого ГГ переставляем заново - на загрузке его могло отбросить
	// от занятого стула, хотя адрес остался прежним
	ChangeCharacterAddressGroup(PChar, "LaVega_tavern", "sit", "sit_front3");

	LAi_SetSitType(PChar);
	LAi_SetActorType(chr);
	LAi_ActorSetSitMode(chr);
	LAi_ActorDialogNow(chr, PChar, "", -1);
}

// Разговор окончен, Клайв ждёт у выхода.
void MysteriousArtefactInLaVegaTavernTalkWithClive()
{
	ref chr = CharacterFromID("MA Banda 1");
	chr.Dialog.CurrentNode = "44";

	LAi_SetPlayerType(PChar);
	ChangeCharacterAddressGroup(chr, "LaVega_tavern", "reload", "reload1_back");
	ChangeCharacterAddressGroup(PChar, "LaVega_tavern", "goto", "goto1");

	SetDialogCharacter2Character(PChar, chr, 5.0, true);
}

// Банда идёт с ГГ обратно к воротам Порт-о-Принса - и там нападает.
void MysteriousArtefactToPortPaxExitTown()
{
	ChangeCharacterAddress(CharacterFromID("Tulip"), "none", "");
	LAi_SetPlayerType(PChar);

	BO_MARecord("18");

	ref chr;
	int iN;
	string sId;
	string sGun;
	for (iN = 1; iN <= 3; iN++)
	{
		sId = "MA Banda " + iN;
		chr = CharacterFromID(sId);
		LAi_type_actor_Reset(chr);
		LAi_SetStayType(chr);
		LAi_SetActorType(chr);
		LAi_ActorFollowEverywhere(chr, "", -1);
		sGun = "pistol4";
		if (iN == 1) sGun = "pistol3";
		FantomMakeCoolFighter(chr, 30, 20, 20, BLADE_NORMAL, sGun, 100);
		LAi_SetHP(chr, 500.0, 500.0);
	}

	PChar.quest.MysteriousArtefactToPortPaxExitTownBattleWithTulipBanda.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactToPortPaxExitTownBattleWithTulipBanda.win_condition.l1.location = "PortPax_ExitTown";
	PChar.quest.MysteriousArtefactToPortPaxExitTownBattleWithTulipBanda.function = "MysteriousArtefactToPortPaxExitTownBattleWithTulipBanda";

	// Gregg: двое отстают на выходе из таверны - им заново выдаётся приказ следовать
	PChar.quest.MysteriousArtefactToPortPaxExitTownAddBanda.win_condition.l1 = "ExitFromLocation";
	PChar.quest.MysteriousArtefactToPortPaxExitTownAddBanda.win_condition.l1.location = "LaVega_tavern";
	PChar.quest.MysteriousArtefactToPortPaxExitTownAddBanda.function = "MysteriousArtefactToPortPaxExitTownAddBanda";
}

void MysteriousArtefactToPortPaxExitTownAddBanda(string qName)
{
	ref chr = CharacterFromID("MA Banda 2");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	chr = CharacterFromID("MA Banda 3");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);
}

// У ворот банда решает не доводить дело до коменданта - а комендант уже идёт навстречу.
void MysteriousArtefactToPortPaxExitTownBattleWithTulipBanda(string qName)
{
	bDisableOfficers = false;
	chrDisableReloadToLocation = true;

	ref chr;
	int iN;
	string sType;
	string sSpot;

	for (iN = 1; iN <= 3; iN++)
	{
		LAi_type_actor_Reset(CharacterFromID("MA Banda " + iN));
	}

	// Мушкетёры коменданта - трое у ворот, двое по сторонам дороги.
	for (iN = 1; iN <= 5; iN++)
	{
		sType = "fra_rifle_" + (rand(2)+1);
		chr = GetCharacter(NPC_GenerateMusketeer("MA Musket " + iN, sType, "man", 20, PIRATE, 0, false));
		SetFantomParamFromRank(chr, 15 + MOD_SKILL_ENEMY_RATE, true);
		LAi_SetStayType(chr);

		sSpot = "officers";
		if (iN == 4) sSpot = "goto";
		if (iN == 5) sSpot = "goto";
		if (iN == 4) ChangeCharacterAddressGroup(chr, "PortPax_ExitTown", "goto", "goto1");
		else
		{
			if (iN == 5) ChangeCharacterAddressGroup(chr, "PortPax_ExitTown", "goto", "goto6");
			else ChangeCharacterAddressGroup(chr, "PortPax_ExitTown", "officers", "reload3_" + iN);
		}
	}

	// Gregg: начальника тюрьмы CSP заводит сама при загрузке тюрьмы. Если игрок туда
	// почему-то не попал, поднимаем его тем же способом, что и штатный генератор.
	ref JailOff;
	if (GetCharacterIndex("PortPaxJailOff") != -1)
	{
		JailOff = CharacterFromID("PortPaxJailOff");
	}
	else
	{
		int iRank = sti(PChar.rank) + rand(4);
		if (iRank < 10) iRank = 10;
		if (iRank > 20) iRank = 20;
		string sOffModel = "off_fra_" + (rand(1)+1);
		JailOff = GetCharacter(NPC_GenerateCharacter("PortPaxJailOff", sOffModel, "man", "man", iRank + 5, FRANCE, 2, true));
		JailOff.City = "PortPax";
		JailOff.CityType = "soldier";
		JailOff.standUp = true;
		JailOff.Dialog.FileName = "Common_prison.c";
		JailOff.Dialog.CurrentNode = "First_officer";
		JailOff.greeting = "soldier_common";
		FantomMakeCoolFighter(JailOff, iRank + 5, 70, 50, BLADE_LONG, "pistol3", 30);
		LAi_SetLoginTime(JailOff, 0.0, 24.0);
		LAi_SetHuberType(JailOff);
		LAi_group_MoveCharacter(JailOff, "FRANCE_CITIZENS");
		SetFoodToCharacter(JailOff, 5, 40);
	}

	LAi_SetStayType(JailOff);
	ChangeCharacterAddressGroup(JailOff, "PortPax_ExitTown", "reload", "reload3");
	LAi_SetImmortal(JailOff, true);

	// Gregg: в К:ПС третий бандит и ГГ вставали на Duelist_2 и Mercenary_2 - локаторы
	// той модели. Новых не заводил: сцена идёт на том же отрезке дороги, где banda уже
	// стояла, и tulip1/tulip3 к этому моменту свободны.
	ChangeCharacterAddressGroup(CharacterFromID("MA Banda 1"), "PortPax_ExitTown", "goto", "goto7");
	ChangeCharacterAddressGroup(CharacterFromID("MA Banda 2"), "PortPax_ExitTown", "goto", "tulip2");
	ChangeCharacterAddressGroup(CharacterFromID("MA Banda 3"), "PortPax_ExitTown", "goto", "tulip3");
	ChangeCharacterAddressGroup(PChar, "PortPax_ExitTown", "goto", "tulip1");
	for (iN = 1; iN <= 3; iN++)
	{
		SetFoodToCharacter(CharacterFromID("MA Banda " + iN), 5, 30);
	}

	chr = CharacterFromID("MA Banda 1");
	chr.Dialog.CurrentNode = "58";
	SetDialogCharacter2Character(PChar, chr, 0.1, true);
}

void MysteriousArtefactToPortPaxExitTownBattleWithTulipBandaBegin()
{
	LAi_SetPlayerType(PChar);

	int iN;
	ref chr;
	for (iN = 1; iN <= 3; iN++)
	{
		chr = CharacterFromID("MA Banda " + iN);
		LAi_SetImmortal(chr, false);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	for (iN = 1; iN <= 5; iN++)
	{
		chr = CharacterFromID("MA Musket " + iN);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	}

	chr = CharacterFromID("PortPaxJailOff");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("MysteriousArtefactToPortPaxExitTownBattleWithTulipBandaWinner", "MysteriousArtefactToPortPaxExitTownBattleWithTulipBandaWinner", "MA Banda ", 3);
}

void MysteriousArtefactToPortPaxExitTownBattleWithTulipBandaWinner(string qName)
{
	ref JailOff = CharacterFromID("PortPaxJailOff");
	JailOff.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Soldier.c";
	JailOff.Dialog.CurrentNode = "34";
	SetDialogCharacter2Character(PChar, JailOff, 25.0, true);
}

// Комендант благодарен и обещает разрешение. Солдаты уходят.
void MysteriousArtefactToPortPaxExitTownBattleWithTulipBandaEnd()
{
	BO_SetEncounters("PortPax_ExitTown", false);
	chrDisableReloadToLocation = false;
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("PortPaxJailOff");
	LAi_group_MoveCharacter(chr, "FRANCE_CITIZENS");
	chr.Dialog.FileName = "Common_prison.c";
	chr.Dialog.CurrentNode = "First_officer";
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload3", "none", "", "", "", 30.0);

	int iN;
	string sId;
	for (iN = 1; iN <= 5; iN++)
	{
		sId = "MA Musket " + iN;
		if (GetCharacterIndex(sId) != -1)
		{
			chr = CharacterFromID(sId);
			LAi_SetActorType(chr);
			LAi_ActorRunToLocation(chr, "reload", "reload3", "none", "", "", "", 30.0);
		}
	}

	BO_MARecord("19");

	chr = CharacterFromID("MA HouseSoldier");
	chr.Dialog.CurrentNode = "51";
	LAi_SetStayType(chr);

	chr = CharacterFromID("Stefan Lanven");
	chr.Dialog.CurrentNode = "28";
	ChangeCharacterAddressGroup(chr, "FortFrance_townhall", "goto", "goto6");

	Island_SetReloadEnableGlobal("Hispaniola2", true);

	PChar.quest.MysteriousArtefactJailOffToPrison.win_condition.l1 = "ExitFromLocation";
	PChar.quest.MysteriousArtefactJailOffToPrison.win_condition.l1.location = PChar.location;
	PChar.quest.MysteriousArtefactJailOffToPrison.function = "MysteriousArtefactJailOffToPrison";
}

void MysteriousArtefactJailOffToPrison(string qName)
{
	ref chr = CharacterFromID("PortPaxJailOff");
	LAi_SetImmortal(chr, false);
	LAi_SetHuberType(chr);
	ChangeCharacterAddressGroup(chr, "PortPax_prison", "sit", "sit1");
	Island_SetReloadEnableGlobal("Hispaniola2", true);
}

// Разрешение получено - охранник пропускает в дом.
void MysteriousArtefactHouseGeorgeBryanOpen()
{
	ref chr = CharacterFromID("MA HouseSoldier");
	LAi_SetActorType(chr);
	LAi_group_MoveCharacter(chr, "FRANCE_CITIZENS");

	BO_LockBryanDoor(false);
	// Gregg: дверь наверх снова запираем - наверху делать больше нечего
	Locations[FindLocation("HouseGeorgeBryan")].reload.l2.disable = 1;

	PChar.quest.MysteriousArtefactToHouseGeorgeBryanOpen.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactToHouseGeorgeBryanOpen.win_condition.l1.location = "HouseGeorgeBryan";
	PChar.quest.MysteriousArtefactToHouseGeorgeBryanOpen.function = "MysteriousArtefactToHouseGeorgeBryanOpen";
}

// Сундук наконец открывается: полмиллиона, монета с шифром и книга.
void MysteriousArtefactToHouseGeorgeBryanOpen(string qName)
{
	chrDisableReloadToLocation = true;

	ref loc = LocFromID("HouseGeorgeBryan");
	// Gregg: отпираем сундук. Ключ забираем руками: проверку .key itemLogic.c делает
	// только для «private»-локаторов, а box1 к ним не относится. Раньше игрока сюда
	// не пускают - дверь дома заперта с самой сцены с женщиной, - так что окна, где
	// ключ на руках, а сундук уже доступен, не возникает.
	if (CheckAttribute(&Locations[FindLocation("HouseGeorgeBryan")], "box1.QuestClosed"))
	{
		DeleteAttribute(&Locations[FindLocation("HouseGeorgeBryan")], "box1.QuestClosed");
	}
	TakeNItems(PChar, "HouseGeorgeBryan_Key", -1);
	loc.box1.items.MysteriousArtefact_Coin = 1;
	loc.box1.items.MysteriousArtefact_Book = 1;
	loc.box1.money = 500000;
	loc.box1 = Items_MakeTime(0, 0, 1, 2003);

	PChar.quest.MysteriousArtefactFindBookAndCoin.win_condition.l1 = "item";
	PChar.quest.MysteriousArtefactFindBookAndCoin.win_condition.l1.item = "MysteriousArtefact_Coin";
	PChar.quest.MysteriousArtefactFindBookAndCoin.win_condition.l2 = "item";
	PChar.quest.MysteriousArtefactFindBookAndCoin.win_condition.l2.item = "MysteriousArtefact_Book";
	PChar.quest.MysteriousArtefactFindBookAndCoin.function = "MysteriousArtefactFindBookAndCoin";
}

void MysteriousArtefactFindBookAndCoin(string qName)
{
	ClearQuestBoxStay("HouseGeorgeBryan");
	chrDisableReloadToLocation = false;

	BO_MARecord("20");
	PChar.MysteriousArtefact = "13";

	// Gregg: дом отработал, дверь возвращаем городу вместе со снятием disable
	BO_OpenBryanHouse(false);
}

void MysteriousArtefactTalkWithMayor()
{
	BO_MARecord("21");
	PChar.MysteriousArtefact = "14";
}

void MysteriousArtefactNeedOpenBook()
{
	BO_MARecord("22");
	PChar.MysteriousArtefact = "15";
}

// Шифр с монеты подошёл - но внутри пусто.
// Gregg: параметр обязателен - зовут через DoQuestFunctionDelay из осмотра монеты
// в инвентаре, а он передаёт имя квеста строкой (quests.c:917, call sFunction(stmp)).
// Без параметра игра валилась в «stack error», и книга не открывалась.
void MysteriousArtefactOpenBook(string _tmp)
{
	BO_MARecord("23");
	PChar.MysteriousArtefact = "16";

	// Gregg: подменяем описание предмета на «открытую» версию
	Items[FindItem("MysteriousArtefact_Book")].describe = "itmdescr_MysteriousArtefact_Book1";
}

void MysteriousArtefactTimerReadBook()
{
	BO_MARecord("24");
	PChar.MysteriousArtefact = "17";

	// Gregg: у К:ПС тут SetQuestFunction с кодом "efl", у нас условие пишется явно
	PChar.quest._MysteriousArtefactReadBook.win_condition.l1 = "ExitFromLocation";
	PChar.quest._MysteriousArtefactReadBook.win_condition.l1.location = PChar.location;
	PChar.quest._MysteriousArtefactReadBook.function = "_MysteriousArtefactReadBook";
}

void _MysteriousArtefactReadBook(string qName)
{
	SetTimerFunction("MysteriousArtefactReadBook", 0, 0, 4);
}

void MysteriousArtefactReadBook(string qName)
{
	ref chr = CharacterFromID("Stefan Lanven");
	chr.Dialog.CurrentNode = "37";
}

// Ланвен отправляет в залив Гибели на Мейне.
void MysteriousArtefactToDeadShore()
{
	BO_MARecord("25");
	PChar.MysteriousArtefact = "18";

	BO_SetEncounters("Shore_ship2", true);
	PChar.quest.MysteriousArtefactInDeadShore.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInDeadShore.win_condition.l1.location = "Shore_ship2";
	PChar.quest.MysteriousArtefactInDeadShore.function = "MysteriousArtefactInDeadShore";
}

// В заливе - четверо испанских солдат и их командир Лукаш де Грасси.
void MysteriousArtefactInDeadShore(string qName)
{
	ref chr = CharacterFromID("Stefan Lanven");
	chr.LifeDay = 0;
	ChangeCharacterAddress(chr, "None", "");

	chrDisableReloadToLocation = true;

	int iN;
	string sModel;
	for (iN = 1; iN <= 4; iN++)
	{
		sModel = "sold_spa_" + (rand(7)+1);
		chr = GetCharacter(NPC_GenerateCharacter("MA DeadShore Sold " + iN, sModel, "man", "man", 1, SPAIN, 0, false));
		chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Grassi.c";
		chr.Dialog.CurrentNode = "First time";
		chr.greeting = "Gr_AllowToDump";
		LAi_SetCitizenType(chr);
		LAi_SetImmortal(chr, true);
		SetFoodToCharacter(chr, 5, 30);
		ChangeCharacterAddressGroup(chr, "Shore_ship2", "goto", "goto" + iN);
	}

	sModel = "off_spa_" + (rand(1)+1);
	chr = GetCharacter(NPC_GenerateCharacter("Lukash de Grassi", sModel, "man", "man", 20, SPAIN, -1, false));
	chr.name = iTxt("BO_GrassiName");
	chr.lastname = iTxt("BO_GrassiLastName");
	chr.greeting = "Gr_AllowToDump";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Grassi.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetImmortal(chr, true);
	LAi_SetCitizenType(chr);
	ChangeCharacterAddressGroup(chr, "Shore_ship2", "goto", "goto5");
	SetFoodToCharacter(chr, 5, 30);

	SetRandSPECIAL(chr);
	FantomMakeCoolFighter(chr, 30, 20, 20, BLADE_LONG, "pistol3", 100);
	SetShipSkill(chr, 50, 50, 55, 45, 60, 60, 30, 50, 50);
	SetSelfSkill(chr, 70, 65, 65, 70, 30);

	SetDialogCharacter2Character(PChar, chr, 25.0, false);
}

// Испанцы просятся на борт - им нужно к мысу Камагуэй.
void MysteriousArtefactToSantiagoShore()
{
	chrDisableReloadToLocation = false;

	// Gregg: если ГГ пришёл сюда своим кораблём, солдаты уходят к морю, если пешком -
	// к обычному выходу с берега
	string sLocator = "sea";
	if (GetCharacterShipLocation(PChar) != "Shore_ship2") sLocator = "reload1";

	ref chr;
	int iN;
	for (iN = 1; iN <= 4; iN++)
	{
		chr = CharacterFromID("MA DeadShore Sold " + iN);
		LAi_SetActorType(chr);
		LAi_ActorRunToLocation(chr, "reload", sLocator, "none", "", "", "", 30.0);
	}

	chr = CharacterFromID("Lukash de Grassi");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", sLocator, "none", "", "", "", 30.0);

	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);
	AddCharacterCrew(PChar, 4);

	LAi_SetPlayerType(PChar);

	BO_MARecord("26");
	PChar.MysteriousArtefact = "19";

	BO_SetEncounters("Shore_ship2", false);
	BO_SetEncounters("Shore11", true);
	PChar.quest.MysteriousArtefactInSantiagoShore.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInSantiagoShore.win_condition.l1.location = "Shore11";
	PChar.quest.MysteriousArtefactInSantiagoShore.function = "MysteriousArtefactInSantiagoShore";
}

void MysteriousArtefactInSantiagoShore(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Lukash de Grassi");
	chr.Dialog.CurrentNode = "30";
	ChangeCharacterAddressGroup(chr, "Shore11", "goto", "goto9");

	SetDialogCharacter2Character(PChar, chr, 25.0, false);
}

// Ночь на берегу, штурм назначен на утро.
void MysteriousArtefactInSantiagoShoreTime()
{
	chrDisableReloadToLocation = false;

	BO_MARecord("27");
	PChar.MysteriousArtefact = "20";

	BO_SetEncounters("Cuba_Jungle_01", true);
	Locations[FindLocation("Shore11")].reload.l2.disable = 1;

	// Gregg: раньше здесь дверь из джунглей перенацеливалась на клон-город. Убрано:
	// восстановления не было вовсе, и Cuba_Jungle_01 навсегда оставались завязаны
	// на клон. В клон игрока заводит телепорт после разговора с де Грасси -
	// см. MysteriousArtefactPreBattleInSantiagoExitTown.
	// Сами клоны заводятся штатно, в хвосте InitLocations (DefineMASantiago*), а здесь
	// им ставится состояние на время штурма: энкаунтеры, noFight, быстрый переход.
	BO_FixSantiagoCloneDoors();

	// Gregg: вся сцена собирается в ОДНОМ тике - кадр между операторами не рисуется,
	// поэтому ни смены суток, ни появления отряда игрок не увидит. Табличку поднимаем
	// ПЕРВОЙ, чтобы дальше всё шло при замороженных слоях (interface.c:1273 ->
	// seadogs.c:702). Run_Function здесь не годится и был причиной бага:
	// SetLaunchFrameRunFunctionParam планирует вызов в IDoExit, то есть уже ПОСЛЕ того,
	// как табличка погасла (boal_frame.c), да ещё плюс своя задержка - смена суток
	// происходила на глазах. Параметр _delay на прямом вызове LaunchFrameForm мёртв:
	// FrameDelay читается только в кейсе "LaunchFrameForm" квестового свитча.
	SetLaunchFrameFormParam(iTxt("BO_Morning"), "", 0.1, 4.0);
	LaunchFrameForm();

	WaitDate("", 0, 0, 1, 0, 0);
	SetCurrentTime(6, 10);
	RecalculateJumpTable();
	RefreshWeather();
	RefreshLandTime();

	MysteriousArtefactInSantiagoShoreReload("");
}



// Отряд для штурма собирается из команды ГГ: чем больше матросов, тем больше людей.
void MysteriousArtefactInSantiagoShoreReload(string _tmp)
{
	chrDisableReloadToLocation = true;

	int iQty = GetCrewQuantity(PChar) / 50;
	if (iQty < 10) iQty = 10;
	if (iQty > 23) iQty = 23;

	PChar.MysteriousArtefact.SantiagoAttackers = iQty;

	ref chr;
	int iN;
	// Gregg: GetRank(PChar, 5) прибавила бы ранг ГГ дважды
	int iRank = sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE;
	for (iN = 1; iN <= iQty; iN++)
	{
		string sModel = "officer_" + iN;
		chr = GetCharacter(NPC_GenerateCharacter("MA Attack " + iN, sModel, "man", "man", iRank, PIRATE, -1, false));
		// Gregg: отряд встаёт на item1..item23, как в оригинале. На нашем Shore11
		// в модели были только item10..item14 - недостающие дописаны двумя рядами
		// вдоль берега, вглубь от воды. Файл:
		// RESOURCE\MODELS\Locations\Outside\Shores\Shore02\Shore02_locators.gm
		ChangeCharacterAddressGroup(chr, "Shore11", "item", "item" + iN);
		SetFoodToCharacter(chr, 5, 30);
		// Gregg: SetFantomParamFromRank вместо голого LAi_NPC_Equip - он же зовёт
		// CalculateSkillsFromRank и заводит skill.*_exp. Без них начисление опыта
		// группе игрока падало в RUNTIME ERROR «missed attribute: <навык>_exp»
		// (RPGUtilite.c:2262), а бойцы выходили без навыков вовсе.
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetImmortal(chr, true);

		LAi_SetActorType(chr);
		LAi_ActorFollowEverywhere(chr, "", -1);
	}

	chr = CharacterFromID("Lukash de Grassi");
	chr.Dialog.CurrentNode = "35";
	SetDialogCharacter2Character(PChar, chr, 20.0, false);
}

void MysteriousArtefactToSantiagoJungle()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Lukash de Grassi");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	LAi_SetPlayerType(PChar);

	PChar.MysteriousArtefact = "21";
	chr = CharacterFromID("MA HouseSoldier");
	chr.LifeDay = 0;
	ChangeCharacterAddress(chr, "None", "");

	// Gregg: дверь дома Брайана в оригинале возвращали городу здесь. У нас это уже
	// сделано раньше, в MysteriousArtefactFindBookAndCoin - дом отработал ещё тогда.

	PChar.quest.MysteriousArtefactInSantiagoJungle.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInSantiagoJungle.win_condition.l1.location = "Cuba_Jungle_01";
	PChar.quest.MysteriousArtefactInSantiagoJungle.function = "MysteriousArtefactInSantiagoJungle";
	DoFunctionReloadToLocation("Cuba_Jungle_01", "reload", "reload2_back", "");
}

// В джунглях ждёт отряд корсаров Мэтью Грехома.
void MysteriousArtefactInSantiagoJungle(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr;
	int iN;
	for (iN = 1; iN <= 10; iN++)
	{
		string sModel = "pirate_" + iN;
		chr = GetCharacter(NPC_GenerateCharacter("MA Pirates " + iN, sModel, "man", "man", 1, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "Cuba_Jungle_01", "reload", "reload1");
		LAi_SetImmortal(chr, true);

		if (iN == 1)
		{
			LAi_SetStayType(chr);
			chr.name = iTxt("BO_GrehomName");
			chr.lastname = iTxt("BO_GrehomLastName");
			chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Pirates.c";
			chr.Dialog.CurrentNode = "1";
			chr.greeting = "Gr_EvilPirate";
			ChangeCharacterAddressGroup(chr, "Cuba_Jungle_01", "goto", "goto1");
		}
		else
		{
			LAi_SetActorType(chr);
			LAi_ActorFollow(chr, CharacterFromID("MA Pirates 1"), "", -1);
		}
	}
}

void MysteriousArtefactInSantiagoJungleDialog()
{
	ref chr;
	int iN;
	for (iN = 1; iN <= 10; iN++)
	{
		chr = CharacterFromID("MA Pirates " + iN);
		LAi_SetActorType(chr);
		LAi_ActorRunToLocation(chr, "reload", "reload2_back", "none", "", "", "", 30.0);
	}

	chr = CharacterFromID("Lukash de Grassi");
	chr.Dialog.CurrentNode = "46";
	SetDialogCharacter2Character(PChar, chr, 20.0, false);
}

// LEO: адресная диагностика перехода в клон Сантьяго, без изменения очереди и состояния квеста.
void BO_TraceSantiagoReload(string sPhase)
{
	string sStage = "_";
	string sPlayerLocation = "_";
	string sLoadedLocation = "_";
	string sPendingFunction = "_";
	if (CheckAttribute(PChar, "MysteriousArtefact")) sStage = PChar.MysteriousArtefact;
	if (CheckAttribute(PChar, "location")) sPlayerLocation = PChar.location;
	if (CheckAttribute(PChar, "GenQuest.CallFunctionParam")) sPendingFunction = PChar.GenQuest.CallFunctionParam;
	int iLoaded = FindLoadedLocation();
	if (iLoaded >= 0)
	{
		if (CheckAttribute(&Locations[iLoaded], "id")) sLoadedLocation = Locations[iLoaded].id;
	}
	Trace("LEO: [BO_SANTIAGO] phase=" + sPhase + " stage=" + sStage + " player_location=" + sPlayerLocation + " loaded_location=" + sLoadedLocation + " target=MA_Santiago_ExitTown/reload/reload1_back pending_reload=" + sQuestNameAfterReload + " pending_function=" + sPendingFunction);
}

void MysteriousArtefactPreBattleInSantiagoExitTown()
{
	BO_TraceSantiagoReload("request");
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Lukash de Grassi");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	LAi_SetPlayerType(PChar);

	// Gregg: в клон-город игрока надо ЗАВЕСТИ, а не ждать, пока он придёт туда сам.
	// Дверь из джунглей ведёт в настоящий Santiago_ExitTown; перенацеливать её не станем -
	// промахнёшься с восстановлением, и настоящий город останется недоступен навсегда.
	// Телепорт сразу после диалога проще и ничего в мире не трогает. Дальше клоны
	// сцеплены между собой сами, см. DefineMASantiago* в Locations\init\Santiago.c.
	if (DoFunctionReloadToLocation("MA_Santiago_ExitTown", "reload", "reload1_back", "MysteriousArtefactBattleInExitSantiago"))
	{
		BO_TraceSantiagoReload("accepted");
	}
	else
	{
		BO_TraceSantiagoReload("rejected");
	}
}

// Первый бой - у городских ворот. Двадцать защитников против отряда ГГ.
void MysteriousArtefactBattleInExitSantiago()
{
	BO_TraceSantiagoReload("arrived");
	chrDisableReloadToLocation = true;
	// Gregg: вымести горожан и стражу, что нагенерил CreateCitizens при загрузке
	BO_ClearCitizens("MA_Santiago_ExitTown", "");

	int iQty = sti(PChar.MysteriousArtefact.SantiagoAttackers);
	ref chr;
	int iN;
	// Gregg: GetRank(PChar, 5) прибавила бы ранг ГГ дважды
	int iRank = sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE;

	for (iN = 1; iN <= iQty; iN++)
	{
		chr = CharacterFromID("MA Attack " + iN);
		ChangeCharacterAddressGroup(chr, "MA_Santiago_ExitTown", "goto", "goto3");
		chr.LifeDay = 0;
		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	}

	for (iN = 1; iN <= 20; iN++)
	{
		string sModel = "sold_spa_" + (rand(7)+1);
		chr = GetCharacter(NPC_GenerateCharacter("MA Defender " + iN, sModel, "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "MA_Santiago_ExitTown", "reload", "reload3");
		SetFoodToCharacter(chr, 5, 50);
		// Gregg: SetFantomParamFromRank вместо голого LAi_NPC_Equip - он же зовёт
		// CalculateSkillsFromRank и заводит skill.*_exp. Без них начисление опыта
		// группе игрока падало в RUNTIME ERROR «missed attribute: <навык>_exp»
		// (RPGUtilite.c:2262), а бойцы выходили без навыков вовсе.
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	chr = CharacterFromID("Lukash de Grassi");
	ChangeCharacterAddressGroup(chr, "MA_Santiago_ExitTown", "goto", "goto1");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("MysteriousArtefactBattleInExitSantiagoWinner", "MysteriousArtefactBattleInExitSantiagoWinner", "MA Defender ", 20);
}

// Gregg: потери отряда переносятся в следующий бой - сколько людей полегло у ворот,
// столько и не выйдет на улицы города.
void MysteriousArtefactBattleInExitSantiagoWinner(string qName)
{
	chrDisableReloadToLocation = false;

	PlayStereoSound("new_round");
	Log_Info(iTxt("BO_EnemyCrushed"));

	int iDead = 0;
	int iBase = sti(PChar.MysteriousArtefact.SantiagoAttackers);
	int iN;
	for (iN = 1; iN <= iBase; iN++)
	{
		if (GetCharacterIndex("MA Attack " + iN) == -1) iDead++;
	}

	Locations[FindLocation("MA_Santiago_ExitTown")].reload.l1.disable = 0;
	PChar.MysteriousArtefact.SantiagoAttackers = iBase - iDead;

	PChar.quest.MysteriousArtefactPreBattleInSantiago.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactPreBattleInSantiago.win_condition.l1.location = "MA_Santiago_town";
	PChar.quest.MysteriousArtefactPreBattleInSantiago.function = "MysteriousArtefactPreBattleInSantiago";
}

// Второй бой - на улицах города. Отряд подходит поредевшим после ворот.
void MysteriousArtefactPreBattleInSantiago(string qName)
{
	BO_ClearCitizens("MA_Santiago_town", "");
	chrDisableReloadToLocation = true;

	int iQty = sti(PChar.MysteriousArtefact.SantiagoAttackers);
	ref chr;
	int iN;
	string sModel;
	int iRank = sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE;

	for (iN = 1; iN <= iQty; iN++)
	{
		sModel = "officer_" + iN;
		chr = GetCharacter(NPC_GenerateCharacter("MA InTown Attacker " + iN, sModel, "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "MA_Santiago_town", "reload", "reload3_back");
		// Gregg: SetFantomParamFromRank вместо голого LAi_NPC_Equip - он же зовёт
		// CalculateSkillsFromRank и заводит skill.*_exp. Без них начисление опыта
		// группе игрока падало в RUNTIME ERROR «missed attribute: <навык>_exp»
		// (RPGUtilite.c:2262), а бойцы выходили без навыков вовсе.
		SetFantomParamFromRank(chr, iRank, true);
		SetFoodToCharacter(chr, 5, 50);
		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	}

	for (iN = 1; iN <= 15; iN++)
	{
		sModel = "sold_spa_" + (rand(7)+1);
		chr = GetCharacter(NPC_GenerateCharacter("MA InTown Defender " + iN, sModel, "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "MA_Santiago_town", "reload", "reload1_back");
		// Gregg: SetFantomParamFromRank вместо голого LAi_NPC_Equip - он же зовёт
		// CalculateSkillsFromRank и заводит skill.*_exp. Без них начисление опыта
		// группе игрока падало в RUNTIME ERROR «missed attribute: <навык>_exp»
		// (RPGUtilite.c:2262), а бойцы выходили без навыков вовсе.
		SetFantomParamFromRank(chr, iRank, true);
		SetFoodToCharacter(chr, 5, 50);
		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	chr = CharacterFromID("Lukash de Grassi");
	ChangeCharacterAddressGroup(chr, "MA_Santiago_town", "goto", "goto10");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("MysteriousArtefactBattleInSantiagoWinner", "MysteriousArtefactBattleInSantiagoWinner", "MA InTown Defender ", 15);
}

void MysteriousArtefactBattleInSantiagoWinner(string qName)
{
	ref chr = CharacterFromID("Lukash de Grassi");
	LAi_SetFightMode(PChar, false);
	chr.Dialog.CurrentNode = "50";
	SetDialogCharacter2Character(PChar, chr, 25.0, false);

	PlayStereoSound("new_round");
	Log_Info(iTxt("BO_EnemyCrushed"));
	Locations[FindLocation("MA_Santiago_ExitTown")].reload.l1.disable = 0;
}

void MysteriousArtefactToSantiagoTownhall()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Lukash de Grassi");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload5_back", "none", "", "", "", 30.0);

	Locations[FindLocation("MA_Santiago_town")].reload.l3.disable = 0;

	int iDead = 0;
	int iBase = sti(PChar.MysteriousArtefact.SantiagoAttackers);
	int iN;
	for (iN = 1; iN <= iBase; iN++)
	{
		if (GetCharacterIndex("MA Attack " + iN) == -1) iDead++;
	}
	PChar.MysteriousArtefact.SantiagoAttackers = iBase - iDead;

	PChar.quest.MysteriousArtefactInSantiagoTownhall.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInSantiagoTownhall.win_condition.l1.location = "MA_Santiago_townhall";
	PChar.quest.MysteriousArtefactInSantiagoTownhall.function = "MysteriousArtefactInSantiagoTownhall";
}

// Третий бой - в резиденции. Губернатор Сантьяго берётся живым.
void MysteriousArtefactInSantiagoTownhall(string qName)
{
	BO_ClearCitizens("MA_Santiago_townhall", "");
	chrDisableReloadToLocation = true;

	// Gregg: клон-губернатор копирует внешность и имя настоящего мэра Сантьяго,
	// но это отдельный персонаж - настоящий остаётся жив и на своём месте
	ref rMayor = CharacterFromID("Santiago_Mayor");
	string sModel = rMayor.model;
	string sName = rMayor.name;
	string sLastName = rMayor.lastname;

	ref chr = GetCharacter(NPC_GenerateCharacter("MA Santiago Mayor", sModel, "man", "man", 1, PIRATE, 0, false));
	chr.name = sName;
	chr.lastname = sLastName;
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Mayor.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "mayor_capture";
	LAi_SetActorType(chr);
	LAi_ActorAfraid(chr, PChar, false);
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "MA_Santiago_townhall", "goto", "governor1");

	int iQty = sti(PChar.MysteriousArtefact.SantiagoAttackers);
	if (iQty > 3) iQty = 3;

	int iRank = sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE;
	int iN;
	for (iN = 1; iN <= iQty; iN++)
	{
		sModel = "officer_" + iN;
		chr = GetCharacter(NPC_GenerateCharacter("MA Hall Attacker " + iN, sModel, "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "MA_Santiago_townhall", "goto", "goto" + iN);
		// Gregg: SetFantomParamFromRank вместо голого LAi_NPC_Equip - он же зовёт
		// CalculateSkillsFromRank и заводит skill.*_exp. Без них начисление опыта
		// группе игрока падало в RUNTIME ERROR «missed attribute: <навык>_exp»
		// (RPGUtilite.c:2262), а бойцы выходили без навыков вовсе.
		SetFantomParamFromRank(chr, iRank, true);
		SetFoodToCharacter(chr, 5, 50);
		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	}

	string sSpot;
	string sGroup;
	for (iN = 1; iN <= 6; iN++)
	{
		sModel = "sold_spa_" + (rand(7)+1);
		chr = GetCharacter(NPC_GenerateCharacter("MA Hall Defender " + iN, sModel, "man", "man", iRank, PIRATE, 0, false));
		// Gregg: SetFantomParamFromRank вместо голого LAi_NPC_Equip - он же зовёт
		// CalculateSkillsFromRank и заводит skill.*_exp. Без них начисление опыта
		// группе игрока падало в RUNTIME ERROR «missed attribute: <навык>_exp»
		// (RPGUtilite.c:2262), а бойцы выходили без навыков вовсе.
		SetFantomParamFromRank(chr, iRank, true);
		SetFoodToCharacter(chr, 5, 50);

		// Gregg: трое вбегают из дверей, трое уже стоят в зале
		sGroup = "goto";
		sSpot = "goto" + iN;
		if (iN < 4)
		{
			sGroup = "reload";
			sSpot = "reload2";
		}
		ChangeCharacterAddressGroup(chr, "MA_Santiago_townhall", sGroup, sSpot);

		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);

	LAi_SetPlayerType(PChar);
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("MysteriousArtefactInSantiagoTownhallWinner", "MysteriousArtefactInSantiagoTownhallWinner", "MA Hall Defender ", 6);
}

void MysteriousArtefactInSantiagoTownhallWinner(string qName)
{
	PlayStereoSound("new_round");
	Log_Info(iTxt("BO_EnemyCrushed"));
	SetDialogCharacter2Character(PChar, CharacterFromID("MA Santiago Mayor"), 25.0, true);
}

// Gregg: ГГ расстреливает губернатора. Настоящий губернатор Сантьяго при этом
// остаётся жив - убивают клона в клон-городе.
void MysteriousArtefactKillSantiagoGovernor()
{
	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, -5, 2, 0, false);

	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("MA Santiago Mayor"));
	DoQuestFunctionDelay("MysteriousArtefactKillSantiagoGovernor2", 3.0);
}

void MysteriousArtefactKillSantiagoGovernor2(string _tmp)
{
	LAi_ActorTurnToCharacter(PChar, CharacterFromID("MA Santiago Mayor"));
	LAi_ActorAnimation(PChar, "Shot", "", -1);
	DoQuestFunctionDelay("MysteriousArtefactKillSantiagoGovernor3", 1.0);
}

void MysteriousArtefactKillSantiagoGovernor3(string _tmp)
{
	ref chr = CharacterFromID("MA Santiago Mayor");
	LAi_SetStayType(chr);
	LAi_KillCharacter(chr);
	DoQuestFunctionDelay("MysteriousArtefactKillSantiagoGovernorEnd", 1.0);
}

void MysteriousArtefactKillSantiagoGovernorEnd(string _tmp)
{
	chrDisableReloadToLocation = false;
	locCameraFollow();

	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;
	LAi_SetPlayerType(PChar);

	PChar.MysteriousArtefact = "22";

	// Gregg: настоящему Сантьяго назначается новый мэр - в мире города губернатора
	// как бы убили, хотя резня шла в клоне. Единственный след, который штурм
	// оставляет в настоящем мире, и он ровно такой, каким должен быть.
	SetRandomNameToCharacter(CharacterFromID("Santiago_Mayor"));
	SetTownMayor(CharacterFromID("Santiago_Mayor"), SPAIN);

	Locations[FindLocation("MA_Santiago_town")].reload.l3.disable = 1;
	PChar.quest.MysteriousArtefactPreExitSantiago.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactPreExitSantiago.win_condition.l1.location = "MA_Santiago_town";
	PChar.quest.MysteriousArtefactPreExitSantiago.function = "MysteriousArtefactPreExitSantiago";
}

void MysteriousArtefactPreExitSantiago(string qName)
{
	ref chr = CharacterFromID("Lukash de Grassi");
	chr.Dialog.CurrentNode = "52";
	ChangeCharacterAddressGroup(chr, "MA_Santiago_town", "goto", "goto29");
	SetDialogCharacter2Character(PChar, chr, 25.0, false);
}

// Матильда прячется в магазине, при ней охранник.
void MysteriousArtefactToSantiagoStore()
{
	chrDisableReloadToLocation = false;

	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Lukash de Grassi");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload5_back", "none", "", "", "", 30.0);

	Locations[FindLocation("MA_Santiago_town")].reload.l4.disable = 0;

	PChar.quest.MysteriousArtefactInSantiagoStore.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInSantiagoStore.win_condition.l1.location = "MA_Santiago_store";
	PChar.quest.MysteriousArtefactInSantiagoStore.function = "MysteriousArtefactInSantiagoStore";
}

void MysteriousArtefactInSantiagoStore(string qName)
{
	chrDisableReloadToLocation = true;
	// Gregg: в лавке и на складе своя обслуга - в захваченном городе её быть не должно
	BO_ClearCitizens("MA_Santiago_store", "");
	BO_ClearCitizens("MA_Store_Packhouse", "");

	// Gregg: телохранитель Матильды - именной боец, а не рядовой защитник.
	// Ранг и здоровье задаём явно, без привязки к рангу ГГ: в оригинале его
	// не мог одолеть никто, и слабаком он быть не должен.
	int iRank = BO_VERSTEK_RANK;
	ref chr = GetCharacter(NPC_GenerateCharacter("Verstekholsten", "officer_13", "man", "man", iRank, PIRATE, 0, false));
	chr.name = iTxt("BO_VerstekName");
	chr.lastname = "";
	chr.rank = iRank;
	LAi_SetWarriorType(chr);

	FantomMakeCoolFighter(chr, iRank, 10, 10, BLADE_LONG, "pistol6", 100);
	LAi_SetHP(chr, BO_VERSTEK_HP, BO_VERSTEK_HP);
	SetSelfSkill(chr, 90, 85, 70, 75, 70);
	SetFoodToCharacter(chr, 5, 50);

	ChangeCharacterAddressGroup(chr, "MA_Santiago_store", "goto", "goto2");

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.MysteriousArtefactInSantiagoStoreWinner.win_condition.l1 = "NPC_Death";
	PChar.quest.MysteriousArtefactInSantiagoStoreWinner.win_condition.l1.character = "Verstekholsten";
	PChar.quest.MysteriousArtefactInSantiagoStoreWinner.function = "MysteriousArtefactInSantiagoStoreWinner";
}

// Gregg: вся концовка теперь в самой лавке - склад из цепочки убран целиком.
// Он тянул за собой сторожа SkladMan (CreateSkladInsideEncounters по атрибуту
// packhouse), лишнюю дверь и обратный проход через город, где заново заселялись
// жители. Матильда лежит на месте торговца - локатор barmen/stay, тот же, где
// в обычной игре стоит Santiago_trader.
void MysteriousArtefactInSantiagoStoreWinner(string qName)
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("MA Woman");
	chr.name = iTxt("BO_MatildaName");
	chr.lastname = "";
	chr.Dialog.CurrentNode = "8";
	chr.greeting = "";
	BO_FreeLocator("MA_Santiago_store", "stay", "MA Woman");
	ChangeCharacterAddressGroup(chr, "MA_Santiago_store", "barmen", "stay");
	LAi_SetImmortal(chr, true);

	LAi_SetActorType(chr);
	LAi_ActorSetLayMode(chr);

	SetDialogCharacter2Character(PChar, chr, 25.0, false);
}

// Gregg: та самая «незнакомка» из Порт-о-Принса оказывается Матильдой.


void MysteriousArtefactDialogWithMatilda(string qName)
{
	LAi_ActorDialogNow(CharacterFromID("MA Woman"), PChar, "", -1);
}

// Gregg: ==> ЗДЕСЬ ОРИГИНАЛ ОТБИРАЛ У ИГРОКА КОРАБЛЬ.
// К:ПС подменяла PChar.Ship.Type на фрегат «Бесполезный» с 15% корпуса и минус
// восемьдесят к парусам - чтобы в морском финале главы игрока гарантированно
// потопили и глава 5 началась нищим у ловцов жемчуга. Мы от конфискации отказались:
// корабль, команда, каюта и эскадра остаются при игроке. Сюжетную беспомощность
// в главе 5 даёт запертый остров (bQuestDisableMapEnter + Island_SetReloadEnableGlobal),
// а не отъём имущества. Соответственно весь блок подмены корабля выброшен целиком.
void MysteriousArtefactMatildaIsDead()
{
	LAi_KillCharacter(CharacterFromID("MA Woman"));
	LAi_SetPlayerType(PChar);

	Locations[FindLocation("MA_Santiago_town")].reload.l4.disable = 1;

	// Gregg: де Грасси приходит за игроком прямо в лавку - обратной дороги через
	// город нет: там заново заселяются жители, а пирс заперт до самого прорыва.
	ref chr = CharacterFromID("Lukash de Grassi");
	chr.Dialog.CurrentNode = "59";
	// Gregg: ставим в дверях и даём ДОЙТИ, а не проявляем посреди лавки.
	// reload1_back - тот самый дверной локатор, через который входят с улицы
	// (Store02, уровень пола). Пара ActorWaitDialog + ActorDialog - штатная связка:
	// ГГ ждёт, де Грасси подходит и сам начинает разговор.
	ChangeCharacterAddressGroup(chr, "MA_Santiago_store", "reload", "reload1_back");
	LAi_SetActorType(chr);
	LAi_ActorWaitDialog(PChar, chr);
	LAi_ActorDialog(chr, PChar, "", 15.0, 1.0);
}
// Gregg: <== конфискация корабля выброшена



// Финал главы: на выходе из бухты Сантьяго ждёт испанская погоня.
void MysteriousArtefactToSantiagoSea()
{
	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Lukash de Grassi");
	LAi_SetActorType(chr);

	// Gregg: уходим НЕ через порт Сантьяго. Своих кораблей там нет - эскадра
	// осталась на Камагуэе, откуда и шли на штурм, а под пушками форта нас бы
	// просто расстреляли. Возвращаем на тот же берег: Shore11, лодочный выход
	// в море там отпирается обратно (гасили его на время штурма).
	Locations[FindLocation("Shore11")].reload.l2.disable = 0;
	ChangeCharacterAddressGroup(chr, "Shore11", "goto", "goto9");

	ref sld;
	int iN;
	for (iN = 1; iN <= 3; iN++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("MA_Cuba1_" + iN, "", "man", "man", 20, SPAIN, -1, true));
		SetCaptanModelByEncType(sld, "war");
		FantomMakeCoolFighter(sld, 20, 80, 70, "blade016", "pistol3", 20);
		// Gregg: в К:ПС тут SHIP_WARSHIP, которого у нас нет. Берём тяжёлый фрегат -
		// третий класс, как и у отряда с Мартиники: погоня, а не эскадра метрополии
		FantomMakeCoolSailor(sld, SHIP_FRIGATE_H, "", CANNON_TYPE_CULVERINE_LBS24, 70, 70, 70);
		SetGeraldDetails(sld, iN, rand(7), "", "spa_" + (rand(3)+2));
		sld.AlwaysSandbankManeuver = true;
		sld.AlwaysEnemy = true;
		sld.AnalizeShips = true;
		sld.DontRansackCaptain = true;
		LAi_group_MoveCharacter(sld, "EnemyFight");
		BO_SetSeaPerks(sld);
	}

	Group_FindOrCreateGroup("MA_Cuba1");
	Group_AddCharacter("MA_Cuba1", "MA_Cuba1_1");
	Group_AddCharacter("MA_Cuba1", "MA_Cuba1_2");
	Group_AddCharacter("MA_Cuba1", "MA_Cuba1_3");
	Group_SetType("MA_Cuba1", "pirate");
	Group_SetGroupCommander("MA_Cuba1", "MA_Cuba1_1");
	// Gregg: reload_fort1 - устье бухты Сантьяго (264.62, -997.22). Отход перенесён
	// на Камагуэй, а туда от бухты 2580 юнитов, и прямая между ними идёт через сушу -
	// игроку пришлось бы огибать пол-Кубы. Quest_ship_6 (-787.86, 1748.51) стоит
	// в 513 юнитах от точки выхода с Shore11 (reload_3/ship_1 = -331.74, 1513.50),
	// до берега 420, вода на всём пути чистая.
	Group_SetAddress("MA_Cuba1", "Cuba1", "quest_ships", "Quest_ship_6");
	Group_SetTaskAttack("MA_Cuba1", PLAYER_GROUP);

	PChar.quest.MysteriousArtefactInSantiagoSea.win_condition.l1 = "location";
	PChar.quest.MysteriousArtefactInSantiagoSea.win_condition.l1.location = "Cuba1";
	PChar.quest.MysteriousArtefactInSantiagoSea.function = "MysteriousArtefactInSantiagoSea";

	// Gregg: плашка и перенос на мыс. Reload_To_Location - штатная пара
	// SetLaunchFrameReloadLocationParam + LaunchFrameForm, см. boal_frame.c
	SetLaunchFrameReloadLocationParam("Shore11", "goto", "goto9", "");
	SetLaunchFrameFormParam(iTxt("BO_ToCamaguey"), "Reload_To_Location", 0.1, 4.0);
	LaunchFrameForm();
}

// Gregg: ==> ЗДЕСЬ ОРИГИНАЛ ТОПИЛ ИГРОКА.
// В К:ПС бой был заведомо проигрышным: корабль подменяли на «Бесполезный» с 15%
// корпуса, ждали Ship_HP <= 1000 и уводили героя в беспамятстве к ловцам жемчуга.
// Раз конфискации нет, нет и гарантированного потопления - да и условия Ship_HP
// в quests_check.c у нас попросту не существует. Бой стал настоящим, с двумя
// исходами, как у Мартиники: перетопить погоню или уйти на глобальную карту.
// В обоих случаях глава заканчивается одинаково - ГГ приходит в посёлок ловцов
// жемчуга сам, своим ходом, а не приплывает туда на обломке мачты.
void MysteriousArtefactInSantiagoSea(string qName)
{
	Island_SetReloadEnableGlobal("Cuba1", false);

	PChar.quest.MysteriousArtefactSeaWin.win_condition.l1 = "Group_Death";
	PChar.quest.MysteriousArtefactSeaWin.win_condition.l1.group = "MA_Cuba1";
	PChar.quest.MysteriousArtefactSeaWin.function = "MysteriousArtefactSeaWin";

	PChar.quest.MysteriousArtefactSeaEscape.win_condition.l1 = "MapEnter";
	PChar.quest.MysteriousArtefactSeaEscape.function = "MysteriousArtefactSeaEscape";
}

void MysteriousArtefactSeaDone()
{
	Island_SetReloadEnableGlobal("Cuba1", true);
	Group_DeleteGroup("MA_Cuba1");

	BO_MARecord("28");
	PChar.MysteriousArtefact = "23";

	// Gregg: штурм позади - возвращаем быстрый переход, погашенный на время
	// работы в клон-городе (иначе он уводил в настоящий Сантьяго)
	bDisableFastReload = false;

	MysteriousArtefactReturnAll();
	DoQuestFunctionDelay("MysteriousArtefactEnd", 1.0);
}

void MysteriousArtefactSeaWin(string qName)
{
	PChar.quest.MysteriousArtefactSeaEscape.over = "yes";
	MysteriousArtefactSeaDone();
}

void MysteriousArtefactSeaEscape(string qName)
{
	PChar.quest.MysteriousArtefactSeaWin.over = "yes";
	MysteriousArtefactSeaDone();
}

void MysteriousArtefactEnd(string _tmp)
{
	// Gregg: метка "DestinyGift_1" из К:ПС ловилась кейсом в свитче quests_reaction.c,
	// которого мы не трогаем. DoFunctionReloadToLocation зовёт функцию по имени
	// напрямую и без аргументов - тем же способом, что вход в клон-Сантьяго.
	// Приходим в себя В ДОМЕ, а не у ворот посёлка: так говорит и сам диалог главы
	// («Мы перенесли тебя сюда и подлечили чем умели», «жить будешь в доме Кендарта»).
	BO_PearlHutDoorOn();
	// Gregg: сутки беспамятства - ровно то, что глава посёлка говорит в BO_PearlHead.c
	// («Сутки с небольшим»). Без этого календарь стоял на месте: дата пробуждения
	// совпадала с датой боя у Сантьяго. Образец - ночь на берегу выше по файлу.
	WaitDate("", 0, 0, 1, 0, 0);
	SetCurrentTime(9, 20);
	RecalculateJumpTable();
	RefreshWeather();
	RefreshLandTime();
	// Gregg: goto1 - у лежанки, а не на пороге: ГГ очнулся там, куда его положили
	DoFunctionReloadToLocation("BO_PearlHut", "goto", "goto1", "DestinyGiftStart");
}

// Gregg: у К:ПС эта функция называлась «Потеряли всё» и делала ровно это: удаляла
// корабль игрока, убивала ВСЕХ спутников и пассажиров и вычищала сундуки каюты.
// Мы от этого отказались - от неё остались только снятие сюжетных блокировок.
// Название сохранено, чтобы место сходилось с оригиналом при сверке.
void MysteriousArtefactReturnAll()
{
	Island_SetReloadEnableGlobal("Cuba1", true);
	bQuestDisableMapEnter = false;
	bDisableCharacterMenu = false;

	// Gregg: Лукаш де Грасси свою роль отыграл и уходит сам - а не гибнет вместе
	// со всей эскадрой, как в оригинале
	ref chr = CharacterFromID("Lukash de Grassi");
	if (CheckAttribute(chr, "id"))
	{
		RemovePassenger(PChar, chr);
		SetCharacterRemovable(chr, true);
		chr.LifeDay = 0;
		ChangeCharacterAddress(chr, "None", "");
	}

	// Gregg: закрытие главы 4 стояло в оригинале ВНУТРИ блока конфискаций (MainLine.c:3473),
	// который мы выбросили целиком, - и вместе с ним пропало. Из-за этого «Таинственный
	// артефакт» навсегда оставался открытым в судовом журнале.
	PChar.MysteriousArtefact = "END";
	CloseQuestHeader("MysteriousArtefact");
	pchar.questdata.MysteriousArtefact = 1;	// LEO: отметка в энциклопедии
}
// Gregg: <== потопление и «потеряли всё» выброшены
// Gregg: <== глава 4

// Gregg: ==> глава 5. «Чёрная Орхидея - подарок судьбы», поселенческий акт

// Прорвавшись от Сантьяго, ГГ оказывается выброшен в посёлок ловцов жемчуга и
// застревает там: корабля под рукой нет, эскадра неизвестно где, уйти некуда.
// Отрабатывает свободу у главы посёлка, а кончается акт морской охотой на
// Ферегуэйро - первого обладателя Орхидеи, который громит посёлок раз за разом.
//
// Отличия от К:ПС:
// - оригинал держал игрока в посёлке ничем: корабль просто не выдавали, а уйти
//   мешала только география. У нас акт запирается честно - см. BO_PearlLockOn;
// - вместо подарочного люггера «Броненосец» в конце возвращается СВОЙ флагман:
//   починенный и с пополненным боезапасом, пока ГГ был недоступен;
// - корабль Ферегуэйро масштабируется по классу флагмана игрока: в оригинале был
//   намертво зашитый шлюп, а к этому месту ГГ приходит на чём угодно.

#define BO_PEARL_GRAPES_MIN		1000	// ниже этого картечь пополняем до этой отметки
#define BO_PEARL_POWDER_MIN		1000	// то же для пороха
#define BO_MERITOR_PRICE		400		// цена седьмой части у лоточника Виллемстада

// Gregg: запереть игрока в ареале ловцов. Несущий засов - обнуление точки стоянки
// корабля: выход в море пускает только из локации, записанной в from_sea
// (reload.c:197), и это же закрывает единственную живую морскую дверь зоны -
// лодку на Shore9 в Белизе. Остальное - страховка.
// Островной запрет Island_SetReloadEnableGlobal тут звать НЕЛЬЗЯ: островов «Mein»
// и «Pearl» в islands_init.c нет, а GetIslandByID при промахе возвращает Islands[0],
// то есть молча заперся бы Барбадос.
void BO_PearlLockOn()
{
	if (CheckAttribute(PChar, "DestinyGift.Locked")) return;
	PChar.DestinyGift.Locked = true;

	// Gregg: стоянку запоминаем как страховку. Штатно акт ставит корабль в бухту
	// Аматике (Shore9) в DestinyGiftNeedKillFeregueyro, поверх того, что вернёт LockOff,
	// - но если цепочка где-то оборвётся, сейв не останется без выхода в море навсегда.
	PChar.DestinyGift.KeepShipLoc = GetCharacterShipLocation(PChar);
	SetCharacterShipLocation(PChar, "");

	// атлас: экран открывается, но телепорт отказывает (mapview.c:383).
	// Хоткей M не перекрывается bDisableCharacterMenu, поэтому глушим сам телепорт.
	PChar.MapBestTeleportDisable = true;

	// если игрок всё же окажется в море - глобальная карта закрыта
	bQuestDisableMapEnter = true;

	// Gregg: свору снимаем по образцу дуэли ГВИК (Addons/utils.c:1515). Флаг глобальный
	// на весь мод, поэтому маркер владения обязателен - иначе вернём чужое.
	if (!CheckAttribute(PChar, "DestinyGift.NoOfficers"))
	{
		PChar.DestinyGift.NoOfficers = true;
		DeleteAllOfficersFromLocation();
		bDisableOfficers = true;
	}

	// Gregg: глушим штатный генератор грабежа жемчуга на время акта. Он взводится на
	// вход в Pearl_Jungle_02 и Pearl_Jungle_06 - обе локации внутри ареала, а первая ещё
	// и лежит на единственной дороге из посёлка. Сработав, он зовёт
	// LAi_group_Delete("PearlGroup_1") (то есть сносит группу главы посёлка) и поднимает
	// locations.pearlVillage, после чего CreatePearlVillage перебивает главе диалог
	// обратно на Pearl_dialog.c (LAi_utilites.c:1226) - акт после этого не продолжить.
	// Запоминаем, был ли генератор взведён, чтобы вернуть ровно то, что забрали.
	if (CheckAttribute(PChar, "quest.PearlGen_checkTeno.win_condition"))
	{
		PChar.DestinyGift.PearlGenTeno = true;
		PChar.quest.PearlGen_checkTeno.over = "yes";
	}
	if (CheckAttribute(PChar, "quest.PearlGen_checkSantaCatalina.win_condition"))
	{
		PChar.DestinyGift.PearlGenSC = true;
		PChar.quest.PearlGen_checkSantaCatalina.over = "yes";
	}
}

// Gregg: отпустить. Быстрый переход в зоне не трогаем вовсе - атрибута fastreload
// нет ни у одной локации ареала, гасить нечего.
void BO_PearlLockOff()
{
	if (!CheckAttribute(PChar, "DestinyGift.Locked")) return;
	DeleteAttribute(PChar, "DestinyGift.Locked");

	if (CheckAttribute(PChar, "MapBestTeleportDisable")) DeleteAttribute(PChar, "MapBestTeleportDisable");
	bQuestDisableMapEnter = false;

	if (CheckAttribute(PChar, "DestinyGift.KeepShipLoc"))
	{
		SetCharacterShipLocation(PChar, PChar.DestinyGift.KeepShipLoc);
		DeleteAttribute(PChar, "DestinyGift.KeepShipLoc");
	}

	if (CheckAttribute(PChar, "DestinyGift.NoOfficers"))
	{
		DeleteAttribute(PChar, "DestinyGift.NoOfficers");
		bDisableOfficers = false;
	}

	// Gregg: генератор промысла здесь НЕ возвращаем - это делает BO_PearlGenRestore
	// в самом конце акта. LockOff зовётся в состоянии 18, когда игроку ещё предстоит
	// вернуться в посёлок пешком через Pearl_Jungle_03 -> Pearl_Jungle_02, а это ровно
	// условие срабатывания PearlGen_checkTeno.
}

// Gregg: вернуть штатный генератор грабежа промысла ровно в том объёме, в каком глушили.
// Зовётся только из финала акта: вернуть его раньше нельзя - обратная дорога с моря
// в посёлок идёт через Pearl_Jungle_02, генератор сработал бы на входе, поднял
// pearlVillage, и CreatePearlVillage перебил бы главе и диалог, и ИМЯ
// (SetRandomNameToCharacter), которое к тому времени уже записано в квестбук.
void BO_PearlGenRestore()
{
	int iLoc = FindLocation("Pearl_town_1");
	if (iLoc >= 0) Locations[iLoc].pearlVillage = false;

	if (CheckAttribute(PChar, "DestinyGift.PearlGenTeno"))
	{
		DeleteAttribute(PChar, "DestinyGift.PearlGenTeno");
		PearlGen_SetTeno("");
	}
	if (CheckAttribute(PChar, "DestinyGift.PearlGenSC"))
	{
		DeleteAttribute(PChar, "DestinyGift.PearlGenSC");
		PearlGen_SetSantaCatalina("");
	}
}

// Gregg: флагман дождался хозяина и был приведён в порядок за счёт корабельной казны.
// Чиним корпус, паруса и мачты, добираем команду - и ВЫБОРОЧНО пополняем расходники:
// картечь и порох до отметки, оружие и лекарства - до численности команды. Прочие
// товары не трогаем: это не склад, а то, без чего в бой не выйти.
// Эскадру не возвращаем: у игрока её может не быть вовсе, а бой с Ферегуэйро
// задуман поединком.
// Gregg: добить товар до отметки, но строго в пределах свободного трюма.
// SetCharacterGoods вместимость НЕ проверяет - при переполнении только пишет Trace
// (CharacterUtilite.c:327), а перегруженный корабль игроку не нужен. Вес всех четырёх
// расходников - 1 за единицу, но считаем всё равно через GetCharacterFreeSpace.
void BO_TopUpGood(int iGood, int iWant)
{
	int iHave = GetCargoGoods(PChar, iGood);
	if (iHave >= iWant) return;
	int iAdd = iWant - iHave;
	int iFree = GetCharacterFreeSpace(PChar, iGood);
	if (iAdd > iFree) iAdd = iFree;
	if (iAdd <= 0) return;
	SetCharacterGoods(PChar, iGood, iHave + iAdd);
}

void BO_RestockFlagship()
{
	// Gregg: без корабля GetCharacterShipHP вернёт 0, и «починка» записала бы
	// потопленный корпус. Такого сценария быть не должно, но проверка дешёвая.
	if (GetCharacterShipType(PChar) == SHIP_NOTUSED) return;

	PChar.ship.hp = GetCharacterShipHP(PChar);
	if (CheckAttribute(PChar, "ship.blots")) DeleteAttribute(PChar, "ship.blots");
	PChar.ship.sp = GetCharacterShipSP(PChar);
	if (CheckAttribute(PChar, "ship.sails")) DeleteAttribute(PChar, "ship.sails");
	RepairMasts(PChar);
	PChar.ship.soiling = 0;
	SetCrewQuantityFull(PChar);

	int iCrew = GetCrewQuantity(PChar);

	BO_TopUpGood(GOOD_GRAPES, BO_PEARL_GRAPES_MIN);
	BO_TopUpGood(GOOD_POWDER, BO_PEARL_POWDER_MIN);
	BO_TopUpGood(GOOD_WEAPON, iCrew);
	BO_TopUpGood(GOOD_MEDICAMENT, iCrew);
}

// Gregg: корабль Ферегуэйро по классу флагмана игрока. В оригинале был намертво
// зашитый шлюп, но к этому месту ГГ приходит на чём угодно - от брига до мановара,
// и фиксированный класс вышел бы либо формальностью, либо стеной.
int BO_FeregueyroShip()
{
	int iClass = GetCharacterShipClass(PChar);
	// класс в CSP тем меньше, чем крупнее корабль: 1 - мановар, 7 - «нет корабля»
	if (iClass <= 3) return SHIP_FRIGATE_H;
	if (iClass == 4) return SHIP_CORVETTE;
	return SHIP_BRIG;
}
// Gregg: глава посёлка. В К:ПС он к началу акта существовал всегда, в CSP его
// поднимает CreatePearlVillage (LAi_utilites.c:1223) - и только при первом заходе
// в посёлок: дальше locations.pearlVillage гасится, а если игрок раньше грабил
// промысел, генератор вылетает по return ещё до создания главы. Поэтому заводим
// его сами, если его нет, и в любом случае готовим к акту.
void BO_EnsurePearlHead()
{
	ref chr;
	int iRank;
	int iNation;
	if (GetCharacterIndex("HeadMan_1") == -1)
	{
		iNation = sti(colonies[FindColony("SantaCatalina")].nation);
		iRank = 20 + rand(15);
		chr = GetCharacter(NPC_GenerateCharacter("HeadMan_1", "SpaOfficer" + (rand(2)+1), "man", "man", iRank, iNation, 30, true));
		SetCharacterPerk(chr, "Energaiser");
		SetCharacterPerk(chr, "SwordplayProfessional");
		chr.city = "SantaCatalina";
		SetFantomParamFromRank(chr, iRank, true);
	}
	else
	{
		chr = CharacterFromID("HeadMan_1");
	}
	// Gregg: на время акта он вечный - в CSP генератор даёт ему LifeDay 30, а акт
	// с морской охотой за Ферегуэйро легко перевалит за месяц. Срок жизни возвращает
	// DestinyGiftExitPearlVillage, как в оригинале.
	// ВАЖНО: «вечный» - это ОТСУТСТВИЕ атрибута, а не -1. EmptyAllFantomCharacter
	// (scripts\utils.c:1459) сравнивает прожитые дни как pastDay >= sti(LifeDay),
	// и при -1 условие истинно СРАЗУ (pastDay всегда >= 0), а зовётся чистильщик из
	// ReloadEndFade, то есть на каждом переходе - главу стирало на первом же выходе
	// из посёлка. Гварда на NotRemoved в том цикле нет.
	// Так же поступает и сам NPC_GenerateCharacter при отрицательном сроке (utils.c:1875).
	if (CheckAttribute(chr, "LifeDay")) DeleteAttribute(chr, "LifeDay");
	chr.NotRemoved = true;
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_PearlHead.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetImmortal(chr, true);
	LAi_SetStayType(chr);
	LAi_group_MoveCharacter(chr, "PearlGroup_1");
	LAi_group_SetRelation("PearlGroup_1", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
	// Gregg: goto19 (-1.5, -0.4, -1.6) - в двух шагах от reload2, двери дома Кендарта,
	// откуда ГГ выходит после пробуждения. Глава ждёт прямо у порога, а не на другом
	// конце улицы, как было на goto1 (21.5, -0.3, -3.5).
	ChangeCharacterAddressGroup(chr, "Pearl_town_1", "goto", "goto19");

	// Gregg: вход в главу - это загрузка Pearl_town_1, и на ней же успевает отработать
	// CreatePearlVillage. Если часовой промысла был жив, он взводит собственную сцену
	// с окриком (quests_reaction.c:3521), которая столкнулась бы с нашим разговором.
	if (CheckAttribute(PChar, "quest.Pearl_GuardManIsLife")) PChar.quest.Pearl_GuardManIsLife.over = "yes";
	// Gregg: флаг pearlVillage тут НЕ гасим. Гасить его руками нечего - CreatePearlVillage
	// снимает его сам, отработав (LAi_utilites.c:1220), а от повторного взвода посреди
	// акта защищает глушилка генератора промысла в BO_PearlLockOn. Зато преждевременное
	// гашение оставляло посёлок вовсе безлюдным: ГГ теперь очухивается в доме, Pearl_town_1
	// на этой загрузке не трогается, и генератор к выходу на улицу уже был бы отключён.
}

// Gregg: AddQuestUserData привязывается к ПОСЛЕДНЕЙ добавленной записи
// (quests.c:404 берёт GetAttributesNum(...)-1), а не к квесту целиком. Значит
// подстановку надо повторять после КАЖДОЙ записи, иначе в дневнике печатается
// сырое @<sOfficerLastName>. Заодно регистрируем род ГГ - он бывает женщиной.
void BO_PearlRecord(string sRec)
{
	AddQuestRecord("DestinyGift", sRec);
	AddQuestUserData("DestinyGift", "sSex", GetSexPhrase("ся", "ась"));
	// Gregg: обычное окончание тоже нужно - без него «нашёл@<sSex1>» доезжало сырым
	AddQuestUserData("DestinyGift", "sSex1", GetSexPhrase("", "а"));
	// Gregg: записи ставятся и вдали от посёлка, поэтому голову проверяем - чтение
	// атрибута у несуществующего персонажа это рантайм-ошибка, а не пустая строка.
	if (GetCharacterIndex("HeadMan_1") == -1) return;
	ref rHead = CharacterFromID("HeadMan_1");
	AddQuestUserData("DestinyGift", "sOfficerLastName", rHead.lastname);
	AddQuestUserData("DestinyGift", "sOfficerFullName", GetFullName(rHead));
}

// Gregg: сам дом Кендарта заводится штатно, фабрикой DefineBOPearlHut (Locations\init\Pearl.c).
// Здесь на время акта перецепляется только дверь посёлка напротив ратуши - глава говорит
// «дом, в котором ты отлёживался... там и оставайся», а вещи лежат «там же, в сундуке».
// Правим только .go у Pearl_town_1, саму заглушку CommonHut не трогаем: на этих граблях
// мод уже стоял. Возвращает DestinyGiftExitPearlVillage.
void BO_PearlHutDoorOn()
{
	int iTown = FindLocation("Pearl_town_1");
	if (iTown >= 0) Locations[iTown].reload.l2.go = "BO_PearlHut";
}

// Gregg: вход в главу. Зовётся по имени из MysteriousArtefactEnd через
// DoFunctionReloadToLocation, то есть БЕЗ аргументов. ГГ очнулся в доме - глава
// посёлка ждёт его на улице, у порога.
void DestinyGiftStart()
{
	BO_PearlLockOn();

	PChar.DestinyGift.PearlVillageLive = true;
	PChar.DestinyGift = "1";
	Log_Info(iTxt("BO_PearlWakeUp"));
	// Gregg: главу поднимаем не здесь, а на выходе из дома. Здесь нельзя: Pearl_town_1
	// ещё не загружался, а когда загрузится - CreatePearlVillage заселит посёлок и
	// заодно перебьёт главе диалог и имя, если тот уже существует.
	SetQuestFunction("DestinyGiftPreMeetHead", "DestinyGiftPreMeetHead", "l", "Pearl_town_1");
}

void DestinyGiftPreMeetHead(string qName)
{
	// Gregg: полкадра паузы, чтобы встать ПОСЛЕ CreatePearlVillage, а не наперегонки
	// с ним - иначе наша настройка главы будет затёрта генератором посёлка.
	DoQuestFunctionDelay("DestinyGiftMeetHead", 0.1);
}

// Первая встреча: ГГ вышел из дома, глава посёлка окликает его у порога.
void DestinyGiftMeetHead(string _tmp)
{
	BO_EnsurePearlHead();
	SetDialogCharacter2Character(PChar, CharacterFromID("HeadMan_1"), 5.0, false);
}

void DestinyGiftRestTimer()
{
	LAi_SetPlayerType(PChar);
	ref rHead = CharacterFromID("HeadMan_1");
	LAi_SetStayType(rHead);
	// Gregg: первый разговор отыгран - и глава уходит к себе в ратушу. Там его держит
	// и штатный генератор CSP (LAi_utilites.c:1233), и туда же его возвращает финал
	// акта, и там он сам себя помещает по сюжету («Мой долг - всё время быть при
	// жемчуге»). На улице его было не найти, а при штурме на его локатор садился ацтек.
	// Уходит ногами, а не телепортом: доходит до двери ратуши (reload6) и заходит внутрь.
	// Таймаут конечный - при -1 сорвавшийся маршрут вешает шаблон в falure навсегда.
	LAi_SetActorType(rHead);
	PChar.quest.DestinyGiftHeadAtHome.function = "DestinyGiftHeadAtHome";
	LAi_ActorRunToLocation(rHead, "reload", "reload6", "PearlTown1_Townhall", "goto", "goto5", "DestinyGiftHeadAtHome", 20.0);

	SetQuestHeader("DestinyGift");
	BO_PearlRecord("1");

	PChar.DestinyGift = "2";
	chrDisableReloadToLocation = false;
	SetTimerFunction("DestinyGiftRestTimerEnd", 0, 0, 1);
}

// Дошёл до ратуши и зашёл внутрь - возвращаем обычный тип, чтобы с ним говорили.
void DestinyGiftHeadAtHome(string qName)
{
	if (GetCharacterIndex("HeadMan_1") == -1) return;
	ref rHead = CharacterFromID("HeadMan_1");
	LAi_SetStayType(rHead);
	LAi_group_MoveCharacter(rHead, "PearlGroup_1");
}

void DestinyGiftRestTimerEnd(string qName)
{
	PChar.DestinyGift = "3";
}

void DestinyGiftFirstTask()
{
	PChar.DestinyGift = "4";
	BO_PearlRecord("2");

	BO_SetEncounters("Temple", true);
	SetQuestFunction("DestinyGiftInTemple", "DestinyGiftInTemple", "l", "Temple");
}

// Первое поручение: в храме засели двое, которых глава посёлка хочет видеть мёртвыми.
void DestinyGiftInTemple(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr;
	int iN;
	// Gregg: GetRank(PChar, 5) из оригинала прибавила бы ранг ГГ дважды
	int iRank = sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE;
	string sSpot;
	for (iN = 1; iN <= 2; iN++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("DestinyGift Pirate In Temple " + iN, "pirate_" + (rand(9)+1), "man", "man", iRank, PIRATE, 0, true));
		sSpot = "goto" + (iN + 12);
		ChangeCharacterAddressGroup(chr, "Temple", "goto", sSpot);
		// Gregg: SetFantomParamFromRank вместо голого LAi_NPC_Equip - он заводит skill.*_exp,
		// без которых начисление опыта падает в RUNTIME ERROR (RPGUtilite.c:2262)
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("DestinyGiftInTempleWinner", "DestinyGiftInTempleWinner", "DestinyGift Pirate In Temple ", 2);
}

void DestinyGiftInTempleWinner(string qName)
{
	BO_SetEncounters("Temple", false);
	chrDisableReloadToLocation = false;

	PChar.DestinyGift = "5";
	BO_PearlRecord("3");
}

// Второе поручение: набрать жемчуга в гроте.
void DestinyGiftToSeachPearl()
{
	PChar.DestinyGift = "6";
	BO_PearlRecord("4");

	// Gregg: открытие сундука в локации типа "cave" с группой локаторов monsters с шансом
	// 50% зовёт генератор «Проклятый капитан» (spawnToughSkeleton, items\itemLogic.c).
	// Pearl_Grot подходит под оба условия, а скелет 20+ рангов посреди поручения лишний.
	// Штатная блокировка - таймер локации; та же грабля уже ловилась в «Лунном камне»
	// (moonstone.c:522). Срок берём с запасом: поручение открытое, игрок ищет грот сам.
	LAi_LocationDisableToughSkeletonTimer("Pearl_Grot", 30);
	PChar.GenQuestBox.Pearl_Grot = true;
	PChar.GenQuestBox.Pearl_Grot.box1.items.DestinyGiftPearl = 4;
	PChar.GenQuestBox.Pearl_Grot.stay = true;
	SetQuestFunction("DestinyGiftSeachPearlInGrot", "DestinyGiftSeachPearlInGrot", "l", "Pearl_Grot");
}

void DestinyGiftSeachPearlInGrot(string qName)
{
	chrDisableReloadToLocation = true;
	// Gregg: перевзводим блокировку на входе в грот - если игрок искал его дольше
	// отведённого срока, к сундуку он всё равно подойдёт без скелета
	LAi_LocationDisableToughSkeletonTimer("Pearl_Grot", 30);

	PChar.DestinyGift = "7";
	PChar.quest.DestinyGiftFindPearls.win_condition.l1 = "item_qty";
	PChar.quest.DestinyGiftFindPearls.win_condition.l1.item = "DestinyGiftPearl";
	PChar.quest.DestinyGiftFindPearls.win_condition.l1.item.quantity = 4;
	PChar.quest.DestinyGiftFindPearls.function = "DestinyGiftFindPearls";
}

void DestinyGiftFindPearls(string qName)
{
	ClearQuestBoxStay("Pearl_Grot");

	PChar.DestinyGift = "8";
	// Gregg: в оригинале тут поднимались скелеты, но весь блок был закомментирован -
	// боя в гроте нет и не было. Оставляем как есть.
	chrDisableReloadToLocation = false;
	PChar.DestinyGift = "9";
	BO_PearlRecord("5");
}

void DestinyGiftNextTimer()
{
	PChar.DestinyGift = "10";
	BO_PearlRecord("6");

	SetTimerFunction("DestinyGiftNextTimerEnd", 0, 0, 2);
}

void DestinyGiftNextTimerEnd(string qName)
{
	PChar.DestinyGift = "11";
}

// Заданий больше нет - иди погуляй по бухтам. В заливе Никоя ждёт человек в балахоне.
void DestinyGiftNoneTask()
{
	PChar.DestinyGift = "12";
	// Gregg: своя запись на каждый этап. Раньше 12-й и 13-й шаги не попадали
	// в дневник вовсе, а потом всё сваливалось в одну запись задним числом.
	BO_PearlRecord("7");

	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	ref chr = GetCharacter(NPC_GenerateCharacter("Feregueyro", "Animists1", "man", "man", iRank, PIRATE, -1, false));
	chr.name = iTxt("BO_FeregueyroName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Feregueyro.c";
	chr.Dialog.CurrentNode = "1";
	// Gregg: SetGeraldDetails из оригинала выброшен. Во-первых, здесь он бесполезен -
	// корабля у Ферегуэйро ещё нет, а функция молча выходит без Ship.Type (utils.c:3023).
	// Во-вторых, и это важнее, она пишет не в персонажа, а в GetRealShip(тип), то есть
	// в ОБЩИЙ шаблон класса: перекрасились бы все тяжёлые фрегаты в игре. В К:ПС корабль
	// был жёстко SHIP_SLOOP, а у нас класс подбирается под игрока - цена ошибки другая.
	chr.DontRansackCaptain = true;
	// Gregg: корабль ему выдаём не здесь, а перед морской охотой (см. DestinyGiftNeedKillFeregueyro):
	// класс считается от флагмана игрока, а тот к тому моменту может смениться
	FantomMakeCoolFighter(chr, 30, 50, 50, "blade30", "pistol5", 100);
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "Shore50", "goto", "goto8");
	BO_SetEncounters("Shore50", true);
}

void DestinyGiftFindFeregueyro()
{
	PChar.DestinyGift = "13";
	BO_PearlRecord("8");
}

// Посёлок готовится к обороне, а ГГ послан за наёмным бойцом.
void DestinyGiftSearchKuvalda()
{
	ChangeCharacterAddress(CharacterFromID("Feregueyro"), "None", "");

	PChar.DestinyGift = "14";
	BO_PearlRecord("9");


	BO_SetEncounters("Pearl_GrotEntrance", true);
	SetQuestFunction("DestinyGiftFindKuvalda", "DestinyGiftFindKuvalda", "l", "Pearl_GrotEntrance");
}

void DestinyGiftFindKuvalda(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Cristobal Kuvalda", "officer_14", "man", "man", 20, PIRATE, -1, false));
	chr.name = iTxt("BO_KuvaldaName");
	chr.lastname = iTxt("BO_KuvaldaLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kuvalda.c";
	chr.Dialog.CurrentNode = "1";
	// Gregg: без NotRemoved его уводит DeleteAllFantomCharactersFromLocation в сцене посёлка
	chr.NotRemoved = true;
	chr.greeting = "Gr_MiddPirate";
	FantomMakeCoolFighter(chr, 15, 10, 10, "topor2", "pistol3", 100);
	// Gregg: К:ПС сажал Кувалду на локатор CorrectWork, но в модели CSP (caveEnt2_locators.gm)
	// такого нет, а goto там ровно один. Сажаем его на этот goto1, а троих ацтеков разводим
	// по monster1..3 - тогда глава не тянет за собой правку .gm, которая в гит не попадает.
	ChangeCharacterAddressGroup(chr, "Pearl_GrotEntrance", "goto", "goto1");
	LAi_SetImmortal(chr, true);
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	int iN;
	string sLocator;
	for (iN = 1; iN <= 3; iN++)
	{
		sLocator = "monster" + iN;
		chr = GetCharacter(NPC_GenerateCharacter("Cristobal Kuvalda Killer " + iN, "Canib_" + iN, "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "Pearl_GrotEntrance", "monsters", sLocator);
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("DestinyGiftFindKuvaldaWinner", "DestinyGiftFindKuvaldaWinner", "Cristobal Kuvalda Killer ", 3);
}

void DestinyGiftFindKuvaldaWinner(string qName)
{
	SetDialogCharacter2Character(PChar, CharacterFromID("Cristobal Kuvalda"), 10.0, false);
}

void DestinyGiftToPearlVillageJungle()
{
	BO_SetEncounters("Pearl_GrotEntrance", false);
	ref chr = CharacterFromID("Cristobal Kuvalda");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	chrDisableReloadToLocation = false;
	PChar.DestinyGift = "15";

	BO_SetEncounters("Pearl_Jungle_02", true);
	SetQuestFunction("DestinyGiftPreInPearlVillageJungle", "DestinyGiftPreInPearlVillageJungle", "l", "Pearl_Jungle_02");
}

void DestinyGiftPreInPearlVillageJungle(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_SetActorType(PChar);
	ref chr = CharacterFromID("Cristobal Kuvalda");
	LAi_SetStayType(chr);
	ChangeCharacterAddressGroup(PChar, "Pearl_Jungle_02", "goto", "goto13");
	ChangeCharacterAddressGroup(chr, "Pearl_Jungle_02", "goto", "goto14");
	LAi_ActorTurnToCharacter(PChar, chr);
	DoQuestFunctionDelay("DestinyGiftInPearlVillageJungle", 0.1);
}

// Засада на лесной тропе: шестеро на двоих плюс двое подоспевших из посёлка.
void DestinyGiftInPearlVillageJungle(string _tmp)
{
	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, -3, 5, 25, false);

	ref chr;
	if (GetCharacterIndex("PearlGuardMan_1") != -1)
	{
		chr = CharacterFromID("PearlGuardMan_1");
		ChangeCharacterAddress(chr, "None", "");
	}

	int iRank = sti(PChar.rank) + 8 + MOD_SKILL_ENEMY_RATE;
	int iN;
	string sModel;
	for (iN = 1; iN <= 6; iN++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("DestinyGift PearlVillageJungle Attack " + iN, "Canib_" + (rand(5)+1), "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "Pearl_Jungle_02", "reload", "reload3_back");
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}
	for (iN = 1; iN <= 2; iN++)
	{
		sModel = "pirate_" + (rand(9)+1);
		if (rand(1) == 1) sModel = "officer_" + (rand(19)+1);
		chr = GetCharacter(NPC_GenerateCharacter("DestinyGift PearlVillageJungle Defender " + iN, sModel, "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "Pearl_Jungle_02", "reload", "reload2_back");
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	}

	chr = CharacterFromID("Cristobal Kuvalda");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("DestinyGiftInPearlVillageJungle2", "DestinyGiftInPearlVillageJungle2", "DestinyGift PearlVillageJungle Attack ", 6);

	DoQuestFunctionDelay("DestinyGiftInPearlVillageJungleCamera", 10.0);
}

void DestinyGiftInPearlVillageJungleCamera(string _tmp)
{
	LAi_SetPlayerType(PChar);
	locCameraFollow();
}

void DestinyGiftInPearlVillageJungle2(string qName)
{
	BO_SetEncounters("Pearl_Jungle_02", false);
	chrDisableReloadToLocation = false;
	PChar.DestinyGift = "16";

	ref chr;
	if (GetCharacterIndex("PearlGuardMan_1") != -1)
	{
		chr = CharacterFromID("PearlGuardMan_1");
		ChangeCharacterAddressGroup(chr, "Pearl_Jungle_02", "reload", "reload3_back");
	}

	chr = CharacterFromID("Cristobal Kuvalda");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	SetQuestFunction("DestinyGiftInPearlVillage", "DestinyGiftInPearlVillage", "l", "Pearl_town_1");
}

// Штурм самого посёлка: пятнадцать против ГГ, Кувалды и десятка защитников.
void DestinyGiftInPearlVillage(string qName)
{
	DeleteAllFantomCharactersFromLocation(&Locations[FindLocation("Pearl_town_1")]);

	ref chr;
	if (GetCharacterIndex("PearlGuardMan_1") != -1)
	{
		chr = CharacterFromID("PearlGuardMan_1");
		ChangeCharacterAddress(chr, "None", "");
	}

	chrDisableReloadToLocation = true;

	chr = CharacterFromID("Cristobal Kuvalda");
	ChangeCharacterAddressGroup(chr, "Pearl_town_1", "reload", "reload2");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	int iRank = sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE;
	int iN;
	string sModel;
	string sSpot;
	for (iN = 1; iN <= 15; iN++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("DestinyGift PearlVillage Attack " + iN, "Canib_" + (rand(5)+1), "man", "man", iRank, PIRATE, 0, false));
		sSpot = "goto" + iN;
		ChangeCharacterAddressGroup(chr, "Pearl_town_1", "goto", sSpot);
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}
	for (iN = 1; iN <= 10; iN++)
	{
		sModel = "pirate_" + (rand(9)+1);
		if (rand(1) == 1) sModel = "officer_" + (rand(19)+1);
		chr = GetCharacter(NPC_GenerateCharacter("DestinyGift PearlVillage Defender " + iN, sModel, "man", "man", iRank, PIRATE, 0, false));
		sSpot = "goto" + (iN + 15);
		ChangeCharacterAddressGroup(chr, "Pearl_town_1", "goto", sSpot);
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("DestinyGiftInPearlVillageWinner", "DestinyGiftInPearlVillageWinner", "DestinyGift PearlVillage Attack ", 15);
}

void DestinyGiftInPearlVillageWinner(string qName)
{
	ref chr = CharacterFromID("Cristobal Kuvalda");
	chr.Dialog.CurrentNode = "3";
	SetDialogCharacter2Character(PChar, chr, 20.0, false);
}

void DestinyGiftInPearlVillageToOfficerHouse()
{
	chrDisableReloadToLocation = false;
	PChar.DestinyGift = "17";
	SetQuestFunction("DestinyGiftPreToPearlVillageToOfficerHouse", "DestinyGiftPreToPearlVillageToOfficerHouse", "l", "PearlTown1_Townhall");
}

void DestinyGiftPreToPearlVillageToOfficerHouse(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_SetActorType(PChar);
	DoQuestFunctionDelay("DestinyGiftToPearlVillageToOfficerHouse2", 0.1);
}

// В ратуше главу посёлка уже ждёт именной убийца.
void DestinyGiftToPearlVillageToOfficerHouse2(string _tmp)
{
	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, -4, 2, -1, false);

	ref rHead = CharacterFromID("HeadMan_1");
	ChangeCharacterAddressGroup(rHead, "PearlTown1_Townhall", "goto", "goto5");
	LAi_SetWarriorType(rHead);
	LAi_group_MoveCharacter(rHead, LAI_GROUP_PLAYER);

	LAi_ActorTurnToCharacter(PChar, rHead);

	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	ref chr = GetCharacter(NPC_GenerateCharacter("DestinyGift HeadMan Killer", "Canib_Boss", "man", "man", iRank, PIRATE, 0, false));
	LAi_SetWarriorType(chr);

	FantomMakeCoolFighter(chr, iRank, 10, 10, BLADE_LONG, "pistol6", 100);
	SetSelfSkill(chr, 90, 85, 70, 75, 70);
	LAi_SetHP(chr, 700.0, 700.0);

	ChangeCharacterAddressGroup(chr, "PearlTown1_Townhall", "goto", "goto2");

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.DestinyGiftToPearlVillageToOfficerHouse.win_condition.l1 = "NPC_Death";
	PChar.quest.DestinyGiftToPearlVillageToOfficerHouse.win_condition.l1.character = "DestinyGift HeadMan Killer";
	PChar.quest.DestinyGiftToPearlVillageToOfficerHouse.function = "DestinyGiftToPearlVillageToOfficerHouse";
	DoQuestFunctionDelay("DestinyGiftToPearlVillageToOfficerHouseCameraFollow", 2.5);
}

void DestinyGiftToPearlVillageToOfficerHouseCameraFollow(string _tmp)
{
	LAi_SetPlayerType(PChar);
	locCameraFollow();
}

void DestinyGiftToPearlVillageToOfficerHouse(string qName)
{
	ref chr = CharacterFromID("HeadMan_1");
	chr.Dialog.CurrentNode = "47";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

// Gregg: ==> ЗДЕСЬ ОРИГИНАЛ ДАРИЛ ЛЮГГЕР.
// К:ПС выдавал ГГ люггер «Броненосец» с полным экипажем и боезапасом - потому что
// в оригинале игрок к этому месту приходил вовсе без корабля и вернуть было нечего.
// У нас возвращается СВОЙ флагман: эскадра дождалась хозяина, а корабль за время его
// отсутствия починили и снарядили за счёт корабельной казны. Эскорты не возвращаем -
// у игрока эскадры может не быть вовсе, а бой с Ферегуэйро задуман поединком.
void DestinyGiftNeedKillFeregueyro()
{
	chrDisableReloadToLocation = false;
	PChar.DestinyGift = "18";
	// Gregg: запись выдаётся ЗДЕСЬ, а не после победы: игрок должен выйти из ратуши,
	// зная, где стоит корабль и кого искать. В оригинале запись висела на победе.
	BO_PearlRecord("10");

	ref rHead = CharacterFromID("HeadMan_1");
	LAi_SetStayType(rHead);
	LAi_group_MoveCharacter(rHead, "PearlGroup_1");
	ChangeCharacterAddressGroup(rHead, "PearlTown1_Townhall", "barmen", "bar1");
	// Gregg: Кувалду не просто убираем с карты, как делал оригинал, а распускаем начисто.
	// Иначе он остаётся в сейве навсегда: бессмертный (LAi_SetImmortal при создании -
	// а это ещё и полная неуязвимость корабля, см. AIShip.c:3760) и несносимый (NotRemoved).
	ref rKuvalda = CharacterFromID("Cristobal Kuvalda");
	LAi_SetImmortal(rKuvalda, false);
	if (CheckAttribute(rKuvalda, "NotRemoved")) DeleteAttribute(rKuvalda, "NotRemoved");
	rKuvalda.LifeDay = 0;
	SaveCurrentNpcQuestDateParam(rKuvalda, "LifeTimeCreate");
	ChangeCharacterAddress(rKuvalda, "None", "");

	BO_PearlLockOff();
	BO_RestockFlagship();
	SetCharacterShipLocation(PChar, "Shore9");

	// Gregg: матрос у сходней. В К:ПС корабль просто возникал у берега, и как команда
	// нашла выброшенного на Мэйн капитана, не объяснялось ничем. Он и объясняет:
	// анонимное письмо через курьера в Белизе. Кто послал - не знает никто.
	// Gregg: модель и приветствие берём существующие. Моделей sailor_N в CSP нет вовсе,
	// а Gr_Sailor нет среди алиасов приветствий - и то и другое я сперва выдумал.
	// Имя не задаём: это рядовой матрос, генератор даст своё.
	ref rSailor = GetCharacter(NPC_GenerateCharacter("DestinyGift Sailor", "pirate_" + (rand(9) + 1), "man", "man", 5, PIRATE, -1, false));
	rSailor.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Sailor.c";
	rSailor.Dialog.CurrentNode = "1";
	rSailor.greeting = "Gr_MiddPirate";
	rSailor.NotRemoved = true;
	LAi_SetStayType(rSailor);
	LAi_SetImmortal(rSailor, true);
	LAi_group_MoveCharacter(rSailor, LAI_GROUP_PLAYER);

	// Gregg: адрес не выставляем - его выдаст встречающий узел. Стоящий у скалы
	// матрос теряется на фоне бухты, поэтому он не ждёт, а подходит сам.
	SetQuestFunction("DestinyGiftSailorMeet", "DestinyGiftSailorMeet", "l", "Shore9");
	setWDMPointXZ("Shore9");
	BO_SetEncounters("Shore50", false);

	PChar.quest.DestinyGiftNeedKillFeregueyroGood.win_condition.l1 = "Character_Capture";
	PChar.quest.DestinyGiftNeedKillFeregueyroGood.win_condition.l1.character = "Feregueyro";
	PChar.quest.DestinyGiftNeedKillFeregueyroGood.function = "DestinyGiftNeedKillFeregueyroGood";

	// Gregg: оригинал засчитывал победу ТОЛЬКО за абордаж, а утопить Ферегуэйро пушками
	// никто не мешает - и тогда квест вставал намертво. Хуже того, вторая часть Меритора
	// выдаётся в его каюте (BO_Feregueyro.c, узел 35), и вместе с ним она уходила бы на дно,
	// обрывая всю трилогию. Дублируем условие на потопление и достаём часть из обломков.
	PChar.quest.DestinyGiftFeregueyroSink.win_condition.l1 = "Character_sink";
	PChar.quest.DestinyGiftFeregueyroSink.win_condition.l1.character = "Feregueyro";
	PChar.quest.DestinyGiftFeregueyroSink.function = "DestinyGiftFeregueyroSink";

	ref sld = CharacterFromID("Feregueyro");
	// Gregg: корабль по классу флагмана игрока - см. BO_FeregueyroShip
	FantomMakeCoolSailor(sld, BO_FeregueyroShip(), iTxt("BO_FeregueyroShipName"), CANNON_TYPE_CULVERINE_LBS24, 70, 70, 70);
	BO_SetSeaPerks(sld);
	// Gregg: наземное бессмертие ставилось для разговора на Shore50, и с берега он уже
	// убран - снимаем. В каюте его снимет QuestAboardCabinDialogFree, но только с КЛОНА
	// (QuestsUtilite.c:2575), а исходный персонаж иначе так и остаётся бессмертным в сейве.
	LAi_SetImmortal(sld, false);
	// Gregg: без AlwaysEnemy отношение стирается при загрузке корабля в море.
	// DontRansackCaptain и AlwaysSandbankManeuver уже поставил FantomMakeCoolSailor.
	sld.AlwaysEnemy = true;
	sld.AnalizeShips = true;
	sld.CanCrewDamage = true;

	Group_FindOrCreateGroup("FeregueyroGroup");
	Group_SetTaskAttackInMap("FeregueyroGroup", PLAYER_GROUP);
	Group_LockTask("FeregueyroGroup");
	Group_AddCharacter("FeregueyroGroup", "Feregueyro");
	Group_SetGroupCommander("FeregueyroGroup", "Feregueyro");

	// Gregg: не торговец-челнок, как в оригинале, а ДОГОНЯЮЩИЙ охотник: он ищет ГГ сам,
	// и пропустить его почти невозможно. Из-за этого ушёл весь маятник с перекладкой
	// курса каждые 11 суток - он был нужен только чтобы челнок хоть иногда попадался.
	// type = "war" лидеру ОБЯЗАТЕЛЕН, иначе GenerateMapEncounter_Alone не спавнит
	// энкаунтер вовсе (грабля выловлена на ГВИК, Addons\utils.c:1966).
	sld.mapEnc.type = "war";
	sld.mapEnc.worldMapShip = "quest_ship";
	sld.mapEnc.Name = iTxt("BO_FeregueyroName");

	// Срок -1: охота не истекает, пока его не возьмут.
	// Gregg: первый аргумент для типа warrior не читается вовсе - догоняющий выходит
	// рядом с игроком (worldmap_events.c:119, вызов по beginlocator закомментирован).
	// Оставлен только ради читаемости: обещание главы посёлка про лагуну Каратаска
	// движок не исполняет.
	Map_CreateWarrior("Shore10", "Feregueyro", -1);
}

// Gregg: матрос выходит навстречу, когда ГГ впервые сходит на берег в бухте.
// Тот же приём, что у посланцев главы 6: стоящего в стороне NPC игрок просто
// не замечает.
void DestinyGiftSailorMeet(string qName)
{
	// Gregg: блок переходов тут НЕ ставим. Разговор необязательный, выходной функции
	// у диалога нет, и снимать флаг было бы некому - игрок остался бы заперт в бухте.
	ref chr = CharacterFromID("DestinyGift Sailor");
	ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");
	SetDialogCharacter2Character(PChar, chr, 25.0, false);
	// LEO: и сразу заводим его проводы. Матрос рождается постоянным и с NotRemoved, разговор
	//   необязательный, выходной функции у диалога нет - снять его было НЕКОМУ, и он навсегда
	//   оставался стоять на переходе в джунгли. Уход по выходу из бухты покрывает обе ветки:
	//   и когда игрок поговорил, и когда прошёл мимо.
	SetQuestFunction("DestinyGiftSailorLeave", "DestinyGiftSailorLeave", "efl", "Shore9");
}

// LEO: доклад сделан, бухту покидаем - матрос уходит вместе с командой. Порядок тот же, каким
//   выше отправляют Кувалду: снять бессмертие и NotRemoved, погасить LifeDay, убрать адрес.
void DestinyGiftSailorLeave(string qName)
{
	if (GetCharacterIndex("DestinyGift Sailor") == -1) return;
	ref chr = CharacterFromID("DestinyGift Sailor");
	LAi_SetImmortal(chr, false);
	DeleteAttribute(chr, "NotRemoved");
	chr.LifeDay = 0;
	SaveCurrentNpcQuestDateParam(chr, "LifeTimeCreate");
	ChangeCharacterAddress(chr, "None", "");
}

void DestinyGiftNeedKillFeregueyroGood(string qName)
{
	PChar.DestinyGift = "19";
	BO_PearlRecord("11");

	Group_DeleteGroup("FeregueyroGroup");
	// Gregg: маятник глушим целиком - и таймер, и сам карт-энкаунтер. Оригинал этого
	// не делал: метка на глобалке оставалась висеть после победы.
	Map_ReleaseQuestEncounter("Feregueyro");
	PChar.quest.DestinyGiftFeregueyroSink.over = "yes";
	PChar.quest.DestinyGiftNeedKillFeregueyroGood.over = "yes";
}

// Ферегуэйро потоплен без абордажа - засчитываем и выдаём часть Меритора из обломков.
// Gregg: ЗАГЛУШКА СОВМЕСТИМОСТИ, к логике главы отношения не имеет.
// До коммита 1fa3450c Ферегуэйро был челноком-торговцем, и его вёл узел
// FeregueyroQuestCheckEncounter со взведённым .again - то есть не гаснущий никогда.
// Переделка на догоняющего охотника функцию удалила, а в сейвах, сделанных между
// cac27cf7 и 1fa3450c, узел остался и зовёт несуществующее имя по нескольку раз
// в секунду. Гасим его при первом же вызове. Убрать, когда старые сейвы станут
// неактуальны.
void FeregueyroQuestCheckEncounter(string qName)
{
	PChar.quest.FeregueyroQuestCheckEncounter.over = "yes";
}

void FeregueyroQuestCheckEncounterStartTimer(string qName)
{
	PChar.quest.FeregueyroQuestCheckEncounterStartTimer.over = "yes";
}

void DestinyGiftFeregueyroSink(string qName)
{
	// Gregg: ПРОВАЛ. Вторая часть Меритора лежала у Ферегуэйро в каюте, и вместе
	// с кораблём ушла на дно. Раньше мы её оттуда «вылавливали», чтобы не оставить
	// игрока в тупике, - решение пользователя: пусть линия честно проваливается.
	PChar.DestinyGift = "FAIL";
	BO_PearlRecord("25");
	BO_QuestFailed("DestinyGift");

	PChar.quest.DestinyGiftNeedKillFeregueyroGood.over = "yes";
	PChar.quest.DestinyGiftFeregueyroSink.over = "yes";
	Group_DeleteGroup("FeregueyroGroup");
	Map_ReleaseQuestEncounter("Feregueyro");
}

// Свобода. Глава посёлка отдаёт третью часть Меритора и отпускает.
void DestinyGiftExitPearlVillage()
{
	ref chr = CharacterFromID("HeadMan_1");
	chr.dialog.Filename = "Pearl_dialog.c";
	chr.dialog.currentnode = "HeadMan";
	chr.quest.meeting = "1";
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "PearlGroup_1");
	ChangeCharacterAddressGroup(chr, "PearlTown1_Townhall", "goto", "goto5");
	LAi_SetImmortal(chr, false);

	// Gregg: снимаем то, что навесил BO_EnsurePearlHead на время акта - иначе глава
	// останется несносимым фантомом навсегда, хотя срок жизни ему уже вернули.
	if (CheckAttribute(chr, "NotRemoved")) DeleteAttribute(chr, "NotRemoved");
	chr.LifeDay = 30;
	SaveCurrentNpcQuestDateParam(chr, "LifeTimeCreate");

	// Gregg: только теперь, когда возвращаться в посёлок больше не надо, безопасно
	// вернуть штатный генератор грабежа промысла - см. BO_PearlGenRestore.
	BO_PearlGenRestore();

	// Gregg: и дверь дома Кендарта возвращаем на штатную заглушку - акт кончился,
	// ГГ здесь больше не живёт
	int iTown = FindLocation("Pearl_town_1");
	if (iTown >= 0) Locations[iTown].reload.l2.go = "CommonHut";

	PChar.DestinyGift.PearlVillageLive = false;
	PChar.DestinyGift = "20";
	BO_PearlRecord("12");
	// Gregg: подсказку про Бермуды даём здесь, а не после разговора с трактирщиком:
	// иначе между «путь открыт» и следующим шагом у игрока нет вообще никакого указания,
	// куда идти, а сама запись приходит уже после того, как Левинсон найден.
	BO_PearlRecord("13");
}
// Gregg: <== глава 5, поселенческий акт

// Gregg: ==> глава 5, вторая половина: Меритор по частям
// Левинсон на Бермудах (четвёртая часть), Фернандо Эстикос и остов «Стервятника»
// (пятая), абордаж галеона Тюльпана (шестая), Виллемстад (седьмая) и Теркс.

// Gregg: бухта с остовом «Стервятника» заводится штатно, в InitLocations, а дорога
// к ней - третья тропа развилки у ворот Дес-Мойнеса (DesMoines.c). Раньше и локацию,
// и дверь ставила рантайм-функция BO_EnsureWreckShore: у старой модели выхода третьей
// тропы не было, и мыс вешали на свободную дверь домика в джунглях.
// Левинсон всё ещё на Бермудах. Бермудский трактирщик вспоминает постояльца
// и отдаёт комнату наверху.
void DestinyGiftFindLevinson()
{
	PChar.DestinyGift = "21";

	// Gregg: лестница наверх в Pirates_tavern закрыта штатно (Bermudes.c:173,
	// «закроем, но связку сделаем для квестовых нужд») - в CSP её не открывает никто,
	// и без этой строки до Левинсона просто не дойти. Закрываем обратно после боя.
	LocatorReloadEnterDisable("Pirates_tavern", "reload2_back", false);

	ref chr = CharacterFromID("Frederik Levinson");
	chr.dialog.currentNode = "28";
	// Gregg: в К:ПС он прятался в Pirates_townhall_room - отдельной комнате при
	// резиденции, которой в CSP нет. Сажаем в съёмную комнату таверны: она есть,
	// прятаться там логичнее, и не приходится плодить локацию. goto3, а не goto1:
	// goto1 занят и штатной арендой комнаты, и квестом «Чёрных парусов».
	ChangeCharacterAddressGroup(chr, "Pirates_tavern_upstairs", "goto", "goto3");
	LAi_SetStayType(chr);
	chr.NotRemoved = true;
	if (CheckAttribute(chr, "LifeDay")) DeleteAttribute(chr, "LifeDay");

	SetQuestFunction("DestinyGiftFindLevinsonGood", "DestinyGiftFindLevinsonGood", "l", "Pirates_tavern_upstairs");
}

void DestinyGiftFindLevinsonGood(string qName)
{
	chrDisableReloadToLocation = true;

	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	SetDialogCharacter2Character(PChar, CharacterFromID("Frederik Levinson"), 10.0, true);
}

void DestinyGiftBattleWithLevinson()
{
	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;

	ref chr = CharacterFromID("Frederik Levinson");
	LAi_SetWarriorType(chr);
	LAi_SetPlayerType(PChar);
	LAi_SetImmortal(chr, false);
	LAi_SetHP(chr, 500.0, 500.0);

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.DestinyGiftBattleWithLevinsonWinner.win_condition.l1 = "NPC_Death";
	PChar.quest.DestinyGiftBattleWithLevinsonWinner.win_condition.l1.character = "Frederik Levinson";
	PChar.quest.DestinyGiftBattleWithLevinsonWinner.function = "DestinyGiftBattleWithLevinsonWinner";
}

void DestinyGiftBattleWithLevinsonWinner(string qName)
{
	// Gregg: часть выдаём прямо, а не через выпадение из трупа - в оригинале оба
	// варианта были написаны, но рабочим оставили этот, второй закомментирован.
	TakeNItems(PChar, "MysteriousArtefact_MeritorChapter_4", 1);

	chrDisableReloadToLocation = false;
	// Gregg: возвращаем лестницу в штатное закрытое состояние - наверху больше нечего делать
	LocatorReloadEnterDisable("Pirates_tavern", "reload2_back", true);
	PChar.DestinyGift = "22";
	BO_PearlRecord("14");
}

// Пятая часть у Бернальдинио Торангера. Мэр Джекман наводит на его компаньона.
void DestinyGiftFindFiveMeritorChapter()
{
	PChar.DestinyGift = "23";
	// Gregg: у оригинала на этом шаге записи не было, и всё, что рассказал Джекмен,
	// сваливалось в запись про смерть Левинсона - то есть игрок читал совет раньше,
	// чем его получал. Разнесли на два шага.
	BO_PearlRecord("15");
}

// Лавочник Портобелло вспоминает Фернандо Эстикоса - тот сидит в местной таверне.
void DestinyGiftToPortoBelloTavern()
{
	PChar.DestinyGift = "24";
	BO_PearlRecord("16");

	ref chr = GetCharacter(NPC_GenerateCharacter("Fernando Estikos", "officer_12", "man", "man", 1, PIRATE, -1, true));
	chr.name = iTxt("BO_EstikosName");
	chr.lastname = iTxt("BO_EstikosLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Estikos.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_Tavern_Mate";
	chr.NotRemoved = true;
	LAi_SetSitType(chr);
	LAi_SetImmortal(chr, true);

	FreeSitLocator("PortoBello_tavern", "sit_base2");
	ChangeCharacterAddressGroup(chr, "PortoBello_tavern", "sit", "sit_base2");
	SetQuestMarkToCharacter(chr, true);
}

void DestinyGiftInPortoBelloTavern()
{
	ref chr = CharacterFromID("Fernando Estikos");
	chr.Dialog.CurrentNode = "2";

	// Gregg: сажали Эстикоса из лавки, когда таверна была выгружена, - сгонять с sit_base2
	// там было некого, а посетителей на это место расставляет уже генератор при входе.
	// Повторяем освобождение здесь, чтобы Фернандо не стоял в чужом посетителе.
	BO_FreeLocator("PortoBello_tavern", "sit_base2", "Fernando Estikos");
	ChangeCharacterAddressGroup(chr, "PortoBello_tavern", "sit", "sit_base2");

	// Gregg: BO_FreeLocator вместо FreeLocator из К:ПС - он же чистит адрес согнанного
	// и умеет никого не трогать по исключению (третий аргумент)
	BO_FreeLocator("PortoBello_tavern", "sit_front2", "");
	ChangeCharacterAddressGroup(PChar, "PortoBello_tavern", "sit", "sit_front2");
	LAi_SetSitType(PChar);
	LAi_SetActorType(chr);
	LAi_ActorSetSitMode(chr);
	LAi_ActorDialogNow(chr, PChar, "", -1);
}

void DestinyGiftFindBernaldinioShip()
{
	ChangeCharacterAddressGroup(PChar, "PortoBello_tavern", "goto", "goto4");
	LAi_SetPlayerType(PChar);
	LAi_SetSitType(CharacterFromID("Fernando Estikos"));

	PChar.DestinyGift = "25";
	BO_PearlRecord("17");

	PChar.GenQuestBox.Shore_ship3 = true;
	PChar.GenQuestBox.Shore_ship3.box1.items.MysteriousArtefact_MeritorChapter_5 = 1;
	PChar.GenQuestBox.Shore_ship3.box1.items.DestinyGiftBernaldinioNotice = 1;
	PChar.GenQuestBox.Shore_ship3.stay = true;

	// Фернандо своё отыграл и уходит вместе с ближайшей чисткой фантомов
	ref rEst = CharacterFromID("Fernando Estikos");
	if (CheckAttribute(rEst, "NotRemoved")) DeleteAttribute(rEst, "NotRemoved");
	LAi_SetImmortal(rEst, false);
	SetQuestMarkToCharacter(rEst, false);
	rEst.LifeDay = 0;
	SaveCurrentNpcQuestDateParam(rEst, "LifeTimeCreate");

	SetQuestFunction("DestinyGiftFindedBernaldinioShip", "DestinyGiftFindedBernaldinioShip", "l", "Shore_ship3");
}

void DestinyGiftFindedBernaldinioShip(string qName)
{
	chrDisableReloadToLocation = true;
	BO_PearlRecord("18");

	PChar.quest.DestinyGiftFindFiveChapterMeritorGood.win_condition.l1 = "item";
	PChar.quest.DestinyGiftFindFiveChapterMeritorGood.win_condition.l1.item = "MysteriousArtefact_MeritorChapter_5";
	PChar.quest.DestinyGiftFindFiveChapterMeritorGood.win_condition.l2 = "item";
	PChar.quest.DestinyGiftFindFiveChapterMeritorGood.win_condition.l2.item = "DestinyGiftBernaldinioNotice";
	PChar.quest.DestinyGiftFindFiveChapterMeritorGood.function = "DestinyGiftFindFiveChapterMeritorGood";
}

// Письмо Бернальдинио выводит на шестую часть - она у Тюльпана, и тот уже ищет ГГ.
void DestinyGiftFindFiveChapterMeritorGood(string qName)
{
	ClearQuestBoxStay("Shore_ship3");
	chrDisableReloadToLocation = false;
	BO_PearlRecord("19");

	// Gregg: оригинал ставил Тюльпана на островные корабельные локаторы Маракайбо
	// и ловил игрока условием на морскую локацию. У нас он выходит из Кюрасао
	// догоняющим квест-энкаунтером - охотник сам ищет ГГ, а не ждёт его в одной точке.
	// Образец - ГВИК-перехватчики (Addons\utils.c:809): группа с задачей атаки на карте
	// плюс Map_CreateWarrior от ареала нужного города.
	ref sld = CharacterFromID("Tulip");
	Group_FindOrCreateGroup("TulipShip");
	Group_SetType("TulipShip", "pirate");
	Group_SetTaskAttackInMap("TulipShip", PLAYER_GROUP);
	Group_LockTask("TulipShip");
	Group_AddCharacter("TulipShip", "Tulip");
	Group_SetGroupCommander("TulipShip", "Tulip");

	// Gregg: type = "war" лидеру ОБЯЗАТЕЛЕН - без него GenerateMapEncounter_Alone
	// энкаунтер не спавнит вовсе (грабля выловлена на ГВИК, Addons\utils.c:1966)
	sld.mapEnc.type = "war";
	sld.mapEnc.worldMapShip = "quest_ship";
	sld.mapEnc.Name = iTxt("BO_TulipShipName");
	sld.AlwaysEnemy = true;
	sld.CanCrewDamage = true;

	// Gregg: наземное бессмертие ему ставили ещё в главе 4, для сцены в таверне Ла-Веги.
	// Снимаем: LAi_IsImmortal проверяется в AIShip.c (3655/3760/3800) и делает корабль
	// НЕУЯЗВИМЫМ - ровно та же грабля, что была с Ферегуэйро.
	LAi_SetImmortal(sld, false);
	// и подтягиваем бойца: держатель части Меритора на тяжёлом галеоне не может быть
	// слабее рядового капитана. В главе 4 он заводился под сцену разговора, не под бой.
	// Gregg: blade23 - бретта. Фламберж (blade32) для него слишком жирно:
	// это фиолетовое уникальное оружие, а Тюльпан - крепкий капитан, не более.
	FantomMakeCoolFighter(sld, sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE, 60, 55, "blade23", "pistol6", 250);
	// Gregg: шестая часть Меритора лежит на самом Тюльпане, а не выдаётся с неба
	// по факту победы: её снимают с тела. SaveItemsForDead обязателен, иначе труп
	// обчистит штатный уборщик. Подстраховка на случай, если игрок тело не обыщет,
	// стоит в DestinyGiftBattleSeaWithTulipWinner.
	TakeNItems(sld, "MysteriousArtefact_MeritorChapter_6", 1);
	sld.SaveItemsForDead = true;
	SetSelfSkill(sld, 90, 85, 80, 75, 70);
	BO_SetSeaPerks(sld);

	PChar.quest.DestinyGiftBattleSeaWithTulip.win_condition.l1 = "MapEnter";
	PChar.quest.DestinyGiftBattleSeaWithTulip.function = "DestinyGiftBattleSeaWithTulip";

	PChar.quest.DestinyGiftBattleSeaWithTulipWinner.win_condition.l1 = "Character_Capture";
	PChar.quest.DestinyGiftBattleSeaWithTulipWinner.win_condition.l1.character = "Tulip";
	PChar.quest.DestinyGiftBattleSeaWithTulipWinner.function = "DestinyGiftBattleSeaWithTulipWinner";

	// Gregg: та же беда, что была у Ферегуэйро: Character_Capture засчитывается только
	// за абордаж (quests_check.c:234, Killer.status == KILL_BY_ABORDAGE), а утопить
	// Тюльпана пушками никто не мешает. Шестая часть Меритора выдаётся ровно здесь
	// и больше нигде - без этой ветки глава вставала бы намертво на состоянии «25».
	PChar.quest.DestinyGiftTulipSink.win_condition.l1 = "Character_sink";
	PChar.quest.DestinyGiftTulipSink.win_condition.l1.character = "Tulip";
	PChar.quest.DestinyGiftTulipSink.function = "DestinyGiftTulipSink";
}

// Игрок вышел на глобалку - «Тюльпан» снимается с якоря в Кюрасао и идёт за ним.
void DestinyGiftBattleSeaWithTulip(string qName)
{
	// Gregg: записи тут НЕТ сознательно. Она приходила в момент спауна на глобалке,
	// то есть ГГ «узнавал» о Тюльпане раньше, чем мог его увидеть, да ещё и «слух
	// с Кюрасао» посреди открытого моря. Про шестую часть у Тюльпана игрок и так
	// знает из письма Бернальдинио, а встречу отметит запись о победе.

	// Gregg: запирания острова, как в оригинале, тут больше нет и не нужно - охотник
	// догоняющий, уйти от него на берег всё равно значит только отложить встречу.
	// Gregg: срок -1, как у Ферегуэйро. Узел сидит на MapEnter без .again, то есть
	// отрабатывает один раз и стирается: с конечным сроком не догнавший за него игрок
	// терял охотника навсегда, а вместе с ним и шестую часть Меритора.
	// Первый аргумент для типа warrior не читается вовсе (worldmap_events.c:119) -
	// догоняющий выходит рядом с игроком, откуда бы его ни «выпускали».
	Map_CreateWarrior("Villemstad_town", "Tulip", -1);
}

void DestinyGiftTulipSink(string qName)
{
	// Gregg: ПРОВАЛ по той же причине - шестая часть Меритора была на борту.
	PChar.DestinyGift = "FAIL";
	BO_PearlRecord("26");
	BO_QuestFailed("DestinyGift");

	PChar.quest.DestinyGiftBattleSeaWithTulipWinner.over = "yes";
	PChar.quest.DestinyGiftTulipSink.over = "yes";
	Group_DeleteGroup("TulipShip");
	Map_ReleaseQuestEncounter("Tulip");
}

void DestinyGiftBattleSeaWithTulipWinner(string qName)
{
	BO_PearlRecord("20");
	// Gregg: часть уже могла быть снята с тела - см. выдачу при создании Тюльпана.
	// Здесь только подстраховка, если игрок тело обыскать не догадался.
	if (!CheckCharacterItem(PChar, "MysteriousArtefact_MeritorChapter_6"))
	{
		TakeNItems(PChar, "MysteriousArtefact_MeritorChapter_6", 1);
	}

	// Gregg: в оригинале тут сносили группу "Tulip", которой не существует -
	// группа называется TulipShip. Иначе она оставалась висеть в сейве.
	Group_DeleteGroup("TulipShip");
	// наземное бессмертие Тюльпану больше не нужно, а оно ещё и корабль бережёт
	// от урона (AIShip.c:3760) - снимаем, пока он не уплыл неубиваемым
	LAi_SetImmortal(CharacterFromID("Tulip"), false);

	// Gregg: карт-энкаунтер снимаем, иначе метка охотника останется висеть на глобалке.
	// А вот разлочки острова и глобальной карты тут БОЛЬШЕ НЕТ: с переходом Тюльпана
	// в догоняющие их никто не ставит, и сброс этих общих на весь мод флагов мог
	// отпереть то, что заперто чужим квестом.
	Map_ReleaseQuestEncounter("Tulip");
	PChar.quest.DestinyGiftTulipSink.over = "yes";
	PChar.quest.DestinyGiftBattleSeaWithTulipWinner.over = "yes";

	// Gregg: в оригинале простой был 30 суток. Неделя - решение по переносу:
	// месяц реального ожидания в этом месте лишний.
	SetTimerFunction("DestinyGiftSeeCuracao", 0, 0, 7);
}

void DestinyGiftSeeCuracao(string qName)
{
	BO_PearlRecord("21");
	PChar.DestinyGift = "26";

	// Gregg: цену части ставим уже здесь, а не по приходу в Виллемстад. Метку лоточнику
	// вешает генератор горожан, а он отрабатывает РАНЬШЕ квестового узла на вход
	// в локацию - иначе на первом заходе части в коробе не было бы вовсе.
	// IsQuestUsedItem (items_utilite.c:280) считает квестовым всё, у чего цена ноль,
	// а торговый интерфейс такое молча пропускает (itemstrade.c:271). Ноль вернём
	// в момент покупки, в DestinyGiftFullMeritor, и часть снова станет непродаваемой.
	int iPart = FindItem("MysteriousArtefact_MeritorChapter_7");
	if (iPart >= 0) Items[iPart].price = BO_MERITOR_PRICE;
	SetQuestFunction("DestinyGiftInCuracao", "DestinyGiftInCuracao", "l", "Villemstad_town");
}

void DestinyGiftInCuracao(string qName)
{
	BO_PearlRecord("22");
	PChar.DestinyGift = "27";
	SetQuestFunction("DestinyGiftFullMeritor", "DestinyGiftFullMeritor", "i", "MysteriousArtefact_MeritorChapter_7");
}

// Семь частей на руках - Меритор собран и указывает на Теркс.
void DestinyGiftFullMeritor(string qName)
{
	// Gregg: часть куплена - возвращаем ноль, чтобы её нельзя было перепродать
	// и чтобы она не осела товаром у прочих лоточников
	int iPart = FindItem("MysteriousArtefact_MeritorChapter_7");
	if (iPart >= 0) Items[iPart].price = 0;

	int m;
	string sPart;
	for (m = 1; m <= 7; m++)
	{
		sPart = "MysteriousArtefact_MeritorChapter_" + m;
		TakeNItems(PChar, sPart, -1);
	}
	TakeNItems(PChar, "MysteriousArtefact_Meritor", 1);

	BO_PearlRecord("23");
	PChar.DestinyGift = "28";

	SetQuestFunction("DestinyGiftToTerks", "DestinyGiftToTerks", "l", "Terks_Grot");
	// Gregg: .again - чтобы узел пережил отложенный вход, см. DestinyGiftToTerks.
	// Снимается там же присвоением over (штатная идиома, quests_check.c:453).
	PChar.quest.DestinyGiftToTerks.again = true;
}

// Грот Теркса. Кендарт ждал здесь.
void DestinyGiftToTerks(string qName)
{
	// Gregg: грот на Терксе делим с «Нежданным наследством»: оно ставит своего
	// бессмертного скелета на те же локаторы monsters (reaction_functions.c:3408).
	// Пока он там - финал откладываем, узел взведён с .again и отработает на
	// следующем заходе. Иначе ролик с Кендартом шёл бы посреди чужого боя.
	int iSkel = GetCharacterIndex("UI_skel1");
	if (iSkel >= 0)
	{
		if (CheckAttribute(&characters[iSkel], "location"))
		{
			if (characters[iSkel].location == "Terks_Grot") return;
		}
	}
	PChar.quest.DestinyGiftToTerks.over = "yes";

	chrDisableReloadToLocation = true;
	PChar.DestinyGift = "29";

	TakeNItems(PChar, "MysteriousArtefact_Meritor", -1);
	TakeNItems(PChar, "DestinyGiftBernaldinioNotice", -1);

	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	// Gregg: модель CCS_Mechanic вместо Kendart из К:ПС - она уже лежит в ресурсах CSP
	// вместе с вариантами одежды, и лицо ей прописано (characters_face.c:449, FaceId 295).
	// Анимации man2 в CSP нет, везде man.
	ref chr = GetCharacter(NPC_GenerateCharacter("Alehon Kendart", "CCS_Mechanic", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_KendartName");
	chr.lastname = iTxt("BO_KendartLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart.c";
	chr.Dialog.CurrentNode = "1";
	chr.NotRemoved = true;
	chr.greeting = "Gr_Master";
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	ChangeCharacterAddressGroup(chr, "Terks_Grot", "monsters", "monster4");

	SetDialogCharacter2Character(PChar, chr, 15.0, true);
}

// Gregg: ==> ЗДЕСЬ ОРИГИНАЛ ДАВАЛ БЕССМЕРТИЕ НА ДВА МЕСЯЦА.
// У нас Чёрная Орхидея - оберег, а не таймер неуязвимости: экипируемый предмет
// со снижением урона и одним спасением от смертельного удара (см. BlackOrchid_IsWorn
// и BlackOrchid_LastStand в начале файла). Поэтому вместо LAi_SetImmortal выдаём
// сам предмет и сразу надеваем его - финальная драка идёт уже под его защитой.
void DestinyGiftFinalBattle()
{
	ChangeCharacterAddress(CharacterFromID("Alehon Kendart"), "None", "");

	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;
	LAi_SetPlayerType(PChar);

	TakeNItems(PChar, "BlackOrchid", 1);
	EquipCharacterByItem(PChar, "BlackOrchid");
	Log_Info(iTxt("BO_OrchidTaken"));

	// Gregg: в оригинале сюда вываливалось семеро на ранг игрока плюс десять,
	// и все из одной точки - выходила толпа, а не финал. Четверо и помягче по рангу,
	// вразброс по гроту: трое на локаторах monsters, четвёртый от входа.
	int iRank = sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE;
	int iN;
	ref chr;
	string sGroup;
	string sSpot;
	for (iN = 1; iN <= 4; iN++)
	{
		sGroup = "monsters";
		sSpot = "monster" + iN;
		if (iN == 4)
		{
			sGroup = "reload";
			sSpot = "reload1_back";
		}
		chr = GetCharacter(NPC_GenerateCharacter("DestinyGiftFinalBattle " + iN, "Canib_" + (rand(5)+1), "man", "man", iRank, PIRATE, 0, false));
		ChangeCharacterAddressGroup(chr, "Terks_Grot", sGroup, sSpot);
		SetFantomParamFromRank(chr, iRank, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("DestinyGiftFinalBattleWinner", "DestinyGiftFinalBattleWinner", "DestinyGiftFinalBattle ", 4);
	AddSimpleRumourToAllNations(iTxt("BO_OrchidRumour1") + GetFullName(PChar) + iTxt("BO_OrchidRumour2"), 30, 5);
}

void DestinyGiftFinalBattleWinner(string qName)
{
	chrDisableReloadToLocation = false;

	BO_PearlRecord("24");
	CloseQuestHeader("DestinyGift");
	pchar.questdata.DestinyGift = 1;	// LEO: отметка в энциклопедии
	PChar.DestinyGift = "END";

	// Gregg: оригинал вешал сюда таймер DestinyGiftImmortalReset - тот снимал
	// двухмесячное бессмертие и через неделю запускал главу 6. Бессмертия у нас нет,
	// снимать нечего. Срок до «Испанской щедрости» - 15 суток вместо недели оригинала:
	// после грота на Терксе игроку нужно время выдохнуть, а не сразу новый посыльный.
	SetTimerFunction("SpanishGenerosityStart", 0, 0, 15);
}
// Gregg: <== глава 5, вторая половина
// Gregg: ==> глава 6. «Чёрная Орхидея - испанская щедрость»
// Оригинал - lugger\MainLine.c:4408-5089 К:ПС, 31 функция, 17 записей, 3 диалога.
//
// Gregg: ОТСТУПЛЕНИЯ ОТ ОРИГИНАЛА, все обсуждены и приняты.
// 1. Испанцам уходит ПОДДЕЛКА. В К:ПС продавали настоящую Орхидею, срезав ей срок
//    действия до трёх суток: испанцы покупали артефакт, который протухнет раньше,
//    чем их эскадра дойдёт до Сан-Хуана. У нас Орхидея - вечный оберег без срока,
//    срезать нечего, поэтому Кендарт делает копию. Настоящая остаётся у игрока.
// 2. Деньги. В оригинале двадцать пять миллионов не платят НИ РАЗУ - во всей главе
//    нет ни одного AddMoneyToCharacter, сумма живёт только в репликах. У нас платят
//    задаток (BO_SG_DEPOSIT), остальное не доходит: подделку вскрывают.
// 3. Казна эскадры. Оригинал обещает её на флагмане (узел 41 диалога Кендарта)
//    и не кладёт; заодно проверяет в финале Орхидею у игрока, которую вернуть неоткуда.
//    У нас казна на «Альтее» по-настоящему: абордаж даёт BO_SG_TREASURE, потопление
//    топит вместе с кораблём.
// 4. Сан-Хуан у игрока НЕОТНИМАЕМ: на нём стоят главы 7-9 и сам Кендарт. Осады идут,
//    но Enemy_SanJuan держим закрытым - см. BO_SanJuanSiegeGuard.
// 5. Кендарт не требует уничтожить Орхидею: его мотив исчерпан.
//
// Состояния PChar.SpanishGenerosity:
//   1  посланец найден, ждут в Сантьяго      9  простой 21 сутки
//   2  торг был, отказ                      10  сделка закрыта, задаток получен
//   3  Кендарт зовёт в Ле-Франсуа           11  нести подделку в грот Доминики
//   4  план принят, Орхидея отдана          12  подделка в тайнике
//   5  копия готова                         13  Сан-Хуан наш
//   6  подделка на руках, Альборетто ждёт    14  эскадра на подходе
//   7  не используется (слился с 6)          15  морской бой
//   8  засада отбита                        16  победа

#define BO_SG_DEPOSIT		1000000	// задаток, который испанцы платят на месте
#define BO_SG_TREASURE		2000000	// казна эскадры в трюме «Альтеи»
#define BO_SG_COPY_DAYS		45		// сколько Кендарт делает копию
#define BO_SG_WAIT_DAYS		21		// простой, пока испанцы освобождают колонию
#define BO_SG_SIEGE_DAYS	365		// как часто испанцы приходят за Сан-Хуаном
#define BO_SG_GUARD_DAYS	14		// как часто сторож проверяет чужие перевзводы
#define BO_SG_FAKE_PRICE	3		// цена подделки на время, пока её кладут в сундук

void BO_SpanishRecord(string sRec)
{
	AddQuestRecord("SpanishGenerosity", sRec);
	// Gregg: две подстановки на род. sSex - возвратные глаголы (отказался/отказалась),
	// sSex1 - обычное прошедшее время (отдал/отдала). Функции в .txt не работают,
	// поэтому род в записях только так.
	AddQuestUserData("SpanishGenerosity", "sSex", GetSexPhrase("ся", "ась"));
	AddQuestUserData("SpanishGenerosity", "sSex1", GetSexPhrase("", "а"));
}

// Gregg: осады Сан-Хуана раз в полгода. Общий colony.c:1494 перевзводит осаду сам,
// на 25 + rand(70) суток, и править его нельзя - он обслуживает все захваченные
// города. Поэтому расписание держим своё, а чужой перевзвод гасит сторож.
void BO_SanJuanSiegeArm()
{
	SetTimerFunction("BO_SanJuanSiege", 0, 0, BO_SG_SIEGE_DAYS);
	SetTimerFunction("BO_SanJuanSiegeGuard", 0, 0, BO_SG_GUARD_DAYS);
}

void BO_SanJuanSiege(string qName)
{
	// Gregg: сама осада заводится штатным механизмом - нам нужен только её запуск.
	// over снимаем руками: ни SetTimerCondition, ни SetTimerFunction его не чистят
	// (quests.c:1704 и 1721), а сторож мог закрыть узел в прошлый раз.
	DeleteAttribute(PChar, "quest.Attack_Fort_SanJuan.over");
	SetTimerCondition("Attack_Fort_SanJuan", 0, 0, 1, false);
	// Gregg: помечаем осаду своей на один проход сторожа - иначе он погасит её сам,
	// он ведь ровно для того и заведён, чтобы гасить ЧУЖИЕ перевзводы.
	PChar.SpanishGenerosity.SiegeGrace = true;
	PChar.Quest.Attack_Fort_SanJuan.CityId = "SanJuan";
	PChar.Quest.Attack_Fort_SanJuan.function = "TWN_Siege_Any";

	// Gregg: перевзводим себя. over снять ОБЯЗАТЕЛЬНО: OnQuestComplete ставит его
	// ДО вызова функции (quests_check.c:455), а хвостовой цикл QuestsCheck узел
	// с over удаляет целиком (quests_check.c:437-443). Без этого таймер отработал бы
	// ровно один раз, и осад больше не было бы никогда.
	SetTimerFunction("BO_SanJuanSiege", 0, 0, BO_SG_SIEGE_DAYS);
	DeleteAttribute(PChar, "quest.BO_SanJuanSiege.over");
}

void BO_SanJuanSiegeGuard(string qName)
{
	// Gregg: две вещи, обе - защита от общего кода колоний.
	// Первая: Enemy_SanJuan заводится в TWN_Siege_Any на 9 + rand(15) суток и по
	// истечении отдаёт город обратно Испании (TWN_Enemy_Any -> SetCaptureTownByNation).
	// Нам это нельзя: на Сан-Хуане стоят главы 7-9 и Кендарт в ратуше. Держим закрытым.
	// Вторая: после отбитой осады colony.c:1494 сам ставит следующую на 25 + rand(70).
	// Гасим - расписание у нас своё, BO_SanJuanSiege.
	if (CheckAttribute(PChar, "quest.Enemy_SanJuan"))
	{
		PChar.quest.Enemy_SanJuan.over = "yes";
	}
	if (CheckAttribute(PChar, "SpanishGenerosity.SiegeGrace"))
	{
		// наша осада, только что заведённая, - пропускаем ровно один проход
		DeleteAttribute(PChar, "SpanishGenerosity.SiegeGrace");
	}
	else
	{
		if (CheckAttribute(PChar, "quest.Attack_Fort_SanJuan"))
		{
			PChar.quest.Attack_Fort_SanJuan.over = "yes";
		}
	}
	SetTimerFunction("BO_SanJuanSiegeGuard", 0, 0, BO_SG_GUARD_DAYS);
	DeleteAttribute(PChar, "quest.BO_SanJuanSiegeGuard.over");
}

// --- акт первый: сделка ---------------------------------------------------------

void SpanishGenerosityStart(string qName)
{
	PChar.quest.SpanishGenerosityFirstTalk.win_condition.l1 = "Location_Type";
	PChar.quest.SpanishGenerosityFirstTalk.win_condition.l1.location_type = "town";
	PChar.quest.SpanishGenerosityFirstTalk.function = "SpanishGenerosityFirstTalk";
}

void SpanishGenerosityFirstTalk(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Alboretto Man 1", "citiz_" + (rand(11) + 1), "man", "man", 1, SPAIN, 0, false));
	chr.greeting = "cit_quest";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_SGCitizens.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto1");
	SetDialogCharacter2Character(PChar, chr, 30.0, false);
}

void SpanishGenerosityFirstToSantiago()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Alboretto Man 1");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	SetQuestHeader("SpanishGenerosity");
	BO_SpanishRecord("1");
	PChar.SpanishGenerosity = "1";

	SetQuestFunction("SpanishGenerosityFirstInSantiago", "SpanishGenerosityFirstInSantiago", "l", "Santiago_tavern");
}

void SpanishGenerosityFirstInSantiago(string qName)
{
	chrDisableReloadToLocation = true;

	// Gregg: модель Alboretto из К:ПС в CSP нет. PGG_Espinosa_0 - Диего Эспиноза,
	// чёрно-золотой гранд с испанской перевязью; лицо ему прописано отдельно
	// в characters_face.c рядом с ключом «Espinosa». Решение пользователя.
	ref chr = GetCharacter(NPC_GenerateCharacter("Emilio Alboretto", "PGG_Espinosa_0", "man", "man", 25, SPAIN, -1, false));
	chr.name = iTxt("BO_AlborettoName");
	chr.lastname = iTxt("BO_AlborettoLastName");
	chr.greeting = "Gr_QuestMan";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Alboretto.c";
	chr.Dialog.CurrentNode = "1";
	chr.NotRemoved = true;
	LAi_SetImmortal(chr, true);
	LAi_SetSitType(chr);

	LAi_Fade("", "");

	// Gregg: освобождать стол НАДО ЗДЕСЬ, непосредственно перед посадкой, а не в начале
	// функции: между освобождением и посадкой успевают отработать NPC_GenerateCharacter
	// и LAi_Fade, и на только что освобождённый стул садится случайный посетитель.
	// Так это сделано во всех рабочих сценах мода - образец Addons\Quests.c:829.
	// Третий стул того же стола, sit5, освобождаем тоже: иначе за спиной у Альборетто
	// сидит посторонний. В оригинале К:ПС для него была отдельная строка в конце.
	BO_FreeLocator("Santiago_tavern", "sit_base4", "Emilio Alboretto");
	BO_FreeLocator("Santiago_tavern", "sit_front4", "Emilio Alboretto");
	BO_FreeLocator("Santiago_tavern", "sit5", "Emilio Alboretto");

	ChangeCharacterAddressGroup(chr, "Santiago_tavern", "sit", "sit_base4");
	ChangeCharacterAddressGroup(PChar, "Santiago_tavern", "sit", "sit_front4");

	LAi_SetSitType(PChar);
	LAi_SetActorType(chr);
	LAi_ActorSetSitMode(chr);
	LAi_ActorDialogNow(chr, PChar, "", -1);
}

void SpanishGenerosityExitFromTavern()
{
	LAi_Fade("", "");

	ref chr = CharacterFromID("Emilio Alboretto");
	ChangeCharacterAddressGroup(PChar, "Santiago_tavern", "tables", "stay6");

	// Gregg: сидящего сперва поднять со стула. Актёр, которому велено идти прямо
	// из режима сидения, пути не строит и остаётся на месте - это и есть то самое
	// «застрял в локаторе сидения». Оригинал К:ПС переставлял его на стоячий
	// локатор группы tables, и был прав; я эту строку однажды выбросил, приняв
	// зависание за уход с непроходимой точки. Срок при этом всё равно конечный:
	// с timeout -1 провал поиска пути не разрешается никогда.
	ChangeCharacterAddressGroup(chr, "Santiago_tavern", "tables", "stay5");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	LAi_SetPlayerType(PChar);
	PChar.SpanishGenerosity = "2";
	BO_SpanishRecord("2");

	chrDisableReloadToLocation = false;
	SetQuestFunction("SpanishGenerosityNoticeInSantiago", "SpanishGenerosityNoticeInSantiago", "l", "Santiago_town");
}

void SpanishGenerosityNoticeInSantiago(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Kendart Man 1", "citiz_" + (rand(11) + 1), "man", "man", 1, PIRATE, 0, false));
	chr.greeting = "cit_quest";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_SGCitizens.c";
	chr.Dialog.CurrentNode = "2";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto19");
	SetDialogCharacter2Character(PChar, chr, 60.0, false);
}

void SpanishGenerosityToLeFransua()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Kendart Man 1");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "gate_back", "none", "", "", "", -1);

	PChar.SpanishGenerosity = "3";
	BO_SpanishRecord("3");

	// Gregg: лестница наверх заперта штатно, как и на Бермудах в главе 5.
	// Открываем на время главы и закрываем в SpanishGenerosityToSanJuan.
	LocatorReloadEnterDisable("LeFransua_tavern", "reload2_back", false);

	chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart6.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetStayType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	ChangeCharacterAddressGroup(chr, "LeFransua_tavern_upstairs", "goto", "goto1");
}

void SpanishGenerosityFirstTimer()
{
	PChar.SpanishGenerosity = "4";
	BO_SpanishRecord("4");

	// Gregg: Орхидея уходит Кендарту на время работ - оберег всё это время не работает.
	// Это и есть цена плана, и она приходится ровно на срок добычи сильного корабля.
	// Gregg: слот снимаем ОТДЕЛЬНО: TakeNItems убирает предмет из инвентаря и не
	// трогает экипировку, а BlackOrchid_IsWorn смотрит именно на equip.talisman -
	// без этого оберег работал бы все сорок пять суток, пока «лежит у Кендарта».
	RemoveCharacterEquip(PChar, TALISMAN_ITEM_TYPE);
	TakeNItems(PChar, "BlackOrchid", -1);

	SetTimerFunction("SpanishGenerosityFirstTimerEnd", 0, 0, BO_SG_COPY_DAYS);
}

void SpanishGenerosityFirstTimerEnd(string qName)
{
	PChar.SpanishGenerosity = "5";
	BO_SpanishRecord("5");

	ref chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.CurrentNode = "20";
}

void SpanishGenerosityGetFake()
{
	PChar.SpanishGenerosity = "6";
	BO_SpanishRecord("6");

	// Gregg: настоящая возвращается хозяину, копия идёт на продажу
	TakeNItems(PChar, "BlackOrchid", 1);
	EquipCharacterByItem(PChar, "BlackOrchid");
	// LEO: квест с Орхидеей закончен - даём ей цену, как у Лунного камня. С ценой ноль IsQuestUsedItem считает
	//   её квестовой: ни выложить, ни передать офицеру, ни продать. Дальше по линии она нигде не нужна.
	int iOrchid = FindItem("BlackOrchid");
	if (iOrchid >= 0) Items[iOrchid].price = 40000;
	TakeNItems(PChar, "BlackOrchid_Fake", 1);

	// Gregg: и возвращаем Альборетто за стол в таверне Сантьяго. Раньше это делала
	// отдельная SpanishGenerositySecondToSantiago, которую никто не звал, - глава
	// вставала намертво: подделка на руках, а продавать её некому.
	// Локатор освобождаем заново: он ушёл актёром в ExitFromTavern, а посетителей
	// на это место расставляет генератор при каждом входе в таверну.
	ref chr = CharacterFromID("Emilio Alboretto");
	chr.Dialog.CurrentNode = "20";
	BO_FreeLocator("Santiago_tavern", "sit_base4", "Emilio Alboretto");
	BO_FreeLocator("Santiago_tavern", "sit5", "Emilio Alboretto");
	ChangeCharacterAddressGroup(chr, "Santiago_tavern", "sit", "sit_base4");
	LAi_SetSitType(chr);
}

// --- акт второй: засада у Санта-Каталины -----------------------------------------

void SpanishGenerosityAlborettoInShore()
{
	PChar.SpanishGenerosity = "8";
	BO_SpanishRecord("7");

	ref chr = CharacterFromID("Emilio Alboretto");

	FantomMakeCoolSailor(chr, SHIP_LINESHIP, iTxt("BO_AndromedaName"), CANNON_TYPE_CULVERINE_LBS32, 100, 100, 100);
	BO_SetSeaPerks(chr);
	chr.AlwaysFriend = true;
	chr.DontRansackCaptain = true;
	chr.DontHitInStorm = true;
	chr.ShipEnemyDisable = true;
	chr.DontCheckFlag = true;
	chr.Abordage.Enable = false;
	// Gregg: SetGeraldDetails из оригинала выброшен намеренно. Он пишет в GetRealShip(type),
	// то есть в шаблон КЛАССА, и перекрашивает все корабли этого типа в игре.

	// Gregg: уход из таверны - ПОСЛЕДНИМ делом. Любая смена типа ИИ после
	// LAi_ActorRunToLocation сбрасывает актёрское состояние, и персонаж встаёт
	// столбом там, где его застали: боевой тип ему нужен в море, там его и ставим -
	// в SpanishGenerosityBattleOurShore.
	// Gregg: сидящего сперва поднять со стула. Актёр, которому велено идти прямо
	// из режима сидения, пути не строит и остаётся на месте - это и есть то самое
	// «застрял в локаторе сидения». Оригинал К:ПС переставлял его на стоячий
	// локатор группы tables, и был прав; я эту строку однажды выбросил, приняв
	// зависание за уход с непроходимой точки. Срок при этом всё равно конечный:
	// с timeout -1 провал поиска пути не разрешается никогда.
	ChangeCharacterAddressGroup(chr, "Santiago_tavern", "tables", "stay5");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	SetQuestFunction("SpanishGenerosityBattleOurShore", "SpanishGenerosityBattleOurShore", "l", "SantaCatalina");
}

void SpanishGenerosityBattleOurShore(string qName)
{
	BO_SpanishRecord("8");

	bQuestDisableMapEnter = true;
	Island_SetReloadEnableLocal("SantaCatalina", "reload_1", false);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_2", false);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_3", false);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_4", false);

	ref chr;
	int iShip = 0;
	// Gregg: ранг читаем напрямую. GetRank(PChar, X) из оригинала в CSP прибавляет
	// ранг ГГ ещё раз внутри и вернула бы удвоенное значение.
	int iRank = sti(PChar.rank) + MOD_SKILL_ENEMY_RATE;

	Group_FindOrCreateGroup("AndromedaShip");
	Group_SetType("AndromedaShip", "trader");
	Group_AddCharacter("AndromedaShip", "Emilio Alboretto");
	Group_SetGroupCommander("AndromedaShip", "Emilio Alboretto");

	Group_FindOrCreateGroup("AndromedaAttackers");
	Group_SetType("AndromedaAttackers", "war");
	for (int i = 1; i <= 3; i++)
	{
		iShip = SHIP_LINESHIP;
		if (i == 2) iShip = SHIP_GALEON_H;
		if (i == 3) iShip = SHIP_DUTCHLINESHIP;

		chr = GetCharacter(NPC_GenerateCharacter("AndromedaAttacker_" + i, "pirate_" + (rand(9) + 1), "man", "man", iRank, PIRATE, 0, true));
		FantomMakeCoolSailor(chr, iShip, "", CANNON_TYPE_CANNON_LBS32, 70, 70, 70);
		FantomMakeCoolFighter(chr, iRank, 70, 70, "", "", 100);
		LAi_NPC_Equip(chr, iRank, true, true);
		BO_SetSeaPerks(chr);
		chr.AlwaysEnemy = true;
		Group_AddCharacter("AndromedaAttackers", "AndromedaAttacker_" + i);
		SetCharacterRelationBoth(nMainCharacterIndex, GetCharacterIndex("AndromedaAttacker_" + i), RELATION_ENEMY);
		SetCharacterRelationBoth(GetCharacterIndex("Emilio Alboretto"), GetCharacterIndex("AndromedaAttacker_" + i), RELATION_ENEMY);
	}

	Group_SetGroupCommander("AndromedaAttackers", "AndromedaAttacker_1");
	Group_SetTaskAttack("AndromedaAttackers", "AndromedaShip");
	Group_SetAddress("AndromedaAttackers", "SantaCatalina", "quest_ships", "quest_ship_8");
	Group_LockTask("AndromedaAttackers");

	Group_SetTaskAttack("AndromedaShip", "AndromedaAttackers");
	Group_LockTask("AndromedaShip");
	Group_SetAddress("AndromedaShip", "SantaCatalina", "quest_ships", "quest_ship_9");

	SetCharacterRelationBoth(GetCharacterIndex("Emilio Alboretto"), nMainCharacterIndex, RELATION_FRIEND);
	// Gregg: боевой тип ему нужен здесь, в море, а не в таверне - см. AlborettoInShore
	LAi_SetWarriorType(CharacterFromID("Emilio Alboretto"));

	PChar.quest.SpanishGenerosityBattleOurShoreWin.win_condition.l1 = "Group_Death";
	PChar.quest.SpanishGenerosityBattleOurShoreWin.win_condition.l1.group = "AndromedaAttackers";
	PChar.quest.SpanishGenerosityBattleOurShoreWin.function = "SpanishGenerosityBattleOurShoreWin";

	// Gregg: ПРОВАЛ, если «Андромеду» пустят на дно. Бессмертие Альборетто здесь же
	// и снимается: с ним корабль был неуязвим (AIShip.c), и защищать было некого -
	// сопровождение без риска не сопровождение.
	LAi_SetImmortal(CharacterFromID("Emilio Alboretto"), false);
	PChar.quest.SpanishGenerosityAndromedaSink.win_condition.l1 = "Character_sink";
	PChar.quest.SpanishGenerosityAndromedaSink.win_condition.l1.character = "Emilio Alboretto";
	PChar.quest.SpanishGenerosityAndromedaSink.function = "SpanishGenerosityAndromedaSink";
}

void SpanishGenerosityAndromedaSink(string qName)
{
	// Gregg: ПРОВАЛ. Альборетто - единственный ход к сделке, других у линии нет.
	PChar.SpanishGenerosity = "FAIL";
	BO_SpanishRecord("20");
	BO_QuestFailed("SpanishGenerosity");

	PChar.quest.SpanishGenerosityBattleOurShoreWin.over = "yes";
	PChar.quest.SpanishGenerosityAndromedaSink.over = "yes";

	bQuestDisableMapEnter = false;
	Island_SetReloadEnableLocal("SantaCatalina", "reload_1", true);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_2", true);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_3", true);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_4", true);
	BO_SGDropAndromeda();

	// Gregg: Кендарта тоже убираем - линия мертва, а он несносимый и бессмертный
	ref chr = CharacterFromID("Alehon Kendart");
	if (CheckAttribute(chr, "NotRemoved")) DeleteAttribute(chr, "NotRemoved");
	LAi_SetImmortal(chr, false);
	ChangeCharacterAddress(chr, "none", "");
	LocatorReloadEnterDisable("LeFransua_tavern", "reload2_back", true);
}

void SpanishGenerosityBattleOurShoreWin(string qName)
{
	BO_SpanishRecord("9");
	Group_DeleteGroup("AndromedaAttackers");

	// Gregg: разговор на палубе «Андромеды» после боя. DeckDialogNode в CSP живой
	// и используется десятком квестов - берём механизм оригинала как есть.
	// Каютный хук тут не при чём: Альборетто союзник, его никто не абордирует.
	ref chr = CharacterFromID("Emilio Alboretto");
	chr.DeckDialogNode = "30";
}

void SpanishGenerosityPreSecondTimer()
{
	bQuestDisableMapEnter = false;
	Island_SetReloadEnableLocal("SantaCatalina", "reload_1", true);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_2", true);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_3", true);
	Island_SetReloadEnableLocal("SantaCatalina", "reload_4", true);

	ref chr = CharacterFromID("Emilio Alboretto");
	chr.DeckDialogNode = "First Time";

	// Gregg: запись про ответ Севильи - здесь, сразу после разговора на палубе.
	// Раньше её выдавал SpanishGenerositySecondTimer по выходу на глобальную карту,
	// и игрок узнавал новость из журнала через полчаса после того, как ему её сказали.
	BO_SpanishRecord("10");

	// Gregg: три недели отсчитываем ОТСЮДА, а не от выхода на глобальную карту.
	// Раньше срок запускал узел на MapEnter - и если игрок шёл до Сантьяго морем,
	// не открывая карты (в CSP так можно, доплытие), отсчёт не начинался вовсе
	// и Альборетто в ратуше не появлялся никогда.
	SetTimerFunction("SpanishGenerositySecondTimerEnd", 0, 0, BO_SG_WAIT_DAYS);

	// Gregg: а вот УБОРКА групп по-прежнему на MapEnter: сносить корабль Альборетто,
	// пока игрок стоит с ним в одном море, нельзя. Опоздает - не страшно, тот же
	// снос продублирован в SpanishGenerositySecondTimerEnd.
	PChar.quest.SpanishGenerositySecondTimer.win_condition.l1 = "MapEnter";
	PChar.quest.SpanishGenerositySecondTimer.function = "SpanishGenerositySecondTimer";
}

void SpanishGenerositySecondTimer(string qName)
{
	PChar.quest.SpanishGenerositySecondTimer.over = "yes";

	PChar.SpanishGenerosity = "9";
	BO_SGDropAndromeda();

}

// Gregg: снос морских групп Альборетто. Зовётся дважды - с выхода на глобальную
// карту и по истечении срока, - поэтому обе половины под проверкой существования.
void BO_SGDropAndromeda()
{
	if (Group_FindGroup("AndromedaAttackers") != -1)
	{
		Group_SetAddress("AndromedaAttackers", "none", "none", "none");
		Group_DeleteGroup("AndromedaAttackers");
	}
	if (Group_FindGroup("AndromedaShip") != -1)
	{
		Group_SetAddress("AndromedaShip", "none", "none", "none");
		Group_DeleteGroup("AndromedaShip");
	}
}

void SpanishGenerositySecondTimerEnd(string qName)
{
	PChar.quest.SpanishGenerositySecondTimer.over = "yes";
	BO_SGDropAndromeda();

	ref chr = CharacterFromID("Emilio Alboretto");
	chr.Dialog.CurrentNode = "40";
	ChangeCharacterAddressGroup(chr, "Santiago_townhall", "goto", "goto3");
	LAi_SetStayType(chr);
	BO_SpanishRecord("11");
}

// --- акт третий: Сан-Хуан --------------------------------------------------------

void SpanishGenerosityDealDone()
{
	PChar.SpanishGenerosity = "10";
	// Gregg: одна запись на весь разговор - и очищенный город, и задаток, и куда идти
	// дальше. Раньше их было три подряд, включая связку «Кендарт ждёт» из ToKendart:
	// три уведомления за одну реплику читаются как разговор с идиотом.
	BO_SpanishRecord("12");

	// Gregg: задаток. Остальное «переводом через банкирский дом» не дойдёт -
	// подделку вскроют раньше, и об этом скажет Кендарт после захвата колонии.
	AddMoneyToCharacter(PChar, BO_SG_DEPOSIT);

	// Gregg: сразу и следующий шаг - возвращаться к Кендарту. Отдельной репликой
	// в диалоге Альборетто это не сделать: он уходит из ратуши тем же выходом.
	SpanishGenerosityToKendart();
}

void SpanishGenerosityToKendart()
{
	PChar.SpanishGenerosity = "11";

	ref chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.CurrentNode = "25";
}

void SpanishGenerosityToGrot()
{
	PChar.SpanishGenerosity = "12";
	BO_SpanishRecord("13");

	// Gregg: глушим генератор «Проклятый капитан». Открытие сундука в локации типа
	// "cave" с локаторами monsters с шансом 50% зовёт spawnToughSkeleton (itemLogic.c:914),
	// а грот Доминики подходит под оба условия. Скелет двадцатого ранга посреди тайника
	// лишний. Та же грабля ловилась в главе 5 на Pearl_Grot и в «Лунном камне».
	LAi_LocationDisableToughSkeletonTimer("Dominica_Grot", 60);

	SetQuestFunction("SpanishGenerosityInGrot", "SpanishGenerosityInGrot", "l", "Dominica_Grot");
}

void SpanishGenerosityInGrot(string qName)
{
	// Gregg: перевзводим глушилку - игрок мог искать грот дольше отведённого срока
	LAi_LocationDisableToughSkeletonTimer("Dominica_Grot", 60);

	// Gregg: подделке НУЖНА цена, иначе её физически не выложить в сундук:
	// IsQuestUsedItem (items_utilite.c:276) считает квестовым всё, у чего цена ноль,
	// а короб такое запрещает переносить (itemsbox.c:1699). Ставим три песо - ровно
	// столько, во сколько её оценивает описание, и продать за такие деньги некому.
	// Ноль вернём в FakeToChest, а если игрок уйдёт из грота, не выложив, - в
	// SpanishGenerosityOutGrot, чтобы подделка не осталась продаваемой.
	int iFake = FindItem("BlackOrchid_Fake");
	if (iFake >= 0) Items[iFake].price = BO_SG_FAKE_PRICE;

	SetQuestFunction("SpanishGenerosityOutGrot", "SpanishGenerosityOutGrot", "efl", "Dominica_Grot");

	PChar.quest.SpanishGenerosityFakeToChest.win_condition.l1 = "item_cur_qty";
	PChar.quest.SpanishGenerosityFakeToChest.win_condition.l1.item = "BlackOrchid_Fake";
	PChar.quest.SpanishGenerosityFakeToChest.win_condition.l1.item.quantity = 0;
	PChar.quest.SpanishGenerosityFakeToChest.function = "SpanishGenerosityFakeToChest";
}

// Gregg: игрок ушёл из грота, не выложив подделку. Возвращаем ноль, чтобы её нельзя
// было продать, и снова взводим вход - цену поставим на следующем заходе.
void SpanishGenerosityOutGrot(string qName)
{
	if (CheckAttribute(PChar, "SpanishGenerosity.FakeHidden")) return;

	int iFake = FindItem("BlackOrchid_Fake");
	if (iFake >= 0) Items[iFake].price = 0;
	// Gregg: over снять обязательно - SetQuestFunction его не чистит, а узел уже
	// отработал на входе, и хвостовой цикл QuestsCheck успел его удалить.
	DeleteAttribute(PChar, "quest.SpanishGenerosityInGrot.over");
	SetQuestFunction("SpanishGenerosityInGrot", "SpanishGenerosityInGrot", "l", "Dominica_Grot");
}

void SpanishGenerosityFakeToChest(string qName)
{
	PChar.SpanishGenerosity.FakeHidden = true;
	PChar.quest.SpanishGenerosityOutGrot.over = "yes";

	int iFake = FindItem("BlackOrchid_Fake");
	if (iFake >= 0) Items[iFake].price = 0;

	ref itm = ItemsFromID("BlackOrchid_Fake");
	itm.NotTakeFromChest = true;
	SetQuestBoxItems("Dominica_Grot", "BlackOrchid_Fake", 1);

	BO_SpanishRecord("14");

	ref chr = CharacterFromID("Alehon Kendart");
	ChangeCharacterAddress(chr, "None", "");

	chr = CharacterFromID("Emilio Alboretto");
	ChangeCharacterAddress(chr, "None", "");

	SetQuestFunction("SpanishGenerosityNoticeFromKendart", "SpanishGenerosityNoticeFromKendart", "l", "LeFransua_tavern");
}

void SpanishGenerosityNoticeFromKendart(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Kendart Man 2", "citiz_" + (rand(11) + 1), "man", "man", 1, PIRATE, 0, false));
	chr.greeting = "cit_quest";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_SGCitizens.c";
	chr.Dialog.CurrentNode = "3";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "LeFransua_tavern", "goto", "goto2");
	SetDialogCharacter2Character(PChar, chr, 25.0, false);
}

void SpanishGenerosityToSanJuan()
{
	chrDisableReloadToLocation = false;
	PChar.SpanishGenerosity = "13";

	// Gregg: тайник опустел - подделку забрали испанцы
	ref loc = LocFromID("Dominica_Grot");
	if (CheckAttribute(loc, "box1")) DeleteAttribute(loc, "box1");
	ClearQuestBoxStay("Dominica_Grot");

	// Gregg: лестницу в Ле-Франсуа возвращаем в штатное закрытое состояние
	LocatorReloadEnterDisable("LeFransua_tavern", "reload2_back", true);

	ref chr = CharacterFromID("Kendart Man 2");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	BO_SpanishRecord("15");

	if (GetCharacterIndex("SanJuan_Smuggler") != -1)
	{
		ChangeCharacterAddress(CharacterFromID("SanJuan_Smuggler"), "None", "");
	}
	ChangeCharacterAddress(CharacterFromID("SanJuan_Mayor"), "None", "");

	SetCaptureTownByHero("SanJuan");

	ref colony = &Colonies[FindColony("SanJuan")];
	colony.isBought = true;
	colony.DontSetShipInPort = true;
	colony.OfficerIdx = nMainCharacterIndex;

	chr = CharacterFromID("Alehon Kendart");
	ChangeCharacterAddressGroup(chr, "SanJuan_townhall", "goto", "governor1");
	chr.Dialog.CurrentNode = "30";
	chr.ShipEnemyDisable = true;
	chr.DontCheckFlag = true;

	chr = CharacterFromID("SanJuan Fort Commander");
	chr.AnalizeShips = true;
	chr.AlwaysFriend = true;
	chr.ShipEnemyDisable = true;
	chr.DontCheckFlag = true;

	PlayStereoOGG("music_ship_victory");
	Log_Info(iTxt("BO_SanJuanTaken"));
	LaunchColonyInfoScreen();

	// Gregg: Бэлами в мире ещё нет - заводим. Кувалда есть с главы 5.
	// Gregg: Кувалду глава 5 распускает начисто (LifeDay = 0 плюс адрес None), и
	// EmptyAllFantomCharacter стирает слот на первом же переходе. К главе 6 его в мире
	// уже нет: CharacterFromID вернул бы NullCharacter, PlaceCharacterShip получил бы
	// индекс -1, а characters[-1] обрывает выполнение всей функции - вместе с ней
	// не взвёлся бы и узел, двигающий главу дальше. Заводим заново.
	if (GetCharacterIndex("Cristobal Kuvalda") == -1)
	{
		ref chKuv = GetCharacter(NPC_GenerateCharacter("Cristobal Kuvalda", "officer_14", "man", "man", 30, PIRATE, -1, false));
		chKuv.name = iTxt("BO_KuvaldaName");
		chKuv.lastname = iTxt("BO_KuvaldaLastName");
	}

	if (GetCharacterIndex("Roger Belami") == -1)
	{
		ref chBel = GetCharacter(NPC_GenerateCharacter("Roger Belami", "officer_17", "man", "man", 30, PIRATE, -1, false));
		chBel.name = iTxt("BO_BelamiName");
		chBel.lastname = iTxt("BO_BelamiLastName");
	}

	// Gregg: Кувалда - главный сторожевик колонии, Бэлами в его группе.
	// Gregg: Кувалде 36-фунтовые кулеврины, Бэлами вместо военного фрегата - линкор
	// с 32-фунтовыми (решения пользователя: фрегат против испанской линии не жил).
	BO_SGPlaceAlly("Cristobal Kuvalda", SHIP_MANOWAR, "BO_FeliciaName", "officer_14", "Cristobal Kuvalda", CANNON_TYPE_CULVERINE_LBS36);
	BO_SGPlaceAlly("Roger Belami", SHIP_LINESHIP, "BO_ReginaldName", "officer_17", "Cristobal Kuvalda", CANNON_TYPE_CULVERINE_LBS32);

}

// Gregg: два союзных корабля в порту Сан-Хуана. Кувалда уже есть в мире с главы 5,
// Бэлами заводится здесь - поэтому проверяем, а не создаём вслепую.
void BO_SGPlaceAlly(string sId, int iShip, string sShipKey, string sModel, string sLeadId, int iCannon)
{
	// Gregg: обоих заводит вызывающая сторона, тут только берём готового -
	// без склейки ключей из id (в id пробел). Гвард обязателен: без персонажа
	// CharacterFromID вернёт NullCharacter, и мы запишем корабельные атрибуты
	// в шаблон, которым пользуется весь мод.
	if (GetCharacterIndex(sId) == -1) return;
	ref chr = CharacterFromID(sId);
	chr.model = sModel;
	// Gregg: параметры корабля на случай его гибели в финальном бою: по ним
	// BO_SGReturnAlly отстроит спасённому капитану новый той же постройки
	chr.BO_SG.ship = iShip;
	chr.BO_SG.shipname = sShipKey;
	chr.BO_SG.cannon = iCannon;
	FantomMakeCoolSailor(chr, iShip, iTxt(sShipKey), iCannon, 100, 100, 100);
	FantomMakeCoolFighter(chr, 30, 100, 100, "", "", 500);
	LAi_NPC_Equip(chr, 30, true, true);
	BO_SetSeaPerks(chr);
	chr.Skill.Sailing = 100;
	// Gregg: команды лучшей выучки - решение пользователя
	chr.Ship.Crew.Exp.Sailors = 100;
	chr.Ship.Crew.Exp.Cannoners = 100;
	chr.Ship.Crew.Exp.Soldiers = 100;
	chr.NotRemoved = true;
	// Gregg: IslandShips обязателен. PlaceCharacterShip ставит им fortDefender = 1,
	// а Ship_CS_DefenderAI первой же строкой читает GetFortCommander(chr.IslandShips) -
	// без атрибута это RUNTIME ERROR на каждом тике ИИ (AIShip.c 1580 и 1630).
	// От зачистки колонии их бережёт гвард по NotRemoved в ClearIslandShips.
	chr.IslandShips = Colonies[FindColony("SanJuan")].id;
	chr.AlwaysFriend = true;
	chr.ShipEnemyDisable = true;
	chr.DontCheckFlag = true;
	chr.Ship.Mode = "war";
	chr.location.from_sea = Colonies[FindColony("SanJuan")].from_sea;
	LAi_SetImmortal(chr, true);
	// Gregg: в CSP у PlaceCharacterShip два лишних против К:ПС аргумента - сторожевик
	// колонии и индекс главного сторожевика, под чью группу он встаёт (islandships.c:183).
	PlaceCharacterShip(GetCharacterIndex(sId), PIRATE, "PuertoRico", FindColony("SanJuan"), true, GetCharacterIndex(sLeadId));
	SetCharacterRelationBoth(GetCharacterIndex(sId), GetMainCharacterIndex(), RELATION_FRIEND);
	SetCaptanModelByEncType(chr, "war");
	Fantom_SetCannons(chr, "war");
	Fantom_SetBalls(chr, "war");
	Fantom_SetGoods(chr, "war");
}

// Gregg: зовётся из узла 35 диалога Кендарта через SpanishGenerosityToFinal.
// Раньше висела на входе в SanJuan_town - гонец перехватывал игрока у ворот раньше,
// чем тот доходил до Кендарта в ратуше, и запись про эскадру приходила прежде
// разговора, из которого игрок о ней и узнаёт.
void SpanishGenerosityMessage(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Kendart Man 3", "citiz_" + (rand(11) + 1), "man", "man", 1, PIRATE, 0, false));
	chr.greeting = "cit_quest";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_SGCitizens.c";
	chr.Dialog.CurrentNode = "4";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");
	SetDialogCharacter2Character(PChar, chr, 30.0, true);
}

void SpanishGenerosityToFinal()
{
	// Gregg: нульарная обёртка под AddDialogExitQuestFunction. Ловим игрока НА УЛИЦЕ,
	// а не задержкой в три секунды: с задержкой гонец вбегал в ту же ратушу, где игрок
	// только что вышел от Кендарта, и звал его к Кендарту же. Раньше срока узел
	// не сработает - взводится он только отсюда, из конца разговора.
	SetQuestFunction("SpanishGenerosityMessage", "SpanishGenerosityMessage", "l", "SanJuan_town");
}

void SpanishGenerosityPrepareFinalBattle()
{
	chrDisableReloadToLocation = false;
	PChar.SpanishGenerosity = "14";
	BO_SpanishRecord("16");

	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("Kendart Man 3");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", -1);

	BO_SGMakeCompanion("Cristobal Kuvalda");
	BO_SGMakeCompanion("Roger Belami");

	// Gregg: гибель корабля союзника не должна быть гибелью капитана. ShipDead и
	// ShipTaken ставят ему HP 0, а штатные ветки спасения (AIShip.c) требуют
	// GetRemovable, который BO_SGMakeCompanion как раз закрыл. Ловим оба исхода.
	PChar.quest.SpanishGenerosityKuvaldaSink.win_condition.l1 = "Character_sink";
	PChar.quest.SpanishGenerosityKuvaldaSink.win_condition.l1.character = "Cristobal Kuvalda";
	PChar.quest.SpanishGenerosityKuvaldaSink.function = "SpanishGenerosityKuvaldaSink";
	PChar.quest.SpanishGenerosityKuvaldaCapture.win_condition.l1 = "Character_Capture";
	PChar.quest.SpanishGenerosityKuvaldaCapture.win_condition.l1.character = "Cristobal Kuvalda";
	PChar.quest.SpanishGenerosityKuvaldaCapture.function = "SpanishGenerosityKuvaldaCapture";
	PChar.quest.SpanishGenerosityBelamiSink.win_condition.l1 = "Character_sink";
	PChar.quest.SpanishGenerosityBelamiSink.win_condition.l1.character = "Roger Belami";
	PChar.quest.SpanishGenerosityBelamiSink.function = "SpanishGenerosityBelamiSink";
	PChar.quest.SpanishGenerosityBelamiCapture.win_condition.l1 = "Character_Capture";
	PChar.quest.SpanishGenerosityBelamiCapture.win_condition.l1.character = "Roger Belami";
	PChar.quest.SpanishGenerosityBelamiCapture.function = "SpanishGenerosityBelamiCapture";

	// Gregg: на время осады форт вдвое прочнее - решение пользователя. Штатный максимум
	// (пушки х100, у Сан-Хуана 4500) четыре линкора разбирают за считаные залпы.
	// Множитель подхватит Fort_CreateEvent при входе в море; снимается после победы.
	ref rFort = CharacterFromID("SanJuan Fort Commander");
	rFort.Fort.QuestHPMult = 2.0;

	SetQuestFunction("SpanishGenerosityFinalBattle", "SpanishGenerosityFinalBattle", "l", "PuertoRico");
}

void BO_SGMakeCompanion(string sId)
{
	ref chr = CharacterFromID(sId);
	string sGroupID = "IslandGroup" + GetCharacterIndex(sId);
	Group_SetAddress(sGroupID, "None", "None", "None");
	LAi_SetImmortal(chr, false);
	SetCompanionIndex(PChar, -1, GetCharacterIndex(sId));
	SetShipRemovable(chr, false);
	SetCharacterRemovable(chr, false);
}

void SpanishGenerosityFinalBattle(string qName)
{
	PChar.SpanishGenerosity = "15";
	PChar.SpanishGenerosity.SiegeNow = true;

	bQuestDisableMapEnter = true;
	Island_SetReloadEnableLocal("PuertoRico", "reload_1", false);
	Island_SetReloadEnableLocal("PuertoRico", "reload_2", false);
	Island_SetReloadEnableLocal("PuertoRico", "reload_3", false);

	string sModel = "";
	string sShipKey = "";
	int iShip = 0;
	int iCannon = 0;
	ref chr;

	Group_FindOrCreateGroup("SpanishGenerosityShips");
	Group_SetType("SpanishGenerosityShips", "war");

	// Gregg: четыре корабля вместо трёх в оригинале. У игрока к этому моменту форт
	// на своей стороне, мановар Кувалды и фрегат Бэлами - втроём против трёх бой
	// решается слишком уверенно. Четвёртый - конвойный линкор без имени собственного,
	// казна по-прежнему только на флагмане.
	for (int i = 1; i <= 4; i++)
	{
		sModel = "off_spa_1";
		sShipKey = "BO_AlteaName";
		iShip = SHIP_SANTAANNA;
		iCannon = CANNON_TYPE_CANNON_LBS42;
		if (i == 2)
		{
			sModel = "off_spa_2";
			sShipKey = "BO_AheldioName";
			iShip = SHIP_DUTCHLINESHIP;
			iCannon = CANNON_TYPE_CULVERINE_LBS32;
		}
		if (i == 3)
		{
			sModel = "PGG_Espinosa_0";
			sShipKey = "BO_AndromedaName";
			iShip = SHIP_LINESHIP;
			iCannon = CANNON_TYPE_CULVERINE_LBS32;
		}
		if (i == 4)
		{
			sModel = "off_spa_2";
			sShipKey = "BO_EskolteName";
			iShip = SHIP_LINESHIP;
			iCannon = CANNON_TYPE_CANNON_LBS32;
		}

		chr = GetCharacter(NPC_GenerateCharacter("SpanishGenerosityShip_" + i, sModel, "man", "man", 30, SPAIN, 0, true));
		chr.name = iTxt("BO_SGCap" + i + "Name");
		chr.lastname = iTxt("BO_SGCap" + i + "LastName");
		chr.Abordage.Enable = true;
		if (i == 1) FantomMakeCoolestSailor(chr, iShip, iTxt(sShipKey), iCannon, 100, 100, 100);
		else FantomMakeCoolSailor(chr, iShip, iTxt(sShipKey), iCannon, 100, 100, 100);
		FantomMakeCoolFighter(chr, 30, 100, 100, "", "", 100);

		// Gregg: «Альтеа» - не рядовая «Санта-Анна», а флагман эскадры со своими статами.
		// Править ShipsTypes нельзя: это шаблон КЛАССА, перестроились бы все «Санта-Анны»
		// в игре. Но FantomMakeCoolSailor уже завёл кораблю отдельный слот в RealShips
		// через GenerateShipExt -> CreateBaseShip, и вот его править можно свободно.
		// Против штатной «Санты»: корпус меньше, зато быстрее, вертлявее, круче к ветру
		// и вместительнее - под казну эскадры в трюме.
		if (i == 1)
		{
			ref rAltea = GetRealShip(sti(chr.Ship.Type));
			rAltea.HP = 14500.0;
			rAltea.SpeedRate = 11.0;
			rAltea.TurnRate = 7.0;
			rAltea.WindAgainstSpeed = 3.0;
			rAltea.Capacity = 8250;
			// Gregg: перечитываем статы в персонажа - SetBaseShipData отработал внутри
			// помощника ДО наших правок и взял старые значения.
			SetBaseShipData(chr);
			SetCrewQuantityFull(chr);
		}
		LAi_NPC_Equip(chr, 30, true, true);
		BO_SetSeaPerks(chr);
		if (CheckAttribute(chr, "AlwaysFriend")) DeleteAttribute(chr, "AlwaysFriend");
		chr.AlwaysEnemy = true;
		chr.CanCrewDamage = true;
		Group_AddCharacter("SpanishGenerosityShips", "SpanishGenerosityShip_" + i);
	}

	// Gregg: казна эскадры в трюме флагмана - оригинал её обещает и не кладёт.
	// Взял «Альтею» на абордаж - получил деньги, пустил на дно - утопил вместе с ней.
	PChar.quest.SpanishGenerosityFlagmanCapture.win_condition.l1 = "Character_Capture";
	PChar.quest.SpanishGenerosityFlagmanCapture.win_condition.l1.character = "SpanishGenerosityShip_1";
	PChar.quest.SpanishGenerosityFlagmanCapture.function = "SpanishGenerosityFlagmanCapture";

	PChar.quest.SpanishGenerosityFlagmanSink.win_condition.l1 = "Character_sink";
	PChar.quest.SpanishGenerosityFlagmanSink.win_condition.l1.character = "SpanishGenerosityShip_1";
	PChar.quest.SpanishGenerosityFlagmanSink.function = "SpanishGenerosityFlagmanSink";

	Group_SetGroupCommander("SpanishGenerosityShips", "SpanishGenerosityShip_1");
	Group_SetTaskAttack("SpanishGenerosityShips", PLAYER_GROUP);
	// Gregg: reload_fort1_siege, а не reload_fort1: в модели острова первого нет вовсе
	// (PuertoRico_locators.gm), и эскадра вставала не туда. Это штатная точка подхода
	// осаждающих к форту - ровно там ей и место.
	Group_SetAddress("SpanishGenerosityShips", "PuertoRico", "quest_ships", "reload_fort1_siege");
	Group_LockTask("SpanishGenerosityShips");

	PChar.quest.SpanishGenerosityFinalBattleWin.win_condition.l1 = "Group_Death";
	PChar.quest.SpanishGenerosityFinalBattleWin.win_condition.l1.group = "SpanishGenerosityShips";
	PChar.quest.SpanishGenerosityFinalBattleWin.function = "SpanishGenerosityFinalBattleWin";
}

void SpanishGenerosityFlagmanCapture(string qName)
{
	PChar.SpanishGenerosity.Treasure = true;
	PChar.quest.SpanishGenerosityFlagmanSink.over = "yes";
	AddMoneyToCharacter(PChar, BO_SG_TREASURE);
	Log_Info(iTxt("BO_SGTreasureTaken"));
}

void SpanishGenerosityFlagmanSink(string qName)
{
	PChar.quest.SpanishGenerosityFlagmanCapture.over = "yes";
	Log_Info(iTxt("BO_SGTreasureLost"));
}

void SpanishGenerosityFinalBattleWin(string qName)
{
	PChar.SpanishGenerosity = "16";
	DeleteAttribute(PChar, "SpanishGenerosity.SiegeNow");

	bQuestDisableMapEnter = false;
	Island_SetReloadEnableLocal("PuertoRico", "reload_1", true);
	Island_SetReloadEnableLocal("PuertoRico", "reload_2", true);
	Island_SetReloadEnableLocal("PuertoRico", "reload_3", true);

	Group_DeleteGroup("SpanishGenerosityShips");
	PChar.quest.SpanishGenerosityFlagmanCapture.over = "yes";
	PChar.quest.SpanishGenerosityFlagmanSink.over = "yes";
	// Gregg: спасательные узлы дальше не нужны - защитник, утонувший через месяц
	// в рядовой стычке, квест уже не касается
	PChar.quest.SpanishGenerosityKuvaldaSink.over = "yes";
	PChar.quest.SpanishGenerosityKuvaldaCapture.over = "yes";
	PChar.quest.SpanishGenerosityBelamiSink.over = "yes";
	PChar.quest.SpanishGenerosityBelamiCapture.over = "yes";

	// Gregg: осада кончилась - удвоение прочности снимаем, максимум возвращаем к
	// штатному (пушки х100), текущее ХП зажимаем в новый потолок
	ref rFort = CharacterFromID("SanJuan Fort Commander");
	DeleteAttribute(rFort, "Fort.QuestHPMult");
	if (CheckAttribute(rFort, "Fort.Cannons.Quantity"))
	{
		rFort.Fort.HP = sti(rFort.Fort.Cannons.Quantity) * 100;
		if (stf(rFort.Ship.HP) > stf(rFort.Fort.HP)) rFort.Ship.HP = rFort.Fort.HP;
	}

	string sRec = "17";
	if (CheckAttribute(PChar, "SpanishGenerosity.Treasure")) sRec = "18";
	BO_SpanishRecord(sRec);

	SetQuestFunction("SpanishGenerosityFinalBattleEnd", "SpanishGenerosityFinalBattleEnd", "l", "SanJuan_town");
}

void SpanishGenerosityFinalBattleEnd(string qName)
{
	BO_SGReturnAlly("Cristobal Kuvalda", "Cristobal Kuvalda");
	BO_SGReturnAlly("Roger Belami", "Cristobal Kuvalda");

	ref chr = CharacterFromID("Alehon Kendart");
	LAi_SetStayType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	chr.Dialog.CurrentNode = "40";

	// Gregg: губернатор колонии. Налог считает его собственный диалог - там же
	// и множитель за свободный заход пиратов, см. BO_Reksvell.c.
	chr = GetCharacter(NPC_GenerateCharacter("Robert Reksvell", "trader_5", "man", "man", 25, PIRATE, -1, false));
	chr.name = iTxt("BO_ReksvellName");
	chr.lastname = iTxt("BO_ReksvellLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Reksvell.c";
	chr.Dialog.CurrentNode = "First Time";
	chr.greeting = "Gr_questMan";
	chr.NotRemoved = true;
	LAi_SetHuberType(chr);
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "SanJuan_townhall", "sit", "sit1");
	SetSelfSkill(chr, 20, 20, 20, 20, 50);
	SetShipSkill(chr, 100, 100, 20, 20, 20, 50, 20, 35, 45);

	ref colony = &Colonies[FindColony("SanJuan")];
	colony.OfficerIdx = GetCharacterIndex("Robert Reksvell");
	// Gregg: без ColonyIdx его диалог не найдёт колонию и посчитает налог
	// по заглушке FortValue = 20 вместо настоящих 75 (Enc_Officer_dialog.c:1971).
	chr.ColonyIdx = FindColony("SanJuan");

	SaveCurrentNpcQuestDateParam(chr, "StartTaxDate");
	SaveCurrentNpcQuestDateParam(chr, "GoverTalkDate");
	LAi_LoginInCaptureTown(chr, true);

	ReOpenQuestHeader("Gen_CityCapture");
	AddQuestRecord("Gen_CityCapture", "t3_1");
	AddQuestUserData("Gen_CityCapture", "sCity", GetCityName("SanJuan"));
	AddQuestUserData("Gen_CityCapture", "sName", GetFullName(chr));
}

void SpanishGenerosityKuvaldaSink(string qName)
{
	BO_SGRescueAlly("Cristobal Kuvalda");
}

void SpanishGenerosityKuvaldaCapture(string qName)
{
	BO_SGRescueAlly("Cristobal Kuvalda");
}

void SpanishGenerosityBelamiSink(string qName)
{
	BO_SGRescueAlly("Roger Belami");
}

void SpanishGenerosityBelamiCapture(string qName)
{
	BO_SGRescueAlly("Roger Belami");
}

// Gregg: союзник пережил гибель корабля. Поднимаем HP (идиома абордажа:
// LAi_SetCurHPMax в LAi_boarding.c, «нужно, чтоб был живой»), убираем из эскадры,
// помечаем на выдачу нового корабля в BO_SGReturnAlly. Killer стираем: живому
// человеку статус утопленника не положен.
void BO_SGRescueAlly(string sId)
{
	if (GetCharacterIndex(sId) == -1) return;
	ref chr = CharacterFromID(sId);
	LAi_SetCurHPMax(chr);
	DeleteAttribute(chr, "Killer");
	RemoveCharacterCompanion(PChar, chr);
	chr.BO_SG.Sunk = true;
	Log_Info(GetFullName(chr) + iTxt("BO_SGAllyRescued"));
}

void BO_SGReturnAlly(string sId, string sLeadId)
{
	if (GetCharacterIndex(sId) == -1) return;
	ref chr = CharacterFromID(sId);
	// Gregg: спасённому после гибели корабля строим новый той же постройки, параметры
	// лежат с BO_SGPlaceAlly. Осиротевший слот RealShips приберёт сборщик на загрузке:
	// ClearAllFantomShips лочит только корабли живых владельцев.
	if (CheckAttribute(chr, "BO_SG.Sunk"))
	{
		DeleteAttribute(chr, "BO_SG.Sunk");
		FantomMakeCoolSailor(chr, sti(chr.BO_SG.ship), iTxt(chr.BO_SG.shipname), sti(chr.BO_SG.cannon), 100, 100, 100);
	}
	RemoveCharacterCompanion(PChar, chr);
	LAi_SetImmortal(chr, true);
	SetBaseShipData(chr);
	SetCrewQuantityFull(chr);
	chr.Ship.Mode = "war";
	chr.ShipEnemyDisable = true;
	chr.DontCheckFlag = true;
	chr.location.from_sea = Colonies[FindColony("SanJuan")].from_sea;
	// Gregg: в CSP у PlaceCharacterShip два лишних против К:ПС аргумента - сторожевик
	// колонии и индекс главного сторожевика, под чью группу он встаёт (islandships.c:183).
	PlaceCharacterShip(GetCharacterIndex(sId), PIRATE, "PuertoRico", FindColony("SanJuan"), true, GetCharacterIndex(sLeadId));
	SetCharacterRelationBoth(GetCharacterIndex(sId), GetMainCharacterIndex(), RELATION_FRIEND);
	SetCaptanModelByEncType(chr, "war");
	Fantom_SetCannons(chr, "war");
	Fantom_SetBalls(chr, "war");
	Fantom_SetGoods(chr, "war");
}

void SpanishGenerosityHappyEnd()
{
	bQuestDisableMapEnter = false;
	PChar.SpanishGenerosity = "END";
	BO_SpanishRecord("19");
	CloseQuestHeader("SpanishGenerosity");
	pchar.questdata.SpanishGenerosity = 1;	// LEO: отметка в энциклопедии
	UnlockAchievement("BlackOrchid_quest", 3);	// LEO: трилогия "Чёрная Орхидея" закончена

	// Gregg: с этой минуты Сан-Хуан живёт по нашему расписанию осад
	BO_SanJuanSiegeArm();

	// Gregg: через неделю Кендарт зовёт снова - глава 7
	SetTimerFunction("_SmallMongrelStart", 0, 0, BO_SM_START_DAYS);
}
// Gregg: <== глава 6

// Gregg: ==> глава 7. «Маленький ублюдок»
// Кендарт нашёл след Проклятых часов: они у контрабандиста по кличке Харпич, ходит
// на судне «Маленький ублюдок». Цепочка расспросов ведёт с Бермуд через Кюрасао
// и Мартинику в Пуэрто-Принсипе, финал - перехват и абордаж в море.

// Gregg: запись журнала главы 7 с подстановкой рода. sSex1 - обычное прошедшее
// время (пошёл/пошла через полные формы), sSex - возвратные глаголы.
// Gregg: закрыть бой. Без этого EnemyFight остаётся врагом игрока, и посторонние
// на локации - тюремщики, солдаты, горожане - продолжают на него кидаться уже после
// того, как противник упал. Штатный набор CSP, см. captain_blood_line_functions.c.
void BO_EndFight()
{
	LAi_SetFightMode(PChar, false);
	LAi_SetFightModeForOfficers(false);
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	// Gregg: LAi_group_FightGroups раздувает радиусы слышимости и переклички до
	// сорока миллионов - о драке узнаёт вообще вся локация. Вернуть их к штатным
	// умеет только эта функция, иначе посторонние продолжают реагировать на ГГ.
	LAi_group_NotFightPlayerVSGroup("EnemyFight");
	// Gregg: и снять цели. Пока шёл бой, тюремщики и прочие успевали взять ГГ на
	// прицел, а по окончании боя переключались уже прямо на него - отсюда и агр
	// именно ПОСЛЕ драки, а не в начале.
	LAi_group_ClearAllTargets();
}

void BO_SMRecord(string sRec)
{
	AddQuestRecord("SmallMongrel", sRec);
	AddQuestUserData("SmallMongrel", "sSex", GetSexPhrase("ся", "ась"));
	AddQuestUserData("SmallMongrel", "sSex1", GetSexPhrase("", "а"));
}

void _SmallMongrelStart(string qName)
{
	ref chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart7.c";
	chr.Dialog.CurrentNode = "1";
	ChangeCharacterAddressGroup(chr, "SanJuan_townhall", "goto", "governor1");
	LAi_SetStayType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	PChar.SmallMongrel = "1";
}

void SmallMongrelStart()
{
	PChar.SmallMongrel = "1";

	SetQuestHeader("SmallMongrel");
	BO_SMRecord("1");

	// Gregg: резиденция Джекмена закрывается - и это НЕ на время квеста. В оригинале
	// Баррет с этого момента насовсем становится привратником и берёт плату за вход.
	// Пиратская линия CSP Джекменом пользуется, но тысяча золотых на этом этапе игры
	// не преграда, а дверь всегда можно выкупить.
	Locations[FindLocation("Pirates_town")].reload.l3.disable = 1;
	PChar.quest.SmallMongrelResidenceClosed.win_condition.l1 = "locator";
	PChar.quest.SmallMongrelResidenceClosed.win_condition.l1.location = "Pirates_town";
	PChar.quest.SmallMongrelResidenceClosed.win_condition.l1.locator_group = "reload";
	PChar.quest.SmallMongrelResidenceClosed.win_condition.l1.locator = "reload3_back";
	PChar.quest.SmallMongrelResidenceClosed.function = "SmallMongrelResidenceClosed";
}

void SmallMongrelResidenceClosed(string qName)
{
	PChar.SmallMongrel = "2";
	Log_InfoS(iTxt("BO_SM_ResidenceShut"));
}

// Gregg: плата Баррету за вход к Джекмену. Разовый пропуск закрывается сразу, как
// только ГГ вошёл в резиденцию, месячный - по таймеру.
void BO_BermudesResidenceOpen(int iMoney, string sType)
{
	AddMoneyToCharacter(PChar, -iMoney);
	Locations[FindLocation("Pirates_town")].reload.l3.disable = 0;

	if (sType == "m")
	{
		SetTimerFunction("BO_BermudesResidenceClose", 0, 0, BO_SM_MONTH_DAYS);
		return;
	}
	PChar.quest.BO_BermudesResidenceClose.win_condition.l1 = "location";
	PChar.quest.BO_BermudesResidenceClose.win_condition.l1.location = "Pirates_townhall";
	PChar.quest.BO_BermudesResidenceClose.function = "BO_BermudesResidenceClose";
}

void BO_BermudesResidenceClose(string qName)
{
	Log_InfoS(iTxt("BO_SM_ResidenceShut"));
	Locations[FindLocation("Pirates_town")].reload.l3.disable = 1;
}

void SmallMongrelFindBarret()
{
	PChar.SmallMongrel = "3";

	ref chr = GetCharacter(NPC_GenerateCharacter("Alen Barret", "officer_8", "man", "man", 1, PIRATE, -1, false));
	chr.rank = 25;
	chr.name = iTxt("BO_BarretName");
	chr.lastname = iTxt("BO_BarretLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Barret.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_EvilPirate";
	// Gregg: он остаётся привратником навсегда, поэтому не даём его вычистить
	chr.NotRemoved = true;
	FantomMakeCoolFighter(chr, 100, 100, 100, "blade18", "", 50);
	ChangeCharacterAddressGroup(chr, "Shore_ship1", "goto", "goto1");
	LAi_SetCitizenType(chr);
	LAi_SetImmortal(chr, true);

	// Gregg: сценарный проигрыш. Диспетч по атрибуту, кейс в quests_reaction.c не нужен.
	BO_SMRecord("13");
	PChar.quest.SmallMongrel_1.function = "SmallMongrelBattleWithBarretLoose";
}

void SmallMongrelBattleWithBarret()
{
	chrDisableReloadToLocation = true;

	ref chr = CharacterFromID("Alen Barret");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	// Gregg: в оригинале бой был сценарным проигрышем ГГ, и это плохо ложилось: у сильного
	// героя он мог не кончиться вовсе, таймера в К:ПС не было. Здесь наоборот - Баррет
	// смертен, держит BO_SM_BARRET_HP, и разговор начинается, когда его сбили. Бессмертие
	// вернёт сам чекер: третий аргумент true.
	LAi_SetImmortal(chr, false);
	LAi_SetHP(chr, BO_SM_BARRET_HP, BO_SM_BARRET_HP);
	LAi_SetCheckMinHP(chr, 1, true, "SmallMongrel_1");
}

// Gregg: Баррет сбит. Имя функции оставлено прежним - на него ссылается атрибут квеста.
void SmallMongrelBattleWithBarretLoose(string qName)
{
	BO_EndFight();
	ref chr = CharacterFromID("Alen Barret");
	chr.Dialog.CurrentNode = "7";

	LAi_SetFightMode(PChar, false);
	LAi_SetHP(chr, 100.0, 100.0);
	LAi_RemoveCheckMinHP(chr);
	LAi_UseAtidoteBottle(chr);
	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

void SmallMongrelToJackman()
{
	chrDisableReloadToLocation = false;

	PChar.SmallMongrel = "4";
	BO_SMRecord("2");

	LAi_SetPlayerType(PChar);
	LAi_SetCurHPMax(PChar);

	// Gregg: Баррет уходит с пристани в город. Бессмертие ему возвращаем - дальше он
	// привратник, и убивать его нельзя.
	LAi_SetImmortal(CharacterFromID("Alen Barret"), true);

	// Gregg: Баррет уходит с пристани в город. Порядок важен: смена типа персонажа
	// ПОСЛЕ LAi_ActorRunToLocation отменяет состояние актёра, поэтому группу меняем
	// до запуска, а не после, и таймаут ставим конечный - с -1 актёр, не дошедший
	// до точки, застывает навсегда.
	ref chr = CharacterFromID("Alen Barret");
	LAi_group_MoveCharacter(chr, "PIRATE_CITIZENS");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	SetQuestFunction("SmallMongrelSetBarretToTown", "SmallMongrelSetBarretToTown", "efl", PChar.location);
}

void SmallMongrelSetBarretToTown(string qName)
{
	ref chr = CharacterFromID("Alen Barret");
	LAi_SetStayType(chr);
	ChangeCharacterAddressGroup(chr, "Pirates_town", "goto", "goto17");
}

void SmallMongrelToCuracao()
{
	PChar.SmallMongrel = "5";
	BO_SMRecord("3");
}

void SmallMongrelToCuracaoPortMan()
{
	PChar.SmallMongrel = "6";
}

void SmallMongrelToLeFransua()
{
	PChar.SmallMongrel = "7";
	BO_SMRecord("4");
}

void SmallMongrelInLeFransua()
{
	PChar.SmallMongrel = "8";
	BO_SMRecord("5");

	SetQuestFunction("SmallMongrelToFortFranceTown", "SmallMongrelToFortFranceTown", "l", "FortFrance_town");
}

void SmallMongrelToFortFranceTown(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("SmallMongrel Janson", "citiz_9", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_JansonName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Janson.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_Tavern_Mate";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "FortFrance_town", "goto", "goto9");
	SetDialogCharacter2Character(PChar, chr, 25.0, false);

	// Gregg: по прибытии в подземелье обоим вернуть управление
	PChar.quest.SmallMongrel_2.function = "SmallMongrelDungeonArrive";
}

void SmallMongrelDungeonArrive(string qName)
{
	LAi_SetPlayerType(PChar);
}

void SmallMongrelToDungeon()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("SmallMongrel Janson");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocator(chr, "reload", "basement1", "SmallMongrel_2", 10.0);

	LAi_SetActorType(PChar);
	LAi_ActorRunToLocator(PChar, "reload", "basement1", "SmallMongrel_2", 30.0);

	SetQuestFunction("SmallMongrelInDungeon", "SmallMongrelInDungeon", "l", "FortFrance_Dungeon");
}

void SmallMongrelInDungeon(string qName)
{
	PChar.SmallMongrel = "9";

	ref chr = GetCharacter(NPC_GenerateCharacter("SmallMongrel Frank", "citiz_5", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_FrankName");
	chr.lastname = "";
	chr.greeting = "Gr_Tavern_Mate";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Janson.c";
	chr.Dialog.CurrentNode = "6";
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);
	// Gregg: не monster35 - на нём стоит Вейн из «Куртуазных страстей» (black_sails_functions.c).
	ChangeCharacterAddressGroup(chr, "FortFrance_Dungeon", "monsters", "monster36");
}

void SmallMongrelFromDungeon()
{
	ref chr = CharacterFromID("SmallMongrel Frank");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "", 30.0);

	PChar.SmallMongrel = "10";
	SetQuestFunction("SmallMongrelTalkWithJanson", "SmallMongrelTalkWithJanson", "l", "FortFrance_town");
}

void SmallMongrelTalkWithJanson(string qName)
{
	chrDisableReloadToLocation = true;

	ref frank = CharacterFromID("SmallMongrel Frank");
	ChangeCharacterAddressGroup(frank, "FortFrance_town", "goto", "goto9");
	LAi_SetStayType(frank);

	ref chr = CharacterFromID("SmallMongrel Janson");
	chr.Dialog.CurrentNode = "14";
	ChangeCharacterAddressGroup(chr, "FortFrance_town", "goto", "goto22");

	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

void SmallMongrelTalkWithFrank()
{
	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("SmallMongrel Frank");
	chr.Dialog.CurrentNode = "22";
	// Gregg: Фрэнк подходит сам. Искать пьяного грузчика по городу игроку неоткуда -
	// Янсон говорит только «приходите в лавку через пару дней», а Фрэнк всё это время
	// стоит на улице. Он и так увязался за спасителем, так что пусть догоняет.
	// Таймаут конечный: с -1 актёр залипает в «falure», если маршрут не сложится.
	LAi_SetActorType(chr);
	LAi_ActorDialog(chr, PChar, "", 30.0, -1.0);
	// Gregg: и значок - на случай, если игрок успел уйти раньше, чем Фрэнк дошёл.
	SetQuestMarkToCharacter(chr, true);
}

void SmallMongrelSecondTalkWithJanson()
{
	// Gregg: Фрэнк своё сказал - возвращаем его на место и снимаем значок.
	ref frank = CharacterFromID("SmallMongrel Frank");
	LAi_SetStayType(frank);
	SetQuestMarkToCharacter(frank, false);

	ref chr = CharacterFromID("SmallMongrel Janson");
	chr.Dialog.CurrentNode = "28";

	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

void SmallMongrelLeFransuaStoreTimer()
{
	LAi_SetPlayerType(PChar);
	chrDisableReloadToLocation = false;

	ref frank = CharacterFromID("SmallMongrel Frank");
	LAi_SetActorType(frank);
	LAi_ActorRunToLocation(frank, "reload", "gate_back", "none", "", "", "", 30.0);

	ref chr = CharacterFromID("SmallMongrel Janson");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "gate_back", "none", "", "", "", 30.0);

	PChar.SmallMongrel = "11";
	BO_SMRecord("6");

	SetTimerFunction("SmallMongrelLeFransuaStoreTimerEnd", 0, 0, BO_SM_FRANK_DAYS);
}

void SmallMongrelLeFransuaStoreTimerEnd(string qName)
{
	PChar.SmallMongrel = "12";
}

void SmallMongrelToLeFransuaStoreRoom()
{
	PChar.SmallMongrel = "13";

	ref chr = CharacterFromID("SmallMongrel Frank");
	chr.Dialog.CurrentNode = "32";

	// Gregg: в оригинале Фрэнка сажали в отдельную LeFransua_store_room и на время
	// квеста перевешивали туда дверь лавки. В CSP такой локации нет, а reload.l2 лавки
	// и без того ведёт в подсобку CommonPackhouse_1 - сажаем прямо туда и дверь
	// не трогаем вовсе. Перевешивание общих дверей уже один раз выходило боком.
	ChangeCharacterAddressGroup(chr, "CommonPackhouse_1", "goto", "goto1");
	LAi_SetStayType(chr);
}

void SmallMongrelToPuertoPrincipe()
{
	PChar.SmallMongrel = "14";
	BO_SMRecord("7");

	Characters[GetCharacterIndex("SmallMongrel Frank")].LifeDay = 0;
	Characters[GetCharacterIndex("SmallMongrel Janson")].LifeDay = 0;

	SetQuestFunction("SmallMongrelInPuertoPrincipe", "SmallMongrelInPuertoPrincipe", "l", "PuertoPrincipe_tavern");
}

void SmallMongrelInPuertoPrincipe(string qName)
{
	chrDisableReloadToLocation = true;
	// Gregg: в модели Tavern09 всего три пары мест, четвёртой нет - в отличие от таверны
	// оригинала. Сажаем на третью.
	FreeSitLocator("PuertoPrincipe_tavern", "sit_front3");
	FreeSitLocator("PuertoPrincipe_tavern", "sit_base3");

	ref chr = GetCharacter(NPC_GenerateCharacter("Will Breddoks", "trader_9", "man", "man", 1, PIRATE, -1, true));
	chr.name = iTxt("BO_BreddoksName");
	chr.lastname = iTxt("BO_BreddoksLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Breddoks.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_Tavern_Mate";
	LAi_SetSitType(chr);
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, "PuertoPrincipe_tavern", "sit", "sit_base3");
	SetQuestMarkToCharacter(chr, true);
}

void SmallMongrelInPuertoPrincipeTalk()
{
	ref chr = CharacterFromID("Will Breddoks");
	chr.Dialog.CurrentNode = "2";

	FreeSitLocator("PuertoPrincipe_tavern", "sit_front3");
	ChangeCharacterAddressGroup(PChar, "PuertoPrincipe_tavern", "sit", "sit_front3");
	LAi_SetSitType(PChar);
	LAi_SetActorType(chr);
	LAi_ActorSetSitMode(chr);
	LAi_ActorDialogNow(chr, PChar, "", -1);
}

void SmallMongrelFindKharpich()
{
	ref rWill = CharacterFromID("Will Breddoks");
	LAi_SetSitType(rWill);
	SetQuestMarkToCharacter(rWill, false);
	ChangeCharacterAddressGroup(PChar, "PuertoPrincipe_tavern", "goto", "goto1");
	LAi_SetPlayerType(PChar);

	chrDisableReloadToLocation = false;
	PChar.SmallMongrel = "15";
	BO_SMRecord("8");

	ref chr = GetCharacter(NPC_GenerateCharacter("Kharpich", "officer_1", "man", "man", 25, PIRATE, -1, false));
	chr.name = iTxt("BO_KharpichName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Breddoks.c";
	chr.Dialog.CurrentNode = "20";
	// Gregg: топовая генерация. GenerateShip прогоняет статы через SetShipsBonuses и режет
	// часть стволов на rand(MaxCannonDiff); GenerateShipTop ставит QuestShip и оставляет
	// корабль в полной комплектации. Уникальные статы ниже ложатся поверх и не сбиваются:
	// на QuestShip бонусная раскрутка не идёт.
	chr.Ship.Type = GenerateShipTop(SHIP_LEOPARD, true, chr);
	chr.Ship.Name = iTxt("BO_MongrelShipName");
	chr.CanCrewDamage = true;
	chr.DontRansackCaptain = true;
	chr.AlwaysEnemy = true;
	// Gregg: SetGeraldDetails из оригинала выброшен - он пишет в GetRealShip(тип),
	// то есть в ОБЩИЙ шаблон класса, и перекрашивает все корабли этого типа в мире.

	// Gregg: «Маленький ублюдок» - уникальный экземпляр на базе английского тяжёлого
	// фрегата. Правим слот конкретного корабля в RealShips, а не шаблон класса.
	ref rMongrel = GetRealShip(sti(chr.Ship.Type));
	rMongrel.HP = 7500.0;
	rMongrel.SpeedRate = 13.0;
	rMongrel.TurnRate = 11.0;
	rMongrel.Capacity = 5000;
	SetBaseShipData(chr);
	// Gregg: в оригинале было 100 человек - под бригантину с полутора сотнями мест.
	// На тяжёлом фрегате это пустой корабль: абордаж превращался в прогулку.
	SetCrewQuantityFull(chr);

	// LEO: боезапас по числу пушек; стартовых 20-200 снарядов может не хватить даже на зарядку картечью.
	Fantom_SetBalls(chr, "pirate");

	FantomMakeCoolFighter(chr, 30, 50, 50, "blade20", "pistol6", 100);
	LAi_SetStayType(chr);
	// Gregg: бессмертия тут НЕТ, в отличие от оригинала. «Маленького ублюдка» можно
	// утопить, и тогда часы уходят на дно вместе с ним - это провал всей линии.
	// Каютный диалог держится на LAi_SetCheckMinHP клона в SetQuestAboardCabinDialog,
	// бессмертие оригинала ему не нужно.
	// Gregg: каютный диалог на абордаже вешается не отсюда, а веткой по CaptanId
	// в SetQuestAboardCabinDialog (characters\QuestsUtilite.c) - там же, где Ферегуэйро.

	string sGroup = "KharpichGroup";
	Group_FindOrCreateGroup(sGroup);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Group_AddCharacter(sGroup, chr.id);
	Group_SetGroupCommander(sGroup, chr.id);

	chr.mapEnc.type = "trade";
	chr.mapEnc.worldMapShip = "quest_ship";
	chr.mapEnc.Name = iTxt("BO_MongrelShipName");
	chr.CapCurrentShore = "Shore32";
	Map_CreateTrader("Shore11", "Shore32", chr.id, 10);

	PChar.quest.KharpichEncounterArm.win_condition.l1 = "MapEnter";
	PChar.quest.KharpichEncounterArm.function = "KharpichEncounterArm";

	PChar.quest.SmallMongrelCapture.win_condition.l1 = "Character_Capture";
	PChar.quest.SmallMongrelCapture.win_condition.l1.character = "Kharpich";
	PChar.quest.SmallMongrelCapture.function = "SmallMongrelCapture";

	PChar.quest.SmallMongrelSink.win_condition.l1 = "Character_Sink";
	PChar.quest.SmallMongrelSink.win_condition.l1.character = "Kharpich";
	PChar.quest.SmallMongrelSink.function = "SmallMongrelSink";

	Characters[GetCharacterIndex("Will Breddoks")].LifeDay = 0;
}

void KharpichEncounterArm(string qName)
{
	SetTimerFunction("KharpichEncounterMove", 0, 0, BO_SM_SHUTTLE_DAYS);
	// Gregg: .again держит узел живым, поэтому OnQuestComplete не ставит over и чистить
	// его вручную не нужно - в отличие от самоперевзводных таймеров главы 6.
	PChar.quest.KharpichEncounterMove.again = true;
}

void KharpichEncounterMove(string qName)
{
	if (GetCharacterIndex("Kharpich") == -1)
	{
		PChar.quest.KharpichEncounterMove.over = "yes";
		return;
	}

	ref chr = CharacterFromID("Kharpich");
	string sCurrentShore = "";
	if (CheckAttribute(chr, "CapCurrentShore")) sCurrentShore = chr.CapCurrentShore;

	string sTargetShore = "Shore11";
	if (sCurrentShore == "Shore11") sTargetShore = "Shore32";

	Map_ReleaseQuestEncounter("Kharpich");

	string sGroup = "KharpichGroup";
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Group_AddCharacter(sGroup, chr.id);
	Group_SetGroupCommander(sGroup, chr.id);

	chr.mapEnc.type = "trade";
	chr.mapEnc.worldMapShip = "quest_ship";
	chr.mapEnc.Name = iTxt("BO_MongrelShipName");
	chr.CapCurrentShore = sTargetShore;
	Map_CreateTrader(sCurrentShore, sTargetShore, chr.id, 10);

	SetTimerFunction("KharpichEncounterMove", 0, 0, BO_SM_SHUTTLE_DAYS);
}

// Gregg: провал. Часы были на борту, борта больше нет - продолжать нечем.
void SmallMongrelSink(string qName)
{
	PChar.quest.KharpichEncounterMove.over = "yes";
	Group_DeleteGroup("KharpichGroup");

	PChar.SmallMongrel = "FAILED";
	BO_SMRecord("12");
	BO_QuestFailed("SmallMongrel");
}

void SmallMongrelCapture(string qName)
{
	bQuestDisableMapEnter = true;

	PChar.SmallMongrel = "16";
	BO_SMRecord("9");

	PChar.SmallMongrel.CaptureLocation = PChar.location;
	Island_SetReloadEnableGlobal(PChar.SmallMongrel.CaptureLocation, false);

	// Gregg: узел потопления снимаем, иначе после абордажа его ещё можно утопить
	// и получить запись о провале поверх успеха.
	PChar.quest.SmallMongrelSink.over = "yes";
	DeleteAttribute(PChar, "quest.SmallMongrelSink");

	// Gregg: часы кладутся в сундук каюты САМОГО «Ублюдка» ещё на абордаже - см. ветку
	// по CaptanId в SetQuestAboardCabinDialog. В оригинале они появлялись в каюте
	// ИГРОКА (жёстко My_Cabin_Medium), и это ломалось дважды: на мановаре каюта другая,
	// а если пересесть на приз - тем более. Здесь искать надо там, где по сюжету и надо:
	// на захваченном судне.

	// Gregg: в К:ПС подбор ловил общий хук CheckNewItemsForQuests, которого в CSP нет.
	// Здесь для этого есть штатное условие квеста по предмету.
	PChar.quest.SmallMongrelFindClock.win_condition.l1 = "item";
	PChar.quest.SmallMongrelFindClock.win_condition.l1.item = "DamnedClock";
	PChar.quest.SmallMongrelFindClock.function = "SmallMongrelFindClock";
}

void SmallMongrelFindClock(string qName)
{
	bQuestDisableMapEnter = false;
	Island_SetReloadEnableGlobal(PChar.SmallMongrel.CaptureLocation, true);

	PChar.SmallMongrel = "17";
	BO_SMRecord("10");
}

void SmallMongrelEnd()
{
	BO_SMRecord("11");
	CloseQuestHeader("SmallMongrel");
	pchar.questdata.SmallMongrel = 1;	// LEO: отметка в энциклопедии

	PChar.SmallMongrel = "END";

	// Gregg: отпираем резиденцию насовсем. В оригинале она оставалась платной до конца
	// игры, но проверил весь код К:ПС: на этом флаге не завязано ничего, кроме самого
	// диалога Баррета. В CSP Джекмен нужен пиратской линии, и налог ей ни к чему.
	Locations[FindLocation("Pirates_town")].reload.l3.disable = 0;
	if (CheckAttribute(PChar, "quest.BO_BermudesResidenceClose"))
	{
		DeleteAttribute(PChar, "quest.BO_BermudesResidenceClose");
	}

	SetTimerFunction("RaulEsperantoTime", 0, 0, BO_SM_NEXT_DAYS);
	AddSimpleRumourToAllNations(iTxt("BO_SM_Rumour1") + GetFullName(PChar) + iTxt("BO_SM_Rumour2"), 30, 5);
}
// Gregg: <== глава 7

// Gregg: ==> глава 8. «Рауль Эсперанто»
// Кендарт вспомнил, где видел знак: у Рауля Эсперанто, который сидит у инквизиции
// в Сантьяго. Чтобы до него добраться, нужно вытащить из тюрьмы Чарльстауна Джо
// Ньюмена, забрать у пиратов под Тортугой именной мушкет и через него дожать
// банкира Яна Бозена в Мариго.

void RaulEsperantoTime(string qName)
{
	ref chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart8.c";
	chr.Dialog.CurrentNode = "1";
	ChangeCharacterAddressGroup(chr, "SanJuan_townhall", "goto", "governor1");
	LAi_SetStayType(chr);

	// Gregg: в оригинале тюрьма выбиралась случайно из восьми испанских колоний, но сцена
	// с Эсперанто и инквизитором всё равно жёстко в Santiago_Incquisitio - Хорхе называл
	// один город, а ГГ ехал в другой. Фиксируем Сантьяго, чтобы слова сходились с делом.
	PChar.RaulEsperanto.Prison = "Santiago";
	PChar.RaulEsperanto.BeausingBattle = 1;
}

// Gregg: запись журнала главы 8 с подстановкой рода
void BO_RERecord(string sRec)
{
	AddQuestRecord("RaulEsperanto", sRec);
	AddQuestUserData("RaulEsperanto", "sSex", GetSexPhrase("ся", "ась"));
	AddQuestUserData("RaulEsperanto", "sSex1", GetSexPhrase("", "а"));
}

void RaulEsperantoStart()
{
	PChar.RaulEsperanto = "1";
	SetQuestHeader("RaulEsperanto");
	BO_RERecord("1");
}

void RaulEsperantoNewmanTime()
{
	PChar.RaulEsperanto = "2";
	BO_RERecord("2");
	SetTimerFunction("RaulEsperantoNewmanTimeEnd", 0, 0, BO_RE_NEWMAN_DAYS);
}

// Gregg: окно, в которое Ньюмена можно перехватить в форте Чарльстауна. Открывается
// на сутки; не успел - следующая попытка через три месяца.
// Gregg: день Ньюмена. В оригинале сцена была рассчитана на то, что ГГ приходит
// в форт снаружи: закрывалась дверь в тюрьму, у ворот вставал стражник, и только
// после разговора с ним дверь открывалась. Но ждать день игрок может и внутри
// тюрьмы - тогда порядок выворачивался: сначала призрак, потом предупреждение
// о нём. Поэтому всё перенесено в саму тюрьму, а дверями мы больше не играем.
void RaulEsperantoNewmanTimeEnd(string qName)
{
	BO_RERecord("19");

	if (CheckAttribute(PChar, "quest.RaulEsperantoNewmanToFort"))
	{
		DeleteAttribute(PChar, "quest.RaulEsperantoNewmanToFort");
	}
	SetQuestFunction("RaulEsperantoNewmanToFort", "RaulEsperantoNewmanToFort", "l", "Charles_prison");
	SetTimerFunction("RaulEsperantoNewmanTimeFailed", 0, 0, BO_RE_WINDOW_DAYS);

	// Gregg: если ГГ прождал день прямо в тюрьме, условие по входу не сработает
	// никогда - разыгрываем сцену сразу.
	if (PChar.location == "Charles_prison") RaulEsperantoNewmanToFort("");
}

void RaulEsperantoNewmanTimeFailed(string qName)
{
	BO_RERecord("3");
	PChar.quest.RaulEsperantoNewmanToFort.over = "yes";
	if (GetCharacterIndex("RaulEsperanto Guardian") != -1)
	{
		Characters[GetCharacterIndex("RaulEsperanto Guardian")].LifeDay = 0;
	}
	SetTimerFunction("RaulEsperantoNewmanTimeEnd", 0, 0, BO_RE_RETRY_DAYS);
}

// Gregg: тюремщик встречает ГГ у лестницы и предупреждает, что сегодня вниз не ходят.
void RaulEsperantoNewmanToFort(string qName)
{
	PChar.quest.RaulEsperantoNewmanTimeFailed.over = "yes";

	// Gregg: ночь, а не десять утра как в оригинале: сцена про призрака.
	SetCurrentTime(0, 23);
	RecalculateJumpTable();
	RefreshWeather();
	RefreshLandTime();

	if (GetCharacterIndex("RaulEsperanto Guardian") == -1)
	{
		ref chr = GetCharacter(NPC_GenerateCharacter("RaulEsperanto Guardian", "sold_eng_" + (rand(7) + 1), "man", "man", 1, PIRATE, 0, false));
		chr.name = iTxt("BO_GuardianName");
		chr.lastname = "";
		chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Guardian.c";
		chr.Dialog.CurrentNode = "1";
		chr.greeting = "soldier_common";
		LAi_SetImmortal(chr, true);
		LAi_SetStayType(chr);
		// Gregg: goto9 оказался в дальнем конце тюрьмы и камеру выворачивало вдаль -
		// ставим у входа.
		ChangeCharacterAddressGroup(chr, "Charles_prison", "goto", "goto19");
	}
	SetDialogCharacter2Character(PChar, CharacterFromID("RaulEsperanto Guardian"), 15.0, true);
}

// Gregg: ГГ сказал, что спустится. Тюремщик уходит наверх, внизу появляется Ньюмен.
void RaulEsperantoNewmanToPrison()
{
	LAi_SetPlayerType(PChar);
	chrDisableReloadToLocation = true;

	ref grd = CharacterFromID("RaulEsperanto Guardian");
	grd.LifeDay = 0;
	LAi_SetActorType(grd);
	LAi_ActorGoToLocation(grd, "reload", "reload1", "none", "", "", "", 30.0);

	DoQuestFunctionDelay("RaulEsperantoNewmanInPrison", 2.0);
}

void RaulEsperantoNewmanInPrison(string qName)
{
	// Gregg: GetRank в CSP прибавляет ранг героя внутри себя, поэтому берём PChar.rank
	// напрямую - иначе противник получался вдвое сильнее задуманного.
	int iRank = sti(PChar.rank) + 15 + MOD_SKILL_ENEMY_RATE;
	// Gregg: анимации man2 из К:ПС в CSP нет вовсе, а man1 устарела - берём man.
	ref chr = GetCharacter(NPC_GenerateCharacter("Joe Newman", "priest_sp1", "man", "man", iRank, PIRATE, 0, true));
	chr.name = iTxt("BO_NewmanName");
	chr.lastname = iTxt("BO_NewmanLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Guardian.c";
	chr.Dialog.CurrentNode = "2";
	chr.greeting = "Gr_Azzy";
	ChangeCharacterAddressGroup(chr, "Charles_prison", "goto", "goto20");
	FantomMakeCoolFighter(chr, iRank, 50, 50, BLADE_LONG, "pistol5", 100);
	RemoveCharacterEquip(chr, GUN_ITEM_TYPE);

	LAi_SetHP(chr, BO_RE_NEWMAN_HP, BO_RE_NEWMAN_HP);
	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.RaulEsperanto_1.function = "RaulEsperantoTalkWithNewman";
	LAi_SetCheckMinHP(chr, 1, true, "RaulEsperanto_1");
}

void RaulEsperantoTalkWithNewman(string qName)
{
	BO_EndFight();
	ref chr = CharacterFromID("Joe Newman");
	SetDialogCharacter2Character(PChar, chr, 10.0, true);
	LAi_SetHP(chr, 100.0, 100.0);
	LAi_RemoveCheckMinHP(chr);
	LAi_UseAtidoteBottle(chr);
}

void RaulEsperantoNewmanExit()
{
	chrDisableReloadToLocation = false;
	ChangeCharacterAddress(CharacterFromID("Joe Newman"), "None", "");
	LAi_SetPlayerType(PChar);

	BO_RERecord("4");
	SetQuestFunction("RaulEsperantoToSantiagoLightouse", "RaulEsperantoToSantiagoLightouse", "l", "Mayak9");
	BO_SetEncounters("Mayak9", true);
	Locations[FindLocation("Mayak9")].locators_radius.goto.goto31 = 3.0;
}

void RaulEsperantoToSantiagoLightouse(string qName)
{
	ref chr = GetCharacter(NPC_GenerateCharacter("Khorkhe de Maranja", "sold_spa_" + (rand(7) + 1), "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_KhorkheName");
	chr.lastname = iTxt("BO_KhorkheLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Khorkhe.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "soldier_common";
	LAi_SetImmortal(chr, true);
	LAi_SetStayType(chr);
	// Gregg: goto34 в модели Lighthouse нет, максимум goto33 - ставим на него.
	ChangeCharacterAddressGroup(chr, "Mayak9", "goto", "goto31");

	PChar.quest.RaulEsperantoInSantiagoLightouse.win_condition.l1 = "locator";
	PChar.quest.RaulEsperantoInSantiagoLightouse.win_condition.l1.location = "Mayak9";
	PChar.quest.RaulEsperantoInSantiagoLightouse.win_condition.l1.locator_group = "goto";
	PChar.quest.RaulEsperantoInSantiagoLightouse.win_condition.l1.locator = "goto31";
	PChar.quest.RaulEsperantoInSantiagoLightouse.function = "RaulEsperantoInSantiagoLightouse";
}

void RaulEsperantoInSantiagoLightouse(string qName)
{
	// Gregg: bActor=false намеренно. С true игрок сам становится актёром, и выйти из
	// этого состояния должен кто-то явно - а у ветки «приду с деньгами» продолжения
	// нет, и ГГ оставался актёром насовсем.
	SetDialogCharacter2Character(PChar, CharacterFromID("Khorkhe de Maranja"), 5.0, false);
}

void RaulEsperantoToTortuga()
{
	LAi_SetPlayerType(PChar);
	PChar.RaulEsperanto = "4";
	BO_RERecord("5");

	Locations[FindLocation("Mayak9")].locators_radius.goto.goto31 = 0.5;

	SetTimerFunction("RaulEsperantoToTortugaTimer", 0, 0, BO_RE_TORTUGA_DAYS);
	SetQuestFunction("RaulEsperantoInTortuga", "RaulEsperantoInTortuga", "l", "Tortuga");
}

void RaulEsperantoToTortugaTimer(string qName)
{
	PChar.quest.RaulEsperantoInTortuga.over = "yes";
	PChar.RaulEsperanto = "5";
	BO_RERecord("6");
	BO_SetEncounters("Mayak9", false);
	Characters[GetCharacterIndex("Khorkhe de Maranja")].LifeDay = 0;
}

void RaulEsperantoToTortugaFailed(string qName)
{
	Group_DeleteGroup("RaulEsperantoTortugaShip");
	PChar.RaulEsperanto = "5";
	BO_RERecord("6");
	BO_SetEncounters("Mayak9", false);
	Characters[GetCharacterIndex("Khorkhe de Maranja")].LifeDay = 0;
}

void RaulEsperantoInTortuga(string qName)
{
	PChar.quest.RaulEsperantoToTortugaTimer.over = "yes";

	ref sld = GetCharacter(NPC_GenerateCharacter("RaulEsperanto Tortuga Ship", "", "man", "man", 20, PIRATE, -1, true));
	SetCaptanModelByEncType(sld, "pirate");
	// Gregg: в К:ПС тут SHIP_WARSHIP, которого в CSP нет, - тот же тяжёлый фрегат,
	// что и у погони в главе 5. Пират на линкоре под Тортугой смотрелся бы странно.
	FantomMakeCoolSailor(sld, SHIP_FRIGATE_H, iTxt("BO_RE_ShipName"), CANNON_TYPE_CULVERINE_LBS24, 50, 50, 50);
	sld.AlwaysSandbankManeuver = true;
	sld.AnalizeShips = true;
	sld.DontRansackCaptain = true;
	sld.SaveItemsForDead = true;
	sld.AlwaysEnemy = true;
	// Gregg: SetGeraldDetails из оригинала выброшен - он красит общий шаблон класса.
	BO_SetSeaPerks(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");
	TakeNItems(sld, "RaulEsperanto_Key", 1);

	Group_FindOrCreateGroup("RaulEsperantoTortugaShip");
	Group_AddCharacter("RaulEsperantoTortugaShip", "RaulEsperanto Tortuga Ship");
	Group_SetType("RaulEsperantoTortugaShip", "pirate");
	Group_SetGroupCommander("RaulEsperantoTortugaShip", "RaulEsperanto Tortuga Ship");
	Group_SetAddress("RaulEsperantoTortugaShip", "Tortuga", "quest_ships", "quest_ship_7");
	Group_SetTaskAttack("RaulEsperantoTortugaShip", PLAYER_GROUP);

	PChar.quest.RaulEsperantoToTortugaFailed.win_condition.l1 = "MapEnter";
	PChar.quest.RaulEsperantoToTortugaFailed.function = "RaulEsperantoToTortugaFailed";

	PChar.quest.RaulEsperantoInTortugaShipSink.win_condition.l1 = "Character_Sink";
	PChar.quest.RaulEsperantoInTortugaShipSink.win_condition.l1.character = "RaulEsperanto Tortuga Ship";
	PChar.quest.RaulEsperantoInTortugaShipSink.function = "RaulEsperantoInTortugaShipSink";

	PChar.quest.RaulEsperantoInTortugaShipCapture.win_condition.l1 = "Character_Capture";
	PChar.quest.RaulEsperantoInTortugaShipCapture.win_condition.l1.character = "RaulEsperanto Tortuga Ship";
	PChar.quest.RaulEsperantoInTortugaShipCapture.function = "RaulEsperantoInTortugaShipCapture";
}

void RaulEsperantoInTortugaShipSink(string qName)
{
	PChar.quest.RaulEsperantoToTortugaFailed.over = "yes";
	Group_DeleteGroup("RaulEsperantoTortugaShip");
	PChar.RaulEsperanto = "5";

	BO_RERecord("7");
	Characters[GetCharacterIndex("Khorkhe de Maranja")].LifeDay = 0;
}

void RaulEsperantoInTortugaShipCapture(string qName)
{
	PChar.quest.RaulEsperantoToTortugaFailed.over = "yes";
	// Gregg: корабль уже наш - снимаем узел потопления, иначе его можно утопить
	// следом и получить запись о потере поверх записи об успехе.
	PChar.quest.RaulEsperantoInTortugaShipSink.over = "yes";
	DeleteAttribute(PChar, "quest.RaulEsperantoInTortugaShipSink");
	Group_DeleteGroup("RaulEsperantoTortugaShip");
	PChar.RaulEsperanto = "5";

	// Gregg: сундук в каюте призового корабля заводится в SetQuestAboardCabinDialog,
	// там же, где каютные диалоги. Здесь только прибираем за собой.
	if (CheckAttribute(PChar, "RaulEsperanto.CabinKey"))
	{
		string sCabin = PChar.RaulEsperanto.CabinKey;
		int iCab = FindLocation(sCabin);
		if (iCab >= 0)
		{
			Locations[iCab].box1.items.RaulEsperanto_Musket = 0;
			if (CheckAttribute(&Locations[iCab], "box1.key")) DeleteAttribute(&Locations[iCab], "box1.key");
			if (CheckAttribute(&Locations[iCab], "box1.opened")) DeleteAttribute(&Locations[iCab], "box1.opened");
		}
	}

	BO_SetEncounters("Mayak9", false);
	if (GetCharacterItem(PChar, "RaulEsperanto_Musket") >= 1)
	{
		PChar.RaulEsperanto = "4";
		BO_RERecord("9");
		SetQuestFunction("RaulEsperantoToShoreSanAntonio", "RaulEsperantoToShoreSanAntonio", "l", "Shore15");
		ChangeCharacterAddressGroup(CharacterFromID("Khorkhe de Maranja"), "Shore15", "goto", "goto11");
		BO_SetEncounters("Shore15", true);
		return;
	}
	BO_RERecord("8");
	Characters[GetCharacterIndex("Khorkhe de Maranja")].LifeDay = 0;
}

void RaulEsperantoToShoreSanAntonio(string qName)
{
	ref chr = CharacterFromID("Khorkhe de Maranja");
	chr.Dialog.CurrentNode = "12";
	ChangeCharacterAddressGroup(chr, "Shore15", "goto", "goto11");
	SetDialogCharacter2Character(PChar, chr, 5.0, true);
}

void RaulEsperantoToNeedPrison()
{
	BO_SetEncounters("Shore15", false);
	ref chr = CharacterFromID("Khorkhe de Maranja");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	LAi_SetPlayerType(PChar);
	PChar.RaulEsperanto = "5";
	BO_RERecord("10");

	// Gregg: перезаписываем и здесь. В старых сейвах лежит случайная колония, выбранная
	// до того, как я зафиксировал Сантьяго, - иначе Хорхе называет одно место, а ехать
	// надо в другое.
	PChar.RaulEsperanto.Prison = "Santiago";
	string sPrison = PChar.RaulEsperanto.Prison;
	string sKey = "Colony" + sPrison + "Gen";
	AddQuestUserData("RaulEsperanto", "sPrison", XI_ConvertString(sKey));

	// Gregg: сразу взводим шаг в инквизицию - в К:ПС его дёргала реплика, которой
	// у нас нет, и функция висела мёртвой.
	RaulEsperantoToIncquisitio();
}

void RaulEsperantoToIncquisitio()
{
	PChar.RaulEsperanto = "6";
	BO_RERecord("11");
	SetQuestFunction("RaulEsperantoInIncquisitio", "RaulEsperantoInIncquisitio", "l", "Santiago_Incquisitio");

	// Gregg: в оригинале тут сидел Антонио де Соуза. В CSP он занят - сюжетный персонаж
	// испанской линии и линии Виспер, - поэтому под инквизитора заведён свой человек.
	// Gregg: модель и анимация - как у здешнего де Соузы из линии Виспер
	// (whisper_line_functions.c). Модель priest ходит по инквизиции сломанной куклой.
	ref sld = GetCharacter(NPC_GenerateCharacter("BO_Tavora", "PGG_Vincento_0", "man", "man", 10, SPAIN, -1, false));
	sld.name = iTxt("BO_TavoraName");
	sld.lastname = iTxt("BO_TavoraLastName");
	sld.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Tavora.c";
	sld.Dialog.CurrentNode = "First time";
	sld.greeting = "Gr_padre";
	// Gregg: не HuberType - это тип губернатора, он включает сидячую анимацию
	// (LAi_SetHuberSitAnimation), и на goto-локаторе персонаж садится в воздух.
	LAi_SetStayType(sld);
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	// Gregg: armchair1 в инквизиции уже занят - двое садились в одно кресло.
	ChangeCharacterAddressGroup(sld, "Santiago_Incquisitio", "goto", "goto20");
}

void RaulEsperantoInIncquisitio(string qName)
{
	ref sld = GetCharacter(NPC_GenerateCharacter("Raul Esperanto", "usurer_5", "man", "man", 1, PIRATE, -1, false));
	sld.name = iTxt("BO_RaulName");
	sld.lastname = iTxt("BO_RaulLastName");
	sld.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Raul.c";
	sld.Dialog.CurrentNode = "First time";
	sld.greeting = "Gr_slave";
	LAi_SetImmortal(sld, true);
	LAi_SetGroundSitType(sld);
	ChangeCharacterAddressGroup(sld, "Santiago_Incquisitio", "prison", "prison3");
}

void RaulEsperantoFindBeausing()
{
	PChar.RaulEsperanto = "7";
	BO_RERecord("12");

	// Gregg: дом Бозена. Отдельной локации HouseBeausing в CSP нет, поэтому дверь
	// временно ведёт в общий пиратский дом. Прежний адрес запоминаем и восстанавливаем
	// из атрибута, а не из константы: в CSP за этой дверью CommonRoom_MH7, а не то,
	// что было в оригинале.
	int iTown = FindLocation("Marigo_town");
	PChar.RaulEsperanto.DoorBack = Locations[iTown].reload.l21.go;
	PChar.RaulEsperanto.LabelBack = Locations[iTown].reload.l21.label;
	Locations[iTown].reload.l21.go = "CommonPirateHouse";
	// Gregg: не общий «House» - дверь ничем не отличалась от соседних, и дом приходилось
	// искать наугад. Ключ в LocLables.txt обеих локалей.
	Locations[iTown].reload.l21.label = "BO_BeausingHouse";
	Locations[iTown].reload.l21.disable = 1;

	SetQuestFunction("RaulEsperantoInMarigo", "RaulEsperantoInMarigo", "l", "Marigo_town");
}

void RaulEsperantoInMarigo(string qName)
{
	BO_RERecord("13");
}

void RaulEsperantoFindSecretary()
{
	PChar.RaulEsperanto = "8";
	BO_RERecord("14");

	Locations[FindLocation("Marigo_Cave")].locators_radius.monsters.monster11 = 3.0;
	SetQuestFunction("RaulEsperantoInMarigoCave", "RaulEsperantoInMarigoCave", "l", "Marigo_Cave");
}

void RaulEsperantoInMarigoCave(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Beausing Secretary", "officer_2", "man", "man", 1, PIRATE, 0, true));
	ChangeCharacterAddressGroup(chr, "Marigo_Cave", "monsters", "monster11");

	for (int b = 0; b <= 4; b++)
	{
		LaunchBlood(chr, 7.0, true);
	}

	LAi_SetActorType(chr);
	LAi_ActorSetLayMode(chr);
	LAi_SetImmortal(chr, true);

	PChar.quest.RaulEsperantoFindSecretaryCorpse.win_condition.l1 = "locator";
	PChar.quest.RaulEsperantoFindSecretaryCorpse.win_condition.l1.location = "Marigo_Cave";
	PChar.quest.RaulEsperantoFindSecretaryCorpse.win_condition.l1.locator_group = "monsters";
	PChar.quest.RaulEsperantoFindSecretaryCorpse.win_condition.l1.locator = "monster11";
	PChar.quest.RaulEsperantoFindSecretaryCorpse.function = "RaulEsperantoFindSecretaryCorpse";
}

void RaulEsperantoFindSecretaryCorpse(string qName)
{
	chrDisableReloadToLocation = false;
	BO_RERecord("15");

	Locations[FindLocation("Marigo_Cave")].locators_radius.monsters.monster11 = 0.5;
	Locations[FindLocation("Marigo_town")].reload.l21.disable = 0;
	SetQuestFunction("RaulEsperantoToBeausingHouse", "RaulEsperantoToBeausingHouse", "l", "CommonPirateHouse");
}

void RaulEsperantoToBeausingHouse(string qName)
{
	chrDisableReloadToLocation = true;
	LAi_SetActorType(PChar);

	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	ref chr = GetCharacter(NPC_GenerateCharacter("Jan Beausing", "trader_10", "man", "man", iRank, PIRATE, -1, true));
	chr.name = iTxt("BO_BeausingName");
	chr.lastname = iTxt("BO_BeausingLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Beausing.c";
	chr.Dialog.CurrentNode = "First time";
	FantomMakeCoolFighter(chr, iRank, 40, 40, "", "", 10);
	LAi_SetImmortal(chr, true);
	LAi_NPC_Equip(chr, sti(chr.rank), true, true);
	LAi_SetHuberType(chr);
	ChangeCharacterAddressGroup(chr, "CommonPirateHouse", "sit", "sit1");

	string sLocator = "goto3";
	string sModel = "officer_7";
	for (int i = 1; i <= 3; i++)
	{
		sModel = "officer_7";
		if (rand(1) == 1) sModel = "officer_17";
		switch (i)
		{
			case 1: sLocator = "goto3"; break;
			case 2: sLocator = "goto5"; break;
			case 3: sLocator = "goto1"; break;
		}
		chr = GetCharacter(NPC_GenerateCharacter("Jan Beausing Killer " + i, sModel, "man", "man", iRank, PIRATE, 0, true));
		FantomMakeCoolFighter(chr, iRank, 10, 10, "", "", 10);
		LAi_NPC_Equip(chr, sti(chr.rank), true, true);
		ChangeCharacterAddressGroup(chr, "CommonPirateHouse", "goto", sLocator);
		LAi_SetActorType(chr);
		LAi_ActorTurnToCharacter(chr, PChar);
	}

	DoQuestFunctionDelay("RaulEsperantoInBeausingHouse", 0.1);
}

void RaulEsperantoInBeausingHouse(string _tmp)
{
	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, -10, 5, 4, false);
	DoQuestFunctionDelay("RaulEsperantoBattleInBeausingHouse", 5.0);
}

void RaulEsperantoBattleInBeausingHouse(string qName)
{
	for (int i = 1; i <= 3; i++)
	{
		LAi_SetWarriorType(CharacterFromID("Jan Beausing Killer " + i));
		LAi_group_MoveCharacter(CharacterFromID("Jan Beausing Killer " + i), "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("RaulEsperantoBattleInBeausingHouseEnd", "RaulEsperantoBattleInBeausingHouseEnd", "Jan Beausing Killer ", 3);

	LAi_SetPlayerType(PChar);
	DoQuestFunctionDelay("RaulEsperantoBattleInBeausingHouseCam", 5.0);
}

void RaulEsperantoBattleInBeausingHouseCam(string _tmp)
{
	locCameraFollow();
}

void RaulEsperantoBattleInBeausingHouseEnd(string qName)
{
	BO_EndFight();
	ref chr = CharacterFromID("Jan Beausing");
	chr.Dialog.CurrentNode = "1";
	LAi_SetStayType(chr);
	ChangeCharacterAddressGroup(chr, "CommonPirateHouse", "goto", "goto5");
	SetDialogCharacter2Character(PChar, chr, 15.0, true);
}

void RaulEsperantoBattleWithBeausing()
{
	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Jan Beausing");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, "EnemyFight");

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	LAi_SetImmortal(chr, false);
	PChar.quest.RaulEsperanto_2.function = "RaulEsperantoFisrtTalkWithBeausing";
	LAi_SetCheckMinHP(chr, 1, true, "RaulEsperanto_2");

	// Gregg: с каждой отбитой попыткой Бозен слабеет - он не боец, а банкир,
	// и каждый раз выходит на клинок всё неохотнее.
	int iBattleNum = sti(PChar.RaulEsperanto.BeausingBattle);
	float HP = 400.0;
	switch (iBattleNum)
	{
		case 1: HP = 700.0; break;
		case 2: HP = 600.0; break;
		case 3: HP = 500.0; break;
		case 4: HP = 400.0; break;
	}
	LAi_SetHP(chr, HP, HP);
}

void RaulEsperantoFisrtTalkWithBeausing(string qName)
{
	BO_EndFight();
	ref chr = CharacterFromID("Jan Beausing");

	int iBattleNum = sti(PChar.RaulEsperanto.BeausingBattle);
	string sNode = "9";
	switch (iBattleNum)
	{
		case 1: sNode = "5"; break;
		case 2: sNode = "7"; break;
		case 3: sNode = "8"; break;
		case 4: sNode = "9"; break;
	}

	chr.Dialog.CurrentNode = sNode;
	SetDialogCharacter2Character(PChar, chr, 10.0, true);

	LAi_SetHP(chr, 500.0, 500.0);
	LAi_RemoveCheckMinHP(chr);
	LAi_UseAtidoteBottle(chr);
}

void RaulEsperantoToVillemstadBank()
{
	chrDisableReloadToLocation = false;

	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Jan Beausing");
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "", 30.0);

	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);

	PChar.RaulEsperanto = "9";
	BO_RERecord("16");

	SetQuestFunction("RaulEsperantoInVillemstadBank", "RaulEsperantoInVillemstadBank", "l", "Villemstad_bank");
}

void RaulEsperantoInVillemstadBank(string qName)
{
	ref chr = CharacterFromID("Jan Beausing");
	// Gregg: не goto1 - это штатное место вилемстадского ростовщика (quests_reaction.c),
	// вдвоём они вставали друг в друга.
	ChangeCharacterAddressGroup(chr, "Villemstad_bank", "goto", "goto3");
	chr.Dialog.CurrentNode = "14";
	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

void RaulEsperantoGetEvangelie()
{
	PChar.RaulEsperanto = "10";

	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Jan Beausing");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	// Gregg: в оригинале трость лежала в локаторе item2, но в модели InquisitionCave
	// предметный локатор всего один, item1, и он занят ацтекским тотемом. Поэтому
	// трость не разбрасываем по полу, а получаем у Таворы: он её при аресте и отобрал.

	PChar.quest.RaulEsperantoFindStickGood.win_condition.l1 = "item";
	PChar.quest.RaulEsperantoFindStickGood.win_condition.l1.item = "RaulEsperanto_Stick";
	PChar.quest.RaulEsperantoFindStickGood.function = "RaulEsperantoFindStickGood";
}

// Gregg: Рауль попросил трость. Своей функции у этого шага в CSP не было, а диалог
// на неё ссылается - заводим: помечаем состояние и пишем в журнал, куда идти.
void RaulEsperantoFindStick()
{
	BO_RERecord("20");
	// Gregg: метка «Рауль попросил трость». Ветка у Таворы висела на состоянии "11",
	// а оно ставится в момент его согласия - трость выдавалась ещё до встречи со стариком.
	PChar.RaulEsperanto.Stick = "asked";
}

// Gregg: Тавора отдаёт трость. Отдельная функция, чтобы диалог не лез в предметы сам.
void RaulEsperantoGiveStick()
{
	GiveItem2Character(PChar, "RaulEsperanto_Stick");
	// Gregg: метку снимаем, иначе Тавора выдаёт трость при каждом следующем подходе.
	DeleteAttribute(PChar, "RaulEsperanto.Stick");
}

void RaulEsperantoTalkWithEsperanto()
{
	PChar.RaulEsperanto = "11";
	BO_RERecord("21");
	ref chr = CharacterFromID("Raul Esperanto");
	chr.Dialog.CurrentNode = "1";
}

void RaulEsperantoFindStickGood(string qName)
{
	ref chr = CharacterFromID("Raul Esperanto");
	chr.Dialog.CurrentNode = "4";
}

void RaulEsperantoReturnToSanJuan()
{
	PChar.RaulEsperanto = "12";
	BO_RERecord("17");

	ref chr = CharacterFromID("Raul Esperanto");
	ChangeCharacterAddressGroup(chr, "Santiago_Incquisitio", "goto", "goto4");
	LAi_SetActorType(chr);
	LAi_ActorGoToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);

	SetQuestFunction("RaulEsperantoReturnInSanJuan", "RaulEsperantoReturnInSanJuan", "l", "SanJuan_town");
	SetQuestFunction("RaulEsperantoBeausingReturn", "RaulEsperantoBeausingReturn", "l", "Marigo_town");

	chrDisableReloadToLocation = false;
}

void RaulEsperantoReturnInSanJuan(string qName)
{
	ref chr = CharacterFromID("Raul Esperanto");
	RemovePassenger(PChar, chr);
	chr.LifeDay = 0;

	chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart8.c";
	chr.Dialog.CurrentNode = "13";
}

void RaulEsperantoBeausingReturn(string qName)
{
	RemovePassenger(PChar, CharacterFromID("Jan Beausing"));
	SetQuestFunction("RaulEsperantoBeausingClear", "RaulEsperantoBeausingClear", "efl", "Marigo_town");
	AddSimpleRumourToAllNations(iTxt("BO_RE_Rumour1") + GetFullName(PChar) + iTxt("BO_RE_Rumour2"), 30, 5);
}

void RaulEsperantoBeausingClear(string qName)
{
	ref chr;
	if (GetCharacterIndex("Khorkhe de Maranja") != -1)
	{
		chr = &Characters[GetCharacterIndex("Khorkhe de Maranja")];
		chr.LifeDay = 0;
	}
	if (GetCharacterIndex("RaulEsperanto Tortuga Ship") != -1)
	{
		chr = &Characters[GetCharacterIndex("RaulEsperanto Tortuga Ship")];
		chr.LifeDay = 0;
	}
	if (GetCharacterIndex("BO_Tavora") != -1)
	{
		chr = &Characters[GetCharacterIndex("BO_Tavora")];
		chr.LifeDay = 0;
	}
	if (GetCharacterIndex("Jan Beausing") != -1)
	{
		chr = &Characters[GetCharacterIndex("Jan Beausing")];
		chr.LifeDay = 0;
	}

	// Gregg: дверь возвращаем на тот адрес, который у неё был до нас
	int iTown = FindLocation("Marigo_town");
	if (CheckAttribute(PChar, "RaulEsperanto.DoorBack"))
	{
		Locations[iTown].reload.l21.go = PChar.RaulEsperanto.DoorBack;
		Locations[iTown].reload.l21.label = PChar.RaulEsperanto.LabelBack;
	}
	Locations[iTown].reload.l21.disable = 0;
}

void RaulEsperantoEnd()
{
	PChar.RaulEsperanto = "END";
	BO_RERecord("18");
	CloseQuestHeader("RaulEsperanto");
	pchar.questdata.RaulEsperanto = 1;	// LEO: отметка в энциклопедии

	SetTimerFunction("FindAndDestroyTime", 0, 0, BO_RE_NEXT_DAYS);
}
// Gregg: <== глава 8

// Gregg: ==> глава 9. «Найти и уничтожить»
// Рауль объяснил, что часы - половина пары, и вторая половина у капитана, которого
// уже нет в живых. Цепочка ведёт от трупа на берегу Доминики через шахты Каймана
// к штурму Тортуги и заканчивается в море, у чёрного мановара.

void FindAndDestroyTime(string qName)
{
	ref chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart9.c";
	chr.Dialog.CurrentNode = "1";
	ChangeCharacterAddressGroup(chr, "SanJuan_townhall", "goto", "governor1");
	LAi_SetStayType(chr);

	// Gregg: у какого берега шляется Брус Фишер
	string sShore = "Shore6";
	switch (rand(7))
	{
		case 0: sShore = "Shore_ship2"; break;
		case 1: sShore = "Shore6"; break;
		case 2: sShore = "Shore7"; break;
		case 3: sShore = "Shore8"; break;
		case 4: sShore = "Shore9"; break;
		case 5: sShore = "Shore10"; break;
		case 6: sShore = "Shore47"; break;
		case 7: sShore = "Shore54"; break;
	}
	PChar.FindAndDestroy.BrusShore = sShore;
}

void BO_FDRecord(string sRec)
{
	AddQuestRecord("FindAndDestroy", sRec);
	AddQuestUserData("FindAndDestroy", "sSex", GetSexPhrase("ся", "ась"));
	AddQuestUserData("FindAndDestroy", "sSex1", GetSexPhrase("", "а"));
	// Gregg: обе стоянки Фишера. Домашняя выбирается случайно при старте главы, поэтому
	// имена берутся из LocLables на момент записи, а не вписываются в текст руками.
	if (CheckAttribute(PChar, "FindAndDestroy.BrusShore"))
	{
		string sHome = PChar.FindAndDestroy.BrusShore;
		AddQuestUserData("FindAndDestroy", "sShore1", GetConvertStr(sHome, "LocLables.txt"));
		AddQuestUserData("FindAndDestroy", "sShore2", GetConvertStr("Shore55", "LocLables.txt"));
	}
	// Gregg: берег, где ждёт Клови. В тексте стояла «Кайонна» - такого топонима в CSP нет,
	// берём подпись локации.
	AddQuestUserData("FindAndDestroy", "sBeach", GetConvertStr("Shore58", "LocLables.txt"));
}

void FindAndDestroyStart()
{
	PChar.FindAndDestroy = "1";
	SetQuestHeader("FindAndDestroy");
	BO_FDRecord("1");

	ref chr = GetCharacter(NPC_GenerateCharacter("Alen Roshfor", "pirate_7", "man", "man", 10, PIRATE, -1, false));
	chr.name = iTxt("BO_RoshforName");
	chr.lastname = iTxt("BO_RoshforLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Roshfor.c";
	chr.Dialog.CurrentNode = "1";
	chr.DeckDialogNode = "1";
	chr.Ship.Type = GenerateShip(SHIP_BARKENTINE, 0);
	chr.Ship.Name = iTxt("BO_AnnaShipName");
	chr.greeting = "Gr_questOfficer";
	chr.DontRansackCaptain = true;
	chr.AlwaysFriend = true;
	chr.ShipEnemyDisable = true;
	chr.ShipTaskLock = true;
	chr.Abordage.Enable = false;
	LAi_SetImmortal(chr, true);

	SetTimerFunction("FindAndDestroyAnnaToSanJuan", 0, 0, BO_FD_ANNA_DAYS);
}

void FindAndDestroyAnnaToSanJuan(string qName)
{
	Group_FindOrCreateGroup("AlenRoshfor");
	Group_AddCharacter("AlenRoshfor", "Alen Roshfor");
	Group_SetType("AlenRoshfor", "pirate");
	Group_SetGroupCommander("AlenRoshfor", "Alen Roshfor");
	// Gregg: reload_fort1 в PuertoRico_locators.gm нет, есть только осадный
	Group_SetAddress("AlenRoshfor", "PuertoRico", "quest_ships", "reload_fort1_siege");
	Group_SetTaskNone("AlenRoshfor");
	Group_SetNeutralToCharacter("AlenRoshfor", nMainCharacterIndex);
}

void FindAndDestroyToCaiman()
{
	ref chr = CharacterFromID("Alen Roshfor");
	chr.Dialog.CurrentNode = "First time";
	chr.DeckDialogNode = "First time";
	SetQuestFunction("FindAndDestroyClearShipAnna", "FindAndDestroyClearShipAnna", "efl", "PuertoRico");
	SetQuestFunction("FindAndDestroyInCaiman", "FindAndDestroyInCaiman", "l", "Shore27");
	BO_SetEncounters("Shore27", true);

	BO_FDRecord("2");
	Locations[FindLocation("Shore27")].locators_radius.goto.goto14 = 3.0;
}

void FindAndDestroyClearShipAnna(string qName)
{
	Group_DeleteGroup("AlenRoshfor");
	Characters[GetCharacterIndex("Alen Roshfor")].LifeDay = 0;
}

void FindAndDestroyInCaiman(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr = GetCharacter(NPC_GenerateCharacter("Moris Bartelemi", "officer_20", "man", "man", 1, PIRATE, 0, true));
	ChangeCharacterAddressGroup(chr, "Shore27", "goto", "goto14");
	for (int b = 0; b <= 4; b++)
	{
		LaunchBlood(chr, 7.0, true);
	}
	LAi_SetActorType(chr);
	LAi_ActorSetLayMode(chr);
	LAi_SetImmortal(chr, true);

	PChar.quest.FindAndDestroyFindMorisBartelemi.win_condition.l1 = "locator";
	PChar.quest.FindAndDestroyFindMorisBartelemi.win_condition.l1.location = "Shore27";
	PChar.quest.FindAndDestroyFindMorisBartelemi.win_condition.l1.locator_group = "goto";
	PChar.quest.FindAndDestroyFindMorisBartelemi.win_condition.l1.locator = "goto14";
	PChar.quest.FindAndDestroyFindMorisBartelemi.function = "FindAndDestroyFindMorisBartelemi";
}

void FindAndDestroyFindMorisBartelemi(string qName)
{
	chrDisableReloadToLocation = false;

	PChar.FindAndDestroy = "2";
	BO_FDRecord("3");
	Items[FindItem("FindAndDestroy_Pistol")].shown = true;
	Locations[FindLocation("Shore27")].locators_radius.goto.goto14 = 0.5;
	BO_SetEncounters("Shore27", false);
}

void FindAndDestroyFindKharviMurom()
{
	PChar.FindAndDestroy = "3";
	BO_FDRecord("4");

	ref chr = GetCharacter(NPC_GenerateCharacter("Kharvi Murom", "shipowner_15", "man", "man", 1, PIRATE, -1, true));
	chr.name = iTxt("BO_KharviName");
	chr.lastname = iTxt("BO_KharviLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kharvi.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_Tavern_Mate";
	LAi_SetSitType(chr);
	LAi_SetImmortal(chr, true);

	FreeSitLocator("SanJuan_tavern", "sit_base1");
	ChangeCharacterAddressGroup(chr, "SanJuan_tavern", "sit", "sit_base1");
	SetQuestFunction("FindAndDestroyTalkWithKharviMurom", "FindAndDestroyTalkWithKharviMurom", "l", "SanJuan_tavern");
}

void FindAndDestroyTalkWithKharviMurom(string qName)
{
	FreeSitLocator("SanJuan_tavern", "sit_front1");
	ChangeCharacterAddressGroup(PChar, "SanJuan_tavern", "sit", "sit_front1");
	LAi_SetSitType(PChar);
	ref chr = CharacterFromID("Kharvi Murom");
	LAi_SetActorType(chr);
	LAi_ActorSetSitMode(chr);
	LAi_ActorDialogNow(chr, PChar, "", -1);
}

void FindAndDestroyGetKharviInformation()
{
	ChangeCharacterAddressGroup(PChar, "SanJuan_tavern", "goto", "goto4");
	LAi_SetPlayerType(PChar);

	PChar.FindAndDestroy = "4";
	BO_FDRecord("5");
	Characters[GetCharacterIndex("Kharvi Murom")].LifeDay = 0;

	int iRank = sti(PChar.rank) + 15 + MOD_SKILL_ENEMY_RATE;
	ref chr = GetCharacter(NPC_GenerateCharacter("Brus Fisher", "officer_16", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_BrusName");
	chr.lastname = iTxt("BO_BrusLastName");
	chr.DontRansackCaptain = true;
	chr.SaveItemsForDead = true;
	chr.AlwaysEnemy = true;
	chr.CanCrewDamage = true;
	chr.items.FindAndDestroy_key_1_not = 1;
	FantomMakeCoolFighter(chr, iRank, 50, 50, "blade30", "pistol5", 100);
	FantomMakeCoolSailor(chr, SHIP_FRIGATE, iTxt("BO_BrusShipName"), CANNON_TYPE_CULVERINE_LBS24, 50, 50, 50);
	SetHalfPerksToChar(chr, false);
	BO_SetSeaPerks(chr);
	// Gregg: бессмертия нет, как и у Харпича в главе 7. Ключ уходит на дно вместе
	// с Фишером, и это провал линии, а не тупик.
	// Gregg: SetGeraldDetails выброшен - красит общий шаблон класса.

	string sGroup = "BrusFisherGroup";
	Group_FindOrCreateGroup(sGroup);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Group_AddCharacter(sGroup, chr.id);
	Group_SetGroupCommander(sGroup, chr.id);

	chr.mapEnc.type = "trade";
	chr.mapEnc.worldMapShip = "quest_ship";
	chr.mapEnc.Name = iTxt("BO_BrusShipName");
	chr.CapCurrentShore = "Shore55";
	Map_CreateTrader(PChar.FindAndDestroy.BrusShore, "Shore55", chr.id, 25);

	PChar.quest.BrusEncounterArm.win_condition.l1 = "MapEnter";
	PChar.quest.BrusEncounterArm.function = "BrusEncounterArm";

	PChar.quest.FindAndDestroyBrusCapture.win_condition.l1 = "Character_Capture";
	PChar.quest.FindAndDestroyBrusCapture.win_condition.l1.character = "Brus Fisher";
	PChar.quest.FindAndDestroyBrusCapture.function = "FindAndDestroyBrusCapture";

	PChar.quest.FindAndDestroyBrusSink.win_condition.l1 = "Character_Sink";
	PChar.quest.FindAndDestroyBrusSink.win_condition.l1.character = "Brus Fisher";
	PChar.quest.FindAndDestroyBrusSink.function = "FindAndDestroyBrusSink";
}

void BrusEncounterArm(string qName)
{
	SetTimerFunction("BrusEncounterMove", 0, 0, BO_FD_BRUS_DAYS);
	PChar.quest.BrusEncounterMove.again = true;
}

// Gregg: «Искатель» дошёл до стоянки. Зовётся из Map_TraderSucces_quest. Без разворота
// корабль исчезал с карты до следующего срабатывания таймера, то есть до BO_FD_BRUS_DAYS,
// и найти его в названной бухте было нельзя: энкаунтер-торговец на прибытии снимается.
void BrusEncounterArrived()
{
	if (!CheckAttribute(PChar, "FindAndDestroy")) return;
	if (PChar.FindAndDestroy != "4") return;
	if (GetCharacterIndex("Brus Fisher") == -1) return;
	BrusEncounterMove("");
}

void BrusEncounterMove(string qName)
{
	if (GetCharacterIndex("Brus Fisher") == -1)
	{
		PChar.quest.BrusEncounterMove.over = "yes";
		return;
	}

	ref chr = CharacterFromID("Brus Fisher");
	string sHome = PChar.FindAndDestroy.BrusShore;
	string sCurrentShore = sHome;
	if (CheckAttribute(chr, "CapCurrentShore")) sCurrentShore = chr.CapCurrentShore;

	string sTargetShore = sHome;
	if (sCurrentShore == sHome) sTargetShore = "Shore55";

	Map_ReleaseQuestEncounter("Brus Fisher");

	string sGroup = "BrusFisherGroup";
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
	Group_LockTask(sGroup);
	Group_AddCharacter(sGroup, chr.id);
	Group_SetGroupCommander(sGroup, chr.id);

	chr.mapEnc.type = "trade";
	chr.mapEnc.worldMapShip = "quest_ship";
	chr.mapEnc.Name = iTxt("BO_BrusShipName");
	chr.CapCurrentShore = sTargetShore;
	Map_CreateTrader(sCurrentShore, sTargetShore, chr.id, 25);

	SetTimerFunction("BrusEncounterMove", 0, 0, BO_FD_BRUS_DAYS);
}

// Gregg: провал. Ключ был у Фишера, Фишер на дне.
void FindAndDestroyBrusSink(string qName)
{
	PChar.quest.BrusEncounterMove.over = "yes";
	Group_DeleteGroup("BrusFisherGroup");

	PChar.FindAndDestroy = "FAILED";
	BO_FDRecord("17");
	BO_QuestFailed("FindAndDestroy");
}

void FindAndDestroyBrusCapture(string qName)
{
	if (GetCharacterItem(PChar, "FindAndDestroy_key_1_not") <= 0)
	{
		TakeNItems(PChar, "FindAndDestroy_key_1_not", 1);
	}

	PChar.FindAndDestroy = "5";
	BO_FDRecord("6");

	// Gregg: снимаем узел потопления - корабль уже взят
	PChar.quest.FindAndDestroyBrusSink.over = "yes";
	DeleteAttribute(PChar, "quest.FindAndDestroyBrusSink");
	Group_DeleteGroup("BrusFisherGroup");

	ref chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart9.c";
	chr.Dialog.CurrentNode = "18";
}

void FindAndDestroyFindUnlocker()
{
	PChar.FindAndDestroy = "6";
	BO_FDRecord("7");
}

void FindAndDestroyFindMines()
{
	PChar.FindAndDestroy = "7";
	BO_FDRecord("8");

	ref itm = ItemsFromID("FindAndDestroy_key_1_not");
	itm.name = "itmname_FindAndDestroy_key_1_not_2";

	// Gregg: шахт Гваделупы в CSP нет, а кайманские не годятся - они часть колонии
	// игрока и существуют только если он их построил. Берём пещеру Куманы: она есть
	// всегда и не занята ни одним квестом. Группы goto в модели cavernMedium1 нет,
	// поэтому и каторжники, и триггер сидят на monsters.
	Locations[FindLocation("Cumana_Cave")].locators_radius.monsters.monster1 = 2.5;
	SetQuestFunction("FindAndDestroyInMines", "FindAndDestroyInMines", "l", "Cumana_Cave");
}

void FindAndDestroyInMines(string qName)
{
	chrDisableReloadToLocation = true;
	int iM = FindLocation("Cumana_Cave");
	Locations[iM].box1.items.FindAndDestroy_FisherNotice = 1;
	Locations[iM].box1.items.FindAndDestroy_key_1 = 1;
	Locations[iM].box1.items.cirass4 = 1;
	Locations[iM].box1.items.pistol6 = 1;
	Locations[iM].box1.items.backpack4 = 1;
	Locations[iM].box1.items.spyglass5 = 1;
	Locations[iM].box1.items.jewelry2 = 66;
	Locations[iM].box1.items.jewelry3 = 66;
	Locations[iM].box1.items.jewelry4 = 66;
	Locations[iM].box1.money = BO_FD_MINES_MONEY;
	Locations[iM].box1 = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
	// Gregg: сундук вскрывается отмычкой, а не квестовым ключом. В оригинале ключ с Брюса
	// оказывался «не тем», и дальше шла цепочка через Мастера Ключников - но в CSP это
	// сюжетный персонаж «Убить всех нищих», и трогать его незачем.
	Locations[iM].box1.key = "Lockpick";
	Locations[iM].box1.key.delItem = false;

	int iRank = sti(PChar.rank) + 5 + MOD_SKILL_ENEMY_RATE;
	ref chr;
	for (int i = 1; i <= 5; i++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("FindAndDestroy Miner " + i, "prison_" + i, "man", "man", iRank, PIRATE, 0, true));
		chr.name = iTxt("BO_MinerName");
		chr.lastname = "";
		chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Miner.c";
		chr.Dialog.CurrentNode = "1";
		chr.greeting = "Gr_prison";
		LAi_SetImmortal(chr, true);
		LAi_SetStayType(chr);
		LAi_SetHP(chr, 500.0, 500.0);
		GiveItem2Character(chr, "topor3");
		EquipCharacterByItem(chr, "topor3");
		ChangeCharacterAddressGroup(chr, "Cumana_Cave", "monsters", "monster" + i);
	}

	PChar.quest.FindAndDestroyTalkWithMiner.win_condition.l1 = "locator";
	PChar.quest.FindAndDestroyTalkWithMiner.win_condition.l1.location = "Cumana_Cave";
	PChar.quest.FindAndDestroyTalkWithMiner.win_condition.l1.locator_group = "monsters";
	PChar.quest.FindAndDestroyTalkWithMiner.win_condition.l1.locator = "monster1";
	PChar.quest.FindAndDestroyTalkWithMiner.function = "FindAndDestroyTalkWithMiner";
}

void FindAndDestroyTalkWithMiner(string qName)
{
	Locations[FindLocation("Cumana_Cave")].locators_radius.monsters.monster1 = 0.5;
	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, -8, 2, -5, false);
	SetDialogCharacter2Character(PChar, CharacterFromID("FindAndDestroy Miner 1"), 15.0, true);
}

void FindAndDestroyBattleWithMiners()
{
	locCameraFollow();
	LAi_SetPlayerType(PChar);

	ref chr;
	for (int i = 1; i <= 5; i++)
	{
		chr = CharacterFromID("FindAndDestroy Miner " + i);
		LAi_SetImmortal(chr, false);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("FindAndDestroyBattleWithMinersWinner", "FindAndDestroyBattleWithMinersWinner", "FindAndDestroy Miner ", 5);
}

void FindAndDestroyBattleWithMinersWinner(string qName)
{
	BO_EndFight();
	PChar.quest.FindAndDestroyFindFirstKey.win_condition.l1 = "item";
	PChar.quest.FindAndDestroyFindFirstKey.win_condition.l1.item = "FindAndDestroy_key_1";
	PChar.quest.FindAndDestroyFindFirstKey.win_condition.l2 = "item";
	PChar.quest.FindAndDestroyFindFirstKey.win_condition.l2.item = "FindAndDestroy_FisherNotice";
	PChar.quest.FindAndDestroyFindFirstKey.function = "FindAndDestroyFindFirstKey";
}

void FindAndDestroyFindFirstKey(string qName)
{
	chrDisableReloadToLocation = false;
	PChar.FindAndDestroy = "8";
	BO_FDRecord("9");

	// Gregg: сундук вскрыт - ключ от него и записка Фишера, по которой его искали,
	// свою роль отыграли. Иначе они висят в трюме до конца игры.
	TakeNItems(PChar, "FindAndDestroy_key_1_not", -1);
	TakeNItems(PChar, "FindAndDestroy_FisherNotice", -1);
}

void FindAndDestroyWaitTime()
{
	PChar.FindAndDestroy = "9";
	BO_FDRecord("10");
	SetTimerFunction("FindAndDestroyWaitTimeEnd", 0, 0, BO_FD_WAIT_DAYS);
}

void FindAndDestroyWaitTimeEnd(string qName)
{
	PChar.FindAndDestroy = "10";
}
// Gregg: ==> глава 9, часть 2. Тортуга
// Клови Дюбуа ведёт ГГ брать Тортугу изнутри: сперва драка у ворот, потом переодевание
// в нищих, ратуша, комендант, мэр и уход подземельем. Локацию Tortuga_House_Shelter,
// которой в CSP нет, заменяет общий пиратский дом за дверью l25 - прежний адрес двери
// запоминается и возвращается, а не пишется константой, как было в оригинале.

void FindAndDestroyToTortuga()
{
	PChar.FindAndDestroy = "11";
	BO_FDRecord("11");

	ref chr = GetCharacter(NPC_GenerateCharacter("Clovis Dubua", "officer_6", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_ClovisName");
	chr.lastname = iTxt("BO_ClovisLastName");
	chr.NotRemoved = true;
	chr.greeting = "Gr_questOfficer";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Clovis.c";
	chr.Dialog.CurrentNode = "First time";
	FantomMakeCoolFighter(chr, 50, 50, 50, "blade17", "pistol2", 100);
	LAi_SetImmortal(chr, true);
	LAi_SetStayType(chr);

	ChangeCharacterAddressGroup(chr, "Shore58", "goto", "goto9");
	BO_SetEncounters("Shore58", true);
}

void FindAndDestroyToTortugaExitTown()
{
	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	PChar.FindAndDestroy = "12";
	// Gregg: запись про людей у ворот переехала в FindAndDestroyFindClothes. Здесь игрок
	// ещё никого не считал: Клови докладывает только у самых ворот.

	ref loc = LocFromID("Shore58");
	loc.reload.l2.disable = true;
	loc.reload.l3.disable = true;

	BO_SetEncounters("Tortuga_ExitTown", true);
	SetQuestFunction("FindAndDestroyInTortugaExitTown", "FindAndDestroyInTortugaExitTown", "l", "Tortuga_ExitTown");
}

void FindAndDestroyInTortugaExitTown(string qName)
{
	chrDisableReloadToLocation = true;

	SetCurrentTime(1, 5);
	RecalculateJumpTable();
	RefreshWeather();
	RefreshLandTime();

	ref chr;
	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	for (int i = 1; i <= 12; i++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("FindAndDestroy ExitTown Defender " + i, "sold_fra_" + (rand(7) + 1), "man", "man", iRank, PIRATE, 0, true));
		ChangeCharacterAddressGroup(chr, "Tortuga_ExitTown", "reload", "reload4");
		LAi_SetCitizenType(chr);
		LAi_NPC_Equip(chr, sti(chr.rank), true, true);
	}

	chr = CharacterFromID("Clovis Dubua");
	chr.Dialog.CurrentNode = "9";
	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

void FindAndDestroyInTortugaExitTownBattle()
{
	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	for (int i = 1; i <= 12; i++)
	{
		chr = CharacterFromID("FindAndDestroy ExitTown Defender " + i);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("FindAndDestroyExitTownWinner", "FindAndDestroyExitTownWinner", "FindAndDestroy ExitTown Defender ", 12);
}

void FindAndDestroyExitTownWinner(string qName)
{
	BO_EndFight();
	ref chr = CharacterFromID("Clovis Dubua");
	chr.Dialog.CurrentNode = "12";
	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

void FindAndDestroyInTortugaTown()
{
	chrDisableReloadToLocation = false;

	ref loc = LocFromID("Tortuga_town");
	loc.DeleteCharacters = true;
	loc = LocFromID("Tortuga_townhall");
	loc.DeleteCharacters = true;
	loc = LocFromID("Tortuga_townhallRoom");
	loc.DeleteCharacters = true;

	Locations[FindLocation("Tortuga_ExitTown")].reload.l1.disable = true;
	Locations[FindLocation("Tortuga_ExitTown")].reload.l2.disable = true;
	Locations[FindLocation("Tortuga_ExitTown")].reload.l3.disable = true;

	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	SetQuestFunction("FindAndDestroyToTortugaTown", "FindAndDestroyToTortugaTown", "l", "Tortuga_town");
}

void FindAndDestroyToTortugaTown(string qName)
{
	chrDisableReloadToLocation = true;
	// Gregg: OpenCloseLocationReload в CSP нет - те же двери правим напрямую
	Locations[FindLocation("Tortuga_town")].reload.l29.disable = true;
	Locations[FindLocation("Tortuga_town")].reload.l91.disable = true;
	Locations[FindLocation("Tortuga_town")].reload.gate.disable = true;

	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	for (int i = 1; i <= 7; i++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("FindAndDestroy Town Defender " + i, "sold_fra_" + (rand(7) + 1), "man", "man", iRank, PIRATE, 0, true));
		chr.NotRemoved = true;
		ChangeCharacterAddressGroup(chr, "Tortuga_town", "reload", "reload1_back");
		LAi_NPC_Equip(chr, sti(chr.rank), true, true);
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "EnemyFight");
	}

	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	BO_SetNPCDeadFunction("FindAndDestroyTownWinner", "FindAndDestroyTownWinner", "FindAndDestroy Town Defender ", 7);
}

void FindAndDestroyTownWinner(string qName)
{
	BO_EndFight();
	ref chr = CharacterFromID("Clovis Dubua");
	chr.Dialog.CurrentNode = "16";
	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

void FindAndDestroyToTortugaShelter()
{
	chrDisableReloadToLocation = false;

	LAi_SetPlayerType(PChar);
	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "houseH2", "none", "", "", "", 30.0);

	int iTown = FindLocation("Tortuga_town");
	PChar.FindAndDestroy.DoorBack = Locations[iTown].reload.l25.go;
	Locations[iTown].reload.l25.go = "CommonPirateHouse";
	Locations[iTown].reload.l25.disable = false;

	SetQuestFunction("FindAndDestroyInTortugaShelter", "FindAndDestroyInTortugaShelter", "l", "CommonPirateHouse");
}

void FindAndDestroyInTortugaShelter(string qName)
{
	int iTown = FindLocation("Tortuga_town");
	Locations[iTown].reload.l25.disable = 1;
	if (CheckAttribute(PChar, "FindAndDestroy.DoorBack"))
	{
		Locations[iTown].reload.l25.go = PChar.FindAndDestroy.DoorBack;
	}

	chrDisableReloadToLocation = true;
	ref chr = CharacterFromID("Clovis Dubua");
	ChangeCharacterAddressGroup(chr, "CommonPirateHouse", "goto", "goto3");
	chr.Dialog.CurrentNode = "18";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void FindAndDestroyFindClothes()
{
	LAi_SetPlayerType(PChar);
	BO_FDRecord("12");
	// Gregg: Клови оставался актёром, пока игрок роется в сундуке, и заговорить с ним
	// было нельзя. Если из сундука унесли один комплект вместо двух, условие квеста
	// не срабатывает - без живого Клови из этого тупика не выйти.
	LAi_SetStayType(CharacterFromID("Clovis Dubua"));
	PChar.FindAndDestroy.NeedSuits = true;
	Locations[FindLocation("CommonPirateHouse")].box1.items.suit_1 = 2;

	PChar.quest.FindAndDestroyFindClothesGood.win_condition.l1 = "item_qty";
	PChar.quest.FindAndDestroyFindClothesGood.win_condition.l1.item = "suit_1";
	PChar.quest.FindAndDestroyFindClothesGood.win_condition.l1.item.quantity = 2;
	PChar.quest.FindAndDestroyFindClothesGood.function = "FindAndDestroyFindClothesGood";
}

void FindAndDestroyFindClothesGood(string qName)
{
	ref chr = CharacterFromID("Clovis Dubua");
	chr.Dialog.CurrentNode = "20";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void FindAndDestroyDialogInShelter()
{
	DeleteAttribute(PChar, "FindAndDestroy.NeedSuits");
	LAi_Fade("", "");

	SetIntegratedMusketDrawn(pchar, false);

	string sModel = PChar.model;
	string sAnimation = PChar.model.animation;

	PChar.FindAndDestroy.DisableMusket = true;
	PChar.FindAndDestroy.PreSuit.Model = sModel;
	PChar.FindAndDestroy.PreSuit.Animation = sAnimation;

	// Gregg: без Cirgnore кирасная логика на входе в следующую локацию пересобирает модель
	// из HeroModel, и тряпьё слетает прямо за дверью. Атрибут снимаем при переодевании назад.
	PChar.Cirgnore = true;
	PChar.model = "panhandler_6";
	PChar.model.animation = "man";
	TakeNItems(PChar, "suit_1", -2);
	SetNewModelToChar(PChar);

	ref chr = CharacterFromID("Clovis Dubua");
	chr.model = "panhandler_4";
	chr.model.animation = "man";
	SetNewModelToChar(chr);

	DoQuestFunctionDelay("FindAndDestroyShelterOfficer", 2.5);
}

void FindAndDestroyShelterOfficer(string _tmp)
{
	ref chr = GetCharacter(NPC_GenerateCharacter("Tortuga Shelter Officer", "off_fra_2", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_ComendantName");
	chr.lastname = "";
	chr.greeting = "soldier_arest";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Comendant.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetImmortal(chr, true);

	ChangeCharacterAddressGroup(chr, "CommonPirateHouse", "reload", "reload1");
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void FindAndDestroyDialogInShelterEnd()
{
	ref chr = CharacterFromID("Tortuga Shelter Officer");
	LAi_SetActorType(chr);
	LAi_ActorGoToLocation(chr, "reload", "reload1", "none", "", "", "", 30.0);

	LAi_Fade("", "");
	DoQuestFunctionDelay("FindAndDestroyShelterAfter", 5.0);
}

void FindAndDestroyShelterAfter(string _tmp)
{
	ref chr = CharacterFromID("Clovis Dubua");
	chr.Dialog.CurrentNode = "21";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void FindAndDestroyToTownhall()
{
	chrDisableReloadToLocation = false;
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	Locations[FindLocation("Tortuga_town")].locators_radius.officers.reload3_1 = 4.0;
	SetQuestFunction("FindAndDestroyGoToTownhall", "FindAndDestroyGoToTownhall", "l", "Tortuga_town");
}

void FindAndDestroyGoToTownhall(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr;
	for (int i = 1; i <= 2; i++)
	{
		chr = GetCharacter(NPC_GenerateCharacter("FindAndDestroy Officer Near Townhall " + i, "sold_fra_" + (rand(7) + 1), "man", "man", 1, PIRATE, 0, false));
		chr.name = iTxt("BO_PatrolName");
		chr.lastname = "";
		chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Comendant.c";
		chr.Dialog.CurrentNode = "4";
		chr.NotRemoved = true;
		chr.greeting = "soldier_arest";
		LAi_SetImmortal(chr, true);
		LAi_SetActorType(chr);
		LAi_ActorTurnToLocator(chr, "officers", "reload3_1");
		ChangeCharacterAddressGroup(chr, "Tortuga_town", "soldiers", "soldier" + i);
	}

	PChar.quest.FindAndDestroyDialogNearTownhall.win_condition.l1 = "locator";
	PChar.quest.FindAndDestroyDialogNearTownhall.win_condition.l1.location = "Tortuga_town";
	PChar.quest.FindAndDestroyDialogNearTownhall.win_condition.l1.locator_group = "officers";
	PChar.quest.FindAndDestroyDialogNearTownhall.win_condition.l1.locator = "reload3_1";
	PChar.quest.FindAndDestroyDialogNearTownhall.function = "FindAndDestroyDialogNearTownhall";
}

void FindAndDestroyDialogNearTownhall(string qName)
{
	Locations[FindLocation("Tortuga_town")].locators_radius.officers.reload3_1 = 0.5;
	SetDialogCharacter2Character(PChar, CharacterFromID("FindAndDestroy Officer Near Townhall 1"), 1.0, true);
}

void FindAndDestroyReloadToTownhall()
{
	chrDisableReloadToLocation = false;
	LAi_SetPlayerType(PChar);

	ref loc = LocFromID("Tortuga_town");
	loc.reload.l3.disable = 0;
	loc.reload.l3.close_for_night = 0;

	loc = LocFromID("Tortuga_townhall");
	loc.type = "arena";
	SetQuestFunction("FindAndDestroyReloadInTownhall", "FindAndDestroyReloadInTownhall", "l", "Tortuga_townhall");
}

void FindAndDestroyReloadInTownhall(string qName)
{
	ref loc = LocFromID("Tortuga_town");
	loc.reload.l1.disable = 1;

	int iRank = sti(PChar.rank) + 10 + MOD_SKILL_ENEMY_RATE;
	ref chr = CharacterFromID("Tortuga Shelter Officer");
	chr.NotRemoved = true;
	chr.Dialog.CurrentNode = "7";
	LAi_SetActorType(chr);
	LAi_ActorTurnToLocator(chr, "goto", "goto5");
	LAi_SetStayType(chr);
	FantomMakeCoolFighter(chr, iRank, 50, 50, "blade13", "pistol5", 100);
	ChangeCharacterAddressGroup(chr, "Tortuga_townhall", "goto", "goto5");

	// Gregg: мэр-двойник берёт внешность и имя настоящего мэра Тортуги
	int iMayor = GetCharacterIndex("Tortuga_Mayor");
	string sModel = Characters[iMayor].model;
	string sName = Characters[iMayor].name;
	string sLastName = Characters[iMayor].lastname;

	iRank = sti(PChar.rank) + 15 + MOD_SKILL_ENEMY_RATE;
	chr = GetCharacter(NPC_GenerateCharacter("FindAndDestroy Tortuga Mayor", sModel, "man", "man", 25, PIRATE, -1, false));
	chr.name = sName;
	chr.lastname = sLastName;
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Comendant.c";
	chr.Dialog.CurrentNode = "8";
	chr.greeting = "soldier_arest";
	chr.NotRemoved = true;
	LAi_SetActorType(chr);
	LAi_SetImmortal(chr, true);
	FantomMakeCoolFighter(chr, iRank, 50, 50, "blade14", "pistol6", 100);
	ChangeCharacterAddressGroup(chr, "Tortuga_townhall", "goto", "governor1");

	// Gregg: настоящего мэра надо убрать, иначе в резиденции стоят двое Рене Паульо -
	// двойник с его моделью и именем и он сам. Возвращаем в FindAndDestroyExitClearAll.
	ref rMayor = &Characters[iMayor];
	PChar.FindAndDestroy.MayorBack = "Tortuga_townhall";
	PChar.FindAndDestroy.MayorBack.group = "goto";
	PChar.FindAndDestroy.MayorBack.locator = "governor1";
	// Gregg: адрес читаем по одному полю с проверкой - у мэра в момент подмены может
	// не оказаться какого-то из них, а промах вернёт его в никуда.
	if (CheckAttribute(rMayor, "location")) PChar.FindAndDestroy.MayorBack = rMayor.location;
	if (CheckAttribute(rMayor, "location.group")) PChar.FindAndDestroy.MayorBack.group = rMayor.location.group;
	if (CheckAttribute(rMayor, "location.locator")) PChar.FindAndDestroy.MayorBack.locator = rMayor.location.locator;
	ChangeCharacterAddressGroup(rMayor, "none", "", "");
}

void FindAndDestroyFirstBattleInTownhall()
{
	chrDisableReloadToLocation = true;
	LAi_LocationFightDisable(&Locations[FindLocation(PChar.location)], false);

	ref mayor = CharacterFromID("FindAndDestroy Tortuga Mayor");
	LAi_SetActorType(mayor);
	LAi_ActorAfraid(mayor, PChar, false);

	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	chr = CharacterFromID("Tortuga Shelter Officer");
	LAi_SetHP(chr, 600.0, 600.0);
	LAi_SetImmortal(chr, false);

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.FindAndDestroyTownhallWinner.win_condition.l1 = "NPC_Death";
	PChar.quest.FindAndDestroyTownhallWinner.win_condition.l1.character = "Tortuga Shelter Officer";
	PChar.quest.FindAndDestroyTownhallWinner.function = "FindAndDestroyTownhallWinner";
}

void FindAndDestroyTownhallWinner(string qName)
{
	BO_EndFight();
	LAi_LocationFightDisable(&Locations[FindLocation(PChar.location)], true);

	ref chr = CharacterFromID("Clovis Dubua");
	chr.Dialog.CurrentNode = "28";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void FindAndDestroyTalkWithMayor()
{
	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1", "none", "", "", "", 30.0);

	SetDialogCharacter2Character(PChar, CharacterFromID("FindAndDestroy Tortuga Mayor"), 15.0, false);
}

void FindAndDestroyBattleWithGovernor()
{
	LAi_LocationFightDisable(&Locations[FindLocation(PChar.location)], false);
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("FindAndDestroy Tortuga Mayor");
	LAi_SetHP(chr, 500.0, 500.0);
	LAi_SetImmortal(chr, false);

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.FindAndDestroy_1.function = "FindAndDestroyBattleWithGovernorWinner";
	LAi_SetCheckMinHP(chr, 1, true, "FindAndDestroy_1");
}

void FindAndDestroyBattleWithGovernorWinner(string qName)
{
	BO_EndFight();
	LAi_LocationFightDisable(&Locations[FindLocation(PChar.location)], true);

	ref chr = CharacterFromID("FindAndDestroy Tortuga Mayor");
	chr.Dialog.CurrentNode = "13";
	SetDialogCharacter2Character(PChar, chr, 1.0, false);
	LAi_SetHP(chr, 100.0, 100.0);
	LAi_RemoveCheckMinHP(chr);
	LAi_UseAtidoteBottle(chr);
}

void FindAndDestroyToTownhallRoom()
{
	LAi_SetPlayerType(PChar);
	chrDisableReloadToLocation = false;

	Locations[FindLocation("Tortuga_townhall")].reload.l1.disable = 1;
	Locations[FindLocation("Tortuga_townhall")].reload.l2.disable = 1;
	Locations[FindLocation("Tortuga_townhall")].reload.l3.disable = 0;

	SetQuestFunction("FindAndDestroyInTownhallRoom", "FindAndDestroyInTownhallRoom", "l", "Tortuga_townhallRoom");
}

void FindAndDestroyInTownhallRoom(string qName)
{
	chrDisableReloadToLocation = true;
	Characters[GetCharacterIndex("FindAndDestroy Tortuga Mayor")].LifeDay = 0;

	string sLastName = Characters[GetCharacterIndex("Tortuga_Mayor")].lastname;
	ref chr = GetCharacter(NPC_GenerateCharacter("FindAndDestroy Mayor Daughter", "lady01", "woman", "towngirl", 1, PIRATE, -1, false));
	chr.name = iTxt("BO_DaughterName");
	chr.lastname = sLastName;
	chr.NotRemoved = true;
	chr.greeting = "Gr_Woman_Citizen";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Comendant.c";
	chr.Dialog.CurrentNode = "15";
	ChangeCharacterAddressGroup(chr, "Tortuga_townhallRoom", "goto", "goto5");
	LAi_SetImmortal(chr, true);

	SetDialogCharacter2Character(PChar, chr, 30.0, false);

	// Gregg: ключ тот же самый, золотой ключ Андерсена, - меняем только название.
	// Иконку не трогаем: ITEMS_14/16 это гвозди, а не ключ.
	ref itm = ItemsFromID("FindAndDestroy_key_1");
	itm.name = "itmname_FindAndDestroy_key_2";
}

void FindAndDestroyInTownhallRoomDialog()
{
	ref chr = CharacterFromID("Clovis Dubua");
	chr.Dialog.CurrentNode = "29";
	ChangeCharacterAddressGroup(chr, "Tortuga_townhallRoom", "reload", "reload1");
	SetDialogCharacter2Character(PChar, chr, 25.0, false);
}

void FindAndDestroyRoomOfficers()
{
	LAi_Fade("", "");

	ref chr;
	string sModel = "off_fra_1";
	string sLocator = "reload1";
	string sGroup = "reload";
	for (int i = 1; i <= 3; i++)
	{
		switch (i)
		{
			case 1:
				sModel = "off_fra_" + (rand(1) + 1);
				sLocator = "reload1";
				sGroup = "reload";
			break;
			case 2:
				sModel = "sold_fra_" + (rand(7) + 1);
				sLocator = "reload1";
				sGroup = "reload";
			break;
			case 3:
				sModel = "sold_fra_" + (rand(7) + 1);
				sLocator = "bar2";
				sGroup = "barmen";
			break;
		}

		chr = GetCharacter(NPC_GenerateCharacter("FindAndDestroy Room Officer " + i, sModel, "man", "man", 1, PIRATE, -1, true));
		chr.name = iTxt("BO_PatrolName");
		chr.lastname = "";
		chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Comendant.c";
		chr.Dialog.CurrentNode = "17";
		chr.greeting = "soldier_arest";
		LAi_SetActorType(chr);
		LAi_SetImmortal(chr, true);
		ChangeCharacterAddressGroup(chr, "Tortuga_townhallRoom", sGroup, sLocator);

		if (i == 1) SetDialogCharacter2Character(PChar, chr, 20.0, false);
	}
}

void FindAndDestroyRoomOfficersOut()
{
	ref chr;
	for (int i = 1; i <= 3; i++)
	{
		chr = CharacterFromID("FindAndDestroy Room Officer " + i);
		LAi_SetActorType(chr);
		LAi_ActorGoToLocation(chr, "reload", "reload1", "none", "", "", "", 30.0);
	}

	LAi_Fade("", "");
	DoQuestFunctionDelay("FindAndDestroyRoomAfter", 10.0);
}

void FindAndDestroyRoomAfter(string _tmp)
{
	ref chr = CharacterFromID("Clovis Dubua");
	chr.Dialog.CurrentNode = "30";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void FindAndDestroyToDungeon()
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Clovis Dubua");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	chr = CharacterFromID("FindAndDestroy Mayor Daughter");
	LAi_SetActorType(chr);
	LAi_ActorFollowEverywhere(chr, "", -1);

	PChar.quest.FindAndDestroy_2.function = "FindAndDestroyGoToDungeon";
	LAi_Fade("", "FindAndDestroy_2");
}

void FindAndDestroyGoToDungeon(string qName)
{
	LAi_SetPlayerType(PChar);
	// Gregg: выход из резиденции на улицу (reload.l1 -> Tortuga_town) закрывался при подъёме
	// в кабинет и открывался только в FindAndDestroyExitClearAll, то есть уже в Сан-Хуане.
	// Игрок оказывался заперт в ратуше: с девушкой поговорил, с Клови поговорил, а выйти
	// к ходу в пещеры не может. Открываем здесь, когда пора уходить.
	Locations[FindLocation("Tortuga_townhall")].reload.l1.disable = 0;
	Locations[FindLocation("Tortuga_townhall")].reload.l2.disable = 0;
	Locations[FindLocation("Tortuga_town")].reload.l29.disable = true;
	Locations[FindLocation("Tortuga_town")].reload.l13.disable = 0;

	SetQuestFunction("FindAndDestroyInDungeon", "FindAndDestroyInDungeon", "l", "Tortuga_Cave");
}

void FindAndDestroyInDungeon(string qName)
{
	Locations[FindLocation("Tortuga_Cave")].reload.l1.disable = 0;
	Locations[FindLocation("Tortuga_Cave")].reload.l2.disable = 1;

	PChar.FindAndDestroy = "12";
	SetQuestFunction("FindAndDestroyToShoreFromDungeon", "FindAndDestroyToShoreFromDungeon", "l", "Shore58");
}

void FindAndDestroyToShoreFromDungeon(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr;
	string sLocator = "goto9";
	for (int i = 1; i <= 3; i++)
	{
		switch (i)
		{
			case 1: sLocator = "goto9"; break;
			case 2: sLocator = "goto10"; break;
			case 3: sLocator = "goto4"; break;
		}
		chr = CharacterFromID("FindAndDestroy Room Officer " + i);
		chr.Dialog.CurrentNode = "21";
		LAi_SetStayType(chr);
		ChangeCharacterAddressGroup(chr, "Shore58", "goto", sLocator);
	}

	ChangeCharacterAddressGroup(CharacterFromID("FindAndDestroy Mayor Daughter"), "Shore58", "reload", "reload2_back");
	ChangeCharacterAddressGroup(CharacterFromID("Clovis Dubua"), "Shore58", "goto", "goto15");

	PChar.quest.FindAndDestroyShoreMeet.win_condition.l1 = "locator";
	PChar.quest.FindAndDestroyShoreMeet.win_condition.l1.location = "Shore58";
	PChar.quest.FindAndDestroyShoreMeet.win_condition.l1.locator_group = "encdetector";
	PChar.quest.FindAndDestroyShoreMeet.win_condition.l1.locator = "enc02";
	PChar.quest.FindAndDestroyShoreMeet.function = "FindAndDestroyShoreMeet";
}

void FindAndDestroyShoreMeet(string qName)
{
	SetCharacterShipLocation(PChar, "Shore58");
	SetDialogCharacter2Character(PChar, CharacterFromID("FindAndDestroy Room Officer 1"), 20.0, false);
}

void FindAndDestroyExitTortuga()
{
	PChar.FindAndDestroy = "13";
	BO_FDRecord("13");

	chrDisableReloadToLocation = false;

	ref chr;
	for (int i = 1; i <= 3; i++)
	{
		chr = CharacterFromID("FindAndDestroy Room Officer " + i);
		chr.LifeDay = 0;
		LAi_SetActorType(chr);
		LAi_ActorGoToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);
	}

	chr = CharacterFromID("FindAndDestroy Mayor Daughter");
	chr.LifeDay = 0;
	LAi_SetActorType(chr);
	LAi_ActorGoToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	chr = CharacterFromID("Clovis Dubua");
	LAi_SetActorType(chr);
	LAi_ActorGoToLocation(chr, "reload", "sea", "none", "", "", "", 30.0);

	SetCharacterRemovable(chr, false);
	AddPassenger(PChar, chr, 0);

	LAi_SetPlayerType(PChar);
	Locations[FindLocation("Shore58")].reload.l1.disable = 1;
	Locations[FindLocation("Shore58")].reload.l2.disable = 1;
	Locations[FindLocation("Shore58")].reload.l3.disable = 0;
	Island_SetReloadEnableGlobal("Tortuga", false);

	SetQuestFunction("FindAndDestroyReturnModel", "FindAndDestroyReturnModel", "efl", "Shore58");
	SetQuestFunction("FindAndDestroyExitClearAll", "FindAndDestroyExitClearAll", "l", "SanJuan_town");
}

void FindAndDestroyReturnModel(string qName)
{
	ref loc = LocFromID("Tortuga_town");
	DeleteAttribute(loc, "DeleteCharacters");
	loc = LocFromID("Tortuga_townhall");
	loc.type = "residence";
	DeleteAttribute(loc, "DeleteCharacters");
	loc = LocFromID("Tortuga_townhallRoom");
	DeleteAttribute(loc, "DeleteCharacters");

	DeleteAttribute(PChar, "FindAndDestroy.DisableMusket");
	DeleteAttribute(PChar, "Cirgnore");
	if (CheckAttribute(PChar, "FindAndDestroy.PreSuit.Model") && CheckAttribute(PChar, "FindAndDestroy.PreSuit.Animation"))
	{
		string model = CanonicalMusketModel(PChar.FindAndDestroy.PreSuit.Model);
		string ani = PChar.FindAndDestroy.PreSuit.Animation;
		if (HasSubStr(ani, "mushketer"))
		{
			ani = MainChAnim;
			if (IsCharacterPerkOn(PChar, "AgileMan")) ani = ani + "_fast";
		}
		PChar.model = model;
		PChar.model.animation = ani;
		SetNewModelToChar(PChar);
		TakeNItems(PChar, "suit_1", 1);
	}
	DeleteAttribute(PChar, "FindAndDestroy.PreSuit");

	BO_SetEncounters("Tortuga_ExitTown", false);
	BO_SetEncounters("Shore58", false);
}

void FindAndDestroyExitClearAll(string qName)
{
	Island_SetReloadEnableGlobal("Tortuga", true);
	Locations[FindLocation("Shore58")].reload.l1.disable = 0;
	Locations[FindLocation("Shore58")].reload.l2.disable = 0;
	Locations[FindLocation("Tortuga_Cave")].reload.l2.disable = 0;
	Locations[FindLocation("Tortuga_townhall")].reload.l1.disable = 0;
	Locations[FindLocation("Tortuga_townhall")].reload.l2.disable = 0;
	Locations[FindLocation("Tortuga_ExitTown")].reload.l1.disable = 0;
	Locations[FindLocation("Tortuga_ExitTown")].reload.l2.disable = 0;
	Locations[FindLocation("Tortuga_ExitTown")].reload.l3.disable = 0;
	Locations[FindLocation("Tortuga_town")].reload.l3.close_for_night = 1;
	Locations[FindLocation("Tortuga_town")].reload.l29.disable = false;
	Locations[FindLocation("Tortuga_town")].reload.l91.disable = false;
	Locations[FindLocation("Tortuga_town")].reload.gate.disable = false;
	RemovePassenger(PChar, CharacterFromID("Clovis Dubua"));
	Characters[GetCharacterIndex("Clovis Dubua")].LifeDay = 0;

	// Gregg: возвращаем настоящего мэра на его место. Адрес запомнен при подмене.
	if (CheckAttribute(PChar, "FindAndDestroy.MayorBack"))
	{
		ref rMayor = CharacterFromID("Tortuga_Mayor");
		string sLoc = PChar.FindAndDestroy.MayorBack;
		string sGrp = PChar.FindAndDestroy.MayorBack.group;
		string sLct = PChar.FindAndDestroy.MayorBack.locator;
		ChangeCharacterAddressGroup(rMayor, sLoc, sGrp, sLct);
		DeleteAttribute(PChar, "FindAndDestroy.MayorBack");
	}
}

void FindAndDestroyOpenClock()
{
	// Gregg: ключи отданы Кендарту и вставлены в часы - в кармане им больше не место.
	TakeNItems(PChar, "FindAndDestroy_key_1", -1);
	ref chr = GetCharacter(NPC_GenerateCharacter("FindAndDestroyOpenClock", "citiz_6", "man", "man", 1, PIRATE, 0, false));
	chr.name = iTxt("BO_MessengerName");
	chr.lastname = "";
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_CursedCaptain.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetImmortal(chr, true);

	ChangeCharacterAddressGroup(chr, "SanJuan_townhall", "reload", "reload1_back");
	SetDialogCharacter2Character(PChar, chr, 20.0, false);

	chr = CharacterFromID("Alehon Kendart");
	LAi_SetActorType(chr);
}

void FindAndDestroyTalkWithKendart()
{
	ref chr = CharacterFromID("FindAndDestroyOpenClock");
	LAi_SetActorType(chr);
	LAi_ActorRunToLocation(chr, "reload", "reload1_back", "none", "", "", "", 30.0);

	chr = CharacterFromID("Alehon Kendart");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_Kendart9.c";
	chr.Dialog.CurrentNode = "42";
	SetDialogCharacter2Character(PChar, chr, 10.0, false);
}

void FindAndDestroyToPuertoRico()
{
	LAi_SetPlayerType(PChar);
	ref kend = CharacterFromID("Alehon Kendart");
	LAi_SetStayType(kend);
	LAi_group_MoveCharacter(kend, LAI_GROUP_PLAYER);
	PChar.FindAndDestroy = "14";
	BO_FDRecord("14");

	SetNextWeather("Storm04_add");

	int iRank = sti(PChar.rank) + 15 + MOD_SKILL_ENEMY_RATE;
	// Gregg: в К:ПС тут "skeletcap". В CSP от этого имени FaceMaker строит варианты
	// skeletcap_1..5, которых нет (есть только skeletcap_1_1..1_5), - берём полное семейство.
	ref sld = GetCharacter(NPC_GenerateCharacter("CursedBlackManowar", "PGG_Skeletcap_0", "skeleton", "man", iRank, PIRATE, -1, true));
	SetCaptanModelByEncType(sld, "pirate");
	// Gregg: SHIP_MANOWAR_HEAVY в CSP нет - берём обычный мановар
	// Gregg: топовая генерация. Coolest -> GenerateShipTop: ставит QuestShip, не гоняет
	// статы через SetShipsBonuses и не срезает стволы на rand(MaxCannonDiff). Обычный
	// FantomMakeCoolSailor выдавал проклятому мановару неполную комплектацию.
	FantomMakeCoolestSailor(sld, SHIP_MANOWAR, iTxt("BO_BlackManowarName"), CANNON_TYPE_CULVERINE_LBS24, 100, 100, 100);
	sld.AlwaysSandbankManeuver = true;
	sld.AnalizeShips = true;
	sld.DontRansackCaptain = true;
	sld.AlwaysEnemy = true;
	sld.CanCrewDamage = true;
	// Gregg: не CrewSkelMode - в CSP это рыболюди с «Летучего Голландца» (GetRandSkelModel
	// -> PotCMob1..3, characters_face.c:570). Проклятому мановару нужны скелеты, их даёт
	// CrewSkelModeClassic (GetRandSkelModelClassic -> Skel1..3).
	sld.GenQuest.CrewSkelModeClassic = true;
	// Gregg: в К:ПС тут стояло LAi_SetImmortal(sld, true). В CSP это делает НЕУЯЗВИМЫМ
	// и корабль: LAi_IsImmortal проверяется в AIShip.c (3655/3760/3800) и выходит из
	// расчёта урона до его применения. Мановар просто не топился. Та же грабля, что была
	// с Ферегуэйро и Тюльпаном; бессмертие тут ничему не нужно - в каюте разговор ведёт
	// клон, а Полл Стронгфилд бессмертен сам по себе.

	// Gregg: SHIP_MANOWAR_HEAVY в CSP нет, поэтому вес добираем уникальными статами
	// на слоте этого экземпляра в RealShips - шаблон класса не трогаем. Корабль
	// проклятый: он не должен ходить как обычный линкор. Статы черновые.
	ref rBlack = GetRealShip(sti(sld.Ship.Type));
	rBlack.HP = 22000.0;
	rBlack.SpeedRate = 12.5;
	rBlack.TurnRate = 7.5;
	rBlack.WindAgainstSpeed = 4.0;
	rBlack.Capacity = 7000;
	// LEO: повторный SetBaseShipData затирает боекомплект, выданный FantomMakeCoolestSailor.
	sld.Ship.HP = rBlack.HP;
	SetHalfPerksToChar(sld, false);
	BO_SetSeaPerks(sld);
	LAi_group_MoveCharacter(sld, "EnemyFight");

	Group_FindOrCreateGroup("CursedBlackManowar");
	Group_AddCharacter("CursedBlackManowar", "CursedBlackManowar");
	Group_SetType("CursedBlackManowar", "pirate");
	Group_SetGroupCommander("CursedBlackManowar", "CursedBlackManowar");
	Group_SetAddress("CursedBlackManowar", "PuertoRico", "quest_ships", "reload_fort1_siege");
	Group_SetTaskAttack("CursedBlackManowar", PLAYER_GROUP);

	SetQuestFunction("FindAndDestroyInPuertoRico", "FindAndDestroyInPuertoRico", "l", "PuertoRico");
}

void FindAndDestroyInPuertoRico(string qName)
{
	SetNextWeather("Storm04_add");
	Island_SetReloadEnableGlobal("PuertoRico", false);
	bQuestDisableMapEnter = true;

	// Gregg: условия на Group_Death тут больше нет. Глава закрывается в каюте, а группа
	// умирает позже, когда приз добит или взят, - запись про мановар приходила ПОСЛЕ
	// записи про Стронгфилда и вдобавок откатывала состояние с END обратно на «15».
	// Дно мановара теперь фиксируется там же, где гибнет Стронгфилд.

	// Gregg: глава закрывается в каюте мановара, разговором с Поллом Стронгфилдом. Пустить
	// корабль на дно пушками никто не мешает, и тогда всё, ради чего затевалась трилогия,
	// уходит под воду вместе с ним. Это провал, как у Харпича в главе 7 и Фишера в главе 9.
	PChar.quest.FindAndDestroyManowarSink.win_condition.l1 = "Character_sink";
	PChar.quest.FindAndDestroyManowarSink.win_condition.l1.character = "CursedBlackManowar";
	PChar.quest.FindAndDestroyManowarSink.function = "FindAndDestroyManowarSink";
}

// Gregg: мановар утоплен. Стронгфилда и ответа больше нет - линия закрывается провалом.
void FindAndDestroyManowarSink(string qName)
{
	// Gregg: после каюты линия уже дошла до «15», топить там нечего - выходим.
	if (PChar.FindAndDestroy != "14") return;
	PChar.FindAndDestroy = "FAILED";
	BO_FDRecord("18");

	Island_SetReloadEnableGlobal("PuertoRico", true);
	bQuestDisableMapEnter = false;

	Group_DeleteGroup("CursedBlackManowar");
	SetNextWeather("12 Hour");
	CloseQuestHeader("FindAndDestroy");
}

// Gregg: Стронгфилд мёртв, мановар уходит на дно. Раньше это висело на Group_Death
// и приходило после финальной записи.
void FindAndDestroyDeadManowar(string qName)
{
	if (PChar.FindAndDestroy == "FAILED") return;
	PChar.quest.FindAndDestroyManowarSink.over = "yes";
	PChar.FindAndDestroy = "15";
	BO_FDRecord("15");

	Island_SetReloadEnableGlobal("PuertoRico", true);
	bQuestDisableMapEnter = false;

	Group_DeleteGroup("CursedBlackManowar");
	SetNextWeather("12 Hour");
}

void FindAndDestroyTalkInManowar(string qName)
{
	ref chr = GetCharacter(NPC_GenerateCharacter("Poll Strongfield", "trader_5", "man", "man", 1, PIRATE, 0, false));
	chr.name = iTxt("BO_PollName");
	chr.lastname = iTxt("BO_PollLastName");
	chr.Dialog.FileName = "Quest\KPS\BlackOrchid\BO_CursedCaptain.c";
	chr.Dialog.CurrentNode = "4";
	chr.greeting = "Gr_Azzy";
	LAi_SetImmortal(chr, true);

	ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1");
	SetDialogCharacter2Character(PChar, chr, 15.0, true);
}

void FindAndDestroyManowarShot()
{
	// Gregg: камеру не переставляем. MSG_CAMERA_TOPOS принимает АБСОЛЮТНЫЕ координаты
	// локации; точка (-4, 12, 0) перенесена из К:ПС дословно, но каюта приза в CSP другая,
	// и камера уезжала наружу, снимая её сверху сквозь крышу. Штатная следящая камера
	// снимает сцену правильно. Остальные камерные точки главы стоят в наземных локациях
	// и работают, эта была единственная в каюте.

	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("Poll Strongfield"));
	DoQuestFunctionDelay("FindAndDestroyManowarAim", 1.5);
}

bool FindAndDestroyManowarCanShoot()
{
	if (GetCharacterEquipByGroup(PChar, GUN_ITEM_TYPE) == "") return false;
	if (!CheckAttribute(PChar, "chr_ai.charge")) return false;
	return stf(PChar.chr_ai.charge) >= 1.0 && iGetMinPistolChargeNum(PChar) > 0;
}

void FindAndDestroyManowarAim(string _tmp)
{
	LAi_ActorTurnToCharacter(PChar, CharacterFromID("Poll Strongfield"));
	PChar.quest.FindAndDestroyManowarDead.function = "FindAndDestroyManowarDead";
	if (!FindAndDestroyManowarCanShoot())
	{
		// LEO: без заряда завершаем сцену за затемнением, не требуя от игрока боеприпасов.
		LAi_Fade("FindAndDestroyManowarDead", "");
		return;
	}

	// LEO: квест по окончании анимации сбрасывает позу актёра, иначе Shot запускается снова.
	SetEventHandler("ActorMakeShot", "FindAndDestroyManowarSpendShot", 0);
	LAi_ActorAnimation(PChar, "Shot", "FindAndDestroyManowarDead", 3.0);
}

void FindAndDestroyManowarSpendShot()
{
	aref shooter = GetEventData();
	if (sti(shooter.index) != GetMainCharacterIndex()) return;
	DelEventHandler("ActorMakeShot", "FindAndDestroyManowarSpendShot");

	// LEO: ActorMakeShot только рисует выстрел; списываем один заряд, даже у многоствольного оружия.
	RemoveItems(PChar, LAi_GetCharacterBulletType(PChar), 1);
	string sPowder = LAi_GetCharacterGunpowderType(PChar);
	if (sPowder != "") RemoveItems(PChar, sPowder, 1);
	string sGun = GetCharacterEquipByGroup(PChar, GUN_ITEM_TYPE);
	PChar.chr_ai.charge = func_max(0, sti(PChar.chr_ai.charge) - 1);
	PChar.chr_ai.chargeprc = "1";
	PChar.chargestage.(sGun) = PChar.chr_ai.charge;
}

void FindAndDestroyManowarDead(string _tmp)
{
	DeleteAttribute(PChar, "quest.FindAndDestroyManowarDead");
	DelEventHandler("ActorMakeShot", "FindAndDestroyManowarSpendShot");
	ref chr = CharacterFromID("Poll Strongfield");
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, false);
	LAi_KillCharacter(chr);
	DoQuestFunctionDelay("FindAndDestroyManowarDeadDone", 1.0);
}

void FindAndDestroyManowarDeadDone(string _tmp)
{
	locCameraFollow();
	LAi_Fade("", "");
	LAi_SetPlayerType(PChar);
	// LEO: даже при прерванной анимации убираем пистолет и возвращаем ГГ в бой с капитаном.
	SendMessage(PChar, "lsl", MSG_CHARACTER_EX_MSG, "GunBelt", true);
	LAi_SetFightMode(PChar, true);
	FindAndDestroyDeadManowar("");
	// Gregg: глава кончается не выстрелом, а разговором с Кендартом - как в К:ПС.
	// Ветку открывает case "First time" по состоянию "15".
}

void FindAndDestroyEnd()
{
	TakeNItems(PChar, "SandClock", 1);
	PChar.FindAndDestroy = "END";
	BO_FDRecord("16");
	CloseQuestHeader("FindAndDestroy");
	pchar.questdata.FindAndDestroy = 1;	// LEO: отметка в энциклопедии
	UnlockAchievement("RiddlesCaribbean_quest", 3);	// LEO: трилогия "Загадки Карибов" закончена

	// Gregg: следом идёт глава 10 - «Две сестры»
	SetTimerFunction("TwoSistersTimeEnd", 0, 0, BO_FD_NEXT_DAYS);
	AddSimpleRumourToAllNations(iTxt("BO_FD_Rumour1") + GetFullName(PChar) + iTxt("BO_FD_Rumour2"), 30, 5);
}
// Gregg: <== глава 9
// Gregg: <== трилогия «Чёрная Орхидея»

// LEO: миграция использует сроки BO_* выше; подключаем её после их определения.
#include "Quests\quest_load_repairs.c"
