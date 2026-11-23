// Gregg: посыльный с Бермуд - трилогия «Чёрная Орхидея» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c
// В оригинале эти два узла лежали в общем файле Pirates.c под номерами 8 и 9;
// у нас посыльный вынесен в свой файл, чтобы не тащить весь чужой диалог.

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
			dialog.text = "Что вам надо?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Ты - капитан " + GetFullName(PChar) + "?";
			link.l1 = "Да. Чего тебе надо?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "К вам меня послал " + GetFullName(CharacterFromID("Pirates_tavernkeeper")) + ", хозяин таверны на Бермудах. Просит зайти к нему - говорит, есть для вас интересные новости.";
			link.l1 = "Вот как. Хорошо, передай, что я его навещу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactFirstToBermudes");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
