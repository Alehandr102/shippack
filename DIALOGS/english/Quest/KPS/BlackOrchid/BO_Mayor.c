// Gregg: the governor of Santiago - the "Black Orchid" trilogy (ported from Damned
// Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// This is a CLONE of the real mayor: same face and same name, but a separate
// character in the clone town. The real governor of Santiago stays alive.

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
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Ah, senor captain. Enjoy your triumph. Enjoy it - you have not long left.";
			link.l1 = "And you, Jose. If your men in the fort do not surrender at our demand, I fear I shall have to kill both you and senora Matilda. How does that suit you?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Captain, step out onto the pier and look at the horizon. Do you see three white specks? That is admiral Merques's squadron, five or six hours from port. The way into the jungle is held by our soldiers and a dozen royal guns. As you see, you have no way out.";
			link.l1 = "There is always a way out, Jose. Write an ultimatum to the fort commandant: 'Grant free passage out of the town to captain " + GetMainCharacterNameDat() + " and his men. Otherwise the governor of Santiago and senora Matilda will be killed'. And your seal and signature below.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "I shall not write that, senor captain. You may kill me here and now if you please. My death will change nothing.";
			link.l1 = "It will give me great pleasure, though. And what of Matilda, Jose? Or have you forgotten that you are bound to protect her by every means?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Allow me to explain something. Matilda is spent material. Her constant failures forced us to find a replacement, and that replacement will arrive in Santiago aboard admiral Merques's flagship. Matilda's death is inevitable, a matter of time: the poison put in her wine at dinner will take effect within an hour, an hour and a half at most. My own death is a matter of time as well - I am gravely ill and shall not last two days.";
			link.l1 = "You are lying, you Spanish cur!";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "No, senor captain. All I have said is true. Believe it or not, it changes nothing. You are in a trap, and all that remains to you is to await your death.";
			link.l1 = "We shall see about that, Jose. Now speak: where is Matilda?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "I should like to know that myself. When the assault began she vanished without a trace. Though she has little time left. Hm... ...(he muses)... You know, captain, how pleasant it is when everything you set out to do in life comes off. When you are ready to go to the next world knowing nothing holds you here. When you see before you captain " + GetMainCharacterNameGen() + " - the most successful of the seekers - driven into a corner and... how shall I put it.";
			link.l1 = "Do not trouble yourself, Jose.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactKillSantiagoGovernor");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
