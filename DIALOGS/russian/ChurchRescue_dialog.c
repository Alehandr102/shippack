// Диалог главаря похитителей (церковный квест №4 "Вызволение монаха")
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	ref rChar;
	int i, iCaptors;
	string sTitle;

	switch(Dialog.CurrentNode)
	{
		case "ChurchRescue_Captor_Start":
			dialog.text = "Стоять! Дальше хода нет. Этот святоша теперь наш товар. Гони " + FindRussianMoneyString(sti(PChar.GenQuest.ChurchRescue.Ransom)) + " - и забирай его. Или ляжешь тут рядом с ним.";
			if (sti(PChar.money) >= sti(PChar.GenQuest.ChurchRescue.Ransom))
			{
				link.l1 = "Вот выкуп. Отпустите брата, и разойдёмся миром.";
				link.l1.go = "ChurchRescue_Captor_Pay";
			}
			link.l2 = "С разбойниками я говорю только сталью. Защищайтесь!";
			link.l2.go = "ChurchRescue_Captor_Fight";
			link.l3 = "Слушай внимательно: за этого монаха горой стоит Святая Инквизиция. Тронете его - и вас сожгут на медленном огне. Отпустите по-хорошему.";
			link.l3.go = "ChurchRescue_Captor_Deceive";
			link.l4 = "Ты хоть знаешь, кто перед тобой? Одно моё слово - и от вашей шайки мокрого места не останется. Отдай монаха, пока цел.";
			link.l4.go = "ChurchRescue_Captor_Threaten";
		break;

		case "ChurchRescue_Captor_Deceive":
			if (rand(100) < 40 + GetCharacterSkill(pchar, "Sneak")) // обман - хитрость (Sneak), лёгкий порог
			{
				PChar.GenQuest.ChurchRescue.Freed = "deceive";
				dialog.text = "Инквизиция?! Чёрт... нам только костра и не хватало. Забирай святошу и проваливайте оба, да поживее, пока я не передумал!";
				link.l1 = "Идём, брат. Ты свободен.";
				link.l1.go = "ChurchRescue_Captor_TalkWin";
			}
			else
			{
				dialog.text = "Инквизиция, говоришь? Складно врёшь, да только мы не лыком шиты. А ну, ребята, проучим лжеца!";
				link.l1 = "Что ж, тогда по-плохому!";
				link.l1.go = "ChurchRescue_Captor_Fight";
			}
		break;

		case "ChurchRescue_Captor_Threaten":
			if (rand(100) < 40 + sti(GetSummonSkillFromName(pchar, SKILL_LEADERSHIP))) // запугать - авторитет (Leadership), лёгкий порог
			{
				PChar.GenQuest.ChurchRescue.Freed = "threaten";
				dialog.text = "А ведь и правда, неохота связываться с таким головорезом... Ладно, забирай монаха. Своя шкура дороже какого-то попа.";
				link.l1 = "Идём, брат. Ты свободен.";
				link.l1.go = "ChurchRescue_Captor_TalkWin";
			}
			else
			{
				dialog.text = "Ха! Пугать он нас вздумал. Нас тут больше, и сталь у нас острая. Взять его!";
				link.l1 = "Значит, придётся силой!";
				link.l1.go = "ChurchRescue_Captor_Fight";
			}
		break;

		case "ChurchRescue_Captor_TalkWin":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			iCaptors = sti(PChar.GenQuest.ChurchRescue.CaptorsCount);
			for(i=0; i<iCaptors; i++)
			{
				rChar = CharacterFromID("ChurchRescue_Captor_" + i);
				LAi_CharacterDisableDialog(rChar);
				rChar.LifeDay = 0;
			}
			LAi_LocationFightDisable(LoadedLocation, false);
			DeleteAttribute(LoadedLocation, "DisableEncounters");
			chrDisableReloadToLocation = false;
			DeleteAttribute(PChar, "GenQuest.ChurchRescue.CaptorsInCave");
			DeleteAttribute(PChar, "quest.ChurchRescue_SpawnCaptors");
			sTitle = PChar.GenQuest.ChurchRescue.QuestTown + "ChurchRescue";
			AddQuestRecordEx(sTitle, "ChurchRescue", "2");
			AddQuestUserData(sTitle, "sMonk", PChar.GenQuest.ChurchRescue.MonkName);
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + PChar.GenQuest.ChurchRescue.QuestTown + "Gen"));
			ChurchRescue_SpawnMonk(); // освобождённый монах выходит поблагодарить
		break;

		case "ChurchRescue_Captor_Pay":
			dialog.text = "Разумный выбор. Деньги вперёд... Эй, тащите сюда монаха! А ты проваливай и не оглядывайся.";
			link.l1 = "Идём, брат. Ты свободен.";
			link.l1.go = "ChurchRescue_Captor_PayDone";
		break;

		case "ChurchRescue_Captor_PayDone":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			AddMoneyToCharacter(PChar, -sti(PChar.GenQuest.ChurchRescue.Ransom));
			iCaptors = sti(PChar.GenQuest.ChurchRescue.CaptorsCount);
			for(i=0; i<iCaptors; i++)
			{
				rChar = CharacterFromID("ChurchRescue_Captor_" + i);
				LAi_CharacterDisableDialog(rChar);
				rChar.LifeDay = 0;
			}
			LAi_LocationFightDisable(LoadedLocation, false); // вернуть возможность боя (SpawnCaptors её запретил)
			DeleteAttribute(LoadedLocation, "DisableEncounters");
			chrDisableReloadToLocation = false;
			PChar.GenQuest.ChurchRescue.Freed = "pay";
			DeleteAttribute(PChar, "GenQuest.ChurchRescue.CaptorsInCave");
			DeleteAttribute(PChar, "quest.ChurchRescue_SpawnCaptors");
			sTitle = PChar.GenQuest.ChurchRescue.QuestTown + "ChurchRescue";
			AddQuestRecordEx(sTitle, "ChurchRescue", "2");
			AddQuestUserData(sTitle, "sMonk", PChar.GenQuest.ChurchRescue.MonkName);
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + PChar.GenQuest.ChurchRescue.QuestTown + "Gen"));
			ChurchRescue_SpawnMonk(); // освобождённый монах выходит поблагодарить
		break;

		case "ChurchRescue_Captor_Fight":
			LAi_LocationFightDisable(LoadedLocation, false); // снова разрешаем бой (SpawnCaptors его запретил)
			AddDialogExitQuest("ChurchRescue_CaptorsEnemies");
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

		case "ChurchRescue_Monk_Thanks":
			dialog.text = "Благослови тебя Господь, спаситель мой! Я уж и не чаял увидеть свет Божий... Поспеши к отцу " + PChar.GenQuest.ChurchRescue.PriestName + ". Он, наверное, уже извёлся от тревоги и ждёт добрых вестей. А я доберусь до обители сам.";
			link.l1 = "Ступай с миром, брат " + PChar.GenQuest.ChurchRescue.MonkName + ".";
			link.l1.go = "ChurchRescue_Monk_Leave";
		break;

		case "ChurchRescue_Monk_Leave":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			rChar = CharacterFromID("ChurchRescue_Monk");
			LAi_CharacterDisableDialog(rChar);
			rChar.LifeDay = 0; // монах уходит в обитель
		break;
	}
}
