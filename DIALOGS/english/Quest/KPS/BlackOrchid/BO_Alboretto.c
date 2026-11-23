// Gregg: Эмилио Альборетто - «Чёрная Орхидея», глава 6 «Испанская щедрость» (перенос из К:ПС).
// Логика - Program\Quests\reaction_functions\blackorchid.c
// Оригинал - DamnedDestiny\SpanishGenerosity\Emilio Alboretto.c.
//   1-6   - таверна Сантьяго, первый торг, ГГ отказывается продавать;
//   20-25 - второй заход: цена названа - Сан-Хуан и двадцать пять миллионов;
//   30-34 - палуба «Андромеды» после отбитой засады;
//   40-44 - ратуша Сантьяго: колония очищена, задаток на руки.
//
// Gregg: ОТСТУПЛЕНИЯ. Лестница торга оригинала (5 -> 10 -> ГГ просит 200 -> 25) сжата:
// двести миллионов и «да вы охренели что ли» из оригинала выброшены, торг идёт короче
// и суше. Двадцать пять миллионов остаются в реплике, но платят из них только задаток -
// см. BO_SG_DEPOSIT в логике. Модель - PGG_Espinosa_0 (решение пользователя).

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sLastName = GetHeroLastName();


	switch (Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "All that we agreed upon stands, captain. I have nothing to add.";
			link.l1 = "Nor I.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(rises to meet you)... Captain " + sLastName + ". Emilio Alboretto, at your service. Sit, I beg you. My business is short, but not of the kind one conducts standing.";
			link.l1 = "On whose commission are you here, senor Alboretto?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "The Spanish crown. Let us not play hide and seek, captain: we know the Black Orchid is yours. The crown wishes to buy it.";
			link.l1 = "And if I say I have no notion what you mean?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Then I shall beg your pardon and go. But you will not say it, or you would not have sat down at this table.";
			link.l1 = "...(with a smirk)... Suppose so. How much?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Five million piastres. In gold, wherever you please.";
			link.l1 = "Five million. For the thing half the Caribbean has hunted for half a year.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "...(without a pause)... Ten. And that is not my last word, captain, it is my second.";
			link.l1 = "Senor Alboretto, I see you are ordered to bring back the Orchid at any price. Which makes this conversation pointless.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Name the price yourself. Any price.";
			link.l1 = "Not today. The Orchid is not for sale. Good day, senor.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityExitFromTavern");
		break;

		case "20":
			dialog.text = "...(rises slowly)... Captain. I confess I did not expect to see you.";
			link.l1 = "I have changed my mind, senor Alboretto. The Orchid is for sale.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "...(sits down)... I am listening most attentively.";
			link.l1 = "Twenty-five million piastres. And the colony of San Juan, into my own possession.";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "...(a long pause)... The money is a solvable question. But a colony, captain? You are asking the crown for a piece of the crown.";
			link.l1 = "I am asking for a thing of which the world holds one. I shall not haggle: the price is one, and it is whole.";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "You understand San Juan must not merely be handed over but cleared? Garrison, officials, fleet - all of it.";
			link.l1 = "I understand. And until then you get no stone.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "...(rubs the bridge of his nose)... I wrote to Seville before ever we first spoke, captain: I knew how the haggling would end. The answer will be waiting for me aboard the Andromeda off Santa Catalina. Come there, and you shall hear it first.";
			link.l1 = "And why not here, at this table?";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "Because, captain, I am not the only one in this tavern who knows of your Orchid. At sea it is quieter. So it seems to me.";
			link.l1 = "Off Santa Catalina, then. I shall be there.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityAlborettoInShore");
		break;

		case "30":
			dialog.text = "...(breathing hard at the rail)... I owe you my life, captain. Another quarter hour and the Andromeda would have been taken apart for planks.";
			link.l1 = "Those were not Spaniards, senor.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "They were not. They were men who want the Orchid no less than the crown does, and had no intention of paying at all. I did tell you I am not the only one who knows.";
			link.l1 = "Have you brought the answer from Seville?";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "I have. The crown consents. San Juan and twenty-five million - on your terms, entire.";
			link.l1 = "I did not think they would consent so easily.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "...(looks away)... Easily, captain, they consent only to what they reckon on getting back. Ask me no further; I have said too much as it is. Clearing the town will take three weeks.";
			link.l1 = "Where do we meet?";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "The town hall at Santiago, in three weeks. By that day not one of our soldiers will remain on Puerto Rico.";
			link.l1 = "In three weeks, senor Alboretto.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityPreSecondTimer");
		break;

		case "40":
			dialog.text = "San Juan is empty, captain. Garrison, officials, both guard ships - all went out yesterday on the morning tide. The town is yours.";
			link.l1 = "You were quick about it. The money?";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "The deposit - one million, here, by the note. The remaining twenty-four go by transfer through a banking house in Seville: such sums are not held in a colonial treasury and never have been.";
			link.l1 = "A transfer. That is to say, paper.";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "Paper of the Spanish crown, captain. It is worth more than gold, or Europe would long since have gone to war with carts of coin.";
			link.l1 = "...(sweeping up the note)... Very well. The stone?";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "Leave it in a cache ashore and send me the place. I shall not touch it until the last of our soldiers steps ashore in Havana. The word of Alboretto.";
			link.l1 = "You shall have the place. Good day, senor.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SpanishGenerosityDealDone");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
