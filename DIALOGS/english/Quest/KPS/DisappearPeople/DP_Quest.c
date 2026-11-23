// Gregg: Bill Matthew and Ben Gregori - quest "The Vanishing" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\disappearpeople.c
// Nodes 1-6 - the rescued navigator's tale, 7-9 - the talk with Gregori in the Barbados cave.

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
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Thank you for helping me, captain - without you they would have killed me and taken the coin.";
			link.l1 = "What coin do you speak of?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Perhaps I should tell you my story. I was once navigator aboard the pirate brig ''Lucky Joker''. One day we decided to hide our haul in the Barbados cave and quite by chance came upon an ancient Inca treasure. Besides the jewels we found a casket holding a vial of some liquid and three old gold coins. None of us had ever seen their like. We divided the haul - one of those coins fell to me, and the captain took the casket for himself.";
			link.l1 = "Hm, and what happened then?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Some time later captain Gregori managed to find a man who could read the inscription on the lid of that casket. It turned out that whoever held its contents could reach the evil Indian spirits by reciting the spell from the lid and mixing his own blood with the liquid from the vial. Our captain did just that - and turned into a mummy, a living skeleton: an evil spirit entered him! What is more, Gregori gained an inexplicable power over his crew, and turned some of the men into the same sort of skeletons with that sorcerous liquid." + NewStr() + "Strangely, his spells had no hold on me - now I understand, it was that old coin that saved me. In time I managed to escape and changed my trade - on my father-in-law's word I was taken as navigator aboard a merchantman. Here is the trouble: the more people Gregori and his crew kill, the stronger he grows.";
			link.l1 = "And?..";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "But for some purpose he must gather all three coins together. One fell to Gregori when the treasure was divided, another was held by our boatswain, who was killed in Basse-Terre not long ago - he escaped with me back then, and we met now and again. So the second coin is surely his already. And had you not come in time, the third would have been in his hands too.";
			link.l1 = "You tell me curious things indeed!";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "I speak nothing but the truth. And to be wholly frank, this coin burns a hole in my pocket - they will reach me sooner or later all the same!";
			link.l1 = "Why are you so certain of that?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "How could it be otherwise! Captain Gregori needs all three gold coins of the Incas! I tell you what, captain - perhaps you may win this war with the spirits. I shall surely be killed, and then everyone will learn what true evil is.";
			link.l1 = "Very well, hand over that coin, and I shall try to get to the bottom of it.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DisappearPeopleTakeCoin");
		break;

		case "7":
			dialog.text = "Aha, my coin has come to me on two legs of its own!";
			link.l1 = "There is no man left in you, captain Gregori! What have those evil Indian spirits done to you! You look like a dead monkey, strike me down!";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "What do you understand, you wretched little man! Strength and power - that is what matters, the rest is worth no more to me than a picked-over cod. And now I shall take from you the thing I have so lacked.";
			link.l1 = "You shall, if you can hold a sword properly with those rotten bones!";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Foolish whelp! Now I shall show you what I have achieved in this time! Prepare to die!";
			link.l1 = "Aha!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DisappearPeopleFinalFight");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
