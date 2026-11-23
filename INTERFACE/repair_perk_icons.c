// LEO: Только две ремонтные способности в окне персонажа; ENABLE/DISABLE означает изучение, не hover.
string RepairPerk_Texture(string group,string perk)
{
	if(!CommandIcons_Enabled()) return "";
	if(perk!="LightRepair" && perk!="InstantRepair") return "";
	if(group!="PERK_ENABLE" && group!="PERK_DISABLE") return "";
	bool alternate=false;
	if(CheckAttribute(&InterfaceStates,"AltIntIcons")) alternate=sti(InterfaceStates.AltIntIcons)!=0;
	string style="Standard";
	if(alternate) style="AltIcons";
	// LEO: До установки отдельного экспорта сохраняем оригинал; исходные PERKS DDS/INI не изменяются.
	string folder="RESOURCE\Textures\battle_interface\NewHUD\CommandIcons\"+style+"\Static";
	if(!IsFileExist(folder,"RepairPerks_"+CommandIcons_GetBackground(alternate)+".dds")) return "";
	return CommandIcons_StaticTexture("RepairPerks");
}

// LEO: Атлас 2x2: сверху LightRepair/InstantRepair изучены, снизу те же рисунки не изучены.
void RepairPerk_SetTableIcon(aref icon,string group,string perk)
{
	DeleteAttribute(icon,"texture");
	DeleteAttribute(icon,"uv");
	icon.group=group;
	icon.image=perk;
	string texture=RepairPerk_Texture(group,perk);
	if(texture=="") return;
	DeleteAttribute(icon,"group");
	DeleteAttribute(icon,"image");
	icon.texture=texture;
	if(group=="PERK_ENABLE")
	{
		icon.uv="0,0,0.5,0.5";
		if(perk=="InstantRepair") icon.uv="0.5,0,1,0.5";
	}
	else
	{
		icon.uv="0,0.5,0.5,1";
		if(perk=="InstantRepair") icon.uv="0.5,0.5,1,1";
	}
}

void RepairPerk_SetPicture(string node,string group,string perk)
{
	string texture=RepairPerk_Texture(group,perk);
	if(texture=="")
	{
		SetNewGroupPicture(node,group,perk);
		return;
	}
	float left=0.0;
	float top=0.0;
	if(perk=="InstantRepair") left=0.5;
	if(group=="PERK_DISABLE") top=0.5;
	SetNewPicture(node,texture);
	// LEO: Сообщение PICTURE принимает UV в порядке left,right,top,bottom, в отличие от table icon.
	SendMessage(&GameInterface,"lslffff",MSG_INTERFACE_MSG_TO_NODE,node,1,left,left+0.5,top,top+0.5);
}
