//==================================================
// Rumours
//==================================================
//добавить слух о действиях ПГГ
void PGG_AddRumour(ref chr, string _idTmpl)
{
	int iCol;
	//фильтруем действия.. хотя функция AddTemplRumour сейчас защищена от "дурака"
	if (_idTmpl != PGG_TASK_WAITINTAVERN && _idTmpl != PGG_TASK_WORKONMAYOR &&
		_idTmpl != PGG_TASK_SAILTOISLAND && _idTmpl != PGG_TASK_WORKONSTORE &&
		_idTmpl != PGG_TASK_WORKWITHCONTRA) return;

	//нет слухов с пиратских поселений.
	iCol = FindColony(chr.PGGAi.location.town);
	if (iCol != -1 && sti(Colonies[iCol].nation) == PIRATE) return;

	//эти с более низким шансом.
	if (_idTmpl == PGG_TASK_SAILTOISLAND || _idTmpl == PGG_TASK_WORKONSTORE || _idTmpl == PGG_TASK_WAITINTAVERN)
	{
		if (rand(10) != 1) return;
	}

	pchar.questTemp.PGG_Rumour = chr.index;
	AddTemplRumour(_idTmpl, id_counter+1);
	DeleteAttribute(pchar, "questTemp.PGG_Rumour");
}

//---> Иниты шаблонов слухов...
void PGG_OnInit_WaitInTavern(ref rTmpl)
{
	object oPrm;
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));
	oPrm.PsHero_Name = GetFullName(chr);
	oPrm.Town_Name = XI_ConvertString("Colony" + chr.PGGAi.location.town + "Gen");
	if (chr.sex == "woman") ttttstr = LanguageConvertString(LanguageOpenFile("RumourTexts.txt"), rTmpl.curtid+"W");
	ttttstr = GetAssembledString(ttttstr, &oPrm);
}

void PGG_OnInit_WorkOnMayor(ref rTmpl)
{
	object oPrm;
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));

	oPrm.PsHero_Name = GetHeroName(sti(chr.PGGAi.HeroNum));
	if (chr.sex == "woman") ttttstr = LanguageConvertString(LanguageOpenFile("RumourTexts.txt"), rTmpl.curtid+"W");
	ttttstr = GetAssembledString(ttttstr, &oPrm);

	rTmpl.City = chr.PGGAi.location.town;
}

void PGG_OnInit_SailToIsland(ref rTmpl)
{
	object oPrm;
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));
	oPrm.PsHero_Name = GetFullName(chr);
	oPrm.To = "в";
	if (chr.PGGAi.Task.Target == "Tortuga") oPrm.To = "на";
	oPrm.Town_Name = XI_ConvertString("Colony" + chr.PGGAi.Task.Target + "Acc");
	if (chr.sex == "woman") ttttstr = LanguageConvertString(LanguageOpenFile("RumourTexts.txt"), rTmpl.curtid+"W");
	ttttstr = GetAssembledString(ttttstr, &oPrm);

	rTmpl.actualtime = sti(chr.PGGAi.Task.Target.days);
}

void PGG_OnInit_WorkOnStore(ref rTmpl)
{
	int iGood;
	object oPrm;
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));

	iGood = sti(chr.PGGAi.Task.Target.Goods);
	oPrm.PsHero_Name = GetFullName(chr);
	oPrm.Good = XI_ConvertString(Goods[iGood].name + "Gen");
	oPrm.Town_Name = XI_ConvertString("Colony" + chr.PGGAi.Task.Target + "Acc");
	if (chr.sex == "woman") ttttstr = LanguageConvertString(LanguageOpenFile("RumourTexts.txt"), rTmpl.curtid+"W");
	ttttstr = GetAssembledString(ttttstr, &oPrm);

	rTmpl.actualtime = sti(chr.PGGAi.Task.Target.days);
}

void PGG_OnInit_WorkWithContra(ref rTmpl)
{
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));

	rTmpl.City = chr.PGGAi.location.town;
	rTmpl.Var = sti(chr.index);
}

//---> Обработка слухов...
string PGG_Event_WorkWithContra(ref rRum)
{
	int i, iTmp;
	string sSmugglersLocation, sTemp;
	object oPrm;
	ref sld;
	ref chr = GetCharacter(sti(rRum.Var));

	sSmugglersLocation = SelectSmugglingLocation();
	if (!CheckAttribute(pchar, "quest.Contraband.Active") && !CheckAttribute(pchar, "questTemp.PGGContra") && !CheckAttribute(pchar, "GenQuest.contraTravel.active"))
	{
		chr.PGGAi.location.town.back = chr.PGGAi.location.town;
		chr.PGGAi.location.town = "none";
		pchar.questTemp.PGGContra.Loc = sSmugglersLocation;
		pchar.questTemp.PGGContra = chr.index;
		PlaceSmugglersOnShore(sSmugglersLocation);

		ChangeCharacterAddressGroup(chr, sSmugglersLocation, "smugglers", "smugglerload");
		chr.location.from_sea = sSmugglersLocation;

		//подумал я, и решил добавить кампании ;) чтобы веселее было...
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "PGGTmp");
//		LAi_warrior_SetStay(chr, true); //можно убрать, будет шататься по бухте.

		iTmp = 3 + rand(MakeInt(MOD_SKILL_ENEMY_RATE*3*0.4));
		for (i = 0; i < iTmp; i++)
		{
			sTemp = "pirate_" + (rand(9)+1);
			sld = GetCharacter(NPC_GenerateCharacter("Rand_Enemy0" + i, sTemp, "man", "man", 5, PIRATE, 3, true));
			ChangeCharacterAddressGroup(sld, sSmugglersLocation, "goto", "goto" + (5 + i));

			LAi_SetWarriorType(sld);
			LAi_group_MoveCharacter(sld, "PGGTmp");
			LAi_warrior_SetCommander(sld, chr);
			SetCharacterTask_FollowCharacter(sld, chr);

			sld.Dialog.FileName = "Quest\sailors_dialog.c";
		}
		LAi_group_SetRelation("PGGTmp", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
		LAi_group_SetRelation("PGGTmp", "Smugglers_1", LAI_GROUP_FRIEND);

		//фича, можно будет стучать мэру, и с солдатами ломиться на облаву :)
		pchar.questTemp.PGGContra.Know = true;
		pchar.PGG_Contra_Current = chr.id;
		SetTimerCondition("PGGContra_close", 0, 0, 3, false);
		ReOpenQuestHeader("Gen_StrangeInfo");
		AddQuestRecord("Gen_StrangeInfo", "1");
		AddQuestUserData("Gen_StrangeInfo", "sName", GetFullName(chr));
		AddQuestUserData("Gen_StrangeInfo", "sLoc", GetConvertStr(sSmugglersLocation, "LocLables.txt"));
	}

	oPrm.PsHero_Name = GetFullName(chr);
	oPrm.ContraPlace = GetConvertStr(sSmugglersLocation, "LocLables.txt");

	if (chr.sex == "woman") rRum.text = LanguageConvertString(LanguageOpenFile("RumourTexts.txt"), rRum.curtid+"W");

	return GetAssembledString(rRum.text, &oPrm);
}

//Дебаггер...
void PGG_DebugLog(string _sLog)
{
	if (bBettaTestMode && bPGGLogShow)
	{
		Log_SetStringToLog(_sLog);
		trace("PGG: " + GetQuestBookDataDigit() + " " + _sLog);
	}
}

