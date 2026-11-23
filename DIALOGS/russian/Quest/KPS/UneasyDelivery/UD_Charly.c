// Gregg: Чарли Большая Пиписка, связной на жемчужном промысле - квест
// «Товар для Ричарда Тилльманна» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\uneasydelivery.c
//
// Узел 1 требует ввести ключевую фразу руками. Правильная - «вот и славно, вот и хорошо»,
// её ГГ узнаёт от Шона, прочитав письмо с тела убийцы Тилльманна.

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
			dialog.text = "Что вам надо?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Ты есть тот, кто разделаться Львиное Сердце? Назови ключевой фраз.";
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "2";
			NextDiag.TempNode = "1";
		break;

		case "2":
			string sString = GetStrSmallRegister(dialogEditStrings[1]);
			// Gregg: однородная цепочка || - смешивать с && в одном if нельзя
			if (sString == "вот и славно, вот и хорошо" || sString == "вот и славно вот и хорошо")
			{
				// Gregg: в К:ПС Чарли отдавал 25 000 - награду убийцам Тилльманна
				// (схрон 50 000 вместо 300 000), эта - тем же порядком, ради связности.
				AddMoneyToCharacter(PChar, 10000);
				dialog.text = "Вот твоя деньга.";
				link.l1 = "Слушай, Чарли, я и мои люди недовольны своим куском пирога, они хотят большего. Мне нужно увидеть Преноля, чтобы потребовать с него больше денег. Где его найти?";
				link.l1.go = "3";
			}
			else
			{
				dialog.text = "Ключевой фраз неверно. Чарли больше не иметь дело с Белый Брат.";
				link.l1 = "Аргх!..";
				link.l1.go = "exit";
				AddDialogExitQuestFunction("UneasyDeliveryDialogWithCharlyFailed");
			}
		break;

		case "3":
			dialog.text = "Чарли не знать, где есть Преноль. Чарли только отдать деньга. Чарли ничего больше не делать.";
			link.l1 = "Чарли, тебе ведь не нужны неприятности? Не нужны? Ответь мне, где Преноль, и у тебя их не будет.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Чарли не вкуривать, о чём говорить Белый Брат. Чарли не понимать.";
			link.l1 = "Всё ты вкуриваешь. Повторю ещё раз. Не говоришь, где Преноль - есть неприятности, Чарли плохо. Говоришь, где Преноль - нет неприятностей, Чарли хорошо.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Чарли ничего не знать, Чарли только делать свою делу, отдавать деньга.";
			link.l1 = "Значит, неприятности, Чарли. Хорошо, я устрою тебе...";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Нет! Нет! Чарли сказать, Чарли сказать! Преноля искать в острове Бермудо, в острове Бермудо, Преноля там, Чарли точно знать.";
			link.l1 = "Спасибо.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Белый Брат! Не надо говорить Преноль, что Чарли тут вам сказать, не надо выдавать Чарли, Чарли просто не хотеть плохо.";
			link.l1 = "Хорошо, не скажу.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Чарли не виноват, Чарли не хотеть плохо. Ты не выдать Чарли?";
			link.l1 = "Не выдам. Всё, отвали от меня.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryDialogWithCharlyGood");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
