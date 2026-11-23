// Посредник пиратов: выкуп пленного капитана ГВИК (поручение конторы)

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iRansom = 0;
	if (CheckAttribute(pchar, "GWIK.Job.Ransom")) iRansom = sti(pchar.GWIK.Job.Ransom);
	if (CheckAttribute(pchar, "GWIK.Job.RansomActual")) iRansom = sti(pchar.GWIK.Job.RansomActual); // цена после торга
	int comSkill, nomRansom, leadSkill;
	bool ok;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			ok = false;
			if (CheckAttribute(pchar, "GWIK.Job") && pchar.GWIK.Job == "ransom") ok = true;
			if (CheckAttribute(pchar, "GWIK.Job.Rescued")) ok = false;
			if (!ok)
			{
				dialog.text = "Чего уставил"+ GetSexPhrase("ся","ась") +"? Иди своей дорогой, пока цел"+ GetSexPhrase("","а") +".";
				link.l1 = "Уже уш"+ GetSexPhrase("ёл","ла") +"...";
				link.l1.go = "exit";
				break;
			}
			if (CheckAttribute(pchar, "GWIK.Job.CaptiveDead")) // Gregg: редкий исход - пленника зарезали. Пират лишь СООБЩАЕТ (денег не даёт - с чего бы ему); мизерную компенсацию за хлопоты выплатит КОМПАНИЯ при докладе в конторе (узел GWIK_RansomDeadReport)
			{
				if (!CheckAttribute(pchar, "GWIK.Job.CaptiveDeadKnown"))
				{
					pchar.GWIK.Job.CaptiveDeadKnown = true; // теперь нужно доложить о гибели в конторе ГВИК
					AddQuestRecord("GWIK_RansomJob", "10");
					dialog.text = "Опоздал ты, голланд" + GetSexPhrase("ец", "ка") + ". Твой капитан оказался больно дерзок на язык - ребята не стерпели и пустили ему кровь. Ступай, обрадуй своих хозяев - с меня взятки гладки.";
				}
				else
				{
					dialog.text = "Я же сказал - твой капитан кормит крабов на дне. Ступай к своим хозяевам, тут тебе больше делать нечего.";
				}
				link.l1 = "Проклятье...";
				link.l1.go = "exit";
				break;
			}
			if (CheckAttribute(pchar, "GWIK.Job.Force")) // выбрал силу - шлюп уже в море, торговаться не о чем
			{
				dialog.text = "Ты своё решил" + GetSexPhrase("", "а") + ", голланд" + GetSexPhrase("ец", "ка") + ". Корабль '"+ pchar.GWIK.Job.ForceShipName +"' в море у Бермуд - там твой капитан. Догоняй, коли не трус.";
				link.l1 = "Уже иду.";
				link.l1.go = "exit";
				break;
			}
			if (CheckAttribute(pchar, "GWIK.Job.Mutiny")) // Gregg: редкий твист посредника - выкупать некого: капитан Компании сам поднял бунт и подался в пираты. Он и сидит перед тобой
			{
				if (CheckAttribute(pchar, "GWIK.Job.MutinyReturnedSelf")) // уже убедил - беглец отбыл в контору сам, тут больше нечего делать (страховка, если deferred-уход не сработал)
				{
					dialog.text = "Мы обо всём договорились, голланд" + GetSexPhrase("ец", "ка") + ". Ступай в контору - свидимся там.";
					link.l1 = "До встречи.";
					link.l1.go = "exit";
					break;
				}
				if (!CheckAttribute(pchar, "GWIK.Job.MutinyMet"))
				{
					pchar.GWIK.Job.MutinyMet = true;
					AddQuestRecord("GWIK_RansomJob", "12"); // раскрытие: пленника нет, капитан переметнулся
					dialog.text = "Выкуп? Ха-ха-ха! Оглянись, голландец: твой капитан - вот он я, собственной персоной. Надоело мне гнуть спину на Компанию за жалкие гроши - теперь я сам себе хозяин под чёрным флагом. Так что ступай, обрадуй хозяев: выкупать некого.";
				}
				else
				{
					dialog.text = "Опять ты? Я своё сказал: назад в Компанию меня и калачом не заманишь.";
				}
				if (!CheckAttribute(pchar, "GWIK.Job.MutinyRefused")) // сперва - попытка убедить (Лидерство); отказ откроет пути «доложить» и «взять силой»
				{
					link.l1 = "Одумайся. Ты добрый капитан - Компания простит и примет назад. Вернись по-хорошему.";
					link.l1.go = "Mutiny_Persuade";
					link.l2 = "Что ж, дело твоё. Мне надо это обдумать.";
					link.l2.go = "exit";
				}
				else
				{
					link.l1 = "Раз по-хорошему не хочешь - Компания узнает, что ты предатель. (доложить)";
					link.l1.go = "Mutiny_Report";
					link.l2 = "Не хочешь по-хорошему - возьму силой. Выходи в море, там и поговорим.";
					link.l2.go = "Mutiny_Threaten";
					link.l3 = "Мне надо это обдумать.";
					link.l3.go = "exit";
				}
				break;
			}
			dialog.text = "А-а, ты от голландцев, за их капитаном? Деньги вперёд: "+ FindRussianMoneyString(iRansom) +" - и забирай его хоть сейчас.";
			if (CheckAttribute(pchar, "GWIK.Job.Bargained")) dialog.text = "Ну что, надумал"+ GetSexPhrase("","а") +"? Цена прежняя: "+ FindRussianMoneyString(iRansom) +", и капитан твой.";
			if (sti(pchar.money) >= iRansom)
			{
				link.l1 = "Вот выкуп. Где капитан?";
				link.l1.go = "pay";
			}
			else
			{
				link.l1 = "Столько у меня при себе нет...";
				link.l1.go = "exit";
			}
			if (!CheckAttribute(pchar, "GWIK.Job.Bargained")) // торг - только раз
			{
				link.l2 = "Побойся бога, столько он не стоит. Уступи.";
				link.l2.go = "bargain";
			}
			link.l3 = "Платить пиратам? Много чести. Заберу капитана сам"+ GetSexPhrase("","а") +".";
			link.l3.go = "force_warn";
			link.l4 = "Я ещё подумаю.";
			link.l4.go = "exit";
		break;

		case "bargain": // Gregg: торг - проверка Торговли; контора возместит НОМИНАЛ, сторгованное осядет у игрока
			pchar.GWIK.Job.Bargained = true;
			comSkill = GetSummonSkillFromName(pchar, SKILL_COMMERCE);
			nomRansom = sti(pchar.GWIK.Job.Ransom);
			if (comSkill >= 60) // Gregg: навык Торговли 0-100 (было гвоздём 8 - срабатывало почти всегда)
			{
				pchar.GWIK.Job.RansomActual = makeint(nomRansom * 75 / 100);
				AddCharacterExpToSkill(pchar, SKILL_COMMERCE, 30); // Gregg: опыт торговли за успешный торг (сбил четверть цены)
				dialog.text = "Тьфу на тебя, умеешь считать, голландская душа... Ладно, "+ sti(pchar.GWIK.Job.RansomActual) +" - и по рукам, только чтоб глаза мои тебя больше не видели.";
			}
			else
			{
				if (comSkill >= 35)
				{
					pchar.GWIK.Job.RansomActual = makeint(nomRansom * 85 / 100);
					AddCharacterExpToSkill(pchar, SKILL_COMMERCE, 20); // Gregg: опыт торговли за торг (сбил часть цены)
					dialog.text = "Хм, торгуешься не хуже нашего... Скину малость: "+ sti(pchar.GWIK.Job.RansomActual) +", и не пиастром меньше.";
				}
				else
				{
					pchar.GWIK.Job.RansomActual = makeint(nomRansom * 110 / 100);
					dialog.text = "Ты меня за дурака держишь?! За такую наглость - набавлю. Теперь "+ sti(pchar.GWIK.Job.RansomActual) +", и радуйся, что не вдвое.";
				}
			}
			link.l1 = "Идёт.";
			link.l1.go = "First time";
		break;

		case "force_warn": // Gregg: силовой путь - посредник ухмыляется и уводит капитана на шлюп
			dialog.text = "Заберёшь сам, значит? Ха! Ну гляди: пока ты тут храбришься, наш капитан уже на корабле, и тот поднимает якорь. Догонишь в море - твой, спору нет. А пустишь на дно - так голландцы тебе за утопленника спасибо не скажут. Ну что, всё ещё по-своему?";
			link.l1 = "Да. Пусть отчаливает - я и не таких достава"+ GetSexPhrase("л","ла") +" из воды.";
			link.l1.go = "force_go";
			link.l2 = "Пожалуй, обойдёмся без крови. Вернёмся к делу.";
			link.l2.go = "First time";
		break;

		case "force_go":
			if (!GWIK_RansomForceSpawn("Pirates")) // шлюп не вышел - не загоняем в тупик; якорь патруля у Бермуд (текст посредника «у Бермуд»)
			{
				dialog.text = "Хм, а корабль-то уже отвалил, и куда - без понятия. Загляни попозже, разнюхаю.";
				link.l1 = "Загляну.";
				link.l1.go = "exit";
				break;
			}
			AddQuestRecord("GWIK_RansomJob", "5");
			AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName); // строго после AddQuestRecord
			AddQuestUserData("GWIK_RansomJob", "sCap", pchar.GWIK.Job.ForceCapName);
			dialog.text = "Дело твоё, отчаянная голова. Корабль '"+ pchar.GWIK.Job.ForceShipName +"', капитан "+ pchar.GWIK.Job.ForceCapName +", уже в море у Бермуд, пленник в трюме. Догоняй - и помни, что я сказал про дно.";
			link.l1 = "Найду и заберу.";
			link.l1.go = "exit";
		break;

		case "pay":
			AddMoneyToCharacter(pchar, -iRansom);
			GWIK_RansomPay();
			AddQuestRecord("GWIK_RansomJob", "2");
			dialog.text = "Вот и славно. Капитан уже на твоём корабле - мои ребята его проводили. И передай голландцам: будут платить так же исправно - будем брать их людей почаще, хе-хе.";
			link.l1 = "Ну и шуточки у тебя...";
			link.l1.go = "exit";
		break;

		case "Mutiny_Persuade": // Gregg: попытка убедить беглеца вернуться мирно (проверка Лидерства)
			leadSkill = GetSummonSkillFromName(pchar, SKILL_LEADERSHIP);
			if (leadSkill >= 25 + sti(pchar.rank)) // Gregg: порог = навык Авторитет (шкала 0-100, НЕ атрибут «Лидерство» 1-10) >= 25 + ранг ГГ - растёт с игроком; авторитет капитана перевесил, беглец согласен вернуться
			{
				pchar.GWIK.Job.MutinyReturnedSelf = true; // сам вернётся в контору - пассажира НЕ создаём (доклад заберёшь в конторе, узел GWIK_MutinyReturnReport)
				AddDialogExitQuestFunction("GWIK_MutineerDepart"); // после закрытия диалога беглец покидает таверну (deferred - обнулять локацию собеседника безопасно только после закрытия; образец найма офицера)
				AddQuestRecord("GWIK_RansomJob", "14");
				dialog.text = "...Ай, чёрт с тобой. Твоя правда - под этим флагом мне долго не сдобровать, а Компания хоть платит исправно. Ладно, голланд" + GetSexPhrase("ец", "ка") + ", твоя взяла. Сам доберусь до конторы и повинюсь - свидимся там.";
				link.l1 = "Верное решение. Жду тебя в конторе.";
				link.l1.go = "exit";
			}
			else
			{
				pchar.GWIK.Job.MutinyRefused = true; // мирный путь закрыт - теперь только доложить или взять силой
				dialog.text = "Складно поёшь, голланд" + GetSexPhrase("ец", "ка") + ", да только зубы мне не заговоришь. Я своё решил. Или проваливай подобру, или бери силой - только лёгкой добычи не жди.";
				link.l1 = "Раз так - Компания узнает, что ты предатель. (доложить)";
				link.l1.go = "Mutiny_Report";
				link.l2 = "Не хочешь по-хорошему - возьму силой. Выходи в море.";
				link.l2.go = "Mutiny_Threaten";
				link.l3 = "Мне надо это обдумать.";
				link.l3.go = "exit";
			}
		break;

		case "Mutiny_Threaten": // Gregg: грозим захватом - беглец выходит в море враждебным (силовой путь; захват = вернул капитана, потопление = провал)
			DeleteAttribute(pchar, "GWIK.Job.MutinyReportReady"); // передумал докладывать - идём на силовой путь
			if (!GWIK_RansomForceSpawn("Pirates")) // его корабль выходит в море у Бермуд (Force=true)
			{
				dialog.text = "Мои ребята сейчас в разгуле, корабль к выходу не готов. Загляни попозже - тогда и решим спор по-морскому.";
				link.l1 = "Загляну.";
				link.l1.go = "exit";
				break;
			}
			AddQuestRecord("GWIK_RansomJob", "13");
			AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName); // строго после AddQuestRecord
			AddQuestUserData("GWIK_RansomJob", "sType", pchar.GWIK.Job.ForceShipType); // Gregg: тип корабля вместо имени капитана - @<sCap> здесь ПИРАТ (случайное имя), не беглец; путало
			dialog.text = "Ха! А ты не робкого десятка. Что ж, выходи в море, голландец - моя команда уже на корабле '"+ pchar.GWIK.Job.ForceShipName +"'. Догонишь и возьмёшь на абордаж - твоя взяла. А нет - пойдёшь на корм рыбам. Только помни: пустишь корабль на дно - и меня утопишь, а мёртвый я Компании без надобности.";
			link.l1 = "Выходи в море. Там и встретимся.";
			link.l1.go = "exit";
		break;

		case "Mutiny_Report": // Gregg: махнуть рукой и доложить Компании, что капитан переметнулся (мизерная компенсация в конторе)
			pchar.GWIK.Job.MutinyReportReady = true; // доложить в конторе ГВИК (узел GWIK_MutinyReport)
			AddQuestRecord("GWIK_RansomJob", "15");
			dialog.text = "Беги-беги, ябеда. Только Компания твоя мне больше не указ - я нынче вольный человек. Проваливай, пока цел.";
			link.l1 = "Ты ещё пожалеешь об этом.";
			link.l1.go = "exit";
		break;

		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
