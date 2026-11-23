/////////  новый файл под нужды ВМЛ, код К3 не совместим - весь потерт  ///////////
// boal абордаж офицеров 27.01.2004 -->
void CompaniontDefaultCapturedDecision(ref rCompanion, ref rEnemy)
{
	if (CheckAttribute(PChar, "CompanionsLeaveCapturesDrift"))
	{
		ref sld = GetCharacter(NPC_GenerateCharacter("_DriftCap_" + rEnemy.id, "off_hol_2", "man", "man", 1, sti(rEnemy.nation), 0, true));
		DeleteAttribute(sld, "ship");
		sld.ship = "";

		aref arTo, arFrom;
		makearef(arTo, sld.ship);
		makearef(arFrom, rEnemy.Ship);
		CopyAttributes(arTo, arFrom);

		sld.AlwaysFriend = true;
		sld.Abordage.Enable = false;
		sld.Dialog.Filename = "Capitans_dialog.c";
		sld.greeting = "Gr_Commander";
		sld.DeckDialogNode = "Go_away";

		SeaAI_SetDriftCaptain(sld, rEnemy);
		PGG_Q1KeepDriftPrize(sld);
		rEnemy.location = "none";

		ShipTakenFree(sti(rEnemy.index), KILL_BY_ABORDAGE, sti(rCompanion.index));
		PostEvent("evntQuestsCheck", 400);
		UpdateRelations();
		RefreshBattleInterface();
		CheckQuestAboardCabinSituation(sld);
		DoQuestCheckDelay("NationUpdate", 1.5);
	}
	else
	{
		if (CheckChanceOfBetterShip(rCompanion, rEnemy))
		{
			SeaExchangeCharactersShips(rCompanion, rEnemy, true, true);
		}
		DoTakeAbordageGoods(rCompanion, rEnemy);
		ShipDead(sti(rEnemy.index), KILL_BY_ABORDAGE, sti(rCompanion.index));
	}
}

// LEO: ===== вариации исхода авто-абордажа ИИ (НЕ компаньонов): топит / присвоение нации / дрейф =====
// LEO: вызывается из Ship_AutoAbordage для пары победитель/проигравший среди ИИ-кораблей третьей стороны.
// LEO: "флаг тип 'имя'" одной строкой для иконочного боевого лога абордажа - единый стиль с логом потопления/авто-абордажа ИИ
string AbordLogShipPart(ref rChar, bool bFlagFirst)
{
	ref rbs = GetRealShip(sti(rChar.Ship.Type)); // LEO: Storm не даёт GetRealShip(...).BaseName инлайн - сначала в ref
	string sShip = XI_ConvertString(rbs.BaseName) + " '" + rChar.Ship.Name + "'";
	if (bFlagFirst) return TransformNationToIcon(sti(rChar.nation)) + " " + sShip; // LEO: флаг победителя СЛЕВА (без ᶑ/Ẹ - единый стиль)
	return sShip + " " + TransformNationToIcon(sti(rChar.nation));                  // LEO: флаг побеждённого в КОНЕЦ
}

#define AIABORD_TEST_ALWAYS_DRIFT  0   // LEO: ТЕСТ - ИИ всегда бросает приз в дрейф (для проверки ловушек). Боевое значение: 0

// LEO: можно ли присвоить приз - хватает команды на ОБА корабля + в море + есть боевая группа
bool AIAbord_CanNationalize(ref rWinner, ref rLoser)
{
	if (!bSeaActive) return false;
	if (!CheckAttribute(rWinner, "SeaAI.Group.Name")) return false;
	if ((GetCrewQuantity(rWinner) - GetMinCrewQuantity(rLoser)) < GetMinCrewQuantity(rWinner)) return false;
	return true;
}

// LEO: потопить приз БЕЗ отдельной строки ShipDead (исход пишем одной строкой ниже)
void AIAbord_SinkNoLog(ref rWinner, ref rLoser)
{
	DoTakeAbordageGoods(rWinner, rLoser);
	g_AIAbordNoSinkLog = 1;
	ShipDead(sti(rLoser.index), KILL_BY_ABORDAGE, sti(rWinner.index));
	g_AIAbordNoSinkLog = 0;
}

