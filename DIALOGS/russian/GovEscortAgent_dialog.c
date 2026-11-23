// Палубные переговоры с союзным посыльным-агентом (морское событие эскорта, вариант "агент").
// Энкаунтер создаётся в Treasure.c (GovEscortAgentOnMap); исходы - там же (GovEscortAgent_Redirect и обёртки _OnExit).
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iBase;
	string sZ;

	switch (Dialog.CurrentNode)
	{
		case "exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

		case "First time":
			// решение уже принято (фантом мог пересоздаться и потерять DontDeskTalk)
			if (CheckAttribute(pchar, "GovEscortAgentResolved"))
			{
				dialog.text = "Решение принято, капитан. Доброго ветра.";
				link.l1 = "И тебе.";
				link.l1.go = "exit";
				NextDiag.TempNode = "First time";
				break;
			}
			sZ = GovEscortAgent_GetCity(NPChar);
			if (sZ == "")
			{
				dialog.text = "Обстановка изменилась, капитан. Приказ о другом порте отменён. Везите груз по прежнему назначению.";
				link.l1 = "Продолжу прежним курсом.";
				link.l1.go = "exit";
				NextDiag.TempNode = "First time";
				AddDialogExitQuestFunction("GovEscortAgent_Depart_OnExit");
				break;
			}
			iBase = sti(pchar.CargoQuest.iMoney);
			dialog.text = "Спокойно, капитан, сабли ни к чему - я свой. Доверенное лицо короны " + NationNameGenitive(sti(NPChar.nation)) + ". Вести дурные: на вашем маршруте залегли пираты - кто-то проболтался про казённый груз. Послушайте доброго совета: уведите конвой в " + XI_ConvertString("Colony" + sZ + "Acc") + " - там груз примут так же, прямо у губернатора, а за хлопоты и риск корона заплатит впятеро. Или идите как шли - дело ваше.";
			link.l1 = "Идёт. Уведу груз в " + XI_ConvertString("Colony" + sZ + "Acc") + ". (" + FindRussianMoneyString(iBase * 5) + ")";
			link.l1.go = "GovEscortAgent_Yes";
			link.l2 = "Нет. Иду прежним курсом.";
			link.l2.go = "GovEscortAgent_No";
			NextDiag.TempNode = "First time";
		break;

		case "GovEscortAgent_Yes":
			if (!GovEscortAgent_Redirect(NPChar))
			{
				dialog.text = "Приказ о другом порте уже недействителен, капитан. Везите груз по прежнему назначению.";
				link.l1 = "Продолжу прежним курсом.";
				link.l1.go = "exit";
				AddDialogExitQuestFunction("GovEscortAgent_Depart_OnExit");
				break;
			}
			AddDialogExitQuestFunction("GovEscortAgent_Depart_OnExit");   // уход агента - на ВЫХОДЕ (despawn в диалоге = софтлок)
			dialog.text = "Разумно, капитан. Курс на " + XI_ConvertString("Colony" + pchar.CargoQuest.iTradeColony + "Acc") + ". Там вас встретят и расплатятся сполна - впятеро против гильдейской цены. Доброго ветра в спину.";
			link.l1 = "Курс проложен.";
			link.l1.go = "exit";
		break;

		case "GovEscortAgent_No":
			dialog.text = "Вольному воля, капитан. Дай вам бог проскочить. Прощайте.";
			link.l1 = "Разойдёмся.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("GovEscortAgent_RefuseResolve_OnExit"); // агент уходит + 50% перехватчики
		break;
	}
}
