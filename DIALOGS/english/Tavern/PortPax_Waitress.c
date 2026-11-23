// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Questions, " + GetAddress_Form(NPChar) + "?", "Whatever you want, " + GetAddress_Form(NPChar) + "?"), ""+ GetSexPhrase("Hmm, aren't you bothering me, " + GetAddress_Form(NPChar) + "? ", "Again with strange questions? Girl, would you drink rum, or something...") +"", "This is the third time you've been talking about some issue this day..."+ GetSexPhrase("Are these tokens of attention?", "") +"",
                          "Are you going to ask me again, " + GetAddress_Form(NPChar) + "?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("I've changed my mind...", "I have nothing to talk about right now."), "No, no, you're beautiful...",
                      "No way, dear, sclerosis...", "No, what questions...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// Gregg: the "Black Orchid" (ported from Damned Destiny) - the waitress has
			// the key to Brian George's chest. The attribute may not exist at all, hence
			// a separate check: Storm has no short-circuit, conditions cannot be merged.
			if (CheckAttribute(PChar, "MysteriousArtefact"))
			{
				if (PChar.MysteriousArtefact == "6")
				{
					link.l2 = "Do you remember the brawl in your tavern two weeks ago?";
					link.l2.go = "MysteriousArtefact_1";
				}
			}
		break;

		//==> Gregg: the Black Orchid - a mysterious artefact
		case "MysteriousArtefact_1":
			dialog.text = "I would much rather not remember it, captain. It was dreadful.";
			link.l1 = "I understand, I shall come straight to the point. A man named Brian George lost a key in that brawl. Did you find it?";
			link.l1.go = "MysteriousArtefact_2";
		break;

		case "MysteriousArtefact_2":
			dialog.text = "I did. I was hiding in the corner, and suddenly a key fell right beside me. I picked it up, and when the fight was over I asked every customer whether it was his. Nobody had lost a key, so it stayed with me.";
			link.l1 = "And Brian George? He was there too - why did you not ask him?";
			link.l1.go = "MysteriousArtefact_3";
		break;

		case "MysteriousArtefact_3":
			dialog.text = "The keeper threw him out at once. And I could never have supposed such a pretty little key belonged to that filthy drunkard.";
			link.l1 = "Do you still have the key?";
			link.l1.go = "MysteriousArtefact_4";
		break;

		case "MysteriousArtefact_4":
			dialog.text = "I have. But I shall give it only to its rightful owner - Brian George.";
			link.l1 = "Listen, Brian George is dead; he was killed a few days ago on the Bermudas. In the chest that key opens there is something of mine that he was to give back. It is dear to me as a keepsake. I beg you, give me the key. My fate is in your hands.";
			link.l1.go = "MysteriousArtefact_5";

			if (sti(PChar.money) >= 5000)
			{
				link.l2 = "Perhaps I might buy that little key? Five thousand piastres, say. What do you think?";
				link.l2.go = "MysteriousArtefact_6";
			}

			link.l3 = "Listen here, girl. Do not give me that key and you will be in serious trouble, that I promise you.";
			link.l3.go = "MysteriousArtefact_7";
		break;

		case "MysteriousArtefact_5":
			PChar.MysteriousArtefact.WaitressTalk = 1;
			dialog.text = "Very well, take it. It is yours.";
			link.l1 = "Much obliged.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactFirstKeyGive");
		break;

		case "MysteriousArtefact_6":
			PChar.MysteriousArtefact.WaitressTalk = 2;
			AddMoneyToCharacter(PChar, -5000);
			dialog.text = "The key is yours.";
			link.l1 = "Much obliged.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactFirstKeyGive");
		break;

		case "MysteriousArtefact_7":
			dialog.text = "Threaten me and I shall call the guard.";
			link.l1 = "Call them. You see, I am on good terms with the governor of your town, and a few days ago I saved his life. He will not let his rescuer be thrown into gaol. But you I shall have locked in the foulest cell of your prison for the rest of your days. The pretty key they find on you at the search will be given to me - nobody will believe it is yours. The choice is yours.";
			link.l1.go = "MysteriousArtefact_8";
		break;

		case "MysteriousArtefact_8":
			PChar.MysteriousArtefact.WaitressTalk = 3;
			dialog.text = "...(turns pale)... Take it. It is yours.";
			link.l1 = "Much obliged. And remember: baring your teeth can bring you no end of trouble. Good day to you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactFirstKeyGive");
		break;
		//<== Gregg: the Black Orchid

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
