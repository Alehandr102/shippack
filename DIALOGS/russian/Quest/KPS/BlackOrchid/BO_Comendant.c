// Gregg: Комендант Тортуги, патруль, мэр-двойник и его дочь - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "Проходи.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(разглядывает вас двоих)... Двое нищих в запертом доме. Занятно.";
			link.l1 = "Мы тут ночуем. Дом пустой.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Пустой, да не ничей. Этот дом мой, и сдан он был две недели назад. ...(подходит ближе)... Руки покажи.";
			link.l1 = "...(протягивает ладони)... Смотри, коли надо.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(долго смотрит, потом отступает)... Мозоли не те. Ладно. К утру чтобы вас тут не было.";
			link.l1 = "Уйдём.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyDialogInShelterEnd");
		break;

		case "4":
			dialog.text = "Куда.";
			link.l1 = "В ратушу. Просить.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "...(усмехается)... Проси, кто ж тебе запретит. Только внутрь по одному и молча. Второй ждёт снаружи.";
			link.l1 = "Он подождёт.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyReloadToTownhall");
		break;

		case "7":
			dialog.text = "...(поворачивается)... А, это вы. Руки у вас всё-таки не те.";
			link.l1 = "Ты знал ещё в доме.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyFirstBattleInTownhall");
		break;

		case "8":
			dialog.text = "...(говорит ровно, не мигая)... Вы вошли не туда, капитан. Здесь резиденция губернатора.";
			link.l1 = "Знаю. И знаю, что губернатор не вы.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Я губернатор Тортуги. Спросите любого в этом городе.";
			link.l1 = "Спрошу кого-нибудь за воротами. Там ещё отвечают своими словами.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "...(впервые меняет тон)... Не спросите.";
			link.l1 = "Мне нужен ключ, который носит ваша дочь.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "...(наконец меняет тон)... Значит, вы за этим. Она не знает, что носит, и не узнает. Отсюда её не вынесут.";
			link.l1 = "...(обнажает клинок)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyBattleWithGovernor");
		break;

		case "13":
			dialog.text = "...(на полу, держась за бок)... Наверху. У неё на шее, на шнурке. Он всё равно не мой и не ваш.";
			link.l1 = "Чей?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Того, кто придёт за обоими ключами. ...(закрывает глаза)... Девочка ни в чём не виновата. Уведите её отсюда, слышите.";
			link.l1 = "Наверх.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTownhallRoom");
		break;

		case "15":
			dialog.text = "...(вжимается в угол)... Не подходите. Пожалуйста.";
			link.l1 = "Я не за вами. Вас отсюда выведут.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Отец... Он ведь не отец, да? Я поняла на третий день, а сказать было некому.";
			link.l1 = "Правильно поняли. И ещё: снимите ключ с шеи, он мне нужен. Идёмте.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyInTownhallRoomDialog");
		break;

		case "17":
			dialog.text = "...(в дверях, растерянно)... Господин мэр велел... а господина мэра нет.";
			link.l1 = "Господина мэра больше нет. Идите и никому не мешайте выйти.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyRoomOfficersOut");
		break;

		case "21":
			dialog.text = "Капитан. Мы вас не задерживаем. ...(отходит с дороги)... Мы вас, если честно, благодарим.";
			link.l1 = "Присмотрите за девочкой.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyExitTortuga");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
