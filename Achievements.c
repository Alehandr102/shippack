// LEO: единые пороги, выдача и локализация достижений без новых глобальных переменных.
// LEO: уровни в pchar.achievements, очки в pchar.ach_points, счётчики в pchar.questTemp.

void AddAchievementPoints(int points) // Даем очки достижений
{
	pchar.ach_points = sti(pchar.ach_points)+points;
	Log_Info(GetConvertStrWithReplace("Variable_CharacterUtilite_5", "Logs.txt", "#space#", " ") + pchar.ach_points);
}

void UnlockAchievement(string ach_name, int level) // Открываем достижениие
{
	int points;

	if(sti(pchar.achievements.(ach_name)) == level) return; // Не даем дважды открыть один и тот же уровень достижения

	if(sti(pchar.achievements.(ach_name)) == 3)
	{
		if((level == 2) || (level == 1)) return;
	}

	if(sti(pchar.achievements.(ach_name)) == 2)
	{
		if(level == 1) return;
	}


	string achievement = GetAchievementName(ach_name, level);

	if(level == 1) points = 25;
	if(level == 2) points = 50;
	if(level == 3) points = 100;

	Log_Info(GetConvertStrWithReplace("Variable_CharacterUtilite_6", "Logs.txt", "#space#", " ") + achievement + "' (+" + points + GetConvertStrWithReplace("Variable_CharacterUtilite_7", "Logs.txt", "#space#", " "));
    PlaySound("interface\AchievementComplite.wav");

	if (ach_name == "Nation_quest_E" || ach_name == "Nation_quest_F" || ach_name == "Nation_quest_H" || ach_name == "Nation_quest_S" || ach_name == "Nation_quest_P" )
	{//какую-то из линеек только что выполнили
		if (CheckAttribute(pchar, "achievements.Nation_quest_P") && sti(pchar.achievements.Nation_quest_P)!=3) DeleteAttribute(pchar, "achievements.Nation_quest_P");
		if (CheckAttribute(pchar, "achievements.Nation_quest_E") && sti(pchar.achievements.Nation_quest_E)!=3) DeleteAttribute(pchar, "achievements.Nation_quest_E");
		if (CheckAttribute(pchar, "achievements.Nation_quest_F") && sti(pchar.achievements.Nation_quest_F)!=3) DeleteAttribute(pchar, "achievements.Nation_quest_F");
		if (CheckAttribute(pchar, "achievements.Nation_quest_H") && sti(pchar.achievements.Nation_quest_H)!=3) DeleteAttribute(pchar, "achievements.Nation_quest_H");
		if (CheckAttribute(pchar, "achievements.Nation_quest_S") && sti(pchar.achievements.Nation_quest_S)!=3) DeleteAttribute(pchar, "achievements.Nation_quest_S");
		pchar.Nation_q_achiev_fixed = true;
	}//<--Qwerry, St. - скрытие других национальных линеек из таблицы достижений

	pchar.achievements.(ach_name) = level;
	AddAchievementPoints(points);
}

// ----------------------------------------------------------------------------
//  ЕДИНЫЙ источник истины по многоуровневым (tiered) достижениям.
//  Ach_Tier(id, lvl) — порог уровня lvl (1..3); 0 => достижение не многоуровневое
//  (бинарное/квестовое) либо не зарегистрировано.
//  Те же значения, что показывает таблица QuestBook (знаменатели "/ N").
// ----------------------------------------------------------------------------
int Ach_Tier(string id, int lvl)
{
	if(id=="rank")          { if(lvl==1) return 10;      if(lvl==2) return 20;      return 35; }
	if(id=="money")         { if(lvl==1) return 1000000; if(lvl==2) return 5000000; return 15000000; }
	if(id=="taxes")         { if(lvl==1) return 10;      if(lvl==2) return 50;      return 100; }
	if(id=="AchVzyatka")    { if(lvl==1) return 10;      if(lvl==2) return 25;      return 50; }
	if(id=="AchTreasure")   { if(lvl==1) return 5;       if(lvl==2) return 10;      return 20; }
	if(id=="AchChest")      { if(lvl==1) return 5;       if(lvl==2) return 15;      return 30; }
	if(id=="kills")         { if(lvl==1) return 50;      if(lvl==2) return 150;     return 500; }
	if(id=="ship_kills")    { if(lvl==1) return 10;      if(lvl==2) return 35;      return 70; }
	if(id=="skel_kills")    { if(lvl==1) return 30;      if(lvl==2) return 75;      return 150; }
	if(id=="poisons")       { if(lvl==1) return 50;      if(lvl==2) return 100;     return 150; }
	if(id=="criticals")     { if(lvl==1) return 100;     if(lvl==2) return 250;     return 500; }
	if(id=="AchBlood")      { if(lvl==1) return 50;      if(lvl==2) return 150;     return 300; }
	if(id=="AchTravma")     { if(lvl==1) return 50;      if(lvl==2) return 150;     return 300; }
	if(id=="AchStanS")      { if(lvl==1) return 50;      if(lvl==2) return 150;     return 300; }
	if(id=="AchStanH")      { if(lvl==1) return 50;      if(lvl==2) return 150;     return 300; }
	if(id=="craft")         { if(lvl==1) return 300;     if(lvl==2) return 500;     return 1000; }
	if(id=="books")         { if(lvl==1) return 3;       if(lvl==2) return 6;       return 10; }
	if(id=="officers")      { if(lvl==1) return 3;       if(lvl==2) return 6;       return 10; }
	if(id=="heal_bottles")  { if(lvl==1) return 50;      if(lvl==2) return 100;     return 200; }
	if(id=="AchFood")       { if(lvl==1) return 50;      if(lvl==2) return 150;     return 300; }
	if(id=="gen_quests")    { if(lvl==1) return 10;      if(lvl==2) return 20;      return 40; }
	if(id=="AchTurnir")     { if(lvl==1) return 1;       if(lvl==2) return 3;       return 7; }
	if(id=="AchDuelyant")   { if(lvl==1) return 5;       if(lvl==2) return 10;      return 20; }
	if(id=="AchShipOrder")  { if(lvl==1) return 5;       if(lvl==2) return 10;      return 15; }
	if(id=="AchShipSearch") { if(lvl==1) return 50;      if(lvl==2) return 95;      return 124; }
	if(id=="AchGoldFleet")  { if(lvl==1) return 1;       if(lvl==2) return 3;       return 7; }
	if(id=="AchSityRobbery"){ if(lvl==1) return 3;       if(lvl==2) return 6;       return 10; }
	if(id=="AchPrisonShooting") { if(lvl==1) return 10; if(lvl==2) return 30; return 50; }
	return 0;
}

