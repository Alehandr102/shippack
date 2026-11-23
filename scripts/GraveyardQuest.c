//=====================================================================================================================================
// Генераторный квест смотрителя кладбища "Упокоение умерших"
// Раз в 90 дней (только ночью, 00:00-06:00) смотритель кладбища (Гавана, Порт-Ройал, Виллемстад, Бас-Тер) просит ГГ
// зачистить кладбище от нежити: поверхность кладбища, склеп (Common_Crypt) и крипту (Common_CryptBig2).
// Кулдаун общий для всех 4 городов (хранится на ГГ, отсчёт от выполнения). Все записи - в судовом журнале (квест GraveyardRest).
//=====================================================================================================================================

// Доступен с 10 ранга ГГ
#define GRAVEYARDQUEST_MINRANK		10
// Не чаще одного раза в 90 дней (раз в 3 месяца)
#define GRAVEYARDQUEST_COOLDOWN		90
// Скелетов на поверхности кладбища - по одному на КАЖДЫЙ локатор rld (loc1..loc18, модель Churchyard общая для всех 4 городов)
#define GRAVEYARDQUEST_MAXSURFACE	18
// Шанс редкой пасхалки: говорящий скелет в часовне после зачистки (%)
#define GRAVEYARDQUEST_CHAPEL_CHANCE	5

// Окно времени, когда квест можно взять (ночь, 00:00-06:00)
bool GraveyardQuest_IsNight()
{
	return (GetTime() < 6.0);
}

// Проверка доступности нового задания у конкретного смотрителя
bool GraveyardQuest_IsAvailable(ref npchar)
{
	if (CheckAttribute(pchar, "GenQuest.GraveyardQuest")) return false; // уже есть активное задание
	if (sti(pchar.rank) < GRAVEYARDQUEST_MINRANK) return false;
	if (!GraveyardQuest_IsNight()) return false; // только ночью
	// общий кулдаун на квест (хранится на ГГ): выполнил в одном городе - 90 дней нельзя брать ни в одном
	if (GetNpcQuestPastDayWOInit(pchar, "GraveyardQuestDate") < GRAVEYARDQUEST_COOLDOWN) return false;
	return true;
}

// Старт задания (вызывается из диалога смотрителя)
void GraveyardQuest_Start(ref npchar)
{
	string sTown = npchar.City;

	pchar.GenQuest.GraveyardQuest.Town      = sTown;
	pchar.GenQuest.GraveyardQuest.WatcherId = npchar.id;
	pchar.GenQuest.GraveyardQuest.Stage     = "surface";
	// 50% - после зачистки склепа и крипты на поверхности поднимется СТАРШИЙ.
	// Ролим ПРИ ВЗЯТИИ квеста (не переролить перезагрузкой), флаг живёт в GenQuest
	if (rand(1) == 1) pchar.GenQuest.GraveyardQuest.AvengerRoll = true;

	// судовой журнал (userdata привязывается к ПОСЛЕДНЕЙ записи, поэтому ставим её ПОСЛЕ AddQuestRecord)
	ReOpenQuestHeader("GraveyardRest");
	AddQuestRecord("GraveyardRest", "1");
	AddQuestUserData("GraveyardRest", "sCity", XI_ConvertString("Colony" + sTown + "Gen"));

	// спавн нежити при входе на кладбище
	pchar.quest.GraveyardQuest_Surface.win_condition.l1 = "location";
	pchar.quest.GraveyardQuest_Surface.win_condition.l1.location = sTown + "_Graveyard";
	pchar.quest.GraveyardQuest_Surface.function = "GraveyardQuest_SpawnSurface";
}

