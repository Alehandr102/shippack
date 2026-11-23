#define PARSE_STRING	"PARSE_STRING"

#event_handler(PARSE_STRING, "ParseString");

int AddStr2Array(ref rNames, int iNum, string sStr)
{
	SetArraySize(rNames, iNum + 1);
	rNames[iNum] = sStr;
	iNum++;
	return iNum;
}

string GetSubStringByNum(string sStr, int iSelect)
{
	string sTemp;
	int iFindPos = findSubStr(&sStr, ",", 0);
	if (iFindPos < 0) return sStr;	// one word in string
	int iNumFind = 1;
    while (iFindPos > 0)
	{
		iNumFind++;
		iFindPos = findSubStr(&sStr, ",", iFindPos + 1);
	}
	if (iNumFind <= iSelect)
	{
        return GetSubStringByNum(sStr, 0);
	}
	int	iLastPos = 0;
	iFindPos = 0;
	for (int i=0; i<iNumFind; i++)
	{
		iFindPos = findSubStr(&sStr, ",", iFindPos + 1);
		if (i == iSelect)
		{
			if (iFindPos < 0)
            {
                iFindPos = strlen(&sStr);
            }
            if (iLastPos >=  iFindPos) iFindPos = iLastPos + 2;
			sTemp = strcut(&sStr, iLastPos, iFindPos - 1);
			return sTemp;
		}
		iLastPos = iFindPos + 1;
	}

	return GetSubStringByNum(sStr, 0);
}

string GetRandSubString(string strInput)
{
	string sTemp;
    string sStr = strInput;

    if (strlen(sStr) > 0)//fix
    {
    	int iFindPos = findSubStr(sStr, ",", 0);
    	if (iFindPos < 0) return sStr;	// one word in string
    	int iNumFind = 1;
    	while (iFindPos > 0)
    	{
    		iNumFind++;
    		iFindPos = findSubStr(sStr, ",", iFindPos + 1);
    	}
    	int iSelect = rand(iNumFind - 1);
    	int	iLastPos = 0;
    	iFindPos = 0; // вот где собака порылась!!!!! считай я убил Вано :) Boal
    	for (int i=0; i<iNumFind; i++)
    	{
    		iFindPos = findSubStr(sStr, ",", iFindPos + 1);
    		if (i == iSelect)
    		{
    			if (iFindPos == -1)
                {
                    iFindPos = strlen(sStr);
                }

                 if (iLastPos >=  iFindPos) return "";
    			sTemp = strcut(sStr, iLastPos, iFindPos - 1);
    			return sTemp;
    		}
    		iLastPos = iFindPos + 1;
    	}
    	return "ERROR: bad string return";
	}
	return   "";
}

String GetRussianNumberString(int _num)
{
	if(_num < -999 || _num > 999) return "Error";
	if(_num == 0) return XI_ConvertString("zero");

	String numString = its(abs(_num));

	int strLength = strlen(numString);

	String ones = GetSymbol(numString, strLength - 1);
	String tens = GetSymbol(numString, strLength - 2) + "0";
	String hundreds = GetSymbol(numString, strLength - 3) + "00";

	numString = "";

	if(_num < 0)
	{
		numString = XI_ConvertString("minus") + " ";
	}

	if(tens == "10")
	{
		tens = "";
		ones = "1" + ones;
	}

	ones = XI_ConvertString(ones);

	if(tens != "")
	{
		tens = XI_ConvertString(tens) + " ";
	}

	if(hundreds != "")
	{
		hundreds = XI_ConvertString(hundreds) + " ";
	}

	return numString + hundreds + tens + ones;
}
float GetDotProduct(float fA1, float fA2)
{
    return cos(fA2 - fA1);
}

