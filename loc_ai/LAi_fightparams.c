/*

	Возможные типы атаки attackType:
	"fast" быстрая атака
	"force" обычная
	"round" круговая
	"break" пробивающая блок
	"feint" атака после финта

*/
#define BLADE_DMG_RAND 0.1 // уровень рандома урона ножа
#define BLADE_EXP_MULT 0.12
#define BLOODING_MIN_TIME 10
#define SWIFT_TIME_RAND 2
#define STUN_TIME_RAND 2
#define TRAUMA_TIME_RAND 40
#define POISON_TIME_RAND 30
//--------------------------------------------------------------------------------
// LEO: Параметры холодного оружия.
//--------------------------------------------------------------------------------

//Расчитать базовый урон - стат клинка, навык, рандом
//#20200522-01
float LAi_CalcDamageForBlade_Base(aref attack, float aSkill, bool bIsRandomOn)
{
	//Расчитываем повреждение от сабли
	float min = 10.0;
	float max = 10.0;
	if(CheckAttribute(attack, "chr_ai.dmgbldmin")) {
		min = stf(attack.chr_ai.dmgbldmin);
	}
	if(CheckAttribute(attack, "chr_ai.dmgbldmax")) {
		max = stf(attack.chr_ai.dmgbldmax);
	}
	if (IsCharacterUsingMusket(attack)) {
		min = stf(Items[GetItemIndex(attack.equip.gun)].melee_dmg_min);
		max = stf(Items[GetItemIndex(attack.equip.gun)].melee_dmg_max);
	}

	//Коэфициент в зависимости от скилов
	float bladeDmg = 0;
	float atSkill = aSkill + BLADE_DMG_RAND * bIsRandomOn * (Random() - 0.5);
	if (atSkill < 0.0) { atSkill *= -1; }// чуть больше чем мин урон

	bladeDmg = (min + atSkill * (max - min))

	return bladeDmg;
}

//расчитать множитель от: вида удара, факта блока, перков, мушкета и опционалки "профильные удары" альт. баланса
//iAttackType 0 = fast, 1 = force, 2 = round, 3 = break, 4 = feint
float LAi_CalcDamageForBlade_AttackTypeMult(aref attack, int iAttackType, string sFencingType, bool isBlocked, bool blockSave)
{
	float kAttackDmg = 1.0;

	// TO_DO оптимизация на ветку параметров

	bool isAttTypeBreak = false;
	if (iAttacktype == 3) { isAttTypeBreak = true;}
	// множ урона в блок
	kAttackDmg *= (1 - 0.33 * isBlocked);
	bool bABProfHitPenalty = false;
	switch(iAttacktype) {
		case 0: // быстрая атака
			kAttackDmg *= 0.7;

			if (sFencingType != "Fencing") { bABProfHitPenalty = true; }
		break;

		case 1: // обычная атака

			if (sFencingType != "FencingLight") { bABProfHitPenalty = true; }
		break;

		case 2: // круговая атака
			kAttackDmg *= 0.6;

			kAttackDmg *= (1.0 + 0.3 * CheckCharacterPerk(attack, "BladeDancer"));
			if (sFencingType != "Fencing") { bABProfHitPenalty = true; }
			else { kAttackDmg *= 1.2; }// +20% конкретно для СО без учета профильных ударов
		break;

		case 3: // пробивающая блок
			kAttackDmg *= 2.0;

			kAttackDmg *= (1 + IsEquipCharacterByArtefact(attack, "talisman19"));
			if (sFencingType != "FencingHeavy") { bABProfHitPenalty = true; }
		break;

		case 4:  // фикс после изучения ядра //Атакующие продолжение финта
			if(isBlocked && blockSave) {
				kAttackDmg = 0.0;
			}
			else {
				kAttackDmg = 0.8;
			}

			kAttackDmg *= (1 + CheckCharacterPerk(attack, "Agent"));
			if (sFencingType != "FencingLight") { bABProfHitPenalty = true; }
		break;
	}
	// только пробивающий бьет в идеальный блок
	if (blockSave) { kAttackDmg = 0.8 * isAttTypeBreak;}

	kAttackDmg *= (1 - 0.3 * bAltBalanceProHits * bABProfHitPenalty);// профильные удары

	if (IsCharacterUsingMusket(attack)) {
		if (IsEquipCharacterByItem(attack, "mushket_SeaCarbine") && iAttacktype == 1) {
			kAttackDmg += 1.0;
		}
		if (iAttacktype == 0) { kAttackDmg += 0.5; }
		if (iAttacktype == 3) { kAttackDmg += 1.5; }
	}
	//Результирующий демедж
	return kAttackDmg; // *kArcadeDmg * kSkillDmg
}

//бонусы от перков - СКЛАДЫВАЕМ а не перемножаем их
float LAi_CalcDamageForBlade_PerksMult(aref attack)
{
	return (1.0 + CheckAttribute(attack, "perks.list.Rush.active") + 0.1 * IsEquipCharacterByArtefact(attack, "talisman1") + 0.15 * IsCharacterPerkOn(attack, "Grunt") + 0.1 * CheckAttribute(attack, "StrangeElixir"));  // берсерк атака + Пернатый Змей + Рубака + Убойная смесь
}

//итоговый множитель урона клинка
float LAi_CalcDamageForBlade(aref attack, float aSkill, int iAttackType, string sFencingType, bool isBlocked, bool blockSave, bool bIsRandomOn)
{
	float dmg = LAi_CalcDamageForBlade_Base(attack, aSkill, bIsRandomOn);
	dmg *= LAi_CalcDamageForBlade_AttackTypeMult(attack, iAttackType, sFencingType, isBlocked, blockSave);
	dmg *= LAi_CalcDamageForBlade_PerksMult(attack);
	return dmg;
}

string RecalculateMushketHitsType(aref attack)
{
	if (IsEquipCharacterByItem(attack, "mushket_SeaCarbine"))
	{
		return "force";
	}
	return "";
}

//Расчитать полученный опыт при ударе саблей
//#20200522-01
float LAi_CalcExperienceForBlade(aref attack, aref enemy, int iAttackType, bool isBlocked, float dmg, bool blockSave)
{
	float fEnemyHP = stf(enemy.chr_ai.hp);
	if (fEnemyHP < dmg) {
       dmg = fEnemyHP;
	}
	float exp = dmg * (0.8 + Random() * 0.2);

	if(iAttackType == 4) {//feintc
		exp *= 1.5;
	}
	if (CheckCharacterPerk(attack,"SwordplayProfessional")) {
		exp *= (1.5 + 0.03 * sti(attack.rank));
	}
	return exp;
}

//Энергия, необходимая для запуска действия
float LAi_CalcUseEnergyForBlade(aref character, string actionType, float fSkill)
{
	float energytype = LAi_GetBladeEnergyType(character);// = weight / 9 + 0.4
	float energy = 0.0;
	switch(actionType)
	{
		case "fast":
			energy = 8.0;
		break;
		case "force":
			energy = 7.0;
		break;
		case "round":
			energy = 16.0;
			if (energytype > 1.17) {//~7.0
				energy *= 1.5;
			}
		break;
		case "break":
			energy = 22.0;
		break;
		case "hit_parry":  // boal fix эту энергию тратит не атакующий, а атакуемый в анимации fgt_hit_parry
			energy = 16.0;
		break;
		case "feintc":
			energy = 7.0; // расход при успехе финта
		break;
	}
	 // честно все всем
	//чек на энергию ломает ИИ
	fSkill = 1.0 - 0.3 * fSkill;

	if (IsCharacterUsingMusket(character)) {
		energy = energy * fSkill * GetMushketEnergyDrain(character);
	}
	else {
		energy = energy * fSkill * (2.0 * energytype + 1.0) / 3.0;  // энергоемкость от веса
	}
	return energy;
}

float GetMushketEnergyDrain(ref character)
{
	if (!CheckAttribute(character,"equip")) return 0.0;
	return stf(Items[GetItemIndex(character.equip.gun)].weight)/10.0 + 0.2;
}

// LEO: скорость восстановления энергии (ед./сек) - одна формула и для боя, и для строки в F2.
float LAi_GetEnergyRegenRate(aref chr)
{
	float fMultiplier = 1.35+(GetCharacterSPECIALSimple(chr,SPECIAL_S)/20.0);// 1.5 ... 1.85 - влияние силы на скорость восстановления энергии

	if((MOD_SKILL_ENEMY_RATE == 3) || (CheckCharacterPerk(chr, "Energaiser"))) // скрытый перк боссов и ГГ
	{
		fMultiplier = fMultiplier * 1.5;
	}
	if(CheckCharacterPerk(chr, "Tireless"))
	{
		fMultiplier = fMultiplier * 1.15;
	}
	if(CheckAttribute(chr, "bonusEnergy"))
	{
		fMultiplier = fMultiplier * 2;
	}
	// Gregg: лунный камень призрачного света ("Помощь святой инквизиции", перенос из К:ПС):
	// восстановление энергии на 20% быстрее.
	if(IsEquipCharacterByArtefact(chr, "MoonStone"))
	{
		fMultiplier = fMultiplier * 1.2;
	}
	return fMultiplier;
}

float Lai_UpdateEnergyPerDltTime(aref chr, float curEnergy, float dltTime, attack)
{
	return curEnergy + dltTime * LAi_GetEnergyRegenRate(chr);
}


//--------------------------------------------------------------------------------
// LEO: Параметры огнестрельного оружия.
//--------------------------------------------------------------------------------

