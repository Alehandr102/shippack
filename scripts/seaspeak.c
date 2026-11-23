// ============================================================================
//  ПЕРЕГОВОРЫ В МОРЕ (Speak) - логика. UI: INTERFACE\Speak.c (форма Speak.ini).
//  Точка входа - команда "Переговоры" боевого интерфейса (BI_Speak, радиус 500,
//  включается в BattleInterface.c: Commands.Speak.enable).
//  Sea_Speak решает режим разговора и запускает интерфейс LaunchSpeakInterface.
// ============================================================================


void Sea_Speak(aref chr, int whospeak, int isTaxAsk)
{
	// chr - корабль собеседника; whospeak - кто начал разговор (индекс ГГ либо чужого корабля)
	chr.dialogtype = "";
	string sMode = "";

	if (whospeak != sti(pchar.index))
	{
		// корабль заговаривает сам (зовёт Speak_EnemyInitiative из периодики ИИ);
		// повторные вызовы дозволены - частоту режет кулдаун в Speak_EnemyInitiative
		if (isTaxAsk == -2)
		{
			sMode = "surrender"; // враг сам сдаёт корабль (решение принято в Speak_EnemyInitiative)
		}
		else
		{
			if (isTaxAsk == -1)
			{
				if (CalculateForRansom(chr)) sMode = "ransom";
			}
			else
			{
				if (!CheckAttribute(chr, "speak.tax")) sMode = "tax";
			}
		}
	}

	if (sMode != "" || whospeak == sti(pchar.index))
	{
		// входящий вызов - звуковой сигнал
		if (whospeak != sti(pchar.index)) PlaySound("interface\noviy_urov.wav");
		LaunchSpeakInterface(chr, whospeak, sMode);
	}
}

// чистка отметок переговоров у фантомов (вызывать при необходимости при пересоздании моря)
void DeleteRansomCharacters()
{
	for (int i = 800; i < 1000; i++)
	{
		DeleteAttribute(&characters[i], "ransom");
		DeleteAttribute(&characters[i], "speak");
	}
}

// выход из интерфейса с iFlag == NEUTRAL: враг откупился контрибуцией - отпускаем с миром
void AnalizeSituations(aref rSpeakCharacter)
{
	string sGroup = "Analize_Group_" + rSpeakCharacter.id;
	if (CheckAttribute(rSpeakCharacter, "speak.ransom.taked"))
	{
		Ship_Neutral(GetCharacterIndex(rSpeakCharacter.id), sGroup);
	}
}

// выход с iFlag == TAX: мы откупились - вся группа корабля становится нейтральной к нам
void AnalizeSituationsTax(ref rSpeakCharacter)
{
	if (CheckAttribute(rSpeakCharacter, "speak.tax"))
	{
		if (CheckAttribute(rSpeakCharacter, "SeaAI.Group.Name"))
		{
			string sGroup = rSpeakCharacter.SeaAI.Group.Name;
			Group_SetNeutralToCharacter(sGroup, nMainCharacterIndex);
			NationUpdate();
		}
	}
}

// выход с iFlag == ENEMY: переговоры провалены - собеседник атакует
void AnalizeSituationsEnemy()
{
	string sTarget = pchar.speakchr;
	Ship_SetTaskAttack(PRIMARY_TASK, GetCharacterIndex(sTarget), nMainCharacterIndex);
	SetCharacterRelationBoth(GetCharacterIndex(sTarget), nMainCharacterIndex, RELATION_ENEMY);
	NationUpdate();
}

// командир ли морской группы (требовать дань положено старшему; одиночка - сам себе командир)
bool Speak_IsGroupCommander(aref chr)
{
	if (!CheckAttribute(chr, "SeaAI.Group.Name")) return true;
	string sGroup = chr.SeaAI.Group.Name;
	int iCmdr = Group_GetGroupCommanderIndex(sGroup);
	if (iCmdr < 0) return true;
	return iCmdr == sti(chr.index);
}

// LEO: ОДИН входящий вызов НА ГРУППУ ЗА ВСТРЕЧУ (флаг на морской группе, умирает вместе с ней;
// одиночка без группы - на самом корабле). Юзер: слишком часто тревожили - раньше был кулдаун раз
// в игровой час, теперь одна попытка на группу за встречу (перезаход в море - заново)
bool Speak_CallOnCooldown(aref chr, int iHourNow)
{
	if (CheckAttribute(chr, "SeaAI.Group.Name"))
	{
		int iGrp = Group_FindGroup(chr.SeaAI.Group.Name);
		if (iGrp >= 0)
		{
			ref rGroup = Group_GetGroupByIndex(iGrp);
			return CheckAttribute(rGroup, "speaklastcall"); // LEO: один сигнал на группу за встречу (флаг живёт с группой)
		}
	}
	return CheckAttribute(chr, "speak.lastcall"); // LEO: одиночка без группы — сигналил ли уже (флаг на корабле)
}

void Speak_SetCallCooldown(aref chr, int iHourNow)
{
	chr.speak.lastcall = iHourNow;
	if (CheckAttribute(chr, "SeaAI.Group.Name"))
	{
		int iGrp = Group_FindGroup(chr.SeaAI.Group.Name);
		if (iGrp >= 0)
		{
			ref rGroup = Group_GetGroupByIndex(iGrp);
			rGroup.speaklastcall = iHourNow;
		}
	}
}

