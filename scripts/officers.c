// BOAL полностью переделано под нужны ВМЛ

// -> ugeen 27.01.09 - лочим лояльность квестовых офф-ов (всегда максимальная)
void OfficersReaction(string alignment)
{
	int iPassenger;
	int i, cn;
	ref sld;

	for (int io = 0; io<GetPassengersQuantity(pchar); io++)
	{   // любой пассажир у кого есть пристрастие может свалить если наши дела ему не по душе
		iPassenger = GetPassenger(pchar, io);
		if (iPassenger != -1)
		{
			sld = GetCharacter(iPassenger);
			if (CheckAttribute(sld, "loyality") && !CheckAttribute(sld, "OfficerWantToGo.DontGo") && !IsEquipCharacterByArtefact(sld, "talisman17"))
			{
				if (sld.alignment == alignment)
				{
					sld.loyality = makeint(sld.loyality) + 1;
				}
				else
				{
					sld.loyality = makeint(sld.loyality) - 1;
				}
			}
		}
	}
	for (i=1; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if(cn!=-1)
		{
			sld = GetCharacter(cn);
			if (CheckAttribute(sld, "loyality") && !CheckAttribute(sld, "OfficerWantToGo.DontGo") && !IsEquipCharacterByArtefact(sld, "talisman17"))
			{
				if (sld.alignment == alignment)
				{
					sld.loyality = makeint(sld.loyality) + 1;
					if (CheckAttribute(sld, "PGGAi")) PGG_ChangeRelation2MainCharacter(sld, 1); //navy
				}
				else
				{
					sld.loyality = makeint(sld.loyality) - 1;
					// специально не далею приведение к 0 и мах. потому чтоб был запас минусов для проверок
					if (CheckAttribute(sld, "PGGAi")) PGG_ChangeRelation2MainCharacter(sld, -1); //navy
				}
			}
		}
	}
}
//-> ugeen

void ChangeOfficersLoyality(string alignment, int iLoyality)
{ // aliggnment types : "bad" , "good" , "bad_all" , "good_all"

	int iPassenger;
	int i, cn;
	ref sld;

	for (int io = 0; io<GetPassengersQuantity(pchar); io++)
	{   // любой пассажир у кого есть пристрастие может свалить если наши дела ему не по душе
		iPassenger = GetPassenger(pchar, io);
		if (iPassenger != -1)
		{
			sld = GetCharacter(iPassenger);
			if (CheckAttribute(sld, "loyality") && !CheckAttribute(sld, "OfficerWantToGo.DontGo"))
			{
				switch (alignment)
				{
					case "bad":
						if (sld.alignment == alignment) sld.loyality = makeint(sld.loyality) + iLoyality;
						else 							sld.loyality = makeint(sld.loyality) - iLoyality;
					break;
					case "good":
						if (sld.alignment == alignment) sld.loyality = makeint(sld.loyality) + iLoyality;
						else 							sld.loyality = makeint(sld.loyality) - iLoyality;
					break;
					case "bad_all":
						sld.loyality = makeint(sld.loyality) - iLoyality;
					break;
					case "good_all":
						sld.loyality = makeint(sld.loyality) + iLoyality;
					break;
				}
			}
		}
	}
	for (i=1; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if(cn!=-1)
		{
			sld = GetCharacter(cn);
			if (CheckAttribute(sld, "loyality") && !CheckAttribute(sld, "OfficerWantToGo.DontGo"))
			{
				switch (alignment)
				{
					case "bad":
						if (sld.alignment == alignment) {
							sld.loyality = makeint(sld.loyality) + iLoyality;
							if (CheckAttribute(sld, "PGGAi")) PGG_ChangeRelation2MainCharacter(sld, 1); //navy
						}
						else
						{
							sld.loyality = makeint(sld.loyality) - iLoyality;
							if (CheckAttribute(sld, "PGGAi")) PGG_ChangeRelation2MainCharacter(sld, -1); //navy
						}
					break;
					case "good":
						if (sld.alignment == alignment) {
							sld.loyality = makeint(sld.loyality) + iLoyality;
							if (CheckAttribute(sld, "PGGAi")) PGG_ChangeRelation2MainCharacter(sld, 1); //navy
						}
						else
						{
							sld.loyality = makeint(sld.loyality) - iLoyality;
							if (CheckAttribute(sld, "PGGAi")) PGG_ChangeRelation2MainCharacter(sld, -1); //navy
						}
					break;
					case "bad_all":
						sld.loyality = makeint(sld.loyality) - iLoyality;
						if (CheckAttribute(sld, "PGGAi")) PGG_ChangeRelation2MainCharacter(sld, -1); //navy
					break;
					case "good_all":
						sld.loyality = makeint(sld.loyality) + iLoyality;
						if (CheckAttribute(sld, "PGGAi")) PGG_ChangeRelation2MainCharacter(sld, 1); //navy
					break;
				}
			}
		}
	}
}

