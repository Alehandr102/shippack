#define RELOAD_TIME_FADE_OUT	0.15	// Gregg: было 0.5; скорость fade-out механических транзишенов (НЕ 0.07 — моргает лоадскрин; подбор по ощущению)
#define RELOAD_TIME_FADE_IN		0.33	// Gregg: выход в геймплей из картинки загрузки, норма 0.5
#define CABIN_FADE_OUT			0.15	// Gregg: каюта / палуба шлюпки под картинку (ваниль 1.2.12); норма 1.0
#define SEAMAP_FADE_OUT			0.15	// Gregg: вход на морскую карту Sea_MapLoad/Land_MapLoad (ваниль 1.2.12); норма 0.7

object reload_fader;
object reload_locator_ref;
int reload_cur_island_index, reload_cur_location_index;
int reload_island_index, reload_location_index;
float reload_cur_time = -1.0, reload_time = -1.0;

object reload_xaddress;

bool LoadMainCharacterInFirstLocation(string location_id, string emerge_locator, string ship_location)
{
	int lindex;
	if(ship_location != "")
	{
		lindex = FindLocation(ship_location);
		if(lindex < 0)
		{
			Trace("Incorrect location id for location character's ship (location " + ship_location + " not found");
		}
	}
	if(lindex >= 0)
	{
		SetPlayerShipLocation(lindex);
	}
	lindex = FindLocation(location_id);
	if(lindex < 0)
	{
		Trace("Incorrect location id for first location (location " + location_id + " not found");
		return false;
	}
	aref reload_group;
	aref locator_ref;
	makearef(reload_group, Locations[lindex].reload);

	int num = GetAttributesNum(reload_group);
	if(num == 0)
	{
		Trace("Location " + location_id + " not have reload groups!");
		return false;
	}
	if(emerge_locator != "")
	{
		for(int i = 0; i < num; i++)
		{
            locator_ref = GetAttributeN(reload_group, i);
			if(emerge_locator == locator_ref.name) break;
		}
		if(i == num)
		{
			Trace("Emerge locator " + emerge_locator + " in location " + location_id + " not found!");
			locator_ref = GetAttributeN(reload_group, 0);
			emerge_locator = locator_ref.name;
			Trace("Character emerge on first locator (" + emerge_locator + ")");
		}
	}else{
		locator_ref = GetAttributeN(reload_group, 0);
		emerge_locator = locator_ref.name;
	}
	if (findsubstr(emerge_locator, "_back" , 0) != -1)
	{
        emerge_locator = strcut(emerge_locator, 0, strlen(emerge_locator)-6);
	}
	lockedReloadLocator = emerge_locator;
	ref mc = GetMainCharacter();
	mc.location = Locations[lindex].id;
	mc.location.group = "reload";
	mc.location.locator = emerge_locator;
	if(IsEntity(&chrAnimationKipper) == false) CreateEntity(&chrAnimationKipper, "CharacterAnimationKipper");
	ReloadProgressUpdate();
	LoadLocation(&Locations[lindex]);
	//Fader
	PostEvent("LoadSceneSound", 500);
	return true;
}

bool LoadMainCharacterInFirstLocationGroup(string location_id, string sGroup, string emerge_locator)
{
	int lindex;
	ReloadProgressStart();
	lindex = FindLocation(location_id);
	if(lindex < 0)
	{
		Trace("Incorrect location id for first location (location " + location_id + " not found");
		return false;
	}

	lockedReloadLocator = emerge_locator;
	ref mc = GetMainCharacter();
	mc.location         = location_id;
	mc.location.group   = sGroup;
	mc.location.locator = emerge_locator;
	if(IsEntity(&chrAnimationKipper) == false) CreateEntity(&chrAnimationKipper, "CharacterAnimationKipper");
	ReloadProgressUpdate();
	LoadLocation(&Locations[lindex]);
	PostEvent("LoadSceneSound", 500);
	ReloadProgressEnd();
	return true;
}

void SetPlayerShipLocation(int location_index)
{
	if(location_index < 0)
	{
		Trace("SetPlayerShipLocation - incorrect location index");
		return;
	}
	ref mc = GetMainCharacter();
	mc.location.from_sea = Locations[location_index].id;
}

