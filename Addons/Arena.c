
//-------------------------------------------------------------------------------
// Арена и всё такое...
//-------------------------------------------------------------------------------

// LEO: приз за состязание, взятое со всеми зверями. Предмет берём готовый - "Серебряный ключ"
// заведён в игре, но не был задействован нигде; новый в Items[] сдвинул бы индексы и порвал сейвы.
#define ARENA_CRYPT_KEY		"key_ruins"

// LEO: склепы, в один из которых ложится приз под склепный ключ. Сундук там штатный - private1,
// запертый ключом, которого в игре не существует, так что до сих пор он был недоступен.
string Arena_GetCryptLocation(int iCrypt)
{
	if(iCrypt == 0) return "BasTer_CryptBig1";		// Гваделупа
	if(iCrypt == 1) return "Havana_CryptBig1";		// Куба
	if(iCrypt == 2) return "PortRoyal_CryptBig1";	// Ямайка
	return "Villemstad_CryptBig1";					// Кюрасао
}

// LEO: замок, который стоял на склепе изначально - ключа с таким именем в игре нет,
// так что сундук снова становится наглухо запертым
string Arena_GetCryptIdleKey(int iCrypt)
{
	if(iCrypt == 3) return "key_deadman";		// Кюрасао
	return "key_archy";
}

// LEO: полный ли забег - все пять этапов взяты со зверями. Игрок волен снять любой этап крестиком,
// чтобы дойти до финального босса налегке, но тогда и ключа не будет.
bool Arena_IsFullEtapsRun()
{
	int i;
	string sEtap;

	for(i = 1; i <= 5; i++)
	{
		sEtap = "Etap_" + i;
		if(!CheckAttribute(PChar, "Arena.Etaps." + sEtap)) return false;
		if(PChar.Arena.Etaps.(sEtap) != "YES") return false;
	}
	return true;
}

// LEO: приз за полный забег - ключ от склепа. Какого именно, игроку не говорят: обход четырёх
// кладбищ входит в цену награды, на трёх остальных будет «Нет нужного ключа».
void Arena_GiveCryptKey()
{
	if(!Arena_IsFullEtapsRun()) return;
	if(CheckCharacterItem(PChar, ARENA_CRYPT_KEY)) return;	// прошлый ключ ещё не потрачен

	int i;
	int iLoc;

	// прошлый склеп мог остаться заряженным, если игрок до него не дошёл и лишился ключа: возвращаем
	// ему собственный замок и вычищаем добро, иначе один ключ отпирал бы два сундука, а невостребованный
	// лут копился бы в сейве
	for(i = 0; i < 4; i++)
	{
		iLoc = FindLocation(Arena_GetCryptLocation(i));
		if(iLoc < 0) continue;
		if(!CheckAttribute(&Locations[iLoc], "private1.key")) continue;
		if(Locations[iLoc].private1.key != ARENA_CRYPT_KEY) continue;

		Locations[iLoc].private1.key = Arena_GetCryptIdleKey(i);
		DeleteAttribute(&Locations[iLoc], "private1.items");
		DeleteAttribute(&Locations[iLoc], "private1.opened");
		Locations[iLoc].private1.money = 0;
	}

	string sCrypt = Arena_GetCryptLocation(dRand(3));
	iLoc = FindLocation(sCrypt);
	if(iLoc < 0) return;

	// сундук мог быть вскрыт прошлым ключом - отпираем заново и чистим под свежий приз
	DeleteAttribute(&Locations[iLoc], "private1.opened");
	DeleteAttribute(&Locations[iLoc], "private1.items");
	Locations[iLoc].private1.money = 0;
	Locations[iLoc].private1.key = ARENA_CRYPT_KEY;
	Locations[iLoc].private1.key.delItem = true;

	ChestRollLoot("Chest_quest", sCrypt, "private1");	// набор тот же, что у странного сундука

	GiveItem2Character(PChar, ARENA_CRYPT_KEY);
	Log_InfoColored("yellow", GetConvertStrWithReplace("Variable_Arena_CryptKey", "Logs.txt", "#space#", " "));
}

// LEO: сброс боевых эффектов между раундами. Раунд восстанавливает ХП/энергию, но таймеры травмы,
// кровотока, яда и стана оставались висеть - бойцы выходили на новый бой уже покалеченными.
// Чистим и спутников таймеров: curen (заначка энергии под «резким ударом», без неё энергия останется в
// нуле), backuptype (тип ИИ, спрятанный станом), TraumaQ (стак травмы), накопители тика poison/blooding.hp.
// ВАЖНО: звать ДО SetEnergyToCharacter/LAi_SetCurHPMax и до SetWarriorType/SetPlayerType.
void Arena_ClearFightEffects(ref chr)
{
	DeleteAttribute(chr, "chr_ai.Blooding");
	DeleteAttribute(chr, "chr_ai.poison");
	DeleteAttribute(chr, "chr_ai.Swift");
	DeleteAttribute(chr, "chr_ai.understun");
	DeleteAttribute(chr, "chr_ai.Trauma");
	DeleteAttribute(chr, "chr_ai.TraumaQ");
	DeleteAttribute(chr, "chr_ai.curen");
	DeleteAttribute(chr, "chr_ai.backuptype");
	DeleteAttribute(chr, "poison.hp");
	DeleteAttribute(chr, "blooding.hp");
	UnmarkStuckFXEffect(chr); // и залипшая 3D-метка эффекта, если боец приехал из старого сейва
	// LEO: замок боевого режима ставит стан (MushketStun) и не снимает никто. Стёртого таймера мало:
	//   пока замок взведён, движковая SetFightMode выходит первой же строкой (character.cpp) - боец
	//   выходил на следующий раунд и не мог достать клинок.
	LAi_LockFightMode(chr, false);
}

void GenerateArena()
{
	PChar.Arena.Mode = "none";

	GenerateArenaDuel();
	GenerateArenaEtaps();
	GenerateArenaTournament();
	GenerateArenaOdds();

	int iMoneyView = sti(PChar.rank) * 100 + rand(100);
	if(iMoneyView > 10000) { iMoneyView = 10000 + rand(100); }

	PChar.Arena.ViewCost = iMoneyView;

	PChar.Arena.Mode.NotOdd = false;

	PChar.Arena = "Generate";

	if(!CheckAttribute(PChar, "Statistic.Arena.Count")) 		{ PChar.Statistic.Arena.Count = 0; }
	if(!CheckAttribute(PChar, "Statistic.Arena.LooserCount")) 	{ PChar.Statistic.Arena.LooserCount = 0; }
	if(!CheckAttribute(PChar, "Statistic.Arena.WinnerCount")) 	{ PChar.Statistic.Arena.WinnerCount = 0; }

	if(CheckAttribute(PChar, "quest.ClearArenaTime.over"))
	{
		DeleteAttribute(PChar, "quest.ClearArenaTime.over");
	}

	int iTime = rand(5)+20;
	SetTimerFunction("ClearArenaTime", 0, 0, iTime);
}

//-------------------------------------------------------------------------------
// Дуэли
//-------------------------------------------------------------------------------
void GenerateArenaDuel()
{
	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE) + sti(PChar.rank);

	int iCharacter = 0;

	if(GetCharacterIndex("Arena Duel Characer") == -1)
	{
		iCharacter = NPC_GenerateCharacterIndep("Arena Duel Characer", "officer_"+(rand(42)+1), "man", "man", iRank, PIRATE, -1, true);
	}
	else
	{
		iCharacter = CharacterFromID("Arena Duel Characer");
	}

	Characters[iCharacter].id = "Arena Duel Characer";

	string sSaber = GetSaberForArena("FencingLight");
	PChar.Arena.Duel.Saber.Light = sSaber;

	sSaber = GetSaberForArena("Fencing");
	PChar.Arena.Duel.Saber.Saber = sSaber;

	sSaber = GetSaberForArena("FencingHeavy");
	PChar.Arena.Duel.Saber.Heavy = sSaber;

	string sSkill = GetSkillFromSaber(sSaber);

	int iLeaderShip = sti(PChar.skill.LeaderShip);
	int iFencingLight = sti(PChar.skill.FencingLight) + 30;
	int iFencing = sti(PChar.skill.Fencing) + 30;
	int iFencingHeavy = sti(PChar.skill.FencingHeavy) + 30;

	if(iFencingLight < 30) { iFencingLight = 30; }
	if(iFencingLight > 100) { iFencingLight = 100; }

	if(iFencing < 30) { iFencing = 30; }
	if(iFencing > 100) { iFencing = 100; }

	if(iFencingHeavy < 30) { iFencingHeavy = 30; }
	if(iFencingHeavy > 100) { iFencingHeavy = 100; }

	if(rand(3) > 1)
	{
		iLeaderShip += rand(10)+10;
	}
	else
	{
		iLeaderShip -= rand(5)+10;
	}

	if(iLeaderShip > 100)
	{
		iLeaderShip = 100;
	}

	if(iLeaderShip < 30)
	{
		iLeaderShip = 30;
	}

	InitCharacter(&Characters[iCharacter], iCharacter);
	SetRandSPECIAL(&Characters[iCharacter]);

	Characters[iCharacter].id = "Arena Duel Characer";
	Characters[iCharacter].rank = iRank;
	Characters[iCharacter].skill.FencingLight = iFencingLight;
	Characters[iCharacter].skill.Fencing = iFencing;
	Characters[iCharacter].skill.FencingHeavy = iFencingHeavy;
	Characters[iCharacter].skill.LeaderShip = iLeaderShip;
	Characters[iCharacter].model = "officer_" + (rand(42)+1);
	if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) Characters[iCharacter].model.animation = "man_fast"; // LEO: Превозмогаторам страдать 15.12.2021
	else Characters[iCharacter].model.animation = "man";
	Characters[iCharacter].greeting = "Gr_ArenaMember";

	SetRandomNameToCharacter(&Characters[iCharacter]);
	FaceMaker(&Characters[iCharacter]);

	float fHp = LAi_GetCharacterMaxHP(PChar);
	float fEnergy = LAi_GetCharacterMaxEnergy(PChar);

	int iSkill = GetArenaOddsGreatestFencingSkill(&Characters[iCharacter]) * 10;

	int rand_1 = rand(9)+1;
	int rand_2 = rand(9)+1;
	rand_1 += rand_2;

	float HP = fHp + 250 + (rand(20) * rand_1);
	if(HP < 200) { HP = 200; }

	fEnergy += 10 + rand(5);

	Characters[iCharacter].chr_ai.energy = stf(fEnergy);
	Characters[iCharacter].chr_ai.energyMax = stf(fEnergy);
	SetEnergyToCharacter(&Characters[iCharacter]);
	LAi_SetHP(&Characters[iCharacter], stf(HP), stf(HP));

	int iOdd = GetOddForDuelCharacter(&Characters[iCharacter], sti(Characters[iCharacter].skill.(sSkill)));
	Characters[iCharacter].Odd = iOdd;

    SetSpeciality(&Characters[iCharacter], PerksChars()); // LEO: Характеры 01.12.2021
	SetCharacterPerk(&Characters[iCharacter], "Energaiser");
	if (MOD_SKILL_ENEMY_RATE == 3)
	{
		TakeNItems(&Characters[iCharacter], "BackPack5", 1);
		EquipCharacterbyItem(&Characters[iCharacter], "BackPack5");
		TakeNItems(&Characters[iCharacter], "Food"+(2+rand(3)), 30);
		TakeNItems(&Characters[iCharacter], "Food"+(2+rand(3)), 30);
	}
	else
	{
		TakeNItems(&Characters[iCharacter], "Food"	 + (1+rand(2)), 20);
	}
	LAi_NPC_EquipPerk(&Characters[iCharacter], "fantom"); // LEO: Перки от уровня 01.12.2021
	for (iOdd = 1; iOdd < 5; iOdd++)
	{
		TakeNItems(&Characters[iCharacter], "potion" + iOdd, -100);
	}
	if (LAi_GetCharacterMaxHP(&Characters[iCharacter]) > 500) Characters[iCharacter].chr_ai.hp_max = 500.0;

	if (IsCharacterPerkOn(&Characters[iCharacter], "Ciras"))
	{
		string cirnum;
		switch (rand(4))
		{
			case 0: cirnum = "cirass1"; break;
			case 1: cirnum = "cirass2"; break;
			case 2: cirnum = "cirass3"; break;
			case 3: cirnum = "cirass4"; break;
			case 4: cirnum = "cirass5"; break;
		}
		if (CheckAttribute(&Characters[iCharacter], "HeroModel")) // все, у кого есть что одеть
		{
			switch (cirnum)
			{
				case "cirass1": Characters[iCharacter].model = GetSubStringByNum(Characters[iCharacter].HeroModel, 1); break;
				case "cirass2": Characters[iCharacter].model = GetSubStringByNum(Characters[iCharacter].HeroModel, 2); break;
				case "cirass3": Characters[iCharacter].model = GetSubStringByNum(Characters[iCharacter].HeroModel, 3); break;
				case "cirass4": Characters[iCharacter].model = GetSubStringByNum(Characters[iCharacter].HeroModel, 4); break;
				case "cirass5": Characters[iCharacter].model = GetSubStringByNum(Characters[iCharacter].HeroModel, 5); break;
			}
		}
		Characters[iCharacter].cirassId = Items_FindItemIdx(cirnum);
		Log_TestInfo(GetConvertStrWithReplace("Variable_Arena_1", "Logs.txt", "#space#", " ")+Characters[iCharacter].name+GetConvertStrWithReplace("Variable_Arena_2", "Logs.txt", "#space#", " ")+cirnum);
	}

	int iMinOdd = sti(Characters[iCharacter].Odd) / 2;
	int iMaxOdd = sti(Characters[iCharacter].Odd) * 2;
	if(iMinOdd < 500) { iMinOdd = 500; }

	PChar.Arena.Duel = "Generate";
	PChar.Arena.Duel.Character = iCharacter;
	PChar.Arena.Duel.Saber = sSaber;
	PChar.Arena.Duel.Odd = iMinOdd;
	PChar.Arena.Duel.MinOdd = iMinOdd;
	PChar.Arena.Duel.MaxOdd = iMaxOdd;
	PChar.Arena.Duel.Prize = 0;
	PChar.Arena.Locators = "";

	PChar.Arena.Duel.Count = 0;
	PChar.Arena.Duel.LooserCount = 0;
	PChar.Arena.Duel.WinnerCount = 0;

	PChar.Arena.Duel.BiletCount = 1;
}

void PrepareArenaDuel()
{
	PChar.Arena.Duel.BiletCount = 1;
	ChangeCharacterAddressGroup(CharacterFromID("ArenaBileter"), "FencingTown_ExitTown", "rld", "aloc6");
	LAi_SetActorType(CharacterFromID("ArenaBileter"));
	PChar.Arena.Mode = "Duel";
	PChar.quest.PrepareArenaDuelContinue.win_condition.l1 = "locator";
	PChar.quest.PrepareArenaDuelContinue.win_condition.l1.location = "FencingTown_ExitTown";
	PChar.quest.PrepareArenaDuelContinue.win_condition.l1.locator_group = "rld";
	PChar.quest.PrepareArenaDuelContinue.win_condition.l1.locator = "aloc6";
	PChar.quest.PrepareArenaDuelContinue.function = "PrepareArenaDuelContinue";
}

void PrepareArenaDuelContinue(string _tmp)
{
	Characters[GetCharacterIndex("ArenaBileter")].Dialog.CurrentNode = "First Time";
	LAi_SetActorType(CharacterFromID("ArenaBileter"));
	LAi_ActorDialogNow(CharacterFromID("ArenaBileter"), Pchar, "", -1);
}

void PrepareArenaDuelEnd()
{
	LAi_SetStayType(CharacterFromID("ArenaBileter"));
	LAi_SetStayType(CharacterFromID("FencingTown_Head"));
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_head");
	Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_1";
	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = false;

	Locations[FindLocation("FencingTown_Arena")].locators_radius.item.looser_detector = 2.0;

	PChar.quest.StartArenaDuel.win_condition.l1 = "location";
	PChar.quest.StartArenaDuel.win_condition.l1.location = "FencingTown_Arena";
	PChar.quest.StartArenaDuel.function = "StartArenaDuel";
}

void StartArenaDuel(string _tmp)
{
	InterfaceStates.Buttons.Save.enable = false;

	PChar.Arena = "Start";
	
	SetIntegratedMusketDrawn(pchar, false);

	CreateFireInArena();
	CreateArenaFightPeople();

	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = true;

	ChangeCharacterAddress(CharacterFromID("ArenaBileter"), "None", "");
	Locations[FindLocation(PChar.location)].reload.l1.disable = true;

	int iEnemy = sti(PChar.Arena.Duel.Character);

	string sLocator = "arena_member_" + (rand(7)+1);
	ChangeCharacterAddressGroup(&Characters[iEnemy], "FencingTown_Arena", "goto", sLocator);
	LAi_SetStayType(&Characters[iEnemy]);

	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorWaitDialog(PChar, CharacterFromID("FencingTown_Head"));
	LAi_ActorDialog(CharacterFromID("FencingTown_Head"), PChar, "", 5.0, 1.0);
}

void ArenaDuelTalkWithEnemy()
{
	int iEnemy = sti(PChar.Arena.Duel.Character);
	Characters[iEnemy].Dialog.FileName = "FencingTownHead_dialog.c";
	Characters[iEnemy].Dialog.CurrentNode = "EnemyFightInArena_1";

	LAi_SetActorType(&Characters[iEnemy]);
	LAi_ActorWaitDialog(PChar, &Characters[iEnemy]);
	LAi_ActorDialog(&Characters[iEnemy], PChar, "", 10.0, 1.0);
}

void ArenaDuelStartBattleWithEnemy()
{
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_vip_3");
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorTurnToLocator(CharacterFromID("FencingTown_Head"), "item", "looser_detector");

	LAi_Fade("", "");

	int iEnemy = sti(PChar.Arena.Duel.Character);
	ref chr = &Characters[iEnemy];

	chr.ArenaAction = true;
	PChar.ArenaAction = true;

	Arena_ClearFightEffects(chr); // LEO: на арену не выходят с эффектами, принесёнными с улицы
	Arena_ClearFightEffects(PChar);

	LAi_SetPlayerType(PChar);
	LAi_SetWarriorType(chr);

	string sPlayerLocator, sEnemyLocator;
	if(rand(1) == 1)
	{
		sPlayerLocator = "aloc1"; sEnemyLocator = "aloc3";
	}
	else
	{
		sPlayerLocator = "aloc3"; sEnemyLocator = "aloc1";
	}

	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "rld", sPlayerLocator);
	ChangeCharacterAddressGroup(chr, "FencingTown_Arena", "rld", sEnemyLocator);

	string sSaber = "blade1";

	if(CheckAttribute(PChar, "Arena.Duel.Saber"))
	{
		sSaber = PChar.Arena.Duel.Saber;
	}

	RemoveCharacterEquip(PChar, GUN_ITEM_TYPE);
	RemoveCharacterEquip(PChar, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(chr, GUN_ITEM_TYPE);

    GiveItem2Character(PChar, sSaber);
	EquipCharacterByItem(PChar, sSaber);

    GiveItem2Character(chr, sSaber);
	EquipCharacterByItem(chr, sSaber);

	LAi_SetFightMode(PChar, true);
	LAi_SetFightMode(chr, true);

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetHearRadius("EnemyFight", 100.0);
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);

	PChar.quest.ArenaDuelLooserDetector.win_condition.l1 = "locator";
	PChar.quest.ArenaDuelLooserDetector.win_condition.l1.location = "FencingTown_Arena";
	PChar.quest.ArenaDuelLooserDetector.win_condition.l1.locator_group = "item";
	PChar.quest.ArenaDuelLooserDetector.win_condition.l1.locator = "looser_detector";
	PChar.quest.ArenaDuelLooserDetector.function = "ArenaDuelLooserDetector";
}

void ArenaDuelLooserDetector(string _tmp)
{
	ArenaDuelSetLooser(PChar);
}

void ArenaDuelSetLooser(ref chr)
{
	LAi_SetImmortal(chr, true);

	int iEnemy = sti(PChar.Arena.Duel.Character);
	LAi_SetFightMode(&Characters[iEnemy], false);
	LAi_SetActorType(&Characters[iEnemy]);

	LAi_SetFightMode(PChar, false);
	LAi_SetActorType(PChar);
	if(CheckAttribute(chr, "id") && GetCharacterIndex(chr.id) == nMainCharacterIndex)
	{
		PChar.Arena.Duel.LooserCount = sti(PChar.Arena.Duel.LooserCount) + 1;
	}
	else
	{
		PChar.Arena.Duel.WinnerCount = sti(PChar.Arena.Duel.WinnerCount) + 1;
	}

	LAi_QuestDelay("ArenaDuelCheckNewRound", 3.0);

}

