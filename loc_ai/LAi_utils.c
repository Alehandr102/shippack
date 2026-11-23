
//Поверить ссылку на персонажа
bool LAi_CheckCharacter(aref chr, string out)
{
	if(!TestRef(chr))
	{
		Trace("LAi_CheckCharacter -> invalide aref, call from " + out);
		return false;
	}
	if(CheckAttribute(chr, "index") == false)
	{
		Trace(out + " -> invalide character, no field <index>");
		return false;
	}
	if(CheckAttribute(chr, "chr_ai.type") == false)
	{
		Trace(out + " -> invalide character " + chr.id + ", no field <chr_ai.type>");
		return false;
	}
	if(CheckAttribute(chr, "chr_ai.tmpl") == false)
	{
		Trace(out + " -> invalide character, no field <chr_ai.tmpl>");
		return false;
	}
	return true;
}

//Зарядился ли пистолет у персонажа
bool LAi_CharacterCanFrie(aref chr)
{
	if (!CheckAttribute(chr, "chr_ai.chargeprc"))
	{
		chr.chr_ai.chargeprc = "1";
		chr.chr_ai.charge = 0;
		return false;
	}
	
	if (stf(chr.chr_ai.charge) >= 1.0) return true;
	
	return false;
}

//Получить количество энергии у персонажа 0..100
float Lai_CharacterGetEnergy(aref chr)
{
	if(CheckAttribute(chr, "chr_ai.energy"))
	{
		return stf(chr.chr_ai.energy);
	}
	return 0.0;
}

//Изменить количество энергии у персонажа 0..100
void Lai_CharacterChangeEnergy(aref chr, float dlt)
{
	if(CheckAttribute(chr, "chr_ai.energy"))
	{
		float cur = stf(chr.chr_ai.energy);
		cur = cur + dlt;
		if(cur < 0.0) cur = 0.0;
		if(cur > LAi_GetCharacterMaxEnergy(chr)) cur = LAi_GetCharacterMaxEnergy(chr); //boal
		chr.chr_ai.energy = cur;
	}else{
		if(dlt < 0.0) dlt = 0.0;
		if(dlt > LAi_GetCharacterMaxEnergy(chr)) dlt = LAi_GetCharacterMaxEnergy(chr);
		chr.chr_ai.energy = dlt;
	}
}

//Может ли сражаться персонаж в заданной локации
bool LAi_LocationCanFight()
{
	if(IsEntity(loadedLocation) != true) return true;
	if(CheckAttribute(loadedLocation, "noFight") != true)  return true;
	if(sti(loadedLocation.noFight) != false) return false;
	return true;
}

//Найт кол-во локаторов в заданной группе <-- ugeen
int LAi_GetLocatorNum(string group)
{
	if(IsEntity(loadedLocation) != true) return -1;
	string at = "locators." + group;
	if(CheckAttribute(loadedLocation, at) == 0) return 0;
	aref grp;
	makearef(grp, loadedLocation.(at));
	return GetAttributesNum(grp);
}

//Найти случайный локатор в заданной группе локаторов
string LAi_FindRandomLocator(string group)
{
	if(IsEntity(loadedLocation) != true) return "";
	string at = "locators." + group;
	if(CheckAttribute(loadedLocation, at) == 0) return "";
	aref grp;
	makearef(grp, loadedLocation.(at));
	int num = GetAttributesNum(grp);
	if(num <= 0) return "";
	num = rand(num - 1);
	return GetAttributeName(GetAttributeN(grp, num));
}

//Найти дальний локатор в заданной группе локаторов
string LAi_FindFarLocator(string group, float x, float y, float z)
{
	if(IsEntity(loadedLocation) != true) return "";
	string at = "locators." + group;
	if(CheckAttribute(loadedLocation, at) == 0) return "";
	aref grp;
	makearef(grp, loadedLocation.(at));
	int num = GetAttributesNum(grp);
	if(num <= 0) return "";
	int j = -1;
	float dist = -1;
	for(int i = 0; i < num; i++)
	{
		aref loc = GetAttributeN(grp, i);
		float dx = x - stf(loc.x);
		float dy = y - stf(loc.y);
		float dz = z - stf(loc.z);
		float d = dx*dx + dy*dy + dz*dz;
		if(j >= 0)
		{
			if(d >= dist)
			{
				dist = d;
				j = i;
			}
		}else{
			j = i;
			dist = d;
		}
	}
	if(j < 0) return "";
	return GetAttributeName(GetAttributeN(grp, j));
}

//Найти дальний свободный локатор в заданной группе локаторов
string LAi_FindFarFreeLocator(string group, float x, float y, float z)
{
	if(IsEntity(loadedLocation) != true) return "";
	string at = "locators." + group;
	if(CheckAttribute(loadedLocation, at) == 0) return "";
	aref grp;
	makearef(grp, loadedLocation.(at));
	int num = GetAttributesNum(grp);
	if(num <= 0) return "";
	int j = -1;
	float dist;
	for(int i = 0; i < num; i++)
	{
		aref loc = GetAttributeN(grp, i);
		float lx = stf(loc.x);
		float ly = stf(loc.y);
		float lz = stf(loc.z);
		if(CheckLocationPosition(loadedLocation, lx, ly, lz))
		{
			float dx = x - lx;
			float dy = y - ly;
			float dz = z - lz;
			float d = dx*dx + dy*dy + dz*dz;
			if(j >= 0)
			{
				if(d > dist)
				{
					dist = d;
					j = i;
				}
			}else{
				j = i;
				dist = d;
			}
		}
	}
	if(j < 0) return "";
	return GetAttributeName(GetAttributeN(grp, j));
}

//Найти ближайший свободный локатор
string LAi_FindNearestFreeLocator(string group, float x, float y, float z)
{
	if(IsEntity(loadedLocation) != true) return "";
	string at = "locators." + group;
	if(CheckAttribute(loadedLocation, at) == 0) return "";
	aref grp;
	makearef(grp, loadedLocation.(at));
	int num = GetAttributesNum(grp);
	if(num <= 0) return "";
	int j = -1;
	float dist = 0;
	for(int i = 0; i < num; i++)
	{
		aref loc = GetAttributeN(grp, i);
		float lx = stf(loc.x);
		float ly = stf(loc.y);
		float lz = stf(loc.z);
		if(CheckLocationPosition(loadedLocation, lx, ly, lz))
		{
			float dx = x - lx;
			float dy = y - ly;
			float dz = z - lz;
			float d = dx*dx + dy*dy + dz*dz;
			if(j >= 0)
			{
				if(d < dist)
				{
					dist = d;
					j = i;
				}
			}else{
				j = i;
				dist = d;
			}
		}
	}
	if(j < 0) return "";
	return GetAttributeName(GetAttributeN(grp, j));
}
// boal
string LAi_FindNearestLocator(string group, float x, float y, float z)
{
	if(IsEntity(loadedLocation) != true) return "";
	string at = "locators." + group;
	if(CheckAttribute(loadedLocation, at) == 0) return "";
	aref grp;
	makearef(grp, loadedLocation.(at));
	int num = GetAttributesNum(grp);
	if(num <= 0) return "";
	int j = -1;
	float dist = -1;
	for(int i = 0; i < num; i++)
	{
		aref loc = GetAttributeN(grp, i);
		float dx = x - stf(loc.x);
		float dy = y - stf(loc.y);
		float dz = z - stf(loc.z);
		float d = dx*dx + dy*dy + dz*dz;
		if(j >= 0)
		{
			if(d < dist)
			{
				dist = d;
				j = i;
			}
		}else{
			j = i;
			dist = d;
		}
	}
	if(j < 0) return "";
	return GetAttributeName(GetAttributeN(grp, j));
}
string LAi_FindNearestFreeLocator2Pchar(string group)
{
	float locx, locy, locz;
    GetCharacterPos(Pchar, &locx, &locy, &locz);
    return LAi_FindNearestFreeLocator(group, locx, locy, locz);
}
//Найти ближайшего видимого персонажа в заданном радиусе
int LAi_FindNearestCharacter(aref chr, float radius)
{
	int res = FindNearCharacters(chr, radius, -1.0, -1.0, 0.01, false, true);
	if(res <= 0) return -1;
	return sti(chrFindNearCharacters[0].index);
}

//Найти ближайшего видимого персонажа в заданном радиусе
int LAi_FindNearestVisCharacter(aref chr, float radius)
{
	int res = FindNearCharacters(chr, radius, -1.0, -1.0, 0.01, true, true);
	if(res <= 0) return -1;
	return sti(chrFindNearCharacters[0].index);
}

//Получить уровень драки приведёный к 0..1
float LAi_GetCharacterFightLevel(aref character)
{
	//Fencing skill
	float fgtlevel = 0.0;
	fgtlevel = GetCharacterSkill(character, LAi_GetBladeFencingType(character)); // new RPG

	//Level
	fgtlevel = fgtlevel/SKILL_MAX;
	return fgtlevel;
}
// boal skill -->
//Получить уровень pistol приведёный к 0..1
float LAi_GetCharacterGunLevel(aref character)
{
	//pistol skill
	float fgtlevel = 0.0;
	fgtlevel = GetCharacterSkill(character, SKILL_PISTOL);
	fgtlevel = fgtlevel/SKILL_MAX;
	return fgtlevel;
}

float LAi_GetCharacterLuckLevel(aref character)
{
	float fgtlevel = 0.0;
	fgtlevel = GetCharacterSkill(character, SKILL_FORTUNE);
	fgtlevel = fgtlevel/SKILL_MAX;
	return fgtlevel;
}
// boal skill <--

//Применить повреждение к персонажу
void LAi_ApplyCharacterDamage(aref chr, int dmg)
{
	LAi_ApplyCharacterDamageEx(chr, dmg, false);
}

