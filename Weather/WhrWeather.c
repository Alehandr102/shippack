#include "weather\WhrUtils.c"
#include "weather\WhrLightning.c"
#include "weather\WhrRain.c"
#include "weather\WhrSea.c"
#include "weather\WhrSky.c"
#include "weather\WhrSun.c"
#include "weather\WhrTornado.c"
#include "weather\WhrAstronomy.c"
#include "weather\WhrSeaPresets.c"
// Legacy symbols referenced by unmodified Corsairs Ship Pack scripts.

// CSP resets this token before dispatching WeatherTimeUpdate. The current
// weather implementation does not consume it, but the loader must compile.
int iWhrHeavyTick = -1;

// CSP Cabin.c uses the legacy entry point after leaving a ship cabin.
void doShipLightChange(ref aCurWeather)
{
	Whr_ShipLightChange();
}

// Keep weather interior dimming aligned with CSP's location loader without
// depending on the InterfaceStates.DynamicLight option absent in CSP.
bool WhrCsp_IsDynamicLightsEnabled(string sModelPath)
{
	bool bDisabled = HasSubstr(sModelPath, "decks") ||
		HasSubstr(sModelPath, "EstateRooms") ||
		HasSubstr(sModelPath, "Fort_Inside") ||
		HasSubstr(sModelPath, "Incas Temple") ||
		HasSubstr(sModelPath, "Inside") ||
		HasSubstr(sModelPath, "LostShipsCityInsides") ||
		HasSubstr(sModelPath, "TenochtitlanInside") ||
		HasSubstr(sModelPath, "UnderWater");
	return !bDisabled;
}

#define WIND_NORMAL_POWER		20.0

#define MAX_WEATHERS   34

#define FOG_DAY_CHANCE   60

object	Weather, WeatherParams, WhrCommonParams;
int		iNextWeatherNum = -1;
int		iCurWeatherNum = -1;
int		iBlendWeatherNum = -1;
int		iCurWeatherHour = -1;
bool	bCurWeatherStorm = false;
int		iTotalNumWeathers = 0;
string	sLightingPath = "day";
string	sLmLightingPath = "day";
string	sInsideBack = "";
int		iNewExecuteLayer, iNewRealizeLayer;
string	sCurrentFog;

string	sBlendedFog;

float	fHourBlend = 0.0;

void Whr_CalcHourBlend()
{
	float f = stf(Environment.time) - sti(Environment.time);
	if (f < 0.0) { f = 0.0; }
	if (f > 1.0) { f = 1.0; }
	fHourBlend = f * f;
}
float	fWeatherDelta = 0.0;
float	fWeatherAngle, fWeatherSpeed;
float	fFogDensity;
bool	bWhrStatesPending = false;
int		iShadowDensity[2];
int		iPrevWeather = -1;
int		sunIsShine = true;
bool	bWeatherLoaded = false;
bool	bWeatherIsNight = false;
bool	bWeatherIsLight = false;
bool	bWeatherIsRain = false;
bool	bWeatherIsStorm = false;

object Weathers[MAX_WEATHERS];

extern int InitWeather();


#event_handler("EWhr_GetWindAngle", "Whr_GetWindAngle");
#event_handler("EWhr_GetWindSpeed", "Whr_GetWindSpeed");
#event_handler("EWhr_GetShadowDensity", "Whr_GetShadowDensity");
#event_handler("EWhr_GetFogDensity", "Whr_GetFogDensity");
#event_handler("WeatherTimeUpdate", "Whr_TimeUpdate" );

void SetNextWeather(string sWeatherID)
{
	// find weather
	iNextWeatherNum = -1;
	for (int i=0; i<MAX_WEATHERS; i++)
	{
		if (!CheckAttribute(&Weathers[i], "id")) { continue; }
		if (Weathers[i].id == sWeatherID)
		{
			iNextWeatherNum = i;
			return;
		}
	}
}

void WeatherInit()
{
	Whr_InitLocationEvents();
	if (LoadSegment("weather\WhrInit.c"))
	{
		iTotalNumWeathers = InitWeather();
		UnloadSegment("weather\WhrInit.c");
	}
}

void DeleteWeatherEnvironment()
{
	iBlendWeatherNum = -1;
	if (isEntity(&Weather))
	{
		DeleteClass(&Weather);
	}
	DeleteAttribute(&Weather,"");

	WhrDeleteRainEnvironment();
	WhrDeleteSkyEnvironment();
	WhrDeleteSunGlowEnvironment();
	WhrDeleteLightningEnvironment();
	WhrDeleteTornadoEnvironment();
	WhrDeleteSeaEnvironment();
	WhrDeleteAstronomyEnvironment();

	DelEventHandler(WEATHER_CALC_FOG_COLOR,"Whr_OnCalcFogColor");
	DelEventHandler("frame","Whr_OnWindChange");
	DelEventHandler("ShipSceneShowcase","Whr_OnShowcase");

	bWeatherLoaded = false;
}

aref GetCurrentWeather()
{
	aref arWeather;
	makearef(arWeather,Weathers[iCurWeatherNum]);
	return arWeather;
}

bool Whr_IsInsideLocation(string sType)
{
	if (sType == "residence") { return true; }
	if (sType == "tavern")    { return true; }
	if (sType == "house")     { return true; }
	if (sType == "shop")      { return true; }
	if (sType == "shipyard")  { return true; }
	if (sType == "church")    { return true; }
	if (sType == "cave")      { return true; }
	if (sType == "Dungeon")   { return true; }
	return false;
}

bool Whr_IsNoRainLocation(aref rLoc)
{
	if (CheckAttribute(rLoc, "environment.weather.rain"))
	{
		if (!sti(rLoc.environment.weather.rain)) { return true; }
	}
	if (CheckAttribute(rLoc, "type"))
	{
		if (Whr_IsInsideLocation(rLoc.type)) { return true; }
	}
	return false;
}

bool Whr_IsInsideCurrentLocation()
{
	if (!CheckAttribute(pchar, "location")) { return false; }
	int iLoc = FindLocation(pchar.location);
	if (iLoc == -1) { return false; }
	ref rLoc;
	rLoc = &Locations[iLoc];
	if (!CheckAttribute(rLoc, "type")) { return false; }
	return Whr_IsInsideLocation(rLoc.type);
}

