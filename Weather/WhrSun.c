object SunGlow;

void WhrDeleteSunGlowEnvironment()
{
	if (isEntity(&SunGlow))
	{
		DeleteClass(&SunGlow);
	}
	DeleteAttribute(&SunGlow,"");
}

void WhrCreateSunGlowEnvironment()
{

	// create sunglow
	if (!isEntity(&SunGlow))
	{
		CreateEntity(&SunGlow,"SunGlow");
		LayerAddObject(SEA_SUNROAD,&SunGlow,-1);
	}
	WhrFillSunGlowData(iCurWeatherNum, iBlendWeatherNum);
	SunGlow.isDone = true;
}

float Whr_MoonReflectionSize(float fTime)
{
	float fCurveTime = fTime;
	if (fCurveTime < 21.0) { fCurveTime = fCurveTime + 24.0; }

	aref aCurve, aCurrent, aNext;
	makearef(aCurve, WhrCommonParams.Sun.MoonRefl);
	int n = GetAttributesNum(aCurve);
	if (n < 1) { return 0.0; }

	int i;
	for (i = 0; i < n - 1; i++)
	{
		aCurrent = GetAttributeN(aCurve, i);
		aNext = GetAttributeN(aCurve, i + 1);
		float fT1 = stf(aCurrent.Time);
		float fT2 = stf(aNext.Time);
		if (fCurveTime >= fT1 && fCurveTime < fT2)
		{
			return Whr_BlendFloat((fCurveTime - fT1) / (fT2 - fT1), stf(aCurrent.Size), stf(aNext.Size));
		}
	}
	aCurrent = GetAttributeN(aCurve, n - 1);
	if (fCurveTime >= stf(aCurrent.Time)) { return stf(aCurrent.Size); }
	return 0.0;
}

