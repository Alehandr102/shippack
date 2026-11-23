// boal 25/04/04 общий диалог горожан
#include "DIALOGS\russian\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 16/06/06
    int iTest, iTemp;
    iTest = FindColony(NPChar.City); // город
    ref rColony;
    string sTemp, sCity;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
	}

	String attrLoc = Dialog.CurrentNode;

	if(HasSubStr(attrLoc, "RumourAboutOwners_"))
	{
		NPChar.RumourOwnerType = FindStringAfterChar(attrLoc, "_");
		Dialog.CurrentNode = "RumourAboutOwners_TakeRumour";
	}

	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "fight":
            LAi_group_Attack(NPChar, Pchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "First time":


            if(NPChar.quest.meeting == "0")
			{
				NPChar.quest.meeting = "1";

                // homo 05/08/06
				dialog.Text =NPCharRepPhrase(npchar,
                                LinkRandPhrase("Меня зовут ", "Моё имя ", "Можешь называть меня ")
                                +GetFullName(npchar)
                                +LinkRandPhrase(". Чего тебе нужно?", ". Я тебя раньше здесь не "
                                +NPCharSexPhrase(NPChar, "видел", "видела")
                                +", кто ты так"+ GetSexPhrase("ой","ая") +"?", ". Кто ты и что тебе от меня надо?"),

        		                LinkRandPhrase("Приветствую вас, " + GetAddress_Form(NPChar) + ". Моё имя " + GetFullName(npchar) + ". А как зовут вас?",
                                 "Здравствуйте, " + GetAddress_Form(NPChar) + "! Я " + GetFullName(npchar) + ". Могу я узнать ваше имя?",
                                 "Да, " + GetAddress_Form(NPChar) + ". Что вы хотели? Кстати, меня зовут " + GetFullName(npchar) + ". А вас?"));

				Link.l1 = pcharrepphrase(
                                LinkRandPhrase("Разрази меня гром! ", "Тысяча чертей! ", "Чтоб тебе пусто было! ") + "Да я капитан "+ GetFullName(Pchar)+ LinkRandPhrase(", ты разве не слыхал"+NPCharSexPhrase(NPChar,"","а")+" обо мне"+NPCharSexPhrase(NPChar,", каналья?","?"), " и "+ GetSexPhrase("самый известный пират","самая известная женщина-пират") +" в этих водах! ", " и провалиться мне на этом месте, если это не так!"),
                                LinkRandPhrase("Я " + GetFullName(Pchar) + ", капитан.", "Меня зовут " + GetFullName(Pchar) + ".", "Можешь называть меня капитан " + GetFullName(Pchar) + "."));

				Link.l1.go = "Meeting";
				if (pchar.sex == "Skeleton")	//Проверка на скелета
				{
					if	(GetCharacterEquipSuitID(pchar) == "suit_1")
					{
						dialog.text = "Хоть ты и приодел" + GetSexPhrase("ся", "ась") + " в эти лохмотья, но мои старые глаза не обманешь...";
						link.l1 = "Неужели так плоха моя маскировка?",
						link.l1.go = "UndeadObuchenie_2";
					}
					else
					{
						dialog.text = "Судя по твоей нелепой походке, ты не так давно встал" + GetSexPhrase("", "а") + " из могилы. Но можешь не бояться меня, тебя я не обижу.";
						link.l1 = "Удивительно, но это ТЫ должен МЕНЯ бояться, а не наоборот!";
						link.l1.go = "UndeadObuchenie_11";
					}
				}
			}
			else
			{
// Квест смотрителя кладбища "Упокоение умерших" (первой веткой) -->
				if (CheckAttribute(pchar, "GenQuest.GraveyardQuest") && pchar.GenQuest.GraveyardQuest.Town == NPChar.City)
				{
					if (pchar.GenQuest.GraveyardQuest.Stage == "reward")
					{
						link.lgrave = LinkRandPhrase("Я упокоил"+ GetSexPhrase("","а") +" всех мертвецов на твоём кладбище, как ты и просил.", "Дело сделано, старик. На твоём кладбище снова покой.", "И кладбище, и склеп, и крипта очищены от нежити.");
						link.lgrave.go = "GraveyardQuest_Reward";
						// пасхалка: спросить смотрителя про скелета в часовне (только если ГГ уже его видел)
						if (CheckAttribute(pchar, "GenQuest.GraveyardQuest.ChapelSkelMet") && !CheckAttribute(pchar, "GenQuest.GraveyardQuest.SkelAsked"))
						{
							link.lskel = "Постой. В часовне на твоём кладбище сидит говорящий скелет. Откуда он там взялся?";
							link.lskel.go = "GraveyardQuest_AskSkel";
						}
					}
					else
					{
						link.lgrave = LinkRandPhrase("Я ещё не закончил"+ GetSexPhrase("","а") +" с твоим кладбищем.", "Дай мне ещё немного времени, старик.", "Нежить на твоём кладбище ещё не вся упокоена.");
						link.lgrave.go = "GraveyardQuest_Busy";
					}
				}
				else
				{
					if (GraveyardQuest_IsAvailable(NPChar))
					{
						link.lgrave = LinkRandPhrase("Не нужна ли тебе помощь с нежитью на кладбище?", "Старик, я вижу, тебя что-то беспокоит?", "Заходя к тебе в сторожку, я слышал"+ GetSexPhrase("","а") +" шум на кладбище - что там происходит?");
						link.lgrave.go = "GraveyardQuest_Offer";
					}
				}
				// <-- Квест смотрителя кладбища
				link.l1 = PCharRepPhrase("Хотел"+ GetSexPhrase("","а") +" кое-что узнать о городишке по близости!", "Хотел"+ GetSexPhrase("","а") +" узнать кое-что о городе, который здесь недалеко.");
				link.l1.go = "new question";
				// Квестовый генератор священника. Квест №2. Warship -->
				if(CheckAttribute(PChar, "GenQuest.ChurchQuest_2.AskPeople") && !CheckAttribute(NPChar, "GenQuest.ChurchQuest_2.AskedPeople") && NPChar.location == PChar.GenQuest.ChurchQuest_2.QuestTown + "_town")
	            {
					link.l2 = LinkRandPhrase("Ты в курсе", "Ты знаешь", "Ты знаешь что-нибудь о том") + ", что недавно была ограблена местная церковь?";
					link.l2.go = "Citiz_GenQuest_Church_2_1";
				}
				// <-- Квестовый генератор священника. Квест №2.

                dialog.Text = NPCharRepPhrase(npchar,
		                PCharRepPhrase(LinkRandPhrase("А, это ты "+GetFullName(Pchar)
                        +" "+ GetSexPhrase("- старый пройдоха","- отчаянная деваха") +"", "Рад тебя видеть, "+ GetSexPhrase("старина","красавица") +" "
                        + Pchar.name, "О, да это же "+ GetSexPhrase("сам капитан","сама") +" "
                         +GetFullName(Pchar))
                                        +LinkRandPhrase(". Думал, тебя уже нет на этом свете!", ". И, наверное, "+ GetSexPhrase("как всегда пьян.","уже слегка навеселе.") +"", ". И, как вижу, ещё на свободе!")
                                        +LinkRandPhrase(" Ну, говори, чего тебе нужно?", " Чего тебе на этот раз?", " Опять тревожишь меня по пустякам?"),
                                        LinkRandPhrase(TimeGreeting() + ", капитан ", "Здравствуйте, "+GetAddress_Form(NPChar)+" ", "Приветствую вас, капитан ")+GetFullName(Pchar)
                                        +LinkRandPhrase(". Что вас интересует?", ". Зачем вы пришли?", ". Что на этот раз вы хотите узнать?")

                                        ),

                        PCharRepPhrase(LinkRandPhrase("Здравствуйте, капитан ", "Приветствую, ах это вы "+GetAddress_Form(NPChar)+" ", "А, капитан ")
                                        +GetFullName(Pchar)
                                        +LinkRandPhrase(", я думал"+NPCharSexPhrase(NPChar, " ", "а ")+"мы больше с вами не встретимся,",
                                                         ", не скажу, что рад"+NPCharSexPhrase(NPChar, " ", "а ")+"вас видеть, но", ", вижу, вы ещё живы, прискорбно, а")
                                        +LinkRandPhrase(" что вам нужно?", " зачем вы пришли?", " чем могу быть "+NPCharSexPhrase(NPChar, "полезен?", "полезна?")),
                                        LinkRandPhrase(TimeGreeting() + ", капитан " + GetFullName(Pchar) + ". Чем могу помочь?",
                                        "О, да это же сам"+ GetSexPhrase("","а") +" капитан " + GetFullName(Pchar) + "! Что вы хотите узнать на этот раз?",
                                        "Здравствуйте, " + GetAddress_Form(NPChar) + " " + GetHeroLastName() + ". Вы что-то хотели?"))
                                        );
	   			link.l3 = "Ничего, я передумал" + GetSexPhrase(".","а.");
				link.l3.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;
		case "Meeting":
		    dialog.Text  = NPCharRepPhrase(npchar, LinkRandPhrase("А ты не врёшь, капитан " + GetFullName(Pchar)+"?",
                                                                    "И что с того " + PChar.name+"?",
                                                                    "Я запомню тебя, " + GetFullName(Pchar)+".")
                                                                     + " А теперь говори, чего тебе надо?",
                                                    LinkRandPhrase("Приятно познакомиться, капитан " + PChar.name,
                                                                    "Рад"+NPCharSexPhrase(NPChar, " ", "а ")+" нашему знакомству, " + GetAddress_Form(NPChar) + "" + GetHeroLastName(),
                                                                     "Очень приятно, капитан " + PChar.name) + ". Но вряд ли вы просто хотели узнать моё имя, не так ли?");

// Квест смотрителя кладбища "Упокоение умерших" (первой веткой) -->
				if (CheckAttribute(pchar, "GenQuest.GraveyardQuest") && pchar.GenQuest.GraveyardQuest.Town == NPChar.City)
				{
					if (pchar.GenQuest.GraveyardQuest.Stage == "reward")
					{
						link.lgrave = LinkRandPhrase("Я упокоил"+ GetSexPhrase("","а") +" всех мертвецов на твоём кладбище, как ты и просил.", "Дело сделано, старик. На твоём кладбище снова покой.", "И кладбище, и склеп, и крипта очищены от нежити.");
						link.lgrave.go = "GraveyardQuest_Reward";
						// пасхалка: спросить смотрителя про скелета в часовне (только если ГГ уже его видел)
						if (CheckAttribute(pchar, "GenQuest.GraveyardQuest.ChapelSkelMet") && !CheckAttribute(pchar, "GenQuest.GraveyardQuest.SkelAsked"))
						{
							link.lskel = "Постой. В часовне на твоём кладбище сидит говорящий скелет. Откуда он там взялся?";
							link.lskel.go = "GraveyardQuest_AskSkel";
						}
					}
					else
					{
						link.lgrave = LinkRandPhrase("Я ещё не закончил"+ GetSexPhrase("","а") +" с твоим кладбищем.", "Дай мне ещё немного времени, старик.", "Нежить на твоём кладбище ещё не вся упокоена.");
						link.lgrave.go = "GraveyardQuest_Busy";
					}
				}
				else
				{
					if (GraveyardQuest_IsAvailable(NPChar))
					{
						link.lgrave = LinkRandPhrase("Не нужна ли тебе помощь с нежитью на кладбище?", "Старик, я вижу, тебя что-то беспокоит?", "Заходя к тебе в сторожку, я слышал"+ GetSexPhrase("","а") +" шум на кладбище - что там происходит?");
						link.lgrave.go = "GraveyardQuest_Offer";
					}
				}
				// <-- Квест смотрителя кладбища
            link.l1 = PCharRepPhrase("Хотел"+ GetSexPhrase("","а") +" кое-что узнать об этом городишке!", "Хотел"+ GetSexPhrase("","а") +" узнать кое-что о городе.");
			link.l1.go = "new question";
			// Квестовый генератор священника. Квест №2. Warship -->
			if(CheckAttribute(PChar, "GenQuest.ChurchQuest_2.AskPeople") && !CheckAttribute(NPChar, "GenQuest.ChurchQuest_2.AskedPeople") && NPChar.location == PChar.GenQuest.ChurchQuest_2.QuestTown + "_town")
	        {
				link.l2 = LinkRandPhrase("Ты в курсе", "Тебе что-нибудь известно о том", "Ты знаешь что-нибудь о том") + ", что недавно была ограблена местная церковь?";
				link.l2.go = "Citiz_GenQuest_Church_2_1";
			}
			// <-- Квестовый генератор священника. Квест №2.

			link.l3 = "Ничего, я передумал" + GetSexPhrase(".","а.");
			Link.l3.go = "exit";
		break;

		case "new question":
            dialog.text = NPCharRepPhrase(npchar,
		                PCharRepPhrase(LinkRandPhrase("О, да я рад"+NPCharSexPhrase(NPChar, "", "а")+" тебе, как выпивке, спрашивай, чего хочешь.", "Любишь поболтать, капитан? Что ж, я тоже... Особенно за кружечкой рома.", "Да, " + PChar.name + "?"),
                                        LinkRandPhrase("Ну, чего тебе ещё?", "Вижу, капитан "+PChar.name+", ты охотни"+ GetSexPhrase("к","ца") +" поболтать? ", "Замучил"+ GetSexPhrase("","а") +" вопросами, капитан, лучше бы угостил"+ GetSexPhrase("","а") +" ромом.")),
		                PCharRepPhrase(LinkRandPhrase("Что вы хотели узнать "+ GetAddress_Form(NPChar) + "?", "Я вас слушаю капитан.", "Мне нужно идти, поэтому спрашивайте скорее, капитан."),
                                        LinkRandPhrase("Всегда рад"+NPCharSexPhrase(NPChar, "", "а")+" приятному собеседнику, " + GetAddress_Form(NPChar) + " " + GetHeroLastName() + ". Говорите.",
                                                        "Да, " + GetAddress_Form(NPChar) + "?",
                                                        "Любите поболтать, капитан? Что ж, я тоже..."))
                            );

            // homo 25/06/06
			link.l1 = LinkRandPhrase ("Какие байки в ходу в здешней таверне?",
                                    "Что новенького в этих краях?",
                                    "Как течёт жизнь на суше?");
			link.l1.go = "rumours_citizen";
			link.l2 = "Ничего, я передумал" + GetSexPhrase(".","а.");
			link.l2.go = "exit";
		break;

		case "RumourAboutOwners_TakeRumour":
			sTemp = sRumourAboutOwners_CityRumour(NPChar.City, NPChar.RumourOwnerType);
			Dialog.Text = sTemp;
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			// Здесь можно делать проверка по квестам, какой слух выпал
			DeleteAttribute(PChar, "QuestTemp.RumoursAboutOwners");
			break;
		//=================== ноды квеста священника. поиск грабителей (квест №2)==================
		case "Citiz_GenQuest_Church_2_1":
			if(rand(3) != 1)
			{
				dialog.text = LinkRandPhrase("Нет, я ничего об этом не знаю.", "Мне ничего не известно по этому вопросу.", "Я ничего не знаю.");
					link.l1 = LinkRandPhrase("Ладно, хорошо.", "Ясно, спасибо.", "Хорошо, извини.");
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = LinkRandPhrase("Я ничего об этом не знаю, но вы можете расспросить завсегдатаев в таверне - они уж точно что-нибудь вам расскажут.", "Я вам ничего сказать не могу, спросите лучше кого-нибудь в таверне.", "Мне ничего конкретного не известно, но я слышал, что об этом трепались в таверне.");
				link.l1 = "Спасибо и на том.";
				link.l1.go = "exit";
			}

			NPChar.GenQuest.ChurchQuest_2.AskedPeople = true;
		break;

		//замечение по обнажённому оружию
		case "CitizenNotBlade":
			if (loadedLocation.type == "town")
			{
				dialog.text = NPCharSexPhrase(NPChar, "Послушайте, я, как гражданин этого города, прошу вас не ходить у нас с обнажённым клинком.", "Знаете, я, как гражданка этого города, прошу вас не ходить у нас с обнажённым клинком.");
				link.l1 = LinkRandPhrase("Хорошо.", "Ладно.", "Как скажете...");
			}
			else
			{
				dialog.text = NPCharSexPhrase(NPChar, "Острожней на поворотах, "+ GetSexPhrase("приятель","подруга") +", когда бежишь с оружием в руках. Я ведь могу и занервничать...", "Мне не нравится, когда "+ GetSexPhrase("мужчины","всякие тут") +" ходят передо мной с оружием на изготовку. Это меня пугает...");
				link.l1 = RandPhraseSimple("Понял"+ GetSexPhrase("","а") +".", "Убираю.");
			}
			link.l1.go = "exit";
			NextDiag.TempNode = "First Time";
		break;

		case "Man_FackYou":
			dialog.text = LinkRandPhrase("Да ты "+ GetSexPhrase("вор, милейший! Стража, держи его","воровка! Стража, держи её") +"!!!", "Вот это да! Чуть я загляделся, а ты сразу в сундук с головой! Держи "+ GetSexPhrase("вора","воровку") +"!!!", "Стража! Грабят!!! Держи "+ GetSexPhrase("вора","воровку") +"!!!");
			link.l1 = "А-ать, дьявол!!!";
			link.l1.go = "fight";
		break;

		case "fight":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LAi_SetOwnerTypeNoGroup(npchar);
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		
		case "UndeadObuchenie_2":
			dialog.text = "Я прожил на этом кладбище практически всю свою жизнь. И могу с лёгкостью понять, кто передо мной, человек или нежить. Но не одежда выдаёт в тебе нежить, а твоя ПОХОДКА. Посмотри, как ты стоишь - неуверенно, сразу видно, что ты недавно встал" + GetSexPhrase("", "а") + " из могилы. Проще говоря - новенький.";
			link.l1 = "Да, мёртвыми глазами совсем по-другому видишь этот мир. И от этого меня качает из стороны в сторону.",
			link.l1.go = "UndeadObuchenie_3";
		break;
		
		case "UndeadObuchenie_3":
			dialog.text = "Можешь не бояться, тут только я такой глазастый. Остальные жители этого острова даже не заметят подвоха, и будут тебя принимать за своего, то есть за человека. Но стоит тебе снять маскировку, как люди будут настроены к тебе враждебно. Понятно ведь тебе, да?";
			link.l1 = "Это неприятно. Но... А есть ли у меня союзники?",
			link.l1.go = "UndeadObuchenie_13";
		break;
		
		case "UndeadObuchenie_11":
			dialog.text = "Хе-хе, нет! К таким как ты я привык. Даже поддерживаю хорошие отношения с некоторой нежитью. Но я - исключение из правил. Тебе нужно быть осторожнее, так как остальные жители этого острова будут тебя бояться и... будут пытаться уничтожить тебя.";
			link.l1 = "Это неприятно... Но тогда что мне делать?",
			link.l1.go = "UndeadObuchenie_12";
		break;
		
		case "UndeadObuchenie_12":
			dialog.text = "Тебе нужно носить маскировку. Найти какие-нибудь лохмотья и ходить среди людей как вылитый свой.";
			link.l1 = "А есть ли у меня союзники?",
			link.l1.go = "UndeadObuchenie_13";
		break;
		
		case "UndeadObuchenie_13":
			dialog.text = "Конечно, практически любая нежить - твой друг. Можешь поискать своих сородичей в пещерах, они с радостью пойдут к тебе в команду на корабль.";
			link.l1 = "Как же хорошо, что я к тебе заглянул" + GetSexPhrase("", "а") + ". Буду дальше изучать этот мир, в этом проклятом теле. Прощай.",
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		//=================== Квест смотрителя кладбища "Упокоение умерших" ===================
		case "GraveyardQuest_Offer":
			dialog.text = RandPhraseSimple("Ты вовремя. Покоя на моём кладбище больше нет - мертвецы повадились подниматься из могил, бродят меж надгробий, а самые скверные засели в склепе и в крипте под кладбищем. Мне, старику, с ними не совладать. Упокой их всех - и на кладбище, и в склепе, и в крипте, - а я в долгу не останусь.", "Хорошо, что ты "+ GetSexPhrase("зашёл","зашла") +". Беда у меня: мёртвым не лежится в могилах, всю ночь бродят меж надгробий, а в склепе и крипте засело такое, что и помыслить страшно. Стар я уже махать клинком. Упокой нечисть - и на кладбище, и под ним - не пожалеешь.");
			link.l1 = LinkRandPhrase("Я берусь за это. Где искать нежить?", "Я помогу тебе. Куда идти?", "Договорились. Где засела нечисть?");
			link.l1.go = "GraveyardQuest_Accept";
			link.l2 = RandPhraseSimple("Прости, у меня сейчас другие заботы.", "Не сейчас, у меня есть дела поважнее.");
			link.l2.go = "exit";
		break;

		case "GraveyardQuest_Accept":
			dialog.text = RandPhraseSimple("Да прямо здесь, на кладбище. Сначала очисти поверхность от тех, что бродят меж могил. А как управишься - спустись в склеп и в крипту, там засела самая сильная нечисть. Возвращайся ко мне, когда упокоишь всех до единого.", "Здесь, на кладбище, и под ним. Сперва упокой тех, что бродят поверху, затем спустись в склеп и в крипту - там сидят самые сильные. Возвращайся, как закончишь со всеми.");
			link.l1 = LinkRandPhrase("Я разберусь с ними.", "Считай, нежити уже нет.", "Будет сделано, старик.");
			link.l1.go = "exit";
			GraveyardQuest_Start(NPChar);
		break;

		case "GraveyardQuest_Busy":
			dialog.text = RandPhraseSimple("Нежить сама себя не упокоит. Доведи дело до конца - очисти поверхность кладбища, склеп и крипту.", "Рано "+ GetSexPhrase("пришёл","пришла") +". Покуда хоть один мертвец бродит - на кладбище, в склепе или крипте - дело не сделано.");
			link.l1 = RandPhraseSimple("Скоро вернусь.", "Уже занимаюсь этим.");
			link.l1.go = "exit";
		break;

		case "GraveyardQuest_Reward":
			dialog.text = RandPhraseSimple("Наконец-то на кладбище снова тихо. Ты сдержал"+ GetSexPhrase("","а") +" слово, и я сдержу своё. Возьми эти книги - в них немало мудрости, они тебе пригодятся. И прими мою благодарность.", "Снова покой на моём кладбище... Я твой должник. Прими эти книги - в них мудрость, что не купишь за золото. И моя благодарность с тобой.");
			link.l1 = LinkRandPhrase("Благодарю. Если снова полезет нечисть - ты знаешь, где меня искать.", "Рад"+ GetSexPhrase("","а") +" был"+ GetSexPhrase("","а") +" помочь. Бывай, старик.", "Не стоит благодарности. Бывай здоров.");
			link.l1.go = "exit";
			GraveyardQuest_GiveReward(NPChar);
		break;

		// пасхалка: рассказ смотрителя о говорящем скелете в часовне (5 вариантов)
		case "GraveyardQuest_AskSkel":
			switch (rand(4))
			{
				case 0: dialog.text = "А, этот... Да он тут дольше меня сидит, в часовне. Тихий, безобидный - за всю жизнь мухи не обидел, что при жизни, что после. Я его не трогаю, и он меня. Иной раз и поговорить с ним любопытнее, чем с иными живыми."; break;
				case 1: dialog.text = "Это старый смотритель, что был тут до меня. Так прикипел к своему кладбищу, что и смерть его отсюда не выгнала. Поднялся да и сел в часовне - сторожить, как привык. Руки на него не поднимаю: он своё дело знает лучше всякого живого."; break;
				case 2: dialog.text = "Когда нечисть полезла, поднялся и он. Да только он один остался в своём уме - не кинулся убивать, а сел в часовне и заговорил. Жаль мне стало его упокаивать. Пусть сидит, коли никому не вредит."; break;
				case 3: dialog.text = "Монах это, что молился тут ещё при старой часовне. Праведной души был человек, оттого и встал не злым мертвецом, а тихим. Молитвы свои бормочет да байки сказывает. Пускай - кому от того худо?"; break;
				case 4: dialog.text = "Честно скажу - сам не знаю, откуда он. Однажды просто оказался в часовне и заговорил со мной по-людски. С тех пор и живём по соседству. Видать, не всякая смерть превращает человека в чудовище. Иные и костями остаются людьми."; break;
			}
			pchar.GenQuest.GraveyardQuest.SkelAsked = true; // про скелета спросили - больше не предлагать (до след. пасхалки)
			link.l1 = RandPhraseSimple("Занятно. Ну, бывай, старик.", "Вот оно как... Спасибо, что рассказал.");
			link.l1.go = "exit";
		break;
		//=================== <-- Квест смотрителя кладбища ===================
	}
}
