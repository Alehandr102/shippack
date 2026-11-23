// Gregg: посланцы главы 6 «Испанская щедрость» - «Чёрная Орхидея» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c
// Оригинал - DamnedDestiny\SpanishGenerosity\Characters.c, узлы 5-14.
// Четыре одноразовых горожанина, каждый со своим узлом:
//   1 - человек Альборетто, зовёт в Сантьяго       (Alboretto Man 1);
//   2 - человек Кендарта, зовёт в Ле Франсуа       (Kendart Man 1);
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
			dialog.text = "...(отводит глаза)... Простите, капитан, обознался.";
			link.l1 = "Бывает.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Капитан " + sLastName + "? Меня просили передать. Один сеньор из Сантьяго хотел бы с вами поговорить. Дело у него к вам, и, говорят, крупное.";
			link.l1 = "Какой ещё сеньор? Имя есть у твоего дела?";
			link.l1.go = "1a";
		break;

		case "1a":
			dialog.text = "Эмилио Альборетто. Он ждёт в таверне Сантьяго и просил сказать, что готов ждать долго.";
			link.l1 = "Передай, что приду.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityFirstToSantiago");
		break;

		case "2":
			dialog.text = "Капитан " + sLastName + ", меня просили передать на словах, без письма. Алехон Кендарт ждёт вас в комнате наверху, в таверне пиратского поселения Ле Франсуа на Мартинике.";
			link.l1 = "Кендарт? Он же... Эй, постой!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityToLeFransua");
		break;

		case "3":
			dialog.text = "Капитан " + sLastName + ". Алехон Кендарт просил передать вам вот это и сказать, что дело сделано.";
			link.l1 = "...(разворачивая письмо)... Дай сюда.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityToSanJuan");
		break;

		case "4":
			dialog.text = "Капитан! С мыса передали: паруса. Четыре, идут прямо на Сан-Хуан, и идут не торговать.";
			link.l1 = "Дождались. К кораблям.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityPrepareFinalBattle");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
