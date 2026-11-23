// Dialog of the easter-egg talking skeleton in the graveyard chapel (quest "Laying the Dead to Rest")
// The skeleton tells one of ten versions of why the unquiet dead rise here.
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
			DialogExit();
		break;

		case "First time":
			// mark that the player has seen/spoken with the skeleton - then the watcher can be asked about it
			if (CheckAttribute(pchar, "GenQuest.GraveyardQuest")) pchar.GenQuest.GraveyardQuest.ChapelSkelMet = true;
			dialog.text = "Don't be so quick to draw your blade, captain. I'm not one of those that roam at night and gnash their teeth at the living. I just sit here, in this old chapel, and think... Since you've laid the others to rest - perhaps you'll hear me out too?";
			link.l1 = "Where does all this undeath come from, anyway?";
			link.l1.go = "Story";
			link.l2 = "I've no time to chat with bones. Farewell.";
			link.l2.go = "exit";
		break;

		case "Story":
			switch (rand(9))
			{
				case 0:
					dialog.text = "The ground here is foul, friend. They say the chapel was raised on an old heathen altar - from before the white men came. The blood spilled here for idols soaked into the very soil. So the dead find no rest - the earth itself spits them back out.";
				break;
				case 1:
					dialog.text = "There was one... a learned man, in a black robe. He dug in the crypts by night, whispering over the bones in Latin. One day he vanished, and ever since the dead have been rising. I think he never died - he's somewhere near, pulling our strings like a puppeteer.";
				break;
				case 2:
					dialog.text = "It's the rite. The old priest, who knew how to lay the dead in the ground properly, is long gone. The new ones bury folk any old way, without the proper words. A soul seen off wrongly cannot move on - so it returns to the body it left behind.";
				break;
				case 3:
					dialog.text = "Plague, friend. A hundred years back it cut down half the town. They buried them in common pits, in haste, with no rites - just dumped them in and covered them with lime. So many wronged dead in one place... they remember being treated like refuse. And they're angry.";
				break;
				case 4:
					dialog.text = "Beneath the crypt there's something you'd be better off not knowing. An old seal, and under it - something that either sleeps or waits. While the seal holds, it only draws the dead to itself, raising them into its service. But should anyone break it... best not bring that about.";
				break;
				case 5:
					dialog.text = "Better ask the watcher why he clings to this graveyard so and never sets foot beyond it. He knows more than he says. I reckon he struck a bargain with someone long ago - and now he pays for it, year after year, with our bones.";
				break;
				case 6:
					dialog.text = "There's cursed gold here, captain. Some pirate buried his hoard right in a grave, and laid a curse on top - to make the dead man guard the loot. Only the curse spread across the whole graveyard. Now they all stand guard - not even knowing over what.";
				break;
				case 7:
					dialog.text = "It's the full moon, that's what. On some nights the moon hangs low here, red as rusted iron. On such nights the veil between that world and this thins to a hair. The dead hear the call and rise - not out of malice, but because they don't understand they're already dead.";
				break;
				case 8:
					dialog.text = "There was a tale of a girl buried alive - taken for dead in a fever. She woke beneath the earth and screamed until she choked. Ever since, her spite rouses the others. They say that on some nights you can still hear a scratching sound from under the ground.";
				break;
				case 9:
					dialog.text = "Or maybe there's no great secret at all. Maybe we just don't want to leave. It's quiet here, it's familiar. Who'd want to go into unknown darkness when you can stay where you lived your life? You, the living, cling to life. And we, the dead, cling to death.";
				break;
			}
			// упоминание СТАРШЕГО: пасхалка могла выпасть после его убийства ЛИБО когда он вовсе не поднялся
			if (CheckAttribute(pchar, "GenQuest.GraveyardQuest.AvengerKilled"))
			{
				dialog.text = dialog.text + "\n\nAnd one more thing... So it was you who laid the ELDER to rest? The warrior from the farthest barrow, the one who rose to avenge our troubled sleep? In life he wielded a blade so well that Mictlantecuhtli himself - lord of the realm of the dead - marked him and called him into his guard after death... If you have bested one chosen by the god of the dead, why should I fear you? That's why I speak with you so freely.";
			}
			else
			{
				dialog.text = dialog.text + "\n\nAnd one more thing... Count yourself lucky the ELDER did not rise - the warrior from the farthest barrow must be sleeping soundly. In life he wielded a blade so well that Mictlantecuhtli himself - lord of the realm of the dead - marked him and called him into his guard after death. Had you woken him, who knows which of you would still be standing.";
			}
			DeleteAttribute(NPChar, "dialog"); // one tale at a time: the skeleton won't talk again (until the next easter egg)
			link.l1 = RandPhraseSimple("Thanks for the tale. Rest in peace.", "So that's how it is... Be off, old bones.");
			link.l1.go = "exit";
		break;
	}
}
