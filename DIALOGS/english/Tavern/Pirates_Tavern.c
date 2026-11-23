// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			// Gregg: The Black Orchid, chapter 7 - why the residence is shut
			if (CheckAttribute(PChar, "SmallMongrel"))
			{
				if (PChar.SmallMongrel == "2")
				{
					link.l30 = "Tell me, why can nobody get in to Jackman any more?";
					link.l30.go = "BO_SM_Pir_1";
				}
			}
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("What questions?", "What do you want, " + GetAddress_Form(NPChar) + "?"), "Just recently you tried to ask me a question, " + GetAddress_Form(NPChar) + "...", "This is the third time you've talked about a question this day...",
                          "Are we going to ask questions again?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("I changed my mind...", "I have nothing to talk about right now."), "Hmm, something happened to my memory...",
                      "Yeah, really for the third time...", "No, what questions...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
            if (pchar.questTemp.State == "SeekBible_toBermudes")
			{
                link.l1 = HeroStringReactionRepeat("Listen, I'm looking for a pirate who sold some kind of strange Bible here. Haven't you heard about this case?",
                "I'm talking about the Bible... we've already found out, I'm sorry.", "Yes, that's right...", "Ah, we've already figured it out, I'm sorry...", npchar, Dialog.CurrentNode);
                link.l1.go = DialogGoNodeRepeat("Step_H8_1", "", "", "", npchar, Dialog.CurrentNode);
            }
            if (pchar.RomanticQuest == "BrotherIsDead")
			{
                link.l1 = HeroStringReactionRepeat("Listen, do you know where Atilla is now?",
                "Oh, yes, we've already told about Atilla...", "Yeah, right...", "Sorry, I forgot it...", npchar, Dialog.CurrentNode);
                link.l1.go = DialogGoNodeRepeat("Step_Romantic_1", "", "", "", npchar, Dialog.CurrentNode);
            }
			if (pchar.questTemp.BlueBird == "toBermudes")
			{
				link.l1 = "Listen, buddy, have you heard anything interesting about the Bluebird xebec in the tavern?";
				link.l1.go = "BlueBird_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toBermudes")
			{
				link.l1 = "Did Edward Law happen to be here?";
				link.l1.go = "PL_Q3_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toTavernAgain" || pchar.questTemp.piratesLine == "KillLoy_2toTavernAgain")
			{
				link.l1 = "I'm looking for Edward Law.";
				link.l1.go = "PL_Q3_2";
			}
			if (pchar.questTemp.piratesLine == "PL5Hunter_toBermudes")
			{
				link.l1 = "Did John Avory stop by your tavern?";
				link.l1.go = "StepPL5Berm_1";
			}
			if (CheckAttribute(PChar, "questTemp.BSPrologue.WaitingForNassauSiege") && Colonies[FindColony("Nassau")].nation == ENGLAND)
			{
				link.l1 = "Is Ms. Guthrie here?";
				link.l1.go = "GatrisBarman";
			}
            // <== Проверяем поле состояния квестов.
			// Gregg: "Goods for Richard Tillman" - asking about Prenol on Bermuda
			// Gregg: the attribute is checked in a separate if - a hero who never took the
			// quest has none, and a bare comparison raises a RUNTIME ERROR. It cannot be
			// folded into one condition with && : there is no short-circuit evaluation.
			if (CheckAttribute(PChar, "UneasyDelivery"))
			{
				if (PChar.UneasyDelivery == "12")
				{
					link.l8 = "Listen, " + NPChar.name + ", I am looking for a man, his name is Jacque Prenol. Has he been here?";
					link.l8.go = "UneasyDelivery_1";
				}
			}

			// Gregg: "The Treachery of Grifo Sajfus" (ported from Damned Destiny) -->
			// The quest attribute is checked for existence BEFORE any comparison: a hero
			// who took neither the alternative prologue nor the innkeeper branch has none.
			int iSajfusRankReq = 1;
			if (!bQuestsRank) iSajfusRankReq = 5;
			if (!CheckAttribute(PChar, "TreacheryOfGrifoSajfus"))
			{
				// Entry for those who did not take the alternative prologue, where the quest
				// is granted by the prologue itself. Here the innkeeper tells the tale of
				// Sajfus's execution and the player hunts the missing xebec as a legend.
				if (sti(pchar.rank) >= iSajfusRankReq)
				{
					link.l11 = "What is the news in our waters, " + NPChar.name + "?";
					link.l11.go = "TreacheryOfGrifoSajfus_0";
				}
			}
			else
			{
				if (PChar.TreacheryOfGrifoSajfus == "1")
				{
					link.l11 = "Good to see you, " + NPChar.name + "!";
					link.l11.go = "TreacheryOfGrifoSajfus_1";
					// Gregg: if the talk was broken off midway, send the player back to their
					// OWN branch - node 1 speaks for the alternative prologue hero
					if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha")) link.l11.go = "TreacheryOfGrifoSajfus_2";
				}
				if (PChar.TreacheryOfGrifoSajfus == "2")
				{
					link.l12 = "Any news of the xebec?";
					link.l12.go = "TreacheryOfGrifoSajfus_9";
				}
				if (PChar.TreacheryOfGrifoSajfus == "3")
				{
					link.l13 = "Any news of the xebec?";
					link.l13.go = "TreacheryOfGrifoSajfus_10";
				}
				if (PChar.TreacheryOfGrifoSajfus == "6")
				{
					link.l14 = "I found that xebec. And she is not the ''Piranha''.";
					link.l14.go = "TreacheryOfGrifoSajfus_17";
				}
				if (PChar.TreacheryOfGrifoSajfus == "7")
				{
					link.l15 = "Any news of the ''Piranha''?";
					link.l15.go = "TreacheryOfGrifoSajfus_9";
				}
				if (PChar.TreacheryOfGrifoSajfus == "8")
				{
					link.l16 = "Any news of the ''Piranha''?";
					link.l16.go = "TreacheryOfGrifoSajfus_20";
				}
				if (PChar.TreacheryOfGrifoSajfus == "14")
				{
					link.l17 = "I have learned what became of the ''Piranha''.";
					link.l17.go = "TreacheryOfGrifoSajfus_27_2";
				}
			}
			// Gregg: "The Treachery of Grifo Sajfus" <--
			// Gregg: the "Black Orchid" (ported from Damned Destiny) - the tavernkeeper
			// holds word from Frederik Levinson. The branch is opened by the messenger in
			// port, whose node 2 sets MysteriousArtefact to "CanTake" and later to "1".
			if (CheckAttribute(PChar, "MysteriousArtefact"))
			{
				if (PChar.MysteriousArtefact == "CanTake")
				{
					link.l20 = "I hear you were looking for me.";
					link.l20.go = "MysteriousArtefact_1";
				}
			}
			// Gregg: the "Black Orchid", chapter 5 - the fourth piece of the Meritor is with
			// Levinson. A hero who never reached chapter five simply has no DestinyGift
			// attribute, so its existence is checked in a separate nested if: Storm has no
			// short-circuit evaluation and this cannot be folded into one && condition.
			if (CheckAttribute(PChar, "DestinyGift"))
			{
				if (PChar.DestinyGift == "20")
				{
					link.l21 = "Listen, " + NPChar.name + ", I need Frederik Levinson. Has he shown himself here?";
					link.l21.go = "BO_Levinson_1";
				}
			}

		break;

		case "UneasyDelivery_1":
			dialog.text = "Hm. Let me think. No, I do not recall. Perhaps he did come in, but how am I to know every visitor to my tavern? They do not introduce themselves when they ask for rum.";
			link.l1 = "True enough. Very well, " + NPChar.name + ", I shall go and look.";
			link.l1.go = "UneasyDelivery_2";
		break;

		case "UneasyDelivery_2":
			dialog.text = "And what do you want him for?";
			link.l1 = "He and I have old scores to settle.";
			link.l1.go = "UneasyDelivery_3";
		break;

		case "UneasyDelivery_3":
			dialog.text = "Old scores, you say? Well, if you find him, settle them properly.";
			link.l1 = "That I can do. Very well, until we meet.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryDialogWithBermudesBarmen");
		break;

		// Gregg: THE TREACHERY OF GRIFO SAJFUS (ported from Damned Destiny) -->
		// Node 0 - entry via the tavern, node 1 - entry from the alternative prologue.
		case "TreacheryOfGrifoSajfus_0":
			TreacheryOfGrifoSajfusStartFromTavern();
			dialog.text = "There is a fresh tale, just the thing for you. Some five days back Grifo Sajfus put into our harbour. A rare guest in these waters, as you know. Well, one of his sailors got drunk and let slip what Sajfus did off Dominica.";
			link.l1 = "And what did he do?";
			link.l1.go = "TreacheryOfGrifoSajfus_2";
		break;

		case "TreacheryOfGrifoSajfus_1":
			dialog.text = PChar.name + "! You are alive, devil take you, alive! And we had already buried you here, the whole settlement of us - we thought that was the end of you, left in that hell off Dominica.";
			link.l1 = "Alive, as you see! And how did you learn of what happened off Dominica?";
			link.l1.go = "TreacheryOfGrifoSajfus_2";
		break;

		case "TreacheryOfGrifoSajfus_2":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "He lured a comrade of his, a captain he had sailed with for years, to Dominica and laid him low there along with his whole crew. And took the ship for himself.";
				link.l1 = "Over a ship? A fine friend.";
			}
			else
			{
				dialog.text = "Some five days back Grifo Sajfus put into our harbour. He is a rare guest in these waters, as you know. Well, one of his sailors got drunk and let slip what Sajfus did off Dominica.";
				link.l1 = "And what happened then?";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_3";
		break;

		case "TreacheryOfGrifoSajfus_3":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "We put Grifo to the wall and questioned him. At first he denied it, but a couple of hours later he confessed. We shot him right here for the betrayal.";
				link.l1 = "Serves him right. And what ship was it?";
			}
			else
			{
				dialog.text = "We put Grifo to the wall and questioned him. At first he denied it, said there was no such thing, and how could he, your old friend, have killed you. But a couple of hours later he confessed, and we shot him right here for the betrayal. We avenged your death. We thought then that you were gone.";
				link.l1 = "Damn it, and I had wanted to cut his throat with my own hands.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_4";
		break;

		case "TreacheryOfGrifoSajfus_4":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "The xebec ''Piranha''. A famous vessel, no one in the Caribbean had her like. He had always dreamed of her, and so he turned traitor.";
				link.l1 = "And where is she now?";
			}
			else
			{
				dialog.text = "Had we known you were alive, we would have left him to you. But as it is...";
				link.l1 = "I understand. There is only one thing I cannot grasp - why did he betray me? What for? So many years of friendship, and then this...";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_5";
		break;

		case "TreacheryOfGrifoSajfus_5":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "That is the riddle. Grifo came to Bermuda on an old rotten sloop, not on her. And where he put her he never confessed, however hard we pressed him.";
				link.l1 = "A ship like that is no needle. Sooner or later she will surface.";
			}
			else
			{
				dialog.text = "That part is plain enough. It was your xebec. He wanted your ''Piranha'', he had always dreamed of her, and so he turned traitor. He admitted it himself under questioning.";
				link.l1 = "The vermin! Betrayed me for a ship!";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_6";
		break;

		case "TreacheryOfGrifoSajfus_6":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "My thoughts exactly. A famous xebec, no one in the Caribbean had her like. If anyone has taken her, we shall hear of it at once.";
				link.l1 = "And if they sent her to the bottom? Grifo could hardly handle a ship at all, and a beauty like that would have been quite beyond him.";
			}
			else
			{
				dialog.text = "Only something here does not add up. He came to Bermuda on an old rotten sloop, not on your beauty. And where he put her he never confessed, however hard we pressed him. Still, we shall soon find out. Yours was a famous xebec, no one in the Caribbean had her like. If anyone has taken her, we shall hear of it at once.";
				link.l1 = "Unless they sent her to the bottom. Grifo could hardly handle a ship at all, and the ''Piranha'' would have been quite beyond him. So they might well have sunk her.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_7";
		break;

		case "TreacheryOfGrifoSajfus_7":
			dialog.text = "No one in his right mind would sink a ship like that. Only take her. Think for yourself - you see a beauty of a ship at sea, a hull unique in her qualities, what will you do with her? Try to take her, of course.";
			link.l1 = "True enough. But if Sajfus was attacked at night, they might not have seen what ship was before them and sent her to the bottom. That is possible too.";
			link.l1.go = "TreacheryOfGrifoSajfus_8";
		break;

		case "TreacheryOfGrifoSajfus_8":
			dialog.text = "Why guess, though? Time will tell. Look in on me now and again, and if there are rumours of her I shall be sure to pass them on.";
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				link.l1 = "Agreed. A ship like that is worth chasing.";
			}
			else
			{
				link.l1 = "Agreed, I shall look in from time to time.";
			}
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusPiratesBarmenDialogEnd");
		break;

		case "TreacheryOfGrifoSajfus_9":
			dialog.text = "Nothing to be heard so far.";
			link.l1 = "A pity.";
			link.l1.go = "exit";
		break;

		// Gregg: the convoy moved from Cuba to Puerto Rico - see the head of grifosajfus.c
		case "TreacheryOfGrifoSajfus_10":
			dialog.text = "Yes, there is news. Yesterday some captain rolled into my tavern, I do not know his name, but he looked new to these parts. He began telling how off the shores of Puerto Rico he met a Spanish trade convoy of four galleons and a xebec. He decided not to meddle with them, being on a light sloop himself, so he turned aside. The galleons held their course, but the xebec gave chase. And by his account she ran his sloop down in a matter of minutes, though the distance between them was considerable.";
			link.l1 = "And then what? A boarding?";
			link.l1.go = "TreacheryOfGrifoSajfus_11";
		break;

		case "TreacheryOfGrifoSajfus_11":
			dialog.text = "Then came a miracle. His gunners contrived to bring down two masts with a single volley of chain shot! They were amazed themselves when they saw what that volley had done. In short, the xebec lost way at once and they cleared off smartly.";
			link.l1 = "You think it is the ''Piranha''?";
			link.l1.go = "TreacheryOfGrifoSajfus_12";
		break;

		case "TreacheryOfGrifoSajfus_12":
			dialog.text = "It looks very like it. That cap said she was the fastest ship he had ever met.";
			link.l1 = "Hm. Her repairs will drag on a long while, I am sure of that. So that xebec is in San Juan now. Did that captain mention which port the convoy was making for?";
			link.l1.go = "TreacheryOfGrifoSajfus_13";
		break;

		case "TreacheryOfGrifoSajfus_13":
			dialog.text = "San Juan, I believe. Though I cannot say for certain. What do you mean to do?";
			link.l1 = "I shall sail to Puerto Rico and look the ship over. If that xebec really is the ''Piranha'', I shall board her.";
			link.l1.go = "TreacheryOfGrifoSajfus_14";
		break;

		case "TreacheryOfGrifoSajfus_14":
			dialog.text = "And if her masts are not yet stepped?";
			link.l1 = "We shall see. First I must make certain it is her.";
			link.l1.go = "TreacheryOfGrifoSajfus_15";
		break;

		case "TreacheryOfGrifoSajfus_15":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Well then, a fair wind to you, " + PChar.name + ", and mind yourself. The things they tell of that xebec are more than many a captain can chew.";
				link.l1 = "They tell all sorts of things. We shall see what she is worth in the event.";
			}
			else
			{
				dialog.text = "Well then, a fair wind to you, " + PChar.name + ", and mind yourself if you are to fight the ''Piranha''. You know what she is capable of...";
				link.l1 = "...in the hands of a skilled captain. Few can master her besides me.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_16";
		break;

		case "TreacheryOfGrifoSajfus_16":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Mind they do not tan your hide out there, hero.";
				link.l1 = "Eh... We shall do without that.";
			}
			else
			{
				dialog.text = "Mind they do not tan your hide a second time, hero.";
				link.l1 = "Eh... There will be no second time.";
			}
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusXebecNewsInCuba");
		break;

		case "TreacheryOfGrifoSajfus_17":
			dialog.text = "And? What of it?";
			if (PChar.TreacheryOfGrifoSajfus.ChameleonSink == false)
			{
				// Gregg: without the prologue the player never laid eyes on the Piranha -
				// they judge by hearsay, not by familiar cabin fittings
				if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
				{
					link.l1 = "But the moment I took her, it was plain. They said that xebec could outrun anything, and this one barely crawls. She is simply a ship very like her.";
				}
				else
				{
					link.l1 = "But the moment I took her, I understood she was not the one. Wrong fittings below, wrong guns, wrong qualities. She is simply a ship very like her.";
				}
			}
			else
			{
				link.l1 = "But the moment I engaged her, I understood she was not the one. She behaved far too strangely even for an unskilled captain. She is simply a ship very like her.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_18";
		break;

		case "TreacheryOfGrifoSajfus_18":
			dialog.text = "Are you certain of it?";
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				link.l1 = "Entirely. Against the wind she runs half as fast as they tell of the ''Piranha'', her handling is wretched, she has no speed at all. She is not the one.";
			}
			else
			{
				link.l1 = "Entirely. Against the wind she runs half as fast as the ''Piranha'', her handling is wretched, she has no speed at all. She is not the ''Piranha''.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_19";
		break;

		case "TreacheryOfGrifoSajfus_19":
			dialog.text = "Then we shall go on looking. Look in from time to time, perhaps I shall learn something.";
			link.l1 = "Very well, " + NPChar.name + ".";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusItIsNotShip");
		break;

		case "TreacheryOfGrifoSajfus_20":
			dialog.text = "No rumours at all. Nothing new has been happening in the Caribbean of late. But listen to what I shall tell you. Yesterday I bought a pair of diamonds cheap from some travelling peddler, a gift for my wife. Devil take me, you should have seen those diamonds! Brilliance, top class! The peddler says he brought them from Africa.";
			link.l1 = "From Africa? You said from Africa?";
			link.l1.go = "TreacheryOfGrifoSajfus_21";
		break;

		case "TreacheryOfGrifoSajfus_21":
			dialog.text = "Yes. What of it? Never heard of them?";
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				link.l1 = "You said yourself that Sajfus was making for Dominica after a vessel with African diamonds. That is where he laid his comrade low, and where he took the ship.";
			}
			else
			{
				link.l1 = "Sajfus offered me an attack on a vessel said to be carrying African diamonds. We agreed to meet at Dominica, and there he took my ship from me.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_22";
		break;

		case "TreacheryOfGrifoSajfus_22":
			dialog.text = "And so? I do not see what you are driving at.";
			link.l1 = "That peddler who sold you the diamond said it came from Africa, yes?";
			link.l1.go = "TreacheryOfGrifoSajfus_23";
		break;

		case "TreacheryOfGrifoSajfus_23":
			dialog.text = "Aye, that is so.";
			link.l1 = "I would wager that diamond came from the ship Sajfus meant to attack.";
			link.l1.go = "TreacheryOfGrifoSajfus_24";
		break;

		case "TreacheryOfGrifoSajfus_24":
			dialog.text = "And?";
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				link.l1 = "Aboard the ''Piranha'' Grifo could surely have boarded that ship with the diamonds. What happened after, no one knows. And now a trader turns up with African diamonds. Do you see? He is bound up in this story somehow, and if I learn where he got the stones, I may learn what became of the xebec as well.";
			}
			else
			{
				link.l1 = "Aboard my ''Piranha'' Grifo could surely have boarded that ship with the diamonds. What happened after, no one knows. And now a trader turns up with African diamonds. Do you see? That peddler is bound up in this story somehow, and perhaps with the ''Piranha'' too, and if I learn where he got those diamonds, I may learn what became of my xebec.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_25";
		break;

		case "TreacheryOfGrifoSajfus_25":
			dialog.text = "That is nonsense. How will you learn it?";
			link.l1 = "I do not yet see how myself, but I am sure there is some connection. Do you happen to know where that trader went?";
			link.l1.go = "TreacheryOfGrifoSajfus_26";
		break;

		case "TreacheryOfGrifoSajfus_26":
			dialog.text = "He is probably still knocking about Bermuda, trying to sell the rest of the diamonds. Look for him, perhaps you will find him somewhere.";
			link.l1 = "Understood.";
			link.l1.go = "TreacheryOfGrifoSajfus_27";
		break;

		case "TreacheryOfGrifoSajfus_27":
			dialog.text = "Hey, " + PChar.name + ", when you find out what became of the ''Piranha'', look in on me. I am growing curious myself as to where she has got to.";
			link.l1 = "Agreed, " + NPChar.name + ", I shall tell you when I know.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusNeedTrader");
		break;

		// LEO: intermediate node - the hero delivers the news first. The menu line used to jump straight
		//   into the keeper reaction, so he mourned the sunken ship before a word was said to him.
		case "TreacheryOfGrifoSajfus_27_2":
			dialog.text = "Do not keep me waiting. What became of her?";
			link.l1 = "She is gone, " + NPChar.name + ". She lies on the sea floor, off Turks.";
			link.l1.go = "TreacheryOfGrifoSajfus_28";
		break;

		case "TreacheryOfGrifoSajfus_28":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "So that is how it is. And who sent her there?";
			}
			else
			{
				dialog.text = "Devil take it! I am sorry for your beauty, " + PChar.name + ", truly sorry. And who sent her to the bottom?";
			}

			if (PChar.TreacheryOfGrifoSajfus.MarlinSink == false)
			{
				link.l1 = "That I could not find out. But there is fine news too. I have taken another ship, far better than the ''Piranha''!";
				link.l1.go = "TreacheryOfGrifoSajfus_29";
			}
			else
			{
				link.l1 = "That I could not find out.";
				link.l1.go = "TreacheryOfGrifoSajfus_31";
			}
		break;

		case "TreacheryOfGrifoSajfus_29":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Better than the ''Piranha''? That I should like to see.";
			}
			else
			{
				dialog.text = "Hm, is this really you, " + PChar.name + "? I cannot believe you claim some ship could be better than your ''Piranha''.";
			}
			link.l1 = "She is not a ship, she is a work of art. Splendid handling, magnificent speed, guns just as they should be.";
			link.l1.go = "TreacheryOfGrifoSajfus_30";
		break;

		case "TreacheryOfGrifoSajfus_30":
			dialog.text = "This calls for a drink.";
			link.l1 = "Pour it out!..";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusEnd");
		break;

		case "TreacheryOfGrifoSajfus_31":
			dialog.text = "Well. She was a good ship.";
			link.l1 = "A superb ship.";
			link.l1.go = "TreacheryOfGrifoSajfus_32";
		break;

		case "TreacheryOfGrifoSajfus_32":
			dialog.text = "What, " + PChar.name + ", did you not hear me?";
			link.l1 = "She was not a good ship, she was a superb one.";
			link.l1.go = "TreacheryOfGrifoSajfus_33";
		break;

		case "TreacheryOfGrifoSajfus_33":
			dialog.text = "True. But enough grieving. Finish your rum, forget the ''Piranha'' and get on with some business or other.";
			link.l1 = "There is no one home for me today. Pour me a little more rum...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusEnd");
		break;
		// Gregg: THE TREACHERY OF GRIFO SAJFUS <--

		//шебека Синяя Птица
		case "BlueBird_1":
			dialog.text = NPCStringReactionRepeat("I hear a lot of things. They mostly say that the ship is very fast. They tried to catch it, but all in vain.",
				"We've already talked about this.",
				"Again? We've already found out everything.",
                "I'm sorry, if you don't need anything else, then I'll probably take care of other clients.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Who tried to catch it?",
				"Yes, that's right...",
                "Hmm, that's right.",
				"Okay...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("BlueBird_2", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
		break;

		case "BlueBird_2":
			dialog.text = "A lot of people tried, but they broke off, haha!!";
			link.l1 = "Do you know anything useful? Where it is often seen, for example.";
			link.l1.go = "BlueBird_3";
		break;
		case "BlueBird_3":
			dialog.text = "They see it here, near Bermudes. The Blue Bird has no equal in walking in our reefs, galleons and frigates have fought more than once in pursuit of it - and straight to the bottom!";
			link.l1 = "That's how it is... Well, let's look for it. Listen, what kind of person local storekeeper is?";
			link.l1.go = "BlueBird_4";
		break;
		case "BlueBird_4":
			dialog.text = "Highly reputable, respected among merchants.";
			link.l1 = "Understood... Is there a person who is not on very good terms with him?";
			link.l1.go = "BlueBird_5";
		break;
		case "BlueBird_5":
			dialog.text = "He has no outright enemies. But the former owner of the store, I think, has a grudge against him, hehe...";
			link.l1 = "Would you kindly tell me his name, my dear.";
			link.l1.go = "BlueBird_6";
		break;
		case "BlueBird_6":
			dialog.text = "His name is John Norton. You won't find him in Bermudes anymore. I heard that he moved to Puerto Principe, trades there on the sly and does not stick out anywhere.";
			link.l1 = "Thank you very much, buddy. You're a good innkeeper.";
			link.l1.go = "BlueBird_7";
		break;
		case "BlueBird_7":
			dialog.text = "Thank you, " + GetAddress_Form(NPChar) + ". Good luck!";
			link.l1 = "And have a nice day, buddy.";
			link.l1.go = "exit";
			AddQuestRecord("Xebeca_BlueBird", "3");
			pchar.questTemp.BlueBird = "toPuertoPrincipe";
		break;

		//голл. линейка, квест №8
 		case "Step_H8_1":
    		dialog.text = "No, I haven't heard.";
    		link.l1 = "It seems to be a well-known story, I was told it in Curacao.";
    		link.l1.go = "Step_H8_2";
 		break;
 		case "Step_H8_2":
    		dialog.text = "I don't know anything about it, I didn't have that in the tavern.";
    		link.l1 = "Thanks for the help.";
    		link.l1.go = "exit";
 		break;
		//Изабелла, ищем Атиллу
 		case "Step_Romantic_1":
    		dialog.text = "I know, he bought a house here in the city. So that's where it should be.";
    		link.l1 = "Thanks, buddy...";
    		link.l1.go = "exit";
 		break;
		//Пиратка, квест №3, поиски Лоу
		case "PL_Q3_1":
			dialog.text = "Appeared. He kept walking around, sniffing around. He gets hooked on one, then on the other... It seems he was interested in our shipyard.";
			link.l1 = "Thank you.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "9");
			pchar.questTemp.piratesLine = "KillLoy_toAlexus";
		break;

		case "PL_Q3_2":
			dialog.text = "So you've already asked about him.";
			link.l1 = "Maybe you remembered something?";
			link.l1.go = "PL_Q3_3";
		break;
		case "PL_Q3_3":
			dialog.text = "Yes, what's there to remember, he asked all about the shipyard, sat down with different people. By the way, Captain Goodley is sitting there, he was talking to Law.";
			link.l1 = "Captain Goodley?! Thanks, that's something.";
			link.l1.go = "exit";
			pchar.questTemp.piratesLine = "KillLoy_toGoogly";
		break;
		//Пиратка, квест №5, ОЗГ
		case "StepPL5Berm_1":
			dialog.text = "Yes, I just ran in. He asked where Orrie Bruce lived and left.";
			link.l1 = "And where does this Orry Bruce live?";
			link.l1.go = "StepPL5Berm_2";
		break;
		case "StepPL5Berm_2":
			dialog.text = "So, he lives in a two-story masonry house. As soon as you leave the tavern, the first house is on the left. That's where it lives...";
			link.l1 = "Thank you, you helped me a lot!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "10");
			pchar.questTemp.piratesLine = "PL5Hunter_toOrryHouse";
		break;

		case "GatrisBarman":
			dialog.text = "Yes, she's in the room. Would you like an audience?";
			link.l1 = "Yes.";
			link.l1.go = "GatrisBarmanIndoor";
			link.l2 = "Another time.";
			link.l2.go = "exit";
		break;
		
		case "GatrisBarmanIndoor":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
            DoReloadCharacterToLocation("Pirates_tavern_upstairs","goto", "goto1");
		break;	

		// Gregg: THE BLACK ORCHID (ported from Damned Destiny) -->
		case "MysteriousArtefact_1":
			dialog.text = "I was, captain, I was. An old acquaintance of yours turned up here on Bermudes - Frederik Levinson. Remember him?";
			link.l1 = "Levinson? Of course I remember. He left for Europe swearing he would never come back.";
			link.l1.go = "MysteriousArtefact_2";
		break;

		case "MysteriousArtefact_2":
			dialog.text = "Came back, as you see. He asked after you, then rushed off somewhere into the jungle - and has not been seen since. I do not like it, captain. You had better go after him.";
			link.l1 = "I shall. Thank you, old friend.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactStart");
		break;

		// Gregg: chapter 5 - the tavernkeeper points the way to Levinson. He is paid for
		// his silence and never asked the lodger's name, so he cannot connect the man to
		// Jackman's bounty until the player names him.
		case "BO_Levinson_1":
			dialog.text = "Levinson? Your friend, the one who used to sit at that very table? He is gone, captain. Once you put out to sea, he was seen no more. Though I do have a lodger upstairs - a quiet one, pays in advance, hardly ever leaves the room. Months he has been sitting there. I never asked his name: that is exactly what he pays me extra for.";
			link.l1 = "Months without leaving his room? A curious lodger you have.";
			link.l1.go = "BO_Levinson_2";
		break;

		case "BO_Levinson_2":
			dialog.text = "Half this town is hiding from someone, it is the common way. Jackman is after one such himself - ten thousand on his head. They say that three years ago the man sold his own corsairs to the Spaniards. Not a ship, not a share - the men he sailed with. We do not forgive that here.";
			link.l1 = "I have heard that tale. And I know it closer than I would like: one of those corsairs was a friend of mine.";
			link.l1.go = "BO_Levinson_3";
		break;

		case "BO_Levinson_3":
			dialog.text = "...(after a pause)... Then you understand why they put such money on his head. Only there is no one to collect it: they combed the jungle, crawled through the caves, turned the settlement upside down - and nothing. They decided he had sailed away from Bermudes... ...(breaking off)... Captain. Captain, what are you driving at?";
			link.l1 = "At the fact that they searched the bushes while he sat upstairs in your own tavern all along. Keep quiet, old friend, I shall say nothing to Jackman - I have my own account to settle with your lodger.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftFindLevinson");
		break;
		// Gregg: THE BLACK ORCHID <--

		// Gregg: The Black Orchid, chapter 7 -->
		case "BO_SM_Pir_1":
			dialog.text = "You have not heard? Well, there is a thing.";
			link.l1 = "I have not. Tell it.";
			link.l1.go = "BO_SM_Pir_2";
		break;

		case "BO_SM_Pir_2":
			dialog.text = "Somebody shot at Jackman. He has let nobody in since. All Bermudes is buzzing with it and it never reached you.";
			link.l1 = "I need to see him. Today.";
			link.l1.go = "BO_SM_Pir_3";
		break;

		case "BO_SM_Pir_3":
			dialog.text = "Then it is not me you want, it is Alan Barret. He stands at that door. Tell him you want Jackman and he will explain the arrangement.";
			link.l1 = "And where do I find Barret?";
			link.l1.go = "BO_SM_Pir_4";
		break;

		case "BO_SM_Pir_4":
			dialog.text = "At Broken Hulk Bay, where the old wreck lies. He is almost always there: quiet, and no spare eyes about. Only do not run your mouth at him, he does not care for it.";
			link.l1 = "Noted. I have nothing against quiet men, they give less trouble.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelFindBarret");
		break;
		// Gregg: <--
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
