// Gregg: Alejon Kendart - "Two Sisters", chapter 10 of the K:PS story line.
// Logic - Program\Quests\reaction_functions\twosisters.c.

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
			// Kendart is waited on three times in this chapter - split by quest state
			if (PChar.TwoSisters == "3")
			{
				dialog.text = "Alive. ...(looks at the bandage)... I will not ask. Sit down and talk.";
				link.l1 = "This medallion. Does it mean anything to you?";
				link.l1.go = "17";
				break;
			}
			if (PChar.TwoSisters == "6")
			{
				dialog.text = "All four are here, I have seen them myself. The house opens within the week.";
				link.l1 = "So I have earned my keep.";
				link.l1.go = "28";
				break;
			}
			if (PChar.TwoSisters == "1")
			{
				dialog.text = "Come in, we need to talk. And I will say it at once: the business is idiotic.";
				link.l1 = "Go on.";
				link.l1.go = "1";
				break;
			}
			dialog.text = "All quiet for now. Look in if anything comes up.";
			link.l1 = "I will.";
			link.l1.go = "exit";
		break;

		// The setup
		case "1":
			dialog.text = "Do you remember complaining that San Juan has no brothel?";
			link.l1 = "I remember nothing of the sort.";
			link.l1.go = "2";
			link.l2 = "I do. And?";
			link.l2.go = "2";
		break;

		case "2":
			dialog.text = "Now the corsairs are complaining. Second week running, and louder every day.";
			link.l1 = "Let them complain.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Let them. Only after the complaining comes knife work on the docks, and then half of them leave for the French.";
			link.l1 = "And what do you want from me?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Persuade four women to move to San Juan and bring them here.";
			link.l1 = "...(a long pause)... You are serious?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Entirely. We are sitting on a powder keg, my dear partner.";
			link.l1 = "Send somebody else.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Somebody else they will send to the devil. You they might actually hear out.";
			link.l1 = "That is quite a compliment.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "It is the one you get. Besides, you promised to help this colony, and I was not the only one who heard it.";
			link.l1 = "I did. Fine, who are they?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "The pirates drew up the list themselves. Four: Viki Henderson on Martinique, Natali Remis on the Bermudas and the Lus sisters on Tortuga.";
			link.l1 = "Henderson - the one in the brothel at Fort de France?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "The very one. Say you come from me and she will hear you out.";
			link.l1 = "And how am I to persuade them? Walk up and invite them?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "You will work it out. They will not come for nothing, each will ask her own price.";
			link.l1 = "So I will be working for them as well.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Anything is possible.";
			link.l1 = "...(a heavy sigh)...";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Stop sighing and get to it. Time is short.";
			link.l1 = "...(leave)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersStart");
		break;

		// The satanist's medallion
		case "17":
			dialog.text = "...(takes the medallion)... It does. Where did you get it?";
			link.l1 = "Off a body in the Lus sisters' house. The body disappeared afterwards.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Then listen. A sect wears these, they call themselves animatists. Little is known about them and all of it foul.";
			link.l1 = "Where do I look for them?";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Fifteen years ago their den stood near Spanish Beliz. That was when people began to vanish there, first one at a time, then by the dozen.";
			link.l1 = "Were they driven out?";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "They tried. The fort garrison lost three quarters of its men, and Beliz lost its governor. Nobody touched them after that.";
			link.l1 = "And no one ever found out who they are?";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "The new governor offered a hundred thousand to whoever explained the disappearances. A man named Meril Kreno took the money.";
			link.l1 = "What did he explain?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "Nothing. He wrote a paper about swamp fever, collected his reward, and people kept vanishing.";
			link.l1 = "Is Kreno alive?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "Alive and living in Beliz, the house next to the port office. If anyone knows about that medallion, it is him.";
			link.l1 = "Then I go to him.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "Not head on, " + GetFullName(PChar) + ". He is not the sort who frightens.";
			link.l1 = "One more thing. That satanist got up after he was killed.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "...(grimaces)... Or he was never killed. I have been told that twice, and both tellers were drunk.";
			link.l1 = "I was sober.";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "Which is why I am listening. Find out anything and send me word, and I will dig up the old papers.";
			link.l1 = "Agreed.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToBeliz");
		break;

		// The end
		case "28":
			dialog.text = "You have. The house is open, the corsairs are content, and so am I. ...(after a pause)... About Kreno, by the way, you were right.";
			link.l1 = "What about him?";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "Nothing. Papers fifteen years old, signed in his own hand, and nothing more to look for - you buried him.";
			link.l1 = "Then it is over.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "Wait, do not go. While you were off at sea, a shipment of guns came to us.";
			link.l1 = "What guns?";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "...(lower)... Forty-two pounders. Do not ask where from. If you want them, we will fit them for you.";
			link.l1 = "I want them.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "Then I will have a word with the shipwright. Tell him when you are ready.";
			link.l1 = "I am ready right now.";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "...(a sly look)... Naturally. Two things, then. The ship must be second class or better, and they go on once - take them off and you will not put them back.";
			link.l1 = "Understood. Thank you, Alejon.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersEnd");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
