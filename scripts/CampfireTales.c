// LEO: Костёр рассказчиков в Мариго - ЧЕТЫРЕ моряка/офицера травят байки у огня (21:00-22:59).
// Каждый рассказывает ОДНУ байку за вечер (монолог); ГГ подсаживается и слушает всех по очереди,
// диалог не прерывается, камера переключается на говорящего. Байки за вечер не повторяются.
// Локаторы Fire1_1..Fire1_5 (группа sit, локация Marigo_town): 4 под рассказчиков, 1 свободный под ГГ - все случайно.
// Появляются СИДЯ при входе в город (костёр перенесён на хук LAi_CharacterPostLogin в LAi_login.c).
//
// БАГ-ФИКС: рассказчикам НЕ ставим CityType. isLocationHasCitizens (LAi_utilites.c) считает "жители есть"
// по ЛЮБОМУ персонажу с атрибутом CityType в локации -> CreateCitizens рано выходит и жители города не
// генерятся (а после ухода рассказчиков не возвращаются). Без CityType жители города живут как обычно.

#define CAMPFIRE_STORY_COUNT   50   // LEO: число доступных баек (узлы S<n>_1..S<n>_3 в обоих диалогах CampfireTales.c)
#define CAMPFIRE_TELLERS       4    // LEO: рассказчиков за вечер (по одной байке каждый)

// LEO: случайная модель рассказчика (rand - косметика). if вместо switch: Storm не принимает 'return' в case.
string Campfire_RandSailorModel()
{
	string s = "shipowner_22";
	int r = rand(3);
	if (r == 1) s = "pirate_19";
	if (r == 2) s = "pirate_20";
	if (r == 3) s = "shipowner_23";
	return s;
}
string Campfire_RandOfficerModel()
{
	string s = "officer_4";
	int r = rand(3);
	if (r == 1) s = "officer_6";
	if (r == 2) s = "officer_8";
	if (r == 3) s = "officer_17";
	return s;
}

// LEO: фикс-список ИД рассказчиков (по слотам 0..3)
string Campfire_TellerId(int i)
{
	string s = "CampTeller_A";
	if (i == 1) s = "CampTeller_B";
	if (i == 2) s = "CampTeller_C";
	if (i == 3) s = "CampTeller_D";
	return s;
}

// LEO: ИД рассказчика, занятого в слоте idx этого вечера (для камеры в диалоге)
string Campfire_GetTeller(int idx)
{
	string sKey = "Teller" + idx;
	if (CheckAttribute(pchar, "CampfireTales." + sKey)) return pchar.CampfireTales.(sKey);
	return "CampTeller_A";
}

// LEO: номер байки, назначенной слоту idx
int Campfire_GetTale(int idx)
{
	string sKey = "Tale" + idx;
	if (CheckAttribute(pchar, "CampfireTales." + sKey)) return sti(pchar.CampfireTales.(sKey));
	return 1;
}

// LEO: ключ текущего игрового дня (для гейта "один костёр за вечер", переживает деспавн рассказчиков)
string Campfire_TodayKey()
{
	return "" + GetDataYear() + "." + GetDataMonth() + "." + GetDataDay();
}

// LEO: подобрать 4 РАЗНЫЕ байки на вечер (не повторять подряд за вечер; не начинать с прошлой). Пишет Tale0..Tale3.
void Campfire_PickStories()
{
	int iLast = 0;
	if (CheckAttribute(pchar, "CampfireTales.LastStory")) iLast = sti(pchar.CampfireTales.LastStory);

	int picked[4];
	int cnt, guard, j;
	bool bDup;
	cnt = 0;
	guard = 0;
	while (cnt < CAMPFIRE_TELLERS && guard < 500)
	{
		guard++;
		int n = rand(CAMPFIRE_STORY_COUNT - 1) + 1;   // 1..COUNT
		bDup = (n == iLast);
		for (j = 0; j < cnt; j++) { if (picked[j] == n) bDup = true; }
		if (bDup) continue;
		picked[cnt] = n;
		cnt++;
	}
	pchar.CampfireTales.Tale0 = picked[0];
	pchar.CampfireTales.Tale1 = picked[1];
	pchar.CampfireTales.Tale2 = picked[2];
	pchar.CampfireTales.Tale3 = picked[3];
	pchar.CampfireTales.LastStory = picked[CAMPFIRE_TELLERS - 1]; // последняя за вечер - чтоб следующий вечер не начался с неё
}

// LEO: активна ли линейка Анжелики Тич (квест AT_PZD)? Тогда костёр уступает ей Мариго ЦЕЛИКОМ
// (её матросы садятся на те же fire1_*). InProgress-флаг ИЛИ её матросы уже в Мариго (запас для старых сейвов).
bool Campfire_TichLineActive()
{
	if (CheckAttribute(pchar, "questTemp.AT_PZD_InProgress")) return true;
	int iAtp = GetCharacterIndex("AT_PZD_Matros1");
	if (iAtp >= 0)
	{
		if (CheckAttribute(&Characters[iAtp], "location"))
		{
			if (Characters[iAtp].location == "Marigo_town") return true;
		}
	}
	return false;
}

