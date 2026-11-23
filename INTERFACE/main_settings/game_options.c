string GameOptionsList[50];
int GameOptionsQty = 0;
string GameMasterOptionsList[20];
int GameMasterOptionsQty = 0;

// LEO: Левая таблица: переключатели без вложенных окон.
void GetGameOptionsList()
{
	string s;
	GameOptionsQty = 0;
	for(int i = 0; i < 50; i++)
	{
		s = "";
		switch(i)
		{
			case 0: s = "RegionsWorldmap"; break;
			case 1: s = "SimpleSea"; break;
			case 2: s = "ShowBattleMode"; break;
			case 3: s = "SpyglassTextures"; break;
			case 4: s = "ShowBoardMode"; break;
			case 5: s = "EnableSailors"; break;
			case 6: s = "CharVoice"; break;
			case 7: s = "IslandLoader"; break;
			case 8: s = "EnabledCMControls"; break;
			case 9: s = "EnabledOldStore"; break;
			case 10: s = "ShowExpLogs"; break;
			case 11: s = "DiscordRPC"; break;
			case 12: s = "ShowGameLog"; break;
			case 13: s = "DirectSail"; break;
			case 14: s = "SpyglassFromMars"; break;
			case 15: s = "RollingBroadside"; break;
			case 16: s = "SeaSpeakSignals"; break;
			case 17: s = "SeaCamFreeAzimuth"; break;
			case 18: s = "GunTrail"; break;
			case 19: s = "ChopBodies"; break;
			case 20: s = "DialogPortrait"; break;
			case 21: s = "BlueInterfaceTheme"; break;
			case 22: s = "NpcHeadLook"; break;
		}
		GameOptionsList[GameOptionsQty] = s;
		if(s != "") GameOptionsQty++;
	}
}

// LEO: Правая таблица: двойной щелчок открывает настройки опции.
void GetGameMasterOptionsList()
{
	string s;
	GameMasterOptionsQty = 0;
	for(int i = 0; i < 20; i++)
	{
		s = "";
		switch(i)
		{
			case 0: s = "CharCap"; break;
			case 1: s = "ShipCap"; break;
			case 2: s = "RadarEnabled"; break;
			case 3: s = "ConsumablesHUD"; break;
			case 4: s = "DialogDelay"; break;
			case 5: s = "BoardingSignals"; break;
			case 6: s = "FleetFormationCfg"; break;
			case 7: s = "WorldRadarOn"; break;
			case 8: s = "TimeAccelSettings"; break;
			case 9: s = "EnabledAutoSaveMode"; if(bHardcoreGame) continue; break;
			case 10: s = "ProgressAnimation"; break;
			case 11: s = "NewHUDEnabled"; break;
			case 12: s = "RagdollEnabled"; break;
		}
		GameMasterOptionsList[GameMasterOptionsQty] = s;
		if(s != "") GameMasterOptionsQty++;
	}
}

void ClickGameOptions() {
	ClickOption("TABLE_GAME_OPTIONS");
}
void RightClickGameOptions() {
	RightClickOption("TABLE_GAME_OPTIONS");
}
void SetupGameOptions() {
	// LEO: Повторный щелчок компенсирует переключение при двойном нажатии.
	ClickOption("TABLE_GAME_OPTIONS");
}

void ClickMasterOptions() {
	ClickOption("TABLE_GAME_OPTIONS_MASTER");
}
void RightClickMasterOptions() {
	RightClickOption("TABLE_GAME_OPTIONS_MASTER");
}
// LEO: Движок сначала присылает обычный щелчок; отменяем переключение перед открытием окна.
void SetupMasterOptions() {
	string srow = "tr" + GameInterface.TABLE_GAME_OPTIONS_MASTER.select;
	aref row;
	makearef(row, GameInterface.TABLE_GAME_OPTIONS_MASTER.(srow));
	if(row.hidden) return;
	string param = row.currentNode;
	ClickOption("TABLE_GAME_OPTIONS_MASTER");
	if(param == "CharCap") ShowCharCapWindow(true);
	if(param == "ShipCap") ShowShipCapWindow(true);
	if(param == "RadarEnabled") ShowRadarWindow(true);
	if(param == "ConsumablesHUD") ShowConsumablesWindow(true);
	if(param == "DialogDelay") ShowDialogDelayWindow(true);
	if(param == "BoardingSignals") ShowBoardingSignalsWindow(true);
	if(param == "FleetFormationCfg") ShowFormationWindow(true);
	if(param == "WorldRadarOn") ShowWorldRadarWindow(true);
	if(param == "TimeAccelSettings") ShowTimeAccelWindow(true);
	if(param == "EnabledAutoSaveMode") ShowAutoSaveWindow(true);
	if(param == "ProgressAnimation") ShowProgressAnimWindow(true);
	if(param == "NewHUDEnabled") ShowNewHUDWindow(true);
	if(param == "RagdollEnabled") ShowRagdollWindow(true);
}
