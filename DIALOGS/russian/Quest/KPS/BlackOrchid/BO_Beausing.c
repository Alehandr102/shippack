// Gregg: Ян Бозен - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
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
			dialog.text = "Неплохо машете, капитан. Продолжайте в том же духе.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Опять инквизиция прислала своих. Третий раз за год. Никак не уймутся.";
			link.l1 = "Инквизиция? Что вы им сделали?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Ничего. У меня есть вещь, которая им нужна, вот и всё дело. Я поначалу отбивался сам, потом надоело и я нанял Секретаря.";
			link.l1 = "Секретарь мёртв. Я наш" + GetSexPhrase("ёл", "ла") + " его в пещере.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Мёртв. ...(помолчав)... Значит, и на него нашёлся кто-то. Ладно. Спасибо, что убрали этих. Теперь говорите, зачем пришли. Только не врите, я это слышу.";
			link.l1 = "За вами, Бозен. Я работаю на инквизицию, и мне велено доставить вас в Сантьяго.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "...(ставит бокал)... Прямо. Что ж. Один вопрос перед тем, как я вас убью: вы правда думали, что войдёте и уведёте меня?";
			link.l1 = "Проверим.";
			link.l1.go = "exit";
			PChar.RaulEsperanto.BeausingBattle = 1;
			AddDialogExitQuestFunction("RaulEsperantoBattleWithBeausing");
		break;

		case "5":
			dialog.text = "Аргх.";
			link.l1 = "Хватит, Ян. Дальше будет только хуже.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Нет.";
			link.l1 = "Как хотите.";
			link.l1.go = "exit";
			PChar.RaulEsperanto.BeausingBattle = 2;
			AddDialogExitQuestFunction("RaulEsperantoBattleWithBeausing");
		break;

		case "7":
			dialog.text = "Думаете, выиграли? Ничего вы не выиграли.";
			link.l1 = "Опять?";
			link.l1.go = "exit";
			PChar.RaulEsperanto.BeausingBattle = 3;
			AddDialogExitQuestFunction("RaulEsperantoBattleWithBeausing");
		break;

		case "8":
			dialog.text = "Живым не дамся. К бою.";
			link.l1 = "Вы когда-нибудь остановитесь?";
			link.l1.go = "exit";
			PChar.RaulEsperanto.BeausingBattle = 4;
			AddDialogExitQuestFunction("RaulEsperantoBattleWithBeausing");
		break;

		case "9":
			dialog.text = "...(сидит на полу, тяжело дыша)... Всё. Выиграли. Слышите? Выиграли.";
			link.l1 = "Теперь тихо выходим из дома и поднимаемся на борт.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Только не в Сантьяго.";
			link.l1 = "Выбора у вас нет.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Есть. Послушайте меня одну минуту. Им нужен не я. Им нужно Евангелие, которое у меня было. Я отдам его вам, только не везите меня туда.";
			link.l1 = "Мне велено доставить вас, а не книгу.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Вам велено, а им нужно другое. Отдадите меня - они начнут вытягивать из меня, где книга. Долго. А я скажу вам сразу и бесплатно.";
			link.l1 = "Хорошо. Сделаем так: сначала едем за книгой, потом в Сантьяго. Если они скажут, что вы им не нужны, я верну вас на Сан-Мартин.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Идёт. Книга у ростовщика в Виллемстаде.";
			link.l1 = "Тогда в путь.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoToVillemstadBank");
		break;

		case "14":
			dialog.text = "Вот она. Только что забрал у ростовщика. ...(держит осторожно)... Тяжелее, чем помнилась.";
			link.l1 = "Теперь на Кубу. Вы останетесь на корабле, книгу отдам я.";
			link.l1.go = "15";
			TakeNItems(PChar, "RaulEsperanto_Evangelie", 1);
		break;

		case "15":
			dialog.text = "Останусь. Знаете, капитан, я почти жалею, что не увижу его лица.";
			link.l1 = "Посмотрим.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoGetEvangelie");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
