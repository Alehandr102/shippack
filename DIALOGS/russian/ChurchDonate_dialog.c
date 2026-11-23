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
				dialog.text = "Чем могу служить, "+ GetSexPhrase("сударь","сударыня") +"?";
			}
			else
			{
				dialog.text = "Вы что-то хотели?";
			}
			link.l1 = "Святой отец собирает на восстановление храма. Не уделите ли лепту?";
			link.l1.go = "ChurchDonate_Donor_Ask";
			link.l2 = "Человек вашего достатка не откажет приходу в щедром даре, я уверен"+ GetSexPhrase("","а") +".";
			link.l2.go = "ChurchDonate_Donor_Persuade";
			link.l3 = "Простите за беспокойство.";
			link.l3.go = "exit";
		break;

		case "ChurchDonate_Donor_Ask":
			iBase = 300 + rand(500) + sti(pchar.rank) * 100;
			if (sti(PChar.reputation) > REPUTATION_NEUTRAL) iBase = iBase + 200 + rand(300);
			PChar.GenQuest.ChurchDonate.PendingGift = iBase;
			dialog.text = "На храм - дело богоугодное. Вот, держите от чистого сердца.";
			link.l1 = "Благодарю. Господь воздаст вам за щедрость.";
			link.l1.go = "ChurchDonate_Donor_Give";
		break;

		case "ChurchDonate_Donor_Persuade":
			iBase = 300 + rand(500) + sti(pchar.rank) * 100;
			if (sti(PChar.reputation) > REPUTATION_NEUTRAL) iBase = iBase + 200 + rand(300);
			if (rand(100) < 20 + sti(GetSummonSkillFromName(pchar, SKILL_LEADERSHIP)))
			{
				PChar.GenQuest.ChurchDonate.PendingGift = iBase + makeint(iBase/2); // уговорил - щедрее
				dialog.text = "А ведь и впрямь негоже скупиться на дело Господне! Будь по-вашему, вот, не обеднею.";
			}
			else
			{
				PChar.GenQuest.ChurchDonate.PendingGift = iBase - makeint(iBase/5); // не убедил - даёт меньше, с обидой
				dialog.text = "Хм, и без вас знаю свой долг перед Богом. Сколько не жалко, столько и дам - вот.";
			}
			link.l1 = "Благодарю. Господь воздаст вам за щедрость.";
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
			dialog.text = "Я уже внёс свою лепту на восстановление храма. Доброго вам дня.";
			link.l1 = "И вам доброго дня.";
			link.l1.go = "exit";
		break;

		case "exit":
			DialogExit();
		break;
	}
}
