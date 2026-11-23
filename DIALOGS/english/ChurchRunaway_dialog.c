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
			dialog.text = "Please, keep your voice down... Yes, I fled the cloister. I could not bear the monastery walls - fear and doubt overcame me. Did Father " + PChar.GenQuest.ChurchRunaway.PriestName + " send you after me?";
			link.l1 = "Your place is in the cloister, brother. Return and do your duty before God.";
			link.l1.go = "ChurchRunaway_Persuade";
			link.l2 = "Every man is free to choose his own path. Go in peace, I will not hold you.";
			link.l2.go = "ChurchRunaway_Release";
			link.l3 = "Enough whining. You're coming with me - willing or bound.";
			link.l3.go = "ChurchRunaway_Force";
		break;

		case "ChurchRunaway_Persuade":
			if (rand(100) < 30 + sti(GetSummonSkillFromName(pchar, SKILL_LEADERSHIP)))
			{
				dialog.text = "Your words... they have restored my courage. You are right, I was weak. I will return and accept my penance. Thank you for guiding me back to the true path.";
				link.l1 = "Go, brother. You are awaited.";
				link.l1.go = "ChurchRunaway_Done_Persuade";
			}
			else
			{
				dialog.text = "No... you do not understand! I cannot go back. Better poverty and wandering than those cold walls again!";
				link.l1 = "Very well, I will not force you. Go in peace.";
				link.l1.go = "ChurchRunaway_Release";
				link.l2 = "No one asked for your consent. You're coming with me.";
				link.l2.go = "ChurchRunaway_Force";
			}
		break;

		case "ChurchRunaway_Release":
			dialog.text = "May God bless you for your kindness! I will never forget this. Farewell...";
			link.l1 = "Farewell. And may the Lord keep you.";
			link.l1.go = "ChurchRunaway_Done_Release";
		break;

		case "ChurchRunaway_Force":
			dialog.text = "No! Let go!.. Fine, fine, have it your way. I will go. But God sees what is in my heart.";
			link.l1 = "Walk. And no tricks.";
			link.l1.go = "ChurchRunaway_Done_Force";
		break;

		case "ChurchRunaway_Done_Persuade":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			PChar.GenQuest.ChurchRunaway.Outcome = "persuade";
			DeleteAttribute(PChar, "quest.ChurchRunaway_SpawnMonk");
			characters[GetCharacterIndex("ChurchRunaway_Monk")].LifeDay = 0;
			sTitle = PChar.GenQuest.ChurchRunaway.QuestTown + "ChurchRunaway";
			AddQuestRecordEx(sTitle, "ChurchRunaway", "2");
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
			AddQuestRecordEx(sTitle, "ChurchRunaway", "2");
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
			AddQuestRecordEx(sTitle, "ChurchRunaway", "2");
			AddQuestUserData(sTitle, "sMonk", PChar.GenQuest.ChurchRunaway.MonkName);
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + PChar.GenQuest.ChurchRunaway.QuestTown + "Gen"));
		break;
	}
}
