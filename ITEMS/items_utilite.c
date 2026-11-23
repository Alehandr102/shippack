int FindFoodFromChr(ref chref, ref arFind, int startIdx)
{
	int i;
	aref arItm;
	for(i=startIdx; i<10; i++)
	{
		if (i!= -1)
		{
			makearef(arItm,Items[i]);
			if( CheckAttribute(arItm,"Food") && GetCharacterItem(chref,Items[i].id)>0 )
			{
				arFind = arItm;
				return i;
			}
		}
	}
	return -1;
}

int FindBetterFoodFromChr(ref chref, ref arFind)
{
	int i;
	aref arItm;
	for(i=10; i>-1; i--)
	{
		makearef(arItm,Items[i]);
		if( CheckAttribute(arItm,"Food") && GetCharacterItem(chref,Items[i].id)>0 )
		{
			arFind = arItm;
			return i;
		}
	}
	return -1;
}

bool EnableFoodUsing(ref mc, aref arItm)
{
	if(CheckAttribute(arItm,"Food.energy") && !CheckAttribute(mc,"chr_ai.noeat"))
	{
		if(LAi_GetCharacterEnergy(mc) < LAi_GetCharacterMaxEnergy(mc))
		{
			return true;
		}
	}
	if(CheckAttribute(mc,"chr_ai.noeat"))
	{
		if(sti(mc.index)==GetMainCharacterIndex() && !CheckAttribute(pchar, "autofood"))
		{
			if (CheckAttribute(pchar, "pressedFoodButton"))
			{
				DeleteAttribute(pchar, "pressedFoodButton");

				if (!CheckAttribute(pchar, "foodquery"))
				{
					pchar.foodquery = 1;
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_items_utilite_1", "Logs.txt", "#space#", " "));
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_items_utilite_2", "Logs.txt", "#space#", " ")+pchar.foodquery);
				}
				else
				{
					pchar.foodquery = sti(pchar.foodquery)+1;
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_items_utilite_3", "Logs.txt", "#space#", " "));
					Log_SetStringToLog(GetConvertStrWithReplace("Variable_items_utilite_4", "Logs.txt", "#space#", " ")+pchar.foodquery);
				}
			}

		}
	}
	return false;
}

void DoCharacterUsedFood(ref chref, string itmID)
{
	aref arItm;
	
	if(strlen(itmID) < 1 || CheckAttribute(chref,"chr_ai.Swift")){
		return;
	}
	if( Items_FindItem(itmID,&arItm) < 0 ) {
		return; 
	}
	TakeItemFromCharacter(chref,itmID);

	float fFoodEnergy = stf(arItm.Food.energy);
	float fFoodEnergySPD = stf(arItm.Food.energy.speed);
	int iFoodEnergySPD2X = MakeInt(2 * fFoodEnergySPD);//для корректной работы ифа
	if(fFoodEnergy <= 0) {
		return;
	}

	float fEnergy = stf(chref.chr_ai.energy);
	float fEnergyMax = stf(chref.chr_ai.energyMax);
	if (fEnergy >= fEnergyMax) {
		return;
	}
	int iQueryIndex = 0;
	string sQueryIndex = ""+iQueryIndex;
	if(!CheckAttribute(chref, "chr_ai.FoodEnergySPD."+sQueryIndex) || chref.chr_ai.FoodEnergy.(sQueryIndex) == 0) {//первая пошла 
		chref.chr_ai.FoodEnergySPD.(sQueryIndex) = fFoodEnergySPD;
		chref.chr_ai.FoodEnergy.(sQueryIndex) = fFoodEnergy;
		setEffectText(chref, XI_ConvertString("Energy Up"), argb(255,180,180,255), GetCharCapNotifScale());
	}
	else {
		while (MakeInt(2 * stf(chref.chr_ai.FoodEnergySPD.(sQueryIndex))) !=  iFoodEnergySPD2X) {	
			iQueryIndex++;
			sQueryIndex = ""+iQueryIndex;
			if(!CheckAttribute(chref, "chr_ai.FoodEnergy."+sQueryIndex)) {//активен ли след слот
				chref.chr_ai.FoodEnergySPD.(sQueryIndex) = fFoodEnergySPD;
			}
		}
		if(MakeInt(2 * stf(chref.chr_ai.FoodEnergySPD.(sQueryIndex))) ==  iFoodEnergySPD2X) {
			chref.chr_ai.FoodEnergy.(sQueryIndex) = stf(chref.chr_ai.FoodEnergy.(sQueryIndex)) + fFoodEnergy;
		}
		if(!CheckAttribute(chref, "chr_ai.FoodEnergy."+sQueryIndex)) {
			chref.chr_ai.FoodEnergy.(sQueryIndex) = "0";
		}
	}

	if(sti(chref.index)==GetMainCharacterIndex())
	{
		Log_SetStringToLog(XI_ConvertString("Energy Up"));
		PlaySound("interface\_Hrust_"+rand(3)+".wav");
		int foodcount = sti(pchar.questTemp.foodcount)+1;
		pchar.questTemp.foodcount = foodcount;
		// Открываем достижения
		CheckAndUnlock("AchFood", foodcount);
	}
}
// ===== ТРУБКИ ("Покурить") - быстрое меню по образцу еды/зелий =====
// Трубки (StrengthTube/EnergyTube/HealthTube) лежат в массиве предметов далеко (не в первой десятке как еда),
// поэтому ищем по ВСЕМУ массиву Items[] (в отличие от FindFoodFromChr c i<10).
int FindSmokeFromChr(ref chref, ref arFind, int startIdx)
{
	int i;
	aref arItm;
	for(i=startIdx; i<ITEMS_QUANTITY; i++)
	{
		makearef(arItm,Items[i]);
		if( CheckAttribute(arItm,"Smoke") && GetCharacterItem(chref,Items[i].id)>0 )
		{
			arFind = arItm;
			return i;
		}
	}
	return -1;
}

// трубку нельзя курить повторно, пока её эффект ещё активен (как в items.c)
bool EnableSmokeUsing(ref mc, aref arItm)
{
	// курево только для ГГ (эффекты вешаются на pchar); повтор РАЗРЕШЁН - курить можно даже при активном эффекте типа
	return sti(mc.index) == GetMainCharacterIndex();
}

// "Передоз" от смешения 2-3 трубок (перенесён сюда из interface\items.c - тот грузится сегментом и при
// курении из быстрого меню недоступен; здесь постоянный сегмент, виден везде). Зовётся из ApplyTubeEffect.
void GetHigh()
{
	Log_Info(GetConvertStrWithReplace("Variable_items_25", "Interface.txt", "#space#", " "));
	pchar.HighOnDrugs = true;
	if (pchar.sex == "woman")
	{
		PlaySound("interface\baba_kxe_kxe.wav");
	}
	else
	{
		PlaySound("interface\man_kxe_kxe.wav");
	}

	for(int i = 0; i < MAX_CHARACTERS; i++)
	{
		ref chr;
		makeref(chr,Characters[i]);
		if(!CheckAttribute(chr, "BeforeDrugsModel") && chr.model.animation == "man")
		{
			chr.BeforeDrugsModel = chr.model;
			switch (rand(2))
			{
				case 0:
					chr.model = GetRandSkelModel();
				break;
				case 1:
					chr.model = GetRandSkelModelClassic();
				break;
				case 2:
					chr.model = "Canib_"+(rand(5)+1);
				break
			}
			if (rand(10) == 0)
			{
				switch (rand(1))
				{
					case 0:
						chr.model = "PotCMobCap";
					break;
					case 1:
						chr.model = "Canib_boss";
					break;
				}
			}
			SetNewModelToChar(chr);
		}
	}
}

// Общий эффект курения трубки (вынесено из items.c, чтобы и инвентарь, и быстрое меню работали одинаково).
// Возвращает true, если эффект применён (предмет надо израсходовать); false - если эффект уже активен (не тратим).
bool ApplyTubeEffect(ref chr, string sItemID)
{
	// бонус трубки: СТАТ применяем только если этот тип ещё не активен (иначе дрейф HP/энергии),
	// но повторное курение РАЗРЕШЕНО - оно продлевает эффект (таймер) и засчитывается в суточный лимит.
	switch (sItemID)
	{
		case "HealthTube":
			if (!CheckAttribute(chr, "chr_ai.bonushptube"))
			{
				chr.chr_ai.bonushptube = 0;
				chr.PerkValue.HPBONUS = 50+rand(50)+sti(chr.rank);
				float nphpp = LAi_GetCharacterMaxHP(chr) + sti(chr.PerkValue.HPBONUS);
				LAi_SetHP(chr,nphpp,nphpp);
			}
			SetTimerFunction("ClearHPTubeEffect",0,0,1);
		break;
		case "EnergyTube":
			if (!CheckAttribute(chr, "chr_ai.bonusentube"))
			{
				chr.chr_ai.bonusentube = 0;
				chr.tubeBonusEnergy = 30+rand(20)+sti(chr.rank);
				AddBonusEnergyToCharacter(chr,sti(chr.tubeBonusEnergy));
			}
			SetTimerFunction("ClearENTubeEffect",0,0,1);
		break;
		case "StrengthTube":
			if (!CheckAttribute(chr, "chr_ai.bonusweighttube"))
			{
				chr.chr_ai.bonusweighttube = 0;
			}
			SetTimerFunction("ClearSTubeEffect",0,0,1);
		break;
	}

	// СУТОЧНЫЙ СЧЁТЧИК: за день можно выкурить до 3 трубок (по одной каждого типа) БЕЗ последствий.
	int dayNow = DateToInt(0);
	if (!CheckAttribute(chr, "tubesDay") || sti(chr.tubesDay) != dayNow)
	{
		chr.tubesDay = dayNow;
		chr.tubesDayCount = 0;
	}
	chr.tubesDayCount = sti(chr.tubesDayCount) + 1;

	// ПЕРЕКУР: 4-я трубка и далее за сутки → галлюцинации + штраф навыков + откат 3 дня.
	if (sti(chr.tubesDayCount) >= 4)
	{
		GetHigh();
		chr.drugstaken = sti(chr.tubesDayCount); // флаг штрафа (RPGUtilite: drugstaken>=3 → -50 ко всем навыкам)
		SetTimerFunction("ClearDrugs",0,0,3);    // откат/штраф 3 дня (было 10)
	}
	ApplayNewSkill(chr, "", 0);
	return true;
}

