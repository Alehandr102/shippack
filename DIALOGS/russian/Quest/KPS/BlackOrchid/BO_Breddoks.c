// Gregg: Уилл Бреддокс и Харпич - «Чёрная Орхидея», глава 7 «Маленький ублюдок».
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
			dialog.text = "Будь он проклят, этот фрегат.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(не поднимая головы)... Садись, капитан. Помянем мою команду.";
			link.l1 = "Что с ней стало?";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelInPuertoPrincipeTalk");
		break;

		case "2":
			dialog.text = "Рассказывать незачем. Всё равно не поверишь. Тут уже никто не верит.";
			link.l1 = "Я поверю.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Мы стали на якорь в бухте и собирались на берег. И вдруг стемнело. Не как в грозу, а сразу, будто накрыли. Я прижался к борту и смотрел, а не видел собственных рук.";
			link.l1 = "А слышал?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Слышал. Такой звук человек издаёт один раз в жизни, капитан, и второй раз ни с чем его не спутаешь. Он шёл отовсюду. Со всей палубы разом.";
			link.l1 = "И ты прыгнул.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "...(долгая пауза)... Прыгнул. Последнее, что я слышал над водой, был свист клинка возле уха. Я плыл до берега и ждал, что меня догонят.";
			link.l1 = "А наутро?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Наутро вышел на берег и посмотрел на свой корабль. Он стоял пустой. Ни тел, ни крови, ни лодки спущенной. Я в тот же день продал его за бесценок капитану Харпичу и не торговался.";
			link.l1 = "Где Харпич теперь?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Понятия не имею. Снялся вчера под вечер. Если поторопишься - нагонишь.";
			if (sti(PChar.money) >= BO_SM_WAKE_PRICE)
			{
				link.l1 = "Я бы выпил" + GetSexPhrase("", "а") + " с тобой за твоих, но мне надо идти. Вот тысяча, помяни их за меня.";
				link.l1.go = "8";
			}
			else
			{
				link.l1 = "Я бы выпил" + GetSexPhrase("", "а") + " с тобой за твоих, но мне надо идти.";
				link.l1.go = "9";
			}
			AddDialogExitQuestFunction("SmallMongrelFindKharpich");
		break;

		case "8":
			AddMoneyToCharacter(PChar, -BO_SM_WAKE_PRICE);
			dialog.text = "...(смотрит на монеты, потом на вас)... Помяну. Всех тридцать.";
			link.l1 = "Прощай, Бреддокс.";
			link.l1.go = "exit";
		break;

		case "9":
			dialog.text = "Иди, капитан. И на его борт ночью не оставайся, слышишь? Ни " + GetSexPhrase("сам", "сама") + ", ни люди.";
			link.l1 = "Услышал" + GetSexPhrase("", "а") + ".";
			link.l1.go = "exit";
		break;

		case "20":
			dialog.text = "Какого чёрта. Бой ещё не кончен.";
			link.l1 = "Мне нужен твой корабль, Харпич. Тебя я готов" + GetSexPhrase("", "а") + " отпустить.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "Корабль ты получишь через мой труп. И зря радуешься. Он тебе достанется вместе со всем, что на нём.";
			link.l1 = "Значит, через труп.";
			link.l1.go = "exit";
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattleNoParam");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
