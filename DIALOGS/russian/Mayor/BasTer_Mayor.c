// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Что вы хотели? Спрашивайте.", "Я слушаю вас, что за вопрос?"), "Второй раз за день вы пытаетесь задать ворпос...", "В третий раз за день вы опять пытаетесь задать вопрос...",
                          "Да когда же это кончится?! У меня дел полно по управлению делами города, а ты все вопросы пытаешься задать!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал"+ GetSexPhrase("","а") +"...", "Не сейчас, не место и не время..."), "Да, верно... Но не сейчас, позже...",
                      "Задам, задам... Только позже...", "Извините, " + GetAddress_FormToNPC(NPChar) + "...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.State == "Fr4SoleiRoyal_GoodWork")
			{
				dialog.text = "Я слушаю вас, капитан.";
				link.l1 = "Сударь, генерал-губернатором Тортуги мне поручено сопроводить линейный корабль первого класса 'Солей Руаяль' до острова Доминика. Там меня должна была ожидать эскадра Гваделупы, но её не оказалось на месте. Вы можете как-то прояснить ситуацию?";
				link.l1.go = "Step_F4_1";
			}
			if (pchar.QuestTemp.TohmasPetit_Quest == "return_to_baster")
			{
				link.l2 = "Месье губернатор, Томас Петит и Мадлен Леблан у меня в трюме. Те самые жулики, которые наводили бандитов и грабили на приезжих капитанов в джунглях. Я поймал"+GetSexPhrase("","а")+" его.";
				link.l2.go = "TohmasPetit_Gov_HandOver";
			}
		break;
		//************************** Фр.линкейка, квест №4. Солей Руаяль ************************
		case "Step_F4_1":
            dialog.text = "Дело в том, что я получил распоряжение о выдвижении эскадры к Доминике только вчера - курьер сильно опоздал. Таким образом, я не успел подготовить эскадру. Я полагаю, что вы как раз из Доминики...";
			link.l1 = "Верно. Мне пришлось принять бой с испанской эскадрой некоего Хуана Галено. Он выследил и пытался захватить 'Солей Руаяль'.";
            link.l1.go = "Step_F4_2";
        break;
		case "Step_F4_2":
            dialog.text = "И где сейчас этот Хуан Галено с его эскадрой?";
			link.l1 = "На дне, месье...";
            link.l1.go = "Step_F4_3";
        break;
		case "Step_F4_3":
			if (CheckCompanionInCharacter(pchar, "SoleiRoyalCaptain"))
			{
				dialog.text = "Ха-ха, прекрасная работа! Ну что же, ваша задача выполнена - вы сумели доставить 'Солей Руаяль' до места назначения и с честью справились с непростой ситуацией. Прекрасная работа, капитан. Сейчас я принимаю у вас командование 'Солей Руаялем'.";
				link.l1 = "Прекрасно, сударь. Не буду вас задерживать - дела... Прощайте, месье.";
				link.l1.go = "exit";
				RemoveCharacterCompanion(pchar, characterFromId("SoleiRoyalCaptain"));
				pchar.questTemp.State = "Fr4SoleiRoyal_backTortuga";
				AddQuestRecord("Fra_Line_4_SoleiRoyal", "6");
			}
			else
			{
				dialog.text = "Но вы не сумели сохранить 'Солей Руаяль'! Это плохо, судар"+ GetSexPhrase("ь","ыня") +", очень плохо... Теперь вам надлежит явиться к генерал-губернатору Тортуги с докладом о провале вашей миссии. Прощайте.";
				link.l1 = "Прощайте, месье...";
				link.l1.go = "exit";
				pchar.questTemp.State = "Fr4SoleiRoyal_SoleiRoyalSunk";
				AddQuestRecord("Fra_Line_4_SoleiRoyal", "7");
			}
        break;
		//************************** Квестовые штрумы ************************
		case "Cupture_after":
            dialog.text = RandPhraseSimple("Вы уже все забрали. Что вам ещё нужно?", "Неужели осталось ещё что-то, что вы не прихватили?");
            link.l1 = RandPhraseSimple("Осматриваюсь напоследок...", "Проверяю, может забыл"+ GetSexPhrase("","а") +" что забрать...");
            link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
		break;
		//Квест Аскольда, штурм в обличие мумии
        case "Ascold_MummyAttack":
			dialog.text = "А-а-а-а!!!";
			Link.l1 = "Хватит орать, идиот. Я здесь по делу и не причиню тебе вреда.";
			Link.l1.go = "Step_Asc1";
			AfterTownBattle();
        break;
        case "Step_Asc1":
            dialog.text = "Мумия говорит!!! А-а-а-а!!!";
			Link.l1 = "Слушай меня, несчастный трус. Я знаю, что у тебя есть четыре предмета: три хрустальных черепа тольтеков разного цвета и странный индейский предмет, похожий на погремушку. Отдай мне эти предметы и мы уберёмся из города.";
			Link.l1.go = "Step_Asc2";
        break;
        case "Step_Asc2":
            dialog.text = "Они в шкафу слева от меня, в одном из закрытых ящиков. Вот ключ, можешь все забирать...";
			Link.l1 = "Ещё бы ты мне запрещал, червяк...";
			Link.l1.go = "exit";
			NextDiag.TempNode = "Cupture_after";
			Locations[FindLocation(pchar.location)].box1.items.sculMa1 = 1;
            Locations[FindLocation(pchar.location)].box1.items.sculMa2 = 1;
            Locations[FindLocation(pchar.location)].box1.items.sculMa3 = 1;
            Locations[FindLocation(pchar.location)].box1.items.indian22 = 1;
            pchar.questTemp.Ascold = "Ascold_MummyFoundItems";
			chrDisableReloadToLocation = false;
			SetReturn_Gover_Dialog_Exit(NPChar);
        break;
		
		case "TohmasPetit_Gov_HandOver":
			dialog.text = "А, Томас Петит... Помню-помню, на него тут столько кляуз настрочили, что у меня чернил не хватало подписывать. Что ж, похвально, капитан.";
			link.l1 = "Хочу так же сообщить, что ваш комендант пальцем не пошевелил, когда я просил"+GetSexPhrase("","а")+" о помощи! Заявил, что у него нет людей. Пока стража протирала штаны, я вел"+GetSexPhrase("","а")+" изнурительные поиски, нагнал"+GetSexPhrase("","а")+" беглецов в море и едва уговорил"+GetSexPhrase("","а")+" капитана шлюпа выдать их. И всё ради того, чтобы вернуть свои 40 000 пиастров, которые этот гад у меня выудил!";
			link.l1.go = "TohmasPetit_Gov_HandOver_Details";
		break;

		case "TohmasPetit_Gov_HandOver_Details":
			dialog.text = "Коменданта я, разумеется, отчитаю при случае, но строго судить не стану — у него и так дел по горло, испанские каперы то и дело маячат на горизонте, так что ловля рядовых мошенников в его приоритеты не входит. На этом всё, забирайте законные 15 000 пиастров и свободны.";
			link.l1 = "И это всё?! 40 000 пиастров украдено, а мне швыряют пятнадцать тысяч? Да этот плевок со стороны вашей канцелярии даже близко не компенсирует мои потери! Я требую полной компенсации из конфискованного имущества вора!";
			link.l1.go = "TohmasPetit_Gov_HandOver_Value";
		break;

		case "TohmasPetit_Gov_HandOver_Value":
			dialog.text = "Вы слишком много себе позволяете в моем кабинете, капитан! Скажите спасибо, что вам вообще платят. Пятнадцать тысяч, это более чем щедрая награда за головы целой шайки головорезов в джунглях, которые несут прямую угрозу городу. Ваш воришка на их фоне, мелкая сошка. Забирайте золото и покиньте резиденцию.";
			link.l1 = "Рад"+GetSexPhrase("","а")+" был"+GetSexPhrase("","а")+" помочь... правосудию, господин губернатор. Всего хорошего.";
			link.l1.go = "TohmasPetit_Gov_HandOver_Complete";
		break;

		case "TohmasPetit_Gov_HandOver_Complete":
			DialogExit();
			// Запись в журнал и закрытие квеста
			AddQuestRecord("TohmasPetit", "TohmasPetit_GovComplete");
			CloseQuestHeader("TohmasPetit");
			AddDialogExitQuest("TohmasPetit_Gov_Reward");
			pchar.questdata.TohmasPetit = 1;
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

