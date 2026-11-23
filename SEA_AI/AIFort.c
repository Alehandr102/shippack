#include "storm-engine\sail_msg.h"

#define		MAX_FORTS						16
#define		MIN_CANNON_DAMAGE_DISTANCE		20.0

// LEO: splash-взрыв мортиры форта (локаторы mortar* -> слот Fort.Cannons.Type.3 = LBS48). Тюнинг здесь.
#define		FORT_MORTAR_AOE_RADIUS			80.0	// радиус splash от точки попадания, м (за границей урона нет)
#define		FORT_MORTAR_AOE_FULLDIST		10.0	// ближе этой дистанции splash-урон максимальный
#define		FORT_MORTAR_AOE_MAXDAMAGE		250.0	// урон по корпусу в эпицентре; к границе радиуса спадает до 0

object		AIFort;
object		Forts[MAX_FORTS];
object		FortsBlots[MAX_FORTS];
int			iNumForts = 0;
int iNumDamagedCannonsQuantity = 0;

void DeleteFortEnvironment()
{
    // fix boal -->
	DelEventHandler(FORT_CANNON_DAMAGE, "Fort_CannonDamage");
	DelEventHandler(FORT_CREATE, "Fort_CreateEvent");
	DelEventHandler(FORT_LOADDMGCANNON, "Fort_LoadDamagedCannon");
	// fix
	if (isEntity(&AIFort)) { DeleteClass(&AIFort); }
	for (int i=0; i<iNumForts; i++)
	{
		DeleteClass(&Forts[i]);
		DeleteClass(&FortsBlots[i]);
	}
	iNumForts = 0;
	for(i = 0; i < MAX_FORTS; i++) { Forts[i].fortcmdridx = -1; }
}

void CreateFortEnvironment()
{
	iNumForts = 0;
	for(int i = 0; i < MAX_FORTS; i++) { Forts[i].fortcmdridx = -1; }
	SetEventHandler(FORT_CANNON_DAMAGE, "Fort_CannonDamage", 0);
	SetEventHandler(FORT_CREATE, "Fort_CreateEvent", 0);
	SetEventHandler(FORT_LOADDMGCANNON, "Fort_LoadDamagedCannon", 0);
}

