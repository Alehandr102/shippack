

int Whr_InitDay(int n)
{

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "11 Hour";
	Weathers[n].Hour.Min = 11;
	Weathers[n].Hour.Max = 11;

	Weathers[n].Lighting = "day11";
	Weathers[n].LightingLm = "day1";
	Weathers[n].InsideBack = "d";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\11\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00024;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,195,185,157 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 17.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "12 Hour";
	Weathers[n].Hour.Min = 12;
	Weathers[n].Hour.Max = 12;

	Weathers[n].Lighting = "day12";
	Weathers[n].LightingLm = "day1";
	Weathers[n].InsideBack = "d";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.SubTexX = 4;

	Weathers[n].Sky.Dir = "weather\skies\12\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00023;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,200,190,160 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 4.0;
	Weathers[n].Wind.Speed.Max = 18.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "13 Hour";
	Weathers[n].Hour.Min = 13;
	Weathers[n].Hour.Max = 13;

	Weathers[n].Lighting = "day13";
	Weathers[n].LightingLm = "day2";
	Weathers[n].InsideBack = "d";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\13\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00023;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,200,190,160 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 7.0;
	Weathers[n].Wind.Speed.Max = 18.5;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "14 Hour";
	Weathers[n].Hour.Min = 14;
	Weathers[n].Hour.Max = 14;

	Weathers[n].Lighting = "day14";
	Weathers[n].LightingLm = "day2";
	Weathers[n].InsideBack = "d";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\14\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00023;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,195,185,156 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 7.0;
	Weathers[n].Wind.Speed.Max = 17.5;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "15 Hour";
	Weathers[n].Hour.Min = 15;
	Weathers[n].Hour.Max = 15;

	Weathers[n].Lighting = "day15";
	Weathers[n].LightingLm = "day3";
	Weathers[n].InsideBack = "d";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\15\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00023;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,185,175,148 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 7.0;
	Weathers[n].Wind.Speed.Max = 18.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "16 Hour";
	Weathers[n].Hour.Min = 16;
	Weathers[n].Hour.Max = 16;

	Weathers[n].Lighting = "day16";
	Weathers[n].LightingLm = "day3";
	Weathers[n].InsideBack = "d";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\16\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00023;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,170,161,137 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 7.0;
	Weathers[n].Wind.Speed.Max = 15.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "17 Hour";
	Weathers[n].Hour.Min = 17;
	Weathers[n].Hour.Max = 17;

	Weathers[n].Lighting = "day17";
	Weathers[n].LightingLm = "day4";
	Weathers[n].InsideBack = "d";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Sky.Dir = "weather\skies\17\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.00023;
	Weathers[n].SeaFog.IslandDensity = 0.00025;
	Weathers[n].SeaFog.Color         = argb(0,132,155,170);
	Weathers[n].LandFog.Color        = argb(0,144,171,196);

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,160,153,129 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 18.0;

	n++;

	Whr_PresetBase(n);
	Whr_PresetHour(n);
	Weathers[n].id = "18 Hour";
	Weathers[n].Hour.Min = 18;
	Weathers[n].Hour.Max = 18;

	Weathers[n].Lighting = "day18";
	Weathers[n].LightingLm = "day4";
	Weathers[n].InsideBack = "d";

	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.SubTexX = 4;

	Weathers[n].Sky.Dir = "weather\skies\18\";

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Density       = 0.0004;
	Weathers[n].SeaFog.SeaDensity  = 0.0003;
	Weathers[n].SeaFog.IslandDensity = 0.00024;
	Weathers[n].SeaFog.Color         = argb(0,98,110,120);
	Weathers[n].LandFog.Color        = argb(  0,156,163,164 );

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );

	Weathers[n].Sun.Color = argb(  0,145,138,119 );
	Weathers[n].Sun.Ambient = argb(  0,128,128,128 );

	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Flare.Enable = true;

	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 18.5;

	n++;

	return n;
}
