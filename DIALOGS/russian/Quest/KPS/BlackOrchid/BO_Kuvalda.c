// Gregg: Кристобаль Кувалда, наёмный боец с секирой - трилогия «Чёрная Орхидея»
// (перенос из К:ПС). Логика - Program\Quests\reaction_functions\blackorchid.c
// Глава посёлка ловцов жемчуга просит привести Кувалду на подмогу перед налётом
// слуг Ферегуэйро:
//   First time - реплика-заглушка;
//   1-2 - у входа в пещеры, сразу после схватки с людоедами: уговариваемся идти в посёлок;
//   3-8 - посёлок отбит, Кувалда хвастает своим ударом; три ветки ответа сходятся
//         на одном - идти с докладом к главе посёлка.
// В оригинале ГГ звал главу посёлка Родриго, хотя имя тому выдаётся случайное при
// генерации (HeadMan_1), - у нас он просто «глава посёлка».

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sBro = GetSexPhrase("брат", "сестра");

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "Чего надо?";
			link.l1 = "Ничего.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Это слуги Ферегуэйро, " + sBro + "! Надо предупредить посёлок - Ферегуэйро вернулся!";
			link.l1 = "Глава посёлка уже предупреждён, там готовятся к обороне. А меня послали за тобой - без тебя нам не отбиться.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Знаю, сам уже собирался туда бежать. Чёрт, надо торопиться, времени совсем в обрез! Да, и последнее. Спасибо за выручку, " + sBro + ".";
			link.l1 = "Не за что, Кристобаль. Времени мало, бежим в посёлок.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftToPearlVillageJungle");
		break;

		case "3":
			dialog.text = "Эх, вот это была мясорубка! Видал" + GetSexPhrase("", "а") + ", " + sBro + ", как я одного хмыря уложил?";
			link.l1 = "Некогда мне было на тебя смотреть, своих хватало.";
			link.l1.go = "4";
			link.l2 = "Не видел, хотя нет-нет да поглядывал, как у тебя дела.";
			link.l2.go = "6";
			link.l3 = "Ещё бы не видал! Знатный был удар.";
			link.l3.go = "8";
		break;

		case "4":
			dialog.text = "Плохо, " + sBro + ". За друзьями в бою надо приглядывать хоть краем глаза: вдруг кому подмога нужна - подбежал, выручил.";
			link.l1 = "Ладно, учту. Буду поглядывать.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Вот и славно. Ступай, " + sBro + ", доложи, что мы отбились, а я тебя тут подожду.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftInPearlVillageToOfficerHouse");
		break;

		case "6":
			dialog.text = "Эх, такое надо было видеть! Летит на меня один из этих, я размахнулся по-молодецки да и приложил его секирой плашмя. Думал только оглушить, а силу не рассчитал - голову ему и разнёс.";
			link.l1 = "Потренируйся рассчитывать удар, когда собираешься кого-то оглушить.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Ладно, " + sBro + ", ступай, доложи, что мы отбились, а я тебя тут подожду.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftInPearlVillageToOfficerHouse");
		break;

		case "8":
			dialog.text = "Я и сам подивился. Хотел ведь только оглушить, а силу не рассчитал - и убил.";
			link.l1 = "Потренируйся рассчитывать удар, когда собираешься кого-то оглушить.";
			link.l1.go = "7";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