// Расчёт вероятности попадания из огнестрельного оружия, новая формула Assainisateur 09.2025
float LAi_GunCalcProbability(aref attack, aref enemy, float kDist, float fAttackerPistolSkill)// возвращает значения строго в диапазоне 0.0 – 1.0
{
	if (kDist >= 0.924909) return 1.0;// 0.924909 - это предельно близкий возможный показатель дистанции выстрела в упор в текущем балансе
	//                                             (самый косой ствол:             базовая точность 0.1
	//                                              в руках самого косого стрелка: Восприятие 1, Пистолеты 1
	//                                              в самых плохих условиях:       у стрелка НЕТ 'Сына ягуара', у цели ЕСТЬ Шкура ягуара)
	//                                   соответственно, если дистанция выстрела ближе этой, то промах невозможен даже в теории и все дальнейшие расчёты не нужны
	
	float fAccMod = 0.0;
	float fAccAdj = 0.0;
	if (CheckAttribute(attack, "chr_ai.accuracy"))
	{
		fAccMod = stf(attack.chr_ai.accuracy);// считываем точность из ТТХ экипированного оружия (если таковое экипировано)
		switch (MakeInt(fAccMod * 100.0))//      устанавливаем поправочные коэффициенты для каждого значения точности с шагом в 5
		{
			case 5: fAccAdj = 0.819336337566571; break;
			case 10: fAccAdj = 0.892060660124888; break;
			case 15: fAccAdj = 0.918976872415378; break;
			case 20: fAccAdj = 0.932835820895522; break;
			case 25: fAccAdj = 0.941176470588235; break;
			case 30: fAccAdj = 0.946670874092774; break;
			case 35: fAccAdj = 0.950505804874737; break;
			case 40: fAccAdj = 0.953288846520496; break;
			case 45: fAccAdj = 0.955363303434; break;
			case 50: fAccAdj = 0.956937799043062; break;
			case 55: fAccAdj = 0.958146422194155; break;
			case 60: fAccAdj = 0.959079283887468; break;
			case 65: fAccAdj = 0.959799180479161; break;
			case 70: fAccAdj = 0.960351214158321; break;
			case 75: fAccAdj = 0.960768614891914; break;
			case 80: fAccAdj = 0.961076405574243; break;
			case 85: fAccAdj = 0.961293788345727; break;
			case 90: fAccAdj = 0.96143574404444; break;
			case 95: fAccAdj = 0.961514131727436; break;
			case 100: fAccAdj = 1.0; break;
		}
	}
	
	fAccMod *= (0.9 + GetCharacterSPECIALSimple(attack, SPECIAL_P) * 0.01);// скейлинг от Восприятия
	
	fAccMod *= fAttackerPistolSkill;// скейлинг от умения Пистолеты
	
	if (IsCharacterPerkOn(attack, "GunProfessional")) {}// скейлинг от стрелковых перков
	else
	{
		if (IsCharacterPerkOn(attack, "Gunman")) fAccMod *= 0.88;
		else fAccMod *= 0.8;
	}
	
	if (!IsDay())
	{
		if (IsEquipCharacterByArtefact(attack, "talisman15")) fAccMod = Clampf(fAccMod * 2.0);// бонус от Талисмана 'Сын ягуара' у стрелка
	}
	
	if (IsEquipCharacterByArtefact(enemy, "talisman18")) fAccMod = 1.25 * fAccMod - 0.25;// штраф от Шкуры ягуара у цели
	
	float fPointBlankRange = 0.1 + 0.1 * fAccMod;// дистанция выстрела в упор
	
	// выстрелы в упор не промахиваются
	if (kDist >= (1 - fPointBlankRange)) return 1.0;
	
	return Clampf(fAccAdj * (sqr(kDist + fPointBlankRange) + fAccMod));
}

// Расчёт базового урона от огнестрельного оружия, ревью Assainisateur 09.2025
float LAi_GunCalcDamage(aref attack, aref enemy, float fArmorLevel, float fAttackerPistolSkill, int iMultiShot)
{
	// устанавливаем исходные показатели урона
	float min = 0.0;
	float max = 0.0;
	if (CheckAttribute(attack, "chr_ai.dmggunmin")) min = stf(attack.chr_ai.dmggunmin);
	if (CheckAttribute(attack, "chr_ai.dmggunmax")) max = stf(attack.chr_ai.dmggunmax);
	
	float fEnergyDamage = 0.0;
	if (fArmorLevel > 0.0)// принимаем во внимание наличие или отсутствие экипированной кирасы у цели
	{
		min = stf(attack.chr_ai.DmgMin_C);
		max = stf(attack.chr_ai.DmgMax_C);
		
		fEnergyDamage = stf(attack.chr_ai.EnergyP_C);// урон по энергии цели в кирасе
	}
	else
	{
		min = stf(attack.chr_ai.DmgMin_NC);
		max = stf(attack.chr_ai.DmgMax_NC);
		
		fEnergyDamage = stf(attack.chr_ai.EnergyP_NC);// урон по энергии цели без кирасы
	}
	if (fEnergyDamage > 0.0) Lai_CharacterChangeEnergy(enemy, -fEnergyDamage);// наносим урон по энергии цели
	
	// считаем базовый урон
	// верхняя граница разброса урона повышается с ростом умения "Пистолеты" атакующего
	// 100 умения "Пистолеты" дают случайное значение урона в полном диапазоне между min и max ТТХ оружия
	float dmg = 0.0;
	for (int i = 0; i < iMultiShot; i++)
	{
		dmg += min + (max - min) * FRAND(fAttackerPistolSkill);
	}
	
	// учитываем текущую сложность игры (получаемый игроком урон)
	if (sti(enemy.index) == GetMainCharacterIndex())
	{
		dmg = (dmg * (4.0 + MOD_SKILL_ENEMY_RATE * 3) / 10.0) * bModDamage;
	}
	
	if (IsEquipCharacterByArtefact(attack, "talisman18")) dmg *= 1.25;// бонус от Шкуры ягуара
	
	return dmg;
}

// Расчёт опыта за урон из огнестрельного оружия, ревью Assainisateur 09.2025
float LAi_GunCalcExperience(aref attack, aref enemy, float dmg)
{
	float fEnemyHP = stf(enemy.chr_ai.hp);// проверяем хп цели...
	if (fEnemyHP < dmg) dmg = fEnemyHP;// ... чтобы не нанести урона больше, чем есть хп
	
	dmg = dmg * (0.8 + 0.2 * Random());// опыт рандомизируется от урона в диапазоне от 0.8 до 1.0
	
	if (IsCharacterUsingMusket(attack)) dmg *= 0.8;// мушкетёры получают на 20% меньше опыта (??? зачем-почему? оригинальный дизайн, без изменений)
	
	if (!bRechargePistolOnLine) dmg *= 2;// без включённой дозарядки опыт удваивается
	
	return dmg;
}

// Расчёт текущей скорости перезарядки огнестрельного оружия, новая формула Assainisateur 09.2025
float LAi_GunReloadSpeed(aref chr)
{
	float charge_dlt = LAI_DEFAULT_DLTCHRG;// значение по умолчанию из константы
	if (CheckAttribute(chr, "chr_ai.charge_dlt")) charge_dlt = stf(chr.chr_ai.charge_dlt);// считываем показатель из ТТХ экипированного оружия (если таковое экипировано)
	
	charge_dlt *= (0.85 + GetCharacterSPECIALSimple(chr, SPECIAL_A) * 0.02);// скейлинг от Реакции
	
	charge_dlt *= (0.94127 + 0.15873 * LAi_GetCharacterGunLevel(chr));// скейлинг от умения Пистолеты
	
	// бонусы от стрелковых перков и способности Буканьера
	if (IsCharacterPerkOn(chr, "GunProfessional")) charge_dlt *= (1.25 + 0.25 * IsCharacterPerkOn(chr, "Buccaneer"));
	else
	{
		if (IsCharacterPerkOn(chr, "Gunman")) charge_dlt *= (1.1 + 0.25 * IsCharacterPerkOn(chr, "Buccaneer"));
		else charge_dlt *= (1.0 + 0.25 * IsCharacterPerkOn(chr, "Buccaneer"));
	}
	
	return charge_dlt;
}

// LEO: Всплывающий текст эффекта над персонажем, если он включён в настройках.
/*!
 * \brief Запуск всплывающего текста над персонажем.
 * Используется для текстового отображения эффектов.
 * \param chref объект персонажа (в том числе и ГГ)
 * \param text текст, который улетит в небо примерно через 3с
 * \param color цвет текста
 * \param scale масштаб текста
 */
void setEffectText(ref chref, string text, int color, float scale)
{
	if(sti(InterfaceStates.CharCap) && sti(InterfaceStates.CharCapShowNotifications))
	{
		SendMessage(chref, "lslfs", MSG_CHARACTER_SET_EFFECT_TEXT, text, color, scale*0.75, "CHARACTHER_POPUP_TEXT");
	}
}

//Поломка катаром оружия - Gregg
void CheckForWeaponBreak(ref attack, ref enemy) 
{
	if(!HasSubStr(attack.equip.blade, "katar")) {
		return;
	}
	if (rand(19) != 0) {
		return;
	}

	string weaponID = GetCharacterEquipByGroup(enemy, BLADE_ITEM_TYPE);
	aref weapon;
	Items_FindItem(weaponID, &weapon);
	if(weapon.model == "unarmed" || weapon.quality == "excellent") {//Не отличное и не безоружный
		return;
	}

	if(GetCharacterItem(enemy, weaponID) <= 1) { RemoveCharacterEquip(enemy, weapon.groupID); }
	TakeItemFromCharacter(enemy, weaponID);

	string sEquipItem = GetGeneratedItem("unarmed");
	AddItems(enemy, sEquipItem, 1);
	EquipCharacterbyItem(enemy, sEquipItem);

	setEffectText(enemy, XI_ConvertString("WeaponBreak"), argb(255,255,255,255), GetCharCapNotifScale());
	
	PlaySound("interface\Crash_"+rand(2)+".wav");
	return;
}

int LAi_CalcCritChance(ref attack, int iAttackType, string fencing_type, int valueCrB)
{
	int critchance = 0;
	critchance += 10 * IsCharacterPerkOn(attack, "SwordplayProfessional");
	critchance += 5 * IsCharacterPerkOn(attack, "CriticalHit");
	critchance += 10 * IsCharacterPerkOn(attack, "Fencer");
	if (iAttackType == 4) {//финт
		critchance += 33 * IsCharacterPerkOn(attack, "Agent"); 
	}
	if(critchance > 0)	{
		critchance += GetCharacterSPECIALSimple(attack, SPECIAL_L);
		critchance += valueCrB;//доп крит
		if (iAttackType == 2 && fencing_type == "FencingLight") {//круговой ЛО
			critchance += 15; 
		}
	}
	return critchance;
}

// LEO: прибавка к урону за победы на турнирах арены. Счёт хранится в PGG_ArenaWins у самого конкурента
// (Arena.c: PGG_ArenaAwardWinner) и копируется в его слот на арене. Зовётся только в ветке урона по ГГ,
// поэтому проверять цель здесь не нужно. Огнестрел не трогаем - это школа клинка.
float PGG_ArenaWinsDamageMult(aref attack)
{
	float fMult;
	if (!CheckAttribute(attack, "PGG_ArenaWins")) return 1.0;
	fMult = 1.0 + 0.05 * sti(attack.PGG_ArenaWins);
	return fMult;
}