void AIWinAbordageDecision(ref rWinner, ref rLoser)
{
	// LEO: суда для ОДНОСТРОЧНОГО лога фиксируем ДО обмена кораблём - показываем их на момент абордажа
	int iWT = sti(rWinner.Ship.Type); string sWN = rWinner.Ship.Name; int iWNa = sti(rWinner.nation);
	int iLT = sti(rLoser.Ship.Type);  string sLN = rLoser.Ship.Name;  int iLNa = sti(rLoser.nation);

	// LEO: захватчик пересаживается на ЛУЧШИЙ из двух (вариант A); _showLog=false - имя капитана в лог не пишем
	if (CheckChanceOfBetterShip(rWinner, rLoser)) SeaExchangeCharactersShips(rWinner, rLoser, false, true);

	string sOutcome = ""; // иконка исхода для однострочного лога

	if (AIABORD_TEST_ALWAYS_DRIFT == 1) // LEO: ТЕСТ - форсим дрейф на 100% (проверка ловушек)
	{
		AILeaveCapturedShipDrift(rWinner, rLoser);
		sOutcome = "ś";
	}
	else
	{
		if (sti(rWinner.nation) == PIRATE)
		{
			// LEO: ПИРАТЫ не топят/не бросают - присваивают приз, если хватает команды на оба; иначе грабят и топят
			if (AIAbord_CanNationalize(rWinner, rLoser)) { AINationalizeCapturedShip(rWinner, rLoser); sOutcome = "ţ " + TransformNationToIcon(iWNa); }
			else { AIAbord_SinkNoLog(rWinner, rLoser); sOutcome = "È"; }
		}
		else
		{
			int iRoll = (dRand(99) + sti(rLoser.index)) % 100; // LEO: dRand (сейв-лоад-устойчиво) + индекс жертвы
			if (iRoll < 40) // 40% - топит
			{
				AIAbord_SinkNoLog(rWinner, rLoser); sOutcome = "È";
			}
			else
			{
				if (iRoll < 70) // 30% - присвоение (или дрейф/топь, если некому в команду)
				{
					if (AIAbord_CanNationalize(rWinner, rLoser)) { AINationalizeCapturedShip(rWinner, rLoser); sOutcome = "ţ " + TransformNationToIcon(iWNa); }
					else { if (bSeaActive) { AILeaveCapturedShipDrift(rWinner, rLoser); sOutcome = "ś"; } else { AIAbord_SinkNoLog(rWinner, rLoser); sOutcome = "È"; } }
				}
				else // 30% - дрейф
				{
					AILeaveCapturedShipDrift(rWinner, rLoser); sOutcome = "ś";
				}
			}
		}
	}

	// LEO: ОДНА строка - [флаг победителя] победитель Ê жертва [флаг жертвы] Ø(стрелка) [исход]. Суда на момент абордажа (до обмена). Ø - иконка-глиф стрелки (480,224 в шрифте; U+2192 движок не резолвит, потому Ø).
	ref rbsW = GetRealShip(iWT);
	ref rbsL = GetRealShip(iLT);
	Log_SetStringToLog(TransformNationToIcon(iWNa) + " " + XI_ConvertString(rbsW.BaseName) + " '" + sWN + "' Ê " + XI_ConvertString(rbsL.BaseName) + " '" + sLN + "' " + TransformNationToIcon(iLNa) + " Ø " + sOutcome);
}

// LEO: ванильная судьба офицера-КОМПАНЬОНА игрока при ПОТЕРЕ корабля (враг захватил: нацио/дрейф). 1:1 как в ShipDead при потоплении (AIShip.c ~2776):
// перк ShipEscape → офицер выживает (Survive_In_SeaOfficer) + снятие из эскадры + минус со счётчика контрактов; HalfImmortal → шлюпка + снятие + минус;
// иначе → просто снятие из эскадры (офицер потерян/пленён, но не застревает «компаньоном с кораблём»). Для ИИ-vs-ИИ (не наш компаньон) - ничего.
void ApplyCompanionLossFate(ref rComp)
{
	if (!IsCompanion(rComp)) return; // не наш компаньон (захват ИИ у ИИ) - судьбу эскадры не трогаем
	// LEO: наличие узла PGGAi, а НЕ значение IsPGG - тот же фикс, что в AIShip.c (ShipDead): у бывшего ПГГ
	//   IsPGG стоит в false, гарда открывалась, и подобранная шлюпка клонировала его в новый слот.
	bool bIsNotAIPGG = true;
	if (CheckAttribute(rComp, "PGGAi")) bIsNotAIPGG = false;
	if (bIsNotAIPGG && CheckOfficersPerk(rComp, "ShipEscape") && GetRemovable(rComp)) // спаслись
	{
		AISeaGoods_AddGood(rComp, "boat", "lo_boat", 1000.0, 1); // LEO: офицер уходит на шлюпке (в ванильном ShipDead строка закомментирована //homo; перк "спасение на шлюпке" => шлюпку видно)
		RemoveCharacterCompanion(pchar, rComp);
		pchar.GenQuest.Survive_In_SeaOfficerIdx = rComp.index;
		DoQuestCheckDelay("Survive_In_SeaOfficer", 1.0);
		if (CheckForContractImmortal(rComp)) pchar.Option_ImmortalOfficers = sti(sti(pchar.Option_ImmortalOfficers) - 1);
		return;
	}
	if (bIsNotAIPGG && CheckAttribute(rComp, "HalfImmortal") && GetRemovable(rComp))
	{
		bool bOkSave = bHalfImmortalPgg && CheckAttribute(rComp, "ImmortalOfficer");
		if (bOkSave || bHalfImmortalPgg == false)
		{
			AISeaGoods_AddGood(rComp, "boat", "lo_boat", 1000.0, 1);
			RemoveCharacterCompanion(pchar, rComp);
			pchar.Option_ImmortalOfficers = sti(sti(pchar.Option_ImmortalOfficers) - 1);
			return;
		}
	}
	// нет спасения - офицер потерян (захвачен/погиб), но снимаем из эскадры (автоснятие, чтобы не застревал)
	RemoveCharacterCompanion(pchar, rComp);
}

