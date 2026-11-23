// Gregg: вынесено из GenQuests.c (сплит god-файла) — церковные генераторы (Church-квесты).
// #include на исходное место — file-scope глобалы/дефайны остаются в GenQuests.c выше include; поведение идентично.

void Church_GenQuest1_Timer(string sQuest)
{
	if(GetCharacterIndex("ChurchGenQuest1_Cap") != -1) // Если генерили
	{
		ref sld = CharacterFromID("ChurchGenQuest1_Cap");
		ChangeCharacterAddressGroup(sld, "none", "", ""); // Уберем кэпа
	}
	items[FindItem("Bible")].Name = "itmname_bible"; // Вернёт аттрибуты
	BackItemDescribe("Bible");
	DeleteAttribute(items[FindItem("Bible")], "City");
	Group_DeleteGroup("ChurchGenQuest1_CapGroup"); // Потрем шип
	PChar.Quest.Church_GenQuest1_ChangeCapitanLocation.over = "yes";
	PChar.GenQuest.ChurchQuest_1.Complete.Timer = true;
	AddQuestRecordEx(sQuestTitle, "ChurchGenQuest1", "14");
	CloseQuestHeader(sQuestTitle);
}

void Church_GenQuest1_ChangeCapitanLocation(string sQuest)
{
	if(GetCharacterIndex("ChurchGenQuest1_Cap") == -1) return; // Может не быть
	ref sld = CharacterFromID("ChurchGenQuest1_Cap");
	if(!CheckAttribute(PChar, "GenQuest.ChurchQuest_1.CapWaitOnTavern")) // Кэп на судне
	{
		PChar.Quest.Church_GenQuest1_ChangeCapitanLocation.over = "yes";
		sld.DeckDialogNode = "ChurchGenQuest_1_DeckDialog_1";
		return;
	}

	sld.Dialog.Filename = "GenQuests_Dialog.c";
	sld.Dialog.CurrentNode = "ChurchGenQuest_1_DeckDialog_1";
	sld.DeckDialogNode = "ChurchGenQuest_1_DeckDialog_1";
	FreeSitLocator(PChar.GenQuest.ChurchQuest_1.CurPortManColony + "_tavern", "Sit1");
	ChangeCharacterAddressGroup(sld, PChar.GenQuest.ChurchQuest_1.CurPortManColony + "_tavern", "sit", "Sit1");
	LAi_SetSitType(sld);
}

void Church_GenQuest1_DeleteCapitan(string sQuest)
{
	ref sld = CharacterFromID("ChurchGenQuest1_Cap");
	Group_DeleteGroup("ChurchGenQuest1_CapGroup");
	PChar.Quest.Church_GenQuest1_ChangeCapitanLocation.over = "yes";
	sld.LifeDay = 0;
}
//=====================================================================================================================================
// <-- ЦЕРКОВНЫЙ ГЕНЕРАТОР. КВЕСТ № 1
//=====================================================================================================================================

