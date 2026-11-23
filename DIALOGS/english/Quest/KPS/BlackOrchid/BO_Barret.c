// Gregg: Alan Barret - «Чёрная Орхидея», глава 7 «Маленький ублюдок».
// Логика - Program\Quests\reaction_functions\blackorchid.c.
// После главы 7 Баррет остаётся привратником Джекмена насовсем: вход в резиденцию
// платный, разово или на месяц. Так в оригинале К:ПС.

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
			if (Locations[FindLocation("Pirates_town")].reload.l3.disable == 1)
			{
				dialog.text = "Jackman, is it? You know the rate.";
				link.l1 = "I know it. I am paying.";
				link.l1.go = "Open_1";
				link.l2 = "Another time.";
				link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Door is open. Go on.";
				link.l1 = "I am going.";
				link.l1.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;

		case "Open_1":
			dialog.text = "The once, or the month?";
			link.l1 = "The once. A thousand.";
			link.l1.go = "Open_2";
			link.l2 = "The month. Fifteen thousand.";
			link.l2.go = "Open_4";
			link.l3 = "Neither.";
			link.l3.go = "exit";
		break;

		case "Open_2":
			if (sti(PChar.money) >= BO_SM_VISIT_PRICE)
			{
				dialog.text = "Door is open. The once.";
				link.l1 = "Noted.";
				link.l1.go = "Open_3";
			}
			else
			{
				dialog.text = "You are light. Come back with coin.";
				link.l1 = "I will.";
				link.l1.go = "exit";
			}
		break;

		case "Open_3":
			BO_BermudesResidenceOpen(BO_SM_VISIT_PRICE, "o");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Open_4":
			if (sti(PChar.money) >= BO_SM_MONTH_PRICE)
			{
				dialog.text = "The month is yours. Counting from today.";
				link.l1 = "Noted.";
				link.l1.go = "Open_5";
			}
			else
			{
				dialog.text = "You are light. Come back with coin.";
				link.l1 = "I will.";
				link.l1.go = "exit";
			}
		break;

		case "Open_5":
			BO_BermudesResidenceOpen(BO_SM_MONTH_PRICE, "m");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "1":
			dialog.text = "What do you want, pup?";
			link.l1 = "Mind your words, Barret.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "You know who I am? Alan Barret. Forty years on these waters.";
			link.l1 = "Forty years is an age, not an argument.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(grins)... My argument is on my belt. Care to see it?";
			link.l1 = "I want Jackman, not your belt.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Jackman is busy. For you he is always busy.";
			link.l1 = "Did he decide that, or did you?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "While I stand at the door, I did. Clear off, pup.";
			link.l1 = "Call me pup once more and you will regret it.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Pup.";
			link.l1 = "...(draws)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelBattleWithBarret");
		break;

		case "7":
			dialog.text = "...(sits propped against the wreck)... Enough. Enough, do you hear.";
			link.l1 = "Where did you learn to fight like that?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Forty years, lad. I told you. In those forty years I have been put down three times, and you are the third. I will not lie about it.";
			link.l1 = "I need Jackman. Now.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "That can be arranged. For coin.";
			link.l1 = "Paying for a conversation?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Paying for quiet. Since that shot he has taken a liking to it. And I stand here and count.";
			link.l1 = "I heard about the shot in the tavern. Did they find the man?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "...(looks away)... A man who is not around any more. Do not ask further.";
			link.l1 = "How much.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "A thousand for the once. Or fifteen, and for a month you walk in like it is your own house.";
			link.l1 = "And half of that stays with you, does it not?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "...(without a trace of embarrassment)... Less. But it stays.";
			link.l1 = "At least you do not lie.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "No reason to lie, there is only the one door. Well then, are you paying?";
			link.l1 = "I will think on it. I will find you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelToJackman");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
