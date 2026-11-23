#define MINE_POWDER          200 // 07.07.07 мины

object	AISeaGoods;
int pindex = 0;

void DeleteSeaGoodsEnvironment()
{
	DelEventHandler(SHIP_DEAD, "AISeaGoods_ShipDead");
	DelEventHandler(SHIP_EAT_SWIM_GOOD, "AISeaGoods_ShipEatGood");
	DeleteClass(&AISeaGoods);
	DeleteAttribute(&AISeaGoods, "");
	pindex = 0;
}

void CreateSeaGoodsEnvironment()
{
	CreateEntity(&AISeaGoods, "AISeaGoods");

	LayerAddObject(SEA_EXECUTE, &AISeaGoods, -1);
	LayerAddObject(SEA_REALIZE, &AISeaGoods, 65532);

	SetEventHandler(SHIP_DEAD, "AISeaGoods_ShipDead", 0);
	SetEventHandler(SHIP_EAT_SWIM_GOOD, "AISeaGoods_ShipEatGood", 0);

	AISeaGoods.ModelsPath = "SwimGoods";
	AISeaGoods.DeleteGoodAnyway = false;
	AISeaGoods.DistanceMultiply = 0.5;
}

void AISeaGoods_ShipDead()
{
	ref rCharacter = GetCharacter(GetEventData());

	aref aGoods; makearef(aGoods, rCharacter.Ship.Cargo.Goods);
	int iNumGoods;
	iNumGoods = GetAttributesNum(aGoods); // оптимизация
	for (int i=0; i<iNumGoods; i++)
	{
		aref aGood = GetAttributeN(aGoods, i);
		int	iQuantity = sti(GetAttributeValue(aGood));
		ref	rGood = GetGoodByID(GetAttributeName(aGood));

		if (!CheckAttribute(rGood, "Swim")) { continue; }

		iQuantity = iQuantity / sti(rGood.Units);
		if (iQuantity <= 1) { continue; }

		if (rand(100) > 95) { continue; }

		int iSwimQuantity = 0;
		iSwimQuantity = MakeInt(1 + iQuantity / 3 + rand(MakeInt(iQuantity / 2)));

		float fTime = stf(rGood.Swim.Time);
		string sModel = rGood.Swim.Model;

		fTime = fTime + (frnd() - 0.5) * fTime / 2.0;

		if (rand(1) == 0 && iSwimQuantity >= 2)
		{
			AISeaGoods_AddGood(rCharacter, GetAttributeName(aGood), sModel, fTime * 60.0, makeint(iSwimQuantity / 2));
			AISeaGoods_AddGood(rCharacter, GetAttributeName(aGood), sModel, fTime * 60.0, makeint(iSwimQuantity / 2)); // две бочки на один товар
		}
		else
		{
			AISeaGoods_AddGood(rCharacter, GetAttributeName(aGood), sModel, fTime * 60.0, iSwimQuantity);
		}
	}
}

void AISeaGoods_AddGood(ref rCharacter, string sGood, string sModel, float fTime, int iQuantity)
{
	if (!bSeaActive) return;

	// calculate random position
	float fAng = stf(rCharacter.Ship.Ang.y) + PI + (frnd() - 0.5) * PI/2;
	float fDist = stf(rCharacter.Ship.BoxSize.z) / 2.0 + frnd() * 10.0;

	AISeaGoods.Good = sGood;
	AISeaGoods.Pos.x = stf(rCharacter.Ship.Pos.x) + fDist * sin(fAng);
	AISeaGoods.Pos.z = stf(rCharacter.Ship.Pos.z) + fDist * cos(fAng);
	AISeaGoods.CharIndex = rCharacter.Index;
	if (HasSubStr(sGood,"|"))
	{
		CreateParticleSystem("splash", stf(AISeaGoods.Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.Pos.z),0,1.0,0,0);
		AISeaGoods.(sGood).Pos.x = AISeaGoods.Pos.x;
		AISeaGoods.(sGood).Pos.z = AISeaGoods.Pos.z;
    }
	AISeaGoods.Time = fTime;
	AISeaGoods.Quantity = iQuantity;
	AISeaGoods.Model = sModel;
	AISeaGoods.Add = "";
}

