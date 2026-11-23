//#20200510-03
#define SLIDING_BLK_THRESH 275.0
#define DEFAULT_BLK_THRESH 375.0
//==========================================================================================
//Update events
//==========================================================================================

void LAi_Character_FrameUpdate()
{
	float dltTime = GetEventData();
	//Обновляем состояние персонажей
	LAi_AllCharactersUpdate(dltTime);
	//Задержка исполнения квестов
	LAi_QuestDelayProcess(dltTime);
}

void LAi_CharacterUpdate()
{
	//Параметры
	aref chr = GetEventData();
	float dltTime = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterUpdate") == false) return;
	//Процессируем соответствующий тип
	string func = chr.chr_ai.type;
	if(func != "")
	{
		func = "LAi_type_" + func + "_CharacterUpdate";
		call func(chr, dltTime);
	}
	//Процессируем соответствующий шаблон
	func = chr.chr_ai.tmpl;
	if(func == "") return;
	func = "LAi_tmpl_" + func + "_CharacterUpdate";
	call func(chr, dltTime);
}

//==========================================================================================
//EndTask events
//==========================================================================================

void LAi_CharacterEndTask()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterEndTask") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Определяем отработавшую задачу
	bool isProcessed = false;
	switch(endTask)
	{
		case "Goto point":
		func = "LAi_tmpl_" + func + "_EndGoToPoint";
		isProcessed = true;
		break;
		case "Runto point":
		func = "LAi_tmpl_" + func + "_EndRunToPoint";
		isProcessed = true;
		break;
		case "Escape":
		func = "LAi_tmpl_" + func + "_EndEscape";
		isProcessed = true;
		break;
	}
	if(isProcessed != false)
	{
		call func(chr);
		}else{
		Trace("LAi_CharacterEndTask -> unknow end task <" + endTask + ">");
	}
}

//==========================================================================================
//TaskFailure events
//==========================================================================================

void LAi_CharacterTaskFailure()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterTaskFailure") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Определяем невыполнившиюся задачу
	bool isProcessed = false;
	switch(endTask)
	{
		case "Goto point":
		func = "LAi_tmpl_" + func + "_FailureGoToPoint";
		isProcessed = true;
		break;
		case "Runto point":
		func = "LAi_tmpl_" + func + "_FailureRunToPoint";
		isProcessed = true;
		break;
		case "Follow character":
		func = "LAi_tmpl_" + func + "_FailureFollow";
		isProcessed = true;
		break;
		case "Fight":
		func = "LAi_tmpl_" + func + "_FailureFight";
		isProcessed = true;
		break;
		case "Escape":
		func = "LAi_tmpl_" + func + "_FailureEscape";
		isProcessed = true;
		break;
	}
	if(isProcessed != false)
	{
		call func(chr);
		}else{
		Trace("LAi_CharacterTaskFailure -> unknow failure task <" + endTask + ">");
	}
}

void LAi_CharacterBusyPos()
{
	//Параметры
	aref chr = GetEventData();
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterTaskFailure") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	func = "LAi_tmpl_" + func + "_BusyPos";
	call func(chr, x, y, z);
}


//==========================================================================================
//Follow character's events
//==========================================================================================

void LAi_CharacterFollowGo()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterFollowGo") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_FollowGo";
	call func(chr);
}

void LAi_CharacterFollowStay()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterFollowStay") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_FollowStay";
	call func(chr);
}

//==========================================================================================
//Fight character's events
//==========================================================================================

void LAi_CharacterFightGo()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterFightGo") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_FightGo";
	call func(chr);
}

void LAi_CharacterFightStay()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterFightStay") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_FightStay";
	call func(chr);
}

