

void Whr_SunStep(string sId, float fTime, int iSunSize, int iSize, int iReflSize,
                 float fFlareScale, string sSunTex, string sGlowTex, string sReflTex, string sOverTex)
{
	aref a;
	makearef(a, WhrCommonParams.SunStep.(sId));
	a.Time = fTime;
	if (iSunSize    >= 0)   { a.SunSize    = iSunSize; }
	if (iSize       >= 0)   { a.Size       = iSize; }
	if (iReflSize   >= 0)   { a.ReflSize   = iReflSize; }
	if (fFlareScale >= 0.0) { a.FlareScale = fFlareScale; }
	if (sSunTex  != "")     { a.SunTex  = sSunTex; }
	if (sGlowTex != "")     { a.GlowTex = sGlowTex; }
	if (sReflTex != "")     { a.ReflTex = sReflTex; }
	if (sOverTex != "")     { a.OverTex = sOverTex; }
}

void Whr_SunStepRefl(string sId, int iColor)
{
	WhrCommonParams.SunStep.(sId).ReflColor = iColor;
}

void Whr_WaterRange(string sKind, int iFrom, int iTo, int iColor, float fReflection, float fTransparency, float fFrenel, float fAttenuation)
{
	int    h;
	string sH;
	for (h = iFrom; h <= iTo; h++)
	{

		sH = "h" + h;
		WhrCommonParams.Water.(sKind).(sH).Color        = iColor;
		WhrCommonParams.Water.(sKind).(sH).Reflection   = fReflection;
		WhrCommonParams.Water.(sKind).(sH).Transparency = fTransparency;
		WhrCommonParams.Water.(sKind).(sH).Frenel       = fFrenel;
		WhrCommonParams.Water.(sKind).(sH).Attenuation  = fAttenuation;
	}
}

void Whr_SeaRow(int h, int iColor, float fReflection, float fAttenuation)
{
	string sH = "h" + h;
	WhrCommonParams.Water.sea.(sH).Color        = iColor;
	WhrCommonParams.Water.sea.(sH).Reflection   = fReflection;
	WhrCommonParams.Water.sea.(sH).Transparency = 0.13;
	WhrCommonParams.Water.sea.(sH).Frenel       = 0.3;
	WhrCommonParams.Water.sea.(sH).Attenuation  = fAttenuation;
}

void Whr_FoamHours(int iFrom, int iTo, float fFoamK)
{
	int    h;
	string sH;
	for (h = iFrom; h <= iTo; h++)
	{
		sH = "h" + h;
		WhrCommonParams.Waves.Hour.(sH).FoamK = fFoamK;
	}
}

void Whr_StormFoamHours(int iFrom, int iTo, float fFoamK)
{
	int    h;
	string sH;
	for (h = iFrom; h <= iTo; h++)
	{
		sH = "h" + h;
		WhrCommonParams.Waves.Storm.HourFoamK.(sH) = fFoamK;
	}
}

void Whr_MoonRefl(string sId, float fTime, int iSize)
{
	WhrCommonParams.Sun.MoonRefl.(sId).Time = fTime;
	WhrCommonParams.Sun.MoonRefl.(sId).Size = iSize;
}

void Whr_WavePoint(string sId, float fWind, float fAmp1, float fAmp2, float fScale1, float fScale2,
                   float fAnim1, float fAnim2, float fMove1, float fMove2, float fBump, float fPosShift,
                   float fFoamUV, float fTexDist)
{
	aref a;
	makearef(a, WhrCommonParams.Waves.Curve.(sId));
	a.Wind     = fWind;
	a.Amp1     = fAmp1;
	a.Amp2     = fAmp2;
	a.Scale1   = fScale1;
	a.Scale2   = fScale2;
	a.Anim1    = fAnim1;
	a.Anim2    = fAnim2;
	a.Move1    = fMove1;
	a.Move2    = fMove2;
	a.Bump     = fBump;
	a.PosShift = fPosShift;
	a.FoamUV   = fFoamUV;
	a.TexDist  = fTexDist;
}

