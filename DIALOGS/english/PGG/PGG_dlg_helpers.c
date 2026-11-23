void ContrabandDialog(aref link) {
	Dialog.text = "Listen, get out of here! You 'll rip it off " + GetSexPhrase("A deal, and I'll rip your heart out!", "I care, and I'll squeeze your filthy teeth out!");
	link.l1 = RandPhraseSimple("You dare threaten me, " + GetSexPhrase("the dog", "the creature") + "?", GetSexPhrase("You'll choke on your words, you rascal!", "I'm going to cut you into straps, bitch!"));
	link.l1.go = "Exit_Smugglers_Fight";
	link.l2 = "Come on, I know what business is. See you later.";
	link.l2.go = "Exit";
}

void MaxCompanionsExceededDialog(aref link, ref NPChar) {
	Dialog.Text = RandPhraseSimple("It looks like you've had enough companions already.", "You already have a lot of ships. Why do you need another one?");
	link.l1 = "yeah... Maybe you're right.";
	link.l1.go = "exit";
}

void NotEnoughMoneyDialog(aref link, ref NPChar) {
	Dialog.Text = RandSwear() + "Was it a joke? The wind is whistling in your pockets!"
	link.l1 = "It's true... I'll go.";
	link.l1.go = "exit";
}

void PGGNoShipDialog(aref link, ref NPChar) {
	Dialog.Text = "Yes, I'm left without a ship.";
	link.l1 = RandSwear() + "Then we'll talk later about how you get the ship.";
	link.l1.go = "exit";
}

void PCharNoShipDialog(aref link, ref NPChar) {
	Dialog.Text = "What do you want me to do? Get the boat first, then we'll talk!";
	link.l1 = RandSwear() + "Well, you don't have to!";
	link.l1.go = "exit";
}

void PCharShipClassIsLowDialog(aref link, ref NPChar) {
	Dialog.Text = "Is the matter with you!?? Find a decent ship first! Well, your shell only needs to carry mail...";
	link.l1 = RandSwear() + "Well, you don't have to!";
	link.l1.go = "exit";
}

void PCharRankLowDialog(aref link, ref NPChar) {
	Dialog.Text = "Is the matter with you!?? Gain experience first!";
	link.l1 = RandSwear() + "Well, you don't have to!";
	link.l1.go = "exit";
}

void PGGQuestNotEnoughtTimePassedDialog(aref link, ref NPChar) {
	Dialog.Text = NPCharRepPhrase(NPChar, 
		"No business today, get lost.", 
		"Sorry, I can't offer anything."
	);
	link.l1 = PCharRepPhrase(RandSwear(), "Well, see you later.");
	link.l1.go = "exit";
}

bool PGGQuestAlreadyActive(aref link, ref NPChar) {
	if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid"))
	{//висящий флаг без PGGid: само-залечивание
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
		return false;
	}
	int currQuestPGGid = GetCharacterIndex(PChar.GenQuest.PGG_Quest.PGGid);
	if (currQuestPGGid < 0 || currQuestPGGid >= TOTAL_CHARACTERS) {
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
		return false;
	}

	ref currQuestPGG = &characters[currQuestPGGid];
	if (!CheckAttribute(currQuestPGG, "PGGAi.ActiveQuest")) {
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
		return false;
	}
	Dialog.Text = "What do you need me for, I wonder? Come back as soon as you finish all the business with the cap " + GetFullName(currQuestPGG) + "!";
	link.l1 = "Indeed, I forgot it...";
	link.l1.go = "exit";
	return true;
}

void GeneratePGGQuestLocation(ref NPChar) 
{
	PChar.GenQuest.PGG_Quest.Template = drand(1);
	if (CheckAttribute(NPChar, "PGGAi.ActiveQuest.QstNumber.Template"))
	{
		PChar.GenQuest.PGG_Quest.Template = NPChar.PGGAi.ActiveQuest.QstNumber.Template;
	}
	PChar.GenQuest.PGG_Quest.Parts = GetCompanionQuantity(PChar)+1;
	PChar.GenQuest.PGG_Quest.Nation = rand(3);

	// генерим остров и пляж, пока не найдем пустой(без квеста)
	PChar.GenQuest.PGG_Quest.Island.Shore = "";
	string sCurIsland = GetCharacterCurrentIslandId(pchar);
	string sCurIsland2 = "";
	if (PChar.ColonyBuilding.Stage != "0") sCurIsland2 = "Caiman";

	while(PChar.GenQuest.PGG_Quest.Island.Shore == "" || 
		PChar.GenQuest.PGG_Quest.Island == "Pearl" || 
        PChar.GenQuest.PGG_Quest.Island == "Panama" ||		
		PChar.GenQuest.PGG_Quest.Island == "KhaelRoa" ||
		PChar.GenQuest.PGG_Quest.Island == "Reefs" ||
		PChar.GenQuest.PGG_Quest.Island == sCurIsland ||
		PChar.GenQuest.PGG_Quest.Island == sCurIsland2)	//фикс - не даём квест на ближайший остров и на начавший строится кайман
	{
		if (sti(PChar.GenQuest.PGG_Quest.Template)) 
		{
			PChar.GenQuest.PGG_Quest.Island = GetRandomUninhabitatIsland();
		} 
		else 
		{
			PChar.GenQuest.PGG_Quest.Island = GetRandomIslandId();
		}
		PChar.GenQuest.PGG_Quest.Island.Shore = GetIslandRandomShoreId(PChar.GenQuest.PGG_Quest.Island);
		if (!isLocationFreeForQuests(PChar.GenQuest.PGG_Quest.Island.Shore)) PChar.GenQuest.PGG_Quest.Island.Shore = "";
	}
	
	PChar.GenQuest.PGG_Quest.Island.Town = FindTownOnIsland(PChar.GenQuest.PGG_Quest.Island);

	if (isSetSail) PChar.GenQuest.PGG_Quest.Days = rand(4) + 5;
	else PChar.GenQuest.PGG_Quest.Days = GetMaxDaysFromIsland2Island(GetCharacterCurrentIslandId(PChar), PChar.GenQuest.PGG_Quest.Island) + 1;

	PChar.GenQuest.PGG_Quest.Goods = GOOD_SLAVES + drand(2);
	
	PChar.GenQuest.PGG_Quest.PGGid = NPChar.id;
	NPChar.Nation.Bak = NPChar.Nation;
}
