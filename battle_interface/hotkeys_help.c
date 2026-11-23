// Пассивная подсказка: только REALIZE, без перехвата управления и остановки игры.
object hhOverlay;
object hhRows;
bool hhVisible = false;
string hhGroup = "";
int hhPage = 0;
int hhPages = 1;
int hhRow = 0;
int hhLanguage = -1;
int hhControlLanguage = -1;
int hhWidth = 0;
int hhHeight = 0;
int hhRevision = -1;
float hhFontScale;
int hhLineHeight;
int hhLeft, hhTop, hhRight, hhBottom;
int hhBodyTop;
int hhKeyWidth, hhActionWidth, hhConditionWidth;
int hhColumnWidth;

#event_handler("frame", "HH_Update");

string HH_GameplayGroup()
{
    if (sti(InterfaceStates.Launched) || DialogRun || IsEntity(&worldMap) || IsEntity(&aviVideoObj)) return "";
    if (!CheckAttribute(&InterfaceStates, "Buttons.Resume.enable")) return "";
    if (sti(InterfaceStates.Buttons.Resume.enable) != 1 || questMovieProcess > 0) return "";
    if (IsEntity(reload_fader) || CharacterIsDead(GetMainCharacter())) return "";
    if (bSeaActive && !bAbordageStarted)
    {
        if (SeaCameras.Camera == "SeaDeckCamera") return "Sailing1Pers";
        return "Sailing3Pers";
    }
    if (!bLandInterfaceStart) return "";
    if (SendMessage(GetMainCharacter(), "ls", MSG_CHARACTER_EX_MSG, "IsFightMode") != 0) return "FightModeControls";
    return "PrimaryLand";
}

void HH_OverlayToggle()
{
    if (hhVisible)
    {
        HH_Reset();
        return;
    }
    if (HH_GameplayGroup() == "")
    {
        trace("Hotkeys help: ignored outside gameplay; group=" + GetCurControlGroup());
        return;
    }
    hhVisible = true;
    hhPage = 0;
    HH_Update();
}

void HH_OverlayChangePage(int step)
{
    if (!hhVisible || HH_GameplayGroup() == "") return;
    hhPage += step;
    if (hhPage < 0) hhPage = hhPages - 1;
    if (hhPage >= hhPages) hhPage = 0;
    // Перестраиваем только подсказку, не боевой интерфейс.
    hhRevision = -1;
    HH_Update();
}

void HH_OverlayReset()
{
    hhVisible = false;
    hhGroup = "";
    hhRevision = -1;
    hhPage = 0;
    DeleteClass(&hhOverlay);
    DeleteAttribute(&hhOverlay, "");
    DeleteAttribute(&hhRows, "");
}

void HH_Update()
{
    if (!hhVisible) return;
    string group = HH_GameplayGroup();
    if (group == "")
    {
        // Временно скрываем в меню, диалогах, роликах и переходах.
        if (IsEntity(&hhOverlay)) DeleteClass(&hhOverlay);
        hhRevision = -1;
        return;
    }
    aref viewportBounds; viewportBounds=HH_GetScreenBounds();
    int width = sti(viewportBounds.right) - sti(viewportBounds.left);
    int height = sti(viewportBounds.bottom) - sti(viewportBounds.top);
    if (width <= 0 || height <= 0) return;
    int revision = 0;
    if (CheckAttribute(&objControlsState, "bindingRevision")) revision = sti(objControlsState.bindingRevision);
    if (IsEntity(&hhOverlay) && group == hhGroup && width == hhWidth && height == hhHeight && revision == hhRevision) return;
    if (group != hhGroup) hhPage = 0;
    hhGroup = group;
    hhWidth = width;
    hhHeight = height;
    hhRevision = revision;
    HH_Rebuild();
}