// ugeen --> выкидываем либо бочку со спецпредметом либо шлюпку с кораблекрушенцем
void AISeaGoods_AddGood_Special(ref rCharacter, string sGood, string sModel, float fTime, int iQuantity)
{
	if (!bSeaActive) return;

	float fAng = stf(pchar.Ship.Ang.y) + PI + (frnd() - 0.5) * PI;
	float fDist = stf(pchar.Ship.BoxSize.z) / 2.0 + 20.0 + makefloat(rand(30));

	AISeaGoods.Good = sGood;
	AISeaGoods.Pos.x = stf(pchar.Ship.Pos.x) + fDist * sin(fAng);
	AISeaGoods.Pos.z = stf(pchar.Ship.Pos.z) + fDist * cos(fAng);
	AISeaGoods.CharIndex = rCharacter.Index;
	if (HasSubStr(sGood,"|"))
	{
		CreateParticleSystem("splash", stf(AISeaGoods.Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.Pos.z),0,1.0,0,0);
		AISeaGoods.(sGood).Pos.x = AISeaGoods.Pos.x;
		AISeaGoods.(sGood).Pos.z = AISeaGoods.Pos.z;
    }
	AISeaGoods.Time = fTime;
	AISeaGoods.Quantity = iQuantity;
	AISeaGoods.Model = sModel;
	AISeaGoods.Add = "";
}
// <-- ugeen

