// Gregg: Frederik Levinson - the "Black Orchid" trilogy (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\blackorchid.c
// Nodes 1-4 - by the cave on Bermudes right after the fight, 5-27 - the talk in the
// tavern. The long retellings about the Elusive Squadron are tightened a little: in
// the original they are half-screen walls of text. The meaning is kept in full.
//
// Nodes 28-37 - chapter 5, the reckoning for the betrayal, in the rented room upstairs
// in the Bermudes tavern. The entry point (node 28) is set by the logic, the last node
// leads straight into the fight (DestinyGiftBattleWithLevinson), which Levinson does
// not survive.
//
// Gregg: a departure from the original. In Damned Destiny Levinson hid in a room by
// Jackman's residence and said "he never comes into this room". There is no such
// location in CSP, here he rents a room above the tavern - so the line about the room
// is rewritten around the tavern keeper who is paid for his silence.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sName = PChar.name;

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";

			if (PChar.MysteriousArtefact == "3")
			{
				dialog.text = sName + ", have you found the creditor's money?";
				link.l1 = "Still looking, Frederik, still looking.";
				link.l1.go = "exit";
			}
		break;

		case "1":
			dialog.text = "What the devil made you meddle in our duel?";
			link.l1 = "I meant to help, Frederik. Had I not come in time, he would have killed you.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I would have managed that bastard without your help. Wait. How do you know my name?";
			link.l1 = "You did not recognise me? " + GetFullName(PChar) + ". Three years ago you and I and Anthony pulled off a fine piece of work at Puerto Rico.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = sName + ", devil take me, I did not know you at all. That day I remember perfectly. Neatly done, very neatly.";
			link.l1 = "Why have you come back? You swore you would never set foot on the Archipelago again. You kept saying you were too old for this.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "That is a long story, " + sName + ". Let us look in at the tavern - we shall talk there.";
			link.l1 = "Yes, you are right.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToBermudesTavern");
		break;

		case "5":
			dialog.text = "Well then, out with it, " + sName + ". What happened here after I left? How is Anthony? Did he win that beauty from Charlestown after all?";
			link.l1 = "Much has changed, Frederik. Let me begin with this: Anthony is dead - three days after that business at Puerto Rico. We were making for Nevis, and at night, a few miles off the island, two heavy Spanish galleons fell upon us. Anthony's brig was blown to splinters by the very first broadside, and my corvette went under in a couple of minutes. I caught hold of a piece of mast, and by morning I was washed ashore.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Did anyone besides you survive?";
			link.l1 = "A mortally wounded sailor off the brig - he did not last a quarter of an hour. Three men of mine came through. But do you know what is strangest in the whole affair?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "What?";
			link.l1 = "To this day I feel those galleons did not stumble upon us by chance. Not a single lantern lit - like ghosts in the dark. They were waiting for us.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Unlikely. More likely they were hunting English merchantmen with live cargo off Nevis - there is always demand for that in Spain. They took you for traders and struck.";
			link.l1 = "Do you seriously think a corvette and a brig can be mistaken for slavers? Merchants do not sail such ships.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "The devil knows, " + sName + ". It was night, by your own account - so they did not make out the class of your hulls. And Spaniards are Spaniards, they fire at anything foreign. Enough. Let us drink to Anthony and to both crews. Fine lads, they were.";
			link.l1 = "Aye. A pity, but there is nothing to be done.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Did you at least pay the Spaniards back? Show them who is master at sea?";
			link.l1 = "The English got there first. A squadron out of Port Royale boarded those galleons, and the captains were hanged publicly on the town square.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Ha. So the bastards did not outlive Anthony by long. Though in your place I would still have burned a Spanish settlement or two.";
			link.l1 = "Frederik, after that wreck it took me three months to get back on my feet. No ship, no blade, no pistol - nothing. I crawled out of my skin to get a hull under me again. I would rather not remember it. Enough about me. Why did you come to the Caribbean? What was wrong with Europe?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "That is just it - nothing was wrong. I bought myself a farm in England and could not stand two months of that cursed peace. So back I came.";
			link.l1 = "And I see you wasted no time - already in a fight. Who was that fellow I finished? An old enemy?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "A creditor. He owed me an enormous sum, and to avoid paying he bolted out here. I did promise him I would find him anywhere.";
			link.l1 = "But you did not come here to kill him?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "True enough, " + sName + ". I have business here. Are you with me?";
			link.l1 = "Of course. I must help an old friend.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Good, I see you have not lost your grip. To business, then. Have you ever heard of an artefact called the Black Orchid?";
			link.l1 = "The Black Orchid? Never. What sort of artefact?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "I began badly. Listen carefully. You have surely heard of the Spanish Elusive Squadron. It terrified the whole Archipelago: today it is sighted off Tortuga, tomorrow off Curacao, the day after already on the coast of Cuba. To meet it meant one thing only - defeat. Only ships under the Spanish flag went about in peace.";
			link.l1 = "I have heard of it. And of the captain who bested it at last. They call him the Saviour.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "He is not the point just now. Many no longer believe the Squadron truly was elusive: they say men feared it so much that every Spanish squadron was taken for that one, and called it unbeatable only because they entered battle already beaten. But for a time all of it was plain truth.";
			link.l1 = "Yet the Saviour defeated it. So it was nothing but the talk of frightened men after all.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "By the day the Saviour took his first ship of the Squadron, admiral Don Pedro de la Encantario had already lost the Black Orchid.";
			link.l1 = "What Don Pedro, and what Orchid? Explain at last, I understand nothing.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Don Pedro de la Encantario - admiral of the Elusive Squadron, captain of the flagship manowar Azor. He died storming a pirate settlement. And the Black Orchid is the very artefact that made the Squadron elusive and unbeatable. Until the admiral lost it. Do you follow now?";
			link.l1 = "Frederik, this all smells of raving. Some Orchid, an unbeatable squadron... Next you will tell me the Spaniards turned into skeletons at night and fired muskets at an immortal monkey for sport.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Devil take you and your wit! The Black Orchid existed. With your help or without it, I shall find it.";
			link.l1 = "Very well, cool down. I agree to help, but there is one thing: I want proof. Did anyone see this Orchid besides the late admiral?";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "One man saw it. Alehon Kendart, a scholar, a student of Indian and Aztec cults. When the Squadron came to the Caribbean they hunted for him a long while, and having found him, took him by force to the admiral's cabin aboard the Azor. Don Pedro showed him the Orchid and ordered its properties studied to the last: he suspected it held more than the invincibility of his ships. And by morning Alehon was gone - and the Orchid with him, guards or no guards.";
			link.l1 = "Raving. He simply slipped away when the Squadron passed some island, swam ashore and vanished. But how do you know so much about this Kendart?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "I spoke with him. One night a mad old man in rags came to my farm, quite out of his senses. I was for driving him off, but the maid stopped me and gave him some draught - and it returned his reason for a few hours. That was when he told me of the Squadron, of the Orchid, and of his own past.";
			link.l1 = "And you believed the tales of an old man out of his wits?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "I told you: the draught gave him back his reason. You had to hear WHAT he said and HOW. It was no raving, but a clear and deliberate account.";
			link.l1 = "And what became of him afterwards? The old man, I mean.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "He left in the morning. I never saw him again. And I bolted for the Caribbean. Understand, " + sName + ", I must find... no, WE must find the Black Orchid. If we do, we shall be the most powerful men not merely in the Caribbean - in the world.";
			link.l1 = "Hm. And where do we begin? Did the old man tell you anything at all?";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "He said: 'now the Black Orchid is in its rightful place'.";
			link.l1 = "And that is all?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "That is all. But I have something of a plan. First we need money - two or three hundred thousand piastres. Here is your first task: go to Port-au-Prince, where that creditor you killed today used to live. I have it on good authority that shortly before his death he came into an enormous sum. Find that money, and we shall begin the search.";
			link.l1 = "One question. What was the creditor's name?";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "Bryan George. And yes, " + sName + ", thank you. For saving my life by that cave. It went hard with me - I thought it was the end.";
			link.l1 = "Do not mention it. Well, I am off. Wait here, I shall be back soon.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToPortPax");
		break;

		case "28":
			// Gregg: a guard for case "Exit" - it reads NextDiag.TempNode, and this
			// branch of the dialogue can be reached without ever opening "First time".
			dialog.text = sName + "?!";
			link.l1 = "Frederik?! What the devil are you...";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "Quiet, " + sName + ", I beg you! If Jackman hears...";
			link.l1 = "...(in a whisper)... What the devil are you doing here, right under Jackman's nose?";
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "Hiding. He never comes up here, and the tavern keeper takes my money for his silence - so here I sit. Devil take it, " + sName + ", how glad I am to see you. Will you help an old friend get out of this place?";
			link.l1 = "On the contrary. I have come to pay you back. For your treachery.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "Treachery? What treachery? I do not understand what you are talking about.";
			link.l1 = "You understand perfectly well, Frederik. Do not deny it, I was told: after the business at Puerto Rico you were unhappy with your share of the spoils, went over to the Spaniards and betrayed Anthony and me.";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "...(after a long silence)... Imagine how it was for me, " + sName + ". We prepared that theft for two months, we risked our lives there, we all but went down under Spanish muskets. And then this Anthony appears, and you take him into the share, though he did nothing, nothing at all. He only helped us a little in the escape.";
			link.l1 = "...(quietly)... He saved us. I need not tell you - you saw yourself that with those corvettes we were a hair from death. Had it not been for Anthony...";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "His help was not worth giving him a third of our spoils. It was not worth it.";
			link.l1 = "Yet you did not demand a half - you agreed to a third. How was I to know you were unhappy with the division?";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "And why should I be content with a third, " + sName + "? Even that Anthony saw my displeasure when you handed him his share. And you did not notice.";
			link.l1 = "I am under no obligation to notice. You know the law. If you are unhappy with the division, say so aloud or put it on paper.";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "And what would I have got if I had said it? Nothing. Anthony would sooner have come to blows with us than give up the money. And you? You would have started preaching at me and explaining that I was in the wrong.";
			link.l1 = "That is not so, I would have...";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "It is so, " + sName + ", and you know it yourself! I would have gained nothing by it anyway.";
			link.l1 = "You never even tried! Now I see it was easier for you to betray an old friend than to ask for your own share.";
			link.l1.go = "37";
		break;

		case "37":
			dialog.text = "Perhaps so. But you know, " + sName + ", I do not regret that deed in the least.";
			link.l1 = "You will regret it now! You filth!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftBattleWithLevinson");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
