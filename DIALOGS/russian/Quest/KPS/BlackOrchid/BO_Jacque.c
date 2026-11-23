// Gregg: Жак, бродяга в доме Брайана Джорджа - трилогия «Чёрная Орхидея» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c
// Узлы 1-7 - внизу у сундука, три способа спровадить его наверх; 8-15 - наверху,
// где он вспоминает историю с потерянным ключом.

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
			NextDiag.TempNode = "First time";
			dialog.text = "Чего надо?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Три сотни парней на бочонок винца! И-ик... Или как там поётся? Эй, а ты кто такой?";
			link.l1 = "А ты кто? И что делаешь в доме Брайана Джорджа?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "И-ик... А тебе что за дело? П" + GetSexPhrase("шёл", "ошла") + " вон! И-ик...";
			link.l1 = "Я друг Брайана. Так что ты делаешь в его доме?";
			link.l1.go = "3";
			link.l2 = "Брайан забыл вернуть мне одну вещицу, а она нужна позарез. Поищу, ты не против?";
			link.l2.go = "6";
			link.l3 = "Заткнись, развалина. " + GetSexPhrase("Я пришёл", "Я пришла") + " забрать своё. Ещё раз пикнешь - пеняй на себя, я сегодня злой.";
			link.l3.go = "7";
		break;

		case "3":
			dialog.text = "Друг? Брайана? Так я тоже друг Брайана. И-ик... Будем знакомы. Жак. Просто Жак.";
			link.l1 = "Ты не ответил. Что ты делаешь в его доме?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Так эта... он сам предложил пожить, пока его нет. Я тут уже давно. Неплохой домик. А ты зачем приш" + GetSexPhrase("ёл", "ла") + "?";
			link.l1 = "Забрать одну вещь. У Брайана память дырявая, вернуть забыл. Поищу?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Ищи. А я пойду прилягу, устал очень.";
			link.l1 = "Верное решение. Иди проспись, а я поищу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInHouseGeorgeBryanJacqueToRoom");
		break;

		case "6":
			dialog.text = "Валяй. И-ик... А я посплю.";
			link.l1 = "Вот и славно.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInHouseGeorgeBryanJacqueToRoom");
		break;

		case "7":
			dialog.text = "Что?! Сейчас выясним, кто тут развалина. И-ик... Доставай шпагу! Что, струсил? То-то. Языком чесать все горазды, а как драться - так вас нет. И-ик... Что-то я перебрал маленько. Пойду-ка спать.";
			link.l1 = "Давно пора. Вали отсюда.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInHouseGeorgeBryanJacqueToRoom");
		break;

		case "8":
			dialog.text = "И-ик... Ну что ещё? Только прилечь собрался, а тут ты.";
			link.l1 = "Не знаешь, где ключ от сундука внизу? Похоже, то, что мне нужно, именно там, а без ключа его не открыть.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Ничем не помогу, извини. Брайан ключ-то потерял.";
			link.l1 = "Потерял? И давно?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Не помню... Хотя нет, постой, вспомнил. Ровно две недели назад. В тот день мы с Брайаном затеяли в таверне славную драку, надолго её запомнят. А после Брайан ключа и хватился. Хотел поискать, да трактирщик его за дверь вытолкал. Брайан обозлился и полез на мушкетёра.";
			link.l1 = "На мушкетёра? И что потом?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Ничего. Тот его с размаху прикладом уложил. Брайан так до утра и провалялся у входа.";
			link.l1 = "Может, ключ трактирщик нашёл? Или официантка?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Трактирщик не находил, это точно - мы бы узнали. А официантка сразу забилась в угол и до конца драки не высовывалась. Не могла она найти.";
			link.l1 = "А потом, когда убиралась?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Да нет, трактирщик один убирался. Её сразу домой отпустил, перепугана была насмерть.";
			link.l1 = "Может, кто из посетителей подобрал?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Да что ты привязал" + GetSexPhrase("ся", "ась") + " ко мне со своим ключом! Спать хочу.";
			link.l1 = "Хе. Да ты и так неплохо спишь. Хоть до кровати дошёл.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "А мне и так хорошо. Всё, надоел. Дай поспать!";
			link.l1 = "Ладно-ладно, ухожу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToPortPaxTavern");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