void DoCharacterUsedSmoke(ref chref, string itmID)
{
	aref arItm;
	if(strlen(itmID) < 1) return;
	if( Items_FindItem(itmID,&arItm) < 0 ) return;
	if( ApplyTubeEffect(chref, itmID) )
	{
		TakeItemFromCharacter(chref,itmID);
	}
}

bool IsQuestUsedItem(string itmID)
{
	aref arItm;
	if( Items_FindItem(itmID,&arItm)<0 ) return false;
	// Очищенные часы уже награда; цена остаётся нулевой, запрет проклятых часов не меняем.
	if (itmID == "SandClock") return false;
	if( CheckAttribute(arItm,"price") && sti(arItm.price)>0 ) return false;
	if (GetItemPrice(itmID)>0) return false;
	return true;
}

void DoCharacterUsedItem(ref chref, string itmID)
{
	aref arItm;
	if( Items_FindItem(itmID,&arItm)<0 ) return;
	TakeItemFromCharacter(chref,itmID);

	 // Warship 13.06.09 fix - если только отравлен, а жизни полные (а такое бывает), то нечего и строку в лог выводить об прибавлении жизней
	if(CheckAttribute(arItm,"potion.health") && LAi_GetCharacterHP(chref) < LAi_GetCharacterMaxHP(chref))
	{
		//список разных активных хилок
		int iItemHealSpeed = sti(arItm.potion.health.speed);
		float fItemHealAmt = stf(arItm.potion.health);
		int iQueryIndex = 0;
		string sQueryIndex = ""+iQueryIndex;//строка длиной 1 символ === молния маккуин
		if(!CheckAttribute(chref, "chr_ai.hp_dlt_bottle."+sQueryIndex)) {//первая пошла 
			chref.chr_ai.hp_dlt_bottle.(sQueryIndex) = iItemHealSpeed;
			chref.chr_ai.hp_bottle.(sQueryIndex) = fItemHealAmt;
			setEffectText(chref, XI_ConvertString("Health Up"), argb(255,128,200,128), GetCharCapNotifScale());
		}
		else {
			while (sti(chref.chr_ai.hp_dlt_bottle.(sQueryIndex)) != iItemHealSpeed) {	
				iQueryIndex++;
				sQueryIndex = ""+iQueryIndex;
				if(!CheckAttribute(chref, "chr_ai.hp_bottle."+sQueryIndex)) {//активен ли след слот
					chref.chr_ai.hp_dlt_bottle.(sQueryIndex) = iItemHealSpeed;
				}
			}
			if(chref.chr_ai.hp_dlt_bottle.(sQueryIndex) == iItemHealSpeed) {
				chref.chr_ai.hp_bottle.(sQueryIndex) = stf(chref.chr_ai.hp_bottle.(sQueryIndex)) + fItemHealAmt;
			}
			if(!CheckAttribute(chref, "chr_ai.hp_bottle."+sQueryIndex)) {
				chref.chr_ai.hp_bottle.(sQueryIndex) = "0";
			}
		}
		//список разных активных хилок конец

		if(sti(chref.index)==GetMainCharacterIndex()) {
			Log_SetStringToLog( XI_ConvertString("Health Up")+" ("+GetConvertStr(arItm.name, "ItemsDescribe.txt")+")");
			PlaySound("interface\_Glotok_"+rand(3)+".wav");
			
			int healcount = sti(pchar.questTemp.healcount) + 1;
			pchar.questTemp.healcount = healcount;
			// Открываем достижения
			CheckAndUnlock("heal_bottles", healcount);
		}
	}

	// Warship 13.06.09 fix - если не отравлен, то нечего и строку в лог выводить
	if(CheckAttribute(arItm,"potion.antidote") && LAi_IsPoison(chref))
	{
		LAi_UseAtidoteBottle(chref);
		if(sti(chref.index)==GetMainCharacterIndex()) {
			Log_SetStringToLog( XI_ConvertString("You are cured from poison") );
		}
		else{
			Log_SetStringToLog(GetFullName(chref) + XI_ConvertString("are cured from poison") );
		}
	}

	//navy --> алкоголь
	if (CheckAttribute(arItm, "potion.drunk"))
	{
		LAi_AlcoholSetDrunk(chref, stf(arItm.potion.drunk), stf(arItm.potion.drunk.time));
		if(sti(chref.index)==GetMainCharacterIndex())
		{
			Log_SetStringToLog( XI_ConvertString("You're get drunk") );
		}
	}
	//<--
	if( CheckAttribute(arItm,"potion.sound") )
	{
		PlaySound(arItm.potion.sound);
	}
}

float MinHealthPotionForCharacter(ref chref)
{
	float ftmp;
	bool isFinded = false;

	for(int n=0; n<ITEMS_QUANTITY; n++)
	{
		if( CheckAttribute(&Items[n],"potion") )
		{
			if(CheckAttribute(&Items[n],"potion.health"))
			{
				if( GetCharacterItem(chref,Items[n].id)>0 )
				{
					if(isFinded)
					{
						if( stf(Items[n].potion.health)<ftmp )
						{
							ftmp = stf(Items[n].potion.health);
						}
					}else{
						ftmp = stf(Items[n].potion.health);
						isFinded = true;
					}
				}
			}
		}
	}
	if(!isFinded) return 0.0;
	return ftmp;
}

// LEO: зелье искали перебором ВСЕХ предметов игры до конца, да ещё и звали GetCharacterItem на каждом
// (в Storm && не ленивый, вторая проверка шла и там, где атрибута зелья нет). Идём с конца и выходим на
// первой находке - выбор тот же, что и раньше (брался последний подходящий), работы в разы меньше.
// Зовётся каждый кадр на каждого бойца, см. LAi_CombatTryHeal. Величина нехватки НР на выбор не влияет.
string FindHealthForCharacter(ref chref)
{
	int n;
	for (n = ITEMS_QUANTITY - 1; n >= 0; n--)
	{
		if (!CheckAttribute(&Items[n], "potion.health")) continue;
		if (GetCharacterItem(chref, Items[n].id) > 0) return Items[n].id;
	}
	return "";
}

string FindFoodForCharacter(ref chref, float fEnergy)
{
	string sret = "";
	for(int n = 0; n < 5; n++){//проверяем только еду
		if (GetCharacterItem(chref, Items[n].id) > 0){
			if(stf(Items[n].Food.energy) <= fEnergy)
			{
				sret = Items[n].id
			}
		}
	}
	return sret;
}
int FindPotionFromChr(ref chref, ref arFind, int startIdx)
{
	int i;
	aref arItm;
	for(i=startIdx; i<ITEMS_QUANTITY; i++)
	{
		makearef(arItm,Items[i]);
		if( CheckAttribute(arItm,"potion") && GetCharacterItem(chref,Items[i].id)>0 )
		{
			arFind = arItm;
			return i;
		}
	}
	return -1;
}

int UseBestPotion(ref chref, bool needAntidote)
{
	int i;
	int curPotionID = -1;
	int curPotionHealAmt = 0;
	int newPotionHealAmt = 0;
	bool potionTooGood = false;
	bool bValidPot;
	int reqHealAmt = LAi_GetCharacterMaxHP(chref) - LAi_GetCharacterHP(chref);
	reqHealAmt = makeint(MakeFloat(reqHealAmt) * 1.2)
	if (reqHealAmt <=0 && !needAntidote)
	{
		return -1;
	}

	aref arItm;
	for(i=1; i<ITEMS_QUANTITY; i++)
	{
		makearef(arItm,Items[i]);
		bValidPot = false;

		if (CheckAttribute(pchar,"usepotionbest"))
		{
			if (!needAntidote && CheckAttribute(arItm,"potion.health"))
			{
				bValidPot = true;
			}
			if (needAntidote && CheckAttribute(arItm,"potion.antidote"))
			{
				bValidPot = true;
			}
		}
		else
		{
			if (!needAntidote && CheckAttribute(arItm,"potion.health") && !CheckAttribute(arItm,"potion.antidote"))
			{
				bValidPot = true;
			}
			else
			{
				if (needAntidote && CheckAttribute(arItm,"potion.antidote"))
				{
					bValidPot = true;
				}
			}
		}

		if( bValidPot && (GetCharacterItem(chref,arItm.id) > 0))
		{
			if (CheckAttribute(pchar,"usepotionbest"))
			{
				if (CheckAttribute(arItm,"potion.health"))
					newPotionHealAmt = arItm.potion.health;
				else
					newPotionHealAmt = 0;

				if (newPotionHealAmt > curPotionHealAmt)
				{
					curPotionID = i;
					curPotionHealAmt = newPotionHealAmt;
				}
			}
			else
			{
				if (CheckAttribute(arItm,"potion.health"))
					newPotionHealAmt = arItm.potion.health;
				else
					newPotionHealAmt = 0;

				if (potionTooGood)
				{
					if (newPotionHealAmt < curPotionHealAmt)
					{
						curPotionID = i;
						curPotionHealAmt = newPotionHealAmt;
					}
				}
				else
				{
					if ((newPotionHealAmt + 1) > curPotionHealAmt)
					{
						if (curPotionHealAmt == 0 || newPotionHealAmt <= reqHealAmt)
						{
							curPotionID = i;
							curPotionHealAmt = newPotionHealAmt;
						}
					}
				}
				if (curPotionHealAmt > reqHealAmt)
				{
					potionTooGood = true;
				}
			}
		}
	}

	if (curPotionID > 0)
	{
		DoCharacterUsedItem(pchar, Items[curPotionID].id);
		return 1;
	}
	else return 0;
}

