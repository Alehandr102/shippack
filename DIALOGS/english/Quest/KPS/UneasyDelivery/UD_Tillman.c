// Gregg: Richard Tillman - quest "Goods for Richard Tillman" (ported from Damned Destiny).
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
			if (CheckAttribute(PChar, "UneasyDelivery.Cancel"))
			{
				dialog.text = "What else do you want?! We have already said all there is to say!";
				link.l1 = "Do not fret so, trader. I am leaving.";
				link.l1.go = "exit";
				break;
			}
			if (sti(PChar.UneasyDelivery) >= 3)
			{
				dialog.text = "Ah, it is you, captain " + GetFullName(PChar) + "? How do your affairs stand regarding my commission?";
				link.l1 = "I completed it long ago, " + NPChar.lastname + ".";
				link.l1.go = "exit";
				break;
			}
			if (sti(PChar.UneasyDelivery) > 1)
			{
				dialog.text = "Ah, it is you, captain " + GetFullName(PChar) + "? How do your affairs stand regarding my commission?";
				link.l1 = "I am hard at work on it.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = GetFullName(NPChar) + ", at your service. With whom have I the honour?";
			link.l1 = "Captain " + GetFullName(PChar) + ". I have brought you a consignment of cocoa.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Well, at last! Why were you so long in reaching Barbados? I have been waiting weeks. Explain yourself!";
			link.l1 = "That cocoa was to be delivered by captain Tommi Fatman, but it so happened that I took possession of his ship and your cargo. I learned it was to be brought to you and set course for Barbados without delay. There was no delay on my part, and why Fatman dawdled I do not know.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "It matters not who brought me the cargo, what matters is that it came terribly late. I hoped to sell it for decent money, but demand has fallen now, and for this cocoa I shall get precisely nothing.";
			link.l1 = "Wait a couple of months, demand will rise again, and then you will sell it.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Do not instruct me, captain " + GetHeroLastName() + "! I know perfectly well what to do and how. Now, to business: I am prepared to pay you 750 piastres for the delivery.";
			link.l1 = "750 piastres? That is at least half the sum I counted on.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "You did not keep to the terms, so the payment matches. Yes, you are not at fault, the captain who dawdled is. But you took on his obligation to deliver, did you not? You did. The cargo should have arrived weeks ago, should it not? It should. Well then, draw your conclusions.";
			link.l1 = "Devil take you, hand over your money.";
			link.l1.go = "6";
		break;

		case "6":
			AddMoneyToCharacter(PChar, 750);
			RemoveCharacterGoodsSelf(PChar, GOOD_CHOCOLATE, 50);
			dialog.text = "I am glad we have come to terms. My lads are already unloading your ship.";
			link.l1 = "...(through the teeth)... A pleasure doing business with you, " + NPChar.name + ". Farewell.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Wait, captain " + GetHeroLastName() + ", I have a commission for you, and if you carry it out in time you will earn well.";
			link.l1 = "What commission?";
			link.l1.go = "8";
		break;

		// Gregg: the tavern entry - without the prologue the player has neither the cocoa
		// nor the Fatman story, so the conversation begins straight at the commission
		case "30":
			dialog.text = GetFullName(NPChar) + ", at your service. The innkeeper sent you, I take it? I do indeed have business for a captain with a sound vessel.";
			link.l1 = "I am listening.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "To deliver a small consignment of porcelain to my associate Jacque Prenol in Port-au-Prince. Do it and you will receive 7000 piastres from him. Well, do you agree? I advise you not to refuse.";
			link.l1 = "I agree.";
			link.l1.go = "9";
			link.l2 = "No, I refuse.";
			link.l2.go = "11";
		break;

		case "9":
			dialog.text = "Excellent. Only, for God's sake, be careful with the porcelain. One careless move and it may break. And this time there is no time limit.";
			link.l1 = "Very well, I understand.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "You will give this letter to Prenol. That is all. A fair wind to you, captain " + GetHeroLastName() + "!";
			link.l1 = "Good day to you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryAgree");
		break;

		// Gregg: in Damned Destiny the refusal branch was commented out and never used.
		// Restored: the player is entitled to decline, and the quest already handles it.
		case "11":
			// Gregg: register the handler BEFORE DialogExit - calling a function with
			// a parameter after the dialog closes fails with "Invalid call"
			AddDialogExitQuestFunction("UneasyDeliveryCancel");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		// Gregg: player caught stealing. The short local variant of the block is used:
		// the Damned Destiny branches with a pay-off relied on GetStealMoneyLeaderShip /
		// GetStealMoneyLuck, which do not exist in CSP. Model - Graveyard_Watcher_dialog.c.
		case "Man_FackYou":
			dialog.text = LinkRandPhrase("Why, you are a thief! Guards, hold the thief!!!", "Well I never! I look away for a moment and you are head first in the chest! Stop, thief!!!", "Guards! Robbery!!! Stop the thief!!!");
			link.l1 = "Ah, the devil!!!";
			link.l1.go = "fight";
		break;

		case "fight":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LAi_SetOwnerTypeNoGroup(npchar);
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
