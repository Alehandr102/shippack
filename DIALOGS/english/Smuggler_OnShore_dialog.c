
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, Diag;
	string NPC_Meeting;
	int Shit, i;
	ref refStore;
	ref sld;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
		break;

		case "First time":
			Diag.TempNode = "first time";
			// благодарность за бой с патрулём уже получена - все смуглеры сворачивают разговор (без повторных наград и торговли)
			if (CheckAttribute(pchar, "quest.Contraband.ThanksGot"))
			{
				dialog.text = RandPhraseSimple("That's it, captain, time for us to go. Farewell.", "The job is done, we part ways. No hard feelings.");
				link.l1 = "Farewell.";
				link.l1.go = "exit";
				break;
			}
			// === contraband FREIGHT handover (order from the trader journal) ===
			if (CheckAttribute(pchar, "quest.SmuggleFreight.active") && sti(pchar.quest.SmuggleFreight.active) == true)
			{
				// груз принимают только с корабля, пришвартованного в ЭТОЙ бухте - не пешком из города
				// (та же проверка, что у обычной сделки; фикс сдачи "через порт")
				bool bShipInBay = false;
				if (CheckAttribute(pchar, "location.from_sea"))
				{
					if (pchar.location == pchar.location.from_sea)
					{
						if (sti(pchar.Ship.Type) != SHIP_NOTUSED) bShipInBay = true;
					}
				}
				if (!bShipInBay)
				{
					dialog.text = "Oh no. And where's your ship? Planning to haul the goods on your own back? Moor in this bay - we'll transfer everything quietly, board to board.";
					link.l1 = "All right, I'll come alongside.";
					link.l1.go = "exit";
					break;
				}
				// хитрецы: взяли заказ на малой эскадре, а в бухту привели большую/мощную - сделки не будет
				if (!CheckSmuggleFreightShips())
				{
					dialog.text = "What do you think you're doing?! The deal was to come quietly: no more than three sail, and no heavy tubs above the third class. Take your armada away before the patrol swarms in - or there's no deal.";
					link.l1 = "All right, I'll send the extra ships away and come back.";
					link.l1.go = "exit";
					break;
				}
				int fchkGoods = makeint(pchar.CargoQuest.iTradeGoods);
				int fchkQty   = makeint(pchar.CargoQuest.iQuantityGoods);
				if (GetSquadronGoods(pchar, fchkGoods) < fchkQty)
				{
					dialog.text = "Where's the cargo? It's not all here. No full cargo, no deal.";
					link.l1 = "I'll deliver it now.";
					link.l1.go = "exit";
					break;
				}
				// КИДАЛОВО (флаг отролен при взятии заказа, шанс как в обычной сделке): платы не будет - бой или груз даром
				if (CheckAttribute(pchar, "CargoQuest.SmuggleScam"))
				{
					dialog.text = RandPhraseSimple("Well, well, the cargo's all here... You know what? I think we'll take it for free. Who are you going to complain to - the governor, with goods like these?",
						"Hm, brought it all, good work... Now listen here: there'll be no payment. Be thankful we're letting you walk away alive. So, what will it be?");
					link.l1 = "The only way you'll get this cargo is with my blade thrown in!";
					link.l1.go = "SmuggleFreight_ScamFight";
					link.l2 = "(weighing the odds) Fine, take it. You win this time.";
					link.l2.go = "SmuggleFreight_ScamGive";
					break;
				}
				dialog.text = "(under his breath) Right, the cargo's here. We'll take it. Here's your money - now clear off, it may get hot around here.";
				link.l1 = "Already leaving.";
				link.l1.go = "SmuggleFreight_BayDone";
				break;
			}
			if(CheckAttribute(PChar, "quest.Contraband.active"))
			{
				if ((sti(pchar.Ship.Type) != SHIP_NOTUSED) && !CheckCharacterPerk(pchar, "UnlimitedContra"))
				{
					if (GetCompanionQuantity(pchar) > 1 && GetBaseHeroNation() != PIRATE)
					{
						dialog.text = NPCStringReactionRepeat("Listen, they clearly told you at the tavern to come with one ship. Get out and get rid of your squadron.",
							"You should get out of here. Otherwise we'll hand you over to the patrol ourselves.",
							"Come on, get in the boat and get out.",
							"You've tired me out...", "block", 1, npchar, Dialog.CurrentNode);
						link.l1 = HeroStringReactionRepeat("Okay, okay, I'll be right back on the flagship.",
							"Don't swear, I'll just run to the port authority, hand over the extra ships, and immediately go back.",
							"Eh-h, it was not possible to cheat...",
							"Yes, I'm persistent!", npchar, Dialog.CurrentNode);
						link.l1.go = DialogGoNodeRepeat("exit", "", "", "", npchar, Dialog.CurrentNode);
						break;
					}
					//редкостная хрень, но по-другому не работает-класс корабля ГГ считается отдельно от компаньонов, и всё тут
					int iClass, ipClass;
					ipClass = 4-sti(RealShips[sti(pchar.ship.type)].Class);
					iClass = 3;//т.к. не пройдёт по числу кораблей в любом случае
					if (GetBaseHeroNation() == PIRATE)
					{
						ipClass = sti(ipClass)-1;
						int iChIdx;
						// поиск старшего класса компаньонов
						for (i=0; i<COMPANION_MAX; i++)
						{
							iChIdx = GetCompanionIndex(GetMainCharacter(), i);
							if (iChIdx>=0)
							{
								sld = GetCharacter(iChIdx);
								iClass = GetCharacterShipClass(sld);
							}
						}
					}
					if (sti(ipClass) > 0 || 3 - sti(iClass) > 0)
					{
						dialog.text = NPCStringReactionRepeat("Didn't they tell you not to show up on such a conspicuous vessel? You should have brought a couple more mans with you. Get out and come on a smaller ship.",
							"You should get out of here. Otherwise we'll hand you over to the patrol ourselves.",
							"Come on, get in the boat and get out.",
							"You've tired me out...", "block", 1, npchar, Dialog.CurrentNode);
						link.l1 = HeroStringReactionRepeat("Okay, okay, I'll change the boat.",
							"Don't get mad, I'll be right there-one foot here, the other there.",
							"Eh-h, it was not possible to cheat...",
							"Yes, I'm persistent!", npchar, Dialog.CurrentNode);
						link.l1.go = DialogGoNodeRepeat("exit", "", "", "", npchar, Dialog.CurrentNode);
						break;
					}
				}
				Dialog.snd = "voice\SMSH\SMSH001";
				dialog.Text = RandPhraseSimple("Hey, what are you doing here, "+ GetSexPhrase("buddy", "girlfriend") +"?",
                                          RandSwear() + "What do you want here?!");
				Link.l1 = RandPhraseSimple("Relax, man, I brought goods!", "What are you looking at? I brought goods!");
				Pchar.quest.Contraband.Counter = 0; // не торговали
				if ((Pchar.Location == Pchar.location.from_sea) && (sti(pchar.Ship.Type) != SHIP_NOTUSED))
				{
					Link.l1.go = "Exchange";
				}
				else
				{
					Link.l1.go = "No_Ship";
				}
				//по заданию губернатора, истребление контры, только эта ветка.
				if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers == "" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(pchar.location)]))
				{
					Link.l2 = "I'm here by order of the governor of " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.KillSmugglers.MayorId)].city+"Gen") + "! I order you to lay down your weapons and follow me-you are under arrest!";
					Link.l2.go = "GenQuestKillContraband_1";
				}
			}
			else
			{
				if (CheckAttribute(PChar, "GenQuest.contraTravel.payed") && sti(PChar.GenQuest.contraTravel.payed) == true) //если заплатил, то разговаривают, иначе посылают
				{
					dialog.Text = RandPhraseSimple("Yeah, finally!!! Here you are, otherwise we've been waiting for quite a while. Come on, we're leaving.", "Move it, there are patrols everywhere. We need to get out of here faster!");
					//по заданию губернатора, истребление контры, только эта ветка.
					if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers == "" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(pchar.location)]))
					{
						Link.l1 = "I'm here by order of the governor of " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.KillSmugglers.MayorId)].city+"Gen") + "! I order you to lay down your weapons and follow me-you are under arrest!";
						Link.l1.go = "GenQuestKillContraband_1";
						break;
					}
					//если набрал пассажиров, в сад..
					if (GetPassengersQuantity(PChar) > 0)
					{
						dialog.Text = "And who are you? We agreed to deliver one passenger, not a crowd.";
						Link.l1 = "Oops...";
						Link.l1.go = "Exit";
						break;
					}
					//если набрал компаньонов или купил корабль себе уже :), в сад..
					if (GetCompanionQuantity(PChar) > 1 || sti(PChar.ship.type) != SHIP_NOTUSED)
					{
						dialog.Text = RandPhraseSimple("Who are you? We agreed to deliver the passenger, not captain.", "Get out! Don't come until you get rid of your ship.");
						Link.l1 = "Oh, damn...";
						Link.l1.go = "Exit";
						break;
					}
					//типа помог отбиться, свой парень...
					if (CheckAttribute(PChar, "GenQuest.contraTravel.PatrolFight") && sti(PChar.GenQuest.contraTravel.PatrolFight) == true)
					{
						if (chrDisableReloadToLocation) // ещё бой идёт
						{
							dialog.Text = "Patrol! We don't know you, you don't know us.";
							Link.l1 = "Yeah...";
							Link.l1.go = "Exit";
							break;
						}
						if (CheckAttribute(PChar, "GenQuest.contraTravel.PatrolBribed")) // патруль спроважен миром (откуп/на испуг), боя не было
						{
							dialog.Text = RandPhraseSimple("Nicely done - you got rid of those soldiers without spilling a drop of blood. Respect. Let's go.", "You sure know how to handle a patrol. We're off.");
							DeleteAttribute(PChar, "GenQuest.contraTravel.PatrolBribed");
						}
						else
						{
							dialog.Text = RandPhraseSimple("Thanks for the help. We will not forget this. Let's go.", "Well done! You fought well.");
						}
						ChangeContrabandRelation(PChar, 5);
						PChar.GenQuest.contraTravel.PatrolFight = false;
						AddCharacterExpToSkill(Pchar, "Sneak", 50);
					}
					//тут все ок, отправляемся.
					Link.l2 = "I'm on my way.";
					Link.l2.go = "Exit";
					
					//поместим Грея в локацию.
					refStore = CharacterFromID("Abracham_Gray");
					ChangeCharacterAddressGroup(refStore, "Ship_deck", "goto", "goto2");
					refStore.dialog.Filename = "Smuggler_Ship_dialog.c";
					refStore.dialog.CurrentNode = "Travel_talkStart";
					//поплыл, иначе RemoveTravelSmugglers грохнет всю ветку
					PChar.GenQuest.contraTravel.ship = true;
					PChar.quest.Munity = "";  // признак выхода с палубы

					SetLaunchFrameFormParam("It's gone" + sti(Pchar.GenQuest.contraTravel.destination.days) + " days " + NewStr() + "The deck of the smugglers' ship.",
																	"Reload_To_Location", 0.1, 5.0);
											bQuestCheckProcessFreeze = true;
					WaitDate("", 0, 0, sti(Pchar.GenQuest.contraTravel.destination.days), rand(20), 0);
					bQuestCheckProcessFreeze = false;
					MakeCloneShipDeck(refStore, true); // подмена палубы
					SetLaunchFrameReloadLocationParam("Ship_deck", "reload", "reload1", "Travel_talkOnDeck");
					AddDialogExitQuest("LaunchFrameForm");
					AddCharacterExpToSkill(Pchar, "Sneak", 50);
				}
				else
				{
					if (CheckAttribute(NPChar, "ContrabandInterruption")) // если таможня уже бежит
					{
						if (CheckAttribute(PChar, "GenQuest.contraTravel.PatrolFight"))
						{
							if (chrDisableReloadToLocation)
							{
								dialog.Text = RandSwear()+ "Patrol! We don't know you, you don't know us!";
								Link.l1 = "Exactly!";
								Link.l1.go = "Exit";
								break;
							}
							if (CheckAttribute(PChar, "GenQuest.contraTravel.PatrolBribed")) // патруль спроважен миром (откуп/на испуг), боя не было
							{
								dialog.Text = RandPhraseSimple("Nicely done - you got rid of those soldiers without spilling a drop of blood. Respect.", "You sure know how to handle a patrol. A pleasure working with captains like you.");
								DeleteAttribute(PChar, "GenQuest.contraTravel.PatrolBribed");
							}
							else
							{
								dialog.Text = RandPhraseSimple("Thanks for the help. We will not forget this. Let's go.", "Well done! You fought well.");
							}
							ChangeContrabandRelation(PChar, 5);
							DeleteAttribute(PChar, "GenQuest.contraTravel.PatrolFight");
							ContrabandInterruptionSetStatus(false);
							AddCharacterExpToSkill(Pchar, "Sneak", 50);
							pchar.quest.Contraband.ThanksGot = true; // благодарность ОДИН раз: дальше все смуглеры сворачивают разговор (чистится при новой высадке)
							Link.l2 = "I'm on my way.";
							Link.l2.go = "Exit";
							break;
						}
					}
					else
					{
						// один диалог - одна сделка: повторный запуск торговли (Exchange1) УБРАН - закрывает дыру с многократным спавном патруля и фармом наград
						dialog.Text = "Listen, get out of here! The deal is done - no use hanging around before a patrol shows up.";
						Link.l2 = RandPhraseSimple("Do you dare threaten me, dog?", "You'll choke on your words, you rascal!");
						Link.l2.go = "Exit_fight";
						Link.l3 = "Come on, I know what business is. See you later.";
						Link.l3.go = "Exit";
					}
				}
			}
		break;

		case "No_Ship":
			Dialog.snd = "voice\SMSH\SMSH003";
			dialog.Text = "On your back, or what? Where is your ship?";
			Link.l1 = RandPhraseSimple("At first I wanted to make sure there's no catch here.", "I didn't want to risk the ship. I prefer to look at everything with my own eyes first.");
			Link.l1.go = "No_ship_1";
			Link.l2 = "The deal is off!";
			Link.l2.go = "Cancellation";
		break;

		case "No_ship_1":
			Dialog.snd = "voice\SMSH\SMSH004";
			dialog.Text = "We are honest people, and we do not live by deception. Bring your ship, but be careful - so that the coastal patrol does not follow you.";
			Link.l1 = "Good. Wait for me here.";
			Link.l1.go = "Exit";
		break;

		case "Exit_fight":
			DeleteAttribute(Pchar, "quest.Contraband.active");
			DeleteAttribute(Pchar, "quest.Contraband.counter");
			DeleteAttribute(Pchar, "quest.Contraband.price1");
			DeleteAttribute(Pchar, "quest.Contraband.price2");
			DeleteAttribute(Pchar, "quest.Contraband.price3");
			DeleteAttribute(Pchar, "quest.Contraband.price4");
			DeleteAttribute(Pchar, "quest.Contraband.price5");
			DeleteAttribute(Pchar, "quest.Contraband.price6");
			DeleteAttribute(Pchar, "quest.Contraband.price7");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX1");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX2");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX3");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX4");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX5");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX6");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX7");
			DeleteAttribute(Pchar, "quest.Contraband.sum");
			DeleteAttribute(Pchar, "quest.Contraband.ChooseGoodsQty");
			DeleteAttribute(Pchar, "quest.Contraband.ChooseGoodsID");

			AddSimpleRumourCity("Have you heard? Recently, a patrol raided smugglers in the bay. And what would you think? The captain, who was shipping goods to local buyers, managed to get away! " +
				"Dashing guys turned out to be on his team - the whole patrol squad was put down. And our locals have disappeared into the water. So nothing but corpses was found in the bay - neither the goods, nor the hiding place, nor the smugglers themselves. That's how these crooks have their fishing organized!", Pchar.quest.contraband.City, 3, 5, "");

			CloseQuestHeader("Gen_Contraband");
			Pchar.quest.Rand_Smuggling.over = "yes";
			LAi_group_FightGroups(LAI_GROUP_PLAYER, pchar.GenQuest.Smugglers_Group, true);
			RemoveSmugglersFromShore();
			//#20190121-01

			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
			//#20190121-01
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "Exit_Cancel":
			DeleteAttribute(Pchar, "quest.Contraband");
			CloseQuestHeader("Gen_Contraband");
			Pchar.quest.Rand_Smuggling.over = "yes";
			RemoveSmugglersFromShore();
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
		break;

		case "Cancellation":
			if (sti(Pchar.quest.Contraband.Counter) == 0)
			{
				dialog.Text = "Cancelled? Are you kidding?";
				Link.l1 = "Absolutely serious.";
				Link.l1.go = "Cancellation_1";
			}
			else
			{
				dialog.Text = "Well, that's fine. We've traded well enough as it is.";
				Link.l1 = "Exactly!";
				Link.l1.go = "Finish_exit";
			}
		break;

		case "Cancellation_1":
			if( makeint(Pchar.rank) <= 3 || GetSummonSkillFromNameToOld(Pchar, SKILL_FENCING) <= 5 || GetSummonSkillFromNameToOld(Pchar, SKILL_LEADERSHIP) <= 5 )
			{
				if(Rand(1) == 1)
				{
					ChangeContrabandRelation(pchar, -30);
					Dialog.snd = "voice\SMSH\SMSH006";
					dialog.Text = "Dog! You can't fool us like that! You will pay for this with your head!";
					Link.l1 = "But that's unlikely!";
					Link.l1.go = "Exit_fight";
				}
				else
				{
					ChangeContrabandRelation(pchar, -10);
					Dialog.snd = "voice\SMSH\SMSH007";
					dialog.Text = "You can't get away with this!";
					Link.l1 = "Go bark somewhere else, dogs!";
					Link.l1.go = "Exit_cancel";
				}
			}
			else
			{
				ChangeContrabandRelation(pchar, -10);
				Dialog.snd = "voice\SMSH\SMSH008";
				dialog.Text = "You're going to regret this!";
				Link.l1 = "Well, well!";
				Link.l1.go = "Exit_cancel";
			}
		break;

		case "GenQuestKillContraband_1":
			//счётчик подстав по "metro"...
			if(CheckAttribute(PChar, "GenQuest.contraTravel.active") && sti(PChar.GenQuest.contraTravel.active) == true)
			{
				Statistic_AddValue(PChar, "contr_TravelKill", 1);
				ChangeContrabandRelation(pchar, -20); //репу контры вниз
			}
			dialog.Text = "You're going to answer to us now for such a setup!";
			Link.l1 = "Then you will all die. I have a clear order - in case of resistance, do not take the living!";
			Link.l1.go = "Exit_fight";
		break;

		case "SmuggleFreight_BayDone":
			int fGoods = makeint(pchar.CargoQuest.iTradeGoods);
			int fQty   = makeint(pchar.CargoQuest.iQuantityGoods);
			int fReward = makeint(pchar.CargoQuest.iMoney);
			bool late = CheckQuestAttribute("generate_trade_quest_progress", "failed");
			bool bEarlyBonus = false;
			int iLateFine = 0;
			int iBonus = 0;
			if (late)
			{
				iLateFine = makeint(fReward / 2);
				fReward = fReward - iLateFine; // overdue - half price
			}
			else if (GetQuestPastDayParam("CargoQuest") * 2 < sti(pchar.CargoQuest.iDaysExpired))
			{
				iBonus = makeint(fReward * 0.2);
				fReward = fReward + iBonus; // early - bonus
				bEarlyBonus = true;
			}

			if (late)
			{
				Log_Info(XI_ConvertString("Freight_LateFine"));
				Log_InfoColored("red", "-" + FindRussianMoneyStringLog(iLateFine));
			}
			else if (bEarlyBonus)
			{
				Log_Info(XI_ConvertString("Freight_EarlyBonusCargo"));
				Log_InfoColored("green", "+" + FindRussianMoneyStringLog(iBonus));
			}
			Log_Info(XI_ConvertString("Freight_TotalSmuggler"));
			AddMoneyToCharacter(pchar, fReward);
			Freight_AddLoyalty(); // LEO: +1% к будущей премии выдавшего работодателя за выполненный контрабандный фрахт
			ChangeContrabandRelation(pchar, 3); // +contraband reputation
			if (!CheckAttribute(pchar, "contrabandmoneyback")) pchar.contrabandmoneyback = fReward; // counts toward the "Hardened Smuggler" perk
			else pchar.contrabandmoneyback = sti(pchar.contrabandmoneyback) + fReward;
			// опыт навыков как у обычного фрахта (Sailing=навигация, Leadership, Commerce); при просрочке - без бонуса за дни
			int fDaysMul = GetQuestPastDayParam("CargoQuest") + 1;
			if (late) fDaysMul = 1;
			AddCharacterExpToSkill(pchar, "Sailing", fDaysMul * sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_I));
			AddCharacterExpToSkill(pchar, "Leadership", fDaysMul * sti(pchar.rank) * GetCharacterSPECIALSimple(pchar, SPECIAL_C));
			AddCharacterExpToSkill(pchar, "Commerce", fDaysMul * sti(pchar.rank) * (GetCharacterSPECIALSimple(pchar, SPECIAL_C) + GetCharacterSPECIALSimple(pchar, SPECIAL_I)));
			RemoveCharacterGoods(pchar, fGoods, fQty);
			pchar.quest.generate_trade_quest_progress = "";
			pchar.quest.generate_trade_quest.over = "yes";
			AddQuestRecord("DELIVERY_TRADE_QUEST", "smug3");
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(fGoods));
			CloseQuestHeader("DELIVERY_TRADE_QUEST");
			AchievementsCounter_genquests(1);
			RemoveSmugglersFromShore(); // лагерь с грузами сворачивается, как в обычной сделке (гард по quest.SmuggleFreight не даст закрыть "Сбыт контрабанды"; NPC стоят до выхода - благодарность за патруль работает)
			DeleteAttribute(pchar, "quest.SmuggleFreight");
			pchar.contratradedmoney = fReward; // patrol bribe is 10%+difficulty of this sum (same as a normal deal)
			// patrols just like a normal deal: land (on shore) + sea (patrol OR pirates)
			if (GetSummonSkillFromName(pchar, "Sneak") < Rand(120))
			{
				DoQuestCheckDelay("Rand_ContrabandInterruption", 0.1);
				ContrabandInterruptionSetStatus(true);
				Pchar.GenQuest.contraTravel.PatrolFight = true; // как в обычной сделке: за помощь в бою с патрулём смуглеры поблагодарят (один раз)
				DeleteAttribute(PChar, "GenQuest.contraTravel.PatrolBribed"); // новый патруль - мирный исход прошлого не в счёт
			}
			else
			{
				SmugRevenge_ArmDeathWatch(); // сухопутного патруля нет: вырежешь смуглеров - вместо нацпатруля придёт возмездие братства
			}
			if (GetSummonSkillFromNameToOld(pchar, SKILL_SNEAK) < dRand(12))
			{
				ArmContraFreightPursuit();
				SetTimerCondition("Rand_ContrabandInterruptionAtSeaEnded", 0, 0, 2, false);
				Pchar.quest.Rand_ContrabandAtSeaEnded.win_condition.l1 = "MapEnter";
				Pchar.quest.Rand_ContrabandAtSeaEnded.win_condition = "Rand_ContrabandAtSeaEnded";
			}
			DeleteAttribute(pchar, "quest.Contraband.active");
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
		break;

		case "SmuggleFreight_ScamFight":
			// кидалово, вариант "перебить всех": груз остаётся нам, заказ закрывается записью в журнале. Патрули не армим (как в Exit_fight обычной сделки)
			pchar.quest.generate_trade_quest_progress = "";
			pchar.quest.generate_trade_quest.over = "yes";
			// LEO: outcome record is NOT written here - the last scammer ALWAYS surrenders and pays (SmugScam_Surrender_Done),
			// the journal is filled BY FACT of surrender (smug6). smug5 was written prematurely on choosing "fight" and contradicted the outcome.
			CloseQuestHeader("DELIVERY_TRADE_QUEST");
			DeleteAttribute(pchar, "CargoQuest.SmuggleScam");
			DeleteAttribute(pchar, "quest.Contraband.active");
			AddSimpleRumourCity("Have you heard? They found the whole gang of contraband fences at the bay - cut down to a man. They say the fools tried to cheat the wrong captain: take the cargo and pay nothing. Well, they paid after all - in blood.", Pchar.quest.contraband.City, 3, 5, "");
			LAi_group_FightGroups(LAI_GROUP_PLAYER, pchar.GenQuest.Smugglers_Group, true);
			SmuggleFreightScamHelp(); // 50% (общий дневной dRand): подмога смуглеров - сильные бойцы (как скелеты "Упокоения умерших")
			RemoveSmugglersFromShore();
			DeleteAttribute(pchar, "quest.SmuggleFreight"); // ПОСЛЕ RemoveSmugglersFromShore - флаг гасит лишнее закрытие "Сбыта контрабанды" внутри неё
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "SmuggleFreight_ScamGive":
			// кидалово, вариант "отдать груз": без оплаты, зато уходим ТИХО - ни сухопутного, ни морского патруля; +150 опыта в Везение и Скрытность
			dialog.text = "A wise choice. Unload it - and clear off. You never saw us, we don't know you.";
			link.l1 = "I'll remember you, lads... We'll meet again.";
			link.l1.go = "exit";
			AddSimpleRumourCity("They say the fences at the bay cheated a visiting captain - took the cargo and refused to pay. I don't envy them: such grudges are not forgiven on these shores.", Pchar.quest.contraband.City, 3, 5, "");
			RemoveCharacterGoods(pchar, makeint(pchar.CargoQuest.iTradeGoods), makeint(pchar.CargoQuest.iQuantityGoods));
			AddCharacterExpToSkill(pchar, "Fortune", 150);
			AddCharacterExpToSkill(pchar, "Sneak", 150);
			pchar.quest.generate_trade_quest_progress = "";
			pchar.quest.generate_trade_quest.over = "yes";
			AddQuestRecord("DELIVERY_TRADE_QUEST", "smug4");
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(makeint(pchar.CargoQuest.iTradeGoods)));
			CloseQuestHeader("DELIVERY_TRADE_QUEST");
			RemoveSmugglersFromShore(); // лагерь с грузами сворачивается (гард по quest.SmuggleFreight не даст закрыть "Сбыт контрабанды")
			DeleteAttribute(pchar, "quest.SmuggleFreight");
			DeleteAttribute(pchar, "CargoQuest.SmuggleScam");
			DeleteAttribute(pchar, "quest.Contraband.active");
			// если после кидалова нападём на шайку - они тоже призовут подмогу (детект по первому удару)
			DeleteAttribute(pchar, "quest.SmugScamGiveCalled");
			int ssgI;
			ref ssgM;
			for (ssgI = 1; ssgI <= 3 + makeint(MOD_SKILL_ENEMY_RATE * 3 / 2); ssgI++)
			{
				if (GetCharacterIndex("Rand_Smug0" + ssgI) == -1) continue;
				ssgM = CharacterFromID("Rand_Smug0" + ssgI);
				if (LAi_isDead(ssgM)) continue;
				LAi_SetCheckMinHP(ssgM, LAi_GetCharacterHP(ssgM) - 1, false, "SmugScamGive_Attacked");
			}
		break;

		case "SmugScam_Surrender": // последний выживший из шайки кидал: бой остановлен, сдаётся и расплачивается за груз
			LAi_SetStayTypeNoGroup(NPChar); // застывает на время разговора (как барыга при торговле), иначе бродит
			NPChar.actions.walk = "walk";   // подбегал бегом (walk=run) - возвращаем шаг
			LAi_SetFightMode(pchar, false); // страховка: все сабли в ножны
			LAi_SetFightModeForOfficers(false);
			dialog.text = "Stop, stop!.. Enough! You win, devil take you... Here, take it - full payment for the cargo, as agreed. Take it and spare me.";
			link.l1 = "That's more like it. And remember: tricks like that end badly with me.";
			link.l1.go = "SmugScam_Surrender_Done";
		break;

		case "SmugScam_Surrender_Done":
			dialog.text = "A deal is worth more than a life, I see that now... We're leaving, lads... ah, right... there's no one left to leave...";
			link.l1 = "";
			link.l1.go = "exit";
			AddSimpleRumourCity("Heard about the slaughter at the bay? The fences thought to cheat a captain out of his cargo, but picked the wrong one. Only one survived - and that one, they say, paid the full price on his knees, down to the last piastre.", Pchar.quest.contraband.City, 3, 5, "");
			// расплата за груз: если груз ещё в трюме (исход "перебить всех") - передаём его, деньги в любом случае
			int ssGoods = makeint(pchar.CargoQuest.iTradeGoods);
			int ssQty   = makeint(pchar.CargoQuest.iQuantityGoods);
			bool ssHadCargo = (GetSquadronGoods(pchar, ssGoods) >= ssQty); // LEO: cargo still aboard = came from "kill them all"; gone = from "give cargo away"
			if (ssHadCargo) RemoveCharacterGoods(pchar, ssGoods, ssQty);

			Log_Info(XI_ConvertString("Freight_TotalBlade"));
			Log_InfoColored("yellow", XI_ConvertString("Freight_PayBlood") + FindRussianMoneyString(makeint(pchar.CargoQuest.iMoney)));
			AddMoneyToCharacter(pchar, makeint(pchar.CargoQuest.iMoney));
			// LEO: journal BY FACT - deal ended in peace (cargo handed over, payment received), record deferred here from ScamFight.
			// Only for the "kill them all" outcome (cargo was aboard): the "give cargo away" branch already wrote its own smug4 record.
			if (ssHadCargo)
			{
				AddQuestRecord("DELIVERY_TRADE_QUEST", "smug6");
				AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(ssGoods));
				CloseQuestHeader("DELIVERY_TRADE_QUEST");
			}
			// финализация НА ВЫХОДЕ из диалога (в ноде нельзя - уйдёт бродить, не договорив): бессмертный немой истукан
			AddDialogExitQuestFunction("SmugScamSurvivorRetire");
		break;

		case "SmugFreightPre_Surrender": // ГГ напал до передачи груза: последний выживший просит пощады и предлагает завершить сделку
			Diag.TempNode = "SmugFreightPre_Surrender";
			LAi_SetStayTypeNoGroup(NPChar); // застывает на время разговора (как барыга при торговле), иначе бродит
			NPChar.actions.walk = "walk";   // подбегал бегом (walk=run) - возвращаем шаг
			LAi_SetFightMode(pchar, false); // страховка: все сабли в ножны
			LAi_SetFightModeForOfficers(false);
			dialog.text = RandPhraseSimple(
				RandPhraseSimple(
					"Mercy!.. Enough blood. Let's just finish the deal: the cargo to us, the money to you, as agreed. Kill me, and the brotherhood will learn who cut down their own - and a knife will be waiting for you in every port.",
					"Stop, hold on! Let's keep it business: finish the deal as agreed - and we part ways. Otherwise OUR people will hear of this slaughter, and trading with the coast will get mighty hard for you."),
				"Enough, you win!.. But think it through: break the deal, and the Coastal Brotherhood won't forget the grudge. Let's finish the exchange fair and square - and we're even.");
			link.l1 = "You win. Let's finish the deal - and part ways.";
			link.l1.go = "SmugFreightPre_Spare";
			link.l2 = "No. Go join your friends.";
			link.l2.go = "SmugFreightPre_Kill";
		break;

		case "SmugFreightPre_Spare": // пощада: сделка завершается под клинком, бонусы за милосердие
			dialog.text = "There's a good captain... The cargo ashore - and we go our separate ways. The brotherhood doesn't need to know what happened here. I saw nothing.";
			link.l1 = "Farewell. And do your best to forget my face.";
			link.l1.go = "exit";
			AddSimpleRumourCity("Word is, it came to blades among the smugglers at the bay - nearly the whole gang cut down. And the strangest part: the last survivor and the visiting captain shook hands, and the deal went through after all. What people - first they fight, then they trade!", Pchar.quest.contraband.City, 3, 5, "");
			// завершение сделки: груз - смуглерам, деньги - нам
			int spGoods = makeint(pchar.CargoQuest.iTradeGoods);
			int spQty   = makeint(pchar.CargoQuest.iQuantityGoods);
			if (GetSquadronGoods(pchar, spGoods) >= spQty) RemoveCharacterGoods(pchar, spGoods, spQty);

			Log_InfoColored("yellow", XI_ConvertString("Freight_PayBlade") + FindRussianMoneyString(makeint(pchar.CargoQuest.iMoney)));
			AddMoneyToCharacter(pchar, makeint(pchar.CargoQuest.iMoney));
			ChangeContrabandRelation(pchar, 5);               // контрабандисты ценят, что одумался
			ChangeCharacterReputation(pchar, 10);             // +репутация ГГ
			AddCharacterExpToSkill(pchar, "Leadership", 200); // +авторитет
			pchar.quest.generate_trade_quest_progress = "";
			pchar.quest.generate_trade_quest.over = "yes";
			AddQuestRecord("DELIVERY_TRADE_QUEST", "smug6");
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(spGoods));
			CloseQuestHeader("DELIVERY_TRADE_QUEST");
			RemoveSmugglersFromShore(); // лагерь с грузами сворачивается (гард по quest.SmuggleFreight не даст закрыть "Сбыт контрабанды")
			DeleteAttribute(pchar, "quest.SmuggleFreight");
			// сделка СОСТОЯЛАСЬ (пусть и под клинком) - патрули как при обычной передаче: сухопутный + морской
			pchar.contratradedmoney = makeint(pchar.CargoQuest.iMoney); // откуп патрулю - 10%+сложность от этой суммы
			if (GetSummonSkillFromName(pchar, "Sneak") < Rand(120))
			{
				DoQuestCheckDelay("Rand_ContrabandInterruption", 0.1);
				ContrabandInterruptionSetStatus(true);
			}
			if (GetSummonSkillFromNameToOld(pchar, SKILL_SNEAK) < dRand(12))
			{
				ArmContraFreightPursuit();
				SetTimerCondition("Rand_ContrabandInterruptionAtSeaEnded", 0, 0, 2, false);
				Pchar.quest.Rand_ContrabandAtSeaEnded.win_condition.l1 = "MapEnter";
				Pchar.quest.Rand_ContrabandAtSeaEnded.win_condition = "Rand_ContrabandAtSeaEnded";
			}
			DeleteAttribute(pchar, "quest.Contraband.active");
			// финализация НА ВЫХОДЕ из диалога (в ноде нельзя - уйдёт бродить, не договорив): бессмертный немой истукан
			AddDialogExitQuestFunction("SmugScamSurvivorRetire");
		break;

		case "SmugFreightPre_Kill": // отказ: добиваем последнего, груз остаётся нам, отношение контрабандистов - в ноль
			dialog.text = "So be it... The Coastal Brotherhood will not forget this. Damn you...";
			link.l1 = "I'm trembling already. Go join the others.";
			link.l1.go = "exit";
			AddSimpleRumourCity("A dreadful thing at the bay: the whole gang of fences slaughtered to a man - even the one who begged for mercy. The Coastal Brotherhood is furious, they say. I wouldn't want to be in that butcher's shoes when the smugglers find out who did it.", Pchar.quest.contraband.City, 3, 5, "");
			int pkRel = ChangeContrabandRelation(pchar, 0);
			if (pkRel > 0) ChangeContrabandRelation(pchar, -pkRel); // минусуем отношение контрабандистов в ноль
			ChangeCharacterReputation(pchar, -20); // добить молящего о пощаде - удар по репутации ГГ
			pchar.quest.generate_trade_quest_progress = "";
			pchar.quest.generate_trade_quest.over = "yes";
			AddQuestRecord("DELIVERY_TRADE_QUEST", "smug7");
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(makeint(pchar.CargoQuest.iTradeGoods)));
			CloseQuestHeader("DELIVERY_TRADE_QUEST");
			RemoveSmugglersFromShore(); // лагерь с грузами сворачивается (гард по quest.SmuggleFreight не даст закрыть "Сбыт контрабанды")
			DeleteAttribute(pchar, "quest.SmuggleFreight");
			DeleteAttribute(pchar, "quest.Contraband.active");
			// возвращаем его в бой смертным на последнем издыхании - добить легко
			DeleteAttribute(pchar, "GenQuest.SmugScamSurvivorId"); // финализация пощады не понадобится
			// после его смерти снимаем боевую тревогу (врагов не останется)
			pchar.quest.SmugScamKillDone.win_condition.l1 = "NPC_Death";
			pchar.quest.SmugScamKillDone.win_condition.l1.character = NPChar.id;
			pchar.quest.SmugScamKillDone.win_condition = "SmugScamKillDone";
			LAi_SetImmortal(NPChar, false);
			LAi_SetWarriorType(NPChar);
			LAi_group_MoveCharacter(NPChar, pchar.GenQuest.Smugglers_Group);
			LAi_group_SetRelation(pchar.GenQuest.Smugglers_Group, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups(LAI_GROUP_PLAYER, pchar.GenQuest.Smugglers_Group, true);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "Exchange":
			// сама торговля -->
			if(FindFirstContrabandGoods(Pchar) == -1 && sti(Pchar.quest.Contraband.Counter) == 0)
			{
				dialog.Text = "Well, what are you doing here? You don't have anything to sell! Everything you have in your hold can be bought in the city!";
				Link.l1 = "Bad luck.";
				Link.l1.go = "Exit";
			}
			else
			{
				// установим окружение -->
				if (sti(Pchar.quest.Contraband.Counter) == 0) // не торговали ещё
				{
					if(drand(11) == 3)
					{
						dialog.Text = "Well, well... I think we'll pick up your goods for free. You're hardly going to complain to the authorities.";
						Link.l1 = "Well, try it!";
						Link.l1.go = "Exit_fight";
										break;
					}
					// море и солдаты на суше НЕЗАВИСИМЫ!!!
					if(GetSummonSkillFromNameToOld(Pchar, SKILL_SNEAK) < dRand(12))
					{
						SetCoastalGuardPursuit();
					}
					// при убегании от патруля на карту - корабли трём
					SetTimerCondition("Rand_ContrabandInterruptionAtSeaEnded", 0, 0, 2, false);// если в порту сидим, спим, то 2 день

					Pchar.quest.Rand_ContrabandAtSeaEnded.win_condition.l1 = "MapEnter";
					Pchar.quest.Rand_ContrabandAtSeaEnded.win_condition = "Rand_ContrabandAtSeaEnded";
				}
				// установим окружение <--
				dialog.Text = RandPhraseSimple("So, what kind of cargo do you want to sell?",
																	"What are you selling?");
				Link.l1 = "Well, let's see.";
				Link.l1.go = "Exchange1";
				if (sti(Pchar.quest.Contraband.Counter) == 0)
				{
					Link.l99 = "I think I've changed my mind.";
					Link.l99.go = "Cancellation";
				}
			}
		break;

		case "Exchange1":
			NPChar.quest.meeting = NPC_Meeting;
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			LaunchContrabandTrade(CharacterFromId(pchar.GenQuest.Contraband.SmugglerId),  sti(pchar.FindContrabandGoods.StoreIdx));
		break;

		case "Finish_exit":
			// таможня на суше
			if(GetSummonSkillFromName(pchar, "Sneak") < Rand(120) && !CheckAttribute(pchar, "quest.Contraband.InterruptionGot"))
			{
				Pchar.GenQuest.contraTravel.PatrolFight = true;
				DeleteAttribute(PChar, "GenQuest.contraTravel.PatrolBribed"); // новый патруль - мирный исход прошлого не в счёт
				Pchar.quest.Contraband.InterruptionGot = true;
				DoQuestCheckDelay("Rand_ContrabandInterruption", 0.1);
				ContrabandInterruptionSetStatus(true);
			}
			else
			{
				SmugRevenge_ArmDeathWatch(); // сухопутного патруля нет: вырежешь смуглеров - вместо нацпатруля придёт возмездие братства
			}
			if (!CheckAttribute(pchar, "quest.Contraband.SneakGot"))
			{
				pchar.quest.Contraband.SneakGot = true;
				AddCharacterExpToSkill(Pchar, "Sneak", 100);
				SetTimerCondition("ContraСlearGot", 0, 0, 1, false);
			}
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
			ChangeContrabandRelation(pchar, 15);
			OfficersReaction("bad");
			ChangeCharacterReputation(pchar, -1);

			CloseQuestHeader("Gen_Contraband");

			DeleteAttribute(Pchar, "quest.Contraband.active");
			DeleteAttribute(Pchar, "quest.Contraband.counter");
			DeleteAttribute(Pchar, "quest.Contraband.price1");
			DeleteAttribute(Pchar, "quest.Contraband.price2");
			DeleteAttribute(Pchar, "quest.Contraband.price3");
			DeleteAttribute(Pchar, "quest.Contraband.price4");
			DeleteAttribute(Pchar, "quest.Contraband.price5");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX1");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX2");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX3");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX4");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX5");
			DeleteAttribute(Pchar, "quest.Contraband.sum");
			DeleteAttribute(Pchar, "quest.Contraband.ChooseGoodsQty");
			DeleteAttribute(Pchar, "quest.Contraband.ChooseGoodsID");

			Pchar.quest.Rand_Smuggling.over = "yes";
			RemoveSmugglersFromShore();
		break;
	}
}
