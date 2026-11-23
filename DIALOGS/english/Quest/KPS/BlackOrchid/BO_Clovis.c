// Gregg: Clovis Dubois - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
// Логика - Program\Quests\reaction_functions\blackorchid.c.
// Клови нанят Кендартом за 300 000 как соглядатай: две недели он смотрел за дочерью
// губернатора. Драки в городе не входили в его план и ему самому не нравятся.

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
			// Gregg: strictly by state. Without the gate any second approach replayed the meeting
			// on the beach and fired the march into town all over again.
			if (PChar.FindAndDestroy == "11")
			{
				dialog.text = "Clovis Dubois. Kendart paid, I did the work.";
				link.l1 = "Two weeks for three hundred thousand. What did it buy?";
				link.l1.go = "1a";
			}
			else
			{
			// Gregg: a hint while the rags are being fetched. It is easy to carry one set out of
			// the chest instead of two, and then the quest condition simply stays silent.
			if (CheckAttribute(PChar, "FindAndDestroy.NeedSuits"))
			{
				dialog.text = "Take both sets, Captain. One is mine, and I am not walking into that hall in an officer coat.";
				link.l1 = "Both, then.";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "I am here. Say the word and we go.";
				link.l1 = "We hold for now.";
				link.l1.go = "exit";
			}
			}
			NextDiag.TempNode = "First time";
		break;

		case "1a":
			dialog.text = "Two weeks in a rented house by the square with a spyglass. I saw the girl twenty three times, and all twenty three in the same window. She does not leave the town hall.";
			link.l1 = "Is the key on her?";
			link.l1.go = "1b";
		break;

		case "1b":
			dialog.text = "At her neck, on a cord. Never takes it off, and by the look of her has no idea what it is. Her father I did not see once in two weeks, and a governor of Tortuga, Captain, is a public man.";
			link.l1 = "Where has he got to?";
			link.l1.go = "1c";
		break;

		case "1c":
			dialog.text = "Nowhere. He is in the hall, he steps out on the balcony, he waves. Only it is not him. ...(a pause)... Twelve years I have known the man, and I look and do not know him. The walk is wrong.";
			link.l1 = "And you sat on that for two weeks?";
			link.l1.go = "1d";
		break;

		case "1d":
			dialog.text = "I wrote to Kendart twice a week. He told me to wait for you and not to go into the town alone. ...(picks up his bag)... Come, I will report the rest on the way.";
			link.l1 = "Report as we walk.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTortugaExitTown");
		break;

		case "9":
			dialog.text = "...(stops, whispering)... Hold. Twelve at the gate.";
			link.l1 = "How many yesterday?";
			link.l1.go = "9a";
		break;

		case "9a":
			dialog.text = "Four. I counted every day for a fortnight and it was four. ...(slowly)... And they are already looking this way, Captain. A garrison does not look like that.";
			link.l1 = "Then there will be no talking.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyInTortugaExitTownBattle");
		break;

		case "12":
			dialog.text = "Twelve. And not one of them shouted, called for help or ordered us to lay down arms. They fought in silence.";
			link.l1 = "And you never noticed that in a fortnight?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "I noticed something else. People in this town stopped greeting one another. I put it down to fear. ...(wipes his blade)... I put it down wrong. Into the town, then, and stay close and speak to nobody.";
			link.l1 = "Lead on.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyInTortugaTown");
		break;

		case "16":
			dialog.text = "Seven. Captain, I know every second man here by sight. Not one of them was familiar.";
			link.l1 = "Where now?";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "The house on the corner. I rented it for the fortnight and watched from there. What we need to get into the town hall is in it too.";
			link.l1 = "Go on.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTortugaShelter");
		break;

		case "18":
			dialog.text = "The chest in the corner. Open it, there are two sets.";
			link.l1 = "Two sets of what?";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Rags. The town hall admits officers and beggars. We shall not pass for the first, and beggars are not counted as people there, so nobody looks at them closely.";
			link.l1 = "...(goes to the chest)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyFindClothes");
		break;

		case "20":
			dialog.text = "Got them? Change, and hide your blade. Leave the musket: there are no beggars with muskets.";
			link.l1 = "Changing.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyDialogInShelter");
		break;

		case "21":
			dialog.text = "Gone. ...(exhales)... Captain, he looked at you longer than at me, and not at your face but at your hands. I do not like it.";
			link.l1 = "To the town hall.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTownhall");
		break;

		case "28":
			dialog.text = "Done. ...(looks at the body)... And here is a thing, Captain. The girl wears a key at her neck, I watched it through a glass for a fortnight. This one had the same at his.";
			link.l1 = "We will ask the one upstairs. Go to the door and wait.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyTalkWithMayor");
		break;

		case "29":
			dialog.text = "The girl has no part in it, Captain. I watched her a fortnight: she is kept here, she does not do the keeping.";
			link.l1 = "I know. We take her out.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyRoomOfficers");
		break;

		case "30":
			dialog.text = "We do. Only not by the gate, they know about it there. There is a way from the town hall cellar into the caves. I found it on the fourth day and did not honestly expect to need it.";
			link.l1 = "Take us through the cellar.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToDungeon");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
