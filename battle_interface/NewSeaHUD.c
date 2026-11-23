// LEO: Новый морской HUD: только построение атрибутов; старые координаты задаёт BattleInterface.c.
// LEO: Все готовые координаты и размеры ниже в экранных пикселях, ratio = Render.screen_y / screenscaling.
#define SEA_NAVIGATOR_MIN_SIZE 0.5
#define SEA_NAVIGATOR_MAX_SIZE 1.5

float NewSeaHUD_Clamp(float value,float low,float high)
{
	if(high<low) high=low;
	if(value<low) return low;
	if(value>high) return high;
	return value;
}

string NewSeaHUD_OptionKey(int index)
{
	string key="";
	switch(index)
	{
		case 0: key="SeaResourcesPosition"; break;
		case 1: key="SeaNavigatorX"; break;
		case 2: key="SeaNavigatorY"; break;
		case 3: key="SeaNavigatorSize"; break;
		case 4: key="SeaInfoOpacity"; break;
		case 5: key="SeaMapQuality"; break;
		case 6: key="SeaPerksSide"; break;
		case 7: key="SeaShipRenderMode"; break;
		case 8: key="SeaExpLogX"; break;
		case 9: key="SeaExpLogY"; break;
		case 10: key="SeaExpLogLines"; break;
		case 11: key="SeaAimReadout"; break;
	}
	return key;
}

float NewSeaHUD_Default(string key)
{
	if(key=="SeaResourcesPosition") return 0.5;
	if(key=="SeaNavigatorY") return 0.0;
	if(key=="SeaInfoOpacity") return 0.55;
	if(key=="SeaMapQuality") return 2.0;
	if(key=="SeaShipRenderMode") return 0.0;
	if(key=="SeaExpLogY") return 0.58;
	if(key=="SeaExpLogLines") return 10.0;
	if(key=="SeaAimReadout") return 0.0;
	return 1.0;
}

float NewSeaHUD_NavigatorSizeToSlider(float size)
{
	return (size-SEA_NAVIGATOR_MIN_SIZE)/(SEA_NAVIGATOR_MAX_SIZE-SEA_NAVIGATOR_MIN_SIZE);
}

float NewSeaHUD_NavigatorSizeFromSlider(float value)
{
	return SEA_NAVIGATOR_MIN_SIZE+(SEA_NAVIGATOR_MAX_SIZE-SEA_NAVIGATOR_MIN_SIZE)*value;
}

// LEO: Положение и непрозрачность 0..1; размер 0,50..1,50 (100% = 1,0).
float NewSeaHUD_GetOption(string key)
{
	float value=NewSeaHUD_Default(key);
	if(CheckAttribute(&InterfaceStates,key)) value=stf(InterfaceStates.(key));
	if(key=="SeaExpLogLines") return makefloat(ExpLog_ClampLines(value));
	if(key=="SeaNavigatorSize") return NewSeaHUD_Clamp(value,SEA_NAVIGATOR_MIN_SIZE,SEA_NAVIGATOR_MAX_SIZE);
	if(key=="SeaMapQuality" || key=="SeaPerksSide")
	{
		value=makeint(NewSeaHUD_Clamp(value,0.0,2.0));
		return value;
	}
	if(key=="SeaShipRenderMode" || key=="SeaAimReadout")
	{
		value=makeint(NewSeaHUD_Clamp(value,0.0,1.0));
		return value;
	}
	return NewSeaHUD_Clamp(value,0.0,1.0);
}

void NewSeaHUD_SaveOptions(ref options)
{
	for(int i=0;i<12;i++)
	{
		string key=NewSeaHUD_OptionKey(i);
		options.cameramode.(key)=NewSeaHUD_GetOption(key);
	}
}

void NewSeaHUD_LoadOptions(ref options)
{
	for(int i=0;i<12;i++)
	{
		string key=NewSeaHUD_OptionKey(i);
		InterfaceStates.(key)=NewSeaHUD_Default(key);
		if(CheckAttribute(options,"cameramode."+key)) InterfaceStates.(key)=stf(options.cameramode.(key));
		InterfaceStates.(key)=NewSeaHUD_GetOption(key);
	}
}