void LAi_ApplyCharacterDamageEx(aref chr, int dmg, bool fixedDamage)
{
	float damage    = MakeFloat(dmg);
	bool  bIsOfficer = false;
	//Офицерам ослабляем поврежрение
	if(CheckAttribute(chr, "chr_ai.type"))
	{
		if(chr.chr_ai.type == LAI_TYPE_OFFICER)
		{
			if (!fixedDamage) damage = damage*0.7;
			bIsOfficer = true;
		}
	}
	// LEO: сопротивление урону (Старший скелет кладбища и т.п.) - доля 0.0..1.0 (0.2/0.4/0.6 по сложности)
	if (!fixedDamage && CheckAttribute(chr, "DmgResist")) damage = damage * (1.0 - stf(chr.DmgResist));
	// Gregg: Чёрная Орхидея - оберег из трилогии К:ПС, срезает часть входящего урона
	if (!fixedDamage && BlackOrchid_IsWorn(chr)) damage = damage * (1.0 - BO_DMG_REDUCE);
	//Получаем текущие параметры
	if(!CheckAttribute(chr, "chr_ai.hp"))     chr.chr_ai.hp     = LAI_DEFAULT_HP;
	if(!CheckAttribute(chr, "chr_ai.hp_max")) chr.chr_ai.hp_max = LAI_DEFAULT_HP_MAX;
	float maxhp = stf(chr.chr_ai.hp_max);
	float hp    = stf(chr.chr_ai.hp);
	//Пересчитываем
	if (damage > hp)  damage = hp;
	hp = hp - damage;
	if(!fixedDamage && hp < 1.0) hp = 0.0;
	// Gregg: вторая жизнь от Чёрной Орхидеи здесь НЕ ловится, хотя раньше стояла именно тут.
	// Функция урона - не единственный сток HP: тик кровотечения вычитает здоровье напрямую
	// (LAi_character.c, hp -= fLimit) и у него, в отличие от яда, нет нижнего порога. То есть
	// в клинковом бою последние очки жизни чаще всего съедает кровь, а не удар, и оберег
	// не срабатывал вовсе. Спасение перенесено в LAi_CheckKillCharacter - единственный
	// гейт смерти, через который приходят ВСЕ стоки HP.
	chr.chr_ai.hp = hp;
	Boarding_AccumPlayerDamage(chr, damage, maxhp); // LEO: учёт НАНЕСЁННОГО урона по абордажникам ГГ → потери команды по урону (а не по трупам)
	//Проверим квест
	LAi_ProcessCheckMinHP(chr);

	bool bloodSize = false;
	if (damage > 0.3*maxhp) bloodSize = true;
	float fRange = 1.0 + frand(0.6);
	if(!fixedDamage && damage > func_min(22, makeint(0.1*maxhp)) && CheckAttribute(chr, "model.animation") && CheckAttribute(chr, "sex") && chr.model.animation != "skeleton" && chr.model.animation != "Terminator" && chr.sex != "skeleton")
	{
		LaunchBlood(chr, fRange, bloodSize);
	}

	if(sti(pchar.index) == sti(chr.index))
	{
		// здоровье -->
        pchar.Health.Damg      = stf(pchar.Health.Damg) + damage;
	    pchar.Health.weekDamg  = stf(pchar.Health.weekDamg) + damage;
        pchar.Health.TotalDamg = stf(pchar.Health.TotalDamg) + damage;
        if(damage > 0.0) SendMessage(chr, "l", MSG_CHARACTER_AIM_HIT);
        // здоровье <--
		SendMessage(chr, "lfff", MSG_CHARACTER_VIEWDAMAGE, dmg, MakeFloat(MakeInt(hp)), MakeFloat(MakeInt(maxhp)));
        return;
	}
	if (bIsOfficer)
    {
        chr.Health.TotalDamg = stf(chr.Health.TotalDamg) + damage; // статистика
        return;
    }
	//Напишем о нанесённом повреждении
	SendMessage(chr, "lfff", MSG_CHARACTER_VIEWDAMAGE, dmg, MakeFloat(MakeInt(hp)), MakeFloat(MakeInt(maxhp)));
}

//ККС - Jason: самовосстанавливающийся абордажник
void LAi_CheckHalfImmortal(aref chr)
{
	pchar.DeadOfficer = chr.id;

	SetCharacterTask_Dead(chr);
	LAi_SetCurHPMax(chr);

	CheckForReleaseOfficer(sti(chr.index));
	int healing_time = makeint(LAi_GetCharacterMaxHP(chr)/10);//время от
	if (CheckOfficersPerk(pchar, "EmergentSurgeon"))
	{
		healing_time -= makeint(healing_time/10*6);//снижения от перков врачей
	}
	else
	{
		if (CheckOfficersPerk(pchar, "Doctor2"))
		{
			healing_time -= makeint(healing_time/10*4);
		}
		else
		{
			if (CheckOfficersPerk(pchar, "Doctor1")) healing_time -= makeint(healing_time/5);
		}
	}
	if (CheckAttribute(chr,"TimesKilled"))
	{
		if (healing_time > 3) healing_time -= sti(chr.TimesKilled);
		chr.TimesKilled = sti(chr.TimesKilled) + 1;
		if (healing_time < 3) healing_time = 3;
	}
	else chr.TimesKilled = 1;

	if(!CheckAttribute(chr, "HPminusDays") && !CheckAttribute(chr, "HPminusDaysNeedtoRestore"))//Если теряет сознание без уже действующих "незаживших ран"
	{
		chr.HPminusDays = 0;               //Для подчета дней, которые офицер уже прожил после потери сознания.
		chr.HPminusDaysNeedtoRestore = healing_time; //Количество дней для выздоровления
	}
	else //Если теряет сознание при одной "незвжившей ране" и более
	{
		chr.HPminusDaysNeedtoRestore = sti(chr.HPminusDaysNeedtoRestore) + healing_time;
	}
}

//Убить персонажа, если закончились hp
void LAi_CheckKillCharacter(aref chr)
{
	LAi_CheckLethalDamage(chr, -1, false, false);
}