void Whr_SelectFogBlocks()
{
	aref aCurWeather = GetCurrentWeather();

	ref mchrFog = GetMainCharacter();
	bool bOnLand = false;
	if (CheckAttribute(mchrFog, "location"))
	{
		if (FindLocation(mchrFog.location) != -1 && !bCabinStarted) { bOnLand = true; }
	}
	sCurrentFog = "SeaFog";
	sBlendedFog = "SeaFog";
	if (bOnLand) { sCurrentFog = "LandFog"; sBlendedFog = "LandFog"; }

	if (!CheckAttribute(&WeatherParams, "Fog.ThisDay")) { return; }
	if (!sti(WeatherParams.Fog.ThisDay)) { return; }
	if (CheckAttribute(&WeatherParams, "Rain.ThisDay") && sti(WeatherParams.Rain.ThisDay)) { return; }
	if (!CheckAttribute(&WeatherParams, "Fog.Type")) { return; }

	if (!bSeaActive && Whr_IsInsideCurrentLocation()) { return; }

	string sBlock = "SpecialLandFog";
	if (bSeaActive) { sBlock = "SpecialSeaFog"; }

	int iFogType = sti(WeatherParams.Fog.Type);

	if (CheckAttribute(aCurWeather, sBlock + ".Type")
	    && sti(aCurWeather.(sBlock).Type) == iFogType)
	{
		sCurrentFog = sBlock;
	}
	if (iBlendWeatherNum > -1
	    && CheckAttribute(&Weathers[iBlendWeatherNum], sBlock + ".Type")
	    && sti(Weathers[iBlendWeatherNum].(sBlock).Type) == iFogType)
	{
		sBlendedFog = sBlock;
	}
}

void Whr_SetDayCycle()
{
	if (!isEntity(&Weather)) { return; }

	aref aS;
	makearef(aS, WhrCommonParams.Sun.Arc);
	Weather.Sun.Height    = stf(aS.Height);
	Weather.Sun.BegAngle  = stf(aS.BegAngle);
	Weather.Sun.BegTime   = stf(aS.BegTime);
	Weather.Sun.EndAngle  = stf(aS.EndAngle);
	Weather.Sun.EndTime   = stf(aS.EndTime);

	aref aM;
	makearef(aM, WhrCommonParams.Sun.MoonArc);
	Weather.Moon.Height   = stf(aM.Height);
	Weather.Moon.BegAngle = stf(aM.BegAngle);
	Weather.Moon.BegTime  = stf(aM.BegTime);
	Weather.Moon.EndAngle = stf(aM.EndAngle);
	Weather.Moon.EndTime  = stf(aM.EndTime);
}

bool bWhrShowcase = false;

void Whr_DimSunInside()
{
	if (!isEntity(&Weather)) { return; }
	if (bSeaActive) { return; }

	ref mchrS = GetMainCharacter();
	if (!CheckAttribute(mchrS, "location")) { return; }
	int iLoc = FindLocation(mchrS.location);
	if (iLoc == -1) { return; }
	if (!CheckAttribute(&Locations[iLoc], "type")) { return; }
	if (!Whr_IsInsideLocation(Locations[iLoc].type)) { return; }
	if (!CheckAttribute(&Locations[iLoc], "filespath.models")) { return; }
	if (!WhrCsp_IsDynamicLightsEnabled(Locations[iLoc].filespath.models)) { return; }
	if (bWhrShowcase) { Whr_ShowcaseAmbient(); return; }

	Weather.Sun.Ambient = sti(WhrCommonParams.Light.Inside.Ambient);
	Weather.Sun.Color   = sti(WhrCommonParams.Light.Inside.SunColor);
}

void Whr_ShowcaseAmbient()
{
	int iFloor = sti(WhrCommonParams.Light.ShowcaseAmbient);
	int col = sti(Weather.Sun.Ambient);
	int r = shr(and(col,16711680), 16);
	int g = shr(and(col,65280), 8);
	int b = and(col,255);

	if (r < iFloor) { r = iFloor; }
	if (g < iFloor) { g = iFloor; }
	if (b < iFloor) { b = iFloor; }

	Weather.Sun.Ambient = argb(0,r,g,b);
}

void Whr_SetShowcase(bool bOpen)
{
	bWhrShowcase = bOpen;
	if (!isEntity(&Weather)) { return; }

	FillWeatherData(iCurWeatherNum, iBlendWeatherNum);
	Whr_DimSunInside();
	Weather.isDone = "";
}

// Событие от окна верфи (мод интерфейса): открыто - свет уличный, закрыто - снова комнатный.
void Whr_OnShowcase()
{
	Whr_SetShowcase(sti(GetEventData()));
}

void CreateWeatherEnvironment()
{
	int iHour = MakeInt(GetHour());

	bool bWhrStorm = false;
	bool bWhrTornado = false;

	if (CheckAttribute(&WeatherParams,"Storm")) { bWhrStorm = sti(WeatherParams.Storm); }
	if (CheckAttribute(&WeatherParams,"Tornado")) { bWhrTornado = sti(WeatherParams.Tornado); }

	bool bRain = Whr_RollDailyEvents(iHour);
	Whr_CleanRainTraces(bRain);
	Whr_PickPreset(iHour, bWhrStorm, bWhrTornado);

	if (Whr_PlaceHas("storm") || Whr_PlaceHas("alwaysStorm") || Whr_PlaceHas("alwaysStorm_2")) { bWhrStorm = true; }
	if (Whr_PlaceHas("tornado")) { bWhrTornado = true; }
	bWeatherIsStorm = bWhrStorm;
	bCurWeatherStorm = bWhrStorm;

	bool bQuestlockWeather = Whr_ApplyOverrides();
	Whr_FillEngineWeather(bQuestlockWeather);
	Whr_BuildEnvironments(bWhrTornado);
}

bool Whr_RollDailyEvents(int iHour)
{
	bool bRain = false;

	int iMonth = GetDataMonth();
	int iTmp, iChance;
	if (CheckAttribute(&WeatherParams,"Rain")) { bRain = sti(WeatherParams.Rain); }
	if (!CheckAttribute(&WeatherParams, "Rain.ThisDay")) WeatherParams.Rain.ThisDay = false;

	int iFogDay = GetDataYear()*10000 + GetDataMonth()*100 + GetDataDay();
	if (!CheckAttribute(&WeatherParams, "Fog.Day") || sti(WeatherParams.Fog.Day) != iFogDay)
	{
		WeatherParams.Fog.Day     = iFogDay;
		WeatherParams.Fog.ThisDay = (rand(99) < FOG_DAY_CHANCE);
		WeatherParams.Fog.Type    = rand(1);
	}

	int iRainDay = GetDataYear()*10000 + GetDataMonth()*100 + GetDataDay();
	bool bRainRollToday = true;
	if (CheckAttribute(&WeatherParams, "Rain.Day"))
	{
		if (sti(WeatherParams.Rain.Day) == iRainDay) { bRainRollToday = false; }
	}

	if (sti(WeatherParams.Rain.ThisDay) && bRainRollToday)
	{
		if (CheckAttribute(&WeatherParams, "Rain.Sound") && sti(WeatherParams.Rain.Sound))
		{
			WeatherParams.Rain.Sound = false;
			if (iCurWeatherNum >= 0) { Whr_SetRainSound(false, sti(Weathers[iCurWeatherNum].Night)); }
		}
		bRain = false;
		WeatherParams.Rain = false;
		WeatherParams.Rain.ThisDay = false;
		DeleteAttribute(&WeatherParams, "Rain.StartTime");
	}

	if (iHour >= 5 && iHour <= 15 && !sti(WeatherParams.Rain.ThisDay) && bRainRollToday)
	{
		WeatherParams.Rain.Day = iRainDay;
		//шанс.
		iChance = 0;

		if (iMonth > 4 && iMonth < 9) iChance = 32;

		bRain = ((rand(50) + rand(50)) < (25 + iChance));

		if (bRain || CheckAttribute(pchar, "questTemp.StartRain"))
		{
			//час начала...
			WeatherParams.Rain.ThisDay   = true;
			WeatherParams.Rain.StartTime = iHour + 2 + rand(2);
			//продолжительность в минутах //1 час мин, иначе небо не успеет.
			WeatherParams.Rain.Duration  = 60 + rand(120);

			WeatherParams.Rain.IsLightingActive = (rand(99) < 30);

			Log_TestInfo("Will raining in : " + WeatherParams.Rain.StartTime);
		}
	}

	//принудительно убираем дождь, если время просрочено
	if (bRain)
	{
		iTmp = sti(WeatherParams.Rain.StartTime) + (sti(WeatherParams.Rain.Duration)/60 + 0.5);
		if (iHour > iTmp)
		{
			bRain = false;
			WeatherParams.Rain = false;
			WeatherParams.Rain.ThisDay = false;
		}
	}

	return bRain;
}

