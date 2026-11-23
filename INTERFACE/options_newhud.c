// LEO: Используем общий жизненный цикл модального окна и сохраняемые ключи настроек.
bool bNewHUDLayoutPending=false;

void NewHUD_RebuildLayout()
{
	// LEO: Пересоздание HUD не должно сбрасывать узлы открытых диалогов XInterface.
	if(CheckAttribute(&InterfaceStates,"OptSubWin"))
	{
		if(InterfaceStates.OptSubWin=="NEWHUD" || InterfaceStates.OptSubWin=="SEA_HUD" || InterfaceStates.OptSubWin=="LAND_HUD")
		{
			bNewHUDLayoutPending=true;
			return;
		}
	}
	bNewHUDLayoutPending=false;
	NewHUD_RebuildInterface();
}

// LEO: Сравниваем с живым HUD: двойной щелчок возвращает master до открытия окна.
bool NewHUD_MasterLayoutChanged()
{
	int desired=NewHUD_GetOption("NewHUDEnabled");
	if(IsEntity(&ILogAndActions))
	{
		string texture=Log_CommandIconsTexture();
		if(texture!="" && CheckAttribute(&ILogAndActions,"ActiveActions.TextureName"))
		{
			if(ILogAndActions.ActiveActions.TextureName!=texture) return true;
		}
	}
	if(IsEntity(&objActivePerkShower))
	{
		if(CheckAttribute(&objActivePerkShower,"Textures.t0.file"))
		{
			if(objActivePerkShower.Textures.t0.file!=GetPerkTextureName("TimeSpeed")) return true;
		}
	}
	if(IsEntity(&objLandInterface))
	{
		if(CommandIcons_LayoutChanged(&objLandInterface)) return true;
		if(CheckAttribute(&objLandInterface,"ManSign.newhud.enabled"))
		{
			if(sti(objLandInterface.ManSign.newhud.enabled)!=desired) return true;
		}
	}
	if(IsEntity(&BattleInterface))
	{
		if(IsEntity(&worldMap))
		{
			if(CommandIcons_LayoutChanged(&BattleInterface)) return true;
			if(CheckAttribute(&BattleInterface,"wm_sign.newhud.enabled"))
			{
				if(sti(BattleInterface.wm_sign.newhud.enabled)!=desired) return true;
			}
		}
		else
		{
			if(NewSeaHUD_LayoutChanged(&BattleInterface)) return true;
			if(CommandIcons_LayoutChanged(&BattleInterface)) return true;
			if(CheckAttribute(&BattleInterface,"ShipIcon.newhud.enabled"))
			{
				if(sti(BattleInterface.ShipIcon.newhud.enabled)!=desired) return true;
			}
		}
	}
	return false;
}

void NewHUD_SyncMaster()
{
	if(bNewHUDLayoutPending || NewHUD_MasterLayoutChanged()) NewHUD_RebuildLayout();
}

void NewHUD_OptionsData()
{
	NewHUD_InitOptions();
	for(int i=1;i<12;i++)
	{
		string key=NewHUD_OptionKey(i);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_NEWHUD_"+i,2,1,sti(InterfaceStates.(key)));
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_NEWHUD_COMMAND_ICONS",2,1,NewHUD_GetOption("NewCommandIconsEnabled"));
	int alternate=0;
	if(CheckAttribute(&InterfaceStates,"AltIntIcons")) alternate=sti(InterfaceStates.AltIntIcons)!=0;
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_NEWHUD_ALT_ICONS",2,1,alternate);
	CommandIcons_OptionsData();
}

void CommandIcons_OptionsData()
{
	for(int i=1;i<=3;i++)
	{
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_COMMAND_BG_STANDARD",2,i,CommandIcons_GetBackground(false)==i-1);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_COMMAND_BG_ALT",2,i,CommandIcons_GetBackground(true)==i-1);
	}
}

bool CommandIcons_CheckChange(string node,int index,int state)
{
	// LEO: Общий обработчик радиокнопок передаёт морское качество без пересборки HUD.
	if(NewSeaHUD_CheckChange(node,index,state)) return true;
	bool alternate=false;
	if(node=="CHECK_COMMAND_BG_ALT") alternate=true;
	else
	{
		if(node!="CHECK_COMMAND_BG_STANDARD") return false;
	}
	int previous=CommandIcons_GetBackground(alternate);
	if(state!=0 && index>=1 && index<=3)
	{
		if(alternate) InterfaceStates.AltCommandIconBackground=index-1;
		else InterfaceStates.CommandIconBackground=index-1;
	}
	CommandIcons_OptionsData();
	if(previous!=CommandIcons_GetBackground(alternate)) NewHUD_RebuildLayout();
	return true;
}

