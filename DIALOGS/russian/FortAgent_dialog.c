// Палубные переговоры с посыльным-агентом вражеской нации (перехват комендантского конвоя в море).
// Энкаунтер создаётся в Treasure.c (FortAgentOnMap); логика исходов - там же (FortAgent_HandOver/StartDeliver).
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iBase = sti(pchar.CargoQuest.iMoney);

	switch (Dialog.CurrentNode)
	{
		case "exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

		case "First time":
			// сделка уже заключена ранее - предложений больше нет (фантом мог пересоздаться и потерять DontDeskTalk)
			if (CheckAttribute(pchar, "FortAgentResolved"))
			{
				dialog.text = "Слово сказано, капитан. Нам больше нечего обсуждать.";
				link.l1 = "Как скажешь.";
				link.l1.go = "exit";
				NextDiag.TempNode = "First time";
				break;
			}
			dialog.text = "Не хватайтесь за сабли, капитан, я пришёл не воевать. Я - доверенное лицо короны " + NationNameGenitive(sti(NPChar.nation)) + ", а у неё повсюду свои глаза и уши. По донесениям нашей разведки, в трюмах вашего конвоя - боеприпасы для форта, чья нация во вражде с нами. Не место такому грузу в арсенале наших врагов. Предлагаю выбор: передайте суда конвоя моим людям - и получите вдвое против обещанного гильдией, либо доставьте груз своим ходом в наш форт - там заплатят впятеро. Откажетесь - разойдёмся в открытом море.";
			link.l1 = "Идёт. Забирайте конвой. (" + FindRussianMoneyString(iBase * 2) + ")";
			link.l1.go = "FortAgent_Hand";
			link.l2 = "Довезу груз в ваш форт сам"+ GetSexPhrase("","а") +". (" + FindRussianMoneyString(iBase * 5) + ")";
			link.l2.go = "FortAgent_Deliver";
			link.l3 = "Я везу груз по адресу. Прочь с дороги.";
			link.l3.go = "FortAgent_Refuse";
			NextDiag.TempNode = "First time";
		break;

		case "FortAgent_Hand":
			// раскассирование конвоя + уход агента - на ВЫХОДЕ из диалога (иначе софтлок: despawn во время диалога)
			AddDialogExitQuestFunction("FortAgent_HandOver_OnExit");
			dialog.text = "Мудрое решение, капитан. Мои люди примут суда - дальше это наша забота. Вот ваше золото, вдвое против обещанного гильдией. Расходимся, и да забудем друг о друге.";
			link.l1 = "Договорились.";
			link.l1.go = "exit";
		break;

		case "FortAgent_Deliver":
			FortAgent_StartDeliver(NPChar);                          // сделка/срок/город - сразу (нужно для текста)
			AddDialogExitQuestFunction("FortAgent_Depart_OnExit");   // уход агента - на ВЫХОДЕ (иначе софтлок)
			dialog.text = "По рукам, капитан. Ведите конвой в форт " + XI_ConvertString("Colony" + pchar.FortAgentDeliver.city + "Gen") + ". Вот вам мой опознавательный флаг - поднимите его на мачте: под ним и береговые пушки, и городская стража признают в вас своего, с важным грузом для гарнизона, и пальцем не тронут. Тамошний комендант ждёт боеприпасы и заплатит впятеро против гильдейской цены - за то, что довезёте. Срок - " + FindRussianDaysString(sti(pchar.FortAgentDeliver.days)) + " от сего часа; опоздавший товар нас уже не заинтересует.";
			link.l1 = "Груз будет в форте.";
			link.l1.go = "exit";
		break;

		case "FortAgent_Refuse":
			dialog.text = "Жаль. Значит, договориться не вышло. Что ж, тогда поговорим иначе - в открытом море. Поднимай паруса, капитан, и моли своего бога.";
			link.l1 = "Посмотрим, чья возьмёт.";
			link.l1.go = "exit";
			// враждебность вешаем на РЕАЛЬНОГО агента на ВЫХОДЕ (в диалоге может быть клон - отсюда "не мог атаковать")
			AddDialogExitQuestFunction("FortAgent_Refuse_OnExit");
		break;
	}
}
