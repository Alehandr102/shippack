// Pirate go-between: ransom of the captured GWIC captain (office assignment)

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iRansom = 0;
	if (CheckAttribute(pchar, "GWIK.Job.Ransom")) iRansom = sti(pchar.GWIK.Job.Ransom);
	if (CheckAttribute(pchar, "GWIK.Job.RansomActual")) iRansom = sti(pchar.GWIK.Job.RansomActual); // price after haggling
	int comSkill, nomRansom, leadSkill;
	bool ok;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			ok = false;
			if (CheckAttribute(pchar, "GWIK.Job") && pchar.GWIK.Job == "ransom") ok = true;
			if (CheckAttribute(pchar, "GWIK.Job.Rescued")) ok = false;
			if (!ok)
			{
				dialog.text = "What are you staring at? Move along while you're still in one piece.";
				link.l1 = "Already gone...";
				link.l1.go = "exit";
				break;
			}
			if (CheckAttribute(pchar, "GWIK.Job.CaptiveDead")) // Gregg: rare outcome - the captive got himself killed. The pirate only INFORMS you (no coin - why would he); the token compensation for the trouble is paid by the COMPANY when you report at an office (node GWIK_RansomDeadReport)
			{
				if (!CheckAttribute(pchar, "GWIK.Job.CaptiveDeadKnown"))
				{
					pchar.GWIK.Job.CaptiveDeadKnown = true; // now report the death at a GWIC office
					AddQuestRecord("GWIK_RansomJob", "10");
					dialog.text = "Too late, Dutchman. Your captain had too sharp a tongue - the lads lost patience and bled him. Off you go, break the news to your masters - it's no business of mine.";
				}
				else
				{
					dialog.text = "I already told you - your captain's feeding the crabs on the bottom. Go to your masters, there's nothing more for you here.";
				}
				link.l1 = "Damn it...";
				link.l1.go = "exit";
				break;
			}
			if (CheckAttribute(pchar, "GWIK.Job.Force")) // chose force - the sloop is already at sea, nothing to haggle
			{
				dialog.text = "You've made your choice, Dutchman. The ship '"+ pchar.GWIK.Job.ForceShipName +"' is at sea off Bermuda - your captain's aboard her. Go and catch her, if you've the nerve.";
				link.l1 = "On my way.";
				link.l1.go = "exit";
				break;
			}
			if (CheckAttribute(pchar, "GWIK.Job.Mutiny")) // Gregg: rare go-between twist - there's no one to ransom: the Company's captain mutinied and turned pirate himself. He's the one sitting before you
			{
				if (CheckAttribute(pchar, "GWIK.Job.MutinyReturnedSelf")) // already persuaded - the runaway left for an office on his own, nothing more to do here (safety net if the deferred departure didn't fire)
				{
					dialog.text = "We've settled everything, Dutchman. Off to the office with you - I'll see you there.";
					link.l1 = "See you there.";
					link.l1.go = "exit";
					break;
				}
				if (!CheckAttribute(pchar, "GWIK.Job.MutinyMet"))
				{
					pchar.GWIK.Job.MutinyMet = true;
					AddQuestRecord("GWIK_RansomJob", "12"); // reveal: no captive - the captain turned coat
					dialog.text = "Ransom? Ha-ha-ha! Look around, Dutchman: your captain - here I am, in the flesh. I got sick of breaking my back for the Company for a pittance - now I'm my own master under the black flag. So off you go, cheer your masters up: there's no one to ransom.";
				}
				else
				{
					dialog.text = "You again? I've said my piece: you couldn't lure me back to the Company with all the gold in Cartagena.";
				}
				if (!CheckAttribute(pchar, "GWIK.Job.MutinyRefused")) // first - an attempt to persuade (Leadership); refusal opens the 'report' and 'take by force' paths
				{
					link.l1 = "Come to your senses. You're a fine captain - the Company will forgive you and take you back. Return the peaceful way.";
					link.l1.go = "Mutiny_Persuade";
					link.l2 = "Well, it's your choice. I need to think this over.";
					link.l2.go = "exit";
				}
				else
				{
					link.l1 = "If you won't come quietly - the Company will learn you're a traitor. (report)";
					link.l1.go = "Mutiny_Report";
					link.l2 = "You won't come the easy way - then I'll take you by force. Put out to sea, we'll settle it there.";
					link.l2.go = "Mutiny_Threaten";
					link.l3 = "I need to think this over.";
					link.l3.go = "exit";
				}
				break;
			}
			dialog.text = "Ah, you're from the Dutch, here for their captain? Money up front: "+ FindRussianMoneyString(iRansom) +" - and you can take him right away.";
			if (CheckAttribute(pchar, "GWIK.Job.Bargained")) dialog.text = "Well, made up your mind? The price stands: "+ FindRussianMoneyString(iRansom) +", and the captain is yours.";
			if (sti(pchar.money) >= iRansom)
			{
				link.l1 = "Here's the ransom. Where's the captain?";
				link.l1.go = "pay";
			}
			else
			{
				link.l1 = "I don't have that much on me...";
				link.l1.go = "exit";
			}
			if (!CheckAttribute(pchar, "GWIK.Job.Bargained")) // haggle - once only
			{
				link.l2 = "For God's sake, he's not worth that. Come down a bit.";
				link.l2.go = "bargain";
			}
			link.l3 = "Pay pirates? I think not. I'll take the captain myself.";
			link.l3.go = "force_warn";
			link.l4 = "I'll think about it.";
			link.l4.go = "exit";
		break;

		case "bargain": // Gregg: haggling - Commerce check; the office reimburses the NOMINAL, the saving stays with the player
			pchar.GWIK.Job.Bargained = true;
			comSkill = GetSummonSkillFromName(pchar, SKILL_COMMERCE);
			nomRansom = sti(pchar.GWIK.Job.Ransom);
			if (comSkill >= 60) // Gregg: Commerce skill 0-100 (was hardcoded 8 - fired almost always)
			{
				pchar.GWIK.Job.RansomActual = makeint(nomRansom * 75 / 100);
				AddCharacterExpToSkill(pchar, SKILL_COMMERCE, 30); // Gregg: trade xp for a successful haggle (knocked a quarter off)
				dialog.text = "Bah, you can count, you Dutch soul... Fine, "+ sti(pchar.GWIK.Job.RansomActual) +" and we have a deal, just so I never see your face again.";
			}
			else
			{
				if (comSkill >= 35)
				{
					pchar.GWIK.Job.RansomActual = makeint(nomRansom * 85 / 100);
					AddCharacterExpToSkill(pchar, SKILL_COMMERCE, 20); // Gregg: trade xp for a haggle (knocked part of the price off)
					dialog.text = "Hmph, you haggle as well as we do... I'll knock off a little: "+ sti(pchar.GWIK.Job.RansomActual) +", and not a piastre less.";
				}
				else
				{
					pchar.GWIK.Job.RansomActual = makeint(nomRansom * 110 / 100);
					dialog.text = "Do you take me for a fool?! For that cheek - I'll add to it. Now it's "+ sti(pchar.GWIK.Job.RansomActual) +", and be glad it's not double.";
				}
			}
			link.l1 = "Deal.";
			link.l1.go = "First time";
		break;

		case "force_warn": // Gregg: force path - the go-between sneers and spirits the captain onto a sloop
			dialog.text = "Take him yourself, is it? Ha! Look here: while you're playing the hero, our captain's already aboard a ship, and she's weighing anchor. Catch her at sea and he's yours, fair enough. But send her to the bottom - and the Dutch won't thank you for a drowned man. Well, still want it your way?";
			link.l1 = "Aye. Let her sail - I've pulled better men out of the water.";
			link.l1.go = "force_go";
			link.l2 = "On second thought, let's do without blood. Back to business.";
			link.l2.go = "First time";
		break;

		case "force_go":
			if (!GWIK_RansomForceSpawn("Pirates")) // the sloop didn't put out - don't dead-end; patrol anchored off Bermuda (mediator text says "off Bermuda")
			{
				dialog.text = "Hmph, the ship's already gone, and where to - haven't the faintest. Look in later, I'll sniff it out.";
				link.l1 = "I'll be back.";
				link.l1.go = "exit";
				break;
			}
			AddQuestRecord("GWIK_RansomJob", "5");
			AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName); // strictly after AddQuestRecord
			AddQuestUserData("GWIK_RansomJob", "sCap", pchar.GWIK.Job.ForceCapName);
			dialog.text = "Your funeral, you reckless soul. The ship '"+ pchar.GWIK.Job.ForceShipName +"' under Captain "+ pchar.GWIK.Job.ForceCapName +" is already at sea off Bermuda, the captive in her hold. Go catch her - and remember what I said about the bottom.";
			link.l1 = "I'll find her and take him.";
			link.l1.go = "exit";
		break;

		case "pay":
			AddMoneyToCharacter(pchar, -iRansom);
			GWIK_RansomPay();
			AddQuestRecord("GWIK_RansomJob", "2");
			dialog.text = "There we go. The captain is already on your ship - my lads saw him aboard. And tell the Dutch: keep paying this promptly, and we'll be taking their people more often, heh-heh.";
			link.l1 = "Some sense of humour you have...";
			link.l1.go = "exit";
		break;

		case "Mutiny_Persuade": // Gregg: attempt to talk the runaway captain into returning peacefully (Leadership check)
			leadSkill = GetSummonSkillFromName(pchar, SKILL_LEADERSHIP);
			if (leadSkill >= 25 + sti(pchar.rank)) // Gregg: threshold = Leadership skill (0-100 scale, NOT the 1-10 attribute) >= 25 + player rank - scales with the player; the captain's authority won out, the runaway agrees to return
			{
				pchar.GWIK.Job.MutinyReturnedSelf = true; // he sails back on his own - NO passenger spawned (collect the reward by reporting at an office, node GWIK_MutinyReturnReport)
				AddDialogExitQuestFunction("GWIK_MutineerDepart"); // after the dialog closes the runaway leaves the tavern (deferred - nulling an interlocutor's location is only safe post-close; officer-hire pattern)
				AddQuestRecord("GWIK_RansomJob", "14");
				dialog.text = "...Ah, to the devil with it. You're right - I won't last long under this flag, and the Company at least pays on time. Fine, Dutchman, you win. I'll make my own way to an office and own up - I'll see you there.";
				link.l1 = "A wise choice. I'll expect you at the office.";
				link.l1.go = "exit";
			}
			else
			{
				pchar.GWIK.Job.MutinyRefused = true; // the peaceful path is closed - now only report or take by force
				dialog.text = "Fine words, Dutchman, but you won't talk me round. My mind's made up. Either clear off while you can, or take me by force - just don't expect easy pickings.";
				link.l1 = "So be it - the Company will learn you're a traitor. (report)";
				link.l1.go = "Mutiny_Report";
				link.l2 = "You won't come the easy way - then I'll take you by force. Put out to sea.";
				link.l2.go = "Mutiny_Threaten";
				link.l3 = "I need to think this over.";
				link.l3.go = "exit";
			}
		break;

		case "Mutiny_Threaten": // Gregg: threaten to take him - the runaway puts to sea hostile (force path; capture = captain recovered, sinking = failure)
			DeleteAttribute(pchar, "GWIK.Job.MutinyReportReady"); // changed his mind about reporting - going the force route
			if (!GWIK_RansomForceSpawn("Pirates")) // his ship puts out to sea off Bermuda (Force=true)
			{
				dialog.text = "My lads are deep in their cups just now, the ship's in no state to sail. Look in later - then we'll settle this the sailor's way.";
				link.l1 = "I'll be back.";
				link.l1.go = "exit";
				break;
			}
			AddQuestRecord("GWIK_RansomJob", "13");
			AddQuestUserData("GWIK_RansomJob", "sShip", pchar.GWIK.Job.ForceShipName); // strictly after AddQuestRecord
			AddQuestUserData("GWIK_RansomJob", "sType", pchar.GWIK.Job.ForceShipType); // Gregg: ship type instead of the captain's name - @<sCap> here is the PIRATE (random name), not the runaway; was misleading
			dialog.text = "Ha! No coward, are you. Very well, put out to sea, Dutchman - my crew's already aboard the '"+ pchar.GWIK.Job.ForceShipName +"'. Catch her and board her and you've won. If not - you'll feed the fish. But remember: send her to the bottom and you drown me too, and dead I'm no use to the Company.";
			link.l1 = "Put out to sea. I'll meet you there.";
			link.l1.go = "exit";
		break;

		case "Mutiny_Report": // Gregg: give up and report to the Company that the captain turned coat (a token compensation at the office)
			pchar.GWIK.Job.MutinyReportReady = true; // report at a GWIC office (node GWIK_MutinyReport)
			AddQuestRecord("GWIK_RansomJob", "15");
			dialog.text = "Run along, tattletale. Only your Company's no master of mine now - I'm a free man. Clear off while you're in one piece.";
			link.l1 = "You'll regret this.";
			link.l1.go = "exit";
		break;

		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
