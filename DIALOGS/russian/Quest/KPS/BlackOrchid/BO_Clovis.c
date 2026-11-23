// Gregg: Клови Дюбуа - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
// Логика - Program\Quests\reaction_functions\blackorchid.c.
// Клови нанят Кендартом за 300 000 как соглядатай: две недели он смотрел за дочерью
// губернатора. Драки в городе не входили в его план и ему самому не нравятся.

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
			// Gregg: строго по состоянию. Без гейта любой повторный подход проигрывал встречу
			// на пляже заново и снова заводил выход в город.
			if (PChar.FindAndDestroy == "11")
			{
				dialog.text = "Клови Дюбуа. Кендарт заплатил, я отработал.";
				link.l1 = "Две недели за триста тысяч. Что купили?";
				link.l1.go = "1a";
			}
			else
			{
			// Gregg: подсказка на время поисков тряпья. Игрок легко уносит из сундука один
			// комплект вместо двух, и тогда квестовое условие молчит.
			if (CheckAttribute(PChar, "FindAndDestroy.NeedSuits"))
			{
				dialog.text = "Оба комплекта берите, капитан. Один мой, я в офицерском в ратушу не пойду.";
				link.l1 = "Беру оба.";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "Я рядом. Скажете - пойдём.";
				link.l1 = "Пока стоим.";
				link.l1.go = "exit";
			}
			}
			NextDiag.TempNode = "First time";
		break;

		case "1a":
			dialog.text = "Две недели я сидел в снятом доме у площади и смотрел в подзорную трубу. Девушку видел двадцать три раза, и все двадцать три - в одном окне. Из ратуши она не выходит.";
			link.l1 = "Ключ на ней?";
			link.l1.go = "1b";
		break;

		case "1b":
			dialog.text = "На шее, на шнурке. Носит не снимая, а что это, судя по всему, не знает. Отца её я за две недели не видел ни разу, а губернатор Тортуги, капитан, человек публичный.";
			link.l1 = "Куда он делся?";
			link.l1.go = "1c";
		break;

		case "1c":
			dialog.text = "Никуда. Он в ратуше, выходит на балкон, машет рукой. Только это не он. ...(помолчав)... Я его двенадцать лет знаю, а тут смотрю и не узнаю. Походка чужая.";
			link.l1 = "И ты две недели молчал?";
			link.l1.go = "1d";
		break;

		case "1d":
			dialog.text = "Я писал Кендарту дважды в неделю. Он велел ждать вас и не соваться в город одному. ...(поднимает мешок)... Идём, по дороге доложу остальное.";
			link.l1 = "Докладывай на ходу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTortugaExitTown");
		break;

		case "9":
			dialog.text = "...(остановился, шёпотом)... Стоп. Двенадцать у ворот.";
			link.l1 = "Вчера сколько было?";
			link.l1.go = "9a";
		break;

		case "9a":
			dialog.text = "Четверо. Я две недели считал каждый день, и было четверо. ...(медленно)... И они уже смотрят сюда, капитан. Гарнизон так не смотрит.";
			link.l1 = "Значит, разговора не будет.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyInTortugaExitTownBattle");
		break;

		case "12":
			dialog.text = "Двенадцать. И ни один не крикнул, не позвал подмогу, не потребовал бросить оружие. Дрались молча.";
			link.l1 = "Ты за две недели ни разу этого не заметил?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Заметил другое. Люди в городе перестали здороваться друг с другом. Я списал на страх. ...(вытирает клинок)... Списал зря. Идём в город, только держись рядом и ни с кем не заговаривай.";
			link.l1 = "Веди.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyInTortugaTown");
		break;

		case "16":
			dialog.text = "Семеро. Капитан, я знаю тут каждого второго в лицо. Ни одного знакомого.";
			link.l1 = "Куда теперь?";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "В дом на углу, я снимал его две недели, оттуда и смотрел. Там же лежит то, без чего в ратушу не войти.";
			link.l1 = "Идём.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTortugaShelter");
		break;

		case "18":
			dialog.text = "Сундук в углу. Открывай, там два комплекта.";
			link.l1 = "Что за комплекты?";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Тряпьё. В ратушу пускают офицеров и нищих. Первыми нам не стать, а нищих там не считают за людей и потому не разглядывают.";
			link.l1 = "...(идёт к сундуку)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyFindClothes");
		break;

		case "20":
			dialog.text = "Взяли? Переодевайся и спрячь клинок. Мушкет оставь: с мушкетом нищих не бывает.";
			link.l1 = "Переодеваюсь.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyDialogInShelter");
		break;

		case "21":
			dialog.text = "Ушёл. ...(выдыхает)... Капитан, он смотрел на вас дольше, чем на меня, и не на лицо, а на руки. Мне это не нравится.";
			link.l1 = "В ратушу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTownhall");
		break;

		case "28":
			dialog.text = "Готово. ...(смотрит на тело)... И вот что, капитан. У девушки ключ на шее, я его две недели видел в подзорную трубу. У этого на шее был такой же.";
			link.l1 = "Проверим у того, кто сидит наверху. Иди к двери и жди.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyTalkWithMayor");
		break;

		case "29":
			dialog.text = "Девочка ни при чём, капитан. Я две недели за ней смотрел: её тут держат, а не она держит.";
			link.l1 = "Знаю. Выводим.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyRoomOfficers");
		break;

		case "30":
			dialog.text = "Выводим. Только не к воротам: там уже знают. Есть ход из подвала ратуши в пещеры, я его нашёл на четвёртый день и, честно говоря, не думал, что пригодится.";
			link.l1 = "Веди через подвал.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToDungeon");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
