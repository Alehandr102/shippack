// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("What questions?", "What do you want, " + GetAddress_Form(NPChar) + "?"), "Just recently you tried to ask me a question, " + GetAddress_Form(NPChar) + "...", "This is the third time you've talked about a question this day...",
                          "Are we going to ask questions again?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("I've changed my mind..", "I have nothing to talk about right now."), "Hmm, something happened to my memory...",
                      "Yeah, really for the third time...", "No, what questions...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// Gregg: "Goods for Richard Tillman" - where Shon Stablovski sits.
			// Sent Jons has no tavern file of its own, so the city is checked as well.
			if (PChar.UneasyDelivery == "6" && NPChar.city == "SentJons")
			{
				link.l8 = "I need Shon Stablovski. Where can he be found?";
				link.l8.go = "UneasyDelivery_1";
			}

		break;

		case "UneasyDelivery_1":
			dialog.text = "Shon? Shon Stablovski?";
			link.l1 = "Yes, he is the very man I need.";
			link.l1.go = "UneasyDelivery_2";
		break;

		case "UneasyDelivery_2":
			dialog.text = "Why, there he sits, at the far table.";
			link.l1 = "Thank you for the help.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryAntiguaDialogExit");
		break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

