// Gregg: Captain Mowgli - quest "The Monkey Ship" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\captainmonkey.c
// Nodes 1-2 - the first meeting in the grotto, 3-4 - the talk after the hopeless
// fight (these are new: in the original the player simply lost and was stripped),
// 5-8 - the cabin of the "Banana Paradise" during the final boarding.

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
			NextDiag.TempNode = "First time";
			dialog.text = "What are you staring at?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Hey there, friend, what wind blew you in here? Well, what are you staring at? You look as though you had never seen a talking monkey in your life.";
			link.l1 = "Frankly - I have not. Never had the pleasure: neither monkey nor elephant... Hard to believe - not even a giraffe!";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Making jokes, are you? And at captain Mowgli's expense! Now you shall learn what that means, a thousand rotten bananas!";
			link.l1 = "?!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyBattleWithCaptainInGrot");
		break;

		// Gregg: reached by lasting forty-five seconds against him, or by dropping
		// to five hit points - whichever comes first. Blade away, and a talk.
		case "3":
			dialog.text = "Had your fill? You may wave that bit of iron about until the second coming - it will not take me. Put it away before you hurt yourself.";
			link.l1 = "What in the devil's name are you? I put everything I had into those blows, and you are not even out of breath.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Me? I am captain Mowgli, and I cannot be killed. Not at all. And you sit here a while and think on your conduct - my lads will keep an eye on you. Perhaps you will grow wiser.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyReloadInGrot");
		break;

		case "5":
			dialog.text = "You are the first man I shall have to best twice! However did you get here, you leaky coconut?";
			link.l1 = "I shall kill you, Mowgli.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Hope springs eternal in the young! I fear disappointment awaits you once more.";
			link.l1 = "I shall kill you, Mowgli.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "You shall never best me! You do not understand who you are dealing with.";
			link.l1 = "Better than you think. Your contract with the demon has turned to a handful of ash. Fine news, is it not?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Devil take it, how did you get to it?! That complicates matters somewhat... But all the same, you shall be killed!";
			link.l1 = "No. It is I who shall kill you, Mowgli.";
			link.l1.go = "exit";
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattleNoParam");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
