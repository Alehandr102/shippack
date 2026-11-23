object objActivePerkShower;
int animPerkCnt = 0;	/*!< Кол-во анимированных перков (морских или на суше), используется для динамического добавления перков без "гвоздей" */
int PERK_READY_LEAD = 4; // LEO: за сколько секунд до конца отката пускать вспышку "скоро готов" (см. PerkFX.outrodur — длительность затухания)

int g_PerkTmrCountSea  = 0;
int g_PerkTmrCountLand = 0;

void ActivePerks_SetWorldMapLayout(ref layout, float ratio, int dx, int dy)
{
	int size = makeint(48 * ratio);
	int gap = makeint(8 * ratio);
	if(size < 1) size = 1;
	if(gap < 1) gap = 1;
	int left = sti(showWindow.left);
	int top = sti(showWindow.top);
	int right = sti(showWindow.right) - size + dx;
	int bottom = sti(showWindow.bottom) - makeint(28 * ratio) + dy;
	if(CheckAttribute(&worldMap,"minimap.enable") && sti(worldMap.minimap.enable) != 0)
	{
		float radius = stf(worldMap.minimap.radius);
		if(radius >= 8.0)
		{
			// Match DrawWorldMinimap's viewport anchor and outer frame extent.
			float reach = stf(worldMap.minimap.margin) + radius * (1.0 + stf(worldMap.minimap.frameScale));
			int radarTop = makeint(stf(Render.screen_y) - reach) - gap;
			int radarLeft = makeint(stf(Render.screen_x) - reach) - gap;
			int suppliesBottom = top;
			if(CheckAttribute(&worldMap,"medText.pos.y")) suppliesBottom = makeint(stf(worldMap.medText.pos.y) + 16 * ratio);
			if(radarTop < suppliesBottom + size && radarLeft >= left + size)
			{
				// A large radar can leave no vertical space below the supplies.
				if(right > radarLeft) right = radarLeft;
				bottom = radarTop + size + dy;
			}
			else if(bottom > radarTop) bottom = radarTop;
		}
	}
	if(right > sti(showWindow.right)) right = sti(showWindow.right);
	if(right < left + size) right = left + size;
	if(bottom > sti(showWindow.bottom)) bottom = sti(showWindow.bottom);
	if(bottom < top + size) bottom = top + size;
	layout.PosRect.left = right - size;
	layout.PosRect.right = right;
	layout.PosRect.top = bottom - size;
	layout.PosRect.bottom = bottom;
	layout.IconSize.horz = size;
	layout.IconSize.vert = size;
	layout.IconSpace.horz = 0;
	layout.IconSpace.vert = 0;
}

