// LEO: тавернский врач - платное лечение раненых всего флота (вызывается из диалога тавернщика: call_doctor -> spawn_tavern_doctor).
// Врач подходит к ГГ сам (LAi_ActorDialog) и заводит разговор. Оплата -> флаг pchar.Ship.Crew.woundedTavernHeal -> на смену суток лечится весь флот (food.c).
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
				dialog.text = "Мне сказали, у тебя раненые... но, гляжу, всё уже улажено. Тогда не буду мешать, капитан.";
				link.l1 = "И вправду. Бывай, лекарь.";
				link.l1.go = "exit";
			}
			else
			{
				int iDocCost = Wounded_TavernCost(pchar);
				dialog.text = "Тавернщик шепнул, у тебя раненые, капитан. Я лекарь, дело своё знаю. За " + iDocCost + " пиастров нынче же вечером обойду твои корабли - к утру все " + Wounded_FleetTotal(pchar) + " твоих раненых встанут в строй. По рукам?";
				if (sti(pchar.money) >= iDocCost)
				{
					link.l1 = "Договорились, вот деньги.";
					link.l1.go = "doctor_deal";
				}
				else
				{
					link.l1 = "Столько у меня сейчас нет...";
					link.l1.go = "exit";
				}
				link.l2 = "Пожалуй, обойдусь.";
				link.l2.go = "exit";
			}
		break;

		case "doctor_deal":
			int iDocCost2 = Wounded_TavernCost(pchar);
			AddMoneyToCharacter(pchar, -iDocCost2);
			pchar.Ship.Crew.woundedTavernHeal = "1"; // флаг: на смену суток вылечить весь флот
			dialog.text = "Отлично. Нынче же вечером займусь твоими людьми. К утру будут как новенькие. Доброго плавания, капитан.";
			link.l1 = "Благодарю, лекарь.";
			link.l1.go = "exit";
		break;
	}
}
