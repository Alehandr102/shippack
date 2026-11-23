// Gregg: сёстры Лус и всё, что за ними тянется - «Две сестры», основная ветка.
// Один файл на семерых: Ребекка, зевака у дома, Ханно Гроот, Мерил Крено, Мухачи,
// Мишель и Терри Лорто. Логика - Program\Quests\reaction_functions\twosisters.c.

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
			dialog.text = "Что-то ещё, капитан?";
			link.l1 = "Нет.";
			link.l1.go = "exit";
		break;

		// Бордель Тортуги
		case "1":
			dialog.text = "Капитан, вы не хотите немного подзаработать?";
			link.l1 = "Смотря чем.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Мою сестру похитили. Мишель Лус, её тут знают. Я плачу тому, кто её найдёт.";
			link.l1 = "Ты Ребекка Лус?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Я. Вы меня знаете?";
			link.l1 = "Мне назвали двух сестёр Лус. Я как раз за вами обеими.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "...(настороженно)... За нами? Говорите яснее.";
			link.l1 = "В Сан-Хуане открывают заведение. Нужны четыре женщины. Вы с сестрой - двое из них.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Нам это уже предлагали, и мы отказались. Теперь я согласна на что угодно, лишь бы найти Мишель. Одна я поеду только с ней.";
			link.l1 = "Расскажи всё по порядку.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Три дня назад, ночью, она вышла из дома и не вернулась. Соседка видела, как её тащили к катакомбам.";
			link.l1 = "Кто тащил?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Двое в длинных рясах. Она их описала так: лиц не видно, идут молча.";
			link.l1 = "И никто не пошёл следом?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "В катакомбы? ...(усмехается невесело)... Здесь туда не ходят даже за деньги. Оттуда возвращаются не все и не всегда собой.";
			link.l1 = "Я там был" + GetSexPhrase("", "а") + ".";
			link.l1.go = "9";
			link.l2 = "Что там внизу?";
			link.l2.go = "10";
		break;

		case "9":
			dialog.text = "Значит, знаете. И всё равно спрашиваете, почему за ней не пошли?";
			link.l1 = "Понял" + GetSexPhrase("", "а") + ". Что там внизу?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Старые склепы и кости. Ход к бухте Глаз Рыбы, им пользуются контрабандисты. И эти, в рясах, тоже.";
			link.l1 = "К Мишель кто-нибудь ходил в последнее время?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Ходили. Такие же, в рясах. Она с ними подолгу разговаривала и мне не рассказывала о чём.";
			link.l1 = "Тогда её увели свои.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "...(тихо)... Свои. Хороши свои.";
			link.l1 = "Я спущусь в катакомбы.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Если найдёте её мёртвой - скажите мне сразу. Не жалейте меня, я не выдержу второго ожидания.";
			link.l1 = "Скажу как есть.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Тогда идите, капитан. Я буду здесь.";
			link.l1 = "...(уйти)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToDungeon");
		break;

		// После катакомб
		case "15":
			dialog.text = "...(поднимается навстречу)... Ну? Нашли?";
			link.l1 = "Вот что было у выхода к бухте Глаз Рыбы.";
			link.l1.go = "16";
		break;

		case "16":
			TakeNItems(PChar, "TwoSisters_Medallion", -1);
			dialog.text = "...(берёт медальон)... Её. Она его не снимала. Значит, там её и вели.";
			link.l1 = "Тела рядом не было.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Значит, жива. ...(быстро)... Их корабль ушёл от Глаза Рыбы той же ночью, я спрашивала. Надо узнать чей.";
			link.l1 = "Как ты это узнаешь?";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Обойду всех, кто в ту ночь был на берегу. Мне здесь не откажут, капитан, я тут своя.";
			link.l1 = "Сколько тебе нужно времени?";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Дня три. Ждите в таверне наверху, я приду туда сама.";
			link.l1 = "Три дня у меня есть.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Спасибо, капитан.";
			link.l1 = "Пока не за что.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToTavernRoom");
		break;

		// Комната в таверне, три дня спустя
		case "24":
			dialog.text = "Капитан! Скорее, пойдёмте!";
			link.l1 = "Ты узнала про корабль?";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "Не про корабль. Один из них сидит у меня дома. Я зашла, а он роется в моём сундуке.";
			link.l1 = "И ты его оставила там одного?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "Я заперла дверь снаружи. И людей позвала, там сейчас полулицы стоит.";
			link.l1 = "Идём.";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "Дом за таверной, вверх по улице и налево. Короткая лестница, дальше упрётесь в дверь. Я догоню.";
			link.l1 = "...(идти)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusToTortugaTown");
		break;

		// Зевака у двери
		case "28":
			dialog.text = "...(вполголоса)... Я бы туда не совался, капитан. Двое уже сунулись.";
			link.l1 = "И что с ними?";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "А ничего. Как вошли, так и не выходят. Уж минут десять.";
			link.l1 = "Отойди от двери.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToHouse");
		break;

		// Комната наверху: Ханно Гроот над телом
		case "30":
			dialog.text = "...(тяжело дыша, опираясь на стол)... Всё, готов. Живучий был, зараза.";
			link.l1 = "Кто готов?";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "Да этот, в рясе. Мы с Ольсеном вошли, он от сундука к нам и повернулся. Ольсена сразу, а меня вот - зацепил только.";
			link.l1 = "Ты его убил?";
			link.l1.go = "32";
		break;

		case "32":
			TakeNItems(PChar, "TwoSisters_AnimistMedallion", 1);
			dialog.text = "Убил. ...(протягивает)... Вот, снял с него. Медальон какой-то, тяжёлый. Может, вам на что сгодится, мне уже вряд ли.";
			link.l1 = "Сядь и не двигайся, я осмотрю рану.";
			link.l1.go = "33";
		break;

		case "33":
			TwoSistersLusQuestToRoomSetAnimist();
			dialog.text = "...(смотрит вам за спину)... Капитан. Сзади.";
			link.l1 = "...(обернуться)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestRoomFightWithAnimist");
		break;

		// Пять дней спустя, внизу
		case "34":
			dialog.text = "...(увидев вас)... Встали. А лекарь говорил - неделю пролежите.";
			link.l1 = "Сколько я пролежал" + GetSexPhrase("", "а") + "?";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "Трое суток. Мы вас нашли наверху, рядом с двумя мёртвыми капитанами.";
			link.l1 = "Двумя? А третий?";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "Третьего не было. У входа повалил дым, едкий, как из кузни, а когда рассеялся - в доме только вы и те двое.";
			link.l1 = "Дым видела не ты одна?";
			link.l1.go = "37";
		break;

		case "37":
			dialog.text = "Вся улица видела. И солдаты, которые бежали на крик.";
			link.l1 = "В комнате был человек в рясе. Он лежал на полу с моим клинком в груди.";
			link.l1.go = "38";
		break;

		case "38":
			dialog.text = "...(качает головой)... Не было там никого в рясе. Ни живого, ни мёртвого.";
			link.l1 = "Я снял" + GetSexPhrase("", "а") + " с него вот этот медальон.";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "Медальон был у вас в руке, это правда. А тела не было.";
			link.l1 = "Значит, он ушёл сам.";
			link.l1.go = "40";
			link.l2 = "Значит, кто-то его унёс.";
			link.l2.go = "40";
		break;

		case "40":
			dialog.text = "Или притворился мёртвым и вышел под дымом. Я бы поверила в это, капитан, если бы не одно.";
			link.l1 = "Что?";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "Ханно Гроот бил его дважды. Я слышала снизу. После второго раза так не встают.";
			link.l1 = "...(помолчать)...";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "Уберите эту вещь, капитан. Смотреть на неё не могу.";
			link.l1 = "Про корабль ты так ничего и не узнала?";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "Ничего. Никто в ту ночь ничего не видел, а кто видел - молчит.";
			link.l1 = "Тогда я поеду туда, где про такие медальоны знают.";
			link.l1.go = "44";
		break;

		case "44":
			dialog.text = "Вам бы лежать ещё неделю.";
			link.l1 = "Некогда.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestNeedMedallionInfo");
		break;

		// Дом Мерила Крено в Белизе
		case "56":
			dialog.text = "...(не оборачиваясь)... Наконец-то, Касандер! Я жду вас третьи сутки!";
			link.l1 = "Я не Касандер.";
			link.l1.go = "58";
		break;

		case "58":
			dialog.text = "...(оборачивается)... Вот как. И что вы делаете в моём доме?";
			link.l1 = "Мне сказали, вы знаете, кто носит такие медальоны.";
			link.l1.go = "59";
		break;

		case "59":
			dialog.text = "Кто сказал? Впрочем, неважно. У меня нет времени, я отплываю.";
			link.l1 = "Я заплачу за разговор.";
			link.l1.go = "60";
		break;

		case "60":
			dialog.text = "Деньги у меня есть. ...(машет рукой)... Идите, капитан, идите.";
			link.l1 = "...(показать медальон)...";
			link.l1.go = "62";
		break;

		case "62":
			dialog.text = "...(замолкает и берёт медальон)...";
			link.l1 = "Так знаете или нет?";
			link.l1.go = "63";
		break;

		case "63":
			TwoSistersLusQuestSetCasanderToKrenoHouse();
			dialog.text = "...(медленно)... Знаю. Но не здесь. Расскажу на корабле, по дороге.";
			link.l1 = "На каком корабле?";
			link.l1.go = "64";
		break;

		case "64":
			SetCameraDialogMode(CharacterFromID("Captain Casander"));
			dialog.text = "Капитан Касандер: Прошу прощения, сеньор Крено. Шторм задержал. Мы готовы к отплытию.";
			link.l1 = "...(обернуться)...";
			link.l1.go = "65";
		break;

		case "65":
			SetCameraDialogMode(NPChar);
			dialog.text = "Вот и он. Касандер, с нами идёт капитан " + GetFullName(PChar) + ".";
			link.l1 = "Я не говорил" + GetSexPhrase("", "а") + ", что иду с вами.";
			link.l1.go = "66";
		break;

		case "66":
			locCameraFollow();
			dialog.text = "Пойдёте, если хотите услышать ответ. Корабль в бухте Четумаль, идите через джунгли, там короче. Я догоню.";
			link.l1 = "Буду в бухте.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToBelizShore");
		break;

		// Бухта Четумаль
		case "67":
			dialog.text = "А вот и капитан " + GetFullName(PChar) + ". Идите сюда, шлюпка готова.";
			link.l1 = "...(подойти)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestCasanderShotPrepare");
		break;

		// Трюм «Церебуса»
		case "68":
			dialog.text = "Добро пожаловать на борт, капитан " + GetFullName(PChar) + ". Голова, полагаю, болит?";
			link.l1 = "Ты выстрелил мне в спину.";
			link.l1.go = "69";
		break;

		case "69":
			dialog.text = "Касандер выстрелил. Я всего лишь не стал ему мешать.";
			link.l1 = "За что?";
			link.l1.go = "70";
		break;

		case "70":
			dialog.text = "Не за что, а зачем. За вас дадут выкуп. Владелец колонии - товар недешёвый.";
			link.l1 = "Ты знаешь про медальоны. Расскажи.";
			link.l1.go = "71";
		break;

		case "71":
			dialog.text = "...(усаживается на бочку)... Отчего же не рассказать. Слушать вас всё равно некому.";
			link.l1 = "Слушаю.";
			link.l1.go = "72";
		break;

		case "72":
			dialog.text = "Белиз - мой город. Я в нём вырос и собирался в нём остаться хозяином. Для этого нужны были деньги и люди, и люди нашлись.";
			link.l1 = "Люди в рясах.";
			link.l1.go = "73";
		break;

		case "73":
			dialog.text = "Они самые. Платили хорошо и просили немного: не мешать. Я и не мешал.";
			link.l1 = "Чему именно ты не мешал?";
			link.l1.go = "74";
		break;

		case "74":
			dialog.text = "Тому, что они делают с людьми, которых увозят. Я не спрашивал. Мне довольно было того, что увозят не из Белиза.";
			link.l1 = "Мишель Лус увезли с Тортуги.";
			link.l1.go = "75";
		break;

		case "75":
			dialog.text = "И оказалась она у меня в списке. Знаете, чем она мне не угодила? Ничем. Просто у неё была одна вещица, а вещица оказалась нужна.";
			link.l1 = "Она жива?";
			link.l1.go = "76";
		break;

		case "76":
			dialog.text = "Пока да. Мне нужна была здоровая, а не покойница.";
			link.l1 = "И совесть тебя не мучает.";
			link.l1.go = "77";
		break;

		case "77":
			dialog.text = "Не мучает. Я её не держу, как не держу лишнего балласта.";
			link.l1 = "Куда идёт корабль?";
			link.l1.go = "78";
		break;

		case "78":
			dialog.text = "А вот этого я вам не скажу. ...(поднимается)... Отдохните, капитан. Дорога неблизкая.";
			link.l1 = "Ты вернёшься.";
			link.l1.go = "79";
		break;

		case "79":
			dialog.text = "Разумеется, вернусь. Мне же надо получить за вас деньги.";
			link.l1 = "...(проводить взглядом)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestMainHeroStandUpPrepare");
		break;

		// Вторая стоянка, Крено заходит снова
		case "89":
			dialog.text = "Пришли на место, капитан. Полдня стоянки, и дальше. ...(оглядывает трюм)... А вы, я вижу, не скучали.";
			link.l1 = "Мне тут нечем заняться.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestKrenoExitFromShip");
		break;

		// Мухачи
		case "90":
			dialog.text = "Стой, бледнолицый. Назови своё имя.";
			link.l1 = "" + GetFullName(PChar) + ". Я не с ними.";
			link.l1.go = "91";
		break;

		case "91":
			dialog.text = "Мухачи знает, с кем ты. Мы видели, как ты сош" + GetSexPhrase("ёл", "ла") + " с их корабля " + GetSexPhrase("один", "одна") + " и с оружием в руке.";
			link.l1 = "Кто вы такие?";
			link.l1.go = "92";
		break;

		case "92":
			dialog.text = "Я Мухачи. Это моё племя. Наше селение стоит дальше, за рекой, и о делах племени я с бледнолицыми не говорю.";
			link.l1 = "Тогда о чём говорим?";
			link.l1.go = "93";
		break;

		case "93":
			dialog.text = "О тех, кто пришёл в пещеру Крету. Она наша, а в ней сейчас люди в чёрных рясах.";
			link.l1 = "Их я и ищу.";
			link.l1.go = "94";
		break;

		case "94":
			dialog.text = "Тогда мы идём одной дорогой. Помоги выгнать их, и Мухачи не забудет.";
			link.l1 = "У них моя пленница. Женщина.";
			link.l1.go = "95";
		break;

		case "95":
			dialog.text = "Женщина у них есть, я видел. Живая.";
			link.l1 = "Я соглас" + GetSexPhrase("ен", "на") + ".";
			link.l1.go = "96";
		break;

		case "96":
			TakeNItems(PChar, "TwoSisters_MuhachiAmulet", 1);
			dialog.text = "Тогда возьми это. Носи на себе, пока не кончится бой.";
			link.l1 = "Что это?";
			link.l1.go = "97";
		break;

		case "97":
			dialog.text = "Оберег. Он делает пустыми их медальоны.";
			link.l1 = "Что дают им медальоны?";
			link.l1.go = "98";
		break;

		case "98":
			dialog.text = "Защиту. Тяжело убить того, кто носит такой. И чем дольше носит, тем тяжелее.";
			link.l1 = "Один такой медальон я снял" + GetSexPhrase("", "а") + " с трупа. Он мне ничего не дал.";
			link.l1.go = "99";
		break;

		case "99":
			TwoSistersLusQuestMessageIndsar();
			dialog.text = "Он и не даст. Ты носишь его несколько дней. Тот, у кого ты его взял" + GetSexPhrase("", "а") + ", носил годы.";
			link.l1 = "Значит, можно носить и стать неуязвимым?";
			link.l1.go = "100";
		break;

		case "100":
			dialog.text = "Нет. Медальон берёт плату. Сначала усталость после боя, потом усталость без боя, потом человек не встаёт вовсе.";
			link.l1 = "И они об этом знают?";
			link.l1.go = "101";
		break;

		case "101":
			dialog.text = "Знают. Потому и ищут новых, когда старые перестают вставать.";
			link.l1 = "...(понять)... Вот зачем им люди.";
			link.l1.go = "102";
		break;

		case "102":
			SetCameraDialogMode(NPChar);
			dialog.text = "Индеец: Мухачи! Идут!";
			link.l1 = "Сколько их?";
			link.l1.go = "103";
		break;

		case "103":
			locCameraFollow();
			dialog.text = "Много. Держись рядом с моими людьми, бледнолицый.";
			link.l1 = "Женщину не троньте.";
			link.l1.go = "104";
		break;

		case "104":
			dialog.text = "Женщину мои не тронут. За чужие руки не отвечаю.";
			link.l1 = "...(приготовиться)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestFightInJungle");
		break;

		// После боя
		case "109":
			dialog.text = "Ты хорошо бил" + GetSexPhrase("ся", "ась") + ", бледнолиц" + GetSexPhrase("ый", "ая") + ". Мухачи держит слово.";
			link.l1 = "Мне ничего не нужно.";
			link.l1.go = "110";
			link.l2 = "Что ты обещал?";
			link.l2.go = "110";
		break;

		case "110":
			dialog.text = "Нужно или нет - возьмёшь. У нас лежит вещь, с которой мы не знаем что делать.";
			link.l1 = "Какая вещь?";
			link.l1.go = "111";
		break;

		case "111":
			dialog.text = "Ружьё. Длинное, тяжёлое, ствол внутри с нарезкой. Сняли с бледнолицего, который подошёл к селению слишком близко.";
			link.l1 = "Штуцер.";
			link.l1.go = "112";
		break;

		case "112":
			TakeNItems(PChar, "mushket_Shtuzer", 1);
			dialog.text = "Пусть так. Мои с ним не управятся, а тебе оно пригодится - если умеешь. Оно не для всякой руки.";
			link.l1 = "Умею.";
			link.l1.go = "113";
		break;

		case "113":
			dialog.text = "Тогда бери. И верни мой оберег, бледнолицый.";
			link.l1 = "...(с сожалением)... Оставить нельзя?";
			link.l1.go = "114";
		break;

		case "114":
			TwoSistersLusQuestRemoveAllAmulets();
			dialog.text = "Нельзя. Он не твой, а племени.";
			link.l1 = "Жаль.";
			link.l1.go = "115";
		break;

		case "115":
			TakeNItems(PChar, "MuhachiAmulet", 1);
			dialog.text = "...(усмехается)... Держи вот этот. Слабее, зато без платы. Твоим людям под ядрами будет полегче.";
			link.l1 = "Спасибо, Мухачи.";
			link.l1.go = "116";
		break;

		case "116":
			dialog.text = "И запомни: в пещеру Крету не ходи. Ни один, ни с людьми.";
			link.l1 = "Я и не собирал" + GetSexPhrase("ся", "ась") + ".";
			link.l1.go = "117";
		break;

		case "117":
			dialog.text = "Собирался бы - не сказал. ...(поднимает руку)... Иди, женщина ждёт.";
			link.l1 = "Прощай.";
			link.l1.go = "118";
		break;

		case "118":
			dialog.text = "Не прощай. Мы ещё встретимся, бледнолицый, я это знаю.";
			link.l1 = "...(уйти)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestWaitInJungle");
		break;

		// Мишель Лус
		case "120":
			dialog.text = "...(отступая)... Кто вы такой?";
			link.l1 = "Ты в безопасности. Остальное расскажу на борту, корабль рядом.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestReturnToCerebus");
		break;

		case "121":
			dialog.text = "Так кто вы такой?";
			link.l1 = "Капитан " + GetFullName(PChar) + ". Меня послала твоя сестра.";
			link.l1.go = "122";
		break;

		case "122":
			dialog.text = "Ребекка... ...(садится на землю)... Значит, она меня всё-таки искала.";
			link.l1 = "Три недели искала. За что тебя увезли?";
			link.l1.go = "123";
		break;

		case "123":
			dialog.text = "За вещицу. Медальон, тяжёлый такой, мне его оставил один из них - на хранение, за долг.";
			link.l1 = "И ты его не отдала.";
			link.l1.go = "124";
		break;

		case "124":
			dialog.text = "Там золота на полфунта, а мне предлагали тридцать песо. Конечно, не отдала.";
			link.l1 = "Из-за тридцати песо тебя чуть не убили.";
			link.l1.go = "125";
		break;

		case "125":
			dialog.text = "...(усмехается)... А вы бы продали корабль за пару тысяч, потому что вежливо попросили?";
			link.l1 = "Не продал бы.";
			link.l1.go = "126";
		break;

		case "126":
			dialog.text = "Вот и я. ...(тише)... Только я думала, они уйдут. А они пришли ночью, вчетвером.";
			link.l1 = "Куда тебя везли?";
			link.l1.go = "127";
		break;

		case "127":
			dialog.text = "В пещеру, тут недалеко. Держали в ней три дня, потом появились вы и эти, с копьями.";
			link.l1 = "Идём. Нас ждёт корабль.";
			link.l1.go = "128";
		break;

		case "128":
			dialog.text = "Куда? Только не в Белиз, я вас прошу.";
			link.l1 = "На Тортугу, к сестре. Сперва в Белиз: надо пристроить бриг Касандера и тех, кто на нём остался.";
			link.l1.go = "129";
		break;

		case "129":
			dialog.text = "...(вздыхает)... Ладно. Раз надо в Белиз - значит, в Белиз.";
			link.l1 = "Отдыхай, дорога недолгая.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestReturnToBeliz");
		break;

		// Каюта «Церебуса»
		case "130":
			dialog.text = "...(увидев сестру)... Мишель...";
			link.l1 = "...(отойти)...";
			link.l1.go = "131";
		break;

		case "131":
			dialog.text = "Капитан, я не знаю, чем вам за это платить.";
			link.l1 = "Вы обе едете в Сан-Хуан. Этого хватит.";
			link.l1.go = "132";
		break;

		case "132":
			dialog.text = "Едем. ...(твёрдо)... И сегодня же, пока я не передумала бояться.";
			link.l1 = "Тогда собирайтесь.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestComplete");
		break;

		// Терри Лорто в Белизе - трофейный бриг
		case "133":
			dialog.text = "Капитан! ...(машет рукой)... Терри Лорто, канонир. Касандер держал меня на вёслах вторую неделю.";
			link.l1 = "Ты пришёл на его бриге?";
			link.l1.go = "134";
		break;

		case "134":
			dialog.text = "На нём. Нас на борту осталось семеро, все из таких же. Довели до Белиза и встали на рейде.";
			link.l1 = "Бриг цел?";
			link.l1.go = "135";
		break;

		case "135":
			dialog.text = "Цел, только команды на него нет. Семеро - это не команда, это недоразумение.";
			link.l1 = "Пойдёшь под мою руку? Бриг твой, капитаном на нём будешь ты.";
			link.l1.go = "136";
			link.l2 = "Продай его здесь и раздели деньги с остальными.";
			link.l2.go = "137";
		break;

		case "136":
			dialog.text = "...(выпрямляется)... Пойду, капитан. И эти семеро со мной. Людей доберу здесь же, в Белизе: на рейде голодных хватает, пойдут за долю.";
			link.l1 = "Тогда догоняй.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestCerebusToSquadron");
		break;

		case "137":
			dialog.text = "Как скажете. За такой кораблик дадут тысяч сорок пять, не больше - он потрёпан. Ваша доля вот.";
			link.l1 = "Забирай остальное и живи.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestCerebusToMoney");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