// LEO: Единая геометрия иконок и таймеров; готовые reserved и смещения API уже в экранных пикселях.
void ActivePerks_SetLayout(ref layout, bool bSea)
{
	float ratio = stf(Render.screen_y) / screenscaling;
	string profile = HUDLayout_Context(bSea);
	int dx = HUDLayout_GetOffset(profile,"perks","x");
	int dy = HUDLayout_GetOffset(profile,"perks","y");
	layout.PosRect.left = sti(showWindow.right) - RecalculateHIcon(makeint(180 * ratio)) + dx;
	layout.PosRect.top = RecalculateVIcon(makeint(500 * ratio)) + dy;
	layout.PosRect.right = sti(showWindow.right) - RecalculateHIcon(makeint(48 * ratio)) + dx;
	layout.PosRect.bottom = sti(showWindow.bottom) - RecalculateVIcon(makeint(28 * ratio)) + dy;
	layout.IconSize.horz = RecalculateHIcon(makeint(48 * ratio));
	layout.IconSize.vert = RecalculateVIcon(makeint(48 * ratio));
	layout.IconSpace.horz = RecalculateHIcon(2);
	layout.IconSpace.vert = RecalculateVIcon(2);
	layout.FillRowMajor = 0;
	layout.FillBottomUp = 0;
	layout.TimerAbove = 0;
	if(profile == "worldmap")
	{
		ActivePerks_SetWorldMapLayout(layout,ratio,dx,dy);
		return;
	}
	if(!bSea || profile!="sea" || NewHUD_GetOption("NewHUDEnabled")==0) return;
	if(!CheckAttribute(&BattleInterface,"newseahud.perks.left")) return;
	if(!CheckAttribute(&BattleInterface,"newseahud.perks.top")) return;
	if(!CheckAttribute(&BattleInterface,"newseahud.perks.right")) return;
	if(!CheckAttribute(&BattleInterface,"newseahud.perks.bottom")) return;
	if(!CheckAttribute(&BattleInterface,"newseahud.perks.unit")) return;
	aref perks;
	makearef(perks,BattleInterface.newseahud.perks);
	float unit=stf(perks.unit);
	if(unit<=0.0 || sti(perks.right)<=sti(perks.left) || sti(perks.bottom)<=sti(perks.top)) return;

	layout.PosRect.left=sti(perks.left);
	layout.PosRect.top=sti(perks.top);
	layout.PosRect.right=sti(perks.right);
	layout.PosRect.bottom=sti(perks.bottom);

	if(CheckAttribute(perks,"underLog"))
	{
		if(sti(perks.underLog)!=0) return;
	}
	layout.IconSize.horz=makeint(40.0*unit);
	layout.IconSize.vert=makeint(40.0*unit);
	layout.IconSpace.horz=makeint(48.0*unit)-sti(layout.IconSize.horz);
	layout.IconSpace.vert=makeint(60.0*unit)-sti(layout.IconSize.vert);
	layout.FillRowMajor=1;
	layout.FillBottomUp=1;
	layout.TimerAbove=1;
}

// LEO: Слоты в obj.textinfo повторяют движковую геометрию; bSea выбирает море или сушу.
void PerkTimers_CreateSlots(ref obj, bool bSea)
{
	float fHtRatio = stf(Render.screen_y) / screenscaling;
	object layout;
	ActivePerks_SetLayout(&layout,bSea);
	int rLeft = sti(layout.PosRect.left);
	int rTop = sti(layout.PosRect.top);
	int rRight = sti(layout.PosRect.right);
	int rBottom = sti(layout.PosRect.bottom);
	int iconW = sti(layout.IconSize.horz);
	int iconH = sti(layout.IconSize.vert);
	int spaceH = sti(layout.IconSpace.horz);
	int spaceV = sti(layout.IconSpace.vert);

	int nHorzQ = (rRight - rLeft) / (iconW + spaceH); if (nHorzQ <= 0) nHorzQ = 1;
	int nVertQ = (rBottom - rTop) / (iconH + spaceV); if (nVertQ <= 0) nVertQ = 1;
	int total = nHorzQ * nVertQ;

	int oldTotal=g_PerkTmrCountLand;
	if(bSea) oldTotal=g_PerkTmrCountSea;
	for(int oldSlot=total; oldSlot<oldTotal; oldSlot++)
	{
		DeleteAttribute(obj,"textinfo.PtmrA"+oldSlot);
		DeleteAttribute(obj,"textinfo.PtmrC"+oldSlot);
	}
	if (bSea) g_PerkTmrCountSea = total;
	else      g_PerkTmrCountLand = total;

	int ih, iv, idx, pr, pl, pt, cx, cy;
	string sA, sC;
	for (ih = 0; ih < nHorzQ; ih++)
	{
		for (iv = 0; iv < nVertQ; iv++)
		{
			idx = iv + ih * nVertQ;
			pr = rRight - ih * (iconW + spaceH);
			pl = pr - iconW;
			pt = rTop + iv * (iconH + spaceV);

			cx = pl - makeint(iconW * 0.30);          // центр числа чуть левее левого края иконки
			cy = pt + makeint(iconH * 0.32);          // по вертикали - на уровне иконки
			if(sti(layout.FillRowMajor)!=0)
			{

				idx = ih + iv * nHorzQ;
				pl = rLeft + ih * (iconW + spaceH);
				pt = rTop + iv * (iconH + spaceV);
				if(sti(layout.FillBottomUp)!=0) pt = rBottom - iconH - iv * (iconH + spaceV);
			}
			if(sti(layout.TimerAbove)!=0)
			{
				cx = pl + iconW / 2;
				cy = pt - makeint(14.0 * stf(BattleInterface.newseahud.perks.unit));
			}

			sA = "PtmrA" + idx;                        // действие - зелёный
			obj.textinfo.(sA).font = "interface_normal";
			obj.textinfo.(sA).scale = 1.0 * fHtRatio;
			obj.textinfo.(sA).color = argb(255, 90, 255, 90);
			obj.textinfo.(sA).shadow = 1;
			obj.textinfo.(sA).refreshable = 1;
			obj.textinfo.(sA).pos.x = cx;
			obj.textinfo.(sA).pos.y = cy;
			obj.textinfo.(sA).text = "";

			sC = "PtmrC" + idx;                        // откат - красный
			obj.textinfo.(sC).font = "interface_normal";
			obj.textinfo.(sC).scale = 1.0 * fHtRatio;
			obj.textinfo.(sC).color = argb(255, 255, 95, 95);
			obj.textinfo.(sC).shadow = 1;
			obj.textinfo.(sC).refreshable = 1;
			obj.textinfo.(sC).pos.x = cx;
			obj.textinfo.(sC).pos.y = cy;
			obj.textinfo.(sC).text = "";
		}
	}

	if(bSea) ActivePerks_ApplyLayout(&layout);
}

