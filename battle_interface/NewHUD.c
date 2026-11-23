// LEO: Только синхронная смена оформления: штатные ResetTimeScale/перезагрузка локации здесь не нужны.
bool bNewHUDRebuild=false;
bool bNewHUDRestoreTime=false;

void NewHUD_RebuildInterface()
{
	if(bNewHUDRebuild) return;
	string action=g_ActiveActionName;
	// LEO: В модальном меню множитель временно равен 1; после выхода восстанавливаем счётчик, а не preset.
	if(CheckAttribute(&InterfaceStates,"Launched"))
	{
		if(sti(InterfaceStates.Launched)!=0)
		{
			if(TimeScaleCounter!=0 && IsPerkIntoList("TimeSpeed")) bNewHUDRestoreTime=true;
		}
	}
	bNewHUDRebuild=true;
	if(IsEntity(&worldMap))
	{
		if(IsEntity(&BattleInterface)) WM_RebuildInterface();
	}
	else
	{
		bool exists=IsEntity(&objLandInterface);
		if(bSeaActive && !bAbordageStarted) exists=IsEntity(&BattleInterface);
		if(exists)
		{
			ChangeShowIntarface();
			ChangeShowIntarface();
		}
	}
	bNewHUDRebuild=false;

	g_ActiveActionName=action;
	Log_RefreshCommandIcons();
	ActivePerks_RefreshStyle();
	Event("NewHUD_RefreshSpyglass");
}

string NewHUD_OptionKey(int index)
{
	string key="";
	switch(index)
	{
		case 0: key="NewHUDEnabled"; break;
		case 1: key="NewHUDGhosts"; break;
		case 2: key="NewHUDHeroRank"; break;
		case 3: key="NewHUDHeroExperience"; break;
		case 4: key="NewHUDHeroReload"; break;
		case 5: key="NewHUDHeroBullets"; break;
		case 6: key="NewHUDHeroNumbers"; break;
		case 7: key="NewHUDOfficerRank"; break;
		case 8: key="NewHUDOfficerExperience"; break;
		case 9: key="NewHUDOfficerReload"; break;
		case 10: key="NewHUDOfficerBullets"; break;
		case 11: key="NewHUDCrewCount"; break;
		case 12: key="NewCommandIconsEnabled"; break;
	}
	return key;
}

int NewHUD_GetOption(string key)
{
	if(!CheckAttribute(&InterfaceStates,key)) return 1;
	if(sti(InterfaceStates.(key)) != 0) return 1;
	return 0;
}

void NewHUD_InitOptions()
{
	for(int i=0;i<13;i++)
	{
		string key=NewHUD_OptionKey(i);
		int value=NewHUD_GetOption(key);
		InterfaceStates.(key)=value;
	}
}

void NewHUD_SaveOptions(ref options)
{
	for(int i=0;i<13;i++)
	{
		string key=NewHUD_OptionKey(i);
		options.cameramode.(key)=NewHUD_GetOption(key);
	}
}

void NewHUD_LoadOptions(ref options)
{
	for(int i=0;i<13;i++)
	{
		string key=NewHUD_OptionKey(i);
		InterfaceStates.(key)=1;
		if(CheckAttribute(options,"cameramode."+key)) InterfaceStates.(key)=sti(options.cameramode.(key)) != 0;
	}
}

void NewHUD_ApplyOptions(aref owner,bool sea)
{
	if(!CheckAttribute(owner,"newhud.heroprofile")) return;
	owner.newhud.enabled=NewHUD_GetOption("NewHUDEnabled");
	owner.newhud.ghostenabled=NewHUD_GetOption("NewHUDGhosts");
	owner.newhud.hero.numbers=NewHUD_GetOption("NewHUDHeroNumbers");
	if(sea)
	{
		owner.newhud.companion.crewcount=NewHUD_GetOption("NewHUDCrewCount");
		return;
	}
	owner.newhud.hero.rank=NewHUD_GetOption("NewHUDHeroRank");
	owner.newhud.hero.experience=NewHUD_GetOption("NewHUDHeroExperience");
	owner.newhud.hero.reload=NewHUD_GetOption("NewHUDHeroReload");
	owner.newhud.hero.bullets=NewHUD_GetOption("NewHUDHeroBullets");
	owner.newhud.companion.rank=NewHUD_GetOption("NewHUDOfficerRank");
	owner.newhud.companion.experience=NewHUD_GetOption("NewHUDOfficerExperience");
	owner.newhud.companion.reload=NewHUD_GetOption("NewHUDOfficerReload");
	owner.newhud.companion.bullets=NewHUD_GetOption("NewHUDOfficerBullets");
	if(CheckAttribute(owner,"manstateghoston")) owner.newhud.ghostenabled=sti(owner.manstateghoston)*NewHUD_GetOption("NewHUDGhosts");
}

