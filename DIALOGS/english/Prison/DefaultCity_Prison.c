// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
 	switch(Dialog.CurrentNode)
	{
        case "quests":
            // Gregg: The Black Orchid, chapter 8 - asking after Esperanto at the Charlestown prison
            if (NPChar.City == "Charles")
            {
                if (CheckAttribute(PChar, "RaulEsperanto"))
                {
                    if (PChar.RaulEsperanto == "1")
                    {
                        link.l30 = "I am looking for a man brought to you some thirty years ago. Raul Esperanto, a Spaniard.";
                        link.l30.go = "BO_RE_Prison_1";
                    }
                }
            }
            dialog.text = "Speak, I'm listening.";
			link.l1 = "I "+ GetSexPhrase("wrong", "wrong") +". Goodbye.";
			link.l1.go = "Exit";
		break;
        case "BO_RE_Prison_1":
            dialog.text = "Thirty years. ...(smirks)... My books go back ten, the rest burned. But a Spaniard named Esperanto I would remember: we do not keep them here.";
            link.l1 = "So there is no record at all?";
            link.l1.go = "BO_RE_Prison_2";
        break;

        case "BO_RE_Prison_2":
            dialog.text = "None. And it is not me you should be asking. ...(drops his voice)... Once a month there is an old man in our cellar. He was held here under my predecessor, and nothing has changed since except that he died.";
            link.l1 = "A ghost?";
            link.l1.go = "BO_RE_Prison_3";
        break;

        case "BO_RE_Prison_3":
            dialog.text = "Call it what you like. Joe Newman was his name. He remembers everyone ever held here and, unlike me, has forgotten nothing. Only nobody goes down to him: he has killed four men.";
            link.l1 = "I will go down.";
            link.l1.go = "BO_RE_Prison_4";
        break;

        case "BO_RE_Prison_4":
            dialog.text = "Your affair, Captain. His day comes once a month, and you will know it when it does: on that day nobody here goes below even for wine.";
            link.l1 = "I will wait for it.";
            link.l1.go = "exit";
            AddDialogExitQuestFunction("RaulEsperantoNewmanTime");
        break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
