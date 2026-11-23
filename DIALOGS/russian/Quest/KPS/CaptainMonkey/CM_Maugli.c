// Gregg: капитан Маугли - квест «Корабль обезьян» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\captainmonkey.c
// Узлы 1-2 - первая встреча в гроте, 3-4 - разговор после безнадёжного боя
// (этих узлов в оригинале не было: там ГГ просто проигрывал и его обирали),
// 5-8 - каюта «Бананового рая» на абордаже в финале.

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
			dialog.text = "Чего уставился?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Эй, приятель, каким ветром тебя сюда занесло? Ну, чего уставился? У тебя такое выражение лица, будто ты в жизни не видел" + GetSexPhrase("", "а") + " говорящую обезьяну.";
			link.l1 = "Признаться честно - не видел. До сих пор не доводилось: ни обезьяны, ни слона... В такое сложно поверить - ни даже жирафа!";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Шутки шутишь? А ведь ты дерзишь капитану Маугли! Сейчас ты узнаешь, что это такое, тысяча гнилых бананов!";
			link.l1 = "?!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyBattleWithCaptainInGrot");
		break;

		// Gregg: сюда попадаем, продержавшись против него сорок пять секунд либо
		// просев до пяти единиц здоровья - что раньше. Оружие в ножны, разговор.
		case "3":
			dialog.text = "Ну что, наигрался? Можешь махать своей железкой хоть до второго пришествия - меня ею не взять. Убери, пока не поранился.";
			link.l1 = "Что ты такое, чёрт побери? Я вкладывал" + GetSexPhrase("", "а") + " в удары всё, что имел, а ты даже не запыхался.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Я? Я капитан Маугли, и меня нельзя убить. Совсем. А ты посиди тут, подумай над своим поведением - мои ребята за тобой присмотрят. Может, поумнеешь.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyReloadInGrot");
		break;

		case "5":
			dialog.text = "Ты первый человек, которого мне предстоит одолеть дважды! Каким образом ты оказал" + GetSexPhrase("ся", "ась") + " здесь, дырявый кокос?";
			link.l1 = "Я убью тебя, Маугли.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Надежды юношей питают! Боюсь, тебя вновь ждёт разочарование.";
			link.l1 = "Я убью тебя, Маугли.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Тебе ни за что не одолеть меня! Ты не понимаешь, с кем имеешь дело.";
			link.l1 = "Понимаю лучше, чем ты думаешь. Твой договор с демоном превратился в горстку пепла. Хорошие новости, не правда ли?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Чёрт возьми, как тебе удалось до него добраться?! Это несколько осложняет дело... Но несмотря ни на что, ты будешь убит" + GetSexPhrase("", "а") + "!";
			link.l1 = "Нет. Это я убью тебя, Маугли.";
			link.l1.go = "exit";
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattleNoParam");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