// LEO: Прицеп занимает место над ресурсами; строка прицела остаётся выше него и возвращается после отцепа.
void NewSeaHUD_PositionAimReadout(aref owner)
{
	if(!CheckAttribute(owner,"textinfo.DeckAimReadout.baseY")) return;
	int y=sti(owner.textinfo.DeckAimReadout.baseY);
	if(CheckAttribute(owner,"shiptowhud.visible") && CheckAttribute(owner,"shiptowhud.layout.top"))
	{
		if(sti(owner.shiptowhud.visible)!=0 && sti(owner.textinfo.DeckAimReadout.enabled)!=0)
		{
			int limit=sti(owner.shiptowhud.layout.top)-sti(owner.textinfo.DeckAimReadout.lineHeight);
			if(limit<y) y=limit;
		}
	}
	owner.textinfo.DeckAimReadout.pos.y=y;
}

void NewSeaHUD_ApplyAimReadout(aref owner)
{
	if(!CheckAttribute(owner,"textinfo.DeckAimReadout.enabled")) return;
	int enabled=makeint(NewSeaHUD_GetOption("SeaAimReadout"));
	owner.textinfo.DeckAimReadout.enabled=enabled;
	if(enabled==0) owner.textinfo.DeckAimReadout.text="";
	NewSeaHUD_PositionAimReadout(owner);
}

// LEO: Строка работает с любым морским HUD, ниже прицела и выше нижнего ряда ресурсов.
void NewSeaHUD_SetAimReadout(aref owner,int width,int height,float ratio)
{
	if(width<=0 || height<=0 || ratio<=0.0) return;
	owner.textinfo.DeckAimReadout.font="interface_normal";
	owner.textinfo.DeckAimReadout.scale=0.95*ratio;
	owner.textinfo.DeckAimReadout.align="center";
	owner.textinfo.DeckAimReadout.pos.x=width/2;
	owner.textinfo.DeckAimReadout.pos.y=makeint(height*0.875);
	owner.textinfo.DeckAimReadout.baseY=owner.textinfo.DeckAimReadout.pos.y;
	owner.textinfo.DeckAimReadout.lineHeight=makeint(32.0*ratio);
	owner.textinfo.DeckAimReadout.refreshable=1;
	owner.textinfo.DeckAimReadout.text="";
	owner.textinfo.DeckAimReadout.enabled=makeint(NewSeaHUD_GetOption("SeaAimReadout"));
	NewSeaHUD_PositionAimReadout(owner);
}

void NewSeaHUD_UpdateAimReadout(aref owner,int distance,int spreadWidth,int spreadLength)
{
	if(!CheckAttribute(owner,"textinfo.DeckAimReadout.enabled")) return;
	if(distance<=0 || sti(owner.textinfo.DeckAimReadout.enabled)==0)
	{
		if(owner.textinfo.DeckAimReadout.text!="") owner.textinfo.DeckAimReadout.text="";
		return;
	}
	string unit=XI_ConvertString("SeaAimReadoutMeter");
	owner.textinfo.DeckAimReadout.text=XI_ConvertString("SeaAimReadoutRange")+": "+distance+" "+unit+
		"  |  "+XI_ConvertString("SeaAimReadoutSpread")+": "+spreadWidth+" x "+spreadLength+" "+unit;
}