void HH_AddRow(string key, string action, string condition)
{
    // Несколько клавиш одного действия объединяем в одну строку.
    for (int i = 1; i <= hhRow; i++)
    {
        string existing = "tr" + i;
        if (hhRows.(existing).action != action || hhRows.(existing).condition != condition) continue;
        if (hhRows.(existing).key == key) return;
        hhRows.(existing).key = hhRows.(existing).key + " / " + key;
        return;
    }
    hhRow++;
    string row = "tr" + hhRow;
    hhRows.(row).key = key;
    hhRows.(row).action = action;
    hhRows.(row).condition = condition;
    hhRows.(row).section = false;
}

void HH_AddSection(int category)
{
    HH_AddRow("", HH_Text("Category_" + category), "");
    string row = "tr" + hhRow;
    hhRows.(row).section = true;
}

void HH_WrapCell(string row, string cell, string text, int width)
{
    int count = 0;
    string line = "";
    while (text != "")
    {
        int space = findsubstr(text, " ", 0);
        string word = text;
        if (space >= 0)
        {
            if (space == 0) word = "";
            else word = strcut(text, 0, space - 1);
            if (space + 1 >= strlen(text)) text = "";
            else text = strcut(text, space + 1, strlen(text) - 1);
        }
        else text = "";
        if (word == "") continue;
        string candidate = word;
        if (line != "") candidate = line + " " + word;
        if (line != "" && GetStringWidth(candidate, "interface_normal", hhFontScale) > width)
        {
            string id = "l" + count;
            hhRows.(row).(cell).(id) = line;
            count++;
            line = word;
        }
        else line = candidate;
    }
    if (line != "")
    {
        string last = "l" + count;
        hhRows.(row).(cell).(last) = line;
        count++;
    }
    hhRows.(row).(cell).count = count;
}

void HH_Layout()
{
    aref viewportBounds; viewportBounds=HH_GetScreenBounds();
    float ratio = stf(hhHeight) / 768.0;
    hhFontScale = 0.82 * ratio;
    hhLineHeight = makeint(20 * ratio);
    if (hhLineHeight < 12) hhLineHeight = 12;
    hhLeft = sti(viewportBounds.left) + makeint(hhWidth * 0.06);
    hhRight = sti(viewportBounds.right) - makeint(hhWidth * 0.06);
    hhTop = sti(viewportBounds.top) + makeint(hhHeight * 0.10);
    hhBottom = sti(viewportBounds.bottom) - makeint(hhHeight * 0.10);
    hhBodyTop = hhTop + hhLineHeight * 4;
    int bodyBottom = hhBottom - hhLineHeight * 2;
    int innerWidth = hhRight - hhLeft - hhLineHeight * 2;
    hhColumnWidth = (innerWidth - hhLineHeight) / 2;
    hhKeyWidth = makeint(hhColumnWidth * 0.30);
    hhActionWidth = hhColumnWidth - hhKeyWidth;
    hhConditionWidth = 0;
    int y = hhBodyTop;
    int page = 0;
    int column = 0;
    for (int i = 1; i <= hhRow; i++)
    {
        string row = "tr" + i;
        int keyWidth = hhKeyWidth - hhLineHeight / 2;
        int actionWidth = hhActionWidth;
        string action = hhRows.(row).action;
        if (hhRows.(row).condition != "") action = action + " (" + hhRows.(row).condition + ")";
        if (sti(hhRows.(row).section)) actionWidth = hhColumnWidth;
        HH_WrapCell(row, "keylines", hhRows.(row).key, keyWidth);
        HH_WrapCell(row, "actionlines", action, actionWidth);
        int lines = sti(hhRows.(row).keylines.count);
        if (sti(hhRows.(row).actionlines.count) > lines) lines = sti(hhRows.(row).actionlines.count);
        if (lines < 1) lines = 1;
        int rowHeight = (lines * hhLineHeight) + makeint(hhLineHeight * 0.12);
        // Заголовок не отрываем от первой строки раздела.
        int reserve = 0;
        if (sti(hhRows.(row).section) && i < hhRow) reserve = hhLineHeight * 2;
        if (y + rowHeight + reserve > bodyBottom && y > hhBodyTop)
        {
            column++;
            if (column > 1) { column = 0; page++; }
            y = hhBodyTop;
        }
        hhRows.(row).page = page;
        hhRows.(row).column = column;
        hhRows.(row).y = y;
        y += rowHeight;
    }
    hhPages = page + 1;
    if (hhPage >= hhPages) hhPage = hhPages - 1;
}

