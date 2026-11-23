// Match lifecycle helpers only: preserve main-segment variable indices in old saves.
extern void T3D_Start();
extern void T3D_Stop();
extern void T3D_Exit();
bool Throwing3D_IsActive()
{
    return CheckAttribute(GetMainCharacter(), "Throwing3DActive");
}
void Throwing3D_ScheduleUnload()
{
    SetEventHandler("Throwing3D_Unload", "Throwing3D_Unload", 0);
    PostEvent("Throwing3D_Unload", 1);
}
void Throwing3D_Unload()
{
    DelEventHandler("Throwing3D_Unload", "Throwing3D_Unload");
    if (!Throwing3D_IsActive() && SegmentIsLoaded("scripts\throwing3d.c")) UnloadSegment("scripts\throwing3d.c");
}
