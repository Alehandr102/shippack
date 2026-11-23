// Gregg: Jan Beausing - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			dialog.text = "You handle a blade well, Captain. Keep at it.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "The Inquisition has sent its people again. Third time this year. They will not let it rest.";
			link.l1 = "The Inquisition? What did you do to them?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Nothing. I have a thing they want, that is the whole of it. At first I fought them off myself, then I tired of it and hired the Secretary.";
			link.l1 = "The Secretary is dead. I found him in a cave.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Dead. ...(a pause)... So somebody was found for him too. Well. Thank you for clearing these away. Now say what you came for. And do not lie, I hear it.";
			link.l1 = "I came for you, Beausing. I work for the Inquisition and I am to deliver you to Santiago.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "...(sets down his glass)... Direct. Well then. One question before I kill you: did you truly think you would walk in and lead me out?";
			link.l1 = "Let us find out.";
			link.l1.go = "exit";
			PChar.RaulEsperanto.BeausingBattle = 1;
			AddDialogExitQuestFunction("RaulEsperantoBattleWithBeausing");
		break;

		case "5":
			dialog.text = "Argh.";
			link.l1 = "Enough, Jan. It only gets worse from here.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "No.";
			link.l1 = "As you like.";
			link.l1.go = "exit";
			PChar.RaulEsperanto.BeausingBattle = 2;
			AddDialogExitQuestFunction("RaulEsperantoBattleWithBeausing");
		break;

		case "7":
			dialog.text = "You think you have won? You have won nothing.";
			link.l1 = "Again?";
			link.l1.go = "exit";
			PChar.RaulEsperanto.BeausingBattle = 3;
			AddDialogExitQuestFunction("RaulEsperantoBattleWithBeausing");
		break;

		case "8":
			dialog.text = "You will not take me alive. On guard.";
			link.l1 = "Will you ever stop?";
			link.l1.go = "exit";
			PChar.RaulEsperanto.BeausingBattle = 4;
			AddDialogExitQuestFunction("RaulEsperantoBattleWithBeausing");
		break;

		case "9":
			dialog.text = "...(sitting on the floor, breathing hard)... Enough. You have won. Do you hear me? Won.";
			link.l1 = "Now we walk out quietly and go aboard.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Anywhere but Santiago.";
			link.l1 = "You have no choice.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "I have. Hear me for one minute. It is not me they want. It is a Gospel that was in my keeping. I will give it to you, only do not carry me there.";
			link.l1 = "My orders are to deliver you, not a book.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Your orders say one thing, their want is another. Hand me over and they will begin drawing it out of me, where the book is. Slowly. And I will tell you at once and for nothing.";
			link.l1 = "Very well. We fetch the book first, then Santiago. If they say they have no use for you, I return you to San Martin.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Agreed. The book is with the usurer in Willemstad.";
			link.l1 = "Then we sail.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoToVillemstadBank");
		break;

		case "14":
			dialog.text = "Here it is. Fetched from the usurer just now. ...(holds it carefully)... Heavier than I remembered.";
			link.l1 = "Now Cuba. You stay aboard, I hand over the book.";
			link.l1.go = "15";
			TakeNItems(PChar, "RaulEsperanto_Evangelie", 1);
		break;

		case "15":
			dialog.text = "I shall stay. You know, Captain, I am almost sorry I will not see his face.";
			link.l1 = "We shall see.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoGetEvangelie");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
