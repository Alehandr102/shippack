// LEO: Черновик окна; запись ragdoll.ini только по общему ОК.
int iRagdollEnabled = 1;
int iRagdollMaxActive = 24;
int iRagdollSeconds = 8;

void Ragdoll_InitOptions()
{
    InterfaceStates.RagdollEnabled = GetRagdollOption(0);
    InterfaceStates.RagdollMaxActive = GetRagdollOption(1);
    InterfaceStates.RagdollSeconds = GetRagdollOption(2);
}

void Ragdoll_RefreshDraft()
{
    if(iRagdollMaxActive < 1) iRagdollMaxActive = 1;
    if(iRagdollMaxActive > 256) iRagdollMaxActive = 256;
    if(iRagdollSeconds < 2) iRagdollSeconds = 2;
    if(iRagdollSeconds > 8) iRagdollSeconds = 8;
    SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHECK_RAGDOLL_ENABLED",2,1,iRagdollEnabled);
    SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"RAGDOLL_MAX_SLIDE",2,iRagdollMaxActive-1);
    SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"RAGDOLL_SECONDS_SLIDE",2,iRagdollSeconds-2);
    SetFormatedText("RAGDOLL_MAX_VALUE",its(iRagdollMaxActive));
    SetFormatedText("RAGDOLL_SECONDS_VALUE",its(iRagdollSeconds));
}

void ShowRagdollWindow(bool show)
{
    if(show)
    {
        iRagdollEnabled = sti(InterfaceStates.RagdollEnabled);
        iRagdollMaxActive = sti(InterfaceStates.RagdollMaxActive);
        iRagdollSeconds = sti(InterfaceStates.RagdollSeconds);
        InterfaceStates.OptSubWin = "RAGDOLL";
        Ragdoll_RefreshDraft();
        SetFormatedText("RAGDOLL_NOTE",XI_ConvertString("RagdollPerformance"));
        SetFormatedText("RAGDOLL_APPLY_NOTE",XI_ConvertString("RagdollApplyAfterTransition"));
        XI_WindowDisable("RAGDOLL_WINDOW",false);
        XI_WindowShow("RAGDOLL_WINDOW",true);
        XI_WindowDisable("OTHERS_WINDOW",true);
        XI_WindowDisable("MAIN_WINDOW",true);
        SetCurrentNode("RAGDOLL_OK");
    }
    else
    {
        XI_WindowDisable("RAGDOLL_WINDOW",true);
        XI_WindowShow("RAGDOLL_WINDOW",false);
        DeleteAttribute(&InterfaceStates,"OptSubWin");
        XI_WindowDisable("OTHERS_WINDOW",false);
        XI_WindowDisable("MAIN_WINDOW",false);
        SetCurrentNode("TABLE_GAME_OPTIONS_MASTER");
    }
}

bool Ragdoll_CheckChange(string node, int enabled)
{
    if(node != "CHECK_RAGDOLL_ENABLED") return false;
    iRagdollEnabled = enabled;
    return true;
}

bool Ragdoll_Button(string node)
{
    switch(node)
    {
        case "RAGDOLL_OK":
            InterfaceStates.RagdollEnabled = iRagdollEnabled;
            InterfaceStates.RagdollMaxActive = iRagdollMaxActive;
            InterfaceStates.RagdollSeconds = iRagdollSeconds;
            ShowRagdollWindow(false);
            FillOptionsTable("TABLE_GAME_OPTIONS_MASTER");
            return true; break;
        case "RAGDOLL_CANCEL":
            ShowRagdollWindow(false);
            return true; break;
    }
    return false;
}

bool Ragdoll_SlideChange(string node, int value)
{
    if (node != "RAGDOLL_MAX_SLIDE" && node != "RAGDOLL_SECONDS_SLIDE") return false;
    if (!CheckAttribute(&InterfaceStates,"OptSubWin")) return true;
    if (InterfaceStates.OptSubWin != "RAGDOLL") return true;
    if (node == "RAGDOLL_MAX_SLIDE")
    {
        iRagdollMaxActive = value + 1;
        SetFormatedText("RAGDOLL_MAX_VALUE",its(iRagdollMaxActive));
    }
    else
    {
        iRagdollSeconds = value + 2;
        SetFormatedText("RAGDOLL_SECONDS_VALUE",its(iRagdollSeconds));
    }
    return true;
}

bool Ragdoll_SaveOptions()
{
    if(sti(InterfaceStates.RagdollEnabled) == GetRagdollOption(0))
    {
        if(sti(InterfaceStates.RagdollMaxActive) == GetRagdollOption(1))
        {
            if(sti(InterfaceStates.RagdollSeconds) == GetRagdollOption(2)) return true;
        }
    }
    if(SetRagdollOptions(sti(InterfaceStates.RagdollEnabled),sti(InterfaceStates.RagdollMaxActive),sti(InterfaceStates.RagdollSeconds)) != 0) return true;
    ShowRagdollWindow(true);
    SetFormatedText("RAGDOLL_NOTE",XI_ConvertString("RagdollSaveError"));
    return false;
}
