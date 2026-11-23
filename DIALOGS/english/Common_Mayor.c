// boal 25/04/04 общий диалог мэры
#include "DIALOGS\english\Rumours\Common_rumours.c"  //homo 25/06/06

void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref aData;
	aref Link, NextDiag, arName;
	int enemyNation;
	string city2;

	DeleteAttribute(&Dialog,"Links");

	ref FortChref; // фортовый командер
	ref arItem;

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Mayor\" + NPChar.City + "_Mayor.c";
    if (!HasCityDialogFile("Mayor", NPChar.City, "_Mayor.c")) NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Mayor\DefaultCity_Mayor.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
	else
	{
		NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Mayor\DefaultCity_Mayor.c";
	    if (LoadSegment(NPChar.FileDialog2))
		{
	        ProcessCommonDialog(NPChar, Link, NextDiag);
			UnloadSegment(NPChar.FileDialog2);
		}
	}
    // вызов диалога по городам <--
    ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 25/06/06

    FortChref = GetFortCommander(NPChar.City); // будет или фортовик или сам губернатор

    ref offref;
    int i, cn, iRank, iDay;
    int qty;
    string attrLoc, QuestName, sTemp;
	bool   bOk;
	bool bOk2;

    if (CheckNPCQuestDate(npchar, "Card_date"))
	{
		SetNPCQuestDate(npchar, "Card_date");
		npchar.money = 5000 + (rand(10)+1) * makeint(100 * sti(PChar.rank) * (10.0 / MOD_SKILL_ENEMY_RATE*3));
	}
	// генератор ИДХ по кейсу -->
	attrLoc   = Dialog.CurrentNode;
  	if (findsubstr(attrLoc, "GetPrisonerIdx_" , 0) != -1)
 	{
        i = findsubstr(attrLoc, "_" , 0);
	 	pchar.GenQuest.GetPrisonerIdx = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 	    Dialog.CurrentNode = "sell_prisoner_2";
 	}
 	// генератор ИДХ по кейсу <--
	// подставляем квестовую ноду при захвате города, если она есть

	switch (Dialog.CurrentNode)
	{
		string sNationname = GetNationNameByType(sti(npchar.nation));
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOff");
		break;

		case "fight":
            Pchar.quest.ArestInResidenceEnd.win_condition.l1          = "ExitFromLocation";
		    Pchar.quest.ArestInResidenceEnd.win_condition.l1.location = Pchar.location;
		    Pchar.quest.ArestInResidenceEnd.win_condition             = "ArestInResidenceEnd";
		    Pchar.quest.ArestInResidenceEnd.ResidenceLocation = Pchar.location;
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;

			LAi_LockFightMode(Pchar, true); // ножками путь убегает
		    LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false);
		    LAi_group_Attack(NPChar, Pchar); // не работает на бессмертного мера :(
			i = GetCharIDXByParam("CityType", "location", Pchar.location); // фантом солдат
			if (i != -1)
			{
			    LAi_group_Attack(&Characters[i], Pchar);
			}
		break;

		case "First time":
            NextDiag.TempNode = "First time";
            // LEO: Fernando Torres (Des Moines mayor) recognizes the savior after the Enchanted City quest
            if (NPChar.id == "Fernando Torres")
            {
            	if (CheckAttribute(pchar, "questTemp.MC.DesMoines"))
            	{
            		dialog.text = "You... it is you! Our savior! Without you there would be neither this city nor myself. How may I serve you?";
            		link.l1 = "Do you have any work for me?";
            		link.l1.go = "quests";
            		link.l2 = "Nothing in particular.";
            		link.l2.go = "exit";
            		break;
            	}
            }
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar,
					LinkRandPhrase("You dare to come to the governor?! A bold step...", "How did these idlers allow the enemy to break in to me?! It's mind-boggling...", "Yeah, my security isn't worth much, since"+ GetSexPhrase("some kind of bum","some kind of bum") +"running around in my residence..."),
					LinkRandPhrase("What do you need, "+ GetSexPhrase("scoundrel","scoundrel") +"?! My soldiers have already are on your trail, you can't get far"+ GetSexPhrase(" dirty pirate","") +"!", " Dirty murderer, get out of my residence! Guards!!", "I'm not afraid of you, bastard! You'll be hanged in the fort soon, you won't get far..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("The soldiers are not worth much...", "They will never catch me."),
					RandPhraseSimple("Shut your mouth, " + GetWorkTypeOfMan(npchar, "") + ", or I'll rip out your filthy tongue!", "I'll tell you what, buddy: sit tight and you'll live..."));
				link.l1.go = "fight";
				break;
			}
			if (sti(NPChar.nation) == PIRATE)
            {
				if (CheckAttribute(pchar, "questTemp.mushket2x2") && !CheckCharacterItem(pchar, "mushket2x2"))
				{
					dialog.text = "What do you need?";
					link.l1 = "I need to talk to you about something.";
					link.l1.go = "quests";
					break;
				}
				if (CheckAttribute(pchar, "GenQuest.CaptainComission") && CheckAttribute(pchar, "GenQuest.CaptainComission.PiratesCity") && NPChar.city == pchar.GenQuest.CaptainComission.PiratesCity)
				{
					dialog.text = "What do you need?";
					link.l1 = "I need to talk to you about something.";
					link.l1.go = "quests";
					break;
				}
				bOk = CheckAttribute(pchar, "questTemp.silverfleet") && pchar.questTemp.silverfleet != "over" && pchar.questTemp.silverfleet != "ToHuber" && NPChar.id == "LeFransua_Mayor";
				bOk2 = !CheckAttribute(NPChar, "SilverFleetSeaCheck") || GetNpcQuestPastDayParam(NPChar, "SilverFleetSeaCheck") > 0;
		    		if (bOk && bOk2)
				{
					dialog.text = "What you need?";
					link.l1 = "I need to talk to you about a case.";
					link.l1.go = "quests";
					break;
				}
				else
				{
					dialog.text = "We have nothing to talk about, I'm busy.";
					link.l1 = "Well, that's fine.";
					link.l1.go = "Exit";

					if(!CheckAttribute(PChar, "questTemp.pirateStartQuest"))
					{
						link.l2 = "Can you find a job for me?";
						link.l2.go = "pirateStartQuest";
					}
					else
					{
						if(pchar.questTemp.pirateStartQuest == "3" && !CheckAttribute(PChar, "questTemp.pirateStartQuest.many") && npchar.city == pchar.questTemp.pirateStartQuest.City)
						{
							link.l2 = "I'm on the trail, but I need money - 30,000 piastres to lure a rat out of a hole.";
							link.l2.go = "pirateStartQuest_many";
						}
						if(pchar.questTemp.pirateStartQuest == "3" && GetCharacterItem(pchar, "letter_open") > 0 && npchar.city == pchar.questTemp.pirateStartQuest.City)
						{
							link.l2 = "Look at what kind of letter I found on the corpse of one of the smugglers.";
							link.l2.go = "pirateStartQuest_info_1";
						}
						if(pchar.questTemp.pirateStartQuest == "5" && npchar.city == pchar.questTemp.pirateStartQuest.City)
						{
							link.l2 = "The job is done. Norman Vigo and his tub went to the sea devil.";
							link.l2.go = "pirateStartQuest_final";
						}
					}

					if(!CheckAttribute(PChar, "questTemp.pirateVikingQuest"))	//Квест Осколок Прошлого
					{
						if(sti(pchar.rank) >= 10)
						{
							link.l3 = "Is there any interesting case for me?";
							link.l3.go = "pirateVikingQuest";
						}
					}
					else
					{
						if(pchar.questTemp.pirateVikingQuest == "7" && npchar.city == pchar.questTemp.pirateVikingQuest.City)
						{
							dialog.text = "I see you have news?";
							link.l1 = "Yes, I killed this 'north-man', oh, he was fierce. Usually all conversations are started as if they smell death, but this one was growling and frothing at the mouth.";
							link.l1.go = "pirateVikingQuest_final";
						}
					}

					break;
				}
            }
			if (sti(NPChar.nation) != PIRATE && ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", 0) > 15)
            {
                dialog.text = "Oh! " + GetFullName(pchar) + "!. We just recently sent bounty hunters to bring you here. And you know, it's already cost us a tidy sum.";
                if (sti(pchar.Money) >= ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", 0) * 6000)
                {
                    link.l1 = "I just want to settle our differences.";
				    link.l1.go = "arestFree_1";
                }
				link.l2 = "E... I think I should go.";
				link.l2.go = "arest_1";
				break;
            }
			if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY && sti(NPChar.nation) != PIRATE)
			{
    			dialog.text = "The enemy is in residence! Alarm!!";
				link.l1 = "Ah, the devil!";
				link.l1.go = "fight";
				break;
			}


            // губернаторский (казённый) фрахт: вычисляем ОДИН раз до проверки знакомства (показываем и при первом контакте, и в обычном меню)
            bool bGovFreight = false;
            if (CheckAttribute(pchar, "CargoQuest.iOrderType") && sti(pchar.CargoQuest.iOrderType) == 6 && CheckAttribute(pchar, "CargoQuest.TraderID") && pchar.CargoQuest.iTradeColony == npchar.city)
            {
                if (CheckQuestAttribute("generate_trade_quest_progress", "begin") || CheckQuestAttribute("generate_trade_quest_progress", "failed")) bGovFreight = true;
            }

            if (npchar.quest.meeting == "0")
			{
				dialog.text = "I heard that you were very insistent on asking for an audience. My name is " + GetFullName(npchar) +
                              ". I am the governor of the city "+ GetCityName(NPChar.city) + ", patronage of " + NationKingsName(npchar)+
                              ". Now, please tell me the purpose of your visit, " + GetAddress_Form(NPChar) + ".";
				link.l1 = "My name " + GetFullName(pchar) + ".";
				link.l1.go = "node_1";
				if (bGovFreight)
				{
					link.l6 = "I have state cargo for you - delivered as commissioned.";
					link.l6.go = "GovFreight_Deliver";
				}
				npchar.quest.meeting = "1";
			}
			else
			{
               	//ОСАДЫ homo
            if (CheckAttribute(Colonies[FindColony(npchar.City)],"Siege"))
                {

                    makearef(aData, NullCharacter.Siege);

                    if (CheckAttribute(aData, "HelpColony") && sti(aData.HelpColony) == true )
                    {
                        dialog.text = "We've already discussed everything, haven't we?";
                        link.l1 = "That's right - I won't keep you any longer.";
                        link.l1.go = "exit";

                    }
                    else
                    {
                        dialog.text = "I must disappoint you, but at the moment I don't have time to receive you. As you probably already know, our colony was attacked by "+NationNamePeople(sti(aData.nation))+". And I need to organize the defense of the city.";
                        link.l1 = "That's what I wanted - offer your help in the defense of the fort.";
                        link.l1.go = "siege_task";
                        link.l2 = "Then I won't keep you any longer.";
            			link.l2.go = "exit";
                    }
                }
                else
                {//->
                    makearef(aData, NullCharacter.Siege);
                    if (CheckAttribute(aData, "PlayerHelpMayor") && sti(aData.PlayerHelpMayor) == true
                    && CheckAttribute(aData, "HelpColony") && sti(aData.HelpColony) == true )
                    {
                        // LEO: pay only for assault ships actually sank/captured (reward accrued in ShipDead/QuestShipCaptured), no freebie for watching
                        DeleteAttribute(aData, "PlayerHelpMayor");
                        DeleteAttribute(aData, "HelpColony");
                        if (sti(aData.PlayerSiegeKills) > 0)
                        {
                            AddMoneyToCharacter(Pchar, sti(aData.PlayerSiegeReward));
                            ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 20);
                            ChangeCharacterReputation(PChar, 10);
                            AddCharacterExpToSkill(GetMainCharacter(), "Leadership", 80);
                            AddCharacterExpToSkill(GetMainCharacter(), "Sailing", 100);
                            AddCharacterExpToSkill(GetMainCharacter(), "Accuracy", 100);
                            AddCharacterExpToSkill(GetMainCharacter(), "Cannons", 100);
                            SiegeRumour("Rumor has it that you helped us fight off "+NationNameSK(sti(aData.nation))+" squadrons - and defend our colony! We are so grateful to you, "+ GetAddress_Form(NPChar)+".", aData.Colony, sti(aData.conation), -1, 15, 3);
                            dialog.text = "Ah, it's you, "+GetAddress_Form(NPChar)+" "+GetFullName(pchar)+", glad to see you. Good news for our colony - we fought off the "+NationNameSK(sti(aData.nation))+" invaders, and much of that is your merit: you sank and captured "+sti(aData.PlayerSiegeKills)+" of the enemy's ships. The treasury pays you "+sti(aData.PlayerSiegeReward)+" gold, just as promised. Please receive it.";
                            link.l1 = "Thank you, " + GetAddress_FormToNPC(NPChar) + ". Nice doing business with you!";
                            link.l1.go = "exit";
                        }
                        else
                        {
                            // LEO: 0 sank/captured by player - a word of thanks, no gold
                            ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 5);
                            dialog.text = "Ah, it's you, "+GetAddress_Form(NPChar)+" "+GetFullName(pchar)+". Well, we fought off the "+NationNameSK(sti(aData.nation))+" invaders - though, to be honest, I saw no hand of yours in the sea battle: not one enemy ship went down or was boarded by you. The treasury has nothing to reward you for, no offense. But for not standing aside - my thanks.";
                            link.l1 = "I understand, " + GetAddress_FormToNPC(NPChar) + ". Next time I'll prove myself in the fight.";
                            link.l1.go = "exit";
                        }
                    }
                    else
                    {
   						//--> дача квеста найти кольцо в борделе
						if (rand(1) && pchar.questTemp.different == "free" && GetNpcQuestPastDayWOInit(npchar, "questTakeRing") > 50 && sti(pchar.questTemp.HorseQty) > 5)
						{
							if (npchar.city == "Bridgetown" || npchar.city == "Charles" || npchar.city == "FortFrance" || npchar.city == "Marigo" || npchar.city == "Panama" || npchar.city == "PortRoyal" || npchar.city == "SantoDomingo" || npchar.city == "Tortuga" || npchar.city == "Nassau")
							{
        						dialog.text = "Listen, I have a case for you, hmm... unusual order.";
								link.l1 = "I'm listening to you carefully.";
        						link.l1.go = "TakeRing_1";
								SaveCurrentNpcQuestDateParam(npchar, "questTakeRing");
								break;
							}
						}
						//<-- дача квеста найти кольцо в борделе
						dialog.text = RandPhraseSimple("Oh, is that you again? What is the issue this time?",
                                      "Are you interrupting me from the business of running the city again? What do you want, " +GetAddress_Form(NPChar)+"?");
        				link.l1 = "I want to talk about working for the benefit of the Crown of " + NationNameGenitive(sti(NPChar.nation))+".";
        				link.l1.go = "work";

        				link.l2 = "I need to talk to you about something.";
						if(CheckAttribute(pchar,"questTemp.ReasonToFast") && !CheckAttribute(pchar,"questTemp.ReasonToFast.SpeakOther") && !CheckAttribute(pchar, "questTemp.ReasonToFast.cantSpeakOther") && (pchar.questTemp.ReasonToFast.city == NPChar.city))
						{
							link.l2.go = "ReasonToFast_Mayor1";
						}
						else
						{
							link.l2.go = "quests";   //(перессылка в файл города)
						}
						if(CheckAttribute(pchar,"GenQuest.CaptainComission") && (pchar.GenQuest.CaptainComission.variant == "A2") && (pchar.GenQuest.CaptainComission.City == NPChar.city))
						{
							if(pchar.GenQuest.CaptainComission == "Begin_1" && !CheckAttribute(pchar,"GenQuest.CaptainComission.SpeakMayor"))
							{
								link.l12 = "You have captain of patrol " + GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")) + " with name " + pchar.GenQuest.CaptainComission.Name + " on serve, how may I see him?";
								link.l12.go = "CapComission_Mayor1";
							}
							if(CheckAttribute(pchar,"GenQuest.CaptainComission.PrisonFree"))
							{
								link.l12 = "Ex-captain of patrol " + GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")) + " " + pchar.GenQuest.CaptainComission.Name + " agreed to show his treasury to you.";
								link.l12.go = "CapComission_Mayor9";
							}
							if(pchar.GenQuest.CaptainComission == "GetGoodsSuccess")
							{
								link.l12 = "Ex-captain of patrol " + GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")) + " " + pchar.GenQuest.CaptainComission.Name + " showed his treasury to me.";
								link.l12.go = "CapComission_Mayor13";
							}
						}
        				if (GetPrisonerQty() > 0)
        				{
                            link.l3 = "I need to hand over the captive captain to the authorities.";
        				    link.l3.go = "sell_prisoner";
        				}
        				if (npchar.Default  == npchar.location) // если не в тюрьме
        				{
        					if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
        					{
        						link.l4 = "I need to clarify a number of financial issues with you.";
        						link.l4.go = "LoanForAll";//(перессылка в кредитный генератор)
        					}
        				}
						//-->> сдача найденного в борделе кольца
						if (CheckCharacterItem(pchar, "MayorsRing"))
						{
							if (pchar.questTemp.different.TakeMayorsRing == "toBrothel")
							{
        						link.l5 = "I managed to find your ring. Here it is.";
        						link.l5.go = "TakeRing_S1";
							}
							else
							{
        						link.l5 = "I have your wedding ring. As it turned out, you gave it to one of the bordello girls.";
        						link.l5.go = "TakeRing_S3";
							}
						}
						//<<-- сдача найденного в борделе кольца
						if (npchar.id == "Santiago_Mayor" && CheckAttribute(pchar, "PGGWhisperLetter"))
						{
							link.lPGGWhisper = "Have you heard, Governor? A ship belonging to a certain Visper anchored in Puerto Principe. Isn't that the witch who escaped from the Inquisition?";
        					link.lPGGWhisper.go = "PGGWhisper";
						}
						// Gregg: the Jamaica office quest - the Fort Orange commandant is sceptical of the Company's venture (one-time branch)
						if (npchar.city == "FortOrange" && CheckAttribute(pchar, "GWIK.Jamaica.Stage") && !CheckAttribute(pchar, "GWIK.Jamaica.MayorOk"))
						{
							link.lGWIKJam = "They say you are none too pleased with the West India Company's plans for your town.";
							link.lGWIKJam.go = "GWIKJam_Mayor";
						}

        				link.l7 = "I would like to know where the Governor General of " + NationNameGenitive(sti(NPChar.nation)) + " is.";
        				link.l7.go = "Where_guber";
								// губернаторский (казённый) фрахт: сдача груза - он уходит в магазин этого города (incl. overdue)
								if (bGovFreight)
								{
									link.l6 = "I have state cargo for you - delivered as commissioned.";
									link.l6.go = "GovFreight_Deliver";
								}
        				link.l8 = "Maybe you want to take a break from important things? I can offer to spend time playing.";
        			    link.l8.go = "Play_Game";
        				link.l10 = "Please forgive me, but I have things to do.";
        				link.l10.go = "exit";
                    }
                }//<-
			}
		break;

		case "GWIKJam_Mayor": // Gregg: the Jamaica office quest - sceptical commandant, the town gets a voice
			dialog.text = "None too pleased is putting it mildly, Captain. Companies come and go, but the town will pay for their boldness: Port Royal will not forgive us this office. I need guarantees, not the promises of visiting merchants.";
			link.l1 = "The office means warehouses, duties and work for the townsfolk. I vouch for the venture with my own name.";
			link.l1.go = "GWIKJam_Mayor_Talk";
			if (sti(pchar.money) >= 5000)
			{
				link.l2 = "Guarantees? Five thousand piastres into the town treasury - is that guarantee enough?";
				link.l2.go = "GWIKJam_Mayor_Gold";
			}
			link.l3 = "That is a matter for the Company and the governor of Curacao. Good day.";
			link.l3.go = "exit";
		break;

		case "GWIKJam_Mayor_Talk":
			if (GetSummonSkillFromName(pchar, SKILL_LEADERSHIP) >= 25 + sti(pchar.rank)) // Gregg: Leadership skill 0-100 >= 25 + player rank (was hardcoded 6 - gate almost always open); a gold path exists for weak authority
			{
				pchar.GWIK.Jamaica.MayorOk = true;
				GWIK_AddRep(10);
				AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 60); // Gregg: leadership xp for successfully persuading the commandant (Leadership >= 25+rank check)
				dialog.text = "Hm... Your name does carry weight - they speak of you in every port. Very well, Captain: the town will not stand in the Company's way. But tell your merchants - they answer for Fort Orange's peace as much as I do. And tell the envoy: the commandant has given his word, and in these parts it counts for something.";
				link.l1 = "I'll pass it on word for word.";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "Words, Captain, words. They cannot be filed with the ledgers or put in the treasury. Come back when you can offer something weightier.";
				link.l1 = "Then we'll find another argument.";
				link.l1.go = "exit";
			}
		break;

		case "GWIKJam_Mayor_Gold":
			if (sti(pchar.money) < 5000)
			{
				dialog.text = "I don't see five thousand, Captain.";
				link.l1 = "I must have miscounted.";
				link.l1.go = "exit";
				break;
			}
			AddMoneyToCharacter(pchar, -5000);
			pchar.GWIK.Jamaica.MayorOk = true;
			GWIK_AddRep(10);
			PlayStereoSound("notebook");
			dialog.text = "Now that is talk the Dutch way: short and ringing. The town treasury accepts your contribution. Fort Orange will not stand in the Company's way - and I hope it won't regret it. Tell the envoy: the commandant has given his word, and in these parts it counts for something.";
			link.l1 = "It won't, Commandant.";
			link.l1.go = "exit";
		break;

		case "GovEscort_Tell": // escort: captain reports the journey, reaction by losses/redirect
			dialog.text = "I am listening, captain. How was the passage?";
			int teAlive = GovEscort_ShipsAlive();
			int teTotal = sti(pchar.CargoQuest.GovEscortShips);
			bool tePartial = (teAlive < teTotal);
			// LEO: wording follows the actual FIGHT, not the redirect - the captain used to report
			//   a hot battle even when the ambush never happened.
			bool teFought = CheckAttribute(pchar, "CargoQuest.GovEscortFought");
			bool teRedir = CheckAttribute(pchar, "CargoQuest.GovEscortAgentMult");
			if (teFought)
			{
				if (teRedir)
				{
					if (tePartial) link.l1 = "An ambush lay in wait by the island - someone learned of the cargo after all. We fought them off, but not all the ships survived.";
					else link.l1 = "An ambush met us by the island, just as you feared. A hot fight, but I brought every ship through.";
				}
				else
				{
					if (tePartial) link.l1 = "Pirates intercepted us on the way. Some ships could not be saved - we fought as best we could.";
					else link.l1 = "Pirates intercepted us on the way. We fought them off - every ship made it through.";
				}
			}
			else
			{
				if (tePartial) link.l1 = "The passage was hard. Some of the ships never reached the port.";
				else link.l1 = "The road was quiet, all the ships made it through intact.";
			}
			link.l1.go = "GovEscort_Pay";
		break;
		case "GovEscort_Pay": // escort: closing + payment (reached with or without the report)
			int peAlive = GovEscort_ShipsAlive();
			int peTotal = sti(pchar.CargoQuest.GovEscortShips);
			bool pePartial = (peAlive < peTotal);
			float peMult = 1.0;
			if (CheckAttribute(pchar, "CargoQuest.GovEscortAgentMult")) peMult = stf(pchar.CargoQuest.GovEscortAgentMult);
			if (pePartial) dialog.text = "Losses... well, you did not call down that ambush. What matters is you did not abandon the cargo and brought through what you could. The crown values loyalty. Here is your pay.";
			else { if (peMult > 1.5) dialog.text = "Not a single ship lost! You exceeded my expectations, captain - my messenger was not wrong about you. Take your pay, fivefold, as agreed."; else dialog.text = "All ships intact - excellent. The treasury thanks you for your service. Here is your pay."; }
			link.l1 = "Glad to be of service, " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "exit";
			GovEscort_Deliver(NPChar, peMult);
		break;
		case "GovFreight_Deliver":
			// казённый груз принимают с корабля, пришвартованного в ЭТОМ порту (не пешком и не с чужого причала)
			if (!CargoShipMooredInCity(npchar.city))
			{
				dialog.text = "State cargo, you say? But your ship is not in our port, captain. Moor her here - and the cargo will be received with all due honours.";
				link.l1 = "I will bring the ship into port.";
				link.l1.go = "exit";
				break;
			}
			// --- ESCORT (subtype 2): deliver what arrived (partial), pay proportional to surviving cargo ---
				if (CheckAttribute(pchar, "CargoQuest.GovEscortActive"))
				{
					int geAboard = GovEscort_CargoAboard();
					if (geAboard <= 0)
					{
						dialog.text = "Where is the cargo, captain? Not one escort ship made it through. Without the goods there is nothing to discuss.";
						link.l1 = "I haven't lost everything yet.";
						link.l1.go = "exit";
						break;
					}
					int geAlive = GovEscort_ShipsAlive();
					int geTotal = sti(pchar.CargoQuest.GovEscortShips);
					bool gePartial = (geAlive < geTotal); // some ships lost on the way
					bool geRedir = CheckAttribute(pchar, "CargoQuest.GovEscortAgentMult");
					if (geRedir)
					{
						if (gePartial) dialog.text = "Captain! So my messenger found you in time. But I see not all the ships made it through... the pirates lay in wait after all. Foul waters these days. And yet you saved what you could - the crown will not forget it. Fivefold pay, as agreed: for the risk and the loyalty.";
						else dialog.text = "All ships intact? Heaven be praised - so my messenger sought you out for good reason, and you heeded the counsel. The cargo is safe, captain. The crown keeps its word: fivefold pay.";
					}
					else
					{
						if (gePartial) dialog.text = "Not all the ships made it, captain... An ambush? That is what I feared. A pity you could not save it all - but for what you brought through you will be paid in good conscience. My thanks for that, at least.";
						else dialog.text = "The escort has arrived, the cargo accepted intact. The treasury thanks you for the convoy, captain - here is your pay.";
					}
					link.l1 = "Allow me to report on the passage, " + GetAddress_FormToNPC(NPChar) + ".";
					link.l1.go = "GovEscort_Tell";
					link.l2 = "The cargo is yours. I will take what is owed.";
					link.l2.go = "GovEscort_Pay";
					break;
				}
				int gfGoods = GovSwap_HoldGood(); // what's actually in the hold: cheap filler if swapped, else the real cargo
			int gfQty   = makeint(pchar.CargoQuest.iQuantityGoods);
			if (GetSquadronGoods(pchar, gfGoods) < gfQty)
			{
				dialog.text = "Where is the cargo, captain? Your hold comes up short. Bring it all - then we will settle.";
				link.l1 = "I will deliver every last bit.";
				link.l1.go = "exit";
				break;
			}
			// --- CARGO SWAP: customs inspection before settlement ---
				if (CheckAttribute(pchar, "CargoQuest.GovSwapDone"))
				{
					if (!CheckAttribute(pchar, "CargoQuest.GovSwapInspected"))
					{
						// first attempt to deliver the swapped cargo - the mayor demands an inspection
						pchar.CargoQuest.GovSwapMayorId = NPChar.id; // remember the mayor - after the inspection the word returns to him (GovSwap_AfterInspector)
							pchar.CargoQuest.GovSwapMayorType = NPChar.chr_ai.type; // snapshot the mayor's type (sit/huber...) - restore the ORIGINAL after the verdict
						GovSwap_SpawnInspector(sti(NPChar.nation)); // LEO: сам проверит, не идёт ли уже досмотрщик
						dialog.text = RandPhraseSimple(
							"A state order? Very well. But rules are rules - first a customs inspector shall examine the cargo. (His Excellency gives a sign, and a customs inspector heads your way from the door.)",
							"Goods for my commission? Excellent. Just a formality first - my inspector will examine the consignment. (The governor nods, and a man with a ledger heads your way from the door.)"
						);
						link.l1 = "Of course, Your Excellency.";
						link.l1.go = "exit";
						break;
					}
					if (pchar.CargoQuest.GovSwapInspected == "caught")
					{
						// the inspector exposed the swap - the governor announces a fine (GovSwap_CaughtPenalty)
						AddDialogExitQuestFunction("GovSwap_MayorReseat");
						dialog.text = RandPhraseSimple(
							"...A substitution?! You foisted cheap rubbish on the treasury instead of the ordered goods, and under my own seal! A fine of " + FindRussianMoneyString(GovSwap_FineAmount()) + ", and don't you dare haggle. Confiscate the cargo. Get out while I am still merciful.",
							"...So it is a forgery?! You thought the treasury was blind? A fine of " + FindRussianMoneyString(GovSwap_FineAmount()) + ", the cargo confiscated. Be glad it is not the dungeon, captain. The treasury has a long memory."
						);
						link.l1 = "(leave in silence)";
						link.l1.go = "exit";
						GovSwap_CaughtPenalty(NPChar);
						break;
					}
					// GovSwapInspected == "passed" - the inspector missed it, settlement proceeds as usual (below)
				}
				// successful delivery (normal OR a swap that passed): payout/exp/cargo/close + withholding a past fine - in GovSwap_DeliverReward
			AddDialogExitQuestFunction("GovSwap_MayorReseat");
			bool gfLate = CheckQuestAttribute("generate_trade_quest_progress", "failed");

			// LEO: тут только СЧИТАЕМ штраф и премию для лога. Сами они уже учтены в GovSwap_DeliverReward
			//   (GovSwap.c), и правка pchar.CargoQuest.iMoney приводила к двойному счёту: при опоздании
			//   выплата падала до четверти, при досрочной сдаче вырастала в 1.44 раза.
			int gfReward = sti(pchar.CargoQuest.iMoney);
			bool gfEarly = false;
			int gfFine = 0;
			int gfBonus = 0;
			if (gfLate)
			{
				dialog.text = "You are late, captain. The cargo is no longer as needed - the treasury will pay half.";
				gfFine = makeint(gfReward / 2);
			}
			else
			{
				dialog.text = "Excellent, captain. The cargo arrived intact. The treasury does not forget a service - here is your reward.";
				if (GetQuestPastDayParam("CargoQuest") * 2 < sti(pchar.CargoQuest.iDaysExpired))
				{
					gfBonus = makeint(gfReward * 0.2);
					gfEarly = true;
				}
			}

			if (gfLate)
			{
				Log_Info(XI_ConvertString("Freight_LateFineTerm"));
				Log_InfoColored("red", "-" + FindRussianMoneyStringLog(gfFine));
			}
			else if (gfEarly)
			{
				Log_Info(XI_ConvertString("Freight_EarlyBonus"));
				Log_InfoColored("green", "+" + FindRussianMoneyStringLog(gfBonus));
			}
			Log_Info(XI_ConvertString("Freight_TotalGov"));
			link.l1 = "Glad to be of service, " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "exit";
			GovSwap_DeliverReward(NPChar);
		break;

		case "PGGWhisper":
			pchar.PGGWhisperLetterSent = true;
			DeleteAttribute(pchar, "PGGWhisperLetter");

			sTemp = npchar.city;
			if (CheckAttribute(pchar, "GenQuest.MayorQuestsList." + sTemp) && sti(pchar.GenQuest.MayorQuestsList.(sTemp)) > 2)
			{
				dialog.text = "Have you told anyone else about this?";
				link.l1 = "No, you're the first.";
				link.l1.go = "PGGWhisper_1";
			}
			else
			{
				pchar.PGGWhisperLetterSentFail = true;
				AddQuestRecord("PGGWhisper", "6");
				dialog.text = "Really? I haven't heard that, although I usually get all the news before the local innkeeper. And who are you exactly? My people will check your tip, and in the meantime you will be interrogated by my guards.";
				link.l1 = "Damn it, I didn't believe it!";
				link.l1.go = "fight";
			}
		break;

		case "PGGWhisper_1":
			addMoneyToCharacter(pchar, 10000);
			dialog.text = "Excellent. This is just great news! If I bring her here in chains, the Holy Inquisition will be delighted! Here, take this, for such information you deserve a reward, tell me if you still want to serve for the benefit of the Crown of Spain? Can you catch and bring this Whisper to me?";
			link.l1 = "I would love to, but I'm afraid it's time for me to sail. The freight deadlines are burning, you know. Perhaps you can entrust this matter to your soldiers?";
			link.l1.go = "PGGWhisper_2";
		break;

		case "PGGWhisper_2":
			dialog.text = "You know, you're right! Too often I turn to outside captains for help, it's time to get down to business myself. I will entrust the interception of the vessel to my patrol fleet, since her ship is in the bay, Whisper herself may be hiding somewhere in the jungle! I think it's worth connecting the garrison from the fort to the search. Are you still here, Captain? You can go now, thanks for the information.";
			link.l1 = "Goodbye, and good luck in your search.";
			link.l1.go = "exit";
			pchar.quest.QuestWhisper_Siege.win_condition.l1          = "location";
			pchar.quest.QuestWhisper_Siege.win_condition.l1.location = "Santiago_town";
			pchar.quest.QuestWhisper_Siege.function             = "QuestWhisper_Siege";
		break;

		//Blackthorn. Квест викинга
		case "pirateVikingQuest":
			dialog.text = "There's a job for you. Not only did you use your sword, but your mind, I'm told. Listen. A Scandinavian appeared in our waters a couple of years ago, either a Dane or a Swede, it doesn't matter, he called himself Ragnar. His ship is just a sight to behold and the crew consists entirely of Normans. And it seems like everything is fine, he pirates little by little, earns pennies for bread, but he does not contribute a share to the common pot, he is rude to the governors, he sells prizes in general, it is not known where. Do you understand what the conversation is about?";
			link.l1 = "I understand, you need to punish him so that he becomes an example. That's just not according to the laws of the Brethren somehow. Well, the pirate has gone too far, so let someone whisper to the authorities where he lives. And no one will make any claims against us, honest corsairs.";
			link.l1.go = "pirateVikingQuest_1";
			link.l2 = "I don't like this conversation. Goodbye...";
			link.l2.go = "exit";
			pchar.questTemp.pirateVikingQuest = "0";
		break;
		case "pirateVikingQuest_1":
			dialog.text = "Laws of Brethren, Code, pirate honor... Pfft... It's a childish prank. You also remember the Libertalia that Sharp tried to build. Ha ha! Here we have a balance of power, the authorities turn a blind eye to our business, if we do not interfere with their business. What if peace is signed in Europe? So the patents will be revoked, and all the brave privateers will become criminals in an instant. And the punitive squadrons will arrive. To catch you and me. And captains like this Viking spoil the reputation of all of us. Are you smart?";
			link.l1 = "I understand, you gave me food for thought. Where can I find this Viking?";
			link.l1.go = "pirateVikingQuest_2";
		break;
		case "pirateVikingQuest_2":
			dialog.text = "Somewhere. If he knew where he lived, he would go out to sea himself, he would not look at old wounds and age. You are a savvy person, you'll figure it out. One condition is that this Ragnar and his entire team must go to fish food, and I will not offend with gold.";
			link.l1 = "Wait for the news. And about the revocation of patents, if there is any news, let me know. Sometimes, "+GetFullName(npchar)+".";
			link.l1.go = "exit";

			pchar.questTemp.pirateVikingQuest = "1";
			pchar.questTemp.pirateVikingQuest.City = npchar.city;
			pchar.questTemp.pirateVikingQuest.Areal = GetArealByCityName(pchar.questTemp.pirateVikingQuest.City);
			pchar.questTemp.pirateVikingQuest.Name = GetFullName(npchar);
			pchar.LastHearedViking = npchar.city;
			AddQuestRecord("pirateVikingQuest", "1");
			AddQuestUserData("pirateVikingQuest", "sCity", XI_ConvertString("Colony" + pchar.questTemp.pirateVikingQuest.City + "Gen"));
			AddQuestUserData("pirateVikingQuest", "sName", pchar.questTemp.pirateVikingQuest.Name);
		break;
		case "pirateVikingQuest_final":
			CloseQuestHeader("pirateVikingQuest");
			pchar.questdata.OskolokProshlogo = 1;
			TakeNItems(pchar, "chest", 3);
			Log_Info("You got the credit chests");
			dialog.text = "Yeah, I read it once. A berserker in sight. It's like a piece of the past. Here, take the reward, you deserve.";
			pchar.questTemp.pirateVikingQuest = "end";
			link.l1 = "Thank you.";
			link.l1.go = "exit";
		break;

		//zagolski. начальный квест за пирата ===================================
		case "pirateStartQuest":
			dialog.text = "Hmm... a job, you say? And it's even very good that you turned to me. I just need someone from the outside. I need to find out something in the neighboring settlement, and my guys are known there like crazy. I don't promise a lot of money, but something will fall over.";
			link.l1 = "That's nice. What's the deal?";
			link.l1.go = "pirateStartQuest_1";
			link.l2 = "Sorry, I won't risk a penny. Goodbye...";
			link.l2.go = "exit";
			pchar.questTemp.pirateStartQuest = "0";
		break;
		case "pirateStartQuest_1":
			dialog.text = "And the thing is this - about a month ago I conspired with the captain of the schooner Sentblue to deliver 200 slaves to sell them to a planter from a neighboring settlement. The deadlines have already become tight, and the customer is nervous, and yesterday I was informed that this schooner was seen in the waters of the island. But!.. The captain didn't show up. And today the planter demanded to return the deposit and said that the slaves were offered to him elsewhere - although more expensive, but the whole lot at once.";
			link.l1 = "So what? I owe to get 200 slaves in a matter of days, so that you don't lose the deposit? And all this for pennies?";
			link.l1.go = "pirateStartQuest_2";
		break;
		case "pirateStartQuest_2":
			dialog.text = "No, of course not. Do I look like an idiot? I will not refund the deposit in any case. But I need proof that the slaves they're giving him are my party. And you'll just find those who offered him a new deal, and you'll get your 10,000 piastres. The rest is my concern. Well, are you ready?";
			link.l1 = "I'll take it! And who could it be? Do you have any thoughts?";
			link.l1.go = "pirateStartQuest_3";
			link.l2 = "No. It's unclear who to look for, it's unclear where, and for only 10,000 - sorry, I won't take it.";
			link.l2.go = "exit";
		break;
		case "pirateStartQuest_3":
			dialog.text = "If I had even a hint, I would have done without your services. They would have been fishing for squid by nightfall. But I'm more interested in something else - how did they find out about the Sentblue cargo and about my customer? It all worked out very quickly for them. Are you smart?";
			link.l1 = "Yes, clearly someone is peddling information.";
			link.l1.go = "pirateStartQuest_4";

			pchar.questTemp.pirateStartQuest = "1";
			pchar.questTemp.pirateStartQuest.City = npchar.city;
			pchar.questTemp.pirateStartQuest.Areal = GetArealByCityName(pchar.questTemp.pirateStartQuest.City);
			pchar.questTemp.pirateStartQuest.Name = GetFullName(npchar);

			AddQuestRecord("pSQ", "1");
			AddQuestUserData("pSQ", "sCity", XI_ConvertString("Colony" + pchar.questTemp.pirateStartQuest.City + "Gen"));
			AddQuestUserData("pSQ", "sName", pchar.questTemp.pirateStartQuest.Name);
		break;
		case "pirateStartQuest_4":
			dialog.text = "Here! So I'm looking for him. Therefore, quietly slaughtering competitors and returning slaves is not the main thing in this case. I need an informant, or rather, his head on a spit. Now do you understand why none of my people should know about this assignment? Until I find out who is selling the information, I will suspect everyone. And I really don't want that...";
			link.l1 = "Understood... So, we will act quietly and not raise a wave. I'm going on a search.";
			link.l1.go = "exit";
		break;

		case "pirateStartQuest_many":
			dialog.text = "Thirty thousand, you say? That's a lot of money... I can only give you 5,000 piastres, I don't have any more right now. But if you decide to run away with them, I'll find them and hang them on the nearest fence! Such jokes won't work with me.";
			link.l1 = "Give me five thousand...";
			link.l1.go = "pirateStartQuest_many_1";
		break;
		case "pirateStartQuest_many_1":
			dialog.text = "Here you go. I'm waiting for the search results.";
			link.l1 = "Everything will be fine.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 5000);
			pchar.questTemp.pirateStartQuest.many = "";
		break;

		//------------------------------------
		case "pirateStartQuest_info_1":
			dialog.text = "Ha! Great news! I was sure you could handle it. I got some very interesting information.";
			link.l1 = "I've done my job. I want to get my ten thousand.";
			link.l1.go = "pirateStartQuest_info_2";
			TakeItemFromCharacter(pchar, "letter_open");
		break;
		case "pirateStartQuest_info_2":
			dialog.text = "Of course, here is the promised reward. But don't rush, I want to assign you one more case. Pfiefer is a man from Norman Vigo's team who rides on the kutter Normandy. I'm sure Norman Vigo is the informant, even though he's one of ours. He was recently seen in the waters of the island, your job is to send the rat to the sea devil. The first time you didn't let me down, so now I want to entrust this matter to you. I will pay 15,000 piastres. Agree?";
			link.l1 = "I'll take it! Consider the traitor Norman Vigo already dead.";
			link.l1.go = "pirateStartQuest_info_3";
			link.l2 = "You know what, I don't want to get involved in this... And there's no time. I'm sorry, but I'll pass.";
			link.l2.go = "pirateStartQuest_info_noex";
			AddMoneyToCharacter(pchar, 10000);
		break;
		case "pirateStartQuest_info_noex":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			pchar.questTemp.pirateStartQuest = "0";
			CloseQuestHeader("pSQ");
			pchar.questdata.TravlyaKrys = 1;
		break;
		case "pirateStartQuest_info_3":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			pchar.questTemp.pirateStartQuest = "4";

			AddQuestRecord("pSQ", "7");
			AddQuestUserData("pSQ", "sName", pchar.questTemp.pirateStartQuest.Name);

			pchar.quest.zpq_seaBattle.win_condition.l1 = "location";
			pchar.quest.zpq_seaBattle.win_condition.l1.location = pchar.questTemp.pirateStartQuest.Areal;
			pchar.quest.zpq_seaBattle.function = "pirateStartQuest_seaBattle";
		break;

		case "pirateStartQuest_final":
			dialog.text = "We know-we know. My people watched the fight and have already reported to me how well you managed. Well, you did your job properly, here is the promised reward of 15,000 piastres. And thank you, you helped me a lot.";
			link.l1 = "Everything is fine, I'm glad to help! And now I have things to do. See you later!";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 15000);
			pchar.questTemp.pirateStartQuest = "0";
			CloseQuestHeader("pSQ");
			pchar.questdata.TravlyaKrys = 1;
			group_DeleteGroup("pirateStartQuest_Smuggler_group");
		break;

		// ------------------------- Повод для спешки -----------------------------
		case "ReasonToFast_Mayor1":
			dialog.text = "I'm listening to you carefully, Captain.";
			link.l1 = "I want to report a criminal conspiracy between an officer of your garrison and pirates (sets out the circumstances of the case).";
			if(sti(pchar.reputation) < 40)
			{
				link.l1.go = "ReasonToFast_BMayor";
			}
			else
			{
				link.l1.go = "ReasonToFast_GMayor";
			}
			pchar.questTemp.ReasonToFast.SpeakOther = true;
			if(CheckAttribute(pchar,"GenQuest.CannotWait"))	DeleteAttribute(pchar, "GenQuest.CannotWait");
		break;

		case "ReasonToFast_GMayor":
			if(pchar.questTemp.ReasonToFast == "PatrolSuccess_1")
			{
				// вилка_А
				dialog.text = "Thank you, "+ GetSexPhrase("sir","miss") +"! I will immediately give the order to detain the scoundrel. However, you have spent a lot, and the municipal treasury, alas, is empty...";
				link.l1 = "Your Grace! I didn't do it for the money...";
				link.l1.go = "ReasonToFast_GMayor_11";
				if(pchar.questTemp.ReasonToFast.chain == "A0") ReasonToFast_RemoveVictim();
				if(pchar.questTemp.ReasonToFast.chain == "A1") pchar.quest.ReasonToFast_ClearLakey.over = "yes";
			}
			if(pchar.questTemp.ReasonToFast == "PatrolSuccess_2")
			{
				// получена карта
				dialog.text = "Thank you, "+ GetSexPhrase("sir","miss") +"! I will immediately give the order to detain the scoundrel. Just think about it! We wanted to reward him with a personalized weapon for his impeccable service. It's good that everything has cleared up, and there was no such embarrassment!";
				link.l2 = "Always happy to serve justice...";
				link.l2.go = "ReasonToFast_GMayor_21";
			}
			if(pchar.questTemp.ReasonToFast == "PatrolDied")
			{
				// патруль перебит
				dialog.text = GetSexPhrase("Sir","Miss") +"! We have long suspected that this officer and the rabble surrounding him are dishonest, but it seems to me that you acted recklessly by dealing with them without witnesses.";
				link.l3 = "Your Grace! I was protecting my life...";
				link.l3.go = "ReasonToFast_GMayor_31";
				pchar.questTemp.ReasonToFast.speakAfterPatrolDied = true;
				pchar.quest.ReasonToFast_SpeakMayor.over = "yes";
			}
		break;

		case "ReasonToFast_BMayor":
			if(pchar.questTemp.ReasonToFast == "PatrolSuccess_1")
			{
				dialog.text = "Captain, do you even understand what you're getting into?!! We've been trying to set this trap for over a month! And for the sake of entertainment, you disrupted the meeting of our patrol with the messenger " + GetName( NAMETYPE_MAIN, pchar.questTemp.ReasonToFast.p3, NAME_GEN) + " and also came to me to brag about it! Maybe now you can tell me how I should justify the government costs for organizing this operation?!!";
				link.l1 = "Your Grace! You refuse to see the essence of what is happening...";
				link.l1.go = "ReasonToFast_BMayor_1";
				pchar.questTemp.ReasonToFast = "speakSuccess_chain_A";
				if(pchar.questTemp.ReasonToFast.chain == "A0") ReasonToFast_RemoveVictim();
				if(pchar.questTemp.ReasonToFast.chain == "A1") pchar.quest.ReasonToFast_ClearLakey.over = "yes";
			}
			if(pchar.questTemp.ReasonToFast == "PatrolSuccess_2")
			{
				// получена карта
				dialog.text = "Let's take a look at this map...Are you kidding me? Is this worn piece of parchment proof?";
				link.l2 = "Your Grace! You refuse to see the essence of what is happening...";
				link.l2.go = "ReasonToFast_BMayor_1";
				TakeItemFromCharacter(pchar, "mapQuest");
				pchar.questTemp.ReasonToFast = "speakSuccess_chain_B";
			}
			if(pchar.questTemp.ReasonToFast == "PatrolDied")
			{
				// патруль перебит
				dialog.text = "An original justification for the death of an entire patrol unit. Well, at least by coming here, you've saved us the trouble of looking for the killer.";
				link.l3 = "Your Grace! You refuse to see the essence of what is happening...";
				link.l3.go = "ReasonToFast_BMayor_1";
				pchar.questTemp.ReasonToFast = "speakSuccess_chain_A";
				pchar.quest.ReasonToFast_SpeakMayor.over = "yes";
			}
		break;

		case "ReasonToFast_GMayor_11":
			dialog.text = "Nevertheless, I want your efforts to be rewarded. Here, take the map found in the belongings of the recently hanged sea robber. Perhaps, with the help of the Lord, you will indeed discover the treasures, although it is very doubtful...";
			link.l1 = "Thank you, you are very generous!";
			link.l1.go = "exit";
			AddQuestRecord("ReasonToFast", "15");
			ChangeCharacterReputation(pchar, 1);
			GiveItem2Character(pchar, "mapQuest");
			arItem = ItemsFromID("mapQuest");
			ReasonToFast_GenerateTreasureMap(arItem);
			pchar.questTemp.ReasonToFast.state = "chain_A"; // вилка_А, наводки не генерим
			pchar.questTemp.ReasonToFast = "GetMap";
			ReasonToFast_SetHunterCoastal();
		break;

		case "ReasonToFast_GMayor_21":
			dialog.text = "Commendable zeal. Take this blade as a reward. It's the least I can give you! Yes.. and keep this map for yourself. I'm sure there are a lot of such fakes going around the Archipelago.";
			link.l1 = "Thank you, you are very generous!";
			link.l1.go = "exit";
			AddQuestRecord("ReasonToFast", "14");
			ChangeCharacterReputation(pchar, 1);
			GiveItem2Character(pchar, pchar.questTemp.ReasonToFast.p6);
			pchar.questTemp.ReasonToFast.state = "chain_B"; // была получена карта
			pchar.questTemp.ReasonToFast = "GetMap";
			ReasonToFast_SetHunterCoastal();
		break;
		case "ReasonToFast_GMayor_31":
			dialog.text = "Yes, maybe, maybe... Well, I am ready to consider that the matter was resolved by God's judgment and by His will.";
			link.l1 = "Thank you, you are very generous!";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, 1);
			AddQuestRecord("ReasonToFast", "16");
			CloseQuestHeader("ReasonToFast");
			DeleteAttribute(pchar,"ReasonToFast");
		break;

		case "ReasonToFast_BMayor_1":
			dialog.text = "Don't you dare lecture me! Your collusion with scammers is obvious to me! Guard, take "+ GetSexPhrase("this 'well-wisher'","this 'well-wisher'") +"!";
			link.l1 = "No way!";
			link.l1.go = "fight";

			pchar.quest.ReasonToFast_ExitFromTown.win_condition.l1			= "EnterToSea";
            pchar.quest.ReasonToFast_ExitFromTown.function    				= "ReasonToFast_ExitFromTownFight";
			pchar.questTemp.ReasonToFast.canSpeakSailor = true;
			pchar.TownEscape = true;
			Flag_Change(PIRATE);
		break;

		// ------------------------- Повод для спешки -----------------------------

		// --------------------------Операция 'Галеон'-----------------------------
		case "CapComission_Mayor1":
			dialog.text = "This is not possible at the moment. He is under arrest and awaiting trial.";
			link.l1 = "Is that so? And what is he suspected of?";
			link.l1.go = "CapComission_Mayor2";
		break;

		case "CapComission_Mayor2":
			dialog.text = "Unfortunately, this is not a suspicion, but a proven fact. We received information that under the guise of patrolling, he was engaged in essentially maritime robbery - he confiscated goods from passing merchant ships and secretly sold them. But we considered him one of the best captains of the patrol squadron.";
			link.l1 = "And who provided the evidence?";
			link.l1.go = "CapComission_Mayor3";
		break;

		case "CapComission_Mayor3":
			dialog.text = "One of the victims sent a report detailing the incident, indicating the date, the name of the injured vessel and the amount of confiscated goods. " +
				"Believe me, there are very solid figures there. On this day, the patrolman " + GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name"))) + " captain " + pchar.GenQuest.CaptainComission.Name + " was really on duty. The captain in his report described the incident as a skirmish with a pirate ship, but did not mention a word about the cargo.";
			link.l1 = "Was there any cargo at all?";
			link.l1.go = "CapComission_Mayor4";
			pchar.GenQuest.CaptainComission.SpeakMayor = true;
		break;

		case "CapComission_Mayor4":
			dialog.text = "There can be no doubt about it, the victim is too famous a person to engage in petty slander. Right now, patrol squads are searching for the captain's hiding place " + pchar.GenQuest.CaptainComission.Name + ". " +
				"As soon as he is found, the captain will stand trial.If you will help us in this matter, then the city authorities in my person will be very grateful to you.";
			link.l1 = "Probably not. I don't know the surrounding area well enough to search for a hiding place. Excuse me, I have to go.";
			link.l1.go = "CapComission_Mayor5";
			link.l2 = "I think I'm ready. How much time do I have?";
			link.l2.go = "CapComission_Mayor6";
		break;

		case "CapComission_Mayor5":
			AddQuestRecord("CaptainComission2", "49");
			AddQuestUserData("CaptainComission2", "sShipType", GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")));
			AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
			ChangeCharacterReputation(pchar, -2);
			pchar.GenQuest.CaptainComission = "MayorTalkBad";
			CaptainComission_GenerateCaptainInPrison();
			DialogExit();
		break;

		case "CapComission_Mayor6":
			dialog.text = "I think the patrol will find the cache in two days without you, so I advise you to hurry up.";
			link.l1 = "Okay, I'm taking this case. But I'm going to need a date with the captain. " + pchar.GenQuest.CaptainComission.Name + ". Is there any way to arrange this?";
			link.l1.go = "CapComission_Mayor7";
		break;

		case "CapComission_Mayor7":
			dialog.text = "Of course, there will be no problems with this, I will notify the commandant of the fort about your visit.";
			link.l1 = "Thank you. Allow me to take my leave.";
			link.l1.go = "CapComission_Mayor8";
		break;

		case "CapComission_Mayor8":
			AddQuestRecord("CaptainComission2", "50");
			AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("xia","as"));
			AddQuestUserData("CaptainComission2", "sSex1", GetSexPhrase("","a"));
			AddQuestUserData("CaptainComission2", "sShipType", GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")));
			AddQuestUserData("CaptainComission2", "sName", pchar.GenQuest.CaptainComission.Name);
			AddQuestUserData("CaptainComission2", "sCity", XI_ConvertString("Colony" + pchar.GenQuest.CaptainComission.City + "Gen"));
			OfficersReaction("good");
			pchar.GenQuest.CaptainComission.MayorTalkGood = true;
			pchar.GenQuest.CaptainComission = "MayorTalkGood";
			pchar.quest.CaptainComission_TimeIsOver.over = "yes";
			SetFunctionTimerConditionParam("CaptainComission_GetSecretTimeIsOut", 0, 0, 2, MakeInt(24 - GetHour()), false);
			CaptainComission_GenerateCaptainInPrison();
			DialogExit();
		break;

		case "CapComission_Mayor9":
			dialog.text = "Great! And where should we look for his treasures?";
			link.l1 = "I don't know yet. He hasn't given me the exact location yet.";
			link.l1.go = "CapComission_Mayor10";
		break;

		case "CapComission_Mayor10":
			dialog.text = "Hmm, what a coincidence! He hasn't 'pointed out' the exact location to me either...";
			link.l1 = "But I'm working on it.";
			link.l1.go = "CapComission_Mayor11";
		break;

		case "CapComission_Mayor11":
			dialog.text = "Not anymore. Guards!.. Escort the prisoner back to the casemate.You can be free, " + GetFullName(pchar) + ".";
			link.l1 = "But, Mr. Governor...";
			link.l1.go = "CapComission_Mayor12";
		break;

		case "CapComission_Mayor12":
			sld = CharacterFromID("CapComission_1");
			sld.lifeDay = 0;
			LAi_group_MoveCharacter(sld, "EnemyFight");
			LAi_SetActorType(sld);
            LAi_ActorRunToLocation(sld, "reload", "reload1_back", "none", "", "", "", -1);
			pchar.quest.CapComission_DeletePrisonGroup.win_condition.l1 = "ExitFromLocation";
            pchar.quest.CapComission_DeletePrisonGroup.win_condition.l1.location = pchar.location;
            pchar.quest.CapComission_DeletePrisonGroup.win_condition = "CaptainComission_DeletePrisonGroup";
			ChangeOfficersLoyality("bad_all", 1);
			ChangeCharacterReputation(pchar, -4);
			AddQuestRecord("CaptainComission2", "23");
			AddQuestUserData("CaptainComission2", "sSex", GetSexPhrase("","a"));
			CloseQuestHeader("CaptainComission2");
			DeleteAttribute(pchar, "GenQuest.CaptainComission");
			DialogExit();
		break;

		case "CapComission_Mayor13":
			pchar.quest.CaptainComission_SpeakMayorGoods.over = "yes";
			dialog.text = "Great! And where should we look for his treasures?";
			link.l1 = "In " + RandPhraseSimple(RandPhraseSimple("north","west"),RandPhraseSimple("east","south")) + " extremities " + XI_ConvertString(pchar.GenQuest.CaptainComission.ConvoyShore + "Gen") + " at the foot of the cliff there is a large niche, the entrance to which is littered with stones. This is the hiding place.";
			link.l1.go = "CapComission_Mayor14";
		break;

		case "CapComission_Mayor14":
			dialog.text = "Great job! And where is he " + pchar.GenQuest.CaptainComission.Name + " ? I was informed that you took him away from the fort's cellars.";
			link.l1 = "Unfortunately, he died in a skirmish with smugglers.";
			link.l1.go = "CapComission_Mayor15";
		break;

		case "CapComission_Mayor15":
			dialog.text = "It's bad... This is very bad, "+ GetSexPhrase("sir","miss") +" " + GetFullName(pchar) + ". I wanted to execute him publicly, as a warning, so to speak, to the other officers. " +
				"Well, anyway, he couldn't avoid the gallows anyway...So, since you formally violated the law by arbitrarily releasing a prisoner, and even did not save him, I cannot but record this in the protocol. Therefore, you will not receive any reward... But I would like to thank you personally on my own. Accept my gift-a very useful thing for a combat captain.";
			link.l1 = "Thank you. Always proud to serve.";
			link.l1.go = "CapComission_Mayor16";
		break;

		case "CapComission_Mayor16":
			AddQuestRecord("CaptainComission2", "30");
			AddQuestUserData("CaptainComission2", "sCity", XI_ConvertString("Colony" + pchar.GenQuest.CaptainComission.City + "Gen"));
			CloseQuestHeader("CaptainComission2");
			GiveItem2Character(pchar, pchar.GenQuest.CaptainComission.Prize);
			ChangeCharacterReputation(pchar, 4);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 10);
			OfficersReaction("good");
			DeleteAttribute(pchar,"GenQuest.CaptainComission.MayorTalkGood");
			DeleteAttribute(pchar, "GenQuest.CaptainComission");
			DialogExit();
		break;
		// --------------------------Операция 'Галеон'-----------------------------

		// ---------------------- найти кольцо в борделе ----------------------------
		case "TakeRing_1":
			dialog.text = "I know that you are... how should I put it... "+ GetSexPhrase("frequenter of brothels","visiter of brothels") +". That's why I decided to ask you for help\n"+
				"You know, I'm ashamed to admit, but I was there recently...";
			link.l1 = "Hmm, why be ashamed of it? It's all right, attraction and all that...";
			link.l1.go = "TakeRing_2";
		break;
		case "TakeRing_2":
			dialog.text = "Heh, I'm not ashamed of that. I drank too much there and lost my wedding ring!..";
			link.l1 = "Wow, this is a serious problem, I'm sorry.";
			link.l1.go = "TakeRing_3";
		break;
		case "TakeRing_3":
			dialog.text = "Hmm, no sympathy required. We need to return the ring. Find me before midnight, and if successful, I will be very generous. And if I fail, I'm finished.";
			link.l1 = "Understood... And where exactly could you have lost it? Is there anything you can say more or less accurately?";
			link.l1.go = "TakeRing_4";
		break;
		case "TakeRing_4":
			dialog.text = "Alas, no. I don't remember anything...";
			link.l1 = "Clear. Well, let's look for it.";
			link.l1.go = "TakeRing_5";
			link.l2 = "You know, " + GetAddress_FormToNPC(NPChar) + ", I'll probably give up this job. Please excuse me...";
			link.l2.go = "TakeRing_6";
		break;
		case "TakeRing_5":
			dialog.text = "Very good! But remember, you have to find him before midnight.";
			link.l1 = "I remember. Wait for me with the news.";
			link.l1.go = "exit";
			pchar.questTemp.different = "TakeMayorsRing";
			pchar.questTemp.different.TakeMayorsRing = "toBrothel";
			pchar.questTemp.different.TakeMayorsRing.Id = GetFullName(npchar);
			pchar.questTemp.different.TakeMayorsRing.city = npchar.city;
			pchar.questTemp.different.TakeMayorsRing.price = sti(pchar.rank)*100+1000;
			sld = ItemsFromID("MayorsRing");
			sld.CityName = XI_ConvertString("Colony" + npchar.city + "Gen");
			//ложим кольцо в итем
			if (rand(3) != 2)
			{
				switch (rand(2))
				{
					case 0: sTemp = "_Brothel_room";  break;
					case 1: sTemp = "_Brothel";  break;
					case 2: sTemp = "_SecBrRoom";  break;
				}
				sld.shown = true;
				sld.startLocation = pchar.questTemp.different.TakeMayorsRing.city + sTemp;
				sld.startLocator = "item" + (rand(4)+1);
				pchar.questTemp.different.TakeMayorsRing.item = true; //флаг кольцо валяется в итемах
				Log_QuestInfo("The ring is located in the location " + sld.startLocation + ", in the locator " + sld.startLocator);
			}
			SetTimerFunction("TakeMayorsRing_null", 0, 0, 1); //освобождаем разрешалку на миниквесты
			ReOpenQuestHeader("SeekMayorsRing");
			AddQuestRecord("SeekMayorsRing", "1");
			AddQuestUserData("SeekMayorsRing", "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserData("SeekMayorsRing", "sName", pchar.questTemp.different.TakeMayorsRing.Id);
		break;
		case "TakeRing_6":
			dialog.text = "Is that so? Hmm, I didn't expect it, I didn't expect it... Well, have a nice day, Captain.";
			link.l1 = "Goodbye.";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, -2);
		break;

		case "TakeRing_S1":
			dialog.text = "Great, this is really it!! You just saved my life!";
			link.l1 = "Well, what are you...";
			link.l1.go = "TakeRing_S2";
			TakeItemFromCharacter(pchar, "MayorsRing");
		break;
		case "TakeRing_S2":
			dialog.text = "Here is your money for the work done. And know that I am very grateful to you!";
			link.l1 = "Hmm, I'll keep in mind...";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 10);
			AddCharacterExpToSkill(pchar, "Leadership", 20);
			AddCharacterExpToSkill(pchar, "Fortune", 20);
			AddMoneyToCharacter(pchar, sti(pchar.questTemp.different.TakeMayorsRing.price));
			pchar.questTemp.different = "free";
			pchar.quest.TakeMayorsRing_null.over = "yes"; //снимаем таймер
			AddQuestRecord("SeekMayorsRing", "7");
			AddQuestUserData("SeekMayorsRing", "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			CloseQuestHeader("SeekMayorsRing");

			AchievementsCounter_genquests(1);

			DeleteAttribute(pchar, "questTemp.different.TakeMayorsRing");
		break;
		case "TakeRing_S3":
			dialog.text = "Wow!! We should drink less...";
			link.l1 = "That's for sure! Well, here's your ring.";
			link.l1.go = "TakeRing_S4";
		break;
		case "TakeRing_S4":
			dialog.text = "Yes, yes, thank you very much. I am your debtor! Here's your money for the work you've done.";
			link.l1 = "Thank you " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "exit";
			TakeItemFromCharacter(pchar, "MayorsRing");
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 10);
			AddCharacterExpToSkill(pchar, "Leadership", 20);
			AddMoneyToCharacter(pchar, sti(pchar.questTemp.different.TakeMayorsRing.price));
			pchar.questTemp.different = "free";
			pchar.quest.TakeMayorsRing_null.over = "yes"; //снимаем таймер
			AddQuestRecord("SeekMayorsRing", "7");
			AddQuestUserData("SeekMayorsRing", "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			CloseQuestHeader("SeekMayorsRing");

			AchievementsCounter_genquests(1);

			DeleteAttribute(pchar, "questTemp.different.TakeMayorsRing");
		break;
		// ---------------------- найти кольцо в борделе ----------------------------

		case "node_1":
            //ОСАДЫ homo
			if (CheckAttribute(Colonies[FindColony(npchar.City)],"Siege"))
            {

                makearef(aData, NullCharacter.Siege);


                if (CheckAttribute(aData, "HelpColony") && sti(aData.HelpColony) == true )
                {
                    dialog.text = "We've already discussed everything, haven't we?";
                    link.l1 = "That's right - I won't keep you any longer.";
                    link.l1.go = "exit";

                }
                else
                {
                    dialog.text = "I must disappoint you, but at the moment I don't have time to receive you. As you probably already know, our colony was attacked by "+NationNamePeople(sti(aData.nation))+". And I need to organize the defense of the city.";
                    link.l1 = "That's what I wanted - offer your help in the defense of the fort.";
                    link.l1.go = "siege_task";
                    link.l2 = "Then I won't keep you any longer.";
        			link.l2.go = "exit";
                }
            }
            else
            {//->
                makearef(aData, NullCharacter.Siege);
                if (CheckAttribute(aData, "PlayerHelpMayor") && sti(aData.PlayerHelpMayor) == true
                && CheckAttribute(aData, "HelpColony") && sti(aData.HelpColony) == true )
                {
                    // LEO: pay only for assault ships actually sank/captured (reward accrued by class), no freebie for watching
                    DeleteAttribute(aData, "PlayerHelpMayor");
                    DeleteAttribute(aData, "HelpColony");
                    if (sti(aData.PlayerSiegeKills) > 0)
                    {
                        AddMoneyToCharacter(Pchar, sti(aData.PlayerSiegeReward));
                        ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 20);
                        ChangeCharacterReputation(PChar, 10);
                        AddCharacterExpToSkill(GetMainCharacter(), "Leadership", 80);
                        AddCharacterExpToSkill(GetMainCharacter(), "Sailing", 100);
                        AddCharacterExpToSkill(GetMainCharacter(), "Accuracy", 100);
                        AddCharacterExpToSkill(GetMainCharacter(), "Cannons", 100);
                        SiegeRumour("Rumor has it that you helped us fight off "+NationNameSK(sti(aData.nation))+" squadrons - and defend our colony! We are so grateful to you, "+ GetAddress_Form(NPChar)+".", aData.Colony, sti(aData.conation), -1, 15, 3);
                        dialog.text = "Ah, it's you, "+GetAddress_Form(NPChar)+" "+GetFullName(pchar)+", glad to see you. Good news for our colony - we fought off the "+NationNameSK(sti(aData.nation))+" invaders, and much of that is your merit: you sank and captured "+sti(aData.PlayerSiegeKills)+" of the enemy's ships. The treasury pays you "+sti(aData.PlayerSiegeReward)+" gold, just as promised. Please receive it.";
                        link.l1 = "Thank you, " + GetAddress_FormToNPC(NPChar) + ". Nice doing business with you!";
                        link.l1.go = "exit";
                    }
                    else
                    {
                        // LEO: 0 sank/captured by player - a word of thanks, no gold
                        ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 5);
                        dialog.text = "Ah, it's you, "+GetAddress_Form(NPChar)+" "+GetFullName(pchar)+". Well, we fought off the "+NationNameSK(sti(aData.nation))+" invaders - though, to be honest, I saw no hand of yours in the sea battle: not one enemy ship went down or was boarded by you. The treasury has nothing to reward you for, no offense. But for not standing aside - my thanks.";
                        link.l1 = "I understand, " + GetAddress_FormToNPC(NPChar) + ". Next time I'll prove myself in the fight.";
                        link.l1.go = "exit";
                    }
                }
                else
                {
                    if (CheckAttribute(aData, "PlayerHelpMayor")) DeleteAttribute(aData, "PlayerHelpMayor");
                    dialog.text = "So what is the reason that made you come here and distract me from important state affairs?";
        			link.l1 = "I want to talk about working for the benefit of the Crown of " + NationNameGenitive(sti(NPChar.nation))+".";
        			link.l1.go = "work";

        			link.l2 = "I need to talk to you about something.";
					if(CheckAttribute(pchar,"questTemp.ReasonToFast") && !CheckAttribute(pchar,"questTemp.ReasonToFast.SpeakOther") && !CheckAttribute(pchar, "questTemp.ReasonToFast.cantSpeakOther") && (pchar.questTemp.ReasonToFast.city == NPChar.city))
					{
						link.l2.go = "ReasonToFast_Mayor1";
					}
					else
					{
						link.l2.go = "quests";
					}
					if(CheckAttribute(pchar,"GenQuest.CaptainComission") && (pchar.GenQuest.CaptainComission.variant == "A2") && (pchar.GenQuest.CaptainComission.City == NPChar.city))
					{
						if(pchar.GenQuest.CaptainComission == "Begin_1" && !CheckAttribute(pchar,"GenQuest.CaptainComission.SpeakMayor"))
						{
							link.l12 = "You have captain of patrol " + GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")) + " with name " + pchar.GenQuest.CaptainComission.Name + " on serve, how may I see him?";
							link.l12.go = "CapComission_Mayor1";
						}
						if(CheckAttribute(pchar,"GenQuest.CaptainComission.PrisonFree"))
						{
							link.l12 = "Ex-captain of patrol " + GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")) + " " + pchar.GenQuest.CaptainComission.Name + " agreed to show his treasury to you.";
							link.l12.go = "CapComission_Mayor9";
						}
						if(pchar.GenQuest.CaptainComission == "GetGoodsSuccess")
						{
							link.l12 = "Ex-captain of patrol " + GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.CaptainComission.ShipType),"Name") + "Acc")) + " " + pchar.GenQuest.CaptainComission.Name + " showed his treasury to me.";
							link.l12.go = "CapComission_Mayor13";
						}
					}
        			if (GetPrisonerQty() > 0)
        			{
                        link.l3 = "I need to hand over the captive captain to the authorities.";
        			    link.l3.go = "sell_prisoner";
        			}
        			link.l4 = "I would like to know where the Governor General of " + NationNameGenitive(sti(NPChar.nation)) + " is.";
        			link.l4.go = "Where_guber";
        			link.l5 = "Maybe you want to take a break from important things?";
        			link.l5.go = "Play_Game";
        			link.l10 = "This is just a courtesy call and nothing more, " + GetAddress_FormToNPC(NPChar) + ".";
        			link.l10.go = "node_2";
                }
    		}//<-
		break;

		case "Play_Game":
			dialog.text = "Hmm, and which games do you prefer?";
			link.l1 = "Shall we switch to cards for a solid interest?";
			link.l1.go = "Card_Game";
   			link.l2 = "Roll the dice in a poker joker?";
			link.l2.go = "Dice_Game";
			link.l10 = "Please forgive me, but I have things to do.";
			link.l10.go = "exit";
		break;

		//  карты -->
        case "Card_Game":
            if (!CheckNPCQuestDate(npchar, "Card_date_Yet") || sti(PChar.rank) < 4 || isBadReputation(pchar, 60) || GetCharacterSkillToOld(pchar, SKILL_LEADERSHIP) < (2 + rand(2)) )
            {
                SetNPCQuestDate(npchar, "Card_date_Yet");
                dialog.text = "I will not gamble with different crooks!";
    			link.l1 = "As you wish.";
    			link.l1.go = "exit";
			}
			else
			{
				if (CheckNPCQuestDate(npchar, "Card_date_begin"))
				{
					dialog.text = "Come on! Rest has never been harmful to health... only the wallet...";
	    			link.l1 = "Wonderful.";
	    			link.l1.go = "Cards_begin";
	    			link.l2 = "What are the rules of the game?";
	    			link.l2.go = "Cards_Rule";
    			}
    			else
    			{
					dialog.text = "No, I've had enough for today. Things are waiting.";
	    			link.l1 = "As you wish.";
	    			link.l1.go = "exit";
				}
			}
		break;

		case "Cards_Rule":
   			dialog.text = XI_ConvertString("CardsRules");
			link.l1 = "Well, let's get started!";
			link.l1.go = "Cards_begin";
			link.l3 = "No, it's not for me...";
			link.l3.go = "exit";
		break;

		case "Cards_begin":
			dialog.text = "Let's decide on the bid.";
			link.l1 = "Playing 1000 coins per card.";
			link.l1.go = "Cards_Node_100";
			link.l2 = "Let's play 5000 gold per card.";
			link.l2.go = "Cards_Node_500";
			link.l3 = "I think I should go.";
			link.l3.go = "exit";
		break;

		case "Cards_Node_100":
            if (!CheckCardsGameSmallRate())
		    {
                dialog.text = "You have the fame of an unsurpassed cheat. I won't play with you.";
                link.l1 = "Everyone is lying! Well, you don't need to.";
			    link.l1.go = "exit";
			    break;
		    }

			if (sti(pchar.Money) < 3000)
		    {
                dialog.text = "Are you kidding me, " + GetAddress_Form(NPChar) + "? You don't have enough money!";
                link.l1 = "It happens.";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 3000)
		    {
                dialog.text = "Everything! We need to stop playing cards, otherwise they will write us down as embezzlers and dismiss us from office...";
                link.l1 = "Sorry.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Okay, let's play for 1000 coins.";
			link.l1 = "Let's get started!";
			link.l1.go = "Cards_begin_go";
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 1000;
            pchar.GenQuest.Cards.SitType   = false;
		break;

		case "Cards_Node_500":
            if (!CheckCardsGameSmallRate())
		    {
                dialog.text = "You have the fame of an unsurpassed cheat. I won't play with you.";
                link.l1 = "Everyone is lying! Well, you don't need to.";
			    link.l1.go = "exit";
			    break;
		    }
			if (!CheckCardsGameBigRate())
		    {
                dialog.text = "I heard that you play very well. I won't play with you at such high stakes.";
                link.l1 = "Let's get lower rates?";
			    link.l1.go = "Cards_Node_100";
				link.l2 = "Sorry, I have to go.";
			    link.l2.go = "exit";
			    break;
		    }

			if (sti(pchar.Money) < 15000)
		    {
                dialog.text = "Are you kidding me, " + GetAddress_Form(npchar) + "? You don't have enough money!";
                link.l1 = "They will!";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 15000)
		    {
                dialog.text = "No, such bets will not bring the treasury of the city to good.";
                link.l1 = "Whatever.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Okay, we're playing for 5,000 coins.";
			link.l1 = "Let's get started!";
			link.l1.go = "Cards_begin_go";
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 5000;
            pchar.GenQuest.Cards.SitType   = false;
		break;

		case "Cards_begin_go":
            SetNPCQuestDate(npchar, "Card_date_begin");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
            LaunchCardsGame();
		break;
	    // карты <--

	    //  Dice -->
        case "Dice_Game":
            if (!CheckNPCQuestDate(npchar, "Dice_date_Yet") || sti(PChar.rank) < 4 || isBadReputation(pchar, 60) || GetCharacterSkillToOld(pchar, SKILL_LEADERSHIP) < (2+rand(2)) )
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
			dialog.text = "Let's decide on the bid.";
			link.l1 = "We play 500 coins per dice.";
			link.l1.go = "Dice_Node_100";
			link.l2 = "Let's play 2000 gold per dice.";
			link.l2.go = "Dice_Node_500";
			link.l3 = "I think I should go.";
			link.l3.go = "exit";
		break;

		case "Dice_Node_100":
            if (!CheckDiceGameSmallRate())
		    {
                dialog.text = "You have the fame of an unsurpassed cheat. I won't play with you.";
                link.l1 = "Everyone is lying! Well, you don't need to.";
			    link.l1.go = "exit";
			    break;
		    }

			if (sti(pchar.Money) < 3000)
		    {
                dialog.text = "Are you kiddnig me, " + GetAddress_Form(NPChar) + "? You don't have enough money!";
                link.l1 = "It happens.";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 3000)
		    {
                dialog.text = "That's it! We need to stop playing games, otherwise they will sign up for embezzlement and be removed from office...";
                link.l1 = "Sorry.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Okay, let's play for 500 coins.";
			link.l1 = "Let's get started!";
			link.l1.go = "Dice_begin_go";
			pchar.GenQuest.Dice.npcharIdx = npchar.index;
            pchar.GenQuest.Dice.iRate     = 500;
            pchar.GenQuest.Dice.SitType   = false;
		break;

		case "Dice_Node_500":
            if (!CheckDiceGameSmallRate())
		    {
                dialog.text = "You have the fame of an unsurpassed cheat. I won't play with you.";
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

			if (sti(pchar.Money) < 15000)
		    {
                dialog.text = "Are you kidding me, " + GetAddress_Form(NPChar) + "? You don't have enough money!";
                link.l1 = "They will!";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 15000)
		    {
                dialog.text = "No, such bets will not bring the treasury of the city to good.";
                link.l1 = "Whatever.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Okay, we're playing 2000 coins per die.";
			link.l1 = "Let's get started!";
			link.l1.go = "Dice_begin_go";
			pchar.GenQuest.Dice.npcharIdx = npchar.index;
            pchar.GenQuest.Dice.iRate     = 2000;
            pchar.GenQuest.Dice.SitType   = false;
		break;

		case "Dice_begin_go":
            SetNPCQuestDate(npchar, "Dice_date_begin");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
            LaunchDiceGame();
		break;
	    // Dice <--
		case "node_2":
			dialog.snd = "Voice\CHMA\CHMA004";
			dialog.text = "In that case, I will ask you to leave my office and no longer interfere with my work.";
			link.l1 = "Yes, yes, of course, sorry to bother you.";
			link.l1.go = "exit";
		break;

		case "Where_guber":
            offref = characterFromID(NationShortName(sti(NPChar.nation))+"_guber");

            if (NPChar.location == offref.location)
            {
                dialog.text = "Open your eyes, ignorant!!! His  " + RandPhraseSimple("Grace", "Grace") + " General-Governor "+NationNameGenitive(sti(NPChar.nation))+ " " + GetFullName(offref)+
                              " is right here in the room";
    			link.l1 = "Thank you. Forgive my tactlessness.";
    			link.l1.go = "exit";
            }
            else
            {
                if (bBettaTestMode || GetSummonSkillFromNameToOld(Pchar, SKILL_LEADERSHIP) > rand(5) || CheckCharacterItem(Pchar, "patent_" + NationShortName(sti(NPChar.nation))))// когда патен всегда
                {
                    cn = FindLocation(offref.location);
                	if(cn < 0)
                    {
                        attrLoc = "error";
                        dialog.text = "His " + RandPhraseSimple("Grace", "Grace") + " General-Governor "+NationNameGenitive(sti(NPChar.nation))+ " " + GetFullName(offref)+
                                      " has gone to Europe.";
                    }
                	else
                	{
                	    attrLoc = locations[cn].islandId;
                	    if (attrLoc == "Mein")
                	    {
        			    	dialog.text = "His " + RandPhraseSimple("Grace", "Grace") + " General-Governor "+NationNameGenitive(sti(NPChar.nation))+ " " + GetFullName(offref)+
                                  LinkRandPhrase(" is now on the mainland ",
                                            " is engaged in important public affairs on the mainland ",
                                            " is visiting the mainland ") + " in the " + GetConvertStr(locations[cn].fastreload + " Town", "LocLables.txt") + ".";
						}
						else
						{
						    dialog.text = "His " + RandPhraseSimple("Grace", "Grace") + " General-Governor "+NationNameGenitive(sti(NPChar.nation))+ " " + GetFullName(offref)+
                                  LinkRandPhrase(" is now on the island ",
                                            " is engaged in important public affairs on the island ",
                                            " is visiting the island ") + GetConvertStr(attrLoc, "LocLables.txt") + " in the " + GetConvertStr(locations[cn].fastreload + " Town", "LocLables.txt") + ".";
						}
                    }
        			link.l1 = "Thank you so much!";
        			link.l1.go = "exit";
    			}
    			else
    			{
                    dialog.text = "Why should I tell you an important state secret? How do I know you're not a spy?";
        			link.l1 = "No, not at all. Please forgive me.";
        			link.l1.go = "exit";
    			}
			}
		break;
		//---------------------------- генератор квестов мэра -------------------------------
		case "work":
            dialog.text = "This is a bug, notify Eddie about the presence of it.";
        	link.l1 = "Okay, buddy!";
        	link.l1.go = "exit";
        	// boal квесты от ВМЛ -->
            if (CheckAttribute(pchar, "GenQuest.GhostShipDone_" + NPChar.id))
			{
				dialog.text = "The enemy is destroyed. The city is saved! Here is your reward.";
				link.l1 = "Proud to serve you.";
				link.l1.go = "GhostShipDone";
				break;
			}
			if (CheckAttribute(NPChar, "from_sea") && Pchar.location.from_sea == NPChar.from_sea)
            {
    			if (!CheckAttribute(pchar, "GenQuest.GhostShip.LastBattle") &&
                     CheckAttribute(pchar, "GenQuest.GhostShip.NeedCoins")  &&
                    GetNpcQuestPastDayWOInit(npchar, "GhostShipHelp") > 30  &&
                    !CheckAttribute(pchar, "GenQuest.GhostShipWorkId")      &&
                    dRand(2) == 1)
                {
                    dialog.text = "Yes, " + GetAddress_Form(NPChar) + RandPhraseSimple("Hurry up, save our city!! The living dead in the port are fighting with the defenders. I do not know what to do!",
                                                       ", you are on time! I have been informed that a battle is underway in the port. Pirates are storming the city. Help us destroy them.");
    				link.l1 = RandPhraseSimple("Difficult, but possible!",
                                               "Oh! This case is just what I need!");
    				link.l1.go = "GhostShipHelp_1";
    				break;
                }
            }
            // boal квесты от ВМЛ <--
			//==> взят ли какой-либо квест
			if (CheckAttribute(pchar, "GenQuest.questName"))
			{
				//--> Федот, да не тот
				QuestName = pchar.GenQuest.questName;
				if (pchar.GenQuest.(QuestName).MayorId != npchar.id)
				{
					dialog.text = RandPhraseSimple("As far as I know, you already have a task from the governor " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.(QuestName).MayorId)].city+"Gen") + ". Until you complete the previous tasks, I will not give you new ones.",
						"Hmm, aren't you in " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.(QuestName).MayorId)].city+"Voc") + " did you take on certain assignments from the local governor? Yes, that's right! Until you fulfill your obligations, I will not entrust you with anything new.");
					link.l1 = RandPhraseSimple("Clear...", "I got it, "+ GetAddress_FormToNPC(NPChar) + ".");
					link.l1.go = "exit";
					break;
				}
				//<--Федот, да не тот
				//-------- взят квест уничтожения банды ---------->>
				if (QuestName == "DestroyGang")
				{
					switch (pchar.GenQuest.DestroyGang)
					{
						makearef(arName, pchar.GenQuest.DestroyGang);
						case "":
							dialog.text = LinkRandPhrase("The task to search for a bandit by name " + GetFullName(arName) + " you have already received. Do it!",
								"You already have a task to search for a bandit named " + GetFullName(arName) + ". I'm waiting for the results!",
								"While you're lounging here in my residence, " + GetFullName(arName) + " is robbing our colonists! Immediately carry out the task assigned to you!");
							link.l1 = RandPhraseSimple("Okay, " + GetAddress_FormToNPC(NPChar) + ".", "I'm working on it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time given to you to find and destroy the bandit is over. And what do you think they're reporting to me? And I'll tell you - " + GetFullName(arName) + " is still alive and well! Kindly explain, " + GetAddress_Form(NPChar) + ".",
								"All the deadlines given to you for the destruction of this scoundrel named " + GetFullName(arName) + " are out. However, I am informed that there is no progress. What's the matter?");
							link.l1 = "I failed your task, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";
						break;
						case "Found":
							dialog.text = RandPhraseSimple("So, what do you say? Did you manage to destroy the said bandit? " + GetFullName(arName) + " finally killed?", "Tell me just one thing - " + GetFullName(arName) + "alive or dead?");
							link.l1 = "Unfortunately alive, " + GetAddress_FormToNPC(NPChar) + ". I managed to find him, but my forces were unequal and I had to retreat. I'm sorry, this bandit is too tough for me.";
							link.l1.go = "All_Found";
						break;
						case "Execute":
							if (CheckAttribute(pchar,"Kamikazi") && pchar.Kamikazi == "Meet")
							{
								dialog.text = "What? Are you still alive, Captain?";
								link.l1 = "So you knew there was a skeleton roaming the jungle that could blow up anyone you met? You bastard!";
								link.l1.go = "All_Execute_1";
								pchar.(sNationname).quest.mayor = sti(pchar.(sNationname).quest.mayor) + 1;
								AddCharacterExpToSkill(PChar, "Leadership", 500);
								AddCharacterExpToSkill(PChar, "Sneak", 500);
							}
							else
							{
								dialog.text = RandPhraseSimple("So, what do you say? Did you manage to destroy the said bandit? " + GetFullName(arName) + " finally killed?", "Tell me just one thing - " + GetFullName(arName) + "alive or dead?");
								link.l1 = "He's dead, " + GetAddress_FormToNPC(NPChar) + ".";
								link.l1.go = "All_Execute";
								AddCharacterExpToSkill(PChar, "Leadership", 60);
								AddCharacterExpToSkill(PChar, "Sneak", 20);
							}

						break;
					}
				}
				//------ поиска шпиона ------------>>
				if (QuestName == "TakePirateship")
				{
					switch (pchar.GenQuest.TakePirateship)
					{
						case "":
							dialog.text = LinkRandPhrase("You have already received your orders to destroy the pirate. Carry them out!",
								"You are already tasked with destroying that pirate ship. I await your report!",
								"While you idle here in my residence, that vile pirate is hunting for his next victim! Attend to your assigned duty at once!");
							link.l1 = RandPhraseSimple("Very well, " + GetAddress_FormToNPC(NPChar) + ".", "I am on it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time allotted to you for finding and destroying the pirate has expired. And what do you suppose I am told? That vile pirate continues to plunder our ships! I demand an explanation, " + GetAddress_Form(NPChar) + ".",
								"All deadlines granted to you for destroying the pirate ship have passed. There are no results. What happened?");
							link.l1 = "I was unable"+ GetSexPhrase("","") +" to complete the task within the appointed time, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";
						break;
						case "Found":
							dialog.text = RandPhraseSimple("So, what have you to report? Did you destroy the pirate ship?", "Tell me but one thing — is that vile pirate at the bottom of the sea?");
							link.l1 = "Regrettably, no, " + GetAddress_FormToNPC(NPChar) + ". I managed to track down the scoundrel, but I could not sink his ship.";
							link.l1.go = "All_Found";
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("So, what have you to report? Did you destroy the pirate ship?", "Tell me but one thing — is that vile pirate at the bottom of the sea?");
							link.l1 = "Yes, " + GetAddress_FormToNPC(NPChar) + ". This scoundrel shall never again trouble our merchants' peaceful vessels.";
							link.l1.go = "All_Execute";
							AddCharacterExpToSkill(PChar, "Leadership", 150);
							AddCharacterExpToSkill(PChar, "Fortune", 200);
							AddCharacterExpToSkill(PChar, "Sneak", 50);
						break;
					}
				}
				if (QuestName == "SeekSpy")
				{
					switch (pchar.GenQuest.SeekSpy)
					{
						case "":
							dialog.text = LinkRandPhrase("You have already received the task to search for an enemy spy in the city. Do it!",
								"You already have a mission to find an enemy agent in the city. I'm waiting for the results!",
								"While you are lounging here in my residence, an enemy spy has found out another state secret! Start searching immediately!");
							link.l1 = RandPhraseSimple("Okay, " + GetAddress_FormToNPC(NPChar) + ".", "I'm working on it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";
							link.l2 = "I couldn't find the spy, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l2.go = "All_Late";
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time given to you to search for a spy is up. And I understand that you have no result.",
								"All the deadlines given to you for detecting an enemy agent have expired. However, I am informed that there is no progress. What's the matter?");
							link.l1 = "I couldn't find the spy, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("Have you managed to find the spy?", "Tell me, have you found an enemy agent?");
							link.l1 = "Yes, I am found and killed him. He was hiding in one of the houses of the city.";
							link.l1.go = "All_Execute";
							pchar.(sNationname).quest.mayor = sti(pchar.(sNationname).quest.mayor) + 1;
							AddCharacterExpToSkill(PChar, "Leadership", 60);
							AddCharacterExpToSkill(PChar, "Sneak", 20);
						break;
						case "FoundSpy":
							dialog.text = RandPhraseSimple("So, Captain, I assume that the person next to you is the one we were looking for?", "Yeah, apparently, I can safely send the person next to you to jail. Is this the enemy agent we're looking for?");
							link.l1 = "Yes, and he's in front of you. A cunning beast, I'll tell you, I barely found him...";
							link.l1.go = "All_Execute";
							sld = characterFromId("MayorQuestSpy");
							sld.lifeDay = 0;
							ChangeCharacterAddress(sld, "none", "");
							LAi_SetStayType(sld);
							sTemp = npchar.city + "_town";
							LocatorReloadEnterDisable(sTemp, "gate_back", false);
							LocatorReloadEnterDisable(sTemp, "reload1_back", false);
							LocatorReloadEnterDisable(sTemp, "reload2_back", false);
							AddCharacterExpToSkill(PChar, "Leadership", 60);
							AddCharacterExpToSkill(PChar, "Sneak", 20);
						break;
					}
				}
				//------ проникновение во враждебный город ------------>>
				if (QuestName == "Intelligence")
				{
					switch (pchar.GenQuest.Intelligence)
					{
						case "":
							if (GetQuestPastDayParam("GenQuest.Intelligence") > sti(pchar.GenQuest.Intelligence.Terms))
							{
								dialog.text = RandPhraseSimple("The time given to you for the delivery of the letter has expired. And I understand that you have no result.",
									"All the deadlines provided to you for the delivery of the letter have expired. However, as I understand it, there is no progress. What's the matter?");
								link.l1 = "I couldn't manage your task, " + GetAddress_FormToNPC(NPChar) + ".";
								link.l1.go = "All_Late";
							}
							else
							{
								dialog.text = LinkRandPhrase("You have already received the task to deliver the letter to me. Do it!",
									"You already have a task to deliver the letter to me personally. I'm waiting for the results, I need this letter as soon as possible!",
									"While you are hanging out here at my residence, the information I need is still with the one who should send it! Start the task immediately, deliver the letter to me!");
								link.l1 = RandPhraseSimple("Okay, " + GetAddress_FormToNPC(NPChar) + ".", "I'll do it right now, "+ GetAddress_FormToNPC(NPChar) + ".");
								link.l1.go = "exit";
							}
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("Have you delivered the letter I expected?", "Tell me quickly, do you have the package?!");
							link.l1 = "Yes, I did everything. As we agreed, I hand it over personally.";
							if (GetQuestPastDayParam("GenQuest.Intelligence") > sti(pchar.GenQuest.Intelligence.Terms))
							{
								link.l1.go = "Intelligence_ExecuteLate";
								AddCharacterExpToSkill(PChar, "Sneak", 200);
							}
							else
							{
								link.l1.go = "All_Execute";
								AddCharacterExpToSkill(PChar, "Leadership", 100);
								AddCharacterExpToSkill(PChar, "Sneak", 200);
							}
							pchar.(sNationname).quest.mayor = sti(pchar.(sNationname).quest.mayor) + 1;
							TakeItemFromCharacter(pchar, "sicretLetter");
							DeleteAttribute(ItemsFromID("sicretLetter"), "City");
							DeleteAttribute(ItemsFromID("sicretLetter"), "Mayor");
						break;
					}
				}
				//------ уничтожение контры ------------>>
				if (QuestName == "KillSmugglers")
				{
					switch (pchar.GenQuest.KillSmugglers)
					{
						case "":
							dialog.text = LinkRandPhrase("You have already received the task to destroy the smugglers. Do it!",
								"You already have a task to find and destroy smugglers. I'm waiting for the results!",
								"While you are lounging here at my residence, the smugglers have brought another batch of goods into the city! Immediately take up the fulfillment of your obligations!");
							link.l1 = RandPhraseSimple("Okay, " + GetAddress_FormToNPC(NPChar) + ".", "I'm working on it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time given to you to destroy the smugglers is over. And I understand that you have no result.",
								"All the deadlines provided to you for the search and destruction of smugglers have expired. However, I am informed that there is no progress. What's the matter?");
							link.l1 = "I admit I failed to meet the deadlines. It's true, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";
						break;
						case "Execute":
							int lngFileID = LanguageOpenFile("LocLables.txt");
							sTemp = LanguageConvertString(lngFileID, pchar.GenQuest.KillSmugglers.shoreId);
							dialog.text = RandPhraseSimple("Have you managed to curb smuggling?", "Tell me, have you destroyed the smugglers?");
							link.l1 = "Yes, I caught up they in the " + sTemp + " and killed them all.";
							link.l1.go = "All_Execute";
							pchar.(sNationname).quest.mayor = sti(pchar.(sNationname).quest.mayor) + 1;
							AddCharacterExpToSkill(PChar, "Leadership", 50);
							AddCharacterExpToSkill(PChar, SKILL_SNEAK, 100);
						break;
					}
				}
				//------ найти пирата в море ------------>>
				if (QuestName == "DestroyPirate")
				{
					switch (pchar.GenQuest.DestroyPirate)
					{
						case "":
							dialog.text = LinkRandPhrase("You have already received the task to search for the pirate ship. Do it!",
								"You already have a mission to search for and destroy a pirate ship. I'm waiting for the results!",
								"While you are lounging here in my residence, pirates have robbed another merchant ship! Immediately take up the fulfillment of your obligations!");
							link.l1 = RandPhraseSimple("Okay, " + GetAddress_FormToNPC(NPChar) + ".", "I'm working on it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time given to you to find and destroy the pirate is over. And I understand that you have no result.",
								"All the deadlines provided to you for the search and destruction of the pirate ship have expired. However, I am informed that there is no progress. What's the matter?");
							link.l1 = "I confess, I was unable to find the ship within the allotted time. It's true, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("Tell me just one thing - is this damn pirate alive or dead?", "Tell me, is this damn pirate alive or not?");
							link.l1 = "He and his crew are dead, and his ship is at the bottom, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Execute";
							pchar.(sNationname).quest.mayor = sti(pchar.(sNationname).quest.mayor) + 1;
							AddCharacterExpToSkill(PChar, "Leadership", 50);
							AddCharacterExpToSkill(PChar, SKILL_FORTUNE, 50);
							AddCharacterExpToSkill(PChar, SKILL_SNEAK, 50);
							//здесь снимаем торговцев
							for (i=0; i<5; i++)
							{
								Group_DeleteGroup("MQGroupAll_" + i);
							}
						break;
						case "Found":
							dialog.text = RandPhraseSimple("So, what do you say? Did you manage to complete the task? Is this unfortunate pirate finally killed?", "Tell me just one thing - is that pirate alive or dead?");
							link.l1 = "Unfortunately alive, " + GetAddress_FormToNPC(NPChar) + ". I managed to find the ship, but the forces were unequal, and I had to retreat. I'm sorry, this pirate is too much for me.";
							link.l1.go = "All_Found";
						break;
						case "PirateIsOut":
							dialog.text = RandPhraseSimple("I was informed that you had caught up with a pirate and were fighting with him. What are the results?", "It became known to me that you gave battle to some vessel in the surrounding waters. So what happened there?");
							link.l1 = "I tracked down a pirate ship, and we entered the battle. But unfortunately, " + GetAddress_FormToNPC(NPChar) + ", pirates managed to escape from the battlefield. I'm really sorry...";
							link.l1.go = "All_Found";
						break;
					}
				}
				if (QuestName == "TakePostcureer")
				{
					switch (pchar.GenQuest.TakePostcureer)
					{
						case "":
							dialog.text = LinkRandPhrase("You have already received your orders to seize the courier ship. Begone and execute them!",
								"You are already tasked with capturing that courier vessel. I expect results, not idle talk!",
								"While you are idling here in my residence, the documents I require are heading straight into enemy hands! Proceed with your mission immediately!");
							link.l1 = RandPhraseSimple("Very well, " + GetAddress_FormToNPC(NPChar) + ".", "I am on it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time allotted to you for finding and seizing the courier ship has expired. And what do you suppose I am told? The documents I awaited have fallen into enemy hands! I demand an explanation, " + GetAddress_Form(NPChar) + ".",
								"All deadlines granted to you for seizing the documents from the courier ship have passed. There are no results. What happened?");
							link.l1 = "I was unable"+ GetSexPhrase("","") +" to complete the task within the appointed time, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";
						break;
						case "Found":
							dialog.text = RandPhraseSimple("So, what have you to report? Did you seize the packet of documents?", "Tell me but one thing — do you have the correspondence?");
							link.l1 = "Regrettably, no, " + GetAddress_FormToNPC(NPChar) + ". I managed to track down the ship, but I could not seize the documents.";
							link.l1.go = "All_Found";
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("So, what have you to report? Did you seize the packet of documents?", "Tell me but one thing — do you have the correspondence?");
							link.l1 = "Yes, " + GetAddress_FormToNPC(NPChar) + ". I am ready to hand it over to you. Here it is.";
							link.l1.go = "All_Execute";
							AddCharacterExpToSkill(PChar, "Leadership", 100);
							AddCharacterExpToSkill(PChar, "Fortune", 200);
							AddCharacterExpToSkill(PChar, "Sneak", 50);
							RemoveItems(pchar, "sicretLetter", 1);
						break;
					}
				}
				if (QuestName == "TakeArsenalship")
				{
					switch (pchar.GenQuest.TakeArsenalship)
					{
						case "":
							dialog.text = LinkRandPhrase("You have already received your orders to destroy the munitions transport. Begone and execute them!", 
								"You are already tasked with sinking that military transport. I expect results, not idle talk!", 
								"While you are idling here in my residence, gunpowder and munitions are heading straight to the enemy cannons! Proceed with your mission immediately!");
							link.l1 = RandPhraseSimple("Very well, " + GetAddress_FormToNPC(NPChar) + ".", "I am on it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";		
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time allotted to find and destroy the munitions transport has expired. And what do my scouts report? The enemy convoy has reached its destination safely! I expect an explanation, " + GetAddress_Form(NPChar) + ".",
								"All deadlines for destroying the enemy munitions transport have passed. There are no results. What happened?");
							link.l1 = "I was unable"+ GetSexPhrase("","") +" to complete the task within the appointed time, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";		
						break;
						case "Found":
							dialog.text = RandPhraseSimple("So, what news? Did you sink the enemy munitions transport?", "Tell me but one thing — is the enemy arsenal at the bottom of the sea?");
							link.l1 = "Regrettably, no, " + GetAddress_FormToNPC(NPChar) + ". I managed to track down the convoy, but I could not send the target ship to the locker.";
							link.l1.go = "All_Found";		
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("So, what news? Did you sink the enemy munitions transport?", "Tell me but one thing — is the enemy arsenal at the bottom of the sea?");
							link.l1 = "Yes, " + GetAddress_FormToNPC(NPChar) + ". Their gunpowder and munitions shall never reach the enemy's hands.";
							link.l1.go = "All_Execute";		
							AddCharacterExpToSkill(PChar, "Leadership", 120);
							AddCharacterExpToSkill(PChar, "Fortune", 200);
							AddCharacterExpToSkill(PChar, "Sneak", 50);
						break;
					}
				}
				//-------- ОЗГ - пассажир ---------->>
				if (QuestName == "TakePassenger")
				{
					switch (pchar.GenQuest.TakePassenger)
					{
						case "":
							dialog.text = LinkRandPhrase("You have already received your orders to track down the criminal. Begone!", 
								"You are already tasked with finding that scoundrel. I await your report!", 
								"While you waste time here, that vile knave is sailing freely! Execute your commission at once!");
							link.l1 = RandPhraseSimple("As you wish, " + GetAddress_FormToNPC(NPChar) + ".", "I am handling the matter, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";		
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time given to find the scoundrel has expired. And do you know what I am told? He has safely reached his destination and vanished! I expect an explanation, " + GetAddress_Form(NPChar) + ".",
								"All deadlines for locating the criminal have passed. No results. What is the meaning of this?");
							link.l1 = "I failed"+ GetSexPhrase("","") +" to complete your assignment in the allotted time, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";		
						break;
						case "Found":
							dialog.text = RandPhraseSimple("So, what say you? Did you capture the villain?", "Tell me only this — is that wretched scoundrel in irons in your hold?");
							link.l1 = "Unfortunately, no, " + GetAddress_FormToNPC(NPChar) + ". I was unable to take the knave alive. He went down with the ship he was aboard.";
							link.l1.go = "All_Found";		
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("So, what say you? Did you capture the villain?", "Tell me only this — is that wretched scoundrel in irons in your hold?");
							link.l1 = "Yes, " + GetAddress_FormToNPC(NPChar) + ". Order your soldiers to the pier, and I shall deliver him ashore immediately.";
							link.l1.go = "All_Execute";		
							AddCharacterExpToSkill(PChar, "Leadership", 150);
							AddCharacterExpToSkill(PChar, "Fortune", 200);
							AddCharacterExpToSkill(PChar, "Sneak", 100);
							sld = &Characters[sti(Pchar.GenQuest.TakePassenger.PrisonerIDX)];
							ReleasePrisoner(sld); //освободили пленника
							sld.lifeday = 0;
						break;
					}
				}
				//-------- таможенный патруль ---------->>
				if (QuestName == "CustomPatrol")
				{
					switch (pchar.GenQuest.CustomPatrol)
					{
						case "":
							dialog.text = LinkRandPhrase("The mission to hunt down the smugglers is already yours. Execute it!", 
								"You are already tasked with intercepting the smugglers. I await the results!", 
								"While you lounge here in the residence, those smugglers are calmly preparing their deal! Perform your duty immediately!");
							link.l1 = RandPhraseSimple("Very well, " + GetAddress_FormToNPC(NPChar) + ".", "I am working on it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";		
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time granted to find the smugglers has run out. And what news do I receive? They have successfully offloaded their cargo! Explain yourself, " + GetAddress_Form(NPChar) + ".",
								"All deadlines for hunting the smugglers have passed. No results. What is your excuse?");
							link.l1 = "I was unable"+ GetSexPhrase("","") +" to complete your assignment in time, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";		
						break;
						case "Found":
							dialog.text = RandPhraseSimple("Well? Have you dealt with the smugglers?", "Tell me but one thing — has the smuggling deal been thwarted?");
							link.l1 = "Alas, no, " + GetAddress_FormToNPC(NPChar) + ". I was unable to stop the scoundrels.";
							link.l1.go = "All_Found";		
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("Well? Have you dealt with the smugglers?", "Tell me but one thing — has the smuggling deal been thwarted?");
							link.l1 = "Indeed, " + GetAddress_FormToNPC(NPChar) + ". I intercepted their vessels and sent them both — the seller and the buyer — to the bottom.";
							link.l1.go = "All_Execute";		
							AddCharacterExpToSkill(PChar, "Leadership", 100);
							AddCharacterExpToSkill(PChar, "Fortune", 200);
							AddCharacterExpToSkill(PChar, "Sneak", 200);
						break;
					}
				}
				//-------- Дезертир ---------->>
				if (QuestName == "FindFugitive")
				{
					switch (pchar.GenQuest.FindFugitive)
					{
						case "":
							dialog.text = LinkRandPhrase("Your orders to find the deserter have already been issued. Carry them out!", 
								"You are already tasked with tracking down the deserter. I expect results!", 
								"While you idle here, that deserter may be selling state secrets! Proceed with your search at once!");
							link.l1 = RandPhraseSimple("Of course, " + GetAddress_FormToNPC(NPChar) + ".", "I am handling it, "+ GetAddress_FormToNPC(NPChar) + ".");
							link.l1.go = "exit";		
						break;
						case "Late":
							dialog.text = RandPhraseSimple("The time allotted for the search has run out. And what news do I have? None at all! Explain yourself, " + GetAddress_Form(NPChar) + ".",
								"All deadlines for locating the deserter have passed. No results. Why have you failed?");
							link.l1 = "I was unable"+ GetSexPhrase("","") +" to complete the task in the appointed time, " + GetAddress_FormToNPC(NPChar) + ".";
							link.l1.go = "All_Late";		
						break;
						case "Found":
							dialog.text = RandPhraseSimple("So, what news? Have you captured the deserter?", "Tell me only this — is that runaway in irons in your hold?");
							link.l1 = "Unfortunately, no, " + GetAddress_FormToNPC(NPChar) + ". I failed to seize the scoundrel. I found his trail in "+XI_ConvertString("Colony"+pchar.GenQuest.FindFugitive.City)+", but a mere day before my arrival, he fled on a pirate vessel to parts unknown. I fear he has joined the Brethren of the Coast...";
							link.l1.go = "All_Found";		
						break;
						case "Execute":
							dialog.text = RandPhraseSimple("Aha... so here is our "+pchar.GenQuest.FindFugitive.Name+"... Excellent work, Captain! Was the hunt a difficult one?", "Oh, look what guests we have! Good day to you, "+pchar.GenQuest.FindFugitive.Name+"! Splendid work, Captain! Did the search exhaust much of your strength?");
							link.l1 = "Hard to say, " + GetAddress_FormToNPC(NPChar) + ". Be that as it may — I have fulfilled your commission.";
							link.l1.go = "All_Execute";		
							AddCharacterExpToSkill(PChar, "Leadership", 200);
							AddCharacterExpToSkill(PChar, "Fortune", 200);
							AddCharacterExpToSkill(PChar, "Sneak", 100);
							sld = &Characters[sti(Pchar.GenQuest.FindFugitive.PrisonerIDX)];
							ReleasePrisoner(sld); //освободили пленника
							LAi_SetActorType(sld);
							sld.lifeday = 0;
							pchar.quest.FindFugitive_Over.over = "yes"; //снимаем таймер
							chrDisableReloadToLocation = false;//открыть локацию
						break;
					}
				}
				//<<------ взятые квесты ------------
				break;
			}
			if (!CheckAttribute(npchar, "work_date") || GetNpcQuestPastDayParam(npchar, "work_date") > 2 || bBettaTestMode)
    		{
				SaveCurrentNpcQuestDateParam(npchar, "work_date");
				if (rand(10) > 9 && !bBettaTestMode)             // WW     с 20 процентов до 10 процентов   просто посылание  уменьшил
				{
					dialog.text = LinkRandPhrase("I don't have a job for you right now. Come back tomorrow, or something...",
						"I have no assignment for you today. Come back later, in a day or two.",
						"There is nothing today that I can entrust to you. I'll be glad to see you another day.");
					link.l1 = "I got it, " + GetAddress_FormToNPC(NPChar) + ".";
					link.l1.go = "exit";
				}
				else
				{
						if (npchar.city == "Panama") i = drand(6);
						else i = dRand(7);
                    // boal <--
					switch (i)
					{
						//========== квест уничтожения банды ============
						case 0:
							pchar.GenQuest.DestroyGang.Terms = dRand(2) + 2; //сроки выполнения задания
							pchar.GenQuest.DestroyGang.Money = ((dRand(8) + 2) * 170) + (sti(pchar.rank) * 200 + 2000); //вознаграждение
							makearef(arName, pchar.GenQuest.DestroyGang);
							arName.nation = PIRATE;
							arName.sex = "man";
							SetRandomNameToCharacter(arName); //имя бандита в структуру квеста
							dialog.text = "Right now I have a task for you. In the jungle near " + XI_ConvertString("Colony"+npchar.city+"Gen") + " a gang of robbers has appeared, and I know that the leader in it is a certain " + GetFullName(arName) + ". Find and destroy this band of robbers.";
							link.l1 = "Am I limited in time?";
							link.l1.go = "DestroyGang";
						break;
						//========== розыска лазутчика ============
						case 1:
							pchar.GenQuest.SeekSpy.Terms = dRand(2) + 3; //сроки выполнения задания
							pchar.GenQuest.SeekSpy.Money = ((dRand(8) + 2) * 150) + (sti(pchar.rank) * 200 + 2000); //вознаграждение
							dialog.text = "Yes, now I have a responsible assignment for you. I received information that in " + XI_ConvertString("Colony"+npchar.city+"Voc") + " is an enemy spy. You need to find him.";
							link.l1 = "Tell me, " + GetAddress_FormToNPC(NPChar) + ", is the information you received reliable?";
							link.l1.go = "SeekSpy";
						break;
						//========== пробраться во вражеский город ============
						case 2:
							sTemp = GetSpyColony(npchar); //враждебная колония
							if (sTemp == "none")
							{
								dialog.text = LinkRandPhrase("I don't have a job for you right now. Come back tomorrow, or something...",
									"I have no assignment for you today. Come back later, in a day or two.",
									"There is nothing today that I can entrust to you. I'll be glad to see you another day.");
								link.l1 = "I got it, " + GetAddress_FormToNPC(NPChar) + ".";
								link.l1.go = "exit";
							}
							else
							{
								pchar.GenQuest.Intelligence.Terms = dRand(10) + (42 - MOD_SKILL_ENEMY_RATE*3); //сроки выполнения задания
								pchar.GenQuest.Intelligence.Money = ((dRand(4) + 6) * 1000) + (sti(pchar.rank) * 1000 + 10000); //вознаграждение
								pchar.GenQuest.Intelligence.City = sTemp;
								sTemp = ", which is on " + XI_ConvertString(colonies[FindColony(pchar.GenQuest.Intelligence.City)].islandLable+"Voc");
								dialog.text = "I have a task for you that involves serious risk. I need you to get into " + XI_ConvertString("Colony"+pchar.GenQuest.Intelligence.City+"Acc") + sTemp + ", met the specified person there and delivered to me what he will give you.";
								link.l1 = "Uh, I wouldn't say that in " + XI_ConvertString("Colony"+pchar.GenQuest.Intelligence.City+"Voc") + " will be happy to see me...";
								link.l1.go = "Intelligence";
							}
						break;
						//========== ОЗГ - охотник на пирата ============
						case 3:
							dialog.text = "Indeed, I have a mission of great importance for you, one that will test your every skill as a mariner. It concerns the pirate scum...";
							link.l1 = "Pirates? Might I learn the details of what is expected of me?";
							link.l1.go = "TakePirateship";
						break;	
						//========== контркурьер - отобрать почту ============
						case 4:
							pchar.GenQuest.TakePostcureer.City = SelectEnemyColony(npchar, &enemyNation, &city2, &enemyNation);
							if (pchar.GenQuest.TakePostcureer.City == "none")
							{
								dialog.text = LinkRandPhrase("I don't have a job for you right now. Come back tomorrow, or something...",
									"I have no assignment for you today. Come back later, in a day or two.",
									"There is nothing today that I can entrust to you. I'll be glad to see you another day.");
								link.l1 = "I got it, " + GetAddress_FormToNPC(NPChar) + ".";
								link.l1.go = "exit";
							}
							else
							{
								pchar.GenQuest.TakePostcureer.CityA = city2;
								pchar.GenQuest.TakePostcureer.Nation = enemyNation;
								dialog.text = "Yes, I have a commission that demands the utmost haste as well as tactical prowess in naval engagement. Are you prepared to prove your worth?";
								link.l1 = "Your Excellency, could you elaborate on the nature of this assignment?";
								link.l1.go = "TakePostcureer";
							}
						break;
						//========== контрфрахт - потопить корабль с боеприпасами ============
						case 5:
							pchar.GenQuest.TakeArsenalship.City = SelectEnemyColony(npchar, &enemyNation, &city2, &enemyNation);
							if (pchar.GenQuest.TakeArsenalship.City == "none")
							{
								dialog.text = LinkRandPhrase("I don't have a job for you right now. Come back tomorrow, or something...",
									"I have no assignment for you today. Come back later, in a day or two.",
									"There is nothing today that I can entrust to you. I'll be glad to see you another day.");
								link.l1 = "I got it, " + GetAddress_FormToNPC(NPChar) + ".";
								link.l1.go = "exit";
							}
							else
							{
								pchar.GenQuest.TakeArsenalship.CityA = city2;
								pchar.GenQuest.TakeArsenalship.Nation = enemyNation;//вражеская нация
								dialog.text = "Indeed, I have a task that requires extraordinary courage and a masterful hand in naval combat. Do you feel ready to distinguish yourself?";
								link.l1 = "Your Excellency, could you elaborate on the nature of this assignment?";
								link.l1.go = "TakeArsenalship";
							}
						break;
						//========== ОЗГ - охотник на пассажира ============
						case 6:
							pchar.GenQuest.TakePassenger.City = SelectEnemyColony(npchar, &enemyNation, &city2, &enemyNation);
							if (pchar.GenQuest.TakePassenger.City == "none")
							{
								dialog.text = LinkRandPhrase("I don't have a job for you right now. Come back tomorrow, or something...",
									"I have no assignment for you today. Come back later, in a day or two.",
									"There is nothing today that I can entrust to you. I'll be glad to see you another day.");
								link.l1 = "I got it, " + GetAddress_FormToNPC(NPChar) + ".";
								link.l1.go = "exit";
							}
							else
							{
								pchar.GenQuest.TakePassenger.CityA = city2;
								pchar.GenQuest.TakePassenger.Nation = enemyNation;
								dialog.text = "You have arrived at a fortuitous hour, sir. I have an urgent matter that I believe can be entrusted to your hands. It involves the pursuit and capture of a certain individual...";
								link.l1 = "A manhunt? Pray, tell me more about what this task entails.";
								link.l1.go = "TakePassenger";
							}
						break;
						//========== замочить пирата, что грабит суда ============
						case 7:
							//--> генерим ранг
							iRank = GetQuestEnemyRank();
							//<-- генерим ранг
							LAi_group_Delete("MayorPirateGroup");
							sld = GetCharacter(NPC_GenerateCharacter("MQPirate", "", "man", "man", iRank, sti(npchar.nation), -1, true));
							SetCaptanModelByEncType(sld, "pirate");
							DeleteAttribute(sld, "Ship");
							SetShipToFantom(sld, "pirate", false);
							sld.Ship.Mode = "Pirate";
							sld.dialog.filename = "MayorQuests_dialog.c";
							sld.dialog.currentnode = "DestroyPirate_Abordage";
							sld.DeckDialogNode = "DestroyPirate_Deck";
							sld.DontRansackCaptain = true;
							sld.DontCheckFlag = true;  // не проверять флаг ГГ
							sld.SaveItemsForDead = true; // сохранять на трупе вещи
							sld.DontClearDead = true; // не убирать труп через 200с
							sld.AlwaysSandbankManeuver = true;
							sld.WatchFort = true; //видеть форты
							sld.AnalizeShips = true;  //анализировать вражеские корабли при выборе таска
							sld.greeting = "pirat_common";
							LAi_group_MoveCharacter(sld, "MayorPirateGroup");
							dialog.text = "Yes, I have a task for you. The fact is that in the waters near " + XI_ConvertString("Colony"+npchar.city+"Gen") + " a pirate ship has appeared. Several merchant ships have already been robbed. The most interesting thing is that the victims cannot really tell anything about either the pirate ship or the captain of this ship himself. Afraid, I suppose...";
							link.l1 = "And what should I do, " + GetAddress_FormToNPC(NPChar) + "?";
							link.l1.go = "DestroyPirate";
						break;
					}
				}
			}
			else
			{
				dialog.text = LinkRandPhrase("Today I can't offer anything as a job anymore.", "Today there is no work for you anymore.", "That's it for today, there is no work. All questions about work are tomorrow.");
				link.l1 = RandPhraseSimple("Alas...", "Alas, "+ GetAddress_FormToNPC(NPChar) + ". ");
        		link.l1.go = "exit";
			}
		//---------------------------- генератор квестов мэра -------------------------------
		break;

		case "Patent_Doubt":
			dialog.text = "Lay it smoothly... I should have you hanged for just being impertinent. There is no faith in such notorious pirates, but now times are hard in our archipelago and any help will be useful. So be it, I'll sign a patent for you,";
			link.l1 = "You won't regret it, " + GetAddress_FormToNPC(NPChar) + "!";
			link.l1.go = "Patent_Give";
			link.l2 = "Ha! Be led! It was just a joke, I don't want to have anything to do with your worthless state.";
			link.l2.go = "arest_3";
		break;
		case "Patent_Give":
            pchar.PatentNation = NationShortName(sti(NPChar.nation));
            pchar.PatentPrice = 0;
			dialog.text = "Please, I hope for further cooperation. Now you can visit our Governor General. He may have more serious tasks for you.";
			link.l1 = "Thank you. Goodbye, " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "exit";
			AddDialogExitQuest("any_patent_take");
			ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", -100);
		break;

		case "sell_prisoner":
            dialog.text = "Who do you want to release for ransom?";

            qty = 1;
            for(i=0; i < GetPassengersQuantity(pchar); i++)
            {
                cn = GetPassenger(pchar,i);
                if(cn != -1)
                {
                    offref = GetCharacter(cn);
                    if(CheckAttribute(offref,"prisoned"))
                    {
        	            if(sti(offref.prisoned)==true && GetRemovable(offref)) // ставим только фантомов
        	            {
                            attrLoc = "l"+qty;
                            link.(attrLoc)    = GetFullName(offref) + " - " + NationNameMan(sti(offref.nation)) + ".";
                            link.(attrLoc).go = "GetPrisonerIdx_" + offref.index;
                            qty++;
                        }
                    }
                }
            }
			link.l99 = "No one.";
			link.l99.go = "exit";
		break;

		case "sell_prisoner_2":
            offref = GetCharacter(sti(pchar.GenQuest.GetPrisonerIdx));
            attrLoc =  "Okay, there is captain " + GetFullName(offref) + ", " + NationNameMan(sti(offref.nation))+ ".";
            // цена зависит от губернатора
            qty = makeint(sti(offref.rank)*(800 + GetCharacterSPECIALSimple(NPChar, SPECIAL_L)*100) + GetCharacterSkillToOld(offref, "Leadership")*500 + GetCharacterSkillToOld(pchar, "commerce")*500);
            if (sti(offref.nation) == sti(NPChar.nation))
            {
                attrLoc = attrLoc + " I'm ready to redeem my compatriot for " + FindRussianMoneyString(qty) + ".";
            }
            else
            {
                if (sti(offref.nation) == PIRATE)
                {
                    qty = qty / 5;
                    attrLoc = attrLoc + " I can give " + FindRussianMoneyString(qty) + " for that gallows man. And we'll immediately execute the villain.";
                }
                else
                {
                    qty = qty / 2;
                    attrLoc = attrLoc + " For this man I can pay no more than " + FindRussianMoneyString(qty) + ".";
                }
            }
			dialog.text = attrLoc;
			pchar.PrisonerSellPrice =  qty;
			link.l1 = "Deal with it, he is all your's " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "sell_prisoner_3";
			link.l3 = "No. That won't do.";
			link.l3.go = "exit";
		break;
		case "sell_prisoner_3":
			dialog.text = "That's nice.";
			link.l1 = "Have a nice day.";
			link.l1.go = "exit";
			OfficersReaction("bad");

			offref = GetCharacter(sti(pchar.GenQuest.GetPrisonerIdx));
			AddMoneyToCharacter(pchar, sti(pchar.PrisonerSellPrice));
			if (sti(offref.nation) == sti(NPChar.nation))
            {
                AddCharacterExpToSkill(pchar, "Commerce", 25);
            }
            else
            {
                if (sti(offref.nation) == PIRATE)
                {
                    ChangeCharacterReputation(pchar, -2);
                }
                else
                {
                    AddCharacterExpToSkill(pchar, "Commerce", 5);
                }
            }

        	offref.location = "";
        	ReleasePrisoner(offref); // освободили пленника
		break;
		////   CAPTURE //////////
		case "CAPTURE_Main":
            NextDiag.TempNode = "CAPTURE_Main";
			AfterTownBattle();  // все, все свободны

            // fix от грабежа 5 раз на дню -->
            if (!CheckNPCQuestDate(npchar, "GrabbingTownDate"))
			{
                dialog.Text = "You've already taken everything. What else do you need?";
                Link.l1 = "Yes, exactly, I mistaken the city.";
                Link.l1.go = "Exit_City";

                NPChar.NoGiveMoney = true;

            	ChangeCharacterHunterScore(GetMainCharacter(), NationShortName(sti(NPChar.nation)) + "hunter", 20);
                // вернём диалог после разговора и спрячем
                SetReturn_Gover_Dialog_Exit(NPChar);
				break;
			}
			SetNPCQuestDate(npchar, "GrabbingTownDate");
			// fix от грабежа 5 раз на дню <--

            if (CheckAttribute(FortChref, "Fort.Mode") && sti(FortChref.Fort.Mode) != FORT_DEAD)
            { // а форт-то ЖИВ, значит с суши прошли
                dialog.Text = "What an unheard-of impudence! Reinforcements from the fort will arrive soon, and you will be in trouble!";
                Link.l2 = "I'm not going to stick around until he gets here. Pay me and we'll leave this city.";
                Link.l2.go = "Summ";
                Pchar.HalfOfPaymentByCity = true; // токо половина денег
            }
            else
            {
				dialog.Text = "This time you took it, but know that our squadron will arrive soon, and there will be no wet spot left of you.";
				if(npchar.id == "Santiago_mayor" && CheckAttribute(pchar, "PGGWhisperLetterSent"))
				{
					dialog.Text = GetFullName(pchar) + "! I should have known that your news was just a ruse to disarm my city. And I believed you like a naive child. " + dialog.Text;

				}
	            if (!bWorldAlivePause || bBettaTestMode)
	            {
	                int iColony = FindColony(npchar.city);
					if (!CheckAttribute(&colonies[iColony], "notCaptured")) //незахватываемые города
					{
						if (CheckAttribute(FortChref, "Default.Prison"))
	    				{
	    					Link.l1 = "Talk more! This city belongs to me from now on, and I will send your squadron on a date with the sea devil. Hey guys, take this piggy and put him in jail.";
	    				}
	    				else
	    				{
	    					Link.l1 = "This city belongs to me from now on, and I will decide how to protect it myself. Hey guys, take that upstart out.";
	    				}
	    				Link.l1.go = "City";
					}
				}
				if (!isCityHasFort(NPChar.City))
	            {
	                Link.l2 = "The landing squadron? Hmm... pay me and we'll leave this city.";
				}
				else
				{
	            	Link.l2 = "The squadron...Hmmm.. Pay me and we'll leave this city.";
	            }
				Link.l2.go = "Summ";

                bOk = !bWorldAlivePause || bBettaTestMode;
	            if (isMainCharacterPatented() && bOk && !CheckAttribute(&colonies[iColony], "notCaptured")) //не даём захватить колонию
	            {
					// восстановим нацию патента
					PChar.nation = GetBaseHeroNation();

					dialog.Text = "This is an unheard-of audacity! How dare you attack a colony belonging to " + NationNameGenitive(sti(NPChar.nation)) + "?! You will pay dearly for the blood you have wantonly shed and for the damage done to our country.";
	                Link.l1 = "From now on, the colony belongs to me, and if anyone tries to doubt it, they will go straight to heaven.";
	                Link.l1.go = "City_patent";
	                Link.l2 = "I represent the interests of " + NationNameGenitive(sti(PChar.nation)) + " and I officially declare that from now on this colony belongs to " + NationKingsCrown(PChar) + "!";
	                Link.l2.go = "City_nation";
	                Link.l3 = "We should discuss the size of the ransom. If you are stubborn, then we will turn your wonderful city into a pile of ashes, and we will hang you in the city square.";
	                Link.l3.go = "Summ_patent";
	            }
            }
        break;

        case "City":
            ChangeCharacterHunterScore(GetMainCharacter(), NationShortName(sti(NPChar.nation)) + "hunter", 40);
            AddCharacterExpToSkill(GetMainCharacter(), "Leadership", 300);

            Pchar.GenQuestFort.fortCharacterIdx = FortChref.index;
            AddDialogExitQuest("Residence_Captured_Any");

            Statistic_AddValue(Pchar, NationShortName(sti(NPChar.nation)) + "_TakeTown", 1);
            // ремонт
            RepairAllShips();
            Log_Info("All ships have been repaired.");
            //  СЖ -->
			ReOpenQuestHeader("Gen_CityCapture");
	        AddQuestRecord("Gen_CityCapture", "t3");
			AddQuestUserData("Gen_CityCapture", "sSex", GetSexPhrase("","a"));
			AddQuestUserData("Gen_CityCapture", "sCity", XI_ConvertString("colony" + NPChar.City));
			//  СЖ <--
            NextDiag.CurrentNode = "Prison";
			DialogExit();
        break;

        case "City_patent":
            ChangeCharacterReputation(GetMainCharacter(), -20); // пираты мы, но у нас патент
            dialog.Text = "This time you took it, but know that our squadron will arrive soon, and there will be no wet spot left of you.";
			Link.l1 = "Talk more! This colony belongs to me from now on, and I will send your squadron on a date with the sea devil. Hey guys, take this piggy and put him in jail.";
			Link.l1.go = "City";
        break;

        case "Exit_for_pay":
			dialog.Text = "What else do you need from me?";
            Link.l1 = "I'm looking to see if I've taken everything...";
            Link.l1.go = "exit";

            NextDiag.TempNode = "Exit_for_pay";
		break;

        case "Exit_City":
		    NextDiag.CurrentNode = "Exit_for_pay";
		    Pchar.GenQuestFort.fortCharacterIdx = FortChref.index;
		    if (sti(NPChar.NoGiveMoney) == false) // себе берём
		    {
				// вернём диалог после разговора и спрячем
				SetReturn_Gover_Dialog_Exit(NPChar);
				AddDialogExitQuestFunction("TWN_ExitForPay");
            }
            DialogExit();
        break;

        case "Summ":
            dialog.Text = "I have no choice but to agree to your terms. Take your blood money and leave the waters of our colony immediately.";
            Link.l1 = "Let me take my leave.";
            Link.l1.go = "Exit_City";
            NPChar.NoGiveMoney = false;

            ChangeCharacterHunterScore(GetMainCharacter(), NationShortName(sti(NPChar.nation)) + "hunter", 30);
            AddCharacterExpToSkill(GetMainCharacter(), SKILL_FORTUNE, 200);
            AddCharacterExpToSkill(GetMainCharacter(), "Commerce", 200);

            Statistic_AddValue(Pchar, NationShortName(sti(NPChar.nation)) + "_GrabbingTown", 1);
            //  СЖ -->
			ReOpenQuestHeader("Gen_CityCapture");
	        AddQuestRecord("Gen_CityCapture", "t2");
			AddQuestUserData("Gen_CityCapture", "sSex", GetSexPhrase("","a"));
			AddQuestUserData("Gen_CityCapture", "sCity", XI_ConvertString("colony" + NPChar.City));
			//  СЖ <--
        break;

        case "Summ_patent":
            dialog.Text = "Okay, you can dictate your terms...";
            Link.l1 = "Yes, by right of the winner. Where is my money?";
            Link.l1.go = "Summ";
            ChangeCharacterReputation(GetMainCharacter(), -10); // не на службе
        break;

        case "City_nation":
			dialog.Text = "Well, we're at the mercy of the victors.";
			Link.l1 = "I advise you to leave the city as soon as possible, it is still unknown how the upcoming changes will affect your health. Goodbye.";
			Link.l1.go = "Exit_City";
			NPChar.NoGiveMoney = true;

            ChangeCharacterReputation(GetMainCharacter(), 5);
            AddCharacterExpToSkill(GetMainCharacter(), "Leadership", 500);
            AddCharacterExpToSkill(GetMainCharacter(), "Sneak", 300);
            SetNationRelationBoth(sti(PChar.nation), sti(NPChar.nation), RELATION_ENEMY);

            PChar.questTemp.DontSetNewDialogToMayor = true; // иначе может сменить диалог и сбойнуть
            PChar.questTemp.DontNullDeposit = true;    // чтоб не нулили ростовщика
            SetCaptureTownByNation(NPChar.City, sti(PChar.nation));
            DeleteAttribute(PChar, "questTemp.DontSetNewDialogToMayor");
            AddDialogExitQuestFunction("LaunchColonyInfoScreen"); // табличка
            //  СЖ -->
	    	sTemp =  GetNationNameByType(sti(PChar.nation));
			ReOpenQuestHeader("Gen_CityCapture");
	        AddQuestRecord("Gen_CityCapture", "t1");
			AddQuestUserData("Gen_CityCapture", "sCity", XI_ConvertString("colony" + NPChar.City));
			AddQuestUserData("Gen_CityCapture", "sNation", XI_ConvertString(sTemp + "Gen"));
			//  СЖ <--
			AddTitleNextRate(sti(PChar.nation), 1);  // счётчик звания
            SetCharacterRelationBoth(sti(FortChref.index), GetMainCharacterIndex(), RELATION_FRIEND);// нечего не даёт, тк работает OtherChar - а это губер, но он и так друг
            UpdateRelations();

            // 22.03.05  fix вернём диалог после разговора и спрячем
            //  внутри диалога не работало
            SetReturn_Gover_Dialog_Exit(NPChar);

            Statistic_AddValue(Pchar, NationShortName(sti(NPChar.nation)) + "_TakeTown", 1);
	    break;

	    case "Prison":
			dialog.Text = "What else do you want from me, scoundrel?";
            Link.l3 = "I checked how is Your Excellency doing. Well, now I have to go!";
            Link.l3.go = "exit";
            NextDiag.TempNode = "Prison";
		break;

		case "arestFree_1":
			dialog.text = "Settle? How do you imagine it?";
            link.l1 = "I think " + ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", 0) * 6000 + " piastres can save me?";
			link.l1.go = "arestFree_2";
			link.l2 = "No way, I have to go. Have a nice day.";
			link.l2.go = "arest_1";
		break;

		case "arestFree_2":
            if (GetCharacterSkillToOld(PChar, SKILL_FORTUNE) >= rand(7))
            {
    			dialog.text = "Perhaps we can resolve our incident this way.";
    		    link.l1 = "Wonderful!";
    		    link.l1.go = "Exit";
				int iTemp = ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", 0);
    		    AddMoneyToCharacter(pchar, -iTemp * 6000);
    		    ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", -iTemp);//За сколько заплатили, столько и снижаем. Ровно до нуля. 
    		    AddCharacterExpToSkill(pchar, SKILL_FORTUNE, 100);
            }
            else
            {
    			dialog.text = "No! There is only one way to atone for your misdeeds. Guards! Take "+ GetSexPhrase("him", "her") +" away to the fort.";
    		    link.l1 = "Wait!";


    		    link.l1.go = "arest_2";
    		    AddCharacterExpToSkill(pchar, SKILL_FORTUNE, 10);
            }
		break;

		case "arest_1":
			dialog.text = "Of course it's time, you will be escorted. Guards! Take "+ GetSexPhrase("him", "her") +"!";
		    link.l1 = "Well, no way!";
		    link.l1.go = "fight";
		break;

		case "arest_2":
			dialog.text = "I should have thought earlier! Guards!";
			link.l1 = "Well, no way! You won't take me just like that!";
		    link.l1.go = "fight";
		break;

		case "arest_3":
			dialog.text = "Guards! Take "+ GetSexPhrase("him", "her") +"!";
			link.l1 = "Well, no way! You won't take me just like that!";
		    link.l1.go = "fight";
		break;
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	Контрфрахт - уничтожить корабль с арсеналом
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "TakeArsenalship":

			pchar.GenQuest.questName = "TakeArsenalship"; //тип квеста
			pchar.GenQuest.TakeArsenalship.Terms1 = GetMaxDaysFromIsland2Island(Islands[GetCharacterCurrentIsland(PChar)].id, GetArealByCityName(pchar.GenQuest.TakeArsenalship.City));
			pchar.GenQuest.TakeArsenalship.Terms2 = GetMaxDaysFromIsland2Island(GetArealByCityName(pchar.GenQuest.TakeArsenalship.City), GetArealByCityName(pchar.GenQuest.TakeArsenalship.CityA))+1;
			pchar.GenQuest.TakeArsenalship.Terms = sti(pchar.GenQuest.TakeArsenalship.Terms1)+sti(pchar.GenQuest.TakeArsenalship.Terms2);
			pchar.GenQuest.TakeArsenalship.ShipName = GenerateRandomNameToShip(sti(pchar.GenQuest.TakeArsenalship.Nation));
			pchar.GenQuest.TakeArsenalship.Money = (15000 + (sti(pchar.rank) * 1100)) + (drand(5000 + (sti(pchar.rank) * 300)));
			dialog.text = "Certainly. You must locate a military transport of the " + NationNameGenitive(sti(pchar.GenQuest.TakeArsenalship.Nation)) + " carrying gunpowder and munitions—a vessel named '" + pchar.GenQuest.TakeArsenalship.ShipName + "'—and destroy it. By doing so, we shall strike a significant blow to the enemy's military power.\nThis transport will depart with an escort in " + FindRussianDaysString(pchar.GenQuest.TakeArsenalship.Terms1) + " from " + XI_ConvertString("Colony"+pchar.GenQuest.TakeArsenalship.City+"Gen") + ", sailing toward " + XI_ConvertString("Colony"+pchar.GenQuest.TakeArsenalship.CityA+"Acc") + ", and is expected to arrive there in " + FindRussianDaysString(pchar.GenQuest.TakeArsenalship.Terms) + ". I shall pay you " + FindRussianMoneyString(sti(pchar.GenQuest.TakeArsenalship.Money)) + ". You must make haste to intercept it.";
			link.l1 = "Very well, I accept the task. Shall I sink the vessel or may I take it as a prize?";
		    link.l1.go = "TakeArsenalship_agree";
			link.l2 = "Hmm... no, I believe I shall decline.";
		    link.l2.go = "All_disagree";
			break;

		case "TakeArsenalship_agree":
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_complete";
			pchar.GenQuest.TakeArsenalship.MayorId = npchar.id; //Id мэра, чтобы знать, кто дал квест
			dialog.text = "As you wish, it makes no difference to me. The cargo simply must not reach its destination. Should you succeed in this commission, I shall not detain you further, Captain.";
			link.l1 = "I trust you won't have to wait long for the news, " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "exit";
			SetFunctionTimerCondition("TakeArsenalship_CreateShip", 0, 0, sti(pchar.GenQuest.TakeArsenalship.Terms1), false);
			SetTimerCondition("AllMayorsQuests_Late", 0, 0, sti(pchar.GenQuest.TakeArsenalship.Terms), false);
			ReOpenQuestHeader("MayorsQuestsList");
			AddQuestRecord("MayorsQuestsList", "25");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
			AddQuestUserData("MayorsQuestsList", "sCity", XI_ConvertString("Colony"+pchar.GenQuest.TakeArsenalship.City+"Gen"));
			AddQuestUserData("MayorsQuestsList", "sCityA", XI_ConvertString("Colony"+pchar.GenQuest.TakeArsenalship.CityA+"Acc"));
			AddQuestUserData("MayorsQuestsList", "sDay1", FindRussianDaysString(pchar.GenQuest.TakeArsenalship.Terms1));
			AddQuestUserData("MayorsQuestsList", "sDay2", FindRussianDaysString(pchar.GenQuest.TakeArsenalship.Terms));
			AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.TakeArsenalship.Money)));
			AddQuestUserData("MayorsQuestsList", "sShipName", pchar.GenQuest.TakeArsenalship.ShipName);
			AddQuestUserData("MayorsQuestsList", "sNation", NationNameGenitive(sti(pchar.GenQuest.TakeArsenalship.Nation)));
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	ОЗГ - захватить пассажира
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "TakePassenger":
			pchar.GenQuest.questName = "TakePassenger"; //тип квеста
			pchar.GenQuest.TakePassenger.Terms1 = GetMaxDaysFromIsland2Island(Islands[GetCharacterCurrentIsland(PChar)].id, GetArealByCityName(pchar.GenQuest.TakePassenger.City));
			pchar.GenQuest.TakePassenger.Terms2 = GetMaxDaysFromIsland2Island(GetArealByCityName(pchar.GenQuest.TakePassenger.City), GetArealByCityName(pchar.GenQuest.TakePassenger.CityA))+1;
			pchar.GenQuest.TakePassenger.Terms = sti(pchar.GenQuest.TakePassenger.Terms1)+sti(pchar.GenQuest.TakePassenger.Terms2);
			pchar.GenQuest.TakePassenger.ShipName = GenerateRandomNameToShip(sti(pchar.GenQuest.TakePassenger.Nation));
			pchar.GenQuest.TakePassenger.Name = GenerateRandomName(sti(pchar.GenQuest.TakePassenger.Nation), "man");
			pchar.GenQuest.TakePassenger.Money = (12000 + (sti(pchar.rank) * 800)) + (drand(3000));
			string sText = SelectPassText();
			dialog.text = "Indubitably. This concerns a wretch named " + pchar.GenQuest.TakePassenger.Name + ". " + sText + " I have been tracking him for a long time, and finally, I have obtained reliable intelligence. I need you to bring this man to me alive. I intend to make an example of him in the town square. I shall pay you " + FindRussianMoneyString(sti(pchar.GenQuest.TakePassenger.Money)) + ". Are you prepared to take on this manhunt?";
			link.l1 = "I am, " + GetAddress_FormToNPC(NPChar) + ". Where shall I find this scoundrel?";
		    link.l1.go = "TakePassenger_agree";
			link.l2 = "Hmm... no, I believe I shall decline.";
		    link.l2.go = "All_disagree";
		break;
		
		case "TakePassenger_agree":
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_complete";
			pchar.GenQuest.TakePassenger.MayorId = npchar.id; //Id мэра, чтобы знать, кто дал квест
			dialog.text = "In a mere " + FindRussianDaysString(pchar.GenQuest.TakePassenger.Terms1) + ", he will depart as a passenger aboard a ship named '" + pchar.GenQuest.TakePassenger.ShipName + "', leaving from " + XI_ConvertString("Colony"+pchar.GenQuest.TakePassenger.City+"Gen") + " and bound for " + XI_ConvertString("Colony"+pchar.GenQuest.TakePassenger.CityA+"Acc") + ". You have no more than " + FindRussianDaysString(pchar.GenQuest.TakePassenger.Terms) + " to intercept this vessel, board it, and take the villain into custody. Good luck, Captain!";
			link.l1 = "I shall not waste a moment, " + GetAddress_FormToNPC(NPChar) + "! I am setting sail immediately.";
			link.l1.go = "exit";
			SetFunctionTimerCondition("TakePassenger_CreateShip", 0, 0, sti(pchar.GenQuest.TakePassenger.Terms1), false);
			SetTimerCondition("AllMayorsQuests_Late", 0, 0, sti(pchar.GenQuest.TakePassenger.Terms), false);
			ReOpenQuestHeader("MayorsQuestsList");
			AddQuestRecord("MayorsQuestsList", "26");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
			AddQuestUserData("MayorsQuestsList", "sCity", XI_ConvertString("Colony"+pchar.GenQuest.TakePassenger.City+"Gen"));
			AddQuestUserData("MayorsQuestsList", "sCityA", XI_ConvertString("Colony"+pchar.GenQuest.TakePassenger.CityA+"Acc"));
			AddQuestUserData("MayorsQuestsList", "sDay1", FindRussianDaysString(sti(pchar.GenQuest.TakePassenger.Terms1)));
			AddQuestUserData("MayorsQuestsList", "sDay2", FindRussianDaysString(sti(pchar.GenQuest.TakePassenger.Terms)));
			AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.TakePassenger.Money)));
			AddQuestUserData("MayorsQuestsList", "sName", pchar.GenQuest.TakePassenger.Name);
			AddQuestUserData("MayorsQuestsList", "sShipName", pchar.GenQuest.TakePassenger.ShipName);
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	ОЗГ - уничтожить корабль пиратов
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "TakePirateship":
			pchar.GenQuest.questName = "TakePirateship"; //тип квеста
			pchar.GenQuest.TakePirateship.City = SelectAnyColony(npchar.city);
			pchar.GenQuest.TakePirateship.island = GetArealByCityName(pchar.GenQuest.TakePirateship.City);
			pchar.GenQuest.TakePirateship.Terms = dRand(3) + (20 - MOD_SKILL_ENEMY_RATE*3);
			pchar.GenQuest.TakePirateship.ShipName = GenerateRandomNameToShip(PIRATE);
			pchar.GenQuest.TakePirateship.Name = GenerateRandomName(PIRATE, "man");
			pchar.GenQuest.TakePirateship.Money = (10000 + (sti(pchar.rank) * 500)) + (drand(2000 + (sti(pchar.rank) * 300)));
			dialog.text = "Of course. I am utterly infuriated by the brazen activities of a pirate captain named " + pchar.GenQuest.TakePirateship.Name + ". This scoundrel has taken to plundering our merchant ships, severely hindering trade between the colonies. The time for retribution is upon us, as I have learned his current hiding place. For this task, I am prepared to pay you " + FindRussianMoneyString(sti(pchar.GenQuest.TakePirateship.Money)) + ". Are you prepared to set sail and eliminate this enemy of our Crown?";
			link.l1 = "I would be honored! Where might I find this apostate?";
		    link.l1.go = "TakePirateship_agree";
			link.l2 = "Hmm... no, I believe I shall decline.";
		    link.l2.go = "All_disagree";
		break;
		
		case "TakePirateship_agree":
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_complete";
			pchar.GenQuest.TakePirateship.MayorId = npchar.id; //Id мэра, чтобы знать, кто дал квест
			dialog.text = "Within the next " + FindRussianDaysString(pchar.GenQuest.TakePirateship.Terms) + ", his ship, the '" + pchar.GenQuest.TakePirateship.ShipName + "', will be cruising the waters near " + XI_ConvertString("Colony"+pchar.GenQuest.TakePirateship.City+"Gen") + ". Find him and destroy him. Feed him to the sharks along with his vessel, or take it by boarding—I care not, so long as this cursed pirate ceases to defile the Caribbean with his presence.\n Lose no time, Captain: the rogue will not wait for you forever. Go, and may God be with you!";
			link.l1 = "I am weighing anchor as we speak, " + GetAddress_FormToNPC(NPChar) + "! You shall have your results shortly!";
			link.l1.go = "exit";
			pchar.quest.TakePirateship.win_condition.l1 = "location";
			pchar.quest.TakePirateship.win_condition.l1.location = pchar.GenQuest.TakePirateship.island;
			pchar.quest.TakePirateship.function = "TakePirateship_CreateShip";
			SetTimerCondition("AllMayorsQuests_Late", 0, 0, sti(pchar.GenQuest.TakePirateship.Terms), false);
			ReOpenQuestHeader("MayorsQuestsList");
			AddQuestRecord("MayorsQuestsList", "23");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
			AddQuestUserData("MayorsQuestsList", "sCity", XI_ConvertString("Colony"+pchar.GenQuest.TakePirateship.City+"Gen"));
			AddQuestUserData("MayorsQuestsList", "sDay", FindRussianDaysString(sti(pchar.GenQuest.TakePirateship.Terms)));
			AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.TakePirateship.Money)));
			AddQuestUserData("MayorsQuestsList", "sName", pchar.GenQuest.TakePirateship.Name);
			AddQuestUserData("MayorsQuestsList", "sShipName", pchar.GenQuest.TakePirateship.ShipName);
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	Контркурьер
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "TakePostcureer":
			pchar.GenQuest.questName = "TakePostcureer"; //тип квеста
			pchar.GenQuest.TakePostcureer.Terms1 = GetMaxDaysFromIsland2Island(Islands[GetCharacterCurrentIsland(PChar)].id, GetArealByCityName(pchar.GenQuest.TakePostcureer.City));
			pchar.GenQuest.TakePostcureer.Terms2 = GetMaxDaysFromIsland2Island(GetArealByCityName(pchar.GenQuest.TakePostcureer.City), GetArealByCityName(pchar.GenQuest.TakePostcureer.CityA))+1;
			pchar.GenQuest.TakePostcureer.Terms = sti(pchar.GenQuest.TakePostcureer.Terms1)+sti(pchar.GenQuest.TakePostcureer.Terms2);
			pchar.GenQuest.TakePostcureer.ShipName = GenerateRandomNameToShip(sti(pchar.GenQuest.TakePostcureer.Nation));
			pchar.GenQuest.TakePostcureer.Name = GenerateRandomName(sti(pchar.GenQuest.TakePostcureer.Nation), "man");
			pchar.GenQuest.TakePostcureer.Money = (9000 + (sti(pchar.rank) * 500)) + (drand(2000 + (sti(pchar.rank) * 300)));
			dialog.text = "Certainly. You must locate a " + NationNameGenitive(sti(pchar.GenQuest.TakePostcureer.Nation)) + " courier ship named '" + pchar.GenQuest.TakePostcureer.ShipName + "', board it, and bring me the papers found in the captain's cabin. The vessel will depart from " + XI_ConvertString("Colony"+pchar.GenQuest.TakePostcureer.City+"Gen") + " in approximately " + FindRussianDaysString(pchar.GenQuest.TakePostcureer.Terms1) + " and head for " + XI_ConvertString("Colony"+pchar.GenQuest.TakePostcureer.CityA+"Acc") + ". I shall pay you " + FindRussianMoneyString(sti(pchar.GenQuest.TakePostcureer.Money)) + ". You have no more than " + FindRussianDaysString(pchar.GenQuest.TakePostcureer.Terms) + " to intercept it along the specified route.";
			link.l1 = "Very well, I accept the task. What sort of papers should I be looking for?";
		    link.l1.go = "TakePostcureer_agree";
			link.l2 = "Hmm... no, I believe I shall decline.";
		    link.l2.go = "All_disagree";
		break;
		
		case "TakePostcureer_agree":
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_complete";
			pchar.GenQuest.TakePostcureer.MayorId = npchar.id; //Id мэра, чтобы знать, кто дал квест
			dialog.text = "Postal correspondence. Among other things, it contains documents of vital importance to my interests. You need not pore through them yourself; simply deliver the entire dispatch to me.\nI shall expect you back at my residence with the results.";
			link.l1 = "I trust you won't have to wait long, " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "exit";
			SetFunctionTimerCondition("TakePostcureer_CreateShip", 0, 0, sti(pchar.GenQuest.TakePostcureer.Terms1), false);
			SetTimerCondition("AllMayorsQuests_Late", 0, 0, sti(pchar.GenQuest.TakePostcureer.Terms), false);
			ReOpenQuestHeader("MayorsQuestsList");
			AddQuestRecord("MayorsQuestsList", "24");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
			AddQuestUserData("MayorsQuestsList", "sCity", XI_ConvertString("Colony"+pchar.GenQuest.TakePostcureer.City+"Gen"));
			AddQuestUserData("MayorsQuestsList", "sCityA", XI_ConvertString("Colony"+pchar.GenQuest.TakePostcureer.CityA+"Acc"));
			AddQuestUserData("MayorsQuestsList", "sDay1", FindRussianDaysString(sti(pchar.GenQuest.TakePostcureer.Terms1)));
			AddQuestUserData("MayorsQuestsList", "sDay2", FindRussianDaysString(sti(pchar.GenQuest.TakePostcureer.Terms)));
			AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.TakePostcureer.Money)));
			AddQuestUserData("MayorsQuestsList", "sName", pchar.GenQuest.TakePostcureer.Name);
			AddQuestUserData("MayorsQuestsList", "sShipName", pchar.GenQuest.TakePostcureer.ShipName);
			AddQuestUserData("MayorsQuestsList", "sNation", NationNameGenitive(sti(pchar.GenQuest.TakePostcureer.Nation)));
		break;
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	Уничтожение банды
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "DestroyGang":
			dialog.text = "Naturally. To complete this task, I give you " + FindRussianDaysString(sti(pchar.GenQuest.DestroyGang.Terms)) + ", your reward for the work done will be " + FindRussianMoneyString(sti(pchar.GenQuest.DestroyGang.Money)) + ".";
			link.l1 = "Okay, I'm taking this assignment.";
		    link.l1.go = "DestroyGang_agree";
			link.l2 = "Hmm... no, I think I'll refuse.";
		    link.l2.go = "All_disagree";
		break;
		case "DestroyGang_agree":
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_complete";
			pchar.GenQuest.questName = "DestroyGang"; //тип квеста
			pchar.GenQuest.DestroyGang.Location = GetGangLocation(npchar); //определяем локацию,где банда
			pchar.GenQuest.DestroyGang.MayorId  = npchar.id; //Id мэра, чтобы знать, кто дал квест
			if (pchar.GenQuest.DestroyGang.Location == "none") //на всякий случай
			{
				dialog.text = "Wait, I completely forgot about yesterday's report! The fact is that this gang has already left the borders of our island. So I'm canceling the assignment.";
				link.l1 = "Understood. Well, it's a pity.";
				link.l1.go = "exit";
				DeleteAttribute(pchar, "GenQuest." + pchar.GenQuest.questName);
				DeleteAttribute(pchar, "GenQuest.questName");
			}
			else
			{
				dialog.text = "Well, great! I am waiting for you at my residence with the results of the work.";
				link.l1 = "I guess I won't keep you waiting long, " + GetAddress_FormToNPC(NPChar) + ".";
				link.l1.go = "exit";
				pchar.quest.DestroyGang.win_condition.l1 = "location";
				pchar.quest.DestroyGang.win_condition.l1.location = pchar.GenQuest.DestroyGang.Location;
				if (!CheckAttribute(pchar,"Kamikazi") && rand(99)==0) pchar.quest.DestroyGang.win_condition = "Kamikazi";
				else pchar.quest.DestroyGang.win_condition = "DestroyGang_fight";
				pchar.quest.DestroyGang.again = true; //тупо дожидаться своего часа бандиты не будут
				SetTimerCondition("AllMayorsQuests_Late", 0, 0, sti(pchar.GenQuest.DestroyGang.Terms), false);
				//==> энкаунтеров в квестовой локации не будет
				locations[FindLocation(pchar.GenQuest.DestroyGang.Location)].DisableEncounters = true;
				ReOpenQuestHeader("MayorsQuestsList");
				AddQuestRecord("MayorsQuestsList", "1");
				AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
				AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
				makearef(arName, pchar.GenQuest.DestroyGang);
				AddQuestUserData("MayorsQuestsList", "GangName", GetFullName(arName));
				AddQuestUserData("MayorsQuestsList", "sDay", FindRussianDaysString(sti(pchar.GenQuest.DestroyGang.Terms)));
				AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.DestroyGang.Money)));
			}
		break;
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	Поиски лазутчика
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "SeekSpy":
			dialog.text = "Yes, I have reason to trust it. To complete this task, I give you " + FindRussianDaysString(sti(pchar.GenQuest.SeekSpy.Terms)) + ", your reward for the work done will be " + FindRussianMoneyString(sti(pchar.GenQuest.SeekSpy.Money)) + ".";
			link.l1 = "Okay, I'm taking this assignment.";
		    link.l1.go = "SeekSpy_agree";
			link.l2 = "Hmm... no, I think I'll refuse.";
		    link.l2.go = "All_disagree";
		break;
		case "SeekSpy_agree":
			//(rand(1))
			bool boko = (rand(36) < GetCharacterSPECIALSimple(PChar, SPECIAL_E) + GetCharacterSPECIALSimple(PChar, SPECIAL_S) + GetCharacterSPECIALSimple(PChar, SPECIAL_A))     //WW
			bool orange = false;
			if (npchar.city == "FortOrange") orange = true;
			if  (IsCharacterPerkOn(pchar, "Fencer") || IsCharacterPerkOn(pchar, "Grunt") || boko)   //WW
			{
				if (rand(100)<10 || orange)
				{//шпион гуляет по городу
					pchar.GenQuest.SeekSpy.Location = npchar.city + "_town"; //определяем локацию
					pchar.GenQuest.SeekSpy.City = rand(1); //флаг шпион в городе, ренд - кто будет подсказывать в таверне
				}
				else
				{//шпион в одной из общих локаций common
					pchar.GenQuest.SeekSpy.Location = GetSpyLocation(npchar); //определяем локацию
					Log_QuestInfo(pchar.GenQuest.SeekSpy.Location); //чит
				if (pchar.GenQuest.SeekSpy.Location == "none") //на всякий случай
				{
					dialog.text = "Wait, I remember that a suspicious person was detained yesterday, apparently it was the same spy. There is no need for your services.";
					link.l1 = "Clear. Well, it's a pity.";
					link.l1.go = "exit";
					DeleteAttribute(pchar, "GenQuest." + pchar.GenQuest.questName);
					break;
				}
				else
					{
						pchar.quest.SeekSpy_login.again = true; //на всякий случай
					}
				}
			}
			else
			{	//шпион гуляет по городу
				pchar.GenQuest.SeekSpy.Location = npchar.city + "_town"; //определяем локацию
				pchar.GenQuest.SeekSpy.City = rand(1); //флаг шпион в городе, ренд - кто будет подсказывать в таверне
			}
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_complete";
			pchar.GenQuest.questName = "SeekSpy"; //тип квеста
			dialog.text = "Well, great! I am waiting for you at my residence with the search results.";
			link.l1 = "I guess I won't keep you waiting long, " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "exit";
			pchar.GenQuest.SeekSpy.MayorId  = npchar.id; //Id мэра, чтобы знать, кто дал квест
			pchar.quest.SeekSpy.win_condition.l1 = "location";
			pchar.quest.SeekSpy.win_condition.l1.location = pchar.GenQuest.SeekSpy.Location;
			pchar.quest.SeekSpy.win_condition = "SeekSpy_login";
			SetTimerCondition("AllMayorsQuests_Late", 0, 0, sti(pchar.GenQuest.SeekSpy.Terms), false);
			ReOpenQuestHeader("MayorsQuestsList");
			AddQuestRecord("MayorsQuestsList", "3");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
			AddQuestUserData("MayorsQuestsList", "sDay", FindRussianDaysString(sti(pchar.GenQuest.SeekSpy.Terms)));
			AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.SeekSpy.Money)));
		break;
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	Проникновение во враждебный город
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "Intelligence":
			dialog.text = "I understand. Perhaps a reward in " + FindRussianMoneyString(sti(pchar.GenQuest.Intelligence.Money)) + "it will be a good incentive for you.";
			link.l1 = "Yes, it's good money... I'm taking on this assignment.";
		    link.l1.go = "Intelligence_agree";
			link.l2 = "Hmm... no, I guess I'll refuse - it's too dangerous.";
		    link.l2.go = "All_disagree";
		break;
		case "Intelligence_agree":
			if (pchar.GenQuest.Intelligence.City == "none" || isBadReputation(pchar, 30)) //на всякий случай
			{
				dialog.text = "Although, I can't say that I can trust you fully... I guess I won't give you this assignment.";
				link.l1 = "Oh, however... Well, it's a pity.";
				link.l1.go = "exit";
				DeleteAttribute(pchar, "GenQuest." + pchar.GenQuest.questName);
			}
			else
			{
				npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_complete";
				pchar.GenQuest.questName = "Intelligence"; //тип квеста
				sTemp = pchar.GenQuest.Intelligence.City;
				if(sTemp == "DesMoines") // В Дес-Мойнес нет верфи и ПУ
				{
					switch (rand(2))
					{
						case 0:    sTemp += "_Priest";     break;
						case 1:    sTemp += "_trader";     break;
						case 2:    sTemp += "_usurer";     break;
					}
				}
				else
				{
					switch (rand(4))
					{
						case 0:    sTemp += "_Priest";     break;
						case 1:    sTemp += "_trader";     break;
						case 2:    sTemp += "_shipyarder"; break;
						case 3:    sTemp += "_usurer";     break;
						case 4:    sTemp += "_PortMan";    break;
					}
				}
				pchar.GenQuest.Intelligence.SpyId = sTemp; //Id нашего шпиона в городе
				dialog.text = "Fine! Now it's business. In " + XI_ConvertString("Colony"+pchar.GenQuest.Intelligence.City+"Voc") + " you need to find local " + GetWorkTypeOfMan(&characters[GetCharacterIndex(sTemp)], "Gen") +
					", his name is " + GetFullName(&characters[GetCharacterIndex(sTemp)]) + ". You will tell him my name and you will receive a package of documents. To deliver this package to me personally I give you " + FindRussianDaysString(sti(pchar.GenQuest.Intelligence.Terms)) + ". And keep in mind that everything needs to be done quietly. If you are found in the city, the agent will not be at risk and will not make contact. Is that clear to you?";
				link.l1 = "Yes, I understand everything, " + GetAddress_FormToNPC(NPChar) + ". I will begin the task immediately.";
				link.l1.go = "exit";
				pchar.GenQuest.Intelligence.MayorId  = npchar.id; //Id мэра, чтобы знать, кто дал квест
				SaveCurrentQuestDateParam("GenQuest.Intelligence"); //запись даты получения квеста
				ReOpenQuestHeader("MayorsQuestsList");
				AddQuestRecord("MayorsQuestsList", "6");
				AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
				AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
				AddQuestUserData("MayorsQuestsList", "sCity", XI_ConvertString("Colony"+pchar.GenQuest.Intelligence.City+"Acc"));
				AddQuestUserData("MayorsQuestsList", "sIsland", XI_ConvertString(colonies[FindColony(pchar.GenQuest.Intelligence.City)].islandLable+"Voc"));
				AddQuestUserData("MayorsQuestsList", "sWho", GetWorkTypeOfMan(&characters[GetCharacterIndex(sTemp)], "Gen"));
				AddQuestUserData("MayorsQuestsList", "SpyName", GetFullName(&characters[GetCharacterIndex(sTemp)]));
				AddQuestUserData("MayorsQuestsList", "sDay", FindRussianDaysString(sti(pchar.GenQuest.Intelligence.Terms)));
				AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.Intelligence.Money)));
				AddQuestUserData("MayorsQuestsList", "sCity2", XI_ConvertString("Colony"+pchar.GenQuest.Intelligence.City+"Voc"));
			}
		break;
		case "Intelligence_ExecuteLate":
			QuestName = pchar.GenQuest.questName;
			int iIntellBase = sti(pchar.GenQuest.(QuestName).Money);
			int iIntellFine = makeint(iIntellBase / 2);
			pchar.GenQuest.(QuestName).Money = iIntellBase - iIntellFine;   // LEO: было `sti(...) / 2);` - лишняя скобка

			Log_Info(XI_ConvertString("Freight_LateFineLetter"));
			Log_InfoColored("red", "-" + FindRussianMoneyStringLog(iIntellFine));
			Log_Info(XI_ConvertString("Freight_TotalCourier"));
			dialog.text = RandPhraseSimple("It's not bad. However, you did not meet the deadline, so I have to reduce the amount of your remuneration. Your fee is now " + FindRussianMoneyString(sti(pchar.GenQuest.(QuestName).Money)) + ". Please receive it.",
				"That's good. However, you did not meet the allotted time, and I was waiting for this letter earlier. You've messed up my plans, so I'm forced to reduce the amount of your fee. So, I'm still glad to give you the proper reward - " + FindRussianMoneyString(sti(pchar.GenQuest.(QuestName).Money))  + ". Please receive it.");
			link.l1 = "I wouldn't say that I am pleased, but, in the end, the deadlines were indeed violated. Therefore, I have no complaints, " + GetAddress_FormToNPC(NPChar) + ".";
		    link.l1.go = "All_Execute_1";
			//--> слухи
			AddSimpleRumour(RandPhraseSimple("You know, Governor " + GetFullName(npchar) + " not very happy with the promptness of a certain captain " + GetMainCharacterNameGen() + ".",
				"Governor " + GetFullName(npchar) + " not very happy with the captain " + GetMainCharacterNameDat() + ". Captain completed governor's orders, but didn't do it on time..."), sti(npchar.nation), 5, 1);
			//<-- слухи
		break;
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	Уничтожение контры
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "KillSmugglers":
			pchar.GenQuest.questName = "KillSmugglers"; //тип квеста
			pchar.GenQuest.KillSmugglers.Terms = rand(2) + 1; //сроки выполнения задания
			pchar.GenQuest.KillSmugglers.Money = ((rand(8) + 2) * 200) + (sti(pchar.rank) * 200); //вознаграждение
			dialog.text = "Find and destroy these smugglers, of course. To complete this task, I give you " + FindRussianDaysString(sti(pchar.GenQuest.KillSmugglers.Terms)) + ", the fee will be " + FindRussianMoneyString(sti(pchar.GenQuest.KillSmugglers.Money)) + ". Are you taking on this case?";
			link.l1 = "Yes, " + GetAddress_FormToNPC(NPChar) + ", I'll take it.";
		    link.l1.go = "KillSmugglers_agree";
			link.l2 = "No, I don't want to get involved...";
		    link.l2.go = "All_disagree";
		break;
		case "KillSmugglers_agree":
			dialog.text = "Well, fine. In that case, I'm waiting for good news from you.";
			link.l1 = "I believe they will appear soon, " + GetAddress_FormToNPC(NPChar) + ".";
		    link.l1.go = "exit";
			pchar.GenQuest.KillSmugglers.MayorId  = npchar.id; //Id мэра, чтобы знать, кто дал квест
			pchar.GenQuest.KillSmugglers.Areal = GetArealByCityName(npchar.city); //ареал, где нужно поработать
			for(i=0; i<MAX_CHARACTERS; i++)
			{
				sld = &characters[i];
				if (sld.id == "Rand_Smug01" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(sld.location)]))
				{
					pchar.quest.KillSmugglers.win_condition.l1 = "NPC_Death";
					pchar.quest.KillSmugglers.win_condition.l1.character = "Rand_Smug01";
					pchar.quest.KillSmugglers.win_condition.l2 = "NPC_Death";
					pchar.quest.KillSmugglers.win_condition.l2.character = "Rand_Smug02";
					pchar.quest.KillSmugglers.win_condition.l3 = "NPC_Death";
					pchar.quest.KillSmugglers.win_condition.l3.character = "Rand_Smug03";
					pchar.quest.KillSmugglers.win_condition = "KillSmugglers_after";
					break;
				}
			}
			SetTimerCondition("AllMayorsQuests_Late", 0, 0, sti(pchar.GenQuest.KillSmugglers.Terms), false);
			ReOpenQuestHeader("MayorsQuestsList");
			AddQuestRecord("MayorsQuestsList", "8");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
			AddQuestUserData("MayorsQuestsList", "sDay", FindRussianDaysString(sti(pchar.GenQuest.KillSmugglers.Terms)));
			AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.KillSmugglers.Money)));
		break;
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//	Поиск и уничтожение пирата
		/////////////////////////////////////////////////////////////////////////////////////////////////
		case "DestroyPirate":
			pchar.GenQuest.questName = "DestroyPirate"; //тип квеста
			pchar.GenQuest.DestroyPirate.Terms = dRand(3) + (15 - MOD_SKILL_ENEMY_RATE*3); //сроки выполнения задания
			pchar.GenQuest.DestroyPirate.Money = ((dRand(5) + 5) * 500) + (sti(pchar.rank) * 200); //вознаграждение
			sld = characterFromId("MQPirate");
			dialog.text = "I need you to track down and destroy this pirate. Sink it, board it, I don't care. I just need this damn corsair hunting in these waters not to interfere with trade" + XI_ConvertString("Colony"+npchar.city+"Gen") +
				". To complete this task I give you " + FindRussianDaysString(sti(pchar.GenQuest.DestroyPirate.Terms)) + ", your reward in case of success is set at " + FindRussianMoneyString(sti(pchar.GenQuest.DestroyPirate.Money)) + ". So, you're taking this case?";
			link.l1 = "Yes, " + GetAddress_FormToNPC(NPChar) + ", I'll take it.";
		    link.l1.go = "DestroyPirate_agree";
			link.l2 = "No, perhaps not. I don't want to hunt pirates...";
		    link.l2.go = "All_disagree";
		break;
		case "DestroyPirate_agree":
			dialog.text = "Well, fine. Good luck.";
			link.l1 = "Thank you, " + GetAddress_FormToNPC(NPChar) + ".";
		    link.l1.go = "exit";
			pchar.GenQuest.DestroyPirate.MayorId  = npchar.id; //Id мэра, чтобы знать, кто дал квест
			pchar.GenQuest.DestroyPirate.Areal = GetArealByCityName(npchar.city); //ареал, куда будем ставить пирата
			AddDialogExitQuest("DestroyPirate_Begin");
		break;
		// -------------- общий набор для всех квестов мэра ------------------
		case "All_disagree":
			dialog.text = "You disappoint me!";
			link.l1 = "I'm sorry, " + GetAddress_FormToNPC(NPChar) + ", but I can't take on this case.";
		    link.l1.go = "exit";
			ChangeCharacterReputation(pchar, -2);

			if (CheckAttribute(pchar, "GenQuest.questName"))
			{
				QuestName = pchar.GenQuest.questName;
				DeleteAttribute(pchar, "GenQuest." + QuestName);
				DeleteAttribute(pchar, "GenQuest.questName");
			}
		break;
		case "All_Late":
			QuestName = pchar.GenQuest.questName;
			dialog.text = RandPhraseSimple("Well, what can I say is very bad. I was counting on you, but in the end I wasted my time. I'm disappointed.",
				"Well, I must admit, I am extremely disappointed in you... My residence is always full of people who are ready to carry out such assignments, and I was wasting my time on you. Mmm...");
			link.l1 = "Sorry, " + GetAddress_FormToNPC(NPChar) + ", I did all I could.";
		    link.l1.go = "exit";
			//--> слухи
			AddSimpleRumour(LinkRandPhrase("You know, Governor " + GetFullName(npchar) + " very unhappy with the captain " + GetMainCharacterNameDat() + ". Imagine, he did not have time to complete his assignment!",
				"Governor " + GetFullName(npchar) + " he is somewhat annoyed that a certain captain " + GetFullName(pchar) + " I got down to business, but I didn't manage to complete it on time. It's not good...",
				"I heard that the governor " + GetFullName(npchar) + " is very unhappy with you, captain " + GetFullName(pchar) + ", since you did not manage to complete his task on time."), sti(npchar.nation), 5, 1);
			//<-- слухи
			ChangeCharacterReputation(pchar, -4);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			DeleteAttribute(pchar, "GenQuest." + QuestName);
			DeleteAttribute(pchar, "GenQuest.questName");
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_common"; //реплику вертаем
			SaveCurrentNpcQuestDateParam(npchar, "work_date"); //сразу ещё один не даём
			CloseQuestHeader("MayorsQuestsList");
		break;
		case "All_Found":
			QuestName = pchar.GenQuest.questName;
			dialog.text = RandPhraseSimple("I'm just shocked! To be able to find a goal, but not to take sufficient care of creating superiority is, you know, the height of recklessness. I'm disappointed!",
				"Well, I must admit, I'm disappointed in you. It's not enough to find a target, you need to be able to destroy it. I have enough thugs in my residence who are ready to perform this kind of task. Why did I contact you?");
			link.l1 = "Sorry, " + GetAddress_FormToNPC(NPChar) + ", but I don't want to risk my own life so recklessly.";
		    link.l1.go = "exit";
			//--> слухи
			AddSimpleRumour(LinkRandPhrase("You know, Governor " + GetFullName(npchar) + " very unhappy with the captain " + GetMainCharacterNameDat() + ". Imagine, "+ GetSexPhrase("he turned out to be a regular coward","she turned out to be a regular coward") +"!",
				"Governor " + GetFullName(npchar) + " is somewhat annoyed that a certain captain " + GetFullName(pchar) + " took the job, but failed to execute it because of indecision. Hee hee, what captains fell for it!",
				"I heard that the governor " + GetFullName(npchar) + " very unhappy with you, captain " + GetFullName(pchar) + ". They say you're just a coward..."), sti(npchar.nation), 5, 1);
			//<-- слухи
			ChangeCharacterReputation(pchar, -6);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			DeleteAttribute(pchar, "GenQuest." + QuestName);
			DeleteAttribute(pchar, "GenQuest.questName");
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_common"; //реплику вертаем
			SaveCurrentNpcQuestDateParam(npchar, "work_date"); //сразу ещё один не даём
			CloseQuestHeader("MayorsQuestsList");
			AddQuestRecord("MayorsQuestsList", "21");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
		break;
		case "All_Execute":
			QuestName = pchar.GenQuest.questName;
			pchar.(sNationname).quest.mayor = sti(pchar.(sNationname).quest.mayor) + 1;
			dialog.text = RandPhraseSimple("Great news! Well, it's time to take stock. Your fee is " + FindRussianMoneyString(sti(pchar.GenQuest.(QuestName).Money)) + ". Please receive it.",
				"Great! It will be better for everyone... So, I am glad to present you with the required reward - " + FindRussianMoneyString(sti(pchar.GenQuest.(QuestName).Money))  + ". Please receive it.");
			link.l1 = "Thank you, " + GetAddress_FormToNPC(NPChar) + ", " + RandPhraseSimple("nice doing business with you!", "I'm pleasantly surprised by clarity of mutual settlements.");
		    link.l1.go = "All_Execute_1";
			//--> слухи
			AddSimpleRumour(LinkRandPhrase("You know, Governor " + GetFullName(npchar) + "he praises you very much for your performance"+ GetSexPhrase("this captain","this girl") +" " + GetMainCharacterNameGen() + ".",
				"Governor " + GetFullName(npchar) + " praises the captain " + GetMainCharacterNameGen() + ", t"+ GetSexPhrase("from","а") +" clearly fulfills all his instructions. An indispensable person for the governor, I'll tell you...",
				"I heard that the governor " + GetFullName(npchar) + " very pleased with you, Captain " + GetFullName(pchar) + ". He describes you as a neat and efficient officer. It's nice to hear that, Captain..."), sti(npchar.nation), 5, 1);
			//<-- слухи
		break;
		case "All_Execute_1":
			QuestName = pchar.GenQuest.questName;
			if (CheckAttribute(pchar,"Kamikazi") && pchar.Kamikazi == "Meet")
			{
				dialog.text =  "Be glad you survived at all"+GetSexPhrase("","а")+". It's the Caribbean, baby!";
				link.l1 = "Fuck you, asshole!";
				link.l1.go = "exit";
				pchar.Kamikazi = "Done";
				PlayStereoSound("notebook");
			}
			else
			{
				dialog.text = RandPhraseSimple("Well, that's nice. You can come to me again, maybe I'll find a job for you.", "Well, fine! Perhaps I will offer you another job of this kind.");
				link.l1 = RandPhraseSimple("Excellent.", "Excellent.");
				link.l1.go = "exit";
				PlayStereoSound("notebook");
			}

			AddMoneyToCharacter(pchar, sti(pchar.GenQuest.(QuestName).Money));
			CloseQuestHeader("MayorsQuestsList");

			AchievementsCounter_genquests(1);

			ChangeCharacterReputation(pchar, 4);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(PChar, "Leadership", 50);
			OfficersReaction("good");

			DeleteAttribute(pchar, "GenQuest." + QuestName);
			DeleteAttribute(pchar, "GenQuest.questName");
			npchar.greeting = NationShortName(sti(npchar.nation)) + "_gov_common"; //реплику вертаем
			SaveCurrentNpcQuestDateParam(npchar, "work_date"); //сразу ещё один не даём
			// инкремент в базу заданий мэров
			sTemp = npchar.City;
			if (!CheckAttribute(pchar, "GenQuest.MayorQuestsList." + sTemp)) pchar.GenQuest.MayorQuestsList.(sTemp) = 0;
			pchar.GenQuest.MayorQuestsList.(sTemp) = sti(pchar.GenQuest.MayorQuestsList.(sTemp)) + 1;

			if (sTemp == "Santiago")
			{
				if (CheckAttribute(pchar, "PGGWhisperLetter"))
				{
					if (sti(pchar.GenQuest.MayorQuestsList.(sTemp)) > 2)
					{
						AddQuestRecord("PGGWhisper", "3");
					}
					else
					{
						AddQuestRecord("PGGWhisper", "2");
					}
				}
			}

		break;