// результат реакции - действия офа - диалог
void OfficersReactionResult()
{
	int iPassenger;
	int i;
	ref sld;

	if (LAi_IsCapturedLocation) return;
	if (chrDisableReloadToLocation) return; // идёт некий другой квест с запретом выхода
	if (CheckAttribute(Pchar,"questTemp.MunityOfficerID") && Pchar.questTemp.MunityOfficerID.begin == "1") return;

	for (i = 0; i<GetPassengersQuantity(pchar); i++)
	{   // любой пассажир у кого есть пристрастие может свалить если наши дела ему не по душе
		iPassenger = GetPassenger(pchar, i);
		if (iPassenger != -1)
		{
			sld = GetCharacter(iPassenger);
			if (sld.name == GetConvertStrWithReplace("Variable_officers_1", "Names.txt", "#space#", " ") && !CheckAttribute(sld, "PGGWhisperQuestStart") && !CheckAttribute(pchar,"GiantEvilSkeleton"))
			{
				if (GetCharacterShipClass(PChar) <= 4 && pchar.rank >= 20 && !CheckCharacterItem(PChar, "patent_spa"))
				{
					if (PlaceCharacter(sld, "goto", "random_must_be_near") != "")
					{
						sld.dialog.filename = "PGG_dialog.c";
						sld.dialog.currentnode = "Whisper_Officer";
						
						LAi_SetActorType(sld);
						LAi_ActorDialog(sld, pchar, "", 2.0, 0);
						chrDisableReloadToLocation = true;
						DoQuestCheckDelay("OpenTheDoors", 5.0);
					}
				}
			}
			if(sld.id == "W_Chinaman" && !CheckAttribute(sld, "ChinamanAskedSword") && sti(sld.rank) > 7)
			{
				if (PlaceCharacter(sld, "goto", "random_must_be_near") != "")
				{
					sld.dialog.filename = "Quest\WhisperLine\Whisper.c";
					sld.dialog.currentnode = "Chinaman_plead";

					LAi_SetActorType(sld);
					LAi_ActorDialog(sld, pchar, "", 2.0, 0);
					chrDisableReloadToLocation = true;
					DoQuestCheckDelay("OpenTheDoors", 5.0);
				}
			}

			if (CheckAttribute(sld, "loyality") && !CheckAttribute(sld,"prisoned") && GetRemovable(sld) && !CheckAttribute(sld, "OfficerWantToGo.DontGo"))
			{
				if (sti(sld.loyality) < 1)
				{
					if (PlaceCharacter(sld, "goto", "random_must_be_near") != "")
					{
						sld.dialog.filename = "Enc_Officer_dialog.c";
						sld.dialog.currentnode = "WantToRemove";
				      	if (sld.sex != "woman")
						{
					    	sld.greeting = "Gr_Officer_Fired";
					    }
						DeleteAttribute(sld, "quest.officertype");

						LAi_SetActorType(sld);
						LAi_ActorDialog(sld, pchar, "", 2.0, 0);
			    		chrDisableReloadToLocation = true;
			    		DoQuestCheckDelay("OpenTheDoors", 5.0);
						break;
					}
				}
			}
		}
	}
	if (chrDisableReloadToLocation) return; // уже увольняется

	if (CheckNPCQuestDate(pchar, "OfficerWantToGo") && rand(1) == 1) // только раз в день
	{
		SetNPCQuestDate(pchar, "OfficerWantToGo");
		for (i = 0; i<GetPassengersQuantity(pchar); i++)
		{   // любой пассажир у кого есть пристрастие может свалить если наши дела ему не по душе
			iPassenger = GetPassenger(pchar, i);
			if (iPassenger != -1)
			{
				sld = GetCharacter(iPassenger);
				if (!CheckAttribute(sld,"prisoned") && !CheckAttribute(sld, "OfficerWantToGo.DontGo") && GetRemovable(sld) && rand(10) == 5 && GetNpcQuestPastDayParam(sld, "HiredDate") > 60)
				{
					if (sld.dialog.filename == "Enc_Officer_dialog.c") // квестовые не достают
					{
						if (PlaceCharacter(sld, "goto", "random_must_be_near") != "")
						{
							sld.dialog.currentnode = "WantToGo";
							if (sld.sex != "woman")
							{
								sld.greeting           = "Gr_Officer_Salary";
							}
							LAi_SetActorType(sld);
							LAi_ActorDialog(sld, pchar, "", 2.0, 0);
				    		chrDisableReloadToLocation = true;
				    		DoQuestCheckDelay("OpenTheDoors", 5.0);
							break;
						}
					}
				}
			}
		}
	}
}

