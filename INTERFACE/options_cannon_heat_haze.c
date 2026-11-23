// LEO: Отдельная опция графики; Cancel не меняет движок и engine.ini.
void ApplyCannonHeatHazeIdx(int enabled)
{
    InterfaceStates.video.cannonHeatHaze = enabled;
    if (enabled == 0) SetFormatedText("CANNONHEAT_TEXT", XI_ConvertString("VideoOff"));
    else SetFormatedText("CANNONHEAT_TEXT", XI_ConvertString("VideoOn"));
}

void ChangeCannonHeatHaze()
{
    ApplyCannonHeatHazeIdx(1 - sti(InterfaceStates.video.cannonHeatHaze));
}

void InitCannonHeatHazeOption()
{
    int enabled = 0;
    if (CheckAttribute(&Render, "cannonHeatHaze")) enabled = sti(Render.cannonHeatHaze);
    ApplyCannonHeatHazeIdx(enabled);
}

void SaveCannonHeatHazeOption()
{
    if (!CheckAttribute(&InterfaceStates, "video.cannonHeatHaze")) return;
    if (sti(InterfaceStates.video.cannonHeatHaze) != sti(Render.cannonHeatHaze))
        SetCannonHeatHaze(sti(InterfaceStates.video.cannonHeatHaze));
}