void LAi_CheckLethalDamage(aref chr, int attackerIndex, bool firearm, bool headshot)
{
	if(SendMessage(chr, "ls", MSG_CHARACTER_EX_MSG, "IsDead")) return;
	if(!CheckAttribute(chr, "chr_ai.hp")) chr.chr_ai.hp = 0.0;
	//Проверяем
	if(stf(chr.chr_ai.hp) >= 1.0) {
		return;
	}
	if (attackerIndex < 0 && !firearm)
	{
		int iattackID;
		if (CheckAttribute(chr, "chr_ai.Blooding.BlooderIDx")) {
			iattackID = sti(chr.chr_ai.Blooding.BlooderIDx);
		}
		if (CheckAttribute(chr, "chr_ai.KilledByIDx")) {
			iattackID = sti(chr.chr_ai.KilledByIDx);
		}
		if (iattackID) {
			ref rAttack = GetCharacter(iattackID);
			bool isSetBlade = CheckAttribute(chr, "equip.blade");
			AddCharacterExpToSkill(rAttack, LAi_GetBladeFencingType(rAttack), LAi_CalcExpForBladeKill( rAttack, chr, isSetBlade));
		}
	}
	//Убиваем, если смертен
	DeleteAttribute(chr,"chr_ai.Blooding");
	DeleteAttribute(chr,"chr_ai.poison");
	if(CheckAttribute(chr, "chr_ai.immortal"))
	{
		if(sti(chr.chr_ai.immortal) != 0)
		{
			chr.chr_ai.hp = 1.0;
			return;
		}
	}
	if(IsCharacterPerkOn(chr, "Adventurer") && !CheckAttribute(chr, "FrenzyActive"))
	{
		if (!CheckAttribute(chr, "ScriptedDeath") && !CheckAttribute(chr, "Adventurers_Luck") && rand(10) <= GetCharacterSPECIALSimple(chr, SPECIAL_L))
		{
			chr.Adventurers_Luck = true;
			int hitpoints = LAi_GetCharacterMaxHP(chr) / 2;
			if (hitpoints > 200)
			{
				hitpoints = 200;
			}
			if (sti(chr.index) == GetMainCharacterIndex())
			{
				chr.chr_ai.hp =  hitpoints;
				setEffectText(chr, XI_ConvertString("SecondChance"), argb(255,180,255,180), GetCharCapNotifScale());
				PlaySound("interface\heartbeat.wav");
				//Сюда можно поставить юз звука
				return;
			}
			else
			{
				if (rand(1) == 0 || IsOfficer(chr))
				{
					chr.chr_ai.hp =  hitpoints;
					setEffectText(chr, XI_ConvertString("SecondChance"), argb(255,180,255,180), GetCharCapNotifScale());
					return;
				}
			}
		}
	}

	if(CheckAttribute(chr, "HalfImmortal") && !IsCompanion(chr))
	{
		bool bOkSave = bHalfImmortalPGG && CheckAttribute(chr, "ImmortalOfficer");
		if (bOkSave || bHalfImmortalPGG == false)
		{
			LAi_CheckHalfImmortal(chr);
			Log_InfoColoredAnim("red", GetConvertStrWithReplace("Variable_LAi_utils_3", "Logs.txt", "#space#", " ") + GetFullName(CharacterFromID(pchar.DeadOfficer)) + GetConvertStrWithReplace("Variable_LAi_utils_4", "Logs.txt", "#space#", " ")); // LEO: контузия абордажника - старая строка, теперь красным с анимацией
			return;
		}
	}

	// Lugger: Тренировки -->
	bool bArena = CheckAttribute(chr, "LandAcademy") || CheckAttribute(chr, "ArenaAction") || CheckAttribute(chr, "ArenaEtapsAction") || CheckAttribute(chr, "ArenaTournament") || CheckAttribute(chr, "ArenaOdds");
	if(bArena)
	{
		CheckAcademyAndOtherLoosers(chr);
		return;
	}
	// Lugger: Тренировки <--
	
	if(CheckAttribute(chr, "Frenzy"))
	{
		DeleteAttribute(chr, "chr_ai.Blooding");
		DeleteAttribute(chr, "chr_ai.Swift");
		DeleteAttribute(chr, "chr_ai.understun");
		DeleteAttribute(chr, "chr_ai.Trauma");
		DeleteAttribute(chr, "chr_ai.poison");
		DeleteAttribute(chr, "Frenzy");
		UnmarkCharacter(chr);
		chr.FrenzyActive = true;
		float maxHP = LAi_GetCharacterMaxHP(chr);
		chr.chr_ai.Blooding.Power = makeint(maxHP/20.0) + 1;
		chr.chr_ai.Blooding = 100;
		chr.chr_ai.Blooding.BlooderIDx = chr.index;
		setEffectText(chr, XI_ConvertString("Frenzy"), argb(255,255,180,180), GetCharCapNotifScale());
		// LEO: 3D-квестмарка убрана - эффекты показывают иконки над баром (LandInterface, WeaponEffects.dds)
		LAi_CharacterPlaySound(chr, "valhalla_scream");	
		chr.chr_ai.energy = LAi_GetCharacterMaxEnergy(chr);
		LAi_SetHP(chr, maxHP, maxHP);
		return;
	}

	// Gregg: Чёрная Орхидея - последний рубеж. Смертельный удар не убивает, а оставляет
	// на ногах и даёт десять секунд неуязвимости; заряд один и возвращается при смене
	// локации (ReloadEndFade -> BlackOrchid_Recharge).
	// Место выбрано намеренно:
	//  - это ЕДИНСТВЕННЫЙ гейт смерти, сюда приходят все стоки HP, включая тик крови;
	//  - выше уже удалены chr_ai.Blooding и chr_ai.poison, поэтому спасённый не истекает
	//    кровью следующие же секунды после снятия неуязвимости;
	//  - выше отработали бессмертие, перк Adventurer, HalfImmortal, арена и Frenzy, так что
	//    заряд не жжётся там, где персонажа и без оберега спасли бы.
	// ScriptedDeath ставит Lai_KillCharacter: сценарную смерть оберег отменять не должен.
	if (!CheckAttribute(chr, "ScriptedDeath"))
	{
		if (BlackOrchid_IsWorn(chr))
		{
			if (!CheckAttribute(PChar, "BlackOrchid.Spent"))
			{
				chr.chr_ai.hp = 1.0;
				BlackOrchid_LastStand(chr);
				return;
			}
		}
	}

	// LEO: труп НЕ должен сохранять звезду - снимаем защитные флаги ДО штатного UnmarkCharacter,
	// иначе гард по BoardElite/bGangLeader вернёт знак обратно на тело (см. UnmarkCharacter в utils.c).
	if (CheckAttribute(chr, "BoardElite"))  DeleteAttribute(chr, "BoardElite");
	if (CheckAttribute(chr, "bGangLeader")) DeleteAttribute(chr, "bGangLeader");
	UnmarkCharacter(chr);
	DeleteAttribute(chr, "quest.questflag");

	// Новая смерть не наследует состояние головы от предыдущей жизни этого слота.
	if (CheckAttribute(chr, "HeadDetached")) DeleteAttribute(chr, "HeadDetached");
	if (attackerIndex >= 0) chr.chr_ai.KilledByIDx = attackerIndex;
	chr.chr_ai.hp = 0.0;
	// boal dead can be searched 14.12.2003 -->
	Dead_AddLoginedCharacter(chr); // записали ещё живого в список трупов
	// boal dead can be searched 14.12.2003 <--
	// LEO: толчок только при настоящей смерти, после квестовых защит и второй жизни.
	float headshotX = 0.0;
	float headshotZ = 0.0;
	if (headshot)
	{
		ref shooter = GetCharacter(attackerIndex);
		float sx, sy, sz, tx, ty, tz;
		if (GetCharacterPos(shooter, &sx, &sy, &sz) && GetCharacterPos(chr, &tx, &ty, &tz))
		{
			headshotX = tx - sx;
			headshotZ = tz - sz;
			SendMessage(chr, "lsff", MSG_CHARACTER_EX_MSG, "LethalHeadshot", headshotX, headshotZ);
		}
	}
	SetCharacterTask_Dead(chr);
	if (attackerIndex >= 0 && !firearm)
	{
		// Count the musket stock kill once the death is confirmed.
		ref killer = GetCharacter(attackerIndex);
		Statistic_KillChar(killer, chr, "_s");
		LAi_SetResultOfDeath(killer, chr, CheckAttribute(chr, "equip.blade"), false);
	}
	// Вариант отстрела выбирается после смертельного попадания и не зависит от крита урона.
	if (headshot && sti(InterfaceStates.ChopBodies))
		SendMessage(chr, "lsfffl", MSG_CHARACTER_EX_MSG, "CriticalHeadshotGore", headshotX, headshotZ, MOD_DEAD_CLEAR_TIME, rand(1));
	Postevent(EVENT_CHARACTER_DEAD, 1, "a", chr);
	//Переинициируем параметры
	if(CheckAttribute(chr, "chr_ai.type"))
	{
		string func = chr.chr_ai.type;
		chr.chr_ai.type = "";
		chr.chr_ai.tmpl = "";
		if(func != "")
		{
			func = "LAi_type_" + func + "_Init";
			call func(chr);
		}
		if(sti(chr.index) == nMainCharacterIndex)
		{
			//Если убили игрока, то запретим интерфейс
			InterfaceStates.Launched = true;
		}
	}
	LAi_Character_Dead_Process(chr);

	if (CheckAttribute(chr, "CantLoot"))
	{
		Dead_DelLoginedCharacter(chr);//не обыскивается
	}
}

//Создать фантомного персонажа
ref LAi_CreateFantomCharacter(string model, string group, string locator)
{
	return LAi_CreateFantomCharacterEx(model, "man", group, locator);
}

//Создать фантомного персонажа
ref LAi_CreateFantomCharacterEx(string model, string ani, string group, string locator)
{
	//Ищем свободное место для персонажа
	for(int i = 0; i < MAX_CHARS_IN_LOC; i++)
	{
		if(CheckAttribute(&Characters[LOC_FANTOM_CHARACTERS + i], "id") == false) break;
		if(Characters[LOC_FANTOM_CHARACTERS + i].id == "") break;
	}
	if(i >= MAX_CHARS_IN_LOC)
	{
		for(i = 0; i < MAX_CHARS_IN_LOC; i++)
		{
			if(!IsEntity(&Characters[LOC_FANTOM_CHARACTERS + i])) break;
		}
		if(i >= MAX_CHARS_IN_LOC) i = 0;
	}
	ref chr = &Characters[LOC_FANTOM_CHARACTERS + i];
	//Заполняем поля персонажа
	chr.id = "Location fantom character <" + i + ">";
	chr.index = LOC_FANTOM_CHARACTERS + i;			   
	//address
	if(IsEntity(loadedLocation) != true)
	{
		chr.location = "none";
	}else{
		chr.location = loadedLocation.id;
	}
	chr.location.group = "";
	chr.location.locator = "";
	chr.location.from_sea = "";
	chr.location.fantom = "1";
	//model
	if(model != "") chr.model = model; else chr.model = "man1";
	chr.model.entity = "NPCharacter";
	chr.model.animation = ani;
	// fix -->
    if (chr.model.animation == "towngirl" || chr.model.animation == "towngirl3" || chr.model.animation == "towngirl2")
    {
       chr.model.height = 1.75;
	   chr.sex = "Woman";
    }
    else
    {
	   chr.model.height = 1.8;
	   chr.sex = "man";
	}
	// fix <--
	chr.FaceId = "0";
	chr.headModel = "h_" + chr.model;
	FaceMaker(chr);
	//game params
 	CalculateAppropriateSkills(chr);
	SetSpeciality(chr, PerksChars());
	SetFantomHP(chr);
	chr.money = 100 + rand(500);
	chr.reputation = 10 + rand(80);
	chr.skill.freeskill = 0;
	chr.experience = 0;
	chr.spyglass.itemID = COMMON_SPYGLASS;
	//Quest data
	chr.Dialog.CurrentNode = "First time";
	chr.Dialog.TempNode = chr.Dialog.CurrentNode;
	chr.quest.meeting = "0";
	chr.quest = "True";
	//nation
	int nat = GetLocationNation(loadedLocation);
	if(nat >= 0)
	{
		chr.nation = nat;
	}else{
		chr.nation = pchar.nation;
	}
	//name
	SetRandomNameToCharacter(chr);

	SetFoodToCharacter(chr, 3, 10);
	//Логинем персонажа в локацию
	chr.chr_ai.type    = LAI_DEFAULT_TYPE;
	chr.chr_ai.tmpl    = LAI_DEFAULT_TEMPLATE;
	chr.chr_ai.group   = LAI_DEFAULT_GROUP;
	chr.chr_ai.alarmreact = LAI_DEFAULT_ALARMREACT;
	chr.chr_ai.grpalarmr  = LAI_DEFAULT_GRPALARMR;
	chr.chr_ai.charge  = LAI_DEFAULT_CHARGE;
	chr.chr_ai.FencingType  = "Fencing"; //fix
	SetEnergyToCharacter(chr); // boal
	if(LAi_numloginedcharacters >= MAX_CHARS_IN_LOC)
	{
		Trace("LAi_CreateFantomCharacter -> many logined characters in location > " + MAX_CHARS_IN_LOC);
		return chr;
	}
	LAi_AddLoginedCharacter(chr);
	if (IsCharacterPerkOn(chr, "Ciras") && rand(4)==0)
	{
		string cirnum;
		switch (rand(4))
		{
			case 0: cirnum = "cirass1"; break;
			case 1: cirnum = "cirass1"; break;
			case 2: cirnum = "cirass2"; break;
			case 3: cirnum = "cirass3"; break;
			case 4: cirnum = "cirass4"; break;
		}
		chr.cirassId = Items_FindItemIdx(cirnum);
		Log_TestInfo(GetConvertStrWithReplace("Variable_LAi_utils_8", "Logs.txt", "#space#", " ")+chr.name+GetConvertStrWithReplace("Variable_LAi_utils_9", "Logs.txt", "#space#", " ")+cirnum);
	}
	if(!CreateCharacter(chr))
	{
		Trace("LAi_CreateFantomCharacter -> CreateCharacter return false");
		return chr;
	}
	//Поставим персонажа на локатор
	if(group == "")
	{
		group = "goto";
		locator = "";
	}
	if(locator == "")
	{
		//Выбираем дальний локатор
		locator = LAi_FindNPCLocator(group);
	}
	chr.location.group = group;
	chr.location.locator = locator;
	if(SendMessage(chr, "lss", MSG_CHARACTER_ENTRY_TO_LOCATION, group, locator) == false)
	{
		Trace("LAi_CreateFantomCharacter -> can't teleportation character to <" + group + "::" + locator + ">");
	}
	return chr;
}
// boal
string LAi_FindNPCLocator(string group)
{
    string  locator = "";
	//Выбираем дальний локатор
	float posX, posY, posZ;
	posX = 0.0; posY = 0.0; posZ = 0.0;

	ref  MnChar = GetMainCharacter();

	if(GetCharacterPos(MnChar, &posX, &posY, &posZ))
	{
		// boal -->
		if (sti(MnChar.GenQuestFort.FarLocator) == true)
		{
			locator = LAi_FindFarFreeLocator(group, posX, posY, posZ);
		}
		else
		{
			locator = LAi_FindNearestFreeLocator(group, posX, posY, posZ);  // ближайший
            if(locator == "")
			{
				locator = LAi_FindRandomLocator(group);
			}
		}
		if(locator == "")
		{
			locator = LAi_FindFarLocator(group, posX, posY, posZ);
			if(locator == "")
			{
				locator = LAi_FindRandomLocator(group);
			}
		}
	}
	else
	{
		locator = LAi_FindRandomLocator(group);
	}
	return locator;
}

