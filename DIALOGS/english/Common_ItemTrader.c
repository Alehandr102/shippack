
void ProcessDialogEvent()
{
 	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	// вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\ItemTrader\" + NPChar.City + "_ItemTrader.c";
    if (!HasCityDialogFile("ItemTrader", NPChar.City, "_ItemTrader.c")) NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\ItemTrader\DefaultCity_ItemTrader.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
	else
	{
		NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\ItemTrader\DefaultCity_ItemTrader.c";
	    if (LoadSegment(NPChar.FileDialog2))
		{
	        ProcessCommonDialog(NPChar, Link, NextDiag);
			UnloadSegment(NPChar.FileDialog2);
		}
	}
    // вызов диалога по городам <--
// ============================================================================
// ============================= блок angry ==========>>>>>>>>>>>>>>>>>>>>>>>>>
    if (CheckAttribute(npchar, "angry") && !CheckAttribute(npchar, "angry.ok"))
    {
        npchar.angry.ok = 1;
        if (!CheckAttribute(npchar, "angry.first")) //ловушка первого срабатывания
        {
            NextDiag.TempNode = NextDiag.CurrentNode;
            Dialog.CurrentNode = "AngryExitAgain";
            npchar.angry.first = 1;
        }
        else
        {
			if (npchar.angry.name == "AscoldMerchant") Dialog.CurrentNode = "AngryQuest_AscoldMerchant";
        }
    }
// <<<<<<<<<<<<<<<<<<<<<<======= блок angry ===================================
// ============================================================================
	switch(Dialog.CurrentNode)
	{
		// -----------------------------------Диалог первый - первая встреча
  		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
		break;

  		case "First time":
			//зачарованный город -->
			if (pchar.questTemp.MC == "toCaracas" && npchar.city == "Caracas")
			{
				dialog.text = LinkRandPhrase("I'm trading for myself here and I don't want to know any squabbles. It's none of my business.",
					"Yes, there is unrest in the city, however...",
					"I don't think these worries will hurt my trade. It's the tavern owner who needs to worry.");
				link.l1 = "Clear.";
				link.l1.go = "exit";
				break;
			}
			if (pchar.questTemp.MC == "toCaracasPadre" || pchar.questTemp.MC == "toCaracasTavern")
			{
				dialog.text = "They say that you went to this werewolf's tavern! How's it going?";
				link.l1 = "No way...";
				link.l1.go = "exit";
				break;
			}
			//<-- зачарованный город
			
			//Загадочный остров -->
			if (CheckAttribute(pchar, "MysteriousIslandTorgovets"))
			{
				PChar.quest.MysteriousIslandWaitAction.win_condition.l1 = "Timer";
				PChar.quest.MysteriousIslandWaitAction.win_condition.l1.date.hour = sti(GetTime() + 1);
				PChar.quest.MysteriousIslandWaitAction.win_condition.l1.date.day = GetAddingDataDay(0, 0, 0);
				PChar.quest.MysteriousIslandWaitAction.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 0);
				PChar.quest.MysteriousIslandWaitAction.win_condition.l1.date.year = GetAddingDataYear(0, 0, 0);
				PChar.quest.MysteriousIslandWaitAction.function = "MysteriousIslandWaitAction";
				DeleteAttribute(pchar, "MysteriousIslandTorgovets");
			}
			//<-- Загадочный остров

			dialog.text = NPCharSexPhrase(npchar, "Do you want to buy equipment for yourself or your officers, Captain? Then you've come to the right place!", "Oh, Captain, I knew you'd come to me!.. I can offer equipment for you and your officers, as well as many other necessary things.");
			//--> Sinistra. Пролог Анжелика Тич
			if (CheckAttribute(pchar, "questTemp.AnjelikaTichPrologue") && pchar.questTemp.AnjelikaTichPrologue == "ATP")
			{
   				dialog.text = NPCharSexPhrase(npchar, "Come, choose, senorita. Everything is of the highest quality!", "Oh, senorita, I knew you would come to me!.. Feel free to choose!");
			}
			if (CheckAttribute(npchar, "isFence"))
			{
				dialog.text = "Pssst, "+ GetSexPhrase("friend","beauty") +"... Looking for rare crafting ingredients? I've got plenty of that. But I don't show my goods to just anyone, and not for coppers. And don't ask where they come from.";
				if (!CheckAttribute(npchar, "fenceFee")) npchar.fenceFee = 3 + dRand3(7); // LEO: access fee - 3-10 gold ingots (daily dRand, not save-scummable)
			}
			link.l1 = "Show me what you have there.";
			link.l1.go = "Trade_exit";
			// LEO: the fence shows his goods for gold ingots, or free under threat (if the player is a bloody killer)
			if (CheckAttribute(npchar, "isFence"))
			{
				link.l1.go = "Fence_Access";
				if (CheckAttribute(npchar, "fencePaid")) link.l1.go = "Trade_exit"; // LEO: already paid/dealt this generation - don't gate goods again
				if (sti(pchar.reputation) <= 10)
				{
					link.l7 = "I'll cut you in half right now, dog! Show me your junk!";
					link.l7.go = "Fence_Threat";
				}
			}

   			link.l2 = "I'm on another case.";
			link.l2.go = "quests";
			// the fence buys any goods, no questions - a loophole to offload freight cargo without a guild debt
			if (CheckAttribute(npchar, "isFence") && Freight_CargoAboard())
			{
				link.l5 = "I've got some... cargo. Don't ask whose. Care to take it?";
				link.l5.go = "FenceFreight_Offer";
			}
			// cargo of an ALREADY-OVERDUE freight (guild debt active, creditors chasing) - offload to the fence to shake them off (risky)
			if (CheckAttribute(npchar, "isFence") && Freight_DebtCargoAboard())
			{
				link.l6 = "The guild's after me over a certain cargo. If it were to vanish, and you held your tongue - would you take it?";
				link.l6.go = "FenceDebt_Offer";
			}
			// debt cargo is aboard but NOT the full load (rats ate some / sold part) - fence explains he won't take a partial load
			else if (CheckAttribute(npchar, "isFence") && Freight_DebtCargoPartial())
			{
				link.l6 = "The guild's hounding me over a cargo... trouble is, not all of it made it through.";
				link.l6.go = "FenceDebt_Short";
			}
			//--> eddy. Аскольд, перехват на рендомного перца
			if (pchar.questTemp.Ascold == "Seek_powder" && !CheckAttribute(npchar, "quest.answer_2"))
			{
   				link.l4 = LinkRandPhrase("Have you ever seen such a thing as mummy powder?", "Do you have mummy powder on sale?", "Listen, do you know where you can get mummy powder?");
				link.l4.go = "AscoldMerchant";
			}
			//<-- eddy. Аскольд, перехват на рендомного перца
			link.l3 = "Thank you, not interested.";
			link.l3.go = "exit";

			NextDiag.TempNode = "First time";
		break;

		// --- offloading freight cargo to the fence (scam loophole) ---
		case "FenceFreight_Offer":
			if (!Freight_CargoAboard())
			{
				dialog.text = "Eh? You've got no cargo. Don't waste my time.";
				link.l1 = "My mistake.";
				link.l1.go = "First time";
				break;
			}
			int ffPay = Freight_FencePrepare();
			dialog.text = "Hmm... a tidy lot. I'll give you "+ FindRussianMoneyString(ffPay) +" - and we part as if we never met. Deal?";
			link.l1 = "Deal.";
			link.l1.go = "FenceFreight_Done";
			link.l2 = "Changed my mind.";
			link.l2.go = "First time";
		break;

		case "FenceFreight_Done":
			Freight_FenceSell();
			if (CheckAttribute(pchar, "GuildDebt"))
			{
				dialog.text = "The money's yours. Though... word is the guild dislikes its cargo turning up on the side. None of my business, "+ GetSexPhrase("friend","beauty") +".";
			}
			else
			{
				dialog.text = "Here's your coin. I saw no cargo, I don't know you. Be seeing you, "+ GetSexPhrase("friend","beauty") +".";
			}
			link.l1 = "Be seeing you.";
			link.l1.go = "exit";
		break;

		// --- offloading the cargo of an ALREADY-OVERDUE freight (debt active) to the fence: burying the evidence ---
		case "FenceDebt_Offer":
			if (!Freight_DebtCargoAboard())
			{
				dialog.text = "What cargo? You've got nothing of the sort. Go sleep it off.";
				link.l1 = "My mistake.";
				link.l1.go = "First time";
				break;
			}
			int fdPay = Freight_FenceDebtPrepare();
			dialog.text = "Hot goods, with the guild on your tail... Well, I'll risk it. I'll give "+ FindRussianMoneyString(fdPay) +", and it's gone for good - as if it never was. Deal?";
			link.l1 = "Deal. Take it.";
			link.l1.go = "FenceDebt_Done";
			link.l2 = "Changed my mind.";
			link.l2.go = "First time";
		break;

		case "FenceDebt_Done":
			Freight_FenceSellDebt();
			if (CheckAttribute(pchar, "GuildDebt")) // the fence snitched - debt remains and grew
			{
				dialog.text = "The coin's yours... but I'll be honest: cargo like that, the right people hear of it fast. Nothing to do with me, "+ GetSexPhrase("friend","beauty") +".";
			}
			else // debt cleared
			{
				dialog.text = "Done. There was no cargo, I never saw you. Let the guild chase the wind. Be seeing you, "+ GetSexPhrase("friend","beauty") +".";
			}
			link.l1 = "Be seeing you.";
			link.l1.go = "exit";
		break;

		// debt cargo aboard but NOT the full load (rats ate some / sold part) - fence won't take a partial load, needs all of it per the invoice
		case "FenceDebt_Short":
			int gShort = sti(pchar.GuildDebt.GoodsType);
			dialog.text = "Oh no. The guild counts cargo down to the last bale, by the invoice. Take half and they'll pin the shortfall on you - and drag me down with it. Bring everything on the debt papers ("+ sti(pchar.GuildDebt.GoodsQty) +" "+ GetGoodsNameAlt(gShort) +"), and then we'll talk.";
			link.l1 = "Understood. I'll bring it all.";
			link.l1.go = "First time";
		break;

		case "Trade_exit":
            if (!CheckAttribute(npchar, "isFence") && CheckNPCQuestDate(npchar, "Item_date")) // fence stock is fixed (set once on spawn), no daily refresh
			{
				SetNPCQuestDate(npchar, "Item_date");
				GiveItemToTrader(npchar);
			}
			if (CheckAttribute(npchar, "isFence")) LAi_SetStayType(npchar); // fence stands still during trade (resumes wandering when window closes, see IDoExit)
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchItemsTrade(npchar);
		break;

		// --- LEO: fence, access to goods for gold ingots (player's pocket first, then cabin chest) ---
		case "Fence_Access":
			int fAcc = sti(npchar.fenceFee);
			if (Fence_CountIngots() >= fAcc)
			{
				dialog.text = "A look at goods like these costs "+ fAcc +" gold ingots. Deal?";
				link.l1 = "Here, "+ fAcc +" ingots.";
				link.l1.go = "Fence_Pay";
			}
			else
			{
				dialog.text = "A look at goods like these costs "+ fAcc +" gold ingots. And you don't have that many. Come back with gold.";
			}
			link.l2 = "Too steep. Another time.";
			link.l2.go = "First time";
		break;

		case "Fence_Pay":
			int fPaid = Fence_TakeIngots(sti(npchar.fenceFee)); // LEO: actually removes ingots (pocket->chest), returns amount taken
			npchar.fencePaid = true; // LEO: don't charge ingots again this fence generation
			Log_Info("Paid the fence for a look: "+ fPaid +" gold ingots.");
			LAi_SetStayType(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchItemsTrade(npchar);
		break;

		// --- LEO: fence, free look under threat (player is a bloody killer) ---
		case "Fence_Threat":
			dialog.text = "All right, all right, calm down! Here, look... I'll show you everything, just put away the steel.";
			link.l1 = "That's better.";
			link.l1.go = "Fence_ThreatOpen";
		break;

		case "Fence_ThreatOpen":
			npchar.fencePaid = true; // LEO: already intimidated this generation - stop gating goods
			LAi_SetStayType(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchItemsTrade(npchar);
		break;
		//=================== ноды квеста мэра. поиск шпиона на улице, опрос свидетеля ==================
		case "SeekSpy_Checking":
			dialog.text = "Yes, I confirm, " + NPCharSexPhrase(&characters[sti(pchar.GenQuest.SeekSpy.BaseIdx)], "he's really a local.", "she's really a local.");
			link.l1 = RandPhraseSimple("I got it. Thanks for the help.", "Everything is clear. Thanks for the help.");
			link.l1.go = "SeekSpy_Checking_1";
		break;
		case "SeekSpy_Checking_1":
			switch (pchar.GenQuest.SeekSpy.Type)
			{
				case "guardian": LAi_SetGuardianTypeNoGroup(npchar); break;
				case "patrol":   LAi_SetPatrolTypeNoGroup(npchar);   break;
				case "citizen":  LAi_SetCitizenTypeNoGroup(npchar);  break;
				case "merchant": LAi_SetMerchantTypeNoGroup(npchar); break;
			}
			LAi_SetCitizenTypeNoGroup(&characters[sti(pchar.GenQuest.SeekSpy.BaseIdx)]);
			NextDiag.CurrentNode = "First Time";
			DialogExit();
		break;
		//Квест Аскольда. Базары с уличными торговцами
		case "AscoldMerchant":
			if(npchar.city == pchar.questTemp.Ascold.MerchantColony && npchar.location.locator == "merchant1")
			{
				dialog.text = "Uh, "+ GetSexPhrase("sweetest", "darling") +", you're in the wrong place. You need to go to the Indian shamans or sorcerers, as they are called there...";
				link.l1 = "So, where will you find them? No one knows anything, or knows, but doesn't say anything.";
				link.l1.go = "AscoldMerchant_5";
			}
			else
			{
				//BlackThorn: более внятный поиск порошка.
				if (!CheckAttribute(npchar, "quest.answer_1") && rand(2)==0)
				{
					npchar.quest.answer_1 = "true";
					npchar.quest.answer_2 = "true";
					if (NationNameGenitive(sti(npchar.nation)) != NationNameGenitive(sti(pchar.questTemp.Ascold.dialogNation)))
					{
						dialog.text = "I'm sorry, but we don't keep that kind of stuff. You should ask around in colonies of "+ NationNameGenitive(sti(pchar.questTemp.Ascold.dialogNation))+".";
						link.l2 = "Thanks for the information.";
						link.l2.go = "exit";
					}
					else if (XI_ConvertString("Colony" + pchar.questTemp.Ascold.MerchantColony) != XI_ConvertString("Colony" + npchar.city))
					{
						dialog.text = "I don't cheat my clients and I don't sell any garbage, but, according to rumors, in the colony "+ XI_ConvertString("Colony" + pchar.questTemp.Ascold.MerchantColony) +" you can find anything, from love potions to mummy powder.";
					}
					else
					{
						dialog.text = "I don't have it, but someone was definitely selling it in the city. Ask around the other merchants.";
					}
					link.l2 = "Thanks for the information.";
					link.l2.go = "exit";
				}
				else
				{
					npchar.quest.answer_1 = "true";
			dialog.text = NPCStringReactionRepeat(LinkRandPhrase("Mummy powder? I do not know what a mummy is.", "Powder of what? Mummies? What is it?", "Mummy powder? What kind of thing is this?"),
                     LinkRandPhrase("Are you asking me about this again?", "Are you starting a conversation about the same thing again...", "And again about this powder?"),
                     LinkRandPhrase(NPCharSexPhrase(npchar, "I answered that question for you! What else do you want?", "I answered that question for you! What else do you want?"),
                                    NPCharSexPhrase(npchar, "I've already told you everything I think is necessary. Are these strange questions again?", "I've already told you everything I think is necessary. Those weird questions again?"),
                                    "Those weird questions again? I have nothing to add to what I said earlier!"),
                     LinkRandPhrase("Again?!! Again?!!", "There are just no words!!! Again?!", "Are you talking about that damn powder again?!"), "quest", 10, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("This is the embalmed body of an Egyptian who died many millennia ago.",
                                               "Yeah, I'm asking. Did you remember anything?",
                                               "I ask you again, maybe something has changed...",
                                               "Yeah, I'm asking again.", npchar, Dialog.CurrentNode);
            link.l1.go = DialogGoNodeRepeat("AscoldMerchant_2", "AscoldMerchant_3", "AscoldMerchant_4", "", npchar, Dialog.CurrentNode);
				}
			}
		break;

		case "AscoldMerchant_2":
			dialog.text = LinkRandPhrase("Nah, we don't need that kind of stuff. I've never met him. And in general, you are the first, who is asking for such a strange goods. I don't think the Holy Church will like trading in such items. It's not a joke, you know...",
                     NPCharSexPhrase(npchar, "No, there was no such goods. Even if I had, I wouldn't have taken it. I attend church regularly, I don't think the Inquisition would like to trade in such things.", "No, there was no such goods. Even if I had, I wouldn't have taken it. I'm a good parishioner, I don't think the Inquisition would like to trade in such things."),
                     NPCharSexPhrase(npchar, "I've never seen anything like it. And I guess it's for the best. If my wife finds out, she will immediately run to the inquisitors...", "I have never met anything like this. And I guess it's for the best. My husband will not be thrilled with my trade..."));
			Link.l1 = "Yes? Well, okay, no, so no...";
			Link.l1.go = "exit";
		break;

		case "AscoldMerchant_3":
			dialog.text = LinkRandPhrase(NPCharSexPhrase(npchar, "I didn't forget anything. I don't know anything about any mummies and I don't want to know. And I don't recommend you to do this. Otherwise, they will be accused of witchcraft, they will be excommunicated from the church...", "I did not forget anything. I don't know anything about any mummies and I don't want to know. And I don't recommend you to do this. Otherwise, they will be accused of witchcraft, they will be excommunicated from the church..."),
                                    "I remember everything perfectly. But I don't want to talk about mummies. The Holy Inquisition would like to deal with this matter...",
                                    NPCharSexPhrase(npchar, "I remember everything well. Well, if my wife had known about such proposals, the Holy Inquisition would have dealt with this long ago...", "I remember everything well. And I think I should call my husband..."));
			Link.l1 = "Agarch!! You say strange things. Oh well...";
			Link.l1.go = "exit";
		break;

		case "AscoldMerchant_4":
			dialog.text = LinkRandPhrase(NPCharSexPhrase(npchar, "Nothing has changed, absolutely nothing!!! I have not seen the Egyptian dead either in general or in a crushed form!", "Nothing has changed, absolutely nothing!!! I have not seen the Egyptian dead either as a whole or in a crushed form!"),
                                    NPCharSexPhrase(npchar, "Absolutely nothing has changed - I have not seen any crushed corpses and, hopefully, I will not see them.", "Absolutely nothing has changed - I have not seen any crushed corpses and, hopefully, I will not see them. And if you bother with idiotic questions, I'll call my husband..."),
                                    NPCharSexPhrase(npchar, "Has it changed?!! Look, I've made myself clear! Is it really not clear that I do not contact the dead, crushed or whole?!!", "Has it changed?!! Look, I've made it perfectly clear! Isn't it clear that I don't mess with the dead, crushed or whole?!!"));
			Link.l1 = NPCharSexPhrase(npchar, "Don't be so nervous. Calm down, I didn't see it, I didn't see it...", "Don't be so nervous. Calm down, I didn't see it, so I didn't see it...");
			Link.l1.go = "exit";
		break;

		case "AscoldMerchant_5":
			dialog.text = "That's it... And no one will tell you. The Inquisition can hunt yo for much less sins. And even more so for this. Who wants to pay for their long tongue with their lives?";
			link.l1 = "Oh... I really need it. One person made an order, but I haven't been able to find it for long.";
			link.l1.go = "AscoldMerchant_6";
		break;

		case "AscoldMerchant_6":
			dialog.text = "Order, you say? Isn't your client an inquisitorial bloodhound?";
			link.l1 = "No, it doesn't look like it... A reputable seller of a reputable store. Why would he do that?!";
			link.l1.go = "AscoldMerchant_7";
		break;

		case "AscoldMerchant_7":
			dialog.text = "It really doesn't look like it... Well, I'll sell you the powder. Just don't try to tell anyone. Keep in mind that I do not know you and I will deny it at the last judgment.";
			link.l1 = "Well, that's understandable...";
			link.l1.go = "AscoldMerchant_8";
		break;

		case "AscoldMerchant_8":
			dialog.text = "You owe me a thousand coins.";
			if (sti(pchar.money) >= 1000)
			{
				Link.l1 = "It's not cheap, however, but I can't bargain here. Here you go.";
    		    Link.l1.go = "exit";
    		    pchar.questTemp.Ascold = "PowderWasSeek";
    		    AddMoneyToCharacter(pchar, -1000);
    		    GiveItem2Character(Pchar, "Powder_mummie");
			}
			else
			{
				link.l1 = "Damn it! I don't have that much with me. I'll pack it up and come straight to you. Look, don't sell it while I'm running...";
				link.l1.go = "exit";
			}
		break;

// ======================== блок нод angry ===============>>>>>>>>>>>>>>>
		case "AngryQuest_AscoldMerchant":
Dialog.text = RandPhraseSimple("I don't want to deal with you anymore.", "I won't sell you anything anymore.");
			Link.l1 = RandPhraseSimple("Well, whatever you want. Apparently, you don't need the money.", "The loss is small...");
		    Link.l1.go = "AngryExitAgain";
		    //===>> квест Аскольда, розыск порошка мумии.
            if (pchar.questTemp.Ascold == "Seek_powder" && npchar.city == pchar.questTemp.Ascold.MerchantColony && npchar.location.locator == "merchant1")
            {
Dialog.text = "You are very persistent, apparently you really need this powder.";
    			Link.l1 = "That's right. And now I want to know if you really have it. And don't worry, everything will remain between us.";
    		    Link.l1.go = "AngStep_1";
            }
            if (CheckAttribute(npchar, "angry.terms")) //мирилка по квесту Аскольда через 10 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms) && pchar.questTemp.Ascold != "Seek_powder")
                {
Dialog.text = NPCharSexPhrase(npchar, "I really hope that you won't bore me with strange requests. I think I'm ready to do business with you.", "I really hope that you won't bother me with these terrible corpse purchases. I think I'm ready to trade with you.");
        			Link.l1 = RandPhraseSimple("Great!", "Excellent!");
        			Link.l1.go = "First time";
        			CharacterDelAngry(npchar);
                }
            }
    	break;
////////////////////////////////////////////////////////////////////////////////////////////////
		case "AngryExitAgain":
            DialogExit();
            DeleteAttribute(npchar, "angry.ok")
		break;

		case "AngStep_1":
Dialog.text = "There is, but not for everyone...\nI'll sell it to you. A thousand coins, and he's yours.";
            if (sti(pchar.money) >= 1000)
            {
    			Link.l1 = "Okay, here's your money.";
    		    Link.l1.go = "AngryExitAgain";
    		    pchar.questTemp.Ascold = "PowderWasSeek";
    		    CharacterDelAngry(npchar);
    		    AddMoneyToCharacter(pchar, -1000);
    		    GiveItem2Character(Pchar, "Powder_mummie");
            }
            else
            {
    			Link.l1 = "I don't have that amount with me right now. As soon as I get ready, I'll visit you right away.";
    		    Link.l1.go = "AngryExitAgain";
            }
    	break;
// <<<<<<<<<<<<============= блок нод angry =============================
	}
}
