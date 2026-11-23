// Functions only: persistent state is stored on pchar; old save variable indices stay intact.
extern void T3D_Start();
extern void T3D_Exit();
string Throwing_Text(string russian,string english)
{
    if (LanguageGetLanguage() == "english") return english;
    return russian;
}
bool Throwing_IsTavern(aref loc)
{
    if (!CheckAttribute(loc,"type") || loc.type != "tavern") return false;
    if (!CheckAttribute(loc,"habitues") || !CheckAttribute(loc,"fastreload")) return false;
    // Caiman is outside the league even after the player builds its settlement.
    if (loc.fastreload == "Caiman" || (CheckAttribute(loc,"id") && loc.id == "Caiman_tavern")) return false;
    // Only the nine tavern interiors verified with their GM collision and locators
    // enter the roster. An unsupported/custom room cannot block the league.
    if (!CheckAttribute(loc,"models.always.tavern")) return false;
    bool supported=false;
    for (int model=1;model<=9;model++)
    {
        if (loc.models.always.tavern == "Tavern0"+model) supported=true;
    }
    return supported && FindColony(loc.fastreload) >= 0;
}
bool Throwing_EligibleNPC(ref npc,string locationId)
{
    // Address attributes can survive despawning; only a live location entity can compete.
    if (!IsEntity(npc)) return false;
    if (!CheckAttribute(npc,"location") || npc.location != locationId || LAi_IsDead(npc)) return false;
    if (!CheckAttribute(npc,"dialog.filename") || npc.dialog.filename != "Habitue_dialog.c") return false;
    if (findsubstr(npc.id,"GenChar_",0) != 0) return false;
    if (!CheckAttribute(npc,"chr_ai.type") || npc.chr_ai.type != "sit") return false;
    if (CheckAttribute(npc,"CasinoQuest")) return false;
    // Our own offer marker must not disqualify the assigned competitor.
    if (CheckAttribute(npc,"quest.questflag"))
    {
        if (!CheckAttribute(npc,"ThrowingQuestMark") || !CheckAttribute(npc,"quest.questflag.model")) return false;
        if (npc.quest.questflag.model != "exclamationmarkY") return false;
    }
    return true;
}
void Throwing_ClearQuestMark(ref npc)
{
    if (!CheckAttribute(npc,"ThrowingQuestMark")) return;
    // Never erase a marker installed by another quest after ours.
    if (CheckAttribute(npc,"quest.questflag.model") && npc.quest.questflag.model == "exclamationmarkY") UnmarkCharacter(npc);
    DeleteAttribute(npc,"ThrowingQuestMark");
}
void Throwing_AssignCompetitor(aref loc)
{
    bool allowed=Throwing_IsTavern(loc) && !LAi_IsCapturedLocation;
    for (int marked=0;marked<FANTOM_CHARACTERS;marked++)
    {
        ref actor=&Characters[marked];
        if (!CheckAttribute(actor,"location") || actor.location != loc.id) continue;
        if (!allowed || !Throwing_EligibleNPC(actor,loc.id)) Throwing_ClearQuestMark(actor);
    }
    if (!allowed) return;
    int selected=-1;
    ref npc;
    for (int i=0;i<FANTOM_CHARACTERS;i++)
    {
        npc=&Characters[i];
        if (!Throwing_EligibleNPC(npc,loc.id)) continue;
        if (selected < 0) selected=i;
        if (CheckAttribute(npc,"ThrowingCompetitor") && npc.ThrowingCompetitor == loc.id) { selected=i; break; }
    }
    if (selected < 0) return;
    string key=loc.id;
    if (!CheckAttribute(pchar,"Throwing.profiles."+key))
    {
        pchar.Throwing.profiles.(key).Accuracy=25+rand(55);
        pchar.Throwing.profiles.(key).Fortune=20+rand(60);
        pchar.Throwing.profiles.(key).FencingLight=25+rand(60);
        pchar.Throwing.profiles.(key).Fencing=25+rand(60);
        pchar.Throwing.profiles.(key).FencingHeavy=25+rand(60);
        pchar.Throwing.profiles.(key).Luck=3+rand(6);
        pchar.Throwing.profiles.(key).bank=8000+rand(40)*100;
    }
    npc=&Characters[selected];
    if (!CheckAttribute(npc,"ThrowingCompetitor") || npc.ThrowingCompetitor != key)
    {
        aref profile; makearef(profile,pchar.Throwing.profiles.(key));
        npc.skill.Accuracy=profile.Accuracy; npc.skill.Fortune=profile.Fortune;
        npc.skill.FencingLight=profile.FencingLight; npc.skill.Fencing=profile.Fencing; npc.skill.FencingHeavy=profile.FencingHeavy;
        npc.SPECIAL.Luck=profile.Luck;
        DeleteAttribute(npc,"BakSkill"); DeleteAttribute(npc,"BakSkillCount");
    }
    for (int j=0;j<FANTOM_CHARACTERS;j++)
    {
        ref other=&Characters[j];
        if (j != selected && CheckAttribute(other,"location") && other.location == key)
        {
            Throwing_ClearQuestMark(other);
            DeleteAttribute(other,"ThrowingCompetitor");
        }
    }
    npc.ThrowingCompetitor=key;
    Throwing_ClearQuestMark(npc);
    SetQuestMarkToCharacter(npc,true);
    if (CheckAttribute(npc,"quest.questflag.model") && npc.quest.questflag.model == "exclamationmarkY") npc.ThrowingQuestMark=true;
}
bool Throwing_IsCompetitor(ref npc)
{
    int index=FindLoadedLocation();
    if (index < 0 || !Throwing_IsTavern(&Locations[index])) return false;
    if (!CheckAttribute(npc,"ThrowingCompetitor") || npc.ThrowingCompetitor != Locations[index].id) return false;
    return Throwing_EligibleNPC(npc,Locations[index].id);
}
string Throwing_WeaponSkill(int weapon)
{
    if (weapon == 1) return SKILL_FENCING;
    if (weapon == 2) return SKILL_F_HEAVY;
    return SKILL_F_LIGHT;
}
int Throwing_SelectWeapon(ref actor,bool strongest)
{
    // Equal skills keep the first type: light, then medium, then heavy.
    int selected=0;
    int best=GetCharacterSkill(actor,Throwing_WeaponSkill(0));
    for (int weapon=1;weapon<3;weapon++)
    {
        int value=GetCharacterSkill(actor,Throwing_WeaponSkill(weapon));
        if ((strongest && value > best) || (!strongest && value < best))
        { selected=weapon; best=value; }
    }
    return selected;
}
string Throwing_WeaponName(int weapon)
{
    if (weapon == 1) return Throwing_Text("сабля","sabre");
    if (weapon == 2) return Throwing_Text("топор","axe");
    return Throwing_Text("кинжал","dagger");
}
string Throwing_DefaultModel(int weapon)
{
    string itemId="blade5";
    if (weapon == 1) itemId="blade03";
    if (weapon == 2) itemId="topor1";
    ref item=ItemsFromID(itemId);
    return item.model;
}
string Throwing_EquippedItem(ref actor,int weapon)
{
    string equipped=GetCharacterEquipByGroup(actor,BLADE_ITEM_TYPE);
    if (equipped == "") return "";
    int index=GetItemIndex(equipped);
    if (index < 0) return "";
    ref item=&Items[index];
    if (!CheckAttribute(item,"FencingType") || item.FencingType != Throwing_WeaponSkill(weapon)) return "";
    if (!CheckAttribute(item,"model") || item.model == "") return "";
    return equipped;
}
string Throwing_WeaponModel(ref actor,int weapon)
{
    string equipped=Throwing_EquippedItem(actor,weapon);
    if (equipped == "") return Throwing_DefaultModel(weapon);
    ref item=&Items[GetItemIndex(equipped)];
    return item.model;
}
float Throwing_WeaponWeight(ref actor,int weapon)
{
    string id=Throwing_EquippedItem(actor,weapon);
    if (id == "")
    {
        id="blade5";
        if (weapon == 1) id="blade03";
        if (weapon == 2) id="topor1";
    }
    ref item=ItemsFromID(id);
    if (CheckAttribute(item,"Weight") && stf(item.Weight)>0.0) return stf(item.Weight);
    if (CheckAttribute(item,"Generation.Weight.min") && CheckAttribute(item,"Generation.Weight.max"))
        return (stf(item.Generation.Weight.min)+stf(item.Generation.Weight.max))*0.5;
    if (weapon == 1) return 5.25;
    if (weapon == 2) return 10.0;
    return 0.9;
}
float Throwing_WeaponBonus(ref actor,int weapon)
{
    // Quality is the game's rarity tier; generated IDs retain their base item's tier.
    string equipped=Throwing_EquippedItem(actor,weapon);
    if (equipped == "") return 0.0;
    ref item=&Items[GetItemIndex(equipped)];
    if (!CheckAttribute(item,"quality")) return 0.0;
    if (item.quality == "ordinary") return 2.0;
    if (item.quality == "good") return 4.0;
    if (item.quality == "excellent") return 6.0;
    return 0.0;
}
void Throwing_PrepareWeapons(ref npc)
{
    pchar.Throwing.selection.weapon=0;
    pchar.Throwing.selection.npcWeapon=0;
}
float Throwing_GetSkillProfile(ref actor,int weapon)
{
    string skill=Throwing_WeaponSkill(weapon);
    float luck=GetCharacterSPECIAL(actor,SPECIAL_L);
    if (luck < 1.0) luck=1.0;
    if (luck > 10.0) luck=10.0;
    float fortune=0.70*GetCharacterSkill(actor,SKILL_FORTUNE)+0.30*(1.0+(luck-1.0)*11.0);
    float value=0.50*GetCharacterSkill(actor,SKILL_ACCURACY)+0.35*GetCharacterSkill(actor,skill)+0.15*fortune;
    if (value < 1.0) return 1.0;
    if (value > 100.0) return 100.0;
    return value;
}
int Throwing_Rate(int level)
{
    if (level == 0) return 200;
    if (level == 1) return 500;
    if (level == 2) return 1000;
    return 0;
}
float Throwing_Distance(int level) { return 2.0+level; }
float Throwing_GetPowerLimit(ref actor)
{
    float strength=GetCharacterSPECIAL(actor,SPECIAL_S);
    if (strength < 1.0) strength=1.0;
    if (strength > 10.0) strength=10.0;
    return 0.1+0.05*strength;
}