object lai_questdelays;
string LAi_QuestDelay(string quest, float delayTime)
{
	int num = GetAttributesNum(&lai_questdelays);
	string atr = "e";
	for(int i = 0; i < num + 10; i++)
	{
		atr = "e" + i;
		if(!CheckAttribute(&lai_questdelays, atr)) break;
		if (lai_questdelays.(atr).quest == quest) break; // boal Это уже сущ квест, нам нужно его переназначить
	}
	lai_questdelays.(atr) = delayTime;
	lai_questdelays.(atr).quest = quest;

	return atr; //boal
}
//boal -->
void LAi_MethodDelay(string quest, float delayTime)
{
	string atr = LAi_QuestDelay(quest, delayTime);
	lai_questdelays.(atr).method = true;
}
// boal <--
void LAi_QuestDelayProcess(float dltTime)
{
	string atr;
	int num = GetAttributesNum(&lai_questdelays);
	//Обсчитываем времена
	for(int i = 0; i < num; i++)
	{
		atr = GetAttributeName(GetAttributeN(&lai_questdelays, i));
		lai_questdelays.(atr) = stf(lai_questdelays.(atr)) - dltTime;
	}
	//Проверяем исполнение и вызываем обработку
	for(i = 0; i < num; i++)
	{
		atr = GetAttributeName(GetAttributeN(&lai_questdelays, i));
		if(atr == "root")
		{
			Trace("LAi_QuestDelayProcess -> invalide attibute field: root!!!");
			continue;
		}
		float time = stf(lai_questdelays.(atr));
		if (time <= 0)
		{
			string quest = lai_questdelays.(atr).quest;
			if (CheckAttribute(&lai_questdelays, atr + ".method"))
			{
                DeleteAttribute(&lai_questdelays, atr);
                call quest();
			}
			else
			{
                DeleteAttribute(&lai_questdelays, atr);
                CompleteQuestName(quest, "");
			}
			i = -1;
			num = GetAttributesNum(&lai_questdelays);
		}
	}
}

void LAi_CheckCharacterID(aref chr)
{
	SendMessage(chr, "ss", "CheckID", chr.id);
}

void LAi_SetDefaultStayAnimation(aref chr)
{
	if(IsEntity(chr))
	{
		BeginChangeCharacterActions(chr);
		SetDefaultStayIdle(chr);
		SetDefaultNormWalk(chr);
		SetDefaultFight(chr);
		SetDefaultDead(chr);
		SetDefaultFightDead(chr);
		if (CheckAttribute(chr,"norun")) SetNoRun(chr);
		EndChangeCharacterActions(chr);
	}
}

void LAi_SetDefaultDead(aref chr)
{
	if(IsEntity(chr))
	{
		BeginChangeCharacterActions(chr);
		SetDefaultDead(chr);
		EndChangeCharacterActions(chr);
	}
}

void LAi_SetAfraidDead(aref chr)
{
	if(IsEntity(chr))
	{
		BeginChangeCharacterActions(chr);
		SetAfraidDead(chr);
		EndChangeCharacterActions(chr);
	}
}

void LAi_SetDefaultSitAnimation(aref chr)
{
	if(IsEntity(chr))
	{
		BeginChangeCharacterActions(chr);
		if (CheckAttribute(chr, "nonTable")) SetDefaultSit2Idle(chr);
		else SetDefaultSitIdle(chr);
		SetDefaultSitDead(chr);
		EndChangeCharacterActions(chr);
	}
}

void LAi_SetHuberSitAnimation(aref chr)
{
	if(IsEntity(chr))
	{
		BeginChangeCharacterActions(chr);
		SetHuberAnimation(chr);
		SetDefaultSitDead(chr);
		EndChangeCharacterActions(chr);
	}
}


//Вывести экран в темноту, выполнить квест questFadeOut, вернуть всё обратно, выполнить квест questFadeIn
object LAi_QuestFader;
void LAi_Fade(string questFadeOut, string questFadeIn)
{
	if(questFadeOut != "") LAi_QuestDelay(questFadeOut, 0.55);
	if(questFadeIn != "") LAi_QuestDelay(questFadeIn, 1.10);

	if(IsEntity(&LAi_QuestFader))
	{
		Trace("LAi_Fade -> previous fade operation not ended!");
		return;
	}
	//Fader
	SetEventHandler("FaderEvent_EndFade", "LAi_FadeEndFadeOut", 0);
	SetEventHandler("FaderEvent_EndFadeIn", "LAi_FadeEndFadeIn", 0);
	CreateEntity(&LAi_QuestFader, "fader");
	SendMessage(&LAi_QuestFader, "lfl", FADER_OUT, 0.5, false);
	LAi_QuestFader.oldSaveState = InterfaceStates.Buttons.Save.enable;
	InterfaceStates.Buttons.Save.enable = 0;
}

void LAi_FadeEndFadeOut()
{
	DelEventHandler("FaderEvent_EndFade", "LAi_FadeEndFadeOut");
	SendMessage(&LAi_QuestFader, "lfl", FADER_IN, 0.5, true);
}

void LAi_FadeEndFadeIn()
{
	DelEventHandler("FaderEvent_EndFadeIn", "LAi_FadeEndFadeIn");
	InterfaceStates.Buttons.Save.enable = LAi_QuestFader.oldSaveState;
}

//Вывести экран в темноту, через указанное время вернуть
void LAi_FadeDelay(float _delayTime, string sPath)
{
    pchar.GenQuest.CallFunctionParam = "FadeDelay";
    DoQuestCheckDelay("CallFunctionParam", _delayTime);
	//Fader
	CreateEntity(&LAi_QuestFader, "fader");
	//Boyer add #20170401-02
	pchar.loadscreen = sPath;
	SendMessage(&LAi_QuestFader, "ls", FADER_PICTURE0, sPath);
	SendMessage(&LAi_QuestFader, "lfl", FADER_OUT, 0.5, false);
	LAi_QuestFader.oldSaveState = InterfaceStates.Buttons.Save.enable;
	InterfaceStates.Buttons.Save.enable = 0;
}

void FadeDelay()    // относится к методу выше.
{
	LAi_FadeEndFadeIn();
	LAi_FadeEndFadeOut();
}

//Есть ли оружие у персонада
bool LAi_IsSetBale(aref chr)
{
	return (SendMessage(chr, "ls", MSG_CHARACTER_EX_MSG, "IsSetBalde") != 0);
}

//В режиме боя
bool LAi_IsFightMode(aref chr)
{
	// boal fix -->
	if ((bLandInterfaceStart) && CheckAttribute(chr, "equip.blade"))
    {
        return (SendMessage(&chr, "ls", MSG_CHARACTER_EX_MSG, "IsFightMode") != 0);
    }
    return false;
    // boal fix <--
}

//Установить флажёк для востановления хитпойнтов и отношений
#event_handler("EventWorldMapInit", "LAi_SetRestoreStates");
#event_handler(EVENT_SEA_LOGIN, "LAi_SetRestoreStates");
void LAi_SetRestoreStates()
{
	LAi_restoreStates = true;
	// boal 04/02/2005 FIX
	QuestsCheck();
}

#event_handler("EventSGMode", "LAi_GetSGMode");
bool globalSGMode = false;
bool LAi_GetSGMode()
{
	return globalSGMode;
}

bool LAi_IsBottleWork(aref chr)
{
	if(CheckAttribute(chr, "chr_ai.hp_bottle"))
	{
		if(stf(chr.chr_ai.hp_bottle) > 0.0) return true;
	}
	return false;
}

//Найти в близи врагов
bool LAi_CanNearEnemy(aref chr, float radius)
{
	int num = FindNearCharacters(chr, radius, -1.0, -1.0, 0.001, false, false);
	if(num <= 0) return false;
	for(int i = 0; i < num; i++)
	{
		int idx = sti(chrFindNearCharacters[i].index);
		if(LAi_group_IsEnemy(chr, &Characters[idx])) return true;
	}
	return false;
}
// boal dead can be searched 14.12.2003 -->
object	Dead_Characters[100];
int     Dead_Char_num = 0;

