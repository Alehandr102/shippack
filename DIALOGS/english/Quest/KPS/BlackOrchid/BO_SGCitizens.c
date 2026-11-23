// Gregg: посланцы главы 6 «Испанская щедрость» - «Чёрная Орхидея» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c
// Оригинал - DamnedDestiny\SpanishGenerosity\Characters.c, узлы 5-14.
// Четыре одноразовых горожанина, каждый со своим узлом:
//   1 - человек Альборетто, зовёт в Сантьяго       (Alboretto Man 1);
//   2 - человек Кендарта, зовёт в Ле-Франсуа       (Kendart Man 1);
//   3 - человек Кендарта с письмом: Сан-Хуан наш   (Kendart Man 2);
//   4 - человек Кендарта: эскадра на подходе       (Kendart Man 3).
// Все четверо бессмертны на время реплики и уходят своим ходом - см. логику.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sLastName = GetHeroLastName();


	switch (Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "...(looks away)... Beg pardon, captain, I took you for another.";
			link.l1 = "It happens.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Captain " + sLastName + "? I was asked to pass on a word. A certain senor in Santiago would like to speak with you. He has business with you, and they say it is large.";
			link.l1 = "What senor? Has your business a name?";
			link.l1.go = "1a";
		break;

		case "1a":
			dialog.text = "Emilio Alboretto. He waits in the tavern at Santiago, and asked me to say he is prepared to wait a long while.";
			link.l1 = "Tell him I shall come.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityFirstToSantiago");
		break;

		case "2":
			dialog.text = "Captain " + sLastName + ", I was asked to pass this on by word, without a letter. Alejon Kendart awaits you in the room upstairs, in the tavern of the pirate settlement of Le Francois on Martinique.";
			link.l1 = "Kendart? But he... Hey, wait!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityToLeFransua");
		break;

		case "3":
			dialog.text = "Captain " + sLastName + ". Alejon Kendart asked me to hand you this, and to say the thing is done.";
			link.l1 = "...(unfolding the letter)... Give it here.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityToSanJuan");
		break;

		case "4":
			dialog.text = "Captain! Word from the cape - sails. Four of them, standing straight in for San Juan, and not to trade.";
			link.l1 = "So it comes. To the ships.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityPrepareFinalBattle");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
