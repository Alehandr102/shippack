// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{

	switch (Dialog.CurrentNode)
	{
		case "quests":
			// Gregg: The Black Orchid, chapter 7 - provisions for the frigate, and the dock hands
			if (CheckAttribute(PChar, "SmallMongrel"))
			{
				if (PChar.SmallMongrel == "7")
				{
					link.l30 = "The question will sound odd, but has a heavy frigate put into your bay?";
					link.l30.go = "BO_SM_ST_1";
				}
				if (PChar.SmallMongrel == "12")
				{
					link.l31 = "I am looking for two dock hands, Frank and Janson. They...";
					link.l31.go = "BO_SM_ST_3";
				}
			}
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("What questions?", "What do you want?"), "Some time ago, while in my store, you tried to ask some question...", "Today is the third question. I need to trade, not talk nonsense...",
                          "More questions? Maybe we should trade better?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("I changed my mind...", "I have nothing to talk about right now."), "Yes, exactly, I tried...",
                      "Yeah, really for the third time...", "Hmm, maybe we'll trade...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.piratesLine == "KillLoy_toSeek")
			{
				link.l1 = "Excuse me, did Edward Law buy a ship from you?";
				link.l1.go = "PL_Q3_1";
			}
		break;
		//пиратка, квест №3, поиски Лоу
		case "PL_Q3_1":
			dialog.text = NPCStringReactionRepeat("Me?! You're confusing me with someone else, I'm a trader, but with ships... I don't sell ships! You, "+ GetSexPhrase("young man", "girl") +" need to contact the shipbuilders at the shipyard. Yes, to them, and to me for everything else!",
				"You've already asked. No, he didn't buy it.",
				"No, "+ GetSexPhrase("young man", "young lady") +", no...",
                "Oh, what a bore you are!!!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Yes, but there is no shipyard in your town!",
				"I see...",
                "Yeah, I see...",
				"Hmm, I'm already leaving...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q3_2", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q3_2":
			dialog.text = "I can not help, if I wish the shipyard will not appear. Ask someone else...";
			link.l1 = "Right... well, thanks for that.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "3");
		break;

		// Gregg: The Black Orchid, chapter 7 -->
		case "BO_SM_ST_1":
			dialog.text = "One did. My lads loaded provisions into her, half a day of carrying.";
			link.l1 = "Did her captain say where he was bound?";
			link.l1.go = "BO_SM_ST_2";
		break;

		case "BO_SM_ST_2":
			dialog.text = "He said nothing at all. Paid without haggling and went. I like that sort of customer.";
			link.l1 = "Paid without haggling and said nothing. In a hurry, then. Thank you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelInLeFransua");
		break;

		case "BO_SM_ST_3":
			dialog.text = "...(cutting in)... They have been abducted!";
			link.l1 = "What?!";
			link.l1.go = "BO_SM_ST_4";
		break;

		case "BO_SM_ST_4":
			dialog.text = "...(choking on laughter)... Abducted and locked up. In the back room. So Frank does not slip off for rum before opening. Go through, they are both in there.";
			link.l1 = "Very funny. Take me to these captives of yours.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelToLeFransuaStoreRoom");
		break;
		// Gregg: <--
	}
	UnloadSegment(NPChar.FileDialog2);
}

