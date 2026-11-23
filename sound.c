#define MUSIC_CHANGE_TIME  3000
#define MUSIC_SILENCE_TIME 180000.0
#define SOUNDS_FADE_TIME   200

#event_handler ("LoadSceneSound", "LoadSceneSound")
#event_handler ("ReloadStaticSound", "ReloadStaticSound")

// LEO: Причина действует только внутри вызова; прямой нативный запрос не наследует старую причину.
string MusicTraceBegin(string reason, string alias)
{
	if (!CheckAttribute(&Sound, "MusicTrace")) Sound.MusicTrace = 1;
	string parent = "";
	if (CheckAttribute(&Sound, "MusicTraceContext")) parent = Sound.MusicTraceContext;
	if (sti(Sound.MusicTrace) == 0) return parent;
	string context = reason + "(" + alias + ")";
	if (parent != "") context = parent + ">" + context;
	Sound.MusicTraceContext = context;
	int sequence = 0;
	if (CheckAttribute(&Sound, "MusicTraceScriptSeq")) sequence = sti(Sound.MusicTraceScriptSeq);
	sequence++;
	Sound.MusicTraceScriptSeq = sequence;
	string traceLocation = "";
	int traceHeroIndex = GetMainCharacterIndex();
	if (traceHeroIndex >= 0)
	{
		if (traceHeroIndex < GetArraySize(&Characters))
		{
			ref traceHero = GetMainCharacter();
			if (CheckAttribute(traceHero, "location")) traceLocation = traceHero.location;
		}
	}
	string line = "[MUSIC_SCRIPT] scriptSeq=" + sequence + " context=" + context + " alias=" + alias;
	line = line + " location=" + traceLocation + " musicID=" + musicID + " oldMusicID=" + oldMusicID;
	line = line + " musicName=" + musicName + " oldMusicName=" + oldMusicName + " base=" + musicBaseName;
	line = line + " set=" + musicSetPlayed + " force=" + musicForceRestart + " alarm=" + alarmed;
	line = line + " seaAlarm=" + seaAlarmed + " sea=" + bSeaActive + " map=" + bMapEnter + " boarding=" + LAi_boarding_process;
	Trace(line);
	return parent;
}

void MusicTraceEnd(string parent)
{
	Sound.MusicTraceContext = parent;
}

int Play3DSound(string name, float x, float y, float z)
{
	InitSound();
	return SendMessage(Sound,"lsllllllfff",MSG_SOUND_PLAY, name, SOUND_WAV_3D, VOLUME_FX, false, false, false, 0, x, y, z);
}

int Play3DSoundRet(string name, float x, float y, float z)
{
	InitSound();
	return SendMessage(Sound,"lsllllllfff",MSG_SOUND_PLAY, name, SOUND_WAV_3D, VOLUME_FX, false, false, false, 0, x, y, z);
}

int Play3DSoundCached(string name, float x, float y, float z)
{
	InitSound();
	return SendMessage(Sound,"lsllllllfff",MSG_SOUND_PLAY, name, SOUND_WAV_3D, VOLUME_FX, false, false, true, 0, x, y, z);
}

int Play3DSoundComplex(string name, float x, float y, float z, bool bLooped, bool bCached)
{
	InitSound();
	return SendMessage(Sound,"lsllllllfff",MSG_SOUND_PLAY, name, SOUND_WAV_3D, VOLUME_FX, false, bLooped, bCached, 0, x, y, z);
}

int PlayStereoSound(string name)
{
	InitSound();
	return SendMessage(Sound,"lslllll",MSG_SOUND_PLAY, name, SOUND_WAV_STEREO, VOLUME_FX, false, false, false);
}

int PlayVoiceSound(string name)
{
	InitSound();
	return SendMessage(Sound,"lslllll",MSG_SOUND_PLAY, name, SOUND_WAV_STEREO, VOLUME_SPEECH, false, false, false);
}

