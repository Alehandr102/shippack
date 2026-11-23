void ContrabandDialog(aref link) {
	Dialog.text = "Слушай, проваливай отсюда! Сорвёшь " + GetSexPhrase("сделку, и я вырву тебе сердце!", "мне дело, и я тебе зенки поганые выдавлю!");
	link.l1 = RandPhraseSimple("Ты смеешь мне угрожать, " + GetSexPhrase("собака", "тварь") + "?", GetSexPhrase("Ты подавишься своими словами, каналья!", "Да я тебя сейчас на ремни нарежу, сука!"));
	link.l1.go = "Exit_Smugglers_Fight";
	link.l2 = "Да ладно, я знаю, что такое бизнес. До встречи.";
	link.l2.go = "Exit";
}

void MaxCompanionsExceededDialog(aref link, ref NPChar) {
	Dialog.Text = RandPhraseSimple("Похоже, тебе уже достаточно компаньонов.", "У тебя и так много кораблей. Зачем тебе ещё один?");
	link.l1 = "Да... Может ты и прав" + NPCharSexPhrase(NPChar, "", "а") + ".";
	link.l1.go = "exit";
}

void NotEnoughMoneyDialog(aref link, ref NPChar) {
	Dialog.Text = RandSwear() + "Это была шутка? У тебя же в карманах ветер свищет!"
	link.l1 = "И правда... Пойду я.";
	link.l1.go = "exit";
}

void PGGNoShipDialog(aref link, ref NPChar) {
	Dialog.Text = "Да я вот без корабля остал" + NPCharSexPhrase(NPChar, "ся", "ась") + ".";
	link.l1 = RandSwear() + "Тогда позже поговорим, как корабль достанешь.";
	link.l1.go = "exit";
}

void PCharNoShipDialog(aref link, ref NPChar) {
	Dialog.Text = "А ты мне на что? Кораблик достань для начала, потом поговорим!";
	link.l1 = RandSwear() + "Ну и не нужно!";
	link.l1.go = "exit";
}

void PCharShipClassIsLowDialog(aref link, ref NPChar) {
	Dialog.Text = "Дело с тобой!?? Достойный корабль сначала найди! На твоей скорлупке только почту возить...";
	link.l1 = RandSwear() + "Ну и не нужно!";
	link.l1.go = "exit";
}

void PCharRankLowDialog(aref link, ref NPChar) {
	Dialog.Text = "Дело с тобой!?? Опыта наберись сначала!";
	link.l1 = RandSwear() + "Ну и не нужно!";
	link.l1.go = "exit";
}

void PGGQuestNotEnoughtTimePassedDialog(aref link, ref NPChar) {
	Dialog.Text = NPCharRepPhrase(NPChar, 
		"Никаких дел сегодня, проваливай.", 
		"Извини, ничего не могу предложить."
	);
	link.l1 = PCharRepPhrase(RandSwear(), "Что ж, до встречи.");
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
	Dialog.Text = "А я вам на что, интересно? Приходи, как закончишь все дела с кэпом по имени " + GetFullName(currQuestPGG) + "!";
	link.l1 = "Действительно, что-то я подзабыл" + GetSexPhrase("", "а") + ".";
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