void LAi_CharacterAttack()
{
	aref attack = GetEventData();
	aref enemy = GetEventData();
	string attackType = GetEventData();
	string fencing_type = LAi_GetBladeFencingType(attack);
	float aSkill = LAi_GetCharacterFightLevel(attack);
	
	//пробитие блока
	bool isBlocked = GetEventData();
	bool isSaberGun = GetEventData();
	int blckTime = GetEventData();
	bool blockSave = isBlocked;
	bool bAtkTypeBreak = (attackType == "break");

	// основной чек на пробитие - Gregg
	bool bBlockedMusket = isBlocked && IsIntegratedMusketDrawn(attack);
	if (isBlocked && !bBlockedMusket)
	{
		if (sti(InterfaceStates.OtherParticles) && enemy.sex != "crab") LaunchSparks(enemy);
		if (pnrand(attack, LAi_CalcBlockBreakChance(attack, fencing_type, aSkill), "blockbreak"))
		{
			if (sti(enemy.index) == GetMainCharacterIndex()) {// идеальный блок - только у гг
				blockSave = (blckTime < SLIDING_BLK_THRESH) || bAtkTypeBreak && (blckTime < DEFAULT_BLK_THRESH);
				if (blockSave) {
					setEffectText(enemy, XI_ConvertString("PerfectBlock"), argb(255,255,255,255), GetCharCapNotifScale());
				}
				else {
					setEffectText(enemy, XI_ConvertString("BlockBreak"), argb(255,255,255,255), GetCharCapNotifScale());
					PlayStereoSound("Fx_Block");
				}
			}
			else {
				blockSave = false; //всех остальных блок не спас
				if (sti(attack.index) == GetMainCharacterIndex() && !bAtkTypeBreak) {// индикация пробития не от пробивающего
					setEffectText(enemy, XI_ConvertString("BlockBreak"), argb(255,255,255,255), GetCharCapNotifScale());
					PlayStereoSound("Fx_Block");
				}
			}
		}
	}
	if (bBlockedMusket && bAtkTypeBreak) blockSave = false;
	//пробитие блока конец
	
	//Реакция груп на атаку
	LAi_group_Attack(attack, enemy);
	//Начисление повреждений
	LAi_ApplyCharacterAttackDamage(attack, enemy, fencing_type, attackType, isBlocked, blockSave, aSkill);
	//Обновим цель сразу
	LAi_group_UpdateTargets(enemy);
	string func = enemy.chr_ai.type;
	if(func == "") return;
	func = "LAi_type_" + func + "_Attacked";
	call func(enemy, attack);
	func = "LAi_type_" + enemy.chr_ai.type + "_CharacterUpdate";
	call func(enemy, 0.0001);
}

//считает шанс пробития блока. aSkill - навык атакующего
int LAi_CalcBlockBreakChance(ref attack, string fencing_type, float aSkill)
{
	if (CheckAttribute(attack, "FullBlockPierce")) return 100; // LEO: 100% пробитие блока (Старший скелет кладбища; как у Стража грота)
	float valueBB = stf(attack.chr_ai.special.valueBB); //получаем значение пробития от оружки
	if (fencing_type == "FencingHeavy") {
		valueBB += 10.0 + 10.0 * aSkill; //коэфф от тяжа
	}
	valueBB += 5.0 * CheckCharacterPerk(attack, "HardHitter"); //тяж рука
	valueBB += 20.0 * CheckCharacterPerk(attack, "sliding"); //неотразимый
	valueBB += 3.0 * HasSubStr(attack.equip.blade, "topor"); //15+% если топор
	return MakeInt(valueBB);
}

void LAi_CharacterAimStats()
{
	aref chr = GetEventData();
	float perception = GetCharacterSPECIALSimple(chr, SPECIAL_P);
	SendMessage(chr, "lsff", MSG_CHARACTER_EX_MSG, "SetAimStats", perception, LAi_GetCharacterGunLevel(chr));
}

void LAi_CharacterFire()// обработчик события выстрела из огнестрельного оружия, ревью Assainisateur 01.2026
{
	LAi_CharacterFireImpl(false);
}

void LAi_CharacterAimFire()
{
	LAi_CharacterFireImpl(true);
}

bool LAi_IsAimedHeadshot(int hitKind, string bullet)
{
	return hitKind == 3 && bullet != "grapeshot" && bullet != "GunEchin" &&
		bullet != "powder_pellet" && bullet != "grenade";
}