// Вход на кладбище -> спавним сильную нежить на поверхности (группа локаторов rld, loc#)
// Скелеты делаются такими же, как в церковном квесте "Очистка от нечисти" (DestroyGhost).
void GraveyardQuest_SpawnSurface(string qName)
{
	pchar.quest.GraveyardQuest_Surface.over = "yes";

	string sTown  = pchar.GenQuest.GraveyardQuest.Town;
	string sGrave = sTown + "_Graveyard";
	ref    rLoc   = &locations[FindLocation(sGrave)];

	LAi_LocationFightDisable(rLoc, false);

	// количество от числа офицеров-абордажников: ГГ один - 3 скелета, при максимуме (9 абордажников) - все 18.
	// Линейно: 3 + офицеры*5/3 (0->3, 3->8, 6->13, 9->18)
	int iCount = 3 + makeint(GetOfficersQuantity(pchar) * 5 / 3);
	if (iCount > GRAVEYARDQUEST_MAXSURFACE) iCount = GRAVEYARDQUEST_MAXSURFACE;

	LAi_group_Register("GraveyardSurfaceGrp");

	ref sld;
	for (int i = 0; i < iCount; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("GraveyardQuest_Skel_" + i, "Skel" + (rand(3)+1), "skeleton", "skeleton", sti(pchar.rank)+5, PIRATE, 1, true));
		// крутые скелеты, как в церковном квесте упокоения нечисти
		FantomMakeCoolFighter(sld, sti(pchar.rank)+5, 90, 90, LinkRandPhrase(RandPhraseSimple("blade23","blade25"), RandPhraseSimple("blade30","blade26"), RandPhraseSimple("blade24","blade13")), RandPhraseSimple("pistol6", "pistol3"), MOD_SKILL_ENEMY_RATE*3*4);
		DeleteAttribute(sld, "SuperShooter");
		if (IsCharacterPerkOn(sld, "Ciras")) SetFantomWearCirass(sld);
		LAi_SetWarriorType(sld); // без SetStay - не стоят на точках, а преследуют
		LAi_group_MoveCharacter(sld, "GraveyardSurfaceGrp");
		ChangeCharacterAddressGroup(sld, sGrave, "rld", "loc" + (i+1));
		SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY); // личная вражда: не кликаются на диалог
	}

	LAi_group_SetRelation("GraveyardSurfaceGrp", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_SetLookRadius("GraveyardSurfaceGrp", 1000.0); // видят ГГ через всё кладбище
	LAi_group_FightGroups(LAI_GROUP_PLAYER, "GraveyardSurfaceGrp", true); // бой стартует сразу - сбегаются в атаку со всех локаторов
	LAi_group_SetCheckFunction("GraveyardSurfaceGrp", "GraveyardQuest_SurfaceCleared");

	AddQuestRecord("GraveyardRest", "2");
}

// Поверхность зачищена -> отправляем в склеп и крипту
void GraveyardQuest_SurfaceCleared(string grp)
{
	pchar.GenQuest.GraveyardQuest.Stage = "depths";

	AddQuestRecord("GraveyardRest", "3");

	// склеп - малая крипта на кладбище
	pchar.quest.GraveyardQuest_Sklep.win_condition.l1 = "location";
	pchar.quest.GraveyardQuest_Sklep.win_condition.l1.location = "Common_Crypt";
	pchar.quest.GraveyardQuest_Sklep.function = "GraveyardQuest_SpawnSklep";

	// крипта - внутренняя крипта (вход из <City>_CryptBig1)
	pchar.quest.GraveyardQuest_Crypt.win_condition.l1 = "location";
	pchar.quest.GraveyardQuest_Crypt.win_condition.l1.location = "Common_CryptBig2";
	pchar.quest.GraveyardQuest_Crypt.function = "GraveyardQuest_SpawnCrypt";
}

// Генерация сильного скелета-мини-босса в локаторе goto
void GraveyardQuest_MakeBoss(string sId, string sLoc, string sLocator, int iRankBonus, int iFenc, float fAddHP, int iHpBonus, string sGroup)
{
	ref sld = GetCharacter(NPC_GenerateCharacter(sId, "Skel" + (rand(3)+1), "skeleton", "skeleton", sti(pchar.rank)+iRankBonus, PIRATE, 1, true));
	FantomMakeCoolFighter(sld, sti(pchar.rank)+iRankBonus, iFenc, iFenc, LinkRandPhrase(RandPhraseSimple("blade23","blade25"), RandPhraseSimple("blade30","blade26"), RandPhraseSimple("blade24","blade13")), RandPhraseSimple("pistol6", "pistol3"), fAddHP);
	DeleteAttribute(sld, "SuperShooter");
	if (IsCharacterPerkOn(sld, "Ciras")) SetFantomWearCirass(sld);
	// мини-боссам добавляем запас здоровья
	int hp = LAi_GetCharacterMaxHP(sld) + iHpBonus;
	LAi_SetHP(sld, hp, hp);
	LAi_SetCurHPMax(sld);
	LAi_SetWarriorType(sld);
	LAi_warrior_SetStay(sld, true);
	LAi_group_MoveCharacter(sld, sGroup);
	ChangeCharacterAddressGroup(sld, sLoc, "goto", sLocator);
}