void ParseString()
{
	int		i, iLen;
	string	sParams[2];
	string	sDst = "";

	aref aRes = GetEventData();
	string rSourceString = GetEventData();
	int	iNumParameters = GetEventData();

	if (iNumParameters == 0)
	{
		aRes.Str = rSourceString;
		return;
	}

	SetArraySize(&sParams, iNumParameters + 1);
	for (i=0; i<iNumParameters; i++) { sParams[i] = GetEventData();	}

	iLen = strlen(&rSourceString);
	for (i=0; i<iLen; i++)
	{
		string sChar = GetSymbol(&rSourceString, i);
		if (sChar == "%")
		{
			if (GetSymbol(&rSourceString, i + 1) == "%")
			{
				sDst = sDst + "%";
				i++;
				continue;
			}
			string sNumber = "";
			i++;
			while (i < iLen)
			{
				string sTmp = GetSymbol(&rSourceString, i);
				if (!isDigit(&sTmp, 0)) { break; }
				sNumber = sNumber + sTmp;
				i++;
			}
			sDst = sDst + sParams[sti(sNumber) - 1]; i--;
			continue;
		}
		sDst = sDst + sChar;
	}

	aRes.Str = sDst;
}

float GetDistance2D(float x1, float y1, float x2, float y2)
{
	return sqrt(sqr(x1 - x2) + sqr(y1 - y2));
}

float GetDistance3D(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2)
{
	return sqrt(sqr(_x1 - _x2) + sqr(_y1 - _y2) + sqr(_z1 - _z2));
}

float GetDistance2DRel(float x1, float y1, float x2, float y2)
{
	return sqr(x1 - x2) + sqr(y1 - y2);
}

void RotateAroundY(ref rX, ref rZ, float fCos, float fSin)
{
	float xxx = rX;
	float zzz = rZ;
	float xx = xxx * fCos + zzz * fSin;
	float zz = zzz * fCos - xxx * fSin;
	rX = xx;
	rZ = zz;
}

int Tonnes2CWT(int iTonnes)
{
	int a = iTonnes * 10000 / 508;
	a = 100 * ((a + 99) / 100);
	return a;
}

int i_min(int a1, int a2)
{
	if (a1 < a2) { return a1; }
	return a2;
}

string GetVerNum()
{
 	string  VERSION_STR = VERSION_NUMBER2;

    if (MOD_BETTATESTMODE == "On")
    {
       VERSION_STR = VERSION_STR + "  DEV";
    }
    if (MOD_BETTATESTMODE == "Test")
    {
       VERSION_STR = VERSION_STR + "  TST";
    }
    return VERSION_STR;
}

void Log_Info(string _str)
{
    Log_SetStringToLog(_str); // запись в Журнал игры происходит внутри Log_SetStringToLog (ловит и прямые боевые логи)
}

int bGameLogSkip = 0;

// LEO: добавить запись в Журнал игры с игровой датой и игровым временем. Вне игры (нет даты) — не пишем.
void GameLog_Record(string _str)
{
    if (GetDataYear() < 100) return;

    if (CheckAttribute(&InterfaceStates, "ShowGameLog") && InterfaceStates.ShowGameLog == "0") return;
    int mn = GetDataMonth();
    string sDate = GetDataDay() + " " + XI_ConvertString("target_month_" + mn) + " " + GetDataYear();
    int gh = makeint(GetHour());
    int gm = makeint(GetMinute());
    string sH = "" + gh; if (gh < 10) sH = "0" + gh;
    string sM = "" + gm; if (gm < 10) sM = "0" + gm;
    GameLogAdd(sDate + ", " + sH + ":" + sM + " - " + _str); // инлайн-маркеры \1 выкидывает сам GameLogAdd (движок)
}
// LEO: пустую скриптовую заглушку GameLogAdd УБРАЛ — она перехватывала вызов, и запись не доходила до движка.
// GameLogAdd — нативная функция движка (объявлена `native void GameLogAdd(string)` в locations.c, #libriary ScriptLocationLibrary), как GameLogGetCount/GetLine.

