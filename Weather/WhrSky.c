#event_handler("frame", "UpdateSky");
object Sky;

float fPrevCameraAX = 0;
void WhrDeleteSkyEnvironment()
{
	if (isEntity(&Sky))
	{
		DeleteClass(&Sky);
	}
	DeleteAttribute(&Sky,"");
}

void WhrCreateSkyEnvironment()
{
	aref aCurWeather = GetCurrentWeather();
	aref aSky; makearef(aSky,aCurWeather.Sky);

	DeleteAttribute(&Sky,"");
	if (!isEntity(&Sky))
	{
		CreateEntity(&Sky, "Sky");
		LayerAddObject(SEA_REFLECTION, &Sky, 1);
	}

	FillSkyDir(&Sky);

	Sky.Color = Whr_GetColor(aSky,"Color");

	Sky.RotateSpeed = 0.0;
	Sky.Angle = Whr_GetFloat(aSky,"Angle");
	Sky.Size = Whr_GetFloat(aSky,"Size");

    if(bSeaActive) {
        Sky.Size = Whr_GetFloat(aSky, "Size");
        Sky.techSky = "Sky";
        Sky.techSkyBlend = "SkyBlend";
        Sky.techskyAlpha = "skyblend_alpha";
        Sky.techSkyFog = "SkyFog";
	}

	Sky.isDone = "";
}

void UpdateSky()
{

	if (!CheckAttribute(&Environment, "time")) { return; }

	float windSpeed = 5.0;
	float timeScale = 1.0 + TimeScaleCounter * 0.25;

	if(timeScale <= 2)
	{
		timeScale = 1;
	}
	else
	{
		timeScale /= 2;
	}

	if(CheckAttribute(Weather, "Wind.Speed"))
	{
		windSpeed = stf(Weather.Wind.Speed);
	}

	float fSkyTime = GetTime();
	if (fSkyTime >= 5.0 && fSkyTime < 8.0)
	{
		Sky.RotateSpeed = 0.0;
	}
	else
	{
		if (fSkyTime >= 18.0 && fSkyTime < 21.0)
		{
			Sky.RotateSpeed = 0.0;
		}
		else
		{
			Sky.RotateSpeed = windSpeed / 16000 / timeScale;
		}
	}
}

void FillSkyDir(aref aSky)
{
	int i;
	string satr;
	aref aCurWeather = GetCurrentWeather();
	string sDir;

	DeleteAttribute(aSky,"Dir");
	if( iBlendWeatherNum < 0 )
	{

		sDir = aCurWeather.Sky.Dir;
		if (sti(aCurWeather.Hour.Min) >= 6 && sti(aCurWeather.Hour.Min) < 22 && Whr_IsRainSkyHour(sti(aCurWeather.Hour.Min), sti(aCurWeather.Hour.Max)))
		{
			sDir = Whr_GetRainSkyDir(sti(aCurWeather.Hour.Min));
		}
		aSky.Dir.d1 = sDir;
		aSky.Dir = GetHour();
	} else {
		for (i=0;i<MAX_WEATHERS;i++)
		{
			if (!CheckAttribute(&Weathers[i], "Hour")) {continue;}
			if (CheckAttribute(&Weathers[i], "Skip") && sti(Weathers[i].Skip)==true) {continue;}
			if (CheckAttribute(&Weathers[i], "Storm")&& sti(Weathers[i].Storm)==true) {continue;}

			satr = "d" + sti(Weathers[i].Hour.Min);
			if( satr=="d24" ) {continue;}

			sDir = Weathers[i].Sky.Dir;
			if (sti(Weathers[i].Hour.Min) >= 6 && sti(Weathers[i].Hour.Min) < 22 && Whr_IsRainSkyHour(sti(Weathers[i].Hour.Min), sti(Weathers[i].Hour.Max)))
			{
				sDir = Whr_GetRainSkyDir(sti(Weathers[i].Hour.Min));
			}

			aSky.Dir.(satr) = sDir;
		}
		aSky.Dir = GetTime();
	}
}

void MoveSkyToLayers(int iExecuteLayer, int iRealizeLayer)
{
	LayerDelObject(EXECUTE,&Sky);
	LayerDelObject(REALIZE,&Sky);
	LayerDelObject(SEA_EXECUTE,&Sky);
	LayerDelObject(SEA_REALIZE,&Sky);

	LayerAddObject(iExecuteLayer,&Sky,2);
    LayerAddObject(iRealizeLayer,&Sky,3);
}