bool isOfficerInShip(ref _chr, bool _withBoard)
{
	if(sti(pchar.Fellows.Passengers.navigator) == sti(_chr.index))
	{
  		return true;
	}

	if(sti(pchar.Fellows.Passengers.boatswain) == sti(_chr.index))
	{
		return true;
	}

	if(sti(pchar.Fellows.Passengers.cannoner) == sti(_chr.index))
	{
  		return true;
	}

	if(sti(pchar.Fellows.Passengers.doctor) == sti(_chr.index))
	{
  		return true;
	}

	if(sti(pchar.Fellows.Passengers.carpenter) == sti(_chr.index))
	{
  		return true;
	}
    if(sti(pchar.Fellows.Passengers.treasurer) == sti(_chr.index))
	{
  		return true;
	}
	if(_withBoard && IsOfficer(_chr))
	{
		return true;
	}
	return false;
}
// метод увольняет офицера - название левое, наследие к3
void CheckForReleaseOfficer(int iCharIndex)
{
	// запоминаем ВСЕ занимаемые должности для авто-возврата (перед снятием): ранение / назначение капитаном
	ref _rch = &characters[iCharIndex];
	bool _bAnyPost = false;
	if(sti(pchar.Fellows.Passengers.navigator) == iCharIndex) _bAnyPost = true;
	if(sti(pchar.Fellows.Passengers.boatswain) == iCharIndex) _bAnyPost = true;
	if(sti(pchar.Fellows.Passengers.cannoner)  == iCharIndex) _bAnyPost = true;
	if(sti(pchar.Fellows.Passengers.doctor)    == iCharIndex) _bAnyPost = true;
	if(sti(pchar.Fellows.Passengers.carpenter) == iCharIndex) _bAnyPost = true;
	if(sti(pchar.Fellows.Passengers.treasurer) == iCharIndex) _bAnyPost = true;
	if(IsOfficer(_rch)) _bAnyPost = true;
	if(_bAnyPost)
	{
		DeleteAttribute(_rch, "RestorePost");
		if(sti(pchar.Fellows.Passengers.navigator) == iCharIndex) _rch.RestorePost.navigator = true;
		if(sti(pchar.Fellows.Passengers.boatswain) == iCharIndex) _rch.RestorePost.boatswain = true;
		if(sti(pchar.Fellows.Passengers.cannoner)  == iCharIndex) _rch.RestorePost.cannoner  = true;
		if(sti(pchar.Fellows.Passengers.doctor)    == iCharIndex) _rch.RestorePost.doctor    = true;
		if(sti(pchar.Fellows.Passengers.carpenter) == iCharIndex) _rch.RestorePost.carpenter = true;
		if(sti(pchar.Fellows.Passengers.treasurer) == iCharIndex) _rch.RestorePost.treasurer = true;
		if(IsOfficer(_rch)) _rch.RestorePost.fighter = true;
		// LEO: с должностью запоминаем и приказ обходить город - иначе он терялся при назначении
		//   капитаном, хотя саму должность ReturnAutoOfficers возвращает. Отдельными if: && не ленивый.
		if(CheckAttribute(_rch, "RestorePost.treasurer"))
		{
			if(CheckAttribute(_rch, "TreasurerPortRun"))
			{
				if(sti(_rch.TreasurerPortRun) == 1) _rch.RestorePost.portrun = true;
			}
		}
	}

	if(sti(pchar.Fellows.Passengers.navigator) == iCharIndex)
	{
		pchar.Fellows.Passengers.navigator = -1;
		DeleteAttribute(&characters[iCharIndex], "navigator"); // совместитель дожности
	}

	if(sti(pchar.Fellows.Passengers.boatswain) == iCharIndex)
	{
		pchar.Fellows.Passengers.boatswain = -1;
		DeleteAttribute(&characters[iCharIndex], "boatswain"); // совместитель дожности
	}

	if(sti(pchar.Fellows.Passengers.cannoner) == iCharIndex)
	{
		pchar.Fellows.Passengers.cannoner = -1;
		DeleteAttribute(&characters[iCharIndex], "cannoner"); // совместитель дожности
	}

	if(sti(pchar.Fellows.Passengers.doctor) == iCharIndex)
	{
		pchar.Fellows.Passengers.doctor = -1;
		DeleteAttribute(&characters[iCharIndex], "doctor"); // совместитель дожности
	}

	if(sti(pchar.Fellows.Passengers.carpenter) == iCharIndex)
	{
		pchar.Fellows.Passengers.carpenter = -1;
		DeleteAttribute(&characters[iCharIndex], "carpenter"); // совместитель дожности
	}

    if(sti(pchar.Fellows.Passengers.treasurer) == iCharIndex)
	{
		pchar.Fellows.Passengers.treasurer = -1;
		DeleteAttribute(&characters[iCharIndex], "treasurer"); // совместитель дожности
		// LEO: приказ умирает вместе с УХОДОМ ИЗ КОМАНДЫ - иначе пережил бы повторный наём, и новичок
		//   забегал бы сам. Авто-возврат должности приказ не теряет: он перенесён в RestorePost.portrun.
		//   Простое снятие с поста (AcceptRemoveOfficer) сюда не заходит - человек остаётся и помнит приказ.
		DeleteAttribute(&characters[iCharIndex], "TreasurerPortRun");
	}

	// без проверки на должность: все проверки внутри, иначе не работает на трупе
	RemoveOfficersIndex(pchar, iCharIndex);
	DeleteAttribute(&characters[iCharIndex], "fighter"); // совместитель дожности
	DeleteAttribute(&characters[iCharIndex], "isfree"); // для формы ф2, название странное - обратный смысл, если есть, то занят
}

