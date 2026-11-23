// LEO: Только представление буксира ГГ; реестр и физика SeaTow остаются у морского runtime.
#define SHIP_TOW_HUD_UPDATE_MS 250
int ShipTowHUDGeneration=0;
int ShipTowHUDLastSecond=0;
int ShipTowHUDLastMs=0;

void ShipTowHUD_Collect(ref snapshot)
{
	DeleteAttribute(snapshot,"");
	snapshot.count=0;
	snapshot.signature="";
	if(!bSeaActive || bAbordageStarted || bSeaReloadStarted) return;
	if(!SeaTow_HasLinks()) return;
	if(LAi_IsDead(pchar)) return;
	aref links,link,tug,prize;
	makearef(links,pchar.SeaTow.Links);
	for(int i=0;i<GetAttributesNum(links);i++)
	{
		link=GetAttributeN(links,i);
		if(!CheckAttribute(link,"Tug.Index") || !CheckAttribute(link,"Prize.Index")) continue;
		makearef(tug,link.Tug);
		makearef(prize,link.Prize);
		if(!CheckAttribute(tug,"Id") || !CheckAttribute(tug,"Hull") || !CheckAttribute(tug,"Stamp")) continue;
		if(!CheckAttribute(prize,"Id") || !CheckAttribute(prize,"Hull") || !CheckAttribute(prize,"Stamp")) continue;
		if(!SeaTow_Matches(tug,pchar) || !SeaTow_ValidEndpoint(prize)) continue;
		ref chr=GetCharacter(sti(prize.Index));
		if(LAi_IsDead(chr)) continue;
		int slot=sti(snapshot.count);
		string key="s"+slot;
		snapshot.slots.(key).index=prize.Index;
		snapshot.signature=snapshot.signature+"|"+tug.Index+":"+tug.Id+":"+tug.Hull+":"+tug.Stamp+
			">"+prize.Index+":"+prize.Id+":"+prize.Hull+":"+prize.Stamp;
		snapshot.count=slot+1;
		if(slot==2) break;
	}
}

void ShipTowHUD_Clear(aref owner)
{
	for(int i=0;i<3;i++)
	{
		HUDLayout_RemoveObject(owner,"tow_portrait"+i);
		HUDLayout_RemoveObject(owner,"tow_speedicon"+i);
		HUDLayout_RemoveObject(owner,"tow_massicon"+i);
		HUDLayout_RemoveObject(owner,"tow_speed"+i);
		HUDLayout_RemoveObject(owner,"tow_mass"+i);
		HUDLayout_RemoveObject(owner,"tow_tension"+i);
		HUDLayout_RemoveObject(owner,"tow_slack"+i);
		HUDLayout_RemoveObject(owner,"tow_summary"+i);
	}
	DeleteAttribute(owner,"shiptowhud");
	NewSeaHUD_PositionAimReadout(owner);
}

bool ShipTowHUD_Intersects(float left,float top,float right,float bottom,float bl,float bt,float br,float bb)
{
	return left<br && right>bl && top<bb && bottom>bt;
}

