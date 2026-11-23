// Gregg: человек конкурентов Компании - наземный вариант поручения ГВИК «удар по конкурентам».
// В море не выходит, сидит в таверне колонии. Устранение целиком на штатном дуэльном фреймворке:
// свои узлы доводят до оскорбления, дальше link.go = "land_duel" - и Common_Duel.c/duel.c сами разводят
// «за городские ворота» (чистая дуэль 1x1) и «здесь и сейчас» (клон таверны, с телохранителями).
#include "DIALOGS\russian\Common_Duel.c"

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	bool ok;

	ProcessDuelDialog(NPChar, link, NextDiag); // дуэльные узлы (outraged/land_duel/fight_*/talk_off_town/change_mind) обслуживает фреймворк

	switch(Dialog.CurrentNode)
	{
		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "First time":
			NextDiag.TempNode = "First time";
			ok = false;
			if (CheckAttribute(pchar, "GWIK.Job") && CheckAttribute(pchar, "GWIK.Job.RivalLand")) ok = true;
			if (ok && pchar.GWIK.Job != "rival") ok = false;
			if (CheckAttribute(pchar, "GWIK.Job.Done")) ok = false; // уже мёртв - сюда не попасть, но пусть будет
			if (!ok)
			{
				dialog.text = "Прошу простить, у меня дела. Всего доброго.";
				link.l1 = "Не буду мешать.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Хороший ром в этой дыре, не находите? Присаживайтесь, если вам есть что сказать. А если нет - не заслоняйте мне свет.";
			link.l1 = "Обознал"+ GetSexPhrase("ся","ась") +". Приятного вечера.";
			link.l1.go = "exit";
			link.l2 = "Вы ведь "+ GetFullName(NPChar) +"? Тот, кто ведёт здесь дела в обход Вест-Индской Компании.";
			link.l2.go = "GWIK_RivalTalk";
		break;

		case "GWIK_RivalTalk":
			dialog.text = "А, вот оно что. Голландцы прислали кого-то поговорить со мной. Ведут дела в обход? Смешно. Это они лезут в мои дела, а не я в их. И, между нами, у них это выходит скверно.";
			link.l1 = "Компания предлагала вам договориться. Вы плюнули в протянутую руку.";
			link.l1.go = "GWIK_RivalTalk2";
			link.l2 = "Я услышал"+ GetSexPhrase("","а") +" достаточно. До встречи.";
			link.l2.go = "exit";
		break;

		case "GWIK_RivalTalk2":
			dialog.text = "Я плюнул в протянутую ЛАПУ, "+ GetSexPhrase("любезный","любезная") +". Разница есть. И передайте своим хозяевам: пока я жив, ни один их груз в этих водах не пойдёт по их цене.";
			link.l1 = "Пока живы. Это условие легко изменить.";
			link.l1.go = "GWIK_RivalProvoke";
			link.l2 = "Я передам. Слово в слово.";
			link.l2.go = "exit";
		break;

		case "GWIK_RivalProvoke":
			// Gregg: телохранители ставятся ЗАРАНЕЕ - фреймворк подсадит их только в бою в таверне (Duel_Prepare_Fight делает это лишь при type=="tavern"),
			// а за городскими воротами дуэль остаётся один на один. Duel_Kill_Enemy сам вычистит атрибут.
			pchar.questTemp.duel.enemyQty = 1 + rand(1);
			dialog.text = "Что?.. Да как ты смеешь, голландская подстилка! Здесь тебя никто не защитит - ни твоя Компания, ни твой бог!";
			link.l1 = "Тогда выйдем за городские ворота. Там нам никто не помешает.";
			link.l1.go = "GWIK_RivalDuelOut";
			link.l2 = "К чёрту прогулки. Здесь и сейчас.";
			link.l2.go = "GWIK_RivalDuelHere"; // свой узел: то же, что штатный fight_right_now, но сначала снимаем свору
			link.l3 = "Впрочем, живи пока. Мы ещё вернёмся к этому разговору.";
			link.l3.go = "exit";
		break;

		case "GWIK_RivalDuelOut":
			// Gregg: узел «за город» НЕ отдаём фреймворку (land_duel/fight_off_town_prep/talk_off_town): та цепочка ждёт N часов,
			// требует прийти к воротам, даёт «я передумал», трусость и возврат дуэлянта - слишком много состояний, они ломались
			// и позволяли подвесить поручение. Здесь пути назад уже нет: следующая реплика - телепорт за город и бой.
			dialog.text = "Идём. Надеюсь, клинком ты машешь бодрее, чем языком.";
			link.l1 = "Только после вас.";
			link.l1.go = "GWIK_RivalDuelGo";
		break;

		case "GWIK_RivalDuelGo":
			AddDialogExitQuestFunction("GWIK_RivalDuelOutTown"); // телепорт за город + бой по прибытии (образец fight_right_now -> Duel_Prepare_Fight)
			NextDiag.CurrentNode = "First time";
			DialogExit();
		break;

		case "GWIK_RivalDuelHere":
			// Gregg: повторяет штатный fight_right_now (клон таверны через Duel_Prepare_Fight), но сначала снимает свору:
			// дуэль - один на один, а офицеры и абордажники шли за ГГ и в клон тоже. Телохранители ЦЕЛИ (questTemp.duel.enemyQty) остаются
			GWIK_RivalDuelOfficersOff();
			pchar.questTemp.duel.enemy = NPChar.id;
			AddDialogExitQuestFunction("Duel_Prepare_Fight");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "after_peace":
			// Gregg: ПЕРЕКРЫВАЕМ узел фреймворка. Штатный after_peace умеет только «я ухожу» -> peace -> снова after_peace,
			// то есть после «я передумал» цель залипала бы в мирном узле и стала неубиваемой по-дуэльному.
			dialog.text = "Ты ещё здесь? Ступай, пока я добр.";
			link.l1 = "Ухожу.";
			link.l1.go = "exit";
			link.l2 = "Я передумал"+ GetSexPhrase("","а") +". Доставай клинок.";
			link.l2.go = "GWIK_RivalProvoke";
		break;
	}
}
