// Gregg: Matthew Grehom, leader of the corsair band - the "Black Orchid" trilogy
// (ported from Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// The meeting in the Cuban jungle before the assault on Santiago. Grehom means to
// storm the fort head-on; the player proposes taking the defenceless town, and is refused.
// In the original these nodes shared a file with the Bermudes messenger; ours lives
// in BO_Messenger.c.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sName = GetHeroLastName();

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Well met, captain " + sName + "! I am Grehom, Matthew Grehom, leader of this corsair band. My lads are fine fighters, but without your help we shall not manage. The risk is great, but if we free Jason we shall pay you a hundred thousand piastres. Time is short - decide.";
			link.l1 = "Attacking the fort a second time is pointless. The Spaniards have prepared their defence well, several guns cover the road you would take, and considerable forces have been moved there from the town.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Say no more, captain " + sName + ", I understand: you will not help. I shall not press you. But think again before you refuse. A hundred thousand is no small sum.";
			link.l1 = "On the contrary, I do want to help. Only my plan is a different one.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Out with it, " + sName + ". And be quick.";
			link.l1 = "I propose we strike the town itself. The main Spanish force is in the fort now, and Santiago is left uncovered. While they hold their defence and wait for an attack, we take the defenceless town, cut our way into the residence and seize the governor - and with him a very influential lady, senora Matilda. From there, two roads. The first: we threaten to kill them both unless the Spaniards lay down their arms and hand over Jason.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "The Spaniards are no fools. They will storm the town, and then...";
			link.l1 = "And that is the second road. If they storm it, we shall not free Jason and shall have to withdraw by sea to Puerto Principe.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Are you fool enough, " + sName + ", to hope I would agree to this?";
			link.l1 = "And are you fool enough, Grehom, to throw yourself and all your men against a well-fortified fort? Do you truly believe a hundred corsairs will stand against an enemy who outnumbers and outguns you?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "I will wager that with your help or without it I shall cut my way into that prison, captain " + sName + ".";
			link.l1 = "In that case I can only wish you luck, Grehom.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "...(with malice)... And luck to you, captain " + sName + ". Farewell.";
			link.l1 = "Good day to you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInSantiagoJungleDialog");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
