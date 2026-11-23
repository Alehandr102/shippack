// Gregg: John and Nils, Jacque Prenol's creditors - quest "Goods for Richard Tillman"
// (ported from Damned Destiny). The scene is overheard: the player stands aside, and both
// sides of the exchange are the creditors talking to Prenol and then to each other.
// Nodes 1-5 are led by John, 6-10 by Nils.
// Logic - Program\Quests\reaction_functions\uneasydelivery.c

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
		case "First time":
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "John: Well then, Prenol, debts are meant to be paid. We gave you a final term, until tonight, and now we are waiting for our money.";
			link.l1 = "Jacque Prenol: I could not bring it here, John, I...";
			link.l1.go = "2";
		break;

		// Gregg: in Damned Destiny the debt was "two hundred and fifty thousand", a quarter
		// of a million. The sums of the quest are cut by a factor of ten, so here it is
		// twenty-five thousand: exactly what the hoard holds over and above Shon's debt,
		// and the figures agree with one another.
		case "2":
			dialog.text = "John: What?! Prenol, did we not warn you, eh?";
			link.l1 = "Jacque Prenol: But I am not such a fool as to bring twenty-five thousand here, to Bermuda. Here they are ready to cut each other's throats for a couple of hundred! And what would they have done to me over the two weeks I waited for you, had they learned I carried such money?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "John: So you have not brought us the money, Prenol?";
			link.l1 = "Jacque Prenol: I hid it by the lighthouse. Here is the map. You have only to go there and take it.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "John: Jacque, we told you: do not bring the money here and you are finished!";
			link.l1 = "Jacque Prenol: No! Do not kill me, John, I have explained why I did not bring it here.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "John: That does not interest us. We told you the money was to be here today. Instead you send us off to some lighthouse. No, Prenol, we know you far too well to believe you really hid the money there.";
			link.l1 = "Jacque Prenol: No! No, John, do not kill me! No, I beg you...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryPrenolKill");
		break;

		case "6":
			dialog.text = "John: What an idiot. I told him: do not bring the money and I shall kill you and be damned.";
			link.l1 = "Nils: And what if we check that hoard by the lighthouse? Who knows, perhaps he really did hide the money there.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "John: And you believed him? Believed what he told us here about the lighthouse and the money?";
			link.l1 = "Nils: And what if it is true? What does it cost us to check? We have the map, and the time.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "John: Hm. And you are right, damn it!";
			link.l1 = "Nils: Then let us sail at once, why waste time.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "John: Hey, and who is that fellow?";
			link.l1 = "Nils: Whom do you mean?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "John: There he stands, eyes glittering.";
			link.l1 = "Nils: He was listening to us. He saw us kill Prenol. Let us finish him.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryBattleWithPrenolKillers");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
