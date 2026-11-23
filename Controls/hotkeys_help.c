// Shared bindings and key labels for the four gameplay control groups.
// Load HUD globals only after the main segment has compiled. Legacy saves store
// references by variable index, so including HUD globals here corrupts them.
extern void HH_OverlayToggle();
extern void HH_OverlayReset();
extern void HH_OverlayChangePage(int step);

// LEO: ссылка компилируется до загрузки интерфейсов с одноимённой функцией ShowWindow.
aref HH_GetScreenBounds()
{
    aref bounds;
    makearef(bounds,showWindow);
    return bounds;
}

void HH_Toggle()
{
    if (!SegmentIsLoaded("battle_interface\hotkeys_help.c"))
    {
        if (!LoadSegment("battle_interface\hotkeys_help.c"))
        {
            trace("Hotkeys help: HUD module failed to load");
            return;
        }
    }
    HH_OverlayToggle();
}

void HH_Reset()
{
    if (SegmentIsLoaded("battle_interface\hotkeys_help.c")) HH_OverlayReset();
}

void HH_ChangePage(int step)
{
    if (SegmentIsLoaded("battle_interface\hotkeys_help.c")) HH_OverlayChangePage(step);
}

bool HH_BindingConflicts(string sControl, string sKey)
{
    int keyCode = CI_GetKeyCode(sKey);
    if (sControl != "HotkeysHelp" && sControl != "HotkeysHelpPrevPage" && sControl != "HotkeysHelpNextPage")
    {
        if (CheckAttribute(&objControlsState, "boundkeys.HotkeysHelp") && keyCode == sti(objControlsState.boundkeys.HotkeysHelp)) return true;
        if (CheckAttribute(&objControlsState, "boundkeys.HotkeysHelpPrevPage") && keyCode == sti(objControlsState.boundkeys.HotkeysHelpPrevPage)) return true;
        if (CheckAttribute(&objControlsState, "boundkeys.HotkeysHelpNextPage") && keyCode == sti(objControlsState.boundkeys.HotkeysHelpNextPage)) return true;
        return false;
    }

    aref arBindings, arBinding;
    makearef(arBindings, objControlsState.boundkeys);
    for (int i = 0; i < GetAttributesNum(arBindings); i++)
    {
        arBinding = GetAttributeN(arBindings, i);
        string name = GetAttributeName(arBinding);
        // Legacy F1 interface exit and unused network statistics may share F1.
        if (name == sControl || name == "IExit_F1" || name == "gamestat") continue;
        if (!HH_OverlapsGameplay(name)) continue;
        if (sti(GetAttributeValue(arBinding)) == keyCode) return true;
    }
    return false;
}

bool HH_OverlapsGameplay(string control)
{
    bool mapped = false;
    aref groups, group;
    makearef(groups, objControlsState.keygroups);
    for (int i = 0; i < GetAttributesNum(groups); i++)
    {
        group = GetAttributeN(groups, i);
        if (!CheckAttribute(group, control)) continue;
        mapped = true;
        string name = GetAttributeName(group);
        if (name == "PrimaryLand" || name == "FightModeControls" || name == "Sailing1Pers" || name == "Sailing3Pers" || name == "BattleInterfaceControls" || name == "LandTactical") return true;
    }
    // Controls without a group remain active globally. Dialog/interface-only
    // bindings cannot conflict with this HUD, which is hidden in those modes.
    return !mapped;
}

void HH_EnsureBinding()
{
    HH_EnsureControlBinding("HotkeysHelp");
    HH_EnsureControlBinding("HotkeysHelpPrevPage");
    HH_EnsureControlBinding("HotkeysHelpNextPage");
}

void HH_EnsureControlBinding(string control)
{
    if (!CheckAttribute(&objControlsState, "keygroups.PrimaryLand." + control)) return;
    string key = objControlsState.keygroups.PrimaryLand.(control);
    if (!HH_BindingConflicts(control, key)) return;

    // Preserve older custom F1 bindings rather than steal their gameplay key.
    string candidate;
    for (int i = 1; i <= 12; i++)
    {
        candidate = "VK_F" + i;
        if (!HH_BindingConflicts(control, candidate))
        {
            CI_CreateAndSetControls("PrimaryLand", control, CI_GetKeyCode(candidate), 0, true);
            trace("Hotkeys help: conflicting binding replaced with " + candidate);
            return;
        }
    }
    trace("Hotkeys help: no unused function key; keep current binding until options are edited");
}

int HH_ControlKeyCode(string groupName, string controlName)
{
    // Group attributes are the authoritative saved/reassigned bindings.
    if (CheckAttribute(&objControlsState, "keygroups." + groupName + "." + controlName))
    {
        string key = objControlsState.keygroups.(groupName).(controlName);
        if (key != "") return CI_GetKeyCode(key);
    }
    if (CheckAttribute(&objControlsState, "boundkeys." + controlName))
        return sti(objControlsState.boundkeys.(controlName));
    return -1;
}
