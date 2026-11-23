// Gregg: Ralph Vatterson and his wife Emily - quest "The Monkey Ship" (ported from
// Damned Destiny). Logic - Program\Quests\reaction_functions\captainmonkey.c
// Nodes 1-3 - the rescued prisoner in the grotto, 4-6 - the couple's meeting by the
// bank, 7 - the failure branch, if Allen reached the money first.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sLocation = PChar.CaptainMonkey.BankID;
	sLocation = XI_ConvertString("Colony" + sLocation + "Dat");

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Captain, I do not even know how to thank you...";
			link.l1 = "No thanks are needed. Not long ago I was literally in your place myself, and got free by a miracle. What did they want of you?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Money, my money - what else. They made me write a letter to my wife, Emily, asking her to draw all our savings from the moneylender and hand them to that scoundrel Gladstone. Otherwise Allen promised me a long and painful death. He is already on his way, I suppose. My house is in " + sLocation + ".";
			link.l1 = "When did you give him the letter?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Yesterday morning, sir.";
			link.l1 = "We may still be in time. Follow me to my ship.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyToBankColony");
		break;

		case "4":
			dialog.text = "My dear, by what miracle are you here? The letter was handed to me this morning, I was just about to do as you asked...";
			link.l1 = "...";
			link.l1.go = "5";
		break;

		case "5":
			SetCameraDialogMode(CharacterFromID("Ralph Vatterson"));
			dialog.text = "Oh my dear, how glad I am to see you! I broke free of captivity with the help of this courageous captain, whose name is " + GetFullName(PChar) + ". Tell me, my love, where is that ''gentleman'' you were to give our money to?";
			link.l1 = "...";
			link.l1.go = "6";
		break;

		case "6":
			SetCameraDialogMode(NPChar);
			dialog.text = "The man who handed me the letter said he would wait for me beyond the town gates.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyFromBankToExitTown");
		break;

		case "7":
			dialog.text = "?!...";
			link.l1 = "Forgive me, friend. I did everything I could.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyBankTimeIsLeftClosedQuest");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
