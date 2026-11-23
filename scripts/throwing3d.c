// Tavern matches; scripts own escrow/quest, engine owns flight/score.
#define T3D_START_MESSAGE 57501
#define T3D_STOP_MESSAGE 57502
object t3dScene;
object t3dAlarmFader;
bool t3dOldReload, t3dOldMenu;
void T3D_Start()
{
    ref hero = GetMainCharacter();
    // Release entry point: only a pending wager from the tavern dialogue may start a match.
    if (!CheckAttribute(hero,"Throwing.pending")) return;
    int index = FindLoadedLocation();
    if (index < 0 || !Throwing_CanStart()) return;
    bool match=CheckAttribute(hero,"Throwing.pending");
    int opponent=-1;
    if (match)
    {
        opponent=GetCharacterIndex(hero.Throwing.pending.npc);
        if (opponent < 0 || !Throwing_CanBet(&Characters[opponent],sti(hero.Throwing.pending.level))) return;
    }
    float x, y, z;
    if (!GetCharacterPos(hero, &x, &y, &z)) { Throwing3D_ScheduleUnload(); return; }
    DeleteAttribute(&t3dScene, "");
    t3dScene.title = "Метание оружия";
    t3dScene.dagger = "Кинжал"; t3dScene.sabre = "Сабля"; t3dScene.axe = "Топор";
    t3dScene.scoreLabel = "Очки:";
    t3dScene.remainingLabel="Осталось бросков:";
    t3dScene.hitLabel = "Последний бросок:";
    t3dScene.finished = "Серия завершена.";
    t3dScene.playerTurn="Ваши броски"; t3dScene.opponentTurn="Броски соперника";
    t3dScene.playerLabel="Вы:"; t3dScene.opponentLabel="Соперник:";
    if (LanguageGetLanguage() == "english")
    {
        t3dScene.title = "Weapon throwing";
        t3dScene.dagger = "Dagger"; t3dScene.sabre = "Sabre"; t3dScene.axe = "Axe";
        t3dScene.scoreLabel = "Score:";
        t3dScene.remainingLabel="Throws remaining:";
        t3dScene.hitLabel = "Last throw:";
        t3dScene.finished = "Round finished.";
        t3dScene.playerTurn="Your throws"; t3dScene.opponentTurn="Opponent throws";
        t3dScene.playerLabel="You:"; t3dScene.opponentLabel="Opponent:";
    }
    t3dScene.playerIndex=GetMainCharacterIndex();
    t3dScene.match=match;
    if (match)
    {
        t3dScene.playerName=GetFullName(hero);
        t3dScene.npcName=GetFullName(&Characters[opponent]);
        t3dScene.playerPortrait="interfaces\PORTRAITS\face_"+hero.faceId+".tga";
        t3dScene.npcPortrait="interfaces\PORTRAITS\face_"+Characters[opponent].faceId+".tga";
        t3dScene.throwsPerPlayer=3*(sti(hero.Throwing.pending.level)+1);
        t3dScene.distance=Throwing_Distance(sti(hero.Throwing.pending.level));
        t3dScene.weapon=sti(hero.Throwing.pending.weapon);
        t3dScene.npcWeapon=sti(hero.Throwing.pending.npcWeapon);
        t3dScene.npcModel=Throwing_WeaponModel(&Characters[opponent],sti(hero.Throwing.pending.npcWeapon));
        t3dScene.npcFallback=Throwing_DefaultModel(sti(hero.Throwing.pending.npcWeapon));
        t3dScene.playerFallback=Throwing_DefaultModel(sti(hero.Throwing.pending.weapon));
        t3dScene.playerBonus=Throwing_WeaponBonus(hero,sti(hero.Throwing.pending.weapon));
        t3dScene.npcBonus=Throwing_WeaponBonus(&Characters[opponent],sti(hero.Throwing.pending.npcWeapon));
        t3dScene.playerPowerLimit=Throwing_GetPowerLimit(hero);
        t3dScene.npcPowerLimit=Throwing_GetPowerLimit(&Characters[opponent]);
        for (int type=0;type<3;type++)
        {
            string key="npcModel"+type; t3dScene.(key)=Throwing_WeaponModel(&Characters[opponent],type);
            key="fallback"+type; t3dScene.(key)=Throwing_DefaultModel(type);
            key="playerWeight"+type; t3dScene.(key)=Throwing_WeaponWeight(hero,type);
            key="npcWeight"+type; t3dScene.(key)=Throwing_WeaponWeight(&Characters[opponent],type);
            key="playerBonus"+type; t3dScene.(key)=Throwing_WeaponBonus(hero,type);
            key="npcBonus"+type; t3dScene.(key)=Throwing_WeaponBonus(&Characters[opponent],type);
        }
        t3dScene.seed=1+rand(1000000);
        t3dScene.npcSkill0=Throwing_GetSkillProfile(&Characters[opponent],0);
        t3dScene.npcSkill1=Throwing_GetSkillProfile(&Characters[opponent],1);
        t3dScene.npcSkill2=Throwing_GetSkillProfile(&Characters[opponent],2);
    }
    if (!CreateEntity(&t3dScene, "TavernThrowing"))
    {
        Log_Info("Метание: нужен новый engine.exe с классом TavernThrowing.");
        Throwing3D_ScheduleUnload();
        return;
    }
    float facing = 0.0;
    GetCharacterAy(hero, &facing);
    ref dagger = ItemsFromID("blade5");
    ref sabre = ItemsFromID("blade03");
    ref axe = ItemsFromID("topor1");
    string daggerModel=dagger.model;
    string sabreModel=sabre.model;
    string axeModel=axe.model;
    if (match)
    {
        daggerModel=Throwing_WeaponModel(hero,0);
        sabreModel=Throwing_WeaponModel(hero,1);
        axeModel=Throwing_WeaponModel(hero,2);
    }
    bool started = SendMessage(&t3dScene, "liffffsssfff", T3D_START_MESSAGE, &Locations[index], x, y, z, facing, daggerModel, sabreModel, axeModel, Throwing_GetSkillProfile(hero,0), Throwing_GetSkillProfile(hero,1), Throwing_GetSkillProfile(hero,2));
    if (!started)
    {
        DeleteClass(&t3dScene);
        Log_Info("Метание: не найдена свободная стена или не загружена модель. См. compile.log.");
        Throwing3D_ScheduleUnload();
        return;
    }
    if (match && !Throwing_Reserve())
    {
        SendMessage(&t3dScene,"l",T3D_STOP_MESSAGE); DeleteClass(&t3dScene);
        Throwing3D_ScheduleUnload(); return;
    }
    t3dOldReload = chrDisableReloadToLocation;
    t3dOldMenu = bDisableCharacterMenu;
    chrDisableReloadToLocation = true;
    bDisableCharacterMenu = true;
    hero.Throwing3DActive = true;
    SetLandInterfaceShow(false);
    LandRadar_ReconcileVisibility();
    procUpdateControlKeyGroup();
    SetEventHandler("frame", "T3D_Frame", 0);
    SetEventHandler("EventUnloadLocation", "T3D_Stop", 0);
    SetEventHandler("T3D_Launch", "T3D_LaunchSound", 0);
    SetEventHandler("T3D_Impact", "T3D_ImpactSound", 0);
}
void T3D_LaunchSound() {
    string soundFile;
    if(rand(1) == 0) soundFile = "People Fight\Player_Man_0" + (rand(2) + 1) + "_0" + (rand(2) + 1) + ".wav";
    else soundFile = "People Fight\Swish_Sword_0" + (rand(7) + 1) + ".wav";
    PlaySound(soundFile);
}
void T3D_ImpactSound() { PlaySound("PEOPLE\der-man1-04.wav"); }
void T3D_Frame()
{
    ref hero = GetMainCharacter();
    if (!IsEntity(&t3dScene) || !CheckAttribute(t3dScene, "active")) { T3D_Stop(); return; }
    if (CheckAttribute(t3dScene,"alarmFading")) return;
    if (CheckAttribute(t3dScene,"civilianHit"))
    {
        t3dScene.alarmFading=true;
        SetEventHandler("FaderEvent_EndFade","T3D_AlarmBlack",0);
        SetEventHandler("FaderEvent_EndFadeIn","T3D_AlarmVisible",0);
        CreateEntity(&t3dAlarmFader,"fader");
        if (!IsEntity(&t3dAlarmFader)) { T3D_AlarmBlack(); T3D_AlarmVisible(); return; }
        SendMessage(&t3dAlarmFader,"lfl",FADER_OUT,0.25,false);
        return;
    }
    if (LAi_IsDead(hero) || dialogRun || sti(InterfaceStates.Launched) || g_ibVideoExecuting || questMovieProcess > 0) { T3D_Stop(); return; }
    if (CheckAttribute(hero,"Throwing.match"))
    {
        int opponent=GetCharacterIndex(hero.Throwing.match.npc);
        if (opponent < 0 || !Throwing_IsCompetitor(&Characters[opponent])) { T3D_Stop(); return; }
        if (sti(t3dScene.completed) != 0)
        {
            hero.Throwing.match.completed=true;
            int result=3;
            if (sti(t3dScene.playerScore) > sti(t3dScene.npcScore)) result=1;
            if (sti(t3dScene.playerScore) < sti(t3dScene.npcScore)) result=2;
            Throwing_Settle(result,sti(t3dScene.playerScore),sti(t3dScene.npcScore));
            T3D_Stop(); Throwing_QueueResult(); return;
        }
    }
    if (sti(t3dScene.active) == 0) T3D_Stop();
}
void T3D_AlarmBlack()
{
    DelEventHandler("FaderEvent_EndFade","T3D_AlarmBlack");
    if (!Throwing3D_IsActive() || !IsEntity(&t3dScene)) return;
    ref hero=GetMainCharacter();
    int victim=sti(t3dScene.civilianHit);
    Throwing_Settle(2,sti(t3dScene.playerScore),sti(t3dScene.npcScore));
    DeleteAttribute(hero,"Throwing.result");
    // Restore the camera only after the screen is completely black.
    SendMessage(&t3dScene,"l",T3D_STOP_MESSAGE);
    int loc=FindLoadedLocation();
    if (loc >= 0) LAi_LocationFightDisable(&Locations[loc],false);
    for (int i=0;i<MAX_CHARACTERS;i++)
    {
        ref citizen=&Characters[i];
        if (i == GetMainCharacterIndex() || !CheckAttribute(citizen,"location") || citizen.location != hero.location || LAi_IsDead(citizen)) continue;
        if (!CheckAttribute(citizen,"chr_ai.group") || citizen.chr_ai.group == hero.chr_ai.group || !IsEntity(citizen)) continue;
        bool seated=(CheckAttribute(citizen,"chr_ai.type") && citizen.chr_ai.type == LAI_TYPE_SIT) || (CheckAttribute(citizen,"location.group") && citizen.location.group != "goto");
        float x,y,z;
        if (seated) GetCharacterPos(citizen,&x,&y,&z);
        LAi_SetWarriorTypeNoGroup(citizen);
        if (seated)
        {
            string locator=LAi_FindNearestFreeLocator("goto",x,y,z);
            if (locator != "") ChangeCharacterAddressGroup(citizen,citizen.location,"goto",locator);
        }
        LAi_group_Attack(hero,citizen);
        LAi_group_UpdateTargets(citizen);
    }
    if (victim >= 0 && victim < MAX_CHARACTERS && !LAi_IsDead(&Characters[victim])) LAi_ProcessAI_aux(hero,&Characters[victim]);
    LAi_SetFightMode(hero,true);
    Log_Info(Throwing_Text("Вы попали в человека! Таверна подняла тревогу.","You hit someone! The tavern is raising the alarm."));
    if (IsEntity(&t3dAlarmFader)) SendMessage(&t3dAlarmFader,"lfl",FADER_IN,0.25,false);
}
void T3D_AlarmVisible()
{
    DelEventHandler("FaderEvent_EndFadeIn","T3D_AlarmVisible");
    T3D_Stop();
}
void T3D_Exit()
{
    if (CheckAttribute(t3dScene,"alarmFading")) return;
    if (!Throwing3D_IsActive()) return;
    if (CheckAttribute(pchar,"Throwing.match") && IsEntity(&t3dScene))
    {
        int result=0;
        if (sti(t3dScene.completed) != 0)
        {
            pchar.Throwing.match.completed=true;
            result=3;
            if (sti(t3dScene.playerScore) > sti(t3dScene.npcScore)) result=1;
            if (sti(t3dScene.playerScore) < sti(t3dScene.npcScore)) result=2;
        }
        else if (sti(t3dScene.active) != 0 && sti(t3dScene.hasThrown) != 0) result=2;
        Throwing_Settle(result,sti(t3dScene.playerScore),sti(t3dScene.npcScore));
    }
    T3D_Stop(); Throwing_QueueResult();
}
void T3D_Stop()
{
    if (!Throwing3D_IsActive()) return;
    DelEventHandler("FaderEvent_EndFade","T3D_AlarmBlack");
    DelEventHandler("FaderEvent_EndFadeIn","T3D_AlarmVisible");
    if (IsEntity(&t3dAlarmFader)) DeleteClass(&t3dAlarmFader);
    Throwing_Settle(0,0,0);
    DeleteAttribute(pchar,"Throwing.match");
    DelEventHandler("frame", "T3D_Frame");
    DelEventHandler("EventUnloadLocation", "T3D_Stop");
    DelEventHandler("T3D_Launch", "T3D_LaunchSound");
    DelEventHandler("T3D_Impact", "T3D_ImpactSound");
    if (IsEntity(&t3dScene))
    {
        SendMessage(&t3dScene, "l", T3D_STOP_MESSAGE);
        DeleteClass(&t3dScene);
    }
    DeleteAttribute(GetMainCharacter(), "Throwing3DActive");
    chrDisableReloadToLocation = t3dOldReload;
    bDisableCharacterMenu = t3dOldMenu;
    if (bLandInterfaceStart && !dialogRun && !sti(InterfaceStates.Launched)) BLI_EnableShow();
    procUpdateControlKeyGroup();
    Throwing3D_ScheduleUnload();
}