int FindQuestUsableItem(ref arFind, int startIdx)
{
	int i;
	aref arItm;
	bool bSeaInterface = bSeaActive && !bAbordageStarted;

	if(startIdx<0) startIdx=0;
	for(i=startIdx; i<ITEMS_QUANTITY; i++)
	{
		makearef(arItm,Items[i]);
		if( CheckAttribute(arItm,"quest") && CheckAttribute(arItm,"quest.tex"))// boal 16.03.2004
		{
			if( bSeaInterface && arItm.quest.tex=="QuestCommands" )
			{
				arFind = arItm;
				return i;
			}
		}
	}

	return -1;
}

bool EnablePotionUsing(ref mc, aref arItm)
{
	if( CheckAttribute(arItm,"potion.health") ) {
		if( LAi_GetCharacterHP(mc)<LAi_GetCharacterMaxHP(mc) ) {
			return true;
		}
	}

	return false;
}

// Warship 13.06.09 Для противоядий
bool EnableAntidoteUsing(ref _char, aref _item)
{
	if(CheckAttribute(_item, "potion.antidote") && LAi_IsPoison(_char) && CheckCharacterItem(_char, _item.Id))
	{
		return true;
	}

	return false;
}

bool FindCharacterAntidote(ref _char, ref _itemId)
{
	int itemIndex;
	ref item;

	for(itemIndex = 0; itemIndex < ITEMS_QUANTITY; itemIndex++)
	{
		item = &Items[itemIndex];

		if(EnableAntidoteUsing(_char, item))
		{
			_itemID = item.ID;
			return true;
		}
	}

	return false;
}

int FindItem(string sItemID)
{
	// Warship 07.07.09 Перевел на движковую функцию - по-идее, так должно работать быстрее
	return NativeFindCharacter(&Items, GetOriginalItem(sItemID));
}

int BookReadTime(string sBook) //книги, расчёт длительности - Gregg
{
	int intel = sti(pchar.SPECIAL.Intellect);
	int time = 0;

	if (HasSubStr(sBook, "book1_")) time = 4;
	if (HasSubStr(sBook, "book2_")) time = 7;
	if (HasSubStr(sBook, "book3_")) time = 15;
	if (HasSubStr(sBook, "book4_")) time = 30;

	return makeint(time * (1 + ((10 - intel) * 0.285)));
}

int BookBonus(string sBook)
{
	if (HasSubStr(sBook, "book1_")) return 800;
	if (HasSubStr(sBook, "book2_")) return 1500;
	if (HasSubStr(sBook, "book3_")) return 3500;
	if (HasSubStr(sBook, "book4_")) return 7500;
	return 0;
}

void TryReadBook()
{
	if (!CheckAttribute(pchar, "equip.book")) return;

	string sBook = pchar.equip.book;
	pchar.books.(sBook) = sti(pchar.books.(sBook)) - 1;

	if (sti(pchar.books.(sBook)) <= 0)
	{
		aref arItm;
		if (Items_FindItem(sBook, &arItm) < 0) return;

		AddCharacterExpToSkill(pchar, arItm.skill, BookBonus(sBook));
		int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
		Log_Info(GetFullName(pchar) + GetConvertStrWithReplace("Variable_items_utilite_5", "Logs.txt", "#space#", " ")+GetSexPhrase("",GetConvertStrWithReplace("Variable_items_utilite_6", "Logs.txt", "#space#", " "))+GetConvertStrWithReplace("Variable_items_utilite_7", "Logs.txt", "#space#", " ") + LanguageConvertString(idLngFile, arItm.name) + GetConvertStrWithReplace("Variable_items_utilite_8", "Logs.txt", "#space#", " ")+GetSexPhrase("",GetConvertStrWithReplace("Variable_items_utilite_9", "Logs.txt", "#space#", " "))+GetConvertStrWithReplace("Variable_items_utilite_10", "Logs.txt", "#space#", " ") + XI_ConvertString(arItm.skill) + "'");
		LanguageCloseFile(idLngFile);

		RemoveCharacterEquip(pchar, BOOK_ITEM_TYPE);
		RemoveItems(pchar, sBook, 1);

		IncAndUnlock("books", 1);
	}
}