// Вход в склеп (Common_Crypt) -> 2 сильных скелета-мини-босса
void GraveyardQuest_SpawnSklep(string qName)
{
	pchar.quest.GraveyardQuest_Sklep.over = "yes";

	ref rLoc = &locations[FindLocation("Common_Crypt")];
	LAi_LocationFightDisable(rLoc, false);

	LAi_group_Register("GraveyardSklepGrp");
	GraveyardQuest_MakeBoss("GraveyardQuest_Sklep_0", "Common_Crypt", "goto1", 8, 95, MOD_SKILL_ENEMY_RATE*3*6, 100 + sti(pchar.rank)*3, "GraveyardSklepGrp");
	GraveyardQuest_MakeBoss("GraveyardQuest_Sklep_1", "Common_Crypt", "goto2", 8, 95, MOD_SKILL_ENEMY_RATE*3*6, 100 + sti(pchar.rank)*3, "GraveyardSklepGrp");

	LAi_group_SetRelation("GraveyardSklepGrp", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_SetCheckFunction("GraveyardSklepGrp", "GraveyardQuest_SklepCleared");
}

// Вход в крипту (Common_CryptBig2) -> 2 мини-босса посильнее, чем в склепе
void GraveyardQuest_SpawnCrypt(string qName)
{
	pchar.quest.GraveyardQuest_Crypt.over = "yes";

	ref rLoc = &locations[FindLocation("Common_CryptBig2")];
	LAi_LocationFightDisable(rLoc, false);

	LAi_group_Register("GraveyardCryptGrp");
	GraveyardQuest_MakeBoss("GraveyardQuest_Crypt_0", "Common_CryptBig2", "goto1", 12, 100, MOD_SKILL_ENEMY_RATE*3*8, 200 + sti(pchar.rank)*5, "GraveyardCryptGrp");
	GraveyardQuest_MakeBoss("GraveyardQuest_Crypt_1", "Common_CryptBig2", "goto2", 12, 100, MOD_SKILL_ENEMY_RATE*3*8, 200 + sti(pchar.rank)*5, "GraveyardCryptGrp");

	LAi_group_SetRelation("GraveyardCryptGrp", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_SetCheckFunction("GraveyardCryptGrp", "GraveyardQuest_CryptCleared");
}

// Склеп зачищен
void GraveyardQuest_SklepCleared(string grp)
{
	pchar.GenQuest.GraveyardQuest.SklepDone = true;
	GraveyardQuest_CheckDepths();
}

// Крипта зачищена
void GraveyardQuest_CryptCleared(string grp)
{
	pchar.GenQuest.GraveyardQuest.CryptDone = true;
	GraveyardQuest_CheckDepths();
}

// Ролл пасхалки: говорящий скелет в часовне. ОДНА ЗА ВСЮ ИГРУ (плоский флаг pchar.GraveyardChapelEgg,
// как pchar.Dovahkiin у звука довакина в церковном квесте). Зовётся ПОСЛЕ убийства Старшего,
// либо из CheckDepths - если Старший в этот раз не поднялся (50% при взятии квеста).
void GraveyardQuest_TryChapelEgg()
{
	if (!CheckAttribute(pchar, "GenQuest.GraveyardQuest")) return; // квест уже сдан/сброшен
	if (CheckAttribute(pchar, "GraveyardChapelEgg")) return;       // пасхалка уже была - одна за всю игру
	if (rand(99) < GRAVEYARDQUEST_CHAPEL_CHANCE)
	{
		pchar.GraveyardChapelEgg = 1; // случилась - больше никогда (флаг ВНЕ GenQuest, навсегда в сейве)
		pchar.GenQuest.GraveyardQuest.ChapelSkel = true;
		// спавн скелета при входе в часовню (<City>_Chapel)
		pchar.quest.GraveyardQuest_Chapel.win_condition.l1 = "location";
		pchar.quest.GraveyardQuest_Chapel.win_condition.l1.location = pchar.GenQuest.GraveyardQuest.Town + "_Chapel";
		pchar.quest.GraveyardQuest_Chapel.function = "GraveyardQuest_SpawnChapelSkel";
	}
}

// Проверка: оба подземелья (склеп и крипта) зачищены -> можно за наградой
void GraveyardQuest_CheckDepths()
{
	if (!CheckAttribute(pchar, "GenQuest.GraveyardQuest.SklepDone")) return;
	if (!CheckAttribute(pchar, "GenQuest.GraveyardQuest.CryptDone")) return;
	pchar.GenQuest.GraveyardQuest.Stage = "reward";
	AddQuestRecord("GraveyardRest", "4");
	AddQuestUserData("GraveyardRest", "sCity", XI_ConvertString("Colony" + pchar.GenQuest.GraveyardQuest.Town + "Gen"));

	if (CheckAttribute(pchar, "GenQuest.GraveyardQuest.AvengerRoll"))
	{
		// выпал СТАРШИЙ (50% при взятии): при выходе на поверхность встретит скелет-воин.
		// Пасхалка тогда ролится ПОСЛЕ его убийства - см. GraveyardQuest_AvengerDead
		pchar.quest.GraveyardQuest_Avenger.win_condition.l1 = "location";
		pchar.quest.GraveyardQuest_Avenger.win_condition.l1.location = pchar.GenQuest.GraveyardQuest.Town + "_Graveyard";
		pchar.quest.GraveyardQuest_Avenger.function = "GraveyardQuest_SpawnAvenger";
	}
	else
	{
		// Старший не поднялся - пасхалка ролится сразу после зачистки подземелий
		GraveyardQuest_TryChapelEgg();
	}
}

// Проверка офицеров на страх (по образцу штатной scareOfficers, но с ВИДИМЫМ бегством):
// провалившие проверку не телепортируются, а БЕГУТ без оглядки к выходу (reload1) и скрываются в каюте.
void GraveyardQuest_ScareOfficers(int minSkill)
{
	int i, idx;
	int iStayed = 0; // LEO: счётчик абордажников, переборовших страх и оставшихся (Старшему +100 HP за каждого)
	ref sld;
	if (CheckOfficersPerk(pchar, "IronWill")) minSkill = makeint(minSkill * 0.7);
	// страховка от "вечных беглецов": при выходе ГГ из локации вернуть штатный тип всем застрявшим акторам
	pchar.quest.GraveyardQuest_FledReset.win_condition.l1 = "ExitFromLocation";
	pchar.quest.GraveyardQuest_FledReset.win_condition.l1.location = pchar.location;
	pchar.quest.GraveyardQuest_FledReset.win_condition = "GraveyardQuest_FledReset";
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		idx = GetOfficersIndex(pchar, i);
		if (idx == -1) continue;
		sld = GetCharacter(idx);
		if (sld.location != pchar.location) continue;
		if (CheckAttribute(sld, "HasNoFear"))
		{
			iStayed++; // LEO: бесстрашный - остался драться
			Log_Info(GetFullName(sld)+GetConvertStrWithReplace("Variable_reaction_functions_47", "Logs.txt", "#space#", " ")+NPCharSexPhrase(sld,GetConvertStrWithReplace("Variable_reaction_functions_48", "Logs.txt", "#space#", " "),GetConvertStrWithReplace("Variable_reaction_functions_49", "Logs.txt", "#space#", " "))+ GetConvertStrWithReplace("Variable_reaction_functions_50", "Logs.txt", "#space#", " "));
			continue;
		}
		if (minSkill + drand(30 + i) > sti(GetSummonSkillFromName(sld, SKILL_LEADERSHIP)))
		{
			// бежит без оглядки до выхода с кладбища и скрывается в каюте корабля
			LAi_SetFightMode(sld, false);
			LAi_SetActorType(sld);
			LAi_ActorRunToLocation(sld, "reload", "reload1", Get_My_Cabin(), "goto", "goto1", "GraveyardQuest_OfficerFled", -1.0);
			Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_39", "Logs.txt", "#space#", " ")+GetFullName(sld)+GetConvertStrWithReplace("Variable_reaction_functions_40", "Logs.txt", "#space#", " ")+NPCharSexPhrase(sld,"",GetConvertStrWithReplace("Variable_reaction_functions_41", "Logs.txt", "#space#", " "))+ GetConvertStrWithReplace("Variable_reaction_functions_42", "Logs.txt", "#space#", " "));
		}
		else
		{
			iStayed++; // LEO: переборол страх - остался драться
			Log_Info(GetConvertStrWithReplace("Variable_reaction_functions_43", "Logs.txt", "#space#", " ")+GetFullName(sld)+GetConvertStrWithReplace("Variable_reaction_functions_44", "Logs.txt", "#space#", " ")+NPCharSexPhrase(sld,"",GetConvertStrWithReplace("Variable_reaction_functions_45", "Logs.txt", "#space#", " "))+ GetConvertStrWithReplace("Variable_reaction_functions_46", "Logs.txt", "#space#", " "));
		}
	}
	pchar.GenQuest.GraveyardQuest.AvengerStayed = iStayed; // LEO: сохраняем для GraveyardQuest_SpawnAvenger (+100 HP/абордажник)
}

// Сбежавший офицер добрался до каюты - вернуть ему штатный тип офицера (иначе останется актором)
void GraveyardQuest_OfficerFled()
{
	int i, idx;
	ref sld;
	string sCabin = Get_My_Cabin();
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		idx = GetOfficersIndex(pchar, i);
		if (idx == -1) continue;
		sld = GetCharacter(idx);
		if (sld.location == sCabin) LAi_SetOfficerType(sld);
	}
}

