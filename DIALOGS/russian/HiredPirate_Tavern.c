// LEO: Наёмный пират-капер в таверне. Предлагает за плату захватить корабль.
// Логика - scripts/HiredPirate.c. Жёлтый "?" над головой (isHiredCaper -> LAi_citizen.c).
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int fee = 40000;
	if (CheckAttribute(NPChar, "hpFee")) fee = sti(NPChar.hpFee);
	int deposit = fee / 2;

	switch (Dialog.CurrentNode)
	{
		case "exit":
			DialogExit();
		break;

		case "First time":
			if (HiredPirate_IsActive()) // уже нанят - не предлагаем снова (страховка от повторного найма)
			{
				dialog.text = "Мы уже сговорились, капитан. Я отправляюсь отрабатывать монету. Спрашивай обо мне у барменов.";
				link.l1 = "Идёт. Удачной охоты.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "(наклоняется, вполголоса) По тебе видно капитана, которому не помешает ещё один корпус под флагом, и который не слишком разборчив, откуда тот возьмётся. Я - вольный капер, работаю за плату. Укажи мне воды, и я возьму для тебя приз - корабль будет твой. Цена по его размеру: " + FindRussianMoneyString(fee) + " всего, половина сейчас, половина - когда заберёшь его в бухте.";
			link.l1 = "Как проходит передача?";
			link.l1.go = "explain";
			link.l2 = "Не интересует.";
			link.l2.go = "decline";
		break;

		case "explain":
			dialog.text = "Просто. Половину платишь сейчас. Я ухожу на охоту - дней десять-двадцать. Возьму приз - отстоюсь в тихой бухте и буду ждать. Бармены - народ языкатый и в доле: спрашивай их про меня, и как буду готов, кто-нибудь шепнёт тебе, в какой я бухте. Приходи туда с остатком монеты и с запасным офицером на борту - кораблю нужен капитан, а я не останусь. Проканителишься больше недели после того, как бармен шепнёт - продам приз в другом месте. По рукам?";
			if (sti(pchar.money) >= deposit)
			{
				link.l1 = "По рукам. Вот твоя половина. (" + FindRussianMoneyString(deposit) + ")";
				link.l1.go = "hire";
			}
			else
			{
				link.l1 = "У меня нет при себе " + FindRussianMoneyString(deposit) + ".";
				link.l1.go = "nomoney";
			}
			link.l2 = "Надо подумать.";
			link.l2.go = "decline";
		break;

		case "hire":
			HiredPirate_Hire(NPChar);
			DeleteAttribute(NPChar, "isHiredCaper"); // снять флаг метки (иначе тик LAi_sit перерисует "?")
			UnmarkCharacter(NPChar);                 // и саму метку - прямо здесь, у собеседника
			dialog.text = "Половина у меня - остальное при передаче. Тогда я пошёл. Помни: спрашивай барменов и приведи в бухту запасного офицера. Попутного ветра, капитан.";
			link.l1 = "Удачной охоты.";
			link.l1.go = "exit";
		break;

		case "nomoney":
			dialog.text = "Возвращайся, когда кошелёк потяжелеет, капитан. На обещаниях я не хожу.";
			link.l1 = "Тогда позже.";
			link.l1.go = "exit";
		break;

		case "decline":
			dialog.text = "Как знаешь. Передумаешь - таверны ты знаешь, хотя я подолгу не засиживаюсь.";
			link.l1 = "Прощай.";
			link.l1.go = "exit";
		break;
	}
}
