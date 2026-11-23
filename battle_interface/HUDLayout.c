// LEO: Смещения ЦЕЛЫХ блоков, не внутренних значков. X вправо, Y вниз.
// LEO: Единицы как при базовой высоте screenscaling (обычно 900); обе оси масштабируются по высоте экрана.
// LEO: Профили независимы: land, sea, worldmap. Нулевые значения сохраняют штатную раскладку любого стиля.
// LEO: Пример: HUDLayout_SetOffset("land","hero",20,-10); HUDLayout_SetOffset("land","party",40,0);
// LEO: Затем HUDLayout_Reload(); штатная пересборка закрывает командное меню, как смена оформления.
// LEO: Настройки живут в InterfaceStates.HUDLayout; функции построения всегда начинают со штатной базы.
bool HUDLayout_IsProfile(string profile)
{
	if(profile=="land" || profile=="sea" || profile=="worldmap") return true;
	return false;
}

bool HUDLayout_IsBlock(string block)
{
	if(block=="hero" || block=="party" || block=="commands" || block=="perks" || block=="experience") return true;
	if(block=="consumables_top" || block=="consumables_bottom") return true;
	if(block=="resources" || block=="navigator" || block=="radar") return true;
	return false;
}

// LEO: Здесь можно задать исходные смещения; уже заданные через API значения не затираем.
void HUDLayout_Init()
{
	string profiles[3]; profiles[0]="land";profiles[1]="sea";profiles[2]="worldmap";
	string blocks[10]; blocks[0]="hero";blocks[1]="party";blocks[2]="commands";
	blocks[3]="consumables_top";blocks[4]="consumables_bottom";blocks[5]="resources";
	blocks[6]="navigator";blocks[7]="radar";blocks[8]="perks";
	blocks[9]="experience";
	for(int p=0;p<3;p++)
	{
		string profile=profiles[p];
		for(int b=0;b<10;b++)
		{
			string block=blocks[b];
			if(!CheckAttribute(&InterfaceStates,"HUDLayout."+profile+"."+block+".x")) InterfaceStates.HUDLayout.(profile).(block).x=0;
			if(!CheckAttribute(&InterfaceStates,"HUDLayout."+profile+"."+block+".y")) InterfaceStates.HUDLayout.(profile).(block).y=0;
		}
	}
}

bool HUDLayout_SetOffset(string profile,string block,float dx,float dy)
{
	if(!HUDLayout_IsProfile(profile) || !HUDLayout_IsBlock(block)) return false;
	InterfaceStates.HUDLayout.(profile).(block).x=dx;
	InterfaceStates.HUDLayout.(profile).(block).y=dy;
	return true;
}

// LEO: Координаты точки экрана задаются долями; отступы и шаг ряда — в единицах screenscaling.
bool HUDLayout_SetPortraitRow(string profile,float x,float y,float dx,float dy,float step)
{
	if(!HUDLayout_IsProfile(profile) || x<0.0 || x>1.0 || y<0.0 || y>1.0 || step<=0.0) return false;
	InterfaceStates.HUDLayout.(profile).portraitrow.x=x;
	InterfaceStates.HUDLayout.(profile).portraitrow.y=y;
	InterfaceStates.HUDLayout.(profile).portraitrow.dx=dx;
	InterfaceStates.HUDLayout.(profile).portraitrow.dy=dy;
	InterfaceStates.HUDLayout.(profile).portraitrow.step=step;
	return true;
}

void HUDLayout_ClearPortraitRow(string profile)
{
	if(HUDLayout_IsProfile(profile)) DeleteAttribute(&InterfaceStates,"HUDLayout."+profile+".portraitrow");
}

bool HUDLayout_SetCommandAnchor(string profile,string mode,string align,float x,float y)
{
	if(!HUDLayout_IsProfile(profile)) return false;
	int anchor=0;
	if(mode=="screen") anchor=1;
	else if(mode=="portrait") anchor=2;
	else if(mode!="legacy") return false;
	int alignment=0;
	if(align=="center") alignment=1;
	else if(align=="right") alignment=2;
	else if(align!="left") return false;
	if(anchor==1)
	{
		if(x<0.0 || x>1.0 || y<0.0 || y>1.0) return false;
	}
	InterfaceStates.HUDLayout.(profile).commands.anchor=anchor;
	InterfaceStates.HUDLayout.(profile).commands.align=alignment;
	InterfaceStates.HUDLayout.(profile).commands.screenx=x;
	InterfaceStates.HUDLayout.(profile).commands.screeny=y;
	return true;
}

