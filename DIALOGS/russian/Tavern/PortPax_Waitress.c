// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Вопросы, " + GetAddress_Form(NPChar) + "?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), ""+ GetSexPhrase("Хм, уж не пристаёте ли вы ко мне, " + GetAddress_Form(NPChar) + "? ","Опять со странными вопросами? Девушка, выпили бы вы рому, что ли...") +"", "В течение этого дня вы уже третий раз говорите о каком-то вопросе..."+ GetSexPhrase(" Это знаки внимания?","") +"",
                          "Опять вопросы будете задавать, " + GetAddress_Form(NPChar) + "?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал"+ GetSexPhrase("","а") +"...", "Сейчас мне не о чем говорить."), "Да нет, что ты, красавица...",
                      "Никак нет, дорогуша, склероз...", "Да нет, какие вопросы...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// Gregg: «Чёрная Орхидея» (перенос из К:ПС) - у официантки ключ от сундука
			// Брайана Джорджа. Атрибута может не быть вовсе, потому проверка отдельная:
			// короткого замыкания в Storm нет, объединять условия в одно нельзя.
			if (CheckAttribute(PChar, "MysteriousArtefact"))
			{
				if (PChar.MysteriousArtefact == "6")
				{
					link.l2 = "Помнишь, две недели назад у вас в таверне была драка?";
					link.l2.go = "MysteriousArtefact_1";
				}
			}
		break;

		//==> Gregg: Чёрная Орхидея - таинственный артефакт
		case "MysteriousArtefact_1":
			dialog.text = "Мне бы очень не хотелось это вспоминать, капитан. Это было ужасно.";
			link.l1 = "Понимаю, перейду сразу к делу. Один человек по имени Брайан Джордж потерял в той драке ключ. Ты его не находила?";
			link.l1.go = "MysteriousArtefact_2";
		break;

		case "MysteriousArtefact_2":
			dialog.text = "Находила. Я пряталась в углу, и вдруг прямо рядом со мной упал ключ. Я подобрала его, а когда драка кончилась, спросила каждого посетителя: не ваш ли? Никто ключа не терял, так он у меня и остался.";
			link.l1 = "А Брайан Джордж? Он ведь тоже там был, почему ты его не спросила?";
			link.l1.go = "MysteriousArtefact_3";
		break;

		case "MysteriousArtefact_3":
			dialog.text = "Так его хозяин сразу вытолкал за дверь. Да я и предположить не могла, что такой красивый ключик может принадлежать этому грязному пьянице.";
			link.l1 = "Ключ сейчас у тебя?";
			link.l1.go = "MysteriousArtefact_4";
		break;

		case "MysteriousArtefact_4":
			dialog.text = "У меня. Но отдам я его только законному владельцу - Брайану Джорджу.";
			link.l1 = "Послушайте, Брайан Джордж мёртв, его убили несколько дней назад на Бермудах. В сундуке, который отпирает этот ключ, лежит моя вещь - он должен был мне её вернуть. Она дорога мне как память. Прошу вас, отдайте ключ. Моя судьба в ваших руках.";
			link.l1.go = "MysteriousArtefact_5";

			if (sti(PChar.money) >= 5000)
			{
				link.l2 = "Быть может, я смогу этот ключик купить? Скажем, за пять тысяч пиастров. Что скажете?";
				link.l2.go = "MysteriousArtefact_6";
			}

			link.l3 = "Слушай сюда, курица. Не отдашь ключ - будут у тебя серьёзные неприятности, это я обещаю.";
			link.l3.go = "MysteriousArtefact_7";
		break;

		case "MysteriousArtefact_5":
			PChar.MysteriousArtefact.WaitressTalk = 1;
			dialog.text = "Хорошо, возьмите. Он ваш.";
			link.l1 = "Премного благодарен.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactFirstKeyGive");
		break;

		case "MysteriousArtefact_6":
			PChar.MysteriousArtefact.WaitressTalk = 2;
			AddMoneyToCharacter(PChar, -5000);
			dialog.text = "Ключ ваш.";
			link.l1 = "Премного благодарен.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactFirstKeyGive");
		break;

		case "MysteriousArtefact_7":
			dialog.text = "Будете угрожать - позову стражу.";
			link.l1 = "Зови. Знаешь, я в хороших отношениях с губернатором вашего города и несколько дней назад спас ему жизнь. Он не допустит, чтобы его спасителя бросили в тюрьму. А вот тебя я упрячу в самую зловонную камеру до конца твоих дней. Красивый ключ, который найдут у тебя при обыске, отдадут мне - никто не поверит, что он твой. Выбор за тобой.";
			link.l1.go = "MysteriousArtefact_8";
		break;

		case "MysteriousArtefact_8":
			PChar.MysteriousArtefact.WaitressTalk = 3;
			dialog.text = "...(побледнела)... Возьмите. Он ваш.";
			link.l1 = "Премного благодарен. И запомните: показывая зубки, можно нажить немало неприятностей. Счастливо оставаться.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactFirstKeyGive");
		break;
		//<== Gregg: Чёрная Орхидея

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
