// Gregg: отчёт казначея о рейде по городу.
// Данные собирает Treasurer_PortRun (store\StoreUtilite.c) в pchar.TreasurerReport, экран их только показывает.
// Левая таблица - что и кому закуплено (товары в трюм тоже), правая - редкий товар у лавочника и лоточников.

int tr_curBuyRow = 0;

// LEO: число с ЯВНЫМ знаком. Минус несёт само значение, плюс дописываем - чтобы приход и расход
//   читались с одного взгляда, а не по цвету и не по памяти о том, что значит знак в этой колонке.
string TR_Signed(int iVal)
{
	if (iVal > 0) return "+" + iVal;
	return "" + iVal;
}

// LEO: ЕДИНСТВЕННОЕ место, где крутится цвет знака - его берут и ячейки таблицы (TR_SignColor),
//   и строка "Итого" (TR_SignTag). Держим СВЕТЛУЮ пастель: тёмные насыщенные тона на этом экране
//   читаются тяжело. Крутить только "гасящие" каналы (у плюса R и B, у минуса G и B), ведущий - 255:
//   196 блёкло, ниже ~120 уже резко. ⚠️ Фоны разные (таблица на пергаменте, "Итого" на тёмной полосе) -
//   понадобится развести, заведи второй набор для тега.
#define TR_PLUS_R	150
#define TR_PLUS_G	255
#define TR_PLUS_B	150
#define TR_MINUS_R	255
#define TR_MINUS_G	150
#define TR_MINUS_B	150

// LEO: цвет по знаку - приход зелёным, расход красным. Для ячеек TABLE (атрибут .color).
int TR_SignColor(int iVal)
{
	if (iVal > 0) return argb(255, TR_PLUS_R, TR_PLUS_G, TR_PLUS_B);
	return argb(255, TR_MINUS_R, TR_MINUS_G, TR_MINUS_B);
}

// LEO: тот же цвет, но тегом - FORMATEDTEXT красит ТОЛЬКО обёрнутый кусок, поэтому и подпись
//   "Итого:", и слово-склонение остаются обычным цветом, а цвет достаётся одному числу.
//   Закрывать обязательно: цвет тянется через переносы строк до </color>.
string TR_SignTag(int iVal)
{
	if (iVal > 0) return "<color=255," + TR_PLUS_R + "," + TR_PLUS_G + "," + TR_PLUS_B + ">";
	return "<color=255," + TR_MINUS_R + "," + TR_MINUS_G + "," + TR_MINUS_B + ">";
}