// LEO: Нулевая пара ширины и высоты восстанавливает штатные размеры, шаг и число видимых иконок.
bool HUDLayout_SetCommandIcons(string profile,float width,float height,float gap,int limit)
{
	if(!HUDLayout_IsProfile(profile)) return false;
	if(width==0.0 && height==0.0)
	{
		DeleteAttribute(&InterfaceStates,"HUDLayout."+profile+".commands.icons");
		return true;
	}
	if(width<1.0 || width>300.0 || height<1.0 || height>300.0 || gap<0.0 || gap>100.0 || limit<1 || limit>32) return false;
	InterfaceStates.HUDLayout.(profile).commands.icons.width=width;
	InterfaceStates.HUDLayout.(profile).commands.icons.height=height;
	InterfaceStates.HUDLayout.(profile).commands.icons.gap=gap;
	InterfaceStates.HUDLayout.(profile).commands.icons.limit=limit;
	return true;
}

// LEO: Возвращает экранные пиксели; существующие pos/offset уже масштабированы, повторно Recalculate не нужен.
int HUDLayout_GetOffset(string profile,string block,string axis)
{
	if(!HUDLayout_IsProfile(profile) || !HUDLayout_IsBlock(block)) return 0;
	if(axis!="x" && axis!="y") return 0;
	if(!CheckAttribute(&InterfaceStates,"HUDLayout."+profile+"."+block+"."+axis)) return 0;
	if(screenscaling<=0) return 0;
	return makeint(stf(InterfaceStates.HUDLayout.(profile).(block).(axis))*stf(Render.screen_y)/makefloat(screenscaling));
}

string HUDLayout_Context(bool sea)
{
	if(IsEntity(&worldMap)) return "worldmap";
	if(sea) return "sea";
	return "land";
}

string HUDLayout_ShiftPoint(string point,int dx,int dy)
{
	if(dx==0 && dy==0) return point;
	int x=sti(GetSubStringByNum(point,0))+dx;
	int y=sti(GetSubStringByNum(point,1))+dy;
	return x+","+y;
}

// LEO: fresh=true только ПОСЛЕ штатного построения слотов; повторное применение использует сохранённую базу.
void HUDLayout_ApplySigns(aref signs,string profile,int slots,bool fresh)
{
	if(fresh) DeleteAttribute(signs,"hudlayoutbase");
	bool row=CheckAttribute(&InterfaceStates,"HUDLayout."+profile+".portraitrow.step") && screenscaling>0;
	float rowX=0.0;
	float rowY=0.0;
	float rowStep=0.0;
	if(row)
	{
		float unit=stf(Render.screen_y)/makefloat(screenscaling);
		aref rowConfig;
		makearef(rowConfig,InterfaceStates.HUDLayout.(profile).portraitrow);
		rowX=stf(Render.screen_x)*stf(rowConfig.x)+stf(rowConfig.dx)*unit;
		rowY=stf(Render.screen_y)*stf(rowConfig.y)+stf(rowConfig.dy)*unit;
		rowStep=stf(rowConfig.step)*unit;
	}
	for(int i=1;i<=slots;i++)
	{
		string key="iconoffset"+i;
		if(!CheckAttribute(signs,key)) continue;
		if(!CheckAttribute(signs,"hudlayoutbase."+key)) signs.hudlayoutbase.(key)=signs.(key);
		string block="party";
		if(i==1) block="hero";
		string point=signs.hudlayoutbase.(key);
		if(row) point=makeint(rowX+(i-1)*rowStep)+","+makeint(rowY);
		signs.(key)=HUDLayout_ShiftPoint(point,HUDLayout_GetOffset(profile,block,"x"),HUDLayout_GetOffset(profile,block,"y"));
	}
}

