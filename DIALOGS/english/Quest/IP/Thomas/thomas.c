
void ProcessDialogEvent()
{
	ref NPChar, sld, chr;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "TohmasPetit_1":
			dialog.text = "Hello, "+GetSexPhrase("sir","madam")+". I see you are armed, and you look like a skilled "+GetSexPhrase("swordsman","swordswoman")+".";
			link.l1 = "Yes, you are not mistaken.";
			link.l1.go = "TohmasPetit_2";
		break;
		
		case "TohmasPetit_2":
			dialog.text = "I urgently need the help of someone like you!";
			link.l1 = "Tell me what happened.";
			link.l1.go = "TohmasPetit_3";
		break;
		
		case "TohmasPetit_3":
			dialog.text = "Local thugs attacked me and my fiancée just outside the gates and dragged her into the jungle! I'm a poor swordsman, and I can't handle them on my own. Please, help me. My father is a respected merchant in the city. If you help us rescue her, we will pay you very handsomely!";
			link.l1 = "All right, I'll help you. Lead the way, lad.";
			link.l1.go = "TohmasPetit_ChooseYes";
			link.l2 = "Solve your own problems. I have plenty of my own business to attend to.";
			link.l2.go = "TohmasPetit_ChooseNo";
		break;

		case "TohmasPetit_ChooseNo":
			NextDiag.CurrentNode = "TohmasPetit_5";
			pchar.questdata.TohmasPetit = 1;
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
			npchar.lifeday = 5;
			DialogExit();
		break;

		case "TohmasPetit_5":
			dialog.text = "Ah, it's you. I don't have time for this!";
			link.l1 = "...";
			link.l1.go = "Exit";
			NextDiag.TempNode = "TohmasPetit_5";
		break;

		case "TohmasPetit_ChooseYes":
			DialogExit();
			NextDiag.CurrentNode = "TohmasPetit_6";
			SetQuestHeader("TohmasPetit");
			AddQuestRecord("TohmasPetit", "QuestTaken");
			DoQuestCheckDelay("TohmasPetit_yes", 0.1);
		break;
		
		case "TohmasPetit_6":
			if (GetOfficersQuantity(pchar) > 0)
			{
				dialog.text = "Captain, before we go any further, I must ask you to have your officers wait in the city. If the bandits see a crowd, they might kill my fiancée and run, and I wouldn't survive that.";
				link.l1 = "Very well, don't worry about it.";
				link.l1.go = "TohmasPetit_ProcessGoFurther1";
				link.l2 = "And why didn't you mention this earlier, lad? This all looks very suspicious.";
				link.l2.go = "TohmasPetit_8";
			}
			else
			{
				dialog.text = "Excellent, Captain, we haven't a minute to lose. Those scoundrels dragged my fiancée further into the jungle. We must hurry before they harm my LeBlanc. Follow me!";
				link.l1 = "Fine, lead the way.";
				link.l1.go = "TohmasPetit_ProcessGoFurther";
			}
		break;

		case "TohmasPetit_8":
			dialog.text = "Captain! Nothing of the sort, I assure you. Please, Captain, let's hurry before those scumbags do any harm to my fiancée.";
			link.l1 = "All right, I believe you. I'll order my men to wait in the city.";
			link.l1.go = "TohmasPetit_ProcessGoFurther1";
			link.l2 = "No, I think you're hiding something, Thomas. So I've changed my mind, go rescue your fiancée yourself.";
			link.l2.go = "TohmasPetit_9";
			NextDiag.TempNode = "TohmasPetit_5";
		break;
		
		case "TohmasPetit_9":
			dialog.text = "That is very low of you! The whole city will know that you are a coward!";
			link.l1 = "Yeah, yeah. Get out of here.";
			link.l1.go = "TohmasPetit_ProcessCancel";
		break;
		
		case "TohmasPetit_10":
			dialog.text = "Captain, there they are, and my fiancée is with them! I will go up to them now, tell them I brought the gold, and try to distract them. I don't want them harming my fiancée. Wait here, I'll try to keep them busy, and then you follow me.";
			link.l1 = "Good plan, go ahead.";
			link.l1.go = "TohmasPetit_ubejal";
		break;
		
		case "TohmasPetit_ubejal":
			AddDialogExitQuest("TohmasPetit_Rastanovka");
			DialogExit();
		break;
		
		case "TohmasPetit_ProcessCancel":
			AddDialogExitQuest("TohmasPetit_konec");
			AddQuestRecord("TohmasPetit", "QuestRefused");
			CloseQuestHeader("TohmasPetit");
			DialogExit();
			pchar.questdata.TohmasPetit = 1;
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
		break;

		case "TohmasPetit_ProcessGoFurther1":
			DeleteAllOfficersFromLocation();
			AddQuestRecord("TohmasPetit", "SuspiciousRequest");
			AddDialogExitQuest("TohmasPetit_to_jungle");
			NextDiag.CurrentNode = "TohmasPetit_10";
			DialogExit();
		break;
		
		case "TohmasPetit_ProcessGoFurther":
			AddQuestRecord("TohmasPetit", "SuspiciousRequest");
			AddDialogExitQuest("TohmasPetit_to_jungle");
			NextDiag.CurrentNode = "TohmasPetit_10";
			DialogExit();
		break;

		case "exit":
			if (CheckAttribute(NextDiag,"TempNode")) NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}	
}
