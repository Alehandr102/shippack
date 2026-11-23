// Gregg: Otto Groberg - quest "The Treachery of Grifo Sajfus" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\grifosajfus.c
// Nodes 1-2 - the stranger with the note in the pirate town, 3-15 - Otto himself in the
// tavern, 16-19 - the settling of accounts at Puerto Principe.

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
			dialog.text = "Captain " + GetFullName(PChar) + "? I was told to hand you this upon your arrival.";
			link.l1 = "Told by whom?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I do not know. Perhaps it says so in the note.";
			link.l1 = "Very well, give me the note.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusTakeNotice");
		break;

		case "3":
			dialog.text = "Ah, captain " + PChar.name + ", I am glad you have reached Bermuda at last. My name is Otto Groberg.";
			link.l1 = "What do you want, Otto?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "You have something that ought to have been mine.";
			link.l1 = "And what would that be?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "The diamonds, captain " + GetHeroLastName() + ", the diamonds you found on Turks. I am prepared to buy them from you.";
			link.l1 = "The price will be several times that of ordinary diamonds, and besides...";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "I know the price of these diamonds, captain " + GetHeroLastName() + ", and I am prepared to give the most valuable commodity in the world for them.";
			link.l1 = "And what commodity is that?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Do you still not guess, captain?";
			link.l1 = "Speak plainly, do not drag it out.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Information, captain " + GetHeroLastName() + ". I shall buy those diamonds from you for tidings most interesting and exceedingly useful to you.";
			link.l1 = "Do you take me for a fool, Otto? You think I shall sell you diamonds for information?";
			link.l1.go = "9";
		break;

		// Gregg: without the alternative prologue the Piranha was never the player's -
		// they are after a legend, not their own ship
		case "9":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "For valuable information, captain. Information about a dream of a ship, far better than that very ''Piranha'' you are seeking.";
				link.l1 = "Better than the ''Piranha''? They say nothing finer ever sailed the Caribbean.";
			}
			else
			{
				dialog.text = "For valuable information, captain. Information about a dream of a ship, far better than the ''Piranha'' you used to sail.";
				link.l1 = "No, there never was and never will be a ship better than the ''Piranha''!";
			}
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "You are mistaken, captain. I know a thing or two about the ''Piranha''. At the yard where she was built, two more ships were made. In appearance the three are indistinguishable. First came the ''Chameleon'', with plain flaws in her design, then the ''Piranha'', in whose making every fault of the ''Chameleon'' was accounted for." + NewStr() + "But the summit of the craft was the ship that embodied all the experience gathered in building the ''Chameleon'' and the ''Piranha'' - the ''Marlin''. That is the ship I propose you take. Agree, captain, she is a work of art. You will have to sweat for her, to be sure, but she is worth it.";
			link.l1 = "Well then, suppose I sell you the diamonds, you tell me where to find this ship, I sail there and find nothing.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "This is verified information, captain. Still, if you want guarantees, I am ready to give them.";
			link.l1 = "And what guarantees are those?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Hm. Let me think. I could sail with you as a passenger and point her out when we run her down. I shall not join the fighting, but I can look after myself.";
			link.l1 = "Hm. Are you handy with a sword?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "No worse than your fellow corsairs. Are such guarantees enough?";
			link.l1 = "They will do. So, where am I to find this wonder of a ship?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "In a week the ''Marlin'' will put in at Puerto Principe. We have some ten days to reach Cuba, captain. Once she is taken, set me ashore in the port of Puerto Principe.";
			link.l1 = "There is no time to lose! We must weigh anchor at once!";
			link.l1.go = "15";
		break;

		case "15":
			TakeNItems(PChar, "SajfusDiamondChest", -1);
			dialog.text = "Hey, and what of my diamonds?";
			link.l1 = "Here are your stones, and now - onward.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusToPuertoPrincipe");
		break;

		case "16":
			if (PChar.TreacheryOfGrifoSajfus.MarlinSink == true)
			{
				dialog.text = "My condolences, " + PChar.name + ", you were close to victory, but...";
				link.l1 = "Yes, I know. A pity, of course, but there is nothing for it, life goes on.";
				link.l1.go = "17";
			}
			else
			{
				dialog.text = "Congratulations, " + PChar.name + ". You are now the captain of one of the fastest vessels in the Caribbean.";
				link.l1 = "You were right, this ship is simply magnificent! Perfection itself.";
				link.l1.go = "19";
			}
		break;

		case "17":
			dialog.text = "How many other swift vessels are there in the Caribbean? And one of them you will surely take!";
			link.l1 = "I shall try. If another chance comes my way, I will not let it slip.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "That is the spirit. Very well, " + PChar.name + ", I have no time, I must be off. Farewell.";
			link.l1 = "Keep well, Otto.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusOttoExit");
		break;

		case "19":
			dialog.text = "Congratulations once more! I would drink with you, but I have no time, I must be off. Farewell, " + PChar.name + ", and mind the ship.";
			link.l1 = "Keep well, Otto.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusOttoExit");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