// LEO: До штатного построения удаляем только принадлежащие модулю добавления, в том числе после master off.
void NewSeaHUD_Clear(aref owner)
{
	DeleteAttribute(owner,"navigation.cannonsPosition");
	DeleteAttribute(owner,"navigation.cannonsSize");
	DeleteAttribute(owner,"navigation.mapTextureSize");
	DeleteAttribute(owner,"navigation.mapDetailMode");
	DeleteAttribute(owner,"navigation.mapCacheScale");
	DeleteAttribute(owner,"navigation.mapCacheRecenterThreshold");
	DeleteAttribute(owner,"navigation.mapCacheZoomThreshold");
	DeleteAttribute(owner,"navigation.mapCacheRefreshMs");
	DeleteAttribute(owner,"navigation.shipModels");
	DeleteAttribute(owner,"navigation.centerTexture");
	DeleteAttribute(owner,"navigation.centerScale");
	DeleteAttribute(owner,"navigation.compassScale");
	DeleteAttribute(owner,"navigation.compassBehindFrame");
	DeleteAttribute(owner,"navigation.fireRange");
	DeleteAttribute(owner,"navigation.circleSegments");
	DeleteAttribute(owner,"navigation.compassLabels");
	DeleteAttribute(owner,"navigation.compassDirections");
	DeleteAttribute(owner,"navigation.compassFont");
	DeleteAttribute(owner,"navigation.compassFontScale");
	DeleteAttribute(owner,"navigation.compassFontColor");
	DeleteAttribute(owner,"navigation.compassLabelRadius");
	DeleteAttribute(owner,"navigation.compassLabelYOffset");
	DeleteAttribute(owner,"navigation.compassNorthColor");
	DeleteAttribute(owner,"navigation.compassShadow");
	DeleteAttribute(owner,"navigation.cannonPictureGroup");
	DeleteAttribute(owner,"navigation.cannonPicture");
	DeleteAttribute(owner,"navigation.cannonIconSize");
	DeleteAttribute(owner,"navigation.cannonSeparateSectors");
	DeleteAttribute(owner,"textinfo.Date.maxWidth");
	DeleteAttribute(owner,"textinfo.Date.maxLines");
	DeleteAttribute(owner,"textinfo.Date.lineHeight");
	DeleteAttribute(owner,"textinfo.Location.maxWidth");
	DeleteAttribute(owner,"textinfo.Location.maxLines");
	DeleteAttribute(owner,"textinfo.Location.lineHeight");
	DeleteAttribute(owner,"newseahud");
	HUDLayout_RemoveObject(owner,"sea_cannons");
	HUDLayout_RemoveObject(owner,"sea_info");
}

