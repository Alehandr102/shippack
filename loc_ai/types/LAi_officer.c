/*
Тип: офицер

	Используемые шаблоны:
		stay
		dialog
*/



#define LAI_TYPE_OFFICER		"officer"


//Инициализация
void LAi_type_officer_Init(aref chr)
{
	//Новый тип
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = LAI_TYPE_OFFICER;
	chr.chr_ai.type.bottle = rand(10)+2;
	chr.chr_ai.type.checkTarget = 0;
	if(CheckAttribute(chr, "chr_ai.tmpl"))
	{
		if (chr.chr_ai.tmpl != LAI_TMPL_STAY)//команда ждать!
		{
	    	if (!LAi_IsFightMode(GetMainCharacter()))
	        {
	    	    LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
	    	}
	    	else
	    	{
	            LAi_tmpl_fight_SetWaitState(chr);
	    	}
		}
	}
	else
	{
     	LAi_tmpl_SetFollow(chr, pchar, -1.0);
	}
	if (CheckAttribute(chr, "NpcMusket") && !CheckAttribute(chr, "CanTakeMushket") && !CheckAttribute(chr, "isMusketer.weapon"))
	{
		LAi_NPC_MushketerEquip(chr);
	}
	else
	{
		LAi_SetDefaultStayAnimation(chr);
	}
	LAi_EnsurePetAnimalWeapon(chr); // LEO: зверю нужен надетый клинок, иначе движок не пустит его в бой
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

void LAi_type_follower_Init(aref chr)
{
	//Новый тип
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = "follower";
	LAi_tmpl_SetFollow(chr, pchar, -1.0);
	//Установим анимацию персонажу
	LAi_SetDefaultStayAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAi_type_officer_CharacterUpdate(aref chr, float dltTime)
{
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return;
	//При отравлении детравимся
	float fCheck = CheckAttribute(chr, "chr_ai.hp_bottle.0");//слот 0 юзается последним
	if (CheckAttribute(chr, "chr_ai.poison")) {
		if (GetCharacterItem(chr, "potion3") > 0)
		{
			DoCharacterUsedItem(chr, "potion3");
		}
		else if (GetCharacterItem(chr, "potion4") > 0)
		{
			DoCharacterUsedItem(chr, "potion4");
		}
		else if (GetCharacterItem(chr, "potionrum") > 0)
		{
			DoCharacterUsedItem(chr, "potionrum");
		}
	}
	// Gregg: еда вынесена в LAi_CombatTryEat (LAi_types.c); sItem нужен ниже для лечения
	string sItem = "";
	LAi_CombatTryEat(chr);
	//Дистанция до главного персонажа
	float dist = 0.0;
	float dhlt;
	if(!GetCharacterDistByChr3D(chr, pchar, &dist)) dist = -1.0;
	if(fCheck <= 0) {
		// LEO: порог проверяем ДО поиска зелья - целому офицеру перебирать список предметов незачем
		dhlt = stf(chr.chr_ai.hp_max) - stf(chr.chr_ai.hp);
		if (dhlt > 20.0) {
			sItem = FindHealthForCharacter(chr);
			if (sItem != "") {
				DoCharacterUsedItem(chr, sItem);//чек уже есть
			}
		}
	}
	// Gregg (этап D): командный стейт наземной тактики. Ручные приказы держатся, авто-таргетинг/авто-follow не перебивают.
	if (CheckAttribute(chr, "chr_ai.LandTacCmdTarget"))
	{
		int ltCmdT = sti(chr.chr_ai.LandTacCmdTarget);
		if (ltCmdT >= 0)
		{
			if (!LAi_IsDead(&Characters[ltCmdT]))
			{
				if (LAi_tmpl_fight_GetTarget(chr) != ltCmdT) LAi_tmpl_SetFight(chr, &Characters[ltCmdT]);
				return; // держим НАЗНАЧЕННУЮ цель, авто-ИИ не вмешивается
			}
		}
		DeleteAttribute(chr, "chr_ai.LandTacCmdTarget"); // цель убита/невалидна
		chr.chr_ai.LandTacHold = "1";                    // держим позицию, к ГГ не бежим
		DeleteAttribute(chr, "chr_ai.tmpl");             // сбросить FIGHT/FOLLOW
		chr.chr_ai.tmpl = "stay";                        // метка STAY напрямую
		SetCharacterTask_Stay(chr);                      // встать на месте (тут goto нет — можно останавливать)
	}
	if (CheckAttribute(chr, "chr_ai.LandTacHold")) return; // удержание позиции (приказ goto), без вмешательства ИИ
	if (chr.chr_ai.tmpl == LAI_TMPL_STAY) return; // приказ ему стоять. Врага не ищем 18.06.05
	if (chr.chr_ai.tmpl == LAI_TMPL_WALK) return; // приказ вольно.

	if (CheckAttribute(loadedLocation, "noFight")) return;

	// LEO: грейс боевой готовности тикает вниз (сбрасывается в FindTarget при наличии врага).
	// Пока >0, оружие не прячем, даже если цель на миг пропала (восприятие/оптимальная цель мерцает) - убирает дёрганье достать/спрятать.
	if (CheckAttribute(chr, "chr_ai.type.engageHold"))
	{
		float fHold = stf(chr.chr_ai.type.engageHold) - dltTime;
		if (fHold < 0.0) fHold = 0.0;
		chr.chr_ai.type.engageHold = fHold;
	}

	// LEO: пока рядом с САМИМ офицером (а не с ГГ) есть враг в его радиусе - держим боевую готовность.
	// Раньше готовность зависела от дистанции ГГ->враг (CheckDists), отсюда мерцание, когда безоружный ГГ отходит ("чем дальше, тем чаще").
	int trgNear = LAi_group_GetTarget(chr);
	if (trgNear >= 0)
	{
		if (LAi_IsSetBale(&Characters[trgNear]))
		{
			float dEnemy = -1.0;
			if (!GetCharacterDistByChr3D(chr, &Characters[trgNear], &dEnemy)) dEnemy = -1.0;
			float fRng = 15.0;
			if (CheckAttribute(pchar, "OfficerAttRange")) fRng = stf(pchar.OfficerAttRange) * 1.5;
			if (dEnemy >= 0.0)
			{
				if (dEnemy < fRng) chr.chr_ai.type.engageHold = 4.0;
			}
		}
	}

	//Дистанция до главного персонажа
	if (chr.chr_ai.tmpl != LAI_TMPL_FIGHT)
	{
		if(!GetCharacterDistByChr3D(chr, GetMainCharacter(), &dist)) dist = -1.0;
		// LEO: к ГГ НЕ уводим, пока есть свежая угроза (грейс>0) - иначе вдали от ГГ офицер мерцает достать/спрятать.
		// Вложенные if (а не A&&B||C) - Storm-парсер не любит сложные булевы выражения.
		float fEngFar = 0.0;
		if (CheckAttribute(chr, "chr_ai.type.engageHold")) fEngFar = stf(chr.chr_ai.type.engageHold);
		if (fEngFar <= 0.0)
		{
			if (dist < 0.0 || dist > 30)
			{
				if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0)
				{
					LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
	    			return;
				}
			}
		}
	}
	LAi_type_officer_FindTarget(chr);   // fix
	//Текущее действие
	if(chr.chr_ai.tmpl == LAI_TMPL_FOLLOW)
	{
		if(dist < 30.0)
		{
			LAi_type_officer_FindTarget(chr);
		}
	}else{
		if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
		{
			if(LAi_tmpl_fight_LostTarget(chr))
			{
				LAi_type_officer_FindTarget(chr);
			}else{
				if(LAi_tmpl_fight_IsGo(chr))
				{
					if(dist > 12.0)
					{
						//Пора возвращаться
						// LEO: пока есть свежая угроза (грейс>0) - не уводим к ГГ, офицер добивает врага, а не мерцает (вложенные if - гоча Storm)
						float fEngGo = 0.0;
						if (CheckAttribute(chr, "chr_ai.type.engageHold")) fEngGo = stf(chr.chr_ai.type.engageHold);
						if (fEngGo <= 0.0)
						{
							if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0)
	                        {
							    LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
							}
						}
					}
				}
				else
				{
					//пробуем сменить цель, может уже есть кто ближе
					fCheck = stf(chr.chr_ai.type.checkTarget) - dltTime;
					chr.chr_ai.type.checkTarget = fCheck;
					if (CheckAttribute(LAi_grp_relations,"distance"))
					{
						if (stf(LAi_grp_relations.distance) > 2.0 && fCheck < 0) //цель далеко, попробуем сменить на ближайшую
						{
							LAi_type_officer_FindTarget(chr);
						}
					}
				}
			}
		}
		else
		{
		    LAi_type_officer_FindTarget(chr);
		}
	}
}

