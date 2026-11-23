// LEO: Новые команды независимы от портретов и баров; master off всегда оставляет исходные атласы.
bool CommandIcons_Enabled()
{
	return NewHUD_GetOption("NewHUDEnabled")!=0 && NewHUD_GetOption("NewCommandIconsEnabled")!=0;
}

// LEO: Фоны независимы от формы портретов и сохраняются отдельно для двух оформлений.
int CommandIcons_GetBackground(bool alternate)
{
	string key="CommandIconBackground";
	int fallback=0;
	if(alternate) {key="AltCommandIconBackground";fallback=1;}
	if(!CheckAttribute(&InterfaceStates,key)) return fallback;
	int value=sti(InterfaceStates.(key));
	if(value<0 || value>2) return fallback;
	return value;
}

void CommandIcons_SaveOptions(ref options)
{
	options.cameramode.CommandIconBackground=CommandIcons_GetBackground(false);
	options.cameramode.AltCommandIconBackground=CommandIcons_GetBackground(true);
}

void CommandIcons_LoadOptions(ref options)
{
	InterfaceStates.CommandIconBackground=0;
	InterfaceStates.AltCommandIconBackground=1;
	if(CheckAttribute(options,"cameramode.CommandIconBackground"))
		InterfaceStates.CommandIconBackground=sti(options.cameramode.CommandIconBackground);
	if(CheckAttribute(options,"cameramode.AltCommandIconBackground"))
		InterfaceStates.AltCommandIconBackground=sti(options.cameramode.AltCommandIconBackground);
	InterfaceStates.CommandIconBackground=CommandIcons_GetBackground(false);
	InterfaceStates.AltCommandIconBackground=CommandIcons_GetBackground(true);
}

void CommandIcons_SetTexture(aref entry,string family)
{
	DeleteAttribute(entry,"skin");
	if(!CommandIcons_Enabled()) return;
	if(!CheckAttribute(entry,"name")) return;
	string style="Standard";
	if(AltIntIcons) style="AltIcons";
	entry.skin.enabled=1;
	entry.skin.profile="resource\textures\battle_interface\NewHUD\CommandIcons\"+style+"\Atlas.ini";
	entry.skin.family=family;
	entry.skin.background=CommandIcons_GetBackground(AltIntIcons);
}

void CommandIcons_SetSlot(aref list,string slot,string family)
{
	if(!CheckAttribute(list,slot)) return;
	aref entry;
	makearef(entry,list.(slot));
	CommandIcons_SetTexture(entry,family);
}

// LEO: Корабли командного списка сохраняют исходный выбор атласа и номера 2*N/2*N+1, меняется только skin.
void CommandIcons_SetShipTexture(aref list)
{
	if(!CommandIcons_Enabled() || !CheckAttribute(list,"t1.name")) return;
	aref entry;
	makearef(entry,list.t1);
	// LEO: Корабли ПКМ сохраняют родные фон, рамку и выделение исходного атласа.
	if(entry.name=="battle_interface\ship_icons1_PotC.dds") return;
	string variant="";
	if(entry.name=="battle_interface\ship_icons1.dds") variant="Standard";
	if(entry.name=="battle_interface\ship_icons1_Konshud.dds") variant="AltIcons";
	if(variant=="") return;
	entry.skin.enabled=1;
	entry.skin.profile="resource\textures\battle_interface\NewHUD\ShipCommandIcons\"+variant+"\Atlas.ini";
	entry.skin.family="Ships";
	entry.skin.background=CommandIcons_GetBackground(AltIntIcons);
}

// LEO: Совместимая сетка для F3 и статичных перков, читающих старые номера напрямую.
string CommandIcons_Texture(string family)
{
	bool alternate=false;
	if(CheckAttribute(&InterfaceStates,"AltIntIcons")) alternate=sti(InterfaceStates.AltIntIcons)!=0;
	if(!CommandIcons_Enabled())
	{
		string suffix="";
		if(alternate) suffix="_Konshud";
		return "battle_interface\"+family+suffix+".dds";
	}
	string style="Standard";
	if(alternate) style="AltIcons";
	return "battle_interface\NewHUD\CommandIcons\"+style+"\Legacy\"+family+"_"+CommandIcons_GetBackground(alternate)+".dds";
}

