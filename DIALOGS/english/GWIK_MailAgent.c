// Gregg: a rival agent - intercepting the GWIC "hot parcel" on the town streets.
// Spawned when the player enters the office's town (GWIK_MailAgentArrive), walks up to the PC.
// Style - conspiratorial, low-voiced, unlike the prim office manager.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int i;
	int cash;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			if (!CheckAttribute(pchar, "GWIK.Job") || pchar.GWIK.Job != "mail" || !CheckAttribute(pchar, "GWIK.Job.Hot") || CheckAttribute(pchar, "GWIK.Job.HotResolved"))
			{
				dialog.text = "My mistake. On your way, friend.";
				link.l1 = "...";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Don't turn round sharp, Captain, just listen. That sealed parcel they slipped you in the office is worth a good deal more than a courier's wage. I'll pay well for it - or at least to let my clerk take a copy. Balk at that, and, well, your funeral - but don't come crying after.";
			// Gregg: we missed each other - he walked out of town mid-approach. The agent brings it up (counter kept by GWIK_MailAgentLeft)
			if (CheckAttribute(pchar, "GWIK.Job.HotDodge")) dialog.text = "Quick on your feet, Captain. Last time you walked off before I'd finished - thought I'd lose interest? Same talk: that sealed parcel from the office is worth more than a courier's wage. Good price, or at least a copy. And if not - you know how it goes.";
			link.l1 = "What's your price for the parcel?";
			link.l1.go = "sell_ask";
			link.l2 = "A copy, then - but the parcel stays with me.";
			link.l2.go = "copy";
			link.l3 = "Keep your hands off Company papers.";
			link.l3.go = "refuse";
		break;

		case "sell_ask": // LEO: price first - the parcel changes hands only once the player agrees
			cash = makeint(sti(pchar.GWIK.Job.Pay) * 2);
			dialog.text = "Twice what the office will pay you: " + FindRussianMoneyString(cash) + ". Hand over the parcel, and we never met.";
			link.l1 = "Deal. Take the parcel.";
			link.l1.go = "sell";
			link.l2 = "A copy, then - but the parcel stays with me.";
			link.l2.go = "copy";
			link.l3 = "No deal. Keep your hands off Company papers.";
			link.l3.go = "refuse";
		break;

		case "sell": // sell the parcel - assignment failed, a bribe, reputation loss
			cash = makeint(sti(pchar.GWIK.Job.Pay) * 2);
			AddMoneyToCharacter(pchar, cash);
			GWIK_AddRep(-15);
			AddQuestRecord("GWIK_MailJob", "5");
			AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_MailJob");
			pchar.GWIK.Job.HotResolved = true;
			DeleteAttribute(pchar, "GWIK.Job");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Sensible. Here's your gold, "+ FindRussianMoneyString(cash) +". I'll see to the parcel myself. Be seeing you, Captain.";
			link.l1 = "Done.";
			link.l1.go = "exit";
		break;

		case "copy": // let him copy it - intel (archipelago price list), reputation loss, you still deliver the parcel
			for (i = 0; i < MAX_COLONIES; i++)
			{
				ref rcolony = &Colonies[i];
				if (rcolony.id != "Caiman" && rcolony.id != "Panama" && rcolony.nation != "none") SetPriceListByStoreMan(rcolony);
			}
			PlayStereoSound("notebook");
			GWIK_AddRep(-5);
			AddQuestRecord("GWIK_MailJob", "6");
			pchar.GWIK.Job.HotResolved = true;
			dialog.text = "Now that's good business. While the clerk copies it - take a look yourself: the freshest prices across the whole archipelago, they'll serve you well. The parcel's yours, carry on. A pleasure.";
			link.l1 = "Interesting. Farewell.";
			link.l1.go = "exit";
		break;

		case "refuse": // Gregg: refusing is no longer free - the agent whistles up his men and demands the packet by force
			dialog.text = "Poor choice, Captain. That was the polite request... (he gives a short whistle, and men with knives stroll out of the alleys) The packet. Here. Or we take it off your body - makes no odds to us.";
			link.l1 = "Take it. Another man's papers aren't worth my blood.";
			link.l1.go = "hand_over";
			link.l2 = "Come and take it.";
			link.l2.go = "GWIK_MailFight";
		break;

		case "hand_over": // handed over under threat - packet lost, no money, reputation down
			pchar.GWIK.Job.HotResolved = true;
			GWIK_AddRep(-10);
			AddQuestRecord("GWIK_MailJob", "7");
			CloseQuestHeader("GWIK_MailJob");
			DeleteAttribute(pchar, "GWIK.Job");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Sensible. (the packet vanishes inside his coat) On your way, Captain. And I'd not go complaining to the Dutchmen - it would cost you dearer.";
			link.l1 = "...";
			link.l1.go = "exit";
		break;

		case "GWIK_MailFight": // Gregg: the brawl. Backup is 1 + the player's officers; no leaving the location until the whole gang is dead (GWIK_MailAgentFightStart)
			pchar.GWIK.Job.HotResolved = true;
			AddDialogExitQuestFunction("GWIK_MailAgentFightStart"); // the fight starts AFTER the dialog closes
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "exit":
			GWIK_MailAgentDismiss(NPChar); // the agent leaves (as an actor, BEFORE DialogExit - otherwise "Invalid function call")
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
