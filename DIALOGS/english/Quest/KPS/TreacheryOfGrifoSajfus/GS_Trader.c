// Gregg: the trader with African diamonds - quest "The Treachery of Grifo Sajfus"
// (ported from Damned Destiny). Logic - Program\Quests\reaction_functions\grifosajfus.c
// Nodes 1-4 - the talk before the fight, 5-15 - the tale of the beaten trader.

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
			dialog.text = "Hey, care to buy a pair of African diamonds? Top grade stones, we shall agree on a price.";
			link.l1 = "Where did you get them?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "And what business is that of yours?";
			link.l1 = "Where did you get those diamonds? Answer me!";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Go to the devil!";
			link.l1 = "Do not push your luck, or you will regret it.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Ha! And what can you do to me? You think you are the only clever one here? Others have tried too, and they are all in the next world now. So you had best clear off quickly, before I break your bones.";
			link.l1 = "Well, try it.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusBattleWithTrader");
		break;

		case "5":
			dialog.text = "All right, all right, I shall tell you, I shall tell you everything.";
			link.l1 = "I am listening.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "I am an Englishman, I served as a sailor on the schooner ''Rainbow''. A few days ago they put me and three of my mates ashore on Turks for trying to raise a mutiny. Three days later pearl divers came to the island." + NewStr() + "We passed ourselves off as shipwrecked Spaniards - we could jabber Spanish well enough - and somehow got the divers to agree to carry us at least as far as Puerto Rico. And the next day one of them found a xebec sunk in the shallows, some seven to ten metres down...";
			link.l1 = "Stop! Was that xebec called the ''Piranha''?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "That seems to have been the name. The divers did not make it out exactly. And how do you know?";
			link.l1 = "Never mind. Go on.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "The divers hauled a heap of surviving goods out of the captain's cabin. Among it all was a small chest with these diamonds. One of them, a judge of such things it seems, reckoned the stones came from Africa and told everyone.";
			link.l1 = "They worked out just like that that they were African?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "As I say, he was a judge of it and knew such things well.";
			link.l1 = "Very well, go on, what did they do with them?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Counted them, put them back in the chest and set the strictest watch on it. They meant to sail at dawn, but in the night pirates attacked. They must have learned the divers were working Turks and came.";
			link.l1 = "And then what?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "They caught the divers unawares. Landed a party of musketeers and cut down nearly all of them with the first volleys. A few of us survived that hail of grapeshot from every side and fell back into the jungle." + NewStr() + "Most of the pirate party stayed on the shore, and a smaller one came after us. I do not know what became of the others, but I managed to shake off the pursuit.";
			link.l1 = "And the chest of diamonds went to those pirates?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Hear me out. I ran as far as the northern shore and found a wounded diver there, writhing in pain. He told me of a tartane hidden on that rocky shore and bade me get off the island before the corsairs reached it." + NewStr() + "Then his voice failed, and the only thing he managed to say was that during the chase the chest of diamonds was with him and he had hidden it...";
			link.l1 = "Where? Where did he hide it?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "He never managed to say. He gave up the ghost.";
			link.l1 = "So the chest of diamonds is still somewhere on Turks?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "If the pirates did not find it. They surely learned of it and combed the whole island end to end.";
			link.l1 = "They may not have learned of it. In any case, Turks must be visited.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "And what of me, cap? May I go? I have told you all I know.";
			link.l1 = "Devil take you, live - but keep out of my sight.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusTraderExit");
		break;

		case "CitizenNotBlade":
			NextDiag.TempNode = "First time";
			dialog.text = "Mind how you go, friend, running about with a blade in your hand. I might get nervous...";
			link.l1 = RandPhraseSimple("Understood.", "Putting it away.");
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