void Log_TestInfo(string logtext)
{
	if (bBettaTestMode)
	{
		bGameLogSkip = 1; // LEO: тест-логи НЕ пишем в Журнал игры
		Log_SetStringToLog(logtext);
		bGameLogSkip = 0;
		trace("TestInfo: " + GetQuestBookDataDigit() + " " + logtext)
	}
}
void Log_QuestInfo(string _info)
{
    if (bQuestLogShow)
	{
		Log_SetStringToLog(_info);
	}
}

void list_ref_props(ref objRef) {
	aref arItem;
	string sItem;
	for (int i = 0; i < GetAttributesNum(objRef); i++) {
		arItem = GetAttributeN(objRef, i);
		sItem = GetAttributeName(arItem);
		log_info(sItem)
	}
}

string NewStr()
{
    int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
    string sTemp;
    sTemp = LanguageConvertString(idLngFile,"new_string");
    LanguageCloseFile(idLngFile);

    return sTemp;
}

Object objGrass;

void CreateGrass(string sDataFile, string sTextureFile, float fScale, float fW,float fH, float fMinDist,float fMaxDist, float fMinLod)
{
	CreateEntity(&objGrass,"Grass");
	if( bSeaActive && !bAbordageStarted )
	{
		LayerAddObject(SEA_EXECUTE,&objGrass,1000);
		LayerAddObject(SEA_REALIZE,&objGrass,1000);
	}
	else
	{
		LayerAddObject(EXECUTE,&objGrass,1000);
		LayerAddObject(REALIZE,&objGrass,1000);
	}
	SendMessage(objGrass,"ls",41666, sTextureFile);
	SendMessage(objGrass,"lffffff",42666, fScale, fW,fH, fMinDist,fMaxDist, fMinLod);
	SendMessage(objGrass,"ls",40666, sDataFile);

	InterfaceStates.LastGrass.scale = fScale;
	InterfaceStates.LastGrass.w = fW;
	InterfaceStates.LastGrass.h = fH;
	ApplyIslandVegetation();
}

// Gregg: перевод ползунка «Количество спрайтов» (0..1 = доля видимой растительности) в ПОЛ LOD,
// который ждёт движок. Прямой пропорции нет: уровни LOD лежат в самих данных .grs и дико
// нелинейны — у Сент-Мартина 718489/150006/74726/37099 травинок, то есть 100/21/10/5%. Внутри
// уровня движок не выбрасывает лишние травинки, а гасит их размером, и площадь падает как квадрат
// множителя — отсюда корень в формуле. Соотношения уровней хардкодные: карты травы генерились
// одним инструментом, у других островов пропорции те же с точностью до пары процентов.
float GrassLodFromAmount(float v)
{
	float b;
	if (v >= 1.0) return 0.0;   // максимум: пола нет, работает только дистанционный LOD
	if (v <= 0.0) return 1.0;   // ноль: контракт «трава выключена»
	if (v >= 0.209)
	{
		b = sqrt((v - 0.209) / 0.791);
		return (1.0 - b) / 3.0;
	}
	if (v >= 0.104)
	{
		b = sqrt((v - 0.104) / 0.105);
		return (2.0 - b) / 3.0;
	}
	if (v >= 0.052)
	{
		b = sqrt((v - 0.052) / 0.052);
		return (3.0 - b) / 3.0;
	}
	return 0.995; // ниже LOD3 данных нет — это минимум, дальше только полное выключение
}

