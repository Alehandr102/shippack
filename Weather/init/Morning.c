

int Whr_InitMorning(int n)
{

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "6 Hour";
	Weathers[n].Hour.Min = 6;
	Weathers[n].Hour.Max = 6;

	Weathers[n].Lighting = "morning6";
	Weathers[n].LightingLm = "morning";

	Weathers[n].InsideBack = "m";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\06\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = true;

	Weathers[n].SeaFog.Density       = 0.0005;
	Weathers[n].LandFog.Density      = 0.002;
	Weathers[n].SeaFog.SeaDensity  = 0.0004;
	Weathers[n].SeaFog.IslandDensity = 0.00028;
	Weathers[n].LandFog.SeaDensity = 0.0030;
	Weathers[n].SeaFog.Color         = argb(0,52,64,74);
	Weathers[n].LandFog.Color        = argb(0,72,92,104);

	Weathers[n].SpecialSeaFog.Enable = true;
	Weathers[n].SpecialSeaFog.Type = 0;
	Weathers[n].SpecialSeaFog.Height = 1400;
	Weathers[n].SpecialSeaFog.Density = 0.003;
	Weathers[n].SpecialSeaFog.IslandDensity = 0.00168;
	Weathers[n].SpecialSeaFog.SeaDensity = 0.0024;
	Weathers[n].SpecialSeaFog.Color = argb(0,39,48,57);

	Weathers[n].SpecialLandFog.Enable = true;
	Weathers[n].SpecialLandFog.Type = 0;
	Weathers[n].SpecialLandFog.Height = 1400;
	Weathers[n].SpecialLandFog.Density = 0.016;
	Weathers[n].SpecialLandFog.IslandDensity = 0.015;
	Weathers[n].SpecialLandFog.SeaDensity = 0.0144;
	Weathers[n].SpecialLandFog.Color = argb(0,57,73,82);

	Weathers[n].Rain.DropsColor = argb( 50,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,189,155, 115);
	Weathers[n].Sun.Ambient = argb(  0, 84, 84, 84 );

	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);

	Weathers[n].Sun.Overflow.Size = 5500.0;

	Weathers[n].Sun.Flare.Enable = true;
	Weathers[n].Sun.Flares.f20 = "0.2,1200.0,9,787878";

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 9.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "7 Hour";
	Weathers[n].Hour.Min = 7;
	Weathers[n].Hour.Max = 7;

	Weathers[n].Lighting = "morning7";
	Weathers[n].LightingLm = "morning";

	Weathers[n].InsideBack = "m";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\07\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00036;
	Weathers[n].LandFog.SeaDensity = 0.0025;
	Weathers[n].SeaFog.IslandDensity = 0.00027;
	Weathers[n].SeaFog.Color         = argb(0,92,112,126);
	Weathers[n].LandFog.Color        = argb(0,115,140,156);

	Weathers[n].SpecialSeaFog.Enable = true;
	Weathers[n].SpecialSeaFog.Type = 0;
	Weathers[n].SpecialSeaFog.Height = 1400;
	Weathers[n].SpecialSeaFog.Density = 0.0024;
	Weathers[n].SpecialSeaFog.IslandDensity = 0.00162;
	Weathers[n].SpecialSeaFog.SeaDensity = 0.00216;
	Weathers[n].SpecialSeaFog.Color = argb(0,73,89,100);

	Weathers[n].SpecialLandFog.Enable = true;
	Weathers[n].SpecialLandFog.Type = 0;
	Weathers[n].SpecialLandFog.Height = 1400;
	Weathers[n].SpecialLandFog.Density = 0.013;
	Weathers[n].SpecialLandFog.IslandDensity = 0.009;
	Weathers[n].SpecialLandFog.SeaDensity = 0.0117;
	Weathers[n].SpecialLandFog.Color = argb(0,116,147,166);

	Weathers[n].Rain.DropsColor = argb( 50,255,255,255 );

	Weathers[n].Sun.Color = argb(  0, 98, 94, 84 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 6.0;
	Weathers[n].Wind.Speed.Max = 11.5;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "8 Hour";
	Weathers[n].Hour.Min = 8;
	Weathers[n].Hour.Max = 8;

	Weathers[n].Lighting = "morning8";
	Weathers[n].LightingLm = "morning";

	Weathers[n].InsideBack = "m";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.SubTexX = 4;

	Weathers[n].Sky.Dir = "weather\skies\08\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00032;
	Weathers[n].LandFog.SeaDensity = 0.0020;
	Weathers[n].SeaFog.IslandDensity = 0.00026;
	Weathers[n].SeaFog.Color         = argb(0,112,133,148);
	Weathers[n].LandFog.Color        = argb(0,150,174,196);

	Weathers[n].SpecialSeaFog.Enable = true;
	Weathers[n].SpecialSeaFog.Type = 0;
	Weathers[n].SpecialSeaFog.Height = 700;
	Weathers[n].SpecialSeaFog.Density = 0.0024;
	Weathers[n].SpecialSeaFog.IslandDensity = 0.00156;
	Weathers[n].SpecialSeaFog.SeaDensity = 0.00192;
	Weathers[n].SpecialSeaFog.Color = argb(0,99,117,130);

	Weathers[n].SpecialLandFog.Enable = true;
	Weathers[n].SpecialLandFog.Type = 0;
	Weathers[n].SpecialLandFog.Height = 700;
	Weathers[n].SpecialLandFog.Density = 0.010;
	Weathers[n].SpecialLandFog.IslandDensity = 0.006;
	Weathers[n].SpecialLandFog.SeaDensity = 0.009;
	Weathers[n].SpecialLandFog.Color = argb(0,152,176,198);

	Weathers[n].Rain.DropsColor = argb( 55,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,115,110, 95 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 5.5;
	Weathers[n].Wind.Speed.Max = 13.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "9 Hour";
	Weathers[n].Hour.Min = 9;
	Weathers[n].Hour.Max = 9;

	Weathers[n].Lighting = "morning9";
	Weathers[n].LightingLm = "morning";

	Weathers[n].InsideBack = "m";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.SubTexX = 4;

	Weathers[n].Sky.Dir = "weather\skies\09\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00029;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 59,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,155,148,125 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 6.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "10 Hour";
	Weathers[n].Hour.Min = 10;
	Weathers[n].Hour.Max = 10;

	Weathers[n].Lighting = "morning10";
	Weathers[n].LightingLm = "morning";

	Weathers[n].InsideBack = "m";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\10\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00026;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 61,255,255,255 );

	Weathers[n].Sun.Color =  argb(  0,180,170,145 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 7.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

	return n;
}
