// Gregg: диалоги квеста "Легенда о призрачном свете" (перенос из К:ПС).
// Клейн на Бермудах, его связной в Бас-Тере, Адам Кромвель, Фрэнсис Айво,
// Джейк Смитт и Педро Карлос в гроте Невиса.
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "First time":
			// Gregg: у каждого свой отбой. Гард по MoonStone тут не нужен - узел
			// достижим только пока квест идёт, но проверку id оставляем как в оригинале.
			if (NPChar.id == "Adam Kromvel")
			{
				dialog.text = "Do not trouble me.";
				if (pchar.MoonStone == "3") dialog.text = "I shall wait outside the town. Do not forget to confess.";
				link.l1 = "As you say.";
				link.l1.go = "exit";
				break;
			}
			if (NPChar.id == "Klein Man")
			{
				dialog.text = "Have you done your work? No? Then go and do it.";
				link.l1 = "As you say.";
				link.l1.go = "exit";
				break;
			}
			if (NPChar.id == "Klein")
			{
				dialog.text = "Did you not understand? Set sail, and lay a course for Martinique.";
				link.l1 = "...";
				link.l1.go = "exit";
				break;
			}
			NextDiag.TempNode = "First time";
		break;

		case "Adam_Kromvel_1":
			dialog.text = "Who the devil are you?! Get out of here!";
			link.l1 = "Oh, now you have insulted me twice. I challenge you to a duel!";
			link.l1.go = "Adam_Kromvel_2";
		break;

		case "Adam_Kromvel_2":
			dialog.text = "Er... ahem... I accept the challenge. But what is the second reason?";
			link.l1 = "You looked at my woman improperly. I shall wait for you outside the town.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			AddDialogExitQuestFunction("MoonStoneToSentJonsExit");
		break;

		case "Adam_Kromvel_3":
			dialog.text = "Ah, you came. Shall we begin?";
			link.l1 = "One moment. I am curious: whom did you cross badly enough for them to send an assassin?";
			link.l1.go = "Adam_Kromvel_4";
		break;

		case "Adam_Kromvel_4":
			dialog.text = "So you are one of those swine?! Die!";
			link.l1 = "Ah well...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithKromvel");
		break;

		case "Francis_Ayvo_1":
			dialog.text = "And you, it seems, are the hired killer? Die!";
			link.l1 = "Wait!..";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithAjvoGardian");
		break;

		case "Francis_Ayvo_2":
			dialog.text = "Aaah, you killed him!";
			link.l1 = "Be quiet, or you will be next.";
			link.l1.go = "Francis_Ayvo_3";
		break;

		case "Francis_Ayvo_3":
			dialog.text = "All right... q-quiet. B-but who are you?";
			link.l1 = "That does not matter. Better tell me whom you crossed.";
			link.l1.go = "Francis_Ayvo_4";
		break;

		case "Francis_Ayvo_4":
			dialog.text = "A-anyone at all. You would know b-better - you came to k-kill me.";
			link.l1 = "Stop stammering. I want your version.";
			link.l1.go = "Francis_Ayvo_5";
		break;

		case "Francis_Ayvo_5":
			dialog.text = "I have s-stammered since birth. And my d-death could be wanted only by that b-brotherhood of k-killers...";
			link.l1 = "From there in more detail.";
			link.l1.go = "Francis_Ayvo_6";
		break;

		case "Francis_Ayvo_6":
			dialog.text = "It b-began when that b-brotherhood came to the Caribbean. They k-kill for money and s-spare nobody.";
			link.l1 = "And do you know anything of Jake Smitt?";
			link.l1.go = "Francis_Ayvo_7";
		break;

		case "Francis_Ayvo_7":
			dialog.text = "H-he is their chief.";
			link.l1 = "Very well. Now tell me about the moonstone instead.";
			link.l1.go = "Francis_Ayvo_8";
		break;

		case "Francis_Ayvo_8":
			dialog.text = "N-no! I would s-sooner die!";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithAjvo");
		break;

		case "Claine_0":
			MoonStoneToBermudesTavernTalk();
			dialog.text = "And who are you? May I know your name?";
			link.l1 = GetSexPhrase("Nathan Rich.", "I come from Nathan Rich.") + " ...(shows the amulet)...";
			link.l1.go = "Claine_2";
		break;

		case "Claine_1":
			dialog.text = "And who are you? May I know your name?";
			link.l1 = GetSexPhrase("Nathan Rich.", "I come from Nathan Rich.") + " ...(shows the amulet)...";
			link.l1.go = "Claine_2";
		break;

		case "Claine_2":
			dialog.text = GetSexPhrase("I have been waiting for you. Where have you been?", "At last. Has Nathan sent you in his place?");
			link.l1 = GetSexPhrase("Forgive me. Urgent business came up.", "Yes. He has urgent business. I shall do his work.");
			link.l1.go = "Claine_3";
		break;

		case "Claine_3":
			dialog.text = "Very well. My apologies for not bringing your friends into it at once. Where are they, by the way?";
			link.l1 = "They too found urgent business and will not be joining us. To the point.";
			link.l1.go = "Claine_4";
		break;

		case "Claine_4":
			dialog.text = "To the point, then. I need to reach Fort de France.";
			link.l1 = "Very well.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			AddDialogExitQuestFunction("MoonStoneToFortFrance");
		break;

		case "Claine_5":
			dialog.text = "They did not speak of you as a good captain for nothing.";
			link.l1 = "You flatter me.";
			link.l1.go = "Claine_6";
		break;

		case "Claine_6":
			dialog.text = "Enough of that. To business. Settle your affairs, and in a week be in Bas-Terre, where you are lodging. Our man will come and tell you what happens next. He will say «greetings from Klein», and you will show him your amulet in reply. Do not keep him waiting.";
			link.l1 = "Very well, in a week I shall be home.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneExitFortFrance");
		break;

		case "Claine_7":
			dialog.text = "Greetings from Klein.";
			link.l1 = "...(shows the amulet)...";
			link.l1.go = "Claine_8";
		break;

		case "Claine_8":
			dialog.text = "I have been waiting, and was about to leave. Though Klein did warn me of your poor timekeeping.";
			link.l1 = "Forgive the delay. Have you something for me?";
			link.l1.go = "Claine_9";
		break;

		case "Claine_9":
			dialog.text = "I have. There is a man in the Caribbean who is very much in our way.";
			link.l1 = "First I should like to know...";
			link.l1.go = "Claine_10";
		break;

		case "Claine_10":
			dialog.text = "You will ask your questions when the work is done. So: an Englishman, lives in Saint Johnss, named Adam Cromwell. See to it that he ceases to be. How is of no interest to me. The reward is twenty-five thousand.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneToSentJons");
		break;

		case "Claine_11":
			dialog.text = "There you are. Well, did you manage it?";
			link.l1 = "I did. But there is something else.";
			link.l1.go = "Claine_12";
		break;

		case "Claine_12":
			dialog.text = "Has something happened?";
			link.l1 = "Nothing much. Except for this letter.";
			link.l1.go = "Claine_13";
		break;

		case "Claine_13":
			TakeNItems(pchar, "HelpSacredInquisition_Letter3", -1);
			dialog.text = "Well now. Interesting. You have read it, of course? I have new work for you, and it is urgent.";
			link.l1 = "You promised to answer my questions.";
			link.l1.go = "Claine_14";
		break;

		case "Claine_14":
			AddMoneyToCharacter(pchar, 25000);
			dialog.text = "Later. For now, sail to Nevis. Find Francis Ivo there and destroy him. The reward is the same - twenty-five thousand. I shall wait here again. And here is your money for Cromwell.";
			link.l1 = "Understood.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneToNevis");
		break;

		case "Claine_15":
			dialog.text = "Since you are here, may I take the work as done?";
			link.l1 = "Entirely. You promised to answer my questions.";
			link.l1.go = "Claine_16";
		break;

		case "Claine_16":
			dialog.text = "Ask.";
			link.l1 = "What is this moonstone?";
			link.l1.go = "Claine_17";
		break;

		case "Claine_17":
			dialog.text = "I knew you would ask in the end. What exactly do you wish to know? I gather you know something already.";
			link.l1 = "If it is a piece of the moon, how did it come to be here?";
			link.l1.go = "Claine_18";
		break;

		case "Claine_18":
			dialog.text = "We know one thing: the Lord Himself gave it to some ancient civilisation. Took it from the moon and gave it to them.";
			link.l1 = "The Atlanteans, then. Why do you believe it true? And what power does the stone hold?";
			link.l1.go = "Claine_19";
		break;

		case "Claine_19":
			dialog.text = "Well now. You have got far with this - you even know whose it was. And the power... The power is great. Immeasurably great.";
			link.l1 = "As I understand it, you have not found it yet?";
			link.l1.go = "Claine_20";
		break;

		case "Claine_20":
			dialog.text = "True, but we are close. Only you, my friend, will not be invited to share the fruit of our searching.";
			link.l1 = "I understood that at our last conversation.";
			link.l1.go = "Claine_21";
		break;

		case "Claine_21":
			dialog.text = "You know... Klein and I did believe you were " + GetSexPhrase("Nathan Rich", "acting for Nathan Rich") + ". Jake Smitt made us see our mistake. You lie with real artistry - you should be a governor with that gift. I even had the money for Francis ready, but I fear I shall keep it. A pity you have surely spent what you got for Cromwell - I would have taken that too. And give back the letters you found here. And the diary. I know you have them.";
			link.l1 = GetSexPhrase("So I am a dead man.", "So I am doomed.") + " Then tell me at least, before the end: who is Jake Smitt?";
			link.l1.go = "Claine_22";
		break;

		case "Claine_22":
			dialog.text = "He is everything to us. Without him we would not exist. Enough talk, or I shall miss my passage to Martinique. I advise you not to resist - then you will feel no pain at all.";
			link.l1 = "We shall see. Wait for it.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithKleinMan");
		break;

		case "Claine_23":
			dialog.text = "I did not think you would come. You killed one of the finest mercenaries!";
			link.l1 = "I have seen better.";
			link.l1.go = "Claine_24";
		break;

		case "Claine_24":
			dialog.text = "Wait, do not kill me! I can be of use.";
			link.l1 = "Perhaps. It depends on what you know.";
			link.l1.go = "Claine_25";
		break;

		case "Claine_25":
			dialog.text = "What do you wish to know?";
			link.l1 = "What do you know of the stone, and of Jake Smitt?";
			link.l1.go = "Claine_26";
		break;

		case "Claine_26":
			dialog.text = "Of the stone I know little, and Jake is a riddle of a man. A hired killer, uncommonly gifted. Always a step ahead.";
			link.l1 = "Why did you come to the Caribbean?";
			link.l1.go = "Claine_27";
		break;

		case "Claine_27":
			dialog.text = "To settle certain matters with the Spanish governor. And to find a scholar for Jake Smitt.";
			link.l1 = "More about the scholar.";
			link.l1.go = "Claine_28";
		break;

		case "Claine_28":
			dialog.text = "There is some troublemaker here. I found him at once. I did not even remember the name.";
			link.l1 = "Where can I find him?";
			link.l1.go = "Claine_29";
		break;

		case "Claine_29":
			dialog.text = "I do not know. I can tell you where Jake is, but I strongly advise against going near him.";
			link.l1 = "I did not ask for advice. Where is he?";
			link.l1.go = "Claine_30";
		break;

		case "Claine_30":
			dialog.text = "He should be somewhere on Nevis, on the island itself. But think before you sail. And one more thing - do not tell Jake we met. My life is dear to me, I have a family.";
			link.l1 = "You should have thought of your family sooner.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneToNevisIsland");
		break;

		case "Jack_Smitt_1":
			dialog.text = "You are quite the hero, " + GetFullName(pchar) + ". You have rid me of everyone I should have had to share with.";
			link.l1 = "And you are a remarkably vile and greedy sort.";
			link.l1.go = "Jack_Smitt_2";
		break;

		case "Jack_Smitt_2":
			dialog.text = "Come for the key?";
			link.l1 = "The key?";
			link.l1.go = "Jack_Smitt_3";
		break;

		case "Jack_Smitt_3":
			dialog.text = "Though what does it matter. Draw your blade, I like a fair fight.";
			link.l1 = "Tell that to God. If you meet Him.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithJackSmitt");
		break;

		case "Pedro_Carlos_1":
			AddMoneyToCharacter(pchar, 10000);
			dialog.text = "You are my saviour! That idiot held me here by force. Do not trouble yourself: for such a killing you will only be thanked, and besides he was an Englishman, and they are not loved here. I shall even reward you.";
			link.l1 = "I have been looking for him a long while. A pity we never talked - I had much to ask.";
			link.l1.go = "Pedro_Carlos_2";
		break;

		case "Pedro_Carlos_2":
			dialog.text = "Then ask me. I shall answer all I know.";
			link.l1 = "I wanted to know about the moonstone.";
			link.l1.go = "Pedro_Carlos_3";
		break;

		case "Pedro_Carlos_3":
			dialog.text = "Ah, the one that grants the power of God... Well, I can assure you it is an invention from beginning to end.";
			link.l1 = "An invention? But there is so much evidence!";
			link.l1.go = "Pedro_Carlos_4";
		break;

		case "Pedro_Carlos_4":
			dialog.text = "It may be a piece of the moon - though I doubt it. But the power of God it does not grant. You have heard what it is called? The legend of the moonstone of ghostly light. And do you know why ghostly?";
			link.l1 = "No. Why?";
			link.l1.go = "Pedro_Carlos_5";
		break;

		case "Pedro_Carlos_5":
			dialog.text = "The meaning is in the name itself. There is as much power in it as there is light - that is to say, none. Though I do not rule out that it gives its owner something. Certainly not the power of God.";
			link.l1 = "Damn... Then what does it give?";
			link.l1.go = "Pedro_Carlos_6";
		break;

		case "Pedro_Carlos_6":
			dialog.text = "I do not know. Light that is not there, perhaps. I do not know and do not wish to - I dislike wasting time.";
			link.l1 = "Wait. Jake spoke of some key.";
			link.l1.go = "Pedro_Carlos_7";
		break;

		case "Pedro_Carlos_7":
			dialog.text = "Ah yes. Jake spoke of a map. Of where the stone lies, rather. As far as I understood, somewhere here on the island.";
			link.l1 = "Curious.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneCarlosToExit");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