// LEO: присвоение трофея к нации захватчика - фантом-офицер на корпус, команда переводится С КОРАБЛЯ ЗАХВАТЧИКА (не из воздуха),
// LEO: корабль остаётся боевым и встаёт в группу захватчика. Не хватает команды на оба / некуда влить - фолбэк в дрейф (вне боя топим).
void AINationalizeCapturedShip(ref rWinner, ref rLoser)
{
	// LEO: призовую команду берём ИЗ команды захватчика - нужен минимум для призового (худшего) корабля
	int iPrizeCrew = GetMinCrewQuantity(rLoser);
	// LEO: хватит ли у захватчика людей на ОБА корабля (после передачи у него должно остаться не меньше его минимума)
	bool bCanCrewBoth = (GetCrewQuantity(rWinner) - iPrizeCrew) >= GetMinCrewQuantity(rWinner);
	if (!bSeaActive || !CheckAttribute(rWinner, "SeaAI.Group.Name") || !bCanCrewBoth)
	{
		// LEO: не присвоить (некуда влить / не хватает команды на оба) - оставляем в дрейф; вне боя топим
		if (bSeaActive)
		{
			AILeaveCapturedShipDrift(rWinner, rLoser);
		}
		else
		{
			DoTakeAbordageGoods(rWinner, rLoser);
			ShipDead(sti(rLoser.index), KILL_BY_ABORDAGE, sti(rWinner.index)); // LEO: строку потопления (иконки) пишет сам ShipDead
		}
		return;
	}
	int iNation = sti(rWinner.nation);
	// LEO: лог исхода пишется ОДНОЙ строкой в AIWinAbordageDecision (здесь не дублируем)
	// LEO: офицер-фантом нации захватчика (ранг от проигравшего - т.е. под класс корабля)
	ref off = GetCharacter(NPC_GenerateCharacter("_PrizeCap_" + rLoser.id, "off_hol_2", "man", "man", sti(rLoser.rank), iNation, 0, true));
	DeleteAttribute(off, "ship");
	off.ship = "";
	aref arTo, arFrom;
	makearef(arTo, off.ship);
	makearef(arFrom, rLoser.Ship);
	CopyAttributes(arTo, arFrom); // LEO: корпус захваченного (худшего) корабля переходит офицеру
	off.nation = iNation;
	off.Abordage.Enable = true; // LEO: трофей нации остаётся абордабельным/боевым
	off.Dialog.Filename = "Capitans_dialog.c";
	off.greeting = "Gr_Commander";

	SeaAI_SetOfficer2ShipAfterAbordage(off, rLoser); // LEO: передать физический корабль офицеру (он суёт его в PLAYER_GROUP)
	// LEO: команду призу переводим С захватчика (не из воздуха): у него снимаем, офицеру выдаём
	SetCrewQuantity(rWinner, GetCrewQuantity(rWinner) - iPrizeCrew);
	SetCrewQuantity(off, iPrizeCrew);

	// LEO: убрать из PLAYER_GROUP (туда сунул SeaAI_SetOfficer2ShipAfterAbordage)
	Group_DelCharacter(PLAYER_GROUP, off.id);
	// LEO: цель = та же вражеская группа, что бьёт захватчик
	string sWinGrp = rWinner.SeaAI.Group.Name;
	ref rWinGroup = Group_FindOrCreateGroup(sWinGrp);
	string sEnemyGroup = "";
	if (CheckAttribute(rWinGroup, "Task.Target")) sEnemyGroup = rWinGroup.Task.Target;
	// LEO: трофею - СВОЯ группа. КЛЮЧЕВОЕ: ChangeCharacterShipGroup шлёт движку AI_MESSAGE_SHIP_CHANGE_GROUP -
	// LEO: без него движок держит трофей в PLAYER_GROUP (своя кэш-матрица отношений) и он не воюет. Паттерн как в AIShip.c:5462.
	string sPrizeGroup = "Prize_" + off.index;
	off.SeaAI.Group.Name = sPrizeGroup;
	DeleteAttribute(off, "relation.UseOtherCharacter");
	Group_ChangeCharacter(sPrizeGroup, off.id);
	Group_SetGroupCommander(sPrizeGroup, off.id);
	ChangeCharacterShipGroup(off, sPrizeGroup); // LEO: принудительный пересчёт группы/отношений трофея в движке (тот самый недостающий вызов)
	// LEO: трофей наследует отношение захватчика ко ВСЕМ кораблям боя; ИГРОКА пропускаем -
	// LEO: пусть работает нацио-откат (иначе явная запись = прицел зелёный, а маркер по нации красный = рассинхрон).
	int kRel, ksRel, nEnemy = 0;
	for (kRel = 0; kRel < iNumShips; kRel++)
	{
		ksRel = Ships[kRel];
		if (ksRel < 0 || ksRel == sti(off.index) || ksRel == sti(rWinner.index) || ksRel == GetMainCharacterIndex()) continue;
		int iRel = GetRelation(sti(rWinner.index), ksRel);
		SetCharacterRelationBoth(sti(off.index), ksRel, iRel);
		if (iRel == RELATION_ENEMY) nEnemy++;
	}
	SetCharacterRelationBoth(sti(off.index), sti(rWinner.index), RELATION_FRIEND); // LEO: дружелюбен к захватчику
	// LEO: задача - группой по вражеской группе + прямой приказ на ближайшего враждебного (если групповая цель тонкая)
	if (sEnemyGroup != "") Group_SetTaskAttack(sPrizeGroup, sEnemyGroup);
	for (kRel = 0; kRel < iNumShips; kRel++)
	{
		ksRel = Ships[kRel];
		if (ksRel < 0 || ksRel == sti(off.index)) continue;
		if (GetRelation(sti(off.index), ksRel) == RELATION_ENEMY) { Ship_SetTaskAttack(SECONDARY_TASK, sti(off.index), ksRel); break; }
	}
	off.AnalizeShips = true;  // LEO: трофей активно ищет/атакует врагов
	off.PrizeNoFlee = true;   // LEO: приз бьётся в группе захватчика, не убегает (скелет-команда + одиночка vs флот ГГ трипали бы бегство)

	rLoser.location = "none";
	ShipTakenFree(sti(rLoser.index), KILL_BY_ABORDAGE, sti(rWinner.index)); // LEO: убрать старого кэпа (корабль уже у офицера); лог присвоения уже записан выше
	ApplyCompanionLossFate(rLoser); // LEO: если проигравший - наш компаньон, ванильная судьба офицера + снятие из эскадры (иначе застревал бы)
	Event("eSwitchPerks", "l", sti(off.index));
	Event(SHIP_UPDATE_PARAMETERS, "lf", sti(off.index), 1.0);
	UpdateRelations();         // LEO: ОДИН раз в конце - движок пересчитывает матрицу из согласованного состояния
	Ship_FlagRefresh(off);     // LEO: флаг под нацию (после ре-регистрации - чтобы маркер совпал)
	RefreshBattleInterface();
}