string GetRandomBookbyTier (int tier)// возвращает случайную книгу заданного уровня
{
	return ("book"+tier+"_"+(rand(13)+1));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Warship 08.05.09 НОВАЯ СИСТЕМА ПРЕДМЕТОВ -->
//      Ugeen --> 10.02.10 добавлена первичная генерация предметов и выбор из массива сгенерированных предметов
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Проверка, уникален ли каждый предмет "серии", или же идентичен
#define GEN_ITEM_DISCRET 100
#define GEN_ITEM_SEPARATOR "|"

bool IsGenerableItemIndex(int idx)
{
	ref itemRef = &Items[idx];
	return CheckAttribute(itemRef, "Generation");
}

bool IsGenerableItem(String _itemID)
{
	int itemIndex = GetItemIndex(_itemID);
	if (itemIndex == -1)
	{
		return false;
	}

	return IsGenerableItemIndex(itemIndex);
}

bool IsGeneratedItem(String _itemID)
{
	return findSubStr(_itemID, GEN_ITEM_SEPARATOR, 0) >= 0;
}

string GenerateBladeByParams(string sItemID, float dmg_min, float dmg_max, float weight)
{
	string sOriginalItemID = GetOriginalItem(sItemID);

	// LEO: округляем до сотых, иначе 4.1999998 превращается в 4.19 и ритуал не достигает предела.
	string dmg_min_scaled = round(dmg_min * GEN_ITEM_DISCRET);
	string dmg_max_scaled = round(dmg_max * GEN_ITEM_DISCRET);
	string weight_scaled = round(weight * GEN_ITEM_DISCRET);

	return sOriginalItemID + GEN_ITEM_SEPARATOR +
		dmg_min_scaled + GEN_ITEM_SEPARATOR +
		dmg_max_scaled + GEN_ITEM_SEPARATOR +
		weight_scaled;
}

string GetOriginalItem(string sItemID)
{
	if (sItemID == "") return "";
	int sub = findSubStr(sItemID, GEN_ITEM_SEPARATOR, 0);
	if (sub > 0)
	{
		string result = strcut(sItemID, 0, sub - 1);
		return result;
	}
	return sItemID;
}

string GetBladeParams(string sItemID, ref dmg_min, ref dmg_max, ref weight)
{
	int sub0 = findSubStr(sItemID, GEN_ITEM_SEPARATOR, 0);
	if (sub0 < 0)
	{
		ref item = ItemsFromID(sItemID);
		dmg_min = item.dmg_min;
		dmg_max = item.dmg_max;
		weight = item.weight;
		return sItemID;
	}

	string origItemID = strcut(sItemID, 0, sub0 - 1);

	int sub1 = findSubStr(sItemID, GEN_ITEM_SEPARATOR, sub0 + 1);
	if (sub1 < 0) { return "ERROR"; }
	dmg_min = stf(strcut(sItemID, sub0 + 1, sub1 - 1)) / GEN_ITEM_DISCRET;

	int sub2 = findSubStr(sItemID, GEN_ITEM_SEPARATOR, sub1 + 1);
	if (sub2 < 0) { return "ERROR"; }
	dmg_max = stf(strcut(sItemID, sub1 + 1, sub2 - 1)) / GEN_ITEM_DISCRET;

	weight = stf(strcut(sItemID, sub2 + 1, strlen(sItemID) - 1)) / GEN_ITEM_DISCRET;

	return origItemID;
}

float GetItemWeight(string sItemID)
{
	if (!IsGeneratedItem(sItemID))
	{
		ref item = ItemsFromID(sItemID);
		if (!CheckAttribute(item, "weight"))
		{
			return 0;
		}
		return stf(item.weight);
	}

	float dmg_min, dmg_max, weight;
	GetBladeParams(sItemID, &dmg_min, &dmg_max, &weight);

	return weight;
}

int CalculateBladePrice(string fencingType, float dmg_min, float dmg_max, float weight)
{
	int priceMod = 1;
	switch(fencingType)
	{
		case "FencingLight": // Легкое оружие
			priceMod = 4;
		break;

		case "Fencing": // Среднее оружие
			priceMod = 5;
		break;

		case "FencingHeavy": // Тяжелое оружие
			priceMod = 7;
		break;
	}

	if (weight == 0.0) return 0; //вещи с нулевым весом квестовые и стоить должны нисколько
	return sti(priceMod * dmg_min * dmg_max / weight);
}

int GetItemPrice(String _itemId)
{
	ref item = ItemsFromID(_itemId);

	if (!IsGeneratedItem(_itemId))
	{
		if (!CheckAttribute(item, "price"))
		{
			return -1;
		}
		return sti(item.price);
	}

	float dmg_min, dmg_max, weight;
	GetBladeParams(_itemId, &dmg_min, &dmg_max, &weight);

	return CalculateBladePrice(item.FencingType, dmg_min, dmg_max, weight);
}

// Создадим предмет, вернёт АйДи нового предмета
string GenerateItem(String _itemId)
{
	_itemId = GetOriginalItem(_itemId);

	int itemIndex = GetItemIndex(_itemID);
	if (itemIndex == -1)
	{
		return _itemID;
	}

	ref item = &Items[itemIndex];
	if (!CheckAttribute(item, "Generation")) // Генерящийся ли предмет
	{
		return _itemID;
	}

	int minValue, maxValue;

	// Минимальный урон
	minValue = round(stf(item.Generation.dmg_min.min) * GEN_ITEM_DISCRET);
	maxValue = round(stf(item.Generation.dmg_min.max) * GEN_ITEM_DISCRET);
	float minDmg = stf(minValue + rand(maxValue - minValue)) / GEN_ITEM_DISCRET;

	// Максимальный урон
	minValue = round(stf(item.Generation.dmg_max.min) * GEN_ITEM_DISCRET);
	maxValue = round(stf(item.Generation.dmg_max.max) * GEN_ITEM_DISCRET);
	float maxDmg = stf(minValue + rand(maxValue - minValue)) / GEN_ITEM_DISCRET;

	// Вес
	minValue = round(stf(item.Generation.Weight.min) * GEN_ITEM_DISCRET);
	maxValue = round(stf(item.Generation.Weight.max) * GEN_ITEM_DISCRET);
	float weight = stf(minValue + rand(maxValue - minValue)) / GEN_ITEM_DISCRET;

	return GenerateBladeByParams(_itemId, minDmg, maxDmg, weight);
}

string ModifyGeneratedBlade(string sItemID, float delta_dmg_min, float delta_dmg_max, float delta_weight)
{
	if (!IsGenerableItem(sItemID))
	{
		return sItemID;
	}

	ref item = ItemsFromID(sItemID);

	float dmg_min, dmg_max, weight;
	string origItem = GetBladeParams(sItemID, &dmg_min, &dmg_max, &weight);

	dmg_min += delta_dmg_min;
	if (dmg_min < stf(item.Generation.dmg_min.min)) dmg_min = stf(item.Generation.dmg_min.min);
	if (dmg_min > stf(item.Generation.dmg_min.max)) dmg_min = stf(item.Generation.dmg_min.max);

	dmg_max += delta_dmg_max;
	if (dmg_max < stf(item.Generation.dmg_max.min)) dmg_max = stf(item.Generation.dmg_max.min);
	if (dmg_max > stf(item.Generation.dmg_max.max)) dmg_max = stf(item.Generation.dmg_max.max);

	weight += delta_weight;
	if (weight < stf(item.Generation.weight.min)) weight = stf(item.Generation.weight.min);
	if (weight > stf(item.Generation.weight.max)) weight = stf(item.Generation.weight.max);

	return GenerateBladeByParams(origItem, dmg_min, dmg_max, weight);
}

bool BladeIsPerfect(string sItemID)
{
	if (!IsGenerableItem(sItemID))
	{
		return true;
	}

	ref item = ItemsFromID(sItemID);

	float dmg_min, dmg_max, weight;
	string origItem = GetBladeParams(sItemID, &dmg_min, &dmg_max, &weight);

	return (dmg_min == stf(item.Generation.dmg_min.max)) &&
		(dmg_max == stf(item.Generation.dmg_max.max)) &&
		(weight == stf(item.Generation.weight.min));
}

string ImproveGeneratedBlade(string sItemID, float improveAmount)
{
	if (!IsGenerableItem(sItemID))
	{
		return sItemID;
	}

	ref item = ItemsFromID(sItemID);

	float dmg_min, dmg_max, weight, statNew;
	string origItem = GetBladeParams(sItemID, &dmg_min, &dmg_max, &weight);

	bool dmg_min_perfect = dmg_min == stf(item.Generation.dmg_min.max);
	bool dmg_max_perfect = dmg_max == stf(item.Generation.dmg_max.max);
	bool weight_perfect = weight == stf(item.Generation.weight.min);
	int statsToChoose = !dmg_min_perfect + !dmg_max_perfect + !weight_perfect;
	if (statsToChoose == 0)
	{
		return sItemID;
	}

	int seed = sti((dmg_min + dmg_max + weight) * GEN_ITEM_DISCRET);
	int statInd = seed % statsToChoose;

	if (!dmg_min_perfect)
	{
		if (statInd == 0)
		{
			statNew = (stf(item.Generation.dmg_min.max) - stf(item.Generation.dmg_min.min)) * improveAmount;
			statNew = retMin(dmg_min + statNew, stf(item.Generation.dmg_min.max));
			Log_Info(GetConvertStrWithReplace("Variable_items_utilite_11", "Logs.txt", "#space#", " ") + dmg_min + GetConvertStrWithReplace("Variable_items_utilite_12", "Logs.txt", "#space#", " ") + statNew);
			dmg_min = statNew;
		}
		statInd = statInd - 1;
	}

	if (!dmg_max_perfect)
	{
		if (statInd == 0)
		{
			statNew = (stf(item.Generation.dmg_max.max) - stf(item.Generation.dmg_max.min)) * improveAmount;
			statNew = retMin(dmg_max + statNew, stf(item.Generation.dmg_max.max));
			Log_Info(GetConvertStrWithReplace("Variable_items_utilite_13", "Logs.txt", "#space#", " ") + dmg_max + GetConvertStrWithReplace("Variable_items_utilite_14", "Logs.txt", "#space#", " ") + statNew);
			dmg_max = statNew;
		}
		statInd = statInd - 1;
	}

	if (!weight_perfect)
	{
		if (statInd == 0)
		{
			statNew = (stf(item.Generation.weight.max) - stf(item.Generation.weight.min)) * improveAmount;
			statNew = retMax(weight - statNew, stf(item.Generation.weight.min));
			Log_Info(GetConvertStrWithReplace("Variable_items_utilite_15", "Logs.txt", "#space#", " ") + weight + GetConvertStrWithReplace("Variable_items_utilite_16", "Logs.txt", "#space#", " ") + statNew);
			weight = statNew;
		}
	}

	return GenerateBladeByParams(origItem, dmg_min, dmg_max, weight);
}

//ugeen --> вернем случайный ID сгенерированного зараннее предмета
string GetGeneratedItem(string _itemId)
{
	return GenerateItem(_itemID);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										<-- Warship НОВАЯ СИСТЕМА ПРЕДМЕТОВ
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Warship Проверка на оружие
bool IsBlade(String _itemID)
{
	int itemIndex = GetItemIndex(_itemID);
	ref item;

	if (itemIndex == -1)
	{
		return false;
	}

	item = &Items[itemIndex];

	if (CheckAttribute(&item, "groupID"))
	{
		if (item.groupID == BLADE_ITEM_TYPE)
		{
			return true;
		}
	}

	return false;
}

// eddy -->
/////////////////////// ==> Items-методы
int GetItemIndex(string _ItemID)
{
	int result = FindItem(_ItemID);
	if (result < 0)
	{
		trace("Item not found: " + _ItemID);
	}
	return result;
}

ref ItemsFromID(string _Items)
{
	int iItem = GetItemIndex(_Items); 
	if (iItem >= 0) return &items[iItem]; else trace("Invalid index for item: " + _Items);
}

void ChangeItemDescribe(string _Items, string _Describe)
{
    Items[GetItemIndex(_Items)].describe = _Describe;
}

void BackItemDescribe(string _Items)
{
    ref ItemAR = ItemsFromID(_Items);
    ItemAR.describe = "itmdescr_" + ItemAR.id;
}

void ChangeItemName(string _Items, string _Name)
{
    Items[GetItemIndex(_Items)].name = _Name;
}

void BackItemName(string _Items)
{
    ref ItemAR = ItemsFromID(_Items);
    ItemAR.name = "itmname_" + ItemAR.id;
}
///////////////////////  Items-методы <--


void QuestCheckEnterLocItem(aref _location, string _locator) /// <<<проверка вхождения ГГ в локаторы группы Item.<<<
{
	ref sld;
	int i;
	//======> Генератор инквизиции.
	if (_location.id == "Santiago_Incquisitio" && CheckNPCQuestDate(_location, "AttackGuard_date") && sti(Colonies[FindColony(_location.fastreload)].nation) == SPAIN && findsubstr(_locator, "detector" , 0) != -1)
	{
		SetNPCQuestDate(_location, "AttackGuard_date"); //одна засада в день.

		if (CheckAttribute(pchar, "questTemp.WhisperTutorial"))
		{
			//Стража только вернулась после обеда и немного не в форме
			DoQuestFunctionDelay("WhisperIncqAlarm", 2.5);
		}
		else
		{
			StartIncquisitioAttack();
			LAi_group_AttackGroup("SPAIN_CITIZENS", LAI_GROUP_PLAYER);
		}

		//==>фр.линейка, квест №7. Рок Бразилец, даем диалог.
		if (pchar.questTemp.State == "Fr7RockBras_toSeekPlace") LAi_ActorWaitDialog(characterFromId("RockBrasilian"), pchar);
	}
	//=======> Квест Изабеллы, детектор на скрипт базара Сальватора с братом
	if (_location.id == "SanJuan_town" && pchar.RomanticQuest == "SeeTalkNearHouse")
	{
        pchar.quest.Romantic_DeadBrother_Cancel.over = "yes"; //убираем таймер на вовремя не явлился
		StartQuestMovie(true, true, true);
		pchar.RomanticQuest = "BrotherIsDead";
		LAi_SetActorType(pchar);
		SetMainCharacterIndex(GetCharacterIndex("Husband"));
        locCameraToPos(40.585, 6.0, -47.549, false);
		LAi_SetActorType(PChar);
		PChar.BaseNation = SPAIN; //скрипт в городе, иначе глючит опрос патрулями
		PChar.RomanticQuest = "";
		LAi_SetActorType(CharacterFromID("MigelDeValdes"));
		SetActorDialogAny2Pchar("MigelDeValdes", "", 1.0, 0.0);
		LAi_ActorFollow(pchar, CharacterFromID("MigelDeValdes"), "ActorDialog_Any2Pchar", 0.0);
	}
	//=======> Квест Изабеллы, закрываем дверь дома, если прошлялся не заходя домой более 3 месяцев
	if (_location.id == "SanJuan_town" && pchar.RomanticQuest == "NewLifeForHero")
	{
		if (GetQuestPastMonthParam("RomanticQuest") > 3)
		{
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true);
			pchar.RomanticQuest = "TheDoosIsClosed";
			AddQuestRecord("Romantic_Line", "29");
		}
	}
	//=======> Квест Изабеллы, закрываем дверь дома, если был у шлюх
	if (_location.id == "SanJuan_town" && CheckAttribute(pchar, "RomanticQuest.HorseCheck") && pchar.RomanticQuest.HorseCheck != "-1" && sti(pchar.questTemp.HorseQty) > sti(pchar.RomanticQuest.HorseCheck))
	{
		LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true);
		pchar.RomanticQuest = "over";
		pchar.RomanticQuest.HorseCheck = -1;
		AddQuestRecord("Romantic_Line", "29");
	}
	//======> Генератор маяка Порт Рояля.
	if (_location.id == "Mayak3")
	{
		int iNation = GetCityNation(GetCityNameByIsland(GiveArealByLocation(_location)));
		if(iNation == -1) return;
		string sGroup = GetNationNameByType(iNation)  + "_mayak";
		LAi_group_AttackGroup(sGroup, LAI_GROUP_PLAYER);
	}
	//======> Генератор монстров при входе в локатор духов
	if (_locator == "duhi1" && CheckAttribute(_location, "locators.monsters") && !bMonstersGen)
	{
		//проверяем флаг запрещения генерации
		if(LAi_LocationIsMonstersGen(_location) && LAi_grp_playeralarm == 0 && GenQuest_CheckMonstersGen() && _location.id != "Treasure_alcove")
		{
			if (pchar.sex == "Skeleton" && GetCharacterEquipSuitID(pchar)!= "suit_1")
			{
				sTemp = "skel_"+(rand(3)+1);
				sld = GetCharacter(NPC_GenerateCharacter("Skelet_Drug", sTemp, "skeleton", "skeleton", sti(pchar.rank), PIRATE, -1, true));
				LAi_SetActorType(sld);
				PlaceCharacter(sld, "monsters", PChar.location);
				LAi_ActorDialog(sld, pchar, "", -1, 0);
				sld.lifeday = 0;
				sld.dialog.filename = "Sailor.c";
				sld.dialog.currentnode = "First time";
				LAi_SetImmortal(sld, true);
				LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
				if (rand(20) <= 10+GetSummonSkillFromNameToOld(GetMainCharacter(),SKILL_LEADERSHIP)) // WW нанимаются в команду в % от авторитета (У нежити выше шанс)
				{
					sld.quest.crew = "true";
					sld.quest.crew.qty = 10+rand(14)+(GetSummonSkillFromNameToOld(GetMainCharacter(),SKILL_LEADERSHIP) * 8); // WW 10-24 + 6-60 = 16-84 от авторитета
					sld.quest.crew.type = rand(2);
					sld.quest.crew.money = ((rand(4)+1))*(1+(sti(Pchar.rank)/4))+rand(100);	//Для нежити дешевле
				}
				bMonstersGen = true; //флаг генерации скелетов
				for (i=1; i<=16; i++)
				{
				sTemp = "skel_"+(rand(3)+1);
				sld = GetCharacter(NPC_GenerateCharacter("Skelet_Drug_"+i, sTemp, "skeleton", "skeleton", sti(pchar.rank), PIRATE, -1, true));
				PlaceCharacter(sld, "monsters", "random_free");
				LAi_SetWarriorType(sld);
				sld.lifeday = 0;
				LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
				LAi_CharacterDisableDialog(sld);
				}
			}
			else
			{
				SetSkeletonsToLocation(_location);
			}
		}
	}
	if (_locator == "spawndeadsmangod")
	{
		if (!CheckAttribute(pchar,"UmSamil") && CheckAttribute(pchar,"UmSamilGuardsDefeated"))
		{
			LoginDeadmansGod2();
			pchar.UmSamil = true;
		}
	}
	// =========> Генератор акулы Sinistra
	if (_locator == "akula3" && !CheckAttribute(pchar, "questTemp.CSP_KvM_AkulaLocator3") && CheckAttribute(pchar, "questTemp.CSP_KvM_PoiskGatilio"))
	{
		sld = GetCharacter(NPC_GenerateCharacter("akula", "SharkCharacter", "man", "SharkCharacter", 50, PIRATE, -1, false));
		sld.name = GetConvertStrWithReplace("Variable_items_utilite_1", "Names.txt", "#space#", " ");
		sld.lastname = "";
		LAi_SetImmortal(sld, true);
		sld.lifeday = 0;
		ChangeCharacterAddressGroup(sld, "SantoDomingo_town", "item", "akula1");
		LAi_SetActorType(sld);
		LAi_ActorFollow(sld, pchar, "", -1);
		pchar.questTemp.CSP_KvM_Akula = true;
		pchar.questTemp.CSP_KvM_AkulaLocator3 = true;
		InterfaceStates.Buttons.Save.enable = false;
		bDisableFastReload = true;
		SetTimeScale(1.0);	
		DeleteAttribute(pchar, "quest.CSP_KvM_KusayuTebya_1.over");
		DeleteAttribute(pchar, "quest.CSP_KvM_KusayuTebya_2.over");
		DoQuestFunctionDelay("CSP_KvM_KusayuTebya_1", 15.0);
	}
	if (_locator == "akula2" && CheckAttribute(pchar, "questTemp.CSP_KvM_AkulaLocator3"))
	{		
		sld = CharacterFromID("akula");
		ChangeCharacterAddressGroup(sld, "SantoDomingo_town", "item", "akula1");
		LAi_SetActorType(sld);
		LAi_ActorFollow(sld, pchar, "", -1);
	}
	if (_locator == "akula4" && CheckAttribute(pchar, "questTemp.CSP_KvM_AkulaLocator3"))
	{
		sld = CharacterFromID("akula");
		LAi_SetActorType(sld);
		ChangeCharacterAddressGroup(sld, "none", "", "");
		Event("QuestDelayExit","sl", "", 0);
		DeleteAttribute(pchar, "questTemp.CSP_KvM_Akula");
		DeleteAttribute(pchar, "questTemp.CSP_KvM_AkulaLocator3");
		InterfaceStates.Buttons.Save.enable = true;
		bDisableFastReload = false;
	}
	if (_locator == "akula5" || _locator == "akula6")
	{
		if (CheckAttribute(pchar, "questTemp.CSP_KvM_AkulaLocator3"))
		{
			sld = CharacterFromID("akula");
			LAi_SetActorType(sld);
			ChangeCharacterAddressGroup(sld, "none", "", "");
			Event("QuestDelayExit","sl", "", 0);
			DeleteAttribute(pchar, "questTemp.CSP_KvM_AkulaLocator3");
		}
	}
	// <========= Генератор акулы
	if (_locator == "fire3" && _location.id == "MountainPath")
	{
		//проверяем флаг запрещения генерации
		if(LAi_LocationIsMonstersGen(_location) && LAi_grp_playeralarm == 0 && GenQuest_CheckMonstersGen())
		{
			if (makeint(environment.time) >= 22.0 || makeint(environment.time) < 6.0) SetReefSkeletonsToLocation(_location, "MountainPath");
		}
	}
	if (_location.id == "DeckWithReefs")
	{
		if (_locator == "fire56")
		{
			//проверяем флаг запрещения генерации
			if(LAi_LocationIsMonstersGen(_location) && LAi_grp_playeralarm == 0 && GenQuest_CheckMonstersGen())
			{
				if (makeint(environment.time) >= 22.0 || makeint(environment.time) < 6.0) SetReefSkeletonsToLocation(_location, "DeckWithReefs");
			}
		}
		if (_locator == "item1" && !CheckAttribute(pchar,"talismangot") && CheckAttribute(pchar,"talismanpreget"))
		{
			PlayStereoSound("notebook");
			log_info(GetConvertStrWithReplace("Variable_items_utilite_17", "Logs.txt", "#space#", " "));
			TakeNItems(pchar,"talisman9",1);
			pchar.talismangot = true;
		}
	}
	//======> детектор в тюрьме, вторжение без разрешения
	if (_location.type == "jail" && !sti(pchar.questTemp.jailCanMove) && _locator == "detector1")
	{
		pchar.questTemp.jailCanMove = true; //чтобы не срабатывало 2 раза
		if (!LAi_grp_alarmactive && !IsLocationCaptured(_location.id))
		{
			string slai_group = GetNationNameByType(GetCityNation(_location.parent_colony)) + "_citizens";
			LAi_group_AttackGroup(slai_group, LAI_GROUP_PLAYER);
			LAi_SetFightMode(pchar, true);
		}
	}
	//======> установка метки нахождения в локаторе item1 в погребке
	if (pchar.questTemp.LSC == "toInterception" && _location.id == "FleuronTavern" && _locator == "detector1")
	{
        pchar.questTemp.LSC.itemState = true;
	}
	//======> скафандр
	if (pchar.questTemp.LSC == "toUnderwater" && _location.id == "FenixPlatform")
	{
		if (pchar.model == "protocusto")
		{	//смена со скафандра на норму
			bDisableFastReload = false;
			i = FindLocation("FenixPlatform");
			Locations[i].models.always.inside = "FenixPlatform";
			pchar.model = pchar.questTemp.LSC.immersions.model;
			pchar.model.animation = pchar.questTemp.LSC.immersions.animation;
			LocatorReloadEnterDisable("LostShipsCity_town", "reload58", false);
			LocatorReloadEnterDisable("LostShipsCity_town", "reload73", true);
			DoQuestReloadToLocation("LostShipsCity_town", "reload", "reload72", "");
		}
		else
		{	//смена с нормы на скафандр
			if (sti(pchar.questTemp.LSC.immersions.pay))
			{
				bDisableFastReload = true;
				i = FindLocation("FenixPlatform");
				Locations[i].models.always.inside = "FenixPlatform_wout";
				SetIntegratedMusketDrawn(pchar, false);
				pchar.questTemp.LSC.immersions.model = pchar.model;
				pchar.questTemp.LSC.immersions.animation = pchar.model.animation;
				pchar.model = "protocusto";
				pchar.model.animation = "armor";
				LocatorReloadEnterDisable("LostShipsCity_town", "reload58", true);
				LocatorReloadEnterDisable("LostShipsCity_town", "reload73", false);
				DoQuestReloadToLocation("LostShipsCity_town", "reload", "reload72", "");
			}
		}
	}
	if (_location.id == "Havana_houseS1Bedroom" && pchar.questTemp.State == "Sp4Detection_toMirderPlace") //фикс описания письма по испанке
	{
		ChangeItemDescribe("letter_notes", "itmdescr_letter_notes");
	}
}

