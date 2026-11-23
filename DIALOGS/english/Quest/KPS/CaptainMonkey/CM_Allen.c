// Gregg: Allen Gladstonn and the Black Boy - quest "The Monkey Ship" (ported from
// Damned Destiny). Logic - Program\Quests\reaction_functions\captainmonkey.c
// Nodes 1-5 - the hook by the Bas-Terre church, 9-10 - Allen in the Beliz church,
// 11-13 - the last meeting beyond the gates, 14-19 - the Black Boy on the shore.
// Nodes 6-8 of the original are gone: there Allen came to the grotto to search the
// prisoner and take his sabre, and we take nothing - skeletons guard it instead.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sIsland = PChar.CaptainMonkey.IslandID;
	sIsland = XI_ConvertString(sIsland + "Gen");

	string sGrotIsland;
	int iMoney;
	// Gregg: the mod-wide "remove quest rank requirements" toggle. Read LIVE, the way
	// Tillman and Sajfus do it: with the option on the hook opens from rank one.
	int iRankReq = 1;
	if (!bQuestsRank) iRankReq = 13;

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "Nobody, nobody will believe me... They all take me for a madman...";
			link.l1 = "That is a great pity...";
			link.l1.go = "exit";

			// Gregg: the hook is offered until the quest is taken. The rank threshold
			// matches CM_RANK_REQ in the logic (13): defines do not reach a dialogue segment.
			// The rank is read directly: GetRank(PChar, 0) adds the player's rank again
			// inside and would return double - the gate would open at rank seven.
			if (PChar.CaptainMonkey == "0")
			{
				if (CheckAttribute(PChar, "CaptainMonkey.NotTrust"))
				{
					link.l1 = "Where did you say it happened - off " + sIsland + "?";
					link.l1.go = "4";
					link.l2 = "That is a great pity...";
					link.l2.go = "exit";
				}
				else
				{
					if (sti(PChar.rank) >= iRankReq)
					{
						link.l1 = "What are you talking about, vagabond? Who will not believe what?";
						link.l1.go = "1";
						link.l2 = "That is a great pity...";
						link.l2.go = "exit";
					}
				}
			}
		break;

		case "1":
			dialog.text = "Nobody believes me, not even the priest, and yet I speak the honest truth, nothing but the truth...";
			link.l1 = "What are you mumbling about? Tell it in order. What happened to you?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "In order... What order can there be, sir, when you are boarded by a ship with monkeys aboard!";
			link.l1 = "Well now, lad... I see you have had a drop too much.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Nothing of the sort, " + GetAddress_FormToNPC(NPChar) + "! Believe me or not, but I speak the honest truth! In the coastal waters of " + sIsland + " my trading sloop was attacked by a lugger with monkeys aboard! With monkeys, " + GetAddress_FormToNPC(NPChar) + "! Those apes dealt with my sailors like seasoned pirates, and threw me overboard like an empty rum keg! Do not ask me how I made my way home! Nobody believes me, nobody...";
			link.l1 = "Well, only a madman could believe a tale like that. Where did you say it happened - off " + sIsland + "?";
			link.l1.go = "4";
			link.l2 = "And you expect me to believe this nonsense? To my mind you had better rinse your brains out with a mug of strong rum!";
			link.l2.go = "5";
		break;

		case "4":
			dialog.text = "Yes, " + GetAddress_FormToNPC(NPChar) + ". It was in the coastal waters of " + sIsland + ". I swear I speak the plain truth!";
			link.l1 = "Very well, I believe you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyStart");
		break;

		case "5":
			PChar.CaptainMonkey.NotTrust = true;
			dialog.text = "Nobody, nobody will believe me...";
			link.l1 = "...";
			link.l1.go = "exit";
		break;

		case "9":
			dialog.text = "Captain, is that you?! However did you find me?!";
			link.l1 = "Why these foolish questions? You had better think how many seconds are left to your sinful soul before it flies up to heaven.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Surely you would not bare a blade in a holy place? I think you are not capable of such sacrilege! Let us step outside the town - there I shall tell you a great deal of interest!";
			link.l1 = "Very well, let us go.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyFromChurchToTown");
		break;

		case "11":
			dialog.text = "Captain, this is the second time you astonish me! How did you find me now?!";
			link.l1 = "I trust this meeting of ours will be the last, my dear friend of animals.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "I understand - you wish to make me tell all I know of captain Mowgli. But not even on pain of death shall I do it. If I loosen my tongue, Mowgli will find me anywhere, and then I am certainly a dead man. In a fight with you I have at least some chance.";
			link.l1 = "I am sorry to disappoint you, but you have chosen the shortest road to the next world.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "We shall see! On guard, captain " + GetFullName(PChar) + "!";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyBattleWithAllen");
		break;

		case "14":
			dialog.text = UpperFirst(GetAddress_FormToNPC(NPChar)) + ", could you spare me a few coins for food?..";
			link.l1 = "Why do you not find work? Living on alms is hardly an enviable lot.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "The thing is, I have not lived here long, and my whole childhood was spent in the jungle, among monkeys. I have no trade, and I hardly know a soul here - so I cannot yet earn my daily bread...";
			link.l1 = "In the jungle among monkeys? Then who taught you to speak? In all my life I have seen only one talking monkey - a certain captain Mowgli.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "You know captain Mowgli?! An unenviable fate usually awaits anyone who makes his acquaintance! And yet you, as I see, are alive and well?!";
			link.l1 = "So you know him too? Speak, then!";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Indeed I do, I knew him back when he was a man - he raised me. And ever since he made his pact with the unclean powers and turned into an ape, his cruelty has known no bounds. I could bear his abuse no longer and ran away. But how did you survive a meeting with him, when he is immortal?!";
			link.l1 = "That is a long story - my life was spared only to get at my money, but by good fortune I managed to escape. So you say he made a pact with the unclean powers?";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Yes, " + GetAddress_FormToNPC(NPChar) + ". One day he found a sealed bottle washed up on the shore and out of curiosity broke it open... The wicked demon freed from its prison talked him into it... I do not know what came over him, but he took that step. Why he agreed I still cannot understand. The demon promised Mowgli immortality and the support of every kind of fiend, and in return demanded that he punish men with death for one of the most innocent of earthly sins - curiosity... They signed a contract, and Mowgli set his name to it in his own blood. As long as that contract exists, Mowgli is immortal.";
			link.l1 = "What do you mean, as long as the contract exists? Then it can be destroyed?";
			link.l1.go = "19";
		break;

		case "19":
			iMoney = sti(PChar.money) / 100;
			if (iMoney > 10000) iMoney = 10000;
			AddMoneyToCharacter(PChar, -iMoney);

			sGrotIsland = CaptainMonkeyGetGrotIsland();
			dialog.text = "That is no easy matter, " + GetAddress_FormToNPC(NPChar) + ". You were very lucky to meet me - I shall tell you how to reach it. Mowgli hid the casket with the contract in a grotto, " + sGrotIsland + ". The casket is well guarded by fiends of every kind, but if you manage to seize that paper and give it to the fire, the pact will be broken, Mowgli will lose his immortality and be powerless!";
			link.l1 = "Yes, I was lucky indeed to meet you... Take these " + iMoney + " coins and try to learn some trade. I hope we shall meet again!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("CaptainMonkeyFindContract");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