// LEO: оставить захваченный корабль в дрейф (как ГГ/компаньоны), предварительно забрав весь товар.
// LEO: механика дрейфа 1:1 как в CompaniontDefaultCapturedDecision (проверенный код).
void AILeaveCapturedShipDrift(ref rWinner, ref rLoser)
{
	DoTakeAbordageGoods(rWinner, rLoser); // LEO: захватчик берёт до 3 самых дорогих товаров (сколько влезет в трюм)
	// LEO: лог исхода (дрейф) пишется ОДНОЙ строкой в AIWinAbordageDecision (здесь не дублируем)
	DeleteAttribute(rLoser, "Ship.Cargo.Goods"); // LEO: остаток груза стираем - чтобы на дрейфующем корпусе не осталось халявы игроку
	ref sld = GetCharacter(NPC_GenerateCharacter("_DriftCap_" + rLoser.id, "off_hol_2", "man", "man", 1, sti(rLoser.nation), 0, true));
	DeleteAttribute(sld, "ship");
	sld.ship = "";
	aref arTo2, arFrom2;
	makearef(arTo2, sld.ship);
	makearef(arFrom2, rLoser.Ship);
	CopyAttributes(arTo2, arFrom2);
	sld.AlwaysFriend = true;
	sld.Abordage.Enable = false;
	sld.Dialog.Filename = "Capitans_dialog.c";
	sld.greeting = "Gr_Commander";
	sld.DeckDialogNode = "Go_away";
	AICapturedTrap_ArmDrift(sld); // LEO: ловушка (взрыв/эпидемия) на брошенном ИИ дрейф-призе - см. GenQuests.c
	SeaAI_SetDriftCaptain(sld, rLoser);
	rLoser.location = "none";
	ShipTakenFree(sti(rLoser.index), KILL_BY_ABORDAGE, sti(rWinner.index));
	ApplyCompanionLossFate(rLoser); // LEO: если проигравший - наш компаньон, ванильная судьба офицера + снятие из эскадры (иначе застревал бы)
	PostEvent("evntQuestsCheck", 400);
	UpdateRelations();
	RefreshBattleInterface();
}

