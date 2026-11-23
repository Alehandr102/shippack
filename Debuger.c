void ActiveINSERTControl()
{
	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	string totalInfo = "телепорт по тавернам";
	ref mc, ch;
	int n, idx;
	mc = GetMainCharacter();
    idx = GetCharIDXByParam("TavernMan", "location", mc.location);
    bool ok = true;
    while (ok)
    {
        for(n=0; n<MAX_CHARACTERS; n++)
    	{
    		makeref(ch,Characters[n]);
    		if (CheckAttribute(ch, "TavernMan"))
    		{
                if (ch.location == "none") continue; // фикс для новых, невидимых до поры островов
                if (n > idx)
                {
                    ok = false;
                    if (GetCityFrom_Sea(ch.City) != "")
                    {
						setCharacterShipLocation(mc, GetCityFrom_Sea(ch.City));
                        setWDMPointXZ(GetCityFrom_Sea(ch.City));
                    }
                    DoQuestReloadToLocation(ch.location,"reload","reload1", "");
                    Log_Info(ch.location);
                    break;
                }
    		}
        }
        idx = -1;
    }
	// <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
	// Статистика по читам
	Statistic_AddValue(PChar, "Cheats.F8", 1);
}

void ActiveF4Control()
{
    LAi_Fade("", "");
    for (int i = 1; i<=1; i++)
    {
		AddDataToCurrent(0, 0, 1);
	}
}

// Warship. Сюда повесил дамп аттрибутов - по Ф12 не нужно это
void ActiveF5Control()
{
	int characterIndex = LAi_FindNearestVisCharacter(PChar, 15);
	ref characterRef;

    if(characterIndex != -1)
    {
        characterRef = GetCharacter(characterIndex);

		trace("==================== " + characterRef.ID + " ====================");
		DumpAttributes(characterRef);
		trace("========================================");
	}

	Log_Info(GetConvertStrWithReplace("Variable_Debuger_1", "Logs.txt", "#space#", " "));
}

void ActiveF7Control()
{
	// Warship. Убить боижайшего персонажа
	int characterIndex = LAi_FindNearestVisCharacter(PChar, 25);
	ref characterRef;

    if(characterIndex != -1)
	{
		characterRef = &Characters[characterIndex];

		if(!LAi_IsImmortal(characterRef)) // Квестовые
		{
			LAi_KillCharacter(characterRef);
			Log_Info(GetConvertStrWithReplace("Variable_Debuger_1.1", "Logs.txt", "#space#", " ") + characterRef.Id);
		}
		else
		{
			Log_Info(GetConvertStrWithReplace("Variable_Debuger_1.2", "Logs.txt", "#space#", " ") + characterRef.Id);
		}
	}
}

void ActiveF10Control()
{
	Log_Info("Test F10 button");
	LaunchPsHeroScreen();
    trace("===================RUMOURS=======================");
    for(int i = 0; i < MAX_RUMOURS; i++)
	{
        DumpAttributes(&Rumour[i]);
    	trace("===================");
	}
	Log_Info(GetStrSmallRegister(GetRussianNumberString(rand(999) - rand(999))));
}

///  статы персонажа в близи
void ActiveF12Control()
{
    ref mainCh = GetMainCharacter();

    //Найти ближайшего видимого персонажа в заданном радиусе
    int res = LAi_FindNearestVisCharacter(mainCh, 15);
    if (res != -1)
    {
        ref Itm;
        ref findCh = GetCharacter(res);
        
        string info1 = " ("+GetConvertStr(GetSpeciality(findCh), "RPGDescribe.txt")+")";
        string info2 = "";
        string info3 = "";
        string info4 = "";
		
        res = findCh.chr_ai.hp;
        if(CheckCharacterPerk(pchar, "SwordplayProfessional") || CheckCharacterPerk(pchar, "Fencer") || CheckCharacterPerk(pchar, "Grunt")) 
        {
            if (CheckAttribute(findCh, "equip.blade"))
            {
                Itm = ItemsFromID(findCh.equip.blade);
			    info3 = "ХО - "+GetConvertStr(Itm.name, "ItemsDescribe.txt");
                if(CheckCharacterPerk(findCh, "SwordplayProfessional")) {info2 = "Профессиональный фехтовальщик";}
            }
        }
        if(CheckCharacterPerk(pchar, "GunProfessional") || CheckCharacterPerk(pchar, "Buccaneer"))
        {
            if (CheckAttribute(findCh, "equip.gun"))
            {
                Itm = ItemsFromID(findCh.equip.gun);
                info3 += ", огнестрел - "+GetConvertStr(Itm.name, "ItemsDescribe.txt");
            }
        }
        if (CheckCharacterPerk(pchar, "Trader")) info4 = "Денег: "+findCh.money;
        Log_SetStringToLog("" + GetFullName(findCh) + info1 +
                           " "+XI_ConvertString("Rank")+" " + findCh.rank + " "+XI_ConvertString("Health")+" "+res + "/" + LAi_GetCharacterMaxHP(findCh));
        if (info2 != "") Log_SetStringToLog(info2);
        if (info3 != "") Log_SetStringToLog(info3);
        if (info4 != "") Log_SetStringToLog(info4);
        if (bBettaTestMode)
        {
            Log_SetStringToLog("Id= " + findCh.id);
            Log_SetStringToLog("Idx= " + findCh.index);
            if(CheckAttribute(findCh, "LifeDay"))
		Log_Info("LifeDay - " + findCh.LifeDay);

	    if(CheckAttribute(findCh, "City"))
	    	Log_Info("City - " + findCh.City);
            if (CheckAttribute(findCh, "equip.blade"))
            {
                Log_SetStringToLog("Blade= " + findCh.equip.blade);
            }
            if (CheckAttribute(findCh, "equip.pistol"))
            {
                Log_SetStringToLog("Pistol= " + findCh.equip.pistol);
            }
            Log_SetStringToLog("model= " + findCh.model);
            Log_SetStringToLog("face= " + findCh.faceId);
            Log_SetStringToLog("chr_ai.type= " + findCh.chr_ai.type);
            Log_SetStringToLog("Group= "+findCh.location.group + " locator= " + findCh.location.locator);
            if (CheckAttribute(findCh, "cirassId"))
            {
                Log_SetStringToLog("cirassId= " + sti(findCh.cirassId));
            }
            else
            {
                Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_2", "Logs.txt", "#space#", " "));
            }
            if (CheckAttribute(findCh, "chr_ai.FencingType"))
            {
                Log_SetStringToLog("chr_ai.FencingType= " + findCh.chr_ai.FencingType));
            }
            else
            {
                Log_SetStringToLog("Нет FencingType - error");
            }
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_3", "Logs.txt", "#space#", " ") + findCh.nation);
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_4", "Logs.txt", "#space#", " ") + findCh.sex);
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_5", "Logs.txt", "#space#", " ") + findCh.chr_ai.group);
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_6", "Logs.txt", "#space#", " ") + findCh.chr_ai.tmpl);
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_Debuger_7", "Logs.txt", "#space#", " ") + findCh.chr_ai.tmpl.state);
			dumpattributes(findCh);

            mainCh.SystemInfo.OnlyShowCharacter = true;
            mainCh.SystemInfo.CheckNpcIdx = sti(findCh.index); // LEO: вкладки чеккера (Корабль/Предметы) роутятся на этого NPC; сбрасывается в IDoExit при полном выходе
			LaunchCharacter(findCh);
        }
        else
        {
            if (MOD_BETTATESTMODE == "Test")
	        {
	            Log_SetStringToLog("Id= " + findCh.id);
	        }
        }
    }
}