float LAi_CalcDodgeChanceMelee(ref enemy)
{
	float dodgechance = 0.0;
    if (IsCharacterPerkOn(enemy, "Fencer")) dodgechance += 0.15;
    dodgechance += makefloat(GetCharacterSkill(enemy, SKILL_FORTUNE)/1000.0);
	return dodgechance;
}

// LEO: ЭТО ЖЕ значение идёт в окно персонажа, поле "Уклон. ХО/выстрел". Раньше туда шла отдельная
//   формула, добавлявшая Шкуре ягуара плоские 25%: обещали 49% там, где работали 32%. Шкура НЕ даёт
//   уклонения - она правит точность стрелка как 1.25*acc-0.25, то есть меткому не мешает вовсе,
//   и итог зависит от дистанции. Плоским процентом не выражается, в этом поле ей не место.
float LAi_CalcDodgeChanceRanged(ref enemy)
{
	float dodgechance = 0.0;
	dodgechance += LAi_GetCharacterLuckLevel(enemy) / 10.0;
	if(CheckCharacterPerk(enemy, "AgileMan")) dodgechance += 0.25;
	if(CheckCharacterPerk(enemy, "Fencer")) dodgechance += 0.15;
	return dodgechance;
}

//бонусный урон крита от удачи
float LAi_CalcCritDamageMult(ref attack) {
	return (2 + 0.05 * GetCharacterSPECIALSimple(attack, SPECIAL_L));
} 

//чекает на смерть. если убило, то возвращает экспу и начисляет опыт в везение, авторитет и защиту
float LAi_CalcExpForBladeKill(ref attack, ref enemy, bool isSetBlade)
{
	if (!LAi_IsDead(enemy)) {
		return 0.0;
	}
	float experience = 0;
	// boal  check skill -->
	float ra = 1.0;
	float re = 1.0;
	if(CheckAttribute(attack, "rank")) {
		ra = stf(attack.rank);
	}
	if(CheckAttribute(enemy, "rank")) {
		re = stf(enemy.rank);
	}
	if (re < 4.0) {
		experience = GetCharacterSPECIALSimple(attack, SPECIAL_I) + rand(GetCharacterSPECIALSimple(attack, SPECIAL_A)); 
	}
	else {
		experience = Lai_GetCharacterMaxHP(enemy)/re + GetCharacterSPECIALSimple(attack, SPECIAL_I) * re / ra;;//Lipsar передeлка опыта
	}
	AddCharacterExpToSkill(attack, SKILL_DEFENCE, 1);
	AddCharacterExpToSkill(attack, SKILL_FORTUNE, 1);
	AddCharacterExpToSkill(attack, SKILL_LEADERSHIP, 1);
	// boal <--
	// boal statistic info 17.12.2003 -->
	Statistic_KillChar(attack, enemy, "_s");
	// boal statistic info 17.12.2003 <--
	LAi_SetResultOfDeath(attack, enemy, isSetBlade, true);
	return experience;
}

//--------------------------------------------------------------------------------
// LEO: Итоговый урон.
//--------------------------------------------------------------------------------

//Начисление повреждений при атаке мечём
//#20200522-01
void LAi_ApplyCharacterAttackDamage(aref attack, aref enemy, string fencing_type, string attackType, bool isBlocked, bool blockSave, float aSkill)
{
	//Если неубиваемый, то нетрогаем его
	if(CheckAttribute(enemy, "chr_ai.immortal"))	{
		if(sti(enemy.chr_ai.immortal) != 0)	{
			return;
		}
	}
	if(CheckAttribute(attack, "chr_ai.group") && CheckAttribute(enemy, "chr_ai.group"))	{
		if(attack.chr_ai.group == enemy.chr_ai.group)	{
			return;
		}
	}
    
	int dodgechance = makeint(LAi_CalcDodgeChanceMelee(enemy)*100.0);
    if (pnrand(enemy,makefloat(dodgechance),"dodgemelee")) //уклонение в ближнем бою
    {
		setEffectText(enemy, XI_ConvertString("MeleeEvade") + " (" + dodgechance + ")%", argb(255,255,180,180), GetCharCapNotifScale());
        return;
    }
	if (IsIntegratedMusketDrawn(attack))
	{
		// LEO: приклад наносит тупой урон, не наследует саблю, кровотечение и пробитие кирасы.
		if (blockSave) return;
		float blunt = 5.0 + GetCharacterSPECIALSimple(attack, SPECIAL_S) * 1.5;
		if (attackType == "force" || attackType == "break") blunt *= 1.35;
		if (isBlocked) blunt *= 0.50;
		LAi_ApplyCharacterDamage(enemy, MakeInt(blunt + 0.5));
		LAi_CheckLethalDamage(enemy, sti(attack.index), false, false);
		return;
	}

	int iFencingType = 0;
	switch (fencing_type) {
		case "FencingLight": iFencingType = 0; break;//условный дефолт
		case "Fencing": iFencingType = 1; break;
		case "FencingHeavy": iFencingType = 2; break;
	}
	int iAttackType = 0;
	switch (attackType) {
		case "fast": iAttackType = 0; break;//условный дефолт
		case "force": iAttackType = 1; break;
		case "round": iAttackType = 2; break;
		case "break": iAttackType = 3; break;
		case "feint": iAttackType = 4; break;
	}
	float coeff = 0.0;

	int nAttackIdx = sti(attack.index);
	int nEnemyIdx = sti(enemy.index);
	float eSkill = LAi_GetCharacterFightLevel(enemy);
	int valueCrB = sti(attack.chr_ai.special.valueCrB);
	bool bEnemyRush = CheckAttribute(enemy, "perks.list.Rush.active");
	bool bAttackHardHitter = CheckCharacterPerk(attack, "HardHitter");
	//--->Пробитие блоков - Gregg
	//<---Пробитие блоков тяжёлым оружием

	bool cirign = false;
	string sEnemyCuirassID;
	int iEnemyCuirassID = 0;
	if (CheckAttribute(enemy, "cirassId")) {
		sEnemyCuirassID = enemy.cirassId;
		iEnemyCuirassID = sti(sEnemyCuirassID);
	}
	int iDmgAdd = 0;
	if (!blockSave && !isBlocked) {
		iDmgAdd += CheckForBlooding(attack, nAttackIdx, enemy, nEnemyIdx, iFencingType, aSkill, iAttackType);
		if (!bEnemyRush) {
			iDmgAdd += CheckForSwift(attack, nAttackIdx, enemy, nEnemyIdx, iFencingType, aSkill);
			CheckForStun(attack, nAttackIdx, enemy, nEnemyIdx);
		}
		if (iFencingType == 2) {//только ТО
			CheckForTrauma(attack, nAttackIdx, enemy, nEnemyIdx, iEnemyCuirassID)
		}
		cirign = CheckForCirassBreak(attack, nAttackIdx, enemy, nEnemyIdx, iFencingType, bAttackHardHitter, iEnemyCuirassID, aSkill);
	}
	else {
		CheckForWeaponBreak(attack, enemy);// Поломка катаром оружия - Gregg 
    }

	// дамаг начало
	float dmg = LAi_CalcDamageForBlade(attack, aSkill, iAttackType, fencing_type, isBlocked, blockSave, true);
	if(CheckAttribute(enemy, "sex") && enemy.sex == "skeleton") {
		dmg *= (1 + 0.33 * IsEquipCharacterByArtefact(attack, "talisman9"));
	}
	// модификатор от сложности
	if (nEnemyIdx == GetMainCharacterIndex()) {
		dmg = (dmg * (4.0 + MOD_SKILL_ENEMY_RATE*3) / 10.0) * bModDamage;

		// Warship 27.08.09 Для сильных противников
		// Если долбить совсем сильных (хардкорные абордажи), то шансов взять шип будет меньше
		if (CheckAttribute(loadedLocation, "CabinType")) {
			dmg *= (1.0 + stf(attack.rank)/100);
		}
		// LEO: турнирная школа конкурента. Каждая победа на арене Доминики = +5% урона, но ТОЛЬКО по ГГ:
		//   это личное соперничество, на прочих его призовой опыт не отражается.
		dmg *= PGG_ArenaWinsDamageMult(attack);
	}

	// дамаг защита
	dmg *= (1.0 - 0.5 * IsCharacterPerkOn(enemy, "Rush"));

	float kDmg = 1.0;// сборный множитель
	kDmg -= 0.1 * IsCharacterPerkOn(enemy, "SwordplayProfessional");
	kDmg -= 0.1 * IsCharacterPerkOn(enemy, "AdvancedDefense");
	kDmg -= 0.1 * IsCharacterPerkOn(enemy, "BasicDefense");
	if (CheckAttribute(attack, "sex") && attack.sex == "skeleton") {	
		kDmg -= (0.33 * IsEquipCharacterByArtefact(enemy, "talisman9"));
	}
	
	// Warship 27.08.09 Для сильных противников
	// Если долбить совсем сильных (хардкорные абордажи), то шансов взять шип будет меньше
	if (sti(enemy.Rank) > 50) {
		dmg *= (45.0 / stf(enemy.Rank));
	}
	if (aSkill < eSkill) {
		dmg *= (1.0 + 0.7 * (aSkill - eSkill));
	}

	float fCuirassResist = 0.0;
	if(iEnemyCuirassID) {//сопротивления уронам у кирас
		switch (iAttackType) {
			case 0: fCuirassResist = stf(Items[iEnemyCuirassID].CirassLevel.fast); break;
			case 1: fCuirassResist = stf(Items[iEnemyCuirassID].CirassLevel.force); break;
			case 2: fCuirassResist = stf(Items[iEnemyCuirassID].CirassLevel.round); break;
			case 3: fCuirassResist = stf(Items[iEnemyCuirassID].CirassLevel.break); break;
			case 4: fCuirassResist = stf(Items[iEnemyCuirassID].CirassLevel.feint); break;
		}
	}

	// ГПК 1.2.3
	dmg = kDmg * dmg;
	if (CheckAttribute(attack, "FrenzyActive")) dmg *= 5.0;
	// дамаг конец
	if (dmg < 1.0) {
		return;
	}

	// крит начало
	float critical = 0.0;
	int critchance = LAi_CalcCritChance(attack, iAttackType, fencing_type, valueCrB);

	// блочим криты обычным блоком?
	if (!isBlocked && pnrand(attack,makefloat(critchance),"critmelee"))	{//"<=" не нужно 
		critical = 1.0;
	}

	// крит защита
	if (critical > 0.0) {
		int iCritDefPrc = 0;
		if(!cirign && iEnemyCuirassID) {// защиты от критов у кирас
			switch (Items[iEnemyCuirassID].id) {// внутреннее имя предмета
				case "cirass1": iCritDefPrc += 40; break;
				case "cirass2": iCritDefPrc += 60; break;
				case "cirass3": iCritDefPrc += 50; break;
				case "cirass4": iCritDefPrc += 80; break;
				case "cirass5": iCritDefPrc += 100; break;
			}
		}
		if (pnrand(enemy,makefloat(iCritDefPrc),"critDefmelee")) {
			critical = 0.0;
			setEffectText(enemy, XI_ConvertString("CritBlock"), argb(255,255,255,255), GetCharCapNotifScale());
		}
	}
	// крит опыт и множитель
	if (critical > 0.0) {
        AddCharacterExpToSkill(attack, SKILL_FORTUNE, 5);
		critical = LAi_CalcCritDamageMult(attack);
		
		if(nAttackIdx == GetMainCharacterIndex()) {
			setEffectText(enemy, XI_ConvertString("Critical Hit"), argb(255,128,255,128), GetCharCapNotifScale());
			Log_TestInfo(" " + critical + GetConvertStrWithReplace("Variable_LAi_fightparams_6", "Logs.txt", "#space#", " "));

			int iCriticalCount = sti(pchar.questTemp.criticalcount) + 1;
			pchar.questTemp.criticalcount = iCriticalCount;
			
			// Открываем достижения
			CheckAndUnlock("criticals", iCriticalCount);
		}
	}
	else {
		critical = 1.0;//оверрайдим
	}
	float fTempDmg = dmg * fCuirassResist;
	dmg *= (1.0 - fCuirassResist);
	dmg *= critical;
	dmg += cirign * fTempDmg; //если крит И пробитие кирасы - ослабляем
	dmg += kDmg * MakeFloat(iDmgAdd);
	// крит конец

	//Начисляем опыт - ДО нанесения урона
	float exp = LAi_CalcExperienceForBlade(attack, enemy, iAttackType, isBlocked, dmg, blockSave);
	
	LAi_ApplyCharacterDamage(enemy, MakeInt(dmg + 0.5));

	//Проверим на смерть
	enemy.chr_ai.KilledByIDx = nAttackIdx; 
	LAi_CheckKillCharacter(enemy);
	//проверим на отравление
	MakePoisonAttackCheckSex(attack, enemy);
	//Есть ли оружие у цели
	bool isSetBlade = (CheckAttribute(enemy, "equip.blade"));
	if (CheckAttribute(attack,"vampire") || sti(attack.chr_ai.special.valueV) > 0) {
		float hp = attack.chr_ai.hp;
		float maxhp = attack.chr_ai.hp_max;
		float hpup = dmg / 2.0;
		log_Testinfo(FloatToString(hpup, 2));
		if ((hp + hpup) >= maxhp) { attack.chr_ai.hp = maxhp; }
		else { attack.chr_ai.hp = hp + hpup; }

		SendMessage(attack, "lfff", MSG_CHARACTER_VIEWDAMAGE, hpup, MakeFloat(attack.chr_ai.hp), MakeFloat(attack.chr_ai.hp_max));
	}

	if(CheckAttribute(enemy, "chr_ai.energy")) {
		float fEnergy = stf(enemy.chr_ai.energy);
		fEnergy *= (1.0 - 0.1 * bAttackHardHitter); //fix
		enemy.chr_ai.energy = fEnergy * isEquippedArtefactUse(attack, "talisman10", 1.0, 0.9); // отнимаем энергию
        if (IsEquipCharacterByArtefact(attack, "talisman10")) attack.chr_ai.energy = stf(attack.chr_ai.energy) + (fEnergy * 0.1); //добавляем энергию атакующему
	}
	if(!isSetBlade) {
		return;//не начисляем опыт
	}


    AddCharacterExpToSkill(attack, fencing_type, Makefloat(BLADE_EXP_MULT * exp));
}