// LEO: Таймеры следуют порядку PerkList.list, как и иконки; пустые места очищаем.
void PerkTimers_Refresh()
{
	if (!IsEntity(&objActivePerkShower) || IsEntity(&worldMap)) return;
	bool bSea = bSeaActive && !bAbordageStarted;
	aref tinfo;
	int total;
	if (bSea)
	{
		if (!CheckAttribute(&BattleInterface, "textinfo")) return;
		makearef(tinfo, BattleInterface.textinfo);
		total = g_PerkTmrCountSea;
	}
	else
	{
		if (!CheckAttribute(&objLandInterface, "textinfo")) return;
		makearef(tinfo, objLandInterface.textinfo);
		total = g_PerkTmrCountLand;
	}
	if (total <= 0) return;

	int place = 0;
	if (CheckAttribute(&objActivePerkShower, "PerkList.list"))
	{
		aref arRoot, arPerk;
		makearef(arRoot, objActivePerkShower.PerkList.list);
		int nPerks = GetAttributesNum(arRoot);
		int j;
		string pn, aTxt, cTxt, slotA, slotC;
		string oType; int oIdx; bool onP; // LEO: разрешение владельца перка (ГГ или офицер)
		ref owner = pchar;

		for (j = 0; j < nPerks; j++)
		{
			if (place >= total) break;
			arPerk = GetAttributeN(arRoot, j);
			pn = GetAttributeName(arPerk);
			aTxt = "";
			cTxt = "";

			owner = pchar;
			onP = CheckAttribute(pchar, "perks.list." + pn + ".active");
			if (!onP) onP = CheckAttribute(pchar, "perks.list." + pn + ".delay");
			if (!onP)
			{
				if (CheckAttribute(&ChrPerksList, "list." + pn + ".OfficerType"))
				{
					oType = ChrPerksList.list.(pn).OfficerType;
					if (CheckAttribute(pchar, "Fellows.Passengers." + oType))
					{
						oIdx = sti(pchar.Fellows.Passengers.(oType));
						if (oIdx >= 0) owner = GetCharacter(oIdx);
					}
				}
			}
			if (CheckAttribute(owner, "perks.list." + pn + ".active"))
			{
				if(sti(owner.perks.list.(pn).active)>0) aTxt = "" + sti(owner.perks.list.(pn).active);
			}
			if(aTxt=="")
			{
				if (CheckAttribute(owner, "perks.list." + pn + ".delay"))
				{
					if(sti(owner.perks.list.(pn).delay)>0) cTxt = "" + sti(owner.perks.list.(pn).delay);
				}
			}
			if (pn == "BoardingRetry" && bSea) cTxt = "" + Boarding_RetryLeft(pchar);
			slotA = "PtmrA" + place; // аксессор .(...) требует готовую строковую переменную, не инлайн-конкат
			slotC = "PtmrC" + place;
			tinfo.(slotA).text = aTxt;
			tinfo.(slotC).text = cTxt;
			place = place + 1;
		}
	}

	int k;
	string slotAe, slotCe;
	for (k = place; k < total; k++)
	{
		slotAe = "PtmrA" + k;
		slotCe = "PtmrC" + k;
		tinfo.(slotAe).text = "";
		tinfo.(slotCe).text = "";
	}
}