// LEO: пересчитать счётчик занятых должностей ПО ФАКТУ. Раньше isfree только инкрементили при
//   назначении и декрементили при снятии, и любое расхождение оставалось в сейве навсегда. Счётчик
//   больше числа реальных должностей = человек не проходит фильтр списка кандидатов (там нужно
//   isfree < howWork): из назначения он пропадает, продолжая занимать место по лимиту найма.
int SyncOfficerIsFree(ref chr)
{
	int idx = sti(chr.index);
	int n = 0;
	if(IsOfficer(chr)) n++;
	if(CheckAttribute(pchar, "Fellows.Passengers.navigator")) { if(sti(pchar.Fellows.Passengers.navigator) == idx) n++; }
	if(CheckAttribute(pchar, "Fellows.Passengers.boatswain")) { if(sti(pchar.Fellows.Passengers.boatswain) == idx) n++; }
	if(CheckAttribute(pchar, "Fellows.Passengers.cannoner"))  { if(sti(pchar.Fellows.Passengers.cannoner)  == idx) n++; }
	if(CheckAttribute(pchar, "Fellows.Passengers.doctor"))    { if(sti(pchar.Fellows.Passengers.doctor)    == idx) n++; }
	if(CheckAttribute(pchar, "Fellows.Passengers.carpenter")) { if(sti(pchar.Fellows.Passengers.carpenter) == idx) n++; }
	if(CheckAttribute(pchar, "Fellows.Passengers.treasurer")) { if(sti(pchar.Fellows.Passengers.treasurer) == idx) n++; }
	if(n > 0) chr.isfree = n;
	else DeleteAttribute(chr, "isfree");
	return n;
}

