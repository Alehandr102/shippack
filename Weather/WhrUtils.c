

int Whr_GetColor(aref arRef, string sAttribute)
{
	if (!CheckAttribute(&arRef, sAttribute))
	{

		trace("ПОГОДА: нет цвета '" + sAttribute + "' в блоке '" + GetAttributeName(arRef) + "', текущий пресет №" + iCurWeatherNum);
		return argb(0, 0, 0, 0);
	}
	return sti(arRef.(sAttribute));
}

float Whr_GetFloat(aref arRef, string sAttribute)
{
	string sTmpAttr = sAttribute + ".max";
	if (CheckAttribute(arRef,sTmpAttr))
	{
		float fMin = stf(arRef.(sAttribute).min);
		float fMax = stf(arRef.(sAttribute).max);
		return fMin + frnd() * (fMax - fMin);
	}

	if (!CheckAttribute(&arRef, sAttribute)) return 0.0;

	return stf(arRef.(sAttribute));
}

int Whr_GetLong(aref arRef, string sAttribute)
{
	string sTmpAttr = sAttribute + ".max";
	if (CheckAttribute(arRef,sTmpAttr))
	{
		int iMin = sti(arRef.(sAttribute).min);
		int iMax = sti(arRef.(sAttribute).max);
		return iMin + rand(iMax - iMin - 1);
	}
	if (CheckAttribute(arRef, sAttribute)) return sti(arRef.(sAttribute));
	else return 0;
}

string Whr_GetString(aref arRef, string sAttribute)
{
	string result = "none";
	if (CheckAttribute(arRef, sAttribute)) result = arRef.(sAttribute);
	else trace("ERROR: not found attribute '" + sAttribute + "' for Whr_GetString");
	return result;
}

void eChangeDayNight()
{

}
