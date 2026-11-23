// Gregg: the stranger from Port-au-Prince, who is Matilda - the "Black Orchid" trilogy
// (ported from Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// Nodes 1-7 - the set-up in town, 8-9 - the same woman dying in the Santiago storeroom.
// The first part of the Meritor is handed over here.

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

			if (PChar.MysteriousArtefact == "8")
			{
				dialog.text = "Captain, have you punished that scoundrel?";
				link.l1 = "I am working on it. I promise you I shall.";
				link.l1.go = "exit";
			}
		break;

		case "1":
			dialog.text = "Captain, I beg you, help me! Will you help me? Promise you will!";
			link.l1 = "Very well, I shall help. What has happened?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(weeping)... It is hard for me to speak of it, captain. Believe me, very hard.";
			link.l1 = "Calm yourself and tell me in order.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(bursts into tears)... Forgive me. When I remember it I cannot hold back the tears.";
			link.l1 = "It is quite all right, I understand. Perhaps you will tell me tomorrow? It will come easier tomorrow, believe me.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "...(wipes her face with a handkerchief)... No. I shall not rest until that villain is sent to the next world. I must tell it now.";
			link.l1 = "I am all attention.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "My husband Louis Bourdeaux and I lived here in Port-au-Prince. Respectable townsfolk, we broke no laws. Today we went out of town to rest - there is a lovely spot there, Louis and I were fond of it. But today... today... ...(weeps again)...";
			link.l1 = "Perhaps tomorrow after all?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "No, I shall go on. Some drunken captain came up to us and began to insult me. Louis, my brave Louis, drew his blade to defend my honour - and the man pulled a pistol from his belt and shot him through the forehead. And then... then he used me. In the foulest way ...(bursts into tears)...";
			link.l1 = "Is this captain still beyond the town gates?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "He is. Avenge my husband and me, captain, I implore you. Kill the villain! We were people of means, I shall pay you generously. I shall wait for you in the church.";
			link.l1 = "Very well. I must hurry.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactPortPaxWomanHelp");
		break;

		case "8":
			TakeNItems(PChar, "MysteriousArtefact_MeritorChapter_1", 1);
			dialog.text = "...(faintly)... Captain... take this thing... take it... it will help you find the Orchid...";
			link.l1 = "But how? What am I to do with it?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "...(her voice growing weaker still)... Take it... when the time comes... you will understand what must... be done...";
			link.l1 = "Matilda! Matilda!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactMatildaIsDead");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
