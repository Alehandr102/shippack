// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
 	switch(Dialog.CurrentNode)
	{
        case "quests":
            // Gregg: «Чёрная Орхидея», глава 8 - расспрос про Эсперанто в тюрьме Чарльстауна
            if (NPChar.City == "Charles")
            {
                if (CheckAttribute(PChar, "RaulEsperanto"))
                {
                    if (PChar.RaulEsperanto == "1")
                    {
                        link.l30 = "Я ищу человека, которого привезли к вам лет тридцать назад. Рауль Эсперанто, испанец.";
                        link.l30.go = "BO_RE_Prison_1";
                    }
                }
            }
            dialog.text = "Говорите, я слушаю.";
			link.l1 = "Я "+ GetSexPhrase("ошибся","ошиблась") +". Прощайте.";
			link.l1.go = "Exit";
		break;
        case "BO_RE_Prison_1":
            dialog.text = "Тридцать лет. ...(усмехается)... У меня книги за десять, остальное сгорело. Но испанца по фамилии Эсперанто я бы запомнил: у нас их тут не держат.";
            link.l1 = "Значит, записей нет вовсе?";
            link.l1.go = "BO_RE_Prison_2";
        break;

        case "BO_RE_Prison_2":
            dialog.text = "Нет. И спрашивать вам лучше не у меня. ...(понижает голос)... У нас в подвале раз в месяц бывает один старик. Он тут сидел ещё при моём предшественнике, и с тех пор ничего не изменилось, кроме того, что он умер.";
            link.l1 = "Призрак?";
            link.l1.go = "BO_RE_Prison_3";
        break;

        case "BO_RE_Prison_3":
            dialog.text = "Называйте как хотите. Джо Ньюмен его звали. Он помнит всех, кто тут сидел, и, в отличие от меня, ничего не забыл. Только вниз к нему никто не ходит: он убил четверых.";
            link.l1 = "А я схожу.";
            link.l1.go = "BO_RE_Prison_4";
        break;

        case "BO_RE_Prison_4":
            dialog.text = "Дело ваше, капитан. Его день бывает раз в месяц, и когда он настанет, вы поймёте: в этот день у нас никто не спускается вниз даже за вином.";
            link.l1 = "Дождусь.";
            link.l1.go = "exit";
            AddDialogExitQuestFunction("RaulEsperantoNewmanTime");
        break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