// Назначить персонажа idx на должность postType ("navigator/boatswain/cannoner/doctor/carpenter/treasurer/fighter"),
// если слот свободен. Дублирует логику UI-назначения (слот + метка должности + isfree). true при успехе.
bool AutoAssignPost(int idx, string postType)
{
	ref ch = &characters[idx];
	if(postType == "fighter")
	{
		// LEO: он уже в абордажной команде - слот не трогаем. SetOfficersIndex ниже такой повтор
		//   молча отбивает (там свой гард), а мы раньше всё равно накручивали isfree: счётчик рос
		//   без назначения, и человек переставал проходить фильтр списка кандидатов. Метку и
		//   счётчик при этом всё равно приводим к факту - они могли разойтись со слотом.
		if(IsOfficer(ch))
		{
			ch.(postType) = true;
			SyncOfficerIsFree(ch);
			return false;
		}
		int s, freeSlot;
		freeSlot = -1;
		for(s = 1; s <= MAX_NUM_FIGHTERS; s++) { if(GetOfficersIndex(pchar, s) == -1) { freeSlot = s; break; } }
		if(freeSlot == -1) return false;        // нет свободного слота абордажа
		SetOfficersIndex(pchar, freeSlot, idx); // ставит слот + LAi_SetOfficerType
	}
	else
	{
		if(CheckAttribute(pchar, "Fellows.Passengers." + postType))
		{
			// LEO: должность уже за ним - слот не трогаем, но метку и счётчик приводим к факту:
			//   они ставятся отдельно от слота и могли с ним разойтись. Раньше выходили молча.
			if(sti(pchar.Fellows.Passengers.(postType)) == idx)
			{
				ch.(postType) = true;
				SyncOfficerIsFree(ch);
				return false;
			}
			if(sti(pchar.Fellows.Passengers.(postType)) >= 0) return false; // занята кем-то другим
		}
		pchar.Fellows.Passengers.(postType) = idx;
	}
	ch.(postType) = true; // метка должности (нужна фильтру списка совместителей в FillFreePassengerScroll)
	SyncOfficerIsFree(ch);
	return true;
}

// Вернуть офицеров на ВСЕ их прежние должности (RestorePost), записанные в CheckForReleaseOfficer.
// Срабатывает после продажи/потопления/обмена-корабля (ex-компаньон стал пассажиром) и после выздоровления раненого.
// Пропускаем: раненых и тех, кто командует кораблём (компаньон). Возврат одноразовый: что свободно - заняли, запись стираем.
void ReturnAutoOfficers()
{
	int q = GetNotCaptivePassengersQuantity(pchar);
	int i, s, idx;
	bool bIsComp, bChanged;
	ref ch;
	bChanged = false;
	for(i = 0; i < q; i++)
	{
		idx = GetPassenger(pchar, i);
		if(idx < 0) continue;
		ch = GetCharacter(idx);
		if(!CheckAttribute(ch, "RestorePost")) continue;             // нечего возвращать
		if(CheckAttribute(ch, "HPminusDaysNeedtoRestore")) continue; // ранен - ждём выздоровления
		// командует кораблём (компаньон)? - не трогаем
		bIsComp = false;
		for(s = 1; s < COMPANION_MAX; s++) { if(GetCompanionIndex(pchar, s) == idx) { bIsComp = true; break; } }
		if(bIsComp) continue;
		// восстановить должности (что сейчас свободно)
		if(CheckAttribute(ch, "RestorePost.navigator")) { if(AutoAssignPost(idx, "navigator")) bChanged = true; }
		if(CheckAttribute(ch, "RestorePost.boatswain")) { if(AutoAssignPost(idx, "boatswain")) bChanged = true; }
		if(CheckAttribute(ch, "RestorePost.cannoner"))  { if(AutoAssignPost(idx, "cannoner"))  bChanged = true; }
		if(CheckAttribute(ch, "RestorePost.doctor"))    { if(AutoAssignPost(idx, "doctor"))    bChanged = true; }
		if(CheckAttribute(ch, "RestorePost.carpenter")) { if(AutoAssignPost(idx, "carpenter")) bChanged = true; }
		if(CheckAttribute(ch, "RestorePost.treasurer"))
		{
			if(AutoAssignPost(idx, "treasurer"))
			{
				bChanged = true;
				// LEO: вернулся на ту же должность - помнит и распоряжение обходить город
				if(CheckAttribute(ch, "RestorePost.portrun")) ch.TreasurerPortRun = 1;
			}
		}
		if(CheckAttribute(ch, "RestorePost.fighter"))   { if(AutoAssignPost(idx, "fighter"))   bChanged = true; }
		DeleteAttribute(ch, "RestorePost"); // одноразовый возврат
	}
	if(bChanged) Event(EVENT_CHANGE_OFFICERS, ""); // обновить производные параметры (бонусы должностей)
}