void TR_FillBuyTable()
{
	Table_Clear("BUY_TABLE", false, true, false);
	GameInterface.BUY_TABLE.hr.td1.str = GetConvertStrWithReplace("Variable_treasurer_rep_col_item", "Interface.txt", "#space#", " ");
	GameInterface.BUY_TABLE.hr.td2.str = GetConvertStrWithReplace("Variable_treasurer_rep_col_qty", "Interface.txt", "#space#", " ");
	GameInterface.BUY_TABLE.hr.td3.str = GetConvertStrWithReplace("Variable_treasurer_rep_col_sum", "Interface.txt", "#space#", " ");
	GameInterface.BUY_TABLE.hr.td4.str = GetConvertStrWithReplace("Variable_treasurer_rep_col_who", "Interface.txt", "#space#", " ");
	GameInterface.BUY_TABLE.hr.td1.scale = 0.8;
	GameInterface.BUY_TABLE.hr.td2.scale = 0.8;
	GameInterface.BUY_TABLE.hr.td3.scale = 0.8;
	GameInterface.BUY_TABLE.hr.td4.scale = 0.8;

	int iRows = 0;
	if (CheckAttribute(pchar, "TreasurerReport.rows")) iRows = sti(pchar.TreasurerReport.rows);
	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	string sSrc, sRow, sItem;
	int i, n, iOwner, iQty, iCost;
	ref rItem;
	n = 1;
	for (i = 0; i < iRows; i++)
	{
		sSrc = "r" + i;
		sRow = "tr" + n;
		sItem = pchar.TreasurerReport.buy.(sSrc).item;
		iQty = sti(pchar.TreasurerReport.buy.(sSrc).qty);
		iOwner = sti(pchar.TreasurerReport.buy.(sSrc).owner);

		if (CheckAttribute(pchar, "TreasurerReport.buy." + sSrc + ".goods")) // товар в трюм
		{
			GameInterface.BUY_TABLE.(sRow).td1.icon.group = "GOODS";
			GameInterface.BUY_TABLE.(sRow).td1.icon.image = sItem;
			GameInterface.BUY_TABLE.(sRow).td1.str = XI_ConvertString(sItem);
		}
		else
		{
			rItem = ItemsFromID(sItem);
			GameInterface.BUY_TABLE.(sRow).td1.icon.group = rItem.picTexture;
			GameInterface.BUY_TABLE.(sRow).td1.icon.image = "itm" + rItem.picIndex;
			GameInterface.BUY_TABLE.(sRow).td1.str = LanguageConvertString(idLngFile, rItem.name);
		}
		GameInterface.BUY_TABLE.(sRow).td1.icon.offset = "-2, 0";
		GameInterface.BUY_TABLE.(sRow).td1.icon.width = 38;
		GameInterface.BUY_TABLE.(sRow).td1.icon.height = 38;
		GameInterface.BUY_TABLE.(sRow).td1.textoffset = "38,0";
		GameInterface.BUY_TABLE.(sRow).td1.align = "left";
		GameInterface.BUY_TABLE.(sRow).td1.scale = 0.8;

		// LEO: количество и сумма со знаком и цветом. Куплено / выручено - зелёным с плюсом,
		//   продано / потрачено - красным с минусом. РОВНО НОЛЬ не красим и знака не ставим:
		//   такие строки (список задан, но брать было нечего) должны оставаться белыми.
		iCost = sti(pchar.TreasurerReport.buy.(sSrc).cost);
		GameInterface.BUY_TABLE.(sRow).td2.str = TR_Signed(iQty);
		GameInterface.BUY_TABLE.(sRow).td2.scale = 0.8;
		GameInterface.BUY_TABLE.(sRow).td3.str = TR_Signed(iCost);
		GameInterface.BUY_TABLE.(sRow).td3.scale = 0.8;
		if (iQty != 0) GameInterface.BUY_TABLE.(sRow).td2.color = TR_SignColor(iQty);
		if (iCost != 0) GameInterface.BUY_TABLE.(sRow).td3.color = TR_SignColor(iCost);

		GameInterface.BUY_TABLE.(sRow).td4.str = TR_OwnerName(iOwner, false);
		GameInterface.BUY_TABLE.(sRow).td4.scale = 0.8;
		GameInterface.BUY_TABLE.(sRow).owner = iOwner;
		n++;
	}
	LanguageCloseFile(idLngFile);
	GameInterface.BUY_TABLE.select = 0;
	Table_UpdateWindow("BUY_TABLE");
}

// занимает ли персонаж указанный офицерский пост у ГГ (ветки может не быть вовсе)
bool TR_IsPost(int iOwner, string sPost)
{
	if (!CheckAttribute(pchar, "Fellows.Passengers." + sPost)) return false;
	return sti(pchar.Fellows.Passengers.(sPost)) == iOwner;
}

// должность персонажа словами (значки-символы шрифта в ячейках таблицы не рисуются - выходил пустой квадрат)
string TR_OwnerPost(int iOwner)
{
	if (TR_IsPost(iOwner, "treasurer")) return XI_ConvertString("treasurer");
	if (TR_IsPost(iOwner, "doctor")) return XI_ConvertString("doctor");
	if (TR_IsPost(iOwner, "carpenter")) return XI_ConvertString("carpenter");
	if (TR_IsPost(iOwner, "navigator")) return XI_ConvertString("navigator");
	if (TR_IsPost(iOwner, "boatswain")) return XI_ConvertString("boatswain");
	if (TR_IsPost(iOwner, "cannoner")) return XI_ConvertString("cannoner");
	if (isOfficer(&Characters[iOwner])) return XI_ConvertString("fighter");
	return "";
}