// ---------------------------------------------------------------------------
//  ИНИЦИАТИВА ВРАГА: зовётся периодикой ИИ (Ship_CheckSituation, AIShip.c).
//  Тикает боевую мораль корабля (Ship_CheckMorale) и решает, не выйти ли ему
//  на связь самому: сломленный просит пощады - предлагает откупиться (платит
//  ОН); сильный хищник (пираты / военная эскадра) требует дань при подавляющем
//  перевесе (платим МЫ). Раз за встречу; дань требует только командир группы.
// ---------------------------------------------------------------------------
void Speak_EnemyInitiative(aref chr)
{
	if (bAbordageStarted) return;
	if (sti(InterfaceStates.Launched)) return;
	if (sti(chr.ship.type) == SHIP_FORT) return;
	if (LAi_isDead(chr)) return;
	int cn = sti(chr.index);
	if (GetRelation(cn, nMainCharacterIndex) != RELATION_ENEMY) return;
	if (Speak_IsQuestHunter(chr)) return; // квестовые и ОЗГ инициативы не проявляют
	if (CheckAttribute(chr, "surrendered")) return;

	// тик боевой морали (внутри рейт-лимит CHANGE_MORALE_EVERY); при морали ниже порога
	// сдачи копится счётчик отчаяния seatime.thinkSurr
	Ship_CheckMorale(cn, false);

	// LEO: опция «Входящие переговоры в море» — при выключенной враг сам на связь не выходит (мораль всё равно тикает)
	if (CheckAttribute(&InterfaceStates, "SeaSpeakSignals")) { if (sti(InterfaceStates.SeaSpeakSignals) == 0) return; }

	// докричаться можно с дистанции кнопки "Переговоры"
	if (Ship_GetDistance2D(chr, pchar) > 500.0) return;

	// LEO: один входящий сигнал НА ГРУППУ за встречу (кулдаун-флаг ставится ниже при удачном вызове)
	int iHourNow = DateToInt(0) * 24 + GetHour();
	if (Speak_CallOnCooldown(chr, iHourNow)) return;

	// 1) сломленный просит пощады: отчаяние держится пару тиков подряд,
	//    и бьём его именно МЫ (последний попавший - ГГ или компаньон)
	int iThink = 0;
	if (CheckAttribute(chr, "seatime.thinkSurr")) iThink = sti(chr.seatime.thinkSurr);
	if (iThink >= 2)
	{
		bool bOurVictim = false;
		int iLastHitBy = -1;
		if (CheckAttribute(chr, "Ship.LastBallCharacter")) iLastHitBy = sti(chr.Ship.LastBallCharacter);
		if (iLastHitBy == nMainCharacterIndex) bOurVictim = true;
		if (!bOurVictim)
		{
			if (iLastHitBy >= 0)
			{
				if (IsCompanion(&Characters[iLastHitBy])) bOurVictim = true;
			}
		}
		if (bOurVictim)
		{
			// сломленный и наш - может либо предложить СДАТЬ корабль, либо просто откупиться.
			// Сдача = ролл воли (его слабее нашей - сдаётся); на тесте форсим 100%
			// LEO: БАГ был - макрос SPEAK_TEST_FORCE_SURRENDER (false) при bool-присвоении давал ИСТИНУ (причуда true/false в #define),
			// формула пропускалась -> сдавался кто угодно. Прямой вызов без макроса (путь "ГГ требует" так и работал верно).
			bool bOfferSurr = Speak_TryDemandSurrender(chr);
			if (bOfferSurr)
			{
				Sea_Speak(chr, cn, -2); // входящее предложение сдачи
				if (sti(InterfaceStates.Launched)) Speak_SetCallCooldown(chr, iHourNow);
				return;
			}
			Sea_Speak(chr, cn, -1); // иначе просит откупиться (внутри CalculateForRansom - может и не дозреть)
			if (sti(InterfaceStates.Launched)) Speak_SetCallCooldown(chr, iHourNow);
			return;
		}
	}

	// 2) хищник требует дань: пираты либо военная эскадра, перевес минимум двукратный
	if (CheckAttribute(chr, "speak.tax")) return;
	bool bPredator = false;
	if (sti(chr.nation) == PIRATE) bPredator = true;
	if (!bPredator)
	{
		if (CheckAttribute(chr, "EncType"))
		{
			if (chr.EncType == "war") bPredator = true;
		}
	}
	if (!bPredator) return;
	if (!Speak_IsGroupCommander(chr))
	{
		// рядовой требует свою долю, только если флагман уже выкуплен либо заплатил контрибуцию
		bool bCmdrPaid = false;
		if (CheckAttribute(chr, "SeaAI.Group.Name"))
		{
			string sGrp = chr.SeaAI.Group.Name;
			if (CheckAttribute(pchar, "questTemp.SpeakTruce.grp." + sGrp)) bCmdrPaid = true;
		}
		if (!bCmdrPaid) return;
	}
	if (Speak_ForcePercent(chr) >= 34) return; // их боевая ценность минимум вдвое больше нашей
	// ПЛЮС требование от штатной МОЩИ эскадры (классы кораблей): она тоже должна
	// превышать нашу более чем вдвое - класс обязывает, прежде чем требовать
	if (Speak_GroupClassPower(chr) <= CalculateSquadronPowerPchar() * 2) return;
	if (rand(99) >= 5) return; // не с порога - пусть назреет (~раз в 20 тиков)
	Sea_Speak(chr, cn, 0);
	if (sti(InterfaceStates.Launched)) Speak_SetCallCooldown(chr, iHourNow);
}

