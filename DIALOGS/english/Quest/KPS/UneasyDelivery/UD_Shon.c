// Gregg: Shon Stablovski - quest "Goods for Richard Tillman" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\uneasydelivery.c
//
// The sums are cut by a factor of ten along with the whole quest: Prenol's debt is 20,000
// instead of 200,000, Tillman's half is ten thousand instead of a hundred, and the earlier
// deals "never above three thousand". The wait is shortened from three months to two weeks.
// The money is taken and the blade given by the DIALOG itself (nodes 25 and 27) - the logic
// does not repeat it, otherwise it would be charged twice.

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
			if (PChar.UneasyDelivery == "END")
			{
				dialog.text = "Ah, it is you, captain " + GetFullName(PChar) + ". How do you fare?";
				link.l1 = "Better than most. Be well, Shon.";
				link.l1.go = "exit";
				break;
			}
			if (PChar.UneasyDelivery == "16")
			{
				dialog.text = "Well, how do things stand, " + PChar.name + "?";
				link.l1 = "It is a long story. In short, Jacque Prenol is dead, but I could not recover your money.";
				link.l1.go = "22";
				if (sti(PChar.money) >= 30000)
				{
					link.l2 = "All in good order. Prenol is in the next world, and I managed to recover your thirty thousand. Here it is.";
					link.l2.go = "25";
				}
				break;
			}
			if (sti(PChar.UneasyDelivery) >= 11)
			{
				dialog.text = "Ah, it is you, " + GetFullName(PChar) + ". How goes the work on our mutual friend?";
				link.l1 = "I am working on him, have no doubt.";
				link.l1.go = "exit";
				break;
			}
			if (PChar.UneasyDelivery == "10")
			{
				dialog.text = "Ah, it is you, " + GetFullName(PChar) + ". There is nothing yet, I have not been able to learn anything.";
				link.l1 = "But I have learned something. Jacque Prenol was in league with Richard Tillman...";
				link.l1.go = "18";
				break;
			}
			if (PChar.UneasyDelivery == "8")
			{
				dialog.text = "Ah, it is you, " + GetFullName(PChar) + ". There is nothing yet, I have not been able to learn anything.";
				link.l1 = "Very well, I shall call on you later.";
				link.l1.go = "exit";
				break;
			}
			if (PChar.UneasyDelivery == "6")
			{
				dialog.text = "I would ask you to relieve me of your presence.";
				link.l1 = "Ahem... As you wish.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "What is your pleasure?";
			link.l1 = "Heh-heh. We need to talk...";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Sit down, let us talk.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryShonFirstSitDialog");
		break;

		case "3":
			dialog.text = "So then, what is your pleasure?";
			link.l1 = "Your cutthroats are dead, " + NPChar.name + ".";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "What cutthroats? What are you talking about?";
			link.l1 = "Do not pretend you had nothing to do with it.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Forgive me, but I truly do not understand what you are speaking of. Perhaps you might tell me in more detail, for I can make nothing of it.";
			link.l1 = "Does the name Jacque Prenol mean anything to you?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Of course, we have been partners a long while and...";
			link.l1 = "Enough lying! A few days ago three killers tried to finish Prenol in his own house. You sent them, they acted on your order, and you...";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "...(interrupting)... What? Listen, what nonsense are you talking?! What killers, damn it? I am an honest trader, I have long worked with Jacque Prenol, and... Wait, you said there were three killers?";
			link.l1 = "Yes, three. Do you remember at last?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Argh! The damned scoundrel! He deceived me. I cannot believe it, damn it all.";
			link.l1 = "Deceived?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Listen here. Those three you finished in Prenol's house were my trade representatives - they were carrying Prenol the earnest money on a large sandalwood contract - 30,000 piastres. But Jacque deceived me! Evidently he played the victim whom three cutthroats were supposedly trying to kill. \n And once you had finished them, he sent you to kill me, his supposed client, while he himself made off with the money. You are no doubt surprised that I know so much of what happened there? There is nothing surprising in it: this is the second time Prenol has used such a scheme. Only I did not think he would dare use it against me.";
			link.l1 = "But... you said you had long worked with Prenol? Then why should he kill your representatives?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "To take the earnest money - those same 30,000. The whole contract ran to a good hundred thousand, but the earnest was enough for Jacque: before that he had never held three thousand at once. So he resolved to steal it, the money being so great.";
			link.l1 = "So he deceived me as well!";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "No, you he used as a murder weapon, but me he deceived! I never thought he would have the nerve. The villain!";
			link.l1 = "Indeed, there is nothing worse than betrayal.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Listen, will you help me take revenge on Prenol? You look a sturdy sort, and your head seems to be in order.";
			link.l1 = "I agree. What must be done?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Nothing for now. Go about your own affairs, but in two weeks I shall be waiting for you here, in this tavern. I hope that by then my lads will have dug up something about Prenol.";
			link.l1 = "And what if I call at his house in Port-au-Prince?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "And what do you hope to find there? Prenol certainly will not be there, that I tell you with absolute certainty.";
			link.l1 = "I shall look in there all the same, in Port-au-Prince, perhaps I shall learn something.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Very well, act as you see fit, but in two weeks be here on the dot, understood?";
			link.l1 = "Understood, understood - in two weeks, in this tavern.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "And one last thing: what are you called? I quite forgot to ask.";
			link.l1 = GetFullName(PChar) + ". Captain " + GetFullName(PChar) + ".";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Well, be well, captain " + GetFullName(PChar) + ".";
			link.l1 = "Until we meet.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryShonDialogExit");
		break;

		case "18":
			dialog.text = "...(interrupting)... Another villain! I did not mean to interrupt, go on.";
			link.l1 = "Prenol was to give Tillman half the money, fifteen thousand, but chose to kill him rather than pay. When I called at Tillman's house he was already dead. I managed to bring down the killers, and on one of them I found a letter. Here, read it.";
			link.l1.go = "19";
		break;

		case "19":
			TakeNItems(PChar, "RichardTillmanNotice", -1);
			TakeNItems(PChar, "TillmanKillersNotice", -1);
			dialog.text = "...(reads)... «And do not forget to say the key phrase».";
			link.l1 = "We must learn that phrase. If I find the man and give it to him, I may be able to learn where Prenol is.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Hm? You know, " + PChar.name + ", I have a thought. Prenol was very fond of repeating the phrase: «That is fine, that is good». Perhaps he said it to you more than once as well. Might that be the key phrase?";
			link.l1 = "Yes, I recall he said such a thing to me. It is worth a try.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "Good luck, " + PChar.name + ".";
			link.l1 = "And to you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryShonReadNotice");
		break;

		case "22":
			dialog.text = "What ill luck! And I so counted on your help. Now I shall have to sell my weapon collection. Ah, it is a pity to part with it, but one must live somehow.";
			link.l1 = "All will be well. You will rise again and gather a new collection.";
			link.l1.go = "23";
		break;

		// Gregg: the original had "Alex" here - the name of the Damned Destiny protagonist,
		// leaked into the line
		case "23":
			dialog.text = "It is not to be, " + PChar.name + ", not to be. My grip is not what it was, few connections remain, there is nobody to lean on. After this deal I meant to leave the business. And now... I shall have to sell the weapons. I shall raise some twenty thousand, if I do not undersell, and then to my rest.";
			link.l1 = "Very well, Shon, I must go. Forgive me for failing to recover your money. There was no chance.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "I do not blame you, " + PChar.name + ". In the end I bungled it with Prenol myself, and you were under no obligation to help me. But in any case, thank you for everything.";
			link.l1 = "Keep well, Shon.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryShonCancelMoney");
		break;

		case "25":
			AddMoneyToCharacter(PChar, -30000);
			dialog.text = "Thank you indeed, " + PChar.name + ", I was already thinking of selling my weapon collection.";
			link.l1 = "Next time be more careful with the likes of Prenol.";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "That I shall be, have no doubt. Listen, " + PChar.name + ", for your help I wish to make you a gift. It is one of the finest pieces in my collection. It is yours now.";
			link.l1 = "A worthy weapon. I see it has been in battle more than once.";
			link.l1.go = "27";
		break;

		case "27":
			UD_GiveShonReward();
			// Gregg: the gift is picked by the player's strongest weapon skill, so it may well
			// be a firearm - the line follows the fact, not the other way round
			if (PChar.UneasyDelivery.RewardGun == true)
			{
				dialog.text = "Yes, it has a rich history. That is what makes it precious. They say this piece killed one of the finest Spanish captains, the favourite of the Governor General of Spain - Alvarez Istilohos.";
			}
			else
			{
				dialog.text = "Yes, it has a rich history. That is what makes it precious. They say this blade killed one of the finest Spanish captains, the favourite of the Governor General of Spain - Alvarez Istilohos.";
			}
			link.l1 = "Who is that?";
			link.l1.go = "28";
		break;

		case "28":
			dialog.text = "So you do not know him? Remember that name, " + PChar.name + ", you will have to hear it more than once.";
			link.l1 = "I shall remember it, so be it. Very well, Shon, thank you for the gift.";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "Thank you. Well, farewell, " + PChar.name + ". Perhaps we shall meet again some day.";
			link.l1 = "Perhaps we shall. In that case, until we meet.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryShonYesMoney");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