// LEO: Экипажи уже понесли потери в бою; здесь только статистика и судьба приза.
void AutoAbord_ResolveBoarding(ref rWinner, ref rLoser, int deadCrew, int lostCrew, string sLostA, string sLostB)
{
	bool bWinnerCompanion = IsCompanion(rWinner);
	bool bLoserCompanion  = IsCompanion(rLoser);

	// лог: наш проигравший компаньон — своей строкой; захват компаньоном чужого — иконочной строкой;
	// ИИ-vs-ИИ пишется ОДНОЙ строкой внутри AIWinAbordageDecision.
	if (bLoserCompanion)
	{
		Log_SetStringToLog(sLostA + rLoser.Ship.Name + sLostB);
	}
	else
	{
		if (bWinnerCompanion) Log_SetStringToLog(AbordLogShipPart(rWinner, true) + " Ê " + AbordLogShipPart(rLoser, false));
	}

	if (bWinnerCompanion) Statistic_AddValue(pchar, "Sailors_dead", deadCrew);
	if (bLoserCompanion)  Statistic_AddValue(pchar, "Sailors_dead", lostCrew);
	LAi_SetCurHP(rLoser, 0.0);

	if (bWinnerCompanion)
	{
		RemoveCharacterGoodsSelf(rWinner, GOOD_WEAPON, deadCrew);
		if (GetRemovable(rWinner) && GetShipRemovable(rWinner))
		{
			LaunchRansackMain(rWinner, rLoser, "companion_captured");
		}
		else
		{
			CompaniontDefaultCapturedDecision(rWinner, rLoser);
		}
	}
	else
	{
		AIWinAbordageDecision(rWinner, rLoser); // LEO: вариации исхода авто-абордажа ИИ — топит / присвоение нации / дрейф
	}
}

// авто-абордаж: решение о сцепке и разрешение боя «команда против команды». boal 27.01.2004
bool Ship_AutoAbordage(ref rCharacter, float fMinEnemyDistance)
{
	if (Boarding_Busy(rCharacter) || Boarding_RetryLeft(rCharacter) > 0) return false;
	if (fMinEnemyDistance >= 150.0) return false;

	int   bGrapplingProfessional  = sti(rCharacter.TmpPerks.GrapplingProfessional);
	float fOurGrappling           = 10 * stf(rCharacter.TmpSkill.Grappling);
	float fOurMaxAbordageDistance = 15 + 7 * (bGrapplingProfessional + fOurGrappling);

	// navy 18.02.08 — запрет абордажа таском
	if (CheckAttribute(rCharacter, "Tasks.CanBoarding") && !sti(rCharacter.Tasks.CanBoarding)) return false;
	if (!CheckAttribute(rCharacter, "SeaAI.Task.Target")) return false;

	if (rCharacter.SeaAI.Task.Target == "") return false;
	int targetIndex = sti(rCharacter.SeaAI.Task.Target);
	if (targetIndex < 0 || targetIndex >= TOTAL_CHARACTERS) return false;
	ref rEnemyCharacter = GetCharacter(targetIndex);
	if (LAi_IsDead(rEnemyCharacter)) return false;
	if (!CheckAttribute(rEnemyCharacter, "Ship.BoxSize.x")) return false;
	// LEO: Проверяем расстояние до бортов выбранной цели, а не до центра ближайшего врага.
	float reach = retMin(70.0, fOurMaxAbordageDistance) +
		(stf(rCharacter.Ship.BoxSize.x) + stf(rEnemyCharacter.Ship.BoxSize.x)) * 0.5;
	if (Ship_GetDistance2DRel(rCharacter, rEnemyCharacter) >= reach * reach) return false;

	int rCharIdx = sti(rCharacter.index);
	int rEnemyCharIdx = sti(rEnemyCharacter.index);
	int iRelation = SeaAI_GetRelation(rCharIdx, rEnemyCharIdx);

	if (!Character_IsAbordageEnable(rEnemyCharacter) || rEnemyCharIdx == GetMainCharacterIndex()) return false;
	if (iRelation != RELATION_ENEMY) return false;

	// LEO: квестовые/уникальные корабли НЕ берут на абордаж компаньонов ГГ — иначе дюп уникального корабля
	// (квестовый инициатор = NonRemovable капитан и/или ShipRemovable=false; те же признаки используются ниже при дележе призов)
	if (IsCompanion(rEnemyCharacter))
	{
		if (!GetRemovable(rCharacter) || !GetShipRemovable(rCharacter)) return false;
	}

	// порог решимости: не лезем, если своей команды меньше 60% вражеской
	int rCharCrew = sti(rCharacter.Ship.Crew.Quantity);
	int rEnemyCharCrew = sti(rEnemyCharacter.Ship.Crew.Quantity);
	if (rCharCrew < (stf(rEnemyCharCrew) * 0.6)) return false;

	// боевая сила нашей команды (учёт оружия — только для компаньона, у ИИ упрощённо)
	float fOurFencing = stf(rCharacter.TmpSkill.Grappling);
	float fOurCrewFencing = (0.1 + fOurFencing * stf(rCharacter.Ship.Crew.Quantity)*GetCrewExp(rCharacter, "Soldiers"));
	bool bIsCharCompanion = IsCompanion(rCharacter);   // #20180126-01
	bool bIsEnemyCompanion = IsCompanion(rEnemyCharacter);
	if (bIsCharCompanion)
	{
		fOurCrewFencing = (0.1 + fOurFencing * GetWeaponCrew(rCharacter, rCharCrew)*GetCrewExp(rCharacter, "Soldiers"));
	}
	float fMorale = GetCharacterCrewMorale(rCharacter);
	fOurCrewFencing = fOurCrewFencing * (0.5 + fMorale / MORALE_MAX);

	// боевая сила вражеской команды
	float fEnFencing = stf(rEnemyCharacter.TmpSkill.Grappling);
	float fEnCrewFencing = (0.1 + fEnFencing * stf(rEnemyCharCrew) *GetCrewExp(rEnemyCharacter, "Soldiers"));
	if (bIsEnemyCompanion)
	{
		fEnCrewFencing = (0.1 + fEnFencing * GetWeaponCrew(rEnemyCharacter, rEnemyCharCrew)*GetCrewExp(rEnemyCharacter, "Soldiers") );
	}
	fMorale  = GetCharacterCrewMorale(rEnemyCharacter);
	fEnCrewFencing = fEnCrewFencing * (0.5 + fMorale / MORALE_MAX);

	if (rCharacter.id == "SilverSurferCaptain") fOurCrewFencing *= 10;      // Серебряный Сёрфер
	if (rEnemyCharacter.id == "SilverSurferCaptain") fEnCrewFencing *= 10;
	return Boarding_Begin(rCharacter, rEnemyCharacter, false, fOurCrewFencing, fEnCrewFencing);
}

