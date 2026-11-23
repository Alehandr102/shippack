

void WhrDeleteSeaEnvironment()
{
	DeleteAttribute(&Sea,"");
}

void Whr_InitLocationEvents()
{
	DelEventHandler("EventLoadLocation", "Whr_OnLocationLoaded");
	SetEventHandler("EventLoadLocation", "Whr_OnLocationLoaded", 0);
}

void Whr_OnLocationLoaded()
{
	if (!isEntity(&Sea)) { return; }

	ref mchr = GetMainCharacter();
	if (!CheckAttribute(mchr, "location")) { return; }
	if (FindLocation(mchr.location) == -1) { return; }

	// CSP uses a negative height as an explicit request to suppress the sea.
	if (CheckAttribute(&Sea, "MaxSeaHeight") && stf(Sea.MaxSeaHeight) < 0.0)
	{
		Whr_UpdateLocationSeaFog();
		return;
	}

	Sea.MaxSeaHeight = Whr_LocationMaxSeaHeight(mchr.location);
	Whr_UpdateLocationSeaFog();
}

void Whr_UpdateLocationSeaFog()
{
	if (!isEntity(&Sea)) { return; }
	if (bSeaActive) { return; }

	ref mchr = GetMainCharacter();
	if (!CheckAttribute(mchr, "location")) { return; }
	if (FindLocation(mchr.location) == -1) { return; }

	Sea.Fog.Color      = stf(Weather.Fog.Color);
	Sea.Fog.SeaDensity = stf(Weather.Fog.SeaDensity);

	ref loc = &Locations[FindLocation(mchr.location)];
	if (CheckAttribute(loc, "underwater") && sti(loc.underwater))
	{
		Render.BackColor = Weather.Fog.Color;
	}
}

string Whr_WaterKind(string sLocation)
{
	int iLoc = FindLocation(sLocation);
	if (iLoc == -1) { return ""; }

	ref loc;
	loc = &Locations[iLoc];
	if (!CheckAttribute(loc, "type")) { return ""; }

	string sType = loc.type;

	if (sType == "seashore")    { return "land"; }
	if (sType == "x_seashore")  { return "land"; }
	if (sType == "questisland") { return "land"; }
	if (sType == "mayak")       { return "land"; }
	if (sType == "fort")        { return "land"; }
	if (sType == "cave")        { return "cave";  }
	if (sType == "Cave")        { return "cave";  }
	if (sType == "Dungeon")     { return "cave";  }
	if (sType == "town")        { return "land";  }

	if (sType == "shipyard")    { return "land";  }

	if (sType == "deck")        { return "sea";   }
	if (sType == "deck_fight")  { return "sea";   }

	return "";
}

string Whr_WavePlace(string sLocation)
{
	string sKind = Whr_WaterKind(sLocation);
	if (sKind == "") { return "other"; }
	if (CheckAttribute(&WhrCommonParams, "Waves.Place." + sKind)) { return sKind; }
	return "other";
}

float Whr_PlaceHeight(string sPlace)
{
	aref aP;
	makearef(aP, WhrCommonParams.Waves.Place.(sPlace));
	if (Whr_IsSeaStorm()) { return stf(aP.StormHeight); }
	return stf(aP.Height);
}

float Whr_LocationMaxSeaHeight(string sLocation)
{
	float fHeight = 0.5;
	int iLoc = FindLocation(sLocation);
	if (iLoc == -1) { return fHeight; }

	if (Whr_WaterKind(sLocation) == "land") { fHeight = 0.55; }

	ref loc = &Locations[iLoc];
	if (CheckAttribute(loc, "MaxWaveHeigh"))
	{
		if (stf(loc.MaxWaveHeigh) > 0.0) { return stf(loc.MaxWaveHeigh); }
	}
	return fHeight;
}

