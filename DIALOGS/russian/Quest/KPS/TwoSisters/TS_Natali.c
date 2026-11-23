// Gregg: Натали Ремис и Ортодоксе Малкин - «Две сестры», ветка Бермуд.
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
			dialog.text = "Чего надо?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
		break;

		// Ссора Малкина с Натали посреди улицы - ГГ подходит на шум
		case "1":
			SetCameraDialogMode(NPChar);
			dialog.text = "...(не замечая подошедшего)... Кому ты нужна, дура? Таких на любом причале десяток, и все сговорчивее.";
			link.l1 = "...(остановиться)...";
			link.l1.go = "2";
		break;

		case "2":
			SetCameraDialogMode(CharacterFromID("Natali Remis"));
			LAi_ActorTurnToCharacter(CharacterFromID("Natali Remis"), CharacterFromID("Ortodokse Malkin"));
			dialog.text = "Натали Ремис: Десяток, значит. А кто в прошлый месяц под дверью ночевал и скулил, чтобы пустили?";
			link.l1 = "...(слушать)...";
			link.l1.go = "3";
		break;

		case "3":
			SetCameraDialogMode(NPChar);
			dialog.text = "Замолчи.";
			link.l1 = "...(слушать)...";
			link.l1.go = "4";
		break;

		case "4":
			SetCameraDialogMode(CharacterFromID("Natali Remis"));
			dialog.text = "Натали Ремис: Не замолчу. Ты же меня отсюда не выпустишь, верно? Ни на корабль, ни в церковь, никуда.";
			link.l1 = "...(слушать)...";
			link.l1.go = "5";
		break;

		case "5":
			SetCameraDialogMode(NPChar);
			dialog.text = "Ты моя. Я за тебя заплатил, и не раз.";
			link.l1 = "...(слушать)...";
			link.l1.go = "6";
		break;

		case "6":
			SetCameraDialogMode(CharacterFromID("Natali Remis"));
			dialog.text = "Натали Ремис: Заплатил он. За вино плати, за меня уже не выйдет.";
			link.l1 = "...(слушать)...";
			link.l1.go = "7";
		break;

		case "7":
			SetCameraDialogMode(NPChar);
			dialog.text = "...(тихо)... Значит, так. Сейчас пойдёшь со мной. Своими ногами.";
			link.l1 = "Отойди от неё.";
			link.l1.go = "8";
		break;

		case "8":
			SetCameraDialogMode(NPChar);
			dialog.text = "...(поворачивается)... А ты кто такой, чтобы мне указывать? Иди мимо, пока цел.";
			link.l1 = "Мимо не пойду.";
			link.l1.go = "9";
			link.l2 = "Я за ней. Она уезжает в Сан-Хуан.";
			link.l2.go = "9";
		break;

		case "9":
			SetCameraDialogMode(NPChar);
			dialog.text = "Ну, значит, обоих и закопаю.";
			link.l1 = "...(взяться за оружие)...";
			link.l1.go = "10";
		break;

		case "10":
			locCameraFollow();
			dialog.text = "...";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersRemisFightWithMalkin");
		break;

		// После драки
		case "11":
			dialog.text = "...(не сразу)... Спасибо. Я думала, он меня всё-таки уведёт.";
			link.l1 = "Он больше никого никуда не уведёт.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "У него трое своих на бриге. Про меня они всё знают. Как только сойдут на берег, тут и начнётся.";
			link.l1 = "Значит, до берега они не дойдут.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "...(смотрит недоверчиво)... Вы за меня полезете на чужой корабль? Зачем?";
			link.l1 = "Мне нужны четыре женщины для Сан-Хуана. Ты четвёртая.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Вот оно что. А я уж подумала невесть что. ...(усмехается)... Сан-Хуан так Сан-Хуан, здесь меня всё равно ничего не держит.";
			link.l1 = "Тогда собирайся.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Соберусь. Только сперва бриг, иначе я и до пристани не дойду.";
			link.l1 = "Что там за люди?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Трое. Малкин их с каторги вытащил, и с тех пор они за него хоть в петлю. Здоровые, как быки, и такие же тупые.";
			link.l1 = "Где стоит бриг?";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Сразу за молом, шлюпка у камней. Один сидит в каюте, второй в кают-компании, третий в трюме - они и на суше врозь не ходят.";
			link.l1 = "Жди меня здесь.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersRemisToShip");
		break;

		// Матросы Малкина на бриге
		case "22":
			dialog.text = "...(поднимается)... Ты не наш. Наши стучат.";
			link.l1 = "Твой хозяин мёртв.";
			link.l1.go = "25";
		break;

		case "23":
			dialog.text = "Слышал внизу шум? Вот и я слышал. ...(берётся за нож)...";
			link.l1 = "Внизу было тише.";
			link.l1.go = "25";
		break;

		case "24":
			dialog.text = "...(щурится в темноте)... Двоих уже, значит. Я третий и последний, дальше некому.";
			link.l1 = "Последний и есть.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "...";
			link.l1 = "...";
			link.l1.go = "exit";
			if (NPChar.id == "Malkin Sailor 1")
			{
				AddDialogExitQuestFunction("TwoSistersRemisFightWithFirstSailor");
			}
			if (NPChar.id == "Malkin Sailor 2")
			{
				AddDialogExitQuestFunction("TwoSistersRemisFightWithSecondSailor");
			}
			if (NPChar.id == "Malkin Sailor 3")
			{
				AddDialogExitQuestFunction("TwoSistersRemisFightWithThirdSailor");
			}
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
