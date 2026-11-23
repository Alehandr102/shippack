
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "First time":
			if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY)
			{
					dialog.text = RandPhraseSimple("Братья, в Инквизиции враг!!!", "Поднимайте тревогу, братья!!!");
					link.l1 = "Да уж, враги Инквизиции не дремлют...";
					link.l1.go = "fight";
			}
			else
			{
				dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Здравствуй, "+ GetSexPhrase("сын мой","дочь моя") +". Какими судьбами у нас?..", "Рад видеть тебя в нашей обители, "+ GetSexPhrase("сын мой","дочь моя") +". Зачем пожаловал"+ GetSexPhrase("","а") +"?"),
							""+ GetSexPhrase("Сын мой","Дочь моя") +", мы уже говорили с тобой сегодня. Тебе нужно что-то ещё?", ""+ GetSexPhrase("Сын мой","Дочь моя") +", послушников нашего ордена не отличает терпение. Ещё раз тебя спрашиваю: тебе что-нибудь нужно?",
							RandPhraseSimple("Негоже находится в праздности самому и отвлекать других от работы. Более я не произнесу ни слова...", ""+ GetSexPhrase("Сын мой","Дочь моя") +", я не желаю вести бессмысленные разговоры. Тебе я не скажу более ни слова."), "block", 1, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Да так, заскочил"+ GetSexPhrase("","а") +" по делам, святой отец...", "Все по делам, падре. Тружусь, аки пчела - "+ GetSexPhrase("весь","вся") +" в заботах..."),
							"Да нет, святой отец...", "Нет, падре, просто хотел"+ GetSexPhrase("","а") +" поболтать...", RandPhraseSimple("Хм...", "Ну, как знаете, святой отец..."), npchar, Dialog.CurrentNode);
				link.l1.go = DialogGoNodeRepeat("exit", "none", "none", "NoMoreTalkExit", npchar, Dialog.CurrentNode);

				// Gregg: --> "Помощь святой инквизиции" (перенос из К:ПС)
				// Выдача перебивает приветствие намеренно: это завязка, а не пункт меню.
				if (!CheckAttribute(pchar, "HelpSacredInquisition"))
				{
					dialog.text = "Не могу поверить: в городе недавно объявлялся капитан - злостный еретик! Знали бы вы, как этим обеспокоена Святая Инквизиция.";
					link.l1 = "Хм... И что же он такого сделал?";
					link.l1.go = "HelpSacredInquisition_1";
					link.l2 = "Извините, мне некогда.";
					link.l2.go = "exit";
					break;
				}
				if (pchar.HelpSacredInquisition == "Good")
				{
					link.l2 = "Натан Рич понёс заслуженное наказание, святой отец. И получил бесплатный билет в ад.";
					link.l2.go = "HelpSacredInquisition_6";
				}
				// Gregg: <-- "Помощь святой инквизиции"
				if (CheckAttribute(pchar,"GiantEvilSkeleton") && CheckAttribute(pchar,"MalteseInfo") && !CheckAttribute(pchar,"MalteseOrder"))
				{
					link.l2 = "Святой отец, я хотел"+ GetSexPhrase("","а") +" сообщить вам о том, что я посетил"+ GetSexPhrase("","а") +" риф Скелета и уничтожил"+ GetSexPhrase("","а") +" находящуюся там нежить. Может мне полагается какая-нибудь награда?";
					link.l2.go = "Maltese";
					if (CheckAttribute(pchar,"MalteseOrderOnHold"))
					{
						link.l2 = "Я хочу снова поговорить о рифе Скелета.";
						link.l2.go = "Maltese_5";
					}
				}
			}
			if (CheckAttribute(npchar, "protector.CheckAlways")) //гарды на камерах
			{
				if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY)
				{
					dialog.text = "Чёрт возьми, это шпион! Держи его!!!";
					link.l1 = "Каррамба!!";
					link.l1.go = "fight";
					StartIncquisitioAttack();
				}
				else
				{
					dialog.text = LinkRandPhrase("Это охраняемая камера, вход сюда категорически запрещён!!!", "Даже и не думай сюда входить - карается смертью.", "Один шаг внутрь этой камеры - и тебе конец...");
					link.l1 = RandPhraseSimple("Ясно, солдат.", "Понятно...");
					link.l1.go = "exit";
				}
			}
			if (findsubstr(npchar.id, "Prisoner", 0) != -1) //заключённые
			{
				if (LAi_group_IsActivePlayerAlarm())
				{
					dialog.text = LinkRandPhrase("Хо-хо, до чего же весело смотреть, как 'святоши' отправляются прямиком в ад!", "Господи, неужели хоть кто-то осмелился бросить им вызов!!", "Я уже не жилец на этом свете - пытки... Но ты доставил"+ GetSexPhrase("","а") +" мне последнее удовольствие в жизни. Спасибо...");
					link.l1 = RandPhraseSimple("Хех!", "Аргх...");
					link.l1.go = "exit";
					//==> квест №7, базар о Роке Бразильце.
					if (pchar.questTemp.State == "Fr7RockBras_toSeekPlace")
					{
						link.l1 = "Послушай, ты не знаешь здесь такого человека - Рок Бразилец?";
						link.l1.go = "Step_F7_2";
					}
					//<== квест №7, базар о Роке Бразильце.
				}
				else
				{
					dialog.text = LinkRandPhrase("Нет ничего людского у этих 'святош'. Попомни моё слово - гореть им в аду за наши муки...", "Ежедневные истязания и пытки! А ведь я же ни в чём не виноват перед Господом!!!", "Вчера умер мой сосед по камере, они два дня пытали его на дыбе... А сколько мне осталось?..");
					link.l1 = RandPhraseSimple("Хм... Ну, не знаю...", "Что же, понимаю...");
					link.l1.go = "exit";
					//==> квест №7, базар о Роке Бразильце.
					if (pchar.questTemp.State == "Fr7RockBras_toSeekPlace")
					{
						link.l1 = "Послушай, ты не знаешь здесь такого человека - Рок Бразилец?";
						link.l1.go = "Step_F7_1";
					}
					//<== квест №7, базар о Роке Бразильце.
				}
			}
		break;

		// Gregg: --> "Помощь святой инквизиции"
		case "HelpSacredInquisition_1":
			dialog.text = "Как?! Вы не знаете? Он распространял слухи, будто любой человек может обладать силой Бога.";
			link.l1 = "Серьёзное заявление.";
			link.l1.go = "HelpSacredInquisition_2";
		break;

		case "HelpSacredInquisition_2":
			dialog.text = "Вы как раз вовремя. От лица всей Святой Инквизиции я прошу вас о помощи.";
			link.l1 = "Вы хотите...";
			link.l1.go = "HelpSacredInquisition_3";
		break;

		case "HelpSacredInquisition_3":
			dialog.text = "Найдите его и покарайте! Мы простим вам все грехи, на которые придётся пойти ради правосудия. И, разумеется, вознаградим.";
			link.l1 = "Нет. Быть божьим карателем - не по мне.";
			link.l1.go = "HelpSacredInquisition_4";
			link.l2 = "Хорошо. Имя? И есть ли хоть какое-то представление, где его искать?";
			link.l2.go = "HelpSacredInquisition_5";
		break;

		// Gregg: отказ. Ставим "END" - предложение больше не всплывёт.
		case "HelpSacredInquisition_4":
			pchar.HelpSacredInquisition = "END";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "HelpSacredInquisition_5":
			dialog.text = "Если бы я знал, где его искать. Наверняка уплыл куда-то. А зовут его Натан Рич.";
			link.l1 = "Что ж, начинаю поиски. До встречи, святой отец.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("HelpSacredInquisitionStart");
		break;

		case "HelpSacredInquisition_6":
			dialog.text = "Святая Инквизиция благодарит вас, капитан " + GetFullName(pchar) + ".";
			link.l1 = "Благодарю, святой отец.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("HelpSacredInquisitionEnd");
		break;
		// Gregg: <-- "Помощь святой инквизиции"

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "NoMoreTalkExit":
			LAi_CharacterDisableDialog(npchar);
			DialogExit();
		break;

		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
		break;