void QuestCheckExitLocItem(aref _location, string _locator) /// <<<проверка выхода ГГ из локаторов группы Item.<<<
{
	//=======> Испанская линейка, квест №4. В спальне нашёл возле комода нашёл недописанное письмо
	if (_location.id == "Havana_houseS1Bedroom" && pchar.questTemp.State == "Sp4Detection_toMirderPlace" && CheckCharacterItem(pchar, "letter_notes"))
    {
        DoQuestCheckDelay("TalkSelf_Quest", 0.1); //диалог сам-на-сам
    }
	//=======> Квест Аскольда, нашли гробницу.
    if (_location.id == "Guadeloupe_Cave" && _locator == "detector1")
    {
        if (pchar.questTemp.Ascold == "Ascold_SeekGrave" || pchar.questTemp.Ascold == "Ascold_CantSeekGrave")
        {
            Log_QuestInfo(GetConvertStrWithReplace("Variable_items_utilite_18", "Logs.txt", "#space#", " "));
            pchar.questTemp.Ascold = "Ascold_NotEnterFoundGrave";
    		DoQuestCheckDelay("TalkSelf_Quest", 0.1); //диалог сам-на-сам
        }
    }
	//=======> Квест Аскольда, ГГ добрался до мумии, но бутылки ещё не имеет. Вместо прерывания на локатор.
    if (_location.id == "Guadeloupe_Cave" && _locator == "button02")
    {
		if (pchar.questTemp.Ascold == "Ascold_SeekRockLetter" || pchar.questTemp.Ascold == "Ascold_EnterGrave")
		{
			pchar.questTemp.Ascold = "Ascold_FoundMummy";
			characters[GetCharacterIndex("Ascold")].dialog.currentnode = "Found_Grave";
		}
	}
	//=======> Энкаунтеры заманухи в пещеру, открываем закрытый релоад на колодце.
    if (_locator == "CheckReload1" && CheckAttribute(pchar, "GenQuest.OpenTheRopeExit") && pchar.GenQuest.OpenTheRopeExit == pchar.location)
    {
		DeleteAttribute(pchar, "GenQuest.OpenTheRopeExit");
		LocatorReloadEnterDisable(pchar.location, "reload2", false);
	}
	//=======> Изабелла, в доме начало скрипта с братом
    if (_location.id == "SanJuan_houseSp6" && pchar.RomanticQuest == "exitFromDetector")
    {
		pchar.RomanticQuest = "executeFromDetector";
		StartQuestMovie(true, true, true);
		// ГГ теперь Сальватор
		SetMainCharacterIndex(GetCharacterIndex("MigelDeValdes"));
        PChar   = GetMainCharacter();
		locCameraToPos(-2.41, 2.35, -2.41, false);
		SetActorDialogAny2Pchar("Isabella", "", 3.0, 0.0);
		LAi_ActorFollow(PChar, CharacterFromID("Isabella"), "ActorDialog_Any2Pchar", 4.0);
	}
	//=======> Изабелла, в доме Роситы после смерти мужа
    if (_location.id == "Beliz_houseS4" && pchar.RomanticQuest == "Beliz_exitFromDetector")
    {
		pchar.RomanticQuest = "Beliz_executeFromDetector";
		StartQuestMovie(true, true, true);
        // ГГ теперь Изабелла
		ChangeCharacterAddressGroup(CharacterFromID("Isabella"), pchar.location, "goto",  "goto3");
		ChangeCharacterAddressGroup(CharacterFromID("Rosita"), pchar.location, "goto",  "goto1");
        SetMainCharacterIndex(GetCharacterIndex("Isabella"));
        PChar   = GetMainCharacter();
        locCameraToPos(-3.84, 2.35, 0.85, false);
		LAi_SetActorType(CharacterFromID("Rosita"));
		LAi_SetActorType(pchar);
		PChar.BaseNation = SPAIN; //скрипт в городе, иначе глючит опрос патрулями
		SetActorDialogAny2Pchar("Rosita", "", 1.0, 0.0);
		LAi_ActorFollow(PChar, CharacterFromID("Rosita"), "ActorDialog_Any2Pchar", 0.0);
	}
	//=======> Пиратка, квест №5
    if (_location.id == "Pirates_houseS1" && pchar.questTemp.piratesLine == "PL5Hunter_exitReload")
    {
		pchar.questTemp.piratesLine = "PL5Hunter_executeReload";
		StartQuestMovie(true, true, true);
		SetMainCharacterIndex(GetCharacterIndex("Orry"));
        PChar   = GetMainCharacter();
		locCameraToPos(1.34, 2.1, 1.68, false);
		LAi_SetActorType(CharacterFromID("Aivory"));
		LAi_SetActorType(pchar);
		SetActorDialogAny2Pchar("Aivory", "", 3.0, 0.0);
		LAi_ActorFollow(PChar, CharacterFromID("Aivory"), "ActorDialog_Any2Pchar", 4.0);
	}
	//======> установка метки нахождения в локаторе item1 в погребке
	if (pchar.questTemp.LSC == "toInterception" && _location.id == "FleuronTavern" && _locator == "detector1")
	{
		pchar.questTemp.LSC.itemState = false;
	}
}