// LEO: снять всех рассказчиков со сцены (разрешение конфликта с линейкой Тич / финальный деспавн)
void Campfire_RemoveTellers()
{
	int i;
	ref t;
	for (i = 0; i < CAMPFIRE_TELLERS; i++)
	{
		// LEO: через GetCharacterIndex, НЕ CharacterFromID - последний в бета-режиме СПАМИТ "Не найден НПС"
		// на каждый несуществующий id (рассказчики могли ещё не спавниться). Индекс -1 = молча пропускаем.
		int idx = GetCharacterIndex(Campfire_TellerId(i));
		if (idx < 0) continue;
		t = &characters[idx];
		if (CheckAttribute(t, "id")) ChangeCharacterAddress(t, "none", "");
	}
	DeleteAttribute(pchar, "CampfireTales");
}

// LEO: спавн костра у Мариго - зовётся из LAi_CharacterPostLogin (LAi_login.c) на входе в локацию
// (НЕ из CreateCitizens - тот гейтится isLocationHasCitizens и не ловит загрузку сейва на месте / выход из магазина)
void Campfire_TrySpawn(aref loc)
{
	if (!CheckAttribute(loc, "id")) return;
	if (loc.id != "Marigo_town") return;

	// LEO: миграция старых сейвов - снять CityType со старых рассказчиков (он глушил генерацию жителей города)
	int iHeal;
	ref th;
	for (iHeal = 0; iHeal < CAMPFIRE_TELLERS; iHeal++)
	{
		// LEO: GetCharacterIndex вместо CharacterFromID - тот спамил "Не найден НПС" в бета-режиме на старте
		// в Мариго, когда рассказчиков ещё нет (миграция гоняется на КАЖДОМ входе). Нет персонажа - пропуск.
		int iThIdx = GetCharacterIndex(Campfire_TellerId(iHeal));
		if (iThIdx < 0) continue;
		th = &characters[iThIdx];
		if (CheckAttribute(th, "id")) { if (CheckAttribute(th, "CityType")) DeleteAttribute(th, "CityType"); }
	}

	// LEO: во время квеста Анжелики Тич (AT_PZD) костёр НЕ спавним - просто отклоняем спавн в fire1_*,
	// чтобы локаторы достались её сцене. Жителей города это НЕ трогает (рассказчики без CityType).
	// Уже сидящих рассказчиков снимаем ТОЛЬКО если они реально заняли fire1_* (освободить локаторы).
	if (Campfire_TichLineActive())
	{
		int iTich = GetCharacterIndex("CampTeller_A");
		if (iTich >= 0)
		{
			if (CheckAttribute(&Characters[iTich], "location"))
			{
				if (Characters[iTich].location == "Marigo_town") Campfire_RemoveTellers();
			}
		}
		return;
	}

	int iHour = GetHour();
	if (iHour < 21 || iHour >= 23) return;   // LEO: только 21:00-22:59

	// LEO: один костёр за вечер - если сегодня уже отслушали (Campfire_End пометил день), не появляемся снова
	if (CheckAttribute(pchar, "CampfireDoneDay")) { if (pchar.CampfireDoneDay == Campfire_TodayKey()) return; }

	// LEO: не плодить дубли - если рассказчик уже сидит в Мариго (вложенные if - && в Storm не ленивый)
	int iExist = GetCharacterIndex("CampTeller_A");
	if (iExist >= 0)
	{
		if (CheckAttribute(&Characters[iExist], "location"))
		{
			if (Characters[iExist].location == "Marigo_town") return;
		}
	}
	// LEO: нация колонии (как в GovSwap_TryDealerOnEnter - из loc.fastreload)
	if (!CheckAttribute(loc, "fastreload")) return;
	int iColony = FindColony(loc.fastreload);
	if (iColony < 0) return;
	int iNation = sti(Colonies[iColony].nation);

	int i, k;
	string sTmp;

	// тип рассказчиков на вечер: все моряки ИЛИ все офицеры (по dRand)
	bool bOff = (dRand(1) == 1);

	// локаторы Fire1_1..Fire1_5 случайно тасуем: [0..3] - рассказчикам, [4] - свободный под ГГ
	string sLocs[5];
	for (i = 0; i < 5; i++) sLocs[i] = "Fire1_" + (i + 1);
	for (i = 4; i > 0; i--)
	{
		k = rand(i);
		sTmp = sLocs[i]; sLocs[i] = sLocs[k]; sLocs[k] = sTmp;
	}

	// имена случайно из пула, 4 разных на вечер
	string sNames[8];
	sNames[0] = "Корвин"; sNames[1] = "Бен";    sNames[2] = "Тоби";   sNames[3] = "Магнус";
	sNames[4] = "Жак";    sNames[5] = "Гаспар"; sNames[6] = "Пью";    sNames[7] = "Орри";
	for (i = 7; i > 0; i--)
	{
		k = rand(i);
		sTmp = sNames[i]; sNames[i] = sNames[k]; sNames[k] = sTmp;
	}

	string sGroup = GetNationNameByType(iNation) + "_citizens";
	ref t;
	string sId, sKey, mdl;

	for (i = 0; i < CAMPFIRE_TELLERS; i++)
	{
		sId = Campfire_TellerId(i);
		if (bOff) mdl = Campfire_RandOfficerModel(); else mdl = Campfire_RandSailorModel();

		t = GetCharacter(NPC_GenerateCharacter(sId, mdl, "man", "man", 3, iNation, -1, false));
		t.name = sNames[i];
		t.lastname = "";
		LAi_SetLoginTime(t, 21.0, 23.99);
		LAi_SetGroundSitType(t);
		t.dialogCamH = "0.5";   // LEO: камера диалога ниже (сидят на земле) - лица не закрыты диалог-боксом
		t.dialog.filename = "Quest\CampfireTales.c";
		t.dialog.currentnode = "first time";
		t.greeting = "Gr_Tavern_Mate";
		t.City = "Marigo";
		// LEO: НЕ ставим CityType - иначе глушит генерацию жителей города (см. шапку файла)
		if (i == 0) t.campHost = true;   // LEO: главный рассказчик (к нему подсаживается ГГ)
		LAi_SetImmortal(t, true);
		LAi_group_MoveCharacter(t, sGroup);
		ChangeCharacterAddressGroup(t, "Marigo_town", "sit", sLocs[i]);

		sKey = "Teller" + i;
		pchar.CampfireTales.(sKey) = sId;
	}

	Campfire_PickStories();                       // 4 разные байки -> Tale0..Tale3
	pchar.CampfireTales.FreeLoc = sLocs[4];        // свободный локатор под ГГ
	pchar.CampfireTales.Idx     = 0;               // сколько баек уже рассказано за вечер
	pchar.CampfireTales.Target  = CAMPFIRE_TELLERS; // всего баек за вечер (диалог завершает по Idx >= Target)
}