void ArenaDuelCheckNewRound()
{
	PChar.Arena.Duel.Count = sti(PChar.Arena.Duel.Count) + 1;

	if(sti(PChar.Arena.Duel.LooserCount) >= 3)
	{
		ArenaDuelEnd(sti(PChar.Arena.Duel.Count), false);
	}
	else
	{
		if(sti(PChar.Arena.Duel.WinnerCount) >= 3)
		{
			ArenaDuelEnd(sti(PChar.Arena.Duel.Count), true);
		}
		else
		{
			ArenaDuelStartNewRound(sti(PChar.Arena.Duel.Count));
		}
	}
}

void ArenaDuelStartNewRound(int iCount)
{
	LAi_Fade("", "");

	PlayStereoSound("new_round");
	Log_Info((iCount+1) + GetConvertStrWithReplace("Variable_Arena_3", "Logs.txt", "#space#", " "));

	int iEnemy = sti(PChar.Arena.Duel.Character);
	ref chr = &Characters[iEnemy];

	Log_Info(GetConvertStrWithReplace("Variable_Arena_4", "Logs.txt", "#space#", " ") + GetFullName(pchar) + " - " + sti(PChar.Arena.Duel.WinnerCount) + ":" + sti(PChar.Arena.Duel.LooserCount) + " - " + chr.name + " " + chr.lastname);

	LAi_SetImmortal(chr, false);
	LAi_SetImmortal(PChar, false);

	Arena_ClearFightEffects(chr); // LEO: новый раунд - оба выходят чистыми
	Arena_ClearFightEffects(PChar);

	float fEnergy = LAi_GetCharacterMaxEnergy(chr);
	Characters[iEnemy].chr_ai.energy = stf(fEnergy);
	Characters[iEnemy].chr_ai.energyMax = stf(fEnergy);

	LAi_SetCurHPMax(&Characters[iEnemy]);
	SetEnergyToCharacter(&Characters[iEnemy]);

	SetEnergyToCharacter(PChar);
	LAi_SetCurHPMax(PChar);

	LAi_SetPlayerType(PChar);
	LAi_SetWarriorType(chr);

	string sPlayerLocator, sEnemyLocator;
	if(rand(1) == 1)
	{
		sPlayerLocator = "aloc1"; sEnemyLocator = "aloc3";
	}
	else
	{
		sPlayerLocator = "aloc3"; sEnemyLocator = "aloc1";
	}

	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "rld", sPlayerLocator);
	ChangeCharacterAddressGroup(chr, "FencingTown_Arena", "rld", sEnemyLocator);

	LAi_group_MoveCharacter(chr, "EnemyFight");
	LAi_group_SetHearRadius("EnemyFight", 100.0);
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);

	PChar.quest.ArenaDuelLooserDetector.win_condition.l1 = "locator";
	PChar.quest.ArenaDuelLooserDetector.win_condition.l1.location = "FencingTown_Arena";
	PChar.quest.ArenaDuelLooserDetector.win_condition.l1.locator_group = "item";
	PChar.quest.ArenaDuelLooserDetector.win_condition.l1.locator = "looser_detector";
	PChar.quest.ArenaDuelLooserDetector.function = "ArenaDuelLooserDetector";
}

void ArenaDuelEnd(int iCount, bool bWin)
{
	LAi_Fade("", "");

	PChar.Statistic.Arena.Count = sti(PChar.Statistic.Arena.Count) + 1;
	if(!bWin)
	{
		PChar.Statistic.Arena.LooserCount = sti(PChar.Statistic.Arena.LooserCount) + 1;
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Arena_1", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_2", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_3", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Arena_4", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + " "+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_5", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_6", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_7", "AddRumours.txt", "#space#", " "), 5, 1);
	}
	else
	{
		PChar.Statistic.Arena.WinnerCount = sti(PChar.Statistic.Arena.WinnerCount) + 1;
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Arena_8", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_9", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_10", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Arena_11", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + " "+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_12", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_13", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_14", "AddRumours.txt", "#space#", " "), 5, 1);
	}

	PChar.quest.ArenaDuelLooserDetector.over = "yes";
	PChar.Arena.Duel.Win = bWin;

	Arena_ClearFightEffects(PChar); // LEO: с арены уходим чистыми - иначе вылеченный ГГ тут же истекал кровью
	LAi_SetCurHPMax(PChar);
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_head");


	InterfaceStates.Buttons.Save.enable = true;

	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorWaitDialog(PChar, CharacterFromID("FencingTown_Head"));
	LAi_ActorDialog(CharacterFromID("FencingTown_Head"), PChar, "", 5.0, 1.0);

	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "goto", "arena_member_" + (rand(7)+1));
	Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_2";
	Locations[FindLocation("FencingTown_Arena")].locators_radius.item.looser_detector = 0.5;
}

void ArenaDuelTheEnd()
{
	InterfaceStates.Buttons.Save.enable = true;
	LAi_SetPlayerType(PChar);
	LAi_SetImmortal(PChar, false);

	int iEnemy = sti(PChar.Arena.Duel.Character);
	LAi_SetStayType(&Characters[iEnemy]);

	ChangeCharacterAddress(&Characters[iEnemy], "None", "");

	LAi_SetOwnerType(CharacterFromID("FencingTown_Head"));
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Townhall", "goto", "goto2");

	string sSaber = "blade1";

	if(CheckAttribute(PChar, "Arena.Duel.Saber"))
	{
		sSaber = PChar.Arena.Duel.Saber;
	}

	RemoveCharacterEquip(&Characters[iEnemy], BLADE_ITEM_TYPE);
	RemoveCharacterEquip(PChar, BLADE_ITEM_TYPE);
	TakeNItems(&Characters[iEnemy], sSaber, -1);
	TakeNItems(PChar, sSaber, -1);

	DoQuestReloadToLocation("FencingTown_ExitTown", "reload", "reload4", "");

	if(CheckAttribute(PChar, "Arena.Duel"))
	{
		DeleteAttribute(PChar, "Arena.Duel");
	}

	if(CheckAttribute(PChar, "ArenaAction"))
	{
		DeleteAttribute(PChar, "ArenaAction");
	}

	ClearArena("");

	ref member;
	if(GetCharacterIndex("Arena Duel Characer") != -1)
	{
		member = &Characters[GetCharacterIndex("Arena Duel Characer")];
		member.LifeDay = 0;
		Log_TestInfo(GetConvertStrWithReplace("Variable_Arena_5", "Logs.txt", "#space#", " "));
	}
}

//-------------------------------------------------------------------------------
// Этапы
//-------------------------------------------------------------------------------
void GenerateArenaEtaps()
{
	ArenaEtapsGetEtap();

	string sSaber = GetSaberForArena("FencingLight");
	PChar.Arena.Etaps.Saber.Light = sSaber;

	sSaber = GetSaberForArena("Fencing");
	PChar.Arena.Etaps.Saber.Saber = sSaber;

	sSaber = GetSaberForArena("FencingHeavy");
	PChar.Arena.Etaps.Saber.Heavy = sSaber;

	PChar.Arena.Etaps.Prize = 0;
}

void PrepareArenaEtaps()
{
	PChar.Arena.Etaps.BiletCount = 1;
	PChar.Arena.Mode = "Etaps";
	ChangeCharacterAddressGroup(CharacterFromID("ArenaBileter"), "FencingTown_ExitTown", "rld", "aloc6");
	LAi_SetActorType(CharacterFromID("ArenaBileter"));
	PChar.quest.PrepareArenaEtapsContinue.win_condition.l1 = "locator";
	PChar.quest.PrepareArenaEtapsContinue.win_condition.l1.location = "FencingTown_ExitTown";
	PChar.quest.PrepareArenaEtapsContinue.win_condition.l1.locator_group = "rld";
	PChar.quest.PrepareArenaEtapsContinue.win_condition.l1.locator = "aloc6";
	PChar.quest.PrepareArenaEtapsContinue.function = "PrepareArenaEtapsContinue";
}

void PrepareArenaEtapsContinue(string _tmp)
{
	Characters[GetCharacterIndex("ArenaBileter")].Dialog.CurrentNode = "First Time";
	LAi_SetActorType(CharacterFromID("ArenaBileter"));
	LAi_ActorDialogNow(CharacterFromID("ArenaBileter"), Pchar, "", -1);
}

void PrepareArenaEtapsEnd()
{
	LAi_SetStayType(CharacterFromID("ArenaBileter"));
	LAi_SetStayType(CharacterFromID("FencingTown_Head"));
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_head");
	Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_3";
	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = false;

	Locations[FindLocation("FencingTown_Arena")].locators_radius.item.looser_detector = 2.0;

	PChar.quest.StartArenaEtaps.win_condition.l1 = "location";
	PChar.quest.StartArenaEtaps.win_condition.l1.location = "FencingTown_Arena";
	PChar.quest.StartArenaEtaps.function = "StartArenaEtaps";
}

void StartArenaEtaps(string _tmp)
{
	InterfaceStates.Buttons.Save.enable = false;
	PChar.Arena = "Start";
	
	SetIntegratedMusketDrawn(pchar, false);

	CreateFireInArena();
	CreateArenaFightPeople();

	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = true;

	ChangeCharacterAddress(CharacterFromID("ArenaBileter"), "None", "");
	Locations[FindLocation(PChar.location)].reload.l1.disable = true;

	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorWaitDialog(PChar, CharacterFromID("FencingTown_Head"));
	LAi_ActorDialog(CharacterFromID("FencingTown_Head"), PChar, "", 5.0, 1.0);
}

void StartArenaEtapsBattle()
{
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_vip_3");
	LAi_Fade("", "");

	PChar.ArenaEtapsAction = true;

	LAi_SetPlayerType(PChar);

	string sPlayerLocator;
	if(rand(1) == 1)
	{
		sPlayerLocator = "aloc1";
	}
	else
	{
		sPlayerLocator = "aloc3";
	}

	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "rld", sPlayerLocator);
	string sSaber = "blade1";

	if(CheckAttribute(PChar, "Arena.Etaps.Saber"))
	{
		sSaber = PChar.Arena.Etaps.Saber;
	}

	RemoveCharacterEquip(PChar, GUN_ITEM_TYPE);
	RemoveCharacterEquip(PChar, BLADE_ITEM_TYPE);

        GiveItem2Character(PChar, sSaber);
	EquipCharacterByItem(PChar, sSaber);

	LAi_SetFightMode(PChar, true);
	PChar.Arena = "Start";

	ArenaEtapsSetRound(1);
}

void ArenaEtapsSetRound(int iEtap)
{
	string sEtap = "Etap_" + iEtap;

	if(PChar.Arena.Etaps.(sEtap) == "NO")
	{
		ArenaEtapsSetPauseToDoNewRound(iEtap);
		return;
	}

	LAi_SetFightMode(PChar, true);
	PChar.Arena.Etaps.Etap = iEtap;
	Arena_ClearFightEffects(PChar); // LEO: новый этап - ГГ выходит без эффектов с прошлого

	int iQuantity = 0;
	string sType = "";

	iQuantity = sti(PChar.Arena.Etaps.(sEtap).Quantity);
	sType = PChar.Arena.Etaps.(sEtap).Type;
	string sModel = "";
	string sAnimation = "";

	string sQuest = "";
	string sSex = "";
	switch(sType)
	{
		case "crabBig": sModel = "crabBig"; sAnimation = "crabBig"; sSex = "crab"; break;
		case "Skel": sModel = "Skel"; sAnimation = "skeleton"; sSex = "skeleton"; break; // LEO: Ловкач
		case "SkelOld": sModel = "Skel_"; sAnimation = "skeleton"; sSex = "skeleton"; break; // LEO: Ловкач
		case "monkey": sModel = "monkey"; sAnimation = "monkey"; sSex = "monkey"; break;
		case "crabBigKing": sModel = "crabBigKing"; sAnimation = "crabBigKing"; sSex = "crab"; break;
		case "SkelKing": sModel = "skeletcap"; sAnimation = "skeleton_fast"; sSex = "skeleton"; break; // LEO: Ловкач
	}

	int iChar = 0;
	ref chr;
	string sCdn = "";

	if(sType == "Skel")
	{
		sModel = "Skel" + (rand(3)+1);
	}

	if(sType == "SkelOld")
	{
		sModel = "Skel_" + (rand(4)+1);
	}

	if(iQuantity == 1)
	{
		iChar = NPC_GeneratePhantomCharacter("", PIRATE, MAN, 0);
		chr = &Characters[iChar];
		chr.id = "arena_etaps_" + iEtap + "_monster_1";
		chr.model = sModel;
		chr.model.animation = sAnimation;
		chr.sex = sSex;
		SetNewModelToChar(chr);
		ArenaEtapsSetMonsterAttributes(chr, iEtap);

		ChangeCharacterAddressGroup(chr, PChar.location, "goto", "wall_enemy_1");

		LAi_group_MoveCharacter(chr, "EnemyFight");
		LAi_group_SetHearRadius("EnemyFight", 100.0);
		LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
		LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);

		sQuest = "ArenaEtapsPrepareWinEtap_" + iEtap;
		PChar.quest.(sQuest).win_condition.l1 = "NPC_Death";
		PChar.quest.(sQuest).win_condition.l1.character = chr.id;
		PChar.quest.(sQuest).function = "ArenaEtapsPrepareWinEtap";
	}
	else
	{
		for(int i=1; i <= iQuantity; i++)
		{
			if(sType == "Skel")
			{
				sModel = "Skel" + (rand(3)+1);
			}

			if(sType == "SkelOld")
			{
				sModel = "Skel_" + (rand(4)+1);
			}

			iChar = NPC_GeneratePhantomCharacter("", PIRATE, MAN, 0);
			chr = &Characters[iChar];
			chr.id = "arena_etaps_" + iEtap + "_monster_" + i;
			chr.model = sModel;
			chr.model.animation = sAnimation;
			chr.sex = sSex;
			SetNewModelToChar(chr);
			ChangeCharacterAddressGroup(chr, PChar.location, "goto", "wall_enemy_" + i);
			ArenaEtapsSetMonsterAttributes(chr, iEtap);

			if(sModel == "monkey")
			{
				LAi_SetMonkeyTypeNoGroup(chr);
			}

			LAi_group_MoveCharacter(chr, "EnemyFight");

			sCdn = "l" + i;

			sQuest = "ArenaEtapsPrepareWinEtap_" + iEtap;
			PChar.quest.(sQuest).win_condition.(sCdn) = "NPC_Death";
			PChar.quest.(sQuest).win_condition.(sCdn).character = chr.id;
			PChar.quest.(sQuest).function = "ArenaEtapsPrepareWinEtap";
		}

		LAi_group_SetHearRadius("EnemyFight", 100.0);
		LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
		LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	}

	// LEO: подписи над головой расставляет NPCShowName, а зовётся он ИЗ CreateCharacter - то есть по тем,
	//   кто уже стоит в локации. Для ПОСЛЕДНЕГО созданного монстра этого прохода никто не сделает, и
	//   именно он выходил на арену безымянным. Прогоняем ещё раз, когда весь этап на местах.
	if(sti(InterfaceStates.CharCap))
	{
		NPCShowName(sti(InterfaceStates.CharCapShowOff), sti(InterfaceStates.CharCapShowOthers), sti(InterfaceStates.CharCapTargetOfficers));
	}

	PChar.quest.ArenaEtapsLooserDetector.win_condition.l1 = "locator";
	PChar.quest.ArenaEtapsLooserDetector.win_condition.l1.location = "FencingTown_Arena";
	PChar.quest.ArenaEtapsLooserDetector.win_condition.l1.locator_group = "item";
	PChar.quest.ArenaEtapsLooserDetector.win_condition.l1.locator = "looser_detector";
	PChar.quest.ArenaEtapsLooserDetector.function = "ArenaEtapsLooserDetector";
}

void ArenaEtapsLooserDetector(ref chr)
{
	ArenaEtapsSetLooser(PChar);

	int iEtap = 1;
	if(CheckAttribute(PChar, "Arena.Etaps.Etap"))
	{
		iEtap = sti(PChar.Arena.Etaps.Etap);
	}

	ArenaEtapsClearMonster(iEtap);

	string sQuest = "ArenaEtapsPrepareWinEtap_" + iEtap;

	if(CheckAttribute(PChar, "quest." + sQuest))
	{
		DeleteAttribute(PChar, "quest." + sQuest);
	}
}

void ArenaEtapsSetLooser(ref chr)
{
	if(GetCharacterIndex(chr.id) == nMainCharacterIndex)
	{
		LAi_SetImmortal(chr, true);
		LAi_SetFightMode(PChar, false);
		LAi_SetActorType(PChar);
		ArenaEtapsEnd(false);

		int iEtap = 1;
		if(CheckAttribute(PChar, "Arena.Etaps.Etap"))
		{
			iEtap = sti(PChar.Arena.Etaps.Etap);
		}

		string sQuest = "ArenaEtapsPrepareWinEtap_" + iEtap;
		if(CheckAttribute(PChar, "quest." + sQuest))
		{
			DeleteAttribute(PChar, "quest." + sQuest);
		}
	}
}

void ArenaEtapsSetPauseToDoNewRound(int iEtap)
{
	iEtap++;
	ArenaEtapsSetRound(iEtap);
}

void ArenaEtapsPrepareWinEtap(string qName)
{
	LAi_SetFightMode(PChar, false);
	LAi_SetActorType(PChar);

	DoQuestFunctionDelay("ArenaEtapsWinEtap", 3.0);
}

void ArenaEtapsWinEtap(string qName)
{
	LAi_SetPlayerType(PChar);

	LAi_Fade("", "");

	int iEtap = 1;
	if(CheckAttribute(PChar, "Arena.Etaps.Etap"))
	{
		iEtap = sti(PChar.Arena.Etaps.Etap);
	}

	ArenaEtapsClearMonster(iEtap);

	if(iEtap == 5)
	{
		PlayStereoSound("new_round");
		Log_Info(GetConvertStrWithReplace("Variable_Arena_6", "Logs.txt", "#space#", " "));
		ArenaEtapsEnd(true);
		return;
	}

	iEtap++;

	PlayStereoSound("new_round");
	Log_Info(GetConvertStrWithReplace("Variable_Arena_7", "Logs.txt", "#space#", " ") + (iEtap-1) + GetConvertStrWithReplace("Variable_Arena_8", "Logs.txt", "#space#", " "));
	Log_Info(GetConvertStrWithReplace("Variable_Arena_9", "Logs.txt", "#space#", " ") + iEtap + GetConvertStrWithReplace("Variable_Arena_10", "Logs.txt", "#space#", " "));

	ArenaEtapsSetRound(iEtap);
}

void ArenaEtapsEnd(bool bWin)
{
	LAi_Fade("", "");

	PChar.Statistic.Arena.Count = sti(PChar.Statistic.Arena.Count) + 1;
	if(!bWin)
	{
		PChar.Statistic.Arena.LooserCount = sti(PChar.Statistic.Arena.LooserCount) + 1;
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Arena_15", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_16", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_17", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Arena_18", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + "  "+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_19", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_20", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_21", "AddRumours.txt", "#space#", " "), 5, 1);
	}
	else
	{
		Arena_GiveCryptKey();	// LEO: ключ от склепа за забег со всеми зверями
		PChar.Statistic.Arena.WinnerCount = sti(PChar.Statistic.Arena.WinnerCount) + 1;
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Arena_22", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_23", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_24", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Arena_25", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + " "+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_26", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_27", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_28", "AddRumours.txt", "#space#", " "), 5, 1);
	}

	Arena_ClearFightEffects(PChar); // LEO: с арены уходим чистыми (антидот снимал только яд)
	LAi_SetCurHPMax(PChar);
	PChar.Arena.Etaps.Win = bWin;
	PChar.quest.ArenaEtapsLooserDetector.over = "yes";

	InterfaceStates.Buttons.Save.enable = true;
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_head");
	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorWaitDialog(PChar, CharacterFromID("FencingTown_Head"));
	LAi_ActorDialog(CharacterFromID("FencingTown_Head"), PChar, "", 5.0, 1.0);

	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "goto", "arena_member_" + (rand(7)+1));
	Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_4";
	Locations[FindLocation("FencingTown_Arena")].locators_radius.item.looser_detector = 0.5;
}