int Reload(aref reload_group, string locator_name, string current_location)
{
	// Gregg: скриптовый гвард от задвоения фейдера снят — защита перенесена в движок
	// (Fader::Init реапит осиротевшие fade-out'ы isAutodelete==false при рождении нового
	// фейдера). Блокировать здесь по IsEntity(&reload_fader) нельзя: море/палуба→берег
	// (DoReloadFromSeaToLocation) легитимно зовут Reload() из EndFade ещё живого фейдера,
	// и гвард намертво чернил экран.
	SetTimeScale(1.0);
	if(CheckAttribute(&Environment,"oldtime"))
        reload_cur_time = Environment.oldtime;
    else
        reload_cur_time = GetTime();
	SetReloadNextTipsImage();
	dialogDisable = true;
	reload_xaddress.active = "false";
	if(locator_name == lockedReloadLocator)
	{
		lockedReloadLocator = "";
		return -1;
	}

	// LEO: из зала славы выходят не в локацию, а в титры и главное меню. Проверка идёт ПОСЛЕ
	//   блокировки локатора приземления: на выходной локатор игрока приводит телепорт изнутри
	//   зала, и без неё титры включались бы прямо в момент прибытия.
	if( FinalGame_CatchExit(current_location, locator_name) ) return 1;
	int num = GetAttributesNum(reload_group);
	if(num == 0) return 0;
	for(int i = 0; i < num; i++)
	{
        CopyAttributes(&reload_locator_ref, GetAttributeN(reload_group, i));
		if(!CheckAttribute(reload_locator_ref, "name")) { continue; }
		if(locator_name == reload_locator_ref.name) break;
	}
	if(i == num)
	{
		Trace("Can't reload...");
		Trace("Reload locator '" + locator_name + "' in current location not found!");
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "name") == 0)
	{
		Trace("Can't reload...");
		Trace("Reload: attribute 'name' in " + GetAttributeName(reload_locator_ref) + " not accessible");
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "go") == 0)
	{
		Trace("Can't reload...");
		Trace("Reload: attribute 'go' in " + GetAttributeName(reload_locator_ref) + " not accessible");
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "emerge") == 0)
	{
		Trace("Can't reload...");
		Trace("Reload: attribute 'emerge' in " + GetAttributeName(reload_locator_ref) + " not accessible");
		return 0;
	}
	reload_cur_island_index = -1;
	reload_cur_location_index = -1;
	if(current_location != "")
	{
		reload_cur_island_index = FindIsland(current_location);
		if(reload_cur_island_index < 0)
		{
			reload_cur_location_index = FindLocation(current_location);
			if(reload_cur_location_index < 0)
			{
				Trace("Can't unload currend location...");
				Trace("Current location not found: id = " + current_location);
			}
		}
	}
	reload_island_index = -1;
	reload_location_index = -1;
	reload_island_index = FindIsland(reload_locator_ref.go);
	if(reload_island_index < 0)
	{
		reload_location_index = FindLocation(reload_locator_ref.go);
		if(reload_location_index < 0)
		{
			Trace("Can't reload...");
			Trace("Location not found: id = " + reload_locator_ref.go);
			return 0;
		}
	}
	ref mc = GetMainCharacter();

	if((reload_cur_island_index < 0) && (reload_island_index >= 0))
	{
		if(reload_cur_location_index >= 0)
		{
			if(Locations[reload_cur_location_index].id != mc.location.from_sea)
			{
				Trace("Not reload to sea, no ships: cur:" + Locations[reload_cur_location_index].id + " from:" + mc.location.from_sea);
				return -1;
			}
		}
	}
	// LEO: автосейв ПЕРЕД выходом в море С СУШИ (море->суша ловит Sea_LandLoad, суша<->суша не сохраняем).
	//   Асинхронен, а ниже создаётся фейдер, при котором сейв уже не пишется - откладываем переход до evntSave.
	if (reload_cur_island_index < 0)
	{
		if (reload_island_index >= 0)
		{
			if (AutoSave_Start("Sea"))
			{
				SetEventHandler("evntSave", "Reload_AfterAutoSave", 0);
				return 1;
			}
		}
	}
	return Reload_Go();
}

// LEO: продолжение перехода после отработавшего автосейва
void Reload_AfterAutoSave()
{
	DelEventHandler("evntSave", "Reload_AfterAutoSave");
	SetEventHandler("frame", "Reload_Frame", 0); // кадр на дозапись слота (как Continue_Sea_AbordageLoadPre)
}

void Reload_Frame()
{
	DelEventHandler("frame", "Reload_Frame");
	Reload_Go();
}