void QuestCheckUseButton(aref _location, string _locator, string _itemId) /// <<< квестовые действия при установке предметов в button <<<
{
    //==> Квест Аскольда, установка скрижалей для открытия входа в пещеру к гробнице.
    if (_location.id == "Guadeloupe_Cave" && _locator == "button01")
    {
		//==> перебиваем иниты пещеры Гваделупы
		_location.reload.l1.name = "reload3_back";
		locations[FindLocation("Guadeloupe_CaveEntrance")].reload.l1.emerge = "reload3";
		//==> перемещаемся на reload5
		DoQuestFunctionDelay("Ascold_OpenTheGrave", 1.5);
    }
    //==> Квест Аскольда, вскрытие гробницы кочергой.
    if (_location.id == "Guadeloupe_Cave" && _locator == "button02" && pchar.questTemp.Ascold == "Ascold_WateringMummy")
    {
        ref sld;
		for (int i=1; i<=3; i++)
        {
			sld = GetCharacter(NPC_GenerateCharacter("Enemy_"+i, "Skel"+i, "skeleton", "skeleton", 30, PIRATE, 0, true));
			FantomMakeCoolFighter(sld, 30, 85, 85, "topor2", "pistol3", 100);
            LAi_SetWarriorType(sld);
			LAi_group_MoveCharacter(sld, "EnemyFight");
            ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto"+i);
        }
        LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
        LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
        LAi_group_SetCheck("EnemyFight", "Ascold_InGraveAfterFight");
        chrDisableReloadToLocation = true;
    }
    //==> Квест Аскольда, вскрытие лампы и появление Аззи на верфи
    if (_location.id == (pchar.questTemp.Ascold.ShipyarderColony + "_shipyard") && _locator == "button01")
    {
		LAi_SetActorType(PChar);
		DoQuestCheckDelay("TalkSelf_Quest", 0.2);
    }
	//==> Пиратка, квест №7
    if (_location.id == "PuertoPrincipe_town" && _locator == "button01")
    {
		ref loc = &locations[FindLocation("PuertoPrincipe_townhall")];
		loc.reload.l2.name = "reload2";
		loc.reload.l2.go = "PuertoPrincipe_town";
		loc.reload.l2.emerge = "reload9";
		loc.reload.l2.autoreload = "0";
		loc.reload.l2.label = "Street";
		DoQuestFunctionDelay("PQ7_loadToRecidence", 2.0);
	}
	//установки тотемов в храмах
    if (findsubstr(_itemId, "Totem_" , 0) != -1)
    {
		SetItemModelOnLocation(_location, _itemId, _locator);
		SetAztecUsedTotem(_location, _itemId, _locator);
	}
	//открываем доступ в храм к шотгану
    if (_itemId == "KnifeAztec")
    {
		SetItemModelOnLocation(_location, _itemId, _locator);
		LocatorReloadEnterDisable("Tenochtitlan", "reloadTemple31", false);
	}
	// Квест Загадочный остров -->
	if(_itemId == "MysteriousIsland_Idol")
	{
		DoQuestFunctionDelay("MysteriousIslandIdolInstall", 3.0);
	}
	// Квест Загадочный остров <--
}
// Gregg: квестовый гейт НА подбор предмета из локатора item. false = предмет не отдаём.
// Зовётся из Item_OnPickItem ДО выдачи (в отличие от QuestCheckTakeItem, который срабатывает уже после).
// Причину отказа объясняем сами - молчаливый отказ игрок читает как баг.
bool QuestCheckCanTakeItem(aref _location, string _itemId)
{
	// Gregg: нефритовый череп - пока в Великом храме жива стража, череп не отдаётся. Это же убирает абуз,
	// когда Миктлантекутли выманивали на уцелевших ацтеков: к его выходу их уже нет в живых.
	if (_itemId == "SkullAztec")
	{
		// Считаем живых НАПРЯМУЮ, а не полагаемся на флаг: групповой чек мог не сработать
		// (умер не последний логиненный, застрявший страж и т.п.) - тогда флаг соврал бы в плюс.
		int iAlive = Teno_CountAliveGuards();
		if (iAlive > 0)
		{
			Log_Info(GetConvertStrWithReplace("Variable_teno_skull_locked", "Logs.txt", "#space#", " ") + iAlive);
			return false;
		}
		// Живых нет, но храм и не зачищали (напр. вошли в клетки подводным ходом - стража не ставилась).
		if (!CheckAttribute(_location, "TenoGuardsCleared"))
		{
			Log_Info(GetConvertStrWithReplace("Variable_teno_skull_sealed", "Logs.txt", "#space#", " "));
			return false;
		}
	}
	return true;
}

