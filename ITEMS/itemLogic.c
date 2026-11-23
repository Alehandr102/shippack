#include "items\items.h"

object randItemModels[MAX_LOADED_RANDITEMS];
object itemModels[ITEMS_QUANTITY];
object buttonModels[MAX_BUTTONS];

aref useLocators[MAX_BUTTONS];
aref useAttrs[MAX_BUTTONS];
int  useLocatorsCount=0;

int iScriptItemStartNum=0;
int iScriptItemEndNum=0;
int iScriptItemCount=0;

float SPAWN_TIME = 168.0; //hours = 168.0

void Items_LoadModel (ref _itemModel, ref _item)
{
	if (!IsEntity(_itemModel))
	{
		string itemFolder = "items";
		CreateEntity(&_itemModel, "MODELR");
		if (CheckAttribute(_item, "folder"))
			itemFolder = _item.folder;
		SendMessage(&_itemModel, "ls", MSG_MODEL_SET_DIRPATH, itemFolder+"\");
		SendMessage(&_itemModel, "ls", MSG_MODEL_LOAD_GEO, itemFolder + "\" + _item.model);
		SendMessage(&_itemModel, "ls", MSG_MODEL_SET_DIRPATH,"");
		SendMessage(&_itemModel, "ls", MSG_MODEL_SET_TECHNIQUE, "RandItem");
		LayerAddObject(EXECUTE, &_itemModel, ITEMS_LAYER);
		LayerAddObject(REALIZE, &_itemModel, ITEMS_LAYER);

	}
}

void Item_OnLoadLocation(string currentLocation)
{

	// precache active locators in current location
	aref   activeLocation;
	ref chr = GetMainCharacter();
	int currentLocationIndex = FindLocation(currentlocation);
	chr.itemLocationIndex = currentLocationIndex;
	aref aloc, an, al;
	useLocatorsCount = 0;
	makearef(activeLocation, Locations[currentLocationIndex]);
	makearef (aloc, activeLocation.items);
	for (int i=0; i<GetAttributesNum(aloc); i++)
	{
		an = GetAttributeN(aloc, i);
		if (!CheckAttribute(an,"locator"))
			continue;
		if (!FindLocator(activeLocation.id, an.locator, &al, true))
			continue;

		useLocators[useLocatorsCount] = al;
		useAttrs[useLocatorsCount] = an;
		useLocatorsCount++;
	}

	// load buttons & items
	Items_ShowButtons(currentLocation);
	RandItems_OnLoadLocation(activeLocation);
	Box_OnLoadLocation(activeLocation);

	for (int itemN=0; itemN<ITEMS_QUANTITY; itemN++)
	{
		if (!CheckAttribute(Items[itemN], "startLocator"))
			continue;
		if (!CheckAttribute(Items[itemN], "startLocation"))
			continue;
		if (!CheckAttribute(Items[itemN], "shown"))
			continue;
		if (Items[itemN].shown == "1")
		{
		if (Items[itemN].startLocation == currentLocation)
		{
			Items_ShowItem(itemN);
		}}
	}
}

void Item_OnUnLoadLocation()
{
	for (int itemN=0; itemN<ITEMS_QUANTITY; itemN++)
	{
		if (IsEntity(&itemModels[itemN]))
			DeleteClass(&itemModels[itemN]);
	}
	for (int buttonN=0; buttonN<MAX_BUTTONS; buttonN++)
	{
		if (IsEntity(&buttonModels[buttonN]))
			DeleteClass(&itemModels[buttonN]);
	}

	RandItems_OnUnloadLocation();

	ref chr = GetMainCharacter();
	chr.itemLocationIndex = -1;
}

void Item_OnEnterLocator(aref _location, string _locator)
{
	ref chr = GetMainCharacter();
	int itemN;
	// Gregg: полный обход массива вместо диапазона ItemsForLocators_start..end.
	// Границы задаются в InitItems вокруг непрерывного блока спец-предметов и, как
	// сохраняемые глобалы, приезжают из сейва - новый локаторный предмет, дописанный
	// в ХВОСТ массива миграцией, в диапазон не попадал и молча не размещался.
	// Фильтры ниже (startLocator/useLocator/shown) и так отсеивают всё лишнее,
	// а скан идёт только на входе в локатор, не покадрово.
	int itemsTotal = GetArraySize(&Items);

	if (findsubstr(_locator, "button", 0) != -1)
	{ // use item
        for (itemN = 0; itemN < itemsTotal; itemN++)
		{
			if (!CheckAttribute(Items[itemN], "useLocator"))
				continue;
			if (!CheckAttribute(Items[itemN], "useLocation"))
				continue;
			if (!CheckAttribute(Items[itemN], "shown"))
				continue;
			if (Items[itemN].shown == "0")
			{
			if (Items[itemN].useLocation == _location.id)
			{
			if (Items[itemN].useLocator == _locator)
			{
				Log_SetActiveAction("Action");
				chr.activeItem = itemN;
				chr.activeLocator = _locator;
				chr.activeRandItem = false;
				chr.activeItem.pick = false;
			}}}
		}
	}
	else
	{
		for (itemN = 0; itemN < itemsTotal; itemN++)
		{
			if (!CheckAttribute(Items[itemN], "startLocator"))
				continue;
			if (!CheckAttribute(Items[itemN], "startLocation"))
				continue;
			if (!CheckAttribute(Items[itemN], "shown"))
				continue;
			if (Items[itemN].startLocator == _locator)
			{
			if (Items[itemN].shown == "1")
			{
			if (Items[itemN].startLocation == _location.id)
			{
				if (!CheckAttribute(pchar,"CantTakeItems"))
				{
					Log_SetActiveAction("Pick");
					chr.activeItem = itemN;
					chr.activeLocator = _locator;
					chr.activeRandItem = false;
					chr.activeItem.pick = true;
				}
			}}}
		}
	}
    // ===> eddy. проверка вхождения ГГ в локаторы группы Item.
    QuestCheckEnterLocItem(_location, _locator);
    // <===
	BLI_RefreshCommandMenu();
}

void Item_OnExitLocator(aref _location, string _locator)
{
	Log_SetActiveAction("Nothing");
	Characters[GetMainCharacterIndex()].activeItem = "";
	BLI_RefreshCommandMenu();
    // ===> eddy. проверка выхождения ГГ из локаторов группы Item.
    QuestCheckExitLocItem(_location, _locator);
    // <===
}

void Item_OnPickItem()
{
	if (LAi_IsFightMode(pchar)) {PlaySound("interface\knock.wav"); return;}
	aref activeLocation;
	ref chr = GetMainCharacter();
	makearef(activeLocation, Locations[sti(chr.itemLocationIndex)]);

	// Gregg: квестовый гейт на подбор (нефритовый череп - пока в храме жива стража). Отказ ДО
	// Log_SetActiveAction("Nothing"), чтобы подсказка "взять" осталась висеть и предмет можно было
	// попробовать снова. Рандом-предметы (activeRandItem) не гейтим - там нет квестовых айди.
	// Сравнение только через "== true" (проверенная идиома этого файла): "!= true" на атрибуте ненадёжно.
	bool bRandItem = false;
	if (CheckAttribute(chr, "activeRandItem"))
	{
		if (chr.activeRandItem == true) bRandItem = true;
	}
	if (!bRandItem)
	{
		if (!QuestCheckCanTakeItem(activeLocation, Items[sti(chr.activeItem)].id))
		{
			PlaySound("interface\knock.wav");
			return;
		}
	}
	Log_SetActiveAction("Nothing");

	int langFile = LanguageOpenFile("ItemsDescribe.txt");
	string displayItemName, youvegotString;
	youvegotString = LanguageConvertString(langFile, "youve_got");
	PlayStereoSound("interface\important_item.wav");

	if (chr.activeRandItem == true)
	{
		string activeRandItemAttribute = "RandItemType"+sti(chr.activeItem);
		if (CheckAttribute(activeLocation, activeRandItemAttribute))
		{
			SendMessage(&randItemModels[sti(chr.activeItem)], "lslff", MSG_MODEL_BLEND, "blenditemlit", 1000, 1.0, 0.0);
			GenerateAndAddItems(GetMainCharacter(), Items[sti(activeLocation.(activeRandItemAttribute))].id, 1);
			displayItemName = LanguageConvertString(langFile, Items[sti(activeLocation.(activeRandItemAttribute))].name);
			Log_SetStringToLog(youvegotString+" "+displayItemName+"!");

			activeLocation.(activeRandItemAttribute) = -1;
		}
	}
	else
	{
		int activeItem = sti(chr.activeItem);
		Items[activeItem].shown = false;
		SendMessage(&itemModels[activeItem], "lslff", MSG_MODEL_BLEND, "blenditemlit", 1000, 1.0, 0.0);
		GenerateAndAddItems(GetMainCharacter(), Items[activeItem].id, 1);

		displayItemName = LanguageConvertString(langFile, Items[activeItem].name);
		Log_SetStringToLog(youvegotString+" "+displayItemName+"!");
		// ===> перехват взятия предметов из локатора item на метод обрабоки для квестовых нужд
		QuestCheckTakeItem(activeLocation, Items[activeItem].id);
		// <===
	}
	DeleteAttribute(chr,"activeItem");
	LanguageCloseFile(langFile);
	LandRadar_BuildPOI(); // LEO этап 2б: предмет подобран — обновить радар (блип исчезает сразу)
}

void Item_OnUseItem()
{
	if (LAi_IsFightMode(pchar)) {PlaySound("interface\knock.wav"); return;}
	aref aloc, an, al;
	aref activeLocation;
	ref chr = GetMainCharacter();
	int activeItem = sti(chr.activeItem);
	makearef(activeLocation, Locations[sti(chr.itemLocationIndex)]);

	makearef (aloc, activeLocation.items);
	// boal баг! нет предмета, а он работает!!!
    if (!CheckCharacterItem(chr, Items[activeItem].id))
    {
		Log_SetStringToLog(XI_ConvertString("You have not need item"));
        PlaySound("interface\knock.wav");
        return;
    }
    Log_SetActiveAction("Nothing");
	Items[activeItem].shown = true;
	Items[activeItem].shown.used = true; //использован
	Items_LoadModel(&itemModels[activeItem], &Items[activeItem]);

	FindLocator(activeLocation.id, chr.activeLocator, &al, true);
	SendMessage(&itemModels[activeItem], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y), makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));
	SendMessage(&itemModels[activeItem], "lslff", MSG_MODEL_BLEND, "blenditem", 1000, 0.0, 1.0);

	SetEventHandler("frame", "Item_OnUseFrame", 0);
	TakeItemFromCharacter(GetMainCharacter(), Items[activeItem].id);
	Items[activeItem].startLocator = "";
	al.active = true;
	al.timePassed = 0;

	int langFile = LanguageOpenFile("ItemsDescribe.txt");
	string displayItemName, youvegotString;
	youvegotString = LanguageConvertString(langFile, "used_item");
	displayItemName = LanguageConvertString(langFile, Items[activeItem].name);
	Log_SetStringToLog(youvegotString+" "+displayItemName+"!");
    PlaySound("interface\sobitie_na_karte_001.wav");

    // ===> перехват на метод обрабоки для квестовых нужд при опускании предмета в локаторы button. Эдди.
    QuestCheckUseButton(activeLocation, chr.activeLocator, Items[activeItem].id);
    // <===
	DeleteAttribute(chr,"activeItem");

	CompleteQuestName("OnUse_"+Items[activeItem].id, "");
}

