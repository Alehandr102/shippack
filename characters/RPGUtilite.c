// 11.08.05 Boal Модуль для новой РПГ системы S.P.E.C.I.A.L
// метод для совместимости с .ИНИ файлом (секция SKILLCHANGER)
#include "scripts\Pusher.c"
#include "scripts\Utils2.c"
string checkskillfortrauma = "LeadershipFencingLightFencingHeavyPistolFortuneSneak"
int exp_skill_counter = 8;

// порог ранга
int GetCharacterRankRate(ref _refCharacter)
{
    return 35 - GetCharacterSPECIALSimple(_refCharacter, SPECIAL_I);
}

// LEO: потолок ранга. Ранг растёт от суммы 14 навыков (SetRankFromSkill), а цена ранга - это
//   35 минус Обучаемость, поэтому предел у каждого персонажа свой. Нужен залу славы: стендов
//   там ровно столько, сколько рангов этот капитан вообще способен взять.
int GetCharacterMaxRank(ref _refCharacter)
{
	int iRate = GetCharacterRankRate(_refCharacter);
	if( iRate < 1 ) iRate = 1;
	return 1 + (14 * SKILL_MAX - 84) / iRate;
}

int GetCharacterRankRateCur(ref _refCharacter)
{
    if(!CheckAttribute(_refCharacter, "rank_exp"))
	{
		_refCharacter.rank_exp = 0;
	}
    return sti(_refCharacter.rank_exp);
}
// прирост НР
int GetCharacterAddHPValue(ref _refCharacter)
{
    int E = sti(_refCharacter.SPECIAL.Endurance);
    int ret = makeint(2 + E * 0.55 + 0.5);
    return ret;
}

int GetCharacterBaseHPValue(ref _refCharacter)
{
	int ret = makeint(30.0 + GetCharacterSPECIALSimple(_refCharacter, SPECIAL_S)*GetCharacterSPECIALSimple(_refCharacter, SPECIAL_E)*0.5);
	if(_refCharacter == GetMainCharacter()) {ret = ret + 38;}
	return ret;
}

int GetCharacterNormalHp(ref _refCharacter)
{
	int ret = GetCharacterBaseHpValue(_refCharacter) + GetCharacterAddHpValue(_refCharacter) * (sti(_refCharacter.rank) - 1);
	if (CheckCharacterPerk(_refCharacter, "HPPlus"))
	{
		ret += 80;
	}
	if (CheckCharacterPerk(_refCharacter, "HPPlusFixed"))
	{
		ret += 60;
	}
	return ret;
}

float GetCharacterEffectiveHp(ref _refCharacter)
{
	float EffHp;
	if (CheckAttribute(_refCharacter, "Cirassid")) 
	{
		if(IsCharacterPerkOn(_refCharacter, "SwordplayProfessional"))
			EffHp = sti(_refCharacter.chr_ai.hp_max) / 0.7 / (1 - stf(Items[sti(_refCharacter.cirassId)].CirassLevel.break));
		else
		{
			if(IsCharacterPerkOn(_refCharacter, "AdvancedDefense"))
				EffHp = sti(_refCharacter.chr_ai.hp_max) / 0.8 / (1 - stf(Items[sti(_refCharacter.cirassId)].CirassLevel.break));
			else
			{
				if(IsCharacterPerkOn(_refCharacter, "BasicDefense"))
					EffHp = sti(_refCharacter.chr_ai.hp_max) / 0.9 / (1 - stf(Items[sti(_refCharacter.cirassId)].CirassLevel.break));
				else
					EffHp = sti(_refCharacter.chr_ai.hp_max) / (1 - stf(Items[sti(_refCharacter.cirassId)].CirassLevel.break));
			}
		}
	}
	else
	{
		if(IsCharacterPerkOn(_refCharacter, "SwordplayProfessional"))
			EffHp = sti(_refCharacter.chr_ai.hp_max) / 0.7 ;
		else
		{
			if(IsCharacterPerkOn(_refCharacter, "AdvancedDefense"))
				EffHp = sti(_refCharacter.chr_ai.hp_max) / 0.8;
			else
			{
				if(IsCharacterPerkOn(_refCharacter, "BasicDefense"))
					EffHp = sti(_refCharacter.chr_ai.hp_max) / 0.9;
				else
					EffHp = sti(_refCharacter.chr_ai.hp_max);
			}
		}
	}
	return EffHp;
}

void SetHealthToCharacter(ref _refCharacter)
{
	_refCharacter.chr_ai.hp_max = GetCharacterNormalHp(_refCharacter);
	// LEO: эта формула бонус перков на здоровье УЖЕ включает - помечаем его выданным, иначе
	//   SyncCharacterHPPerks добавит его вторым слоем.
	if (CheckCharacterPerk(_refCharacter, "HPPlus")) _refCharacter.PerkValue.HPPlus = 0;
	else DeleteAttribute(_refCharacter, "PerkValue.HPPlus");
	if (CheckCharacterPerk(_refCharacter, "HPPlusFixed")) _refCharacter.PerkValue.HPPlusFixed = 0;
	else DeleteAttribute(_refCharacter, "PerkValue.HPPlusFixed");
	if (!CheckAttribute(_refcharacter, "chr_ai.hp"))
	{
		_refcharacter.chr_ai.hp = GetCharacterNormalHp(_refCharacter);
	}
	else
	{
		if (sti(_refcharacter.chr_ai.hp) > sti(_refcharacter.chr_ai.hp_max))
		{
			_refcharacter.chr_ai.hp = _refcharacter.chr_ai.hp_max;
		}
	}
}

// LEO: приводит запас здоровья в соответствие с перками на него. Предел энергии считается формулой
//   (GetCharacterMaxEnergyValue), поэтому перки на неё работают везде сами; бонус на здоровье выдаётся
//   РАЗОВО, и его приходится синхронизировать явно при каждом изменении набора перков.
//   Маркер PerkValue.* делает выдачу однократной, а пропажу перка - обратимой: фантомам перки
//   перекатывают (DeleteAllPerksExceptChar чистит perks.list, маркер остаётся), иначе бонус копился бы.
//   Правка идёт и к пределу, и к текущему запасу: ни лечить, ни калечить функция не должна.
//   Звать после ЛЮБОЙ смены perks.list; на чистом персонаже это пара CheckAttribute.
void SyncCharacterHPPerks(ref _chref)
{
	float fCur;
	float fMax;
	if (LAi_IsDead(_chref)) return;		// заодно отсекает тех, кому здоровье ещё не выставили
	if (CheckCharacterPerk(_chref, "HPPlus"))
	{
		if (!CheckAttribute(_chref, "PerkValue.HPPlus"))
		{
			_chref.PerkValue.HPPlus = 0;
			LAi_SetHP(_chref, LAi_GetCharacterHP(_chref) + 80.0, LAi_GetCharacterMaxHP(_chref) + 80.0);
		}
	}
	else
	{
		if (CheckAttribute(_chref, "PerkValue.HPPlus"))
		{
			DeleteAttribute(_chref, "PerkValue.HPPlus");
			fMax = LAi_GetCharacterMaxHP(_chref) - 80.0;
			fCur = LAi_GetCharacterHP(_chref) - 80.0;
			if (fCur < 1.0) fCur = 1.0;
			LAi_SetHP(_chref, fCur, fMax);
		}
	}
	if (CheckCharacterPerk(_chref, "HPPlusFixed"))
	{
		if (!CheckAttribute(_chref, "PerkValue.HPPlusFixed"))
		{
			_chref.PerkValue.HPPlusFixed = 0;
			LAi_SetHP(_chref, LAi_GetCharacterHP(_chref) + 60.0, LAi_GetCharacterMaxHP(_chref) + 60.0);
		}
	}
	else
	{
		if (CheckAttribute(_chref, "PerkValue.HPPlusFixed"))
		{
			DeleteAttribute(_chref, "PerkValue.HPPlusFixed");
			fMax = LAi_GetCharacterMaxHP(_chref) - 60.0;
			fCur = LAi_GetCharacterHP(_chref) - 60.0;
			if (fCur < 1.0) fCur = 1.0;
			LAi_SetHP(_chref, fCur, fMax);
		}
	}
}

float GetCharacterMaxEnergyValue(ref _refCharacter)
{
    float ret = (30.0 + GetCharacterSPECIAL(_refCharacter, SPECIAL_A)*10);
    if (CheckCharacterPerk(_refCharacter, "EnergyPlus"))
	{
		ret = ret + 60;
	}
	if (CheckCharacterPerk(_refCharacter, "EnergyPlusFixed"))
	{
  		ret = ret + 45;
	}
	if (CheckCharacterPerk(_refCharacter, "Grunt"))
	{
  		ret = ret + 30;
	}
	if(CheckAttribute(_refCharacter, "bonusEnergy"))
	{
		ret = ret + stf(_refCharacter.bonusEnergy);
	}
	return ret;
}

float GetCharacterMaxEnergyABSValue(ref _refCharacter)
{
    // проверки перков to_do
    float ret = (30.0 + GetCharacterSPECIALSimple(_refCharacter, SPECIAL_A)*10);
	if (CheckCharacterPerk(_refCharacter, "EnergyPlus"))
	{
		ret = ret + 60;
	}
	if (CheckCharacterPerk(_refCharacter, "EnergyPlusFixed"))
	{
  		ret = ret + 45;
	}
	if (CheckCharacterPerk(_refCharacter, "Grunt"))
	{
  		ret = ret + 30;
	}
	return ret;
}

//--> добавляем и убираем бонусную энергию для перса
void AddBonusEnergyToCharacter(ref _refCharacter, int iEnrg)
{
	if(CheckAttribute(_refCharacter, "bonusEnergy"))
	{//Теперь бонусы суммируются а не переписывают друг друга
		_refCharacter.bonusEnergy = sti(_refCharacter.bonusEnergy) + iEnrg;
	}
	else
	{
		_refCharacter.bonusEnergy = iEnrg;
	}
	SetEnergyToCharacter(_refCharacter);
}

void RemoveBonusEnergyFromCharacter(ref _refCharacter, int howMuch)
{
	_refCharacter.bonusEnergy = sti(_refCharacter.bonusEnergy) - howMuch;
	if (sti(_refCharacter.bonusEnergy) <= 0)
	{
		DeleteAttribute(_refCharacter, "bonusEnergy");
	}
	SetEnergyToCharacter(_refCharacter);
}
//<-- добавляем и убираем бонусную энергию для перса

void SetEnergyToCharacter(ref _refCharacter)
{
	_refCharacter.chr_ai.energyMax = GetCharacterMaxEnergyValue(_refCharacter);
	if (!CheckAttribute(_refCharacter, "chr_ai.energy") || sti(_refCharacter.chr_ai.energy) > sti(_refCharacter.chr_ai.energyMax)) {
		_refCharacter.chr_ai.energy    = _refCharacter.chr_ai.energyMax;
	}
}

int GetCharacterMaxOfficersQty(ref _refCharacter)
{
	int skillN = sti(_refCharacter.Skill.Leadership);
    return GetCharacterSPECIAL(_refCharacter, SPECIAL_C)*2+makeint(skillN/20);
}

int AddCharacterExp(ref _refCharacter,int _exp)
{
	int retVal = false;
	// boal now use AddCharacterExpToSkill

	return retVal;
}

int CalculateExperienceFromRank(int _Rank)
{
	if(_Rank<2) return 0;
	int retVal = 100;//(MakeInt(1000 * pow(_Rank-1,2.3))/1000)*1000;
	return retVal;
}

int GetFreePoints_SelfRate(ref _chref)
{
	if (bHigherSelfRate) return 30 - GetCharacterSPECIALSimple(_chref, SPECIAL_I);
    else return 40 - GetCharacterSPECIALSimple(_chref, SPECIAL_I);
}

int GetFreePoints_ShipRate(ref _chref)
{
	if (bHigherShipRate) return 30 - GetCharacterSPECIALSimple(_chref, SPECIAL_I);
    else return 40 - GetCharacterSPECIALSimple(_chref, SPECIAL_I);
}
/////////////////////////////////////////////////////////////////////////

void SetSelfSkill(ref _refCharacter, int _fl, int _f, int _fh, int _p, int _fr)
{
    _refCharacter.Skill.FencingLight  = _fl;
    _refCharacter.Skill.Fencing       = _f;
    _refCharacter.Skill.FencingHeavy  = _fh;
    _refCharacter.Skill.Pistol        = _p;
    _refCharacter.Skill.Fortune       = _fr;
}

void SetShipSkill(ref _refCharacter, int _ld, int _cr, int _ac, int _cn, int _sl, int _re, int _gr, int _de, int _sn)
{
    _refCharacter.Skill.Leadership = _ld;
    _refCharacter.Skill.Commerce   = _cr;
    _refCharacter.Skill.Accuracy   = _ac;
    _refCharacter.Skill.Cannons    = _cn;
    _refCharacter.Skill.Sailing    = _sl;
    _refCharacter.Skill.Repair     = _re;
    _refCharacter.Skill.Grappling  = _gr;
    _refCharacter.Skill.Defence    = _de;
    _refCharacter.Skill.Sneak      = _sn;
}

void SetSPECIAL(ref _refCharacter, int _s, int _p, int _e, int _c, int _i, int _a, int _l)
{
    _refCharacter.SPECIAL.Strength   = _s;
    _refCharacter.SPECIAL.Perception = _p;
    _refCharacter.SPECIAL.Endurance  = _e;
    _refCharacter.SPECIAL.Charisma   = _c;
    _refCharacter.SPECIAL.Intellect  = _i;
    _refCharacter.SPECIAL.Agility    = _a;
    _refCharacter.SPECIAL.Luck       = _l;
}

void SetRandSPECIAL(ref _refCharacter)  // Для всех
{
    SetSPECIAL(_refCharacter,
               (4 + rand(6)),
               (2 + rand(8)),
               (4 + rand(6)),
               (2 + rand(8)),
               (4 + rand(6)),
               (4 + rand(6)),
               (2 + rand(8)));
	if (CheckAttribute(_refCharacter,"id") && HasSubStr(_refCharacter.id,"Fort Commander"))  _refCharacter.SPECIAL.Perception = 10;
}

 void SetRandSPECIAL_PGG(ref _refCharacter)  //WW для ПГГ
 {
    SetSPECIAL(_refCharacter,
               (6 + rand(4)),
               (4 + rand(6)),
               (6 + rand(4)),
               (4 + rand(6)),
               (6 + rand(4)),
               (7 + rand(3)),
               (4 + rand(6)));
}
void SetRandSPECIAL_F(ref _refCharacter)  // для абордажников
{
    SetSPECIAL(_refCharacter,
               (6 + rand(4)),
               (2 + rand(6)),
               (6 + rand(4)),
               (2 + rand(6)),
               (4 + rand(4)),
               (7 + rand(3)),
               (2 + rand(8)));
}

void SetRandSPECIAL_K(ref _refCharacter)  // для штурманов-казначеев
{
    SetSPECIAL(_refCharacter,
               (4 + rand(4)),
               (4 + rand(6)),
               (4 + rand(4)),
               (4 + rand(6)),
               (4 + rand(6)),
               (4 + rand(4)),
               (2 + rand(8)));
}

int ChecKSufficientRankForClass(int shipClass)
{
	switch (shipClass)
    {
		case 1 : return 33; break;
		case 2 : return 26; break;
		case 3 : return 20; break;
		case 4 : return 12; break;
		case 5 : return 6; break;
		case 6 : return 1; break;
		case 7 : return 1; break;
		else return 0;
    }
}

/// влияет только на СПЕЦИАЛ
int ApplayNavyPenalty(ref _refCharacter, string skillName, int sumSkill)
{
	if (CheckAttribute(_refCharacter,"bchangepirates")) {return sumSkill;} // фикс неправильного расчета начальных значений скиллов при старте игры от наличия минусов от навигации
    if (IsCompanion(_refCharacter) && GetRemovable(_refCharacter))//пусть будет для компаньонов тоже sti(_refCharacter.index) == GetMainCharacterIndex()) // только для главного, чтоб не тормозить всю игру
    {
        int sailSkill;
        // общее умение навигации
        sailSkill = GetSummonSkillFromNameSimple(_refCharacter, SKILL_SAILING);
        int shipClass = GetCharacterShipClass(_refCharacter);
        int needSkill = GetShipClassNavySkill(shipClass);

        if (sailSkill < needSkill)
        {
			sailSkill = makeint((needSkill - sailSkill)/10.0 + 0.9); // округление до мах всегда
			sumSkill = sumSkill - sailSkill;
	        if (sumSkill < 1) sumSkill = 1;
        }
    }
    return  sumSkill;
}
// пенальти в скилы
int ApplayNavyPenaltyToSkill(ref _refCharacter, string skillName, int sumSkill)
{
	if (CheckAttribute(_refCharacter,"bchangepirates")) {return sumSkill;}
    if (IsCompanion(_refCharacter) && GetRemovable(_refCharacter))//пусть будет для компаньонов тоже sti(_refCharacter.index) == GetMainCharacterIndex()) // только для главного, чтоб не тормозить всю игру
    {
        int sailSkill;
        // общее умение навигации
        sailSkill = GetSummonSkillFromNameSimple(_refCharacter, SKILL_SAILING);

        int shipClass = GetCharacterShipClass(_refCharacter);
        int needSkill = GetShipClassNavySkill(shipClass);
		if (bRankRequirement)
		{
			int needRank = (ChecKSufficientRankForClass(shipClass) - sti(_refCharacter.rank))*2;
			if (needRank < 0) needRank = 0;
			needSkill += needRank;
		}

        if (sailSkill < needSkill)
        {
			sailSkill = needSkill - sailSkill;
			sumSkill = sumSkill - sailSkill;
	        if (sumSkill < 1) sumSkill = 1;
        }
		if (CheckAttribute(_refCharacter,"chr_ai.Trauma") && HasSubStr(checkskillfortrauma,skillname)) sumSkill = sumSkill - makeint(sumSkill*0.2); //штраф от травмы - Gregg
		if (CheckAttribute(_refCharacter,"chr_ai.HeavyTrauma") && HasSubStr(checkskillfortrauma,skillname)) sumSkill = sumSkill - makeint(sumSkill*0.3); //штраф от тяжелой травмы - Gregg
    }
	else
	{
		if (CheckAttribute(_refCharacter,"chr_ai.Trauma")) sumSkill = sumSkill - makeint(sumSkill*0.4); //штраф от травмы - Gregg
	}
    return  sumSkill;
}
// с пенальти и вещами +1
int GetCharacterSPECIAL(ref _refCharacter, string skillName)
{
    int skillN;

	skillN = GetCharacterSPECIALSimple(_refCharacter, skillName);

 	skillN = ApplayNavyPenalty(_refCharacter, skillName, skillN);

    return skillN;
}

int GetCharacterSPECIALSimple(ref _refCharacter, string skillName)
{
	if (!CheckAttribute(_refCharacter,"SPECIAL."+skillName) )
    {
        return 3;
    }
	int skillN = sti(_refCharacter.SPECIAL.(skillName));
    // Health
    if (sti(_refCharacter.index) == GetMainCharacterIndex() && MOD_SKILL_ENEMY_RATE > 1) // не халява
    {
        skillN = skillN + GetHealthNum(_refCharacter) - 6; // max -5
    }

	if(skillName == SPECIAL_S)
	{

	}

	if(skillName == SPECIAL_P)
	{
		if (CheckCharacterItem(_refCharacter,"SandClock")) skillN += 1;
	}

	if(skillName == SPECIAL_C)
	{

	}

	if(skillName == SPECIAL_I)
	{

	}

	if(skillName == SPECIAL_L)
	{

	}

	if (skillN <= 1) skillN = 1;
	if( skillN > SPECIAL_MAX ) skillN = SPECIAL_MAX;

	return skillN;
}

void InitRPGType()
{
    NullCharacter.SPECIALType.Strength = true;
    NullCharacter.SPECIALType.Perception = true;
    NullCharacter.SPECIALType.Endurance = true;
    NullCharacter.SPECIALType.Charisma = true;
    NullCharacter.SPECIALType.Intellect = true;
    NullCharacter.SPECIALType.Agility = true;
    NullCharacter.SPECIALType.Luck = true;
    NullCharacter.SelfType.Leadership = true;
	NullCharacter.SelfType.FencingLight = true;
    NullCharacter.SelfType.Fencing = true;
    NullCharacter.SelfType.FencingHeavy = true;
    NullCharacter.SelfType.Pistol = true;
    NullCharacter.SelfType.Fortune = true;
    NullCharacter.SelfType.Sneak = true;
    // остальные корабельные
    NullCharacter.ShipType.Commerce = true;
    NullCharacter.ShipType.Accuracy = true;
    NullCharacter.ShipType.Cannons = true;
    NullCharacter.ShipType.Sailing = true;
    NullCharacter.ShipType.Repair = true;
    NullCharacter.ShipType.Grappling = true;
    NullCharacter.ShipType.Defence = true;
}

bool isSPECIALTypeSkill(string _param)
{
    return CheckAttribute(&NullCharacter, "SPECIALType." + _param);
}

bool isShipTypeSkill(string _param)
{
    return CheckAttribute(&NullCharacter, "ShipType." + _param);
}
bool isSelfTypeSkill(string _param)
{
    return CheckAttribute(&NullCharacter, "SelfType." + _param);
}

string GetRPGText(string _param)
{
    int    idLngFile = -1;
    string totalInfo;

    idLngFile = LanguageOpenFile("RPGDescribe.txt");
	if (MOD_SKILL_ENEMY_RATE == 3 && _param == "sailing") _param += "Hard10";
    totalInfo = LanguageConvertString(idLngFile, _param);
    LanguageCloseFile(idLngFile);

    return totalInfo;
}

