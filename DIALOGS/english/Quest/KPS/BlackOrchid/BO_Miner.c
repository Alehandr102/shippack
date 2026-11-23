// Gregg: A runaway convict in the cave near Cumana - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "Stand off.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "You go no further.";
			link.l1 = "I want the chest.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(laughs)... Everybody wants the chest. Eight years everybody wants the chest. Nobody can open it, and everybody wants it.";
			link.l1 = "I have the key.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(stops laughing)... Show me. ...(looks at it a long while)... That is the one. The black man had it.";
			link.l1 = "What black man?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "The one who brought the chest here and left it for us to guard. He said: keep it, a man will come with a key. We have kept it eight years and no man came.";
			link.l1 = "The man has come.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "He has. ...(raises his axe)... Only nobody told us what to do when he did. So we shall do it our own way.";
			link.l1 = "...(draws)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyBattleWithMiners");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
