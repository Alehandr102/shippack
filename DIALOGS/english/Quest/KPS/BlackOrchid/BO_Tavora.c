// Gregg: Jeronimo Tavora, inquisitor of Santiago - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			// Gregg: three exchanges: finding Raul, the Gospel and the cane. Split by state.
			// Gregg: a third exchange - for Esperanto cane
			// Gregg: не по состоянию "11" - оно ставится, когда Тавора согласился, то есть
			// ещё до спуска в подвал. Метку ставит сам Рауль, когда просит трость.
			if (CheckAttribute(PChar, "RaulEsperanto.Stick"))
			{
				dialog.text = "Is he still here?";
				link.l1 = "He cannot walk. His cane was taken from him at the arrest.";
				link.l1.go = "30";
			}
			else
			{
			if (PChar.RaulEsperanto == "10")
			{
				dialog.text = "You are back sooner than I reckoned.";
				link.l1 = "I have brought what you asked for.";
				link.l1.go = "1";
			}
			else
			{
			// Gregg: strictly by state. The else here was a hole: after the talk the state moves
			// to "7" and the branch played again, hanging the same journal record every time.
			if (PChar.RaulEsperanto == "6")
			{
				dialog.text = "Peace be with you. Have you business here, or are you lost?";
				link.l1 = "Business. There is an old man in your prison, Raul Esperanto.";
				link.l1.go = "20";
			}
			else
			{
				dialog.text = "Peace be with you. I have my duties, and you, I take it, have yours.";
				link.l1 = "I will not keep you.";
				link.l1.go = "exit";
			}
			}
			}
			NextDiag.TempNode = "First time";
		break;

		case "20":
			dialog.text = "There is. The Spaniard. Experiments, mummy powder, letters to a man on Guadeloupe whom we know. What do you want with him?";
			link.l1 = "I want him alive and outside.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "...(unsurprised)... Everyone wants something. So do I. Only I cannot let him go: he is not entered against me.";
			link.l1 = "Then who gives the orders here, if not you?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "In Santiago, I do. But Esperanto is written into the papers of the tribunal coming from Europe, and the chief inquisitor will be ashore any day. Release the old man myself and it is to him I answer.";
			link.l1 = "And if I gave you a reason?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "A reason is exactly what I need. On San Martin lives a banker named Jan Beausing. For twelve years he has kept a thing that is not his.";
			link.l1 = "What thing?";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "A book. You need know no more, and I shall sleep the better. Bring me Beausing, or better the book, and the old man walks out the same day.";
			link.l1 = "San Martin. Marigot. Understood.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoFindBeausing");
		break;

		case "1":
			dialog.text = "I am listening.";
			link.l1 = "Jan Beausing. I know where he is and what he was keeping.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(lays down his pen slowly)... Go on.";
			link.l1 = "He is aboard my ship. And this is what you have been hunting him for.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(takes the book, opens it at the middle, reads a few lines, closes it)... It is the one. Twelve years.";
			link.l1 = "Then you have no further use for Beausing?";
			link.l1.go = "4";
			TakeNItems(PChar, "RaulEsperanto_Evangelie", -1);
		break;

		case "4":
			dialog.text = "Beausing was wanted exactly as far as the book lay with him. The book is here. So he owes nobody anything now.";
			link.l1 = "Then I will take him home.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Take him. And name your price, Captain. I dislike being owed: a debt binds harder than a contract.";
			link.l1 = "There is an old man in your prison. Raul Esperanto.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "...(looks at you for a long moment)... The Spaniard. Experiments, mummy powder, letters to a man on Guadeloupe whom we know.";
			link.l1 = "I want him alive and outside.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "I needed him so as to put something at all on the table before the tribunal. Now I have something to meet them with, and it is worth more than the old man.";
			link.l1 = "Is that a yes?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "It is a yes. Take him today, before I reconsider and before the papers leave for Madrid. And, Captain...";
			link.l1 = "Yes?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Do not ask him what he made for Andersen. Thirty years ago I put that question to a man who knew the answer, and I have been looking for the book ever since.";
			link.l1 = "...(says nothing)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoTalkWithEsperanto");
		break;

		case "30":
			dialog.text = "A cane? ...(pulls open a drawer)... Yes, I have it. Everything is taken at the arrest, and as a rule there is nobody left to return it to.";
			link.l1 = "Today there is.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "...(lays the cane on the desk)... Take it. And tell him I did not break it. That will matter to him more than his freedom, you will see.";
			link.l1 = "I will tell him.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoGiveStick");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
