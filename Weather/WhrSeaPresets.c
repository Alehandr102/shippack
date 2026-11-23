

bool Whr_IsSeaStorm()
{
	if (CheckAttribute(&WeatherParams, "Storm"))
	{
		if (sti(WeatherParams.Storm)) { return true; }
	}
	if (iCurWeatherNum < 0) { return false; }
	if (CheckAttribute(&Weathers[iCurWeatherNum], "Storm"))
	{
		if (sti(Weathers[iCurWeatherNum].Storm)) { return true; }
	}
	return false;
}

float Whr_WaveCurve(float fWind, string sField)
{
	aref aC;
	makearef(aC, WhrCommonParams.Waves.Curve);
	int n = GetAttributesNum(aC);
	if (n < 1)
	{
		Trace("ПОГОДА: нет кривой волн в Common.c");
		return 0.0;
	}

	aref aP, aN;
	float fW0, fW1;
	int i;
	aP = GetAttributeN(aC, 0);
	if (fWind <= stf(aP.Wind)) { return stf(aP.(sField)); }
	for (i = 1; i < n; i++)
	{
		aP = GetAttributeN(aC, i - 1);
		aN = GetAttributeN(aC, i);
		fW1 = stf(aN.Wind);
		if (fWind <= fW1)
		{
			fW0 = stf(aP.Wind);
			return Whr_BlendFloat((fWind - fW0) / (fW1 - fW0), stf(aP.(sField)), stf(aN.(sField)));
		}
	}
	aN = GetAttributeN(aC, n - 1);
	return stf(aN.(sField));
}

float Whr_OpenSeaWave(string sField)
{
	aref aS;
	if (Whr_IsSeaStorm())
	{
		makearef(aS, WhrCommonParams.Waves.Storm);
		if (sField == "Amp1") { return fWeatherSpeed * stf(aS.Amp1K) + stf(aS.Amp1B); }
		return stf(aS.(sField));
	}
	if (sField == "FoamVK") { return stf(WhrCommonParams.Waves.Base.FoamVK); }
	return Whr_WaveCurve(fWeatherSpeed, sField);
}

string Whr_WaveMove(float fSpeed, float fTurn)
{
	float fA = fWeatherAngle + fTurn;
	float fX = -fSpeed * sin(fA);
	float fZ = -fSpeed * cos(fA);
	return "" + fX + ", 0.0, " + fZ;
}

float Whr_Range(float fValue, float fMin, float fMax)
{
	if (fValue < fMin) { return fMin; }
	if (fValue > fMax) { return fMax; }
	return fValue;
}

string Whr_LandView()
{
	if (Whr_IsSeaStorm()) { return "storm"; }
	if (fWeatherSpeed > stf(WhrCommonParams.Waves.Land.WindStorm)) { return "storm"; }
	return "normal";
}

void Whr_SetSeaWaves(string sPlace)
{

	Sea.MaxSeaDistance = stf(WhrCommonParams.Waves.Base.MaxSeaDistanceLand);
	if (sPlace == "") { Sea.MaxSeaDistance = stf(WhrCommonParams.Waves.Base.MaxSeaDistance); }

	if (sPlace == "")
	{
		Sea.Sea2.Amp1           = Whr_OpenSeaWave("Amp1");
		Sea.Sea2.Amp2           = Whr_OpenSeaWave("Amp2");
		Sea.Sea2.Scale1         = Whr_OpenSeaWave("Scale1");
		Sea.Sea2.Scale2         = Whr_OpenSeaWave("Scale2");
		Sea.Sea2.AnimSpeed1     = Whr_OpenSeaWave("Anim1");
		Sea.Sea2.AnimSpeed2     = Whr_OpenSeaWave("Anim2");
		Sea.Sea2.MoveSpeed1     = Whr_WaveMove(Whr_OpenSeaWave("Move1"), 0.0);
		Sea.Sea2.MoveSpeed2     = Whr_WaveMove(Whr_OpenSeaWave("Move2"), stf(WhrCommonParams.Waves.Base.Move2Turn));
		Sea.Sea2.BumpScale      = Whr_OpenSeaWave("Bump");
		Sea.Sea2.PosShift       = Whr_OpenSeaWave("PosShift");
		Sea.Sea2.FoamV          = stf(Sea.Sea2.Amp1) * Whr_OpenSeaWave("FoamVK");
		Sea.Sea2.FoamUV         = Whr_OpenSeaWave("FoamUV");
		Sea.Sea2.FoamTexDisturb = Whr_OpenSeaWave("TexDist");
		return;
	}

	string sView = Whr_LandView();
	aref aV;
	makearef(aV, WhrCommonParams.Waves.Land.(sView));

	if (sView == "storm")
	{
		Sea.Sea2.Amp1           = stf(aV.Amp1);
		Sea.Sea2.Amp2           = stf(aV.Amp2);
		Sea.Sea2.Scale1         = stf(aV.Scale1);
		Sea.Sea2.Scale2         = stf(aV.Scale2);
		Sea.Sea2.AnimSpeed1     = stf(aV.Anim1);
		Sea.Sea2.AnimSpeed2     = stf(aV.Anim2);
		Sea.Sea2.MoveSpeed1     = aV.Move1;
		Sea.Sea2.MoveSpeed2     = aV.Move2;
		Sea.Sea2.BumpScale      = stf(aV.Bump);
		Sea.Sea2.PosShift       = stf(aV.PosShift);
		Sea.Sea2.FoamV          = stf(aV.FoamV);
		Sea.Sea2.FoamUV         = stf(aV.FoamUV);
		Sea.Sea2.FoamTexDisturb = stf(aV.TexDist);
		return;
	}

	float fAmpMin = stf(aV.AmpMin);
	float fAmpMax = stf(aV.AmpMax);
	float fV;

	fV = Whr_OpenSeaWave("Amp1") * stf(aV.Amp1K);
	Sea.Sea2.Amp1 = Whr_Range(fV, fAmpMin, fAmpMax);
	fV = Whr_OpenSeaWave("Amp2") * stf(aV.Amp2K);
	Sea.Sea2.Amp2 = Whr_Range(fV, fAmpMin, fAmpMax);

	Sea.Sea2.Scale1         = stf(aV.Scale1);
	Sea.Sea2.Scale2         = stf(aV.Scale2);
	Sea.Sea2.AnimSpeed1     = Whr_OpenSeaWave("Anim1");
	Sea.Sea2.AnimSpeed2     = Whr_OpenSeaWave("Anim2");
	Sea.Sea2.MoveSpeed1     = Whr_WaveMove(Whr_OpenSeaWave("Move1"), 0.0);
	Sea.Sea2.MoveSpeed2     = Whr_WaveMove(Whr_OpenSeaWave("Move2"), stf(aV.Move2Turn));
	Sea.Sea2.BumpScale      = stf(aV.Bump);
	Sea.Sea2.PosShift       = Whr_OpenSeaWave("PosShift");
	Sea.Sea2.FoamV          = stf(aV.FoamV);
	Sea.Sea2.FoamUV         = stf(aV.FoamUV);
	Sea.Sea2.FoamTexDisturb = stf(aV.TexDist);
}
