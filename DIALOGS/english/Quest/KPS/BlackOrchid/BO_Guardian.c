// Gregg: The fort guard and the ghost of Joe Newman - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(bars the way to the stairs)... Captain, nobody goes below tonight. This is his night, and on such nights we do not go down.";
			link.l1 = "That is exactly what I came for. Stand aside.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoNewmanToPrison");
		break;

		case "2":
			dialog.text = "...(very quietly)... In many years you are the first to best me. Name yourself.";
			link.l1 = "Captain " + GetHeroLastName() + ".";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "A good name. Why do you stand there? Finish it. The fight was fair.";
			link.l1 = "That is not what I came for. I need your help.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Help. From me. From a man who has not passed this wall in fifty years.";
			link.l1 = "Raul Esperanto. He was brought here some thirty years ago.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "...(a long silence)... Raul. I remember. He was here some seven months and I saw him twice. I judged then that he would not last the winter, by the look of him.";
			link.l1 = "Did he die?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "He escaped. From this prison, out of which nobody has escaped in fifty years.";
			link.l1 = "Then he may be alive.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "He is alive. That I know. Whether I tell you where to look depends on why you want him.";
			link.l1 = "How would you know where he is? You never leave.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Prisoners leave. And come back. And talk. So why do you want him? Truthfully. I shall hear the difference.";
			link.l1 = "Years ago he made a thing that kills. I mean to stop it, and I cannot without him.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Truthfully. Very well. Then listen.";
			link.l1 = "Wait. Before I listen - how many guards have you killed?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "...(without offence)... You want to know whether I am a monster.";
			link.l1 = "I do.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "The first time I appeared here they came at me with sabres and muskets. I tried to explain that I touch nobody. They did not listen. Since then every man who comes down on my day comes down to finish me. Once a year the commandant hires a new one.";
			link.l1 = "And you took me for another.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "What else was I to see in you.";
			link.l1 = "And the prisoners? The commandant says you frighten them.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "The commandant says a great deal. The prisoners are the only ones who speak to me. It is from them I heard of Esperanto. He is in Cuba.";
			link.l1 = "Cuba. You are sure?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "So one of them said, some ten years back. That is all the help I have.";
			link.l1 = "It is enough. Thank you.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Since we are at it, ask something of me in turn. It is fairer that way.";
			link.l1 = "Then I will. What do you want?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Explain to the commandant what I am. Tell him what I told you.";
			link.l1 = "I will.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "...(after a pause)... No. Do not.";
			link.l1 = "Why not?";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Because he will not believe it, and he will remember you. My name is past mending. Yours need not be spoiled for it.";
			link.l1 = "I will try all the same.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Do not try. That will be the help. Go, Captain.";
			link.l1 = "Fare well, Joe.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoNewmanExit");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