int Fort_FindCharacter(string sLocationID, string sLocationGroup, string sLocationLocator)
{
	for (int i=0; i<MAX_CHARACTERS; i++)
	{
		if (Characters[i].location == sLocationID)
		{
			if (Characters[i].location.group == sLocationGroup)
			{
				if (Characters[i].location.locator == sLocationLocator)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

int Fort_GetDeadDays(ref rCharacter)
{
	int		iDYear = sti(rCharacter.Fort.DieTime.Year);
	int		iDMonth = sti(rCharacter.Fort.DieTime.Month);
	int		iDDay = sti(rCharacter.Fort.DieTime.Day);
	float	fTime = stf(rCharacter.Fort.DieTime.Time);

	return GetPastTime("day", iDYear, iDMonth, iDDay, fTime, GetDataYear(), GetDataMonth(), GetDataDay(), GetTime());
}

void Fort_Login(int iIslandIndex)
{
	// search fort's in current island
	ref rIsland = GetIslandByIndex(iIslandIndex);
	aref arReload; makearef(arReload,rIsland.reload);
	int iNumReloads = GetAttributesNum(arReload);
	for (int i=0; i<iNumReloads; i++)
	{
		aref arLocator = GetAttributeN(arReload,i);
		ReloadProgressUpdate();
		if (CheckAttribute(arLocator,"fort"))
		{
			if (!CheckAttribute(arLocator,"fort.model") || arLocator.fort.model == "")
			{
				Trace("Island.id = " + rIsland.id + ", and reload." + GetAttributeName(arLocator) + ", doesn't have fort.model value.");
				continue;
			}
			CreateEntity(&Forts[iNumForts], "MODELR");
			string sFortModel = rIsland.filespath.models + "\" + arLocator.fort.model;


			SendMessage(&Forts[iNumForts], "ls", MSG_MODEL_SET_LIGHT_PATH, "");
			SendMessage(&Forts[iNumForts], "ls", MSG_MODEL_LOAD_GEO, sFortModel);
			SendMessage(&Island, "li", MSG_ISLAND_ADD_FORT,  &Forts[iNumForts]);
			LayerAddObject(SUN_TRACE, &Forts[iNumForts], 1);
			SendMessage(SeaLighter, "ssi", "AddModel", arLocator.fort.model, &Forts[iNumForts]);
			iNumForts++;

			int iFortCharacter = Fort_FindCharacter(rIsland.id, "reload", arLocator.name);
			string sColonyName = arLocator.colonyname;
			int iColony = FindColony(sColonyName);

			if(iColony < 0)
			{
				trace("we have problem with indetify fort colony");
				continue;
			}
			if (iFortCharacter < 0)
			{
				Trace("FortLogin: Can't find character for fort: '" + arLocator.name + "', on island with id: '" + rIsland.id + "'");
				continue;
			}
			Forts[iNumForts - 1].fortcmdridx = iFortCharacter;
			if(colonies[iColony].nation != characters[iFortCharacter].nation)
			{
				trace("Colony " + colonies[iColony].id + " have wrong nation field.");
				continue;
			}
			if (isEntity(&AIFort) == 0)
			{
				CreateEntity(&AIFort, "AIFort");
				LayerAddObject(SEA_EXECUTE, &AIFort, 2);
				LayerAddObject(SEA_REALIZE, &AIFort, 10001);
			}
			// Add fort to AIFort
			ref rCharacter = GetCharacter(iFortCharacter);
			int iNation = sti(rCharacter.nation);
			if(CheckAttribute(rCharacter,"Ship.LastBallCharacter")) DeleteAttribute(rCharacter,"Ship.LastBallCharacter");

            // boal 22.04.05 сброс всех отношений -->
			if (sti(rCharacter.nation) != PIRATE)
			{
                DelCharacterRelation(iFortCharacter);
            }
            // boal 22.04.05 сброс всех отношений <--
            DeleteAttribute(rCharacter, "CheckFlagYet"); // флаг распознования врага
    		rCharacter.TmpSkillRecall = "0"; //boal 25.05.04 оптимизация расчета, когда == 0 считаем скилы
			int iFortMode = FORT_NORMAL;
			if (CheckAttribute(rCharacter, "Fort.Mode"))
			{
				iFortMode = sti(rCharacter.Fort.Mode);
			}

			bool	bFortRessurect = false;
			int		iDeadDays = 0;
			switch (iFortMode)
			{
				case FORT_NORMAL:
				   // boal -->
				   SetSeaFantomParam(rCharacter, "war"); // генератор!!

				   // boal <--
				break;
				case FORT_ABORDAGE:
					iDeadDays = Fort_GetDeadDays(rCharacter);
					if (iDeadDays >= 6) { bFortRessurect = true; }
				break;
				case FORT_DEAD:
					iDeadDays = Fort_GetDeadDays(rCharacter);
					if (iDeadDays >= 8) { bFortRessurect = true; }
					DeleteAttribute(rCharacter, "PotOfGold"); // золото в сад
					DeleteAttribute(rCharacter, "PotOfSilver");
				break;
			}

			if (iFortMode == FORT_DEAD && iDeadDays > 0)//fix
			{
				rCharacter.Ship.Crew.Quantity = iDeadDays * 100 + rand(100);
			}

			if (bFortRessurect) // восстановили
			{
				int iTest;
				iTest = FindColony(rCharacter.City);
				ref rColony;
				if (iTest != -1)
				{
					rColony = GetColonyByIndex(iTest);
					DeleteAttribute(rColony,"AlreadyGen");
					Group_FreeAllDead();
				}
				SetFortCharacterCaptured(rCharacter, false);
				iFortMode = FORT_NORMAL;
				Log_TestInfo(GetConvertStrWithReplace("Variable_AIFort_1", "Logs.txt", "#space#", " ") + rCharacter.City + GetConvertStrWithReplace("Variable_AIFort_2", "Logs.txt", "#space#", " "));
				rCharacter.Ship.Crew.Quantity = GetMaxCrewQuantity(rCharacter);
				rCharacter.Fort.MaxCrew = GetMaxCrewQuantity(rCharacter); // LEO: макс гарнизона для бара над фортом
				rCharacter.Ship.HP = stf(rCharacter.Fort.Cannons.Quantity) * 100;// восстанавливаем хп
				rCharacter.Fort.HP = rCharacter.Ship.HP;// на всякий случай восстанавливаем макс хп
				iNumDamagedCannonsQuantity = 0;//восстанавливаем пушки для подзорки
			}

			rCharacter.Fort.Mode = iFortMode;
			rCharacter.Fort.PlayerDamage = 0.0;								// player damage

			Event("eSwitchPerks", "l", iFortCharacter);							// Perks
			Ship_UpdateTmpSkills(rCharacter);

			Fort_CheckAttributes(rCharacter);

			AIFort.MinCannonDamageDistance = MIN_CANNON_DAMAGE_DISTANCE;		// in meters
			// LEO: ТУНАБЛЫ осады форта (движок читает каждый кадр). Меняются БЕЗ пересборки — только рестарт игры.
			AIFort.SiegeHoldFrac = 0.5;	// доля дальности ЯДЕР атакующего для дистанции обстрела (меньше = ближе к форту). LEO 12.07: 0.5 ставил флагман под пушки форта -> тонул/садился на мель; 0.65 выводит из плотного огня, ядра ещё достают. Тюнить 0.6-0.65 под высоту форта
			AIFort.SiegeSail = 0.0;			// паруса при осадной бомбардировке (0 = полностью спущены, стоит и долбит)
			rCharacter.Ship.Crew.MinRatio = 1.0;
			rCharacter.Ship.Name = XI_ConvertString(arLocator.fortname);
			rCharacter.Ship.Crew.Morale = 70;
	        if (iFortMode == FORT_NORMAL)
			{
			    SetCharacterGoods(rCharacter, GOOD_BOMBS,  300000 + rand(200));
			    SetCharacterGoods(rCharacter, GOOD_POWDER, 300000 + rand(200));
			}
			else
			{
			    SetCharacterGoods(rCharacter, GOOD_BOMBS,  100 + rand(200));
			    SetCharacterGoods(rCharacter, GOOD_POWDER, 100 + rand(200));
			}
			SetCharacterGoods(rCharacter, GOOD_BALLS,  2000  + rand(200));// boal fix не токо бомбы!

			SetCharacterGoods(rCharacter, GOOD_FOOD,   200 + rand(550));
			SetCharacterGoods(rCharacter, GOOD_WEAPON, 100 + rand(500));
			SetCharacterGoods(rCharacter, GOOD_BRICK,  130  + rand(900));
			SetCharacterGoods(rCharacter, GOOD_SAILCLOTH, 10 + rand(450));
			SetCharacterGoods(rCharacter, GOOD_PLANKS,   20 + rand(750));
			SetCharacterGoods(rCharacter, GOOD_RUM,   5 + rand(150));

			// тут золото по квестам -->
			SetCharacterGoods(rCharacter, GOOD_GOLD,   rand(25));
			SetCharacterGoods(rCharacter, GOOD_SILVER,   rand(50));

			// не чаще раз в мес в один форт
            if (CheckAttribute(rCharacter, "PotOfGoldMonth"))
            {
                if (rCharacter.PotOfGoldMonth == environment.date.month)
                {
                    if (CheckAttribute(rCharacter, "PotOfGold"))
                    {
                        SetCharacterGoods(rCharacter, GOOD_GOLD,   650 + rand(2300));
                    }
                }
                else
                {
                    DeleteAttribute(rCharacter, "PotOfGoldMonth");
                    DeleteAttribute(rCharacter, "PotOfGold");
                }
            }
            if (CheckAttribute(rCharacter, "PotOfSilverMonth"))
            {
                if (rCharacter.PotOfSilverMonth == environment.date.month)
                {
                    if (CheckAttribute(rCharacter, "PotOfSilver"))
                    {
                        SetCharacterGoods(rCharacter, GOOD_SILVER,   960 + rand(4300));
                    }
                }
                else
                {
                    DeleteAttribute(rCharacter, "PotOfSilverMonth");
                    DeleteAttribute(rCharacter, "PotOfSilver");
                }
            }
	        // тут золото по квестам <--
			// create fort blot
			CreateEntity(&FortsBlots[iNumForts - 1], "blots");
			SendMessage(&FortsBlots[iNumForts - 1], "lia", MSG_BLOTS_SETMODEL, &Forts[iNumForts - 1], rCharacter);
			LayerAddObject(SEA_EXECUTE, &Forts[iNumForts], 10001);

			SendMessage(&AIFort, "laaaii", AI_MESSAGE_ADD_FORT, rIsland, arLocator, rCharacter, &Forts[iNumForts-1], &FortsBlots[iNumForts-1]);

			// flags
			ref rNation = GetNationByType(iNation);

			SendMessage(&FortFlag, "lil", MSG_FLAG_INIT, &Forts[iNumForts - 1], iNation); // install flag on fort (frtflg, sflag-группы)
			ReloadProgressUpdate();
		}
	}
}

void Fort_CreateEvent()
{
	int		iCannonsNum;
	aref	rCharacter;

	rCharacter = GetEventData();
	iCannonsNum = GetEventData();

	rCharacter.Fort.Cannons.Quantity = iCannonsNum;
	rCharacter.Fort.MaxCrew = GetMaxCrewQuantity(rCharacter); // LEO: макс гарнизона для бара над фортом
	rCharacter.Fort.HP = iCannonsNum * 100; // макс хп (по всем пушкам)
	// Gregg: квестовый множитель прочности. Ставит и снимает квест (осада Сан-Хуана
	// в «Испанской щедрости»); применяется к максимуму ДО раздачи Ship.HP ниже.
	if (CheckAttribute(rCharacter, "Fort.QuestHPMult")) rCharacter.Fort.HP = stf(rCharacter.Fort.HP) * stf(rCharacter.Fort.QuestHPMult);
	// LEO: сброс Ship.HP в макс — ТОЛЬКО при новой осаде/смене локации (форт чинится между визитами). На сейвлоаде
	//   (actLoadFlag=1) НЕ трогаем: сохранённый Ship.HP уже учитывает урон и пушкам, и стенам. Иначе бар "регенил".
	if (!actLoadFlag)
		rCharacter.Ship.HP = rCharacter.Fort.HP;
	// LEO: порог уничтожения (переход в абордаж, Fort_CannonDamage) как доля Fort.HP; движковый бар ремапит: 0 бара = порог.
	rCharacter.Fort.BarFloorFrac = 0.76 - 0.25 * MOD_SKILL_ENEMY_RATE;
}

int Fort_GetCannonsQuantity(ref rFortCharacter)
{
	if (!CheckAttribute(rFortCharacter, "Fort.Cannons.Quantity")) { return 0; }
	int iMaxCannonsQuantity = sti(rFortCharacter.Fort.Cannons.Quantity);
	int ResultCannons = sti(iMaxCannonsQuantity) - (iNumDamagedCannonsQuantity);
	return ResultCannons;
}

void Fort_ChangeRelation2MainCharacter_FromDamage(ref rFortCharacter, int iFortCharacterIndex, int iBallCharacterIndex, float fDamagePiece)
{
	if (iBallCharacterIndex != nMainCharacterIndex) {
		return;
	}
	int iRelation = SeaAI_GetRelation(iFortCharacterIndex, iBallCharacterIndex);
	if (iRelation == RELATION_ENEMY) {
		return;
	}

	float fCurPlayerDamage = stf(rFortCharacter.Fort.PlayerDamage);
	fCurPlayerDamage += fDamagePiece * (0.5 + 0.3 * (iRelation == RELATION_FRIEND));

	if (fCurPlayerDamage >= 1.0) {
		SetCharacterRelationBoth(iBallCharacterIndex, iFortCharacterIndex, RELATION_ENEMY);
		SetNationRelation2MainCharacter(sti(rFortCharacter.Nation), RELATION_ENEMY);
		UpdateRelations();
	}

	rFortCharacter.Fort.PlayerDamage = fCurPlayerDamage;
}

// проверка на попадание по пушкам форта - считается за каждую пушку в 20 метрах от точки попадания 
float Fort_CannonDamage()
{
	float	x, y, z;
	aref	aFortLabel;
	ref		rBallCharacter, rFortCharacter;
	int		iNumAllCannons, iNumDamagedCannons;
	int		iExp = 0;
	float	fDistance, fDamage;

	int iBallCharacterIndex = GetEventData();
	int iFortCharacterIndex = GetEventData();
	// LEO: владелец ядра может быть -1 (корабль-стрелок, напр. ГГ, взорвался, а его ядра ещё в полёте и бьют форт)
	//      -> GetCharacter(-1) спам "Incorrect Index" при выбивании пушек форта / взрыве корабля ГГ. (|| безопасен — int-сравнения.)
	if (iBallCharacterIndex < 0 || iFortCharacterIndex < 0) return 0.0;

	rBallCharacter = GetCharacter(iBallCharacterIndex);
	rFortCharacter = GetCharacter(iFortCharacterIndex);

	rFortCharacter.Ship.LastBallCharacter = iBallCharacterIndex;

	aFortLabel = GetEventData();

	iNumAllCannons = GetEventData();
	iNumDamagedCannons = GetEventData();
	iNumDamagedCannonsQuantity = iNumDamagedCannons;
	x = GetEventData();
	y = GetEventData();
	z = GetEventData();
	fDistance = GetEventData();	// дистанция от точки попадания до пушки
	fDamage = GetEventData();	// current cannon damage 0.0 percents .. 100.0s percents

	if (fDistance > 5.0 || iBallCharacterIndex == iFortCharacterIndex) {
		return fDamage;
	}

	int iCompanionQuantity = GetCompanionQuantity(rBallCharacter);

	int iFortMode = sti(rFortCharacter.Fort.Mode);
	if (iFortMode != FORT_NORMAL) { return fDamage; }
	float fDistanceMul = pow(0.11, fDistance);// pow(0.035, fDistance / MIN_CANNON_DAMAGE_DISTANCE);

	ref rCannon = GetCannonByType(sti(rBallCharacter.Ship.Cannons.Type));
	float fCannonDamageMultiply = stf(rCannon.DamageMultiply) * fDistanceMul;

	ref rBall = GetGoodByType(sti(AIBalls.CurrentBallType));


	float fHullDamage = stf(rBall.DamageHull) * fCannonDamageMultiply * 0.4;
	float fCrewDamage = stf(rBall.DamageCrew) * fCannonDamageMultiply * 0.4;

	Ship_ApplyCrewHitpoints(rFortCharacter, fCrewDamage);

	float fDamagePiece = fHullDamage;// не нужно тут ранд, все честно, попал - молодец + (frnd() - 0.5) * fHullDamage * 0.4;
	fDamage = (fDamage + fDamagePiece);
	Fort_ChangeRelation2MainCharacter_FromDamage(rFortCharacter, iFortCharacterIndex, iBallCharacterIndex, fDamagePiece);
	bool bImmortal = LAi_IsImmortal(rFortCharacter);
	// boal 13.04.05 фикс пальбы по своему форту -->
	if (iBallCharacterIndex == GetMainCharacterIndex() && sti(Colonies[FindColony(rFortCharacter.City)].HeroOwn) == true)
	{
	    bImmortal = true;
	    iExp = 200;
	    AddCharacterExpToSkill(rBallCharacter, "Accuracy", -iExp / 3);
        AddCharacterExpToSkill(rBallCharacter, "Sailing", -iExp);
        AddCharacterExpToSkill(rBallCharacter, "Sneak", -iExp / 4);
        AddCharacterExpToSkill(rBallCharacter, SKILL_FORTUNE, -iExp / 4);
        AddCharacterExpToSkill(rBallCharacter, "Cannons", -iExp / 4);
	}
	// boal 13.04.05 фикс пальбы по своему форту <--
	if (fDamage >= 1.0)
	{
		CreateBlast(x, y, z, 0.0, 0.0, "CANNONFORTDESTR");
		CreateBlast(x, y, z, 0.0, 0.0, "FORTDESTR");
		if (sti(InterfaceStates.OtherParticles))
		{
			CreateParticleSystem("blast_inv_ext", x, y, z, 0.0, 0.0, 0.0, 0);
			CreateParticleSystem("blast_bomb_ptr", x, y, z, 0.0, 0.0, 0.0, 0);
			CreateParticleSystem("blast_bomb_big", x, y, z, 0.0, 0.0, 0.0, 0);
		}
		else CreateParticleSystem("blast_inv", x, y, z, 0.0, 0.0, 0.0, 0);
		Play3DSound("fort_cann_explode", x, y, z);

		if (!bImmortal)
		{
			iNumDamagedCannons++;
			fDamage = 1.0;
			rFortCharacter.Ship.HP = stf(rFortCharacter.Ship.HP) - stf(rFortCharacter.Fort.HP) / MakeFloat(iNumAllCannons); //отнимаем колво хп равное одной пушке
			if (stf(rFortCharacter.Ship.HP) < 0.0) rFortCharacter.Ship.HP = 0.0; // LEO: пол на нуле - иначе ХП уходят в минус и залипают в сейве
			if (sti(InterfaceStates.OtherParticles)) CreateParticleSystem("Fortfire_net0", x, y, z, 0.0, 0.0, 0.0, 0);
			else CreateParticleSystem("SmallSmoke", x, y, z, -1.57, 0.0, 0.0, 0);
		}
		else
		{
			fDamage = 0.0;
		}

		iExp = MakeInt(200 / iCompanionQuantity); // 1000 boal
		if (iBallCharacterIndex == GetMainCharacterIndex())
		{
			object oRes;
			string sCannonString = LanguageConvertString(iSeaSectionLang, "Fort_cannon");
			// LEO: орудие форта уничтожено. Чекбокс ShipCapShowFortGun вкл -> всплывающий "Орудие форта уничтожено!"
			// (зелёный, блинк) над орудием, БЕЗ строки в логинфо. Выкл -> по-старому строка в лог.
			if (sti(InterfaceStates.ShipCapShowFortGun) == 1)
				SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, x, y, z, sCannonString, argb(255, 80, 255, 80), GetShipCapNotifScale(), GetShipCapNotifDistance());
			else
				Log_SetStringToLog(sCannonString);
		}
        if (!bImmortal)
        {
            //AddCharacterExp(rBallCharacter, iExp);
            // boal -->
            AddCharacterExpToSkill(rBallCharacter, "Accuracy", iExp / 4);
            // LEO: навигация урезана впятеро (было iExp/2 - больше, чем за пушки и меткость вместе).
            //   У форта 20-40 орудий, и за один разбитый форт в навигацию сыпалось несколько тысяч
            //   опыта - больше десятка морских квестов. Жалоба игроков.
            AddCharacterExpToSkill(rBallCharacter, "Sailing", iExp / 10);
            AddCharacterExpToSkill(rBallCharacter, "Sneak", iExp / 12);
            AddCharacterExpToSkill(rBallCharacter, SKILL_FORTUNE, iExp / 12);
            AddCharacterExpToSkill(rBallCharacter, "Cannons", iExp / 4);
            // boal <--
            // LEO-TRACE: за КАЖДОЕ сбитое орудие форта капает скрытность; в HUD суммируется за бой
            Trace("LEOTRACE Sneak: +" + (iExp / 12) + " (сбито орудие форта) fort=" + rFortCharacter.id + " ktoBil=" + rBallCharacter.id + " iExp=" + iExp + " isMain=" + (iBallCharacterIndex == GetMainCharacterIndex()));
        }
	}

	//if (iNumDamagedCannons >= makeint(iNumAllCannons / (1.25 + 0.19*(10 - MOD_SKILL_ENEMY_RATE*3)))) // усложним с 2 до 1.5 // 40%, 50%, 70%
	// 1% хп должно остаться на адмирале, 26% на капитане, 51% на матросе 	
	if (rFortCharacter.Ship.HP < (0.76 - 0.25 * MOD_SKILL_ENEMY_RATE) * stf(rFortCharacter.Fort.HP) ) {
		Fort_SetAbordageMode(rBallCharacter, rFortCharacter);
  		// 13.05.05 anticheat -->
		SetCharacterGoods(rFortCharacter, GOOD_BOMBS,  200 + rand(100));
		SetCharacterGoods(rFortCharacter, GOOD_POWDER,  200 + rand(100));
  		// 13.05.05 anticheat <--
	}

	return fDamage;
}

void Fort_SetAbordageMode(ref rKillerCharacter, ref rFortCharacter)
{
	// open for abordage
	rFortCharacter.Fort.Mode = FORT_ABORDAGE;

	// data of frot die
	rFortCharacter.Fort.DieTime.Year = GetDataYear();
	rFortCharacter.Fort.DieTime.Month = GetDataMonth();
	rFortCharacter.Fort.DieTime.Day = GetDataDay();
	rFortCharacter.Fort.DieTime.Time = GetTime();

	Event(FORT_DESTROYED, "l", sti(rFortCharacter.index));
	int iExp = 20000;
	if (GetRelation(sti(rFortCharacter.index), GetMainCharacterIndex()) == RELATION_ENEMY) // fix если форт друг, а валят его осадники
	{
		// boal -->
		AddCharacterExpToSkillSquadron(rKillerCharacter, "Grappling", 300);
		AddCharacterExpToSkillSquadron(rKillerCharacter, "Leadership", 150);
		AddCharacterExpToSkillSquadron(rKillerCharacter, "Sneak", 35);
		AddCharacterExpToSkillSquadron(rKillerCharacter, SKILL_FORTUNE, 35);
		AddCharacterExpToSkillSquadron(rKillerCharacter, "Cannons", 75);
	    // boal <--
		// LEO-TRACE: форт разбит - скрытность идёт ВСЕЙ эскадре
		Trace("LEOTRACE Sneak: +35 эскадре (форт разбит) fort=" + rFortCharacter.id + " nation=" + sti(rFortCharacter.nation) + " ktoBil=" + rKillerCharacter.id);

		// Fort destroy log
		object oRes;
		string sFortName = rFortCharacter.Ship.Name;
		string sFortString = LanguageConvertString(iSeaSectionLang, "Fort_destroy");
		string sExp = "600";

		Event(PARSE_STRING, "aslss", &oRes, sFortString, 2, sFortName, sExp);
		Log_SetStringToLog(oRes.Str);

	// if main character group killed fort
	if (IsCompanion(rKillerCharacter))
	{
		Ship_PlayVictory("music_ship_victory", 12000);
	}
	}

}
// нигде не используется
void Fort_SetCharacter(ref rCharacter, string sIslandID, string sLocationGroup, string sLocationLocator)
{
	int iCharacterIndex = Fort_FindCharacter(sIslandID, sLocationGroup, sLocationLocator);
	if (iCharacterIndex != -1)
	{
		ChangeCharacterAddress(&Characters[iCharacterIndex], "None", "");
	}

	ChangeCharacterAddressGroup(rCharacter, sIslandID, sLocationGroup, sLocationLocator);

	rCharacter.Ship.Type = Characters[iCharacterIndex].ship.type;

	Fort_CheckAttributes(rCharacter);
}

void Fort_CheckAttributes(ref rCharacter)
{
	if (!CheckAttribute(rCharacter, "Ship.Cannons.Type")) { rCharacter.Ship.Cannons.Type = CANNON_TYPE_CULVERINE_LBS24; }

	if (!CheckAttribute(rCharacter, "Ship.Cannons.Charge.Type")) { rCharacter.Ship.Cannons.Charge.Type = GOOD_BOMBS; }
	if (!CheckAttribute(rCharacter, "Fort.Cannons.Type.1"))
	{
		Trace("Fort Error: Fort.Cannons.Type.1 can't find in character " + rCharacter.Name);
		rCharacter.Fort.Cannons.Type.1 = -1;//CANNON_TYPE_CULVERINE_LBS24;
	}
	if (!CheckAttribute(rCharacter, "Fort.Cannons.Type.2"))
	{
		Trace("Fort Error: Fort.Cannons.Type.2 can't find in character " + rCharacter.Name);
		rCharacter.Fort.Cannons.Type.2 = -1;//CANNON_TYPE_CANNON_LBS32;
	}
	if (!CheckAttribute(rCharacter, "Fort.Cannons.Type.3"))
	{
		Trace("Fort Error: Fort.Cannons.Type.3 can't find in character " + rCharacter.Name);
		rCharacter.Fort.Cannons.Type.3 = -1;//CANNON_TYPE_CANNON_LBS48;
	}
	rCharacter.Ship.Cannons.Charge.Type = GOOD_BOMBS;  // всегда бомбы   boal
}

void Fort_LoadDamagedCannon()
{
	Trace("Fort_LoadDamagedCannon");
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	switch (rand(1))
	{
		case 0:
			if (sti(InterfaceStates.OtherParticles)) CreateParticleSystem("Fortfire_net0", x, y, z, 0.0, 0.0, 0.0, 0);
			else
			{
				CreateParticleSystem("fort_fire", x, y, z, -1.57, 0.0, 0.0, 0);
				CreateParticleSystem("fort_smoke", x, y, z, -1.57, 0.0, 0.0, 0);
			}
		break;
		case 1:
			if (sti(InterfaceStates.OtherParticles)) CreateParticleSystem("Fortfire_net0", x, y, z, 0.0, 0.0, 0.0, 0);
			else
			{
				CreateParticleSystem("fort_fire", x, y, z, -1.57, 0.0, 0.0, 0);
				CreateParticleSystem("fort_gray_smoke", x, y, z, -1.57, 0.0, 0.0, 0);
			}
		break;
	}
}

// boal по порту найдем город и форт 15.01.05 -->
int Fort_FindCharacterByPort(string sIslandId, string sLocationLocator)
{
    ref   isl = GetIslandByID(sIslandId);
    aref  curItem;
	aref  rootItems;
	int   i;

	makearef(rootItems, isl.reload);
	int iHow;
	iHow = GetAttributesNum(rootItems);
	for (i=0; i<iHow; i++)
    {
        curItem = GetAttributeN(rootItems, i);
        if (curItem.name == sLocationLocator)
        {
            sLocationLocator = curItem.go;
            break;
        }
    }
    for (i=0; i<MAX_COLONIES; i++)
	{
	    if (colonies[i].from_sea == sLocationLocator)
	    {
	        if (CheckAttribute(&colonies[i], "commanderIdx"))
	        {
	            return  sti(colonies[i].commanderIdx);
	        }
	    }
	}

	return -1;
}
// boal 15.01.05 <--

// LEO: взрыв мортиры форта в ТОЧКЕ ПОПАДАНИЯ: эффект и звук как у морской мины (разобранный MakeMineBoom
// из AISeaGoods.c) + splash-урон по ВРАГАМ ФОРТА. Зовётся только из Ship_HullHitEvent за гейтом типа пушки.
// Урон прямой цели тут НЕ трогаем - его уже нанёс Ship_HullHitEvent, поэтому iDirectHitIndex из обхода
// исключён (иначе цель получила бы двойной урон за один снаряд).
void Fort_MortarBoom(ref rFortCharacter, int iDirectHitIndex, float x, float y, float z)
{
	// --- эффект + звук ---
	// splash_mine (столб воды) не берём: мина рвётся на воде, а мортира бьёт в борт - там он не к месту.
	if (sti(InterfaceStates.OtherParticles))
	{
		CreateParticleSystem("blast_bomb", x, y, z, 0, 1.0, 0, 0);
		CreateParticleSystem("blast_bomb_big", x, y, z, 0, 1.0, 0, 0);
		CreateParticleSystem("blast_wave_small", x, y, z, 0, 1.0, 0, 0);
		CreateParticleSystem("blast_inv_ext", x, y, z, 0, 1.0, 0, 0);
	}
	else
	{
		CreateParticleSystem("ShipExplode", x, y, z, 0, 1.0, 0, 0);
	}
	Play3DSound("mine_explosion", x, y, z);
	// тряска камеры - вне гейта частиц (это не партикл; у мины она внутри гейта, что выглядит как недосмотр)
	if (iDirectHitIndex >= 0) SendMessage(GetCharacter(iDirectHitIndex), "lfff", MSG_SHIP_DETONATE, 1.0, 1.0, 500.0);

	// --- splash по врагам форта (обход по образцу Ship_BranderDetonate, но центр = точка попадания) ---
	aref	arShipObjects[64];
	int		iNumShips = 1;
	if (!FindEntity(&arShipObjects[0], "ship")) return;
	while (iNumShips < 64 && FindEntityNext(&arShipObjects[iNumShips])) { iNumShips++; }
	iNumShips = i_min(63, iNumShips);

	int		iFortIndex = sti(rFortCharacter.index);
	float	fR2 = FORT_MORTAR_AOE_RADIUS * FORT_MORTAR_AOE_RADIUS;
	for (int i = 0; i < iNumShips; i++)
	{
		int iCharacterIndex = -1;
		SendMessage(arShipObjects[i], "le", MSG_SHIP_GET_CHARACTER_INDEX, &iCharacterIndex);
		if (iCharacterIndex < 0) { continue; }
		if (iCharacterIndex == iDirectHitIndex) { continue; }	// прямая цель свой урон уже получила
		if (iCharacterIndex == iFortIndex) { continue; }

		// LEO: отношение спрашиваем у ДВИЖКА (AI_MESSAGE_GET_RELATION -> Helper.GetRelationSafe) - это ТА ЖЕ
		//      матрица, которой форт выбирает цели (Helper.isEnemy). Скриптовый GetRelation с ней расходится:
		//      компаньоны игрока читались как NEUTRAL, хотя форт по ним лупит прямой наводкой, и splash их
		//      не задевал. В SeaAI_GetRelation этот путь закомментирован ради ФПС (зовётся каждый кадр), а
		//      здесь он срабатывает только на попадание мортиры - редко, на перф не давит.
		int iRel = RELATION_NEUTRAL;
		SendMessage(&AISea, "laae", AI_MESSAGE_GET_RELATION, &Characters[iFortIndex], &Characters[iCharacterIndex], &iRel);
		ref rOther = GetCharacter(iCharacterIndex);
		float dx = stf(rOther.Ship.Pos.x) - x;
		float dz = stf(rOther.Ship.Pos.z) - z;
		float fD2 = dx * dx + dz * dz;						// квадрат - sqrt только для попавших в радиус

		if (iRel != RELATION_ENEMY) { continue; }			// только враги форта
		if (fD2 > fR2) { continue; }

		float fDmg = FORT_MORTAR_AOE_MAXDAMAGE * Bring2Range(1.0, 0.0, FORT_MORTAR_AOE_FULLDIST, FORT_MORTAR_AOE_RADIUS, sqrt(fD2));
		if (makeint(fDmg) < 1) { continue; }
		Ship_ApplyHullHitpoints(rOther, fDmg, KILL_BY_BALL, iFortIndex);

		// всплывающий урон над задетой целью - как у брандера (цель-игрок под своей опцией)
		bool bShowDmg = false;
		if (iCharacterIndex == nMainCharacterIndex) { if (sti(InterfaceStates.ShipCapShowDamageSelf) == 1) bShowDmg = true; }
		else { if (sti(InterfaceStates.ShipCapShowDamage) == 1) bShowDmg = true; }
		if (bShowDmg)
		{
			SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, stf(rOther.Ship.Pos.x), stf(rOther.Ship.Pos.y), stf(rOther.Ship.Pos.z),
				"-" + its(makeint(fDmg)), argb(255, 255, 60, 60), 3.0, GetShipCapNotifDistance());
		}
	}
}
