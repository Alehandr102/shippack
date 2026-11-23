
//Подключённые типы для интелекта

// Gregg: боевое самолечение NPC — было 3 дословные копии в warrior/guardian/patrol
// (officer лечится по своей схеме: fCheck без множителя сложности + детокс зельями — не трогать)
void LAi_CombatTryHeal(aref chr)
{
	float fCheck = (MOD_SKILL_ENEMY_RATE - 1) * CheckAttribute(chr, "chr_ai.hp_bottle.0");//слот 0 юзается последним
	if(fCheck > 0) return;
	// LEO: цел - и лечиться незачем. Порог проверяем ДО поиска зелья: раньше весь гарнизон каждый
	// кадр перебирал список предметов, чтобы в конце выяснить, что лечить некого.
	float dhlt = stf(chr.chr_ai.hp_max) - stf(chr.chr_ai.hp);
	if (dhlt <= 20.0) return;
	string sItem = FindHealthForCharacter(chr);
	if (sItem != "") {
		DoCharacterUsedItem(chr, sItem);//чек уже есть
	}
}

// Gregg: боевой перекус NPC при тревоге — было 4 дословные копии в warrior/guardian/patrol/officer
void LAi_CombatTryEat(aref chr)
{
	// LEO: перекус бывает только по тревоге, поэтому флаг проверяем первым. Раньше вне боя каждый
	// NPC каждый кадр всё равно считал свою энергию и лез за атрибутом - в Storm && не ленивый.
	if (!LAi_grp_alarmactive) return;
	float dfood;
	string sItem;
	float fCharEnergy = LAi_GetCharacterEnergy(chr);
	float fCharMaxEnergy = LAi_GetCharacterMaxEnergy(chr);
	if (fCharEnergy >= 0.5 * fCharMaxEnergy) return;
	//активен ли реген еды
	if (CheckAttribute(chr, "chr_ai.FoodEnergy.0")) {
		dfood = stf(chr.chr_ai.FoodEnergy.0);
	}
	else {
		dfood = 0;
	}
	if (dfood) return;	// уже жуёт
	dfood = fCharMaxEnergy - fCharEnergy;
	sItem = FindFoodForCharacter(chr, dfood);
	if (sItem != "") {
		DoCharacterUsedFood(&Characters[sti(chr.index)], sItem);
	}
}

// Gregg: стандартные диалоговые коллбэки типов (было 14 копий StartDialog и 6 копий EndDialog;
// типы с особой логикой — poor/huber/actor/sit/barman/owner/patrol/citizen и др. — оставлены своими)
void LAi_StdStartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

void LAi_StdEndDialog(aref chr)
{
	LAi_tmpl_stay_InitTemplate(chr);
	LAi_CharacterRestoreAy(chr);
}

//Игрок
#include "Loc_ai\types\LAi_player.c"
//Офицер
#include "Loc_ai\types\LAi_officer.c"
//Горожанин
#include "Loc_ai\types\LAi_citizen.c"
//Охраник
#include "Loc_ai\types\LAi_guardian.c"
//Патруль
#include "Loc_ai\types\LAi_patrol.c"
//Воин
#include "Loc_ai\types\LAi_warrior.c"
//Торговец
#include "Loc_ai\types\LAi_merchant.c"
//Стоячий человек
#include "Loc_ai\types\LAi_stay.c"
//Сидячий человек
#include "Loc_ai\types\LAi_sit.c"
//Бармен
#include "Loc_ai\types\LAi_barman.c"
//Официантка
#include "Loc_ai\types\LAi_waitress.c"
//Нищий
#include "Loc_ai\types\LAi_poor.c"
//Губернатор, сидячий на месте
#include "Loc_ai\types\LAi_huber.c"
//Губернатор, ходячий по помещению
#include "Loc_ai\types\LAi_huberstay.c"
//Священик
#include "Loc_ai\types\LAi_priest.c"
//Актёр
#include "Loc_ai\types\LAi_actor.c"
//Хозяин дома
#include "Loc_ai\types\LAi_owner.c"
//Носильщик
#include "Loc_ai\types\LAi_carrier.c"
//Жители ГПК, не внутрянка
#include "Loc_ai\types\LAi_LSCout.c"
//Сидящий не земле
#include "Loc_ai\types\LAi_groundSit.c"
// Оглушенные
#include "Loc_ai\types\LAi_stunned.c"
//Обезьяна-монстр
#include "Loc_ai\types\LAi_monkey.c"

//------------------------------------------------------------------------------------------
//Player
//------------------------------------------------------------------------------------------

