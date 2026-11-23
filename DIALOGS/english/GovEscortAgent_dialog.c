// Deck talks with the allied messenger-agent (escort marine event, "agent" variant).
// Encounter is created in Treasure.c (GovEscortAgentOnMap); outcomes there (GovEscortAgent_Redirect and _OnExit wrappers).
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iBase;
	string sZ;

	switch (Dialog.CurrentNode)
	{
		case "exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

		case "First time":
			if (CheckAttribute(pchar, "GovEscortAgentResolved"))
			{
				dialog.text = "The matter is settled, captain. Fair winds.";
				link.l1 = "And to you.";
				link.l1.go = "exit";
				NextDiag.TempNode = "First time";
				break;
			}
			sZ = GovEscortAgent_GetCity(NPChar);
			if (sZ == "")
			{
				dialog.text = "The situation has changed, captain. The order to divert has been cancelled. Deliver the cargo to its original destination.";
				link.l1 = "I'll hold my original course.";
				link.l1.go = "exit";
				NextDiag.TempNode = "First time";
				AddDialogExitQuestFunction("GovEscortAgent_Depart_OnExit");
				break;
			}
			iBase = sti(pchar.CargoQuest.iMoney);
			dialog.text = "Easy, captain, no need for sabres - I am a friend. A trusted man of the crown of " + NationNameGenitive(sti(NPChar.nation)) + ". Grim news: pirates lie in wait along your route - someone let slip about the state cargo. Heed good counsel: take the convoy to " + XI_ConvertString("Colony" + sZ + "Acc") + " instead - the cargo will be received there just the same, straight to the governor, and for the trouble and risk the crown will pay fivefold. Or hold your course - your choice.";
			link.l1 = "Agreed. I'll take the cargo to " + XI_ConvertString("Colony" + sZ + "Acc") + ". (" + FindRussianMoneyString(iBase * 5) + ")";
			link.l1.go = "GovEscortAgent_Yes";
			link.l2 = "No. I hold my course.";
			link.l2.go = "GovEscortAgent_No";
			NextDiag.TempNode = "First time";
		break;

		case "GovEscortAgent_Yes":
			if (!GovEscortAgent_Redirect(NPChar))
			{
				dialog.text = "The order to divert is no longer valid, captain. Deliver the cargo to its original destination.";
				link.l1 = "I'll hold my original course.";
				link.l1.go = "exit";
				AddDialogExitQuestFunction("GovEscortAgent_Depart_OnExit");
				break;
			}
			AddDialogExitQuestFunction("GovEscortAgent_Depart_OnExit");
			dialog.text = "Wise, captain. Set course for " + XI_ConvertString("Colony" + pchar.CargoQuest.iTradeColony + "Acc") + ". You will be met and paid in full there - fivefold the guild's price. Fair winds at your back.";
			link.l1 = "Course laid in.";
			link.l1.go = "exit";
		break;

		case "GovEscortAgent_No":
			dialog.text = "As you will, captain. God grant you slip through. Farewell.";
			link.l1 = "We part ways.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("GovEscortAgent_RefuseResolve_OnExit");
		break;
	}
}
