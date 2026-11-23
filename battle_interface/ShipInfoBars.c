// LEO: Только полосы фортов: 0 = всегда, 1 = в бою, 2 = выкл.; имя ключа оставлено для совместимости настроек.
int ShipInfoBars_GetMode()
{
	if (!CheckAttribute(&InterfaceStates, "ShipInfoBarsMode")) return 0;
	int mode = sti(InterfaceStates.ShipInfoBarsMode);
	if (mode < 0 || mode > 2) return 0;
	return mode;
}

bool ShipInfoBars_InCombat()
{
	if (!bSeaActive || bAbordageStarted) return false;
	if (LAi_IsDead(pchar)) return false;
	if (!CheckAttribute(pchar, "Ship.POS.Mode")) return false;
	// LEO: SHIP_WAR вычисляет Ship_CheckReloadEnable по врагам и фортам, не по квестовому запрету карты.
	return sti(pchar.Ship.POS.Mode) == SHIP_WAR;
}

void ShipInfoBars_Init()
{
	bool migrated = false;
	if (CheckAttribute(&BattleInterface, "ShipInfoImages.FortBarsPolicyVersion"))
		migrated = sti(BattleInterface.ShipInfoImages.FortBarsPolicyVersion) >= 1;
	if (!migrated)
	{
		// LEO: Однократно очищаем общий гейт старой версии; последующие записи других модов сохраняем.
		DeleteAttribute(&BattleInterface,"ShipInfoImages.BarsVisible");
		BattleInterface.ShipInfoImages.FortBarsPolicyVersion = 1;
	}
	ShipInfoBars_Update();
}

void ShipInfoBars_Update()
{
	int mode = ShipInfoBars_GetMode();
	int visible = 1;
	if (mode == 1) visible = ShipInfoBars_InCombat();
	if (mode == 2) visible = 0;
	BattleInterface.ShipInfoImages.FortBarsVisible = visible;
}

void ShipInfoBars_SaveOptions(ref optref)
{
	optref.cameramode.ShipInfoBarsMode = ShipInfoBars_GetMode();
}

void ShipInfoBars_LoadOptions(ref optref)
{
	InterfaceStates.ShipInfoBarsMode = 0;
	if (CheckAttribute(optref, "cameramode.ShipInfoBarsMode"))
		InterfaceStates.ShipInfoBarsMode = optref.cameramode.ShipInfoBarsMode;
	InterfaceStates.ShipInfoBarsMode = ShipInfoBars_GetMode();
	ShipInfoBars_Init();
}