void LAi_CharacterFireImpl(bool exactTrail)
{
	aref attack = GetEventData();
	aref enemy = GetEventData();
	float kDist = GetEventData();
	// LEO: -1 — прицельный промах, 0/1 — обычный выстрел, 2/3 — прицельное попадание в корпус/голову.
	int hitKind = GetEventData();
	float aimX, aimY, aimZ;
	if (exactTrail)
	{
		aimX = GetEventData();
		aimY = GetEventData();
		aimZ = GetEventData();
	}
	bool isFindedEnemy = hitKind > 0;
	if (hitKind != 1) bShotDodgedButHit = false;
	
	string sAttackerGun = attack.chr_ai.sgun;// экипированное огнестрельное оружие атакующего
	
	// заряд персонажа
	float charge = 0.0;
	if (!CheckAttribute(attack, "chr_ai.charge")) attack.chr_ai.charge = "0.0";
	else charge = stf(attack.chr_ai.charge) - 1.0;
	
	// считаем количество одновременно выстреленных зарядов
	int iMultiShot = 1;
	if (charge >= 1.0)// если ещё остались заряженные стволы
	{
		if (sAttackerGun == "pistol_grapebok")// особая механика Картечницы (она же "Гусиная Лапка") ... 
		{
			iMultiShot += MakeInt(charge);// ... выстреливает все заряженные стволы одновременно
			charge = 0.0;
		}
		else if (sAttackerGun == "howdah" || sAttackerGun == "mushket2")// механика стрельбы дуплетами из двустволок ...
		{
			if (sti(attack.index) == GetMainCharacterIndex())// ... доступна только для игрока
			{
				if (CheckAttribute(pchar,"ShootingMode"))
				{
					if (pchar.ShootingMode != "SingleShot")// если НЕ выбран режим стрельбы из одного ствола ...
					{
						iMultiShot += MakeInt(charge);// ... стреляем из всех заряженных стволов одновременно
						charge = 0.0;
						
						// стрельба дуплетами больно бьёт по нежным ручкам
						float fCurrentHP = stf(attack.chr_ai.hp);
						if (fCurrentHP > 2.0)// урон от дуплета не убивает стрелка – защита от дурака, чтоб случайных самоубийств не было
						{
							float fAttackerPistolSkill = LAi_GetCharacterGunLevel(attack);
							float fOuch = 0.1 - 0.05 * fAttackerPistolSkill;//           фиксированные 10% - 5% от среднего урона оружия, снижается при росте навыка "Пистолеты"
							fOuch = fOuch + (0.1 * (1.0 - FRAND(fAttackerPistolSkill)));// + случайные 10% - 0% от среднего урона оружия, снижается при росте навыка "Пистолеты" (в среднем)
							fOuch = fOuch * (stf(attack.chr_ai.dmggunmin) + stf(attack.chr_ai.dmggunmax)) * 0.5;
							if ((fCurrentHP - 1.0) < fOuch) fOuch = fCurrentHP - 1.0;// защита от самоубийств активирована
							LAi_ApplyCharacterDamage(attack, MakeInt(fOuch));
						}
					}
				}
			}
		}
	}
	
	// забираем выстреленные пули и порох
	string sBullet = LAi_GetCharacterBulletType(attack);
	RemoveItems(attack, sBullet, iMultiShot);
	string sGunPowder = LAi_GetCharacterGunpowderType(attack);
	if (sGunPowder != "") RemoveItems(attack, sGunPowder, iMultiShot);
	
	if (charge < 0.0) charge = 0.0;// странная проверка: по идее на данном этапе charge не может быть меньше нуля. ну, пусть будет, мало ли что
	
	attack.chr_ai.chargeprc = "1";// начинаем заряжание
	attack.chr_ai.charge = charge;
	if (sti(attack.index) == GetMainCharacterIndex()) attack.chargestage.(sAttackerGun) = charge;
	
	Lai_CharacterChangeEnergy(attack, -4 * iMultiShot);// жрем энергию за выстрел boal 20/06/06
	
	// логика автоматической смены используемого боеприпаса (при его исчерпании) для офицеров
	string sAttackAIType = attack.chr_ai.type;
	if (sAttackAIType == "officer")
	{
		if (sAttackerGun != "")
		{
			// определяем необходимость смены боеприпаса
			bool bNeedAmmoSwitch = false;
			if (GetCharacterItem(attack, sBullet) < 1) bNeedAmmoSwitch = true;// проверяем наличие пуль
			if (!bNeedAmmoSwitch && sGunPowder != "")
			{
				if (GetCharacterItem(attack, sGunPowder) < 1) bNeedAmmoSwitch = true;// проверяем наличие пороха
			}
			
			if (bNeedAmmoSwitch)// при необходимости пытаемся сменить боеприпас
			{
				ref rItm = ItemsFromID(sAttackerGun);
				aref rType;
				makearef(rType, rItm.type);
				int iNum = GetAttributesNum(rType);
				
				// Ищем альтернативные боеприпасы
				for (int i = 0; i < iNum; i++)
				{
					string sAttr = GetAttributeName(GetAttributeN(rType, i));
					string newBullet = rItm.type.(sAttr).bullet;
					
					// Пропускаем текущий тип и проверяем наличие
					if (newBullet != sBullet)
					{
						if (CheckCharacterItem(attack, newBullet))
						{
							if(LAi_SetCharacterUseBullet(attack, newBullet))
							{
								attack.equip.ammo = newBullet;
								// Устанавливаем минимальный заряд без сброса
								attack.chr_ai.charge = 0.0;
								attack.chr_ai.chargeprc = "1";
								
								
								break;
							}
						}
					}
				}
			}
		}
	}
	
	// если выстрел произведён без цели...
	if (!CheckAttribute(enemy, "index") && !isFindedEnemy)
	{
		// LEO: цели нет, но пуля всё равно куда-то улетела - ведём след по развороту стрелка
		if (exactTrail) LaunchGunTrailToImpact(attack, aimX, aimY, aimZ, sBullet, iMultiShot);
		else LaunchGunTrailFree(attack, sBullet, iMultiShot);
		// ... значит, делать здесь больше нечего, прерываем обработку скрипта
		return;
	}
	
	// Реакция на атаку
	LAi_ProcessAI_aux(attack, enemy);
	
	// LEO: координаты для дымного следа пули снимаем ДО урона: смертельный выстрел убивает цель
	// внутри LAi_ApplyCharacterFireDamage, и позицию мертвеца потом уже не спросить - след пропадал
	float fTrX, fTrY, fTrZ, fTrEX, fTrEY, fTrEZ;
	bool bTrailOk = false;
	if (exactTrail) LaunchGunTrailToImpact(attack, aimX, aimY, aimZ, sBullet, iMultiShot);
	else if (GunTrail_Enabled())
	{
		if (CheckAttribute(enemy, "index"))
		{
			GetCharacterPos(attack, &fTrX, &fTrY, &fTrZ);
			GetCharacterPos(enemy, &fTrEX, &fTrEY, &fTrEZ);
			bTrailOk = true;
		}
	}

	// наносим урон по основной цели
	int iMainTargetWasHit = 1;
	// если цель не отскочила, то стреляем в неё
	if (isFindedEnemy) iMainTargetWasHit = LAi_ApplyCharacterFireDamage(attack, enemy, kDist, true, iMultiShot, sBullet, hitKind);
	bShotDodgedButHit = false;
	if (exactTrail && iMainTargetWasHit == 3 && sti(attack.index) == GetMainCharacterIndex() && LAi_IsAimedHeadshot(hitKind, sBullet))
	{
		string headshotText = "Headshot";
		if (LanguageGetLanguage() == "russian") headshotText = "В голову!";
		SendMessage(loadedLocation, "lsfffsf", MSG_LOCATION_EX_MSG, "HeadshotText", aimX, aimY, aimZ, headshotText, GetCharCapNotifScale()*0.75);
	}

	// след ставим после расчёта попадания - промах должен увести линию мимо цели
	bool bTrailHit = isFindedEnemy && iMainTargetWasHit != 0;
	if (bTrailOk) LaunchGunTrail(fTrX, fTrY, fTrZ, fTrEX, fTrEY, fTrEZ, bTrailHit, sBullet, iMultiShot);

	string sFireMode = "bullet";
	switch (sBullet)// выбираем режим определения зоны поражения по боеприпасу
	{
		// два вида картечи
		case "grapeshot":
			sFireMode = "buckshot";
		break;
		case "GunEchin":
			sFireMode = "buckshot";
		break;
		// гранаты склопетты
		case "powder_pellet":
			sFireMode = "explosive";
		break;
		case "grenade":
			sFireMode = "explosive";
		break;
	}
	
	// обрабатываем промах в основную цель (если таковой произошёл)
	if (hitKind >= 0 && hitKind < 2 && iMainTargetWasHit < 2 && sFireMode != "buckshot")
	{
		// ищем новых кандидатов получить пулю
		float fContendersDist = 1.25 + iMainTargetWasHit * (1.0 - kDist) * 25.0;
		
		if (fContendersDist < 25.0)
		{
			int iContendersCount = FindNearCharacters(attack, 25.0, 30.0, 10.0, fContendersDist, true, true);
			
			// перебираем найденных кандидатов
			for (int k = 0; k < iContendersCount; k++)
			{
				int idx_k = -1;
				if (CheckAttribute(chrFindNearCharacters[k], "index")) idx_k = sti(chrFindNearCharacters[k].index);
				if (idx_k == -1) continue;// пропускаем цели без индекса
				
				if (CheckAttribute(enemy, "index"))
				{
					if (idx_k == sti(enemy.index)) continue;// пропускаем исходную цель, если она попала в список кандидатов
				}
				
				// считаем дистанцию до кандидата
				float kDistContender = 0.0;
				GetCharacterDistByChr3D(&Characters[idx_k], attack, &kDistContender);
				kDistContender = 1 - (kDistContender / 25.0);
				
				// стреляем в кандидата
				iMainTargetWasHit = LAi_ApplyCharacterFireDamage(attack, &Characters[idx_k], kDistContender, true, iMultiShot, sBullet, 0);
				if (iMainTargetWasHit > 1)// если попали – кандидат становится новой основной целью
				{
					makearef(enemy, Characters[idx_k]);
					kDist = kDistContender;
					LAi_ProcessAI_aux(attack, enemy);
					break;
				}
			}
		}
	}
	
	// дополнительные аудио-визуальные эффекты для выстрелов из огнестрела с параметром explosion в InitGunExt
	// типа должно дополнительно добавлять БАБАХА и кровищи
	if (CheckAttribute(attack, "chr_ai.explosion" ) && sti(attack.chr_ai.explosion) > 0)
	{
		float x, y, z;
		GetCharacterPos(enemy, &x, &y, &z);
		// LEO: было blood_shoot - схемы нет в ресурсах, движок ругался на каждый выстрел
		if (!LAi_IsBoneOnlySkeleton(enemy)) CreateParticleSystem("blood_expl_grenade", x, y, z, 0,1.0,0,0);
		PlayStereoSound("Sea Battles\cannon_fire_03.wav");
	}
	
	// наносим урон по вторичным целям в зоне поражения огнестрела с аттрибутом MultiDamage
	if (CheckAttribute(attack, "chr_ai.multidmg") && sti(attack.chr_ai.multidmg) > 0)
	{
		// ищем цели в заданной зоне поражения
		switch (sFireMode)// геометрия зоны поражения выбирается в зависимости от типа боеприпаса
		{
			case "buckshot":
				iCollateralTargetsCount = FindNearCharacters(attack, 25.0, 30.0, 10.0, 1.25, true, false);
				CollateralSnapshot();
			break;
			// мессага FindNearCharacters возвращает 0 при вызове от мёртвого "enemy"
			// то есть, если основная цель умирает, получив гранату в лицо, то в зоне поражения вокруг никто не получает урона
			// для решения этой проблемы определение вторичных целей от гранат было перенесено в LAi_fightparams.c/LAi_ApplyCharacterFireDamage
			// прямо перед нанесением урона основной цели (то есть, пока она ещё точно жива)
			//
			// решение неэлегантное, но альтернативы: либо 1) переписывать мессагу в двигле, либо 2) рефакторить LAi_ApplyCharacterFireDamage
			// 1) – может быть задачей на неопределённое будущее,
			// 2) – слишком много возни
		}
		
		// перебираем найденные цели ...
		for (int j = 0; j < iCollateralTargetsCount; j++)
		{
			int idx = CollateralIdx(j);// из снимка, а не из глобального массива - он мог протухнуть
			if (idx == -1) continue;// пропускаем цели без индекса
			
			if (CheckAttribute(enemy, "index"))
			{
				if (idx == sti(enemy.index)) continue;// пропускаем основную цель, если она попала во вторичные
			}
			
			// считаем дистанцию до вторичной цели
			float kDistCollateral = 0.0;
			switch (sFireMode)
			{
				case "buckshot":
					GetCharacterDistByChr3D(&Characters[idx], attack, &kDistCollateral);
				break;
				case "explosive":
					GetCharacterDistByChr3D(&Characters[idx], enemy, &kDistCollateral);
				break;
			}
			kDistCollateral = 1 - (kDistCollateral / 25.0);
			
			LAi_ApplyCharacterFireDamage(attack, &Characters[idx], kDistCollateral, false, iMultiShot, sBullet, 0);// ... и наносим по вторичной цели урон
			
		}
	}
	iCollateralTargetsCount = 0;
	DeleteAttribute(pchar, "questTemp.CollIdx");// снимок нужен был только на время цикла
	
	//Исполнение типа
	// текущая архитектура кода предполагает, что далее исполняются некие дополнительные действия в зависимости от класса персонажа атакующего
	// эти действия могут быть, вообще говоря, какими угодно в пределах возможностей скриптов – то есть, архитектура максимально гибкая
	// на практике же какие-либо действия вообще прописаны только для одного класса персонажей – самого игрока
	// и все эти действия сводятся к отображению остатка хп основной цели после выстрела
	if (iMainTargetWasHit > 1)// если попали в основную цель
	{
		if (sAttackAIType != "")
		{
			string func = "LAi_type_" + sAttackAIType + "_Fire";
			call func(attack, enemy, kDist, isFindedEnemy);
		}
	}
}