void WhrFillSunGlowData(int nw1, int nw2)
{
	if( nw1<0 || nw1>=MAX_WEATHERS ) {return;}

	SunGlow.Clear = "";
	DeleteAttribute(&SunGlow,"");

	aref aSun1;			makearef(aSun1, Weathers[nw1].Sun);
	aref aGlow1;		makearef(aGlow1, aSun1.Glow);
	aref aFlare1;		makearef(aFlare1, aSun1.Flare);
	aref aOverflow1;	makearef(aOverflow1, aSun1.Overflow);
	aref aReflection1;	makearef(aReflection1, aSun1.Reflection);
	ref mchr = GetMainCharacter();
	string sLocation = mchr.location;
	aref aFlares,aFlareN;
	int iNumFlares, i;
	string sTemp;

	float fSunTime = GetTime();
	int   iMoonDay = sti(Environment.date.day);

	if (nw2 >= 0 || sunIsShine)
	{
		if (sti(aSun1.Glow.Enable) != false)
		{
			SunGlow.Glow.Dist = Whr_GetFloat(aGlow1,"Dist");
			SunGlow.Glow.Size = Whr_GetFloat(aGlow1,"Size");
			SunGlow.Glow.Color = Whr_GetColor(aGlow1,"Color");
			SunGlow.Glow.DecayTime = Whr_GetFloat(aGlow1,"DecayTime");
			SunGlow.Glow.TechniqueZ = Whr_GetString(aGlow1,"TechniqueZ");
			SunGlow.Glow.TechniqueNoZ = Whr_GetString(aGlow1,"TechniqueNoZ");
		}

		if (sti(aSun1.Flare.Enable) != false)
		{
			SunGlow.Flare.Dist = Whr_GetFloat(aFlare1,"Dist");
			SunGlow.Flare.Technique = Whr_GetString(aFlare1,"Technique");
			SunGlow.Flare.Scale = Whr_GetFloat(aFlare1,"Scale");
			SunGlow.Flare.Texture = Whr_GetString(aFlare1,"Texture");
			SunGlow.Flare.TexSizeX = Whr_GetString(aFlare1,"TexSizeX");
			SunGlow.Flare.TexSizeY = Whr_GetString(aFlare1,"TexSizeY");

			makearef(aFlares,aSun1.Flares);
			iNumFlares = GetAttributesNum(aFlares);
			for (i=0;i<iNumFlares;i++)
			{
				aFlareN = GetAttributeN(aFlares,i);
				sTemp = "f" + i;
				SunGlow.Flares.(sTemp) = GetAttributeValue(aFlareN);
			}
		}

		if (sti(aSun1.Overflow.Enable) != false)
		{
			SunGlow.Overflow.Texture = Whr_GetString(aOverflow1,"Texture");
			SunGlow.Overflow.Technique = Whr_GetString(aOverflow1,"Technique");
			SunGlow.Overflow.Size = Whr_GetFloat(aOverflow1,"Size");
			SunGlow.Overflow.Color = Whr_GetColor(aOverflow1,"Color");
			SunGlow.Overflow.Start = Whr_GetFloat(aOverflow1,"Start");
		}

		if (sti(aSun1.Reflection.Enable) != false)
		{
			SunGlow.Reflection.Texture = Whr_GetString(aReflection1, "Texture");
			SunGlow.Reflection.Technique = Whr_GetString(aReflection1, "Technique");
			SunGlow.Reflection.Size = Whr_GetFloat(aReflection1, "Size");
			SunGlow.Reflection.Dist = Whr_GetFloat(aReflection1, "Dist");
			SunGlow.Reflection.Color = Whr_GetColor(aReflection1, "Color");
		}

		SunGlow.Moon = aSun1.Moon;
	}

	aref aSunC; makearef(aSunC, WhrCommonParams.Sun);
	if( fSunTime < Whr_GetFloat(aSunC, "NightTo") || fSunTime > Whr_GetFloat(aSunC, "NightFrom") )
	{
		SunGlow.Moon = true;
		SunGlow.Reflection.Color = argb(0,255,255,255);
		SunGlow.Reflection.Texture = Whr_GetString(aSunC, "ReflNightTex");

		SunGlow.Reflection.Size = 0;
		bool bMoonUp = false;
		if (fSunTime >= stf(WhrCommonParams.Sun.MoonArc.BegTime)) { bMoonUp = true; }
		if (fSunTime <  stf(WhrCommonParams.Sun.MoonArc.EndTime)) { bMoonUp = true; }
		bool bMoonPhaseVisible = false;
		if (iMoonDay >= sti(aSunC.MoonDayFrom))
		{
			if (iMoonDay <= sti(aSunC.MoonDayTo)) { bMoonPhaseVisible = true; }
		}
		if (bMoonUp && bMoonPhaseVisible)
		{
			SunGlow.Reflection.Size = Whr_MoonReflectionSize(fSunTime);
		}
	}
	else {
		SunGlow.Moon = false;
	}

	if (nw2 >= 0 || sunIsShine)
	{
		aref aSun; makearef(aSun, WhrCommonParams.Sun);

		SunGlow.Glow.SunTexture  = Whr_GetString(aSun, "BaseSunTex");
		SunGlow.Glow.MoonTexture = Whr_GetString(aSun, "BaseMoonTex");
		SunGlow.Glow.GlowTexture = Whr_GetString(aSun, "BaseGlowTex");

		SunGlow.Reflection.Color = Whr_GetColor(aSun, "ReflColor");

		Whr_ApplySunStep(fSunTime);

		if (CheckAttribute(&WeatherParams, "Storm") && sti(WeatherParams.Storm) == 1)
		{
			SunGlow.Glow.Size = 0;
			SunGlow.Reflection.Size = 0;
		}

		iMoonDay = sti(Environment.date.day);
		string sMoonDay = "" + iMoonDay;
		if (iMoonDay < 10) { sMoonDay = "0" + iMoonDay; }
		SunGlow.Glow.MoonTexture = Whr_GetString(aSun, "MoonDir") + sMoonDay + ".tga.tx";

		SunGlow.Glow.MoonSize    = Whr_GetFloat(aSun, "MoonSize");
		SunGlow.Glow.Color       = Whr_GetColor(aSun, "GlowColor");

		if (CheckAttribute(&WeatherParams, "Rain") && sti(WeatherParams.Rain) && !bCurWeatherStorm)
		{
			SunGlow.Glow.Size     = 0;
			SunGlow.Glow.SunSize  = 0;
			SunGlow.Glow.MoonSize = 0;
			SunGlow.Overflow.Size = 0;
		}
	}
}