// LEO: одна проверка для допуска и диагностического трейса отказа запуска.
string Throwing_StartBlockReason()
{
    if (Throwing3D_IsActive()) return "match_active";
    if (bSeaActive || IsEntity(&worldMap)) return "not_on_land";
    if (dialogRun) return "dialog";
    if (sti(InterfaceStates.Launched) || g_ibVideoExecuting) return "interface_or_video";
    if (questMovieProcess > 0) return "cutscene";
    if (LocCamera_GetCurState() >= 0) return "scripted_camera";
    if (LAi_IsDead(pchar) || LAi_IsFightMode(pchar) || LAi_grp_alarmactive) return "combat_or_dead";
    if (CheckAttribute(pchar,"LandTacOn")) return "tactical_pause";
    if (chrDisableReloadToLocation || bDisableCharacterMenu) return "world_locked";
    aref fader; if (GetEntity(fader,"fader")) return "fader";
    int index=FindLoadedLocation();
    if (index < 0) return "location_unloaded";
    if (!IsEntity(&Locations[index]) || !IsEntity(&locCamera)) return "scene_unloaded";
    if (!CheckAttribute(Locations[index],"type")) return "not_a_tavern";
    if (Locations[index].type != "tavern") return "not_a_tavern";
    return "";
}
bool Throwing_CanStart()
{
    return Throwing_StartBlockReason() == "";
}
void Throwing_CheckDistances(ref npc)
{
    DeleteAttribute(npc,"ThrowingAvailable");
    if (!Throwing_IsCompetitor(npc)) return;
    object probe;
    if (!CreateEntity(&probe,"TavernThrowing")) return;
    float x,y,z,facing;
    facing=0.0;
    int index=FindLoadedLocation();
    if (GetCharacterPos(pchar,&x,&y,&z))
    {
        GetCharacterAy(pchar,&facing);
        for (int level=0;level<3;level++)
        {
            string key="l"+level;
            // SendMessage is VAR_PTR; attributes require a string-compatible integer.
            int available=SendMessage(&probe,"lifffff",57503,&Locations[index],x,y,z,facing,Throwing_Distance(level));
            npc.ThrowingAvailable.(key)=available;
        }
    }
    DeleteClass(&probe);
}
bool Throwing_CanBet(ref npc,int level)
{
    if (!Throwing_IsCompetitor(npc) || Throwing_Rate(level) <= 0) return false;
    string levelKey="l"+level;
    string allowed="ThrowingAvailable."+levelKey;
    if (!CheckAttribute(npc,allowed) || sti(npc.ThrowingAvailable.(levelKey)) == 0) return false;
    string key=npc.ThrowingCompetitor;
    if (!CheckAttribute(pchar,"Throwing.profiles."+key+".bank")) return false;
    return sti(pchar.money) >= Throwing_Rate(level) && sti(pchar.Throwing.profiles.(key).bank) >= Throwing_Rate(level);
}
void Throwing_RequestMatch(ref npc,int level)
{
    if (!Throwing_CanBet(npc,level) || Throwing3D_IsActive() || CheckAttribute(pchar,"Throwing.match")) return;
    if (CheckAttribute(pchar,"Throwing.pending")) return;
    Throwing_PrepareWeapons(npc);
    pchar.Throwing.pending.npc=npc.id; pchar.Throwing.pending.location=npc.location;
    pchar.Throwing.pending.level=level; pchar.Throwing.pending.weapon=sti(pchar.Throwing.selection.weapon);
    pchar.Throwing.pending.npcWeapon=sti(pchar.Throwing.selection.npcWeapon);
    SetEventHandler("Throwing_StartMatch","Throwing_StartMatch",0);
    PostEvent("Throwing_StartMatch",100,"l",30);
}
void Throwing_StartMatch()
{
    int retries=GetEventData();
    DelEventHandler("Throwing_StartMatch","Throwing_StartMatch");
    if (!CheckAttribute(pchar,"Throwing.pending")) return;
    if (pchar.location != pchar.Throwing.pending.location)
    {
        DeleteAttribute(pchar,"Throwing.pending");
        Trace("LEO: [THROWING_START] cancelled: location changed");
        return;
    }
    // LEO: фейдер и камера могут освободиться позже dialogRun; ставка ещё не списана.
    if (dialogRun || !Throwing_CanStart())
    {
        if (retries > 0)
        {
            SetEventHandler("Throwing_StartMatch","Throwing_StartMatch",0);
            PostEvent("Throwing_StartMatch",100,"l",retries-1);
            return;
        }
        DeleteAttribute(pchar,"Throwing.pending");
        Trace("LEO: [THROWING_START] cancelled: "+Throwing_StartBlockReason()+" location="+pchar.location);
        Log_Info(Throwing_Text("Не удалось начать состязание: управление ещё занято. Ставка не списана, попробуйте снова.","The contest could not start because the controls are still busy. No stake was taken; please try again."));
        return;
    }
    int idx=GetCharacterIndex(pchar.Throwing.pending.npc);
    if (idx < 0) { DeleteAttribute(pchar,"Throwing.pending"); return; }
    if (!Throwing_IsCompetitor(&Characters[idx]) || !Throwing_CanBet(&Characters[idx],sti(pchar.Throwing.pending.level)))
    {
        DeleteAttribute(pchar,"Throwing.pending");
        Trace("LEO: [THROWING_START] cancelled: opponent or wager unavailable");
        Log_Info(Throwing_Text("Состязание сейчас недоступно. Ставка не списана.","The contest is currently unavailable. No stake was taken."));
        return;
    }
    if (!SegmentIsLoaded("scripts\throwing3d.c"))
    {
        if (!LoadSegment("scripts\throwing3d.c"))
        {
            DeleteAttribute(pchar,"Throwing.pending");
            Trace("LEO: [THROWING_START] cancelled: scene segment failed to load");
            return;
        }
    }
    T3D_Start();
    DeleteAttribute(pchar,"Throwing.pending");
}
bool Throwing_Reserve()
{
    if (!CheckAttribute(pchar,"Throwing.pending") || CheckAttribute(pchar,"Throwing.match")) return false;
    int idx=GetCharacterIndex(pchar.Throwing.pending.npc);
    if (idx < 0 || !Throwing_CanBet(&Characters[idx],sti(pchar.Throwing.pending.level))) return false;
    string key=pchar.Throwing.pending.location;
    int rate=Throwing_Rate(sti(pchar.Throwing.pending.level));
    if (!CheckAttribute(pchar,"Throwing.serial")) pchar.Throwing.serial=0;
    pchar.Throwing.serial=sti(pchar.Throwing.serial)+1;
    pchar.Throwing.match.id=pchar.Throwing.serial;
    pchar.Throwing.match.npc=pchar.Throwing.pending.npc; pchar.Throwing.match.location=key;
    pchar.Throwing.match.weapon=pchar.Throwing.pending.weapon; pchar.Throwing.match.npcWeapon=pchar.Throwing.pending.npcWeapon;
    pchar.Throwing.match.rate=rate;
    pchar.Throwing.match.level=pchar.Throwing.pending.level;
    pchar.Throwing.match.settled=false;
    pchar.Throwing.profiles.(key).bank=sti(pchar.Throwing.profiles.(key).bank)-rate;
    AddMoneyToCharacter(pchar,-rate);
    pchar.Throwing.match.escrow=true;
    return true;
}
string Throwing_TavernName(string id)
{
    int index=FindLocation(id);
    if (index >= 0 && CheckAttribute(Locations[index],"fastreload"))
    {
        string city=Locations[index].fastreload;
        string name=GetCityName(city);
        if (name != "") return name;
        return city;
    }
    return id;
}
void Throwing_PruneRoster()
{
    if (!CheckAttribute(pchar,"Throwing.league.roster")) return;
    aref roster; makearef(roster,pchar.Throwing.league.roster);
    for (int i=GetAttributesNum(roster)-1;i>=0;i--)
    {
        string key=GetAttributeName(GetAttributeN(roster,i));
        int index=FindLocation(key);
        bool keep=false;
        if (index >= 0) keep=Throwing_IsTavern(&Locations[index]);
        if (!keep) DeleteAttribute(roster,key);
    }
}
void Throwing_UpdateJournal()
{
    if (!CheckAttribute(pchar,"Throwing.league.roster") || !CheckAttribute(pchar,"QuestInfo.ThrowingLeague")) return;
    pchar.QuestInfo.ThrowingLeague.ImageListName="QUEST_ThrowingLeague";
    pchar.QuestInfo.ThrowingLeague.ImageName="quest_icons";
    Throwing_PruneRoster();
    string rows="";
    int wins=0;
    int total=0;
    aref roster; makearef(roster,pchar.Throwing.league.roster);
    for (int i=0;i<GetAttributesNum(roster);i++)
    {
        aref item=GetAttributeN(roster,i);
        bool won=sti(GetAttributeValue(item)) != 0;
        total++;
        if (won) wins++;
        // The script lexer keeps backslash escapes literally; use an actual line break.
        if (rows != "") rows+="
";
        rows+=Throwing_TavernName(GetAttributeName(item))+" — ";
        if (won) rows+=Throwing_Text("победа есть","won");
        else rows+=Throwing_Text("победы нет","not won");
    }
    pchar.Throwing.league.wins=wins;
    pchar.Throwing.league.total=total;
    // Keep the introduction and completion; merge old progress entries into one checklist.
    string intro="";
    string progress="";
    string finish="";
    if (CheckAttribute(pchar,"QuestInfo.ThrowingLeague.Text"))
    {
        aref records; makearef(records,pchar.QuestInfo.ThrowingLeague.Text);
        for (int j=0;j<GetAttributesNum(records);j++)
        {
            aref record=GetAttributeN(records,j);
            string date;
            string textId;
            string questId;
            string raw=GetAttributeValue(record);
            GetDateAndText(raw,&date,&textId,&questId);
            if (textId == "1" && intro == "") intro=raw;
            if (textId == "2") progress=raw;
            if (textId == "3") finish=raw;
        }
    }
    if (intro == "") intro="@ThrowingLeague@"+GetQuestBookData()+"@1";
    if (progress == "") progress="@ThrowingLeague@"+GetQuestBookData()+"@2";
    DeleteAttribute(pchar,"QuestInfo.ThrowingLeague.Text");
    pchar.QuestInfo.ThrowingLeague.Text="";
    pchar.QuestInfo.ThrowingLeague.Text.l0=intro;
    pchar.QuestInfo.ThrowingLeague.Text.l1=progress;
    pchar.QuestInfo.ThrowingLeague.Text.l1.UserData="@<wins>"+wins+"@<total>"+total+"@<taverns>"+rows;
    if (finish != "") pchar.QuestInfo.ThrowingLeague.Text.l2=finish;
}
void Throwing_CompleteLeague(string key)
{
    Ach_EnsureMinigames();
    if (sti(pchar.Throwing.league.total) > 0 && sti(pchar.Throwing.league.wins) >= sti(pchar.Throwing.league.total) && !CheckAttribute(pchar,"Throwing.league.rewarded"))
    {
        string reward=GetGeneratedItem("blade41"); // Red Storta Barbarigo, existing item and model.
        if (!GiveItem2Character(pchar,reward)) return;
        pchar.Throwing.league.rewardItem=reward;
        pchar.Throwing.league.rewarded=true;
        UnlockAchievement("AchThrowingChampion",3);
        pchar.Throwing.league.finalTavern=key;
        pchar.Throwing.league.pendingFinalStory=true;
        DeleteAttribute(pchar,"Throwing.league.pendingStory");
        AddCharacterExpToSkill(pchar,SKILL_ACCURACY,50);
        AddCharacterExpToSkill(pchar,SKILL_FORTUNE,10);
        AddQuestRecord("ThrowingLeague","3");
        CloseQuestHeader("ThrowingLeague");
        Log_Info(Throwing_Text("Поздравляем! Все таверны побеждены. Вы — чемпион метателей архипелага! Награда: Сторта Барбариго.","Congratulations! You have won in every tavern and become the archipelago's throwing champion! Reward: Storta Barbarigo."));
    }
}
void Throwing_RecordWin(string key,int weapon)
{
    int locationIndex=FindLocation(key);
    if (locationIndex < 0 || !Throwing_IsTavern(&Locations[locationIndex])) return;
    if (!CheckAttribute(pchar,"Throwing.league.started"))
    {
        pchar.Throwing.league.started=true; pchar.Throwing.league.wins=0; pchar.Throwing.league.total=0;
        for (int i=0;i<MAX_LOCATIONS;i++)
        {
            if (!Throwing_IsTavern(&Locations[i])) continue;
            string id=Locations[i].id;
            pchar.Throwing.league.roster.(id)=0;
            pchar.Throwing.league.total=sti(pchar.Throwing.league.total)+1;
        }
        SetQuestHeader("ThrowingLeague");
        pchar.QuestInfo.ThrowingLeague.ImageListName="QUEST_ThrowingLeague";
        pchar.QuestInfo.ThrowingLeague.ImageName="quest_icons";
        AddQuestRecord("ThrowingLeague","1");
        pchar.Throwing.league.pendingStory=true;
    }
    if (!CheckAttribute(pchar,"Throwing.league.roster."+key)) return;
    if (sti(pchar.Throwing.league.roster.(key)) != 0) { Throwing_CompleteLeague(key); return; }
    pchar.Throwing.league.roster.(key)=1;
    pchar.Throwing.league.wins=sti(pchar.Throwing.league.wins)+1;
    AddQuestRecord("ThrowingLeague","2");
    Throwing_UpdateJournal();
    Throwing_CompleteLeague(key);
}
// 0=technical cancellation/refund, 1=win, 2=loss/surrender, 3=tie.
void Throwing_Settle(int outcome,int heroScore,int npcScore)
{
    if (!CheckAttribute(pchar,"Throwing.match.escrow") || sti(pchar.Throwing.match.settled) != 0) return;
    string key=pchar.Throwing.match.location;
    int rate=sti(pchar.Throwing.match.rate);
    pchar.Throwing.match.settled=true;
    if (outcome == 1) AddMoneyToCharacter(pchar,2*rate);
    else
    {
        if (outcome == 2) pchar.Throwing.profiles.(key).bank=sti(pchar.Throwing.profiles.(key).bank)+2*rate;
        else
        {
            AddMoneyToCharacter(pchar,rate);
            pchar.Throwing.profiles.(key).bank=sti(pchar.Throwing.profiles.(key).bank)+rate;
        }
    }
    pchar.Throwing.result.npc=pchar.Throwing.match.npc; pchar.Throwing.result.location=key;
    pchar.Throwing.result.outcome=outcome; pchar.Throwing.result.heroScore=heroScore; pchar.Throwing.result.npcScore=npcScore;
    // LEO: отмена и сдача не дают опыта; расчёт защищён match.settled выше.
    int experience=0;
    int level=0;
    if (CheckAttribute(pchar,"Throwing.match.level")) level=sti(pchar.Throwing.match.level);
    if (level < 0) level=0;
    if (level > 2) level=2;
    if (outcome == 1) experience=10*(level+1);
    if (outcome == 2 || outcome == 3)
    {
        if (CheckAttribute(pchar,"Throwing.match.completed"))
        {
            if (sti(pchar.Throwing.match.completed) != 0) experience=4*(level+1);
        }
    }
    if (experience > 0) AddCharacterExpToSkill(pchar,SKILL_ACCURACY,experience);
    if (outcome == 1)
    {
        Throwing_RecordWin(key,sti(pchar.Throwing.match.weapon));
        Log_Info(Throwing_Text("Вы выиграли соревнование по метанию.","You won the throwing match."));
    }
    if (outcome == 2) Log_Info(Throwing_Text("Соревнование проиграно. Ставку забрал соперник.","Match lost. Your opponent takes the stake."));
    if (outcome == 3) Log_Info(Throwing_Text("Ничья. Ставки возвращены.","Draw. Both stakes have been returned."));
    if (outcome == 0) Log_Info(Throwing_Text("Метание прервано. Ставки возвращены.","Throwing cancelled. Both stakes have been returned."));
}
void Throwing_QueueResult()
{
    if (!CheckAttribute(pchar,"Throwing.result")) return;
    SetEventHandler("Throwing_ResultDialog","Throwing_ResultDialog",0);
    PostEvent("Throwing_ResultDialog",350);
}
void Throwing_ResultDialog()
{
    DelEventHandler("Throwing_ResultDialog","Throwing_ResultDialog");
    if (!CheckAttribute(pchar,"Throwing.result")) return;
    if (!Throwing_CanStart() || pchar.location != pchar.Throwing.result.location) return;
    int idx=GetCharacterIndex(pchar.Throwing.result.npc);
    if (idx < 0 || !Throwing_IsCompetitor(&Characters[idx])) return;
    StartInstantDialogNoType(pchar.Throwing.result.npc,"Throwing_Result","Habitue_dialog.c");
}
string Throwing_ChampionText()
{
    return Throwing_Text("Вот это метатель! Ты обошёл все таверны архипелага и победил каждого из нас. Последняя победа — твоя, а звание чемпиона заслужено по праву. Молодец, капитан! Прими Сторту Барбариго — редкий клинок, достойный такого мастера. Пусть служит тебе верой и правдой!","What a thrower! You visited every tavern in the archipelago and beat every one of us. The final victory is yours, and you have earned the champion's title. Well done, captain! Accept this Storta Barbarigo, a rare blade worthy of such a master. May it serve you well!");
}
bool Throwing_FinalStoryReady(ref npc)
{
    if (!CheckAttribute(pchar,"Throwing.league.pendingFinalStory") || !CheckAttribute(pchar,"Throwing.league.finalTavern")) return false;
    return pchar.Throwing.league.finalTavern == "" || npc.location == pchar.Throwing.league.finalTavern;
}
string Throwing_ResultText(ref npc)
{
    string text=Throwing_Text("Серия завершена.","The round is finished.");
    if (CheckAttribute(pchar,"Throwing.result.outcome"))
    {
        int outcome=sti(pchar.Throwing.result.outcome);
        if (outcome == 1) text=Throwing_Text("Ты победил! Забирай выигрыш.","You won! Take your winnings.");
        if (outcome == 2) text=Throwing_Text("На этот раз ставка моя.","This time the stake is mine.");
        if (outcome == 3) text=Throwing_Text("Поровну. Каждый забирает свою ставку.","A draw. We each take our stake back.");
        if (outcome == 0) text=Throwing_Text("Не закончили. Деньги возвращены.","We did not finish. The money has been returned.");
    }
    if (Throwing_FinalStoryReady(npc))
    {
        text+=" "+Throwing_ChampionText();
        DeleteAttribute(pchar,"Throwing.league.pendingFinalStory");
    }
    if (CheckAttribute(pchar,"Throwing.league.pendingStory"))
    {
        text+=" "+Throwing_Text("А знаешь, по всему архипелагу в тавернах собираются такие же метатели. Победи каждого из них — и станешь чемпионом. Первый соперник уже позади! Остальные места отмечены в твоём журнале.","Throwers meet in taverns throughout the archipelago. Beat every one of them and you will be the champion. You have already beaten the first! The remaining taverns are listed in your journal.");
        DeleteAttribute(pchar,"Throwing.league.pendingStory");
    }
    return text;
}
void Throwing_OnLoad()
{
    // An interrupted escrow from a forced save is refunded once, without resuming a native entity.
    Throwing_Settle(0,0,0);
    DeleteAttribute(pchar,"Throwing.match"); DeleteAttribute(pchar,"Throwing.pending");
    DeleteAttribute(pchar,"Throwing3DActive");
    Throwing_UpdateJournal();
    // Removing an excluded tavern may finish an old league. Issue its reward once.
    if (CheckAttribute(pchar,"Throwing.league.roster") && CheckAttribute(pchar,"QuestInfo.ThrowingLeague"))
    {
        if (sti(pchar.Throwing.league.total) == 0) { CloseQuestHeader("ThrowingLeague"); return; }
        string finalTavern="";
        int index=-1;
        if (CheckAttribute(pchar,"location")) index=FindLocation(pchar.location);
        if (index >= 0 && Throwing_IsTavern(&Locations[index])) finalTavern=pchar.location;
        Throwing_CompleteLeague(finalTavern);
    }
}