//=====================================================================================================================================
// Warship. ЦЕРКОВНЫЙ ГЕНЕРАТОР КВЕСТ № 2 -->
//=====================================================================================================================================
void Church_GenQuest2_GenerateBandits(string sQuest)
{
	Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_8", "Logs.txt", "#space#", " "));

	ref location = &locations[reload_location_index];
	LAi_LocationFightDisable(location, true);
	string replace = "";
	if (!CheckAttribute(location,"locators.encdetector")) replace = "yes";
	string encGenGroup = LAi_FindRandomLocator("encdetector");
	string sGenTemp = "locators." + encGenGroup;
	string sLocGroup;
	aref arGenAll;
	makearef(arGenAll, location.(sGenTemp));

	int iBanditsCount = LAi_CalculateRaidersQuantity(GetAttributesNum(arGenAll)); //кол-во человек в банде
	if (replace == "")
	{
		if (GetAttributesNum(arGenAll) < 3) iBanditsCount = 1;
	}

	// if (iBanditsCount <= 0) iBanditsCount = 1;
	// if (iBanditsCount >= 4) iBanditsCount = 3 + rand(1);	// Чаще всего так и бывает

	iGenRank = GetCoffDiff(sti(PChar.rank), 1000);
	if (iGenRank < MOD_SKILL_ENEMY_RATE*3) iGenRank = MOD_SKILL_ENEMY_RATE*3;

	PChar.GenQuest.ChurchQuest_2.BanditsCount = iBanditsCount;
	if(LAi_GetLocatorNum("encdetector") > 0) 	sLocGroup = "encdetector";
	else										sLocGroup = "goto";

	for(int i=0; i<iBanditsCount; i++)
	{
		ref rChar = GetCharacter(NPC_GenerateCharacter("Church_GenQuest2_Bandit_" + i, "pirate_" + (rand(9)+1), "man", "man", iGenRank, PIRATE, -1, true));
		if(i == 0) // Главарь банды
		{
			rChar.SaveItemsForDead = true; // сохранять на трупе вещи
			rChar.DontClearDead = true;
		}
		SetFantomParamFromRank(rChar, iGenRank, true);
		if (replace == "")
		{
			sGenlocator = GetAttributeName(GetAttributeN(arGenAll, 1));
			ChangeCharacterAddressGroup(rChar, location.id, encGenGroup, sGenlocator);
			PlaceCharacter(rChar, "encdetector", "random_must_be_near");
		}
		else
		{
			ChangeCharacterAddressGroup(rChar, location.id, "goto", "goto1");
			PlaceCharacter(rChar, "goto", "random_must_be_near");
		}
		LAi_SetGuardianType(rChar);
		rChar.Dialog.FileName = "GenQuests_Dialog.c";
		LAi_SetImmortal(rChar, true);
		LAi_SetActorType(rChar);
		if (i == 0) LAi_ActorDialog(rChar, pchar, "", -1, 5);
		else
		{
			LAi_CharacterDisableDialog(rChar);
			LAi_ActorFollow(rChar, CharacterFromID("Church_GenQuest2_Bandit_0"), "", -1);
		}
	}
	PChar.Quest.Church_GenQuest2_TimeIsLeft.over = "yes";
	chrDisableReloadToLocation = true;

//	SetFunctionLocatorCondition("Church_GenQuest2_Dialog_With_Bandits", location.id, sLocGroup, encGenGroup, false);
}

void Church_GenQuest2_Dialog_With_Bandits(string sQuest)
{
	int iBanditsCount = PChar.GenQuest.ChurchQuest_2.BanditsCount;
	for(int i=0; i<iBanditsCount; i++)
	{
		ref rChar = CharacterFromID("Church_GenQuest2_Bandit_" + i);
		LAi_SetActorType(rChar);
		if (rChar.id == "Church_GenQuest2_Bandit_0")
			LAi_ActorDialog(rChar, PChar, "", -1, 5);
		else
			LAi_ActorFollow(rChar, CharacterFromID("Church_GenQuest2_Bandit_0"), "", -1);

	}

	// Завершаем прерывания на время
	PChar.Quest.Church_GenQuest2_TimeIsLeft.over = "yes";
	chrDisableReloadToLocation = true;
}

void Church_GenQuest2_DeadBandits(string sQuest)
{
	if(CheckAttribute(PChar, "GenQuest.ChurchQuest_2.WayOfShortVariant"))
	{
		sQuestTitle = PChar.GenQuest.ChurchQuest_2.QuestTown + "ChurchGenQuest2";
		PChar.GenQuest.ChurchQuest_2.Complete.Short_With_Mon = true;	// Квест выполнен по упрощенному варианту
		Delay_DeleteGroup("Church_GenQuest2_Bandits_Group");
		AddQuestRecordEx(sQuestTitle, "ChurchGenQuest2", "3");
		Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_9", "Logs.txt", "#space#", " "));
	}

	DeleteAttribute(LoadedLocation, "DisableEncounters");
	chrDisableReloadToLocation = false;
}