// Gregg: настройки «Растительность на островах» из меню графики -> движок (MSG_GRASS_SET_TREES).
// Зовётся при создании травы и при выходе из настроек, так что применяется без перезахода.
// Значения по умолчанию совпадают с дефолтами engine.ini [Grass], поэтому у игрока, ни разу
// не заходившего в это подменю, поведение не меняется.
void ApplyIslandVegetation()
{

	float fMode = 1.0;    // 0 спрайты, 1 смешанная, 2 только модели
	float fBand = 32.0;   // TreeMaxY, м мира: полоса замены (склон)
	float fMinDist = 5.0; // TreeMinDist, м: минимальный шаг между деревьями = густота (средняя)
	float fDist = 0.0;
	float fLod = 0.0;
	int iBand = 1;
	int iDens = 1;
	if (!IsEntity(&objGrass)) return;
	if (CheckAttribute(&InterfaceStates, "IslandVegType"))
	{
		fMode = stf(InterfaceStates.IslandVegType);
	}
	if (CheckAttribute(&InterfaceStates, "IslandVegBand"))
	{
		iBand = sti(InterfaceStates.IslandVegBand);
		if (iBand == 0) fBand = 16.0;  // пляж
		if (iBand == 2) fBand = 64.0;  // высоко
		if (iBand == 3) fBand = 96.0;  // очень высоко
		if (iBand == 4) fBand = 128.0; // весь остров
	}
	if (CheckAttribute(&InterfaceStates, "IslandVegDensity"))
	{
		iDens = sti(InterfaceStates.IslandVegDensity);
		if (iDens == 0) fMinDist = 8.0;
		if (iDens == 1) fMinDist = 5.0;
	}
	SendMessage(objGrass, "lfff", 43666, fMode, fBand, fMinDist); // MSG_GRASS_SET_TREES

	if (!bSeaActive) return;
	if (!CheckAttribute(&InterfaceStates, "LastGrass.scale")) return;
	if (!CheckAttribute(&InterfaceStates, "GrassDistanceSea")) return;
	if (!CheckAttribute(&InterfaceStates, "GrassCountSea")) return;
	fDist = stf(InterfaceStates.GrassDistanceSea) * 4000.0;
	fLod = GrassLodFromAmount(stf(InterfaceStates.GrassCountSea));
	SendMessage(objGrass, "lffffff", 42666, stf(InterfaceStates.LastGrass.scale), stf(InterfaceStates.LastGrass.w), stf(InterfaceStates.LastGrass.h), fDist / 10.0, fDist, fLod); // MSG_GRASS_SET_PARAM
}

void DeleteGrass()
{
	if (IsEntity(&objGrass))
	{
		DeleteClass(&objGrass);
	}
	DeleteEntitiesByType("Grass");
}

void HideGrass()
{
	if (IsEntity(&objGrass))
	{
        LayerDelObject(EXECUTE, &objGrass);
		LayerDelObject(REALIZE, &objGrass);
		LayerDelObject(SEA_EXECUTE, &objGrass);
		LayerDelObject(SEA_REALIZE, &objGrass);
	}
}

void ShowGrass()
{
	if (IsEntity(&objGrass))
	{
        LayerAddObject(SEA_EXECUTE, &objGrass, 1000);
		LayerAddObject(SEA_REALIZE, &objGrass, 1000);
	}
}

void CalcLocalTime(float _inc)
{
	if (locTmpTime >= _inc)
	{
        locTmpTime = 0.0;
    }
}
void RefreshWeather()
{
    Whr_UpdateWeather();
}
void RefreshLandTime()
{
    objLandInterface.textinfo.datatext.text = XI_convertString("Date:") + GetQuestBookData();
}

string stripblank(string str)
{
	string retstr = "";
	int iMax = strlen(str);
	for(int i = 0; i < iMax; i++) {
		if(GetSymbol(str,i) != " ") { retstr += GetSymbol(str,i); }}
	return retstr;
}

void ResetTimeScale()
{
	// LEO: Смена оформления не меняет множитель, счётчик и TimeSpeed; обычные сбросы остаются штатными.
	if(bNewHUDRebuild) return;
	bNewHUDRestoreTime=false;
    SetTimeScale(1.0);
	TimeScaleCounter = 0;
	if(IsPerkIntoList("TimeSpeed"))
	{	DelPerkFromActiveList("TimeSpeed");
	}
}

void FreeChangeFlagMode(string _tmp)
{
	DeleteAttribute(pchar, "DisableChangeFlagMode");
}

