// Gregg: Хорхе де Маранья - «Чёрная Орхидея», глава 8 «Рауль Эсперанто».
// Логика - Program\Quests\reaction_functions\blackorchid.c.

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
			if (PChar.RaulEsperanto == "3")
			{
				dialog.text = "Опять вы. Ну?";
				if (sti(PChar.money) >= BO_RE_BRIBE)
				{
					link.l1 = "Деньги при мне.";
					link.l1.go = "4";
				}
				link.l2 = "Ничего.";
				link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Проход закрыт. Идите своей дорогой.";
				link.l1 = "Иду.";
				link.l1.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Немедленно покиньте эту территорию. Здесь находиться запрещено.";
			link.l1 = "Что тут случилось?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Вас это не касается. Уходите.";
			link.l1 = "Пять тысяч пиастров.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(поворачивается)... Сколько?";
			// Gregg: если деньги при себе, платим тут же - гонять игрока за тем, что у него
			// в кошельке, глупо.
			if (sti(PChar.money) >= BO_RE_BRIBE)
			{
				link.l1 = "Пять тысяч. За несколько ответов. Вот они, считайте.";
				link.l1.go = "4";
			}
			else
			{
				link.l1 = "Пять тысяч. За несколько ответов. Сейчас при мне столько нет.";
				link.l1.go = "REPLY";
			}
		break;

		case "REPLY":
			dialog.text = "Тогда и разговора нет. Приносите - поговорим, я никуда не денусь.";
			link.l1 = "Принесу.";
			link.l1.go = "exit";
			PChar.RaulEsperanto = "3";
			// Gregg: диалог открывал квест, и Хорхе остался актёром. Пока его не вернуть
			// в stay, он не заговорит при следующем подходе - просто стоит столбом.
			LAi_SetStayType(NPChar);
			LAi_SetPlayerType(PChar);
		break;

		case "4":
			dialog.text = "Вот это по-нашему. Спрашивайте.";
			link.l1 = "Почему нельзя войти в дом?";
			link.l1.go = "5";
			AddMoneyToCharacter(PChar, -BO_RE_BRIBE);
			LAi_SetStayType(NPChar);
		break;

		case "5":
			dialog.text = "Приказ генерал-губернатора. Рауль Эсперанто ставил тут какие-то опыты, был взрыв, задело людей на улице.";
			link.l1 = "Где сам Эсперанто?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Арестован по приказу генерал-губернатора.";
			link.l1 = "Я спроси" + GetSexPhrase("л", "ла") + " где, а не что с ним.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "А вот на это, капитан, пяти тысяч мало. И десяти мало. Я не жадничаю, я считаю, чем рискую.";
			link.l1 = "Тогда назови не цену, а условие.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Условие. Через шесть дней у берегов Тортуги встанет пиратский боевой корабль. Возьмите его на абордаж, убейте капитана, снимите с тела ключ и откройте сундук в каюте. То, что там лежит, привезёте мне на мыс Сан-Антонио.";
			link.l1 = "Что там лежит?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Увидите, когда откроете.";
			link.l1 = "Хорошо. Мыс Сан-Антонио.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Шесть дней, капитан. Опоздаете - корабль уйдёт, и разговор наш кончится.";
			link.l1 = "Не опоздаю.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoToTortuga");
		break;

		case "12":
			dialog.text = "Привезли?";
			link.l1 = "Держи. Откуда ты знал, что мушкет будет именно там?";
			link.l1.go = "13";
		break;

		case "13":
			// Gregg: в записи журнала колония подставляется через @<sPrison>, но в реплике
			// диалога плейсхолдеры не работают - имя собираем в рантайме.
			string sPrisonKey = "Colony" + PChar.RaulEsperanto.Prison + "Gen";
			dialog.text = "Услышал разговор, который слышать не полагалось. Теперь о деле. Эсперанто держат в тюрьме колонии " + XI_ConvertString(sPrisonKey) + ".";
			link.l1 = "Почему именно там?";
			link.l1.go = "14";
			TakeNItems(PChar, "RaulEsperanto_Musket", -1);
			TakeNItems(PChar, "RaulEsperanto_Key", -1);
		break;

		case "14":
			dialog.text = "Потому что туда из Европы едет главный инквизитор, и он будет его судить. Эсперанто который год возится с египетским порошком мумий и водит знакомство с Аскольдом. Знаете Аскольда?";
			link.l1 = GetSexPhrase("Слышал", "Слышала") + ". На Гваделупе.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Он самый. Аскольд у инквизиции в списках давно. Как только они узнали, что Эсперанто с ним переписывается, взяли его в тот же месяц. Он у них под боком был.";
			link.l1 = "И судить его будут там же?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Я же сказал: инквизитор едет туда. Возить арестанта через полархипелага никто не станет. И вы поторопитесь: он вот-вот сойдёт на берег.";
			link.l1 = "Спасибо, Хорхе.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("RaulEsperantoToNeedPrison");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
