
int Whr_InitDayStorm(int n)
{

// Stormy1 - утренний шторм

	Whr_PresetBase(n);
	Weathers[n].id = "Storm01";
	Weathers[n].Hour.Min = 6.00001;
	Weathers[n].Hour.Max = 10;
	Weathers[n].Lighting = "storm1";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].Tornado = true;
	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.Enable = true;

	Weathers[n].Sky.Dir = "weather\skies\Storm01\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 2000;
	Weathers[n].LandFog.Height       = 2000;
	Weathers[n].SeaFog.Density       = 0.0045000;
	Weathers[n].LandFog.Density      = 0.0047;
	Weathers[n].SeaFog.SeaDensity  = 0.0045000;
	Weathers[n].SeaFog.IslandDensity = 0.0005000;
	Weathers[n].LandFog.SeaDensity = 0.0108;
	Weathers[n].LandFog.IslandDensity = 0.0012;
	Weathers[n].SeaFog.Color         = argb(0, 46, 54, 59);
	Weathers[n].LandFog.Color        = argb(  0, 30, 38, 46 );

	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(  0, 23, 23, 23 );

	Weathers[n].Rain.DropsColor = argb( 55,255,255,255 );
	Weathers[n].Rain.DropsSize = 0.1;

	Weathers[n].Rainbow.Enable = true;

	Weathers[n].Sun.Color = argb(0,72,74,78);
	Weathers[n].Sun.Ambient = argb(0,72,72,72);
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;
	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sun.Reflection.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 77, 77, 74);
	Weathers[n].Sea2.Reflection   = 0.5500000;
	Weathers[n].Sea2.Transparency = 0.1000000;
	Weathers[n].Sea2.Frenel       = 0.0300000;
	Weathers[n].Sea2.Attenuation  = 0.2000000;

	Weathers[n].Wind.Speed.Min = 18.0;
	Weathers[n].Wind.Speed.Max = 21.0;

	n++;

// Overcast2 - дневной шторм

	Whr_PresetBase(n);
	Weathers[n].id = "Storm02";
	Weathers[n].Hour.Min = 10.00001;
	Weathers[n].Hour.Max = 18;
	Weathers[n].Lighting = "storm2";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].Tornado = true;
	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.Enable = true;

	Weathers[n].Sky.Dir = "weather\skies\Storm02\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 2000;
	Weathers[n].LandFog.Height       = 2000;
	Weathers[n].SeaFog.Density       = 0.0045000;
	Weathers[n].LandFog.Density      = 0.0195000;
	Weathers[n].SeaFog.SeaDensity  = 0.0040000;
	Weathers[n].SeaFog.IslandDensity = 0.0005000;
	Weathers[n].LandFog.SeaDensity = 0.0145000;
	Weathers[n].LandFog.IslandDensity = 0.0030000;
	Weathers[n].SeaFog.Color         = argb(0, 77, 82, 82);
	Weathers[n].LandFog.Color        = argb(0, 77, 82, 82);

	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(  0, 13, 13, 13 );

	Weathers[n].Rain.DropsColor = argb( 63,255,255,255 );
	Weathers[n].Rain.DropsSize = 0.1;

	Weathers[n].Sun.Color = argb(  0, 78, 80, 84 );
	Weathers[n].Sun.Ambient = argb(  0, 85, 85, 85 );
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;
	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sun.Reflection.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 46, 59, 69);
	Weathers[n].Sea2.Reflection   = 0.6500000;
	Weathers[n].Sea2.Transparency = 0.1;
	Weathers[n].Sea2.Frenel       = 0.2000000;
	Weathers[n].Sea2.Attenuation  = 0.2000000;

	Weathers[n].Wind.Speed.Min = 18.0;
	Weathers[n].Wind.Speed.Max = 21.0;

	n++;

	Whr_PresetBase(n);
	Weathers[n].id = "Storm03";
	Weathers[n].Hour.Min = 18.00001;
	Weathers[n].Hour.Max = 22;
	Weathers[n].Lighting = "storm3";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].Tornado = true;
	Weathers[n].Lights = false;
	Weathers[n].Night = true;

	Weathers[n].Lightning.Enable = true;

	Weathers[n].Sky.Dir = "weather\skies\Storm03\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 1500;
	Weathers[n].LandFog.Height       = 1500;
	Weathers[n].SeaFog.Density       = 0.0045000;
	Weathers[n].LandFog.Density      = 0.0200000;
	Weathers[n].SeaFog.SeaDensity  = 0.0050000;
	Weathers[n].SeaFog.IslandDensity = 0.0020000;
	Weathers[n].LandFog.SeaDensity = 0.0200000;
	Weathers[n].LandFog.IslandDensity = 0.005;
	Weathers[n].SeaFog.Color         = argb(0, 41, 41, 46);
	Weathers[n].LandFog.Color        = argb(0, 43, 43, 46);

	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(  0, 33, 33, 33 );

	Weathers[n].Rain.DropsColor = argb( 60,255,255,255 );
	Weathers[n].Rain.DropsSize = 0.1;

	Weathers[n].Sun.Color = argb(  0, 50, 50, 50 );
	Weathers[n].Sun.Ambient = argb(  0, 35, 35, 35 );
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;
	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sun.Reflection.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 26, 31, 36);
	Weathers[n].Sea2.Reflection   = 0.6500000;
	Weathers[n].Sea2.Transparency = 0.1;
	Weathers[n].Sea2.Frenel       = 0.6;
	Weathers[n].Sea2.Attenuation  = 0.2;

	Weathers[n].Wind.Speed.Min = 18.0;
	Weathers[n].Wind.Speed.Max = 21.0;

	n++;

	Whr_PresetBase(n);
	Weathers[n].id = "Storm04";
	Weathers[n].Hour.Min = 22.00001;
	Weathers[n].Hour.Max = 6;
	Weathers[n].Lighting = "storm4";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].Tornado = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Lightning.Enable = true;

	Weathers[n].Sky.Dir = "weather\skies\Storm04\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 1500;
	Weathers[n].LandFog.Height       = 1500;
	Weathers[n].SeaFog.Density       = 0.0050000;
	Weathers[n].LandFog.Density      = 0.0095000;
	Weathers[n].SeaFog.SeaDensity  = 0.0050000;
	Weathers[n].SeaFog.IslandDensity = 0.0010000;
	Weathers[n].LandFog.SeaDensity = 0.0150000;
	Weathers[n].LandFog.IslandDensity = 0.0031500;
	Weathers[n].SeaFog.Color         = argb(0, 20, 23, 26);
	Weathers[n].LandFog.Color        = argb(0, 18, 23, 26);

	Weathers[n].Rain.NumDrops = 25000;
	Weathers[n].Rain.Color = argb(  0, 43, 43, 33 );

	Weathers[n].Rain.DropsColor = argb( 43,255,255,255 );
	Weathers[n].Rain.DropsSize = 0.1;

	Weathers[n].Sun.Color = argb(  0, 40, 40, 40 );
	Weathers[n].Sun.Ambient = argb(  0, 35, 35, 35 );
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;
	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sun.Reflection.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 59, 64, 59);
	Weathers[n].Sea2.Reflection   = 0.6500000;
	Weathers[n].Sea2.Transparency = 0.1;
	Weathers[n].Sea2.Frenel       = 0.0200000;
	Weathers[n].Sea2.Attenuation  = 0.0200000;

	Weathers[n].Wind.Speed.Min = 18.0;
	Weathers[n].Wind.Speed.Max = 21.0;

	n++;