void LAi_ProcessAI_aux(aref attack, aref enemy)// вторичный метод, обрабатывающий реакции ИИ на выстрел из огнестрела, Assainisateur 01.2026
{
	LAi_group_Attack(attack, enemy);
	LAi_group_UpdateTargets(enemy);
	string sEnemyAIType = enemy.chr_ai.type;
	if (sEnemyAIType != "")
	{
		string func = "LAi_type_" + sEnemyAIType + "_Attacked";
		call func(enemy, attack);
		func = "LAi_type_" + sEnemyAIType + "_CharacterUpdate";
		call func(enemy, 0.0001);
	}
}

bool LAi_tmp_return_bool;
bool LAi_CharacterIsFire()
{
	aref chr = GetEventData();
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_IsFire";
	LAi_tmp_return_bool = call func(chr);
	return LAi_tmp_return_bool;
}

bool LAi_CharacterIsFight()
{
	aref chr = GetEventData();
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_IsFight";
	LAi_tmp_return_bool = call func(chr);
	return LAi_tmp_return_bool;
}

//==========================================================================================
//Escape events
//==========================================================================================

void LAi_CharacterEscapeSlide()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterEscapeSlide") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_EscapeSlide";
	call func(chr);
}

//==========================================================================================
//Collision events
//==========================================================================================

