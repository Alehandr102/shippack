// Gregg: Посыльный и Полл Стронгфилд - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "...";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(с порога, срывающимся голосом)... Корабль! Корабль на горизонте!";
			link.l1 = "Сначала отдышитесь.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Я был в лавке. Вдруг потемнело среди дня, на улице закричали. Выбежал - на море шторм, которого минуту назад не было, и к колонии что-то идёт.";
			link.l1 = "Что именно идёт?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Сам я не разглядел. Кричали, что чёрный мановар, огромный и без единого огня. Мне велели предупредить вас.";
			link.l1 = GetSexPhrase("Понял", "Поняла") + ". Спасибо.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyTalkWithKendart");
		break;

		case "4":
			dialog.text = "Капитан. И господину Кендарту тоже. От всей души благодарю вас обоих.";
			link.l1 = "Кто ты такой?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Тридцать лет назад я ходил на каравелле. Была жена, дети, заработок, и я был доволен всем. В одно плавание нас двое суток носило штормом, а на третьи мы вышли к судну, которого там быть не могло. Пустое, в дрейфе. Я поднялся на борт первым.";
			link.l1 = "И нашли часы.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Нашёл. Песок в них сыпался и не кончался - ни сверху не убывало, ни снизу не прибывало. Я смотрел на них, не отрываясь, потом взял в руки. Дальше боль, и я очнулся в той же каюте, только меня уже никто не видел.";
			link.l1 = "...(молчит)...";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Пока часов никто не трогал, я был просто ничем. Но стоило чужой руке лечь на колбу, во мне просыпалось это. И я убивал. Всех, кто был на борту, до последнего человека.";
			link.l1 = "Тридцать два на реях. Это были вы.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Я. И те, кого стреляли и сваливали в трюм, тоже я. Меня не видели, поэтому и не сопротивлялись.";
			link.l1 = "А когда часы лежали у Кендарта в Сан-Хуане?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Я молился, чтобы их никто не трогал. Если бы кто-то взял их в руки, я вырезал бы весь город и ничего бы с этим не сделал. Вы двое, слава богу, знали, с чем имеете дело.";
			link.l1 = "А этот корабль?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Не знаю. Он появился вместе со мной, когда часы вскрыли, и я на нём такой же гость, как вы.";
			link.l1 = "Тогда уходим отсюда вместе.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Нет. За мной слишком много людей, чтобы я жил дальше. Я прошу вас меня убить.";
			link.l1 = "Вы не виноваты. Это делали не вы.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Руки мои. Помню я тоже сам. Я выбрал, капитан, и выбрал давно - просто до сегодняшнего дня не мог даже этого.";
			link.l1 = "Вы уверены?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Совершенно.";
			if (FindAndDestroyManowarCanShoot()) link.l1 = "...(поднимает пистолет)...";
			else link.l1 = "...(кивает)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyManowarShot");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
