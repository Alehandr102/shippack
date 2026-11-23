// Gregg: Жак Преноль - квест «Товар для Ричарда Тилльманна» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\uneasydelivery.c
//
// Присказка Преноля «Вот и славно, вот и хорошо» (узлы 9 и 18) - не украшение:
// позже она окажется ключевой фразой для связного на жемчужном промысле.

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
			if (sti(PChar.UneasyDelivery) > 3)
			{
				dialog.text = "Как продвигается дело, которое я тебе поручил?";
				link.l1 = "Я работаю над ним.";
				link.l1.go = "exit";
				break;
			}
			if (PChar.UneasyDelivery == "3")
			{
				dialog.text = "На сегодня работы нет, приходи завтра.";
				link.l1 = "Хорошо, завтра обязательно приду.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Что вам надо?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Приветствую, я " + GetFullName(NPChar) + "! Вам нужна партия отменной корицы или паприки? Или вы хотите прикупить немного живого товара? Быть может, капитан хочет приобрести чёрного или красного дерева высшего качества?";
			link.l1 = "Ничего не нужно. " + GetSexPhrase("Я привёз", "Я привезла") + " вам партию фарфора от Ричарда Тилльманна.";
			link.l1.go = "2";
		break;

		case "2":
			// Gregg: путаница с Толстяком уместна только после пролога - там ГГ и правда
			// привёл его люггер. Через трактирщика Преноль просто ждёт человека от Тилльманна.
			if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa"))
			{
				dialog.text = "А-а! Так это вас Ричард прислал?";
				link.l1 = "Капитан " + GetFullName(PChar) + ". Вот письмо Ричарда Тилльманна, в нём он всё вам объясняет.";
			}
			else
			{
				dialog.text = "А-а! Вы капитан Томми по прозвищу Толстяк, верно?";
				link.l1 = "Нет, я капитан " + GetFullName(PChar) + ". Я теперь вместо Томми. Вот письмо Ричарда Тилльманна, в нём он всё вам объясняет.";
			}
			link.l1.go = "3";
		break;

		case "3":
			TakeNItems(PChar, "RichardTillmanNotice", -1);
			// Gregg: похвала за убитого Толстяка уместна только после пролога.
			// Через трактирщика ГГ Толстяка в глаза не видел.
			if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa"))
			{
				dialog.text = "...(читает письмо)... Так-так. Ричард пишет, что на вас можно положиться. Что ж, посмотрим.";
			}
			else
			{
				dialog.text = "...(читает письмо)... Вот это да! Ричард говорит, что вы убили самого Толстяка. Молодец, снимаю шляпу.";
			}
			link.l1 = "Послушайте, месье Преноль, быть может, поговорим о деле? Вы ждёте свой фарфор, я жду свою награду. Давайте наконец покончим с этим.";
			link.l1.go = "4";
		break;

		case "4":
			AddMoneyToCharacter(PChar, 6000);
			TakeNItems(PChar, "UneasyDeliveryPorcelain", -25);
			dialog.text = "Ах да, вот ваши 6 000. Извините, больше за доставку я никогда и никому не платил.";
			link.l1 = "Мы договаривались о семи тысячах. Шесть меня никак не устраивают.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Мы с вами ни о чём не договаривались, месье " + GetHeroLastName() + ". Это Тилльманн вам соврал, сказав, что я заплачу семь тысяч. Если вам угодно, требуйте вашу тысячу у Тилльманна. У меня больше денег нет.";
			link.l1 = "Нет, ты заплатишь мне, " + NPChar.lastname + "! Меня предупреждали, что ты постоянно пытаешься одурачить тех, с кем работаешь. Со мной это не пройдёт. Немедленно отдавай тысячу или... или я за себя не отвечаю, " + NPChar.lastname + ". Я предупредил" + GetSexPhrase("", "а") + ".";
			link.l1.go = "6";
		break;

		case "6":
			AddMoneyToCharacter(PChar, 1000);
			dialog.text = "Ну хорошо, хорошо, вот твоя тысяча. Успокоил" + GetSexPhrase("ся", "ась") + "?";
			link.l1 = "Эх, врезать бы тебе пару раз, чтоб неповадно было.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Надо же как-то выживать. Часто приходится работать с теми, кто не может или не хочет бороться за свои деньги. Сложно не воспользоваться случаем и не попытаться нагреть кого-нибудь на пару тысяч. Но вы мне нравитесь! Умеете добиваться своего. И у меня есть для вас работа. Вернее, будет работа.";
			link.l1 = "Только если попытаешься ещё раз меня...";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "...(перебивая)... Нет, конечно нет, больше никогда. Заходите в течение недели и получите отличное задание. Придёте?";
			link.l1 = "Так и быть, буду заходить.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Вот и славно, вот и хорошо! Буду ждать вас.";
			link.l1 = "Счастливо оставаться.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryTimer");
		break;

		case "10":
			dialog.text = PChar.name + ", это бандиты, они хотят убить меня, помоги!";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryPrenolExit");
		break;

		case "11":
			dialog.text = "Ты уби" + GetSexPhrase("л", "ла") + " их, " + PChar.name + "? Убил" + GetSexPhrase("", "а") + " всех?";
			link.l1 = "Да, бандиты мертвы.";
			link.l1.go = "12";
		break;

		case "12":
			AddMoneyToCharacter(PChar, 5000);
			dialog.text = "Слава богу! Вот держи 5000 пиастров за помощь. Если бы не ты, меня бы здесь уже не было.";
			link.l1 = "Так это и была та работа, которую ты хотел мне поручить?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Нет, нет, я хотел попросить доставить груз. Но тут появились эти отморозки и начали требовать мои сбережения. Я знал, что ты придёшь ко мне, и специально тянул время, дожидаясь твоего прихода.";
			link.l1 = "Мог бы хотя бы остаться и помочь мне! Двое против троих куда лучше, чем в одиночку.";
			link.l1.go = "14";
		break;

		case "14":
			if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa"))
			{
				dialog.text = "Я совсем не умею держать шпагу в руках, " + PChar.name + ". А ты драться умеешь, это по тебе сразу видно, и я был уверен, что ты справишься с теми тремя. Спасибо.";
			}
			else
			{
				dialog.text = "Я совсем не умею держать шпагу в руках, " + PChar.name + ". А ты драться умеешь - после Толстяка в этом никто не усомнится, и я был уверен, что ты справишься с теми тремя. Спасибо.";
			}
			link.l1 = "Да не за что. Ты заплатил мне, я " + GetSexPhrase("доволен", "довольна") + ". А как насчёт работы? Тебе какой-то груз надо было доставить?";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Доставку груза я поручу другому человеку, а вот тебе хочу предложить заказ. Я знаю, кто подослал тех троих, и хочу с ним поквитаться. Это мой давний соперник, который не раз пытался мне насолить. Я хочу, чтобы ты его уби" + GetSexPhrase("л", "ла") + ". Соглас" + GetSexPhrase("ен", "на") + "?";
			link.l1 = "Сколько заплатишь?";
			link.l1.go = "16";
		break;

		// Gregg: в К:ПС обещалось 20 000. Суммы квеста урезаны на порядок - см. шапку
		// uneasydelivery.c; здесь то же деление, иначе обещание Преноля перекрывало бы
		// весь долг, о котором позже говорит сам Шон.
		case "16":
			dialog.text = "В сундуке его дома найдёшь 15 000 пиастров и коллекцию уникального оружия. Он начал собирать её ещё в молодости. Ну что, соглас" + GetSexPhrase("ен", "на") + "?";
			link.l1 = "Отлично. Его имя и где он живёт.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Его имя Шон Стабловски, он живёт на острове Антигуа. Как найдёшь - без разговоров убей.";
			link.l1 = "Хорошо, я понял" + GetSexPhrase("", "а") + ".";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Вот и славно, вот и хорошо! Я буду ждать тебя в своём доме. Счастливого пути.";
			link.l1 = "Удачного дня, Жак.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryJacquePrenolBackToHouse");
		break;

		// Gregg: ГГ пойман на воровстве. Местный короткий вариант блока - ветки К:ПС
		// с откупом опирались на GetStealMoneyLeaderShip/GetStealMoneyLuck, которых
		// в CSP нет. Образец - Graveyard_Watcher_dialog.c.
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