int LAi_CalcBloodingChance(ref attack, int iFencingType, float aSkill, int iAttackType)
{
	if (iAttackType == 2) {//не круговой
		return 0;
	}
	float coeff = 0.0;
	if (iFencingType == 0) {
		coeff = 2.5 + 5.0 * aSkill;
	}
	return (sti(attack.chr_ai.special.valueB) + MakeInt(coeff));
}

float LAi_CalcBloodingDuration(ref attack, int iFencingType, float aSkill, bool bIsRandomOn) 
{
	float coeff = 0.0;
	if (iFencingType == 0) {
		coeff = 5.0 * aSkill;
	}
	else {
		coeff = 3.0; //для урона СО/ТО
	}
	return (BLOODING_MIN_TIME + 5 * coeff * (1 - bIsRandomOn * Random()));// отнимаем рандом
}

//возвращает доп урон если не нанесло кровоток. доп урон при iFencingType = 0 (Легкое оружие)
int CheckForBlooding(ref attack, int nAttackIdx, ref enemy, int nEnemyIdx, int iFencingType, float aSkill, int iAttackType)
{
	int valueB = LAi_CalcBloodingChance(attack, iFencingType, aSkill, iAttackType);
	if (valueB <= 0) { 
		return 0;
	}
	float coeff = LAi_CalcBloodingDuration(attack, iFencingType, aSkill, 1);
	
	if (!pnrand(attack,  valueB, "bleed")) {//основной чек 
		return 0;
	}
	if(nAttackIdx == GetMainCharacterIndex()) {
		PlaySound("Fx_Krovotok");
		pchar.questTemp.bloodingcount = sti(pchar.questTemp.bloodingcount) + 1;
	}
	if(IsEquipCharacterByArtefact(enemy, "talisman9") && rand(9)>0) {//чек на сввятую чашу
		setEffectText(enemy, GetConvertStrWithReplace("Variable_LAi_fightparams_10", "Logs.txt", "#space#", " "), argb(255,255,255,255), GetCharCapNotifScale());
		return 0;
	}
	PlaySound("Fx_Krovotok");
	if(CheckAttribute(enemy, "sex") && enemy.model.animation != "Terminator" && enemy.sex != "skeleton") {
		setEffectText(enemy, XI_ConvertString("Blood"), argb(255,255,200,200), GetCharCapNotifScale());
		MakeBloodingAttack(enemy, attack, coeff);
	}
	else {
		setEffectText(enemy, XI_ConvertString("DeepWound"), argb(255,255,255,255), GetCharCapNotifScale());
		return MakeInt(3.0 * coeff);
	}
	return 0;
}

int LAi_CalcSwiftChance(ref attack, int iFencingType, float aSkill)
{
	float coeff = 0.0;
	if (iFencingType == 1) {
		coeff = 2.5 + 5.0 * aSkill;
	}
	return (sti(attack.chr_ai.special.valueSS) + MakeInt(coeff));
}

float LAi_CalcSwiftDuration(ref attack, int iFencingType, float aSkill, bool bIsRandomOn) 
{
	float coeff = 0.0;
	if (iFencingType == 1) {
		coeff = 2.5 * aSkill;
	}
	else {
		coeff = 1.5; //для ЛО/ТО
	}
	return (1 + bIsRandomOn * rand(SWIFT_TIME_RAND) + coeff);
}

int CheckForSwift(ref attack, int nAttackIdx, ref enemy, int nEnemyIdx, int iFencingType, float aSkill)
{
	int valueSS = LAi_CalcSwiftChance(attack, iFencingType, aSkill);
	if (valueSS <= 0) { 
		return 0;// не вызываем pnrand лишний раз
	}
	float coeff = LAi_CalcSwiftDuration(attack, iFencingType, aSkill, 1);

	if (!pnrand(attack, valueSS, "swift")) {//основной чек
		return 0;
	}

	if(nAttackIdx == GetMainCharacterIndex()) {
		pchar.questTemp.swiftcount = sti(pchar.questTemp.swiftcount) + 1;
	}

	if(CheckAttribute(enemy, "sex") && enemy.model.animation != "Terminator" && enemy.sex != "skeleton") {
		setEffectText(enemy, XI_ConvertString("StanS"), argb(255,255,255,255), GetCharCapNotifScale());
		PlaySound("Fx_Stan");
		MakeSwiftAttack(enemy, attack, coeff);
	}
	else {
		setEffectText(enemy, XI_ConvertString("DeepWound"), argb(255,255,255,255), GetCharCapNotifScale());
		PlaySound("Fx_Krovotok");
		return MakeInt(7.5 + 7.5 * coeff);
	}
	return 0;
}

void CheckForStun(ref attack, int nAttackIdx, ref enemy, int nEnemyIdx)
{
	int valueStS = sti(attack.chr_ai.special.valueStS);
	if (valueStS == 0) {
		return;// не вызываем pnrand лишний раз
	}
	if (!pnrand(attack, valueStS, "stun")) {
		return;
	}

	if(nAttackIdx == GetMainCharacterIndex())
	{
		pchar.questTemp.stuncount = sti(pchar.questTemp.stuncount) + 1;
	}
	setEffectText(enemy, XI_ConvertString("Stunned"), argb(255,255,255,255), GetCharCapNotifScale());
	PlaySound("Fx_Stan");
	MushketStun(enemy);
	return;
}

//шанс травмы. Чекает кирасу цели при iEnemyCuirassID > 0
int LAi_CalcTraumaChance(ref attack, int iEnemyCuirassID)
{
	float valueT = sti(attack.chr_ai.special.valueT);
	if (iEnemyCuirassID) {
		valueT *= 0.833;
	}
	return MakeInt(valueT);
}

void CheckForTrauma(ref attack, int nAttackIdx, ref enemy, int nEnemyIdx, int iEnemyCuirassID)
{
	int valueT = LAi_CalcTraumaChance(attack, iEnemyCuirassID);
	if (valueT == 0) {
		return;
	}
	if (!pnrand(attack, valueT, "injury")) {//основной чек
		return;
	}

	if (!CheckAttribute(enemy,"chr_ai.TraumaQ")) { enemy.chr_ai.TraumaQ = 1; }
	else { enemy.chr_ai.TraumaQ = sti(enemy.chr_ai.TraumaQ)+1; }

	if(nAttackIdx == GetMainCharacterIndex()) {
		pchar.questTemp.traumacount = sti(pchar.questTemp.traumacount) + 1;
	}
	setEffectText(enemy, XI_ConvertString("Travma"), argb(255,255,255,255), GetCharCapNotifScale());
	PlaySound("Fx_HrustBones");
	if (CheckAttribute(enemy,"chr_ai.TraumaQ") && sti(enemy.chr_ai.TraumaQ) > 2) {
		setEffectText(enemy, XI_ConvertString("TravmaH"), argb(255,255,255,255), GetCharCapNotifScale());
		enemy.chr_ai.HeavyTrauma = 2;
		SetNoRun(enemy);
	}
	MakeTraumaAttack(enemy, attack);
	return;
}

