// Gregg: Вики Хендерсон и её сестра Диана - «Две сестры», ветка Мартиники.
// Логика - Program\Quests\reaction_functions\twosisters.c.

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
			dialog.text = "Кольцо принесли?";
			link.l1 = "Пока нет.";
			link.l1.go = "exit";
		break;

		// Знакомство в борделе Форт-де-Франса
		case "1":
			dialog.text = "Капитан ищет, чем себя занять?";
			link.l1 = "Капитан ищет женщину, готовую переехать.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(усмехается)... Переехать. Обычно просят другого. И куда же?";
			link.l1 = "Сан-Хуан, Пуэрто-Рико. Там открывают заведение, и нужны женщины.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Сан-Хуан. ...(помолчав)... А почему я? Тут таких полон дом.";
			link.l1 = "Мне сказали, ты лучшая на Мартинике. Врали?";
			link.l1.go = "4";
			link.l2 = "Потому что ты первая, кто со мной заговорил.";
			link.l2.go = "4";
		break;

		case "4":
			dialog.text = "Не врали. ...(без улыбки)... Хорошо, я поеду. Но не даром и не сразу.";
			link.l1 = "Сколько?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Не деньгами. У меня есть дело, которое я сама сделать не могу.";
			link.l1 = "Говори.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Убейте мою сестру. Диану Хендерсон, капитана бригантины «Амазонка». Она стоит в Пуэрто-Принсипе, на Кубе.";
			link.l1 = "...(помолчать)... Сестру?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Сестру. Родную, старшую, единственную.";
			link.l1 = "Причина есть?";
			link.l1.go = "8";
			link.l2 = "Мне довольно и того, что ты сказала. Но я хочу знать почему.";
			link.l2.go = "8";
		break;

		case "8":
			dialog.text = "Есть. Только рассказывать её я не обязана.";
			link.l1 = "Обязана. Иначе я не пойду.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "...(после паузы)... Мы приехали сюда вдвоём, к родне. Родни не оказалось. Деньги были у неё, корабль купила она.";
			link.l1 = "А ты?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "А я оказалась здесь. Два года она давала мне денег и каждый раз говорила, как ей за меня стыдно. Потом перестала давать. Стыдиться не перестала.";
			link.l1 = "И за это её убивать?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Пока она жива, я никуда отсюда не денусь. Она найдёт меня и в Сан-Хуане, и напишет всем, кому сможет, кто я такая.";
			link.l1 = "Она твоя сестра.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Двадцать пять лет была. ...(ровно)... А теперь она просто женщина, у которой корабль, деньги и хорошее имя.";
			link.l1 = "...(молчать)...";
			link.l1.go = "13";
		break;

		case "13":
			LAi_Fade("", "");
			dialog.text = "Я хочу, чтобы она была мертва. Это единственная цена, и другой не будет.";
			link.l1 = "Ты уверена?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Уверена.";
			link.l1 = "Как я тебе это докажу?";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "У неё на левой руке фамильное кольцо. Снимете и принесёте мне.";
			link.l1 = "Хорошо.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Тогда до встречи, капитан. Я никуда не спешу.";
			link.l1 = "...(уйти)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersVikiQuestStart");
		break;

		// Диана в Пуэрто-Принсипе, после драки с наёмником
		case "17":
			dialog.text = "...(вкладывая клинок в ножны)... Не самое удачное место для прогулок. Кто вы?";
			link.l1 = "Я ищу Диану Хендерсон.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Нашли. Дальше.";
			link.l1 = "Меня прислала твоя сестра.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "...(без удивления)... Опять деньги?";
			link.l1 = "Нет.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Тогда что? ...(смотрит внимательнее)... Впрочем, догадываюсь. Вы за этим и стоите с оружием.";
			link.l1 = "Она просила тебя убить.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "Просила. ...(без выражения)... Я ждала от неё пакости, но не такой.";
			link.l1 = "Она говорит, ты не оставила ей выбора.";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "Я платила за неё два года. Я предлагала ей место на своём корабле, и не одно.";
			link.l1 = "И что она?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "Привыкла жить на чужое. Когда я перестала давать, оказалось, что во всём виновата я.";
			link.l1 = "Ты могла бы поговорить с ней.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "Я говорила. Каждый раз. ...(усмехается)... Вы, я вижу, тоже пробовали.";
			link.l1 = "Я не хочу тебя убивать.";
			link.l1.go = "25";
			link.l2 = "Мне заплатили за твою смерть.";
			link.l2.go = "25";
		break;

		case "25":
			dialog.text = "Хотите вы или нет, значения уже не имеет. Вы пришли сюда с этим, и уйти просто так я вам не дам.";
			link.l1 = "Тогда закончим.";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "Закончим.";
			link.l1 = "...(обнажить клинок)...";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "...";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersVikiFirstFightWithDiana");
		break;

		// Диана сбита с ног
		case "28":
			dialog.text = "...(тяжело дыша)... Ну?";
			link.l1 = "Встань.";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "Почему не добили?";
			link.l1 = "Отдай кольцо, и я скажу ей, что дело сделано.";
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "...(медленно)... То есть соврёте ей. И она успокоится, а я останусь жива.";
			link.l1 = "Так и будет.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "Отдать фамильное кольцо, чтобы оно оказалось у неё? Нет.";
			link.l1 = "Это твоя жизнь против куска золота.";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "Значит, против. ...(поднимает клинок)... Кольцо вы снимете с мёртвой.";
			link.l1 = "...(поднять оружие)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersVikiNextFightWithDiana");
		break;

		// Вики принимает кольцо
		case "33":
			dialog.text = "Принесли?";
			link.l1 = "...(отдать кольцо)...";
			link.l1.go = "34";
		break;

		case "34":
			TakeNItems(PChar, "TwoSisters_Ring", -1);
			dialog.text = "...(долго смотрит на кольцо)... Значит, всё.";
			link.l1 = "Ты едешь в Сан-Хуан.";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "Еду. ...(прячет кольцо)... И больше об этом ни слова, капитан. Никогда.";
			link.l1 = "Идём на корабль.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersVikiQuestComplete");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