//<-- осады homo
		case "siege_task":
            makearef(aData, NullCharacter.Siege);
            aData.HelpColony = true;
			dialog.text = "In such a difficult time, any help is valuable to us. We accept your help - help us defend the colony, and you will be rewarded.";
			link.l1 = "So let's not delay.";
		    link.l1.go = "exit";
		break;
		//  boal из ВМЛ -->
		case "GhostShipDone":
            SaveCurrentNpcQuestDateParam(npchar, "GhostShipHelp");

			AddCharacterExpToSkill(pchar, "Leadership", 200);
			AddMoneyToCharacter(pchar, 5000 + rand(10) * 1000);
			ChangeCharacterReputation(pchar, 1);
			ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", -20);
			sTemp = npchar.City;
   			if (!CheckAttribute(pchar, "GenQuest.MayorQuestsList." + sTemp)) pchar.GenQuest.MayorQuestsList.(sTemp) = 0;
			pchar.GenQuest.MayorQuestsList.(sTemp) = sti(pchar.GenQuest.MayorQuestsList.(sTemp)) + 1;

            OfficersReaction("good");
			DeleteAttribute(pchar, "GenQuest.GhostShipWorkId");
			DeleteAttribute(pchar, "GenQuest.GhostShipDone_" + NPChar.id);

			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();

		break;

		case "GhostShipHelp_1":
            SaveCurrentNpcQuestDateParam(npchar, "GhostShipHelp");
			pchar.GenQuest.GhostShipWorkId = NPChar.id;

			bDisableFastReload              = true;

			SetLocationCapturedState(NPChar.Default, true);

			Pchar.quest.GhostShipHelp.win_condition.l1          = "location";
    	    Pchar.quest.GhostShipHelp.win_condition.l1.location = NPChar.from_sea;
    	    Pchar.quest.GhostShipHelp.win_condition             = "GhostShipHelp";

			Pchar.GenQuest.Hunter2Pause            = true;

			ReOpenQuestHeader("GhostShipQuest");
			AddQuestRecord("GhostShipQuest", "help_t1");
			AddQuestUserData("GhostShipQuest", "sCity", GetCityName(npchar.City));

   			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}

