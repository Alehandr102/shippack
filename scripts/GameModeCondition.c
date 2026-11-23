// Warship. В этом скрипте идёт обработка прерывания, выполняющегося в каждом фрейме

#define MAX_GAMEMODE_CONDITIONS 10

#event_handler("frame", "CheckGameModeConditions");
#event_handler("frame", "CheckItemsGameModeConditions");

string GameModeConditions[MAX_GAMEMODE_CONDITIONS] = { "", "", "", "", "", "", "", "", "", "", };
string sCurGameModeCondition = "";

void SetGameModeCondition(string _sQuest)
{
	if (_sQuest == "") return;
	int freeSlot = -1;
	for(int i=0; i<MAX_GAMEMODE_CONDITIONS; i++)
	{
		if(GameModeConditions[i] == _sQuest) return;
		if(freeSlot < 0 && GameModeConditions[i] == "") freeSlot = i;
	}
	if(freeSlot >= 0)
	{
		SetEventHandler(_sQuest, _sQuest, 0); // Ставим евент
		GameModeConditions[freeSlot] = _sQuest;
		return;
	}

	Log_TestInfo("Critical Error: No more empty GameMode conditions.");
}

void CheckGameModeConditions() // Процессирование всего
{
	for(int i=0; i<MAX_GAMEMODE_CONDITIONS; i++)
	{
		if(GameModeConditions[i] == "") continue;
		sCurGameModeCondition = GameModeConditions[i];
		PostEvent(sCurGameModeCondition, 10);
	}
}

// Работа с предметами -->
void CheckItemsGameModeConditions() // Процессирование предметов
{
	if(iScriptItemCount == 0) return;
	int i;
	ref rItem;
	string sItem;
	for(i=iScriptItemStartNum; i<iScriptItemEndNum; i++)
	{
		rItem = &items[i];
		if(!CheckAttribute(rItem, "ID")) continue;
		sItem = items[i].id;
		if(CheckAttribute(rItem, "Script") && CheckCharacterItem(PChar, sItem))
		{
			Log_TestInfo(GetConvertStrWithReplace("Variable_GameModeCondition_1", "Logs.txt", "#space#", " ")+rItem.Script);
			SetGameModeCondition(rItem.Script);
			DeleteGameModeConditionFromItem(sItem); // Удаляем скрипт с предмета
		}
	}
}

void DeleteGameModeConditionFromItem(string sItemID)
{
	ref rItem = ItemsFromID(sItemID);
	DeleteAttribute(rItem, "Script");
	iScriptItemCount--; // Заскриптованных предметов стало меньше
}
// <-- Работа с предметами

void DeleteGameModeCondition(string _sQuest)
{
	if (_sQuest == "") return;
	bool found = false;
	for(int i=0; i<MAX_GAMEMODE_CONDITIONS; i++)
	{
		if(GameModeConditions[i] == _sQuest)
		{
			GameModeConditions[i] = "";
			found = true;
		}
	}
	if(found) DelEventHandler(_sQuest, _sQuest); // Удалим евент
}
