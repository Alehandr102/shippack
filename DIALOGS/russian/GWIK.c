
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	int price = 50000;
	if (CheckOfficersPerk(pchar,"Trader")) price = 10000;
	int uniDays, uniPrice;
	int uniPerDay = 1000 + sti(pchar.rank) * 50; // цена за день: 1000 + уровень ГГ * 50

	// подписка на сводки: месяц с доставкой каждые 5 дней
	int subPrice = 80000;
	if (CheckOfficersPerk(pchar,"Trader")) subPrice = 20000;

	// вексельная служба
	int billFee = 5; // комиссия, %
	if (CheckOfficersPerk(pchar,"Trader")) billFee = 3;
	int billSum, billPay, cash;

	// контракт и аукцион
	int iGood, cQty, cPrice, cCargo, sell, aQty, aPrice, toBuy, added, cn, i, iClass;
	ref rItem;
	string sItemName;
	string sNode = "";
	string sPartnerId = GWIK_PartnerId(NPChar.id); // контора-партнёр (почта/доставка) - следующая открытая по кольцу
	string sPartner = "на Кюрасао";
	if (sPartnerId == "GVIK_char") sPartner = "на Сан-Мартине";
	if (sPartnerId == "FortOrange_GWIK_char") sPartner = "на Ямайке";

	// акционерство Компании: скидки пайщика на услуги контор
	int shareLvl = 0;
	if (CheckAttribute(pchar,"GWIK.Share.Level")) shareLvl = sti(pchar.GWIK.Share.Level);
	int shareDisc = 0;
	if (shareLvl == 1) shareDisc = 10;
	if (shareLvl == 2) shareDisc = 25;
	if (shareLvl == 3) shareDisc = 50;
	int shareCost = 100000; // цена следующего уровня пая
	if (shareLvl == 1) shareCost = 250000;
	if (shareLvl == 2) shareCost = 500000;

	// поручения Компании
	int iJob;
	int jobMailPay = 8000 + sti(pchar.rank) * 800;
	int jobRansom = 20000 + sti(pchar.rank) * 1000;
	int jobRansomPay = 8000 + sti(pchar.rank) * 800;
	int jobRivalPay = 25000 + sti(pchar.rank) * 1500;
	int jobConvoyBonus = 15000 + sti(pchar.rank) * 1000;
	string sJobCity;
	bool ok, bPrimary, bExtra, bElite, bMap, bTube, bTreasure, bMutiny, bDodged;
	if (shareDisc > 0)
	{
		price = makeint(price * (100 - shareDisc) / 100);
		subPrice = makeint(subPrice * (100 - shareDisc) / 100);
		uniPerDay = makeint(uniPerDay * (100 - shareDisc) / 100);
		if (shareLvl >= 2 && billFee > 1) billFee = billFee - 1;
		if (shareLvl >= 3 && billFee > 1) billFee = billFee - 1;
	}
	// дивиденды и поручения растут с уровнем пая
	int shareRate = 2;                  // % дивидендов в месяц: Пайщик
	if (shareLvl == 2) shareRate = 3;   // Партнёр
	if (shareLvl >= 3) shareRate = 4;   // Компаньон
	int jobCooldown = 4 - shareLvl;     // кулдаун поручений: 4 / 3 / 2 / 1 дня
	if (shareLvl > 0)                   // +15% к наградам Компании за каждый уровень пая
	{
		jobMailPay     = makeint(jobMailPay     * (100 + shareLvl * 15) / 100);
		jobRansomPay   = makeint(jobRansomPay   * (100 + shareLvl * 15) / 100);
		jobRivalPay    = makeint(jobRivalPay    * (100 + shareLvl * 15) / 100);
		jobConvoyBonus = makeint(jobConvoyBonus * (100 + shareLvl * 15) / 100);
	}

	// Gregg: диспетчер выбора корабля для выкупа (имя узла TrophySell_<индекс компаньона> -> общий узел Trophy_Do)
	sNode = Dialog.CurrentNode;
	if (findsubstr(sNode, "TrophySell_", 0) != -1)
	{
		i = findsubstr(sNode, "_", 0);
		pchar.GWIK.TrophyPick = strcut(sNode, i + 1, strlen(sNode) - 1);
		Dialog.CurrentNode = "Trophy_Do";
	}
	// Gregg: «неожиданное предложение» - при запросе поручения у Компаньона с репутацией один раз преумножаем обычную ротацию особым квестом про контору на Ямайке
	if (sNode == "Job_Ask" && GWIK_JamaicaEligible() && !CheckAttribute(pchar, "GWIK.Jamaica.Offered"))
	{
		pchar.GWIK.Jamaica.Offered = true;
		Dialog.CurrentNode = "Jamaica_Offer";
	}

	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Приветствую вас. Я - " + GetFullName(NPChar) + ", представитель офиса Голландской Вест-Индской Торговой Компании на острове Сан-Мартин. Представьтесь, пожалуйста.";
			if (NPChar.id == "GWIK_char") dialog.text = "Приветствую вас. Я - " + GetFullName(NPChar) + ", представитель офиса Голландской Вест-Индской Торговой Компании на острове Кюрасао. Представьтесь, пожалуйста.";
			if (NPChar.id == "FortOrange_GWIK_char") // Gregg: управляющий - тот самый фактор; помнит и дорогу, и то, как уладили дело с англичанами
			{
				dialog.text = "Капитан "+ GetFullName(PChar) +"! Так вот вы "+ GetSexPhrase("какой - тот","какая - та") +", кому Компания обязана этой конторой. Я "+ GetFullName(NPChar) +", здешний управляющий. Дело уже на ходу: прайс-листы, векселя, конфискат, поручения - всё как в Виллемстаде, а для "+ GetSexPhrase("основателя","основательницы") +" - по первому разряду.";
				if (CheckAttribute(pchar, "GWIK.Jamaica.Founder"))
				{
					dialog.text = "Капитан "+ GetFullName(PChar) +"! Уж вас-то я не забуду - сам плыл на вашем корабле в эту самую контору. Теперь я здешний управляющий, и для вас, как для "+ GetSexPhrase("основателя","основательницы") +", всё по первому разряду: прайс-листы, векселя, конфискат, поручения.";
					if (CheckAttribute(pchar, "GWIK.Jamaica.Tone"))
					{
						if (pchar.GWIK.Jamaica.Tone == "force") dialog.text = "Капитан "+ GetFullName(PChar) +"! Уж вас-то я не забуду - сам плыл на вашем корабле в эту самую контору. О 'Гончей' Гейла в порту до сих пор шепчутся - после такого урока англичане обходят нас стороной. Для "+ GetSexPhrase("основателя","основательницы") +" всё по первому разряду: прайс-листы, векселя, конфискат, поручения.";
						if (pchar.GWIK.Jamaica.Tone == "gold") dialog.text = "Капитан "+ GetFullName(PChar) +"! Уж вас-то я не забуду - сам плыл на вашем корабле в эту самую контору. Дело тогда уладили тихо: золото Компании умеет молчать, и англичане смотрят в другую сторону. Для "+ GetSexPhrase("основателя","основательницы") +" всё по первому разряду: прайс-листы, векселя, конфискат, поручения.";
						if (pchar.GWIK.Jamaica.Tone == "word") dialog.text = "Капитан "+ GetFullName(PChar) +"! Уж вас-то я не забуду - сам плыл на вашем корабле в эту самую контору. Контора открылась без единого выстрела - ваше имя оказалось крепче пушек. Для "+ GetSexPhrase("основателя","основательницы") +" всё по первому разряду: прайс-листы, векселя, конфискат, поручения.";
					}
				}
			}
			Link.l1 = "Здравствуйте, " + GetFullName(NPChar) + ". Я " + GetFullName(PChar) + ", капитан корабля '" + PChar.ship.name + "'.";
			link.l1.go = "Meet_2";
			if (NPChar.id == "FortOrange_GWIK_char") // основатель - управляющий уже знает игрока, особый ответ
			{
				Link.l1 = "Рад"+ GetSexPhrase("","а") +" видеть Форт Оранж под флагом Компании.";
				link.l1.go = "Meet_2";
			}
		break;

		case "Exit":
			NextDiag.CurrentNode = "Meet_2";
			DialogExit();
		break;

		case "Meet_2":
			GWIK_CheckContract(NPChar);
			GWIK_CheckAuction(NPChar);
			// страховка: если квест-таймер потерялся (старый сейв), перевзводим
			if (shareLvl > 0 && !CheckAttribute(pchar, "quest.GWIK_DividendTick")) SetTimerFunction("GWIK_DividendTick", 0, 1, 0);
			if (CheckAttribute(pchar, "GWIK.Subscription") && !CheckAttribute(pchar, "quest.GWIK_SubscriptionTick")) SetTimerFunction("GWIK_SubscriptionTick", 0, 0, 5);
			if (!CheckAttribute(npchar,"met")) dialog.text = "Приятно познакомиться, капитан. По какому поводу зашли?";
			else dialog.text = "По какому поводу зашли, капитан?";
			if (NPChar.id == "FortOrange_GWIK_char" && CheckAttribute(pchar, "GWIK.Jamaica.Founder")) dialog.text = "Всегда рад вам, капитан. По какому поводу зашли?"; // Gregg: управляющий-фактор давно знаком с игроком - «приятно познакомиться» неуместно
			if (!CheckAttribute(pchar,"GwikTimer") && !CheckAttribute(pchar,"GWIK.Subscription"))
			{
				Link.l1 = "Я бы хотел"+ GetSexPhrase("","а")+" приобрести у вас информацию о ценах в магазинах по архипелагу.";
				link.l1.go = "trade_info";
			}
			if (!CheckAttribute(pchar,"GWIK.Subscription"))
			{
				Link.l4 = "Можно ли получать ваши сводки цен регулярно, с доставкой?";
				link.l4.go = "Subscribe_Ask";
			}
			else
			{
				Link.l4 = "Хочу продлить подписку на сводки цен. (осталось дней: " + pchar.GWIK.Subscription + ")";
				link.l4.go = "Subscribe_Ask";
			}
			Link.l2 = "Хотел"+ GetSexPhrase("","а")+" бы приобрести универсальную торговую лицензию.";
			link.l2.go = "UniLicense_Ask";
			Link.l5 = "Какие товары сейчас закупает Компания?";
			link.l5.go = "Contract_Info";
			Link.l6 = "Что у вас сегодня на распродаже конфиската?";
			link.l6.go = "Auction_Info";
			Link.l7 = "Я хотел"+ GetSexPhrase("","а")+" бы обратиться по финансовому вопросу."
			link.l7.go = "GWIK_Finance";
			Link.l10 = "Есть ли у Компании поручения для меня?";
			link.l10.go = "Job_Ask";
			if (CheckAttribute(pchar, "GWIK.Job"))
			{
				if (pchar.GWIK.Job == "mail" && pchar.GWIK.Job.Target == NPChar.id)
				{
					Link.l11 = "У меня пакет от вашей конторы-партнёра.";
					link.l11.go = "Job_MailDone";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.Rescued"))
				{
					Link.l11 = "Я "+ GetSexPhrase("привёз","привезла") +" вашего капитана, выкупленного у пиратов.";
					if (CheckAttribute(pchar, "GWIK.Job.Mutiny")) Link.l11 = "Я "+ GetSexPhrase("привёз","привезла") +" вашего беглого капитана - силой, в кандалах."; // Gregg: бунт - его не выкупали, а взяли на абордаж
					link.l11.go = "Job_RansomDone";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.CaptiveDeadKnown"))
				{
					Link.l11 = "Дурные вести: вашего капитана убили пираты, спасти его не вышло.";
					link.l11.go = "GWIK_RansomDeadReport";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.MutinyReturnedSelf"))
				{
					Link.l11 = "Ваш беглый капитан образумился - я убедил его вернуться, он уже в пути к вам.";
					link.l11.go = "GWIK_MutinyReturnReport";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.MutinyReportReady"))
				{
					Link.l11 = "Дурные вести: ваш капитан поднял бунт и переметнулся к пиратам.";
					link.l11.go = "GWIK_MutinyReport";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.MutinyKilled"))
				{
					Link.l11 = "Ваш беглый капитан оказал сопротивление - пришлось его зарубить.";
					link.l11.go = "GWIK_MutineerKilledReport";
				}
				if (pchar.GWIK.Job == "rival" && CheckAttribute(pchar, "GWIK.Job.Done"))
				{
					Link.l11 = "Судно ваших конкурентов больше не помеха.";
					if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) Link.l11 = "Человек ваших конкурентов больше не помеха."; // Gregg: наземный вариант - убит на суше, судна не было
					link.l11.go = "Job_RivalDone";
				}
			}
			if (CheckAttribute(pchar, "GWIK.ConvoyBonus.Done"))
			{
				Link.l12 = "Судно Компании доведено до места. Я за премией.";
				link.l12.go = "Job_ConvoyBonus";
			}
			if (GWIK_RepLevel() >= 2)
			{
				Link.l15 = "Не нужен ли Компании трофейный корабль из моей эскадры?";
				link.l15.go = "Trophy_Ask";
			}
			Link.l14 = "Как Компания оценивает наше сотрудничество?";
			link.l14.go = "GWIK_Standing";
				if (GWIK_JamaicaEligible() && CheckAttribute(pchar, "GWIK.Jamaica.Offered")) // Gregg: вернуться к отложенному предложению о конторе на Ямайке
				{
					Link.l18 = "Я обдумал"+ GetSexPhrase("","а") +" предложение о конторе на Ямайке.";
					link.l18.go = "Jamaica_Offer";
				}
			if (NPChar.id == "GVIK_char" && CheckAttribute(pchar,"GWIK.Jamaica.Stage") && sti(pchar.GWIK.Jamaica.Stage) == 4 && !CheckAttribute(pchar,"GWIK.Jamaica.FactorAboard")) // Gregg: забрать фактора для конторы на Ямайке (этап 4) - он ждёт ТОЛЬКО в Мариго
				{
					Link.l19 = "Я за фактором для конторы на Ямайке.";
					link.l19.go = "Jamaica_Factor";
				}
				if (NPChar.id == "GWIK_char" && CheckAttribute(pchar,"GWIK.Jamaica.Stage") && sti(pchar.GWIK.Jamaica.Stage) == 3 && !CheckAttribute(pchar,"GWIK.Jamaica.Convoy") && !CheckAttribute(pchar,"GWIK.Jamaica.ConvoyArrived")) // Gregg: взять конвой снабжения под эскорт (этап 3) - грузится ТОЛЬКО в Кюрасао
				{
					Link.l20 = "Я за конвоем для конторы на Ямайке.";
					link.l20.go = "Jamaica_Convoy";
				}
				if (bBettaTestMode)
			{
				Link.l16 = "[БЕТА] Тестовое меню ГВИК.";
				link.l16.go = "GWIK_BetaMenu";
			}
			Link.l3 = "Просто заш"+ GetSexPhrase("ёл","ла")+" поздороваться, всего доброго.";
			link.l3.go = "exit";
			npchar.met = 1;
		break;

		case "GWIK_Finance": // Gregg: подветка денежных услуг конторы (пай + вексель) - собрано из Meet_2, чтобы разгрузить главное меню
			dialog.text = "Что конкретно вас интересует, капитан?";
			if (shareLvl == 0)
			{
				Link.l1 = "Могу ли я вложиться в дело Компании?";
				link.l1.go = "Share_Ask";
			}
			if (shareLvl > 0 && shareLvl < 3)
			{
				Link.l1 = "Хочу увеличить свой пай в Компании.";
				link.l1.go = "Share_Ask";
			}
			if (shareLvl > 0)
			{
				Link.l2 = "Хочу довложить средства в свой пай.";
				link.l2.go = "Share_Topup";
			}
			if (shareLvl > 0)
			{
				Link.l3 = "Я по поводу моих дивидендов.";
				link.l3.go = "Share_Dividends";
			}
			// Gregg: вексель - последним пунктом: пай и дивиденды это про долю в Компании (постоянные дела), вексель - разовая услуга
			if (!CheckAttribute(pchar,"GWIK.Bill"))
			{
				Link.l4 = "Хочу оформить вексель для вашей конторы-партнёра.";
				link.l4.go = "Bill_Ask";
			}
			else
			{
				if (pchar.GWIK.Bill.From != NPChar.id)
				{
					Link.l4 = "У меня вексель вашей Компании к оплате.";
					link.l4.go = "Bill_Cash";
				}
				else
				{
					Link.l4 = "Напомните, где я могу обналичить свой вексель?";
					link.l4.go = "Bill_Where";
				}
			}
			Link.l5 = "Вернёмся к другим делам.";
			link.l5.go = "Meet_2";
		break;

		case "GWIK_Standing": // Gregg: репутация ГГ в Компании
			iJob = GWIK_RepLevel(); // переиспользуем iJob под уровень репутации
			sJobCity = "случайный клиент";
			if (iJob == 1) sJobCity = "знакомый Компании";
			if (iJob == 2) sJobCity = "доверенное лицо";
			if (iJob == 3) sJobCity = "друг Компании";
			if (iJob == 4) sJobCity = "столп Компании";
			dialog.text = "В наших книгах вы значитесь как "+ sJobCity +". Компания дорожит проверенными людьми: чем выше ваше имя у нас, тем больше дверей открывается - особые лоты, выкуп ваших трофеев, доверительные поручения.";
			link.l1 = "Благодарю. Буду стараться.";
			link.l1.go = "Meet_2";
		break;

		case "GWIK_BetaRep": // [БЕТА] Gregg: мгновенно выставить репутацию под условие квеста Ямайки (rep>=3)
				pchar.GWIK.Rep = 350; // порог уровня 3 ("друг Компании")
				if (!CheckAttribute(pchar, "GWIK.Share")) // Gregg: пай Компаньона автоматом - вручную его теперь быстро не накрутить (квартальные кулдауны)
				{
					pchar.GWIK.Share.Pool = 0;
					pchar.GWIK.Share.Invested = 0;
					pchar.GWIK.Share.Date = GetQuestBookDataDigit();
					SetTimerFunction("GWIK_DividendTick", 0, 1, 0);
				}
				pchar.GWIK.Share.Level = 3;
				if (sti(pchar.GWIK.Share.Invested) < 850000) pchar.GWIK.Share.Invested = 850000; // номинал трёх взносов (100+250+500 тыс)
				dialog.text = "[БЕТА] Выданы репутация «друг Компании» (350, уровень 3) и пай Компаньона (вклад 850000). Для квеста Ямайки остался голландский Форт Оранж.";
				link.l1 = "Понятно.";
				link.l1.go = "GWIK_BetaMenu";
			break;

			case "GWIK_BetaOpenFO": // [БЕТА] Gregg: открыть контору Форт Оранжа без квеста (тест шага 2)
				GWIK_OpenFortOrangeOffice();
				dialog.text = "[БЕТА] Контора в Форт Оранже открыта: локация, дверь и представитель на месте. Плывите на Ямайку - вход через дом с дверью reload_H3.";
				link.l1 = "Поплыли.";
				link.l1.go = "GWIK_BetaMenu";
			break;

			case "GWIK_BetaMenu": // [БЕТА] Gregg: единое тестовое меню ГВИК - условия квестов и запуск генераторов в конкретном варианте
				dialog.text = "[БЕТА] Тестовое меню ГВИК. Что настроить?";
				Link.l1 = "Условия квеста Ямайки: репутация «друг Компании» + пай Компаньона.";
				link.l1.go = "GWIK_BetaRep";
				Link.l2 = "Открыть контору в Форт Оранже (без квеста).";
				link.l2.go = "GWIK_BetaOpenFO";
				Link.l3 = "Поручение «конкурент»: стоит у острова.";
				link.l3.go = "GWIK_BetaRivalStatic";
				Link.l4 = "Поручение «конкурент»: курсирует в море.";
				link.l4.go = "GWIK_BetaRivalMoving";
				Link.l5 = "Поручение «выкуп»: посредник на Бермудах.";
				link.l5.go = "GWIK_BetaRansomAgent";
				Link.l6 = "Поручение «выкуп»: пленник на борту пирата.";
				link.l6.go = "GWIK_BetaRansomPirate";
				Link.l7 = "Поручение «выкуп»: пленника зарезали (провал).";
				link.l7.go = "GWIK_BetaRansomDead";
				Link.l8 = "Поручение «почта»: горячий пакет (перехват на выходе).";
				link.l8.go = "GWIK_BetaMailHot";
				Link.l9 = "Поручение «эскорт»: с засадой у цели.";
				link.l9.go = "GWIK_BetaConvoyAmbush";
				Link.l10 = "Поручение «выкуп»: капитан поднял бунт (переметнулся в пираты).";
					link.l10.go = "GWIK_BetaRansomMutiny";
				Link.l11 = "Поручение «конкурент»: наземный (человек в таверне, дуэль).";
				link.l11.go = "GWIK_BetaRivalLand";
				Link.l12 = "Прислать письмо за следующий ранг (подарок: карта / книга / премия).";
				link.l12.go = "GWIK_BetaRankLetter";
				Link.l13 = "Назад.";
				link.l13.go = "Meet_2";
			break;

			case "GWIK_BetaRankLetter": // [БЕТА] Gregg: письмо за ранг прямо сейчас, минуя порог репутации и трёхдневную доставку. Каждое нажатие - следующий тир, после четвёртого круг начинается заново
				iJob = 0;
				if (CheckAttribute(pchar, "GWIK.RankLetter.Sent")) iJob = sti(pchar.GWIK.RankLetter.Sent);
				iJob = iJob + 1;
				if (iJob > 4) iJob = 1;
				pchar.GWIK.RankLetter.Sent = iJob - 1; // чтобы гард «за этот ранг уже слали» пропустил
				pchar.GWIK.RankLetter.Pending = iJob;
				GWIK_RankLetter("beta");
				dialog.text = "[БЕТА] Письмо за ранг "+ iJob +" доставлено. Смотрите Журнал игры и инвентарь.";
				link.l1 = "Дальше.";
				link.l1.go = "GWIK_BetaMenu";
			break;

			case "GWIK_BetaRivalStatic": // [БЕТА] Gregg: выдать поручение «конкурент» в статичном варианте (минуя drand и кулдаун)
				GWIK_DropRivalJob(); // очистить текущее поручение и старое судно
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRival = "static"; // читается и стирается в GWIK_SpawnRival
				dialog.text = "[БЕТА] Вариант «стоит у острова» задан. Выдаю поручение.";
				link.l1 = "Дальше.";
				link.l1.go = "Job_RivalGo";
			break;

			case "GWIK_BetaRivalMoving": // [БЕТА] Gregg: выдать поручение «конкурент» в морском варианте (минуя drand и кулдаун)
				GWIK_DropRivalJob();
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRival = "moving";
				dialog.text = "[БЕТА] Вариант «курсирует в море» задан. Выдаю поручение.";
				link.l1 = "Дальше.";
				link.l1.go = "Job_RivalGo";
			break;

			case "GWIK_BetaRivalLand": // [БЕТА] Gregg: выдать поручение «конкурент» в наземном варианте (минуя drand и кулдаун)
				GWIK_DropRivalJob();
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRival = "land";
				dialog.text = "[БЕТА] Вариант «человек в таверне» задан. Выдаю поручение. Цель сядет за стол, когда вы войдёте в таверну колонии.";
				link.l1 = "Дальше.";
				link.l1.go = "Job_RivalGo";
			break;

			case "GWIK_BetaRansomAgent": // [БЕТА] Gregg: выдать выкуп через посредника на Бермудах (минуя drand и кулдаун)
				GWIK_FinishRansomJob(); // очистить текущий выкуп (безопасно если нет)
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRansom = "agent"; // читается и стирается в Job_RansomGo
				dialog.text = "[БЕТА] Вариант «посредник на Бермудах» задан. Выдаю поручение.";
				link.l1 = "Дальше.";
				link.l1.go = "Job_RansomGo";
			break;

			case "GWIK_BetaRansomPirate": // [БЕТА] Gregg: выдать выкуп «на борту курсирующего пирата» (минуя drand и кулдаун)
				GWIK_FinishRansomJob();
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRansom = "pirate";
				dialog.text = "[БЕТА] Вариант «на борту пирата» задан. Выдаю поручение.";
				link.l1 = "Дальше.";
				link.l1.go = "Job_RansomGo";
			break;

			case "GWIK_BetaRansomDead": // [БЕТА] Gregg: выдать выкуп с исходом «пленника зарезали» (тест провала, минуя drand и кулдаун)
				GWIK_FinishRansomJob();
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRansom = "dead";
				dialog.text = "[БЕТА] Исход «пленника зарезали» задан. Выдаю поручение (узнаете у посредника на Бермудах).";
				link.l1 = "Дальше.";
				link.l1.go = "Job_RansomGo";
			break;

			case "GWIK_BetaMailHot": // [БЕТА] Gregg: выдать почту с «горячим пакетом» (агент на выходе, минуя drand и кулдаун)
				DeleteAttribute(pchar, "GWIK.Job");
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceMail = "hot"; // читается и стирается в Job_MailGo
				dialog.text = "[БЕТА] «Горячий пакет» задан. Выдаю почту - агент перехватит на выходе.";
				link.l1 = "Дальше.";
				link.l1.go = "Job_MailGo";
			break;

			case "GWIK_BetaConvoyAmbush": // [БЕТА] Gregg: выдать эскорт С ЗАСАДОЙ у острова назначения (минуя drand и кулдаун)
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceConvoy = "ambush"; // читается и стирается в Job_ConvoyGo
				dialog.text = "[БЕТА] Засада задана. Выдаю эскорт - пираты ждут у острова назначения.";
				link.l1 = "Дальше.";
				link.l1.go = "Job_ConvoyGo";
			break;

			case "GWIK_BetaRansomMutiny": // [БЕТА] Gregg: выдать выкуп с твистом «капитан поднял бунт» (минуя drand и кулдаун)
				GWIK_FinishRansomJob(); // очистить текущий выкуп (безопасно если нет)
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRansom = "mutiny"; // читается и стирается в Job_RansomGo
				dialog.text = "[БЕТА] Твист «капитан поднял бунт» задан. Выдаю поручение (узнаете у «посредника» на Бермудах).";
				link.l1 = "Дальше.";
				link.l1.go = "Job_RansomGo";
			break;

			case "Trophy_Ask": // Gregg: выкуп Компанией трофейного корабля эскадры (rep>=2)
			if (CheckAttribute(pchar, "GWIK.TrophyDate") && GetNpcQuestPastDayParam(pchar, "GWIK.TrophyDate") < sti(pchar.GWIK.TrophyCD))
			{
				dialog.text = "Компания недавно уже выкупила у вас судно, капитан. Казна не бездонна - загляните позже.";
				link.l1 = "Понимаю. Зайду позже.";
				link.l1.go = "Meet_2";
				break;
			}
			bMap = false; // переиспользуем как «нашёлся ли подходящий корабль»
			for (i = 1; i < COMPANION_MAX; i++)
			{
				cn = GetCompanionIndex(pchar, i);
				if (cn < 0) continue;
				rItem = GetCharacter(cn);
				if (!GetRemovable(rItem)) continue;
				if (!GetShipRemovable(rItem)) continue;
				aPrice = GWIK_TrophyPrice(rItem);
				sNode = "l" + i;
				link.(sNode) = XI_ConvertString(RealShips[sti(rItem.Ship.Type)].BaseName) + " (кап. " + GetFullName(rItem) + ") - " + FindRussianMoneyString(aPrice);
				link.(sNode).go = "TrophySell_" + cn;
				bMap = true;
			}
			if (!bMap)
			{
				dialog.text = "В вашей эскадре нет судна, что Компания взяла бы в казну, капитан.";
				link.l1 = "Ясно.";
				link.l1.go = "Meet_2";
				break;
			}
			dialog.text = "Компания готова выкупить трофейное судно - за часть цены, разумеется, за вычетом издержек. Какое предлагаете?";
			link.l99 = "Передумал"+ GetSexPhrase("","а") +".";
			link.l99.go = "Meet_2";
		break;

		case "Trophy_Do": // Gregg: сама сделка (узел-цель диспетчера TrophySell_<idx>)
			cn = sti(pchar.GWIK.TrophyPick);
			if (cn < 0)
			{
				dialog.text = "Что-то не так с этим судном, капитан. Давайте сначала.";
				link.l1 = "Хорошо.";
				link.l1.go = "Trophy_Ask";
				break;
			}
			rItem = GetCharacter(cn);
			if (!IsCompanion(rItem))
			{
				dialog.text = "Этого судна уже нет в вашей эскадре, капитан.";
				link.l1 = "Хм, и правда.";
				link.l1.go = "Meet_2";
				break;
			}
			aPrice = GWIK_TrophyPrice(rItem);
			sItemName = XI_ConvertString(RealShips[sti(rItem.Ship.Type)].BaseName);
			iClass = GetCharacterShipClass(rItem); // Gregg: класс трофея берём ДО продажи - GWIK_SellTrophyShip обнуляет тип судна (иначе вернётся 7)
			if (iClass < 1) iClass = 1;
			if (iClass > 7) iClass = 7;
			if (!GWIK_SellTrophyShip(rItem))
			{
				dialog.text = XI_ConvertString("SeaTowOperationsDetach");
				link.l1 = XI_ConvertString("OK");
				link.l1.go = "Trophy_Ask";
				break;
			}
			AddMoneyToCharacter(pchar, aPrice);
			GWIK_AddRep(10);
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, (7 - iClass + 1) * 25); // Gregg: опыт лидерства и торговли за сделку с Компанией; обратная зависимость от класса (кл.1 -> 175, кл.7 -> 25)
			AddCharacterExpToSkill(pchar, SKILL_COMMERCE,   (7 - iClass + 1) * 25);
			pchar.GWIK.TrophyCD = 14 + drand(7); // Gregg: КД 14..21 дней (дневной рандом)
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.TrophyDate");
			PlayStereoSound("notebook");
			dialog.text = sItemName +" переходит Компании - а вот ваши "+ FindRussianMoneyString(aPrice) +". Приятно иметь с вами дело, капитан.";
			link.l1 = "Благодарю. До свидания!";
			link.l1.go = "exit";
		break;

		case "Share_Ask":
			if (shareLvl > 0 && CheckAttribute(pchar, "GWIK.Share.LevelDate") && GetNpcQuestPastDayParam(pchar, "GWIK.Share.LevelDate") < 90) // Gregg: расширение доли - не чаще раза в квартал (как довложение)
			{
				dialog.text = "Вы недавно расширяли свою долю, капитан. Компания оформляет новый пай не чаще раза в квартал - загляните позже.";
				link.l1 = "Хорошо, зайду позже.";
				link.l1.go = "Meet_2";
				break;
			}
			if (shareLvl == 0)
			{
				dialog.text = "Компания всегда рада видеть надёжных капитанов среди своих пайщиков. Пай стоит "+ FindRussianMoneyString(shareCost) +" и даёт скидку 10% на все услуги наших контор, а также ежемесячные дивиденды - 2% от вложенной суммы. Со временем долю можно будет увеличить. Вступаете?";
			}
			if (shareLvl == 1)
			{
				dialog.text = "Расширение пая до партнёрского обойдётся ещё в "+ FindRussianMoneyString(shareCost) +". Партнёру Компания даёт скидку 25% на услуги контор, пониженную комиссию по векселям и второй лот на каждой распродаже конфиската. А дивиденды вырастут до 3% в месяц - и уже с куда большей суммы. Оформляем?";
			}
			if (shareLvl == 2)
			{
				dialog.text = "Доля компаньона - это ещё "+ FindRussianMoneyString(shareCost) +". Взамен - половинные цены на все услуги контор, минимальная комиссия по векселям, целых 4% дивидендов в месяц со всей вложенной суммы и самые выгодные условия по нашим поручениям. По рукам?";
			}
			if (sti(pchar.money) >= shareCost)
			{
				link.l1 = "Да, вот деньги.";
				link.l1.go = "Share_Buy";
			}
			else
			{
				link.l1 = "Хм, столько у меня при себе нет...";
				link.l1.go = "Meet_2";
			}
			link.l2 = "Нет, я не готов"+ GetSexPhrase("","а")+" расстаться с такой суммой.";
			link.l2.go = "Meet_2";
		break;

		case "Share_Buy":
			if (shareLvl > 0 && CheckAttribute(pchar, "GWIK.Share.LevelDate") && GetNpcQuestPastDayParam(pchar, "GWIK.Share.LevelDate") < 90) // Gregg: страховка кулдауна расширения
			{
				dialog.text = "Не так быстро, капитан: новый пай Компания оформляет не чаще раза в квартал.";
				link.l1 = "Понял"+ GetSexPhrase("","а") +".";
				link.l1.go = "Meet_2";
				break;
			}
			if (sti(pchar.money) < shareCost)
			{
				dialog.text = "У вас не хватает денег, капитан.";
				link.l1 = "Жаль.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -shareCost);
			if (shareLvl == 0)
			{
				pchar.GWIK.Share.Pool = 0;
				pchar.GWIK.Share.Invested = 0;
				pchar.GWIK.Share.Date = GetQuestBookDataDigit();
				SetTimerFunction("GWIK_DividendTick", 0, 1, 0);
			}
			pchar.GWIK.Share.Level = shareLvl + 1;
			pchar.GWIK.Share.Invested = sti(pchar.GWIK.Share.Invested) + shareCost;
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Share.LevelDate"); // Gregg: отсчёт квартального кулдауна расширения
			GWIK_AddRep(35); // Gregg: репутация за вступление/расширение пая
			PlayStereoSound("notebook");
			if (shareLvl == 0)
			{
				dialog.text = "Добро пожаловать в число пайщиков Компании! Бумаги уже оформлены и внесены в ваш судовой журнал. Дивиденды начисляются раз в месяц - получить их можно в любой нашей конторе.";
			}
			else
			{
				dialog.text = "Поздравляю с расширением доли! Бумаги переоформлены, новые условия действуют с этой минуты.";
			}
			link.l1 = "Приятно иметь с вами дело. До свидания!";
			link.l1.go = "exit";
		break;

		case "Share_Dividends":
			cash = 0;
			if (CheckAttribute(pchar,"GWIK.Share.Pool")) cash = sti(pchar.GWIK.Share.Pool);
			if (cash > 0)
			{
				AddMoneyToCharacter(pchar, cash);
				pchar.GWIK.Share.Pool = 0;
				PlayStereoSound("notebook");
				dialog.text = "С прошлой выплаты вам начислено "+ FindRussianMoneyString(cash) +". Вот они, всё до монеты. Компания благодарит за доверие!";
			}
			else
			{
				dialog.text = "Пока ничего не накопилось, капитан. Начисление идёт раз в месяц - "+ shareRate +"% от вложенной суммы. Как только курьер известит вас, заходите в любую контору.";
			}
			link.l1 = "Благодарю. До свидания!";
			link.l1.go = "exit";
			link.l2 = "Что-нибудь ещё?";
			link.l2.go = "Meet_2";
		break;

		case "Share_Topup":
			if (CheckAttribute(pchar, "GWIK.Share.TopupDate") && GetNpcQuestPastDayParam(pchar, "GWIK.Share.TopupDate") < 90)
			{
				dialog.text = "Вы недавно увеличивали пай, капитан. Компания принимает дополнительные вложения не чаще раза в квартал - загляните позже.";
				link.l1 = "Хорошо, зайду позже.";
				link.l1.go = "Meet_2";
				break;
			}
			dialog.text = "Разумеется. Дополнительное вложение увеличит вашу долю, а с ней и дивиденды. Принимаем от 100000 до 200000 пиастров, не чаще раза в квартал. Какую сумму вносите?";
			link.l1.edit = 1;
			link.l1 = "";
			link.l1.go = "Share_TopupCalc";
			link.l2 = "Пожалуй, в другой раз.";
			link.l2.go = "Meet_2";
		break;

		case "Share_TopupCalc":
			cash = sti(dialogEditStrings[1]); // cash используем как сумму вложения
			if (cash < 100000)
			{
				dialog.text = "Дополнительное вложение - не меньше 100000 пиастров, капитан. Назовите другую сумму.";
				link.l1 = "Хорошо, давайте ещё раз.";
				link.l1.go = "Share_Topup";
				link.l2 = "Тогда не нужно.";
				link.l2.go = "Meet_2";
				break;
			}
			if (cash > 200000) cash = 200000; // больше 200000 за раз Компания не принимает
			if (sti(pchar.money) < cash)
			{
				dialog.text = "Столько у вас при себе нет, капитан. Назовите сумму поскромнее.";
				link.l1 = "Хорошо, давайте ещё раз.";
				link.l1.go = "Share_Topup";
				link.l2 = "Тогда не нужно.";
				link.l2.go = "Meet_2";
				break;
			}
			dialog.text = "Вы довносите "+ FindRussianMoneyString(cash) +" в свой пай. Дивиденды будут начисляться уже с увеличенной доли. Оформляем?";
			link.l1 = "Да, вот деньги.";
			link.l1.go = "Share_TopupBuy";
			link.l2 = "Нет, передумал"+ GetSexPhrase("","а")+".";
			link.l2.go = "Meet_2";
		break;

		case "Share_TopupBuy":
			cash = sti(dialogEditStrings[1]);
			if (cash > 200000) cash = 200000;
			if (cash < 100000 || sti(pchar.money) < cash)
			{
				dialog.text = "Что-то не сходится, капитан. Давайте начнём сначала.";
				link.l1 = "Хорошо.";
				link.l1.go = "Share_Topup";
				break;
			}
			AddMoneyToCharacter(pchar, -cash);
			pchar.GWIK.Share.Invested = sti(pchar.GWIK.Share.Invested) + cash;
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Share.TopupDate");
			PlayStereoSound("notebook");
			dialog.text = "Готово! Ваша доля увеличена на "+ FindRussianMoneyString(cash) +". Следующее довложение - не раньше чем через квартал.";
			link.l1 = "Благодарю. До свидания!";
			link.l1.go = "exit";
		break;

		case "Job_Ask":
			// активное поручение - напоминание
			if (CheckAttribute(pchar, "GWIK.Job"))
			{
				if (pchar.GWIK.Job == "mail")
				{
					if (pchar.GWIK.Job.Target == NPChar.id)
					{
						dialog.text = "Так пакет от наших партнёров у вас с собой? Давайте же его сюда!";
						link.l1 = "Да, вот он.";
						link.l1.go = "Job_MailDone";
					}
					else
					{
						dialog.text = "Вы везёте пакет Компании в нашу контору "+ sPartner +". Чем скорее он будет там, тем лучше - двенадцать дней, не забывайте.";
						link.l1 = "Уже в пути.";
						link.l1.go = "Meet_2";
						link.l2 = "Боюсь, я не смогу его доставить. Возвращаю пакет.";
						link.l2.go = "Job_Drop";
					}
					break;
				}
				if (pchar.GWIK.Job == "ransom")
				{
					if (CheckAttribute(pchar, "GWIK.Job.Rescued"))
					{
						dialog.text = "Вы привезли нашего капитана? Передавайте его нам - Компания не забудет этой услуги.";
						link.l1 = "Да, он на моём корабле.";
						link.l1.go = "Job_RansomDone";
					}
					else
					{
						if (CheckAttribute(pchar, "GWIK.Job.Force")) // выбрал силовой путь - шлюп в море
						{
							dialog.text = "Вы предпочли обойтись без выкупа - смело. Пиратский корабль '"+ pchar.GWIK.Job.ForceShipName +"' с нашим капитаном ещё где-то у Бермуд. Верните его нам, и лучше живым.";
							link.l1 = "Приведу.";
							link.l1.go = "Meet_2";
							link.l2 = "Не выходит. Отказываюсь.";
							link.l2.go = "Job_Drop";
						}
						else
						{
							dialog.text = "Наш капитан всё ещё в руках пиратов. Посредник ждёт в таверне пиратского поселения на Бермудах, выкуп - "+ FindRussianMoneyString(jobRansom) +". Компания возместит всё с премией в "+ jobRansomPay +" сверху.";
							link.l1 = "Я помню. Уже иду.";
							link.l1.go = "Meet_2";
							link.l2 = "Это поручение мне не по зубам. Отказываюсь.";
							link.l2.go = "Job_Drop";
						}
					}
					break;
				}
				if (pchar.GWIK.Job == "rival")
				{
					if (CheckAttribute(pchar, "GWIK.Job.Done"))
					{
						dialog.text = "Говорят, конкуренты недосчитались судна... Компания готова рассчитаться.";
						if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) dialog.text = "Говорят, у конкурентов в "+ XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Voc") +" освободилось место... Компания готова рассчитаться.";
						link.l1 = "Да, дело сделано.";
						link.l1.go = "Job_RivalDone";
					}
					else
					{
						dialog.text = "Судно конкурентов - "+ GWIK_RivalShipStr() +" - всё ещё ходит "+ GWIK_JobPlaceStr() +". Сделайте так, чтобы оно не вернулось в порт. И поторопитесь - оно не будет там вечно.";
						if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) dialog.text = "Их человек, "+ pchar.GWIK.Job.RivalName +", всё ещё коротает вечера в таверне "+ XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Gen") +". В море он не выходит, так что искать его надо там. И поторопитесь - в городе он не задержится навечно.";
						if (CheckAttribute(pchar, "GWIK.Job.RivalOutTown")) dialog.text = "Говорят, вы уже позвали его к барьеру. "+ pchar.GWIK.Job.RivalName +" ждёт вас за воротами "+ XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Gen") +" - с клинком и, надо думать, без терпения. Не заставляйте его ждать слишком долго."; // Gregg: дуэль уже назначена - в таверне его больше нет
						link.l1 = "Займусь этим.";
						link.l1.go = "Meet_2";
						link.l2 = "Я передумал"+ GetSexPhrase("","а")+". Отказываюсь от этого дела.";
						link.l2.go = "Job_Drop";
					}
					break;
				}
			}
			// конвой уже ведём?
			if (CheckQuestAttribute("generate_convoy_quest_progress", "begin"))
			{
				dialog.text = "Вы уже сопровождаете судно. Сначала доведите его до места, потом поговорим о новых поручениях.";
				link.l1 = "И то верно.";
				link.l1.go = "Meet_2";
				break;
			}
			// кулдаун
			ok = false;
			if (CheckAttribute(pchar, "GWIK.JobDate") && GetNpcQuestPastDayParam(pchar, "GWIK.JobDate") < jobCooldown) ok = true;
			if (ok)
			{
				dialog.text = "Пока для вас ничего нет, капитан. Загляните через несколько дней - у Компании всегда найдутся дела.";
				link.l1 = "Хорошо, загляну.";
				link.l1.go = "Meet_2";
				break;
			}
			// генерим предложение и держим его до решения
			if (!CheckAttribute(pchar, "GWIK.JobOffer"))
			{
				iJob = drand(3); // Gregg: тип поручения по drand - детерминирован по дню, устойчив к F9
				if (iJob == 0)
				{
					ok = (GetCompanionQuantity(PChar) < COMPANION_MAX);
					if (GetSummonSkillFromName(pchar, SKILL_LEADERSHIP) < 5) ok = false;
					if (sti(Pchar.Ship.Type) == SHIP_NOTUSED) ok = false;
					if (!ok) iJob = 1;
				}
				pchar.GWIK.JobOffer = iJob;
			}
			iJob = sti(pchar.GWIK.JobOffer);
			if (iJob == 0)
			{
				dialog.text = "Есть. Судно Компании с ценным грузом ищет надёжный эскорт до одной из колоний. Капитан оплатит сопровождение по прибытии - сговоритесь с ним сами, - а Компания добавит свою премию: "+ FindRussianMoneyString(jobConvoyBonus) +" в любой из контор. Берётесь?";
				link.l1 = "Берусь. Где капитан?";
				link.l1.go = "Job_ConvoyGo";
			}
			if (iJob == 1)
			{
				dialog.text = "Есть. Нужно доставить пакет деловых бумаг в нашу контору "+ sPartner +" - не позднее чем через двенадцать дней. Компания заплатит "+ FindRussianMoneyString(jobMailPay) +". Берётесь?";
				link.l1 = "Берусь. Давайте пакет.";
				link.l1.go = "Job_MailGo";
			}
			if (iJob == 2)
			{
				dialog.text = "Есть, и дело деликатное. Пираты захватили капитана Компании и требуют выкуп - "+ FindRussianMoneyString(jobRansom) +". Их посредник сидит в таверне пиратского поселения на Бермудах. Выкупите нашего человека и привезите в любую контору - Компания возместит выкуп и добавит "+ jobRansomPay +" премии. Берётесь?";
				link.l1 = "Берусь. Готовьте деньги.";
				link.l1.go = "Job_RansomGo";
			}
			if (iJob == 3)
			{
				dialog.text = "Есть одно дело... не для записи в журналах. Конкуренты Компании зачастили в эти воды и слишком дёшево возят. Компания заплатит "+ FindRussianMoneyString(jobRivalPay) +", если эта помеха исчезнет - а как именно вы её устраните, нас не касается. Берётесь?"; // Gregg: текст НЕЙТРАЛЬНЫЙ - разновидность (судно у острова / судно в море / человек в таверне) катится только в Job_RivalGo
				link.l1 = "Берусь. Назовите место.";
				link.l1.go = "Job_RivalGo";
			}
			link.l2 = "Нет, это не по мне.";
			link.l2.go = "Job_Refuse";
		break;

		case "Job_Refuse":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Что ж, ваше право. Компания найдёт другого исполнителя.";
			link.l1 = "Всего доброго.";
			link.l1.go = "Meet_2";
		break;

		case "Jamaica_Offer": // Gregg: особый квест - открытие конторы в Форт Оранже
			dialog.text = "Капитан, разговор не для чужих ушей. Семнадцать Господ - директора Компании - решили открыть контору на Ямайке, в Форт Оранже, прямо под боком у английского Порт Рояля. Дело тонкое и небыстрое: нужен свой проверенный человек. Компания выбрала вас.";
			link.l1 = "Почему именно Форт Оранж? Расскажите подробнее.";
			link.l1.go = "Jamaica_Offer2";
			link.l2 = "Это честь. Готов"+ GetSexPhrase("","а") +" помочь Компании.";
			link.l2.go = "Jamaica_Accept";
			link.l3 = "Дайте подумать.";
			link.l3.go = "Meet_2";
		break;

		case "Jamaica_Offer2": // Gregg: экспозиция - зачем Компании Ямайка и что с этого игроку
			dialog.text = "Форт Оранж - единственный голландский порт в самом сердце английских вод. Кто держит там склады, тот держит торговлю половины архипелага. Семнадцать Господ готовы вложиться, но действовать открыто нельзя: Порт Рояль задавит дело в колыбели. Нужен человек с именем, кораблём и крепкими нервами. Взамен - доля соучредителя нового отделения и признательность Компании, а она дорогого стоит.";
			link.l1 = "Убедили. Готов"+ GetSexPhrase("","а") +" помочь Компании.";
			link.l1.go = "Jamaica_Accept";
			link.l2 = "Дайте подумать.";
			link.l2.go = "Meet_2";
		break;

		case "Jamaica_Accept":
			GWIK_StartJamaicaQuest();
			dialog.text = "Превосходно. В Форт Оранже вас ждёт наш эмиссар - доверенное лицо Компании, ищите его в резиденции. Он ведёт дело на месте и введёт вас в курс. И помните: об этом - никому лишнего.";
			link.l1 = "Сейчас же отправлюсь.";
			link.l1.go = "exit";
		break;

		case "Jamaica_Factor": // Gregg: фактор (этап 4) входит с улицы и сам подходит представиться (GWIK_Factor.c) - без посредника
			if (!CheckAttribute(pchar, "GWIK.Jamaica.FactorId"))
			{
				GWIK_JamaicaSpawnFactorInOffice();
				dialog.text = "Он ждал этого дня не один месяц, капитан, с утра ходит вокруг конторы... О, а вот и он - лёгок на помине. Дальше он сам всё расскажет.";
				link.l1 = "Вижу. Побеседуем.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Господин "+ pchar.GWIK.Jamaica.FactorLName +" дожидается вас там же, где вы его встретили. Поговорите с ним - и в путь.";
			link.l1 = "Уже иду.";
			link.l1.go = "exit";
		break;

		case "Jamaica_Convoy": // Gregg: приём конвоя снабжения под эскорт в Кюрасао (этап 3)
			if (GetCompanionQuantity(pchar) > COMPANION_MAX - 2)
			{
				dialog.text = "Рад бы, капитан, да два галеона в вашу эскадру не втиснуть - нет места. Освободите пару мест и возвращайтесь.";
				link.l1 = "Освобожу место.";
				link.l1.go = "Meet_2";
				break;
			}
			if (sti(pchar.money) < 50000)
			{
				dialog.text = "Порядок такой: конвой - Компании, а доля учредителя - ваша. Пятьдесят тысяч пиастров в общее дело, и галеоны ваши до Форт Оранжа. Столько при себе найдётся?";
				link.l1 = "Соберу деньги.";
				link.l1.go = "Meet_2";
				break;
			}
			dialog.text = "Два галеона гружены под завязку: красное дерево, сандал, тюки шёлка - на добрую контору хватит. Ваша доля учредителя - пятьдесят тысяч в дело, и я передаю конвой под вашу руку. Ведите его в Форт Оранж и берегите: англичане уже что-то пронюхали про этот груз.";
			link.l1 = "По рукам. Готовьте галеоны.";
			link.l1.go = "Jamaica_Convoy_Go";
			link.l2 = "Дайте подумать.";
			link.l2.go = "Meet_2";
		break;

		case "Jamaica_Convoy_Go":
			if (!GWIK_JamaicaSpawnConvoy()) // не удалось (нет свободных слотов) - не тупик
			{
				dialog.text = "Хм, в вашей эскадре так и не нашлось места для галеонов. Освободите пару мест и возвращайтесь.";
				link.l1 = "Освобожу место.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -50000);
			PlayStereoSound("notebook");
			AddQuestRecord("GWIK_JamaicaJob", "15");
			dialog.text = "С Богом, капитан. Галеоны выйдут в море следом за вами - держите их под пушками. В Форт Оранже вас встретит эмиссар. Доброго ветра!";
			link.l1 = "Идём в Форт Оранж.";
			link.l1.go = "exit";
		break;

		case "Job_ConvoyGo":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			sJobCity = findTraderCity_GWIK(NPChar);
			if (sJobCity == "none")
			{
				dialog.text = "Хм, похоже, наш капитан уже отчалил. Не повезло - загляните в другой раз.";
				link.l1 = "Бывает...";
				link.l1.go = "Meet_2";
				break;
			}
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			pchar.quest.destination = sJobCity;
			pchar.ConvoyQuest.City = NPChar.city;
			DeleteAttribute(pchar, "GWIK.ConvoyBonus"); // Gregg: чистим прошлый узел вместе с .Done - иначе премия за ещё не выполненный конвой
			pchar.GWIK.ConvoyBonus = jobConvoyBonus; // премия Компании сверх платы торговца
			AddDialogExitQuest("prepare_for_convoy_quest");
			ReOpenQuestHeader("GWIK_ConvoyJob"); // Gregg: журнал эскорта Компании
			AddQuestRecord("GWIK_ConvoyJob", "1");
			AddQuestUserData("GWIK_ConvoyJob", "sCity", XI_ConvertString("Colony" + sJobCity + "Gen")); // Gregg: запись говорит «до <города>» - родительный
			AddQuestUserData("GWIK_ConvoyJob", "sMoney", FindRussianMoneyString(jobConvoyBonus));
			// Gregg: РЕДКИЙ шанс засады (~25%) у острова назначения (свой поток drand1); эскорт и так ловит случайные встречи карты, поэтому засада редкая. Бета форсит/запрещает
			bMap = false;
			if (drand1(3) == 0) bMap = true;
			if (CheckAttribute(pchar, "GWIK.BetaForceConvoy")) { bMap = false; if (pchar.GWIK.BetaForceConvoy == "ambush") bMap = true; DeleteAttribute(pchar, "GWIK.BetaForceConvoy"); }
			DeleteAttribute(pchar, "GWIK.ConvoyAmbush"); // сброс от прошлого эскорта
			if (bMap) bMap = GWIK_ConvoySpawnAmbush(sJobCity); // не встала (нет локатора) - эскорт без засады
			dialog.text = "Отлично! А вот и наш капитан - обсудите детали с ним.";
			if (bMap) dialog.text = "Отлично! А вот и наш капитан. Только... поговаривают, у "+ XI_ConvertString("Colony" + sJobCity + "Gen") +" пошаливают пираты. Держите пушки наготове, капитан.";
			link.l1 = "Сейчас переговорю.";
			link.l1.go = "exit";
		break;

		case "Job_MailGo":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			pchar.GWIK.Job = "mail";
			pchar.GWIK.Job.Target = sPartnerId;
			sJobCity = GWIK_OfficeCityById(sPartnerId);
			pchar.GWIK.Job.Pay = jobMailPay;
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Job.Date");
			PlayStereoSound("notebook");
			ReOpenQuestHeader("GWIK_MailJob");
			AddQuestRecord("GWIK_MailJob", "1");
			AddQuestUserData("GWIK_MailJob", "sCity", XI_ConvertString("Colony" + sJobCity + "Voc")); // Gregg: запись говорит «в конторе в <городе>» - предложный
			AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(jobMailPay));
			dialog.text = "Вот пакет, он уже в ваших бумагах. Передайте его лично нашему представителю "+ sPartner +". И помните: печати Компании вскрывать не стоит - у нас длинные руки.";
			link.l1 = "Доставлю в целости. До свидания!";
			link.l1.go = "exit";
			iJob = drand1(2); // Gregg: 1/3 - «горячий пакет»: агент конкурентов перехватит на улице города конторы (свой поток drand1, тип поручения = drand(3))
			if (CheckAttribute(pchar, "GWIK.BetaForceMail")) { iJob = 0; DeleteAttribute(pchar, "GWIK.BetaForceMail"); }
			if (iJob == 0)
			{
				pchar.GWIK.Job.Hot = true;
				pchar.GWIK.Job.HotTown = GWIK_OfficeCityById(NPChar.id) + "_town"; // город текущей конторы - там на улице перехватит агент
				DeleteAttribute(pchar, "quest.GWIK_MailAgent.over");
				pchar.quest.GWIK_MailAgent.win_condition.l1 = "Location";
				pchar.quest.GWIK_MailAgent.win_condition.l1.location = pchar.GWIK.Job.HotTown;
				pchar.quest.GWIK_MailAgent.win_condition = "GWIK_MailAgentArrive";
				pchar.quest.GWIK_MailAgent.function = "GWIK_MailAgentArrive";
			}
		break;

		case "Job_RansomGo":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			pchar.GWIK.Job = "ransom";
			pchar.GWIK.Job.Ransom = jobRansom;
			pchar.GWIK.Job.Bonus = jobRansomPay;
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Job.Date");
			iJob = drand2(1); // Gregg: подвариант выкупа через drand2 (СВОЙ поток! тип поручения - drand(3), морской конкурент - drand1); 0 посредник на Бермудах, 1 пленник на борту курсирующего пирата
			if (CheckAttribute(pchar, "GWIK.BetaForceRansom")) { iJob = 0; if (pchar.GWIK.BetaForceRansom == "pirate") iJob = 1; if (pchar.GWIK.BetaForceRansom == "dead") pchar.GWIK.Job.CaptiveDead = true; if (pchar.GWIK.BetaForceRansom == "mutiny") pchar.GWIK.Job.Mutiny = true; DeleteAttribute(pchar, "GWIK.BetaForceRansom"); }
			if (iJob == 1 && GWIK_RansomForceSpawn("")) // вариант «на борту пирата»: сразу спавним патрулирующего пирата у случайной колонии, минуя посредника
			{
				ReOpenQuestHeader("GWIK_RansomJob");
				AddQuestRecord("GWIK_RansomJob", "9");
				AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName);
				AddQuestUserData("GWIK_RansomJob", "sCap", pchar.GWIK.Job.ForceCapName);
				AddQuestUserData("GWIK_RansomJob", "sCity", XI_ConvertString("Colony" + pchar.GWIK.Job.PirateCity + "Gen"));
				AddQuestUserData("GWIK_RansomJob", "sBonus", FindRussianMoneyString(jobRansomPay));
				dialog.text = "Наш капитан не на Бермудах - его держит на борту пират "+ pchar.GWIK.Job.ForceCapName +", корабль '"+ pchar.GWIK.Job.ForceShipName +"'. Судно рыщет в водах у "+ XI_ConvertString("Colony" + pchar.GWIK.Job.PirateCity + "Gen") +". Перехватите и возьмите на абордаж - Компания добавит премию "+ FindRussianMoneyString(jobRansomPay) +". Но пустите его на дно - утопите и пленника.";
				// Gregg: судно ещё в порту (первые двое суток, GWIK_RansomPutToSea) - говорим как есть, иначе посылаем игрока «в воды» за кораблём, которого там нет
				if (CheckAttribute(pchar, "GWIK.Job.PirateInPort")) dialog.text = "Наш капитан не на Бермудах - его держит на борту пират "+ pchar.GWIK.Job.ForceCapName +", корабль '"+ pchar.GWIK.Job.ForceShipName +"'. Пока оно стоит в порту "+ XI_ConvertString("Colony" + pchar.GWIK.Job.PirateCity + "Gen") +" - догружается, но через день-другой выйдет в море. Перехватите и возьмите на абордаж - Компания добавит премию "+ FindRussianMoneyString(jobRansomPay) +". Но пустите его на дно - утопите и пленника.";
				link.l1 = "Найду и возьму на абордаж.";
				link.l1.go = "exit";
				break;
			}
			GWIK_StartRansomJob();
			if (!CheckAttribute(pchar, "GWIK.Job.Mutiny") && !CheckAttribute(pchar, "GWIK.Job.CaptiveDead")) // Gregg: взаимоисключающие редкие твисты варианта с посредником (если не задано бетой)
			{
				if (drand1(19) == 0) pchar.GWIK.Job.Mutiny = true; // ~1/20 (drand1 - свой поток): пленника нет, капитан сам переметнулся в пираты (узнаешь у «посредника» на Бермудах)
				else if (drand3(9) == 0) pchar.GWIK.Job.CaptiveDead = true; // ~1/10 (drand3 - свой поток): пленника зарежут за строптивость; узнаешь у посредника (провал с мизерной компенсацией)
			}
			ReOpenQuestHeader("GWIK_RansomJob");
			AddQuestRecord("GWIK_RansomJob", "1");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(jobRansom));
			AddQuestUserData("GWIK_RansomJob", "sBonus", FindRussianMoneyString(jobRansomPay));
			dialog.text = "Запомните: таверна пиратского поселения на Бермудах, выкуп - "+ FindRussianMoneyString(jobRansom) +", из ваших средств. Компания всё возместит, как только наш человек окажется в любой из контор. Удачи, капитан.";
			link.l1 = "Скоро ваш капитан будет здесь.";
			link.l1.go = "exit";
		break;

		case "Job_RivalGo":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			sJobCity = findRivalCity_GWIK(NPChar);
			pchar.GWIK.Job = "rival";
			ok = false;
			if (sJobCity != "none") ok = GWIK_SpawnRival(sJobCity);
			if (!ok)
			{
				// не нашлось подходящего места - заменим поручением с почтой
				pchar.GWIK.Job = "mail";
				pchar.GWIK.Job.Target = sPartnerId;
				sJobCity = GWIK_OfficeCityById(sPartnerId);
				pchar.GWIK.Job.Pay = jobMailPay;
				SaveCurrentNpcQuestDateParam(pchar, "GWIK.Job.Date");
				PlayStereoSound("notebook");
				ReOpenQuestHeader("GWIK_MailJob");
				AddQuestRecord("GWIK_MailJob", "1");
				AddQuestUserData("GWIK_MailJob", "sCity", XI_ConvertString("Colony" + sJobCity + "Voc")); // Gregg: та же запись #1 при подмене поручения - предложный
				AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(jobMailPay));
				dialog.text = "Хм... обстоятельства изменились, судно конкурентов уже ушло. Но раз вы здесь - доставьте пакет в нашу контору "+ sPartner +", оплата "+ FindRussianMoneyString(jobMailPay) +". Пакет уже в ваших бумагах.";
				link.l1 = "Договорились.";
				link.l1.go = "exit";
				break;
			}
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Job.Date");
			pchar.GWIK.Job.Pay = jobRivalPay;
			ReOpenQuestHeader("GWIK_RivalJob");
			if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) // Gregg: наземный вариант - цель не судно, а человек в таверне колонии
			{
				AddQuestRecord("GWIK_RivalJob", "6");
				AddQuestUserData("GWIK_RivalJob", "sCity", XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Voc")); // Gregg: запись #6 говорит «в <городе> обосновался» - предложный (в записи #1 та же переменная идёт после «в водах», там родительный)
				AddQuestUserData("GWIK_RivalJob", "sName", pchar.GWIK.Job.RivalName);
				AddQuestUserData("GWIK_RivalJob", "sMoney", FindRussianMoneyString(jobRivalPay));
				dialog.text = "На этот раз дело не в море. Их человек, "+ pchar.GWIK.Job.RivalName +", сидит в таверне "+ XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Gen") +" и оттуда перебивает нам все цены. Сам в море не выходит - осторожен. Дальнейшее на ваше усмотрение, Компания вопросов задавать не будет.";
				// Gregg: подсказка про грамоту - иначе приз за обыск тела никто не найдёт. Атрибут ставится в GWIK_SpawnRivalLand только если своей лицензии у игрока нет
				if (CheckAttribute(pchar, "GWIK.Job.RivalLicence")) dialog.text = dialog.text + " И вот что любопытно: торгует он по нашей же грамоте - выправил через третьи руки. Бумага при нём. Если она к нам не вернётся, Компания переживёт.";
				link.l1 = "Найду его.";
				link.l1.go = "exit";
				break;
			}
			AddQuestRecord("GWIK_RivalJob", "1");
			AddQuestUserData("GWIK_RivalJob", "sCity", XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Gen"));
			AddQuestUserData("GWIK_RivalJob", "sShip", GWIK_RivalShipStr()); // Gregg: тип + имя корабля
			AddQuestUserData("GWIK_RivalJob", "sMoney", FindRussianMoneyString(jobRivalPay));
			dialog.text = "Наши люди донесли: судно конкурентов - "+ GWIK_RivalShipStr() +" - сейчас "+ GWIK_JobPlaceStr() +". Дальнейшее - на ваше усмотрение, Компания вопросов задавать не будет. И поторопитесь: долго оно там не простоит.";
			link.l1 = "Считайте, что оно уже на дне.";
			link.l1.go = "exit";
		break;

		case "Job_Drop":
			if (pchar.GWIK.Job == "mail")
			{
				GWIK_MailAgentCleanup(); // Gregg: убрать агента «горячего пакета», если был
				AddQuestRecord("GWIK_MailJob", "4");
				CloseQuestHeader("GWIK_MailJob");
			}
			if (pchar.GWIK.Job == "ransom")
			{
				AddQuestRecord("GWIK_RansomJob", "4");
				CloseQuestHeader("GWIK_RansomJob");
				GWIK_FinishRansomJob();
			}
			if (pchar.GWIK.Job == "rival")
			{
				AddQuestRecord("GWIK_RivalJob", "5");
				CloseQuestHeader("GWIK_RivalJob");
				GWIK_DropRivalJob();
			}
			DeleteAttribute(pchar, "GWIK.Job"); // для mail; для остальных уже вычищено
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Жаль. Компания предпочитает капитанов, которые доводят дело до конца.";
			link.l1 = "Обстоятельства сильнее меня. До свидания.";
			link.l1.go = "exit";
		break;

		case "Job_MailDone":
			ok = (GetNpcQuestPastDayParam(pchar, "GWIK.Job.Date") <= 12);
			cash = jobMailPay;
			if (CheckAttribute(pchar, "GWIK.Job.Pay")) cash = sti(pchar.GWIK.Job.Pay); // сумма, обещанная при выдаче
			// Gregg: ушёл от перехвата и донёс пакет, ни разу не поговорив с агентом - это полноценный исход, а не «контент не сработал».
			// Флаги читаем ДО уборки: GWIK.Job сносится ниже. Раздельными if - сложное присваивание с CheckAttribute Storm не компилит
			bDodged = false;
			if (CheckAttribute(pchar, "GWIK.Job.HotDodge")) bDodged = true;
			if (CheckAttribute(pchar, "GWIK.Job.HotResolved")) bDodged = false; // разговор всё-таки состоялся - обычный исход
			GWIK_MailAgentCleanup(); // Gregg: убрать агента «горячего пакета», если завис (ДО удаления GWIK.Job - читает MailAgentId)
			DeleteAttribute(pchar, "GWIK.Job");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			PlayStereoSound("notebook");
			if (ok)
			{
				AddMoneyToCharacter(pchar, cash);
				GWIK_AddRep(10); // Gregg: репутация за почту в срок
				AddCharacterExpToSkill(pchar, SKILL_SAILING, 40);    // Gregg: опыт навигации за курьерский рейс, выполненный в срок
				AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 15); // Gregg: и немного командования за исполненное поручение Компании
				AddQuestRecord("GWIK_MailJob", "2");
				AddQuestUserData("GWIK_MailJob", "sCity", XI_ConvertString("Colony" + NPChar.city + "Voc")); // Gregg: запись #2 говорит «контора в <городе>» - предложный
				AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(cash));
				CloseQuestHeader("GWIK_MailJob");
				dialog.text = "Печати целы, сроки соблюдены - приятно иметь дело с профессионалом. Вот ваши "+ FindRussianMoneyString(cash) +".";
			}
			else
			{
				AddMoneyToCharacter(pchar, makeint(cash / 2));
				AddQuestRecord("GWIK_MailJob", "3");
				AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(makeint(cash / 2)));
				CloseQuestHeader("GWIK_MailJob");
				dialog.text = "Печати целы, но вы припозднились, капитан. Компания платит половину - "+ FindRussianMoneyString(makeint(cash / 2)) +". В следующий раз будьте расторопнее.";
			}
			if (bDodged) // Gregg: пакет несли «горячим», но от людей конкурентов ушли - Компания об этом знает и ценит
			{
				GWIK_AddRep(3);
				ReOpenQuestHeader("GWIK_MailJob"); // дописать в уже закрытый заголовок можно (как письмо Семнадцати Господ)
				AddQuestRecord("GWIK_MailJob", "9");
				CloseQuestHeader("GWIK_MailJob");
				dialog.text = dialog.text + " И вот что, капитан: об этом пакете расспрашивали люди, к Компании отношения не имеющие. Вы, я вижу, не стали с ними знакомиться - это к лучшему.";
			}
			link.l1 = "Благодарю. До свидания!";
			link.l1.go = "exit";
		break;

		case "Job_RansomDone":
			cash = sti(pchar.GWIK.Job.Ransom) + jobRansomPay;
			if (CheckAttribute(pchar, "GWIK.Job.Bonus")) cash = sti(pchar.GWIK.Job.Ransom) + sti(pchar.GWIK.Job.Bonus); // суммы, обещанные при выдаче
			ok = CheckAttribute(pchar, "GWIK.Job.Force"); // силой отбит - выкуп не платили, только премия с надбавкой
			bMutiny = CheckAttribute(pchar, "GWIK.Job.Mutiny"); // Gregg: снять флаг ДО GWIK_FinishRansomJob (он удалит GWIK.Job), иначе мятежный текст доставки ниже не сработает
			if (ok)
			{
				cash = jobRansomPay;
				if (CheckAttribute(pchar, "GWIK.Job.Bonus")) cash = sti(pchar.GWIK.Job.Bonus);
				cash = makeint(cash * 125 / 100); // +25% за риск боя
			}
			AddMoneyToCharacter(pchar, cash);
			GWIK_FinishRansomJob();
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			PlayStereoSound("notebook");
			GWIK_AddRep(13); // Gregg: репутация за доставленного капитана
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 30); // Gregg: опыт командования за возвращённого капитана (боевой XP силового пути движок начислил в море)
			if (bMutiny) AddQuestRecord("GWIK_RansomJob", "21"); // Gregg: бунт - приволок беглеца силой, выкупа не было; своя запись без «возместили выкуп»
			else AddQuestRecord("GWIK_RansomJob", "3");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			dialog.text = "Вы вернули Компании одного из лучших капитанов! Вот возмещение выкупа и обещанная премия - всего "+ FindRussianMoneyString(cash) +". Компания не забывает таких услуг.";
			if (ok) dialog.text = "Вырвали нашего капитана у пиратов силой, без всякого выкупа? Дерзко - и достойно! За отвагу Компания платит обещанную премию с надбавкой - всего "+ FindRussianMoneyString(cash) +". Компания ценит таких людей.";
			if (ok && bMutiny) dialog.text = "Приволокли нашего беглеца в кандалах? Что ж, предателя стоило проучить. За отвагу - премия с надбавкой, всего "+ FindRussianMoneyString(cash) +". А с ним Компания теперь разберётся сама.";
			link.l1 = "Рад"+ GetSexPhrase("","а") +" был"+ GetSexPhrase("","а") +" помочь. До свидания!";
			link.l1.go = "exit";
		break;

		case "GWIK_RansomDeadReport": // Gregg: доложить о гибели пленника - платит КОМПАНИЯ (не пират), мизерная компенсация за хлопоты, без штрафа репутации
			cash = 2000 + sti(pchar.rank) * 200;
			AddMoneyToCharacter(pchar, cash);
			AddQuestRecord("GWIK_RansomJob", "11");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			GWIK_FinishRansomJob(); // убрать посредника + GWIK.Job (говорим с представителем конторы, не с посредником - безопасно)
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Печально это слышать. Что ж, пираты есть пираты - вашей вины тут нет, капитан. Вот немного за хлопоты, "+ FindRussianMoneyString(cash) +". Компания помнит тех, кто старался.";
			link.l1 = "Благодарю. До свидания.";
			link.l1.go = "exit";
		break;

		case "GWIK_MutinyReturnReport": // Gregg: беглец вернулся в Компанию сам (успешное убеждение) - положительный доклад: премия + репутация, без выкупа и боя
			cash = jobRansomPay;
			if (CheckAttribute(pchar, "GWIK.Job.Bonus")) cash = sti(pchar.GWIK.Job.Bonus);
			AddMoneyToCharacter(pchar, cash);
			GWIK_FinishRansomJob(); // убрать беглеца из таверны + GWIK.Job (говорим с представителем конторы, не с ним - безопасно)
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			PlayStereoSound("notebook");
			GWIK_AddRep(13); // Gregg: репутация за возвращённого капитана
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 30); // Gregg: опыт командования за то, что образумил беглеца
			AddQuestRecord("GWIK_RansomJob", "17");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			dialog.text = "А, так это вас благодарить? Наш капитан и вправду явился с повинной - сказал, вы его образумили. Компания закрывает на его выходку глаза, а вам - обещанная премия, "+ FindRussianMoneyString(cash) +". Улаживать дела без крови - редкий дар.";
			link.l1 = "Рад"+ GetSexPhrase("","а") +" был"+ GetSexPhrase("","а") +" помочь. До свидания!";
			link.l1.go = "exit";
		break;

		case "GWIK_MutineerKilledReport": // Gregg: беглец оказал сопротивление и зарублен на абордаже - награда за пресечённую измену; пленника нет, без штрафа репутации
			cash = 3000 + sti(pchar.rank) * 300;
			AddMoneyToCharacter(pchar, cash);
			AddQuestRecord("GWIK_RansomJob", "20");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			GWIK_FinishRansomJob();
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			GWIK_AddRep(8); // Gregg: небольшая репутация за пресечённую измену (Компания предпочла бы взять его живым)
			dialog.text = "Зарубили, значит? Крутой нрав... Что ж, крыса, побежавшая к пиратам, лучшего и не заслужила. Мы бы предпочли взять его живым, для острастки - ну да ладно. Вот вам за труды, "+ FindRussianMoneyString(cash) +". На этом дело закрыто.";
			link.l1 = "Благодарю. До свидания.";
			link.l1.go = "exit";
		break;

		case "GWIK_MutinyReport": // Gregg: доложить, что капитан переметнулся в пираты и вернуться отказался - мизерная компенсация за хлопоты, без штрафа репутации
			cash = 2000 + sti(pchar.rank) * 200;
			AddMoneyToCharacter(pchar, cash);
			AddQuestRecord("GWIK_RansomJob", "16");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			GWIK_FinishRansomJob(); // убрать беглеца из таверны + GWIK.Job (говорим с представителем конторы, не с ним - безопасно)
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Переметнулся, значит... Прискорбно, но такое случается - не всякого удержишь. Вашей вины тут нет, капитан. Вот немного за хлопоты, "+ FindRussianMoneyString(cash) +". А с отступником Компания разберётся сама.";
			link.l1 = "Благодарю. До свидания.";
			link.l1.go = "exit";
		break;

		case "Job_RivalDone":
			cash = jobRivalPay;
			if (CheckAttribute(pchar, "GWIK.Job.Pay")) cash = sti(pchar.GWIK.Job.Pay); // сумма, обещанная при выдаче
			AddMoneyToCharacter(pchar, cash);
			pchar.quest.GWIK_RivalDead.over = "yes";
			pchar.quest.GWIK_RivalGone.over = "yes";
			DeleteAttribute(pchar, "GWIK.Job");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			PlayStereoSound("notebook");
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 30); // Gregg: опыт командования за тайную операцию (потопление уже дало боевой XP)
			AddQuestRecord("GWIK_RivalJob", "3"); // данные - строго после записи
			AddQuestUserData("GWIK_RivalJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RivalJob");
			dialog.text = "Компания умеет быть благодарной... и молчаливой. Вот ваши "+ FindRussianMoneyString(cash) +". Этого разговора, разумеется, не было.";
			link.l1 = "Какого разговора? До свидания.";
			link.l1.go = "exit";
		break;

		case "Job_ConvoyBonus":
			cash = sti(pchar.GWIK.ConvoyBonus);
			AddMoneyToCharacter(pchar, cash);
			DeleteAttribute(pchar, "GWIK.ConvoyBonus");
			PlayStereoSound("notebook");
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 25); // Gregg: опыт командования за образцовый эскорт (морские навыки уже дал базовый конвой-квест)
			AddQuestRecord("GWIK_ConvoyJob", "3"); // Gregg: журнал - премия получена
			AddQuestUserData("GWIK_ConvoyJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_ConvoyJob");
			dialog.text = "Наш капитан уже отписался конторе: эскорт был образцовым. Вот обещанная премия Компании - "+ FindRussianMoneyString(cash) +".";
			link.l1 = "С Компанией приятно работать. До свидания!";
			link.l1.go = "exit";
		break;

		case "Subscribe_Ask":
			if (!CheckAttribute(pchar,"GWIK.Subscription"))
			{
				dialog.text = "Разумеется, капитан. Месячная подписка: каждые пять дней наш курьер будет доставлять вам свежую сводку цен по всему архипелагу, где бы вы ни находились. Стоимость - "+ FindRussianMoneyString(subPrice) +", первая сводка - сразу при оформлении. Интересует?";
			}
			else
			{
				dialog.text = "Продление подписки ещё на месяц обойдётся в те же "+ FindRussianMoneyString(subPrice) +". Продлеваем?";
			}
			if (sti(pchar.money) >= subPrice)
			{
				link.l1 = "Да, вот деньги.";
				link.l1.go = "Subscribe_Buy";
			}
			else
			{
				link.l1 = "Хм, столько у меня при себе нет...";
				link.l1.go = "Meet_2";
			}
			link.l2 = "Нет, пожалуй, обойдусь.";
			link.l2.go = "Meet_2";
		break;

		case "Subscribe_Buy":
			AddMoneyToCharacter(pchar, -subPrice);
			PlayStereoSound("notebook");
			if (CheckAttribute(pchar,"GWIK.Subscription"))
			{
				pchar.GWIK.Subscription = sti(pchar.GWIK.Subscription) + 30;
				dialog.text = "Готово, подписка продлена. Курьеры Компании не заставят себя ждать.";
			}
			else
			{
				pchar.GWIK.Subscription = 30;
				SetTimerFunction("GWIK_SubscriptionTick", 0, 0, 5);
				GWIK_RefreshAllPrices();
				log_info("Получен список цен во всех магазинах Карибского архипелага.");
				dialog.text = "Оформлено! Вот ваша первая сводка, остальные курьер будет доставлять каждые пять дней. Выгодных сделок, капитан!";
			}
			link.l1 = "Благодарю вас. До свидания!";
			link.l1.go = "exit";
		break;

		case "Contract_Info":
			iGood = sti(NPChar.GWIK.Contract.Good);
			cQty = sti(NPChar.GWIK.Contract.Qty);
			cPrice = sti(NPChar.GWIK.Contract.Price);
			if (cQty <= 0)
			{
				dialog.text = "Текущий закупочный контракт уже исполнен - не без вашей помощи, капитан. Новый список потребностей Компания пришлёт через пару дней.";
				link.l1 = "Загляну позже.";
				link.l1.go = "Meet_2";
				break;
			}
			cCargo = GetSquadronGoods(pchar, iGood); // Gregg: учитываем трюмы всей эскадры, как и при покупке конфиската
			dialog.text = "Компания закупает: "+ XI_ConvertString(Goods[iGood].name) +", "+ cQty +" ед. по "+ FindRussianMoneyString(cPrice) +" за "+ Goods[iGood].Units +" ед. - заметно выше рыночной. В трюмах вашей эскадры сейчас "+ cCargo +" ед. этого товара.";
			if (cCargo > 0)
			{
				link.l1 = "Отлично, я сдам товар прямо сейчас.";
				link.l1.go = "Contract_Sell";
			}
			link.l2 = "Учту. Благодарю за информацию.";
			link.l2.go = "Meet_2";
		break;

		case "Contract_Sell":
			iGood = sti(NPChar.GWIK.Contract.Good);
			cQty = sti(NPChar.GWIK.Contract.Qty);
			cPrice = sti(NPChar.GWIK.Contract.Price);
			cCargo = GetSquadronGoods(pchar, iGood); // Gregg: суммарно по эскадре
			sell = cCargo;
			if (sell > cQty) sell = cQty;
			if (sell <= 0)
			{
				dialog.text = "Хм, в трюмах вашей эскадры не нашлось ни единицы нужного товара, капитан.";
				link.l1 = "Видимо, ошибся. Прошу прощения.";
				link.l1.go = "Meet_2";
				break;
			}
			GWIK_RemoveGoodsSquadron(pchar, iGood, sell); // Gregg: снимаем каскадом - флагман, затем корабли эскадры
			AddMoneyToCharacter(pchar, GWIK_GoodsMoney(iGood, sell, cPrice));
			NPChar.GWIK.Contract.Qty = cQty - sell;
			PlayStereoSound("notebook");
			if (cQty - sell <= 0)
			{
				SaveCurrentNpcQuestDateParam(NPChar, "GWIK.ContractDate"); // отсчёт до нового контракта
				GWIK_AddRep(10); // Gregg: репутация за полностью закрытый контракт
				AddCharacterExpToSkill(pchar, SKILL_COMMERCE, 40);   // Gregg: опыт торговли за полностью закрытый контракт (сдача прямым AddMoney мимо торгового движка)
				AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 10); // Gregg: и немного командования за исполненное поручение
				dialog.text = "Принято "+ sell +" ед., вот ваши "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, sell, cPrice)) +". Контракт закрыт полностью - Компания ценит таких поставщиков! Новый заказ будет через пару дней.";
			}
			else
			{
				dialog.text = "Принято "+ sell +" ед., вот ваши "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, sell, cPrice)) +". Компания готова выкупить ещё "+ (cQty - sell) +" ед. по той же цене.";
			}
			link.l1 = "С вами приятно иметь дело. До свидания!";
			link.l1.go = "exit";
			link.l2 = "Что-нибудь ещё?";
			link.l2.go = "Meet_2";
		break;

		case "Auction_Info":
			bPrimary = false;
			if (CheckAttribute(NPChar,"GWIK.Auction") && !CheckAttribute(NPChar,"GWIK.Auction.SoldOut")) bPrimary = true;
			bExtra = false;
			if (shareLvl >= 2 && CheckAttribute(NPChar,"GWIK.AuctionExtra") && !CheckAttribute(NPChar,"GWIK.AuctionExtra.SoldOut")) bExtra = true;
			bElite = false; // Gregg: эксклюзив компаньона - редкое ДНЕВНОЕ предложение (drand, устойчив к F9); карта и труба не в один день (один drand(29), разные значения проверки)
			if (shareLvl >= 3 && GetCharacterItem(pchar,"map_full") < 1 && drand(29) == 0) bElite = true;
			if (shareLvl >= 3 && GetCharacterItem(pchar,"spyglass5") < 1 && drand(29) == 15) bElite = true;
			if (!bPrimary && !bExtra && !bElite)
			{
				dialog.text = "Увы, сегодняшний лот уже продан. Конфискат поступает к нам регулярно - загляните через несколько дней.";
				link.l1 = "Непременно загляну.";
				link.l1.go = "Meet_2";
				break;
			}
			if (bPrimary)
			{
				if (NPChar.GWIK.Auction == "goods")
				{
					iGood = sti(NPChar.GWIK.Auction.Good);
					aQty = sti(NPChar.GWIK.Auction.Qty);
					aPrice = sti(NPChar.GWIK.Auction.Price);
					dialog.text = "Сегодня с молотка идёт конфискованная партия: "+ XI_ConvertString(Goods[iGood].name) +", "+ aQty +" ед. по "+ FindRussianMoneyString(aPrice) +" за "+ Goods[iGood].Units +" ед. - много ниже рынка. Целиком партия обойдётся в "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, aQty, aPrice)) +".";
					if (GWIK_GoodsAffordable(iGood, sti(pchar.money), aPrice) > 0)
					{
						link.l1 = "Беру! Сколько влезет в трюм.";
						link.l1.go = "Auction_BuyGoods";
					}
				}
				else
				{
					rItem = ItemsFromID(NPChar.GWIK.Auction.Item);
					aPrice = sti(NPChar.GWIK.Auction.Price);
					sItemName = GetConvertStr(rItem.name, "ItemsDescribe.txt");
					dialog.text = "Сегодня с молотка идёт конфискат из капитанской каюты: "+ sItemName +". Цена - "+ FindRussianMoneyString(aPrice) +", заметно дешевле лавочной.";
					if (sti(pchar.money) >= aPrice)
					{
						link.l1 = "Беру, не глядя!";
						link.l1.go = "Auction_BuyItem";
					}
				}
			}
			else
			{
				if (bExtra) dialog.text = "Основной лот уже ушёл, но для пайщиков Компании припасён ещё один.";
				else dialog.text = "Сегодняшний конфискат уже разобрали, но для компаньонов Компании всегда найдётся кое-что особенное.";
			}
			if (bExtra)
			{
				link.l3 = "А что приготовлено для пайщиков?";
				link.l3.go = "Auction_InfoExtra";
			}
			if (bElite)
			{
				link.l4 = "А что припасено для компаньонов Компании?";
				link.l4.go = "Auction_Elite";
			}
			link.l2 = "Нет, это мне не нужно.";
			link.l2.go = "Meet_2";
		break;

		case "Auction_InfoExtra":
			if (!CheckAttribute(NPChar,"GWIK.AuctionExtra") || CheckAttribute(NPChar,"GWIK.AuctionExtra.SoldOut"))
			{
				dialog.text = "Этот лот уже продан, капитан.";
				link.l1 = "Жаль.";
				link.l1.go = "Meet_2";
				break;
			}
			if (NPChar.GWIK.AuctionExtra == "goods")
			{
				iGood = sti(NPChar.GWIK.AuctionExtra.Good);
				aQty = sti(NPChar.GWIK.AuctionExtra.Qty);
				aPrice = sti(NPChar.GWIK.AuctionExtra.Price);
				dialog.text = "Для пайщиков - ещё одна партия: "+ XI_ConvertString(Goods[iGood].name) +", "+ aQty +" ед. по "+ FindRussianMoneyString(aPrice) +" за "+ Goods[iGood].Units +" ед. Целиком - "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, aQty, aPrice)) +".";
				if (GWIK_GoodsAffordable(iGood, sti(pchar.money), aPrice) > 0)
				{
					link.l1 = "Беру! Сколько влезет в трюм.";
					link.l1.go = "Auction_BuyGoodsExtra";
				}
			}
			else
			{
				rItem = ItemsFromID(NPChar.GWIK.AuctionExtra.Item);
				aPrice = sti(NPChar.GWIK.AuctionExtra.Price);
				sItemName = GetConvertStr(rItem.name, "ItemsDescribe.txt");
				dialog.text = "Для пайщиков - ещё один лот из капитанской каюты: "+ sItemName +". Цена - "+ FindRussianMoneyString(aPrice) +".";
				if (sti(pchar.money) >= aPrice)
				{
					link.l1 = "Беру, не глядя!";
					link.l1.go = "Auction_BuyItemExtra";
				}
			}
			link.l2 = "Нет, это мне не нужно.";
			link.l2.go = "Meet_2";
		break;

		case "Auction_BuyGoods":
			iGood = sti(NPChar.GWIK.Auction.Good);
			aQty = sti(NPChar.GWIK.Auction.Qty);
			aPrice = sti(NPChar.GWIK.Auction.Price);
			toBuy = GWIK_GoodsAffordable(iGood, sti(pchar.money), aPrice); // сколько потянем по деньгам
			if (toBuy > aQty) toBuy = aQty;
			added = GWIK_LoadGoodsSquadron(pchar, iGood, toBuy); // сколько влезло в трюмы ГГ и эскадры
			if (added <= 0)
			{
				dialog.text = "Боюсь, в вашем трюме не осталось места даже для мешка, капитан. Жаль, лот хорош.";
				link.l1 = "И правда, жаль...";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -GWIK_GoodsMoney(iGood, added, aPrice));
			PlayStereoSound("notebook");
			if (added < aQty)
			{
				NPChar.GWIK.Auction.Qty = aQty - added; // Gregg: остаток партии остаётся в продаже, а не сгорает
				dialog.text = "В ваш трюм поместилось "+ added +" ед. - с вас "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, added, aPrice)) +". Остаток партии ("+ (aQty - added) +" ед.) Компания придержит для вас.";
			}
			else
			{
				NPChar.GWIK.Auction.SoldOut = true;
				dialog.text = "Вся партия ваша, капитан: "+ added +" ед. за "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, added, aPrice)) +". Отличная сделка!";
			}
			link.l1 = "Благодарю. До свидания!";
			link.l1.go = "exit";
		break;

		case "Auction_BuyItem":
			aPrice = sti(NPChar.GWIK.Auction.Price);
			if (sti(pchar.money) < aPrice)
			{
				dialog.text = "У вас не хватает денег, капитан.";
				link.l1 = "Жаль.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -aPrice);
			GiveItem2Character(pchar, NPChar.GWIK.Auction.Item);
			NPChar.GWIK.Auction.SoldOut = true;
			PlayStereoSound("notebook");
			dialog.text = "Поздравляю с покупкой! Вещь проверенная - прежний владелец, увы, уже не предъявит претензий.";
			link.l1 = "Благодарю. До свидания!";
			link.l1.go = "exit";
		break;

		case "Auction_BuyGoodsExtra":
			iGood = sti(NPChar.GWIK.AuctionExtra.Good);
			aQty = sti(NPChar.GWIK.AuctionExtra.Qty);
			aPrice = sti(NPChar.GWIK.AuctionExtra.Price);
			toBuy = GWIK_GoodsAffordable(iGood, sti(pchar.money), aPrice); // сколько потянем по деньгам
			if (toBuy > aQty) toBuy = aQty;
			added = GWIK_LoadGoodsSquadron(pchar, iGood, toBuy); // сколько влезло в трюмы ГГ и эскадры
			if (added <= 0)
			{
				dialog.text = "Боюсь, в вашем трюме не осталось места даже для мешка, капитан. Жаль, лот хорош.";
				link.l1 = "И правда, жаль...";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -GWIK_GoodsMoney(iGood, added, aPrice));
			PlayStereoSound("notebook");
			if (added < aQty)
			{
				NPChar.GWIK.AuctionExtra.Qty = aQty - added; // Gregg: остаток партии остаётся в продаже, а не сгорает
				dialog.text = "В ваш трюм поместилось "+ added +" ед. - с вас "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, added, aPrice)) +". Остаток партии ("+ (aQty - added) +" ед.) Компания придержит для вас.";
			}
			else
			{
				NPChar.GWIK.AuctionExtra.SoldOut = true;
				dialog.text = "Вся партия ваша, капитан: "+ added +" ед. за "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, added, aPrice)) +". Отличная сделка!";
			}
			link.l1 = "Благодарю. До свидания!";
			link.l1.go = "exit";
		break;

		case "Auction_BuyItemExtra":
			aPrice = sti(NPChar.GWIK.AuctionExtra.Price);
			if (sti(pchar.money) < aPrice)
			{
				dialog.text = "У вас не хватает денег, капитан.";
				link.l1 = "Жаль.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -aPrice);
			GiveItem2Character(pchar, NPChar.GWIK.AuctionExtra.Item);
			NPChar.GWIK.AuctionExtra.SoldOut = true;
			PlayStereoSound("notebook");
			dialog.text = "Поздравляю с покупкой! Вещь проверенная - прежний владелец, увы, уже не предъявит претензий.";
			link.l1 = "Благодарю. До свидания!";
			link.l1.go = "exit";
		break;

		case "Auction_Elite": // Gregg: эксклюзивные лоты для компаньонов Компании (3 ур. пая)
			bMap = false;
			if (GetCharacterItem(pchar,"Map_Best") < 1 && drand(29) == 0) bMap = true;
			bTube = false;
			if (GetCharacterItem(pchar,"spyglass5") < 1 && drand(29) == 15) bTube = true;
			bTreasure = false;
			if (GetCharacterItem(pchar,"map_full") < 1 && drand(29) == 28) bTreasure = true;
			if (!bMap && !bTube && !bTreasure)
			{
				dialog.text = "Вы уже забрали всё, что Компания держала для своих компаньонов, капитан.";
				link.l1 = "Понятно. До свидания.";
				link.l1.go = "Meet_2";
				break;
			}
			dialog.text = "Для компаньонов Компании - вещи из особых хранилищ, что не выставляют на общий торг:";
			if (bMap)
			{
				link.l1 = "Полная карта архипелага - 500000 пиастров.";
				link.l1.go = "Auction_BuyMap";
			}
			if (bTube)
			{
				link.l2 = "Эксклюзивная подзорная труба - 350000 пиастров.";
				link.l2.go = "Auction_BuyTube";
			}
			if (bTreasure)
			{
				link.l2 = "Карта сокровищ - 30000 пиастров.";
				link.l2.go = "Auction_BuyTreasure";
			}
			link.l3 = "В другой раз.";
			link.l3.go = "Meet_2";
		break;

		case "Auction_BuyMap":
			if (sti(pchar.money) < 500000)
			{
				dialog.text = "Столько у вас при себе нет, капитан. Карта обойдётся в 500000 пиастров.";
				link.l1 = "Жаль.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -500000);
			GiveItem2Character(pchar, "Map_Best");
			PlayStereoSound("notebook");
			dialog.text = "Превосходный выбор! Эту карту составляли лучшие картографы Компании - на ней весь архипелаг до последней банки и мели. Берегите её.";
			link.l1 = "Бесценно. Благодарю!";
			link.l1.go = "exit";
		break;

		case "Auction_BuyTube":
			if (sti(pchar.money) < 350000)
			{
				dialog.text = "Столько у вас при себе нет, капитан. Труба стоит 350000 пиастров.";
				link.l1 = "Жаль.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -350000);
			GiveItem2Character(pchar, "spyglass5");
			PlayStereoSound("notebook");
			dialog.text = "Отменная вещь! Такие трубы делают поштучно - в неё вы разглядите даже модернизации на чужом корабле. Большая редкость.";
			link.l1 = "То, что нужно. Благодарю!";
			link.l1.go = "exit";
		break;
		
		case "Auction_BuyTreasure":
			if (sti(pchar.money) < 30000)
			{
				dialog.text = "Столько у вас при себе нет, капитан. Карта обойдётся в 30000 пиастров.";
				link.l1 = "Жаль.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -30000);
			GiveTreasureMapToPlayer();
			PlayStereoSound("notebook");
			dialog.text = "Превосходный выбор! Надеюсь вас ждёт богатая добыча там, куда ведёт эта карта.";
			link.l1 = "Благодарю!";
			link.l1.go = "exit";
		break;

		case "Bill_Ask":
			dialog.text = "Вексельная служба Компании к вашим услугам. Вы вносите сумму здесь - и получаете вексель на предъявителя, который оплатят в любой другой нашей конторе. Никакого риска возить золото через полные пиратов воды. Комиссия - "+ billFee +"% при оформлении, минимальная сумма - 10000 пиастров. Какую сумму вносим?";
			link.l1.edit = 1;
			link.l1 = "";
			link.l1.go = "Bill_Calc";
			link.l2 = "Я передумал"+ GetSexPhrase("","а")+". В другой раз.";
			link.l2.go = "Meet_2";
		break;

		case "Bill_Calc":
			billSum = sti(dialogEditStrings[1]);
			if (billSum < 10000)
			{
				dialog.text = "Меньше чем на 10000 пиастров вексель не оформляется, капитан. Назовите другую сумму.";
				link.l1 = "Хорошо, давайте ещё раз.";
				link.l1.go = "Bill_Ask";
				link.l2 = "Тогда не нужно.";
				link.l2.go = "Meet_2";
				break;
			}
			if (sti(pchar.money) < billSum)
			{
				dialog.text = "Столько у вас при себе нет, капитан. Назовите сумму поскромнее.";
				link.l1 = "Хорошо, давайте ещё раз.";
				link.l1.go = "Bill_Ask";
				link.l2 = "Тогда не нужно.";
				link.l2.go = "Meet_2";
				break;
			}
			billPay = billSum - makeint(billSum * billFee / 100);
			dialog.text = "Вы вносите "+ FindRussianMoneyString(billSum) +", к выплате в любой другой нашей конторе - "+ FindRussianMoneyString(billPay) +". Оформляем?";
			link.l1 = "Да, вот деньги.";
			link.l1.go = "Bill_Buy";
			link.l2 = "Нет, я передумал"+ GetSexPhrase("","а")+".";
			link.l2.go = "Meet_2";
		break;

		case "Bill_Buy":
			billSum = sti(dialogEditStrings[1]);
			if (billSum < 10000 || sti(pchar.money) < billSum)
			{
				dialog.text = "Что-то не сходится, капитан. Давайте начнём сначала.";
				link.l1 = "Хорошо.";
				link.l1.go = "Bill_Ask";
				break;
			}
			AddMoneyToCharacter(pchar, -billSum);
			pchar.GWIK.Bill = billSum - makeint(billSum * billFee / 100);
			pchar.GWIK.Bill.From = NPChar.id;
			pchar.GWIK.Bill.Date = GetQuestBookDataDigit(); // для печати в судовом журнале
			PlayStereoSound("notebook");
			dialog.text = "Готово! Вексель на "+ FindRussianMoneyString(sti(pchar.GWIK.Bill)) +" - в ваших бумагах, запись внесена в судовой журнал. Предъявите его в любой другой нашей конторе, и вам выплатят всё до монеты.";
			link.l1 = "Благодарю вас. До свидания!";
			link.l1.go = "exit";
		break;

		case "Bill_Cash":
			cash = sti(pchar.GWIK.Bill);
			AddMoneyToCharacter(pchar, cash);
			DeleteAttribute(pchar, "GWIK.Bill");
			PlayStereoSound("notebook");
			dialog.text = "Вексель в полном порядке. Вот ваши "+ FindRussianMoneyString(cash) +", всё до монеты, как и обещала Компания.";
			link.l1 = "С вами приятно иметь дело. До свидания!";
			link.l1.go = "exit";
		break;

		case "Bill_Where":
			dialog.text = "Ваш вексель выписан этой конторой, а оплачен будет в любой другой нашей конторе. Таковы правила Компании - иначе какой смысл в переводе денег?";
			link.l1 = "Точно, благодарю.";
			link.l1.go = "Meet_2";
		break;

		case "UniLicense_Ask":
			dialog.text = "Универсальная торговая лицензия нашей Компании позволяет беспрепятственно входить в города любой нации - даже враждебной вам, - если на вашем корабле поднят флаг той самой нации. Срок вы определяете сами, но не менее десяти дней. Цена - "+ FindRussianMoneyString(uniPerDay) +" за каждый день. На сколько дней оформляем?";
			link.l1.edit = 1;
			link.l1 = "";
			link.l1.go = "UniLicense_Calc";
			link.l2 = "Я передумал"+ GetSexPhrase("","а")+". В другой раз.";
			link.l2.go = "Meet_2";
		break;

		case "UniLicense_Calc":
			uniDays = sti(dialogEditStrings[1]);
			if (uniDays < 10)
			{
				dialog.text = "Простите, капитан, но меньше чем на десять дней лицензию я оформить не могу. Назовите другой срок.";
				link.l1 = "Хорошо, давайте ещё раз.";
				link.l1.go = "UniLicense_Ask";
				link.l2 = "Тогда не нужно.";
				link.l2.go = "Meet_2";
				break;
			}
			if (uniDays > 3650) uniDays = 3650; // Gregg: верхний предел 10 лет - защита от переполнения int (иначе отрицательная цена и начисление денег)
			uniPrice = uniDays * uniPerDay;
			if (sti(pchar.money) >= uniPrice)
			{
				dialog.text = "Лицензия на "+ FindRussianDaysString(uniDays) +" обойдётся вам в "+ FindRussianMoneyString(uniPrice) +". Оформляем?";
				link.l1 = "Да, вот деньги.";
				link.l1.go = "UniLicense_Buy";
			}
			else
			{
				dialog.text = "Лицензия на "+ FindRussianDaysString(uniDays) +" стоила бы "+ FindRussianMoneyString(uniPrice) +", но столько у вас при себе нет, капитан.";
			}
			link.l2 = "Изменю срок.";
			link.l2.go = "UniLicense_Ask";
			link.l3 = "Пожалуй, передумал"+ GetSexPhrase("","а")+".";
			link.l3.go = "Meet_2";
		break;

		case "UniLicense_Buy":
			uniDays = sti(dialogEditStrings[1]);
			if (uniDays < 10) uniDays = 10;
			if (uniDays > 3650) uniDays = 3650; // Gregg: верхний предел 10 лет - защита от переполнения int
			uniPrice = uniDays * uniPerDay;
			if (sti(pchar.money) < uniPrice)
			{
				dialog.text = "У вас не хватает денег на такую лицензию, капитан.";
				link.l1 = "Жаль.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -uniPrice);
			GiveUniLicence(uniDays);
			PlayStereoSound("notebook");
			dialog.text = "Готово! Вот ваша универсальная торговая лицензия сроком на "+ FindRussianDaysString(uniDays) +" - она уже в ваших бумагах. С ней вы вхожи в города любой нации, лишь бы на мачте был её флаг. Удачной торговли, капитан!";
			link.l1 = "Благодарю вас. До свидания!";
			link.l1.go = "exit";
		break;

		case "trade_info":
			dialog.text = "Да, я располагаю такой информацией и могу ей поделиться. С вас "+ FindRussianMoneyString(price) +".";
			if (sti(pchar.money) >= price)
			{
				Link.l1 = "Хорошо, вот ваши деньги.";
				link.l1.go = "trade_info_2";
			}
			Link.l2 = "Пожалуй я передумал"+ GetSexPhrase("","а")+". Всего хорошего.";
			link.l2.go = "exit";
		break;

		case "trade_info_2":
			for (i = 0; i< MAX_COLONIES; i++)
			{
                ref rcolony = &Colonies[i];
			    if (rcolony.id != "Caiman" && rcolony.id != "Panama" && rcolony.nation != "none") SetPriceListByStoreMan(rcolony);
            }
			log_info("Получен список цен во всех магазинах Карибского архипелага.");
			PlayStereoSound("notebook");
			AddMoneyToCharacter(pchar,-price);

			dialog.text = "...Держите, здесь вся нужная вам информация. Желаю успехов!";
			Link.l1 = "Благодарю вас. До свидания!";
			link.l1.go = "exit";
			pchar.GwikTimer = true;
			SetTimerFunction("RefreshGWIK", 0, 0, 15);
		break;
	}
}

string findTraderCity_GWIK(ref NPChar) // колония для эскорта судна Компании (не враждебная Голландии, не текущий остров)
{
	int n;
	int cityArray[MAX_COLONIES];
	int howCity = 0;
	for (n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].id == "Panama") continue;
		if (colonies[n].nation == "none") continue;
		if (GetIslandByCityName(npchar.city) == colonies[n].islandLable) continue;
		if (GetNationRelation(HOLLAND, sti(colonies[n].nation)) == RELATION_ENEMY) continue;
		cityArray[howCity] = n;
		howCity++;
	}
	if (howCity == 0) return "none";
	return colonies[cityArray[cRand(howCity-1)]].id;
}

string findRivalCity_GWIK(ref NPChar) // колония конкурентов (Англия/Франция), не текущий остров
{
	int n;
	int cityArray[MAX_COLONIES];
	int howCity = 0;
	bool okNation;
	for (n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].id == "Panama") continue;
		if (colonies[n].nation == "none") continue;
		if (GetIslandByCityName(npchar.city) == colonies[n].islandLable) continue;
		okNation = (sti(colonies[n].nation) == ENGLAND) || (sti(colonies[n].nation) == FRANCE);
		if (!okNation) continue;
		cityArray[howCity] = n;
		howCity++;
	}
	if (howCity == 0) return "none";
	return colonies[cityArray[cRand(howCity-1)]].id;
}