string  xiStr(string _str) // просто сокращение
{
	return XI_ConvertString(_str);
}

void Dev_Trace(string _str) // логи только в дев версии
{
    if (MOD_BETTATESTMODE	==	"On")	Trace(_str);
}

String FindStringAfterChar(string _string, string _char)
{
	int i = FindSubStr(_string, _char , 0);
	if(i == -1) return "";
	string sRetStr = strcut(_string, i+1, strlen(_string)-1);
	return sRetStr;
}

String FindStringBeforeChar(string _string, string _char) // Поиск строки перед символом
{
	int i = FindSubStr(_string, _char , 0);
	if(i == -1) return "";
	string sRetStr = strcut(_string, 0, i-1);
	return sRetStr;
}

float retMin(float x, float y)
{
	x = makefloat(x); y = makefloat(y);
	if(x<y) return x;
	return y;
}

float retMax(float x, float y)
{
	if(x>y) return makefloat(x);
	return makefloat(y);
}

int iretMin(int x, int y)
{
	if(x<y) return x;
	return y;
}

int iretMax(int x, int y)
{
	if(x>y) return x;
	return y;
}
// Просто сокращения
void Log_AStr(string _str) // Постоянная строка. С экрана никуда не изчезнет
{
	Log_SetEternalString(_str);
}

void Log_TestAStr(string _str)
{
	if (bBettaTestMode)
	{
		Log_AStr(_str);
		trace("TestInfo: " + GetQuestBookDataDigit() + " " + _str)
	}
}

void Log_Clear()
{
	ClearAllLogStrings();
}
// Строка в 10 пробелов. Пригодится
string TenSpaceStr()
{
	string sTemp = XI_ConvertString("TenSpace");
	return sTemp;
}

int round(float num)
{
	return makeint(num + 0.5);
}

void TraceAndLog(string ltext) //to log _and_ trace info
{
	Log_SetStringToLog(ltext);
	trace(ltext);
}

float pow2(float num, float exp) // supports exponents of less than one via sqrt and a loop.
{
	if(exp == 1.0) return num; // NK 05-03-30 add this check
	if(exp > 1.0) return pow(num, exp);
	int sqrtx = 0;
	while(exp < 1.0)
	{
		exp *= 2;
		sqrtx++;
	}
	num = pow(num, exp);
	for(int i = 0; i < sqrtx; i++)
	{
		num = sqrt(num);
	}
	return num;
}

int GetCurrentModelrNumber()
{
	int n = 0;
	aref arModel;
	if (FindEntity(&arModel, "modelr"))
	{
		n++;
		while (FindEntityNext(&arModel))
		{
			n++;
		}
	}
	return n;
}

bool HasSubStr(string basestr, string findstr)
{
	return findSubStr(basestr, findstr, 0) != -1;
}

string strreplace(string strSource, string from, string into)
{
	int pos = findSubStr(strSource, from, 0);
	if (pos < 0) return strSource;
	if (HasSubStr(into, from)) return strSource;
	string ret = "";
	if (pos > 0) ret = strcut(strSource, 0, pos - 1);
	ret += into;
	if (pos + strlen(from) <= strlen(strSource) - 1) ret += strcut(strSource, pos + strlen(from), strlen(strSource) - 1);
	return strreplace(ret, from, into);
}

string DigitsToString(int value, int digits)
{
	int v = value;
	string result = "";
	for (int i = 0; i < digits; i++)
	{
		result = "" + (v % 10) + result;
		v = v / 10;
	}
	return result;
}

void RandomShuffle(aref data)
{
	int data_count = GetAttributesNum(data);
	int j;
	string temp, attr_i, attr_j;
	for (int i = data_count - 1; i > 0; i--)
	{
		j = rand(i);
		attr_i = "s" + i;
		attr_j = "s" + j;
		temp = data.(attr_j);
		data.(attr_j) = data.(attr_i);
		data.(attr_i) = temp;
	}
}