// Сортировка абордажников в слотах по убыванию "силы" (макс.ХП * ранг) - сильнейшие наверху списка.
void SortBoarders()
{
	int i, j, cnt, idx, bestK, ti;
	int   arrIdx[32];
	float arrScore[32];
	float ts;
	ref ch;
	cnt = 0;
	for(i = 1; i <= MAX_NUM_FIGHTERS; i++)
	{
		idx = GetOfficersIndex(pchar, i);
		if(idx == -1) continue;
		if(cnt >= 32) break;
		arrIdx[cnt]   = idx;
		ch = GetCharacter(idx);
		arrScore[cnt] = stf(LAi_GetCharacterMaxHP(ch)) * stf(ch.rank); // вес: макс.ХП в соотношении с рангом
		cnt++;
	}
	if(cnt <= 1) return; // сортировать нечего
	// сортировка выбором по убыванию score
	for(i = 0; i < cnt - 1; i++)
	{
		bestK = i;
		for(j = i + 1; j < cnt; j++)
		{
			if(arrScore[j] > arrScore[bestK]) bestK = j;
		}
		if(bestK != i)
		{
			ts = arrScore[i]; arrScore[i] = arrScore[bestK]; arrScore[bestK] = ts;
			ti = arrIdx[i];   arrIdx[i]   = arrIdx[bestK];   arrIdx[bestK]   = ti;
		}
	}
	// очистить слоты абордажа и расставить заново по порядку (сильнейшие - в слоты 1..cnt)
	for(i = 1; i <= MAX_NUM_FIGHTERS; i++) SetOfficersIndex(pchar, i, -1);
	for(i = 0; i < cnt; i++) SetOfficersIndex(pchar, i + 1, arrIdx[i]);
}

//////////////// OFFICER ////////////////
void SetOfficerParam(ref Npchar, int _type)
{
	Npchar.quest.officertype_2 = ""; //mercen_";
    switch(_type)
	{
		case 0:
		    SetRandSPECIAL_K(Npchar);  // вот сюда методы  под каждого типа оффов
			Npchar.quest.officertype = "boatswain";
			Npchar.quest.officertype_2 = RandPhraseSimple(XI_ConvertString("OffDoctor1"), XI_ConvertString("OffDoctor2"));
		break;

		case 1:
		    SetRandSPECIAL_K(Npchar);
			Npchar.quest.officertype = "cannoner";
		break;

		case 2:
		    SetRandSPECIAL_K(Npchar);
			Npchar.quest.officertype_2 = RandPhraseSimple(XI_ConvertString("OffCarpenter1"), XI_ConvertString("OffCarpenter2"));
			Npchar.quest.officertype = "treasurer";
		break;

		case 3:
		    SetRandSPECIAL_K(Npchar);
			Npchar.quest.officertype_2 = RandPhraseSimple(XI_ConvertString("OffDoctor1"), XI_ConvertString("OffDoctor2"));
			Npchar.quest.officertype = "navigator";
		break;

        case 4:
		    SetRandSPECIAL_F(Npchar);
			Npchar.quest.officertype = "fighter";
		break;
	}
	CalculateTypeSkillsForRank(npchar, sti(pchar.rank) + rand(2), npchar.quest.officertype, 0.1);
    Npchar.reputation = 10 + rand(80);
    // пристрастие офицера -->
    Npchar.loyality = 5 + rand(10);
    if (sti(Npchar.reputation) >= 40)//почему нейтрал считается хорошим?
    {
        Npchar.alignment = "good";
    }
    else
    {
        Npchar.alignment = "bad";
    }

	Npchar.officerequip = true;
    LAi_NPC_Equip(Npchar, sti(Npchar.rank), true, true); // fix 101104 выдадим все заново!!!!

    SetFantomHP(Npchar);

    Npchar.quest.OfficerPrice    = (11 + 2*sti(Npchar.rank))*(150 + MOD_SKILL_ENEMY_RATE*3*20) + rand(5)*10;
    Npchar.quest.OfficerLowPrice = makeint(sti(Npchar.quest.OfficerPrice)/1.5 + 0.5);
	SetSpeciality(Npchar, PerksChars());
}

