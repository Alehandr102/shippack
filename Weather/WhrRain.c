object Rain;

string Whr_GetRainSkyDir(int nHour)
{
	if (nHour >= 6 && nHour < 10) { return "weather\skies_rain\morning\\"; }
	if (nHour >= 10 && nHour < 18) { return "weather\skies_rain\day\\"; }
	if (nHour >= 18 && nHour < 20) { return "weather\skies_rain\evening\\"; }
	if (nHour >= 20 && nHour < 22) { return "weather\skies_rain\twilight\\"; }
	return "weather\skies_rain\night\\";
}

int Whr_GetRainFogColor(int nHour, string sFog)
{
	if (nHour >= 6 && nHour < 10)
	{
		if (sFog == "SeaFog" || sFog == "SpecialSeaFog") { return argb(0,87,116,132); }
		return argb(0,93,96,105);
	}
	if (nHour >= 10 && nHour < 18)
	{
		if (sFog == "SeaFog" || sFog == "SpecialSeaFog") { return argb(0,99,116,128); }
		return argb(0,112,128,143);
	}
	if (nHour >= 18 && nHour < 20)
	{
		if (sFog == "SeaFog") { return argb(0,54,63,70); }
		if (sFog == "LandFog") { return argb(0,66,78,86); }
		return argb(0,30,39,46);
	}
	if (nHour >= 20 && nHour < 22)
	{
		if (sFog == "SeaFog") { return argb(0,11,17,33); }
		if (sFog == "LandFog") { return argb(0,20,29,43); }
		return argb(0,11,17,33);
	}
	return argb(0,2,4,7);
}

bool Whr_IsRainSkyHour(int nMin, int nMax)
{
	if (!CheckAttribute(&WeatherParams, "Rain.ThisDay") || !sti(WeatherParams.Rain.ThisDay)) { return false; }
	if (!CheckAttribute(&WeatherParams, "Rain.StartTime") || !CheckAttribute(&WeatherParams, "Rain.Duration")) { return false; }

	int nStart = sti(WeatherParams.Rain.StartTime);
	float fDuration = stf(WeatherParams.Rain.Duration) / 60.0;
	return nMin >= nStart - 1 && nMax <= nStart + fDuration + 1.0;
}

bool Whr_IsOrdinaryRainActive()
{
	return CheckAttribute(&WeatherParams, "Rain") && sti(WeatherParams.Rain) && !bCurWeatherStorm;
}

int Whr_GetRainAwareFogColor(int nWeather, string sFog)
{
	int iColor = Whr_GetColor(&Weathers[nWeather], sFog + ".Color");
	if (Whr_IsOrdinaryRainActive())
	{
		iColor = Whr_GetRainFogColor(sti(Weathers[nWeather].Hour.Min), sFog);
	}
	return iColor;
}

float Whr_GetRainAwareSeaDensity(int nWeather, string sFog)
{
	float fDensity = Whr_GetFloat(&Weathers[nWeather], sFog + ".SeaDensity");
	int nHour = sti(Weathers[nWeather].Hour.Min);
	if (sFog == "LandFog" && nHour >= 10 && nHour < 18 && Whr_IsOrdinaryRainActive())
	{
		fDensity = 0.0010;
	}
	if (sFog == "SeaFog" && nHour >= 18 && nHour < 20 && Whr_IsOrdinaryRainActive())
	{
		fDensity = 0.00060;
	}
	if (sFog == "LandFog" && nHour >= 18 && nHour < 20 && Whr_IsOrdinaryRainActive())
	{
		fDensity = 0.0020;
	}
	if (sFog == "LandFog" && nHour >= 20 && nHour < 22 && Whr_IsOrdinaryRainActive())
	{
		fDensity = 0.0020;
	}
	if (nHour >= 22 || nHour < 6)
	{
		if (Whr_IsOrdinaryRainActive())
		{
			fDensity = fDensity * 1.20;
		}
	}
	return fDensity;
}

