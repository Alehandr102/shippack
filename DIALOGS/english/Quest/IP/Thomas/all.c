
void ProcessDialogEvent()
{
	ref NPChar, sld, chr, sldGirl;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "TohmasPetit_SelfTalkStart":
			dialog.text = "Времени прошло достаточно, нужно выдвигаться.";
			link.l1 = "";
			link.l1.go = "TohmasPetit_SelfTalkEnd";
		break;
		
		case "TohmasPetit_SelfTalkEnd":
			DialogExit();
			LAi_SetPlayerType(PChar);
			PChar.dialog.filename = "MainHero_dialog.c";
			PChar.Dialog.CurrentNode = "TalkSelf_Main"; 
		break;
		
		case "TohmasPetitBandit_1":
			dialog.text = "Ha-ha! Our Thomas has brought another fool again! Well, Captain, cough up the cash if you value your hide.";
			link.l1 = "What is the meaning of this?";
			link.l1.go = "TohmasPetitBandit_2";
		break;
		
		case "TohmasPetitBandit_2":
			dialog.text = "Thomas and his girlfriend get their cut for every fool they bring in. And you are far from the first "+GetSexPhrase("one","one")+" who ran to the rescue of our mutual acquaintance.";
			link.l1 = "Well, well... He looked like a decent man but turned out to be just another scoundrel. And I fell for it "+GetSexPhrase("like a fool.","like a fool.");
			link.l1.go = "TohmasPetitBandit_2_1"; 
		break;
		
		case "TohmasPetitBandit_2_1":
			dialog.text = "Alright, Captain, enough small talk. Now pay up—40,000 piastres will do, or else we'll have to take the money from your corpse!";
			link.l1 = "Why so quiet, Thomas? Your new friends are waiting for me to start paying. Anything to say in your defense?";
			link.l1.go = "TohmasPetitBandit_ToThomas"; 
		break;

		case "TohmasPetitBandit_ToThomas":
			DialogExit();
			chr = CharacterFromID("TohmasPetit");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetitBandit_3"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.5, 0);
		break;

		case "TohmasPetitBandit_3":
			dialog.text = "Captain... Look, I'm sorry, but you stand no chance against the four of them. Just give them your purse, don't let things get ugly! After all, life is worth more than piastres.";
			link.l1 = "Four ruffians and one cowardly spotter? Your odds aren't that great. Draw your cutlasses, you dogs, I'm about to turn you into worm food!";
			link.l1.go = "TohmasPetitBandit_4"; 
			if(sti(pchar.money)>=40000){
				link.l2 = "You're right, Thomas... Your life might not be worth a groat, but mine is dear to me. I'll pay.";
				link.l2.go = "TohmasPetitBandit_pay"; 
			}
		break;
		
		case "TohmasPetitBandit_pay":
			DialogExit();
			chr = CharacterFromID("TohmasPetit_bandit4");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetit_PayBanditTalk1"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.5, 0);
		break;	
		
		case "TohmasPetit_PayBanditTalk1":
			dialog.text = "Smart choice, Captain. Thomas, take your wench and get out of here. You'll get your cut as usual, at the old place.";
			link.l1 = "That's right, run, you coward. But know this—I will hunt you down no matter where you hide!";
			link.l1.go = "TohmasPetit_PayTomasInit";
		break;
		
		case "TohmasPetit_PayTomasInit":
			DialogExit();
			chr = CharacterFromID("TohmasPetit");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetit_PayTomasTalk"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.5, 0);
		break;
		
		case "TohmasPetit_PayTomasTalk":
			dialog.text = "Captain, there is no need to threaten me. The right people know me in the city, and we have solid protection, so you will have to do a lot of running around just to find us! Farewell.";
			link.l1 = "We shall see about that.";
			link.l1.go = "TohmasPetit_PayTomasExit";
		break;
		
		case "TohmasPetit_PayTomasExit":
			DialogExit();
			chr = CharacterFromID("TohmasPetit");
			LAi_SetActorType(chr);
			LAi_ActorRunToLocation(chr, "reload", "reload3_back", "none", "", "", "", 2);
			chr = CharacterFromID("TohmasPetit_girl");
			LAi_SetActorType(chr);
			LAi_ActorRunToLocation(chr, "reload", "reload3_back", "none", "", "", "", 2);
			chr = CharacterFromID("TohmasPetit_bandit4");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetit_PayBanditStall1"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.0, 0);
			AddMoneyToCharacter(pchar, -40000); // dialogue says 40k, was deducting 30k - mismatch
		break;
		
		case "TohmasPetit_PayBanditStall1":
			dialog.text = "Hand over the sack! It's a pleasure doing business with reasonable people. And you, Captain, I see you don't like risk at all, since you parted with 40,000 piastres so easily?";
			link.l1 = "I value my life more than piastres. Take the money and let's part ways.";
			link.l1.go = "TohmasPetit_PayBanditStall2";
		break;
		
		case "TohmasPetit_PayBanditStall2":
			dialog.text = "Ha-ha! Did you hear that, boys? "+GetSexPhrase("He","She")+" values life! That Thomas is a clever lad. What a scheme he pulled off! Spun a tale about a kidnapped fiancée, bandits in the jungle... and you, the terror of the seas, fell for it "+GetSexPhrase("like a schoolboy","like a silly girl")+"!";
			link.l1 = "Anyone can make a mistake. Stop smirking.";
			link.l1.go = "TohmasPetit_PayBanditStall3";
		break;

		case "TohmasPetit_PayBanditStall3":
			dialog.text = "Come on, don't pout! On the bright side, the entire brothel and all the taverns in town will be partying on your coin tonight. We'll drink the best rum to your health, honest to God!";
			link.l1 = "He laughs best who laughs last.";
			link.l1.go = "TohmasPetit_PayBanditStall4";
		break;

		case "TohmasPetit_PayBanditStall4":
			dialog.text = "Are you threatening us? Well, well. Mind you don't burst with anger, Captain. We have the money, we have the guns, and you're leaving here with empty pockets. Consider it payment for a good lesson.";
			link.l1 = "I am leaving. Our conversation is over.";
			link.l1.go = "TohmasPetit_PayBanditStall5";
		break;

		case "TohmasPetit_PayBanditStall5":
			dialog.text = "Go on, leave, and make it quick. Farewell, gullible Captain!";
			link.l1 = "I hope you choke on that rum, you scum.";
			link.l1.go = "TohmasPetit_PayFinal";
		break;

		case "TohmasPetit_PayFinal":
			DialogExit();
			AddQuestRecord("TohmasPetit", "TrapWithPay");
			AddDialogExitQuest("TohmasPetit_Pay");
		break;
		
		case "TohmasPetitBandit_4":
			dialog.text = "Oh my God! Guards! Guards!!! Murder!!! Help!!!";
			link.l1 = "(draw your cutlass and attack the bandits)";
			link.l1.go = "TohmasPetit_BeforeFightRecord";
		break;

		case "TohmasPetit_BeforeFightRecord":
			DialogExit();
			AddQuestRecord("TohmasPetit", "TrapWithFight");
			AddDialogExitQuest("TohmasPetit_Fight");
		break;	
		
		case "TohmasPetitGirl_1":
			dialog.text	= "Help! Murderer! Over here! Patrol!!! Hurry!!!";
			link.l1 = "Patrol? You should have called for help earlier. You are safe now.";
			link.l1.go = "Exit";
		break;

		case "TohmasPetitSoldier_1":
			dialog.text = "What happened here?! Captain, lower your weapon immediately.";
			link.l1 = "Officer, it was self-defense! These bandits attacked me!";
			link.l1.go = "TohmasPetitSoldier_2";
		break;
		
		case "TohmasPetitSoldier_2":
			dialog.text = "Four corpses and one captain with a bloody cutlass. Doesn't look much like self-defense.";
			link.l1 = "I was set up.";
			link.l1.go = "TohmasPetitSoldier_3";
		break;
		
		case "TohmasPetitSoldier_3":	
			dialog.text = "Thomas, you were here. I want to hear what you have to say.";
			link.l1 = "...";
			link.l1.go = "TohmasPetitSoldier_CallThomas";
		break;
		
		case "TohmasPetitSoldier_CallThomas":
			DialogExit();
			chr = CharacterFromID("TohmasPetit");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetitSoldier_4"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.5, 0);
		break;

		case "TohmasPetitSoldier_4":
			dialog.text = "My fiancée and I wanted to take a walk outside the town. I hired these men as guards, and then this "+GetSexPhrase("monster jumped out and started","madwoman jumped out and started")+" hacking them down like cattle!";
			link.l1 = "Why, you rat... Officer, this is all a lie.";
			link.l1.go = "TohmasPetitThomas_ToSoldier";
		break;
		
		case "TohmasPetitThomas_ToSoldier":
			DialogExit();
			sld = CharacterFromID("TohmasPetit_soldier1");
			sld.dialog.filename = "Quest\IP\Thomas\all.c";
			sld.dialog.currentnode = "TohmasPetitSoldier_5";
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", 1.5, 0);
		break;
		
		case "TohmasPetitSoldier_5":
			dialog.text = "A lie, you say? Let's hear the girl's version of what happened. Miss, tell us what happened.";
			link.l1 = "...";
			link.l1.go = "TohmasPetitSoldier_ToBride";
		break;

		case "TohmasPetitSoldier_ToBride":
			DialogExit();
			sldGirl = CharacterFromID("TohmasPetit_girl");
			sldGirl.dialog.filename = "Quest\IP\Thomas\all.c";
			sldGirl.dialog.currentnode = "TohmasPetitSoldier_6";
			LAi_SetActorType(sldGirl);
			LAi_ActorDialog(sldGirl, pchar, "", 1.5, 0);
		break;
		
		case "TohmasPetitSoldier_6":
			dialog.text = "Officer, thank God you arrived so quickly, I thought this "+GetSexPhrase("monster","madwoman")+" was going to rob and kill me!";
			link.l1 = "Officer, it's not like that. I was set up.";
			link.l1.go = "TohmasPetit_BrideToOfficer";
		break;
		
		case "TohmasPetit_BrideToOfficer":
			DialogExit();
			sld = CharacterFromID("TohmasPetit_soldier1");
			sld.dialog.filename = "Quest\IP\Thomas\all.c";
			sld.dialog.currentnode = "TohmasPetitSoldier_7";
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", 1.5, 0);
		break;

		case "TohmasPetitSoldier_7":
			PlaySound("Kopcapkz\Voices\Soldiers\Soldier_arrest_03.ogg");
			dialog.text = "Silence! We found you with a bloody weapon, surrounded by several corpses who used to be citizens of Basse-Terre. This fact, along with the testimony of two townsfolk against the word of a vagabond captain, is all I need. Hand over your weapon and follow us before I order my men to shoot you on the spot.";
			link.l1 = "Try and take me if you can.";
			link.l1.go = "TohmasPetitSoldier_fight";
			link.l2 = "Very well, Sergeant. I submit. Let's go.";
			link.l2.go = "TohmasPetitSoldier_prison";
		break;

		case "TohmasPetitSoldier_fight":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_soldiers_fight");
		break;

		case "TohmasPetitSoldier_prison": 
			ref rOfficer = CharacterFromID("TohmasPetit_soldier1");
			aref arItems, officerItems;
			ref rItem;
			string sName;
			int i, iItemsNum, iSavedMoney;
			int iPotion1 = 0;
			int iPotion2 = 0;
			int iPotion3 = 0;
			int iPotion4 = 0;
			
			RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
			RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
			RemoveCharacterEquip(pchar, SUIT_ITEM_TYPE);
			RemoveCharacterEquip(pchar, SPYGLASS_ITEM_TYPE);
			iSavedMoney = sti(pchar.money);
			
			iPotion1 = GetCharacterItem(pchar, "potion1"); 
			iPotion2 = GetCharacterItem(pchar, "potion2");
			iPotion3 = GetCharacterItem(pchar, "potion3"); 
			iPotion4 = GetCharacterItem(pchar, "potion4"); 

			makearef(arItems, pchar.items);
			makearef(officerItems, rOfficer.items);
			iItemsNum = GetAttributesNum(arItems);
			for(i = 0; i < iItemsNum; i++)
			{
				sName = GetAttributeName(GetAttributeN(arItems, i));

				if (sName == "unarmed") continue;
				// potions are preserved separately (kept on the player), so don't store them here or they would double up
				if (sName == "potion1" || sName == "potion2" || sName == "potion3" || sName == "potion4") continue;

				// quest items (totems, mummy powder, atlas, etc.) are NOT removed by RemoveAllCharacterItems,
				// so we must not store/copy them to the officer or they would duplicate and stay on the player
				rItem = ItemsFromID(sName);
				if (CheckAttribute(rItem, "ItemType") && rItem.ItemType == "QUESTITEMS") continue;

				// store ALL of the player's other belongings (weapons, armor, maps, bullets, food, jewelry, etc.)
				pchar.QuestTemp.ArrestStorage.(sName) = pchar.items.(sName);
			}
			// Борода: только ЗАПОМИНАЕМ надетый патент - по этой метке офицер отдельно давит на потерю
			// званий, а при выкупе документ вернётся. Сам предмет изымает TohmasPetitSoldier_go_prison:
			// здесь, внутри диалога и сразу после RemoveAllCharacterItems, TakeItemFromCharacter молча
			// не срабатывал (в трассировке патент оставался в инвентаре).
			if (isMainCharacterPatented()) pchar.QuestTemp.ArrestPatent = Items[sti(pchar.EquipedPatentId)].id;
			RemoveAllCharacterItems(pchar, true);
			pchar.money = iSavedMoney;
			if (iPotion1 > 0) TakeNItems(pchar, "potion1", iPotion1);
			if (iPotion2 > 0) TakeNItems(pchar, "potion2", iPotion2);
			if (iPotion3 > 0) TakeNItems(pchar, "potion3", iPotion3);
			if (iPotion4 > 0) TakeNItems(pchar, "potion4", iPotion4);
			if (CheckAttribute(pchar, "QuestTemp.ArrestStorage"))
			{
				makearef(arItems, pchar.QuestTemp.ArrestStorage);
				makearef(officerItems, rOfficer.items);
				iItemsNum = GetAttributesNum(arItems);

				for(i = 0; i < iItemsNum; i++)
				{
					sName = GetAttributeName(GetAttributeN(arItems, i));
					officerItems.(sName) = pchar.QuestTemp.ArrestStorage.(sName);
				}
				// ArrestStorage is intentionally NOT deleted: it holds the player's confiscated belongings
				// for the peaceful (bribe) return, so the player isn't handed the officer's own gear.
			}
			if (CheckAttribute(rOfficer, "items")) 
			{
				SetCharacterGoods(rOfficer, GOOD_BALLS, GetCargoGoods(rOfficer, GOOD_BALLS)); 
			}
			LAi_SetFightMode(pchar, false);
			DialogExit();
			AddDialogExitQuest("TohmasPetitSoldier_go_prison");
		break;

		case "TohmasPetitOfficer_1":
			dialog.text = "Captain, you are in such a tight spot right now that if I were you, I'd start confessing even before the priest arrives.";
			link.l1 = "Is it really that bad?";
			link.l1.go = "TohmasPetitOfficer_2";
		break;

		case "TohmasPetitOfficer_2":
			// Борода: если у ГГ изъяли каперский патент - офицер отдельно давит на потерю званий
			if (CheckAttribute(pchar, "QuestTemp.ArrestPatent"))
			{
				dialog.text = "You're facing charges for the murder of four men and the attempted robbery of two citizens, "+GetSexPhrase("lad","lass")+". And we have seized your letter of marque. Mark my words: refuse to cooperate, and you will be stripped of every rank and privilege you hold. Posthumously, of course. But I can make this whole matter 'disappear', and your patent comes back untouched, as if nothing ever happened. Though, it will cost you a pretty penny.";
				link.l1 = "And what price do you put on my good name?";
			}
			else
			{
				dialog.text = "You're facing charges for the murder of four men and the attempted robbery of two citizens, "+GetSexPhrase("lad","lass")+". But I can make this whole matter 'disappear'. Though, it will cost you a pretty penny.";
				link.l1 = "What kind of amount are we talking about?";
			}
			link.l1.go = "TohmasPetitOfficer_3";
		break;
		
		case "TohmasPetitOfficer_3":
			dialog.text = "Well, let's say one hundred and fifty thousand piastres.";
			link.l1 = "Are you out of your mind? That's a fortune.";
			link.l1.go = "TohmasPetitOfficer_4";
		break;
		
		case "TohmasPetitOfficer_4":
			dialog.text = "Yes, it's a lot, but you have to understand my position too. I'll have to grease many palms, not to mention looking out for my own interest and my friend's share.";
			if(sti(pchar.money)>=150000)
			{
				link.l1 = "It seems I have no choice. I'm ready to pay.";
				link.l1.go = "TohmasPetitOfficer_money";
			}
			link.l2 = "I don't have that kind of money.";
			link.l2.go = "TohmasPetitOfficer_executon";
			
		break;
		
		case "TohmasPetitOfficer_money":
			dialog.text = "Good. You'll have to sit tight until tomorrow, but rest assured, by morning you will be released and all your belongings will be returned.";
			link.l1 = "Nothing to be done about it, I'll have to wait.";
			link.l1.go = "TohmasPetitOfficer_money2";
		break;
		
		case "TohmasPetitOfficer_money2":
			DialogExit();
			AddTimeToCurrent(24, 00);
			SetCurrentTime(8, 20);
			AddQuestRecord("TohmasPetit", "TohmasPetit_PrisonBribePaid");
			AddDialogExitQuest("TohmasPetitSoldier_free");
		break;
		
		case "TohmasPetitOfficer_executon":
			dialog.text = "Well, suit yourself. Tomorrow morning the priest arrives, and by noon you'll be executed. And we'll get our money either way by selling your confiscated property.";
			link.l1 = "I hope you choke on that money.";
			link.l1.go = "TohmasPetit_PrisonDecision_No";
		break;
		
        case "TohmasPetit_PrisonDecision_No":
			DialogExit();
			SetCurrentTime(22, 20);
            SetLaunchFrameFormParam("The officer walked away from your cell. You were left to your thoughts, awaiting the dawn and the noose, and didn't notice falling asleep. A few hours passed, and through a heavy sleep you hear the jail latch clanking...", "Reload_To_Location", 0.1, 2.0);
			SetLaunchFrameReloadLocationParam("BasTer_prison", "goto","goto24", "TohmasPetit_CellmateWakeUp");
			playSound("Ambient\JAIL\jail_door2.wav");
			LaunchFrameForm();
        break;
		
		case "Prison_Escaper_Start":
			dialog.text = GetSexPhrase("Hey, pal... You look terrible. What are you in for?","Wow, talk about luck with a cellmate. Hey, gorgeous, what did they take you in for?");
			link.l1 = "Murder.";
			link.l1.go = "Prison_Escaper_1";
		break;

		case "Prison_Escaper_1":
			dialog.text = "Murder, you say... And who did you kill, then?";
			link.l1 = "Four thugs who turned out to be local citizens, not bandits. And it's all because of that scoundrel Thomas.";
			link.l1.go = "Prison_Escaper_2";
		break;
		
		case "Prison_Escaper_2":
			dialog.text = "Thomas? You wouldn't happen to be talking about Thomas Petit, would you?";
			link.l1 = "Yes, the very same. How do you know him? Did he send you here to finish me off early?";
			link.l1.go = "Prison_Escaper_3";
		break;
		
		case "Prison_Escaper_3":
			dialog.text = "No, of course not. I'd make a terrible assassin, I'm more into lockpicking and theft. As for Thomas, plenty of people know him in certain circles—him and his pal, an officer of the town guard. They often pull off shady scams together.";
			link.l1 = "That's why the patrol arrived so quickly. That explains a lot.";
			link.l1.go = "Prison_Escaper_4";
		break;
		
		case "Prison_Escaper_4":
			dialog.text = "Yeah, you're not the first to fall for this scam. The scheme is simple: they hire all sorts of riffraff, former sailors or thugs—men who can handle a weapon but are drowning their lives at the bottom of a mug. It's a risky business, but those people don't have much to lose anyway.";
			link.l1 = "I see. It was nice chatting, though it's a shame this information won't do me any good—tomorrow I have an appointment with the gallows.";
			link.l1.go = "Prison_Escaper_5";
		break;

		case "Prison_Escaper_5":
			dialog.text = "We both got extremely lucky. I've planned an escape, the lock is my job. But out there in the corridor sits an officer. I can't get past him alone, but with you, we stand a chance. Take this shiv, my pal hid it under the bunks. Your job is to take out the guard quietly. Behind him is the passage to the old wall. No dawdling, patrols pass every half hour. Should I open it?";
			link.l1 = "Hmm, so I do have a fighting chance to survive, and maybe even get my belongings back. Give me the shiv and open the cell.";
			link.l1.go = "Prison_Escaper_6";
		break;
		
		case "Prison_Escaper_6":
			dialog.text = "Alright, hang on a second, let me find where he stashed it. Ah, here it is, take it. Now I'll open the cell.";
			link.l1 = "Wish me luck.";
			link.l1.go = "Prison_Escaper_7";
		break;

		case "Prison_Escaper_7":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Start_Fight");
			AddQuestRecord("TohmasPetit", "TohmasPetit_PrisonEscapeStart");
		break;

		case "Prison_Escaper_8":
			dialog.text = "Whew, we broke out! The fort guard is in an uproar now, there's no turning back for us. Right now we need to get out of here before we get caught.";
			link.l1 = "Let's move!";
			link.l1.go = "exit";
			AddDialogExitQuest("TohmasPetit_follow");
		break;

		case "Prison_Escaper_10":
			dialog.text = "Looks like there's no pursuit. Listen, Cap, I can't get off this island without some hard coin. I need to buy a boat from the local fishermen. I happen to have one little thing... A piece of a treasure map. I'll tell you straight, the map is torn, I only have a part. I've spent a load of time, and I think I've figured out where the place is! Maybe you can make sense of it too, or luck will smile upon you and you'll find the missing piece. I'm ready to sell it to you for forty thousand piastres. You'll help me out, and you can get rich yourself.";
			bool bHasTreasure = false;
			if (GetCharacterItem(pchar, "map_full") > 0)
			{ 
				bHasTreasure = true;
			}
			if (GetCharacterItem(pchar, "map_part1") > 0 && GetCharacterItem(pchar, "map_part2") > 0) 
			{
				bHasTreasure = true;
			}
			if (sti(pchar.money) >= 40000 && bHasTreasure == false) 
			{
				link.l1 = "Deal. 40,000 piastres is a fair price for such a lead. Take the gold, and good luck with the fishermen.";
				link.l1.go = "Prison_Escape_BuyMap";
			}
			if (sti(pchar.Ship.Type) != SHIP_NOTUSED)
			{
				link.l2 = "What treasure, Jacques? The fort is in an uproar, I just want to get my ship out of this mess. Farewell.";
				link.l3 = "Hold on. Since you've already figured out the location, why should I buy a pig in a poke? I have an excellent ship, you have the coordinates. Let's join forces. I'll take us to the cache, we'll take the loot and split it fifty-fifty. Neither of us will be left wanting, I hope.";
				link.l3.go = "Prison_Escape_Klad";
				pchar.questTemp.Tohmas_ShipName = pchar.Ship.name;
			}
			else
			{
				link.l2 = "What treasure, Jacques? The fort is in an uproar, I just want to get out of this mess alive. Farewell.";
			}
			link.l2.go = "Prison_Escape_Farewell";
		break;

		case "Prison_Escape_BuyMap":
			AddMoneyToCharacter(pchar, -40000);
			if (GetCharacterItem(pchar, "map_full") == 0)
			{
				if (!GetCharacterItem(pchar, "map_part1")) GiveItem2Character(pchar, "map_part1");
				else 									   GiveItem2Character(pchar, "map_part2");
			}
			dialog.text = "Good luck, Captain! I hope this scrap of paper brings you more fortune than it did me.";
			link.l1 = "Farewell.";
			link.l1.go = "Prison_Escape_End";
		break;
		
		case "Prison_Escape_End":
			DialogExit();
            DisableAllExits (false);
			bDisableOfficers = false; // bought the map — escape over, officers are back with the player
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "reload1", "none", "", "", "", -1);
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesBoughtMap");
			CloseQuestHeader("TohmasPetit");
			AddGoodDeed("TohmasPetit", 3); // Gregg: доброе дело - развязка без резни
			pchar.questdata.TohmasPetit = 1; 
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
			DeleteAttribute(pchar, "DisableChangeFlagMode"); // unblock flag change (the check tests attribute existence, not its value)
		break;

		case "Prison_Escape_Farewell":
			dialog.text = "Well, suit yourself. Farewell, and don't get caught by the guards again!";
			link.l1 = "Farewell.";
			link.l1.go = "Prison_Escape_End3";
		break;
		
		case "Prison_Escape_End3":
			DialogExit();
            DisableAllExits (false);
			bDisableOfficers = false; // refused the map — escape over, officers are back with the player
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "reload1", "none", "", "", "", -1);
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesRefused");
			CloseQuestHeader("TohmasPetit");
			pchar.questdata.TohmasPetit = 1;
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
			DeleteAttribute(pchar, "DisableChangeFlagMode"); // unblock flag change (the check tests attribute existence, not its value)
		break;
		
		case "Prison_Escape_Klad":
			bool ok = false;
			if (CheckAttribute(pchar, "location.from_sea") && pchar.location.from_sea == "BasTer_town")
			{
				ok = true;
			}
			dialog.text = "Hmm... You're right. With you, we stand a real chance. Where is your ship moored?";
			if (ok == true) 
			{
				link.l1 = "At the city pier, the ship '" + pchar.questTemp.Tohmas_ShipName + "', fully provisioned and ready to sail.";
				link.l1.go = "Prison_Escape_ShipInPort";
			}
			else
			{
				link.l1 = "My ship '" + pchar.questTemp.Tohmas_ShipName + "' is anchored outside the city port, in " + XI_ConvertString(pchar.location.from_sea) + ".";
				link.l1.go = "Prison_Escape_ShipSafe";
			}
			DeleteAttribute(pchar, "DisableChangeFlagMode"); // unblock flag change (the check tests attribute existence, not its value)
		break;
		
		case "Prison_Escape_ShipInPort":
			dialog.text = "That won't work! Even if we bypass the town guards and break through to the pier, the fort will turn your tub into Swiss cheese. I have an idea. Wait here, I'll pass a secret note to my boys in town. They'll find a way to contact your crew so they can quietly move the ship to Morne l'Eau bay. It's a quiet spot, and we'll head there on foot through the jungle.";
			link.l1 = "Smart move. Let me write a note for them so the crew trusts your people. My first mate should recognize my handwriting.";
			link.l1.go = "Prison_Escape_ShipInPort1";
		break;

		case "Prison_Escape_ShipInPort1":
			DialogExit();
			AddTimeToCurrent(0, 15);
			Log_Info("You wrote and delivered the note");
			PlaySound("interface\important_item.wav");
			SetLaunchFrameFormParam("Jacques ran off towards the town. 15 minutes passed, you hear someone's footsteps...", "Reload_To_Location", 0.1, 2.0);
			SetLaunchFrameReloadLocationParam(pchar.location, "reload", "reload3_back", "TohmasPetit_AtGatesScene2");
			LaunchFrameForm();
		break;

		case "Prison_Escape_ShipInPort2":
			dialog.text = "The deed is done, the note has been delivered. Your crew should be moving out. We can't hang around here, let's head to Morne l'Eau bay on foot through the jungle.";
			link.l1 = "Let's go, no time to lose.";
			link.l1.go = "exit";
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesShipInPort");
			AddDialogExitQuest("TohmasPetit_MoveShipToMonrlo_Start");
			DisableAllExits(false);
		break;

		case "Prison_Escape_SailorReport":
			ref rSailor = CharacterFromID("TohmasQuest_Sailor");
			dialog.text = "Captain! You're alive! Well, you sure gave us a tough task. Some shady types come aboard, shove a note in our face... Good thing I recognized your handwriting right away. We weighed anchor quietly and slipped away here, the fort didn't even blink.";
			link.l1 = "Excellent work. Jacques, the ship is here, the longboat is waiting. Where are we heading now?";
			link.l1.go = "exit";
			AddDialogExitQuest("Prison_Escape_SailorReport_2");
		break;

		case "Prison_Escape_ShipInPort_Final":
			dialog.text = "You've got a great crew, Cap! Since the longboat is at the shore, let's jump aboard. Order the crew to set sail for " + XI_ConvertString(pchar.questTemp.TohmasPetit_landId) + " — that's exactly where our gold is buried!";
			link.l1 = "Excellent, Jacques. Take a cabin, we are casting off.";
			link.l1.go = "exit";
			AddDialogExitQuest("TohmasPetit_ToGrot");
			string sTargetIsland1 = XI_ConvertString(pchar.questTemp.TohmasPetit_landId);
			AddQuestRecord("TohmasPetit", "TohmasPetit_SailFromPortToBay");
			AddQuestUserData("TohmasPetit", "sTargetIsland", sTargetIsland1);
		break;

		case "Prison_Escape_ShipSafe":
			dialog.text = "That makes things much easier! Since the fort won't trap us, we'll make our way through the jungle straight to your ship in the bay.";
			link.l1 = "Then let's go, no time to lose.";
			link.l1.go = "exit";
			string sBayName; 
			
			if (CheckAttribute(pchar, "location.from_sea"))
			{
				string sBayID = pchar.location.from_sea; 
				sBayName = XI_ConvertString(sBayID);   
			}
			AddDialogExitQuest("TohmasPetit_WalkToSafeBay_Start");
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesShipSafe");
			AddQuestUserData("TohmasPetit", "sBayName", sBayName);
			DisableAllExits(false);
		break;

		case "Prison_Escape_ShipSafe_Final":
			dialog.text = "Well, we made it to your tub in one piece! Fine vessel. Let's get on board and raise the sails quickly. Our course is " + XI_ConvertString(pchar.questTemp.TohmasPetit_landId) + ", the treasure has been waiting long enough!"; 
			link.l1 = "Excellent, Jacques. Take a cabin, we are casting off.";
			link.l1.go = "exit";
			AddDialogExitQuest("TohmasPetit_ToGrot");
			sTargetIsland1 = XI_ConvertString(pchar.questTemp.TohmasPetit_landId);
			AddQuestRecord("TohmasPetit", "TohmasPetit_SailFromSafeBay");
			AddQuestUserData("TohmasPetit", "sTargetIsland", sTargetIsland1);
		break;
		
		case "Prison_Escaper_InGrot":
			float fFortuneSkill = makefloat(GetCharacterSKILL(pchar, SKILL_FORTUNE)); 
			float fMultiplier = 1.0;

			if (fFortuneSkill <= 40.0) {
				fMultiplier = 0.8 + (fFortuneSkill / 40.0) * 0.2;
			} else {
				fMultiplier = 1.0 + ((fFortuneSkill - 40.0) / 60.0) * 1.0;
			}

			int iLuckChar = sti(pchar.SPECIAL.Luck);
			int iCount;
			// --- TIER 1 ---
			iCount = makeint(8.0 * fMultiplier); 
			TakeNItems(pchar, "icollection", iCount/2);
			TakeNItems(pchar, "chest", iCount);  

			// --- TIER 2 ---
			if (iLuckChar >= 4)
			{
				iCount = makeint(50.0 * fMultiplier); 
				TakeNItems(pchar, "jewelry2", iCount); 
				TakeNItems(pchar, "jewelry4", iCount); 
			}

			// --- TIER 3 ---
			if (iLuckChar >= 8)
			{
				TakeNItems(pchar, "Chest_quest", 1);
				TakeNItems(pchar, "Chest_treasure", 3);
			}		
			Log_Info("You have received your share of the hoard");
			PlaySound("interface\important_item.wav");
            DeleteAttribute(pchar, "questTemp.TohmasPetit");
			dialog.text = "There it is, the hoard! There's a whole mountain of treasure here. Well, Captain, a deal's a deal. Here is your half, take it.";
			link.l1 = "An excellent haul, Jacques. What do you plan to do next?";
			link.l1.go = "Prison_Escaper_InGrot_2";
		break;

		case "Prison_Escaper_InGrot_2":
			dialog.text = "With loot like this, I can retire from piracy and sail back to the Old World. Drop me off at any civilized port except Basse-Terre. The governor's hounds would throw me in irons straight away.";
			link.l1 = "Very well.";
			link.l1.go = "exit";
			AddDialogExitQuest("TohmasPetit_EscaperPassenger_Start");
		break;
		
		case "Prison_Escaper_Goodbye":
			dialog.text = "Yes, this port will do nicely. Thanks for everything, Captain! If it weren't for you, I'd still be rotting on a prison bunk or worse. Farewell, and may the sea always be kind to you!";
			link.l1 = "Goodbye, Jacques. Don't squander all that gold in the very first tavern. Fair winds.";
			link.l1.go = "Prison_Escaper_ubigaet";
			pchar.QuestTemp.TohmasPetit_Search = "active";
			pchar.QuestTemp.TohmasPetit_Search.Path = "TooLate";
		break;
		
		case "Prison_Escaper_ubigaet":
			DialogExit();
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesLeftSearchActive");
			AddCharacterExpToSkill(PChar, "Fortune", 50);
			AddCharacterExpToSkill(PChar, "Sneak", 50);
			AddDialogExitQuest("TohmasPetit_Escaper_FullEnd");
		break;
		
		// Диалог в трюме корабля ГГ
		case "TohmasPetit_Hold_Dialog":
			if (!CheckAttribute(npchar, "IsTomas"))
			{
				if (!CheckAttribute(npchar, "TohmasPetit_HoldTalked"))
				{
					dialog.text = "Please, don't come near me... I'm scared. I won't tell you anything! Talk to Thomas, this is all his doing...";
					link.l1 = "Alright, stop shaking. I'll talk to your sweetheart.";
				}
				else
				{
					dialog.text = "I beg you, don't touch me... Direct all questions to Thomas. Leave me alone!";
					link.l1 = "Fine, sit tight.";
				}
				link.l1.go = "TohmasPetit_Girl_Hold_Exit";
				break;
			}
			dialog.text = "Captain, listen to me! I know I acted terribly by fleeing Guadeloupe, but we simply had no other choice! We never wanted to hurt anyone.";
			link.l1 = "You committed a folly by stealing someone else's gold and dragging a whole bunch of people into this. Now it's time to face the consequences.";
			link.l1.go = "TohmasPetit_Hold_To_Girl_1";
		break;

		case "TohmasPetit_Hold_To_Girl_1":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Hold_Girl_Speak_1");
		break;

		case "TohmasPetit_Girl_Hold_Dialog_1":
			dialog.text = "We didn't want to drag anyone into this! Thomas was just looking for a way to get me out of here. We dreamed of buying ship tickets to Europe and starting a new life, far away from this suffocating island!";
			link.l1 = "A beautiful romantic fairy tale. Too bad it's built on thievery.";
			link.l1.go = "TohmasPetit_Girl_To_Tomas_1";
		break;

		case "TohmasPetit_Girl_To_Tomas_1":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Hold_Tomas_Speak_1");
		break;

		case "TohmasPetit_Hold_Dialog_3":
			dialog.text = "Captain, be reasonable! What profit do you get from taking us back or letting us rot in these damp chains? I still have a portion of the gold left that I didn't manage to spend on the tickets. I will give you EVERYTHING down to the last piastre if you let us go!";
			link.l1 = "Oh ho, a thief trying to buy his freedom with stolen gold? Tempting, but I haven't decided yet.";
			link.l1.go = "TohmasPetit_Hold_To_Girl_2";
		break;

		case "TohmasPetit_Hold_To_Girl_2":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Hold_Girl_Speak_2");
		break;

		case "TohmasPetit_Girl_Hold_Dialog_2":
			dialog.text = "I beg of you, take this cursed gold! It has brought us nothing but trouble. Please, show mercy, don't ruin our lives, give us a chance to leave...";
			link.l1 = "Enough tears and lamentations.";
			link.l1.go = "TohmasPetit_Girl_To_Tomas_2";
		break;

		case "TohmasPetit_Girl_To_Tomas_2":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Hold_Tomas_Speak_2");
		break;

		case "TohmasPetit_Hold_Dialog_5":
			dialog.text = "What is our fate, Captain? What will you decide?";
			if (CheckAttribute(pchar, "equip.gun") && pchar.equip.gun != "")
			{
				link.l1 = "You both have tested my patience for far too long. You deserve death. Right here and now, I'm going to shoot you.";
			}
			else
			{
				link.l1 = "You deserve death. I will put a bullet through your forehead with your own pistol, Thomas, the one my sailors took from you during the search.";
			}
			link.l1.go = "TohmasPetit_Hold_Choice_Kill";
			link.l2 = "Your gold might indeed compensate for my expenses. Take your wench, I will drop you off at the nearest civilized port, and I better never see you again.";
			link.l2.go = "TohmasPetit_Hold_Choice_Release";
			
			if (!CheckAttribute(pchar, "QuestTemp.TohmasPetit_bloodytake"))
			{
				link.l3 = "No deals with thieves. I will deliver you to Basse-Terre and hand you over to the local authorities. Let them decide what to do with you.";
				link.l3.go = "TohmasPetit_Hold_Choice_Gov";
			}
		break;

		case "TohmasPetit_Hold_Choice_Kill":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Kill_Start");
		break;

		case "TohmasPetit_Girl_Shock_Dialog":
			dialog.text = "Thomas?! Oh my God, no, no, no! Thomas! You killed him!.. Monster! Bloodthirsty butcher! I beg you, please no, have mercy-y-y!..";
			link.l1 = "You should have stolen less. It's too late for mercy, gorgeous.";
			link.l1.go = "TohmasPetit_Girl_Shock_End";
		break;

		case "TohmasPetit_Girl_Shock_End":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Kill_Girl_Final");
		break;

		case "TohmasPetit_Hold_Choice_Release":
			DialogExit();
			
			sld = CharacterFromID("TohmasPetit");
			sld.TohmasPetitQuestHold = true;
			RemovePassenger(pchar, sld);
			AddPassenger(pchar, sld, false);
			ChangeCharacterAddressGroup(sld, "none", "", "");
			
			sld = CharacterFromID("TohmasPetit_girl");
			sld.TohmasPetitQuestHold = true;
			RemovePassenger(pchar, sld);
			AddPassenger(pchar, sld, false);
			ChangeCharacterAddressGroup(sld, "none", "", "");

			AddDialogExitQuest("TohmasPetit_SetupRelease");
				AddQuestRecord("TohmasPetit", "TohmasPetit_Released"); // LEO: запись была только в рус. версии
		break;

		case "TohmasPetit_Hold_Choice_Gov":
			DialogExit();
			AddQuestRecord("TohmasPetit", "TohmasPetit_HoldToGov");
			pchar.QuestTemp.TohmasPetit_Quest = "return_to_baster"; // Стадия: Вернуться на Бас-Тер
		break;

		case "TohmasPetit_Girl_Hold_Exit":
			DialogExit();
			NPChar.TohmasPetit_HoldTalked = true;
		break;
		
		case "TohmasPetit_Cap_Abordage":
			if (CheckAttribute(pchar, "QuestTemp.TohmasPetit_CapTalked") && pchar.QuestTemp.TohmasPetit_CapTalked == "failed")
			{
				dialog.text = "You?! So you actually went ahead with this madness and attacked my sloop?! Shedding my sailors' blood for the sake of one miserable runaway... You have neither honor nor conscience!";
				link.l1 = "I warned you, Captain, that I would take Thomas either way. You chose this path yourself when you refused to settle things amicably, and now I'll have to get rid of any witnesses.";
			}
			else
			{
				dialog.text = "What the devil?! Who are you, and by what right have you attacked my ship?!";
				link.l1 = "I need the passenger you are harboring on board, Thomas Petit. And as for you, Captain, our conversation will be brief. I have no use for extra witnesses.";
			}
			link.l1.go = "TohmasPetit_Cap_Abordage_Kill";
		break;

		case "TohmasPetit_Cap_AbordageFOrWhat":
			dialog.text = "But this is sheer madness! I fulfilled your demand - I handed over both Thomas and his bride, just as you asked! We parted on peaceful terms, so why this bloodshed now? What more do you want from me?";
			link.l1 = "That's true, Captain, I have no more grievances with you. But you and your crew have seen and know far too much. And I am not in the habit of leaving witnesses behind.";
			link.l1.go = "TohmasPetit_Cap_Abordage_Kill2";
		break;

		case "TohmasPetit_Cap_Abordage_Kill":
			DialogExit();
			// Борода: QuestAboardCabinDialogExitWithBattle сам ставит LAi_SetFightMode -
			// отдельный MainHeroFightModeOn дублировал его с задержкой (гонка).
			QuestAboardCabinDialogExitWithBattle("TohmasPetit_Vkaute");
		break;
		case "TohmasPetit_Cap_Abordage_Kill2":
			DialogExit();
			// Борода: Томас уже получен мирно - без колбэка, просто дерёмся с командой шлюпа
			QuestAboardCabinDialogExitWithBattle("");
		break;
		
		case "TohmasPetit_Sailor_Report":
			dialog.text = "Captain, the fight on the decks has died down, the vessel is under our control! We thoroughly searched the sloop's holds and found that slicker Thomas along with his lass Madeline, just as you ordered. Shall we drag them to you?";
			link.l1 = "Excellent work. Drag them here, to the cabin, and after that, return to the rest of the boarding crew and prepare to set sail.";
			link.l1.go = "TohmasPetit_Sailor_Leave";
		break;

		case "TohmasPetit_Sailor_Leave":
			DialogExit();
			ChangeCharacterAddressGroup(CharacterFromID("TohmasPetit_sailor"), "none", "", ""); 
			AddDialogExitQuest("TohmasPetit_Vkaute2"); 
		break;
		
		case "TohmasPetit_Cabin_Thomas_Kill":
			dialog.text = "Captain, I beg of you, spare us! We will give you all the gold, down to the last coin! Take everything, just spare our lives!";
			link.l1 = "You cost me a whole ship and a heap of dead sailors. It's too late to beg for mercy, Thomas. And I have no intention of leaving any witnesses of this massacre alive.";
			link.l1.go = "TohmasPetit_Hold_Choice_Kill"; 
		break;
		
		case "TohmasPetit_Gratitude":
			dialog.text = "Thank you for bringing us here. Here is your reward, as agreed.";
			link.l1 = "Good luck to you.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 200000); 
			AddDialogExitQuest("TohmasPetit_RunAway");
		break;

		case "exit":
			if (CheckAttribute(NextDiag,"TempNode")) NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}	
}