// Страховка: ГГ покинул кладбище раньше, чем беглецы добежали до выхода. Актёрская задача "бежать к reload"
// уехала бы с ними по локациям (партийный логин тип не сбрасывает) - офицеры молча убегали бы при каждом входе.
// Возвращаем штатный тип ВСЕМ офицерам партии, застрявшим акторами.
void GraveyardQuest_FledResetAll()
{
	int i, idx;
	ref sld;
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		idx = GetOfficersIndex(pchar, i);
		if (idx == -1) continue;
		sld = GetCharacter(idx);
		if (!CheckAttribute(sld, "chr_ai.type")) continue;
		if (sld.chr_ai.type == LAI_TYPE_ACTOR) LAi_SetOfficerType(sld);
	}
}

// Выход на поверхность кладбища после зачистки склепа и крипты -> СТАРШИЙ: скелет-воин-минибосс.
// Холодное оружие 100, ровно 800 HP, звезда минибосса (как у главаря "Стычки банды и патруля"),
// офицеры проходят проверку на страх (scareOfficers - штатная механика, как у Салазара/Юм Симилию).
void GraveyardQuest_SpawnAvenger(string qName)
{
	pchar.quest.GraveyardQuest_Avenger.over = "yes";
	if (!CheckAttribute(pchar, "GenQuest.GraveyardQuest")) return;       // квест уже сдан/сброшен
	if (GetCharacterIndex("GraveyardQuest_Avenger") != -1) return;       // уже стоит

	string sTown  = pchar.GenQuest.GraveyardQuest.Town;
	string sGrave = sTown + "_Graveyard";
	ref    rLoc   = &locations[FindLocation(sGrave)];
	LAi_LocationFightDisable(rLoc, false);

	LAi_group_Register("GraveyardAvengerGrp");
	ref sld = GetCharacter(NPC_GenerateCharacter("GraveyardQuest_Avenger", "Skel" + (rand(3)+1), "skeleton", "skeleton", sti(pchar.rank)+15, PIRATE, 1, true));
	FantomMakeCoolFighter(sld, sti(pchar.rank)+15, 100, 100, LinkRandPhrase(RandPhraseSimple("blade23","blade25"), RandPhraseSimple("blade30","blade26"), RandPhraseSimple("blade24","blade13")), RandPhraseSimple("pistol6", "pistol3"), MOD_SKILL_ENEMY_RATE*3*8);
	DeleteAttribute(sld, "SuperShooter");
	if (IsCharacterPerkOn(sld, "Ciras")) SetFantomWearCirass(sld);
	// ровно 800 HP (ПОСЛЕ FantomMakeCoolFighter - тот выставляет своё здоровье)
	LAi_SetHP(sld, 800, 800);
	LAi_SetCurHPMax(sld);
	// LEO: усиление Старшего - пропуск флинча/дёрганья от ЛЮБОГО удара ПО СЛОЖНОСТИ 20/40/60% (как сопротивление урона; HitStunSkip через NPC_Event_HitStunSkip)
	// + запрет разоружения (nodisarm) + 100% пробитие блоков (как у Стража грота) + сопротивление урону по сложности 20/40/60%.
	int iStunSkip = 20;
	float fResist = 0.2;
	if (MOD_SKILL_ENEMY_RATE == 2) { iStunSkip = 40; fResist = 0.4; }
	if (MOD_SKILL_ENEMY_RATE == 3) { iStunSkip = 60; fResist = 0.6; }
	sld.HitStunSkip = iStunSkip;
	sld.nodisarm = true;
	sld.FullBlockPierce = true;
	sld.DmgResist = fResist;
	// характер "Рубака": +15% урона от клинка, +30 стартовой энергии.
	// Сначала снимаем случайный характер фантома - иначе SetSpeciality молча откажет ("характер уже есть")
	RemoveSpeciality(sld);
	SetCharacterPerk(sld, "Grunt");
	// звезда минибосса: bGangLeader защищает знак от стирания боевыми FX (см. UnmarkCharacter в utils.c)
	sld.bGangLeader = true;
	MarkCharacter(sld, "QM_MiniBoss");
	LAi_SetWarriorType(sld);
	LAi_group_MoveCharacter(sld, "GraveyardAvengerGrp");
	ChangeCharacterAddressGroup(sld, sGrave, "rld", "loc3");

	LAi_group_SetRelation("GraveyardAvengerGrp", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_SetLookRadius("GraveyardAvengerGrp", 1000.0); // видит ГГ через всё кладбище
	SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY); // личная вражда: не кликается на диалог
	LAi_group_FightGroups(LAI_GROUP_PLAYER, "GraveyardAvengerGrp", true); // бой стартует СРАЗУ - бежит в атаку (group_Attack давал лишь временную цель)
	LAi_group_SetCheckFunction("GraveyardAvengerGrp", "GraveyardQuest_AvengerDead"); // после его смерти - ролл пасхалки

	// проверка офицеров на страх: слабые духом убегают без оглядки к выходу (порог 70, как у сильной нежити)
	GraveyardQuest_ScareOfficers(70);
	// LEO: +100 HP Старшему за КАЖДОГО абордажника, кто переборол страх и остался драться (база 800)
	int iStayed = 0;
	if (CheckAttribute(pchar, "GenQuest.GraveyardQuest.AvengerStayed")) iStayed = sti(pchar.GenQuest.GraveyardQuest.AvengerStayed);
	if (iStayed > 0)
	{
		int iHP = 800 + iStayed * 100;
		LAi_SetHP(sld, iHP, iHP);
		LAi_SetCurHPMax(sld);
	}
	// LEO: метод из квеста "Кондотьер" (убийство Карлоса Кассира) - анти-эксплойт сейв/лоада БЕЗ запрета сохранения:
	// офицеры НЕ ре-появляются в локации при сейв/лоаде (убежавшие не возвращаются), + блок всех выходов до смерти Старшего.
	LAi_LocationDisableOfficersGen(sGrave, true);
	chrDisableReloadToLocation = true;
}