int LAi_CalcCuirassBreakChance(ref attack, int iFencingType, bool bAttackHardHitter, float aSkill)
{
	float valueCB = stf(attack.chr_ai.special.valueCB) + 5.0 * bAttackHardHitter;
	if (iFencingType == 2) {
		valueCB += (5.0 + 10.0 * aSkill);
	}
	valueCB += 3.0 * HasSubStr(attack.equip.blade, "topor");
	return MakeInt(valueCB);
}

bool CheckForCirassBreak(ref attack, int nAttackIdx, ref enemy, int nEnemyIdx, int iFencingType, bool bAttackHardHitter, int iEnemyCuirassID, float aSkill) 
{
	if (!iEnemyCuirassID) {
		return 0;
	}
	int valueCB = LAi_CalcCuirassBreakChance(attack, iFencingType, bAttackHardHitter, aSkill);
	if (valueCB <= 0) {
		return 0;
	} 
	if (!pnrand(attack, valueCB, "cpen")) {//основной чек
		return 0;
	}

	setEffectText(enemy, XI_ConvertString("CirassBreak"), argb(255,255,255,255), GetCharCapNotifScale());
	PlaySound("Fx_Breaking");
	return 1;
}

//boal 19.09.05 -->
void LAi_SetResultOfDeath(ref attack, ref enemy, bool isSetBlade, bool allowBladeEffects)
{
    if (sti(attack.index) == GetMainCharacterIndex())
    {
		// LEO: У этой стычки собственные штрафы и награды. Национальность бандита задаётся при его создании.
		string gangWarSide = GangWar_AttackSide(enemy);
		GangWar_RecordAttack(gangWarSide);
		if (gangWarSide == "")
		{
			if (GetRelation2BaseNation(sti(enemy.nation)) == RELATION_ENEMY)
			{
				if (!isSetBlade)
				{
					ChangeCharacterReputation(attack, -1);   // to_do
					if (rand(1) && CheckAttribute(enemy, "City"))
					{
						ChangeCharacterHunterScore(attack, NationShortName(sti(enemy.nation)) + "hunter", 1);
					}
				}
			}
			else
			{
				if (CheckAttribute(enemy, "City"))
				{
					ChangeCharacterHunterScore(attack, NationShortName(sti(enemy.nation)) + "hunter", 2);
				}
			}
			// обида нации на разборки в городе boal 19.09.05
	  		if (CheckAttribute(enemy, "City"))
			{
				// нужна проверка на дуэли и квесты
				if (GetSummonSkillFromName(attack, SKILL_SNEAK) < rand(140)) // скрытность
				{
				    SetNationRelation2MainCharacter(sti(enemy.nation), RELATION_ENEMY);
			    }
			}
		}
		// LEO: За намеренную помощь банде штраф начисляется в исходе. Гибель своего солдата штрафуется сразу.
		if (gangWarSide == "bandits")
		{
			string gwSide = "";
			if (CheckAttribute(pchar, "GangWar.side")) gwSide = pchar.GangWar.side;
			if (gwSide != "bandits")
			{
				ChangeCharacterNationReputation(attack, sti(enemy.nation), -2);
				ChangeCharacterReputation(attack, -1);
			}
		}
		if (allowBladeEffects && startherotype == 8)
		{
			if (CheckAttribute(pchar,"equip.blade") && HasSubStr(pchar.equip.blade, "Lilarcor"))
			{
				if (!CheckAttribute(pchar,"LilarcorKills")) pchar.LilarcorKills = 0;
				pchar.LilarcorKills = sti(pchar.LilarcorKills)+1;
				if (sti(pchar.LilarcorKills) == 300)
				{
					LAi_CharacterPlaySound(PChar, "Lilarcor_Up1");
					DeleteAttribute(pchar,"items.Lilarcor_Sword1");
					AddItems(pchar, "Lilarcor_Sword2", 1);
					EquipCharacterbyItem(pchar, "Lilarcor_Sword2");
				}
				if (sti(pchar.LilarcorKills) == 800)
				{
					LAi_CharacterPlaySound(PChar, "Lilarcor_Up2");
					DeleteAttribute(pchar,"items.Lilarcor_Sword2");
					AddItems(pchar, "Lilarcor_Sword3", 1);
					EquipCharacterbyItem(pchar, "Lilarcor_Sword3");
				}
				if (sti(pchar.LilarcorKills) == 1800)
				{
					LAi_CharacterPlaySound(PChar, "Lilarcor_Victory");
					string sEquipItem = GetGeneratedItem("blade48");
					AddItems(pchar, sEquipItem, 1);
					EquipCharacterbyItem(pchar, sEquipItem);
					RemoveItems(pchar, "Lilarcor_Sword3", 1);
				}
			}
		}
	}
}
// boal <--

// LEO: ===== БОЕПРИПАСЫ, ОТ КОТОРЫХ НЕ УКЛОНИТЬСЯ =====
// Обе картечи бьют веером, взрывчатка - по площади: увернуться от них нельзя ни телом, ни ловкостью.
// Уклонений в моде ДВА, и список должен быть общим для обоих:
//   1. АКТИВНЫЙ отскок - окно анимации, решает LAi_Chr_CheckHitFire по запросу движка (isRecoilState);
//   2. ПАССИВНЫЙ шанс по навыку - LAi_CalcDodgeChanceRanged внутри расчёта урона.
// Раньше список был только у второго, а первый отменял выстрел ЛЮБЫМ боеприпасом.
#define BULLET_DODGE_DAMAGE_MULT 0.5 // [О] сколько урона доходит, если цель отскакивала (веер задел вскользь)
bool bShotDodgedButHit = false;      // LEO: цель отскакивала, но боеприпас неуклоняемый - срезать урон

float LAi_AmmoDamageMultiplier(string sBullet)
{
	if (sBullet == "grenade") return 3.0;
	return 1.0;
}

bool LAi_BulletIgnoresAvoidance(string sBullet)
{
	if (sBullet == "grapeshot") return true;    // картечь
	if (sBullet == "GunEchin") return true;     // ежи
	if (sBullet == "powder_pellet") return true;// граната склопетты
	if (sBullet == "grenade") return true;      // граната
	return false;
}

#define SCL_BURN_CHARACTER 10016 // sclopetta_burn::Message in the native particle wrapper

void LAi_RecordFirearmKill(aref attack, aref enemy)
{
	float ra = 1.0;
	float re = 1.0;
	float fExp;
	if (CheckAttribute(attack, "rank")) ra = stf(attack.rank);
	if (CheckAttribute(enemy, "rank")) re = stf(enemy.rank);
	if (re < 4.0) fExp = GetCharacterSPECIALSimple(attack, SPECIAL_P) + rand(GetCharacterSPECIALSimple(attack, SPECIAL_I));
	else fExp = Lai_GetCharacterMaxHP(enemy)/re + GetCharacterSPECIALSimple(attack, SPECIAL_P) * re / ra;
	AddCharacterExpToSkill(attack, SKILL_PISTOL, fExp);
	AddCharacterExpToSkill(attack, SKILL_DEFENCE, 1.0);
	AddCharacterExpToSkill(attack, SKILL_FORTUNE, 2.0);
	AddCharacterExpToSkill(attack, SKILL_LEADERSHIP, 1.0);
	Statistic_KillChar(attack, enemy, "_g");
	LAi_SetResultOfDeath(attack, enemy, true, true);
}

#event_handler("SclopettaBurnTick", "LAi_SclopettaBurnTick");
void LAi_SclopettaBurnTick()
{
	aref chr = GetEventData();
	int damage = GetEventData();
	int attackerIndex = GetEventData();
	if (!CheckAttribute(chr, "index") || LAi_IsDead(chr)) return;
	if (CheckAttribute(chr, "chr_ai.immortal") && sti(chr.chr_ai.immortal) != 0) return;
	bool armed = CheckAttribute(chr, "equip.blade");
	LAi_ApplyCharacterDamageEx(chr, damage, true);
	if (stf(chr.chr_ai.hp) >= 1.0) return;
	LAi_CheckLethalDamage(chr, attackerIndex, true, false);
	if (armed && attackerIndex >= 0 && LAi_IsDead(chr))
	{
		ref attack = GetCharacter(attackerIndex);
		LAi_RecordFirearmKill(attack, chr);
	}
}

void LAi_IgniteSclopettaTarget(aref chr, float closeness, int attackerIndex)
{
	object model = GetCharacterModel(chr);
	if (IsEntity(model)) SendMessage(&Particles, "liifl", SCL_BURN_CHARACTER, chr, model, closeness, attackerIndex);
}

void LAi_IgniteSclopettaBlast(aref attack, aref center, float radius)
{
	if (!CreateParticleEntity()) return;
	int attackerIndex = sti(attack.index);
	LAi_IgniteSclopettaTarget(center, 1.0, attackerIndex);
	for (int i = 0; i < iCollateralTargetsCount; i++)
	{
		int index = CollateralIdx(i);
		if (index < 0 || index == sti(center.index) || index == attackerIndex) continue;
		ref target = GetCharacter(index);
		if (LAi_IsDead(target)) continue;
		if (CheckAttribute(attack, "chr_ai.group") && CheckAttribute(target, "chr_ai.group"))
		{
			if (attack.chr_ai.group == target.chr_ai.group) continue;
		}
		if (CheckAttribute(target, "chr_ai.immortal") && sti(target.chr_ai.immortal) != 0) continue;
		float distance;
		if (GetCharacterDistByChr3D(target, center, &distance) && distance < radius)
			LAi_IgniteSclopettaTarget(target, 1.0 - distance / radius, attackerIndex);
	}
}