//navy -->
//обменять кораблями двух персонажей, с сохранением груза
void SeaExchangeCharactersShips(ref rOneChr, ref rSecChr, bool _showLog, bool _swapCargo)
{
	if (SeaTowOperations_BlockSwap(rOneChr, rSecChr)) return;
	int     nMyCrew;
	aref    arCargoOne, arCargoSec;
	object	oTmp;

	//команда первого (основного) перса...
	nMyCrew = GetCrewQuantity(rOneChr);
    float fMor = stf(rOneChr.ship.crew.morale);
    makearef(arCargoOne, rOneChr.Ship.Crew.Exp);
	CopyAttributes(&oTmp, arCargoOne); // опыт

	//меняем корабли...
	SeaAI_SwapShipsAttributes(rOneChr, rSecChr, true);
	//#20190207-01 //#20190627-01
	SeaAI_SwapShipAfterAbordage(rOneChr, rSecChr);

	//вернем команду
	SetCrewQuantity(rOneChr, nMyCrew);
	SetCrewQuantity(rSecChr, 0);
    rOneChr.ship.crew.morale = fMor;
    // опыт на место
    makearef(arCargoOne, rOneChr.Ship.Crew.Exp);
	DeleteAttribute(arCargoOne,"");
	CopyAttributes(arCargoOne,&oTmp);

	//вернем груз
	if (_swapCargo)
	{
		oTmp.Ship.Cargo = "";
		makearef(arCargoOne, rOneChr.Ship.Cargo);
		makearef(arCargoSec, oTmp.Ship.Cargo);

		CopyAttributes(arCargoSec, arCargoOne);
		DeleteAttribute(arCargoOne,"");

		makearef(arCargoSec, rSecChr.Ship.Cargo);
		CopyAttributes(arCargoOne,arCargoSec);
		DeleteAttribute(arCargoSec,"");

		makearef(arCargoOne, oTmp.Ship.Cargo);
		CopyAttributes(arCargoSec, arCargoOne);

		//обновим интерфейс
		DoQuestCheckDelay("NationUpdate", 1.0);
		RefreshBattleInterface();
	}
	if (_showLog)
	{
		Log_SetStringToLog(GetFullName(rOneChr) + GetConvertStrWithReplace("Variable_companions_13", "Logs.txt", "#space#", " ") +
						XI_ConvertString(RealShips[sti(rOneChr.Ship.Type)].BaseName) + " '" + rOneChr.Ship.Name + "'");
		trace(GetFullName(rOneChr) + " обменял свой корабль на " +
						XI_ConvertString(RealShips[sti(rOneChr.Ship.Type)].BaseName) + " " + rOneChr.Ship.Name + ".");
	}
	// LEO: освежить флаги после свопа кораблей - иначе на корпусе остаётся флаг ПРЕЖНЕЙ нации (дубль при последующем присвоении призу)
	Ship_FlagRefresh(rOneChr);
	Ship_FlagRefresh(rSecChr);
}
//сравнить два корабля
bool CheckChanceOfBetterShip(ref rChar, ref rTryChar)
{
	float fChance;
	ref  rShip;
	ref  rTryShip;

	rShip    = GetRealShip(sti(rChar.Ship.Type));
	rTryShip = GetRealShip(sti(rTryChar.Ship.Type));

	//запрет меняться кораблями 18.02.08
	if (CheckAttribute(rChar, "Tasks.CanChangeShipAfterBoarding") && !sti(rChar.Tasks.CanChangeShipAfterBoarding)) return false;
    if (CheckAttribute(rChar, "Tasks.CanBoarding") && !sti(rChar.Tasks.CanBoarding)) return false;
	//заглушка для квестовых кораблей с 48ф пушками. 22.09.05
	if (sti(rChar.Ship.Cannons.Type) == CANNON_TYPE_CANNON_LBS48 || sti(rTryChar.Ship.Cannons.Type) == CANNON_TYPE_CANNON_LBS48) return false;

	fChance = (stf(rShip.Class) / stf(rTryShip.Class)) * 0.3; //при равных классах база 30 процентов

	//форс-мажор... тонем
	if (GetHullPercent(rChar) < 10 && GetHullPercent(rTryChar) > 30)
	{
		fChance += fChance * 3;
	}

	//пока взял примерно из опроса... в сумме 75 процентов макс, может нужно будет добавить...
	if (sti(rShip.SpeedRate) < sti(rTryShip.SpeedRate))				fChance += 0.2;
	if (sti(rShip.TurnRate) < sti(rTryShip.TurnRate))				fChance += 0.15;
	if (sti(rShip.CannonsQuantity) < sti(rTryShip.CannonsQuantity)) fChance += 0.1;
	if (sti(rShip.MaxCrew) < sti(rTryShip.MaxCrew))					fChance += 0.1;
	if (sti(rShip.Capacity) < sti(rTryShip.Capacity))				fChance += 0.1;
	if (sti(rShip.Price) < sti(rTryShip.Price))
	{
		fChance += 0.1;
	}
	else
	{
	    fChance -= 0.2;
	}

	if (fChance > 0.7) return true;
	return false;
}
//взять товары с абордированного корабля
void DoTakeAbordageGoods(ref rOneChr, ref rSecChr)
{
	int i, j, maxPrice, iGood, itmp;
	int freeSpace, freeQuantity;

	//три прохода, т.е. всего возьмем три товара...
	for (j=0; j < 3; j++)
	{
		freeSpace = GetCargoFreeSpace(rOneChr);
		//если у нас полный трюм выходим...
		if (freeSpace <= 0) return;

		maxPrice = 0;
		iGood = -1;
		//ищем самый дорогой товар на борту
		for (i=0; i < GOODS_QUANTITY; i++)
		{
			itmp = GetCargoGoods(rSecChr, i);
			if (itmp > 0 && sti(Goods[i].Cost) * itmp/Goods[i].Units > maxPrice)
			{
				maxPrice = sti(Goods[i].Cost) * itmp/Goods[i].Units;
				iGood = i;
			}
		}

		//если нет ничего тоже выходим
		if (iGood == -1) return;

		//перегружаем товар
		freeQuantity = GetGoodQuantityByWeight(iGood, freeSpace);
		itmp = GetCargoGoods(rSecChr, iGood);
		if (itmp <= freeQuantity)
		{
			AddCharacterGoods(rOneChr, iGood, itmp);
			RemoveCharacterGoods(rSecChr, iGood, itmp);
		}
		else
		{
			AddCharacterGoods(rOneChr, iGood, freeQuantity);
			RemoveCharacterGoods(rSecChr, iGood, freeQuantity);
		}
	}
}

