// boal 08/04/04 общий диалог торговцев
#include "DIALOGS\english\Rumours\Common_rumours.c"  //homo 25/06/06

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	int skullprice = (sti(PChar.rank) * 4000) - ((sti(PChar.rank) * 2400) * (0.01* (GetSummonSkillFromName(PChar, SKILL_COMMERCE))))+ 10000;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	// LEO: приписка-надбавка (NameBonus/Tail) у имени торговца — только на экране заказов фрахта, чистим на каждом узле
	DeleteAttribute(NPChar, "NameBonus");
	DeleteAttribute(NPChar, "NameBonusTail");


	string iDay, iMonth, sTemp, sMoney, attrL;

	iDay = environment.date.day;
	iMonth = environment.date.month;
	string lastspeak_date = iday + " " + iMonth;

	int iMoney;
	int iQuantityGoods;
	int iTradeGoods;
	int iTmp;

    bool ok;
	bool bTradeQuest = false;

	int iTest, cn, i;
	iTest = FindColony(NPChar.City); // город магазина
	ref rColony, chref;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
	}

    int iSeaGoods = LanguageOpenFile("ShipEatGood.txt"); // нужно заменить на GetGoodsNameAlt(idx)

    if (!CheckAttribute(npchar, "quest.item_date"))
    {
        npchar.quest.item_date = "";
    }
    if (!CheckAttribute(npchar, "quest.trade_date"))
    {
        npchar.quest.trade_date = "";
    }

    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Store\" + NPChar.City + "_Store.c";
    if (!HasCityDialogFile("Store", NPChar.City, "_Store.c")) NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Store\DefaultCity_Store.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
	else
	{
		NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Store\DefaultCity_Store.c";
	    if (LoadSegment(NPChar.FileDialog2))
		{
	        ProcessCommonDialog(NPChar, Link, NextDiag);
			UnloadSegment(NPChar.FileDialog2);
		}
	}
    // вызов диалога по городам <--
    ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 25/06/06
	string attrLoc   = Dialog.CurrentNode;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar,
					LinkRandPhrase("The alarm has been raised in the city, they are looking for you everywhere! If I were you, I wouldn't stay here too long.", "The entire city guard is scouring the city in search of you. I'm not an idiot and I won't talk to you!", "Run before the soldiers make a sieve out of you, "+ GetSexPhrase("buddy", "girlfriend") +"..."),
					LinkRandPhrase("What do you need, scoundrel?! The city guards have already are on your trail, you can't get far, dirty pirate!", "Dirty murderer, get out of my house! Guards!!", "I'm not afraid of you, scoundrel! You'll be hanged in our fort soon, you won't get far..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Heh, anxiety is not a problem for me...", "They will never catch me."),
					RandPhraseSimple("Shut your mouth, " + GetWorkTypeOfMan(npchar, "") + ", or I'll rip out your filthy tongue!", "Heh, " + GetWorkTypeOfMan(npchar, "") + ", and still there - catch pirates! I'll tell you what, buddy: stay quiet and you'll live..."));
				link.l1.go = "fight";
				break;
			}
			// должник гильдии: лавочники нации-кредитора до возврата долга встречают ТОЛЬКО разговором о долге
			if (CheckAttribute(pchar, "GuildDebt") && sti(NPChar.nation) == sti(pchar.GuildDebt.nation))
			{
				dialog.text = "(The shopkeeper recognizes you and darkens.) The guild has sent your name to every office, " + GetAddress_Form(NPChar) + ". A pocketed cargo, a forfeit... Until you settle the debt, we have nothing to talk about. Pay - or be gone.";
				link.l1 = "I've come about the debt. I want to settle it.";
				link.l1.go = "GuildDebt_Settle";
				link.l2 = "I need to gather the money. I'll be back.";
				link.l2.go = "exit";
				NextDiag.TempNode = "First time";
				break;
			}
			//homo Линейка Блада
            if (CheckAttribute(Pchar, "questTemp.CapBloodLine") && Pchar.questTemp.CapBloodLine == true)
            {
                dialog.Text = LinkRandPhrase("Hey, Dr. Blood! " + TimeGreeting() + ".",
                                    "Glad to see you, Peter Blood.",
                                    "It's good that you looked at me, " + GetFullName(pchar) + ". How is Colonel Bishop?");
                Link.l1 = "Alas, I'm already leaving, " + npchar.name + ". See you later.";
				Link.l1.go = "exit";
				NextDiag.TempNode = "First time";
				break;
            }
			//Sinistra Пролог Анжелика Тич
            if (CheckAttribute(pchar, "questTemp.AnjelikaTichPrologue") && pchar.questTemp.AnjelikaTichPrologue == "ATP")
            {
                dialog.Text = "Hello, senorita! Would you like to buy something? Maybe you need salt? I have a lot of salt! Ah... What a salt!",
                Link.l1 = "Senor, are you all right?";
				Link.l1.go = "exit";
				break;
            }
			if (npchar.id != "Pirates_trader" && npchar.id != "Caiman_trader" && CheckAttribute(pchar, "BSStart"))
			{
				//Black sails
				if(pchar.questTemp.BlueBird == "over")
				{
					dialog.text = "Captain, nothing personal, but a very influential person has forbidden me to do business with you. No deals, sorry.";
					link.l1 = "I'm sorry, but I don't understand. What happened?";
					link.l1.go = "BS_NPVK_2";
					break;
				}
			}
            dialog.text = NPCharRepPhrase(npchar, "New client - new gold. "
							+ LinkRandPhrase("Money has no smell. ", "Piastres don't smell. ", "You will not be satisfied with virtue alone! ")+
							+GetFullName(npchar)+  ", at your service!",
			                + LinkRandPhrase("Nice to meet a new client, ", "Glad to meet a new client, ", "Welcome, ")
							+ GetAddress_Form(NPChar)+ ". My name is "  + GetFullName(npchar)+
							+ RandPhraseSimple(", my store in your service.", ", I am in your service."));
			link.l1 = "Let me introduce myself. " +RandPhraseSimple("My name is", "I am")+ " captain "+ GetFullName(pchar)+".";
			link.l1.go = "node_1";
			NextDiag.TempNode = "Second time";
			// Gregg: «Чёрная Орхидея», глава 5 - лавочник Портобелло помнит компаньона Торангеры.
			// Common_Store.c обслуживает ВСЕХ лавочников мода, поэтому гард обязателен. Отбираем
			// по городу, а не по id: id пережил бы только текущую расстановку, а City стоит
			// у всех 32 торговцев с этим диалогом (проверено по characters\init\*.c).
			// Чтение pchar.DestinyGift - ВЛОЖЕННЫМ if: короткого замыкания в Storm нет, и у игрока
			// без этого квеста атрибута попросту не существует.
			if (npchar.city == "PortoBello" && CheckAttribute(pchar, "DestinyGift"))
			{
				if (pchar.DestinyGift == "23")
				{
					link.l2 = "Tell me, have you ever heard of Bernaldinio Toranger, captain of the Vulture?";
					link.l2.go = "BO_Estikos_1";
				}
			}
		break;

		//Black sails
		case "BS_NPVK_2":
            dialog.text = "Contact Miss Guthrie, she recently arrived from Nassau and took control of the prize deals. In Bermudes, Jackman himself comes to her with a bow.";
            link.l1 = "Hmm. Guthrie... Something familiar. Okay, see you later.";
			link.l1.go = "exit";
			sld = CharacterFromID("Pirates_trader");
			if(sld.lastname != "Guthrie" && !CheckAttribute(PChar, "questTemp.BSPrologue.WaitingForNassauSiege") && !CheckAttribute(PChar, "BS_NassauWaitingForSiege"))
			{
				SetFunctionLocationCondition("BST_Pirates_Store", "Pirates_Store", false);
				BS_ReplaceTraderWithGatri();
				sld.Dialog.FileName = "Quest\BlackSails\NePluyjVKolodec.c";
				sld.Dialog.CurrentNode = "BS_NPVK_3";
				sld.sex = "woman";
				SetQuestHeader("BSPrologue");
				AddQuestRecord("BSPrologue", "1");

				for (i = 1; i < 4; i++)
				{
					sld = GetCharacter(NPC_GenerateMusketeer("gatri_grunt"+i, GetPirateMushketerModel(), "man", 777, PIRATE, -1, false));
					LAi_SetWarriorType(sld);
					LAi_warrior_DialogEnable(sld, false);
					ChangeCharacterAddressGroup(sld, "Pirates_store", "goto", "goto"+i);
					LAi_SetImmortal(sld, true);
					LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
				}
			}
		break;
		//Black sails

		case "second time":
			if (LAi_group_GetPlayerAlarm() > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar,
					LinkRandPhrase("The alarm has been raised in the city, they are looking for you everywhere! If I were you, I wouldn't stay here too long.", "The entire city guard is scouring the city in search of you. I'm not an idiot and I won't talk to you!", "Run before the soldiers make a sieve out of you, "+ GetSexPhrase("buddy", "girlfriend") +"..."),
					LinkRandPhrase("What do you need, scoundrel?! The city guards have already are on your trail, you can't get far, dirty pirate!", "Dirty murderer, get out of my house! Guards!!", "I'm not afraid of you, scoundrel! You'll be hanged in our fort soon, you won't get far..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Heh, anxiety is not a problem for me...", "They will never catch me."),
					RandPhraseSimple("Shut your mouth, " + GetWorkTypeOfMan(npchar, "") + ", or I'll rip out your filthy tongue!", "Heh, " + GetWorkTypeOfMan(npchar, "") + ", and still there - catch pirates! I'll tell you what, buddy: stay quiet and you'll live..."));
				link.l1.go = "fight";
				break;
			}
			// должник гильдии: лавочники нации-кредитора до возврата долга встречают ТОЛЬКО разговором о долге
			if (CheckAttribute(pchar, "GuildDebt") && sti(NPChar.nation) == sti(pchar.GuildDebt.nation))
			{
				dialog.text = "(The shopkeeper recognizes you and darkens.) The guild has sent your name to every office, " + GetAddress_Form(NPChar) + ". A pocketed cargo, a forfeit... Until you settle the debt, we have nothing to talk about. Pay - or be gone.";
				link.l1 = "I've come about the debt. I want to settle it.";
				link.l1.go = "GuildDebt_Settle";
				link.l2 = "I need to gather the money. I'll be back.";
				link.l2.go = "exit";
				break;
			}

			if (npchar.id != "Pirates_trader" && npchar.id != "Caiman_trader" && CheckAttribute(pchar, "BSStart"))
			{
				//Black sails
				if(pchar.questTemp.BlueBird == "over")
				{
					dialog.text = "Captain, nothing personal, but a very influential person has forbidden me to do business with you. No deals, sorry.";
					link.l1 = "I'm sorry, but I don't understand. What happened?";
					link.l1.go = "BS_NPVK_2";
					break;
				}
			}

   			// проверка награды
			dialog.text = NPCharRepPhrase(npchar,
			                 pcharrepphrase(LinkRandPhrase("Captain ", "Yes it is "+ GetSexPhrase("myself", "myself") +" ", "Who do I see? It's the same ") +GetFullName(pchar)+"! "
							                 + LinkRandPhrase("Have you been strung up on the yard yet? Haha!", "They say you were visiting the sea devil!", "And I heard you were hanged in Port Royal.")
											  + RandPhraseSimple("Glad to see you!", "Welcome!"),
							                LinkRandPhrase("Hello, ", "Welcome, ", "Good afternoon, ") + "captain"
											+ RandPhraseSimple(". You liked", ". Did you like")+" our"+ RandPhraseSimple(" island? ", " town? ")
											+ LinkRandPhrase("It's not bad here, ", "Great place, ", ""+ GetSexPhrase("Beautiful girls here, ", "We have hospitable people, ") +"")+RandPhraseSimple("isn't it?", "isn't it?")),
							 pcharrepphrase(LinkRandPhrase("Captain", "Oh, it's you again, ", "You scared me, ") + GetFullName(pchar)
							 				+ LinkRandPhrase(". Mothers are already scaring naughty children in your name!", ". I hope you don't have any friction with the law?", ". Your reputation leaves much to be desired.")
											 +LinkRandPhrase("To what do I owe for your visit?", "What do you want?", "Why did you come?"),
							                "Welcome, captain " +GetFullName(pchar)+ LinkRandPhrase("! It's a little stormy today, ",
											"! It looks like a storm is rising, ", "! A strong wind is rising, ") +"isn't it?"));

			link.l1 = NPCharRepPhrase(npchar,
			            pcharrepphrase(LinkRandPhrase("You're still alive, ", "And I'm for you "+ GetSexPhrase("glad", "glad") +", ", "You and I have known better times, ")
									  +LinkRandPhrase("scoundrel!", "buddy!", "sea wolf!") +"And I"+ GetSexPhrase("thought", "thought") +", "
									  +LinkRandPhrase("your wife has already eaten you with ale!", "you were devoured by termites!", "your old woman burned you out of the world!"),
						               LinkRandPhrase("Good afternoon, ", "Good health, ", "Hello, ") + GetAddress_FormToNPC(NPChar)+ " " + GetFullName(Npchar)
									   +RandPhraseSimple(". You're right, ", ". I obviously agree, ")
									   +LinkRandPhrase("and the locals "+ GetSexPhrase("mulattoes are just adorable", "residents are very friendly") +"", "it's hard to argue with that", "and"+ GetSexPhrase(" the ladies here will turn anyone's head", " the townspeople are really very kind") +"")+ "."),
						pcharrepphrase(RandPhraseSimple("To hell with such conversations, ", "Turn on the other tack, ") +GetFullName(Npchar)+ "! "
						              +LinkRandPhrase("You talk like a parrot!", "And don't mess with my head!", "Your words are no more expensive than a biscuit!")
						              +RandPhraseSimple("I "+ GetSexPhrase("came", "came") +"to the store!", "Why are you sitting here?"),
						               "Good afternoon, " + GetAddress_FormToNPC(NPChar)+ "! Yes, but yesterday " + LinkRandPhrase("it was completely calm.",
									    "the weather was great.", "there was not a breeze.")));
			link.l1.go = "node_1";
			NextDiag.TempNode = "Second time";
			// Gregg: «Чёрная Орхидея», глава 5 - лавочник Портобелло помнит компаньона Торангеры
			if (npchar.city == "PortoBello" && CheckAttribute(pchar, "DestinyGift"))
			{
				if (pchar.DestinyGift == "23")
				{
					link.l2 = "Tell me, have you ever heard of Bernaldinio Toranger, captain of the Vulture?";
					link.l2.go = "BO_Estikos_1";
				}
			}
		break;

		case "node_1":
			dialog.text = NPCStringReactionRepeat(NPCharRepPhrase(npchar,
			                                         pcharrepphrase(RandPhraseSimple("All hands up! ", "Hold for inspection! ") +LinkRandPhrase("What you need "+ GetSexPhrase("gentleman", "gentleman, oh, sorry, lady") +"good luck in my store, hehe?",
													                                 "What a pleasant surprise for the poor old merchant! Hehe!", "What kind of tailwind brought you to my store?"),
            														LinkRandPhrase("Open ports!", "Hooks to the side!", "Keep straight ahead!")+RandPhraseSimple("You will find it here", "In my store")
																	+LinkRandPhrase("the best prices on the island.", "excellent selection of goods.", " goods of the highest quality.")
																	+RandPhraseSimple("I am at your service", "Whatever you wish")+", captain!"),
													 pcharrepphrase(RandPhraseSimple("Any goods for "+ GetSexPhrase("gentlemen", "gentlemen and ladies") +"good luck! Hehe! Until they were hung up to dry in the sun!",
													                                 "The wind shakes the hanged in the port and carries the clang of chains. But piastres don't smell, right?!"),
                 												"Wish " +LinkRandPhrase("buy the best goods " + NationNameGenitive(sti(NPChar.nation)), "buy or sell cargo", "get a profitable freight")
																	+RandPhraseSimple("? I am completely at your disposal!", "? My store is at your service!")
																	+RandPhraseSimple(" Glad to see you", " Make yourself comfortable") +", captain!")),
				                                  NPCharRepPhrase(npchar,
												  	 pcharrepphrase("My advice to you, Cap: don't spare the money for grapeshot. It's easier to search the dead!",
													                "I saw at first glance that you were"+ GetSexPhrase("clever fellow", "clever little one") +". " +RandPhraseSimple("But now I see that you're straight"+ GetSexPhrase("hero", "daredevil") +".", "You"+ GetSexPhrase("young", "young") +"but "
																	+RandPhraseSimple("you have a sharp mind.", "you can't be fooled."))),
													 pcharrepphrase("I am ready to buy from you all the cargo from the captured prizes. Do you want to buy gold or silver? It's always in the price!",
													                "Merchant shipping is a profitable business, Captain " + GetFullName(pchar)+"isn't that right?")),
												  NPCharRepPhrase(npchar,
												     RandPhraseSimple("All seventy-five did not return home! They were drowned in the depths of the sea! " ,
													   RandPhraseSimple("Fifteen people on a dead man's chest!", "Drink, and the devil will bring you to the end!") +" Yo-ho-ho! And a bottle of rum. ")
													   +RandPhraseSimple("What do you want, Cap?", "What do you want to buy, Captain?"),
													 pcharrepphrase("And you like to bargain, Captain " + GetFullName(pchar)+". As a regular customer, I offer you very favorable prices!",
													                "All right, Captain " + GetFullName(pchar)+", out of respect for you, I am ready to give up a little more in price! What do you want?")),
												  NPCharRepPhrase(npchar,
												     "I swear, I'm sick of talking to you. "
													 +LinkRandPhrase("Buy two or three groundnuts. Pigs love them so much.", "Do you expect your boots to be cleaned with rum here?",
													 pcharrepphrase("I know your brother. If you drink too much rum, you'll be hanged.", "How did your mother let you go to sea! In the sea!!! It would be better if you "+ GetSexPhrase("became a tailor", "became a seamstress") +"...")),
													 pcharrepphrase(RandPhraseSimple("I believed, ", "I hoped, ")+RandPhraseSimple("that you left our island.", "that I would never see you again.")
													 +RandPhraseSimple("The dead are hanging around your neck like millstones...", "How many sailors have you sent to rot among the corals?!"),
													 "Captain " + GetFullName(pchar)+"Your passion for commerce exceeds all my expectations!"
													 +RandPhraseSimple("Do you want to make last minute purchases before sailing?", "Do you want to buy something special?"))),
										"cycle", 10, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Come on, show me what you've got, you old pirate!",
													               "I just want to look at your goods, my dear."),
													pcharrepphrase("I swear by the gallows, " +GetFullName(npchar)+"I won't let you lead me by the nose! Show your goods now!",
													               "Let's look at your goods, " +GetAddress_FormToNPC(NPChar)+ ".")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("That's for sure! The dead don't bite! Well, what do you have in the hold?",
													               "You, my dear, better mind your own business. What goods can you offer me?"),
												    pcharrepphrase("One day you will be my prize, hehe. I'm kidding. Give me your goods.",
													               "Let's discuss trading matters, " +GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+ ".")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("What do I need? I want to buy something!",
													               "Drunkenness will drive you to your grave, " +GetFullName(npchar)+ ". I need to see your goods."),
												    pcharrepphrase("I know your discounts! Two pounds of cocoa for the price of three! Show your goods now",
													               "It's always nice. Let's bargain.")),
											  "Show the goods, less words and more business. I'm in a hurry.",
											  npchar, Dialog.CurrentNode);
			link.l1.go = "market";
			ChangeImport();
			link.l2 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Oh, that would be it "+ GetSexPhrase("cut", "cut") +" someone! Okay, let's get to the point.",
													               "Let's move on to other things, my dear"),
													pcharrepphrase("Carramba! I'm on a completely different case!",
													               "I need to talk to you about business, "+GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+".")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("I have business with you, old pirate!",
													               "Let's get down to business, my dear!"),
													pcharrepphrase("By the devil, you can't fool me, you rascal! I have another case!",
													               "I agree, " +GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+ ", I want to discuss other matters with you.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Are you drunk already, buddy?! I have business with you.",
													               "I "+ GetSexPhrase("I came", "I came") +" don't trade, my dear. I have another case."),
													pcharrepphrase("Damn you with your prices, you scoundrel! I have business with you.",
													               "Discounts are always nice, " + GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+ ", but I'm on another case.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Hold your tongue, you old drunk, I have business with you.",
													               "You're sick of rum, I suppose. However, I'm not thrilled with you either. Let's get down to business."),
													pcharrepphrase("You've already robbed me to the skin, you scammer! I have business with you.",
													               "Your irony is inappropriate, I have a different order of business for you.")),
											   npchar, Dialog.CurrentNode);
			link.l2.go = "business";
			link.l3 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Business first, then fun. I want to ask you something.",
													               "Not now, I need information."),
													pcharrepphrase("To hell with the prices! First, you tell me everything I need to know.",
													               "With pleasure. I want to know something.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Thanks for the advice, old killer. I need to know something.",
													               "I have a specific question."),
												    pcharrepphrase("The gold can wait. I want to know something.",
													               "Yes, I am " + GetSexPhrase("I agree", "I agree") +". I want to ask you something.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("I think you know what I want to ask.",
													               "I hope you will answer my question."),
													pcharrepphrase("Do you have anything in your head besides price tags? That's what I want to ask.",
													               "It's not about that. I need some information.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Don't get all worked up, old man! I just want to ask you.",
													               "Don't worry so much, you'll have a stroke. Answer my question and I'll leave."),
													pcharrepphrase("I don't expect a clever word from you - you have a cockroach mind. But you know something.",
													               "I won't keep you for long. Just one question.")),
											   npchar, Dialog.CurrentNode);
			link.l3.go = "quests";
			//--> eddy. Аскольд, базар с рендомным торговцем
			if (pchar.questTemp.Ascold == "Seek_powder" && pchar.questTemp.Ascold.TraderId == npchar.id && !CheckAttribute(Pchar, "RomanticQuest.TalkInShop"))
			{
				link.l3.go = "AscoldTrader";
			}
			if (pchar.questTemp.Ascold == "PowderWasSeek" && pchar.questTemp.Ascold.TraderId == npchar.id)
			{
				link.l3.go = "AscoldTrader_WasSeek";
			}
			if (CheckAttribute(pchar, "Whisper.Contraband") && npchar.id == "Tortuga_trader" && GetSquadronGoods(Pchar, GOOD_EBONY) >= 100)
			{//Линейка Виспер
				link.l44 = "Hello. I took on the unfinished work of the former captain of my ship. The magazine says I owe you a hundred pieces of ebony. Everything is in my hold and ready for unloading.";
				link.l44.go = "Whisper_contraband";
			}
			if (CheckAttribute(pchar, "cursed.waitingSkull") && pchar.questTemp.Cursed.TraderId == npchar.id && !CheckAttribute(pchar, "cursed.Skullbought"))
			{
				link.l44 = "I have a somewhat unusual question for you. The fact is that I'm looking for one item - "+ GetConvertStr("itmname_"+pchar.questTemp.Cursed.Item, "ItemsDescribe.txt") +". It became known to me that it could belong to the owner of this store.";
				link.l44.go = "buy_skull";
			}
			if(CheckAttribute(pchar, "GenQuest.EncGirl"))
			{
				if(pchar.GenQuest.EncGirl == "toLoverFather" && pchar.GenQuest.EncGirl.LoverFather == "store_keeper" && pchar.GenQuest.EncGirl.LoverCity == npchar.city)
				{
					link.l9 = "Hello, I'm here at your son's invitation.";
					link.l9.go = "EncGirl_4";
					pchar.quest.EncGirl_GetLoverFather.over = "yes";
				}
				if(pchar.GenQuest.EncGirl == "toParents" || pchar.GenQuest.EncGirl == "FindLover")
				{
					if(pchar.GenQuest.EncGirl.Father == "store_keeper" && npchar.city == pchar.GenQuest.EncGirl.city)
					{
						link.l9 = "I'm talking about your daughter...";
						link.l9.go = "EncGirl_1";
					}
				}
			}
			//<-- eddy. Аскольд, базар с рендомным торговцем
			link.l11 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Bye, old drunk.",
													               "Have a nice day, "+GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+"."),
													pcharrepphrase("Carramba! Don't go anywhere. I'll come back.",
													               "Nice to see you, "+GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+"! Goodbye!")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Give up the mooring lines!",
													               "Goodbye, my dear."),
													pcharrepphrase("Hey! What are you hiding there? Rum? No?! Well, I'll go wet my whistle.",
													               "Please excuse me, I have urgent business in the city.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Heh! See you again!",
													               "I have to go. Goodbye, "+GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+"!"),
													pcharrepphrase("And don't you dare contradict me! I don't have time to argue with you here!",
													               "Perhaps it's worth checking out how things are on the ship. Goodbye, "+GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+"!")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("What's the matter with you, old man? It's me - " +GetFullName(pchar)+ "! Sleep it off, I'll come back later.",
													               "I'm afraid your liver will kill you before I come in here again."),
													pcharrepphrase("Count your piastres, you rascal! I'll come back.",
													               ""+ GetSexPhrase("I was glad", "I was glad") +"see you, " + GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+"!")),
											  npchar, Dialog.CurrentNode);
			link.l11.go = "exit";
		break;

		//Линейка Виспер
		case "Whisper_contraband":
			dialog.text = "Keep it down, girl! Is this your first time in the Caribbean?";
			DeleteAttribute(pchar, "Whisper.Contraband");
			link.l1 = "Yes, is there a problem?";
			link.l1.go = "Whisper_contraband_1";
			link.l2 = "No...";
			link.l2.go = "Whisper_contraband_1";
		break;
		case "Whisper_contraband_1":
			dialog.text = "Actually, trading in ebony on Tortuga is strictly prohibited. No matter how much I want to, I won't be able to accept the goods from you.";
			link.l1 = "What should I do in this case? Looking for another buyer?";
			link.l1.go = "Whisper_contraband_2";
		break;
		case "Whisper_contraband_2":
			dialog.text = "Don't jump to conclusions so quickly. Of course, we cannot unload the goods directly at the port, in front of the guards. However, this formality can be circumvented. You can sail a little from the colony itself and dock at the beach, which is called the Eye of the Fish. It is located in the northern part of the island. My friends will meet you there and make the payment with unloading.";
			link.l1 = "What if some patrol notices our deal?";
			link.l1.go = "Whisper_contraband_3";
		break;
		case "Whisper_contraband_3":
			dialog.text = "In this case, you are on your own. If you see them, you'd better run. But don't worry unnecessarily, the guards don't look into the place I named.";
			pchar.Whisper.ContraSmuggler = true;
			WhisperPlaceSmugglersOnShore("Shore58");
			AddQuestRecord("WhisperContraband", "2");
			SetTimerCondition("W_Smuggling", 0, 0, 10, false);
			link.l1 = "Okay. Then I'll sail to the bay.";
			link.l1.go = "exit";
		break;
		//Линейка Виспер
		case "buy_skull":
			dialog.text = GetConvertStr("itmname_"+pchar.questTemp.Cursed.Item, "ItemsDescribe.txt") +"? I'm sorry, but this is a family heirloom, it's not for sale. Many years ago, my grandfather bought it from a pirate, and since then the skull has never left the walls of this store.";
			link.l1 = "Don't lie. For you merchants, everything has a price, even your own mother. How much do you want for this skull?";
			link.l1.go = "buy_skull_1";
		break;

		case "buy_skull_1":
			dialog.text = sti(skullprice) + " and not a coin less.";
			if (sti(pchar.Money) >= skullprice)
            {
    			link.l1 = "Here's the money. Give it to me.";
				link.l1.go = "buy_skull_2";
			}
			link.l2 = "Yes, for this money I will buy a dozen of these skulls. Goodbye.";
			link.l2.go = "exit";
		break;

		case "buy_skull_2":
			pchar.cursed.Skullbought = true;
			AddQuestRecord("CursedSkeleton", "2");
			dialog.text = "Nice to do business with you.";
			AddMoneyToCharacter(pchar, -skullprice);
			GiveItem2Character(pchar, pchar.questTemp.Cursed.Item);
    		link.l1 = "Yeah, goodbye.";
			link.l1.go = "exit";
		break;

		// Gregg: «Чёрная Орхидея», глава 5 - Джекмен советовал расспросить испанских лавочников
		// о Бернальдинио Торангере; в Портобелло след ведёт к его компаньону Фернандо Эстикосу.
		case "BO_Estikos_1":
			dialog.text = "Toranger? Aye, I remember him. A Spaniard, sailed the Vulture, carried cargo through all our waters. A lucky man - the pirates never once took him, though they hunted him. And then he up and vanished.";
			link.l1 = "Vanished? How do you mean, vanished?";
			link.l1.go = "BO_Estikos_2";
		break;

		case "BO_Estikos_2":
			dialog.text = "The way men vanish at sea, " + GetAddress_Form(NPChar) + ". He left port and never came back. Not he, nor the ship, nor the cargo I was never paid for. I hardly knew him myself - I did all my business through his partner.";
			link.l1 = "He had a partner? Is he alive?";
			link.l1.go = "BO_Estikos_3";
		break;

		case "BO_Estikos_3":
			dialog.text = "Alive, if you can call it living. Fernando Estikos. When Bernaldinio was lost he took to the bottle, and since then he never leaves our tavern. Drinking away his last coin. If you want to know about the Vulture, go there - nobody else will tell you a thing.";
			link.l1 = "Thank you, " + GetAddress_FormToNPC(NPChar) + " " + GetFullName(npchar) + ". I'll look into the tavern.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftToPortoBelloTavern");
		break;



		case "market":
