/////////////////////////
// GOF 1.2
/////////////////////////

#define DIRECTSAILDEBUG	0;

#define DIRECTENCOUNTERRANDDISTANCE 	6250000.0 	// 2500 ^ 2
#define DIR_SAIL_STORM_DIST_REL  		12250000.0 	// 3500 ^ 2
#define DIR_SAIL_STORMWARN_DIST_REL  	25000000.0 	// 5000 ^ 2
#define DIR_SAIL_STORM_DIST 			3500.0

#define DIR_PORTBOW 1
#define DIR_FORWARD 2
#define DIR_STARBOW 3
#define DIR_STAR 4
#define DIR_PORT 5
#define DIR_PORTSTERN 6
#define DIR_ABAFT 7
#define DIR_STARSTERN 8

#define DIR_FORE 		4

extern void evtDirSail(aref rRawG, aref encDataForS, int iNumMerchantS, int iNumWarS,
                      float playerShipX, float playerShipZ, float locx, float locay, float locz,
                      float dst, float fScale, string eID);

string CheckForPotentialIslandChange()
{
	string result = "";
	float 		RTplayerShipX;
	float 		RTplayerShipZ;
	int 		scale;
	
	getRTplayerShipXZ(&RTplayerShipX, &RTplayerShipZ, &scale);

	float 		distToClosestIsland, distStorm;
	bool 		bScaleDiff;
	
	int 		nextisland = getRTclosestIsland(RTplayerShipX, RTplayerShipZ, scale, &distToClosestIsland, &bScaleDiff);
	
	while(nextisland > -1 && nextisland != FindIsland(worldMap.island))
	{
		ref rIsland 		= GetIslandByIndex(nextisland);
		string sNewIslandId = rIsland.id;
		string sIslandNow 	= worldMap.island;

		float distToCurIsland;
		
		if (worldMap.island == WDM_NONE_ISLAND) 
		{	
			distToCurIsland = 2500000000.0;
		}	
		else
		{		
			distToCurIsland = GetDistance2DRel(RTplayerShipX, RTplayerShipZ, stf(worldMap.islands.(sIslandNow).position.x), stf(worldMap.islands.(sIslandNow).position.z));
		}	
		
		if (bScaleDiff) 
		{
			if(distToClosestIsland * 1.12 > distToCurIsland)
				break;
		}
		else 
		{
			// Gregg: 1.85->1.0 - доплытие включается на середине плеча (а не ~58%), иначе у края акватории не загорается; держать одинаковым в обеих функциях
			if(distToClosestIsland * 1.0 > distToCurIsland)
				break;
		}
		
		if(!bEnableIslandSailTo || bDisableMapEnter || CheckAttribute(pchar,"GhostCap.Fight"))
		{
			break;
		}		
		
		result = sNewIslandId;
		break;
	}
	return result;
}