// LEO: собственно запуск перехода (вынесено из Reload ради отложенного автосейва). Все reload_* - глобалки
//   (объявлены вверху файла); локальной была только mc - берём заново.
int Reload_Go()
{
	ref mc = GetMainCharacter();
	SetEventHandler("FaderEvent_StartFade", "ReloadStartFade", 0);
	SetEventHandler("FaderEvent_EndFade", "ReloadEndFade", 0);
	CreateEntity(&reload_fader, "fader");
	if(reload_island_index >= 0)
	{
		if(!CheckAttribute(mc, "todeck"))
		{
			pchar.loadscreen = "loading\jonny_load\port\port_" + rand(4) + ".dds";
		}
		else
		{
			pchar.loadscreen = "loading\cabine.tga";
		}
		SendMessage(&reload_fader, "ls", FADER_PICTURE0, pchar.loadscreen);
	}
	else
	{
		int loc_pict_index = reload_location_index;
		if(loc_pict_index < 0) loc_pict_index = reload_cur_location_index;
		if(loc_pict_index >= 0)
		{
			if(CheckAttribute(&Locations[loc_pict_index],"image"))
			{
				if (sGlobalTemp == "afterFDsink")
				{
				    //Boyer add #20170401-02
                    pchar.loadscreen = "loading\jonny_load\load\lifeboat.dds";
					sGlobalTemp = "";
				}
				else
				{
                    //Boyer add #20170401-02
                    pchar.loadscreen = Locations[loc_pict_index].image;
				}
				SendMessage(&reload_fader, "ls", FADER_PICTURE0, pchar.loadscreen);
			}
		}
	}

	bool IsMakeShot = true;
	float fadeOutTime = RELOAD_TIME_FADE_OUT;
	if((reload_cur_island_index < 0) && (reload_cur_location_index < 0))
	{
		fadeOutTime = 0.00001;
		IsMakeShot = false;
	}
	SendMessage(&reload_fader, "lfl", FADER_OUT, fadeOutTime, false);
	if(IsMakeShot == true) SendMessage(&reload_fader, "l", FADER_STARTFRAME);
	if(reload_cur_island_index < 0)
	{
		if(reload_island_index < 0)
		{
			lockedReloadLocator = reload_locator_ref.emerge;
		}
		else
		{
			lockedReloadLocator = "";
		}
	}
	else
	{
		if(reload_island_index < 0)
		{
			lockedReloadLocator = reload_locator_ref.emerge;
		}
		else
		{
			lockedReloadLocator = "";
		}
	}
	return 1;
}

void ReloadStartFade()
{
    //#20191123-01 Fix
    DialogExit();
	ApplayNewSkill(pchar, "", 0);
	if (CheckAttribute(pchar,"activelocator")) Item_OnExitLocator(loadedLocation, pchar.activelocator);
	reload_time = GetTime();
	if(reload_cur_location_index >= 0 && reload_location_index >=0)
	{
		if(bChangeScheme(reload_cur_location_index, reload_location_index))
		{
			PauseAllSounds();
            ResetSound(); // new
		}
	}
	else
	{
        PauseAllSounds();
		ResetSound(); // new
	}
	DelEventHandler("FaderEvent_StartFade", "ReloadStartFade");
	ref mc = GetMainCharacter();
	if(reload_cur_island_index < 0)
	{
		if(reload_island_index < 0)
		{
			if(reload_cur_location_index >= 0) UnloadLocation(&Locations[reload_cur_location_index]);
			return;
		}else{

			if(reload_cur_location_index >= 0)
			{
				UnloadLocation(&Locations[reload_cur_location_index]);
			}else mc.location.from_sea = "";
			DeleteClass(&chrAnimationKipper);
			return;
		}
	}else{
		if(reload_island_index < 0)
		{
			DeleteSeaEnvironment();
			mc.location.from_sea = reload_locator_ref.go;
			return;
		}else{
			DeleteSeaEnvironment();
			mc.location.from_sea = "";
			return;
		}
		Partition_SetValue("after");// Дележ добычи уход на сушу
	}
}