void Item_OnUseFrame()
{
	aref aloc, an, al;
	aref activeLocation;
	ref chr = GetMainCharacter();

    if (CheckAttribute(chr, "itemLocationIndex") && sti(chr.itemLocationIndex) > 0) // boal fix 230804
    {
    	makearef(activeLocation, Locations[sti(chr.itemLocationIndex)]);

    	makearef (aloc, activeLocation.items);
    	bool usedOnFrame=false;
    	for (int i=0; i<useLocatorsCount; i++)
    	{
    		an = useAttrs[i];
    		al = useLocators[i];
    		if (!CheckAttribute(al, "active"))
    			continue;
    		if (al.active != "1")
    			continue;
    		int timePassed = sti(al.timePassed) + GetDeltaTime();
    		al.timePassed = timePassed;
    		if (timePassed > BUTTON_ACTIVATION_TIME)
    		{
    			al.active = false;
    			continue;
    		}

    		usedOnFrame = true;
    		float timeK = makefloat(timePassed) / makefloat(BUTTON_ACTIVATION_TIME);
    		float deltaY = makefloat(an.deltaY) * timeK;

    		SendMessage(&buttonModels[i], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y)+deltaY, makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));

    		for (int j=0; j<ITEMS_QUANTITY; j++)
    		{
    			if (Items[j].useLocator == an.locator)
    			{
    				SendMessage(&itemModels[j], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y)+makeFloat(an.itemDeltaY)+deltaY, makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));
    				break;
    			}
    		}
    	}
    }
    else
    {
        if (MOD_BETTATESTMODE == "On") Log_Info("Error: Item_OnUseFrame нет mch.itemLocationIndex ");
    }
	if (!usedOnFrame)
		DelEventHandler("frame", "Item_OnUseFrame");
}

void Items_ShowButtons(string _locationName)
{
	aref al;
	for (int i=0; i<useLocatorsCount; i++)
	{
		Items_LoadModel(&buttonModels[i], &useAttrs[i]);
		al = useLocators[i];
		if (useAttrs[i].trigged != "1")
			SendMessage(&buttonModels[i], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y), makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));
		else
			SendMessage(&buttonModels[i], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y)+makefloat(useAttrs[i].deltaY), makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));
	}
}

void Items_ShowItem(int _itemN)
{
	aref al;
	aref activeLocation;
	ref chr = GetMainCharacter();
	makearef(activeLocation, Locations[sti(chr.itemLocationIndex)]);

	if (Items[_itemN].model == "")
	{
		Trace("ItemLogic: no model for item "+activeLocation.id+"."+Items[_itemN].id);
		return;
	}

	if (Items[_itemN].startLocator != "")
	{ //unused
		Items_LoadModel(&itemModels[_itemN], &Items[_itemN]);

		if (!FindLocator(Items[_itemN].startLocation, Items[_itemN].startLocator, &al, true)) 
		{Trace("ItemLogic: locator for item "+_itemN+" not found! ["+Items[_itemN].startLocation+">>"+Items[_itemN].startLocator);}
		else 
		{
			Trace ("ItemLogic: showing item at "+al.x+", "+al.y+", "+al.z);
			SendMessage(&itemModels[_itemN], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y), makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));
		}
	}
	else
	{ //used
		if (Items[_itemN].useLocation != activeLocation.id)
			return;

		Items_LoadModel(&itemModels[_itemN], &Items[_itemN]);

		if (FindLocator(Items[_itemN].useLocation, Items[_itemN].useLocator, &al, true)) 
		{
			SendMessage(&itemModels[_itemN], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y), makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));
		}
	}
}

void Items_HideItem(int itemN)
{
}

int Items_FindItem(string itemID, ref itemARef)
{
	int index = GetItemIndex(itemID);
	if (index < 0)
	{
		return -1;
	}

    aref curItem;
	makearef(curItem, Items[index]);
	itemARef = curItem;

	return index;
}

int Items_FindItemIdx(string itemID) // нужно для поиска только номера
{
	return GetItemIndex(itemID);
}

// ************** RANDOM ITEMS *****************
void RandItems_OnLoadLocation(ref _location)
{
	string locatorName;
	aref randItemLocator;
	for (int i=1; i<MAX_LOADED_RANDITEMS; i++)
	{
		string lastSpawnTimeString = "LastSpawnTime"+i;
		string randItemTypeString = "RandItemType"+i;
		locatorName = "randitem"+i;
		if (!FindLocator(_location.id, locatorName, &randItemlocator, true))
			break;

        // boal -->
        if (!SetLocationQuestRandItem(i, _location, locatorName, randItemLocator))
        {
        // boal <--
            // randitem locator found
    		if (CheckAttribute(_location, lastSpawnTimeString))
    		{ // already spawned?
    			if (CheckAttribute(_location, randItemTypeString))
    			{
    				if (sti(_location.(randItemTypeString)) != -1)
    				{
    					RandItems_DrawRandItem(i, _location, randItemLocator);
    				}
    				else
    				{ //already picked up, try to respawn
    					if ((stf(_location.(lastSpawnTimeString))+SPAWN_TIME) < Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear()))
    					{
    						RandItems_SpawnRandItem(i, _location, randItemLocator);
    					}
    				}
    			}
    			else
    			{ // nothing here
    				if ((stf(_location.(lastSpawnTimeString))+SPAWN_TIME) < Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear()))
    				{
    					RandItems_SpawnRandItem(i, _location, randItemLocator);
    				}
    			}
    		}
    		else
    		{
    			RandItems_SpawnRandItem(i, _location, randItemLocator);
    		}
		}
	}
}

