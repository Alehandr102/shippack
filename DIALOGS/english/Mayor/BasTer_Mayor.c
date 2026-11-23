// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("What did you want? Ask.", "I'm listening to you, what's the question?"), "For the second time in a day you're trying to ask a question...", "For the third time in a day you're trying to ask a question again...",
                          "But when will it end?! I have a lot of things to do to manage the affairs of the city, and you're trying to ask all the questions!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("I've changed my mind...", "Not now, not the place and not the time..."), "Yes, that's right... But not now, later...",
                      "I will ask, I will ask... Only later...", "I'm sorry, " + GetAddress_FormToNPC(NPChar) + "...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.State == "Fr4SoleiRoyal_GoodWork")
			{
				dialog.text = "I'm listening to you, Captain.";
				link.l1 = "Sir, I have been instructed by the Governor-General of Tortuga to escort the first-class battleship Soleil Royal to the island of Dominica. The Guadeloupe squadron was supposed to be waiting for me there, but it wasn't there. Is there any way you can clarify the situation?";
				link.l1.go = "Step_F4_1";
			}
			if (pchar.QuestTemp.TohmasPetit_Quest == "return_to_baster")
			{
				link.l2 = "Monsieur Governor, Thomas Petit and Madeline LeBlanc are in my hold. Those very swindlers who tipped off bandits and robbed visiting captains in the jungle. I caught him.";
				link.l2.go = "TohmasPetit_Gov_HandOver";
			}
		break;
		//************************** Фр.линкейка, квест №4. Солей Руаяль ************************
		case "Step_F4_1":
            dialog.text = "The fact is that I received an order to move the squadron to Dominica only yesterday - the courier was very late. Thus, I did not have time to prepare the squadron. I believe that you are from Dominica...";
			link.l1 = "True. I had to take on the Spanish squadron of a certain Juan Galeno. He tracked down and tried to capture the Soleil Royal.";
            link.l1.go = "Step_F4_2";
        break;
		case "Step_F4_2":
            dialog.text = "And where is this Juan Galeno and his squadron now?";
			link.l1 = "At the bottom, monsieur...";
            link.l1.go = "Step_F4_3";
        break;
		case "Step_F4_3":
			if (CheckCompanionInCharacter(pchar, "SoleiRoyalCaptain"))
			{
				dialog.text = "Haha, great job! Well, your task is done - you managed to deliver the Soleil Royal to its destination and coped with a difficult situation with honor. Great job, Captain. I am now taking command of the Soleil Royal.";
				link.l1 = "Fine, sir. I won't keep you - business... Goodbye, monsieur.";
				link.l1.go = "exit";
				RemoveCharacterCompanion(pchar, characterFromId("SoleiRoyalCaptain"));
				pchar.questTemp.State = "Fr4SoleiRoyal_backTortuga";
				AddQuestRecord("Fra_Line_4_SoleiRoyal", "6");
			}
			else
			{
				dialog.text = "But you failed to save the Soleil Royal! This is bad, "+ GetSexPhrase("sir", "miss") +", very bad... Now you must report to the Governor-General of Tortuga on the failure of your mission. Goodbye.";
				link.l1 = "Goodbye, monsieur...";
				link.l1.go = "exit";
				pchar.questTemp.State = "Fr4SoleiRoyal_SoleiRoyalSunk";
				AddQuestRecord("Fra_Line_4_SoleiRoyal", "7");
			}
        break;
		//************************** Квестовые штрумы ************************
		case "Cupture_after":
            dialog.text = RandPhraseSimple("You've already taken everything. What else do you need?", "Is there really anything left that you didn't bring?");
            link.l1 = RandPhraseSimple("Looking around for the last time...", "Checking, maybe I forgot to pick up something...");
            link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
		break;
		//Квест Аскольда, штурм в обличие мумии
        case "Ascold_MummyAttack":
			dialog.text = "Ahhh!!!";
			Link.l1 = "Stop yelling, you idiot. I'm here on business and I won't hurt you.";
			Link.l1.go = "Step_Asc1";
			AfterTownBattle();
        break;
        case "Step_Asc1":
            dialog.text = "The mummy speaks!!! Ahhh!!!";
			Link.l1 = "Listen to me, you miserable coward. I know that you have four items: three crystal Toltec skulls of different colors and a strange Indian object that looks like a rattle. Give me these items and we'll get out of town.";
			Link.l1.go = "Step_Asc2";
        break;
        case "Step_Asc2":
            dialog.text = "They are in the closet to my left, in one of the closed drawers. Here's the key, you can take everything...";
			Link.l1 = "Of course you forbid me, worm...";
			Link.l1.go = "exit";
			NextDiag.TempNode = "Cupture_after";
			Locations[FindLocation(pchar.location)].box1.items.sculMa1 = 1;
            Locations[FindLocation(pchar.location)].box1.items.sculMa2 = 1;
            Locations[FindLocation(pchar.location)].box1.items.sculMa3 = 1;
            Locations[FindLocation(pchar.location)].box1.items.indian22 = 1;
            pchar.questTemp.Ascold = "Ascold_MummyFoundItems";
			chrDisableReloadToLocation = false;
			SetReturn_Gover_Dialog_Exit(NPChar);
        break;
		
		case "TohmasPetit_Gov_HandOver":
			dialog.text = "Ah, Thomas Petit... I remember, I remember. So many complaints have been filed against him here that I didn't have enough ink to sign them all. Well, commendable, Captain.";
			link.l1 = "I also want to inform you that your commandant didn't lift a finger when I asked for help! He claimed he had no men. While the guard was twiddling their thumbs, I was conducting an exhausting search, caught up with the fugitives at sea, and barely managed to convince the sloop's captain to hand them over. And all that just to get back my 40,000 piastres that this scoundrel swindled out of me!";
			link.l1.go = "TohmasPetit_Gov_HandOver_Details";
		break;

		case "TohmasPetit_Gov_HandOver_Details":
			dialog.text = "I will reprimand the commandant when the opportunity arises, of course, but I won't judge him harshly. He is up to his neck in work as it is, with Spanish privateers appearing on the horizon time and again, so catching ordinary swindlers is not among his priorities. That will be all, take your lawful 15,000 piastres and you are dismissed.";
			link.l1 = "And that's it?! 40,000 piastres stolen, and you toss me fifteen thousand? This insult from your chancellery doesn't even come close to compensating for my losses! I demand full compensation from the thief's confiscated property!";
			link.l1.go = "TohmasPetit_Gov_HandOver_Value";
		break;

		case "TohmasPetit_Gov_HandOver_Value":
			dialog.text = "You permit yourself too much in my office, Captain! You should be grateful that you are being paid at all. Fifteen thousand is a more than generous reward for the heads of an entire gang of cutthroats in the jungle who pose a direct threat to the town. Your petty thief is small fry compared to them. Take the gold and leave the residence.";
			link.l1 = "I was glad to help... justice, Your Excellency. Good day.";
			link.l1.go = "TohmasPetit_Gov_HandOver_Complete";
		break;

		case "TohmasPetit_Gov_HandOver_Complete":
			DialogExit();
			// Log entry and quest closure
			AddQuestRecord("TohmasPetit", "TohmasPetit_GovComplete");
			CloseQuestHeader("TohmasPetit");
			AddDialogExitQuest("TohmasPetit_Gov_Reward");
			pchar.questdata.TohmasPetit = 1;
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

