// Runtime scene stays in its own segment so old saves keep main-global indices.
object srScene;
bool srOldReload,srOldMenu;
int srSoundSerial;
void SR_Start()
{
    if (!CheckAttribute(pchar,"Shooting.pending")) { Shooting_ScheduleUnload(); return; }
    int loc=FindLoadedLocation(); int opponent=GetCharacterIndex(pchar.Shooting.pending.npc);
    if (loc < 0 || opponent < 0 || !IsEntity(&Characters[opponent]) || pchar.location != pchar.Shooting.pending.location) { DeleteAttribute(pchar,"Shooting.pending"); Shooting_ScheduleUnload(); return; }
    int level=sti(pchar.Shooting.pending.level); int rate=Shooting_Rate(level); int rounds=Shooting_Rounds(level);
    string problem=Shooting_AmmoProblem(rounds); string host=Characters[opponent].id;
    if (problem == "" && !Shooting_CanPlayToday()) problem=Shooting_DailyLimitText();
    if (problem != "" || sti(pchar.money) < rate || sti(pchar.Shooting.daily.bank) < rate)
    { DeleteAttribute(pchar,"Shooting.pending"); if (problem != "") Log_Info(problem); Shooting_ScheduleUnload(); return; }
    DeleteAttribute(&srScene,"");
    if (!Shooting_SetPlace(&srScene,&Characters[opponent],level)) { DeleteAttribute(pchar,"Shooting.pending"); Shooting_ScheduleUnload(); return; }
    srScene.rounds=rounds;
    srScene.playerSkill=Shooting_Skill(pchar); srScene.npcSkill=Shooting_Skill(&Characters[opponent]); srScene.seed=1+rand(1000000);
    srScene.playerGun=Shooting_GunModel(pchar); srScene.npcGun=Shooting_GunModel(&Characters[opponent]);
    srScene.playerPortrait="interfaces\PORTRAITS\face_"+pchar.faceId+".tga";
    srScene.npcPortrait="interfaces\PORTRAITS\face_"+Characters[opponent].faceId+".tga";
    srScene.scoreLabel=Shooting_Text("Очки:","Score:"); srScene.remainingLabel=Shooting_Text("Осталось выстрелов:","Shots remaining:");
    float x,y,z,ay; GetCharacterPos(pchar,&x,&y,&z); GetCharacterAy(pchar,&ay);
    bool started=CreateEntity(&srScene,"BottleShooting");
    if (started) started=SendMessage(&srScene,"liffff",57601,&Locations[loc],x,y,z,ay);
    if (!started) { if (IsEntity(&srScene)) DeleteClass(&srScene); DeleteAttribute(pchar,"Shooting.pending"); Log_Info(Shooting_Text("Не удалось подготовить площадку для стрельбы.","Could not prepare a shooting lane.")); Shooting_ScheduleUnload(); return; }
    // Debit wagers only after native geometry and camera setup succeeded.
    // LEO: любая начатая серия запускает общий КД, даже если игрок затем сдастся.
    Shooting_RecordGame();
    pchar.Shooting.match.npc=host; pchar.Shooting.match.location=pchar.location; pchar.Shooting.match.rate=rate; pchar.Shooting.match.settled=false;
    pchar.Shooting.match.level=level;
    pchar.Shooting.match.bullet=LAi_GetCharacterBulletType(pchar); pchar.Shooting.match.powder=LAi_GetCharacterGunpowderType(pchar);
    AddMoneyToCharacter(pchar,-rate); pchar.Shooting.daily.bank=sti(pchar.Shooting.daily.bank)-rate;
    DeleteAttribute(pchar,"Shooting.pending");
    srOldReload=chrDisableReloadToLocation; srOldMenu=bDisableCharacterMenu; chrDisableReloadToLocation=true; bDisableCharacterMenu=true;
    pchar.Shooting.active=true; SetLandInterfaceShow(false); LandRadar_ReconcileVisibility(); procUpdateControlKeyGroup(); srSoundSerial=0;
    SetEventHandler("frame","SR_Frame",0); SetEventHandler("EventUnloadLocation","SR_Stop",0);
}
void SR_Frame()
{
    if (!IsEntity(&srScene)) { SR_Stop(); return; }
    if (LAi_IsDead(pchar) || dialogRun || sti(InterfaceStates.Launched) || g_ibVideoExecuting || questMovieProcess > 0) { SR_Stop(); return; }
    int opponent=GetCharacterIndex(pchar.Shooting.match.npc);
    if (opponent < 0 || LAi_IsDead(&Characters[opponent]) || !IsEntity(&Characters[opponent])) { SR_Stop(); return; }
    if (sti(srScene.shotRequested) != 0)
    {
        string bullet=pchar.Shooting.match.bullet; string powder=pchar.Shooting.match.powder;
        bool missingAmmo=false;
        if (GetCharacterItem(pchar,bullet) < 1) missingAmmo=true;
        if (powder != "")
        {
            if (GetCharacterItem(pchar,powder) < 1) missingAmmo=true;
        }
        if (missingAmmo)
        {
            Shooting_Settle(0,sti(srScene.playerScore),sti(srScene.npcScore));
            pchar.Shooting.result.problem=Shooting_Text("Патроны закончились. Состязание прервано, ставки возвращены.","You are out of ammunition. The contest is cancelled and the stakes are returned.");
            SR_Stop(); Shooting_QueueResult(); return;
        }
        RemoveItems(pchar,bullet,1); if (powder != "") RemoveItems(pchar,powder,1);
        SendMessage(&srScene,"l",57604);
    }
    if (CheckAttribute(srScene,"fireSerial") && sti(srScene.fireSerial) != srSoundSerial)
    {
        srSoundSerial=sti(srScene.fireSerial);
        PlaySound("Kopcapkz\SoundFX\pistol_shot_0"+rand(5)+".ogg");
        CreateParticleSystem("gunfire",stf(srScene.fireX),stf(srScene.fireY),stf(srScene.fireZ),0.0,0.0,1.0,1000);
        if (sti(srScene.lastHit) != 0) PlaySound("Sea Battles\tros_lopaetsa_002.wav");
        else PlaySound("interface\Ricochet_6.wav");
    }
    if (sti(srScene.completed) != 0)
    {
        pchar.Shooting.match.completed=true;
        int result=3; if (sti(srScene.playerScore) > sti(srScene.npcScore)) result=1; if (sti(srScene.playerScore) < sti(srScene.npcScore)) result=2;
        Shooting_Settle(result,sti(srScene.playerScore),sti(srScene.npcScore)); SR_Stop(); Shooting_QueueResult(); return;
    }
    if (sti(srScene.active) == 0) SR_Stop();
}
void SR_Exit()
{
    if (!Shooting_Active()) return;
    int result=0;
    if (IsEntity(&srScene))
    {
        if (sti(srScene.completed) != 0)
        {
            pchar.Shooting.match.completed=true;
            result=3;
            if (sti(srScene.playerScore) > sti(srScene.npcScore)) result=1;
            if (sti(srScene.playerScore) < sti(srScene.npcScore)) result=2;
        }
        else if (sti(srScene.shots) > 0) result=2;
    }
    Shooting_Settle(result,sti(srScene.playerScore),sti(srScene.npcScore)); SR_Stop(); Shooting_QueueResult();
}
void SR_Stop()
{
    if (!Shooting_Active()) return;
    Shooting_Settle(0,0,0); DeleteAttribute(pchar,"Shooting.match");
    DelEventHandler("frame","SR_Frame"); DelEventHandler("EventUnloadLocation","SR_Stop");
    if (IsEntity(&srScene)) { SendMessage(&srScene,"l",57602); DeleteClass(&srScene); }
    DeleteAttribute(pchar,"Shooting.active"); chrDisableReloadToLocation=srOldReload; bDisableCharacterMenu=srOldMenu;
    if (bLandInterfaceStart && !dialogRun && !sti(InterfaceStates.Launched)) BLI_EnableShow();
    procUpdateControlKeyGroup(); Shooting_ScheduleUnload();
}
