// LEO: Совместимость со старыми сейвами. Удалить вместе с вызовами при обязательной новой игре в 2.4.9.
string ReadLegacyParkedPistol(ref chr)
{
	string pistol = GetParkedPistol(chr);
	if (pistol == "" && CheckAttribute(chr, "IsMushketer.LastGunID")) pistol = chr.IsMushketer.LastGunID;
	if (pistol == "-1") return "";
	return pistol;
}

void MigrateLegacyParkedPistol(ref chr)
{
	if (!CheckAttribute(chr, "IsMushketer")) return;
	if (CheckAttribute(chr, "MusketLoadout.Drawn") && !CheckAttribute(chr, "MusketLoadout.Pistol"))
		chr.MusketLoadout.Pistol = ReadLegacyParkedPistol(chr);
	DeleteAttribute(chr, "IsMushketer");
	DeleteAttribute(chr, "Equip.TempGunID");
}

string ReadLegacyMusketKitMode(string mode)
{
	if (mode == "mushketer") return "rifle";
	return mode;
}

void MigratePlayerMusket(ref chr)
{
	if (!IsMainCharacter(chr)) return;
	DeleteAttribute(chr, "mushket.timer");
	DeleteAttribute(chr, "MushketSwap");
	DeleteAttribute(chr, "needmushreset");
	if (!CheckAttribute(chr, "model.animation")) return;
	if (!HasSubStr(chr.model.animation, "mushketer"))
	{
		MigrateLegacyParkedPistol(chr);
		return;
	}
	chr.model = CanonicalMusketModel(chr.model);
	chr.model.animation = MainChAnim;
	if (IsCharacterPerkOn(chr, "AgileMan")) chr.model.animation = MainChAnim + "_fast";
	string gun = GetCharacterEquipByGroup(chr, GUN_ITEM_TYPE);
	string pistol = ReadLegacyParkedPistol(chr);
	if (isMushket(gun)) chr.mushket = gun;
	DeleteAttribute(chr, "IsMushketer");
	DeleteAttribute(chr, "Equip.TempGunID");
	if (!isMushket(gun)) return;
	DeleteAttribute(chr, "equip.gun");
	if (pistol != "" && CheckCharacterItem(chr, pistol)) EquipCharacterByItem(chr, pistol);
	else LAi_GunSetUnload(chr);
}

// LEO: переносим старые имена только при чтении; в игре используются обычные модели.
string CanonicalMusketModel(string modelName)
{
	if (modelName == "PGG_Black_Mush") return "PGG_Black_0";
	if (modelName == "PGG_Doggerty_Mush") return "PGG_Doggerty_0";
	if (modelName == "Animists1_mush_x") return "Animists1";
	int count = strlen(modelName);
	if (count > 5 && strcut(modelName, count - 5, count - 1) == "_mush")
		modelName = strcut(modelName, 0, count - 6);
	int split = findsubstr(modelName, "_mush_", 0);
	if (split == 3 || split == 5)
		return strcut(modelName, 0, split - 1) + "_rifle_" + strcut(modelName, split + 6, strlen(modelName) - 1);
	return modelName;
}

void MigrateNpcMusket(ref chr)
{
	if (IsMainCharacter(chr) || !CheckAttribute(chr, "model.animation")) return;
	if (!HasSubStr(chr.model.animation, "mushketer"))
	{
		MigrateLegacyParkedPistol(chr);
		return;
	}
	if (!CheckAttribute(chr, "IsMushketer.MushketID"))
	{
		ConfigureGeneratedMusket(chr);
		return;
	}
	string gun = chr.IsMushketer.MushketID;
	string pistol = ReadLegacyParkedPistol(chr);
	chr.model = CanonicalMusketModel(chr.model);
	if (CheckAttribute(chr, "DefaultAnimation")) chr.model.animation = chr.DefaultAnimation;
	else
	{
		chr.model.animation = "man";
		if (chr.sex == "skeleton") chr.model.animation = "skeleton";
		if (chr.sex == "woman") chr.model.animation = "YokoDias";
	}
	DeleteAttribute(chr, "IsMushketer");
	DeleteAttribute(chr, "Equip.TempGunID");
	DeleteAttribute(chr, "DefaultAnimation");
	if (isMushket(gun) && SupportsMusketLoadout(chr))
	{
		chr.mushket = gun;
		chr.MusketLoadout.Drawn = true;
		chr.MusketLoadout.Pistol = pistol;
		return;
	}
	DeleteAttribute(chr, "mushket");
	DeleteAttribute(chr, "isMusketer");
	DeleteAttribute(chr, "MusketerDistance");
	if (isMushket(GetCharacterEquipByGroup(chr, GUN_ITEM_TYPE))) DeleteAttribute(chr, "equip.gun");
	if (pistol != "" && CheckCharacterItem(chr, pistol)) EquipCharacterByItem(chr, pistol);
}