// ОЗГ (охотники за головой, bountyhunters.c): атрибут OZG ставится в SetShipHunter ШИРОКО
// (взыскатели Гатри, квестовые патрули и пр.), поэтому уточняем по id "<Nat>Hunter0N" -
// тот же признак, что у движка квестов (worldmap_events.c). ОЗГ нельзя стребовать
// контрибуцию, но можно ОТКУПИТЬСЯ золотом (меню Speak, исключение из гейта охотников)
bool Speak_IsOZG(ref chr)
{
	if (!CheckAttribute(chr, "OZG")) return false;
	if (findsubstr(chr.id, "Hunter0", 0) == -1) return false;
	return true;
}

// сам ПГГ (персонаж PsHero_N): с ним биться до конца - откупом его энкаунтер с глобалки
// не убрать. Наёмники-компаньоны (id PsHero_N_hunter_X / PsHero_N_Trader) различаются
// атрибутом PGG_companion - им на лапу дать можно
bool Speak_IsPGGHimself(ref chr)
{
	if (!HasSubStr(chr.id, "PsHero_")) return false;
	if (CheckAttribute(chr, "PGG_companion")) return false;
	return true;
}

// индекс корабля самого ПГГ в эскадре собеседника (-1 - в группе его нет)
int Speak_GroupPGGIndex(ref chr)
{
	if (Speak_IsPGGHimself(chr)) return sti(chr.index);
	if (!CheckAttribute(chr, "SeaAI.Group.Name")) return -1;
	int iGrp = Group_FindGroup(chr.SeaAI.Group.Name);
	if (iGrp < 0) return -1;
	ref rGroup = Group_GetGroupByIndex(iGrp);
	int i = 0;
	int cn;
	while (true)
	{
		cn = Group_GetCharacterIndexR(rGroup, i);
		if (cn < 0) break;
		if (Speak_IsPGGHimself(&Characters[cn])) return cn;
		i = i + 1;
	}
	return -1;
}

// враги, с которыми нельзя вести переговоры, но МОЖНО откупиться золотом:
// ОЗГ и нац. патруль контрафрахта (ContraPatrol, smuggling.c - у него нет квестовых сцен)
bool Speak_IsTaxOnly(ref chr)
{
	if (Speak_IsOZG(chr)) return true;
	if (CheckAttribute(chr, "ContraPatrol")) return true;
	return false;
}

// живых кораблей в группе собеседника (для разделения откупа корабль/эскадра)
int Speak_GroupAliveCount(ref chr)
{
	if (!CheckAttribute(chr, "SeaAI.Group.Name")) return 1;
	int n = Group_GetLiveCharactersNum(chr.SeaAI.Group.Name);
	if (n < 1) n = 1;
	return n;
}

// квестовые охотники переговоров НЕ ведут: у них контракт, а у нас квестовые сцены
// (каперы конвоя, взыскатели Гатри, мстители братства, перехватчики - сдача/бой в каюте по сюжету).
// Откуп от них сломал бы квестовый цикл, поэтому в меню Speak им доступен только выход.
bool Speak_IsQuestHunter(ref chr)
{
	if (CheckAttribute(chr, "ContraHunter")) return true;
	if (CheckAttribute(chr, "GentlemenHunter")) return true;
	if (CheckAttribute(chr, "FortHunter")) return true;
	if (CheckAttribute(chr, "GuildEnforcer")) return true;
	if (CheckAttribute(chr, "SmugRevengeHunter")) return true;
	if (CheckAttribute(chr, "fortDefender")) return true; // сторожевики форта/колонии (islandships.c) - на службе, не торгуются
	if (CheckAttribute(chr, "PearlTartane")) return true; // LEO: квест-тартаны ловцов жемчуга (Sharp) - дают жемчуг при СБЛИЖЕНИИ (Ship_CS_PearlOrAttack), переговоров/сдачи в море у них НЕТ (юзер: запретить вообще)
	if (Speak_IsPGGHimself(chr)) return true; // сам ПГГ бьётся до конца - его энкаунтер откупом с глобалки не снять
	if (CheckAttribute(chr, "cannotsurrender")) return true;
	if (CheckAttribute(chr, "DontRansackCaptain")) return true;
	// корабли на квестовых шаблонах (QuestShip в Ships_init.c / GenerateShipTop) - сюжетные,
	// их судьбу решают квесты: откуп/контрибуция им тоже недоступны
	if (CheckAttribute(chr, "ship.type"))
	{
		int t = sti(chr.ship.type);
		if (t != SHIP_NOTUSED)
		{
			ref rShp = GetRealShip(t);
			if (CheckAttribute(rShp, "QuestShip")) return true;
		}
	}
	return false;
}

// согласится ли враг откупиться контрибуцией - ЧЕСТНО от живой боевой морали.
// Ship_Check_Surrender тут не годится: это механика БЕЛОГО ФЛАГА с анти-сдачными
// гейтами (SURR_GLOBAL_SCL=0.2 и каскад роллов) - даже сломленный (мораль 0) отказывал
// в ~85% попыток, хотя сам же предлагал откуп (инициатива молотит роллы каждый тик).
bool CalculateForRansom(ref rEnemyChar)
{
	if (CheckAttribute(rEnemyChar, "cannotsurrender")) return false;
	if (CheckAttribute(rEnemyChar, "DontRansackCaptain")) return false;
	if (CheckAttribute(rEnemyChar, "AlwaysEnemy")) return false;

	int idx = GetCharacterIndex(rEnemyChar.id);
	if (idx < 0) return false;
	if (!CheckAttribute(rEnemyChar, "seatime.tempmorale"))
	{
		Ship_CheckMorale(idx, true); // мораль ещё не считалась - посчитать сейчас
	}
	if (!CheckAttribute(rEnemyChar, "seatime.tempmorale")) return false;
	float fTemp = stf(rEnemyChar.seatime.tempmorale);
	float fSur = 0.0;
	if (CheckAttribute(rEnemyChar, "seatime.surmorale")) fSur = stf(rEnemyChar.seatime.surmorale);
	if (fSur < 0.01) return false; // о сдаче не помышляет (цел, врага рядом нет и т.п.)

	float fGate = fSur * 3.0;      // откупиться проще, чем сдаться: порог утроенный
	if (fTemp < 5.0) return true;  // дух сломлен - платит без разговоров
	if (fTemp >= fGate) return false;
	// между дном и порогом: шанс тем выше, чем глубже мораль просела
	float fChance = (fGate - fTemp) / fGate;
	if (frnd() < fChance) return true;
	return false;
}

