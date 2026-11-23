// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			// Gregg: «Чёрная Орхидея», глава 7 - почему заперта резиденция Джекмена
			if (CheckAttribute(PChar, "SmallMongrel"))
			{
				if (PChar.SmallMongrel == "2")
				{
					link.l30 = "Слушай, а почему к Джекмену теперь не войти?";
					link.l30.go = "BO_SM_Pir_1";
				}
			}
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Совсем недавно вы пытались задать мне вопрос, " + GetAddress_Form(NPChar) + "...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе...",
                          "Опять вопросы будем задавать?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я "+ GetSexPhrase("передумал","передумала") +"...", "Сейчас мне не о чем говорить."), "Хм, что-то с памятью моей стало...",
                      "Да уж, действительно в третий раз...", "Да нет, какие вопросы...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
            if (pchar.questTemp.State == "SeekBible_toBermudes")
			{
                link.l1 = HeroStringReactionRepeat("Послушай, я ищу пирата, который продавал здесь Библию какую-то странную. Не слышал об этом деле?",
                "Я про Библию "+ GetSexPhrase("хотел","хотела") +" спросить... уже выяснили, извини.", "Да уж, верно...", "А-а-а, разобрались уже, извини...", npchar, Dialog.CurrentNode);
                link.l1.go = DialogGoNodeRepeat("Step_H8_1", "", "", "", npchar, Dialog.CurrentNode);
            }
            if (pchar.RomanticQuest == "BrotherIsDead")
			{
                link.l1 = HeroStringReactionRepeat("Слушай, ты не в курсе, где Атилла сейчас обретается?",
                "Да, да, про Атиллу говорили...", "Да уж, верно...", "А-а-а, разобрались уже, извини...", npchar, Dialog.CurrentNode);
                link.l1.go = DialogGoNodeRepeat("Step_Romantic_1", "", "", "", npchar, Dialog.CurrentNode);
            }
			if (pchar.questTemp.BlueBird == "toBermudes")
			{
				link.l1 = "Послушай, приятель, ничего не слышно в таверне интересного про шебеку 'Синяя Птица'?";
				link.l1.go = "BlueBird_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toBermudes")
			{
				link.l1 = "Здесь, случайно, не появлялся Эдвард Лоу?";
				link.l1.go = "PL_Q3_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toTavernAgain" || pchar.questTemp.piratesLine == "KillLoy_2toTavernAgain")
			{
				link.l1 = "Я ищу Эдварда Лоу.";
				link.l1.go = "PL_Q3_2";
			}
			if (pchar.questTemp.piratesLine == "PL5Hunter_toBermudes")
			{
				link.l1 = "К тебе в таверну, Джон Эйвори не забегал?";
				link.l1.go = "StepPL5Berm_1";
			}
			if (CheckAttribute(PChar, "questTemp.BSPrologue.WaitingForNassauSiege") && Colonies[FindColony("Nassau")].nation == ENGLAND)
			{
				link.l1 = "Мисс Гатри здесь?";
				link.l1.go = "GatrisBarman";
			}
            // <== Проверяем поле состояния квестов.
			// Gregg: «Товар для Ричарда Тилльманна» - расспрос о Преноле на Бермудах
			// Gregg: атрибут проверяем на существование отдельным if - у героя, который
			// квест не брал, его нет, и голое сравнение валится в RUNTIME ERROR.
			// Складывать это в одно условие через && нельзя: короткого замыкания нет.
			if (CheckAttribute(PChar, "UneasyDelivery"))
			{
				if (PChar.UneasyDelivery == "12")
				{
					link.l8 = "Слушай, " + NPChar.name + ", я тут ищу одного человека, его имя Жак Преноль. Он не появлялся здесь?";
					link.l8.go = "UneasyDelivery_1";
				}
			}

			// Gregg: «Предательство Грифо Сайфуса» (перенос из К:ПС) -->
			// Атрибут квеста проверяется на существование ДО любых сравнений: у героя,
			// который не брал ни альтернативный пролог, ни ветку трактирщика, его нет.
			int iSajfusRankReq = 1;
			if (!bQuestsRank) iSajfusRankReq = 5;
			if (!CheckAttribute(PChar, "TreacheryOfGrifoSajfus"))
			{
				// Вход для тех, кто не брал альтернативный пролог: там квест выдаётся
				// сам, финалом пролога. Здесь историю о казни Сайфуса рассказывает
				// трактирщик, а ГГ идёт за пропавшей шебекой как за легендой.
				if (sti(pchar.rank) >= iSajfusRankReq)
				{
					link.l11 = "Что нового слышно в наших водах, " + NPChar.name + "?";
					link.l11.go = "TreacheryOfGrifoSajfus_0";
				}
			}
			else
			{
				if (PChar.TreacheryOfGrifoSajfus == "1")
				{
					link.l11 = "Здорово, " + NPChar.name + "!";
					link.l11.go = "TreacheryOfGrifoSajfus_1";
					// Gregg: если разговор прервали на середине, вернуть надо в СВОЮ ветку -
					// узел 1 написан от лица героя альтернативного пролога
					if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha")) link.l11.go = "TreacheryOfGrifoSajfus_2";
				}
				if (PChar.TreacheryOfGrifoSajfus == "2")
				{
					link.l12 = "Есть новости о шебеке?";
					link.l12.go = "TreacheryOfGrifoSajfus_9";
				}
				if (PChar.TreacheryOfGrifoSajfus == "3")
				{
					link.l13 = "Есть новости о шебеке?";
					link.l13.go = "TreacheryOfGrifoSajfus_10";
				}
				if (PChar.TreacheryOfGrifoSajfus == "6")
				{
					link.l14 = "Я наш" + GetSexPhrase("ёл", "ла") + " ту шебеку. И это не ''Пиранья''.";
					link.l14.go = "TreacheryOfGrifoSajfus_17";
				}
				if (PChar.TreacheryOfGrifoSajfus == "7")
				{
					link.l15 = "Есть новости о ''Пиранье''?";
					link.l15.go = "TreacheryOfGrifoSajfus_9";
				}
				if (PChar.TreacheryOfGrifoSajfus == "8")
				{
					link.l16 = "Есть новости о ''Пиранье''?";
					link.l16.go = "TreacheryOfGrifoSajfus_20";
				}
				if (PChar.TreacheryOfGrifoSajfus == "14")
				{
					link.l17 = "Я " + GetSexPhrase("выяснил", "выяснила") + ", что стало с ''Пираньей''.";
					link.l17.go = "TreacheryOfGrifoSajfus_27_2";
				}
			}
			// Gregg: «Предательство Грифо Сайфуса» <--
			// Gregg: «Чёрная Орхидея» (перенос из К:ПС) - трактирщик держит весточку
			// от Фредерика Левинсона. Ветку открывает посыльный в порту, узел 2 своего
			// диалога которого ставит MysteriousArtefact в "CanTake" и дальше в "1".
			if (CheckAttribute(PChar, "MysteriousArtefact"))
			{
				if (PChar.MysteriousArtefact == "CanTake")
				{
					link.l20 = "Мне передали, что ты меня искал.";
					link.l20.go = "MysteriousArtefact_1";
				}
			}
			// Gregg: «Чёрная Орхидея», глава 5 - четвёртая часть Меритора у Левинсона.
			// Атрибута DestinyGift у героя, не дошедшего до пятой главы, попросту нет,
			// поэтому существование проверяем отдельным вложенным if: короткого замыкания
			// в Storm нет, и через && это не сложить.
			if (CheckAttribute(PChar, "DestinyGift"))
			{
				if (PChar.DestinyGift == "20")
				{
					link.l21 = "Слушай, " + NPChar.name + ", мне нужен Фредерик Левинсон. Он у тебя не объявлялся?";
					link.l21.go = "BO_Levinson_1";
				}
			}

		break;

		case "UneasyDelivery_1":
			dialog.text = "Хм. Дай вспомнить. Нет, не припоминаю. Может быть, и заходил, да только откуда мне знать всех посетителей моей таверны. Они же мне не представляются, когда рому просят.";
			link.l1 = "Да, верно. Ладно, " + NPChar.name + ", пойду искать.";
			link.l1.go = "UneasyDelivery_2";
		break;

		case "UneasyDelivery_2":
			dialog.text = "А зачем он тебе нужен-то?";
			link.l1 = "У меня с ним старые счёты.";
			link.l1.go = "UneasyDelivery_3";
		break;

		case "UneasyDelivery_3":
			dialog.text = "Старые счёты, говоришь? Ну, если найдёшь его, как следует поквитайся.";
			link.l1 = "Что-что, а это я умею. Ладно, до встречи.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryDialogWithBermudesBarmen");
		break;

		// Gregg: ПРЕДАТЕЛЬСТВО ГРИФО САЙФУСА (перенос из К:ПС) -->
		// Узел 0 - вход через таверну, узел 1 - вход из альтернативного пролога.
		case "TreacheryOfGrifoSajfus_0":
			TreacheryOfGrifoSajfusStartFromTavern();
			dialog.text = "Есть свежая история, как раз для тебя. Дней пять назад Грифо Сайфус к нам в гавань пожаловал. Редкий гость в наших водах, сам знаешь. Так один из его матросов напился и проболтался о том, что Сайфус на Доминике натворил.";
			link.l1 = "И что же он натворил?";
			link.l1.go = "TreacheryOfGrifoSajfus_2";
		break;

		case "TreacheryOfGrifoSajfus_1":
			dialog.text = PChar.name + "! Ты " + GetSexPhrase("жив", "жива") + ", чёрт тебя подери, " + GetSexPhrase("жив", "жива") + "! А мы уже тебя тут похоронили всем поселением, думали, что всё, положили тебя в том аду около Доминики.";
			link.l1 = GetSexPhrase("Живой", "Живая") + " я, как видишь! А откуда ты узнал про то, что случилось у Доминики?";
			link.l1.go = "TreacheryOfGrifoSajfus_2";
		break;

		case "TreacheryOfGrifoSajfus_2":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Своего товарища, капитана, с которым не один год ходил, заманил на Доминику и там положил вместе со всей командой. А корабль забрал себе.";
				link.l1 = "Из-за корабля? Хорош друг.";
			}
			else
			{
				dialog.text = "Где-то дней пять назад Грифо Сайфус к нам в гавань пожаловал. Он редкий гость в наших водах, сам знаешь. Так один из его матросов напился и проболтался о том, что Сайфус на Доминике натворил.";
				link.l1 = "И что было дальше?";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_3";
		break;

		case "TreacheryOfGrifoSajfus_3":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Грифо прижали к стенке и допрос устроили. Сначала отнекивался, а часа через два сознался в содеянном. Мы его прямо здесь и расстреляли за предательство.";
				link.l1 = "Поделом. А что за корабль-то был?";
			}
			else
			{
				dialog.text = "Грифо прижали к стенке, и допрос устроили. Он сначала отнекивался, говорил, не было такого, да и как он, твой давний друг, мог тебя убить. Но часа через два сознался в содеянном, и мы его прямо здесь и расстреляли за предательство. Отомстили за твою гибель. Мы же думали тогда, что тебя уже с нами нет.";
				link.l1 = "Чёрт, а я-то " + GetSexPhrase("хотел", "хотела") + " собственноручно ему глотку перерезать.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_4";
		break;

		case "TreacheryOfGrifoSajfus_4":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Шебека ''Пиранья''. Знатная была посудина, ни у кого такой на Карибах не было. Он всегда о ней мечтал, вот и решился на предательство.";
				link.l1 = "И где она теперь?";
			}
			else
			{
				dialog.text = "Знали бы мы, что ты " + GetSexPhrase("остался", "осталась") + " в живых, оставили бы его тебе на расправу. А так...";
				link.l1 = "Да понимаю. Я только одного понять не могу, почему он меня предал? Зачем? Столько лет дружбы, а тут...";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_5";
		break;

		case "TreacheryOfGrifoSajfus_5":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Вот это и загадка. На Бермуды-то Грифо приплыл на старом, прогнившем шлюпе, а не на ней. А куда её дел, не сознался, сколько мы его ни кололи.";
				link.l1 = "Такой корабль не иголка. Рано или поздно всплывёт.";
			}
			else
			{
				dialog.text = "Здесь-то как раз дело ясное. Из-за шебеки твоей. Ему твоя ''Пиранья'' нужна была, он всегда мечтал о ней, вот и решился на предательство. Сам признался, когда его допрашивали.";
				link.l1 = "Вот тварь! Ради корабля " + GetSexPhrase("предал", "предала") + "!";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_6";
		break;

		case "TreacheryOfGrifoSajfus_6":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Вот и я о том же. Шебека знатная, ни у кого такой на Карибах не было. Если кто её захватил, сразу узнаем.";
				link.l1 = "А если её на дно пустили? Грифо с кораблём толком управляться не умел, а уж с такой красавицей точно бы не совладал.";
			}
			else
			{
				dialog.text = "Только здесь кое-чего не вяжется. На Бермуды-то он приплыл на старом, прогнившем шлюпе, а не на твоей красавице. А вот куда он её дел, не сознался, сколько мы его ни кололи. Впрочем, это мы быстро выясним. Шебека у тебя была знатная, ни у кого такой на Карибах не было. Если кто её захватил, сразу узнаем.";
				link.l1 = "Если её на дно не пустили. Грифо с кораблём толком управляться не умеет, а уж с ''Пираньей'' точно бы не совладал. Вот и могли её пустить ко дну.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_7";
		break;

		case "TreacheryOfGrifoSajfus_7":
			dialog.text = "Никто в здравом уме и светлой памяти не будет топить такой корабль. Только захватывать. Сам подумай, ты видишь в море красавец-корабль, уникальное по своим характеристикам корыто, что ты с ним будешь делать? Конечно, попытаешься захватить.";
			link.l1 = "Да, верно. Но если Сайфуса атаковали ночью, могли и не увидеть, что за корабль перед ними, и отправить его на дно. Тоже возможно.";
			link.l1.go = "TreacheryOfGrifoSajfus_8";
		break;

		case "TreacheryOfGrifoSajfus_8":
			dialog.text = "А чего гадать, собственно? Время покажет. Ты заходи ко мне время от времени, если будут слухи о ней, обязательно передам.";
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				link.l1 = "Заметано. Такой корабль стоит того, чтобы за ним побегать.";
			}
			else
			{
				link.l1 = "Заметано, буду заходить время от времени.";
			}
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusPiratesBarmenDialogEnd");
		break;

		case "TreacheryOfGrifoSajfus_9":
			dialog.text = "Пока ничего не слышно.";
			link.l1 = "Эх, жаль.";
			link.l1.go = "exit";
		break;

		// Gregg: караван перенесён с Кубы на Пуэрто-Рико - см. шапку grifosajfus.c
		case "TreacheryOfGrifoSajfus_10":
			dialog.text = "Да, есть новости. Вчера завалился ко мне в таверну какой-то капитан, как звать не знаю, но по виду новенький. Начал рассказывать, как у берегов Пуэрто-Рико встретил испанский торговый караван из четырёх галеонов и шебеки. Он, ясное дело, решил не соваться к ним, сам был на лёгком шлюпе, поэтому отвернул в сторону. Галеоны курс не сменили, а вот шебека ринулась в погоню. И по его словам, догнала его шлюп за считанные минуты, хотя расстояние между ними было приличным.";
			link.l1 = "И что дальше? Абордаж?";
			link.l1.go = "TreacheryOfGrifoSajfus_11";
		break;

		case "TreacheryOfGrifoSajfus_11":
			dialog.text = "Дальше произошло чудо. Канониры умудрились одним залпом книппелей сбить две мачты! Они сами удивились, когда увидели, что из этого залпа вышло. В общем, шебека ход резко потеряла, и они быстро оттуда свалили.";
			link.l1 = "Думаешь, это ''Пиранья''?";
			link.l1.go = "TreacheryOfGrifoSajfus_12";
		break;

		case "TreacheryOfGrifoSajfus_12":
			dialog.text = "Очень похоже на то. Тот кэп сказал, что это самый быстрый корабль, который он когда-либо встречал.";
			link.l1 = "Хм. Ремонт корабля у них затянется надолго, в этом я " + GetSexPhrase("уверен", "уверена") + ". Значит, та шебека сейчас в Сан-Хуане. Тот капитан не упоминал, в какой порт шёл караван?";
			link.l1.go = "TreacheryOfGrifoSajfus_13";
		break;

		case "TreacheryOfGrifoSajfus_13":
			dialog.text = "Кажется, в Сан-Хуан. Впрочем, точно сказать не могу. Что собираешься делать?";
			link.l1 = "Поплыву к Пуэрто-Рико и проверю корабль. Если эта шебека действительно ''Пиранья'', пойду на абордаж.";
			link.l1.go = "TreacheryOfGrifoSajfus_14";
		break;

		case "TreacheryOfGrifoSajfus_14":
			dialog.text = "А если на корабле ещё не установили мачты?";
			link.l1 = "Там видно будет. Для начала надо убедиться, что это она.";
			link.l1.go = "TreacheryOfGrifoSajfus_15";
		break;

		case "TreacheryOfGrifoSajfus_15":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Ну, тогда попутного ветра, " + PChar.name + ", и осторожней будь. Про эту шебеку рассказывают такое, что не всякому капитану по зубам.";
				link.l1 = "Рассказывают всякое. Посмотрим, чего она стоит на деле.";
			}
			else
			{
				dialog.text = "Ну, тогда попутного ветра, " + PChar.name + ", и осторожней будь, если с ''Пираньей'' будешь сражаться. Сам знаешь, что она способна вытворять...";
				link.l1 = "...в руках умелого капитана. С ней мало кто способен совладать, кроме меня.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_16";
		break;

		case "TreacheryOfGrifoSajfus_16":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Ну, смотри, как бы тебе там задницу не надрали, герой.";
				link.l1 = "Ээ... Обойдёмся без этого.";
			}
			else
			{
				dialog.text = "Ну, смотри, как бы тебе во второй раз задницу не надрали, герой.";
				link.l1 = "Ээ... Второго раза точно не будет.";
			}
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusXebecNewsInCuba");
		break;

		case "TreacheryOfGrifoSajfus_17":
			dialog.text = "И? Что?";
			if (PChar.TreacheryOfGrifoSajfus.ChameleonSink == false)
			{
				// Gregg: без пролога ГГ «Пиранью» в глаза не видел - судит по слухам,
				// а не по знакомому убранству кают
				if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
				{
					link.l1 = "Но как только я её " + GetSexPhrase("захватил", "захватила") + ", всё стало ясно. Про ту шебеку говорили, что она уходит от любого, а эта еле ползёт. Это просто очень похожий на неё корабль.";
				}
				else
				{
					link.l1 = "Но как только я её " + GetSexPhrase("захватил", "захватила") + ", я " + GetSexPhrase("понял", "поняла") + ", что это не она. Не то внутреннее убранство, не те пушки, не те возможности. Это просто очень похожий на неё корабль.";
				}
			}
			else
			{
				link.l1 = "Но как только я " + GetSexPhrase("вступил", "вступила") + " с ней в схватку, я " + GetSexPhrase("понял", "поняла") + ", что это не она. Уж очень странно она себя вела даже для неумелого капитана. Это просто очень похожий на неё корабль.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_18";
		break;

		case "TreacheryOfGrifoSajfus_18":
			dialog.text = GetSexPhrase("Ты уверен", "Ты уверена") + " в этом?";
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				link.l1 = "На все сто. Против ветра ходит вдвое медленнее, чем рассказывают про ''Пиранью'', манёвренность ни к чёрту, скорости совсем нет. Это не она.";
			}
			else
			{
				link.l1 = "На все сто. Против ветра ходит раза в два медленнее, чем ''Пиранья'', манёвренность ни к чёрту, скорости совсем нет. Это не ''Пиранья''.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_19";
		break;

		case "TreacheryOfGrifoSajfus_19":
			dialog.text = "Значит, будем искать дальше. Заходи время от времени, может, что выясню.";
			link.l1 = "Хорошо, " + NPChar.name + ".";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusItIsNotShip");
		break;

		case "TreacheryOfGrifoSajfus_20":
			dialog.text = "Никаких слухов. В последнее время вообще ничего нового на Карибах не происходит. Зато послушай, что расскажу. Вчера по сходной цене у одного торгаша приезжего купил пару алмазов жене в подарок. Чёрт, ты бы видел" + GetSexPhrase("", "а") + " эти алмазы! Блеск, высший класс! Торгаш говорит, из Африки привёз.";
			link.l1 = "Из Африки? Ты сказал - из Африки?";
			link.l1.go = "TreacheryOfGrifoSajfus_21";
		break;

		case "TreacheryOfGrifoSajfus_21":
			dialog.text = "Да. А что такое? Ни разу не слышал о них?";
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				link.l1 = "Ты же сам говорил, что Сайфус шёл к Доминике за судном с африканскими алмазами. Там он товарища и положил, там же и корабль забрал.";
			}
			else
			{
				link.l1 = "Сайфус предлагал мне атаку судна, на котором по слухам находились африканские алмазы. Мы договорились о встрече на Доминике, где он и отнял у меня корабль.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_22";
		break;

		case "TreacheryOfGrifoSajfus_22":
			dialog.text = "И что? Я не понимаю, к чему ты клонишь.";
			link.l1 = "Тот торгаш, который продал тебе алмаз, сказал, что он из Африки, так?";
			link.l1.go = "TreacheryOfGrifoSajfus_23";
		break;

		case "TreacheryOfGrifoSajfus_23":
			dialog.text = "Ну да, всё так.";
			link.l1 = "Держу пари, что этот алмаз был с того корабля, который Сайфус собирался атаковать.";
			link.l1.go = "TreacheryOfGrifoSajfus_24";
		break;

		case "TreacheryOfGrifoSajfus_24":
			dialog.text = "И?";
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				link.l1 = "На ''Пиранье'' Грифо наверняка смог взять на абордаж тот корабль с алмазами. Что случилось после, никто не знает. И тут объявляется какой-то торговец с африканскими алмазами. Понимаешь? Он как-то связан со всей этой историей, и если я " + GetSexPhrase("узнаю", "узнаю") + ", откуда у него камни, то, возможно, " + GetSexPhrase("узнаю", "узнаю") + " и что случилось с шебекой.";
			}
			else
			{
				link.l1 = "На моей ''Пиранье'' Грифо наверняка смог взять на абордаж тот корабль с алмазами. Что случилось после, никто не знает. И тут объявляется какой-то торговец с африканскими алмазами. Понимаешь? Тот торгаш как-то связан со всей этой историей и, возможно, с ''Пираньей'', и если я " + GetSexPhrase("узнаю", "узнаю") + ", откуда он взял эти алмазы, возможно, я " + GetSexPhrase("узнаю", "узнаю") + ", что случилось с моей шебекой.";
			}
			link.l1.go = "TreacheryOfGrifoSajfus_25";
		break;

		case "TreacheryOfGrifoSajfus_25":
			dialog.text = "Бред какой-то. Как ты узнаешь?";
			link.l1 = "Пока сам" + GetSexPhrase("", "а") + " не понимаю, но уверен" + GetSexPhrase("", "а") + ", что какая-то связь тут есть. Ты случаем не знаешь, куда тот торговец делся?";
			link.l1.go = "TreacheryOfGrifoSajfus_26";
		break;

		case "TreacheryOfGrifoSajfus_26":
			dialog.text = "Да на Бермудах, наверное, ещё ошивается, пытается продать остальные алмазы. Поищи, может, где найдёшь.";
			link.l1 = "Угу, " + GetSexPhrase("понял", "поняла") + ".";
			link.l1.go = "TreacheryOfGrifoSajfus_27";
		break;

		case "TreacheryOfGrifoSajfus_27":
			dialog.text = "Эй, " + PChar.name + ", как выяснишь, что с ''Пираньей'', загляни ко мне. Самому интересно становится, куда же она подевалась.";
			link.l1 = "Замётано, " + NPChar.name + ", как узнаю, расскажу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusNeedTrader");
		break;

		// LEO: промежуточный узел - ГГ сперва САМ сообщает новость. Раньше из меню («я выяснил, что стало
		//   с Пираньей») разговор прыгал прямо в реакцию трактирщика, и тот сокрушался об утопленном
		//   корабле раньше, чем ему хоть слово сказали, а ответ ГГ отвечал на незаданный вопрос.
		case "TreacheryOfGrifoSajfus_27_2":
			dialog.text = "Ну не томи. Что с ней?";
			link.l1 = "Её больше нет, " + NPChar.name + ". Лежит на дне, около Теркса.";
			link.l1.go = "TreacheryOfGrifoSajfus_28";
		break;

		case "TreacheryOfGrifoSajfus_28":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Вот оно как. И кто ж её туда отправил?";
			}
			else
			{
				dialog.text = "Вот чёрт! Жаль твою ласточку, " + PChar.name + ", очень жаль. А кто ж её на дно пустил?";
			}

			if (PChar.TreacheryOfGrifoSajfus.MarlinSink == false)
			{
				link.l1 = "Этого выяснить не удалось. Но есть и отличная новость. Я " + GetSexPhrase("захватил", "захватила") + " другой корабль, гораздо лучше ''Пираньи''!";
				link.l1.go = "TreacheryOfGrifoSajfus_29";
			}
			else
			{
				link.l1 = "Этого выяснить не удалось.";
				link.l1.go = "TreacheryOfGrifoSajfus_31";
			}
		break;

		case "TreacheryOfGrifoSajfus_29":
			if (CheckAttribute(PChar, "TreacheryOfGrifoSajfus.NoPiranha"))
			{
				dialog.text = "Лучше ''Пираньи''? Хотел бы я на это посмотреть.";
			}
			else
			{
				dialog.text = "Хм, ты ли это, " + PChar.name + "? Поверить не могу, что ты утверждаешь, будто какой-то корабль может быть лучше, чем твоя ''Пиранья''.";
			}
			link.l1 = "Это не корабль, это произведение искусства. Отменная манёвренность, великолепная скорость, вооружение что надо.";
			link.l1.go = "TreacheryOfGrifoSajfus_30";
		break;

		case "TreacheryOfGrifoSajfus_30":
			dialog.text = "Это дело надо обмыть.";
			link.l1 = "На-ли-вай!..";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusEnd");
		break;

		case "TreacheryOfGrifoSajfus_31":
			dialog.text = "Мда, хороший был корабль.";
			link.l1 = "Превосходный корабль.";
			link.l1.go = "TreacheryOfGrifoSajfus_32";
		break;

		case "TreacheryOfGrifoSajfus_32":
			dialog.text = "Что, " + PChar.name + ", не расслышал?";
			link.l1 = "Это был не хороший, а превосходный корабль.";
			link.l1.go = "TreacheryOfGrifoSajfus_33";
		break;

		case "TreacheryOfGrifoSajfus_33":
			dialog.text = "Верно. Но хватит уже горевать. Допивай ром, забывай про ''Пиранью'' и давай уже начинай делом каким-нибудь заниматься.";
			link.l1 = "Сегодня для меня никого нет. Налей-ка мне ещё немного рома...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TreacheryOfGrifoSajfusEnd");
		break;
		// Gregg: ПРЕДАТЕЛЬСТВО ГРИФО САЙФУСА <--

		//шебека Синяя Птица
		case "BlueBird_1":
			dialog.text = NPCStringReactionRepeat("Слышно-то много всякого. В основном говорят, что корабль очень быстрый. Поймать пытались, да куда там...",
				"Мы уже говорили об этом.",
				"Опять? Уже все выяснили.",
                "Простите, если вам больше ничего не надо, то, пожалуй, займусь другими клиентами.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("А кто пытался поймать?",
				"Да, точно...",
                "Хм, верно.",
				"Хорошо...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("BlueBird_2", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
		break;

		case "BlueBird_2":
			dialog.text = "Многие пытались, да обломались, ха-ха!!";
			link.l1 = "А что-нибудь путное тебе известно? Где её видят часто, к примеру.";
			link.l1.go = "BlueBird_3";
		break;
		case "BlueBird_3":
			dialog.text = "Дык здесь и видят, у Бермуд. По хождению в наших рифах 'Синей Птице' равных нет, не раз в погоне за ней галеоны да фрегаты бились - и прямиком на дно!";
			link.l1 = "Вот значит как... Ну что же, будем искать. Слушай, а хозяин магазина местный, он что за человек?";
			link.l1.go = "BlueBird_4";
		break;
		case "BlueBird_4":
			dialog.text = "Авторитетный весьма, уважаемый среди торговцев.";
			link.l1 = "Понятно... А есть человек, который с ним не в очень хороших отношениях?";
			link.l1.go = "BlueBird_5";
		break;
		case "BlueBird_5":
			dialog.text = "Откровенных врагов у него нет. Но вот бывший хозяин магазина, думаю, зуб на него имеет, хе-хе...";
			link.l1 = "Не будешь ли ты любезен, милейший, назвать мне его имя.";
			link.l1.go = "BlueBird_6";
		break;
		case "BlueBird_6":
			dialog.text = "Его зовут Джон Нортон. В Шарп-Тауне ты его уже не найдёшь. Слышал я, что он перебрался в Пуэрто-Принсипе, торгует там себе потихоньку и никуда не высовывается.";
			link.l1 = "Большое тебе спасибо, приятель. Ты - хороший трактирщик.";
			link.l1.go = "BlueBird_7";
		break;
		case "BlueBird_7":
			dialog.text = "Спасибо за комплимент, " + GetAddress_Form(NPChar) + ". Удачи вам!";
			link.l1 = "И тебе всего хорошего, приятель.";
			link.l1.go = "exit";
			AddQuestRecord("Xebeca_BlueBird", "3");
			pchar.questTemp.BlueBird = "toPuertoPrincipe";
		break;

		//голл. линейка, квест №8
 		case "Step_H8_1":
    		dialog.text = "Нет, не слышал.";
    		link.l1 = "Вроде история известная, мне на Кюрасао её рассказали.";
    		link.l1.go = "Step_H8_2";
 		break;
 		case "Step_H8_2":
    		dialog.text = "Ничего об этом не знаю, у меня в таверне такого не было.";
    		link.l1 = "Спасибо за помощь.";
    		link.l1.go = "exit";
 		break;
		//Изабелла, ищем Атиллу
 		case "Step_Romantic_1":
    		dialog.text = "Знаю, он дом купил здесь, в городе. Так что там и должен быть.";
    		link.l1 = "Спасибо, приятель...";
    		link.l1.go = "exit";
 		break;
		//Пиратка, квест №3, поиски Лоу
		case "PL_Q3_1":
			dialog.text = "Появлялся. Все ходил, чего-то вынюхивал. То к одному подсядет, то к другому... Кажется, он интересовался нашей верфью.";
			link.l1 = "Благодарю.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "9");
			pchar.questTemp.piratesLine = "KillLoy_toAlexus";
		break;

		case "PL_Q3_2":
			dialog.text = "Так вы уже про него спрашивали.";
			link.l1 = "Может, ты чего вспомнил?";
			link.l1.go = "PL_Q3_3";
		break;
		case "PL_Q3_3":
			dialog.text = "Да чего тут вспоминать, он все про верфь выспрашивал, к людям разным подсаживался. Вон, кстати, Капитан Гудли сидит, он с Лоу разговаривал.";
			link.l1 = "Капитан Гудли?! Спасибо, уже кое-что.";
			link.l1.go = "exit";
			pchar.questTemp.piratesLine = "KillLoy_toGoogly";
		break;
		//Пиратка, квест №5, ОЗГ
		case "StepPL5Berm_1":
			dialog.text = "Да вот, только что и забегал. Спросил, где живёт Орри Брюс, и ушёл. ";
			link.l1 = "И где же этот Орри Брюс обитает?";
			link.l1.go = "StepPL5Berm_2";
		break;
		case "StepPL5Berm_2":
			dialog.text = "Так, в двухэтажном доме с каменной кладкой он живёт. Как выйдешь из таверны, налево первый дом. Там и обитает...";
			link.l1 = "Спасибо, ты мне здорово помог!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "10");
			pchar.questTemp.piratesLine = "PL5Hunter_toOrryHouse";
		break;

		case "GatrisBarman":
			dialog.text = "Да, она в комнате. Желаете аудиенции?";
			link.l1 = "Да.";
			link.l1.go = "GatrisBarmanIndoor";
			link.l2 = "В другой раз.";
			link.l2.go = "exit";
		break;
		
		case "GatrisBarmanIndoor":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
            DoReloadCharacterToLocation("Pirates_tavern_upstairs","goto", "goto1");
		break;		

		// Gregg: ЧЁРНАЯ ОРХИДЕЯ (перенос из К:ПС) -->
		case "MysteriousArtefact_1":
			dialog.text = "Искал, капитан, искал. Тут на Бермуды заявился один твой старый знакомый - Фредерик Левинсон. Помнишь такого?";
			link.l1 = "Левинсон? Помню, конечно. Он же уехал в Европу и клялся, что не вернётся.";
			link.l1.go = "MysteriousArtefact_2";
		break;

		case "MysteriousArtefact_2":
			dialog.text = "Вернулся, как видишь. Расспрашивал про тебя, а потом сорвался куда-то в джунгли - и с тех пор его не видели. Не нравится мне это, капитан. Сходил бы ты за ним.";
			link.l1 = "Схожу. Спасибо, старина.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactStart");
		break;

		// Gregg: глава 5 - трактирщик наводит на Левинсона. Он берёт с постояльца
		// за молчание и имени не спрашивал, поэтому связать его с наградой Джекмена
		// не может, пока ГГ не назовёт имя сам.
		case "BO_Levinson_1":
			dialog.text = "Левинсон? Твой приятель, что вон за тем столом сиживал? Пропал он, капитан. Как ты в море ушёл" + GetSexPhrase("", "а") + ", так его и не видно. Хотя постоялец у меня наверху есть - тихий, платит вперёд, из комнаты почти не выходит. Который месяц так и сидит. Имени я не спрашивал: он как раз за это и приплачивает.";
			link.l1 = "Который месяц не выходит из комнаты? Занятный у тебя постоялец.";
			link.l1.go = "BO_Levinson_2";
		break;

		case "BO_Levinson_2":
			dialog.text = "У нас полгорода от кого-нибудь прячется, дело обычное. Вот и Джекмен ищет одного - десять тысяч за голову объявил. Говорят, три года назад тот продал испанцам своих же корсаров. Не корабль, не долю - людей, с которыми сам же и ходил. За такое у нас не прощают.";
			link.l1 = GetSexPhrase("Слышал", "Слышала") + " я эту историю. И знаю её ближе, чем хотелось бы: один из тех корсаров был мне другом.";
			link.l1.go = "BO_Levinson_3";
		break;

		case "BO_Levinson_3":
			dialog.text = "...(помолчав)... Тогда ты понимаешь, почему за него дают такие деньги. Только взять их некому: джунгли прочесали, пещеры облазили, поселение перетряхнули - и пусто. Решили уже, что он с Бермуд уплыл... ...(осекается)... Капитан. Капитан, ты это к чему?";
			link.l1 = "К тому, что искали его по кустам, а он всё это время сидел у тебя наверху. Молчи, старина, Джекмену я ничего не скажу - у меня к твоему постояльцу свой счёт.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftFindLevinson");
		break;
		// Gregg: ЧЁРНАЯ ОРХИДЕЯ <--

		// Gregg: «Чёрная Орхидея», глава 7 -->
		case "BO_SM_Pir_1":
			dialog.text = "А ты не слыхал" + GetSexPhrase("", "а") + "? Ну ты даёшь.";
			link.l1 = "Не слыхал. Рассказывай.";
			link.l1.go = "BO_SM_Pir_2";
		break;

		case "BO_SM_Pir_2":
			dialog.text = "В Джекмена стреляли. С тех пор он к себе никого и не пускает. Все Бермуды об этом гудят, а до тебя не дошло.";
			link.l1 = "Мне к нему надо. Сегодня.";
			link.l1.go = "BO_SM_Pir_3";
		break;

		case "BO_SM_Pir_3":
			dialog.text = "Тогда тебе не ко мне, а к Алену Баррету. Он у той двери стоит. Скажешь ему, что надо к Джекмену, он объяснит порядок.";
			link.l1 = "А где искать Баррета?";
			link.l1.go = "BO_SM_Pir_4";
		break;

		case "BO_SM_Pir_4":
			dialog.text = "В бухте Разбитого корыта, где старый остов лежит. Он там почти всегда: тихо, и лишних глаз нет. Только языком при нём не мети, он этого не любит.";
			link.l1 = "Учту. Молчаливых я уважаю, с ними меньше хлопот.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelFindBarret");
		break;
		// Gregg: <--
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