void Church_GenQuest2_TimeIsLeft(string sQuest) // Не нашли бандюков в джунглях за 1 день
{
	Log_TestInfo(GetConvertStrWithReplace("Variable_GenQuests_10", "Logs.txt", "#space#", " "));

	// Если прерывание отработало, значит бандюки сгенерились, а значит не будет еррора, когда мы будем получать ссылку на них
	if(!CheckAttribute(PChar, "Quest.Church_GenQuest2_GenerateBandits"))
	{
		int iBanditsCount = PChar.GenQuest.ChurchQuest_2.BanditsCount;

		for(int i=0; i<iBanditsCount; i++)
		{
				ref rChar = CharacterFromID("Church_GenQuest2_Bandit_" + i);
				rChar.Dialog.CurrentNode = "Left_Time_Case";
				rChar.LifeDay = 0;
		}
	}

	PChar.Quest.Church_GenQuest2_GenerateBandits.over = "yes";
	PChar.Quest.Church_GenQuest2_Dialog_With_Bandits.over = "yes";
	ChangeCharacterReputation(PChar, -3);
	sQuestTitle = PChar.GenQuest.ChurchQuest_2.QuestTown + "ChurchGenQuest2";
	AddQuestRecordEx(sQuestTitle, "ChurchGenQuest2", "13");
	AddQuestUserData(sQuestTitle, "sSex", GetSexPhrase("","ла"));
	AddQuestUserData(sQuestTitle, "sName", PChar.GenQuest.ChurchQuest_2.QuestTown.PriestName);
	CloseQuestHeader(sQuestTitle);
	if(CheckAttribute(PChar, "GenQuest.ChurchQuest_2.QuestGangJungle")) // Потрем аттрибуты джунглей (короткий вариант прохождения)
	{
		LAi_LocationFightDisable(&locations[FindLocation(PChar.GenQuest.ChurchQuest_2.QuestGangJungle)], false);
		DeleteAttribute(&locations[FindLocation(PChar.GenQuest.ChurchQuest_2.QuestGangJungle)], "DisableEncounters");
	}

	if(CheckAttribute(PChar, "GenQuest.ChurchQuest_2.QuestGangShore")) // Потрем аттрибуты бухты, если есть
	{
		LAi_LocationFightDisable(&locations[FindLocation(PChar.GenQuest.ChurchQuest_2.QuestGangShore)], false);
		DeleteAttribute(&locations[FindLocation(PChar.GenQuest.ChurchQuest_2.QuestGangShore)], "DisableEncounters");
	}
	DeleteAttribute(PCHar, "GenQuest.ChurchQuest_2");	// Трем все аттрибуты по церковному квесту
}

void Church_GenQuest2_BanditsIsEnemies_2_Clear(string sQuest)
{
	Delay_DeleteGroup("Church_GenQuest2_Bandits_Group");
}
//=====================================================================================================================================
// <-- ЦЕРКОВНЫЙ ГЕНЕРАТОР. КВЕСТ № 2
//=====================================================================================================================================

//=====================================================================================================================================
// Церковный квест №4 "Вызволение монаха" - похитители в пещере -->
//=====================================================================================================================================
string ChurchRescue_FindCaveEntrance(string sFromLocation) // ищем локацию CaveEntrance на том же острове, что и церковь
{
	string sIsland = locations[FindLocation(sFromLocation)].islandId;
	int storeArray[MAX_LOCATIONS];
	int howStore = 0;
	int n;
	for(n=0; n<MAX_LOCATIONS; n++)
	{
		if(!CheckAttribute(&locations[n], "islandId") || locations[n].islandId != sIsland) continue;
		if(findsubstr(locations[n].id, "CaveEntrance", 0) == -1) continue;
		if(CheckAttribute(&locations[n], "DisableEncounters") && locations[n].DisableEncounters == true) continue;
		storeArray[howStore] = n;
		howStore++;
	}
	if (howStore == 0) return "none";
	n = storeArray[rand(howStore-1)];
	return locations[n].id;
}

