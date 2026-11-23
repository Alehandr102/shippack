// Gregg: Alehon Kendart - «Чёрная Орхидея», глава 7 «Маленький ублюдок».
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
			// Gregg: the clock is found - that is the only errand he is waiting on
			if (PChar.SmallMongrel == "17")
			{
				dialog.text = "At last. What have you brought?";
				link.l1 = "This. Taken off the Little Mongrel.";
				link.l1.go = "16";
			}
			else
			{
				dialog.text = "I am working. Look in later.";
				link.l1 = "I shall not disturb you.";
				link.l1.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Ah, it is you. Good. Sit down, this will take a while... and I am afraid it will not be pleasant.";
			link.l1 = "Unpleasant how?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "In that I am about to ask you for a thing that may kill you. And I cannot promise I would understand what did it.";
			link.l1 = "Men have tried to kill me before. It has not taken yet.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Men. This may not be men. I have held for thirty years that nothing is inexplicable, only unexplained. This is the first case where I am not certain.";
			link.l1 = "Come to the point, Alehon.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "The point. Have you heard of the Little Mongrel?";
			link.l1 = "No. A man or a ship?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "A ship. A heavy frigate. The Spaniards named her, and not out of affection. Some fifteen years ago two fluyts running from Maracaibo to Belize met her off Amatike Bay. Empty, adrift, not one soul aboard.";
			link.l1 = "And they took her as a prize, of course.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "They did. Put thirty two men aboard as a prize crew and sailed on. By morning the whole crew was hanging from the yards. Thirty two bodies. Nobody heard a sound in the night, and the sea was calm.";
			link.l1 = "Someone must have lived to tell it.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "One sailor. He reached Belize, and I was the one who listened to him. He could barely speak, and a fortnight later he died, and the surgeon could not say of what.";
			link.l1 = "One story is only one story.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "That is exactly how I reasoned at the time. And then, years later, a pirate named Jacob told me the same thing. With one difference: his prize crew was not hanged but shot, and piled in the hold.";
			link.l1 = "And Jacob was the only one left.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "The only one. And the Spaniard was the only one. Do you see what I dislike? Not the dead. The sea has dead enough. I dislike that each time exactly one teller is left over.";
			link.l1 = "As though somebody wants the story carried.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "...(after a pause)... That is the thought I forbade myself. It took you a minute.";
			link.l1 = "And where is she now?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "A month ago I was told Jackman has her. Beached by the pirate settlement on Bermudes, with four men set to watch her. Jackman is close with his money but he is no fool, and he would not keep her for nothing.";
			link.l1 = "Then she has to be bought.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Bought and searched. All of her: hold, cabins, planking. If you find nothing, tow her to San Juan and we will take her apart board by board. Only do not sleep aboard her, I beg you, and leave none of your people on her.";
			link.l1 = "I will not.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "And one thing more. If anything goes wrong, cast her off and leave. I will find another way. I will not find another you.";
			link.l1 = "Sentimental, for a man of science.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "...(drily)... That is not sentiment, it is arithmetic. Go on, then.";
			link.l1 = "I am for Bermudes.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelStart");
		break;

		case "16":
			dialog.text = "...(holding the glass up to the light)... An hourglass. Old work, the silver has gone dark. And what is it about this that has taken hold of you?";
			link.l1 = "Look at the bulb. A skull in a ring of bones.";
			link.l1.go = "17";
			TakeNItems(PChar, "DamnedClock", -1);
		break;

		case "17":
			dialog.text = "I see it. A skull with crossed bones I know, it is daubed on every flag afloat. But the ring...";
			link.l1 = "I have never seen it anywhere.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "And I believe I have. Very long ago. And here is the wretched part: I remember the mark, and where I saw it I do not remember at all.";
			link.l1 = "Then try to remember.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "I shall. Your part is done, the rest is mine. Only memory will not work to order, not even to my own. Give me a week.";
			link.l1 = "You have your week.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Until then, Captain. And thank you. Those thirty two were hanging in my head alone for a long while.";
			link.l1 = "Fare well, Alehon.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelEnd");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