#event_handler(EVENT_LOCATION_LOAD,"procLoadIntoNew");

// LEO: Только границы жизненного цикла и смена геометрии; список, FX и таймеры персонажей не пересоздаём.
void ActivePerks_ApplyLayout(ref layout)
{
	aref showParam;
	makearef(showParam,objActivePerkShower.ShowParam);
	CopyAttributes(showParam,layout);
	if(IsEntity(&objActivePerkShower)) SendMessage(&objActivePerkShower,"l",MSG_ACTIVE_PERK_ICON_REFRESH);
}

void InitActivePerkShower()
{
	object layout;
	ActivePerks_SetLayout(&layout,bSeaActive && !bAbordageStarted);
	ActivePerks_ApplyLayout(&layout);
}

void procLoadIntoNew()
{

	if(bNewHUDRebuild && IsEntity(&objActivePerkShower)) return;
	UnloadActivePerkShower();
	SetEventHandler("frame","LoadActivePerkShower",1);
}

void LoadActivePerkShower()
{
	DelEventHandler("frame","LoadActivePerkShower");

	DeleteAttribute(&objActivePerkShower,"PerkList.list");
	DeleteAttribute(&objActivePerkShower,"Textures");

	objActivePerkShower.PerkFX.introdur = 0.6;                       // сек: длительность вспышки на активации
	objActivePerkShower.PerkFX.shakedur = 0.5;                       // сек: длительность подпрыгивания
	objActivePerkShower.PerkFX.shakeamp = 4.0;                       // px: амплитуда подпрыгивания
	objActivePerkShower.PerkFX.introcolor = argb(110, 80, 220, 80); // вспышка зелёная, тает к нейтралу (альфа = сила)
	// LEO: Общий контрастный прогресс для старого/нового Standard и Alt; рисунок иконки остаётся виден.
	objActivePerkShower.PerkFX.maskcolor = argb(112, 92, 112, 128);
	objActivePerkShower.PerkFX.edgecolor = argb(224, 218, 230, 236);
	objActivePerkShower.PerkFX.edgeoutlinecolor = argb(192, 8, 12, 16);
	objActivePerkShower.PerkFX.edgeheight = 0.025;                  // LEO: Толщина светлой границы, доля высоты иконки; 0 отключает.
	objActivePerkShower.PerkFX.edgeoutlineheight = 0.015;           // LEO: Толщина тёмной обводки с каждой стороны, доля высоты.
	objActivePerkShower.PerkFX.outrodur   = 3.5;                     // сек: длительность вспышки "скоро готов" (затухает за это время до готовности)
	objActivePerkShower.PerkFX.outrocolor = argb(180, 100, 255, 100);// вспышка "скоро готов": ярко-зелёная, тает к нейтралу

	bool bMap = IsEntity(&worldMap);
	bool bSea = bSeaActive && !bAbordageStarted;
	if(bMap || bSea)
	{	// морская часть
		objActivePerkShower.Textures.t0.file = CommandIcons_StaticTexture("List_icons");
		objActivePerkShower.Textures.t0.horz = 16;
		objActivePerkShower.Textures.t0.vert = 8;

		if(!bMap)
		{
			markPerkFx("ImmediateReload", true);
			markPerkFx("LightRepair", true);
			markPerkFx("InstantRepair", true);
			markPerkFx("Turn180", true);
		}
	}
	else
	{	// земная часть
		objActivePerkShower.Textures.t0.file = CommandIcons_StaticTexture("LandCommands");
		objActivePerkShower.Textures.t0.horz = 16;
		objActivePerkShower.Textures.t0.vert = 4;

		markPerkFx("Rush", false);
	}

	InitActivePerkShower();
	CreateEntity(&objActivePerkShower,"ActivePerkShower");
	if(bSea && !bMap)
	{
		LayerAddObject(SEA_EXECUTE,&objActivePerkShower,-1);
		LayerAddObject(SEA_REALIZE,&objActivePerkShower,-1);
	}
	else
	{
		LayerAddObject(EXECUTE,&objActivePerkShower,-1);
		LayerAddObject(REALIZE,&objActivePerkShower,-1);
	}

	if(bMap)
	{
		if(TimeScaleCounter != 0) AddPerkToActiveList("TimeSpeed");
		return;
	}

	aref arPerksRoot,arPerk;
	makearef(arPerksRoot,pchar.perks.list);
	int perksQ = GetAttributesNum(arPerksRoot);

	string stmp;
	int idx = 0;
	for(int j=0; j<perksQ; j++)
	{
		arPerk = GetAttributeN(arPerksRoot,j);
		if( CheckAttribute(arPerk,"delay") )
		{
			if( CheckAttribute(arPerk,"active") )
			{
				string tmpStr = GetAttributeName(arPerk);

				bool seaState = sti(ChrPerksList.list.(tmpStr).seaPerk) && (bSeaActive) && (!bSeaReloadStarted);
				bool landState = (!sti(ChrPerksList.list.(tmpStr).seaPerk)) && (!bSeaActive) && (bSeaReloadStarted);
				if(seaState || landState)
				{
					AddAndUpdatePerkIcon(GetAttributeName(arPerk), GetPerkPictureIndex(GetAttributeName(arPerk))); // LEO: переподхват на статичной ячейке (маска-FX)
				}
				else
				{

					if(CheckAttribute(&ChrPerksList, "list." + tmpStr + ".UseAnimation"))
					{
						DelPerkFromActiveList(tmpStr);
					}
				}
			}
		}
	}

}

