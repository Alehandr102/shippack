// Gregg: Jule Gotie - quest "The Vanishing" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\disappearpeople.c
// Nodes 1-3 - the first visit, 4-7 - reading the tablet from the Inca temple.

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
			dialog.text = "Captain " + GetFullName(PChar) + "? What brings you this time?";
			link.l1 = "Nothing as yet. Good day to you.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Good day, captain! How may I be of service?";
			link.l1 = "Good day, monsieur Gotie. I should like to know what you make of these tales of people vanishing? I know something of it myself. A man told me that a certain pirate captain, Ben Gregori, found an ancient hoard of magical things - a casket holding three old gold coins and a small bottle of some liquid. With someone's help this captain managed to read the inscription on the lid and turned into a mummy, a living skeleton. What do you say to that story?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "A dreadful story, captain. I take an interest in ancient Indian culture and have read of something similar in old books. This man grows stronger by killing people, does he not?";
			link.l1 = "Just so, monsieur Gotie. The navigator of the brig ''Lucky Joker'' said the same.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Good and evil dwell side by side in this world. Where there is poison, there is an antidote. We are dealing with an ancient Inca curse - the pirates must have found their old cache. Here is what I shall tell you, captain: seek the truth at the abandoned temple that stands somewhere on the Main. Perhaps a visit there will help you.";
			link.l1 = "I understand, monsieur Gotie, thank you for the help - I set out at once.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DisappearPeopleToTemple");
		break;

		case "4":
			TakeNItems(PChar, "DisappearPeople_Table", -1);
			dialog.text = "It seems this is exactly what we needed - three ancient gold coins are mentioned here!";
			link.l1 = "Yes, I think so too. The thing is, one of them is with me. The other two, unfortunately, are already Ben Gregori's.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Captain Gregori simply must have the third coin - without it he cannot leave the cave where he gained his magical power. Only by gathering all the coins together can he take human form without losing his sorcery. And there is one more thing of importance: he can be killed only by one who holds one of the three gold coins. To all others Gregori is immortal.";
			link.l1 = "So it follows he is still in the Barbados cave?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "So it follows, captain. Were he to leave it, he could not work all those evils we are witness to. And besides, he can hardly walk the towns in the shape of a mummy!";
			link.l1 = "I set out for Barbados at once - holding one of those coins, I can kill him.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Yes, that is so. But it will be no easy matter - he has gathered a great deal of power by now.";
			link.l1 = "Farewell, monsieur Gotie, and wish me luck.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DisappearPeopleToBarbados");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
