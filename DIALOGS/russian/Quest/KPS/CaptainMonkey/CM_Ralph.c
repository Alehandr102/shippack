// Gregg: Ральф Ваттерсон и его жена Эмили - квест «Корабль обезьян» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\captainmonkey.c
// Узлы 1-3 - спасённый пленник в гроте, 4-6 - встреча супругов у банка,
// 7 - ветка провала, если Аллен успел к деньгам раньше.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sLocation = PChar.CaptainMonkey.BankID;
	sLocation = XI_ConvertString("Colony" + sLocation + "Dat");

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "Что вам угодно?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Капитан, я даже не знаю, как вас благодарить...";
			link.l1 = "Не надо благодарностей. Я сам" + GetSexPhrase("", "а") + " недавно в буквальном смысле был на вашем месте и освободился чудом. Что им было от вас нужно?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Деньги, мои деньги - что же ещё. Они заставили меня написать письмо жене, Эмили, где я прошу её забрать у ростовщика все наши сбережения и передать этому негодяю Гладстонну. Иначе Аллен обещал мне долгую и мучительную смерть. Он, полагаю, уже в пути. Мой дом в " + sLocation + ".";
			link.l1 = "Когда вы отдали ему письмо?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Вчера утром, сэр.";
			link.l1 = "Мы ещё можем успеть. Следуйте за мной на мой корабль.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyToBankColony");
		break;

		case "4":
			dialog.text = "Дорог" + GetSexPhrase("ой", "ая") + ", каким чудом ты здесь оказал" + GetSexPhrase("ся", "ась") + "? Мне вручили это письмо сегодня утром, я как раз собиралась исполнить твою просьбу...";
			link.l1 = "...";
			link.l1.go = "5";
		break;

		case "5":
			SetCameraDialogMode(CharacterFromID("Ralph Vatterson"));
			dialog.text = "О, милая, как я рад тебя видеть! Я вырвался из плена с помощью этого мужественного капитана, его зовут " + GetFullName(PChar) + ". Скажи, дорогая, где тот ''господин'', которому ты должна была отдать наши деньги?";
			link.l1 = "...";
			link.l1.go = "6";
		break;

		case "6":
			SetCameraDialogMode(NPChar);
			dialog.text = "Человек, передавший мне письмо, сказал, что будет ждать меня за городскими воротами.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyFromBankToExitTown");
		break;

		case "7":
			dialog.text = "?!...";
			link.l1 = "Прости, дружище. Я сделал" + GetSexPhrase("", "а") + " всё, что " + GetSexPhrase("мог", "могла") + ".";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyBankTimeIsLeftClosedQuest");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