string GetSkillNameByIdx(int idx)
{
    string ret = "";
    switch (idx)
    {
        case 1:    ret = SKILL_F_LIGHT;   break;
        case 2:    ret = SKILL_FENCING;   break;
        case 3:    ret = SKILL_F_HEAVY;   break;
        case 4:    ret = SKILL_PISTOL;    break;
        case 5:    ret = SKILL_FORTUNE;   break;
        case 6:    ret = SKILL_LEADERSHIP; break;
        case 7:    ret = SKILL_SNEAK;      break;
        case 8:    ret = SKILL_ACCURACY;   break;
        case 9:    ret = SKILL_CANNONS;    break;
        case 10:   ret = SKILL_SAILING;    break;
        case 11:   ret = SKILL_REPAIR;     break;
        case 12:   ret = SKILL_GRAPPLING;  break;
        case 13:   ret = SKILL_DEFENCE;    break;
        case 14:   ret = SKILL_COMMERCE;   break;
        case 15:   ret = SPECIAL_S;   break;
        case 16:   ret = SPECIAL_P;   break;
        case 17:   ret = SPECIAL_A;   break;
        case 18:   ret = SPECIAL_C;   break;
        case 19:   ret = SPECIAL_I;   break;
        case 20:   ret = SPECIAL_E;   break;
        case 21:   ret = SPECIAL_L;   break;
    }
    return ret;
}
//  _type   ShipType SelfType  SPECIALType
string GetSkillNameByTRIdx(string _type, int idx)
{
    string ret = "";

	switch (_type)
	{
		case "SelfType" :
		    switch (idx)
		    {
                case 1:    ret = SKILL_LEADERSHIP; break;
				case 2:    ret = SKILL_F_LIGHT;   break;
		        case 3:    ret = SKILL_FENCING;   break;
		        case 4:    ret = SKILL_F_HEAVY;   break;
		        case 5:    ret = SKILL_PISTOL;    break;
		        case 6:    ret = SKILL_FORTUNE;   break;
                case 7:    ret = SKILL_SNEAK;      break;
		    }
		break;
		case "ShipType" :
		    switch (idx)
		    {
                case 1:    ret = SKILL_SAILING; break;
				case 2:    ret = SKILL_ACCURACY;   break;
		        case 3:    ret = SKILL_CANNONS;   break;
		        case 4:    ret = SKILL_GRAPPLING;   break;
		        case 5:    ret = SKILL_DEFENCE;    break;
		        case 6:    ret = SKILL_REPAIR;   break;
                case 7:    ret = SKILL_COMMERCE;      break;
		    }
		break;
		case "SPECIALType" :
		    switch (idx)
		    {
                case 1:   ret = SPECIAL_S;   break;
		        case 2:   ret = SPECIAL_P;   break;
		        case 3:   ret = SPECIAL_A;   break;
		        case 4:   ret = SPECIAL_C;   break;
		        case 5:   ret = SPECIAL_I;   break;
		        case 6:   ret = SPECIAL_E;   break;
		        case 7:   ret = SPECIAL_L;   break;
		    }
		break;
    }
    return ret;
}
// по имени скила вернуть значение
int GetSkillValue(ref _refCharacter, string _type, string _skillName)
{
    if (CheckAttribute(_refCharacter, _type + "." + _skillName))
    {
        return sti(_refCharacter.(_type).(_skillName));
    }
    return 0;
}

float GetSkillValueExp(ref _refCharacter, string _skillName)
{
    string skillName_exp = _skillName + "_exp";
    if (!CheckAttribute(_refCharacter, "Skill."+skillName_exp))
	{
		_refCharacter.Skill.(skillName_exp) = 0;
	}
    return stf(_refCharacter.Skill.(skillName_exp));
}
int AddSPECIALValue(ref _refCharacter, string _skillName, int _add)
{
    string _type = SPECIAL_TYPE;
    if (CheckAttribute(_refCharacter, _type + "." + _skillName))
    {
        _refCharacter.(_type).(_skillName) = sti(_refCharacter.(_type).(_skillName)) + _add;
    }
    else
    {
        _refCharacter.(_type).(_skillName) = _add;
    }
    if (sti(_refCharacter.(_type).(_skillName)) < 1) {_refCharacter.(_type).(_skillName) = 1;}
    if (sti(_refCharacter.(_type).(_skillName)) > SPECIAL_MAX) {_refCharacter.(_type).(_skillName) = SPECIAL_MAX;}
    return sti(_refCharacter.(_type).(_skillName));
}

int AddCharacterSkill(ref _chref, string _skill, int _addValue)
{
    string  _skill_exp = _skill + "_exp";

    if (_addValue > 0)
	{
	    if ((sti(_chref.skill.(_skill)) + _addValue) > SKILL_MAX)
	    {
	        _addValue = SKILL_MAX - sti(_chref.skill.(_skill));
	    }
	}
    _chref.skill.(_skill) = sti(_chref.skill.(_skill)) + _addValue;

	if (sti(_chref.skill.(_skill)) < 1)
	{
        _chref.skill.(_skill) = 1;
	}
	if (sti(_chref.skill.(_skill)) > SKILL_MAX )
	{
	    _chref.skill.(_skill) = SKILL_MAX;
	}
	_chref.skill.(_skill_exp) = 0;
	if (_addValue >= 0)
	{
	   ApplayNewSkill(_chref, _skill, _addValue);
	}
	return sti(_chref.skill.(_skill));
}

void AddCharacterSkillDontClearExp(ref _chref, string _skill, int _addValue)
{
	_chref.skill.(_skill) = sti(_chref.skill.(_skill)) + _addValue;

	if (sti(_chref.skill.(_skill)) < 1)
	{
        _chref.skill.(_skill) = 1;
	}
	if (sti(_chref.skill.(_skill)) > SKILL_MAX )
	{
	    _chref.skill.(_skill) = SKILL_MAX;
	}
	if (_addValue >= 0)
	{
	   ApplayNewSkill(_chref, _skill, _addValue);
	}
}

void ApplayNewSkill(ref _chref, string _skill, int _addValue)
{
	// TODO: переместить в другое место
	if (_chref.id == pchar.id)
	{
		float P = makefloat(GetCharacterSPECIALSimple(_chref, SPECIAL_P)*10.0);
		float k = 0.0;
		if (checkattribute(pchar, "cheatGlobalMapView")) k = 2500.0;
		if (CheckAttribute(pchar, "equip.spyglass") && pchar.equip.spyglass != "")
		{
			k += stf(strcut(pchar.equip.spyglass,8,strlen(pchar.equip.spyglass)-1))*10.0;
		}
		//Enemy ship
		worldMap.enemyshipViewDistMin = 60.0+P+k;		//Растояние на котором корабль начинает исчезать
		worldMap.enemyshipViewDistMax = 120.0+P+k;		//Растояние на котором корабль исчезает полностью
		worldMap.enemyshipBrnDistMin = 80.0+P;		//Минимальное растояние на котором рожается корабль
		worldMap.enemyshipBrnDistMax = 130.0+P;		//Максимальное растояние на котором рожается корабль
		//storm
		worldMap.stormViewDistMin = 90.0+P+k;			//Растояние на котором шторм начинает исчезать
		worldMap.stormViewDistMax = 180.0+P+k;			//Растояние на котором шторм исчезает полностью
		worldMap.stormDistKill = 190.0+P;				//Расстояние на котором убиваем шторм
		worldMap.stormBrnDistMin = 100.0+P;			//Минимальное растояние на котором рожается шторм
		worldMap.stormBrnDistMax = 140.0+P;			//Максимальное растояние на котором рожается шторм
		WorldRadar_ApplyLive();						// LEO: пересчитать охват миникарты глобалки под новую дальность видимости (скилл/труба)

		int count = sti(pchar.questTemp.bloodingcount);
		CheckAndUnlock("AchBlood", count);
		
		count = sti(pchar.questTemp.swiftcount);
		CheckAndUnlock("AchStanS", count);
		
		count = sti(pchar.questTemp.stuncount);
		CheckAndUnlock("AchStanH", count);
		
		count = sti(pchar.questTemp.traumacount);
		CheckAndUnlock("AchTravma", count);
		
		count = sti(pchar.rank);
		CheckAndUnlock("rank", count);
	}

    // трем кэш
    CheckAttribute(_chref, "BakSkill." + _skill);

	// LEO: ⚠️ выдача бонуса НЕ ДОЛЖНА ЛЕЧИТЬ. Раньше здесь ставилось cur = max = новый предел, а сюда
	//   заходят в том числе из экрана персонажа (character.c: RefreshCharacterSkillExpRate) - осмотр
	//   раненого NPC восстанавливал ему здоровье (защитник 270/270 -> 355/355 без единого удара).
	//   Заодно к бонусу приплюсовывался GetCharacterAddHPValue - лишний ранг сверху ни за что.
	SyncCharacterHPPerks(_chref);

	// TODO: переместить в другое место
	if (CheckCharacterPerk(_chref, "AboardCollector")) {
		pchar.CSM.LootCollector.Enable = true;
	}

	// TODO: переместить в другое место
	if (CheckCharacterPerk(_chref, "AgileMan")) {
		if (_chref.model.animation == "man") _chref.model.animation = "man_fast";
		if (_chref.model.animation == "Jessika") _chref.model.animation = "Jessika_fast";
		if (_chref.model.animation == "YokoDias") _chref.model.animation = "YokoDias_fast";
		if (_chref.model.animation == "Milenace") _chref.model.animation = "Milenace_fast";
		if (_chref.model.animation == "Danielle") _chref.model.animation = "Danielle_fast";
		if (_chref.model.animation == "Giant") _chref.model.animation = "Giant_fast";
		if (_chref.model.animation == "Moscovit") _chref.model.animation = "Moscovit_fast";
		if (_chref.model.animation == "Chani") _chref.model.animation = "Chani_fast";
		if (_chref.model.animation == "skeleton") _chref.model.animation = "skeleton_fast";
	}

	// boal 05.05.04 разделение по группам -->
	int iFreePoints_exp = 0;
	if (isSelfTypeSkill(_skill)) {
		if(CheckAttribute(_chref,"perks.FreePoints_self_exp")) {
			iFreePoints_exp = sti(_chref.perks.FreePoints_self_exp);
		}
		iFreePoints_exp += _addValue;
		if (iFreePoints_exp >= GetFreePoints_SelfRate(_chref)) {
			iFreePoints_exp -= GetFreePoints_SelfRate(_chref);
			if(CheckAttribute(_chref,"perks.FreePoints_self")) {
				_chref.perks.FreePoints_self = sti(_chref.perks.FreePoints_self) + 1;
			}
			else {	
				_chref.perks.FreePoints_self = 1;
			}
			if (sti(_chref.index) == GetMainCharacterIndex()) {
				Log_SetStringToLog(XI_ConvertString("Personal abilities") + " + 1");
			}
		}
		_chref.perks.FreePoints_self_exp = iFreePoints_exp;
    }
    else {
		if(CheckAttribute(_chref,"perks.FreePoints_ship_exp")) {
			iFreePoints_exp = sti(_chref.perks.FreePoints_ship_exp);
		}
		iFreePoints_exp += _addValue;
		if (iFreePoints_exp >= GetFreePoints_ShipRate(_chref)) {
			iFreePoints_exp -= GetFreePoints_ShipRate(_chref);
			if(CheckAttribute(_chref,"perks.FreePoints_ship")) {
				_chref.perks.FreePoints_ship = sti(_chref.perks.FreePoints_ship) + 1;
			}
			else {
				_chref.perks.FreePoints_ship = 1;
			}
			if (sti(_chref.index) == GetMainCharacterIndex()) {
				Log_SetStringToLog(XI_ConvertString("Ship abilities") + " + 1");
			}
		}
		_chref.perks.FreePoints_ship_exp = iFreePoints_exp;
    }
    // boal 05.05.04 разделение по группам <--

    if(!CheckAttribute(_chref, "rank_exp")) {
      _chref.rank_exp = 0;
    }
    _chref.rank_exp = sti(_chref.rank_exp) + _addValue;

	// левелап
    if (sti(_chref.rank_exp) >= GetCharacterRankRate(_chref)) // use classic mode - 2 skill = 1 rank
    {
        if (sti(_chref.rank_exp) - GetCharacterRankRate(_chref) > 0) _chref.rank_exp = sti(_chref.rank_exp) - GetCharacterRankRate(_chref); //-Правка обнуления кача ранга-SeaBoss
        else _chref.rank_exp = 0; //-Правка обнуления кача ранга-SeaBoss
        _chref.rank = sti(_chref.rank) + 1;
        // LEO: снимок кадра под стенд зала славы - только у ГГ, снимок делается после отрисовки кадра
        if( IsMainCharacter(_chref) ) FinalGame_MakeRankShot(sti(_chref.rank));
        float mhp = LAi_GetCharacterMaxHP(_chref) + GetCharacterAddHPValue(_chref);
        LAi_SetHP(_chref, mhp, mhp);

        if (CheckCharacterPerk(_chref, "EnergyPlus") 
			|| CheckCharacterPerk(_chref, "EnergyPlusFixed") 
			|| CheckCharacterPerk(_chref, "Grunt") 
			|| CheckAttribute(_chref, "bonusEnergy")) 
		{
			SetEnergyToCharacter(_chref);
		}
        // сообщение в лог
        if(IsOfficer(_chref) || IsCompanion(_chref)) {
            AddMsgToCharacter(_chref,MSGICON_LEVELUP);
            if(_chref.id == "Blaze") {
            	// LEO: свой ранг капитану называем прямо - в потоке лога иначе непонятно, какой взят.
            	//   Анимация (вспышка + тряска) в движке идёт на ВСЮ строку, поэтому цвет отдаём только
            	//   числу: текст держим белым маркером "\6", а "\2" возвращает числу цвет строки -
            	//   зелёный, который и мигает.
            	string sLevelUp = "\6" + GetConvertStrWithReplace("Variable_RPGUtilite_1", "Logs.txt", "#space#", " ");
            	sLevelUp = sLevelUp + " " + GetConvertStrWithReplace("Variable_RPGUtilite_rank", "Logs.txt", "#space#", " ");
            	sLevelUp = sLevelUp + " \2" + _chref.rank;
            	Log_InfoColoredAnim("green", sLevelUp);
            	PlayStereoSound("interface\new_level.wav");
            }
            else {
                Log_Info("" + GetFullName(_chref) + GetConvertStrWithReplace("Variable_RPGUtilite_2", "Logs.txt", "#space#", " "));
            }
        }
    }
}

void ClearHPTubeEffect(string qName)
{
	float nphp = LAi_GetCharacterMaxHP(pchar);
	LAi_SetHP(pchar,nphp-sti(pchar.PerkValue.HPBONUS),nphp-sti(pchar.PerkValue.HPBONUS));
	DeleteAttribute(pchar,"chr_ai.bonushptube");
	DeleteAttribute(pchar,"PerkValue.HPBONUS");
	RestoreModelsBeforeDrugs();
}

void ClearENTubeEffect(string qName)
{
	DeleteAttribute(pchar,"chr_ai.bonusentube");
	RemoveBonusEnergyFromCharacter(pchar, sti(pchar.tubeBonusEnergy));
	RestoreModelsBeforeDrugs();
}

void ClearSTubeEffect(string qName)
{
	DeleteAttribute(pchar,"chr_ai.bonusweighttube");
	RestoreModelsBeforeDrugs();
}

void ClearDrugs()
{
	DeleteAttribute(pchar,"drugstaken");
	RestoreModelsBeforeDrugs();
}

void RestoreModelsBeforeDrugs()
{
	Log_Info(GetConvertStrWithReplace("Variable_RPGUtilite_3", "Logs.txt", "#space#", " "));
	DeleteAttribute(pchar, "HighOnDrugs")
	DeleteAttribute(pchar, "GenQuest.CamShuttle")
	for(int i = 0; i < MAX_CHARACTERS; i++)
	{
		ref chr;
		makeref(chr,Characters[i]);
		if(CheckAttribute(chr, "BeforeDrugsModel"))
		{
			chr.model = chr.BeforeDrugsModel;
			DeleteAttribute(chr, "BeforeDrugsModel");
			SetNewModelToChar(chr);
		}
	}
}
// по специал распередить скилы
void InitStartParam(ref _chref)
{
    int i;
    string  skillName;
    ClearCharacterExpRate(_chref);
    for (i=1; i<15; i++)
    {
        skillName = GetSkillNameByIdx(i);
        _chref.skill.(skillName) = makeint(MOD_EXP_RATE / GetCharacterExpRate(_chref, skillName));
    }
    LAi_SetHP(_chref, GetCharacterBaseHPValue(_chref), GetCharacterBaseHPValue(_chref));
	MAX_NUM_FIGHTERS=MOD_OFFICERS_RATE;
}
// порог следующего скила (множетель)
float GetCharacterExpRate(ref _chref, string _skill)
{
    string  skill_rate = _skill + "_rate";

    if (!CheckAttribute(_chref, "skill." + skill_rate))
    {
        float  divBy = 1.0;

        switch (_skill)
        {
            case SKILL_F_LIGHT:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_A)*0.9 + GetCharacterSPECIAL(_chref, SPECIAL_P)*0.1;
            break;
            case SKILL_FENCING:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_S)*0.6 + GetCharacterSPECIAL(_chref, SPECIAL_A)*0.4;
            break;
            case SKILL_F_HEAVY:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_S)*0.9 + GetCharacterSPECIAL(_chref, SPECIAL_E)*0.1;
            break;
            case SKILL_PISTOL:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_A)*0.5 + GetCharacterSPECIAL(_chref, SPECIAL_L)*0.5;
            break;
            case SKILL_FORTUNE:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_L);
            break;
            case SKILL_LEADERSHIP:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_I)*0.1 + GetCharacterSPECIAL(_chref, SPECIAL_C)*0.9;
            break;
            case SKILL_COMMERCE:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_I)*0.8 + GetCharacterSPECIAL(_chref, SPECIAL_C)*0.2;
            break;
            case SKILL_ACCURACY:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_P)*0.8 + GetCharacterSPECIAL(_chref, SPECIAL_L)*0.2;
            break;
            case SKILL_CANNONS:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_I)*0.6 + GetCharacterSPECIAL(_chref, SPECIAL_S)*0.4;
            break;
            case SKILL_SAILING:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_P)*0.2 + GetCharacterSPECIAL(_chref, SPECIAL_I)*0.8;
            break;
            case SKILL_REPAIR:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_P)*0.2 + GetCharacterSPECIAL(_chref, SPECIAL_E)*0.8;
            break;
            case SKILL_GRAPPLING:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_A)*0.7 + GetCharacterSPECIAL(_chref, SPECIAL_I)*0.3;
            break;
            case SKILL_DEFENCE:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_C)*0.5 + GetCharacterSPECIAL(_chref, SPECIAL_E)*0.5;
            break;
            case SKILL_SNEAK:
                divBy = GetCharacterSPECIAL(_chref, SPECIAL_P)*0.5 + GetCharacterSPECIAL(_chref, SPECIAL_L)*0.5;
            break;
        }
        _chref.skill.(skill_rate) = makefloat(MOD_EXP_RATE / (divBy * GetExpAcceleration(divBy)));
    }
    return  stf(_chref.skill.(skill_rate));
}

// table service
string GetReputationName(int reputation)
{
    if(reputation<10)
        return "REPUTATIONT_BAD_4";
    if(reputation<20)
        return "REPUTATIONT_BAD_3";
    if(reputation<30)
        return "REPUTATIONT_BAD_2";
    if(reputation<40)
        return "REPUTATIONT_BAD_1";
    if(reputation<=60)
        return "REPUTATIONT_NEUTRAL";//40-60 включительно
    if(reputation<=70)
        return "REPUTATIONT_GOOD_1";
    if(reputation<=80)
        return "REPUTATIONT_GOOD_2";
    if(reputation<=90)
        return "REPUTATIONT_GOOD_3";
	return "REPUTATIONT_GOOD_4";
}

// LEO: плохая репутация ГГ (< 40 = тиры BAD_1..BAD_4, см. GetReputationName) -> корабли/капитаны в море не ведут торговлю
bool SeaTrade_RepTooLow()
{
	if (sti(pchar.reputation) < 40) return true;
	return false;
}
// to_do  del
string GetFameName(int fame)
{
   return "TO_DO for del";
}

string GetMoraleName(int morale)
{
	if(morale<10)
		return MoraleTable[MORALET_TREACHEROUS];
	if(morale<20)
		return MoraleTable[MORALET_AWFUL];
	if(morale<30)
		return MoraleTable[MORALET_LOW];
	if(morale<40)
		return MoraleTable[MORALET_POOR];
	if(morale<50)
		return MoraleTable[MORALET_NORMAL];
	if(morale<60)
		return MoraleTable[MORALET_ELATED];
	if(morale<70)
		return MoraleTable[MORALET_GOOD];
	if(morale<80)
		return MoraleTable[MORALET_HIGH];
	if(morale<90)
		return MoraleTable[MORALET_EXCELLENT];
	if(morale<=100)
		return MoraleTable[MORALET_HEROIC];
	return "";
}

string GetLoyalityName(int iLoyality)
{
	if(iLoyality<10)
		return LoyalityTable[MORALET_TREACHEROUS];
	if(iLoyality<20)
		return LoyalityTable[MORALET_AWFUL];
	if(iLoyality<30)
		return LoyalityTable[MORALET_LOW];
	if(iLoyality<40)
		return LoyalityTable[MORALET_POOR];
	if(iLoyality<50)
		return LoyalityTable[MORALET_NORMAL];
	if(iLoyality<60)
		return LoyalityTable[MORALET_ELATED];
	if(iLoyality<70)
		return LoyalityTable[MORALET_GOOD];
	if(iLoyality<80)
		return LoyalityTable[MORALET_HIGH];
	if(iLoyality<90)
		return LoyalityTable[MORALET_EXCELLENT];
	if(iLoyality<=100)
		return LoyalityTable[MORALET_HEROIC];
	return "";
}

