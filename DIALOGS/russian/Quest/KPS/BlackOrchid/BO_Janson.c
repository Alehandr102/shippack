// Gregg: Дженсон и Фрэнк, грузчики из Ле Франсуа - «Чёрная Орхидея», глава 7 «Маленький ублюдок».
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
			dialog.text = "Здравствуйте, кэп.";
			link.l1 = "Здравствуй.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Кэп! Кэп, стойте! Дело жизни и смерти, честное слово!";
			link.l1 = "Говори.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Фрэнк дурак.";
			link.l1 = "Это я и без тебя вижу. Дальше?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Я не договорил! Фрэнк дурак, залез в подземелье и не вылез. Там скелеты, кэп. Он там третий час.";
			link.l1 = "Три часа. Значит, уже не вылезет.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Вылезет! Я его слышу оттуда, он поёт. Живой он, кэп, живой, только достать его некому.";
			link.l1 = "А полез он туда зачем?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Так по пьяни. Мы утром отгрузились, он и начал. Кэп, я вас прошу. Я на колени встану, если надо.";
			link.l1 = "Не надо. Веди.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelToDungeon");
		break;

		case "6":
			dialog.text = "Хы. Человек. И-ик.";
			link.l1 = "Тебя Фрэнком зовут?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Фрэнк это я. И-ик. А ты кто.";
			link.l1 = GetSexPhrase("Тот, кто пришёл", "Та, кто пришла") + " тебя отсюда вытащить.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Не надо. Тут хорошо. Тихо.";
			link.l1 = "Тут скелеты.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Хы! Скелеты. Я им кости обрублю. И-ик. Все.";
			link.l1 = "...(улыбается)... Ты стоять не можешь.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Могу. Толкни.";
			link.l1 = "Слушай, Фрэнк. Наверху есть ром.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "...(разом умолкает)... Где.";
			link.l1 = "У входа в пещеру. Я остави" + GetSexPhrase("л", "ла") + ".";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "А чего сразу не " + GetSexPhrase("сказал", "сказала") + ". И-ик.";
			link.l1 = "Так идём или нет?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Идём. Ты " + GetSexPhrase("первый", "первая") + ", тут темно.";
			link.l1 = "К выходу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelFromDungeon");
		break;

		case "14":
			dialog.text = "Кэп, я... Спасибо. Я даже не знаю, чем отдать.";
			link.l1 = "Ничем не надо.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "До вас тут один капитан проходил. Я его тоже просил. Он послушал и пошёл дальше.";
			link.l1 = "Бывает.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Бывает, кэп, только я это запомню. Может, посидите с нами? За возвращение Фрэнка.";
			link.l1 = "Не могу. У меня дело, и оно горит.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Какое дело? Может, поможем.";
			link.l1 = "Вряд ли.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "А вы скажите. Мы должники, а долги я помню.";
			link.l1 = "Ищу тяжёлый фрегат. Он заходил в Ле Франсуа за провиантом и ушёл.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "...(оживляется)... Так провиант на него Фрэнк и грузил! Он и знает.";
			link.l1 = "Фрэнк? Грузчик из Ле Франсуа?";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Мы оба оттуда. В тот день грузил он один, у меня спина.";
			link.l1 = "И он видел, куда он пошёл?";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "Видеть видел, а сказать сегодня не скажет. Наврёт ещё. Приходите в лавку через пару дней, он к тому времени человеком станет.";
			link.l1 = "Я всё-таки спрошу сейчас.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelTalkWithFrank");
		break;

		case "22":
			dialog.text = "Где ром. Ты сказал" + GetSexPhrase("", "а") + " ром.";
			link.l1 = "Кто-то унёс. Куплю новый. Фрэнк, ты грузил провиант на тот фрегат?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "Грузил. И-ик. Мясо грузил. Рыбу грузил. Воду грузил, воду тяжело.";
			link.l1 = "В какой порт он ушёл?";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "Я в таверну ушёл.";
			link.l1 = "Не ты. Фрегат.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "А. Он за ромом.";
			link.l1 = "За каким ромом, Фрэнк?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "За обычным. И-ик. Который в таверне. Хороший ром.";
			link.l1 = "...(про себя: последняя попытка)... Куда. Пошёл. Фрегат.";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "В порт. Корабли ходят из порта в порт. Ты не знал" + GetSexPhrase("", "а") + ", что ли?";
			link.l1 = GetSexPhrase("Знал", "Знала") + ". Подожду два дня.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelSecondTalkWithJanson");
		break;

		case "28":
			dialog.text = "Ну как, узнали?";
			link.l1 = "Нет. Он рассказал мне про ром.";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "Я говорил. Приходите в лавку Ле Франсуа через два дня. И рома с собой возьмите, бутылки три.";
			link.l1 = "Я его из пещеры вытащил" + GetSexPhrase("", "а") + ", а он с меня ром требует?";
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "Не требует, это я вперёд говорю. Я ему растолкую, кто вы " + GetSexPhrase("такой", "такая") + ", может и не понадобится.";
			link.l1 = "Ладно. Возьму.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "Вот и хорошо. Через два дня, кэп.";
			link.l1 = "Тогда до встречи, Фрэнк.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelLeFransuaStoreTimer");
		break;

		case "32":
			dialog.text = "...(поднимается, не глядя в глаза)... Капитан. Дженсон мне всё рассказал.";
			link.l1 = "Я не за извинениями.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "Я всё равно скажу. Пока трезвый. Через час опять не вспомню, как вас звать.";
			link.l1 = "Забыли. Мне нужен ответ на один вопрос.";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "Знаю какой. Фрегат ушёл в Пуэрто-Принсипе. Я с их матросом разговорился, пока грузили, он и сказал.";
			link.l1 = "Уверен?";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "Уверен, капитан. Вы мне жизнь спасли, я бы не стал врать.";
			link.l1 = "Спасибо, Фрэнк.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelToPuertoPrincipe");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
