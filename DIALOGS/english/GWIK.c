
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	int price = 50000;
	if (CheckOfficersPerk(pchar, "Trader")) price = 10000;
	int uniDays, uniPrice;
	int uniPerDay = 1000 + sti(pchar.rank) * 50; // price per day: 1000 + hero level * 50

	// price report subscription: a month with delivery every 5 days
	int subPrice = 80000;
	if (CheckOfficersPerk(pchar,"Trader")) subPrice = 20000;

	// bill of exchange service
	int billFee = 5; // commission, %
	if (CheckOfficersPerk(pchar,"Trader")) billFee = 3;
	int billSum, billPay, cash;

	// contract and auction
	int iGood, cQty, cPrice, cCargo, sell, aQty, aPrice, toBuy, added, cn, i, iClass;
	ref rItem;
	string sItemName;
	string sNode = "";
	string sPartnerId = GWIK_PartnerId(NPChar.id); // partner office (mail/delivery) - next open office in the ring
	string sPartner = "on Curacao";
	if (sPartnerId == "GVIK_char") sPartner = "on Saint Martin";
	if (sPartnerId == "FortOrange_GWIK_char") sPartner = "on Jamaica";

	// Company shareholding: shareholder discounts on office services
	int shareLvl = 0;
	if (CheckAttribute(pchar,"GWIK.Share.Level")) shareLvl = sti(pchar.GWIK.Share.Level);
	int shareDisc = 0;
	if (shareLvl == 1) shareDisc = 10;
	if (shareLvl == 2) shareDisc = 25;
	if (shareLvl == 3) shareDisc = 50;
	int shareCost = 100000; // price of the next share level
	if (shareLvl == 1) shareCost = 250000;
	if (shareLvl == 2) shareCost = 500000;

	// Company assignments
	int iJob;
	int jobMailPay = 8000 + sti(pchar.rank) * 800;
	int jobRansom = 20000 + sti(pchar.rank) * 1000;
	int jobRansomPay = 8000 + sti(pchar.rank) * 800;
	int jobRivalPay = 25000 + sti(pchar.rank) * 1500;
	int jobConvoyBonus = 15000 + sti(pchar.rank) * 1000;
	string sJobCity;
	bool ok, bPrimary, bExtra, bElite, bMap, bTube, bTreasure, bMutiny, bDodged;
	if (shareDisc > 0)
	{
		price = makeint(price * (100 - shareDisc) / 100);
		subPrice = makeint(subPrice * (100 - shareDisc) / 100);
		uniPerDay = makeint(uniPerDay * (100 - shareDisc) / 100);
		if (shareLvl >= 2 && billFee > 1) billFee = billFee - 1;
		if (shareLvl >= 3 && billFee > 1) billFee = billFee - 1;
	}
	// dividends and assignments grow with the share level
	int shareRate = 2;                  // % dividends a month: shareholder
	if (shareLvl == 2) shareRate = 3;   // partner
	if (shareLvl >= 3) shareRate = 4;   // associate
	int jobCooldown = 4 - shareLvl;     // assignment cooldown: 4 / 3 / 2 / 1 days
	if (shareLvl > 0)                   // +15% to Company rewards per share level
	{
		jobMailPay     = makeint(jobMailPay     * (100 + shareLvl * 15) / 100);
		jobRansomPay   = makeint(jobRansomPay   * (100 + shareLvl * 15) / 100);
		jobRivalPay    = makeint(jobRivalPay    * (100 + shareLvl * 15) / 100);
		jobConvoyBonus = makeint(jobConvoyBonus * (100 + shareLvl * 15) / 100);
	}

	// Gregg: trophy-ship pick dispatcher (node name TrophySell_<companion index> -> shared node Trophy_Do)
	sNode = Dialog.CurrentNode;
	if (findsubstr(sNode, "TrophySell_", 0) != -1)
	{
		i = findsubstr(sNode, "_", 0);
		pchar.GWIK.TrophyPick = strcut(sNode, i + 1, strlen(sNode) - 1);
		Dialog.CurrentNode = "Trophy_Do";
	}
	// Gregg: the "unexpected offer" - when a high-standing Companion asks for a job, preempt the rotation once with the Jamaica office quest
	if (sNode == "Job_Ask" && GWIK_JamaicaEligible() && !CheckAttribute(pchar, "GWIK.Jamaica.Offered"))
	{
		pchar.GWIK.Jamaica.Offered = true;
		Dialog.CurrentNode = "Jamaica_Offer";
	}

	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Greetings. I am " + GetFullName(NPChar) + ", a representative of the office of the Dutch West India Trading Company on the island of Saint Martin. Please introduce yourself.";
			if (NPChar.id == "GWIK_char") dialog.text = "Greetings. I am " + GetFullName(NPChar) + ", a representative of the office of the Dutch West India Trading Company on the island of Curacao. Please introduce yourself.";
			if (NPChar.id == "FortOrange_GWIK_char") // Gregg: the manager is the very factor the player delivered; remembers the voyage and how the English were dealt with
			{
				dialog.text = "Captain "+ GetFullName(PChar) +"! So this is the man the Company owes this office to. I am "+ GetFullName(NPChar) +", the manager here. Everything is already running: price lists, bills of exchange, confiscated goods, assignments - just as in Willemstad, and for the founder, of the first order.";
				if (CheckAttribute(pchar, "GWIK.Jamaica.Founder"))
				{
					dialog.text = "Captain "+ GetFullName(PChar) +"! You I will never forget - I sailed to this very office aboard your ship. Now I manage it, and for you, as the founder, everything is of the first order: price lists, bills of exchange, confiscated goods, assignments.";
					if (CheckAttribute(pchar, "GWIK.Jamaica.Tone"))
					{
						if (pchar.GWIK.Jamaica.Tone == "force") dialog.text = "Captain "+ GetFullName(PChar) +"! You I will never forget - I sailed to this very office aboard your ship. The port still whispers about Gale's 'Greyhound' - after that lesson the English keep well clear of us. For the founder everything is of the first order: price lists, bills, confiscated goods, assignments.";
						if (pchar.GWIK.Jamaica.Tone == "gold") dialog.text = "Captain "+ GetFullName(PChar) +"! You I will never forget - I sailed to this very office aboard your ship. The matter was settled quietly back then: the Company's gold knows how to keep silent, and the English look the other way. For the founder everything is of the first order: price lists, bills, confiscated goods, assignments.";
						if (pchar.GWIK.Jamaica.Tone == "word") dialog.text = "Captain "+ GetFullName(PChar) +"! You I will never forget - I sailed to this very office aboard your ship. The office opened without a single shot - your name proved stronger than cannon. For the founder everything is of the first order: price lists, bills, confiscated goods, assignments.";
					}
				}
			}
			Link.l1 = "Hello, " + GetFullName(NPChar) + ". I am " + GetFullName(PChar) + ", captain of the ship '" + PChar.ship.name + "'.";
			link.l1.go = "Meet_2";
			if (NPChar.id == "FortOrange_GWIK_char") // the founder - the manager already knows the player, special reply
			{
				Link.l1 = "Good to see Fort Orange under the Company's flag.";
				link.l1.go = "Meet_2";
			}
		break;

		case "Exit":
			NextDiag.CurrentNode = "Meet_2";
			DialogExit();
		break;

		case "Meet_2":
			GWIK_CheckContract(NPChar);
			GWIK_CheckAuction(NPChar);
			// safety: if the quest timer was lost (old save), re-arm it
			if (shareLvl > 0 && !CheckAttribute(pchar, "quest.GWIK_DividendTick")) SetTimerFunction("GWIK_DividendTick", 0, 1, 0);
			if (CheckAttribute(pchar, "GWIK.Subscription") && !CheckAttribute(pchar, "quest.GWIK_SubscriptionTick")) SetTimerFunction("GWIK_SubscriptionTick", 0, 0, 5);
			if (!CheckAttribute(npchar, "met")) dialog.text = "Nice to meet you, Captain. What's the occasion?";
			else dialog.text = "What's the occasion, Captain?";
			if (NPChar.id == "FortOrange_GWIK_char" && CheckAttribute(pchar, "GWIK.Jamaica.Founder")) dialog.text = "Always glad to see you, Captain. What's the occasion?"; // Gregg: the manager-factor knows the player well - "nice to meet you" would be off
			if (!CheckAttribute(pchar, "GwikTimer") && !CheckAttribute(pchar,"GWIK.Subscription"))
			{
				Link.l1 = "I would like to purchase price information from you in stores across the archipelago.";
				link.l1.go = "trade_info";
			}
			if (!CheckAttribute(pchar,"GWIK.Subscription"))
			{
				Link.l4 = "Is there a way to receive your price reports regularly, by courier?";
				link.l4.go = "Subscribe_Ask";
			}
			else
			{
				Link.l4 = "I'd like to extend my price report subscription. (days left: " + pchar.GWIK.Subscription + ")";
				link.l4.go = "Subscribe_Ask";
			}
			Link.l2 = "I'd like to buy a universal trade license.";
			link.l2.go = "UniLicense_Ask";
			Link.l5 = "What goods is the Company buying at the moment?";
			link.l5.go = "Contract_Info";
			Link.l6 = "What's on your confiscated goods sale today?";
			link.l6.go = "Auction_Info";
			Link.l7 = "I have business on the money side.";
			link.l7.go = "GWIK_Finance";
			Link.l10 = "Does the Company have any assignments for me?";
			link.l10.go = "Job_Ask";
			if (CheckAttribute(pchar, "GWIK.Job"))
			{
				if (pchar.GWIK.Job == "mail" && pchar.GWIK.Job.Target == NPChar.id)
				{
					Link.l11 = "I have a parcel from your partner office.";
					link.l11.go = "Job_MailDone";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.Rescued"))
				{
					Link.l11 = "I've brought your captain, ransomed from the pirates.";
					if (CheckAttribute(pchar, "GWIK.Job.Mutiny")) Link.l11 = "I've brought your runaway captain - by force, in irons."; // Gregg: mutiny - not ransomed, taken by boarding
					link.l11.go = "Job_RansomDone";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.CaptiveDeadKnown"))
				{
					Link.l11 = "Grim news: the pirates killed your captain, I couldn't save him.";
					link.l11.go = "GWIK_RansomDeadReport";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.MutinyReturnedSelf"))
				{
					Link.l11 = "Your runaway captain saw sense - I talked him into returning; he's already on his way to you.";
					link.l11.go = "GWIK_MutinyReturnReport";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.MutinyReportReady"))
				{
					Link.l11 = "Grim news: your captain mutinied and went over to the pirates.";
					link.l11.go = "GWIK_MutinyReport";
				}
				if (pchar.GWIK.Job == "ransom" && CheckAttribute(pchar, "GWIK.Job.MutinyKilled"))
				{
					Link.l11 = "Your runaway captain resisted capture - I had to cut him down.";
					link.l11.go = "GWIK_MutineerKilledReport";
				}
				if (pchar.GWIK.Job == "rival" && CheckAttribute(pchar, "GWIK.Job.Done"))
				{
					Link.l11 = "Your rivals' ship is no longer a problem.";
					if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) Link.l11 = "Your rivals' man is no longer a problem."; // Gregg: land variant - killed ashore, there was no ship
					link.l11.go = "Job_RivalDone";
				}
			}
			if (CheckAttribute(pchar, "GWIK.ConvoyBonus.Done"))
			{
				Link.l12 = "The Company's ship has reached its destination. I'm here for the bonus.";
				link.l12.go = "Job_ConvoyBonus";
			}
			if (GWIK_RepLevel() >= 2)
			{
				Link.l15 = "Might the Company want a captured ship from my squadron?";
				link.l15.go = "Trophy_Ask";
			}
			Link.l14 = "How does the Company regard our dealings?";
			link.l14.go = "GWIK_Standing";
				if (NPChar.id == "GWIK_char" && CheckAttribute(pchar,"GWIK.Jamaica.Stage") && sti(pchar.GWIK.Jamaica.Stage) == 3 && !CheckAttribute(pchar,"GWIK.Jamaica.Convoy") && !CheckAttribute(pchar,"GWIK.Jamaica.ConvoyArrived")) // Gregg: take the supply convoy under escort (stage 3) - loaded ONLY at Curacao
				{
					Link.l20 = "I'm here for the convoy for the Jamaica office.";
					link.l20.go = "Jamaica_Convoy";
				}
				if (GWIK_JamaicaEligible() && CheckAttribute(pchar, "GWIK.Jamaica.Offered")) // Gregg: return to the postponed Jamaica office offer
				{
					Link.l18 = "I've thought over your offer about the Jamaica office.";
					link.l18.go = "Jamaica_Offer";
				}
			if (NPChar.id == "GVIK_char" && CheckAttribute(pchar,"GWIK.Jamaica.Stage") && sti(pchar.GWIK.Jamaica.Stage) == 4 && !CheckAttribute(pchar,"GWIK.Jamaica.FactorAboard")) // Gregg: collect the factor for the Jamaica office (stage 4) - he waits ONLY in Marigo
				{
					Link.l19 = "I'm here for the factor for the Jamaica office.";
					link.l19.go = "Jamaica_Factor";
				}
				if (bBettaTestMode)
			{
				Link.l16 = "[BETA] GWIC test menu.";
				link.l16.go = "GWIK_BetaMenu";
			}
			Link.l3 = "Dropped by to say hello, have a nice day.";
			link.l3.go = "exit";
			npchar.met = 1;
		break;

		case "GWIK_Finance": // Gregg: office money-services sub-branch (share + bill) - grouped out of Meet_2 to declutter the main menu
			dialog.text = "How can I help you on the money side, Captain?";
			if (shareLvl == 0)
			{
				Link.l1 = "Could I invest in the Company's business?";
				link.l1.go = "Share_Ask";
			}
			if (shareLvl > 0 && shareLvl < 3)
			{
				Link.l1 = "I'd like to increase my share in the Company.";
				link.l1.go = "Share_Ask";
			}
			if (shareLvl > 0)
			{
				Link.l2 = "I'd like to invest more into my share.";
				link.l2.go = "Share_Topup";
			}
			if (shareLvl > 0)
			{
				Link.l3 = "I'm here about my dividends.";
				link.l3.go = "Share_Dividends";
			}
			// Gregg: the bill goes last - share and dividends are the standing business with the Company, a bill is a one-off service
			if (!CheckAttribute(pchar,"GWIK.Bill"))
			{
				Link.l4 = "I'd like to draw a bill of exchange for your partner office.";
				link.l4.go = "Bill_Ask";
			}
			else
			{
				if (pchar.GWIK.Bill.From != NPChar.id)
				{
					Link.l4 = "I have a bill of exchange of your Company to cash.";
					link.l4.go = "Bill_Cash";
				}
				else
				{
					Link.l4 = "Remind me, where can I cash my bill of exchange?";
					link.l4.go = "Bill_Where";
				}
			}
			Link.l5 = "Back to other matters.";
			link.l5.go = "Meet_2";
		break;

		case "GWIK_Standing": // Gregg: player's standing with the Company
			iJob = GWIK_RepLevel(); // reuse iJob as the reputation level
			sJobCity = "a casual client";
			if (iJob == 1) sJobCity = "a Company acquaintance";
			if (iJob == 2) sJobCity = "a trusted hand";
			if (iJob == 3) sJobCity = "a friend of the Company";
			if (iJob == 4) sJobCity = "a pillar of the Company";
			dialog.text = "In our books you stand as "+ sJobCity +". The Company values proven men: the higher your name with us, the more doors open - special lots, buying up your prizes, assignments of trust.";
			link.l1 = "Thank you. I'll keep at it.";
			link.l1.go = "Meet_2";
		break;

		case "GWIK_BetaRep": // [BETA] Gregg: instantly set reputation for the Jamaica quest gate (rep>=3)
				pchar.GWIK.Rep = 350; // level 3 threshold ("friend of the Company")
				if (!CheckAttribute(pchar, "GWIK.Share")) // Gregg: grant the Companion share too - quarterly cooldowns make it slow to grind by hand now
				{
					pchar.GWIK.Share.Pool = 0;
					pchar.GWIK.Share.Invested = 0;
					pchar.GWIK.Share.Date = GetQuestBookDataDigit();
					SetTimerFunction("GWIK_DividendTick", 0, 1, 0);
				}
				pchar.GWIK.Share.Level = 3;
				if (sti(pchar.GWIK.Share.Invested) < 850000) pchar.GWIK.Share.Invested = 850000; // nominal of the three instalments (100+250+500k)
				dialog.text = "[BETA] Granted: 'friend of the Company' reputation (350, level 3) and a Companion share (850000 invested). All that's left for the Jamaica quest is a Dutch Fort Orange.";
				link.l1 = "Understood.";
				link.l1.go = "GWIK_BetaMenu";
			break;

			case "GWIK_BetaOpenFO": // [BETA] Gregg: open the Fort Orange office without the quest (step 2 test)
				GWIK_OpenFortOrangeOffice();
				dialog.text = "[BETA] The Fort Orange office is open: location, door and representative are in place. Sail to Jamaica - the entrance is the house with door reload_H3.";
				link.l1 = "Let's sail.";
				link.l1.go = "GWIK_BetaMenu";
			break;

			case "GWIK_BetaMenu": // [BETA] Gregg: single GWIC test menu - quest conditions and launching generators in a chosen variant
				dialog.text = "[BETA] GWIC test menu. What to set up?";
				Link.l1 = "Jamaica quest conditions: 'friend of the Company' reputation + Companion share.";
				link.l1.go = "GWIK_BetaRep";
				Link.l2 = "Open the Fort Orange office (no quest).";
				link.l2.go = "GWIK_BetaOpenFO";
				Link.l3 = "Rival assignment: anchored off an island.";
				link.l3.go = "GWIK_BetaRivalStatic";
				Link.l4 = "Rival assignment: cruising at sea.";
				link.l4.go = "GWIK_BetaRivalMoving";
				Link.l5 = "Ransom assignment: mediator on Bermuda.";
				link.l5.go = "GWIK_BetaRansomAgent";
				Link.l6 = "Ransom assignment: captive aboard a pirate.";
				link.l6.go = "GWIK_BetaRansomPirate";
				Link.l7 = "Ransom assignment: captive killed (failure).";
				link.l7.go = "GWIK_BetaRansomDead";
				Link.l8 = "Mail assignment: hot parcel (intercepted on the way out).";
				link.l8.go = "GWIK_BetaMailHot";
				Link.l9 = "Escort assignment: with an ambush at the destination.";
				link.l9.go = "GWIK_BetaConvoyAmbush";
				Link.l10 = "Ransom assignment: the captain mutinied (turned pirate).";
					link.l10.go = "GWIK_BetaRansomMutiny";
				Link.l11 = "Rival assignment: ashore (a man in a tavern, duel).";
				link.l11.go = "GWIK_BetaRivalLand";
				Link.l12 = "Send the letter for the next rank (gift: chart / book / bonus).";
				link.l12.go = "GWIK_BetaRankLetter";
				Link.l13 = "Back.";
				link.l13.go = "Meet_2";
			break;

			case "GWIK_BetaRankLetter": // [BETA] Gregg: deliver the rank letter right now, skipping the reputation threshold and the three-day post. Each press sends the next tier; after the fourth the cycle restarts
				iJob = 0;
				if (CheckAttribute(pchar, "GWIK.RankLetter.Sent")) iJob = sti(pchar.GWIK.RankLetter.Sent);
				iJob = iJob + 1;
				if (iJob > 4) iJob = 1;
				pchar.GWIK.RankLetter.Sent = iJob - 1; // so the "already sent for this rank" guard lets it through
				pchar.GWIK.RankLetter.Pending = iJob;
				GWIK_RankLetter("beta");
				dialog.text = "[BETA] The letter for rank "+ iJob +" has been delivered. Check the ship's log and your inventory.";
				link.l1 = "Next.";
				link.l1.go = "GWIK_BetaMenu";
			break;

			case "GWIK_BetaRivalStatic": // [BETA] Gregg: give the rival assignment as the static variant (bypassing drand and the cooldown)
				GWIK_DropRivalJob(); // clear the current assignment and old ship
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRival = "static"; // read and cleared in GWIK_SpawnRival
				dialog.text = "[BETA] Variant 'anchored off an island' set. Issuing the assignment.";
				link.l1 = "Next.";
				link.l1.go = "Job_RivalGo";
			break;

			case "GWIK_BetaRivalMoving": // [BETA] Gregg: give the rival assignment as the moving variant (bypassing drand and the cooldown)
				GWIK_DropRivalJob();
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRival = "moving";
				dialog.text = "[BETA] Variant 'cruising at sea' set. Issuing the assignment.";
				link.l1 = "Next.";
				link.l1.go = "Job_RivalGo";
			break;

			case "GWIK_BetaRivalLand": // [BETA] Gregg: give the rival assignment as the land variant (bypassing drand and the cooldown)
				GWIK_DropRivalJob();
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRival = "land";
				dialog.text = "[BETA] The 'man in a tavern' variant is set. Issuing the assignment. The target will take his seat once you enter the colony's tavern.";
				link.l1 = "Go on.";
				link.l1.go = "Job_RivalGo";
			break;

			case "GWIK_BetaRansomAgent": // [BETA] Gregg: give the ransom job via the Bermuda mediator (bypassing drand and the cooldown)
				GWIK_FinishRansomJob(); // clear the current ransom (safe if none)
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRansom = "agent"; // read and cleared in Job_RansomGo
				dialog.text = "[BETA] Variant 'mediator on Bermuda' set. Issuing the assignment.";
				link.l1 = "Next.";
				link.l1.go = "Job_RansomGo";
			break;

			case "GWIK_BetaRansomPirate": // [BETA] Gregg: give the ransom job as 'captive aboard a cruising pirate' (bypassing drand and the cooldown)
				GWIK_FinishRansomJob();
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRansom = "pirate";
				dialog.text = "[BETA] Variant 'aboard a pirate' set. Issuing the assignment.";
				link.l1 = "Next.";
				link.l1.go = "Job_RansomGo";
			break;

			case "GWIK_BetaRansomDead": // [BETA] Gregg: give the ransom job with the 'captive killed' outcome (failure test, bypassing drand and the cooldown)
				GWIK_FinishRansomJob();
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRansom = "dead";
				dialog.text = "[BETA] Outcome 'captive killed' set. Issuing the assignment (you'll learn it from the mediator on Bermuda).";
				link.l1 = "Next.";
				link.l1.go = "Job_RansomGo";
			break;

			case "GWIK_BetaMailHot": // [BETA] Gregg: give the mail job as a 'hot parcel' (agent on the way out, bypassing drand and the cooldown)
				DeleteAttribute(pchar, "GWIK.Job");
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceMail = "hot"; // read and cleared in Job_MailGo
				dialog.text = "[BETA] 'Hot parcel' set. Issuing the mail job - the agent will intercept you on the way out.";
				link.l1 = "Next.";
				link.l1.go = "Job_MailGo";
			break;

			case "GWIK_BetaConvoyAmbush": // [BETA] Gregg: give the escort WITH an ambush off the destination (bypassing drand and the cooldown)
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceConvoy = "ambush"; // read and cleared in Job_ConvoyGo
				dialog.text = "[BETA] Ambush set. Issuing the escort - pirates wait off the destination island.";
				link.l1 = "Next.";
				link.l1.go = "Job_ConvoyGo";
			break;

			case "GWIK_BetaRansomMutiny": // [BETA] Gregg: give the ransom job with the 'captain mutinied' twist (bypassing drand and the cooldown)
				GWIK_FinishRansomJob(); // clear the current ransom (safe if none)
				DeleteAttribute(pchar, "GWIK.JobDate");
				DeleteAttribute(pchar, "GWIK.Job.Date");
				pchar.GWIK.BetaForceRansom = "mutiny"; // read and cleared in Job_RansomGo
				dialog.text = "[BETA] The 'captain mutinied' twist is set. Issuing the assignment (you'll learn it from the 'go-between' on Bermuda).";
				link.l1 = "Next.";
				link.l1.go = "Job_RansomGo";
			break;

			case "Trophy_Ask": // Gregg: Company buys a captured squadron ship (rep>=2)
			if (CheckAttribute(pchar, "GWIK.TrophyDate") && GetNpcQuestPastDayParam(pchar, "GWIK.TrophyDate") < sti(pchar.GWIK.TrophyCD))
			{
				dialog.text = "The Company has bought a ship from you recently, Captain. The coffers aren't bottomless - come by later.";
				link.l1 = "Understood. I'll return later.";
				link.l1.go = "Meet_2";
				break;
			}
			bMap = false; // reused: whether any eligible ship was found
			for (i = 1; i < COMPANION_MAX; i++)
			{
				cn = GetCompanionIndex(pchar, i);
				if (cn < 0) continue;
				rItem = GetCharacter(cn);
				if (!GetRemovable(rItem)) continue;
				if (!GetShipRemovable(rItem)) continue;
				aPrice = GWIK_TrophyPrice(rItem);
				sNode = "l" + i;
				link.(sNode) = XI_ConvertString(RealShips[sti(rItem.Ship.Type)].BaseName) + " (capt. " + GetFullName(rItem) + ") - " + FindRussianMoneyString(aPrice);
				link.(sNode).go = "TrophySell_" + cn;
				bMap = true;
			}
			if (!bMap)
			{
				dialog.text = "You have no ship in your squadron that the Company would take, Captain.";
				link.l1 = "I see.";
				link.l1.go = "Meet_2";
				break;
			}
			dialog.text = "The Company is willing to buy a captured ship - for part of its value, less expenses, of course. Which do you offer?";
			link.l99 = "I've changed my mind.";
			link.l99.go = "Meet_2";
		break;

		case "Trophy_Do": // Gregg: the deal itself (dispatcher target TrophySell_<idx>)
			cn = sti(pchar.GWIK.TrophyPick);
			if (cn < 0)
			{
				dialog.text = "Something's wrong with that ship, Captain. Let's start over.";
				link.l1 = "Alright.";
				link.l1.go = "Trophy_Ask";
				break;
			}
			rItem = GetCharacter(cn);
			if (!IsCompanion(rItem))
			{
				dialog.text = "That ship is no longer in your squadron, Captain.";
				link.l1 = "Hm, true enough.";
				link.l1.go = "Meet_2";
				break;
			}
			aPrice = GWIK_TrophyPrice(rItem);
			sItemName = XI_ConvertString(RealShips[sti(rItem.Ship.Type)].BaseName);
			iClass = GetCharacterShipClass(rItem); // Gregg: read the trophy class BEFORE the sale - GWIK_SellTrophyShip nulls the ship type (would otherwise return 7)
			if (iClass < 1) iClass = 1;
			if (iClass > 7) iClass = 7;
			if (!GWIK_SellTrophyShip(rItem))
			{
				dialog.text = XI_ConvertString("SeaTowOperationsDetach");
				link.l1 = XI_ConvertString("OK");
				link.l1.go = "Trophy_Ask";
				break;
			}
			AddMoneyToCharacter(pchar, aPrice);
			GWIK_AddRep(10);
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, (7 - iClass + 1) * 25); // Gregg: leadership & commerce xp for the deal; inverse of ship class (cl.1 -> 175, cl.7 -> 25)
			AddCharacterExpToSkill(pchar, SKILL_COMMERCE,   (7 - iClass + 1) * 25);
			pchar.GWIK.TrophyCD = 14 + drand(7); // cooldown 14..21 days (daily rng)
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.TrophyDate");
			PlayStereoSound("notebook");
			dialog.text = sItemName +" passes to the Company - and here are your "+ FindRussianMoneyString(aPrice) +". A pleasure doing business with you, Captain.";
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Share_Ask":
			if (shareLvl > 0 && CheckAttribute(pchar, "GWIK.Share.LevelDate") && GetNpcQuestPastDayParam(pchar, "GWIK.Share.LevelDate") < 90) // Gregg: expanding the share - no more than once a quarter (like the top-up)
			{
				dialog.text = "You expanded your share not long ago, Captain. The Company draws up a new share no more than once a quarter - look in later.";
				link.l1 = "Very well, I'll come back.";
				link.l1.go = "Meet_2";
				break;
			}
			if (shareLvl == 0)
			{
				dialog.text = "The Company is always glad to welcome reliable captains among its shareholders. A share costs "+ FindRussianMoneyString(shareCost) +" and grants a 10% discount on all services of our offices, plus monthly dividends - 2% of the invested sum. In time you will be able to increase your stake. Are you in?";
			}
			if (shareLvl == 1)
			{
				dialog.text = "Expanding your share to a partner's stake will cost another "+ FindRussianMoneyString(shareCost) +". A partner receives a 25% discount on office services, a reduced commission on bills of exchange and a second lot at every confiscated-goods sale. And the dividends rise to 3% a month - on a far larger sum. Shall we draw it up?";
			}
			if (shareLvl == 2)
			{
				dialog.text = "An associate's stake is another "+ FindRussianMoneyString(shareCost) +". In return - half prices on all office services, the lowest commission on bills of exchange, a full 4% a month in dividends on the entire invested sum and the best terms on our assignments. Deal?";
			}
			if (sti(pchar.money) >= shareCost)
			{
				link.l1 = "Yes, here's the money.";
				link.l1.go = "Share_Buy";
			}
			else
			{
				link.l1 = "Hm, I don't have that much on me...";
				link.l1.go = "Meet_2";
			}
			link.l2 = "No, I'm not ready to part with such a sum.";
			link.l2.go = "Meet_2";
		break;

		case "Share_Buy":
			if (shareLvl > 0 && CheckAttribute(pchar, "GWIK.Share.LevelDate") && GetNpcQuestPastDayParam(pchar, "GWIK.Share.LevelDate") < 90) // Gregg: cooldown safety
			{
				dialog.text = "Not so fast, Captain: the Company draws up a new share no more than once a quarter.";
				link.l1 = "Understood.";
				link.l1.go = "Meet_2";
				break;
			}
			if (sti(pchar.money) < shareCost)
			{
				dialog.text = "You don't have enough money, Captain.";
				link.l1 = "A pity.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -shareCost);
			if (shareLvl == 0)
			{
				pchar.GWIK.Share.Pool = 0;
				pchar.GWIK.Share.Invested = 0;
				pchar.GWIK.Share.Date = GetQuestBookDataDigit();
				SetTimerFunction("GWIK_DividendTick", 0, 1, 0);
			}
			pchar.GWIK.Share.Level = shareLvl + 1;
			pchar.GWIK.Share.Invested = sti(pchar.GWIK.Share.Invested) + shareCost;
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Share.LevelDate"); // Gregg: quarterly expansion cooldown starts now
			GWIK_AddRep(35); // Gregg: reputation for joining/expanding the share
			PlayStereoSound("notebook");
			if (shareLvl == 0)
			{
				dialog.text = "Welcome among the Company's shareholders! The papers are drawn up and entered into your ship's log. Dividends are credited once a month - you can collect them at any of our offices.";
			}
			else
			{
				dialog.text = "Congratulations on expanding your stake! The papers have been re-issued, the new terms apply from this very minute.";
			}
			link.l1 = "A pleasure doing business with you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Share_Dividends":
			cash = 0;
			if (CheckAttribute(pchar,"GWIK.Share.Pool")) cash = sti(pchar.GWIK.Share.Pool);
			if (cash > 0)
			{
				AddMoneyToCharacter(pchar, cash);
				pchar.GWIK.Share.Pool = 0;
				PlayStereoSound("notebook");
				dialog.text = "Since the last payout you have been credited "+ FindRussianMoneyString(cash) +". Here they are, every last coin. The Company thanks you for your trust!";
			}
			else
			{
				dialog.text = "Nothing has accumulated yet, Captain. Dividends are credited once a month - "+ shareRate +"% of the invested sum. As soon as the courier notifies you, come by any office.";
			}
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
			link.l2 = "Anything else?";
			link.l2.go = "Meet_2";
		break;

		case "Share_Topup":
			if (CheckAttribute(pchar, "GWIK.Share.TopupDate") && GetNpcQuestPastDayParam(pchar, "GWIK.Share.TopupDate") < 90)
			{
				dialog.text = "You increased your share recently, Captain. The Company accepts additional investments no more than once a quarter - come by later.";
				link.l1 = "All right, I'll come by later.";
				link.l1.go = "Meet_2";
				break;
			}
			dialog.text = "Of course. An additional investment will increase your stake, and with it your dividends. We accept from 100000 to 200000 piastres, no more than once a quarter. How much will you deposit?";
			link.l1.edit = 1;
			link.l1 = "";
			link.l1.go = "Share_TopupCalc";
			link.l2 = "Another time, perhaps.";
			link.l2.go = "Meet_2";
		break;

		case "Share_TopupCalc":
			cash = sti(dialogEditStrings[1]); // cash is used as the investment sum
			if (cash < 100000)
			{
				dialog.text = "An additional investment is no less than 100000 piastres, Captain. Name another sum.";
				link.l1 = "All right, let's try again.";
				link.l1.go = "Share_Topup";
				link.l2 = "Then never mind.";
				link.l2.go = "Meet_2";
				break;
			}
			if (cash > 200000) cash = 200000; // the Company won't take more than 200000 at once
			if (sti(pchar.money) < cash)
			{
				dialog.text = "You don't have that much on you, Captain. Name a more modest sum.";
				link.l1 = "All right, let's try again.";
				link.l1.go = "Share_Topup";
				link.l2 = "Then never mind.";
				link.l2.go = "Meet_2";
				break;
			}
			dialog.text = "You add "+ FindRussianMoneyString(cash) +" to your share. Dividends will be credited on the increased stake. Shall we draw it up?";
			link.l1 = "Yes, here's the money.";
			link.l1.go = "Share_TopupBuy";
			link.l2 = "No, I've changed my mind.";
			link.l2.go = "Meet_2";
		break;

		case "Share_TopupBuy":
			cash = sti(dialogEditStrings[1]);
			if (cash > 200000) cash = 200000;
			if (cash < 100000 || sti(pchar.money) < cash)
			{
				dialog.text = "Something doesn't add up, Captain. Let's start over.";
				link.l1 = "All right.";
				link.l1.go = "Share_Topup";
				break;
			}
			AddMoneyToCharacter(pchar, -cash);
			pchar.GWIK.Share.Invested = sti(pchar.GWIK.Share.Invested) + cash;
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Share.TopupDate");
			PlayStereoSound("notebook");
			dialog.text = "Done! Your stake has grown by "+ FindRussianMoneyString(cash) +". The next additional investment - no sooner than a quarter from now.";
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Job_Ask":
			// active assignment - a reminder
			if (CheckAttribute(pchar, "GWIK.Job"))
			{
				if (pchar.GWIK.Job == "mail")
				{
					if (pchar.GWIK.Job.Target == NPChar.id)
					{
						dialog.text = "So you have the parcel from our partners with you? Hand it over then!";
						link.l1 = "Yes, here it is.";
						link.l1.go = "Job_MailDone";
					}
					else
					{
						dialog.text = "You are carrying the Company's parcel to our office "+ sPartner +". The sooner it gets there, the better - twelve days, remember.";
						link.l1 = "Already on my way.";
						link.l1.go = "Meet_2";
						link.l2 = "I'm afraid I can't deliver it. Here's the parcel back.";
						link.l2.go = "Job_Drop";
					}
					break;
				}
				if (pchar.GWIK.Job == "ransom")
				{
					if (CheckAttribute(pchar, "GWIK.Job.Rescued"))
					{
						dialog.text = "You've brought our captain? Hand him over - the Company won't forget this service.";
						link.l1 = "Yes, he's aboard my ship.";
						link.l1.go = "Job_RansomDone";
					}
					else
					{
						if (CheckAttribute(pchar, "GWIK.Job.Force")) // chose the force path - the sloop is at sea
						{
							dialog.text = "You chose to do without the ransom - bold of you. The pirate ship '"+ pchar.GWIK.Job.ForceShipName +"' with our captain is still somewhere off Bermuda. Bring him back to us, and alive, preferably.";
							link.l1 = "I'll bring him.";
							link.l1.go = "Meet_2";
							link.l2 = "It's not working out. I give up.";
							link.l2.go = "Job_Drop";
						}
						else
						{
							dialog.text = "Our captain is still in the pirates' hands. The go-between is waiting in the tavern of the pirate settlement on Bermuda, the ransom is "+ FindRussianMoneyString(jobRansom) +". The Company will reimburse everything with a "+ jobRansomPay +" bonus on top.";
							link.l1 = "I remember. On my way.";
							link.l1.go = "Meet_2";
							link.l2 = "This assignment is too much for me. I give up.";
							link.l2.go = "Job_Drop";
						}
					}
					break;
				}
				if (pchar.GWIK.Job == "rival")
				{
					if (CheckAttribute(pchar, "GWIK.Job.Done"))
					{
						dialog.text = "They say the rivals are missing a ship... The Company is ready to settle the account.";
						if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) dialog.text = "They say a seat has come free at the rivals' table in "+ XI_ConvertString("Colony" + pchar.GWIK.Job.City) +"... The Company is ready to settle the account.";
						link.l1 = "Yes, the deed is done.";
						link.l1.go = "Job_RivalDone";
					}
					else
					{
						dialog.text = "The rivals' ship - "+ GWIK_RivalShipStr() +" - is still sailing "+ GWIK_JobPlaceStr() +". Make sure it never returns to port. And hurry - it won't stay there forever.";
						if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) dialog.text = "Their man, "+ pchar.GWIK.Job.RivalName +", still spends his evenings in the tavern at "+ XI_ConvertString("Colony" + pchar.GWIK.Job.City) +". He never puts to sea, so that is where you'll have to look for him. And hurry - he won't stay in that town forever.";
						if (CheckAttribute(pchar, "GWIK.Job.RivalOutTown")) dialog.text = "Word is you have already called him out. "+ pchar.GWIK.Job.RivalName +" is waiting beyond the gates of "+ XI_ConvertString("Colony" + pchar.GWIK.Job.City) +" - with a blade, and by all accounts with little patience. Do not keep him waiting too long."; // Gregg: the duel is already called - he is no longer in the tavern
						link.l1 = "I'll see to it.";
						link.l1.go = "Meet_2";
						link.l2 = "I've changed my mind. I withdraw from this business.";
						link.l2.go = "Job_Drop";
					}
					break;
				}
			}
			// already escorting?
			if (CheckQuestAttribute("generate_convoy_quest_progress", "begin"))
			{
				dialog.text = "You are already escorting a ship. See it to its destination first, then we'll talk about new assignments.";
				link.l1 = "Fair enough.";
				link.l1.go = "Meet_2";
				break;
			}
			// cooldown
			ok = false;
			if (CheckAttribute(pchar, "GWIK.JobDate") && GetNpcQuestPastDayParam(pchar, "GWIK.JobDate") < jobCooldown) ok = true;
			if (ok)
			{
				dialog.text = "Nothing for you at the moment, Captain. Come by in a few days - the Company always finds work.";
				link.l1 = "All right, I will.";
				link.l1.go = "Meet_2";
				break;
			}
			// generate an offer and keep it until a decision is made
			if (!CheckAttribute(pchar, "GWIK.JobOffer"))
			{
				iJob = drand(3); // Gregg: assignment type via drand - deterministic per day, F9-proof
				if (iJob == 0)
				{
					ok = (GetCompanionQuantity(PChar) < COMPANION_MAX);
					if (GetSummonSkillFromName(pchar, SKILL_LEADERSHIP) < 5) ok = false;
					if (sti(Pchar.Ship.Type) == SHIP_NOTUSED) ok = false;
					if (!ok) iJob = 1;
				}
				pchar.GWIK.JobOffer = iJob;
			}
			iJob = sti(pchar.GWIK.JobOffer);
			if (iJob == 0)
			{
				dialog.text = "There is. A Company ship with valuable cargo needs a reliable escort to one of the colonies. The captain will pay for the convoy upon arrival - settle the price with him yourself - and the Company will add its own bonus: "+ FindRussianMoneyString(jobConvoyBonus) +" at any of the offices. Will you take it?";
				link.l1 = "I will. Where's the captain?";
				link.l1.go = "Job_ConvoyGo";
			}
			if (iJob == 1)
			{
				dialog.text = "There is. A parcel of business papers must be delivered to our office "+ sPartner +" - within twelve days at the latest. The Company will pay "+ FindRussianMoneyString(jobMailPay) +". Will you take it?";
				link.l1 = "I will. Give me the parcel.";
				link.l1.go = "Job_MailGo";
			}
			if (iJob == 2)
			{
				dialog.text = "There is, and it's a delicate matter. Pirates have captured a Company captain and demand a ransom of "+ FindRussianMoneyString(jobRansom) +". Their go-between sits in the tavern of the pirate settlement on Bermuda. Ransom our man and bring him to any office - the Company will reimburse the ransom and add a "+ jobRansomPay +" bonus. Will you take it?";
				link.l1 = "I will. Have the money ready.";
				link.l1.go = "Job_RansomGo";
			}
			if (iJob == 3)
			{
				dialog.text = "There is one matter... not for the record books. The Company's rivals have grown fond of these waters and they carry cargo far too cheaply. The Company pays "+ FindRussianMoneyString(jobRivalPay) +" if that nuisance disappears - and how exactly you remove it is no concern of ours. Will you take it?"; // Gregg: text kept NEUTRAL - the variant (ship at anchor / ship at sea / man in a tavern) is only rolled in Job_RivalGo
				link.l1 = "I will. Name the place.";
				link.l1.go = "Job_RivalGo";
			}
			link.l2 = "No, that's not for me.";
			link.l2.go = "Job_Refuse";
		break;

		case "Job_Refuse":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "As you wish. The Company will find another man for the job.";
			link.l1 = "Good day.";
			link.l1.go = "Meet_2";
		break;

		case "Jamaica_Offer": // Gregg: special quest - opening the Fort Orange office
			dialog.text = "Captain, a word not for other ears. The Heeren XVII - the Company's directors - have resolved to open an office on Jamaica, in Fort Orange, right under the nose of English Port Royal. A delicate, slow affair: it needs a proven man of our own. The Company has chosen you.";
			link.l1 = "Why Fort Orange of all places? Tell me more.";
			link.l1.go = "Jamaica_Offer2";
			link.l2 = "It's an honour. I'm in.";
			link.l2.go = "Jamaica_Accept";
			link.l3 = "Let me think on it.";
			link.l3.go = "Meet_2";
		break;

		case "Jamaica_Offer2": // Gregg: exposition - why the Company wants Jamaica and what's in it for the player
			dialog.text = "Fort Orange is the only Dutch port in the very heart of English waters. Whoever holds warehouses there holds the trade of half the archipelago. The Heeren XVII are ready to invest, but cannot act openly: Port Royal would smother the venture in its cradle. It needs a man with a name, a ship and steady nerves. In return - a co-founder's share in the new branch, and the Company's gratitude, which is worth a great deal.";
			link.l1 = "You've convinced me. I'm in.";
			link.l1.go = "Jamaica_Accept";
			link.l2 = "Let me think on it.";
			link.l2.go = "Meet_2";
		break;

		case "Jamaica_Accept":
			GWIK_StartJamaicaQuest();
			dialog.text = "Excellent. Our envoy awaits you in Fort Orange - a trusted man of the Company, look for him at the residence. He runs the affair on the spot and will brief you. And remember: not a word of this to anyone.";
			link.l1 = "I'll set off at once.";
			link.l1.go = "exit";
		break;

		case "Jamaica_Factor": // Gregg: the factor (stage 4) walks in from the street and introduces himself (GWIK_Factor.c) - no middleman
			if (!CheckAttribute(pchar, "GWIK.Jamaica.FactorId"))
			{
				GWIK_JamaicaSpawnFactorInOffice();
				dialog.text = "He has waited months for this day, Captain - been circling the office since morning... Ah, there he is - speak of the devil. He'll tell you everything himself.";
				link.l1 = "I see him. We'll talk.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Mister "+ pchar.GWIK.Jamaica.FactorLName +" is waiting for you where you met him. Have a word with him - and off you go.";
			link.l1 = "On my way.";
			link.l1.go = "exit";
		break;

		case "Jamaica_Convoy": // Gregg: take the supply convoy under escort at Curacao (stage 3)
			if (GetCompanionQuantity(pchar) > COMPANION_MAX - 2)
			{
				dialog.text = "I would gladly, Captain, but two galleons won't fit into your squadron - there's no room. Free up a couple of berths and come back.";
				link.l1 = "I'll make room.";
				link.l1.go = "Meet_2";
				break;
			}
			if (sti(pchar.money) < 50000)
			{
				dialog.text = "Here's how it works: the convoy is the Company's, but the founder's stake is yours. Fifty thousand piastres into the venture, and the galleons are yours as far as Fort Orange. Do you have that on you?";
				link.l1 = "I'll gather the money.";
				link.l1.go = "Meet_2";
				break;
			}
			dialog.text = "Two galleons are loaded to the gunwales: mahogany, sandalwood, bales of silk - enough to set up a fine office. Your founder's stake is fifty thousand into the venture, and I'll place the convoy under your hand. Lead it to Fort Orange and guard it well: the English have already caught wind of this cargo.";
			link.l1 = "Agreed. Ready the galleons.";
			link.l1.go = "Jamaica_Convoy_Go";
			link.l2 = "Let me think on it.";
			link.l2.go = "Meet_2";
		break;

		case "Jamaica_Convoy_Go":
			if (!GWIK_JamaicaSpawnConvoy()) // failed (no free slots) - don't dead-end
			{
				dialog.text = "Hmph, there turned out to be no room in your squadron for the galleons after all. Free up a couple of berths and come back.";
				link.l1 = "I'll make room.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -50000);
			PlayStereoSound("notebook");
			AddQuestRecord("GWIK_JamaicaJob", "15");
			dialog.text = "Godspeed, Captain. The galleons will put to sea behind you - keep them under your guns. The envoy will meet you in Fort Orange. Fair winds!";
			link.l1 = "We sail for Fort Orange.";
			link.l1.go = "exit";
		break;

		case "Job_ConvoyGo":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			sJobCity = findTraderCity_GWIK(NPChar);
			if (sJobCity == "none")
			{
				dialog.text = "Hm, it seems our captain has already set sail. Bad luck - come by another time.";
				link.l1 = "It happens...";
				link.l1.go = "Meet_2";
				break;
			}
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			pchar.quest.destination = sJobCity;
			pchar.ConvoyQuest.City = NPChar.city;
			DeleteAttribute(pchar, "GWIK.ConvoyBonus"); // Gregg: clear the old node together with .Done - otherwise a bonus for an undelivered convoy
			pchar.GWIK.ConvoyBonus = jobConvoyBonus; // Company bonus on top of the trader's pay
			AddDialogExitQuest("prepare_for_convoy_quest");
			ReOpenQuestHeader("GWIK_ConvoyJob"); // Gregg: Company escort journal
			AddQuestRecord("GWIK_ConvoyJob", "1");
			AddQuestUserData("GWIK_ConvoyJob", "sCity", XI_ConvertString("Colony" + sJobCity + "Gen")); // Gregg: запись говорит «до <города>» - родительный
			AddQuestUserData("GWIK_ConvoyJob", "sMoney", FindRussianMoneyString(jobConvoyBonus));
			// Gregg: RARE chance of an ambush (~25%) off the destination island (own stream drand1); the escort already catches the world map's random encounters, so keep it rare. Beta forces/disables
			bMap = false;
			if (drand1(3) == 0) bMap = true;
			if (CheckAttribute(pchar, "GWIK.BetaForceConvoy")) { bMap = false; if (pchar.GWIK.BetaForceConvoy == "ambush") bMap = true; DeleteAttribute(pchar, "GWIK.BetaForceConvoy"); }
			DeleteAttribute(pchar, "GWIK.ConvoyAmbush"); // reset from a previous escort
			if (bMap) bMap = GWIK_ConvoySpawnAmbush(sJobCity); // didn't spawn (no locator) - escort without an ambush
			dialog.text = "Excellent! And here comes our captain - discuss the details with him.";
			if (bMap) dialog.text = "Excellent! And here comes our captain. Only... word is pirates have been prowling off "+ XI_ConvertString("Colony" + sJobCity + "Gen") +". Keep your guns ready, Captain.";
			link.l1 = "I'll have a word with him now.";
			link.l1.go = "exit";
		break;

		case "Job_MailGo":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			pchar.GWIK.Job = "mail";
			pchar.GWIK.Job.Target = sPartnerId;
			sJobCity = GWIK_OfficeCityById(sPartnerId);
			pchar.GWIK.Job.Pay = jobMailPay;
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Job.Date");
			PlayStereoSound("notebook");
			ReOpenQuestHeader("GWIK_MailJob");
			AddQuestRecord("GWIK_MailJob", "1");
			AddQuestUserData("GWIK_MailJob", "sCity", XI_ConvertString("Colony" + sJobCity + "Voc")); // Gregg: запись говорит «в конторе в <городе>» - предложный
			AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(jobMailPay));
			dialog.text = "Here's the parcel, it's already among your papers. Hand it personally to our representative "+ sPartner +". And remember: the Company's seals are better left unbroken - our reach is long.";
			link.l1 = "I'll deliver it intact. Goodbye!";
			link.l1.go = "exit";
			iJob = drand1(2); // Gregg: 1/3 - a 'hot parcel': a rival agent intercepts you on the street of the office's town (own stream drand1, job type = drand(3))
			if (CheckAttribute(pchar, "GWIK.BetaForceMail")) { iJob = 0; DeleteAttribute(pchar, "GWIK.BetaForceMail"); }
			if (iJob == 0)
			{
				pchar.GWIK.Job.Hot = true;
				pchar.GWIK.Job.HotTown = GWIK_OfficeCityById(NPChar.id) + "_town"; // the current office's town - the agent will intercept you there in the street
				DeleteAttribute(pchar, "quest.GWIK_MailAgent.over");
				pchar.quest.GWIK_MailAgent.win_condition.l1 = "Location";
				pchar.quest.GWIK_MailAgent.win_condition.l1.location = pchar.GWIK.Job.HotTown;
				pchar.quest.GWIK_MailAgent.win_condition = "GWIK_MailAgentArrive";
				pchar.quest.GWIK_MailAgent.function = "GWIK_MailAgentArrive";
			}
		break;

		case "Job_RansomGo":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			pchar.GWIK.Job = "ransom";
			pchar.GWIK.Job.Ransom = jobRansom;
			pchar.GWIK.Job.Bonus = jobRansomPay;
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Job.Date");
			iJob = drand2(1); // Gregg: ransom sub-variant via drand2 (own stream! job type - drand(3), sea rival - drand1); 0 mediator on Bermuda, 1 captive aboard a cruising pirate
			if (CheckAttribute(pchar, "GWIK.BetaForceRansom")) { iJob = 0; if (pchar.GWIK.BetaForceRansom == "pirate") iJob = 1; if (pchar.GWIK.BetaForceRansom == "dead") pchar.GWIK.Job.CaptiveDead = true; if (pchar.GWIK.BetaForceRansom == "mutiny") pchar.GWIK.Job.Mutiny = true; DeleteAttribute(pchar, "GWIK.BetaForceRansom"); }
			if (iJob == 1 && GWIK_RansomForceSpawn("")) // variant "aboard a pirate": spawn the patrolling pirate off a random colony straight away, skipping the mediator
			{
				ReOpenQuestHeader("GWIK_RansomJob");
				AddQuestRecord("GWIK_RansomJob", "9");
				AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName);
				AddQuestUserData("GWIK_RansomJob", "sCap", pchar.GWIK.Job.ForceCapName);
				AddQuestUserData("GWIK_RansomJob", "sCity", XI_ConvertString("Colony" + pchar.GWIK.Job.PirateCity + "Gen"));
				AddQuestUserData("GWIK_RansomJob", "sBonus", FindRussianMoneyString(jobRansomPay));
				dialog.text = "Our captain isn't on Bermuda - a pirate named "+ pchar.GWIK.Job.ForceCapName +" holds him aboard the ship '"+ pchar.GWIK.Job.ForceShipName +"'. She prowls the waters off "+ XI_ConvertString("Colony" + pchar.GWIK.Job.PirateCity + "Gen") +". Intercept and board her - the Company will add a bonus of "+ FindRussianMoneyString(jobRansomPay) +". But send her to the bottom and you drown the captive too.";
				// Gregg: she is still in port for the first two days (GWIK_RansomPutToSea) - say so, or we send the player to waters where there is no ship yet
				if (CheckAttribute(pchar, "GWIK.Job.PirateInPort")) dialog.text = "Our captain isn't on Bermuda - a pirate named "+ pchar.GWIK.Job.ForceCapName +" holds him aboard the ship '"+ pchar.GWIK.Job.ForceShipName +"'. For now she lies in the port of "+ XI_ConvertString("Colony" + pchar.GWIK.Job.PirateCity + "Gen") +" - still taking on cargo, but she will put to sea in a day or two. Intercept and board her - the Company will add a bonus of "+ FindRussianMoneyString(jobRansomPay) +". But send her to the bottom and you drown the captive too.";
				link.l1 = "I'll find her and board her.";
				link.l1.go = "exit";
				break;
			}
			GWIK_StartRansomJob();
			if (!CheckAttribute(pchar, "GWIK.Job.Mutiny") && !CheckAttribute(pchar, "GWIK.Job.CaptiveDead")) // Gregg: mutually exclusive rare twists of the mediator variant (unless forced by beta)
			{
				if (drand1(19) == 0) pchar.GWIK.Job.Mutiny = true; // ~1/20 (drand1 - own stream): there's no captive, the captain turned pirate himself (you learn it from the 'go-between' on Bermuda)
				else if (drand3(9) == 0) pchar.GWIK.Job.CaptiveDead = true; // ~1/10 (drand3 - own stream): the captive will be killed for insolence; you learn it from the mediator (failure with a token compensation)
			}
			ReOpenQuestHeader("GWIK_RansomJob");
			AddQuestRecord("GWIK_RansomJob", "1");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(jobRansom));
			AddQuestUserData("GWIK_RansomJob", "sBonus", FindRussianMoneyString(jobRansomPay));
			dialog.text = "Remember: the tavern of the pirate settlement on Bermuda, the ransom is "+ FindRussianMoneyString(jobRansom) +", out of your own funds. The Company will reimburse everything as soon as our man reaches any of the offices. Good luck, Captain.";
			link.l1 = "Your captain will be here soon.";
			link.l1.go = "exit";
		break;

		case "Job_RivalGo":
			DeleteAttribute(pchar, "GWIK.JobOffer");
			sJobCity = findRivalCity_GWIK(NPChar);
			pchar.GWIK.Job = "rival";
			ok = false;
			if (sJobCity != "none") ok = GWIK_SpawnRival(sJobCity);
			if (!ok)
			{
				// no suitable spot found - replace with a mail run
				pchar.GWIK.Job = "mail";
				pchar.GWIK.Job.Target = sPartnerId;
				sJobCity = GWIK_OfficeCityById(sPartnerId);
				pchar.GWIK.Job.Pay = jobMailPay;
				SaveCurrentNpcQuestDateParam(pchar, "GWIK.Job.Date");
				PlayStereoSound("notebook");
				ReOpenQuestHeader("GWIK_MailJob");
				AddQuestRecord("GWIK_MailJob", "1");
				AddQuestUserData("GWIK_MailJob", "sCity", XI_ConvertString("Colony" + sJobCity + "Voc")); // Gregg: та же запись #1 при подмене поручения - предложный
				AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(jobMailPay));
				dialog.text = "Hm... circumstances have changed, the rivals' ship is already gone. But since you're here - deliver a parcel to our office "+ sPartner +", the pay is "+ FindRussianMoneyString(jobMailPay) +". The parcel is already among your papers.";
				link.l1 = "Agreed.";
				link.l1.go = "exit";
				break;
			}
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.Job.Date");
			pchar.GWIK.Job.Pay = jobRivalPay;
			ReOpenQuestHeader("GWIK_RivalJob");
			if (CheckAttribute(pchar, "GWIK.Job.RivalLand")) // Gregg: land variant - the target is not a ship but a man in a colony tavern
			{
				AddQuestRecord("GWIK_RivalJob", "6");
				AddQuestUserData("GWIK_RivalJob", "sCity", XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Voc")); // Gregg: запись #6 говорит «в <городе> обосновался» - предложный (в записи #1 та же переменная идёт после «в водах», там родительный)
				AddQuestUserData("GWIK_RivalJob", "sName", pchar.GWIK.Job.RivalName);
				AddQuestUserData("GWIK_RivalJob", "sMoney", FindRussianMoneyString(jobRivalPay));
				dialog.text = "This time the matter is not at sea. Their man, "+ pchar.GWIK.Job.RivalName +", sits in the tavern at "+ XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Gen") +" and undercuts our every price from there. He never puts to sea himself - a careful sort. The rest is up to you, the Company will ask no questions.";
				// Gregg: a hint about the charter - otherwise nobody will find the prize on the body. The attribute is set in GWIK_SpawnRivalLand only when the player holds no licence of their own
				if (CheckAttribute(pchar, "GWIK.Job.RivalLicence")) dialog.text = dialog.text + " And here is the curious part: he trades under our own charter - drawn up through third hands. He carries the paper on him. Should it never find its way back to us, the Company will survive.";
				link.l1 = "I'll find him.";
				link.l1.go = "exit";
				break;
			}
			AddQuestRecord("GWIK_RivalJob", "1");
			AddQuestUserData("GWIK_RivalJob", "sCity", XI_ConvertString("Colony" + pchar.GWIK.Job.City + "Gen"));
			AddQuestUserData("GWIK_RivalJob", "sShip", GWIK_RivalShipStr()); // Gregg: type + ship name
			AddQuestUserData("GWIK_RivalJob", "sMoney", FindRussianMoneyString(jobRivalPay));
			dialog.text = "Our people have seen their ship - "+ GWIK_RivalShipStr() +" - "+ GWIK_JobPlaceStr() +". The rest is up to you, the Company will ask no questions. And hurry: it won't stay there long.";
			link.l1 = "Consider it sunk already.";
			link.l1.go = "exit";
		break;

		case "Job_Drop":
			if (pchar.GWIK.Job == "mail")
			{
				GWIK_MailAgentCleanup(); // Gregg: remove the 'hot parcel' agent if any
				AddQuestRecord("GWIK_MailJob", "4");
				CloseQuestHeader("GWIK_MailJob");
			}
			if (pchar.GWIK.Job == "ransom")
			{
				AddQuestRecord("GWIK_RansomJob", "4");
				CloseQuestHeader("GWIK_RansomJob");
				GWIK_FinishRansomJob();
			}
			if (pchar.GWIK.Job == "rival")
			{
				AddQuestRecord("GWIK_RivalJob", "5");
				CloseQuestHeader("GWIK_RivalJob");
				GWIK_DropRivalJob();
			}
			DeleteAttribute(pchar, "GWIK.Job"); // for mail; the others are already cleaned up
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "A pity. The Company prefers captains who see things through.";
			link.l1 = "Circumstances are stronger than me. Goodbye.";
			link.l1.go = "exit";
		break;

		case "Job_MailDone":
			ok = (GetNpcQuestPastDayParam(pchar, "GWIK.Job.Date") <= 12);
			cash = jobMailPay;
			if (CheckAttribute(pchar, "GWIK.Job.Pay")) cash = sti(pchar.GWIK.Job.Pay); // the sum promised at the briefing
			// Gregg: slipped the interception and carried the parcel through without ever talking to the agent - a full outcome,
			// not "the content failed to fire". Flags read BEFORE the cleanup: GWIK.Job is deleted below. Separate ifs - Storm will not compile a compound assignment with CheckAttribute
			bDodged = false;
			if (CheckAttribute(pchar, "GWIK.Job.HotDodge")) bDodged = true;
			if (CheckAttribute(pchar, "GWIK.Job.HotResolved")) bDodged = false; // the talk did happen after all - ordinary outcome
			GWIK_MailAgentCleanup(); // Gregg: remove the 'hot parcel' agent if left over (BEFORE deleting GWIK.Job - it reads MailAgentId)
			DeleteAttribute(pchar, "GWIK.Job");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			PlayStereoSound("notebook");
			if (ok)
			{
				AddMoneyToCharacter(pchar, cash);
				GWIK_AddRep(10); // Gregg: reputation for mail delivered on time
				AddCharacterExpToSkill(pchar, SKILL_SAILING, 40);    // Gregg: navigation xp for a courier run delivered on time
				AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 15); // Gregg: and a little command xp for the completed Company job
				AddQuestRecord("GWIK_MailJob", "2");
				AddQuestUserData("GWIK_MailJob", "sCity", XI_ConvertString("Colony" + NPChar.city + "Voc")); // Gregg: запись #2 говорит «контора в <городе>» - предложный
				AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(cash));
				CloseQuestHeader("GWIK_MailJob");
				dialog.text = "Seals intact, deadline met - a pleasure to deal with a professional. Here are your "+ FindRussianMoneyString(cash) +".";
			}
			else
			{
				AddMoneyToCharacter(pchar, makeint(cash / 2));
				AddQuestRecord("GWIK_MailJob", "3");
				AddQuestUserData("GWIK_MailJob", "sMoney", FindRussianMoneyString(makeint(cash / 2)));
				CloseQuestHeader("GWIK_MailJob");
				dialog.text = "The seals are intact, but you are late, Captain. The Company pays half - "+ FindRussianMoneyString(makeint(cash / 2)) +". Be quicker next time.";
			}
			if (bDodged) // Gregg: the parcel was hot, yet the rivals' men never got near you - the Company knows and appreciates it
			{
				GWIK_AddRep(3);
				ReOpenQuestHeader("GWIK_MailJob"); // appending to an already closed header works (as with the Heeren XVII letter)
				AddQuestRecord("GWIK_MailJob", "9");
				CloseQuestHeader("GWIK_MailJob");
				dialog.text = dialog.text + " And one more thing, Captain: men with no connection to the Company were asking after this parcel. You did not stop to make their acquaintance - just as well.";
			}
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Job_RansomDone":
			cash = sti(pchar.GWIK.Job.Ransom) + jobRansomPay;
			if (CheckAttribute(pchar, "GWIK.Job.Bonus")) cash = sti(pchar.GWIK.Job.Ransom) + sti(pchar.GWIK.Job.Bonus); // the sums promised at the briefing
			ok = CheckAttribute(pchar, "GWIK.Job.Force"); // taken by force - no ransom paid, bonus plus a premium
			bMutiny = CheckAttribute(pchar, "GWIK.Job.Mutiny"); // Gregg: capture BEFORE GWIK_FinishRansomJob deletes GWIK.Job, else the mutiny delivery text below never fires
			if (ok)
			{
				cash = jobRansomPay;
				if (CheckAttribute(pchar, "GWIK.Job.Bonus")) cash = sti(pchar.GWIK.Job.Bonus);
				cash = makeint(cash * 125 / 100); // +25% for the combat risk
			}
			AddMoneyToCharacter(pchar, cash);
			GWIK_FinishRansomJob();
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			PlayStereoSound("notebook");
			GWIK_AddRep(13); // Gregg: reputation for the delivered captain
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 30); // Gregg: command xp for the returned captain (force-path combat xp already granted at sea)
			if (bMutiny) AddQuestRecord("GWIK_RansomJob", "21"); // Gregg: mutiny - dragged the runaway back by force, no ransom; own record without "reimbursed the ransom"
			else AddQuestRecord("GWIK_RansomJob", "3");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			dialog.text = "You have returned one of the Company's finest captains! Here is the ransom reimbursement and the promised bonus - "+ FindRussianMoneyString(cash) +" in total. The Company does not forget such services.";
			if (ok) dialog.text = "Wrested our captain from the pirates by force, without any ransom? Bold - and worthy! For your daring the Company pays the promised bonus with a premium - "+ FindRussianMoneyString(cash) +" in all. The Company values men like you.";
			if (ok && bMutiny) dialog.text = "Dragged our runaway back in irons? Well, the traitor had it coming. For your daring - the bonus with a premium, "+ FindRussianMoneyString(cash) +" in all. The Company will deal with him from here.";
			link.l1 = "Glad to help. Goodbye!";
			link.l1.go = "exit";
		break;

		case "GWIK_RansomDeadReport": // Gregg: report the captive's death - paid by the COMPANY (not the pirate), a token compensation for the trouble, no rep penalty
			cash = 2000 + sti(pchar.rank) * 200;
			AddMoneyToCharacter(pchar, cash);
			AddQuestRecord("GWIK_RansomJob", "11");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			GWIK_FinishRansomJob(); // remove the mediator + GWIK.Job (we're speaking to the office rep, not the mediator - safe)
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "I'm sorry to hear it. Well, pirates are pirates - no fault of yours, Captain. Here's a little for your trouble, "+ FindRussianMoneyString(cash) +". The Company remembers those who tried.";
			link.l1 = "Thank you. Goodbye.";
			link.l1.go = "exit";
		break;

		case "GWIK_MutinyReturnReport": // Gregg: the runaway returned to the Company on his own (successful persuasion) - a positive report: bonus + reputation, no ransom, no battle
			cash = jobRansomPay;
			if (CheckAttribute(pchar, "GWIK.Job.Bonus")) cash = sti(pchar.GWIK.Job.Bonus);
			AddMoneyToCharacter(pchar, cash);
			GWIK_FinishRansomJob(); // remove the runaway from the tavern + GWIK.Job (we're speaking to the office rep, not to him - safe)
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			PlayStereoSound("notebook");
			GWIK_AddRep(13); // Gregg: reputation for the returned captain
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 30); // Gregg: command xp for bringing the runaway to his senses
			AddQuestRecord("GWIK_RansomJob", "17");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			dialog.text = "Ah, so it's you I have to thank? Our captain did turn himself in - said you brought him to his senses. The Company will overlook his lapse, and you get the promised bonus, "+ FindRussianMoneyString(cash) +". Settling matters without bloodshed is a rare gift.";
			link.l1 = "Glad to help. Goodbye!";
			link.l1.go = "exit";
		break;

		case "GWIK_MutineerKilledReport": // Gregg: the runaway resisted and was cut down aboard his ship - a bounty for stopping the traitor; no prisoner, no rep penalty
			cash = 3000 + sti(pchar.rank) * 300;
			AddMoneyToCharacter(pchar, cash);
			AddQuestRecord("GWIK_RansomJob", "20");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			GWIK_FinishRansomJob();
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			GWIK_AddRep(8); // Gregg: small reputation for stopping the defector (the Company would have preferred him alive)
			dialog.text = "Cut him down, did you? A hard end - but a coat-turner who runs to the pirates earns no better. We'd sooner have had him alive to make an example... still, here's a bounty for your trouble, "+ FindRussianMoneyString(cash) +". The matter is closed.";
			link.l1 = "Thank you. Goodbye.";
			link.l1.go = "exit";
		break;

		case "GWIK_MutinyReport": // Gregg: report that the captain turned pirate and refused to return - a token compensation for the trouble, no rep penalty
			cash = 2000 + sti(pchar.rank) * 200;
			AddMoneyToCharacter(pchar, cash);
			AddQuestRecord("GWIK_RansomJob", "16");
			AddQuestUserData("GWIK_RansomJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RansomJob");
			GWIK_FinishRansomJob(); // remove the runaway from the tavern + GWIK.Job (we're speaking to the office rep, not to him - safe)
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			dialog.text = "Turned coat, did he... A pity, but it happens - you can't hold every man. No fault of yours, Captain. Here's a little for your trouble, "+ FindRussianMoneyString(cash) +". The Company will deal with the deserter itself.";
			link.l1 = "Thank you. Goodbye.";
			link.l1.go = "exit";
		break;

		case "Job_RivalDone":
			cash = jobRivalPay;
			if (CheckAttribute(pchar, "GWIK.Job.Pay")) cash = sti(pchar.GWIK.Job.Pay); // the sum promised at the briefing
			AddMoneyToCharacter(pchar, cash);
			pchar.quest.GWIK_RivalDead.over = "yes";
			pchar.quest.GWIK_RivalGone.over = "yes";
			DeleteAttribute(pchar, "GWIK.Job");
			SaveCurrentNpcQuestDateParam(pchar, "GWIK.JobDate");
			PlayStereoSound("notebook");
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 30); // Gregg: command xp for the covert operation (the sinking already granted combat xp)
			AddQuestRecord("GWIK_RivalJob", "3"); // user data strictly after the record
			AddQuestUserData("GWIK_RivalJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_RivalJob");
			dialog.text = "The Company knows how to be grateful... and silent. Here are your "+ FindRussianMoneyString(cash) +". This conversation, of course, never happened.";
			link.l1 = "What conversation? Goodbye.";
			link.l1.go = "exit";
		break;

		case "Job_ConvoyBonus":
			cash = sti(pchar.GWIK.ConvoyBonus);
			AddMoneyToCharacter(pchar, cash);
			DeleteAttribute(pchar, "GWIK.ConvoyBonus");
			PlayStereoSound("notebook");
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 25); // Gregg: command xp for an exemplary escort (sea skills already granted by the base convoy quest)
			AddQuestRecord("GWIK_ConvoyJob", "3"); // Gregg: journal - bonus collected
			AddQuestUserData("GWIK_ConvoyJob", "sMoney", FindRussianMoneyString(cash));
			CloseQuestHeader("GWIK_ConvoyJob");
			dialog.text = "Our captain has already reported to the office: the escort was exemplary. Here is the Company's promised bonus - "+ FindRussianMoneyString(cash) +".";
			link.l1 = "A pleasure working with the Company. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Subscribe_Ask":
			if (!CheckAttribute(pchar,"GWIK.Subscription"))
			{
				dialog.text = "Of course, Captain. A monthly subscription: every five days our courier will deliver you a fresh price report for the whole archipelago, wherever you are. The price is "+ FindRussianMoneyString(subPrice) +", with the first report issued right away. Interested?";
			}
			else
			{
				dialog.text = "Extending the subscription for another month will cost you the same "+ FindRussianMoneyString(subPrice) +". Shall we?";
			}
			if (sti(pchar.money) >= subPrice)
			{
				link.l1 = "Yes, here's the money.";
				link.l1.go = "Subscribe_Buy";
			}
			else
			{
				link.l1 = "Hm, I don't have that much on me...";
				link.l1.go = "Meet_2";
			}
			link.l2 = "No, I'll manage without it.";
			link.l2.go = "Meet_2";
		break;

		case "Subscribe_Buy":
			AddMoneyToCharacter(pchar, -subPrice);
			PlayStereoSound("notebook");
			if (CheckAttribute(pchar,"GWIK.Subscription"))
			{
				pchar.GWIK.Subscription = sti(pchar.GWIK.Subscription) + 30;
				dialog.text = "Done, your subscription is extended. The Company's couriers won't keep you waiting.";
			}
			else
			{
				pchar.GWIK.Subscription = 30;
				SetTimerFunction("GWIK_SubscriptionTick", 0, 0, 5);
				GWIK_RefreshAllPrices();
				log_info("Received a price list for every store in the Caribbean archipelago.");
				dialog.text = "All set! Here is your first report, the rest will be delivered every five days. Profitable trading, Captain!";
			}
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Contract_Info":
			iGood = sti(NPChar.GWIK.Contract.Good);
			cQty = sti(NPChar.GWIK.Contract.Qty);
			cPrice = sti(NPChar.GWIK.Contract.Price);
			if (cQty <= 0)
			{
				dialog.text = "The current purchase contract has already been fulfilled - not without your help, Captain. The Company will send a new list of needs in a couple of days.";
				link.l1 = "I'll come by later.";
				link.l1.go = "Meet_2";
				break;
			}
			cCargo = GetSquadronGoods(pchar, iGood); // Gregg: count the holds of the whole squadron, as with confiscated-goods purchase
			dialog.text = "The Company is buying: "+ XI_ConvertString(Goods[iGood].name) +", "+ cQty +" units at "+ FindRussianMoneyString(cPrice) +" per "+ Goods[iGood].Units +" units - well above the market price. Your squadron's holds currently have "+ cCargo +" units of this good.";
			if (cCargo > 0)
			{
				link.l1 = "Excellent, I'll deliver right now.";
				link.l1.go = "Contract_Sell";
			}
			link.l2 = "Noted. Thank you for the information.";
			link.l2.go = "Meet_2";
		break;

		case "Contract_Sell":
			iGood = sti(NPChar.GWIK.Contract.Good);
			cQty = sti(NPChar.GWIK.Contract.Qty);
			cPrice = sti(NPChar.GWIK.Contract.Price);
			cCargo = GetSquadronGoods(pchar, iGood); // Gregg: total across the squadron
			sell = cCargo;
			if (sell > cQty) sell = cQty;
			if (sell <= 0)
			{
				dialog.text = "Hm, there isn't a single unit of the required good in your squadron's holds, Captain.";
				link.l1 = "My mistake, I beg your pardon.";
				link.l1.go = "Meet_2";
				break;
			}
			GWIK_RemoveGoodsSquadron(pchar, iGood, sell); // Gregg: remove in cascade - flagship, then squadron ships
			AddMoneyToCharacter(pchar, GWIK_GoodsMoney(iGood, sell, cPrice));
			NPChar.GWIK.Contract.Qty = cQty - sell;
			PlayStereoSound("notebook");
			if (cQty - sell <= 0)
			{
				SaveCurrentNpcQuestDateParam(NPChar, "GWIK.ContractDate"); // countdown to the next contract
				GWIK_AddRep(10); // Gregg: reputation for a fully closed contract
				AddCharacterExpToSkill(pchar, SKILL_COMMERCE, 40);   // Gregg: trade xp for a fully closed contract (turn-in pays via direct AddMoney, bypassing the trade engine)
				AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 10); // Gregg: and a little command xp for the completed job
				dialog.text = "Received "+ sell +" units, here are your "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, sell, cPrice)) +". The contract is fully closed - the Company values suppliers like you! A new order will arrive in a couple of days.";
			}
			else
			{
				dialog.text = "Received "+ sell +" units, here are your "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, sell, cPrice)) +". The Company is ready to buy "+ (cQty - sell) +" more units at the same price.";
			}
			link.l1 = "A pleasure doing business with you. Goodbye!";
			link.l1.go = "exit";
			link.l2 = "Anything else?";
			link.l2.go = "Meet_2";
		break;

		case "Auction_Info":
			bPrimary = false;
			if (CheckAttribute(NPChar,"GWIK.Auction") && !CheckAttribute(NPChar,"GWIK.Auction.SoldOut")) bPrimary = true;
			bExtra = false;
			if (shareLvl >= 2 && CheckAttribute(NPChar,"GWIK.AuctionExtra") && !CheckAttribute(NPChar,"GWIK.AuctionExtra.SoldOut")) bExtra = true;
			bElite = false; // Gregg: Companion exclusive - rare DAILY offer (drand, F9-proof); map and spyglass never on the same day (one drand(29), different check values)
			if (shareLvl >= 3 && GetCharacterItem(pchar,"map_full") < 1 && drand(29) == 0) bElite = true;
			if (shareLvl >= 3 && GetCharacterItem(pchar,"spyglass5") < 1 && drand(29) == 15) bElite = true;
			if (!bPrimary && !bExtra && !bElite)
			{
				dialog.text = "Alas, today's lot has already been sold. Confiscated goods arrive regularly - come by in a few days.";
				link.l1 = "I certainly will.";
				link.l1.go = "Meet_2";
				break;
			}
			if (bPrimary)
			{
				if (NPChar.GWIK.Auction == "goods")
				{
					iGood = sti(NPChar.GWIK.Auction.Good);
					aQty = sti(NPChar.GWIK.Auction.Qty);
					aPrice = sti(NPChar.GWIK.Auction.Price);
					dialog.text = "Under the hammer today is a confiscated cargo: "+ XI_ConvertString(Goods[iGood].name) +", "+ aQty +" units at "+ FindRussianMoneyString(aPrice) +" per "+ Goods[iGood].Units +" units - well below the market. The whole lot will cost you "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, aQty, aPrice)) +".";
					if (GWIK_GoodsAffordable(iGood, sti(pchar.money), aPrice) > 0)
					{
						link.l1 = "I'll take it! As much as fits in the hold.";
						link.l1.go = "Auction_BuyGoods";
					}
				}
				else
				{
					rItem = ItemsFromID(NPChar.GWIK.Auction.Item);
					aPrice = sti(NPChar.GWIK.Auction.Price);
					sItemName = GetConvertStr(rItem.name, "ItemsDescribe.txt");
					dialog.text = "Under the hammer today is a confiscated piece from a captain's cabin: "+ sItemName +". The price is "+ FindRussianMoneyString(aPrice) +", well below what the shops ask.";
					if (sti(pchar.money) >= aPrice)
					{
						link.l1 = "I'll take it, no questions asked!";
						link.l1.go = "Auction_BuyItem";
					}
				}
			}
			else
			{
				if (bExtra) dialog.text = "The main lot is already gone, but the Company has set aside one more for its shareholders.";
				else dialog.text = "Today's confiscated goods are all spoken for, but the Company always keeps something special for its closest partners.";
			}
			if (bExtra)
			{
				link.l3 = "What's set aside for shareholders?";
				link.l3.go = "Auction_InfoExtra";
			}
			if (bElite)
			{
				link.l4 = "What does the Company keep for its closest partners?";
				link.l4.go = "Auction_Elite";
			}
			link.l2 = "No, I don't need that.";
			link.l2.go = "Meet_2";
		break;

		case "Auction_InfoExtra":
			if (!CheckAttribute(NPChar,"GWIK.AuctionExtra") || CheckAttribute(NPChar,"GWIK.AuctionExtra.SoldOut"))
			{
				dialog.text = "That lot has already been sold, Captain.";
				link.l1 = "A pity.";
				link.l1.go = "Meet_2";
				break;
			}
			if (NPChar.GWIK.AuctionExtra == "goods")
			{
				iGood = sti(NPChar.GWIK.AuctionExtra.Good);
				aQty = sti(NPChar.GWIK.AuctionExtra.Qty);
				aPrice = sti(NPChar.GWIK.AuctionExtra.Price);
				dialog.text = "For shareholders - one more cargo: "+ XI_ConvertString(Goods[iGood].name) +", "+ aQty +" units at "+ FindRussianMoneyString(aPrice) +" per "+ Goods[iGood].Units +" units. The whole lot - "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, aQty, aPrice)) +".";
				if (GWIK_GoodsAffordable(iGood, sti(pchar.money), aPrice) > 0)
				{
					link.l1 = "I'll take it! As much as fits in the hold.";
					link.l1.go = "Auction_BuyGoodsExtra";
				}
			}
			else
			{
				rItem = ItemsFromID(NPChar.GWIK.AuctionExtra.Item);
				aPrice = sti(NPChar.GWIK.AuctionExtra.Price);
				sItemName = GetConvertStr(rItem.name, "ItemsDescribe.txt");
				dialog.text = "For shareholders - one more piece from a captain's cabin: "+ sItemName +". The price is "+ FindRussianMoneyString(aPrice) +".";
				if (sti(pchar.money) >= aPrice)
				{
					link.l1 = "I'll take it, no questions asked!";
					link.l1.go = "Auction_BuyItemExtra";
				}
			}
			link.l2 = "No, I don't need that.";
			link.l2.go = "Meet_2";
		break;

		case "Auction_BuyGoods":
			iGood = sti(NPChar.GWIK.Auction.Good);
			aQty = sti(NPChar.GWIK.Auction.Qty);
			aPrice = sti(NPChar.GWIK.Auction.Price);
			toBuy = GWIK_GoodsAffordable(iGood, sti(pchar.money), aPrice); // how much we can afford
			if (toBuy > aQty) toBuy = aQty;
			added = GWIK_LoadGoodsSquadron(pchar, iGood, toBuy); // how much fits in the holds of the flagship and squadron
			if (added <= 0)
			{
				dialog.text = "I'm afraid there's no room left in your hold even for a single sack, Captain. A pity, it's a fine lot.";
				link.l1 = "A pity indeed...";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -GWIK_GoodsMoney(iGood, added, aPrice));
			PlayStereoSound("notebook");
			if (added < aQty)
			{
				NPChar.GWIK.Auction.Qty = aQty - added; // Gregg: the remainder of the lot stays on sale instead of burning
				dialog.text = "Your hold took "+ added +" units - that's "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, added, aPrice)) +". The Company will hold the rest of the lot ("+ (aQty - added) +" units) for you.";
			}
			else
			{
				NPChar.GWIK.Auction.SoldOut = true;
				dialog.text = "The whole lot is yours, Captain: "+ added +" units for "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, added, aPrice)) +". An excellent deal!";
			}
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Auction_BuyItem":
			aPrice = sti(NPChar.GWIK.Auction.Price);
			if (sti(pchar.money) < aPrice)
			{
				dialog.text = "You don't have enough money, Captain.";
				link.l1 = "A pity.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -aPrice);
			GiveItem2Character(pchar, NPChar.GWIK.Auction.Item);
			NPChar.GWIK.Auction.SoldOut = true;
			PlayStereoSound("notebook");
			dialog.text = "Congratulations on your purchase! The piece is in working order - its former owner, alas, is in no position to complain.";
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Auction_BuyGoodsExtra":
			iGood = sti(NPChar.GWIK.AuctionExtra.Good);
			aQty = sti(NPChar.GWIK.AuctionExtra.Qty);
			aPrice = sti(NPChar.GWIK.AuctionExtra.Price);
			toBuy = GWIK_GoodsAffordable(iGood, sti(pchar.money), aPrice); // how much we can afford
			if (toBuy > aQty) toBuy = aQty;
			added = GWIK_LoadGoodsSquadron(pchar, iGood, toBuy); // how much fits in the holds of the flagship and squadron
			if (added <= 0)
			{
				dialog.text = "I'm afraid there's no room left in your hold even for a single sack, Captain. A pity, it's a fine lot.";
				link.l1 = "A pity indeed...";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -GWIK_GoodsMoney(iGood, added, aPrice));
			PlayStereoSound("notebook");
			if (added < aQty)
			{
				NPChar.GWIK.AuctionExtra.Qty = aQty - added; // Gregg: the remainder of the lot stays on sale instead of burning
				dialog.text = "Your hold took "+ added +" units - that's "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, added, aPrice)) +". The Company will hold the rest of the lot ("+ (aQty - added) +" units) for you.";
			}
			else
			{
				NPChar.GWIK.AuctionExtra.SoldOut = true;
				dialog.text = "The whole lot is yours, Captain: "+ added +" units for "+ FindRussianMoneyString(GWIK_GoodsMoney(iGood, added, aPrice)) +". An excellent deal!";
			}
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Auction_BuyItemExtra":
			aPrice = sti(NPChar.GWIK.AuctionExtra.Price);
			if (sti(pchar.money) < aPrice)
			{
				dialog.text = "You don't have enough money, Captain.";
				link.l1 = "A pity.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -aPrice);
			GiveItem2Character(pchar, NPChar.GWIK.AuctionExtra.Item);
			NPChar.GWIK.AuctionExtra.SoldOut = true;
			PlayStereoSound("notebook");
			dialog.text = "Congratulations on your purchase! The piece is in working order - its former owner, alas, is in no position to complain.";
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Auction_Elite": // Gregg: exclusive lots for the Company's partners (share lvl 3)
			bMap = false;
			if (GetCharacterItem(pchar,"Map_Best") < 1 && drand(29) == 0) bMap = true;
			bTube = false;
			if (GetCharacterItem(pchar,"spyglass5") < 1 && drand(29) == 15) bTube = true;
			bTreasure = false;
			if (GetCharacterItem(pchar,"map_full") < 1 && drand(29) == 28) bTreasure = true;
			if (!bMap && !bTube && !bTreasure)
			{
				dialog.text = "You've already taken everything the Company kept for its partners, Captain.";
				link.l1 = "Understood. Goodbye.";
				link.l1.go = "Meet_2";
				break;
			}
			dialog.text = "For the Company's partners - pieces from special vaults, never put up for the common trade:";
			if (bMap)
			{
				link.l1 = "The full map of the archipelago - 500000 piastres.";
				link.l1.go = "Auction_BuyMap";
			}
			if (bTube)
			{
				link.l2 = "An exclusive spyglass - 350000 piastres.";
				link.l2.go = "Auction_BuyTube";
			}
			if (bTreasure)
			{
				link.l2 = "Treasure map - 30000 piastres.";
				link.l2.go = "Auction_BuyTreasure";
			}
			link.l3 = "Another time.";
			link.l3.go = "Meet_2";
		break;

		case "Auction_BuyMap":
			if (sti(pchar.money) < 500000)
			{
				dialog.text = "You don't have that much on you, Captain. The map costs 500000 piastres.";
				link.l1 = "A pity.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -500000);
			GiveItem2Character(pchar, "Map_Best");
			PlayStereoSound("notebook");
			dialog.text = "An excellent choice! This map was drawn by the Company's finest cartographers - the whole archipelago down to the last shoal and reef. Keep it safe.";
			link.l1 = "Priceless. Thank you!";
			link.l1.go = "exit";
		break;

		case "Auction_BuyTube":
			if (sti(pchar.money) < 350000)
			{
				dialog.text = "You don't have that much on you, Captain. The spyglass costs 350000 piastres.";
				link.l1 = "A pity.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -350000);
			GiveItem2Character(pchar, "spyglass5");
			PlayStereoSound("notebook");
			dialog.text = "A fine piece! Such spyglasses are made one at a time - through it you'll make out even the upgrades on another's ship. A great rarity.";
			link.l1 = "Just what I needed. Thank you!";
			link.l1.go = "exit";
		break;
		
		case "Auction_BuyTreasure":
			if (sti(pchar.money) < 30000)
			{
				dialog.text = "You don't have that much on you, Captain. The map costs 30000 piastres.";
				link.l1 = "A pity.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -30000);
			GiveTreasureMapToPlayer();
			PlayStereoSound("notebook");
			dialog.text = "An excellent choice! I hope you would get a good bounty there.";
			link.l1 = "Thank you!";
			link.l1.go = "exit";
		break;

		case "Bill_Ask":
			dialog.text = "The Company's bill of exchange service is at your disposal. You deposit a sum here - and receive a bearer's bill payable at any of our other offices. No need to risk hauling gold through pirate-infested waters. The commission is "+ billFee +"% upon issue, the minimum sum is 10000 piastres. How much shall we deposit?";
			link.l1.edit = 1;
			link.l1 = "";
			link.l1.go = "Bill_Calc";
			link.l2 = "I've changed my mind. Another time.";
			link.l2.go = "Meet_2";
		break;

		case "Bill_Calc":
			billSum = sti(dialogEditStrings[1]);
			if (billSum < 10000)
			{
				dialog.text = "A bill cannot be drawn for less than 10000 piastres, Captain. Name another sum.";
				link.l1 = "All right, let's try again.";
				link.l1.go = "Bill_Ask";
				link.l2 = "Then never mind.";
				link.l2.go = "Meet_2";
				break;
			}
			if (sti(pchar.money) < billSum)
			{
				dialog.text = "You don't have that much on you, Captain. Name a more modest sum.";
				link.l1 = "All right, let's try again.";
				link.l1.go = "Bill_Ask";
				link.l2 = "Then never mind.";
				link.l2.go = "Meet_2";
				break;
			}
			billPay = billSum - makeint(billSum * billFee / 100);
			dialog.text = "You deposit "+ FindRussianMoneyString(billSum) +", and "+ FindRussianMoneyString(billPay) +" will be paid out at any of our other offices. Shall I draw it up?";
			link.l1 = "Yes, here's the money.";
			link.l1.go = "Bill_Buy";
			link.l2 = "No, I've changed my mind.";
			link.l2.go = "Meet_2";
		break;

		case "Bill_Buy":
			billSum = sti(dialogEditStrings[1]);
			if (billSum < 10000 || sti(pchar.money) < billSum)
			{
				dialog.text = "Something doesn't add up, Captain. Let's start over.";
				link.l1 = "All right.";
				link.l1.go = "Bill_Ask";
				break;
			}
			AddMoneyToCharacter(pchar, -billSum);
			pchar.GWIK.Bill = billSum - makeint(billSum * billFee / 100);
			pchar.GWIK.Bill.From = NPChar.id;
			pchar.GWIK.Bill.Date = GetQuestBookDataDigit(); // printed in the ship's log
			PlayStereoSound("notebook");
			dialog.text = "Done! A bill for "+ FindRussianMoneyString(sti(pchar.GWIK.Bill)) +" is in your papers, and the entry has been made in your ship's log. Present it at any of our other offices, and you will be paid every last coin.";
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Bill_Cash":
			cash = sti(pchar.GWIK.Bill);
			AddMoneyToCharacter(pchar, cash);
			DeleteAttribute(pchar, "GWIK.Bill");
			PlayStereoSound("notebook");
			dialog.text = "The bill is in perfect order. Here are your "+ FindRussianMoneyString(cash) +", every last coin, just as the Company promised.";
			link.l1 = "A pleasure doing business with you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "Bill_Where":
			dialog.text = "Your bill was drawn by this office, and it will be paid at any of our other offices. Those are the Company's rules - otherwise what would be the point of transferring money?";
			link.l1 = "Right, thank you.";
			link.l1.go = "Meet_2";
		break;

		case "UniLicense_Ask":
			dialog.text = "Our Company's universal trade license lets you enter the cities of any nation freely - even one hostile to you - as long as your ship flies that nation's flag. You set the term yourself, but no less than ten days. The price is "+ FindRussianMoneyString(uniPerDay) +" per day. For how many days shall I make it out?";
			link.l1.edit = 1;
			link.l1 = "";
			link.l1.go = "UniLicense_Calc";
			link.l2 = "I've changed my mind. Another time.";
			link.l2.go = "Meet_2";
		break;

		case "UniLicense_Calc":
			uniDays = sti(dialogEditStrings[1]);
			if (uniDays < 10)
			{
				dialog.text = "I'm sorry, Captain, but I cannot issue a license for less than ten days. Name another term.";
				link.l1 = "Alright, let's try again.";
				link.l1.go = "UniLicense_Ask";
				link.l2 = "Never mind, then.";
				link.l2.go = "Meet_2";
				break;
			}
			if (uniDays > 3650) uniDays = 3650; // Gregg: upper cap of 10 years - guards against int overflow (negative price would credit money)
			uniPrice = uniDays * uniPerDay;
			if (sti(pchar.money) >= uniPrice)
			{
				dialog.text = "A license for "+ uniDays +" days will cost you "+ FindRussianMoneyString(uniPrice) +". Shall I make it out?";
				link.l1 = "Yes, here's the money.";
				link.l1.go = "UniLicense_Buy";
			}
			else
			{
				dialog.text = "A license for "+ uniDays +" days would cost "+ FindRussianMoneyString(uniPrice) +", but you don't have that much on you, Captain.";
			}
			link.l2 = "I'll change the term.";
			link.l2.go = "UniLicense_Ask";
			link.l3 = "On second thought, never mind.";
			link.l3.go = "Meet_2";
		break;

		case "UniLicense_Buy":
			uniDays = sti(dialogEditStrings[1]);
			if (uniDays < 10) uniDays = 10;
			if (uniDays > 3650) uniDays = 3650; // Gregg: upper cap of 10 years - guards against int overflow
			uniPrice = uniDays * uniPerDay;
			if (sti(pchar.money) < uniPrice)
			{
				dialog.text = "You don't have enough money for such a license, Captain.";
				link.l1 = "A pity.";
				link.l1.go = "Meet_2";
				break;
			}
			AddMoneyToCharacter(pchar, -uniPrice);
			GiveUniLicence(uniDays);
			PlayStereoSound("notebook");
			dialog.text = "Done! Here is your universal trade license, valid for "+ uniDays +" days - it's in your papers now. With it you may enter the cities of any nation, so long as you fly its flag. Good luck with your trade, Captain!";
			link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
		break;

		case "trade_info":
			dialog.text = "Yes, I have such information and I can share it. That's "+ FindRussianMoneyString(price) +".";
			if (sti(pchar.money) >= price)
			{
				Link.l1 = "Okay, here's your money.";
				link.l1.go = "trade_info_2";
			}
			Link.l2 = "I've changed my mind. Have a nice day.";
			link.l2.go = "exit";
		break;

		case "trade_info_2":
			for (i = 0; i< MAX_COLONIES; i++)
			{
                ref rcolony = &Colonies[i];
			    if (rcolony.id != "Caiman" && rcolony.id != "Panama" && rcolony.nation != "none") SetPriceListByStoreMan(rcolony);
            }
			log_info("Received a list of prices in all stores in the Caribbean Archipelago.");
			PlayStereoSound("notebook");
			AddMoneyToCharacter(pchar,-price);

			dialog.text = "...Here you go, all the information you need is here. I wish you success!";
			Link.l1 = "Thank you. Goodbye!";
			link.l1.go = "exit";
			pchar.GwikTimer = true;
			SetTimerFunction("RefreshGWIK", 0, 0, 15);
		break;
	}
}

string findTraderCity_GWIK(ref NPChar) // colony for escorting a Company ship (not hostile to Holland, not the current island)
{
	int n;
	int cityArray[MAX_COLONIES];
	int howCity = 0;
	for (n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].id == "Panama") continue;
		if (colonies[n].nation == "none") continue;
		if (GetIslandByCityName(npchar.city) == colonies[n].islandLable) continue;
		if (GetNationRelation(HOLLAND, sti(colonies[n].nation)) == RELATION_ENEMY) continue;
		cityArray[howCity] = n;
		howCity++;
	}
	if (howCity == 0) return "none";
	return colonies[cityArray[cRand(howCity-1)]].id;
}

string findRivalCity_GWIK(ref NPChar) // a rival colony (England/France), not the current island
{
	int n;
	int cityArray[MAX_COLONIES];
	int howCity = 0;
	bool okNation;
	for (n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].id == "Panama") continue;
		if (colonies[n].nation == "none") continue;
		if (GetIslandByCityName(npchar.city) == colonies[n].islandLable) continue;
		okNation = (sti(colonies[n].nation) == ENGLAND) || (sti(colonies[n].nation) == FRANCE);
		if (!okNation) continue;
		cityArray[howCity] = n;
		howCity++;
	}
	if (howCity == 0) return "none";
	return colonies[cityArray[cRand(howCity-1)]].id;
}
