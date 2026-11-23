// Gregg: Стражник форта и призрак Джо Ньюмена - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			dialog.text = "Чего надо?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(преграждает дорогу к лестнице)... Капитан, вниз сегодня нельзя. Сегодня его ночь, а мы в такие ночи туда не ходим.";
			link.l1 = "Я как раз за этим и приш" + GetSexPhrase("ёл", "ла") + ". Отойдите.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoNewmanToPrison");
		break;

		case "2":
			dialog.text = "...(очень тихо)... За много лет ты " + GetSexPhrase("первый", "первая") + ", кто меня одолел" + GetSexPhrase("", "а") + ". Назовись.";
			link.l1 = "Капитан " + GetHeroLastName() + ".";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Хорошее имя. Что стоишь? Добивай, бой был честный.";
			link.l1 = "Я не за этим приш" + GetSexPhrase("ёл", "ла") + ". Мне нужна твоя помощь.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Помощь. От меня. От того, кто пятьдесят лет не выходил дальше этой стены.";
			link.l1 = "Рауль Эсперанто. Его привезли сюда лет тридцать назад.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "...(долго молчит)... Рауль. Помню. Он просидел здесь месяцев семь, и я видел его дважды. Я тогда решил, что он не дотянет и до зимы: такой был вид.";
			link.l1 = "Он умер?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Он сбежал. Из этой тюрьмы, откуда за пятьдесят лет не сбежал никто.";
			link.l1 = "Значит, он может быть жив.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Он жив. Это я знаю точно. А вот скажу я тебе, где его искать, или нет, зависит от того, зачем он тебе.";
			link.l1 = "Откуда тебе знать, где он? Ты отсюда не выходишь.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Заключённые выходят. И возвращаются. И рассказывают. Так зачем он тебе? Только честно, я услышу разницу.";
			link.l1 = "Он много лет назад сделал вещь, которая убивает. Я хочу её остановить, а без него не сумею.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Честно. Хорошо. Тогда слушай.";
			link.l1 = "Постой. Прежде чем я тебя выслушаю - сколько стражников ты убил?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "...(без обиды)... Ты хочешь знать, чудовище я или нет.";
			link.l1 = "Хочу.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "В первый раз, когда я тут появился, на меня бросились с саблями и мушкетами. Я пробовал объяснить, что не трогаю никого. Меня не слушали. С тех пор всякий, кто спускается сюда в мой день, спускается меня добивать. Раз в год комендант нанимает нового.";
			link.l1 = "И меня ты принял за такого же.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "А кого мне было в тебе увидеть.";
			link.l1 = "А заключённые? Комендант говорит, ты их пугаешь.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Комендант много чего говорит. Заключённые - единственные, кто со мной разговаривает. От них я и узнал про Эсперанто. Он на Кубе.";
			link.l1 = "На Кубе. Точно?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Так сказал один из них лет десять назад. Больше я ничем не помогу.";
			link.l1 = "Этого достаточно. Спасибо.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Раз уж на то пошло, попроси и ты меня о чём-нибудь. Так честнее.";
			link.l1 = "Тогда попрошу. Что тебе нужно?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Объясни коменданту, кто я. Скажи ему то, что я сказал тебе.";
			link.l1 = "Скажу.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "...(помолчав)... Нет. Не надо.";
			link.l1 = "Почему?";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Потому что он не поверит, а тебя запомнит. Своё имя мне уже не вернуть, а твоё портить незачем.";
			link.l1 = "Я всё равно попробую.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Не пробуй. Это и будет мне помощь. Иди, капитан.";
			link.l1 = "Прощай, Джо.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoNewmanExit");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