//Установить персонажу тип игрока
void LAi_SetPlayerType(aref chr)
{
	//Тип персонажа по умолчанию
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	//Шаблон специальный
	LAi_type_player_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
}

//------------------------------------------------------------------------------------------
//Officer
//------------------------------------------------------------------------------------------

//Установить персонажу тип офицера
void LAi_SetOfficerType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_officer_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
}

//Установить персонажу тип офицера
void LAi_SetFollowerType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_follower_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
}

//------------------------------------------------------------------------------------------
//Citizen
//------------------------------------------------------------------------------------------

//Установить персонажу тип горожанина
void LAi_SetCitizenType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_citizen_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип горожанина, без перемещения в группу
void LAi_SetCitizenTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_citizen_Init(chr);
}

//------------------------------------------------------------------------------------------
//Guardian
//------------------------------------------------------------------------------------------

//Установить персонажу тип стражника
void LAi_SetGuardianType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_guardian_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_GUARDS);
}

//Установить персонажу тип стражника, без перемещения в группу
void LAi_SetGuardianTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_guardian_Init(chr);
}

//------------------------------------------------------------------------------------------
//Patrol
//------------------------------------------------------------------------------------------

//Установить персонажу тип патруля
void LAi_SetPatrolType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_SetPatrolTypeNoGroup(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_GUARDS);
}

//Установить персонажу тип патруля, без перемещения в группу
void LAi_SetPatrolTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_patrol_Init(chr);
}

//------------------------------------------------------------------------------------------
//Warrior
//------------------------------------------------------------------------------------------

// LEO: ЗВЕРИ И ОРУЖИЕ. Без НАДЕТОГО клинка движок вообще не пускает в боевой режим (SetFightMode выходит
// на isBladeSet), а флаг «бой без оружия» во всём моде включает только тип обезьяны - поэтому звериному
// спутнику нужен фиктивный клинок с невидимой моделью, иначе он стоит столбом. У краба-питомца "unarmed"
// в инвентаре лежал, но экипирован не был - отсюда и «не атакует вообще».
// Сюда же просится нормальный звериный тип ИИ - см. TODO у обезьяны ниже.
void LAi_EnsurePetAnimalWeapon(aref chr)
{
	if (!CheckAttribute(chr, "id")) return;
	if (chr.id != "pet_crab") return;
	if (GetCharacterEquipByGroup(chr, BLADE_ITEM_TYPE) != "unarmed")
	{
		if (!CheckCharacterItem(chr, "unarmed")) GiveItem2Character(chr, "unarmed");
		EquipCharacterByItem(chr, "unarmed");
	}
	// ПЕРЕХОДНОЕ: monkrab (урон 15-60) питомцу не по чину, в релиз не уходил - чистим сейвы, где он
	// успел попасть крабу в инвентарь. Через пару версий строку можно убрать.
	if (CheckCharacterItem(chr, "monkrab")) TakeItemFromCharacter(chr, "monkrab");
}

//Установить персонажу тип война
void LAi_SetWarriorType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_warrior_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_GUARDS);

	if (!CheckAttribute(chr, "model")) {Log_TestInfo("LAi_SetWarriorType: нет model у " + chr.id); return;}
	if (chr.model == "monkey")//TO DO - переделать это нормально
	{
		LAi_SetMonkeyTypeNoGroup(chr);
	}
}

//Установить персонажу тип война, без перемещения в группу
void LAi_SetWarriorTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_warrior_Init(chr);

	if (!CheckAttribute(chr, "model")) {Log_TestInfo("LAi_SetWarriorTypeNoGroup: нет model у " + chr.id); return;}
	if (chr.model == "monkey")
	{
		LAi_SetMonkeyTypeNoGroup(chr);
	}
}

//------------------------------------------------------------------------------------------
//Merchant
//------------------------------------------------------------------------------------------

//Установить персонажу тип торговца
void LAi_SetMerchantType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_merchant_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип торговца, без перемещения в группу
void LAi_SetMerchantTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_merchant_Init(chr);
}

//------------------------------------------------------------------------------------------
//Stay
//------------------------------------------------------------------------------------------

//Установить персонажу тип стоячего
void LAi_SetStayType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_stay_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип стоячего, без перемещения в группу
void LAi_SetStayTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_stay_Init(chr);
}

//------------------------------------------------------------------------------------------
//Sit
//------------------------------------------------------------------------------------------

//Установить персонажу тип сидячего
void LAi_SetSitType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_sit_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип сидячего, без перемещения в группу
void LAi_SetSitTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_sit_Init(chr);
}


//------------------------------------------------------------------------------------------
//Barmen
//------------------------------------------------------------------------------------------

