// Gregg: Роберт Рэксвелл, губернатор Сан-Хуана - «Чёрная Орхидея», глава 6 (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c, SpanishGenerosityFinalBattleEnd.
// Оригинал - DamnedDestiny\SpanishGenerosity\Robert Reksvell.c.
// Сбор налога устроен как у штатного офицера-губернатора (Enc_Officer_dialog.c, узлы
// Gover_Tax / Gover_Tax_Get), но формула СВОЯ - см. комментарий в узле Gover_Tax.
// Увольнения нет намеренно: Рэксвелл нужен главам 7-9, снимать его с должности нельзя.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iTemp;
	int iTax;
	int iFortValue;
	int iPerDay;
	ref sld;

	NextDiag.TempNode = "Gover_Main";

	switch (Dialog.CurrentNode)
	{
		case "First Time":
			dialog.text = "Роберт Рэксвелл, к вашим услугам. Кендарт предупредил, что вы зайдёте. Город принят, книги в порядке, недостачи нет - если не считать той, что оставили после себя прежние хозяева.";
			link.l1 = "Займитесь делом, Рэксвелл. Я загляну.";
			link.l1.go = "exit";
			NextDiag.TempNode = "Gover_Main";
		break;

		case "Gover_Main":
			iTemp = GetNpcQuestPastDayParam(NPChar, "GoverTalkDate");
			if (iTemp > 0)
			{
				dialog.text = "Какие будут распоряжения? Ваш прошлый визит был " + FindRussianDaysString(iTemp) + " назад.";
			}
			else
			{
				dialog.text = "Какие будут распоряжения?";
			}
			link.l1 = "Сколько собрано налогов?";
			link.l1.go = "Gover_Tax";
			link.l2 = "Пока никаких. Всего доброго.";
			link.l2.go = "exit";
			SaveCurrentNpcQuestDateParam(NPChar, "GoverTalkDate");
		break;

		case "Gover_Tax":
			iTemp = GetNpcQuestPastDayParam(NPChar, "StartTaxDate");
			iTax = makeint((GetCharacterSkillSimple(NPChar, SKILL_COMMERCE) + GetCharacterSkillSimple(NPChar, SKILL_LEADERSHIP)) / 2);
			sld = GetColonyByIndex(sti(NPChar.ColonyIdx));
			iFortValue = 20;
			if (CheckAttribute(sld, "FortValue")) iFortValue = sti(sld.FortValue);
			// Gregg: полуторный сбор против штатной формулы. Обоснование внутриигровое:
			// Сан-Хуан при нас стал пиратской базой, корсары заходят свободно и тратят
			// здесь то, что взяли в море. Считаем целыми, без float: x + x/2.
			iPerDay = iFortValue * iTax;
			iPerDay = iPerDay + iPerDay / 2;
			NPChar.Gover_Tax_Sum = iPerDay * iTemp;
			dialog.text = "С тех пор как корсары заходят к нам без опаски, город берёт " + FindRussianMoneyString(iPerDay) + " в день. За " + FindRussianDaysString(iTemp) + " набралось " + FindRussianMoneyString(sti(NPChar.Gover_Tax_Sum)) + ".";
			if (sti(NPChar.Gover_Tax_Sum) > 0)
			{
				link.l1 = "Забираю всё.";
				link.l1.go = "Gover_Tax_Get";
			}
			link.l2 = "Пусть лежит. Хорошей службы, Рэксвелл.";
			link.l2.go = "exit";
		break;

		case "Gover_Tax_Get":
			SaveCurrentNpcQuestDateParam(NPChar, "StartTaxDate");
			AddMoneyToCharacter(PChar, sti(NPChar.Gover_Tax_Sum));
			NextDiag.CurrentNode = "Gover_Main";
			DialogExit();
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