void ReloadEndFade()
{
	ResetExpLogs();//очистка лога опыта
	
    EmptyAllFantomCharacter(); // fix место тут!!!! а не выше, вот вам и баги по квестам, блин boal
    PGG_DailyUpdateReload();
    Siege_DailyUpdate();//homo осады 05/11/06
    BlackOrchid_Recharge(); // Gregg: перезарядка оберега из трилогии К:ПС + полное здоровье
    MysteriousArtefactCheckCan(); // Gregg: готовность к завязке "Чёрной Орхидеи"
    BlackOrchid_CloneCleanup(); // Gregg: выселить горожан из клон-города на время штурма
    if (CheckAttribute(pchar,"questTemp.CapBloodLine")) DetectBlades();
	dialogDisable = false;
	ReloadProgressStart();
	DelEventHandler("FaderEvent_EndFade", "ReloadEndFade");
	ReloadProgressUpdate();
	ref mc = GetMainCharacter();
	int iLoc;
	if(reload_cur_island_index < 0)
	{
		if(reload_island_index < 0)
		{
			ReloadToLocation(reload_location_index, reload_locator_ref);
		}
		else
		{
			ReloadToSea(reload_island_index, reload_locator_ref);
		}
	}
	else
	{
		if(reload_island_index < 0)
		{
			ReloadToLocation(reload_location_index, reload_locator_ref);
		}
		else
		{
			ReloadToSea(reload_island_index, reload_locator_ref);
		}
	}
	SendMessage(&reload_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);
	if(reload_cur_location_index >= 0 && reload_location_index >=0)
	{
		if(bChangeScheme(reload_cur_location_index, reload_location_index))
		{
			PostEvent("LoadSceneSound", 500);
		}
	}
	else
	{
		PostEvent("LoadSceneSound", 500);
	}
	if (startherotype == 8)
	{
		if (CheckAttribute(pchar,"equip.blade") && HasSubStr(pchar.equip.blade, "Lilarcor"))
		{
			bool no2nd = false;
			bool TalkMore = false;
			int CanMore = 0;
			if (CheckAttribute(pchar,"LilarcorKills"))
			{
				if (GetNpcQuestPastDayWOInit(pchar, "LilarcorTimer")>0) DeleteAttribute(pchar,"LilarcorTalksMore");
				else
				{
					if (sti(pchar.LilarcorKills) > 299 && sti(pchar.LilarcorKills) < 800) CanMore = 1;
					if (sti(pchar.LilarcorKills) > 800) CanMore = 2;
					if (CanMore > 0 && rand(3)==0)
					{
						if (!CheckAttribute(pchar,"LilarcorTalksMore"))
						{
							pchar.LilarcorTalksMore = CanMore;
						}
						else pchar.LilarcorTalksMore = sti(pchar.LilarcorTalksMore)-1;
						if (sti(pchar.LilarcorTalksMore)>-1) TalkMore = true;
					}
				}
			}
			if (LAi_group_IsActivePlayerAlarm())
			{
				if (GetNpcQuestPastDayWOInit(pchar, "LilarcorTimer")>0 || TalkMore)
				{
					PostEvent("Lilarcor_Talks",2000);
					SaveCurrentNpcQuestDateParam(pchar, "LilarcorTimer");
					no2nd = true;
				}
			}
			else
			{
				if (GetNpcQuestPastDayWOInit(pchar, "LilarcorTimer2")>2 && !no2nd)
				{
					PostEvent("Lilarcor_Talks2",2000);
					SaveCurrentNpcQuestDateParam(pchar, "LilarcorTimer2");
				}
			}
		}
	}
	if(CheckAttribute(&Environment,"oldtime")) 
    {
        Environment.oldtime = reload_time;
    }
	ReloadProgressUpdate();
	ReloadProgressEnd();
	SetPerspectiveSettings();
}

string FindEmergeLocator(ref rObject, string emerge_str)
{
	aref arReload;
	makearef(arReload,rObject.reload);
	int n = GetAttributesNum(arReload);
	for (int i=0;i<n;i++)
	{
		aref arLoc;
		arLoc = GetAttributeN(arReload,i);
		if (arLoc.name == emerge_str)
		{
			string loc_name = GetAttributeName(arLoc);
			return loc_name;
		}
	}
 	Trace("ERROR(no find emerge string): string FindEmergeLocator(ref rObject, string emerge_str) : " + emerge_str);
	return "";
}

object chrAnimationKipper;