// LEO: Якорь берём из ГОТОВОГО ряда, поэтому работают старый HUD, новый HUD и смещения ресурсов.
bool ShipTowHUD_Layout(aref owner,int count,ref layout)
{
	if(!CheckAttribute(owner,"imageslist.ammoicon0.pos") || !CheckAttribute(owner,"imageslist.ammoicon8.pos")) return false;
	float ratio=stf(Render.screen_y)/makefloat(screenscaling);
	if(ratio<=0.0) return false;
	float rowLeft=stf(GetSubStringByNum(owner.imageslist.ammoicon0.pos,0));
	float rowRight=stf(GetSubStringByNum(owner.imageslist.ammoicon8.pos,2));
	float rowTop=stf(GetSubStringByNum(owner.imageslist.ammoicon0.pos,1));
	int columns=count;
	if(columns<2) columns=2;
	float unit=ratio;
	if(rowRight-rowLeft<120.0*columns*unit) unit=(rowRight-rowLeft)/(120.0*columns);
	if(unit<=0.0) return false;
	float width=120.0*columns*unit;
	float height=100.0*unit;
	float gap=8.0*unit;
	float baseY=rowTop-24.0*ratio-height;
	if(CheckAttribute(owner,"textinfo.ammocount0.pos.y")) baseY=stf(owner.textinfo.ammocount0.pos.y)-gap-height;
	float baseX=(rowLeft+rowRight-width)*0.5;
	object obstacles,perks;
	aref target,source;
	int n=0;
	if(CheckAttribute(owner,"newseahud.explog"))
	{
		makearef(target,obstacles.b0);
		makearef(source,owner.newseahud.explog);
		CopyAttributes(target,source);
		n++;
	}
	else
	{
		if(CheckAttribute(owner,"textinfo.EXP1.pos.x") && CheckAttribute(owner,"expLogLines"))
		{
			obstacles.b0.right=owner.textinfo.EXP1.pos.x;
			obstacles.b0.left=stf(obstacles.b0.right)-280.0*ratio;
			obstacles.b0.top=owner.textinfo.EXP1.pos.y;
			obstacles.b0.bottom=stf(obstacles.b0.top)+20.0*ratio*sti(owner.expLogLines);
			n++;
		}
	}
	ActivePerks_SetLayout(&perks,true);
	string block="b"+n;
	makearef(target,obstacles.(block));
	makearef(source,perks.PosRect);
	CopyAttributes(target,source);
	obstacles.(block).top=stf(obstacles.(block).top)-14.0*ratio;
	n++;
	float xs[5]; float ys[3];
	xs[0]=baseX; ys[0]=baseY;
	for(int i=0;i<n;i++)
	{
		block="b"+i;
		xs[2*i+1]=stf(obstacles.(block).left)-gap-width;
		xs[2*i+2]=stf(obstacles.(block).right)+gap;
		ys[i+1]=stf(obstacles.(block).top)-gap-height;
	}
	float best=0.0;
	bool found=false;
	for(int y=0;y<=n;y++)
	{
		if(ys[y]<gap || ys[y]>baseY) continue;
		for(int x=0;x<=2*n;x++)
		{
			float left=xs[x];
			if(left<gap) left=gap;
			if(left+width>stf(Render.screen_x)-gap) left=stf(Render.screen_x)-gap-width;
			if(left<gap) continue;
			bool collision=false;
			for(i=0;i<n;i++)
			{
				block="b"+i;
				if(ShipTowHUD_Intersects(left-gap,ys[y]-gap,left+width+gap,ys[y]+height+gap,
					stf(obstacles.(block).left),stf(obstacles.(block).top),stf(obstacles.(block).right),stf(obstacles.(block).bottom))) collision=true;
			}
			if(collision) continue;
			float score=abs(left-baseX)+2.0*abs(ys[y]-baseY);
			if(found && score>=best) continue;
			best=score;
			found=true;
			layout.left=makeint(left); layout.top=makeint(ys[y]);
			layout.right=makeint(left+width); layout.bottom=makeint(ys[y]+height);
			layout.unit=unit;
		}
	}
	return found;
}

// LEO: Те же страницы 256 px и inset, что у портретов ГГ; не подключаем старый огромный атлас.
void ShipTowHUD_Portrait(aref owner,aref pages,int picture,string id,string rect)
{
	for(int i=0;i<GetAttributesNum(pages);i++)
	{
		aref page=GetAttributeN(pages,i);
		int delta=picture-sti(page.first);
		if(delta<0 || delta%sti(page.stride)!=0) continue;
		int cell=delta/sti(page.stride);
		if(cell>=sti(page.count)) continue;
		float w=stf(page.width)/sti(page.columns);
		float h=stf(page.height)/sti(page.rows);
		float px=(cell%sti(page.columns))*w;
		float py=(cell/sti(page.columns))*h;
		float inset=stf(page.inset);
		string uv=FloatToString((px+inset)/stf(page.width),6)+","+FloatToString((py+inset)/stf(page.height),6)+","+
			FloatToString((px+w-inset)/stf(page.width),6)+","+FloatToString((py+h-inset)/stf(page.height),6);
		HUDLayout_SetImage(owner,id,page.texture,rect,argb(255,128,128,128),uv);
		return;
	}
}

