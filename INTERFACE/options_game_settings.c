#include "interface\main_settings\game_options.c"
#include "interface\main_settings\sound_options.c"

void FillOptionsTable(string table_name)
{
	switch(table_name)
	{
		case "TABLE_GAME_OPTIONS":
			GetGameOptionsList();
			for (i = 0; i < GameOptionsQty; i++) SetDefaultsOptionsTable(table_name, GameOptionsList[i], i+1); // Gregg: < вместо <= — без хвостовой пустой строки (мешала скроллу)
			break;
		case "TABLE_GAME_OPTIONS_MASTER":
			GetGameMasterOptionsList();
			for (i = 0; i < GameMasterOptionsQty; i++) SetDefaultsOptionsTable(table_name, GameMasterOptionsList[i], i+1);
			break;
		case "TABLE_SOUND_OPTIONS": 
			GetSoundOptionsList(); 
			for (i = 0; i <= SoundOptionsQty; i++) SetDefaultsOptionsTable(table_name, SoundOptionsList[i], i+1);
			break;
	}
    
    Table_UpdateWindow(table_name);
}

void SetDefaultsOptionsTable(string table_name, string param, int idx) 
{
	string srow = "tr"+idx;
	aref row;
	makearef(row, GameInterface.(table_name).(srow));

	row.hidden = false;
	if (param == "") {
		row.hidden = true;
		return;
	}

	row.currentNode = param;
	row.td2.icon.align = "center";
	row.td2.icon.valign = "center";
	row.td2.icon.width = 16;
	row.td2.icon.height = 16;
	row.td2.icon.group = "CHECKBUTTON_ICON";
	row.td1.str = XI_ConvertString(param);
	if (sti(InterfaceStates.(param)))
	{
		row.td2.icon.image = "check_yes";
		row.td1.color = argb(255,255,255,255);
	}
	else
	{
		row.td2.icon.image = "check_no";
		row.td1.color = argb(255,180,180,180);
	}
}

void RightClickOption(string table_name)
{
	string srow = "tr" + GameInterface.(table_name).select;
	aref row;
	makearef(row, GameInterface.(table_name).(srow));
	if (row.hidden) return;
	
	ShowSimpleOptionInfo(row.currentNode);
}

void ClickOption(string table_name)
{
	string srow = "tr" + GameInterface.(table_name).select;
	aref row;
	makearef(row, GameInterface.(table_name).(srow));
	if (row.hidden) return;

	string param = row.currentNode;

	if (row.td2.icon.image == "check_yes")
	{
		row.td2.icon.image = "check_no";
		row.td1.color = argb(255,180,180,180);
	}
	else
	{
		row.td2.icon.image = "check_yes";
		row.td1.color = argb(255,255,255,255);
	}
	Table_UpdateWindow(table_name);

	InterfaceStates.(param) = !sti(InterfaceStates.(param));
	// LEO: Раскладка морских баров применяется при создании HUD.
	if (param == "ShipBarsHorizontal")
	{
		NewHUD_RebuildLayout();
	}
	// LEO: Здесь нельзя обращаться к loadedLocation: настройки доступны и из главного меню.
	if (param == "RollingBroadside")
	{
		RollingBroadside_ApplyLive();
	}
	// LEO: Открытые окна сохраняют старые текстуры; новая тема применяется при следующем открытии.
	if (param == "BlueInterfaceTheme")
	{
		SetInterfaceTheme(sti(InterfaceStates.BlueInterfaceTheme));
	}
	if (param == "RegionsWorldmap")
	{
		RegionsWorldmap_ApplyLive();
	}
	if (param == "WorldRadarOn")
	{
		WorldRadar_ApplyLive();
	}
	if (param == "SeaCamFreeAzimuth")
	{
		SeaCamFreeAzimuth_ApplyLive();
	}
}