//navy -->
			//занят ПГГ
//navy <--
			dialog.text = RandPhraseSimple("I have sabers, pistols, cannonballs, valuable wood and other goods! What are you interested in?", "Do you want to buy gold or silver? Or maybe look for a great blade?");
			link.l1 = pcharrepphrase(LinkRandPhrase("My hold is full of loot! Piastres don't smell, do they?",
			                         "I need to empty my hold of loot and stuff it with your gold. Ha ha!",
									 "You'll sell your own mother for full-fledged doubloons! But I just need to restock."),
									 LinkRandPhrase("I want to sell the cargo and replenish stocks.",
			                         "I want to buy an item for sale.",
									 "Show me the balls, bombs and other goods."));
			link.l1.go = "trade_1";
			link.l2 = LinkRandPhrase("Show me the sabers, pistols, and what else do you have there?",
			                         "I could use a couple of new pistols, bullets and a beautiful ring.",
									 "Precious stones, outlandish idols - that's what interests me.");
			link.l2.go = "items";

			link.l3 = pcharrepphrase(RandPhraseSimple("I'm leaving without trading. Come on!",
			                                          "I'm going to wet my whistle. Don't go anywhere."),
									 RandPhraseSimple("No, I'm not in the mood for shopping right now. Goodbye.",
									                  "Excuse me, I have an urgent matter. Goodbye!"));
			link.l3.go = "exit";
		break;

		case "WasteTime":
			environment.time = 16.0;
			WasteTime(1);
			DialogExit();
		break;

		case "EncGirl_1":
			dialog.text = "I'm listening to you carefully.";
			link.l1 = "I brought your fugitive.";
			link.l1.go = "EncGirl_2";
		break;

		case "EncGirl_2":
			dialog.text = "Ah, Captain, thank you very much! How is she? Are you healthy, are you not injured? It's been a day since she ran away, the wretch. Do I wish her any harm? I'm going to get married... The groom is rich, young, and what is not good-looking, so do not drink water from the face. No! She tries everything in her own way-just like her mother, damn her...Yes, and she would not have been born if her mother had not run away with an enterprising blockhead at the time... Well, what to remember... Youth is naive, stupid... and cruel.";
			link.l1 = "Of course, you are the father and it's up to you, but I wouldn't rush the happy wedding...";
			link.l1.go = "EncGirl_3";
		break;

		case "EncGirl_3":
			dialog.text = "What do you understand? Do you have children of your own? Not there? So, when they are, come in, then we'll talk... yes... I promised a reward to the one who returns it - please accept it.";
			link.l1 = "Thank you. And hold her tight. Something tells me she's not going to stop there.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("EncGirl_ToLoverParentsExit");
		break;

		case "EncGirl_4":
			if(sti(pchar.GenQuest.EncGirl.LoverFatherAngry) == 0)
			{
				dialog.text = "Ah, so you are that "+ GetSexPhrase("captain who brought", "girl who brought") +" my prodigal son with a young bride?";
				link.l1 = "Yes, I helped them escape.";
				link.l1.go = "EncGirl_5";
			}
			else
			{
				dialog.text = "Ah, revealed"+ GetSexPhrase("you, benefactor", "you, benefactress") +". Are you expecting a reward?";
				link.l1 = "Well, not that there are awards, but it would be nice to hear gratitude.";
				link.l1.go = "EncGirl_6";
			}
		break;

		case "EncGirl_5":
			dialog.text = "I am very grateful to you for not leaving my child in a difficult situation and helping him to get out of such a delicate situation with honor. Let me thank you as much as I can. Accept this modest sum and a gift from me personally.";
			link.l1 = "Thank you, it was a pleasure to help these young people.";
			link.l1.go = "EncGirl_5_1";
		break;

		case "EncGirl_6":
			dialog.text = "What thanks?! What is the gratitude for?! Not only has this blockhead been hanging around without work for six months, he's also found time to be nice! Yes, I was already running my own business at his age. And look, he's gone hunting! The governor has a marriageable daughter, and this one has brought some kind of rootless slut, God forgive me, and is asking for parental blessing!";
			link.l1 = "Hmm, so you don't believe in sincere feelings?";
			link.l1.go = "EncGirl_6_1";
		break;

		case "EncGirl_5_1":
			AddMoneyToCharacter(pchar, sti(pchar.GenQuest.EncGirl.sum));
			GiveItem2Character(pchar, pchar.GenQuest.EncGirl.item);
			AddQuestRecord("JungleGirl", "18");
			CloseQuestHeader("JungleGirl");
			DeleteAttribute(pchar, "GenQuest.EncGirl");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "EncGirl_6_1":
			dialog.text = "What kind of feelings are these?! What kind of feelings are you talking about? An adult, like a person, but there... Feelings! It should be a shame for you young people to indulge in such matters and to contract as a pimp. The girl was torn out of her parents' nest and my sucker's life was ruined. There will be no gratitude to you. Goodbye.";
			link.l1 = "Well, you don't get sick either...";
			link.l1.go = "EncGirl_6_2";
		break;

		case "EncGirl_6_2":
			ChangeCharacterReputation(pchar, -1);
			AddQuestRecord("JungleGirl", "19");
			AddQuestUserData("JungleGirl", "sSex", GetSexPhrase("ate", "la"));
			AddQuestUserData("JungleGirl", "sSex1", GetSexPhrase("", "a"));
			CloseQuestHeader("JungleGirl");
			DeleteAttribute(pchar, "GenQuest.EncGirl");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "trade_1":
			ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea) || (Pchar.location == "Caiman_Store");
		    if (sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok)
			{
			    NextDiag.CurrentNode = NextDiag.TempNode;
			    DialogExit();
			    Pchar.PriceList.StoreManIdx = rColony.index; // boal 27.02.05
			    LaunchStore(sti(rColony.StoreNum));
			}
			else
			{
			    dialog.text = NPCharRepPhrase(npchar, pcharrepphrase("Are you kidding me? And where is your ship? Pierce doesn't have it!",
				                                                     "By the devil, you can't fool me! The pier doesn't have your ship!"),
													  pcharrepphrase("I don't see your ship in port, captain " +GetFullName(pchar)+ ". I hope it's not the Flying Dutchman?",
													                 "Captain, it's more convenient to ship the goods from the pier. Moor the ship and come."));
				link.l1 = NPCharRepPhrase(npchar, pcharrepphrase("" +RandSwear()+"What a nuisance!!! Okay, old weasel, I'll see you later!",
				                                                 "I didn't want to deceive you, " +GetFullName(npchar)+ ", the ship is on the other side of the island."),
												  pcharrepphrase("No, my ship is Black Pearl. Don't be scared! I'm joking!",
												                 "Thanks for the advice, I will definitely use it."));
			    link.l1.go = "exit";//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		break;

		case "storage_0":
			NPChar.MoneyForStorage = GetNpcQuestPastMonthParam(NPChar, "Storage.Date") * sti(NPChar.Storage.MoneyForStorage);
			if(sti(NPChar.MoneyForStorage) > 0)
			{
				dialog.text = "I'll charge you for the rent again " + FindRussianMoneyString(sti(NPChar.MoneyForStorage)) + ".";
				if(sti(pchar.money) >= sti(NPChar.MoneyForStorage))
				{
					link.l1 = "Okay, I'll pay the rent now.";
					link.l1.go = "storage_3";
				}
				else
				{
					link.l1 = "I'll come back later.";
					link.l1.go = "exit";
				}
			}
			else
			{
				dialog.text = "Come on in.";
				link.l1 = "Thank you.";
				link.l1.go = "storage_2";
			}
			link.l2 = "No, I changed my mind.";
			link.l2.go = "exit";
		break;

		case "storage_01":
			dialog.text = "No, "+ GetSexPhrase("dear", "beauty") +", it's waiting for you. I knew you wouldn't leave this offer.";
			link.l1 = "That's good. I'm renting it.";
			link.l1.go = "storage_1";
			link.l2 = "No, I just remembered. Maybe I'll ever need...";
			link.l2.go = "exit";
		break;

		case "storage_1":
			NPChar.MoneyForStorage = GetStoragePriceExt(NPChar, pchar);
			dialog.text = "As you remember - payment for a month in advance.";
			if(sti(pchar.money) >= sti(NPChar.MoneyForStorage))
			{
				link.l1 = "I remember - keep it.";
				link.l1.go = "storage_11";
			}
			else
			{
				link.l1 = "Damn it. I'll get the money now.";
				link.l1.go = "exit";
			}
		break;

		case "storage_11":
			AddMoneyToCharacter(pchar, -makeint(NPChar.MoneyForStorage));
			NPChar.Storage.MoneyForStorage = NPChar.MoneyForStorage;
			NPChar.Storage.Activate = true;
			stores[sti(rColony.StoreNum)].Storage.Activate = true; // mirror rent flag onto store object - so store interface shows warehouse in the ship scroll
			SaveCurrentNpcQuestDateParam(NPChar, "Storage.Date");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchStorage(sti(rColony.StoreNum));
		break;

		case "storage_2":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchStorage(sti(rColony.StoreNum));
		break;

		case "storage_3":
			AddMoneyToCharacter(pchar, -sti(NPChar.MoneyForStorage));
			NPChar.MoneyForStorage = GetStoragePriceExt(NPChar, pchar);
			NPChar.Storage.MoneyForStorage = NPChar.MoneyForStorage;
			SaveCurrentNpcQuestDateParam(NPChar, "Storage.Date");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchStorage(sti(rColony.StoreNum));
		break;

		case "storage_04":
			dialog.text = "Are you really freeing me? Ai-yai-yai, such a great place, such favorable conditions. I assure you, you will not be offered similar services anywhere else in the Caribbean.";
			link.l1 = "I told you - I release you. Or are you suggesting that I pay for air storage? You can look for another tenant.";
			link.l1.go = "storage_4";
			link.l2 = "They won't offer it anywhere, you say? Okay, I'll keep it to myself for now. But the rental price is due to say... rather big, in a word.";
			link.l2.go = "exit";
		break;

		case "storage_4":
			NPChar.MoneyForStorage = GetNpcQuestPastMonthParam(NPChar, "Storage.Date") * sti(NPChar.Storage.MoneyForStorage);
			if(sti(NPChar.MoneyForStorage) > 0)
			{
				dialog.text = "I'll charge you for the rent again " + FindRussianMoneyString(sti(NPChar.MoneyForStorage)) + ".";
				if(sti(pchar.money) >= sti(NPChar.MoneyForStorage))
				{
					link.l1 = "Okay.";
					link.l1.go = "storage_5";
				}
			}
			else
			{
				dialog.text = "Pick up your goods and I will close the warehouse.";
				link.l1 = "Okay.";
				link.l1.go = "storage_6";
			}
		break;

		case "storage_5":
			SetStorageGoodsToShip(&stores[sti(rColony.StoreNum)]);
			AddMoneyToCharacter(pchar, -sti(NPChar.MoneyForStorage));
			NPChar.Storage.NoActivate = true;
			DeleteAttribute(NPChar, "Storage.Activate");
			DeleteAttribute(&stores[sti(rColony.StoreNum)], "Storage.Activate"); // remove the mirror from the store object
			DialogExit();
		break;

		case "storage_6":
			SetStorageGoodsToShip(&stores[sti(rColony.StoreNum)]);
			DeleteAttribute(NPChar, "Storage.Activate");
			DeleteAttribute(&stores[sti(rColony.StoreNum)], "Storage.Activate"); // remove the mirror from the store object
			NPChar.Storage.NoActivate = true;
			DialogExit();
		break;

		case "items":
			if (npchar.quest.item_date != lastspeak_date)
			{
				GiveItemToTrader(npchar);
				npchar.quest.item_date = lastspeak_date;
			}
			Nextdiag.CurrentNode = Nextdiag.TempNode;
			DialogExit();
			LaunchItemsTrade(NPChar);
		break;

		case "business":
			iTest = 0;
    		//квест Синей Птицы, начальный диалог
			int iBlueBirdRank = 1;
			if (!bQuestsRank) iBlueBirdRank = 5; 	//Если стоит галочка, то убрать требования рангов у квестов
			if (pchar.questTemp.BlueBird == "begin" && sti(npchar.nation) != PIRATE && npchar.city != "Panama" && sti(pchar.rank) >= iBlueBirdRank)
			{
				dialog.text = RandPhraseSimple("Captain, please help us!!!", "Captain, I ask for your help on behalf of all merchants!");
				link.l1 = "What happened? How can I help you?";
				link.l1.go = "RBlueBird";
				break;
			}
//navy -->
			//занят ПГГ
			if(CheckAttribute(PChar, "CargoQuest.TraderID") && CheckAttribute(PChar, "CargoQuest.GiveTraderID")) bTradeQuest = pchar.CargoQuest.TraderID == NPChar.id || pchar.CargoQuest.GiveTraderID == NPChar.id;
			iTmp = CheckAvailableTaskForNPC(NPChar, PGG_TASK_WORKONSTORE);
			if (iTmp != -1 && !bTradeQuest)
			{
				dialog.text = "I had a job, but " + GetFullName(&Characters[iTmp]) + " has already undertaken to do it for me.";
				link.l1 = "What are you saying! Okay, I'll come back later.";
				link.l1.go = "exit";
				break;
			}
//navy <--
			dialog.text = NPCharRepPhrase(npchar, "What's up?! Tell me everything in order!", "I'm listening to you. What kind of case are we talking about?");
			// долг перед торговой гильдией: уладить можно в любом магазине нации торговца
			if (CheckAttribute(pchar, "GuildDebt") && sti(NPChar.nation) == sti(pchar.GuildDebt.nation))
			{
				link.lGuildDebt = "I've come about my debt to the merchant guild. I want to settle it.";
				link.lGuildDebt.go = "GuildDebt_Settle";
			}
			// loophole: bribe the guild clerk (before the deadline) - at the CONSIGNEE (the one we deliver to): goods and money off the books.
			// Storm && is not lazy - access CargoQuest.TraderID only inside the Freight_HasActive guard
			if (Freight_HasActive())
			{
				// convoy (type 7) excluded: serious state cargo, the clerk can't be bribed - only delivered to the commandant
				// a freight-blocked store (NoContraCity) offers no clerk bribe: the trader is sore at the player and waiting for the forfeit - shady deals with him here make no sense.
				// LEO: governor's escort also excluded - cargo is on the state convoy ships: bribing the clerk gains nothing (convoy leaves with the cargo, money wasted).
				if (pchar.CargoQuest.TraderID == npchar.id && sti(pchar.CargoQuest.iOrderType) != 7 && !CheckAttribute(pchar, "CargoQuest.GovEscortActive") && CheckQuestAttribute("generate_trade_quest_progress", "begin") && !CheckAttribute(pchar, "NoContraCity." + npchar.city))
				{
					link.lClerk = "(under my breath) And what if this manifest... were to go missing? The cargo stays with me, nothing in the books. I'll pay well.";
					link.lClerk.go = "FreightClerk_Offer";
				}
			}
            ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea) || (Pchar.location.from_sea == "Shore17");
            if (sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok)
			{
    			if (CheckQuestAttribute("generate_trade_quest_progress", "begin") || CheckQuestAttribute("generate_trade_quest_progress",  "failed"))
    			{
    				if (pchar.CargoQuest.TraderID == npchar.id )//&&  GetSquadronGoods(pchar, iQuestTradeGoods) >= iQuantityShipGoods)
    				{
    					if (sti(pchar.CargoQuest.iOrderType) == 5) // CARGO_SMUGGLE - a legit shopkeeper won't take contraband, need the smuggler agent
    					{
    						link.l2 = "I've got... that special cargo. Where do you want it?";
    						link.l2.go = "smuggle_wrong_trader";
    					}
    					else if (sti(pchar.CargoQuest.iOrderType) == 6) // CARGO_GOVERNOR - state cargo, delivered to the residence
    					{
    						link.l2 = "I'm carrying state cargo. Where do you want it?";
    						link.l2.go = "governor_wrong_trader";
    					}
    					else if (sti(pchar.CargoQuest.iOrderType) == 7) // CARGO_FORTAMMO - ammunition, delivered to the fort commandant
    					{
    						link.l2 = "I'm carrying a load of ammunition. Where do you want it?";
    						link.l2.go = "fortammo_wrong_trader";
    					}
    					else
    					{
    						link.l2 = RandPhraseSimple("There is cargo in the hold of my ship for your store.", "My ship is chartered to deliver cargo to your store. The goods are already being unloaded.");
    						link.l2.go = "generate_quest_2";
    					}
    				}
    				else
    				{
                        if (CheckQuestAttribute("generate_trade_quest_progress", "failed") && pchar.CargoQuest.GiveTraderID == npchar.id)
                        {
                            // LEO: failed freight - settle it with the employer who issued it. The generate_quest_failed node
                            // used to be orphaned (never routed to), so the player got stuck in the dead-end not_closed. Now we route to the failure settlement.
                            link.l2 = "I know I missed the freight deadline. How can I make amends?";
            				link.l2.go = "generate_quest_failed";
                        }
                        else if (!CheckQuestAttribute("generate_trade_quest_progress",  "failed") && pchar.CargoQuest.GiveTraderID == npchar.id && GetNationRelation2MainCharacter(sti(characters[GetCharacterIndex(pchar.CargoQuest.TraderID)].nation)) == RELATION_ENEMY)
                        {
                            link.l2 = "Unfortunately, I have to renounce the commitments made. The political situation does not allow me to deliver the cargo.";
            				link.l2.go = "generate_quest_cannot_done";
                        }
                        else
                        {
                            if (rand(1) == 1)
                            {
                                link.l2 = RandPhraseSimple("I want to offer you to charter my ship for cargo transportation.",
								                           "I have a beautiful ship, and I can deliver any cargo wherever you want.");
            				    link.l2.go = "generate_quest_not_closed";
        				    }
    				    }
    				}
    			}
    			else
    			{
    				link.l2 = "Don't want to charter a ship, " +GetAddress_FormToNPC(NPChar)+ "? I have a reliable ship and experienced sailors.";
    				link.l2.go = "generate_quest";
    			}
    			// --> на кредитный генератор
    			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
    			{
	                link.l3 = pcharrepphrase(RandPhraseSimple("Eh, exhausted in the sea... Here's the thing, buddy, the money issue is acute.",
	                                                          "Well, old rascal, let's discuss our money matters."),
	                                        RandPhraseSimple("I wanted to discuss financial issues with you.",
	                                                         "Let's discuss financial issues, we have something to talk about."));,

	                link.l3.go = "LoanForAll";
                }
				//квест мэра - на связь с нашим шпионом
  				if (CheckAttribute(pchar, "GenQuest.Intelligence.SpyId") && pchar.GenQuest.Intelligence.SpyId == npchar.id && pchar.GenQuest.Intelligence == "")
				{
					link.l4 = RandPhraseSimple("I'm here on behalf of a man. His name is the governor " + GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + ".",
						GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + " sent me to pick up something...");
					link.l4.go = "IntelligenceForAll";
				}
				// ----------------- квест получения Синей Птицы, сдаём квест -----------------
				if (pchar.questTemp.BlueBird == "weWon" && pchar.questTemp.BlueBird.traiderId == npchar.id)
				{
					link.l1 = "I want to inform you that I managed to complete your assignment. The xebec 'Blue Bird' will no longer annoy the merchants. I have arrived for the agreed fee.";
					link.l1.go = "RBlueBirdWon";
				}
				if (pchar.questTemp.BlueBird == "DieHard" && pchar.questTemp.BlueBird.traiderId == npchar.id)
				{
					link.l1 = "I must inform you that I failed to sink the xebec 'Blue Bird'. I did everything I could...";
					link.l1.go = "RBlueBirdDieHard";
				}
				if (pchar.questTemp.BlueBird == "returnMoney" && pchar.questTemp.BlueBird.traiderId == npchar.id && sti(pchar.questTemp.BlueBird.count) > 0)
				{
					if (sti(pchar.questTemp.BlueBird.count) < 5)
					{
						link.l1 = "How are your colleagues' own flutes doing? Is everything okay?";
						link.l1.go = "RBlueBird_retMoney_1";
					}
					else
					{
						link.l1 = "Have you come to your senses yet, do you want to give me my honestly earned money?";
						link.l1.go = "RBlueBird_retMoney_3";
					}
				}
				// ----------------- квест получения Синей Птицы, продолжение -----------------
				//homo 25/06/06 слухи
                link.l6 = pcharrepphrase(RandPhraseSimple("Will you tell me the latest gossip? You're stuck here all day anyway.",
                                                          "Don't fuss, you old rascal, but rather tell me how life flows on land?"),
                                        RandPhraseSimple("It seems that I've been in the sea for ages. What's new in your area?",
                                                         "You probably know all the latest news? What's important happened?"));,

    		    link.l6.go = "rumours_trader";

				// ugeen --> склад товаров для ГГ
				if(NPChar.city != "Pirates")
				{
					if(!CheckAttribute(NPChar, "Storage.NoActivate"))
					{
						link.l7 = "Is it possible to rent a warehouse from you?";
						link.l7.go = "storage_rent";
					}
					if(CheckAttribute(NPChar, "Storage.NoActivate") && !CheckAttribute(NPChar, "Storage.Activate"))
					{
						link.l7 = "I would like to rent a warehouse from you again.";
						link.l7.go = "storage_rent";
					}
				}
				if(CheckAttribute(NPChar, "Storage"))
				{
					if(CheckAttribute(NPChar, "Storage.Activate"))
					{
						stores[sti(rColony.StoreNum)].Storage.Activate = true; // re-sync rent mirror onto store object (existing saves: show warehouse in store scroll)
						link.l7 = "Take me to the warehouse. I want to take a look at the condition of it.";
						link.l7.go = "storage_0";
						link.l8 = "I have decided to vacate the warehouse. I don't need him anymore.";
						link.l8.go = "storage_04";
					}
					else
					{
						if(!CheckAttribute(NPChar, "Storage.NoActivate") && CheckAttribute(pchar, "questTemp.BlueBird.speakWon"))
						{
							link.l7 = "I remember you mentioned the warehouse once. Is the seat still occupied?";
							link.l7.go = "storage_01";
						}
					}
				}
				// <-- ugeen
				// <-- homo
				if (CheckOfficersPerk(pchar, "Trader"))
				{
					link.l8 = "Colleague, would you share information about the prices you know?";
					link.l8.go = "GetTradeInfo";
				}
				link.l99 = "However, now is not the time to talk.";
    			link.l99.go = "no_quests";
			}
			else
			{
                link.l1 = NPCharRepPhrase(npchar, pcharrepphrase("" +RandSwear()+"I left my ship on the other side of the island. I'll come back later!", "Excuse me, " +GetFullName(npchar)+ ", I'll come back later. My ship hasn't reached the pier yet."), pcharrepphrase("The devil! These rotters didn't put the ship under loading! I'll come back later!", "Sorry, I owe to order to put your ship to the pier!"));
                link.l1.go = "exit";
    			// --> на кредитный генератор
    			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
    			{
	                link.l3 = pcharrepphrase(RandPhraseSimple("Eh, exhausted in the sea... Here's the thing, buddy, the money issue is acute.",
	                                                          "Well, old rascal, let's discuss our money matters."),
	                                        RandPhraseSimple("I wanted to discuss financial issues with you.",
	                                                         "Let's discuss financial issues, we have something to talk about."));,

	                link.l3.go = "LoanForAll";
                }
				//квест мэра - на связь с нашим шпионом
  				if (CheckAttribute(pchar, "GenQuest.Intelligence.SpyId") && pchar.GenQuest.Intelligence.SpyId == npchar.id && pchar.GenQuest.Intelligence == "")
				{
					link.l7 = RandPhraseSimple("I'm here on behalf of a man. His name is the governor " + GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + ".",
						GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + " sent me to pick up something...");
					link.l7.go = "IntelligenceForAll";
				}
			}
		break;

		case "generate_quest":
			int slot; string sErr; string aName;
			int otype, g, qty, maxDays, dexp;
			string sToCity;

			if (sti(NPChar.nation) != PIRATE && GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY)
			{
				dialog.text = "No, you don't need to! At least as long as you're feuding with " + NationNameAblative(sti(NPChar.nation)) + ".";
				link.l1 = "In that case, goodbye.";
				link.l1.go = "exit";
				break;
			}
			// refuse if reputation is too low / отказ при плохой репутации
			if (isBadReputation(pchar, 40))
			{
				dialog.text = RandSwear() + "Isn't that right?" + GetFullName(pchar)+ ", About you... " + RandPhraseSimple("There are terrible rumors.", "They don't speak in the best way.") + " I don't want to deal with you.";
				link.l1 = RandPhraseSimple("Eh... once a person stumbles, everyone will spit at him.", RandSwear()+ " Well, there are more interesting things to do at sea, " + GetFullName(npchar)+".");
				link.l1.go = "exit";
				break;
			}

			// === заказы на дневном рандоме (dRand): стабильны сутки и переживают перезагрузку ===
			RecalculateSquadronCargoLoad(pchar);
			sErr = "";
			if (GuildDebt_IsFreightBlocked()) sErr = "guilddebt"; // чёрные списки гильдии: присвоил фрахтовый груз - фрахтов не видать
				if (sErr == "" && CheckAttribute(pchar, "NoContraCity." + npchar.city)) sErr = "contrablock"; // кинул контра-фрахт ЭТОГО магазина - он не даёт НИКАКИХ фрахтов, пока не уладишь через связного
			// LEO: один фрахт из порта в сутки. Список заказов дневной и за сутки не меняется, а закрыть
			//   фрахт можно не доходя до адресата (слить конвой посыльному прямо в море) - без этого заказ
			//   крутился по кругу. Честной игре не мешает: рейс до адресата и обратно дольше суток.
			if (sErr == "" && CheckAttribute(pchar, "FreightLastTake.city"))
			{
				if (pchar.FreightLastTake.city == npchar.city && GetQuestPastDayParam("FreightLastTake") < 1) sErr = "takentoday";
			}

			for (slot = 1; slot <= 3; slot++)
			{
				aName = "iTradeNation" + slot;
				npchar.(aName) = GenerateNationTrade(sti(NPChar.nation));
			}
			if (sErr == "" && sti(npchar.iTradeNation1) < 0 && sti(npchar.iTradeNation2) < 0 && sti(npchar.iTradeNation3) < 0) sErr = "nofreight";

			if (sErr == "")
			{
				for (slot = 1; slot <= 3; slot++)
				{
					aName = "storeMan" + slot;
					npchar.(aName) = findStoreMan(NPChar, slot - 1);
				}
				if (sti(npchar.storeMan1) <= 0 || sti(npchar.storeMan2) <= 0 || sti(npchar.storeMan3) <= 0) sErr = "political";
			}

			if (sErr == "")
			{
				// в пиратских поселениях не выдаём контрабандный/губернаторский фрахт (нет официального губернатора, а контрабанда среди пиратов бессмысленна)
				bool bAllowSpecialFreight = (sti(NPChar.nation) != PIRATE);
				for (slot = 1; slot <= 3; slot++)
				{
					otype = RollCargoOrderType(slot, bAllowSpecialFreight);
					// loophole 2: if the player kept a contraband freight from this city, the brotherhood closed the channel - no more contra orders here (demote to standard)
					if (otype == 5 && CheckAttribute(pchar, "NoContraCity." + npchar.city)) otype = 1;

					aName = "storeMan" + slot;
					sToCity = Characters[sti(npchar.(aName))].city;

					// anti-softlock (all cities one nation / all pirate): governor(6)/commandant(7) to a pirate
					// destination or a fortless town has no one to deliver to (no governor/commandant) - demote to a normal order
					if (otype == 6 || otype == 7)
					{
						int iDestCol = FindColony(sToCity);
						if (iDestCol < 0) otype = 1;
						else
						{
							if (sti(Colonies[iDestCol].nation) == PIRATE) otype = 1;
							else if (otype == 7 && CheckAttribute(&Colonies[iDestCol], "HasNoFort")) otype = 1;
						}
					}
					aName = "iOrderType" + slot;     npchar.(aName) = otype;

					aName = "iTradeIsland" + slot;   npchar.(aName) = GetIslandByCityName(sToCity);
					maxDays = GetMaxDaysFromIsland2Island(GetArealByCityName(NPChar.city), GetArealByCityName(sToCity));

					g = -1;
					// КОНТРАБАНДА (5): возим РЕАЛЬНЫЙ контрабандный товар острова-отправителя (не топ-10).
					// Нет контрабанды на острове - контра-фрахт не даём: демотим в обычный (перезаписываем тип).
					if (otype == 5)
					{
						g = GetCargoContrabandGood(slot, NPChar.city);
						if (g < 0)
						{
							otype = 1;
							aName = "iOrderType" + slot;    npchar.(aName) = otype;
						}
					}
					// ГУБЕРНАТОРСКИЙ (6): возим товар АГРЕССИВНОГО СПРОСА (коричневый) магазина-отправителя.
					// Нет такого товара на острове - губ.фрахт не даём: демотим в обычный (перезаписываем тип).
					if (otype == 6)
					{
						g = GetCargoAggressiveGood(slot, NPChar.city);
						if (g < 0)
						{
							otype = 1;
							aName = "iOrderType" + slot;    npchar.(aName) = otype;
						}
					}
					// ЖИВАЯ ЭКОНОМИКА (базовые типы 0..4): возим то, что дёшево здесь и дорого у адресата
					if (g < 0 && otype <= 4) g = GetCargoLiveGood(slot, otype, NPChar.city, sToCity);
					if (g < 0) g = GetCargoOrderGood(otype, slot); // спец-типы и фоллбек, если выгодных пар нет
					aName = "iTradeGoods" + slot;    npchar.(aName) = g;

					qty = GetCargoOrderQty(otype, GetSquadronFreeSpace(pchar, g), slot);
					if (otype == 7) qty = GetFortConvoyRequiredWeight(); // конвой: объём не зависит от трюма ГГ (груз везут барки), храним требуемый ВЕС
					aName = "iQuantityGoods" + slot; npchar.(aName) = qty;

					dexp = GetCargoOrderDays(otype, maxDays, slot);
					aName = "iDaysExpired" + slot;   npchar.(aName) = dexp;
					aName = "iMaxDays" + slot;     npchar.(aName) = maxDays; // для премии эскорта (как у конвоя)

					// доля ГГ = 25% от прибыли перевозки по живым ценам; фоллбек - старая формула (спец-типы/нет пары)
					aName = "iMoney" + slot;
					npchar.(aName) = 0;
					if (otype <= 4) npchar.(aName) = GetCargoLiveMoney(otype, g, qty, NPChar.city, sToCity);
					if (otype == 7) npchar.(aName) = GetFortConvoyMoney(maxDays, dexp); // конвой: очень высокая плата за риски
					if (sti(npchar.(aName)) <= 0) npchar.(aName) = GetCargoOrderMoney(otype, qty, g, maxDays, dexp);

					// стоимость груза по живой цене отправителя - показывается в тексте заказа
					aName = "iCargoCost" + slot;
					if (otype == 7) npchar.(aName) = 0; // конвой: груз смешанный, стоимость в заказе не показываем
					else if (otype == 5) npchar.(aName) = 0; // LEO: контрабанда - стоимость груза в заказе НЕ показываем (лишняя сумма путала: платится только "моя доля")
						else if (otype == 6) npchar.(aName) = GetCargoBaseCost(g, qty); // губернаторский: по базовой стоимости
					else npchar.(aName) = GetCargoLiveCost(g, qty, NPChar.city);
					// LEO: фолбэк стоимости (как у оплаты выше) - магазин отправителя может вообще не торговать
					// этим товаром, тогда живая цена = 0. Чаще всего ловится на скоропорте: его товар берётся
					// из фикс-списка съестного БЕЗ сверки с ассортиментом магазина. Без фолбэка "(груз на N)"
					// молча пропадало из текста заказа.
					if (otype != 5)
					{
						if (otype != 7)
						{
							if (sti(npchar.(aName)) <= 0) npchar.(aName) = GetCargoBaseCost(g, qty);
						}
					}
				}
				if (sti(npchar.iQuantityGoods1) < 100 || sti(npchar.iQuantityGoods2) < 100 || sti(npchar.iQuantityGoods3) < 100) sErr = "shipsmall";
			}

			if (sErr == "guilddebt")
			{
				dialog.text = "A freight? For you?! (the shopkeeper slams the order book shut.) The guild has blacklisted you, "+GetAddress_Form(NPChar)+". To pocket someone else's cargo - and then ask for a new freight... What insolence.";
				link.l1 = "We'll see about that. Good day.";
				link.l1.go = "exit";
			}
			else if (sErr == "contrablock")
			{
				dialog.text = "(The shopkeeper shakes his head coldly.) A freight? After what you did with my cargo, "+GetAddress_Form(NPChar)+"? Don't even ask. Settle things with the right people - then we'll talk.";
				link.l1 = "Understood. Good day.";
				link.l1.go = "exit";
			}
			else if (sErr == "takentoday")
			{
				dialog.text = NPCharRepPhrase(npchar, "No orders for today, "+GetAddress_Form(NPChar)+". Come back tomorrow.", "No more orders for today, "+GetAddress_Form(NPChar)+". Do look in tomorrow.");
				link.l1 = "Very well, I'll come back tomorrow.";
				link.l1.go = "exit";
			}
			else if (sErr == "nofreight")
			{
				dialog.text = NPCharRepPhrase(npchar, "Eh, "+GetAddress_Form(NPChar)+", not up to charters today. Come back tomorrow.", "Come back tomorrow. Perhaps I will make you a good offer.");
				link.l1 = "Okay, I'll come back tomorrow.";
				link.l1.go = "exit";
			}
			else if (sErr == "political")
			{
				dialog.text = "The political situation in the archipelago does not allow me to trade with "+ XI_ConvertString(NationShortName(sti(NPChar.nation))+"hunter") + " at enmity with everyone, and I'm the only one left with the store.";
				link.l1 = "Oh! I'm sorry. Have a nice day.";
				link.l1.go = "exit";
			}
			else if (sErr == "shipsmall")
			{
				dialog.text = NPCharRepPhrase(npchar, "Your pathetic vessel won't fit all the cargo, there won't be a deal today.", "Unfortunately, captain "+GetFullName(pchar)+", I need a bigger ship than yours to deliver the cargo.");
				link.l1 = NPCharRepPhrase(npchar, "I have a decent ship, but I get your point. Goodbye.", "I understand you, business first of all. Goodbye.");
				link.l1.go = "exit";
			}
			else
			{
				SaveCurrentQuestDateParam("CargoQuest");
				// LEO: надбавка постоянного клиента (0-60%) к "моей доле" обычных фрахтов у ЭТОГО работодателя (не казённый конвой/эскорт).
				int iFreightLoy = Freight_LoyaltyPct(NPChar);
				// LEO: надбавку показываем У ИМЕНИ ("+X%" зелёным + " to the reward"), а не в тексте. 0% -> ничего.
				if (iFreightLoy >= 1) { NPChar.NameBonus = "+" + iFreightLoy + "%"; NPChar.NameBonusTail = " to the reward"; }
				if (sti(npchar.iOrderType1) != 7) npchar.iMoney1 = sti(npchar.iMoney1) * (100 + iFreightLoy) / 100;
				if (sti(npchar.iOrderType2) != 7) npchar.iMoney2 = sti(npchar.iMoney2) * (100 + iFreightLoy) / 100;
				if (sti(npchar.iOrderType3) != 7) npchar.iMoney3 = sti(npchar.iMoney3) * (100 + iFreightLoy) / 100;
				NPChar.sNation1 = XI_ConvertString("Colony"+Characters[sti(npchar.storeMan1)].city);
				NPChar.sNation2 = XI_ConvertString("Colony"+Characters[sti(npchar.storeMan2)].city);
				NPChar.sNation3 = XI_ConvertString("Colony"+Characters[sti(npchar.storeMan3)].city);
				NPChar.sTemp1 = ""; NPChar.sTemp2 = ""; NPChar.sTemp3 = "";
				if (npchar.iTradeIsland1 != Characters[sti(npchar.storeMan1)].city) NPChar.sTemp1 = " which is on the "  + XI_ConvertString(npchar.iTradeIsland1+"Voc");
				if (npchar.iTradeIsland2 != Characters[sti(npchar.storeMan2)].city) NPChar.sTemp2 = " which is on the "  + XI_ConvertString(npchar.iTradeIsland2+"Voc");
				if (npchar.iTradeIsland3 != Characters[sti(npchar.storeMan3)].city) NPChar.sTemp3 = " which is on the "  + XI_ConvertString(npchar.iTradeIsland3+"Voc");

				dialog.text = "Just a minute, I'm going to find my logbook. What I need to deliver now. Choose which order suits you best.";
				link.l1 = "[" + GetCargoOrderName(sti(npchar.iOrderType1)) + "] " + "Will undertake to deliver the cargo of " + GetGoodsNameAlt(sti(npchar.iTradeGoods1)) + " in quantity " + FindRussianQtyStringClr(sti(npchar.iQuantityGoods1), "\7") + " to the city " + NPChar.sNation1 + NPChar.sTemp1 + GetCargoCostNoteClr(sti(npchar.iCargoCost1), "\7") + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney1), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired1))) + ".";
				link.l1.go = "exit_trade1";
				link.l2 = "[" + GetCargoOrderName(sti(npchar.iOrderType2)) + "] " + "Will undertake to deliver the cargo of " + GetGoodsNameAlt(sti(npchar.iTradeGoods2)) + " in quantity " + FindRussianQtyStringClr(sti(npchar.iQuantityGoods2), "\7") + " to the city " + NPChar.sNation2 + NPChar.sTemp2 + GetCargoCostNoteClr(sti(npchar.iCargoCost2), "\7") + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney2), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired2))) + ".";
				link.l2.go = "exit_trade2";
				link.l3 = "[" + GetCargoOrderName(sti(npchar.iOrderType3)) + "] " + "Will undertake to deliver the cargo of " + GetGoodsNameAlt(sti(npchar.iTradeGoods3)) + " in quantity " + FindRussianQtyStringClr(sti(npchar.iQuantityGoods3), "\7") + " to the city " + NPChar.sNation3 + NPChar.sTemp3 + GetCargoCostNoteClr(sti(npchar.iCargoCost3), "\7") + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney3), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired3))) + ".";
				link.l3.go = "exit_trade3";
				// контрабандный заказ - только для неприметной эскадры (не больше 3 кораблей, каждый не мощнее 3 класса)
				if (sti(npchar.iOrderType1) == 5 && !CheckSmuggleFreightShips()) link.l1.go = "smuggle_need_small_fleet";
				if (sti(npchar.iOrderType2) == 5 && !CheckSmuggleFreightShips()) link.l2.go = "smuggle_need_small_fleet";
				if (sti(npchar.iOrderType3) == 5 && !CheckSmuggleFreightShips()) link.l3.go = "smuggle_need_small_fleet";
				// комендантский конвой: свой текст заказа (груз смешанный, везут казённые барки) + нужны 4 свободных места в эскадре
				if (sti(npchar.iOrderType1) == 7) link.l1 = "[" + GetCargoOrderName(7) + "] " + "Will undertake to escort a convoy: four state vessels with powder and munitions for the fort of " + NPChar.sNation1 + NPChar.sTemp1 + "\2" + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney1), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired1))) + ".";
				if (sti(npchar.iOrderType2) == 7) link.l2 = "[" + GetCargoOrderName(7) + "] " + "Will undertake to escort a convoy: four state vessels with powder and munitions for the fort of " + NPChar.sNation2 + NPChar.sTemp2 + "\2" + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney2), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired2))) + ".";
				if (sti(npchar.iOrderType3) == 7) link.l3 = "[" + GetCargoOrderName(7) + "] " + "Will undertake to escort a convoy: four state vessels with powder and munitions for the fort of " + NPChar.sNation3 + NPChar.sTemp3 + "\2" + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney3), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired3))) + ".";
				// governor subtype rolled AT GENERATION (stable by dRand), stored per slot - order is labelled
				// [Governor's escort] in the list, no submenu-confirm on taking
				DeleteAttribute(npchar, "GovSubtype1"); DeleteAttribute(npchar, "GovSubtype2"); DeleteAttribute(npchar, "GovSubtype3");
				if (sti(npchar.iOrderType1) == 6) npchar.GovSubtype1 = 1 + dRandSlotLow(1, 2); // 2 = ESCORT (~1/3, if conditions); 1/3 = plain governor. Swap is a SEPARATE 25-50% chance on non-escort (trade_quest_open)
				if (sti(npchar.iOrderType2) == 6) npchar.GovSubtype2 = 1 + dRandSlotLow(2, 2);
				if (sti(npchar.iOrderType3) == 6) npchar.GovSubtype3 = 1 + dRandSlotLow(3, 2);
				// LEO: недоступный эскорт не предлагается; обычную губернаторскую доставку сохраняем.
				if (sti(npchar.iOrderType1) == 6 && sti(npchar.GovSubtype1) == 2 && !GovEscort_CanTake()) npchar.GovSubtype1 = 1;
				if (sti(npchar.iOrderType2) == 6 && sti(npchar.GovSubtype2) == 2 && !GovEscort_CanTake()) npchar.GovSubtype2 = 1;
				if (sti(npchar.iOrderType3) == 6 && sti(npchar.GovSubtype3) == 2 && !GovEscort_CanTake()) npchar.GovSubtype3 = 1;
				// subtype 2 + conditions (GovEscort_CanTake) -> [Governor's escort] label + escort-a-convoy text
				if (sti(npchar.iOrderType1) == 6 && sti(npchar.GovSubtype1) == 2 && GovEscort_CanTake()) npchar.iMoney1 = GetGovEscortMoney(sti(npchar.iMaxDays1), sti(npchar.iDaysExpired1)); // премия эскорта = % стоимости охраняемого груза
				if (sti(npchar.iOrderType2) == 6 && sti(npchar.GovSubtype2) == 2 && GovEscort_CanTake()) npchar.iMoney2 = GetGovEscortMoney(sti(npchar.iMaxDays2), sti(npchar.iDaysExpired2));
				if (sti(npchar.iOrderType3) == 6 && sti(npchar.GovSubtype3) == 2 && GovEscort_CanTake()) npchar.iMoney3 = GetGovEscortMoney(sti(npchar.iMaxDays3), sti(npchar.iDaysExpired3));
				if (sti(npchar.iOrderType1) == 6 && sti(npchar.GovSubtype1) == 2 && GovEscort_CanTake()) npchar.iCargoCost1 = GetGovEscortCargoValue(sti(npchar.iTradeGoods1)); // стоимость охраняемого груза (полная загрузка)
				if (sti(npchar.iOrderType2) == 6 && sti(npchar.GovSubtype2) == 2 && GovEscort_CanTake()) npchar.iCargoCost2 = GetGovEscortCargoValue(sti(npchar.iTradeGoods2)); // стоимость охраняемого груза (полная загрузка)
				if (sti(npchar.iOrderType3) == 6 && sti(npchar.GovSubtype3) == 2 && GovEscort_CanTake()) npchar.iCargoCost3 = GetGovEscortCargoValue(sti(npchar.iTradeGoods3)); // стоимость охраняемого груза (полная загрузка)
				if (sti(npchar.iOrderType1) == 6 && sti(npchar.GovSubtype1) == 2 && GovEscort_CanTake()) link.l1 = "[Governor's escort] Will undertake to escort a convoy carrying " + GetGoodsNameAlt(sti(npchar.iTradeGoods1)) + " in quantity " + FindRussianQtyStringClr(GetGovEscortCargoQty(sti(npchar.iTradeGoods1)), "\7") + " to the city " + NPChar.sNation1 + NPChar.sTemp1 + GetCargoCostNoteClr(sti(npchar.iCargoCost1), "\7") + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney1), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired1))) + ".";
				if (sti(npchar.iOrderType2) == 6 && sti(npchar.GovSubtype2) == 2 && GovEscort_CanTake()) link.l2 = "[Governor's escort] Will undertake to escort a convoy carrying " + GetGoodsNameAlt(sti(npchar.iTradeGoods2)) + " in quantity " + FindRussianQtyStringClr(GetGovEscortCargoQty(sti(npchar.iTradeGoods2)), "\7") + " to the city " + NPChar.sNation2 + NPChar.sTemp2 + GetCargoCostNoteClr(sti(npchar.iCargoCost2), "\7") + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney2), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired2))) + ".";
				if (sti(npchar.iOrderType3) == 6 && sti(npchar.GovSubtype3) == 2 && GovEscort_CanTake()) link.l3 = "[Governor's escort] Will undertake to escort a convoy carrying " + GetGoodsNameAlt(sti(npchar.iTradeGoods3)) + " in quantity " + FindRussianQtyStringClr(GetGovEscortCargoQty(sti(npchar.iTradeGoods3)), "\7") + " to the city " + NPChar.sNation3 + NPChar.sTemp3 + GetCargoCostNoteClr(sti(npchar.iCargoCost3), "\7") + ", my share is " + FindRussianMoneyStringClr(sti(npchar.iMoney3), "\4") + ", no later than for " + FindRussianDaysString(makeint(sti(npchar.iDaysExpired3))) + ".";
				int iFreeSlots = COMPANION_MAX - GetCompanionQuantity(pchar);
				if (sti(npchar.iOrderType1) == 7 && iFreeSlots < 4) link.l1.go = "fortammo_need_slots";
				if (sti(npchar.iOrderType2) == 7 && iFreeSlots < 4) link.l2.go = "fortammo_need_slots";
				if (sti(npchar.iOrderType3) == 7 && iFreeSlots < 4) link.l3.go = "fortammo_need_slots";
				// LEO: устаревшие конвойные предложения не обходят ограничения ранга и флагмана.
				if (!FreightConvoy_CanConfirm(npchar, 1)) DeleteAttribute(link, "l1");
				if (!FreightConvoy_CanConfirm(npchar, 2)) DeleteAttribute(link, "l2");
				if (!FreightConvoy_CanConfirm(npchar, 3)) DeleteAttribute(link, "l3");
				link.l4 = "I need to think about it.";
				link.l4.go = "exit";
			}
		break;

		case "exit_trade":
			AddDialogExitQuest("trade_quest_open");
			Nextdiag.CurrentNode = Nextdiag.TempNode;
			DialogExit();
		break;
		case "exit_trade1":
			if (!FreightConvoy_CanConfirm(npchar, 1))
			{
				dialog.text = "This convoy requires rank " + FreightConvoy_MinRank(sti(npchar.iOrderType1)) + " or higher, a flagship of class 5 or better, and room in your squadron. A commandant's convoy requires four free slots; a governor's escort allows no more than four ships of your own.";
				link.l1 = "I will return when I am ready.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Great! Allow me to begin loading.";
			link.l1 = "Proceed, dear!";
			link.l1.go = "exit";
			pchar.CargoQuest.iDaysExpired = sti(npchar.iDaysExpired1);
    		pchar.CargoQuest.iTradeGoods = npchar.iTradeGoods1;
    		pchar.CargoQuest.iQuantityGoods = sti(npchar.iQuantityGoods1);
    		pchar.CargoQuest.iMoney = sti(npchar.iMoney1);
    		pchar.CargoQuest.iTradeNation = sti(npchar.iTradeNation1);
    		pchar.CargoQuest.iTradeColony = Characters[sti(npchar.storeMan1)].city;
    		pchar.CargoQuest.iTradeIsland = npchar.iTradeIsland1;
			pchar.CargoQuest.iOrderType = npchar.iOrderType1;
			if (sti(npchar.iOrderType1) == 7) dialog.text = "Excellent! The ships are already in the harbour, their captains will report to you in the roadstead - the convoy is under your command now. Take care of it, captain: gunpowder is a nervous cargo.";
			pchar.CargoQuest.TraderID     = Characters[sti(npchar.storeMan1)].id;
    		pchar.CargoQuest.GiveTraderID = NPChar.id;
			pchar.CargoQuest.GovSubtype = 0;
			if (sti(pchar.CargoQuest.iOrderType) == 6) pchar.CargoQuest.GovSubtype = sti(npchar.GovSubtype1); // подтип выпал при генерации (в списке уже подписан)
			if (sti(pchar.CargoQuest.GovSubtype) == 2 && GovEscort_CanTake()) dialog.text = "Excellent! The convoy of important cargo is already in the harbour, its captains will report to you in the roadstead - the ships are under your command. Bring them through, and you will be paid.";
			AddDialogExitQuest("trade_quest_open");
			Nextdiag.CurrentNode = Nextdiag.TempNode;
		break;
		case "exit_trade2":
			if (!FreightConvoy_CanConfirm(npchar, 2))
			{
				dialog.text = "This convoy requires rank " + FreightConvoy_MinRank(sti(npchar.iOrderType2)) + " or higher, a flagship of class 5 or better, and room in your squadron. A commandant's convoy requires four free slots; a governor's escort allows no more than four ships of your own.";
				link.l1 = "I will return when I am ready.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Great! Allow me to begin loading.";
			link.l1 = "Proceed, dear!";
			link.l1.go = "exit";
			pchar.CargoQuest.iDaysExpired = sti(npchar.iDaysExpired2);
    		pchar.CargoQuest.iTradeGoods = npchar.iTradeGoods2;
    		pchar.CargoQuest.iQuantityGoods = sti(npchar.iQuantityGoods2);
    		pchar.CargoQuest.iMoney = sti(npchar.iMoney2);
    		pchar.CargoQuest.iTradeNation = sti(npchar.iTradeNation2);
    		pchar.CargoQuest.iTradeColony = Characters[sti(npchar.storeMan2)].city;
    		pchar.CargoQuest.iTradeIsland = npchar.iTradeIsland2;
			pchar.CargoQuest.iOrderType = npchar.iOrderType2;
			if (sti(npchar.iOrderType2) == 7) dialog.text = "Excellent! The ships are already in the harbour, their captains will report to you in the roadstead - the convoy is under your command now. Take care of it, captain: gunpowder is a nervous cargo.";
			pchar.CargoQuest.TraderID     = Characters[sti(npchar.storeMan2)].id;
    		pchar.CargoQuest.GiveTraderID = NPChar.id;
			pchar.CargoQuest.GovSubtype = 0;
			if (sti(pchar.CargoQuest.iOrderType) == 6) pchar.CargoQuest.GovSubtype = sti(npchar.GovSubtype2); // подтип выпал при генерации (в списке уже подписан)
			if (sti(pchar.CargoQuest.GovSubtype) == 2 && GovEscort_CanTake()) dialog.text = "Excellent! The convoy of important cargo is already in the harbour, its captains will report to you in the roadstead - the ships are under your command. Bring them through, and you will be paid.";
			AddDialogExitQuest("trade_quest_open");
			Nextdiag.CurrentNode = Nextdiag.TempNode;
		break;
		case "exit_trade3":
			if (!FreightConvoy_CanConfirm(npchar, 3))
			{
				dialog.text = "This convoy requires rank " + FreightConvoy_MinRank(sti(npchar.iOrderType3)) + " or higher, a flagship of class 5 or better, and room in your squadron. A commandant's convoy requires four free slots; a governor's escort allows no more than four ships of your own.";
				link.l1 = "I will return when I am ready.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Great! Allow me to begin loading.";
			link.l1 = "Proceed, dear!";
			link.l1.go = "exit";
			pchar.CargoQuest.iDaysExpired = sti(npchar.iDaysExpired3);
    		pchar.CargoQuest.iTradeGoods = npchar.iTradeGoods3;
    		pchar.CargoQuest.iQuantityGoods = sti(npchar.iQuantityGoods3);
    		pchar.CargoQuest.iMoney = sti(npchar.iMoney3);
    		pchar.CargoQuest.iTradeNation = sti(npchar.iTradeNation3);
    		pchar.CargoQuest.iTradeColony = Characters[sti(npchar.storeMan3)].city;
    		pchar.CargoQuest.iTradeIsland = npchar.iTradeIsland3;
			pchar.CargoQuest.iOrderType = npchar.iOrderType3;
			if (sti(npchar.iOrderType3) == 7) dialog.text = "Excellent! The ships are already in the harbour, their captains will report to you in the roadstead - the convoy is under your command now. Take care of it, captain: gunpowder is a nervous cargo.";
			pchar.CargoQuest.TraderID     = Characters[sti(npchar.storeMan3)].id;
    		pchar.CargoQuest.GiveTraderID = NPChar.id;
			pchar.CargoQuest.GovSubtype = 0;
			if (sti(pchar.CargoQuest.iOrderType) == 6) pchar.CargoQuest.GovSubtype = sti(npchar.GovSubtype3); // подтип выпал при генерации (в списке уже подписан)
			if (sti(pchar.CargoQuest.GovSubtype) == 2 && GovEscort_CanTake()) dialog.text = "Excellent! The convoy of important cargo is already in the harbour, its captains will report to you in the roadstead - the ships are under your command. Bring them through, and you will be paid.";
			AddDialogExitQuest("trade_quest_open");
			Nextdiag.CurrentNode = Nextdiag.TempNode;
		break;

		case "smuggle_need_small_fleet":
			dialog.text = "Hush, not so loud... Cargo like that travels quietly. With a squadron of more than three sail, or a powerful ship above the third class, any patrol will spot you - the right people won't even come ashore. Get yourself something more modest, then we'll talk about this order.";
			link.l1 = "Understood. I'll come back with something less conspicuous.";
			link.l1.go = "exit";
		break;

		case "fortammo_need_slots":
			dialog.text = "This is a convoy order, captain: four state vessels loaded with powder will sail under your pennant, each with her own crew. I need four free berths in your squadron. Make room - and come back, the job is worth it.";
			link.l1 = "Very well, I'll sort out my squadron and return.";
			link.l1.go = "exit";
		break;

		// --- loophole: bribe the guild clerk (the cargo manifest "goes missing", no debt; 12-55% the clerk cheats - debt comes later) ---
		case "FreightClerk_Offer":
			int fcCost = Freight_BribePrepare();
			if (sti(pchar.money) < fcCost)
			{
				dialog.text = "Heh... a bold offer. Only the right paper doesn't come cheap - " + FindRussianMoneyString(fcCost) + ". You don't carry that much. Come back with the coin.";
				link.l1 = "I'll come back later.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "The trader glances about and lowers his voice. - For " + FindRussianMoneyString(fcCost) + " the right manifest will vanish into the hearth, as if it never was. The guild likes order in its papers... and papers do sometimes get lost. Deal?";
			link.l1 = "Deal. Here's the money.";
			link.l1.go = "FreightClerk_Done";
			link.l2 = "Changed my mind.";
			link.l2.go = "exit";
		break;

		case "FreightClerk_Done":
			// outcome (honest erase / cheat) is outwardly identical - betrayal only surfaces as a debt later
			Freight_BribeClerk();
			dialog.text = "The trader pockets the coins and gives a faint nod. - As far as anyone knows, that manifest never existed. Now off you go, and quickly.";
			link.l1 = "Much obliged.";
			link.l1.go = "exit";
		break;

		case "GuildDebt_Settle":
			int gdGoods = sti(pchar.GuildDebt.GoodsType);
			int gdQty   = sti(pchar.GuildDebt.GoodsQty);
			int gdSum   = sti(pchar.GuildDebt.sum);
			int gdComp  = sti(pchar.GuildDebt.GoodsComp);
			// last call: the cargo was sold to the fence, nothing to return in goods - money only
			if (CheckAttribute(pchar, "GuildDebt.NoGoods"))
			{
				dialog.text = "(The shopkeeper consults the guild ledger and narrows his eyes.) Ah, it's you... That pocketed cargo surfaced on the side, and the guild is not blind. Since there are no goods left to return, the talk is short: the forfeit in coin, " + FindRussianMoneyString(gdSum) + ", and not a piaster less. Otherwise the Guthries' men will collect it themselves.";
				if (sti(pchar.money) >= gdSum)
				{
					link.l1 = "Here is the full sum. Let us close this.";
					link.l1.go = "GuildDebt_PayMoney";
				}
				link.l3 = "I need to gather the money. I shall return.";
				link.l3.go = "exit";
				break;
			}
			dialog.text = "(The shopkeeper slowly consults the guild ledger.) Well, well... You are charged with a pocketed cargo: " + GetGoodsNameAlt(gdGoods) + ", " + FindRussianQtyString(gdQty) + ". The forfeit is " + FindRussianMoneyString(gdSum) + ". The guild will accept either the money, or the goods themselves with a surcharge of a quarter of their value (" + FindRussianMoneyString(gdComp) + ").";
			if (sti(pchar.money) >= gdSum)
			{
				link.l1 = "Here is the full sum. Let us forget this.";
				link.l1.go = "GuildDebt_PayMoney";
			}
			bool bGdGoods = false;
			if (GetSquadronGoods(pchar, gdGoods) >= gdQty && sti(pchar.money) >= gdComp) bGdGoods = true;
			if (bGdGoods)
			{
				link.l2 = "The goods are in my hold, take them. Here is the surcharge.";
				link.l2.go = "GuildDebt_PayGoods";
			}
			link.l3 = "I need to gather the money. I'll be back.";
			link.l3.go = "exit";
		break;

		case "GuildDebt_PayMoney":
			AddQuestRecord("GuildDebt", "3");
			AddQuestUserData("GuildDebt", "sMoney", FindRussianMoneyString(sti(pchar.GuildDebt.sum)));
			AddMoneyToCharacter(pchar, -sti(pchar.GuildDebt.sum));
			dialog.text = "(The shopkeeper counts the piastres and crosses you out of the guild ledger.) Payment accepted. The guild will recall the bounty on your head, and the blacklists will be burned. Don't trifle with guild cargo again, " + GetAddress_Form(NPChar) + ".";
			link.l1 = "Lesson learned.";
			link.l1.go = "exit";
			GuildDebt_Close();
		break;

		case "GuildDebt_PayGoods":
			AddQuestRecord("GuildDebt", "4");
			AddQuestUserData("GuildDebt", "sSex", GetSexPhrase("","а"));
			RemoveCharacterGoods(pchar, sti(pchar.GuildDebt.GoodsType), sti(pchar.GuildDebt.GoodsQty));
			AddMoneyToCharacter(pchar, -sti(pchar.GuildDebt.GoodsComp));
			dialog.text = "(The shopkeeper orders the unloading and crosses you out of the guild ledger.) The goods and the surcharge are accepted. The guild will recall the bounty on your head, and the blacklists will be burned. Don't trifle with guild cargo again, " + GetAddress_Form(NPChar) + ".";
			link.l1 = "Lesson learned.";
			link.l1.go = "exit";
			GuildDebt_Close();
		break;

		case "smuggle_wrong_trader":
			dialog.text = "Quiet, you! I'm an honest merchant, I don't touch cargo like that. Go find the right people - this town has someone you can talk to about such business. And not another word about it in front of me.";
			link.l1 = "Understood. My apologies.";
			link.l1.go = "exit";
		break;

		case "governor_wrong_trader":
			dialog.text = "State cargo isn't my business, captain. Take it straight to the residence, to the governor. He'll decide where to put it.";
			link.l1 = "Understood. I'll head to the residence.";
			link.l1.go = "exit";
		break;

		case "fortammo_wrong_trader":
			dialog.text = "Ammunition? That's not for a shop, captain. Take it to the fort commandant - he runs the garrison arsenal and will settle up with you.";
			link.l1 = "Understood. I'll head to the commandant.";
			link.l1.go = "exit";
		break;

		case "generate_quest_2":
			if (pchar.quest.generate_trade_quest_progress == "failed")
			{
				dialog.text = RandSwear() + "You remembered the task assigned to you too late, don't you think? I had to order the goods again. Goodbye.";
				link.l1 = "Forgive me, but I was in such a hurry. Perhaps if I cover your losses, it will restore my business reputation?";
				link.l1.go = "generate_quest_failed";
				link.l9 = "Have a nice day.";
				link.l9.go = "exit";
				ChangeCharacterReputation(pchar, -1);
			}
			else
			{
                iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
                iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);

                dialog.text = "Exactly! I've been waiting for him for a long time. You have to deliver the cargo of " +
                GetGoodsNameAlt(iTradeGoods) + " for me in quantity " + FindRussianQtyString(iQuantityGoods) + " for " +
                FindRussianMoneyString(sti(pchar.CargoQuest.iMoney)) + ".";
				link.l1 = "That's right.";
				link.l1.go = "generate_quest_ready";
			}
		break;

		case "generate_quest_ready":
            iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);

            // блок кидалова нас на бабки -->
            // === consignee store is BLOCKED (you scammed contraband there): the trader withholds the forfeit from your premium, paying off the debt via deliveries ===
            // (stores already paid off at the contact - NoUnblockWait - are skipped: debt settled, only the timed unblock remains)
            if (CheckAttribute(pchar, "NoContraCity." + npchar.city) && !CheckAttribute(pchar, "NoUnblockWait." + npchar.city))
            {
                string sCty = npchar.city; // accessor .(...) won't take npchar.city directly (it's an attribute, not a variable) - extract it
                if (GetSquadronGoods(pchar, iTradeGoods) < iQuantityGoods)
                {
                    int iMiss = iQuantityGoods - GetSquadronGoods(pchar, iTradeGoods);
                    dialog.text = "You haven't brought all the goods!! You're still " + FindRussianQtyString(iMiss) + " short.";
                    link.l9 = "I'll make up the shortfall and come again.";
                    link.l9.go = "exit";
                    break;
                }
                int iPrem = makeint(pchar.CargoQuest.iMoney);
                if (GetQuestPastDayParam("CargoQuest") >= sti(pchar.CargoQuest.iDaysExpired))
                {
                    iPrem = makeint(iPrem / 2);            // late - half pay
                    Log_Info(XI_ConvertString("Freight_LateFine"));
                    Log_InfoColored("red", "-" + FindRussianMoneyStringLog(iPrem));
                }
                else if (GetQuestPastDayParam("CargoQuest") * 2 < sti(pchar.CargoQuest.iDaysExpired))
                {
                    iPrem = makeint(iPrem * 1.2);          // early
                    Log_Info(XI_ConvertString("Freight_EarlyBonusCargo"));
                    Log_InfoColored("green", "+" + FindRussianMoneyStringLog(makeint(sti(pchar.CargoQuest.iMoney)*0.2)));
                }
                int iDebt = sti(pchar.NoContraCity.(sCty));
                int iTake = iPrem;
                if (iTake > iDebt) iTake = iDebt;   // no more than the remaining forfeit
                int iLeft = iDebt - iTake;
                Log_Info(XI_ConvertString("Freight_Accrued") + FindRussianMoneyString(iPrem));
                if (iTake > 0)
                {
                    Log_Info(XI_ConvertString("Freight_DebtHeld"));
                    Log_InfoColored("red", "-" + FindRussianMoneyStringLog(iTake));
                }
                AddMoneyToCharacter(pchar, iPrem - iTake);                                  // premium minus the withheld forfeit - into the player's purse
                RemoveCharacterGoods(pchar, iTradeGoods, iQuantityGoods);
                ref bstore = GetColonyByIndex(FindColony(npchar.city));
                AddStoreGoods(&stores[sti(bstore.StoreNum)], iTradeGoods, iQuantityGoods);
                pchar.quest.generate_trade_quest_progress = "";
                pchar.quest.generate_trade_quest.over = "yes";
                CloseQuestHeader("DELIVERY_TRADE_QUEST");
                AddCharacterExpToSkill(pchar, "Sailing", sti(pchar.rank) * 5);
                AddCharacterExpToSkill(pchar, "Leadership", sti(pchar.rank) * 3);
                AddCharacterExpToSkill(pchar, "COMMERCE", sti(pchar.rank) * 4);
                OfficersReaction("good");
                AchievementsCounter_genquests(1);
                if (iLeft <= 0)
                {
                    DeleteAttribute(pchar, "NoContraCity." + npchar.city);                  // forfeit paid in full - the store block is lifted
                    dialog.text = "(The shopkeeper counts the coins and softens for the first time.) I've taken the last " + FindRussianMoneyString(iTake) + " of the forfeit from your premium - and the debt is settled. Let bygones be bygones, " + GetAddress_Form(NPChar) + ": I'm ready to work with you again.";
                }
                else
                {
                    pchar.NoContraCity.(sCty) = iLeft;                               // forfeit remainder - the block stands
                    dialog.text = "(The shopkeeper consults his ledger.) I'll take the goods, but you owe me - that unfinished business. I'll withhold " + FindRussianMoneyString(iTake) + " of the forfeit from your premium. " + FindRussianMoneyString(iLeft) + " left - pay it off, and we'll do business again.";
                }
                link.l1 = "Fair enough.";
                link.l1.go = "exit";
                break;
            }

            if (rand(8) == 1 && sti(Goods[iTradeGoods].Cost) <= 44 && GetQuestPastDayParam("CargoQuest") > 5)
            {
                dialog.text = RandSwear() + "I have to apologize to you, "+GetAddress_Form(NPChar)+
                              "That is I am " + RandPhraseSimple("in a difficult situation and will not be able to pay for your services", "I have already taken this cargo elsewhere at a better price")+
                              ". As payment for your freight, I am handing over to you all the cargo you have delivered.";
				link.l1 = RandSwear() + RandPhraseSimple("And I'm was saving this damn cargo from all misfortunes!!! I killed two hundred rats because of it!",
                                                         "You're putting me in a hopeless position, and I'm going to have to agree.");
				link.l1.go = "exit";
				ChangeCharacterReputation(pchar, 2);
				AddCharacterExpToSkill(pchar, "Sailing", 60);
				AddCharacterExpToSkill(pchar, "Leadership", 20);
				AddCharacterExpToSkill(pchar, "COMMERCE", 15);

				pchar.quest.generate_trade_quest_progress = "";
				pchar.quest.generate_trade_quest.over = "yes";

                AddQuestRecord("DELIVERY_TRADE_QUEST", "4");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
			    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Dat"));
                CloseQuestHeader("DELIVERY_TRADE_QUEST");

				AchievementsCounter_genquests(1);

                break;
            }
            // блок кидалова нас на бабки <--
			if (GetSquadronGoods(pchar, iTradeGoods) < iQuantityGoods)
			{
                iTmp = iQuantityGoods - GetSquadronGoods(pchar, iTradeGoods);
                dialog.text = "You didn't bring all the goods!! According to the terms of the agreement, there is still a lack of "
                              + FindRussianQtyString(iTmp) + " goods.";
				link.l9 = RandPhraseSimple("Yes, indeed. We'll have to make up the shortfall. I'll come back again.",
                                           RandSwear() +"The devil rats messed up the damn cargo. I'll make up the shortfall and come back again.");
				link.l9.go = "exit";
                AddQuestRecord("DELIVERY_TRADE_QUEST", "8");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sHaveNotQty", iTmp);
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
			}
			else
			{
				if (GetQuestPastDayParam("CargoQuest") >= sti(pchar.CargoQuest.iDaysExpired))
				{
					dialog.text = "You are 1 day late. Therefore, I will only pay you half the price.";
					link.l1 = "Ehh, it doesn't happen to anyone, let's";
					link.l1.go = "exit";
					ChangeCharacterReputation(pchar, 3);
					AddCharacterExpToSkill(pchar, "Sailing", sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_I) );
					AddCharacterExpToSkill(pchar, "Leadership", sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_C));
					AddCharacterExpToSkill(pchar, "COMMERCE", sti(pchar.rank) * (GetCharacterSPECIALSimple(pchar, SPECIAL_C) + GetCharacterSPECIALSimple(pchar, SPECIAL_I)));

					Log_Info(XI_ConvertString("Freight_LateFine"));
					Log_InfoColored("red", "-" + FindRussianMoneyStringLog(makeint(sti(pchar.CargoQuest.iMoney)/2)));
					Log_Info(XI_ConvertString("Freight_TotalPlain"));
					AddMoneyToCharacter(pchar, makeint(sti(pchar.CargoQuest.iMoney)/2));
					Freight_AddLoyalty(); // LEO: +1% к будущей премии этого работодателя (груз доставлен, пусть и с опозданием)
					pchar.quest.generate_trade_quest_progress = "";
					pchar.quest.generate_trade_quest.over = "yes";
					RemoveCharacterGoods(pchar, makeint(pchar.CargoQuest.iTradeGoods), makeint(pchar.CargoQuest.iQuantityGoods));
					ref cstore = GetColonyByIndex(FindColony(npchar.city));
					AddStoreGoods(&stores[sti(cstore.StoreNum)],makeint(pchar.CargoQuest.iTradeGoods),makeint(pchar.CargoQuest.iQuantityGoods));

					OfficersReaction("good");

					AddQuestRecord("DELIVERY_TRADE_QUEST", "Late1Day");
					CloseQuestHeader("DELIVERY_TRADE_QUEST");

					AchievementsCounter_genquests(1);
				}
				else
				{
					dialog.text = "Wonderful! Here is your reward, Captain " + GetFullName(pchar)+".";
					link.l1 = "Nice to work with you.";
					link.l1.go = "exit";
					ChangeCharacterReputation(pchar, 3);

					AddCharacterExpToSkill(pchar, "Sailing", (GetQuestPastDayParam("CargoQuest") + 1) * sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_I) );
					AddCharacterExpToSkill(pchar, "Leadership", (GetQuestPastDayParam("CargoQuest") + 1) * sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_C));
					AddCharacterExpToSkill(pchar, "COMMERCE", (GetQuestPastDayParam("CargoQuest") + 1) * sti(pchar.rank) * (GetCharacterSPECIALSimple(pchar, SPECIAL_C) + GetCharacterSPECIALSimple(pchar, SPECIAL_I)));

					int iReward = makeint(pchar.CargoQuest.iMoney);
					if (GetQuestPastDayParam("CargoQuest") * 2 < sti(pchar.CargoQuest.iDaysExpired))
					{
						iReward = makeint(iReward * 1.2);
						Log_Info(XI_ConvertString("Freight_EarlyBonusCargo"));
						Log_InfoColored("green", "+" + FindRussianMoneyStringLog(makeint(sti(pchar.CargoQuest.iMoney)*0.2)));
					}
					Log_Info(XI_ConvertString("Freight_TotalPlain"));
					AddMoneyToCharacter(pchar, iReward);
					Freight_AddLoyalty(); // LEO: +1% к будущей премии этого работодателя за выполненный фрахт
					ChangeCharacterNationReputation(pchar, sti(npchar.nation), 1);
					pchar.quest.generate_trade_quest_progress = "";
					pchar.quest.generate_trade_quest.over = "yes";
					RemoveCharacterGoods(pchar, makeint(pchar.CargoQuest.iTradeGoods), makeint(pchar.CargoQuest.iQuantityGoods));
					ref custore = GetColonyByIndex(FindColony(npchar.city));
					AddStoreGoods(&stores[sti(custore.StoreNum)],makeint(pchar.CargoQuest.iTradeGoods),makeint(pchar.CargoQuest.iQuantityGoods));

					OfficersReaction("good");
					AddQuestRecord("DELIVERY_TRADE_QUEST", "2");
					AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
					AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));
					CloseQuestHeader("DELIVERY_TRADE_QUEST");

					AchievementsCounter_genquests(1);
				}
			}
		break;

		case "generate_quest_failed":
            iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);
            iMoney =  3*makeint(iQuantityGoods * sti(Goods[iTradeGoods].Cost) / sti(Goods[iTradeGoods].Units));
			dialog.text = "Okay... The amount in " + FindRussianMoneyString(iMoney) + " will just cover my losses from your violation of the delivery time of the goods.";
			link.l1 = "Wow! No way! I don't think I'm going to carry these damn loads anymore! There are enough of them in the sea!";
			link.l1.go = "exit";
            if (sti(pchar.Money) >= iMoney)
            {
    			link.l2 = "Here is the required amount";
    			link.l2.go = "generate_quest_failed_2";
			}
            else
            {
                AddQuestRecord("DELIVERY_TRADE_QUEST", "7");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sMoney", FindRussianMoneyString(iMoney));
    		    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));
            }
		break;

		case "generate_quest_failed_2":
            iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);
            iMoney =  3*makeint(iQuantityGoods * sti(Goods[iTradeGoods].Cost) / sti(Goods[iTradeGoods].Units));
			dialog.text = "Wonderful. Now I can inform my colleagues that you can still be dealt with.";
			link.l1 = "Thank you! I won't let you down next time.";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, 10);
			AddCharacterExpToSkill(pchar, "Sailing", -100);
			AddCharacterExpToSkill(pchar, "Leadership", -50);
			AddCharacterExpToSkill(pchar, "COMMERCE", -100);

			AddMoneyToCharacter(pchar, -iMoney);
			pchar.quest.generate_trade_quest_progress = "";
			pchar.quest.generate_trade_quest.over = "yes";

            AddQuestRecord("DELIVERY_TRADE_QUEST", "5");
            AddQuestUserData("DELIVERY_TRADE_QUEST", "sMoney", FindRussianMoneyString(iMoney));
            AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
		    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));
            CloseQuestHeader("DELIVERY_TRADE_QUEST");

			AchievementsCounter_genquests(1);
		break;

		case "generate_quest_not_closed":
            iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);

            dialog.text = "Hmm. I heard that you have not fulfilled your obligations under the previous freight yet, and you are already asking for a new one. You were supposed to deliver the cargo of " +
            LanguageConvertString(iSeaGoods, "seg_" + Goods[iTradeGoods].Name)+ "in" + XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony) + ".";
			link.l1 = "Yes, exactly! You're right!";
			link.l1.go = "exit";
		break;

		case "generate_quest_cannot_done":
            dialog.text = RandSwear() + GetAddress_Form(NPChar) +"you're letting me down!!! Maybe you can try to solve this problem somehow?";
			link.l1 = "Okay. I will try to deliver the cargo.";
			link.l1.go = "exit";
			link.l2 = "No. I can't. I'm sorry.";
			link.l2.go = "generate_quest_cannot_done_2";

		break;

		case "generate_quest_cannot_done_2":
		    iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);
            if (GetSquadronGoods(pchar, iTradeGoods) < iQuantityGoods)
			{
				dialog.text = "Wait a minute! But there is not such a quantity of goods on the ship!! You lack more "
                              + FindRussianQtyString(iQuantityGoods - GetSquadronGoods(pchar, iTradeGoods)) + " goods.";
				link.l9 = RandPhraseSimple("Yes, indeed. We'll have to make up the shortfall.",
                                           RandSwear() +"The ship's rats have damaged the cargo. I'll finish buying the goods and come back again.");
				link.l9.go = "exit";
			}
			else
			{
                dialog.text = "Okay, I understand... I accept the goods back.";
    			link.l1 = "Thank you. I won't let you down next time!";
    			link.l1.go = "exit";
    			ChangeCharacterReputation(pchar, -3);

				pchar.quest.generate_trade_quest_progress = "";
				pchar.quest.generate_trade_quest.over = "yes";
				RemoveCharacterGoods(pchar, makeint(pchar.CargoQuest.iTradeGoods), makeint(pchar.CargoQuest.iQuantityGoods));

                AddQuestRecord("DELIVERY_TRADE_QUEST", "6");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodQty", FindRussianQtyString(sti(pchar.CargoQuest.iQuantityGoods)));
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
    		    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Dat"));
                CloseQuestHeader("DELIVERY_TRADE_QUEST");
            }
		break;

		case "no_quests":
			dialog.text = "I don't need your services at the moment.";
			link.l1 = "Very sorry. Then let's talk about something else.";
			link.l1.go = "node_1";
		break;

        case "fight":
			DialogExit();
            NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
		break;

		case "Exit":
			if (pchar.questTemp.Ascold == "canTakeQuest" && pchar.questTemp.Ascold.TraderId == npchar.id)//eddy. Аскольд, начало
			{
				dialog.Text = "You go a lot of places. Listen, if you find Egyptian mummy powder somewhere, then be kind, bring it to me. I have a serious client for this case.";
				Link.l1 = "Good.";
				Link.l1.go = "exit";
				SaveCurrentQuestDateParam("questTemp.Ascold");
				pchar.questTemp.Ascold = "Seek_powder";
				AddQuestRecord("Ascold", "1");
				AddQuestUserData("Ascold", "sName", GetFullName(npchar));
				AddQuestUserData("Ascold", "sCity", XI_ConvertString("Colony" + npchar.city + "Voc"));
			}
			else
			{
				Nextdiag.CurrentNode = Nextdiag.TempNode;
				DialogExit();
			}
		break;
		case "storage_rent":
			dialog.text = "So you want to rent one of our warehouses for a reasonable fee?";
			link.l1 = "Is it big? and how much will you ask for the rent?";
			link.l1.go = "storage_rent1";
		break;

		case "storage_rent1":
			NPChar.MoneyForStorage = GetStoragePriceExt(NPChar, pchar);
			if (NPChar.city == "Tortuga" || NPChar.city == "Villemstad" || NPChar.city == "PortRoyal" || NPChar.city == "Havana")
			{
				dialog.text = "It is spacious enough even for a port warehouse - a capacity of 100000 centners. For " + FindRussianMoneyString(sti(NPChar.MoneyForStorage)) + " per month I am ready to ensure the safety of your goods. "+
				"This includes security services, flood protection and rodent control. Is it okay? I guarantee confidentiality, of course.";
			}
			else
			{
				dialog.text = "It is spacious enough even for a port warehouse - a capacity of 50000 centners. For " + FindRussianMoneyString(sti(NPChar.MoneyForStorage)) + " per month I am ready to ensure the safety of your goods. "+
				"This includes security services, flood protection and rodent control. Is it okay? I guarantee confidentiality, of course.";
			}

			link.l1 = "Good. May I examine him?";
			link.l1.go = "storage_rent2";
			link.l2 = "Ask too much. There's probably knee-deep water and rats running around.";
			link.l2.go = "storage_rent3";
		break;

		case "storage_rent2":
			dialog.text = "Of course, of course. Only, uh... I'll ask for the payment for the month in advance.";
			if(sti(pchar.money) >= sti(NPChar.MoneyForStorage))
			{
				link.l1 = "What are you... mercantile. Keep your money-I'm renting this barn.";
				link.l1.go = "storage_11";
			}
			else
			{
				link.l1 = "What are you... mercantile. I'll get the money now.";
				link.l1.go = "exit";
			}
		break;

		case "storage_rent3":
			dialog.text = "Well, as you know. If you decide to contact us. Just keep in mind that such a treasure will not be empty for a long time. No matter how late you are.";
			link.l1 = "Don't worry. If necessary, I'll ask.";
			link.l1.go = "exit";
		break;

		case "GetTradeInfo":
			dialog.text = "For a small financial reward, it is quite possible. I would like 15,000 piastres for this information. What do you say?";
			if (sti(pchar.money) >= 15000)
			{
				Link.l1 = "Okay, here's your money.";
				link.l1.go = "GetTradeInfo2";
			}
			Link.l2 = "I've changed my mind. Have a nice day.";
			link.l2.go = "exit";
		break;

		case "GetTradeInfo2":
			ref getcolony;
			int t;
			switch (sti(NPChar.nation))
			{
				case ENGLAND:
					for (t = 0; t< MAX_COLONIES; t++)
					{
						getcolony = &Colonies[t];
						if (getcolony.id != "Caiman" && getcolony.id != "Panama" && getcolony.nation != "none" && sti(getcolony.nation) != SPAIN && sti(getcolony.nation) != HOLLAND && sti(getcolony.nation) != PIRATE) SetPriceListByStoreMan(getcolony);
					}
				break;
				case FRANCE:
					for (t = 0; t< MAX_COLONIES; t++)
					{
						getcolony = &Colonies[t];
						if (getcolony.id != "Caiman" && getcolony.id != "Panama" && getcolony.nation != "none" && sti(getcolony.nation) != SPAIN && sti(getcolony.nation) != PIRATE) SetPriceListByStoreMan(getcolony);
					}
				break;
				case SPAIN:
					for (t = 0; t< MAX_COLONIES; t++)
					{
						getcolony = &Colonies[t];
						if (getcolony.id != "Caiman" && getcolony.id != "Panama" && getcolony.nation != "none" && sti(getcolony.nation) != ENGLAND && sti(getcolony.nation) != FRANCE && sti(getcolony.nation) != PIRATE) SetPriceListByStoreMan(getcolony);
					}
				break;
				case HOLLAND:
					for (t = 0; t< MAX_COLONIES; t++)
					{
						getcolony = &Colonies[t];
						if (getcolony.id != "Caiman" && getcolony.id != "Panama" && getcolony.nation != "none" && sti(getcolony.nation) != ENGLAND && sti(getcolony.nation) != PIRATE) SetPriceListByStoreMan(getcolony);
					}
				break;
				case PIRATE:
					for (t = 0; t< MAX_COLONIES; t++)
					{
						getcolony = &Colonies[t];
						if (getcolony.id != "Caiman" && getcolony.id != "Panama" && getcolony.nation != "none" && sti(getcolony.nation) != ENGLAND && sti(getcolony.nation) != FRANCE && sti(getcolony.nation) != SPAIN && sti(getcolony.nation) != HOLLAND) SetPriceListByStoreMan(getcolony);
					}
				break;

			}
			log_info("A list of prices of friendly colonies of the Caribbean Archipelago has been received.");
			PlayStereoSound("notebook");
			AddMoneyToCharacter(pchar,-15000);

			dialog.text = "...Here you go, all the information you need is here. I wish you success!";
			Link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

	}
	LanguageCloseFile(iSeaGoods);
}