void Whr_UpdateSeaWater()
{
	if (!isEntity(&Sea)) { return; }
	if (iCurWeatherNum < 0) { return; }

	ref mchr = GetMainCharacter();
	if (!CheckAttribute(mchr, "location")) { return; }
	string sLocation = mchr.location;
	int iLoc = FindLocation(sLocation);
	bool bStorm = Whr_IsSeaStorm();

	Whr_CalcHourBlend();
	float fBlend = fHourBlend;
	if (iBlendWeatherNum < 0) { fBlend = 0.0; }
	int iHour = MakeInt(GetTime());
	int iNext = iHour + 1;
	if (iNext > 23) { iNext = 0; }
	string sH = "h" + iHour;
	string sN = "h" + iNext;

	aref aH, aHN;
	makearef(aH,  WhrCommonParams.Waves.Hour.(sH));
	makearef(aHN, WhrCommonParams.Waves.Hour.(sN));
	if (!CheckAttribute(aH, "FoamK") || !CheckAttribute(aHN, "FoamK"))
	{
		Trace("ПОГОДА: нет коэффициента пены на часы " + sH + "/" + sN + " в Common.c");
		return;
	}

	aref aSeaWater;
	makearef(aSeaWater, WhrCommonParams.Water.sea);
	int iColor     = Whr_BlendColor(fBlend, Whr_GetColor(aSeaWater, sH + ".Color"), Whr_GetColor(aSeaWater, sN + ".Color"));
	float fRefl    = Whr_BlendFloat(fBlend, Whr_GetFloat(aSeaWater, sH + ".Reflection"), Whr_GetFloat(aSeaWater, sN + ".Reflection"));
	float fTransp  = Whr_BlendFloat(fBlend, Whr_GetFloat(aSeaWater, sH + ".Transparency"), Whr_GetFloat(aSeaWater, sN + ".Transparency"));
	float fFrenel  = Whr_BlendFloat(fBlend, Whr_GetFloat(aSeaWater, sH + ".Frenel"), Whr_GetFloat(aSeaWater, sN + ".Frenel"));
	float fAtten   = Whr_BlendFloat(fBlend, Whr_GetFloat(aSeaWater, sH + ".Attenuation"), Whr_GetFloat(aSeaWater, sN + ".Attenuation"));
	float fFoamK  = Whr_BlendFloat(fBlend, stf(aH.FoamK), stf(aHN.FoamK));

	aref aS;
	if (bStorm)
	{
		makearef(aS, WhrCommonParams.Waves.Storm);
		if (CheckAttribute(aS, "HourFoamK." + sH)) { fFoamK = stf(aS.HourFoamK.(sH)); }
	}

	aref aV;
	if (iLoc != -1)
	{
		string sView = Whr_LandView();
		makearef(aV, WhrCommonParams.Waves.Land.(sView));
		if (CheckAttribute(aV, "FoamK")) { fFoamK = stf(aV.FoamK); }
	}

	string sKind = Whr_WaterKind(sLocation);

	bool bTable = false;
	float fCoast = 0.0;
	if (!bSeaActive && iLoc != -1) { bTable = true; }
	if (bSeaActive && iLoc == -1 && !bCabinStarted)
	{
		fCoast = Whr_CoastPart(FindIsland(sLocation));
		if (fCoast > 0.0) { sKind = "seashore"; bTable = true; }
	}
	aref aW;
	if (bTable && sKind != "" && sKind != "sea")
	{
		makearef(aW, WhrCommonParams.Water.(sKind));

		if (!CheckAttribute(aW, sH + ".Color") || !CheckAttribute(aW, sN + ".Color"))
		{
			Trace("ПОГОДА: в таблице воды нет ячейки " + sKind + "." + sH + " или " + sN);
		}
		else
		{

			fBlend  = fHourBlend;
			int   iPlaceColor = Whr_BlendColor(fBlend, Whr_GetColor(aW, sH + ".Color"),  Whr_GetColor(aW, sN + ".Color"));
			float fPlaceRefl   = Whr_BlendFloat(fBlend, Whr_GetFloat(aW, sH + ".Reflection"), Whr_GetFloat(aW, sN + ".Reflection"));
			float fPlaceTrans  = Whr_BlendFloat(fBlend, Whr_GetFloat(aW, sH + ".Transparency"), Whr_GetFloat(aW, sN + ".Transparency"));
			float fPlaceFrenel = Whr_BlendFloat(fBlend, Whr_GetFloat(aW, sH + ".Frenel"), Whr_GetFloat(aW, sN + ".Frenel"));
			float fPlaceAtten  = Whr_BlendFloat(fBlend, Whr_GetFloat(aW, sH + ".Attenuation"), Whr_GetFloat(aW, sN + ".Attenuation"));

			if (fCoast > 0.0)
			{
				if (fCoast < 1.0)
				{
					iPlaceColor = Whr_BlendColor(fCoast, iColor, iPlaceColor);
					fPlaceRefl  = Whr_BlendFloat(fCoast, fRefl, fPlaceRefl);
					fPlaceTrans = Whr_BlendFloat(fCoast, fTransp, fPlaceTrans);
					fPlaceFrenel = Whr_BlendFloat(fCoast, fFrenel, fPlaceFrenel);
					fPlaceAtten = Whr_BlendFloat(fCoast, fAtten, fPlaceAtten);
				}
			}

			iColor  = iPlaceColor;
			fRefl   = fPlaceRefl;
			fTransp = fPlaceTrans;
			fFrenel = fPlaceFrenel;
			fAtten = fPlaceAtten;
		}
	}

	aref aPreset;
	makearef(aPreset, Weathers[iCurWeatherNum]);
	if (CheckAttribute(aPreset, "Sea2.WaterColor"))   { iColor  = Whr_GetColor(aPreset, "Sea2.WaterColor"); }
	if (CheckAttribute(aPreset, "Sea2.Reflection"))   { fRefl   = stf(aPreset.Sea2.Reflection); }
	if (CheckAttribute(aPreset, "Sea2.Transparency")) { fTransp = stf(aPreset.Sea2.Transparency); }
	if (CheckAttribute(aPreset, "Sea2.Frenel"))       { fFrenel = stf(aPreset.Sea2.Frenel); }
	if (CheckAttribute(aPreset, "Sea2.Attenuation"))  { fAtten  = stf(aPreset.Sea2.Attenuation); }

	Sea.Sea2.WaterColor   = iColor;
	Sea.Sea2.Reflection   = fRefl;
	Sea.Sea2.Transparency = fTransp;
	Sea.Sea2.Frenel       = fFrenel;
	Sea.Sea2.Attenuation  = fAtten;
	Sea.Sea2.FoamK        = fFoamK;
}