void RandItems_OnUnloadLocation()
{
	for (int itemN=0; itemN<MAX_LOADED_RANDITEMS; itemN++)
	{
		if (IsEntity(&randItemModels[itemN]))
			DeleteClass(&randItemModels[itemN]);
	}
}

float Items_MakeTime (float _h, float _d, float _m, float _y)
{
	return (_h+24.0*(_d + 30.0*(_m + 12.0*_y)));
}

void RandItems_SpawnRandItem(int _index, aref _location, aref al)
{
	aref randItem;
	string lastSpawnTimeString;
	int n;

	string alName = GetAttributeName(al);
	string alNameAttribute = "items."+alName;
	if (CheckAttribute(_location, alNameAttribute))
	{
		n = Items_FindItem(_location.items.(alName), &randItem);
		if (n == -1)
			return;

		lastSpawnTimeString = "LastSpawnTime"+_index;
		_location.(lastSpawnTimeString) = Items_MakeTime(0, 15, 1, 2003);
	}
	else
	{
		n = Items_FindItem(RandItems[rand(RANDITEMS_QUANTITY-1)].id, &randItem);
		if (n == -1)
			return;

		lastSpawnTimeString = "LastSpawnTime"+_index;
		_location.(lastSpawnTimeString) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
	}

    if (GetCharacterSkillToOld(GetMainCharacter(), SKILL_FORTUNE) < rand(15)) return; // boal fix

	if (!CheckAttribute(randItem, "model") || randItem.model == "")
	{
		Trace("ItemLogic: no model for item "+_location.id+"."+randItem.id);
		return;
	}

	Items_LoadModel(&randItemModels[_index],  randItem);
	SendMessage(&randItemModels[_index], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y), makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));

	lastSpawnTimeString = "RandItemType"+_index;
	_location.(lastSpawnTimeString) = n;
}

void RandItems_DrawRandItem(int _index, aref _location, aref al)
{
	string randItemTypeString = "RandItemType"+_index;
	int n = sti(_location.(randItemTypeString));

    if (GetCharacterSkillToOld(GetMainCharacter(), SKILL_FORTUNE) < rand(15)) return; // boal fix

	if (Items[n].model == "")
	{
		Trace("ItemLogic: no model for item "+_location.id+"."+Items[n].id);
		return;
	}

	Items_LoadModel(&randItemModels[_index],  &Items[n]);
	SendMessage(&randItemModels[_index], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y), makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));
}

void RandItem_OnEnterLocator(aref _location, string _locator)
{

    int randIndex = sti(strcut(_locator, 8, strlen(_locator)-1));
	string randItemAttribute = "RandItemType"+randIndex;
	ref chr = GetMainCharacter();
	if (CheckAttribute(_location, randItemAttribute))
	{
		int currentType = sti(_location.(RandItemAttribute));
		if (currentType != -1)
		{
			Log_SetActiveAction("Pick");
			chr.activeItem = randIndex;
			chr.activeLocator = _locator;
			chr.activeRandItem = true;
			chr.activeItem.pick = true;
			BLI_RefreshCommandMenu();
		}
	}
}

// ****************** BOXES ********************
void Box_EnterToLocator(aref loc, string locName)
{
	if(!CheckAttribute(loc,locName)) return;
	
	if(CheckAttribute(loc, "id"))
	{
		if(loc.id == "SantaQuiteriaInside")
		{
			if(PChar.MysteriousIsland.DisableBoxOpen == true)
			{
				Log_SetStringToLog(GetConvertStrWithReplace("Variable_itemLogic_1", "Logs.txt", "#space#", " "));
				PlaySound("interface\box_locked.wav");
			}
		}
	}

	if(HasSubStr(locName, "private"))
	{
		// check if private box opened
		if(!CheckAttribute(loc, locName+".opened"))
		{
			if(CheckAttribute(loc, locName+".key"))
			{
				if(CheckCharacterItem(pchar, loc.(locName).key)) //проверяем ключ
				{
					loc.(locName).opened = true;
					PlaySound("interface\key.wav");

					if(CheckAttribute(loc, locName+".key.delItem"))
					{
						TakeItemFromCharacter(pchar, loc.(locName).key); //забрать ключ
					}
				}
				else
				{
					Log_SetStringToLog(XI_ConvertString("You have not need key"));
					PlaySound("interface\box_locked.wav");
					return;
				}
			}
			else
			{
				if(CheckAttribute(loc, locName+".closed")) //проверяем, не закрыт ли сундук
				{
					Log_SetStringToLog(XI_ConvertString("Box is closed"));
					PlaySound("interface\door_locked.wav");
					return;
				}
			}
		}
	}

	// Warship 15.08.09 Проверка на квестовую закрытость сундуков (не только приватов, вообще всех)
	if(CheckAttribute(loc, locName + ".QuestClosed"))
	{
		Log_SetStringToLog(XI_ConvertString("Box is closed"));
		PlaySound("interface\door_locked.wav");
		return;
	}

		pchar.boxname = locName;
		Log_SetActiveAction("OpenBox");
		BLI_RefreshCommandMenu();
	}

void Box_ExitFromLocator(aref loc, string locName)
{
	DeleteAttribute(GetMainCharacter(),"boxname");
	Log_SetActiveAction("Nothing");
	BLI_RefreshCommandMenu();
}

void OpenBoxProcedure()
{
	ref chr = GetMainCharacter();
	if( !CheckAttribute(chr,"boxname") ) return;
	int locidx = FindLoadedLocation();
	if(locidx<0) return;
	string atrName = chr.boxname;
	aref ar;
	makearef(ar,Locations[locidx].(atrName));
	if(GetAttributesNum(ar)==0)
	{
		Locations[locidx].(atrName).Money = 0;
		makearef(ar,Locations[locidx].(atrName));
	}
	// God_hit_us  это такой прикол - задействовать в ловушки для сундуков(boal)
	// токо сундуки и дома
	if (sti(chr.GenQuest.God_hit_us) == 1 && rand(100) >= (85 + GetCharacterSkillToOld(chr, SKILL_FORTUNE)) && Locations[locidx].id != "Caiman_StoreHouse" && Locations[locidx].id != "Reefs_chapter")
	{
		Log_Info(GetConvertStrWithReplace("Variable_itemLogic_2", "Logs.txt", "#space#", " "));
		PlaySound("people\clothes1.wav");
		DoQuestCheckDelay("God_hit_us", 0.2);
	}
	else
	{
		chr.boxx = atrName;
		LaunchItemsBox(&ar);
	}
}

void Box_OnLoadLocation(ref _location)
{
	string locatorName;
	aref boxLocator;
	int i;
	bool isAbordageBox = false;
	if (CheckAttribute(_location, "boarding"))
	{
		if (_location.boarding == "true")
		{
            if (isShipInside(_location.id) || _location.id == "Deck_Near_Ship")// boal fix 08.01.04
            {
                isAbordageBox = false;
            }
            else
            {
	            isAbordageBox = true;
            }
		}
	}


	// private box
	for (i=1; i < MAX_HANDLED_BOXES; i++)
	{
		locatorName = "private"+i;
		if (!FindLocator(_location.id, locatorName, &boxlocator, true))
		{
			break;
		}

		if (!CheckAttribute(_location, locatorName))
		{
			_location.(locatorName) = "";
			_location.(locatorName).open = true;
		}

		// Warship генерим предметы в сундуке
		if(!CheckAttribute(_location, locatorName + ".opened"))
		{
			makearef(boxLocator, _location.(locatorName));
			FillGenerableItemsForChest(boxLocator);
		}
	}

	// simple box
	for (i=1; i < MAX_HANDLED_BOXES; i++)
	{
		locatorName = "box"+i;
		if (!FindLocator(_location.id, locatorName, &boxlocator, true))
		{
			break;
		}

		// found box
		bool needRespawn;
		// boal -->
	    needRespawn = SetLocationQuestBox(_location, locatorName);

		if (!needRespawn)
		{
		// boal <--
    		if (!CheckAttribute(_location, (locatorName)))
    		{
    			needRespawn = true;
    			_location.(locatorName) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
    		}
    		else
    		{
				if (CheckAttribute(_location,locatorName+".notouch")) {DeleteAttribute(_location,locatorName+".notouch"); needRespawn = true; _location.(locatorName) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear()); continue;}
    			if ((sti(_location.(locatorName)) + SPAWN_TIME) < Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear()))
    			{
    				needRespawn = true;
    				_location.(locatorName) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
    			}
    			else
    			{
    				needRespawn = false;
    			}
    		}
            if (isShipInside(_location.id) || _location.id == "Deck_Near_Ship")// boal fix 08.01.04
            {
                needRespawn = false;
            }
            // перетираем общие сундуки boal 11/03/06 -->
            if (!needRespawn && CheckAttribute(_location, "MustSetReloadBack"))
            {
                //clear items in box
    			if (CheckAttribute(_location, locatorName))
    			{
					else
					{
						DeleteAttribute(_location, locatorName + ".items");
						_location.(locatorName).items = "";
						_location.(locatorName).money = 0;
					}
    			}
            }
			// <--

    		//respawn items in box
    		if (needRespawn || isAbordageBox)
    		{
				if (_location.id == "Caiman_StoreHouse" || _location.id == "Reefs_chapter") continue;
    			//clear items in box
    			if (CheckAttribute(_location, locatorName))
    			{
    				DeleteAttribute(_location, locatorName + ".items");
    				DeleteAttribute(_location, locatorName + ".Treasure"); // признак сокровища в сундуке boal
				    _location.(locatorName).items = "";
				    _location.(locatorName).money = 0;
    			}

    			//fill box with new items
    			string goodItemName;
    			int spawnItemsCount = OBJECTS_IN_BOX;
    			ref chr = GetMainCharacter();
    			spawnItemsCount = spawnItemsCount + spawnItemsCount * GetCharacterSkillToOld(chr, "Fortune") / 10.0;
    			for (int j=0; j<spawnItemsCount; j++)
    			{
    				if (needRespawn || isAbordageBox)
    				{
    					string itemID;
    					if (SpawnItem(chr, &itemID,isAbordageBox, GetCharacterSkillToOld(chr, "Fortune")))
    					{
    						if (CheckAttribute(_location, locatorName+".items."+itemID))
    						{
    							_location.(locatorName).items.(itemID) = sti(_location.(locatorName).items.(itemID)) + 1;
    						}
    						else
    						{
    							_location.(locatorName).items.(itemID) = 1;
    						}
    					}
    				}

    			}
				if (needRespawn || isAbordageBox) // это должно быть здесь когда уже известно значение переменной needRespawn
				{
					// Warship генерим предметы в сундуке
					makearef(boxLocator, _location.(locatorName));
					FillGenerableItemsForChest(boxLocator);
				}
    		}//respawn
		}

	} //for(i)
}