void HH_TextNode(string name, string text, int x, int y, int color, float scale)
{
    hhOverlay.textinfo.(name).font = "interface_normal";
    hhOverlay.textinfo.(name).align = "left";
    hhOverlay.textinfo.(name).pos.x = x;
    hhOverlay.textinfo.(name).pos.y = y;
    hhOverlay.textinfo.(name).scale = scale;
    hhOverlay.textinfo.(name).color = color;
    hhOverlay.textinfo.(name).shadow = true;
    hhOverlay.textinfo.(name).text = text;
}

void HH_DrawCell(string row, string cell, int x, int color)
{
    for (int i = 0; i < sti(hhRows.(row).(cell).count); i++)
    {
        string line = "l" + i;
        HH_TextNode(row + cell + line, hhRows.(row).(cell).(line), x, sti(hhRows.(row).y) + i * hhLineHeight, color, hhFontScale);
    }
}

void HH_Rebuild()
{
    DeleteClass(&hhOverlay);
    DeleteAttribute(&hhOverlay, "");
    hhLanguage = LanguageOpenFile("HotkeysHelp.txt");
    hhControlLanguage = LanguageOpenFile("ControlsNames.txt");
    HH_FillList();
    HH_Layout();
    int x = hhLeft + hhLineHeight;
    int white = argb(255, 245, 239, 221);
    int gold = argb(255, 255, 218, 137);
    string title = HH_Text("Title") + " - " + LanguageConvertString(hhControlLanguage, hhGroup) + "  (" + (hhPage + 1) + "/" + hhPages + ")";
    HH_TextNode("title", title, x, hhTop + hhLineHeight, gold, hhFontScale);
    HH_TextNode("keyheader", HH_Text("ColumnKey"), x, hhTop + hhLineHeight * 3, gold, hhFontScale);
    HH_TextNode("actionheader", HH_Text("ColumnAction"), x + hhKeyWidth, hhTop + hhLineHeight * 3, gold, hhFontScale);
    HH_TextNode("keyheader2", HH_Text("ColumnKey"), x + hhColumnWidth + hhLineHeight, hhTop + hhLineHeight * 3, gold, hhFontScale);
    HH_TextNode("actionheader2", HH_Text("ColumnAction"), x + hhColumnWidth + hhLineHeight + hhKeyWidth, hhTop + hhLineHeight * 3, gold, hhFontScale);
    for (int i = 1; i <= hhRow; i++)
    {
        string row = "tr" + i;
        if (sti(hhRows.(row).page) != hhPage) continue;
        int color = white;
        if (sti(hhRows.(row).section)) color = gold;
        int rowX = x + sti(hhRows.(row).column) * (hhColumnWidth + hhLineHeight);
        HH_DrawCell(row, "keylines", rowX, color);
        if (!sti(hhRows.(row).section)) rowX += hhKeyWidth;
        HH_DrawCell(row, "actionlines", rowX, color);
    }
    string footer = HH_KeyLabel(HH_ControlKeyCode(hhGroup, "HotkeysHelp")) + " - " + HH_Text("HideHint") + "; "
        + HH_KeyLabel(HH_ControlKeyCode(hhGroup, "HotkeysHelpPrevPage")) + " / "
        + HH_KeyLabel(HH_ControlKeyCode(hhGroup, "HotkeysHelpNextPage")) + " - " + HH_Text("PageHint");
    HH_TextNode("footer", footer, x, hhBottom - hhLineHeight, gold, hhFontScale);
    // Рендер HUD с пустым списком офицеров; без EXECUTE он не перехватывает ввод.
    hhOverlay.Parameters.DoShowCommandos = true;
    hhOverlay.data.icons = "";
    hhOverlay.imageslist.panel.texture = "interfaces\\sith\\papirus_big.dds";
    hhOverlay.imageslist.panel.color = argb(225, 28, 32, 40);
    hhOverlay.imageslist.panel.uv = "0.15,0.08,0.85,0.92";
    hhOverlay.imageslist.panel.pos = "" + hhLeft + "," + hhTop + "," + hhRight + "," + hhBottom;
    CreateEntity(&hhOverlay, "BATTLE_LAND_INTERFACE");
    // Поверх мира и воды, но под переходным затемнением.
    LayerAddObject(INTERFACE_REALIZE, &hhOverlay, -1);
    LanguageCloseFile(hhLanguage);
    LanguageCloseFile(hhControlLanguage);
    hhLanguage = -1;
    hhControlLanguage = -1;
}