void NewSeaHUD_SetNavigator(aref owner,int width,int height,float ratio)
{
	float size=260.0*ratio*NewSeaHUD_GetOption("SeaNavigatorSize");
	float margin=40.0*ratio;
	float verticalMargin=4.0*ratio;
	float footer=122.0*ratio;
	float header=14.0*ratio;
	size=NewSeaHUD_Clamp(size,1.0,width-2.0*margin);
	size=NewSeaHUD_Clamp(size,1.0,height-footer-header-2.0*verticalMargin);
	float x=margin+(width-size-2.0*margin)*NewSeaHUD_GetOption("SeaNavigatorX");
	float y=header+verticalMargin+(height-size-footer-header-2.0*verticalMargin)*NewSeaHUD_GetOption("SeaNavigatorY");
	x=NewSeaHUD_Clamp(x+HUDLayout_GetOffset("sea","navigator","x"),margin,width-size-margin);
	y=NewSeaHUD_Clamp(y+HUDLayout_GetOffset("sea","navigator","y"),header+verticalMargin,height-size-footer-verticalMargin);
	owner.navigation.navigatorWidth=makeint(size);
	owner.navigation.navigatorHeight=makeint(size);
	owner.navigation.aspectRatio=1.0;
	owner.navigation.rightPos=makeint(x+size);
	owner.navigation.topPos=makeint(y);

	owner.newseahud.quickActions.left=makeint(NewSeaHUD_Clamp(x-36.0*ratio,4.0*ratio,width-52.0*ratio));
	owner.newseahud.quickActions.top=makeint(y+8.0*ratio);

	owner.navigation.mapRadius=makeint(size*0.394+0.5);

	owner.navigation.mapTextureSize=makeint(1024*pow(2.0,NewSeaHUD_GetOption("SeaMapQuality")));

	owner.navigation.mapDetailMode=1;
	owner.navigation.mapCacheScale=3.0;
	owner.navigation.mapCacheRecenterThreshold=0.75;
	owner.navigation.mapCacheZoomThreshold=1.25;
	owner.navigation.mapCacheRefreshMs=150;

	owner.navigation.shipModels.mode=makeint(NewSeaHUD_GetOption("SeaShipRenderMode"));
	owner.navigation.shipModels.textureSize=512;
	owner.navigation.shipModels.scale=2.0;
	owner.navigation.shipModels.maxShips=32;
	owner.navigation.shipModels.markerRadius=3.0*ratio;
	owner.navigation.shipModels.markerOffset=5.0*ratio;
	owner.navigation.shipModels.rimColor=argb(255,219,193,123);
	owner.navigation.shipModels.friendColor=argb(255,74,159,245);
	owner.navigation.shipModels.allyColor=argb(255,54,214,94);
	owner.navigation.shipModels.enemyColor=argb(255,243,74,64);
	owner.navigation.shipModels.neutralColor=argb(255,146,153,156);
	owner.navigation.shipModels.deadColor=argb(255,114,114,114);

	owner.navigation.fireRange.readyColor=argb(37,0,255,255);
	owner.navigation.fireRange.reloadColor=argb(37,128,0,0);
	owner.navigation.fireRange.centerAlpha=0.03;
	owner.navigation.fireRange.radialPower=1.0;
	owner.navigation.fireRange.pulseHz=1.25;
	owner.navigation.fireRange.minAlpha=0.20;
	owner.navigation.fireRange.heightOffset=0.35;
	owner.navigation.fireRange.depthBias=-0.000001;
	owner.navigation.fireRange.slopeDepthBias=-1.0;
	owner.navigation.fireRange.segments=24;
	owner.navigation.fireRange.radialSegments=16;

	owner.navigation.circleSegments=96;
	owner.navigation.compasTexture="battle_interface\NewHUD\Navigation\Compass.dds";
	owner.navigation.compassScale=1.03;
	owner.navigation.compassBehindFrame=1;
	owner.navigation.emptyTexture="battle_interface\NewHUD\Navigation\Frame.dds";

	owner.navigation.centerTexture="battle_interface\NewHUD\Navigation\Center.dds";
	owner.navigation.centerScale=0.90;
	owner.navigation.cannonsTexture="battle_interface\NewHUD\Navigation\ReloadMask.dds";

	owner.navigation.compassLabels=1;
	int compassSize=makeint(13.0*size/260.0+0.5);
	if(compassSize<8) compassSize=8;
	if(compassSize>40) compassSize=40;

	owner.navigation.compassFont="newhud_compass_"+compassSize;
	owner.navigation.compassFontScale=1.0;
	owner.navigation.compassFontColor=argb(255,255,255,255);
	owner.navigation.compassNorthColor=argb(255,245,65,55);
	owner.navigation.compassShadow=1;
	owner.navigation.compassLabelRadius=size*0.375;
	owner.navigation.compassLabelYOffset=-makefloat(compassSize)*0.5;
	owner.navigation.windWidth=makeint(20.0*size/232.0);
	owner.navigation.windHeight=makeint(122.0*size/232.0);
	owner.navigation.sailstatePos=makeint(23.0*ratio)+","+makeint(size*0.5+27.0*ratio);
	owner.navigation.windPos=makeint(-23.0*ratio)+","+makeint(size*0.5+27.0*ratio);
	owner.navigation.sailstatePictureSize=makeint(32.0*ratio)+","+makeint(32.0*ratio);
	owner.navigation.windPictureSize=makeint(32.0*ratio)+","+makeint(32.0*ratio);

	owner.navigation.speedShowFont="interface_normal";
	owner.navigation.fontScale=1.0*ratio;
	owner.navigation.speedOutYOffset=makeint(size*0.5+47.0*ratio);
	owner.navigation.shipSpeedXOffset=makeint(23.0*ratio);
	owner.navigation.windSpeedXOffset=makeint(-23.0*ratio);
	owner.textinfo.Speed.pos.x=makeint(x+size*0.5);
	owner.textinfo.Speed.pos.y=makeint(y+size+47.0*ratio);
	float infoWidth=NewSeaHUD_Clamp(180.0*ratio,1.0,width-2.0*margin);
	float infoLeft=NewSeaHUD_Clamp(x+(size-infoWidth)*0.5,margin,width-infoWidth-margin);
	owner.newseahud.infoleft=infoLeft;
	owner.newseahud.inforight=infoLeft+infoWidth;
	owner.newseahud.infotop=y+size+70.0*ratio;
	owner.newseahud.infobottom=y+size+116.0*ratio;
	// LEO: Габариты навигатора вместе с подписями доступны скриптам раскладки.
	owner.newseahud.reserved.left=x-22.0*ratio;
	owner.newseahud.reserved.right=x+size+22.0*ratio;
	owner.newseahud.reserved.top=y-header;
	owner.newseahud.reserved.bottom=y+size+116.0*ratio;
}

