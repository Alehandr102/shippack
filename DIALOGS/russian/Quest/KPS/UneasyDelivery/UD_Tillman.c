// Gregg: Ричард Тилльманн - квест «Товар для Ричарда Тилльманна» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\uneasydelivery.c

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
			NextDiag.TempNode = "First time";
			if (CheckAttribute(PChar, "UneasyDelivery.Cancel"))
			{
				dialog.text = "Что вам ещё надо?! Мы уже с вами обо всём поговорили!";
				link.l1 = "Не волнуйся так, торгаш. Уже ухожу.";
				link.l1.go = "exit";
				break;
			}
			if (sti(PChar.UneasyDelivery) >= 3)
			{
				dialog.text = "Ах, это вы, капитан " + GetFullName(PChar) + "? Как продвигаются ваши дела по поводу моего поручения?";
				link.l1 = "Я давно выполни" + GetSexPhrase("л", "ла") + " его, " + NPChar.lastname + ".";
				link.l1.go = "exit";
				break;
			}
			if (sti(PChar.UneasyDelivery) > 1)
			{
				dialog.text = "Ах, это вы, капитан " + GetFullName(PChar) + "? Как продвигаются ваши дела по поводу моего поручения?";
				link.l1 = "Я вовсю работаю над ним.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Что вам надо?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = GetFullName(NPChar) + ", к вашим услугам. С кем имею честь?";
			link.l1 = "Капитан " + GetFullName(PChar) + ". Я доставил" + GetSexPhrase("", "а") + " вам партию какао.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Ну, наконец-то! Почему вы так долго добирались до Барбадоса? Я жду вас вот уже какую неделю. Объяснитесь!";
			link.l1 = "Это какао вам должен был доставить капитан Томми Толстяк, но так случилось, что я завладел" + GetSexPhrase("", "а") + " его кораблём и вашим грузом. Я узнал" + GetSexPhrase("", "а") + ", что он должен быть доставлен вам, и без промедления взял" + GetSexPhrase("", "а") + " курс на Барбадос. С моей стороны задержек не было, а из-за чего Толстяк медлил, я не знаю.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Неважно, кто доставил мне груз, важно то, что он доставлен с огромным опозданием. Я надеялся продать его за приличные деньги, но сейчас спрос упал, и за это какао я ровным счётом ничего не получу.";
			link.l1 = "Подождите пару месяцев, спрос снова вырастет, вот тогда-то и продадите его.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Не учите меня, капитан " + GetHeroLastName() + "! Я прекрасно знаю, что и как мне делать. А теперь ближе к делу: я готов заплатить вам за доставку 750 пиастров.";
			link.l1 = "750 пиастров? Это как минимум вдвое меньше той суммы, что я рассчитыв" + GetSexPhrase("ал", "ала") + " получить.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "В сроки вы не уложились, вот и плата за доставку соответствующая. Да, вы не виноваты, виноват тот капитан, что задержался. Но вы приняли его обязательства по доставке, так? Так. Груз должен был быть доставлен ещё несколько недель назад, так? Так. Ну вот и делайте выводы.";
			link.l1 = "Чёрт с вами, давайте ваши деньги.";
			link.l1.go = "6";
		break;

		case "6":
			AddMoneyToCharacter(PChar, 750);
			RemoveCharacterGoodsSelf(PChar, GOOD_CHOCOLATE, 50);
			dialog.text = "Я рад, что мы договорились. Мои ребята уже разгружают ваш корабль.";
			link.l1 = "...(сквозь зубы)... Приятно было иметь с вами дело, " + NPChar.name + ". Прощайте.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Постойте, капитан " + GetHeroLastName() + ", у меня есть для вас одно поручение, и если вы выполните его в срок, неплохо заработаете.";
			link.l1 = "Что за поручение?";
			link.l1.go = "8";
		break;

		// Gregg: вход через трактирщика - без пролога у ГГ нет ни какао, ни истории
		// с Толстяком, поэтому разговор начинается сразу с поручения
		case "30":
			dialog.text = GetFullName(NPChar) + ", к вашим услугам. Вас, надо полагать, прислал трактирщик? У меня и правда есть дело для капитана с крепким судном.";
			link.l1 = "Слушаю.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Доставить небольшую партию фарфора моему товарищу Жаку Пренолю в Порт-о-Принс. Выполните - получите от него 7000 пиастров. Ну что, согласны? Советую не отказываться.";
			link.l1 = "Я соглас" + GetSexPhrase("ен", "на") + ".";
			link.l1.go = "9";
			link.l2 = "Нет, я отказываюсь.";
			link.l2.go = "11";
		break;

		case "9":
			dialog.text = "Отлично. Только, ради бога, осторожнее с фарфором. Одно неосторожное движение, и он может разбиться. И на этот раз никаких ограничений по времени.";
			link.l1 = "Хорошо, я всё понял" + GetSexPhrase("", "а") + ".";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Вот это письмо отдадите Пренолю. Это всё. Попутного ветра, капитан " + GetHeroLastName() + "!";
			link.l1 = "Удачного дня.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryAgree");
		break;

		// Gregg: в К:ПС отказ был закомментирован и ветка не использовалась.
		// Возвращена: игрок вправе не браться, а квест это состояние уже умеет.
		case "11":
			// Gregg: регистрация обработчика ДО DialogExit - вызов функции с параметром
			// после закрытия диалога валится в "Invalid call"
			AddDialogExitQuestFunction("UneasyDeliveryCancel");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		// Gregg: ГГ пойман на воровстве. Взят местный, короткий вариант блока:
		// ветки К:ПС с откупом опирались на GetStealMoneyLeaderShip/GetStealMoneyLuck,
		// которых в CSP нет. Образец - Graveyard_Watcher_dialog.c.
		case "Man_FackYou":
			dialog.text = LinkRandPhrase("Да ты " + GetSexPhrase("вор, милейший! Стража, держи его", "воровка! Стража, держи её") + "!!!", "Вот это да! Чуть я загляделся, а ты сразу в сундук с головой! Держи " + GetSexPhrase("вора", "воровку") + "!!!", "Стража! Грабят!!! Держи " + GetSexPhrase("вора", "воровку") + "!!!");
			link.l1 = "А-ать, дьявол!!!";
			link.l1.go = "fight";
		break;

		case "fight":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LAi_SetOwnerTypeNoGroup(npchar);
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