bool NewHUD_CheckChange(string node,int state)
{
	if(node=="CHECK_NEWHUD_COMMAND_ICONS")
	{
		bool previousCommands=CommandIcons_Enabled();
		InterfaceStates.NewCommandIconsEnabled=state!=0;
		if(previousCommands!=CommandIcons_Enabled()) NewHUD_RebuildLayout();
		return true;
	}
	if(node=="CHECK_NEWHUD_ALT_ICONS")
	{
		int previous=0;
		if(CheckAttribute(&InterfaceStates,"AltIntIcons")) previous=sti(InterfaceStates.AltIntIcons)!=0;
		InterfaceStates.AltIntIcons=state!=0;
		if(previous!=sti(InterfaceStates.AltIntIcons)) NewHUD_RebuildLayout();
		return true;
	}
	for(int i=1;i<12;i++)
	{
		if(node != "CHECK_NEWHUD_"+i) continue;
		string key=NewHUD_OptionKey(i);
		InterfaceStates.(key)=state != 0;
		NewHUD_ApplyLive();
		return true;
	}
	return false;
}

bool NewHUD_OptionInfo(string node)
{
	if(node=="SEA_HUD_AIM_READOUT")
	{
		CreateTooltip("#"+XI_ConvertString("SeaAimReadout"),XI_ConvertString("SeaAimReadout_desc"),argb(255,255,255,255),"",0,"",0,"",0,"none","","",256,256);
		return true;
	}
	if(node=="CHECK_NEWHUD_COMMAND_ICONS")
	{
		ShowSimpleOptionInfo("NewCommandIconsEnabled");
		return true;
	}
	if(node=="CHECK_NEWHUD_ALT_ICONS")
	{
		ShowSimpleOptionInfo("AltIntIcons");
		return true;
	}
	for(int i=1;i<12;i++)
	{
		if(node != "CHECK_NEWHUD_"+i) continue;
		string key=NewHUD_OptionKey(i);
		CreateTooltip("#"+XI_ConvertString(key),XI_ConvertString(key+"_desc"),argb(255,255,255,255),"",0,"",0,"",0,"none","","",256,256);
		return true;
	}
	return false;
}

string NewSeaHUD_SliderKey(int index)
{
	if(index<0 || index>=8) return "";
	// LEO: Первые пять индексов прежние; новые ползунки лога не занимают ключи радиогрупп 5..7.
	if(index>=5) return NewSeaHUD_OptionKey(index+3);
	return NewSeaHUD_OptionKey(index);
}

void NewSeaHUD_AimReadoutData()
{
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SEA_HUD_AIM_READOUT",2,1,makeint(NewSeaHUD_GetOption("SeaAimReadout")));
}

// LEO: Инициализация слайдеров допустима только в скрытом окне.
void NewSeaHUD_OptionsData()
{
	for(int i=0;i<8;i++)
	{
		string key=NewSeaHUD_SliderKey(i);
		string node="SEA_HUD_SLIDE_"+i;
		float value=NewSeaHUD_GetOption(key);
		if(i==3) value=NewSeaHUD_NavigatorSizeToSlider(value);
		if(key=="SeaExpLogLines") value=ExpLog_LinesToSlider(value);
		GameInterface.nodes.(node).value=value;
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,node,0,value);
		NewSeaHUD_OptionLabel(i);
	}
	NewSeaHUD_QualityData();
	NewSeaHUD_PerksSideData();
	NewSeaHUD_ShipRenderData();
	NewSeaHUD_AimReadoutData();
}

void NewSeaHUD_ShipRenderData()
{
	int mode=makeint(NewSeaHUD_GetOption("SeaShipRenderMode"));
	for(int i=1;i<=2;i++)
	{
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SEA_HUD_SHIP_MODE",2,i,mode==i-1);
	}
}

void NewSeaHUD_PerksSideData()
{
	int side=makeint(NewSeaHUD_GetOption("SeaPerksSide"));
	for(int i=1;i<=3;i++)
	{
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SEA_HUD_PERKS_SIDE",2,i,side==i-1);
	}
}

void NewSeaHUD_QualityData()
{
	int quality=makeint(NewSeaHUD_GetOption("SeaMapQuality"));
	for(int i=1;i<=3;i++)
	{
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SEA_HUD_QUALITY",2,i,quality==i-1);
	}
}

