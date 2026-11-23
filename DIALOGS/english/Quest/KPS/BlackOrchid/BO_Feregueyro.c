// Gregg: Feregueyro, once known as Ambrosio - the "Black Orchid" trilogy (ported from
// Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// A former pearl diver, the first possessor of the Black Orchid and the eighth seeker:
//   First time - the stock line; after node 5 - the muttering of the hooded man;
//   1-5   - the shore of Shore50, he speaks of himself in the third person;
//   6-13  - the cabin after the boarding: who he really is;
//   14-25 - Turks, the grotto, how he found the Orchid and how he lost it;
//   26-31 - Jamaica, Fort Orange, the journal and the "rightful place";
//   32-34 - the Meritor: seven parts, and the curse that drives him at the village;
//   35-44 - the rules of the search: who holds which part, and how to find the last two;
//   45    - the talk ends in a fight in the cabin.
// Node 35 hands over MysteriousArtefact_MeritorChapter_2 - the second part of the Meritor.
// In the original nodes 40 and 44 contradicted each other over who held the fourth part
// and who the fifth. Set straight to match the quest logic: the fourth is Levinson's,
// the fifth lies in the hold of Bernaldinio Toranger's ship.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sName = PChar.name;
	string sFullName = GetFullName(PChar);

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";

			if (CheckAttribute(PChar, "DestinyGift"))
			{
				if (PChar.DestinyGift == "13")
				{
					dialog.text = "Feregueyro... shall be... avenged...";
					link.l1 = "Hm...";
					link.l1.go = "exit";
				}
			}
		break;

		case "1":
			dialog.text = "All of them... shall be... killed...";
			link.l1 = "Killed? Who is to be killed?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Not one... shall leave... alive...";
			link.l1 = "Hey! Can you hear me at all? What is wrong with you?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Feregueyro... will spare... no one...";
			link.l1 = "Stand here. Do not go anywhere.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "The hour... has come... the curse... shall be... lifted...";
			link.l1 = "Speak plainly, man! What curse?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Feregueyro... shall be... avenged...";
			link.l1 = "Stay where you are. I shall bring help.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftFindFeregueyro");
		break;

		case "6":
			dialog.text = "...(breathing hard)... You have the better of me, " + sFullName + ". Finish it and be done - what use are empty words?";
			link.l1 = "Why do you send your creatures against the pearl divers' village time after time? What are you after there?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "As though you did not know, " + sFullName + ", why I am set on getting in there.";
			link.l1 = "Enough riddles, Feregueyro. Speak plainly.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "And here I thought one of the foremost seekers of the Black Orchid would be quicker of wit.";
			link.l1 = "You know of the Orchid? Speak, and be quick about it - what do you know?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Do not press me, " + sFullName + ". Let me get my breath and I shall tell you all of it.";
			link.l1 = "Do not drag it out. I have no time to wait.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Very well. You know already that old Alejon Kendart lived in the pearl divers' village - before ever you came there?";
			link.l1 = "I know that much.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "And has it never crossed your mind, " + sFullName + ", why Kendart chose that village of all the places in the Main? I shall tell you. Because every seeker holds the pearl divers' village to be the rightful place of the Black Orchid - the very place where it was found.";
			link.l1 = "So from the day Kendart found the Orchid it lay in the village all along?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Just so, " + sName + "! And two men only knew of it - sir Andrew Murdoch and I, Feregueyro. Murdoch learned it by chance a few months ago; I have known it always, and tried more than once to take the Orchid back.";
			link.l1 = "So you are the eighth seeker of the Black Orchid.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Aim higher, " + sName + ". Before you stands the first possessor of the Black Orchid. How is that, eh?";
			link.l1 = "It does not astound me. It surprises me somewhat.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "The tale runs thus. Long ago I was a plain pearl diver and my name was Ambrosio. I remember that day well: we were taking pearls off the shores of Turks, an island empty of men. \nThe squall came so suddenly there was no reaching land. The surf threw our tartane on the rocks and broke her to splinters. Two of us lived - I and a mate of mine.";
			link.l1 = "You were fortunate to come out of it alive at all.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Fortunate is a strong word, " + sName + ". We were both broken men: both my legs went, and my left arm was crushed to pieces. My mate came off lighter and dragged me to a grotto near by, out of the rain and the wind. \nBy nightfall three more of the survivors came in. I remember that night as hell: the arm hurt so that I held on with the last of my strength not to cry out.";
			link.l1 = "I understand.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "In the morning my mates went off to look for water and food. The pain was such that I began to think of ending myself. And then I caught sight of somebody's net with three oysters in it. For want of anything better I set about opening them. \nTwo were empty; in the third lay a big yellow pearl, shaped like a pear. My arm seized again and out of spite I flung the pearl into the depths of the grotto. And then I forgot everything: where it fell, in the dark, a faint silver light was burning.";
			link.l1 = "A silver glow? The sign of the Black Orchid?";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Yes. I crawled there through the pain. What glowed was a small smooth stone, shaped for all the world like a flower. I held it in my hands, the pain drained away and I fell asleep. I woke about noon - no pain at all, and every bone knitted. \nI got to my feet and walked out of the grotto: a thin rain, a raw wind. I came back, sat down and looked over the stone that had healed me. For its shape I gave it the name of Black Orchid.";
			link.l1 = "And why an orchid?";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Orchids are the flowers of my childhood - my mother often brought them into the house. That is the whole of the reason.";
			link.l1 = "And what happened then?";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "I looked round that wretched grotto and thought how fine it would be to be home in the divers' village that very minute. A light struck my eyes, pain went through me and I lost my senses. I came to in my own house, the Orchid clenched in my fist and glowing so that it hurt to look at it.";
			link.l1 = "You had used one of the Orchid's properties and carried yourself across?";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "So I had, though I did not grasp it at once. I was frightened out of my wits and was long coming round. But it soon became plain that there was no end to what it could do. To my neighbours I said not a word, of course - I made up a tale of my rescue, played the invalid and spent whole days studying it at home. Within a month I knew every property it had.";
			link.l1 = "And your mates left on Turks? You did not go back for them?";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "No. It comes hard to own to it, but no. I was so taken with my find that I forgot the whole world, and them along with it. Even when the English fell on the village I locked myself in, took up the Orchid and meant to carry myself far away. \nI was too slow. They broke the door, seized me and took the Orchid. From that day I was a slave, and they carried me to Nevis in the stinking hold of an English frigate.";
			link.l1 = "And the Englishman who took the Orchid from you? What did he do with it?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "I am coming to that. While I studied it the Orchid had lost all its lustre - nothing to look at, of no value whatever to an Englishman. At Nevis they brought me up on deck and drove me towards the boat. And as I walked the deck I saw that red-haired fellow who had taken it from me. \nHe was turning the stone over in his hands, asking another what it was good for. The other turned it about, gave it back and said it was a useless lump of... well, you can guess what he said.";
			link.l1 = "Heh, I can guess.";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "The red-haired man looked it over once more and pitched it over the side. And I was in the seventh heaven. Sitting in that hold I had thought the Orchid lost to me for ever - and here it turned out that all was not lost.";
			link.l1 = "It went into the sea. What good did it do you to see where?";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "Why, I am a diver! The frigate lay in some fifteen feet of water, and I marked the spot where it went down - there was a pier alongside. Only my hopes were not to be: that same day I was bought and carried off to Jamaica.";
			link.l1 = "And the Black Orchid was found by Giancarlo, a dock hand! Resting on the pier, he saw a faint silver glow in the water, dived - and became its next possessor.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "Yes, the tale of the later possessors is known to every seeker. But how the Orchid was first found, that I alone knew - I, its first possessor. Now you know it too.";
			link.l1 = "And what became of you on Jamaica?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "On Jamaica I worked a long while as a carpenter in the shipyard, until a chance to run came my way. I took to the jungle and reached the Dutch at Fort Orange.";
			link.l1 = "And the Dutch took you in? A runaway slave?";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "They did. There is no peace between the Dutch and the English; runaways are not given up. So I stayed and went to work as a porter at a shop. And one day in the tavern I overheard a Dutchman with an odd by-name talking to a great hulking fellow.";
			link.l1 = "Tulip! That was the Dutchman's name, was it not?";
			link.l1.go = "28";
		break;

		case "28":
			dialog.text = "So it was, I believe. He was telling the hulking fellow that the Black Orchid was back in its rightful place - only where that rightful place might be, nobody knew. I knew. The rightful place of the Black Orchid is the pearl divers' village. And there I went.";
			link.l1 = "Hold on. Why the divers' village? It was found on Turks - then Turks is its rightful place.";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "It was found on Turks, you have that right. But the words 'rightful place' are mine. While I studied the Orchid I kept a journal and set down every observation in it. And at the very beginning I wrote that the rightful place of the Black Orchid was to be held the pearl divers' village. Turks holds foul memories for me, so of Turks I said nothing.";
			link.l1 = "Then how did Kendart learn that the rightful place was the divers' village?";
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "I can only guess at it. He found the journal, that much is certain: he used the words 'rightful place', and there is nowhere else to take them from. How the journal came into his hands I do not know. \nMost likely Kendart was in the village after the English raid and stumbled on it. And when the Orchid came to him, he chose to return it to its rightful place - the one the journal named.";
			link.l1 = "It is strange all the same that he hid it in the village, and spoke of a rightful place besides. Did it never occur to him that you would come looking?";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "He was sure I had died in that raid. And if so, then nobody knew of the rightful place. There he slipped.";
			link.l1 = "He did not expect it, true. But here is what I do not understand: what do you want with the village now? The Orchid is not there, Kendart is gone. And why do you set these creatures on people?";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "Kendart is gone, but the thing that will lead you to the Orchid is not. One of its seven parts, rather. Gather all seven and the thing will show you the direction you must go to find the Orchid.";
			link.l1 = "So it is a compass, in short, that points at the Black Orchid?";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "You may put it so. I call the thing the Meritor.";
			link.l1 = "And the raids? What are those for?";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "I cannot set foot in the village while one living soul remains in it. That curse has lain on me since the very day I left the place. Where it came from I do not know. I do not understand it. So I send my servants: they kill them all, and then I walk into an empty village and take my part of the Meritor.";
			link.l1 = "And where are the rest of the parts? One is in the village, that I have.";
			link.l1.go = "35";
		break;

		case "35":
			TakeNItems(PChar, "MysteriousArtefact_MeritorChapter_2", 1);
			dialog.text = "I shall tell you all I know. One part is mine - and I give it to you. Another is held by a seeker named Matilda.";
			link.l1 = "Matilda? You said Matilda?";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "Yes, that is her name, I believe. One of the parts is hers.";
			link.l1 = "Wait. Look at this thing. Matilda gave it to me before she died. Is it a part of the Meritor?";
			link.l1.go = "37";
		break;

		case "37":
			dialog.text = "...(peers at the thing)... It is. Then that part you have found already. The third, as I said, is in the divers' village - in the house where Kendart lived.";
			link.l1 = "That house has been searched from top to bottom. There is nothing in it.";
			link.l1.go = "38";
		break;

		case "38":
			dialog.text = "Search better, it is there. I am certain - wholly certain that it is in the house where Kendart lived.";
			link.l1 = "And where does such certainty come from?";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "In time you will understand. This is not the moment for it.";
			link.l1 = "Very well, that is the third. And the others?";
			link.l1.go = "40";
		break;

		case "40":
			dialog.text = "The fourth is with Frederick Levinson, one of the seekers. The fifth is with a Spaniard, Bernaldinio Toranger, captain of the ''Vulture''.";
			link.l1 = "Of Frederick I know. But where do I look for Toranger?";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "How should I know? I tell you with whom the parts of the Meritor are, not where their keepers are to be found.";
			link.l1 = "As you please. And the last two?";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "That, in truth, is the whole of what I know. Where the sixth and the seventh lie I have no notion. But in time you will learn it yourself. Only look the oftener at the parts you already hold, and you will understand.";
			link.l1 = "Can you not say it plainly? Must it all be hints?";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "Devil take you, I have told you very nearly everything as it is! All that is left you is to find the missing parts of the Meritor - and the Orchid is yours. And still you round on me as though I sat here saying nothing.";
			link.l1 = "I only meant...";
			link.l1.go = "44";
		break;

		case "44":
			dialog.text = "Hold your tongue, " + sName + ". So: two parts of the Meritor are yours. The third is in Kendart's house, the fourth with Levinson, the fifth with Bernaldinio Toranger. Would you find the sixth and the seventh - look the oftener at those you have gathered, and you will understand. And now, " + sFullName + ", kill me. I have lost.";
			link.l1 = "I have no wish to kill you.";
			link.l1.go = "45";
		break;

		case "45":
			dialog.text = "This is no hour for mercy! I have lost, and so it falls to me to die. And you will have to kill me in any case - or die yourself. Defend yourself!";
			link.l1 = "But...";
			link.l1.go = "exit";
			LAi_SetHP(NPChar, 500.0, 500.0);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattleNoParam");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
