// Gregg: the grotto guards - quest "The Monkey Ship" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\captainmonkey.c
// The skeletons stand still until they are struck, so the player CAN start a talk -
// and without this file the game threw "Error opening dialogue file". They have
// nothing to talk with, so the whole exchange is a rattle and a guess at its meaning.

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
			dialog.text = RandPhraseSimple("Hhhhrrrr...", "Khrrr... sssshhh...");
			link.l1 = RandPhraseSimple("I have no idea what that means.", "Let us call that a conversation.");
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
