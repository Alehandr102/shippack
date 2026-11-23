// Gregg: Pierre Arnu and Rene Matezh, Sajfus's men in the grotto - alternative prologue
// "Desert Island" (ported from Damned Destiny). The scene is overheard: the player sits in
// cover, and both sides of the exchange are the pirates talking to each other.
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
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Pierre Arnu: And why did we climb in here? There is nobody here, and there cannot be. Under such a hail of grapeshot nobody has a chance to survive.";
			link.l1 = "Rene Matezh: The captain ordered us to comb every inch of the island. Stop whining and start searching the grotto.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Pierre Arnu: Oh no, I am not going any further in there, it is even gloomier than here, and I do not care about the captain's orders.";
			link.l1 = "Rene Matezh: Is that so. You would dare disobey a captain's order?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Pierre Arnu: And why do you not go there yourself, since you are so clever? Go on, crawl in, and I shall watch what they do to you.";
			link.l1 = "Rene Matezh: ...(smirks)... And who exactly would be doing anything to me in there?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Pierre Arnu: So you have not heard?";
			link.l1 = "Rene Matezh: Heard about what?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Pierre Arnu: About the vicious foul creatures that dwell in these grottoes and kill anyone who enters their territory.";
			link.l1 = "Rene Matezh: ...(smirks)... Nursery tales to frighten little children.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Pierre Arnu: ...(angrily)... Nursery tales? Well, if they are nursery tales, then you have nothing to fear. Go on, crawl in, and I shall follow.";
			link.l1 = "Rene Matezh: Ahem... Erm... Actually you are right, there is probably nobody here, no need to search at all.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Pierre Arnu: What, lost your nerve? You are afraid, you rogue.";
			link.l1 = "Rene Matezh: I am not afraid! I simply... I simply recalled our grapeshot, and... Under such a hail of fire nobody would truly have survived. That is that, let us go.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Pierre Arnu: You lost your nerve, Rene, and do not deny it, I can see it. Fine, let us get out of here, this place gives me the creeps.";
			link.l1 = "Rene Matezh: Wait. We tell the captain we combed this grotto from end to end, understood?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Pierre Arnu: No, better you tell him alone, he trusts you more.";
			link.l1 = "Rene Matezh: So be it, I shall tell him. That is that, let us get out of here.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameEnemyInGrotExit");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