void Whr_PlaceRow(string sKind, float fHeight, float fStormHeight)
{
	WhrCommonParams.Waves.Place.(sKind).Height      = fHeight;
	WhrCommonParams.Waves.Place.(sKind).StormHeight = fStormHeight;
}

void Whr_InitCommon()
{

	DeleteAttribute(&WhrCommonParams, "Water");

	Whr_WaterRange("land",   0,  4, argb(0,16, 24, 36), 0.55, 0.6, 0.3, 0.4);
	Whr_WaterRange("land",   5,  5, argb(0,16, 24, 36), 0.55, 0.6, 0.3, 0.3);
	Whr_WaterRange("land",   6,  6, argb(0, 0, 76, 93), 0.55, 0.6, 0.3, 0.3);
	Whr_WaterRange("land",   7,  8, argb(0, 0, 84,108), 0.60, 0.6, 0.3, 0.3);
	Whr_WaterRange("land",   9,  9, argb(0, 0,128,152), 0.60, 0.6, 0.3, 0.3);
	Whr_WaterRange("land",  10, 18, argb(0, 0,172,196), 0.60, 0.6, 0.3, 0.3);
	Whr_WaterRange("land",  19, 19, argb(0,61,122,148), 0.55, 0.6, 0.3, 0.3);
	Whr_WaterRange("land",  20, 20, argb(0,42, 70, 92), 0.55, 0.6, 0.2, 0.2);
	Whr_WaterRange("land",  21, 21, argb(0,26, 42, 60), 0.75, 0.6, 0.2, 0.2);
	Whr_WaterRange("land",  22, 23, argb(0,16, 24, 36), 0.55, 0.6, 0.3, 0.3);

	Whr_WaterRange("seashore",  0,  4, argb(0,16, 24, 36), 0.3, 0.13, 0.3, 0.4);
	Whr_WaterRange("seashore",  5,  5, argb(0,16, 24, 36), 0.37, 0.13, 0.3, 0.3);
	Whr_WaterRange("seashore",  6,  6, argb(0, 0, 76, 93), 0.44, 0.13, 0.3, 0.3);
	Whr_WaterRange("seashore",  7,  8, argb(0, 0, 84,108), 0.51, 0.13, 0.3, 0.3);
	Whr_WaterRange("seashore",  9,  9, argb(0, 0,128,152), 0.51, 0.13, 0.3, 0.3);
	Whr_WaterRange("seashore", 10, 18, argb(0, 7,122,155), 0.51, 0.13, 0.3, 0.3);
	Whr_WaterRange("seashore", 19, 19, argb(0,61,122,148), 0.44, 0.13, 0.3, 0.3);
	Whr_WaterRange("seashore", 20, 20, argb(0,42, 70, 92), 0.37, 0.13, 0.3, 0.3);
	Whr_WaterRange("seashore", 21, 21, argb(0,26, 42, 60), 0.51, 0.13, 0.2, 0.2);
	Whr_WaterRange("seashore", 22, 23, argb(0,16, 24, 36), 0.3, 0.13, 0.3, 0.3);

	Whr_WaterRange("cave",   0,  4, argb(0, 0, 15, 20), 0.60, 1.3, 0.3, 0.4);
	Whr_WaterRange("cave",   5, 23, argb(0, 0, 15, 20), 0.60, 1.3, 0.3, 0.3);

	Whr_SeaRow( 0, argb(0,7,16,30),   0.55, 0.4);
	Whr_SeaRow( 1, argb(0,7,16,30),   0.55, 0.4);
	Whr_SeaRow( 2, argb(0,7,16,30),   0.55, 0.4);
	Whr_SeaRow( 3, argb(0,7,16,30),   0.55, 0.4);
	Whr_SeaRow( 4, argb(0,7,18,33),   0.55, 0.4);
	Whr_SeaRow( 5, argb(0,8,21,37),   0.55, 0.3);
	Whr_SeaRow( 6, argb(0,8,23,40),   0.51, 0.3);
	Whr_SeaRow( 7, argb(0,8,29,53),   0.51, 0.3);
	Whr_SeaRow( 8, argb(0,10,40,70),  0.51, 0.3);
	Whr_SeaRow( 9, argb(0,5,37,65),   0.51, 0.3);
	Whr_SeaRow(10, argb(0,5,47,77),   0.51, 0.3);
	Whr_SeaRow(11, argb(0,15,75,120), 0.51, 0.3);
	Whr_SeaRow(12, argb(0,15,73,115), 0.51, 0.3);
	Whr_SeaRow(13, argb(0,15,73,115), 0.51, 0.3);
	Whr_SeaRow(14, argb(0,15,73,115), 0.51, 0.3);
	Whr_SeaRow(15, argb(0,15,73,115), 0.51, 0.3);
	Whr_SeaRow(16, argb(0,15,73,115), 0.51, 0.3);
	Whr_SeaRow(17, argb(0,15,73,115), 0.51, 0.3);
	Whr_SeaRow(18, argb(0,13,66,105), 0.51, 0.3);
	Whr_SeaRow(19, argb(0,3,46,69),   0.8, 0.2);
	Whr_SeaRow(20, argb(0,3,26,46),   0.55, 0.2);
	Whr_SeaRow(21, argb(0,3,23,41),   0.51, 0.3);
	Whr_SeaRow(22, argb(0,7,17,31),   0.55, 0.3);
	Whr_SeaRow(23, argb(0,7,16,30),   0.55, 0.3);

	DeleteAttribute(&WhrCommonParams, "Waves");

	aref w;
	makearef(w, WhrCommonParams.Waves.Base);
	w.MaxSeaDistance     = 20000.0;
	w.MaxSeaDistanceLand =  3000.0;

	w.SkyColor  = argb(0,255,255,255);
	w.FoamVK    = 0.50;
	w.Move2Turn = 0.0;

	Whr_FoamHours( 0,  4, 0.030);
	Whr_FoamHours( 5,  5, 0.050);
	Whr_FoamHours( 6, 18, 0.080);
	Whr_FoamHours(19, 19, 0.060);
	Whr_FoamHours(20, 20, 0.050);
	Whr_FoamHours(21, 21, 0.035);
	Whr_FoamHours(22, 23, 0.030);

	Whr_WavePoint("p1",  4.0,  3.85000, 1.7500, 0.600, 1.50, 1.25, 2.0, 2.0, 2.00, 0.0440, 2.25,    0.55,  0.5);
	Whr_WavePoint("p2",  6.5,  5.68750, 1.7500, 0.400, 1.50, 3.00, 2.0, 3.5, 3.50, 0.0440, 3.20,    0.40,  0.5);
	Whr_WavePoint("p3",  9.3,  6.78125, 2.6250, 0.300, 1.25, 1.50, 3.0, 3.0, 1.41, 0.0440, 2.25,    0.30,  0.5);
	Whr_WavePoint("p4", 12.2,  8.57500, 2.6250, 0.300, 1.00, 2.00, 3.0, 4.0, 1.00, 0.0440, 3.00,    0.24,  0.7);
	Whr_WavePoint("p5", 15.5, 13.47500, 2.8875, 0.200, 1.00, 1.75, 4.0, 5.0, 1.00, 0.0440, 2.40,    0.23,  0.5);
	Whr_WavePoint("p6", 19.0, 22.13750, 5.2500, 0.125, 0.75, 2.00, 6.0, 8.0, 1.00, 0.0440, 2.55,    0.10,  0.7);

	makearef(w, WhrCommonParams.Waves.Storm);
	w.Amp1K = 1.4;   w.Amp1B = 1.0;  w.Amp2 = 9.0;
	w.Scale1 = 0.075; w.Scale2 = 0.5; w.Anim1 = 2.25; w.Anim2 = 8.0; w.Move1 = 12.0; w.Move2 = 4.0;
	w.Bump = 0.05;  w.PosShift = 2.25; w.FoamVK = 0.47; w.FoamUV = 0.3; w.TexDist = 0.7;
	Whr_StormFoamHours( 0,  5, 0.015);
	Whr_StormFoamHours( 6, 20, 0.016);
	Whr_StormFoamHours(21, 23, 0.015);

	makearef(w, WhrCommonParams.Waves.Ceiling);

	w.Near  = 5.0;   w.NearR = 1.0;
	w.Far   = 7.0;   w.FarR  = 2.0;
	w.StormK = 2.0;
	w.Open  = 200.0;
	w.NoSea = 6.0;

	WhrCommonParams.Waves.Land.WindStorm  = 18.0;

	makearef(w, WhrCommonParams.Waves.Land.normal);

	w.Amp1K  = 0.300; w.Amp2K  = 0.120; w.AmpMin = 0.10;  w.AmpMax  = 5.00;
	w.Scale1 = 1.75;  w.Scale2 = 2.00;

	w.Bump   = 0.15;

	w.Move2Turn = 0.0;
	w.FoamV  = 0.30;  w.FoamUV = 0.30;  w.TexDist = 0.70;
	w.FoamK  = 0.0;

	makearef(w, WhrCommonParams.Waves.Land.storm);
	w.Amp1 = 3.0; w.Amp2 = 0.45;  w.Scale1 = 3.0; w.Scale2 = 3.6; w.Anim1 = 4.0; w.Anim2 = 5.5;
	w.Move1 = "4.0, 0.0, 0.0";  w.Move2 = "0.0, 0.0, -2.4";
	w.Bump = 0.08;  w.PosShift = 0.5;  w.FoamV = 0.6; w.FoamK = 0.15; w.FoamKDay = 0.14; w.FoamUV = 0.20; w.TexDist = 0.7;

	Whr_PlaceRow("land",  4.0, 6.0);
	Whr_PlaceRow("cave",  0.25, 0.25);
	Whr_PlaceRow("other", 1.5, 4.5);

	DeleteAttribute(&WhrCommonParams, "SunStep");

	string sSun   = "weather\sun\glow\sun.tga.tx";
	string sSunDg = "weather\sun\glow\sun_dg.tga.tx";
	string sRefN  = "weather\sun\reflection\refl_night.tga.tx";
	string sRefM  = "weather\sun\reflection\refl_morning.tga.tx";
	string sRefD  = "weather\sun\reflection\refl_day.tga.tx";
	string sRefE  = "weather\sun\reflection\refl_evening.tga.tx";
	string sOvfM  = "weather\sun\overflow\overflow_morning.tga.tx";
	string sOvfE  = "weather\sun\overflow\overflow_evening.tga.tx";

	Whr_SunStep("s01",  5.00,     0,    0,       0,     0.0,          "",         "",            "",            "");
	Whr_SunStep("s1a",  6.17,     0,  175,     120,     0.0,          "",       sSun,          sRefE,           "");
	Whr_SunStep("s1b",  6.25,     0,  175,     450,     0.0,          "",       sSun,          sRefE,           "");
	Whr_SunStep("s02",  6.33,     0,  175,     640,     0.0,          "",       sSun,          sRefE,           "");
	Whr_SunStep("s03",  6.42,     0,  175,     600,    -1.0,          "",       sSun,          sRefM,         sOvfM);
	Whr_SunStep("s04",  7.00,   250,  175,     765,    -1.0,       sSunDg,      sSun,          sRefD,           "");
	Whr_SunStep("s05",  8.00,   500,  175,     765,    -1.0,       sSunDg,      sSun,          sRefD,           "");
	Whr_SunStep("s06",  9.00,  2000,  175,     765,    -1.0,       sSunDg,      sSun,          sRefD,           "");
	Whr_SunStep("s07", 17.00,   750,  175,     765,    -1.0,       sSunDg,      sSun,          sRefD,           "");
	Whr_SunStep("s08", 18.80,     0,  500,     765,    -1.0,       sSun,        sSunDg,        sRefD,           "");
	Whr_SunStep("s09", 19.00,     0,  175,     600,    -1.0,          "",       sSun,          sRefE,         sOvfE);
	Whr_SunStep("s10", 19.70,     0,  175,     450,     0.0,          "",       sSun,          sRefE,         sOvfE);
	Whr_SunStep("s11", 20.00,     0,    0,       0,    -1.0,          "",         "",            "",            "");

	Whr_SunStepRefl("s01", argb(0,  0,  0,  0));
	Whr_SunStepRefl("s1a", argb(0, 40, 37, 33));
	Whr_SunStepRefl("s1b", argb(0,115,108, 98));
	Whr_SunStepRefl("s02", argb(0,185,175,160));
	Whr_SunStepRefl("s03", argb(0,215,206,192));
	Whr_SunStepRefl("s04", argb(0,225,225,225));

	WhrCommonParams.Sun.StepFrom      = 5.0;
	WhrCommonParams.Sun.StepTo        = 20.5;
	WhrCommonParams.Sun.NightFrom     = 20.5;
	WhrCommonParams.Sun.NightTo       = 5.5;
	WhrCommonParams.Sun.MoonSize      = 250;
	WhrCommonParams.Sun.GlowColor     = argb(0,255,255,255);
	WhrCommonParams.Sun.ReflColor     = argb(0,225,225,225);
	WhrCommonParams.Sun.OverColorM    = argb(0,30,30,30);
	WhrCommonParams.Sun.MoonDir       = "weather\sun\glow\moon\moon";
	WhrCommonParams.Sun.ReflNightTex  = sRefN;
	WhrCommonParams.Sun.BaseSunTex    = sSun;
	WhrCommonParams.Sun.BaseMoonTex   = "weather\sun\glow\moon.tga.tx";
	WhrCommonParams.Sun.BaseGlowTex   = "weather\sun\glow\sunglow.tga.tx";

	WhrCommonParams.Sun.MoonDayFrom  = 3;
	WhrCommonParams.Sun.MoonDayTo    = 29;

	DeleteAttribute(&WhrCommonParams, "Sun.MoonRefl");

	Whr_MoonRefl("m0",  21.00,   0);
	Whr_MoonRefl("m0a", 22.00,   0);
	Whr_MoonRefl("m0b", 23.00, 250);
	Whr_MoonRefl("m1",  28.00, 250);
	Whr_MoonRefl("m2",  29.00,   0);

	WhrCommonParams.Sun.Arc.Height       = 3.5 / 4.0 * PId2;
	WhrCommonParams.Sun.Arc.BegAngle     = 0.3;
	WhrCommonParams.Sun.Arc.BegTime      = 5.5;
	WhrCommonParams.Sun.Arc.EndAngle     = -3.25;
	WhrCommonParams.Sun.Arc.EndTime      = 20.5;
	WhrCommonParams.Sun.MoonArc.Height   = 2.5 / 4.0 * PId2;
	WhrCommonParams.Sun.MoonArc.BegAngle = 0.0;
	WhrCommonParams.Sun.MoonArc.BegTime  = 21.0;
	WhrCommonParams.Sun.MoonArc.EndAngle = 3.0;
	WhrCommonParams.Sun.MoonArc.EndTime  = 5.0;

	DeleteAttribute(&WhrCommonParams, "Light");

	WhrCommonParams.Light.Inside.SunColor = argb(0,85,80,80);
	WhrCommonParams.Light.Inside.Ambient  = argb(0,50,50,50);

	WhrCommonParams.Light.ShowcaseAmbient = 90;
}