// LEO: Перепривязываем текстуры без сброса списка, кадров, владельцев и таймеров персонажей.
void ActivePerks_RefreshStyle()
{
	if(IsEntity(&worldMap))
	{
		UnloadActivePerkShower();
		LoadActivePerkShower();
		return;
	}
	if(!IsEntity(&objActivePerkShower)) return;
	DelEventHandler("frame","LoadActivePerkShower");
	object icons;
	aref list,icon,restored;
	if(CheckAttribute(&objActivePerkShower,"PerkList.list"))
	{
		makearef(list,objActivePerkShower.PerkList.list);
		CopyAttributes(&icons,list);
	}
	DeleteClass(&objActivePerkShower);
	objActivePerkShower.Textures.t0.file=GetPerkTextureName("TimeSpeed");
	DeleteAttribute(&objActivePerkShower,"PerkList.list");
	InitActivePerkShower();

	CreateEntity(&objActivePerkShower,"ActivePerkShower");
	if(bSeaActive && !bAbordageStarted)
	{
		LayerAddObject(SEA_EXECUTE,&objActivePerkShower,-1);
		LayerAddObject(SEA_REALIZE,&objActivePerkShower,-1);
	}
	else
	{
		LayerAddObject(EXECUTE,&objActivePerkShower,-1);
		LayerAddObject(REALIZE,&objActivePerkShower,-1);
	}
	int count=GetAttributesNum(&icons);
	for(int i=0;i<count;i++)
	{
		icon=GetAttributeN(&icons,i);
		string key=GetAttributeName(icon);
		makearef(restored,objActivePerkShower.PerkList.list.(key));
		CopyAttributes(restored,icon);
		int texture=GetTextureIndex(GetPerkTextureName(key));
		if(texture>=0) restored.texture=texture;
		int initialPicture=sti(restored.pic_idx);
		if(CheckAttribute(restored,"old_pic_idx")) restored.pic_idx=restored.old_pic_idx;
		restored.old_texture=restored.texture;
		restored.new_texture=restored.texture;
		restored.new_pic_idx=restored.pic_idx;
		SendMessage(&objActivePerkShower,"lsa",MSG_ACTIVE_PERK_LIST_REFRESH,"add",restored);

		if(CheckAttribute(restored,"fxfill")) PerkFx_Update(key,stf(restored.fxfill),false,false);

		restored.pic_idx=initialPicture;
	}
	PerkTimers_Refresh();
}

