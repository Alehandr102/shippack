// Gregg: Алехон Кендарт - трилогия «Чёрная Орхидея», финал главы 5 (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c
// Грот Теркса. Последний обладатель Орхидеи ждал того, кто соберёт Меритор:
//   1-3   - передача Орхидеи;
//   4-6   - чем она была и что от неё осталось;
//   6a-6b - посев: кого-то взяли вместо него, кто Орхидею и не искал (см. ниже);
//   7-9   - Меритор ушёл к новым соискателям, уничтожить Орхидею Кендарт не умеет;
//   10    - прощание, выход в бой (DestinyGiftFinalBattle).
// Узел 1 выставляет логика. Кендарт остаётся в мире и переходит в главы 6-9,
// поэтому «First time» - заглушка, её займёт диспетчер следующих глав.
//
// Gregg: ТРИ отступления от оригинала, оба принципиальные.
// 1. В К:ПС остаток силы Орхидеи - два месяца полной неуязвимости, после чего она
//    становится простым камнем. У нас Орхидея - оберег (снижение урона плюс одно
//    спасение от смертельного удара, см. BlackOrchid_IsWorn в blackorchid.c), и это
//    объясняется его же работой: он стёр её почти до конца, остаток скромен, но вечен.
// 2. В оригинале ГГ обещал через два месяца уничтожить Орхидею или спрятать её.
//    Обещания нет: глава 6 у нас идёт иначе, настоящая Орхидея остаётся у игрока,
//    и давать зарок, который мы не исполним, незачем. Вопрос оставлен открытым.
// 3. Узлы 6a-6b - НАШЕ добавление, в оригинале их нет. Оригинальный Кендарт молчит
//    о своём мотиве до самой главы 6, где выясняется, что испанцы, разыскивая его,
//    взяли и запытали до смерти его брата (Alehon Kendart.c, узел SpanishGenerosity_9).
//    Из-за этого его разворот в шестой главе - «продадим Орхидею Испании» - выглядит
//    подменой характера. Здесь мы кладём зацепку: кого-то взяли вместо него, и этот
//    кто-то Орхидею не искал. Ни нации, ни родства, ни имени - иначе разворот
//    в главе 6 перестанет быть разворотом. Заодно объясняется, почему он живёт
//    в гроте. Реплика узла 7 про «чтобы Орхидея досталась вам, а не Испании» тоже
//    наша: в главе 6 игрок процитирует её Кендарту в лицо.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sName = PChar.name;
	string sFullName = GetFullName(PChar);


	switch (Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Good luck to you, " + sName + ".";
			link.l1 = "And to you, Alejon.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(a quiet, soft voice)... " + sFullName + ". I knew this would come. I knew it would be you who found me.";
			link.l1 = "...(with command)... The Black Orchid. Give it to me. Now.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I shall, " + sName + ". But first I will ask. Are you ready to be its " + GetSexPhrase("master", "mistress") + "? Ready for the others to come after you - and not to stop?";
			link.l1 = "I am ready. Hand it over.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "From this moment you are the " + GetSexPhrase("master", "mistress") + " of the Black Orchid.";
			link.l1 = "...(turning the Orchid over)... Is that all? An ordinary stone in a setting.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Now, yes. Every year the Orchid was mine I spent wearing its power down to nothing. And I very nearly succeeded, which is the thing I am proudest of in my life. What you hold is the shadow of the Orchid that was.";
			link.l1 = "A shadow? Then is it good for anything at all?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "It is. Wear it, and the blade that should have cut you in two will leave a scratch. And once, when the blow does prove mortal, it will not let you fall. Once. Then it needs time to gather itself again. That is all that is left of it, and it is not little.";
			link.l1 = "Not little? I was told the Orchid makes a man invulnerable.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "It did. And that is why it killed. The Black Orchid brings death even in the hands of one who does not reach for power - and I did not reach. How many died while you were searching for it? How many more will die? It makes murderers of good men, the moment they grasp what they are capable of while holding it. I took that from it. I left exactly enough for its master to live, and not one drop more.";
			link.l1 = "You speak as though you had paid for it yourself.";
			link.l1.go = "6a";
		break;

		case "6a":
			dialog.text = "I did. And not only those who searched for it paid. Once they searched for it through me - they did not find me, so they took a man who had never heard of the Orchid. He had nothing to tell them. They asked him for a long time.";
			link.l1 = "Who was he?";
			link.l1.go = "6b";
		break;

		case "6b":
			dialog.text = "...(a long pause)... Forgive me. I do not speak of it. I will say only that I live in a grotto on a bare island for reasons other than a fondness for damp.";
			link.l1 = "...(quietly)... So all of this was for a charm on a string.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "All of this was so that the Orchid came to you and not to Spain. As for the Meritor, it has served its turn: the moment the Orchid lay in your hand, it went from your pocket. It has broken into seven again and scattered across the Caribbean - to wait for the next one.";
			link.l1 = "The next one? So they will come for me.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "They will. And they will keep coming while the Orchid exists.";
			link.l1 = "Then help me be rid of it, Alejon. You lived with it for years - you must know how.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "I do not know, " + GetHeroLastName() + ". And I would not advise guessing at it - those who guessed died long before the Orchid began to fade. I have done what I could; the rest is yours to decide. And now I shall leave you.";
			link.l1 = "Leave? And how do you mean to get out of here?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "I have learned to go without the Orchid too, " + GetHeroLastName() + ". Good luck. Farewell.";
			link.l1 = "No, wait!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftFinalBattle");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
