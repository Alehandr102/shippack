// Gregg: Bob McCain, a drunkard in the pirate tavern - quest "Goods for Richard Tillman"
// (ported from Damned Destiny). Sells the place and time of Prenol's meeting for 5000.
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
			NextDiag.TempNode = "First time";
			// Gregg: read through CheckAttribute - in Storm, touching a missing attribute
			// is a runtime error, not "false", and the function plays on with garbage
			if (CheckAttribute(PChar, "UneasyDelivery.CancelMoneyForBobMcCain"))
			{
				if (PChar.UneasyDelivery.CancelMoneyForBobMcCain == true)
				{
					dialog.text = "Well then?";
					if (sti(PChar.money) >= 5000)
					{
						link.l1 = "Here is your money.";
						link.l1.go = "8";
					}
					else
					{
						link.l1 = "I do not have the sum yet.";
						link.l1.go = "11";
					}
					link.l2 = "Nothing for now.";
					link.l2.go = "11";
					break;
				}
			}
			dialog.text = "Cap, sit down, have a drink with me!";
			link.l1 = "I have no time.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Hey, cap, care for a drink?";
			link.l1 = "No desire. And I am busy.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "In your place I would drink. You might come by useful information. Well, will you sit down?";
			link.l1 = "Hm...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryDialogWithMcCain");
		break;

		case "4":
			dialog.text = "There, that is better!";
			link.l1 = "Very well, out with it, what have you got?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Not so fast, cap, let us first discuss one thing. Information is not free, as you understand, so you will have to pay. Five thousand ringing piastres, and I shall tell you where to look for Prenol.";
			link.l1 = "How do you know that I am looking for him?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "I heard you gabbing about Prenol with the innkeeper.";
			link.l1 = "Let us do it this way: I shall pay you, but only if your information proves useful.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "No, cap, that is not how business is done. First you pour five thousand into my pocket, and only then do I tell you everything. Otherwise - no money, no information.";
			if (sti(PChar.money) >= 5000)
			{
				link.l1 = "Very well, here is the money. Now tell me where Prenol is.";
				link.l1.go = "8";
			}
			else
			{
				link.l1 = "I shall return when I have the money.";
				link.l1.go = "11";
			}
			link.l2 = "If I decide on it, I shall come to you for help.";
			link.l2.go = "11";
		break;

		case "8":
			AddMoneyToCharacter(PChar, -5000);
			PChar.UneasyDelivery.CancelMoneyForBobMcCain = false;
			dialog.text = "Now then, cap, listen carefully, I do not like repeating myself. I was sitting today as usual over a mug of rum and heard three men talking. One of them was that Prenol you are looking for. \n Now, those two who spoke with Prenol set him a meeting at two in the morning at the bay of the broken tub.";
			link.l1 = "And how do you know one of them was Prenol?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Those two addressed him as nothing but Prenol. So rest assured, that is the man you seek.";
			link.l1 = "Very well, I believe you. But if you have lied to me and nobody is there tonight, I shall find you and...";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "I vouch for the information, cap. Be assured, it is exact.";
			link.l1 = "Mind you do. If you have made a fool of me, I shall find you and bury you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryDialogWithMcCainEnd");
		break;

		case "11":
			ChangeCharacterAddressGroup(PChar, "Pirates_tavern", "goto", "goto2");
			LAi_SetPlayerType(PChar);
			LAi_SetSitType(CharacterFromID("Bob McCain"));
			PChar.UneasyDelivery.CancelMoneyForBobMcCain = true;
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