// Warship Для новой системы - перебор итемов для генерации, т.к. везде ложится по-старинке - items.(itemID) = n, а генерить нужно
void FillGenerableItemsForChest(aref _chest)
{
	int i, count;
	String itemID;

	aref chestItems;
	aref item;

	makearef(chestItems, _chest.items);

	for(i = 0; i < GetAttributesNum(chestItems); i++)
	{
		item = GetAttributeN(chestItems, i);
		itemID = GetAttributeName(item);

		if(IsGenerableItem(itemID))
		{
			count = sti(chestItems.(itemID));
			DeleteAttribute(chestItems, itemID);

			itemID = GetGeneratedItem(itemID);
			chestItems.(itemID) = count;
		}
	}
}

// LEO: розыгрыш содержимого сундука. Пустой sBoxLocation - лут уходит игроку в руки
// с тултипом (обычный взлом отмычками), иначе ложится в приватный сундук указанной
// локации: тот же набор нужен арене, где приз ждёт в склепе.
void ChestRollLoot(string sChestID, string sBoxLocation, string sBoxLocator)
{
	string nameitm[35];
	int itmq[35];
	int i;
	int value;
	int value_alt;
	switch (sChestID)
	{
		case "Chest_treasure":
			value = chest_treasure[sti(pchar.Chest_treasure)];
			pchar.Chest_treasure = sti(pchar.Chest_treasure) + 1;
			if (sti(pchar.Chest_treasure) > 99) pchar.Chest_treasure = 0;
			value_alt = chest_treasure2[sti(pchar.Chest_treasure2)];
			pchar.Chest_treasure2 = sti(pchar.Chest_treasure2) + 1;
			if (sti(pchar.Chest_treasure2) > 99) pchar.Chest_treasure2 = 0;
		break;
		case "Chest_Craftsmans":
			value = chest_craftsmans[sti(pchar.Chest_Craftsmans)];
			pchar.Chest_Craftsmans = sti(pchar.Chest_Craftsmans) + 1;
			if (sti(pchar.Chest_Craftsmans) > 99) pchar.Chest_Craftsmans = 0;
			value_alt = chest_craftsmans2[sti(pchar.Chest_Craftsmans2)];
			pchar.Chest_Craftsmans2 = sti(pchar.Chest_Craftsmans2) + 1;
			if (sti(pchar.Chest_Craftsmans2) > 99) pchar.Chest_Craftsmans2 = 0;
		break;
		case "Chest_ammo":
			value = chest_ammo[sti(pchar.chest_ammo)];
			pchar.chest_ammo = sti(pchar.chest_ammo) + 1;
			if (sti(pchar.chest_ammo) > 99) pchar.chest_ammo = 0;
			value_alt = chest_ammo2[sti(pchar.chest_ammo2)];
			pchar.chest_ammo2 = sti(pchar.chest_ammo2) + 1;
			if (sti(pchar.chest_ammo2) > 99) pchar.chest_ammo2 = 0;
		break;
		case "Chest_quest":
			value = chest_quest[sti(pchar.Chest_quest)];
			pchar.Chest_quest = sti(pchar.Chest_quest) + 1;
			if (sti(pchar.Chest_quest) > 99) pchar.Chest_quest = 0;
			value_alt = chest_quest2[sti(pchar.Chest_quest2)];
			pchar.Chest_quest2 = sti(pchar.Chest_quest2) + 1;
			if (sti(pchar.Chest_quest2) > 99) pchar.Chest_quest2 = 0;
		break;
	}
	int value2 = 999 - value;
	int value3 = 999 - value_alt;
	//В каждом сундуке может сгенерироваться до 10 предметов (массивы с индексом от 0 до 9) по текущей логике. Задействовано максимум 4 слота пока.

	if(sChestID == "Chest_treasure") // Малый
	{
		switch (makeint(value/111))
		{
			case 0:
				nameitm[1] = "jewelry4"; // Изумруд
				itmq[1] = 10;
				nameitm[2] = "jewelry8"; // Бронзовое кольцо
				itmq[2] = 10;
				nameitm[3] = "jewelry9"; // Бронзовый крестик
				itmq[3] = 10;
				nameitm[4] = "jewelry15"; // Изумрудные подвески
				itmq[4] = 10;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 4000*(sti(PChar.rank)/2);
			break;
			case 1:
				nameitm[1] = "jewelry1"; // Сапфир
				itmq[1] = 10;
				nameitm[2] = "jewelry2"; // Алмаз
				itmq[2] = 10;
				nameitm[3] = "jewelry3"; // Рубин
				itmq[3] = 10;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 18000*(sti(PChar.rank)/2);
			break;
			case 2:
				nameitm[1] = "jewelry5"; // Золотой слиток
				itmq[1] = 4;
				nameitm[2] = "jewelry6"; // Серебряное кольцо с сапфиром
				itmq[2] = 9;
				nameitm[3] = "jewelry7"; // Золотое кольцо с изумрудом
				itmq[3] = 8;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 12000*(sti(PChar.rank)/2);
			break;
			case 3:
				nameitm[1] = "jewelry10"; // Золотое кольцо с сапфиром
				itmq[1] = 10;
				nameitm[2] = "jewelry11"; // Большая жемчужина
				itmq[2] = 10;
				nameitm[3] = "jewelry12"; // Маленькая жемчужина
				itmq[3] = 10;
				nameitm[4] = "jewelry13"; // Камея
				itmq[4] = 10;
				nameitm[5] = "jewelry14"; // Золотая брошь
				itmq[5] = 10;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 6000*(sti(PChar.rank)/2);
			break;
			case 4:
				nameitm[1] = "jewelry16"; // Ожерелье
				itmq[1] = 10;
				nameitm[2] = "jewelry17"; // Серебряный слиток
				itmq[2] = 5;
				nameitm[3] = "jewelry18"; // Золотое кольцо с рубином
				itmq[3] = 3;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 16000*(sti(PChar.rank)/2);
			break;
			case 5:
				nameitm[1] = "jewelry5"; // Золотой слиток
				itmq[1] = 7;
				nameitm[2] = "jewelry7"; // Золотое кольцо с изумрудом
				itmq[2] = 8;
				nameitm[3] = "jewelry10"; // Золотое кольцо с сапфиром
				itmq[3] = 10;
				nameitm[4] = "jewelry14"; // Золотая брошь
				itmq[4] = 10;
				nameitm[5] = "jewelry18"; // Золотое кольцо с рубином
				itmq[5] = 6;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 10000*(sti(PChar.rank)/2);
			break;
			case 6:
				nameitm[1] = "jewelry6"; // Серебряное кольцо с сапфиром
				itmq[1] = 20;
				nameitm[2] = "jewelry17"; // Серебряный слиток
				itmq[2] = 20;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 20000*(sti(PChar.rank)/2);
			break;
			case 7:
				nameitm[1] = "jewelry6"; // Серебряное кольцо с сапфиром
				itmq[1] = 15;
				nameitm[2] = "jewelry7"; // Золотое кольцо с изумрудом
				itmq[2] = 15;
				nameitm[3] = "jewelry8"; // Бронзовое кольцо
				itmq[3] = 15;
				nameitm[4] = "jewelry10"; // Золотое кольцо с сапфиром
				itmq[4] = 15;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 7000*(sti(PChar.rank)/2);
			break;
			case 8:
				nameitm[1] = "jewelry5"; // Золотой слиток
				itmq[1] = 25;
				nameitm[2] = "jewelry11"; // Большая жемчужина
				itmq[2] = 25;
				nameitm[3] = "jewelry12"; // Маленькая жемчужина
				itmq[3] = 25;
				nameitm[4] = "jewelry17"; // Серебряный слиток
				itmq[4] = 25;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 1000*(sti(PChar.rank)/2);
			break;
			case 9:
				nameitm[1] = "jewelry8"; // Бронзовое кольцо
				itmq[1] = 10;
				nameitm[2] = "jewelry9"; // Бронзовый крестик
				itmq[2] = 10;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 15000*(sti(PChar.rank)/2);
			break;
			case 10:
				nameitm[1] = "jewelry8"; // Бронзовое кольцо
				itmq[1] = 10;
				nameitm[2] = "jewelry9"; // Бронзовый крестик
				itmq[2] = 10;
				nameitm[9] = "gold"; // Золото
				itmq[9] = 15000*(sti(PChar.rank)/2);
			break;
		}
		if (itmq[0] > 0) itmq[0] += makeint(value/50);
		if (itmq[1] > 0) itmq[1] += makeint(value/50);
		if (itmq[2] > 0) itmq[2] += makeint(value/50);
		if (itmq[3] > 0) itmq[3] += makeint(value/50);
		if (itmq[4] > 0) itmq[4] += makeint(value/50);
		if (itmq[5] > 0) itmq[5] += makeint(value/50);
		itmq[9] += makeint(value/3);
		if (value < 100) {nameitm[3] = "Mineral2"; itmq[3] = 1;}
		if (value >= 200 && value < 400) {nameitm[3] = "Mineral4"; itmq[3] = 1;}
		if (value >= 800) {nameitm[3] = "Mineral8"; itmq[3] = 1;}
	}

	if(sChestID == "Chest_Craftsmans") // Ремесленника
	{
		switch (makeint(value/83))
		{
			case 0:
				nameitm[1] = "CompCraft_Parchment"; // Бумага
				itmq[1] = 150;
			break;
			case 1:
				nameitm[1] = "CompCraft_Nails"; // Гвозди
				itmq[1] = 50;
				nameitm[2] = "CompCraft_Lead"; // Свинец
				itmq[2] = 12;
				nameitm[3] = "CompCraft_Grindstone"; // Оселок
				itmq[3] = 5+(sti(PChar.rank));
			break;
			case 2:
				nameitm[1] = "CompCraft_Magnesium"; // Магний
				itmq[1] = 10;
				nameitm[2] = "CompCraft_Flint"; // Кремень
				itmq[2] = 11;
				nameitm[3] = "CompCraft_Copper"; // Медь
				itmq[3] = 15;
				nameitm[4] = "CompCraft_Sulfur"; // Сера
				itmq[4] = 17;
			break;
			case 3:
				nameitm[1] = "CompCraft_SulfurCrush"; // Измельчённая сера
				itmq[1] = 30;
				nameitm[2] = "CompCraft_PowderMixture"; // Пороховая смесь
				itmq[2] = 31;
				nameitm[3] = "CompCraft_Parchment"; // Бумага
				itmq[3] = 32;
			break;
			case 4:
				nameitm[1] = "CompCraft_Nails"; // Гвозди
				itmq[1] = 200;
				nameitm[2] = "CompCraft_Parchment"; // Бумага
				itmq[2] = 100;
			break;
			case 5:
				nameitm[1] = "CompCraft_Copper"; // Медь
				itmq[1] = 83;
			break;
			case 6:
				nameitm[1] = "CompCraft_Flint"; // Кремень
				itmq[1] = 45+(sti(PChar.rank)*2);
			break;
			case 7:
				nameitm[1] = "CompCraft_Magnesium"; // Магний
				itmq[1] = 57;
			break;
			case 8:
				nameitm[1] = "CompCraft_Nails"; // Гвозди
				itmq[1] = 10;
				nameitm[2] = "CompCraft_Magnesium"; // Магний
				itmq[2] = 10;
				nameitm[3] = "CompCraft_Flint"; // Кремень
				itmq[3] = 10;
				nameitm[4] = "CompCraft_Lead"; // Свинец
				itmq[4] = 10;
				nameitm[5] = "CompCraft_Copper"; // Медь
				itmq[5] = 10;
				nameitm[6] = "CompCraft_Sulfur"; // Сера
				itmq[6] = 10;
				nameitm[7] = "CompCraft_SulfurCrush"; // Измельчённая сера
				itmq[7] = 10;
				nameitm[8] = "CompCraft_PowderMixture"; // Пороховая смесь сера
				itmq[8] = 10;
				nameitm[9] = "CompCraft_Parchment"; // Бумага
				itmq[9] = 10;
				nameitm[10] = "CompCraft_Grindstone"; // Оселок
				itmq[10] = 5+(sti(PChar.rank));
			break;
			case 9:
				nameitm[1] = "CompCraft_SulfurCrush"; // Измельчённая сера
				itmq[1] = 100;
				nameitm[2] = "CompCraft_PowderMixture"; // Пороховая смесь сера
				itmq[2] = 100;
			break;
			case 10:
				nameitm[1] = "CompCraft_Grindstone"; // Оселок
				itmq[1] = 15+(sti(PChar.rank));
			break;
			case 11:
				nameitm[1] = "slave_01"; // Гарпун
				itmq[1] = 15+(sti(PChar.rank)*2);
			break;
			case 12:
				nameitm[1] = "CompCraft_Grindstone"; // Оселок
				itmq[1] = 11+(sti(PChar.rank));
				nameitm[2] = "slave_01"; // Гарпун
				itmq[2] = 12+(sti(PChar.rank));
			break;
			case 13:
				nameitm[1] = "CompCraft_Grindstone"; // Оселок
				itmq[1] = 11+(sti(PChar.rank)*2);
				nameitm[2] = "slave_01"; // Гарпун
				itmq[2] = 12+(sti(PChar.rank)*2);
			break;
		}
		if (itmq[0] > 0) itmq[0] += makeint(value/50);
		if (itmq[1] > 0) itmq[1] += makeint(value/50);
		if (itmq[2] > 0) itmq[2] += makeint(value/50);
		if (itmq[3] > 0) itmq[3] += makeint(value/50);
		if (itmq[4] > 0) itmq[4] += makeint(value/50);
		if (itmq[5] > 0) itmq[5] += makeint(value/50);
		if (itmq[6] > 0) itmq[6] += makeint(value/50);
		if (itmq[7] > 0) itmq[7] += makeint(value/50);
		if (itmq[8] > 0) itmq[8] += makeint(value/50);
		if (itmq[9] > 0) itmq[9] += makeint(value/50);
		if (itmq[10] > 0) itmq[10] += makeint(value/50);

		if (value < 333) {nameitm[11] = "CompCraft_HealthTobacco"; itmq[11] = 1;}
		if (value >= 333 && value < 666) {nameitm[12] = "CompCraft_EnergyTobacco"; itmq[12] = 1;}
		if (value >= 666) {nameitm[13] = "CompCraft_StrengthTobacco"; itmq[13] = 1;}
		if (value >= 700) {nameitm[14] = "CompCraft_Scissors"; itmq[14] = 1;}
	}

	if(sChestID == "Chest_ammo") // Снаряжение
	{
		switch (makeint(value/59)) // от 0 до 14
		{
			case 0:
				nameitm[1] = "bullet"; // Свинцовые пули
				itmq[1] = 37+(sti(PChar.rank)*2);
				nameitm[2] = "gunpowder"; // Гранулированный порох
				itmq[2] = 35+(sti(PChar.rank)*2);
				nameitm[4] = "grapeshot"; // Картечь
				itmq[4] = 33+(sti(PChar.rank)*2);
				nameitm[5] = "cartridge"; // Бумажный патрон
				itmq[5] = 31+(sti(PChar.rank)*2);
			break;
			case 1:
				nameitm[1] = "bullet"; // Свинцовые пули
				itmq[1] = 52+(sti(PChar.rank)*2);
				nameitm[2] = "gunpowder"; // Гранулированный порох
				itmq[2] = 55+(sti(PChar.rank)*2);
			break;
			case 2:
				nameitm[1] = "bullet"; // Свинцовые пули
				itmq[1] = 154;
				nameitm[2] = "gunpowder"; // Гранулированный порох
				itmq[2] = 151;
			break;
			case 3:
				nameitm[1] = "grapeshot"; // Картечь
				itmq[1] = 53;
				nameitm[2] = "cartridge"; // Бумажный патрон
				itmq[2] = 31;
			break;
			case 4:
				nameitm[1] = "grapeshot"; // Картечь
				itmq[1] = 155;
				nameitm[2] = "cartridge"; // Бумажный патрон
				itmq[2] = 101;
			break;
			case 5:
				nameitm[1] = "powder_pellet"; // Петарда
				itmq[1] = 10+(sti(PChar.rank)*2);
				nameitm[2] = "grenade"; // Граната
				itmq[2] = 5+(sti(PChar.rank)*2);
			break;
			case 6:
				nameitm[1] = "powder_pellet"; // Петарда
				itmq[1] = 32;
				nameitm[2] = "grenade"; // Граната
				itmq[2] = 15;
			break;
			case 7:
				nameitm[1] = "powder_pellet"; // Петарда
				itmq[1] = 50+(sti(PChar.rank));
			break;
			case 8:
				nameitm[2] = "grenade"; // Граната
				itmq[2] = 32+(sti(PChar.rank));
			break;
			case 9:
				nameitm[1] = "harpoon"; // Стрела
				itmq[1] = 16+(sti(PChar.rank)*2);
			break;
			case 10:
				nameitm[1] = "harpoon"; // Стрела
				itmq[1] = 26;
			break;
			case 11:
				nameitm[1] = "GunEchin"; // Ежовый заряд
				itmq[1] = 15+(sti(PChar.rank)*2);
			break;
			case 12:
				nameitm[1] = "GunEchin"; // Ежовый заряд
				itmq[1] = 28;
			break;
			case 13:
				nameitm[1] = "bullet"; // Свинцовые пули
				itmq[1] = 52+(sti(PChar.rank));
				nameitm[2] = "gunpowder"; // Гранулированный порох
				itmq[2] = 54+(sti(PChar.rank));
				nameitm[3] = "grapeshot"; // Картечь
				itmq[3] = 31+(sti(PChar.rank));
				nameitm[4] = "cartridge"; // Бумажный патрон
				itmq[4] = 23+(sti(PChar.rank));
				nameitm[5] = "powder_pellet"; // Петарда
				itmq[5] = 11+(sti(PChar.rank));
				nameitm[6] = "grenade"; // Граната
				itmq[6] = 5+(sti(PChar.rank));
				nameitm[7] = "harpoon"; // Стрела
				itmq[7] = 5+(sti(PChar.rank));
				nameitm[8] = "GunEchin"; // Ежовый заряд
				itmq[8] = 5+(sti(PChar.rank));
			break;
			case 14:
				nameitm[1] = "potion1"; // Лечебное зелье
				itmq[1] = 31+(sti(PChar.rank));
				nameitm[2] = "potion2"; // Эликсир
				itmq[2] = 15+(sti(PChar.rank));
				nameitm[3] = "potionrum"; // Бутылка рома
				itmq[3] = 23+(sti(PChar.rank));
			break;
			case 15:
				nameitm[1] = "potion1"; // Лечебное зелье
				itmq[1] = 72+(sti(PChar.rank));
			break;
			case 16:
				nameitm[1] = "potion2"; // Эликсир
				itmq[1] = 31+(sti(PChar.rank));
			break;
			case 17:
				nameitm[1] = "potionrum"; // Бутылка рома
				itmq[1] = 43+(sti(PChar.rank));
			break;
			case 18:
				nameitm[3] = "potionrum"; // Бутылка рома
				itmq[3] = 43+(sti(PChar.rank));
			break;
		}
		if (itmq[0] > 0) itmq[0] += makeint(value/50);
		if (itmq[1] > 0) itmq[1] += makeint(value/50);
		if (itmq[2] > 0) itmq[2] += makeint(value/50);
		if (itmq[3] > 0) itmq[3] += makeint(value/50);
		if (itmq[4] > 0) itmq[4] += makeint(value/50);
		if (itmq[5] > 0) itmq[5] += makeint(value/50);
		if (itmq[6] > 0) itmq[6] += makeint(value/50);
		if (itmq[7] > 0) itmq[7] += makeint(value/50);
		if (itmq[8] > 0) itmq[8] += makeint(value/50);
		if (itmq[9] > 0) itmq[9] += makeint(value/50);
		if (value >= 800) {nameitm[9] = "GunCap_colt"; itmq[9] = 5;}
	}

	if(sChestID == "Chest_quest") // Странный
	{
		switch (makeint(value/62))
		{
			case 0:
				nameitm[0] = "indian4"; // Глиняный кувшин
				itmq[0] = 1;
				nameitm[1] = "indian8"; // Серебряный кубок
				itmq[1] = 1;
				nameitm[2] = "indian9"; // Алебастровый сосуд
				itmq[2] = 1;
				nameitm[3] = "indian13"; // Древняя курительница
				itmq[3] = 1;
				nameitm[4] = "indian16"; // Раскрашенный сосуд
				itmq[4] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 9000*(sti(PChar.rank)/2);
			break;
			case 1:
				nameitm[0] = "indian3"; // Нефритовая маска
				itmq[0] = 1;
				nameitm[1] = "indian5"; // Двойная маска
				itmq[1] = 1;
				nameitm[2] = "indian10"; // Оберег Эхекатля
				itmq[2] = 1;
				nameitm[3] = "indian2"; // Пугающая фигурка
				itmq[3] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 10000*(sti(PChar.rank)/2);
			break;
			case 2:
				nameitm[0] = "indian6"; // Амулет Шиукоатля
				itmq[0] = 1;
				nameitm[1] = "indian7"; // Идол Великой Матери
				itmq[1] = 1;
				nameitm[2] = "indian12"; // Кубок-тотем Тепейоллотля
				itmq[2] = 1;
				nameitm[3] = "indian14"; // Чаша Ололиуки
				itmq[3] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 11000*(sti(PChar.rank)/2);
			break;
			case 3:
				nameitm[0] = "indian18"; // Идол Атлауа
				itmq[0] = 1;
				nameitm[1] = "indian19"; // Статуэтка Тлалока
				itmq[1] = 1;
				nameitm[2] = "indian21"; // Церемониальный сосуд
				itmq[2] = 1;
				nameitm[3] = "indian22"; // Голова воина племени майя
				itmq[3] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 12000*(sti(PChar.rank)/2);
			break;
			case 4:
				nameitm[0] = "indian15"; // Базальтовая голова
				itmq[0] = 1;
				nameitm[1] = "indian17"; // Тельная ладанка
				itmq[1] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 13000*(sti(PChar.rank)/2);
			break;
			case 5:
				nameitm[0] = "indian1"; // Оберег Тлальчитонатиу
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 27000*(sti(PChar.rank)/2);
			break;
			case 6:
				nameitm[0] = "indian2"; // Пугающая фигурка
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 36000*(sti(PChar.rank)/2);
			break;
			case 7:
				nameitm[0] = "indian3"; // Нефритовая маска
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 25000*(sti(PChar.rank)/2);
			break;
			case 8:
				nameitm[0] = "indian5"; // Двойная маска
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 24000*(sti(PChar.rank)/2);
			break;
			case 9:
				nameitm[0] = "indian6"; // Амулет Шиукоатля
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 14000*(sti(PChar.rank)/2);
			break;
			case 10:
				nameitm[0] = "indian7"; // Идол Великой Матери
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 15000*(sti(PChar.rank)/2);
			break;
			case 11:
				nameitm[0] = "indian12"; // Кубок-тотем Тепейоллотля
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 16000*(sti(PChar.rank)/2);
			break;
			case 12:
				nameitm[0] = "indian14"; // Чаша Ололиуки
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 17000*(sti(PChar.rank)/2);
			break;
			case 13:
				nameitm[0] = "indian18"; // Идол Атлауа
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 18000*(sti(PChar.rank)/2);
			break;
			case 14:
				nameitm[0] = "indian19"; // Статуэтка Тлалока
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 19000*(sti(PChar.rank)/2);
			break;
			case 15:
				nameitm[0] = "indian21"; // Церемониальный сосуд
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 23000*(sti(PChar.rank)/2);
			break;
			case 16:
				nameitm[0] = "indian22"; // Голова воина племени майя
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 22000*(sti(PChar.rank)/2);
			break;
			case 17:
				nameitm[0] = "indian22"; // Голова воина племени майя
				itmq[0] = 1;
				nameitm[30] = "gold"; // Золото
				itmq[30] = 21000*(sti(PChar.rank)/2);
			break;
		}
		itmq[30] += makeint(value/3);

        if (value >= 1 && value < 80) {nameitm[15] = "BackPack2"; itmq[15] = 1;}     		// Обычный подсумок
        if (value_alt >= 80 && value_alt < 160) {nameitm[15] = "BackPack3"; itmq[15] = 1;}  // Хороший подсумок
        if (value2 >= 160 && value2 < 240) {nameitm[15] = "BackPack4"; itmq[15] = 1;} 		// Отличный подсумок

        if (value3 >= 240 && value3 < 320) {nameitm[15] = "cirass2"; itmq[15] = 1;}     	// Рейтарский панцирь
        if (value >= 320 && value < 400) {nameitm[15] = "cirass3"; itmq[15] = 1;}     		// Гвардейская кираса
        if (value_alt >= 400 && value_alt < 480) {nameitm[15] = "cirass4"; itmq[15] = 1;}   // Офицерская кираса

        if (value2 >= 480 && value2 < 560) {nameitm[15] = "Totem_1"; itmq[15] = 1;}     	// Тотем Шочикецаль
        if (value3 >= 560 && value3 < 640) {nameitm[15] = "Totem_2"; itmq[15] = 1;}     	// Тотем Миктлантекутли
        if (value >= 640 && value < 720) {nameitm[15] = "Totem_3"; itmq[15] = 1;}     		// Тотем Кецалькоатля
        if (value_alt >= 720 && value_alt < 800) {nameitm[15] = "Totem_4"; itmq[15] = 1;}   // Тотем Мишкоатля
        if (value2 >= 800 && value2 < 880) {nameitm[15] = "Totem_7"; itmq[15] = 1;}     	// Тотем Уицилопочтли
        if (value3 >= 880 && value3 < 999) {nameitm[15] = "Totem_8"; itmq[15] = 1;}     	// Тотем Тлалока

        if (value >= 1 && value < 40) {nameitm[16] = "blade19"; itmq[16] = 1;}     			// Маринера ЛО
        if (value_alt >= 40 && value_alt < 80) {nameitm[16] = "blade22"; itmq[16] = 1;}     // Испанская рапира ЛО
        if (value2 >= 80 && value2 < 120) {nameitm[16] = "blade36"; itmq[16] = 1;}     		// Саксенфедер ЛО
        if (value3 >= 120 && value3 < 160) {nameitm[16] = "blade05"; itmq[16] = 1;}     	// Бретерская рапира ЛО

        if (value >= 160 && value < 200) {nameitm[16] = "blade31"; itmq[16] = 1;}     		// Шамшир СО
        if (value_alt >= 200 && value_alt < 240) {nameitm[16] = "blade34"; itmq[16] = 1;}   // Скаллоп СО
        if (value2 >= 240 && value2 < 280) {nameitm[16] = "blade014"; itmq[16] = 1;}    	// Кавалерийская сабля СО
        if (value3 >= 280 && value3 < 320) {nameitm[16] = "blade40"; itmq[16] = 1;}    		// Мадонна СО
        if (value >= 320 && value < 360) {nameitm[16] = "blade46"; itmq[16] = 1;}    		// Кракемарт СО

        if (value_alt >= 360 && value_alt < 400) {nameitm[16] = "blade15"; itmq[16] = 1;}   // Сторта ТО
        if (value2 >= 400 && value2 < 440) {nameitm[16] = "blade21"; itmq[16] = 1;}    		// Бастард ТО
        if (value3 >= 440 && value3 < 480) {nameitm[16] = "blade29"; itmq[16] = 1;}     	// Цвайхандер ТО
        if (value >= 480 && value < 520) {nameitm[16] = "blade01"; itmq[16] = 1;}     		// Палаш ТО
        if (value_alt >= 520 && value_alt < 560) {nameitm[16] = "topor2"; itmq[16] = 1;}    // Рейтарский чекан ТО

        if (value2 >= 560 && value2 < 600) {nameitm[16] = "blade23"; itmq[16] = 1;}       	// Бретта ЛО
        if (value3 >= 600 && value3 < 640) {nameitm[16] = "blade30"; itmq[16] = 1;}     	// Нобиль ЛО
        if (value >= 640 && value < 680) {nameitm[16] = "blade26"; itmq[16] = 1;}     		// Паппенхаймер ЛО

        if (value_alt >= 680 && value_alt < 720) {nameitm[16] = "blade47"; itmq[16] = 1;}   // Персидский меч СО
        if (value2 >= 720 && value2 < 760) {nameitm[16] = "blade24"; itmq[16] = 1;}     	// Офицерский клеванг СО
        if (value3 >= 760 && value3 < 800) {nameitm[16] = "blade25"; itmq[16] = 1;}     	// Карабела СО

        if (value >= 800 && value < 840) {nameitm[16] = "blade13"; itmq[16] = 1;}     		// Скьявона ТО
        if (value_alt >= 840 && value_alt < 880) {nameitm[16] = "blade20"; itmq[16] = 1;}   // Клейбэг ТО
        if (value2 >= 880 && value2 < 920) {nameitm[16] = "blade33"; itmq[16] = 1;}     	// Валлонская шпага ТО
        if (value3 >= 920 && value3 < 999) {nameitm[16] = "blade28"; itmq[16] = 1;}     	// Танат ТО

		if (value >= 1 && value < 150) {nameitm[17] = "pistol_grapebok"; 		itmq[17] = 1;} // Картечница
		if (value_alt >= 200 && value_alt < 350) {nameitm[17] = "pistol6"; 		itmq[17] = 1;} // Бок-пистолет
		if (value2 >= 400 && value2 < 550) {nameitm[17] = "pistol11"; 			itmq[17] = 1;} // Дуэльный пистолет
		if (value3 >= 600 && value3 < 750) {nameitm[17] = "pistol10"; 			itmq[17] = 1;} // Трёхствольный пистолет
		if (value >= 800 && value < 900) {nameitm[17] = "pistol5"; 				itmq[17] = 1;} // Бретерский пистолет
		if (value_alt >= 900 && value_alt < 950) {nameitm[17] = "mushket"; 		itmq[17] = 1;} // Фитильный мушкет
		if (value >= 950 && value < 999) {nameitm[17] = "mushket_drob"; 		itmq[17] = 1;} // Аркебуза

		if (value >= 10 && value < 30) {nameitm[18] = "mushket_spanish"; 			itmq[18] = 1;} // Испанский мушкетон
		if (value_alt >= 150 && value_alt < 170) {nameitm[18] = "mushket_english"; 	itmq[18] = 1;} // Английский мушкетон
		if (value >= 600 && value < 620) {nameitm[18] = "mushket_france"; 			itmq[18] = 1;} // Французcкий мушкетон
		if (value_alt >= 900 && value_alt < 920) {nameitm[18] = "mushket_holland"; 	itmq[18] = 1;} // Голландский мушкетон
	}

	int iBoxLoc = -1;
	int iBoxMoney = 0;
	string sBoxItem = "";
	if (sBoxLocation != "") iBoxLoc = FindLocation(sBoxLocation);

	// LEO: Вот выскажусь. Как же я заебался впиливать эту ссаную херню с сундуками. А вот всякие хейтеры потом будут подпёздывать, что мы тип ток готовое пиздим. Сукины вы дети. Если бы вы знали, сколько мы делаем уже эти ёбанные сундуки (и это ток сундуки), чтобы они были ахуенно разнообразны и избавив вас от сейвскама, чтобы вы не могли рандомить их.
	// LEO: Вот реально, я рот эбал эту рутину. Чот в 2013 году, когда я начинал моделить кораблики в иксах и делал это каждый день и всирал кучу времени, мне тогда было как то похуй. Сейчас чот мне как то немного жалко потраченного времени, конкретно на такую вот рутину. Но сук, это же и для себя делается в первую очередь. Один раз сделать ЗБС и потом играть-кайфовать. Т.ч. я всё ещё в деле и едем дальше :)
	// LEO: Ну и да, без Gregg я бы тут вообще лицо сломал фейспалмом с этой логики и кода в целом. Но оно всё работает и работает как швейцарские часы.
	for (i = 0; i < 35; i++)
	{
		if (itmq[i] == 0) continue;
		if (nameitm[i] == "") continue;

		if (iBoxLoc >= 0)
		{
			// в сундук: золото деньгами, клинки прогоняем через генератор так же, как при выдаче
			// в руки - иначе в сундук лёг бы шаблон вместо конкретного оружия
			if (nameitm[i] == "gold")
			{
				iBoxMoney = iBoxMoney + itmq[i];
				continue;
			}
			sBoxItem = nameitm[i];
			if (HasSubstr(sBoxItem, "blade")) sBoxItem = GetGeneratedItem(sBoxItem);
			Locations[iBoxLoc].(sBoxLocator).items.(sBoxItem) = itmq[i];
			continue;
		}

		if (nameitm[i] == "gold") AddMoneyToCharacter(pchar, itmq[i]);
		else
		{
			if (HasSubstr(nameitm[i], "blade")) TakeNItems(pchar, GetGeneratedItem(nameitm[i]), itmq[i]);
			else TakeNItems(pchar, nameitm[i], itmq[i]);
		}

		if (i == 0) nameitm[i] = "- " + GetConvertStr("itmname_"+nameitm[i], "ItemsDescribe.txt") + GetConvertStrWithReplace("Variable_lockpicking_2", "Interface.txt", "#space#", " ") + itmq[i];
		else nameitm[i] = NewStr() + "- " + GetConvertStr("itmname_"+nameitm[i], "ItemsDescribe.txt") + GetConvertStrWithReplace("Variable_lockpicking_3", "Interface.txt", "#space#", " ") + itmq[i];
	}

	if (iBoxLoc >= 0)
	{
		Locations[iBoxLoc].(sBoxLocator).money = iBoxMoney;
		return;                               // сундук наполнен, показывать игроку нечего
	}

	EngineLayersOffOn(true);
	CreateTooltip("#" + GetConvertStrWithReplace("Variable_lockpicking_4", "Interface.txt", "#space#", " "), GetConvertStrWithReplace("Variable_lockpicking_5", "Interface.txt", "#space#", " "), argb(255,255,255,255), "", argb(255,255,192,192), nameitm[0]+nameitm[1]+nameitm[2]+nameitm[3]+nameitm[4]+nameitm[5]+nameitm[6]+nameitm[7]+nameitm[8]+nameitm[9]+nameitm[10]+nameitm[11]+nameitm[12]+nameitm[13]+nameitm[14]+nameitm[15]+nameitm[16]+nameitm[17]+nameitm[18]+nameitm[19]+nameitm[20]+nameitm[21]+nameitm[22]+nameitm[23]+nameitm[24]+nameitm[25]+nameitm[26]+nameitm[27]+nameitm[28]+nameitm[29]+nameitm[30], argb(255,255,255,255), "", argb(255,255,255,255), "none", "none", "none", 64, 64);
}

