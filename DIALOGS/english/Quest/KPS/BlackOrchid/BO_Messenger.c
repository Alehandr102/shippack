// Gregg: the messenger from Bermudes - the "Black Orchid" trilogy (ported from
// Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// In the original these two nodes sat in the shared Pirates.c file as 8 and 9;
// here the messenger has his own file so we need not drag in a whole foreign dialogue.

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
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Are you captain " + GetFullName(PChar) + "?";
			link.l1 = "I am. What do you want?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I was sent by " + GetFullName(CharacterFromID("Pirates_tavernkeeper")) + ", the keeper of the tavern on Bermudes. He asks you to call on him - says he has news that will interest you.";
			link.l1 = "Is that so. Very well, tell him I shall pay him a visit.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactFirstToBermudes");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
