void CharacterExitFromLocation(ref character)
{
	SendMessage(character, "l", MSG_CHARACTER_EXIT_FROM_LOCATION);
}

bool SetCharacterTask_None(ref character)
{
	return SendMessage(character, "ls", MSG_NPCHARACTER_SETTASK, "None");
}

bool SetCharacterTask_Stay(ref character)
{
	return SendMessage(character, "ls", MSG_NPCHARACTER_SETTASK, "Stay");
}

bool SetCharacterTask_GotoPoint(ref character, string locatorGroup, string locatorName)
{
	float x, y, z;
	//Get current location
	if(IsEntity(loadedLocation) == 0)
	{
		Trace("SetCharacterTask_GotoPoint -> location not created");
		return false;
	}
	//Check locator by create
	string slocator = "locators." + locatorGroup + "." + locatorName;
	if(CheckAttribute(loadedLocation, slocator) == 0)
	{
		if (locatorName != "")
		{
			Trace("SetCharacterTask_GotoPoint -> locator '" + locatorName + "' in group '" + locatorGroup + "' not found");
		}
		return false;
	}
	//Get locator position
	aref locator;
	MakeARef(locator, loadedLocation.(slocator));
	x = MakeFloat(locator.x);
	y = MakeFloat(locator.y);
	z = MakeFloat(locator.z);
	//Set task
	return SendMessage(character, "lsfff", MSG_NPCHARACTER_SETTASK, "Goto point", x, y, z);
}

// Gregg: приказ идти в ПРОИЗВОЛЬНУЮ мировую координату (движковый goto уже координатный, локатор не нужен).
// Клампинг к патчу ходьбы и поиск пути делает движок (CmdGotoPoint -> FindNodeIndex/FindPathDir).
bool SetCharacterTask_GotoCoord(ref character, float x, float y, float z)
{
	return SendMessage(character, "lsfff", MSG_NPCHARACTER_SETTASK, "Goto point", x, y, z);
}

// Gregg: как GotoCoord, но БЕГОМ (движковый "Runto point" -> SetRunMode(true)).
bool SetCharacterTask_RuntoCoord(ref character, float x, float y, float z)
{
	return SendMessage(character, "lsfff", MSG_NPCHARACTER_SETTASK, "Runto point", x, y, z);
}

// Gregg: ТЕСТОВЫЙ спайк наземной тактики (дебаг-клавиша NUMPAD5). Проверяет три ядра:
//  1) центр экрана -> луч -> точка земли (движок TacPick) + попадание в патч ходьбы;
//  2) красный тинт ближайшего врага + зелёный тинт офицера №1 (движок SetTacTint);
//  3) приказ офицеру идти в найденную точку (SetCharacterTask_GotoCoord).
// Диагностика в compile.log через Trace(). Тинт снимается повторным вызовом с 0 (см. LandTacPickClear).
void LandTacPickTest()
{
	ref pchar = GetMainCharacter();
	float x = 0.0; float y = 0.0; float z = 0.0; float status = 0.0;
	SendMessage(loadedLocation, "lseeee", MSG_LOCATION_EX_MSG, "TacPick", &x, &y, &z, &status);
	Trace("LandTacPick: status=" + status + " pos=(" + x + ", " + y + ", " + z + ")");
	if (status < 0.5)
	{
		Trace("LandTacPick: нет земли под центром экрана");
		return;
	}
	// красный тинт ближайшего врага
	int enemy = LAi_group_GetTarget(pchar);
	if (enemy >= 0)
	{
		SendMessage(&Characters[enemy], "lsfl", MSG_CHARACTER_EX_MSG, "SetTacTint", 1.0, 0);
		Trace("LandTacPick: враг " + enemy + " -> красный тинт");
	}
	// зелёный тинт офицера №1 + приказ идти в точку
	int off = GetOfficersIndex(pchar, 1);
	if (off >= 0)
	{
		SendMessage(&Characters[off], "lsfl", MSG_CHARACTER_EX_MSG, "SetTacTint", 1.0, 1);
		if (status > 1.5)
		{
			SetCharacterTask_GotoCoord(&Characters[off], x, y, z);
			Trace("LandTacPick: офицер " + off + " -> goto точка (в навмеше)");
		}
		else
		{
			Trace("LandTacPick: точка ВНЕ навмеша, goto пропущен");
		}
	}
}