bool SpawnItem(ref _chr, ref _id, bool isAbordageBox, float luck)
{
	int i = rand(RANDITEMS_QUANTITY-1);
	int chrRank = sti(_chr.rank);
	int itemRank = 0;
	aref randItem;
	if (Items_FindItem(RandItems[i].id, &randItem) == -1)
	{
		return false;
	}
	if (CheckAttribute(randItem, "minLevel"))
	{
		itemRank = sti(randItem.minLevel);
	}
	if (itemRank > chrRank)
	{
		return false;
	}

	float itemProb = 0.05;
	if (CheckAttribute(randItem, "rare"))
	{
		itemProb = stf(randItem.rare);
	}
	if (isAbordageBox)
	{
		itemProb = itemProb * (1.5 + luck/20.0);
	}
	if (itemProb > 1.0)
	{
		itemProb = 1.0;
	}
	int rndNumber = rand(1000);
	if (rndNumber > (itemProb*1000))
	{
		return false;
	}
	// отличные клинки в сундуках больше не генерятся <-- ugeen
	if(CheckAttribute(randItem,"quality") && randItem.quality == "excellent")
	{
		return false;
	}

	_id = GetGeneratedItem(randItem.id);
	return true;
}

object g_TmpModelVariable; // код от к3, в скриптах нет вообще, есть проверка в ядре

