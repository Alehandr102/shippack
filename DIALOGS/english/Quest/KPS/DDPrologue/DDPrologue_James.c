// Gregg: James Robinson - alternative prologue "Desert Island" (ported from Damned Destiny).
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
			NextDiag.TempNode = "First time";
			if (CheckAttribute(PChar, "StartDamnedDestiny"))
			{
				if (PChar.StartDamnedDestiny == "1")
				{
					dialog.text = "Captain, we should hurry to the grotto!";
					link.l1 = "You are right, let us go.";
					link.l1.go = "exit";
					break;
				}
			}
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "Start_1":
			dialog.text = "Captain " + GetHeroLastName() + ", you are alive! Damn it, how glad I am to see you! Are you wounded?";
			link.l1 = "No, I do not think so. Only my head hurts badly.";
			link.l1.go = "Start_2";
		break;

		case "Start_2":
			dialog.text = "No wonder. You struck your head against a cannon during that terrible blast. You were very lucky to survive at all.";
			link.l1 = "What happened, anyway? I remember nothing at all...";
			link.l1.go = "Start_3";
		break;

		case "Start_3":
			dialog.text = "Nothing at all?";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartDamnedDestinyStartFade");
		break;

		case "Start_4":
			dialog.text = "Not even my name?";
			link.l1 = "No, I cannot recall a thing.";
			link.l1.go = "Start_5";
		break;

		case "Start_5":
			dialog.text = "You must have hit that cannon hard. All right, captain, listen. I am James Robinson, I entered your service about three months ago. I was proud of the honour of sailing on the ship of captain " + GetMainCharacterNameGen() + ". \n About a month ago, in a tavern on Bermuda, you met your old friend captain Grifo Sajfus, who offered you a job. An attack on a small vessel that supposedly carried a parcel of diamonds from Africa. You agreed to meet here, on the uninhabited island of Dominica, to settle the plan and act, but \n Grifo betrayed you. And all of us with you.";
			link.l1 = "Betrayed? How did it happen?";
			link.l1.go = "Start_6";
		break;

		case "Start_6":
			dialog.text = "At night, when the ships closed in, he emptied every gun of his port side into our swift xebec, then came about and swept our deck with grapeshot from the starboard. Half our crew died in the first salvo alone, and you struck your head against a cannon and lost consciousness. \n We meant to fight back, but after the second salvo, which took another fifteen or twenty lives, we understood it was useless and jumped into the sea. I saw our boatswain Andy Rejdol heave you onto his shoulder and jump with you. I managed to reach the shore, and there I found you. Damn it, cap, how glad I am that you live!";
			link.l1 = "I am glad too, " + NPChar.name + ". How many of our crew do you think survived?";
			link.l1.go = "Start_7";
		break;

		case "Start_7":
			dialog.text = "I do not know, cap. I doubt many of those who jumped into the sea were saved. Grifo Sajfus's ship kept raking the water with grapeshot for a long while, hoping to finish us all. So you see, cap, if there are survivors at all, four men at the most, well, five, no more.";
			link.l1 = "Listen, " + NPChar.name + ", I have been thinking that we stand on this beach for nothing. If Sajfus's ship is still here, they will spot us and open fire. Or worse, land a party to hunt the survivors. We must move into the jungle.";
			link.l1.go = "Start_8";
		break;

		case "Start_8":
			dialog.text = "I know there is a small grotto somewhere nearby, we can take cover there for a time and think what to do next. And who knows, maybe those who survived found their refuge there.";
			link.l1 = "Then let us not linger, forward!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameGoToGrot");
		break;

		case "Start_9":
			dialog.text = "Captain " + GetHeroLastName() + ", excellent news! A small lugger has dropped anchor off the neighbouring beach! Her captain and several of his mates have already come ashore, and if you hurry we may still catch them there.";
			link.l1 = "Splendid, " + NPChar.name + ". We move out!";
			link.l1.go = "Start_10";
		break;

		case "Start_10":
			dialog.text = "Wait, captain " + GetHeroLastName() + "! Be careful in your dealings with the lugger's cap, he looked suspicious to me.";
			link.l1 = "Then you, James, and you, Andy, cover me if anything goes wrong. Is that clear? Then forward!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameNextDayToLugger");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
