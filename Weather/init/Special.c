int Whr_InitSpecial(int n)
{

	Whr_PresetBase(n);
	Weathers[n].id = "Inside";

	Weathers[n].Hour.Min = 0;
	Weathers[n].Hour.Max = 24;

	Weathers[n].Lighting = "inside";
	Weathers[n].LightingLm = "inside";

	Weathers[n].InsideBack = "";

	Weathers[n].Skip = true;
	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.Enable = false;

	Weathers[n].Sky.Dir = "weather\skies\10\";
	Weathers[n].Sky.Size = 512;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = false;
	Weathers[n].LandFog.Enable       = false;
	Weathers[n].SeaFog.Height        = 200;
	Weathers[n].LandFog.Height       = 200;
	Weathers[n].SeaFog.Density       = 0.0005;
	Weathers[n].LandFog.Density      = 0.0005;
	Weathers[n].SeaFog.IslandDensity = 0.001;
	Weathers[n].LandFog.IslandDensity = 0.0005;
	Weathers[n].SeaFog.SeaDensity  = 0.00048;
	Weathers[n].LandFog.SeaDensity = 0.00072;
	Weathers[n].SeaFog.Color         = argb(0,159,158,152);
	Weathers[n].LandFog.Color        = argb(0,159,158,152);

	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.Color = argb(0,73,73,73);

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	Weathers[n].Rain.DropsNearRadius = 25.0;
	Weathers[n].Rain.DropsFarRadius = 75.0;
	Weathers[n].Rain.DropsLifeTime = 0.25;
	Weathers[n].Rain.DropsSize = 0.06;
	Weathers[n].Rain.DropsColor = argb(43,255,255,255);

	Weathers[n].Sun.Color = argb(  0, 80, 80, 68 );
	Weathers[n].Sun.Ambient = argb(  0, 40, 30, 20 );

	Weathers[n].Sun.Glow.Enable = false;

	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Reflection.Enable = true;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 10, 55, 100);

	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 8.0;

	n++;

	Whr_PresetBase(n);
	Weathers[n].id = "Underwater";
	Weathers[n].Hour.Min = 0;
	Weathers[n].Hour.Max = 24;

	Weathers[n].Lighting = "morning10";
	Weathers[n].LightingLm = "morning";

	Weathers[n].InsideBack = "m";

	Weathers[n].Skip = true;

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.SubTexX = 4;

	Weathers[n].Sky.Dir = "weather\skies\10\";
	Weathers[n].Sky.Size = 2048.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;

	Weathers[n].SeaFog.Enable        = true;
	Weathers[n].LandFog.Enable       = true;
	Weathers[n].SeaFog.Height        = 1000;
	Weathers[n].LandFog.Height       = 1000;
	Weathers[n].SeaFog.Density       = 0.005;
	Weathers[n].LandFog.Density      = 0.005;
	Weathers[n].SeaFog.IslandDensity = 0.01;
	Weathers[n].LandFog.IslandDensity = 0.005;
	Weathers[n].SeaFog.SeaDensity  = 0.03;
	Weathers[n].LandFog.SeaDensity = 0.045;
	Weathers[n].SeaFog.Color         = argb(  0, 2, 95, 138 );
	Weathers[n].LandFog.Color        = argb(  0, 2, 95, 138 );

	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.Color = argb(0,73,73,73);

	Weathers[n].Rain.DropsFarNum = 500;
	Weathers[n].Rain.DropsColor = argb(43,255,255,255);

	Weathers[n].Sun.Color = argb(  0, 10, 40, 43 );
	Weathers[n].Sun.Ambient = argb(  0,134,199,209 );

	Weathers[n].Sun.Glow.Enable = false;

	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Size = 4000.0;

	Weathers[n].Sun.Reflection.Enable = true;

	Weathers[n].Sun.Flare.Enable = false;

	Weathers[n].Sea2.WaterColor = argb(0, 65, 100, 110);

	Weathers[n].Wind.Speed.Min = 7.0;
	Weathers[n].Wind.Speed.Max = 14.5;

	n++;

	return n;
}
