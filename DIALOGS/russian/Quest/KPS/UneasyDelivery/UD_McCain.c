// Gregg: Боб Маккейн, пьянчуга из пиратской таверны - квест «Товар для Ричарда
// Тилльманна» (перенос из К:ПС). Продаёт место и время встречи Преноля за 5000.
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
			// Gregg: чтение через CheckAttribute - в Storm обращение к несуществующему
			// атрибуту это рантайм-ошибка, а не «false», и функция играет дальше с мусором
			if (CheckAttribute(PChar, "UneasyDelivery.CancelMoneyForBobMcCain"))
			{
				if (PChar.UneasyDelivery.CancelMoneyForBobMcCain == true)
				{
					dialog.text = "Ну, что?";
					if (sti(PChar.money) >= 5000)
					{
						link.l1 = "Вот твои деньги.";
						link.l1.go = "8";
					}
					else
					{
						link.l1 = "У меня пока нет нужной суммы.";
						link.l1.go = "11";
					}
					link.l2 = "Пока ничего.";
					link.l2.go = "11";
					break;
				}
			}
			dialog.text = "Кэп, садись, выпей со мной!";
			link.l1 = "Некогда мне.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Эй, кэп, выпить не хочешь?";
			link.l1 = "Нет желания. Да и занят" + GetSexPhrase("", "а") + " я.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "А я бы на твоём месте выпил. Глядишь, информацией полезной разжился. Ну что, может, присядешь?";
			link.l1 = "Хм...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryDialogWithMcCain");
		break;

		case "4":
			dialog.text = "Вот, другое дело!";
			link.l1 = "Ну хорошо, выкладывай, что там у тебя.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Не торопись, кэп, давай-ка сначала кое-что обсудим. Информация не бесплатная, " + GetSexPhrase("сам", "сама") + " понимаешь, так что придётся платить. Пять тысяч звонких пиастров - и я скажу тебе, где искать Преноля.";
			link.l1 = "Откуда тебе известно, что я его ищу?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Слышал, как ты трепал" + GetSexPhrase("ся", "ась") + " о Преноле с тавернщиком.";
			link.l1 = "Давай сделаем так: я тебе заплачу, но только если твоя информация будет полезна.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Нет, кэп, так дела не делают. Сначала ты отсыплешь в мой карман пять тысяч, и только потом я тебе всё скажу. Иначе - нет денег, нет информации.";
			if (sti(PChar.money) >= 5000)
			{
				link.l1 = "Ну хорошо, вот деньги. Теперь говори, где Преноль.";
				link.l1.go = "8";
			}
			else
			{
				link.l1 = "Я вернусь, когда у меня будут деньги.";
				link.l1.go = "11";
			}
			link.l2 = "Если надумаю, обращусь к тебе за помощью.";
			link.l2.go = "11";
		break;

		case "8":
			AddMoneyToCharacter(PChar, -5000);
			PChar.UneasyDelivery.CancelMoneyForBobMcCain = false;
			dialog.text = "Значит так, кэп, слушай внимательно, второй раз повторять не буду. Сидел я сегодня как обычно за кружечкой рома и слышал разговор трёх человек. Один из них и был тот Преноль, которого ты ищешь. \n Так вот, эти двое, что разговаривали с Пренолем, назначили ему встречу в два часа ночи в бухте разбитого корыта.";
			link.l1 = "А откуда ты знаешь, что один из них был Пренолем?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Те двое обращались к нему не иначе как Преноль. Так что будь уверен" + GetSexPhrase("", "а") + ", это тот, кого ты ищешь.";
			link.l1 = "Ладно, верю. Но если ты мне солгал и сегодня ночью там никого не будет, найду и...";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "За информацию ручаюсь, кэп. Будь уверен" + GetSexPhrase("", "а") + ", она точная.";
			link.l1 = "Ну смотри, если одурачил меня - найду и закопаю.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryDialogWithMcCainEnd");
		break;

		case "11":
			ChangeCharacterAddressGroup(PChar, "Pirates_tavern", "goto", "goto2");
			LAi_SetPlayerType(PChar);
			LAi_SetSitType(CharacterFromID("Bob McCain"));
			PChar.UneasyDelivery.CancelMoneyForBobMcCain = true;
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
