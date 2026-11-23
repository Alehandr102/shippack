// Эмиссар Компании в Форт Оранже: квест по открытию конторы ГВИК на Ямайке (Gregg)

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int stage = 0;
	if (CheckAttribute(pchar, "GWIK.Jamaica.Stage")) stage = sti(pchar.GWIK.Jamaica.Stage);
	// Gregg: путь «слово» открыт при высокой репутации; слово коменданта (MayorOk) снижает планку - город замолвился за Компанию
	bool bWord = false;
	if (GWIK_RepLevel() >= 4) bWord = true;
	if (CheckAttribute(pchar, "GWIK.Jamaica.MayorOk") && GWIK_RepLevel() >= 3) bWord = true;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			if (CheckAttribute(pchar, "GWIK.Jamaica.Done")) // квест завершён - указываем на постоянного представителя
			{
				dialog.text = "Контора открыта, капитан, и это во многом ваша заслуга. По делам Компании теперь - к нашему представителю, он в самой конторе.";
				link.l1 = "Доброго дня.";
				link.l1.go = "exit";
				break;
			}
			if (stage == 1) // брифинг → перехват курьера
			{
				dialog.text = "Вы от директоров? Наконец-то, капитан. Я "+ GetFullName(NPChar) +", доверенное лицо Компании в этих краях. Дело щекотливое - о таком на улице не говорят.";
				link.l1 = "Я весь внимание. О чём речь?";
				link.l1.go = "Jam_Brief1";
				link.l2 = "Дайте сперва осмотреться в городе.";
				link.l2.go = "exit";
				break;
			}
			if (stage == 2) // перехват курьера
			{
				if (CheckAttribute(pchar, "GWIK.Jamaica.CourierDone"))
				{
					dialog.text = "Курьер на дне? Превосходно. Донос не дойдёт - мы выиграли время. Но контора сама себя не откроет: теперь о снабжении.";
					link.l1 = "Слушаю.";
					link.l1.go = "Courier_Done";
					break;
				}
				if (CheckAttribute(pchar, "GWIK.Jamaica.CourierEscaped")) // курьер дошёл до порта - донос ушёл, предлагаем откуп
				{
					dialog.text = "Курьер ускользнул в порт, и донос лёг на стол губернатора. Но не всё потеряно: такие бумаги имеют свойство теряться, стоит вложить тяжёлый кошель в нужные руки. Сто тысяч пиастров - и о доносе забудут.";
					if (sti(pchar.money) >= 100000)
					{
						link.l1 = "Вот деньги. Уладьте это.";
						link.l1.go = "Courier_Bribe";
					}
					link.l2 = "Сейчас столько нет.";
					link.l2.go = "exit";
					break;
				}
				dialog.text = "Курьер всё ещё идёт нашими водами, капитан, - лёгкий корвет 'Стриж' капитана Пейджа. Найдите его в море у Ямайки и отправьте на дно - вместе с доносом. И поторопитесь.";
				link.l1 = "Иду на перехват.";
				link.l1.go = "exit";
				break;
			}
			if (stage == 3) // снабжение: эскорт конвоя из Кюрасао
			{
				if (CheckAttribute(pchar, "GWIK.Jamaica.ConvoyArrived")) // конвой доведён - принять груз и перейти к фактору
				{
					if (sti(pchar.GWIK.Jamaica.ConvoyArrived) >= 2)
						dialog.text = "Оба галеона в гавани, трюмы на разгрузке - вы провели конвой без единой потери! Контора теперь при полных запасах. Остался лишь опытный управляющий.";
					else
						dialog.text = "Ваш галеон в гавани... а второй, стало быть, на дне. Что ж, и половина груза лучше, чем ничего: контора при запасах. Остался лишь опытный управляющий.";
					link.l1 = "Что за управляющий?";
					link.l1.go = "Supply_Deliver";
					break;
				}
				if (CheckAttribute(pchar, "GWIK.Jamaica.Convoy")) // эскорт в процессе
				{
					dialog.text = "Конвой ещё в пути? Берегите галеоны, капитан - без товара конторе не встать. И держите оружие наготове: за конвоем гонятся английские корветы.";
					link.l1 = "Доведу.";
					link.l1.go = "exit";
					break;
				}
				dialog.text = "Контора пуста - ей нужны товар и капитал. В Виллемстаде, на Кюрасао, наш управляющий уже грузит два галеона ценной древесиной и шёлком. Ступайте туда, возьмите конвой под охрану и приведите в Форт Оранж. Дорога неспокойна: прознав о грузе, конкуренты пустят вдогонку корветы.";
				link.l1 = "Отправляюсь за конвоем в Кюрасао.";
				link.l1.go = "exit";
				break;
			}
			if (stage == 4) // фактор Компании
			{
				if (CheckAttribute(pchar, "GWIK.Jamaica.FactorAboard"))
				{
					dialog.text = "Наш фактор у вас на борту? Прекрасно. Пусть пока поживёт под вашей охраной - в Форт Оранже ему ещё негде, пока контора не открыта."; // фолбэк для старого сейва без имени
					if (CheckAttribute(pchar, "GWIK.Jamaica.FactorLName")) dialog.text = "Господин "+ pchar.GWIK.Jamaica.FactorLName +" у вас на борту? Прекрасно. Пусть пока поживёт под вашей охраной - в Форт Оранже ему ещё негде, пока контора не открыта.";
					link.l1 = "Понятно. Что дальше?";
					link.l1.go = "Factor_Deliver";
					break;
				}
				dialog.text = "Конторе нужен опытный управляющий. Наш фактор ждёт в конторе Компании на Сан-Мартине, в Мариго, - заберите его и доставьте сюда. И будьте настороже: наши люди видели английский корвет, что третий день крутится у берегов. Конкуренты не дремлют.";
				link.l1 = "Привезу фактора.";
				link.l1.go = "exit";
				break;
			}
			if (stage == 5) // финал: открытие с осложнением + выбор тона
			{
				if (CheckAttribute(pchar, "GWIK.Jamaica.FinalDone"))
				{
					dialog.text = "'Гончая' Гейла на дне - видели бы вы лица англичан! Теперь никто не усомнится, чей флаг над этой конторой. Пора открывать - это ваша победа, капитан.";
					link.l1 = "Открываем!";
					link.l1.go = "Final_OpenForce";
					break;
				}
				if (CheckAttribute(pchar, "GWIK.Jamaica.Tone") && pchar.GWIK.Jamaica.Tone == "force")
				{
					dialog.text = "Капер Джеффри Гейл на 'Гончей' всё ещё ходит у Ямайки. Отправьте его на дно - и возвращайтесь, откроем контору с гордо поднятым флагом.";
					link.l1 = "Иду топить.";
					link.l1.go = "exit";
					break;
				}
				dialog.text = "Фактор на месте, товар на складе - остался последний шаг. Но конкуренты так просто не сдадутся: наверняка попробуют сорвать открытие. Как будем с ними?";
				link.l1 = "Встретим пушками - пусть знают наших.";
				link.l1.go = "Tone_Force";
				link.l2 = "Откупимся, выйдет дешевле.";
				link.l2.go = "Tone_Gold";
				if (bWord)
				{
					link.l3 = "Уладим словом - у меня есть имя и связи.";
					link.l3.go = "Tone_Word";
				}
				link.l4 = "Дайте подумать.";
				link.l4.go = "exit";
				break;
			}
			dialog.text = "Пока распоряжений нет, капитан. Загляните позже.";
			link.l1 = "Зайду.";
			link.l1.go = "exit";
		break;

		case "Jam_Brief1":
			dialog.text = "Открыть здесь, на Ямайке, контору Компании - всё равно что развести костёр под носом у англичан Порт Рояля. И они уже что-то пронюхали: наш человек в таверне видел, как губернатор снарядил курьера с доносом в другой английский порт. Дойдут бумаги - дело прикроют в зародыше.";
			link.l1 = "И что вы предлагаете?";
			link.l1.go = "Jam_Brief2";
			link.l2 = "А чем рискую лично я?";
			link.l2.go = "Jam_BriefRisk";
		break;

		case "Jam_BriefRisk":
			dialog.text = "Официально Компания тут ни при чём, а вы - вольный капитан, мало ли что в море случается. Но такой услуги Семнадцать Господ не забывают: контора, доля соучредителя, доброе имя. Рискуете не больше, чем в любом деле.";
			link.l1 = "Хорошо. Так что с курьером?";
			link.l1.go = "Jam_Brief2";
		break;

		case "Jam_Brief2":
			dialog.text = "Перехватите курьера в море, пока он не дошёл до порта, и отправьте на дно вместе с доносом. Тихо и наверняка.";
			link.l1 = "Где его искать?";
			link.l1.go = "Brief_Courier";
			link.l2 = "Дайте подготовиться.";
			link.l2.go = "exit";
		break;

		case "Brief_Courier":
			if (!GWIK_JamaicaSpawnCourier()) // курьер не вышел в море - не загоняем квест в тупик
			{
				dialog.text = "Странно - наш осведомитель потерял курьера из виду. Загляните чуть позже, я уточню маршрут.";
				link.l1 = "Хорошо, зайду.";
				link.l1.go = "exit";
				break;
			}
			pchar.GWIK.Jamaica.Stage = 2;
			AddQuestRecord("GWIK_JamaicaJob", "2");
			dialog.text = "Курьер - лёгкий корвет 'Стриж' капитана Джереми Пейджа, уже вышел от берегов Ямайки в английский порт. Не дайте ему дойти. И помните: официально Компания тут ни при чём.";
			link.l1 = "Сделаю.";
			link.l1.go = "exit";
		break;

		case "Courier_Done":
			pchar.GWIK.Jamaica.Stage = 3;
			DeleteAttribute(pchar, "GWIK.Jamaica.CourierDone");
			AddQuestRecord("GWIK_JamaicaJob", "4");
			dialog.text = "В Виллемстаде, на Кюрасао, наш управляющий уже грузит два галеона ценной древесиной и шёлком. Ступайте туда, возьмите конвой под охрану и приведите в Форт Оранж. Дорога неспокойна: прознав о грузе, конкуренты пустят вдогонку корветы.";
			link.l1 = "Отправляюсь за конвоем в Кюрасао.";
			link.l1.go = "exit";
		break;

		case "Courier_Bribe":
			if (sti(pchar.money) < 100000)
			{
				dialog.text = "На откуп нужно сто тысяч пиастров, а столько у вас при себе нет, капитан.";
				link.l1 = "Найду деньги.";
				link.l1.go = "exit";
				break;
			}
			AddMoneyToCharacter(pchar, -100000);
			DeleteAttribute(pchar, "GWIK.Jamaica.CourierEscaped");
			pchar.GWIK.Jamaica.Stage = 3;
			PlayStereoSound("notebook");
			AddQuestRecord("GWIK_JamaicaJob", "12");
			dialog.text = "Готово. Донос утонул в чужих бумагах, как и не бывало. Теперь о снабжении: в Виллемстаде, на Кюрасао, уже грузят два галеона древесиной и шёлком. Ступайте туда, возьмите конвой под охрану и приведите в Форт Оранж - конкуренты наверняка пустят вдогонку корветы.";
			link.l1 = "Отправляюсь за конвоем в Кюрасао.";
			link.l1.go = "exit";
		break;

		case "Supply_Deliver": // Gregg: конвой доведён - снабжение закрыто, переход к этапу фактора
			if (CheckAttribute(pchar, "GWIK.Jamaica.ConvoyArrived") && sti(pchar.GWIK.Jamaica.ConvoyArrived) >= 2) GWIK_AddRep(10); // Gregg: бонус за конвой без потерь
				DeleteAttribute(pchar, "GWIK.Jamaica.ConvoyArrived");
			pchar.GWIK.Jamaica.Stage = 4;
			PlayStereoSound("notebook");
			AddCharacterExpToSkill(pchar, SKILL_SAILING, 60);    // Gregg: опыт навигации за проводку конвоя снабжения под преследованием
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 30); // Gregg: и командования - эскорт эскадры до Форт Оранжа
			AddQuestRecord("GWIK_JamaicaJob", "5");
			dialog.text = "Древесина и шёлк с ваших галеонов пойдут в дело - контора при запасах. Теперь ей нужен управляющий: наш фактор ждёт в конторе на Сан-Мартине, в Мариго. Привезите его сюда.";
			link.l1 = "Привезу.";
			link.l1.go = "exit";
		break;

		case "Factor_Deliver":
			pchar.GWIK.Jamaica.Stage = 5;
			GWIK_JamaicaClearAmbush(); // если от перехватчика оторвались - он своё дело провалил, убираем
			AddCharacterExpToSkill(pchar, SKILL_SAILING, 50);    // Gregg: опыт навигации за доставку фактора мимо засады
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 20); // Gregg: и немного командования
			AddQuestRecord("GWIK_JamaicaJob", "7");
			dialog.text = "Фактор уже осматривает будущие склады - отлично. Остался последний шаг: открытие. Но конкуренты так просто не сдадутся - наверняка попробуют сорвать дело. Как будем с ними?"; // фолбэк для старого сейва без имени
			if (CheckAttribute(pchar, "GWIK.Jamaica.FactorLName")) dialog.text = "Господин "+ pchar.GWIK.Jamaica.FactorLName +" уже осматривает будущие склады - отлично. Остался последний шаг: открытие. Но конкуренты так просто не сдадутся - наверняка попробуют сорвать дело. Как будем с ними?";
			link.l1 = "Встретим пушками - пусть знают наших.";
			link.l1.go = "Tone_Force";
			link.l2 = "Откупимся, выйдет дешевле.";
			link.l2.go = "Tone_Gold";
			if (bWord)
			{
				link.l3 = "Уладим словом - у меня есть имя и связи.";
				link.l3.go = "Tone_Word";
			}
			link.l4 = "Дайте подумать.";
			link.l4.go = "exit";
		break;

		case "Tone_Force":
			if (!GWIK_JamaicaSpawnFinal()) // приватир не заспавнился - не залипаем на пути силы, выбор тона остаётся
			{
				dialog.text = "Странно, приватир не показывается у берегов. Загляните чуть позже - или решим дело иначе.";
				link.l1 = "Хорошо.";
				link.l1.go = "exit";
				break;
			}
			pchar.GWIK.Jamaica.Tone = "force";
			AddQuestRecord("GWIK_JamaicaJob", "8");
			dialog.text = "По-нашему! Англичане наняли капера - некоего Джеффри Гейла на корвете 'Гончая', он уже вышел к Ямайке. Встретьте его в море и отправьте на дно. Потом возвращайтесь - и откроем контору открыто, при всех.";
			link.l1 = "Будет сделано.";
			link.l1.go = "exit";
		break;

		case "Tone_Gold":
			if (sti(pchar.money) < 80000)
			{
				dialog.text = "На откуп нужно восемьдесят тысяч пиастров, а столько у вас при себе нет, капитан.";
				link.l1 = "Найду деньги.";
				link.l1.go = "exit";
				break;
			}
			AddMoneyToCharacter(pchar, -80000);
			pchar.GWIK.Jamaica.Tone = "gold";
			GWIK_FinishJamaicaQuest(0);
			if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) // открытие сорвалось - деньги назад, выбор не сжигаем
			{
				AddMoneyToCharacter(pchar, 80000);
				DeleteAttribute(pchar, "GWIK.Jamaica.Tone");
				dialog.text = "Заминка с бумагами, капитан... Деньги пока при вас. Загляните чуть позже - уладим.";
				link.l1 = "Хорошо.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Мудро. Несколько тяжёлых кошельков в нужных руках - и англичане отвернутся в нужный момент. Дело сделано: контора открыта! Компания не забудет вашего вклада.";
			link.l1 = "Рад"+ GetSexPhrase("","а") +" за Компанию.";
			link.l1.go = "exit";
		break;

		case "Tone_Word":
			pchar.GWIK.Jamaica.Tone = "word";
			GWIK_FinishJamaicaQuest(0);
			if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) // открытие сорвалось - выбор не сжигаем
			{
				DeleteAttribute(pchar, "GWIK.Jamaica.Tone");
				dialog.text = "Заминка с бумагами, капитан... Загляните чуть позже - уладим.";
				link.l1 = "Хорошо.";
				link.l1.go = "exit";
				break;
			}
			GWIK_AddRep(20); // бонус за самый трудный путь - только при успехе
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 100); // Gregg: крупный опыт лидерства за чисто дипломатический финал (самый трудный путь)
			dialog.text = "Ваше имя и впрямь открывает двери. Пара слов нужным людям - и буря улеглась, не начавшись. Контора открыта, и без единого выстрела. Компания высоко это ценит.";
			link.l1 = "Так-то лучше.";
			link.l1.go = "exit";
		break;

		case "Final_OpenForce":
			GWIK_FinishJamaicaQuest(15);
			if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) // открытие сорвалось - не теряем финал
			{
				dialog.text = "Заминка с бумагами, капитан... Загляните чуть позже - уладим.";
				link.l1 = "Хорошо.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Дело сделано - контора Вест-Индской Компании в Форт Оранже открыта, и под нашим флагом! Это ваша победа, капитан. Компания этого не забудет.";
			link.l1 = "За Компанию!";
			link.l1.go = "exit";
		break;

		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