void CheckIslandChange(int num)
{
	float 		RTplayerShipX;
    float 		RTplayerShipZ;
    int 		scale;
    bool 		isStorm, isTornado;
	
    getRTplayerShipXZ(&RTplayerShipX, &RTplayerShipZ, &scale);

    float 		distToClosestIsland, distStorm;
    bool 		bScaleDiff;
    bool 		bCheckEnc = true;
	
	int 		nextisland = getRTclosestIsland(RTplayerShipX, RTplayerShipZ, scale, &distToClosestIsland, &bScaleDiff);
	if (num == 1)
	{
		while(nextisland > -1 && nextisland != FindIsland(worldMap.island))
		{
			ref rIsland 		= GetIslandByIndex(nextisland);
			string sNewIslandId = rIsland.id;
			string sIslandNow 	= worldMap.island;

			float distToCurIsland;
			
			if (worldMap.island == WDM_NONE_ISLAND) 
			{	
				distToCurIsland = 2500000000.0;
			}	
			else
			{		
				distToCurIsland = GetDistance2DRel(RTplayerShipX, RTplayerShipZ, stf(worldMap.islands.(sIslandNow).position.x), stf(worldMap.islands.(sIslandNow).position.z));
			}	
			
			if (bScaleDiff) 
			{
				if(distToClosestIsland * 1.12 > distToCurIsland)
					break;
			}
			else 
			{
				// Gregg: 1.85->1.0 - доплытие включается на середине плеча (а не ~58%), иначе у края акватории не загорается; держать одинаковым в обеих функциях
				if(distToClosestIsland * 1.0 > distToCurIsland)
					break;
			}

			bCheckEnc = false;
			

			if(!bEnableIslandSailTo || bDisableMapEnter || CheckAttribute(pchar,"GhostCap.Fight"))
			{
				break;
			}		
			
			// mitrokosta проверка перегруза итд
			if(!CheckMapEnterConditions()) 
			{
				break;
			}
			
			sIslandID = sNewIslandId;
			
			SetEventHandler("FaderEvent_StartFade", "ChangeSeaMapNew", 0);
			SetEventHandler("FaderEvent_EndFade", "Sea2Sea_Reload", 0);
			
			CreateEntity(&wdm_fader, "fader");
			if(IsEntity(wdm_fader) == 0) Trace("Fader not created!!!");
			
			float fadeOutTime = RELOAD_TIME_FADE_OUT; // Gregg: единый дефайн транзишена (было 0.5)

			SendMessage(&wdm_fader, "lfl", FADER_OUT, fadeOutTime, true);
			SendMessage(&wdm_fader, "l", FADER_STARTFRAME);
			
			string imageName = "Loading\jonny_load\sea\sea_" + rand(3) + ".dds";
			SendMessage(&wdm_fader, "ls", FADER_PICTURE0, imageName);
		
			break;
		}
	}
	
	if(bCheckEnc) 
	{
        checkWMEnctr(RTplayerShipX, RTplayerShipZ, scale, &isStorm, &isTornado, &distStorm);
		
        if(isStorm) 
		{
            if(distStorm <= DIR_SAIL_STORM_DIST_REL) 
			{
                WeatherParams.Tornado 	= isTornado;
                WeatherParams.Storm 	= true;
                iStormLockSeconds 		= 60;
                Seafoam.storm 			= "true";
                Whr_UpdateWeather();
                SetSchemeForSea();
            }
            else 
			{
                WeatherParams.Rain.ThisDay 		= true;
                WeatherParams.Rain.StartTime 	= MakeInt(GetHour());
                WeatherParams.Rain.Duration 	= 75.0;
				WeatherParams.Rain.Type			= rand(1);								// тип дождя 
				WeatherParams.Rain.year 		= GetDataYear();		
				WeatherParams.Rain.month 		= GetDataMonth();
				WeatherParams.Rain.day 			= GetDataDay();
				WeatherParams.Rain.time 		= stf(WeatherParams.Rain.StartTime);
				Whr_UpdateWeather();
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_Islands_loader_8", "Logs.txt", "#space#", " "));
            }
        }
	}	
}

bool CheckMapEnterConditions() 
{
    int cn;
    ref chref;
	bool canEnter = true;
	bool bLog = false;

    for (int i = 0; i<COMPANION_MAX; i++) 
	{
		cn = GetCompanionIndex(pchar, i);
		if (cn >= 0) 
		{
			chref = GetCharacter(cn);

			if (!GetRemovable(chref)) 
			{
				continue;
			}
			
            if (GetCargoLoad(chref) > GetCargoMaxSpace(chref)) 
			{
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_Islands_loader_1", "Logs.txt", "#space#", " ") +  chref.Ship.Name + GetConvertStrWithReplace("Variable_Islands_loader_2", "Logs.txt", "#space#", " "));
				bLog = true;
				canEnter = false;
				break;
            }

		}
    }

	if (canEnter && !CheckEnemyCompanionDistance2GoAway(false)) 
	{
		CheckEnemyCompanionDistance2GoAway(true); // получается по-дурацки - двойной вызов одной функции, но это нужно чтобы не было спама табличками
		canEnter = false;
	}
	
	if (bLog) 
	{
		Log_Info(GetConvertStrWithReplace("Variable_Islands_loader_7", "Logs.txt", "#space#", " "));
		PlaySound("interface\knock.wav");
	}
	
	return canEnter;
}

//#20190624-01
void getRTplayerShipXZ(ref RTplayerShipX, ref RTplayerShipZ, ref scale)
{
	float zeroX = MakeFloat(worldMap.zeroX);
	float SeaX = stf(pchar.Ship.Pos.x);
	float zeroZ = MakeFloat(worldMap.zeroZ);
	float SeaZ = stf(pchar.Ship.Pos.z);
	string sIsland = worldMap.island;

	scale = WDM_MAP_TO_SEA_SCALE;
	if (sIsland == "Cuba1" || sIsland == "Cuba2" || sIsland == "Beliz" || sIsland == "SantaCatalina"
		|| sIsland == "PortoBello" || sIsland == "Cartahena" || sIsland == "Maracaibo"
		|| sIsland == "Caracas" || sIsland == "Cumana")
	{
		scale = 25;
	}
	RTplayerShipX = (SeaX/scale) + zeroX;
	RTplayerShipZ = (SeaZ/scale) + zeroZ;
	return;
}