// LEO: Штатный режим использует Y портрета; новые режимы привязываются к экрану или к центру портрета.
void HUDLayout_SetCommands(aref commands,string profile,int heroIndex)
{
	commands.hudlayout.heroindex=heroIndex;
	commands.hudlayout.herox=HUDLayout_GetOffset(profile,"hero","x");
	commands.hudlayout.partyx=HUDLayout_GetOffset(profile,"party","x");
	commands.hudlayout.x=HUDLayout_GetOffset(profile,"commands","x");
	commands.hudlayout.y=HUDLayout_GetOffset(profile,"commands","y");
	commands.hudlayout.anchor=0;
	commands.hudlayout.align=0;
	commands.hudlayout.screenx=0;
	commands.hudlayout.screeny=0;
	commands.hudlayout.screenwidth=sti(Render.screen_x);
	DeleteAttribute(commands,"hudlayout.iconlimit");
	if(CheckAttribute(&InterfaceStates,"HUDLayout."+profile+".commands.anchor"))
	{
		aref config;
		makearef(config,InterfaceStates.HUDLayout.(profile).commands);
		commands.hudlayout.anchor=sti(config.anchor);
		commands.hudlayout.align=sti(config.align);
		commands.hudlayout.screenx=makeint(stf(config.screenx)*stf(Render.screen_x));
		commands.hudlayout.screeny=makeint(stf(config.screeny)*stf(Render.screen_y));
	}
	if(CheckAttribute(&InterfaceStates,"HUDLayout."+profile+".commands.icons.limit") && screenscaling>0)
	{
		aref icons;
		makearef(icons,InterfaceStates.HUDLayout.(profile).commands.icons);
		float unit=stf(Render.screen_y)/makefloat(screenscaling);
		commands.CommandIconWidth=makeint(stf(icons.width)*unit);
		commands.CommandIconHeight=makeint(stf(icons.height)*unit);
		commands.CommandIconSpace=makeint(stf(icons.gap)*unit);
		if(sti(commands.CommandIconWidth)<1) commands.CommandIconWidth=1;
		if(sti(commands.CommandIconHeight)<1) commands.CommandIconHeight=1;
		int fit=makeint((stf(Render.screen_x)+stf(commands.CommandIconSpace))/(stf(commands.CommandIconWidth)+stf(commands.CommandIconSpace)));
		int limit=sti(icons.limit);
		if(limit>fit) limit=fit;
		if(limit<1) limit=1;
		commands.CommandMaxIconQuantity=limit;
		commands.hudlayout.iconlimit=limit;
	}
}

void HUDLayout_ShiftText(aref owner,string key,int dx,int dy)
{
	if(dx==0 && dy==0) return;
	if(!CheckAttribute(owner,"textinfo."+key+".pos.x")) return;
	owner.textinfo.(key).pos.x=sti(owner.textinfo.(key).pos.x)+dx;
	owner.textinfo.(key).pos.y=sti(owner.textinfo.(key).pos.y)+dy;
}

// LEO: Навигатор включает связанные цифры скорости и бортовых орудий; размеры и масштаб не меняем.
void HUDLayout_ApplyNavigator(aref owner)
{
	if(NewHUD_GetOption("NewHUDEnabled")!=0) return;
	int dx=HUDLayout_GetOffset("sea","navigator","x");
	int dy=HUDLayout_GetOffset("sea","navigator","y");
	if(dx==0 && dy==0) return;
	owner.navigation.rightPos=sti(owner.navigation.rightPos)+dx;
	owner.navigation.topPos=sti(owner.navigation.topPos)+dy;
	HUDLayout_ShiftText(owner,"Speed",dx,dy);
	HUDLayout_ShiftText(owner,"CannonsNumL",dx,dy);
	HUDLayout_ShiftText(owner,"CannonsNumR",dx,dy);
	HUDLayout_ShiftText(owner,"CannonsNumF",dx,dy);
	HUDLayout_ShiftText(owner,"CannonsNumB",dx,dy);
}

// LEO: Объекты принадлежат owner, имена изолированы префиксом; применяются при штатном создании imageslist/textinfo.
void HUDLayout_SetImage(aref owner,string id,string texture,string rect,int color,string uv)
{
	if(CheckAttribute(owner,"hudlayoutobjects."+id+".slices")) HUDLayout_RemoveObject(owner,id);
	string key="hudcustom_"+id;
	owner.imageslist.(key).texture=texture;
	owner.imageslist.(key).pos=rect;
	owner.imageslist.(key).color=color;
	owner.imageslist.(key).uv=uv;
	owner.hudlayoutobjects.(id).image=1;
}