// boal 15/01/04 учет вещей выношу в спец метод для простоты работы далее -->
int SetCharacterSkillByItem(ref _refCharacter, string _skillName, string _itemSkillName, string _item, int _addValue)
{
	int iRetValue = 0;

	if(_skillName == _itemSkillName && GetCharacterItem(_refCharacter, _item) > 0)
	{
		iRetValue = _addValue;
	}

	return iRetValue;
}

int SetCharacterSkillByItemEquipped(ref _refCharacter, string _skillName, string _itemSkillName, string _item, int _addValue)//метод под экипируемые индианы/идолы/тотемы - Gregg
{
	return IsEquipCharacterByItem(_refCharacter, _item) * _addValue;
}

int SetCharacterSkillBySculArtefact(ref _refCharacter, string _skillName)
{
	bool bSkillSailing, bSkillCannons, bSkillGrappling, bSkillDefense, bSculMa1, bSculMa2, bSculMa3;
	bSkillSailing = (_skillName == SKILL_SAILING);
	bSkillCannons = (_skillName == SKILL_CANNONS);
	bSkillGrappling = (_skillName == SKILL_GRAPPLING);
	bSkillDefense = (_skillName == SKILL_DEFENCE);
	if (!bSkillSailing && !bSkillCannons && !bSkillGrappling && !bSkillDefense) {
		return 0;
	}
	bSculMa1 = (GetCharacterItem(_refCharacter, "sculMa1") > 0);
	bSculMa2 = (GetCharacterItem(_refCharacter, "sculMa2") > 0);
	bSculMa3 = (GetCharacterItem(_refCharacter, "sculMa3") > 0);

	if (bSculMa1 && bSculMa2 && bSculMa3) {// основной чек
		return 20;
	}
	else {
		if (bSkillGrappling && bSculMa1) {
			return 10;
		}
		if (bSkillCannons && bSculMa2) {
			return 10;
		}
		if (bSkillDefense && bSculMa3) {
			return 10;
		}
	}
    return 0;
}

int SetCharacterSkillByHaosArtefact(ref _refCharacter, string _skillName)
{	
    if (_skillName == SKILL_CANNONS || _skillName == SKILL_ACCURACY || _skillName == SKILL_F_HEAVY || _skillName == SKILL_FENCING || _skillName == SKILL_LEADERSHIP)
	{
       if (IsEquipCharacterByItem(_refCharacter, "PKM_SvtvA_amulet") && GetCharacterItem(_refCharacter, "PKM_SvtvA_pismo3")>0)
       {
           switch (_skillName)
           {
               case  SKILL_CANNONS:
                   return 15;
               break;

               case  SKILL_ACCURACY:
                   return 15;
               break;

               case  SKILL_F_HEAVY:
                   return 15;
               break;

               case  SKILL_FENCING:
                   return 15;
               break;
           }
       }
       else
       {
			if (IsEquipCharacterByItem(_refCharacter, "PKM_SvtvA_amulet"))
			{
				switch (_skillName)
				{
					case  SKILL_LEADERSHIP:
						return -20;
					break;
				}
				switch (_skillName)
				{
					case  SKILL_F_HEAVY:
						return 15;
					break;
				}
				switch (_skillName)
				{
					case  SKILL_FENCING:
						return 15;
					break;
				}
			}
		}
	}
    return 0;
}

int MaxSailing(ref _refCharacter, string _skillName)
{	
    if (_skillName == SKILL_SAILING)
	{
       if (CheckAttribute(pchar, "MaxSailing"))
       {
           switch (_skillName)
           {
               case  SKILL_SAILING:
                   return 100;
               break;
           }
       }
	}
    return 0;
}

// Warship 25.10.08 Учет одежды
int SetCharacterSkillBySuit(ref rChar, String sSkillName)
{
	int iValue = 0;
	String sSuit = GetCharacterEquipSuitID(rChar);

	if(sSuit == INVALID_SUIT) return 0;

	switch(sSuit)
	{
		case "suit_1": // Обноски
			if(sSkillName == SKILL_SNEAK)
			{
				iValue = 20;
			}
		break;

		case "suit_2": // Одежда торговца
			if(sSkillName == SKILL_COMMERCE)
			{
				iValue = 20;
			}
		break;

		case "suit_3": // Платье дворянина
			if(sSkillName == SKILL_LEADERSHIP)
			{
				iValue = 20;
			}
		break;
	}

	return iValue;
}
// boal 15/01/04 учет вещей выношу в спец метод для простоты работы далее <--
float GetCharacterSkillToOld(ref _refCharacter, string skillName)
{
    return makefloat(GetCharacterSkill(_refCharacter, skillName) / SKILL_TO_OLD);
}
int GetCharacterSkill(ref _refCharacter, string skillName)
{
    int skillN;

    // boal 051104 метод тормозит страшно - проверка +1 вещей, будет опрос раз в 10
    if (CheckAttribute(_refCharacter, "BakSkill." + skillName))
    {
        if (sti(_refCharacter.BakSkillCount.(skillName)) < 20 )
        {
            _refCharacter.BakSkillCount.(skillName) = sti(_refCharacter.BakSkillCount.(skillName)) + 1;
            return sti(_refCharacter.BakSkill.(skillName));
        }
    }
	skillN = GetCharacterSkillSimple(_refCharacter, skillName);
    // 14.03.05 пенальти от класса корабля -->
 	skillN = ApplayNavyPenaltyToSkill(_refCharacter, skillName, skillN);
	// 14.03.05 пенальти от класса корабля <--
	_refCharacter.BakSkill.(skillName)      =  skillN; // значение
    _refCharacter.BakSkillCount.(skillName) = rand(5); // счетчик немного размажем пересчет

    return skillN;
}

int GetCharacterSkillNoBonus(ref _refCharacter, string skillName)
{
	return sti(_refCharacter.Skill.(skillName));
}

float GetCharacterSkillNoBonusToOld(ref _refCharacter, string skillName)
{
	return GetCharacterSkillNoBonus(_refCharacter,skillName)/SKILL_TO_OLD;
}

int GetCharacterSkillSimple(ref _refCharacter, string skillName)
{
	if( !CheckAttribute(_refCharacter,"Skill."+skillName) ) {
		return 1;
	}

	int skillN = sti(_refCharacter.Skill.(skillName));

	if (CheckAttribute(_refCharacter,"HPminusDaysNeedtoRestore")) {
		skillN -= 50; //штраф от контузии - Gregg
	}
	bool   bHero = (sti(_refCharacter.index) == GetMainCharacterIndex());

	if (bHero) {
		if (CheckAttribute(pchar,"drugstaken") && pchar.drugstaken >= 3) {
			skillN -= 50; //штраф от перекура
		}
		if (skillName == "Leadership") {
			if (CheckAttribute(pchar,"LeadershipLose"))	{
				skillN -= 200;
			}
		}
		// Health
		if (MOD_SKILL_ENEMY_RATE > 1 && isSelfTypeSkill(skillName)) {// не халява
			skillN = skillN + 5*(GetHealthNum(_refCharacter) - 6); // max -5
		}
	}
    // boal учет вещей -->
    // LEO: гейт был по Payment - а это переключатель ОПЛАТЫ офицера, а не признак «свой». Примкнувшим
    //   бесплатно (стартовая пара альтернативного пролога, квестовые спутники) бонусы предметов -
    //   вплоть до черепов и костюмов - не считались вообще.
    //   Порядок проверок - это перф: перебор списков строит ~22 строки на вызов, а в абордаже код зовут
    //   десятки тел каждый кадр (микролаги 2.4.8.8). Сначала дешёвые Payment и location.fantom (её ставит
    //   только фабрика массовки LAi_CreateFantomCharacterEx - у офицера её не бывает).
    bool bOurs = bHero;
    if (!bOurs) bOurs = CheckAttribute(_refCharacter, "Payment");
    if (!bOurs)
    {
        if (!CheckAttribute(_refCharacter, "location.fantom")) // массовка своей не бывает
        {
            bOurs = IsPostOfficer(_refCharacter);
            if (!bOurs) bOurs = IsOfficer(_refCharacter);
            if (!bOurs) bOurs = IsCompanion(_refCharacter);
        }
    }
    if (!bOurs)
	{
		if (skillN <= 1) {
			skillN = 1;
		}
		if (skillN > SKILL_MAX) {
			skillN = SKILL_MAX;
		}
		return skillN;
	}
	
	// бонусы в скиллы от предметов, ревью Assainisateur 06.2026
	string sItemID = "";
	switch (skillName)
	{
		case SKILL_LEADERSHIP:
			// слот BLADE_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, BLADE_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "topor_emperor") skillN += 20;// {Императорский топор} (+20 к авторитету, -30 к скрытности при ношении)
				sItemID = "";
			}
			
			// слот JEWELRY_INDIAN_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "jewelry4") skillN += 10;// {Изумруд} (+10 авторитет при ношении)
				sItemID = "";
			}
			
			// слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "indian1": skillN += 10; break;//      {Оберег Тлальчитонатиу} (+10 авторитет и скрытность, -20 пистолеты при ношении)
					case "indian20": skillN += 20; break;//     {Церемониальный нож} (+20 авторитет и -10 везение при ношении)
					case "indian21": skillN += 20; break;//     {Церемониальный сосуд} (+20 авторитет, -10 скрытность при ношении)
					case "DeSouzaCross": skillN += 20; break;// {Крест Антонио де Соуза} (+30 к везению, +20 к авторитету, +10 к торговле при ношении)
					case "DHGlove": skillN += 15; break;//      {Темпоральный деформатор} (-15 к скрытности, +15 к авторитету при ношении)
					case "indian15": skillN += 10; break;//     {Базальтовая голова} (+10 авторитет и защита, -10 везение при ношении)
					case "indian22": skillN -= 10; break;//     {Голова воина племени майя} (+20 защита, +10 лёгкое оружие, -10 авторитет и везение при ношении)
					case "indian14": skillN -= 20; break;//     {Чаша Ололиуки} (+20 торговля, -20 авторитет при ношении)
					case "PKM_SvtvA_amulet":
						if (GetCharacterItem(_refCharacter, "PKM_SvtvA_pismo3") < 1) skillN -= 20;// {Амулет Хаоса, без Книги Хаоса} (+15 тяжёлое оружие, +15 среднее оружие, -20 авторитет при ношении)
					break;
				}
				sItemID = "";
			}
			
			//слот IDOLS_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "Totem_6": skillN += 5; break;//    {Тотем Чалчиуитликуэ} (+5 авторитет при ношении)
					case "hatWhisper": skillN += 5; break;// {Шляпа капитана "Чёрной Вдовы"} (+5 к навигации, +5 к авторитету, + 5 к абордажу, -5 к скрытности при ношении)
				}
				sItemID = "";
			}
			
			//работают без экипировки
			if ((GetCharacterItem(_refCharacter, "mineral4") > 0)) skillN -= 10;//   {Баклан} (-10 авторитет и -10 скрытность)
			if ((GetCharacterItem(_refCharacter, "indian2") > 0)) skillN -= 10;//    {Пугающая фигурка} (-10 авторитет при ношении)
			if ((GetCharacterItem(_refCharacter, "SkullAztec") > 0)) skillN += 10;// {Нефритовый череп} (-20 везение, +10 авторитет)
			// Gregg: {Три древние монеты} (+10 везение, авторитет и торговля при ношении)
			if ((GetCharacterItem(_refCharacter, "DisappearPeople_Coins") > 0)) skillN += 10;
			// Gregg: {Лапа капитана Маугли} (+15 везение и защита, -10 авторитет при ношении)
			if ((GetCharacterItem(_refCharacter, "MaugliPaw") > 0)) skillN -= 10;
		break;
		case SKILL_F_LIGHT:
			// слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "indian22") skillN += 10;// {Голова воина племени майя} (+20 защита, +10 лёгкое оружие, -10 авторитет и везение при ношении)
				sItemID = "";
			}
			
			// слот IDOLS_LEFT_ITEM_TYPE::
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "Totem_8") skillN += 5;// {Тотем Тлалока} (+5 лёгкое оружие при ношении)
				sItemID = "";
			}
			
			//работают без экипировки
			if ((GetCharacterItem(_refCharacter, "KnifeAztec") > 0)) skillN -= 20;// {Обсидиановый церемониальный нож} (-30 пистолеты, -20 во все остальные типы оружия)
		break;
		case SKILL_FENCING:
			// слот BLADE_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, BLADE_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "blade013") skillN += 10;// {Ирис} (+10 среднее оружие,-30 везение)
				sItemID = "";
			}
			
			// слот JEWELRY_INDIAN_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "indian6") skillN += 10;// {Амулет Шиукоатля} (+10 среднее оружие при ношении)
				sItemID = "";
			}
			
			// слот IDOLS_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "Totem_7") skillN += 5;// {Тотем Уицилопочтли} (+5 среднее оружие при ношении)
				sItemID = "";
			}
			
			// слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "PKM_SvtvA_amulet") skillN += 15;// {Амулет Хаоса} (+15 тяжёлое оружие, +15 среднее оружие, -20 авторитет при ношении)
				sItemID = "";
			}
			
			//работают без экипировки
			if ((GetCharacterItem(_refCharacter, "KnifeAztec") > 0)) skillN -= 20;// {Обсидиановый церемониальный нож} (-30 пистолеты, -20 во все остальные типы оружия)
		break;
		case SKILL_F_HEAVY:
			// слот JEWELRY_INDIAN_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "indian12") skillN += 10;// {Кубок-тотем Тепейоллотля} (+10 тяжёлое оружие при ношении)
				sItemID = "";
			}
			
			// слот IDOLS_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "Totem_9") skillN += 5;// {Тотем Майяуэль} (+5 тяжёлое оружие при ношении)
				sItemID = "";
			}
			
			// слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "PKM_SvtvA_amulet") skillN += 15;// {Амулет Хаоса} (+15 тяжёлое оружие, +15 среднее оружие, -20 авторитет при ношении)
				sItemID = "";
			}
			
			//работают без экипировки
			if ((GetCharacterItem(_refCharacter, "KnifeAztec") > 0)) skillN -= 20;// {Обсидиановый церемониальный нож} (-30 пистолеты, -20 во все остальные типы оружия)
		break;
		case SKILL_PISTOL:
			//слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "indian1": skillN -= 20; break;//  {Оберег Тлальчитонатиу} (+10 авторитет и скрытность, -20 пистолеты при ношении)
					case "indian10": skillN += 20; break;// {Оберег Эхекатля} (+20 пистолеты, +10 меткость, -20 скрытность при ношении)
				}
				sItemID = "";
			}
			
			// слот IDOLS_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "Totem_5") skillN += 5;// {Тотем Тескатлипока} (+5 пистолеты при ношении)
				sItemID = "";
			}
			
			// слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "glasses") skillN -= 5;// {Очки} (+15 к скрытности, -5 к пистолетам, - 5 к орудиям, -5 к меткости при ношении)
				sItemID = "";
			}
			
			//работают без экипировки
			if ((GetCharacterItem(_refCharacter, "KnifeAztec") > 0)) skillN -= 30;// {Обсидиановый церемониальный нож} (-30 пистолеты, -20 во все остальные типы оружия)
		break;
		case SKILL_FORTUNE:
			// слот BLADE_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, BLADE_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "blade013") skillN -= 30;// {Ирис} (+10 среднее оружие,-30 везение)
				sItemID = "";
			}
			
			//слот JEWELRY_INDIAN_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "jewelry8": skillN += 10; break;//        {Бронзовое кольцо} (+10 к везению при ношении)
					case "Dozor_HorseShoe": skillN += 15; break;// {Счастливая подкова} (+15 к везению при ношении)
				}
				sItemID = "";
			}
			
			//слот JEWELRY_INDIAN_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "indian7": skillN += 10; break;//  {Идол Великой Матери} (+10 к везению при ношении)
					case "jewelry9": skillN += 10; break;// {Бронзовый крестик} (+10 к везению при ношении)
				}
				sItemID = "";
			}
			
			//слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "DeSouzaCross": skillN += 30; break;// {Крест Антонио де Соуза} (+30 к везению, +20 к авторитету, +10 к торговле при ношении)
					case "indian15": skillN -= 10; break;//     {Базальтовая голова} (+10 авторитет и защита, -10 везение при ношении)
					case "indian20": skillN -= 10; break;//     {Церемониальный нож} (+20 авторитет и -10 везение при ношении)
					case "indian22": skillN -= 10; break;//     {Голова воина племени майя} (+20 защита, +10 лёгкое оружие, -10 авторитет и везение при ношении)
				}
				sItemID = "";
			}
			
			// слот TALISMAN_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, TALISMAN_ITEM_TYPE);
			if (sItemID != "")
			{// Gregg: в данной категории предмет пока один
				if (sItemID == "MoonStone") skillN += 10;// {Лунный камень призрачного света} (+10 везение при ношении)
				sItemID = "";
			}
			
			// слот IDOLS_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "Totem_1") skillN += 5;// {Тотем Шочикецаль} (+5 везение при ношении)
				sItemID = "";
			}
			
			// слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "PDM_PJ_BsRL") skillN += 5;// {Бутылка с розовой ленточкой} (+5 везение, +5 торговля, -5 скрытность при ношении)
				sItemID = "";
			}
			
			//работают без экипировки
			if ((GetCharacterItem(_refCharacter, "Cursed_idol") > 0)) skillN -= 40;// {Идол Хурукацелитипочтли} (-40 везение)
			if ((GetCharacterItem(_refCharacter, "Coins") > 0)) skillN -= 50;//       {Проклятые жемчужины} (-50 везение)
			if ((GetCharacterItem(_refCharacter, "Mineral8") > 0)) skillN -= 10;//    {Башмак} (-10 везение)
			if ((GetCharacterItem(_refCharacter, "SkullAztec") > 0)) skillN -= 20; // {Нефритовый череп} (-20 везение, +10 авторитет)
			// Gregg: {Три древние монеты} (+10 везение, авторитет и торговля при ношении)
			if ((GetCharacterItem(_refCharacter, "DisappearPeople_Coins") > 0)) skillN += 10;
			// Gregg: {Лапа капитана Маугли} (+15 везение и защита, -10 авторитет при ношении)
			if ((GetCharacterItem(_refCharacter, "MaugliPaw") > 0)) skillN += 15;
		break;
		case SKILL_SNEAK:
			// слот BLADE_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, BLADE_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "topor_emperor") skillN -= 30;// {Императорский топор} (+20 к авторитету, -30 к скрытности при ношении)
				sItemID = "";
			}
			
			//слот JEWELRY_INDIAN_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "indian5": skillN += 10; break;//   {Двойная маска} (+10 скрытность при ношении)
					case "jewelry15": skillN += 10; break;// {Изумрудные подвески} (+10 к скрытности при ношении)
				}
				sItemID = "";
			}
			
			// слот JEWELRY_INDIAN_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "indian17") skillN += 10;// {Тельная ладанка} (+10 к скрытности при ношении)
				sItemID = "";
			}
			
			//слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "DHGlove": skillN -= 15; break;//  {Темпоральный деформатор} (-15 к скрытности, +15 к авторитету при ношении)
					case "indian1": skillN += 10; break;//  {Оберег Тлальчитонатиу} (+10 авторитет и скрытность, -20 пистолеты при ношении)
					case "indian10": skillN -= 20; break;// {Оберег Эхекатля} (+20 пистолеты, +10 меткость, -20 скрытность при ношении)
					case "indian18": skillN -= 10; break;// {Идол Атлауа} (+10 навигация, -10 скрытность при ношении)
					case "indian19": skillN -= 20; break;// {Статуэтка Тлалока} (+20 меткость, +10 орудия, -20 скрытность при ношении)
					case "indian21": skillN -= 10; break;// {Церемониальный сосуд} (+20 авторитет, -10 скрытность при ношении)
				}
				sItemID = "";
			}
			
			//слот IDOLS_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "Totem_2": skillN += 5; break;//       {Тотем Миктлантекутли} (+5 скрытность при ношении)
					case "Dozor_Mirror": skillN += 15; break;// {Карманное зеркало} (+15 к скрытности при ношении)
					case "hatWhisper": skillN -= 5; break;//    {Шляпа капитана "Чёрной Вдовы"} (+5 к навигации, +5 к авторитету, + 5 к абордажу, -5 к скрытности при ношении)
				}
				sItemID = "";
			}
			
			//слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "glasses": skillN += 15; break;//    {Очки} (+15 к скрытности, -5 к пистолетам, - 5 к орудиям, -5 к меткости при ношении)
					case "PDM_PJ_BsRL": skillN -= 5; break;// {Бутылка с розовой ленточкой} (+5 везение, +5 торговля, -5 скрытность при ношении)
				}
				sItemID = "";
			}
			
			//работают без экипировки
			if ((GetCharacterItem(_refCharacter, "mineral4") > 0)) skillN -= 10; // {Баклан} (-10 авторитет и -10 скрытность)
		break;
		case SKILL_SAILING:
			// слот BLADE_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, BLADE_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "blackbeard_sword") skillN += 10;// {Меч Тритона} (+10 к навигации, +20 к абордажу, +10% к скорости обращения с парусами)
				sItemID = "";
			}
			
			// слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "indian18") skillN += 10;// {Идол Атлауа} (+10 навигация, -10 скрытность при ношении)
				sItemID = "";
			}
			
			//слот IDOLS_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "hatWhisper": skillN += 5; break;//  {Шляпа капитана "Чёрной Вдовы"} (+5 к навигации, +5 к авторитету, + 5 к абордажу, -5 к скрытности при ношении)
					case "Dozor_Storm": skillN += 5; break;// {Санта-Мария} (+5 к навигации при ношении)
				}
				sItemID = "";
			}
			
			// слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "Totem_12") skillN += 5;// {Тотем Синтеотля} (+5 к навигации при ношении)
				sItemID = "";
			}
			
			// што блять?!
			if (CheckAttribute(pchar, "MaxSailing")) {
				skillN = SKILL_MAX;
			}
		break;
		case SKILL_ACCURACY:
			//слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "Totem_4": skillN += 5; break;// {Тотем Мишкоатля} (+5 меткость при ношении)
					case "glasses": skillN -= 5; break;// {Очки} (+15 к скрытности, -5 к пистолетам, - 5 к орудиям, -5 к меткости при ношении)
				}
				sItemID = "";
			}
			
			//слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "indian10": skillN += 10; break;// {Оберег Эхекатля} (+20 пистолеты, +10 меткость, -20 скрытность при ношении)
					case "indian19": skillN += 20; break;// {Статуэтка Тлалока} (+20 меткость, +10 орудия, -20 скрытность при ношении)
					case "PKM_SvtvA_amulet":
						if (GetCharacterItem(_refCharacter, "PKM_SvtvA_pismo3") > 0) skillN += 15;// Амулет Хаоса + Книга Хаоса
					break;
				}
				sItemID = "";
			}
			
			// Талисман Ягуар
			if (!IsDay())// бонус только по ночам
			{
				if (IsCharacterEquippedArtefact(_refCharacter, "talisman15")) skillN = skillN * 2;// x2 по ночам
			}
		break;
		case SKILL_CANNONS:
			//слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "indian19": skillN += 10; break;// {Статуэтка Тлалока} (+20 меткость, +10 орудия, -20 скрытность при ношении)
					case "PKM_SvtvA_amulet":
						if (GetCharacterItem(_refCharacter, "PKM_SvtvA_pismo3") > 0) skillN += 15;// Амулет Хаоса + Книга Хаоса
					break;
				}
				sItemID = "";
			}
			
			//слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "Totem_11": skillN += 5; break;// {Тотем Камаштли} (+5 к орудиям при ношении)
					case "glasses": skillN -= 5; break;//  {Очки} (+15 к скрытности, -5 к пистолетам, - 5 к орудиям, -5 к меткости при ношении)
				}
				sItemID = "";
			}
		break;
		case SKILL_GRAPPLING:
			// слот BLADE_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, BLADE_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "blackbeard_sword") skillN += 20;// {Меч Тритона} (+10 к навигации, +20 к абордажу, +10% к скорости обращения с парусами)
				sItemID = "";
			}
			
			// слот IDOLS_LEFT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_LEFT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "hatWhisper") skillN += 5;// {Шляпа капитана "Чёрной Вдовы"} (+5 к навигации, +5 к авторитету, + 5 к абордажу, -5 к скрытности при ношении)
				sItemID = "";
			}
		break;
		case SKILL_DEFENCE:
			//слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "indian15": skillN += 10; break;// {Базальтовая голова} (+10 авторитет и защита, -10 везение при ношении)
					case "indian22": skillN += 20; break;// {Голова воина племени майя} (+20 защита, +10 лёгкое оружие, -10 авторитет и везение при ношении)
				}
				sItemID = "";
			}
			
			// слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "Totem_3") skillN += 5;// {Тотем Кецалькоатля} (+5 защита при ношении)
				sItemID = "";
			}

			//работают без экипировки
			// Gregg: {Лапа капитана Маугли} (+15 везение и защита, -10 авторитет при ношении)
			if ((GetCharacterItem(_refCharacter, "MaugliPaw") > 0)) skillN += 15;
		break;
		case SKILL_REPAIR:
			// слот JEWELRY_INDIAN_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "STATUE1") skillN += 30;// {Статуэтка Шочипилли} (+30 ремонт при ношении)
				sItemID = "";
			}
			
			// слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "Totem_15") skillN += 5;// {Тотем Шипе-Тотеку} (+5 к починке при ношении)
				sItemID = "";
			}
		break;
		case SKILL_COMMERCE:
			// слот JEWELRY_INDIAN_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, JEWELRY_INDIAN_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{// в данной категории предмет всего один, поэтому обходимся простым if. при добавлении предметов переписать на switch
				if (sItemID == "indian3") skillN += 10;// {Нефритовая маска} (+10 торговля при ношении)
				sItemID = "";
			}
			
			//слот INDIAN_CENTER_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, INDIAN_CENTER_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "indian14": skillN += 20; break;//     {Чаша Ололиуки} (+20 торговля, -20 авторитет при ношении)
					case "DeSouzaCross": skillN += 10; break;// {Крест Антонио де Соуза} (+30 к везению, +20 к авторитету, +10 к торговле при ношении)
				}
				sItemID = "";
			}
			
			//слот IDOLS_RIGHT_ITEM_TYPE:
			sItemID = GetCharacterEquipByGroup(_refCharacter, IDOLS_RIGHT_ITEM_TYPE);
			if (sItemID != "")
			{
				switch (sItemID)
				{
					case "PDM_PJ_BsRL": skillN += 5; break;// {Бутылка с розовой ленточкой} (+5 везение, +5 торговля, -5 скрытность при ношении)
					case "Totem_10": skillN += 5; break;//    {Тотем Тонакатекутли} (+5 торговля при ношении)
				}
				sItemID = "";
			}

			//работают без экипировки
			// Gregg: {Три древние монеты} (+10 везение, авторитет и торговля при ношении)
			if ((GetCharacterItem(_refCharacter, "DisappearPeople_Coins") > 0)) skillN += 10;
		break;
	}

	skillN += SetCharacterSkillBySculArtefact(_refCharacter, skillName);
	skillN += SetCharacterSkillBySuit(_refCharacter, skillName);

	//navy --> действие алкоголя
	if (CheckAttribute(_refCharacter, "chr_ai.drunk.skill." + skillName)) {
		skillN += sti(_refCharacter.chr_ai.drunk.skill.(skillName));
	}
	// boal <--
	if (skillN <= 1) {
		skillN = 1;
	}
	if (skillN > SKILL_MAX) {
		skillN = SKILL_MAX;
	}		
	return skillN;
}
// boal 20.03.2004 -->
int GetShipClassNavySkill(int shipClass)
{
    int needSkill = 0;

    switch (shipClass)
    {
            case 1 : needSkill = 100; break; // LEO: Немного апнули требования навигации 01.12.2021
            case 2 : needSkill = 85; break;
            case 3 : needSkill = 70; break;
            case 4 : needSkill = 50; break;
            case 5 : needSkill = 30; break;
            case 6 : if (MOD_SKILL_ENEMY_RATE == 3) needSkill = 10; else needSkill = 1; break; // LEO: Превозмогаторам страдать 07.12.2021
            case 7 : needSkill = 0; break;
    }
    return needSkill;
}
// boal 20.03.2004 <--
void DelBakSkillAttr(ref _refCharacter) // boal оптимизация скилов
{
    DeleteAttribute(_refCharacter, "BakSkill");
    DeleteAttribute(_refCharacter, "BakSkillCount");
}
// для коэффициента ускорения набора опыта (если ускорение не нужно ставим 1)
float GetExpAcceleration(float modif)
{
	float curve = 2.864178 * pow(modif,-0.457);
	return curve;
}
// сброс всех порогов (буфер расчета, пересчитывается от вещей +1)
void ClearCharacterExpRate(ref _chref)
{
    int    i;
    string name;

    for (i=1; i<15; i++)
    {
        name = GetSkillNameByIdx(i);
        DeleteAttribute(_chref, "skill." + name + "_rate");
    }
}
// LEO: догнать уровень навыка под ТЕКУЩИЙ порог, ничего не начисляя.
//   Порог (skill * ExpRate) считается от S.P.E.C.I.A.L. С модификаторами, а те плывут от здоровья,
//   шмоток и перков. Накопленный опыт разбирался ТОЛЬКО в момент начисления, поэтому после
//   лечения в окне персонажа висело "350/332" и "05%" (то есть 105%) до следующего выстрела.
void ApplyPendingSkillExp(ref _chref, string _skill)
{
	if (!CheckAttribute(_chref, "skill." + _skill)) return;
	string _skill_exp = _skill + "_exp";
	if (!CheckAttribute(_chref, "skill." + _skill_exp)) return;

	int iSkill = sti(_chref.skill.(_skill));
	if (iSkill < 1) return;              // неразученный навык не трогаем
	if (iSkill >= SKILL_MAX) return;

	int iSkillOld = iSkill;
	float fExp = stf(_chref.skill.(_skill_exp));
	float fRate = GetCharacterExpRate(_chref, _skill);
	while (iSkill < SKILL_MAX)           // Storm не сокращает && - второе условие отдельным break
	{
		if (fExp < iSkill * fRate) break;
		fExp -= iSkill * fRate;
		iSkill++;
	}
	if (iSkill == iSkillOld) return;

	_chref.skill.(_skill) = iSkill;
	_chref.skill.(_skill_exp) = fExp;
	ApplayNewSkill(_chref, _skill, iSkill - iSkillOld);
}
// когда меняется порог кача, нужно сбросить очки, иначе будет по 120 процентов
void RefreshCharacterSkillExpRate(ref _chref)
{
    int    i;
    string name;

    for (i=1; i<15; i++)
    {
        name = GetSkillNameByIdx(i);
        ApplyPendingSkillExp(_chref, name); // LEO: было AddCharacterExpToSkill(..., 0.0), а она выходит на нулевом приросте и порог не пересчитывала
        ApplayNewSkill(_chref, name, 0); // иначе перки  будут 23/22
    }
}

