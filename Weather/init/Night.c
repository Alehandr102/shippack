

int Whr_InitNight(int n)
{

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "0 Hour";
	Weathers[n].Hour.Min = 0;
	Weathers[n].Hour.Max = 0;

	Weathers[n].Lighting = "night0";
	Weathers[n].LightingLm = "storm";

	Weathers[n].InsideBack = "n";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\24\";

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.0006;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,11,15,20);
	Weathers[n].LandFog.Color        = argb(0,11,15,20);

	Weathers[n].Rain.DropsColor = argb( 43,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 25, 30, 35 );

	Weathers[n].Sun.Moon = true;

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "1 Hour";
	Weathers[n].Hour.Min = 1;
	Weathers[n].Hour.Max = 1;

	Weathers[n].Lighting = "night1";
	Weathers[n].LightingLm = "storm";

	Weathers[n].InsideBack = "n";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\01\";

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.0006;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,11,15,20);
	Weathers[n].LandFog.Color        = argb(0,11,15,20);

	Weathers[n].Rain.DropsColor = argb( 43,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 25, 30, 35 );

	Weathers[n].Sun.Moon = true;

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "2 Hour";
	Weathers[n].Hour.Min = 2;
	Weathers[n].Hour.Max = 2;

	Weathers[n].Lighting = "night2";
	Weathers[n].LightingLm = "storm";

	Weathers[n].InsideBack = "n";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\02\";

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.0006;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,11,15,20);
	Weathers[n].LandFog.Color        = argb(0,11,15,20);

	Weathers[n].Rain.DropsColor = argb( 43,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 25, 30, 35 );

	Weathers[n].Sun.Moon = true;

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "3 Hour";
	Weathers[n].Hour.Min = 3;
	Weathers[n].Hour.Max = 3;

	Weathers[n].Lighting = "night3";
	Weathers[n].LightingLm = "storm";

	Weathers[n].InsideBack = "n";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\03\";

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.0006;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,9,12,16);
	Weathers[n].LandFog.Color        = argb(0,9,12,16);

	Weathers[n].Rain.DropsColor = argb( 43,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 25, 30, 35 );

	Weathers[n].Sun.Moon = true;

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "4 Hour";
	Weathers[n].Hour.Min = 4;
	Weathers[n].Hour.Max = 4;

	Weathers[n].Lighting = "night4";
	Weathers[n].LightingLm = "storm";

	Weathers[n].InsideBack = "n";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\04\";

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.0006;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,8,11,15);
	Weathers[n].LandFog.Color        = argb(0,8,11,15);

	Weathers[n].Rain.DropsColor = argb( 43,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 25, 30, 35 );

	Weathers[n].Sun.Moon = true;
	Weathers[n].Sun.Glow.Enable = false;

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "5 Hour";
	Weathers[n].Hour.Min = 5;
	Weathers[n].Hour.Max = 5;

	Weathers[n].Lighting = "night5";
	Weathers[n].LightingLm = "storm";

	Weathers[n].InsideBack = "n";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\05\";

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0005;
	Weathers[n].SeaFog.SeaDensity  = 0.0005;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,21,27,35);
	Weathers[n].LandFog.Color        = argb(  0, 29, 46, 54 );

	Weathers[n].SpecialSeaFog.Enable = true;
	Weathers[n].SpecialSeaFog.Type = 0;
	Weathers[n].SpecialSeaFog.Height = 1200;
	Weathers[n].SpecialSeaFog.Density = 0.0025;
	Weathers[n].SpecialSeaFog.IslandDensity = 0.0024;
	Weathers[n].SpecialSeaFog.SeaDensity = 0.0055;
	Weathers[n].SpecialSeaFog.Color = argb(0,21,27,35);

	Weathers[n].SpecialLandFog.Enable = true;
	Weathers[n].SpecialLandFog.Type = 0;
	Weathers[n].SpecialLandFog.Height = 700;
	Weathers[n].SpecialLandFog.Density = 0.015;
	Weathers[n].SpecialLandFog.IslandDensity = 0.006;
	Weathers[n].SpecialLandFog.SeaDensity = 0.0135;
	Weathers[n].SpecialLandFog.Color = argb(0,10, 21, 37);

	Weathers[n].Rain.DropsColor = argb( 43,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 25, 30, 35 );

	Weathers[n].Sun.Moon = false;

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	return n;
}