void ChurchRescue_SpawnCaptors(string sQuest) // срабатывает при входе в пещеру
{
	if (CheckAttribute(PChar, "GenQuest.ChurchRescue.Freed")) return; // уже освобождён - не плодим

	ref location = &locations[reload_location_index];
	LAi_LocationFightDisable(location, true);

	int iCaptors = 2 + rand(2); // 2..4 человека
	PChar.GenQuest.ChurchRescue.CaptorsCount = iCaptors;

	iGenRank = GetCoffDiff(sti(PChar.rank), 1000);
	if (iGenRank < MOD_SKILL_ENEMY_RATE*3) iGenRank = MOD_SKILL_ENEMY_RATE*3;

	for(int i=0; i<iCaptors; i++)
	{
		ref rChar = GetCharacter(NPC_GenerateCharacter("ChurchRescue_Captor_" + i, "pirate_" + (rand(9)+1), "man", "man", iGenRank, PIRATE, -1, true));
		if(i == 0) // главарь
		{
			rChar.SaveItemsForDead = true;
			rChar.DontClearDead = true;
		}
		SetFantomParamFromRank(rChar, iGenRank, true);
		ChangeCharacterAddressGroup(rChar, location.id, "goto", "goto1");
		PlaceCharacter(rChar, "goto", "random_must_be_near");
		LAi_SetGuardianType(rChar);
		LAi_SetImmortal(rChar, true);
		LAi_SetActorType(rChar);
		if (i == 0)
		{
			rChar.Dialog.FileName = "ChurchRescue_dialog.c";
			rChar.Dialog.CurrentNode = "ChurchRescue_Captor_Start";
			LAi_ActorDialog(rChar, pchar, "", -1, 5);
		}
		else
		{
			LAi_CharacterDisableDialog(rChar);
			LAi_ActorFollow(rChar, CharacterFromID("ChurchRescue_Captor_0"), "", -1);
		}
	}
	PChar.GenQuest.ChurchRescue.CaptorsInCave = true;
	chrDisableReloadToLocation = true; // не выпускаем из локации до развязки (как ShoreBandit), чтоб триггер был одноразовым без софтлока
}

void ChurchRescue_DeadCaptors(string sQuest) // все похитители убиты - монах освобождён силой
{
	Delay_DeleteGroup("ChurchRescue_Captors_Group");
	LAi_LocationFightDisable(LoadedLocation, false); // вернуть возможность боя в локации
	DeleteAttribute(LoadedLocation, "DisableEncounters");
	chrDisableReloadToLocation = false;
	PChar.GenQuest.ChurchRescue.Freed = "fight";
	DeleteAttribute(PChar, "GenQuest.ChurchRescue.CaptorsInCave");
	DeleteAttribute(PChar, "quest.ChurchRescue_SpawnCaptors"); // снимаем триггер спавна
	sQuestTitle = PChar.GenQuest.ChurchRescue.QuestTown + "ChurchRescue";
	AddQuestRecordEx(sQuestTitle, "ChurchRescue", "2");
	AddQuestUserData(sQuestTitle, "sMonk", PChar.GenQuest.ChurchRescue.MonkName);
	AddQuestUserData(sQuestTitle, "sCity", XI_ConvertString("Colony" + PChar.GenQuest.ChurchRescue.QuestTown + "Gen"));
	ChurchRescue_SpawnMonk(); // освобождённый монах выходит поблагодарить
}

void ChurchRescue_CaptorsEnemies_Clear(string sQuest) // игрок сбежал из боя - чистим группу
{
	Delay_DeleteGroup("ChurchRescue_Captors_Group");
	LAi_LocationFightDisable(LoadedLocation, false);
}