int ReloadToLocation(int location_index, aref reload_data)
{
	ref mc = GetMainCharacter();
	// boal переход в зашлушки, ставим дверь обратно-->
	if (CheckAttribute(&Locations[location_index], "MustSetReloadBack"))
	{
		aref	arReload, arLocator;

		makearef(arReload, Locations[location_index].reload);
		int		iNumReload = GetAttributesNum(arReload);
		int i;
		if (!CheckAttribute(reload_data, "FromSea"))
		{   // переход суша-суша
			for (i=0; i<iNumReload; i++)
			{
				arLocator = GetAttributeN(arReload, i);
				if (arLocator.Name == reload_data.emerge)
				{
	                arLocator.go     = mc.location; // где был
	                arLocator.emerge = reload_data.name; // откуда шагнул
	                if (CheckAttribute(&Locations[reload_cur_location_index], "fastreload"))
	                {
	                    Locations[location_index].fastreload = Locations[reload_cur_location_index].fastreload;
	                }
	                else
	                {
	                    DeleteAttribute(&Locations[location_index], "fastreload");
	                }
	                if (CheckAttribute(&Locations[reload_cur_location_index], "id.label"))
	                {
	                	arLocator.label  = Locations[reload_cur_location_index].id.label; // как завать, куда вернуться
	                }
	                else
	                {
	                    arLocator.label  = "";
	                }
	                if (CheckAttribute(&Locations[reload_cur_location_index], "townsack"))
	                {
	                    Locations[location_index].townsack = Locations[reload_cur_location_index].townsack;
	                }
	                else
	                {
	                    DeleteAttribute(&Locations[location_index], "townsack");
	                }
	                if (CheckAttribute(&Locations[reload_cur_location_index], "islandId"))
	                {
	                    Locations[location_index].islandId = Locations[reload_cur_location_index].islandId;
	                }
	                else
	                {
	                    DeleteAttribute(&Locations[location_index], "islandId");
	                }
	                if (CheckAttribute(&Locations[reload_cur_location_index], "id.label") && locations[reload_cur_location_index].type != "town")
	                {
	                    Locations[location_index].id.label = Locations[reload_cur_location_index].id.label;
	                }
					break;
				}
			}
		}
		else
		{ // переход море-суша
			// идея как работает
			// в Islands_init.c на локаторах входа в бухту-заглушку приписочка  .FromSea = true;
			// на суше в Locations\init\ на локаторах выхода в море приписочка  .ToSea = true;
			// на самой локации .MustSetReloadBack = true;
		    for (i=0; i<iNumReload; i++)
			{
				arLocator = GetAttributeN(arReload, i);
				if (CheckAttribute(arLocator, "ToSea")) // признак, что локатор выхода в море
				{
	                arLocator.go     = mc.location; // где был
	                arLocator.emerge = reload_data.name; // откуда шагнул
                    // метка == море всегда
				}
			}
		}
	}
	// boal <--
	mc.location = Locations[location_index].id;
	mc.location.group = "reload";

	//blackthorn - фикс одной ошибки, что у мен¤ возникала при вызове релоада на старте линейки
	if (!CheckAttribute(Pchar, "questTemp.WhisperLine"))
	{
		mc.location.locator = reload_data.emerge;
	}
	if(reload_xaddress.active == "true")
	{
		mc.location.group = reload_xaddress.group;
		mc.location.locator = reload_xaddress.locator;
	}
	//сменить анимацию перед загрузкой локации, нужно для подводных храмов.
	if (CheckAttribute(&locations[location_index], "changeAnimation"))
	{
		if (locations[location_index].id.label == "TempleUnderwater")
		{
			SetIntegratedMusketDrawn(mc, false);
			mc.model.animation = MainChAnim + "_swim";
		}
		else
		{
			mc.model.animation = MainChAnim;
			if (CheckCharacterPerk(mc, "AgileMan")) mc.model.animation = MainChAnim + "_fast";
		}
	}
	if(IsEntity(&chrAnimationKipper) == false) CreateEntity(&chrAnimationKipper, "CharacterAnimationKipper");

    // eddy --> открыть/закрыть двери в заглушках городов.
    if (CheckNPCQuestDate(&Locations[location_index], "Common_date"))
    {
    	if (Locations[location_index].type == "town" && CheckAttribute(&locations[location_index], "houseEnc"))
    	{
            string  sName;
            aref	arDis, arRld;
    		makearef(arRld, Locations[location_index].reload);
    		int		Qty = GetAttributesNum(arRld);
            SetNPCQuestDate(&Locations[location_index], "Common_date");
    		for (int a=0; a<Qty; a++)
    		{
    			arDis = GetAttributeN(arRld, a);
				if (CheckAttribute(arDis,"go"))
				{
					sName = arDis.go;
					if (findsubstr(sName, "Common" , 0) != -1)
					{
						if (CheckAttribute(arDis, "canEnter"))
						{
							arDis.disable = false;
							DeleteAttribute(arDis, "canEnter"); //автосъем флага на открытую дверь
						}
						else arDis.disable = rand(1);
					}
				}
    		}
    	}
    }
	//меняем радиусы детекторов энкаунтеров
	Locations[location_index].locators_radius.encdetector = 8 + rand(10);
    // <-- eddy
	return LoadLocation(&Locations[location_index]);
}

