// Gregg: Tommi Fatman and his crew - alternative prologue "Desert Island"
// (ported from Damned Destiny). One file for three: Tommi (nodes 1-6),
// Walter Korten (7-13) and Johny Norton (14).
// Logic - Program\Quests\reaction_functions\ddprologue.c

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
			dialog.text = "All questions to the captain.";
			link.l1 = "Very well.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Greetings! I am captain Tommi Fatman. What do you want?";
			link.l1 = "Captain " + GetFullName(PChar) + ". Listen, Tommi, I have business with you, and you can help me with it.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "And what business is that?";
			link.l1 = "We want to reach the nearest English or French port.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "I am willing to deliver you to Martinique. Will that do?";
			link.l1 = "Excellent, that suits us.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "How much? Name your sum.";
			link.l1 = "Fifteen hundred in advance and another fifteen hundred on arrival at Martinique.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Done! Hand over the advance.";
			link.l1 = "Here, fifteen hundred, as agreed. When may we come aboard?";
			link.l1.go = "6";
		break;

		// Gregg: the money is taken here, and that is correct - the player already handed
		// it over with the reply in node 5
		case "6":
			AddMoneyToCharacter(PChar, -1500);
			dialog.text = "...(laughs)... Here is the thing, captain Loser, you will not come aboard any time soon, if ever, rather than croak on this island. Ha! Did you really think I would carry you to Martinique, you naive fool? Clear off while your life is dear to you, I am still in a good mood.";
			link.l1 = "Thought you would take me for a ride, Fatman? Oh no, I shall let nobody make a fool of me again. Defend yourself!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameLuggerCapBattle");
		break;

		case "7":
			dialog.text = "You are our captain now, mister " + GetHeroLastName() + ".";
			link.l1 = "I regret having to kill your captain, but he deceived me, and he paid for it with his life.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "We saw what happened, captain " + GetHeroLastName() + ", and we are entirely on your side. To be honest, we are grateful to you for killing that useless sack. We had long wanted to mutiny and elect a new captain.";
			link.l1 = "...(smiles)... So I turned up just in time.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Somewhat, captain. Though our ship is in a sorry state. That idiot went up against a galleon.";
			link.l1 = "A lugger against a galleon? Was he right in the head?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "That is just what I say. The galleon's very first broadside sobered his mind, we came about and fled the field. We were lucky to shake off the pursuit.";
			link.l1 = "And why did you drop anchor off this island? To take on supplies?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Here we meant to finish that bastard off. Had you come five minutes later, we would have done for Fatman ourselves.";
			link.l1 = "I see. Well, there is no time for talk, it is time to weigh anchor and get out of here.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Captain, we have a small cargo of cocoa in the hold, which we are carrying to Barbados for a man named Richard Tillman. I take it you will assume the old captain's obligation to deliver the cargo?";
			link.l1 = "Certainly. Course for Barbados.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameHappyEnd");
		break;

		case "14":
			dialog.text = "At your service, captain?";
			link.l1 = "At ease.";
			link.l1.go = "exit";
			NextDiag.TempNode = "14";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