// готов ли корабль сдаться (капитуляция целиком)
bool CalculateForCapture(aref rSpeakCharacter)
{
	if (CheckAttribute(rSpeakCharacter, "cannotsurrender")) return false;
	if (CheckAttribute(rSpeakCharacter, "DontRansackCaptain")) return false;

	return Ship_CheckMoraleFail(GetCharacterIndex(rSpeakCharacter.id), 0.9); // они и так подумывали о сдаче
}

// требование ПОЛНОЙ СДАЧИ под давлением личности: Авторитет (чистый БЕЛЫЙ навык, он же SKILL_LEADERSHIP)
// ПОМНОЖЕННЫЙ на Лидерство (характеристика, она же Харизма / SPECIAL_C) - ГГ против тех же качеств капитана.
// Произведение, а не сумма: капитану нужны И навык командования, И личная харизма, чтобы держать флаг.
// ЛИНЕЙНО с жёстким порогом: если воля врага НЕ меньше нашей - не сдаётся ВООБЩЕ; если меньше - линейный
// ролл по тому, насколько он слабее (fEn=0 → 100%, fEn=fPC/2 → 50%, fEn→fPC → ~0%). Чистый ролл воли,
// от морали не зависит; зовётся раз в игровой час. true - капитан спустил флаг.
// LEO: ЧИСТАЯ Харизма (Лидерство) - голый атрибут SPECIAL.Charisma напрямую, у ОБЕИХ сторон одинаково.
// НЕ через GetCharacterSPECIALSimple: та для ГГ добавляет ситуативный модификатор здоровья (+GetHealthNum-6),
// раздувая Лидерство ГГ выше чистого. У врага бонусов не бывает - его sti(SPECIAL.Charisma) и есть чистое значение.
int Speak_RawCharisma(ref chr)
{
	if (CheckAttribute(chr, "SPECIAL.Charisma")) return sti(chr.SPECIAL.Charisma);
	return 1; // подстраховка, если атрибут вдруг не задан (без мутаций)
}

bool Speak_TryDemandSurrender(ref chr)
{
	// чистые (белые) значения у обеих сторон: голый навык Лидерства (Авторитет) × голая Харизма (Лидерство), без бонусов/здоровья
	int iPcA = GetCharacterSkillNoBonus(pchar, SKILL_LEADERSHIP);
	int iPcC = Speak_RawCharisma(pchar);
	int iEnA = GetCharacterSkillNoBonus(chr, SKILL_LEADERSHIP);
	int iEnC = Speak_RawCharisma(chr);
	// LEO: воля = Авторитет * Лидерство(Харизма). Считаем в ЦЕЛЫХ: makefloat(iA*iB) (умножение ВНУТРИ makefloat)
	// валит скрипт-функцию, и она аварийно возвращает "истину" -> сдавался КТО УГОДНО. Через int всё работает.
	int iPcW = iPcA * iPcC;
	int iEnW = iEnA * iEnC;
	if (iPcW < 1) iPcW = 1;
	if (iEnW < 0) iEnW = 0;
	if (iEnW >= iPcW) return false;      // враг не слабее по воле - флаг не спустит вообще
	float fChance = makefloat(iPcW - iEnW) / makefloat(iPcW); // линейно: чем слабее враг, тем выше шанс
	if (frnd() < fChance) return true;
	return false;
}

// ---------------------------------------------------------------------------
//  БОЕВАЯ ЦЕННОСТЬ (battle value): вес бортового залпа x готовность корпуса + команда.
//  На ней строятся суммы контрибуции и откупа: платит слабый, диктует сильный.
// ---------------------------------------------------------------------------

int Speak_ShipBV(ref chr)
{
	if (!CheckAttribute(chr, "ship.type")) return 0;
	int t = sti(chr.ship.type);
	if (t == SHIP_NOTUSED) return 0;
	if (LAi_isDead(chr)) return 0;

	int iCannons = GetCannonQuantity(chr);
	int iCaliber = 8;
	if (CheckAttribute(chr, "Ship.Cannons.Type")) iCaliber = GetCannonCaliber(sti(chr.Ship.Cannons.Type));
	if (iCaliber < 1) iCaliber = 1;
	int iHullPct = makeint(GetHullPercent(chr));
	int iCrew = GetCrewQuantity(chr);

	int iBV = makeint(iCannons * iCaliber * iHullPct / 100) + iCrew / 2;
	if (iBV < 1) iBV = 1;
	return iBV;
}

// сила эскадры ГГ: флагман + все компаньоны (конвой тоже строй держит)
int Speak_PlayerBV()
{
	int iBV = Speak_ShipBV(pchar);
	int i, cn;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if (cn < 0) continue;
		iBV = iBV + Speak_ShipBV(&Characters[cn]);
	}
	return iBV;
}

