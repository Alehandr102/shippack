// Gregg: the rivals' agent - the land variant of the GWIC "strike at the competitors" assignment.
// He never puts to sea; he sits in a colony tavern. The killing rides entirely on the stock duel framework:
// our own nodes lead up to the insult, then link.go = "land_duel" - and Common_Duel.c/duel.c handle both
// "outside the town gates" (a clean one-on-one duel) and "right here and now" (tavern clone, with bodyguards).
#include "DIALOGS\english\Common_Duel.c"

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	bool ok;

	ProcessDuelDialog(NPChar, link, NextDiag); // the duel nodes (outraged/land_duel/fight_*/talk_off_town/change_mind) are served by the framework

	switch(Dialog.CurrentNode)
	{
		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "First time":
			NextDiag.TempNode = "First time";
			ok = false;
			if (CheckAttribute(pchar, "GWIK.Job") && CheckAttribute(pchar, "GWIK.Job.RivalLand")) ok = true;
			if (ok && pchar.GWIK.Job != "rival") ok = false;
			if (CheckAttribute(pchar, "GWIK.Job.Done")) ok = false; // already dead - unreachable, but let it stand
			if (!ok)
			{
				dialog.text = "Forgive me, I have business to attend to. Good day to you.";
				link.l1 = "I won't keep you.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Decent rum for such a hole, wouldn't you say? Sit down if you have something to tell me. If not - stop blocking my light.";
			link.l1 = "My mistake. Enjoy your evening.";
			link.l1.go = "exit";
			link.l2 = "You are " + GetFullName(NPChar) + ", are you not? The man who does business here behind the West India Company's back.";
			link.l2.go = "GWIK_RivalTalk";
		break;

		case "GWIK_RivalTalk":
			dialog.text = "Ah, so that's it. The Dutchmen have sent someone to have a word with me. Behind their back, is it? That's rich. They are the ones meddling in my trade, not the other way round - and between us, they make a poor job of it.";
			link.l1 = "The Company offered you terms. You spat on the hand that was held out to you.";
			link.l1.go = "GWIK_RivalTalk2";
			link.l2 = "I've heard enough. We'll meet again.";
			link.l2.go = "exit";
		break;

		case "GWIK_RivalTalk2":
			dialog.text = "I spat on the PAW that was held out to me, my friend. There is a difference. And tell your masters this: while I draw breath, not one of their cargoes will move through these waters at their price.";
			link.l1 = "While you draw breath. That condition is easily changed.";
			link.l1.go = "GWIK_RivalProvoke";
			link.l2 = "I'll tell them. Word for word.";
			link.l2.go = "exit";
		break;

		case "GWIK_RivalProvoke":
			// Gregg: bodyguards are armed UP FRONT - the framework only seats them for a fight inside the tavern
			// (Duel_Prepare_Fight does that solely when type=="tavern"), so a duel outside the gates stays one-on-one.
			// Duel_Kill_Enemy clears the attribute itself.
			pchar.questTemp.duel.enemyQty = 1 + rand(1);
			dialog.text = "What?.. How dare you, you Dutch lapdog! No one here will shield you - not your Company, not your god!";
			link.l1 = "Then let us step outside the town gates. No one will trouble us there.";
			link.l1.go = "GWIK_RivalDuelOut";
			link.l2 = "To hell with walking. Here and now.";
			link.l2.go = "GWIK_RivalDuelHere"; // our own node: same as the stock fight_right_now, but the escort is stripped first
			link.l3 = "Live a while longer, then. We'll come back to this talk.";
			link.l3.go = "exit";
		break;

		case "GWIK_RivalDuelOut":
			// Gregg: the "outside the gates" path is NOT handed to the framework (land_duel/fight_off_town_prep/talk_off_town):
			// that chain waits N hours, requires you to walk to the gates, offers a change of mind, cowardice and the opponent's
			// return - far too many states, they kept breaking and let the assignment hang. Here there is no way back:
			// the next line teleports you out of town and starts the fight.
			dialog.text = "Come along, then. I hope you swing a blade better than you wag your tongue.";
			link.l1 = "After you.";
			link.l1.go = "GWIK_RivalDuelGo";
		break;

		case "GWIK_RivalDuelGo":
			AddDialogExitQuestFunction("GWIK_RivalDuelOutTown"); // teleport out of town + fight on arrival (pattern: fight_right_now -> Duel_Prepare_Fight)
			NextDiag.CurrentNode = "First time";
			DialogExit();
		break;

		case "GWIK_RivalDuelHere":
			// Gregg: mirrors the stock fight_right_now (tavern clone via Duel_Prepare_Fight) but strips the escort first:
			// a duel is one-on-one, yet officers and boarders followed the player into the clone as well.
			// The TARGET's bodyguards (questTemp.duel.enemyQty) stay.
			GWIK_RivalDuelOfficersOff();
			pchar.questTemp.duel.enemy = NPChar.id;
			AddDialogExitQuestFunction("Duel_Prepare_Fight");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "after_peace":
			// Gregg: we OVERRIDE the framework node. The stock after_peace offers only "I'm leaving" -> peace -> after_peace again,
			// so after a change of mind the target would be stuck in a peaceful node and could never be duelled.
			dialog.text = "Still here? Be off with you, while I'm in a good mood.";
			link.l1 = "I'm going.";
			link.l1.go = "exit";
			link.l2 = "I've changed my mind. Draw your blade.";
			link.l2.go = "GWIK_RivalProvoke";
		break;
	}
}