void Dead_AddLoginedCharacter(aref chr)
{
	float   x, y, z;
	ref     chref, rItem;
	ref     mchr = GetMainCharacter();
	aref    arFromChar;
	aref    arToChar;
	float     nLuck   = GetCharacterSkillToOld(GetMainCharacter(), SKILL_FORTUNE);
    string  itemID;
    int     value, count;
    aref    typeRef;
	if(GetCharacterPos(chr, &x, &y, &z) == false || Dead_Char_num >= (100)){
		return;
	}
	Dead_Characters[Dead_Char_num].id = chr.id;
	Dead_Characters[Dead_Char_num].index = chr.index;
	Dead_Characters[Dead_Char_num].name     = chr.name;
	Dead_Characters[Dead_Char_num].lastname = chr.lastname;
	if (CheckAttribute(chr, "faceid"))
	{
		Dead_Characters[Dead_Char_num].faceid   = chr.faceid;
	}
	chref = &Dead_Characters[Dead_Char_num];
	DeleteAttribute(chref, "items");
	DeleteAttribute(chref, "Money");
	if (IsOfficer(chr) || CheckAttribute(chr, "SaveItemsForDead"))// для офицеров и НПС сохраняем все вещи
	{
		chref.Money = chr.Money;
		makearef(arToChar, chref.items);
		makearef(arFromChar, chr.items);
		CopyAttributes(arToChar, arFromChar);
		DeleteAttribute(chref,"items.unarmed"); // 18.06.05 убираем безоружность с трупа офицера
		DelBakSkillAttr(mchr); // boal оптимизация скилов
		ClearCharacterExpRate(mchr);
		DeleteAttribute(chr, "SaveItemsForDead");// убрать чтоб не было случайно потом

		string delItem;

		//BlackThorn - залоченные пушки лутать нельзя
		if (CheckAttribute(chr, "DontChangeBlade"))
		{
			itemID = chr.equip.blade;
			count = GetCharacterItem(chref, itemID);
			RemoveItems(chref, itemID, count);
		}
		if (CheckAttribute(chr, "DontChangeGun") && CheckAttribute(chr, "equip.gun"))
		{
			itemID = chr.equip.gun;
			count = GetCharacterItem(chref, itemID);
			RemoveItems(chref, itemID, count);
		}
		//Sinistra - еду на костёр
		if (CheckAttribute(chr, "DeleteFood"))
		{
			TakeNItems(chref, "food1", -10);
			TakeNItems(chref, "food2", -10);
			TakeNItems(chref, "food3", -10);
			TakeNItems(chref, "food4", -10);
			TakeNItems(chref, "food5", -10);
		}

		// Генерим предметы
		for(value = 0; value < ITEMS_QUANTITY; value++)
		{
			itemID = Items[value].ID;
			if(!CheckAttribute(Items[value], "DontDrop") && IsGenerableItem(itemID) && CheckCharacterItem(chr, itemID)) {
				count = GetCharacterItem(chr, itemID);
				RemoveItems(chr, itemID, count); // Забираетм обычные
				GenerateAndAddItems(chr, itemID, count); // Даем сгенеренные
			}
		}
	}
	else
	{
		// матрос с ЧЖ -->
		if (CheckAttribute(chr, "GhostShipCrew"))
		{
			// обыск скелетов давал вылет, даём сразу в ГГ
			TakeNItems(pchar, "Coins", Rand(5) + 1);
			Log_Info(GetConvertStrWithReplace("Variable_LAi_utils_10", "Logs.txt", "#space#", " "));
		}
		else
		// матрос с ЧЖ <--
		{
			if (makeint(nLuck + 0.5) > rand(15)) {
				chref.Money = rand(9) + rand(makeint(nLuck))*5 + rand(makeint(nLuck))*20;
			}
			int iRand = rand(MOD_SKILL_ENEMY_RATE*3 + 7 - makeint(GetCharacterSPECIALSimple(mchr, SPECIAL_L)/3));//выдаем либо пистоль либо саблю - матрос 12+12%, кэп 9+9%, ад 7+7%
			// вернем саблю и пистоль -->
			if(iRand == 1 && CheckAttribute(chr, "equip.blade") &&  chr.equip.blade != "unarmed")//
			{
				rItem = ItemsFromID(chr.equip.blade);
				if(frand(1.0) <= (stf(rItem.rare)*100) && CheckAttribute(rItem,"quality") && rItem.quality != "excellent" && !CheckAttribute(rItem, "DontDrop"))//фильтр rare оружия. >=0.01 - без фильтра //краснуху не дропать
				{	// Даем трупу сгенеренное оружие
					if(rItem.quality == "poor")
					{
						AddItems(chref, GetGeneratedItem(chr.equip.blade), 1);
					}
					if(rItem.quality == "ordinary" && sti(mchr.rank) >= 5)
					{
						AddItems(chref, GetGeneratedItem(chr.equip.blade), 1);
					}
					if(rItem.quality == "good" && sti(mchr.rank) >= (12+MOD_SKILL_ENEMY_RATE))//13...15
					{
						AddItems(chref, GetGeneratedItem(chr.equip.blade), 1);
					}
				}
				if(HasSubStr(rItem.id, "blade_01") || HasSubStr(rItem.id, "blade_02") || HasSubStr(rItem.id, "topor_01") || HasSubStr(rItem.id, "topor_05"))
				{
					RemoveItems(chref, rItem, 100);
				}
			}
			if (iRand == 2 && CheckAttribute(chr, "equip.gun"))
			{
				rItem = ItemsFromID(chr.equip.gun);
				if(CheckAttribute(rItem,"quality") && rItem.quality != "excellent") {//краснуху не дропать
					if(rItem.quality == "good" && rand(4) == 0) {//20%
						GiveItem2Character(chref, chr.equip.gun);
					}
					else {
						GiveItem2Character(chref, chr.equip.gun);
					}
				}
				bool rifleman = CheckAttribute(chr, "NpcMusket") || CheckAttribute(chr, "BoardingMusket");
				float fAmmoNerf = 2.0*rifleman;//делитель пуль в дропе
				TakeNItems(chref, "bullet", makeint(GetCharacterItem(chr, "bullet")/(1.0+fAmmoNerf)));// boal gun bullet
				TakeNItems(chref, "grapeshot", makeint(GetCharacterItem(chr, "grapeshot")/(1.0+fAmmoNerf)));// картечь
				TakeNItems(chref, "cartridge", makeint(GetCharacterItem(chr, "cartridge")/(1.0+fAmmoNerf)));// картечь
				TakeNItems(chref, "GunPowder", makeint(GetCharacterItem(chr, "GunPowder")/(1.0+fAmmoNerf))); // 21.03.09 Warship fix Порох
			}
			// вернем саблю и пистоль <--
			//fill box with new items
			int j, howI;
			ref    itm;
			string name = "";
			string  sModel = chr.model;
			howI = 0;
			// выясняем тип -->
			if (chr.sex != "man") {
				name = "Citizen_f";
			}
			else if (findsubstr(sModel, "off_" , 0) != -1) {
				name = "Solder_o";
			}

			if (name == "") {
				name = GetCharType(chr);
			}
			// выясняем тип <--

			// рандом дроп всякого шрота // нужно оптимизировать!!! 137 - это много. начало с опред места
			float frandom;
			for (j= ItemDeadStartCount; j<ITEMS_QUANTITY; j++){
				makeref(itm,Items[j]);
				//чек типа трупа
				if (!CheckAttribute(itm, name)) {
					continue;
				}
				makearef(typeRef, itm.(name));
				if (!CheckAttribute(typeRef, "rare")) {
					trace("Dead_AddLoginedCharacter error: Не найден RARE для предмета = "+itm.id+" тип = "+name);
				}

				//дроп рейт каждого итема при rare=1: 40,7%. удача х1.16, везение х1.45, сложность x0.33 (на "капитан" == ванилла гпк)
				frandom = frand(99 - 2*GetCharacterSPECIALSimple(mchr, SPECIAL_L));
				if (frandom < (37.2*stf(typeRef.rare)*(1 + 0.05*nLuck) / MOD_SKILL_ENEMY_RATE)) {
					value = sti(typeRef.min);
					value += rand(sti(typeRef.max) - value);
					itemID = itm.id;
					if (itemID == "indian18") {value = 0;}
					chref.items.(itemID) = value;
				}
			}
			if(CheckAttribute(chr, "Frenzy") || CheckAttribute(chr, "FrenzyActive"))
			{
				if (LAi_IsBoardingProcess() && CheckAttribute(boarding_enemy, "id") && boarding_enemy.id == "pirateVikingQuest_Captain") howI = 1 > rand(10);
				else howI = sti(pchar.SPECIAL.Luck) > rand(100);
				if (CheckAttribute(chref, "items.StrangePotion")) chref.items.StrangePotion = sti(chref.items.StrangePotion) + howI;
				else chref.items.StrangePotion = howI;
			}
		}
		// заполнили по списку
	}
	Dead_Characters[Dead_Char_num].px = x;
	Dead_Characters[Dead_Char_num].pz = z;
	Dead_Char_num++;
	SendMessage(chr, "ll", MSG_CHARACTER_SET_CAPTURE_TEXT1_FONT_COLOR, argb(255,85,85,85));
	SendMessage(chr, "ll", MSG_CHARACTER_SET_CAPTURE_TEXT2_FONT_COLOR, argb(255,85,85,85));
	SendMessage(chr, "ll", MSG_CHARACTER_SET_CAPTURE_TEXT3_FONT_COLOR, argb(255,85,85,85));
}
void Dead_DelLoginedCharacter(aref chr)
{
	aref arToChar;
	aref arFromChar;
	ref c1, c2;
    for(int i = 0; i < Dead_Char_num; i++)
	{
		if(CheckAttribute(chr, "index") && Dead_Characters[i].index == chr.index)
		{
            Dead_Characters[i].px = 30000; // локация -2000 до 2000
			Dead_Characters[i].pz = 30000;
		}
	}
	if(Dead_Char_num < 0) Dead_Char_num = 0;
}