// сила эскадры собеседника: вся его морская группа (либо одиночка)
int Speak_EnemyBV(ref chr)
{
	int iBV = 0;
	int i, cn;
	bool bGroupFound = false;
	if (CheckAttribute(chr, "SeaAI.Group.Name"))
	{
		string sGroup = chr.SeaAI.Group.Name;
		int iGrp = Group_FindGroup(sGroup);
		if (iGrp >= 0)
		{
			ref rGroup = Group_GetGroupByIndex(iGrp);
			bGroupFound = true;
			i = 0;
			while (true)
			{
				cn = Group_GetCharacterIndexR(rGroup, i);
				if (cn < 0) break;
				iBV = iBV + Speak_ShipBV(&Characters[cn]);
				i = i + 1;
			}
		}
	}
	if (!bGroupFound) iBV = Speak_ShipBV(chr);
	if (iBV < 1) iBV = 1;
	return iBV;
}

// штатная МОЩЬ эскадры собеседника по классам кораблей (ClassPowerCalculate,
// worldmap_globals.c: 7 кл = 1 ... 1 кл = 100); считаем только живых
int Speak_GroupClassPower(ref chr)
{
	int iPower = 0;
	int i, cn;
	bool bFound = false;
	if (CheckAttribute(chr, "SeaAI.Group.Name"))
	{
		int iGrp = Group_FindGroup(chr.SeaAI.Group.Name);
		if (iGrp >= 0)
		{
			ref rGroup = Group_GetGroupByIndex(iGrp);
			bFound = true;
			i = 0;
			while (true)
			{
				cn = Group_GetCharacterIndexR(rGroup, i);
				if (cn < 0) break;
				if (!LAi_isDead(&Characters[cn])) iPower = iPower + ClassPowerCalculate(GetCharacterShipClass(&Characters[cn]));
				i = i + 1;
			}
		}
	}
	if (!bFound) iPower = ClassPowerCalculate(GetCharacterShipClass(chr));
	return iPower;
}

// доля силы ГГ в стычке: 0..100 (50 - силы равны)
int Speak_ForcePercent(ref chr)
{
	int iOur = Speak_PlayerBV();
	int iEnemy = Speak_EnemyBV(chr);
	return makeint(iOur * 100 / (iOur + iEnemy));
}

// сумма контрибуции (платит ОН): доля его казны по соотношению сил + красноречие Коммерции.
// Равные силы ~30%, тотальное превосходство ГГ - до 75%, слабому много не выжать.
int CalculateSumm(aref chr)
{
	int iMoney = sti(chr.money);
	if (iMoney < 1500) // у фантомов казна пуста - кубышка капитана по его рангу и классу корабля
	{
		ref rEnShip = GetRealShip(sti(chr.ship.type));
		int iEnClass = sti(rEnShip.Class);
		iMoney = 2000 + rand(2000) + sti(chr.rank) * 500 + (8 - iEnClass) * (8 - iEnClass) * 800;
		if (rand(99) < 5) iMoney = iMoney + 10000 + rand(15000); // редкий куш
		chr.money = iMoney;
	}
	int iForce = Speak_ForcePercent(chr); // наша доля силы 0..100
	float fShare = (iForce - 20) * 0.01;  // 50 -> 0.30, 80 -> 0.60, 95 -> 0.75
	if (fShare < 0.05) fShare = 0.05;
	if (fShare > 0.75) fShare = 0.75;
	int iCommerce = GetSummonSkillFromName(pchar, SKILL_COMMERCE);
	fShare = fShare * (1.0 + iCommerce * 0.005); // до +50% за Коммерцию
	if (fShare > 0.9) fShare = 0.9;

	int iPrice = makeint(iMoney * fShare);
	if (iPrice < 100) iPrice = 100;
	if (iPrice > iMoney) iPrice = iMoney; // больше казны не выжать
	return iPrice;
}

// LEO: сумма контрибуции, когда враг САМ предлагает откупиться (сломлен, путь sGlobalMode=="ransom"). Считаем от
//   ЕГО СОБСТВЕННОГО положения (казна + отчаяние: потрёпанность корпуса + просадка морали), а НЕ от соотношения сил
//   ЭСКАДР (как CalculateSumm). Иначе сильная его группа занижала долю в пол 0.05 -> тонущий одиночка предлагал
//   смешные 100-200 (жалоба юзера). Тонущему терять нечего -> платит МНОГО, независимо от силы его группы.
int CalculateSummSelf(aref chr)
{
	ref rEnShip = GetRealShip(sti(chr.ship.type));
	int iEnClass = sti(rEnShip.Class);
	int iMoney = sti(chr.money);
	if (iMoney < 1500) // казна фантома пуста - кубышка по рангу и классу (как в CalculateSumm)
	{
		iMoney = 2000 + rand(2000) + sti(chr.rank) * 500 + (8 - iEnClass) * (8 - iEnClass) * 800;
		if (rand(99) < 5) iMoney = iMoney + 10000 + rand(15000); // редкий куш
		chr.money = iMoney;
	}
	// доля от ОТЧАЯНИЯ: потрёпанность корпуса (0 цел .. 1 при 0% корпуса) + просадка боевой морали (0 при 50+ .. 1 при 0)
	float fHullDmg = (100.0 - GetHullPercent(chr)) * 0.01;
	if (fHullDmg < 0.0) fHullDmg = 0.0;
	if (fHullDmg > 1.0) fHullDmg = 1.0;
	float fMorLow = 0.0;
	if (CheckAttribute(chr, "seatime.tempmorale"))
	{
		float fMor = stf(chr.seatime.tempmorale);
		fMorLow = (50.0 - fMor) * 0.02;
		if (fMorLow < 0.0) fMorLow = 0.0;
		if (fMorLow > 1.0) fMorLow = 1.0;
	}
	// база 0.25 + потрёпанность до +0.35 + просадка морали до +0.15 -> итог 0.25..0.75 (тонущий платит щедро)
	float fShare = 0.25 + fHullDmg * 0.35 + fMorLow * 0.15;
	if (fShare < 0.25) fShare = 0.25;
	if (fShare > 0.85) fShare = 0.85;
	int iPrice = makeint(iMoney * fShare);
	// абсолютный пол по рангу/классу - чтоб даже мелочь не предлагала гроши
	int iFloor = 500 + sti(chr.rank) * 300 + (8 - iEnClass) * (8 - iEnClass) * 250;
	if (iPrice < iFloor) iPrice = iFloor;
	if (iPrice > iMoney) iPrice = iMoney; // больше казны не выжать
	return iPrice;
}

