// Company envoy in Fort Orange: quest to open the GWIC office on Jamaica (Gregg)

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int stage = 0;
	if (CheckAttribute(pchar, "GWIK.Jamaica.Stage")) stage = sti(pchar.GWIK.Jamaica.Stage);
	// Gregg: the "word" path opens at high standing; the commandant's word (MayorOk) lowers the bar - the town has spoken for the Company
	bool bWord = false;
	if (GWIK_RepLevel() >= 4) bWord = true;
	if (CheckAttribute(pchar, "GWIK.Jamaica.MayorOk") && GWIK_RepLevel() >= 3) bWord = true;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			if (CheckAttribute(pchar, "GWIK.Jamaica.Done")) // quest finished - point to the permanent representative
			{
				dialog.text = "The office is open, Captain, and largely thanks to you. For Company business, see our representative now - he's inside the office itself.";
				link.l1 = "Good day.";
				link.l1.go = "exit";
				break;
			}
			if (stage == 1) // briefing -> intercept the courier
			{
				dialog.text = "You're from the directors? At last, Captain. I am "+ GetFullName(NPChar) +", the Company's trusted man in these parts. It's a delicate affair - not the sort one speaks of in the street.";
				link.l1 = "I'm all ears. What is it?";
				link.l1.go = "Jam_Brief1";
				link.l2 = "Let me look around the town first.";
				link.l2.go = "exit";
				break;
			}
			if (stage == 2) // intercept the courier
			{
				if (CheckAttribute(pchar, "GWIK.Jamaica.CourierDone"))
				{
					dialog.text = "The courier's at the bottom? Excellent. The report won't arrive - we've bought time. But the office won't open itself: now, about the supplies.";
					link.l1 = "I'm listening.";
					link.l1.go = "Courier_Done";
					break;
				}
				if (CheckAttribute(pchar, "GWIK.Jamaica.CourierEscaped")) // courier reached port - the denunciation got through, offer a bribe
				{
					dialog.text = "The courier slipped into port, and the denunciation is on the governor's desk. But all is not lost: such papers have a way of going missing when a heavy purse finds the right hands. One hundred thousand piastres, and the report will be forgotten.";
					if (sti(pchar.money) >= 100000)
					{
						link.l1 = "Here's the money. See to it.";
						link.l1.go = "Courier_Bribe";
					}
					link.l2 = "I don't have that much right now.";
					link.l2.go = "exit";
					break;
				}
				dialog.text = "The courier is still sailing our waters, Captain - the light corvette 'Swift' under Captain Page. Find it at sea off Jamaica and send it down - denunciation and all. And be quick about it.";
				link.l1 = "I'm off to intercept it.";
				link.l1.go = "exit";
				break;
			}
			if (stage == 3) // supply: escort a convoy from Curacao
			{
				if (CheckAttribute(pchar, "GWIK.Jamaica.ConvoyArrived")) // convoy delivered - take the cargo and move to the factor
				{
					if (sti(pchar.GWIK.Jamaica.ConvoyArrived) >= 2)
						dialog.text = "Both galleons are in the harbour, the holds being unloaded - you brought the convoy through without a single loss! Now the office has its stores in full. Only an experienced manager is still wanting.";
					else
						dialog.text = "Your galleon is in the harbour... and the other, then, is on the bottom. Well, half the cargo is better than none: the office has its stores. Only an experienced manager is still wanting.";
					link.l1 = "What manager?";
					link.l1.go = "Supply_Deliver";
					break;
				}
				if (CheckAttribute(pchar, "GWIK.Jamaica.Convoy")) // escort in progress
				{
					dialog.text = "The convoy is still at sea? Guard those galleons, Captain - without the cargo the office can't stand. And keep your guns ready: English corvettes are on the convoy's tail.";
					link.l1 = "I'll get them through.";
					link.l1.go = "exit";
					break;
				}
				dialog.text = "The office is empty - it needs goods and capital. At Willemstad, on Curacao, our manager is already loading two galleons with precious timber and silk. Go there, take the convoy under escort and bring it to Fort Orange. The road is uneasy: word of the cargo will send the competition's corvettes after you.";
				link.l1 = "I'll set out for the convoy at Curacao.";
				link.l1.go = "exit";
				break;
			}
			if (stage == 4) // the Company factor
			{
				if (CheckAttribute(pchar, "GWIK.Jamaica.FactorAboard"))
				{
					dialog.text = "Our factor is aboard your ship? Splendid. Let him stay under your protection for now - there's no place for him in Fort Orange until the office is open."; // fallback for old saves without a stored name
					if (CheckAttribute(pchar, "GWIK.Jamaica.FactorLName")) dialog.text = "Mister "+ pchar.GWIK.Jamaica.FactorLName +" is aboard your ship? Splendid. Let him stay under your protection for now - there's no place for him in Fort Orange until the office is open.";
					link.l1 = "Understood. What next?";
					link.l1.go = "Factor_Deliver";
					break;
				}
				dialog.text = "The office needs an experienced manager. Our factor waits at the Company office on Saint Martin, in Marigo - collect him and bring him here. And stay sharp: our people have seen an English corvette circling these shores for three days now. The competition never sleeps.";
				link.l1 = "I'll bring the factor.";
				link.l1.go = "exit";
				break;
			}
			if (stage == 5) // finale: opening with a complication + choice of tone
			{
				if (CheckAttribute(pchar, "GWIK.Jamaica.FinalDone"))
				{
					dialog.text = "Gale's 'Greyhound' is at the bottom - you should have seen the English faces! Now no one will doubt whose flag flies over this office. Time to open it - this is your victory, Captain.";
					link.l1 = "Let's open it!";
					link.l1.go = "Final_OpenForce";
					break;
				}
				if (CheckAttribute(pchar, "GWIK.Jamaica.Tone") && pchar.GWIK.Jamaica.Tone == "force")
				{
					dialog.text = "The privateer Jeffrey Gale on the 'Greyhound' is still prowling off Jamaica. Send him to the bottom and come back - we'll open the office with the flag held high.";
					link.l1 = "Off to sink it.";
					link.l1.go = "exit";
					break;
				}
				dialog.text = "The factor's here, the goods are in the warehouse - one step remains. But the competition won't give up so easily: they'll surely try to wreck the opening. How shall we deal with them?";
				link.l1 = "Meet them with cannon - let them learn who we are.";
				link.l1.go = "Tone_Force";
				link.l2 = "Buy them off, it'll come cheaper.";
				link.l2.go = "Tone_Gold";
				if (bWord)
				{
					link.l3 = "We'll settle it with words - I have a name and connections.";
					link.l3.go = "Tone_Word";
				}
				link.l4 = "Let me think.";
				link.l4.go = "exit";
				break;
			}
			dialog.text = "No orders for now, Captain. Look in later.";
			link.l1 = "I will.";
			link.l1.go = "exit";
		break;

		case "Jam_Brief1":
			dialog.text = "Opening a Company office here on Jamaica is like lighting a fire under the nose of the English at Port Royal. And they've already caught a whiff of it: our man in the tavern saw the governor send off a courier with a denunciation to another English port. If those papers arrive, they'll smother the venture in its cradle.";
			link.l1 = "And what do you propose?";
			link.l1.go = "Jam_Brief2";
			link.l2 = "What's the risk to me, personally?";
			link.l2.go = "Jam_BriefRisk";
		break;

		case "Jam_BriefRisk":
			dialog.text = "Officially the Company has nothing to do with it, and you're a free captain - things happen at sea. But the Heeren XVII do not forget such a service: an office, a founder's share, a good name. You risk no more than in any other affair.";
			link.l1 = "Very well. What about the courier?";
			link.l1.go = "Jam_Brief2";
		break;

		case "Jam_Brief2":
			dialog.text = "Intercept the courier at sea before it reaches port, and send it to the bottom, papers and all. Quietly and for certain.";
			link.l1 = "Where do I find it?";
			link.l1.go = "Brief_Courier";
			link.l2 = "Let me prepare.";
			link.l2.go = "exit";
		break;

		case "Brief_Courier":
			if (!GWIK_JamaicaSpawnCourier()) // courier didn't put to sea - don't dead-end the quest
			{
				dialog.text = "Strange - our informant has lost sight of the courier. Look in a little later and I'll pin down the route.";
				link.l1 = "All right, I'll come back.";
				link.l1.go = "exit";
				break;
			}
			pchar.GWIK.Jamaica.Stage = 2;
			AddQuestRecord("GWIK_JamaicaJob", "2");
			dialog.text = "The courier is the light corvette 'Swift' under Captain Jeremy Page - it has already left the shores of Jamaica for an English port. Don't let it get there. And remember: officially the Company has nothing to do with this.";
			link.l1 = "Consider it done.";
			link.l1.go = "exit";
		break;

		case "Courier_Done":
			pchar.GWIK.Jamaica.Stage = 3;
			DeleteAttribute(pchar, "GWIK.Jamaica.CourierDone");
			AddQuestRecord("GWIK_JamaicaJob", "4");
			dialog.text = "At Willemstad, on Curacao, our manager is already loading two galleons with precious timber and silk. Go there, take the convoy under escort and bring it to Fort Orange. The road is uneasy: word of the cargo will send the competition's corvettes after you.";
			link.l1 = "I'll set out for the convoy at Curacao.";
			link.l1.go = "exit";
		break;

		case "Courier_Bribe":
			if (sti(pchar.money) < 100000)
			{
				dialog.text = "The bribe needs one hundred thousand piastres, and you don't have that much on you, Captain.";
				link.l1 = "I'll find the money.";
				link.l1.go = "exit";
				break;
			}
			AddMoneyToCharacter(pchar, -100000);
			DeleteAttribute(pchar, "GWIK.Jamaica.CourierEscaped");
			pchar.GWIK.Jamaica.Stage = 3;
			PlayStereoSound("notebook");
			AddQuestRecord("GWIK_JamaicaJob", "12");
			dialog.text = "Done. The denunciation has drowned in other men's papers, as if it never was. Now, about the supplies: at Willemstad, on Curacao, they're already loading two galleons with timber and silk. Go there, take the convoy under escort and bring it to Fort Orange - the competition will surely send corvettes after you.";
			link.l1 = "I'll set out for the convoy at Curacao.";
			link.l1.go = "exit";
		break;

		case "Supply_Deliver": // Gregg: convoy delivered - supply done, move to the factor stage
			if (CheckAttribute(pchar, "GWIK.Jamaica.ConvoyArrived") && sti(pchar.GWIK.Jamaica.ConvoyArrived) >= 2) GWIK_AddRep(10); // Gregg: bonus for a convoy delivered without loss
				DeleteAttribute(pchar, "GWIK.Jamaica.ConvoyArrived");
			pchar.GWIK.Jamaica.Stage = 4;
			PlayStereoSound("notebook");
			AddCharacterExpToSkill(pchar, SKILL_SAILING, 60);    // Gregg: navigation xp for running the supply convoy through pursuit
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 30); // Gregg: and command xp - escorting the squadron to Fort Orange
			AddQuestRecord("GWIK_JamaicaJob", "5");
			dialog.text = "The timber and silk from your galleons will go into the venture - the office has its stores. Now it needs a manager: our factor waits at the office on Saint Martin, in Marigo. Bring him here.";
			link.l1 = "I'll bring him.";
			link.l1.go = "exit";
		break;

		case "Factor_Deliver":
			pchar.GWIK.Jamaica.Stage = 5;
			GWIK_JamaicaClearAmbush(); // if the interceptor was merely outrun, it has failed - remove it quietly
			AddCharacterExpToSkill(pchar, SKILL_SAILING, 50);    // Gregg: navigation xp for delivering the factor past the ambush
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 20); // Gregg: and a little command xp
			AddQuestRecord("GWIK_JamaicaJob", "7");
			dialog.text = "The factor is already inspecting the future warehouses - excellent. One step remains: the opening. But the competition won't give up easily - they'll surely try to wreck it. How shall we deal with them?"; // fallback for old saves without a stored name
			if (CheckAttribute(pchar, "GWIK.Jamaica.FactorLName")) dialog.text = "Mister "+ pchar.GWIK.Jamaica.FactorLName +" is already inspecting the future warehouses - excellent. One step remains: the opening. But the competition won't give up easily - they'll surely try to wreck it. How shall we deal with them?";
			link.l1 = "Meet them with cannon - let them learn who we are.";
			link.l1.go = "Tone_Force";
			link.l2 = "Buy them off, it'll come cheaper.";
			link.l2.go = "Tone_Gold";
			if (bWord)
			{
				link.l3 = "We'll settle it with words - I have a name and connections.";
				link.l3.go = "Tone_Word";
			}
			link.l4 = "Let me think.";
			link.l4.go = "exit";
		break;

		case "Tone_Force":
			if (!GWIK_JamaicaSpawnFinal()) // privateer didn't spawn - don't latch the force path, keep the tone choice open
			{
				dialog.text = "Strange, the privateer isn't showing off the coast. Look in later - or we'll settle this another way.";
				link.l1 = "All right.";
				link.l1.go = "exit";
				break;
			}
			pchar.GWIK.Jamaica.Tone = "force";
			AddQuestRecord("GWIK_JamaicaJob", "8");
			dialog.text = "That's the spirit! The English have hired a privateer - one Jeffrey Gale on the corvette 'Greyhound', already bound for Jamaica. Meet him at sea and send him down. Then come back, and we'll open the office openly, for all to see.";
			link.l1 = "It will be done.";
			link.l1.go = "exit";
		break;

		case "Tone_Gold":
			if (sti(pchar.money) < 80000)
			{
				dialog.text = "The bribe needs eighty thousand piastres, and you don't have that on you, Captain.";
				link.l1 = "I'll find the money.";
				link.l1.go = "exit";
				break;
			}
			AddMoneyToCharacter(pchar, -80000);
			pchar.GWIK.Jamaica.Tone = "gold";
			GWIK_FinishJamaicaQuest(0);
			if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) // the opening fell through - refund, don't burn the choice
			{
				AddMoneyToCharacter(pchar, 80000);
				DeleteAttribute(pchar, "GWIK.Jamaica.Tone");
				dialog.text = "A hitch with the papers, Captain... Your money stays with you for now. Look in a little later - we'll sort it out.";
				link.l1 = "Very well.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Wise. A few heavy purses in the right hands - and the English will look away at the right moment. It's done: the office is open! The Company won't forget your part in this.";
			link.l1 = "Glad for the Company.";
			link.l1.go = "exit";
		break;

		case "Tone_Word":
			pchar.GWIK.Jamaica.Tone = "word";
			GWIK_FinishJamaicaQuest(0);
			if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) // the opening fell through - don't burn the choice
			{
				DeleteAttribute(pchar, "GWIK.Jamaica.Tone");
				dialog.text = "A hitch with the papers, Captain... Look in a little later - we'll sort it out.";
				link.l1 = "Very well.";
				link.l1.go = "exit";
				break;
			}
			GWIK_AddRep(20); // bonus for the hardest path - only on success
			AddCharacterExpToSkill(pchar, SKILL_LEADERSHIP, 100); // Gregg: large leadership xp for the purely diplomatic finale (the hardest path)
			dialog.text = "Your name truly opens doors. A few words to the right people - and the storm settled before it began. The office is open, and without a single shot. The Company values this highly.";
			link.l1 = "Much better this way.";
			link.l1.go = "exit";
		break;

		case "Final_OpenForce":
			GWIK_FinishJamaicaQuest(15);
			if (!CheckAttribute(pchar, "GWIK.Jamaica.Done")) // the opening fell through - don't lose the finale
			{
				dialog.text = "A hitch with the papers, Captain... Look in a little later - we'll sort it out.";
				link.l1 = "Very well.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "It's done - the West India Company office in Fort Orange is open, and under our flag! This is your victory, Captain. The Company won't forget it.";
			link.l1 = "For the Company!";
			link.l1.go = "exit";
		break;

		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
