// Gregg: Alehon Kendart - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			// Gregg: Kendart waits for the hero four times this chapter - split by quest state
			if (PChar.FindAndDestroy == "2")
			{
				dialog.text = "Did you find Bartelemi?";
				link.l1 = "We did. Dead.";
				link.l1.go = "5";
			}
			else
			{
				if (PChar.FindAndDestroy == "6")
				{
					dialog.text = "I have gone through Fisher papers. Sit down.";
					link.l1 = "I am listening.";
					link.l1.go = "25";
				}
				else
				{
					if (PChar.FindAndDestroy == "8")
					{
						dialog.text = "Show me what was in the chest.";
						link.l1 = "A golden key and a letter from Fisher.";
						link.l1.go = "30";
					}
					else
					{
						if (PChar.FindAndDestroy == "10")
						{
							dialog.text = "Clovis has answered.";
							link.l1 = "What does he write?";
							link.l1.go = "35";
						}
						else
						{
						// Gregg: the return from Tortuga. In K:PS the glass was opened by a separate
						// screen (UnlockClock.c) which CSP has not got, so the scene plays here.
						// Gregg: the return from Puerto Rico, the close of the chapter.
						if (PChar.FindAndDestroy == "15")
						{
							dialog.text = "...(rises to meet you)... Alive. And the man o war?";
							link.l1 = "Silent. And the man who steered her is dead.";
							link.l1.go = "45";
						}
						else
						{
						if (PChar.FindAndDestroy == "13")
						{
							dialog.text = "...(looks up)... Back. Both of you alive. The key?";
							link.l1 = "Both of them. Here.";
							link.l1.go = "38";
						}
						else
						{
							dialog.text = "I am working. Look in later.";
							link.l1 = "I shall not disturb you.";
							link.l1.go = "exit";
						}
						}
						}
					}
				}
			}
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(without raising his head)... Raul died in the night. He knew he would, and yesterday he told me everything.";
			link.l1 = "Everything about what?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "About the glass. Thirty years ago Andersen ordered him not merely to make it but to lock a power inside it. Raul built two levels of protection, each with its own key. Opening the glass with both keys means letting out what is in it, and destroying it.";
			link.l1 = "And without the keys?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Without the keys the glass will not break, will not sink and will not open. I have tried. The keys were lost more than thirty years ago, and about the second I have nothing at all.";
			link.l1 = "Then we start with the first.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "The first key is gold, of an unusual shape. Rick Fisher, an English privateer, took it off Andersen body in the cabin during the boarding. Raul was in the same cupboard and saw it all.";
			link.l1 = "Where is Fisher now?";
			link.l1.go = "5a";
		break;

		case "5a":
			dialog.text = "Rick gave up the trade two years ago and left his son Bruce the frigate Seeker and, by report, that same key. Bruce, one must suppose, carries on his father business: looking for whatever the key opens.";
			link.l1 = "How do I find him?";
			link.l1.go = "6a";
		break;

		case "6a":
			dialog.text = "That is the harder part. But I have found a man who has been looking for Bruce longer than I have: a Frenchman, Moris Bartelemi. Rick Fisher once took the Seeker from him, and Moris has not been able to let it go since.";
			link.l1 = "And where is he?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "In three days his barquentine Anna will lie off San Juan. Talk to him.";
			link.l1 = "I will wait for the Anna.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyStart");
		break;

		case "5":
			dialog.text = "...(a long silence)... So that trail is cut too. Very well. What was on him?";
			link.l1 = "A loaded pistol he never got to fire.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Never got to. ...(turns the pistol over)... Moris was not hunting Bruce alone, he had a squadron of his own. Somebody from those men is still alive and must know where to look for the younger Fisher.";
			link.l1 = "Have you a name?";
			link.l1.go = "7b";
		break;

		case "7b":
			dialog.text = "Harvey Murom. You can usually find him at the San Juan tavern, drinking at someone else's expense. Ask him.";
			link.l1 = "I will.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyFindKharviMurom");
		break;

		case "18":
			dialog.text = "A key. ...(holds it to the light)... An ordinary one. Steel, a plain bit, they make them by the dozen in any port.";
			link.l1 = "Ordinary? I took it off Bruce Fisher.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "I believe you. That does not make it the golden key of unusual shape. Bruce spent thirty years looking for what his father key opened, and it seems he found something: what he carried was not that key but another.";
			link.l1 = "Then what is this one for?";
			link.l1.go = "20a";
		break;

		case "20a":
			dialog.text = "That is what I shall find out. Bruce must have kept notes, and you brought his papers along with him. Give me a few days.";
			link.l1 = "Go through them. I will wait.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyFindUnlocker");
		break;

		case "25":
			dialog.text = "Worked it out. The younger Fisher kept his records neatly, and I thank him for it. It is a chest key, and the chest stands in a cave near Cumana.";
			link.l1 = "Why there of all places?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "Because Rick Fisher hid it where nobody would think to look: among runaways from the quarries. They have guarded it eight years and cannot open it, because the key was with Bruce.";
			link.l1 = "The key is with me now.";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "It is. Only the lock has seized solid in eight years, so take a lockpick along or you will be at it until evening.";
			link.l1 = "A lockpick I can find. Cumana, then.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyFindMines");
		break;

		case "30":
			dialog.text = "...(reads the letter twice)... There it is, the gold one. And here at last is the second key. Fisher writes that he sold it twenty years ago to a Frenchman, and the Frenchman gave it to his daughter.";
			link.l1 = "Who is the father?";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "The governor of Tortuga. The key hangs at his daughter neck as an ornament, and she most likely has no idea what it is.";
			link.l1 = "Tortuga, then.";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "Yes, but not by storm. I know a corsair, Clovis Dubois, a born watcher. For a fee he will find out everything about the girl: when she goes out, where, how many men are with her.";
			link.l1 = "What is his fee?";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "Three hundred thousand. Dear, but cheaper than taking a town. I shall take it out of what we lifted from the Mongrel; you rest meanwhile, Clovis needs a fortnight.";
			link.l1 = "Let him count. I will be back in a fortnight.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyWaitTime");
		break;

		case "35":
			dialog.text = "The answer is one I do not care for. The girl never leaves the town hall at all, and nobody has seen the governor for a month.";
			link.l1 = "What does that mean?";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "Clovis believes it is not the governor sitting in that hall but somebody wearing him. And the girl is held there too. He will wait for you on Tortuga, on the shore they call " + GetConvertStr("Shore58", "LocLables.txt") + ".";
			link.l1 = "I am going to him.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTortuga");
		break;

		case "38":
			dialog.text = "...(lays both keys on the table and looks at them a long while)... Thirty years. One off a dead man, one off a girl neck, and they lie side by side as though they were forged yesterday.";
			link.l1 = "Open it.";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "...(takes out the glass and sets it before him)... Step back, Captain. I do not know what comes out of it, and I want you whole.";
			link.l1 = "...(steps back)...";
			link.l1.go = "40";
		break;

		case "40":
			dialog.text = "...(sets the first key and turns it; it goes hard)... And the second... ...(a click)...";
			link.l1 = "...(says nothing)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyOpenClock");
		break;

		case "42":
			dialog.text = "...(a long silence)... The glass is open. Both keys turned, the sand ran out, and it became an ordinary object. And then this man came and spoke about a ship.";
			link.l1 = "What ship?";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "A black man o war with no lights, standing for Puerto Rico, and the storm comes with her. She appeared at the exact hour we opened the glass, and I do not take that for coincidence.";
			link.l1 = "What was inside has come out.";
			link.l1.go = "44";
		break;

		case "44":
			dialog.text = "So it seems. ...(lays the glass on the table)... Raul said the power could be destroyed, but that it would not vanish, it would be released. He did not say into what.";
			link.l1 = "Then we shall see for ourselves.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToPuertoRico");
		break;

		case "45":
			dialog.text = "...(a long silence)... So that is that. Thirty years, and all of it fitted into one shot. ...(picks the glass off the table)... Look: no sand, no skull, the silver has lightened. An ordinary thing.";
			link.l1 = "Quite ordinary?";
			link.l1.go = "46";
		break;

		case "46":
			dialog.text = "Not quite. Something is left in it, only now it keeps quiet. ...(holds it out)... Take it. It is hard for me to look at, and you will have something to remember this by.";
			link.l1 = "I will take it.";
			link.l1.go = "47";
		break;

		case "47":
			dialog.text = "And rest. Look in on me in a fortnight: there is a piece of business, but neither of us should touch it now.";
			link.l1 = "In a fortnight, then.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyEnd");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