void Whr_PresetBase(int n)
{
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "weather\lightning\flash.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.Texture = "weather\lightning\lightning_storm.tga";
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.DropsFarNum = 500;
	Weathers[n].Rain.DropsFarRadius = 75.0;
	Weathers[n].Rain.DropsLifeTime = 0.25;
	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsNearRadius = 25.0;
	Weathers[n].Rain.DropsSize = 0.1;
	Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.MaxBlend = 70;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 14.0;
	Weathers[n].Rain.TimeBlend = 2000;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";
	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);
	Weathers[n].Sky.Angle = 0.0;
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 27.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.VisualMagnitude = 8.0;
	Weathers[n].Sun.Flare.Dist = 3000.0;
	Weathers[n].Sun.Flare.Scale = 2.0;
	Weathers[n].Sun.Flare.Technique = "sunflare";
	Weathers[n].Sun.Flare.TexSizeX = 4;
	Weathers[n].Sun.Flare.TexSizeY = 4;
	Weathers[n].Sun.Flare.Texture = "weather\sun\flare\allflares.tga";
	Weathers[n].Sun.Flares.f1 = "1.0,600.0,0,808080";
	Weathers[n].Sun.Flares.f10 = "0.48,70.0,13,303030";
	Weathers[n].Sun.Flares.f11 = "0.41,60.0,14,404040";
	Weathers[n].Sun.Flares.f12 = "0.30,130.0,7,202020";
	Weathers[n].Sun.Flares.f13 = "0.32,110.0,6,404040";
	Weathers[n].Sun.Flares.f14 = "0.26,50.0,12,404040";
	Weathers[n].Sun.Flares.f15 = "0.22,60.0,13,202020";
	Weathers[n].Sun.Flares.f16 = "0.20,60.0,15,303030";
	Weathers[n].Sun.Flares.f17 = "0.15,50.0,12,404040";
	Weathers[n].Sun.Flares.f18 = "0.10,40.0,11,808080";
	Weathers[n].Sun.Flares.f19 = "0.13,400.0,10,404040";
	Weathers[n].Sun.Flares.f2 = "1.0,250.0,1,606060";
	Weathers[n].Sun.Flares.f3 = "0.90,100.0,2,404040";
	Weathers[n].Sun.Flares.f4 = "0.85,300.0,3,606060";
	Weathers[n].Sun.Flares.f5 = "0.75,50.0,12,808080";
	Weathers[n].Sun.Flares.f6 = "0.66,60.0,15,404040";
	Weathers[n].Sun.Flares.f7 = "0.59,50.0,13,202020";
	Weathers[n].Sun.Flares.f8 = "0.55,70.0,5,404040";
	Weathers[n].Sun.Flares.f9 = "0.42,100.0,5,404040";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
	Weathers[n].Sun.Overflow.Color = argb(0,100,100,100);
	Weathers[n].Sun.Overflow.Start = 0.7;
	Weathers[n].Sun.Overflow.Technique = "sunoverflow";
	Weathers[n].Sun.Overflow.Texture = "weather\sun\overflow\overflow.tga";
	Weathers[n].Sun.Reflection.Color = argb(0,200,200,200);
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_day.tga";
}

void Whr_PresetHour(int n)
{
	Weathers[n].LandFog.Enable = true;
	Weathers[n].LandFog.Density = 0.001;
	Weathers[n].LandFog.Height = 200;
	Weathers[n].LandFog.IslandDensity = 0.0002;
	Weathers[n].LandFog.SeaDensity = 0.0017;
	Weathers[n].Lightning.Enable = false;
	Weathers[n].Rain.Color = argb(0,73,73,73);
	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].SeaFog.Enable = true;
	Weathers[n].SeaFog.Height = 200;
	Weathers[n].Sky.Size = 2048.0;
	Weathers[n].Storm = false;
	Weathers[n].Sun.Glow.Enable = true;
	Weathers[n].Sun.Overflow.Enable = true;
	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Tornado = false;
}
