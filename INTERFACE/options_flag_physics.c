// LEO: Черновик не входит в профиль; rigging.ini меняется только по общему ОК.
int iFlagPhysicsDraft = 0;

void ApplyFlagPhysicsIdx(int enabled)
{
    if (enabled != 0) enabled = 1;
    iFlagPhysicsDraft = enabled;
    if (enabled == 0) SetFormatedText("FLAGPHYSICS_TEXT", XI_ConvertString("VideoOff"));
    else SetFormatedText("FLAGPHYSICS_TEXT", XI_ConvertString("VideoOn"));
}

void ChangeFlagPhysics()
{
    ApplyFlagPhysicsIdx(1 - iFlagPhysicsDraft);
}

void InitFlagPhysicsOption()
{
    ApplyFlagPhysicsIdx(GetFlagPhysicsOption());
}

bool SaveFlagPhysicsOption()
{
    if (iFlagPhysicsDraft == GetFlagPhysicsOption()) return true;
    return SetFlagPhysicsOption(iFlagPhysicsDraft) != 0;
}