void ShipTowHUD_Text(aref owner,string id,string value)
{
	string key="hudcustom_"+id;
	if(!CheckAttribute(owner,"textinfo."+key+".text")) return;
	if(owner.textinfo.(key).text!=value) owner.textinfo.(key).text=value;
}

// Некорректные строки не передаём в stf: NaN вызывает ошибку VM.
float ShipTowHUD_Number(aref source,string name)
{
	if(!CheckAttribute(source,name)) return -1.0;
	string text=source.(name);
	bool digits=false;
	bool point=false;
	bool exponent=false;
	for(int i=0;i<strlen(text);i++)
	{
		string symbol=GetSymbol(text,i);
		if(IsDigit(&text,i)) {digits=true;continue;}
		if(symbol=="." && !point && !exponent) {point=true;continue;}
		if(symbol=="e" || symbol=="E")
		{
			if(!digits || exponent) return -1.0;
			exponent=true;
			digits=false;
			continue;
		}
		if(symbol=="+" || symbol=="-")
		{
			if(i==0) return -1.0;
			string previous=GetSymbol(text,i-1);
			if(previous=="e" || previous=="E") continue;
		}
		return -1.0;
	}
	if(!digits) return -1.0;
	float value=stf(text);
	if(value<0.0 || value>stf("3.402823e38")) return -1.0;
	return value;
}

int ShipTowHUD_TensionColor(aref tow,float tension)
{
	int neutral=argb(255,255,255,255);
	if(tension<0.0) return neutral;
	if(!CheckAttribute(tow,"PhysicsVersion")) return neutral;
	if(tow.PhysicsVersion!="2") return neutral;
	float tugMass=ShipTowHUD_Number(tow,"TugMass");
	float prizeMass=ShipTowHUD_Number(tow,"PrizeMass");
	float addedMass=ShipTowHUD_Number(tow,"AddedMass");
	float acceleration=ShipTowHUD_Number(tow,"MaxAcceleration");
	if(tugMass<=0.0 || prizeMass<=0.0 || addedMass<1.0 || acceleration<=0.0) return neutral;
	float mass=tugMass;
	if(prizeMass<mass) mass=prizeMass;
	// Линейный предел Solve, не прочность каната; угловой предел может быть ниже.
	float limit=(mass/1000.0)*addedMass*acceleration;
	if(limit<=0.0 || limit>stf("3.402823e38")) return neutral;
	float load=tension/limit;
	if(load>=1.0) return argb(255,224,80,80);
	// Два непрерывных участка: зелёный -> жёлтый -> красный.
	if(load<=0.5) return argb(255,80+makeint(304.0*load+0.5),208,80-makeint(32.0*load+0.5));
	load=(load-0.5)*2.0;
	return argb(255,232-makeint(8.0*load+0.5),208-makeint(128.0*load+0.5),64+makeint(16.0*load+0.5));
}

