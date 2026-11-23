// Gregg: The messenger and Paul Strongfield - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "...";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(from the doorway, his voice breaking)... A ship! A ship on the horizon!";
			link.l1 = "Get your breath first.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I was in the shop. It went dark in the middle of the day, people started shouting in the street. I ran out, and there was a storm at sea that had not been there a minute before, and something standing in toward the colony.";
			link.l1 = "Standing in as what?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "I did not make it out myself. They were shouting a black man o war, huge, and not one light aboard. I was told to warn you.";
			link.l1 = "Understood. Thank you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyTalkWithKendart");
		break;

		case "4":
			dialog.text = "Captain. And Master Kendart also. I thank you both from my heart.";
			link.l1 = "Who are you?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Thirty years ago I sailed a caravel. I had a wife, children, a living, and I was content with all of it. On one voyage a storm carried us two days, and on the third we came up on a vessel that had no business being there. Empty, adrift. I went aboard first.";
			link.l1 = "And you found the glass.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "I found it. The sand ran and never ran out - the upper bulb never emptied, the lower never filled. I looked at it without lifting my eyes, then I took it in my hands. After that, pain, and I woke in the same cabin, only nobody could see me any more.";
			link.l1 = "...(says nothing)...";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "While nobody touched the glass I was simply nothing. But the moment another hand lay on the bulb, the thing in me woke. And I killed. Everyone aboard, to the last man.";
			link.l1 = "Thirty two on the yards. That was you.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "It was. And the ones shot and piled in the hold, that was me too. They could not see me, so they did not resist.";
			link.l1 = "And while the glass lay with Kendart in San Juan?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "I prayed nobody would touch it. Had anyone picked it up I would have cut down the whole town and been able to do nothing about it. You two, thank God, knew what you were handling.";
			link.l1 = "And this ship?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "I do not know. She appeared along with me when the glass was opened, and I am as much a guest aboard her as you are.";
			link.l1 = "Then we leave her together.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "No. There are too many men behind me for me to go on living. I am asking you to kill me.";
			link.l1 = "It was not your doing.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "They were my hands. And I remember it myself. I made my choice long ago, Captain. Until today I could not even manage that.";
			link.l1 = "You are certain?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Entirely.";
			if (FindAndDestroyManowarCanShoot()) link.l1 = "...(raises the pistol)...";
			else link.l1 = "...(nods)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyManowarShot");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