// LEO: Размещение быстрого действия и обеих подсказок без создания сущностей и кадрового опроса.
void NewSeaHUD_SetQuickAction(aref actions,aref layout,float ratio)
{
	if(NewHUD_GetOption("NewHUDEnabled")==0 || ratio<=0.0) return;
	if(!CheckAttribute(layout,"newseahud.quickActions")) return;
	int left=sti(layout.newseahud.quickActions.left);
	int top=sti(layout.newseahud.quickActions.top);
	actions.left=left;
	actions.top=top;
	actions.text1.pos.x=left+makeint(24.0*ratio);
	actions.text1.pos.y=top+makeint(54.0*ratio);
	actions.text2.pos.x=actions.text1.pos.x;
	actions.text2.pos.y=actions.text1.pos.y;
}

// LEO: Границы видимой части лога задают также якорь перков в режиме «Под логом».
void NewSeaHUD_SetExperienceLog(aref owner,int width,int height,float ratio)
{
	float margin=8.0*ratio;
	float sideMargin=72.0*ratio;
	float logWidth=NewSeaHUD_Clamp(240.0*ratio,1.0,width-2.0*sideMargin);
	int count=makeint(NewSeaHUD_GetOption("SeaExpLogLines"));
	float reserve=0.0;
	if(NewSeaHUD_GetOption("SeaPerksSide")==2.0) reserve=86.0*ratio;
	float step=NewSeaHUD_Clamp(20.0*ratio,1.0,(height-2.0*margin-reserve)/count);
	float logHeight=step*count;
	float left=sideMargin+(width-logWidth-2.0*sideMargin)*NewSeaHUD_GetOption("SeaExpLogX");
	float top=margin+(height-logHeight-2.0*margin)*NewSeaHUD_GetOption("SeaExpLogY");
	left=NewSeaHUD_Clamp(left+HUDLayout_GetOffset("sea","experience","x"),margin,width-logWidth-margin);
	top=NewSeaHUD_Clamp(top+HUDLayout_GetOffset("sea","experience","y"),margin,height-logHeight-reserve-margin);
	owner.newseahud.explog.left=left;
	owner.newseahud.explog.right=left+logWidth;
	owner.newseahud.explog.top=top;
	owner.newseahud.explog.bottom=top+logHeight;
	owner.newseahud.explog.lineHeight=step;
	for(int i=1;i<=count;i++)
	{
		string key="EXP"+i;
		owner.textinfo.(key).pos.x=makeint(left+logWidth);
		owner.textinfo.(key).pos.y=makeint(top+(i-1)*step);
	}
}