// добавочные штормы для локций в низкоуровневым волнением

// Stormy1 - утренний шторм для локаций

	Whr_PresetBase(n);
	Weathers[n].id = "Storm01_add";
	Weathers[n].Hour.Min = 6.00001;
	Weathers[n].Hour.Max = 10;
	Weathers[n].Lighting = "storm1";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].skip = true;
	Weathers[n].Tornado = true;
	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.Enable = true;

	Weathers[n].Sky.Dir = "weather\skies\Storm01\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 2000;
	Weathers[n].LandFog.Height       = 2000;
	Weathers[n].SeaFog.Density       = 0.0045000;
	Weathers[n].LandFog.Density      = 0.0047;
	Weathers[n].SeaFog.SeaDensity  = 0.0045000;
	Weathers[n].SeaFog.IslandDensity = 0.0005000;
	Weathers[n].LandFog.SeaDensity = 0.0108;
	Weathers[n].LandFog.IslandDensity = 0.0012;
	Weathers[n].SeaFog.Color         = argb(0, 46, 54, 59);
	Weathers[n].LandFog.Color        = argb(  0, 30, 38, 46 );

	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,23,23,23);

	Weathers[n].Rain.DropsColor = argb(55,255,255,255);
	Weathers[n].Rain.DropsSize = 0.1;

	Weathers[n].Sun.Color = argb(0,72,74,78);
	Weathers[n].Sun.Ambient = argb(0,72,72,72);
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;
	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sun.Reflection.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 77, 77, 74);
	Weathers[n].Sea2.Reflection   = 0.5500000;
	Weathers[n].Sea2.Transparency = 0.1000000;
	Weathers[n].Sea2.Frenel       = 0.0300000;
	Weathers[n].Sea2.Attenuation  = 0.2000000;

	Weathers[n].Wind.Speed.Min = 15.0;
	Weathers[n].Wind.Speed.Max = 18.0;

	n++;