void Dead_RefreshBodyPosition(int slot)
{
    if (!CheckAttribute(&Dead_Characters[slot], "index")) return;
    if (!CheckAttribute(&Dead_Characters[slot], "id")) return;
    if (stf(Dead_Characters[slot].px) >= 30000.0 || stf(Dead_Characters[slot].pz) >= 30000.0) return;
    int index = sti(Dead_Characters[slot].index);
    if (index < 0 || index >= TOTAL_CHARACTERS) return;
    ref body = GetCharacter(index);
    if (!IsEntity(body)) return;
    if (body.id != Dead_Characters[slot].id) return;
    float x, y, z;
    if (SendMessage(body, "lseee", MSG_CHARACTER_EX_MSG, "GetRagdollPosition", &x, &y, &z))
    {
        Dead_Characters[slot].px = x;
        Dead_Characters[slot].pz = z;
    }
}

int Dead_FindCloseBody()
{
	ref mainChr = GetMainCharacter();
	int dchr_index = -1;
	float x, y, z;
	//#20180524-01 Remove unneeded sqrt, was 100, now 100^2
	float x1, y1, z1, rd, min_rd = 10000; //100; // 100 - is super max
	GetCharacterPos(mainChr, &x, &y, &z);

    for(int i = 0; i < Dead_Char_num; i++)
	{
	    Dead_RefreshBodyPosition(i);
	    x1 = Dead_Characters[i].px;
		z1 = Dead_Characters[i].pz;
		// без sqrt: сравниваем квадраты расстояний
		rd = sqr(x1 - x) + sqr(z1 - z);
		if ( rd < 2.25) //<-- was 1.5, now 1.5^2 //JA was 1.2
		{
		    if (min_rd > rd)
		    {
		        min_rd = rd;
                dchr_index = i;
		    }
		}
	}
	return dchr_index;
}

void Dead_OpenBoxProcedure()
{
	ref chr = GetMainCharacter();
	int dchr_index;
    ref deadCh;
    dchr_index = Dead_FindCloseBody();
    if (dchr_index == -1) return;

	deadCh = &Dead_Characters[dchr_index];

	Dead_LaunchCharacterItemChange(deadCh);
}

void Dead_LaunchCharacterItemChange(ref chref)
{
    //#20200217-02
    if(CheckAttribute(pchar, "chr_ai.type.state") && pchar.chr_ai.type.state == "follow")
	{
         Log_SetStringToLog(xiStr("CantNow"));
         return;
    }
	if(procInterfacePrepare(INTERFACE_ITEMSBOX))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSBOX;
		aref charef; makearef(charef,chref);
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,&charef, "666");
	}
}

// boal dead can be searched 14.12.2003 <--

void MakePoisonAttack(aref attack, aref enemy, int iQuantity)
{
	if(enemy.sex == "skeleton" || CheckAttribute(enemy, "PoisonImmune") || IsEquipCharacterByArtefact(enemy, "talisman8"))
	{
		if (enemy.index == GetMainCharacterIndex()) Log_SetStringToLog(GetConvertStrWithReplace("Variable_LAi_utils_11", "Logs.txt", "#space#", " "));
		else Log_SetStringToLog(GetConvertStrWithReplace("Variable_LAi_utils_12", "Logs.txt", "#space#", " ") + GetFullName(enemy) + GetConvertStrWithReplace("Variable_LAi_utils_13", "Logs.txt", "#space#", " "));
		return;
	}

	if (!CheckAttribute(enemy, "chr_ai.poison"))
	{
		if (enemy.index == GetMainCharacterIndex()) Log_SetStringToLog(XI_ConvertString("You've been poisoned"));
		else Log_SetStringToLog(GetConvertStrWithReplace("Variable_LAi_utils_14", "Logs.txt", "#space#", " ") + GetFullName(enemy) + GetConvertStrWithReplace("Variable_LAi_utils_15", "Logs.txt", "#space#", " "));
	}
	if (attack.index == GetMainCharacterIndex()) // Проводим все махинации с флаконами и счетчиками, если только атакующий - игрок
	{
		int poisoncount = sti(pchar.questTemp.poisoncount) + 1;
		pchar.questTemp.poisoncount = poisoncount;

		// Открываем достижения
		CheckAndUnlock("poisons", poisoncount);
	}
	//Отравляем персонажа
	float poison = 0.0;
	if(CheckAttribute(enemy, "chr_ai.poison"))
	{
		poison = stf(enemy.chr_ai.poison);
		if(poison < 1.0) poison = 1.0;
	}
	enemy.chr_ai.poison = poison + iQuantity;
	// LEO: 3D-квестмарка убрана - эффекты показывают иконки над баром
}

int LAi_CalcPoisonChance(ref attack)
{
	float coeff = 0.0;
	if (attack.sex == "skeleton" || attack.sex == "crab" || HasSubStr(attack.model, "Canib_") || HasSubStr(attack.model, "PGG_Chani")) {
		coeff = 15.0;
	}
	return (sti(attack.chr_ai.special.valueP) + MakeInt(coeff));
}

float LAi_CalcPoisonDuration(bool bIsRandomOn)
{
	return (50.0 + bIsRandomOn * (rand(POISON_TIME_RAND) + rand(POISON_TIME_RAND)));
}

void MakePoisonAttackCheckSex(aref attack, aref enemy)
{
	if (!pnrand(attack, LAi_CalcPoisonChance(attack), "poison")) {//основной чек 
		return;
	}
	MakePoisonAttack(attack, enemy, LAi_CalcPoisonDuration(1));
}

string LAi_FindFreeRandomLocator(string group)
{
	if(IsEntity(loadedLocation) != true) return "";
	string at = "locators." + group;
	if(CheckAttribute(loadedLocation, at) == 0) return "";
	aref grp;
	makearef(grp, loadedLocation.(at));
	int num = GetAttributesNum(grp);
	if(num <= 0) return "";
	int base = rand(num - 1);
	for(int i = 0; i < num; i++)
	{
		int index = base + i;
		if(index >= num)
		{
			index = index - num;
		}
		aref loc = GetAttributeN(grp, index);
		//Boyer change #20170318-38
		if(!CheckAttribute(loc, "x"))
            continue;
		float lx = stf(loc.x);
		float ly = stf(loc.y);
		float lz = stf(loc.z);
		if(CheckLocationPosition(loadedLocation, lx, ly, lz))
		{
			return GetAttributeName(GetAttributeN(grp, index));
		}
	}
	return "";
}

bool LAi_CheckLocatorFree(string _group, string _locator)
{
	if(!IsEntity(loadedLocation)) return false;
	string at = "locators." + _group + "." + _locator;
	if(!CheckAttribute(loadedLocation, at)) return false;
	aref grp;
	makearef(grp, loadedLocation.(at));
	if (!CheckAttribute(grp, "x") || !CheckAttribute(grp, "y") || !CheckAttribute(grp, "z"))
		{trace("ERROR: cannot find xyz position for '" + at + "'"); return false;}
	float lx = stf(grp.x);
	float ly = stf(grp.y);
	float lz = stf(grp.z);
	if(CheckLocationPosition(loadedLocation, lx, ly, lz))
	{
		return true;
	}
	return false;
}

void LaunchSparks(aref chr);
{
	float x, y, z, ay, xc, yc, zc, spr;
	GetCharacterPos(chr, &x, &y, &z);
	GetCharacterAy(chr, &ay);
	spr = frandsmall(1.0);
	if (chr.sex == "woman")
	{
		if (chr.model.animation != "Danielle")
		{
			xc = - 0.21 + 0.15 * spr;
			yc = 1.65 - 0.25 * spr;
			zc = 0.48;
		}
		else
		{
			xc = - 0.47 + 0.38 * spr;
			yc = 1.35 - 0.1 * spr;
			zc = 0.45;
		}
	}
	else
	{
		xc = - 0.38 + 0.4 * spr;
		yc = 1.35 - 0.05 * spr;
		zc = 0.5 + 0.1 * spr;
	}
	x += xc * cos(-ay) - zc * sin(-ay);
	y += yc;
	z += xc * sin(-ay) + zc * cos(-ay);
	CreateParticleSystem("impact_sparks", x, y, z, 0,1.0,0,0);
}

// LEO: снимок вторичных целей сразу после поиска. chrFindNearCharacters - ГЛОБАЛЬНЫЙ массив на
//   весь мод, а поиск и обработка разнесены по файлам: между ними смерть/опыт/реакция группы
//   успевают его перетереть, а счётчик остаётся старым - урон уходил по ЧУЖИМ целям.
//   Держим снимок во ВРЕМЕННОМ атрибуте, а не в глобалке: глобалки пишутся в сейв.
void CollateralSnapshot()
{
	int i;
	int idx;
	string sKey;
	DeleteAttribute(pchar, "questTemp.CollIdx");
	for (i = 0; i < iCollateralTargetsCount; i++)
	{
		idx = -1;
		if (CheckAttribute(chrFindNearCharacters[i], "index")) idx = sti(chrFindNearCharacters[i].index);
		sKey = "n" + i;// ключ строкой: числовой индекс в .(...) Storm понимает не так, как ожидаешь
		pchar.questTemp.CollIdx.(sKey) = idx;
	}
}

// индекс вторичной цели из снимка (-1, если её там нет)
int CollateralIdx(int n)
{
	string sKey = "n" + n;
	if (!CheckAttribute(pchar, "questTemp.CollIdx." + sKey)) return -1;
	return sti(pchar.questTemp.CollIdx.(sKey));
}

// LEO: единый гейт дымного следа пули: тумблер сборки + опция настроек + общий тумблер частиц.
//   Опция int 0/1 (не bool - sti("true") в Storm даёт 0), поэтому молчим только при явном нуле.
bool GunTrail_Enabled()
{
	if (GUNTRAIL_ENABLE == 0) return false;
	if (!sti(InterfaceStates.OtherParticles)) return false;
	if (CheckAttribute(&InterfaceStates, "GunTrail"))
	{
		if (sti(InterfaceStates.GunTrail) == 0) return false;
	}
	return true;
}

