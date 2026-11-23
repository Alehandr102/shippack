// LEO: Campfire storytellers in Marigo. FOUR sailors/officers, each spins ONE yarn per evening (a monologue).
// The player listens to each in turn, the dialog never breaks; the camera switches to the speaker (node tale_next).
// Spawn/tale-pick/seating logic is in scripts\CampfireTales.c. Tale nodes S<n>_1..S<n>_3 (one teller's monologue).
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iIdx, iTale;
	ref rT, rHost;
	string sNum, sHostName;

	switch(Dialog.CurrentNode)
	{
		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "first time":
			NextDiag.TempNode = "first time";
			// LEO: not the chief teller - send them to the host (slot 0)
			if (!CheckAttribute(NPChar, "campHost"))
			{
				sHostName = "him over there";
				rHost = CharacterFromID(Campfire_GetTeller(0));
				if (CheckAttribute(rHost, "name")) sHostName = rHost.name;
				dialog.text = "You'll be wanting " + sHostName + ", captain - he's our chief yarn-spinner tonight.";
				link.l1 = "Right.";
				link.l1.go = "exit";
				break;
			}
			// LEO: all the evening's yarns are already told (Idx reached Target)
			if (CheckAttribute(pchar, "CampfireTales.Idx") && CheckAttribute(pchar, "CampfireTales.Target"))
			{
				if (sti(pchar.CampfireTales.Idx) >= sti(pchar.CampfireTales.Target))
				{
					dialog.text = "That's all for tonight, captain. Our tongues won't turn any more. Look in another evening.";
					link.l1 = "Good night.";
					link.l1.go = "exit";
					break;
				}
			}
			SetCameraDialogMode(NPChar);
			dialog.text = "Good evening, captain. Sit by the fire - we're spinning yarns here. Care to hear a tale or two?";
			link.l1 = "Gladly.";
			link.l1.go = "begin_listen";
			link.l2 = "Another time.";
			link.l2.go = "exit";
		break;

		case "begin_listen":
			dialog.text = "Then settle in, take a load off - no sense standing.";
			LAi_Fade("", "");
			DoQuestFunctionDelay("Campfire_SeatPlayer", 0.5);
			link.l1 = "(Listen)";
			link.l1.go = "tale_next";
		break;

		// LEO: dispatcher - sets the camera on the next teller and leads to HIS yarn; ends the evening after the 4th.
		case "tale_next":
			iIdx = sti(pchar.CampfireTales.Idx);
			if (iIdx >= sti(pchar.CampfireTales.Target))
			{
				SetCameraDialogMode(NPChar);
				dialog.text = "And that's all the yarns for tonight, captain. Good night by the fire.";
				link.l1 = "Thanks for the tales.";
				link.l1.go = "end_tales";
				break;
			}
			rT = CharacterFromID(Campfire_GetTeller(iIdx));
			if (CheckAttribute(rT, "id")) SetCameraDialogMode(rT);
			iTale = Campfire_GetTale(iIdx);
			pchar.CampfireTales.Idx = iIdx + 1;
			if      (iIdx == 0) dialog.text = "Well, captain, listen here - here's a thing that once happened to me...";
			else if (iIdx == 1) dialog.text = "My turn now. There was this time...";
			else if (iIdx == 2) dialog.text = "And here's what I once saw, captain...";
			else                dialog.text = "And mine to finish. Listen...";
			sNum = "" + iTale;
			link.l1 = "(Listen)";
			link.l1.go = "S" + sNum + "_1";
		break;

		// ===================== YARNS (one teller's monologue, camera set in tale_next) =====================
		// --- S1 ---
		case "S1_1":
			dialog.text = "Off Guadeloupe once I reckoned I'd hooked a whale. The line near tore my arms off - hauled half the night, the whole watch hauling.";
			link.l1 = "...";
			link.l1.go = "S1_2";
		break;
		case "S1_2":
			dialog.text = "And up came an old anchor snagged in someone's torn net. Yet in that net thrashed a fish the length of my arm - the cook fed the whole crew for a week.";
			link.l1 = "...";
			link.l1.go = "S1_3";
		break;
		case "S1_3":
			dialog.text = "Mind, I went over the side myself the moment the anchor showed. But I was the first to hug it. Ha-ha!";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S2 ---
		case "S2_1":
			dialog.text = "We had a bosun, Crooked Magnus, rest his soul. He'd whistle so loud the gulls dropped dead from the sky.";
			link.l1 = "...";
			link.l1.go = "S2_2";
		break;
		case "S2_2":
			dialog.text = "Once in a dead calm he piped all hands - the crew tumbled up, no wind, the sails hanging like rags. He bawled at the sky for a solid hour, as if it owed him a debt.";
			link.l1 = "...";
			link.l1.go = "S2_3";
		break;
		case "S2_3":
			dialog.text = "And by morning it blew! Lying, of course - he just read the barometer better than many a captain. Still, with him aboard no watch ever slept through the call.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S3 ---
		case "S3_1":
			dialog.text = "That hoard on the nameless little isle... I still can't believe we took it.";
			link.l1 = "...";
			link.l1.go = "S3_2";
		break;
		case "S3_2":
			dialog.text = "The chart lied at every step - 'twenty paces from the palm,' and there was a whole grove of palms. We dug three days; I thought the captain had lost his wits.";
			link.l1 = "...";
			link.l1.go = "S3_3";
		break;
		case "S3_3":
			dialog.text = "And on the fourth - a chest: doubloons, pearls, and a cross of pure gold. Blew half of it in the first port, fools that we were. Still, there's something to remember by the fire.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S4 ---
		case "S4_1":
			dialog.text = "On the mainland an old Indian told of Mictlantecuhtli, lord of the dead - the one they carried gold to in the temples.";
			link.l1 = "...";
			link.l1.go = "S4_2";
		break;
		case "S4_2":
			dialog.text = "He swore: whoever takes gold from the temple, the god marks him - a shadow at his back, whispers in the night. I saw a man carry an idol out of the jungle myself: within a month he went grey and vanished without a trace.";
			link.l1 = "...";
			link.l1.go = "S4_3";
		break;
		case "S4_3":
			dialog.text = "Their gods are cruel and hungry, not like ours. That's why I'll not set foot in those temples - let the gold lie where it lies.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S5 ---
		case "S5_1":
			dialog.text = "There was a Spanish galleon off Campeche. We closed hull to hull, smoke like a wall.";
			link.l1 = "...";
			link.l1.go = "S5_2";
		break;
		case "S5_2":
			dialog.text = "I leapt across first - and straight away slipped on someone's blood, slid half the deck on my back. The Spaniards were so dumbstruck half of them ran - took me for a berserker.";
			link.l1 = "...";
			link.l1.go = "S5_3";
		break;
		case "S5_3":
			dialog.text = "A berserker! I just couldn't get up. But I earned a name that lasted years - Slippery they've called me ever since. And let them, so long as the share is fair.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S6 ---
		case "S6_1":
			dialog.text = "And on that island where we took on water, bone men crawled out of the sand as the moon rose. With cutlasses, in rotted uniforms.";
			link.l1 = "...";
			link.l1.go = "S6_2";
		break;
		case "S6_2":
			dialog.text = "I blamed the rum, till one swung a blade an arm's length from me. You hack one down - it crumbles, and by morning it stands whole again at the surf.";
			link.l1 = "...";
			link.l1.go = "S6_3";
		break;
		case "S6_3":
			dialog.text = "The captain ordered us gone before nightfall. No one went back a second time. Peace to the dead, and the living best keep away.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S7 ---
		case "S7_1":
			dialog.text = "On Dominica every season blades gather from across all the seas - a fencers' tournament. The prize money so rich, some don't plunder that much in a year.";
			link.l1 = "...";
			link.l1.go = "S7_2";
		break;
		case "S7_2":
			dialog.text = "I saw one there snuff a candle with his rapier without touching the wick, three times running. And the losers? Blood on the sand, the surgeon can't bind them fast enough.";
			link.l1 = "...";
			link.l1.go = "S7_3";
		break;
		case "S7_3":
			dialog.text = "The clever sort bet on other men's blades and grow rich without a scratch. That's for me - a wise man fights with his purse.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S8 ---
		case "S8_1":
			dialog.text = "The cleverest man the sea ever carried was Captain James Flint. They feared to speak his name aloud.";
			link.l1 = "...";
			link.l1.go = "S8_2";
		break;
		case "S8_2":
			dialog.text = "He played the great powers like we play dice: set governors against each other, led whole fleets by the nose. And his hoard's never been found to this day - he hid the map so well the dead man laughs at us all.";
			link.l1 = "...";
			link.l1.go = "S8_3";
		break;
		case "S8_3":
			dialog.text = "Devilishly clever, and lonely for it - a great mind is a heavy burden. Which is why we drink simpler: less wit, sounder sleep.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S9 ---
		case "S9_1":
			dialog.text = "Off Tortuga a hammerhead made off with our baited trap - keg and all! We chased it half a day in the jolly boat.";
			link.l1 = "...";
			link.l1.go = "S9_2";
		break;
		case "S9_2":
			dialog.text = "Caught up, and it had already died, tangled in its own tail. The cook said the meat was tough, fit only for shoe soles.";
			link.l1 = "...";
			link.l1.go = "S9_3";
		break;
		case "S9_3":
			dialog.text = "So soles we made of it. Tramped two years in those boots - and the rascals never let in a drop of water.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S10 ---
		case "S10_1":
			dialog.text = "Once the sea glowed off the side, like stars had fallen into the water.";
			link.l1 = "...";
			link.l1.go = "S10_2";
		break;
		case "S10_2":
			dialog.text = "I thought the souls of the drowned had risen to say farewell, mumbled prayers the whole watch. And it was just plankton, little fish a-glowing.";
			link.l1 = "...";
			link.l1.go = "S10_3";
		break;
		case "S10_3":
			dialog.text = "Who knew. But pretty, I won't lie - never saw the like of it anywhere since.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S11 ---
		case "S11_1":
			dialog.text = "Flying fish flew right onto the deck once, a dozen at a stroke - supper jumped into the pot itself, the cook near wept for joy.";
			link.l1 = "...";
			link.l1.go = "S11_2";
		break;
		case "S11_2":
			dialog.text = "One caught me square in the brow, I went off the bench heels over head. But I ate with special relish after - out of spite.";
			link.l1 = "...";
			link.l1.go = "S11_3";
		break;
		case "S11_3":
			dialog.text = "The sea feeds you itself sometimes, if you live by your conscience. Or if you just open your gob wide enough.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S12 ---
		case "S12_1":
			dialog.text = "We had a cook who'd boil soup from anything that twitched. A rat fell in the pot once - he never batted an eye.";
			link.l1 = "...";
			link.l1.go = "S12_2";
		break;
		case "S12_2":
			dialog.text = "'Meat is meat,' says he, and stirs on. The crew ate and praised it, till they found out.";
			link.l1 = "...";
			link.l1.go = "S12_3";
		break;
		case "S12_3":
			dialog.text = "And when they found out, they near put him in the pot himself. Bolted at the first port - quick on his feet, the rogue.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S13 ---
		case "S13_1":
			dialog.text = "We had a cabin boy, Pierrot, scared of everything - the sea, the captain. Yet in a storm he was first up the mast, when the old hands quailed.";
			link.l1 = "...";
			link.l1.go = "S13_2";
		break;
		case "S13_2":
			dialog.text = "Made the sail fast, came down white as a sheet, and dropped in a faint. The captain gave him his own tot of rum, in front of all.";
			link.l1 = "...";
			link.l1.go = "S13_3";
		break;
		case "S13_3":
			dialog.text = "From that day Pierrot was a changed lad, made a fine sailor. Fear's no vice - the vice is giving in to it.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S14 ---
		case "S14_1":
			dialog.text = "Our first mate stacked cards slicker than the navigator laid a course - diced the whole crew down to their drawers.";
			link.l1 = "...";
			link.l1.go = "S14_2";
		break;
		case "S14_2":
			dialog.text = "One day he overplayed - lost his own chest to the captain. And in the chest, the captain's own share, that he'd pinched!";
			link.l1 = "...";
			link.l1.go = "S14_3";
		break;
		case "S14_3":
			dialog.text = "What a laugh - the captain roared till he held his sides. And forgave him: said cheek like that is worth a price.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S15 ---
		case "S15_1":
			dialog.text = "We bought a chart off a drunk in a tavern once, for a bottle - thought we'd cheated the fool. And the chart turned out true.";
			link.l1 = "...";
			link.l1.go = "S15_2";
		break;
		case "S15_2":
			dialog.text = "It led to a cave, and there - barrels, all empty. All but one: coin enough in it for three lives over.";
			link.l1 = "...";
			link.l1.go = "S15_3";
		break;
		case "S15_3":
			dialog.text = "That drunk likely never knew what he'd sold. Or knew, and reckoned we'd never make it anyway.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S16 ---
		case "S16_1":
			dialog.text = "The captain could smell gold like a hound smells game. Found a cache under an old flagstone where all had walked past.";
			link.l1 = "...";
			link.l1.go = "S16_2";
		break;
		case "S16_2":
			dialog.text = "Said the stone lay wrong, set by another man's hand. We'd have trampled there an age, and he - one move, and cracked it.";
			link.l1 = "...";
			link.l1.go = "S16_3";
		break;
		case "S16_3":
			dialog.text = "A nose like that's a gift: who has it, has riches. And who hasn't, walks at his heel for a share.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S17 ---
		case "S17_1":
			dialog.text = "One chest we hauled three days out of the silt, by the very reef. Up it came - not gold, but someone's bones in irons.";
			link.l1 = "...";
			link.l1.go = "S17_2";
		break;
		case "S17_2":
			dialog.text = "A slave, like as not, dropped overboard with the cargo to spare a split. The captain bade us lay the bones in the earth, decent-like.";
			link.l1 = "...";
			link.l1.go = "S17_3";
		break;
		case "S17_3":
			dialog.text = "And come morning, under that spot, another chest turned up, with coin. The dead pay the living for kindness now and then.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S18 ---
		case "S18_1":
			dialog.text = "In the south we saw a temple, all carved - serpents and skulls. Quetzalcoatl, the feathered serpent: the priests bore him hearts.";
			link.l1 = "...";
			link.l1.go = "S18_2";
		break;
		case "S18_2":
			dialog.text = "The stone in the yard was dark, as if soaked through with blood. I never went near it - I felt an ill place.";
			link.l1 = "...";
			link.l1.go = "S18_3";
		break;
		case "S18_3":
			dialog.text = "And one of ours spat on the altar, had a laugh. A week on he drowned in a flat calm. A calm, you hear?";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S19 ---
		case "S19_1":
			dialog.text = "A shaman in the village read my fortune in the coals of the fire. Said I'd die not by blade nor by ball.";
			link.l1 = "...";
			link.l1.go = "S19_2";
		break;
		case "S19_2":
			dialog.text = "I was glad of it - thought, in my own bed. And then he added: by water.";
			link.l1 = "...";
			link.l1.go = "S19_3";
		break;
		case "S19_3":
			dialog.text = "Ever since I've no love for swimming. A sailor afeared of water - the gods know how to jest.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S20 ---
		case "S20_1":
			dialog.text = "A jade idol we took from a ruined city - green, eyes of stone, heavy as sin.";
			link.l1 = "...";
			link.l1.go = "S20_2";
		break;
		case "S20_2":
			dialog.text = "The whole way to the ship it rained, though the sky was clear. The captain took one look - bade us put it back.";
			link.l1 = "...";
			link.l1.go = "S20_3";
		break;
		case "S20_3":
			dialog.text = "We did - and the rain stopped at once, believe it? Some gold costs more than they give for it.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S21 ---
		case "S21_1":
			dialog.text = "We took a slaver's brig, all of us black with fury. I was first into the hold - to cut the chains, free the slaves.";
			link.l1 = "...";
			link.l1.go = "S21_2";
		break;
		case "S21_2":
			dialog.text = "And they took us for the new masters, came at us swinging. Took some doing to make them see - friends, freedom.";
			link.l1 = "...";
			link.l1.go = "S21_3";
		break;
		case "S21_3":
			dialog.text = "Half of them joined our crew after. The finest fighters - held their freedom with their teeth.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S22 ---
		case "S22_1":
			dialog.text = "A merchant struck his colours without a fight, soon as he spied the flag. And in the hold - pepper, cloves, silk. A fortune.";
			link.l1 = "...";
			link.l1.go = "S22_2";
		break;
		case "S22_2":
			dialog.text = "The captain never laid a finger on him, even asked him to sup. 'A dead merchant,' says he, 'sails back no more.'";
			link.l1 = "...";
			link.l1.go = "S22_3";
		break;
		case "S22_3":
			dialog.text = "And sail back he did! A year on he sought us out himself, with cargo. Clever plunder is when they're glad to see you again.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S23 ---
		case "S23_1":
			dialog.text = "A frigate chased us two days, no shaking it. The captain bade us pitch empty barrels over, one by one.";
			link.l1 = "...";
			link.l1.go = "S23_2";
		break;
		case "S23_2":
			dialog.text = "The pursuer reckoned we were sinking, losing the haul, and eased off. And we to windward - and gone like smoke.";
			link.l1 = "...";
			link.l1.go = "S23_3";
		break;
		case "S23_3":
			dialog.text = "Cunning saves where the guns can't. And the powder's spared - powder costs money.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S24 ---
		case "S24_1":
			dialog.text = "On Isla de Muertos, they say, the dead guard a hoard - who takes the gold turns to a dead man himself.";
			link.l1 = "...";
			link.l1.go = "S24_2";
		break;
		case "S24_2":
			dialog.text = "One bold fellow bore off a coin. Came back - and had no shadow, and nothing in the glass. Alive, yet as if not.";
			link.l1 = "...";
			link.l1.go = "S24_3";
		break;
		case "S24_3":
			dialog.text = "He wore that coin till he put it back. Put it back - and his shadow returned. Never grasped again.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S25 ---
		case "S25_1":
			dialog.text = "A ghost ship we saw in the fog off the cape - sails in tatters, no soul on deck, and her sailing against the wind.";
			link.l1 = "...";
			link.l1.go = "S25_2";
		break;
		case "S25_2":
			dialog.text = "We hailed her - silence. She passed through us like smoke. The captain forbade us to speak of her till we made land.";
			link.l1 = "...";
			link.l1.go = "S25_3";
		break;
		case "S25_3":
			dialog.text = "Said whoever prattles of such a thing, it comes and takes him. So we whisper - you mustn't speak loud of the dead.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S26 ---
		case "S26_1":
			dialog.text = "They hanged a pirate on Tortuga once, before all the folk. And before the noose he up and swore he'd come back.";
			link.l1 = "...";
			link.l1.go = "S26_2";
		break;
		case "S26_2":
			dialog.text = "A year on they saw him in that same tavern, at that same table. I poured him the rum myself - and his hand was cold as iron in December.";
			link.l1 = "...";
			link.l1.go = "S26_3";
		break;
		case "S26_3":
			dialog.text = "He drank up, nodded, and left. Never showed again.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S27 ---
		case "S27_1":
			dialog.text = "On Dominica a Spaniard fought so fast you couldn't see the blade. They said he'd trained under the master of Toledo himself.";
			link.l1 = "...";
			link.l1.go = "S27_2";
		break;
		case "S27_2":
			dialog.text = "Against him went our bosun, drunk as a lord. And he landed a hit! The devil himself favours a drunk.";
			link.l1 = "...";
			link.l1.go = "S27_3";
		break;
		case "S27_3":
			dialog.text = "The Spaniard took such offence he quit the tournament and sailed off. And the bosun, come morning, couldn't recall how he'd won.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S28 ---
		case "S28_1":
			dialog.text = "I saw a woman with a blade at the tournament - fought like a fury, downed three running and never drew breath.";
			link.l1 = "...";
			link.l1.go = "S28_2";
		break;
		case "S28_2":
			dialog.text = "Men refused to face her after - shamed to lose. And she just smirked and honed her sabre.";
			link.l1 = "...";
			link.l1.go = "S28_3";
		break;
		case "S28_3":
			dialog.text = "They say she took a kiss for every man bested. Now there's a prize I'd step out for myself, eh.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S29 ---
		case "S29_1":
			dialog.text = "A fop turned up at the tournament in lace and a perfumed kerchief. All jeered, till he bared his rapier.";
			link.l1 = "...";
			link.l1.go = "S29_2";
		break;
		case "S29_2":
			dialog.text = "Six fighters - and not a scratch on the lace. Dabbed his brow with the kerchief and bowed, like at the theatre.";
			link.l1 = "...";
			link.l1.go = "S29_3";
		break;
		case "S29_3":
			dialog.text = "So much for the fop - don't judge by feathers. Ever since I don't laugh at dandies, just in case.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S30 ---
		case "S30_1":
			dialog.text = "They say Flint had a quartermaster - one leg, and a parrot. Silver, I think, slyer than Flint himself.";
			link.l1 = "...";
			link.l1.go = "S30_2";
		break;
		case "S30_2":
			dialog.text = "Hid his money so well he couldn't find it after. Yet he got away alive - and how many got away from Flint alive?";
			link.l1 = "...";
			link.l1.go = "S30_3";
		break;
		case "S30_3":
			dialog.text = "Him alone, like as not. The clever live long. Or those who twig in time when to run.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S31 ---
		case "S31_1":
			dialog.text = "Blackbeard wove fuses into his beard and lit them in a fight - smoke, sparks, the very devil climbing out of hell.";
			link.l1 = "...";
			link.l1.go = "S31_2";
		break;
		case "S31_2":
			dialog.text = "Foes threw down their arms before the boarding even came. Half his victories he took by his look alone.";
			link.l1 = "...";
			link.l1.go = "S31_3";
		break;
		case "S31_3":
			dialog.text = "They struck his head off in the end - he fought on with nine wounds. Fear's a weapon, but there's lead for fear, too.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S32 ---
		case "S32_1":
			dialog.text = "Bartholomew Roberts drank water, not rum. An odd one, eh? Yet he took four hundred ships - sober's the handier way.";
			link.l1 = "...";
			link.l1.go = "S32_2";
		break;
		case "S32_2":
			dialog.text = "Dressed like a lord - a crimson coat, a feather in his hat. Died in it too, on deck, from a charge of grape.";
			link.l1 = "...";
			link.l1.go = "S32_3";
		break;
		case "S32_3":
			dialog.text = "Lived fine, and fell fine. Better so than to rot on the gallows in chains.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S33 ---
		case "S33_1":
			dialog.text = "An octopus clamped onto our rudder once, down at the bottom - a monster, an arm thick as the main yard.";
			link.l1 = "...";
			link.l1.go = "S33_2";
		break;
		case "S33_2":
			dialog.text = "Our diver went down - and back up like a shot, without his knife. Left the blade to the octopus for a keepsake, ha.";
			link.l1 = "...";
			link.l1.go = "S33_3";
		break;
		case "S33_3":
			dialog.text = "It let go itself once it darkened - they've no love for noise. And the rudder stank of ink for a week after.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S34 ---
		case "S34_1":
			dialog.text = "We had a navigator who read the stars like a book. Went blind with age - so he steered by memory.";
			link.l1 = "...";
			link.l1.go = "S34_2";
		break;
		case "S34_2":
			dialog.text = "And never once erred, while he could see a spark in the sky. And when he went wholly blind - he went ashore, weeping.";
			link.l1 = "...";
			link.l1.go = "S34_3";
		break;
		case "S34_3":
			dialog.text = "You can't cross the sea without eyes, not even with such a memory. He died a lighthouse keeper - light, at least, and by the sea.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S35 ---
		case "S35_1":
			dialog.text = "Our captain hated gulls worse than a storm. One snatched his hat off his head once, before all.";
			link.l1 = "...";
			link.l1.go = "S35_2";
		break;
		case "S35_2":
			dialog.text = "He chased it the length of the deck with a sabre - the crew slid down the shrouds laughing.";
			link.l1 = "...";
			link.l1.go = "S35_3";
		break;
		case "S35_3":
			dialog.text = "The gull dropped the hat in the sea, taking offence. And the captain went bareheaded after, on principle.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S36 ---
		case "S36_1":
			dialog.text = "We buried our share on an islet, scratched a cross. Came back a year on - and the islet was under water!";
			link.l1 = "...";
			link.l1.go = "S36_2";
		break;
		case "S36_2":
			dialog.text = "The tide, a shoal... We stood like fools over the waves. The captain said the sea had taken its own share.";
			link.l1 = "...";
			link.l1.go = "S36_3";
		break;
		case "S36_3":
			dialog.text = "Ever since we hide hoards only high and dry. And set a cross - the sea leaves crosses be, so far.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S37 ---
		case "S37_1":
			dialog.text = "An old man on his deathbed pressed a chart into my hand. I followed it to an empty well and a note: 'wealth is life.'";
			link.l1 = "...";
			link.l1.go = "S37_2";
		break;
		case "S37_2":
			dialog.text = "What an old rogue - died with a smile, I'd wager. And I carry that note on me to this day.";
			link.l1 = "...";
			link.l1.go = "S37_3";
		break;
		case "S37_3":
			dialog.text = "Maybe he was no fool. Maybe we are, chasing gold our whole lives.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S38 ---
		case "S38_1":
			dialog.text = "In the jungle stood an idol, facing the sunrise, all in moss. The locals brought it maize and flowers once a year.";
			link.l1 = "...";
			link.l1.go = "S38_2";
		break;
		case "S38_2":
			dialog.text = "They said it sleeps, and must on no account be woken. And what happens if you wake it - they don't know themselves.";
			link.l1 = "...";
			link.l1.go = "S38_3";
		break;
		case "S38_3":
			dialog.text = "Somehow no volunteers turned up to find out. And rightly - best not to touch sleeping gods.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S39 ---
		case "S39_1":
			dialog.text = "We took a schooner laden with powder kegs to the brim. One spark - and we'd all have gone to heaven at once.";
			link.l1 = "...";
			link.l1.go = "S39_2";
		break;
		case "S39_2":
			dialog.text = "We fought on tiptoe, near cursing in whispers. A laugh, truly - a boarding quieter than church.";
			link.l1 = "...";
			link.l1.go = "S39_3";
		break;
		case "S39_3":
			dialog.text = "But we took her clean, not a shot fired. Silence is worse than guns, now and then.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S40 ---
		case "S40_1":
			dialog.text = "The enemy captain turned out an old debtor of ours - the one that diced away his ship to him once.";
			link.l1 = "...";
			link.l1.go = "S40_2";
		break;
		case "S40_2":
			dialog.text = "Went pale as a shroud at the sight of us. And ours took but the debt - the debt and interest.";
			link.l1 = "...";
			link.l1.go = "S40_3";
		break;
		case "S40_3":
			dialog.text = "Honour, says he, is dearer than plunder. That's why we'd follow him into fire - for honour's sake.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S41 ---
		case "S41_1":
			dialog.text = "At the graveyard by the fort, they say, lights walk at night - the souls of those buried without rites.";
			link.l1 = "...";
			link.l1.go = "S41_2";
		break;
		case "S41_2":
			dialog.text = "I bedded down there once, like a fool, on a dare. A little flame came up, stood by a while, and quietly went out.";
			link.l1 = "...";
			link.l1.go = "S41_3";
		break;
		case "S41_3":
			dialog.text = "I didn't go grey, as you see. Maybe it wished to say farewell - they're lonely too.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S42 ---
		case "S42_1":
			dialog.text = "An old man came out at the tournament, seventy if a day. All thought it a joke, and he downed three young bucks.";
			link.l1 = "...";
			link.l1.go = "S42_2";
		break;
		case "S42_2":
			dialog.text = "Not by strength - by guile: where he stands, there the blade goes. Youth swings, age aims.";
			link.l1 = "...";
			link.l1.go = "S42_3";
		break;
		case "S42_3":
			dialog.text = "He took the prize, bowed, and left without a word. Would that I could do the same - hold a blade to my grey hairs.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S43 ---
		case "S43_1":
			dialog.text = "They say Flint, near death, kept crying out about a black dog - raving, like as not. Or his conscience caught him at the last hour.";
			link.l1 = "...";
			link.l1.go = "S43_2";
		break;
		case "S43_2":
			dialog.text = "Took six to the grave with him, the ones who hid the hoard - so they'd not blab. The dead keep mum better than the living.";
			link.l1 = "...";
			link.l1.go = "S43_3";
		break;
		case "S43_3":
			dialog.text = "That's why none has found his chart to this day. Nor will - Flint left dead men for sentries.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S44 ---
		case "S44_1":
			dialog.text = "Mary Read went dressed as a man, till she rose to captain. Fought finer than many a man, I've heard.";
			link.l1 = "...";
			link.l1.go = "S44_2";
		break;
		case "S44_2":
			dialog.text = "Went into battle big with child, and begged no quarter. Died in prison, the babe never born.";
			link.l1 = "...";
			link.l1.go = "S44_3";
		break;
		case "S44_3":
			dialog.text = "She lived free - not every soul is granted that. And freedom is what costs the dearest of all.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S45 ---
		case "S45_1":
			dialog.text = "A whale surfaced under the side once, big as a fair islet. The ship rolled - half the crew near went over the side.";
			link.l1 = "...";
			link.l1.go = "S45_2";
		break;
		case "S45_2":
			dialog.text = "It looked with an eye, wise as a man's, and went down into the deep, never touching us.";
			link.l1 = "...";
			link.l1.go = "S45_3";
		break;
		case "S45_3":
			dialog.text = "The captain said it had blessed the ship. And true enough, the whole voyage we had luck. Go figure.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S46 ---
		case "S46_1":
			dialog.text = "Our parrot cursed fouler than the bosun - called the captain by name and added a word.";
			link.l1 = "...";
			link.l1.go = "S46_2";
		break;
		case "S46_2":
			dialog.text = "One grand guest near fell off his stool, and the captain roared: you can't reschool a bird, says he.";
			link.l1 = "...";
			link.l1.go = "S46_3";
		break;
		case "S46_3":
			dialog.text = "When the parrot died, the whole crew held a wake. You'll grieve a bird more than a man, sometimes.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S47 ---
		case "S47_1":
			dialog.text = "We bought pearls off the divers once, for a song. And in one shell - a pearl big as a pigeon's egg.";
			link.l1 = "...";
			link.l1.go = "S47_2";
		break;
		case "S47_2":
			dialog.text = "Sold it to a grandee for a whole fortune - on that money half the crew bought houses.";
			link.l1 = "...";
			link.l1.go = "S47_3";
		break;
		case "S47_3":
			dialog.text = "And the divers stayed in want as before. Luck is blind: to one the shell, to another the egg in it.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S48 ---
		case "S48_1":
			dialog.text = "A volcano smoked on an island, and the locals brought offerings - cast gold into the crater so the mountain wouldn't rage.";
			link.l1 = "...";
			link.l1.go = "S48_2";
		break;
		case "S48_2":
			dialog.text = "One of ours went by night to fetch that gold back out. The mountain woke that very night - we barely sailed clear.";
			link.l1 = "...";
			link.l1.go = "S48_3";
		break;
		case "S48_3":
			dialog.text = "So he never got rich off it. The gods don't like it when you take back what's theirs.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S49 ---
		case "S49_1":
			dialog.text = "Once we took a ship, and the captain there - a mere lad: the father had died, the son took the wheel, but couldn't manage.";
			link.l1 = "...";
			link.l1.go = "S49_2";
		break;
		case "S49_2":
			dialog.text = "Our captain never touched him, taught him his course, and let him go with the cargo - said, let him grow.";
			link.l1 = "...";
			link.l1.go = "S49_3";
		break;
		case "S49_3":
			dialog.text = "Years on, that lad saved our lives, in a storm. Good travels the sea in rings, like a wave.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;
		// --- S50 ---
		case "S50_1":
			dialog.text = "They say Flint's hoard lies on an island that drifts in the fog. Whoever sails for it, the fog spins him round, back to the sea.";
			link.l1 = "...";
			link.l1.go = "S50_2";
		break;
		case "S50_2":
			dialog.text = "Many have sighted the island. Set foot on it - none. Maybe there's no such isle at all, maybe just a tale.";
			link.l1 = "...";
			link.l1.go = "S50_3";
		break;
		case "S50_3":
			dialog.text = "Or maybe Flint wished it so - that they'd seek forever. The best guard for a hoard is human greed and fog.";
			link.l1 = "(Nod)";
			link.l1.go = "tale_next";
		break;

		case "end_tales":
			NextDiag.CurrentNode = "first time";
			AddDialogExitQuestFunction("Campfire_End");
			DialogExit();
		break;
	}
}
