// boal 25/04/04 общий диалог Governor
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Governor\" + NationShortName(sti(NPChar.nation)) + "_Governor.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}

	int Matheria1 = 4000;//шёлк
	int Matheria2 = 3000;//красное
	int Matheria3 = 2000;//чёрное
	int BuildPrice = 3000000;

    // вызов диалога по городам <--

    ref offref, sld;
    int i, cn;
    int qty;
    int iSumm;
    string attrLoc;

    /// выкуп колонии
    ref CaptGovenor, FortColony;
    int f, colony_money;

    int k = 1000;
    if (CheckAttribute(Nations[sti(NPChar.nation)], "Fort"))
    {
        k = (300 - sti(Nations[sti(NPChar.nation)].Fort)*10);
    }
	bool ok;

	attrLoc = Dialog.CurrentNode;
	if (findsubstr(attrLoc, "CityGive_" , 0) != -1)
 	{
        i = findsubstr(attrLoc, "_" , 0);
	 	NPChar.quest.CityIdx = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 	    Dialog.CurrentNode = "ColonyGive";
 	}

	switch (Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOff");
		break;

		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
		break;

		case "arest":
			dialog.text = "Guards! Take "+ GetSexPhrase("him", "her") +"!";
			link.l1 = "Well, no way! You won't take me just like that!";
		    link.l1.go = "fight";
		break;

		case "Patent_Doubt":
			dialog.text = "Lay it smoothly... I should have you hanged for just being impertinent. There is no faith in such notorious pirates, but now times are hard in our archipelago and any help will be useful. So be it, I'll sign you a patent, and a pardon. Just don't make me regret it.";
			link.l1 = "You won't regret it, " + GetAddress_FormToNPC(NPChar) + "!";
			link.l1.go = "Patent_Give";
			link.l2 = "Ha! Be led! It was just a joke, I don't want to have anything to do with your worthless state.";
			link.l2.go = "arest_3";
		break;
		case "Patent_Give":
            pchar.PatentNation = NationShortName(sti(NPChar.nation));
            pchar.PatentPrice = 0;
			dialog.text = "Please, I hope for further cooperation. Come to me from time to time. I may have important tasks for you.";
			link.l1 = "Thank you. Goodbye, " + GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "exit";
			AddDialogExitQuest("any_patent_take");
			ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", -100);
		break;

		case "First time":
            NextDiag.TempNode = "First time";
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar,
					LinkRandPhrase("You dare to come to the Governor-General?! A bold step...", "How did these idlers allow the enemy to break in to me?! It's mind-boggling...", "Yeah, my security isn't worth much, since"+ GetSexPhrase("some kind of bum","some kind of bum") +"running around in my residence..."),
					LinkRandPhrase("What do you need, "+ GetSexPhrase("scoundrel","scoundrel") +"?! My soldiers have already are on your trail, you can't get far, dirty pirate!", "Dirty murderer, get out of my residence! Guards!!", "I'm not afraid of you,scoundrel! You'll be hanged in the fort soon, you won't get far..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("The soldiers are not worth much...", "They will never catch me."),
					RandPhraseSimple("Shut your mouth, buddy, or I'll rip out your filthy tongue!", "I'll tell you what, buddy: sit tight and you'll live..."));
				link.l1.go = "fight";
				break;
			}
			if (sti(NPChar.nation) != PIRATE && ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", 0) > 15)
            {
                dialog.text = "Oh! " + GetFullName(pchar) + "! We just recently sent bounty hunters to bring you here. And you know, it's already cost us a tidy sum.";
                if (sti(NPChar.nation) != PIRATE && ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", 0) == 100 && !isMainCharacterPatented())
				{
				link.l3 = "You've seen what I'm doing with the enemy, now imagine what benefits I can bring to your country as an ally. I suggest just once, here and now: give me a patent and all your enemies will become mine.";
				link.l3.go = "Patent_Doubt";
				}
				link.l2 = "E... I think I should go.";
				link.l2.go = "arest";
				break;
            }
			if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY && sti(NPChar.nation) != PIRATE)
			{
    			dialog.text = "The enemy is in residence! Alarm!!";
				link.l1 = "Ah, the devil!";
				link.l1.go = "fight";
				break;
			}

			if (npchar.quest.meeting == "0")
			{
				dialog.text = "I heard that you were very insistent on asking for an audience. My name is " + GetFullName(npchar) +
                              ". I am the governor general of the " + NationNameGenitive(sti(NPChar.nation))+ " colonies, Viceroy of the " + NationKingsName(npchar)+
                              " Crown in these waters. Now kindly inform me of the purpose of your visit, " + GetAddress_Form(NPChar) + ".";
				link.l1 = "My name is " + GetFullName(pchar) + ".";
				link.l1.go = "node_1";
				npchar.quest.meeting = "1";
			}
			else
			{
				dialog.text = RandPhraseSimple("Oh, is that you again? On what issue did you need the Governor-General of " + NationNameGenitive(sti(NPChar.nation))+ " this time?",
                              "Are you interrupting me from important state affairs again? What do you want, " +GetAddress_Form(NPChar)+"?");
				link.l1 = "I want to talk about working for the benefit of the Crown of " + NationNameGenitive(sti(NPChar.nation)) + ".";
				link.l1.go = "work";
				link.l2 = "I need to talk to you about something.";
				link.l2.go = "quests"; // файл нации
				link.l10 = "I'm sorry, but I have things to do.";
				link.l10.go = "exit";

				// Gregg: постройка копии «Солей Руаяль» - ветку ведёт тот губернатор, что принял заказ:
				// quest.answer_1 - заказ принят, Tuning - материалы ещё не сданы, quest.Gave_Ship - корабль уже выдан.
				if (sti(npchar.nation) == FRANCE && CheckAttribute(npchar, "quest.answer_1") && !CheckAttribute(npchar, "quest.Gave_Ship"))
				{
					if (CheckAttribute(npchar, "Tuning"))
					{
						Link.l15 = "About the materials for the man o'war.";
						link.l15.go = "check_material";
					}
					else
					{
						if (GetQuestPastDayParam("pchar.questTemp.buildSoley") >= 60)
						{
							Link.l15 = "Well, is the man o'war ready?";
							link.l15.go = "get_ship";
						}
						else
						{
							Link.l15 = "How does the building go?";
							link.l15.go = "build_ship_wait";
						}
					}
				}
			}
			if(CheckAttribute(npchar,"patent.othertime") && FindCharacterItemByGroup(Pchar, PATENT_ITEM_TYPE) == "")
			{
				link.l16 = "Greetings, " + GetFullName(npchar) + ". I would like to return to the issue of the patent.";
				link.l16.go = "Other time patent";
			}
		break;

		case "build_ship":
			dialog.Text = "A sound thought, though the treasury has not a livre for it. But if the material is yours and you pay the shipwrights, I will arrange the rest.";
			Link.l1 = "Agreed. What am I to bring, and how much?";
			Link.l1.go = "build_ship_1";
			Link.l2 = "I have neither the time nor the money for that.";
			Link.l2.go = "exit";
		break;

		case "build_ship_1":
			dialog.Text = "For a royal man o'war we shall need "+Matheria2+" units of mahogany, "+Matheria3+" of ebony and "+Matheria1+" of silk. Canvas and planking our colonies will supply. From you - "+BuildPrice+" gold in advance, for the purchase, the hiring and everything else.";

			if(makeint(Pchar.money) >= BuildPrice)
			{
				Link.l1 = "Agreed. Here is the money; the rest I will bring.";
				Link.l1.go = "build_ship_2";
			}
			Link.l2 = "Steep. I will do without.";
			Link.l2.go = "exit";
		break;

		case "build_ship_2":
			AddMoneyToCharacter(Pchar, -BuildPrice);
			dialog.Text = "Then we lay the keel. We shall expect you with the cargo.";
			npchar.quest.answer_1 = "true";

			NPChar.Tuning.Matherial1 = Matheria1;
			NPChar.Tuning.Matherial2 = Matheria2;
			NPChar.Tuning.Matherial3 = Matheria3;
			// Gregg: TempNode на check_material тут не ставим - иначе до конца сдачи материалов
			// губернатор умеет только спрашивать про них. Вход в ветку - ссылкой из узла "First time".
			AddQuestRecord("Fra_Line_13_BuildSoley", "1");
			AddQuestUserData("Fra_Line_13_BuildSoley", "sSex", GetSexPhrase("","а"));
			AddQuestUserData("Fra_Line_13_BuildSoley", "sGoods", "mahogany - " + Matheria2 + ", ebony - " + Matheria3 + ", silk - " + Matheria1);
			Link.l1 = "Goodbye.";
			Link.l1.go = "exit";
		break;

		// Gregg: заглушка на срок постройки, чтобы ссылка из "First time" не вела в пустоту
		case "build_ship_wait":
			dialog.Text = "The keel is laid, the rest is the yard's business. The hull will not take the water before its time, however much we both wish it.";
			link.l1 = "Then I will call again later.";
			link.l1.go = "exit";
		break;

		case "check_material":
			// Gregg: расклинивает старые сейвы, где build_ship_2 успел записать сюда TempNode
			// и губернатор больше ни о чём другом говорить не мог.
			NextDiag.TempNode = "First time";
			dialog.Text = "The yard stands idle. Have you brought what I asked for?";
			Link.l1 = "Yes, I managed to get some of it.";
			Link.l1.go = "check_material_1";
			Link.l2 = "No, still working on it.";
			Link.l2.go = "Exit";
		break;

		case "check_material_1":

			checkSoleyMatherial(Pchar, NPChar, GOOD_SILK, GOOD_MAHOGANY, GOOD_EBONY);

		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Matherial3) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");

				dialog.text = "Everything, down to the last plank. Come back in a couple of months - the hull will not take the water sooner.";
				SaveCurrentQuestDateParam("pchar.questTemp.buildSoley");
				NextDiag.TempNode = "First time";
				AddQuestRecord("Fra_Line_13_BuildSoley", "2");
			    link.l1 = "I will wait.";
			    link.l1.go = "exit";

			}
			else
			{
				dialog.Text = "You have still to bring: mahogany - "+ sti(NPChar.Tuning.Matherial2) + ", ebony - "+ sti(NPChar.Tuning.Matherial3) + ", silk - "+ sti(NPChar.Tuning.Matherial1) + ".";
				link.l1 = "Very well.";
				link.l1.go = "Exit";
			}
		break;

		case "get_ship":
			// Gregg: без свободного места в эскадре SetCompanionIndex молча вернёт -1, и оплаченный
			// корабль пропадёт. Слот 0 занят самим ГГ, потому сравнение именно с COMPANION_MAX.
			if (GetCompanionQuantity(pchar) >= COMPANION_MAX)
			{
				dialog.text = "She is ready and lying in the roads. Only there is no one to take her - your squadron has not a single free berth. Free one and come back.";
				link.l1 = "I will see to it.";
				link.l1.go = "Exit";
				break;
			}
			dialog.text = "Indeed she is. We even tried her out on a pirate squadron. I confess I envy you - there is no other hull like her in these waters. She already flies your colours, and take the officer with her: he sailed her from the yard and knows her better than the men who built her.";
			NextDiag.TempNode = "First time";
			npchar.quest.Gave_Ship = "true";
			AddQuestRecord("Fra_Line_13_BuildSoley", "3");
			CloseQuestHeader("Fra_Line_13_BuildSoley");
			sld = GetCharacter(NPC_GenerateCharacter("SoleiRoyalCaptain1", "off_fra_2", "man", "man", 35, FRANCE, -1, true));
			sld.Ship.Name = "Soleil Royal";
			sld.Ship.Type = GenerateShipExt(SHIP_SOLEYRU, true, sld);
			ResetShipCannonsDamages(sld);
			SetBaseShipData(sld);
			SetCrewQuantityFull(sld);
			SetShipSkill(sld, 50, 60, 70, 55, 95, 55, 50, 55, 50);
			Fantom_SetBalls(sld, "pirate");
			FantomMakeCoolFighter(sld, 35, 80, 70, BLADE_LONG, "pistol3", 100);

			sld.Dialog.TempNode = "hired";
			sld.Dialog.FileName = "Enc_Officer_dialog.c";
			sld.quest.meeting = true;
			SetBaseShipData(sld);
			sld.CompanionEnemyEnable = false;
			SetCompanionIndex(pchar, -1, sti(sld.index));

			// Gregg: SetBaseShipData выше зовёт SetGoodsInitNull и набивает трюм на своё усмотрение,
			// поэтому команду и припасы королевского мановара выставляем строго после него.
			sld.Ship.Crew.Exp.Sailors   = 95;	// порог "морских волков" в GetExpName() - 90
			sld.Ship.Crew.Exp.Cannoners = 95;
			sld.Ship.Crew.Exp.Soldiers  = 95;
			sld.Ship.Crew.Morale = MORALE_MAX;
			SetCharacterGoods(sld, GOOD_BALLS,      1000);
			SetCharacterGoods(sld, GOOD_GRAPES,     1000);
			SetCharacterGoods(sld, GOOD_KNIPPELS,   1000);
			SetCharacterGoods(sld, GOOD_BOMBS,      1000);
			SetCharacterGoods(sld, GOOD_POWDER,     4000);
			SetCharacterGoods(sld, GOOD_FOOD,       2000);
			SetCharacterGoods(sld, GOOD_WEAPON,     1500);
			SetCharacterGoods(sld, GOOD_MEDICAMENT, 3000);
			SetCharacterGoods(sld, GOOD_RUM,        1000);

			sld.quest.OfficerPrice = sti(pchar.rank)*500;
			Pchar.questTemp.HiringOfficerIDX = GetCharacterIndex(sld.id);
			sld.OfficerWantToGo.DontGo = true; //не пытаться уйти
			sld.loyality = MAX_LOYALITY;


			link.l1 = "Thank you. I will test it immediately.";
			link.l1.go = "Exit";
		break;

		case "node_1":
			dialog.text = "So what reason made you come here and distract me from important state affairs?";
			link.l1 = "I want to talk about working for the crown of " + NationNameGenitive(sti(NPChar.nation));
			link.l1.go = "work";
			link.l2 = "I need to talk to you about something.";
			link.l2.go = "quests";
			link.l10 = "This is just a courtesy call and nothing more, "+GetAddress_FormToNPC(NPChar);
			link.l10.go = "node_2";
		break;

		case "node_2":
			dialog.text = "In that case, I will ask you to leave my office and not interfere with my work.";
			link.l1 = "Yes, yes, of course, sorry to bother you.";
			link.l1.go = "exit";
		break;

		case "work":
            if (!CheckCharacterItem(Pchar, "patent_" + NationShortName(sti(NPChar.nation))) || !CheckAttribute(pchar, "EquipedPatentId"))
			{
    			dialog.text = "I don't know you at all, " + GetAddress_Form(NPChar) + ". Leave my office. I only deal with people who are in the service of " + NationKingsName(npchar) + ".";
    			link.l1 = "Sorry to bother you. When will I have a patent "+NationNameGenitive(sti(NPChar.nation))+"I'll be back.";
    			link.l1.go = "exit";
			}
			else
			{// даём задание
                // звание подрасло
                if (isReadyToNextTitle(sti(NPChar.nation)))
                {
                    Items[sti(pchar.EquipedPatentId)].TitulCurNext = 0;
                    Items[sti(pchar.EquipedPatentId)].TitulCur = sti(Items[sti(pchar.EquipedPatentId)].TitulCur) + 1;

                    dialog.text = "I must make you happy"+ ", " + GetAddress_Form(NPChar)+". The application for your promotion has been signed in the office of " + NationKingsName(npchar) +
                                ". You have been awarded the title " + GetAddress_FormTitle(sti(NPChar.nation), sti(Items[sti(pchar.EquipedPatentId)].TitulCur)) +
                                " of the Navy " + NationNameGenitive(sti(NPChar.nation)) +".";
        			link.l1 = "Thank you! I promise to continue to serve with honor "+NationKingsCrown(npchar) + ".";
        			link.l1.go = "exit";
        			break
                }
                // даём задание
                dialog.text = LinkRandPhrase("Do you have any questions for me, "+GetAddress_FormTitle(sti(NPChar.nation), sti(Items[sti(pchar.EquipedPatentId)].TitulCur))+"?",
                                             "I'm listening to you, " + GetAddress_Form(NPChar)+".",
                                             "Speak up, I'm all ears.");

				if (pchar.questTemp.NationQuest == "")
                {
	    			if (pchar.questTemp.piratesLine == "begin" || pchar.questTemp.piratesLine == "over" || pchar.questTemp.piratesLine == "waiting_Q8" || pchar.questTemp.LSC == "over")
					{
						link.l1 = "Is there any job for a patent corsair?";
	    				link.l1.go = "work_1";
						if (!bWorldAlivePause && pchar.questTemp.State == "") //eddy. только, если не проходил линейку.
						{
							link.l1.go = "onlyGenWork_Alter";
						}
					}
					else
					{
						link.l1 = "Is there any work for a patent corsair?";
	    				link.l1.go = "onlyGenWork_2";
					}
    			}
    			else
    			{
    			    if (pchar.questTemp.NationQuest == NPChar.nation)
	                {
		    			link.l1 = RandPhraseSimple("I arrived on a case directly related to your assignments.", "About your assignments...");
		    			link.l1.go = "work_1";
	    			}
	    			else
	    			{// линейкам - фиг
	    			    link.l1 = "Is there any job for a patent corsair?";
	    				link.l1.go = "onlyGenWork_1";
	    			}
    			}

                // Сдача города -->
				if (isHeroOwnCity(false))
				{
				    Link.l2 = "I want to transfer the settlement I captured to the Crown of "+ XI_ConvertString(GetNationNameByType(sti(NPChar.nation)) + "Gen") +".";
					Link.l2.go = "colony_capture";
				}
                // Сдача города <--
                link.l9 = RandPhraseSimple("Sorry, I'll come back later.",
                                           "Nothing serious, "+GetAddress_FormToNPC(NPChar)+". Just a courtesy call.");
    			link.l9.go = "exit";
			}
		break;

		case "onlyGenWork_1":
			dialog.text = "Hmm... I heard that you were doing secret assignments for "+ NationNameGenitive(sti(pchar.questTemp.NationQuest)) +", therefore, I will not be able to entrust you with anything secret anymore.";
			link.l1 = "Very sorry.";
			link.l1.go = "exit";
		break;

		case "onlyGenWork_2":
			dialog.text = "There are rumors that you are very closely associated with the leaders of the Brethren. Therefore, I do not think that I will be able to entrust you with work for the benefit of " + NationNameGenitive(sti(npchar.nation)) + ".";
			link.l1 = "Hmm... Well, it's a pity.";
			link.l1.go = "exit";
		break;

		case "onlyGenWork_Alter":
   			dialog.text = "Unfortunately for you and fortunately for me, I don't have any risky and highly paid corsair assignments right now.";
			link.l1 = "Well, I'll come back later.";
			link.l1.go = "exit";
		break;

		/////Награда за захваченную колонию.
		case "colony_capture":
   			dialog.text = "What city are we talking about?";
   			for (i=0; i<MAX_COLONIES; i++)
			{
				if (sti(colonies[i].HeroOwn) == true && colonies[i].id != "Caiman")
				{
					sld = GetFortCommander(colonies[i].id);
					attrLoc = "l" + i;
					Link.(attrLoc) = GetCityName(colonies[i].id);
					Link.(attrLoc).go = "CityGive_" + i;
				}
			}
			link.l99 = "I'm wrong, it was about something else.";
			link.l99.go = "exit";
		break;

        case "ColonyGive":
            i = sti(NPChar.quest.CityIdx);
            sld = GetFortCommander(colonies[i].id);
            colony_money = TWN_CityCost(colonies[i].id) / 50;  // те *1000 в итоге
            TWN_RealeseForMoney(colonies[i].id, false);
            dialog.text = "Great news for you, "+GetAddress_FormTitle(sti(NPChar.nation), sti(Items[sti(pchar.EquipedPatentId)].TitulCur))+
                          "! Your diligence will not go unnoticed, and of course you will be rewarded. Here you are " + colony_money +
                          " gold as a reward for your services to "+NationNameAblative(sti(NPChar.nation))+
                          ". Now if you'll excuse me, I have a lot to do.";
            link.l1 = "In that case, " + GetAddress_FormToNPC(NPChar) + ", I dare not detain you any longer. All the best.";
            link.l1.go = "exit";

            ReOpenQuestHeader("Gen_CityCapture");
		    AddQuestRecord("Gen_CityCapture", "t8");
			AddQuestUserData("Gen_CityCapture", "sSex", GetSexPhrase("cя","ась"));
			AddQuestUserData("Gen_CityCapture", "sCity", GetCityName(colonies[i].id));
			AddQuestUserData("Gen_CityCapture", "sNation", XI_ConvertString(GetNationNameByType(sti(NPChar.nation)) + "Gen"));

            AddMoneyToCharacter(PChar, colony_money);
            AddTitleNextRate(sti(NPChar.nation), 1);  // счётчик звания
            ChangeCharacterNationReputation(Pchar, sti(NPChar.nation), 10);
            PChar.questTemp.DontNullDeposit = true;    // чтоб не нулили ростовщика
            SetCaptureTownByNation(colonies[i].id, sti(NPChar.nation));
        break;

		//*************************** Квест "Orion fluyt accompaniment"Орион"" **********************
        case "Andre_Abel_Quest_Hol_Gov_Node_1":
			//if (GetCityNameByIsland(Pchar.curIslandId) == "Villemstad")
			if (pchar.location == "Villemstad_townhall")
			{
			dialog.text = "I'm listening to you, Captain.";
			link.l1 = "I have arrived from Henry Morgan with instructions to give you this letter...";
			link.l1.go = "Andre_Abel_Quest_Hol_Gov_Node_2";
			TakeItemFromCharacter(PChar, "letter_1");
			Log_TestInfo("The letter has been given to the Governor-General of Curacao.");
			PChar.Quest.Andre_Abel_Quest_Curasao_10Days_Left.over = "yes";
			}
			else
			{
			dialog.text = "Are you interrupting me from important state affairs? Whatever you want, " +GetAddress_Form(NPChar)+"?";
			link.l1 = "This is just a courtesy call and nothing more, "+GetAddress_FormToNPC(NPChar)".";
			link.l1.go = "exit";
			NextDiag.TempNode = "Andre_Abel_Quest_Hol_Gov_Node_1";
			}
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_2":
			dialog.text = "Hmm... from Morgan? However... Well, let's have your letter from Henry Morgan.";
			link.l1 = "Please...";
			link.l1.go = "Andre_Abel_Quest_Hol_Gov_Node_3";
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_3":
			dialog.text = "What?! Leave Fort Orange?! Ah, English dogs! It's not enough for them to Port Royal, so they gaped at the whole island! Grab "+ GetSexPhrase("this scoundrel","this scoundrel") +"!";
			link.l1 = "Mynheer! I'm just "+ GetSexPhrase("messenger","messenger") +", and I can't be responsible for the contents of the letter!..";
			link.l1.go = "Andre_Abel_Quest_Hol_Gov_Node_4";
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_4":
			dialog.text = "You are Morgan's man, and you are "+ GetSexPhrase("Englishman","Englishwoman") +". That's enough!.. Take "+ GetSexPhrase("him","her") +" to the casemate!";
			link.l1 = "Yes, you need to treat your nerves!!!";
			link.l1.go = "Andre_Abel_Quest_Hol_Gov_Node_5";
			link.l2 = "Well, I will obey the authorities! But I repeat, my conscience is clear, and I did not commit and did not plan nothing against Holland for which I could be persecuted!";
			link.l2.go = "Andre_Abel_Quest_Hol_Gov_Node_7";
			link.l3 = "Mr. Governor-General, why go to jail right away?! I am sure that we could have a good agreement...";
			link.l3.go = "Andre_Abel_Quest_Hol_Gov_Node_9";
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_5":
			dialog.text = "How dare you, scoundrel! Kill "+ GetSexPhrase("him","her") +", immediately!!!";
			link.l1 = "Well, guards, hold on!..";
			link.l1.go = "Andre_Abel_Quest_Hol_Gov_Node_6";
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_6":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_SetFightMode(PChar, true);
			Flag_Change(PIRATE);
			LAi_SetImmortal(CharacterFromID("Andre_Abel_Quest_Officer_1"), false);
			sTemp = "hol_guard";
			LAi_group_MoveCharacter(CharacterFromID("Andre_Abel_Quest_Officer_1"), sTemp);
			LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups(sTemp, LAI_GROUP_PLAYER, true);
			QuestSetCurrentNode("Henry Morgan", "Andre_Abel_Quest_Morgan_Dialog_16");
			LAi_Group_Attack(PChar, CharacterFromID("hol_guber"));
			AddQuestRecord("Andre_Abel_Quest", "16");
			AddQuestUserData("Andre_Abel_Quest", "sText", "Fortunately, I was once again saved by the ability to fence well");
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_7":
			dialog.text = "Whether your conscience is clear or not, we'll find out later... Take "+ GetSexPhrase("him","her") +" away!";
			link.l1 = "Find out, just don't pull. I don't have much time...";
			link.l1.go = "Andre_Abel_Quest_Hol_Gov_Node_8";
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_8":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_LocationDisableOfficersGen("Villemstad_prison",true);
			LAi_LocationDisableOfficersGen("Villemstad_fort",true);
			LAi_LocationDisableOfficersGen("Villemstad_ExitTown",true);
			DoFunctionReloadToLocation("Villemstad_prison", "goto", "goto9", "Andre_Abel_Quest_In_Prison");
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_9":
			iSumm = sti(PChar.rank)*20000;
			dialog.text = "Agree? Hmm... well, if we are talking about the amount of " + iSumm + " piastres, then I am ready to continue the dialogue.";
			if(sti(PChar.money) >= iSumm)
			{
				link.l1 = "Great! I told you it would be easy.";
				link.l1.go = "Andre_Abel_Quest_Hol_Gov_Node_10";
			}
			link.l2 = "What?! Are you out of your mind?! For such money, I'm not like your guards!.. I will cut out the whole of Curacao, along with the harbor horses and monkeys in the jungle!!!";
			link.l2.go = "Andre_Abel_Quest_Hol_Gov_Node_5";
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_10":
			iSumm = sti(PChar.rank)*20000;
			AddMoneyToCharacter(PChar, -iSumm);
			dialog.text = "Well, that's great. You can go now... Just don't come across me with such errands anymore! My generosity is not infinite.";
			link.l1 = "Of course! Have a nice stay...";
			link.l1.go = "Andre_Abel_Quest_Hol_Gov_Node_11";
		break;

		case "Andre_Abel_Quest_Hol_Gov_Node_11":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			QuestSetCurrentNode("Henry Morgan", "Andre_Abel_Quest_Morgan_Dialog_16");
			AddQuestRecord("Andre_Abel_Quest", "16");
			AddQuestUserData("Andre_Abel_Quest", "sText", "Fortunately, I was once again bailed out by money");
		break;
		case "GivePatent":
			dialog.text = "Oh, there you are, " + GetFullName(pchar) + ". I've been waiting for you!";
			link.l1 = "Greetings, dear " + GetFullName(npchar) + ". Thank you for such a warm welcome. What do I owe the honor?";
			link.l1.go = "Give Money";
			npchar.quest.meeting = "1";//флаг знакомства с губером
		break;
		case "Give Money":
			dialog.text = "You have worked very hard for the benefit of our state. I want to invite you to become a corsair in the service of " + NationNameGenitive(sti(NPChar.nation))+".";
			link.l1 = "I will gladly accept your offer.";
			link.l2 = "Sorry, but I have to refuse."
			link.l1.go = "Accept";
			link.l2.go = "Refuse";
		break;
		case "Accept":
			dialog.text = "Great! Then here is your corsair patent, I hope your service will bring even more victories and glory!";
			link.l1 = "";
			link.l1.go = "Exit";
			pchar.PatentNation = NationShortName(sti(npchar.nation));
			GiveItem2Character(pchar, "patent_" + pchar.PatentNation);
			EquipCharacterbyItem(pchar, "patent_" + pchar.PatentNation);
			if(CheckAttribute(npchar,"patent.othertime")) DeleteAttribute(npchar,"patent.othertime"));
		break;
		case "Refuse":
			dialog.text = "What a pity, what a pity... But that's okay, I'm sure you'll find yourself in another business! But you can come to me again if you change your mind, remember that.";
			link.l1 = "Thank you, I will keep this opportunity in mind.";
			link.l1.go = "Exit";
			npchar.patent.othertime = true;
		break;
		case "Other time patent":
			dialog.text = "Oh, you're back. Have you decided that you still want to get a patent?";
			link.l1 = "Yes, I have decided to accept your offer.";
			link.l2 = "I'll still think about it, goodbye.";
			link.l1.go = "Accept";
			link.l2.go = "Refuse";
		break;
	}
}

void checkSoleyMatherial(ref Pchar, ref NPChar, int good1, int good2, int good3)
{
    int amount;

    amount = GetSquadronGoods(Pchar, good1) - sti(NPChar.Tuning.Matherial1);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Matherial1);
    }
    else
    {
        amount = sti(NPChar.Tuning.Matherial1);
    }
    RemoveCharacterGoods(Pchar, good1, amount);
    NPChar.Tuning.Matherial1 = sti(NPChar.Tuning.Matherial1) - amount;

    amount = GetSquadronGoods(Pchar, good2) - sti(NPChar.Tuning.Matherial2);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Matherial2);
    }
    else
    {
        amount = sti(NPChar.Tuning.Matherial2);
    }
    RemoveCharacterGoods(Pchar, good2, amount);
    NPChar.Tuning.Matherial2 = sti(NPChar.Tuning.Matherial2) - amount;


    amount = GetSquadronGoods(Pchar, good3) - sti(NPChar.Tuning.Matherial3);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Matherial3);
    }
    else
    {
        amount = sti(NPChar.Tuning.Matherial3);
    }
    RemoveCharacterGoods(Pchar, good3, amount);
    NPChar.Tuning.Matherial3 = sti(NPChar.Tuning.Matherial3) - amount;

}
