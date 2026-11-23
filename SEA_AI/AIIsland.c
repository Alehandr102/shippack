void Island_Start()
{
	SendMessage(&Island, "l", MSG_ISLAND_START);
}

void Island_Reload(string sIslandID, string sNewIslandModel)
{
	int iIslandID = FindIsland(sIslandID);
	if (iIslandID < 0)
	{
		return;
	}
	ref rIsland = GetIslandByIndex(iIslandID);
	rIsland.model = sNewIslandModel;
	if (bSeaActive)
	{
		SendMessage(&Island, "lss", MSG_ISLAND_LOAD_GEO, "islands", sNewIslandModel);
	}
}

//navy -->
string Island_FindNearestLocator(string group, float x, float z)
{
	ref rIsland;
	aref arGroup, arLocator;
	string retStr = "";
	float fMin = 4000;
	float fDist;
	int i, iNum;
	int iIslandID = FindIsland(PChar.location);
	if (iIslandID < 0)
	{
		return retStr;
	}

	makearef(arGroup, Islands[iIslandID].(group));
	iNum = GetAttributesNum(arGroup);
	if (iNum < 1) return retStr;

	for (i = 0; i < iNum; i++)
	{
		arLocator = GetAttributeN(arGroup, i);
		fDist = GetDistance2D(x, z, stf(arLocator.x), stf(arLocator.z));
		if (fDist < fMin)
		{
			fMin = fDist;
			retStr = GetAttributeName(arLocator);
		}
	}
	return retStr;
}
string Island_FindNearestLocator2PChar(string group)
{
	return Island_FindNearestLocator(group, stf(pchar.Ship.Pos.x), stf(pchar.Ship.Pos.z))
}
//navy <--

// LEO: выгрузка карты острова в PNG - исходник для карт атласа. Файлы кладутся в
//   Docs_AND_Report\Screenshots, имя берётся по острову. Звать, находясь В МОРЕ.
//   nSize - сторона снимка в пикселях (4096 обычно хватает, 8192 = 256 МБ видеопамяти разово).
//   fRadius - радиус кадра в метрах вокруг точки; 0 = весь остров по габаритам модели.
//   sTech - техника отрисовки. ПУСТАЯ строка = родной материал острова (рельеф видно),
//   "battle_island_gettexture" = прежний плоский диффуз. Своя техника из .fx - рантайм-ресурс,
//   пробовать можно без пересборки движка.
//   fSunAzim/fSunElev - откуда и под каким углом светит солнце, градусы. Именно оно даёт рельеф:
//   лайтмапов у морских островов нет. fSunElev = 0 гасит свет и возвращает плоскую картинку.
//   fAmbient - подсветка теней 0..1: чем меньше, тем контрастнее горы.
void Island_DumpMap(int nSize, float fCenterX, float fCenterZ, float fRadius, string sTech, float fSunAzim, float fSunElev, float fAmbient)
{
	if(IsEntity(&Island) == 0)
	{
		Log_Info("Карта: остров не загружен, нужно быть в море");
		return;
	}
	// "" - имя файла движок берёт по острову, 1 - прозрачный фон вместо цвета моря
	SendMessage(&Island, "llslfffsfff", MSG_ISLAND_DUMP_MAP, nSize, "", 1, fCenterX, fCenterZ, fRadius, sTech, fSunAzim, fSunElev, fAmbient);
}

void DumpIslandMap(int nSize, string sTech, float fSunAzim, float fSunElev, float fAmbient)
{
	Island_DumpMap(nSize, 0.0, 0.0, 0.0, sTech, fSunAzim, fSunElev, fAmbient);
}

// LEO: снимок вокруг корабля ГГ - в одной модели острова часто лежит несколько земель, и полный
//   снимок захватывает соседей, нужный кусок выходит мелким. Файл тот же, но с подчёркиванием
//   на конце: Antigua_.png
void DumpIslandMapAtShip(int nSize, float fRadius, string sTech, float fSunAzim, float fSunElev, float fAmbient)
{
	Island_DumpMap(nSize, stf(pchar.Ship.Pos.x), stf(pchar.Ship.Pos.z), fRadius, sTech, fSunAzim, fSunElev, fAmbient);
}