void Whr_CleanRainTraces(bool bRain)
{

	for (int iClr = 0; iClr < MAX_WEATHERS; iClr++)
	{

		if (!CheckAttribute(&Weathers[iClr], "id")) { continue; }

		if (!bRain && CheckAttribute(&Weathers[iClr], "Rain.NumDrops")
		    && sti(Weathers[iClr].Rain.NumDrops) > 0 && sti(Weathers[iClr].Rain.NumDrops) < 5100)
		{
			Weathers[iClr].Rain.NumDrops = 0;
			Weathers[iClr].Lightning.Enable = false;
		}

		if (!bRain && CheckAttribute(&Weathers[iClr], "Rainbow.Enable"))
		{
			Weathers[iClr].Rainbow.Enable = false;
		}
	}
}

void Whr_PickPreset(int iHour, bool bWhrStorm, bool bWhrTornado)
{
	int iWeatherFound[MAX_WEATHERS];
	int iNumWeatherFound = 0;

	if (iNextWeatherNum != -1)
	{
		if (iPrevWeather == -1) { iPrevWeather = iCurWeatherNum; }
		iCurWeatherHour = iHour;
		iCurWeatherNum = iNextWeatherNum;
		iNextWeatherNum = -1;

	}
	else
	{
		// search weather for hour
		for (int i=0;i<MAX_WEATHERS;i++)
		{
			if (!CheckAttribute(&Weathers[i], "hour")) { continue; }
			if (CheckAttribute(&Weathers[i], "skip"))
			{
				if (sti(Weathers[i].skip)) { continue; }
			}

			bool bCanStorm = sti(Weathers[i].Storm);
			bool bCanTornado = sti(Weathers[i].Tornado);
			int iMin = sti(Weathers[i].Hour.Min);
			int iMax = sti(Weathers[i].Hour.Max);
			if (iMin == iMax && iMin != iHour) { continue; }
			if (iMin > iMax)
			{
				if (iHour < iMin && iHour > iMax) { continue; }
			}
			if (iMin < iMax)
			{
				if (iHour < iMin || iHour > iMax) { continue; }
			}
			if (bWhrStorm != bCanStorm) { continue; }
			if (bWhrTornado == true && bWhrTornado != bCanTornado) { continue; }

			iWeatherFound[iNumWeatherFound] = i;
			iNumWeatherFound++;
		}

		iCurWeatherHour = iHour;

		if (iNumWeatherFound == 0)
		{
			Trace("Error: iNumWeatherFound = 0, for hour = " + iHour);
			iCurWeatherNum = 0;
		}
		else
		{
			iCurWeatherNum = iWeatherFound[rand(iNumWeatherFound-1)];
		}
	}
}

bool Whr_PlaceHas(string sFlag)
{
	if (!CheckAttribute(pchar, "location")) { return false; }
	int iLoc = reload_location_index;
	if (iLoc != -1)
	{
		if (CheckAttribute(&locations[iLoc], sFlag)) { return true; }
		return false;
	}
	int iIsl = FindIsland(pchar.location);
	if (iIsl != -1)
	{
		if (CheckAttribute(&Islands[iIsl], sFlag)) { return true; }
	}
	return false;
}