// LEO: Углы и поперечная толщина рамки масштабируются одинаково; растягивается только середина полос.
void HUDLayout_SetImage9Slice(aref owner,string id,string texture,string rect,int color,int textureWidth,int textureHeight,int sourceBorder,int border)
{
	HUDLayout_RemoveObject(owner,id);
	if(textureWidth<=0 || textureHeight<=0 || sourceBorder<=0) return;
	if(2*sourceBorder>=textureWidth || 2*sourceBorder>=textureHeight) return;
	int x[4];
	int y[4];
	float u[4];
	float v[4];
	x[0]=sti(GetSubStringByNum(rect,0));
	y[0]=sti(GetSubStringByNum(rect,1));
	x[3]=sti(GetSubStringByNum(rect,2));
	y[3]=sti(GetSubStringByNum(rect,3));
	if(x[3]<=x[0] || y[3]<=y[0]) return;
	if(border<1) border=1;
	if(2*border>x[3]-x[0]) border=makeint((x[3]-x[0])/2);
	if(2*border>y[3]-y[0]) border=makeint((y[3]-y[0])/2);
	x[1]=x[0]+border; x[2]=x[3]-border;
	y[1]=y[0]+border; y[2]=y[3]-border;
	u[0]=0.0; u[1]=makefloat(sourceBorder)/makefloat(textureWidth); u[2]=1.0-u[1]; u[3]=1.0;
	v[0]=0.0; v[1]=makefloat(sourceBorder)/makefloat(textureHeight); v[2]=1.0-v[1]; v[3]=1.0;
	for(int row=0;row<3;row++)
	{
		for(int column=0;column<3;column++)
		{
			if(x[column+1]<=x[column] || y[row+1]<=y[row]) continue;
			string part=id+"_slice"+(row*3+column);
			if(row==1 && column==1) part=id;
			string cell=x[column]+","+y[row]+","+x[column+1]+","+y[row+1];
			string uv=u[column]+","+v[row]+","+u[column+1]+","+v[row+1];
			HUDLayout_SetImage(owner,part,texture,cell,color,uv);
			if(part!=id) DeleteAttribute(owner,"hudlayoutobjects."+part);
		}
	}
	owner.hudlayoutobjects.(id).slices=9;
	owner.hudlayoutobjects.(id).rect=rect;
}

// LEO: X/Y — экранные пиксели, scale — множитель шрифта; для сменяемой строки установлен refreshable.
void HUDLayout_SetText(aref owner,string id,string text,string font,float scale,int x,int y,int color,string align)
{
	string key="hudcustom_"+id;
	owner.textinfo.(key).text=text;
	owner.textinfo.(key).font=font;
	owner.textinfo.(key).scale=scale;
	owner.textinfo.(key).pos.x=x;
	owner.textinfo.(key).pos.y=y;
	owner.textinfo.(key).color=color;
	owner.textinfo.(key).align=align;
	owner.textinfo.(key).refreshable=true;
	owner.hudlayoutobjects.(id).text=1;
}

void HUDLayout_RemoveObject(aref owner,string id)
{
	if(CheckAttribute(owner,"hudlayoutobjects."+id+".slices"))
	{
		for(int i=0;i<9;i++) DeleteAttribute(owner,"imageslist.hudcustom_"+id+"_slice"+i);
	}
	DeleteAttribute(owner,"imageslist.hudcustom_"+id);
	DeleteAttribute(owner,"textinfo.hudcustom_"+id);
	DeleteAttribute(owner,"hudlayoutobjects."+id);
}

// LEO: Удаляем только зарегистрированные здесь объекты, не затрагивая штатные значки и подписи.
void HUDLayout_ClearObjects(aref owner)
{
	if(!CheckAttribute(owner,"hudlayoutobjects")) return;
	aref list;
	makearef(list,owner.hudlayoutobjects);
	for(int i=GetAttributesNum(list)-1;i>=0;i--)
	{
		string id=GetAttributeName(GetAttributeN(list,i));
		HUDLayout_RemoveObject(owner,id);
	}
}

// LEO: Перезагрузка только уже существующего интерфейса штатным путём; из главного меню ничего не создаём.
// LEO: Как при смене стиля, закрывается команда и может сброситься ускорение времени; активные перки пересоздаются.
void HUDLayout_Reload()
{
	if(IsEntity(&worldMap))
	{
		if(IsEntity(&BattleInterface))
		{
			WM_RebuildInterface();
			HUDLayout_ReloadPerks();
		}
		return;
	}
	bool exists=IsEntity(&objLandInterface);
	if(bSeaActive && !bAbordageStarted) exists=IsEntity(&BattleInterface);
	if(!exists) return;
	ChangeShowIntarface();
	ChangeShowIntarface();
	HUDLayout_ReloadPerks();
	// LEO: При переходе из меню HUD уже может существовать, а loadedLocation ещё не назначена.
	if(!bSeaActive || bAbordageStarted)
	{
		if(FindLoadedLocation()>=0) LandRadar_SendParams();
	}
}

void HUDLayout_ReloadPerks()
{
	if(!IsEntity(&objActivePerkShower)) return;
	UnloadActivePerkShower();
	InitActivePerkShower();
	LoadActivePerkShower();
}