// Расчёт полного урона и опыта от применения огнестрельного оружия, ревью Assainisateur 01.2026
int LAi_ApplyCharacterFireDamage(aref attack, aref enemy, float kDist, bool bMainTarget, int iMultiShot, string sBullet, int hitKind)// возвращает:  промахи: 0 – истинный промах
//                                                                                                                                               1 – уклонение
//                                                                                                                                    попадания: 2 – рикошет
//                                                                                                                                               3 – истинное попадание
//                                                                                                                         особые статусы целей: 4 – дружественная цель
//                                                                                                                                               5 – бессмертная цель
{
	// френдли-фаер текущими правилами запрещён: урон не наносится, опыт не начисляется...
	if (CheckAttribute(attack, "chr_ai.group"))
	{
		if (CheckAttribute(enemy, "chr_ai.group"))
		{
			if (attack.chr_ai.group == enemy.chr_ai.group) return 4;// ... значит, делать здесь больше нечего, прерываем обработку скрипта
		}
	}
	
	// если цель бессмертна...
	if (CheckAttribute(enemy, "chr_ai.immortal"))
	{
		if (sti(enemy.chr_ai.immortal) != 0) return 5;// ... то делать здесь больше нечего, прерываем обработку скрипта
	}
	
	float fAttackerPistolSkill = LAi_GetCharacterGunLevel(attack);// навык "Пистолеты" атакующего
	bool bHeadshot = LAi_IsAimedHeadshot(hitKind, sBullet);
	
	// считаем вероятность попадания
	float fHitProbab = 1.0;
	if (hitKind < 2) fHitProbab = LAi_GunCalcProbability(attack, enemy, kDist, fAttackerPistolSkill);
	
	// обрабатываем промахи с учётом iMultiShot
	if (fHitProbab < 1.0)
	{
		int iShotsFired = iMultiShot;
		for (int i = 0; i < iShotsFired; i++)
		{
			if (rand(9999) >= fHitProbab * 10000.0) iMultiShot -= 1;
		}
	}
	
	// если промахнулись...
	if (iMultiShot < 1)
	{
		if (bMainTarget)// ... отображаем сообщения о промахах только по основной цели...
		{
			if (sti(attack.index) == GetMainCharacterIndex())// ... и только о промахах игрока...
			{
				string missed = "";
				if (LanguageGetLanguage() != "russian")
				{
					switch (rand(4))
					{
						case 0: missed = "Missed!"; break;
						case 1: missed = "Are you blind?!"; break;
						case 2: missed = "Aim better!"; break;
						case 3: missed = "Aim already!"; break;
						case 4: missed = "Don't shoot in the air!"; break;
					}
				}
				else
				{
					switch (rand(4))
					{
						case 0: missed = "Промах!"; break;
						case 1: missed = "Мазила!"; break;
						case 2: missed = "Целься лучше!"; break;
						case 3: missed = "Да прицелься уже!"; break;
						case 4: missed = "Не попал!"; break;
					}
				}
				setEffectText(attack, missed, argb(255,255,255,255), GetCharCapNotifScale());
			}
		}
		return 0;// ... и прерываем обработку скрипта
	}
	
	// некоторые боеприпасы имеют дополнительные свойства
	bool bPunchThrough = false;// флаг пробития брони
	bool bNoAvoidance = false;//  флаг игнорирования уклонения и рикошета
	bool bExplosive = false;//    флаг взрывчатки (урон по основной цели не зависит от дальности выстрела)
	bool bSclopettaPetard = sBullet == "powder_pellet" && attack.chr_ai.sgun == "grape_mushket";
	int iStunCheck = 2;//         значение по умолчанию ("2") отменяет последующую проверку на оглушение (MushketStun)
	bNoAvoidance = LAi_BulletIgnoresAvoidance(sBullet);// LEO: список один на оба уклонения, см. функцию
	switch (sBullet)//            отрабатываем аудио-визуальные эффекты взрывов гранат и раздаём флаги
	{
		case "GunEchin":
			iStunCheck = 1;
		break;
		case "harpoon":
			bPunchThrough = true;
			iStunCheck = -1;
		break;
		case "powder_pellet":
			LaunchBlastPellet(enemy, bMainTarget, !bSclopettaPetard);
			bExplosive = true;
			iStunCheck = 0;
		break;
		case "grenade":
			LaunchBlastGrenade(enemy, bMainTarget);
			bPunchThrough = true;
			bExplosive = true;
			iStunCheck = 1;
		break;
	}
	
	// если есть возможность уклониться...
	if (!bNoAvoidance && hitKind < 2)// (от bNoAvoidance боеприпасов уклониться нельзя)
	{
		float fDodgeChance = LAi_CalcDodgeChanceRanged(enemy) * 100.0;
		
		if (pnrand(enemy, fDodgeChance, "dodgeranged"))// ... и цель уклонилась...
		{
			setEffectText(enemy, XI_ConvertString("RangedEvade") + " (" + MakeInt(fDodgeChance + 0.5) + "%)", argb(255,255,255,255), GetCharCapNotifScale());
			return 1;// ... то делать здесь больше нечего, прерываем обработку скрипта
		}
	}
	
	bool bAttackerIsBuccaneer = CheckCharacterPerk(attack, "Buccaneer");// проверяем, является ли атакующий Буканьером
	
	float fArmorLevel = 0.0;
	if (!bHeadshot && CheckAttribute(enemy, "cirassId")) fArmorLevel = stf(Items[sti(enemy.cirassId)].CirassLevel);// проверяем, защищена ли цель кирасой
	
	// если есть возможность рикошета...
	if (fArmorLevel > 0.0)// (рикошет возможен только от кирас)
	{
		if (!bAttackerIsBuccaneer && !bNoAvoidance)// (выстрелы Буканьера и bNoAvoidance боеприпасы не рикошетят)
		{
			if (rand(999) < fArmorLevel * 500.0)// ... и рикошет произошёл...
			{
				setEffectText(enemy, XI_ConvertString("Ricochet"), argb(255,255,255,255), GetCharCapNotifScale());
				PlayStereoSound("Fx_Ricochet");
				LaunchCirassRicochet(attack, enemy);// LEO: искры по броне + дымный отскок пули в сторону
				return 2;// ... то делать здесь больше нечего, прерываем обработку скрипта
			}
		}
	}
	
	if (bAttackerIsBuccaneer)// классовый перк Буканьера...
	{
		if (CheckAttribute(enemy, "chr_ai.energy"))
		{
			float fEnergy = stf(enemy.chr_ai.energy) - stf(enemy.chr_ai.energymax) * 0.35;// ... отнимает 35% энергии у цели
			if (fEnergy < 0.0) fEnergy = 0.0;
			enemy.chr_ai.energy = fEnergy;
		}
	}
	
	// считаем базовый урон
	float fDamage = LAi_GunCalcDamage(attack, enemy, fArmorLevel, fAttackerPistolSkill, iMultiShot);
	if (fDamage <= 0.0) return 3;// если базовый урон меньше или равен нулю (что странно), то делать здесь больше нечего, прерываем обработку скрипта
	
	// Мушкетное оглушение - Gregg, ревью Assainisateur 01.2026
	if (iStunCheck > 1 && !bNoAvoidance)
	{
		if (HasSubStr(attack.equip.gun, "mushket")) iStunCheck = 1;
	}
	if (iStunCheck < 2)
	{
		if (rand(2) > iStunCheck)
		{
			setEffectText(enemy, XI_ConvertString("Stunned"), argb(255,255,255,255), GetCharCapNotifScale());
			MushketStun(enemy);
		}
	}
	
	// уменьшаем базовый урон на защиту цели
	if (fArmorLevel != 0.0)// получаем защиту от кирас
	{
		if (!bPunchThrough && !bNoAvoidance) bPunchThrough = bAttackerIsBuccaneer && rand(2) > 0;// выстрел Буканьера имеет 66% шанс проигнорировать защиту
		
		if (bPunchThrough)// если кираса пробита
		{
			setEffectText(enemy, XI_ConvertString("CirassBreak"), argb(255,255,255,255), GetCharCapNotifScale());
		}
		else//               если кираса не пробита...
		{
			fDamage = fDamage * (1.0 - fArmorLevel);// ... то уменьшаем урон в зависимости от уровня кирасы
			setEffectText(enemy, XI_ConvertString("CirassDef"), argb(255,255,255,255), GetCharCapNotifScale());
			LaunchCirassSparks(attack, enemy);// LEO: броня погасила пулю - высекаем искры
		}
	}
	
	bool bIsSetBlade = (CheckAttribute(enemy, "equip.blade"));// проверяем вооружена ли цель
	
	// считаем итоговый урон и опыт в умения
	if (!bExplosive || !bMainTarget)// гранаты наносят полный урон по основной цели независимо от дистанции
	{
		fDamage *= fHitProbab;// вероятность попадания является модификатором к нанесённому урону
	}
	// LEO: цель отскакивала, но боеприпас неуклоняемый - засчитали попадание, зато вскользь.
	// Флаг РАСХОДУЕМ: вторичные цели в зоне поражения к её отскоку непричастны.
	// Надпись - только за выстрелы ГГ, как и в остальных ветках уклонения (иначе стрельба NPC по NPC засыпает экран).
	if (bMainTarget && bShotDodgedButHit)
	{
		bShotDodgedButHit = false;
		fDamage *= BULLET_DODGE_DAMAGE_MULT;
		if (attack.index == GetMainCharacterIndex())
		{
			setEffectText(enemy, XI_ConvertString("RangedEvade"), argb(255,255,255,255), GetCharCapNotifScale());
		}
	}
	float fExp = 0.0;
	
	if (bIsSetBlade)
	{//                                                                опыт получаем только за вооружённые цели
		fExp = LAi_GunCalcExperience(attack, enemy, fDamage) * 0.65;// считаем опыт за чистый урон (без модификаторов и бонусов) с дисконтом 0.65 (??? почему уменьшаем?)
		if (fExp < 1.0) fExp = 1.0;
		AddCharacterExpToSkill(attack, SKILL_PISTOL, fExp);//          начисляем опыт
	}
	
	fDamage *= LAi_AmmoDamageMultiplier(sBullet);

	// применяем модификаторы и бонусы к урону
	int critchance = 0;// шанс крита
	float critd = 1.0;//  модификатор урона крита
	float pured = 0.0;//  бонус чистого урона
	
	if (bAttackerIsBuccaneer)// бонусы Буканьера
	{
		critd += 0.3;
		critchance += 10;
	}
	if(IsEquipCharacterByArtefact(attack, "talisman1"))// бонусы от Пернатого Змея
	{
		critd += 0.5;
		critchance += 20;
		pured += 25.0;
	}
	
	if (critchance > 0 && rand(99) < critchance)// бросок на крит
	{
		fDamage *= critd;// если повезло с критом
		setEffectText(enemy, GetConvertStrWithReplace("Variable_LAi_fightparams_53", "Logs.txt", "#space#", " "), argb(255,255,255,255), GetCharCapNotifScale());
	}
	else
	{
		fDamage += pured;// если не повезло с критом
	}
	
	if (bExplosive && bMainTarget)// определение вторичных целей при попадании гранатой в основную цель (перенос из LAi_events.c, временное решение)
	{
		float blastRadius = 5.0;
		iCollateralTargetsCount = FindNearCharacters(enemy, blastRadius, -1.0, -1.0, 0.001, true, false);
		CollateralSnapshot();
		if (bSclopettaPetard) LAi_IgniteSclopettaBlast(attack, enemy, blastRadius);
	}
	
	// Gregg: персональное сопротивление ОГНЕСТРЕЛЬНОМУ урону, доля 0.0..1.0. По умолчанию нет.
	// Отдельно от DmgResist (тот режет любой урон в LAi_ApplyCharacterDamage). Нужен потому, что
	// делитель урона 45/rank работает только по клинку (LAi_ApplyCharacterAttackDamage), а по
	// огнестрелу нет - без этого высокоранговых боссов выгоднее расстреливать с дистанции,
	// не входя в клинковый размен. Кламп "не меньше 1" ниже не даёт сделать цель неуязвимой.
	if (bHeadshot) fDamage *= 10.0;
	if (CheckAttribute(enemy, "GunResist")) fDamage = fDamage * (1.0 - stf(enemy.GunResist));
	if (fDamage < 1.0) fDamage = 1.0;
	// LEO: После смерти движок уже не отдаёт позицию обычным сообщением.
	float goreX, goreY, goreZ;
	bool hasGorePosition = false;
	if (bExplosive && sti(InterfaceStates.ChopBodies))
		hasGorePosition = GetCharacterPos(enemy, &goreX, &goreY, &goreZ);
	LAi_ApplyCharacterDamage(enemy, MakeInt(fDamage + 0.5));// наконец-то наносим цели итоговый урон
	
	LAi_CheckLethalDamage(enemy, sti(attack.index), true, bHeadshot);

	// LEO: расчленение - только от взрыва (граната/петарда) и только если цель им УБИТА.
	//   Движок режет меш по костям сам: скиннинг уже хранит, какая кость держит вершину.
	if (bExplosive)
	{
		if (LAi_IsDead(enemy))
		{
			if (sti(InterfaceStates.ChopBodies))
			{
				if (SendMessage(enemy, "lsff", MSG_CHARACTER_EX_MSG, "Chop", CHOP_BLAST_POWER, MOD_DEAD_CLEAR_TIME))
				{
					if (hasGorePosition) LAi_AddExplosionKillGroundBlood(enemy, goreX, goreY, goreZ);
				}
			}
		}
	}
	
	if (bIsSetBlade && LAi_IsDead(enemy)) LAi_RecordFirearmKill(attack, enemy);
	
	return 3;
}