void Whr_ApplySunStep(float fTime)
{
	aref aSun; makearef(aSun, WhrCommonParams.Sun);
	if (fTime <  Whr_GetFloat(aSun, "StepFrom")) { return; }
	if (fTime >= Whr_GetFloat(aSun, "StepTo"))   { return; }

	aref aSteps; makearef(aSteps, WhrCommonParams.SunStep);
	int  n = GetAttributesNum(aSteps);
	if (n < 1) { return; }

	aref aStep, aNext, aCur;
	bool bFound = false;
	bool bHaveNext = false;
	int  i;
	for (i = 0; i < n; i++)
	{
		aCur = GetAttributeN(aSteps, i);
		if (Whr_GetFloat(aCur, "Time") > fTime)
		{
			makearef(aNext, aCur);
			bHaveNext = true;
			break;
		}
		makearef(aStep, aCur);
		bFound = true;
	}
	if (!bFound) { return; }

	if (CheckAttribute(aStep, "SunSize"))    { SunGlow.Glow.SunSize       = Whr_GetFloat(aStep, "SunSize"); }
	if (CheckAttribute(aStep, "Size"))       { SunGlow.Glow.Size          = Whr_GetFloat(aStep, "Size"); }

	if (CheckAttribute(aStep, "ReflSize"))
	{
		float fRefl = Whr_GetFloat(aStep, "ReflSize");
		if (bHaveNext)
		{
			if (CheckAttribute(aNext, "ReflSize"))
			{
				float fT1 = Whr_GetFloat(aStep, "Time");
				float fT2 = Whr_GetFloat(aNext, "Time");
				if (fT2 > fT1)
				{
					float fPart = (fTime - fT1) / (fT2 - fT1);
					fRefl = Whr_BlendFloat(fPart, fRefl, Whr_GetFloat(aNext, "ReflSize"));
				}
			}
		}
		SunGlow.Reflection.Size = fRefl;
	}

	if (CheckAttribute(aStep, "ReflColor"))
	{
		int iRefl = Whr_GetColor(aStep, "ReflColor");
		if (bHaveNext)
		{
			if (CheckAttribute(aNext, "ReflColor"))
			{
				float fCT1 = Whr_GetFloat(aStep, "Time");
				float fCT2 = Whr_GetFloat(aNext, "Time");
				if (fCT2 > fCT1)
				{
					float fCPart = (fTime - fCT1) / (fCT2 - fCT1);
					iRefl = Whr_BlendColor(fCPart, iRefl, Whr_GetColor(aNext, "ReflColor"));
				}
			}
		}
		SunGlow.Reflection.Color = iRefl;
	}
	if (CheckAttribute(aStep, "FlareScale")) { SunGlow.Flare.Scale        = Whr_GetFloat(aStep, "FlareScale"); }
	if (CheckAttribute(aStep, "SunTex"))     { SunGlow.Glow.SunTexture    = Whr_GetString(aStep, "SunTex"); }
	if (CheckAttribute(aStep, "GlowTex"))    { SunGlow.Glow.GlowTexture   = Whr_GetString(aStep, "GlowTex"); }
	if (CheckAttribute(aStep, "ReflTex"))    { SunGlow.Reflection.Texture = Whr_GetString(aStep, "ReflTex"); }
	if (CheckAttribute(aStep, "OverTex"))
	{
		SunGlow.Overflow.Texture = Whr_GetString(aStep, "OverTex");

		if (Whr_GetFloat(aStep, "Time") < 12.0) { SunGlow.Overflow.Color = Whr_GetColor(aSun, "OverColorM"); }
	}
}

void MoveSunGlowToLayers(int sExecuteLayer, int sRealizeLayer)
{
	LayerDelObject(EXECUTE,&SunGlow);
	LayerDelObject(REALIZE,&SunGlow);
	LayerDelObject(SEA_EXECUTE,&SunGlow);
	LayerDelObject(SEA_REALIZE,&SunGlow);

	LayerAddObject(sExecuteLayer, &SunGlow, -2);
	LayerAddObject(sRealizeLayer, &SunGlow, -2);
}

