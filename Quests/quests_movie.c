

int questMovieProcess = 0;
object questMovieStack;
object questMovieWideScreen;
bool questMovieIsLockPlayerCtrl = false;
string questMovieOldSaveState = true;



void ResetQuestMovie()
{
	questMovieProcess = 0;
	questMovieIsLockPlayerCtrl = false;
	questMovieOldSaveState = true;
}


bool StartQuestMovie(bool noReload, bool noSave, bool lockPlayerCtrl)
{
	Event("EventStartQuestMovie", "lll", noReload, noSave, lockPlayerCtrl);
	//Stack level
	int prevLevel = questMovieProcess - 1;
	string level = "L" + questMovieProcess;
	questMovieStack.(level) = "";
	aref stk;
	makearef(stk, questMovieStack.(level));
	//Set current parameters
	stk.noReload = noReload;
	stk.noSave = noSave;
	stk.lock = lockPlayerCtrl;
	//Set parameters
	if(questMovieProcess > 0)
	{
		InterfaceStates.Buttons.Save.enable = noSave;
		if(lockPlayerCtrl != false)
		{
			QuestMovieLockPlayer();
		}
		else
		{
			QuestMovieUnLockPlayer();
		}
	}
	else
	{
		questMovieOldSaveState = InterfaceStates.Buttons.Save.enable;
		InterfaceStates.Buttons.Save.enable = noSave;
		if(lockPlayerCtrl != false) QuestMovieLockPlayer();
	}
	questMovieProcess = questMovieProcess + 1;
	BI_ShowInterface(false);
	Log_SetVisible(false);   // строки лога поверх кино-полос не нужны
	return true;
}

bool EndQuestMovie()
{
	if(questMovieProcess == 0) return false;
	Event("EndStartQuestMovie");
	questMovieProcess = questMovieProcess - 1;
	if(questMovieProcess == 0)
	{
		QuestMovieUnLockPlayer();
		InterfaceStates.Buttons.Save.enable = questMovieOldSaveState;
		// Gregg: возврат боевого интерфейса. Раньше эта строка стояла ТОЛЬКО ниже, за ранним
		// return, поэтому отрабатывала лишь при выходе из ВЛОЖЕННОЙ катсцены. После внешней
		// интерфейс, погашенный в StartQuestMovie через BI_ShowInterface(false), так и оставался
		// скрытым до перезагрузки локации (её обработчик EVENT_LOCATION_LOAD поднимал его заново).
		BI_ShowInterface(true);
		Log_SetVisible(true);
		return false;
	}
	string level = "L" + questMovieProcess;
	aref stk;
	makearef(stk, questMovieStack.(level));
	InterfaceStates.Buttons.Save.enable = stk.noSave;
	if(sti(stk.lock) != 0)
	{
		QuestMovieLockPlayer();
	}
	else
	{
		QuestMovieUnLockPlayer();
	}
	BI_ShowInterface(true);
	return true;
}

// Gregg: ЯВНАЯ установка боевого интерфейса для катсцены.
// Раньше катсцена дёргала ChangeShowIntarface() - а это ПЕРЕКЛЮЧАТЕЛЬ, который на суше
// физически создаёт или уничтожает лендинтерфейс (interface_utils.c). Пока катсцена идёт,
// интерфейс успевает подняться сам - обработчиком EVENT_LOCATION_LOAD при смене локации
// или через BLI_EnableShow на выходе из диалога. Тумблер об этом не знает и на закрытии
// катсцены щёлкает второй раз, УНИЧТОЖАЯ интерфейс вместо возврата; дальше он мёртв
// до следующей загрузки локации.
void QuestMovieSetInterface(bool bShow)
{
	if (bSeaActive && !bAbordageStarted)
	{
		if (bShow)
		{
			if (!IsEntity(BattleInterface))
			{
				InitBattleInterface();
				StartBattleInterface();
				RefreshBattleInterface();
			}
			return;
		}
		if (IsEntity(BattleInterface)) DeleteBattleInterface();
		return;
	}
	if (IsEntity(worldMap)) return;
	if (bShow)
	{
		if (!bLandInterfaceStart) StartBattleLandInterface();
		return;
	}
	if (bLandInterfaceStart) EndBattleLandInterface();
}

void QuestMovieLockPlayer()
{
	if(questMovieIsLockPlayerCtrl == true) return;
	questMovieIsLockPlayerCtrl = true;
	QuestMovieSetInterface(false);
	SetCharacterTask_Stay(GetMainCharacter());
	CreateEntity(&questMovieWideScreen, "WideScreen");
}

void QuestMovieUnLockPlayer()
{
	if(questMovieIsLockPlayerCtrl == false) return;
	questMovieIsLockPlayerCtrl = false;
	QuestMovieSetInterface(true);
	SetCharacterTask_None(GetMainCharacter());
	SendMessage(&questMovieWideScreen, "");
}

bool qmIsNoReload()
{
	if(questMovieProcess > 0)
	{
		int l = questMovieProcess - 1;
		string level = "L" + l + ".noReload";
		if(sti(questMovieStack.(level)) != 0) return true;
	}
	return false;
}