string GetGangLocation(ref npchar)
{
    int n;
	string island = GetArealByCityName(npchar.city);
	string LocationId, sAreal;
	int storeArray[MAX_LOCATIONS];
    int howStore = 0;

	for(n=0; n<MAX_LOCATIONS; n++)
	{
		if (GiveArealByLocation(&locations[n]) == island)
		{
			//==> на всякий случай
			if (CheckAttribute(&locations[n], "DisableEncounters") && locations[n].DisableEncounters == true) continue;
			LocationId = locations[n].id;
			if (findsubstr(LocationId, "jungle" , 0) != -1 || findsubstr(LocationId, "Shore" , 0) != -1 || findsubstr(LocationId, "CaveEntrance" , 0) != -1)
			{
				storeArray[howStore] = n;
				howStore++;
			}
		}
	}
	if (howStore == 0) return "none";
	n = storeArray[dRand(howStore-1)];
	return locations[n].id;
}

string GetSpyLocation(ref npchar)
{
    aref	arCommon, arRld, arRld2;
    int	i, n, Qty, Qty2;
	string LocId;
	string islandId, islandId2;
	string	storeArray [50];
	int howStore = 0;
	int locationId = FindLocation(npchar.city + "_town");
    makearef(arRld, Locations[locationId].reload);
	islandId = Locations[locationId].islandId;
	Qty = GetAttributesNum(arRld);
    if (CheckAttribute(pchar, "questTemp.jailCanMove.Deliver.locationId")) //если взят квест по доставке малявы
	{
		for (i=0; i<Qty; i++)
		{
    		arCommon = GetAttributeN(arRld, i);
			LocId = arCommon.go;
    		if (findsubstr(LocId, "Common" , 0) != -1 && LocId != pchar.questTemp.jailCanMove.Deliver.locationId)
    		{
				storeArray[howStore] = LocId;
				howStore++;
			}
			if (arCommon.label != "Sea")
			{
				makearef(arRld2, Locations[FindLocation(LocId)].reload);
				Qty2 = GetAttributesNum(arRld2);
				for (n=0; n<Qty2; n++)
				{
    				arCommon = GetAttributeN(arRld2, n);
					LocId = arCommon.go;
					if (findsubstr(LocId, "Common" , 0) != -1 && LocId != pchar.questTemp.jailCanMove.Deliver.locationId && LocId != "CommonPackhouse_1" && LocId != "CommonPackhouse_2")
    				{
						storeArray[howStore] = LocId;
						howStore++;
					}
				}
			}
		}
	}
	else
	{
		for (i=0; i<Qty; i++)
		{
    		arCommon = GetAttributeN(arRld, i);
			LocId = arCommon.go;
    		if (findsubstr(LocId, "Common" , 0) != -1)
    		{
				storeArray[howStore] = LocId;
				howStore++;
			}
			if (arCommon.label != "Sea")
			{
				int locationId2 = FindLocation(LocId);
				if (!CheckAttribute(Locations[locationId2],"reload") || !CheckAttribute(Locations[locationId2],"islandId")) continue;
				makearef(arRld2, Locations[locationId2].reload);
				islandId2 = Locations[locationId2].islandId;

				if (islandId == islandId2) {
					Qty2 = GetAttributesNum(arRld2);
					for (n = 0; n < Qty2; n++)
					{
						arCommon = GetAttributeN(arRld2, n);
						LocId = arCommon.go;
						if (findsubstr(LocId, "Common", 0) != -1 && LocId != "CommonPackhouse_1" && LocId != "CommonPackhouse_2")
						{
							storeArray[howStore] = LocId;
							howStore++;
						}
					}
				}
			}
		}
	}
	if (howStore == 0) return "none";
	LocId = storeArray[dRand(howStore-1)];
	SetOpenDoorCommonLoc(npchar.city, LocId); //открываем дверь
	for(n=0; n<MAX_CHARACTERS; n++)
	{
		if (CheckAttribute(&characters[n], "locations") && characters[n].locations == LocId)
		{
			characters[n].lifeDay = 0;
		}
	}
	return LocId;
}

