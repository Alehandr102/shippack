// Gregg: агент конкурентов - перехват «горячего пакета» ГВИК на улицах города.
// Спавнится при входе в город конторы (GWIK_MailAgentArrive), сам подходит к ГГ.
// Стиль - шпионский, вполголоса, не как чинный управляющий конторы.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int i;
	int cash;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			if (!CheckAttribute(pchar, "GWIK.Job") || pchar.GWIK.Job != "mail" || !CheckAttribute(pchar, "GWIK.Job.Hot") || CheckAttribute(pchar, "GWIK.Job.HotResolved"))
			{
				dialog.text = "Обознался. Иди своей дорогой, " + GetSexPhrase("приятель", "подруга") + ".";
				link.l1 = "...";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Не оборачивайся резко, капитан, просто слушай. Тот запечатанный пакет, что тебе сунули в конторе, стоит куда больше, чем платят почтальону. Я дам за него хорошую цену - или хотя бы за то, чтобы мой писарь снял копию. А заартачишься - что ж, дело твоё, только потом не жалуйся.";
			// Gregg: разминулись - ушёл из города, не дослушав. Агент это припоминает (счётчик ведёт GWIK_MailAgentLeft)
			if (CheckAttribute(pchar, "GWIK.Job.HotDodge")) dialog.text = "А ты шустрый, капитан. В прошлый раз ушёл, не дослушав - думал, я отстану? Разговор тот же: тот запечатанный пакет из конторы стоит куда больше, чем платят почтальону. Цена хорошая, или хотя бы копию снимем. Ну а нет - сам понимаешь.";
			link.l1 = "Сколько дашь за пакет?";
			link.l1.go = "sell_ask";
			link.l2 = "Копию - так и быть. Пакет остаётся у меня.";
			link.l2.go = "copy";
			link.l3 = "Убери руки от бумаг Компании.";
			link.l3.go = "refuse";
		break;

		case "sell_ask": // LEO: сперва только цена - пакет уходит лишь после согласия
			cash = makeint(sti(pchar.GWIK.Job.Pay) * 2);
			dialog.text = "Вдвое против того, что тебе заплатят в конторе: " + FindRussianMoneyString(cash) + ". Отдаёшь пакет - и мы не знакомы.";
			link.l1 = "По рукам, забирай пакет.";
			link.l1.go = "sell";
			link.l2 = "Копию - так и быть. Пакет остаётся у меня.";
			link.l2.go = "copy";
			link.l3 = "Не пойдёт. Убери руки от бумаг Компании.";
			link.l3.go = "refuse";
		break;

		case "sell": // продать пакет - провал поручения, отступные, минус репутация
			cash = makeint(sti(pchar.GWIK.Job.Pay) * 2);
			AddMoneyToCharacter(pchar, cash);
			GWIK_AddRep(-15);
			AddQuestRecord("GWIK_MailJob", "5");
			AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_MailJob");
			pchar.GWIK.Job.HotResolved = true;
			DeleteAttribute(pchar, "GWIK.Job");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Разумно. Вот твоё золото, "+ FindRussianMoneyString(cash) +". О пакете позабочусь сам. Бывай, капитан.";
			link.l1 = "По рукам.";
			link.l1.go = "exit";
		break;

		case "copy": // дать снять копию - инфа (прайс-лист архипелага), минус репутация, пакет доставляешь дальше
			for (i = 0; i < MAX_COLONIES; i++)
			{
				ref rcolony = &Colonies[i];
				if (rcolony.id != "Caiman" && rcolony.id != "Panama" && rcolony.nation != "none") SetPriceListByStoreMan(rcolony);
			}
			PlayStereoSound("notebook");
			GWIK_AddRep(-5);
			AddQuestRecord("GWIK_MailJob", "6");
			pchar.GWIK.Job.HotResolved = true;
			dialog.text = "Вот это по-деловому. Пока писарь копирует - глянь и ты: тут свежие цены по всему архипелагу, пригодится. Пакет твой, неси куда нёс. Приятно иметь дело.";
			link.l1 = "Занятно. Прощай.";
			link.l1.go = "exit";
		break;

		case "refuse": // Gregg: отказ больше не бесплатный - агент свистит подмогу и требует пакет силой
			dialog.text = "Зря ты так, капитан. Это была вежливая просьба... (он коротко свистит, и из подворотен неспешно выходят люди с ножами) Пакет. Сюда. Или снимем с тела - нам без разницы.";
			link.l1 = "Забирай. Чужие бумаги моей крови не стоят.";
			link.l1.go = "hand_over";
			link.l2 = "Попробуй возьми.";
			link.l2.go = "GWIK_MailFight";
		break;

		case "hand_over": // отдал под угрозой - пакет потерян, денег нет, репутация вниз
			pchar.GWIK.Job.HotResolved = true;
			GWIK_AddRep(-10);
			AddQuestRecord("GWIK_MailJob", "7");
			CloseQuestHeader("GWIK_MailJob");
			DeleteAttribute(pchar, "GWIK.Job");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Разумно. (пакет исчезает у него за пазухой) Ступай, капитан. И голландцам жаловаться не советую - себе дороже выйдет.";
			link.l1 = "...";
			link.l1.go = "exit";
		break;

		case "GWIK_MailFight": // Gregg: драка. Подмога - 1 + число офицеров ГГ; из локации не выйти, пока вся ватага не перебита (GWIK_MailAgentFightStart)
			pchar.GWIK.Job.HotResolved = true;
			AddDialogExitQuestFunction("GWIK_MailAgentFightStart"); // бой начинается ПОСЛЕ закрытия диалога
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "exit":
			GWIK_MailAgentDismiss(NPChar); // агент уходит (актёром, ДО DialogExit - иначе «Invalid function call»)
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
