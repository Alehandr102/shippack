// Gregg: матрос с флагманского корабля, бухта Аматике - «Чёрная Орхидея», глава 5.
// Логика - Program\Quests\reaction_functions\blackorchid.c, DestinyGiftNeedKillFeregueyro.
// НАШЕГО сочинения, в К:ПС этого нет: там корабль просто оказывался у берега, и как
// команда нашла выброшенного на Мэйн капитана, не объяснялось никак. Объяснение -
// анонимное письмо через курьера; кто послал, не знает никто, и это намеренно.

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
			dialog.text = "All ready, captain. We await your word.";
			link.l1 = "We sail soon.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Captain! Alive... We have been searching the whole coast for you these several days, and had all but given up.";
			link.l1 = "How did you know where to look for me at all?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "A letter came. In Belize a courier pushed it into the bosun hands on the wharf: your captain is in Amatike Bay, make haste. No signature, no name. The bosun took him by the collar - from whom? And the man did not know himself: paid in advance and told to deliver.";
			link.l1 = "And you believed a scrap of paper with no signature?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(shrugs)... What else was left to us, captain? There was no other word of you at all. We came, and here you stand. So it did not lie.";
			link.l1 = "So it did not lie. And the ship?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Patched up and stocked, all as it should be. The carpenter grumbled, but we managed. We are waiting on you, captain.";
			link.l1 = "We sail soon.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time"; // LEO: иначе после дочитанного разговора узел уезжал в пустоту
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
