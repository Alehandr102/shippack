string PGG_Stories[MAX_PGG_STORIES] = {
	"...only the devil and I know this place, and the last one alive will take it all!..",
	"...and Maynard replied, 'I'm not expecting mercy from you, and you won't get it from me either'.",
	"...Cats have clung to the hull! Take your places! And the sea devil can't resist here!..",
	"...In the right hand, a saber! In the left - a pistol! Together we'll send hundreds to the other world!..",
	"...We're pressed to the mast! Smoke - like in hell! Better to die in battle than hang in port!",
	"...once at sea, a little drunk, he suggested: 'Let's create our own hell here right now and see who can endure longer'.",
	"...And here's a funny story that happened to my old friend in Portobello. We were walking through the evening streets from the port to the hot quarter. I'm on the foc'sle, and my companion has already taken the topsail in a reef, then a pair of brigantines came towards us. Both ready for battle. And as soon as we aligned side by side, my friend fires from both sides! Ha-ha-ha-ha...",
	"...Ha-ha-ha!.. And in Santa Cruz, there was an incident during mooring. With a shout of 'Watch out!' my sailor throws the heaving line and hits a local onlooker right in the forehead! The guy - boom! - and falls unconscious. The sailor, frightened, without waiting for the gangway, jumps onto the pier and starts pouring water from the end of the line. Finally, the guy comes to and says to the sailor: - When you throw the heaving line, you need to shout 'Look out!'. Ah-ha-ha...",
	"TBD in InitDynamicValues",
	"TBD in InitDynamicValues",
	"TBD in InitDynamicValues",
	"TBD in InitDynamicValues"
};

string PGG_QuestStories[MAX_PGG_QUEST_STORIES] = {
	"...Somehow we got stuck in Florida - waiting out the calm. When the rum supplies ran out at the tavern, we ended up in a brothel. And being under the reef, I apparently blurted out to one of the girls about the gold I had tucked away for a rainy day. So imagine - when a month later I visited my hiding place, it was as empty as a hungover head! Bite me, sardine, if that talkative beast didn't spill the beans about my stash to the first scoundrel. So, I have no faith in women, not even half a peso!..\nBut then an experienced hanger-on approached our table. He surely has a special approach to the ladies. Or am I wrong? Bite me, sardine, ha-ha-ha!..",
	"...and he mockingly says to her, 'Let's go upstairs. Before joining the crew, we need to check what you can do, like an officer.' And the girl grabbed a knife almost as big as her and says, 'First, let's see if you're fit to be my captain!' Ha-ha-ha... She chased him around the tavern like a shark chasing a cod, and it would have been very funny, gentlemen, if she hadn't killed the poor fellow right in the middle of the hall in front of everyone. May the Lord rest his soul... But since then, I keep an eye out on the Bermudes, just in case she shows up there too!\nHo, let's welcome the worthy captain, who surely would find a way to get along with this fury! Ah-ha-ha-ha...",
	"TBD in InitDynamicValues"
}

string PGG_Meet_GoodRep[MAX_PGG_MEET_REP] = {
	"It's rare to meet a decent captain in these seas.",
	"Nobility is not in fashion nowadays.",
	"And here's an honest captain, no match for you thugs! Hehe!",
	"It's rare to meet a decent captain in these parts.",
	"Damn it, nobility is not in fashion these days.",
	"TBD in InitDynamicValues",
	"TBD in InitDynamicValues",
	"TBD in InitDynamicValues"
};

string PGG_Meet_BadRep[MAX_PGG_MEET_REP] = {
	"Here comes another pirate!",
	"There was a smell of gunpowder and blood, a filibuster on the starboard side!",
	"I feel the smell of gunpowder... Gentlemen, we have been joined by another glorious corsair, who will certainly have something to tell in confession. Ha ha ha",
	"There was a smell of gunpowder and blood on the starboard side!",
	"TBD in InitDynamicValues",
	"TBD in InitDynamicValues",
	"TBD in InitDynamicValues",
	"TBD in InitDynamicValues"
};

string PGGQuestID = "Gen_PGGQuest1";
bool isSetSail = false; // флаг того, что поймали ПГГ на море
bool isPGGInitiative = false; // флаг того, что ПГГ сам подошел