void ArenaEtapsTheEnd()
{
	InterfaceStates.Buttons.Save.enable = true;
	LAi_SetPlayerType(PChar);
	LAi_SetImmortal(PChar, false);
	LAi_SetOwnerType(CharacterFromID("FencingTown_Head"));
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Townhall", "goto", "goto2");

	string sSaber = "blade1";

	if(CheckAttribute(PChar, "Arena.Etaps.Saber"))
	{
		sSaber = PChar.Arena.Etaps.Saber;
	}

	RemoveCharacterEquip(PChar, BLADE_ITEM_TYPE);
	TakeNItems(PChar, sSaber, -1);

	DoQuestReloadToLocation("FencingTown_ExitTown", "reload", "reload4", "");

	if(CheckAttribute(PChar, "Arena.Etaps"))
	{
		DeleteAttribute(PChar, "Arena.Etaps");
	}

	if(CheckAttribute(PChar, "ArenaEtapsAction"))
	{
		DeleteAttribute(PChar, "ArenaEtapsAction");
	}

	SetEnergyToCharacter(PChar);
	Arena_ClearFightEffects(PChar); // LEO: с арены уходим чистыми (антидот снимал только яд)
	LAi_SetCurHPMax(PChar);

	ClearArena("");
}

void ArenaEtapsClearMonster(int iEtap)
{
	int iQuantity = 0;
	string sMonster = "";
	string sEtap = "";

	sEtap = "Etap_" + iEtap;

	iQuantity = sti(PChar.Arena.Etaps.(sEtap).Quantity);

	if(iQuantity == 1)
	{
		sMonster = "arena_etaps_" + iEtap + "_monster_1";
		ChangeCharacterAddress(CharacterFromID(sMonster), "None", "");
	}
	else
	{
		for(int i=1; i <= iQuantity; i++)
		{
			sMonster = "arena_etaps_" + iEtap + "_monster_" + i;
			ChangeCharacterAddress(CharacterFromID(sMonster), "None", "");
		}
	}
}

void ArenaEtapsSetMonsterAttributes(ref monster, int iEtap)
{
	float HP = LAi_GetCharacterMaxHP(PChar);
	HP += 20 + (iEtap * 10);

	float Energy = LAi_GetCharacterMaxEnergy(PChar);
	monster.chr_ai.energy = stf(Energy);
	monster.chr_ai.energyMax = stf(Energy);

	if(monster.model == "skeletcap" || monster.model == "crabBigKing")
	{
		HP += 500 + rand(500);
	}

	if(HP > 5000)
	{
		HP = 5000;
	}

	LAi_SetHP(monster, HP, HP);
	SetEnergyToCharacter(monster);

	int iFencingLight = sti(PChar.skill.FencingLight) + 20;
	if(iFencingLight < 20) { iFencingLight = 20; }
	if(iFencingLight > 100) { iFencingLight = 100; }

	int iFencing = sti(PChar.skill.Fencing) + 20;
	if(iFencing < 20) { iFencing = 20; }
	if(iFencing > 100) { iFencing = 100; }

	int iFencingHeavy = sti(PChar.skill.FencingHeavy) + 20;
	if(iFencingHeavy < 20) { iFencingHeavy = 20; }
	if(iFencingHeavy > 100) { iFencingHeavy = 100; }

	monster.rank = GetRank(PChar, 5) + MOD_SKILL_ENEMY_RATE*3;
	monster.skill.FencingLight = iFencingLight;
	monster.skill.Fencing = iFencing;
	monster.skill.FencingHeavy = iFencingHeavy;

	SetEnergyToCharacter(PChar);
	LAi_SetCurHPMax(PChar);

	TakeNItems(monster, "potion" + (rand(3)+1), (rand(10)+10));
	TakeNItems(monster, "potion" + (rand(3)+1), (rand(10)+10));
	TakeNItems(monster, "potion" + (rand(3)+1), (rand(10)+10));
	TakeNItems(monster, "Food"	 + (1+rand(2)), 20);

	RemoveCharacterEquip(monster, BLADE_ITEM_TYPE);
	RemoveCharacterEquip(monster, GUN_ITEM_TYPE);

	string sSkeletSaber = GetSaberForArena("Fencing");
	if(monster.model == "crabBig" || monster.model == "crabBigKing" || monster.model == "monkey")
	{
		GiveItem2Character(monster, "monkrab");
		EquipCharacterByItem(monster, "monkrab");
	}
	else
	{
		if(monster.model == "skeletcap")
		{
			sSkeletSaber = PChar.Arena.Etaps.Saber;
		}

		GiveItem2Character(monster, sSkeletSaber);
		EquipCharacterByItem(monster, sSkeletSaber);
	}
}

//-------------------------------------------------------------------------------
// Турнир!
//-------------------------------------------------------------------------------
void GenerateArenaTournament()
{
	int iChar = 0;
	string sChar = "";
	ref chr;

	// LEO: прошлый турнир закрывается этой генерацией. Если игрок на него не пришёл, разыгрываем исход
	//   абстрактно - конкуренты плыли на Доминику не впустую (деньги + слух + счёт побед победителю).
	PGG_SimulateTournament();
	PGG_CloseTournamentForAll();   // прошлый турнир отыгран - гости снимаются со стоянки и разъезжаются
	DeleteAttribute(PChar, "Arena.Tournament.PlayerPlayed");
	// LEO: слоты бойцов НЕ создаются заново - NPC_FindOrCreateCharacter возвращает тот же персонаж, поэтому
	//   следы прошлой подсадки (облик ПГГ, отметка места) переживают перегенерацию. Чистим их до того, как
	//   генератор наполнит слоты: иначе конкурент считает, что он всё ещё в сетке, и повторно не садится.
	PGG_ResetTournamentSeats();
	// LEO: зовём свободных ПГГ на турнир. Они реально плывут на Доминику (задача PGG_TASK_ARENA), и если
	//   успевают к боям - займут места в сетке. Турнир перегенерируется раз в 20-25 дней, времени на дорогу
	//   хватает. Не успел - снимается с турнира (см. PGG_ClearTournament).
	PGG_SendToTournament();

	int iNumPosition = 0;
	string sModel = "";

	int iMoney = 0;
	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE);

	iRank *= 1000;
	iRank *= rand(4)+1;
	iMoney = iRank * 8;
	if(sti(PChar.rank) < 10 && iMoney > 250000) { iMoney = 250000; }

	for(int i=1; i <= 7; i++)
	{
		int iRank2 = GetRank(PChar, 5) + MOD_SKILL_ENEMY_RATE*3;
		if (iMoney >= 1000000 && bHardAnimations) iChar = NPC_GenerateCharacterIndep("Arena_Tournament_Character_" + i, "officer_"+(rand(42)+1), "man", "man_fast", iRank2, PIRATE, -1, true); // LEO: Превозмогаторам страдать 15.12.2021
		else iChar = NPC_GenerateCharacterIndep("Arena_Tournament_Character_" + i, "officer_"+(rand(42)+1), "man", "man", iRank2, PIRATE, -1, true);

		chr = &Characters[iChar];
		DeleteAttribute(chr, "items");
		chr.items = "";
		if (iMoney >= 1000000 || MOD_SKILL_ENEMY_RATE == 3)
		{
			TakeNItems(&Characters[iChar], "BackPack5", 1);
			EquipCharacterbyItem(&Characters[iChar], "BackPack5");
			TakeNItems(&Characters[iChar], "Food"   + (2+rand(3)), 30);
			TakeNItems(&Characters[iChar], "Food"   + (2+rand(3)), 30);
		}
		else
		{
			TakeNItems(&Characters[iChar], "Food"   + (1+rand(2)), 20);
		}
		for (int j = 1; j < 5; j++)
		{
			TakeNItems(&Characters[iChar], "potion" + j, -100);
		}

		sChar = "Arena_Tournament_Character_" + i;

		iNumPosition = GetNumPositionForCharInTournament();
		chr.greeting = "Gr_ArenaMember";

		if (IsCharacterPerkOn(chr, "Ciras") && rand(4)==0) SetFantomWearCirass(chr);

		SetRandomNameToCharacter(chr);

		PChar.Arena.Tournament.(sChar).Position = iNumPosition;
		ArenaTournamentSetPosition(iNumPosition, sChar, "1_2");
		ArenaTournamentSetPosition(iNumPosition, sChar, "final");
	}

	iNumPosition = GetNumPositionForCharInTournament();
	PChar.Arena.Tournament.Blaze.Position = iNumPosition;

	ArenaTournamentSetPosition(iNumPosition, "Blaze", "1_2");
	ArenaTournamentSetPosition(iNumPosition, "Blaze", "final");

	DeleteAttribute(PChar, "Arena.Tournament.Positions");
	DeleteAttribute(PChar, "Arena.Tournament.Models");
	DeleteAttribute(PChar, "Arena.Tournament.PGGSeat");   // LEO: новый турнир - места ПГГ в сетке свободны

	string sSaber = GetSaberForArenaT("FencingLight",1);
	PChar.Arena.Tournament.Saber.Light1 = sSaber;
	sSaber = GetSaberForArenaT("FencingLight",2);
	PChar.Arena.Tournament.Saber.Light2 = sSaber;
	sSaber = GetSaberForArenaT("FencingLight",3);
	PChar.Arena.Tournament.Saber.Light3 = sSaber;

	sSaber = GetSaberForArenaT("Fencing",1);
	PChar.Arena.Tournament.Saber.Saber1 = sSaber;
	sSaber = GetSaberForArenaT("Fencing",2);
	PChar.Arena.Tournament.Saber.Saber2 = sSaber;
	sSaber = GetSaberForArenaT("Fencing",3);
	PChar.Arena.Tournament.Saber.Saber3 = sSaber;

	sSaber = GetSaberForArenaT("FencingHeavy",1);
	PChar.Arena.Tournament.Saber.Heavy1 = sSaber;
	sSaber = GetSaberForArenaT("FencingHeavy",2);
	PChar.Arena.Tournament.Saber.Heavy2 = sSaber;
	sSaber = GetSaberForArenaT("FencingHeavy",3);
	PChar.Arena.Tournament.Saber.Heavy3 = sSaber;

	PChar.Arena.Tournament.Money = iMoney;

	PChar.Arena.Tournament.Count = 0;

	PChar.Arena.Tournament.Temp.Attack = "";
	PChar.Arena.Tournament.Temp.Enemy = "";
	PChar.Arena.Tournament.Temp.Stage = 1;
	PChar.Arena.Tournament.Temp.StageTemp = 1;
	PChar.Arena.Tournament.Temp.Count = 0;
	PChar.Arena.Tournament.Temp.WinnerCount = 0;
	PChar.Arena.Tournament.Temp.LooserCount = 0;

	PChar.Arena.Tournament.InterfacePause = false;
	PChar.Arena.Tournament.Temp.StagePause = false;
	PChar.Arena.Tournament.Temp.InterfaceStage = "1_4";
	PChar.Arena.Tournament.Temp.Winner = "";

	// LEO: сетка готова - сажаем тех, кто уже стоит в бухте Доминики с прошлого раза. Обычно ПГГ садится
	//   в момент парковки, но если турнир перегенерировался, пока он ждал на якоре, места он бы не получил.
	PGG_ReseatParkedTournament();
}

void PrepareArenaTournament()
{
	PChar.Arena.Tournament.BiletCount = 1;
	ChangeCharacterAddressGroup(CharacterFromID("ArenaBileter"), "FencingTown_ExitTown", "rld", "aloc6");
	LAi_SetActorType(CharacterFromID("ArenaBileter"));
	PChar.Arena.Mode = "Tournament";
	PChar.quest.PrepareArenaTournamentContinue.win_condition.l1 = "locator";
	PChar.quest.PrepareArenaTournamentContinue.win_condition.l1.location = "FencingTown_ExitTown";
	PChar.quest.PrepareArenaTournamentContinue.win_condition.l1.locator_group = "rld";
	PChar.quest.PrepareArenaTournamentContinue.win_condition.l1.locator = "aloc6";
	PChar.quest.PrepareArenaTournamentContinue.function = "PrepareArenaTournamentContinue";
}

void PrepareArenaTournamentContinue(string _tmp)
{
	Characters[GetCharacterIndex("ArenaBileter")].Dialog.CurrentNode = "First Time";
	LAi_SetActorType(CharacterFromID("ArenaBileter"));
	LAi_ActorDialogNow(CharacterFromID("ArenaBileter"), Pchar, "", -1);
}

void PrepareArenaTournamentEnd()
{
	LAi_SetStayType(CharacterFromID("ArenaBileter"));
	LAi_SetStayType(CharacterFromID("FencingTown_Head"));
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_head");
	Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_5";
	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = false;

	Locations[FindLocation("FencingTown_Arena")].locators_radius.item.looser_detector = 2.0;

	PChar.quest.StartArenaTournament.win_condition.l1 = "location";
	PChar.quest.StartArenaTournament.win_condition.l1.location = "FencingTown_Arena";
	PChar.quest.StartArenaTournament.function = "StartArenaTournament";
}

void StartArenaTournament(string _tmp)
{
	SetTimeScale(2.0);
	InterfaceStates.Buttons.Save.enable = false;
	PChar.Arena = "Start";
	PChar.Arena.Tournament.PlayerPlayed = true;   // LEO: игрок дерётся сам - фоновую симуляцию не проводим
	
	SetIntegratedMusketDrawn(pchar, false);

	CreateFireInArena();
	CreateArenaFightPeople();

	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = true;

	ChangeCharacterAddress(CharacterFromID("ArenaBileter"), "None", "");
	Locations[FindLocation(PChar.location)].reload.l1.disable = true;

	int iEnemy = -1;
	string sLocator = "arena_member_" + (rand(7)+1);

	for(int e=1; e<=7; e++)
	{
		iEnemy = GetCharacterIndex("Arena_Tournament_Character_" + e);

		if(iEnemy != -1)
		{
			sLocator = "arena_member_" + e;
			ChangeCharacterAddressGroup(&Characters[iEnemy], "FencingTown_Arena", "goto", sLocator);
			// LEO: боец попал в мир - только теперь применяется облик подсаженного ПГГ. Пока персонаж не
			//   загружен, смена модели ни на что не влияет: в сетке стояло имя конкурента, а на арену
			//   выходил прежний статист.
			if (CheckAttribute(&Characters[iEnemy], "ArenaTournament.PGGId"))
			{
				SetNewModelToChar(&Characters[iEnemy]);
			}
			LAi_SetStayType(&Characters[iEnemy]);
		}
	}

	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorWaitDialog(PChar, CharacterFromID("FencingTown_Head"));
	LAi_ActorDialog(CharacterFromID("FencingTown_Head"), PChar, "", 5.0, 1.0);
}

void ArenaTournamentTalkWithRandEnemy()
{
	SetTimeScale(2.0);
	int iEnemy = -1;
	int iRand = rand(6)+1;

	for(int e=1; e<=7; e++)
	{
		iEnemy = GetCharacterIndex("Arena_Tournament_Character_" + e);

		if(iEnemy != -1)
		{
			Characters[iEnemy].Dialog.FileName = "FencingTownHead_dialog.c";
			Characters[iEnemy].Dialog.CurrentNode = "FightInArena_6";
			LAi_SetActorType(&Characters[iEnemy]);
			LAi_ActorTurnToLocator(&Characters[iEnemy], "item", "looser_detector");

			if(iRand == e)
			{
				LAi_ActorWaitDialog(PChar, &Characters[iEnemy]);
				LAi_ActorDialog(&Characters[iEnemy], PChar, "", 5.0, 1.0);
			}
		}
	}
}

void ArenaTournamentGo()
{
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_vip_3");
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorTurnToLocator(CharacterFromID("FencingTown_Head"), "item", "looser_detector");

	PChar.Arena.Tournament.InterfacePause = true;
	PChar.Arena.Tournament.Temp.InterfaceStage = "1_4";
	LAi_SetActorType(PChar);
	DoQuestFunctionDelay("ArenaResultTournament", 3.0);

	ArenaTournamentSetCharacterFromArena(PChar);
	PChar.Arena = "Start";
	
	SetIntegratedMusketDrawn(pchar, false);
}

void ArenaTournamentGoStart(int iStage, int iStageTemp)
{
	ref attack, enemy;

	if(iStage == 1)
	{
		if(iStageTemp != 1)
		{
			ArenaTournamentPrepareInterface();
		}
	}
	else
	{
		ArenaTournamentPrepareInterface();
	}

	LAi_Fade("", "");

	int iChar = 0;

	int iEnemyPos = 0;
	int iAttackPos = 0;

	string sStage = "1_4";
	switch(iStage)
	{
		case 1: sStage = "1_4";
			if(iStageTemp == 1)
			{
				iAttackPos = 1; iEnemyPos = 2;
			}
			else
			{
				if(iStageTemp == 2)
				{
					iAttackPos = 3; iEnemyPos = 4;
				}
				else
				{
					if(iStageTemp == 3)
					{
						iAttackPos = 5; iEnemyPos = 6;
					}
					else
					{
						iAttackPos = 7; iEnemyPos = 8;
					}
				}
			}

		break;
		case 2: sStage = "1_2"; if(iStageTemp == 1) { iAttackPos = 1; iEnemyPos = 2; }else{ iAttackPos = 3; iEnemyPos = 4; } break;
		case 3: sStage = "final"; iAttackPos = 1; iEnemyPos = 2; break;
	}

	string sChar = ArenaTournamentGetCharacterIDFromPos(iAttackPos, sStage);
	iChar = GetCharacterIndex(sChar);
	attack = &Characters[iChar];
	attack.ArenaTournament = true;
	attack.ArenaTournament.Attack = true;

	sChar = ArenaTournamentGetCharacterIDFromPos(iEnemyPos, sStage);
	iChar = GetCharacterIndex(sChar);
	enemy = &Characters[iChar];
	enemy.ArenaTournament = true;
	enemy.ArenaTournament.Enemy = true;

	PChar.Arena.Tournament.Temp.Attack = attack.id;
	PChar.Arena.Tournament.Temp.Enemy = enemy.id;

	PChar.Arena.Tournament.Temp.Count = 0;
	PChar.Arena.Tournament.Temp.WinnerCount = 0;
	PChar.Arena.Tournament.Temp.LooserCount = 0;

	ArenaTournamentSetDetailsForChar(attack, iStage);
	ArenaTournamentSetDetailsForChar(enemy, iStage);

	// LEO: финал против самого игрока - запоминаем соперника. Отношение меняем НЕ здесь, а по исходу
	//   боя (ArenaTournamentEnd): выиграл ГГ - конкурент проникается уважением, проиграл - тот задирает нос.
	//   ⚠️ сравниваем ИНДЕКСЫ, а не id: в сетке игрок фигурирует как "Blaze" (Characters[1]), и равенство
	//   строковых id зависит от того, за какого стартового героя играют.
	if (iStage == 3)
	{
		DeleteAttribute(PChar, "Arena.Tournament.FinalFoe");
		if (sti(attack.index) == GetMainCharacterIndex()) PChar.Arena.Tournament.FinalFoe = enemy.id;
		if (sti(enemy.index)  == GetMainCharacterIndex()) PChar.Arena.Tournament.FinalFoe = attack.id;
	}
	
	if (attack.id != pchar.id && enemy.id != pchar.id) 
	{
		pchar.SkipBattle = true;
		pchar.ArenaAttack = attack.id;
		pchar.ArenaEnemy = enemy.id;
	}
	else pchar.SkipBattle = false;

	Log_Info("" + GetFullName(attack) + " VS " + GetFullName(enemy));
}