void LAi_type_follower_CharacterUpdate(aref chr, float dltTime)
{
}

//Загрузка персонажа в локацию
bool LAi_type_officer_CharacterLogin(aref chr)
{
	return true;
}

bool LAi_type_follower_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_officer_CharacterLogoff(aref chr)
{
	return true;
}

bool LAi_type_follower_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_officer_TemplateComplite(aref chr, string tmpl)
{
	if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW)
    {
        if (chr.chr_ai.tmpl != LAI_TMPL_STAY && SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0) // fix
        {
            // LEO: по завершении шаблона (убил цель) НЕ прячем оружие, пока рядом ещё есть враг (грейс) - убирает одиночный блик достать/спрятать на килле
            float fEngTC = 0.0;
            if (CheckAttribute(chr, "chr_ai.type.engageHold")) fEngTC = stf(chr.chr_ai.type.engageHold);
            if (fEngTC <= 0.0)
            {
                LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
            }
        }
    }
}

void LAi_type_follower_TemplateComplite(aref chr, string tmpl)
{
	if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW) LAi_tmpl_SetFollow(chr, pchar, -1.0);
}

//Сообщить о желании завести диалог
void LAi_type_officer_NeedDialog(aref chr, aref by)
{
}

void LAi_type_follower_NeedDialog(aref chr, aref by)
{
}