void Throwing_AddOffer(ref npc,aref link)
{
    if (!Throwing_IsCompetitor(npc) || chrDisableReloadToLocation || LAi_grp_alarmactive || Throwing3D_IsActive()) return;
    if (CheckAttribute(pchar,"Throwing.pending")) return;
    if (Throwing_FinalStoryReady(npc))
    {
        link.l13=Throwing_Text("Я победил во всех тавернах архипелага.","I have won in every tavern in the archipelago.");
        link.l13.go="Throwing_Champion";
    }
    Dialog.text+=" "+Throwing_Text("Хочешь проверить меткость? Можем пометать оружие в мишень на деньги.","Care to test your aim? We can throw weapons at the target for a wager.");
    link.l11=Throwing_Text("Посоревнуемся в метании?","Let us compete at throwing?");
    link.l11.go="Throwing_Rules";
    if (CheckAttribute(pchar,"Throwing.league.pendingStory"))
    {
        link.l12=Throwing_Text("Расскажи о соревновании метателей.","Tell me about the throwing league.");
        link.l12.go="Throwing_LeagueStory";
    }
}
void Throwing_ShowLevels(ref npc,aref link)
{
    Throwing_PrepareWeapons(npc);
    Throwing_CheckDistances(npc);
    Dialog.text=Throwing_Text("Выбирай расстояние и ставку. Чем дальше мишень, тем труднее попасть.","Choose the distance and stake. A more distant target is harder to hit.");
    Dialog.text+=" "+Throwing_Text("Тебе — ","You will throw a ")+Throwing_WeaponName(sti(pchar.Throwing.selection.weapon))+Throwing_Text(", мне — ","; I will throw a ")+Throwing_WeaponName(sti(pchar.Throwing.selection.npcWeapon))+".";
    int choice=0;
    for (int level=0;level<3;level++)
    {
        string label=makeint(Throwing_Distance(level))+Throwing_Text(" м, по "," m, ")+Throwing_Rate(level)+Throwing_Text(" монет."," coins each.");
        if (Throwing_CanBet(npc,level))
        {
            choice++;
            string key="l"+choice;
            link.(key)=label; link.(key).go="Throwing_Level"+level;
        }
        else
        {
            string levelKey="l"+level;
            if (!CheckAttribute(npc,"ThrowingAvailable."+levelKey) || sti(npc.ThrowingAvailable.(levelKey)) == 0)
                Dialog.text+=" "+label+Throwing_Text(" Для этой линии здесь не хватает свободного места."," There is not enough clear room for this line.");
            else if (sti(pchar.money) < Throwing_Rate(level))
                Dialog.text+=" "+label+Throwing_Text(" У тебя не хватает денег на ставку."," You cannot afford this stake.");
            else Dialog.text+=" "+label+Throwing_Text(" У меня не хватает денег на ставку."," I cannot afford this stake.");
        }
    }
    link.l4=Throwing_Text("В другой раз.","Another time."); link.l4.go="exit";
}
void Throwing_Confirm(ref npc,aref link,int level)
{
    if (!Throwing_CanBet(npc,level))
    {
        Dialog.text=Throwing_Text("Эту ставку сейчас сыграть не получится.","We cannot play for this stake right now.");
        link.l1=Throwing_Text("Выберем другую.","Let us choose another."); link.l1.go="Throwing_Levels"; return;
    }
    pchar.Throwing.selection.level=level;
    int throwsPerPlayer=3*(level+1);
    Dialog.text=makeint(Throwing_Distance(level))+Throwing_Text(" м. Каждый ставит "," m. We each stake ")+Throwing_Rate(level)+Throwing_Text(" монет. Бросков у каждого: "," coins. Throws each: ")+throwsPerPlayer+Throwing_Text(". После первого броска уход считается сдачей. Договорились?",". Leaving after your first throw counts as a surrender. Agreed?");
    link.l1=Throwing_Text("Договорились, начинаем.","Agreed, let us begin."); link.l1.go="Throwing_Begin";
    link.l2=Throwing_Text("Поменяем условия.","Let us change the terms."); link.l2.go="Throwing_Levels";
    link.l3=Throwing_Text("Я передумал.","I changed my mind."); link.l3.go="exit";
}
bool Throwing_ProcessDialog(ref npc,aref link,aref diag)
{
    if (findsubstr(Dialog.CurrentNode,"Throwing_",0) != 0) return false;
    if (!Throwing_IsCompetitor(npc)) { DialogExit(); return true; }
    switch (Dialog.CurrentNode)
    {
        case "Throwing_Rules":
            Dialog.text=Throwing_Text("На ближней линии у каждого три броска, на средней шесть, на дальней девять. Бросаем по очереди кинжал, саблю и топор. Больше очков - выигрыш твой. При ничьей возвращаем ставки. Дальняя линия дороже. Чем дольше замахиваешься, тем сильнее бросок. Наводись остриём и учитывай падение. До первого броска можно отказаться, потом уход считается сдачей.","At close range we each make three throws, at medium range six, and at long range nine. We take turns throwing a dagger, a sabre and an axe. The higher score wins. A draw returns both stakes. The distant line costs more. A longer wind-up makes a stronger throw. Aim with the tip and allow for the drop. You may cancel before your first throw; leaving afterwards counts as a surrender.");
            link.l1=Throwing_Text("Выберем расстояние и ставку.","Let us choose the distance and stake."); link.l1.go="Throwing_Levels";
            link.l2=Throwing_Text("Пока не хочу.","Not right now."); link.l2.go="exit";
        break;
        case "Throwing_Levels": Throwing_ShowLevels(npc,link); break;
        case "Throwing_Level0": Throwing_Confirm(npc,link,0); break;
        case "Throwing_Level1": Throwing_Confirm(npc,link,1); break;
        case "Throwing_Level2": Throwing_Confirm(npc,link,2); break;
        case "Throwing_Begin":
            if (CheckAttribute(pchar,"Throwing.selection.level")) Throwing_RequestMatch(npc,sti(pchar.Throwing.selection.level));
            diag.TempNode="First time"; diag.CurrentNode=diag.TempNode; DialogExit();
        break;
        case "Throwing_Result":
            Dialog.text=Throwing_ResultText(npc);
            diag.TempNode="First time";
            link.l1=Throwing_Text("Спасибо за игру.","Thanks for the game."); link.l1.go="exit";
        break;
        case "Throwing_Champion":
            if (Throwing_FinalStoryReady(npc))
            {
                Dialog.text=Throwing_ChampionText();
                DeleteAttribute(pchar,"Throwing.league.pendingFinalStory");
            }
            else Dialog.text=Throwing_Text("Твоё звание чемпиона никто не оспаривает, капитан.","No one disputes your champion's title, captain.");
            link.l1=Throwing_Text("Спасибо. Хорошее было соревнование!","Thank you. It was a fine competition!"); link.l1.go="exit";
        break;
        case "Throwing_LeagueStory":
            Dialog.text=Throwing_ResultText(npc);
            link.l1=Throwing_Text("Обойду таверны и проверю себя.","I will visit the taverns and test my skill."); link.l1.go="exit";
        break;
    }
    return true;
}