void ShipTowHUD_Update(aref owner,aref snapshot)
{
	if(!CheckAttribute(owner,"shiptowhud.visible")) return;
	if(sti(owner.shiptowhud.visible)==0) return;
	// LEO: SeaTow_Mass включает корпус и груз в CWT; в подписях 1 CWT = 0.05 тонны.
	for(int i=0;i<sti(snapshot.count);i++)
	{
		string key="s"+i;
		ref chr=GetCharacter(sti(snapshot.slots.(key).index));
		float mass=SeaTow_Mass(chr);
		float speed=0.0;
		if(CheckAttribute(chr,"Ship.Speed.z")) speed=abs(stf(chr.Ship.Speed.z));
		if(CheckAttribute(owner,"ShipSpeedScaler")) speed=speed*stf(owner.ShipSpeedScaler);
		ShipTowHUD_Text(owner,"tow_speed"+i,FloatToString(speed,1)+" "+XI_ConvertString("ShipTowHUDKnots"));
		ShipTowHUD_Text(owner,"tow_mass"+i,makeint(mass*0.05+0.5)+" "+XI_ConvertString("ShipTowHUDTons"));
		string tension="-";
		string slack="-";
		int tensionColor=argb(255,255,255,255);
		string tensionPrefix=XI_ConvertString("ShipTowHUDTension")+" ";
		if(CheckAttribute(chr,"Ship.SeaTow"))
		{
			aref tow;
			makearef(tow,chr.Ship.SeaTow);
			float force=ShipTowHUD_Number(tow,"TensionKN");
			if(force>=0.0)
			{
				tensionColor=ShipTowHUD_TensionColor(tow,force);
				tension=FloatToString(force,1)+" "+XI_ConvertString("ShipTowHUDKilonewtons");
				if(tensionColor!=argb(255,255,255,255))
				{
					tensionPrefix="\6"+tensionPrefix;
					tension="\2"+FloatToString(force,1)+"\6 "+XI_ConvertString("ShipTowHUDKilonewtons");
				}
			}
		}
		if(CheckAttribute(chr,"Ship.SeaTow.SlackMeters")) slack=FloatToString(stf(chr.Ship.SeaTow.SlackMeters),1)+" "+XI_ConvertString("ShipTowHUDMeters");
		string tensionKey="hudcustom_tow_tension"+i;
		if(CheckAttribute(owner,"textinfo."+tensionKey)) owner.textinfo.(tensionKey).color=tensionColor;
		ShipTowHUD_Text(owner,"tow_tension"+i,tensionPrefix+tension);
		ShipTowHUD_Text(owner,"tow_slack"+i,XI_ConvertString("ShipTowHUDSlack")+" "+slack);
	}
	float own=SeaTow_Mass(pchar);
	string summary=XI_ConvertString("ShipTowHUDOwn")+" "+makeint(own*0.05+0.5)+" "+XI_ConvertString("ShipTowHUDTons");
	ShipTowHUD_Text(owner,"tow_summary0",summary);
}

void ShipTowHUD_Build(aref owner)
{
	ShipTowHUD_Clear(owner);
	object snapshot,layout,atlas;
	ShipTowHUD_Collect(&snapshot);
	owner.shiptowhud.signature=snapshot.signature;
	owner.shiptowhud.count=snapshot.count;
	owner.shiptowhud.visible=0;
	int count=sti(snapshot.count);
	if(count==0) return;
	if(!ShipTowHUD_Layout(owner,count,&layout)) return;
	aref savedLayout;
	makearef(savedLayout,owner.shiptowhud.layout);
	CopyAttributes(savedLayout,&layout);
	NewHUD_SetShipPortraitPages(&atlas,sti(InterfaceStates.InterfaceSelection));
	aref pages;
	makearef(pages,atlas.newhud.portraitpages);
	float unit=stf(layout.unit);
	int top=sti(layout.top);
	for(int i=0;i<count;i++)
	{
		string slot="s"+i;
		ref chr=GetCharacter(sti(snapshot.slots.(slot).index));
		int hull=SeaTow_Hull(chr);
		int picture=sti(RealShips[hull].basetype)*2;
		int x=(sti(layout.left)+sti(layout.right)-makeint(120.0*unit*count))/2+makeint(120.0*unit*i);
		int size=makeint(44.0*unit);
		ShipTowHUD_Portrait(owner,pages,picture,"tow_portrait"+i,x+","+top+","+(x+size)+","+(top+size));
		int ix=x+makeint(48.0*unit);
		int icon=makeint(14.0*unit);
		for(int row=0;row<2;row++)
		{
			int y=top+makeint((3.0+21.0*row)*unit);
			string id="tow_speedicon"+i;
			string uv="0.25,0.875,0.3125,1";
			if(row==1) {id="tow_massicon"+i;uv="0.3125,0.875,0.375,1";}
			HUDLayout_SetImage(owner,id,"interfaces\icons.dds",ix+","+y+","+(ix+icon)+","+(y+icon),argb(255,128,128,128),uv);
			id="tow_speed"+i;
			if(row==1) id="tow_mass"+i;
			HUDLayout_SetText(owner,id,"","interface_normal",0.75*unit,ix+icon+makeint(3.0*unit),y,argb(255,255,255,255),"left");
			string textKey="hudcustom_"+id;
			owner.textinfo.(textKey).maxWidth=55.0*unit;
			owner.textinfo.(textKey).maxLines=1;
		}
		for(row=0;row<2;row++)
		{
			string gauge="tow_tension"+i;
			if(row==1) gauge="tow_slack"+i;
			HUDLayout_SetText(owner,gauge,"","interface_normal",0.70*unit,x,top+makeint((47.0+16.0*row)*unit),argb(255,255,255,255),"left");
			string gaugeKey="hudcustom_"+gauge;
			owner.textinfo.(gaugeKey).maxWidth=116.0*unit;
			owner.textinfo.(gaugeKey).maxLines=1;
		}
	}
	HUDLayout_SetText(owner,"tow_summary0","","interface_normal",0.80*unit,
		(sti(layout.left)+sti(layout.right))/2,top+makeint(82.0*unit),argb(255,255,255,255),"center");
	owner.textinfo.hudcustom_tow_summary0.maxWidth=sti(layout.right)-sti(layout.left);
	owner.textinfo.hudcustom_tow_summary0.maxLines=1;
	owner.shiptowhud.visible=1;
	NewSeaHUD_PositionAimReadout(owner);
	ShipTowHUD_Update(owner,&snapshot);
}