// Gregg: наземный тактический режим (этап A) — переключатель по F8: пауза мира + курсор мыши.
void LandTacToggle()
{
	ref pchar = GetMainCharacter();
	if (CheckAttribute(pchar, "LandTacOn"))
	{
		DeleteAttribute(pchar, "LandTacOn");
		SendMessage(loadedLocation, "lsl", MSG_LOCATION_EX_MSG, "TacMode", 0);
		SendMessage(loadedLocation, "ll", MSG_LOCATION_PAUSE, false); // разморозить позиции + камеру
		SetTimeScale(1.0);                                            // вернуть темп (анимации)
		DeleteAttribute(pchar, "pause");
		dialogDisable = false;                                        // вернуть диалоги
		LandTac_ClearSel();                                           // снять выделение + зелёную подсветку со всех
		int me;
		for (me = 1; me <= MAX_NUM_FIGHTERS; me++)                    // снять пометку офицеров
		{
			int meo = GetOfficersIndex(pchar, me);
			if (meo >= 0) DeleteAttribute(&Characters[meo], "LandTacOfc");
		}
		Trace("LandTac: режим ВЫКЛ");
	}
	else
	{
		pchar.LandTacOn = "1";
		SendMessage(loadedLocation, "lsl", MSG_LOCATION_EX_MSG, "TacMode", 1);
		SendMessage(loadedLocation, "ll", MSG_LOCATION_PAUSE, true); // камера стоит (мышь->курсор) + позиции
		pchar.pause = true;
		SetTimeScale(0.0); // полный стоп времени -> замирают и АНИМАЦИИ (иначе бег на месте)
		dialogDisable = true; // не начинать диалог с ближним NPC (сбивал бы паузу)
		int mi;
		for (mi = 1; mi <= MAX_NUM_FIGHTERS; mi++) // пометить моих офицеров (движку: наведение зелёным + рамка), БЕЗ лимита на количество
		{
			int moi = GetOfficersIndex(pchar, mi);
			if (moi >= 0) Characters[moi].LandTacOfc = "1";
		}
		LandTac_ClearSel(); // Gregg: по умолчанию НИКТО не выделен — игрок выбирает ЛКМ/рамкой
		SendMessage(loadedLocation, "lsss", MSG_LOCATION_EX_MSG, "TacHint", "Тактическая пауза", "ЛКМ - выбор (рамкой - неск.)   ПКМ - приказ   СКМ - камера   Y - выход");
		Trace("LandTac: режим ВКЛ");
	}
}

// Индекс idx — один из моих офицеров?
bool LandTac_IsMyOfficer(int idx)
{
	if (idx < 0) return false;
	ref pchar = GetMainCharacter();
	int i;
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		if (GetOfficersIndex(pchar, i) == idx) return true;
	}
	return false;
}

// Снять выделение со ВСЕХ офицеров (атрибут LandTacSel + зелёный тинт). Движок хавер-тинт смотрит на LandTacSel.
void LandTac_ClearSel()
{
	ref pchar = GetMainCharacter();
	int i;
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		int oi = GetOfficersIndex(pchar, i);
		if (oi < 0) continue;
		if (CheckAttribute(&Characters[oi], "LandTacSel"))
		{
			DeleteAttribute(&Characters[oi], "LandTacSel");
			SendMessage(&Characters[oi], "lsfl", MSG_CHARACTER_EX_MSG, "SetTacTint", 0.0, 0);
		}
	}
}

// Добавить офицера в выделение (зелёный тинт).
void LandTac_AddSel(int idx)
{
	if (idx < 0) return;
	Characters[idx].LandTacSel = "1";
	SendMessage(&Characters[idx], "lsfl", MSG_CHARACTER_EX_MSG, "SetTacTint", 1.0, 1); // зелёный
}

// ЛКМ-клик по офицеру -> выбрать ТОЛЬКО его (сброс остальных); повторный клик по единственному -> снять; мимо -> снять всё.
#event_handler("evntLandTacLSelect", "LandTac_OnSelect");
void LandTac_OnSelect()
{
	int idx = makeint(GetEventData());
	if (LandTac_IsMyOfficer(idx))
	{
		bool wasSel = CheckAttribute(&Characters[idx], "LandTacSel");
		LandTac_ClearSel();
		if (!wasSel)
		{
			LandTac_AddSel(idx);
			Trace("LandTac: выбран офицер " + idx);
		}
		else
		{
			Trace("LandTac: выделение снято");
		}
	}
	else
	{
		LandTac_ClearSel();
		Trace("LandTac: выделение снято (мимо офицера)");
	}
}