//проверка взятия предметов из локатора item
void QuestCheckTakeItem(aref _location, string _itemId)
{
	//квест промышленного шпионажа не верфях
	if (_itemId == "ShipyardsMap")
	{
		AddQuestRecord("ShipyardsMap", "2");
		if (IsLoginTime() && !IsLocationCaptured(_location.id))
		{
			ref sld = characterFromId(_location.fastreload + "_shipyarder");
			sld.dialog.currentnode = "Allarm";
			LAi_SetActorType(pchar);
			LAi_ActorDialog(pchar, sld, "", 0, 0);
		}
	}
	//генератор - "Спихнуть судовые документы"
	if (_itemId == "CaptainBook")
	{
		ReOpenQuestHeader("GiveShipLetters");
		AddQuestRecord("GiveShipLetters", "1");
		AddQuestUserData("GiveShipLetters", "sSex", GetSexPhrase("","а"));
	}
	//квест поиски кольца мэра в борделях
	if (_itemId == "MayorsRing")
	{
		AddQuestRecord("SeekMayorsRing", "6");
		AddQuestUserData("SeekMayorsRing", "sCity", XI_ConvertString("Colony" + pchar.questTemp.different.TakeMayorsRing.city + "Gen"));
	}
	//квест поиска драгоценного камня ростовщика
	if (_itemId == "UsurersJew")
	{
		// LEO: реагируем только если квест реально активен. Иначе «ничей» камень (отменённый квест / протухший старый сейв) давал ложную всплывашку «выполнил», а сдать некому.
		if (CheckAttribute(pchar, "questTemp.different.SeekUsurersJewel"))
		{
			AddQuestRecord("SeekUsurersJewel", "2");
			if (drand(1)==0) SpawnGreedyBastards();
		}
	}
	//пиратка, квест №7
	if (_itemId == "OpenBook")
	{
		AddQuestRecord("Pir_Line_7_Soukins", "9");
		SaveCurrentQuestDateParam("questTemp.piratesLine");
		pchar.questTemp.piratesLine = "Soukins_catchBattleship";
		Pchar.quest.PQ7_setBattleShip.win_condition.l1 = "MapEnter";
	    Pchar.quest.PQ7_setBattleShip.function = "PQ7_setBattleShip";
	}
	//линейка ГПК, квест со скелетом Декстера. найденный ключ адмирала
	if (_itemId == "keyPanama" && CheckAttribute(pchar, "questTemp.LSC.lostDecster") && pchar.questTemp.LSC.lostDecster == "admiralLostKey")
	{
		pchar.quest.LSC_admiralFoundOwnKey.over = "yes"; //снимаем таймер
		AddQuestRecord("LSC_findDekster", "12");
		CloseQuestHeader("LSC_findDekster");
		pchar.questTemp.LSC.lostDecster = "over";
	}
	//меняем сеабед у Тено, снимаем 11 тотем
	if (_itemId == "Totem_11")
	{
		_location.models.always.seabed = "TenochtitlanWout_sb";
	}
	//взятие нефритового черепа
	if (_itemId == "SkullAztec")
	{
		LoginDeadmansGod();
	}
}

void StartIncquisitioAttack()
{
	ref sld;
    for (int i=1; i<=5; i++)
    {
		sld = GetCharacter(NPC_GenerateCharacter("IncqAddGuard_"+i, "sold_spa_"+(rand(7)+1), "man", "man", 35, SPAIN, 1, true));
		FantomMakeCoolFighter(sld, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE*3+8, 80, 70, BLADE_LONG, "pistol4", 50);
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
        ChangeCharacterAddressGroup(sld, "Santiago_Incquisitio", "goto", LAi_FindRandomLocator("goto"));
    }
}

void SetItemModelOnLocation(ref loc, string model, string locator)
{
	loc.models.always.totem = model;
	loc.models.always.totem.locator.group = "item";
	loc.models.always.totem.locator.name = locator;
	loc.models.always.totem.tech = "DLightModel";
}

// Gregg: ЯДРО автозакупки припасов (вынесено из interface\itemstrade.c, чтобы работало и без открытого экрана торговли).
//   itemstrade.c - сегмент (LoadSegment), его функции живут только пока открыт экран; фоновому рейду казначея
//   (Treasurer_PortRun в store\StoreUtilite.c) они нужны вне интерфейса. Логика 1-в-1, отличия:
//   торговец приходит параметром (был глобал refStoreChar), bSilent глушит интерфейсный лог покупок
//   (AddTradeLog пишет в GameInterface - вне экрана звать нельзя), rStat копит сводку (деньги/позиции).
//   Обёртки Trade_* с прежними именами остались в itemstrade.c.
extern void AddTradeLog(string sItem, int sellQty, int sellcost); // живёт в сегменте itemstrade.c, зовётся только при !bSilent (экран открыт)

void TradeStat_Add(ref rStat, int iMoney, int iCount)
{
	if (!CheckAttribute(rStat, "money")) rStat.money = 0;
	if (!CheckAttribute(rStat, "cnt")) rStat.cnt = 0;
	rStat.money = sti(rStat.money) + iMoney;
	rStat.cnt = sti(rStat.cnt) + iCount;
}

// Строка журнала рейда казначея для отчётного экрана (в интерфейсной торговле не пишется - там свой TABLE_LOG).
// iOwner - индекс персонажа-получателя, REPORT_OWNER_CHEST - сундук каюты. Одинаковые предмет+получатель сливаются
// в одну строку (два лоточника могут добить один и тот же припас одному офицеру).
#define REPORT_OWNER_CHEST	-2

void TradeReport_AddBuy(ref rStat, string sItem, int qty, int cost, int iOwner, bool bGoods)
{
	if (!CheckAttribute(rStat, "rows")) rStat.rows = 0;
	int n = sti(rStat.rows);
	string sRow;
	int i;
	for (i = 0; i < n; i++)
	{
		sRow = "r" + i;
		if (rStat.buy.(sRow).item != sItem) continue;
		if (sti(rStat.buy.(sRow).owner) != iOwner) continue;
		rStat.buy.(sRow).qty = sti(rStat.buy.(sRow).qty) + qty;
		rStat.buy.(sRow).cost = sti(rStat.buy.(sRow).cost) + cost;
		return;
	}
	sRow = "r" + n;
	rStat.buy.(sRow).item = sItem;
	rStat.buy.(sRow).qty = qty;
	rStat.buy.(sRow).cost = cost;
	rStat.buy.(sRow).owner = iOwner;
	if (bGoods) rStat.buy.(sRow).goods = true; // товар в трюм, а не предмет в инвентарь
	rStat.rows = n + 1;
}

// расходник ли это (припасы/крафт) - общий фильтр списков закупки
bool Trade_IsConsumeItem(string sItem)
{
	if (IsGenerableItem(sItem)) return false;
	ref rItem = ItemsFromID(sItem);
	if (!CheckAttribute(rItem, "sortIndex"))
	{
		if (!HasSubStr(sItem, "CompCraft")) return false; // в списке только расходники
	}
	else
	{
		if (rItem.SortIndex == 1) {}
		else
		{
			if (rItem.SortIndex == 2) {}
			else
			{
				if (rItem.ItemType != "CRAFTCOMPONENTS") return false; // в списке расходники и крафт
			}
		}
	}
	if (sItem == "Lockpick") return false; // отмычки пропускаем
	if (sItem == "CompCraft_Tools") return false; // исключение выбранных Шахом штук
	if (sItem == "CompCraft_Locksmith") return false;
	if (sItem == "CompCraft_Puleleyka") return false;
	return true;
}