string GetSpyColony(ref NPChar)
{
    int storeArray[MAX_COLONIES];
    int howStore = 0;
	string sChrId;

	for(int n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].nation != "none" && sti(colonies[n].nation) != PIRATE && GetNationRelation(sti(colonies[n].nation), sti(NPChar.nation)) == RELATION_ENEMY && colonies[n].id != "Panama" && colonies[n].id != npchar.City)
		{
			storeArray[howStore] = n;
			howStore++;
		}
	}
	if (howStore == 0) return "none";
	return colonies[storeArray[Rand(howStore-1)]].id;
}
string SelectPassText()
{
	string sText;
	switch (drand(5))
	{
		case 0: sText = "This Judas, abusing his high station, has sold vital state secrets to a hostile power for a handful of silver."; break;
		case 1: sText = "This villain foully murdered his own kin, a man of noble standing and renown, all for the sake of an inheritance."; break;
		case 2: sText = "This vile scoundrel has cast a grievous insult upon my family and my person."; break;
		case 3: sText = "This thieving rogue made off with a substantial sum of gold from our colonial treasury."; break;
		case 4: sText = "This murderer is responsible for the deaths of nearly a dozen of our good citizens."; break;
		case 5: sText = "This swindler, using his connections, deceitfully seized a merchant vessel, made off with it, and sold it at a neighboring shipyard."; break;
	}
	return sText;
}

string SelectFugitiveText()
{
	string sText;
	switch (drand(5))
	{
		case 0: sText = "a junior officer of our garrison has abandoned his post and fled our colony aboard a merchantman."; break;
		case 1: sText = "one of our high-ranking officers deserted while on active duty and escaped on a smuggler's vessel."; break;
		case 2: sText = "one of our naval officers, stationed on a patrol brig, deserted for reasons unknown, stole a longboat, and vanished."; break;
		case 3: sText = "a prison guard has inexplicably abandoned his service and fled our colony on a private ship."; break;
		case 4: sText = "a soldier of our garrison deserted his post while on watch, made for the bay, and left these shores on an unidentified vessel."; break;
		case 5: sText = "a military officer, captain of a patrol lugger, abandoned his ship at sea and fled in an unknown direction aboard a fishing boat."; break;
	}
	return sText;
}
