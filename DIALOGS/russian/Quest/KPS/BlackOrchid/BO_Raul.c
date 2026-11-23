// Gregg: Рауль Эсперанто - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			// Gregg: трость уже попрошена, но ещё не принесена
			if (CheckAttribute(PChar, "RaulEsperanto.Stick"))
			{
				dialog.text = "Нашли?";
				link.l1 = "Пока нет. Ищу.";
				link.l1.go = "exit";
			}
			else
			{
			// Gregg: благодарность - только когда старик уже вышел, то есть на "12".
			// На "11" он ещё сидит в подвале и благодарить ему не за что.
			if (PChar.RaulEsperanto == "12")
			{
				dialog.text = "Я не знаю, чем вам отплатить, капитан.";
				link.l1 = "Не нужно.";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "Что вам нужно... Разве не видно, что мне и без вас худо.";
				link.l1 = "Потерпите ещё немного, Рауль.";
				link.l1.go = "exit";
			}
			}
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Кто здесь. Я вас не вижу.";
			link.l1 = "Капитан " + GetHeroLastName() + ". Я приш" + GetSexPhrase("ёл", "ла") + " вас отсюда забрать.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(долго молчит)... Меня отсюда не забирают. Меня отсюда выносят.";
			link.l1 = "Сегодня выйдете сами. Идти можете?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Нет. Стою с трудом. Мне нужна палка, любая. У меня была трость, её забрали при аресте и, надо думать, не выбросили.";
			link.l1 = "Поищу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoFindStick");
		break;

		case "4":
			dialog.text = "...(берёт трость обеими руками)... Она. Вот теперь, кажется, дойду.";
			link.l1 = "Тогда на корабль. Он в порту.";
			link.l1.go = "5";
			TakeNItems(PChar, "RaulEsperanto_Stick", -1);
		break;

		case "5":
			dialog.text = "Куда мы идём, капитан?";
			link.l1 = "В Сан-Хуан. Пуэрто-Рико.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "...(останавливается)... Опять к испанцам.";
			link.l1 = "...(улыбается)... Вы отстали от новостей, Рауль. Сан-Хуан давно не испанский.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "А чей же он?";
			link.l1 = "Расскажу по дороге. Сейчас надо идти.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoReturnToSanJuan");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
