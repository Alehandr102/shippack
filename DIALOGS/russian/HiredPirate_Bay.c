// LEO: Наёмный пират в бухте - передача приза. Оплатить остаток, выбрать офицера-капитана.
// Логика - scripts/HiredPirate.c. Выбор офицера через суффикс узла "assign_<slot>" (образец Common_Governor.c).
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;
	int i;
	int n;
	int oidx;
	int slot;
	string attr;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iRest = 0;
	if (CheckAttribute(pchar, "GenQuest.HiredPirate.Rest")) iRest = sti(pchar.GenQuest.HiredPirate.Rest);

	// выбор офицера: узел "assign_<slot>" -> достаём slot, идём в "assign"
	attr = Dialog.CurrentNode;
	if (findsubstr(attr, "assign_", 0) != -1)
	{
		i = findsubstr(attr, "_", 0);
		pchar.GenQuest.HiredPirate.PickSlot = strcut(attr, i + 1, strlen(attr) - 1);
		Dialog.CurrentNode = "assign";
	}

	switch (Dialog.CurrentNode)
	{
		case "exit":
			DialogExit();
		break;

		case "First time":
			// квест уже закрыт (приз забран/провал) - ничего не предлагаем (страховка от повторной выдачи за 0)
			if (!HiredPirate_IsActive())
			{
				dialog.text = "Дело сделано, капитан. Попутного ветра.";
				link.l1 = "Попутного ветра.";
				link.l1.go = "exit";
				break;
			}
			// нет свободного слота корабля в эскадре
			if (GetCompanionQuantity(pchar) >= COMPANION_MAX)
			{
				dialog.text = "Вот он, капитан, - славный приз. Да только флот у тебя полон. Освободи место в эскадре и приходи за ним.";
				link.l1 = "Освобожу.";
				link.l1.go = "exit";
				break;
			}
			// нет денег на остаток
			if (sti(pchar.money) < iRest)
			{
				dialog.text = "Вот он, ждёт. Остаток по уговору - " + FindRussianMoneyString(iRest) + ", а у тебя не хватает. Возвращайся с монетой.";
				link.l1 = "Достану.";
				link.l1.go = "exit";
				break;
			}
			// список офицеров свиты
			dialog.text = "А, добрался. Твой корабль стоит в бухте, капитан, - твой, как только рассчитаешься. С тебя остаток: " + FindRussianMoneyString(iRest) + ". Кого посадишь капитаном? Назови офицера из свиты.";
			n = 0;
			for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
			{
				oidx = GetOfficersIndex(pchar, i);
				if (oidx < 0) continue;
				attr = "l" + i;
				link.(attr) = GetFullName(&characters[oidx]) + " (" + FindRussianMoneyString(iRest) + ")";
				link.(attr).go = "assign_" + i;
				n++;
			}
			if (n == 0)
			{
				dialog.text = "Вот он, капитан, - да только запасного офицера у тебя на борту нет, некого сажать капитаном. Возвращайся с офицером в свите.";
				link.l1 = "Приведу.";
				link.l1.go = "exit";
				break;
			}
			link.l90 = "Дай подумать.";
			link.l90.go = "exit";
		break;

		case "assign":
			slot = sti(pchar.GenQuest.HiredPirate.PickSlot);
			oidx = GetOfficersIndex(pchar, slot);
			if (oidx < 0) { dialog.text = "Этого при тебе нет. Выбери другого."; link.l1 = "..."; link.l1.go = "First time"; break; }
			HiredPirate_GiveShip(oidx);
			dialog.text = "Готово. Бумаги на корабль уже переписаны на тебя - всё чисто, ни одна портовая крыса не придерётся. Корабль теперь под твоим офицером, береги его. Дело сделано, капитан. Попутного ветра.";
			link.l1 = "Попутного ветра.";
			link.l1.go = "exit";
		break;
	}
}