// LEO: лёгкая регистрация перка под маску-FX (вместо addAnimationToPerk): иконка из основного атласа
// (GetPerkTextureName/GetPerkPictureIndex), спрайт-атлас не нужен. UseAnimation/seaPerk оставляем — жизненный цикл (висит весь актив+откат) + ветки sea/land.
void markPerkFx(string perkID, bool seaPerk)
{
	ChrPerksList.list.(perkID).UseAnimation = true;
	ChrPerksList.list.(perkID).seaPerk = seaPerk;
}

/*!
 * \brief Добавить поддержку анимации для перка. Так же необходимо указать пути к атласам в функции "GetPerkTextureName" или реализовать
 * возврат свойства objActivePerkShower.Textures.tx.file в зависимости от перка
 * \param perkID идентификатор перка
 * \param perkIconPath путь к атласу иконок
 * \param perkAltIconPath путь к атласу иконок альтернативного интерфейса
 * \param perkIconCnt Количество иконок в файле .dds для анимации перка, необходимо для корректного вызова обновления иконки перка в perks.c
 * \param horzCnt кол-во иконок в атласе по горизонтали
 * \param vertCnt кол-во иконок в атласе по вертикали
 * \param seaPerk тип перка, true - морской, false - земной. Для фикса подмены перков между переходами локаций
 */
void addAnimationToPerk(string perkID, string perkIconPath, string perkAltIconPath, int perkIconCnt, int horzCnt, int vertCnt, bool seaPerk)
{
	int tmpCnt = animPerkCnt + 1;
	for (int i = 0; i < tmpCnt; i++)
	{
		if(!CheckAttribute(&objActivePerkShower, "Textures.t" + its(i)))
		{
			string tmpStr = "t" + its(i);
			if(InterfaceStates.AltIntIcons) objActivePerkShower.Textures.(tmpStr).file = perkAltIconPath;
			else objActivePerkShower.Textures.(tmpStr).file = perkIconPath;
			objActivePerkShower.Textures.(tmpStr).horz = horzCnt;
			objActivePerkShower.Textures.(tmpStr).vert = vertCnt;

			ChrPerksList.list.(perkID).UseAnimation = true;

			ChrPerksList.list.(perkID).UseAnimationIconCnt = perkIconCnt;

			ChrPerksList.list.(perkID).FPS = its(((sti(ChrPerksList.list.(perkID).TimeDelay) - sti(ChrPerksList.list.(perkID).TimeDuration)) / sti(ChrPerksList.list.(perkID).UseAnimationIconCnt)));

			ChrPerksList.list.(perkID).AnimPerkCnt = i;
			ChrPerksList.list.(perkID).seaPerk = seaPerk;
			animPerkCnt = animPerkCnt + 1;
			break;
		}
		tmpCnt = tmpCnt + 1;
	}
}

void UnloadActivePerkShower()
{
	DeleteClass(&objActivePerkShower);
	DeleteAttribute(&objActivePerkShower,"PerkList");
}

bool IsPerkIntoList(string perkID)
{
	return CheckAttribute(&objActivePerkShower,"PerkList.list."+perkID);
}

int GetPerkPictureIndex(string perkName)
{
	switch(perkName)
	{
		case "TimeSpeed":
	        if(IsEntity(&worldMap) || (bSeaActive && !bAbordageStarted))
			{
				return 59;
			}
			else
			{
			    return 15;
			}
		break;
		case "Sink":			return 43; break;
		case "Repair":			return 55; break;

		case "LightRepair":		return 54; break;

		case "ImmediateReload":	return 53; break;
		case "InstantRepair":	return 55; break;
		case "BoardingRetry":	return 30; break;
		case "Turn180":			return 56; break;

		case "Rush":			return 28; break;
	}

	if(CheckAttribute(&objActivePerkShower, "PerkList.list."+(perkName)) && CheckAttribute(&ChrPerksList, "list." + perkName + ".UseAnimation"))
	{
		return 0;
	}
	trace("WARNING!!! Perk name - "+perkName+" hav`t picture");
	return 0;
}

