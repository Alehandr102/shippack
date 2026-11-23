// Gregg: Ален Баррет - «Чёрная Орхидея», глава 7 «Маленький ублюдок».
// Логика - Program\Quests\reaction_functions\blackorchid.c.
// После главы 7 Баррет остаётся привратником Джекмена насовсем: вход в резиденцию
// платный, разово или на месяц. Так в оригинале К:ПС.

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
			if (Locations[FindLocation("Pirates_town")].reload.l3.disable == 1)
			{
				dialog.text = "К Джекмену? Расценки ты знаешь.";
				link.l1 = "Знаю. Плачу.";
				link.l1.go = "Open_1";
				link.l2 = "В другой раз.";
				link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Дверь открыта, иди.";
				link.l1 = "Иду.";
				link.l1.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;

		case "Open_1":
			dialog.text = "Один раз или на месяц?";
			link.l1 = "Один раз. Тысяча.";
			link.l1.go = "Open_2";
			link.l2 = "На месяц. Пятнадцать тысяч.";
			link.l2.go = "Open_4";
			link.l3 = "Ни то, ни другое.";
			link.l3.go = "exit";
		break;

		case "Open_2":
			if (sti(PChar.money) >= BO_SM_VISIT_PRICE)
			{
				dialog.text = "Дверь открыта. Один раз.";
				link.l1 = "Учтено.";
				link.l1.go = "Open_3";
			}
			else
			{
				dialog.text = "Пусто у тебя. Приходи с деньгами.";
				link.l1 = "Приду.";
				link.l1.go = "exit";
			}
		break;

		case "Open_3":
			BO_BermudesResidenceOpen(BO_SM_VISIT_PRICE, "o");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Open_4":
			if (sti(PChar.money) >= BO_SM_MONTH_PRICE)
			{
				dialog.text = "Месяц твой. Считаю с сегодняшнего дня.";
				link.l1 = "Учтено.";
				link.l1.go = "Open_5";
			}
			else
			{
				dialog.text = "Пусто у тебя. Приходи с деньгами.";
				link.l1 = "Приду.";
				link.l1.go = "exit";
			}
		break;

		case "Open_5":
			BO_BermudesResidenceOpen(BO_SM_MONTH_PRICE, "m");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "1":
			dialog.text = "Чего надо, щенок?";
			link.l1 = "Выбирай слова, Баррет.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Ты знаешь, кто я? Ален Баррет. Сорок лет на этих водах.";
			link.l1 = "Сорок лет - это возраст, а не довод.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(усмехается)... Довод у меня на поясе. Хочешь посмотреть?";
			link.l1 = "Мне нужен Джекмен, а не твой пояс.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Джекмен занят. Для тебя - всегда занят.";
			link.l1 = "Это он решил или ты?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Пока я стою у двери - я. Иди отсюда, щенок.";
			link.l1 = "Ещё раз назовёшь щенком - пожалеешь.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Щенок.";
			link.l1 = "...(берётся за клинок)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelBattleWithBarret");
		break;

		case "7":
			dialog.text = "...(сидит, привалившись к остову)... Хватит. Хватит, слышишь.";
			link.l1 = "Где ты научился так драться?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Сорок лет, парень. Я же сказал. За эти сорок лет меня сбивали трижды, и ты третий. Врать не буду.";
			link.l1 = "Мне нужен Джекмен. Срочно.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Могу устроить. За деньги.";
			link.l1 = "Платить за разговор?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "За тишину. После того выстрела он к ней пристрастился. А я стою и считаю.";
			link.l1 = "Про выстрел я слыш" + GetSexPhrase("ал", "ала") + " в таверне. Стрелка нашли?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "...(смотрит в сторону)... Тот, кого больше нет. Дальше не спрашивай.";
			link.l1 = "Сколько.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Тысяча за один раз. Или пятнадцать, и месяц ходишь как к себе.";
			link.l1 = "Половина этого оседает у тебя, верно?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "...(без тени смущения)... Меньше. Но оседает.";
			link.l1 = "Хоть не врёшь.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Врать мне незачем, дверь-то одна. Ну так что, платишь?";
			link.l1 = "Подумаю. Найду тебя.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelToJackman");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
