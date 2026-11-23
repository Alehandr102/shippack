// Gregg: Cristobal Kuvalda, a hired axeman - the "Black Orchid" trilogy (ported from
// Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// The headman of the pearl divers' village asks the player to fetch Kuvalda before
// Feregueyro's servants fall upon the village:
//   First time - the stock line;
//   1-2 - at the cave mouth, right after the fight with the Aztecs: talked into going back;
//   3-8 - the village has held; Kuvalda brags about his stroke. All three answers
//         come out at the same place - go and report to the headman.
// In the original the player called the headman Rodrigo, though HeadMan_1 is given a
// random name when generated - here he is simply "the headman".

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sBro = GetSexPhrase("brother", "sister");

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "These are Feregueyro's servants, " + sBro + "! The village must be warned - Feregueyro is back!";
			link.l1 = "The headman has been warned already, they are making ready to defend the place. And I was sent for you: the village needs you.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I know, I was about to run there myself. Devil take it, we must hurry, there is no time at all! Oh, and one last thing. Thanks for the helping hand, " + sBro + ".";
			link.l1 = "Think nothing of it, Cristobal. Time is short - let us run for the village.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftToPearlVillageJungle");
		break;

		case "3":
			dialog.text = "Ha, that was a proper slaughter! Did you see me lay out one of those wretches, " + sBro + "?";
			link.l1 = "I had no time to watch you, I had plenty of my own to deal with.";
			link.l1.go = "4";
			link.l2 = "I did not, though I glanced your way now and then to see how you fared.";
			link.l2.go = "6";
			link.l3 = "Of course I saw it. A fine stroke.";
			link.l3.go = "8";
		break;

		case "4":
			dialog.text = "That is bad, " + sBro + ". In a fight you must keep half an eye on your friends: one of them may need a hand, and then you run and pull him out.";
			link.l1 = "Very well, I shall remember that. I will keep an eye out.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Good enough. Off you go, " + sBro + ", report that we beat them back, and I shall wait for you here.";
			link.l1 = "Very well.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftInPearlVillageToOfficerHouse");
		break;

		case "6":
			dialog.text = "Ah, you should have seen it! One of them comes flying at me, so I swung round like a lad at a fair and gave him the flat of the axe. Meant only to stun him, but I misjudged the blow - and staved his head in.";
			link.l1 = "Practise judging your blow when you mean only to stun a man.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Very well, " + sBro + ", off you go, report that we beat them back, and I shall wait for you here.";
			link.l1 = "Very well.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftInPearlVillageToOfficerHouse");
		break;

		case "8":
			dialog.text = "I was taken aback myself. I meant only to stun him, but I misjudged the blow - and killed him.";
			link.l1 = "Practise judging your blow when you mean only to stun a man.";
			link.l1.go = "7";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
