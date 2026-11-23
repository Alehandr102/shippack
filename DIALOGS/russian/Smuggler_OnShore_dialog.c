
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, Diag;
	string NPC_Meeting;
	int Shit, i;
	ref refStore;
	ref sld;

	DeleteAttribute(&Dialog,"Links");

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
				dialog.text = RandPhraseSimple("Всё, капитан, нам пора уходить. Бывай.", "Дело сделано, разошлись. Не поминай лихом.");
				link.l1 = "Бывай.";
				link.l1.go = "exit";
				break;
			}
			// === передача КОНТРАБАНДНОГО ФРАХТА (заказ из журнала торговца) ===
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
					dialog.text = "Э, нет. А корабль твой где? На горбу товар притащить вздумал"+ GetSexPhrase("","а") +"? Швартуйся в этой бухте - перегрузим всё тихо, борт к борту.";
					link.l1 = "Хорошо, подойду на корабле.";
					link.l1.go = "exit";
					break;
				}
				// хитрецы: взяли заказ на малой эскадре, а в бухту привели большую/мощную - сделки не будет
				if (!CheckSmuggleFreightShips())
				{
					dialog.text = "Ты что творишь?! Уговор был - приходить тихо: не больше трёх вымпелов, и без тяжёлых посудин выше третьего класса. Уводи свою армаду, пока патруль не слетелся, - иначе сделки не будет.";
					link.l1 = "Ладно, уведу лишние корабли и вернусь.";
					link.l1.go = "exit";
					break;
				}
				int fchkGoods = makeint(pchar.CargoQuest.iTradeGoods);
				int fchkQty   = makeint(pchar.CargoQuest.iQuantityGoods);
				if (GetSquadronGoods(pchar, fchkGoods) < fchkQty)
				{
					dialog.text = "Где груз? Тут не всё. Без полного груза и разговора нет.";
					link.l1 = "Сейчас доставлю.";
					link.l1.go = "exit";
					break;
				}
				// КИДАЛОВО (флаг отролен при взятии заказа, шанс как в обычной сделке): платы не будет - бой или груз даром
				if (CheckAttribute(pchar, "CargoQuest.SmuggleScam"))
				{
					dialog.text = RandPhraseSimple("Так-так, груз на месте... Знаешь что? Пожалуй, мы заберём его бесплатно. Жаловаться-то тебе некому - не к губернатору же с таким товаром идти.",
						"Хм, всё привёз"+ GetSexPhrase("","ла") +", молодец... А теперь слушай сюда: платы не будет. Скажи спасибо, что жив"+ GetSexPhrase("ым","ой") +" отпускаем. Ну, что выберешь?");
					link.l1 = "Груз вы получите только с моим клинком в придачу!";
					link.l1.go = "SmuggleFreight_ScamFight";
					link.l2 = "(прикинув расклад) Ладно, забирайте. Сегодня ваша взяла.";
					link.l2.go = "SmuggleFreight_ScamGive";
					break;
				}
				dialog.text = "(вполголоса) Так, груз на месте. Принимаем. Вот твои деньги - и убирайся поскорее, тут может стать жарко.";
				link.l1 = "Уже ухожу.";
				link.l1.go = "SmuggleFreight_BayDone";
				break;
			}
			if(CheckAttribute(PChar, "quest.Contraband.active"))
			{
				if ((sti(pchar.Ship.Type) != SHIP_NOTUSED) && !CheckCharacterPerk(pchar, "UnlimitedContra"))
				{
					if (GetCompanionQuantity(pchar) > 1 && GetBaseHeroNation() != PIRATE)
					{
						dialog.text = NPCStringReactionRepeat("Слушай, тебе же в таверне ясно сказали, чтобы ты приходил"+ GetSexPhrase("","а") +" с одним кораблём. Проваливай и избавляйся от своей эскадры.",
							"Ш"+ GetSexPhrase("ел","ла") +" бы ты отсюда. А то сами тебя патрулю сдадим.",
							"Давай-давай, садись в шлюпку и уматывай.",
							"Как же ты меня утомил"+ GetSexPhrase("","а") +"...", "block", 1, npchar, Dialog.CurrentNode);
						link.l1 = HeroStringReactionRepeat("Хорошо-хорошо, сейчас вернусь на флагмане.",
							"Да не ругайся, только сбегаю в портовое управление, сдам лишние корабли, и сразу назад.",
							"Эх-х, не удалось схитрить...",
							"Да, я настырн"+ GetSexPhrase("ый","ая") +"!", npchar, Dialog.CurrentNode);
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
						dialog.text = NPCStringReactionRepeat("Тебе что, не говорили, чтобы ты не являл"+ GetSexPhrase("ся","ась") +" на таком приметном корыте? Ты бы ещё парочку мановаров с собой прихватил"+ GetSexPhrase("","а") +". Проваливай и приходи на меньшем корабле.",
							"Ш"+ GetSexPhrase("ел","ла") +" бы ты отсюда. А то сами тебя патрулю сдадим.",
							"Давай-давай, садись в шлюпку и уматывай.",
							"Как же ты меня утомил"+ GetSexPhrase("","a") +"...", "block", 1, npchar, Dialog.CurrentNode);
						link.l1 = HeroStringReactionRepeat("Хорошо-хорошо, пош"+ GetSexPhrase("ёл","ла") +" кораблик менять.",
							"Не злись, я мигом - одна нога тут, другая там.",
							"Эх-х, не удалось схитрить...",
							"Да, я настырн"+ GetSexPhrase("ый","ая") +"!", npchar, Dialog.CurrentNode);
						link.l1.go = DialogGoNodeRepeat("exit", "", "", "", npchar, Dialog.CurrentNode);
						break;
					}
				}
				Dialog.snd = "voice\SMSH\SMSH001";
				dialog.Text = RandPhraseSimple("Эй, что ты тут забыл"+ GetSexPhrase("","а") +", "+ GetSexPhrase("приятель","подруга") +"?",
                                          RandSwear() + "Что тебе тут нужно?!");
				Link.l1 = RandPhraseSimple("Расслабься, парень, я прив"+ GetSexPhrase("ёз","езла") +" товар!", "Ты чем смотришь? Я доставил"+ GetSexPhrase("","а") +" товар!");
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
					Link.l2 = "Я здесь по распоряжению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.KillSmugglers.MayorId)].city+"Gen") + "! Приказываю вам сложить оружие и следовать за мной - вы арестованы!";
					Link.l2.go = "GenQuestKillContraband_1";
				}
			}
			else
			{
				if (CheckAttribute(PChar, "GenQuest.contraTravel.payed") && sti(PChar.GenQuest.contraTravel.payed) == true) //если заплатил, то разговаривают, иначе посылают
				{
					dialog.Text = RandPhraseSimple("Ага, наконец-то!!! Вот и ты, а то мы уж заждались совсем. Идём, мы отчаливаем.", "Пошевеливайся, тут повсюду патрули. Нужно быстрее убраться отсюда!");
					//по заданию губернатора, истребление контры, только эта ветка.
					if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers == "" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(pchar.location)]))
					{
						Link.l1 = "Я здесь по распоряжению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.KillSmugglers.MayorId)].city+"Gen") + "! Приказываю вам сложить оружие и следовать за мной - вы арестованы!";
						Link.l1.go = "GenQuestKillContraband_1";
						break;
					}
					//если набрал пассажиров, в сад..
					if (GetPassengersQuantity(PChar) > 0)
					{
						dialog.Text = "А ты кто такой? Мы договаривались доставить одного пассажира, а не толпу.";
						Link.l1 = "Упс...";
						Link.l1.go = "Exit";
						break;
					}
					//если набрал компаньонов или купил корабль себе уже :), в сад..
					if (GetCompanionQuantity(PChar) > 1 || sti(PChar.ship.type) != SHIP_NOTUSED)
					{
						dialog.Text = RandPhraseSimple("Ты кто? Мы договаривались доставить пассажира, а не капитана.", "Проваливай! Пока не избавишься от своего корабля, не приходи.");
						Link.l1 = "Ах, чёрт...";
						Link.l1.go = "Exit";
						break;
					}
					//типа помог отбиться, свой парень...
					if (CheckAttribute(PChar, "GenQuest.contraTravel.PatrolFight") && sti(PChar.GenQuest.contraTravel.PatrolFight) == true)
					{
						if (chrDisableReloadToLocation) // ещё бой идёт
						{
							dialog.Text = "Патруль! Мы тебя не знаем, ты нас не знаешь.";
							Link.l1 = "Да уж...";
							Link.l1.go = "Exit";
							break;
						}
						if (CheckAttribute(PChar, "GenQuest.contraTravel.PatrolBribed")) // патруль спроважен миром (откуп/на испуг), боя не было
						{
							dialog.Text = RandPhraseSimple("Ловко ты спровадил"+ GetSexPhrase("","а") +" этих служак - и без единой капли крови. Уважаю. Идем.", "Умеешь ты улаживать дела с патрулём. Отправляемся.");
							DeleteAttribute(PChar, "GenQuest.contraTravel.PatrolBribed");
						}
						else
						{
							dialog.Text = RandPhraseSimple("Спасибо за помощь. Мы не забудем этого. Отправляемся.", "Молодец! Отлично сражал"+ GetSexPhrase("ся","ась") +". Идем.");
						}
						ChangeContrabandRelation(PChar, 5);
						PChar.GenQuest.contraTravel.PatrolFight = false;
						AddCharacterExpToSkill(Pchar, "Sneak", 50);
					}
					//тут все ок, отправляемся.
					Link.l2 = "Уже иду.";
					Link.l2.go = "Exit";
					
					//поместим Грея в локацию.
					refStore = CharacterFromID("Abracham_Gray");
					ChangeCharacterAddressGroup(refStore, "Ship_deck", "goto", "goto2");
					refStore.dialog.Filename = "Smuggler_Ship_dialog.c";
					refStore.dialog.CurrentNode = "Travel_talkStart";
					//поплыл, иначе RemoveTravelSmugglers грохнет всю ветку
					PChar.GenQuest.contraTravel.ship = true;
					PChar.quest.Munity = "";  // признак выхода с палубы

					SetLaunchFrameFormParam("Прошло " + sti(Pchar.GenQuest.contraTravel.destination.days) + " дней." + NewStr() + "Палуба корабля контрабандистов.",
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
								dialog.Text = RandSwear()+ "Патруль! Мы тебя не знаем, ты нас не знаешь!";
								Link.l1 = "Точно!";
								Link.l1.go = "Exit";
								break;
							}
							if (CheckAttribute(PChar, "GenQuest.contraTravel.PatrolBribed")) // патруль спроважен миром (откуп/на испуг), боя не было
							{
								dialog.Text = RandPhraseSimple("Ловко ты спровадил"+ GetSexPhrase("","а") +" этих служак - и без единой капли крови. Уважаю.", "Умеешь ты улаживать дела с патрулём. С такими капитанами приятно работать.");
								DeleteAttribute(PChar, "GenQuest.contraTravel.PatrolBribed");
							}
							else
							{
								dialog.Text = RandPhraseSimple("Спасибо за помощь. Мы не забудем этого. Отправляемся.", "Молодец! Отлично сражал"+ GetSexPhrase("ся","ась") +". Идем.");
							}
							ChangeContrabandRelation(PChar, 5);
							DeleteAttribute(PChar,"GenQuest.contraTravel.PatrolFight");
							ContrabandInterruptionSetStatus(false);
							AddCharacterExpToSkill(Pchar, "Sneak", 50);
							pchar.quest.Contraband.ThanksGot = true; // благодарность ОДИН раз: дальше все смуглеры сворачивают разговор (чистится при новой высадке)
							Link.l2 = "Уже иду.";
							Link.l2.go = "Exit";
							break;
						}
					}
					else
					{
						// один диалог - одна сделка: повторный запуск торговли (Exchange1) УБРАН - закрывает дыру с многократным спавном патруля и фармом наград
						dialog.Text = "Слушай, проваливай отсюда! Дело сделано - нечего тут отсвечивать, пока патруль не нагрянул.";
						Link.l2 = RandPhraseSimple("Ты смеешь мне угрожать, собака?", "Ты подавишься своими словами, каналья!");
						Link.l2.go = "Exit_fight";
						Link.l3 = "Да ладно, я знаю, что такое бизнес. До встречи.";
						Link.l3.go = "Exit";
					}
				}
			}
		break;

		case "No_Ship":
			Dialog.snd = "voice\SMSH\SMSH003";
			dialog.Text = "На своём горбу, что ли? Где твой корабль?";
			Link.l1 = RandPhraseSimple("Сначала я хотел"+ GetSexPhrase("","а") +" убедиться, что здесь нет никакого подвоха.", "Не хотел"+ GetSexPhrase("","а") +" рисковать кораблём. Предпочитаю сначала поглядеть на все своими глазами.");
			Link.l1.go = "No_ship_1";
			Link.l2 = "Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;

		case "No_ship_1":
			Dialog.snd = "voice\SMSH\SMSH004";
			dialog.Text = "Мы люди честные, и обманом не живём. Приводите свой корабль, только аккуратнее - чтобы за вами не увязался береговой патруль.";
			Link.l1 = "Хорошо. Ждите меня здесь.";
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

			AddSimpleRumourCity("Слыхали? Недавно в бухте патруль облаву на контрабандистов устроил. И чтобы вы думали? Капитану, который местным скупщикам товар отгружал, удалось уйти! " +
				"Лихие ребята у него в команде оказались - весь патрульный отряд положили. И наши местные, как в воду канули. Так в бухте ничего, кроме трупов, и не нашли - ни товара, ни тайника, ни самих контрабандистов. Вот же как у этих проходимцев промысел-то организован!", Pchar.quest.contraband.City, 3, 5, "");

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
				dialog.Text = "Отменяется? Ты что, смеёшься?";
				Link.l1 = "Совершенно серьезно.";
				Link.l1.go = "Cancellation_1";
			}
			else
			{
				dialog.Text = "Ну и ладно. Мы и так неплохо поторговали.";
				Link.l1 = "Точно!";
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
					dialog.Text = "Паршив"+ GetSexPhrase("ец","ка") +"! Ты не можешь нас так дурачить! Поплатишься за это своей головой!";
					Link.l1 = "А вот это вряд ли!";
					Link.l1.go = "Exit_fight";
				}
				else
				{
					ChangeContrabandRelation(pchar, -10);
					Dialog.snd = "voice\SMSH\SMSH007";
					dialog.Text = "Это не сойдёт тебе с рук!";
					Link.l1 = "Отправляйтесь гавкать куда-нибудь ещё, псы!";
					Link.l1.go = "Exit_cancel";
				}
			}
			else
			{
				ChangeContrabandRelation(pchar, -10);
				Dialog.snd = "voice\SMSH\SMSH008";
				dialog.Text = "Ты об этом ещё пожалеешь!";
				Link.l1 = "Ну-ну!";
				Link.l1.go = "Exit_cancel";
			}
		break;

		case "GenQuestKillContraband_1":
			//счётчик подстав по "метро"...
			if(CheckAttribute(PChar, "GenQuest.contraTravel.active") && sti(PChar.GenQuest.contraTravel.active) == true)
			{
				Statistic_AddValue(PChar, "contr_TravelKill", 1);
				ChangeContrabandRelation(pchar, -20); //репу контры вниз
			}
			dialog.Text = "Ты нам сейчас ответишь за такую подставу!";
			Link.l1 = "Тогда вы все умрёте. У меня чёткий приказ - в случае сопротивления живых не брать!";
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
				fReward = fReward - iLateFine; // просрочен - полцены
			} 
			else if (GetQuestPastDayParam("CargoQuest") * 2 < sti(pchar.CargoQuest.iDaysExpired)) 
			{
				iBonus = makeint(fReward * 0.2);
				fReward = fReward + iBonus; // досрочно - премия
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
			ChangeContrabandRelation(pchar, 3); // +репутация у контрабандистов
			
			if (!CheckAttribute(pchar, "contrabandmoneyback")) pchar.contrabandmoneyback = fReward; // в зачёт перка "Заядлый контрабандист"
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
			pchar.contratradedmoney = fReward; // откуп патрулю считается как 10%+сложность от этой суммы (как в обычной сделке)
			
			// патрули как в обычной сделке: сухопутный (на берегу) + морской (патруль ЛИБО пираты)
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
			// LEO: запись об исходе НЕ пишем тут - последний кидала ВСЕГДА сдаётся и расплачивается (SmugScam_Surrender_Done),
			// журнал заполняется ПО ФАКТУ сдачи (smug6). smug5 писалась преждевременно при выборе "драться" и противоречила исходу.
			CloseQuestHeader("DELIVERY_TRADE_QUEST");
			DeleteAttribute(pchar, "CargoQuest.SmuggleScam");
			DeleteAttribute(pchar, "quest.Contraband.active");
			AddSimpleRumourCity("Слыхали? В бухте нашли всю шайку скупщиков контрабанды - перебиты до единого. Поговаривают, болваны решили кинуть не того капитана: забрать груз и не заплатить. Ну вот и расплатились - кровью.", Pchar.quest.contraband.City, 3, 5, "");
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
			dialog.text = "Разумный выбор. Разгружай - и проваливай. Ты нас не видел"+ GetSexPhrase("","а") +", мы тебя не знаем.";
			link.l1 = "Запомню я вас, ребята... Ещё свидимся.";
			link.l1.go = "exit";
			AddSimpleRumourCity("Говорят, скупщики в бухте кинули заезжего капитана - груз забрали, а платить отказались. Ох, не завидую я им: такие обиды на берегу не прощают.", Pchar.quest.contraband.City, 3, 5, "");
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
			dialog.text = "Стой, стой!.. Хватит! Твоя взяла, дьявол... Вот, держи - плата за груз сполна, как уговаривались. Забирай и пощади.";
			link.l1 = "Так бы сразу. И запомни: со мной такие шутки плохо кончаются.";
			link.l1.go = "SmugScam_Surrender_Done";
		break;

		case "SmugScam_Surrender_Done":
			dialog.text = "Уговор дороже жизни, теперь-то я понял... Уходим, ребята... ах, да... уходить-то уже некому...";
			link.l1 = "";
			link.l1.go = "exit";
			AddSimpleRumourCity("Слыхали про резню в бухте? Скупщики вздумали кинуть капитана с грузом, да не на того напали. В живых остался один - и тот, говорят, на коленях выложил всю плату до последнего пиастра.", Pchar.quest.contraband.City, 3, 5, "");
			// расплата за груз: если груз ещё в трюме (исход "перебить всех") - передаём его, деньги в любом случае
			int ssGoods = makeint(pchar.CargoQuest.iTradeGoods);
			int ssQty   = makeint(pchar.CargoQuest.iQuantityGoods);
			bool ssHadCargo = (GetSquadronGoods(pchar, ssGoods) >= ssQty); // LEO: груз ещё при нас = пришли из "перебить всех"; нет = из "отдал груз даром"
			if (ssHadCargo) RemoveCharacterGoods(pchar, ssGoods, ssQty);
			
			Log_Info(XI_ConvertString("Freight_TotalBlade"));
			Log_InfoColored("yellow", XI_ConvertString("Freight_PayBlood") + FindRussianMoneyString(makeint(pchar.CargoQuest.iMoney)));
			
			AddMoneyToCharacter(pchar, makeint(pchar.CargoQuest.iMoney));
			
			// LEO: журнал ПО ФАКТУ - сделка завершилась миром (груз передан, плата получена), запись отложена сюда из ScamFight.
			// Только для исхода "перебить всех" (груз был при нас): в ветке "отдал груз" уже стоит своя запись smug4.
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
					"Пощади!.. Хватит крови. Давай просто завершим сделку: груз - нам, деньги - тебе, как и уговаривались. А убьёшь меня - братство узнает, кто резал своих, и в каждом порту тебя будет ждать нож.",
					"Стой, остановись! Предлагаю по-деловому: заверши сделку, как договаривались, - и разойдёмся. Иначе о резне узнают НАШИ, и торговать с берегом тебе станет ох как трудно."),
				"Всё, твоя взяла!.. Но подумай головой: сорвёшь сделку - Береговое братство таких обид не забывает. Давай закончим обмен по-честному - и в расчёте.");
			link.l1 = "Твоя взяла. Завершим сделку - и разойдёмся.";
			link.l1.go = "SmugFreightPre_Spare";
			link.l2 = "Нет. Отправляйся за своими дружками.";
			link.l2.go = "SmugFreightPre_Kill";
		break;

		case "SmugFreightPre_Spare": // пощада: сделка завершается под клинком, бонусы за милосердие
			dialog.text = "Вот и славно... Груз на берег - и расходимся. О том, что здесь было, братству знать не обязательно. Я ничего не видел.";
			link.l1 = "Бывай. И постарайся забыть моё лицо.";
			link.l1.go = "exit";
			AddSimpleRumourCity("Поговаривают, в бухте у контрабандистов дело дошло до клинков - почти всю шайку положили. А самое чудное: последний выживший и заезжий капитан ударили по рукам, и сделка-таки состоялась. Вот же народ - сначала режутся, потом торгуют!", Pchar.quest.contraband.City, 3, 5, "");
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
			dialog.text = "Что ж... Береговое братство этого не забудет. Будь ты проклят" + GetSexPhrase("","а") + "...";
			link.l1 = "Уже дрожу. Отправляйся следом за остальными.";
			link.l1.go = "exit";
			AddSimpleRumourCity("Страшное дело в бухте: всю шайку скупщиков вырезали подчистую - даже того, что молил о пощаде. Береговое братство, говорят, рвёт и мечет. Не хотел бы я оказаться на месте душегуба, когда контрабандисты найдут концы.", Pchar.quest.contraband.City, 3, 5, "");
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
				dialog.Text = "Ну, и чего ты здесь делаешь? У тебя же ничего нет, что можно продать! Всё, что лежит у тебя в трюме, можно купить в городе!";
				Link.l1 = "Не повезло.";
				Link.l1.go = "Exit";
			}
			else
			{
				// установим окружение -->
				if (sti(Pchar.quest.Contraband.Counter) == 0) // не торговали ещё
				{
					if(drand(11) == 3)
					{
						dialog.Text = "Так-так... пожалуй, мы заберём твой товар бесплатно. Врядли ты побежишь жаловаться властям.";
						Link.l1 = "Что ж, попробуйте!";
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
				dialog.Text = RandPhraseSimple("Итак, какой груз ты хотел"+ GetSexPhrase("","а") +" продать?",
																	"Что продаёшь?");
				Link.l1 = "Ну давай посмотрим.";
				Link.l1.go = "Exchange1";
				if (sti(Pchar.quest.Contraband.Counter) == 0)
				{
					Link.l99 = "Пожалуй, я передумал"+ GetSexPhrase("","а") +".";
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
			if(GetSummonSkillFromName(pchar, "Sneak") < Rand(120) && !CheckAttribute(pchar,"quest.Contraband.InterruptionGot"))
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
			if (!CheckAttribute(pchar,"quest.Contraband.SneakGot"))
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
