#include "Weather\Init\Common.c"
#include "Weather\Init\Day.c"
#include "Weather\Init\Evening.c"
#include "Weather\Init\Morning.c"
#include "Weather\Init\Night.c"
#include "Weather\Init\DayStorm.c"
#include "Weather\Init\Special.c"

void Whr_ValidateWeatherInit(int n)
{
	int iErrors = 0;
	int i, j, k;
	int iCount = n;
	string sH, sKind;

	if (n != MAX_WEATHERS)
	{
		Trace("ПОГОДА: InitWeather создал " + n + " пресетов, MAX_WEATHERS = " + MAX_WEATHERS);
		iErrors++;
	}
	if (iCount > MAX_WEATHERS) { iCount = MAX_WEATHERS; }

	for (i = 0; i < iCount; i++)
	{
		if (!CheckAttribute(&Weathers[i], "id"))
		{
			Trace("ПОГОДА: у пресета " + i + " нет id");
			iErrors++;
		}
		if (!CheckAttribute(&Weathers[i], "Hour.Min") || !CheckAttribute(&Weathers[i], "Hour.Max"))
		{
			Trace("ПОГОДА: у пресета " + i + " нет диапазона Hour");
			iErrors++;
		}
		if (!CheckAttribute(&Weathers[i], "Lightning.ScaleX") || !CheckAttribute(&Weathers[i], "Lightning.ScaleY"))
		{
			Trace("ПОГОДА: у пресета " + i + " нет масштаба молнии");
			iErrors++;
		}
		if (!CheckAttribute(&Weathers[i], "Shadow.Density.Head") || !CheckAttribute(&Weathers[i], "Shadow.Density.Foot"))
		{
			Trace("ПОГОДА: у пресета " + i + " нет плотности теней");
			iErrors++;
		}
		for (j = i + 1; j < iCount; j++)
		{
			if (CheckAttribute(&Weathers[i], "id") && CheckAttribute(&Weathers[j], "id"))
			{
				if (Weathers[i].id == Weathers[j].id)
				{
					Trace("ПОГОДА: повторяется id пресета " + Weathers[i].id);
					iErrors++;
				}
			}
		}
	}

	for (i = 0; i < 24; i++)
	{
		sH = "h" + i;
		if (i >= iCount)
		{
			Trace("ПОГОДА: нарушена последовательность часового пресета " + i);
			iErrors++;
		}
		else
		{
			if (!CheckAttribute(&Weathers[i], "Hour.Min") || sti(Weathers[i].Hour.Min) != i)
			{
				Trace("ПОГОДА: нарушена последовательность часового пресета " + i);
				iErrors++;
			}
		}
		for (k = 0; k < 4; k++)
		{
			if (k == 0) { sKind = "sea"; }
			if (k == 1) { sKind = "land"; }
			if (k == 2) { sKind = "seashore"; }
			if (k == 3) { sKind = "cave"; }
			if (!CheckAttribute(&WhrCommonParams, "Water." + sKind + "." + sH + ".Color") ||
				!CheckAttribute(&WhrCommonParams, "Water." + sKind + "." + sH + ".Reflection") ||
				!CheckAttribute(&WhrCommonParams, "Water." + sKind + "." + sH + ".Transparency") ||
				!CheckAttribute(&WhrCommonParams, "Water." + sKind + "." + sH + ".Frenel") ||
				!CheckAttribute(&WhrCommonParams, "Water." + sKind + "." + sH + ".Attenuation"))
			{
				Trace("ПОГОДА: неполный материал воды " + sKind + " для часа " + i);
				iErrors++;
			}
		}
		if (!CheckAttribute(&WhrCommonParams, "Waves.Hour." + sH + ".FoamK"))
		{
			Trace("ПОГОДА: нет пены открытого моря для часа " + i);
			iErrors++;
		}
	}

	if (iErrors > 0) { Trace("ПОГОДА: проверка инициализации, ошибок: " + iErrors); }
}

int InitWeather()
{
	int n = 0;
	n = Whr_InitNight(n);
	n = Whr_InitMorning(n);
	n = Whr_InitDay(n);
	n = Whr_InitEvening(n);
	n = Whr_InitDayStorm(n);
	n = Whr_InitSpecial(n);

	Whr_InitCommon();

	Trace("Init weathers complete. " + n + " weathers found.");

	for (int i=0; i<n; i++)
	{
		// Stock location loaders still read this one legacy field underwater.
		// Runtime fog selection remains in Weather.Fog and is not copied back here.
		Weathers[i].Fog.Color = Weathers[i].LandFog.Color;

		if (!CheckAttribute(&Weathers[i], "Stars.Enable")) { Weathers[i].Stars.Enable = false; }
		if (!CheckAttribute(&Weathers[i], "Planets.Enable")) { Weathers[i].Planets.Enable = false; }
		if (!CheckAttribute(&Weathers[i], "Sun.Reflection.Enable")) { Weathers[i].Sun.Reflection.Enable = false; }
		if (!CheckAttribute(&Weathers[i], "Sounds.Sea.Postfix")) { Weathers[i].Sounds.Sea.Postfix = ""; }
		if (!CheckAttribute(&Weathers[i], "Sun.Moon")) { Weathers[i].Sun.Moon = false; }

		if (!CheckAttribute(&Weathers[i], "Rain.DropsNearNum")) { Weathers[i].Rain.DropsNearNum = 0; }
		if (!CheckAttribute(&Weathers[i], "Rain.DropsFarNum")) { Weathers[i].Rain.DropsFarNum = 0; }
		if (!CheckAttribute(&Weathers[i], "Rain.DropsNearRadius")) { Weathers[i].Rain.DropsNearRadius = 12.0; }
		if (!CheckAttribute(&Weathers[i], "Rain.DropsFarRadius")) { Weathers[i].Rain.DropsFarRadius = 55.0; }
		if (!CheckAttribute(&Weathers[i], "Rain.DropsLifeTime")) { Weathers[i].Rain.DropsLifeTime = 0.25; }
		if (!CheckAttribute(&Weathers[i], "Rain.DropsSize")) { Weathers[i].Rain.DropsSize = 0.07; }
		if (!CheckAttribute(&Weathers[i], "Rain.DropsTexture")) { Weathers[i].Rain.DropsTexture = "weather\rain_drops.tga.tx"; }
		if (!CheckAttribute(&Weathers[i], "Rain.DropsColor")) { Weathers[i].Rain.DropsColor = argb(63, 255, 255, 255); }
	}

	Whr_ValidateWeatherInit(n);

	return n;
}