void spawnToughSkeleton(aref _location)
{
	if(CheckAttribute(_location, "ToughSkeletonTimer"))
	{
		if (GetNpcQuestPastDayParam(_location, "ToughSkeletonTimer") <= sti(_location.ToughSkeletonTimer)) return;//в этой локации запрещена генерация проклятого кэпа
			else DeleteAttribute(_location, "ToughSkeletonTimer");
	}
	if (drand(100) < 50 && CheckAttribute(_location, "locators.monsters") && !bMonstersGen && _location.type == "cave" && !CheckAttribute(pchar, "cursed.waitingSkull") && pchar.rank >= 3 && pchar.sex != "skeleton")
	{
		if (!CheckAttribute(pchar, "cursed.quest") || GetQuestPastDayParam("pchar.questTemp.Cursed") >= 90)
		{
			pchar.cursed.quest = true;
			ref sld;

			if (MOD_SKILL_ENEMY_RATE == 3 && bHardAnimations) sld = GetCharacter(NPC_GenerateCharacter("CursedSkeleton", "PGG_Skeletcap_0", "skeleton", "skeleton_fast", sti(pchar.rank)+20, PIRATE, 1, true));
			else sld = GetCharacter(NPC_GenerateCharacter("CursedSkeleton", "PGG_Skeletcap_0", "skeleton", "skeleton", sti(pchar.rank)+20, PIRATE, 1, true));
			FantomMakeCoolFighter(sld, sti(pchar.rank)+20, 100, 100, LinkRandPhrase(RandPhraseSimple("blade23","blade25"), RandPhraseSimple("blade30","blade26"), RandPhraseSimple("blade24","blade13")), RandPhraseSimple("pistol6", "pistol3"), MOD_SKILL_ENEMY_RATE*3*4);
			DeleteAttribute(sld, "SuperShooter");
			int hitpoints = rand(sti(pchar.rank)*15)+1000;
			LAi_SetHP(sld, hitpoints, hitpoints);
			sld.SaveItemsForDead = true;
			sld.cirassId = Items_FindItemIdx("cirass5");  // предмета нет, но влияение есть
			sld.name = GetConvertStrWithReplace("Variable_itemLogic_1", "Names.txt", "#space#", " ");
			sld.lastname = "";
			if (bHardBoss) sld.AlwaysReload = true;//перезарядка независимо от Дозарядки

			pchar.quest.cursed_appearance.name  = sld.name;
			pchar.quest.cursed_appearance.lastname = sld.lastname;
			AddBonusEnergyToCharacter(sld, 200);

			sld.quest.meeting = "0";
			sld.Dialog.CurrentNode = "First time";
			sld.dialog.filename = "Cursed_Skeleton.c";
			LAi_group_Delete("cursed");
			Group_FindOrCreateGroup("cursed");
			Group_AddCharacter("cursed", sld.id);
			LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
			LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER_OWN, LAI_GROUP_NEITRAL);

			LAi_SetActorTypeNoGroup(sld);
			LAi_SetCheckMinHP(sld, (LAi_GetCharacterHP(sld) - 1), false, "Battle_Hunters_Land");
			PlaceCharacter(sld, "monsters", "random_must_be_near");
			LAi_SetImmortal(sld, true);

			LAi_type_actor_Reset(sld);
			LAi_ActorDialog(sld, pchar, "", 20.0, 0);
			SaveCurrentQuestDateParam("pchar.questTemp.Cursed");
			// Gregg: было "monster" в единственном числе - такой группы локаторов нет НИ В ОДНОЙ модели,
			// есть только "monsters". Поиск возвращал пустую строку, адрес становился monster:: и эта
			// строка затирала корректный, выставленный PlaceCharacter двумя строками выше. При следующем
			// логине локации (сейв/лоад, перезаход) движок удалял персонажа:
			//   "Delete character <CursedSkeleton>, error teleportation ... on locator: monster::"
			// Плюс гвард на пустой локатор - если все точки заняты, лучше оставить адрес от PlaceCharacter.
			string sCursedLoc = LAi_FindNearestFreeLocator2Pchar("monsters");
			if (sCursedLoc != "") ChangeCharacterAddressGroup(sld, _location.id, "monsters", sCursedLoc);
			LAi_CharacterPlaySound(sld, "DeadmansGod");
		}

	}
}
