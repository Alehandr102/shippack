// Gregg: Харви Муром, бывший из эскадры Бартелеми - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
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
			dialog.text = "Не мешай, я занят.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Кого сюда занесло? Чего тебе понадобилось от старого Харви?";
			link.l1 = "Говорят, ты ходил в эскадре Мориса Бартелеми.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(отставляет кружку)... Ходил. Четыре года ходил, и все четыре года мы искали одного человека.";
			link.l1 = "Брюса Фишера.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Его. Весь Мэйн перерыли. ...(усмехается)... А он всё это время ходил у нас под носом и в порты не заходил вовсе.";
			link.l1 = "Как это - не заходил?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "А вот так. «Искатель» стоит на воде круглый год. Провиант ему возят шлюпками с берега, за деньги. Ни таможни, ни портовых, ни бумаг. Отец его так же жил последние годы.";
			link.l1 = "И где он стоит?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Меняет места. Две стоянки, между ними ходит и больше никуда. Я тебе их назову, только с одним условием.";
			link.l1 = "Каким?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Возьмёшь его живым - спроси, за что он убил Мориса. ...(смотрит в стол)... Морис мне четыре года жалованье платил. А теперь его нет.";
			link.l1 = "Спрошу.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyGetKharviInformation");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
