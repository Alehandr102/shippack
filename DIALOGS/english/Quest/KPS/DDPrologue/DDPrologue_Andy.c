// Gregg: Andy Rejdol - alternative prologue "Desert Island" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\ddprologue.c

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
			NextDiag.TempNode = "First time";
		break;

		case "Start_1":
			dialog.text = PChar.name + "! Alive, no less!";
			link.l1 = "So you see. And you are...";
			link.l1.go = "Start_2";
		break;

		case "Start_2":
			dialog.text = "...(cutting in)... You cunning devil! I knew it, I knew you would not croak, not in that living hell on deck, nor in the sea!";
			link.l1 = "I know the question is strange, but who are you?";
			link.l1.go = "Start_3";
		break;

		case "Start_3":
			dialog.text = "What?! You do not recognise me?";
			link.l1 = "No. After striking my head on a cannon I remember nothing.";
			link.l1.go = "Start_4";
		break;

		case "Start_4":
			dialog.text = GetFullName(NPChar) + "! Faithful boatswain of captain " + GetMainCharacterNameGen() + ". And your memory will come back, captain, I am sure of it. Many caps have lost it for a short while after a hard blow, and then recalled everything.";
			link.l1 = "That is reassuring. Have you been here long, " + NPChar.name + "?";
			link.l1.go = "Start_5";
		break;

		case "Start_5":
			dialog.text = "About an hour. I found this grotto by chance while hiding from Sajfus's cutthroats. Had I not found it, they would have found me and done me in.";
			link.l1 = "Damn, so Sajfus has already landed a party on the island! We must be extremely care... ...(in a whisper)... Quiet! Do you hear that?";
			link.l1.go = "Start_6";
		break;

		case "Start_6":
			dialog.text = "Hear what, " + PChar.name + "?";
			link.l1 = "Someone is coming down here. Into cover, quickly. If those are Sajfus's men, they must not spot us, or we are finished.";
			link.l1.go = "Start_7";
		break;

		case "Start_7":
			dialog.text = "...(in a whisper)... Understood, cap. James, hide.";
			link.l1 = "Come on, faster.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameEnemyInGrot");
		break;

		case "Start_8":
			dialog.text = "Phew, we got away with it. And what do we do next, cap? The island is uninhabited and we have no ship.";
			link.l1 = "First we rest here for two or three days, and then we shall see. Ships call here now and then to take on provisions and fresh water. We shall try to arrange with their captains to carry us to the nearest port.";
			link.l1.go = "Start_9";
		break;

		case "Start_9":
			dialog.text = "And have you anything to pay with, cap? Our pockets are empty.";
			link.l1 = "Hm. True enough, we have nothing to pay with. Have you searched that chest, " + NPChar.name + "?";
			link.l1.go = "Start_10";
		break;

		case "Start_10":
			dialog.text = "And why search it, what would I find in there?";
			link.l1 = "Well, I do not know. Perhaps some captain hid a little gold here against a rainy day. It is worth checking.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameSearchedItems");
		break;

		case "Start_11":
			dialog.text = "Ha! Cap, you were right! There is indeed something worth taking in that chest.";
			link.l1 = "Now we have the fare. All that remains is to wait for some ship. We shall walk down to the shore in turns each day and watch the horizon.";
			link.l1.go = "Start_12";
		break;

		case "Start_12":
			dialog.text = "Very well, captain, so we shall.";
			link.l1 = "And now everyone rest. I have one thing to attend to.";
			link.l1.go = "exit_special";
		break;

		// Gregg: same trick CSP uses in MainHero_dialog.c (Exit_Special) - open the character
		// build screen. The ChangePiratesExit flag is picked up in INTERFACE\character.c
		// when the screen closes, and the prologue continues.
		case "exit_special":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			pchar.SystemInfo.ChangePIRATES = true;
			PChar.StartDamnedDestiny.ChangePiratesExit = true;
			LaunchCharacter(PChar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