void PlaceCompanionCloneNearMChr(int _index, bool _isCampus)
{
	int iTemp;
	ref chr, sld;
	aref arClone, arReal;

	chr = GetCharacter(GetCompanionIndex(PChar, _index));
	iTemp = NPC_GeneratePhantomCharacter("citizen", sti(chr.Nation), MAN, 0);  // создать клон
	if (iTemp != -1)
	{
		sld = &Characters[iTemp];
        ChangeAttributesFromCharacter(sld, chr, true);
		makearef(arReal, chr.Tasks);
		makearef(arClone, sld.Tasks);
		CopyAttributes(arClone, arReal);

		if (_isCampus)
		{
			if (_index < 5)
			{
				PlaceCharacter(sld, "sit", PChar.location);
				LAi_SetSitType(sld);
			}
			else
			{
				LAi_SetCitizenType(sld);
				PlaceCharacter(sld, "rld", PChar.location);
			}
		}
		else
		{
			LAi_SetCitizenType(sld);
			PlaceCharacter(sld, "rld", PChar.location);
		}
		LAi_SetImmortal(sld, true); //неубиваемый.
		sld.IsCompanionClone = true; //флаг для диалога, т.к. он не компаньон все таки...
        sld.RealCompanionIdx = chr.index;

		chr.Tasks.Clone = iTemp; //признак, что клон уже вызван.
	}
}
//18.02.08
void CompanionSaveTasks()
{
	int i, cn;
	ref chr, sld;
	aref arClone, arReal;

	// LEO: по ВСЕМ слотам, а не по их КОЛИЧЕСТВУ: слоты бывают разрежены (ушёл компаньон из id1 -
	//   остались id2,id3), и из слота выше счётчика приказы не переносились с клона, пропадая вместе
	//   с ним. Пустые пропускаем - GetCompanionIndex отдаёт -1, а GetCharacter(-1) лезет в мусор.
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(PChar, i);
		if (cn < 0) continue;
		chr = GetCharacter(cn);
		if (CheckAttribute(chr, "Tasks.Clone"))
		{
			sld = GetCharacter(sti(chr.Tasks.Clone));

			makearef(arReal, chr.Tasks);
			makearef(arClone, sld.Tasks);
			CopyAttributes(arReal, arClone);

			sld.location = "none";
			DeleteAttribute(chr, "Tasks.Clone");
		}
	}
}
//navy <--

