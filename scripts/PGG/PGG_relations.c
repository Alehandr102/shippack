//==================================================
// Relations
//==================================================
//изменить отношение ПГГ к персонажу
//pgg - ПГГ, которому меняем, chr - отношение к какому персу, _addValue - величина изменения (+/-)
int PGG_ChangeRelation2Character(ref pgg, ref chr, int _addValue)
{
	int curRel = 0;
	string chrID = chr.id;
	if (CheckAttribute(pgg, "PGGAi.relations." + chrID))
	{
		curRel = sti(pgg.PGGAi.relations.(chrID));
	}

	curRel += _addValue;
	if (curRel > 100) curRel = 100;
	if (curRel < 0) curRel = 0;

	PGG_DebugLog(pgg.id + " changed relation to " + chr.id + " " + _addValue);
	pgg.PGGAi.relations.(chrID) = curRel;
	return curRel;
}

//изменить отношение ПГГ к ГГ
//pgg - ПГГ, которому меняем, _addValue - величина изменения (+/-)
int PGG_ChangeRelation2MainCharacter(ref pgg, int _addValue)
{
	return PGG_ChangeRelation2Character(pgg, GetMainCharacter(), _addValue);
}

// LEO: цвет числа отношения ПГГ градиентом 0..100: 0=красный -> 50=жёлтый -> 100=зелёный. Возвращает RGB 0xRRGGBB для атрибута NameBonusColor (движок добавляет альфу). <41 враждебен, 50+ дружелюбен.
int PGG_RelationColorRGB(int rel)
{
	int r, g, b;
	if (rel < 0) rel = 0;
	if (rel > 100) rel = 100;
	b = 64;
	if (rel < 50)
	{
		r = 224;                             // красный -> жёлтый
		g = 64 + (200 - 64) * rel / 50;
	}
	else
	{
		r = 224 - (224 - 80) * (rel - 50) / 50; // жёлтый -> зелёный
		g = 200;
	}
	return r * 65536 + g * 256 + b;
}

// LEO: ПОЛНОЕ замирение ПГГ с игроком - снимает всё, чем его делали враждебным. Раньше «мир» правил
//   только число PGGAi.relations, и помирившийся через дипломата ПГГ звал игрока на общее дело в
//   таверне, а в море встречал огнём: AlwaysEnemy сильнее любого отношения, залоченная задача группы
//   держит атаку, нация PIRATE поднимает пиратский флаг. Обратная операция к PGG_MakeHunter.
//   Квестовую нацию (Nation.Bak) не трогаем - её возвращает сам квест.
void PGG_MakePeace(ref chr)
{
	string sGroup;

	if (!CheckAttribute(chr, "index")) return;

	DeleteAttribute(chr, "AlwaysEnemy");
	DeleteAttribute(chr, "PGG_Hunter");
	DeleteAttribute(chr, "PGG_warrior");
	DeleteAttribute(chr, "PGG_Convoy");
	DeleteAttribute(chr, "hunter");
	DeleteAttribute(chr, "DontRansackCaptain");
	DeleteAttribute(chr, "Coastal_Captain");
	DeleteAttribute(chr, "WatchFort");
	DeleteAttribute(chr, "Abordage.Enable");   // без атрибута абордаж и так разрешён (дефолт true)

	// военным энкаунтером его сделала охота; торговый рейс не трогаем, иначе собьём роль на глобалке
	if (CheckAttribute(chr, "mapEnc.type"))
	{
		if (chr.mapEnc.type == "war") DeleteAttribute(chr, "mapEnc.type");
	}
	// нация: охота уводит ПГГ в PIRATE, без возврата его бьют форты, а флаг читается пиратским
	if (CheckAttribute(chr, "nation_backup"))
	{
		chr.nation = chr.nation_backup;
		DeleteAttribute(chr, "nation_backup");
	}

	// нейтраль ко ВСЕЙ эскадре игрока: иначе компаньоны сами открывают огонь и мир рассыпается
	Ship_MakeNeutralToPlayerFleet(sti(chr.index));
	DeleteAttribute(chr, "CheckFlagYet");   // распознавание по флагу пройдёт заново

	// без снятия лока группа продолжает атаковать даже с мирным отношением
	sGroup = "Sea_" + chr.id;
	if (Group_FindGroup(sGroup) != -1)
	{
		Group_UnlockTask(sGroup);
		Group_SetTaskNone(sGroup);
	}
	PGG_Disband_Fleet(chr);   // внутри свой гейт по PGGAi.Task.SetSail
}

//уставновит нейтральные отношения ко всем остальным ПГГ и ГГ.
void PGG_SetRelation2OtherAsNormal(ref pgg)
{
	ref chr;
	string chrID;
	for (int i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);
		if (chr.id != pgg.id)
		{
			chrID = chr.id;
			pgg.PGGAi.relations.(chrID) = 50;
		}
	}
	chr = GetMainCharacter();
	chrID = chr.id;
	pgg.PGGAi.relations.(chrID) = 50;
}

