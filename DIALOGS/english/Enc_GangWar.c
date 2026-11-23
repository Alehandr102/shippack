// "Gang and patrol skirmish" event dialog: thanks for help, rewards and battle stories
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "exit":
			if (CheckAttribute(NPChar, "bGangLeader")) { DeleteAttribute(NPChar, "bGangLeader"); UnmarkCharacter(NPChar); } // remove the mini-boss mark after the talk
			LAi_SetCitizenTypeNoGroup(NPChar); // walks off after the talk
			DialogExit();
		break;

		// --- Patrol thanks (player helped the soldiers, officer survived) ---
		case "ThankPatrol":
			GangWar_RewardPatrol();
			switch (rand(4))
			{
				case 0: dialog.text = "Captain! If not for your help, those bandits would have cut us all down to the last man. The Crown won't forget this service. Please accept our sincere gratitude."; break;
				case 1: dialog.text = "You arrived just in time, captain. A moment longer and my squad would have been nothing but a heap of bodies. I thank you on behalf of the Crown."; break;
				case 2: dialog.text = "Bravely fought, captain! Not everyone would risk standing shoulder to shoulder with a patrol against a gang of cutthroats. This won't go unrewarded."; break;
				case 3: dialog.text = "Without your blade we'd have been done for. I'll be sure to report your valor to the governor. Accept my gratitude, captain."; break;
				case 4: dialog.text = "God as my witness, you saved our lives. Men like you are the Crown's backbone in these wild lands. I scarcely know how to thank you."; break;
			}
			link.l1 = RandPhraseSimple("Glad I could help, officer.", "It's every man's duty to aid the Crown.");
			link.l1.go = "exit";
		break;

		// --- Bandits thanks (player helped the gang, leader survived) ---
		case "ThankBandits":
			GangWar_RewardBandits();
			if (CheckAttribute(pchar, "GangWar.bandWithhold")) // penalty - no reward
			{
				if (pchar.GangWar.bandWithhold == "betray") // last time slaughtered the gang after thanks
				{
					switch (rand(2))
					{
						case 0: dialog.text = "Hold on... I know you. Last time you took our thanks and then cut down the survivors in cold blood. Word travels fast. I'll take the help, but don't expect a share - the boys don't trust you."; break;
						case 1: dialog.text = "Ah, it's you... We've heard. Took your cut last time, then butchered those who were left. Be glad I even speak to you. No coin for you, traitor."; break;
						case 2: dialog.text = "You're already known around here, friend. Whoever sells out the brotherhood after a handshake gets no trust. Thanks for the hand, but I'll keep my purse to myself."; break;
					}
				}
				else // last event helped the patrol/soldiers
				{
					switch (rand(2))
					{
						case 0: dialog.text = "Ah, it's you... We heard you were swinging steel side by side with the soldiers last time. You helped us today, true, but there's no trust for you here. Thanks for the hand, but don't expect a share."; break;
						case 1: dialog.text = "I'll take the help, but I'll keep my purse. Whoever was licking the Crown's boots the other day gets no trust from us. No offense, friend."; break;
						case 2: dialog.text = "We saw you with the patrol last time, friend. Today you're with us - and thanks for that - but we won't share our spoils with someone who was against us yesterday."; break;
					}
				}
				link.l1 = RandPhraseSimple("Suit yourself.", "Your call. Be off.");
			}
			else
			{
				switch (rand(4))
				{
					case 0: dialog.text = "Heh, you swing that steel handsomely! Without you they'd have buried us here. We shook a little something out of some travelers - take your cut for the help. Be off, and don't let the soldiers spot you."; break;
					case 1: dialog.text = "You're a beast in a fight, friend! Those toy soldiers thought they'd butcher us like chickens. Here, take a share from the common pot - you've earned it."; break;
					case 2: dialog.text = "You're one of us, through and through! Didn't leave us in the lurch. Take the coin - we share fair with those who stand by us."; break;
					case 3: dialog.text = "A good scrap, and you didn't falter in it! Here's for your trouble - all honestly shaken out of passersby. We'll meet again on some narrow road."; break;
					case 4: dialog.text = "Cut them down nicely, you did! It'd be a sin not to repay such help. Take your jingling share, captain, and remember - the gang doesn't forget its own."; break;
				}
				link.l1 = RandPhraseSimple("Don't mention it. Good luck on the roads.", "Coin's never unwelcome. Be off.");
			}
			link.l1.go = "exit";
		break;

		// --- Battle story (player didn't intervene, can ask the winner) ---
		case "Story":
			switch (rand(4))
			{
				case 0:
					dialog.text = "It was a fierce slaughter, friend... Steel rang so loud the birds fell from the trees. We lost many, but few of them walked away alive either.";
				break;
				case 1:
					dialog.text = "You should have seen us clash! Muskets thundered, blood splashed, screams filled the whole thicket. We barely held our ground.";
				break;
				case 2:
					dialog.text = "They burst out from behind the trees, we met them with blades. Fought to the last, the ground turned brown with blood. But we ground them down.";
				break;
				case 3:
					dialog.text = "These parts haven't seen such carnage in ages. Someone meant to treat us all to gunpowder, but it didn't pan out. Only the toughest and luckiest are left.";
				break;
				case 4:
					dialog.text = "We met head-on - not a step back. Some found a bullet, some a blade. It's quiet on the clearing now... only the crows are circling.";
				break;
			}
			link.l1 = "Aye, a hot affair. Be well.";
			link.l1.go = "exit";
		break;

		// --- Repeat talk after the reward ---
		case "AfterThanks":
			dialog.text = "Thanks again, captain. Go in peace.";
			link.l1 = "Same to you.";
			link.l1.go = "exit";
		break;
	}
}