bool SoundIsPlaying(int id)
{
	InitSound();
	return SendMessage(Sound, "ll", MSG_SOUND_IS_PLAYING, id);
}

int PlayStereoSoundLooped(string name)
{
	InitSound();
	return SendMessage(Sound,"lsllll",MSG_SOUND_PLAY, name, SOUND_WAV_STEREO, VOLUME_FX, false, true, false);
}

int PlayStereoOGG(string name)
{
	string traceParent = MusicTraceBegin("PlayStereoOGG.FX", name);
	InitSound();
	int result = SendMessage(Sound,"lsllllll",MSG_SOUND_PLAY, name, SOUND_MP3_STEREO, VOLUME_FX, false, false, false, 0); //fix boal
	MusicTraceEnd(traceParent);
	return result;
}

void StopSound(int id, int fade)
{
	InitSound();
	SendMessage(Sound, "lll", MSG_SOUND_STOP, id, fade);
}

void ResumeSound(int id, int fade)
{
	InitSound();
	SendMessage(Sound, "lll", MSG_SOUND_RESUME, id, fade);
}

void ReleaseSound(int id)
{
	InitSound();
	SendMessage(Sound, "ll", MSG_SOUND_RELEASE, id);
}

void ResetSoundScheme()
{
	InitSound();
	SendMessage(Sound,"l",MSG_SOUND_SCHEME_RESET);
}

void SetSoundScheme(string schemeName)
{
	InitSound();
	SendMessage(Sound,"ls",MSG_SOUND_SCHEME_SET,schemeName);
}

void AddSoundScheme(string schemeName)
{
	InitSound();
	SendMessage(Sound,"ls",MSG_SOUND_SCHEME_ADD,schemeName);
}

void SetWeatherScheme(string scheme)
{
	if (Whr_IsStorm())
	{
		if (Whr_IsNight())
		{
			SetSoundScheme(scheme+"_night_storm");
		}
		else
		{
			SetSoundScheme(scheme+"_day_storm");
		}
	}
	else
	{
		if (Whr_IsRain())
		{
			if (Whr_IsNight())
			{
					SetSoundScheme(scheme+"_night_rain");
			}
			else
			{
					SetSoundScheme(scheme+"_day_rain");
			}
		}
		else
		{
			if (Whr_IsNight())
			{
					SetSoundScheme(scheme+"_night");
			}
			else
			{
					SetSoundScheme(scheme+"_day");
			}
		}
	}
}