int FindFreeRandomOfficer()
{
	int Counter, OfficerIdx;
	string OfficerId;
	// special -->
	if (GetCharacterMaxOfficersQty(Pchar) < (GetNotQuestPassengersQuantity(Pchar) + GetNotQuestCompanionQuantity(Pchar) - 1)) {pchar.LeadershipLose = true; return -1;}
	if (GetCharacterMaxOfficersQty(Pchar) == (GetNotQuestPassengersQuantity(Pchar) + GetNotQuestCompanionQuantity(Pchar) - 1)) {DeleteAttribute(pchar,"LeadershipLose"); return 0;}
	else DeleteAttribute(pchar,"LeadershipLose");
	return 1;
}
void LandEnc_OfficerHired()
{
	ref sld = &Characters[sti(Pchar.questTemp.HiringOfficerIDX)];
    sld.Money = 0; // деньги
	sld.Payment = true;
	AddPassenger(pchar, sld, false);

	sld.location = "None";
	sld.Dialog.CurrentNode = "hired";
	// boal новый учет зп -->
	sld.Payment = true;  // признак офицера для +1 предметов
	sld.DontClearDead = true;  // не убирать труп
	// boal новый учет зп <--
	DeleteAttribute(Pchar, "questTemp.HiringOfficerIDX");
	DeleteAttribute(sld, "LifeDay") // постоянный
	SaveCurrentNpcQuestDateParam(sld, "HiredDate"); // дата найма
}

void LandEnc_OfficerStay()
{
    SetCharacterTask_Stay(Characters[makeint(Pchar.questTemp.HiringOfficerIDX)]);
    Characters[makeint(Pchar.questTemp.HiringOfficerIDX)].chr_ai.tmpl = LAI_TMPL_STAY;
    DeleteAttribute(Pchar, "questTemp.HiringOfficerIDX");
}

void LandEnc_OfficerFired()
{
	ref sld = &Characters[sti(Pchar.questTemp.FiringOfficerIDX)];
	CheckForReleaseOfficer(sti(Pchar.questTemp.FiringOfficerIDX));
	RemovePassenger(Pchar, sld);
	RemoveCharacterCompanion(pchar, sld);
	LAi_SetCitizenType(sld);
	sld.location = "none";
	if (sld.sex != "woman")
	{
    	sld.greeting = "Gr_Officer_Fired";
    }
    sld.LifeDay = 0; // стереть при выходе
    DeleteAttribute(sld, "Payment"); // признак офицера для +1 предметов
	if (CheckAttribute(sld,"OfficerInStockMan"))
	{
		DeleteAttribute(sld, "OfficerInStockMan");
		ref ch = CharacterFromID("MalteseOrderCharacter");
		ch.Portman	= sti(ch.Portman) - 1;
		pchar.OfficerInStock = sti(pchar.OfficerInStock) - 1;
	}
	DeleteAttribute(Pchar, "questTemp.FiringOfficerIDX");
}

// LEO: обёртка над общим SetCharacterToMushketer (CharacterUtilite.c)
bool SetOfficerToMushketer(ref rCharacter, string sMushket, bool _ToMushketer)
{
	return SetCharacterToMushketer(rCharacter, sMushket, _ToMushketer);
}