string GetPerkTextureName(string perkName)
{
	string family="";
	switch(perkName)
	{
		case "TimeSpeed":
			family="LandCommands";
			if(IsEntity(&worldMap) || (bSeaActive && !bAbordageStarted)) family="List_icons";
		break;
		case "Sink": family="List_icons"; break;
		case "Repair": family="List_icons"; break;
		case "InstantBoarding": family="List_icons"; break;
		case "BoardingRetry": family="List_icons"; break;
		case "LightRepair": family="List_icons"; break;
		case "ImmediateReload": family="List_icons"; break;
		case "InstantRepair": family="List_icons"; break;
		case "Turn180": family="List_icons"; break;
		case "Rush": family="LandCommands"; break;
	}
	if(family!="") return CommandIcons_StaticTexture(family);

	if(CheckAttribute(&objActivePerkShower, "PerkList.list."+(perkName)) && CheckAttribute(&ChrPerksList, "list." + perkName + ".UseAnimation") && CheckAttribute(&ChrPerksList, "list." + perkName + ".AnimPerkCnt"))
	{
		string tmpStr = "t" + its(ChrPerksList.list.(perkName).AnimPerkCnt);
		return objActivePerkShower.Textures.(tmpStr).file;
	}
	trace("WARNING!!! Perk name - "+perkName+" hav`t texture");
	return "";
}

int GetTextureIndex(string texName)
{
	aref arTexList,arCurTex;
	makearef(arTexList,objActivePerkShower.Textures);
	int i,q;

	q = GetAttributesNum(arTexList);
	for(i=0; i<q; i++)
	{
		arCurTex = GetAttributeN(arTexList,i);
		if(arCurTex.file == texName) return i;
	}
	trace("WARNING!!! Texture name - "+texName+" hav`t describe");
	return -1;
}

/*!
 * \brief Отображает иконку активного перка в правую часть экрана
 * \param perkID идентификатор перка
 */
// LEO: Послать движку FX-состояние: fill 0..1 = доля немаскированной части, не признак действия/отката.
// LEO: Актив убывает 1->0, обычный откат растёт 0->1; BoardingRetry передаёт остаток и убывает 1->0.
// intro=true — вспышка+шейк при появлении (активация); outro=true — вспышка "скоро готов" в конце отката.
// Маска/вспышки рисуются в active_perk_shower.cpp (переиспользуемый аниматор иконок).
void PerkFx_Update(string perkID, float fill, bool intro, bool outro)
{
	if( !IsEntity(&objActivePerkShower) ) return;
	aref arRoot, arCur;
	makearef(arRoot, objActivePerkShower.PerkList.list);
	if( !CheckAttribute(arRoot, perkID) ) return;
	makearef(arCur, arRoot.(perkID));
	arCur.fxfill = fill;
	if(intro) arCur.fxintro = 1;
	else      arCur.fxintro = 0;
	if(outro) arCur.fxoutro = 1;
	else      arCur.fxoutro = 0;
	SendMessage(&objActivePerkShower,"lsa",MSG_ACTIVE_PERK_LIST_REFRESH,"fx",arCur);
}

void AddPerkToActiveList(string perkID)
{
	if( !IsEntity(&objActivePerkShower) ) return;
	aref arRoot,arCur;
	makearef(arRoot,objActivePerkShower.PerkList.list);
	if( CheckAttribute(arRoot,perkID) ) return;

	makearef(arCur,arRoot.(perkID));
	arCur.texture = GetTextureIndex(GetPerkTextureName(perkID));
	arCur.pic_idx = GetPerkPictureIndex(perkID);

	arCur.old_texture = arCur.texture;
	arCur.old_pic_idx = arCur.pic_idx;
	arCur.start_pic_idx = arCur.pic_idx;
	SendMessage(&objActivePerkShower,"lsa",MSG_ACTIVE_PERK_LIST_REFRESH,"add",arCur);
	PerkTimers_Refresh();

	if(CheckAttribute(&ChrPerksList, "list." + perkID + ".UseAnimation"))
		PerkFx_Update(perkID, 1.0, true, false);
}

