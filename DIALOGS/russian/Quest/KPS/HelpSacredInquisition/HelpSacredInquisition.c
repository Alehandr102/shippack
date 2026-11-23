// Gregg: диалоги квеста "Помощь святой инквизиции" (перенос из К:ПС).
// Стража за воротами Бас-Тера, Жаклин Дефо на Тортуге, Натан Рич на Бермудах.
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "Да?";
			link.l1 = "Нет.";
			link.l1.go = "exit";
		break;

		case "BasTer_Guardians_1":
			dialog.text = "Стоять! Ты арестован"+ GetSexPhrase("","а") +" за убийство этого человека.";
			link.l1 = "Но я его не убивал"+ GetSexPhrase("","а") +"!";
			link.l1.go = "BasTer_Guardians_2";
		break;

		case "BasTer_Guardians_2":
			AddDialogExitQuestFunction("HelpSacredInquisitionFightWithSoldAtExitBasTer");
			dialog.text = "Не отпирайся. Один горожанин сам рассказал нам, что ты пош"+ GetSexPhrase("ёл","ла") +" его убивать. Сдай оружие и следуй за нами.";
			link.l1 = "Ну уж нет.";
			link.l1.go = "exit";
		break;

		case "Jaklin_Defo_1":
			dialog.text = "Что вам угодно, месье?";
			link.l1 = "Вы Жаклин Дефо? Жена Франсуа Дефо?";
			link.l1.go = "Jaklin_Defo_2";
		break;

		case "Jaklin_Defo_2":
			dialog.text = "Да. А вы кто?";
			link.l1 = "Знаком"+ GetSexPhrase("","а") +" с вашим мужем.";
			link.l1.go = "Jaklin_Defo_3";
		break;

		case "Jaklin_Defo_3":
			dialog.text = "Отчего у вас такое лицо?..";
			link.l1 = "Соболезную. Вашего мужа больше нет.";
			link.l1.go = "Jaklin_Defo_4";
		break;

		case "Jaklin_Defo_4":
			dialog.text = "Как?!";
			link.l1 = "Его убили. И я ищу того, кто это сделал.";
			link.l1.go = "Jaklin_Defo_5";
		break;

		case "Jaklin_Defo_5":
			dialog.text = "Я... я не знаю. Приходите завтра, мне нужно прийти в себя.";
			link.l1 = "Понимаю и сочувствую. Но след убийцы стынет с каждым часом, и завтра его может уже не быть.";
			link.l1.go = "Jaklin_Defo_6";
		break;

		case "Jaklin_Defo_6":
			dialog.text = "...Он говорил про какой-то камень. Перед отплытием сказал, что мы будем богаты, когда он вернётся. Я чувствовала, что добром это не кончится.";
			link.l1 = "Что именно он говорил про камень? И называл ли имя - Натан Рич?";
			link.l1.go = "Jaklin_Defo_7";
		break;

		case "Jaklin_Defo_7":
			dialog.text = "Да, называл. Говорил, что Натан ему друг. А про камень - ничего такого.";
			link.l1 = "Где Натана можно найти?";
			link.l1.go = "Jaklin_Defo_8";
		break;

		case "Jaklin_Defo_8":
			NextDiag.TempNode = "Jaklin_Defo_9";
			AddDialogExitQuestFunction("HelpSacredInquisitionToBermudes");
			dialog.text = "Не знаю. Франсуа как-то обмолвился, что они часто встречаются на Бермудах. Обещайте, что найдёте убийцу!";
			link.l1 = "Сделаю всё, что смогу.";
			link.l1.go = "exit";
		break;

		case "Jaklin_Defo_9":
			NextDiag.TempNode = "Jaklin_Defo_9";
			dialog.text = "Простите, я сейчас не в силах говорить.";
			link.l1 = "Да, да... Простите.";
			link.l1.go = "exit";
		break;

		case "Natan_Rich_1":
			SetCameraDialogMode(CharacterFromID("Natan Rich Guardian 1"));
			dialog.text = "Да что у вас тут происходит?";
			link.l1 = "Ничего - если не считать шума, который ты поднял.";
			link.l1.go = "Natan_Rich_2";
		break;

		case "Natan_Rich_2":
			SetCameraDialogMode(CharacterFromID("Natan Rich Guardian 2"));
			dialog.text = "Это вышло случайно, а с Франсуа Джейк уже всё уладил.";
			link.l1 = "Ты заставляешь того, на кого работаешь, исправлять твои ошибки. Джейк Смитт тебе не игрушка. Играешь с огнём, приятель.";
			link.l1.go = "Natan_Rich_3";
		break;

		case "Natan_Rich_3":
			SetCameraDialogMode(CharacterFromID("Natan Rich Guardian 1"));
			dialog.text = "Понимаю. Надеюсь, разговор останется между нами. И что за Клейн, о котором вы говорили?";
			link.l1 = "У этого человека большие связи в Старом Свете, в Испании. Не вздумай ничего испортить - он нам очень нужен. Узнает тебя по твоему амулету, надеюсь, он при тебе. Ждать будет в трактире; не найдёшь - спроси у трактирщика, он у него отметится... Что это за шум? Слышите?";
			link.l1.go = "Natan_Rich_4";
		break;

		case "Natan_Rich_4":
			SetCameraDialogMode(NPChar);
			AddDialogExitQuestFunction("HelpSacredInquisitionInBermudesFirstFight");
			dialog.text = "У нас гости! Убить его!";
			link.l1 = "...";
			link.l1.go = "exit";
		break;

		case "Natan_Rich_5":
			dialog.text = "Да кто ты такой?! Что ты тут забыл" + GetSexPhrase("", "а") + "?!";
			link.l1 = "Капитан " + GetFullName(pchar) + ". По поручению Святой Инквизиции. Ты изрядно согрешил.";
			link.l1.go = "Natan_Rich_6";
		break;

		case "Natan_Rich_6":
			dialog.text = "Чёрт... Я знал, что они до меня доберутся. Ну, делай, что должен.";
			link.l1 = "Успею. Сначала расскажи, что за камень вы все ищете.";
			link.l1.go = "Natan_Rich_7";
		break;

		case "Natan_Rich_7":
			dialog.text = "Ладно. Раз уж нашёл... Это легенда о лунном камне призрачного света. Всё началось с сумасшедшего, который объявился на Карибах и уверял, что видел его. Говорил, камень даёт своему хозяину силу Бога.";
			link.l1 = "Это я слышал"+ GetSexPhrase("","а") +". Ближе к делу.";
			link.l1.go = "Natan_Rich_8";
		break;

		case "Natan_Rich_8":
			dialog.text = "Говорят, артефакт принадлежал атлантам. Они повелевали великой силой, но удержать её не сумели и заплатили жизнями. Мы взялись за поиски, когда пошёл слух, что камень давным-давно привёз сюда какой-то древний орден. Его последователи стерегут камень до сих пор. Где - нам неизвестно.";
			link.l1 = "Любопытно. А кто такой Джейк Смитт?";
			link.l1.go = "Natan_Rich_9";
		break;

		case "Natan_Rich_9":
			AddDialogExitQuestFunction("HelpSacredInquisitionInBermudesFightWithRich");
			dialog.text = "Вот этого я тебе не скажу. Ты и так узнал" + GetSexPhrase("", "а") + " слишком много - с этим и умрёшь.";
			link.l1 = "Хех...";
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