void SetSchemeForLocation (ref loc)
{
	string traceParent = MusicTraceBegin("SetSchemeForLocation", loc.id);
    ref mchref = GetMainCharacter();
    mchref.GenQuest.God_hit_us = false; // нефиг воровать :)  (только в домах)
    int iColony = -1;

    if(CheckAttribute(loc,"type"))
	{
		ResetSoundScheme();
		musicName = ""; // музыка не играла, если переходили меж локациями одной схемы - багфиx boal 28.06.06
		switch (loc.type)
		{
			case "town":
				SetWeatherScheme("town");
				if (CheckAttribute(loc,"fastreload"))
				{
					iColony = FindColony(loc.fastreload);
				}
				if (iColony != -1)
				{
					if (Whr_IsDay()) SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music_day");
					else SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music_night");
				}
				else
				{
					SetMusicAlarm("music_gorod");
				}
			break;
			
			case "port":
				SetWeatherScheme("town");
				if (CheckAttribute(loc,"fastreload"))
				{
					iColony = FindColony(loc.fastreload);
				}
				if (iColony != -1)
				{
					if (Whr_IsDay()) SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music_day");
					else SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music_night");
				}
				else
				{
					SetMusicAlarm("music_gorod");
				}
			break;

			case "land": // дуэльное поле, лэндфорт и так далее
				SetWeatherScheme("land");
				SetMusicAlarm("music_jungle");
			break;

			case "jungle":
				SetWeatherScheme("land");
				if (loc.id.label == "ExitTown" || loc.id.label == "Incas Temple")
				{
					switch (loc.id.label)
					{
						case "ExitTown":
							if (Whr_IsDay()) SetMusicAlarm("music_exittown");
							else SetMusicAlarm("music_nightjungle");
						break;
						case "Incas Temple":
							SetMusicAlarm("music_teno");
						break;
					}
				}
				else
				{
					if (Whr_IsDay()) SetMusicAlarm("music_jungle");
					else SetMusicAlarm("music_nightjungle");
				}
				DeleteAttribute(pchar, "CheckStateOk"); // убрать флаг проверенности протектором
			break;

			case "arena":
				ResetSoundScheme();
				SetSoundScheme("arena");
				SetMusicAlarm("music_arena");
			break;

			case "Reefs_Chapter":
				ResetSoundScheme();
				SetSoundScheme("Reefs_Chapter");
				SetMusicAlarm("music_Reefs_Chapter");
			break;

			case "mayak":
				ResetSoundScheme();
				SetWeatherScheme("seashore");
				SetMusicAlarm("music_mayak");
			break;

			case "seashore":
				ResetSoundScheme();
				SetWeatherScheme("seashore");
				SetMusicAlarm("music_shore");
				DeleteAttribute(pchar, "CheckStateOk"); // убрать флаг проверенности протектором
			break;

			case "cave":
				SetSoundScheme("cave");
				SetMusicAlarm("music_cave");
				bMonstersGen = false; //сбросить флаг монстров
			break;

			case "dungeon":
				SetSoundScheme("dungeon");
				SetMusicAlarm("music_cave");
				if (!CheckAttribute(Pchar, "questTemp.WhisperLine"))
				{
					bMonstersGen = false; //сбросить флаг монстров
				}
			break;

			case "Labirint":
				SetSoundScheme("dungeon");
				SetMusicAlarm("music_Labirint");
			break;

			case "Alcove":
				SetSoundScheme("teno_inside");
				SetMusicAlarm("music_Alcove");
			break;

			// LEO: зал славы - интерьер по звуку, но со своей музыкой
			case "Final":
				SetSoundScheme("teno_inside");
				SetMusicAlarm("music_Final");
			break;

			case "questisland":
				ResetSoundScheme();
				if (loc.id.label == "jungle") SetWeatherScheme("land");
				else SetWeatherScheme("seashore");
				SetMusicAlarm("music_questislands");
			break;

			case "plantation":
				ResetSoundScheme();
				SetWeatherScheme("land");
				SetMusicAlarm("music_plantation");
			break;

			case "house":
				SetSoundScheme("house");
				if (CheckAttribute(loc,"brothel") && sti(loc.brothel) == true)
				{
					SetMusicAlarm("music_brothel");
				}
				else
				{
					if (CheckAttribute(loc,"id.label") && loc.id.label == "portoffice")
					{
						SetMusicAlarm("music_portoffice");
					}
					else
					{
						SetMusicAlarm("music_gorod");
					}
				}
				mchref.GenQuest.God_hit_us = true; // нефиг воровать :)
			break;

			case "tavern":
				SetSoundScheme("tavern");
				if (CheckAttribute(loc,"fastreload"))
				{
					iColony = FindColony(loc.fastreload);
				}
				if (iColony != -1)
				{
			    	SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music_tavern");
				}
				else
				{
					SetMusicAlarm("music_tavern");
				}
			break;

			case "shop":
				SetSoundScheme("shop");
				if (CheckAttribute(loc,"id.label") && loc.id.label == "Usurer House")
				{
					SetMusicAlarm("music_bank");
				}
				else
				{
					SetMusicAlarm("music_shop");
				}
			break;

			case "residence":
				SetSoundScheme("residence");
				if (CheckAttribute(loc,"fastreload"))   // boal
				{
					iColony = FindColony(loc.fastreload);
				}
				if (iColony != -1)
				{
			    	SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music_gubernator");
				}
				else
				{
					SetMusicAlarm("music_deck");
				}
			break;

			case "church":
				SetSoundScheme("church");
				SetMusicAlarm("music_church");
			break;

			case "cabine":
				SetSoundScheme("church");
				SetMusicAlarm("music_church");
			break;

			case "shipyard":
				SetSoundScheme("shipyard");
				SetMusicAlarm("music_shipyard");
			break;

			case "fort_attack": // атакуем форт, внутренняя локация
				SetSoundScheme("fort_attack");
				SetMusicAlarm("music_bitva");
			break;

			case "fort": // форт для мирных прогулок
				SetWeatherScheme("seashore");
				if (CheckAttribute(loc, "parent_colony"))
				{
					iColony = FindColony(loc.parent_colony);
				}
				if (iColony != -1)
				{
					if (Whr_IsDay()) SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music_day");
					else SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music_night");
				}
				else
				{
					SetMusicAlarm("music_gorod");
				}
			break;

			case "deck": // мирная палуба
				SetSoundScheme("deck");
				if (Whr_IsDay()) SetMusic("music_deck");
				else SetMusic("music_sea_night");
			break;

			case "deck_fight": // боевая палуба
				SetSoundScheme("deck_fight");
				SetMusic("music_abordage");
			break;

			case "boarding_cabine":
				SetSoundScheme("cabine");
				SetMusicAlarm("music_abordage");
			break;

			case "cap_cabine":
				SetSoundScheme("deck");
				if (Whr_IsDay()) SetMusicAlarm("music_deck");
				else SetMusicAlarm("music_sea_night");
			break;

			case "jail":
				//пасхалка
				SetSoundScheme("jail");
				if (GetNpcQuestPastDayWOInit(pchar, "jail_ee") > 20 && rand(1))
				{
					SaveCurrentNpcQuestDateParam(pchar, "jail_ee");
					SetMusicAlarm("music_jail_ee");
				}
				else	SetMusicAlarm("music_jail");
			break;

			case "ammo":
				SetSoundScheme("jail");
				SetMusicAlarm("music_jail");
			break;

			case "LostShipsCity":
				SetWeatherScheme("LostShipsCity");
				SetMusicAlarm("music_LostShipsCity");
			break;

			case "LSC_inside":
				SetWeatherScheme("LSC_inside");
				SetMusicAlarm("music_LostShipsCity");
			break;

			case "underwater":
				SetSoundScheme("underwater");
				SetMusicAlarm("music_underwater");
			break;

			case "teno":
				SetWeatherScheme("land");
				SetMusicAlarm("music_teno");
			break;

			case "teno_inside":
				SetSoundScheme("teno_inside");
				SetMusicAlarm("music_teno_inside");
			break;

			case "MountainPath":
				SetSoundScheme("land");
				SetMusicAlarm("music_MountainPath");
			break;

			case "DeckWithReefs":
				SetSoundScheme("land");
				SetMusicAlarm("music_DeckWithReefs");
			break;
			
			case "QuestTragic":
				SetSoundScheme("land");
				SetMusicAlarm("music_Tragic");
			break;
			
			case "hut":
				SetSoundScheme("house");
				SetMusicAlarm("music_gorod");
			break;
		}
	}
	SetStaticSounds(loc);
	MusicTraceEnd(traceParent);
}