void DelPerkFromActiveList(string perkID)
{
	aref arRoot,arCur;
	makearef(arRoot,objActivePerkShower.PerkList.list);
	if( !CheckAttribute(arRoot,perkID) ) return;

	makearef(arCur,arRoot.(perkID));

	if(perkID != "TimeSpeed")
	{
		UpdatePerkIcon(perkID, arCur.start_pic_idx);
	}
	SendMessage(&objActivePerkShower,"lsa",MSG_ACTIVE_PERK_LIST_REFRESH,"del",arCur);
	DeleteAttribute(arRoot,perkID);
	PerkTimers_Refresh();
}

/*!
 * \brief Обновляет иконку перка. Используется для отрисовки анимации времени отката перка
 * \param perkID: string строка идентификатора перка, например "ImmediateReload"
 * \param iconID: идентификатор иконки в массиве текстур, следите чтобы кол-во
 * иконок в файле соответствовало iconID
 */
void UpdatePerkIcon(string perkID, int iconID)
{
	aref arRoot,arCur;
	makearef(arRoot,objActivePerkShower.PerkList.list);
	if( !CheckAttribute(arRoot,perkID) ) return;
	makearef(arCur,arRoot.(perkID));

	arCur.new_texture = GetTextureIndex(GetPerkTextureName(perkID));
	arCur.new_pic_idx = iconID;
	SendMessage(&objActivePerkShower,"lsa",MSG_ACTIVE_PERK_LIST_REFRESH,"upd",arCur);

	arCur.old_texture = arCur.new_texture;
	arCur.old_pic_idx = arCur.new_pic_idx;
}

/*!
 * \brief Добавить перк в список с указанием иконки.
 * Используется для установки нужной иконки прогресса после перезагрузки,
 * т.к. теряется вызов "AddPerkToActiveList" и иконка пропадает с экрана
 * \param perkID идентификатор перка
 * \param iconID номер иконки в атласе перка
 */
void AddAndUpdatePerkIcon(string perkID, int iconID)
{
	if( !IsEntity(&objActivePerkShower) ) return;
	aref arRoot,arCur;
	makearef(arRoot,objActivePerkShower.PerkList.list);
	if( CheckAttribute(arRoot,perkID) ) return;

	makearef(arCur,arRoot.(perkID));
	arCur.texture = GetTextureIndex(GetPerkTextureName(perkID));
	arCur.pic_idx = iconID;

	arCur.old_texture = arCur.texture;
	arCur.old_pic_idx = arCur.pic_idx;
	arCur.start_pic_idx = arCur.pic_idx;
	SendMessage(&objActivePerkShower,"lsa",MSG_ACTIVE_PERK_LIST_REFRESH,"add",arCur);
	PerkTimers_Refresh();
}

/*!
 * \brief Возвращает текущий кадр иконки перка. Используется для анимированных перков
 * \param perkID: string строка идентификатора перка, например "ImmediateReload"
 * \return int номер кадра из .dds для анимированных перков
 */
int getPerkCurrentIconID(string perkID)
{
	aref arRoot,arCur;
	makearef(arRoot,objActivePerkShower.PerkList.list);
	if( !CheckAttribute(arRoot,perkID) ) return 0;
	makearef(arCur,arRoot.(perkID));
	return sti(arCur.old_pic_idx);
}

/*!
 * \brief Возвращает наличие иконки перка на экране.
 * Используется для установки нужной иконки прогресса после перезагрузки
 * \param perkID идентификатор перка
 * \return true перк есть на экране (перезагрузки не было)
 * \return false перка на экране нет (была перезагрузка)
 */
bool isPerkInActiveList(string perkID)
{
	return CheckAttribute(&objActivePerkShower, "PerkList.list."+(perkID));
}

/*!
 * \brief Возвращает отношение кадров к времени действия перка,
 * т.е. сколько секунд приходится на один кадр (процесс обработки
 * перков запускается раз в секунду)
 * \param perkID идентификатор перка
 * \return int число секунд на один кадр
 */
int getAnimFpsCnt(string perkID)
{
	return sti(ChrPerksList.list.(perkID).FPS);
}