float getRTplayerShipAY()
{
	float RTplayerShipAY = stf(pchar.Ship.Ang.y);
	return RTplayerShipAY;
}
//#20190628-01
int getRTclosestIsland(float RTplayerShipX, float RTplayerShipZ, int curScale, ref iDistanceNow, ref scaleDiff)
{
	bool 			tempDiff;
	float 			distance, iBearing, offShip, shipAY, iX, iZ;
	iDistanceNow 	= 2500000000.0; 
    scaleDiff 		= false;
	int nextisland 	= -1;
	ref 			rIsland;
	string 			islandTemp;
    shipAY 			= stf(pchar.Ship.Ang.y);
	
	for (int inum = 0; inum <= iNumIslands; inum++ )
	{
		rIsland = GetIslandByIndex(inum);
		islandTemp = rIsland.id;
		
		if(Islands[inum].visible == false) continue;
		
		iX = stf(worldMap.islands.(islandTemp).position.x);
		iZ = stf(worldMap.islands.(islandTemp).position.z);

		iBearing = GetAngleY(iX - RTplayerShipX, iZ - RTplayerShipZ);
        offShip = iBearing - shipAY;
        int nBear = ClosestDirFA(offShip);
        if(nBear != DIR_FORWARD) continue; 

		tempDiff = false;
		if(curScale == WDM_MAP_TO_SEA_SCALE) {
            if (islandTemp == "Cuba1" || islandTemp == "Cuba2" || islandTemp == "Beliz" || islandTemp == "SantaCatalina"
                || islandTemp == "PortoBello" || islandTemp == "Cartahena" || islandTemp == "Maracaibo"
                || islandTemp == "Caracas" || islandTemp == "Cumana")
            {
                tempDiff = true;
            }
		}

		distance = GetDistance2DRel(RTplayerShipX, RTplayerShipZ, iX, iZ);

		if (distance < iDistanceNow)
		{
			iDistanceNow = distance
			nextisland = inum;
			scaleDiff = tempDiff;
		}
	}

	return nextisland;
}

//#20180813-02
void ChangeSeaMapNew()
{
	DelEventHandler("FaderEvent_StartFade", "ChangeSeaMapNew");
	DeleteSeaEnvironment();
	LayerAddObject(SEA_REALIZE, &wdm_fader, -1);
}

void Sea2Sea_Reload()
{
    //#20180813-02
	DelEventHandler("FaderEvent_EndFade", "Sea2Sea_Reload");
    ReloadProgressStart();
	object seaLoginToSea;

	//#20190624-01
    float CX;
    float CZ;
    int scale = WDM_MAP_TO_SEA_SCALE;
    float fScrap;
    getRTplayerShipXZ(&CX, &CZ, &scale);

	float CAY = getRTplayerShipAY();
	//#20190628-01 //#20190628-01
	bool bScaleDiff;
	int nextisland = getRTclosestIsland(CX, CZ, scale, &fScrap, &bScaleDiff);
	ref rIsland = GetIslandByIndex(nextisland);
	string CIsland = rIsland.id;

	float ix = MakeFloat(worldMap.islands.(CIsland).position.x);
	float iz = MakeFloat(worldMap.islands.(CIsland).position.z);

	seaLoginToSea.playerGroup.x = (CX - ix)*scale;
	seaLoginToSea.playerGroup.z = (CZ - iz)*scale;
	seaLoginToSea.playerGroup.ay = CAY;
	seaLoginToSea.island = CIsland;

    DeleteAttribute(pchar, "pirateFriendly");
	SeaLogin(&seaLoginToSea);
	LayerAddObject(SEA_REALIZE, &wdm_fader, -1);
	ReloadProgressEnd();

	//#20190624-01
    float RTplayerShipX;
    float RTplayerShipZ;
    getRTplayerShipXZ(&RTplayerShipX, &RTplayerShipZ, &scale);
	worldMap.playerShipX = RTplayerShipX;
	worldMap.playerShipZ = RTplayerShipZ;
	worldMap.playerShipAY = getRTplayerShipAY();
	worldMap.island = rIsland.id;
	worldMap.zeroX = ix;
	worldMap.zeroZ = iz;
}

// Возвращает DIR_XXX.
int ClosestDirE(float dir)
{
    float aX[8];
    float aZ[8];
    // Storm не поддерживает инициализаторы массивов float.
    aX[0] = -0.5; aX[1] = 0.0; aX[2] = 0.5; aX[3] = 1.0; aX[4] = -1.0; aX[5] = -0.5; aX[6] = 0.0; aX[7] = 0.5;
    aZ[0] = 1.0; aZ[1] = 1.0; aZ[2] = 1.0; aZ[3] = 0.0; aZ[4] = 0.0; aZ[5] = -1.0; aZ[6] = -1.0; aZ[7] = -1.0;

    float maxDot = -999999999.0;
    int nRet = 0;
    for(int i = 0; i < 8; i++) {
        float v = GetAngleY(aX[i], aZ[i]);
        float t = GetDotProduct(v, dir);
        if (t > maxDot) {
             nRet = i + 1;
             maxDot = t;
         }
    }
    return nRet;
}