bool NewSeaHUD_CheckChange(string node,int index,int state)
{
	if(node=="SEA_HUD_AIM_READOUT")
	{
		InterfaceStates.SeaAimReadout=state!=0;
		NewSeaHUD_ApplyAimReadout(&BattleInterface);
		return true;
	}
	if(node=="SEA_HUD_SHIP_MODE")
	{
		if(state!=0 && index>=1 && index<=2) InterfaceStates.SeaShipRenderMode=index-1;
		NewSeaHUD_ShipRenderData();
		return true;
	}
	if(node=="SEA_HUD_PERKS_SIDE")
	{
		if(state!=0 && index>=1 && index<=3) InterfaceStates.SeaPerksSide=index-1;
		NewSeaHUD_PerksSideData();
		return true;
	}
	if(node!="SEA_HUD_QUALITY") return false;
	if(state!=0 && index>=1 && index<=3) InterfaceStates.SeaMapQuality=index-1;
	return true;
}

void NewSeaHUD_OptionLabel(int index)
{
	string key=NewSeaHUD_SliderKey(index);
	if(key=="") return;
	if(key=="SeaExpLogLines")
	{
		SetFormatedText("SEA_HUD_VALUE_"+index,""+makeint(NewSeaHUD_GetOption(key)));
		return;
	}
	SetFormatedText("SEA_HUD_VALUE_"+index,makeint(NewSeaHUD_GetOption(key)*100.0+0.5)+"%");
}

bool NewSeaHUD_SlideChange(string node,float value)
{
	for(int i=0;i<8;i++)
	{
		if(node!="SEA_HUD_SLIDE_"+i) continue;
		string key=NewSeaHUD_SliderKey(i);
		value=NewSeaHUD_Clamp(value,0.0,1.0);
		if(i==3) value=NewSeaHUD_NavigatorSizeFromSlider(value);
		if(key=="SeaExpLogLines") value=ExpLog_LinesFromSlider(value);
		InterfaceStates.(key)=value;
		NewSeaHUD_OptionLabel(i);
		return true;
	}
	return false;
}

void ShowNewSeaHUDWindow(bool show)
{
	if(show)
	{
		NewSeaHUD_OptionsData();
		XI_WindowDisable("NEWHUD_WINDOW",true);
		XI_WindowShow("NEWHUD_WINDOW",false);
		XI_WindowShow("SEA_HUD_WINDOW",true);
		XI_WindowDisable("SEA_HUD_WINDOW",false);
		SetCurrentNode("SEA_HUD_WINDOW_CLOSE");
		InterfaceStates.OptSubWin="SEA_HUD";
	}
	else
	{
		XI_WindowDisable("SEA_HUD_WINDOW",true);
		XI_WindowShow("SEA_HUD_WINDOW",false);
		if(IsEntity(&BattleInterface))
		{
			if(!IsEntity(&worldMap))
			{
				if(NewSeaHUD_LayoutChanged(&BattleInterface)) NewHUD_RebuildLayout();
			}
		}
		ShowNewHUDWindow(true);
		SetCurrentNode("SEA_HUD_OPEN");
	}
}

// LEO: Сброс обновляет скрытые узлы; после показа явно снимаем блокировку и возвращаем текущий узел.
bool NewSeaHUD_Button(string node)
{
	if(node=="SEA_HUD_OPEN") {ShowNewSeaHUDWindow(true);return true;}
	if(node=="SEA_HUD_AIM_READOUT")
	{
		NewSeaHUD_CheckChange(node,1,NewSeaHUD_GetOption("SeaAimReadout")==0.0);
		NewSeaHUD_AimReadoutData();
		return true;
	}
	if(node=="SEA_HUD_RESET")
	{
		XI_WindowShow("SEA_HUD_WINDOW",false);
		for(int i=0;i<12;i++)
		{
			string key=NewSeaHUD_OptionKey(i);
			InterfaceStates.(key)=NewSeaHUD_Default(key);
		}
		NewSeaHUD_ApplyAimReadout(&BattleInterface);
		NewSeaHUD_OptionsData();
		XI_WindowShow("SEA_HUD_WINDOW",true);
		XI_WindowDisable("SEA_HUD_WINDOW",false);
		SetCurrentNode("SEA_HUD_RESET");
		return true;
	}
	return false;
}

void NewLandHUD_OptionLabel(int index)
{
	string key=LandHUD_OptionKey(index);
	float value=LandHUD_GetOption(key);
	if(key=="LandExpLogLines")
	{
		SetFormatedText("LAND_HUD_VALUE_"+index,""+makeint(value));
		return;
	}
	SetFormatedText("LAND_HUD_VALUE_"+index,makeint(value*100.0+0.5)+"%");
}