bool Whr_ApplyOverrides()
{
	int iCurLocation;
	int iTestWeather;
	bool bQuestlockWeather;

	bQuestlockWeather = false;
	sunIsShine = true;

	if(CheckAttribute(pchar, "location"))
	{
		iCurLocation = reload_location_index;
		if(iCurLocation != -1)
		{
			//малый шторм в локациях
			if(CheckAttribute(&locations[iCurLocation], "alwaysStorm"))
			{
				if (GetTime() >= 6.0 && GetTime() < 10.0) locations[iCurLocation].QuestlockWeather = "Storm01_add";
				if (GetTime() >= 10.0 && GetTime() < 18.0) locations[iCurLocation].QuestlockWeather = "Storm02_add";
				if (GetTime() >= 18.0 && GetTime() < 22.0) locations[iCurLocation].QuestlockWeather = "Storm03_add";
				if (GetTime() >= 22.0 && GetTime() <= 23.99) locations[iCurLocation].QuestlockWeather = "Storm04_add";
				if (GetTime() >= 0 && GetTime() < 6.0) locations[iCurLocation].QuestlockWeather = "Storm04_add";
				if (CheckAttribute(&locations[iCurLocation], "alwaysStorm.WaveHeigh")) locations[iCurLocation].MaxWaveHeigh = 4.5; //установим уровень воды
			}
			//большой шторм в локациях
			if(CheckAttribute(&locations[iCurLocation], "alwaysStorm_2"))
			{
				if (GetTime() >= 6.0 && GetTime() < 10.0) locations[iCurLocation].QuestlockWeather = "Storm01";
				if (GetTime() >= 10.0 && GetTime() < 18.0) locations[iCurLocation].QuestlockWeather = "Storm02";
				if (GetTime() >= 18.0 && GetTime() < 22.0) locations[iCurLocation].QuestlockWeather = "Storm03";
				if (GetTime() >= 22.0 && GetTime() <= 23.99) locations[iCurLocation].QuestlockWeather = "Storm04";
				if (GetTime() >= 0 && GetTime() < 6.0) locations[iCurLocation].QuestlockWeather = "Storm04";
				if (CheckAttribute(&locations[iCurLocation], "alwaysStorm_2.WaveHeigh")) locations[iCurLocation].MaxWaveHeigh = 40.0; //установим уровень воды
			}
			if(CheckAttribute(&locations[iCurLocation], "QuestlockWeather"))
			{
				iTestWeather = FindWeather(locations[iCurLocation].QuestlockWeather);
				if(iTestWeather != -1)
				{
					iCurWeatherNum = iTestWeather;
					bQuestlockWeather = true;
					if (CheckAttribute(&locations[iCurLocation], "lockWeather") && locations[iCurLocation].lockWeather == "Inside")
					{
						sunIsShine = false; //выключить солнце
					}
				}
			}
		}
		if(iCurLocation == -1)
		{
			iCurLocation = FindIsland(pchar.location);
			if(iCurLocation != -1)
			{
				if(CheckAttribute(&Islands[iCurLocation], "alwaysStorm"))
				{
					if (GetTime() >= 6.0 && GetTime() < 10.0) Islands[iCurLocation].QuestlockWeather = "Storm01";
					if (GetTime() >= 10.0 && GetTime() < 18.0) Islands[iCurLocation].QuestlockWeather = "Storm02";
					if (GetTime() >= 18.0 && GetTime() < 22.0) Islands[iCurLocation].QuestlockWeather = "Storm03";
					if (GetTime() >= 22.0 && GetTime() <= 23.99) Islands[iCurLocation].QuestlockWeather = "Storm04";
					if (GetTime() >= 0 && GetTime() < 6.0) Islands[iCurLocation].QuestlockWeather = "Storm04";
				}
				if(CheckAttribute(&Islands[iCurLocation], "QuestlockWeather"))
				{
					iTestWeather = FindWeather(Islands[iCurLocation].QuestlockWeather);
					if(iTestWeather != -1)
					{
						iCurWeatherNum = iTestWeather;
						bQuestlockWeather = true;
					}
				}
			}
		}
	}

	//ставим погоду, сгенеренную в главном меню
	if (bMainMenu)
	{
		iTestWeather = sti(InterfaceStates.mainmenuweather);
		if(iTestWeather != -1)
		{
			iCurWeatherNum = iTestWeather;
			bQuestlockWeather = true;
		}
	}
	if (startGameWeather)
	{
		iTestWeather = sti(InterfaceStates.startGameWeather);
		if(iTestWeather != -1)
		{
			iCurWeatherNum = iTestWeather;
			bQuestlockWeather = true;
		}
		startGameWeather = false;
	}

	if (!bQuestlockWeather)
	{
		iBlendWeatherNum = FindBlendWeather(iCurWeatherNum);
	}
	else
	{
		iBlendWeatherNum = -1; // залоченная погода
	}

	return bQuestlockWeather;
}

void Whr_FillEngineWeather(bool bQuestlockWeather)
{
	// create main module Weather
	DeleteAttribute(&Weather,"");

	aref arWhrCheck;
	bool bWhrAlive = false;
	if (isEntity(&Weather))
	{
		if (GetEntity(arWhrCheck, "weather")) { bWhrAlive = true; }
	}
	if (!bWhrAlive)
	{
		CreateEntity(&Weather, "Weather");
	}

	Whr_SetDayCycle();

	aref aCurWeather = GetCurrentWeather();

	sLightingPath = Whr_GetString(aCurWeather,"Lighting");
	sLmLightingPath = Whr_GetString(aCurWeather,"LightingLm");
	sInsideBack = Whr_GetString(aCurWeather,"InsideBack");
	bWeatherIsNight = Whr_GetLong(aCurWeather,"Night");
	bWeatherIsLight = Whr_GetLong(aCurWeather,"Lights");

	if(CheckAttribute(pchar, "wind.angle"))
	{
		Weather.Wind.Angle = pchar.wind.angle;

	}
	else
	{
		Weather.Wind.Angle = frand(PIm2);
		pchar.wind.angle = Weather.Wind.Angle;
	}

	if (CheckAttribute(pchar, "wind.speed"))
	{
		Weather.Wind.Speed = pchar.wind.speed;

	}
	else
	{
		Weather.Wind.Speed = Whr_GetFloat(aCurWeather,"Wind.Speed");
		pchar.wind.speed = Weather.Wind.Speed;
	}

	pchar.quest.EraseWind.win_condition.l1 = "ExitFromSea";
	pchar.quest.EraseWind.win_condition = "EraseWind";

	Whr_SelectFogBlocks();

	FillWeatherData(iCurWeatherNum, iBlendWeatherNum);
	Whr_DimSunInside();

	if (iBlendWeatherNum < 0 || bQuestlockWeather)
	{
		Weather.Time.time = GetTime();
		Weather.Time.speed = 350.0; // количество секунд на смену погоды
		Weather.Time.updatefrequence = 12; // количество обновлений на смену погоды
	} else {
		Weather.Time.time = GetTime();
		Weather.Time.speed = 450;
		Weather.Time.updatefrequence = 15; // количество обновлений на смену погоды
		if (bSeaActive && !bAbordageStarted)
		{
			if (iArcadeSails == 1)  // скорости в аркаде больше, время быстрее
			{
				Weather.Time.speed = 250; // количество секунд на смену погоды
				Weather.Time.updatefrequence = 10;
			}
		}
		else
		{ // суша
			Weather.Time.speed = 350;
			Weather.Time.updatefrequence = 12; // количество обновлений на смену погоды
		}
	}

	Weather.isDone = "";

	bWhrStatesPending = true;

	SetEventHandler(WEATHER_CALC_FOG_COLOR,"Whr_OnCalcFogColor",0);
	SetEventHandler("frame","Whr_OnWindChange",0);
	SetEventHandler("ShipSceneShowcase","Whr_OnShowcase",0);


	fWeatherDelta = 0.0;
	fWeatherAngle = stf(Weather.Wind.Angle);

	fWeatherSpeed = stf(Weather.Wind.Speed);
}

int Whr_GetRainStage(int iTime, int iDuration)
{
	int iStage = 3;
	if (iTime < 4 * iDuration / 5) { iStage = 2; }
	if (iTime < 2 * iDuration / 3) { iStage = 1; }
	if (iTime < iDuration / 3) { iStage = 0; }
	return iStage;
}

void Whr_SetRainStageDrops(int iStage, bool bOnlyMissing)
{
	bool bWriteCurrent = true;
	bool bWriteBlend = true;
	if (iStage == 0) { bWriteCurrent = false; }
	if (iStage == 3) { bWriteBlend = false; }

	if (bOnlyMissing)
	{
		if (sti(Weathers[iCurWeatherNum].Rain.NumDrops) != 0) { bWriteCurrent = false; }
		if (sti(Weathers[iBlendWeatherNum].Rain.NumDrops) != 0) { bWriteBlend = false; }
	}

	if (bWriteCurrent)
	{
		Weathers[iCurWeatherNum].Rain.NumDrops = 3500 + rand(1500);
		Weathers[iCurWeatherNum].Rain.DropLength = 4 + rand(8);
		Weathers[iCurWeatherNum].Rain.Color = argb(0,30,30,30);
	}

	if (bWriteBlend)
	{
		Weathers[iBlendWeatherNum].Rain.NumDrops = 3500 + rand(1500);
		Weathers[iBlendWeatherNum].Rain.DropLength = 4 + rand(8);
		Weathers[iBlendWeatherNum].Rain.Color = argb(0,30,30,30);
	}
}

