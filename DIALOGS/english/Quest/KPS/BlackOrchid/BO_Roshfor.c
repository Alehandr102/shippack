// Gregg: Alan Rochefort, sailing master of the Anna - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "We await your word, Captain.";
			link.l1 = "We stay for now.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			// LEO: срок в три дня относится к приходу «Анны», а не к исчезновению Мориса.
			dialog.text = "You have come for Moris? You are late. He went ashore and never came back.";
			link.l1 = "And who are you?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Alan Rochefort, sailing master. The Anna is his, but I sail her, because these two years Moris has looked in one direction only.";
			link.l1 = "Toward Bruce Fisher.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "That very one. Fisher father took the Seeker from Moris some twenty years back, and there has been nothing else in his head since. When Rick died, Moris turned to the son.";
			link.l1 = "Where did he go ashore?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "The far shore of Dominica. He was told the Seeker was careening there. He took a pistol, rowed ashore and forbade anyone to follow. I brought the Anna to San Juan without him.";
			link.l1 = "Show me the place.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToCaiman");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
