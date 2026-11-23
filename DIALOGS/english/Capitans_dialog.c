// разговор с капитаном на палубе  Boal
//homo 25/06/06
#include "DIALOGS\english\Rumours\Common_rumours.c"

string coolstory1[6] = {"Charleston", "Charleston", "Charlestown", "Charlestown", "Charlestown", "Charlestown"};
string coolstory2[4] = {"hornigold", "hornigold", "hornigold", "hornigold"};

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, Diag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);
    //homo Осады
    aref aData;
    makearef(aData, NullCharacter.Siege);
    string sCap, sGroup;
    string fort;
	string coolstories = "";
	int questionsstate = 0;

    if (CheckAttribute(aData, "nation"))
    {
	    sCap = NationShortName(sti(aData.nation))+"SiegeCap_";
	    sGroup = "Sea_"+sCap+"1";


        switch(sti(aData.conation))
        {
            case 0:  fort = "The English fort"; break;
            case 1:  fort = "The French fort"; break;
            case 2:  fort = "The Spanish fort"; break;
            case 3:  fort = "Dutch fort"; break;
            case 4:  fort = "Pirate Fort"; break;
        }
        int ifortPower = sti(colonies[FindColony(aData.colony)].FortValue);
        int fortDamage = CheckFortInjuri();
        int SquadronDamage = CheckSquadronInjuri();

    }

    int iMoney;
	ref sld;

    if (CheckNPCQuestDate(npchar, "Card_date"))
	{
		SetNPCQuestDate(npchar, "Card_date");
		npchar.money = 3000 + rand(10) * makeint(100 * sti(PChar.rank) * (10.0 / MOD_SKILL_ENEMY_RATE*3));
	}
    //homo 26/06/06
	ProcessCommonDialogRumors(NPChar, Link, Diag);
	if (npchar.id == "BlackBeardNPC" && CheckAttribute(npchar, "met") && Dialog.CurrentNode == "First time") Dialog.CurrentNode = "Titch_7";
	switch(Dialog.CurrentNode)
	{
        case "Exit":
			DialogExit();
			Diag.CurrentNode = Diag.TempNode;
		break;

		case "First time":
			if (!bDisableMapEnter)
			{
				if (npchar.id == "BlackBeardNPC")
				{
					dialog.text = "What the hell do you want here? (pulls out a gun)";
					link.l1 = "Calm down, old man! My name is " + GetFullName(Pchar) + ". I heard that you are looking for a lucky captain.";
					link.l1.go = "Titch_1";
					link.l2 = "Hey hey! I'm already leaving, calm!";
					link.l2.go = "exit";
					if (pchar.questTemp.AnjelikaTichPrologue9_BlackBeard == "ATP9")	//Sinistra Анжелика Тич и Эдвард Тич (в будущем поменяем)
					{
						link.l1 = "Calm down, old man! My name is Angelica. I heard that you are looking for a lucky captain.";
						link.l1.go = "AnjelikaTitch_1";
					}
					break;
				}
                if (CheckAttribute(NPChar, "EncType")  && NPChar.EncType == "war")
                {
Dialog.text = TimeGreeting() + ", "+ GetAddress_Form(NPChar)+"! I am "+ GetFullName(NPChar) +" - captain of navy "+NationKingsName(NPChar)+". What brings you aboard my ship?";
    			}
    			else
                {
Dialog.text = TimeGreeting() + ", "+ GetAddress_Form(NPChar)+"! I am captain " + GetFullName(NPChar)+". What brings you aboard my ship?";
    			}
    			link.l1 = "My name is " + GetFullName(Pchar) + ". I have a matter for you!";
    			link.l1.go = "quests";
                link.l2 = "Nothing special, just dropped by to welcome you!";
    			link.l2.go = "exit";
            }
            else
            {
Dialog.text = "You noticed, " + GetAddress_Form(NPChar) + ", that there a battle going on? We don't have time to talk right now!";
    			link.l1 = "You're right! I'm going back to my ship.";
    			link.l1.go = "exit";
            }
			if (npchar.id == "TohmasPetit_TomasShip" && CheckAttribute(npchar, "TohmasPetit_Talked"))
			{
				dialog.text = "Captain, I no longer have any business with you. I must ask you to leave my ship.";
				link.l1 = "Very well, Captain. Farewell.";
				link.l1.go = "exit";
				link.l2 = "Alright, alright, don't get your knickers in a twist. I'm already leaving.";
				link.l2.go = "exit";
				npchar.dialog.currentnode = "First time";
				Diag.TempNode = "First time"; // Борода: без этого case "Exit" перезатирает CurrentNode старым TempNode
				break;
			}
			Diag.TempNode = "first time";
 		break;

        case "quests":
            if (!CheckAttribute(NPChar, "MainCaptanId")  || NPChar.MainCaptanId == pchar.GenQuest.CaptainId)
            {
Dialog.text = "I'm listening to you carefully, " + GetAddress_Form(NPChar)+".";
                link.l1 = "I wanted to know the latest news of the archipelago.";
                //homo 25/06/06
                link.l1.go = "rumours_capitan";
                //
                if (CheckAttribute(NPChar, "EncGroupName")) // только для фантомов грабёж
                {
                    link.l2 = RandPhraseSimple("Do you know who I am? I think it's clear that my ship's guns can make a sieve out of your trough. Let's settle this peacefully. The ransom will suit me fine.",
                                               "The sea... And we are alone... How about we appease my guys a little bit for your safety?");
                    link.l2.go = "Talk_board";
                }
                if (CheckAttribute(NPChar, "Ship.Mode") && NPChar.Ship.Mode == "Trade")
                {
	           		link.l3 = "Where are you coming from?";
	    			link.l3.go = "price_1";
	    				link.l4 = "Shall we trade?";
	    				if (SeaTrade_RepTooLow()) link.l4.go = "Trade_RepBad"; // LEO: плохая репутация ГГ -> отказ в торговле
	    				else link.l4.go = "Trade_1";
    			}
				//--> квест мэра, поиски пирата
				if (CheckAttribute(pchar, "GenQuest.DestroyPirate"))
				{
	    			link.l6 = "I'm looking for a pirate ship operating in these waters. Did you see anything suspicious?";
	    			link.l6.go = "MQ_step_1";
				}
				//<-- квест мэра, поиски пирата
				//--> Квест ***Клан Ламбрини*** Sinistra
				if (CheckAttribute(pchar, "questTemp.PDM_CL_Ishem") && npchar.name == "Antonio" && npchar.lastname == "de Galvez")
				{
	    			link.l10 = "The Lambrini family sends greetings to you! (attack)";
	    			link.l10.go = "Antonio_Bitva";
					link.l11 = "Someone wants to kill you. (betray Lambrini)";
	    			link.l11.go = "Antonio_1_4";
				}
				//<-- Квест ***Клан Ламбрини*** Sinistra
				//--> Квест ***Странные вещи творятся на архипелаге*** Sinistra
				if (CheckAttribute(pchar, "questTemp.PKM_SvtvA_SprositKapitanov_v_more"))
				{
	    			link.l10 = "I am interested in the black frigate that left the port last night. Do you know anything about him?";
	    			link.l10.go = "PKM_SvtvA_Deti_Plachut";
				}
				//<-- Квест ***Странные вещи творятся на архипелаге*** Sinistra
    			link.l5 = "Do you want to unwind?";
			    link.l5.go = "Play_Game";
				if (npchar.id == "TohmasPetit_TomasShip")
				{
					link.l12 = "Greetings, Captain. My visit is strictly business. Rumor has it that a certain Thomas Petit and his companion Madeline LeBlanc are passengers on your sloop. I need you to hand them over to me.";
					link.l12.go = "TohmasPetit_Ship_Exposition";
				}
                link.l9 = "I think I have to go!";
    			link.l9.go = "exit";
			}
			else
			{
Dialog.text = "I am not authorized to resolve matters with you, contact the commander of our squadron. His name is " + GetFullName(characterFromID(NPChar.MainCaptanId))+
                              ", he's on the " + XI_ConvertString(RealShips[sti(characters[GetCharacterIndex(NPChar.MainCaptanId)].Ship.Type)].BaseName) + " '" + characters[GetCharacterIndex(NPChar.MainCaptanId)].Ship.Name + "'.";
                link.l1 = "Thank you. That's what I'll do.";
                link.l1.go = "exit";
                Diag.TempNode = "Go_away_Good";
			}
        break;
		//--> Квест ***Странные вещи творятся на архипелаге*** Sinistra
		case "PKM_SvtvA_Deti_Plachut":
			dialog.text = "I have no idea... but several of my men, who were watching him, assured me that when this ship weighed anchor, they heard a child crying coming to them from the deck.";
			link.l1 = "Understood. Okay, I have to go now.";
			link.l1.go = "PKM_SvtvA_Deti_Plachut_2";
		break;
		case "PKM_SvtvA_Deti_Plachut_2":
			DialogExit();
			DeleteAttribute(pchar, "questTemp.PKM_SvtvA_SprositKapitanov_v_more");
			AddQuestRecord("PKM_Animists", "5");
			AddQuestUserData("PKM_Animists", "sSex", GetSexPhrase("", "a"));
		break;
		//<-- Квест ***Странные вещи творятся на архипелаге*** Sinistra
		//--> Квест ***Клан Ламбрини*** Sinistra
		case "Antonio_Bitva":
			bDisableFastReload = true;
			chrDisableReloadToLocation = true;
			LAi_LocationFightDisable(loadedLocation, false);
			AddDialogExitQuest("MainHeroFightModeOn");
			LAi_group_SetRelation("", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("", LAI_GROUP_PLAYER, false);
			LAi_group_SetCheck("", "PDM_CL_Lodka");
			DialogExit();
		break;

		case "Antonio_1_4":
			dialog.text = "And what of it? If you wanted to surprise me, it didn't work out. Lambrini wants to kill me, but like the rest of the smugglers, they wouldn't mind.";
			link.l1 = "I came to you with an offer. What would you say if I gave you the opportunity to capture or sink Lambrini's ship?";
			link.l1.go = "Antonio_1_6";
			link.l2 = "Well, then, goodbye.";
			link.l2.go = "exit";
		break;

		case "Antonio_1_6":
			dialog.text = "I would give my right hand for this. But this is not possible, alas. He has already bribed, it seems, every customs officer in the Caribbean. What can I do alone?";
			link.l1 = "Lambrini offered me to help him get rid of you. He is sure that he cannot cope with you alone...";
			link.l1.go = "Antonio_1_7";
		break;

		case "Antonio_1_7":
			dialog.text = "If you work for smugglers, then you and I have nothing to talk about, "+ GetSexPhrase("senor", "senorita") +"...";
			link.l1 = "Uh, wait - you didn't even listen to me. I can tell Lambrini that I drowned your ship. After that, sooner or later he will sail here, thinking that he is no longer in danger. And you can waylay him in the bay, or near it.";
			link.l1.go = "Antonio_1_8";
		break;

		case "Antonio_1_8":
			dialog.text = "Curious... This plan may work, "+ GetSexPhrase("senor", "senorita") +" "+pchar.name+". And if it works, I will ensure that the senor governor generously rewards you!";
			link.l1 = "And how much do you think a Lambrini head might be worth?";
			link.l1.go = "Antonio_1_9";
		break;

		case "Antonio_1_9":
			int Bonus = sti(pchar.rank);
			if (Bonus <= 6) Bonus = 1;
			if (Bonus >= 7 && Bonus <= 12) Bonus = 1.2;
			if (Bonus >= 13) Bonus = 1.5;
			int Plata2 = 20000 + 2000 * sti(pchar.rank) * Bonus;
			dialog.text = "I guess it's about " + Plata2 + ". Our income has dropped a lot lately because of the smugglers.";
			link.l1 = "I think that " + Plata2 + " is a sufficient fee. So, I'll tell Octavio Lambrini that you're done... When he gets here, we'll grab him here. What could be simpler?";
			link.l1.go = "Antonio_1_10";
		break;

		case "Antonio_1_10":
			dialog.text = "As soon as you tell Lambrini I'm dead - come back to me immediately, I'll be waiting for you at the church from 9 am to 1 pm. We'll figure out what to do next.";
			link.l1 = "Take care of yourself, senor. Good luck!";
			link.l1.go = "Antonio_1_11";
		break;

		case "Antonio_1_11":
			sld = CharacterFromID("PDM_Octavio_Lambrini")
			sld.Dialog.Filename = "Quest\PDM\Clan_Lambrini.c";
			sld.dialog.currentnode   = "Octavio_3_1";
			DeleteAttribute(pchar, "questTemp.PDM_CL_Ishem");
			PChar.quest.PDM_CL_Antonio_Ubit.over = "yes";
			AddQuestRecord("PDM_Clan_Lambrini", "4");
			AddQuestUserData("PDM_Clan_Lambrini", "sSex", GetSexPhrase("", "a"));
			AddQuestUserData("PDM_Clan_Lambrini", "sSex2", GetSexPhrase("ся", "ась"));

			DialogExit();
		break;
		//<-- Квест ***Клан Ламбрини*** Sinistra
        case "Trade_RepBad":
			dialog.text = "Trade? With you? Your ill fame runs ahead of you across the whole archipelago. Honest folk want no dealings with your sort. Be gone.";
			link.l1 = "Have it your way.";
			link.l1.go = "quests";
        break;

        case "Trade_1":
			dialog.text = "Why not?";
			link.l1 = "Let's go see the goods.";
			link.l1.go = "Trade_2";
			link.l2 = "I changed my mind.";
			link.l2.go = "exit";
		break;

		case "Trade_2":
			FillShipStore(NPChar);
	    	pchar.shiptrade.character = NPChar.id;
   			Diag.CurrentNode = Diag.TempNode;
		    DialogExit();
		    DeleteAttribute(pchar, "PriceList.StoreManIdx"); // не вести лог по кораблям
		    LaunchStore(SHIP_STORE);
		break;

        case "Play_Game":
			dialog.text = "What do you suggest?";
			link.l1 = "Shall we switch to cards for a solid interest?";
			link.l1.go = "Card_Game";
   			link.l2 = "Roll the dice in a poker joker?";
			link.l2.go = "Dice_Game";
			link.l10 = "I'm sorry, but I have things to do.";
			link.l10.go = "exit";
		break;
        // карты -->
        case "Card_Game":
            if (!CheckNPCQuestDate(npchar, "Card_date_Yet") || isBadReputation(pchar, 30) || GetCharacterSkillToOld(pchar, SKILL_LEADERSHIP) < 1+rand(2))
            {
                SetNPCQuestDate(npchar, "Card_date_Yet");
                dialog.text = "I will not gamble with different crooks!";
    			link.l1 = "As you wish.";
    			link.l1.go = "exit";
			}
			else
			{
                dialog.text = "Come on! I'll take a little break from shipboard business...";
    			link.l1 = "Wonderful.";
    			link.l1.go = "Cards_begin";
    			link.l2 = "What are the rules of the game?";
    			link.l2.go = "Cards_Rule";
			}
		break;

		case "Cards_Rule":
   			dialog.text = XI_ConvertString("CardsRules")
			link.l1 = "Well, let's get started!";
			link.l1.go = "Cards_begin";
			link.l3 = "No, it's not for me...";
			link.l3.go = "exit";
		break;

		case "Cards_begin":
Dialog.text = "Let's decide on the bid.";
			link.l1 = "Let's play 100 gold per card.";
			link.l1.go = "Cards_Node_100";
			link.l2 = "Playing 500 gold per card.";
			link.l2.go = "Cards_Node_500";
			link.l3 = "I think I should go.";
			link.l3.go = "exit";
		break;

		case "Cards_Node_100":
		    if (sti(pchar.Money) < 300)
		    {
                dialog.text = "Are you kidding? You don't have any money!";
                link.l1 = "It happens.";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 300)
		    {
                dialog.text = "That's enough for me, otherwise there won't be enough left for the maintenance of the ship...";
                link.l1 = "Sorry.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Well, let's play for 100 piastres.";
			link.l1 = "Let's get started!";
			link.l1.go = "Cards_begin_go";
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 100;
            pchar.GenQuest.Cards.SitType   = false;
		break;

		case "Cards_Node_500":
		    if (sti(pchar.Money) < 1500)
		    {
                dialog.text = "You don't have enough money!";
                link.l1 = "They will!";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 1500)
		    {
                dialog.text = "No, such bets will not do any good.";
                link.l1 = "Whatever.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Well, let's play for 500 piastres.";
			link.l1 = "Let's get started!";
			link.l1.go = "Cards_begin_go";
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 500;
            pchar.GenQuest.Cards.SitType   = false;
		break;

		case "Cards_begin_go":
            Diag.CurrentNode = Diag.TempNode;
			DialogExit();
            LaunchCardsGame();
		break;
	    // карты <--
	    //  Dice -->
        case "Dice_Game":
            if (!CheckNPCQuestDate(npchar, "Dice_date_Yet") || isBadReputation(pchar, 30) || GetCharacterSkillToOld(pchar, SKILL_LEADERSHIP) < 1+rand(2))
            {
                SetNPCQuestDate(npchar, "Dice_date_Yet");
                dialog.text = "I will not gamble with different crooks!";
    			link.l1 = "As you wish.";
    			link.l1.go = "exit";
			}
			else
			{
				if (CheckNPCQuestDate(npchar, "Dice_date_begin"))
				{
					dialog.text = "Come on! Rest has never been harmful to health... only the wallet...";
	    			link.l1 = "Wonderful.";
	    			link.l1.go = "Dice_begin";
	    			link.l2 = "What are the rules of the game?";
	    			link.l2.go = "Dice_Rule";
    			}
    			else
    			{
					dialog.text = "No, I've had enough for today. Things are waiting.";
	    			link.l1 = "As you wish.";
	    			link.l1.go = "exit";
				}
			}
		break;

		case "Dice_Rule":
   			dialog.text = XI_ConvertString("DiceRules");
			link.l1 = "Well, let's get started!";
			link.l1.go = "Dice_begin";
			link.l3 = "No, it's not for me...";
			link.l3.go = "exit";
		break;

		case "Dice_begin":
Dialog.text = "Let's decide on the bid.";
			link.l1 = "Playing 50 gold per dice.";
			link.l1.go = "Dice_Node_100";
			link.l2 = "Let's play 200 coins per dice.";
			link.l2.go = "Dice_Node_500";
			link.l3 = "I think I should go.";
			link.l3.go = "exit";
		break;

		case "Dice_Node_100":
            if (!CheckDiceGameSmallRate())
		    {
                dialog.text = "Fame is unsurpassed about you, cheater. I won't play dice with you at all.";
                link.l1 = "Everyone is lying! Well, you don't need to.";
			    link.l1.go = "exit";
			    break;
		    }

			if (sti(pchar.Money) < 300)
		    {
                dialog.text = "Are you kidding? You don't have any money!";
                link.l1 = "It happens.";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 300)
		    {
                dialog.text = "Everything! We need to stop playing games, otherwise they will write us down as embezzlers and write us ashore...";
                link.l1 = "Sorry.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Well, let's play for 50 coins.";
			link.l1 = "Let's get started!";
			link.l1.go = "Dice_begin_go";
			pchar.GenQuest.Dice.npcharIdx = npchar.index;
            pchar.GenQuest.Dice.iRate     = 50;
            pchar.GenQuest.Dice.SitType   = false;
		break;

		case "Dice_Node_500":
            if (!CheckDiceGameSmallRate())
		    {
                dialog.text = "Fame is unsurpassed about you, cheater. I won't play dice with you at all.";
                link.l1 = "Everyone is lying! Well, you don't need to.";
			    link.l1.go = "exit";
			    break;
		    }
			if (!CheckDiceGameBigRate())
		    {
                dialog.text = "I heard that you play very well. I won't play with you at such high stakes.";
                link.l1 = "Let's get lower rates?";
			    link.l1.go = "Dice_Node_100";
				link.l2 = "Sorry, I have to go.";
			    link.l2.go = "exit";
			    break;
		    }

			if (sti(pchar.Money) < 1500)
		    {
                dialog.text = "Are you kiddnig me? You dont have enough money!";
                link.l1 = "They will!";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 1500)
		    {
                dialog.text = "No, such bets will not do any good.";
                link.l1 = "Whatever.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Well, let's play for 200 coins.";
			link.l1 = "Let's get started!";
			link.l1.go = "Dice_begin_go";
			pchar.GenQuest.Dice.npcharIdx = npchar.index;
            pchar.GenQuest.Dice.iRate     = 200;
            pchar.GenQuest.Dice.SitType   = false;
		break;

		case "Dice_begin_go":
            SetNPCQuestDate(npchar, "Dice_date_begin");
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
            LaunchDiceGame();
		break;
	    // Dice <--
        case "price_1":
			if (CheckNPCQuestDate(npchar, "trade_date"))
			{
                SetNPCQuestDate(npchar, "trade_date");
				iMoney = findPriceStoreMan(NPChar);
	            if (iMoney == -1)
	            {
	                Dialog.Text = "Why would you want to know that? This is my personal business.";
					Link.l1 = "As you wish.";
					Link.l1.go = "exit";
	            }
	            else
	            {
                    sld = &Characters[iMoney];
                    pchar.PriceList.ShipStoreIdx = iMoney;

					Dialog.Text = "I'm coming from the city " + GetCityName(sld.City) + ".";
					Link.l1 = "What are the prices of the goods in the local store?";
					Link.l1.go = "price_2";
					Link.l9 = "I have to go now.";
					Link.l9.go = "exit";
				}
			}
			else
			{
                Dialog.Text = "I've already told you everything. Don't bother me with trifles.";
				Link.l1 = "Yes, you're right. Goodbye.";
				Link.l1.go = "exit";
            }
		break;

		case "price_2":
            sld = &Characters[sti(pchar.PriceList.ShipStoreIdx)];
			SetPriceListByStoreMan(&Colonies[FindColony(sld.City)]);
			Dialog.Text = "These are... (you have received a summary of the prices of goods).";
			Link.l1 = "Thank you so much!";
			Link.l1.go = "exit";
			PlaySound("interface\important_item.wav");
		break;

        case "Talk_board":
            if ((Group_GetCharactersNum(NPChar.EncGroupName) - Group_GetDeadCharactersNum(NPChar.EncGroupName)) > GetCompanionQuantity(PChar) && rand(11) > GetCharacterSkillToOld(PChar, SKILL_FORTUNE))
            {
Dialog.text = "Ha ha! It's a good joke. Especially since I have more ships. Get on your ship and sink with it.";
                link.l1 = "Bigger does not mean more powerful...";
                link.l1.go = "Boarding";
            }
            else
            {
                if(rand(21) > (GetSummonSkillFromNameToOld(PChar, SKILL_GRAPPLING) + GetSummonSkillFromNameToOld(PChar, SKILL_LEADERSHIP)) )
                {
Dialog.text = "You shouldn't have started your dirty business, Captain, aboard my ship. I will show mercy and let you return to your ship and drown with it.";
                    link.l1 = "We'll see who goes to feed the sharks today!";
                    link.l1.go = "Boarding";
                }
                else
                {
                    Pchar.GenQuest.MoneyForCaptureShip = makeint(100 + (Group_GetCharactersNum(NPChar.EncGroupName) - Group_GetDeadCharactersNum(NPChar.EncGroupName))*(7 - sti(RealShips[sti(NPChar.Ship.Type)].Class))*(1+rand(10))*500);
Dialog.text = RandSwear() + "Yes, I've heard a lot about your outrages. Have it your way, but remember that " +
                                  XI_ConvertString(NationShortName(sti(NPChar.nation))+"hunter") + " ain't gonna leave it easy!";
                    link.l1 = "Wonderful. The amount in "+Pchar.GenQuest.MoneyForCaptureShip+" gold is fine with me, "+GetAddress_FormToNPC(NPChar)+".";
                    link.l1.go = "Capture";
                    link.l2 = "Oh. Come on. I joked!";
                    link.l2.go = "exit";
                }
            }
            // тут нужен учёт запрета повторного наезда
            Diag.TempNode = "Go_away";
			sld = characterFromId(pchar.GenQuest.CaptainId);
            sld.talk_date_Go_away =  lastspeakdate(); // boal злопамятность :)
        break;

        case "rumours":
			Dialog.Text = SelectRumour(); // to_do
			Link.l1 = RandPhraseSimple(RandSwear() + "It's very interesting. Another question?",
                                     "One more thing.");
			Link.l1.go = "quests";
			Link.l2 = RandPhraseSimple("Thanks, I have to take my leave.",
                                     "Have a nice day.");
			Link.l2.go = "exit";
			Diag.TempNode = "quests";
		break;

		case "Go_away":
			Dialog.Text = "Get off my ship!";
			Link.l1 = "I'm leaving now.";
			Link.l1.go = "exit";
			Diag.TempNode = "Go_away";
		break;

		case "Go_away_Good":
			Dialog.Text = "Our conversation is already over. I won't tell you anything new, "+GetAddress_Form(NPChar)+".";
			Link.l1 = "I see. See you at sea!";
			Link.l1.go = "exit";
			Diag.TempNode = "Go_away_Good";
		break;

        case "Capture":
            Diag.TempNode = "Go_away";
            Dialog.Text = "Get it! Now get out!";
			Link.l1 = "Wonderful!";
			Link.l1.go = "exit";
			AddMoneyToCharacter(pchar, sti(Pchar.GenQuest.MoneyForCaptureShip));
			ChangeCharacterReputation(pchar, -5);
			AddCharacterExpToSkill(pchar, "Leadership", 20);
			ChangeCharacterHunterScore(pchar, NationShortName(sti(NPChar.nation)) + "hunter", 7 + rand(10));
        break;

        case "Boarding":
            if (CheckAttribute(NPChar, "MainCaptanId"))
            {
                PChar.StartBattleAfterDeck = true;
                PChar.StartBattleMainCaptanId = NPChar.MainCaptanId;
                PChar.StartBattleEncGroupName = NPChar.EncGroupName;
            }
            DialogExit();
            Diag.CurrentNode = Diag.TempNode;
        break;

        case "QuestAboardCabinDialog":  // тестовый диалог, в игре не работает, для метода SetQuestAboardCabinDialog
			Diag.TempNode = "QuestAboardCabinDialog";
            Dialog.Text = "Wait, you can kill like that. What do you want from me?";
			Link.l1 = "Kill!";
			Link.l1.go = "QuestAboardCabinDialog_1";
			Link.l2 = "Capture.";
			Link.l2.go = "QuestAboardCabinDialog_2";
			Link.l3 = "Give us the map, Billy (c)";  // предмет, далее не ясно или  QuestAboardCabinDialog_1 или QuestAboardCabinDialog_2
			Link.l3.go = "QuestAboardCabinDialog_3";  // можно закодить покидание корабля, но экипаж уже вырезан и тп...
        break;

        case "ContraHunterCabin":  // captain of the contraband-freight pirate hunters surrenders in his cabin
			Diag.TempNode = "ContraHunterCabin";
			Dialog.Text = RandSwear() + RandPhraseSimple(
				"Hold, don't cut me down! You win... And here I was, hoping for easy pickings. I hunt your sort - the ones hauling someone else's cargo.",
				"Enough, I yield! Damn... you were carrying a fine prize. I've been lying in wait for couriers like you - easy money, I thought...");
			Link.l1 = "Easy pickings, eh? Choke on it!";
			Link.l1.go = "QuestAboardCabinDialog_Finish"; // finish off the wounded captain (fight, no heal)
			Link.l2 = "Consider yourself lucky to be alive today. Surrender.";
			Link.l2.go = "ContraHunterCabin_Spare"; // mercy - leadership & sneak bonus
		break;

        case "ContraHunterCabin_Spare":  // sparing the hunter captain: reward for mercy
			Dialog.Text = "All right, you win! Don't kill me...";
			Link.l1 = "Live, then. Consider you've bought yourself another day.";
			Link.l1.go = "exit";
			AddCharacterExpToSkill(pchar, "Leadership", 150); // +leadership for mercy
			AddCharacterExpToSkill(pchar, "Sneak", 100);      // +sneak
			AddDialogExitQuestFunction("QuestAboardCabinDialogSurrender");
		break;

        case "GentlemenHunterCabin":  // captain of the governor-freight interceptors surrenders in his cabin (several variants, lore - hired to intercept the cargo)
			Diag.TempNode = "GentlemenHunterCabin";
			Dialog.Text = RandSwear() + RandPhraseSimple(
				RandPhraseSimple(
					"Hold, don't cut me down! You win... We were sent to intercept your cargo - precious freight it must be, since they pay so well for it. Didn't work out.",
					"Enough, I yield! Damn... All they told me was to lie in wait for the ship with the special cargo and seize it. Picked the wrong prey, it seems."),
				RandPhraseSimple(
					"Wait, easy there! I'll admit it, you win. It wasn't my own will that set me on you - I was paid to keep your cargo from reaching its destination.",
					"That's enough, you win! Don't kill me... I had a contract - intercept an important shipment. Don't ask who hired me; such names aren't spoken aloud."));
			Link.l1 = "Thought you'd intercept my cargo? Go to the devil!";
			Link.l1.go = "QuestAboardCabinDialog_Finish"; // finish off the wounded captain (fight, no heal)
			Link.l2 = "Consider yourself lucky to be alive today. Surrender.";
			Link.l2.go = "GentlemenHunterCabin_Spare"; // mercy - prisoner + exp
		break;

        case "GentlemenHunterCabin_Spare":  // sparing the interceptor captain: take prisoner + exp for mercy
			Dialog.Text = RandPhraseSimple(
				"All right, you win! Don't kill me...",
				"Thank you for my life, captain. Consider me in your debt.");
			Link.l1 = "Live, then. Consider you've bought yourself another day.";
			Link.l1.go = "exit";
			int ghRew = 100; // escort interceptors (GovEscortHalfReward) give half - 50; set in SetQuestAboardCabinDialog
			if (CheckAttribute(pchar, "GenQuest.GentlemenReward")) ghRew = sti(pchar.GenQuest.GentlemenReward);
			AddCharacterExpToSkill(pchar, "Sneak", ghRew);      // +sneak for mercy
			AddCharacterExpToSkill(pchar, "Leadership", ghRew); // +leadership for mercy
			AddDialogExitQuestFunction("QuestAboardCabinDialogSurrender");
		break;

        case "FortHunterCabin":  // privateer (commandant-freight interceptor) surrenders in his cabin; lore - letter of marque and war
			Diag.TempNode = "FortHunterCabin";
			Dialog.Text = RandSwear() + RandPhraseSimple(
				RandPhraseSimple(
					"Hold, sheathe your blade! You win... I carry a letter of marque: intercepting the enemy's military cargo. Bad luck I ran into you.",
					"Enough, I yield! The Admiralty pays good coin for every military shipment seized... Seems that money will go to someone else now."),
				RandPhraseSimple(
					"Wait, don't strike! I admit defeat. There's a war on, captain - your ammunition was never meant to reach that fort. I was only following orders.",
					"That's it, you win! We've trailed your cargo from the very port - the enemy garrison wasn't to get a single keg of powder. It didn't work out."));
			Link.l1 = "Hunting my cargo, were you? War is war, then. Go to the devil!";
			Link.l1.go = "QuestAboardCabinDialog_Finish"; // finish off the wounded captain (fight, no heal)
			Link.l2 = "Consider yourself lucky to be alive today. Surrender.";
			Link.l2.go = "FortHunterCabin_Spare"; // mercy - prisoner + exp (same as ContraHunter)
		break;

        case "GuildEnforcerCabin":  // captain of the Guthrie Trading House (freight forfeit collection) surrenders in his cabin
			Diag.TempNode = "GuildEnforcerCabin";
			Dialog.Text = RandSwear() + RandPhraseSimple(
				"Hold, don't strike!.. You win. You have crossed the Guthrie Trading House - and God knows, you'd have done better to simply pay.",
				"Enough, I yield! We are no pirates - we sail under the Guthrie pennant. The trade of the whole archipelago rests on that family, and forgiving debtors is not their habit. Pay up, and none of ours will stand in your way again.");
			Link.l1 = "Tell the guild: I will return everything within a month. You have my word.";
			Link.l1.go = "GuildEnforcerCabin_Word";
			Link.l2 = "Consider yourself lucky to be alive today. Surrender.";
			Link.l2.go = "GuildEnforcerCabin_Spare";
			Link.l3 = "Debt? What debt? Go to the bottom for your pay!";
			Link.l3.go = "QuestAboardCabinDialog_Finish"; // finish off the wounded captain (fight, no heal), the debt remains
		break;

        case "GuildEnforcerCabin_Word":  // gave the word to repay within a month: the captain leaves by longboat to deliver it, the squadron is recalled
			Dialog.Text = "A captain's word is no empty sound - the Guthrie House will accept it. The ship is yours - by right of boarding; leave me the longboat, and I shall deliver your word. One month, " + GetAddress_FormToNPC(NPChar) + ". Not a day more. Fail to repay - and there will be no mercy: not for you, nor for your word.";
			Link.l1 = "Agreed. The longboat is yours - you'll do the rowing yourself.";
			Link.l1.go = "exit";
			GuildDebt_GiveWord();
			AddDialogExitQuestFunction("QuestAboardCabinDialogNotBattle"); // капитан НЕ пленник: ушёл на шлюпке с твоим словом
		break;

        case "GuildEnforcerCabin_Spare":  // mercy without the word: prisoner, but the debt and the waves remain
			Dialog.Text = "All right, you win! Don't kill me... But know this: the Guthrie House will send others. A debt is a debt.";
			Link.l1 = "Let them come. Live.";
			Link.l1.go = "exit";
			AddCharacterExpToSkill(pchar, "Leadership", 100); // +leadership for mercy
			AddDialogExitQuestFunction("QuestAboardCabinDialogSurrender");
		break;

        case "FortHunterCabin_Spare":  // sparing the privateer captain: take prisoner + exp for mercy
			Dialog.Text = RandPhraseSimple(
				"All right, you win! Don't kill me...",
				"Thank you, captain. War is war, but a debt of honour I shall remember.");
			Link.l1 = "Live, then. Consider you've bought yourself another day.";
			Link.l1.go = "exit";
			AddCharacterExpToSkill(pchar, "Leadership", 50); // +leadership for mercy
			AddCharacterExpToSkill(pchar, "Sneak", 50);      // +sneak
			AddDialogExitQuestFunction("QuestAboardCabinDialogSurrender");
		break;

        case "GWIK_MutineerCabin":  // Gregg: the runaway GWIC captain (mutiny 'threaten' branch), boarded at sea - the player decides his fate
			Diag.TempNode = "GWIK_MutineerCabin";
			Dialog.Text = "Hold your blade!.. You win, Dutchman. So the Company sent you to drag me back? Go on then - clap me in irons and haul me before them like a dog, or run me through here and be done with it.";
			Link.l1 = "You'll answer to the Company. Surrender - you're my prisoner.";
			Link.l1.go = "GWIK_MutineerCabin_Prisoner";
			Link.l2 = "A traitor's end for a traitor. Die.";
			Link.l2.go = "GWIK_MutineerCabin_Kill";
		break;

        case "GWIK_MutineerCabin_Prisoner":  // Gregg: take him alive - delivered as a passenger to any GWIC office
			Dialog.Text = "...So be it. A cell beats the deep. Lead on.";
			Link.l1 = "In irons. Move.";
			Link.l1.go = "exit";
			pchar.GWIK.Job.MutinyFate = "prisoner"; // read in GWIK_RansomBoarded (utils.c)
			AddCharacterExpToSkill(pchar, "Leadership", 100); // command xp for taking the runaway alive
			AddDialogExitQuestFunction("QuestAboardCabinDialogQuestSurrender"); // Gregg: quest capture - takes the ONE real captain (stores GenQuest.LastQuestPrisonerIdx) + sets DontRansackCaptain so the ransack form won't produce a SECOND prisoner; caught by NPC_Death -> GWIK_RansomBoarded
		break;

        case "GWIK_MutineerCabin_Kill":  // Gregg: cut him down - no prisoner; report the execution at a GWIC office for a bounty
			Dialog.Text = "Then we'll settle it the sailor's way!";
			Link.l1 = "We will.";
			Link.l1.go = "exit";
			pchar.GWIK.Job.MutinyFate = "killed"; // read in GWIK_RansomBoarded (utils.c) - no prisoner spawned
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattle"); // no heal - resumes the fight at ~10 HP and dies on the next blow
		break;

        case "FortAgentCabin":  // enemy-nation courier whose offers were refused, then boarded: bargains for the convoy
			Diag.TempNode = "FortAgentCabin";
			Dialog.Text = RandSwear() + RandPhraseSimple(
				"Stay your blade! You win... but we both know you'll not deliver this cargo in peace. Let us do it otherwise: give me back the convoy ships and spare me a handful of your sailors to crew them - and I'll pay fivefold the guild's price, here and now.",
				"Enough, I yield! Here's the bargain: the convoy is mine, and a few of your hands to crew it. The gold is yours, fivefold what the guild promised. We part, each with his own.");
			Link.l1 = "Deal. Take the convoy and the men. (" + FindRussianMoneyString(sti(pchar.CargoQuest.iMoney) * 5) + ")";
			Link.l1.go = "FortAgentCabin_Deal";
			Link.l2 = "No. This cargo reaches its address, and you meet your maker.";
			Link.l2.go = "QuestAboardCabinDialog_Finish"; // kill - fight, the convoy stays with the player
		break;

        case "FortAgentCabin_Deal":  // accepted: x5 + convoy + a handful of crew to the agent, he leaves; no exp / no nation rep
			Dialog.Text = "Wise, captain. My men will take the ships, and here is the gold - fivefold, as agreed. Your sailors I'll return once we make landfall... perhaps. Farewell.";
			Link.l1 = "Get out.";
			Link.l1.go = "exit";
			AddDialogExitQuestFunction("FortAgent_CabinDeal");             // x5 + convoy + crew + agent keeps his ship and sails off (no exp/rep, NO capture form)
		break;

        case "SmugRevengeCabin":  // мститель берегового братства (за вырезанных смуглеров после откупа от патруля): пощады нет
			Diag.TempNode = "SmugRevengeCabin";
			Dialog.Text = RandSwear() + RandPhraseSimple(
				RandPhraseSimple(
					"You shouldn't have started this, captain... The Coastal Brotherhood does not forgive the slaughter of its own. Today you win - but others will come after us. Again and again, until the sea takes you.",
					"Kh-he... You thought you'd cut down that gang in the bay and the water would hide it all? The Brotherhood has long arms and a long memory. You shouldn't have started this..."),
				RandPhraseSimple(
					"Damn you... Those lads on the shore traded with you fair and square - and you butchered them like sheep. You shouldn't have started this, captain.",
					"Wait... let me speak. We came for your blood to pay for our brothers from the shore. If not us - others will collect. You shouldn't have started this, oh no..."));
			Link.l1 = "Give my regards to the Brotherhood. At the bottom of the sea."; // пощады нет - ни ему, ни тебе
			Link.l1.go = "SmugRevengeCabin_1";
		break;

        case "SmugRevengeCabin_1":  // возмездие без пощады: добиваем, -10 к отношению контрабандистов за каждого капитана
			Dialog.Text = "Well, we'll see about that!";
			Link.l1 = "What's there to see?";
			Link.l1.go = "exit";
			ChangeContrabandRelation(pchar, -10);
			ChangeCharacterReputation(pchar, -5); // добить сдавшегося - удар по репутации ГГ
			AddCharacterExpToSkill(pchar, "Leadership", 100); // +авторитет: одолеть аса братства дорогого стоит
			AddCharacterExpToSkill(pchar, "Sneak", 200);      // +скрытность: концы мстителей - в воду
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattle");
		break;

        case "QuestAboardCabinDialog_1":
			Dialog.Text = "Well, we'll see about that!";
			Link.l1 = "What's there to see?";
			Link.l1.go = "exit";
			// тут можно восстановить НР противника (NPChar) или добавить парочку гардов против ГГ
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattle"); // тут ругань обратно
		break;

        case "QuestAboardCabinDialog_Finish":  // LEO: finish off the wounded captain - fight WITHOUT restoring HP (unlike _1 with LAi_SetCurHPMax). Captain stays near death - one hit finishes him. For freight hunters: "if you don't want to take him prisoner".
			Dialog.Text = "Well, we'll see about that!";
			Link.l1 = "What's there to see?";
			Link.l1.go = "exit";
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattle"); // no heal - captain resumes the fight at ~10 HP
		break;

		case "QuestAboardCabinDialog_2":
			Dialog.Text = "Okay, you got it!";
			Link.l1 = "So it would be right away!";
			Link.l1.go = "exit";
			AddDialogExitQuestFunction("QuestAboardCabinDialogSurrender");
		break;

		case "QuestAboardCabinDialog_3":
			Dialog.Text = "Yes, take it!";
			Link.l1 = "That's good, now surrender.";
			if (rand(1) == 1)  // пример!!!
			{
				Link.l1.go = "QuestAboardCabinDialog_2";
			}
			else
			{
			    Link.l1.go = "QuestAboardCabinDialog_3_1";
			}
			TakeNItems(NPChar, "Chest", -1);
			TakeNItems(pchar, "Chest", 1);
		break;

		case "QuestAboardCabinDialog_3_1":
			Dialog.Text = ""+ "You came, made all around dirty, took everything away and gonna take me as a slave? Fuck you!";
			Link.l1 = "Then prepare the white sneakers.";
			Link.l1.go = "QuestAboardCabinDialog_1";
		break;
		//eddy. квест мэра, поиски пирата
		case "MQ_step_1":
			Dialog.Text = "No, I haven't seen any suspicious pirate-like ship.";
			Link.l1 = "I see.";
			Link.l1.go = "exit";
		break;

		//homo Наводка на купца
		case "MerchantTrap_Abordage":

			Dialog.Text = "In the name of "+NationKingsName(NPChar)+" - give up, "+ GetSexPhrase("sneaky pirate", "sneaky villainess") +"! And you will be spared your life until a fair trial, where you will answer for every "+NationNameSK(sti(NPChar.nation))+" ships that you robbed.";
			Link.l1 = "No matter how it is, give up yourself.";
			Link.l1.go = "exit";
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattleNoParam"); // тут ругань обратно
		break;

		case "Siegehelp":

            ref rchar = Group_GetGroupCommander(sGroup);
            if (NPChar.id == rchar.id)
            {

    			dialog.text = "I'm listening to you, but let's quickly lay out the purpose of your visit, we are fighting with " + fort +
                          " and I need to take care of battle!";
                link.l1 = "Thats why I'm here, " + GetAddress_FormToNPC(NPChar) + ".";
                link.l1.go = "attack_fort";

                link.l2 = "In that case, I won't keep you any longer. Goodbye, "+ GetAddress_FormToNPC(NPChar) + ".";
                link.l2.go = "exit";

            }
            else
            {
Dialog.text = "I am not authorized to resolve matters with you, contact the commander of our squadron. His name is " + GetFullName(rchar)+
                              ", he is on the " + XI_ConvertString(RealShips[sti(rchar.Ship.Type)].BaseName) + " '" + rchar.Ship.Name + "'.";
                link.l1 = "Thank you. That's what I'll do.";
                link.l1.go = "exit";

            }
            Diag.TempNode = "Siegehelp";

		break;

		case "attack_fort":
                dialog.text = "And what is it about?";
                link.l1 = "I can help you defeat the colony fort" +GetConvertStr(aData.Colony+" Town", "LocLables.txt")+ "and capture the city, and the loot obtained in case of our success, we will divide among ourselves.";
                link.l1.go = "Siegehelp_1";
                link.l2 = "Actually, my case hardly deserves your attention. Goodbye, "+ GetAddress_FormToNPC(NPChar) + ".";
                link.l2.go = "exit";
                Diag.TempNode = "Siegehelp_0";
		break;

		case "Siegehelp_0":
Dialog.text = "Is it you again? I thought we had already discussed everything?";
            link.l1 = "You're right - I forgot it.";
			link.l1.go = "exit";
			NPChar.DeckDialogNode = "Siegehelp_0";
            Diag.TempNode = "Siegehelp_0";

		break;

		case "Siegehelp_1":
            SiegeResult("");
            if (sti(aData.win)==0)
            {
                dialog.text = "And what part of the loot do you expect to get?";
                link.l1 = "Half.";
                link.l1.go = "Big_part";
                link.l2 = "One third.";
                link.l2.go = "Middle_part";
                link.l3 = "The fourth part.";
                link.l3.go = "Small_part";
            }
            else
            {
                dialog.text = "I don't need your services, Captain! I don't need anyone's help, because I'm going to take over this colony anyway.";
                link.l1 = "How can you be so sure, " + GetAddress_FormToNPC(NPChar) + ", failures can befall anyone.";
                link.l1.go = "attack_fort_03";
                link.l2 = "In that case, I won't keep you any longer, " + GetAddress_FormToNPC(NPChar) + ". Goodbye!";
                link.l2.go = "exit";
            }
			Diag.TempNode = "Siegehelp_0";

		break;

		case "attack_fort_03":


            if((sti(aData.iSquadronPower)*SquadronDamage - (ifortPower)*fortDamage) < 4000 || bBettaTestMode) // LEO: беттатест — 100% согласие
            {
                dialog.text = "On the other hand, my ships have already been pretty battered in this battle, and we could use reinforcements. If you agree to " + GetPart(4) + ", then I think we can come to an agreement.";
                link.l1 = "It suits me, " + GetAddress_FormToNPC(NPChar) + ". I will try to take the fort by storm, and you, for your part, will support us with the fire of the ship's guns.";
                link.l1.go = "attack_fort_04";
                aData.PartAttaksFort = 4;
                link.l2 = "I'm not that cheap, " + GetAddress_FormToNPC(NPChar) + ". I'm sorry to interrupt your important business. Goodbye.";
                link.l2.go = "exit";
            }else{
                dialog.text = "There are enough ships in my squadron to smash this fort to smithereens, it's only a matter of time. And now deign to leave my ship, victory awaits me ahead!";
                link.l1 = "Well, in that case, " + GetAddress_FormToNPC(NPChar) + ", let me take my leave. Goodbye!";
                link.l1.go = "exit";
            }
		    Diag.TempNode = "Siegehelp_0";
		break;
		case "Big_part":
            if( (sti(aData.iSquadronPower)*SquadronDamage - (ifortPower)*fortDamage) <= -1500 || bBettaTestMode) // LEO: беттатест — 100% согласие
            {
                dialog.text = "The guns of the fort severely damaged the ships of my squadron, and the outcome of this battle may not turn in our favor. I agree to your terms and accept your help.";
                link.l1 = "Excellent, " + GetAddress_FormToNPC(NPChar) + ", in that case, we will take the fort by storm, and you will support us with the fire of the ship's cannons.";
                link.l1.go = "attack_fort_04";
                aData.PartAttaksFort = 2;
            }
            else
            {
                dialog.text = "Isn't that a bit much, Captain? You have " + GetNumShips(GetCompanionQuantity(PChar)) +
                              ", in my squadron " + GetNumShips(Group_GetCharactersNum(sGroup)) + ". I can offer you " + GetPart(4) +
                              ". And I think it's the right thing to do.";
                aData.PartAttaksFort = 4;
                link.l1 = "I agree, " + GetAddress_FormToNPC(NPChar) + ", in that case, I will take over the capture of the fort, and you will support us with the fire of the ship's guns.";
                link.l1.go = "attack_fort_04";
                link.l2 = "Unfortunately, I don't think so, " + GetAddress_FormToNPC(NPChar) + ". We have nothing more to talk about. Goodbye.";
                link.l2.go = "exit";
            }
            Diag.TempNode = "Siegehelp_0";
        break;

        case "attack_fort_04":
            dialog.text = "As soon as the fort is captured, we will immediately begin landing landing groups on the shore, and then we will continue the battle in the city itself, where, I hope, we will meet with you.";
            link.l1 = "Let's stay on this. See you soon " + GetAddress_FormToNPC(NPChar) + "!";
            link.l1.go = "exit";
            Diag.TempNode = "Siegehelp_0";
            PChar.quest.Union_with_Escadra = "Yes";
        break;

        case "Middle_part":
            if((sti(aData.iSquadronPower)*SquadronDamage - (ifortPower)*fortDamage) <= -600 || bBettaTestMode) // LEO: беттатест — 100% согласие
            {
                dialog.text = "I think I'll agree to your terms. The joint operation increases my chances of a successful outcome of this battle. If the case is successfully completed, you will receive your share of the loot.";
                link.l1 = "Well, it's nice that we agreed, " + GetAddress_FormToNPC(NPChar) + ". I will immediately order my men to attack the fort.";
                link.l1.go = "attack_fort_04";
                aData.PartAttaksFort = 3;
            }else{
                dialog.text = "Isn't that a bit much, Captain? You have " + GetNumShips(GetCompanionQuantity(PChar)) + ", in my squadron " + GetNumShips(Group_GetCharactersNum(sGroup)) + ". I can offer you " + GetPart(4) + ". And I think it's the right thing to do.";
                link.l1 = "I agree, " + GetAddress_FormToNPC(NPChar) + ", in that case, I will take over the storming of the fort, and you will support us with the fire of the ship's guns.";
                link.l1.go = "attack_fort_04";
                aData.PartAttaksFort = 4;
                link.l2 = "Unfortunately, I don't think so, " + GetAddress_FormToNPC(NPChar) + ". We have nothing more to talk about. Goodbye.";
                link.l2.go = "exit";
            }
            Diag.TempNode = "Siegehelp_0";
        break;

        case "Small_part":
            dialog.text = "The ships of my squadron are exhausted from the battle, so fresh forces will not interfere with us. I agree to these terms and accept your offer.";
            link.l1 = "I am glad that we were able to agree, " + GetAddress_FormToNPC(NPChar) + ". I will immediately order my men to engage and launch an attack on the fort.";
            link.l1.go = "attack_fort_04";
            aData.PartAttaksFort = 4;
            Diag.TempNode = "Siegehelp_0";
        break;

		case "Talk_Capture_City":
            AfterTownBattle();  // все, все свободны
            LAi_LoginInCaptureTown(NPChar, false);
            aData.win = 1;
            EndOfTheSiege("End");
            NPChar.location = "";
            int ilt = makeint(sti(aData.loot)/sti(aData.PartAttaksFort));
            dialog.text = "Hello, Captain! Although the battle was hot, we still managed to break the fierce resistance of the defenders of the city. The goods is " + sti(aData.loot)+
                          "gold. And according to our agreement, you get "+ilt+
                          ". Here's your money, you certainly deserve it. Now, if you'll excuse me, I have some business to attend to.";
            link.l1 = "In that case, let me take my leave, " + GetAddress_FormToNPC(NPChar) + ". Goodbye!";
            link.l1.go = "exit";

            Diag.TempNode = "no_return_02";
            AddMoneyToCharacter(PChar, ilt);
            ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 20);
            ChangeCharacterReputation(PChar, 10);
            AddCharacterExpToSkill(GetMainCharacter(), "Leadership", 80);
            AddCharacterExpToSkill(GetMainCharacter(), "Sailing", 100);
            AddCharacterExpToSkill(GetMainCharacter(), "Defence", 100);
            AddCharacterExpToSkill(GetMainCharacter(), "Accuracy", 100);
            AddCharacterExpToSkill(GetMainCharacter(), "Cannons", 100);
            AddCharacterExpToSkill(GetMainCharacter(), "Fortune", 80);
            aData.loot = sti(aData.loot) - ilt;
            if (CheckAttribute(PChar, "quest.LeaveTown")) Pchar.quest.LeaveTown.over = "yes";
            //--> слухи
            SiegeRumour("There was a rumor that you helped our squadron during the assault "+NationNameSK(sti(aData.conation))+"oh colony - "+GetConvertStr(aData.Colony+" Town", "LocLables.txt")+"! We are so grateful to you, "+ GetAddress_Form(NPChar)+".", "", sti(aData.nation), -1, 30, 3);
            //<-- слухи
		break;

        case "no_return_02":
            dialog.text = "Greetings, Captain! Do you have business with me?";
            link.l1 = "None, " + GetAddress_FormToNPC(NPChar) + ". Goodbye!";
            link.l1.go = "exit";
            Diag.TempNode = "no_return_02";
		break;

		case "GoldSquadron":
            ref rch = Group_GetGroupCommander("Sea_Head_of_Gold_Squadron");
            if (NPChar.id == rch.id)
            {
    			dialog.text = "I'm listening to you, but let's tell you the purpose of your visit as soon as possible, I'm in a hurry. I have an important mission of a secret nature assigned to me, and besides, I am ordered to sink all suspicious ships on the high seas that may pose a danger to our convoy.";

                link.l1 = "In that case, I won't keep you any longer. Goodbye, "+ GetAddress_FormToNPC(NPChar) + ".";
                link.l1.go = "exit";

            }
            else
            {
Dialog.text = "I am not authorized to resolve matters with you, contact the commander of our squadron. His name is " + GetFullName(rch)+
                              ", he is on the " + XI_ConvertString(RealShips[sti(rch.Ship.Type)].BaseName) + " '" + rch.Ship.Name + "'.";
                link.l1 = "Thank you. That's what I'll do.";
                link.l1.go = "exit";

            }
            Diag.TempNode = "GoldSquadron";
		break;
		
		case "AnjelikaTitch_1":		//Sinistra Анжелика Тич и Эдвард Тич
Dialog.text = "Angelica? I had a daughter, Angelica. Her lascivious mother snatched my baby right from under her nose. She ran off with some bastard... And, damn it, if I had managed to find them then, I would have missed both of them under the keel.";
			link.l1 = "I heard a completely different story.";
			link.l1.go = "AnjelikaTitch_2";
		break;
		
		case "AnjelikaTitch_2":
Dialog.text = "I don't care what they say about me, are you here on business? So listen up: It's time for me to retire with dignity and transfer my knowledge and skills to my successor. But I have to make sure he deserves it.";
			link.l1 = "Your successor? And what falls under the criteria of dignity?";
			link.l1.go = "Titch_2";
			DeleteAttribute(pchar, "questTemp.AnjelikaTichPrologue9_BlackBeard");
		break;

		case "Titch_1":
Dialog.text = "There is such a thing. It's time for me to retire with dignity and transfer my knowledge and skills to my successor. But I have to make sure he deserves it.";
			link.l1 = "Your successor? And what falls under the criteria of dignity?";
			link.l1.go = "Titch_2";
		break;

		case "Titch_2":
Dialog.text = "In my best years, as a pirate, I had no equal and therefore my successor should be no worse.";
			link.l1 = "I'm listening...";
			link.l1.go = "Titch_3";
		break;

		case "Titch_3":
			if (!CheckAttribute(pchar, "ShipsSurr")) pchar.ShipsSurr = 0;
Dialog.text = "You have to board 5 merchant ships and force them to surrender so that captains beg you to spare them. As soon as they saw my pennant, the opponents usually surrendered without a fight. Anyway, after I became more famous in these waters.";
			link.l1 = "It will be fast, or very difficult. However, I will make the rich beg for mercy! You'll see!";
			link.l1.go = "Titch_4";
		break;

		case "Titch_4":
Dialog.text = "I need to check your swordsmanship level, because this is the foundation of any self-respecting pirate or corsair. If luck has bitten you in the ass, then I want to personally check if this is the case. Let's try to play twenty-one. If you lose, then my gun will bite you for...";
			link.l1 = "I understand... Someone really bit my ass. Will you check it out? And fencing is mine... I'll smash anyone! But only in a fair duel. I hope she will be honest.";
			link.l1.go = "Titch_5";
		break;

		case "Titch_5":
Dialog.text = "To make legends about you, you must be able to compose them. I want to hear what you know about me. A real corsair should have a really sharp mind and a sober head.";
			link.l1 = "Well... I've heard a lot about you. Whether it's true or not, you'll confirm it yourself.";
			link.l1.go = "Titch_6";
		break;

		case "Titch_6":
Dialog.text = "And finally, the naval battle. No one will help you here or come to your aid. We will fight to the death!";
			link.l1 = "You have a very impressive frigate... It definitely doesn't sound like something simple.";
			link.l1.go = "Titch_7";
		break;

		case "Titch_7":
			npchar.met = true;
Dialog.text = "Well?! And what do you say?";
			if (!CheckAttribute(npchar, "Duel"))
			{
				link.l1 = "I think I have the strength to leave a history of cuts and bruises on your body. Where and when will we fight?";
				link.l1.go = "Titch_duel";
			}
			if (!CheckAttribute(npchar, "Cards"))
			{
				link.l2 = "Shall we play cards?";
				link.l2.go = "Titch_cards";
			}
			if (!CheckAttribute(npchar, "questions"))
			{
				link.l3 = "Test me for knowledge about you. Unless these are childish riddles.";
				link.l3.go = "Titch_questions";
			}
			if (!CheckAttribute(npchar, "ships_sur") && CheckAttribute(pchar, "ShipsSurr") && sti(pchar.ShipsSurr) >= 5)
			{
				link.l4 = "I made five merchant ships beg for mercy.";
				link.l4.go = "Titch_ships_sur";
			}
			if (CheckAttribute(npchar, "Duel") && CheckAttribute(npchar, "Cards") && CheckAttribute(npchar, "questions") && CheckAttribute(npchar, "ships_sur") && !CheckAttribute(pchar, "Titch_seabattle") && !CheckAttribute(pchar, "questTemp.BlackBeardRaidStarted"))
			{
				link.l5 = "Only the battle at sea remains. I'm ready!";
				link.l5.go = "Titch_seabattle";
			}
			link.l6 = "Nothing yet.";
			link.l6.go = "exit";
			if (bBettaTestMode)
			{
				link.l7 = "[БЕТА] Морское сражение";
				link.l7.go = "Titch_seabattle";
			}
			
		break;

		case "Titch_seabattle":
			if (GetCompanionQuantity(PChar) >= COMPANION_MAX)
			{
				Dialog.text = "Not now. Free a slot in your squadron, then we'll go on this raid.";
				link.l1 = "Understood.";
				link.l1.go = "exit";
			}
			else
			{
				if (!CheckAttribute(pchar, "questTemp.BlackBeardRaidStartIsland") || !BlackBeard_IsGoodRaidIsland(pchar.questTemp.BlackBeardRaidStartIsland)) pchar.questTemp.BlackBeardRaidStartIsland = BlackBeard_GetRandomUninhabitedIsland();
				Dialog.text = "Since you've handled all my terms, I have one more job for you. There is a gold caravan circling the uninhabited island of " + BlackBeard_GetRaidIslandName(pchar.questTemp.BlackBeardRaidStartIsland) + ". Let's take it with my squadron, and then we'll talk about a pirate's share.";
				link.l1 = "I'm in. Lead the way.";
				link.l1.go = "Titch_seabattle_7days";
				link.l2 = "Not now.";
				link.l2.go = "exit";
			}
		break;

		case "Titch_seabattle_7days":
			Dialog.text = "But don't drag your feet! We have exactly 7 days. If you don't reach the island in time — the convoy will sail into open sea, and that'll be the end of it.";
			link.l1 = "Understood. We move immediately.";
			link.l1.go = "Titch_seabattle_exit";
		break;

		case "Titch_seabattle_exit":
			DialogExit();
			Diag.CurrentNode = Diag.TempNode;
			AddDialogExitQuestFunction("BlackBeard_StartGoldRaid");
		break;

		case "Titch_Seabattle_won":
			Dialog.text = "Well then, looks like this is it. You handled the caravan, and I misjudged the hand.";
			link.l1 = "So that's why you turned on us?";
			link.l1.go = "Titch_Seabattle_won_2";
		break;

		case "Titch_Seabattle_won_2":
			Dialog.text = "Such is the pirate's lot. Today you're an ally, tomorrow you're the prey. Don't take it too hard.";
			link.l1 = "So it goes...";
			link.l1.go = "Titch_Seabattle_won_3";
		break;

		case "Titch_Seabattle_won_3":
Dialog.text = "Just tell me one of the characteristics, don't ask stupid questions.";
			if (AddSPECIALValue(pchar, SPECIAL_S, 0) != SPECIAL_MAX)
			{
				Link.l1 = "Power";
				Link.l1.go = "Strength";
			}
			if (AddSPECIALValue(pchar, SPECIAL_P, 0) != SPECIAL_MAX)
			{
				Link.l2 = "Impression";
				Link.l2.go = "Perception";
			}
			if (AddSPECIALValue(pchar, SPECIAL_A, 0) != SPECIAL_MAX)
			{
				Link.l3 = "Reaction";
				Link.l3.go = "Agility";
			}
			if (AddSPECIALValue(pchar, SPECIAL_C, 0) != SPECIAL_MAX)
			{
				Link.l4 = "Authority";
				Link.l4.go = "Charisma";
			}
			if (AddSPECIALValue(pchar, SPECIAL_I, 0) != SPECIAL_MAX)
			{
				Link.l5 = "Talent";
				Link.l5.go = "Intellect";
			}
			if (AddSPECIALValue(pchar, SPECIAL_E, 0) != SPECIAL_MAX)
			{
			Link.l6 = "Endurance";
			Link.l6.go = "Endurance";
			}
			if (AddSPECIALValue(pchar, SPECIAL_L, 0) != SPECIAL_MAX)
			{
				Link.l7 = "Success";
				Link.l7.go = "Luck";
			}
			Link.l8 = "Everything suits me as it is. What now?";
			Link.l8.go = "Titch_Seabattle_won_4";
		break;

		case "Strength":
			dialog.text =  "Power, good choice. Ready! You'll notice the difference soon.";
			Link.l1 = "And now what?";
			Link.l1.go = "Titch_Seabattle_won_4";
			AddSPECIALValue(pchar, SPECIAL_S, 1);
		break;
        case "Perception":
			dialog.text =  "Impression, very good. Ready! You'll notice the difference soon.";
			Link.l1 = "And now what?";
			Link.l1.go = "Titch_Seabattle_won_4";
			AddSPECIALValue(pchar, SPECIAL_P, 1);
		break;
        case "Endurance":
			dialog.text =  "Endurance, good. Ready! You'll notice the difference soon.";
			Link.l1 = "And now what?";
			Link.l1.go = "Titch_Seabattle_won_4";
			AddSPECIALValue(pchar, SPECIAL_E, 1);
		break;
        case "Charisma":
			dialog.text =  "Authority? Hmm, a good choice. Ready! You'll notice the difference soon.";
			Link.l1 = "And now what?";
			Link.l1.go = "Titch_Seabattle_won_4";
			AddSPECIALValue(pchar, SPECIAL_C, 1);
		break;
        case "Intellect":
			dialog.text =  "Talent is a good choice. Ready! You'll notice the difference soon.";
			Link.l1 = "And now what?";
			Link.l1.go = "Titch_Seabattle_won_4";
			AddSPECIALValue(pchar, SPECIAL_I, 1);
		break;
        case "Agility":
			dialog.text =  "Reaction, good. Ready! You'll notice the difference soon.";
			Link.l1 = "And now what?";
			Link.l1.go = "Titch_Seabattle_won_4";
			AddSPECIALValue(pchar, SPECIAL_A, 1);
		break;
        case "Luck":
			dialog.text =  "Do you want to be a lucky pirate? Good choice. Ready! You'll notice the difference soon.";
			Link.l1 = "And now what?";
			Link.l1.go = "Titch_Seabattle_won_4";
			AddSPECIALValue(pchar, SPECIAL_L, 1);
		break;

		case "Titch_Seabattle_won_4":
Dialog.text = "For you, life goes on. But I have to go. Now - goodbye forever! (It's all about how you put yourself! Fooling around for fun will convince only fools. And if you are the devil, everyone will submit to you.)";
			link.l1 = "I didn't understand anything, but goodbye, Edward Teach!";
			link.l1.go = "Titch_Seabattle_won_exit";
		break;

		case "Titch_Seabattle_won_exit":
			LAi_LocationFightDisable(loadedLocation, false);
			DeleteAttribute(&NullCharacter, "capitainBase.BlackBeardNPC");
			DeleteAttribute(pchar, "questTemp.BlackBeardRaidStarted");
			DeleteAttribute(pchar, "questTemp.BlackBeardRaidStartIsland");
			PostEvent("csmEvent_RefreshReload", 100);
			BattleInterface.LAi_ActivateReload = true;
			SetEventHandler("Control Activation", "LAi_ActivateReload",1);
			Log_SetActiveAction("Reload");
			ChangeCharacterAddress(npchar, "none", "");
            DialogExit();
			UnlockAchievement("AchTich", 3);
			pchar.questdata.ChernayaBoroda = 1;
		break;

		case "Titch_duel":
Dialog.text = "Ho ho! Yes, you're straight "+ GetSexPhrase("sea wolf", "sea lioness") +"! Here and now! I don't like any scraping and bowing.";
			link.l1 = "Hold on, old man..!";
			link.l1.go = "Titch_duel_start";
		break;

		case "Titch_duel_start":
			DialogExit();
			chrDisableReloadToLocation = true;
			Diag.CurrentNode = Diag.TempNode;
			LAi_group_MoveCharacter(npchar, "EnemyFight");
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_LocationFightDisable(loadedLocation, false);
			AddDialogExitQuest("MainHeroFightModeOn");

			for(int i=0;i<MAX_CHARACTERS;i++)
			{
				sld = &characters[i];
				if (CheckAttribute(sld, "location") && sld.location == loadedLocation.id && sld.id != "BlackBeardNPC" && sld.id != "Blaze")
				{
					LAi_SetActorType(sld);
					LAi_ActorRunToLocation(sld, "reload", "reload1", "", "", "", "", -1);
				}
			}

			LAi_SetCheckMinHP(npchar, 5.0, true, "Titch_duel");
		break;

		case "DuelWon":
Dialog.text = "I respect you, my friend, well done.";
			link.l1 = "To be honest, it wasn't easy, (that's the old, honest duel, honor... Yes, yes...). What's next?";
			link.l1.go = "Titch_7";
			npchar.Duel = true;
			DeleteAttribute(npchar, "chr_ai.poison");
			DeleteAttribute(npchar, "chr_ai.Blooding");
			LAi_SetImmortal(npchar,false);
			LAi_SetCitizenType(npchar);
		break;

		case "Titch_cards":
Dialog.text = "Playing cards? Keep in mind, the bid is 50,000 piastres and no less. So if your pockets are dry, go for a walk. Don't even bother with less than 500,000. Well, are you ready?";
			if (sti(pchar.money) >= 500000)
			{
				link.l1 = "Am I ready? What a stupid question, of course!";
				link.l1.go = "Titch_cards_go";
			}
			link.l2 = "Later.";
			link.l2.go = "exit";
		break;

		case "Titch_cards_go":
			npchar.money = 1000000;
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 50000;
            pchar.GenQuest.Cards.SitType   = false;
            Diag.CurrentNode = Diag.TempNode;
			DialogExit();
            LaunchCardsGame();
		break;

		case "Titch_questions":
			npchar.coolstories = "Charleston";
			npchar.questionsstate = 1;
Dialog.text = "All right, listen up. Since you've been digging into me and managed to find me, then you probably know about my biggest case. Which city have I besieged and forced to pay me a round sum?";
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Titch_check";
			link.l2 = "Later.";
			link.l2.go = "exit";
		break;
		case "Titch_check":
			if (npchar.questionsstate == 1)
			{
				bool check1 = false;
				for(int x=0;x<=5;x++)
				{
					if (coolstory1[x] == GetStrSmallRegister(dialogEditStrings[1])) check1 = true;
				}
				if (check1)
				{
					npchar.coolstories = "hornigold";
					npchar.questionsstate = 2;
Dialog.text = "That's right! Excellent awareness, respect. Listen to the next question! (I just want to be remembered...)";
					Link.l1 = "Come on.";
					Link.l1.go = "Titch_q2";
					break;
				}
			}
			if (npchar.questionsstate == 2)
			{
				bool check2 = false;
				for(int y=0;y<=3;y++)
				{
					if (coolstory2[y] == GetStrSmallRegister(dialogEditStrings[1])) check2 = true;
				}
				if (check2)
				{
					npchar.coolstories = "Concord";
					npchar.questionsstate = 3;
Dialog.text = "Bravo! I'm starting to like you for your charisma. And here's the last question.";
					Link.l1 = "And?";
					Link.l1.go = "Titch_q3";
					break;
				}
			}
			if (npchar.questionsstate == 3 && npchar.coolstories == GetStrSmallRegister(dialogEditStrings[1]))
			{
Dialog.text = "Amazing, you got to this point! I have no more questions for you.";
				Link.l1 = "Great!";
				Link.l1.go = "Titch_7";
				npchar.questions = true;
				break;
			}
			else
			{
Dialog.text = "Wrong, try again.";
				if (npchar.questionsstate == 1)
				{
					Link.l1 = "One more time.";
					Link.l1.go = "Titch_questions";
				}
				if (npchar.questionsstate == 2)
				{
					Link.l1 = "One more time.";
					Link.l1.go = "Titch_q2";
				}
				if (npchar.questionsstate == 3)
				{
					Link.l1 = "One more time.";
					Link.l1.go = "Titch_q3";
				}
				link.l2 = "Later.";
				link.l2.go = "exit";
			}
		break;
		case "Titch_q2":
Dialog.text = "Under whose command did I sail until I became known as Blackbeard?";
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Titch_check";
			link.l2 = "Later.";
			link.l2.go = "exit";
		break;
		case "Titch_q3":
Dialog.text = "What was the name of the ship Queen Anne's Revenge before?";
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Titch_check";
			link.l2 = "Later.";
			link.l2.go = "exit";
		break;

		case "Titch_ships_sur":
Dialog.text = "That's good! The archipelago will still be able to shake. (Scare your victim, create a creepy image out of a feverish delirium, and everyone will fall to their knees, begging their God for mercy.)";
			Link.l1 = "Thank you!";
			Link.l1.go = "Titch_7";
			npchar.ships_sur = true;
			DeleteAttribute(pchar, "ShipsSurr");
		break;
		
		case "TohmasPetit_Ship_Exposition":
			dialog.text = "Monsieur Petit and his lady? Yes, they showed up on my deck at the literal last moment before we weighed anchor. The lad forked out such a sum in gold for the passage that it would have been downright foolish to refuse. Admittedly, he seemed suspicious to me from the very start — all on edge, jumpy as a cat. We managed to talk a bit while the sailors were securing the rigging, and only then did I agree to take them aboard. But why do you demand their surrender so insistently?";
			link.l1 = "Because that man is a runaway and a thief.";
			link.l1.go = "TohmasPetit_Ship_MainCheck";
		break;
		
		case "TohmasPetit_Ship_MainCheck":
			npchar.TohmasPetit_Talked = true;
			
			bool bAuth = false;
			if (sti(pchar.SPECIAL.Charisma) > 6) 
			{
				bAuth = true;
			}
			if (sti(pchar.skill.LeaderShip) >= 50) 
			{
				bAuth = true;
			}
			bool bNoKills = !CheckAttribute(pchar, "QuestTemp.TohmasPetit_KilledAnyone");
			bool bGoodRep = (sti(pchar.reputation) >= 50);
			bool bBloody = (sti(pchar.reputation) <= 10);

			if (bNoKills)
			{
				if (bAuth && bGoodRep) 
				{
					dialog.text = "I have heard of you as an honest and respected man, Captain. No one would dare argue with your authority in these waters. Since you have come for this lad in person, you must have a serious score to settle with him. I have no desire to make an enemy out of you over a random passenger.";
					link.l1 = "I am glad we understand each other. How shall we proceed with them?";
					link.l1.go = "TohmasPetit_Cap_Peace_1";
				} 
				else 
				{
					dialog.text = "Why on earth should I take your word for it and hand people over? Anyone can say anything on the piers. Besides, I see no proof. You haven't convinced me, so I must ask you to leave my ship.";
					link.l1 = "Think carefully, Captain. You are protecting someone who isn't worth it. This might cost you dearly.";
					link.l1.go = "TohmasPetit_Cap_FailNoKills_1";
				}
			}
			else
			{
				if (bBloody) 
				{
					dialog.text = "I... I know who you are. You are a pirate, and your reputation as a bloody murderer, whose atrocities are whispered about in every tavern on the Main... No one has ever left you alive...";
					link.l1 = "I'm glad my reputation precedes me. Since you know who I am, you know I am in no mood for games. Where is Thomas?";
					link.l1.go = "TohmasPetit_Cap_Bloody_1";
				} 
				else 
				{
					dialog.text = "Thomas told me that you staged a total massacre while tracking him down! You are a butcher and a killer, I will not hand the boy and his bride over to you to be slaughtered. Leave my ship!";
					link.l1 = "You don't know the whole story, Captain. That scoundrel deserves everything that's coming to him.";
					link.l1.go = "TohmasPetit_Cap_FailKills_1";
				}
			}
		break;

		case "TohmasPetit_Cap_Peace_1":
			dialog.text = "I will order the sailors to bring them out on deck. Let them gather their belongings and transfer to your ship.";
			link.l1 = "A wise decision, Captain. I'm glad we managed this without a cannon fight. Farewell.";
			link.l1.go = "TohmasPetit_Cap_Peace_GoodExit";
		break;

		case "TohmasPetit_Cap_FailNoKills_1":
			dialog.text = "Enough threats on my own quarterdeck! On the 'Atlantica', I am the law. The conversation is over, get back to your ship before I order the crew to be called to arms!";
			link.l1 = "Well, have it your way then. Good day to you, Captain.";
			link.l1.go = "TohmasPetit_Cap_Attack_NoKillsExit";
		break;

		case "TohmasPetit_Cap_Bloody_1":
			dialog.text = "Take them! Take the lad and his wench Madeline, just spare my crew and don't touch the ship! They are in the hold, I will immediately order them into a longboat to be handed over to your men.";
			link.l1 = "A smart move. Unload them quicker, and perhaps your ship will stay afloat today.";
			link.l1.go = "TohmasPetit_Cap_Peace_FearExit";
		break;

		case "TohmasPetit_Cap_FailKills_1":
			dialog.text = "It is quite enough for me that blood has already been shed because of your hunt. I will not become a headsman's accomplice. Leave my ship at once, or we will feed you to the sharks right here!";
			link.l1 = "Your stubbornness will be the death of you. Farewell.";
			link.l1.go = "TohmasPetit_Cap_Attack_KillsExit";
		break;
		
		// --- Исходы диалогов ---

		case "TohmasPetit_Cap_Peace_GoodExit":
			DialogExit();
			NPChar.dialog.currentnode = "First time";
			AddQuestRecord("TohmasPetit", "TohmasPetit_CapPeaceGood");
			pchar.QuestTemp.TohmasPetit_Quest = "talk_in_hold";
			pchar.QuestTemp.TohmasPetit_SecuredPeacefully = true;
			AddDialogExitQuest("TohmasPetit_MoveToHold");

			pchar.quest.TohmasPetit_ClearShip_Case.win_condition.l1 = "ExitFromSea";
			pchar.quest.TohmasPetit_ClearShip_Case.win_condition = "TohmasPetit_ClearShip_Case";
			TohmasPetit_ReleaseShipEncounter(); // судьба судна решена - метку с глобалки снимаем сразу
		break;

		case "TohmasPetit_Cap_Peace_FearExit":
			DialogExit();
			NPChar.dialog.currentnode = "First time";
			AddQuestRecord("TohmasPetit", "TohmasPetit_CapPeaceFear");
			pchar.QuestTemp.TohmasPetit_Quest = "talk_in_hold";
			pchar.QuestTemp.TohmasPetit_SecuredPeacefully = true;
			AddDialogExitQuest("TohmasPetit_MoveToHold");
			pchar.QuestTemp.TohmasPetit_bloodytake = true; // LEO: flag "captives taken by intimidation (bloody reputation)" - statement was incomplete, never set

			pchar.quest.TohmasPetit_ClearShip_Case.win_condition.l1 = "ExitFromSea";
			pchar.quest.TohmasPetit_ClearShip_Case.win_condition = "TohmasPetit_ClearShip_Case";
			TohmasPetit_ReleaseShipEncounter(); // судьба судна решена - метку с глобалки снимаем сразу
		break;

		case "TohmasPetit_Cap_Attack_NoKillsExit":
			DialogExit();
			NPChar.dialog.currentnode = "First time";
			AddQuestRecord("TohmasPetit", "TohmasPetit_CapAttackNoKills");
			pchar.QuestTemp.TohmasPetit_CapTalked = "failed"; 
			
			pchar.quest.TohmasPetit_LetThemGo.win_condition.l1 = "ExitFromSea";
			pchar.quest.TohmasPetit_LetThemGo.win_condition = "TohmasPetit_LetThemGo";
			TohmasPetit_ReleaseShipEncounter(); // судьба судна решена - метку с глобалки снимаем сразу
		break;

		case "TohmasPetit_Cap_Attack_KillsExit":
			DialogExit();
			NPChar.dialog.currentnode = "First time";
			AddQuestRecord("TohmasPetit", "TohmasPetit_CapAttackKills");
			pchar.QuestTemp.TohmasPetit_CapTalked = "failed"; 
			
			pchar.quest.TohmasPetit_LetThemGo.win_condition.l1 = "ExitFromSea";
			pchar.quest.TohmasPetit_LetThemGo.win_condition = "TohmasPetit_LetThemGo";
			TohmasPetit_ReleaseShipEncounter(); // судьба судна решена - метку с глобалки снимаем сразу
		break;

		// Stub after talking to the "Atlantica" captain, so re-opening the dialog isn't empty and doesn't hang
		case "TohmasPetit_Cap_Deck_Stub":
			dialog.text = "Captain, I no longer have any business with you. I must ask you to leave my ship.";
			link.l1 = "Very well, Captain. Farewell.";
			link.l1.go = "exit";
			link.l2 = "Alright, alright, don't get your knickers in a twist. I'm already leaving.";
			link.l2.go = "exit";
		break;
	}
}

string GetNumShips(int ship)
{
    string numships;
    switch(ship)
    {
       case 0: numships = "not a single ship"; break;
       case 1: numships = "one ship";      break;
       case 2: numships = "two ships";       break;
       case 3: numships = "three ships";       break;
       case 4: numships = "four ships";    break;
       case 5: numships = "five ships";     break;
       case 6: numships = "six ships";    break;
    }
    return numships;
}

string GetPart(int part)
{
    string numpart;
    switch(part)
    {
       case 0: numpart = ""; break;
       case 1: numpart = "all the loot"; break;
       case 2: numpart = "half of the loot"; break;
       case 3: numpart = "a third of the loot"; break;
       case 4: numpart = "a quarter of the loot";      break;

    }
    return numpart;
}

int findPriceStoreMan(ref NPChar)
{
    ref ch;
	int n;
    int storeArray[40];
    int howStore = 0;

	for(n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(ch,Characters[n]);
		if (CheckAttribute(ch, "Dialog.Filename") && ch.Dialog.Filename == "Common_Store.c") // магазин
		{
            if (ch.location == "none") continue;
			if (ch.City == "Caiman") continue;			// фикс для новых, невидимых до поры островов
            storeArray[howStore] = n;
            howStore++;

            // homo 05/09/06
            if ( CheckAttribute(NPChar, "FromColony") && NPChar.FromColony == ch.City ) return n;
            //
		}
    }
    if (howStore == 0)
    {
        return -1;
    }
    else
    {
        return storeArray[rand(howStore-1)];
    }
}