float Whr_SeaCeiling(int iIsland)
{
	aref aC;
	makearef(aC, WhrCommonParams.Waves.Ceiling);
	if (!bSeaActive) { return stf(aC.NoSea); }
	float fOpen = stf(aC.Open);
	if (iIsland == -1) { return fOpen; }

	if (CheckAttribute(&Islands[iIsland], "MaxSeaHeight")) { return stf(Islands[iIsland].MaxSeaHeight); }
	if (!CheckAttribute(pchar, "Ship.Pos.x")) { return fOpen; }

	float fX = stf(pchar.Ship.Pos.x);
	float fZ = stf(pchar.Ship.Pos.z);
	aref aR, aL;
	makearef(aR, Islands[iIsland].reload);
	int n = GetAttributesNum(aR);
	int i;
	float fD;
	float fBest = -1.0;
	float fRad = 0.0;
	for (i = 0; i < n; i++)
	{
		aL = GetAttributeN(aR, i);
		if (!CheckAttribute(aL, "x")) { continue; }
		if (!CheckAttribute(aL, "radius")) { continue; }
		fD = GetDistance2D(fX, fZ, stf(aL.x), stf(aL.z));
		if (fBest < 0.0 || fD < fBest)
		{
			fBest = fD;
			fRad = stf(aL.radius);
		}
	}
	if (fBest < 0.0) { return fOpen; }

	float fK = 1.0;
	if (Whr_IsSeaStorm()) { fK = stf(aC.StormK); }
	if (fBest < fRad * stf(aC.NearR)) { return stf(aC.Near) * fK; }
	if (fBest < fRad * stf(aC.FarR))  { return stf(aC.Far) * fK; }
	return fOpen;
}