string HH_Text(string id)
{
    return LanguageConvertString(hhLanguage, id);
}

string HH_KeyLabel(int keyCode)
{
    if (keyCode < 0) return HH_Text("Unassigned");
    if (keyCode == 256) return HH_Text("Mouse");
    if (keyCode == 257) return HH_Text("Mouse");
    if (keyCode >= 65 && keyCode <= 90) return strcut("ABCDEFGHIJKLMNOPQRSTUVWXYZ", keyCode - 65, keyCode - 65);
    if (keyCode >= 48 && keyCode <= 57) return "" + (keyCode - 48);
    if (keyCode >= 112 && keyCode <= 123) return "F" + (keyCode - 111);
    if (keyCode >= 96 && keyCode <= 105) return "Num " + (keyCode - 96);
    string label = HH_Text("Key_" + keyCode);
    if (label == "" || label == "Key_" + keyCode) return CI_GetKeyName(keyCode);
    return label;
}

int HH_Category(string control)
{
    switch (control)
    {
        case "HotkeysHelp": return 0; break;
        case "HotkeysHelpPrevPage": return 0; break;
        case "HotkeysHelpNextPage": return 0; break;
        case "MainMenuDouble": return 0; break;
        case "Interface": return 0; break;
        case "InterfaceDisabler": return 0; break;
        case "QuickSave": return 0; break;
        case "QuickLoad": return 0; break;
        case "TimeScale": return 0; break;
        case "TimeScaleFaster": return 0; break;
        case "TimeScaleSlower": return 0; break;
        case "TimeScaleFasterBA": return 0; break;
        case "TimeScaleSlowerBA": return 0; break;
        case "VK_PAUSETimePause": return 0; break;
        case "ChrForward": return 1; break;
        case "ChrForward2": return 1; break;
        case "ChrBackward": return 1; break;
        case "ChrStrafeLeft": return 1; break;
        case "ChrStrafeRight": return 1; break;
        case "ChrRun": return 1; break;
        case "ChrJump": return 1; break;
        case "DeckCamera_Forward": return 1; break;
        case "DeckCamera_Backward": return 1; break;
        case "ChrFightMode": return 2; break;
        case "ChrFire": return 2; break;
        case "MushketHotkey": return 2; break;
        case "SwitchShootingMode": return 2; break;
        case "SoloParry": return 2; break;
        case "SoloFient": return 2; break;
        case "BOAL_ActivateRush": return 2; break;
        case "Ship_Fire": return 2; break;
        case "DropMine": return 2; break;
    }
    if (findsubstr(control, "Ship_Turn", 0) == 0 || findsubstr(control, "Ship_Sail", 0) == 0) return 1;
    if (findsubstr(control, "ChrAttack", 0) == 0 || findsubstr(control, "ChrAltAttack", 0) == 0 || findsubstr(control, "ChrBlock", 0) == 0) return 2;
    if (findsubstr(control, "hk_charge", 0) == 0) return 2;
    if (findsubstr(control, "Sea_Tac", 0) == 0 || control == "LandTacTest" || control == "Land_TacCmd") return 6;
    if (findsubstr(control, "Officers", 0) == 0 || findsubstr(control, "CIC_", 0) == 0 || findsubstr(control, "BICommands", 0) == 0 || control == "LICommandsActivate" || control == "BIFastCommand" || control == "SeaTowToggle") return 4;
    if (findsubstr(control, "Camera", 0) >= 0 || findsubstr(control, "ChrCam", 0) == 0 || findsubstr(control, "Telescope", 0) == 0 || findsubstr(control, "Radar", 0) == 0 || findsubstr(control, "MiniMap", 0) == 0 || findsubstr(control, "Zoom", 0) == 0 || findsubstr(control, "Sea_", 0) == 0 || control == "MapView" || control == "Map_Best" || control == "Ship_GetCoordinates" || control == "drawShootDistance") return 3;
    return 5;
}

