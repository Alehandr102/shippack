// Gregg: Беглый каторжник в пещере у Куманы - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "Отойди.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Дальше не пойдёшь.";
			link.l1 = "Мне нужен сундук.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(смеётся)... Всем нужен сундук. Восемь лет всем нужен сундук. Открыть его никто не может, а нужен всем.";
			link.l1 = "У меня ключ.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(перестаёт смеяться)... Покажи. ...(долго смотрит)... Он самый. Тот, что был у чёрного.";
			link.l1 = "У какого чёрного?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "У того, что привёз сюда сундук и оставил его нам стеречь. Сказал: сторожите, придёт человек с ключом. Мы восемь лет сторожим, а человека нет.";
			link.l1 = "Человек пришёл.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Пришёл. ...(поднимает топор)... Только нам не сказали, что делать, когда он придёт. Значит, будем делать по-своему.";
			link.l1 = "...(берётся за клинок)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyBattleWithMiners");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