void SetStaticSounds (ref loc)
{
	if(IsEntity(loc) != true) return;
	string locatorGroupName = "locators.sound";
	if(CheckAttribute(loadedLocation, locatorGroupName) == 0) return;
	aref locatorGroup;
	makearef(locatorGroup, loc.(locatorGroupName));
	int locatorCount = GetAttributesNum(locatorGroup);
	if(locatorCount <= 0) return;
	string locatorName, locatorType;
	int locatorNameLength;

	for(int i = 0; i < locatorCount; i++)
	{
		aref locator = GetAttributeN(locatorGroup, i);
		locatorName = GetAttributeName(locator);
		locatorNameLength = strlen(locatorName);
		locatorType = strcut(locatorName, 0, locatorNameLength-3);
		if ((locatorType == "nightinsects")||(locatorType == "torch"))
		{
			if (Whr_IsDay() && loc.type != "Dungeon" && loc.type != "cave" && loc.type != "fort_attack") continue;
   		}
		if (locatorType == "shipyard")
		{
			if (Whr_IsNight()) continue;
   		}
		if (locatorType == "church")
		{
			if (Whr_IsNight()) continue;
   		}
		if (locatorType == "windmill")
		{
			continue;
   		}

		SendMessage(Sound, "lsllllllfff", MSG_SOUND_PLAY, locatorType, SOUND_WAV_3D, VOLUME_FX, 0, 1, 0, 0, stf(locator.x), stf(locator.y), stf(locator.z));
	}

}