// СТАРШИЙ убит -> метка для текста пасхалки (со Старшим/без) + ролл пасхалки.
// Если выпавшего Старшего НЕ убили (сдали квест/ушли) - пасхалки в этот раз нет.
void GraveyardQuest_AvengerDead(string grp)
{
	chrDisableReloadToLocation = false; // LEO: Старший мёртв - разблок выходов
	if (!CheckAttribute(pchar, "GenQuest.GraveyardQuest")) return; // квест уже сдан/сброшен
	LAi_LocationDisableOfficersGen(pchar.GenQuest.GraveyardQuest.Town + "_Graveyard", false); // снова пускаем офицеров в локацию
	pchar.GenQuest.GraveyardQuest.AvengerKilled = true; // скелет в часовне упомянет победу над Старшим
	GraveyardQuest_TryChapelEgg();
}

// Вход в часовню (пасхалка) -> ставим мирного говорящего скелета в локатор barmen/stay
void GraveyardQuest_SpawnChapelSkel(string qName)
{
	pchar.quest.GraveyardQuest_Chapel.over = "yes";
	if (!CheckAttribute(pchar, "GenQuest.GraveyardQuest")) return;        // квест уже сдан/сброшен
	if (GetCharacterIndex("GraveyardQuest_ChapelSkel") != -1) return;     // уже стоит

	string sChapel = pchar.GenQuest.GraveyardQuest.Town + "_Chapel";
	ref sld = GetCharacter(NPC_GenerateCharacter("GraveyardQuest_ChapelSkel", "Skel" + (rand(3)+1), "skeleton", "skeleton", sti(pchar.rank), PIRATE, 1, true));
	sld.dialog.filename    = "Graveyard_ChapelSkel_dialog.c";
	sld.dialog.currentnode = "First time";
	sld.greeting = "";
	LAi_SetStayTypeNoGroup(sld); // стоит на месте, отвечает на диалоги, не боится
	SetCharacterRelationBoth(GetMainCharacterIndex(), sti(sld.index), RELATION_NEUTRAL); // мирный к ГГ
	ChangeCharacterAddressGroup(sld, sChapel, "barmen", "stay");
}