float GetSummonSkillFromNameToOld(ref _refCharacter, string skillName)
{
    return (GetSummonSkillFromName(_refCharacter, skillName) / SKILL_TO_OLD);
}
int GetSummonSkillFromName(ref _refCharacter, string skillName)
{
    int sumSkill;

    // boal 051104 метод тормозит страшно - проверка +1 вещей, будет опрос раз в 10
    if (CheckAttribute(_refCharacter, "BakSkill." + skillName))
    {
        if (sti(_refCharacter.BakSkillCount.(skillName)) < 25 )
        {
            _refCharacter.BakSkillCount.(skillName) = sti(_refCharacter.BakSkillCount.(skillName)) + 1;
            return sti(_refCharacter.BakSkill.(skillName));
        }
    }
    sumSkill = GetSummonSkillFromNameSimple(_refCharacter, skillName);
    // boal 27.01.2004 пенальти за крутой корабль -->
    sumSkill = ApplayNavyPenaltyToSkill(_refCharacter, skillName, sumSkill);
    // boal 27.01.2004 <--

    _refCharacter.BakSkill.(skillName)      =  sumSkill; // значение
    _refCharacter.BakSkillCount.(skillName) = rand(5); // счетчик немного размажем пересчет

    return sumSkill;
}

// LEO: сбросить кэш суммарных навыков при СМЕНЕ ЭКИПИРОВКИ - иначе бонусы вещей отдавались старые до смены
//   локации (надел скрытный комплект, а палили по прежнему значению). Кэша ДВА слоя, гасить надо оба:
//   1) BakSkill/BakSkillCount - кэш GetSummonSkillFromName выше, живёт ~25 обращений;
//   2) TmpSkill - снимок 9 навыков (Ship_UpdateTmpSkills@AIShip.c), из него читают проверка фальшивого флага,
//      стрельба, абордаж, ремонт; пересчёт только по TmpSkillRecall==0, иначе ждёт 25+rand(49) тиков.
void ResetSummonSkillCache(ref _refCharacter)
{
    DeleteAttribute(_refCharacter, "BakSkill");
    DeleteAttribute(_refCharacter, "BakSkillCount");
    _refCharacter.TmpSkillRecall = 0;
}

int GetSummonSkillFromNameSimple(ref _refCharacter, string skillName)
{
    int sumSkill = GetCharacterSkillSimple(_refCharacter,skillName);

	if (sti(_refCharacter.index) == sti(pchar.index))
	{
	    int iOfficer = -1;
		int iOffSkill = -1;
		switch (skillName)
		{
			case SKILL_SAILING:
				iOfficer = sti(pchar.Fellows.Passengers.navigator);
			break;
			case SKILL_CANNONS:
				iOfficer = sti(pchar.Fellows.Passengers.cannoner);
			break;
			case SKILL_ACCURACY:
				iOfficer = sti(pchar.Fellows.Passengers.cannoner);
			break;
			case SKILL_GRAPPLING:
				iOfficer = sti(pchar.Fellows.Passengers.boatswain);
			break;
			case SKILL_DEFENCE:
				iOfficer = sti(pchar.Fellows.Passengers.doctor);
			break;
			case SKILL_REPAIR:
				iOfficer = sti(pchar.Fellows.Passengers.carpenter);
			break;
			case SKILL_COMMERCE:
				iOfficer = sti(pchar.Fellows.Passengers.treasurer);
			break;
		}
		if(iOfficer != -1)
		{
			iOffSkill = GetCharacterSkillSimple(&characters[iOfficer], skillName);
			if (bNoBonusSkillOff)
			{
				if ((iOffSkill - sumSkill) > 25) sumSkill = sumSkill + 25;
				else
				{
					if (iOffSkill > sumSkill)
					{
						sumSkill = iOffSkill;
					}
				}
			}
			else
			{
				if (iOffSkill > sumSkill)
				{
					sumSkill = iOffSkill;
				}
			}
		}
	}
	return sumSkill;
}

void AddExpAndShow(ref chref, int _exp)
{
}
// boal -->

void AddCharacterExpToSkillSquadron(ref _refCharacter, string _skill, float _addValue)
{
    int cn,i;
	ref chref;

	for(i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(_refCharacter,i);
		if(cn!=-1)
		{    // и квестовым тоже
			AddCharacterExpToSkill(GetCharacter(cn), _skill, _addValue);
		}
	}
}

string TransformSkillToIcon(string _skill) //преобразует латинское наименование скила в иконку
{
	switch (_skill)
	{
		case "Leadership": return "À";
		break;
		case "FencingLight": return "Á";
		break;
		case "Fencing": return "Â";
		break;
		case "FencingHeavy": return "Ã";
		break;
		case "Pistol": return "Ä";
		break;
		case "Fortune": return "Å";
		break;
		case "Sneak": return "Æ";
		break;
		case "Sailing": return "Ç";
		break;
		case "Accuracy": return "È";
		break;
		case "Cannons": return "É";
		break;
		case "Grappling": return "Ê";
		break;
		case "Defence": return "Ë";
		break;
		case "Commerce": return "Í";
		break;
		case "Repair": return "Ì";
		break;
	}
	return "";
}

string TransformNationToIcon(int iNation) //преобразует нацию в иконку
{
	switch (iNation)
	{
		case ENGLAND: return "Ď";
		break;
		case FRANCE: return "Ḑ";
		break;
		case HOLLAND: return "Ɖ";
		break;
		case SPAIN: return "Đ";
		break;
		case PIRATE: return "Ð";
		break;
	}
	return "";
}

// две команды в одной. первая начисляет опыт, вторая дает опыт от ГГ напарникам/пассажирам
void AddCharacterExpToSkill(ref _chref, string _skill, float _addValue)
// _chref - character _skill - name ex -"Fencing"  _skill_exp = "Fencing_exp"   _addValue = 100
{
	if (_addValue <= 0.0) {// TODO: заменить на отнимание навыка
		return;
	}
	if (!CheckAttribute(_chref, "skill." + _skill)) {
		return;
	}

	if (sti(InterfaceStates.ShowExpLogs) == 1 && _addValue > 0)
	{
		if (IsMainCharacter(_chref))
		{
			string icon=TransformSkillToIcon(_skill);
			if(ExpLog_IsSea()) ExpLog_AddSea(_skill,_addValue,icon,0,false);
			else ExpLog_PushLand("ᶑ"+FloatToString(_addValue,2)+" "+icon+" exp");
		}
	}

	string _skill_exp = _skill + "_exp";
	if (!CheckAttribute(_chref, "skill." + _skill_exp)) {
		_chref.skill.(_skill_exp) = 0;
	}

	int iChRefSkill = sti(_chref.skill.(_skill)); //колво единиц навыка

	if (iChRefSkill < SKILL_MAX) { // skill <= 0 это вылет
		int iChRefSkillOld = iChRefSkill;
		float fChRefSkillExp = stf(_chref.skill.(_skill_exp)); //прокачка единицы навыка
		float fCharExpRate = GetCharacterExpRate(_chref, _skill);//не обновляется   

		fChRefSkillExp = fChRefSkillExp + _addValue;

		while (iChRefSkill < SKILL_MAX && fChRefSkillExp >= (iChRefSkill * fCharExpRate)) {
			fChRefSkillExp -= (iChRefSkill * fCharExpRate);

			iChRefSkill++;

		}
		if (CheckAttribute(_chref, "BakSkill." + _skill) && iChRefSkill > sti(_chref.BakSkill.(_skill)))
		{
			DeleteAttribute(_chref, "BakSkill." + _skill);
			DeleteAttribute(_chref, "BakSkillCount." + _skill);
		}
		// оптимизация скилов
		if (iChRefSkill - iChRefSkillOld) {
			_chref.skill.(_skill) = iChRefSkill;
			ApplayNewSkill(_chref, _skill, (iChRefSkill - iChRefSkillOld));
			if (sti(InterfaceStates.ShowExpLogs) == 1)
			{
				if (IsMainCharacter(_chref))
				{
					ExpLog_PushLand(TransformSkillToIcon(_skill)+"Ö "+_chref.skill.(_skill));
				}
			}
		}
		_chref.skill.(_skill_exp) = fChRefSkillExp;
	}
	else if (_skill_exp != "Commerce_exp")
	{
		_chref.skill.(_skill_exp) = stf(_chref.skill.(_skill_exp)) + _addValue;
		if (makeint(_chref.skill.(_skill_exp)) >= GetCharacterExpRate(_chref, _skill) * 100)
		{
			_chref.skill.(_skill_exp) = 0;
			AddExcessivePoints(_chref, _skill, 1);
		}
	}
	/// officers
	if (sti(_chref.index) != GetMainCharacterIndex()) {// только для ГГ 
		return;
	}
	int cn, i, iPas;
	_addValue *= (0.25 + 0.75 * CheckCharacterPerk(_chref, "SharedExperience"));
	float fExpMult = 0;
	ref chRefOfficer;
	iPas = GetPassengersQuantity(_chref); // оптимиация
	for (i = 0; i < iPas; i++) {
		cn = GetPassenger(_chref, i);
		if (cn == -1) {
			continue;
		}
		chRefOfficer = GetCharacter(cn);
		if (CheckAttribute(chRefOfficer, "prisoned")) {
			continue;
		}
		fExpMult = 0.25 + 0.75 * isOfficerInShip(chRefOfficer, true);
		iChRefSkill = sti(chRefOfficer.skill.(_skill)); //колво единиц навыка
		fCharExpRate = GetCharacterExpRate(chRefOfficer, _skill);//не обновляется 
		if (iChRefSkill < SKILL_MAX) { // skill <= 0 это вылет
			iChRefSkillOld = iChRefSkill;
			fChRefSkillExp = stf(chRefOfficer.skill.(_skill_exp)); //прокачка единицы навыка

			fChRefSkillExp = fChRefSkillExp + fExpMult * _addValue;

			while (iChRefSkill < SKILL_MAX && fChRefSkillExp >= (iChRefSkill * fCharExpRate)) {
				fChRefSkillExp -= (iChRefSkill * fCharExpRate);

				iChRefSkill++;
			}
			// оптимизация скилов
			if (CheckAttribute(chRefOfficer, "BakSkill." + _skill))
			{
				if (iChRefSkill > sti(chRefOfficer.BakSkill.(_skill)))
				{
					DeleteAttribute(chRefOfficer, "BakSkill." + _skill);
					DeleteAttribute(chRefOfficer, "BakSkillCount." + _skill);
				}
			}
			if (iChRefSkill - iChRefSkillOld) {
				chRefOfficer.skill.(_skill) = iChRefSkill;
				ApplayNewSkill(chRefOfficer, _skill, (iChRefSkill - iChRefSkillOld));
			}
			chRefOfficer.skill.(_skill_exp) = fChRefSkillExp;
		}
		else
		{
			chRefOfficer.skill.(_skill_exp) = stf(chRefOfficer.skill.(_skill_exp)) + fExpMult * _addValue;
			if (makeint(chRefOfficer.skill.(_skill_exp)) >= fCharExpRate * 100)
			{
				chRefOfficer.skill.(_skill_exp) = 0;
				AddExcessivePoints(chRefOfficer, _skill, 1);
			}
		}
	}

	for (i = 1; i < COMPANION_MAX; i++) {
		cn = GetCompanionIndex(_chref, i);
		if (cn == -1) {
			continue;
		}
		chRefOfficer = GetCharacter(cn);

		iChRefSkill = sti(chRefOfficer.skill.(_skill)); //колво единиц навыка
		iChRefSkillOld = iChRefSkill;
		fCharExpRate = GetCharacterExpRate(chRefOfficer, _skill);//не обновляется   
		if (iChRefSkill < SKILL_MAX) { // skill <= 0 это вылет
			fChRefSkillExp = stf(chRefOfficer.skill.(_skill_exp)); //прокачка единицы навыка
			fChRefSkillExp = fChRefSkillExp + _addValue;

			while (iChRefSkill < SKILL_MAX && fChRefSkillExp >= (iChRefSkill * fCharExpRate)) {
				fChRefSkillExp -= (iChRefSkill * fCharExpRate);

				iChRefSkill++;
			}
			// оптимизация скилов
			if (CheckAttribute(chRefOfficer, "BakSkill." + _skill))
			{
				if (iChRefSkill > sti(chRefOfficer.BakSkill.(_skill)))
				{
					DeleteAttribute(chRefOfficer, "BakSkill." + _skill);
					DeleteAttribute(chRefOfficer, "BakSkillCount." + _skill);
				}
			}
			if (iChRefSkill - iChRefSkillOld) {
				chRefOfficer.skill.(_skill) = iChRefSkill;
				ApplayNewSkill(chRefOfficer, _skill, (iChRefSkill - iChRefSkillOld));
			}
			chRefOfficer.skill.(_skill_exp) = fChRefSkillExp;
		}
		else
		{
			chRefOfficer.skill.(_skill_exp) = stf(chRefOfficer.skill.(_skill_exp)) + _addValue;
			if (makeint(chRefOfficer.skill.(_skill_exp)) >= fCharExpRate * 100)
			{
				chRefOfficer.skill.(_skill_exp) = 0;
				AddExcessivePoints(chRefOfficer, _skill, 1);
			}
		}
	}
}

