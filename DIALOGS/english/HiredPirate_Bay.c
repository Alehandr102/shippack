// LEO: Hired privateer at the bay - hand over the prize. Pay the rest, pick an officer to captain her.
// Logic - scripts/HiredPirate.c. Officer picker uses the "assign_<slot>" node-suffix trick (see Common_Governor.c).
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
				dialog.text = "Our business is done, captain. Fair winds.";
				link.l1 = "Fair winds.";
				link.l1.go = "exit";
				break;
			}
			// нет свободного слота корабля в эскадре
			if (GetCompanionQuantity(pchar) >= COMPANION_MAX)
			{
				dialog.text = "There she is, captain - a fine prize. But your fleet's full. Free up a berth and come back for her.";
				link.l1 = "I'll make room.";
				link.l1.go = "exit";
				break;
			}
			// нет денег на остаток
			if (sti(pchar.money) < iRest)
			{
				dialog.text = "There she lies. The rest of my fee's " + FindRussianMoneyString(iRest) + " - and you're short. Come back with the coin.";
				link.l1 = "I'll get it.";
				link.l1.go = "exit";
				break;
			}
			// список офицеров свиты
			dialog.text = "Ah, you made it. Your ship's riding at anchor in the bay, captain - yours the moment you settle up. The rest of my fee: " + FindRussianMoneyString(iRest) + ". Who'll captain her? Name one of your officers.";
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
				dialog.text = "There she is, captain - but you've no spare officer aboard to captain her. Come back with one in your retinue.";
				link.l1 = "I'll bring one.";
				link.l1.go = "exit";
				break;
			}
			link.l90 = "Give me a moment.";
			link.l90.go = "exit";
		break;

		case "assign":
			slot = sti(pchar.GenQuest.HiredPirate.PickSlot);
			oidx = GetOfficersIndex(pchar, slot);
			if (oidx < 0) { dialog.text = "That one's not with you. Pick another."; link.l1 = "..."; link.l1.go = "First time"; break; }
			HiredPirate_GiveShip(oidx);
			dialog.text = "Done. Her papers are already made over to you - all in order, no harbor rat will find fault. She's your officer's now, handle her well. Our business is settled, captain. Fair winds.";
			link.l1 = "Fair winds.";
			link.l1.go = "exit";
		break;
	}
}
