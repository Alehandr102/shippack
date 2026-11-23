
void ProcessDialogEvent()
{
	ref NPChar, sld, chr;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "TohmasPetit_1":
			dialog.text = "Здравствуйте, "+GetSexPhrase("уважаемый","уважаемая")+". Я вижу вы при оружии, да и выглядите как "+GetSexPhrase("умелый фехтовальщик","умелая фехтовальщица")+".";
			link.l1 = "Да, ты не ошибся.";
			link.l1.go = "TohmasPetit_2";
		break;
		
		case "TohmasPetit_2":
			dialog.text = "Мне срочно нужна помощь такого человека, как вы!";
			link.l1 = "Расскажи, что случилось.";
			link.l1.go = "TohmasPetit_3";
		break;
		
		case "TohmasPetit_3":
			dialog.text = "Местные бродяги напали на меня и мою невесту сразу за воротами и утащили её в джунгли! Я - плохой фехтовальщик, и мне с ними не справиться. Прошу вас, помогите мне. Мой отец является уважаемым торговцем в городе. Если вы поможете её отбить, мы заплатим очень щедро!";
			link.l1 = "Хорошо, я тебе помогу. Веди, парень.";
			link.l1.go = "TohmasPetit_ChooseYes";
			link.l2 = "Свои проблемы решай сам. У меня дел по горло.";
			link.l2.go = "TohmasPetit_ChooseNo";
		break;

		case "TohmasPetit_ChooseNo":
			NextDiag.CurrentNode = "TohmasPetit_5";
			pchar.questdata.TohmasPetit = 1;
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
			npchar.lifeday = 5;
			DialogExit();
		break;

		case "TohmasPetit_5":
			dialog.text = "А, это ты. Мне некогда!";
			link.l1 = "...";
			link.l1.go = "Exit";
			NextDiag.TempNode = "TohmasPetit_5";
		break;

		case "TohmasPetit_ChooseYes":
			DialogExit();
			NextDiag.CurrentNode = "TohmasPetit_6";
			SetQuestHeader("TohmasPetit");
			AddQuestRecord("TohmasPetit", "QuestTaken");
			DoQuestCheckDelay("TohmasPetit_yes", 0.1);
		break;
		
		case "TohmasPetit_6":
			if (GetOfficersQuantity(pchar) > 0)
			{
				dialog.text = "Капитан, перед тем как продолжить путь, прошу вас попросить ваших офицеров подождать в городе. Если бандиты увидят толпу, они могут убить мою невесту и сбежать, а я этого не переживу.";
				link.l1 = "Хорошо, не беспокойся об этом.";
				link.l1.go = "TohmasPetit_ProcessGoFurther1";
				link.l2 = "А почему ты раньше об этом не сказал, парень? Всё это выглядит очень подозрительно.";
				link.l2.go = "TohmasPetit_8";
			}
			else
			{
				dialog.text = "Прекрасно, капитан, нам нельзя терять ни минуты. Те мерзавцы утащили мою невесту дальше в джунгли, нужно спешить, пока они не навредили моей Леблан. Следуйте за мной!";
				link.l1 = "Хорошо, веди.";
				link.l1.go = "TohmasPetit_ProcessGoFurther";
			}
		break;

		case "TohmasPetit_8":
			dialog.text = "Капитан! Ничего подобного, уверяю вас. Прошу вас капитан, давайте поторопимся, пока эти мрази не причинили вред моей невесте.";
			link.l1 = "Хорошо, я верю тебе, я дам приказ своим людям, чтобы они ждали в городе.";
			link.l1.go = "TohmasPetit_ProcessGoFurther1";
			link.l2 = "Не, мне кажется ты что-то темнишь, Томас. Поэтому я передумал"+GetSexPhrase("","а")+", выручай свою невесту сам.";
			link.l2.go = "TohmasPetit_9";
			NextDiag.TempNode = "TohmasPetit_5";
		break;
		
		case "TohmasPetit_9":
			dialog.text = "Это очень подло с вашей стороны, весь город узнает о том, что вы "+GetSexPhrase("трус","трусиха")+"!";
			link.l1 = "Да, да. Проваливай отсюда.";
			link.l1.go = "TohmasPetit_ProcessCancel";
		break;
		
		case "TohmasPetit_10":
			dialog.text = "Капитан, вон они, а также моя невеста. Я сейчас подойду к ним, скажу, что принёс золото, и постараюсь отвлечь их. Не хочу, чтобы они нанесли вред моей невесте. Подождите тут, я постараюсь их отвлечь, а затем следуйте за мной.";
			link.l1 = "Хороший план, иди.";
			link.l1.go = "TohmasPetit_ubejal";
		break;
		
		case "TohmasPetit_ubejal":
			AddDialogExitQuest("TohmasPetit_Rastanovka");
			DialogExit();
		break;
		
		case "TohmasPetit_ProcessCancel":
			AddDialogExitQuest("TohmasPetit_konec");
			AddQuestRecord("TohmasPetit", "QuestRefused");
			CloseQuestHeader("TohmasPetit");
			DialogExit();
			pchar.questdata.TohmasPetit = 1;
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
		break;

		case "TohmasPetit_ProcessGoFurther1":
			DeleteAllOfficersFromLocation();
			AddQuestRecord("TohmasPetit", "SuspiciousRequest");
			AddQuestUserData("TohmasPetit", "sSex", GetSexPhrase("ся","ась")); // LEO: пол ГГ - "согласился/согласилась"
			AddDialogExitQuest("TohmasPetit_to_jungle");
			NextDiag.CurrentNode = "TohmasPetit_10";
			DialogExit();
		break;
		
		case "TohmasPetit_ProcessGoFurther":
			AddQuestRecord("TohmasPetit", "SuspiciousRequest");
			AddQuestUserData("TohmasPetit", "sSex", GetSexPhrase("ся","ась")); // LEO: пол ГГ - "согласился/согласилась"
			AddDialogExitQuest("TohmasPetit_to_jungle");
			NextDiag.CurrentNode = "TohmasPetit_10";
			DialogExit();
		break;

		case "exit":
			if (CheckAttribute(NextDiag,"TempNode")) NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}	
}
