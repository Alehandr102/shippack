// LEO: Hired privateer (caper) in the tavern. Offers to capture a ship for a fee.
// Logic - scripts/HiredPirate.c. Yellow "?" over head (isHiredCaper -> LAi_citizen.c).
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int fee = 40000;
	if (CheckAttribute(NPChar, "hpFee")) fee = sti(NPChar.hpFee);
	int deposit = fee / 2;

	switch (Dialog.CurrentNode)
	{
		case "exit":
			DialogExit();
		break;

		case "First time":
			if (HiredPirate_IsActive()) // уже нанят - не предлагаем снова (страховка от повторного найма)
			{
				dialog.text = "We've a deal already, captain. I'm off to earn my coin. Ask the barmen for me.";
				link.l1 = "Right. Good hunting.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "(leans in, low) You've the look of a captain who could use another hull under his flag and isn't fussy how it's come by. I'm a privateer for hire. Point me at these waters and I'll take a prize for you - the ship's yours. My fee's by the size of her: " + FindRussianMoneyString(fee) + " all told, half now, half when you collect her in the bay.";
			link.l1 = "How does the handover work?";
			link.l1.go = "explain";
			link.l2 = "Not interested.";
			link.l2.go = "decline";
		break;

		case "explain":
			dialog.text = "Simple. You pay me half now. I go hunting - give it ten days to a fortnight. When she's taken, I'll lie up in some quiet bay and wait. The barmen are a loose-tongued lot and in on my cut: ask them after me, and once I'm ready one of them will whisper you which cove I'm in. Sail there with the rest of my coin and a spare officer aboard - she'll need a captain, and I'm not staying. Dawdle more than a week after a barman tips you off, and I'll sell her elsewhere. Deal?";
			if (sti(pchar.money) >= deposit)
			{
				link.l1 = "Deal. Here's your half. (" + FindRussianMoneyString(deposit) + ")";
				link.l1.go = "hire";
			}
			else
			{
				link.l1 = "I don't have " + FindRussianMoneyString(deposit) + " on me.";
				link.l1.go = "nomoney";
			}
			link.l2 = "I'll think on it.";
			link.l2.go = "decline";
		break;

		case "hire":
			HiredPirate_Hire(NPChar);
			DeleteAttribute(NPChar, "isHiredCaper"); // снять флаг метки (иначе тик LAi_sit перерисует "?")
			UnmarkCharacter(NPChar);                 // и саму метку - прямо здесь, у собеседника
			dialog.text = "Half in hand - the rest when you take delivery. I'm off, then. Remember: ask the barmen, and bring a spare officer to the bay. Fair winds, captain.";
			link.l1 = "Good hunting.";
			link.l1.go = "exit";
		break;

		case "nomoney":
			dialog.text = "Come back when your purse is heavier, captain. I don't sail on promises.";
			link.l1 = "Later, then.";
			link.l1.go = "exit";
		break;

		case "decline":
			dialog.text = "Suit yourself. You know where the taverns are if you change your mind - though I don't linger.";
			link.l1 = "Farewell.";
			link.l1.go = "exit";
		break;
	}
}