bool ShipTowHUD_Poll(aref owner)
{
	object snapshot;
	ShipTowHUD_Collect(&snapshot);
	string previous="";
	if(CheckAttribute(owner,"shiptowhud.signature")) previous=owner.shiptowhud.signature;
	if(previous!=snapshot.signature) return true;
	if(sti(snapshot.count)>0) ShipTowHUD_Update(owner,&snapshot);
	return false;
}

// LEO: Штатные морские часы уже копят реальные мс; новый обработчик frame не нужен даже на x8.
bool ShipTowHUD_Due()
{
	int seconds=GetSeaTime();
	int elapsed=(seconds-ShipTowHUDLastSecond)*1000+iSeaTimeAccumMs-ShipTowHUDLastMs;
	if(elapsed>=0 && elapsed<SHIP_TOW_HUD_UPDATE_MS) return false;
	ShipTowHUDLastSecond=seconds;
	ShipTowHUDLastMs=iSeaTimeAccumMs;
	return true;
}

void ShipTowHUD_Schedule()
{
	float delay=makefloat(SHIP_TOW_HUD_UPDATE_MS);
	if(iRealDeltaTime>0 && GetDeltaTime()>iRealDeltaTime) delay=delay*GetDeltaTime()/iRealDeltaTime;
	PostEvent("ShipTowHUDTick",makeint(delay+0.5),"l",ShipTowHUDGeneration);
}

void ShipTowHUD_Stop()
{
	ShipTowHUDGeneration++;
	DelEventHandler("ShipTowHUDTick","ShipTowHUD_Tick");
}

void ShipTowHUD_Start()
{
	ShipTowHUD_Stop();
	ShipTowHUDLastSecond=GetSeaTime();
	ShipTowHUDLastMs=iSeaTimeAccumMs;
	SetEventHandler("ShipTowHUDTick","ShipTowHUD_Tick",0);
	ShipTowHUD_Schedule();
}

bool ShipTowHUD_CanRebuild(aref owner)
{
	if(bNewHUDRebuild || DialogRun || questMovieProcess>0) return false;
	if(sti(InterfaceStates.Launched)!=0 || IsEntity(reload_fader)) return false;
	if(CheckAttribute(owner,"ComState"))
	{
		if(sti(owner.ComState)!=0) return false;
	}
	return true;
}

void ShipTowHUD_Tick()
{
	int generation=GetEventData();
	if(generation!=ShipTowHUDGeneration) return;
	if(!IsEntity(&BattleInterface) || !bSeaActive || bAbordageStarted || bSeaReloadStarted)
	{
		ShipTowHUD_Stop();
		return;
	}
	if(ShipTowHUD_Due())
	{
		if(ShipTowHUD_Poll(&BattleInterface) && ShipTowHUD_CanRebuild(&BattleInterface))
		{
			// LEO: Морской BI_MSG_REFRESH не перечитывает imageslist; штатная пересборка нужна лишь при смене состава.
			NewHUD_RebuildInterface();
			return;
		}
	}
	ShipTowHUD_Schedule();
}