void Whr_ApplyRainFogRuntime(int nw1, int nw2)
{
	if (nw1 < 0 || nw1 >= MAX_WEATHERS) { return; }

	string sFog1 = Whr_GetCurrentFog();
	int iColor1 = Whr_GetRainAwareFogColor(nw1, sFog1);
	float fSeaDensity1 = Whr_GetRainAwareSeaDensity(nw1, sFog1);
	if (nw2 < 0 || nw2 >= MAX_WEATHERS)
	{
		Weather.Fog.Color = iColor1;
		Weather.Fog.SeaDensity = fSeaDensity1;
		return;
	}

	string sFog2 = Whr_GetBlendedFog();
	int iColor2 = Whr_GetRainAwareFogColor(nw2, sFog2);
	float fSeaDensity2 = Whr_GetRainAwareSeaDensity(nw2, sFog2);
	Whr_CalcHourBlend();
	float fBlend = fHourBlend;
	Weather.Fog.Color = Whr_BlendColor(fBlend, iColor1, iColor2);
	Weather.Fog.SeaDensity = Whr_BlendFloat(fBlend, fSeaDensity1, fSeaDensity2);
}

void WhrDeleteRainEnvironment()
{
	if (isEntity(&Rain))
	{
		DeleteClass(&Rain);
	}
	DeleteAttribute(&Rain,"");
}

void WhrCreateRainEnvironment()
{
	bWeatherIsRain = false;

	if (!isEntity(&Rain))
	{
		CreateEntity(&Rain,"rain");
	}

	FillRainData(iCurWeatherNum,iBlendWeatherNum);
	Rain.isDone = "";

}

void ClearRainEnvironment()
{
	if (!isEntity(&Rain))
	{
		CreateEntity(&Rain,"rain");
	}

	FillRainData(iCurWeatherNum,iBlendWeatherNum);
	Rain.NumDrops = 0.0;
	Rain.DropLength = 0.1;
	Rain.isDone = "";

	bWeatherIsRain = false;
}

