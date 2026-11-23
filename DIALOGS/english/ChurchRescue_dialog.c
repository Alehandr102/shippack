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
			dialog.text = "Hold it! No further. This little holy man is our merchandise now. Hand over " + FindRussianMoneyString(sti(PChar.GenQuest.ChurchRescue.Ransom)) + " and take him. Or lie down here beside him.";
			if (sti(PChar.money) >= sti(PChar.GenQuest.ChurchRescue.Ransom))
			{
				link.l1 = "Here is the ransom. Release the brother and we part in peace.";
				link.l1.go = "ChurchRescue_Captor_Pay";
			}
			link.l2 = "With cutthroats I speak only with steel. Defend yourselves!";
			link.l2.go = "ChurchRescue_Captor_Fight";
				link.l3 = "Listen well: the Holy Inquisition stands behind this monk. Touch him, and you'll burn on a slow fire. Let him go quietly.";
				link.l3.go = "ChurchRescue_Captor_Deceive";
				link.l4 = "Do you even know who stands before you? One word from me and there'll be nothing left of your band. Hand over the monk while you still can.";
				link.l4.go = "ChurchRescue_Captor_Threaten";
		break;

		case "ChurchRescue_Captor_Deceive":
			if (rand(100) < 40 + GetCharacterSkill(pchar, "Sneak")) // обман - хитрость (Sneak), лёгкий порог
			{
				PChar.GenQuest.ChurchRescue.Freed = "deceive";
				dialog.text = "The Inquisition?! Damn... a bonfire is the last thing we need. Take the holy man and clear off, both of you, before I change my mind!";
				link.l1 = "Come, brother. You are free.";
				link.l1.go = "ChurchRescue_Captor_TalkWin";
			}
			else
			{
				dialog.text = "The Inquisition, you say? A fine lie, but we weren't born yesterday. Right, lads, let's teach this liar a lesson!";
				link.l1 = "So be it - the hard way!";
				link.l1.go = "ChurchRescue_Captor_Fight";
			}
		break;

		case "ChurchRescue_Captor_Threaten":
			if (rand(100) < 40 + sti(GetSummonSkillFromName(pchar, SKILL_LEADERSHIP))) // запугать - авторитет (Leadership), лёгкий порог
			{
				PChar.GenQuest.ChurchRescue.Freed = "threaten";
				dialog.text = "Aye, on second thought... no sense crossing a cutthroat like you. Fine, take the monk. My own hide is worth more than some priest.";
				link.l1 = "Come, brother. You are free.";
				link.l1.go = "ChurchRescue_Captor_TalkWin";
			}
			else
			{
				dialog.text = "Ha! He thinks to scare us. There are more of us, and our steel is sharp. Get him!";
				link.l1 = "Then it'll have to be by force!";
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
			dialog.text = "A wise choice. Money first... Hey, bring the monk over here! And you - clear off and don't look back.";
			link.l1 = "Come, brother. You are free.";
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
			dialog.text = "God bless you, my saviour! I had given up hope of ever seeing God's light again... Hurry to Father " + PChar.GenQuest.ChurchRescue.PriestName + ", he is probably worry sick and awaits good news. I will make my own way to the cloister.";
			link.l1 = "Go in peace, Brother " + PChar.GenQuest.ChurchRescue.MonkName + ".";
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