void SetSchemeForSea()
{
	string traceParent = MusicTraceBegin("SetSchemeForSea", "");
	ResetSoundScheme();

	if (Whr_IsNight())
	{
		if (Whr_IsStorm())
		{
			AddSoundScheme("sea_night_storm");
			SetMusic("music_storm");
		}
		else
		{
			if (pchar.Ship.POS.Mode == SHIP_WAR)
			{
				SetMusic("music_sea_battle");
			}
			else
			{
				if (Whr_IsDay()) SetMusic("music_sea_day");
				else SetMusic("music_sea_night");
			}
			if (Whr_IsRain())
			{
				AddSoundScheme("sea_night_rain");
			}
			else
			{
				AddSoundScheme("sea_night");
			}
		}
	}
	else
	{
		if (Whr_IsStorm())
		{
			AddSoundScheme("sea_day_storm");
			SetMusic("music_storm");
		}
		else
		{
   			if (pchar.Ship.POS.Mode == SHIP_WAR)
			{
				SetMusic("music_sea_battle");
			}
			else
			{
				if (Whr_IsDay()) SetMusic("music_sea_day");
				else SetMusic("music_sea_night");
			}
			if (Whr_IsRain())
			{
				AddSoundScheme("sea_day_rain");
			}
			else
			{
				AddSoundScheme("sea_day");
			}
		}
	}
	ResumeAllSounds();
	MusicTraceEnd(traceParent);
}

void SetSchemeForMap()
{
	string traceParent = MusicTraceBegin("SetSchemeForMap", "");
	ResetSoundScheme();
	AddSoundScheme("sea_map");
	SetMusic("music_map");
	ResumeAllSounds();
	bFortCheckFlagYet = false; //eddy. уберем флаг распознавания фортом врага
	MusicTraceEnd(traceParent);
}

int musicID = -1;
int oldMusicID = -1;
int boardM = -1;
string musicName = "";
string oldMusicName = "";
string musicBaseName = ""; // LEO: имя трека БЕЗ суффикса набора - чтобы перезапустить его при смене набора музыки на лету
int musicSetPlayed = -1;   // LEO: набор, в котором СТАРТОВАЛ текущий трек (по нему решаем, нужна ли смена)
int musicForceRestart = 0; // LEO: разовый флаг - перезапустить трек, даже если имя совпало с musicName
// LEO: тревога и морской переход могут вернуть уже оформленный алиас из старого состояния.
// LEO: суффикс выбранного набора добавляется ровно один раз, включая сохранённые двойные суффиксы.
string MusicBaseAlias(string name)
{
	int length = strlen(name);
	while (length > 3)
	{
		string suffix = strcut(name, length - 3, length - 1);
		if (suffix != "_CF" && suffix != "_VN") return name;
		name = strcut(name, 0, length - 4);
		length = strlen(name);
	}
	return name;
}

