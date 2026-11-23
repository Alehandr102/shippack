
#include "Loc_ai\LAi_defines.c"
#include "Loc_ai\LAi_events.c"
#include "Loc_ai\LAi_utils.c"
#include "Loc_ai\LAi_login.c"
#include "Loc_ai\LAi_character.c"
#include "Loc_ai\LAi_templates.c"
#include "Loc_ai\LAi_types.c"
#include "Loc_ai\LAi_groups.c"
#include "Loc_ai\LAi_fightparams.c"
#include "Loc_ai\LAi_boarding.c"
#include "Loc_ai\LAi_monsters.c"
#include "Loc_ai\LAi_location.c"
#include "Loc_ai\LAi_equip.c"
#include "Loc_ai\LAi_sounds.c"
#include "Loc_ai\LAi_utilites.c"
#include "Loc_ai\LAi_zLoadUtils.c"


bool LAi_IsInitedAI = false;
bool LAi_IsCapturedLocation = false;
bool LAi_restoreStates = false;
bool LAi_IsBoarding = false;

bool LAi_IsTownInterior(ref loc)
{
	if (!CheckAttribute(loc, "fastreload") || loc.fastreload == "") return false;
	if (loc.type == "tavern" || loc.type == "shop" || loc.type == "shipyard") return true;
	if (loc.type == "residence" || loc.type == "church") return true;
	return loc.id.label == "PortOffice" || loc.id.label == "Usurer House" || loc.id.label == "Brothel";
}

string LAi_FindTavernPost(ref loc, string otherPost)
{
	if (!CheckAttribute(loc, "locators.goto") || !CheckAttribute(loc, "locators.reload")) return "";
	aref points, doors, other;
	makearef(points, loc.locators.goto);
	makearef(doors, loc.locators.reload);
	float otherX = 0.0;
	float otherZ = 0.0;
	if (otherPost != "")
	{
		makearef(other, points.(otherPost));
		otherX = stf(other.x);
		otherZ = stf(other.z);
	}
	string best = "";
	float bestClearance = 2.25;
	int pointCount = GetAttributesNum(points);
	int doorCount = GetAttributesNum(doors);
	for (int i = 0; i < pointCount; i++)
	{
		aref point = GetAttributeN(points, i);
		string name = GetAttributeName(point);
		float x = stf(point.x);
		float z = stf(point.z);
		float clearance = 10000.0;
		if (otherPost != "")
		{
			float dx = x - otherX;
			float dz = z - otherZ;
			if (dx*dx + dz*dz < 2.25) continue;
		}
		// LEO: Расстояние в XZ, поскольку локаторы дверей и лестниц могут лежать ниже пола.
		for (int j = 0; j < doorCount; j++)
		{
			aref door = GetAttributeN(doors, j);
			float doorX = x - stf(door.x);
			float doorZ = z - stf(door.z);
			float distance = doorX*doorX + doorZ*doorZ;
			if (distance < clearance) clearance = distance;
			if (clearance <= bestClearance) break;
		}
		if (clearance > bestClearance && LAi_CheckLocatorFree("goto", name))
		{
			best = name;
			bestClearance = clearance;
		}
	}
	return best;
}

void LAi_ClearOfficerPost(aref chr)
{
	if (!CheckAttribute(chr, "chr_ai.OfficerAtPost")) return;
	DeleteAttribute(chr, "chr_ai.OfficerAtPost");
	if (chr.chr_ai.type == LAI_TYPE_HUBERSTAY) LAi_SetOfficerType(chr);
	if (chr.chr_ai.type == LAI_TYPE_OFFICER) LAi_tmpl_SetFollow(chr, pchar, -1.0);
}

