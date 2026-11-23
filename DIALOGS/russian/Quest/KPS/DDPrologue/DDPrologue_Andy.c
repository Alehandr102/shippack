// Gregg: Энди Рэйдол - альтернативный пролог «Необитаемый остров» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\ddprologue.c

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

		case "Start_1":
			dialog.text = PChar.name + "! Живой, не иначе!";
			link.l1 = "Как видишь. А ты...";
			link.l1.go = "Start_2";
		break;

		case "Start_2":
			dialog.text = "...(перебивая)... Хитрый ты чёрт! А я знал, знал, что ты не сдохнешь ни в том кромешном аду на палубе, ни в море!";
			link.l1 = "Знаю, вопрос странный, но кто ты?";
			link.l1.go = "Start_3";
		break;

		case "Start_3":
			dialog.text = "Что?! Ты не узнал" + GetSexPhrase("", "а") + " меня?";
			link.l1 = "Нет. После удара головой о пушку я ничего не помню.";
			link.l1.go = "Start_4";
		break;

		case "Start_4":
			dialog.text = GetFullName(NPChar) + "! Верный боцман капитана " + GetMainCharacterNameGen() + ". А память вернётся, капитан, я в этом уверен. Многие кэпы после сильных ударов на короткое время теряли её, но потом всё вспоминали.";
			link.l1 = "Обнадёжил. Давно ты здесь, " + NPChar.name + "?";
			link.l1.go = "Start_5";
		break;

		case "Start_5":
			dialog.text = "Около часа. Этот грот я обнаружил случайно, когда скрывался от головорезов Сайфуса. Если бы я его не нашёл, они бы нашли меня и пришили.";
			link.l1 = "Чёрт, значит, Сайфус уже высадил десант на остров! Надо быть предельно острож... ...(шёпотом)... Тихо! Вы слышите?";
			link.l1.go = "Start_6";
		break;

		case "Start_6":
			dialog.text = "Слышим что, " + PChar.name + "?";
			link.l1 = "Кто-то спускается сюда. Быстро в укрытие, если это люди Сайфуса, они не должны нас заметить, иначе нам крышка.";
			link.l1.go = "Start_7";
		break;

		case "Start_7":
			dialog.text = "...(шёпотом)... Понял, кэп. Джеймс, прячься.";
			link.l1 = "Давайте, быстрее.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameEnemyInGrot");
		break;

		case "Start_8":
			dialog.text = "Фух, выкрутились. Ну а дальше-то что будем делать, кэп? Остров необитаем, а у нас нет корабля.";
			link.l1 = "Для начала отдохнём здесь дня два-три, а там видно будет. Иногда сюда заходят корабли, чтобы пополнить запасы продовольствия и пресной воды. Попробуем договориться с капитанами этих судов, чтобы доставили нас до ближайшего порта.";
			link.l1.go = "Start_9";
		break;

		case "Start_9":
			dialog.text = "А у вас есть чем заплатить, кэп? Наши карманы пусты.";
			link.l1 = "Хм. Это верно, платить нам нечем. А ты обыскивал этот сундук, " + NPChar.name + "?";
			link.l1.go = "Start_10";
		break;

		case "Start_10":
			dialog.text = "А зачем его обыскивать, что я там найду?";
			link.l1 = "Ну, не знаю. Может быть, какой-нибудь капитан припрятал здесь золотишко на чёрный день. Надо проверить.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("StartGameSearchedItems");
		break;

		case "Start_11":
			dialog.text = "Ха! Кэп, а вы были правы! В этом сундуке действительно есть чем поживиться.";
			link.l1 = "Теперь у нас появилась плата за проезд. Осталось только дождаться какого-нибудь корабля. Будем каждый день поочерёдно ходить к побережью и смотреть на морской горизонт.";
			link.l1.go = "Start_12";
		break;

		case "Start_12":
			dialog.text = "Хорошо, капитан, так и сделаем.";
			link.l1 = "А сейчас всем отдыхать. А мне пока нужно сделать одно дело.";
			link.l1.go = "exit_special";
		break;

		// Gregg: тот же приём, что у CSP в MainHero_dialog.c (Exit_Special) - открыть экран
		// распределения характеристик. Флаг ChangePiratesExit подхватывается в INTERFACE\character.c
		// при выходе с экрана и продолжает пролог.
		case "exit_special":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			pchar.SystemInfo.ChangePIRATES = true;
			PChar.StartDamnedDestiny.ChangePiratesExit = true;
			LaunchCharacter(PChar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
