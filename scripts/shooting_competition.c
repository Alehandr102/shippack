// Dialogue, wagers and inventory for the prison chief's bottle contest. No main-segment globals.
extern void SR_Start();
extern void SR_Stop();
extern void SR_Exit();
string Shooting_Text(string ru,string en) { if (LanguageGetLanguage() == "english") return en; return ru; }
bool Shooting_Active() { return CheckAttribute(GetMainCharacter(),"Shooting.active"); }
int Shooting_Rate(int level) { if (level == 0) return 1000; if (level == 1) return 3000; return 5000; }
float Shooting_Distance(int level) { return 8.0+3.0*level; }
int Shooting_Rounds(int level) { return 3*(level+1); }
bool Shooting_CanOffer(ref npc)
{
    int loc=FindLoadedLocation();
    if (loc < 0 || LAi_IsDead(npc) || !IsEntity(npc) || npc.location != pchar.location) return false;
    if (LAi_group_IsActivePlayerAlarm() || Shooting_Active() || CheckAttribute(pchar,"Shooting.pending")) return false;
    if (Locations[loc].type != "jail" || !CheckAttribute(Locations[loc],"parent_colony")) return false;
    if (npc.id != Locations[loc].parent_colony+"JailOff" || npc.location.group != "sit") return false;
    string seat=npc.location.locator;
    if (!CheckAttribute(Locations[loc],"locators.sit."+seat+".vz.x")) return false;
    int nation=GetCityNation(Locations[loc].parent_colony);
    if (GetNationRelation2MainCharacter(nation) == RELATION_ENEMY) return false;
    if (GetNationRelation(nation,GetBaseHeroNation()) == RELATION_ENEMY) return false;
    return Shooting_CanPlayToday();
}
float Shooting_WeaponAccuracy(ref chr)
{
    // Use the same quality tiers as weapon throwing, including generated items.
    string gun=GetCharacterEquipByGroup(chr,GUN_ITEM_TYPE);
    if (gun == "") return 1.0;
    int index=GetItemIndex(gun); if (index < 0) return 1.0;
    ref item=&Items[index];
    if (!CheckAttribute(item,"quality")) return 1.0;
    float tier=0.0;
    if (item.quality == "ordinary") tier=1.0;
    if (item.quality == "good") tier=2.0;
    if (item.quality == "excellent") tier=3.0;
    // Four evenly spaced steps on the skill scale: 1, 34, 67, 100.
    return 1.0+tier*(SKILL_MAX-1.0)/3.0;
}
float Shooting_Skill(ref chr)
{
    // Use the shooter's own effective skills, without a cannon officer or shared skill caches.
    float pistol=ApplayNavyPenaltyToSkill(chr,SKILL_PISTOL,GetCharacterSkillSimple(chr,SKILL_PISTOL));
    float fortune=ApplayNavyPenaltyToSkill(chr,SKILL_FORTUNE,GetCharacterSkillSimple(chr,SKILL_FORTUNE));
    float accuracy=ApplayNavyPenaltyToSkill(chr,SKILL_ACCURACY,GetCharacterSkillSimple(chr,SKILL_ACCURACY));
    float luck=GetCharacterSPECIAL(chr,SPECIAL_L);
    if (pistol < 1.0) pistol=1.0; if (pistol > SKILL_MAX) pistol=SKILL_MAX;
    if (fortune < 1.0) fortune=1.0; if (fortune > SKILL_MAX) fortune=SKILL_MAX;
    if (accuracy < 1.0) accuracy=1.0; if (accuracy > SKILL_MAX) accuracy=SKILL_MAX;
    if (luck < 1.0) luck=1.0; if (luck > SPECIAL_MAX) luck=SPECIAL_MAX;
    // Luck 1..10 maps to the same 1..100 scale as the three skills.
    luck=1.0+(luck-1.0)*(SKILL_MAX-1.0)/(SPECIAL_MAX-1.0);
    float skill=0.40*pistol+0.20*fortune+0.25*accuracy+0.15*luck;
    // Own skills supply two thirds; the equipped firearm's class supplies one third.
    skill=(2.0*skill+Shooting_WeaponAccuracy(chr))/3.0;
    if (skill < 1.0) skill=1.0; if (skill > 100.0) skill=100.0; return skill;
}
string Shooting_GunModel(ref chr)
{
    string gun=GetCharacterEquipByGroup(chr,GUN_ITEM_TYPE);
    if (gun != "") { ref item=ItemsFromID(gun); if (CheckAttribute(item,"model")) return item.model; }
    return "pistol1";
}
string Shooting_AmmoProblem(int rounds)
{
    if (GetCharacterEquipByGroup(pchar,GUN_ITEM_TYPE) == "") return Shooting_Text("Для состязания нужно огнестрельное оружие. Надень его и возвращайся.","You need an equipped firearm for this contest. Come back with one.");
    string bullet=LAi_GetCharacterBulletType(pchar);
    string powder=LAi_GetCharacterGunpowderType(pchar);
    bool missingAmmo=false;
    if (bullet == "") missingAmmo=true;
    else if (GetCharacterItem(pchar,bullet) < rounds) missingAmmo=true;
    if (powder != "")
    {
        if (GetCharacterItem(pchar,powder) < rounds) missingAmmo=true;
    }
    if (missingAmmo)
        return Shooting_Text("У тебя нет боеприпасов на всю серию. На каждый выстрел нужен выбранный боеприпас и, если требуется твоему оружию, порох. Приноси запас на ","You do not have ammunition for the whole round. Each shot needs the selected ammunition and powder if your weapon requires it. Bring supplies for ")+rounds+Shooting_Text(" выстрелов."," shots.");
    return "";
}
void Shooting_InitDay()
{
    // LEO: переносим дату старой серии один раз, до обновления дневного банка.
    if (!CheckAttribute(pchar,"Shooting.cooldown.year"))
    {
        if (CheckAttribute(pchar,"Shooting.daily.games") && CheckAttribute(pchar,"Shooting.daily.day"))
        {
            if (sti(pchar.Shooting.daily.games) > 0)
            {
                int previous=sti(pchar.Shooting.daily.day);
                pchar.Shooting.cooldown.year=makeint(previous/10000);
                pchar.Shooting.cooldown.month=makeint(previous/100)%100;
                pchar.Shooting.cooldown.day=previous%100;
                pchar.Shooting.cooldown.time=0.0;
            }
        }
    }
    // A calendar key avoids collisions between the 31st and the next month's 1st.
    int day=GetDataYear()*10000+GetDataMonth()*100+GetDataDay();
    if (!CheckAttribute(pchar,"Shooting.daily.day") || sti(pchar.Shooting.daily.day) != day)
    {
        pchar.Shooting.daily.day=day;
        pchar.Shooting.daily.games=0;
        pchar.Shooting.daily.bank=2*Shooting_Rate(2);
    }
}
bool Shooting_CanPlayToday()
{
    Shooting_InitDay();
    return Shooting_CooldownHoursLeft() == 0;
}
int Shooting_CooldownHoursLeft()
{
    if (!CheckAttribute(pchar,"Shooting.cooldown.year")) return 0;
    if (!CheckAttribute(pchar,"Shooting.cooldown.month")) return 0;
    if (!CheckAttribute(pchar,"Shooting.cooldown.day")) return 0;
    if (!CheckAttribute(pchar,"Shooting.cooldown.time")) return 0;
    int elapsed=GetPastTime("hour",sti(pchar.Shooting.cooldown.year),sti(pchar.Shooting.cooldown.month),sti(pchar.Shooting.cooldown.day),stf(pchar.Shooting.cooldown.time),GetDataYear(),GetDataMonth(),GetDataDay(),GetTime());
    if (elapsed >= 72) return 0;
    return 72-elapsed;
}
void Shooting_RecordGame()
{
    Shooting_InitDay();
    pchar.Shooting.daily.games=sti(pchar.Shooting.daily.games)+1;
    pchar.Shooting.cooldown.year=GetDataYear();
    pchar.Shooting.cooldown.month=GetDataMonth();
    pchar.Shooting.cooldown.day=GetDataDay();
    pchar.Shooting.cooldown.time=GetTime();
}
string Shooting_DailyLimitText()
{
    Shooting_InitDay();
    return Shooting_Text("Между состязаниями выдерживаем три дня. До следующей серии осталось часов: ","We allow three days between contests. Hours until the next round: ")+Shooting_CooldownHoursLeft()+".";
}
bool Shooting_SetPlace(ref scene,ref npc,int level)
{
    int loc=FindLoadedLocation(); if (loc < 0) return false;
    if (!CheckAttribute(Locations[loc],"locators.reload.reload11.x")) return false;
    aref locator; makearef(locator,Locations[loc].locators.reload.reload11);
    scene.table=true; scene.distance=Shooting_Distance(level);
    scene.playerIndex=GetMainCharacterIndex(); scene.npcIndex=sti(npc.index);
    // Use the existing stocks at the end of the open hall, facing away from the cell gates.
    scene.existingSupport=true;
    scene.tableX=stf(locator.x)+1.4654; scene.tableY=stf(locator.y)-0.0331066; scene.tableZ=stf(locator.z)-8.61796;
    scene.laneX=0.01235; scene.laneZ=1.0;
    scene.supportHeight=1.546317;
    // The worn top plank is slightly uneven; each bottle rests at its own height.
    scene.bottleOffset0=-0.0601441; scene.bottleOffset1=0.0; scene.bottleOffset2=-0.0210392;
    return true;
}
bool Shooting_CheckPlace(ref npc,int level)
{
    int loc=FindLoadedLocation(); if (loc < 0) return false;
    object probe; if (!Shooting_SetPlace(&probe,npc,level)) return false;
    if (!CreateEntity(&probe,"BottleShooting")) return false;
    float x,y,z,ay; GetCharacterPos(pchar,&x,&y,&z); GetCharacterAy(pchar,&ay);
    bool available=SendMessage(&probe,"liffff",57603,&Locations[loc],x,y,z,ay);
    DeleteClass(&probe); return available;
}
bool Shooting_Dialog(ref npc,aref link)
{
    string node=Dialog.CurrentNode;
    if (node == "Shooting_Offer")
    {
        if (!Shooting_CanPlayToday()) { dialog.text=Shooting_DailyLimitText(); link.l1=Shooting_Text("Увидимся позже.","Until next time."); link.l1.go="Shooting_Exit"; return true; }
        if (!Shooting_CanOffer(npc)) { dialog.text=Shooting_Text("Сейчас не до состязаний.","This is no time for a contest."); link.l1=Shooting_Text("В другой раз.","Another time."); link.l1.go="Shooting_Exit"; return true; }
        Shooting_InitDay();
        dialog.text=Shooting_Text("Вот и славно! В конце длинного зала стоят деревянные колодки. Расставим бутылки на верхней перекладине и отойдём к противоположной стене. Стреляем по очереди. Оружие и боеприпасы у каждого свои. Выбирай, сколько поставим и на сколько шагов отойдём.","Splendid! There are wooden stocks at the end of the long hall. We will put bottles on the top plank and stand back toward the opposite wall. We take turns: ten points for each broken bottle. Bring your own weapon and ammunition. Choose the wager and how far we stand.");
        for (int level=0;level<3;level++)
        {
            string key="shoot"+level;
            link.(key)=""+Shooting_Distance(level)+Shooting_Text(" шагов, "," paces, ")+Shooting_Rounds(level)+Shooting_Text(" выстрелов. Ставка "," shots. Stake: ")+Shooting_Rate(level)+".";
            link.(key).go="Shooting_Bet"+level;
        }
        link.l9=Shooting_Text("В другой раз.","Another time."); link.l9.go="Shooting_Exit"; return true;
    }
    if (node == "Shooting_Bet0" || node == "Shooting_Bet1" || node == "Shooting_Bet2")
    {
        if (!Shooting_CanPlayToday()) { dialog.text=Shooting_DailyLimitText(); link.l1=Shooting_Text("Увидимся позже.","Until next time."); link.l1.go="Shooting_Exit"; return true; }
        int chosen=0; if (node == "Shooting_Bet1") chosen=1; if (node == "Shooting_Bet2") chosen=2;
        string problem="";
        if (problem == "" && !Shooting_CanOffer(npc)) problem=Shooting_Text("Сейчас состязание невозможно.","We cannot shoot right now.");
        if (problem == "") problem=Shooting_AmmoProblem(Shooting_Rounds(chosen));
        if (problem == "")
        {
            if (sti(pchar.money) < Shooting_Rate(chosen) || sti(pchar.Shooting.daily.bank) < Shooting_Rate(chosen))
                problem=Shooting_Text("Не хватает денег на ставку.","There is not enough money for this wager.");
        }
        if (problem == "" && !Shooting_CheckPlace(npc,chosen)) problem=Shooting_Text("На этой дистанции линия огня перекрыта. Попробуем отойти поближе или вернёмся позже.","The shooting lane is blocked at that distance. Let us try a shorter range or come back later.");
        if (problem != "")
        {
            dialog.text=problem; link.l1=Shooting_Text("Выберем другую дистанцию.","Let us choose another distance."); link.l1.go="Shooting_Offer";
            link.l2=Shooting_Text("В другой раз.","Another time."); link.l2.go="Shooting_Exit"; return true;
        }
        pchar.Shooting.pending.npc=npc.id; pchar.Shooting.pending.level=chosen; pchar.Shooting.pending.location=pchar.location;
        npc.Dialog.CurrentNode="First_officer"; npc.Dialog.TempNode="First_officer"; DialogExit(); AddDialogExitQuestFunction("Shooting_StartPending"); return true;
    }
    if (node == "Shooting_Result")
    {
        dialog.text=Shooting_Text("Серия завершена.","The round is over.");
        if (CheckAttribute(pchar,"Shooting.result"))
        {
            int outcome=sti(pchar.Shooting.result.outcome);
            if (outcome == 1) dialog.text=Shooting_Text("Отличная стрельба! Твоя взяла, забирай выигрыш.","Fine shooting! You win. Take your winnings.");
            if (outcome == 2) dialog.text=Shooting_Text("На этот раз я оказался метче. Ставка моя. Подучись меткости и приноси оружие получше.","This time I was the better shot. The stake is mine.");
            if (outcome == 3) dialog.text=Shooting_Text("Поровну. Каждый забирает свою ставку.","A draw. We each take our stake back.");
            if (outcome == 0) dialog.text=Shooting_Text("Не закончили. Ставки возвращены.","We did not finish. Both stakes are returned.");
            if (CheckAttribute(pchar,"Shooting.result.problem")) dialog.text=pchar.Shooting.result.problem;
            dialog.text+=" "+Shooting_Text("Твои очки: ","Your score: ")+pchar.Shooting.result.playerScore+Shooting_Text(". Мои очки: ",". My score: ")+pchar.Shooting.result.npcScore+".";
        }
        if (Shooting_CanPlayToday()) { link.l1=Shooting_Text("Ещё раз?","Another round?"); link.l1.go="Shooting_Offer"; }
        else dialog.text+=" "+Shooting_DailyLimitText();
        link.l2=Shooting_Text("До встречи.","Until next time."); link.l2.go="Shooting_Exit"; return true;
    }
    if (node == "Shooting_Exit") { npc.Dialog.CurrentNode="First_officer"; npc.Dialog.TempNode="First_officer"; DialogExit(); return true; }
    return false;
}
void Shooting_AddLink(ref npc,aref link)
{
    if (Dialog.CurrentNode == "First_officer" && Shooting_CanOffer(npc)) {
        dialog.text+=" "+Shooting_Text("Эх, засиделся я тут, только бумажки перекладываю. Ничего не происходит... Давай постреляем по бутылкам на деньги?","I have been sitting here shuffling papers far too long. Nothing ever happens... Fancy shooting bottles for money? We can put them on the stocks in the long hall, where there is room to stand back.");
        link.shooting=Shooting_Text("Давай. Посмотрим, кто метче!","Let us see who is the better shot!"); link.shooting.go="Shooting_Offer"; }
}
void Shooting_StartPending()
{
    if (!CheckAttribute(pchar,"Shooting.pending")) return;
    SetEventHandler("Shooting_Start","Shooting_Start",0); PostEvent("Shooting_Start",100);
}
void Shooting_Start()
{
    DelEventHandler("Shooting_Start","Shooting_Start");
    if (!CheckAttribute(pchar,"Shooting.pending") || Shooting_Active()) return;
    if (dialogRun || sti(InterfaceStates.Launched)) { SetEventHandler("Shooting_Start","Shooting_Start",0); PostEvent("Shooting_Start",100); return; }
    if (LoadSegment("scripts\shooting3d.c")) SR_Start();
    else { DeleteAttribute(pchar,"Shooting.pending"); Log_Info(Shooting_Text("Не удалось загрузить соревнование.","The contest could not be loaded.")); }
}
void Shooting_Settle(int outcome,int playerScore,int npcScore)
{
    if (!CheckAttribute(pchar,"Shooting.match.rate") || sti(pchar.Shooting.match.settled) != 0) return;
    Shooting_InitDay();
    string host=pchar.Shooting.match.npc; int rate=sti(pchar.Shooting.match.rate); pchar.Shooting.match.settled=true;
    if (outcome == 1) AddMoneyToCharacter(pchar,2*rate);
    if (outcome == 2) pchar.Shooting.daily.bank=sti(pchar.Shooting.daily.bank)+2*rate;
    if (outcome == 0 || outcome == 3) { AddMoneyToCharacter(pchar,rate); pchar.Shooting.daily.bank=sti(pchar.Shooting.daily.bank)+rate; }
    pchar.Shooting.result.npc=host; pchar.Shooting.result.location=pchar.Shooting.match.location;
    pchar.Shooting.result.outcome=outcome; pchar.Shooting.result.playerScore=playerScore; pchar.Shooting.result.npcScore=npcScore;
    if (outcome == 1)
    {
        Ach_EnsureMinigames();
        IncAndUnlock("AchPrisonShooting",1);
        AddCharacterExpToSkill(pchar,SKILL_PISTOL,50);
        AddCharacterExpToSkill(pchar,SKILL_ACCURACY,50);
    }
    if (outcome == 2 || outcome == 3)
    {
        if (CheckAttribute(pchar,"Shooting.match.completed"))
        {
            if (sti(pchar.Shooting.match.completed) != 0)
            {
                int level=0;
                if (CheckAttribute(pchar,"Shooting.match.level")) level=sti(pchar.Shooting.match.level);
                if (level < 0) level=0;
                if (level > 2) level=2;
                AddCharacterExpToSkill(pchar,SKILL_ACCURACY,4*(level+1));
            }
        }
    }
}
void Shooting_QueueResult() { SetEventHandler("Shooting_Result","Shooting_Result",0); PostEvent("Shooting_Result",350); }
void Shooting_Result()
{
    DelEventHandler("Shooting_Result","Shooting_Result");
    if (!CheckAttribute(pchar,"Shooting.result") || dialogRun || sti(InterfaceStates.Launched) || LAi_group_IsActivePlayerAlarm()) return;
    int index=GetCharacterIndex(pchar.Shooting.result.npc);
    if (index < 0 || LAi_IsDead(&Characters[index]) || !IsEntity(&Characters[index]) || pchar.location != pchar.Shooting.result.location) return;
    StartInstantDialogNoType(pchar.Shooting.result.npc,"Shooting_Result",Characters[index].dialog.filename);
}
void Shooting_ScheduleUnload() { SetEventHandler("Shooting_Unload","Shooting_Unload",0); PostEvent("Shooting_Unload",1); }
void Shooting_Unload()
{
    DelEventHandler("Shooting_Unload","Shooting_Unload");
    if (!Shooting_Active() && SegmentIsLoaded("scripts\shooting3d.c")) UnloadSegment("scripts\shooting3d.c");
}
void Shooting_OnLoad()
{
    Shooting_Settle(0,0,0);
    DeleteAttribute(pchar,"Shooting.pending"); DeleteAttribute(pchar,"Shooting.match"); DeleteAttribute(pchar,"Shooting.active"); DeleteAttribute(pchar,"Shooting.result");
    DelEventHandler("Shooting_Start","Shooting_Start"); DelEventHandler("Shooting_Result","Shooting_Result");
}
