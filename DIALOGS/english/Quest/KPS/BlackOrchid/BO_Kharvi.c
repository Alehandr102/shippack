// Gregg: Harvey Murom, once of Bartelemi squadron - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "Leave me be, I am busy.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Look what the wind brought in. What do you want from old Harvey?";
			link.l1 = "They say you sailed in Moris Bartelemi squadron.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(sets down his mug)... I did. Four years, and all four we were hunting one man.";
			link.l1 = "Bruce Fisher.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Him. We turned over the whole Main. ...(smirks)... And all that while he was under our noses, and never entered a port at all.";
			link.l1 = "What do you mean, never entered?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Just so. The Seeker stays on the water the year round. Provisions come out to her by boat, for coin. No customs, no harbour men, no papers. His father lived the same way at the end.";
			link.l1 = "And where does she lie?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "He shifts. Two anchorages, back and forth between them and nowhere else. I will name them for you, on one condition.";
			link.l1 = "Which is?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "If you take him alive, ask him why he killed Moris. ...(looks at the table)... Moris paid my wages for four years. And now he's gone.";
			link.l1 = "I will ask.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyGetKharviInformation");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