void ArenaTournamentStartNewBattle()
{
	LAi_Fade("", "");

	PlayStereoSound("new_round");

	ref attack, enemy;
	string sAttack = PChar.Arena.Tournament.Temp.Attack;
	string sEnemy = PChar.Arena.Tournament.Temp.Enemy;

	attack = &Characters[GetCharacterIndex(sAttack)];
	enemy = &Characters[GetCharacterIndex(sEnemy)];
	
	// LEO: подсаженный конкурент выходит СВОИМИ данными - ранг/навыки/характер/ХП тянем прямо перед боем
	//   (между посадкой в сетку и поединком проходят дни: ПГГ растёт и лечится).
	PGG_SyncTournamentFighter(attack);
	PGG_SyncTournamentFighter(enemy);

	// кап здоровья статистов; ⚠️ на подсаженных ПГГ НЕ распространяется - у них здоровье настоящее
	if (attack.index != pchar.index && !CheckAttribute(attack, "ArenaTournament.PGGId"))
	{
		if (LAi_GetCharacterMaxHP(attack) > 500) attack.chr_ai.hp_max = 500.0;
	}
	if (enemy.index != pchar.index && !CheckAttribute(enemy, "ArenaTournament.PGGId"))
	{
		if (LAi_GetCharacterMaxHP(enemy) > 500) enemy.chr_ai.hp_max = 500.0;
	}

	LAi_SetImmortal(attack, false);
	LAi_SetImmortal(enemy, false);

	Arena_ClearFightEffects(attack); // LEO: новый бой турнира - оба выходят чистыми
	Arena_ClearFightEffects(enemy);

	float fEnergy = LAi_GetCharacterMaxEnergy(attack);
	attack.chr_ai.energy = stf(fEnergy);
	attack.chr_ai.energyMax = stf(fEnergy);
	LAi_SetCurHPMax(attack);
	SetEnergyToCharacter(attack);

	fEnergy = LAi_GetCharacterMaxEnergy(enemy);
	enemy.chr_ai.energy = stf(fEnergy);
	enemy.chr_ai.energyMax = stf(fEnergy);
	LAi_SetCurHPMax(enemy);
	SetEnergyToCharacter(enemy);

	// LEO: облик подсаженного ПГГ применяем ЗДЕСЬ, а не при записи в сетку. Смена модели действует только
	//   на загруженного персонажа, а в момент подсадки бойцы арены ещё не в мире - поэтому в сетке стояло
	//   имя конкурента, а драться выходил прежний статист.
	if (CheckAttribute(attack, "ArenaTournament.PGGId")) SetNewModelToChar(attack);
	if (CheckAttribute(enemy,  "ArenaTournament.PGGId")) SetNewModelToChar(enemy);

	LAi_SetWarriorType(attack);
	LAi_SetWarriorType(enemy);

	if(attack.id == PChar.id)
	{
		LAi_SetPlayerType(attack);
	}
	if(enemy.id == PChar.id)
	{
		LAi_SetPlayerType(enemy);
	}

	float x = -2.26369;
	float y = 18.762;
	float z = -11.5217;

	locCameraToPos(x, y, z, false);

	if(attack.id == PChar.id || enemy.id == PChar.id)
	{
		locCameraFollow();
	}

	string sPlayerLocator, sEnemyLocator;
	if(rand(1) == 1)
	{
		sPlayerLocator = "aloc1"; sEnemyLocator = "aloc3";
	}
	else
	{
		sPlayerLocator = "aloc3"; sEnemyLocator = "aloc1";
	}

	ChangeCharacterAddressGroup(attack, "FencingTown_Arena", "rld", sPlayerLocator);
	ChangeCharacterAddressGroup(enemy, "FencingTown_Arena", "rld", sEnemyLocator);

	LAi_group_MoveCharacter(attack, "ARENA_TOURNAMENT_CHARACTER_1");
	LAi_group_MoveCharacter(enemy, "ARENA_TOURNAMENT_CHARACTER_2");

	LAi_group_SetHearRadius("ARENA_TOURNAMENT_CHARACTER_2", 100.0);
	LAi_group_SetHearRadius("ARENA_TOURNAMENT_CHARACTER_1", 100.0);
	LAi_group_SetRelation("ARENA_TOURNAMENT_CHARACTER_1", "ARENA_TOURNAMENT_CHARACTER_2", LAI_GROUP_ENEMY);
	LAi_group_FightGroups("ARENA_TOURNAMENT_CHARACTER_1", "ARENA_TOURNAMENT_CHARACTER_2", true);

	if(attack.id == PChar.id)
	{
		LAi_group_MoveCharacter(attack, LAI_GROUP_PLAYER);
		LAi_group_SetRelation(LAI_GROUP_PLAYER, "ARENA_TOURNAMENT_CHARACTER_2", LAI_GROUP_ENEMY);
		LAi_group_FightGroups(LAI_GROUP_PLAYER, "ARENA_TOURNAMENT_CHARACTER_2", true);
	}
	if(enemy.id == PChar.id)
	{
		LAi_group_MoveCharacter(enemy, LAI_GROUP_PLAYER);
		LAi_group_SetRelation(LAI_GROUP_PLAYER, "ARENA_TOURNAMENT_CHARACTER_1", LAI_GROUP_ENEMY);
		LAi_group_FightGroups(LAI_GROUP_PLAYER, "ARENA_TOURNAMENT_CHARACTER_1", true);
	}
	if(attack.id != PChar.id && enemy.id != PChar.id) SetTimeScale(6.0);
	else SetTimeScale(1.0);
	if (CheckAttribute(PChar,"ArenaOdds")) SetTimeScale(1.0);
	if(attack.id == PChar.id || enemy.id == PChar.id)
	{
		string sQuest = "AT_LooserDetector_" + sti(PChar.Arena.Tournament.Temp.Stage) + "_" + sti(PChar.Arena.Tournament.Temp.StageTemp);
		PChar.quest.(sQuest).win_condition.l1 = "locator";
		PChar.quest.(sQuest).win_condition.l1.location = "FencingTown_Arena";
		PChar.quest.(sQuest).win_condition.l1.locator_group = "item";
		PChar.quest.(sQuest).win_condition.l1.locator = "looser_detector";
		PChar.quest.(sQuest).function = "ArenaTournamentLooserDetector";
		LAi_SetFightMode(PChar, true);
	}
}

void ArenaTournamentLooserDetector(string qName)
{
	ArenaTournamentSetLooser(PChar);
}

void ArenaTournamentSetLooser(ref chr)
{
	bool bWin = true;

	LAi_SetCurHPMax(chr);
	LAi_SetImmortal(chr, true);

	string sEnemy = PChar.Arena.Tournament.Temp.Enemy;
	string sAttack = PChar.Arena.Tournament.Temp.Attack;

	int iEnemy = GetCharacterIndex(sEnemy);
	int iAttack = GetCharacterIndex(sAttack);

	LAi_SetFightMode(&Characters[iEnemy], false);
	LAi_SetActorType(&Characters[iEnemy]);

	LAi_SetFightMode(&Characters[iAttack], false);
	LAi_SetActorType(&Characters[iAttack]);

	if(CheckAttribute(chr, "ArenaTournament.Attack"))
	{
		PChar.Arena.Tournament.Temp.LooserCount = sti(PChar.Arena.Tournament.Temp.LooserCount) + 1;
		Log_SetStringToLog(GetConvertStrWithReplace("Variable_Arena_11", "Logs.txt", "#space#", " ") + Characters[iEnemy].name + " " + Characters[iEnemy].lastname);
	}
	else
	{
		PChar.Arena.Tournament.Temp.WinnerCount = sti(PChar.Arena.Tournament.Temp.WinnerCount) + 1;
		Log_SetStringToLog(GetConvertStrWithReplace("Variable_Arena_12", "Logs.txt", "#space#", " ") + Characters[iAttack].name + " " + Characters[iAttack].lastname);
	}

	Log_SetStringToLog(GetConvertStrWithReplace("Variable_Arena_13", "Logs.txt", "#space#", " ") + GetFullName(Characters[iAttack]) + " - " + PChar.Arena.Tournament.Temp.WinnerCount + ":" + sti(PChar.Arena.Tournament.Temp.LooserCount) + " - " + Characters[iEnemy].name + " " + Characters[iEnemy].lastname);

	DoQuestFunctionDelay("ArenaTournamentCheckNewRound", 3.0);
}

void ArenaTournamentCheckNewRound(string qName)
{
	PChar.Arena.Tournament.Temp.Count = sti(PChar.Arena.Tournament.Temp.Count) + 1;

	PlayStereoSound("new_round");

	string sEnemy = PChar.Arena.Tournament.Temp.Enemy;
	string sAttack = PChar.Arena.Tournament.Temp.Attack;

	int iEnemy = GetCharacterIndex(sEnemy);
	int iAttack = GetCharacterIndex(sAttack);
	int fightsnum = 3;

	string sStage = "";

	switch(sti(PChar.Arena.Tournament.Temp.Stage))
	{
		case 1: sStage = "1_4"; break;
		case 2: sStage = "1_2"; break;
		case 3: sStage = "final"; break;
	}
	
	if (sStage == "1_4") fightsnum = 1;
	else if (sStage == "1_2") fightsnum = 2;
	else fightsnum = 3;

	if(sti(PChar.Arena.Tournament.Temp.LooserCount) >= fightsnum)
	{
		Characters[iAttack].ArenaTournament.looser.(sStage) = sStage;
		ArenaTournamentDuelEnd(false);
	}
	else
	{
		if(sti(PChar.Arena.Tournament.Temp.WinnerCount) >= fightsnum)
		{
			Characters[iEnemy].ArenaTournament.looser.(sStage) = sStage;
			ArenaTournamentDuelEnd(true);
		}
		else
		{
			ArenaTournamentStartNewBattle();
		}
	}
	
	if (Characters[iAttack].id != pchar.id && Characters[iEnemy].id != pchar.id) 
	{
		pchar.SkipBattle = true;
	}
}

void ArenaTournamentDuelEnd(bool bWin)
{
	LAi_Fade("", "");

	ref looser;
	string sEnemy = PChar.Arena.Tournament.Temp.Enemy;
	string sAttack = PChar.Arena.Tournament.Temp.Attack;

	int iEnemy = GetCharacterIndex(sEnemy);
	int iAttack = GetCharacterIndex(sAttack);

	ref enemy, attack;
	if(CheckAttribute(&Characters[iAttack], "ArenaTournament.looser"))
	{
		looser = &Characters[iAttack];
		PChar.Arena.Tournament.Temp.Winner = Characters[iEnemy].id;
	}

	if(CheckAttribute(&Characters[iEnemy], "ArenaTournament.looser"))
	{
		looser = &Characters[iEnemy];
		PChar.Arena.Tournament.Temp.Winner = Characters[iAttack].id;
	}

	int iStage = 0;
	int iStageTemp = 0;

	string sQuest = "AT_LooserDetector_" + sti(PChar.Arena.Tournament.Temp.Stage) + "_" + sti(PChar.Arena.Tournament.Temp.StageTemp);
	PChar.quest.(sQuest).over = "yes";

	if(sti(PChar.Arena.Tournament.Temp.Stage) == 1)
	{
		if(sti(PChar.Arena.Tournament.Temp.StageTemp) <= 4)
		{
			PChar.Arena.Tournament.Temp.StageTemp = sti(PChar.Arena.Tournament.Temp.StageTemp) + 1;
		}

		if(sti(PChar.Arena.Tournament.Temp.StageTemp) > 4)
		{
			PChar.Arena.Tournament.Temp.Stage = 2;
			PChar.Arena.Tournament.Temp.StageTemp = 1;
			PChar.Arena.Tournament.Temp.StagePause = true;
		}
	}

	if(sti(PChar.Arena.Tournament.Temp.Stage) == 2 && PChar.Arena.Tournament.Temp.StagePause == false)
	{
		if(sti(PChar.Arena.Tournament.Temp.StageTemp) <= 2)
		{
			PChar.Arena.Tournament.Temp.StageTemp = sti(PChar.Arena.Tournament.Temp.StageTemp) + 1;
		}

		if(sti(PChar.Arena.Tournament.Temp.StageTemp) > 2)
		{
			PChar.Arena.Tournament.Temp.Stage = 3;
			PChar.Arena.Tournament.Temp.StageTemp = 1;
			PChar.Arena.Tournament.Temp.StagePause = true;
		}
	}

	if(sti(PChar.Arena.Tournament.Temp.Stage) == 3 && PChar.Arena.Tournament.Temp.StagePause == false)
	{
		ArenaTournamentEnd();
		return;
	}

	iStage = sti(PChar.Arena.Tournament.Temp.Stage);
	iStageTemp = sti(PChar.Arena.Tournament.Temp.StageTemp);

	if(iStage == 2 && iStageTemp == 1)
	{
		PChar.Arena.Tournament.InterfacePause = true;
		PChar.Arena.Tournament.Temp.InterfaceStage = "1_2";
		LAi_SetActorType(PChar);
		DoQuestFunctionDelay("ArenaResultTournament", 3.0);
		return;
	}

	if(iStage == 3 && iStageTemp == 1)
	{
		PChar.Arena.Tournament.InterfacePause = true;
		PChar.Arena.Tournament.Temp.InterfaceStage = "final";
		LAi_SetActorType(PChar);
		DoQuestFunctionDelay("ArenaResultTournament", 3.0);
		return;
	}

	ArenaTournamentGoStart(iStage, iStageTemp);
	ArenaTournamentStartNewBattle();
}

void ArenaResultTournament(string qName)
{
	LaunchArena(true);
	ArenaTournamentPrepareInterface();
}

void ArenaTournamentEnd()
{
	int iPggWin, iPrize;
	ref rWin;
	string sFoe;

	LAi_Fade("", "");

	bool bWinner = false;

	string sWinner = PChar.Arena.Tournament.Temp.Winner;
	if(sWinner == "Blaze")
	{
		bWinner = true;
	}
	else
	{
		bWinner = false;
		// LEO: турнир взял не игрок. Если победителем оказался подсаженный конкурент - касса, слух и
		//   засчитанная победа достаются ему (без этого выигрыш ПГГ на глазах игрока проходил впустую).
		if (GetCharacterIndex(sWinner) >= 0)
		{
			rWin = CharacterFromID(sWinner);
			if (CheckAttribute(rWin, "ArenaTournament.PGGId"))
			{
				iPggWin = GetCharacterIndex(rWin.ArenaTournament.PGGId);
				if (iPggWin >= 0)
				{
					iPrize = 0;
					if (CheckAttribute(PChar, "Arena.Tournament.Money")) iPrize = sti(PChar.Arena.Tournament.Money);
					PGG_ArenaAwardWinner(GetCharacter(iPggWin), iPrize);
				}
			}
		}
	}

	// LEO: итог финала против конкурента. Соперника запомнили при старте решающего боя
	//   (ArenaTournamentGoStart, iStage==3), а меняем отношение только теперь - по РЕЗУЛЬТАТУ:
	//   игрок взял верх -> уважение (+5), уступил -> конкурент задирает нос (-5).
	if (CheckAttribute(PChar, "Arena.Tournament.FinalFoe"))
	{
		sFoe = PChar.Arena.Tournament.FinalFoe;
		if (GetCharacterIndex(sFoe) >= 0)
		{
			if (bWinner) PGG_ArenaFinalRespect(CharacterFromID(sFoe), 5);
			else PGG_ArenaFinalRespect(CharacterFromID(sFoe), -5);
		}
		DeleteAttribute(PChar, "Arena.Tournament.FinalFoe");
	}

	PChar.Arena.Tournament.Win = bWinner;

	PChar.Statistic.Arena.Count = sti(PChar.Statistic.Arena.Count) + 1;
	if(!bWinner)
	{
		PChar.Statistic.Arena.LooserCount = sti(PChar.Statistic.Arena.LooserCount) + 1;
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Arena_29", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_30", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_31", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Arena_32", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + ", "+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_33", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_34", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_35", "AddRumours.txt", "#space#", " ")+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_36", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_37", "AddRumours.txt", "#space#", " ")), 5, 1);
	}
	else
	{
		PChar.Statistic.Arena.WinnerCount = sti(PChar.Statistic.Arena.WinnerCount) + 1;
		AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Arena_38", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_39", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_40", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Arena_41", "AddRumours.txt", "#space#", " ")+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_42", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_43", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_44", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + ".", 5, 1);
	}

	locCameraFollow();

	Arena_ClearFightEffects(PChar); // LEO: с турнира уходим чистыми
	LAi_SetCurHPMax(PChar);

	InterfaceStates.Buttons.Save.enable = true;
	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "reload", "reload1");
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_head");
	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorWaitDialog(PChar, CharacterFromID("FencingTown_Head"));
	LAi_ActorDialog(CharacterFromID("FencingTown_Head"), PChar, "", 5.0, 1.0);

	Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_7";
	Locations[FindLocation("FencingTown_Arena")].locators_radius.item.looser_detector = 0.5;

	// LEO: турнир отыгран по-настоящему - конкурентам больше нечего ждать на Доминике, снимаемся и по делам.
	PGG_CloseTournamentForAll();
}

void ArenaTournamentTheEnd()
{
	InterfaceStates.Buttons.Save.enable = true;
	LAi_SetPlayerType(PChar);
	LAi_SetImmortal(PChar, false);

	string sSaber = "blade1";

	RemoveCharacterEquip(PChar, BLADE_ITEM_TYPE);
	if (CheckAttribute(pchar,"Arena.TournamentWeapon"))
	{
		sSaber = pchar.Arena.TournamentWeapon;
		TakeNItems(PChar, sSaber, -1);
	}

	string sMember = "";

	ref member;
	for(int i = 1; i <= 7; i++)
	{
		sMember = "Arena_Tournament_Character_" + i;
		member = &Characters[GetCharacterIndex(sMember)];

		RemoveCharacterEquip(member, BLADE_ITEM_TYPE);
		if(CheckAttribute(member, "ArenaAction"))
		{
			DeleteAttribute(member, "ArenaAction");
		}

		if(CheckAttribute(member, "ArenaTournament.looser"))
		{
			DeleteAttribute(member, "ArenaTournament.looser");
		}

		LAi_SetStayType(member);
		ChangeCharacterAddress(member, "None", "");
		member.LifeDay = 0;
	}

	LAi_SetOwnerType(CharacterFromID("FencingTown_Head"));
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Townhall", "goto", "goto2");

	DoQuestReloadToLocation("FencingTown_ExitTown", "reload", "reload4", "");

	if(CheckAttribute(PChar, "ArenaAction"))
	{
		DeleteAttribute(PChar, "ArenaAction");
	}

	if(CheckAttribute(PChar, "ArenaTournament.looser"))
	{
		DeleteAttribute(PChar, "ArenaTournament.looser");
	}

	if(CheckAttribute(PChar, "Arena.Tournament"))
	{
		DeleteAttribute(PChar, "Arena.Tournament");
	}

	ClearArena("");

	LAi_group_MoveCharacter(PChar, LAI_GROUP_PLAYER);
	LAi_group_Delete("ARENA_TOURNAMENT_CHARACTER_1");
	LAi_group_Delete("ARENA_TOURNAMENT_CHARACTER_2");
}

void ArenaTournamentSetDetailsForChar(ref chr, int iStage)
{
	float fEnPgg;
	string sSaber = pchar.Arena.TournamentWeapon;

	RemoveCharacterEquip(chr, GUN_ITEM_TYPE);
	RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);

	if (chr.index == nMainCharacterIndex)
	{
		if (iStage == 2 || iStage == 3) TakeNItems(chr,sSaber,-1);
	}
	GiveItem2Character(chr, sSaber);
	EquipCharacterByItem(chr, sSaber);

	if(chr.id == PChar.id)
	{
		return;
	}

	// LEO: подсаженный конкурент дерётся СВОИМИ данными. Ниже идёт подгонка статиста под игрока (здоровье
	//   от ХП ГГ +200, фехтование от навыков ГГ + бонус стадии, боевые перки) - она и перебивала всё, что
	//   скопировано из настоящего ПГГ. Оружие выдано выше, оно турнирное и единое для всех - остаётся.
	if (CheckAttribute(chr, "ArenaTournament.PGGId"))
	{
		PGG_SyncTournamentFighter(chr);
		fEnPgg = LAi_GetCharacterMaxEnergy(chr);
		chr.chr_ai.energy    = stf(fEnPgg);
		chr.chr_ai.energyMax = stf(fEnPgg);
		return;
	}

	int iBonus = iStage * 10;

	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE);
	int iFencingLight = sti(PChar.skill.FencingLight);
	int iFencing = sti(PChar.skill.Fencing);
	int iFencingHeavy = sti(PChar.skill.FencingHeavy);

	float HPmx = LAi_GetCharacterMaxHP(PChar);
	float Energy = LAi_GetCharacterMaxEnergy(PChar);

	int rand_1 = rand(9)+1;
	int rand_2 = rand(9)+1;
	rand_1 += rand_2;

	float HP = HPmx + 200 + (rand(20) * rand_1);
	Energy += 10 + rand(5);

	chr.chr_ai.energy = stf(Energy);
	chr.chr_ai.energyMax = stf(Energy);

	if(chr.id != PChar.id)
	{
		LAi_SetHP(chr, stf(HP), stf(HP));
	}
	else
	{
		LAi_SetCurHPMax(PChar);
	}

	iFencingLight += (iBonus + 10);
	iFencing += (iBonus + 10);
	iFencingHeavy += (iBonus + 10);

	if(iFencingLight > 100) { iFencingLight = 100; }
	if(iFencingLight < 30) { iFencingLight = 30; }

	if(iFencing > 100) { iFencing = 100; }
	if(iFencing < 30) { iFencing = 30; }

	if(iFencingHeavy > 100) { iFencingHeavy = 100; }
	if(iFencingHeavy < 30) { iFencingHeavy = 30; }

	chr.skill.FencingLight = iFencingLight;
	chr.skill.Fencing = iFencing;
	chr.skill.FencingHeavy = iFencingHeavy;

	if(chr.id != PChar.id)
	{
		SetCharacterPerk(chr, "Energaiser");
		SetCharacterPerk(chr, "SwordplayProfessional");
		SetCharacterPerk(chr, "AdvancedDefense");
		SetCharacterPerk(chr, "CriticalHit");
		SetCharacterPerk(chr, "Sliding");
	}
}