void SetMusic(string name)
{
	string traceParent = MusicTraceBegin("SetMusic", name);
	if (pchar.location == "UnderWater") //не играть музон под водой
	{
		MusicTraceEnd(traceParent);
		return;
	}
	InitSound();

	name = MusicBaseAlias(name);
	musicBaseName = name; // LEO: запоминаем ДО приписки суффикса набора

	switch (sti(InterfaceStates.StreamerMode)) {
		case 1: name = name + "_CF"; break; // без авторских прав (для стримов)
		case 2: name = name + "_VN"; break; // ванильная (оригинальная) музыка
	}

	// LEO: musicName - НЕ надёжный признак "что играет сейчас": SetMusicAlarm пишет туда мирный трек
	//   БЕЗ суффикса набора, пока играет боевая схема. Из-за этого смена набора могла залипнуть
	//   (имя совпадало -> уходили в RESUME) и старый трек доигрывал до перезагрузки локации.
	//   Поэтому смену набора пропускаем сюда принудительно, флагом.
	if (name == musicName && musicForceRestart == 0 && SoundIsPlaying(musicID))
	{
		SendMessage(Sound, "lll", MSG_SOUND_RESUME, musicID, SOUNDS_FADE_TIME);
		MusicTraceEnd(traceParent);
		return;
	}
	musicForceRestart = 0;

	if (oldMusicID >= 0)
	{
		//#20191125-03
		SendMessage(Sound, "lll", MSG_SOUND_STOP, oldMusicID, 0);
		oldMusicID = -1;
	}

	if (musicID >= 0)
	{
		SendMessage(Sound, "lll", MSG_SOUND_STOP, musicID, MUSIC_CHANGE_TIME);
		oldMusicID = musicID;
	}

	musicID = SendMessage(Sound, "lslllllll", MSG_SOUND_PLAY, name, SOUND_MP3_STEREO, VOLUME_MUSIC, true, true, false, MUSIC_CHANGE_TIME, 0);
	// LEO: неудачный запуск не кэшируем и не возобновляем все звуки нулевым дескриптором.
	if (musicID <= 0)
	{
		if (musicName != "") oldMusicName = musicName;
		musicID = -1;
		musicName = "";
		musicSetPlayed = -1;
		MusicTraceEnd(traceParent);
		return;
	}
	SendMessage(Sound, "lll", MSG_SOUND_RESUME, musicID, MUSIC_CHANGE_TIME);

	// LEO: повтор текущего или неудавшегося трека не заменяет музыку возврата после тревоги.
	if (name != musicName)
	{
		if (musicName != "" || musicSetPlayed >= 0) oldMusicName = musicName;
	}
	musicName = name;
	musicSetPlayed = sti(InterfaceStates.StreamerMode); // LEO: в каком наборе стартовал этот трек
	MusicTraceEnd(traceParent);
}

// LEO: перезапуск ТЕКУЩЕГО трека в другом наборе музыки - применение опции "Выбор музыкального
//   сопровождения" на лету. Раньше набор подхватывался только при следующей смене локации:
//   SetMusic дописывает суффикс набора (_CF/_VN) к имени, а сам вызывается лишь по событиям сцены.
//   Все три набора алиасов движок грузит при старте разом, так что трек нужно лишь переиграть.
//   Сама смена плавная: SetMusic глушит старый трек затуханием MUSIC_CHANGE_TIME и вводит новый
//   с таким же fade_in - строить кроссфейд отдельно не нужно.
//
//   Нужна ли смена - решаем по musicSetPlayed (набор, в котором стартовал текущий трек), а НЕ по
//   совпадению имён: musicName подменяет SetMusicAlarm, из-за чего смена набора залипала.
void ReapplyMusicSet()
{
	if (musicBaseName == "") return;
	if (musicSetPlayed == sti(InterfaceStates.StreamerMode)) return; // набор не менялся - трек не дёргаем
	string traceParent = MusicTraceBegin("ReapplyMusicSet", musicBaseName);

	// LEO: смена набора не меняет композицию, к которой нужно вернуться после тревоги.
	string returnMusicName = oldMusicName;
	musicForceRestart = 1;
	SetMusic(musicBaseName);
	musicForceRestart = 0; // SetMusic мог выйти раньше (под водой) - не оставляем флаг взведённым
	oldMusicName = returnMusicName;
	MusicTraceEnd(traceParent);
}