// LEO: Обновляем только оформление, сохраняя видимость, управление и состояние команд.
void NewHUD_ApplyLive()
{
	aref owner;
	if(IsEntity(&objLandInterface))
	{
		makearef(owner,objLandInterface.ManSign);
		NewHUD_ApplyOptions(owner,false);
		// LEO: В меню игровой тик может стоять; обновляем только данные уже существующих иконок.
		if(CheckAttribute(&objLandInterface,"data.icons"))
		{
			aref icons,icon;
			makearef(icons,objLandInterface.data.icons);
			int count=GetAttributesNum(icons);
			for(int i=0;i<count;i++)
			{
				icon=GetAttributeN(icons,i);
				if(!CheckAttribute(icon,"chrindex")) continue;
				int chrindex=sti(icon.chrindex);
				if(chrindex<0 || chrindex>=TOTAL_CHARACTERS) continue;
				ref chref=GetCharacter(chrindex);
				NewHUD_SetCharacterExtras(chref,icon);
			}
		}
		SendMessage(&objLandInterface,"l",BI_MSG_RELOAD_NEWHUD);
	}
	if(IsEntity(&BattleInterface))
	{
		if(IsEntity(&worldMap)) makearef(owner,BattleInterface.wm_sign);
		else makearef(owner,BattleInterface.ShipIcon);
		NewHUD_ApplyOptions(owner,true);
		SendMessage(&BattleInterface,"l",BI_MSG_RELOAD_NEWHUD);
	}
	Event("NewHUD_RefreshSpyglass");
}

float NewHUD_SetLandLayout(aref owner,int style,int slots,float heightRatio,float screenHeight)
{
	float diameter=64.0*heightRatio;
	if(style==POTC) style=KVADRAT;
	if(style != KVADRAT && style != KRUGLYI) return diameter;
	if(NewHUD_GetOption("NewHUDEnabled")==0) return diameter;
	if(slots<2) return diameter;
	float firstY=60.0*heightRatio;
	float available=screenHeight-firstY-6.0*heightRatio;
	float required=(94.0+(slots-2)*88.0+48.0)*heightRatio;
	float firstGap=94.0;
	float stride=88.0;
	if(style==KRUGLYI)
	{
		diameter=72.0*heightRatio;
		firstY=66.0*heightRatio;
		firstGap=96.0;
		stride=90.0;
		available=screenHeight-firstY-8.0*heightRatio;
		required=(firstGap+(slots-2)*stride+43.0)*heightRatio;
	}
	if(available<=0.0) return diameter;
	float fit=1.0;
	if(required>available) fit=available/required;
	for(int i=1;i<=slots;i++)
	{
		string key="iconoffset"+i;
		float y=firstY;
		if(i>1) y=firstY+(firstGap+(i-2)*stride)*heightRatio*fit;
		owner.(key)=makeint(60.0*heightRatio)+","+makeint(y);
	}
	return diameter*fit;
}

float NewHUD_SetSeaLayout(aref owner,int style,bool overhead,int slots,float heightRatio,float screenHeight,float oldDiameter)
{
	if(style==POTC) style=KVADRAT;
	if(style!=KVADRAT || NewHUD_GetOption("NewHUDEnabled")==0 || slots<2) return oldDiameter;
	float diameter=88.0*heightRatio;
	float top=0.5818182;
	if(overhead)
	{
		diameter=80.0*heightRatio;
		top=0.7181818;
	}
	float firstY=72.0*heightRatio;
	float nameGap=18.0*heightRatio;
	float coefficient=0.6+top+(slots-2)*(0.5+top)+0.5;
	float room=screenHeight-firstY-8.0*heightRatio-(slots-1)*nameGap;
	if(room<=0.0) return oldDiameter;
	if(coefficient*diameter>room) diameter=room/coefficient;
	float firstGap=(0.6+top)*diameter+nameGap;
	float stride=(0.5+top)*diameter+nameGap;
	for(int i=1;i<=slots;i++)
	{
		string key="iconoffset"+i;
		float y=firstY;
		if(i>1) y=firstY+firstGap+(i-2)*stride;
		owner.(key)=makeint(70.0*heightRatio)+","+makeint(y);
	}
	return diameter;
}