// LEO: ПОСАДИТЬ ПРИБЫВШЕГО ПГГ В СЕТКУ ТУРНИРА.
// Сетка жёстко перебирает Arena_Tournament_Character_1..7 и Blaze (ArenaTournamentGetCharacterIDFromPos),
// поэтому вместо ломки перебора «вселяем» конкурента в свободный слот: боец получает его имя, облик и
// боевые данные. Обратная связь слот -> ПГГ живёт в PChar.Arena.Tournament.PGGSeat.<i> - по ней начисляем
// победу. Механика турнира при этом не меняется.
bool PGG_SeatInTournament(ref pgg)
{
	if (!CheckAttribute(PChar, "Arena.Tournament")) return false;   // турнира сейчас нет
	if (CheckAttribute(pgg, "PGG_Tournament.Seat")) return true;    // уже посажен

	string sSlot, sIdx;
	ref fighter;
	for (int i = 1; i <= 7; i++)
	{
		// ⚠️ номер слота держим СТРОКОЙ. Обращение вида .(i) с числом заводит не тот атрибут, что ищет
		//   CheckAttribute по склеенному имени: занятость не находилась, и каждый ПГГ садился на место 1,
		//   затирая предыдущего - в сетке оставался только последний.
		sIdx = "" + i;
		sSlot = "Arena_Tournament_Character_" + sIdx;
		if (GetCharacterIndex(sSlot) < 0) continue;                               // бойца с таким ид нет
		if (CheckAttribute(PChar, "Arena.Tournament.PGGSeat." + sIdx)) continue;   // слот уже занят другим ПГГ

		fighter = CharacterFromID(sSlot);
		// LEO: запоминаем облик статиста - при снятии ПГГ с турнира слот должен вернуться к прежнему виду,
		//   иначе безымянный боец так и останется ходить с моделью конкурента.
		if (!CheckAttribute(fighter, "ArenaTournament.OldModel"))
		{
			fighter.ArenaTournament.OldModel = fighter.model;
			fighter.ArenaTournament.OldAnim  = fighter.model.animation;
		}
		fighter.name     = pgg.name;
		fighter.lastname = pgg.lastname;
		fighter.model    = pgg.model;
		fighter.FaceId   = pgg.FaceId;
		fighter.sex      = pgg.sex;
		// ⚠️ ОБЛИК - ЭТО НАБОР, а не одна модель: HeroModel (базовая + кирасные варианты), отдельная голова,
		//   рост и анимация. Без HeroModel кирасная логика (CheckForCirass при создании персонажа в локации)
		//   пересобирает бойца из данных статиста; без анимации модель с чужим набором просто не встанет
		//   (среди конкурентов есть женщины).
		fighter.model.animation = pgg.model.animation;
		if (CheckAttribute(pgg, "HeroModel")) fighter.HeroModel = pgg.HeroModel;
		else DeleteAttribute(fighter, "HeroModel");
		if (CheckAttribute(pgg, "headModel")) fighter.headModel = pgg.headModel;
		if (CheckAttribute(pgg, "model.height")) fighter.model.height = pgg.model.height;
		// применяем: запись атрибутов меняет только карточку, в мир облик доносит SetNewModelToChar.
		//   Оружие она не выдаёт, лишь перевешивает надетое - единый турнирный клинок остаётся в силе.
		SetNewModelToChar(fighter);
		fighter.ArenaTournament.PGGId = pgg.id;   // кто на самом деле дерётся
		PGG_SyncTournamentFighter(fighter);       // ранг, навыки, характер, SPECIAL и ХП - как у живого конкурента

		PChar.Arena.Tournament.PGGSeat.(sIdx) = pgg.id;
		pgg.PGG_Tournament.Seat = sIdx;
		return true;
	}
	return false;
}

// ============================ СИМУЛЯЦИЯ ТУРНИРА БЕЗ ИГРОКА ============================
// LEO: восьмая позиция сетки - всегда сам ГГ (id "Blaze" = Characters[1]), поэтому настоящий турнир идёт
// только когда игрок пришёл. Не пришёл - разыгрываем абстрактно, на выбывание: конкуренты плыли не зря.
// Насколько перевес в силе решает исход: 1 - шанс пропорционален силе (вдвое сильнее берёт 2 боя из 3),
// 2 - фаворит заметно надёжнее, 3 - почти без сюрпризов, 0 - монетка.
#define PGG_ARENA_SIM_FAVOR	2

// Исход поединка: true - победил первый. Силу считаем как игра при пропуске боя (seadogs.c): эффективные
// ХП плюс множитель за перевес в фехтовании. Но НЕ сравнением «у кого больше» - иначе подогнанный под
// игрока боец арены не проиграл бы конкуренту никогда. Шанс победы = доля своей силы в общей.
bool PGG_ArenaDuelWin(ref chr1, ref chr2)
{
	int i, iChance;
	float p1, p2, lvl1, lvl2, eff1, eff2;

	lvl1 = LAi_GetCharacterFightLevel(chr1);
	lvl2 = LAi_GetCharacterFightLevel(chr2);
	eff1 = GetCharacterEffectiveHp(chr1);
	eff2 = GetCharacterEffectiveHp(chr2);
	if (lvl1 > lvl2) eff1 = eff1 / (1.0 - 0.7 * (lvl1 - lvl2));
	else             eff2 = eff2 / (1.0 - 0.7 * (lvl2 - lvl1));
	if (eff1 < 1.0) eff1 = 1.0;
	if (eff2 < 1.0) eff2 = 1.0;
	// возводим силы в степень PGG_ARENA_SIM_FAVOR - чем выше, тем реже побеждает слабейший
	p1 = 1.0;
	p2 = 1.0;
	i = 0;
	while (i < PGG_ARENA_SIM_FAVOR)
	{
		p1 = p1 * eff1;
		p2 = p2 * eff2;
		i++;
	}
	// шанс первого = p1/(p1+p2); дробим на тысячные, чтобы бросить целочисленным dRand (он не сейв-скамится)
	iChance = makeint(1000.0 * p1 / (p1 + p2));
	if (dRand(1000) < iChance) return true;
	return false;
}

// Итог финала против ГГ: победил игрок - конкурент проникается уважением (+5), проиграл - тот задирает
// нос (-5). Начисляется РОВНО ОДИН раз за турнир.
void PGG_ArenaFinalRespect(ref fighter, int iDelta)
{
	int iPgg;
	ref pgg;
	if (!CheckAttribute(fighter, "ArenaTournament.PGGId")) return;         // в финале был обычный боец
	if (CheckAttribute(fighter, "ArenaTournament.FinalRespect")) return;   // уже начислено за этот турнир
	iPgg = GetCharacterIndex(fighter.ArenaTournament.PGGId);
	if (iPgg < 0) return;
	pgg = GetCharacter(iPgg);
	fighter.ArenaTournament.FinalRespect = true;
	PGG_ChangeRelation2MainCharacter(pgg, iDelta);
	Log_TestInfo("ТУРНИР: финал ГГ против " + pgg.id + ", отношение " + iDelta);
}

// Награда победителю-конкуренту: касса турнира, слух по всем нациям и счёт побед (он же копится
// на прибавку к урону по ГГ - см. LAi_fightparams.c).
void PGG_ArenaAwardWinner(ref pgg, int iMoney)
{
	int iWins, iPurse;
	iWins = 0;
	if (CheckAttribute(pgg, "PGG_ArenaWins")) iWins = sti(pgg.PGG_ArenaWins);
	iWins++;
	pgg.PGG_ArenaWins = iWins;
	// ⚠️ деньги ПГГ живут под потолком «база + ранг×коэффициент» (PGG_AddMoneyToCharacter), и приз при
	//   первом же обычном доходе срезался обратно. Копим призовые отдельной суммой - на неё потолок и
	//   поднимается, поэтому выигрыш остаётся у конкурента (и ему будет на что нанимать охрану в каюту).
	iPurse = 0;
	if (CheckAttribute(pgg, "PGG_ArenaPurse")) iPurse = sti(pgg.PGG_ArenaPurse);
	pgg.PGG_ArenaPurse = iPurse + iMoney;
	pgg.Money = sti(pgg.Money) + iMoney;
	// LEO: род слуха определяется победителем-ПГГ, а не главным героем.
	string sRumourStart = "Variable_Arena_67";
	string sRumourEnd = "Variable_Arena_68";
	if (pgg.sex == "woman")
	{
		sRumourStart = "Variable_Arena_67f";
		sRumourEnd = "Variable_Arena_68f";
	}
	AddSimpleRumourToAllNations(GetConvertStrWithReplace(sRumourStart, "AddRumours.txt", "#space#", " ") +
	                            GetFullName(pgg) +
	                            GetConvertStrWithReplace(sRumourEnd, "AddRumours.txt", "#space#", " "), 5, 1);
	Log_TestInfo("ТУРНИР: победитель " + pgg.id + " (" + GetFullName(pgg) + "), приз " + iMoney +
	             ", всего побед " + iWins);
}

// Прогон турнира без игрока: раунды на выбывание среди тех, кто реально стоит в сетке.
void PGG_SimulateTournament()
{
	int i, iPair, iQty, iNext, iMoney, iPgg;
	string sSlot;
	ref winner;
	int aFighter[8];

	if (!CheckAttribute(PChar, "Arena.Tournament")) return;                // сетки уже нет - считать нечего
	if (CheckAttribute(PChar, "Arena.Tournament.PlayerPlayed")) return;    // игрок отыграл сам, награду выдал ArenaTournamentEnd
	if (!CheckAttribute(PChar, "Arena.Tournament.PGGSeat")) return;        // в сетке не было ни одного конкурента

	iQty = 0;
	for (i = 1; i <= 7; i++)
	{
		sSlot = "Arena_Tournament_Character_" + i;
		if (GetCharacterIndex(sSlot) < 0) continue;
		aFighter[iQty] = GetCharacterIndex(sSlot);
		iQty++;
	}
	if (iQty < 2) return;

	// раунды на выбывание; при нечётном числе последний проходит дальше без боя
	// ⚠️ только простые сравнения переменных: выражение в условии цикла Storm не переваривает
	//    (`for (i = 0; i + 1 < iQty; i = i + 2)` -> COMPILE ERROR: invalid 'for' syntax)
	while (iQty > 1)
	{
		iNext = 0;
		i = 0;
		iPair = 1;
		while (iPair < iQty)
		{
			if (PGG_ArenaDuelWin(&Characters[aFighter[i]], &Characters[aFighter[iPair]]))
			{
				aFighter[iNext] = aFighter[i];
			}
			else
			{
				aFighter[iNext] = aFighter[iPair];
			}
			iNext++;
			i = i + 2;
			iPair = i + 1;
		}
		if (i < iQty)
		{
			aFighter[iNext] = aFighter[i];
			iNext++;
		}
		iQty = iNext;
	}

	winner = &Characters[aFighter[0]];
	if (!CheckAttribute(winner, "ArenaTournament.PGGId")) return;   // выиграл обычный боец - награждать некого
	iPgg = GetCharacterIndex(winner.ArenaTournament.PGGId);
	if (iPgg < 0) return;

	iMoney = 0;
	if (CheckAttribute(PChar, "Arena.Tournament.Money")) iMoney = sti(PChar.Arena.Tournament.Money);
	PGG_ArenaAwardWinner(GetCharacter(iPgg), iMoney);
}
// ==========================================================================================

// LEO: характер персонажа (он же специальность) - это один из семи перков. Отдельного поля нет,
// поэтому вытаскиваем перебором. Пусто - характера нет (у части статистов так и есть).
string PGG_GetSpeciality(ref chr)
{
	if (IsCharacterPerkOn(chr, "Adventurer")) return "Adventurer";
	if (IsCharacterPerkOn(chr, "SeaWolf"))    return "SeaWolf";
	if (IsCharacterPerkOn(chr, "Agent"))      return "Agent";
	if (IsCharacterPerkOn(chr, "Grunt"))      return "Grunt";
	if (IsCharacterPerkOn(chr, "Fencer"))     return "Fencer";
	if (IsCharacterPerkOn(chr, "Trader"))     return "Trader";
	if (IsCharacterPerkOn(chr, "Buccaneer"))  return "Buccaneer";
	return "";
}

// LEO: подтянуть боевые данные слота из живого ПГГ. Зовётся при подсадке И перед каждым боем: между
// этими моментами проходят дни, конкурент растёт в ранге и лечится, а слот держал бы снимок с прибытия.
// Что берём: ранг, фехтовальные навыки, SPECIAL, характер и запас здоровья - именно они решают исход.
void PGG_SyncTournamentFighter(ref fighter)
{
	int iPgg;
	ref pgg;
	aref arDst, arSrc;
	string sSpec;
	float fHP;

	if (!CheckAttribute(fighter, "ArenaTournament.PGGId")) return;
	iPgg = GetCharacterIndex(fighter.ArenaTournament.PGGId);
	if (iPgg < 0) return;
	pgg = GetCharacter(iPgg);

	fighter.name     = pgg.name;
	fighter.lastname = pgg.lastname;
	fighter.rank     = pgg.rank;
	fighter.skill.FencingLight = pgg.skill.FencingLight;
	fighter.skill.Fencing      = pgg.skill.Fencing;
	fighter.skill.FencingHeavy = pgg.skill.FencingHeavy;
	fighter.skill.Pistol       = pgg.skill.Pistol;
	fighter.skill.Fortune      = pgg.skill.Fortune;
	// SPECIAL: от него зависят и урон, и уклонения - без него боец дрался бы по броску генератора
	if (CheckAttribute(pgg, "SPECIAL.Strength"))
	{
		SetSPECIAL(fighter, sti(pgg.SPECIAL.Strength), sti(pgg.SPECIAL.Perception), sti(pgg.SPECIAL.Endurance),
		           sti(pgg.SPECIAL.Charisma), sti(pgg.SPECIAL.Intellect), sti(pgg.SPECIAL.Agility),
		           sti(pgg.SPECIAL.Luck));
	}
	// ⚠️ ПЕРКИ переносим ЦЕЛИКОМ: характер персонажа - это тоже перк (Adventurer/Fencer/...), и слот держал
	//   бы свой, от генератора. Заодно уходят боевые перки статиста, которыми арена одаривает подставных.
	sSpec = PGG_GetSpeciality(pgg);
	DeleteAttribute(fighter, "perks");
	if (CheckAttribute(pgg, "perks"))
	{
		fighter.perks = "";
		makearef(arDst, fighter.perks);
		makearef(arSrc, pgg.perks);
		CopyAttributes(arDst, arSrc);
	}
	else
	{
		RemoveSpeciality(fighter);
		if (sSpec != "") SetSpeciality(fighter, sSpec);
	}

	// ГОЛОС: набор арены (Gr_ArenaMember) - сплошь мужские угрозы. Для женщин заведён свой алиас
	//   Gr_ArenaMember_Woman (Greetings_alias.ini, обе локализации).
	if (CheckAttribute(pgg, "sex"))
	{
		if (pgg.sex == "woman") fighter.greeting = "Gr_ArenaMember_Woman";
		else fighter.greeting = "Gr_ArenaMember";
	}

	// счёт турнирных побед едет со слотом: от него зависит прибавка к урону по ГГ (LAi_fightparams.c)
	if (CheckAttribute(pgg, "PGG_ArenaWins")) fighter.PGG_ArenaWins = pgg.PGG_ArenaWins;
	else DeleteAttribute(fighter, "PGG_ArenaWins");

	// здоровье ровно как у конкурента в жизни; кап арены (500) на подсаженных не распространяется - иначе
	//   крепкий ПГГ вышел бы слабее себя настоящего (гейт в ArenaTournamentStartNewBattle)
	fHP = LAi_GetCharacterMaxHP(pgg);
	if (fHP < 1.0) fHP = 1.0;
	LAi_SetHP(fighter, fHP, fHP);
	// запас взят у конкурента, а он бонус перка на здоровье уже включает - переносим и отметку о выдаче,
	//   иначе SyncCharacterHPPerks доложит бонус бойцу вторым слоем
	if (CheckAttribute(pgg, "PerkValue.HPPlus")) fighter.PerkValue.HPPlus = 0;
	else DeleteAttribute(fighter, "PerkValue.HPPlus");
	if (CheckAttribute(pgg, "PerkValue.HPPlusFixed")) fighter.PerkValue.HPPlusFixed = 0;
	else DeleteAttribute(fighter, "PerkValue.HPPlusFixed");
}

// LEO: полный сброс сетки перед новой генерацией турнира: снять облик ПГГ со всех слотов и стереть
// у самих конкурентов память о занятом месте. Без этого при следующем турнире PGG_SeatInTournament
// сразу возвращала "уже посажен", и в сетке стояли безымянные статисты.
void PGG_ResetTournamentSeats()
{
	int i;
	ref chr;
	string sSlot;
	for (i = 1; i <= 7; i++)
	{
		sSlot = "Arena_Tournament_Character_" + i;
		if (GetCharacterIndex(sSlot) < 0) continue;
		chr = CharacterFromID(sSlot);
		DeleteAttribute(chr, "ArenaTournament.PGGId");
		DeleteAttribute(chr, "ArenaTournament.OldModel");
		DeleteAttribute(chr, "ArenaTournament.OldAnim");
		DeleteAttribute(chr, "ArenaTournament.FinalRespect");
		DeleteAttribute(chr, "PGG_ArenaWins");
		DeleteAttribute(chr, "HeroModel");    // кирасный набор от прошлого ПГГ - иначе генератор оденет слот в него
		DeleteAttribute(chr, "headModel");
		DeleteAttribute(chr, "perks");        // перки и характер были конкурента; статисту генератор выдаст свои
		chr.sex = "man";                      // пол/голос тоже могли быть женскими - вернуть к статисту
		chr.greeting = "Gr_ArenaMember";
	}
	for (i = 1; i <= PsHeroQty; i++)
	{
		if (GetCharacterIndex("PsHero_" + i) < 0) continue;
		chr = CharacterFromID("PsHero_" + i);
		DeleteAttribute(chr, "PGG_Tournament.Seat");
	}
	DeleteAttribute(PChar, "Arena.Tournament.PGGSeat");
}

// LEO: посадить в свежую сетку всех, кто уже дождался турнира на якоре в бухте.
void PGG_ReseatParkedTournament()
{
	ref chr;
	for (int i = 1; i <= PsHeroQty; i++)
	{
		if (GetCharacterIndex("PsHero_" + i) < 0) continue;
		chr = CharacterFromID("PsHero_" + i);
		if (!CheckAttribute(chr, "PGG_Tournament.Parked")) continue;
		PGG_SeatInTournament(chr);
	}
}

