// LEO: В существующем окне подписей меняем только режим фортов; имена узлов/ключа save сохраняем.
void ShipInfoBars_RefreshOption()
{
	string key = "ShipInfoBarsAlways";
	int mode = ShipInfoBars_GetMode();
	if (mode == 1) key = "ShipInfoBarsCombat";
	if (mode == 2) key = "ShipInfoBarsOff";
	int language = LanguageOpenFile("ShipInfoBars.txt");
	SetFormatedText("SHIPINFO_BARS_LABEL", LanguageConvertString(language, "ShipInfoBarsLabel"));
	SetFormatedText("SHIPINFO_BARS_TEXT", LanguageConvertString(language, key));
	LanguageCloseFile(language);
}

void ShipInfoBars_ChangeMode(bool forward)
{
	int mode = ShipInfoBars_GetMode();
	if (forward) mode++;
	else mode--;
	if (mode > 2) mode = 0;
	if (mode < 0) mode = 2;
	InterfaceStates.ShipInfoBarsMode = mode;
	ShipInfoBars_Update();
	ShipInfoBars_RefreshOption();
}