int findStoreMan(ref NPChar, int delta)
{
    ref ch;
	aref chNat;
	int n;
    int storeArray[30];
    int howStore = 0;
	bool ok;
		for (int i = 0; i < MAX_CHARACTERS; i++)
		{
			makeref(ch,Characters[i]);
			makearef(chNat,Characters[i].Nation);
			if (CheckAttribute(ch, "Dialog.Filename") && ch.Dialog.Filename == "Common_Store.c")
			{
				if (NPChar.id == ch.id) continue;
				if (NPChar.id == "Panama_trader" || ch.id == "Panama_trader") continue; //нельзя доплыть
				if (NPChar.id == "Caiman_trader" || ch.id == "Caiman_trader") continue;
				if (ch.location == "none") continue; // фикс для новых, невидимых до поры островов
				if (GetIslandByCityName(ch.city) == GetIslandByCityName(NPChar.city)) continue; // хрен вам, а не читы!
				ok = (GetNationRelation2MainCharacter(sti(chNat)) == RELATION_ENEMY) && (sti(chNat) != PIRATE);
				if (GetNationRelation(sti(chNat), sti(NPChar.Nation)) != RELATION_ENEMY && !ok)
				{
					storeArray[howStore] = i;
					howStore++;
				}
			}
		}
		if (howStore == 0)
		{
			return -1;
		}
		else
		{
			n = drand(howStore - 3) + delta;
			return storeArray[n];
		}
}

// ugeen -->
//--> расчёт аренды склада
int GetStoragePriceExt(ref NPChar, ref chref)
{


	int price = makeint(2000 * MOD_SKILL_ENEMY_RATE*3);

	return price;
}
//<-- расчёт аренды склада
// --> перемещаем остатки груза со склада на корабль ГГ с перегрузом, пусть сам разбирается что делать со всем этим барахлом
void SetStorageGoodsToShip(ref pStorage)
{
	int iStoreQ;
	for (int i = 0; i< GOODS_QUANTITY; i++)
	{
		iStoreQ = GetStorageGoodsQuantity(pStorage, i);
		if (iStoreQ == 0) continue;
		SetCharacterGoods(pchar, i, GetCargoGoods(pchar, i) + iStoreQ);// перегруз
	}
}