// LEO: усадить ГГ на землю у костра (свободный локатор) - зовётся из begin_listen под затемнением.
// Параметр string _tmp обязателен: DoQuestFunctionDelay зовёт функцию со строковым аргументом (иначе stack error).
void Campfire_SeatPlayer(string _tmp)
{
	if (CheckAttribute(pchar, "CampfireTales.FreeLoc"))
	{
		ChangeCharacterAddressGroup(pchar, "Marigo_town", "sit", pchar.CampfireTales.FreeLoc);
	}
	LAi_SetGroundSitType(pchar);   // LEO: ГГ садится на землю
	// LEO: камеру выставляем ТУТ - ГГ уже пересажен, камера считается от верной (сидячей) позиции
	int iHostIdx = GetCharacterIndex("CampTeller_A"); // LEO: без CharacterFromID - не спамить бету, если рассказчика нет
	if (iHostIdx >= 0) SetCameraDialogMode(&characters[iHostIdx]);
}

// LEO: вечер окончен - ГГ встаёт; рассказчики встают, шагом уходят к выходу и деспавнятся
void Campfire_End()
{
	pchar.CampfireDoneDay = Campfire_TodayKey();   // LEO: помечаем вечер отслушанным - сегодня костёр больше не появится
	LAi_SetPlayerType(pchar);   // LEO: ГГ встаёт и получает управление обратно (снимаем ground-sit)
	int i;
	ref t;
	string sExit = LAi_FindFreeRandomLocator("reload");
	if (sExit == "")
	{
		Campfire_RemoveTellers();
		return;
	}
	for (i = 0; i < CAMPFIRE_TELLERS; i++)
	{
		int iEndIdx = GetCharacterIndex(Campfire_TellerId(i)); // LEO: без CharacterFromID - не спамить бету
		if (iEndIdx < 0) continue;
		t = &characters[iEndIdx];
		if (CheckAttribute(t, "id"))
		{
			LAi_SetActorType(t);
			LAi_ActorGoToLocator(t, "reload", sExit, "", -1);
		}
	}
	DoQuestFunctionDelay("Campfire_Despawn", 8.0);   // дать дойти до выхода, затем убрать со сцены
}

// LEO: финальный деспавн рассказчиков (string _tmp - вызов через DoQuestFunctionDelay)
void Campfire_Despawn(string _tmp)
{
	Campfire_RemoveTellers();
}