void AddExcessivePoints(ref _chref, string _skill, int _addValue)
{
	int iFreePoints_exp = 0;
	if (isSelfTypeSkill(_skill)) {
		if(CheckAttribute(_chref,"perks.FreePoints_self_exp")) {
			iFreePoints_exp = sti(_chref.perks.FreePoints_self_exp);
		}
		iFreePoints_exp += _addValue;
		if (iFreePoints_exp >= GetFreePoints_SelfRate(_chref)) {
			iFreePoints_exp -= GetFreePoints_SelfRate(_chref);
			if(CheckAttribute(_chref,"perks.FreePoints_self")) {
				_chref.perks.FreePoints_self = sti(_chref.perks.FreePoints_self) + 1;
			}
			else {	
				_chref.perks.FreePoints_self = 1;
			}
			if (sti(_chref.index) == GetMainCharacterIndex()) {
				Log_SetStringToLog(XI_ConvertString("Personal abilities") + " + 1");
			}
		}
		_chref.perks.FreePoints_self_exp = iFreePoints_exp;
    }
    else {
		if(CheckAttribute(_chref,"perks.FreePoints_ship_exp")) {
			iFreePoints_exp = sti(_chref.perks.FreePoints_ship_exp);
		}
		iFreePoints_exp += _addValue;
		if (iFreePoints_exp >= GetFreePoints_ShipRate(_chref)) {
			iFreePoints_exp -= GetFreePoints_ShipRate(_chref);
			if(CheckAttribute(_chref,"perks.FreePoints_ship")) {
				_chref.perks.FreePoints_ship = sti(_chref.perks.FreePoints_ship) + 1;
			}
			else {
				_chref.perks.FreePoints_ship = 1;
			}
			if (sti(_chref.index) == GetMainCharacterIndex()) {
				Log_SetStringToLog(XI_ConvertString("Ship abilities") + " + 1");
			}
		}
		_chref.perks.FreePoints_ship_exp = iFreePoints_exp;
    }
}

string GetPerkListText(ref _chref, string _type)
{
    int    perksQ,i;
    int    lngFilePerkID = -1;
	string perkName;
	string ret;
	aref   arPerksRoot;

    lngFilePerkID = LanguageOpenFile("AbilityDescribe.txt");
	// Варианты перков, которые можно добавить
	makearef(arPerksRoot, _chref.perks.list);
	perksQ = GetAttributesNum(arPerksRoot);
	ret = "";
	for(i=0; i<perksQ; i++)
	{
		perkName = GetAttributeName(GetAttributeN(arPerksRoot,i));

        if (CheckAttribute(&ChrPerksList, "list." + perkName) && CheckAttribute(&ChrPerksList, "list." + perkName + ".BaseType"))
        {
            if (ChrPerksList.list.(perkName).BaseType == _type)
            {
                ret += "* " + LanguageConvertString(lngFilePerkID, perkName) + " ";
            }
        }
	}
	LanguageCloseFile(lngFilePerkID);
	return ret;
}
// boal Вес предметов 19.01.2004 -->
float GetItemsWeight(ref _chref)
{
    float   Amount = 0;
    string  itemID;
    ref     itm;

    if (bCabinStarted || bAbordageStarted || !bSeaActive || !CheckAttribute(_chref, "ItemsWeight") || CheckForExchangeAllowed(_chref))//TODO: перебрать условия
    {
		aref arInventory, arItem;
		makearef(arInventory, _chref.Items);
		int inventorySize = GetAttributesNum(arInventory);

		for (int i = 0; i < inventorySize; i++)
		{
			arItem = GetAttributeN(arInventory, i);
			itemID = GetAttributeName(arItem);
			itm = ItemsFromID(itemID);
			Amount += sti(_chref.items.(itemID)) * stf(itm.weight);
        }
        _chref.ItemsWeight = Amount; // оптимицация тормозов в бою на море
    }
    else
    {
        Amount = stf(_chref.ItemsWeight);
    }
    return Amount;
}

bool CheckForExchangeAllowed(ref _chref)
{
	if(!LAi_group_IsActivePlayerAlarm())
	{
		if (bSeaActive || CheckAttribute(loadedLocation,"fastreload") || CheckAttribute(loadedLocation,"townsack") || _chref.location == pchar.location)
		{
			bool ok = CheckAttribute(_chref, "prisoned") && sti(_chref.prisoned) == true;
			if (!ok && !CheckAttribute(_chref,"isquest") && !CheckAttribute(_chref,"nonremovable")) return true;
		}

	}
	else {return false;}
	return false;
}

int GetMaxItemsWeight(ref _chref)
{
	if (CheckAttribute(_chref, "UnlimitedWeight"))
	{
		return 9999999; // бесконечный переносимый
	}
	if (CheckAttribute(_chref, "Skill.Fencing"))
    {
        int iBonus = 9999999;
		//if (IsCharacterPerkOn(_chref, "EnergyPlus")) iBonus = 30;
		//if (IsCharacterPerkOn(_chref, "EnergyPlusFixed")) iBonus = 30;
		//if (IsCharacterPerkOn(_chref, "Trader")) iBonus += 35;
		// Lugger -->
	        if(CheckAttribute(_chref, "equip.backpack"))
	        {
	        	if(_chref.equip.backpack != "")
	        	{
	        		ref BackPack = ItemsFromID(_chref.equip.backpack);
	        		int iBackPack = sti(BackPack.BackPackWeight);

	        		iBonus += iBackPack;
	        	}
	        }
		// Lugger <--
		if(IsEquipCharacterByArtefact(_chref, "talisman5")) iBonus += 30;
        //опасная рекурсия  если писать GetCharacterSPECIAL
		if (CheckAttribute(_chref, "chr_ai.bonusweighttube")) iBonus += 60 + sti(_chref.rank);
        if (bDifficultyWeight) iBonus = iBonus + CHAR_ITEMS_WEIGHT + GetCharacterSPECIALSimple(_chref, SPECIAL_S)*(GetCharacterSPECIALSimple(_chref, SPECIAL_E) + 12);
		else iBonus = iBonus + CHAR_ITEMS_WEIGHT + GetCharacterSPECIALSimple(_chref, SPECIAL_S)*(GetCharacterSPECIALSimple(_chref, SPECIAL_E) + 12 - MOD_SKILL_ENEMY_RATE*3);
        return  iBonus;
    }
    else
    {
        return 999999; // сундук или труп не имееют скила и ограничения
    }
}
// boal 19.01.2004 <--

// boal -->
int Statistic_AddValue(ref _chref, string _attrName, int _add) // set and get(_add==0) value
{
    // KillShip_ 1 ..7
    // AbordShip_ 1..7
    // KillAbordShip_ 1..7
    // Solder_s  Solder_g ( s=sword g=gun)
    // Citizen_s  Citizen_g
    // Monster_s Monster_g
    // Warrior_s  Warrior_g
    // Fort
	if(!CheckAttribute(_chref, "index")) return 0;

    if (sti(_chref.index) != GetMainCharacterIndex()) return 0; // оптимизация
    if( !CheckAttribute(_chref,"Statistic." + _attrName) )
    {
        _chref.Statistic.(_attrName) = 0;
    }
    _chref.Statistic.(_attrName) = sti(_chref.Statistic.(_attrName)) + _add;

    if (sti(_chref.Statistic.(_attrName)) < 0)
    {
        _chref.Statistic.(_attrName) = 0;
    }

	if (_attrName == "PartitionPay") //фикс - выплата денег команде в море считалась убытком и уменьшала раздел добычи
	{
		int iCount;
		if (CheckAttribute(NullCharacter, "Partition")) 
		{
			iCount = sti(NullCharacter.Partition);
			aref arCurPart;
			makearef(arCurPart, NullCharacter.Partition.(iCount));
			if (!CheckAttribute(arCurPart, "after"))//мы в море. инфа о выходе есть, а о возвращении ещё нет
			{
				arCurPart.before.Money = sti(arCurPart.before.Money) - _add;//магия. Уменьшаю начальную сумму денег при выходе в море. Кривое, но самое простое решение. 
			}
		}
	}

	//Открываем достижения
	// --->
	if(_add > 0)
	{
		int monstersall = sti(_chref.Statistic.Monster_s)+sti(_chref.Statistic.Monster_g);
		int killsall = sti(_chref.Statistic.Solder_s)+sti(_chref.Statistic.Citizen_s)+sti(_chref.Statistic.Warrior_s)+sti(_chref.Statistic.Monster_s)+sti(_chref.Statistic.Solder_g)+sti(_chref.Statistic.Citizen_g)+sti(_chref.Statistic.Warrior_g)+sti(_chref.Statistic.Monster_g);
		int aboardships = sti(_chref.Statistic.AbordShip_1)+sti(_chref.Statistic.AbordShip_2)+sti(_chref.Statistic.AbordShip_3)+sti(_chref.Statistic.AbordShip_4)+sti(_chref.Statistic.AbordShip_5)+sti(_chref.Statistic.AbordShip_6)+sti(_chref.Statistic.AbordShip_7)+sti(_chref.Statistic.KillAbordShip_1)+sti(_chref.Statistic.KillAbordShip_2)+sti(_chref.Statistic.KillAbordShip_3)+sti(_chref.Statistic.KillAbordShip_4)+sti(_chref.Statistic.KillAbordShip_5)+sti(_chref.Statistic.KillAbordShip_6)+sti(_chref.Statistic.KillAbordShip_7);
		int killshipsall = sti(_chref.Statistic.KillShip_1)+sti(_chref.Statistic.KillShip_2)+sti(_chref.Statistic.KillShip_3)+sti(_chref.Statistic.KillShip_4)+sti(_chref.Statistic.KillShip_5)+sti(_chref.Statistic.KillShip_6)+sti(_chref.Statistic.KillShip_7)+aboardships;

		// Убийства нежити
		CheckAndUnlock("skel_kills", monstersall);

		// Общие убийства
		CheckAndUnlock("kills", killsall);

		// Потопления кораблей
		CheckAndUnlock("ship_kills", killshipsall);

		if(!CheckCharacterPerk(pchar,"AboardCollector") && killsall >= 200 && aboardships >= 20) {SetCharacterPerk(pchar, "AboardCollector"); log_info(GetConvertStrWithReplace("Variable_RPGUtilite_4", "Logs.txt", "#space#", " "));}
	}
	// <---
    return sti(_chref.Statistic.(_attrName));
}
//==> Gregg: счётчик добрых дел -> перк "Вызывающий доверие"
// Перк выдаётся ЧИСТО ПО ОЧКАМ, в обход condition.AdvancedCommerce/IronWill:
// условия перков проверяются только в экране выбора (INTERFACE\character.c),
// в рантайме GetCharacterPerkUsing смотрит лишь наличие атрибута.
// Штатный путь покупки перка при этом не трогаем - ветка добрых дел просто добавочная.

// Gregg: порог зависит от сложности: 24 юнга / 27 капитан / 30 адмирал.
// Считать НА ЛЕТУ, не запоминать: игрок может сменить сложность в опциях.
int GetGoodDeedsTarget()
{
	return 21 + 3 * MOD_SKILL_ENEMY_RATE;
}

int GetGoodDeedsCount()
{
	if (!CheckAttribute(pchar, "GoodDeeds.count")) return 0;
	return sti(pchar.GoodDeeds.count);
}

// Gregg: начисление очков и проверка порога. Наружу не звать - есть две обёртки ниже.
void GoodDeed_Grant(int iPoints)
{
	if (iPoints < 1) return;

	int iCount = GetGoodDeedsCount() + iPoints;
	pchar.GoodDeeds.count = iCount;

	if (CheckCharacterPerk(pchar, "Trustworthy")) return;
	if (iCount < GetGoodDeedsTarget()) return;

	SetCharacterPerk(pchar, "Trustworthy");
	Log_Info(GetConvertStrWithReplace("Variable_GoodDeeds_1", "Logs.txt", "#space#", " "));
}

// Gregg: ОДНОРАЗОВОЕ доброе дело. sDeedId - уникальное имя источника, второй раз
// тот же источник очков не даёт. Для сюжетных квестов и разовых поступков.
void AddGoodDeed(string sDeedId, int iPoints)
{
	if (CheckAttribute(pchar, "GoodDeeds.done." + sDeedId)) return;
	pchar.GoodDeeds.done.(sDeedId) = "1";

	GoodDeed_Grant(iPoints);
}

// Gregg: ПОВТОРЯЕМОЕ доброе дело - засчитывается каждый раз, без дедупа.
// Для генераторов (церковь, пассажиры, конвои) - там повтор и есть смысл.
void AddGoodDeedRepeat(int iPoints)
{
	GoodDeed_Grant(iPoints);
}
//<== Gregg: счётчик добрых дел

void Statistic_KillChar(aref _attack, aref _enemy, string _attrName)
{
	if (sti(_attack.index) != GetMainCharacterIndex()) return; // оптимизация
	string  name = GetCharType(_enemy);

	name = name + _attrName;

	Statistic_AddValue(_attack, name, 1);
}
string GetCharType(aref _enemy)  //TO_DO переделать на тип в НПС
{
    string  name  = "Warrior"; // define
    string  model = _enemy.model;
	string  sex = _enemy.sex;
    switch (_enemy.chr_ai.type)
	{
		case LAI_TYPE_PATROL :
		    name = "Solder";
		break
		case LAI_TYPE_GUARDIAN :
		    name = "Solder";
		break
		case LAI_TYPE_OFFICER :
		    name = "Solder";
		break;
        // Citizen
		case LAI_TYPE_ACTOR :
		    name = "Citizen";
		break;
		case LAI_TYPE_BARMAN :
		    name = "Citizen";
		break;
		case LAI_TYPE_CITIZEN :
		    name = "Citizen";
		break;
		case LAI_TYPE_HUBER :
		    name = "Citizen";
		break;
		case LAI_TYPE_HUBERSTAY :
		    name = "Citizen";
		break;
		case LAI_TYPE_MERCHANT :
		    name = "Citizen";
		break;
		case LAI_TYPE_POOR :
		    name = "Citizen";
		break;
		case LAI_TYPE_PRIEST :
		    name = "Citizen";
		break;
		case LAI_TYPE_SIT :
		    name = "Citizen";
		break;
		case LAI_TYPE_STAY :
		    name = "Citizen";
		break;
		case LAI_TYPE_WAITRESS :
		    name = "Citizen";
		break;
		case LAI_TYPE_OWNER :
		    name = "Citizen";
		break;
		case LAI_TYPE_WARRIOR : // to_do преверить тип
		    if (model == "mummy" || HasSubStr(model,"Skel") || sex == "skeleton")
		    {
		        name = "Monster";
		    }
		    else
		    {
                if (findsubstr(model, "off_" , 0) != -1)
                {
                    name = "Solder";
                }
                else
                {
                    if (findsubstr(model, "sold_" , 0) != -1)
	                {
	                    name = "Solder";
	                }
                }
		    }
		break;
	}
	return  name;
}
// boal statistic info 17.12.2003 <--

// boal Блок здоровье ГГ -->
int GetHealthNum(ref ch)
{
    if (!CheckAttribute(ch, "Health"))
	{
        return 0;
	}
    int i;

    i = makeint((stf(ch.Health.HP) + 9) / 10.0); // до целого

    return i;
}

int GetHealthMaxNum(ref ch)
{
    if (!CheckAttribute(ch, "Health"))
	{
        return 0;
	}
    int i;

    i = makeint((stf(ch.Health.maxHP) + 9) / 10.0); // до целого

    return i;
}
string GetHealthName(ref ch)
{
    string name = "";
	if (LanguageGetLanguage() != "russian")
	{
		switch (GetHealthNum(ch))
    	{
    	    case 1:
    	        name = "TERRIBLE";
    	    break;
    	    case 2:
    	        name = "BAD";
    	    break;
    	    case 3:
    	        name = "NOT BAD";
    	    break;
    	    case 4:
    	        name = "NORMAL";
    	    break;
    	    case 5:
    	        name = "GOOD";
    	    break;
    	    case 6:
    	        name = "EXCELLENT";
    	    break;
    	}
	}
	else
	{
		switch (GetHealthNum(ch))
    	{
    	    case 1:
    	        name = "УЖАСНОЕ";
    	    break;
    	    case 2:
    	        name = "ПЛОХОЕ";
    	    break;
    	    case 3:
    	        name = "НЕВАЖНОЕ";
    	    break;
    	    case 4:
    	        name = "СРЕДНЕЕ";
    	    break;
    	    case 5:
    	        name = "ХОРОШЕЕ";
    	    break;
    	    case 6:
    	        name = "ОТЛИЧНОЕ";
    	    break;
    	}
	}
    return name;
}

string GetHealthNameMaxSmall(ref ch)
{
    string name = "";
	if (LanguageGetLanguage() == "russian")
	{
		switch (GetHealthMaxNum(ch))
    	{
    	    case 1:
    	        name = "ужасное";
    	    break;
    	    case 2:
    	        name = "плохое";
    	    break;
    	    case 3:
    	        name = "неважное";
    	    break;
    	    case 4:
    	        name = "среднее";
    	    break;
    	    case 5:
    	        name = "хорошее";
    	    break;
    	    case 6:
    	        name = "отличное";
    	    break;
    	}
	}
	else
	{
		switch (GetHealthMaxNum(ch))
    	{
    	    case 1:
    	        name = "terrible";
    	    break;
    	    case 2:
    	        name = "bad";
    	    break;
    	    case 3:
    	        name = "not bad";
    	    break;
    	    case 4:
    	        name = "normal";
    	    break;
    	    case 5:
    	        name = "good";
    	    break;
    	    case 6:
    	        name = "excellent";
    	    break;
    	}
	}
    return name;
}

void SetNewDayHealth()
{
    ref mainChr = GetMainCharacter();

    float maxhp = stf(mainChr.chr_ai.hp_max);
    float damg  = stf(mainChr.Health.Damg);
    int   add   = 0;

    mainChr.Health.Damg = 0.0;

    if (IsCharacterPerkOn(mainChr, "HPPlusFixed"))
	{
        add = 1;
    }
    if (damg >= (maxhp / 1.5) )
    {
        if ((damg / maxhp) > add)
        {
            AddCharacterHealth(mainChr, -damg / maxhp + add);
        }
    }
    else
    {
        if (!IsEntity(worldMap)) // не на карте
        {
            AddCharacterHealth(mainChr, 1 + add);
        }
        else
        {
            AddCharacterHealth(mainChr, 0.5 + add / 2.0);
        }
    }
}

void SetNewDayHealthMax()
{
    ref mainChr = GetMainCharacter();

    float maxhp = stf(mainChr.chr_ai.hp_max);
    float damg  = stf(mainChr.Health.weekDamg);

    mainChr.Health.weekDamg = 0.0;

    if (damg > (maxhp * 3.5))
    {
        AddCharacterMaxHealth(mainChr, -1);
    }
}

void AddCharacterHealth(ref mainChr, float add)
{
	//Lipsar квест История давней дружбы
	if(CheckAttribute(PChar,"Luke.BadPoison")) return;
    float maxhp = stf(mainChr.chr_ai.hp_max);
    int   remHP = GetHealthNum(mainChr);

    mainChr.Health.HP = makefloat(stf(mainChr.Health.HP) + add);
    if (stf(mainChr.Health.HP) < 1)
    {
        mainChr.Health.HP = 1.0;
    }
    if (stf(mainChr.Health.HP) > stf(mainChr.Health.maxHP))
    {
        mainChr.Health.HP = stf(mainChr.Health.maxHP);
    }
	if (LanguageGetLanguage() != "russian")
	{
		if (GetHealthNum(mainChr) > remHP)
    	{
    	    Log_Info("Health became better");
    	}
    	if (GetHealthNum(mainChr) < remHP)
    	{
    	    Log_Info("Health became worse");
    	}
	}
	else
	{
		if (GetHealthNum(mainChr) > remHP)
    	{
    	    Log_Info("Здоровье стало лучше");
    	}
    	if (GetHealthNum(mainChr) < remHP)
    	{
    	    Log_Info("Здоровье стало хуже");
    	}
	}
}

void AddCharacterMaxHealth(ref mainChr, float add)
{
    mainChr.Health.maxHP = makefloat(stf(mainChr.Health.maxHP) + add);

    if (stf(mainChr.Health.maxHP) < 1)
    {
        mainChr.Health.maxHP = 1.0;
    }
    if (stf(mainChr.Health.maxHP) > 60)
    {
        mainChr.Health.maxHP = 60.0;
    }
    if (stf(mainChr.Health.HP) > stf(mainChr.Health.maxHP))
    {
        mainChr.Health.HP = stf(mainChr.Health.maxHP);
    }
}
// boal Блок здоровье ГГ <--

