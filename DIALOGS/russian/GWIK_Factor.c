// Фактор Компании - живое знакомство перед плаванием в Форт Оранж, квест «Контора на Ямайке» (Gregg)

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			if (CheckAttribute(pchar, "GWIK.Jamaica.FactorAboard")) // уже договорились - он «несёт вещи на пристань»
			{
				dialog.text = "Мои сундуки уже несут на пристань, капитан. Увидимся на борту.";
				link.l1 = "До встречи на борту.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Капитан "+ GetFullName(pchar) +"? Наконец-то! "+ GetFullName(NPChar) +", фактор Компании, к вашим услугам - это меня вам предстоит везти в Форт Оранж. Десять лет я вёл склады Виллемстада, а теперь Семнадцать Господ доверили мне целую контору... Признаться, до сих пор не верится.";
			link.l1 = "Расскажите о себе. Кому я доверяю каюту?";
			link.l1.go = "Factor_About";
			link.l2 = "Собирайтесь - отплываем немедленно.";
			link.l2.go = "Factor_Go";
		break;

		case "Factor_About":
			dialog.text = "Все мои сражения - при конторских книгах: приход, расход, коносаменты. Зато уж в них я не знаю поражений. Но не скрою, дорога меня тревожит: у конкурентов длинные руки, и в порту шепчутся об английском корвете у берегов Ямайки. Я не боец, капитан. Вся надежда на вас и ваших канониров.";
			link.l1 = "Пушки у меня не для красоты. Собирайтесь.";
			link.l1.go = "Factor_Go";
			link.l2 = "Подготовлюсь к плаванию и вернусь за вами.";
			link.l2.go = "exit";
		break;

		case "Factor_Go":
			GWIK_JamaicaFactorBoard();
			dialog.text = "По рукам, капитан. Вещи соберут в четверть часа - велю нести их прямо на пристань. Стало быть, в Форт Оранж. Даст бог, дойдём тихо.";
			link.l1 = "Дойдём. До встречи на борту.";
			link.l1.go = "exit";
		break;

		case "exit":
			if (!CheckAttribute(pchar, "GWIK.Jamaica.FactorAboard")) LAi_SetOwnerType(NPChar); // Gregg: отложили отплытие - остаётся стоять и ждать (не замороженный актёр)
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
