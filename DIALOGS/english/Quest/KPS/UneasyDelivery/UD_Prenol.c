// Gregg: Jacque Prenol - quest "Goods for Richard Tillman" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\uneasydelivery.c
//
// Prenol's catchphrase "That is fine, that is good" (nodes 9 and 18) is not decoration:
// later it turns out to be the key phrase for the contact at the pearl fishery.
// It must read identically here, in Shon's node 20 and in Charly's check.

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
			NextDiag.TempNode = "First time";
			if (sti(PChar.UneasyDelivery) > 3)
			{
				dialog.text = "How goes the business I entrusted to you?";
				link.l1 = "I am working on it.";
				link.l1.go = "exit";
				break;
			}
			if (PChar.UneasyDelivery == "3")
			{
				dialog.text = "There is no work today, come tomorrow.";
				link.l1 = "Very well, I shall come tomorrow without fail.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Greetings, I am " + GetFullName(NPChar) + "! Do you need a consignment of excellent cinnamon or paprika? Or would you care to buy a little live merchandise? Perhaps the captain wishes to acquire ebony or mahogany of the highest quality?";
			link.l1 = "I need nothing. I have brought you a consignment of porcelain from Richard Tillman.";
			link.l1.go = "2";
		break;

		case "2":
			// Gregg: mistaking the player for Fatman only fits after the prologue, where the
			// player really did bring his lugger. Via the innkeeper Prenol simply expects
			// a man sent by Tillman.
			if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa"))
			{
				dialog.text = "Ah! So Richard sent you?";
				link.l1 = "Captain " + GetFullName(PChar) + ". Here is Richard Tillman's letter, he explains everything in it.";
			}
			else
			{
				dialog.text = "Ah! You are captain Tommi, called Fatman, are you not?";
				link.l1 = "No, I am captain " + GetFullName(PChar) + ". I stand in Fatman's place now. Here is Richard Tillman's letter, he explains everything in it.";
			}
			link.l1.go = "3";
		break;

		case "3":
			TakeNItems(PChar, "RichardTillmanNotice", -1);
			// Gregg: praise for killing Fatman only fits after the prologue.
			// Coming through the innkeeper, the player never laid eyes on Fatman.
			if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa"))
			{
				dialog.text = "...(reads the letter)... So, so. Richard writes that you may be relied upon. Well, we shall see.";
			}
			else
			{
				dialog.text = "...(reads the letter)... Well I never! Richard says you killed Fatman himself. Well done, I take off my hat.";
			}
			link.l1 = "Listen, monsieur Prenol, perhaps we might talk business? You are waiting for your porcelain, I am waiting for my reward. Let us finally have done with it.";
			link.l1.go = "4";
		break;

		case "4":
			AddMoneyToCharacter(PChar, 6000);
			TakeNItems(PChar, "UneasyDeliveryPorcelain", -25);
			dialog.text = "Ah yes, here are your 6,000. Forgive me, I have never paid anyone more for a delivery.";
			link.l1 = "We agreed on seven thousand. Six does not suit me at all.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "We agreed on nothing, monsieur " + GetHeroLastName() + ". It was Tillman who lied to you, saying I would pay seven thousand. If you please, demand your thousand from Tillman. I have no more money.";
			link.l1 = "No, you will pay me, " + NPChar.lastname + "! I was warned that you constantly try to dupe those you work with. That will not pass with me. Hand over the thousand at once, or... or I shall not answer for myself, " + NPChar.lastname + ". I have warned you.";
			link.l1.go = "6";
		break;

		case "6":
			AddMoneyToCharacter(PChar, 1000);
			dialog.text = "Very well, very well, here is your thousand. Are you calmed now?";
			link.l1 = "I ought to strike you a couple of times, to teach you better.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "One must survive somehow. I often have to work with those who cannot or will not fight for their money. It is hard not to seize the chance and try to cheat someone out of a couple of thousand. But I like you! You know how to get your way. And I have work for you. Or rather, I shall have work.";
			link.l1 = "Only if you try once more to...";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "...(interrupting)... No, of course not, never again. Call in within the week and you shall have an excellent task. Will you come?";
			link.l1 = "So be it, I shall call in.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "That is fine, that is good! I shall be waiting for you.";
			link.l1 = "Keep well.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryTimer");
		break;

		case "10":
			dialog.text = PChar.name + ", these are bandits, they want to kill me, help!";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryPrenolExit");
		break;

		case "11":
			dialog.text = "You killed them, " + PChar.name + "? All of them?";
			link.l1 = "Yes, the bandits are dead.";
			link.l1.go = "12";
		break;

		case "12":
			AddMoneyToCharacter(PChar, 5000);
			dialog.text = "Thank God! Here, take 5000 piastres for your help. If not for you, I should no longer be here.";
			link.l1 = "So that was the work you meant to entrust to me?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "No, no, I wanted to ask you to carry a cargo. But then these thugs turned up and began demanding my savings. I knew you would come to me, and I deliberately played for time, waiting for your arrival.";
			link.l1 = "You might at least have stayed and helped me! Two against three is far better than alone.";
			link.l1.go = "14";
		break;

		case "14":
			if (CheckAttribute(PChar, "UneasyDelivery.NoCocoa"))
			{
				dialog.text = "I cannot hold a sword at all, " + PChar.name + ". But you are a real fighter, one can see it at a glance, and I was certain you would manage those three. Thank you.";
			}
			else
			{
				dialog.text = "I cannot hold a sword at all, " + PChar.name + ". But you are a real fighter, you killed Fatman, and I was certain you would manage those three. Thank you.";
			}
			link.l1 = "Do not mention it. You paid me, I am content. And what of the work? You had some cargo to deliver?";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "I shall entrust the cargo to another man, but to you I wish to offer a commission. I know who sent those three, and I wish to settle with him. He is an old rival of mine who has tried to spite me more than once. I want you to kill him. Do you agree?";
			link.l1 = "How much will you pay?";
			link.l1.go = "16";
		break;

		// Gregg: Damned Destiny promised 20,000. The sums of the quest are cut by a factor
		// of ten - see the header of uneasydelivery.c; the same division here, otherwise
		// Prenol's promise would exceed the whole debt Shon later speaks of.
		case "16":
			dialog.text = "In the chest of his house you will find 15,000 piastres and a collection of unique weapons. He began gathering it in his youth. Well then, do you agree?";
			link.l1 = "Excellent. His name, and where he lives.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "His name is Shon Stablovski, he lives on the island of Antigua. When you find him, kill him without conversation.";
			link.l1 = "Very well, I understand.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "That is fine, that is good! I shall await you in my house. A safe journey.";
			link.l1 = "Good day, Jacque.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("UneasyDeliveryJacquePrenolBackToHouse");
		break;

		// Gregg: player caught stealing - the short local variant, see UD_Tillman.c
		case "Man_FackYou":
			dialog.text = LinkRandPhrase("Why, you are a thief! Guards, hold the thief!!!", "Well I never! I look away for a moment and you are head first in the chest! Stop, thief!!!", "Guards! Robbery!!! Stop the thief!!!");
			link.l1 = "Ah, the devil!!!";
			link.l1.go = "fight";
		break;

		case "fight":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LAi_SetOwnerTypeNoGroup(npchar);
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
