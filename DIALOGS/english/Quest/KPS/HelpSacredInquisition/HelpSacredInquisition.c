// Gregg: диалоги квеста "Помощь святой инквизиции" (перенос из К:ПС).
// Стража за воротами Бас-Тера, Жаклин Дефо на Тортуге, Натан Рич на Бермудах.
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "Yes?";
			link.l1 = "No.";
			link.l1.go = "exit";
		break;

		case "BasTer_Guardians_1":
			dialog.text = "Halt! You are under arrest for the murder of this man.";
			link.l1 = "But I did not kill him!";
			link.l1.go = "BasTer_Guardians_2";
		break;

		case "BasTer_Guardians_2":
			AddDialogExitQuestFunction("HelpSacredInquisitionFightWithSoldAtExitBasTer");
			dialog.text = "Do not deny it. A townsman told us himself that you went out to kill him. Surrender your weapons and follow us.";
			link.l1 = "I think not.";
			link.l1.go = "exit";
		break;

		case "Jaklin_Defo_1":
			dialog.text = "What do you want, monsieur?";
			link.l1 = "Are you Jacqueline Defau? The wife of Francois Defau?";
			link.l1.go = "Jaklin_Defo_2";
		break;

		case "Jaklin_Defo_2":
			dialog.text = "I am. And who are you?";
			link.l1 = "An acquaintance of your husband.";
			link.l1.go = "Jaklin_Defo_3";
		break;

		case "Jaklin_Defo_3":
			dialog.text = "Why do you look like that?..";
			link.l1 = "My condolences. Your husband is dead.";
			link.l1.go = "Jaklin_Defo_4";
		break;

		case "Jaklin_Defo_4":
			dialog.text = "How?!";
			link.l1 = "He was murdered. And I am looking for whoever did it.";
			link.l1.go = "Jaklin_Defo_5";
		break;

		case "Jaklin_Defo_5":
			dialog.text = "I... I do not know. Come tomorrow, I need to gather myself.";
			link.l1 = "I understand, and I am sorry. But the trail goes cold by the hour, and tomorrow there may be none left.";
			link.l1.go = "Jaklin_Defo_6";
		break;

		case "Jaklin_Defo_6":
			dialog.text = "...He spoke of some stone. Before he sailed he said we would be rich when he came back. I felt it would come to no good.";
			link.l1 = "What exactly did he say about the stone? And did he ever name Nathan Rich?";
			link.l1.go = "Jaklin_Defo_7";
		break;

		case "Jaklin_Defo_7":
			dialog.text = "He did name him. He said Nathan was a friend. About the stone - nothing of the kind.";
			link.l1 = "Where can Nathan be found?";
			link.l1.go = "Jaklin_Defo_8";
		break;

		case "Jaklin_Defo_8":
			NextDiag.TempNode = "Jaklin_Defo_9";
			AddDialogExitQuestFunction("HelpSacredInquisitionToBermudes");
			dialog.text = "I do not know. Francois once let slip that they often met in the Bermudas. Promise me you will find the killer!";
			link.l1 = "I shall do everything I can.";
			link.l1.go = "exit";
		break;

		case "Jaklin_Defo_9":
			NextDiag.TempNode = "Jaklin_Defo_9";
			dialog.text = "Forgive me, I cannot speak just now.";
			link.l1 = "Yes, yes... Forgive me.";
			link.l1.go = "exit";
		break;

		case "Natan_Rich_1":
			SetCameraDialogMode(CharacterFromID("Natan Rich Guardian 1"));
			dialog.text = "What is going on here?";
			link.l1 = "Nothing - unless you count the noise you have made.";
			link.l1.go = "Natan_Rich_2";
		break;

		case "Natan_Rich_2":
			SetCameraDialogMode(CharacterFromID("Natan Rich Guardian 2"));
			dialog.text = "That was an accident, and Jake has already settled things with Francois.";
			link.l1 = "You leave the man you work for to clean up after you. Jake Smitt is no plaything. You are playing with fire, friend.";
			link.l1.go = "Natan_Rich_3";
		break;

		case "Natan_Rich_3":
			SetCameraDialogMode(CharacterFromID("Natan Rich Guardian 1"));
			dialog.text = "I understand. I hope this talk stays between us. And who is this Klein you spoke of?";
			link.l1 = "The man has powerful connections in the Old World, in Spain. Do not spoil anything - we need him badly. He will know you by your amulet; I trust you have it. He will wait at the tavern; if you cannot find him, ask the innkeeper, he will have left word... What is that noise? Do you hear it?";
			link.l1.go = "Natan_Rich_4";
		break;

		case "Natan_Rich_4":
			SetCameraDialogMode(NPChar);
			AddDialogExitQuestFunction("HelpSacredInquisitionInBermudesFirstFight");
			dialog.text = "We have visitors! Kill him!";
			link.l1 = "...";
			link.l1.go = "exit";
		break;

		case "Natan_Rich_5":
			dialog.text = "And who are you?! What are you doing here?!";
			link.l1 = "Captain " + GetFullName(pchar) + ". Sent by the Holy Inquisition. You have sinned considerably.";
			link.l1.go = "Natan_Rich_6";
		break;

		case "Natan_Rich_6":
			dialog.text = "Damn... I knew they would get to me. Well, do what you came to do.";
			link.l1 = "There is time. First tell me about this stone you are all hunting.";
			link.l1.go = "Natan_Rich_7";
		break;

		case "Natan_Rich_7":
			dialog.text = "Very well. Since you found me... It is the legend of the moonstone of ghostly light. It began with a madman who turned up in the Caribbean claiming he had seen it. He said the stone grants its holder the power of God.";
			link.l1 = "That much I know. Get to the point.";
			link.l1.go = "Natan_Rich_8";
		break;

		case "Natan_Rich_8":
			dialog.text = "They say the artefact belonged to the Atlanteans. They commanded a great power and could not hold it, and paid with their lives. We took up the search when word went round that some ancient order had brought the stone here long ago. Its followers guard it still. Where - we do not know.";
			link.l1 = "Curious. And who is Jake Smitt?";
			link.l1.go = "Natan_Rich_9";
		break;

		case "Natan_Rich_9":
			AddDialogExitQuestFunction("HelpSacredInquisitionInBermudesFightWithRich");
			dialog.text = "That I will not tell you. You have learned too much already - and you will die with it.";
			link.l1 = "Heh...";
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
