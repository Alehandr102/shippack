// Gregg: Ален Рошфор, штурман «Анны» - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "Ждём вашего слова, капитан.";
			link.l1 = "Пока стоим.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			// LEO: срок в три дня относится к приходу «Анны», а не к исчезновению Мориса.
			dialog.text = "Вы к Морису? Опоздали. Он сошёл на берег и так и не вернулся.";
			link.l1 = "А вы кто?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Ален Рошфор, штурман. «Анна» его, а вожу её я, потому что Морис последние два года смотрит только в одну сторону.";
			link.l1 = "В сторону Брюса Фишера.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "В неё самую. Отец Фишера отнял у Мориса «Искателя» лет двадцать назад, и с тех пор у него в голове ничего другого нет. Как Рик умер, Морис переключился на сына.";
			link.l1 = "Куда он сошёл?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "На дальний берег Доминики. Ему сказали, что «Искатель» там килюется. Он взял пистолет, сошёл на берег на шлюпке и запретил идти за ним. «Анну» к Сан-Хуану привёл я, без него.";
			link.l1 = "Покажите место.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToCaiman");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