// лог изменения отношения с нацией, привязан к опции "Показ получаемых очков опыта" (ShowExpLogs)
void LogNationRelationChange(string _huntName, int _change, int _current) // _change - изменение отношения, _current - текущее отношение (отношение = -счёт охотников)
{
	if (_change == 0 || sti(InterfaceStates.ShowExpLogs) != 1) return;

	int iNation = ENGLAND;
	switch (_huntName)
	{
		case "enghunter":  iNation = ENGLAND; break;
		case "frahunter":  iNation = FRANCE; break;
		case "spahunter":  iNation = SPAIN; break;
		case "holhunter":  iNation = HOLLAND; break;
		case "pirhunter":  iNation = PIRATE; break;
	}
	string sIcon = TransformNationToIcon(iNation);
	// LEO: заметная цветная строка о смене награды за голову + звук журнала — ТОЛЬКО когда награда за голову реально есть (сейчас или была до изменения).
	// Отношение инверсно счёту охотников: _change<0 -> награда ВЫРОСЛА (красный), _change>0 -> снизилась (зелёный).
	int scoreNow  = 0 - _current;        // счёт охотников сейчас (награда за голову появляется при счёте >= 10)
	int scorePrev = scoreNow + _change;  // счёт охотников до изменения (_change = scorePrev - scoreNow)
	if (scoreNow >= 10 || scorePrev >= 10)
	{
		string sMk = "\1"; // LEO: маркер "не красить" (движок переводит печатаемую \1 -> байт 0x01); вокруг иконки, чтобы флаг нации не тинтовался цветом строки
		if (_change < 0) Log_InfoColoredAnim("red",   sMk + sIcon + sMk + " " + XI_ConvertString("BountyHeadUp"));
		else             Log_InfoColoredAnim("green", sMk + sIcon + sMk + " " + XI_ConvertString("BountyHeadDown"));
		PlayStereoSound("interface\bort_jurnal.wav");
	}
	ExpLog_Reputation("RelNation"+iNation,_change,sIcon,_current);
}

// лог изменения репутации ГГ, привязан к опции "Показ получаемых очков опыта" (ShowExpLogs)
void LogReputationChange(int _change, int _current) // _change - изменение репутации, _current - текущая репутация
{
	if (_change == 0 || sti(InterfaceStates.ShowExpLogs) != 1) return;

	ExpLog_Reputation("Reputation",_change,"Ƒ",_current);
}

// лог изменения отношения с контрабандистами, привязан к опции "Показ получаемых очков опыта" (ShowExpLogs)
void LogContrabandRelationChange(int _change, int _current) // _change - изменение отношения, _current - текущее отношение
{
	if (_change == 0 || sti(InterfaceStates.ShowExpLogs) != 1) return;

	ExpLog_Reputation("Contraband",_change,"Ł",_current);
}

// увеличение счетчика награды за голову -->
int ChangeCharacterHunterScore(ref chref, string _huntName, int incr)
{
	int prevVal = -5; // мин набор

	if (_huntName == "pirhunter") return 0;
	if (CheckAttribute(chref, "GenQuest.HunterScore2Pause")) incr = 0;

	if (CheckAttribute(chref, "reputation." + _huntName) )	prevVal = sti(chref.reputation.(_huntName));

	int newVal = prevVal + incr;
	if (newVal < -100)  newVal = -100;
	if (newVal > 100)   newVal = 100;

	chref.reputation.(_huntName) = newVal;

	if( sti(chref.index) != GetMainCharacterIndex() ) return newVal;

	LogNationRelationChange(_huntName, prevVal - newVal, 0 - newVal); // отношение инверсно счёту охотников

	// смена НАИМЕНОВАНИЯ отношения — пишем только когда награды за голову нет (ни сейчас, ни до);
	// саму смену награды за голову прописывает LogNationRelationChange выше (цветная строка + звук журнала)
	if (newVal < 10 && prevVal < 10)
	{
		int iNation = 0;
		switch (_huntName) // код для совместимости со старым
		{
		    case "enghunter":  iNation = ENGLAND; break;
		    case "frahunter":  iNation = FRANCE; break;
		    case "spahunter":  iNation = SPAIN; break;
		    case "holhunter":  iNation = HOLLAND; break;
		}
		string prevName = GetNationReputationName(iNation, -prevVal);
		string newName  = GetNationReputationName(iNation, -newVal);
		if (prevName!=newName)
		{
			string outString = XI_ConvertString("Your reputation with country")+ XI_ConvertString(Nations[iNation].Name + "Gen") +" ";
			if (incr<0)	{outString+=XI_ConvertString("increase");}
			else	{outString+=XI_ConvertString("decrease");}
			outString += " "+XI_ConvertString("to")+" "+newName;
			Log_SetStringToLog(outString);
		}
	}
	return newVal;
}

int ChangeCharacterNationReputation(ref chref, int _Nation, int incr)
{
	return -ChangeCharacterHunterScore(chref, NationShortName(_Nation) + "hunter", -incr); // все наоборот, - это хорошо, + есть НЗГ
}

string GetNationReputation(ref chref, int _Nation)
{
    return GetNationReputationName(_Nation, ChangeCharacterNationReputation(chref, _Nation, 0));
}

string GetNationReputationName(int _Nation, int i)
{
	if (LanguageGetLanguage() != "russian")
	{
		if (i <= -10)  return "Bounty on the head " + (-i*10000);
    	if (i < 0 )    return "Hostile";
    	if (i <= 20 )  return "Neutral";
    	if (i <= 50 )  return "Good";
    	if (i <= 80 )  return "Excellent";
    	if (i <= 100 ) return "Admiration";
	}
	else
	{
		if (i <= -10)  return "Награда за голову " + (-i*10000);
    	if (i < 0 )    return "Враждебная";
    	if (i <= 20 )  return "Нейтральная";
    	if (i <= 50 )  return "Хорошая";
    	if (i <= 80 )  return "Отличная";
    	if (i <= 100 ) return "Восхищение";
	}

    return "Отличная";
}
// boal 04.04.04
string NationShortName(int iNation)
{
    switch(iNation)
	{
		case ENGLAND:
            return "eng";
		break;
		case FRANCE:
            return "fra";
		break;
		case SPAIN:
            return "spa";
		break;
		case HOLLAND:
			return "hol";
		break;
		case PIRATE:
			return "pir";
		break;
	}
	return "eng";
}
// увеличение счетчика награды за голову <--

void setWDMPointXZ(string _location)
{
    // координаты на гловал карте -->
    int n;
	string sTemp;

    n = FindIslandBySeaLocation(_location);
	if(n!=-1)
	{
		worldMap.island = Islands[n].id;
		Pchar.curIslandId = worldMap.island; // fix
		sTemp = worldMap.island;
		if (CheckAttribute(&worldMap, "islands." + sTemp))
		{
		    worldMap.zeroX = worldMap.islands.(sTemp).position.x;
			worldMap.zeroZ = worldMap.islands.(sTemp).position.z;
			if (CheckAttribute(&worldMap, "islands." + sTemp + "." + _location))
			{
			    worldMap.playerShipX = worldMap.islands.(sTemp).(_location).position.x;
				worldMap.playerShipZ = worldMap.islands.(sTemp).(_location).position.z;
			}
			else
			{// для бухт с незаданными координатами
			    worldMap.playerShipX = worldMap.zeroX;
				worldMap.playerShipZ = worldMap.zeroZ;
			}
		}
	}
	// координаты на гловал карте <--
}
// нужно не перекрывать ещё и признаки фантома
void ChangeAttributesFromCharacter(ref CopyChref, ref PastChref, bool _dialogCopy)
{
    aref arToChar;
    aref arFromChar;

	DeleteAttribute(CopyChref,"model");
	DeleteAttribute(CopyChref,"heromodel");

    CopyChref.model            = PastChref.model;
    if (CheckAttribute(PastChref,"heromodel"))
	{
		CopyChref.heromodel = PastChref.heromodel;
		CopyChref.VISUAL_CIRASS = true;
	}
    CopyChref.model.animation  = PastChref.model.animation;
	// LEO: было `CopyChref.sex = CopyChref.sex` - присваивание САМОМУ СЕБЕ, пол оригинала не копировался
	//   никогда. Клона в кают-компанию порождают как MAN (companions.c, NPC_GeneratePhantomCharacter),
	//   поэтому женщина-офицер выглядела собой (модель/голова/лицо копируются строками рядом), но
	//   числилась мужчиной - и NPCharSexPhrase, сверяющая именно sex, выдавала мужские формы:
	//   «Я хочу чтобы ты не менял свой корабль» вместо «не меняла». Касалось всех клонов: каютных
	//   капитанов, пленников, подсаженных офицеров.
    CopyChref.sex              = PastChref.sex;
    CopyChref.headModel        = PastChref.headModel;
    CopyChref.FaceId           = PastChref.FaceId;
    CopyChref.nation           = PastChref.nation;

	CopyChref.name             = PastChref.name;
    CopyChref.lastname         = PastChref.lastname;

    CopyChref.rank             = PastChref.rank;
    CopyChref.reputation       = makeint(PastChref.reputation);
	CopyChref.baseCapIdx       = PastChref.index; //Id оригинального в структуру клона
	
	if(CheckAttribute(PastChref, "ImmortalOfficer"))
	{
		CopyChref.ImmortalOfficer = PastChref.ImmortalOfficer;
		CopyChref.OfficerWantToGo.DontGo = PastChref.OfficerWantToGo.DontGo;
		CopyChref.HalfImmortal = PastChref.HalfImmortal;
	}
	else 
	{
		DeleteAttribute(CopyChref, "ImmortalOfficer");
	}
    if (CheckAttribute(PastChref, "loyality"))
    {
    	CopyChref.loyality         = PastChref.loyality;
	}
	else DeleteAttribute(CopyChref, "loyality");
	if (CheckAttribute(PastChref, "alignment"))
	{
    	CopyChref.alignment        = PastChref.alignment;
    }
    else DeleteAttribute(CopyChref, "alignment");

    CopyChref.Money            = PastChref.Money;

    CopyChref.chr_ai.hp         = makeint(PastChref.chr_ai.hp);
	// LEO: предел копируем КАК ЕСТЬ, вместе с отметкой о выданном бонусе перка. Раньше вычитали 80 в
	//   расчёте на ленивую выдачу, но она срабатывает лишь при осмотре или росте уровня - до тех пор клон
	//   ходил с заниженным пределом (у каютных капитанов: в таблице 338, в каюте 258, «чеккер лечит»).
	// ⚠️ Убрана строка `CopyChref.chr.chr_ai.hp_max = ...` - опечатка с лишним `.chr.`: она заводила
	//   мусорную ветку атрибутов, которую никто не читает, а настоящий предел ставит LAi_SetHP ниже.
	LAi_SetHP(CopyChref, makeint(PastChref.chr_ai.hp_max), makeint(PastChref.chr_ai.hp_max));
	if (CheckAttribute(PastChref, "PerkValue.HPPlus")) CopyChref.PerkValue.HPPlus = 0;
	else DeleteAttribute(CopyChref, "PerkValue.HPPlus");
	if (CheckAttribute(PastChref, "PerkValue.HPPlusFixed")) CopyChref.PerkValue.HPPlusFixed = 0;
	else DeleteAttribute(CopyChref, "PerkValue.HPPlusFixed");

	LAi_SetCurHPMax(CopyChref);
	
	if (CheckAttribute(PastChref,"chr_ai.sgun") && PastChref.chr_ai.sgun != "")
	{
		CopyChref.chr_ai.sgun = PastChref.chr_ai.sgun;
		CopyChref.chr_ai.bullettype = PastChref.chr_ai.bullettype;
		CopyChref.chr_ai.bulletnum = sti(PastChref.chr_ai.bulletnum);
		CopyChref.chr_ai.bullet = PastChref.chr_ai.bullet;
		CopyChref.chr_ai.gunpowder = PastChref.chr_ai.gunpowder;
		CopyChref.chr_ai.accuracy = stf(PastChref.chr_ai.accuracy);
		CopyChref.chr_ai.chargespeed = sti(PastChref.chr_ai.chargespeed);
		CopyChref.chr_ai.dmgmin_nc = stf(PastChref.chr_ai.dmgmin_nc);
		CopyChref.chr_ai.dmgmax_nc = stf(PastChref.chr_ai.dmgmax_nc);
		CopyChref.chr_ai.dmgmin_c = stf(PastChref.chr_ai.dmgmin_c);
		CopyChref.chr_ai.dmgmax_c = stf(PastChref.chr_ai.dmgmax_c);
		CopyChref.chr_ai.energyp_nc = stf(PastChref.chr_ai.energyp_nc);
		CopyChref.chr_ai.energyp_c = stf(PastChref.chr_ai.energyp_c);
		CopyChref.chr_ai.stun_nc = sti(PastChref.chr_ai.stun_nc);
		CopyChref.chr_ai.stun_c = sti(PastChref.chr_ai.stun_c);
		CopyChref.chr_ai.multidmg = sti(PastChref.chr_ai.multidmg);
		CopyChref.chr_ai.misfire = sti(PastChref.chr_ai.misfire);
		CopyChref.chr_ai.selfdamage = sti(PastChref.chr_ai.selfdamage);
		CopyChref.chr_ai.explosion = sti(PastChref.chr_ai.explosion);
		CopyChref.chr_ai.charge_max = sti(PastChref.chr_ai.charge_max);
		CopyChref.chr_ai.charge = stf(PastChref.chr_ai.charge);
		CopyChref.chr_ai.chargeprc = sti(PastChref.chr_ai.chargeprc);
		CopyChref.chr_ai.charge_dlt = stf(PastChref.chr_ai.charge_dlt);
		CopyChref.chr_ai.dmggunmin = stf(PastChref.chr_ai.dmggunmin);
		CopyChref.chr_ai.dmggunmax = stf(PastChref.chr_ai.dmggunmax);
	}
	//копируем структуру quest от оригинального кэпа, очень нужно по квестам :)
	if (CheckAttribute(PastChref, "quest"))
    {
	    aref arToCharQuest, arFromCharQuest;
		makearef(arFromCharQuest, PastChref.quest);
		makearef(arToCharQuest, CopyChref.quest);
		DeleteAttribute(arToCharQuest, "");
		CopyAttributes(arToCharQuest, arFromCharQuest);
	}
	if (CheckAttribute(PastChref, "quest.officertype"))
    {
        CopyChref.quest.officertype     =   PastChref.quest.officertype;
    }
    else
    {
        DeleteAttribute(CopyChref, "quest.officertype");
    }
    if (CheckAttribute(PastChref, "Payment"))
    {
        CopyChref.Payment     =   PastChref.Payment; // платить ЗП
    }
    else
    {
        DeleteAttribute(CopyChref, "Payment");
    }
    if (CheckAttribute(PastChref, "HoldEquip"))
    {
        CopyChref.HoldEquip   =   PastChref.HoldEquip; // не отдавать саблю и пистоль
    }
	else
    {
        DeleteAttribute(CopyChref, "HoldEquip");
    }
    if (CheckAttribute(PastChref, "SaveItemsForDead"))
    {
        CopyChref.SaveItemsForDead   =   true;
    }
	else
    {
        DeleteAttribute(CopyChref, "SaveItemsForDead");
    }
    if (CheckAttribute(PastChref, "DontClearDead"))
    {
        CopyChref.DontClearDead   =   true;
    }
	else
    {
        DeleteAttribute(CopyChref, "DontClearDead");
    }
    if (CheckAttribute(PastChref, "OfficerWantToGo.DontGo"))
    {
        CopyChref.OfficerWantToGo.DontGo   =   true;
    }
	else
    {
        DeleteAttribute(CopyChref, "OfficerWantToGo.DontGo");
    }
	if (CheckAttribute(PastChref, "Frenzy"))
    {
        CopyChref.Frenzy   =   true;
    }
	else
    {
        DeleteAttribute(CopyChref, "Frenzy");
    }
    // skill
    DeleteAttribute(CopyChref, "skill");
    CopyChref.skill = "";

    makearef(arToChar, CopyChref.skill);
    makearef(arFromChar, PastChref.skill);

    CopyAttributes(arToChar,arFromChar);

    // SPECIAL
    DeleteAttribute(CopyChref, "SPECIAL");
    CopyChref.SPECIAL = "";

    makearef(arToChar, CopyChref.SPECIAL);
    makearef(arFromChar, PastChref.SPECIAL);

    CopyAttributes(arToChar,arFromChar);

    // Statistic
    DeleteAttribute(CopyChref, "Statistic");
    CopyChref.Statistic = "";

    makearef(arToChar, CopyChref.Statistic);
    makearef(arFromChar, PastChref.Statistic);

    CopyAttributes(arToChar,arFromChar);

    // perks
    DeleteAttribute(CopyChref, "perks");
    CopyChref.perks = "";

    makearef(arToChar, CopyChref.perks);
    makearef(arFromChar, PastChref.perks);

    CopyAttributes(arToChar,arFromChar);
    // items
    DeleteAttribute(CopyChref, "Items");
    CopyChref.Items = "";

    makearef(arToChar, CopyChref.Items);
    makearef(arFromChar, PastChref.Items);

    CopyAttributes(arToChar,arFromChar);

	makearef(arToChar, CopyChref.equip);
    makearef(arFromChar, PastChref.equip);
    CopyAttributes(arToChar,arFromChar);
	if (CheckAttribute(PastChref, "curammo"))
    {
		makearef(arToChar, CopyChref.curammo);
		makearef(arFromChar, PastChref.curammo);
		CopyAttributes(arToChar,arFromChar);
	}
	// LEO: вместе с curammo - память боеприпаса по стволам (ammostage.<ствол>, см. LAi_character.c).
	//   Сначала стираем: CopyAttributes сливает ветки, и старая память получателя осталась бы висеть.
	DeleteAttribute(CopyChref, "ammostage");
	if (CheckAttribute(PastChref, "ammostage"))
	{
		makearef(arToChar, CopyChref.ammostage);
		makearef(arFromChar, PastChref.ammostage);
		CopyAttributes(arToChar,arFromChar);
	}
	if (CheckAttribute(PastChref, "cirassId"))
	{
		if (CheckAttribute(PastChref,"equip.cirass")) CopyChref.cirassId = PastChref.cirassId;
		else
		{
			if (CheckAttribute(CopyChref,"heromodel")) CopyChref.model = GetSubStringByNum(CopyChref.HeroModel,0);
		}
	}
	// health
	if (CheckAttribute(PastChref, "Health.TotalDamg"))
    {
		CopyChref.Health.TotalDamg =   PastChref.Health.TotalDamg;
	}
	else
	{
        CopyChref.Health.TotalDamg = 0;
	}
	// ugeen --> нужно для генерации различных ситуации в каюте абордированного кэпа
	if (CheckAttribute(PastChref,"Situation")) // если в каюте кэпа возникла ситуация
	{
		CopyChref.Situation = PastChref.Situation;
		CopyChref.Situation.type = PastChref.Situation.type;
	}

	if (CheckAttribute(PastChref,"EncType"))
	{
		CopyChref.EncType = PastChref.EncType;
	}

	if (CheckAttribute(PastChref,"RealEncounterType"))
	{
		CopyChref.RealEncounterType = PastChref.RealEncounterType;
	}
	if (CheckAttribute(PastChref,"Ship"))
	{
		makearef(arToChar, CopyChref.Back.Ship);
		makearef(arFromChar, PastChref.Ship);
		CopyAttributes(arToChar, arFromChar);
	}
	// <-- ugeen
	if (CheckAttribute(PastChref,"TransferGoods"))
	{
		makearef(arToChar, CopyChref.TransferGoods);
		makearef(arFromChar, PastChref.TransferGoods);
		CopyAttributes(arToChar, arFromChar);
	}
	if (CheckAttribute(PastChref,"TransferItems"))
	{
		makearef(arToChar, CopyChref.TransferItems);
		makearef(arFromChar, PastChref.TransferItems);
		CopyAttributes(arToChar, arFromChar);
	}
	if (CheckAttribute(PastChref,"selectedSet"))
	{
		makearef(arToChar, CopyChref.selectedSet);
		makearef(arFromChar, PastChref.selectedSet);
		CopyAttributes(arToChar, arFromChar);
		for (int i = 1; i < 7; i++)
		{
			if (CheckAttribute(PastChref,"Set"+i))
			{
				string SetNum = "Set"+i;
				makearef(arToChar, CopyChref.(SetNum));
				makearef(arFromChar, PastChref.(SetNum));
				CopyAttributes(arToChar, arFromChar);
			}
		}
	}
	if (_dialogCopy && CheckAttribute(PastChref, "Dialog.Filename"))
	{
	    CopyChref.Dialog.Filename    = PastChref.Dialog.Filename;
	    CopyChref.Dialog.CurrentNode = PastChref.Dialog.CurrentNode;
	    if (CheckAttribute(PastChref, "greeting")) CopyChref.greeting = PastChref.greeting; //eddy.нет логам!
		else
		{
			DeleteAttribute(CopyChref, "greeting");
		}
	}
	SetEnergyToCharacter(CopyChref);
}

int GetCharacterLoyality(ref chr)
{
    if (CheckAttribute(chr, "loyality"))
    {
        if (sti(chr.loyality) > MAX_LOYALITY)
        {
            chr.loyality = MAX_LOYALITY;
        }
        if (sti(chr.loyality) < 0)
        {
            chr.loyality = 0;
        }
        return sti(chr.loyality);
    }
    else
    {
        return  0; // для пленных и гг
    }
}

