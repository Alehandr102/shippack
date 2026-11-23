// Диалог жертвователя (церковный квест №2 "Сбор пожертвований на храм")
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sTitle;
	int iBase;

	switch(Dialog.CurrentNode)
	{
		case "ChurchDonate_Donor_Start":
			// донор лишь здоровается - деньги даст, только когда ГГ попросит
			if (rand(1) == 0)
			{
				dialog.text = "How may I serve you, "+ GetSexPhrase("sir","madam") +"?";
			}
			else
			{
				dialog.text = "Did you want something?";
			}
			link.l1 = "The Holy Father is collecting for the temple's restoration. Would you spare a coin?";
			link.l1.go = "ChurchDonate_Donor_Ask";
			link.l2 = "Surely a person of your means will not refuse the parish a generous gift.";
			link.l2.go = "ChurchDonate_Donor_Persuade";
			link.l3 = "Forgive the intrusion.";
			link.l3.go = "exit";
		break;

		case "ChurchDonate_Donor_Ask":
			iBase = 300 + rand(500) + sti(pchar.rank) * 100;
			if (sti(PChar.reputation) > REPUTATION_NEUTRAL) iBase = iBase + 200 + rand(300);
			PChar.GenQuest.ChurchDonate.PendingGift = iBase;
			dialog.text = "For the temple - a godly cause. Here, take it from a pure heart.";
			link.l1 = "Thank you. The Lord will reward your generosity.";
			link.l1.go = "ChurchDonate_Donor_Give";
		break;

		case "ChurchDonate_Donor_Persuade":
			iBase = 300 + rand(500) + sti(pchar.rank) * 100;
			if (sti(PChar.reputation) > REPUTATION_NEUTRAL) iBase = iBase + 200 + rand(300);
			if (rand(100) < 20 + sti(GetSummonSkillFromName(pchar, SKILL_LEADERSHIP)))
			{
				PChar.GenQuest.ChurchDonate.PendingGift = iBase + makeint(iBase/2); // уговорил - щедрее
				dialog.text = "You know, it truly is no good being stingy with the Lord's work! So be it - here, I shall not miss it.";
			}
			else
			{
				PChar.GenQuest.ChurchDonate.PendingGift = iBase - makeint(iBase/5); // не убедил - даёт меньше, с обидой
				dialog.text = "Hmph, I know my duty to God without your prompting. I'll give what I can spare - here.";
			}
			link.l1 = "Thank you. The Lord will reward your generosity.";
			link.l1.go = "ChurchDonate_Donor_Give";
		break;

		case "ChurchDonate_Donor_Give":
			DialogExit();
			NextDiag.CurrentNode = "ChurchDonate_Donor_Done"; // на повторный клик - валидный узел с выходом
			AddMoneyToCharacter(PChar, sti(PChar.GenQuest.ChurchDonate.PendingGift)); // деньги реально передаются ГГ
			PChar.GenQuest.ChurchDonate.Collected = sti(PChar.GenQuest.ChurchDonate.Collected) + sti(PChar.GenQuest.ChurchDonate.PendingGift);
			PChar.GenQuest.ChurchDonate.DonorsDone = sti(PChar.GenQuest.ChurchDonate.DonorsDone) + 1;
			characters[GetCharacterIndex(NPChar.id)].LifeDay = 0; // жертвователь уходит
			if (sti(PChar.GenQuest.ChurchDonate.DonorsDone) >= sti(PChar.GenQuest.ChurchDonate.DonorsTotal))
			{
				DeleteAttribute(PChar, "GenQuest.ChurchDonate.DonorsInTown");
				sTitle = PChar.GenQuest.ChurchDonate.QuestTown + "ChurchDonate";
				AddQuestRecordEx(sTitle, "ChurchDonate", "2"); AddQuestUserData(sTitle, "sSex", GetSexPhrase("","а"));
				AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + PChar.GenQuest.ChurchDonate.QuestTown + "Gen"));
			}
		break;

		case "ChurchDonate_Donor_Done":
			dialog.text = "I have already given my mite for the temple. Good day to you.";
			link.l1 = "And a good day to you.";
			link.l1.go = "exit";
		break;

		case "exit":
			DialogExit();
		break;
	}
}