// сумма откупа (платим МЫ): ЧИСТО по боевой ценности обеих сторон. База - сила эскадры
// противника (слабые торговцы много не запросят, военная эскадра - дорого), множитель по
// соотношению сил: x0.5 при нашем превосходстве, до x4 когда нас давят.
int CalculateTax(aref chr)
{
	int iEnemyBV = Speak_EnemyBV(chr);
	// эскадра ПГГ: сам ПГГ в сделку не входит (с ним сам на сам) - его силу из суммы вычитаем
	int iPGGIdx = Speak_GroupPGGIndex(chr);
	if (iPGGIdx >= 0) iEnemyBV = iEnemyBV - Speak_ShipBV(&Characters[iPGGIdx]);
	if (iEnemyBV < 1) iEnemyBV = 1;

	int iForce = Speak_ForcePercent(chr); // наша доля силы 0..100
	float fMult = (100 - iForce) * 0.04;  // 50 -> 2.0, 80 -> 0.8, 20 -> 3.2
	if (fMult < 0.5) fMult = 0.5;
	if (fMult > 4.0) fMult = 4.0;

	int iTax = makeint(iEnemyBV * 12 * fMult);
	if (iTax < 300) iTax = 300;

	// за голову с наградой запросят и её цену
	string sNation = GetNationNameByType(sti(chr.nation));
	if (CheckAttribute(pchar, "bounty." + sNation + ".money"))
	{
		if (sti(pchar.bounty.(sNation).money) > 0)
		{
			iTax = iTax + sti(pchar.bounty.(sNation).money);
		}
	}
	return iTax;
}

// откуп ТОЛЬКО от этого корабля: его собственная боевая ценность, но множитель сил -
// ЭСКАДРЕННЫЙ (корабль торгуется как часть эскадры). Награда за голову сюда не входит -
// одним кораблём голову не выкупить
int CalculateTaxShip(aref chr)
{
	int iShipBV = Speak_ShipBV(chr);

	int iForce = Speak_ForcePercent(chr);
	float fMult = (100 - iForce) * 0.04;
	if (fMult < 0.5) fMult = 0.5;
	if (fMult > 4.0) fMult = 4.0;

	int iTax = makeint(iShipBV * 12 * fMult);
	if (iTax < 300) iTax = 300;
	return iTax;
}

// ---------------------------------------------------------------------------
//  ПЕРЕМИРИЕ ПОСЛЕ СДЕЛКИ: группа собеседника час "не обижается" на шальные
//  выстрелы (движковый флаг ShipEnemyDisable, AIShip.c). Снимает SpeakTruce_End
//  (реакция в quests_reaction.c по таймеру на +1 час игрового времени).
// ---------------------------------------------------------------------------

// участник перемирия: НЕЙТРАЛИТЕТ до выхода на глобалку. ДЕРЖИМ ДАННЫМИ, а не отношениями:
// флаг SpeakTruce периодика ИИ (Ship_CheckSituation) видит каждый пересчёт и заново ставит
// RELATION_NEUTRAL против ЛЮБЫХ пересчётов (UpdateRelations, распознавание флага, "пираты
// нападают всегда"). AlwaysEnemy на время снимается (вернём по истечении), шальные выстрелы
// прощает ShipEnemyDisable (Ship_FireAction), абордаж выключен, урон от ГГ/компаньонов
// обнулён (SpeakTruce_HitImmune).
void SpeakTruce_MarkShip(int cn, int n)
{
	ref b = &Characters[cn];
	string aName = "c" + n;
	pchar.questTemp.SpeakTruce.(aName) = b.id;
	if (CheckAttribute(b, "AlwaysEnemy"))
	{
		aName = "ae" + n;
		pchar.questTemp.SpeakTruce.(aName) = true;
		DeleteAttribute(b, "AlwaysEnemy");
	}
	b.ShipEnemyDisable = true;
	b.SpeakTruce = true; // нейтралитет (Ship_CheckSituation) + иммунитет к снарядам ГГ/компаньонов (SpeakTruce_HitImmune)
	b.Abordage.Enable = false;
	b.Ship.LastBallCharacter = -1; // забыть, кто бил - иначе тактика ИИ вернётся мстить
	SetCharacterRelationBoth(cn, GetMainCharacterIndex(), RELATION_NEUTRAL);
	// и компаньонам нейтралитет: иначе они продолжают числить его врагом (палят сами,
	// а для него движковый "ближайший враг" - компаньон, и в дрейф он не ляжет)
	int k, ccn;
	for (k = 1; k < COMPANION_MAX; k++)
	{
		ccn = GetCompanionIndex(pchar, k);
		if (ccn < 0) continue;
		SetCharacterRelationBoth(cn, ccn, RELATION_NEUTRAL);
	}
}

