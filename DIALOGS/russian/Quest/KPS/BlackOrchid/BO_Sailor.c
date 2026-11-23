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
			dialog.text = "Всё готово, капитан. Ждём вашего слова.";
			link.l1 = "Скоро выходим.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Капитан! Живой... Мы вас который день вдоль всего берега ищем, уже и надеяться перестали.";
			link.l1 = "Как вы вообще узнали, где меня искать?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Так письмо пришло. В Белизе курьер сунул боцману прямо на пристани: мол, ваш капитан в бухте Аматике, поторопитесь. Ни подписи, ни имени. Боцман его за грудки - от кого? А тот и сам не знает: заплатили вперёд, велели передать.";
			link.l1 = "И вы поверили клочку бумаги без подписи?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(пожимает плечами)... А что нам оставалось, капитан? Других вестей о вас не было вовсе. Пришли - и вот вы стоите. Значит, не соврали.";
			link.l1 = "Значит, не соврали. Что с судном?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Починили и снарядили, всё как положено. Плотник ворчал, но управились. Ждём вас, капитан.";
			link.l1 = "Скоро выходим.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time"; // LEO: иначе после дочитанного разговора узел уезжал в пустоту
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
