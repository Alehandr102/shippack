// Gregg: Raul Esperanto - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			// Gregg: the cane has been asked for but not yet brought
			if (CheckAttribute(PChar, "RaulEsperanto.Stick"))
			{
				dialog.text = "Did you find it?";
				link.l1 = "Not yet. I am looking.";
				link.l1.go = "exit";
			}
			else
			{
			// Gregg: gratitude only once the old man is actually out, that is at "12".
			// At "11" he still sits in the cellar and has nothing to thank anyone for.
			if (PChar.RaulEsperanto == "12")
			{
				dialog.text = "I do not know how to repay you, Captain.";
				link.l1 = "There is no need.";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "What do you want... Can you not see I am badly enough off without you.";
				link.l1 = "Bear it a little longer, Raul.";
				link.l1.go = "exit";
			}
			}
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Who is there. I cannot see you.";
			link.l1 = "Captain " + GetHeroLastName() + ". I have come to take you out of here.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(a long silence)... Nobody is taken out of here. They are carried out.";
			link.l1 = "Today you walk out. Can you walk?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "No. I stand with difficulty. I need a stick, any stick. I had a cane; they took it when they arrested me and I do not suppose they threw it away.";
			link.l1 = "I will look for it.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoFindStick");
		break;

		case "4":
			dialog.text = "...(takes the cane in both hands)... That is it. Now I believe I shall manage.";
			link.l1 = "Then to the ship. She is in the harbour.";
			link.l1.go = "5";
			TakeNItems(PChar, "RaulEsperanto_Stick", -1);
		break;

		case "5":
			dialog.text = "Where are we going, Captain?";
			link.l1 = "San Juan. Puerto Rico.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "...(stops)... Back to the Spaniards.";
			link.l1 = "...(smiles)... You are behind the news, Raul. San Juan has not been Spanish for a good while.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Then whose is it?";
			link.l1 = "I will tell you on the way. Now we walk.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoReturnToSanJuan");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