bool HH_Skip(string control)
{
    return control == "TelescopeOut" || control == "AltModificatorOff" || control == "AltModificator" || control == "HotkeysHelpPrevPage" || control == "HotkeysHelpNextPage" || control == "ChrAttackChoseBase";
}

string HH_Condition(string control)
{
    if (findsubstr(control, "Fast_", 0) == 0) return HH_Text("CondFastTravel");
    if (findsubstr(control, "Sea_Tac", 0) == 0 && control != "Sea_TacticalView") return HH_Text("CondSeaTactical");
    if (control == "Land_TacCmd") return HH_Text("CondLandTactical");
    if (findsubstr(control, "Officers", 0) == 0) return HH_Text("CondOfficers");
    if (findsubstr(control, "CIC_", 0) == 0) return HH_Text("CondSquadron");
    if (findsubstr(control, "setEquipment", 0) == 0) return HH_Text("CondEquipment");
    if (findsubstr(control, "BOAL_Activate", 0) == 0) return HH_Text("CondPerk");
    if (findsubstr(control, "Sea_Mars", 0) == 0 && control != "Sea_MarsView") return HH_Text("CondMars");
    if (findsubstr(control, "TelescopeZoom", 0) == 0) return HH_Text("CondTelescope");
    if (findsubstr(control, "BICommands", 0) == 0 && control != "BICommandsActivate") return HH_Text("CondCommandMenu");
    switch (control)
    {
        case "Sea_AimMode": return HH_Text("CondDeck"); break;
        case "SeaTowToggle": return HH_Text("CondSeaTow"); break;
        case "HotkeysHelpPrevPage": return HH_Text("CondHelpVisible"); break;
        case "HotkeysHelpNextPage": return HH_Text("CondHelpVisible"); break;
        case "TelescopeIn": return HH_Text("CondSpyglass"); break;
        case "ChrCamSpecMode": return HH_Text("CondHold"); break;
        case "ChrCamShoulderSwap": return HH_Text("CondShoulder"); break;
        case "ChrRun": return ""; break;
        case "BOAL_UsePotion": return HH_Text("CondPotion"); break;
        case "UseAntidote": return HH_Text("CondAntidote"); break;
        case "UseFood": return HH_Text("CondFood"); break;
        case "MushketHotkey":
            if (hhGroup == "PrimaryLand" && MusketSharesStoreKey()) return HH_Text("CondMusketNoAlt");
            return HH_Text("CondMusket"); break;
        case "SwitchShootingMode": return HH_Text("CondDoubleShot"); break;
        case "ChrFire": return HH_Text("CondLoadedWeapon"); break;
        case "BOAL_DeadSearch": return HH_Text("CondCorpse"); break;
        case "csmRunLootCollector": return HH_Text("CondLoot"); break;
        case "Say": return HH_Text("CondTreasurer"); break;
        case "Map_Best": return HH_Text("CondMap"); break;
        case "MapView": return HH_Text("CondMap"); break;
        case "GoCabin": return HH_Text("CondCabin"); break;
        case "DropMine": return HH_Text("CondMine"); break;
        case "Ship_Fire": return HH_Text("CondShipFire"); break;
        case "BIFastCommand": return HH_Text("CondFastCommand"); break;
        case "Interface": return HH_Text("CondInterface"); break;
        case "QuickSave": return HH_Text("CondSave"); break;
        case "QuickLoad": return HH_Text("CondLoad"); break;
    }
    return "";
}