// Рамка: движок пометил офицеров в прямоугольнике атрибутом LandTacBoxed; выделяем их (БЕЗ лимита), сброс прежних.
#event_handler("evntLandTacBoxDone", "LandTac_OnBoxDone");
void LandTac_OnBoxDone()
{
	ref pchar = GetMainCharacter();
	LandTac_ClearSel();
	int i, n = 0;
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		int off = GetOfficersIndex(pchar, i);
		if (off < 0) continue;
		if (CheckAttribute(&Characters[off], "LandTacBoxed"))
		{
			LandTac_AddSel(off);
			DeleteAttribute(&Characters[off], "LandTacBoxed");
			n++;
		}
	}
	Trace("LandTac: рамкой выбрано офицеров: " + n);
}

// Отдать команду ОДНОМУ офицеру: атака врага idx / бег в точку (x,y,z). enemy = цель враждебна.
void LandTac_CmdOne(int off, int idx, bool enemy, float x, float y, float z, float onmesh)
{
	if (off < 0) return;
	if (enemy)
	{
		Characters[off].chr_ai.LandTacCmdTarget = idx;            // держать ЭТУ цель (этап D)
		DeleteAttribute(&Characters[off], "chr_ai.LandTacHold");
		LAi_tmpl_SetFight(&Characters[off], &Characters[idx]);
		Characters[off].LTType = "a";                             // линия приказа: атака -> цель
		Characters[off].LTTgt = idx;
		return;
	}
	if (onmesh > 0.5)
	{
		Characters[off].chr_ai.LandTacHold = "1";
		DeleteAttribute(&Characters[off], "chr_ai.LandTacCmdTarget");
		DeleteAttribute(&Characters[off], "chr_ai.tmpl");
		Characters[off].chr_ai.tmpl = "stay";
		SetCharacterTask_RuntoCoord(&Characters[off], x, y, z); // БЕГОМ в точку; по прибытии stay_EndRunToPoint -> стоим
		Characters[off].LTType = "m";                            // линия приказа: движение -> точка
		Characters[off].LTx = x; Characters[off].LTy = y; Characters[off].LTz = z;
	}
}

// ПКМ: команда ВСЕМ выделенным офицерам — враг под курсором -> атака; пустая земля -> идти в точку.
#event_handler("evntLandTacRCmd", "LandTac_OnCmd");
void LandTac_OnCmd()
{
	int idx = makeint(GetEventData());
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	float onmesh = GetEventData();
	ref pchar = GetMainCharacter();
	bool enemy = false;
	if (idx >= 0) enemy = LAi_group_IsEnemy(pchar, &Characters[idx]); // враг относительно ГГ (=офицеров, одна группа)
	if (!enemy && onmesh <= 0.5)
	{
		Trace("LandTac: ПКМ мимо врага и вне навмеша — пропуск");
		return;
	}
	int i, n = 0;
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		int off = GetOfficersIndex(pchar, i);
		if (off < 0) continue;
		if (!CheckAttribute(&Characters[off], "LandTacSel")) continue;
		float ox = 0.0;
		float oz = 0.0;
		if (!enemy) // движение: разводим цели по сетке 3-в-ряд, иначе навмеш отбраковывает скученных (часть стоит)
		{
			int col = n - (n / 3) * 3;
			int row = n / 3;
			ox = (col - 1) * 0.9;
			oz = row * 0.9;
		}
		LandTac_CmdOne(off, idx, enemy, x + ox, y, z + oz, onmesh);
		n++;
	}
	if (n > 0)
	{
		if (enemy) SendMessage(&Characters[idx], "lsfl", MSG_CHARACTER_EX_MSG, "SetTacTint", 1.0, 0); // красный на цель
		PlaySound("interface\ok.wav"); // звук отдачи приказа
		Trace("LandTac: приказ отдан офицерам: " + n);
	}
	else
	{
		Trace("LandTac: нет выделенных офицеров — команда пропущена");
	}
}

// Gregg (этап D): снять командный стейт со ВСЕХ офицеров — вернуть их под обычный ИИ.
// Зовётся из обычных командных клавиш (OfficersFollow/Free/Charge/Hold), чтобы они снова работали.
void LandTac_ReleaseAll()
{
	ref pchar = GetMainCharacter();
	int i;
	for (i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		int oi = GetOfficersIndex(pchar, i);
		if (oi < 0) continue;
		DeleteAttribute(&Characters[oi], "chr_ai.LandTacHold");
		DeleteAttribute(&Characters[oi], "chr_ai.LandTacCmdTarget");
		DeleteAttribute(&Characters[oi], "LTType"); // снять линию приказа
		if (CheckAttribute(&Characters[oi], "LandTacSel"))
		{
			DeleteAttribute(&Characters[oi], "LandTacSel"); // снять выделение
			SendMessage(&Characters[oi], "lsfl", MSG_CHARACTER_EX_MSG, "SetTacTint", 0.0, 0);
		}
	}
}