// LEO: Статичные действия и перки: номер означает данные (открыт/закрыт), а не наведение.
string CommandIcons_StaticTexture(string family)
{
	if(!CommandIcons_Enabled()) return CommandIcons_Texture(family);
	bool alternate=false;
	if(CheckAttribute(&InterfaceStates,"AltIntIcons")) alternate=sti(InterfaceStates.AltIntIcons)!=0;
	string style="Standard";
	if(alternate) style="AltIcons";
	return "battle_interface\NewHUD\CommandIcons\"+style+"\Static\"+family+"_"+CommandIcons_GetBackground(alternate)+".dds";
}

// LEO: Собственный атлас буксировки использует штатные фоны, выделение и выбор оформления.
void CommandIcons_SetTowTexture(aref list)
{
	if(!CommandIcons_Enabled() || !CheckAttribute(list,"t4.name")) return;
	string style="Standard";
	if(AltIntIcons) style="AltIcons";
	aref entry;
	makearef(entry,list.t4);
	if(entry.name!="battle_interface\NewHUD\CommandIcons\Tow\"+style+"\Legacy.dds") return;
	entry.skin.enabled=1;
	entry.skin.profile="resource\textures\battle_interface\NewHUD\CommandIcons\Tow\"+style+"\Atlas.ini";
	entry.skin.family="Tow";
	entry.skin.background=CommandIcons_GetBackground(AltIntIcons);
}

void CommandIcons_Setup(aref owner,int context)
{
	// LEO: Море удаляет CommandTextures после создания движкового HUD; кэш оформления должен пережить это.
	owner.CommandIconState.master=CommandIcons_Enabled();
	owner.CommandIconState.alternate=AltIntIcons;
	owner.CommandIconState.standardBackground=CommandIcons_GetBackground(false);
	owner.CommandIconState.altBackground=CommandIcons_GetBackground(true);
	owner.CommandTextures.backgroundStandard=CommandIcons_GetBackground(false);
	owner.CommandTextures.backgroundAlt=CommandIcons_GetBackground(true);
	if(!CheckAttribute(owner,"CommandTextures.list")) return;
	aref list,entry;
	makearef(list,owner.CommandTextures.list);
	int count=GetAttributesNum(list);
	for(int i=0;i<count;i++)
	{
		entry=GetAttributeN(list,i);
		DeleteAttribute(entry,"skin");
	}
	if(!CommandIcons_Enabled()) return;
	if(context==0)
	{
		CommandIcons_SetSlot(list,"t0","LandCommands");
		CommandIcons_SetSlot(list,"t1","LandTarget_SLB");
		CommandIcons_SetSlot(list,"t2","Cancel");
		CommandIcons_SetSlot(list,"t3","UseFood");
	}
	if(context==1)
	{
		CommandIcons_SetSlot(list,"t0","List_icons");
		CommandIcons_SetShipTexture(list);
		CommandIcons_SetSlot(list,"t2","Cancel");
		CommandIcons_SetTowTexture(list);
	}
	if(context==2)
	{
		CommandIcons_SetSlot(list,"t0","Cancel");
		CommandIcons_SetSlot(list,"t1","WorldMapCommands");
	}
}

bool CommandIcons_LayoutChanged(aref owner)
{
	if(CheckAttribute(owner,"CommandIconState.master"))
	{
		if(sti(owner.CommandIconState.master)!=CommandIcons_Enabled()) return true;
		bool alternate=false;
		if(CheckAttribute(&InterfaceStates,"AltIntIcons")) alternate=sti(InterfaceStates.AltIntIcons)!=0;
		if(sti(owner.CommandIconState.alternate)!=alternate) return true;
		if(sti(owner.CommandIconState.standardBackground)!=CommandIcons_GetBackground(false)) return true;
		if(sti(owner.CommandIconState.altBackground)!=CommandIcons_GetBackground(true)) return true;
		return false;
	}
	// LEO: Совместимость с уже живым интерфейсом, созданным до появления отдельного кэша.
	if(!CheckAttribute(owner,"CommandTextures.backgroundStandard")) return false;
	if(sti(owner.CommandTextures.backgroundStandard)!=CommandIcons_GetBackground(false)) return true;
	if(!CheckAttribute(owner,"CommandTextures.backgroundAlt")) return false;
	return sti(owner.CommandTextures.backgroundAlt)!=CommandIcons_GetBackground(true);
}