// Достижения, у которых уровень выдаётся по СТРОГОМУ равенству значения порогу
// (а не ">="). Исторически так работает только "rank" (== по уровню персонажа).
bool Ach_CmpEq(string id)
{
	if(id=="rank") return true;
	return false;
}

// Имя questTemp-счётчика достижения (для IncAndUnlock).
// "" => счётчик внешний/вычисляемый (rank, money, kills, ship_kills, skel_kills,
// AchSityRobbery) — для них вызывается напрямую CheckAndUnlock с готовым значением.
string Ach_CounterLeaf(string id)
{
	if(id=="taxes")         return "taxescount";
	if(id=="AchVzyatka")    return "bribescount";
	if(id=="AchTreasure")   return "treasurecount";
	if(id=="AchChest")      return "chestcount";
	if(id=="poisons")       return "poisoncount";
	if(id=="criticals")     return "criticalcount";
	if(id=="AchBlood")      return "bloodingcount";
	if(id=="AchTravma")     return "traumacount";
	if(id=="AchStanS")      return "swiftcount";
	if(id=="AchStanH")      return "stuncount";
	if(id=="craft")         return "craftcount";
	if(id=="books")         return "bookcount";
	if(id=="officers")      return "officercount";
	if(id=="heal_bottles")  return "healcount";
	if(id=="AchFood")       return "foodcount";
	if(id=="gen_quests")    return "genquestcount";
	if(id=="AchTurnir")     return "tournamentcount";
	if(id=="AchDuelyant")   return "duelcount";
	if(id=="AchShipOrder")  return "shipordercount";
	if(id=="AchShipSearch") return "shipsearchcount";
	if(id=="AchGoldFleet")  return "GoldFleetNum";
	if(id=="AchPrisonShooting") return "prisonshootingwins";
	return "";
}

// Оценить лестницу порогов для текущего значения счётчика.
// Выдаёт КАЖДЫЙ достигнутый уровень (UnlockAchievement идемпотентен, поэтому
// повтор безопасен; на больших скачках корректно начисляются очки всех тиров).
void CheckAndUnlock(string id, int value)
{
	int t3 = Ach_Tier(id, 3);
	if(t3 == 0) return; // не многоуровневое / не зарегистрировано — молча выходим

	int t1 = Ach_Tier(id, 1);
	int t2 = Ach_Tier(id, 2);

	bool h1 = false; bool h2 = false; bool h3 = false; // только однородные сравнения (без смешанных && / ||)
	if(Ach_CmpEq(id))
	{
		if(value == t1) h1 = true;
		if(value == t2) h2 = true;
		if(value == t3) h3 = true;
	}
	else
	{
		if(value >= t1) h1 = true;
		if(value >= t2) h2 = true;
		if(value >= t3) h3 = true;
	}

	if(h1) UnlockAchievement(id, 1);
	if(h2) UnlockAchievement(id, 2);
	if(h3) UnlockAchievement(id, 3);
}

// Инкремент questTemp-счётчика достижения на add и оценка лестницы.
// Возвращает новое значение счётчика.
int IncAndUnlock(string id, int add)
{
	string leaf = Ach_CounterLeaf(id);
	if(leaf == "") return 0;

	int v = sti(pchar.questTemp.(leaf)) + add;
	pchar.questTemp.(leaf) = v;
	if(add > 0) CheckAndUnlock(id, v);
	return v;
}

