// Gregg: незнакомка из Порт-о-Принса, она же Матильда - трилогия «Чёрная Орхидея»
// (перенос из К:ПС). Логика - Program\Quests\reaction_functions\blackorchid.c
// Узлы 1-7 - подстава в городе, 8-9 - она же, умирающая на складе в Сантьяго.
// Первую часть Меритора игрок получает именно здесь.

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
			dialog.text = "Что вам угодно?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";

			if (PChar.MysteriousArtefact == "8")
			{
				dialog.text = "Капитан, вы наказали этого подонка?";
				link.l1 = "Работаю над этим. Обещаю, я это сделаю.";
				link.l1.go = "exit";
			}
		break;

		case "1":
			dialog.text = "Капитан, прошу вас, помогите мне! Вы поможете? Обещайте, что поможете!";
			link.l1 = "Хорошо, помогу. Что стряслось?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(плачет)... Мне тяжело говорить об этом, капитан. Поверьте, очень тяжело.";
			link.l1 = "Успокойтесь и расскажите по порядку.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(заливается слезами)... Простите. Когда я это вспоминаю, слёз не удержать.";
			link.l1 = "Ничего страшного, я понимаю. Может быть, расскажете завтра? Поверьте, завтра станет легче.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "...(утирает лицо платком)... Нет. Я не успокоюсь, пока этот негодяй не отправится на тот свет. Я должна рассказать сейчас.";
			link.l1 = "Я весь внимание.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Мы с мужем, Луи Бурдо, жили здесь, в Порт-о-Принсе. Добропорядочные горожане, законов не нарушали. Сегодня мы решили отдохнуть за городом - там есть чудесное место, мы с Луи любили проводить там время. Но сегодня... сегодня... ...(снова плачет)...";
			link.l1 = "Может, всё-таки завтра?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Нет, я продолжу. К нам подошёл какой-то пьяный капитан и начал меня оскорблять. Луи, мой храбрый Луи, обнажил шпагу, чтобы вступиться за мою честь, - а тот достал из-за пояса пистолет и выстрелил ему в лоб. А потом... потом надругался надо мной. Самым грязным образом ...(заливается слезами)...";
			link.l1 = "Этот капитан всё ещё за городскими воротами?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Да. Отомстите за моего мужа и за меня, капитан, умоляю. Убейте негодяя! Мы были состоятельными людьми, я щедро заплачу. Я буду ждать вас в церкви.";
			link.l1 = "Хорошо. Мне надо спешить.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactPortPaxWomanHelp");
		break;

		case "8":
			TakeNItems(PChar, "MysteriousArtefact_MeritorChapter_1", 1);
			dialog.text = "...(слабым голосом)... Капитан... возьмите эту вещь... возьмите... она поможет найти Орхидею...";
			link.l1 = "Но как? Что мне с ней делать?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "...(голос совсем слабеет)... Возьмите... когда придёт время... вы поймёте, что нужно... сделать...";
			link.l1 = "Матильда! Матильда!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactMatildaIsDead");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
