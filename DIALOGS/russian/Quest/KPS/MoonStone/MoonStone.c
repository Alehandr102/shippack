// Gregg: диалоги квеста "Легенда о призрачном свете" (перенос из К:ПС).
// Клейн на Бермудах, его связной в Бас-Тере, Адам Кромвель, Фрэнсис Айво,
// Джейк Смитт и Педро Карлос в гроте Невиса.
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
			// Gregg: у каждого свой отбой. Гард по MoonStone тут не нужен - узел
			// достижим только пока квест идёт, но проверку id оставляем как в оригинале.
			if (NPChar.id == "Adam Kromvel")
			{
				dialog.text = "Не мешай мне.";
				if (pchar.MoonStone == "3") dialog.text = "Жду тебя за городом. И исповедаться не забудь.";
				link.l1 = "Как скажешь.";
				link.l1.go = "exit";
				break;
			}
			if (NPChar.id == "Klein Man")
			{
				dialog.text = "Работу свою сделал" + GetSexPhrase("", "а") + "? Нет? Тогда иди и делай.";
				link.l1 = "Как скажешь.";
				link.l1.go = "exit";
				break;
			}
			if (NPChar.id == "Klein")
			{
				dialog.text = "Вы что, не поняли? Отплывайте, и курс на Мартинику.";
				link.l1 = "...";
				link.l1.go = "exit";
				break;
			}
			NextDiag.TempNode = "First time";
		break;

		case "Adam_Kromvel_1":
			dialog.text = "Кто вы, чёрт возьми?! Убирайтесь отсюда!";
			link.l1 = "О-о, теперь вы оскорбили меня дважды. Я вызываю вас на дуэль!";
			link.l1.go = "Adam_Kromvel_2";
		break;

		case "Adam_Kromvel_2":
			dialog.text = "Э... кхм... Вызов принимаю. Но какова же вторая причина?";
			link.l1 = "Вы неприлично посмотрели на мою женщину. Жду вас за городом.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			AddDialogExitQuestFunction("MoonStoneToSentJonsExit");
		break;

		case "Adam_Kromvel_3":
			dialog.text = "А-а, пришли. Приступим?";
			link.l1 = "Погодите. Любопытно: кому вы так насолили, что к вам подослали убийцу?";
			link.l1.go = "Adam_Kromvel_4";
		break;

		case "Adam_Kromvel_4":
			dialog.text = "Так ты " + GetSexPhrase("один", "одна") + " из этих подонков?! Умри!";
			link.l1 = "Эх...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithKromvel");
		break;

		case "Francis_Ayvo_1":
			dialog.text = "А ты, видимо, и есть тот наёмный убийца? Умри!";
			link.l1 = "Постой!..";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithAjvoGardian");
		break;

		case "Francis_Ayvo_2":
			dialog.text = "А-а-а, вы его убили!";
			link.l1 = "Замолчи, или станешь следующим.";
			link.l1.go = "Francis_Ayvo_3";
		break;

		case "Francis_Ayvo_3":
			dialog.text = "Всё... м-молчу. Н-но кто вы?";
			link.l1 = "Неважно. Лучше расскажите, кому вы перешли дорогу.";
			link.l1.go = "Francis_Ayvo_4";
		break;

		case "Francis_Ayvo_4":
			dialog.text = "Д-да кому угодно. В-вам виднее, в-вы же меня пришли у-убить.";
			link.l1 = "Перестаньте заикаться. Мне интересна ваша версия.";
			link.l1.go = "Francis_Ayvo_5";
		break;

		case "Francis_Ayvo_5":
			dialog.text = "Я-я от рождения з-заикаюсь. А с-смерти моей м-может желать только то б-братство у-убийц...";
			link.l1 = "Вот отсюда подробнее.";
			link.l1.go = "Francis_Ayvo_6";
		break;

		case "Francis_Ayvo_6":
			dialog.text = "Н-началось всё с того, как н-на Карибы п-прибыло это б-братство. Они у-убивают за деньги и н-никого не щадят.";
			link.l1 = "А про Джейка Смитта что-нибудь знаешь?";
			link.l1.go = "Francis_Ayvo_7";
		break;

		case "Francis_Ayvo_7":
			dialog.text = "Э-это их главарь.";
			link.l1 = "Ладно. Расскажи-ка лучше про лунный камень.";
			link.l1.go = "Francis_Ayvo_8";
		break;

		case "Francis_Ayvo_8":
			dialog.text = "Н-нет! Я л-лучше умру!";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithAjvo");
		break;

		case "Claine_0":
			MoonStoneToBermudesTavernTalk();
			dialog.text = "Вы кто? Могу я узнать ваше имя?";
			link.l1 = GetSexPhrase("Натан Рич.", "Я от Натана Рича.") + " ...(показывает амулет)...";
			link.l1.go = "Claine_2";
		break;

		case "Claine_1":
			dialog.text = "Вы кто? Могу я узнать ваше имя?";
			link.l1 = GetSexPhrase("Натан Рич.", "Я от Натана Рича.") + " ...(показывает амулет)...";
			link.l1.go = "Claine_2";
		break;

		case "Claine_2":
			dialog.text = GetSexPhrase("Я вас заждался. Где вы пропадали?", "Наконец-то. Натан прислал вас вместо себя?");
			link.l1 = GetSexPhrase("Простите. Появились срочные дела.", "Да. У него срочные дела. Я выполню его работу.");
			link.l1.go = "Claine_3";
		break;

		case "Claine_3":
			dialog.text = "Ладно. Прошу прощения, что не сразу посвятил в дело ваших друзей. Кстати, где они?";
			link.l1 = "У них тоже нашлись важные дела, к нам они не присоединятся. Ближе к делу.";
			link.l1.go = "Claine_4";
		break;

		case "Claine_4":
			dialog.text = "Что ж, о деле. Мне нужно попасть в Форт-де-Франс.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			AddDialogExitQuestFunction("MoonStoneToFortFrance");
		break;

		case "Claine_5":
			dialog.text = "Да, не зря о вас говорили как о хорошем капитане.";
			link.l1 = "Вы мне льстите.";
			link.l1.go = "Claine_6";
		break;

		case "Claine_6":
			dialog.text = "Довольно. Теперь о деле. Уладьте пока свои дела, а через неделю будьте в Бас-Тере, там же, где остановились. К вам придёт наш человек и скажет, что делать дальше. Он скажет «привет от Клейна», а вы в ответ покажете амулет. И не заставляйте его ждать.";
			link.l1 = "Хорошо, через неделю буду дома.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneExitFortFrance");
		break;

		case "Claine_7":
			dialog.text = "Привет от Клейна.";
			link.l1 = "...(показывает амулет)...";
			link.l1.go = "Claine_8";
		break;

		case "Claine_8":
			dialog.text = "Заждался вас, уже собирался уходить. Впрочем, Клейн предупреждал о вашей непунктуальности.";
			link.l1 = "Простите за задержку. У вас для меня что-то есть?";
			link.l1.go = "Claine_9";
		break;

		case "Claine_9":
			dialog.text = "Есть. На Карибах живёт человек, который нам сильно мешает.";
			link.l1 = "Я прежде хотел" + GetSexPhrase("", "а") + " бы узнать...";
			link.l1.go = "Claine_10";
		break;

		case "Claine_10":
			dialog.text = "Вопросы будете задавать, когда сделаете дело. Итак: англичанин, живёт в Сент-Джонсе, зовут Адам Кромвель. Сделайте так, чтобы его не стало. Как - мне неинтересно. Награда - двадцать пять тысяч.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneToSentJons");
		break;

		case "Claine_11":
			dialog.text = "А вот и вы. Ну как, справились?";
			link.l1 = "Справил" + GetSexPhrase("ся", "ась") + ". Но есть кое-что ещё.";
			link.l1.go = "Claine_12";
		break;

		case "Claine_12":
			dialog.text = "Что-то случилось?";
			link.l1 = "В общем, ничего особенного. Кроме вот этого письма.";
			link.l1.go = "Claine_13";
		break;

		case "Claine_13":
			TakeNItems(pchar, "HelpSacredInquisition_Letter3", -1);
			dialog.text = "Вот как. Любопытно. Вы ведь его читали? У меня для вас новое дело, и срочное.";
			link.l1 = "Вы обещали ответить на вопросы.";
			link.l1.go = "Claine_14";
		break;

		case "Claine_14":
			AddMoneyToCharacter(pchar, 25000);
			dialog.text = "Потом. А пока плывите на Невис. Найдёте там Фрэнсиса Айво и уничтожите. Награда прежняя - двадцать пять тысяч. Я снова буду ждать здесь. И вот ваши деньги за Кромвеля.";
			link.l1 = "Всё ясно.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneToNevis");
		break;

		case "Claine_15":
			dialog.text = "Раз вы здесь - задание можно считать выполненным?";
			link.l1 = "Совершенно верно. Вы обещали ответить на мои вопросы.";
			link.l1.go = "Claine_16";
		break;

		case "Claine_16":
			dialog.text = "Задавайте.";
			link.l1 = "Что за лунный камень?";
			link.l1.go = "Claine_17";
		break;

		case "Claine_17":
			dialog.text = "Я знал, что вы всё-таки спросите. Что именно хотите знать? Полагаю, кое-что вам уже известно.";
			link.l1 = "Если это кусок луны, как он оказался здесь?";
			link.l1.go = "Claine_18";
		break;

		case "Claine_18":
			dialog.text = "Знаем мы одно: его сам Господь подарил некой древней цивилизации. Взял с луны и подарил.";
			link.l1 = "Атлантам, значит. Почему вы верите, что это правда? И что за сила в этом камне?";
			link.l1.go = "Claine_19";
		break;

		case "Claine_19":
			dialog.text = "Ого. И тут вы преуспели - даже знаете, кому он принадлежал. А сила... Сила эта велика. Непомерно велика.";
			link.l1 = "Как я понимаю, вы его ещё не нашли?";
			link.l1.go = "Claine_20";
		break;

		case "Claine_20":
			dialog.text = "Верно, но мы уже близко. Только вас, " + GetSexPhrase("дружок", "подруга") + ", пожинать плоды наших поисков мы не позовём.";
			link.l1 = "Я это понял" + GetSexPhrase("", "а") + " ещё в прошлый наш разговор.";
			link.l1.go = "Claine_21";
		break;

		case "Claine_21":
			dialog.text = "А знаете... Мы с Клейном ведь поверили, что вы " + GetSexPhrase("Натан Рич", "действуете по поручению Натана Рича") + ". Джейк Смитт дал нам понять, что мы ошиблись. Врать вы умеете виртуозно - вам бы губернатором быть. Я даже деньги за Фрэнсиса приготовил, но, боюсь, оставлю их себе. Жаль, вы наверняка уже потратили те, что получили за Кромвеля, - я бы и их забрал. И верните письма, что нашли тогда. И дневник. Я знаю, что они у вас.";
			link.l1 = GetSexPhrase("Значит, я не жилец.", "Значит, мне не жить.") + " Тогда хоть перед смертью скажите: кто такой Джейк Смитт?";
			link.l1.go = "Claine_22";
		break;

		case "Claine_22":
			dialog.text = "Он - наше всё. Без него нас бы не было. Ну, довольно разговоров, а то я опоздаю на Мартинику. Советую не сопротивляться - тогда и боли не почувствуете.";
			link.l1 = "Ну-ну. Жди.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithKleinMan");
		break;

		case "Claine_23":
			dialog.text = "Не думал, что вы явитесь. Вы убили одного из лучших наёмников!";
			link.l1 = "Видали и получше.";
			link.l1.go = "Claine_24";
		break;

		case "Claine_24":
			dialog.text = "Погодите, не убивайте! Я могу быть полезен.";
			link.l1 = "Возможно. Смотря что вы знаете.";
			link.l1.go = "Claine_25";
		break;

		case "Claine_25":
			dialog.text = "Что вы хотите знать?";
			link.l1 = "Что вам известно о камне и о Джейке Смитте?";
			link.l1.go = "Claine_26";
		break;

		case "Claine_26":
			dialog.text = "О камне я знаю мало, а Джейк - человек-загадка. Наёмный убийца, на редкость талантливый. Всегда на шаг впереди.";
			link.l1 = "Зачем вы прибыли на Карибы?";
			link.l1.go = "Claine_27";
		break;

		case "Claine_27":
			dialog.text = "Уладить кое-какие дела с испанским губернатором. И найти учёного для Джейка Смитта.";
			link.l1 = "Про учёного подробнее.";
			link.l1.go = "Claine_28";
		break;

		case "Claine_28":
			dialog.text = "Есть тут один баламут. Я его сразу нашёл. Имени даже не запомнил.";
			link.l1 = "Где мне его искать?";
			link.l1.go = "Claine_29";
		break;

		case "Claine_29":
			dialog.text = "Не знаю. Могу сказать, где Джейк, но крайне не советую к нему соваться.";
			link.l1 = "Я не спрашивал" + GetSexPhrase("", "а") + " совета. Где он?";
			link.l1.go = "Claine_30";
		break;

		case "Claine_30":
			dialog.text = "Должен быть где-то на Невисе, на самом острове. Но подумайте, прежде чем плыть. И ещё... не говорите Джейку, что мы виделись. Мне жизнь дорога, у меня семья.";
			link.l1 = "Эх. О семье надо было думать раньше.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneToNevisIsland");
		break;

		case "Jack_Smitt_1":
			dialog.text = "А вы прямо герой, " + GetFullName(pchar) + ". Избавили меня от всех, с кем пришлось бы делиться.";
			link.l1 = "А вы на редкость мерзкий и жадный тип.";
			link.l1.go = "Jack_Smitt_2";
		break;

		case "Jack_Smitt_2":
			dialog.text = "Пришли за ключом?";
			link.l1 = "За ключом?";
			link.l1.go = "Jack_Smitt_3";
		break;

		case "Jack_Smitt_3":
			dialog.text = "Впрочем, какая разница. Обнажайте шпагу, я люблю честный бой.";
			link.l1 = "Расскажешь это Богу. Если встретитесь.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneFightWithJackSmitt");
		break;

		case "Pedro_Carlos_1":
			AddMoneyToCharacter(pchar, 10000);
			dialog.text = "Вы мой спаситель! Этот идиот держал меня силой. Не тревожьтесь: за такое убийство вас только поблагодарят, к тому же он англичанин, а их у нас не жалуют. Я вас даже вознагражу.";
			link.l1 = "Я давно его искал" + GetSexPhrase("", "а") + ". Жаль, разговора не вышло - я о многом хотел" + GetSexPhrase("", "а") + " спросить.";
			link.l1.go = "Pedro_Carlos_2";
		break;

		case "Pedro_Carlos_2":
			dialog.text = "Так спросите у меня. Отвечу обо всём, что знаю.";
			link.l1 = "Я хотел" + GetSexPhrase("", "а") + " узнать про лунный камень.";
			link.l1.go = "Pedro_Carlos_3";
		break;

		case "Pedro_Carlos_3":
			dialog.text = "А-а, тот, что даёт силу Бога... Что ж, могу вас уверить: это выдумка от начала до конца.";
			link.l1 = "Как выдумка? Есть же столько свидетельств!";
			link.l1.go = "Pedro_Carlos_4";
		break;

		case "Pedro_Carlos_4":
			dialog.text = "Может, это и кусок луны - хотя сомнительно. Но силы Бога он не даёт. Вы ведь слышали, как её называют? Легенда о лунном камне призрачного света. А знаете, почему призрачного?";
			link.l1 = "Нет. И почему же?";
			link.l1.go = "Pedro_Carlos_5";
		break;

		case "Pedro_Carlos_5":
			dialog.text = "Смысл в самом названии. Силы в нём столько же, сколько и света, - то есть нисколько. Хотя не исключаю, что кое-что владельцу он всё же даёт. Но точно не силу Бога.";
			link.l1 = "Чёрт... Так что же он даёт?";
			link.l1.go = "Pedro_Carlos_6";
		break;

		case "Pedro_Carlos_6":
			dialog.text = "Не знаю. Может быть, свет, которого нет. Не знаю и знать не хочу - не люблю тратить время впустую.";
			link.l1 = "Погоди. Джейк говорил про какой-то ключ.";
			link.l1.go = "Pedro_Carlos_7";
		break;

		case "Pedro_Carlos_7":
			dialog.text = "Ах да. Джейк говорил о карте. Точнее, о том, где лежит камень. Насколько я понял, где-то здесь, на острове.";
			link.l1 = "Любопытно.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MoonStoneCarlosToExit");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