//.........................  табличка выхода из боя 09.07.06 ...............
// _loadForm - показать форму с вопросом или простая проверка для запрета
bool CheckEnemyCompanionDistance2GoAway(bool _loadForm)
{
    aref	arUpdate;
	int		i, k, cn, iRelation;
	//#20180524-01 Remove unneeded sqrt
	float	fDistance, fMapDisable;
	string  attr;

	ref rCharacter, rFortCharacter, rShipCharacter;

	makearef(arUpdate, pchar.SeaAI.Update);

	if (!CheckAttribute(arUpdate, "Ships")) { return true; }
	if (!CheckAttribute(arUpdate, "Forts")) { return true; }

	aref aShips, aForts;
	aref aFort;
	aref aShip;

	makearef(aShips, arUpdate.Ships);
	makearef(aForts, arUpdate.Forts);

	// check forts for abordage
	int iShipsNum = GetAttributesNum(aShips);

	int iFortsNum = GetAttributesNum(aForts);
	int iFortMode;

	DeleteAttribute(pchar, "CheckEnemyCompanionDistance");
	for (k=1; k<COMPANION_MAX; k++)
	{
		cn = GetCompanionIndex(pchar, k);
		if( cn>=0 )
		{
            attr = "CompIDX" + k;
			rCharacter = GetCharacter(cn);
			if (!GetRemovable(rCharacter)) continue;  // квестовые сами себя спасают

			// живые форты
			for (i=0; i<iFortsNum; i++)
			{
				aFort = GetAttributeN(aForts, i);
				iRelation = sti(aFort.relation);

				rFortCharacter = GetCharacter(sti(aFort.idx));
				if (LAi_IsDead(rFortCharacter)) { continue; }
				iFortMode = sti(rFortCharacter.Fort.Mode);

				switch (iFortMode)
				{
					case FORT_NORMAL:
						if (iRelation == RELATION_ENEMY)
						{
		                    //#20180524-01 Remove unneeded sqrt
						    fMapDisable =  MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER_FORT *  MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER_FORT;
		                    fDistance = Ship_GetDistance2DRel(rCharacter, rFortCharacter);
							if (fDistance < fMapDisable)
							{
							    if (!_loadForm) return false; // все, кто-то есть
								// пишем кто, в форму
								pchar.CheckEnemyCompanionDistance.(attr) = cn;
							}
						}
					break;
				}
			}
			// check ships for abordage
			for (i=0; i<iShipsNum; i++)
			{
				aShip = GetAttributeN(aShips, i);

				rShipCharacter = GetCharacter(sti(aShip.idx));
				iRelation = sti(aShip.relation);

				if (LAi_IsDead(rShipCharacter)) { continue; }
				if (iRelation != RELATION_ENEMY) { continue; }
		        //#20180524-01 Remove unneeded sqrt
                fMapDisable =  MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER *  MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER;
                fDistance = Ship_GetDistance2DRel(rCharacter, rShipCharacter);
                if (fDistance < fMapDisable)
				{
				    if (!_loadForm) return false; // все, кто-то есть
					pchar.CheckEnemyCompanionDistance.(attr) = cn;
				}
			}
		}
	}
	if (_loadForm && CheckAttribute(pchar, "CheckEnemyCompanionDistance"))
	{
		// вызов формы
		LaunchLeaveBattleScreen();
		return false;
	}
	return true; // все хорошо, ничто не мешает
}
