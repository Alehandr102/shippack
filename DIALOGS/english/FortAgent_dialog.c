// Deck parley with the enemy-nation courier-agent (commandant convoy interception at sea).
// The encounter is created in Treasure.c (FortAgentOnMap); outcome logic is there too (FortAgent_HandOver/StartDeliver).
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iBase = sti(pchar.CargoQuest.iMoney);

	switch (Dialog.CurrentNode)
	{
		case "exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

		case "First time":
			// сделка уже заключена ранее - предложений больше нет (фантом мог пересоздаться и потерять DontDeskTalk)
			if (CheckAttribute(pchar, "FortAgentResolved"))
			{
				dialog.text = "The word is spoken, captain. We've nothing more to discuss.";
				link.l1 = "As you say.";
				link.l1.go = "exit";
				NextDiag.TempNode = "First time";
				break;
			}
			dialog.text = "Stay your blade, captain, I came to talk, not to fight. I am a trusted hand of the Crown of " + NationNameGenitive(sti(NPChar.nation)) + ", and the Crown has eyes and ears everywhere. By our intelligence reports, the holds of your convoy carry a shipment of munitions bound for a fort whose nation is at enmity with us. Such cargo has no business in the arsenal of our enemies. Here is your choice: hand the convoy ships over to my men - and I'll pay twice what the guild promised; or deliver the cargo yourself to our fort - and they'll pay fivefold. Refuse, and we'll settle it on the open sea.";
			link.l1 = "Agreed. Take the convoy. (" + FindRussianMoneyString(iBase * 2) + ")";
			link.l1.go = "FortAgent_Hand";
			link.l2 = "I'll bring the cargo to your fort myself. (" + FindRussianMoneyString(iBase * 5) + ")";
			link.l2.go = "FortAgent_Deliver";
			link.l3 = "I'm carrying this cargo to its address. Out of my way.";
			link.l3.go = "FortAgent_Refuse";
			NextDiag.TempNode = "First time";
		break;

		case "FortAgent_Hand":
			// convoy hand-off + agent departure happen ON DIALOG EXIT (otherwise softlock: despawn during the dialog)
			AddDialogExitQuestFunction("FortAgent_HandOver_OnExit");
			dialog.text = "A wise choice, captain. My men will take the ships - the rest is our concern. Here is your gold, twice what the guild promised. We part ways, and let us forget one another.";
			link.l1 = "Agreed.";
			link.l1.go = "exit";
		break;

		case "FortAgent_Deliver":
			FortAgent_StartDeliver(NPChar);                          // deal/deadline/city now (needed for the text)
			AddDialogExitQuestFunction("FortAgent_Depart_OnExit");   // agent leaves ON EXIT (otherwise softlock)
			dialog.text = "Done, captain. Bring the convoy to the fort of " + XI_ConvertString("Colony" + pchar.FortAgentDeliver.city + "Gen") + ". Here, take my recognition flag - run it up your mast: under it both the shore batteries and the city guard will take you for one of ours, carrying vital cargo for the garrison, and will not lay a finger on you. Its commandant awaits the munitions and will pay fivefold the guild's price for delivering them. You have " + FindRussianDaysString(sti(pchar.FortAgentDeliver.days)) + " from this hour; late goods will be of no interest to us.";
			link.l1 = "The cargo will reach the fort.";
			link.l1.go = "exit";
		break;

		case "FortAgent_Refuse":
			dialog.text = "A pity. So we could not come to terms. Then we shall speak in another tongue - out on the open sea. Make sail, captain, and pray to your god.";
			link.l1 = "We'll see whose day it is.";
			link.l1.go = "exit";
			// hostility goes on the REAL agent ON EXIT (the deck speaker may be a clone - hence "couldn't attack")
			AddDialogExitQuestFunction("FortAgent_Refuse_OnExit");
		break;
	}
}