void FadeOutMusic(int _time)
{
	string traceParent = MusicTraceBegin("FadeOutMusic", its(_time));
    //#20180920-05
    if(_time < 100)
        _time *= 1000;
	if (musicID >= 0)
	{
		StopSound(musicID, _time);
		musicID = -1;
		musicName = "";
	}
	MusicTraceEnd(traceParent);
}

void PauseAllSounds()
{
	string traceParent = MusicTraceBegin("PauseAllSounds", "");
	SendMessage(Sound,"lll",MSG_SOUND_STOP, 0, SOUNDS_FADE_TIME);
	MusicTraceEnd(traceParent);
}

void ResumeAllSounds()
{
	string traceParent = MusicTraceBegin("ResumeAllSounds", "");
	SendMessage(Sound,"lll",MSG_SOUND_RESUME, musicID, SOUNDS_FADE_TIME);
	MusicTraceEnd(traceParent);
}

int PlaySoundDist3D(string name, float x, float y, float z)
{
	return Play3DSound(name, x,y,z);
}

int PlaySoundLoc3D(string name, float x, float y, float z)
{
	return Play3DSoundCached(name, x,y,z);
}

int PlaySound3D(string name, float x, float y, float z)
{
	return Play3DSound(name, x,y,z);
}

void Sound_SetVolume(int iSoundID, float fVolume)
{
	SendMessage(Sound, "llf", MSG_SOUND_SET_VOLUME, iSoundID, fVolume);
}

int PlaySound(string name)
{
	return PlayStereoSound(name);
}

int PlayVoice(string name)
{
	return PlayVoiceSound(name);
}

int PlaySoundComplex(string sSoundName, bool bSimpleCache, bool bLooped, bool bCached, int iFadeTime)
{
	return SendMessage(Sound,"lsllllll",MSG_SOUND_PLAY,VOLUME_FX,sSoundName,SOUND_WAV_3D,bSimpleCache,bLooped,bCached,iFadeTime);
}

void StopMusic()
{
}

void PlayMusic(string n)
{
}
object Sound;

int alarmed = 0;
int oldAlarmed = 0;
bool seaAlarmed = false;
bool oldSeaAlarmed = false;
string battleMusicScheme = "music_bitva";

void SetBattleMusicAlarm(string name)
{
    string traceParent = MusicTraceBegin("SetBattleMusicAlarm", name);
    battleMusicScheme = name;
    MusicTraceEnd(traceParent);
}

void SetMusicAlarm(string name)
{
    string traceParent = MusicTraceBegin("SetMusicAlarm", name);
    if(seaAlarmed) {
         if(!bSeaActive || bMapEnter)
            seaAlarmed = false;
    }
	if (alarmed == 0 && seaAlarmed==false)
	{
		SetMusic(name);
	}
	else
	{
		if(name != "") {
			musicName = name;
		}
		SetMusic(battleMusicScheme);

		if (LAi_boarding_process != 0)
		{
			if (!CheckAttribute(loadedLocation, "CabinType"))
			{
				boardM = 1; // потом сбросим звук и схему
			}
		}
	}
	MusicTraceEnd(traceParent);
}

void Sound_OnAllarm()
{
	Sound_OnAlarm(GetEventData());
}

int abordageSoundID;

void Sound_OnAlarm(bool _alarmed)
{
	alarmed = _alarmed;
	if (alarmed == oldAlarmed)
	return;
	string traceParent = MusicTraceBegin("Sound_OnAlarm", its(alarmed));

	if (alarmed != 0)
	{
		switch (loadedLocation.id)
        {
            case "FencingTown_Arena": SetMusic("music_bitva_arena"); break;
            case "MountainPath": SetMusic("music_MountainPathFight"); break;
            case "DeckWithReefs": SetMusic("music_MountainPathFight"); break;
			case "CaveSatanists": SetMusic("music_MountainPath"); break;
            else SetMusic("music_bitva"); break;
        }
	}
	else
	{
		SetMusic(oldMusicName);
	}
	oldAlarmed = alarmed;
	MusicTraceEnd(traceParent);
}

