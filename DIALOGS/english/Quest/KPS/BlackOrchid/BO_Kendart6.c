// Gregg: Алехон Кендарт - «Чёрная Орхидея», глава 6 «Испанская щедрость» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c
// Оригинал реплик - DamnedDestiny\Alehon Kendart.c, ветка SpanishGenerosity_1..55.
//   1-16  - комната наверху в таверне Ле-Франсуа: план, брат, отдача Орхидеи;
//   20-21 - копия готова, подделка на руки;
//   25    - после сделки: спрятать подделку в гроте Доминики;
//   30-34 - ратуша Сан-Хуана: колония наша, денег нет, эскадра на подходе;
//   40-42 - после победы, вход в главы 7-9.
//
// Gregg: ОТСТУПЛЕНИЯ ОТ ОРИГИНАЛА.
// 1. Испанцам уходит ПОДДЕЛКА. В К:ПС Кендарт срезал настоящей Орхидее срок действия
//    до трёх суток и продавал её - у нас Орхидея вечный оберег, срезать нечего.
// 2. Мотив выдан не одной репликой в узле 9, как в оригинале, а собран: зацепку про
//    взятого вместо него человека Кендарт роняет ещё в гроте Теркса (BO_Kendart.c, 6a-6b),
//    здесь она досказывается. И игрок предъявляет ему его же слова из узла 7 главы 5.
// 3. «Все испанцы такие» из оригинала (узел 10) переписано: учёный, который делает
//    выводы по опросу на улице, - слабое место оригинала. Отвечает жёстче и точнее.
// 4. Про уничтожение Орхидеи Кендарт больше не заговаривает: мотив исчерпан.
// 5. Балаган про гарем (узлы 15-18 оригинала) выброшен - он ломает сцену, в которой
//    человек только что рассказал, как пытали его брата.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sName = PChar.name;


	switch (Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Go on, " + sName + ". Each of us has work to do.";
			link.l1 = "Keep well, Alejon.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Sit down, " + sName + ". I hear Spain has been offering you money for the Orchid. Is that true?";
			link.l1 = "It is. Five millions, then ten. I refused. How do you know?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I listen to the Caribbean more closely than it appears. So: I want you to agree.";
			link.l1 = "...(slowly)... Agree to sell the Orchid. To Spain.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Just so.";
			link.l1 = "Alejon, in the grotto on Terks you told me word for word: «All of this was so that the Orchid came to you and not to Spain.» Your words. I remember them well.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "...(without a shade of embarrassment)... Mine. And I do not take them back. Spain will not get the Black Orchid.";
			link.l1 = "Then what will it get for its millions?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "A copy. The same black stone, the same setting, the same weight - the eye will not tell them apart. It will hold no warmth; warmth is past my craft. I spent twenty years making the Orchid powerless - I can make an empty thing in six weeks.";
			link.l1 = "And they will take a stone off the street for the greatest artefact in the Caribbean?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "They will. You have held her and know she warms the hand - in Madrid they know her from papers and hearsay. And recall what you have been told about the Orchid all these months: the power does not come to its owner at once. That is true, and it is what makes testing impossible. Alboretto believes it himself - he is no swindler, he is a messenger.";
			link.l1 = "Very well. Then another question. You have no need of money, Alejon; you lived twenty years in a hole with a stone in your pocket. Why do you want this?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "...(a long silence)... Do you remember what I told you in the grotto? That they took a man who had never heard of the Orchid.";
			link.l1 = "I remember. You did not finish.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "It was my brother. They were not hunting him for the Orchid - they were hunting him to learn where I was. He did not know. He had nothing to tell them. They tortured him for four days.";
			link.l1 = "Who did?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Men with a warrant from the Spanish crown. That is when I went to the grotto, and understood that the Orchid must not be hidden but unmade. For twenty years I thought that would be enough.";
			link.l1 = "Not all Spaniards are like that, Alejon.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Not all. Nor do I mean to drown fishermen in Santiago. But it was not fishermen who gave the order, and it is not a fisherman who will pay. The crown will pay - in money and in a colony.";
			link.l1 = "A colony?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Ask San Juan for the Orchid. And twenty-five millions on top. Spain will pay: they are certain they will take the town back within the month, holding the Orchid.";
			link.l1 = "While what they hold is a pebble worth three pesos.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Just so. They will have to take the town back with what they have, not with what they bought.";
			link.l1 = "Why San Juan? There are richer colonies in the Caribbean.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Because it stands apart, it has a good fort and a deep harbour. And because from there I can go on working without looking over my shoulder. I need a corner, " + sName + ", and you need a base.";
			link.l1 = "And it is I who will have to fight off the Spanish fleet.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "You will. So by the day you must have a ship that will stand against a manowar. I will give you as much time as you need. Better late than in a sloop.";
			link.l1 = "And a garrison? We will not hold an empty town a week.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "There will be a garrison, a governor and two ships in the harbour. That is my part of the work, and it is half done already.";
			link.l1 = "Good. What do you need from me now?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "The Orchid. For six weeks. A copy cannot be taken without the original before one's eyes.";
			link.l1 = "...(taking off the Orchid)... Six weeks without it. Mark me, Alejon - if you vanish with it, I will find you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityFirstTimer");
		break;

		case "20":
			dialog.text = "It is done, " + sName + ". Here they both are. Can you tell which is the true one?";
			link.l1 = "...(turning both stones over)... No.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "Nor would I, had I not marked it. This one, with the scratch at the stem, is yours. Put it on and do not take it off. Take the other to Alboretto in Santiago and name the price: San Juan and twenty-five millions. Not a peso less.";
			link.l1 = "And if he refuses?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "He will not. He has raised the price twice already, unprompted. He is ordered to bring back the Orchid at any cost, and he knows it.";
			link.l1 = "Then I am away.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityGetFake");
		break;

		case "25":
			dialog.text = "I hear they counted out your deposit. So the thing is moving.";
			link.l1 = "A million. The rest they promise by transfer. What now?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "Now, a cache. They have given up the town, but they get no stone from hand to hand: Alboretto learns the place no sooner than his garrison steps ashore in Havana. Take the forgery to the grotto on Dominica and leave it in the chest.";
			link.l1 = "Why such pains, if San Juan is already ours?";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "Because the forgery will be uncovered sooner or later, and by that hour I want a sea lying between us and their soldiers. Go, " + sName + ". And come back to Le Francois.";
			link.l1 = "Very well, Alejon.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityToGrot");
		break;

		case "30":
			dialog.text = "San Juan is ours, " + sName + ". Wholly and undivided. I did not think I would live to see it.";
			link.l1 = "And the money? Did the twenty-five millions come?";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "...(grimacing)... The million of the deposit is all we shall see. The transfer was recalled three days ago. Someone in Seville thought to show the stone to a man who had held the Orchid in his hands.";
			link.l1 = "So the copy is uncovered. Quickly.";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "Quicker than I reckoned. I admit it - my error, and not the last, I expect. But the colony will not be taken back from us with a stroke of the pen. It will have to be taken with a fleet.";
			link.l1 = "And the fleet, I take it, has already sailed.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "Admiral Merkes' squadron. Four pennants: the manowar Altea, the line ships Aheldio and Escolta, and Alboretto's Andromeda - he was sent with them so he might look upon what he has done. They will be here any day.";
			link.l1 = "What do I have?";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "The manowar in the harbour is Cristobal Kuvalda's. He says you are acquainted: that you fought side by side in the pearl divers' settlement against Feregueyro.";
			link.l1 = "We did. He never asked what he was getting into then, either. Where did he come by a manowar?";
			link.l1.go = "34a";
		break;

		case "34a":
			dialog.text = "He took her. From whom I did not ask, and he did not say. We owe Cristobal more than a ship: his word carries among the coastal brotherhood, and it was he who proposed San Juan to the corsairs as a new base. Half the garrison in the town are his men.";
			link.l1 = "So the town is held by pirates he gathered. And if tomorrow they dislike who the master here is?";
			link.l1.go = "34b";
		break;

		case "34b":
			dialog.text = "If they dislike it, you will deal with it. But I put about a rumour that you wore the Black Orchid and inherited something of it. In the harbour they believe it. The second captain is Roger Belami, the line ship Reginald, Kuvalda's man. Both are under your hand for the battle.";
			link.l1 = "That will do. Anything else I ought to know?";
			link.l1.go = "34c";
		break;

		case "34c":
			dialog.text = "One thing. The squadron treasury rides in the hold of the Altea. Take the flagship by boarding and we recover at least part of what we were not paid.";
			link.l1 = "And if I do not take her?";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "Then it goes to the bottom with her. Do not risk a ship for money, " + sName + ". The town matters more.";
			link.l1 = "Understood. I shall make ready.";
			AddDialogExitQuestFunction("SpanishGenerosityToFinal");
			link.l1.go = "exit";
		break;

		case "40":
			dialog.text = "Admiral Merkes' squadron is no more, " + sName + ". You have done it.";
			link.l1 = "We have done it. What now of the Orchid, Alejon?";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "Nothing. It is yours, and there let it stay. I spent twenty years taking its power from it so as to give it to one who would not trade in it. Today I have found him.";
			link.l1 = "You did regret that you could not destroy it.";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "I did. And now I think there was no need: what is left of it will make a murderer of no one. Rest, " + sName + ". In a week look into the town hall - I shall have a conversation for you rather more interesting than this one.";
			link.l1 = "I shall, Alejon.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityHappyEnd");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