float Whr_CoastPart(int iIsland)
{
	if (!bSeaActive) { return 0.0; }
	if (iIsland == -1) { return 0.0; }
	if (!CheckAttribute(pchar, "Ship.Pos.x")) { return 0.0; }

	aref aC;
	makearef(aC, WhrCommonParams.Waves.Ceiling);
	float fX = stf(pchar.Ship.Pos.x);
	float fZ = stf(pchar.Ship.Pos.z);
	aref aR, aL;
	makearef(aR, Islands[iIsland].reload);
	int n = GetAttributesNum(aR);
	int i;
	float fD;
	float fBest = -1.0;
	float fRad = 0.0;
	for (i = 0; i < n; i++)
	{
		aL = GetAttributeN(aR, i);
		if (!CheckAttribute(aL, "x")) { continue; }
		if (!CheckAttribute(aL, "radius")) { continue; }
		fD = GetDistance2D(fX, fZ, stf(aL.x), stf(aL.z));
		if (fBest < 0.0 || fD < fBest)
		{
			fBest = fD;
			fRad = stf(aL.radius);
		}
	}
	if (fBest < 0.0) { return 0.0; }

	float fNear = fRad * stf(aC.NearR);
	float fFar  = fRad * stf(aC.FarR);
	if (fBest <= fNear) { return 1.0; }
	if (fBest >= fFar)  { return 0.0; }
	if (fFar <= fNear)  { return 1.0; }
	return 1.0 - (fBest - fNear) / (fFar - fNear);
}

float Whr_SeaVisible(float fCeil, float fA1, float fA2)
{
	if (fCeil < fA1 + fA2) { return fCeil; }
	return fA1 + fA2;
}

float fCoastWas = -1.0;

void Whr_SeaRetarget()
{
	if (!isEntity(&Sea)) { return; }
	if (!CheckAttribute(&Sea, "Sea2.Amp1")) { return; }
	ref mchr = GetMainCharacter();
	if (FindLocation(mchr.location) != -1) { return; }

	float fA1 = Whr_OpenSeaWave("Amp1");
	Sea.Sea2.Amp1  = fA1;
	Sea.Sea2.Amp2  = Whr_OpenSeaWave("Amp2");
	Sea.Sea2.FoamV = fA1 * Whr_OpenSeaWave("FoamVK");
	Sea.MaxSeaHeight = Whr_SeaCeiling(FindIsland(mchr.location));
	pchar.SystemInfo.ScaleSeaHeight = GetScaleSeaHeight();

	Sea.Sea2.AnimSpeed1 = Whr_OpenSeaWave("Anim1");
	Sea.Sea2.AnimSpeed2 = Whr_OpenSeaWave("Anim2");
	Sea.Sea2.MoveSpeed1 = Whr_WaveMove(Whr_OpenSeaWave("Move1"), 0.0);
	Sea.Sea2.MoveSpeed2 = Whr_WaveMove(Whr_OpenSeaWave("Move2"), stf(WhrCommonParams.Waves.Base.Move2Turn));
}

#event_handler("Whr_SeaCeilingNow", "Whr_SeaCeilingNow");
void Whr_SeaCeilingNow()
{
	if (!isEntity(&Sea)) { return; }
	if (!bSeaActive) { return; }
	if (bAbordageStarted) { return; }
	if (bCabinStarted) { return; }
	ref mchr = GetMainCharacter();
	if (FindLocation(mchr.location) != -1) { return; }

	Whr_SeaRetarget();
}

void Whr_SeaCoastStep()
{
	if (!bSeaActive) { return; }
	if (bAbordageStarted) { return; }
	if (bCabinStarted) { return; }
	if (!isEntity(&Sea)) { return; }
	ref mchrC = GetMainCharacter();
	if (!CheckAttribute(mchrC, "location")) { return; }
	if (FindLocation(mchrC.location) != -1) { return; }

	float fNow = Whr_CoastPart(FindIsland(mchrC.location));
	float fDiff = fNow - fCoastWas;
	if (fDiff < 0.0) { fDiff = -fDiff; }
	if (fDiff < 0.02) { return; }

	fCoastWas = fNow;
	Whr_UpdateSeaWater();
}