// LEO: освободить место ПГГ в сетке. Слот при этом остаётся полноценным бойцом - он просто снова
// безымянный статист, каким и был до подсадки. Зовётся при гибели и при снятии с турнира.
void PGG_FreeTournamentSeat(ref pgg)
{
	if (!CheckAttribute(pgg, "PGG_Tournament.Seat")) return;
	string sSeat = pgg.PGG_Tournament.Seat;
	string sSlot = "Arena_Tournament_Character_" + sSeat;
	if (GetCharacterIndex(sSlot) >= 0)
	{
		ref fighter = CharacterFromID(sSlot);
		DeleteAttribute(fighter, "ArenaTournament.PGGId");
		DeleteAttribute(fighter, "ArenaTournament.FinalRespect");
		DeleteAttribute(fighter, "PGG_ArenaWins");
		SetRandomNameToCharacter(fighter);   // снова статист, а не наш конкурент
		if (CheckAttribute(fighter, "ArenaTournament.OldModel"))
		{
			fighter.model = fighter.ArenaTournament.OldModel;
			fighter.model.animation = fighter.ArenaTournament.OldAnim;
			DeleteAttribute(fighter, "ArenaTournament.OldModel");
			DeleteAttribute(fighter, "ArenaTournament.OldAnim");
			DeleteAttribute(fighter, "HeroModel");   // кирасный набор был от ПГГ, статисту он не принадлежит
			DeleteAttribute(fighter, "headModel");
			// перки/характер тоже были конкурента; боевой набор статисту выдаст ArenaTournamentSetDetailsForChar
			DeleteAttribute(fighter, "perks");
			fighter.sex = "man";                    // статисты арены все мужчины
			fighter.greeting = "Gr_ArenaMember";    // и голос обратно арены
			if (IsEntity(fighter)) SetNewModelToChar(fighter);
		}
	}
	DeleteAttribute(PChar, "Arena.Tournament.PGGSeat." + sSeat);
	DeleteAttribute(pgg, "PGG_Tournament.Seat");
}

int GetNumPositionForCharInTournament()
{
	string sPos = "";

	int iRand = rand(7)+1;

	for(int i=1; i <= 8; i++)
	{
		sPos = "Position_" + i;
		if(iRand == i)
		{
			if(!CheckAttribute(PChar, "Arena.Tournament.Positions." + sPos))
			{
				PChar.Arena.Tournament.Positions.(sPos) = true;
				return i;
			}
			else
			{
				i = 0;
				iRand = rand(7)+1;
				continue;
			}
		}
	}

	return -1;
}

string GetModelForCharacterInTournament()
{
	string sModel = "";
	int iRand = rand(19) + 1;

	for(int i=1; i <= 20; i++)
	{
		sModel = "officer_" + i;

		if(iRand == i)
		{
			if(!CheckAttribute(PChar, "Arena.Tournament.Models." + sModel))
			{
				PChar.Arena.Tournament.Models.(sModel) = true;
				return sModel;
			}
			else
			{
				i = 0;
				iRand = rand(19)+1;
				continue;
			}
		}
	}
	return "officer_" + (rand(42)+1);
}

void ArenaTournamentSetPosition(int iNum, string sChar, string sStage)
{
	int iPosition = 1;
	switch(sStage)
	{
		case "1_2":
			if(iNum == 1 || iNum == 2) { iPosition = 1; }
			if(iNum == 3 || iNum == 4) { iPosition = 2; }
			if(iNum == 5 || iNum == 6) { iPosition = 3; }
			if(iNum == 7 || iNum == 8) { iPosition = 4; }
			PChar.Arena.Tournament.(sChar).Position.1_2 = iPosition;
		break;

		case "final":
			if(iNum == 1 || iNum == 2 || iNum == 3 || iNum == 4) { iPosition = 1;}
			if(iNum == 5 || iNum == 6 || iNum == 7 || iNum == 8) { iPosition = 2;}
			PChar.Arena.Tournament.(sChar).Position.final = iPosition;
		break;
	}
}

string ArenaTournamentGetCharacterIDFromPos(int p, string sType)
{
	aref pos;
	string sChar = "";

	for(int i=1; i <= 8; i++)
	{
		sChar = "Arena_Tournament_Character_" + i;

		if(i == 8)
		{
			sChar = "Blaze";
		}

		if(CheckAttribute(&Characters[GetCharacterIndex(sChar)], "ArenaTournament.looser"))
		{
			continue;
		}

		makearef(pos, PChar.Arena.Tournament.(sChar));

		if(sType == "1_4")
		{
			if(sti(pos.Position) == p)
			{
				return sChar;
			}
		}
		else
		{
			if(sti(pos.Position.(sType)) == p)
			{
				return sChar;
			}
		}
	}

	return "Arena_Tournament_Character_1";
}

void ArenaTournamentSetCharacterFromArena(ref chr)
{
	string sLocator = "";

	if(chr.id == "Arena_Tournament_Character_1") { sLocator = "arena_member_1"; }
	if(chr.id == "Arena_Tournament_Character_2") { sLocator = "arena_member_2"; }
	if(chr.id == "Arena_Tournament_Character_3") { sLocator = "arena_member_3"; }
	if(chr.id == "Arena_Tournament_Character_4") { sLocator = "arena_member_4"; }
	if(chr.id == "Arena_Tournament_Character_5") { sLocator = "arena_member_5"; }
	if(chr.id == "Arena_Tournament_Character_6") { sLocator = "arena_member_6"; }
	if(chr.id == "Arena_Tournament_Character_7") { sLocator = "arena_member_7"; }
	if(chr.id == "Blaze") { sLocator = "arena_member_8"; }

	ChangeCharacterAddressGroup(chr, "FencingTown_Arena", "goto", sLocator);
	// LEO: боец только что попал в мир - вот теперь можно применить облик подсаженного ПГГ. Раньше вызова
	//   не было, потому что при записи в сетку персонаж ещё не загружен и смена модели ни на что не влияет:
	//   в сетке значилось имя конкурента, а на арене стоял прежний статист.
	if (CheckAttribute(chr, "ArenaTournament.PGGId")) SetNewModelToChar(chr);
	LAi_SetActorType(chr);
	LAi_ActorTurnToLocator(chr, "item", "looser_detector");
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

void ArenaTournamentPrepareInterface()
{
	ref attack, enemy;

	string sAttack = PChar.Arena.Tournament.Temp.Attack;
	string sEnemy = PChar.Arena.Tournament.Temp.Enemy;

	if(GetCharacterIndex(sAttack) != -1)
	{
		attack = &Characters[GetCharacterIndex(sAttack)];
		DeleteAttribute(attack, "ArenaTournament.Attack");
		DeleteAttribute(attack, "ArenaTournament.Enemy");
		ArenaTournamentSetCharacterFromArena(attack);
	}

	if(GetCharacterIndex(sEnemy) != -1)
	{
		enemy = &Characters[GetCharacterIndex(sEnemy)];
		DeleteAttribute(enemy, "ArenaTournament.Attack");
		DeleteAttribute(enemy, "ArenaTournament.Enemy");
		ArenaTournamentSetCharacterFromArena(enemy);
	}
}


//-------------------------------------------------------------------------------
// Ставки
//-------------------------------------------------------------------------------
void GenerateArenaOdds()
{
	string sSaber, sSaberType;

	switch(rand(2)) { case 0: sSaberType = "FencingLight"; break; case 1: sSaberType = "Fencing"; break; case 2: sSaberType = "FencingHeavy"; break; }
	sSaber = GetSaberForArena(sSaberType);
	PChar.Arena.Odds.Saber.Light = sSaber;

	switch(rand(2)) { case 0: sSaberType = "FencingLight"; break; case 1: sSaberType = "Fencing"; break; case 2: sSaberType = "FencingHeavy"; break; }
	sSaber = GetSaberForArena(sSaberType);
	PChar.Arena.Odds.Saber.Saber = sSaber;

	switch(rand(2)) { case 0: sSaberType = "FencingLight"; break; case 1: sSaberType = "Fencing"; break; case 2: sSaberType = "FencingHeavy"; break; }
	sSaber = GetSaberForArena(sSaberType);
	PChar.Arena.Odds.Saber.Heavy = sSaber;

	GenerateArenaOddsDuel(1);
	GenerateArenaOddsDuel(2);
	GenerateArenaOddsDuel(3);

	int iMaxOdd = GetArenaOddsMaxOdd();
	PChar.Arena.Odds.MinOdd = 100;
	PChar.Arena.Odds.MaxOdd = iMaxOdd;
	PChar.Arena.Odds.Odd = 100;

	PChar.Arena.Odds.Temp.Attack = "";
	PChar.Arena.Odds.Temp.Enemy = "";
	PChar.Arena.Odds.Temp.Stage = 1;
	PChar.Arena.Odds.Temp.StageTemp = 1;
	PChar.Arena.Odds.Temp.Count = 0;
	PChar.Arena.Odds.Temp.WinnerCount = 0;
	PChar.Arena.Odds.Temp.LooserCount = 0;
	PChar.Arena.Odds.Temp.Winner = "";
}

void PrepareArenaOdds()
{
	if(PChar.Arena.Mode.NotOdd == false)
	{
		PChar.Arena.Odds.BiletCount = 1;
	}
	else
	{
		PChar.Arena.Odds.BiletCount = 0;
	}

	ChangeCharacterAddressGroup(CharacterFromID("ArenaBileter"), "FencingTown_ExitTown", "rld", "aloc6");
	LAi_SetActorType(CharacterFromID("ArenaBileter"));
	PChar.Arena.Mode = "Odds";
	PChar.quest.PrepareArenaOddsContinue.win_condition.l1 = "locator";
	PChar.quest.PrepareArenaOddsContinue.win_condition.l1.location = "FencingTown_ExitTown";
	PChar.quest.PrepareArenaOddsContinue.win_condition.l1.locator_group = "rld";
	PChar.quest.PrepareArenaOddsContinue.win_condition.l1.locator = "aloc6";
	PChar.quest.PrepareArenaOddsContinue.function = "PrepareArenaOddsContinue";
}

void PrepareArenaOddsContinue(string _tmp)
{
	Characters[GetCharacterIndex("ArenaBileter")].Dialog.CurrentNode = "First Time";
	LAi_SetActorType(CharacterFromID("ArenaBileter"));
	LAi_ActorDialogNow(CharacterFromID("ArenaBileter"), Pchar, "", -1);
}

void PrepareArenaOddsBilet()
{
	LAi_SetStayType(CharacterFromID("ArenaBileter"));
}

void PrepareArenaOddsEnd()
{
	LAi_SetStayType(CharacterFromID("ArenaBileter"));
	LAi_SetStayType(CharacterFromID("FencingTown_Head"));
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_head");

	if(PChar.Arena.Mode.NotOdd == false)
	{
		Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_8";
	}
	else
	{
		Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_10";
	}

	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = false;

	Locations[FindLocation("FencingTown_Arena")].locators_radius.item.looser_detector = 2.0;

	PChar.quest.StartArenaOdds.win_condition.l1 = "location";
	PChar.quest.StartArenaOdds.win_condition.l1.location = "FencingTown_Arena";
	PChar.quest.StartArenaOdds.function = "StartArenaOdds";
}

void StartArenaOdds(string _tmp)
{
	InterfaceStates.Buttons.Save.enable = false;
	PChar.Arena = "Start";

	SetIntegratedMusketDrawn(pchar, false);

	CreateFireInArena();
	CreateArenaFightPeople();

	Locations[FindLocation("FencingTown_ExitTown")].reload.l1.disable = true;

	ChangeCharacterAddress(CharacterFromID("ArenaBileter"), "None", "");
	Locations[FindLocation(PChar.location)].reload.l1.disable = true;

	int iEnemy = -1;
	string sLocator = "arena_member_" + (rand(7)+1);

	int iStage = 1;
	int iCount = 1;
	for(int e=1; e<=6; e++)
	{
		switch(e)
		{
			case 1: iStage = 1; iCount = 1; break; case 2: iStage = 1; iCount = 2; break;
			case 3: iStage = 2; iCount = 1; break; case 4: iStage = 2; iCount = 2; break;
			case 5: iStage = 3; iCount = 1; break; case 6: iStage = 3; iCount = 2; break;
		}

		iEnemy = GetCharacterIndex("Arena_Odds_Duel_" + iStage + "_Character_" + iCount);

		if(iEnemy != -1)
		{
			sLocator = "arena_member_" + e;
			ChangeCharacterAddressGroup(&Characters[iEnemy], "FencingTown_Arena", "goto", sLocator);
			LAi_SetActorType(&Characters[iEnemy]);
			LAi_ActorTurnToLocator(&Characters[iEnemy], "item", "looser_detector");
		}
	}

	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorWaitDialog(PChar, CharacterFromID("FencingTown_Head"));
	LAi_ActorDialog(CharacterFromID("FencingTown_Head"), PChar, "", 5.0, 1.0);
}

void StartArenaOddsContinue()
{
	LAi_SetPlayerType(PChar);

	PChar.quest.StartArenaOddsEnd.win_condition.l1 = "locator";
	PChar.quest.StartArenaOddsEnd.win_condition.l1.location = "FencingTown_Arena";
	PChar.quest.StartArenaOddsEnd.win_condition.l1.locator_group = "reload";
	PChar.quest.StartArenaOddsEnd.win_condition.l1.locator = "bashnya_1";
	PChar.quest.StartArenaOddsEnd.function = "StartArenaOddsEnd";
}

void StartArenaOddsEnd(string qName)
{
	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "goto", "arena_vip_4");
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_vip_3");

	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));

	LAi_ActorTurnToLocator(PChar, "item", "looser_detector");
	LAi_ActorTurnToLocator(CharacterFromID("FencingTown_Head"), "item", "looser_detector");

	DoQuestFunctionDelay("ArenaOddsGo", 3.0);
}

void ArenaOddsGo(string qName)
{
	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "goto", "pchar_temp_loc");

	LAi_SetPlayerType(PChar);
	LAi_SetActorType(PChar);
	LAi_ActorStay(PChar);

	bDisableCharacterMenu = true;
	PChar.TmpModel = PChar.model;
	PChar.model = "";
	SetNewModelToChar(PChar);

    	SetCharacterTask_None(GetMainCharacter());

	LAi_Fade("", "");

	float x = -2.26369;
	float y = 18.762;
	float z = -11.5217;

	locCameraToPos(x, y, z, false);

	ArenaOddsGoStart(1);
	ArenaOddsStartNewBattle();
}

void ArenaOddsGoStart(int iStage)
{
	ref attack, enemy;

	if(iStage != 1)
	{
		ArenaOddsSetCharacterFromArenaToLoc();
	}

	LAi_Fade("", "");

	int iChar = 0;

	string sChar = GetCharacterArenaOddsFromStage(iStage, 1);
	iChar = GetCharacterIndex(sChar);
	attack = &Characters[iChar];
	attack.ArenaOdds = true;
	attack.ArenaOdds.Attack = true;

	sChar = GetCharacterArenaOddsFromStage(iStage, 2);
	iChar = GetCharacterIndex(sChar);
	enemy = &Characters[iChar];
	enemy.ArenaOdds = true;
	enemy.ArenaOdds.Enemy = true;

	PChar.Arena.Odds.Temp.Attack = attack.id;
	PChar.Arena.Odds.Temp.Enemy = enemy.id;

	PChar.Arena.Odds.Temp.Count = 0;
	PChar.Arena.Odds.Temp.WinnerCount = 0;
	PChar.Arena.Odds.Temp.LooserCount = 0;

	ArenaOddsSetDetailsForChar(attack, iStage);
	ArenaOddsSetDetailsForChar(enemy, iStage);

	Log_Info("" + GetFullName(attack) + " VS " + GetFullName(enemy));

	if(PChar.Arena.Mode.NotOdd == false)
	{
		int iActionStage = sti(PChar.Arena.Odds.ActionFight);
		int iActionCharacter = sti(PChar.Arena.Odds.Character);

		if(iActionStage == iStage)
		{
			Log_Info(GetConvertStrWithReplace("Variable_Arena_15", "Logs.txt", "#space#", " ") + GetFullName(Characters[iActionCharacter]) + ".");
		}
	}
}

void ArenaOddsStartNewBattle()
{
	LAi_Fade("", "");

	PlayStereoSound("new_round");

	ref attack, enemy;
	string sAttack = PChar.Arena.Odds.Temp.Attack;
	string sEnemy = PChar.Arena.Odds.Temp.Enemy;

	attack = &Characters[GetCharacterIndex(sAttack)];
	enemy = &Characters[GetCharacterIndex(sEnemy)];

	LAi_SetImmortal(attack, false);
	LAi_SetImmortal(enemy, false);

	Arena_ClearFightEffects(attack); // LEO: новый бой тотализатора - оба выходят чистыми
	Arena_ClearFightEffects(enemy);

	LAi_SetWarriorType(attack);
	LAi_SetWarriorType(enemy);

	float x = -2.26369;
	float y = 18.762;
	float z = -11.5217;

	locCameraToPos(x, y, z, false);

	string sPlayerLocator, sEnemyLocator;
	if(rand(1) == 1)
	{
		sPlayerLocator = "aloc1"; sEnemyLocator = "aloc3";
	}
	else
	{
		sPlayerLocator = "aloc3"; sEnemyLocator = "aloc1";
	}

	ChangeCharacterAddressGroup(attack, "FencingTown_Arena", "rld", sPlayerLocator);
	ChangeCharacterAddressGroup(enemy, "FencingTown_Arena", "rld", sEnemyLocator);

	LAi_group_MoveCharacter(attack, "ARENA_ODDS_CHARACTER_1");
	LAi_group_MoveCharacter(enemy, "ARENA_ODDS_CHARACTER_2");

	LAi_group_SetHearRadius("ARENA_ODDS_CHARACTER_1", 100.0);
	LAi_group_SetHearRadius("ARENA_ODDS_CHARACTER_2", 100.0);
	LAi_group_SetRelation("ARENA_ODDS_CHARACTER_1", "ARENA_ODDS_CHARACTER_2", LAI_GROUP_ENEMY);
	LAi_group_FightGroups("ARENA_ODDS_CHARACTER_1", "ARENA_ODDS_CHARACTER_2", true);
}

void ArenaOddsSetLooser(ref chr)
{
	LAi_SetImmortal(chr, true);

	string sEnemy = PChar.Arena.Odds.Temp.Enemy;
	string sAttack = PChar.Arena.Odds.Temp.Attack;

	int iEnemy = GetCharacterIndex(sEnemy);
	int iAttack = GetCharacterIndex(sAttack);

	LAi_SetFightMode(&Characters[iEnemy], false);
	LAi_SetActorType(&Characters[iEnemy]);

	LAi_SetFightMode(&Characters[iAttack], false);
	LAi_SetActorType(&Characters[iAttack]);

	if(CheckAttribute(chr, "ArenaOdds.Attack"))
	{
		PChar.Arena.Odds.Temp.LooserCount = sti(PChar.Arena.Odds.Temp.LooserCount) + 1;
		Log_SetStringToLog(GetConvertStrWithReplace("Variable_Arena_16", "Logs.txt", "#space#", " ") + Characters[iEnemy].name + " " + Characters[iEnemy].lastname);
	}
	else
	{
		PChar.Arena.Odds.Temp.WinnerCount = sti(PChar.Arena.Odds.Temp.WinnerCount) + 1;
		Log_SetStringToLog(GetConvertStrWithReplace("Variable_Arena_17", "Logs.txt", "#space#", " ") + Characters[iAttack].name + " " + Characters[iAttack].lastname);
	}

	Log_SetStringToLog(GetConvertStrWithReplace("Variable_Arena_18", "Logs.txt", "#space#", " ") + GetFullName(Characters[iAttack]) + " - " + PChar.Arena.Odds.Temp.WinnerCount + ":" + sti(PChar.Arena.Odds.Temp.LooserCount) + " - " + Characters[iEnemy].name + " " + Characters[iEnemy].lastname);

	DoQuestFunctionDelay("ArenaOddsCheckNewRound", 3.0);
}

void ArenaOddsCheckNewRound(string qName)
{
	PChar.Arena.Odds.Temp.Count = sti(PChar.Arena.Odds.Temp.Count) + 1;

	PlayStereoSound("new_round");

	string sEnemy = PChar.Arena.Odds.Temp.Enemy;
	string sAttack = PChar.Arena.Odds.Temp.Attack;

	int iEnemy = GetCharacterIndex(sEnemy);
	int iAttack = GetCharacterIndex(sAttack);

	if(sti(PChar.Arena.Odds.Temp.LooserCount) >= 3)
	{
		Characters[iAttack].ArenaOdds.looser = true;
		ArenaOddsDuelEnd(false);
	}
	else
	{
		if(sti(PChar.Arena.Odds.Temp.WinnerCount) >= 3)
		{
			Characters[iEnemy].ArenaOdds.looser = true;
			ArenaOddsDuelEnd(true);
		}
		else
		{
			ArenaOddsSetDetailsForChar(&Characters[iAttack], sti(PChar.Arena.Odds.Temp.Stage));
			ArenaOddsSetDetailsForChar(&Characters[iEnemy], sti(PChar.Arena.Odds.Temp.Stage));
			ArenaOddsStartNewBattle();
		}
	}
}