bool SetCharacterTask_RuntoPoint(ref character, string locatorGroup, string locatorName)
{
	float x, y, z;
	//Get current location
	if(IsEntity(loadedLocation) == 0)
	{
		Trace("SetCharacterTask_GotoPoint -> location not created");
		return false;
	}
	//Check locator by create
	string slocator = "locators." + locatorGroup + "." + locatorName;
	if(CheckAttribute(loadedLocation, slocator) == 0)
	{
		Trace("SetCharacterTask_GotoPoint -> locator '" + locatorName + "' in group '" + locatorGroup + "' not found");
		return false;
	}
	//Get locator position
	aref locator;
	MakeARef(locator, loadedLocation.(slocator));
	x = MakeFloat(locator.x);
	y = MakeFloat(locator.y);
	z = MakeFloat(locator.z);
	//Set task
	return SendMessage(character, "lsfff", MSG_NPCHARACTER_SETTASK, "Runto point", x, y, z);
}

bool SetCharacterTask_FollowCharacter(ref character, ref followCharacter)
{
	return SendMessage(character, "lsi", MSG_NPCHARACTER_SETTASK, "Follow character", followCharacter);
}

bool SetCharacterTask_Fight(ref character, ref targetCharacter)
{
	return SendMessage(character, "lsi", MSG_NPCHARACTER_SETTASK, "Fight", targetCharacter);
}

bool SetCharacterTask_Escape(ref character, ref escapeCharacter)
{
	return SendMessage(character, "lsi", MSG_NPCHARACTER_SETTASK, "Escape", escapeCharacter);
}

bool SetCharacterTask_Dead(ref character)
{
	return SendMessage(character, "ls", MSG_NPCHARACTER_SETTASK, "Dead");
}

bool PushCharacterTask(ref character)
{
	return SendMessage(character, "l", MSG_NPCHARACTER_PUSHTASK);
}

bool PopCharacterTask(ref character)
{
	return SendMessage(character, "l", MSG_NPCHARACTER_POPTASK);
}

bool CharacterTurnByLoc(ref character, string group, string locator)
{
	return SendMessage(character, "lssl", MSG_CHARACTER_TURNBYLOC, group, locator, false);
}

bool CharacterTurnToLoc(ref character, string group, string locator)
{
	return SendMessage(character, "lssl", MSG_CHARACTER_TURNBYLOC, group, locator, true);
}

bool CharacterTurnByChr(ref character, ref byCharacter)
{
	return SendMessage(character, "li", MSG_CHARACTER_TURNBYCHR, byCharacter);
}

bool CharacterTurnByPoint(ref character, float x, float y, float z)
{
	return SendMessage(character, "lfff", MSG_CHARACTER_TURNBYPOINT, x, y, z);
}

bool CharacterTurnAy(ref character, float ay)
{
	return SendMessage(character, "lf", MSG_CHARACTER_TURNAY, ay);
}

bool GetCharacterDistByChr(ref character, ref byCharacter, ref float_dist2D)
{
	float d = -1.0;
	if(SendMessage(character, "lie", MSG_CHARACTER_DIST2D, byCharacter, &d) == 0) return false;
	if(d < 0) return false;
	float_dist2D = d;
	return true;
}

bool GetCharacterDistByChr3D(ref character, ref byCharacter, ref float_dist3D)
{
	float d = -1.0;
	if(SendMessage(character, "lie", MSG_CHARACTER_DIST3D, byCharacter, &d) == 0) return false;
	if(d < 0) return false;
	float_dist3D = d;
	return true;
}

bool CharacterPlayAction(ref character, string actionName)
{
	return SendMessage(character, "ls", MSG_CHARACTER_ACTIONPLAY, actionName);
}

bool GetCharacterDistByLoc(ref character, string group, string locator, ref float_dist3D)
{
	float d = -1.0;
	if(SendMessage(character, "lssse", MSG_CHARACTER_EX_MSG, "DistToLocator", group, locator, &d) == 0) return false;
	if(d < 0) return false;
	float_dist3D = d;
	return true;
}

string FindCharacterOptLocator(aref character, string group)
{
	string str = "";
	if(SendMessage(character, "lsse", MSG_CHARACTER_EX_MSG, "FindForvardLocator", group, &str) == 0) return "";
	return str;
}

string CharacterGetTask(ref character)
{
	string str = "";
	if(SendMessage(character, "le", MSG_NPCHARACTER_GETTASK, &str) == 0) str = "";
	return str;
}