void Whr_RestoreRainDrops()
{
	if (!Whr_IsOrdinaryRainActive()) { return; }
	if (iCurWeatherNum < 0) { return; }
	if (iCurWeatherNum >= MAX_WEATHERS) { return; }
	if (iBlendWeatherNum < 0) { return; }
	if (iBlendWeatherNum >= MAX_WEATHERS) { return; }
	if (!CheckAttribute(pchar, "Rain.Duration.control_year")) { return; }
	if (!CheckAttribute(&WeatherParams, "Rain.Duration")) { return; }

	int iRainDuration = sti(WeatherParams.Rain.Duration);
	if (iRainDuration <= 0) { return; }
	int iRainTime = GetQuestPastMinutesParam("Rain.Duration");
	if (iRainTime < 0) { return; }
	if (iRainTime > iRainDuration) { return; }
	int iStage = Whr_GetRainStage(iRainTime, iRainDuration);
	Whr_SetRainStageDrops(iStage, true);
}

void Whr_BuildEnvironments(bool bWhrTornado)
{
	bool bRain;

	bRain = true; // тут не работает Whr_isRainEnable(); из-за гл меню :(
    string sLocation = pchar.location;
	int iLocation = FindLocation(sLocation);
	if(iLocation != -1)
	{
		ref rLoc;
		rLoc = &Locations[iLocation];

		if (Whr_IsNoRainLocation(rLoc))
		{
			bRain = false;
		}
	}
	//итак, если может быть дождь, создаем окружение
	if (bRain)
	{
		Whr_RestoreRainDrops();
		WhrCreateRainEnvironment();
	}
	//если не может, удаляем.
	else
	{
		ClearRainEnvironment();
	}

	WhrCreateSunGlowEnvironment();
	WhrCreateLightningEnvironment();
	WhrCreateSkyEnvironment();
	WhrCreateSeaEnvironment();
	WhrCreateAstronomyEnvironment();

	if(iLocation != -1)
	{
		if(CheckAttribute(&Locations[iLocation], "lockWeather"))
		{
			bWhrTornado = false;
		}
	}
	if (bWhrTornado) { WhrCreateTornadoEnvironment(); }

	Particles.windpower = 0.05 * Clampf(Whr_GetWindSpeed() / WIND_NORMAL_POWER);
	Particles.winddirection.x = sin(Whr_GetWindAngle());
	Particles.winddirection.z = cos(Whr_GetWindAngle());

	bWeatherLoaded = true;

	if (bSeaActive)
	{
		Island.LightingPath = GetIslandLightingPath();
		Island.FogDensity = Weather.Fog.IslandDensity;

		SendMessage(&IslandReflModel, "lllf", MSG_MODEL_SET_FOG, 1, 1, stf(Weather.Fog.IslandDensity));
	}

}

void Whr_UpdateWeather()
{
	if (!isEntity(&Weather)) { return; }

	CreateWeatherEnvironment();
	MoveWeatherToLayers(iNewExecuteLayer, iNewRealizeLayer);
}

void Whr_LoadNextWeather(int nPlus)
{
	if (!isEntity(&Weather)) { return; }

	iCurWeatherHour = MakeInt(GetHour());
	iCurWeatherNum = iCurWeatherNum + nPlus;

	if (iCurWeatherNum < 0)
	{
		iCurWeatherNum = iTotalNumWeathers - 1;
	}
	if (iCurWeatherNum >= iTotalNumWeathers)
	{
		iCurWeatherNum = 0;
	}

	int nStep = nPlus;
	if (nStep == 0) { nStep = 1; }
	int nTried = 0;
	while (nTried < iTotalNumWeathers && CheckAttribute(&Weathers[iCurWeatherNum], "Skip") && sti(Weathers[iCurWeatherNum].skip))
	{
		iCurWeatherNum = iCurWeatherNum + nStep;
		if (iCurWeatherNum < 0)                  { iCurWeatherNum = iTotalNumWeathers - 1; }
		if (iCurWeatherNum >= iTotalNumWeathers) { iCurWeatherNum = 0; }
		nTried = nTried + 1;
	}
	Log_TestInfo("ПОГОДА: " + Weathers[iCurWeatherNum].id + " ОСВЕЩЕНИЕ: " + Weathers[iCurWeatherNum].Lighting);

	iNextWeatherNum = iCurWeatherNum;
	CreateWeatherEnvironment();
	MoveWeatherToLayers(iNewExecuteLayer, iNewRealizeLayer);

	aref	aCurWeather = GetCurrentWeather();
	iCurWeatherHour = sti(aCurWeather.Hour.Min);

	SetCurrentTime(iCurWeatherHour, 0);
	Weather.Time.time = GetTime();
}

void Whr_OnWindChange()
{
	float fDeltaTime = MakeFloat(GetDeltaTime()) * 0.001;
	fWeatherDelta = fWeatherDelta + fDeltaTime;

	float fSpd = fWeatherSpeed + (fWeatherSpeed / 6.0) * 0.1 * (sin(fWeatherDelta) + sin(0.2 * fWeatherDelta) + sin(PI + 0.8 * fWeatherDelta) + cos(1.5 * fWeatherDelta));
	float fAng = fWeatherAngle + 0.02 * (sin(fWeatherDelta) + sin(0.2 * fWeatherDelta) + sin(PI + 0.8 * fWeatherDelta) + cos(1.5 * fWeatherDelta));

	Weather.Wind.Angle = fAng;
	Weather.Wind.Speed = fSpd;

	Whr_SeaCoastStep();

	if (bWhrStatesPending)
	{
		bWhrStatesPending = false;
		Weather.isDone = "";
	}

}

int Whr_OnCalcFogColor()
{
	int		iAlpha, iColor;
	float	x, y, z;

	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	if( iBlendWeatherNum < 0 )
	{
		aref aCurWeather = GetCurrentWeather();
		iAlpha = 255 - MakeInt(255.0 * Clampf(y / stf(aCurWeather.(sCurrentFog).Height)));

		int iFogColor = sti(Weather.Fog.Color);
		iColor = or(shl(iAlpha, 24), iFogColor);
	}
	else
	{

		Whr_CalcHourBlend();
		float fBlend = fHourBlend;
		float fH = Whr_BlendFloat(fBlend, stf(Weathers[iCurWeatherNum].(sCurrentFog).Height), stf(Weathers[iBlendWeatherNum].(sBlendedFog).Height));
		int iColorTmp = Whr_BlendColor(fBlend, Whr_GetRainAwareFogColor(iCurWeatherNum, sCurrentFog), Whr_GetRainAwareFogColor(iBlendWeatherNum, sBlendedFog));

		iAlpha = 255 - MakeInt(255.0 * Clampf(y / fH));
		iColor = or(shl(iAlpha, 24), iColorTmp);
	}

	return iColor;
}

