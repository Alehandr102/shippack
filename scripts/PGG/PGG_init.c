// PGG_SilentKill: silently remove an unwanted PGG (do not count the death)
void PGG_SilentKill(ref chr)
{
	chr.willDie = true;
	chr.DontCountDeath = true;
	LAi_KillCharacter(chr);
}

// Инициализация ПГГ
void InitPsHeros()
{
    int  i, n, k, iStoreQ;
    ref  ch, ch1;
    bool ok;
    int  heroQty   = sti(GetNewMainCharacterParam("ps_hero_qty"));
    i = 1;iStoreQ = 0;
	string sname;
    PsHeroQty = 0; // глобальная переменная
	for (n=1; n<=heroQty; n++)
	{
		if(n != startHeroType)
		{
			PsHeroQty++;
			ch = GetCharacter(NPC_GenerateCharacter("PsHero_" + PsHeroQty, "off_hol_2", "man", "man", 5, PIRATE, -1, true));
			ch.PGGAi.HeroNum = n; // номер в файле
			ch.reputation = 10 + rand(80);
			ch.RebirthPhantom = true;  // не тереть фантома-многодневку (с -1), если умер
			setNewMainCharacter(ch, n);
			ch.Dialog.Filename = GetPGGDialog(ch);
		    ch.greeting = GetPggTavernGreeting(ch);
			//navy -->
			ch.SaveItemsForDead = true; //сохраняем все вещи
			ch.DontClearDead = true;  // не убирать труп через 200с
			ch.DontRansackCaptain = true; //квестовые не сдаются
			ch.AlwaysSandbankManeuver = true;  // тупым запрет тонуть об берег
			ch.SuperShooter  = true;
			ch.perks.list.ByWorker = "1";
			ch.perks.list.ShipEscape = "1";
			ch.loyality = 10 + rand(10); //лояльность
			if (sti(ch.reputation) >= PGG_REP_GOOD)
			{
				ch.alignment = "good";
			}
			else
			{
				ch.alignment = "bad";
			}
			ch.HoldEquip = true; //не отдавать оружие
			ch.PGGAi = true;    // признак, что ПГГ
			ch.PGGAi.IsPGG = true;  // в данный момент не офицер ГГ
			ch.PGGAi.location = "land";   // где суша-море
			ch.PGGAi.location.town = PGG_FindRandomTown(ch);
			//navy <--
			SetFantomParamFromRank_PPG(ch, 1 + rand(5), true); //WW  генерим статы TO_DO на отдельно специал и эмулятор скилов
			trace("PGG " + GetFullName(ch) + " starting rank " + sti(ch.rank));
			SetCharacterPerk(ch, "Energaiser"); // скрытый перк даёт 1.5 к приросту энергии
			ch.Money = PGG_MONEY_START_BASE + rand(1000)*10; // чтоб быстрее корабли купили
			k = rand(13)+1;
			sname = "Totem_"+k;

			if (sname != "") GiveItem2Character(ch, sname);

			if (rand(4) < 3)
			{
				ch.Ship.Type = GenerateShipExt((SHIP_KETCH + rand(11)), 0, ch);
				SetBaseShipData(ch);
				SetRandomNameToShip(ch);
				Fantom_SetCannons(ch, "war"); // LEO: свежий ПГГ должен уметь драться — пушки + боеприпасы сразу,
				Fantom_SetBalls(ch, "war");   // иначе до первого рефита (PGG_UpdateShipEquip) выходил в бой без боеприпасов

				SetCrewQuantity(ch, GetMinCrewQuantity(ch));
				ChangeCrewExp(ch, "Sailors", 50);
				ChangeCrewExp(ch, "Cannoners", 50);
				ChangeCrewExp(ch, "Soldiers", 50);
				SetCharacterGoods(ch, GOOD_SAILCLOTH, 20 + rand(50));
				SetCharacterGoods(ch, GOOD_PLANKS, 20 + rand(50));
				SetCharacterGoods(ch, GOOD_RUM, 50 + rand(100));
				ch.ship.HP = sti(ch.ship.HP) - makeint(sti(ch.ship.HP)*0.05) * makeint(MOD_SKILL_ENEMY_RATE*3/2);

			}
			else
			{
				ch.Money += ch.Money;
			}
			DeleteCloneHeros(ch);

			string sBlockPGG = "PGG" + ch.PGGAi.HeroNum;
			if (CheckAttribute(pchar,"RemovePGG." + sBlockPGG) && sti(pchar.RemovePGG.(sBlockPGG)) == 1)
			{//Убираем неугодных ПГГ с помощью галочек на старте
				PGG_SilentKill(ch);
			}
			RemoveSpeciality(ch);
			SetSpeciality(ch,GetNewMainCharacterType(n));
	    }
		
	}
	//нормальное отношение всех ко всем.
	for (n = 1; n <= PsHeroQty; n++)
	{
		ch = CharacterFromID("PsHero_" + n);
		PGG_SetRelation2OtherAsNormal(ch);
	}
	trace("ПГГ загружено " + PsHeroQty);
	trace("Параметры апдейта ПГГ установлены.");
	SaveCurrentQuestDateParam("PGG_DailyUpdateReload");
	SaveCurrentQuestDateParam("PGG_DailyUpdateSeaReload");
	SaveCurrentQuestDateParam("PGG_DailyUpdateWorldMapReload");
	SaveCurrentQuestDateParam("QuestTalk");
	if(iStoreQ < 31)
	{
		SaveCurrentQuestDateParam("ChangeImport"+iStoreQ);
		iStoreQ++;
	}
	for(i=0; i<STORE_QUANTITY; i++)
    {
		SaveCurrentQuestDateParam("ChangeImport"+i);
	}
	SaveCurrentQuestDateParam("QuestOffer");//кд на приставания в тавернах
}
// boal 091004 много героев  <--

void DeleteCloneHeros(ref sld)
{
	if (startHeroType > 1 && startHeroType < 10)
	{
		if (startHeroType == 2)
		{
			if(sld.FaceId == 487 || sld.FaceId == 535 || sld.FaceId == 211 || sld.FaceId == 495)
			{//Его мы позже наймем оффом, так что убираем из ПГГ
				PGG_SilentKill(sld);
			}
		}
		if (startHeroType == 4 || startHeroType == 5)
		{
			if(sld.FaceId == 1 || sld.FaceId == 522)
			{//Его мы позже наймем оффом, так что убираем из ПГГ
				PGG_SilentKill(sld);
			}
		}
		if (startHeroType == 6 || startHeroType == 7)
		{
			if(sld.FaceId == 508 || sld.FaceId == 517)
			{//Его мы позже наймем оффом, так что убираем из ПГГ
				PGG_SilentKill(sld);
			}
		}
	}
}