//Инициализация интелекта
void LocAi_Init(ref loc)
{
    //#20190512-01
    string sOfficerLoc;
    aref rLocRoot, rOfcLocRef;
    bool bCheck;
	LAi_numloginedcharacters = 0;
	if(LAi_group_Init() == false) Trace("Error: Characters group logic not inited!!!");
	SetEventHandler("CharacterUpdate", "LAi_CharacterUpdate", 1);
	SetEventHandler("CharactersStateUpdate", "LAi_Character_FrameUpdate", 1);
	SetEventHandler("Location_CharacterEndTask", "LAi_CharacterEndTask", 0);
	SetEventHandler("Location_CharacterTaskFailure", "LAi_CharacterTaskFailure", 0);
	SetEventHandler("Location_CharacterBusyPos", "LAi_CharacterBusyPos", 0);
	SetEventHandler("Location_CharacterFollowGo", "LAi_CharacterFollowGo", 0);
	SetEventHandler("Location_CharacterFollowStay", "LAi_CharacterFollowStay", 0);
	SetEventHandler("Location_CharacterFightGo", "LAi_CharacterFightGo", 0);
	SetEventHandler("Location_CharacterFightStay", "LAi_CharacterFightStay", 0);
	SetEventHandler("Location_CharacterAttack", "LAi_CharacterAttack", 0);
	SetEventHandler("Location_CharacterFire", "LAi_CharacterFire", 0);
	SetEventHandler("Location_CharacterAimFire", "LAi_CharacterAimFire", 0);
	SetEventHandler("Location_CharacterAimStats", "LAi_CharacterAimStats", 0);
	SetEventHandler("Location_CharacterIsFire", "LAi_CharacterIsFire", 0);
	SetEventHandler("Location_CharacterIsFight", "LAi_CharacterIsFight", 0);
	SetEventHandler("Location_CharacterEscapeSlide", "LAi_CharacterEscapeSlide", 0);
	SetEventHandler("Location_CharacterColThreshold", "LAi_CharacterColThreshold", 0);
	SetEventHandler("Location_Character_EndAction", "LAi_Character_EndAction", 0);
	SetEventHandler("Location_CharacterItemAction", "LAi_CharacterItemAction", 0);
	SetEventHandler("Location_IntegratedMusket", "LAi_IntegratedMusket", 0);
	LAi_IsInitedAI = true;
	LAi_IsCapturedLocation = IsLocationCaptured(loc.id);
	if(!actLoadFlag)
	{
		//Очистим фантомных персонажей
		for(int i = 0; i < MAX_CHARS_IN_LOC; i++)
		{
			DeleteAttribute(&Characters[LOC_FANTOM_CHARACTERS + i], "");
			Characters[LOC_FANTOM_CHARACTERS + i].index = LOC_FANTOM_CHARACTERS + i;
			Characters[LOC_FANTOM_CHARACTERS + i].id = "";
			Characters[LOC_FANTOM_CHARACTERS + i].location = "none";
			Characters[LOC_FANTOM_CHARACTERS + i].location.locator = "";
			Characters[LOC_FANTOM_CHARACTERS + i].location.group = "";
		}
	}
	//Выставим адреса офицерам главного персонажа
	bool isBoarding = false;
	if(CheckAttribute(loc, "boarding") == true)
	{
		if(loc.boarding == "true") isBoarding = true;
		if(loc.boarding == "fort") isBoarding = true; // boal 21.01.05 fix!!!
	}
	LAi_IsBoarding = isBoarding;
	if(!isBoarding)
	{
		int locIndex = 1;
		if(LAi_restoreStates)
		{
			LAi_SetCurHPMax(pchar);
			LAi_UseAtidoteBottle(pchar);
		}
		int idx;
		//проверяем таймер запрета генерации офицеров в локацию. eddy
		if (CheckAttribute(loc, "DisableOfficers.QtyDays") && GetNpcQuestPastDayParam(loc, "DisableOfficers") > sti(loc.DisableOfficers.QtyDays))
		{
			DeleteAttribute(loc, "DisableOfficers"); //если есть и истек срок - сносим флаг запрета
		}
		if (!CheckAttribute(loc, "DisableOfficers")) //eddy. офицерский флаг
		{
            bool bHasLocs = CheckAttribute(loc, "locators.officers");
            float posX;
		    float posY;
		    float posZ;
		    GetCharacterPos(pchar, &posX, &posY, &posZ);
			bool townInterior = LAi_IsTownInterior(loc);
			bool tavern = loc.type == "tavern" && townInterior;
			string firstPost = "";
			for(i = 1; i <=MAX_NUM_FIGHTERS; i++)
			{
				idx = GetOfficersIndex(pchar, i);
				if(idx < 0) continue;
				if(LAi_restoreStates)
				{
					LAi_SetCurHPMax(&Characters[idx]);
					LAi_UseAtidoteBottle(&Characters[idx]);
				}

				ref officer = &Characters[idx];
				LAi_ClearOfficerPost(officer);
				if (townInterior)
				{
					if (CheckAttribute(officer, "location.follower") || officer.chr_ai.type == LAI_TYPE_ACTOR) continue;
					if (locIndex > 2 || LAi_IsDead(officer))
					{
						DeleteAttribute(officer, "location");
						officer.location = "none";
						officer.location.group = "";
						officer.location.locator = "";
						continue;
					}
				}
				if (tavern)
				{
					// LEO: При входе шанс 50% для каждого. При загрузке сохраняем состав.
					bool enterTavern;
					if (actLoadFlag) enterTavern = officer.location == pchar.location;
					else enterTavern = rand(1) == 0;
					sOfficerLoc = "";
					if (enterTavern) sOfficerLoc = LAi_FindTavernPost(loc, firstPost);
					DeleteAttribute(officer, "location");
					officer.location = "none";
					officer.location.group = "";
					officer.location.locator = "";
					if (sOfficerLoc == "") continue;
					officer.location = pchar.location;
					officer.location.group = "goto";
					officer.location.locator = sOfficerLoc;
					officer.chr_ai.OfficerAtPost = true;
					LAi_tmpl_stay_InitTemplate(officer);
					if (firstPost == "") firstPost = sOfficerLoc;
					locIndex++;
					continue;
				}
				// LEO: Уже стоящие в локации офицеры из сохранения занимают места в лимите.
				if (officer.location == pchar.location) {locIndex++; continue;}

				DeleteAttribute(&Characters[idx], "location");
				int k = locIndex % 3 + 1;
				if(bHasLocs)
				{
                    Characters[idx].location = pchar.location;
                    Characters[idx].location.group = "officers";
                    sOfficerLoc = pchar.location.locator + "_" + k;
                    bCheck = SendMessage(loc, "ls", MSG_LOCATION_LOC_EXISTS, sOfficerLoc);
                    if(!bCheck) {
                        sOfficerLoc = LAi_FindNearestFreeLocator("officers", posX, posY, posZ);
                        if(sOfficerLoc == "") {
                            makearef(rLocRoot, loc.locators.officers);
                            k = GetAttributesNum(rLocRoot);
                            rOfcLocRef = GetAttributeN(rLocRoot, rand(k-1));
                            sOfficerLoc = GetAttributeName(rOfcLocRef);
                        }
                    }
                    Characters[idx].location.locator = sOfficerLoc;
                    locIndex++;
				}else{
					Characters[idx].location = "none";
					Characters[idx].location.group = "";
					Characters[idx].location.locator = "";
				}
			}
		}
		else
		{
		    for(i = 1; i <=MAX_NUM_FIGHTERS; i++)
			{
				idx = GetOfficersIndex(pchar, i);
				if(idx < 0) continue;

				LAi_ClearOfficerPost(&Characters[idx]);
				Characters[idx].location = "none";
				Characters[idx].location.group = "";
				Characters[idx].location.locator = "";
			}
		}
	}else{
		LAi_restoreStates = false;
	}
	LAi_IsCapturedLocation = IsLocationCaptured(loc.id);
}