void LAi_CharacterColThreshold()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterColThreshold") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_ColThreshold";
	call func(chr);
}

//==========================================================================================
//Animation events
//==========================================================================================

void LAi_Character_EndAction()
{
	//Параметры
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_Character_EndAction") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_EndAction";
	call func(chr);
}


//==========================================================================================
//Dead event
//==========================================================================================

void LAi_Character_Dead_Process(aref chr)
{
	// Рвём диалог, только если погиб его участник, а не любой персонаж в локации
	if (dialogRun)
	{
		int iDead = sti(chr.index);
		if (iDead == nMainCharacterIndex || iDead == sti(CharacterRef.index)) DialogExit();
	}
	LAi_CharacterLogoff(chr);
	if(sti(chr.index) != nMainCharacterIndex)
	{
		// LEO: погиб офицер-контрник абордажа - снять звезду. BoardElite удаляем ПЕРВЫМ, иначе гард в UnmarkCharacter вернёт знак.
		if (CheckAttribute(chr, "BoardElite"))
		{
			DeleteAttribute(chr, "BoardElite");
			UnmarkCharacter(chr);
		}
		// LEO: погиб главарь/мини-босс со звездой (стычка в джунглях, Старший на кладбище) - снять звезду.
		// bGangLeader удаляем ПЕРВЫМ, иначе гард в UnmarkCharacter вернёт знак.
		if (CheckAttribute(chr, "bGangLeader"))
		{
			DeleteAttribute(chr, "bGangLeader");
			UnmarkCharacter(chr);
		}
		//navy --> смерть ПГГ
		if (CheckAttribute(chr, "PGGAi"))
		{
			PGG_CheckDead(chr);
		}
		//navy <--
		//Подождём, а затем удалим персонажа
		// boal трупы не убирать
        if (MOD_DEAD_CLEAR == "On" && !CheckAttribute(chr, "DontClearDead"))
        {
            PostEvent("LAi_event_Character_Dead", 500 * 1000, "i", chr);
		}
        // убираем совместителя в CharacterDeadProcess
		
		//Реинкарнируем
		LAi_GenerateFantomFromMe(chr);
		//Посмотрим группу
		LAi_group_CheckGroupQuest(chr);
	}
	else
	{
		//Доигрались...
		if(!LAi_IsBoardingProcess())
		{
			if (loadedLocation.type == "town")
			PostEvent("LAi_event_GameOver", 5000, "s", "town");
			else
			{
				if (loadedLocation.type == "underwater")
				PostEvent("LAi_event_GameOver", 5000, "s", "sea");
				else
				PostEvent("LAi_event_GameOver", 5000, "s", "land");
			}
		}
		else
		{
			PostEvent("LAi_event_GameOver", 5000, "s", "boarding");
		}
	}
}