// LEO: сколько следов рисовать на выстрел. Картечь и ежовые снаряды - горсть дроби, а не одна пуля.
int GunTrail_PelletCount(string sBullet)
{
	if (sBullet == "grapeshot") return GUNTRAIL_BUCKSHOT_N;
	if (sBullet == "GunEchin") return GUNTRAIL_BUCKSHOT_N;
	return 1;
}

// LEO: дымный след пули - считаем дуло и точку, где пуля закончила путь, остальное рисует движок
//   (сообщение "ShotTrail" -> LocationEffects). Наземная стрельба хитскановая, летящего снаряда нет,
//   да он и не нужен: пуля покрывает боевую дистанцию быстрее кадра, полоска ставится разом.
//   При промахе линия уводится вбок и продлевается за цель - видно, куда ушла пуля. Ручки в _mod_on_off.h
//   Координаты приходят снаружи, а не снимаются здесь: смертельный выстрел убивает цель
//   ещё внутри расчёта урона, и к моменту рисования следа её позицию уже не спросить.
//   Картечь и ежи бьют горстью дроби - для них рисуем веер следов вместо одного.
void LaunchGunTrail(float xa, float ya, float za, float xe, float ye, float ze, bool bHit, string sBullet, int iMultiShot)
{
	if (GunTrail_Enabled()) LaunchGunTrailImpl(xa, ya, za, xe, ye, ze, bHit, sBullet, iMultiShot, false);
}

void LaunchGunTrailToImpact(ref shooter, float xe, float ye, float ze, string bullet, int shots)
{
	if (!GunTrail_Enabled()) return;
	float xa, ya, za;
	if (GetCharacterPos(shooter, &xa, &ya, &za))
		LaunchGunTrailImpl(xa, ya, za, xe, ye, ze, true, bullet, shots, true);
}

void LaunchGunTrailImpl(float xa, float ya, float za, float xe, float ye, float ze, bool bHit, string sBullet, int iMultiShot, bool exact)
{
	float dx, dz, fLen, fSide, fAng, fK, px, pz, fSpread;
	int i, iPellets;

	ya = ya + GUNTRAIL_MUZZLE_Y;
	if (!exact) ye = ye + GUNTRAIL_TARGET_Y;

	dx = xe - xa;
	dz = ze - za;
	fLen = sqrt(dx * dx + dz * dz);
	if (fLen < GUNTRAIL_MIN_DIST) return;// в упор след не читается - его перекрывает дульный дым
	dx = dx / fLen;
	dz = dz / fLen;

	if (!bHit)// промах - пуля идёт мимо цели и дальше за неё
	{
		fSide = GUNTRAIL_MISS_SIDE;
		if (rand(1) == 0) fSide = -fSide;
		xe = xe + dx * GUNTRAIL_MISS_OVER - dz * fSide;
		ze = ze + dz * GUNTRAIL_MISS_OVER + dx * fSide;
		ye = ye + frandsmall(2.0 * GUNTRAIL_MISS_SIDE) - GUNTRAIL_MISS_SIDE;
	}

	xa = xa + dx * GUNTRAIL_MUZZLE_FWD;// стартуем от дула, а не из центра стрелка
	za = za + dz * GUNTRAIL_MUZZLE_FWD;

	iPellets = GunTrail_PelletCount(sBullet);
	// пули из разных стволов идут почти параллельно, дробь - настоящим конусом
	fSpread = GUNTRAIL_BUCKSHOT_SPREAD;
	if (iPellets < 2) fSpread = GUNTRAIL_BUCKSHOT_SPREAD * GUNTRAIL_BARREL_SPREAD;
	// картечница бьёт всеми стволами, двустволка - дуплетом: каждый ствол шлёт свой заряд
	if (iMultiShot > 1) iPellets = iPellets * iMultiShot;
	if (iPellets > GUNTRAIL_MAX_TRAILS) iPellets = GUNTRAIL_MAX_TRAILS;

	if (iPellets < 2 || exact)
	{
		SendMessage(loadedLocation, "lsfffffff", MSG_LOCATION_EX_MSG, "ShotTrail", xa, ya, za, xe, ye, ze, 0.0);
		if (iPellets < 2) return;
	}

	// веер: каждый заряд со своим отворотом и своей длиной полёта
	if (exact) iPellets = iPellets - 1;
	for (i = 0; i < iPellets; i++)
	{
		fAng = frandsmall(2.0 * fSpread) - fSpread;
		fK = 1.0 - GUNTRAIL_BUCKSHOT_LEN * 0.5 + frandsmall(GUNTRAIL_BUCKSHOT_LEN);
		px = xa + (dx * cos(fAng) - dz * sin(fAng)) * fLen * fK;
		pz = za + (dx * sin(fAng) + dz * cos(fAng)) * fLen * fK;
		SendMessage(loadedLocation, "lsfffffff", MSG_LOCATION_EX_MSG, "ShotTrail", xa, ya, za,
			px, ye + frandsmall(GUNTRAIL_MISS_SIDE) - GUNTRAIL_MISS_SIDE * 0.5, pz, GUNTRAIL_BUCKSHOT_STEP);
	}
}

// LEO: осколки гранаты/петарды - короткие дымные хвосты из точки разрыва во все стороны.
void LaunchFragTrails(aref chr, int iCount)
{
	float x, y, z, fAng;
	int i;

	if (!GunTrail_Enabled()) return;

	GetCharacterPos(chr, &x, &y, &z);
	y = y + GUNTRAIL_TARGET_Y;

	for (i = 0; i < iCount; i++)
	{
		fAng = frandsmall(6.2832);
		SendMessage(loadedLocation, "lsfffffff", MSG_LOCATION_EX_MSG, "ShotTrail", x, y, z,
			x + sin(fAng) * GUNTRAIL_FRAG_DIST,
			y + GUNTRAIL_FRAG_UPMIN + frandsmall(GUNTRAIL_FRAG_UPADD),
			z + cos(fAng) * GUNTRAIL_FRAG_DIST, GUNTRAIL_FRAG_STEP);
	}
}

// LEO: искры от пули по кирасе. Ставим на груди цели, сдвигая навстречу стрелку - туда, куда пришла пуля.
//   Гейт общий с искрами блока клинком, это родственный эффект, а не часть следа.
void LaunchCirassSparks(aref attack, aref enemy)
{
	float xa, ya, za, xe, ye, ze, dx, dz, fLen;

	if (!sti(InterfaceStates.OtherParticles)) return;

	GetCharacterPos(attack, &xa, &ya, &za);
	GetCharacterPos(enemy, &xe, &ye, &ze);
	dx = xa - xe;
	dz = za - ze;
	fLen = sqrt(dx * dx + dz * dz);
	if (fLen > 0.01)
	{
		dx = dx / fLen;
		dz = dz / fLen;
	}

	CreateParticleSystem("impact_sparks", xe + dx * CIRASS_SPARK_FWD, ye + CIRASS_SPARK_Y, ze + dz * CIRASS_SPARK_FWD, 0.0, 1.0, 0.0, 0);
}

// LEO: видимый рикошет от кирасы - искры плюс дымный отскок пули в сторону.
//   Вызывается вместо урона (рикошет обрывает расчёт), поэтому цель заведомо жива и её позиция валидна.
void LaunchCirassRicochet(aref attack, aref enemy)
{
	float xa, ya, za, xe, ye, ze;
	float dx, dz, fLen, fAng, rx, rz;

	LaunchCirassSparks(attack, enemy);

	if (!GunTrail_Enabled()) return;

	GetCharacterPos(attack, &xa, &ya, &za);
	GetCharacterPos(enemy, &xe, &ye, &ze);
	dx = xe - xa;
	dz = ze - za;
	fLen = sqrt(dx * dx + dz * dz);
	if (fLen < 0.01) return;
	dx = dx / fLen;
	dz = dz / fLen;
	ye = ye + GUNTRAIL_TARGET_Y;

	// пуля чиркнула по броне и ушла в сторону - широко разворачиваем направление полёта
	fAng = GUNTRAIL_RICO_ANGMIN + frandsmall(GUNTRAIL_RICO_ANGADD);
	if (rand(1) == 0) fAng = -fAng;
	rx = dx * cos(fAng) - dz * sin(fAng);
	rz = dx * sin(fAng) + dz * cos(fAng);

	SendMessage(loadedLocation, "lsfffffff", MSG_LOCATION_EX_MSG, "ShotTrail", xe, ye, ze,
		xe + rx * GUNTRAIL_RICO_DIST, ye + GUNTRAIL_RICO_UP + frandsmall(GUNTRAIL_RICO_UP), ze + rz * GUNTRAIL_RICO_DIST, 0.0);
}

// LEO: след при стрельбе БЕЗ цели - пуля уходит туда, куда развёрнут стрелок.
//   Обработчик выстрела в этом случае выходит рано, поэтому зовётся отдельно, до его return.
void LaunchGunTrailFree(aref attack, string sBullet, int iMultiShot)
{
	float x, y, z, ay, xe, ze;

	if (!GunTrail_Enabled()) return;

	GetCharacterPos(attack, &x, &y, &z);
	GetCharacterAy(attack, &ay);
	xe = x + sin(ay) * GUNTRAIL_FREE_DIST;
	ze = z + cos(ay) * GUNTRAIL_FREE_DIST;

	// дальше считает LaunchGunTrail - там и вынос от дула, и веер дроби, и стволы залпового оружия.
	// Координаты отдаём сырые (от ног): высоты дула и цели она поднимает сама, а разница высот
	// в конечной точке выравнивает линию в горизонт - в пустоту стрелок бьёт прямо перед собой.
	LaunchGunTrail(x, y, z, xe, y + GUNTRAIL_MUZZLE_Y - GUNTRAIL_TARGET_Y, ze, true, sBullet, iMultiShot);
}

bool LAi_IsBoneOnlySkeleton(aref chr)
{
	if (CheckAttribute(chr, "sex") && HasSubStr(GetStrSmallRegister(chr.sex), "skeleton")) return true;
	if (CheckAttribute(chr, "model.animation") && HasSubStr(GetStrSmallRegister(chr.model.animation), "skeleton")) return true;
	if (CheckAttribute(chr, "model") && HasSubStr(GetStrSmallRegister(chr.model), "skelet")) return true;
	return false;
}

