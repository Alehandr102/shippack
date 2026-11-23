// boal 25/04/04 общий диалог горожан
#include "DIALOGS\english\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 16/06/06
    int iTest, iTemp;
    iTest = FindColony(NPChar.City); // город
    ref rColony;
    string sTemp, sCity;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
	}

	String attrLoc = Dialog.CurrentNode;

	if(HasSubStr(attrLoc, "RumourAboutOwners_"))
	{
		NPChar.RumourOwnerType = FindStringAfterChar(attrLoc, "_");
		Dialog.CurrentNode = "RumourAboutOwners_TakeRumour";
	}

	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "fight":
            LAi_group_Attack(NPChar, Pchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "First time":


            if(NPChar.quest.meeting == "0")
			{
				NPChar.quest.meeting = "1";

                // homo 05/08/06
				dialog.Text =NPCharRepPhrase(npchar,
                                LinkRandPhrase("My name is ", "My name ", "You can call me ")
                                +GetFullName(npchar)
                                +LinkRandPhrase(". What do you need?", "I didn't see you before "
                                +", who are you?", ". Who are you and what do you want from me?"),

        		                LinkRandPhrase("Greetings, " + GetAddress_Form(NPChar) + ". My name is " + GetFullName(npchar) + ". What's your name?",
                                 "Hello, " + GetAddress_Form(NPChar) + "! I am " + GetFullName(npchar) + ". May I know your name?",
                                 "Yes, " + GetAddress_Form(NPChar) + ". What did you want? By the way, my name is " + GetFullName(npchar) + ". And you?"));

				Link.l1 = pcharrepphrase(
                                LinkRandPhrase("Damn me!", "A thousand devils! ", "May it be empty for you! ") + "Yes, I am the captain "+ GetFullName(Pchar)+ LinkRandPhrase(", haven't you heard about me"+NPCharSexPhrase(NPChar,", rascal?","?"), " and "+ GetSexPhrase("the most famous pirate","the most famous female pirate") +" in these waters! ", "and I'll be damned if that's not the case!"),
                                LinkRandPhrase("I am " + GetFullName(Pchar) + ", Captain.", "My name is " + GetFullName(Pchar) + ".", "You can call me Captain " + GetFullName(Pchar) + "."));

				Link.l1.go = "Meeting";
				if (pchar.sex == "Skeleton")	//Проверка на скелета
				{
					if	(GetCharacterEquipSuitID(pchar) == "suit_1")
					{
						dialog.text = "Even though you're dressed up in these rags, you can't fool my old eyes...";
						link.l1 = "Is my disguise really that bad?",
						link.l1.go = "UndeadObuchenie_2";
					}
					else
					{
						dialog.text = "Judging by your ridiculous gait, you have recently risen from the grave. But don't be afraid of me, I won't hurt you.";
						link.l1 = "Surprisingly, YOU should be afraid of ME, not the other way around!";
						link.l1.go = "UndeadObuchenie_11";
					}
				}
			}
			else
			{
// Graveyard keeper quest "Laying the Dead to Rest" (first branch) -->
				if (CheckAttribute(pchar, "GenQuest.GraveyardQuest") && pchar.GenQuest.GraveyardQuest.Town == NPChar.City)
				{
					if (pchar.GenQuest.GraveyardQuest.Stage == "reward")
					{
						link.lgrave = LinkRandPhrase("I have laid all the dead in your graveyard to rest, just as you asked.", "It's done, old man. Your graveyard is at peace again.", "The graveyard, the vault and the crypt are all cleared of the undead.");
						link.lgrave.go = "GraveyardQuest_Reward";
						// easter egg: ask the watcher about the chapel skeleton (only if the player has already seen it)
						if (CheckAttribute(pchar, "GenQuest.GraveyardQuest.ChapelSkelMet") && !CheckAttribute(pchar, "GenQuest.GraveyardQuest.SkelAsked"))
						{
							link.lskel = "Hold on. There's a talking skeleton sitting in the chapel of your graveyard. Where did it come from?";
							link.lskel.go = "GraveyardQuest_AskSkel";
						}
					}
					else
					{
						link.lgrave = LinkRandPhrase("I haven't finished with your graveyard yet.", "Give me a little more time, old man.", "Not all the undead in your graveyard are laid to rest yet.");
						link.lgrave.go = "GraveyardQuest_Busy";
					}
				}
				else
				{
					if (GraveyardQuest_IsAvailable(NPChar))
					{
						link.lgrave = LinkRandPhrase("Do you need any help with the undead in the graveyard?", "Old man, I can see something is troubling you?", "On my way into your lodge I heard noises in the graveyard - what's going on there?");
						link.lgrave.go = "GraveyardQuest_Offer";
					}
				}
				// <-- Graveyard keeper quest
				link.l1 = PCharRepPhrase("I want to learn something about the town in the vicinity!", "I want to find out something about the city that is not far from here.");
				link.l1.go = "new question";
				// Квестовый генератор священника. Квест №2. Warship -->
				if(CheckAttribute(PChar, "GenQuest.ChurchQuest_2.AskPeople") && !CheckAttribute(NPChar, "GenQuest.ChurchQuest_2.AskedPeople") && NPChar.location == PChar.GenQuest.ChurchQuest_2.QuestTown + "_town")
	            {
					link.l2 = LinkRandPhrase("You know", "You know", "Do you know anything about that") + "that the local church was robbed recently?";
					link.l2.go = "Citiz_GenQuest_Church_2_1";
				}
				// <-- Квестовый генератор священника. Квест №2.

                dialog.Text = NPCharRepPhrase(npchar,
		                PCharRepPhrase(LinkRandPhrase("Oh, it's you "+GetFullName(Pchar)
                        +" "+ GetSexPhrase(", my friend",", my friend") +"", "Glad to see you, "+ GetSexPhrase("old man","beauty") +" "
                        + Pchar.name, "Oh, yes, it's "+ GetSexPhrase("the captain himself","herself") +" "
                         +GetFullName(Pchar))
                                        +LinkRandPhrase(". I thought you've already dead!", "And probably "+ GetSexPhrase("drunk as always.","already a little tipsy.") +"", ". And, as I see, still at large!")
                                        +LinkRandPhrase(" Tell me, what do you want?", " What do you want this time?", " Are you bothering me again for nothing?"),
                                        LinkRandPhrase(TimeGreeting() + ", captain ", "Hello, "+GetAddress_Form(NPChar)+" ", "Greetings, Captain ")+GetFullName(Pchar)
                                        +LinkRandPhrase(". What are you interested in?", ". Why are you here?", ". What do you want to know this time?")

                                        ),

                        PCharRepPhrase(LinkRandPhrase("Hello, Captain ", "Hello, oh it's you "+GetAddress_Form(NPChar)+" ", "Ah, Captain ")
                                        +GetFullName(Pchar)
                                        +LinkRandPhrase(", I thought that we won't meet again,",
                                                         ", I wouldn't say that I am happy to see you, but", "I see you're still alive, it's unfortunate, huh")
                                        +LinkRandPhrase(" what do you want?", " why are you here?", " how may I be useful?"),
                                        LinkRandPhrase(TimeGreeting() + ", captain " + GetFullName(Pchar) + ". How may I help you?",
                                        "Oh, captain " + GetFullName(Pchar) + "! What do you want to know this time?",
                                        "Greetings, " + GetAddress_Form(NPChar) + " " + GetHeroLastName() + ". Did toy want something?"))
                                        );
	   			link.l3 = "Nothing, I changed my mind" + GetSexPhrase(".","a.");
				link.l3.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;
		case "Meeting":
		    dialog.Text  = NPCharRepPhrase(npchar, LinkRandPhrase("Aren't you lying, Captain " + GetFullName(Pchar)+"?",
                                                                    "So what" + PChar.name+"?",
                                                                    "I'll remember you, " + GetFullName(Pchar)+".")
                                                                     + " Now tell me, what do you want?",
                                                    LinkRandPhrase("Nice to meet you, Captain " + PChar.name,
                                                                    "Glad to our acquaintance, " + GetAddress_Form(NPChar) + " " + GetHeroLastName(),
                                                                     "It's a pleasure, Captain " + PChar.name) + ". But you probably didn't just want to know my name, did you?");

// Graveyard keeper quest "Laying the Dead to Rest" (first branch) -->
				if (CheckAttribute(pchar, "GenQuest.GraveyardQuest") && pchar.GenQuest.GraveyardQuest.Town == NPChar.City)
				{
					if (pchar.GenQuest.GraveyardQuest.Stage == "reward")
					{
						link.lgrave = LinkRandPhrase("I have laid all the dead in your graveyard to rest, just as you asked.", "It's done, old man. Your graveyard is at peace again.", "The graveyard, the vault and the crypt are all cleared of the undead.");
						link.lgrave.go = "GraveyardQuest_Reward";
						// easter egg: ask the watcher about the chapel skeleton (only if the player has already seen it)
						if (CheckAttribute(pchar, "GenQuest.GraveyardQuest.ChapelSkelMet") && !CheckAttribute(pchar, "GenQuest.GraveyardQuest.SkelAsked"))
						{
							link.lskel = "Hold on. There's a talking skeleton sitting in the chapel of your graveyard. Where did it come from?";
							link.lskel.go = "GraveyardQuest_AskSkel";
						}
					}
					else
					{
						link.lgrave = LinkRandPhrase("I haven't finished with your graveyard yet.", "Give me a little more time, old man.", "Not all the undead in your graveyard are laid to rest yet.");
						link.lgrave.go = "GraveyardQuest_Busy";
					}
				}
				else
				{
					if (GraveyardQuest_IsAvailable(NPChar))
					{
						link.lgrave = LinkRandPhrase("Do you need any help with the undead in the graveyard?", "Old man, I can see something is troubling you?", "On my way into your lodge I heard noises in the graveyard - what's going on there?");
						link.lgrave.go = "GraveyardQuest_Offer";
					}
				}
				// <-- Graveyard keeper quest
            link.l1 = PCharRepPhrase("I want to learn something about this town!", "I want to learn something about the city.");
			link.l1.go = "new question";
			// Квестовый генератор священника. Квест №2. Warship -->
			if(CheckAttribute(PChar, "GenQuest.ChurchQuest_2.AskPeople") && !CheckAttribute(NPChar, "GenQuest.ChurchQuest_2.AskedPeople") && NPChar.location == PChar.GenQuest.ChurchQuest_2.QuestTown + "_town")
	        {
				link.l2 = LinkRandPhrase("You know", "Do you know anything about that", "Do you know anything about that") + "that the local church was robbed recently?";
				link.l2.go = "Citiz_GenQuest_Church_2_1";
			}
			// <-- Квестовый генератор священника. Квест №2.

			link.l3 = "Nothing, I changed my mind" + GetSexPhrase(".","a.");
			Link.l3.go = "exit";
		break;

		case "new question":
            dialog.text = NPCharRepPhrase(npchar,
		                PCharRepPhrase(LinkRandPhrase("Oh, yes, I'm glad ask you what you want as a drink.", "Do you like to chat, Captain? Well, me too... Especially over a mug of rum.", "Yes, " + PChar.name + "?"),
                                        LinkRandPhrase("Well, what else do you want?", "I see, Captain "+PChar.name+", you are willing to chat? ", "You tortured me with your questions, Captain, it would be better to drink rum.")),
		                PCharRepPhrase(LinkRandPhrase("What did you want to know "+ GetAddress_Form(NPChar) + "?", "I'm listening to you, Captain.", "I have to go, so ask me soon, Captain."),
                                        LinkRandPhrase("I always glad to have a pleasant companion, " + GetAddress_Form(NPChar) + " " + GetHeroLastName() + ". Speak.",
                                                        "Yes, " + GetAddress_Form(NPChar) + "?",
                                                        "Do you like to chat, Captain? Well, me too..."))
                            );

            // homo 25/06/06
			link.l1 = LinkRandPhrase ("What kind of stories are in use in the tavern here?",
                                    "What's new in these parts?",
                                    "How does life flow on land?");
			link.l1.go = "rumours_citizen";
			link.l2 = "Nothing, I changed my mind" + GetSexPhrase(".","a.");
			link.l2.go = "exit";
		break;

		case "RumourAboutOwners_TakeRumour":
			sTemp = sRumourAboutOwners_CityRumour(NPChar.City, NPChar.RumourOwnerType);
			Dialog.Text = sTemp;
			link.l1 = "Thank you.";
			link.l1.go = "exit";
			// Здесь можно делать проверка по квестам, какой слух выпал
			DeleteAttribute(PChar, "QuestTemp.RumoursAboutOwners");
			break;
		//=================== ноды квеста священника. поиск грабителей (квест №2)==================
		case "Citiz_GenQuest_Church_2_1":
			if(rand(3) != 1)
			{
				dialog.text = LinkRandPhrase("No, I don't know anything about this.", "I don't know anything about this issue.", "I don't know anything.");
					link.l1 = LinkRandPhrase("Okay, good.", "Okay, thanks.", "Okay, sorry.");
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = LinkRandPhrase("I don't know anything about this, but you can ask the regulars at the tavern - they will certainly tell you something.", "I can't tell you anything, ask someone at the tavern better.", "I don't know anything specific, but I I heard they were talking about it in the tavern.");
				link.l1 = "Thanks for that.";
				link.l1.go = "exit";
			}

			NPChar.GenQuest.ChurchQuest_2.AskedPeople = true;
		break;

		//замечение по обнажённому оружию
		case "CitizenNotBlade":
			if (loadedLocation.type == "town")
			{
				dialog.text = NPCharSexPhrase(NPChar, "Listen, as a citizen of this city, I ask you not to walk with a naked blade here.", "You know, as a citizen of this city, I ask you not to walk with a naked blade here.");
				link.l1 = LinkRandPhrase("Okay.", "Okay.", "Whatever you say...");
			}
			else
			{
				dialog.text = NPCharSexPhrase(NPChar, "Be careful on turns, "+ GetSexPhrase("buddy","girlfriend") +", when you run with a gun in your hand. I can get nervous...", "I don't like it when "+ GetSexPhrase("men","all sorts here") +" they walk in front of me with weapons at the ready. It scares me...");
				link.l1 = RandPhraseSimple("Understood.", "I'm removing.");
			}
			link.l1.go = "exit";
			NextDiag.TempNode = "First Time";
		break;

		case "Man_FackYou":
			dialog.text = LinkRandPhrase("Yes you are "+ GetSexPhrase("thief, my dear! Guards, hold him down","thief! Guards, hold her down") +"!!!", "Wow! As soon as I looked at it, you went straight into the chest with your head! Hold this theif!!!", "Guards! They're robbing!!! Hold this theif!!!");
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
		
		case "UndeadObuchenie_2":
			dialog.text = "I have lived in this graveyard almost all my life. And I can easily figure out who is in front of me, human or undead. But it's not your clothes that betray the undead in you, but your GAIT. Look at how you stand - uncertainly, it's immediately obvious that you recently got up from the grave. Simply put, he's new.";
			link.l1 = "Yes, you see this world in a completely different way with dead eyes. And it makes me rock from side to side.",
			link.l1.go = "UndeadObuchenie_3";
		break;
		
		case "UndeadObuchenie_3":
			dialog.text = "You don't have to be afraid, it's just me who's so big-eyed. The rest of the inhabitants of this island will not even notice the trick, and they will take you for their own, that is, for a person. But once you take off your disguise, people will be hostile to you. It's clear to you, isn't it?";
			link.l1 = "It's unpleasant. But... Do I have any allies?",
			link.l1.go = "UndeadObuchenie_13";
		break;
		
		case "UndeadObuchenie_11":
			dialog.text = "Hehe, no! I'm used to people like you. I even maintain good relations with some undead. But I am an exception to the rule. You need to be careful, because the rest of the inhabitants of this island will be afraid of you and... will try to destroy you.";
			link.l1 = "This is unpleasant... But then what should I do?",
			link.l1.go = "UndeadObuchenie_12";
		break;
		
		case "UndeadObuchenie_12":
			dialog.text = "You need to wear a disguise. Find some rags and walk among people like the spitting image of your own.";
			link.l1 = "Do I have any allies?",
			link.l1.go = "UndeadObuchenie_13";
		break;
		
		case "UndeadObuchenie_13":
			dialog.text = "Of course, almost any undead is your friend. You can look for your relatives in the caves, they will gladly join your ship's crew.";
			link.l1 = "It's so good that I looked in on you. I will continue to explore this world, in this cursed body. Goodbye.",
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		//=================== Graveyard keeper quest "Laying the Dead to Rest" ===================
		case "GraveyardQuest_Offer":
			dialog.text = RandPhraseSimple("You came at the right time. There is no more peace in my graveyard - the dead have taken to rising from their graves, wandering among the headstones, and the foulest of them have holed up in the vault and in the crypt beneath the graveyard. An old man like me can't deal with them. Lay them all to rest - in the graveyard, in the vault and in the crypt - and I won't leave you unrewarded.", "Good that you came. I'm in trouble: the dead won't stay in their graves, they wander among the headstones all night, and in the vault and the crypt sits something too dreadful to think of. I'm too old to swing a blade now. Put the undead to rest - both in the graveyard and below it - and you won't regret it.");
			link.l1 = LinkRandPhrase("I'll take it. Where do I look for the undead?", "I'll help you. Where do I go?", "Agreed. Where are the undead holed up?");
			link.l1.go = "GraveyardQuest_Accept";
			link.l2 = RandPhraseSimple("Sorry, I have other matters to attend to right now.", "Not now, I have more important things to do.");
			link.l2.go = "exit";
		break;

		case "GraveyardQuest_Accept":
			dialog.text = RandPhraseSimple("Right here, in the graveyard. First clear out those wandering among the graves. Once you're done, descend into the vault and the crypt - that's where the strongest of the undead nest. Come back to me when you've laid every last one of them to rest.", "Here, in the graveyard and below it. First put to rest those wandering on the surface, then descend into the vault and the crypt - the strongest ones lurk there. Come back when you've finished with all of them.");
			link.l1 = LinkRandPhrase("I'll deal with them.", "Consider the undead gone already.", "It will be done, old man.");
			link.l1.go = "exit";
			GraveyardQuest_Start(NPChar);
		break;

		case "GraveyardQuest_Busy":
			dialog.text = RandPhraseSimple("The undead won't lay themselves to rest. Finish the job - clear the graveyard, the vault and the crypt.", "You came too early. As long as a single dead one still walks - in the graveyard, the vault or the crypt - the job isn't done.");
			link.l1 = RandPhraseSimple("I'll be back soon.", "I'm already on it.");
			link.l1.go = "exit";
		break;

		case "GraveyardQuest_Reward":
			dialog.text = RandPhraseSimple("At last my graveyard is quiet again. You kept your word, and I'll keep mine. Take these books - there is much wisdom in them, and they will serve you well. And accept my gratitude.", "Peace at my graveyard once more... I'm in your debt. Take these books - there is wisdom in them that gold can't buy. And my gratitude goes with you.");
			link.l1 = LinkRandPhrase("Thank you. If the undead rise again - you know where to find me.", "Glad I could help. Be well, old man.", "Don't mention it. Stay well.");
			link.l1.go = "exit";
			GraveyardQuest_GiveReward(NPChar);
		break;

		// easter egg: the watcher's account of the talking skeleton in the chapel (5 variants)
		case "GraveyardQuest_AskSkel":
			switch (rand(4))
			{
				case 0: dialog.text = "Ah, that one... He's been sitting in the chapel longer than I have. Quiet, harmless - never hurt a fly, in life or after it. I don't bother him, and he doesn't bother me. Sometimes he's better company than half the living folk around here."; break;
				case 1: dialog.text = "That's the old watcher who kept this place before me. He grew so attached to his graveyard that not even death could drive him out. He rose and sat himself in the chapel - to keep watch, as he always did. I won't raise a hand to him: he knows his trade better than any living man."; break;
				case 2: dialog.text = "When the undead started rising, he rose too. But he alone kept his wits - didn't lunge to kill, just sat in the chapel and spoke. I hadn't the heart to lay him to rest. Let him sit, so long as he harms no one."; break;
				case 3: dialog.text = "He's a monk, who used to pray here back when the chapel was new. A righteous soul he was, so he rose not as a wicked thing but a gentle one. Mutters his prayers and spins his tales. Let him be - who's the worse for it?"; break;
				case 4: dialog.text = "Honestly - I don't know where he came from myself. One day he was simply there in the chapel, speaking to me like a man. We've been neighbours ever since. Seems not every death turns a man into a monster. Some stay human, even as bones."; break;
			}
			pchar.GenQuest.GraveyardQuest.SkelAsked = true; // the skeleton has been asked about - don't offer again (until the next easter egg)
			link.l1 = RandPhraseSimple("Curious. Well, be seeing you, old man.", "So that's how it is... Thanks for telling me.");
			link.l1.go = "exit";
		break;
		//=================== <-- Graveyard keeper quest ===================
	}
}