//--------------------------------------------------------------------------------
//Параметры NPC
//--------------------------------------------------------------------------------

float npc_return_tmp;
bool npc_return_tmpb;

//Атаки
//Скорость нарастания вероятности атаки в секунду  p > 0, или средняя скорость ударов в секунду
//нужно выставлять соответсвенно скорости анимок ИИ, оставляя минимум 10-20% на защиты/пробивные
#event_handler("NPC_Event_GetAttackActive","LAi_NPC_GetAttackActive");
float LAi_NPC_GetAttackActive()
{
	aref chr = GetEventData();
	float level = LAi_GetCharacterFightLevel(chr);
	npc_return_tmp = 0.22 + MOD_SKILL_ENEMY_RATE*MOD_SKILL_ENEMY_RATE*0.04 - level*0.1;
	//скилл 10 - 0.25, 0.37, 0.57;
	//скилл 100 - 0.16, 0.28, 0.43;
	if(CheckAttribute(chr, "SaveItemsForDead"))
	{
		if(chr.SaveItemsForDead == true)
		{
			npc_return_tmp *= 1.18;
		}
	}
	else
	{
		if(random()*4 <= 1)
		{
			npc_return_tmp *= 1.15;
		}
	}
	return npc_return_tmp;
}

//Вес выбора удара "fast", 0 - никогда не выбирать
#event_handler("NPC_Event_GetAttackWeightFast","LAi_NPC_GetAttackWeightFast");
float LAi_NPC_GetAttackWeightFast()
{
	aref chr = GetEventData();
	npc_return_tmp = 1.0;
	//Boyer mod #20170318-33 Fight/difficulty level rebalancing
	npc_return_tmp = npc_return_tmp * 2.5;
	return npc_return_tmp;
}

//Вес выбора удара "force", 0 - никогда не выбирать
#event_handler("NPC_Event_GetAttackWeightForce", "LAi_NPC_GetAttackWeightForce");
float LAi_NPC_GetAttackWeightForce()
{
	aref chr = GetEventData();
	npc_return_tmp = 1.0;
	//Boyer mod #20170318-33 Fight/difficulty level rebalancing
	return npc_return_tmp;
}

//Вес выбора удара "round", 0 - никогда не выбирать, если врагов <3 то вес удара не накапливается
#event_handler("NPC_Event_GetAttackWeightRound", "LAi_NPC_GetAttackWeightRound");
float LAi_NPC_GetAttackWeightRound()
{
	aref chr = GetEventData();
	npc_return_tmp = 1.0;
	//Boyer mod #20170318-33 Fight/difficulty level rebalancing
	return npc_return_tmp;
}

//Вес выбора удара "break", 0 - никогда не выбирать. если цель не блокирует, то вес удара не накапливается
#event_handler("NPC_Event_GetAttackWeightBreak", "LAi_NPC_GetAttackWeightBreak");
float LAi_NPC_GetAttackWeightBreak()
{
	aref chr = GetEventData();
	npc_return_tmp = 1.0;
	npc_return_tmp = npc_return_tmp	* 0.3 * (0.6 + (0.1 * MOD_SKILL_ENEMY_RATE*MOD_SKILL_ENEMY_RATE));
	return npc_return_tmp;
}

//Вес выбора удара "feint", 0 - никогда не выбирать
#event_handler("NPC_Event_GetAttackWeightFeint", "LAi_NPC_GetAttackWeightFeint");
float LAi_NPC_GetAttackWeightFeint()
{
	aref chr = GetEventData();
	float level = LAi_GetCharacterFightLevel(chr);
	npc_return_tmp = 1.0; //boal fix
	npc_return_tmp = npc_return_tmp * ((0.14 + 0.06 * level * pow(MOD_SKILL_ENEMY_RATE, 1.5)) * MOD_SKILL_ENEMY_RATE);
	// множ навыка 1, 2.83, 5.2
	//cкилл 10 0.15, 0.31, 0.51
	//скилл 30 0.16, 0.38, 0.7
	//скилл 100 0.2, 0.62, 1.35
	return npc_return_tmp;
}

//Прараметры защиты
//Вероятность желания защитится - кубик с такой вероятностью кидается 2 раза в секунду, 1 = 100%
//больше 1 может выдать несколько защит по очереди
#event_handler("NPC_Event_GetDefenceActive", "LAi_NPC_GetAttackDefence");
float LAi_NPC_GetAttackDefence()
{
	aref chr = GetEventData();
	float level = LAi_GetCharacterFightLevel(chr);
	npc_return_tmp = 0.1 + 0.06 * (1.5 + level * MOD_SKILL_ENEMY_RATE) * MOD_SKILL_ENEMY_RATE;
	//матрос 0.19-0.25, кэп 0.3-0.5, ад 0.42-0.53-0.91

	return npc_return_tmp;

}

// boal 20.01.08 коммент - забавно, что спустя два года, понал как и что с вероятностями. Они все приводятся к 0-1 от веса общей суммы, то есть фактически умножение на сложность или цифры распределяют сумму по другим акшенам, а не усиливают этот
// Экшены идут парами - все атаки и защита (блок + пари)
//Вес выбора блока, 0 - никогда не выбирать
#event_handler("NPC_Event_GetDefenceWeightBlock", "LAi_NPC_GetDefenceWeightBlock");
float LAi_NPC_GetDefenceWeightBlock()
{
	aref chr = GetEventData();	
	float level = LAi_GetCharacterFightLevel(chr);
	npc_return_tmp = 0.4 + 0.5 * MOD_SKILL_ENEMY_RATE * (level - 0.2);// влияние exe - 0 навык увеличивает время блока в ~3 раза сравнительно с 100
	return npc_return_tmp;
}

//Вес выбора  паррирования, 0 - никогда не выбирать
//кубик с такой вероятностью кидается 2 раза в секунду
#event_handler("NPC_Event_GetDefenceWeightParry","LAi_NPC_GetDefenceWeightParry");
float LAi_NPC_GetDefenceWeightParry()
{
	aref chr = GetEventData();
	float level = LAi_GetCharacterFightLevel(chr);
	switch (MOD_SKILL_ENEMY_RATE) {
		case 1:
			npc_return_tmp = 0.5 - 0.15 * level;
		break;
		case 2:
			npc_return_tmp = 1.0 - 0.4 * level;			
		break;
		case 3:
			npc_return_tmp = 2.0 - 0.5 * level;			
		break;
	}
	return npc_return_tmp;
}

//Разрешён ли отскок
#event_handler("NPC_Event_EnableRecoil","LAi_NPC_EnableRecoil");
bool LAi_NPC_EnableRecoil()
{
	aref chr = GetEventData();
	npc_return_tmpb = MOD_SKILL_ENEMY_RATE - 1;// разрешаем отскоки начиная с капитана 
	return npc_return_tmpb;
}

#event_handler("NPC_Event_StunChance","LAi_NPC_StunChance");//Добавлен евент на стан после удара. Шансы расписаны ниже. Lipsar
float LAi_NPC_StunChance()
{
	aref chr = GetEventData();
	npc_return_tmp = 100;
	if (CheckAttribute(chr,"cirassid")) npc_return_tmp -= 35;
	if (IsCharacterPerkOn(chr, "SwordplayProfessional")) return npc_return_tmp -= 50;
	if (IsCharacterPerkOn(chr, "AdvancedDefence")) return npc_return_tmp -= 30;
	if (IsCharacterPerkOn(chr, "BasicDefence")) return npc_return_tmp -= 10;
	return npc_return_tmp;
}