float Whr_BlendFloat(float fBlend, float f1, float f2)
{
	float f = f1 + fBlend * (f2-f1);
	return f;
}

int Whr_BlendLong(float fBlend, int i1, int i2)
{
	int i = makeint(i1 + fBlend * (i2-i1));
	return i;
}

int Whr_BlendColor(float fBlend, int col1, int col2)
{
	int r1 = shr(and(col1,16711680), 16);
	int r2 = shr(and(col2,16711680), 16);

	int g1 = shr(and(col1,65280), 8);
	int g2 = shr(and(col2,65280), 8);

	int b1 = and(col1,255);
	int b2 = and(col2,255);

	int r = r1 + MakeInt(fBlend * (r2-r1));
	int g = g1 + MakeInt(fBlend * (g2-g1));
	int b = b1 + MakeInt(fBlend * (b2-b1));

	return argb(0,r,g,b);
}

void Whr_TimeUpdate()
{
	float fTime = GetEventData();
	Environment.time = fTime;
	int nOldHour = sti(Environment.date.hour);
	int nNewHour = makeint(fTime);
	int nNewMin = makeint((fTime - nNewHour)*60);
	int nNewSec = makeint(((fTime - nNewHour)*60 - nNewMin)*60);
	Environment.date.min = nNewMin;
	Environment.date.hour = nNewHour;
	Environment.date.sec = nNewSec;
	worldMap.date.hour = nNewHour;
	worldMap.date.min  = nNewMin;
	if( nNewHour < nOldHour ) // обновление дня переход с 23 в 0
	{
		AddDataToCurrent(0,0,1);
		Weather.Time.time = GetTime();
	}

	if( iBlendWeatherNum < 0 ) {return;}

	string sTmp;
	int iTmp, iTime;
	bool bRain = false;
	if (CheckAttribute(&WeatherParams,"Rain")) { bRain = sti(WeatherParams.Rain); }

	iCurWeatherNum = FindWeatherByHour( makeint(fTime) );
	iBlendWeatherNum = FindBlendWeather( iCurWeatherNum );

	if (bSeaActive)
	{

		Island.LightingPath = GetIslandLightingPath();
	}

	bool  bIsRainEnable = Whr_isRainEnable();

	if (bIsRainEnable) // не для локаций, где низя дождить
	{
		//это собственно генератор дождя...
		//стадии нумеруются с нуля.
		if (bRain)
		{
			iTime = GetQuestPastMinutesParam("Rain.Duration");
			iTmp = Whr_GetRainStage(iTime, sti(WeatherParams.Rain.Duration));
			if (iTime > sti(WeatherParams.Rain.Duration))
			{
				bRain = false;
			}
			Log_TestInfo("Is Raining: " + iTime + " minutes, stage: " + iTmp);
			//в зависимости от заданной стадия моделируем дождь.
			switch (iTmp)
			{
				case 0:
					//ноль
					WeatherParams.Rain.Sound = true;
					if (!bCurWeatherStorm)
					{
						Weathers[iCurWeatherNum].Lightning.Enable = false;
						WhrStopLightning();
					}
					break;
				case 1:

					if (!bCurWeatherStorm && sti(WeatherParams.Rain.IsLightingActive))
					{
						Weathers[iCurWeatherNum].Lightning.Enable = true;
						WhrStartLightning();
					}
					break;
				case 2:

					if (!bCurWeatherStorm)
					{
						Weathers[iCurWeatherNum].Lightning.Enable = false;
						WhrStopLightning();
					}
					break;
				case 3:
					//заканчиваем...
					if (!bCurWeatherStorm)
					{
						Weathers[iCurWeatherNum].Lightning.Enable = false;
						WhrStopLightning();
					}
					if (CheckAttribute(&WeatherParams, "Rain.Sound") && sti(WeatherParams.Rain.Sound))
					{
						WeatherParams.Rain = false;
						WeatherParams.Rain.Sound = false;
						Whr_SetRainSound(false, sti(Weathers[iCurWeatherNum].Night));
						Weathers[iCurWeatherNum].Lightning.Enable = false;
						if (nNewHour > 5 && nNewHour < 20)
						{
							Weathers[iCurWeatherNum].Rainbow.Enable = true;
						}

						DeleteAttribute(&WeatherParams, "Rain.StartTime");
					}
					WeatherParams.Rain.ThisDay = false;
					DeleteAttribute(&WeatherParams,"Rain.IsLightingActive");
					break;
			}

			Whr_SetRainStageDrops(iTmp, false);
		}
		//начало дождя... если генератор не запущен...
		else
		{
			//начинаем...
			if (CheckAttribute(&WeatherParams, "Rain.ThisDay") && sti(WeatherParams.Rain.ThisDay) && sti(WeatherParams.Rain.StartTime) <= nOldHour)
			{
				//интенсивность.
				Weathers[iBlendWeatherNum].Rain.NumDrops = 2000 + rand(3000);
				Weathers[iBlendWeatherNum].Rain.DropLength = 2 + rand(10);
				Weathers[iBlendWeatherNum].Rain.Color = argb(0,30,30,30);
				//сохраняем время начала дождя.
				SaveCurrentQuestDateParam("Rain.Duration");
				//флаг "дождь"
				WeatherParams.Rain = true;
				if (!bCurWeatherStorm)
				{
					Weathers[iCurWeatherNum].Lightning.Enable = false;
					WhrStopLightning();
				}
				//звук
				Whr_SetRainSound(true, sti(Weathers[iCurWeatherNum].Night));
				Log_TestInfo("Rain Strated. Duration: " + WeatherParams.Rain.Duration + " minutes");
			}
		}
	}
	else if (bRain && !bCurWeatherStorm)
	{
		Weathers[iCurWeatherNum].Lightning.Enable = false;
		WhrStopLightning();
	}

	if( nNewHour != nOldHour )
	{
		Whr_UpdateWeatherHour();
	}

	Whr_SelectFogBlocks();

	Whr_UpdateSeaWater();

	// update weather: sun lighting
	FillWeatherData(iCurWeatherNum, iBlendWeatherNum);
	Whr_DimSunInside();
	Weather.isDone = "";

	if (bSeaActive)
	{
		Island.FogDensity = Weather.Fog.IslandDensity;
		Sea.Fog.SeaDensity = stf(Weather.Fog.SeaDensity);
		Sea.Fog.Color = stf(Weather.Fog.Color);
		SendMessage(&IslandReflModel, "lllf", MSG_MODEL_SET_FOG, 1, 1, stf(Weather.Fog.IslandDensity));
	}

	Whr_UpdateLocationSeaFog();

	//update rain: rain drops, rain colors, rain size, rainbow

	if (bIsRainEnable)
	{
		FillRainData(iCurWeatherNum, iBlendWeatherNum);
		Rain.isDone = "";
	}

	// update sun glow: sun\moon, flares
	WhrFillSunGlowData(iCurWeatherNum, iBlendWeatherNum);
	SunGlow.isDone = true;

	// update sky: fog
	Sky.TimeUpdate = fTime;
}