// Полный иммунитет перемирия: снаряды (и таран) ГГ и его компаньонов кораблю под флагом
// SpeakTruce урона НЕ наносят - иначе хитрец топил бы "не обижающийся" корабль и собирал
// груз с утопленника. Чужие ядра (другие враги, форт) бьют как обычно.
bool SpeakTruce_HitImmune(aref rTarget, int iBallChrIndex)
{
	if (!CheckAttribute(rTarget, "SpeakTruce")) return false;
	if (iBallChrIndex < 0) return false;
	if (iBallChrIndex == GetMainCharacterIndex()) return true;
	if (IsCompanion(&Characters[iBallChrIndex])) return true;
	return false;
}

// добавить корабль в действующее перемирие (список НЕ сбрасывается - корабли из разных
// сделок копятся, пока всё не снимет SpeakTruce_End на выходе на глобалку)
void SpeakTruce_AddShip(int cn)
{
	if (cn < 0) return;
	if (CheckAttribute(&Characters[cn], "SpeakTruce")) return; // уже в перемирии
	int n = 0;
	if (CheckAttribute(pchar, "questTemp.SpeakTruce.count")) n = sti(pchar.questTemp.SpeakTruce.count);
	SpeakTruce_MarkShip(cn, n);
	pchar.questTemp.SpeakTruce.count = n + 1;

	// перемирие живёт, пока море не очистится: снимаем при ВЫХОДЕ НА ГЛОБАЛКУ.
	// Если корабль тем временем потопят ДРУГИЕ враги - снимать нечего, мёртвым флаги не мешают
	DeleteAttribute(pchar, "quest.SpeakTruce_End"); // от прошлого перемирия мог остаться мёртвый (over) квест
	pchar.quest.SpeakTruce_End.win_condition.l1 = "MapEnter";
	pchar.quest.SpeakTruce_End.win_condition = "SpeakTruce_End";
}

// Ship_Neutral замирил корабль со ВСЕМИ врагами подряд - возвращаем вражду ТРЕТЬИМ
// сторонам по нац-матрице (мир куплен только у ГГ и компаньонов; стычки наций/пиратов
// и форты - его прежняя забота, иначе пират в дрейфе игнорировал нацию-врага)
void SpeakTruce_RestoreThirdPartyFoes(int cn)
{
	ref b = &Characters[cn];
	int iMyNation = sti(b.nation);
	int i, sIdx;
	for (i = 0; i < iNumShips; i++)
	{
		sIdx = Ships[i];
		if (sIdx < 0) continue;
		if (sIdx == cn) continue;
		if (sIdx == nMainCharacterIndex) continue;
		ref s = &Characters[sIdx];
		if (IsCompanion(s)) continue;
		if (LAi_isDead(s)) continue;
		// собратьев по перемирию НЕ пропускаем: перемирие у каждого С ИГРОКОМ, а не между
		// собой - откупившиеся пираты и испанцы продолжают грызться (юзер словил обратное)
		if (GetNationRelation(iMyNation, sti(s.nation)) == RELATION_ENEMY)
		{
			SetCharacterRelationBoth(cn, sIdx, RELATION_ENEMY);
		}
	}
	// форты: вражда по нации тоже возвращается - форт лупит откупленного, пират он или кто угодно
	if (CheckAttribute(pchar, "seaAI.update.forts"))
	{
		aref rForts;
		makearef(rForts, pchar.seaAI.update.forts);
		int nForts = GetAttributesNum(rForts);
		string sF;
		int iFortIdx;
		for (i = 0; i < nForts; i++)
		{
			sF = "l" + i;
			if (!CheckAttribute(rForts, sF + ".idx")) continue;
			iFortIdx = sti(rForts.(sF).idx);
			if (iFortIdx < 0) continue;
			if (GetNationRelation(iMyNation, sti(Characters[iFortIdx].nation)) == RELATION_ENEMY)
			{
				SetCharacterRelationBoth(cn, iFortIdx, RELATION_ENEMY);
			}
		}
	}
}

// ближайший живой вражеский КОРАБЛЬ для chr в радиусе fMaxDist (ГГ и компаньоны не в счёт).
// Движок в SeaAI.Update.Situation пишет только дистанцию (без индекса) - ищем сами.
// Используется поведением "сдавшегося" (Ship_CheckSituation): дрейф либо бой с третьей стороной
int Speak_FindNearestFoe(aref chr, float fMaxDist)
{
	int cn = sti(chr.index);
	int i, sIdx;
	int iBest = -1;
	float fBest = fMaxDist;
	float fDist;
	for (i = 0; i < iNumShips; i++)
	{
		sIdx = Ships[i];
		if (sIdx < 0) continue;
		if (sIdx == cn) continue;
		if (sIdx == nMainCharacterIndex) continue;
		ref s = &Characters[sIdx];
		if (LAi_isDead(s)) continue;
		if (IsCompanion(s)) continue;
		if (GetRelation(cn, sIdx) != RELATION_ENEMY) continue;
		fDist = Ship_GetDistance2D(chr, s);
		if (fDist < fBest)
		{
			fBest = fDist;
			iBest = sIdx;
		}
	}
	return iBest;
}