void HH_AddControl(string control)
{
    int code = HH_ControlKeyCode(hhGroup, control);
    if (HH_Skip(control) || CheckAttribute(&hhRows, "seen." + control)) return;
    if (!CheckAttribute(&objControlsState, "boundkeys." + control) && !CheckAttribute(&objControlsState, "keygroups." + hhGroup + "." + control)) return;
    string key = HH_KeyLabel(code);
    if (findsubstr(control, "Fast_", 0) == 0)
        key = HH_KeyLabel(HH_ControlKeyCode(hhGroup, "AltModificator")) + " + " + key;
    hhRows.seen.(control) = true;
    string meaning = control;
    if (control == "ChrForward2") meaning = "ChrForward";
    if (control == "TimeScaleFasterBA") meaning = "TimeScaleFaster";
    if (control == "TimeScaleSlowerBA") meaning = "TimeScaleSlower";
    if (control == "ChrCamTurnH" || control == "ChrCamTurnV" || control == "ShipCamera_Turn_H" || control == "ShipCamera_Turn_V" || control == "DeckCamera_Turn_H" || control == "DeckCamera_Turn_V") meaning = "CameraLook";
    string action = HH_Text("Action_" + meaning);
    if (action == "" || action == "Action_" + meaning) action = LanguageConvertString(hhControlLanguage, meaning);
    if (action == "" || action == control)
    {
        trace("Hotkeys help: missing ControlsNames translation for " + control);
        action = control;
    }
    HH_AddRow(key, action, HH_Condition(control));
}

void HH_AddAttackCombo(string baseControl, string action)
{
    int modifier = HH_ControlKeyCode(hhGroup, "ChrAttackChoseBase");
    int baseKey = HH_ControlKeyCode(hhGroup, baseControl);
    if (modifier < 0 || baseKey < 0) return;
    HH_AddRow(HH_KeyLabel(modifier) + " + " + HH_KeyLabel(baseKey), HH_Text(action), HH_Text("CondMelee"));
}


bool HH_HasControl(string control)
{
    return CheckAttribute(&objControlsState, "boundkeys." + control) || CheckAttribute(&objControlsState, "keygroups." + hhGroup + "." + control);
}

void HH_AddPair(string first, string second, string action, string condition)
{
    if (!HH_HasControl(first) || !HH_HasControl(second)) return;
    if (HH_Skip(first) || HH_Skip(second)) return;
    hhRows.seen.(first) = true;
    hhRows.seen.(second) = true;
    HH_AddRow(HH_KeyLabel(HH_ControlKeyCode(hhGroup, first)) + " / " + HH_KeyLabel(HH_ControlKeyCode(hhGroup, second)), HH_Text(action), HH_Text(condition));
}

void HH_PackBegin()
{
    hhRows.pack.keys = "";
    hhRows.pack.actions = "";
}