int ClosestDirFA(float dir)
{
    float aX[6];
    float aZ[6];
    //Bug in compiler...array inits don't work in .c files
    aX[0] = -1.0; aX[1] = 0.0; aX[2] = 1.0; aX[3] = -1.0; aX[4] = 0.0; aX[5] = 1.0;
    aZ[0] = 0.5; aZ[1] = 1.0; aZ[2] = 0.5; aZ[3] = 0.0; aZ[4] = -1.0; aZ[5] = 0.0;

    float maxDot = -99999999.0;
    int nRet = 0;
    for(int i = 0; i < 6; i++) {
        float v = GetAngleY(aX[i], aZ[i]);
        float t = GetDotProduct(v, dir);
        if (t > maxDot) {
             nRet = i + 1;
             maxDot = t;
         }
    }
    if(nRet < 4)
        nRet = DIR_FORWARD;
    else
        nRet = DIR_ABAFT;

    return nRet;
}

void checkWMEnctr(float RTplayerShipX, float RTplayerShipZ, int scale, ref bStorm, ref bTornado, ref distStorm)
{
    bStorm 		= false;
    distStorm 	= 0.0;

    if(!bMapEnter) return;   // если выход на глобус запрещён

    int 	i;
    ref 	rSGroup;
    aref 	WME;
    makearef(WME, worldmap.encounters);
	
    aref 	rRawGroup, encDataForSlot, fndGroup;
	
    int 	nWME = GetAttributesNum(WME);
    int 	iNumWarShips 		= 0;
    int 	iNumMerchantShips 	= 0;
    int 	nShipCnt 			= 0;
    bool 	bFound 				= false;
    float 	fScale 				= makefloat(scale);
    float 	x, z, ay, dist, xs, zs, dists;
    string 	encID = "";
	
    bTornado = false;

    RTplayerShipX *= fScale;
    RTplayerShipZ *= fScale;

    for(i = 0; i < nWME, i++)
    {
        if(bStorm && bFound) break;
        rRawGroup = GetAttributeN(WME, i);
        if(!CheckAttribute(rRawGroup, "type")) continue;
        if(CheckAttribute(rRawGroup, "needDelete")) continue;
        if(CheckAttribute(rRawGroup, "dirSailEnc")) continue;
		
        if(rRawGroup.type == "Storm") 
		{
            if(bStorm) continue;
			
            xs = stf(rRawGroup.px) * fScale;
            zs = stf(rRawGroup.pz) * fScale;
			
            dists = GetDistance2DRel(RTplayerShipX, RTplayerShipZ, xs, zs);
            if(dists > DIR_SAIL_STORMWARN_DIST_REL) continue;
 
            bStorm 		= true;
            distStorm 	= dists;
			
            if(CheckAttribute(rRawGroup, "isTornado") && sti(rRawGroup.isTornado) != 0)
                    bTornado = true;
            if(dists <= DIR_SAIL_STORM_DIST_REL)
                    rRawGroup.needDelete = "Reload delete storm encounter";
        }
        else 
		{
            if(bFound) continue;

            if(!CheckAttribute(rRawGroup, "encdata")) continue;
			
			// mitrokosta эта проверка должна быть выше! -->
			x = stf(rRawGroup.x) * fScale;
			z = stf(rRawGroup.z) * fScale;
			ay = stf(rRawGroup.ay);
			dist = GetDistance2DRel(RTplayerShipX, RTplayerShipZ, x, z);

			if(dist > DIRECTENCOUNTERRANDDISTANCE)
				continue;
			// <--

            encID = ""; 
            encID = GetAttributeName(rRawGroup);
			
            makearef(encDataForSlot, rRawGroup.encdata);
            
            rSGroup = Group_GetGroupByID("Sea_" + encDataForSlot.GroupName);
            if (CheckAttribute(rSGroup, "AlreadyLoaded")) continue;

            nShipCnt = iNumShips;

            if(CheckAttribute(encDataForSlot, "NumMerchantShips"))
            {
                iNumMerchantShips = sti(encDataForSlot.NumMerchantShips);
                nShipCnt += iNumMerchantShips;
            }
            if(CheckAttribute(encDataForSlot, "NumWarShips"))
            {
                iNumWarShips = sti(encDataForSlot.NumWarShips);
                nShipCnt += iNumWarShips;
            }
            if(nShipCnt > MAX_SHIPS_IN_LOCATION) continue;

            fndGroup = rRawGroup;
            bFound = true;
        }	
    }
	
		
    if(!bFound || encID == "") return;

    rRawGroup = fndGroup;
	
    LaunchDirSailEnc();

    evtDirSail(rRawGroup, encDataForSlot, iNumMerchantShips, iNumWarShips, RTplayerShipX, RTplayerShipZ, x, ay, z, dist, fScale, encID);

    return;
}
