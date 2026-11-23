

int Whr_InitEvening(int n)
{

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "19 Hour";
	Weathers[n].Hour.Min = 19;
	Weathers[n].Hour.Max = 19;

	Weathers[n].Lighting = "evening19";
	Weathers[n].LightingLm = "evening";
	Weathers[n].InsideBack = "e";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\19\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.001;
	Weathers[n].LandFog.Density      = 0.0005;
	Weathers[n].SeaFog.SeaDensity  = 0.0004;
	Weathers[n].SeaFog.IslandDensity = 0.0003;
	Weathers[n].LandFog.SeaDensity = 0.0020;
	Weathers[n].SeaFog.Color         = argb(0,46,54,61);
	Weathers[n].LandFog.Color        = argb(0,72,83,94);

	Weathers[n].Rain.DropsColor = argb( 60,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,242,190,142 );
	Weathers[n].Sun.Ambient = argb(  0, 82, 82, 82 );

	Weathers[n].Sun.Overflow.Size = 5500.0;

	Weathers[n].Sun.Flare.Enable = true;
	Weathers[n].Sun.Flares.f20 = "0.2,1200.0,9,737373";

	Weathers[n].Wind.Speed.Min = 7.0;
	Weathers[n].Wind.Speed.Max = 12.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "20 Hour";
	Weathers[n].Hour.Min = 20;
	Weathers[n].Hour.Max = 20;

	Weathers[n].Lighting = "Evening20";
	Weathers[n].LightingLm = "evening";
	Weathers[n].InsideBack = "e";

	Weathers[n].Lights = true;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\20\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0008;
	Weathers[n].SeaFog.SeaDensity  = 0.00043;
	Weathers[n].LandFog.SeaDensity = 0.0025;
	Weathers[n].SeaFog.IslandDensity = 0.00018;
	Weathers[n].SeaFog.Color         = argb(0,18,20,28);
	Weathers[n].LandFog.Color        = argb(0,48,55,66);

	Weathers[n].SpecialSeaFog.Enable = true;
	Weathers[n].SpecialSeaFog.Type = 1;
	Weathers[n].SpecialSeaFog.Height = 700;
	Weathers[n].SpecialSeaFog.Density = 0.0048;
	Weathers[n].SpecialSeaFog.IslandDensity = 0.0016;
	Weathers[n].SpecialSeaFog.SeaDensity = 0.00258;
	Weathers[n].SpecialSeaFog.Color = argb(0,18,22,30);

	Weathers[n].SpecialLandFog.Enable = true;
	Weathers[n].SpecialLandFog.Type = 1;
	Weathers[n].SpecialLandFog.Height = 700;
	Weathers[n].SpecialLandFog.Density = 0.011;
	Weathers[n].SpecialLandFog.IslandDensity = 0.0046;
	Weathers[n].SpecialLandFog.SeaDensity = 0.0162;
	Weathers[n].SpecialLandFog.Color = argb(0,47, 46, 53);

	Weathers[n].Rain.DropsColor = argb( 58,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 67, 70, 77 );
	Weathers[n].Sun.Ambient = argb(  0, 43, 41, 42 );

	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = false;
	Weathers[n].Sun.Flare.Scale = 0.0;

	Weathers[n].Wind.Speed.Min = 8.5;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "21 Hour";
	Weathers[n].Hour.Min = 21;
	Weathers[n].Hour.Max = 21;

	Weathers[n].Lighting = "evening21";
	Weathers[n].LightingLm = "evening";

	Weathers[n].InsideBack = "e";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\21\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0006;
	Weathers[n].SeaFog.SeaDensity  = 0.00051;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,14,18,24);
	Weathers[n].LandFog.Color        = argb(0,27,33,40);

	Weathers[n].SpecialSeaFog.Enable = true;
	Weathers[n].SpecialSeaFog.Type = 1;
	Weathers[n].SpecialSeaFog.Height = 1700;
	Weathers[n].SpecialSeaFog.Density = 0.0036;
	Weathers[n].SpecialSeaFog.IslandDensity = 0.0024;
	Weathers[n].SpecialSeaFog.SeaDensity = 0.00306;
	Weathers[n].SpecialSeaFog.Color = argb(0,14,18,24);

	Weathers[n].SpecialLandFog.Enable = true;
	Weathers[n].SpecialLandFog.Type = 1;
	Weathers[n].SpecialLandFog.Height = 1700;
	Weathers[n].SpecialLandFog.Density = 0.025;
	Weathers[n].SpecialLandFog.IslandDensity = 0.020;
	Weathers[n].SpecialLandFog.SeaDensity = 0.0225;
	Weathers[n].SpecialLandFog.Color = argb(0,26, 25, 30);

	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 14.0;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	Weathers[n].Rain.DropsNearRadius = 25.0;
	Weathers[n].Rain.DropsFarRadius = 75.0;
	Weathers[n].Rain.DropsLifeTime = 0.25;
	Weathers[n].Rain.DropsColor = argb( 56,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 27, 32, 37 );

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Flare.Enable = false;
	Weathers[n].Sun.Glow.Enable = false;

	Weathers[n].Wind.Speed.Min = 8.0;
	Weathers[n].Wind.Speed.Max = 13.5;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "22 Hour";
	Weathers[n].Hour.Min = 22;
	Weathers[n].Hour.Max = 22;

	Weathers[n].Lighting = "evening22";
	Weathers[n].LightingLm = "evening";

	Weathers[n].InsideBack = "e";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\22\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.0006;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,13,17,22);
	Weathers[n].LandFog.Color        = argb(0,13,17,22);

	Weathers[n].SpecialSeaFog.Enable = true;
	Weathers[n].SpecialSeaFog.Type = 1;
	Weathers[n].SpecialSeaFog.Height = 1700;
	Weathers[n].SpecialSeaFog.Density = 0.0024;
	Weathers[n].SpecialSeaFog.IslandDensity = 0.0016;
	Weathers[n].SpecialSeaFog.SeaDensity = 0.0036;
	Weathers[n].SpecialSeaFog.Color = argb(0,13,17,22);

	Weathers[n].SpecialLandFog.Enable = true;
	Weathers[n].SpecialLandFog.Type = 1;
	Weathers[n].SpecialLandFog.Height = 1700;
	Weathers[n].SpecialLandFog.Density = 0.025;
	Weathers[n].SpecialLandFog.IslandDensity = 0.008;
	Weathers[n].SpecialLandFog.SeaDensity = 0.0225;
	Weathers[n].SpecialLandFog.Color = argb(0,14, 14, 23);

	Weathers[n].Rain.DropsColor = argb( 52,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 25, 30, 35 );

	Weathers[n].Sun.Glow.Enable = false;

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Reflection.Enable = true;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Wind.Speed.Min = 6.0;
	Weathers[n].Wind.Speed.Max = 12.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "23 Hour";
	Weathers[n].Hour.Min = 23;
	Weathers[n].Hour.Max = 23;

	Weathers[n].Lighting = "evening23";
	Weathers[n].LightingLm = "storm";

	Weathers[n].InsideBack = "e";

	Weathers[n].Lights = true;
	Weathers[n].Night = true;

	Weathers[n].Sky.Dir = "weather\skies\23\";

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.0006;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.IslandDensity = 0.0002;
	Weathers[n].SeaFog.Color         = argb(0,13,17,22);
	Weathers[n].LandFog.Color        = argb(0,13,17,22);

	Weathers[n].SpecialSeaFog.Enable = true;
	Weathers[n].SpecialSeaFog.Type = 1;
	Weathers[n].SpecialSeaFog.Height = 700;
	Weathers[n].SpecialSeaFog.Density = 0.0024;
	Weathers[n].SpecialSeaFog.IslandDensity = 0.0012;
	Weathers[n].SpecialSeaFog.SeaDensity = 0.0036;
	Weathers[n].SpecialSeaFog.Color = argb(0,13,17,22);

	Weathers[n].SpecialLandFog.Enable = true;
	Weathers[n].SpecialLandFog.Type = 1;
	Weathers[n].SpecialLandFog.Height = 700;
	Weathers[n].SpecialLandFog.Density = 0.015;
	Weathers[n].SpecialLandFog.IslandDensity = 0.003;
	Weathers[n].SpecialLandFog.SeaDensity = 0.0135;
	Weathers[n].SpecialLandFog.Color = argb(0,13, 13, 23);

	Weathers[n].Rain.DropsColor = argb( 46,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 45, 55, 65 );
	Weathers[n].Sun.Ambient = argb(  0, 25, 30, 35 );

	Weathers[n].Sun.Moon = true;

	Weathers[n].Sun.Overflow.Size = 0.0;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Wind.Speed.Min = 6.0;
	Weathers[n].Wind.Speed.Max = 11.5;

	n++;

	return n;
}