void InitDynamicValues(ref NPChar) {
	isSetSail = CheckAttribute(NPChar, "PGGAi.Task.SetSail");
	isPGGInitiative = CheckAttribute(NPChar, "PGGAi.ActiveQuest");
	if (NPChar.sex == "woman") PGGQuestID = "Gen_PGGQuest1" + NPChar.sex;

	PGG_Stories[8] = "... No, gentlemen, these are all jokes... And here's a story that I personally was in not only a witness, but also a participant in some way. I was heading to the Charlestown tavern one day, straight from the dock, where I was watching heeling over his old Condor, and Captain Sharp himself falls out of the tavern door to meet me! It storms him mercilessly, the mizzen has already been completely torn off, and only with a blind he tries to maneuver up the steps. No sooner had I offered him a services of a pilot, then railing broke under him!!! Ho-ho-ho-ho...\n",
	PGG_Stories[9] = "... I can't say anything about the Flying Dutchman. Neither I nor any of my sailors have ever seen him, thank God... But there was a case no less mysterious. To the east of Bermudes, we met a brigantine under full sail, but she was on a strange course – a kind of zigzags. I didn't plan anything like that, but I sent a boat. What was our surprise when there was not a single soul on board!.. And there are 1,700 barrels of excellent wine in the hold!.. That's what I call luck! And without a single shot being fired! Ha ha ha..\n",
  	PGG_Stories[10] = "... Still, I'm a slope to think, gentlemen, that all these tales about the Flying Dutchman are nothing more than idle inventions of cowardly merchants who are ready to see in every sail on the horizon a bloodthirsty privateer with a drunken monster on the bridge, and in every lugger they meet a harbinger of misfortune...\n";
	PGG_Stories[11] = "... Maybe someone remembers the case south of Key West when I was concussed by beams? Although there was no visible damage, I've layed there for two months. " + NPCharSexPhrase(NPChar, "And my passion of that time persuaded me", "And my friend at that time persuaded me") + " to confess. Like, with God's help, things will get better... Well, I went, and the priest asks: 'Have you killed many people?', " + NPCharSexPhrase(NPChar, "my son", "my daughter") + "? Have you sunk many ships?' – 'Lo-o-ot of' – I said. He din't calm down: 'Did you kill men? That's when I say, 'Padre, I'm not here to brag!' Ha-ha-ha...\n";

	// TODO: не подходят по контексту, надо бы заменить для женского ПГГ
	if (NPChar.sex == "woman") {
		PGG_QuestStories[0] = "... In my right, a saber shines, in the left, a pistol gleams, together we'll send them, beyond the earthly dreams...";
		PGG_QuestStories[1] = "... We're pinned to the mast! The smoke is like hell! It's better to die in a fight than to hang in the port!";
	}
	PGG_QuestStories[2] = "...I don't know about the Flying Dutchman himself, but I saw a ghost ship with my own eyes, as I see you now, gentlemen!.. The damn frigate, black as night, flew out of the fog and boarded without firing a shot! I must admit, I was pretty scared, but we dodged on the port tack and were just preparing to fire a volley of buckshot when he disappeared, as suddenly as he appeared! He was flying through the waves like a bird! I only managed to see the title - 'Queen'... And thunder struck me - on the deck and the cables of this vessel were full of living dead men...\nSuch a devil... And who would not be scared? Suddenly the Sea Devil himself was at the helm? ho ho ho! Rum all!",

  PGG_Meet_BadRep[4] = "Oh, gentlemen, here's a seasoned corsair who probably has something to tell in a friendly company over a mug of rum.. " + IfPhraseSimple(isSetSail, "", "Would you like to join us?");
	PGG_Meet_BadRep[5] = "Gentlemen, there is gentleman of fortune, " + IfPhraseSimple(isSetSail, "on our deck", "at our table") + ", I suppose.";
	PGG_Meet_BadRep[6] = "And here's another " + GetSexPhrase("darling of Fortune", "sea wolf") + "!";
	PGG_Meet_BadRep[7] = GetSexPhrase("Another gentleman of fortune", "Another conqueror of the seas") + ", I suppose.";

  PGG_Meet_GoodRep[5] = "And here's another 'Great Captain', who don't look for easy ways..."
	PGG_Meet_GoodRep[6] = "And here's a heroic personality, I've already heard about you!";
	PGG_Meet_GoodRep[7] = "Gentlemen, we have worthy captain, who haven't been in such a mess yet. What will you tell us, respectfull captain?";
}