float NewHUD_SetWorldMapLayout(aref owner,int style,bool overhead,int slots,float heightRatio,float screenHeight,float oldDiameter)
{
	if(style!=KRUGLYI) return NewHUD_SetSeaLayout(owner,style,overhead,slots,heightRatio,screenHeight,oldDiameter);
	if(NewHUD_GetOption("NewHUDEnabled")==0) return oldDiameter;
	for(int i=1;i<=slots;i++)
	{
		string key="iconoffset"+i;
		owner.(key)=makeint(65.0*heightRatio)+","+makeint((61.0+(i-1)*110.0)*heightRatio);
	}
	return oldDiameter;
}

void NewHUD_SetGhostParameters(aref params, bool sea)
{
	params.ghosthold = 0.12;
	params.ghostspeed = 7.0;
	params.ghostcolor = argb(255,255,255,255);
	if(sea)
	{
		params.ghosthold = 1.0;
		params.ghostspeed = 5.0;
	}
}

// LEO: Только портреты, не командлист. first — старый индекс, stride=2 пропускает выделенные состояния.
void NewHUD_SetShipPortraitPages(aref owner,int style)
{
	string variant="\Alternative";
	// LEO: Альт-иконки меняют командлист, но не квадратные портреты героя и компаньонов.
	if(style==KVADRAT) variant="\Square";
	if(style==POTC) variant="\PotC";
	aref pages;
	makearef(pages,owner.newhud.portraitpages);
	for(int i=0;i<2;i++)
	{
		string key="page"+i;
		pages.(key).texture="battle_interface\NewHUD\ShipPortraits"+variant+"\Portraits"+i+".dds";
		pages.(key).first=i*256;
		pages.(key).count=128;
		pages.(key).stride=2;
		pages.(key).columns=16;
		pages.(key).rows=8;
		pages.(key).width=4096;
		pages.(key).height=2048;
		// LEO: Два пикселя продлённого края внутри ячейки защищают линейную фильтрацию.
		pages.(key).inset=2.0;
	}
	pages.page1.count=15;
	pages.page1.rows=1;
	pages.page1.height=256;
}