#event_handler("LAi_event_Character_Dead", "LAi_Character_Dead_Event");
void LAi_Character_Dead_Event()
{
	aref chr = GetEventData();
	if (IsEntity(chr))  //fix
	{
		// boal dead can be searched 14.12.2003 -->
		Dead_DelLoginedCharacter(chr); // трем
		// boal dead can be searched 14.12.2003 <--
		CharacterExitFromLocation(chr);
	}
}

#event_handler("LAi_event_GameOver", "LAi_GameOver");
void LAi_GameOver()
{
	string str = GetEventData();
	GameOver(str);
}



//==========================================================================================
//Internal events
//==========================================================================================

//------------------------------------------------------------------------------------------
//Сообщение об окончании работы темплейта
//------------------------------------------------------------------------------------------

void LAi_Character_TemplateComplite(aref chr, string tmplName)
{
	
	int index = sti(chr.index);
	PostEvent("LAi_event_Character_TemplateComplite", 1, "ls", index, tmplName);
	
}

#event_handler("LAi_event_Character_TemplateComplite", "LAi_Character_TemplateComplite_Event");
void LAi_Character_TemplateComplite_Event()
{
	int index = GetEventData();
	string tmpl = GetEventData();
	ref chr = &Characters[index];
	// boal fix -->
	string func = "";
	if ( CheckAttribute(chr, "chr_ai.type"))
	{
	    func = chr.chr_ai.type;
	}
	// boal fix <--
	if(func != "")
	{
		func = "LAi_type_" + func + "_TemplateComplite";
		call func(chr, tmpl);
	}
}