// Совместимость: исходная сигнатура сохранена (вызывается из множества мест).
int AchievementsCounter_genquests(int _add)
{
	return IncAndUnlock("gen_quests", _add);
}

// LEO: новые достижения добавляются без сброса старых счётчиков и повторной выдачи приза.
void Ach_EnsureMinigames()
{
	if (!CheckAttribute(pchar,"achievements.AchThrowingChampion")) pchar.achievements.AchThrowingChampion=0;
	if (!CheckAttribute(pchar,"achievements.AchPrisonShooting")) pchar.achievements.AchPrisonShooting=0;
	if (!CheckAttribute(pchar,"questTemp.prisonshootingwins")) pchar.questTemp.prisonshootingwins=0;
	if (CheckAttribute(pchar,"Throwing.league.rewarded"))
	{
		if (sti(pchar.Throwing.league.rewarded) != 0) UnlockAchievement("AchThrowingChampion",3);
	}
	CheckAndUnlock("AchPrisonShooting",sti(pchar.questTemp.prisonshootingwins));
}

// Сумма банковских вкладов игрока с учётом неснятых процентов (для достижения bank_money).
// Вынесено из QuestBook, чтобы прогресс bank_money считался и на уровне 3 (раньше там
// использовалась переменная deposits вне своей области видимости -> всегда показывало 0).
int Ach_BankDeposits()
{
	int deposits = 0;
	if (CheckAttribute(pchar, "Quest.Deposits"))
	{
		aref quests;
		makearef(quests, Characters[GetMainCharacterIndex()].Quest.Deposits);
		int nQuestsNum = GetAttributesNum(quests);
		aref quest;
		ref rNPChar;
		int iTemp = 0;
		for(int n = 0; n < nQuestsNum; n++)
		{
			quest = GetAttributeN(quests, n);
			string sQuestName = GetAttributeName(quest);
			if (CheckAttribute(Pchar, "Quest.Deposits."+sQuestName+".Sum"))
			{
				int iPastMonths = GetPastTime("Month", makeint(Pchar.Quest.Deposits.(sQuestName).StartYear), makeint(Pchar.Quest.Deposits.(sQuestName).StartMonth), makeint(Pchar.Quest.Deposits.(sQuestName).StartDay), makefloat(Pchar.Quest.Deposits.(sQuestName).StartTime), getDataYear(), getDataMonth(), GetDataDay(), GetTime());
				iTemp = makeint(Pchar.Quest.Deposits.(sQuestName).Sum);
				rNPChar = CharacterFromID(sQuestName + "_usurer");
				if (iTemp > sti(rNPChar.UsurerDeposit)*1000) iTemp = sti(rNPChar.UsurerDeposit)*1000;
				iTemp = iTemp/100 * makeint(Pchar.Quest.Deposits.(sQuestName).Interest); //процент с учётом максимальной суммы вклада
				deposits += makeint(Pchar.Quest.Deposits.(sQuestName).Sum) + iTemp*iPastMonths; //учитываем неснятые проценты
			}
		}
	}
	return deposits;
}

// ----------------------------------------------------------------------------
//  Геттеры локализации достижений.
//  Оптимизация: варианты *H принимают УЖЕ открытый хендл языкового файла, что
//  позволяет вызывающему (заполнение таблицы) открыть AchievementsDescr.txt один
//  раз вместо ~210 открытий/закрытий за одно открытие журнала.
//  Старые геттеры сохранены как тонкие обёртки — внешние вызовы не ломаются.
// ----------------------------------------------------------------------------
string GetAchievementNameH(int h, string ach_name, int level)
{
	return LanguageConvertString(h, ach_name + "_" + level);
}

string GetAchievementDescribleH(int h, string ach_name, int level)
{
	return LanguageConvertString(h, ach_name + "_" + level + "_descr");
}

string GetAchievementIconH(int h, string ach_name)
{
	return LanguageConvertString(h, ach_name + "_icon");
}

string GetAchievementName(string ach_name, int level) // Получим имя достижения (Rasteador) (ver 0.3.0)
{
	int lngFileID = LanguageOpenFile("AchievementsDescr.txt");
	string describeStr = GetAchievementNameH(lngFileID, ach_name, level);
	LanguageCloseFile(lngFileID);
	return describeStr;
}

string GetAchievementDescrible(string ach_name, int level) // Получим описание достижения (Rasteador) (ver 0.3.0)
{
	int lngFileID = LanguageOpenFile("AchievementsDescr.txt");
	string describeStr = GetAchievementDescribleH(lngFileID, ach_name, level);
	LanguageCloseFile(lngFileID);
	return describeStr;
}

string GetAchievementIcon(string ach_name) // Получим иконку достижения (Rasteador) (ver 0.3.0)
{
	int lngFileID = LanguageOpenFile("AchievementsDescr.txt");
	string describeStr = GetAchievementIconH(lngFileID, ach_name);
	LanguageCloseFile(lngFileID);
	return describeStr;
}