#event_handler("eChangeDayNight", "eChangeDayNight");

void Whr_UpdateWeatherHour()
{
	bool bOldIsDay = Whr_IsDay();
	bool bOldIsLight = Whr_IsLight();
	bool bLightChange = false;
	int i;

	bWeatherIsLight = Whr_GetLong(&Weathers[iCurWeatherNum],"Lights");
	bWeatherIsNight = Whr_GetLong(&Weathers[iCurWeatherNum],"Night");

	FillAstronomyFadeValue();
	Astronomy.TimeUpdate = 1;

	if( Whr_IsLight() != bOldIsLight ) { bLightChange = true; }
	if( Whr_IsDay() != bOldIsDay )     { bLightChange = true; }
	if( bLightChange )
	{ // меняем источники освещения
		Whr_ChangeDayNight();
		Event("eChangeDayNight");
 	}
 	if (bSeaActive && !bAbordageStarted)
	{
        Whr_WindChange(); // диамическая смена ветра boal

		Whr_SeaRetarget();

		if (bLightChange && isEntity(&Sea)) { Whr_ShipLightChange(); }
 	}
}

void Whr_ShipLightChange()
{
	aref aCurWeather = GetCurrentWeather();
	Sea.Lights = aCurWeather.Lights;

	int j, iCharIdx;
	ref rChar;
	for (j = 0; j < iNumShips; j++)
	{
		iCharIdx = Ships[j];
		if (iCharIdx < 0 || iCharIdx >= TOTAL_CHARACTERS) { continue; }
		rChar = GetCharacter(iCharIdx);
		Ship_SetLightsAndFlares(rChar);
		SendMessage(rChar, "ll", MSG_SHIP_LIGHTSRESET, sti(Sea.Lights));
	}
}

void Whr_ChangeDayNight()
{
	int loadLocationIndex = FindLoadedLocation();
	if( loadLocationIndex >= 0 )
	{
		LocationSetLights(&Locations[loadLocationIndex]);
		Whr_LocationShipLightChange();
		if( Whr_IsDay() )
		{
			SendMessage( &Locations[loadLocationIndex], "ls", MSG_LOCATION_EX_MSG, "DelFlys" );
		}
	}
}

void Whr_LocationShipLightChange()
{
	if (!isEntity(&Sea)) { return; }

	aref aCurWeather = GetCurrentWeather();
	Sea.Lights = aCurWeather.Lights;

	int n;
	ref rChar;
	for (n = 0; n < locNumShips; n++)
	{
		if (!isEntity(&locShips[n])) { continue; }
		rChar = GetCharacter(iShips[n]);
		Ship_SetLightsAndFlares(rChar);
		SendMessage(&locShips[n], "ll", MSG_SHIP_LIGHTSRESET, sti(Sea.Lights));
	}
}

void FillWeatherData(int nw1, int nw2)
{
	if( nw1<0 || nw1>=MAX_WEATHERS ) {return;}

	string sCurFog = Whr_GetCurrentFog();
	string sBlnFog = Whr_GetBlendedFog();
	if( nw2<0)
	{
		Weather.Fog.Enable = Whr_GetLong(&Weathers[nw1], sCurFog + ".Enable");
		Weather.Fog.Density = Whr_GetFloat(&Weathers[nw1], sCurFog + ".Density");
		Weather.Fog.Color = Whr_GetColor(&Weathers[nw1], sCurFog + ".Color");
		Weather.Fog.IslandDensity = Whr_GetFloat(&Weathers[nw1], sCurFog + ".IslandDensity");
		Weather.Fog.SeaDensity = Whr_GetFloat(&Weathers[nw1], sCurFog + ".SeaDensity");

		Weather.Sun.Color = Whr_GetColor(&Weathers[nw1],"Sun.Color");
		Weather.Sun.Ambient = Whr_GetColor(&Weathers[nw1],"Sun.Ambient");
	}
	else
	{

		Whr_CalcHourBlend();
		float fBlend = fHourBlend;
		Weather.Fog.Enable = Whr_GetLong(&Weathers[nw1], sCurFog + ".Enable");
		Weather.Fog.Density = Whr_BlendFloat( fBlend, Whr_GetFloat(&Weathers[nw1], sCurFog + ".Density"), Whr_GetFloat(&Weathers[nw2], sBlnFog + ".Density") );
		Weather.Fog.Color = Whr_BlendColor( fBlend, Whr_GetColor(&Weathers[nw1], sCurFog + ".Color"), Whr_GetColor(&Weathers[nw2], sBlnFog + ".Color") );
		Weather.Fog.IslandDensity = Whr_BlendFloat( fBlend, Whr_GetFloat(&Weathers[nw1], sCurFog + ".IslandDensity"), Whr_GetFloat(&Weathers[nw2], sBlnFog + ".IslandDensity") );
		Weather.Fog.SeaDensity = Whr_BlendFloat( fBlend, Whr_GetFloat(&Weathers[nw1], sCurFog + ".SeaDensity"), Whr_GetFloat(&Weathers[nw2], sBlnFog + ".SeaDensity") );

		Weather.Sun.Color = Whr_BlendColor( fBlend, Whr_GetColor(&Weathers[nw1],"Sun.Color"), Whr_GetColor(&Weathers[nw2],"Sun.Color") );
		Weather.Sun.Ambient = Whr_BlendColor( fBlend, Whr_GetColor(&Weathers[nw1],"Sun.Ambient"), Whr_GetColor(&Weathers[nw2],"Sun.Ambient") );
	}
	Whr_ApplyRainFogRuntime(nw1, nw2);
	fFogDensity = stf(Weather.Fog.Density);
}

int FindWeatherByHour(int nHour)
{
	for (int n=0; n<MAX_WEATHERS; n++)
	{
		if (!CheckAttribute(&Weathers[n], "hour")) {continue;}
		if (CheckAttribute(&Weathers[n], "skip") && sti(Weathers[n].skip)==true) {continue;}
		if (CheckAttribute(&Weathers[n], "Storm") && sti(Weathers[n].Storm)==true) {continue;}

		if( sti(Weathers[n].hour.min) == nHour ) {return n;}
	}
	return -1;
}