//**************************** Фр. линейка. Квест №7, спасение Рока Бразильца ********************************
 		case "Step_F7_1":
			dialog.text = "Знаю, в крайней камере сидит, под охраной вон того солдата. Особенное место...";
			link.l1 = "Спасибо тебе!";
			link.l1.go = "exit";
		break;
 		case "Step_F7_2":
			dialog.text = "В крайней камере справа от входа, там, где стоял часовой, пока ты его не убил"+ GetSexPhrase("","а") +"...";
			link.l1 = "Ага, понятно... Спасибо тебе.";
			link.l1.go = "exit";
		break;

//**************************** запил Капитула ********************************
		case "Maltese":
			dialog.text = "Это очень хорошая новость, "+ GetSexPhrase("сын мой","дочь моя") +". Инквизиция давно хотела создать новую базу где-то на архипелаге. И заброшенный остров, пользующийся дурной славой, идеально подойдёт для этого.";
			link.l1 = "Так мне полагается какая-нибудь награда?";
			link.l1.go = "Maltese_2";
		break;

		case "Maltese_2":
			dialog.text = "Мы можем выплатить вам миллион пиастров в награду. Но у нас есть и другое предложение. Не желаете выслушать?";
			link.l1 = "И что же это?";
			link.l1.go = "Maltese_3";
		break;

		case "Maltese_3":
			dialog.text = "Если вы самостоятельно оплатите постройку, то мы назначим вас руководителем этой структуры. И предоставим своих специалистов для обучения рядового и офицерского состава.";
			link.l1 = "Каковы будут мои обязанности?";
			link.l1.go = "Maltese_4";
		break;
		case "Maltese_4":
			dialog.text = "Ваша цель - следить, чтобы на базу не нападала нежить. И в случае, если это произойдёт - защитить её.";
			link.l1 = "Звучит заманчиво... Но я так полагаю, что цена вопроса - миллион пиастров?";
			link.l1.go = "Maltese_5";
		break;

		case "Maltese_5":
			if (!CheckAttribute(pchar,"MalteseOrderOnHold")) dialog.text = "Абсолютно точно. Ну так что скажете?";
			else dialog.text = "И что же вы скажете?";
			if (sti(pchar.money)>=1000000)
			{
				link.l1 = "Золото девать всё равно некуда, а условия меня устраивают. Держите, вот миллион пиастров.";
				link.l1.go = "Maltese_6";
			}
			link.l2 = "В данный момент я не располагаю такой суммой.";
			link.l2.go = "Maltese_51";
			link.l3 = "Я всё же не хочу брать на себя такую ответственность. Я возьму миллион пиастров.";
			link.l3.go = "Maltese_no";
		break;

		case "Maltese_6":
			AddMoneyToCharacter(pchar, -1000000);
			dialog.text = "Рад, что вы согласились. Постройка Капитула (а именно так будет названа эта структура) ориентировочно будет завершена через 1 месяц. Как пройдёт время, отправляйтесь в бухту Дьявола и ищите вход там.";
			link.l1 = "Как придёт время - обязательно.";
			link.l1.go = "Maltese_build";
		break;

		case "Maltese_build":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			pchar.MalteseOrder = true;
			SetTimerFunction("BuildMalteseOrder", 0, 0, 30);
		break;

		case "Maltese_no":
			AddMoneyToCharacter(pchar, 1000000);
			dialog.text = "Это ваше право. Держите, вот ваш миллион.";
			link.l1 = "Благодарю вас. Мне пора.";
			link.l1.go = "Maltese_nofix";
		break;

		case "Maltese_nofix":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			pchar.MalteseOrder = true;
		break;

		case "Maltese_51":
			dialog.text = "Хорошо. Как соберёте нужную сумму или примете другое решение - поговорите с любым из присутствующих здесь.";
			link.l1 = "Мне пора.";
			link.l1.go = "Maltese_52";
		break;

		case "Maltese_52":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			pchar.MalteseOrderOnHold = true;
		break;
	}
}
