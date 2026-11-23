// Gregg: Jorge de Maranha - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			if (PChar.RaulEsperanto == "3")
			{
				dialog.text = "You again. Well?";
				if (sti(PChar.money) >= BO_RE_BRIBE)
				{
					link.l1 = "I have the money.";
					link.l1.go = "4";
				}
				link.l2 = "Nothing.";
				link.l2.go = "exit";
			}
			else
			{
				dialog.text = "This way is closed. Be about your business.";
				link.l1 = "I am going.";
				link.l1.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Leave this ground at once. Nobody is permitted here.";
			link.l1 = "What happened here?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "It does not concern you. Go.";
			link.l1 = "Five thousand piastres.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(turns round)... How much?";
			// Gregg: if the coin is on him, pay here and now - sending the player away for what
			// is already in his purse is silly.
			if (sti(PChar.money) >= BO_RE_BRIBE)
			{
				link.l1 = "Five thousand. For a few answers. Here it is, count it.";
				link.l1.go = "4";
			}
			else
			{
				link.l1 = "Five thousand. For a few answers. I do not have it on me just now.";
				link.l1.go = "REPLY";
			}
		break;

		case "REPLY":
			dialog.text = "Then there is no conversation. Bring it and we will talk, I am not going anywhere.";
			link.l1 = "I will bring it.";
			link.l1.go = "exit";
			PChar.RaulEsperanto = "3";
			// Gregg: the quest opened this dialogue, so Khorkhe was left an actor. Until he is
			// put back to stay he will not speak on the next approach, he just stands there.
			LAi_SetStayType(NPChar);
			LAi_SetPlayerType(PChar);
		break;

		case "4":
			dialog.text = "Now that is more like it. Ask.";
			link.l1 = "Why can nobody enter the house?";
			link.l1.go = "5";
			AddMoneyToCharacter(PChar, -BO_RE_BRIBE);
			LAi_SetStayType(NPChar);
		break;

		case "5":
			dialog.text = "The governor general ordered it. Raul Esperanto was running some experiments in there, there was an explosion, people in the street were hurt.";
			link.l1 = "Where is Esperanto himself?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Arrested by order of the governor general.";
			link.l1 = "I asked where, not what became of him.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "For that, Captain, five thousand is not enough. Nor is ten. I am not being greedy, I am counting what I risk.";
			link.l1 = "Then name a condition instead of a price.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "A condition. In six days a pirate warship will lie off Tortuga. Board her, kill her captain, take the key off the body and open the chest in the cabin. What is in it you will bring me at Cape San Antonio.";
			link.l1 = "What is in it?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "You will see when you open it.";
			link.l1 = "Very well. Cape San Antonio.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Six days, Captain. Be late and she sails, and our conversation is over.";
			link.l1 = "I will not be late.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoToTortuga");
		break;

		case "12":
			dialog.text = "You have it?";
			link.l1 = "Here. How did you know the musket would be aboard her?";
			link.l1.go = "13";
		break;

		case "13":
			// Gregg: the questbook uses @<sPrison>, but placeholders do not work inside a
			// dialogue line - the colony name is assembled at runtime.
			string sPrisonKey = "Colony" + PChar.RaulEsperanto.Prison + "Gen";
			dialog.text = "I overheard a conversation I was not meant to hear. Now to business. Esperanto is held in the prison of " + XI_ConvertString(sPrisonKey) + ".";
			link.l1 = "Why there in particular?";
			link.l1.go = "14";
			TakeNItems(PChar, "RaulEsperanto_Musket", -1);
			TakeNItems(PChar, "RaulEsperanto_Key", -1);
		break;

		case "14":
			dialog.text = "Because the chief inquisitor is coming there from Europe, and he will try him. Esperanto has spent years at work with Egyptian mummy powder and keeps up an acquaintance with Ascold. You know Ascold?";
			link.l1 = "I have heard of him. Guadeloupe.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "That is the man. The Inquisition has had Ascold on its lists for years. The month they learned Esperanto was writing to him, they took him. He was near at hand.";
			link.l1 = "And he is to be tried there as well?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "As I said: the inquisitor is going there. Nobody carts a prisoner across half the archipelago. And make haste, he will be ashore any day.";
			link.l1 = "Thank you, Jorge.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoToNeedPrison");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
