// Gregg: Will Breddoks and Kharpich - «Чёрная Орхидея», глава 7 «Маленький ублюдок».
// Логика - Program\Quests\reaction_functions\blackorchid.c.

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
			dialog.text = "Curse that frigate.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(without raising his head)... Sit down, Captain. Drink to my crew with me.";
			link.l1 = "What became of them?";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelInPuertoPrincipeTalk");
		break;

		case "2":
			dialog.text = "There is no point telling it. You will not believe it. Nobody here does any more.";
			link.l1 = "I will believe it.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "We came to anchor in the bay and were making ready to go ashore. And then it went dark. Not the way it does in a squall, but all at once, as if something had been dropped over us. I pressed against the rail and looked, and I could not see my own hands.";
			link.l1 = "But you heard.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "I heard. A man makes that sound once in his life, Captain, and you never mistake it for anything after. It came from everywhere. From the whole deck at once.";
			link.l1 = "And you jumped.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "...(a long pause)... I jumped. The last thing I heard above the water was a blade going past my ear. I swam for the shore and waited the whole way to be caught.";
			link.l1 = "And in the morning?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "In the morning I came out on the shore and looked at my ship. She stood there empty. No bodies, no blood, no boat put down. That same day I sold her to Captain Kharpich for next to nothing and did not haggle.";
			link.l1 = "Where is Kharpich now?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "No idea at all. He weighed anchor yesterday evening. Make haste and you will run him down.";
			if (sti(PChar.money) >= BO_SM_WAKE_PRICE)
			{
				link.l1 = "I would drink to your men with you, but I must go. Here is a thousand, drink to them for me.";
				link.l1.go = "8";
			}
			else
			{
				link.l1 = "I would drink to your men with you, but I must go.";
				link.l1.go = "9";
			}
			AddDialogExitQuestFunction("SmallMongrelFindKharpich");
		break;

		case "8":
			AddMoneyToCharacter(PChar, -BO_SM_WAKE_PRICE);
			dialog.text = "...(looks at the coin, then at you)... I will. All thirty of them.";
			link.l1 = "Fare well, Breddoks.";
			link.l1.go = "exit";
		break;

		case "9":
			dialog.text = "Go on, Captain. And do not stay aboard her at night, do you hear? Not you, not your people.";
			link.l1 = "I hear you.";
			link.l1.go = "exit";
		break;

		case "20":
			dialog.text = "What the devil. The fight is not over yet.";
			link.l1 = "I want your ship, Kharpich. You I am willing to let go.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "You will have her over my body. And do not look so pleased. You will have her with everything that is aboard her.";
			link.l1 = "Over your body, then.";
			link.l1.go = "exit";
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattleNoParam");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