void NewLandHUD_OptionsData()
{
	for(int i=0;i<6;i++)
	{
		string node="LAND_HUD_SLIDE_"+i;
		string key=LandHUD_OptionKey(i);
		float value=LandHUD_GetOption(key);
		if(key=="LandExpLogLines") value=ExpLog_LinesToSlider(value);
		GameInterface.nodes.(node).value=value;
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,node,0,value);
		NewLandHUD_OptionLabel(i);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"LAND_HUD_AIM_OUTLINE",2,1,LandHUD_GetOption("LandAimOutline")>0.0);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"LAND_HUD_AIM_INVERT",2,1,LandHUD_GetOption("LandAimInvert")>0.0);
}

bool NewLandHUD_CheckChange(string node,int state)
{
	string key;
	if(node=="LAND_HUD_AIM_OUTLINE") key="LandAimOutline";
	else if(node=="LAND_HUD_AIM_INVERT") key="LandAimInvert";
	else return false;
	InterfaceStates.(key)=state!=0;
	if(IsEntity(&objLandInterface)) BLI_ApplyAimHUDOptions();
	return true;
}

bool NewLandHUD_SlideChange(string node,float value)
{
	for(int i=0;i<6;i++)
	{
		if(node!="LAND_HUD_SLIDE_"+i) continue;
		string key=LandHUD_OptionKey(i);
		if(key=="LandExpLogLines") value=ExpLog_LinesFromSlider(value);
		else
		{
			if(value<0.0) value=0.0;
			if(value>1.0) value=1.0;
		}
		if(value!=LandHUD_GetOption(key)) InterfaceStates.(key)=value;
		NewLandHUD_OptionLabel(i);
		return true;
	}
	return false;
}

void ShowNewLandHUDWindow(bool show)
{
	if(show)
	{
		NewLandHUD_OptionsData();
		XI_WindowDisable("NEWHUD_WINDOW",true);
		XI_WindowShow("NEWHUD_WINDOW",false);
		XI_WindowShow("LAND_HUD_WINDOW",true);
		XI_WindowDisable("LAND_HUD_WINDOW",false);
		SetCurrentNode("LAND_HUD_WINDOW_CLOSE");
		InterfaceStates.OptSubWin="LAND_HUD";
		return;
	}
	XI_WindowDisable("LAND_HUD_WINDOW",true);
	XI_WindowShow("LAND_HUD_WINDOW",false);
	BLI_RefreshLandHUDSettings();
	ShowNewHUDWindow(true);
	SetCurrentNode("LAND_HUD_OPEN");
}

bool NewLandHUD_Button(string node)
{
	if(node=="LAND_HUD_OPEN") {ShowNewLandHUDWindow(true);return true;}
	if(node!="LAND_HUD_RESET") return false;
	XI_WindowShow("LAND_HUD_WINDOW",false);
	for(int i=0;i<8;i++)
	{
		string key=LandHUD_OptionKey(i);
		InterfaceStates.(key)=LandHUD_Default(key);
	}
	if(IsEntity(&objLandInterface)) BLI_ApplyAimHUDOptions();
	NewLandHUD_OptionsData();
	XI_WindowShow("LAND_HUD_WINDOW",true);
	XI_WindowDisable("LAND_HUD_WINDOW",false);
	SetCurrentNode("LAND_HUD_RESET");
	return true;
}

void ShowNewHUDWindow(bool show)
{
	if(show)
	{
		InterfaceStates.OptSubWin="NEWHUD";
		NewHUD_SyncMaster();
		NewHUD_OptionsData();
		XI_WindowShow("NEWHUD_WINDOW",true);
		XI_WindowDisable("NEWHUD_WINDOW",false);
		XI_WindowDisable("OTHERS_WINDOW",true);
		XI_WindowDisable("MAIN_WINDOW",true);
		SetCurrentNode("NEWHUD_WINDOW_CLOSE");
	}
	else
	{
		XI_WindowDisable("NEWHUD_WINDOW",true);
		XI_WindowShow("NEWHUD_WINDOW",false);
		DeleteAttribute(&InterfaceStates,"OptSubWin");
		XI_WindowDisable("OTHERS_WINDOW",false);
		XI_WindowDisable("MAIN_WINDOW",false);
		NewHUD_SyncMaster();
		// LEO: Возвращаем внутренний текущий узел после пересборки, не меняя фокус окна ОС.
		SetCurrentNode("BTN_OK");
	}
}
