// Gregg: Stefan Lanven - the "Black Orchid" trilogy (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\blackorchid.c
// The player's employer through the second half of chapter 4:
//   1-27  - the first meeting in the Fort de France residence, the truth about Levinson
//           and the list of the seven seekers;
//   28-34 - the report on Brian George's chest, the task of finding the lock combination;
//   35-36 - the book is open, Lanven needs four days;
//   37-50 - the book proves worthless, but there is word of Matilda.
// Nodes 51-60 of the original belong to the "Deadly Smoking" quest and were not ported.
// The squadron carrying Matilda to the Bay of Doom is frigates here, not ships of the
// line - see the comment in MysteriousArtefactStefanLanvenIsPassenger.

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
			dialog.text = "Make haste, time does not wait!";
			link.l1 = "I am on my way.";
			link.l1.go = "exit";

			if (PChar.MysteriousArtefact == "12")
			{
				dialog.text = "How do your affairs proceed?";
				link.l1 = "I am seeing to them as best I can.";
				link.l1.go = "exit";
			}
			if (PChar.MysteriousArtefact == "15")
			{
				dialog.text = "Have you opened the book?";
				link.l1 = "Not yet.";
				link.l1.go = "exit";
			}
			if (PChar.MysteriousArtefact == "16")
			{
				TakeNItems(PChar, "MysteriousArtefact_Coin", -1);
				TakeNItems(PChar, "MysteriousArtefact_Book", -1);
				dialog.text = "Have you opened the book?";
				link.l1 = "I have. Here, take it.";
				link.l1.go = "35";
			}
			if (PChar.MysteriousArtefact == "17")
			{
				dialog.text = "I must finish reading the book.";
				link.l1 = "Very well, I shall come later.";
				link.l1.go = "exit";
			}
			if (PChar.MysteriousArtefact == "18")
			{
				dialog.text = "Make haste, time does not wait!";
				link.l1 = "I am fitting out for the voyage already.";
				link.l1.go = "exit";
			}
		break;

		case "1":
			dialog.text = "Good day, captain " + GetHeroLastName() + ". I am glad to see you alive and tolerably well.";
			link.l1 = "You are Stefan Lanven?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I am Stefan Lanven, though I prefer to be called Stefano. It pleases me more - there is something Italian in the sound of it, you know.";
			link.l1 = "Something Italian? Forgive me, I do not follow.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Italy, my friend, Italy. A land of wondrous prospects, unforgettable beauty, delightful girls, the home of the finest wine in the world. And the Colosseum! One glance at that majestic pile is worth a journey, at the beauty of its arches. \nOn first sight the Colosseum strikes the traveller dumb and holds him gazing without end at that grand work of human hands. I shall never cease to regret trading such wondrous places for the dreary grey of these godforsaken French colonies.";
			link.l1 = "On the islands of the Caribbean you will find no small store of untouched beauty that will conquer you with its... with its... with its... how shall I put it...";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Do not trouble yourself to find the fitting epithet, my friend. The bounds of your intellect are too narrow, and the vocabulary needed to describe the Caribbean expanses eloquently was exhausted long ago.";
			link.l1 = "...(holding back his anger)... Listen, Stefano, might we come straight to business? I have no wish to sit through this nonsense.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Forgive me, captain. Sometimes the poet in me awakes, and the longing for Italy - the land where I found happiness and eternal love in the person of the fair Sophia, whose beauty struck me down, I fell in love at first sight and...";
			link.l1 = "You shall tell me of your love another time.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "A great pity. I had counted on finding in you an intelligent companion and an attentive listener. It seems I was mistaken. Very well, to business.";
			link.l1 = "...(to himself: at last!)...";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Captain " + GetHeroLastName() + ", I know that you are searching for a certain artefact, the so-called Black Orchid. Let me warn you at once: denying it, objecting, saying something like 'you must be mistaken' or 'no, I hear of this artefact for the first time' is quite useless. \nI am excellently informed and always know everything. Now then, I too am searching for the Black Orchid, and have been for a long time. But there is one trouble that greatly hinders the search. Do you know what it is?";
			link.l1 = "You have not enough money?";
			link.l1.go = "8";
			link.l2 = "Someone is standing badly in your way?";
			link.l2.go = "9";
			link.l3 = "Too little is known of the artefact?";
			link.l3.go = "10";
		break;

		case "8":
			dialog.text = "The trouble is not money, captain " + GetHeroLastName() + "; of that I have plenty. I need a man with a head on his shoulders - one who will make the right decision quickly, judge a situation correctly, find a way out where there seems to be none. In other words, I need you.";
			link.l1 = "I thought my task was another one - to find and kill Matilda.";
			link.l1.go = "11";
		break;

		case "9":
			dialog.text = "Enemies I have in plenty, but the trouble lies elsewhere. I need a man with a head on his shoulders - one who will make the right decision quickly, judge a situation correctly, find a way out where there seems to be none. In other words, I need you, captain " + GetHeroLastName() + ".";
			link.l1 = "I thought my task was another one - to find and kill Matilda.";
			link.l1.go = "11";
		break;

		case "10":
			dialog.text = "No, captain, that is not it. Of the Black Orchid I know a great deal. I need a man with a head on his shoulders - one who will make the right decision quickly, judge a situation correctly, find a way out where there seems to be none. In other words, I need you, captain " + GetHeroLastName() + ".";
			link.l1 = "I thought my task was another one - to find and kill Matilda.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "...(mockingly)... Kill Matilda? Do you truly suppose I pulled you out of that cage for that alone? You need not answer, I know that you did. Though you are partly right: sooner or later you will have to kill her. \nBut her death is only part of my larger design. If that design succeeds, we shall have the Black Orchid. And you will help me carry it out. Oh, and by the way - do you still count Frederick Levinson a friend?";
			link.l1 = "...(to himself: the cunning devil, he knows of Frederick too)... Why do you ask? My affairs with Levinson are no concern of yours.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "I think otherwise. You promised to help Frederick search for the Orchid out of old friendship, so to speak. But you have no notion who Frederick Levinson is. Do you remember, some years ago, being attacked by two Spanish ships off Nevis? I see that you do. \nWell then, they were not there by chance. Those ships lay in wait for you and captain Anthony, to attack and destroy you. And they acted on Frederick Levinson's word. The man you have counted a friend all these years. You are shaken, but I thought it right that you should know.";
			link.l1 = "...(heavily)... How do you know this?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "After the affair on Puerto Rico, captain Anthony helped you greatly. You had to cut him in and divide the spoils by three rather than by two, as first intended. Frederick was bitterly vexed: he had counted on half and got a third. \nBut, judging sensibly that pressing for his half would cost him more than it was worth, he agreed to a third. Afterwards he parted from the squadron and set course for Puerto Rico, where he told the governor about the men who had carried off his daughter - that is, you and Anthony - saying nothing of his own part, and reported that you were making for Nevis. \nThe governor sent two ships in pursuit. By night they overhauled you off Nevis and sank you. Though from reliable sources it is known that you were to be boarded, so that the Spanish gold might be recovered. Why they disobeyed that order I do not know. Levinson the governor rewarded with the royal letter of marque he had dreamed of. \nFor a time Frederick worked for Spain, until he learned by chance that the Black Orchid existed. Whereupon, betraying Spain, he set out after it. The rest you know: you saved him by the cave on the Bermudas, he told you of the Orchid, and you agreed to help - though it was plain he was using you.";
			link.l1 = "So Levinson was in the Caribbean all that time? He never went to Europe?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "He never went. Levinson was working for the Spaniards the whole while.";
			link.l1 = "Then everything he told me about Alejon Kendart, about the Elusive Squadron - all a lie?";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "The tales of the Elusive Squadron and of old Kendart are no invention. They are true.";
			link.l1 = "I understand nothing. Frederick said he met Kendart in Europe, on his own farm.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "A lie. Levinson invented it so that you, " + sName + ", should believe he had truly been in Europe. He learned the Kendart story from another source - which one, I do not know.";
			link.l1 = "And I counted that rat a friend.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "You will have your chance to repay him. And now let me tell you something. A great many people know of the Black Orchid. Not all believe it exists; most take the tales for a pretty legend. Of those who believe, only a handful try to find it. And only a few of those get anywhere at all. \nThere are seven who have got very far indeed. The first stands before you. The second is Matilda. The third is none other than your 'friend' Frederick Levinson. The fourth is Brian George, whom you killed. The fifth is a Dutchman known as Tulip. The sixth is a mysterious figure, sir Andrew Murdoch. The seventh is you.";
			link.l1 = "I? Beside what you know, I know nothing at all.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Others do not know even that much. You know of the seven seekers. You know the former possessors of the Orchid: don Pedro de la Encantario, admiral of the Elusive Squadron, old Alejon Kendart, Alvarez Istilochos and, last of all, Bertrand Grenadier.";
			link.l1 = "Grenadier and Istilochos possessed the Orchid?!";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Yes, I thought you would guess. Istilochos possessed it. You know the story of his companion's mysterious death - the man was found dead in his own cabin. Istilochos used one of the Orchid's properties, passed into the cabin of another ship, and killed him.";
			link.l1 = "Anyone could have done that - any sailor, any man of the crew.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "By the sailors' account there was a faint silver glow in the cabin. The same glow was in the cabin of the manowar Azor when Kendart vanished. The Black Orchid leaves that glow behind when one of its properties is used.";
			link.l1 = "And Grenadier? Where did the Orchid come from in his hands? Wait! Then he...";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "You have it at last. Yes, Bertrand knew the Orchid was with Istilochos. During the council of war before the assault on Port-au-Prince he stole it, and afterwards killed him. There are, however, two more who possessed the Orchid, or at least held it: the dock hand Giancarlo and the beggar Jack Strbeton. I only saw it and never held it.";
			link.l1 = "How was that? Under what circumstances?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "...(with anger)... I shall never tell you that. Do not even try to ask. Understood?";
			link.l1 = "As you wish. Then another question. How could the Black Orchid come into the hands of a dock hand and a beggar?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "It is all tangled. The order in which the Orchid passed from one man to another is hard to establish, but I have done it. The first to find it was the dock hand Giancarlo. Resting on the pier, he saw a faint glow in the water. \nHe dived and brought up the Black Orchid, becoming its first possessor. Giancarlo showed it to his friend, the beggar Jack Strbeton. The next day Giancarlo was killed and the Orchid was with Strbeton. Then it passed to Istilochos - how exactly is unclear. Next came Grenadier, whose story you know. \nLater the Orchid passed to admiral don Pedro de la Encantario. Alejon Kendart was the last. After him nobody has seen the Orchid, and where it went nobody knows. That is what I am trying to find out.";
			link.l1 = "...(remembering, half aloud)... Now the Black Orchid is in its rightful place...";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "...(excitedly)... What did you just say?!";
			link.l1 = "Frederick Levinson said it to me in the tavern. Or rather, Alejon Kendart said it, and Levinson repeated it to me.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "...(thoughtfully)... Hm... " + sName + ", I must think on this. Devil take it, those are devilishly important words, devilishly important. Hm... A rightful place. What could that mean?";
			link.l1 = "Perhaps we should look where the dock hand Giancarlo first found it?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "No. I am sure not. That would be too simple. And Giancarlo was surely not the first to find the Orchid. Very well, " + sName + ", here is your first task. First, carry me to Port-au-Prince: my business on Martinique is finished. \nSecond, search Brian George's house thoroughly - you have the key to his chest, use it. Report the results to me; I shall be waiting at the governor's residence.";
			link.l1 = "One last question. Do you truly believe Brian George was a seeker of the Black Orchid? By the townsfolk's account he was...";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "A drunkard, quite so. But that was merely a cover, so that nobody should suspect him. It did not work, as you see: Levinson saw through him, and I too guessed that Brian was playing a game. Enough, " + sName + ", there is no time; we must reach Port-au-Prince at once.";
			link.l1 = "Then let us waste no time.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactStefanLanvenIsPassenger");
		break;

		case "28":
			dialog.text = "At last! I had begun to think the commandant had botched it and you were dead.";
			link.l1 = "The commandant carried out your order and is waiting for the money you promised. Stefano, I managed to open the chest in Brian George's house!";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "I see you could hardly wait to tell me! What did you find?";
			link.l1 = "A strange coin and a book with a cunning lock.";
			link.l1.go = "30";
			link.l2 = "Five hundred thousand piastres, a strange coin and a book with a cunning lock.";
			link.l2.go = "31";
		break;

		case "30":
			dialog.text = "...(smiles)... And the five hundred thousand piastres? Decided to say nothing of them? Never mind, keep them, you have earned it. Now, what is this book with the strange lock?";
			link.l1 = "Here, look! One must guess the combination of figures that only Brian George knew.";
			link.l1.go = "32";
		break;

		case "31":
			dialog.text = "Keep the money, you have earned it. Now, what is this book with the strange lock?";
			link.l1 = "Here, look! One must guess the combination of figures that only Brian George knew.";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "...(examines the lock long and closely)... Ah, I see. A cunning lock; it is not the first I have met. Yes, without the right combination there is no way round it.";
			link.l1 = "Perhaps you know of someone who deals in picking locks? Some Master of Keys.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "I do, of course, but let me tell you this: never, never use their services. Cunning rogues. Have you heard the tale of the Master of Keys on Curacao? A captain ordered an important key from him, and the scoundrel not only asked a price beyond all reason but brought soldiers to the house when the captain came to collect it. The captain put the soldiers down and took his key, but spared the Master. So I have no intention of going to them.";
			link.l1 = "But Brian George, the only man who knew the combination, is dead. Might we try every possible one? Some must fit.";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "Ah, and there are ten thousand of them. So, " + sName + ", here is your second task: learn that combination. Brian surely left himself a hint somewhere, his memory was full of holes. When you have it, come back to me.";
			link.l1 = "Very well. I understand.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactNeedOpenBook");
		break;

		case "35":
			dialog.text = "Magnifico! Excellent, " + sName + ", simply excellent, you have done it!";
			link.l1 = "What does Brian write in this book?";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "It is all about the Black Orchid! So much of it! More than fifty pages. Here is what we shall do, " + sName + ": I need time to read it through. Come to me in four days.";
			link.l1 = "Understood. In four days.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactTimerReadBook");
		break;

		case "37":
			dialog.text = "Fifty-seven pages of useless waste paper! The cretin!";
			link.l1 = "What?";
			link.l1.go = "38";
		break;

		case "38":
			dialog.text = "I do not mean you, " + sName + ". You did fine work, you got the right combination. It is that Brian George who is an utter cretin! Four days thrown away!";
			link.l1 = "Will you tell me at last what has happened?";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "What has happened? Brian George is a cretin, that is what has happened.";
			link.l1 = "That is the third time I have heard it. What is written in the book?";
			link.l1.go = "40";
		break;

		case "40":
			dialog.text = "What is written in this book is that Brian George was a cretin.";
			link.l1 = "Stefano, I am serious. I am tired of hearing it; I want to know what is in the book. Either you give it to me and I read it myself, or you stop venting your temper and tell me.";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "Very well, very well. So then, " + sName + ": Brian George was a cretin. Do you know how he searched for the Black Orchid? He dully and methodically went through the caves of the Caribbean archipelago.";
			link.l1 = "But if he did that, he must have known the Orchid was to be sought underground.";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "That cretin knew nothing whatever. He simply roamed from island to island, crawled into caves and searched. The reason he searched underground is written on the very first page; listen: 'There are no safer places in the Caribbean than caves, therefore the Black Orchid must be sought precisely there'.";
			link.l1 = "...(gloomily)... Utter nonsense. What a cretin.";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "Just so, " + sName + ". And worst of all, how much time and effort was thrown away: getting into that cretin's house, taking the book, learning the combination.";
			link.l1 = "...(with hope in his voice)... But did you find anything useful in it at all?";
			link.l1.go = "44";
		break;

		case "44":
			dialog.text = "Nothing, " + sName + ". Not one useful word. But there is no cause to despair; there is good news as well. The first and most important concerns Matilda. I had it from my man in Santiago. He reports that in two days two frigates will sail from Santiago. They will set course for the Bay of Doom, west of Havana, on the Main. Matilda will command the expedition. Do you see what I am driving at?";
			link.l1 = "I think I do. Matilda is going after the Black Orchid?";
			link.l1.go = "45";
		break;

		case "45":
			dialog.text = "Just so. Though she can hardly hope to find it there, the Bay of Doom is bound up with the Orchid somehow, of that I am certain. So, " + sName + ", hear your third task. You will sail to the Bay of Doom and find out what Matilda wants there. Prepare well: a meeting with two frigates of the Spanish fleet will be no easy matter. Any questions?";
			link.l1 = "What am I to do with Matilda if I meet her?";
			link.l1.go = "46";
		break;

		case "46":
			dialog.text = "Act as circumstances dictate, " + sName + ". I have no precise instructions for this. If you get the chance, draw everything you can out of her. If not, simply kill her.";
			link.l1 = "I understand, Stefano. I am ready.";
			link.l1.go = "47";
		break;

		case "47":
			dialog.text = "Wait, I have two more pieces of news. One of the seekers of the Orchid is dead - sir Andrew Murdoch. Shot through the head in Port Royale. They say Clive's band had a hand in it.";
			link.l1 = "Of course! Clive and Jacky were prating about it the day I managed to overhear them. Jacky was boasting of his fine shot and of how pleased Tulip had been. They were speaking of Murdoch!";
			link.l1.go = "48";
		break;

		case "48":
			dialog.text = "Perhaps so, but it hardly matters now. Murdoch is dead. And we shall never learn what he knew of the Orchid. He had no house of his own, so...";
			link.l1 = "Hm... An honorary title of sir, and no house of his own. Strange, all of it. And the third piece of news?";
			link.l1.go = "49";
		break;

		case "49":
			dialog.text = "The third you will like. Frederick Levinson fell out with Jackman over something, and Jackman has set ten thousand piastres on his capture. Frederick did not get away from the island in time: his Sea Cucumber was shot to pieces from the shore with forty-two pounders. Now the whole population of the Bermudas is hunting the poor fellow.";
			link.l1 = "One does not envy old Levinson. Though Frederick is a sly fox, he always wriggles out. I remember the Spaniards pinning us down on Puerto Rico, and Frederick...";
			link.l1.go = "50";
		break;

		case "50":
			dialog.text = "There is no time for stories, " + sName + ", make for the Bay of Doom. And make your tub ready for a fight. Two Spaniards of the fleet are no joke.";
			link.l1 = "We have sailed before, we know the trade.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToDeadShore");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