int ReloadToLocationEx(int location_index, string sLocator)
{
	ref mc = GetMainCharacter();
	mc.location = Locations[location_index].id;
	mc.location.group = "reload";
	mc.location.locator = sLocator;

	if(IsEntity(&chrAnimationKipper) == false)
	{
		CreateEntity(&chrAnimationKipper, "CharacterAnimationKipper");
	}
	return LoadLocation(&Locations[location_index]);
}

int ReloadToSea(int island_index, aref reload_data)
{

	object Login;

	ref rPlayer = GetMainCharacter();
	rPlayer.lastFightMode = 0;
	
	if (HasSubStr(rPlayer.location,"town") || HasSubStr(rPlayer.location,"shore") || HasSubStr(rPlayer.location,"mayak")) rPlayer.Do180Turn = true;

	ref rIsland = GetIslandByIndex(island_index);

	rPlayer.location = rIsland.id;
	rPlayer.location.group = "reload";
	rPlayer.location.locator = reload_data.emerge;

	Login.Island = rIsland.id;

	Login.Encounters = "";

	string sLoc = FindEmergeLocator(rIsland,reload_data.emerge);

	Login.PlayerGroup.x = rIsland.reload.(sLoc).ships.l0.x;	// FIX-ME
	Login.PlayerGroup.z = rIsland.reload.(sLoc).ships.l0.z;
	Login.PlayerGroup.ay = rIsland.reload.(sLoc).ships.l0.ay;
	Login.FromCoast = true;

	Trace("Load from location to sea sLoc = " + sLoc + ", x = " + Login.PlayerGroup.x + ",z = " + Login.PlayerGroup.z);

	SeaLogin(Login);
	return 1;
}


bool TeleportCharacterFromCurLocationToLocation(string locatorExit, string group, string locator)
{
	reload_xaddress.group = group;
	reload_xaddress.locator = locator;
	aref reload_group;
	makearef(reload_group, loadedLocation.reload);
	if(Reload(reload_group, locatorExit, loadedLocation.id) == false) return false;
	reload_xaddress.active = "true";
	return true;
}

//boal -->
int BOAL_ReloadToLoc(aref reload_group, string locator_name)
{
	int num = GetAttributesNum(reload_group);
	if(num == 0) return 0;
	for(int i = 0; i < num; i++)
	{
    CopyAttributes(&reload_locator_ref, GetAttributeN(reload_group, i));
		if(locator_name == reload_locator_ref.name) break;
	}
	if(i == num)
	{
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "name") == 0)
	{
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "emerge") == 0)
	{
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "go") == 0)  // нужна только эта проверка
	{
		return 0;
	}
	Pchar.quest.SetNPCInShipDeck.win_condition.l1          = "location";
	Pchar.quest.SetNPCInShipDeck.win_condition.l1.location = "My_Deck";
	Pchar.quest.SetNPCInShipDeck.win_condition             = "SetNPCInShipDeck";

	Pchar.quest.SetQuestHoldBox.win_condition.l1          = "location";
	Pchar.quest.SetQuestHoldBox.win_condition.l1.location = "My_Deck";
	Pchar.quest.SetQuestHoldBox.win_condition             = "SetQuestHoldBox";

	Pchar.quest.SetNPCInShipCampus.win_condition.l1          = "location";
	Pchar.quest.SetNPCInShipCampus.win_condition.l1.location = "My_Campus";
	Pchar.quest.SetNPCInShipCampus.win_condition             = "SetNPCInShipDeck";

	Pchar.quest.SetNPCInShipCabin.win_condition.l1          = "location";
	Pchar.quest.SetNPCInShipCabin.win_condition.l1.location = Get_My_Cabin();
	Pchar.quest.SetNPCInShipCabin.win_condition             = "SetNPCInShipDeck";

	boarding_location  = FindLocation(reload_locator_ref.go); // че потом грохать

	Reload(reload_group, locator_name, Pchar.location);

	return 1;
}
// boal <--