void WhrCreateSeaEnvironment()
{
	aref	aCurWeather = GetCurrentWeather();
	float fMaxSeaHeight = 0.0;

	if (CheckAttribute(&Sea,"MaxSeaHeight")) { fMaxSeaHeight = stf(Sea.MaxSeaHeight); }
	if(fMaxSeaHeight < 0.0) return;
	DeleteAttribute(&Sea,"");

	fCoastWas = -1.0;

	ref mchr = GetMainCharacter();
	string sLocation = mchr.location;
	int iLoc = FindLocation(sLocation);

	if (iLoc != -1 && !bCabinStarted)
	{
		Sea.MaxDim = 65536 * 2;
		Sea.MinDim = 128;
		Sea.Sea2.LodScale = 0.5;
		Sea.Sea2.GridStep = 0.05;
	}
	else
	{
		SetSeaSettings();
	}

	Sea.Lights = aCurWeather.Lights;

	string sCurFog = Whr_GetCurrentFog();

	Sea.Fog.Color = stf(Weather.Fog.Color);
	Sea.Fog.Enable = Whr_GetLong(aCurWeather, sCurFog + ".Enable");

	Sea.Fog.SeaDensity = stf(Weather.Fog.SeaDensity);

	string sPlace = "";
	if (iLoc != -1) { sPlace = Whr_WavePlace(sLocation); }
	Whr_SetSeaWaves(sPlace);
	if (iLoc == -1 || bCabinStarted)
	{
		fMaxSeaHeight = Whr_SeaCeiling(FindIsland(sLocation));
	}
	else
	{
		fMaxSeaHeight = Whr_LocationMaxSeaHeight(sLocation);
	}

	Sea.Sea2.FoamEnable = false;
	if (iLoc == -1) { Sea.Sea2.FoamEnable = true; }
	if (iLoc != -1 && sPlace != "other") { Sea.Sea2.FoamEnable = true; }

	Whr_UpdateSeaWater();

	Whr_UpdateLocationSeaFog();

	Sea.Sea2.SimpleSea = sti(InterfaceStates.SimpleSea);

	Sea.Sea2.SkyColor = sti(WhrCommonParams.Waves.Base.SkyColor);

	Sea.MaxSeaHeight = fMaxSeaHeight;
	Sea.CubeMap.Size = 1024;
	Sea.CubeMap.VectorsSize = 512;
	Sea.CubeMap.Format = "r5g6b5";

	Sea.isDone = "";

	if (bSeaActive && !bAbordageStarted)
	{
		pchar.SystemInfo.ScaleSeaHeight = GetScaleSeaHeight();
	}
}

void SetSeaSettings()
{
	// Значения Легенды (MaxDim 65536*14, LodScale 0.5, GridStep 0.051)
	// здесь не подходят: границы LOD подходят почти втрое ближе к камере,
	// из-за чего широкая полоса средней/дальней воды скачет фрагментами.
	// Меньший GridStep при большом поле также переполняет буфер вершин.
	Sea.MaxDim = 65536 * 8;
	Sea.MinDim = 128;

	Sea.Sea2.LodScale = 0.18;
	Sea.Sea2.GridStep = 0.09 + 0.1 * (1.0 - stf(InterfaceStates.SeaDetails));
}

void SetSeaGridStep(float SeaDetails)
{
	if (!IsEntity(&Sea)) { return; }

	ref mchr = GetMainCharacter();
	string sLocation = mchr.location;

	// if current location is land - return
	if(FindLocation(sLocation) != -1)
	{
		if(CheckAttribute(&locations[FindLocation(sLocation)], "fastreload"))
		{
			return;
		}
	}

	Sea.Sea2.GridStep = 0.09 + 0.1 * (1.0 - SeaDetails);

	Sea.Sea2.SimpleSea = sti(InterfaceStates.SimpleSea);
}

float GetScaleSeaHeight()
{
	if (!CheckAttribute(&Sea, "Sea2.Amp1")) { return 0.0; }
	float fMaxSeaHeight = 0.0;
	if (CheckAttribute(&Sea, "MaxSeaHeight")) { fMaxSeaHeight = stf(Sea.MaxSeaHeight); }
	return Whr_SeaVisible(fMaxSeaHeight, stf(Sea.Sea2.Amp1), stf(Sea.Sea2.Amp2)) / 2.0;
}