void LocAi_Release()
{
	// LEO: Выход удаляет персонажа из списка, переставляя последнего на его место.
	for (int i = LAi_numloginedcharacters - 1; i >= 0; i--)
	{
		int idx = LAi_loginedcharacters[i];
		if(idx >= 0)
		{
			LAi_CharacterLogoff(&Characters[idx]);
			LAi_ClearOfficerPost(&Characters[idx]);
		}
	}
	LAi_group_Release();
	LAi_IsInitedAI = false;
	LAi_numloginedcharacters = 0;
	DelEventHandler("CharacterUpdate", "LAi_CharacterUpdate");
	DelEventHandler("CharactersStateUpdate", "LAi_Character_FrameUpdate");
	DelEventHandler("Location_CharacterEndTask", "LAi_CharacterEndTask");
	DelEventHandler("Location_CharacterTaskFailure", "LAi_CharacterTaskFailure");
	DelEventHandler("Location_CharacterBusyPos", "LAi_CharacterBusyPos");
	DelEventHandler("Location_CharacterFollowGo", "LAi_CharacterFollowGo");
	DelEventHandler("Location_CharacterFollowStay", "LAi_CharacterFollowStay");
	DelEventHandler("Location_CharacterFightGo", "LAi_CharacterFightGo");
	DelEventHandler("Location_CharacterFightStay", "LAi_CharacterFightStay");
	DelEventHandler("Location_CharacterAttack", "LAi_CharacterAttack");
	DelEventHandler("Location_CharacterFire", "LAi_CharacterFire");
	DelEventHandler("Location_CharacterAimFire", "LAi_CharacterAimFire");
	DelEventHandler("Location_CharacterAimStats", "LAi_CharacterAimStats");
	DelEventHandler("Location_CharacterIsFire", "LAi_CharacterIsFire");
	DelEventHandler("Location_CharacterIsFight", "LAi_CharacterIsFight");
	DelEventHandler("Location_CharacterEscapeSlide", "LAi_CharacterEscapeSlide");
	DelEventHandler("Location_CharacterColThreshold", "LAi_CharacterColThreshold");
	DelEventHandler("Location_Character_EndAction", "LAi_Character_EndAction");
	DelEventHandler("Location_CharacterItemAction", "LAi_CharacterItemAction");
	DelEventHandler("Location_IntegratedMusket", "LAi_IntegratedMusket");
	//Очистим фантомных персонажей
	for(i = 0; i < MAX_CHARS_IN_LOC; i++)
	{
		DeleteAttribute(&Characters[LOC_FANTOM_CHARACTERS + i], "");
		Characters[LOC_FANTOM_CHARACTERS + i].index = LOC_FANTOM_CHARACTERS + i;
		Characters[LOC_FANTOM_CHARACTERS + i].id = "";
		Characters[LOC_FANTOM_CHARACTERS + i].location = "none";
		Characters[LOC_FANTOM_CHARACTERS + i].location.locator = "";
		Characters[LOC_FANTOM_CHARACTERS + i].location.group = "";
		// убираем остатки захвата в городе boal
		LAi_LoginInCaptureTown(&Characters[LOC_FANTOM_CHARACTERS + i], false);
	}
}