// LEO: Координаты сохранены до смертельного урона, пока персонаж ещё отдавал позицию.
void LAi_AddExplosionKillGroundBlood(aref chr, float x, float y, float z)
{
	if (!LAi_IsDead(chr)) return;
	if (LAi_IsBoneOnlySkeleton(chr)) return;
	if (loadedLocation.type == "underwater") return;
	SendMessage(loadedLocation, "lsfff", MSG_LOCATION_EX_MSG, "AddGoreBlood", x, y + 0.1, z);
}

void LaunchBlood(aref chr, float addy, bool isBig)
{
	if (LAi_IsBoneOnlySkeleton(chr)) return;
	float x, y, z;
	GetCharacterPos(chr, &x, &y, &z);
	if (loadedLocation.type == "underwater")
	{
		y = y + 0.9;
		CreateParticleSystem("bloodUnderwater", x, y, z, 0,1.0,0,0);
	}
	else
	{
		y = y + addy;
		if(isBig == true)
		{
			CreateParticleSystem("blood_big", x, y, z, 0,1.0,0,0);
		}
		else
		{
			CreateParticleSystem("blood", x, y, z, 0,1.0,0,0);
		}
		SendMessage(loadedLocation, "lsfff", MSG_LOCATION_EX_MSG, "AddBlood", x,y,z);
	}
}

void LaunchBlast(aref chr) // 280812
{
	float x, y, z;
	GetCharacterPos(chr, &x, &y, &z);
	y = y + 0.5;
	CreateParticleSystem("blast_inv", x, y, z, 0,1.0,0,0);
}

void LaunchBlastPellet(aref chr, bool bMainTarget, bool knockback) // Петарда
{
	float x, y, z;
	GetCharacterPos(chr, &x, &y, &z);
	y = y + 0.5;
	if (bMainTarget)
	{
		if (knockback) SendMessage(chr, "lffff", MSG_BLAST_GRENADE, 1.0, 0.7, 5.4, 5.0);
		CreateParticleSystem("pellet_expl", x, y, z, 0,1.0,0,0);
		CreateParticleSystem("blast_pellet_ptr", x, y, z, 0,1.0,0,0);
		Play3DSound("powder_pellet", x, y, z);
		Play3DSound("Fx_Stan", x, y, z);
		LaunchFragTrails(chr, GUNTRAIL_FRAG_PELLET);// LEO: дымные хвосты осколков из точки разрыва
	}
	if (LAi_IsBoneOnlySkeleton(chr)) return;
	y = y + 0.2;
	CreateParticleSystem("blood_expl_grenade", x, y, z, 0,1.0,0,0);
	CreateParticleSystem("blood_big", x, y, z, 0,1.0,0,0);
}

void LaunchBlastGrenade(aref chr, bool bMainTarget) // Граната
{
	float x, y, z;
	GetCharacterPos(chr, &x, &y, &z);
	y = y + 0.5;
	if (bMainTarget)
	{
		SendMessage(chr, "lffff", MSG_BLAST_GRENADE, 1.0, 0.7, 10.8, 10.0);
		CreateParticleSystem("grenade_expl", x, y, z, 0,1.0,0,0);
		Play3DSound("grenade", x, y, z);
		Play3DSound("Fx_Ricochet", x, y, z);
		Play3DSound("Fx_Krovotok", x, y, z);
		LaunchFragTrails(chr, GUNTRAIL_FRAG_GRENADE);// LEO: дымные хвосты осколков из точки разрыва
	}
	if (LAi_IsBoneOnlySkeleton(chr)) return;
	y = y + 0.2;
	CreateParticleSystem("blood_expl_grenade", x, y, z, 0,1.0,0,0);
	CreateParticleSystem("blood_big", x, y, z, 0,1.0,0,0);
	y = y + 0.4;
	CreateParticleSystem("blood_expl_grenade", x, y, z, 0,1.0,0,0);
	CreateParticleSystem("blood_big", x, y, z, 0,1.0,0,0);
	y = y + 0.5;
	CreateParticleSystem("blood_expl_grenade", x, y, z, 0,1.0,0,0);
	CreateParticleSystem("blood_big", x, y, z, 0,1.0,0,0);
}

void LAi_Explosion(ref chr, int damage)
{
	float x, y, z;
	GetCharacterPos(chr, &x, &y, &z);
	// LEO: было blood_shoot - схемы нет в ресурсах, движок ругался на каждый выстрел
	if (!LAi_IsBoneOnlySkeleton(chr)) CreateParticleSystem("blood_expl_grenade", x, y, z, 0,1.0,0,0);
	if (InterfaceStates.FireParticles) CreateParticleSystemX("cancloud_fire_ext", x, y, z, x, y, z, 20);
	else CreateParticleSystemX("cancloud_fire", x, y, z, x, y, z, 20);

	PlayStereoSound("Sea Battles\cannon_fire_03.wav");

	int num = FindNearCharacters(chr, 3.0, -1.0, -1.0, 0.001, false, true);

	for(int j = 0; j <= num; j++)
	{
		int idx = sti(chrFindNearCharacters[j].index);
		ref findCh;
		findCh = GetCharacter(idx);

		float dist = -1.0;
		if(GetCharacterDistByChr3D(chr, findCh, &dist) && dist < 4)
		{
			LAi_ApplyCharacterDamage(findCh, damage);
			if (sti(LAi_GetCharacterHP(findCh)) < damage + 1)
			{
				if(findCh.chr_ai.group != LAI_GROUP_PLAYER)
				{
					Lai_KillCharacter(findCh);
				}
				else
				{
					findCh.chr_ai.hp = 10;
				}
			}
		}
	}

	LAi_ApplyCharacterDamage(chr, damage);
	if (sti(LAi_GetCharacterHP(chr)) < damage + 1) Lai_KillCharacter(chr);
}

//Gregg
void MakeBloodingAttack(aref enemy, aref attack, float coeff) // Кровоточащая атака
{
	float Blooding = 0.0;
	if(CheckAttribute(enemy, "chr_ai.Blooding")) {
		enemy.chr_ai.Blooding.Power = sti(enemy.chr_ai.Blooding.Power) + 1;
		Blooding = stf(enemy.chr_ai.Blooding);
		if(Blooding < 1.0) Blooding = 1.0;
	}
	else enemy.chr_ai.Blooding.Power = 1;
	enemy.chr_ai.Blooding = Blooding + coeff; // Продолжительность 5+(от 0 до коэфф*5)
	// LEO: 3D-квестмарка убрана - эффекты показывают иконки над баром
	enemy.chr_ai.Blooding.BlooderIDx = attack.index;

	if (enemy.index == GetMainCharacterIndex()) {
		AddCharacterHealth(enemy, -0.5); // Кровотечение портит здоровье
	}
	return;
}

void MakeSwiftAttack(aref enemy, aref attacked, float coeff) // Резкий удар
{
	float Swift = 0.0;
	enemy.chr_ai.curen = stf(enemy.chr_ai.energy);
	if(CheckAttribute(enemy, "chr_ai.Swift")) {
		Swift = stf(enemy.chr_ai.Swift);
		if(Swift < 1.0) { Swift = 1.0; }
	}
	enemy.chr_ai.Swift = Swift + coeff; // Продолжительность 1+(от 0 до 4)+коэфф
	// LEO: 3D-квестмарка убрана - эффекты показывают иконки над баром
	return;
}

int CalcStunDuration(bool bIsRandomOn)
{
	return (1 + bIsRandomOn * rand(STUN_TIME_RAND));
}

void MushketStun(aref enemy) // Мушкетный стан - Gregg
{
	LAi_LockFightMode(enemy, true);
	if(enemy.chr_ai.type == "officer") {
		enemy.chr_ai.backuptype = enemy.chr_ai.type; // фикс слета группы у офицеров из-за двойного стана
	}
	LAi_SetActorTypeNoGroup(enemy);
	float understun = 0.0;
	if(CheckAttribute(enemy, "chr_ai.understun")) {
		understun = stf(enemy.chr_ai.understun);
		if(understun < 1.0) understun = 1.0;
	}
	enemy.chr_ai.understun = understun + CalcStunDuration(1); // Продолжительность 1+(от 0 до 2)
	// LEO: 3D-квестмарка убрана - эффекты показывают иконки над баром
	return;
}

// LEO: досрочно снять оглушение - и рукопашное (stuntime), и мушкетное (understun). Нужно везде, где
//   сбитого персонажа поднимают на разговор: оглушённый не подходит, а MushketStun вдобавок ЗАПИРАЕТ
//   боевой режим, и движковая SetFightMode выходит первой же строкой (character.cpp) - клинок так и
//   остаётся в ножнах. Тип персонажа вызывающий выставляет сам, сразу после.
void LAi_ClearStun(aref chr)
{
	DeleteAttribute(chr, "stuntime");
	if (CheckAttribute(chr, "chr_ai.understun"))
	{
		DeleteAttribute(chr, "chr_ai.understun");
		DeleteAttribute(chr, "chr_ai.backuptype");
		UnmarkStuckFXEffect(chr);
	}
	LAi_LockFightMode(chr, false);
}

int CalcTraumaDuration(bool bIsRandomOn)
{
	return (20 + bIsRandomOn * rand(TRAUMA_TIME_RAND));
}

void MakeTraumaAttack(aref enemy, aref attacked) // Травма
{
	float Trauma = 0.0;
	if(CheckAttribute(enemy, "chr_ai.Trauma"))
	{
		Trauma = stf(enemy.chr_ai.Trauma);
		if(Trauma < 1.0) Trauma = 1.0;
	}
	enemy.chr_ai.Trauma = Trauma + CalcTraumaDuration(1); // Продолжительность 20-60
	// LEO: 3D-квестмарка убрана - эффекты показывают иконки над баром
	return;
}
