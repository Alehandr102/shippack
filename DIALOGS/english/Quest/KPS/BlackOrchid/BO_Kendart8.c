// Gregg: Alehon Kendart - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			dialog.text = "I am working. Look in later.";
			link.l1 = "I shall not disturb you.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "I remembered. Thirty years it sat in my head out of reach, and now it came back whole and at once. Sit down.";
			link.l1 = "Where did you see the mark?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "On a sail. I was thirty, newly come to the Caribbean and full of intent to explain the place. Our brigantine was bound for Nevis, and a ship of the line stood out to meet us. On her fore was a skull in a ring of bones.";
			link.l1 = "The same mark as on the glass.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "The same. She belonged to Tony Andersen. They called him the Merciless, and not for his prose.";
			link.l1 = "Andersen. The one who fired on Havana?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "The same. One broadside, to hurry the Spaniards over the ransom. Our captain gave battle, and it was the last thing he did. Andersen had every survivor put to the sword.";
			link.l1 = "Every one but you.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "But me. I thought it was the spectacles and the look of me. It turned out he wanted a man who could read. They brought me to his cabin and explained that I would be making a weapon.";
			link.l1 = "A weapon. The hourglass.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "It seems so. And I was not to work alone, but with a Spaniard named Raul Esperanto. And here is the thing that matters, Captain. I understood not one word of what he was doing. He could have said so to Andersen on the first day.";
			link.l1 = "But he did not.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "He did not. For half a year he did my work for me and said nothing. Then came the fog, and the English, and the boarding. They carried me off senseless.";
			link.l1 = "And Andersen?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Killed. Raul was carried on the same ship as I was, but we were not allowed to speak: he was a Spaniard and a prisoner. We exchanged two sentences the whole voyage, and one of them I remember still.";
			link.l1 = "Which one?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "He said: if it is ever found, do not break it. I did not understand him then. Now there is a thing with Andersen mark on my table, and I begin to.";
			link.l1 = "Have you tried to break it?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "...(looks away)... I have. It will not break, will not come apart and will not sink. I am ashamed to say I even tried to drown it in the bay. It came back to my table.";
			link.l1 = "Came back?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "I will not discuss that. We need Raul. He is the only man who knows everything about that glass. Find him and question him, or better still bring him here.";
			link.l1 = "Where do I look?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Nevis. They threw him into the Charlestown prison as soon as he was landed. I understand that thirty years is thirty years, and that we shall most likely find an entry in a ledger. But there is nothing else.";
			link.l1 = "Nevis, then.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoStart");
		break;

		case "13":
			dialog.text = "...(stands up)... Alive. You truly brought him. Did it take long?";
			link.l1 = "Longer than I reckoned. But I never doubted he was alive.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "And you were right. Well, you shall tell it in order later, but for now...";
			link.l1 = "Wait. On the way I met somebody who should not have been there.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Whom?";
			link.l1 = "There is a ghost in the Charlestown prison. Joe Newman, a former prisoner. Fifty years dead.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "I have not heard of him. And what of the ghost?";
			link.l1 = "He would not talk. I had to persuade him with a blade.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "...(sits slowly back down)... You fought a ghost. And won.";
			link.l1 = "It was hard work, but yes.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "That is impossible. Ghosts are deathless. That is what makes them ghosts.";
			link.l1 = "Mine was not. I checked.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "...(a long silence)... Very well. What happened after?";
			link.l1 = "He told me about Raul and vanished. That is all.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Then either ghosts can be killed, or he was not a ghost. I prefer the second and it explains less.";
			link.l1 = "The glass first, ghosts after.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "The glass will have to wait. Raul is in a bad way: first the Inquisition, then a long passage that left him worse rather than better.";
			link.l1 = "How long?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "Until tomorrow. Today he will not say a word, but with me, God willing, he will talk. And one thing more, Captain. For thirty years I believed I lived because of my spectacles. It turns out I lived because a stranger kept his mouth shut for half a year.";
			link.l1 = "Now you know whom you owe. I will call tomorrow.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoEnd");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
