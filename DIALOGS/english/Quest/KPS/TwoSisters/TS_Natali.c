// Gregg: Natali Remis and Ortodokse Malkin - "Two Sisters", the Bermudas branch.
// Logic - Program\Quests\reaction_functions\twosisters.c.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch (Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		// Malkin quarrels with Natali in the street - the hero walks in on the noise
		case "1":
			SetCameraDialogMode(NPChar);
			dialog.text = "...(not noticing the newcomer)... Who needs you, you fool? There are ten like you on every pier, and all of them easier to deal with.";
			link.l1 = "...(stop)...";
			link.l1.go = "2";
		break;

		case "2":
			SetCameraDialogMode(CharacterFromID("Natali Remis"));
			LAi_ActorTurnToCharacter(CharacterFromID("Natali Remis"), CharacterFromID("Ortodokse Malkin"));
			dialog.text = "Natali Remis: Ten, is it. And who slept under my door last month and whined to be let in?";
			link.l1 = "...(listen)...";
			link.l1.go = "3";
		break;

		case "3":
			SetCameraDialogMode(NPChar);
			dialog.text = "Be quiet.";
			link.l1 = "...(listen)...";
			link.l1.go = "4";
		break;

		case "4":
			SetCameraDialogMode(CharacterFromID("Natali Remis"));
			dialog.text = "Natali Remis: I will not. You will not let me out of here, will you. Not to a ship, not to church, nowhere.";
			link.l1 = "...(listen)...";
			link.l1.go = "5";
		break;

		case "5":
			SetCameraDialogMode(NPChar);
			dialog.text = "You are mine. I paid for you, and more than once.";
			link.l1 = "...(listen)...";
			link.l1.go = "6";
		break;

		case "6":
			SetCameraDialogMode(CharacterFromID("Natali Remis"));
			dialog.text = "Natali Remis: Paid, he says. Pay for the wine. For me it does not work any more.";
			link.l1 = "...(listen)...";
			link.l1.go = "7";
		break;

		case "7":
			SetCameraDialogMode(NPChar);
			dialog.text = "...(quietly)... Right. You are coming with me now. On your own two feet.";
			link.l1 = "Step away from her.";
			link.l1.go = "8";
		break;

		case "8":
			SetCameraDialogMode(NPChar);
			dialog.text = "...(turns around)... And who are you to tell me anything? Walk on while you still can.";
			link.l1 = "I am not walking on.";
			link.l1.go = "9";
			link.l2 = "I came for her. She is leaving for San Juan.";
			link.l2.go = "9";
		break;

		case "9":
			SetCameraDialogMode(NPChar);
			dialog.text = "Then I will bury the pair of you.";
			link.l1 = "...(draw your weapon)...";
			link.l1.go = "10";
		break;

		case "10":
			locCameraFollow();
			dialog.text = "...";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersRemisFightWithMalkin");
		break;

		// After the fight
		case "11":
			dialog.text = "...(after a moment)... Thank you. I thought he would take me with him after all.";
			link.l1 = "He will not be taking anyone anywhere.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "He has three of his own on the brig. They know all about me. The moment they come ashore, it starts again.";
			link.l1 = "Then they will not come ashore.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "...(looks at you, unconvinced)... You would board a stranger's ship for me? What for?";
			link.l1 = "I need four women for San Juan. You are the fourth.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "So that is it. And here I was thinking heaven knows what. ...(a wry smile)... San Juan will do. Nothing holds me here anyway.";
			link.l1 = "Then get ready.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "I will. But the brig comes first, or I will not even make it to the pier.";
			link.l1 = "What sort of men are they?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Three. Malkin pulled them off a convict gang, and since then they would hang for him. Big as oxen and about as clever.";
			link.l1 = "Where is the brig?";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Straight past the mole, the boat is by the rocks. One sits in the cabin, one in the wardroom, one in the hold - they do not walk together even ashore.";
			link.l1 = "Wait for me here.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersRemisToShip");
		break;

		// Malkin's men aboard the brig
		case "22":
			dialog.text = "...(getting up)... You are not one of ours. Ours knock.";
			link.l1 = "Your master is dead.";
			link.l1.go = "25";
		break;

		case "23":
			dialog.text = "Heard the noise below? So did I. ...(takes hold of his knife)...";
			link.l1 = "It was quieter below.";
			link.l1.go = "25";
		break;

		case "24":
			dialog.text = "...(squinting in the dark)... Two of them already, then. I am the third and the last, there is nobody after me.";
			link.l1 = "The last it is.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "...";
			link.l1 = "...";
			link.l1.go = "exit";
			if (NPChar.id == "Malkin Sailor 1")
			{
				AddDialogExitQuestFunction("TwoSistersRemisFightWithFirstSailor");
			}
			if (NPChar.id == "Malkin Sailor 2")
			{
				AddDialogExitQuestFunction("TwoSistersRemisFightWithSecondSailor");
			}
			if (NPChar.id == "Malkin Sailor 3")
			{
				AddDialogExitQuestFunction("TwoSistersRemisFightWithThirdSailor");
			}
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