// Overcast2 - дневной шторм

	Whr_PresetBase(n);
	Weathers[n].id = "Storm02_add";
	Weathers[n].Hour.Min = 10.00001;
	Weathers[n].Hour.Max = 18;
	Weathers[n].Lighting = "storm2";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].skip = true;
	Weathers[n].Tornado = true;
	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.Enable = true;

	Weathers[n].Sky.Dir = "weather\skies\Storm02\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 2000;
	Weathers[n].LandFog.Height       = 2000;
	Weathers[n].SeaFog.Density       = 0.0045000;
	Weathers[n].LandFog.Density      = 0.0195000;
	Weathers[n].SeaFog.SeaDensity  = 0.0040000;
	Weathers[n].SeaFog.IslandDensity = 0.0005000;
	Weathers[n].LandFog.SeaDensity = 0.0145000;
	Weathers[n].LandFog.IslandDensity = 0.0030000;
	Weathers[n].SeaFog.Color         = argb(0, 77, 82, 82);
	Weathers[n].LandFog.Color        = argb(0, 77, 82, 82);

	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,13,13,13);

	Weathers[n].Rain.DropsColor = argb(63,255,255,255);
	Weathers[n].Rain.DropsSize = 0.1;

	Weathers[n].Sun.Color = argb(0,82,99,123);
	Weathers[n].Sun.Ambient = argb(0,25,25,25);
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;
	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sun.Reflection.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 46, 59, 69);
	Weathers[n].Sea2.Reflection   = 0.6500000;
	Weathers[n].Sea2.Transparency = 0.1;
	Weathers[n].Sea2.Frenel       = 0.2000000;
	Weathers[n].Sea2.Attenuation  = 0.2000000;

	Weathers[n].Wind.Speed.Min = 12.0;
	Weathers[n].Wind.Speed.Max = 14.0;

	n++;

// Вечерний шторм

	Whr_PresetBase(n);
	Weathers[n].id = "Storm03_add";
	Weathers[n].Hour.Min = 18.00001;
	Weathers[n].Hour.Max = 22;
	Weathers[n].Lighting = "storm3";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].skip = true;
	Weathers[n].Tornado = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Lightning.Enable = true;

	Weathers[n].Sky.Dir = "weather\skies\Storm03\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 1500;
	Weathers[n].LandFog.Height       = 1500;
	Weathers[n].SeaFog.Density       = 0.0045000;
	Weathers[n].LandFog.Density      = 0.0200000;
	Weathers[n].SeaFog.SeaDensity  = 0.0050000;
	Weathers[n].SeaFog.IslandDensity = 0.0020000;
	Weathers[n].LandFog.SeaDensity = 0.0200000;
	Weathers[n].LandFog.IslandDensity = 0.005;
	Weathers[n].SeaFog.Color         = argb(0, 41, 41, 46);
	Weathers[n].LandFog.Color        = argb(0, 43, 43, 46);

	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,33,33,33);

	Weathers[n].Rain.DropsColor = argb(60,255,255,255);
	Weathers[n].Rain.DropsSize = 0.1;

	Weathers[n].Sun.Color = argb(0,50,50,50);
	Weathers[n].Sun.Ambient = argb(0,40,40,40);
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;
	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sun.Reflection.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 26, 31, 36);
	Weathers[n].Sea2.Reflection   = 0.6500000;
	Weathers[n].Sea2.Transparency = 0.1;
	Weathers[n].Sea2.Frenel       = 0.6;
	Weathers[n].Sea2.Attenuation  = 0.2;

	Weathers[n].Wind.Speed.Min = 13.0;
	Weathers[n].Wind.Speed.Max = 16.0;

	n++;

// Ночной шторм

	Whr_PresetBase(n);
	Weathers[n].id = "Storm04_add";
	Weathers[n].Hour.Min = 22.00001;
	Weathers[n].Hour.Max = 6;
	Weathers[n].Lighting = "storm4";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].skip = true;
	Weathers[n].Tornado = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Lightning.Enable = true;

	Weathers[n].Sky.Dir = "weather\skies\Storm04\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 1500;
	Weathers[n].LandFog.Height       = 1500;
	Weathers[n].SeaFog.Density       = 0.0050000;
	Weathers[n].LandFog.Density      = 0.0095000;
	Weathers[n].SeaFog.SeaDensity  = 0.0050000;
	Weathers[n].SeaFog.IslandDensity = 0.0010000;
	Weathers[n].LandFog.SeaDensity = 0.0150000;
	Weathers[n].LandFog.IslandDensity = 0.0031500;
	Weathers[n].SeaFog.Color         = argb(0, 20, 23, 26);
	Weathers[n].LandFog.Color        = argb(0, 18, 23, 26);

	Weathers[n].Rain.NumDrops = 25000;
	Weathers[n].Rain.Color = argb(0,43,43,33);

	Weathers[n].Rain.DropsColor = argb(43,255,255,255);
	Weathers[n].Rain.DropsSize = 0.1;

	Weathers[n].Sun.Color = argb(0,40,40,40);
	Weathers[n].Sun.Ambient = argb(0,30,30,25);
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;
	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sun.Reflection.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 59, 64, 59);
	Weathers[n].Sea2.Reflection   = 0.6500000;
	Weathers[n].Sea2.Transparency = 0.1;
	Weathers[n].Sea2.Frenel       = 0.0200000;
	Weathers[n].Sea2.Attenuation  = 0.0200000;

	Weathers[n].Wind.Speed.Min = 12.0;
	Weathers[n].Wind.Speed.Max = 17.0;

	n++;

	return n;
}