bool AISeaGoods_ShipEatGood()
{
	object oRes;

	int iCharacterIndex = GetEventData();
	int iGoodCharacterIndex = GetEventData();  // с кого товар
	string sGood = GetEventData();
	int iQuantity = GetEventData();
	
	string sGoodMine = sGood;
	if (HasSubStr(sGood,"|"))
	{
		int sub0 = findSubStr(sGood, "|", 0);
		sGood = strcut(sGood, 0, sub0 - 1);
	}

	ref rCharacter = GetCharacter(iCharacterIndex);

	if (LAi_IsDead(rCharacter))
	{
		return false;
	}

	switch (sGood)
	{
		case "boat":   //homo 22/06/07 если подобрали шлюпку и ГГ
			if (iCharacterIndex == sti(pchar.index))
			{
				pchar.GenQuest.Survive_In_SeaOfficerIdx = iGoodCharacterIndex;
	            DoQuestCheckDelay("Survive_In_SeaOfficer", 1.0);
	            return true;
			}
			else
			{
				DeleteAttribute(Pchar, "GenQuest.Survive_In_SeaOfficerIdx");
				return true;
			}
		break;

		case "enemy_boat":   //homo 22/06/07 если подобрали шлюпку
	        if (iCharacterIndex == sti(pchar.index))
			{
				pchar.GenQuest.Survive_In_SeaPrisonerIdx = iGoodCharacterIndex;
	            DoQuestCheckDelay("Survive_In_SeaPrisoner", 1.0);
	            return true;
			}
			else
			{
				return true;
			}
		break;

		case "unknown_boat":
			if (iCharacterIndex == sti(pchar.index))
			{
				Log_TestInfo(GetConvertStrWithReplace("Variable_AISeaGoods_1", "Logs.txt", "#space#", " "));
				pchar.GenQuest.Survive_In_SeaPrisonerIdx = iGoodCharacterIndex;
				DoQuestCheckDelay("Survive_In_SeaPrisoner", 1.0);
			}
			return true;
		break;
		// мины 07.07.07
		case "powder1":
		    MakeMineBoom(iCharacterIndex, iGoodCharacterIndex, 600.0, sGoodMine);
			return true;
		break;

		case "powder2":
		    MakeMineBoom(iCharacterIndex, iGoodCharacterIndex, 1000.0, sGoodMine);
			return true;
		break;

		case "powder3":
		    MakeMineBoom(iCharacterIndex, iGoodCharacterIndex, 1500.0, sGoodMine);
			return true;
		break;

		case "barrel_treasure":
			Log_TestInfo(GetConvertStrWithReplace("Variable_AISeaGoods_2", "Logs.txt", "#space#", " "));
			DoQuestCheckDelay("Get_TreasureBarrel", 1.0);
			return true;
		break;
    }

	if (iGoodCharacterIndex == iCharacterIndex)
	{
		return false;
	}

	int iGood = FindGood(sGood);
	if (iGood < 0) return false;
	ref rGood = &Goods[iGood];
	int iGoodUnits = sti(rGood.Units);
	int iMaxGoodAllow = GetCharacterFreeSpace(rCharacter, iGood) / iGoodUnits;
	if (iMaxGoodAllow == 0) return false;

	if (iQuantity > iMaxGoodAllow)
	{
		iQuantity = iMaxGoodAllow;
	}

	if (iCharacterIndex == nMainCharacterIndex)
	{
        PlayVoice("INTERFACE\"+LanguageGetLanguage()+"_Gotcha.wav"); // boal
		string sGoodName = LanguageConvertString(iSeaSectionLang, "seg_" + sGood);
		string sGoodQuantity = iQuantity * sti(rGood.Weight);
		string sShipGotGood = LanguageConvertString(iSeaSectionLang, "Ship_got_good");
		Event(PARSE_STRING, "aslss", &oRes, sShipGotGood, 2, sGoodQuantity, sGoodName);
		Log_SetStringToLog(oRes.Str);
	}

	iQuantity = iQuantity * iGoodUnits;

	AddCharacterGoods(rCharacter, iGood, iQuantity);

	return true;
}

// мины boal 07.07.07
void MakeMineBoom(int iCharacterIndex, int iGoodCharacterIndex, float damg, string sGoodMine)
{
    ref rCharacter = GetCharacter(iCharacterIndex);

    // если ГГ или компаньон пустили бочку и был мир
    if (isCompanion(GetCharacter(iGoodCharacterIndex)) && GetNationRelation2MainCharacter(sti(rCharacter.nation)) != RELATION_ENEMY)
    {
        if (!isCompanion(rCharacter) && !CheckAttribute(rCharacter, "Coastal_Captain")) // по своим можно палить таможня пофиг
        {
            Ship_NationAgressive(rCharacter, rCharacter);
            DoQuestCheckDelay("NationUpdate", 0.7);
        }
    }
    Ship_ApplyHullHitpoints(rCharacter, damg, KILL_BY_BALL, iGoodCharacterIndex);

    if (bBettaTestMode)  // даёт вылет :(
    {
		int iRandStartTime = rand(1000);
		float fTotalFireTime = Ship_GetTotalFireTime(rCharacter);

		PostEvent(SHIP_ACTIVATE_FIRE_PLACE, iRandStartTime, "ialsfl", rCharacter, rCharacter, 0, "ship_onfire", fTotalFireTime, iGoodCharacterIndex);
		PostEvent(SHIP_FIRE_DAMAGE, iRandStartTime, "lllf", iCharacterIndex, iGoodCharacterIndex, 0, fTotalFireTime);
	}
    if (sti(InterfaceStates.OtherParticles))
	{
		CreateParticleSystem("blast_bomb", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z),0,1.0,0,0);
		CreateParticleSystem("blast_bomb_big", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z),0,1.0,0,0);
		CreateParticleSystem("blast_wave_small", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z),0,1.0,0,0);
		CreateParticleSystem("splash_mine", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z),0,1.0,0,0);
		CreateParticleSystem("blast_inv_ext", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z),0,1.0,0,0);
		Play3DSound("mine_explosion", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z));
		SendMessage(rCharacter, "lfff", MSG_SHIP_DETONATE, 1.0, 1.0, 500.0);
	}
	else
	{
		CreateParticleSystem("ShipExplode", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z),0,1.0,0,0);
		CreateParticleSystem("splash_mine", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z),0,1.0,0,0);
		Play3DSound("mine_explosion", stf(AISeaGoods.(sGoodMine).Pos.x), stf(rCharacter.Ship.pos.y), stf(AISeaGoods.(sGoodMine).Pos.z));
	}
	DeleteAttribute(AISeaGoods,sGoodMine);
}

void SetMineFree(ref xi_refCharacter, int type)
{
    AISeaGoods_AddGood(xi_refCharacter, "powder" + type + "|" + pindex, "barrel", 600.0, 1);
	pindex++;
    PlaySound("Ships\jakor_002.wav");
    RemoveCharacterGoods(xi_refCharacter, GOOD_POWDER, type * MINE_POWDER);
}