void FillRainData(int nw1, int nw2)
{
	if( nw1 < 0 || nw1 >= MAX_WEATHERS ) {return;}

	aref aRain1; makearef(aRain1, Weathers[nw1].Rain);
	aref aRainbow1; makearef(aRainbow1, Weathers[nw1].Rainbow);

	Rain.Clear = "";
	DeleteAttribute(&Rain,"");

	if( nw2 < 0 )
	{
		Rain.NumDrops = Whr_GetLong(aRain1,"NumDrops");
		Rain.Color = Whr_GetColor(aRain1,"Color");
		Rain.DropLength = Whr_GetFloat(aRain1,"DropLength");
		Rain.Height = Whr_GetFloat(aRain1,"Height");
		Rain.Radius = Whr_GetFloat(aRain1,"Radius");
		Rain.Speed = Whr_GetFloat(aRain1,"Speed");
		Rain.Jitter = Whr_GetFloat(aRain1,"Jitter");
		Rain.WindSpeedJitter = Whr_GetFloat(aRain1,"WindSpeedJitter");
		Rain.TimeBlend = Whr_GetLong(aRain1,"TimeBlend");
		Rain.MaxBlend = Whr_GetLong(aRain1,"MaxBlend");

		Rain.RainDrops.DropsNearNum = Whr_GetLong(aRain1, "DropsNearNum");
		Rain.RainDrops.DropsFarNum = Whr_GetLong(aRain1, "DropsFarNum");
		Rain.RainDrops.DropsNearRadius = Whr_GetFloat(aRain1, "DropsNearRadius");
		Rain.RainDrops.DropsFarRadius = Whr_GetFloat(aRain1, "DropsFarRadius");
		Rain.RainDrops.DropsLifeTime = Whr_GetFloat(aRain1, "DropsLifeTime");
		Rain.RainDrops.DropsSize = Whr_GetFloat(aRain1, "DropsSize");
		Rain.RainDrops.DropsColor = Whr_GetLong(aRain1, "DropsColor");
		Rain.RainDrops.DropsTexture = Whr_GetString(aRain1, "DropsTexture");

		Rain.Rainbow.Enable = Whr_GetLong(aRainbow1,"Enable");
		Rain.Rainbow.Texture = Whr_GetString(aRainbow1,"Texture");

	}
	else
	{
		aref aRain2; makearef(aRain2, Weathers[nw2].Rain);
		aref aRainbow2; makearef(aRainbow2, Weathers[nw2].Rainbow);
		float fBlend = stf(Environment.Time) - sti(Environment.Time);

		Rain.NumDrops = Whr_BlendLong( fBlend, Whr_GetLong(aRain1,"NumDrops"), Whr_GetLong(aRain2,"NumDrops") );
		Rain.Color = Whr_BlendColor( fBlend, Whr_GetColor(aRain1,"Color"), Whr_GetColor(aRain2,"Color") );
		Rain.DropLength = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"DropLength"), Whr_GetFloat(aRain2,"DropLength") );
		Rain.Height = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"Height"), Whr_GetFloat(aRain2,"Height") );
		Rain.Radius = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"Radius"), Whr_GetFloat(aRain2,"Radius") );
		Rain.Speed = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"Speed"), Whr_GetFloat(aRain2,"Speed") );
		Rain.Jitter = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"Jitter"), Whr_GetFloat(aRain2,"Jitter") );
		Rain.WindSpeedJitter = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"WindSpeedJitter"), Whr_GetFloat(aRain2,"WindSpeedJitter") );
		Rain.TimeBlend = Whr_BlendLong( fBlend, Whr_GetLong(aRain1,"TimeBlend"), Whr_GetLong(aRain2,"TimeBlend") );
		Rain.MaxBlend = Whr_BlendLong( fBlend, Whr_GetLong(aRain1,"MaxBlend"), Whr_GetLong(aRain2,"MaxBlend") );

		Rain.RainDrops.DropsNearNum = Whr_BlendLong( fBlend, Whr_GetLong(aRain1, "DropsNearNum"), Whr_GetLong(aRain2, "DropsNearNum"));
		Rain.RainDrops.DropsFarNum = Whr_BlendLong( fBlend, Whr_GetLong(aRain1, "DropsFarNum"), Whr_GetLong(aRain2, "DropsFarNum"));
		Rain.RainDrops.DropsNearRadius = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1, "DropsNearRadius"), Whr_GetFloat(aRain2, "DropsNearRadius"));
		Rain.RainDrops.DropsFarRadius = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1, "DropsFarRadius"), Whr_GetFloat(aRain2, "DropsFarRadius"));
		Rain.RainDrops.DropsLifeTime = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1, "DropsLifeTime"), Whr_GetFloat(aRain2, "DropsLifeTime"));
		Rain.RainDrops.DropsSize = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1, "DropsSize"), Whr_GetFloat(aRain2, "DropsSize"));
		Rain.RainDrops.DropsColor = Whr_GetLong(aRain1, "DropsColor");
		Rain.RainDrops.DropsTexture = Whr_GetString(aRain1, "DropsTexture");

		Rain.Rainbow.Enable = Whr_GetLong(aRainbow1,"Enable") || Whr_GetLong(aRainbow2,"Enable");
		Rain.Rainbow.Texture = Whr_GetString(aRainbow1,"Texture");

	}
	bWeatherIsRain = sti(Rain.NumDrops) > 0;
}

void MoveRainToLayers(int iExecuteLayer, int iRealizeLayer)
{
	LayerDelObject(EXECUTE,&Rain);
	LayerDelObject(REALIZE,&Rain);
	LayerDelObject(SEA_EXECUTE,&Rain);
	LayerDelObject(SEA_REALIZE,&Rain);

	LayerAddObject(iExecuteLayer, &Rain, 65537);
	LayerAddObject(iRealizeLayer, &Rain, 65537);
}