void NewHUD_Setup(aref owner, int style, int mainIndex, bool sea, bool overhead, float diameter, float heightRatio)
{
	string shape = "square";
	string context = "land";
	string companion = "officer";
	bool usePotcPortraits = style == POTC;
	int portraitStyle=style;

	if(usePotcPortraits) style=KVADRAT;
	DeleteAttribute(owner, "newhud");
	owner.newhud.enabled = 0;
	if(style != KRUGLYI)
	{
		if(style != KVADRAT) return;
	}
	if(style == KRUGLYI) shape = "round";
	if(sea)
	{
		context = "sea";
		companion = "companion";

		if(mainIndex>=0) NewHUD_SetShipPortraitPages(owner,portraitStyle);
		if(style == KRUGLYI)
		{
			owner.newhud.namegap = 2.0 * heightRatio;

			owner.newhud.nametextinset = 5.0 * heightRatio;
		}
	}

	owner.newhud.maincharacter = mainIndex;
	// LEO: Сохраняем исходный якорь команд суши, моря и карты до настройки нового оформления.
	owner.newhud.commandlistverticaloffset = -48;
	if(CheckAttribute(owner,"commandlistverticaloffset")) owner.newhud.commandlistverticaloffset = owner.commandlistverticaloffset;
	owner.newhud.heroprofile = context + "-hero-" + shape;
	owner.newhud.companionprofile = context + "-" + companion + "-" + shape;
	if(sea)
	{
		if(style == KVADRAT)
		{
			if(overhead) owner.newhud.companionprofile = "sea-companion-square-overhead";
		}
	}
	// LEO: Размер компаньона в экранных пикселях; отрисовка увеличивает ГГ в 1,2 раза.
	owner.newhud.portraitwidth = diameter;
	owner.newhud.portraitheight = diameter;
	owner.newhud.numbersfont = "newhud_numbers";
	owner.newhud.minfontsize = 9.0 * heightRatio;
	aref ghostParams;
	makearef(ghostParams,owner.newhud);
	NewHUD_SetGhostParameters(ghostParams,sea);

	owner.newhud.healthtempcolor = argb(255,255,100,100);
	owner.newhud.energytempcolor = argb(255,100,100,255);

	owner.newhud.hero.crewcount = 0;

	owner.newhud.hero.alarmexcludehorizontal = 1;
	// LEO: На время команд скрываем все боевые HP/SP, Reload Guns и crew; XP, ранг, портрет и офицеры остаются.
	owner.newhud.hero.hidehorizontaloncommand = 1;
	// LEO: Запас остаётся только для обычной тревожной рамки; команды используют цельный portraitframe профиля.
	owner.newhud.hero.horizontalframepadding = 6.0;
	owner.newhud.companion.numbers = 1;
	owner.newhud.companion.crewcount = 0;
	if(sea)
	{

		bool fullShipCell=usePotcPortraits;
		if(style==KVADRAT && mainIndex>=0) fullShipCell=true;
		if(fullShipCell)
		{
			owner.newhud.hero.portraitfullcell = 1;
			owner.newhud.companion.portraitfullcell = 1;
		}
		owner.newhud.hero.rank = 0;
		owner.newhud.hero.experience = 0;
		owner.newhud.hero.reload = 0;
		owner.newhud.hero.bullets = 0;
		owner.newhud.hero.crewcount = 1;
		owner.newhud.companion.rank = 0;
		owner.newhud.companion.experience = 0;
		owner.newhud.companion.reload = 0;
		owner.newhud.companion.bullets = 0;
		owner.newhud.companion.crewcount = 1;
	}
	NewHUD_ApplyOptions(owner,sea);
}

// LEO: Только читаем текущий опыт ранга, не запуская повышение уровня из HUD.
float NewHUD_GetExperience(ref chref)
{
	float rankRate = makefloat(GetCharacterRankRate(chref));
	float progress = 0.0;
	if(rankRate <= 0.0) return 0.0;
	if(!CheckAttribute(chref, "rank_exp")) return 0.0;
	progress = makefloat(GetCharacterRankRateCur(chref)) / rankRate;
	if(progress < 0.0) progress = 0.0;
	if(progress > 1.0) progress = 1.0;
	return progress;
}

// LEO: Сначала очищаем повторно используемый слот, затем считаем только включённые данные.
void NewHUD_SetCharacterExtras(ref chref,aref arData)
{
	arData.healthtext = "";
	arData.energytext = "";
	arData.rank = 0;
	arData.experience = 0.0;
	if(NewHUD_GetOption("NewHUDEnabled")==0) return;
	int style=sti(InterfaceStates.InterfaceSelection);
	if(style!=KRUGLYI && style!=KVADRAT && style!=POTC) return;
	bool hero=sti(chref.index)==sti(pchar.index);
	bool numbers=false;
	string rankKey="NewHUDOfficerRank";
	string experienceKey="NewHUDOfficerExperience";
	if(hero)
	{
		numbers=NewHUD_GetOption("NewHUDHeroNumbers")==1;
		rankKey="NewHUDHeroRank";
		experienceKey="NewHUDHeroExperience";
	}
	if(numbers)
	{
		arData.healthtext = makeint(LAi_GetCharacterHP(chref)) + "/" + makeint(LAi_GetCharacterMaxHP(chref));
		arData.energytext = makeint(LAi_GetCharacterEnergy(chref)) + "/" + makeint(LAi_GetCharacterMaxEnergy(chref));
	}
	if(NewHUD_GetOption(rankKey)==1)
	{
		if(CheckAttribute(chref,"rank")) arData.rank = sti(chref.rank);
	}
	if(NewHUD_GetOption(experienceKey)==1) arData.experience = NewHUD_GetExperience(chref);
}