void NewSeaHUD_SetResources(aref owner,int width,int height,float ratio)
{
	float margin=8.0*ratio;
	bool underLog=NewSeaHUD_GetOption("SeaPerksSide")==2.0;
	// LEO: При переносе перков под лог положение самого ряда расходников сохраняется.
	float groupWidth=628.0*ratio;
	float fit=1.0;
	if(groupWidth>width-2.0*margin) fit=(width-2.0*margin)/groupWidth;
	float unit=ratio*fit;
	float rowWidth=424.0*unit;
	groupWidth=628.0*unit;
	float groupLeft=margin+(width-groupWidth-2.0*margin)*NewSeaHUD_GetOption("SeaResourcesPosition");
	groupLeft=NewSeaHUD_Clamp(groupLeft+HUDLayout_GetOffset("sea","resources","x"),margin,width-groupWidth-margin);
	float left=groupLeft;
	if(NewSeaHUD_GetOption("SeaPerksSide")==0.0) left=left+204.0*unit;
	float top=height-54.0*unit+HUDLayout_GetOffset("sea","resources","y");
	top=NewSeaHUD_Clamp(top,214.0*unit,height-54.0*unit);
	float perksLeft=groupLeft;
	if(NewSeaHUD_GetOption("SeaPerksSide")!=0.0) perksLeft=left+rowWidth+12.0*unit;
	float perksTop=top-200.0*unit;
	float perksWidth=192.0*unit;
	float perksHeight=240.0*unit;
	if(underLog)
	{
		// LEO: Ширина и размеры старого вертикального блока; меняется только его привязка к логу.
		perksWidth=RecalculateHIcon(makeint(180.0*ratio))-RecalculateHIcon(makeint(48.0*ratio));
		perksLeft=stf(owner.newseahud.explog.right)-perksWidth;
		float minTop=stf(owner.newseahud.explog.bottom)+8.0*ratio;
		float bottom=height-RecalculateVIcon(makeint(28.0*ratio));
		perksTop=NewSeaHUD_Clamp(minTop+HUDLayout_GetOffset("sea","perks","y"),minTop,bottom-RecalculateVIcon(makeint(48.0*ratio)));
		perksHeight=bottom-perksTop;
	}
	perksLeft=NewSeaHUD_Clamp(perksLeft+HUDLayout_GetOffset("sea","perks","x"),margin,width-perksWidth-margin);
	if(!underLog) perksTop=NewSeaHUD_Clamp(perksTop+HUDLayout_GetOffset("sea","perks","y"),18.0*unit,height-perksHeight-margin);
	owner.newseahud.perks.left=perksLeft;
	owner.newseahud.perks.right=perksLeft+perksWidth;
	owner.newseahud.perks.top=perksTop;
	owner.newseahud.perks.bottom=perksTop+perksHeight;
	owner.newseahud.perks.unit=unit;
	owner.newseahud.perks.underLog=underLog;
	for(int i=0;i<9;i++)
	{
		float x=left+i*48.0*unit;
		string icon="ammoicon"+i;
		string count="ammocount"+i;
		owner.imageslist.(icon).pos=makeint(x)+","+makeint(top)+","+makeint(x+40.0*unit)+","+makeint(top+40.0*unit);
		owner.textinfo.(count).pos.x=makeint(x+20.0*unit);
		owner.textinfo.(count).pos.y=makeint(top-15.0*unit);
		if(i<4)
		{
			string slot="slot"+i;
			owner.ammomark.(slot)=makeint(x+11.0*unit)+","+makeint(top+41.0*unit)+","+makeint(x+29.0*unit)+","+makeint(top+52.0*unit);
		}
	}
	owner.imageslist.ammomark.pos=owner.ammomark.slot0;
}

void NewSeaHUD_SetCannons(aref owner,int width,int height,float ratio)
{
	string labels[4]; labels[0]="CannonsNumL";labels[1]="CannonsNumR";labels[2]="CannonsNumF";labels[3]="CannonsNumB";
	for(int i=0;i<4;i++)
	{
		string label=labels[i];
		owner.textinfo.(label).font="interface_normal";
		owner.textinfo.(label).scale=0.90*ratio;
		owner.textinfo.(label).color=argb(255,255,255,255);
		owner.textinfo.(label).shadow=1;
	}
	float size=stf(owner.navigation.navigatorWidth);
	float x=stf(owner.navigation.rightPos)-size*0.5;
	float y=stf(owner.navigation.topPos)+size*0.5;
	owner.navigation.cannonsPosition=makeint(x)+","+makeint(y);
	owner.navigation.cannonsSize=makeint(size);
	owner.textinfo.CannonsNumL.pos.x=makeint(x-size*0.51-10.0*ratio);
	owner.textinfo.CannonsNumL.pos.y=makeint(y-8.0*ratio);
	owner.textinfo.CannonsNumR.pos.x=makeint(x+size*0.51+10.0*ratio);
	owner.textinfo.CannonsNumR.pos.y=makeint(y-8.0*ratio);
	owner.textinfo.CannonsNumF.pos.x=makeint(x);
	owner.textinfo.CannonsNumF.pos.y=makeint(y-size*0.50-14.0*ratio);
	owner.textinfo.CannonsNumB.pos.x=makeint(x);
	owner.textinfo.CannonsNumB.pos.y=makeint(y+size*0.49);
}

