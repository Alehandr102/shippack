// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat("Все сплетни города "+ GetCityName(npchar.city) +" к вашим услугам. Чтобы вы хотели узнать?",
                          "Мы только что поднимали это тему. Вы, вероятно, запамятовали...", "Сегодня вы уже третий раз говорите о каком-то вопросе...",
                          "Что ты "+ GetSexPhrase("заладил","заладила") +" как попугай одно и то же...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Знаешь, " + NPChar.name + ", как-нибудь в следующий раз.", "Точно, "+ GetSexPhrase("забыл","забыла") +" что-то...",
                      "Да уж, действительно в третий раз...", "Да уж...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			 // ==> квест "Странные вещи творятся на архипелаге
            if (CheckAttribute(pchar, "questTemp.PKM_SvtvA_PoiskUchitel_Tavern"))
            {
                link.l1 = "Мне нужна информация об одном человеке. Он постоянно ходит в красном балахоне, и называет себя Учителем.";
                link.l1.go = "PKM_SvtvA_PU_1";
            }
			// Gregg: «Товар для Ричарда Тилльманна» - где искать Преноля
			if (PChar.UneasyDelivery == "2" && !CheckAttribute(PChar, "UneasyDelivery.TavernPorPax"))
			{
				link.l8 = "Послушай, ты не знаешь, где я могу найти Жака Преноля?";
				link.l8.go = "UneasyDelivery_1";
			}

			// Gregg: «Чёрная Орхидея» (перенос из К:ПС) - трактирщик знает, где жил
			// Брайан Джордж, и помнит ту самую драку. Атрибута может не быть вовсе,
			// потому проверки отдельные: короткого замыкания в Storm нет.
			if (CheckAttribute(PChar, "MysteriousArtefact"))
			{
				if (PChar.MysteriousArtefact == "3")
				{
					link.l9 = "Слушай, ты не знаешь, где находится дом Брайана Джорджа?";
					link.l9.go = "MysteriousArtefact_1";
				}
				if (PChar.MysteriousArtefact == "6")
				{
					if (!CheckAttribute(PChar, "MysteriousArtefact.BarmenTalk"))
					{
						link.l10 = "Помнишь, две недели назад Брайан Джордж устроил у тебя дебош? Хотел бы услышать подробности.";
						link.l10.go = "MysteriousArtefact_9";
					}
				}
			}
		break;

		case "UneasyDelivery_1":
			dialog.text = "Он живёт по соседству с портовым управлением. Станешь лицом к управлению - дверь его дома по левую руку.";
			link.l1 = "А что ты можешь о нём сказать? С ним можно иметь дело?";
			link.l1.go = "UneasyDelivery_2";
		break;

		case "UneasyDelivery_2":
			PChar.UneasyDelivery.TavernPorPax = true;
			dialog.text = "С Жаком? Конечно, можно, если ты не глуп. Преноль хитёр, и тут главное - не дать ему себя одурачить. Это он может. Так что будь осторожен.";
			link.l1 = "Что же, спасибо за помощь. Счастливо оставаться.";
			link.l1.go = "exit";
		break;

		//Квест "Странные вещи творятся на архипелаге"
		case "PKM_SvtvA_PU_1":
			dialog.text = "А, помню такого. Но его уже арестовали. Говорят, пробовал поднять бунт. Так что, если хотите его найти - вам прямиком в тюрьму Порт-о-Принса.";
			link.l1 = "Спасибо, ты мне очень помог.";
			link.l1.go = "exit";

			DeleteAttribute(pchar, "questTemp.PKM_SvtvA_PoiskUchitel_Tavern");

			AddQuestRecord("PKM_Animists", "27");
			AddQuestUserData("PKM_Animists", "sSex", GetSexPhrase("","а"));

			PChar.quest.PKM_SvtvA_TuremchikQuest.win_condition.l1 = "location";
			PChar.quest.PKM_SvtvA_TuremchikQuest.win_condition.l1.location = "PortPax_prison";
			PChar.quest.PKM_SvtvA_TuremchikQuest.win_condition = "PKM_SvtvA_TuremchikQuest";

			sld = GetCharacter(NPC_GenerateCharacter("Satanist_Uchitel", "Animists1", "man", "man", sti(pchar.rank), PIRATE, -1, false));
			sld.name = "Учитель";
			sld.lastname = "";
			LAi_SetStayType(sld);
			LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
			sld.Dialog.Filename = "Quest\PKM\Strannie_veshi_tvorytsya_v_arhipelage.c";
			sld.dialog.currentnode = "Учитель_Смерть";
			ChangeCharacterAddressGroup(sld, "PortPax_prison", "goto",  "goto9");
		break;
		//==> Gregg: Чёрная Орхидея - таинственный артефакт
		case "MysteriousArtefact_1":
			dialog.text = "Брайана Джорджа? Это тот чёртов пьяница, что каждый день дебоширил у меня в таверне?";
			link.l1 = "Ммм... Да, похоже, это именно он.";
			link.l1.go = "MysteriousArtefact_2";
		break;

		case "MysteriousArtefact_2":
			dialog.text = "Живёт на окраине Порт-о-Принса, в нескольких шагах от городских ворот.";
			link.l1 = "Спасибо за помощь.";
			link.l1.go = "MysteriousArtefact_3";
		break;

		case "MysteriousArtefact_3":
			dialog.text = "Эй, постой. На что тебе сдался этот чёртов алкаш?";
			link.l1 = "Он задолжал мне несколько сотен золотых. Хочу их вернуть.";
			link.l1.go = "MysteriousArtefact_4";
			link.l2 = "У меня к нему дело.";
			link.l2.go = "MysteriousArtefact_8";
		break;

		case "MysteriousArtefact_4":
			dialog.text = "Пустое дело, капитан. Он мне десять пиастров который день вернуть не может, а вы хотите из него несколько сотен вытрясти. Ничего не выйдет.";
			link.l1 = "У меня есть проверенные способы возвращать долги, даже когда у человека нет денег.";
			link.l1.go = "MysteriousArtefact_5";
		break;

		case "MysteriousArtefact_5":
			dialog.text = "Вы собираетесь его убить, капитан?";

			if (sti(PChar.money) >= 200)
			{
				link.l1 = "А ты что, против?";
				link.l1.go = "MysteriousArtefact_6";
			}

			link.l2 = "Нет, убивать я его не собираюсь, я о других способах. Более действенных. А сейчас я спешу. Прощай.";
			link.l2.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToHouseGeorgeBryan");
		break;

		case "MysteriousArtefact_6":
			dialog.text = "Конечно против, капитан! Убивать человека, пусть даже такого, как Брайан Джордж... Мой долг - позвать стражу.";
			link.l1 = "Послушай, я думаю, тебе станет легче, если Брайана Джорджа не станет. А ещё легче - когда ты получишь от меня пару сотен золотых. С деньгами-то всегда живётся веселее, не правда ли?";
			link.l1.go = "MysteriousArtefact_7";
		break;

		case "MysteriousArtefact_7":
			AddMoneyToCharacter(PChar, -200);
			dialog.text = "Давай их сюда! Скорее!";
			link.l1 = "Держи.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToHouseGeorgeBryan");
		break;

		case "MysteriousArtefact_8":
			dialog.text = "Дело? Ну, насмешил. Какое у тебя может быть дело к этому пьянице? Он до дома сам добраться не может, а ты ему дело предлагаешь. Он даже десять пиастров на улице выклянчить не в силах, чтобы вернуть мне долг.";
			link.l1 = "Дело очень важное, я спешу. Прощай.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToHouseGeorgeBryan");
		break;

		case "MysteriousArtefact_9":
			dialog.text = "На что они тебе, капитан?";
			link.l1 = "Брайан после той драки потерял ключ, а он мне нужен. Может, из твоего рассказа что-нибудь прояснится.";
			link.l1.go = "MysteriousArtefact_10";
		break;

		case "MysteriousArtefact_10":
			dialog.text = "Обычная драка была, что тут сказать. Участвовали все, я и сам старое вспомнил, пару раз врезал одному проходимцу. Потом успокоились, разошлись - словом, всё как обычно.";
			link.l1 = "Никакого ключа после драки не находил?";
			link.l1.go = "MysteriousArtefact_11";
		break;

		case "MysteriousArtefact_11":
			dialog.text = "Нет, не находил.";
			link.l1 = "А официантка?";
			link.l1.go = "MysteriousArtefact_12";
		break;

		case "MysteriousArtefact_12":
			PChar.MysteriousArtefact.BarmenTalk = true;
			dialog.text = "А что официантка? Всю драку в углу просидела, я её потом домой отпустил - перепугана была насмерть.";
			link.l1 = "Что ж, ясно. Спасибо.";
			link.l1.go = "exit";
		break;
		//<== Gregg: Чёрная Орхидея

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