void Sound_OnSeaAlarm(bool _seaAlarmed)
{
	seaAlarmed = _seaAlarmed;
	if (seaAlarmed == oldSeaAlarmed)
		return;
	string traceParent = MusicTraceBegin("Sound_OnSeaAlarm", its(seaAlarmed));

	if (seaAlarmed)
	{
		SetMusic("music_sea_battle");
	}
	else
	{
		SetMusic(oldMusicName);
	}
	oldSeaAlarmed = seaAlarmed;
	MusicTraceEnd(traceParent);
}

// set music without any checks
void Sound_OnSeaAlarm555(bool _seaAlarmed, bool bHardAlarm)
{
	string traceParent = MusicTraceBegin("Sound_OnSeaAlarm555", its(bHardAlarm));
	if (bHardAlarm) { oldSeaAlarmed = !_seaAlarmed; }

	Sound_OnSeaAlarm(_seaAlarmed);
	MusicTraceEnd(traceParent);
}

void InitSound()
{
	if (!IsEntity(&Sound))
	{
		CreateEntity(&Sound, "Sound");
		SetEventHandler("eventAllarm", "Sound_OnAllarm", 0);
	}
}

void ResetSound()
{
	string traceParent = MusicTraceBegin("ResetSound", "");
	// fix -->
	if (musicID > 0)
	{
		StopSound(musicID,0);
	}
	if (oldMusicID > 0)
	{
		StopSound(oldMusicID,0);
	}
	ResetSoundScheme();
	// fix <--
	StopSound(0,0);
	ReleaseSound(0);
	musicName = "";
	oldMusicName = "";
	// LEO: после полного сброса смена набора не должна оживлять музыку прошлой сцены.
	musicBaseName = "";
	musicSetPlayed = -1;
	musicForceRestart = 0;
	musicID = -1;    //fix boal не было нуления ИД
    oldMusicID = -1;
	MusicTraceEnd(traceParent);
}

void LoadSceneSound()
{
	string traceParent = MusicTraceBegin("LoadSceneSound", "");
	if (CheckAttribute(PChar, "currentsoundtrack"))
	{
		string track = pchar.currentsoundtrack;
		SetMusic(track);
		MusicTraceEnd(traceParent);
		return;
	}
	int i = FindLoadedLocation();
	if (i != -1)
		SetSchemeForLocation(&Locations[i]);
	MusicTraceEnd(traceParent);
}

void ReloadStaticSound()
{
	int i = FindLoadedLocation();
	if (i != -1)
		SetStaticSounds(&Locations[i]);
    else {
        if(bSeaActive) {
            if(Whr_IsStorm()) SetSchemeForSea();
            Ship_RecreateStaticSounds();
        }
    }
}
//#20180815-03
void SoundEnviron(int rvbEnviron, int rSetting, float fWet);
{
}

bool bChangeScheme(int i, int j)
{   
	if (CheckAttribute(pchar, "questTemp.lockedMusic")) 
    {
		return false;
	}
	
	if(CheckAttribute(&Locations[i],"type") && CheckAttribute(&Locations[j],"type"))
	{
		if(Locations[i].type == "questisland") 
        {    
            return true;
        }
		if(Locations[i].type != Locations[j].type) 
        {
            return true;
        }
        else
        {
            if(checkDay(reload_cur_time) != checkDay(reload_time))
            {
                return true;
            }
            if(CheckAttribute(&Locations[i],"townsack") && CheckAttribute(&Locations[j],"townsack"))
            {
                int iNation1 = GetCityNation(Locations[i].townsack);
                int iNation2 = GetCityNation(Locations[j].townsack);
                if(iNation1 != iNation2)
                {
                    return true;
                }
            }
        }
	}

	return false;
}