void NewSeaHUD_SetInfo(aref owner,int width,int height,float ratio)
{
	owner.textinfo.Date.font="interface_normal";
	owner.textinfo.Date.scale=0.80*ratio;
	owner.textinfo.Location.font="interface_normal";
	owner.textinfo.Location.scale=0.80*ratio;
	float left=stf(owner.newseahud.infoleft);
	float right=stf(owner.newseahud.inforight);
	float top=stf(owner.newseahud.infotop);
	float bottom=stf(owner.newseahud.infobottom);
	string rect=makeint(left)+","+makeint(top)+","+makeint(right)+","+makeint(bottom);
	int color=argb(makeint(255.0*NewSeaHUD_GetOption("SeaInfoOpacity")),255,255,255);
	HUDLayout_SetImage(owner,"sea_info","battle_interface\NewHUD\Navigation\InfoBacking.dds",rect,color,"0,0,1,1");
	owner.textinfo.Date.pos.x=makeint((left+right)*0.5);
	owner.textinfo.Date.pos.y=makeint(top+3.0*ratio);
	owner.textinfo.Date.maxWidth=makeint(right-left-16.0*ratio);
	owner.textinfo.Date.maxLines=1;
	owner.textinfo.Date.lineHeight=makeint(14.0*ratio);
	owner.textinfo.Location.pos.x=makeint((left+right)*0.5);
	owner.textinfo.Location.pos.y=makeint(top+17.0*ratio);
	owner.textinfo.Location.maxWidth=makeint(right-left-16.0*ratio);
	owner.textinfo.Location.maxLines=2;
	owner.textinfo.Location.lineHeight=makeint(14.0*ratio);
}

// LEO: Вызывается один раз ПОСЛЕ штатных атрибутов, ДО CreateEntity; никакой работы в кадровом тике.
void NewSeaHUD_SetParameters(aref owner,int width,int height,float ratio)
{
	if(NewHUD_GetOption("NewHUDEnabled")==0) return;
	if(width<=0 || height<=0 || ratio<=0.0) return;
	NewSeaHUD_SetNavigator(owner,width,height,ratio);
	NewSeaHUD_SetExperienceLog(owner,width,height,ratio);
	NewSeaHUD_SetResources(owner,width,height,ratio);
	NewSeaHUD_SetCannons(owner,width,height,ratio);
	NewSeaHUD_SetInfo(owner,width,height,ratio);
	// LEO: SeaAimReadout меняется на лету, отдельно от снимка параметров раскладки.
	for(int i=0;i<11;i++)
	{
		string key=NewSeaHUD_OptionKey(i);
		owner.newseahud.options.(key)=NewSeaHUD_GetOption(key);
	}
}

bool NewSeaHUD_LayoutChanged(aref owner)
{
	// LEO: Отмена настроек загружает старое значение, не вызывая обработчик флажка.
	if(CheckAttribute(owner,"textinfo.DeckAimReadout.enabled"))
	{
		if(sti(owner.textinfo.DeckAimReadout.enabled)!=makeint(NewSeaHUD_GetOption("SeaAimReadout"))) return true;
	}
	if(!CheckAttribute(owner,"expLogLines")) return true;
	if(sti(owner.expLogLines)!=makeint(NewSeaHUD_GetOption("SeaExpLogLines"))) return true;
	if(NewHUD_GetOption("NewHUDEnabled")==0) return CheckAttribute(owner,"newseahud.options");
	if(!CheckAttribute(owner,"newseahud.options")) return true;
	for(int i=0;i<11;i++)
	{
		string key=NewSeaHUD_OptionKey(i);
		if(!CheckAttribute(owner,"newseahud.options."+key)) return true;
		if(stf(owner.newseahud.options.(key))!=NewSeaHUD_GetOption(key)) return true;
	}
	return false;
}
