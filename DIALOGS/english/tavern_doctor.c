// LEO: tavern doctor - paid healing of the whole fleet's wounded (summoned from the tavernkeeper dialog: call_doctor -> spawn_tavern_doctor).
// The doctor walks up to the player himself (LAi_ActorDialog). Payment -> flag pchar.Ship.Crew.woundedTavernHeal -> the whole fleet heals on the next day change (food.c).
void ProcessDialogEvent()
{
	ref NPChar, d;
	aref Link, Diag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makeref(d, Dialog);
	makearef(Diag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "exit":
			// LEO: завершаем вызов сразу; актёр уходит сам, без отложенной ссылки на переиспользуемый слот.
			DeleteAttribute(pchar, "woundedDocPending");
			NPChar.LifeDay = 0;
			LAi_SetActorType(NPChar);
			LAi_ActorGoToLocation(NPChar, "reload", "reload1", "none", "", "", "", 10.0);
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
		break;

		case "doctor_greet":
			if (Wounded_FleetTotal(pchar) < 1 || CheckAttribute(pchar, "Ship.Crew.woundedTavernHeal"))
			{
				dialog.text = "I was told you have wounded... but I see it's all been arranged already. I'll not trouble you then, captain.";
				link.l1 = "Indeed. Farewell, doctor.";
				link.l1.go = "exit";
			}
			else
			{
				int iDocCost = Wounded_TavernCost(pchar);
				dialog.text = "The keeper says you have wounded, captain. I'm a doctor, I know my trade. For " + iDocCost + " piastres I'll visit your ships this very evening - by morning all " + Wounded_FleetTotal(pchar) + " of your wounded will be back on their feet. Deal?";
				if (sti(pchar.money) >= iDocCost)
				{
					link.l1 = "Deal, here's the money.";
					link.l1.go = "doctor_deal";
				}
				else
				{
					link.l1 = "I don't have that much right now...";
					link.l1.go = "exit";
				}
				link.l2 = "I'll manage without it.";
				link.l2.go = "exit";
			}
		break;

		case "doctor_deal":
			int iDocCost2 = Wounded_TavernCost(pchar);
			AddMoneyToCharacter(pchar, -iDocCost2);
			pchar.Ship.Crew.woundedTavernHeal = "1"; // flag: heal the whole fleet on the next day change
			dialog.text = "Excellent. I'll see to your men this very evening. By morning they'll be good as new. Fair winds, captain.";
			link.l1 = "Thank you, doctor.";
			link.l1.go = "exit";
		break;
	}
}