void ArenaOddsDuelEnd(bool bWin)
{
	LAi_Fade("", "");

	ref looser;
	string sEnemy = PChar.Arena.Odds.Temp.Enemy;
	string sAttack = PChar.Arena.Odds.Temp.Attack;

	int iEnemy = GetCharacterIndex(sEnemy);
	int iAttack = GetCharacterIndex(sAttack);

	ref enemy, attack;
	if(CheckAttribute(&Characters[iAttack], "ArenaOdds.looser"))
	{
		looser = &Characters[iAttack];
		PChar.Arena.Odds.Temp.Winner = Characters[iEnemy].id;
	}

	if(CheckAttribute(&Characters[iEnemy], "ArenaOdds.looser"))
	{
		looser = &Characters[iEnemy];
		PChar.Arena.Odds.Temp.Winner = Characters[iAttack].id;
	}

	if(PChar.Arena.Mode.NotOdd == false)
	{
	}

	int iStage = 0;

	if(PChar.Arena.Mode.NotOdd == false)
	{
		int iActionStage = sti(PChar.Arena.Odds.ActionFight);
		int iActionCharacter = sti(PChar.Arena.Odds.Character);
		if(iActionStage == sti(PChar.Arena.Odds.Temp.Stage))
		{
			if(PChar.Arena.Odds.Temp.Winner == Characters[iActionCharacter].id)
			{
				PChar.Arena.Odds.Win = true;
				Log_InfoS(GetConvertStrWithReplace("Variable_Arena_20", "Logs.txt", "#space#", " ") + GetFullName(Characters[iActionCharacter]) + GetConvertStrWithReplace("Variable_Arena_21", "Logs.txt", "#space#", " ") + sti(PChar.Arena.Odds.Prize) + GetConvertStrWithReplace("Variable_Arena_22", "Logs.txt", "#space#", " "));
				Log_Info(GetConvertStrWithReplace("Variable_Arena_23", "Logs.txt", "#space#", " "));
			}
			else
			{
				PChar.Arena.Odds.Win = false;
				Log_InfoS(GetConvertStrWithReplace("Variable_Arena_24", "Logs.txt", "#space#", " ") + GetFullName(Characters[iActionCharacter]) + GetConvertStrWithReplace("Variable_Arena_25", "Logs.txt", "#space#", " "));
			}
		}
	}

	if(sti(PChar.Arena.Odds.Temp.Stage) == 1)
	{
		PChar.Arena.Odds.Temp.Stage = 2;
		PChar.Arena.Odds.Temp.StagePause = true;
	}

	if(sti(PChar.Arena.Odds.Temp.Stage) == 2 && PChar.Arena.Odds.Temp.StagePause == false)
	{
		PChar.Arena.Odds.Temp.Stage = 3;
		PChar.Arena.Odds.Temp.StagePause = true;
	}

	if(sti(PChar.Arena.Odds.Temp.Stage) == 3 && PChar.Arena.Odds.Temp.StagePause == false)
	{
		ArenaOddsEnd();
		return;
	}

	iStage = sti(PChar.Arena.Odds.Temp.Stage);
	ArenaOddsGoStart(iStage);
	ArenaOddsStartNewBattle();

	if(iStage == 2 || iStage == 3)
	{
		PChar.Arena.Odds.Temp.StagePause = false;
	}
}

void ArenaOddsEnd()
{
	LAi_Fade("", "");

	locCameraFollow();

	bool bWinner = PChar.Arena.Odds.Win;

	if(PChar.Arena.Mode.NotOdd == false)
	{
		PChar.Statistic.Arena.Count = sti(PChar.Statistic.Arena.Count) + 1;
		if(!bWinner)
		{
			PChar.Statistic.Arena.LooserCount = sti(PChar.Statistic.Arena.LooserCount) + 1;
			AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Arena_45", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_46", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_47", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Arena_48", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + " "+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_49", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_50", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_51", "AddRumours.txt", "#space#", " ")+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_52", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_53", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_54", "AddRumours.txt", "#space#", " "), 5, 1);
		}
		else
		{
			PChar.Statistic.Arena.WinnerCount = sti(PChar.Statistic.Arena.WinnerCount) + 1;
			AddSimpleRumourToAllNations(LinkRandPhrase(GetConvertStrWithReplace("Variable_Arena_55", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_56", "AddRumours.txt", "#space#", " "), GetConvertStrWithReplace("Variable_Arena_57", "AddRumours.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_Arena_58", "AddRumours.txt", "#space#", " ") + GetFullName(PChar) + "  "+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_59", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_60", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_61", "AddRumours.txt", "#space#", " ")+ GetSexPhrase(GetConvertStrWithReplace("Variable_Arena_62", "AddRumours.txt", "#space#", " "),GetConvertStrWithReplace("Variable_Arena_63", "AddRumours.txt", "#space#", " ")) +GetConvertStrWithReplace("Variable_Arena_64", "AddRumours.txt", "#space#", " "), 5, 1);
		}
	}

	if(PChar.Arena.Mode.NotOdd == false)
	{
		Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_9";
	}
	else
	{
		Characters[GetCharacterIndex("FencingTown_Head")].dialog.currentnode = "FightInArena_11";
	}

	bDisableCharacterMenu = false;
	PChar.model = PChar.TmpModel;
	SetNewModelToChar(PChar);

	InterfaceStates.Buttons.Save.enable = true;
	ChangeCharacterAddressGroup(PChar, "FencingTown_Arena", "reload", "bashnya_1");
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Arena", "goto", "arena_head");
	LAi_SetActorType(PChar);
	LAi_SetActorType(CharacterFromID("FencingTown_Head"));
	LAi_ActorWaitDialog(CharacterFromID("FencingTown_Head"), PChar);
	LAi_ActorDialog(PChar, CharacterFromID("FencingTown_Head"), "", 25.0, 1.0);

	Locations[FindLocation("FencingTown_Arena")].locators_radius.item.looser_detector = 0.5;
}

void ArenaOddsTheEnd()
{
	InterfaceStates.Buttons.Save.enable = true;
	LAi_SetPlayerType(PChar);

	string sSaber = "blade";
	string sMember = "";

	ref member;
	int iStage = 1;
	int iCount = 1;
	for(int i = 1; i <= 6; i++)
	{
		switch(i)
		{
			case 1: iStage = 1; iCount = 1; break; case 2: iStage = 1; iCount = 2; break;
			case 3: iStage = 2; iCount = 1; break; case 4: iStage = 2; iCount = 2; break;
			case 5: iStage = 3; iCount = 1; break; case 6: iStage = 3; iCount = 2; break;
		}

		sMember = "Arena_Odds_Duel_" + iStage + "_Character_" + iCount;
		member = &Characters[GetCharacterIndex(sMember)];

		RemoveCharacterEquip(member, BLADE_ITEM_TYPE);

		if(CheckAttribute(member, "ArenaAction"))
		{
			DeleteAttribute(member, "ArenaAction");
		}

		if(CheckAttribute(member, "ArenaOdds.looser"))
		{
			DeleteAttribute(member, "ArenaOdds.looser");
		}

		LAi_SetStayType(member);
		ChangeCharacterAddress(member, "None", "");
		member.LifeDay = 0;
		Log_TestInfo(GetConvertStrWithReplace("Variable_Arena_26", "Logs.txt", "#space#", " ") + sMember);
	}

	LAi_group_Delete("ARENA_ODDS_CHARACTER_1");
	LAi_group_Delete("ARENA_ODDS_CHARACTER_2");

	LAi_SetOwnerType(CharacterFromID("FencingTown_Head"));
	ChangeCharacterAddressGroup(CharacterFromID("FencingTown_Head"), "FencingTown_Townhall", "goto", "goto2");

	DoQuestReloadToLocation("FencingTown_ExitTown", "reload", "reload4", "");

	if(CheckAttribute(PChar, "ArenaAction"))
	{
		DeleteAttribute(PChar, "ArenaAction");
	}

	if(CheckAttribute(PChar, "ArenaOdds.looser"))
	{
		DeleteAttribute(PChar, "ArenaOdds.looser");
	}

	if(CheckAttribute(PChar, "Arena.Odds"))
	{
		DeleteAttribute(PChar, "Arena.Odds");
	}

	ClearArena("");
}

void ArenaOddsSetDetailsForChar(ref chr, int iStage)
{
	float Energy = LAi_GetCharacterMaxEnergy(chr);
	chr.chr_ai.energy = stf(Energy);
	chr.chr_ai.energyMax = stf(Energy);
	SetEnergyToCharacter(chr);
	LAi_SetCurHPMax(chr);

	SetCharacterPerk(chr, "Energaiser");
	SetCharacterPerk(chr, "SwordplayProfessional");
	SetCharacterPerk(chr, "AdvancedDefense");
	SetCharacterPerk(chr, "CriticalHit");
	SetCharacterPerk(chr, "Sliding");

	string sSaber = "blade1";

	switch(iStage)
	{
		case 1: sSaber = PChar.Arena.Odds.Saber.Light; break;
		case 2: sSaber = PChar.Arena.Odds.Saber.Saber; break;
		case 3: sSaber = PChar.Arena.Odds.Saber.Heavy; break;
	}

	RemoveCharacterEquip(chr, GUN_ITEM_TYPE);
	RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);

	GiveItem2Character(chr, sSaber);
	EquipCharacterByItem(chr, sSaber);
}

void ArenaOddsSetCharacterFromArenaToLoc()
{
	ref attack, enemy;

	string sAttack = PChar.Arena.Odds.Temp.Attack;
	string sEnemy = PChar.Arena.Odds.Temp.Enemy;

	attack = &Characters[GetCharacterIndex(sAttack)];
	enemy = &Characters[GetCharacterIndex(sEnemy)];

	DeleteAttribute(attack, "ArenaOdds.Attack");
	DeleteAttribute(attack, "ArenaOdds.Enemy");

	DeleteAttribute(enemy, "ArenaOdds.Attack");
	DeleteAttribute(enemy, "ArenaOdds.Enemy");

	ArenaOddsSetCharacterFromArena(attack);
	ArenaOddsSetCharacterFromArena(enemy);
}

void ArenaOddsSetCharacterFromArena(ref chr)
{
	string sLocator = "";

	if(chr.id == "Arena_Odds_Duel_1_Character_1") { sLocator = "arena_member_1"; }
	if(chr.id == "Arena_Odds_Duel_1_Character_2") { sLocator = "arena_member_2"; }
	if(chr.id == "Arena_Odds_Duel_2_Character_1") { sLocator = "arena_member_3"; }
	if(chr.id == "Arena_Odds_Duel_2_Character_2") { sLocator = "arena_member_4"; }
	if(chr.id == "Arena_Odds_Duel_3_Character_1") { sLocator = "arena_member_5"; }
	if(chr.id == "Arena_Odds_Duel_3_Character_2") { sLocator = "arena_member_6"; }

	ChangeCharacterAddressGroup(chr, "FencingTown_Arena", "goto", sLocator);
	LAi_group_MoveCharacter(chr, "ARENA_ODDS_NONE_ACTION");

	LAi_SetActorType(chr);
	LAi_ActorTurnToLocator(chr, "item", "looser_detector");
}

string GetCharacterArenaOddsFromStage(int iStage, int iNum)
{
	string sChar = "Arena_Odds_Duel_" + iStage + "_Character_1";

	string sFight = "Fight_" + iStage;
	string sFightChar = "Char_" + iNum;

	if(CheckAttribute(PChar, "Arena.Odds." + sFight + "." + sFightChar))
	{
		sChar = PChar.Arena.Odds.(sFight).(sFightChar);
	}

	return sChar;
}

void GenerateArenaOddsDuel(int iCount)
{
	ref chr;
	int iChar = 0;
	string sChar, sModel;

	string sFight = "Fight_" + iCount;
	string sFightChar = "";
	for(int i=1; i <= 2; i++)
	{
		iChar = NPC_GenerateCharacter("Arena_Odds_Duel_" + iCount + "_Character_" + i, "officer_"+(rand(42)+1), "man", "man", GetRank(PChar, MOD_SKILL_ENEMY_RATE), PIRATE, -1, true);

		chr = &Characters[iChar];
		DeleteAttribute(chr, "items");
		chr.items = "";

		sChar = "Arena_Odds_Duel_" + iCount + "_Character_" + i;

		SetRandomNameToCharacter(chr);
		ArenaOddsSetCharacterDetails(chr);

		sFightChar = "Char_" + i;
		PChar.Arena.Odds.(sFight).(sFightChar) = sChar;
	}

	DeleteAttribute(PChar, "Arena.Odds.Models");
}

void ArenaOddsSetCharacterDetails(ref chr)
{
	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE);
	int iFencingLight = sti(PChar.skill.FencingLight);
	int iFencing = sti(PChar.skill.Fencing);
	int iFencingHeavy = sti(PChar.skill.FencingHeavy);

	int rand_1 = rand(9)+1;
	int rand_2 = rand(9)+1;

	rand_1 += rand_2;

	float HP = 200 + (rand(20) * rand_1);
	if(HP > 1000)
	{
		HP = 1000;
	}
	LAi_SetHP(chr, HP, HP);

	float Energy = LAi_GetCharacterMaxEnergy(chr);

	chr.chr_ai.energy = stf(Energy);
	chr.chr_ai.energyMax = stf(Energy);

	int iSkill = GetArenaOddsGreatestFencingSkill(chr);

	iFencingLight += 20 + rand(50);
	iFencing +=  20 + rand(50);
	iFencingHeavy += 20 + rand(50);

	if(iFencingLight > 100) { iFencingLight = 100 - rand(30); }
	if(iFencingLight < 30) { iFencingLight = 30 + rand(30); }

	if(iFencing > 100) { iFencing = 100 - rand(30); }
	if(iFencing < 30) { iFencing = 30 + rand(30); }

	if(iFencingHeavy > 100) { iFencingHeavy = 100 - rand(30); }
	if(iFencingHeavy < 30) { iFencingHeavy = 30 + rand(30); }

	chr.skill.FencingLight = iFencingLight;
	chr.skill.Fencing = iFencing;
	chr.skill.FencingHeavy = iFencingHeavy;

	float fRate = GetArenaOddsRateForCharacter(chr);
	chr.Arena.Odds.Rate = fRate;
}

float GetArenaOddsRateForCharacter(ref chr)
{
	float fRate = 10.0;
	float HP = LAi_GetCharacterMaxHP(chr);

	HP /= 80;
	fRate -= HP;

	int iSkill = GetArenaOddsGreatestFencingSkill(chr);
	iSkill *= 10;
	iSkill /= 100;

	fRate -= stf(iSkill);

	int iRandBonus = rand(3)+1;
	float fBonus = 1.1 * (iRandBonus);

	fRate += fBonus;

	if(fRate > 15.0) { fRate = 15.0; }
	if(fRate < 1.3) { fRate = 1.3; }

	return fRate;
}

int GetArenaOddsGreatestFencingSkill(ref chr)
{
	int iFencingLight = sti(chr.skill.FencingLight);
	int iFencing = sti(chr.skill.Fencing);
	int iFencingHeavy = sti(chr.skill.FencingHeavy);

	if(iFencingLight >= iFencing && iFencingLight >= iFencingHeavy)
	{
		return iFencingLight;
	}

	if(iFencing >= iFencingLight && iFencing >= iFencingHeavy)
	{
		return iFencing;
	}

	if(iFencingHeavy >= iFencing && iFencingHeavy >= iFencingLight)
	{
		return iFencingHeavy;
	}

	return iFencing;
}

int GetArenaOddsMaxOdd() // LEO: Параметры ставок - изимани скейл с ростом уровня
{
	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE);
	int iOdd = iRank * ((20 + rand(20)) * sti(PChar.rank)) + rand(1000);

	if(iOdd < 100) { iOdd = 100; }

	return iOdd;
}

string GetModelForCharacterInOdds()
{
	string sModel = "";
	int iRand = rand(19) + 1;

	for(int i=1; i <= 20; i++)
	{
		sModel = "officer_" + i;

		if(iRand == i)
		{
			if(!CheckAttribute(PChar, "Arena.Odds.Models." + sModel))
			{
				PChar.Arena.Odds.Models.(sModel) = true;
				return sModel;
			}
			else
			{
				i = 0;
				iRand = rand(19)+1;
				continue;
			}
		}
	}
	return "officer_" + (rand(42)+1);
}

//-------------------------------------------------------------------------------
// other functions
//-------------------------------------------------------------------------------
string GetSkillFromSaber(string sSaber)
{
	ref Saber = &Items[FindItem(sSaber)];
	if(CheckAttribute(Saber, "FencingType"))
	{
		return Saber.FencingType;
	}

	return "Fencing";
}

string GetSaberForArena(string sType)
{
	// Примитивным рандомом
	switch(sType)
	{
		case "FencingLight":
			switch(rand(5))
			{
				case 0: return "blade2"; break;
				case 1: return "blade6"; break;
				case 2: return "blade9"; break;
				case 3: return "blade19"; break;
				case 4: return "blade22"; break;
				case 5: return "blade23"; break;
			}
		break;

		case "Fencing":
			switch(rand(8))
			{
				case 0: return "blade1"; break;
				case 1: return "blade4"; break;
				case 2: return "topor3"; break;
				case 3: return "blade3"; break;
				case 4: return "blade7"; break;
				case 5: return "blade12"; break;
				case 6: return "blade18"; break;
				case 7: return "blade31"; break;
				case 8: return "blade25"; break;
			}
		break;

		case "FencingHeavy":
			switch(rand(5))
			{
				case 0: return "blade10"; break;
				case 1: return "blade35"; break;
				case 2: return "blade11"; break;
				case 3: return "topor1"; break;
				case 4: return "blade15"; break;
				case 5: return "blade16"; break;
			}
		break;
	}

	return "blade1";
}

string GetSaberForArenaT(string sType, int LA)
{
	// Примитивным рандомом
	switch(sType)
	{
		case "FencingLight":
			if (LA == 1)
			{
				switch(rand(1))
				{
					case 0: return "blade9"; break;
					case 1: return "blade45"; break;
				}
			}
			if (LA == 2)
			{
				switch(rand(3))
				{
					case 0: return "blade19"; break;
					case 1: return "blade22"; break;
					case 2: return "blade36"; break;
					case 3: return "blade05"; break;
				}
			}
			if (LA == 3)
			{
				switch(rand(3))
				{
					case 0: return "blade23"; break;
					case 1: return "blade27"; break;
					case 2: return "blade38"; break;
					case 3: return "blade26"; break;
				}
			}
		break;

		case "Fencing":
			if (LA == 1)
			{
				switch(rand(3))
				{
					case 0: return "blade7"; break;
					case 1: return "blade12"; break;
					case 2: return "blade18"; break;
					case 3: return "topor_05"; break;
				}
			}
			if (LA == 2)
			{
				switch(rand(4))
				{
					case 0: return "blade31"; break;
					case 1: return "blade34"; break;
					case 2: return "blade47"; break;
					case 3: return "blade40"; break;
					case 4: return "blade46"; break;
				}
			}
			if (LA == 3)
			{
				switch(rand(2))
				{
					case 0: return "blade31"; break;
					case 1: return "blade46"; break;
					case 2: return "blade25"; break;
				}
			}
		break;

		case "FencingHeavy":
			if (LA == 1)
			{
				switch(rand(2))
				{
					case 0: return "blade8"; break;
					case 1: return "blade11"; break;
					case 2: return "blade16"; break;
				}
			}
			if (LA == 2)
			{
				switch(rand(3))
				{
					case 0: return "blade15"; break;
					case 1: return "blade21"; break;
					case 2: return "blade01"; break;
					case 3: return "topor2"; break;
				}
			}
			if (LA == 3)
			{
				switch(rand(3))
				{
					case 0: return "blade13"; break;
					case 1: return "blade20"; break;
					case 2: return "blade33"; break;
					case 3: return "blade28"; break;
				}
			}
		break;
	}

	return "blade1";
}