// LEO: при откупе/контрибуции корабль выходит из боя (Ship_Neutral), но движок НЕ сбрасывает
// задачу у его АТАКУЮЩИХ (третьих сторон) на ставшую нейтральной цель - только на МЁРТВУЮ
// (AIShip.c:1352). Атаковавший/грэпивший откупленного зависает: паруса 0, грэпл-подход, стоит
// на месте. Будим их: переназначаем на ближайшего своего врага и поднимаем паруса.
void SpeakTruce_WakeThirdParties(int cn)
{
	int i, sIdx;
	for (i = 0; i < iNumShips; i++)
	{
		sIdx = Ships[i];
		if (sIdx < 0) continue;
		if (sIdx == cn) continue;
		if (sIdx == nMainCharacterIndex) continue;
		ref s = &Characters[sIdx];
		if (LAi_isDead(s)) continue;
		if (IsCompanion(s)) continue;
		if (CheckAttribute(s, "SpeakTruce")) continue;   // сам в перемирии - не трогаем
		if (CheckAttribute(s, "surrendered")) continue;  // сдавшийся - не трогаем
		if (!CheckAttribute(s, "SeaAI.Task.Target")) continue;
		if (s.SeaAI.Task.Target == "") continue;
		if (sti(s.SeaAI.Task.Target) != cn) continue;    // целил именно в нейтрализованного
		int foe = Speak_FindNearestFoe(s, 4000.0);       // ближайший ЕГО враг (ГГ/компаньоны не в счёт)
		if (foe >= 0) Ship_SetTaskAttack(PRIMARY_TASK, sIdx, foe);
		Ship_SetSailState(sIdx, 1.0);                    // снять зависание (паруса вверх в любом случае)
	}
}

// перемирие с ОДНИМ кораблём (контрибуция либо точечный откуп): вынимаем его из боевой
// группы штатным Ship_Neutral (групповой приказ атаки его больше не ведёт) и помечаем.
// Старую группу запоминаем (questTemp.SpeakTruce.grp.*): её рядовые получают право
// САМИ выходить на связь и требовать свой откуп (Speak_EnemyInitiative)
void SpeakTruce_ApplyShip(ref chr)
{
	int cn = GetCharacterIndex(chr.id);
	if (cn < 0) return;
	if (CheckAttribute(chr, "SpeakTruce")) return; // уже в перемирии - повторно через Ship_Neutral не гоняем
	string sOldGroup = "";
	if (CheckAttribute(chr, "SeaAI.Group.Name")) sOldGroup = chr.SeaAI.Group.Name;
	Ship_Neutral(cn, "SpeakTruceShips");
	SpeakTruce_AddShip(cn);
	SpeakTruce_RestoreThirdPartyFoes(cn);
	SpeakTruce_WakeThirdParties(cn); // LEO: разбудить зависших атакующих откупленного
	if (sOldGroup != "") pchar.questTemp.SpeakTruce.grp.(sOldGroup) = true;
}

// перемирие со ВСЕЙ эскадрой собеседника
void SpeakTruce_Apply(ref chr)
{
	int i, cn, n;
	n = 0;

	if (CheckAttribute(chr, "SeaAI.Group.Name"))
	{
		string sGroup = chr.SeaAI.Group.Name;
		int iGrp = Group_FindGroup(sGroup);
		if (iGrp >= 0)
		{
			ref rGroup = Group_GetGroupByIndex(iGrp);
			// эскадра ПГГ: сам ПГГ из сделки исключён - наёмников вынимаем из группы
			// ПООДИНОЧКЕ (Ship_Neutral), а ПГГ остаётся в ней со своей задачей атаки.
			// Сначала собираем индексы: Ship_Neutral меняет состав группы на лету
			if (Speak_GroupPGGIndex(chr) >= 0)
			{
				int aMercs[16];
				int nMercs = 0;
				i = 0;
				while (nMercs < 16)
				{
					cn = Group_GetCharacterIndexR(rGroup, i);
					if (cn < 0) break;
					if (!Speak_IsPGGHimself(&Characters[cn]))
					{
						aMercs[nMercs] = cn;
						nMercs = nMercs + 1;
					}
					i = i + 1;
				}
				for (i = 0; i < nMercs; i++)
				{
					SpeakTruce_ApplyShip(&Characters[aMercs[i]]);
				}
				return;
			}
			i = 0;
			while (true)
			{
				cn = Group_GetCharacterIndexR(rGroup, i);
				if (cn < 0) break;
				SpeakTruce_AddShip(cn);
				n = n + 1;
				i = i + 1;
			}
			// снять с группы залоченную задачу атаки - иначе сами продолжат палить по нам
			Group_UnlockTask(sGroup);
			Group_SetTaskNone(sGroup);
		}
	}
	if (n == 0) // группы нет - хотя бы сам корабль
	{
		SpeakTruce_AddShip(sti(chr.index));
	}
}

void SpeakTruce_End()
{
	if (!CheckAttribute(pchar, "questTemp.SpeakTruce")) return;
	int n = sti(pchar.questTemp.SpeakTruce.count);
	int i, cn;
	string aName;
	for (i = 0; i < n; i++)
	{
		aName = "c" + i;
		if (!CheckAttribute(pchar, "questTemp.SpeakTruce." + aName)) continue;
		cn = GetCharacterIndex(pchar.questTemp.SpeakTruce.(aName));
		if (cn < 0) continue;
		DeleteAttribute(&Characters[cn], "AlwaysFriend");
		DeleteAttribute(&Characters[cn], "ShipEnemyDisable");
		DeleteAttribute(&Characters[cn], "SpeakTruce");
		DeleteAttribute(&Characters[cn], "Abordage.Enable");
		aName = "ae" + i;
		if (CheckAttribute(pchar, "questTemp.SpeakTruce." + aName)) Characters[cn].AlwaysEnemy = true; // вражда возвращается - час вышел
	}
	DeleteAttribute(pchar, "questTemp.SpeakTruce");
}