// LEO: отдельный per-character ПРОПУСК флинч-реакции (дёрганья) от ЛЮБОГО удара. % шанса пропуска.
// По умолчанию 0 (поведение не меняется). Балансить тушки: ставить персонажу атрибут chr.HitStunSkip = N (0..100).
// Можно и глобально (вернуть общее значение). Отдельно от StunChance (тот только обычный удар/выстрел + перковая логика).
#event_handler("NPC_Event_HitStunSkip","LAi_NPC_HitStunSkip");
float LAi_NPC_HitStunSkip()
{
	aref chr = GetEventData();
	if (CheckAttribute(chr, "HitStunSkip")) return stf(chr.HitStunSkip);
	return 0.0; // LEO: функция float — возвращаем float-ноль (int 0 давал runtime «pointer function return integer value»)
}

//Параметры стрельбы

// LEO: Стрельба сквозь союзников.
#event_handler("NPC_Event_ShotOnlyEnemyTest", "LAi_NPC_ShotOnlyEnemyTest");
bool LAi_NPC_ShotOnlyEnemyTest()
{
	if (bShootOnlyEnemy) return LAi_grp_alarmactive;
	else return false;
}

//Вероятность желания выстрелить - кубик с такой вероятностью кидается 2 раза в секунду
#event_handler("NPC_Event_GetFireActive","LAi_NPC_GetFireActive");
float LAi_NPC_GetFireActive()
{
	aref chr = GetEventData();
	float level = LAi_GetCharacterGunLevel(chr);
	npc_return_tmp = 0.005 + level*0.01;
	// boal наши офицеры пулят из всех стволов -->
	if (chr.chr_ai.group == LAI_GROUP_PLAYER)
	{
      npc_return_tmp = 0.38 + npc_return_tmp;
	}
	else
	{
	// boal наши офицеры пулят из всех стволов <--
		if (CheckAttribute(chr, "SuperShooter"))
		{
			npc_return_tmp = npc_return_tmp + 0.4 * MOD_SKILL_ENEMY_RATE*MOD_SKILL_ENEMY_RATE / 9.0;
		}
		else
		{
			npc_return_tmp = npc_return_tmp + 0.05 * MOD_SKILL_ENEMY_RATE*MOD_SKILL_ENEMY_RATE / 9.0;
		}
	}
	return npc_return_tmp;
}

//Разрешён ли выстрел
#event_handler("NPC_Event_EnableFire","LAi_NPC_EnableFire");
bool LAi_NPC_EnableFire()
{
	aref chr = GetEventData();
	return   true;
}

//Разрещён ли временный перевыбор цели среди ближних - опрашивается постоянно
#event_handler("NPC_Event_AdaptiveTargetSelect","LAi_NPC_AdaptiveTargetSelect");
bool LAi_NPC_AdaptiveTargetSelect()
{
	aref chr = GetEventData();
	if(chr.chr_ai.type == LAI_TYPE_ACTOR)
	{
		npc_return_tmpb = false;
	}else{
		npc_return_tmpb = true;
	}
	return npc_return_tmpb;
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#event_handler("Location_CharacterSGFire","LAi_Location_CharacterSGFire");
// LEO: Обработка попадания картечи.
void LAi_Location_CharacterSGFire()
{
	aref attack = GetEventData();
	aref enemy = GetEventData();
	float kDmg = GetEventData();
	if(LAi_IsDead(enemy)) return;
	//Реакция груп на атаку
	LAi_group_Attack(attack, enemy);
	//Наносим повреждение
	LAi_ApplyCharacterDamage(enemy, MakeInt((5 + rand(5))*kDmg));
	//Проверим на смерть
	LAi_CheckKillCharacter(enemy);
}

#event_handler("ChrAttackAction", "LAi_ChrAttackAction");
bool LAi_ChrAttackAction()
{
	aref attack = GetEventData();
	string attackType = GetEventData();
	float fAttackSkill = LAi_GetCharacterFightLevel(attack);
	float curEnergy = Lai_CharacterGetEnergy(attack);
	float needEnergy = LAi_CalcUseEnergyForBlade(attack, attackType, fAttackSkill);
	if(curEnergy >= needEnergy)
	{
		npc_return_tmpb = true;
	}
	else
	{
		npc_return_tmpb = false;
	}
	return npc_return_tmpb;
}
//#20200510-03
#event_handler("Event_ChrFeint_Init", "LAi_BlockInitTime");
#event_handler("Event_ChrBlock_Init", "LAi_BlockInitTime");
#event_handler("Event_ChrParry_Init", "LAi_BlockInitTime");
void LAi_BlockInitTime()
{
	aref chr = GetEventData();
	float fSet = GetEventData();
	chr.chr_ai.BlockInitTime = fSet;
}

#event_handler("ChrFgtActApply", "LAi_ChrFightActionApply");
void LAi_ChrFightActionApply()
{
	aref attack = GetEventData();
	string attackType = GetEventData();
	float fAttackSkill = LAi_GetCharacterFightLevel(attack);
	float needEnergy = LAi_CalcUseEnergyForBlade(attack, attackType, fAttackSkill);
	Lai_CharacterChangeEnergy(attack, -needEnergy);
}

//Получить относительную затрачиваемую энергию
#event_handler("NPC_Event_GetActionEnergy","LAi_NPC_GetActionEnergy");
float LAi_NPC_GetActionEnergy()
{
	aref chr = GetEventData();
	string act = GetEventData();
	float fAttackSkill = LAi_GetCharacterFightLevel(chr);
	npc_return_tmp = LAi_CalcUseEnergyForBlade(chr, act, fAttackSkill) / LAi_GetCharacterMaxEnergy(chr);  // boal
	return npc_return_tmp;
}

//Необходимо вернуть максимально быстро нормализованое значение жизни
#event_handler("NpcEvtHP", "LAi_NPC_EvtGetHP");
float LAi_NPC_EvtGetHP()
{
	aref chr = GetEventData();
	npc_return_tmp = LAi_GetCharacterRelHP(chr);
	return npc_return_tmp;
}


//Необходимо вернуть максимально быстро нормализованое значение энергии
#event_handler("NpcEvtEny", "LAi_NPC_EvtGetEny");
float LAi_NPC_EvtGetEny()
{
	aref chr = GetEventData();
	npc_return_tmp = LAi_GetCharacterRelEnergy(chr);
	return npc_return_tmp;
}

// EvgAnat - требование энергии для отскока -->
bool bIsRecoilEnableWithoutEnergy = false; // можно ли выполнять отскоки при недостатке энергии (это не влияет на расход энергии)

#event_handler("ChrCheckEnergy", "LAi_Chr_CheckEnergy");
bool LAi_Chr_CheckEnergy()
{
	aref chr = GetEventData();
	string action = GetEventData(); // "recoil" - отскок назад, "strafe_l" и "strafe_r" - отскоки влево и вправо
	bool res = false;
	float needEnergy = 0.0;
	switch(action)
	{
		case "recoil":		needEnergy = 3.0;	break;
		case "strafe_l":	needEnergy = 4.0;	break;
		case "strafe_r":	needEnergy = 4.0;	break;
	}
	if (stf(chr.chr_ai.energy) >= needEnergy)
	{	
		res = true;
		Lai_CharacterChangeEnergy(chr, -needEnergy);
	}
	if (bIsRecoilEnableWithoutEnergy)
		return true;
	return res;
}
// EvgAnat - требование энергии для отскока <--

// EvgAnat - включено ли уклонение от выстрела для нпс -->
#event_handler("NPC_IsDodgeEnabled", "LAi_Chr_IsDodgeEnabled");
bool LAi_Chr_IsDodgeEnabled()
{
	return true;
}
// EvgAnat - включено ли уклонение от выстрела для нпс <--

// EvgAnat - уклонение от выстрела -->
#event_handler("Check_ChrHitFire", "LAi_Chr_CheckHitFire");
int LAi_Chr_CheckHitFire() // 0 - не попал, 1 - попал
{
	aref shooter = GetEventData(); // стрелок
	aref target = GetEventData(); // цель
	bool isRecoil = GetEventData(); // находится ли цель в окне уклонения
	float kDist = GetEventData(); // коэффициент дальности, равный 1-d/25; k(0)=1; k(10)=0.6; k(25)=0

	bShotDodgedButHit = false;
	if (!isRecoil) return 1;

	// LEO: от картечи и взрывчатки отскоком не спастись - веер и площадь накрывают всё равно.
	// Раньше отскок отменял выстрел ЛЮБЫМ боеприпасом: основная цель не получала попадания, и потому
	// одного отскочившего хватало, чтобы вся зона поражения осталась целой.
	if (LAi_BulletIgnoresAvoidance(LAi_GetCharacterBulletType(shooter)))
	{
		bShotDodgedButHit = true;// попадание засчитываем, но урон срежем при расчёте
		return 1;
	}

	if (shooter.index == GetMainCharacterIndex())
	{
		setEffectText(target, XI_ConvertString("RangedEvade"), argb(255,255,255,255), GetCharCapNotifScale());
	}
	return 0;
}
// EvgAnat - уклонение от выстрела <--

// EvgAnat - уклонение от атаки -->
#event_handler("Check_ChrHitAttack", "LAi_Chr_CheckHitAttack");
bool LAi_Chr_CheckHitAttack() // попала ли атака
{
	aref attack = GetEventData();
	aref enemy = GetEventData();
	bool isRecoil = GetEventData(); // находится ли цель в окне уклонения
	bool res = true;
	if (isRecoil)
		res = false;	
	return res;
}
// EvgAnat - уклонение от атаки <--

// EvgAnat - вероятность желания уклониться от выстрела у нпс -->
#event_handler("NPC_IsDodge", "LAi_NPC_IsDodge");
bool LAi_NPC_IsDodge() // true - уклоняется, false - не уклоняется
{
	aref chr = GetEventData();
	
	if (Random() <= 0.04) return true;// ревью Assainisateur 09.2025: шанс выставлен в 4%, как временное решение проблемы бесконечных отскоков от огнестрела. за подробностями обращаться к Bestreducer
	
	return false;
}
// EvgAnat - вероятность желания уклониться от выстрела у нпс <--

// EvgAnat - дальность отскока и стрейфа -->
#event_handler("GetCharacterRecoilDistance", "LAi_GetRecoilDistance");
float LAi_GetRecoilDistance()
{
	aref chr = GetEventData();
	string aType = GetEventData();
	float res = 2.0;
	switch(aType)
	{
		case "recoil":
			res = 1.0; // по умолчанию 2.0
		break;
		case "strafe":
			res = 10.0; // по умолчанию 15.0
		break;
	}
	return res;
}
// EvgAnat - дальность отскока и стрейфа <--
