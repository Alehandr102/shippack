string state;
string tmp;

#define GLOBAL_MAP_RPC_REFRESH_TIMEOUT 2000

void MainMenuUpdateRPC() {
    if (!CheckRPC()) return;
    
    FinalizeRPC(XI_ConvertString("MainMenuRPC"), XI_ConvertString("MainMenuRPC"), "general");
}

void LocUpdateRPC(ref loc) {
    if (!CheckRPC()) return;

	if (loc.id == "SelectMainCharacter_Cabine") {
        FinalizeRPC(XI_ConvertString("NewGameRPC"), XI_ConvertString("NewGameRPC"), "general");
        return;
	}

    state = XI_ConvertString("UnknownLocationRPC");

    int nFile = LanguageOpenFile("LocLables.txt");
    if (loc.id.label != "") {
        tmp = LanguageConvertString(nFile, loc.id.label);
        if (tmp != "") {
            state = tmp;
        }
    }
    if (CheckAttribute(loc, "townsack") && loc.townsack != "") {
        tmp = LanguageConvertString(nFile, loc.townsack);
        if (tmp != "") {
            state += ", " + tmp;
        }
    } else if (CheckAttribute(loc, "islandid") && loc.islandid != "") {
        // LEO: было XI_ConvertString(nFile, ...) - у неё ОДИН аргумент, отсюда 118 "stack error" в логе.
        // Название острова лежит в том же LocLables.txt, что и ветка выше, значит и функция та же.
        tmp = LanguageConvertString(nFile, loc.islandid);
        if (tmp != "") {
            state += ", " + tmp;
        }
    }
    LanguageCloseFile(nFile); // LEO: файл открывался на КАЖДОЙ загрузке локации и не закрывался

    FinalizeRPC(state, "", "");
}

void SeaUpdateRPC(ref loc) {
    if (!CheckRPC()) return;

    state = XI_ConvertString("OpenSea");
    if (CheckAttribute(loc, "island") && loc.island != "") {
        tmp = XI_ConvertString(loc.island);
        if (tmp != "") {
            state = XI_ConvertString("AtSeaRPC") + ", " + tmp;
        } else {
            tmp = XI_ConvertString(loc.island+NAME_GEN);
            if (tmp != "") {
                state = XI_ConvertString("AtSeaRPC") + ", " + XI_ConvertString("NearRPC") + " " + tmp;
            }
        }
	}

    FinalizeRPC(state, "", "");
}

void WDMUpdateRPC() {
    if (!CheckRPC()) return;

    if (!CheckAttribute(worldMap, "isLoaded") || worldMap.isLoaded == "" || worldMap.isLoaded == "false") {
	    StopWDMUpdateRPC();
        return;
    }
    
    state = XI_ConvertString("GlobalMapRPC");
	if (CheckAttribute(worldMap, "island") && worldMap.island != "") {
        tmp = XI_ConvertString(worldMap.island+NAME_GEN);
        if (tmp != "") {
            state += ", "  + XI_ConvertString("NearRPC") + " " + tmp;
        }
	}

    FinalizeRPC(state, "", "");
	PostEvent("WDMUpdateRPC", GLOBAL_MAP_RPC_REFRESH_TIMEOUT);
}

void StartWDMUpdateRPC() {
	SetEventHandler("WDMUpdateRPC","WDMUpdateRPC",0);
	Event("WDMUpdateRPC");
}

void StopWDMUpdateRPC() {
	DelEventHandler("WDMUpdateRPC","WDMUpdateRPC");
}

string DefaultCharDetailsRPC() {
    string details;
    if (CheckAttribute(Pchar, "name") && Pchar.name != "") {
        details = Pchar.name;
    }
    if (CheckAttribute(Pchar, "lastname") && Pchar.lastname != "") {
        details += " " + Pchar.lastname;
    }
    if (details == "") {
        // мало ли, по дефолту поставим Блейза Шарпа, хуле делать
        details = XI_ConvertString("Blaze") + " " + XI_ConvertString("Sharp")
    }
    if (CheckAttribute(Pchar, "rank")) {
        details += ", " + Pchar.rank + " " + XI_ConvertString("LevelRPC");
        details += ", " + GetLevelComplexity(MOD_SKILL_ENEMY_RATE);
    }
    return details;
}

bool CheckRPC() {
    return sti(InterfaceStates.DiscordRPC) == 1;
}

void FinalizeRPC(string state, string details, string nation) {
    if (details == "") {
        details = DefaultCharDetailsRPC();
    }
    if (nation == "") {
        nation = GetStrSmallRegister(GetNationNameByType(sti(Pchar.nation)));
    }


	SetDiscordRPC(details, state, nation, XI_ConvertString("DownloadButtonRPC"));
}

void SwitchRPC(bool on) {
    if (on) {
		SetDiscordRPC("switch_on", "", "", "");
	} else {
		SetDiscordRPC("switch_off", "", "", "");
	}
}