int GetOddForDuelCharacter(ref chr, int iSkill)
{
	int iSum = 0;
	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE);
	int iLeader = sti(chr.skill.LeaderShip);
	float HP = LAi_GetCharacterMaxHP(chr);

	// --> LEO: Серьёзные ставки против потного противника с антифритайзовской системой финтов - анимка Spy
	HP *= 10 + (sti(PChar.rank)*2);

	iRank *= 30 + (sti(PChar.rank)*6);
	iLeader *= 25 + (sti(PChar.rank)*5);
	iSkill *= 35 + (sti(PChar.rank)*7);

	// <-- LEO
	iSum = iRank + iLeader + iSkill + HP;

	return iSum;
}

void ArenaEtapsGetEtap()
{
	string sType_1, sType_2, sType_3, sType_4, sType_5;
	int iQuantity = 0;
	int iCost = 0;

	for(int e=1; e <= 5; e++)
	{
		switch(e)
		{
			case 1:
				sType_1 = ArenaEtapsGetEtapType(1, "");
				iQuantity = 1;
				iCost = ArenaEtapsGetCostForEtap(sType_1, iQuantity);

				PChar.Arena.Etaps.Etap_1.Type = sType_1;
				PChar.Arena.Etaps.Etap_1.Quantity = iQuantity;
				PChar.Arena.Etaps.Etap_1.Cost = iCost;
			break;

			case 2:
				sType_2 = ArenaEtapsGetEtapType(2, sType_1);
				iQuantity = rand(4) + 1;
				iCost = ArenaEtapsGetCostForEtap(sType_2, iQuantity);

				PChar.Arena.Etaps.Etap_2.Type = sType_2;
				PChar.Arena.Etaps.Etap_2.Quantity = iQuantity;
				PChar.Arena.Etaps.Etap_2.Cost = iCost;
			break;

			case 3:
				sType_3 = ArenaEtapsGetEtapType(3, sType_2);
				iQuantity = rand(4) + 1;
				iCost = ArenaEtapsGetCostForEtap(sType_3, iQuantity);

				PChar.Arena.Etaps.Etap_3.Type = sType_3;
				PChar.Arena.Etaps.Etap_3.Quantity = iQuantity;
				PChar.Arena.Etaps.Etap_3.Cost = iCost;
			break;

			case 4:
				sType_4 = ArenaEtapsGetEtapType(4, sType_3);
				iQuantity = rand(4) + 1;
				iCost = ArenaEtapsGetCostForEtap(sType_4, iQuantity);

				PChar.Arena.Etaps.Etap_4.Type = sType_4;
				PChar.Arena.Etaps.Etap_4.Quantity = iQuantity;
				PChar.Arena.Etaps.Etap_4.Cost = iCost;
			break;

			case 5:
				sType_5 = ArenaEtapsGetEtapType(5, "");
				iQuantity = 1;
				iCost = ArenaEtapsGetCostForEtap(sType_5, iQuantity);

				PChar.Arena.Etaps.Etap_5.Type = sType_5;
				PChar.Arena.Etaps.Etap_5.Quantity = iQuantity;
				PChar.Arena.Etaps.Etap_5.Cost = iCost;
			break;
		}
	}
}

string ArenaEtapsGetEtapType(int iEtap, string sBack)
{
	if(iEtap == 1)
	{
		switch(rand(3))
		{
			case 0: return "crabBig"; break;
			case 1: return "Skel"; break;
			case 2: return "SkelOld"; break;
			case 3: return "monkey"; break;
		}
	}

	if(iEtap == 2 || iEtap == 3 || iEtap == 4)
	{
		if(sBack == "Skel" || sBack == "SkelOld")
		{
			switch(rand(1))
			{
				case 0: return "crabBig"; break;
				case 1: return "monkey"; break;
			}
		}
		else
		{
			if(sBack == "crabBig")
			{
				switch(rand(2))
				{
					case 0: return "monkey"; break;
					case 1: return "Skel"; break;
					case 2: return "SkelOld"; break;
				}
			}
			else
			{
				if(sBack == "monkey")
				{
					switch(rand(2))
					{
						case 0: return "crabBig"; break;
						case 1: return "Skel"; break;
						case 2: return "SkelOld"; break;
					}
				}
			}
		}
	}

	if(iEtap == 5)
	{
		if(rand(2) > 1)
		{
			return "SkelKing";
		}
		else
		{
			return "crabBigKing";
		}
	}
}

int ArenaEtapsGetCostForEtap(string sType, int iQuantity)
{
	int iRank = GetRank(PChar, 0);
	int iLeader = GetCharacterSkill(PChar, SKILL_LEADERSHIP);
	int iType = 0;
	int iCost = 0;

	iRank *= 100 + (sti(PChar.rank)*2);
	iLeader *= 200 + (sti(PChar.rank)*4);

	switch(sType)
	{
		case "crabBig": iType = 40 + (sti(PChar.rank)*20); break;
		case "Skel": iType = 220 + (sti(PChar.rank)*110); break;
		case "SkelOld": iType = 220 + (sti(PChar.rank)*110); break;
		case "monkey": iType = 55 + (sti(PChar.rank)*25); break;
		case "crabBigKing": iType = 4000 + (sti(PChar.rank)*2000); break;
		case "SkelKing": iType = 5000 + (sti(PChar.rank)*2500); break;
	}

	iCost = (iRank + iLeader + iType) * iQuantity + rand(1000);

	return iCost;
}

void ClearArena(string qName)
{
	// LEO: то же, что в ClearArenaTime - сетку сносят, значит турнир закончен: разыграть и распустить.
	PGG_SimulateTournament();
	PGG_CloseTournamentForAll();
	if(CheckAttribute(PChar, "Arena"))
	{
		DeleteAttribute(PChar, "Arena");
	}
	if(CheckAttribute(PChar, "ArenaAction"))
	{
		DeleteAttribute(PChar, "ArenaAction");
	}
	if(CheckAttribute(PChar, "ArenaEtapsAction"))
	{
		DeleteAttribute(PChar, "ArenaEtapsAction");
	}
	if(CheckAttribute(PChar, "ArenaTournament"))
	{
		DeleteAttribute(PChar, "ArenaTournament");
	}
	if(CheckAttribute(PChar, "ArenaOdds"))
	{
		DeleteAttribute(PChar, "ArenaOdds");
	}

	Locations[FindLocation("FencingTown_ExitTown")].reload.l2.disable = 0;
	Locations[FindLocation("FencingTown_ExitTown")].reload.l3.disable = 0;
	Locations[FindLocation("FencingTown_ExitTown")].reload.l4.disable = 0;

	ClearArenaCharacters();

	PChar.Arena = "none";
	SetArenaTimeWait();
}

void ClearArenaCharacters()
{
	ref member;
	if(GetCharacterIndex("Arena Duel Characer") != -1)
	{
		member = &Characters[GetCharacterIndex("Arena Duel Characer")];
		member.LifeDay = 0;
	}

	int i = 0;

	string sMember = "";
	for(i = 1; i <= 7; i++)
	{
		sMember = "Arena_Tournament_Character_" + i;
		if(GetCharacterIndex(sMember) == -1)
		{
			continue;
		}

		member = CharacterFromID(sMember);
		member.LifeDay = 0;
	}


	int iStage = 1;
	int iCount = 1;
	for(i = 1; i <= 6; i++)
	{
		switch(i)
		{
			case 1: iStage = 1; iCount = 1; break; case 2: iStage = 1; iCount = 2; break;
			case 3: iStage = 2; iCount = 1; break; case 4: iStage = 2; iCount = 2; break;
			case 5: iStage = 3; iCount = 1; break; case 6: iStage = 3; iCount = 2; break;
		}

		sMember = "Arena_Odds_Duel_" + iStage + "_Character_" + iCount;
		if(GetCharacterIndex(sMember) == -1)
		{
			continue;
		}

		member = CharacterFromID(sMember);
		member.LifeDay = 0;
	}
}

void ClearArenaTime(string qName)
{
	// LEO: вот здесь турнир и заканчивается по-настоящему - этот таймер сносит ВСЮ ветку PChar.Arena
	//   вместе с сеткой и списком мест ПГГ. Он взводится в GenerateArena отдельно от таймера
	//   регенерации и часто срабатывает РАНЬШЕ него, поэтому разыгрывать исход надо тут, до удаления,
	//   иначе симуляции достаётся пустое место («сетки нет» в логе).
	PGG_SimulateTournament();
	PGG_CloseTournamentForAll();
	if(CheckAttribute(PChar, "Arena"))
	{
		DeleteAttribute(PChar, "Arena");
	}
	if(CheckAttribute(PChar, "ArenaAction"))
	{
		DeleteAttribute(PChar, "ArenaAction");
	}
	if(CheckAttribute(PChar, "ArenaEtapsAction"))
	{
		DeleteAttribute(PChar, "ArenaEtapsAction");
	}
	if(CheckAttribute(PChar, "ArenaTournament"))
	{
		DeleteAttribute(PChar, "ArenaTournament");
	}
	if(CheckAttribute(PChar, "ArenaOdds"))
	{
		DeleteAttribute(PChar, "ArenaOdds");
	}

	PChar.Arena = "none";
	SetArenaTimeWait();
}

//-------------------------------------------------------------------------------
// Заселяем
//-------------------------------------------------------------------------------
void CreateArenaCitizens(aref loc)
{
	if(!CheckAttribute(loc, "id"))
	{
		return;
	}

	if(loc.id != "FencingTown_Fort")
	{
		return;
	}

	if(isLocationHasCitizens(loc.id))
	{
		return;
	}

	int iCharacter = 0;
	int iNation = PIRATE;

	ref chr;

	string sModels[12];

	sModels[0] = "";
	sModels[1] = "officer_12";
	sModels[2] = "officer_13";
	sModels[3] = "officer_11";
	sModels[4] = "officer_4";
	sModels[5] = "officer_5";
	sModels[6] = "officer_16";
	sModels[7] = "officer_7";
	sModels[8] = "officer_18";
	sModels[9] = "officer_9";
	sModels[10] = "officer_1";
	sModels[11] = "officer_10";

	int iRank = GetRank(PChar, MOD_SKILL_ENEMY_RATE);
	int iQuantity = rand(5)+6;

	for(int i=1; i <= iQuantity; i++)
	{
		iCharacter = NPC_GenerateCharacter("FencingTown_Citizen_" + i, sModels[i], "man", "man", iRank, iNation, 2, true);
		chr = &Characters[iCharacter];

		SetFantomParamFromRank(chr, iRank, true);
		chr.Dialog.FileName = "FencingTown_dialog.c";
		chr.Dialog.CurrentNode = "First Time";

		if(rand(1) == 1)
		{
			chr.greeting = "cit_common1";
		}
		else
		{
			chr.greeting = "cit_common";
		}

		chr.quest.meeting = "0";
		chr.CityType = "FencingTown";
		PlaceCharacter(chr, "goto", "random");
		LAi_SetCitizenType(chr);
		LAi_group_MoveCharacter(chr, "PIRATE_CITIZENS");
		SetFoodToCharacter(chr, 3, 20);
	}

	if(GetCharacterIndex("FencingTown_Head") == -1)
	{
		SetFencingTownHead();
	}
}

void SetFencingTownHead()
{
	int iRank = 20 + rand(15);

	int iChar = NPC_GenerateCharacter("FencingTown_Head", "Hobart", "man", "man", iRank, PIRATE, -1, true);
	ref chr = &Characters[iChar];

	chr.dialog.Filename = "FencingTownHead_dialog.c";
	chr.dialog.currentnode = "First Time";
	chr.greeting = "Gr_ArenaHead";
	LAi_SetOwnerType(chr);
	LAi_SetImmortal(chr, true);
	LAi_group_MoveCharacter(chr, "PIRATE_CITIZENS");
	ChangeCharacterAddressGroup(chr, "FencingTown_Townhall", "goto", "goto2");
	SetFantomParamFromRank(chr, iRank, true);
	SetCharacterPerk(chr, "Energaiser");
	SetCharacterPerk(chr, "SwordplayProfessional");
}

//-------------------------------------------------------------------------------
// Оформление
//-------------------------------------------------------------------------------

// Огонь
void CreateFireInArena()
{
	float x, y, z;

	int n = 0;
	int step = 0;

	// Первая часть
	x = -1.948;
	y = 15.762;
	z = 2.95;

	step = 42;
	for(n = 1; n <= step; n++)
	{
		x += 0.1;
		CreateParticleSystem("fire", x, y, z,-1.57,0,0,0);
	}

	// Вторая часть
	x = 2.3047;
	z = 3.3012;

	step = 40;
	for(n = 1; n <= step; n++)
	{
		z += 0.1;
		CreateParticleSystem("fire", x, y, z,-1.57,0,0,0);
	}

	// Третья часть
	x = 2.5038;
	z = 7.1013;

	step = 40;
	for(n = 1; n <= step; n++)
	{
		x += 0.1;
		CreateParticleSystem("fire", x, y, z,-1.57,0,0,0);
	}

	// Четвёртая часть
	x = 6.6021;
	z = 6.8021;

	step = 123;
	for(n = 1; n <= step; n++)
	{
		z -= 0.1;
		CreateParticleSystem("fire", x, y, z,-1.57,0,0,0);
	}

	// Пятая часть
	x = 6.4805;
	z = -5.1131;

	step = 130;
	for(n = 1; n <= step; n++)
	{
		x -= 0.1;
		CreateParticleSystem("fire", x, y, z,-1.57,0,0,0);
	}

	// Шестая часть
	x = -6.4291;
	z = -5.053;

	step = 126;
	for(n = 1; n <= step; n++)
	{
		z += 0.1;
		CreateParticleSystem("fire", x, y, z,-1.57,0,0,0);
	}

	// Седьмая часть
	x = -6.2873;
	z = 7.0759;

	step = 40;
	for(n = 1; n <= step; n++)
	{
		x += 0.1;
		CreateParticleSystem("fire", x, y, z,-1.57,0,0,0);
	}

	// Восьмая часть
	x = -2.1548;
	z = 7.0156;

	step = 40;
	for(n = 1; n <= step; n++)
	{
		z -= 0.1;
		CreateParticleSystem("fire", x, y, z,-1.57,0,0,0);
	}

	// "Фонарики"
	x = -4.3352; y = 17.887; z = -4.6905;
	CreateParticleSystem("torch", x, y, z,-1.57,0,0,0);

	x = 4.4049;
	CreateParticleSystem("torch", x, y, z,-1.57,0,0,0);
}

void CreateArenaFightPeople()
{
	int iCitizenQuantity = rand(10) + 13;
	int iTraderQuantity = rand(10) + 8;

	ref chr;
	string sType, sLocator, sAnimation;
	for(int i=1; i <= iCitizenQuantity; i++)
	{
		switch(rand(2))
		{
			case 0: sType = "pirate"; break;
			case 1: sType = "citizen"; break;
			case 2: sType = "shipowner"; break;
		}

		if(rand(1) == 0)
		{
			sAnimation = "Sit_Idle02";
		}
		else
		{
			sAnimation = "Sit2_Idle0" + (rand(6)+1);
		}

		chr = GetCharacter(NPC_GeneratePhantomCharacter(sType, PIRATE, MAN, 0));
		RemoveCharacterEquip(chr, GUN_ITEM_TYPE);
		RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);

		sLocator = GetLocatorForCitizenInArenaFight();
		ChangeCharacterAddressGroup(chr, PChar.location, "sit", sLocator);

		chr.nonTable = true;

		LAi_SetImmortal(chr, true);
		LAi_SetActorTypeNoGroup(chr);
		LAi_ActorSetSitMode(chr);
		LAi_ActorTurnToLocator(chr, "goto", "wall_player_1");
		LAi_ActorAnimation(chr, sAnimation, "", 5.0);

		LAi_group_MoveCharacter(chr, "PIRATE_CITIZENS");
	}

	for(int n=1; n <= iTraderQuantity; n++)
	{
		switch(rand(1))
		{
			case 0: sType = "trader"; break;
			case 1: sType = "usurer"; break;
		}

		sAnimation = "idle_" + (rand(10)+1);

		chr = GetCharacter(NPC_GeneratePhantomCharacter(sType, PIRATE, MAN, 0));
		RemoveCharacterEquip(chr, GUN_ITEM_TYPE);
		RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);

		sLocator = GetLocatorForTraderInArenaFight();
		ChangeCharacterAddressGroup(chr, PChar.location, "goto", sLocator);

		chr.nonTable = true;
		LAi_SetImmortal(chr, true);
		LAi_SetActorTypeNoGroup(chr);
		LAi_ActorTurnToLocator(chr, "goto", "wall_player_1");
		LAi_ActorAnimation(chr, sAnimation, "", 5.0);

		LAi_group_MoveCharacter(chr, "PIRATE_CITIZENS");
	}
}

string GetLocatorForCitizenInArenaFight()
{
	int iRand = rand(117)+1;
	string sLocator = "";

	for(int i=1; i <= 118; i++)
	{
		if(i == iRand)
		{
			sLocator = "arena_sit_" + i;
			if(!CheckAttribute(PChar, "Arena.Locators." + sLocator))
			{
				PChar.Arena.Locators.(sLocator) = "use";
				return sLocator;
			}
			else
			{
				iRand = rand(117)+1;
				i = 1;
			}
		}
	}
	return "arena_sit_1";
}

string GetLocatorForTraderInArenaFight()
{
	int iRand = rand(90)+1;
	string sLocator = "";

	for(int i=1; i <= 91; i++)
	{
		if(i == iRand)
		{
			sLocator = "arena_stay_" + i;
			if(!CheckAttribute(PChar, "Arena.Locators." + sLocator))
			{
				PChar.Arena.Locators.(sLocator) = "use";
				return sLocator;
			}
			else
			{
				iRand = rand(90)+1;
				i = 1;
			}
		}
	}
	return "arena_stay_1";
}

///////////////////////////////////////////////////////////////////////////////////
// OTHER FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////
void CheckAcademyAndOtherLoosers(ref chr)
{
	bool ok = CheckAttribute(chr, "LandAcademy") || CheckAttribute(chr, "ArenaAction") || CheckAttribute(chr, "ArenaEtapsAction") || CheckAttribute(chr, "ArenaTournament") || CheckAttribute(chr, "ArenaOdds");
	if(ok)
	{
		if(CheckAttribute(chr, "LandAcademy"))
		{
			AcademyLandSetLooser(chr);
			return;
		}
		if(CheckAttribute(chr, "ArenaAction"))
		{
			ArenaDuelSetLooser(chr);
			return;
		}
		if(CheckAttribute(chr, "ArenaEtapsAction"))
		{
			ArenaEtapsSetLooser(chr);
			return;
		}
		if(CheckAttribute(chr, "ArenaTournament"))
		{
			ArenaTournamentSetLooser(chr);
			return;
		}
		if(CheckAttribute(chr, "ArenaOdds"))
		{
			ArenaOddsSetLooser(chr);
			return;
		}
	}
}

void SetArenaTimeWait()
{
	int d = rand(5) + 20;
	SetTimerFunction("ArenaReGenerate", 0, 0, d);

	Log_TestInfo(GetConvertStrWithReplace("Variable_Arena_27", "Logs.txt", "#space#", " ") + d + GetConvertStrWithReplace("Variable_Arena_28", "Logs.txt", "#space#", " "));
	AddSimpleRumourToAllNations(GetConvertStrWithReplace("Variable_Arena_65", "AddRumours.txt", "#space#", " "), 5, 1);
}

void ArenaReGenerate(string qName)
{
	GenerateArena();
	Log_TestInfo(GetConvertStrWithReplace("Variable_Arena_29", "Logs.txt", "#space#", " "));
	AddSimpleRumourToAllNations(GetConvertStrWithReplace("Variable_Arena_66", "AddRumours.txt", "#space#", " "), 5, 1);
	// LEO: заводим таймер на следующий цикл. Раньше он ставился ТОЛЬКО когда игрок уходил с арены, поэтому
	//   без его визитов жизнь на Доминике замирала: турнир не обновлялся, а приехавшие конкуренты ждали
	//   впустую и уходили по 30-дневному предохранителю. Теперь турниры идут своим чередом.
	SetTimerFunction("ArenaReGenerate", 0, 0, rand(5) + 20);
}

void ArenaBileterGiveItems(ref chr)
{
	int d = 1;
	int i = 0;

	string sItem = "Food1";
	for(i=1; i<=5; i++)
	{
		sItem = "Food" + i;
		d = (20 / i) + rand(5);
		TakeNItems(chr, sItem, d);
	}

	for(i=1; i<=5; i++)
	{
		sItem = "potion" + i;
		d = (20 / i) + rand(5);
		TakeNItems(chr, sItem, d);
	}
}

