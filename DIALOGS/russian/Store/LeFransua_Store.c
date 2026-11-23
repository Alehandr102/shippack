// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{

	switch (Dialog.CurrentNode)
	{
		case "quests":
			// Gregg: «Чёрная Орхидея», глава 7 - провиант для фрегата и поиски грузчиков
			if (CheckAttribute(PChar, "SmallMongrel"))
			{
				if (PChar.SmallMongrel == "7")
				{
					link.l30 = "Вопрос покажется странным, но не заходил ли в вашу бухту тяжёлый фрегат?";
					link.l30.go = "BO_SM_ST_1";
				}
				if (PChar.SmallMongrel == "12")
				{
					link.l31 = "Я ищу грузчиков, Фрэнка и Дженсона. Они...";
					link.l31.go = "BO_SM_ST_3";
				}
			}
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Некоторое время тому назад, находясь у меня в магазине, вы пытались задать какой-то вопрос...", "За сегодня третий вопрос. Мне торговать надо, а не пустые разговоры вести...",
                          "Опять вопросы? Может лучше торговлей займёмся?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я "+ GetSexPhrase("передумал","передумала") +"...", "Сейчас мне не о чем говорить."), "Да, точно, пытал"+ GetSexPhrase("cя","ась") +"... Находясь в магазине...",
                      "Да уж, действительно в третий раз...", "Хм, может и поторгуем...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.piratesLine == "KillLoy_toSeek")
			{
				link.l1 = "Простите, у вас Эдвард Лоу приобретал корабль?";
				link.l1.go = "PL_Q3_1";
			}
		break;
		//пиратка, квест №3, поиски Лоу
		case "PL_Q3_1":
			dialog.text = NPCStringReactionRepeat("У меня?! Вы меня с кем-то путаете, я торговец, но кораблями... Кораблями не торгую! Вам, "+ GetSexPhrase("молодой человек","девушка") +", на верфь нужно обратиться, к корабелам. Да, да к ним, а ко мне за всем остальным!",
				"Вы уже спрашивали. Нет, не приобретал.",
				"Нет, "+ GetSexPhrase("молодой человек","юная леди") +", нет...",
                "Ох, ну и зануда же вы!!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Да, но в вашем городишке нет верфи!",
				"Понятно...",
                "Ага, ясно...",
				"Хм, уже ухожу...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q3_2", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q3_2":
			dialog.text = "Ничем помочь не могу, по моему желанию верфь у нас не появится. Спросите у кого-нибудь ещё...";
			link.l1 = "Ясно... Ну что же, спасибо и на этом.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "3");
		break;

		// Gregg: «Чёрная Орхидея», глава 7 -->
		case "BO_SM_ST_1":
			dialog.text = "Заходил один. Мои ребята грузили ему провиант, полдня таскали.";
			link.l1 = "Капитан не сказал, куда идёт?";
			link.l1.go = "BO_SM_ST_2";
		break;

		case "BO_SM_ST_2":
			dialog.text = "Он вообще ничего не говорил. Заплатил, не торгуясь, и ушёл. Мне такие покупатели по душе.";
			link.l1 = "Не торгуясь и молча. Спешил, стало быть. Спасибо.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelInLeFransua");
		break;

		case "BO_SM_ST_3":
			dialog.text = "...(перебивая)... Их похитили!";
			link.l1 = "Что?!";
			link.l1.go = "BO_SM_ST_4";
		break;

		case "BO_SM_ST_4":
			dialog.text = "...(давясь смехом)... Похитили и заперли. В подсобке. Чтобы Фрэнк до открытия не сбежал за ромом. Идите, они там оба.";
			link.l1 = "Смешно. Веди к этим твоим похищенным.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelToLeFransuaStoreRoom");
		break;
		// Gregg: <--
	}
	UnloadSegment(NPChar.FileDialog2);
}

