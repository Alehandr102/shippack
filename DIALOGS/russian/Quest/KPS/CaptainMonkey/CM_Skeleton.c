// Gregg: охрана грота - квест «Корабль обезьян» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\captainmonkey.c
// Скелеты стоят смирно, пока их не тронешь, поэтому с ними МОЖНО заговорить -
// и без этого файла игра выдавала «Ошибка открытия файла диалога». Разговаривать
// им нечем, так что весь диалог - это хрип и попытка ГГ его как-то истолковать.

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
			dialog.text = RandPhraseSimple("Х-х-х-р-р-р...", "Кхр-р-р... с-с-с-х-х...");
			link.l1 = RandPhraseSimple("Понятия не имею, что это значит.", "Будем считать, что мы поговорили.");
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