// boal 091004 много героев  -->
// boal 23.01.2004 выбор персонажа-->
void setNewMainCharacter(ref ch, int num)
{
	string heroModel, heroFaceId;
	int    idLngFile = -1;
    string totalInfo;

    idLngFile = LanguageOpenFile("HeroDescribe.txt");

    totalInfo = LanguageConvertString(idLngFile, "heroFace_" + num);
    heroFaceId  = sti(totalInfo);
    ch.FaceId   = heroFaceId;

    totalInfo = LanguageConvertString(idLngFile, "heroModel_" + num);
    ch.HeroModel = totalInfo;
    ch.model	= GetSubStringByNum(ch.HeroModel, 0);
    ch.headModel = "h_" + ch.model;

    totalInfo = LanguageConvertString(idLngFile, "heroName_" + num);
    ch.name 	=  totalInfo;

    totalInfo = LanguageConvertString(idLngFile, "heroLastname_" + num);
    ch.lastname = totalInfo;

    totalInfo = LanguageConvertString(idLngFile, "sex_" + num);
    ch.sex = totalInfo;

    totalInfo = LanguageConvertString(idLngFile, "animation_" + num);
    ch.model.animation = totalInfo;

    if(ch.sex == "woman")
	{
		ch.model.height = 1.75;
	}
    else
    {
		ch.model.height = 1.8;
	}
    LanguageCloseFile(idLngFile);

    // для НПС
    ch.HeroParam.HeroType = GetNewMainCharacterType(num);
    ch.nation             = GetNewMainCharacterNation(num);

}

void SetQuestsData()
{
	// 1 ранг ==>
	// LEO: альтернативный пролог "Проклятая судьба" - с него начинается вся линейка К:ПС
	pchar.questdata.StartDamnedDestiny = 0;
	pchar.questdata.KoltsoLodet = 0;
	pchar.questdata.ProklyatayaJara = 0;
	pchar.questdata.NaitiTorgovtsa = 0;
	pchar.questdata.DengiRost = 0;
	pchar.questdata.SpastiMuja = 0;
	pchar.questdata.Aptekar = 0;
	pchar.questdata.NaitiMushket = 0;
	pchar.questdata.ClanLambrini = 0;
	pchar.questdata.Rimalie = 0;
	pchar.questdata.Ascold = 0;
	pchar.questdata.Rabotorgovets = 0;
	pchar.questdata.ShamankaChani = 0;
	pchar.questdata.PomoshElen = 0;
	pchar.questdata.TravlyaKrys = 0;
	pchar.questdata.Looser = 0;
	pchar.questdata.StroitelstvoKolonii = 0;
	pchar.questdata.HelpSacredInquisition = 0;	// Gregg: перенос из К:ПС
	pchar.questdata.MoonStone = 0;				// Gregg: продолжение инквизиции, своего порога ранга нет
	// <== 1 ранг

	// 3 ранг ==>
	pchar.questdata.ProklyatyIdol = 0;
	pchar.questdata.NeputyovyKaznachey = 0;
	pchar.questdata.Dozor = 0;
	pchar.questdata.Ostin = 0;
	pchar.questdata.UneasyDelivery = 0;			// Gregg: перенос из К:ПС
	// <== 3 ранг

	// 5 ранг ==>
	pchar.questdata.SpasenieNaRifah = 0;
	pchar.questdata.SinyaPtitsa = 0;
	pchar.questdata.KotVMeshke = 0;
	pchar.questdata.Kuali = 0;
	pchar.questdata.FleitOrion = 0;
	pchar.questdata.TohmasPetit = 0;
	pchar.questdata.TreacheryOfGrifoSajfus = 0;	// Gregg: перенос из К:ПС
	// <== 5 ранг

	// 10 ранг ==>
	pchar.questdata.OskolokProshlogo = 0;
	pchar.questdata.Nasledstvo = 0;
	pchar.questdata.Kondotier = 0;
	pchar.questdata.KorablPrizrak = 0;
	pchar.questdata.Satanisty = 0;
	pchar.questdata.ZacharovanyGorod = 0;
	pchar.questdata.DisappearPeople = 0;		// Gregg: перенос из К:ПС
	// <== 10 ранг

	// LEO: 12 ранг.
	pchar.questdata.OpasnyGruz = 0;

	// 13 ранг ==>
	pchar.questdata.CaptainMonkey = 0;			// Gregg: перенос из К:ПС
	// <== 13 ранг

	// 15 ранг ==>
	pchar.questdata.OhotaNaVedmu = 0;
	pchar.questdata.ZagadochnyOstrov = 0;
	pchar.questdata.UbitVsehNishih = 0;
	pchar.questdata.GPK = 0;
	// <== 15 ранг

	// 17 ранг ==>
	// LEO: линейка К:ПС идёт в энциклопедии ПО ГЛАВАМ - у каждой свои требования и своя иконка.
	//   Отметка о выполнении ставится в финале каждой главы, а не всей цепочки.
	pchar.questdata.MysteriousArtefact = 0;	// "Чёрная Орхидея", часть 1
	pchar.questdata.DestinyGift = 0;		// "Чёрная Орхидея", часть 2
	pchar.questdata.SpanishGenerosity = 0;	// "Чёрная Орхидея", часть 3
	pchar.questdata.SmallMongrel = 0;		// "Загадки Карибов", часть 1
	pchar.questdata.RaulEsperanto = 0;		// "Загадки Карибов", часть 2
	pchar.questdata.FindAndDestroy = 0;		// "Загадки Карибов", часть 3
	pchar.questdata.TwoSisters = 0;
	// <== 17 ранг

	// 20 ранг ==>
	pchar.questdata.ChernayaBoroda = 0;
	pchar.questdata.Salazar = 0;
	pchar.questdata.IstoriyaIzabely = 0;
	// <== 20 ранг

	// 25 ранг ==>
	pchar.questdata.ChernyeParusa = 0;
	// <== 25 ранг

	// ?? ранг ==>
	pchar.questdata.Teno = 0;
	pchar.questdata.IstoriaDavneyDrujby = 0;
	pchar.questdata.NovayaRodina = 0;
	pchar.questdata.SilverFleet = 0;
	// <== ?? ранг
}

void SetAllAchievements(int level)
{
	if (level == 0)
	{
		pchar.Statistic.Solder_g = 0
		pchar.Statistic.Solder_s = 0
		pchar.Statistic.Citizen_g = 0
		pchar.Statistic.Citizen_s = 0
		pchar.Statistic.Monster_g = 0
		pchar.Statistic.Monster_s = 0
		pchar.Statistic.Warrior_g = 0
		pchar.Statistic.Warrior_s = 0
		for(int i=1; i<8; i++)
		{
			string KS = "KillShip_"+i;
			string AS = "abordship_"+i;
			string KAS = "killabordship_"+i;
			pchar.Statistic.(KS) = 0;
			pchar.Statistic.(AS) = 0;
			pchar.Statistic.(KAS) = 0;
		}

		pchar.questTemp.depositcount = 0;
		pchar.questTemp.taxescount = 0;
		pchar.questTemp.bribescount = 0;
		pchar.questTemp.treasurecount = 0;
		pchar.questTemp.craftcount = 0;
		pchar.questTemp.stonescount = 0;
		pchar.questTemp.bookcount = 0;
		pchar.questTemp.poisoncount = 0;
		pchar.questTemp.criticalcount = 0;
		pchar.questTemp.bloodingcount = 0;
		pchar.questTemp.swiftcount = 0;
		pchar.questTemp.stuncount = 0;
		pchar.questTemp.traumacount = 0;
		pchar.questTemp.healcount = 0;
		pchar.questTemp.genquestcount = 0;
		pchar.questTemp.officercount = 0;
		pchar.questTemp.chestcount = 0;
		pchar.questTemp.colonystate = 0;
		pchar.questTemp.foodcount = 0;
		pchar.questTemp.duelcount = 0;
		pchar.questTemp.tournamentcount = 0;
		pchar.questTemp.shipordercount = 0;
		pchar.questTemp.shipsearchcount = 0;
		pchar.questTemp.GoldFleetNum = 0;
		pchar.questTemp.prisonshootingwins = 0;
	}
	pchar.achievements.rank = level; // Достижение за ранг 175

	pchar.achievements.money = level; // Достижение за общее кол-во денег 175
	pchar.achievements.bank_money = level; // Общее сумма вкладов в банк 100

	if(bPortPermission) pchar.achievements.taxes = level;	// Кол-во оплаченных портовых сборов 175 ---
	if(bBribeSoldiers) pchar.achievements.AchVzyatka = level; // Кол-во выданных взяток  175 ---

	pchar.achievements.AchTreasure = level; // Достижение за клады 175
	pchar.achievements.AchChest = level; // Достижение за сундуки 175

	pchar.achievements.kills = level; // Кол-во убийств 175
	pchar.achievements.ship_kills = level; // Кол-во потопленных кораблей 175
	pchar.achievements.skel_kills = level; // Кол-во убийств нежити 175
	pchar.achievements.poisons = level; // Кол-во отравленных врагов 175
	pchar.achievements.criticals = level; // Кол-во критических ударов 175
	pchar.achievements.AchBlood = level; // Достижение за кровотечения 175
	pchar.achievements.AchTravma = level; // Достижение за травму 175
	pchar.achievements.AchStanS = level; // Достижение за резкий 175
	pchar.achievements.AchStanH = level; // Достижение за станы 175

	pchar.achievements.craft = level; // Кол-во созданных предметов 175
	pchar.achievements.books = level; // Кол-во прочитанных книг 175

	pchar.achievements.officers = level; // Кол-во нанятых офицеров 175
	pchar.achievements.ships = level; // Кол-во кораблей в эскадре 175

	pchar.achievements.heal_bottles = level; // Кол-во выпитых зелий 175
	pchar.achievements.AchFood = level; // Достижение за еду 175

	pchar.achievements.AchTurnir = level; // Турниры 175
	pchar.achievements.AchDuelyant = level; // Дуэлянт 175
	pchar.achievements.AchShipOrder = level; // Заказы коров :) 175
	pchar.achievements.AchSityRobbery = level; //Грабеж городов 175
	pchar.achievements.AchGoldFleet = level; //Потопление ЗФ 175
	if(bFillEncyShips) pchar.achievements.AchShipSearch = level; // Изучить все кораблики 100
	pchar.achievements.AchBuildColony = level; // Достижение за колонию 175
	pchar.achievements.gen_quests = level; // Общее кол-во выполненных генераторных квестов 175

	pchar.achievements.AchOrion = level; // Чокопай 100 ---
	pchar.achievements.AchRabotorg = level; // Торгораб 100 ---
	pchar.achievements.AchKondotier = level; // Шишкоёб 100 ---
	pchar.achievements.AchDozor = level; // Дозорный 100
	pchar.achievements.AchTich = level; // Чернобород 100 ---
	pchar.achievements.AchRagnar = level; // Суровый викинг 100 ---
	pchar.achievements.AchSalazar = level; // Тухлый испанец 100 ---
	pchar.achievements.AchKaskos = level; // Перетухший испанец 100 ---
	pchar.achievements.AchUmSamil = level; // Забытый клон Миклухи-ягуара 100 ---
	pchar.achievements.AchZagadochniyOstrov = level; // "Загадочный" остров 100 ---
	pchar.achievements.AchNasledstvo = level; // "Не"жданное наследство 100 ---
	pchar.achievements.AchStrannyeDela = level; // Творятся на архипелаге 100 ---
	pchar.achievements.AchMapMaker = level; // Атлас собран 100
	pchar.achievements.LSC_quest = level; // Выполнение квеста "ГПК" 100
	pchar.achievements.Teno_quest = level; // Выполнение квеста "Теночтитлан" 100
	pchar.achievements.Ghostship_quest = level; // Выполнение квеста "Корабль-призрак" 100
	pchar.achievements.Enchantcity_quest = level; // Выполнение квеста "Зачарованный город" 100

	// LEO: квесты, перенесённые из К:ПС
	pchar.achievements.Inquisition_quest = level; // Выполнение квеста "Помощь Святой Инквизиции" 100
	pchar.achievements.MoonStone_quest = level; // Выполнение квеста "Легенда о призрачном свете" 100
	pchar.achievements.UneasyDelivery_quest = level; // Выполнение квеста "Товар для Ричарда Тилльманна" 100
	pchar.achievements.GrifoSajfus_quest = level; // Выполнение квеста "Предательство Грифо Сайфуса" 100
	pchar.achievements.DisappearPeople_quest = level; // Выполнение квеста "Исчезновение людей" 100
	pchar.achievements.CaptainMonkey_quest = level; // Выполнение квеста "Корабль обезьян" 100
	pchar.achievements.BlackOrchid_quest = level; // Выполнение линейки "Чёрная Орхидея" 100
	pchar.achievements.RiddlesCaribbean_quest = level; // Выполнение линейки "Загадки Карибов" 100
	pchar.achievements.TwoSisters_quest = level; // Выполнение квеста "Две сестры" 100
	pchar.achievements.AchThrowingChampion = level;
	pchar.achievements.AchPrisonShooting = level;

	pchar.achievements.Killbeggars_quest = level; // Выполнение квеста "Убить всех нищих" 100
	pchar.achievements.Bluebird_quest = level; // Выполнение квеста "Уничтожить шебеку СП" 100
	pchar.achievements.Berglarsgang_quest = level; // Выполнение квеста "Банда грабителей и убийц Остина" 100
	pchar.achievements.Mummydust_quest = level; // Выполнение квеста "Поиски порошка мумии" 100
	if(pchar.sex != "woman") pchar.achievements.Isabella_quest = level; // Выполнение квеста "История прекрасной Изабеллы" 100
	if(CheckAttribute(Pchar, "questTemp.CapBloodLine") && Pchar.questTemp.CapBloodLine == 1) pchar.achievements.CapBladLine = level; // Выполнение линейки Блада 100
	if(startherotype == 2) pchar.achievements.WhisperLine = level; // Выполнение линейки Виспер 100
	pchar.achievements.Nation_quest_E = level; // Выполнение национальной линейки квестов 100 ---
	pchar.achievements.Nation_quest_F = level; // Выполнение национальной линейки квестов 100 ---
	pchar.achievements.Nation_quest_H = level; // Выполнение национальной линейки квестов 100 ---
	pchar.achievements.Nation_quest_S = level; // Выполнение национальной линейки квестов 100 ---
	pchar.achievements.Nation_quest_P = level; // Выполнение национальной линейки квестов 100 ---

	// Всего очков доступных для получения: 7700 (по 100-175 на каждое достижение) - мне лень пересчитывать (Калькулятор запили, Грегг, блеать! (c) LEOPARD :) )
	// Гарантированно можно получить 6900 очков достижений, если исключать 4 линейки наций, линейки за персонажей и опционалки
	// Всего достижений: 48
	// При пересчёте возможных к получению в 1 партии был максимум в... 5150
}

void AddHealingPots(ref chref, bool strong)
{
	switch (MOD_SKILL_ENEMY_RATE)
	{
		case 1:
			if (strong) TakeNItems(chref,"potion1",3+rand(2));
			else TakeNItems(chref,"potion1",1+rand(1));
		break;
		case 2:
			if (strong) TakeNItems(chref,"potion2",2+rand(2));
			else TakeNItems(chref,"potion1",3+rand(2));
		break;
		case 3:
			if (strong) TakeNItems(chref,"potion2",3+rand(2));
			else TakeNItems(chref,"potion2",1+rand(1));
		break;
	}
}

void CreateOreModels()
{
	int ore1 = 0;
	int ore2 = 0;
	int ore3 = 0;
	int ore4 = 0;

	string locid;

	for(int i=0; i<nLocationsNum; i++)
	{
		if(locations[i].filespath.models == "locations\inside\DungeonDuffer1")
		{
			locid = locations[i].id;
			pchar.GenQuestRandItem.(locid).stay = true;
			pchar.GenQuestRandItem.(locid).randitem1 = "ore_mining1";
			pchar.GenQuestRandItem.(locid).randitem2 = "ore_mining3";

			ore1++;
			ore3++;
		}

		if(locations[i].filespath.models == "locations\inside\DungeonDuffer2")
		{
			locid = locations[i].id;
			pchar.GenQuestRandItem.(locid).stay = true;
			pchar.GenQuestRandItem.(locid).randitem1 = "ore_mining1";
			pchar.GenQuestRandItem.(locid).randitem2 = "ore_mining2";
			pchar.GenQuestRandItem.(locid).randitem3 = "ore_mining3";
			pchar.GenQuestRandItem.(locid).randitem4 = "ore_mining4";

			ore1++;
			ore2++;
			ore3++;
			ore4++;
		}

		if(locations[i].filespath.models == "locations\inside\grotto1")
		{
			locid = locations[i].id;
			pchar.GenQuestRandItem.(locid).stay = true;
			pchar.GenQuestRandItem.(locid).randitem1 = "ore_mining1";

			ore1++;
		}

		if(locations[i].filespath.models == "locations\inside\grotto2")
		{
			locid = locations[i].id;
			pchar.GenQuestRandItem.(locid).stay = true;
			pchar.GenQuestRandItem.(locid).randitem1 = "ore_mining2";

			ore2++;
		}
	}

	if (LanguageGetLanguage() != "russian")
	{
		Log_TestInfo("Total locations checked: " + nLocationsNum);
		Log_TestInfo("Iron veins created: " + ore1);
		Log_TestInfo("Silver veins created: " + ore2);
		Log_TestInfo("Gold veins created: " + ore3);
		Log_TestInfo("Coal veins created: " + ore4);
	}
	else
	{
		Log_TestInfo("Локаций проверено всего: " + nLocationsNum);
		Log_TestInfo("Железных жил создано: " + ore1);
		Log_TestInfo("Серебряных жил создано: " + ore2);
		Log_TestInfo("Золотых жил создано: " + ore3);
		Log_TestInfo("Угольных жил создано: " + ore4);
	}
}
void initNewMainCharacter()
{
	ref ch = GetMainCharacter();
	string sTemp;
	int    iTmp, i;
	pchar.ach_points = 0; // Устанавливаем общее кол-во очков достижений на 0

    setNewMainCharacter(ch, startHeroType);
    // контроль версий -->
    ch.VersionNumber              = GetVerNum();
    ch.VersionNumberCompatibility = VERSION_NUM;
    // контроль версий <--
    ch.nation     = NullCharacter.HeroParam.nation;
    ch.BaseNation = NullCharacter.HeroParam.nation;
    if (MOD_EXP_RATE < 10) MOD_EXP_RATE = 10; // иначе будет развал целостности данных, порог релиховой версии бля всех сложностей.
    // куда плывем
	if (sti(ch.nation) != PIRATE)
	{
		ch.HeroParam.ToCityId = FindAlliedColonyForNation(sti(ch.nation), true);
	}
	else
	{
		ch.HeroParam.ToCityId = FindAlliedColonyForNationExceptColony("Pirates"); //Бермуды не берем, туда ещё добираться надо
	}
	// откуда плывем
	ch.HeroParam.FromCityId  = Colonies[FindNonEnemyColonyForAdventure(sti(ch.nation), ch.HeroParam.ToCityId, true)].id;

	// где появимся
	ch.HeroParam.Location = ch.HeroParam.ToCityId + "_town";
	ch.HeroParam.Group    = "reload";
	ch.HeroParam.Locator  = "reload1";
	if (ch.HeroParam.ToCityId == "PuertoPrincipe" || ch.HeroParam.ToCityId == "FortOrange" || ch.HeroParam.ToCityId == "DesMoines")
	{
		ch.HeroParam.Location = ch.HeroParam.ToCityId + "_port";
		ch.HeroParam.Group    = "reload";
		ch.HeroParam.Locator  = "reload1";
	}
	
	if(startherotype == 2)
	{
		ch.HeroParam.Location = "PuertoPrincipe_port";
	}
	if(startherotype == 4 || startherotype == 5)
	{
		ch.HeroParam.Location = "Shore57";
		ch.HeroParam.Group    = "reload";
		ch.HeroParam.Locator  = "boat";
	}
	setCharacterShipLocation(ch, ch.HeroParam.Location);
    setWDMPointXZ(ch.HeroParam.Location);  // коорд на карте
	SetTimerCondition("Move_Govenour", 0, 0, 25 + rand(10), true); // to_do перенести в один метод инициации
	//Запускаем проверку на падение здоровья раз в 5 дней
	SetTimerCondition("CheckMaxHealthQuest", 0, 0, 5, true);
    SetTimerCondition("Nation_Legend", 0, 0, 25 + rand(10), true);

	///  рассадим губернаторов
	SetGovenourToCity(ENGLAND, "PortRoyal");
    SetGovenourToCity(SPAIN, "Havana");
    SetGovenourToCity(FRANCE, "Tortuga");
    SetGovenourToCity(HOLLAND, "Villemstad");

    AddQuestRecordInfo("Tutorial_Dictonary", "1");
	AddQuestRecordInfo("Tutorial_Eng", "1");
	AddQuestRecordInfo("Tutorial_Fra", "1");
	AddQuestRecordInfo("Tutorial_Spa", "1");
	AddQuestRecordInfo("Tutorial_Hol", "1");
	AddQuestRecordInfo("Tenochtitlan_info", "9");
	AddQuestRecordInfo("Tenochtitlan_info", "8");
	AddQuestRecordInfo("Tenochtitlan_info", "7");
	AddQuestRecordInfo("Tenochtitlan_info", "6");
	AddQuestRecordInfo("Tenochtitlan_info", "5");
	AddQuestRecordInfo("Tenochtitlan_info", "4");
	AddQuestRecordInfo("Tenochtitlan_info", "3");
	AddQuestRecordInfo("Tenochtitlan_info", "2");
	AddQuestRecordInfo("Tenochtitlan_info", "1");
	AddQuestRecordInfo("Tutorial_Sharp", "1");
	AddQuestRecordInfo("Tutorial_Fight_Info", "1");
	AddQuestRecordInfo("Tutorial_ChangeLog_CSP", "1");

	ReloadProgressUpdate();
	pchar.questTemp.Achievements.Points = "0" // Очки достижений на ноль

 	SetNationRelations();
 	// от кого драпаем
 	ch.HeroParam.EnemyNation  = FindEnemyNation2Nation(sti(ch.nation));
    // boal вешаем прерывание на охотников навечно (для моря и земли) -->
    SetTimerCondition("SeaHunterCheck", 0, 0, 6, true);
    SaveCurrentQuestDateParam("Land_HunterTimerEng");
    SaveCurrentQuestDateParam("Land_HunterTimerFra");
    SaveCurrentQuestDateParam("Land_HunterTimerSpa");
    SaveCurrentQuestDateParam("Land_HunterTimerHol");
    // boal вешаем прерывание на охотников навечно (для моря и земли) <--
    LAi_SetHP(ch, LAI_DEFAULT_HP, LAI_DEFAULT_HP);

    ch.HeroParam.HeroType = NullCharacter.HeroParam.HeroType;
	ch.skill.freeskill   = 0;
	ch.Skill.FreeSPECIAL = 0;
	sGlobalTemp   = "Flag" + NationShortName(sti(pchar.nation));
	SetCharacterPerk(pchar, sGlobalTemp); // перк флага нации
	if (sti(ch.nation) != PIRATE || startHeroType == 1) SetCharacterPerk(pchar, "FlagPir");
	SetCharacterPerk(pchar, "Energaiser"); // скрытый перк даёт 1.5 к приросту энергии, дается ГГ и боссам уровней
	// начальные скилы задать
    InitStartParam(pchar);
    SetEnergyToCharacter(pchar);
    initMainCharacterItem();
    DeleteAttribute(pchar, "Ship");
	// LEO: обе «безкорабельные» ветки отменяют выдачу корабля. Раньше else висел только на
	//   альт-прологе, и опция «Старт без корабля» получала корабль поверх SHIP_NOTUSED.
	bool bNoStartShip = false;
	if (startHeroType > 8)
	{
		if (sti(NullCharacter.Start.NoStartShip) == 1) bNoStartShip = true;
		if (sti(NullCharacter.Start.DDPrologue) == 1)  bNoStartShip = true;
	}
	if (bNoStartShip) pchar.Ship.Type = SHIP_NOTUSED;
	// Gregg: альтернативный пролог - ГГ выбрасывает на берег после расстрела его шебеки.
	// Раздеваем не только ради сюжета («наши карманы пусты», говорит боцман): этап со схроном
	// проверяет КОЛИЧЕСТВО вещей, и стартовый комплект (зелья, еда, клинок, деньги) закрывал бы
	// условие ещё до открытия сундука. Образец раздевания - angelika_tich_line_functions.c.
	if (sti(NullCharacter.Start.DDPrologue) == 1 && startHeroType > 8)
	{
		RemoveAllCharacterItems(pchar, true);
		DeleteAttribute(pchar, "items");
		pchar.money = 0;
		// Gregg: одежду возвращаем - ГГ выбросило на берег, а не раздели догола.
		// Обноски единственный стартовый костюм (тайный агент, initMainCharacterItem)
		// и несут профильный для него бонус +20 к скрытности - см.
		// SetCharacterSkillBySuit. Этап со схроном от этого не страдает: его условие
		// победы перечисляет конкретные предметы, suit_1 среди них нет.
		// Не надеваем, как и на обычном старте, - игрок делает это сам.
		if (ch.HeroParam.HeroType == "Agent") TakeNItems(pchar, "suit_1", 1);
	}
	if (!bNoStartShip)   // корабль выдаётся, только если старт с ним
	{
		if (sti(ch.nation) != PIRATE)
		{
			pchar.Ship.Type = GenerateShipExt((SHIP_KETCH + rand(11)), 0, pchar);
			SetBaseShipData(pchar);
			SetRandomNameToShip(pchar);
			SetCrewQuantityFull(pchar);
			ChangeCrewExp(pchar, "Sailors", 50);
			ChangeCrewExp(pchar, "Cannoners", 50);
			ChangeCrewExp(pchar, "Soldiers", 50);
			SetCharacterGoods(pchar, GOOD_SAILCLOTH, 20 + rand(50));
			SetCharacterGoods(pchar, GOOD_PLANKS, 20 + rand(50));
			SetCharacterGoods(pchar, GOOD_RUM, 50 + rand(100));
		}
		else
		{
			pchar.Ship.Type = GenerateShipExt((SHIP_KETCH + rand(11)), 0, pchar);
			SetBaseShipData(pchar);
			SetRandomNameToShip(pchar);
			SetCrewQuantityFull(pchar);
			ChangeCrewExp(pchar, "Sailors", 50);
			ChangeCrewExp(pchar, "Cannoners", 50);
			ChangeCrewExp(pchar, "Soldiers", 50);
			SetCharacterGoods(pchar, GOOD_SLAVES, 10 + rand(20));
			SetCharacterGoods(pchar, GOOD_RUM, 50 + rand(100));
		}
	// коцаем корабль
	pchar.ship.HP = sti(pchar.ship.HP) - makeint(sti(pchar.ship.HP)*0.05) * (MOD_SKILL_ENEMY_RATE*3);
	SetCrewQuantity(pchar, GetMinCrewQuantity(pchar));
	}

	// Warship Для разных квестов
	// Важно: функция MaryCelesteInit() должна быть тут, а не в initStartState2Character()
	// т.к. в ней идёт выборка колоний, которые в функции initStartState2Character() ещё не инитились
	OtherQuestCharactersInit(); // Инициализация прочих квестов

	MaryCelesteInit(); // Warship 07.07.09 Пасхалка "Мэри Селест"

	// Установим начальный дневной рандом
	PChar.DayRandom = Random();
	SetAllAchievements(0); // Обнуляем все достижения
	SetQuestsData();

	// Устанавливаеи функцию расположения кораблей при выходе в море.
	PChar.GroupShipPos_event = "CalculateGroupShipPos";
}

