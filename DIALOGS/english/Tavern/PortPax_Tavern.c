// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat("All the gossip of the city "+ GetCityName(npchar.city) +" at your service. What would you like to know?",
                          "We just brought up this topic. You probably forgot...", "Today is the third time you have talked about some issue...",
                          "Why are you talking like a parrot? The same thing...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("You know, " + npchar.name + ", some other time.", "Exactly, "+ GetSexPhrase("forgot", "forgot") +"something...",
                      "Yeah, really for the third time...", "Yeah...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			 // ==> квест "Странные вещи творятся на архипелаге
            if (CheckAttribute(pchar, "questTemp.PKM_SvtvA_PoiskUchitel_Tavern"))
            {
                link.l1 = "I need information about one person. He always wears a red hoodie, and calls himself a Teacher.";
                link.l1.go = "PKM_SvtvA_PU_1";
            }
			// Gregg: "Goods for Richard Tillman" - where to look for Prenol
			if (PChar.UneasyDelivery == "2" && !CheckAttribute(PChar, "UneasyDelivery.TavernPorPax"))
			{
				link.l8 = "Listen, do you know where I might find Jacque Prenol?";
				link.l8.go = "UneasyDelivery_1";
			}

			// Gregg: the "Black Orchid" (ported from Damned Destiny) - the keeper knows
			// where Brian George lived and remembers that brawl. The attribute may not
			// exist at all, hence separate checks: Storm has no short-circuit.
			if (CheckAttribute(PChar, "MysteriousArtefact"))
			{
				if (PChar.MysteriousArtefact == "3")
				{
					link.l9 = "Listen, do you know where Brian George's house is?";
					link.l9.go = "MysteriousArtefact_1";
				}
				if (PChar.MysteriousArtefact == "6")
				{
					if (!CheckAttribute(PChar, "MysteriousArtefact.BarmenTalk"))
					{
						link.l10 = "Do you remember Brian George starting a brawl here two weeks ago? I should like the details.";
						link.l10.go = "MysteriousArtefact_9";
					}
				}
			}
		break;

		case "UneasyDelivery_1":
			dialog.text = "He lives next door to the port office. Stand facing the office and his door is on your left.";
			link.l1 = "And what can you tell me of him? Can one do business with him?";
			link.l1.go = "UneasyDelivery_2";
		break;

		case "UneasyDelivery_2":
			PChar.UneasyDelivery.TavernPorPax = true;
			dialog.text = "With Jacque? Of course, if you are no fool. Prenol is sly, and the main thing is not to let him dupe you. That he can do. So be careful.";
			link.l1 = "Well, thank you for the help. Keep well.";
			link.l1.go = "exit";
		break;

		
		//Квест "Strange things are happening in the archipelago"
		case "PKM_SvtvA_PU_1":
			dialog.text = "Oh, I remember that. But he's already been arrested. They say he tried to start a riot. So if you want to find him, you go straight to the Port-au-Prince prison.";
			link.l1 = "Thank you, you helped me a lot.";
			link.l1.go = "exit";
			
			DeleteAttribute(pchar, "questTemp.PKM_SvtvA_PoiskUchitel_Tavern");
			AddQuestRecord("PKM_Animists", "27");
			AddQuestUserData("PKM_Animists", "sSex", GetSexPhrase("", "a"));
			
			PChar.quest.PKM_SvtvA_TuremchikQuest.win_condition.l1 = "location";
			PChar.quest.PKM_SvtvA_TuremchikQuest.win_condition.l1.location = "PortPax_prison";
			PChar.quest.PKM_SvtvA_TuremchikQuest.win_condition = "PKM_SvtvA_TuremchikQuest";
			
			sld = GetCharacter(NPC_GenerateCharacter("Satanist_Uchitel", "Animists1", "man", "man", sti(pchar.rank), PIRATE, -1, false));
			sld.name = "Teacher";
			sld.lastname = "";
			LAi_SetStayType(sld);
			LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
			sld.Dialog.Filename = "Quest\PKM\Strannie_veshi_tvorytsya_v_arhipelage.c";
			sld.dialog.currentnode = "Учитель_Смерть";
			ChangeCharacterAddressGroup(sld, "PortPax_prison", "goto",  "goto9");
		break;
		//==> Gregg: the Black Orchid - a mysterious artefact
		case "MysteriousArtefact_1":
			dialog.text = "Brian George? That cursed drunkard who made a scene in my tavern every single day?";
			link.l1 = "Mmm... Yes, that sounds like the man.";
			link.l1.go = "MysteriousArtefact_2";
		break;

		case "MysteriousArtefact_2":
			dialog.text = "He lives on the edge of Port-au-Prince, a few steps from the town gates.";
			link.l1 = "Thank you for the help.";
			link.l1.go = "MysteriousArtefact_3";
		break;

		case "MysteriousArtefact_3":
			dialog.text = "Hey, wait. What do you want with that cursed sot?";
			link.l1 = "He owes me a few hundred gold. I mean to get it back.";
			link.l1.go = "MysteriousArtefact_4";
			link.l2 = "I have business with him.";
			link.l2.go = "MysteriousArtefact_8";
		break;

		case "MysteriousArtefact_4":
			dialog.text = "A wasted errand, captain. He cannot pay me back ten piastres for days on end, and you mean to shake a few hundred out of him. Nothing will come of it.";
			link.l1 = "I have tried and tested ways of collecting debts, even when a man has no money.";
			link.l1.go = "MysteriousArtefact_5";
		break;

		case "MysteriousArtefact_5":
			dialog.text = "Do you mean to kill him, captain?";

			if (sti(PChar.money) >= 200)
			{
				link.l1 = "And what if I do?";
				link.l1.go = "MysteriousArtefact_6";
			}

			link.l2 = "No, I do not mean to kill him, I had other ways in mind. More effective ones. And now I am in a hurry. Farewell.";
			link.l2.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToHouseGeorgeBryan");
		break;

		case "MysteriousArtefact_6":
			dialog.text = "Of course I object, captain! To kill a man, even one like Brian George... It is my duty to call the guard.";
			link.l1 = "Listen, I think you will feel better with Brian George gone from this world. And better still once you have a couple of hundred gold from me. Life is always merrier with money, is it not?";
			link.l1.go = "MysteriousArtefact_7";
		break;

		case "MysteriousArtefact_7":
			AddMoneyToCharacter(PChar, -200);
			dialog.text = "Hand it over! Quickly!";
			link.l1 = "Here you are.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToHouseGeorgeBryan");
		break;

		case "MysteriousArtefact_8":
			dialog.text = "Business? Well, you make me laugh. What business could you have with that drunkard? He cannot get himself home unaided, and you offer him business. He cannot even beg ten piastres in the street to pay off his debt to me.";
			link.l1 = "The business is very important and I am in a hurry. Farewell.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToHouseGeorgeBryan");
		break;

		case "MysteriousArtefact_9":
			dialog.text = "What do you want them for, captain?";
			link.l1 = "Brian lost a key in that brawl, and I need it. Perhaps your account will make something clear.";
			link.l1.go = "MysteriousArtefact_10";
		break;

		case "MysteriousArtefact_10":
			dialog.text = "An ordinary brawl, what is there to tell. Everyone joined in; I remembered my young days myself and landed a couple on some rogue. Then they all calmed down and went their ways - the usual thing.";
			link.l1 = "Did you find no key after the fight?";
			link.l1.go = "MysteriousArtefact_11";
		break;

		case "MysteriousArtefact_11":
			dialog.text = "No, I found none.";
			link.l1 = "And the serving girl?";
			link.l1.go = "MysteriousArtefact_12";
		break;

		case "MysteriousArtefact_12":
			PChar.MysteriousArtefact.BarmenTalk = true;
			dialog.text = "What of her? She sat in the corner through the whole fight; I sent her home afterwards, she was frightened half to death.";
			link.l1 = "I see. Thank you.";
			link.l1.go = "exit";
		break;
		//<== Gregg: the Black Orchid

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