// Выдача награды (вызывается из диалога смотрителя)
void GraveyardQuest_GiveReward(ref npchar)
{
	// LEO: страховка - снимаем анти-эксплойт боя со Старшим (разблок выходов + снова пускаем офицеров)
	chrDisableReloadToLocation = false;
	LAi_LocationDisableOfficersGen(npchar.City + "_Graveyard", false);
	int tier;
	if (sti(pchar.rank) >= 25)      tier = 4;
	else if (sti(pchar.rank) >= 20) tier = 3;
	else                            tier = 2;

	// 2 случайные книги опыта нужного тира, исключая уже прочитанные (их нельзя читать повторно)
	string aBooks[14];
	int    nFree = 0;
	string sId;
	for (int k = 1; k <= 14; k++)
	{
		sId = "book" + tier + "_" + k;
		if (CheckAttribute(pchar, "books." + sId) && sti(pchar.books.(sId)) <= 0) continue; // уже прочитана
		aBooks[nFree] = sId;
		nFree++;
	}

	int iGiven = 0;
	int r;
	while (iGiven < 2 && nFree > 0)
	{
		r = rand(nFree - 1);
		GiveItem2Character(pchar, aBooks[r]);
		aBooks[r] = aBooks[nFree - 1];
		nFree--;
		iGiven++;
	}

	// 500 опыта в авторитет, везение и скрытность
	AddCharacterExpToSkill(pchar, "Leadership", 500);
	AddCharacterExpToSkill(pchar, "Fortune",    500);
	AddCharacterExpToSkill(pchar, "Sneak",      500);

	// репутация ГГ и отношение нации - как в церковном квесте упокоения нечисти
	ChangeCharacterReputation(pchar, 5);
	ChangeCharacterNationReputation(pchar, sti(npchar.nation), 15);
	OfficersReaction("good");

	// общий кулдаун: фиксируем дату выполнения на ГГ - 90 дней нельзя брать квест ни в одном городе
	SaveCurrentNpcQuestDateParam(pchar, "GraveyardQuestDate");

	AddQuestRecord("GraveyardRest", "5");
	AddQuestUserData("GraveyardRest", "sCity", XI_ConvertString("Colony" + npchar.City + "Gen"));
	CloseQuestHeader("GraveyardRest");

	AchievementsCounter_genquests(1); // засчитываем в ачивку "Выполнено генераторных квестов"

	// слух в таверне о том, что ГГ упокоил мертвецов на кладбище этого города
	pchar.GraveyardRumor = npchar.City;
	pchar.GraveyardRumorDay = DateToInt(0); // дата упокоения: через месяц новость затихает сама (фикс вечного слуха)
	DeleteAttribute(pchar, "GraveyardRumorTold");

	// если сработала пасхалка - убрать говорящего скелета из часовни (квест сдан)
	if (CheckAttribute(pchar, "GenQuest.GraveyardQuest.ChapelSkel"))
	{
		if (GetCharacterIndex("GraveyardQuest_ChapelSkel") != -1)
		{
			ref skel = CharacterFromID("GraveyardQuest_ChapelSkel");
			LAi_RemoveCheckMinHP(skel);
			skel.LifeDay = 0; // пометить на удаление
		}
	}
	// нейтрализуем условие спавна в часовне, если оно так и не сработало (ГГ не заходил в часовню)
	if (CheckAttribute(pchar, "quest.GraveyardQuest_Chapel")) pchar.quest.GraveyardQuest_Chapel.over = "yes";
	// нейтрализуем условие спавна СТАРШЕГО (если ГГ так и не вышел на кладбище); живой босс исчезнет сам (lifeDay=1)
	if (CheckAttribute(pchar, "quest.GraveyardQuest_Avenger")) pchar.quest.GraveyardQuest_Avenger.over = "yes";

	DeleteAttribute(pchar, "GenQuest.GraveyardQuest");
}
//=====================================================================================================================================
// <-- Квест смотрителя кладбища "Упокоение умерших"
//=====================================================================================================================================