void initMainCharacterItem()
{
    ref ch = GetMainCharacter();
	String itemID;
	switch (ch.HeroParam.HeroType)
	{
        case "Grunt":	// Рубака
			SetCharacterPerk(pchar, "Grunt");
            TakenItems(Pchar, "potion1", rand(10));
            TakenItems(Pchar, "potion2", rand(5));
            TakenItems(Pchar, "indian12", 1);
            TakenItems(Pchar, "Food1", rand(6)+4);
			switch (sti(ch.nation))
			{
				case ENGLAND:
					itemID = GetGeneratedItem("blade10");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
					GiveItem2Character(Pchar, "pistol2");
					EquipCharacterbyItem(Pchar, "Pistol2");
					TakeNItems(Pchar, "bullet", 5);
					AddItems(Pchar, "GunPowder", 5);
					GiveItem2Character(Pchar, "Spyglass1");
					EquipCharacterbyItem(Pchar, "Spyglass1");
					pchar.money = 1500/MOD_SKILL_ENEMY_RATE*3;
				break;
				case FRANCE:
					itemID = GetGeneratedItem("blade10");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
					pchar.money = 500/MOD_SKILL_ENEMY_RATE*3;
				break;
				case HOLLAND:
					itemID = GetGeneratedItem("blade10");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
					itemID = GetGeneratedItem("pistol" +  (rand(2)+1));
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
					GiveGunAmmunitionPchar(Pchar,itemID,5);
					GiveItem2Character(Pchar, "Spyglass1");
					EquipCharacterbyItem(Pchar, "Spyglass1");
					pchar.money = 1000/MOD_SKILL_ENEMY_RATE*3;
				break;
				case PIRATE:
					itemID = GetGeneratedItem("blade10");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
					itemID = GetGeneratedItem("pistol" + (rand(2)+1));
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
					GiveGunAmmunitionPchar(Pchar,itemID,5);
				break;
				case SPAIN:
					itemID = GetGeneratedItem("blade10");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
					pchar.money = 500/MOD_SKILL_ENEMY_RATE*3;
				break;
			}
	    break;

        case "Trader":	// Торговец
			SetCharacterPerk(pchar, "Trader");
			itemID = GetGeneratedItem("blade5");
            GiveItem2Character(Pchar, itemID);
            EquipCharacterbyItem(Pchar, itemID);
			GiveItem2Character(Pchar, "Ship_Print_6");
			TakenItems(Pchar, "potion1", rand(10));
			TakenItems(Pchar, "Food1", (rand(6)+4));
			switch (sti(ch.nation))
			{
				case ENGLAND:
					GiveItem2Character(Pchar, "Map_bad");
					TakeNItems(Pchar, "indian3", 1);
					pchar.money = 15000/MOD_SKILL_ENEMY_RATE*3;
					GiveUniLicence(180); // универсальная торговая лицензия ГВИК (вместо набора понационных)
				break;
				case FRANCE:
					TakenItems(Pchar, "potion1", 2);
					TakeNItems(Pchar, "indian3", 1);
					GiveItem2Character(Pchar, "Spyglass1");
					EquipCharacterbyItem(Pchar, "Spyglass1");
					pchar.money = 10000/MOD_SKILL_ENEMY_RATE*3;
					GiveUniLicence(180); // универсальная торговая лицензия ГВИК
				break;
				case HOLLAND:
					TakeNItems(Pchar, "indian3", 1);
					GiveItem2Character(Pchar, "Spyglass3");
					EquipCharacterbyItem(Pchar, "Spyglass3");
					pchar.money = 11000/MOD_SKILL_ENEMY_RATE*3;
					GiveUniLicence(180); // универсальная торговая лицензия ГВИК
				break;
				case PIRATE:
					GiveItem2Character(Pchar, "Map_Normal");
					GiveItem2Character(Pchar, "pistol2");
					EquipCharacterbyItem(Pchar, "Pistol2");
					TakeNItems(Pchar, "bullet", 10);
					AddItems(Pchar, "GunPowder", 10);
					GiveItem2Character(Pchar, "Spyglass1");
					EquipCharacterbyItem(Pchar, "Spyglass1");
					pchar.money = 5000/MOD_SKILL_ENEMY_RATE*3;
					GiveUniLicence(180); // универсальная торговая лицензия ГВИК (вместо набора понационных)
				break;
				case SPAIN:
					TakeNItems(Pchar, "indian14", 1);
					TakeNItems(Pchar, "jewelry8", 1);
					TakeNItems(Pchar, "jewelry9", 1);
					pchar.money = 8000/MOD_SKILL_ENEMY_RATE*3;
					GiveUniLicence(180); // универсальная торговая лицензия ГВИК (вместо набора понационных)
				break;
			}
	    break;

	    case "Fencer":	// Фехтовальщик
			SetCharacterPerk(pchar, "Fencer");
			itemID = GetGeneratedItem("Spyglass" + (rand(2)+1));
			GiveItem2Character(Pchar, itemID);
			EquipCharacterbyItem(Pchar, itemID);
			itemID = GetGeneratedItem("blade2");
			GiveItem2Character(Pchar, itemID);
			EquipCharacterbyItem(Pchar, itemID);
			TakenItems(Pchar, "potion1", rand(10));
            TakenItems(Pchar, "Food1", rand(6)+4);
			itemID = GetGeneratedItem("pistol" + (rand(2)+1));
			GiveItem2Character(Pchar, itemID);
			EquipCharacterbyItem(Pchar, itemID);
			GiveGunAmmunitionPchar(Pchar,itemID,10);
			switch (sti(ch.nation))
			{
				case ENGLAND:
					pchar.money = 500/MOD_SKILL_ENEMY_RATE*3;
				break;
				case FRANCE:
					GiveGunAmmunitionPchar(Pchar,itemID,10);
					pchar.money = 1000/MOD_SKILL_ENEMY_RATE*3;
				break;
				case HOLLAND:
					pchar.money = 500/MOD_SKILL_ENEMY_RATE*3;
				break;
				case PIRATE:
					GiveItem2Character(Pchar, "Map_part1");
				break;
				case SPAIN:
					pchar.money = 2000/MOD_SKILL_ENEMY_RATE*3;
				break;
			}
	    break;

	    case "Adventurer":	// Авантюрист
			SetCharacterPerk(pchar, "Adventurer");
			itemID = GetGeneratedItem("blade5");
            GiveItem2Character(Pchar, itemID);
            EquipCharacterbyItem(Pchar, itemID);
			TakenItems(Pchar, "potion1", rand(10));
			GiveItem2Character(Pchar, "Totem_" + rand(14));
			GiveItem2Character(Pchar, "Totem_" + rand(14));
			GiveItem2Character(Pchar, "Totem_" + rand(14));
	        itemID = GetGeneratedItem("Spyglass" + rand(3));
			GiveItem2Character(Pchar, itemID);
			EquipCharacterbyItem(Pchar, itemID);
			pchar.money = 3000/MOD_SKILL_ENEMY_RATE*3;
            TakenItems(Pchar, "Food1", rand(1)+2);
			itemID = GetGeneratedItem("pistol1");
            GiveItem2Character(Pchar, itemID);
            EquipCharacterbyItem(Pchar, itemID);
			GiveGunAmmunitionPchar(Pchar,itemID,10);
			switch (sti(ch.nation))
			{
				case ENGLAND:
					itemID = GetGeneratedItem("blade08");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
				break;
				case FRANCE:
					GiveGunAmmunitionPchar(Pchar,itemID,10);
					itemID = GetGeneratedItem("blade03");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
				break;
				case HOLLAND:
					itemID = GetGeneratedItem("blade3");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
				break;
				case PIRATE:
					GiveGunAmmunitionPchar(Pchar,itemID,10);
					itemID = GetGeneratedItem("blade5");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
				break;
				case SPAIN:
					itemID = GetGeneratedItem("blade09");
					GiveItem2Character(Pchar, itemID);
					EquipCharacterbyItem(Pchar, itemID);
				break;
			}
	    break;

	    case "Buccaneer":	// Буканьер
			SetCharacterPerk(pchar, "Buccaneer");
			itemID = GetGeneratedItem("blade08");
            GiveItem2Character(Pchar, itemID);
            EquipCharacterbyItem(Pchar, itemID);
			itemID = GetGeneratedItem("pistol1");
            GiveItem2Character(Pchar, itemID);
            EquipCharacterbyItem(Pchar, itemID);
			itemID = GetGeneratedItem("mushket_Arguebuze");
            GiveItem2Character(Pchar, itemID);
			TakeNItems(Pchar, "bullet", 30);
			AddItems(Pchar, "GunPowder", 30);
			GiveGunAmmunitionPchar(Pchar,itemID,30);
			TakenItems(Pchar, "potion1", rand(10));
			pchar.money = 1000/MOD_SKILL_ENEMY_RATE*3;
            TakenItems(Pchar, "Food1", rand(6)+4);
	    break;

	    case "Agent":	// Тайный агент
			SetCharacterPerk(pchar, "Agent");
			itemID = GetGeneratedItem("blade5");
            GiveItem2Character(Pchar, itemID);
            EquipCharacterbyItem(Pchar, itemID);
			TakenItems(Pchar, "Food1", rand(6)+4);
			TakenItems(Pchar, "jewelry15", 1);
			TakenItems(Pchar, "Totem_2", 1);
			TakenItems(Pchar, "indian5", 1);
			TakenItems(Pchar, "indian17", 1);
			TakenItems(Pchar, "suit_1", 1);
			SetCharacterPerk(pchar, "FlagEng");
			SetCharacterPerk(pchar, "FlagFra");
			SetCharacterPerk(pchar, "FlagSpa");
			SetCharacterPerk(pchar, "FlagHol");
			switch (sti(ch.nation))
			{
				case ENGLAND:
					GiveItem2Character(Pchar, "patent_eng");
					pchar.PatentNation = "eng";
				break;
				case FRANCE:
					GiveItem2Character(Pchar, "patent_fra");
					pchar.PatentNation = "fra";
				break;
				case HOLLAND:
					GiveItem2Character(Pchar, "patent_hol");
					pchar.PatentNation = "hol";
				break;
				case PIRATE:

				break;
				case SPAIN:
					GiveItem2Character(Pchar, "patent_spa");
					pchar.PatentNation = "spa";
				break;
			}
	    break;

		case "SeaWolf":	// Морской Волк
			SetCharacterPerk(pchar, "SeaWolf");
			itemID = GetGeneratedItem("blade3");
            GiveItem2Character(Pchar, itemID);
            EquipCharacterbyItem(Pchar, itemID);
			GiveItem2Character(Pchar, "Ship_Print_6");
			itemID = GetGeneratedItem("pistol" +  (rand(2)+1));
	        itemID = GetGeneratedItem("Spyglass" + rand(3));
            GiveItem2Character(Pchar, itemID);
            EquipCharacterbyItem(Pchar, itemID);
			GiveGunAmmunitionPchar(Pchar,itemID,10);
			TakenItems(Pchar, "potion1", rand(8));
			pchar.money = 2000/MOD_SKILL_ENEMY_RATE*3;
            TakenItems(Pchar, "Food1", rand(6)+4);
	    break;
	}
}
int GetNewMainCharacterFace()
{
    return sti(GetNewMainCharacterParam("heroFace_" + startHeroType));
}

string GetNewMainCharacterName()
{
    return GetHeroName(startHeroType);
}

string GetMainCharacterNameGen()  // ==> eddy. родительный падеж полного имени ГГ
{
    return GetHeroNameGen(startHeroType);
}
string GetMainCharacterNameDat()  // ==> eddy. дательный падеж полного имени ГГ
{
    return GetHeroNameDat(startHeroType);
}

string GetHeroName(int _n)
{
    string totalInfo;

    string sLast;
    totalInfo = GetNewMainCharacterParam("heroName_" + _n);
    // LEO: у героев без фамилии (Виспер) приклеивался хвостовой пробел - в текстах выходил двойной
    sLast = GetNewMainCharacterParam("heroLastname_" + _n);
    if (sLast != "") totalInfo = totalInfo + " " + sLast;

    return totalInfo;
}

string GetHeroNameGen(int _n)
{
    string totalInfo;

    string sLast;
    totalInfo = GetNewMainCharacterParam("heroNameGen_" + _n);
    sLast = GetNewMainCharacterParam("heroLastnameGen_" + _n); // LEO: см. GetHeroName - пустая фамилия давала двойной пробел
    if (sLast != "") totalInfo = totalInfo + " " + sLast;

    return totalInfo;
}
string GetHeroNameDat(int _n)
{
    string totalInfo;

    string sLast;
    totalInfo = GetNewMainCharacterParam("heroNameDat_" + _n);
    sLast = GetNewMainCharacterParam("heroLastnameDat_" + _n); // LEO: см. GetHeroName - пустая фамилия давала двойной пробел
    if (sLast != "") totalInfo = totalInfo + " " + sLast;

    return totalInfo;
}
int GetNewMainCharacterNation(int _startHeroType)
{
    int    idLngFile = -1;
    string totalInfo;

    idLngFile = LanguageOpenFile("HeroDescribe.txt");
    totalInfo = LanguageConvertString(idLngFile, "heroNation_" + _startHeroType);
    LanguageCloseFile(idLngFile);

    switch(totalInfo)
	{
		case "Англия":
            return ENGLAND;
		break;
		case "Франция":
            return FRANCE;
		break;
		case "Испания":
            return SPAIN;
		break;
		case "Голландия":
			return HOLLAND;
		break;
		case "Береговое братство":
			return PIRATE;
		break;
		case "ENGLAND":
            return ENGLAND;
		break;
		case "FRANCE":
            return FRANCE;
		break;
		case "SPAIN":
            return SPAIN;
		break;
		case "HOLLAND":
			return HOLLAND;
		break;
		case "PIRATE":
			return PIRATE;
		break;
	}
	return PIRATE;
}

string GetNewMainCharacterType(int _startHeroType)
{
    string ret = GetNewMainCharacterParam("heroType_" + _startHeroType);

	if(LanguageGetLanguage() != "russian")
	{
		switch (ret) // нужно по английски для квестбука
		{
    	    case "Merchant":
    		    ret = "Trader";
		    break;

		    case "Slasher":
    		    ret = "Grunt";
		    break;

		    case "Swordsman":
    	        ret = "Fencer";
		    break;

		    case "Adventurer":
    	        ret = "Adventurer";
		    break;

		    case "Buccaneer":
    	        ret = "Buccaneer";
		    break;

		    case "Secret Agent":
    	        ret = "Agent";
		    break;

			case "Sea Wolf":
    	        ret = "SeaWolf";
		    break;
		}
	}
	else
	{
		switch (ret) // нужно по английски для квестбука
		{
    	    case "Торговец":
    		    ret = "Trader";
		    break;

		    case "Рубака":
    		    ret = "Grunt";
		    break;

		    case "Фехтовальщик":
    	        ret = "Fencer";
		    break;

		    case "Авантюрист":
    	        ret = "Adventurer";
		    break;

		    case "Буканьер":
    	        ret = "Buccaneer";
		    break;

		    case "Тайный агент":
    	        ret = "Agent";
		    break;

			case "Морской Волк":
    	        ret = "SeaWolf";
		    break;
		}
	}
    return ret;
}

string GetNewMainCharacterParam(string _param)
{
    int    idLngFile = -1;
    string totalInfo;

    idLngFile = LanguageOpenFile("HeroDescribe.txt");
    totalInfo = LanguageConvertString(idLngFile, _param);
    LanguageCloseFile(idLngFile);

    return totalInfo;
}

// вернуть базовую нацию ГГ или патент
int GetBaseHeroNation()
{
	if (isMainCharacterPatented())
	{
	    return sti(Items[sti(pchar.EquipedPatentId)].Nation);
	}
	return sti(pchar.BaseNation);
}

// LEO: есть ли у ГГ перк-флаг нации (право легитимно нести флаг — гейт как в диалоге смены флага)
bool HeroHasFlagPerk(int iNation)
{
	if (iNation == PIRATE)  return IsCharacterPerkOn(pchar, "FlagPir");
	if (iNation == ENGLAND) return IsCharacterPerkOn(pchar, "FlagEng");
	if (iNation == FRANCE)  return IsCharacterPerkOn(pchar, "FlagFra");
	if (iNation == SPAIN)   return IsCharacterPerkOn(pchar, "FlagSpa");
	if (iNation == HOLLAND) return IsCharacterPerkOn(pchar, "FlagHol");
	return false;
}