void Trade_BuyConsume(ref chr, ref rTrader, bool bSilent, ref rStat)
{
	int 	qty = 0;
	int 	i, iCost;
	int		curqty = 0;
	int		curTraderQty = 0;
	int		iMoneyQty = 0;
	int		iBought = 0;
	bool	bAdded, bNotEnough, bNotOverWeight, bNotEnoughTrader;
	float	freeWeight, fItemWeight;

	string sTemp = "";
	if (chr.sex == "woman") sTemp = GetConvertStrWithReplace("Variable_itemstrade_22", "Interface.txt", "#space#", " ");

	freeWeight = GetMaxItemsWeight(chr) - GetItemsWeight(chr);
	if (freeWeight < 0.1)
	{
		if (!bSilent) Log_Info(GetFullName(chr) + GetConvertStrWithReplace("Variable_itemstrade_23", "Interface.txt", "#space#", " ") + sTemp + GetConvertStrWithReplace("Variable_itemstrade_24", "Interface.txt", "#space#", " "));
		return;
	}

	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	bAdded = false;//будет ли хоть один предмет добавлен
	bNotOverWeight = false;//хватало ли места в инвентаре
	bNotEnough = false;//хватало ли уже всего
	bNotEnoughTrader = false;//хватало ли у торговца

	aref arInventory, arItem;
	string sItem;
	ref rItem;

	makearef(arInventory, chr.TransferItems.I);
	for (i = 0; i < GetAttributesNum(arInventory); i++)
	{
		arItem = GetAttributeN(arInventory, i);
		sItem = GetAttributeName(arItem);
		rItem = ItemsFromID(sItem);

		if (!Trade_IsConsumeItem(sItem)) continue;

		curTraderQty = GetCharacterItem(rTrader, sItem);
		qty = sti(arInventory.(sItem));
		curqty = GetCharacterItem(chr, sItem);
		if (qty > curqty) {qty = qty - curqty;}
			else continue;//этого припаса уже было достаточно
		if (qty > curTraderQty) {qty = curTraderQty; bNotEnoughTrader = true;}//без возможности покупать из воздуха
		if (qty == 0) continue; //нечего купить у торговца

		bNotEnough = true;//если сюда дошли хоть раз за цикл, значит чего-то не хватало до лимита

		fItemWeight = GetItemWeight(sItem);
		if (qty > makeint(freeWeight / fItemWeight)) qty = makeint(freeWeight / fItemWeight);
		if (qty == 0) continue; //не хватает места даже для одного такого предмета
		bNotOverWeight = true;//хватало места хотя бы для одного предмета

		iCost = GetTradeItemPrice(sItem, PRICE_TYPE_BUY);
		if (iCost < 1) iCost = 1; // защита от деления на ноль
		if (qty > (sti(PChar.Money) + iMoneyQty) / iCost) qty = (sti(PChar.Money) + iMoneyQty) / iCost;//остаток С УЧЁТОМ уже потраченного в этом прогоне
		if (qty < 1) continue; //не хватает денег даже на один такой предмет

		iMoneyQty -= iCost*qty;
		iBought += qty;
		TakeNItems(chr, sItem, qty);
		TakeNItems(rTrader, sItem, -qty);
		if (bSilent) TradeReport_AddBuy(rStat, sItem, qty, -iCost*qty, sti(chr.index), false);
		if (!bSilent)
		{
			AddTradeLog(sItem, qty, -iCost*qty);
			if (GetFullName(chr) == GetFullName(PChar)) Log_Info(GetFullName(chr) + GetConvertStrWithReplace("Variable_itemstrade_26", "Interface.txt", "#space#", " ") + "'" + LanguageConvertString(idLngFile, rItem.name) + "' " + qty + GetConvertStrWithReplace("Variable_itemstrade_27", "Interface.txt", "#space#", " ") + iCost*qty + GetConvertStrWithReplace("Variable_itemstrade_28", "Interface.txt", "#space#", " "));
		}
		freeWeight -= fItemWeight*qty;
		bAdded = true;//что-то успешно докупили
	}

	if (iMoneyQty != 0) // Если хоть что-то купили
	{
		AddmoneyToCharacter(PChar, iMoneyQty);
		ref rTreasurer = GetPCharTreasurerRef();//Казначей. Ему даем экспу
		int iExpAdd = MakeInt(abs(iMoneyQty * 0.5) / 700) + 1;
		AddCharacterExpToSkill(rTreasurer, "Commerce", iExpAdd);
		AddCharacterExpToSkill(pchar, "Commerce", iExpAdd);
		TradeStat_Add(rStat, iMoneyQty, iBought);
	}

	LanguageCloseFile(idLngFile);

	if (bSilent) return;
	if (CheckAttribute(&InterfaceStates, "AutoSellLogs"))
	{
		if (bAdded) 			{Log_Info(GetFullName(chr)+GetConvertStrWithReplace("Variable_itemstrade_29", "Interface.txt", "#space#", " ") + sTemp + GetConvertStrWithReplace("Variable_itemstrade_30", "Interface.txt", "#space#", " "));	return;}
		if (bNotOverWeight)		{Log_Info(GetFullName(chr)+GetConvertStrWithReplace("Variable_itemstrade_31", "Interface.txt", "#space#", " ") + sTemp + GetConvertStrWithReplace("Variable_itemstrade_32", "Interface.txt", "#space#", " ")); return;}
		if (bNotEnough) 		{Log_Info(GetFullName(chr)+GetConvertStrWithReplace("Variable_itemstrade_33", "Interface.txt", "#space#", " ") + sTemp + GetConvertStrWithReplace("Variable_itemstrade_34", "Interface.txt", "#space#", " ")); return;}
		if (bNotEnoughTrader) 	{Log_Info(GetFullName(chr)+GetConvertStrWithReplace("Variable_itemstrade_35", "Interface.txt", "#space#", " ") + sTemp + GetConvertStrWithReplace("Variable_itemstrade_36", "Interface.txt", "#space#", " ")); return;}
							 	 Log_Info(GetFullName(chr)+GetConvertStrWithReplace("Variable_itemstrade_37", "Interface.txt", "#space#", " ") + sTemp + GetConvertStrWithReplace("Variable_itemstrade_38", "Interface.txt", "#space#", " "));
	}
}

void Trade_SellExcessConsume(ref chr, ref rTrader, bool bSilent, ref rStat)
{
	if (!CheckAttribute(chr,"TransferItems.SellRestriction")) return;//не включено разрешение продавать
	int 	qty = 0;
	int 	i, iCost;
	int		curqty = 0;
	int		iMoneyQty = 0;
	int		iSold = 0;

	string sTemp = "";
	if (chr.sex == "woman") sTemp = GetConvertStrWithReplace("Variable_itemstrade_39", "Interface.txt", "#space#", " ");

	aref arInventory, arItem;
	string sItem;

	makearef(arInventory, chr.TransferItems.I);
	for (i = 0; i < GetAttributesNum(arInventory); i++)
	{
		arItem = GetAttributeN(arInventory, i);
		sItem = GetAttributeName(arItem);

		if (!Trade_IsConsumeItem(sItem)) continue;

		qty = sti(arInventory.(sItem));
		curqty = GetCharacterItem(chr, sItem);
		if (curqty > qty) qty = curqty - qty; else continue;
		iCost = GetTradeItemPrice(sItem, PRICE_TYPE_SELL);
		iMoneyQty += iCost*qty;
		iSold += qty;
		TakeNItems(chr, sItem, -qty);
		TakeNItems(rTrader, sItem, qty);
		if (bSilent) TradeReport_AddBuy(rStat, sItem, -qty, iCost*qty, sti(chr.index), false);
		if (!bSilent) AddTradeLog(sItem, -qty, iCost*qty);

	}
	if(iMoneyQty != 0) // Если хоть что-то продали
	{
		AddmoneyToCharacter(PChar, iMoneyQty);
		ref rTreasurer = GetPCharTreasurerRef();//Казначей. Ему даем экспу
		int iExpAdd = MakeInt(abs(iMoneyQty * 0.5) / 1000) + 1;
		AddCharacterExpToSkill(rTreasurer, "Commerce", iExpAdd);
		AddCharacterExpToSkill(pchar, "Commerce", iExpAdd);
		TradeStat_Add(rStat, iMoneyQty, iSold);
		if (!bSilent) Log_Info(GetFullName(chr)+GetConvertStrWithReplace("Variable_itemstrade_40", "Interface.txt", "#space#", " ") + sTemp + GetConvertStrWithReplace("Variable_itemstrade_41", "Interface.txt", "#space#", " "));
	}
}

// Казначей закупает предметы по списку капитана в первый сундук текущей каюты.
void Trade_AutobuyChest(ref rTrader, bool bSilent, ref rStat)
{
	int iCabin = Trade_GetCabinChestLocation();
	if (iCabin < 0) return;
	if (!CheckAttribute(pchar, "CabinChestTrade.TransferItems.I")) return;
	aref atcChest;
	makearef(atcChest, locations[iCabin].box1);
	ref rChest;
	makeref(rChest, atcChest); // ref на сундук - совместим с GetCharacterItem/TakeNItems (как ручная покупка в бокс)

	int qty, i, iCost, curqty, curTraderQty;
	int iMoneyQty = 0;
	int iBought = 0;
	string sChestName;
	int idLngFile = -1;
	if (!bSilent)
	{
		sChestName = GetConvertStrWithReplace("Variable_itemsbox_31", "Interface.txt", "#space#", " ");
		idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	}
	aref arInventory, arItem, rItem;
	string sItem;
	makearef(arInventory, pchar.CabinChestTrade.TransferItems.I);
	int itemCount = GetAttributesNum(arInventory);
	for (i = 0; i < itemCount; i++)
	{
		arItem = GetAttributeN(arInventory, i);
		sItem = GetAttributeName(arItem);
		if (IsGenerableItem(sItem) || Items_FindItem(sItem, &rItem) < 0) continue;
		curTraderQty = GetCharacterItem(rTrader, sItem);
		qty = sti(arInventory.(sItem));
		curqty = GetCharacterItem(rChest, sItem);
		if (qty > curqty) {qty = qty - curqty;}
			else continue; // в сундуке уже достаточно
		if (qty > curTraderQty) qty = curTraderQty; // без покупки из воздуха
		if (qty == 0) continue;
		iCost = GetTradeItemPrice(sItem, PRICE_TYPE_BUY);
		if (iCost < 1) iCost = 1;
		if (qty > (sti(PChar.Money) + iMoneyQty) / iCost) qty = (sti(PChar.Money) + iMoneyQty) / iCost;
		if (qty < 1) continue; // не хватает денег
		if (!TakeNItems(rChest, sItem, qty)) continue;
		TakeNItems(rTrader, sItem, -qty);
		iMoneyQty -= iCost*qty;
		iBought += qty;
		if (bSilent) TradeReport_AddBuy(rStat, sItem, qty, -iCost*qty, REPORT_OWNER_CHEST, false);
		if (!bSilent)
		{
			AddTradeLog(sItem, qty, -iCost*qty);
			string itemName = sItem;
			if (idLngFile >= 0) itemName = LanguageConvertString(idLngFile, rItem.name);
			Log_Info(sChestName + GetConvertStrWithReplace("Variable_itemstrade_26", "Interface.txt", "#space#", " ") + "'" + itemName + "' " + qty + GetConvertStrWithReplace("Variable_itemstrade_27", "Interface.txt", "#space#", " ") + iCost*qty + GetConvertStrWithReplace("Variable_itemstrade_28", "Interface.txt", "#space#", " "));
		}
	}
	if (iMoneyQty != 0) // если хоть что-то купили - деньги и опыт казначею, как у обычной автозакупки
	{
		AddmoneyToCharacter(PChar, iMoneyQty);
		ref rTreasurer = GetPCharTreasurerRef();
		int iExpAdd = MakeInt(abs(iMoneyQty * 0.5) / 700) + 1;
		AddCharacterExpToSkill(rTreasurer, "Commerce", iExpAdd);
		AddCharacterExpToSkill(pchar, "Commerce", iExpAdd);
		TradeStat_Add(rStat, iMoneyQty, iBought);
	}
	if (idLngFile >= 0) LanguageCloseFile(idLngFile);
}
