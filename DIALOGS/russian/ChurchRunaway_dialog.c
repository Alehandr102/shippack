// Диалог беглого послушника (церковный квест №3 "Беглый монах")
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sTitle;

	switch(Dialog.CurrentNode)
	{
		case "ChurchRunaway_Monk_Start":
			dialog.text = "Прошу, тише... Да, я сбежал из обители. Не вынес я монастырских стен - страх и сомнения одолели меня. Это отец " + PChar.GenQuest.ChurchRunaway.PriestName + " послал вас за мной?";
			link.l1 = "Твоё место в обители, брат. Вернись и исполни свой долг перед Господом.";
			link.l1.go = "ChurchRunaway_Persuade";
			link.l2 = "Каждый волен выбрать свой путь. Иди с миром, я не держу тебя.";
			link.l2.go = "ChurchRunaway_Release";
			link.l3 = "Хватит ныть. Пойдёшь со мной - добром или связанным.";
			link.l3.go = "ChurchRunaway_Force";
		break;

		case "ChurchRunaway_Persuade":
			if (rand(100) < 30 + sti(GetSummonSkillFromName(pchar, SKILL_LEADERSHIP)))
			{
				dialog.text = "Ваши слова... они вернули мне мужество. Вы правы, я был слаб. Я вернусь и приму епитимью. Благодарю, что наставили меня на путь истинный.";
				link.l1 = "Ступай, брат. Тебя ждут.";
				link.l1.go = "ChurchRunaway_Done_Persuade";
			}
			else
			{
				dialog.text = "Нет... вы не понимаете! Я не могу вернуться. Лучше нищета и скитания, чем снова те холодные стены!";
				link.l1 = "Что ж, неволить не стану. Иди с миром.";
				link.l1.go = "ChurchRunaway_Release";
				link.l2 = "Твоего согласия никто не спрашивал. Идёшь со мной.";
				link.l2.go = "ChurchRunaway_Force";
			}
		break;

		case "ChurchRunaway_Release":
			dialog.text = "Да благословит вас Бог за вашу доброту! Я никогда этого не забуду. Прощайте...";
			link.l1 = "Прощай. И да хранит тебя Господь.";
			link.l1.go = "ChurchRunaway_Done_Release";
		break;

		case "ChurchRunaway_Force":
			dialog.text = "Нет! Пустите!.. Ладно, ладно, будь по-вашему. Я пойду. Но Бог видит, что у меня на сердце.";
			link.l1 = "Шагай. И без глупостей.";
			link.l1.go = "ChurchRunaway_Done_Force";
		break;

		case "ChurchRunaway_Done_Persuade":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			PChar.GenQuest.ChurchRunaway.Outcome = "persuade";
			DeleteAttribute(PChar, "quest.ChurchRunaway_SpawnMonk");
			characters[GetCharacterIndex("ChurchRunaway_Monk")].LifeDay = 0;
			sTitle = PChar.GenQuest.ChurchRunaway.QuestTown + "ChurchRunaway";
			AddQuestRecordEx(sTitle, "ChurchRunaway", "2"); AddQuestUserData(sTitle, "sSex", GetSexPhrase("","а"));
			AddQuestUserData(sTitle, "sMonk", PChar.GenQuest.ChurchRunaway.MonkName);
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + PChar.GenQuest.ChurchRunaway.QuestTown + "Gen"));
		break;

		case "ChurchRunaway_Done_Release":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			PChar.GenQuest.ChurchRunaway.Outcome = "release";
			DeleteAttribute(PChar, "quest.ChurchRunaway_SpawnMonk");
			characters[GetCharacterIndex("ChurchRunaway_Monk")].LifeDay = 0;
			sTitle = PChar.GenQuest.ChurchRunaway.QuestTown + "ChurchRunaway";
			AddQuestRecordEx(sTitle, "ChurchRunaway", "2"); AddQuestUserData(sTitle, "sSex", GetSexPhrase("","а"));
			AddQuestUserData(sTitle, "sMonk", PChar.GenQuest.ChurchRunaway.MonkName);
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + PChar.GenQuest.ChurchRunaway.QuestTown + "Gen"));
		break;

		case "ChurchRunaway_Done_Force":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			PChar.GenQuest.ChurchRunaway.Outcome = "force";
			DeleteAttribute(PChar, "quest.ChurchRunaway_SpawnMonk");
			characters[GetCharacterIndex("ChurchRunaway_Monk")].LifeDay = 0;
			sTitle = PChar.GenQuest.ChurchRunaway.QuestTown + "ChurchRunaway";
			AddQuestRecordEx(sTitle, "ChurchRunaway", "2"); AddQuestUserData(sTitle, "sSex", GetSexPhrase("","а"));
			AddQuestUserData(sTitle, "sMonk", PChar.GenQuest.ChurchRunaway.MonkName);
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + PChar.GenQuest.ChurchRunaway.QuestTown + "Gen"));
		break;
	}
}