void HH_PackControl(string control, string caption)
{
    if (!HH_HasControl(control)) return;
    hhRows.seen.(control) = true;
    string key = HH_KeyLabel(HH_ControlKeyCode(hhGroup, control));
    if (findsubstr(control, "Fast_", 0) == 0) key = HH_KeyLabel(HH_ControlKeyCode(hhGroup, "AltModificator")) + "+" + key;
    if (hhRows.pack.keys != "") hhRows.pack.keys = hhRows.pack.keys + " / ";
    hhRows.pack.keys = hhRows.pack.keys + key;
    if (hhRows.pack.actions != "") hhRows.pack.actions = hhRows.pack.actions + "; ";
    hhRows.pack.actions = hhRows.pack.actions + key + " " + HH_Text(caption);
}

void HH_PackEnd(string action, string condition)
{
    if (hhRows.pack.keys == "") return;
    // У каждого действия сохраняем его переназначенную клавишу.
    HH_AddRow(HH_Text(action), hhRows.pack.actions, HH_Text(condition));
}

void HH_AddSeaGrappleControls()
{
    if (hhGroup != "Sailing1Pers" && hhGroup != "Sailing3Pers") return;
    HH_AddControl("SeaTowToggle");
    int cutKey = HH_ControlKeyCode(hhGroup, "Press7");
    if (cutKey >= 0) HH_AddRow(HH_KeyLabel(cutKey), HH_Text("Action_CutBoardingRopes"), HH_Text("CondCutBoardingRopes"));
}

void HH_CompactCategory(int category)
{
    bool seaMode = hhGroup == "Sailing1Pers" || hhGroup == "Sailing3Pers";
    if (category == 1)
    {
        if (hhGroup == "PrimaryLand" || hhGroup == "FightModeControls")
        {
            HH_AddPair("ChrForward", "ChrBackward", "PairWalk", "Empty");
            HH_AddPair("ChrStrafeLeft", "ChrStrafeRight", "PairStrafe", "Empty");
        }
        else
        {
            HH_AddPair("Ship_TurnLeft", "Ship_TurnRight", "PairSteer", "Empty");
            HH_AddPair("Ship_SailUp", "Ship_SailDown", "PairSails", "Empty");
            if (hhGroup == "Sailing1Pers") HH_AddPair("DeckCamera_Forward", "DeckCamera_Backward", "PairDeckWalk", "Empty");
        }
    }
    if (category == 2 && seaMode)
    {
        HH_PackBegin();
        HH_PackControl("hk_charge1", "AmmoBalls"); HH_PackControl("hk_charge2", "AmmoGrapes");
        HH_PackControl("hk_charge3", "AmmoChain"); HH_PackControl("hk_charge4", "AmmoBombs");
        HH_PackEnd("PackAmmo", "Empty");
    }
    if (category == 3)
    {
        if (hhGroup == "PrimaryLand" || hhGroup == "FightModeControls")
        {
            HH_AddPair("ZoomIn", "ZoomOut", "PairZoom", "Empty");
            HH_AddPair("RadarRangeDown", "RadarRangeUp", "PairRadarRange", "Empty");
        }
        else
        {
            HH_AddPair("ShipCamera_Forward", "ShipCamera_Backward", "PairZoom", "Empty");
            HH_AddPair("TelescopeZoomIn", "TelescopeZoomOut", "PairSpyglassZoom", "CondTelescope");
            HH_AddPair("Sea_MarsPrevMast", "Sea_MarsNextMast", "PairMast", "CondMars");
            HH_AddPair("MiniMapZoomIn", "MiniMapZoomOut", "PairMiniMapZoom", "Empty");
        }
    }
    if (category == 4)
    {
        HH_AddPair("BICommandsLeft", "BICommandsRight", "PairCommandHorizontal", "CondCommandMenu");
        // В некоторых раскладках вертикальные команды не назначены.
        HH_AddPair("BICommandsUp", "BICommandsDown", "PairCommandVertical", "CondCommandMenu");
        if (hhGroup == "Sailing1Pers" || hhGroup == "Sailing3Pers")
        {
            HH_AddSeaGrappleControls();
            HH_PackBegin();
            HH_PackControl("CIC_LoadBalls", "AmmoBalls"); HH_PackControl("CIC_LoadGrapes", "AmmoGrapes");
            HH_PackControl("CIC_LoadChain", "AmmoChain"); HH_PackControl("CIC_LoadBombs", "AmmoBombs");
            HH_PackControl("CIC_LoadAtWill", "AmmoAtWill");
            HH_PackEnd("PackFleetAmmo", "CondSquadron");
        }
    }
    if (category == 5 && hhGroup == "PrimaryLand")
    {
        HH_PackBegin();
        HH_PackControl("Fast_Port", "PlacePort"); HH_PackControl("Fast_store", "PlaceStore");
        HH_PackControl("Fast_Shipyard", "PlaceShipyard"); HH_PackControl("Fast_tavern", "PlaceTavern");
        HH_PackControl("Fast_townhall", "PlaceTownhall");
        HH_PackEnd("PackFastTravel", "CondFastTravel");
        HH_PackBegin();
        HH_PackControl("Fast_bank", "PlaceBank"); HH_PackControl("Fast_church", "PlaceChurch");
        HH_PackControl("Fast_Brothel", "PlaceBrothel"); HH_PackControl("Fast_PortOffice", "PlacePortOffice");
        HH_PackControl("Fast_prison", "PlacePrison");
        HH_PackEnd("PackFastTravel", "CondFastTravel");
        HH_PackBegin();
        for (int j = 1; j <= 6; j++) HH_PackControl("setEquipment" + j, "Set_" + j);
        HH_PackEnd("PackEquipment", "CondEquipment");
    }
    if (category == 6 && seaMode)
    {
        HH_AddPair("Sea_TacPanLeft", "Sea_TacPanRight", "PairPanHorizontal", "CondSeaTactical");
        HH_AddPair("Sea_TacPanUp", "Sea_TacPanDown", "PairPanVertical", "CondSeaTactical");
        HH_AddPair("Sea_TacZoomIn", "Sea_TacZoomOut", "PairZoom", "CondSeaTactical");
        HH_AddPair("Sea_TacRotLeft", "Sea_TacRotRight", "PairRotate", "CondSeaTactical");
    }
}