void LocAi_PostInit(ref loc)
{
	if(!LAi_IsBoarding) LAi_CharacterPostLogin(loc);
	for(int i = 0; i < LAi_numloginedcharacters; i++)
	{
		int idx = LAi_loginedcharacters[i];
		if(idx >= 0)
		{
			LAi_PostLoginInit(&Characters[idx]);
			//#20210221-01
			DeleteAttribute(&Characters[idx], "isLogin");
		}
	}
	LAi_restoreStates = false;
}

//Сохранение информации
void LAi_SaveInfo()
{
	LAi_group_SaveInfo();
}

//Инициализация перед новой игрой
void LAi_NewGame()
{
	//Задержанные квесты
	DeleteAttribute(&Lai_questdelays, "");
	//Группы
	DeleteAttribute(&LAi_grp_relations, "");
	LAi_grp_playeralarm = 0;
	LAi_grp_alarmactive = false;
	//Абордаж
	boarding_location = -1;
	boarding_location_type = -1;
	boarding_enemy_crew = 0;
	boarding_player_crew = 0;
	boarding_officers = 0;
	boarding_player_crew_per_chr = 1.0;
	LAi_boarding_process = false;
	//Разное
	LAi_IsInitedAI = false;
	LAi_IsCapturedLocation = false;
	LAi_restoreStates = true;
	LAi_numloginedcharacters = 0;
	DeleteAttribute(&LAi_QuestFader, "");
	globalSGMode = false;
	//Ставим тип игрока главному персонажу
	LAi_SetPlayerType(pchar);
}