//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_officer_CanDialog(aref chr, aref by)
{
	if(LAi_IsBoardingProcess()) return false;
	//Если уже говорим, то откажем
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return false;
	if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT) return false;
	//Согласимся на диалог
	return true;
}

bool LAi_type_follower_CanDialog(aref chr, aref by)
{
	if(LAi_IsBoardingProcess()) return false;
	//Если уже говорим, то откажем
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return false;
	if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT) return false;
	//Согласимся на диалог
	return true;
}

//Начать диалог
void LAi_type_officer_StartDialog(aref chr, aref by)
{
	LAi_StdStartDialog(chr, by);
}

void LAi_type_follower_StartDialog(aref chr, aref by)
{
	LAi_StdStartDialog(chr, by);
}

//Закончить диалог
void LAi_type_officer_EndDialog(aref chr, aref by)
{
	LAi_CharacterRestoreAy(chr);
	if (chr.chr_ai.tmpl != LAI_TMPL_STAY)//команда ждать!
	   LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
}

void LAi_type_follower_EndDialog(aref chr, aref by)
{
	LAi_CharacterRestoreAy(chr);
	LAi_tmpl_SetFollow(chr, pchar, -1.0);
}


//Персонаж выстрелил
void LAi_type_officer_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{

}


//Персонаж атакован
void LAi_type_officer_Attacked(aref chr, aref by)
{
	//если наносящий удар уже таргет, нефиг крутить код и переназначать цель
	if (LAi_tmpl_fight_GetTarget(chr) == sti(by.index)) return;
	//Своих пропускаем
	if(!LAi_group_IsEnemy(chr, by)) return;
	// boal: цель меняем всегда, без проверок на текущий шаблон - иначе боец топчется на месте
	//Определяем дистанцию до нового противника
	float dist = -1.0;
	if(!GetCharacterDistByChr3D(chr, by, &dist)) return;
	if(dist < 0.0) return;
	if(dist > LAI_TYPE_AGGRO_DIST) return;
	//Натравливаем
	if(!LAi_tmpl_SetFight(chr, by))
	{
		LAi_type_officer_FindTarget(chr);
	}
}


void LAi_type_officer_FindTarget(aref chr)
{
	//Проверим наличие врагов
	int trg = LAi_group_GetTarget(chr);
	if(trg >= 0 && LAi_IsSetBale(&Characters[trg]))
	{
		if(LAi_type_officer_CheckDists(chr, &Characters[trg]))
		{
			if(LAi_tmpl_SetFight(chr, &Characters[trg]))
			{
				chr.chr_ai.type.checkTarget = rand(3) + 2; //таймер на проверялку расстояния до таргета
				chr.chr_ai.type.engageHold = 4.0; // LEO: видим врага - держим боевую готовность 4с (анти-дёрганье оружия)
				return;
			}
		}
	}
	trg = LAi_group_GetTarget(pchar);
	if(trg >= 0  && LAi_IsSetBale(&Characters[trg]))
	{
		if(LAi_type_officer_CheckDists(chr, &Characters[trg]))
		{
			chr.chr_ai.type.checkTarget = rand(3) + 2; //таймер на проверялку расстояния до таргета
			chr.chr_ai.type.engageHold = 4.0; // LEO: видим врага - держим боевую готовность 4с (анти-дёрганье оружия)
			if(!LAi_tmpl_SetFight(chr, &Characters[trg]))
			{
				//Несмогли инициировать шаблон
				LAi_tmpl_fight_SetWaitState(chr);
			}
		}
	}else{
		if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW)
		{
            if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0) // fix
            {
                // LEO: НЕ прячем оружие сразу при кратком исчезновении цели - только когда грейс истёк (враг реально пропал на ~4с).
                // Это убирает мерцание достать/спрятать, когда оптимальная цель/восприятие мигает между тиками.
                float fHold = 0.0;
                if (CheckAttribute(chr, "chr_ai.type.engageHold")) fHold = stf(chr.chr_ai.type.engageHold);
                if (fHold <= 0.0)
                {
                    LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
                }
            }
		}
	}
}

bool LAi_type_officer_CheckDists(aref chr, aref trg)
{
	float dist = -1.0;
	float searchDist = 10;
	if(CheckAttribute(pchar, "OfficerAttRange")) {
		searchDist = stf(pchar.OfficerAttRange);
	}
	if(!GetCharacterDistByChr3D(pchar, trg, &dist)) dist = -1.0;
	if(dist >= 0.0 && dist < searchDist)
	{
		if(!GetCharacterDistByChr3D(chr, trg, &dist)) dist = -1.0;
		if(dist >= 0.0 && dist < searchDist)
		{
			return true;
		}
	}
	return false;
}