void HH_FillList()
{
    DeleteAttribute(&hhRows, "");
    hhRow = 0;
    bool landMode = hhGroup == "PrimaryLand" || hhGroup == "FightModeControls";
    aref group, control;
    makearef(group, objControlsState.keygroups.(hhGroup));
    for (int category = 1; category <= 6; category++)
    {
        int sectionRow = hhRow;
        HH_AddSection(category);
        HH_CompactCategory(category);
        for (int i = 0; i < GetAttributesNum(group); i++)
        {
            control = GetAttributeN(group, i);
            string name = GetAttributeName(control);
            if (HH_Category(name) == category) HH_AddControl(name);
        }
        if (category == 2 && hhGroup == "FightModeControls")
        {
            HH_AddAttackCombo("ChrAttackBase", "ComboParry");
            HH_AddAttackCombo("ChrAttackBreakBase", "ComboFeint");
            HH_AddAttackCombo("ChrAltAttackBase", "ComboRound");
        }
        if (category == 3 && landMode)
        {
            HH_AddControl("ChrCamTurnH"); HH_AddControl("ChrCamTurnV"); HH_AddControl("ChrCamSpecMode");
        }
        if (category == 4)
        {
            HH_AddControl("BICommandsConfirm"); HH_AddControl("BICommandsCancel");
            HH_AddControl("BICommandsLeft"); HH_AddControl("BICommandsRight");
            HH_AddControl("BICommandsUp"); HH_AddControl("BICommandsDown");
        }
        if (hhRow == sectionRow + 1)
        {
            string emptyRow = "tr" + hhRow;
            DeleteAttribute(&hhRows, emptyRow);
            hhRow--;
        }
    }
}