int FindBlendWeather(int iWeatherNum)
{

	if( iWeatherNum<0 || iWeatherNum>=MAX_WEATHERS ) {return -1;}

	if( !CheckAttribute(&Weathers[iWeatherNum], "hour") ) {return -1;}
	if( CheckAttribute(&Weathers[iWeatherNum],"Storm") && sti(Weathers[iWeatherNum].Storm)==true ) {return -1;}
	if( CheckAttribute(&Weathers[iWeatherNum],"skip") && sti(Weathers[iWeatherNum].skip)==true ) {return -1;}

	int nBlendWeatherHour = sti(Weathers[iWeatherNum].hour.min) + 1;
	if( nBlendWeatherHour>=24 ) {nBlendWeatherHour = 0;}

	return FindWeatherByHour(nBlendWeatherHour);
}

void CreateWeather(int iExecuteLayer, int iRealizeLayer)
{
	CreateWeatherEnvironment();
	MoveWeatherToLayers(iExecuteLayer, iRealizeLayer);
}

void DeleteWeather()
{
	DeleteWeatherEnvironment();
}

void MoveWeatherToLayers(int iExecuteLayer, int iRealizeLayer)
{
	LayerDelObject(EXECUTE,&Weather);
	LayerDelObject(REALIZE,&Weather);
	LayerDelObject(SEA_EXECUTE,&Weather);
	LayerDelObject(SEA_REALIZE,&Weather);

	iNewExecuteLayer = iExecuteLayer;
	iNewRealizeLayer = iRealizeLayer;

	LayerAddObject(iExecuteLayer,&Weather,1000000);
	LayerAddObject(iRealizeLayer,&Weather,65530);

	MoveRainToLayers(iExecuteLayer, iRealizeLayer);
	MoveLightningToLayers(iExecuteLayer, iRealizeLayer);
	MoveSkyToLayers(iExecuteLayer, iRealizeLayer);
	MoveSunGlowToLayers(iExecuteLayer, iRealizeLayer);
	MoveAstronomyToLayers(iExecuteLayer, iRealizeLayer);
}

float Whr_GetFogDensity() { return fFogDensity; }

float Whr_GetWindAngle()
{
	if (!CheckAttribute(&Weather,"Wind.Angle")) { return 0.0; }
	return stf(Weather.Wind.Angle);
}

float Whr_GetWindSpeed()
{
	if (!CheckAttribute(&Weather,"Wind.Speed")) { return 3.0; }
	return stf(Weather.Wind.Speed);
}

ref Whr_GetShadowDensity()
{
	iShadowDensity[0] = argb(255,96,96,96);
	iShadowDensity[1] = argb(255,96,96,64);

	if (bWeatherLoaded && iCurWeatherNum >= 0)
	{
		aref aCurWeather = GetCurrentWeather();
		iShadowDensity[0] = Whr_GetColor(aCurWeather,"Shadow.Density.Head");
		iShadowDensity[1] = Whr_GetColor(aCurWeather,"Shadow.Density.Foot");
	}

	return &iShadowDensity;
}

string	Whr_GetCurrentFog() { return sCurrentFog; }
string	Whr_GetBlendedFog() { return sBlendedFog; }
string	Whr_GetInsideBack() { return sInsideBack; }

string GetLightingPath()
{
	if (CheckAttribute(&WeatherParams, "Rain") && sti(WeatherParams.Rain))
	{
		return Whr_GetRainyLightningPath();
	}
	return sLightingPath;
}

string GetIslandLightingPath()
{
	return "";
}

string	GetLmLightingPath()
{
	if (CheckAttribute(&WeatherParams, "Rain") && sti(WeatherParams.Rain))
	{
		return "storm";
	}
	return sLmLightingPath;
}

bool	Whr_IsDay() { return !bWeatherIsNight; }
bool	Whr_IsNight() { return bWeatherIsNight; }
bool	Whr_IsLight() { return bWeatherIsLight; }
bool	Whr_IsRain() { return bWeatherIsRain; }
bool	Whr_IsStorm() { return bWeatherIsStorm; }

int FindWeather(string sWeatherID)
{
	for (int i=0;i<MAX_WEATHERS;i++)
	{
		if (!CheckAttribute(&Weathers[i], "id"))
		{
			continue;
		}
		if(Weathers[i].id == sWeatherID)
		{
			return i;
		}
	}
	return -1;
}

void Whr_SetRainSound(bool _set, bool _isNight)
{

	string scheme = "land";
	//ищем локацию ГГ, три схемы: land, sea, town

	if (!bSeaActive || bAbordageStarted)
	{
		if (IsEntity(loadedLocation) && CheckAttribute(loadedLocation, "type"))
		{
			switch (loadedLocation.type)
			{
			case "town":
				scheme = "town";
				break;
			case "land":
				scheme = "land";
				break;
			case "deck":
				scheme = "deck";
				break;
			}
		}
	}
	else
	{
		scheme = "sea";
	}

	//проверка - день/ночь
	if (_isNight)
    {
		scheme = scheme+"_night";
    }
	else
	{
		scheme = scheme+"_day";
	}

	//если устанавливаем звук дождя
	if (_set)
	{
		scheme = scheme + "_rain";
	}

	trace(": Set Rain Sound... " + _set);
	SetSoundScheme(scheme);
}

string Whr_GetRainyLightningPath()
{

	return "storm1";
}

void Whr_WindChange()
{
	aref aCurWeather = GetCurrentWeather();

    if(CheckAttribute(pchar, "wind.angle"))
    {
    	Weather.Wind.Angle = stf(pchar.wind.angle) +  frand((PI/4.0)) - (PI / 8.0);
    	if (stf(Weather.Wind.Angle) < 0)  Weather.Wind.Angle = PIm2 + stf(Weather.Wind.Angle);
    }
    else
    {
		Weather.Wind.Angle = frand(PIm2);
	}
	pchar.wind.angle = Weather.Wind.Angle;

	Weather.Wind.Speed = Whr_GetFloat(aCurWeather,"Wind.Speed");
	pchar.wind.speed = Weather.Wind.Speed;

	pchar.quest.EraseWind.win_condition.l1 = "ExitFromSea";
	pchar.quest.EraseWind.win_condition = "EraseWind";

	fWeatherAngle = stf(Weather.Wind.Angle);
	fWeatherSpeed = stf(Weather.Wind.Speed);
}

bool Whr_isRainEnable()
{
	bool bRain  = true;
	//ГГ в локации
    if (CheckAttribute(GetMainCharacter(), "HeroParam") && IsEntity(loadedLocation)) // pchar проверяется для гл меню
	{
        if (CheckAttribute(loadedLocation, "environment.weather.rain") && !sti(loadedLocation.environment.weather.rain))
		{
			bRain = false;
		}
		//в других можно включать дождь
	}
	else
	{
		if (bSeaActive && bAbordageStarted ) // в море не срабатывает bLandInterfaceStart
		{
			if (IsEntity(loadedLocation) && CheckAttribute(loadedLocation, "environment.weather.rain") && !sti(loadedLocation.environment.weather.rain))
			{
				bRain = false;
			}
		}
	}
	return bRain;
}