//------------------------------------------------------------------------------------------
//Запрос локатора
//------------------------------------------------------------------------------------------

void LAi_Character_FreeLocator(aref chr, string group, string locator)
{
	int index = sti(chr.index);
	PostEvent("LAi_event_Character_FreePos", 1, "lss", index, group, locator);
}

#event_handler("LAi_event_Character_FreePos", "LAi_Character_FreePos_Event");
void LAi_Character_FreePos_Event()
{
	int index = GetEventData();
	string group = GetEventData();
	string locator = GetEventData();
	for(int i = 0; i < LAi_numloginedcharacters; i++)
	{
		int idx = LAi_loginedcharacters[i];
		if(idx >= 0)
		{
			if(idx != index)
			{
				float dist;
				if(GetCharacterDistByLoc(&Characters[idx], group, locator, &dist))
				{
					if(dist < 0.8)
					{
						string func = Characters[idx].chr_ai.tmpl;
						if(func == "") return;
						//Исполнение
						func = "LAi_tmpl_" + func + "_FreePos";
						call func(&Characters[idx], &Characters[index]);
					}
				}
			}
		}
	}
}


//------------------------------------------------------------------------------------------
//Запрос на диалог
//------------------------------------------------------------------------------------------

void LAi_Character_NeedDialog(aref chr, aref by)
{
	if(IsEntity(by))
	{
		string func = chr.chr_ai.type;
		if(func != "")
		{
			func = "LAi_type_" + func + "_NeedDialog";
			call func(by, chr);
		}
	}
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_Character_CanDialog(aref chr, aref by)
{
	if(IsEntity(by))
	{
		bool isDisable = false;
		if(CheckAttribute(chr, "chr_ai.disableDlg"))
		{
			if(sti(chr.chr_ai.disableDlg) != 0) isDisable = true;
		}
		if(isDisable == false)
		{
			string func = by.chr_ai.type;
			if(func != "")
			{
				func = "LAi_type_" + func + "_CanDialog";
				return call func(by, chr);
			}
		}
	}
	return false;
}

//Начать диалог
void LAi_Character_StartDialog(aref chr, aref by)
{
	if(IsEntity(by))
	{
		string func = by.chr_ai.type;
		if(func != "")
		{
			func = "LAi_type_" + func + "_StartDialog";
			call func(by, chr);
		}
	}
}

//Закончить диалог
void LAi_Character_EndDialog(aref chr, aref by)
{
	if(IsEntity(by))
	{
		string func = by.chr_ai.type;
		if(func != "")
		{
			func = "LAi_type_" + func + "_EndDialog";
			call func(by, chr);
		}
	}
}
// Привязать к/отвязать от модели персонажа какую нить модель (предмет)
void LAi_IntegratedMusket()
{
	aref chr = GetEventData();
	int draw = GetEventData();
	bool playAmmoSwitch = draw != 0 && IsMainCharacter(chr) && !IsIntegratedMusketDrawn(chr);
	SetIntegratedMusketDrawn(chr, draw != 0);
	if (playAmmoSwitch && IsIntegratedMusketDrawn(chr)) PlaySound("People Fight\reload1.wav");
}

void LAi_CharacterItemAction()
{
	aref chr = GetEventData();
	string sActionName = GetEventData();
	int nItemIndex = GetEventData();
	
	switch(sActionName)
	{
		case "reset": LAi_UntieItemFromCharacter(chr,nItemIndex); break;
		case "set": LAi_TieItemToCharacter(chr,nItemIndex); break;
	}
}
void LAi_UntieItemFromCharacter(aref chr, int nItemIndex)
{
	if( nItemIndex<0 ) return;
	SendMessage(chr, "lsl", MSG_CHARACTER_EX_MSG, "UntieItem", nItemIndex);
}
void LAi_TieItemToCharacter(aref chr, int nItemIndex)
{
	if( nItemIndex<0 ) return;
	
	string sModel = "HandsItems\cup";
	string sLocator = "Saber_hand";
	
	string sitm = "TiedItems.itm"+nItemIndex;
	if( CheckAttribute(chr,sitm) )
	{
		if( CheckAttribute(chr,sitm+".model") ) {
			sModel = chr.(sitm).model;
		}
		if( CheckAttribute(chr,sitm+".locator") ) {
			sLocator = chr.(sitm).locator;
		}
	}
	
	SendMessage(chr, "lslss", MSG_CHARACTER_EX_MSG, "TieItem", nItemIndex, sModel, sLocator);
}