// имя получателя. bFull - подпись под портретом: имя + должность либо корабль
string TR_OwnerName(int iOwner, bool bFull)
{
	if (iOwner == REPORT_OWNER_CHEST) return GetConvertStrWithReplace("Variable_treasurer_rep_chest", "Interface.txt", "#space#", " ");
	if (iOwner < 0) return "";
	ref chr = &Characters[iOwner];
	if (!bFull) return GetFullName(chr);

	if (sti(chr.Ship.Type) != SHIP_NOTUSED) // капитан - подписываем кораблём
	{
		return GetFullName(chr) + "\n" + XI_ConvertString(RealShips[sti(chr.Ship.Type)].BaseName) + " '" + chr.Ship.Name + "'";
	}
	string sPost = TR_OwnerPost(iOwner);
	if (sPost == "") return GetFullName(chr);
	return GetFullName(chr) + "\n" + sPost;
}

void TR_FillRareTable()
{
	Table_Clear("RARE_TABLE", false, true, false);
	GameInterface.RARE_TABLE.hr.td1.str = GetConvertStrWithReplace("Variable_treasurer_rep_col_item", "Interface.txt", "#space#", " ");
	GameInterface.RARE_TABLE.hr.td2.str = GetConvertStrWithReplace("Variable_treasurer_rep_col_where", "Interface.txt", "#space#", " ");
	GameInterface.RARE_TABLE.hr.td3.str = GetConvertStrWithReplace("Variable_treasurer_rep_col_price", "Interface.txt", "#space#", " ");
	GameInterface.RARE_TABLE.hr.td1.scale = 0.8;
	GameInterface.RARE_TABLE.hr.td2.scale = 0.8;
	GameInterface.RARE_TABLE.hr.td3.scale = 0.8;

	int iRows = 0;
	if (CheckAttribute(pchar, "TreasurerReport.rare_n")) iRows = sti(pchar.TreasurerReport.rare_n);
	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	string sSrc, sRow, sItem, sName;
	int i, n, iQty;
	ref rItem;
	n = 1;
	for (i = 0; i < iRows; i++)
	{
		sSrc = "r" + i;
		sRow = "tr" + n;
		sItem = pchar.TreasurerReport.rare.(sSrc).item;
		rItem = ItemsFromID(sItem);
		iQty = sti(pchar.TreasurerReport.rare.(sSrc).qty);

		sName = LanguageConvertString(idLngFile, rItem.name);
		if (iQty > 1) sName = sName + " x" + iQty;
		if (CheckAttribute(pchar, "TreasurerReport.rare." + sSrc + ".dmg")) // генерённое оружие - показываем урон и вес
		{
			sName = sName + "\n" + pchar.TreasurerReport.rare.(sSrc).dmg + " / " + pchar.TreasurerReport.rare.(sSrc).weight;
		}

		GameInterface.RARE_TABLE.(sRow).td1.icon.group = rItem.picTexture;
		GameInterface.RARE_TABLE.(sRow).td1.icon.image = "itm" + rItem.picIndex;
		GameInterface.RARE_TABLE.(sRow).td1.icon.offset = "-2, 0";
		GameInterface.RARE_TABLE.(sRow).td1.icon.width = 38;
		GameInterface.RARE_TABLE.(sRow).td1.icon.height = 38;
		GameInterface.RARE_TABLE.(sRow).td1.textoffset = "38,0";
		GameInterface.RARE_TABLE.(sRow).td1.align = "left";
		GameInterface.RARE_TABLE.(sRow).td1.str = sName;
		GameInterface.RARE_TABLE.(sRow).td1.scale = 0.8;
		GameInterface.RARE_TABLE.(sRow).td2.str = pchar.TreasurerReport.rare.(sSrc).where;
		GameInterface.RARE_TABLE.(sRow).td2.scale = 0.8;
		GameInterface.RARE_TABLE.(sRow).td3.str = pchar.TreasurerReport.rare.(sSrc).price;
		GameInterface.RARE_TABLE.(sRow).td3.scale = 0.8;
		n++;
	}
	LanguageCloseFile(idLngFile);
	GameInterface.RARE_TABLE.select = 0;
	Table_UpdateWindow("RARE_TABLE");
}

