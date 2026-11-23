// Cargo swap - governor freight fork. Dealer in the origin port ("First time") offers the swap;
// the customs inspector at delivery ("Inspector_Check") checks the cargo, then WITHOUT a re-click
// the word passes to the governor (GovSwap_AfterInspector -> Common_Mayor.c node GovFreight_SwapResult).
// Logic - scripts/GovSwap.c. Rolls are baked at take-time (quests_reaction.c).
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
		case "exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

		//============ THE DEALER (origin port) ============
		case "First time":
			dialog.text = RandPhraseSimple(
				"(low, catching up to you) A word, captain. Carrying a state order for His Excellency? A gold mine, and you don't even know it. Here's the deal: the real goods come to me, and you hand the residence cheap filler under the same manifest. The governor's clerk won't look twice.",
				RandPhraseSimple(
					"(falls into step beside you, quietly) Don't turn around, captain, just listen. You've a state cargo in your hold - I know it. Sign it over to me, and deliver something cheaper to the governor under the same seal. The papers will tally, no one will look.",
					"(spits, lowers his voice) Captain, there's good money here. His Excellency's order is pure gold. The real goods to me, a cheap substitute to the residence. The old clerk hasn't told the difference under a state seal in years."
				)
			) + " For your trouble I pay " + FindRussianMoneyString(GovSwap_Bonus()) + " right now. Deal?";
			link.l1 = "Deal. Take the goods. (" + FindRussianMoneyString(GovSwap_Bonus()) + ")";
			link.l1.go = "GovSwap_Accept";
			link.l2 = "No. I'll deliver the state cargo as it is.";
			link.l2.go = "GovSwap_Decline";
			NextDiag.TempNode = "First time";
		break;

		case "GovSwap_Accept":
			GovSwap_DoSwap();
			AddDialogExitQuestFunction("GovSwap_DealerLeave");
			dialog.text = RandPhraseSimple(
				"Now that's talking! My men will collect the goods tonight, and you get plain, cheaper goods under the state seal. Hand them over boldly: if customs doesn't poke its nose in, no one will be the wiser. Fair winds, captain.",
				"A wise choice, captain. By morning a substitute will lie where your cargo was - indistinguishable, until someone cuts it open. And they're hardly likely to. Here's the agreed sum, now off you go."
			);
			link.l1 = "Be seeing you.";
			link.l1.go = "exit";
		break;

		case "GovSwap_Decline":
			AddDialogExitQuestFunction("GovSwap_DealerLeave");
			dialog.text = RandPhraseSimple(
				"Honesty's a costly thing these days... a pity. Well, your choice, captain. There was profit to be had, but as you will.",
				"Suit yourself. Just don't bite your elbows later, captain - orders like this don't come twice. Safe travels."
			);
			link.l1 = "Farewell.";
			link.l1.go = "exit";
		break;

		//============ THE CUSTOMS INSPECTOR (check at delivery) ============
		// after this dialog the word passes to the governor (GovSwap_AfterInspector) - no need to re-click the mayor
		case "Inspector_Check":
			AddDialogExitQuestFunction("GovSwap_AfterInspector");
			if (CheckAttribute(pchar, "CargoQuest.GovSwapCaught"))
			{
				pchar.CargoQuest.GovSwapInspected = "caught";
				dialog.text = RandPhraseSimple(
					"(looks the cargo over closely, checking the stamps and weight against the manifest, and frowns) ...And this, captain, is not what His Excellency's manifest lists. A substitution.",
					"(walks around the cargo, taps it, checks the ledger and shakes his head) The stamps are wrong, the weight doesn't match. This is not the state cargo, captain. A substitution, and a plain one."
				) + " You'll explain yourself - not to me, but to the governor.";
				link.l1 = "(say nothing)";
				link.l1.go = "exit";
			}
			else
			{
				pchar.CargoQuest.GovSwapInspected = "passed";
				dialog.text = RandPhraseSimple(
					"(glances over the cargo, checks his paper) Hm... all in order, the goods match. Go to His Excellency for your payment, captain.",
					"(walks along the cargo, checks a couple of stamps, nods) All by the ledger, captain. Collect your payment from His Excellency."
				);
				link.l1 = "Thank you for your diligence.";
				link.l1.go = "exit";
			}
		break;
	}
}
