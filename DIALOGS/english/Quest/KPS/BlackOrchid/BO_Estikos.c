// Gregg: Фернандо Эстикос - трилогия «Чёрная Орхидея», глава 5 (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c
// Спившийся компаньон Бернальдинио Торангеры, сидит в таверне Портобелло:
//   1     - зовёт выпить, ГГ подсаживается (DestinyGiftInPortoBelloTavern);
//   2-7   - что случилось со «Стервятником» и где видели его остов;
//   7     - выход, задание найти остов (DestinyGiftFindBernaldinioShip).
// Узлы 1 и 2 выставляет логика, остальные идут по цепочке.
// Gregg: в оригинале капитан видел остов «где-то на Мэйне» без подробностей, и игрока
// отправляли обыскивать всё побережье. У нас бухта с остовом доступна только пешком,
// с моря к ней не подойти, поэтому Фернандо называет место: мыс Несбывшихся Надежд,
// за Дес-Мойнесом. Иначе игрок ушёл бы искать точку на глобальной карте, которой нет.

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
			dialog.text = "Good luck to you, cap!";
			link.l1 = "And good health to you. And do stop drinking.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Care for a drink with me? Sit down then, what are you standing about for!";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftInPortoBelloTavern");
		break;

		case "2":
			dialog.text = "Innkeeper! Rum! And what shall we drink to, cap?";
			link.l1 = "There is always time to get drunk, Fernando. Tell me instead what became of your partner, of Bernaldinio Toranger.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "I do not know, cap. On our last voyage together a storm of fearful strength broke and drove us apart. I went on alone, and Bernaldinio never came into port. Neither he nor the Vulture. Although...";
			link.l1 = "Although what?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "I heard something from a trader in the tavern at Puerto Rico. He says he saw the rotted hulk of the Vulture on the shore of the Main. So the ship was thrown onto the rocks that cursed day. Only I do not believe it.";
			link.l1 = "Why not? Stouter ships than hers have been driven ashore by storms.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "I know. But Bernaldinio handled a vessel through the most ruinous gale. He would never have let himself be thrown onto a beach.";
			link.l1 = "And where exactly did that trader see the Vulture?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "On the coast of the Main, by the Cape of Unfulfilled Hopes. A lonely spot: there is no approach from the sea, only overland, through the jungle beyond Des Moines. And what is it to you? Are you set on finding his ship?";
			link.l1 = "I am. Bernaldinio had a certain thing I need, and the hulk of the Vulture is my only lead.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Then a fair wind to you, cap.";
			link.l1 = "And a fair wind to you, Fernando.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftFindBernaldinioShip");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