void ChurchRescue_SpawnMonk() // освобождённый монах выходит из пещеры с благодарностью
{
	// ищем локатор устья пещеры на CaveEntrance (reload с label "Cave", напр. reload1_back у модели CaveEnt2)
	string sCaveLocator = "reload2_back";
	ref loc = &locations[FindLocation(PChar.location)];
	/* if (CheckAttribute(loc, "reload"))
	{
		aref arReload; makearef(arReload, loc.reload);
		int rn = GetAttributesNum(arReload);
		for(int k=0; k<rn; k++)
		{
			aref rl = GetAttributeN(arReload, k);
			if(CheckAttribute(rl, "label") && rl.label == "Cave" && CheckAttribute(rl, "name"))
			{
				sCaveLocator = rl.name;
				break;
			}
		}
	} */
	ref rMonk = GetCharacter(NPC_GenerateCharacter("ChurchRescue_Monk", "priest_3", "man", "man", 5, sti(PChar.GenQuest.ChurchRescue.Nation), -1, true));
	rMonk.name = PChar.GenQuest.ChurchRescue.MonkName;
	rMonk.lastname = "";
	ChangeCharacterAddressGroup(rMonk, PChar.location, "reload", sCaveLocator);
	PlaceCharacter(rMonk, "reload", "random_must_be"); // у адресованного локатора (устье пещеры)
	LAi_SetActorType(rMonk);
	LAi_SetImmortal(rMonk, true);
	rMonk.Dialog.FileName = "ChurchRescue_dialog.c";
	rMonk.Dialog.CurrentNode = "ChurchRescue_Monk_Thanks";
	LAi_ActorDialog(rMonk, PChar, "", -1, 3);
}
//=====================================================================================================================================
// <-- Церковный квест №4 "Вызволение монаха"
//=====================================================================================================================================

//=====================================================================================================================================
// Церковный квест №3 "Беглый монах" - послушник прячется в таверне -->
//=====================================================================================================================================
void ChurchRunaway_SpawnMonk(string sQuest) // срабатывает при входе в таверну
{
	if (CheckAttribute(PChar, "GenQuest.ChurchRunaway.Outcome")) return; // судьба уже решена - не плодим
	ref rMonk = GetCharacter(NPC_GenerateCharacter("ChurchRunaway_Monk", "priest_3", "man", "man", 5, sti(PChar.GenQuest.ChurchRunaway.Nation), -1, true));
	rMonk.name = PChar.GenQuest.ChurchRunaway.MonkName;
	rMonk.lastname = "";
	ChangeCharacterAddressGroup(rMonk, locations[reload_location_index].id, "goto", "goto1");
	PlaceCharacter(rMonk, "goto", "random_must_be_near");
	LAi_SetActorType(rMonk);
	LAi_SetImmortal(rMonk, true);
	rMonk.Dialog.FileName = "ChurchRunaway_dialog.c";
	rMonk.Dialog.CurrentNode = "ChurchRunaway_Monk_Start";
	LAi_ActorDialog(rMonk, pchar, "", -1, 5);
	PChar.GenQuest.ChurchRunaway.MonkSpawned = true;
}
//=====================================================================================================================================
// <-- Церковный квест №3 "Беглый монах"
//=====================================================================================================================================

//=====================================================================================================================================
// Церковный квест №2 "Сбор пожертвований" - жертвователи на площади -->
//=====================================================================================================================================
void ChurchDonate_SpawnDonors(string sQuest) // срабатывает при входе на городскую площадь
{
	if (CheckAttribute(PChar, "GenQuest.ChurchDonate.DonorsInTown")) return; // уже спавнили
	if (sti(PChar.GenQuest.ChurchDonate.DonorsDone) >= sti(PChar.GenQuest.ChurchDonate.DonorsTotal)) return; // уже собрано
	ref location = &locations[reload_location_index];
	int iDonors = sti(PChar.GenQuest.ChurchDonate.DonorsTotal);
	if (iDonors <= 0) iDonors = 3;
	for(int i=0; i<iDonors; i++)
	{
		ref rChar = GetCharacter(NPC_GenerateCharacter("ChurchDonate_Donor_" + i, "citiz_" + (rand(11)+1), "man", "man", 5, sti(PChar.GenQuest.ChurchDonate.Nation), -1, true));
		ChangeCharacterAddressGroup(rChar, location.id, "goto", "goto" + (i+1));
		PlaceCharacter(rChar, "goto", "random");
		LAi_SetCitizenType(rChar);
		rChar.Dialog.FileName = "ChurchDonate_dialog.c";
		rChar.Dialog.CurrentNode = "ChurchDonate_Donor_Start";
		rChar.talker = 1; // квест-марка над головой (жёлтый "!") + авто-диалог при приближении ГГ
	}
	PChar.GenQuest.ChurchDonate.DonorsInTown = true;
}