//Установить персонажу тип бармена (only man)
void LAi_SetBarmanType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_barman_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип официантки, без перемещения в группу (only man)
void LAi_SetBarmanTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_barman_Init(chr);
}

//------------------------------------------------------------------------------------------
//Waitress
//------------------------------------------------------------------------------------------

//Установить персонажу тип официантки (only woman)
void LAi_SetWaitressType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_waitress_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип официантки, без перемещения в группу (only woman)
void LAi_SetWaitressTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_waitress_Init(chr);
}


//------------------------------------------------------------------------------------------
//Poor
//------------------------------------------------------------------------------------------

//Установить персонажу тип бедника
void LAi_SetPoorType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_poor_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип бедника, без перемещения в группу
void LAi_SetPoorTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_poor_Init(chr);
}

//------------------------------------------------------------------------------------------
//Huber
//------------------------------------------------------------------------------------------

//Установить персонажу тип губернатора
void LAi_SetHuberType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_huber_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_HUBER);
}

//Установить персонажу тип губернатора, без перемещения в группу
void LAi_SetHuberTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_huber_Init(chr);
}

//------------------------------------------------------------------------------------------
//HuberStay
//------------------------------------------------------------------------------------------

//Установить персонажу тип губернатора, ходячего по комнате
void LAi_SetHuberStayType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_huberstay_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_HUBER);
}

//Установить персонажу тип губернатора, ходячего по комнате, без перемещения в группу
void LAi_SetHuberStayTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_huberstay_Init(chr);
}

//------------------------------------------------------------------------------------------
//Priest
//------------------------------------------------------------------------------------------

//Установить персонажу тип священика
void LAi_SetPriestType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_priest_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип священика, без перемещения в группу
void LAi_SetPriestTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_priest_Init(chr);
}


//------------------------------------------------------------------------------------------
//Actor
//------------------------------------------------------------------------------------------

//Установить персонажу тип актёра
void LAi_SetActorType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_actor_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_ACTOR);
}

//Установить персонажу тип актёра, без перемещения в группу
void LAi_SetActorTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_actor_Init(chr);
}

//------------------------------------------------------------------------------------------
//Owner
//------------------------------------------------------------------------------------------

//Установить персонажу тип хозяина дома/комнаты
void LAi_SetOwnerType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_owner_Init(chr);
	LAi_group_MoveCharacter(chr, "");
}

//Установить персонажу тип хозятна комнаты без перемещения в группу
void LAi_SetOwnerTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_owner_Init(chr);
}

//------------------------------------------------------------------------------------------
//Carrier
//------------------------------------------------------------------------------------------

//Установить персонажу тип хозяина дома/комнаты
void LAi_SetCarrierType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_carrier_Init(chr);
	LAi_group_MoveCharacter(chr, "");
}

//Установить персонажу тип хозятна комнаты без перемещения в группу
void LAi_SetCarrierTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_carrier_Init(chr);
}

//------------------------------------------------------------------------------------------
//LSCout
//------------------------------------------------------------------------------------------

//Установить персонажу тип гражданина ГПК
void LAi_SetLSCoutType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAI_type_LSCout_Init(chr);
	LAi_group_MoveCharacter(chr, "");
}

//Установить персонажу тип гражданина ГПК без перемещения в группу
void LAi_SetLSCoutTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAI_type_LSCout_Init(chr);
}

//------------------------------------------------------------------------------------------
//GroundSit
//------------------------------------------------------------------------------------------

//Установить персонажу тип сидящего на земле
void LAi_SetGroundSitType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAI_type_GroundSit_Init(chr);
	LAi_group_MoveCharacter(chr, "");
}

//Установить персонажу тип сидящего на земле без перемещения в группу
void LAi_SetGroundSitTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAI_type_GroundSit_Init(chr);
}


//------------------------------------------------------------------------------------------
// Stunned
//------------------------------------------------------------------------------------------

//Установить персонажу тип оглушенного
void LAi_SetStunnedType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_Stunned_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
}

//Установить персонажу тип оглушенного, без перемещения в группу
void LAi_SetStunnedTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_Stunned_Init(chr);
}

//------------------------------------------------------------------------------------------
//Monkey
//------------------------------------------------------------------------------------------

//Установить персонажу тип обезьяны-монстра
void LAi_SetMonkeyType(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_monkey_Init(chr);
	LAi_group_MoveCharacter(chr, LAI_GROUP_MONSTERS);
}

//Установить персонажу тип обезьяны-монстра, без перемещения в группу
void LAi_SetMonkeyTypeNoGroup(aref chr)
{
	chr.chr_ai.type = LAI_DEFAULT_TYPE;
	LAi_type_monkey_Init(chr);
}
