// Gregg: Charly Grand Pipiska, the contact at the pearl fishery - quest
// "Goods for Richard Tillman" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\uneasydelivery.c
//
// Node 1 requires the key phrase to be typed in. The correct one is
// "That is fine, that is good" - Prenol's catchphrase, which Shon points out
// after reading the letter taken from Tillman's killer. It must match the wording
// in UD_Prenol.c (nodes 9 and 18) and UD_Shon.c (node 20).

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
			dialog.text = "You be the one who deal with Lion Heart? Say key phrase.";
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "2";
			NextDiag.TempNode = "1";
		break;

		case "2":
			string sString = GetStrSmallRegister(dialogEditStrings[1]);
			// Gregg: a homogeneous chain of || - mixing it with && in one if is not allowed
			if (sString == "that is fine, that is good" || sString == "that is fine that is good")
			{
				// Gregg: Damned Destiny had Charly hand over 25,000 - the killers' fee. The sums of the quest
				// are cut by a factor of ten (the hoard is 50,000 instead of 300,000),
				// and this one by the same, for consistency.
				AddMoneyToCharacter(PChar, 10000);
				dialog.text = "Here be your money.";
				link.l1 = "Listen, Charly, my men and I are unhappy with our slice of the pie, they want more. I need to see Prenol to demand more money from him. Where can he be found?";
				link.l1.go = "3";
			}
			else
			{
				dialog.text = "Key phrase wrong. Charly do no more business with White Brother.";
				link.l1 = "Argh!..";
				link.l1.go = "exit";
				AddDialogExitQuestFunction("UneasyDeliveryDialogWithCharlyFailed");
			}
		break;

		case "3":
			dialog.text = "Charly not know where Prenol be. Charly only hand over money. Charly do nothing more.";
			link.l1 = "Charly, you do not want trouble, do you? You do not? Tell me where Prenol is and you will have none.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Charly not catch what White Brother say. Charly not understand.";
			link.l1 = "You catch it well enough. I shall say it once more. You do not say where Prenol is - there is trouble, bad for Charly. You say where Prenol is - no trouble, good for Charly.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Charly know nothing, Charly only do his work, hand over money.";
			link.l1 = "Then it is trouble, Charly. Very well, I shall arrange some for you...";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "No! No! Charly say, Charly say! Look for Prenol on island Bermuda, on island Bermuda, Prenol there, Charly know for certain.";
			link.l1 = "Thank you.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "White Brother! Do not tell Prenol that Charly say this to you, do not give Charly away, Charly just not want bad.";
			link.l1 = "Very well, I shall not tell.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Charly not to blame, Charly not want bad. You not give Charly away?";
			link.l1 = "I shall not. Now leave me be.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryDialogWithCharlyGood");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