// портрет и подпись получателя по выбранной строке закупок
void TR_ShowOwner(int iOwner)
{
	if (iOwner < 0)
	{
		SetNewPicture("OWNER_PICTURE", "interfaces\BoxImage.dds");
		SetFormatedText("OWNER_NAME", TR_OwnerName(iOwner, true));
		return;
	}
	ref chr = &Characters[iOwner];
	if (CheckAttribute(chr, "FaceId")) SetNewPicture("OWNER_PICTURE", "interfaces\portraits\face_" + chr.FaceId + ".tga");
	else SetNewPicture("OWNER_PICTURE", "interfaces\BoxImage.dds");
	SetFormatedText("OWNER_NAME", TR_OwnerName(iOwner, true));
}

void InitInterface(string iniName)
{
	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);

	string sCity = "";
	if (CheckAttribute(pchar, "TreasurerReport.city")) sCity = GetCityName(pchar.TreasurerReport.city);
	SetFormatedText("REPORT_TITLE", GetConvertStrWithReplace("Variable_treasurer_rep_title", "Interface.txt", "#space#", " ") + sCity);
	SetFormatedText("BUY_CAPTION", GetConvertStrWithReplace("Variable_treasurer_rep_buy", "Interface.txt", "#space#", " "));
	SetFormatedText("RARE_CAPTION", GetConvertStrWithReplace("Variable_treasurer_rep_rare", "Interface.txt", "#space#", " "));

	int iMoney = 0;
	if (CheckAttribute(pchar, "TreasurerReport.money")) iMoney = sti(pchar.TreasurerReport.money);
	// LEO: красим ТОЛЬКО число со знаком, слово-склонение остаётся обычным. FindRussianMoneyString собирает
	//   строку как MakeMoneyShow + " " + слово, поэтому число берём тем же вызовом, а хвост отрезаем по его
	//   длине (склонение считаем по модулю, знак дописываем сами). strcut режет БАЙТЫ включительно, но
	//   граница идёт по числу - цифры и пробелы однобайтовые, кириллица уходит в хвост целиком.
	string sNum = MakeMoneyShow(abs(iMoney), MONEY_SIGN, MONEY_DELIVER);
	string sTotal = FindRussianMoneyString(abs(iMoney));
	string sWord = "";
	if (strlen(sTotal) > strlen(sNum)) sWord = strcut(sTotal, strlen(sNum), strlen(sTotal) - 1);
	if (iMoney != 0) // ноль - без знака и без цвета, как и в таблице закупок
	{
		if (iMoney > 0) sNum = "+" + sNum;
		else sNum = "-" + sNum;
		sNum = TR_SignTag(iMoney) + sNum + "</color>";
	}
	SetFormatedText("REPORT_TOTAL", GetConvertStrWithReplace("Variable_treasurer_rep_total", "Interface.txt", "#space#", " ") + sNum + sWord);

	TR_FillBuyTable();
	TR_FillRareTable();
	TR_ShowOwner(TR_RowOwner(1)); // портрет владельца первой строки

	GameInterface.title = "titleTreasurerReport";

	SetEventHandler("exitCancel", "ProcessCancelExit", 0);
	SetEventHandler("evntDoPostExit", "DoPostExit", 0);
	SetEventHandler("ievnt_command", "ProcCommand", 0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
}

// Индекс получателя по номеру выбранной строки. Движок отдаёт номер строки КАК ЕСТЬ, с единицы
// (tr1 - первая) - так же читает выбор goods_spawn.c. Был лишний +1: клик по строке брал следующую,
// а на последней возвращал -1 - отсюда и пропадавший портрет.
int TR_RowOwner(int iSelected)
{
	string sRow = "tr" + iSelected;
	if (!CheckAttribute(&GameInterface, "BUY_TABLE." + sRow + ".owner")) return -1;
	return sti(GameInterface.BUY_TABLE.(sRow).owner);
}

void IDoExit(int exitCode)
{
	DelEventHandler("exitCancel", "ProcessCancelExit");
	DelEventHandler("evntDoPostExit", "DoPostExit");
	DelEventHandler("ievnt_command", "ProcCommand");
	DelEventHandler("TableSelectChange", "TableSelectChange");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcessCancelExit()
{
	IDoExit(RC_INTERFACE_QUICK_SAVE);
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();
}

void TableSelectChange()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();
	if (sControl != "BUY_TABLE") return;
	tr_curBuyRow = iSelected;
	TR_ShowOwner(TR_RowOwner(iSelected));
}
