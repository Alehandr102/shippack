object	Sail, Rope, Flag, Vant, VantL, VantZ, Pennant, sFlag;
object PirateFlag, PirateCFlag, CommanderFlag, FortFlag, MerchantFlag, PersonalFlag, QuestFlag;

#define MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER				1000.0
#define MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER_FORT		1400.0
#define MIN_ENEMY_DISTANCE_TO_DISABLE_ENTER_2_LOCATION		1150.0
//#20190612-02
#define RA_CANNON_RATIO 0.55
#define RA_AMMO_DET 3.0
#define RA_POWDER_DET 3.0
#define SHIP_DEFAULT_FIRE_TIME 16

// LEO: шторм - урон корпусу/парусам только против ветра (по/поперёк - нет); STORM_DMG_SCALE - общий множитель, опыт лишь при реальном уроне.
#define STORM_DMG_SCALE 0.5

bool	bAbordageShipCanBe = false;
bool 	LetCharge = false;
int		iAbordageShipEnemyCharacter;

bool	bCanEnterToLand = false;
aref	arIslandReload;
string	sIslandLocator;
string	sIslandID;
string	sSeaSoundPostfix;
int		iShipPriorityExecute = 2;
int		iShipPriorityRealize = 31;
bool	bUpdatePosMode = false;
int		iVicSoundTime = 0;
int		iSeaTimeAccumMs = 0;	// аккумулятор кадровых мс для морского времени (GetSeaTime, боевая мораль)
string	sSeaStartMusicName;
bool	isEnemyAround = false;

int		iNumShips = 0;
int		Ships[MAX_SHIPS_ON_SEA];

int 	ShipModelrList[MAX_SHIPS_ON_SEA];
int 	ShipOnLoadModelrList;
#define SHIP_DETONATE_SMALL		"ShipDetonateSmall"

#event_handler("Ship_StartLoad", "Ship_StartLoad");
#event_handler("Ship_EndLoad", "Ship_EndLoad");

// ============================================================================
//   ОКРУЖЕНИЕ, ТАКЕЛАЖ, СЦЕНА, ФЕЙК-ОГОНЬ ФОРТА, ТОЧНОСТЬ ЗАЛПА,
//   ГРУППЫ И НАЦИОНАЛЬНАЯ АГРЕССИЯ (реакция на обстрел)
// ============================================================================
void CreateRiggingEnvironment()
{
	CreateEntity(&Sail, "Sail");
	LayerAddObject(sCurrentSeaExecute, &Sail, iShipPriorityExecute + 1);
	LayerAddObject(sCurrentSeaRealize, &Sail, iShipPriorityRealize + 1);
	LayerAddObject(SAILS_TRACE, &Sail, 10);
	LayerAddObject(SEA_REFLECTION2, &Sail, 3);
	Sail.MinSpeed = 0.05;

	CreateEntity(&Rope, "Rope");
	LayerAddObject(sCurrentSeaExecute, &Rope, iShipPriorityExecute + 2);
	LayerAddObject(sCurrentSeaRealize, &Rope, iShipPriorityRealize + 2);

	Flag.ratioLimit = 1;
	Flag.sizeRatio = 3.5;
	Flag.isDefault = 1;
	Flag.lblGroup = "flag"; // Gregg: B — Flag сканит ТОЛЬКО flag-группы; sflag отдаём sFlag (вымпел) → без задвоения
	CreateEntity(&Flag, "Flag");
	LayerAddObject(sCurrentSeaExecute, &Flag, iShipPriorityExecute + 3);
	LayerAddObject(sCurrentSeaRealize, &Flag, iShipPriorityRealize + 3);
	LayerAddObject(SEA_REFLECTION2, &Flag, 3);

	CreateEntity(&Vant, "Vant");
	LayerAddObject(sCurrentSeaExecute, &Vant, iShipPriorityExecute + 4);
	LayerAddObject(sCurrentSeaRealize, &Vant, iShipPriorityRealize + 4);

	CreateEntity(&VantL, "VantL");
	LayerAddObject(sCurrentSeaExecute, &VantL, iShipPriorityExecute + 4);
	LayerAddObject(sCurrentSeaRealize, &VantL, iShipPriorityRealize + 4);
	
	CreateEntity(&VantZ, "VantZ");
	LayerAddObject(sCurrentSeaExecute, &VantZ, iShipPriorityExecute + 4);
	LayerAddObject(sCurrentSeaRealize, &VantZ, iShipPriorityRealize + 4);

	iNumShips = 0;
	Pennant.ratioExceeds = 1;
	Pennant.sizeRatio = 3.5;
	Pennant.iniSection = "PENNANT";
	Pennant.lblGroup = "penn"; // Gregg: B — сканить группы локаторов penn (вымпелы) в моделях
	CreateEntity(&Pennant, "Flag");
	LayerAddObject(sCurrentSeaExecute, &Pennant, iShipPriorityExecute + 5);
	LayerAddObject(sCurrentSeaRealize, &Pennant, iShipPriorityRealize + 5);
	LayerAddObject(SEA_REFLECTION2, &Pennant, 3);

	// Gregg: B — sflag ПО РОЛИ: 5 сущностей (по атласу), все сканят sflag; procGetRiggingData по char
	//   показывает подходящую роль, прочим возвращает -1. Военные=shpflg, торг=merflg, ГГ+эск=perflg, пираты=pirflg(rand), квест=qstflg.
	CommanderFlag.lblGroup = "sflag";
	CommanderFlag.iniSection = "SFLAG_MIL"; // военные (адмиральский shpflg)
	CreateEntity(&CommanderFlag, "Flag");
	LayerAddObject(sCurrentSeaExecute, &CommanderFlag, iShipPriorityExecute + 6);
	LayerAddObject(sCurrentSeaRealize, &CommanderFlag, iShipPriorityRealize + 6);
	LayerAddObject(SEA_REFLECTION2, &CommanderFlag, 3);

	MerchantFlag.lblGroup = "sflag";
	MerchantFlag.iniSection = "SFLAG_MER";  // торговцы
	CreateEntity(&MerchantFlag, "Flag");
	LayerAddObject(sCurrentSeaExecute, &MerchantFlag, iShipPriorityExecute + 6);
	LayerAddObject(sCurrentSeaRealize, &MerchantFlag, iShipPriorityRealize + 6);
	LayerAddObject(SEA_REFLECTION2, &MerchantFlag, 3);

	PersonalFlag.lblGroup = "sflag";
	PersonalFlag.iniSection = "SFLAG_PER";  // ГГ + эскадра
	CreateEntity(&PersonalFlag, "Flag");
	LayerAddObject(sCurrentSeaExecute, &PersonalFlag, iShipPriorityExecute + 6);
	LayerAddObject(sCurrentSeaRealize, &PersonalFlag, iShipPriorityRealize + 6);
	LayerAddObject(SEA_REFLECTION2, &PersonalFlag, 3);

	PirateFlag.lblGroup = "sflag";
	PirateFlag.iniSection = "SFLAG_PIR";    // пираты (случайный вариант)
	CreateEntity(&PirateFlag, "Flag");
	LayerAddObject(sCurrentSeaExecute, &PirateFlag, iShipPriorityExecute + 6);
	LayerAddObject(sCurrentSeaRealize, &PirateFlag, iShipPriorityRealize + 6);
	LayerAddObject(SEA_REFLECTION2, &PirateFlag, 3);

	QuestFlag.lblGroup = "sflag";
	QuestFlag.iniSection = "SFLAG_QST";     // квестовые
	CreateEntity(&QuestFlag, "Flag");
	LayerAddObject(sCurrentSeaExecute, &QuestFlag, iShipPriorityExecute + 6);
	LayerAddObject(sCurrentSeaRealize, &QuestFlag, iShipPriorityRealize + 6);
	LayerAddObject(SEA_REFLECTION2, &QuestFlag, 3);

	// Gregg: B — форты: атлас frtflg ([FRTFLG]); сканит СУЩЕСТВУЮЩИЕ flag-группы фортов (flag/flag1, без правки моделей).
	//   FortFlag цепляет flag и на кораблях (ship.cpp инитит все flag-сущности), но в procGetRiggingData
	//   гасится по char (корабль: char валиден → -1; форт: char null → frtflg по нации).
	FortFlag.lblGroup = "flag";
	FortFlag.iniSection = "FRTFLG";
	CreateEntity(&FortFlag, "Flag");
	LayerAddObject(sCurrentSeaExecute, &FortFlag, iShipPriorityExecute + 6);
	LayerAddObject(sCurrentSeaRealize, &FortFlag, iShipPriorityRealize + 6);
	LayerAddObject(SEA_REFLECTION2, &FortFlag, 3);
}

void DeleteRiggingEnvironment()
{
	DeleteClass(&Sail);
	DeleteClass(&Rope);
	DeleteClass(&Flag);
	DeleteClass(&sFlag);
	DeleteClass(&Pennant);
	DeleteClass(&PirateFlag);
	DeleteClass(&CommanderFlag);
	DeleteClass(&FortFlag);
	DeleteClass(&PirateCFlag);
	DeleteClass(&MerchantFlag);
	DeleteClass(&PersonalFlag);
	DeleteClass(&QuestFlag);
	DeleteClass(&Vant);
	DeleteClass(&VantL);
	DeleteClass(&VantZ);
}

void DeleteShipEnvironment()
{
	DeleteRiggingEnvironment();

	DelEventHandler(SHIP_DROP_GOOD, "DropGoodsToSea");
	DelEventHandler(SHIP_HULL_HIT, "Ship_HullHitEvent");
	DelEventHandler(SHIP_CHECK_RELOAD_ENABLE, "Ship_CheckMainCharacter");
	DelEventHandler(SHIP_DETONATE_SMALL, "Ship_DetonateSmall");
	DelEventHandler(SHIP_BRANDER_DETONATE, "Ship_BranderDetonate");
	DelEventHandler(SHIP_FIRE_DAMAGE, "Ship_FireDamage");
	DelEventHandler(SHIP_TO_STRAND_NEXT, "Ship_OnStrandNext");
	DelEventHandler(SHIP_BORT_RELOADED, "Ship_OnBortReloaded");
	DelEventHandler(SHIP_ACTIVATE_FIRE_PLACE, "Ship_ActivateFirePlace");
	DelEventHandler(SHIP_UPDATE_PARAMETERS, "Ship_UpdateParameters");
	DelEventHandler(SHIP_CHECK_SITUATION, "Ship_CheckSituation");
	DelEventHandler(SHIP_BOARDING_TICK, "Ship_BoardingTick");
	DelEventHandler(WHR_LIGHTNING, "Ship_Lightning");
	DelEventHandler("eSwitchPerks", "Ship_UpdatePerks");
	DelEventHandler("TornadoDamage", "Ship_TornadoDamage");
	DelEventHandler(SHIP_GET_RUNAWAY_POINT, "Ship_GetRunAwayPoint");
	DelEventHandler("frame", "Ship_VicSoundTime");
	DelEventHandler("Ship_SailsMoveSound", "Ship_SailsMoveSound");
	DelEventHandler("Ship_BortReloadEvent", "Ship_BortReloadEvent");
	DelEventHandler(SHIP_FAKE_FIRE, "Ship_EventFakeFire");

	int i = 0;
	for(i = 0; i < GetArraySize(&LoadedChars); i++) LoadedChars[i] = 0; // clear array of loaded chars on sea
	for(i = 0; i<iNumShips; i++) SendMessage(&Characters[Ships[i]], "l", MSG_SHIP_SAFE_DELETE); // delete particles from ship/etc

	// сканим персонажей для удаления sndID
	DeleteEntitiesByType("ship");
}

void ClearAllFire() // boal new
{
    for (int i=0; i<iNumShips; i++)
	{
		// удаляем партиклы с корабля и т.п.
		SendMessage(&Characters[Ships[i]], "l", MSG_SHIP_SAFE_DELETE);
	}
}
// boal new сбросить всем в море проверку смотрения на флаг.
void Sea_ClearCheckFlag()
{
    if (bSeaActive)
    {
		// LEO-ФИКС: было — цикл фортов ВЛОЖЕН в цикл кораблей и переиспользовал i внешнего → внешний рвался после Ships[0],
		//   CheckFlagYet сбрасывался только у ПЕРВОГО корабля → при смене флага остальные корабли/эскадры не перепроверяли
		//   флаг (Ship_CS_CheckFalseFlag ранний return по CheckFlagYet) и оставались враждебными. Разнёс на 2 независимых цикла.
		for (int i=0; i<iNumShips; i++) // корабли
		{
			if(sti(Ships[i]) >= 0 && sti(Ships[i]) < TOTAL_CHARACTERS)
			{
				if (!CheckAttribute(&Characters[Ships[i]], "DontCheckFlag"))
				{
					DeleteAttribute(&Characters[Ships[i]], "CheckFlagYet"); // флаг распознования врага
					DeleteAttribute(&Characters[Ships[i]], "CheckFlagDate");
				}
			}
		}
		for (int j=0; j<MAX_COLONIES; j++) // форты
		{
			if (CheckAttribute(&colonies[j], "commanderIdx"))
			{
				if (!CheckAttribute(&Characters[sti(colonies[j].commanderIdx)], "DontCheckFlag"))
				{
					DeleteAttribute(&Characters[sti(colonies[j].commanderIdx)], "CheckFlagYet"); // флаг распознования врага
					DeleteAttribute(&Characters[sti(colonies[j].commanderIdx)], "CheckFlagDate");
				}
			}
		}
	}
}

// LEO (баг 2): при игроцкой смене флага в море пере-задать пер-чар отношения кораблей к ГГ ПО НАЦИИ.
//   Причина: боевой мозг Ship_CS_TacticalDecision (:2210) форсит жёсткий SetCharacterRelationBoth(корабль,ГГ,ENEMY) при
//   вступлении в бой; смена флага правит только НАЦ-таблицу + UpdateRelations -> чисто нац-производные (трейдеры) обновляются,
//   а этот жёсткий оверрайд переживает -> корабли оставались красными после смены флага на их нацию.
//   ⚠️ ТОЧЕЧНО, чтобы НЕ ломать намеренно-дружественные корабли (ПГГ-конвои, квест-союзники) флипом друг->враг:
//     - reconcile ТОЛЬКО (застрявший ENEMY в движке) -> обман флагом работает для любых врагов (враг->друг по нации);
//     - ПЛЮС осадников (SiegeAssault) в ОБЕ стороны: у них боевой мозг НЕ перезапускается (Ship_CS_SiegeAttack возвращает
//       true до гейта врага) -> друг->враг сам не переставит -> нужно тут (иначе после дружбы смена флага на враждебную
//       нацию не делала бы осадников красными).
//   Пропускаем: сам ГГ, Always-метки, перемирие (SpeakTruce), призы (PrizeNoFlee), ПГГ (PGGAi - охотники/конвои ловят флаг
//     сами по своей логике), компаньонов ГГ (дружба не по нации). Дружественных не-осадников не трогаем вообще.
//   ⚠️ ВЫЗЫВАТЬ ТОЛЬКО из игроцкого пути (NationRelation.c), НЕ из Flag_Change (его дёргают квесты).
void Sea_ReconcileRelationsToFlag()
{
	if (!bSeaActive) return;
	int iMain = GetMainCharacterIndex();
	int k, idx, nr, c, cn;
	bool bComp;
	for (k = 0; k < iNumShips; k++)
	{
		idx = sti(Ships[k]);
		if (idx < 0 || idx >= TOTAL_CHARACTERS) continue;
		if (idx == iMain) continue;
		if (CheckAttribute(&Characters[idx], "AlwaysEnemy") || CheckAttribute(&Characters[idx], "AlwaysFriend")) continue;
		if (CheckAttribute(&Characters[idx], "SpeakTruce")) continue;   // перемирие переговоров - не сбрасывать
		if (CheckAttribute(&Characters[idx], "PrizeNoFlee")) continue;  // боевой приз (нацио-захват) - спец-отношение
		if (CheckAttribute(&Characters[idx], "PGGAi")) continue;        // ПГГ-ЛИДЕР (охотники/конвои) - своя логика отношений к ГГ
		// РЕКОНСАЙЛ ТОЛЬКО застрявших врагов ИЛИ осадников. Дружественных не-осадников (ПГГ-эскорт без метки, квест-союзники) НЕ трогаем.
		if (GetRelation(idx, iMain) != RELATION_ENEMY && !CheckAttribute(&Characters[idx], "SiegeAssault")) continue;
		// компаньонов ГГ пропускаем: их дружественность НЕ по нации (иностранный офицер-компаньон стал бы врагом)
		bComp = false;
		for (c = 0; c < COMPANION_MAX; c++)
		{
			cn = GetCompanionIndex(pchar, c);
			if (cn == idx) { bComp = true; break; }
		}
		if (bComp) continue;
		nr = GetNationRelation2MainCharacter(sti(Characters[idx].nation));
		SetCharacterRelationBoth(idx, iMain, nr);
	}
}

void CreateShipEnvironment()
{
	isEnemyAround = false;
	bUpdatePosMode = false;
	iVicSoundTime = 0;

	bAbordageFortCanBe = false;
	bAbordageShipCanBe = false;
	bCanEnterToLand = false;
	sIslandLocator = "";
	sIslandID = "";

	for (int x = 0; x < MAX_SHIPS_ON_SEA; x++)
	{
		ShipModelrList[x] = -1;
		Ships[x] = -1;
	}
	ShipOnLoadModelrList = -1;
	aref arCurWeather = GetCurrentWeather();
	sSeaSoundPostfix = arCurWeather.Sounds.Sea.Postfix;

	CreateRiggingEnvironment();

	SetEventHandler(SEA_GET_LAYERS, "SeaAI_GetLayers", 0);

	SetEventHandler(SHIP_NOT_ENOUGH_BALLS, "Ship_NotEnoughBalls", 0);
	SetEventHandler(SHIP_GET_CURRENT_BALLS_NUM, "Ship_GetCurrentBallsNum", 0);
	SetEventHandler(SHIP_SHIP2ISLAND_COLLISION, "Ship_Ship2IslandCollision", 0);
	SetEventHandler(SHIP_SHIP2SHIP_COLLISION, "Ship_Ship2ShipCollision", 0);
	SetEventHandler(SHIP_SAIL_DAMAGE, "Ship_SailDamage", 0);
	SetEventHandler(SHIP_FIRE_DAMAGE, "Ship_FireDamage", 0);
	SetEventHandler(SHIP_CHECK_SITUATION, "Ship_CheckSituation", 0);
	SetEventHandler(SHIP_BOARDING_TICK, "Ship_BoardingTick", 0);
	SetEventHandler(SHIP_TO_STRAND, "Ship_OnStrand", 0);
	SetEventHandler(SHIP_TO_STRAND_NEXT, "Ship_OnStrandNext", 0);
	SetEventHandler(SHIP_DEAD, "Ship_OnDead", 0);
	SetEventHandler(SHIP_UPDATE_PARAMETERS, "Ship_UpdateParameters", 0);
	SetEventHandler(SHIP_DROP_GOOD, "DropGoodsToSea", 0);
	SetEventHandler(SHIP_CREATE, "Ship_OnCreate", 0);
	SetEventHandler(SHIP_BORT_RELOADED, "Ship_OnBortReloaded", 0);
	SetEventHandler(SHIP_HULL_HIT, "Ship_HullHitEvent", 0);
	SetEventHandler(SHIP_CHECK_RELOAD_ENABLE, "Ship_CheckMainCharacter", 0);
	SetEventHandler(SHIP_FIRE_ACTION, "Ship_FireAction", 0);
	SetEventHandler(SHIP_GET_BORT_FIRE_DELTA, "Ship_GetBortFireDelta", 0);
	SetEventHandler(SHIP_DETONATE_SMALL, "Ship_DetonateSmall", 0);
	SetEventHandler(SHIP_ACTIVATE_FIRE_PLACE, "Ship_ActivateFirePlace", 0);
	SetEventHandler(WHR_LIGHTNING, "Ship_Lightning", 0);
	SetEventHandler(SHIP_CREATELOADSHIP, "Ship_LoadShip", 0);
	SetEventHandler(SHIP_LOAD_SHIPACTIVATEFIREPLACE, "Ship_LoadShipActivateFirePlace", 0);
	SetEventHandler(SHIP_GET_RUNAWAY_POINT, "Ship_GetRunAwayPoint", 0);
	SetEventHandler("TornadoDamage", "Ship_TornadoDamage", 0);

	SetEventHandler(SHIP_BRANDER_DETONATE, "Ship_BranderDetonate", 0);

	SetEventHandler(SHIP_MAST_DAMAGE, "Ship_MastDamage", 0);

	SetEventHandler("eSwitchPerks", "Ship_UpdatePerks", 0);

	SetEventHandler("frame", "Ship_VicSoundTime", 0);

	SetEventHandler("Ship_SailsMoveSound", "Ship_SailsMoveSound", 0);
	SetEventHandler("Ship_BortReloadEvent", "Ship_BortReloadEvent", 0);
	SetEventHandler(SHIP_FAKE_FIRE, "Ship_EventFakeFire", 0);
}

void Ship_EventFakeFire()
{
    float fX = GetEventData();
    float fY = GetEventData();
    float fZ = GetEventData();
    float fAng = GetEventData();
	aref rShip = GetEventData();

    if (InterfaceStates.FireParticles) CreateParticleSystem("cancloud_fire_ext", fX, fY, fZ, 0.0, fAng, 0.0, 5);//тут задаётся сам прожеклайт и его направление
    else CreateParticleSystem("cancloud_fire", fX, fY, fZ, 0.0, fAng, 0.0, 5);//тут задаётся сам прожеклайт и его направление
    Play3DSound("cannon_fake_fire", fX, fY, fZ);//звук объёмный я так понимаю тут
	Event(SHIP_BORT_FIRE, "lis", 1, rShip, "cannonl");
	PostEvent("Event_FakeExplosion", 1000+rand(300))
}

void DoFakeFire()
{
	SendMessage(&locShips[sti(pchar.fakefireindex)],"ls",MSG_SHIP_DO_FAKE_FIRE,"cannonl");//собственно стрельба с левого борта
	PostEvent("Event_FakeFire", 3000+rand(3000));//цикловка стрельбы раз в 80 секунд
}

void InitFakeFire()
{
	SetEventHandler("Event_FakeFire", "DoFakeFire", 0);
	PostEvent("Event_FakeFire", 3000);//первый залп через 5 секунд после загрузки
	pchar.Ship.Pos.Mode = SHIP_FORT_ATTACKER;
}

void StopFakeFire()
{
	DelEventHandler("Event_FakeFire", "DoFakeFire");
	pchar.Ship.Pos.Mode = SHIP_SAIL;
}

#event_handler("Event_FakeExplosion", "FakeExplosion");
void FakeExplosion()
{
	int iRand50 = rand(50);
    if (sti(InterfaceStates.OtherParticles)) CreateLocationParticles("ShipExplode_ext", "heaters", "boom"+iRand50, 0, 0, 0, "fake_explosion");
    else CreateLocationParticles("ShipExplode", "heaters", "boom"+iRand50, 0, 0, 0, "fake_explosion");
    CreateLocationParticles("blast", "heaters", "boom"+iRand50, 0, 0, 0, "fake_explosion");
	int iLoadedLocation = FindLoadedLocation();
	if(iLoadedLocation >= 0 && IsEntity(&Locations[iLoadedLocation]))
	{
		SendMessage(&Locations[iLoadedLocation], "lff", MSG_LOCATION_SHAKE, 1.0, 1.0);
	}
}

//Ошибка канонира - смещение центра залпа от цели (м); 0.1 штрафа ≈ 1м в упор, 6м на километре (угловой разлёт ball_addBall).
//точность 100 - 0.5..3м; 50 - 1.5..13м; 10 - 2.5..16м.
float Ship_GetBortFireDeltaFromDistance(aref aCharacter, float fDistance, float fOnOffRandom)
{
	if (fOnOffRandom){
		fOnOffRandom = Random();
	}
	else{
		fOnOffRandom = 1.0;
	}
	float fAccuracy = stf(aCharacter.TmpSkill.Accuracy);
	float fCannons = stf(aCharacter.TmpSkill.Cannons);

	int nShipType = sti(aCharacter.ship.type);
	ref refBaseShip = GetRealShip(nShipType);
	string sFiredBort = "";
	if (CheckAttribute(aCharacter, "Ship.FiredBort")){sFiredBort = aCharacter.Ship.FiredBort;}
	int  iCannonsNum = GetBortIntactCannonsNum(aCharacter, sFiredBort, sti(refBaseShip.(sFiredBort)));
	int iCannonType = sti(aCharacter.Ship.Cannons.Type);

	float fRetValue = (1 - fAccuracy)/4;// 0...0.22
	fRetValue += Calc_CannonsNumPenalty(fAccuracy, iCannonsNum);//-0.1...0...0.1
	fRetValue += Calc_CannonsCaliberPenalty(fCannons, iCannonType);//-0.1...0...0.1
	fRetValue = clampf(fRetValue);
	fRetValue *= 2.0 * fOnOffRandom;
	return fRetValue * Bring2Range(0.0, 60.0, 0.0, 1000.0, fDistance);
}

//смещение от центра цели при попытке попасть - ошибка расчета главного канонира
float Ship_GetBortFireDelta()
{
	aref aCharacter = GetEventData();
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	float fDistance = GetDistance2D(x, z, stf(aCharacter.Ship.Pos.x), stf(aCharacter.Ship.Pos.z));
	aCharacter.DistanceToEnemy = fDistance;

	return Ship_GetBortFireDeltaFromDistance(aCharacter, fDistance, 1.0);
}

string Ship_GetGroupID(ref rCharacter)
{
 	if (CheckAttribute(rCharacter, "SeaAI.Group"))// boal fix bug
    {
        return rCharacter.SeaAI.Group.Name;
    }
    else
    {
        return "";
    }
}
void Ship_NationAgressivePatent(ref rCharacter)
{
	// патент отнимаем всегда, когда палим по другу патента
    ref rMainCharacter = GetMainCharacter();

    if (isMainCharacterPatented())
    {
        if (sti(rCharacter.nation) != PIRATE && GetNationRelation(sti(Items[sti(rMainCharacter.EquipedPatentId)].Nation), sti(rCharacter.nation)) != RELATION_ENEMY && !CheckAttribute(rMainCharacter, "NationAgressivePatentDisable"))
        {
        	TakeItemFromCharacter(rMainCharacter, "patent_" + NationShortName(sti(Items[sti(rMainCharacter.EquipedPatentId)].Nation)));
        	ChangeCharacterHunterScore(rMainCharacter, NationShortName(sti(Items[sti(rMainCharacter.EquipedPatentId)].Nation)) + "hunter", 40);
        	Items[sti(rMainCharacter.EquipedPatentId)].TitulCur = 1; // текущ звание сбросим
        	Items[sti(rMainCharacter.EquipedPatentId)].TitulCurNext = 0; // счетчик звание сбросим
        	RemoveCharacterEquip(rMainCharacter, "patent");// Удаляем патент из экипировки
        	rCharacter.CheckNationBounty = true;
        }
    }
    else
    {// без патента
    	if (sti(rCharacter.nation) != PIRATE && !CheckAttribute(rCharacter, "CheckNationBounty"))   // ещё не наказывал
    	{
    		if (GetNationRelation(GetBaseHeroNation(), sti(rCharacter.nation)) != RELATION_ENEMY)
    		{
				rCharacter.CheckNationBounty = true;
				ChangeCharacterHunterScore(rMainCharacter, NationShortName(sti(rCharacter.nation)) + "hunter", 1 + rand(2));
			}
    	}
    }
}

void SetCompanionsEnemy(ref rCharacter) // ком группы и в кого попали
{
    if(!CheckAttribute(rCharacter, "index")) return;

    int i, iCompanionIndex;
    int nIndex =  sti(rCharacter.index);
    for (i=1; i<COMPANION_MAX; i++)
    {
        iCompanionIndex = GetCompanionIndex(&Characters[nMainCharacterIndex],i);
        if (iCompanionIndex != -1)
        {
            SetCharacterRelationBoth(iCompanionIndex, nIndex, RELATION_ENEMY);
        }
    }
}

void Ship_NationAgressive(ref rMainGroupCharacter, ref rCharacter)// ком группы и в кого попали
{
    ref rMainCharacter = GetMainCharacter();


    if (CheckAttribute(rCharacter, "Coastal_Captain")) return;  // есть флаг есть, то нет ругани, это таможенник и квестовый

	if (rCharacter.relation.1 == 3) return; // Если атакуем враждебный корабль под союзным флагом
	//--> eddy. квест мэра на пирата
	if (rCharacter.id == "MQPirate" && sti(rCharacter.nation) != PIRATE)
	{
		rCharacter.nation = PIRATE;
		Ship_FlagRefresh(characterFromId("MQPirate")); //флаг на лету
      	Pchar.quest.DestroyPirate_PirateIsOut.win_condition.l1 = "ExitFromSea";
		Pchar.quest.DestroyPirate_PirateIsOut.win_condition = "DestroyPirate_PirateIsOut";
	}
	//<-- eddy. квест мэра на пирата
	else
	{
		if (CheckAttribute(rCharacter, "ShipEnemyDisable") || CheckAttribute(rCharacter, "AlwaysFriend"))  return; // флаг не обижаться на выстрелы
	    if( sti(rMainCharacter.nation) == sti(rCharacter.nation) && sti(rMainCharacter.nation) != PIRATE)
	    {
            SetCharacterRelationBoth(sti(rCharacter.index), GetMainCharacterIndex(), RELATION_ENEMY);
            rMainCharacter.nation = PIRATE;
			DoQuestCheckDelay(NationShortName(PIRATE) + "_flag_rise", 0.1); // применение нац отношений флага
	        ChangeCharacterReputation(rMainCharacter, -10);
	    }
	    Ship_NationAgressivePatent(rCharacter); // патент отнимаем всегда, когда палим по другу патента
	    // сброс временного статуса задачи группы
		for (int i=0; i<MAX_SHIP_GROUPS; i++) { AIGroups[i].TempTask = false; } // новая хрень к3

	    SetNationRelation2MainCharacter(sti(rMainGroupCharacter.nation), RELATION_ENEMY);
	    SetCharacterRelationBoth(sti(rMainGroupCharacter.index), GetMainCharacterIndex(), RELATION_ENEMY);
	    //#20200307-02
        SetCompanionsEnemy(rMainGroupCharacter);
	    if (sti(rMainGroupCharacter.nation) != PIRATE)
	    {
	    	ChangeCharacterHunterScore(rMainCharacter, NationShortName(sti(rMainGroupCharacter.nation)) + "hunter", 5);
	    }
    }
    //#20200307-02
    SetCompanionsEnemy(rCharacter);
}
void Ship_FireAction()
{
	aref rCharacter = GetEventData();				// we attack this character
	aref rMainGroupCharacter = GetEventData();		// commander character for rCharacter
	int iRelation = GetEventData();

	if (LAi_IsDead(rCharacter)) return; // fix - нефиг палить в труп!

	// если компаньон — снижаем репутацию
	if (IsCompanion(rCharacter))
	{
		if (CheckAttribute(rCharacter, "CompanionEnemyEnable") && sti(rCharacter.CompanionEnemyEnable))
		{
			if (HasSubStr(rCharacter, "PsHero_") && CheckAttribute(PChar, "GenQuest.PGG_Quest") && PChar.GenQuest.PGG_Quest.PGGid == rCharacter.id) PChar.PGG_EnemyPP = "1";
			SeaAI_SetCompanionEnemy(rCharacter);
			// отключаем компаньона из нашей группы и делаем врагом
		}
		return;
	}
	else
	{
	    if (CheckAttribute(rCharacter, "ShipEnemyDisable") )  return; // флаг не обижаться на выстрелы
	    // убрал || CheckAttribute(rCharacter, "AlwaysFriend") тк если по отпущенному торговцу палить, то должен обижаться
	    DeleteAttribute(rCharacter, "AlwaysFriend");
	}

	if (iRelation != RELATION_ENEMY)
	{
		Ship_NationAgressive(rMainGroupCharacter, rCharacter);// boal 030804 в один метод

		// делаем всю группу враждебной нам
		if (CheckAttribute(rMainGroupCharacter, "SeaAI.Group.Name"))
		{
			Group_SetEnemyToCharacter(rMainGroupCharacter.SeaAI.Group.Name, nMainCharacterIndex);
		}
		NationUpdate();
	}
	else
	{
	    // патент нужно отнять даже если враг
	    Ship_NationAgressivePatent(rCharacter);// патент отнимаем всегда, когда палим по другу патента
	}
	DoQuestCheckDelay("NationUpdate", 0.3);
	RefreshBattleInterface();

	//eddy. корабли с залоченным таском не менять
	if (CheckAttribute(rCharacter, "ShipTaskLock")) return;
	int i;
	string sGroupID, sGroupType;
	ref rGroup;

	// сброс временного статуса задачи группы
	for (i=0; i<MAX_SHIP_GROUPS; i++) { AIGroups[i].TempTask = false; }

	// если форт — выходим
	int iShipType = sti(rCharacter.Ship.Type);
	iShipType = sti(RealShips[iShipType].basetype);

	if (ShipsTypes[iShipType].name == "Fort")
	{
		return;
	}
	sGroupID = Ship_GetGroupID(rMainGroupCharacter);

	Group_SetTaskAttack(PLAYER_GROUP, sGroupID);
}

// ============================================================================
//   УРОН МАЧТАМ, ВЗРЫВ/БРАНДЕР, ПОСТАНОВКА ЗАДАЧ ИИ, ЗАЛП ГГ,
//   ЗАГРУЗКА КОРАБЛЯ, ТРЕКИ, ОГНИ
// ============================================================================
// мод урона по мачтам
#define MAX_CAL_MAST_DMG 50
float Ship_MastDamage()
{
	if (!bSeaActive) return 0.0;
	int iDamageType = GetEventData();
	int iMastNum = GetEventData();
	string sMastNum = "" + iMastNum;
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	float fDamage = GetEventData();
	aref rCharacter = GetEventData();
	if (iDamageType == SHIP_MAST_TOUCH_SHIP)
	{
		aref other = GetEventData();
		if (Boarding_Busy(rCharacter) || Boarding_DamageBlocked(rCharacter, sti(other.index))) return fDamage;
	}
	if (LAi_IsImmortal(rCharacter)) return fDamage;//фикс урона бессмертным
	int iShipType = sti(rCharacter.ship.type);
	ref rBaseShip = GetRealShip(iShipType);
	int nClass = sti(rBaseShip.Class);
	float MastMulti = stf(rBaseShip.MastMultiplier);

	switch (iDamageType)
	{
		case SHIP_MAST_TOUCH_ISLAND:
			fDamage = fDamage + 0.02;
		break;
		case SHIP_MAST_TOUCH_SHIP:
			fDamage = fDamage + 0.02;
		break;
		case SHIP_MAST_TOUCH_BALL:
		    // мод урона по мачтам
           	ref rCannon = GetCannonByType(sti(AIBalls.CurrentBallCannonType));
			int	iBallType = sti(AIBalls.CurrentBallType);
			int nCaliber = sti(rCannon.caliber);
			float nDirect = 0.45; //Glancing
            int nKni = nCaliber;
            if(iBallType == GOOD_KNIPPELS)
                nKni = (nKni + 8) * 1.6;
            if(rand(200 - nClass*25) < nKni)//20% ok 30% good 50+% penetrate
                nDirect = 1.3; 
            float fCbrMDamage = retMin(makefloat(nCaliber) / MAX_CAL_MAST_DMG, 1.0);
            float fClsMDamage = 0.03 * (nClass / 6);
            float tempDamage = 0.0;
            float baseDamage = 0.0;
			switch (iBallType)
			{
				case GOOD_BALLS:
					baseDamage = pow((nClass+1), 1.8) * 0.015;
				break;
				case GOOD_GRAPES:
					baseDamage = 0.0;
					fCbrMDamage = 0.0;
					fClsMDamage = 0.0;
				break;
				case GOOD_KNIPPELS:
					baseDamage = pow((nClass+1), 1.8) * 0.015;
				break;
				case GOOD_BOMBS:
					baseDamage = pow((nClass+1), 1.8) * 0.003;
				break;
			}
			tempDamage = baseDamage * fCbrMDamage + fClsMDamage;
			tempDamage = tempDamage * nDirect;
			string sShip = rBaseShip.BaseName;
			tempDamage = tempDamage * MastMulti;
			if (CheckForUpgrade(rBaseShip,"SUP_SAIL")) tempDamage *= 1.35; //Усиленные паруса доп урон по мачтам
			if (CheckForUpgrade(rBaseShip,"SUP_MASTS")) //Усиленные мачты
			{
				if (nClass == 6 || nClass == 5) tempDamage *= 0.8;
				else if (nClass == 4 || nClass == 3) tempDamage *= 0.86;
				else if (nClass == 2 || nClass == 1) tempDamage *= 0.92;
			}
			if (CheckForUpgrade(rBaseShip,"SUP_ADDITIONALARSENAL")) tempDamage *= 1.1; //Дополнительный арсенал
			if (CheckForUpgrade(rBaseShip,"SUP_REINFORCEDCONSTRUCTION")) tempDamage *= 0.85; //Укрепление трюма и конструкции
			switch (iMastNum)
			{
				case 1:
					tempDamage *= 1.5
				break;
				case 2:
					tempDamage *= 1.0
				break;
				case 3:
					tempDamage *= 0.8
				break;
				case 4:
					tempDamage *= 2			//Потенциальный баг - А ЧТО ДЕЛАТЬ ЕСЛИ МАЧТ больше 4??? хотя бы дефолтное значение нужно было делать.
				break;
			}
        int iBallCharacterIndex = GetEventData();
		if (Boarding_DamageBlocked(rCharacter, iBallCharacterIndex)) return fDamage;
		// LEO: у ядра бывает невалидный владелец (-1: форт/среда) → GetCharacter(-1) спамил ошибку при сломе мачты.
		//      Гард: модификаторы по владельцу (дистанция/перемирие/нация) - только при валидном; урон мачте наносим всегда.
		if (iBallCharacterIndex >= 0)
		{
			ref rBallCharacter = GetCharacter(iBallCharacterIndex);
			float fDistance = Ship_GetDistance2D(rCharacter, rBallCharacter);
			tempDamage = tempDamage * Bring2Range(1.1, 0.2, 0.0, makefloat(GetShootDistance(rBallCharacter, iBallType)), fDistance);
			if (S_armor(rCharacter)) tempDamage *= 0.25; // СФ
			if (SpeakTruce_HitImmune(rCharacter, iBallCharacterIndex)) tempDamage = 0.0; // ПЕРЕМИРИЕ Speak: ядра ГГ/компаньонов мачты не ломают (seaspeak.c)
			fDamage = fDamage + tempDamage;
			//#20190113-06
			if(GetNationRelation(sti(rBallCharacter.Nation), sti(rCharacter.nation)) != RELATION_FRIEND)
			{
				if(CheckAttribute(rCharacter, "SeaAI.fortSanctuary"))
				{
					DeleteAttribute(rCharacter, "SeaAI.fortSanctuary")
					rCharacter.SeaAI.hitInSanctuary = true;
				}
			}
		}
		else
		{
			if (S_armor(rCharacter)) tempDamage *= 0.25; // СФ
			fDamage = fDamage + tempDamage;
		}
		break;
	}

	fDamage = Clampf(fDamage);

	// если мачта упала — проигрываем звук
	if (fDamage >= 1.0)
	{
		Play3DSound("mast_fall", x, y, z);
		rCharacter.ship.sp = CalculateShipSP(rCharacter);  // рефрешим паруса от модели
		RefreshBattleInterface();
	}

	if (checkAttribute(rBaseShip, "mastHPrep."+sMastNum) )
	{
		if (fDamage < makefloat(rBaseShip.mastHPrep.(sMastNum))) 
		{
			rBaseShip.mastHPrep.(sMastNum) = makefloat(rBaseShip.mastHPrep.(sMastNum)) - fDamage;
			fDamage = 0.0;
		}
		else
		{
			fDamage -= makefloat(rBaseShip.mastHPrep.(sMastNum));
			rBaseShip.mastHP.(sMastNum) = fDamage;	
			DeleteAttribute(rBaseShip, "mastHPrep."+sMastNum);
		}
	}
	rBaseShip.mastHP.(sMastNum) = fDamage;	//текущая поврежденность мачты, известная движку

	return fDamage;
}

float Ship_GetSailState(ref rCharacter)
{
	float fSailState = 1.0;
	SendMessage(rCharacter, "le", MSG_SHIP_GET_SAIL_STATE, &fSailState);
	return fSailState;
}
// boal 27.09.05 --> LEO - запилить пиздатого брандера - ВЗРЫВ нахрен (12.07.2025) - сделано :)
void Ship_SetExplosion(ref rCharacter, ref	rShipObject)
{
    int i = 0;

	PostEvent(SHIP_BRANDER_DETONATE, 800, "l", sti(rCharacter.index));
	Ship_SetTaskNone(SECONDARY_TASK, sti(rCharacter.index));
	if (sti(InterfaceStates.OtherParticles) == 1)
	{
		CreateParticleSystem("Cruit_camera", stf(rCharacter.Ship.pos.x), stf(rCharacter.Ship.pos.y), stf(rCharacter.Ship.pos.z),0,1.0,0,0);
		CreateParticleSystem("ShipExplode2", stf(rCharacter.Ship.pos.x), stf(rCharacter.Ship.pos.y), stf(rCharacter.Ship.pos.z),0,1.0,0,0);
		CreateParticleSystem("ShipExplode3", stf(rCharacter.Ship.pos.x), stf(rCharacter.Ship.pos.y), stf(rCharacter.Ship.pos.z),0,1.0,0,0);
		CreateParticleSystem("ShipFire2", stf(rCharacter.Ship.pos.x), stf(rCharacter.Ship.pos.y), stf(rCharacter.Ship.pos.z),0,1.0,0,0);
		CreateParticleSystem("blast_big", stf(rCharacter.Ship.pos.x), stf(rCharacter.Ship.pos.y), stf(rCharacter.Ship.pos.z),0,1.0,0,0);
		CreateParticleSystem("blast_wave", stf(rCharacter.Ship.pos.x), stf(rCharacter.Ship.pos.y), stf(rCharacter.Ship.pos.z),0,1.0,0,0);
		SendMessage(rCharacter, "lfff", MSG_SHIP_DETONATE, 5.0, 4.0, 1500.0);
	}
	Play3DSound("Cruit_camera", stf(rCharacter.Ship.pos.x), stf(rCharacter.Ship.pos.y), stf(rCharacter.Ship.pos.z));
}
void Ship_SetSailState(int iCharacterIndex, float fSailState)
{
	SendMessage(&AISea, "laf", AI_MESSAGE_SHIP_SET_SAIL_STATE, &Characters[iCharacterIndex], fSailState);
}

void Ship_SetTaskNone(int iTaskPriority, int iCharacterIndex)
{
	ref rCharacter = GetCharacter(iCharacterIndex);
	rCharacter.SeaAI.Task = AITASK_NONE;
}

void Ship_SetTaskAbordage(int iTaskPriority, int iCharacterIndex, int iCharacterIndexAbordageTo)
{
	if (iCharacterIndex < 0 || iCharacterIndex >= TOTAL_CHARACTERS || iCharacterIndexAbordageTo < 0 || iCharacterIndexAbordageTo >= TOTAL_CHARACTERS) return;
	ref rCharacter = GetCharacter(iCharacterIndex);
	if (Boarding_Busy(rCharacter) || Boarding_Busy(GetCharacter(iCharacterIndexAbordageTo))) return;
	rCharacter.SeaAI.Task = AITASK_ABORDAGE;
	rCharacter.SeaAI.Task.Target = iCharacterIndexAbordageTo;
	SendMessage(&AISea, "lllaa", AI_MESSAGE_SHIP_SET_TASK, AITASK_ABORDAGE, iTaskPriority, &Characters[iCharacterIndex], &Characters[iCharacterIndexAbordageTo]);
}

void Ship_SetTaskBrander(int iTaskPriority, int iCharacterIndex, int iCharacterIndexBranderTo)
{
	ref rCharacter = GetCharacter(iCharacterIndex);
	if (Boarding_Busy(rCharacter)) return;
	rCharacter.SeaAI.Task = AITASK_BRANDER;
	rCharacter.SeaAI.Task.Target = iCharacterIndexBranderTo;
	SendMessage(&AISea, "lllaa", AI_MESSAGE_SHIP_SET_TASK, AITASK_BRANDER, iTaskPriority, &Characters[iCharacterIndex], &Characters[iCharacterIndexBranderTo]);
}

void Ship_SetTaskMove(int iTaskPriority, int iCharacterIndex, float x, float z)
{
	ref rCharacter = GetCharacter(iCharacterIndex);
	rCharacter.SeaAI.Task = AITASK_MOVE;
	rCharacter.SeaAI.Task.Pos.x = x;
	rCharacter.SeaAI.Task.Pos.z = z;
	SendMessage(&AISea, "lllaff", AI_MESSAGE_SHIP_SET_TASK, AITASK_MOVE, iTaskPriority, &Characters[iCharacterIndex], x, z);
}

void Ship_SetTaskRunaway(int iTaskPriority, int iCharacterIndex, int iFromIndex)
{
	// LEO: сторожевики форта НЕ бегут никогда — держат бой у форта. Жёсткий гейт на всех код-путях; гард idx>=0 ДО CheckAttribute (Storm).
	if (iCharacterIndex >= 0)
	{
		if (CheckAttribute(&Characters[iCharacterIndex], "fortDefender")) { if (sti(Characters[iCharacterIndex].fortDefender) == 1) return; }
	}
	ref rCharacter = GetCharacter(iCharacterIndex);
	rCharacter.SeaAI.Task = AITASK_RUNAWAY;
	rCharacter.SeaAI.Task.Target = iFromIndex;
	SendMessage(&AISea, "llla", AI_MESSAGE_SHIP_SET_TASK, AITASK_RUNAWAY, iTaskPriority, &Characters[iCharacterIndex]);
	Ship_SetSailState(iCharacterIndex, 1);
}

void Ship_SetTaskAttack(int iTaskPriority, int iCharacterIndex, int iCharacterIndexVictim)
{
	ref rCharacter = GetCharacter(iCharacterIndex);
	rCharacter.SeaAI.Task = AITASK_ATTACK;
	rCharacter.SeaAI.Task.Target = iCharacterIndexVictim;
	SendMessage(&AISea, "lllaa", AI_MESSAGE_SHIP_SET_TASK, AITASK_ATTACK, iTaskPriority, &Characters[iCharacterIndex], &Characters[iCharacterIndexVictim]);
}

void Ship_SetTaskSailTo(int iTaskPriority, int iCharacterIndex, int iCharacterIndexSailTo)
{
	ref rCharacter = GetCharacter(iCharacterIndex);
	rCharacter.SeaAI.Task = AITASK_MOVE;
	rCharacter.SeaAI.Task.Target = iCharacterIndexSailTo;
	SendMessage(&AISea, "lllaa", AI_MESSAGE_SHIP_SET_TASK, AITASK_MOVE, iTaskPriority, &Characters[iCharacterIndex], &Characters[iCharacterIndexSailTo]);
}

void Ship_SetTaskDrift(int iTaskPriority, int iCharacterIndex)
{
	ref rCharacter = GetCharacter(iCharacterIndex);
	rCharacter.SeaAI.Task = AITASK_DRIFT;
	SendMessage(&AISea, "llla", AI_MESSAGE_SHIP_SET_TASK, AITASK_DRIFT, iTaskPriority, &Characters[iCharacterIndex]);
}

void Ship_SetTaskDefendGroup(int iTaskPriority, int iCharacterIndex, string sDefendGroup)
{
	SendMessage(&AISea, "lllas", AI_MESSAGE_SHIP_SET_TASK, AITASK_DEFEND_GROUP, iTaskPriority, &Characters[iCharacterIndex], sDefendGroup);
}

void Ship_SetTaskDefend(int iTaskPriority, int iCharacterIndex, int iCharacterIndex2Defend)
{
	ref rCharacter = GetCharacter(iCharacterIndex);
	rCharacter.SeaAI.Task = AITASK_DEFEND;
	rCharacter.SeaAI.Task.Target = iCharacterIndex2Defend;
	SendMessage(&AISea, "lllaa", AI_MESSAGE_SHIP_SET_TASK, AITASK_DEFEND, iTaskPriority, &Characters[iCharacterIndex], &Characters[iCharacterIndex2Defend]);
}

// LEO: множитель упреждения АВТО-залпа ГГ (пробел). Движок (AIShipCannonController::Fire) дефолтит 2.0 (handicap — перелёт по игроку).
//      Калибруем тут; 1.0 = физически точно. Движок читает pchar.AutoFireLeadScale (дефолт 1.0) БЕЗ пересборки.
#define AUTO_LEAD_SCALE   1.0

// LEO: множитель упреждения залпа ИИ. В движке был хардкод 2.0 (handicap — ИИ клал ядра вперёд по курсу); 1.0 = точно как игрок.
//      Движок (AIShipCannonController::Fire) читает chr.AILeadScale (ставим при инициализации корабля), дефолт 1.0, БЕЗ пересборки.
#define AI_LEAD_SCALE     1.0

// LEO: донаводка КАЖДОЙ пушки для ИИ (покадровый доворот стволов на свежую упреждённую точку). 1 = кучнее, 0 = весь борт в одну точку.
//      Движок читает chr.AIPerCannonAim (отдельно от ГГ). Выключить при просадке fps.
#define AI_PER_CANNON_AIM 1

// LEO: физ. скрытие мешей выбитых пушек. Движок прячет меш, когда isDamaged() (уничтожена / не установлена / сверх числа пушек,
//      скрипт метит damages=1.0). Возврат при установке/починке — сам. Только модели с мешами group_name+name локатора (напр. "cannonr_15").
//      1 = вкл, 0 = выкл без пересборки.
#define CANNON_MESH_HIDE  1

// LEO: матросы-перезарядчики (SAILORSPOINTS) не обслуживают "выбитую" пушку (уничтожена/не установлена/сверх числа; damages.cN>=1.0).
//      Движок (либа sailors) гасит флагом disabled ближайшую точку-перезарядки. 1 = вкл, 0 = выкл.
#define CANNON_RELOAD_SKIP 1

// LEO: РАЗРУШАЕМОСТЬ КОРПУСА по попаданию (прототип). Движок вероятностно сносит ближайший кусок-меш dmg_<N> (персист в Ship.HullDamage).
//      Только модели с мешами dmg_<N>.
#define HULL_DAMAGE_PARTS        1    // 1 = вкл, 0 = выкл (без пересборки)
#define HULL_DAMAGE_PART_CHANCE  0.5  // шанс снести кусок при попадании
#define HULL_DAMAGE_PART_RADIUS  8.0  // радиус поиска ближайшего куска к точке удара (юниты модели)

// LEO: ОПЦИЯ "залп бортом нос->корма" (rolling broadside). ЕДИНЫЙ тоггл для ВСЕХ кораблей: InterfaceStates.RollingBroadside
//      (дефолт выкл = разнобой, читается в Ship_Add2Sea). Движок (Fire2Position) задерживает выстрел по продольной позиции: нос первым,
//      корма последней; станция через все деки — вместе. Ниже — глобальные тюнинги каскада (атрибуты RollingBroadsideStep/Reverse):
#define ROLLING_BROADSIDE_STEP    0.125  // базовый шаг между стволами (сек) для ОПЫТНОГО расчёта; зелёные команды катят до ~2.15x медленнее (см. skill-множитель в Ship_Add2Sea)
// почему-то влияет квадратично относительно 0.1
#define ROLLING_BROADSIDE_REVERSE 0     // 1, если у моделей нос/корма по продольной оси Z перепутаны (развернёт каскад)

// LEO: живое применение тоггла "нос->корма" ко ВСЕМ кораблям при смене опции в бою (вне моря — при след. Ship_Add2Sea). Из options_game_settings.c.
void RollingBroadside_ApplyLive()
{
	if (!bSeaActive) return;
	int bRoll = 0;
	if (CheckAttribute(&InterfaceStates, "RollingBroadside")) bRoll = sti(InterfaceStates.RollingBroadside);
	for (int i = 0; i < iNumShips; i++)
	{
		if (Ships[i] < 0) continue; // LEO: пустой/мёртвый слот -> не звать GetCharacter(-1) (спам)
		ref rc = GetCharacter(Ships[i]);
		rc.RollingBroadside = bRoll;
	}
}

void Ship_DoFire()
{
	// Gregg: в палубном прицеле 1-го лица огонь идёт по МОДЕЛИ «зажал-свёл — отпустил-выстрелил» (движок даёт залп
	//        на отпускании Ship_Fire, см. AIShipCameraController::Execute). Press-огонь здесь глушим, чтобы не было
	//        двойного залпа и чтобы фокусировка успевала набраться. Вне палубы (3-е лицо/тактика) — как было.
	if (SeaCameras.Camera == "SeaDeckCamera") return;
	int bCameraOutside = SeaCameras_isCameraOutside();
	Characters[nMainCharacterIndex].AutoFireLeadScale = AUTO_LEAD_SCALE; // knob упреждения авто-залпа (скрипт-сторона)
	SendMessage(&AISea, "lal", AI_MESSAGE_CANNON_FIRE, &Characters[nMainCharacterIndex], bCameraOutside);
}

// Прицельный залп ГГ (тактический): targetIdx>=0 -> по цели с упреждением движка (leadScale, в AIShipCannonController::Fire), иначе -> в точку (x,z).
// Стреляют только борта, чьи арка+дальность покрывают цель.
void Ship_DoFireAt(int targetIdx, float x, float z, float leadScale)
{
	if (targetIdx >= 0)
		SendMessage(&AISea, "lalaf", AI_MESSAGE_CANNON_FIRE_AT, &Characters[nMainCharacterIndex], 1, &Characters[targetIdx], leadScale);
	else
		SendMessage(&AISea, "lalff", AI_MESSAGE_CANNON_FIRE_AT, &Characters[nMainCharacterIndex], 0, x, z);
}

void Ship_StartLoad()
{
	aref rCharacter = GetEventData();
	if(rCharacter.id != "0" && sti(rCharacter.ship.type) != SHIP_NOTUSED)
	{
		ref refBaseShip;
		int nShipType = GetCharacterShipType(rCharacter);
		refBaseShip = GetRealShip(nShipType);
		SetTextureForShip(refBaseShip, rCharacter);
	}
}

void Ship_EndLoad()
{
	SetTexturePath(0, "");
}

void Ship_SetTrackSettings(ref rCharacter)
{
	ref rShip = GetRealShip(GetCharacterShipType(rCharacter));

	rCharacter.Ship.Track.Enable = rShip.Track.Enable;
	if (!sti(rShip.Track.Enable)) return;

	rCharacter.Ship.Track.TrackDistance = 1.0;
	rCharacter.Ship.Track.WaveHeight2 = 0.9;
	rCharacter.Ship.Track.WaveHeight1 = 0.2;

	rCharacter.Ship.Track1.Texture = "ships\trailShip.dds";
	rCharacter.Ship.Track1.TrackWidthSteps = 12.0;

	rCharacter.Ship.Track2.Texture = "ships\trailKeel.dds";
	rCharacter.Ship.Track2.TrackWidthSteps = 4.0;

	rCharacter.Ship.Track1.ZStart = rShip.Track1.ZStart;
	rCharacter.Ship.Track1.LifeTime = rShip.Track1.LifeTime;
	rCharacter.Ship.Track1.Width = rShip.Track1.Width;
	rCharacter.Ship.Track1.Speed = rShip.Track1.Speed;

	rCharacter.Ship.Track2.ZStart = rShip.Track2.ZStart;
	rCharacter.Ship.Track2.LifeTime = rShip.Track2.LifeTime;
	rCharacter.Ship.Track2.Width = rShip.Track2.Width;
	rCharacter.Ship.Track2.Speed = rShip.Track2.Speed;
}

void Ship_SetLightsOff(ref rCharacter, float fTime, bool bLights, bool bFlares, bool bNow)
{
	SendMessage(rCharacter, "lflll", MSG_SHIP_SETLIGHTSOFF, fTime, bLights, bFlares, bNow);
	rCharacter.Ship.Lights = 0; //fix
}

void Ship_SetLightsAndFlares(ref rCharacter)
{
	if(sti(sea.lights) == 1)
	{
		rCharacter.Ship.Flares = 1;
		rCharacter.Ship.Lights = 1;
	}
	else
	{
		rCharacter.Ship.Flares = 0;
		rCharacter.Ship.Lights = 0;
	}
}

// ============================================================================
//   ВВОД КОРАБЛЯ В МОРЕ, СТАТИЧЕСКИЕ ЗВУКИ, ПЕРЕЗАРЯДКА/СМЕНА ЗАРЯДА, ИКОНКИ HUD
// ============================================================================
void Ship_Add2Sea(int iCharacterIndex, bool bFromCoast, string sFantomType)
{
	ref		rCharacter = GetCharacter(iCharacterIndex);

	DeleteAttribute(rCharacter, "ship.cannons.chance_for_explode"); //to_do у нас не юзается

	aref	arCharacter; makearef(arCharacter, Characters[sti(rCharacter.index)]);

	int iShipType = sti(rCharacter.Ship.Type);
	if(iShipType == SHIP_NOTUSED)
	{
		return;
	}

	ShipDebris_InitCharacter(rCharacter);
	ShipDestruction_InitCharacter(rCharacter, bSeaLoad);
	int iRealShip = sti(RealShips[iShipType].basetype);

	if(!CheckAttribute(&RealShips[iShipType], "name"))
	{
		Trace("Ship_Add2Sea() : SHIP " + iShipType + ", have no name!   NPCid = "+ rCharacter.id);
		if (MOD_BETTATESTMODE == "On") Log_Info("Ship_Add2Sea ERROR : SHIP " + iShipType + ", have no name!   NPCid = "+ rCharacter.id);
		return;
	}
	if (iRealShip >= SHIP_TYPES_QUANTITY)
	{
		Trace("Character.id = " + rCharacter.id + ", have invalid ship type = " + iShipType + ", and try load to sea");
		return;
	}
	// 22.04.05 сброс всех отношений
    if (!CheckAttribute(rCharacter, "AlwaysEnemy") && !CheckAttribute(rCharacter, "AlwaysFriend")) // у врагов не трем
    {
    	DelCharacterRelation(iCharacterIndex);
    }
    else
    {
		if (CheckAttribute(rCharacter, "AlwaysFriend"))
		{
			SetCharacterRelationBoth(iCharacterIndex, GetMainCharacterIndex(), RELATION_FRIEND);
		}
		if (CheckAttribute(rCharacter, "AlwaysEnemy"))
		{
			SetCharacterRelationBoth(iCharacterIndex, GetMainCharacterIndex(), RELATION_ENEMY);
		}
    }
    DeleteAttribute(rCharacter, "Ship_SetTaskAbordage"); // boal
    DeleteAttribute(rCharacter, "Killer.status"); // boal 09.06.05 SUPER FIX
    DeleteAttribute(rCharacter, "CheckFlagYet"); // флаг распознования врага
    DeleteAttribute(rCharacter, "CheckNationBounty"); // Проверка НЗГ за нацию друга базе
    DeleteAttribute(rCharacter, "ShipCannonChargeType"); // флаг офов, чем пулять постоянно, ставится игроком командами, до след приказа Атака или конца снарядов
	// переговоры (Speak): хвосты прошлых встреч не тащим — протухший speak.tax прятал бы откуп, SpeakTruce дарил бы нейтралитет новому кораблю
	DeleteAttribute(rCharacter, "speak");
	DeleteAttribute(rCharacter, "SpeakTruce");
	DeleteAttribute(rCharacter, "surr.drift");
	// боевая мораль (Ship_CheckMorale): прошлый бой в новый не тащим, снимем свежий снимок
	DeleteAttribute(rCharacter, "seatime.tempmorale");
	DeleteAttribute(rCharacter, "seatime.thinkSurr");
	DeleteAttribute(rCharacter, "seatime.lasthit");
	DeleteAttribute(rCharacter, "seatime.lastfired");
	DeleteAttribute(rCharacter, "seatime.lastcrew");
	DeleteAttribute(rCharacter, "seatime.lastHP");
	DeleteAttribute(rCharacter, "seatime.basecrewmorale");
	DeleteAttribute(rCharacter, "seatime.basecrewquantity");
	DeleteAttribute(rCharacter, "seatime.enemydead");
	DeleteAttribute(rCharacter, "seatime.timesincelast");

	if (CheckAttribute(rCharacter, "DontCheckFlag"))
	{
	    rCharacter.CheckFlagYet = true;
	}
    rCharacter.TmpSkillRecall = "0"; //boal 25.05.04 оптимизация расчета, когда == 0 считаем скилы
    rCharacter.Tmp.fShipSpeed  = 1; // начальная инициация
	rCharacter.Tmp.fWatchFort  = rand(15); // оптимизация оглядок на форт
	rCharacter.Tmp.fWatchFort.Qty = 15;

	if (iCharacterIndex == GetMainCharacterIndex()) { // NK 04-09-16 clears timer attribute, -21 for updatetime
		rCharacter.seatime = 0;
		rCharacter.lastupdateseatime = 0;
	}
	if (!bSeaLoad) DeleteAttribute(rCharacter, "SeaAI.BoardingRetry");
	if (iCharacterIndex >= FANTOM_CHARACTERS)
	{
		if (!bSeaLoad)
		{
			if (Boarding_Busy(rCharacter)) SendMessage(&AISea, "la", AI_MESSAGE_SHIP_BOARDING_END, rCharacter);
			DeleteAttribute(rCharacter, "SeaAI.Boarding");
		}
		// LEO: мастер-флаг surrendered нигде не снимается насовсем — переиспользованный фантом тащил его в новый энкаунтер (рождался
		// нейтральным/в дрейфе, см. ~1456). Чистим ТОЛЬКО для фантомов; квестовые/Speak-сдачи (index < FANTOM_CHARACTERS) не трогаем
		DeleteAttribute(rCharacter, "surrendered");
		DeleteAttribute(rCharacter, "surrendered.seatime");
		// LEO: тот же класс — пасхалка "битва пяти воинств" метит свои корабли запретом абордажа и переговоров
		// (FiveArmies_SeaCheck). Слот фантома переиспользуется, и запрет уезжал в ЧУЖИЕ энкаунтеры: обычные
		// корабли переставали браться на абордаж. Снимаем всё разом; настоящей пасхалке метки вернёт Sea_FirstInit.
		if (CheckAttribute(rCharacter, "FiveArmies"))
		{
			DeleteAttribute(rCharacter, "FiveArmies");
			DeleteAttribute(rCharacter, "Abordage.Enable");
			DeleteAttribute(rCharacter, "Tasks.CanBoarding");
			// имя воинства ("Гномы") иначе оставалось на чужом корабле. Именно ПЕРЕИМЕНОВЫВАЕМ, а не
			// удаляем: генератор энкаунтеров v2 (Fantom_GenerateShips_ForEnc_v2) имя не выдаёт вовсе,
			// и снос атрибута оставил бы корабль безымянным - надписи читают Ship.Name напрямую.
			SetRandomNameToShip(rCharacter);
		}
		SetBaseShipData(rCharacter);
		Ship_SetFantomData(rCharacter);

		Fantom_SetUpgrade(rCharacter, sFantomType);//бермудку до выдачи пушек.
        Fantom_SetCannons(rCharacter, sFantomType);
		Fantom_SetBalls(rCharacter, sFantomType);
		Fantom_SetGoods(rCharacter, sFantomType);
		SaveCurrentNpcQuestDateParam(rCharacter, "Add2SeaTime"); //фантомы помнят время входа в море
	}
	else 
	{
		FixOverMaxCannons(rCharacter);//уменьшаем орудия до максимального калибра
	}
    AcceptWindCatcherPerk(rCharacter); // применение перка ловец ветра, тут работает, а не где был в к3 boal 02/08/06

	CharacterUpdateShipFromBaseShip(iCharacterIndex);

	rCharacter.Ship.Ang.y = 0.0;

	Ship_ClearImpulseStrength(rCharacter);
	Ship_SetLightsAndFlares(rCharacter);
	Ship_SetTrackSettings(rCharacter);

	aref baseship;
	ref refBaseShip;

	makearef(baseship,rCharacter.ship);
	int nShipType = GetCharacterShipType(rCharacter);
	refBaseShip = GetRealShip(nShipType);

	rCharacter.Tmp.BortsReloaded.Event = false;
	rCharacter.Tmp.BortsReloaded.cannonl = false;
	rCharacter.Tmp.BortsReloaded.cannonr = false;

	rCharacter.Features.GeraldSails = true;
	rCharacter.Ship.LastBallCharacter = -1;
	rCharacter.Ship.WindDotShip = 1.0;
	rCharacter.Ship.Speed.z = 0.0;
	rCharacter.Ship.Speed.y = 0.0;
	rCharacter.Ship.Speed.x = 0.0;

	//  убрать это, когда будет в ядре работать
	rCharacter.Ship.BoxSize.x = 1.0;  // должно считаться в ядре :( не работает
	rCharacter.Ship.BoxSize.y = 1.0;
	rCharacter.Ship.BoxSize.z = 2.0;

	rCharacter.Ship.Strand = false;
	rCharacter.Ship.Strand.SndID = SOUND_INVALID_ID;
	rCharacter.Ship.Stopped = false;
	rCharacter.Ship.POS.Mode = SHIP_SAIL;
	rCharacter.Ship.SeaAI.Init.AbordageDistance = 30.0;
	rCharacter.AILeadScale = AI_LEAD_SCALE; // LEO: упреждение залпа ИИ — движок читает этот атрибут (1.0 = точно; было хардкод 2.0)
	rCharacter.AIPerCannonAim = AI_PER_CANNON_AIM; // LEO: донаводка каждой пушки для ИИ (движок читает атрибут)
	rCharacter.Ship.CannonMeshHide = CANNON_MESH_HIDE; // LEO: физ. скрытие меша выбитой/неустановленной пушки (движок читает атрибут)
	rCharacter.Ship.CannonReloadSkip = CANNON_RELOAD_SKIP; // LEO: матросы не перезаряжают выбитую/отсутствующую пушку (движок читает атрибут)
	rCharacter.Ship.HullDamageParts = HULL_DAMAGE_PARTS;             // LEO: разрушаемость корпуса по попаданию (куски dmg_<N>)
	rCharacter.Ship.HullDamagePartChance = HULL_DAMAGE_PART_CHANCE;  // LEO: шанс снести кусок при попадании
	rCharacter.Ship.HullDamagePartRadius = HULL_DAMAGE_PART_RADIUS;  // LEO: радиус поиска ближайшего куска
	// LEO: залп "нос->корма" — глобальные тюнинги каскада + ЕДИНЫЙ тоггл на КАЖДЫЙ корабль. Шаг МОДУЛИРУЕТСЯ навыком канониров: берём
	//      CalcShipCannonFireTimeMult (~0.15с опытные .. ~0.32с зелёные), нормируем на 0.15 -> множитель ~1.0 .. ~2.15
	aref aChRoll; makearef(aChRoll, characters[iCharacterIndex]);
	// LEO: Новые защитники порта ещё не имеют TmpSkill: первый расчёт залпа выполняется до создания корабля.
	Ship_UpdateTmpSkills(rCharacter);
	float fRollSkillMult = CalcShipCannonFireTimeMult(aChRoll, refBaseShip) / 0.15;
	rCharacter.RollingBroadsideStep = ROLLING_BROADSIDE_STEP * fRollSkillMult;
	rCharacter.RollingBroadsideReverse = ROLLING_BROADSIDE_REVERSE;
	int bRoll = 0;
	if (CheckAttribute(&InterfaceStates, "RollingBroadside")) bRoll = sti(InterfaceStates.RollingBroadside);
	rCharacter.RollingBroadside = bRoll; // движок читает RollingBroadside у всех кораблей
	ref	rCannon = GetCannonByType(sti(rCharacter.Ship.Cannons.Type));
	float range = 50;
	if (CheckAttribute(rCannon, "FireRange")) range = stf(rCannon.FireRange);
	rCharacter.Ship.SeaAI.Init.AttackDistance = range  / (1.4 + frandSmall(2.0));  // это очень важный параметр, он определяет дистанцию АИ, на которой будет стараться держаться корабль
	rCharacter.Ship.SeaAI.Init.AttackDistance.qtyTryChangeCannon = 0; //счетчик попыток смены боеприпасов при полном боезапаса, для фикса неубегаек
	DeleteAttribute(rCharacter, "AIBehaviorAuto"); // LEO: сброс кэша типа поведения ИИ — переклассифицировать под текущие апгрейды (Фаза 1)
	DeleteAttribute(rCharacter, "Ship.SeaAI.SailCap"); // LEO: снять крейсерский кап парусов при входе в море — иначе застревает на захваченном/нанятом экс-командире эскадры
	rCharacter.Ship.SeaAI.Init.FollowDistance = makefloat(180 + rand(120)); // было 300, делаю рандом для разнообразия поведения компаньонов и следования в группе
	DeleteAttribute(rCharacter, "Ship.SeaAI.Init.AttackRangeSet");
	// больше гибкости параметрам ИИ-корабля
    rCharacter.Ship.SeaAI.Init.WindC = 2.0;
    rCharacter.Ship.SeaAI.Init.FortC = 2.75;
    rCharacter.Ship.SeaAI.Init.DefEnemyC = 5.0;
    rCharacter.Ship.SeaAI.Init.MinEnemyC = 3.0;
    rCharacter.Ship.SeaAI.Init.MaxEnemyC = 10.0;
    rCharacter.Ship.SeaAI.Init.IslandDist = 90.0;
    rCharacter.Ship.SeaAI.Init.IslandC = 2.5;
    rCharacter.Ship.SeaAI.Init.POSRatio = 0.3;
    rCharacter.Ship.SeaAI.Init.LowPredTime = 20.0;
    rCharacter.Ship.SeaAI.Init.HighPredTime = 50.0;
    rCharacter.Ship.SeaAI.Init.LowSpeed = 0.1;
    rCharacter.Ship.SeaAI.Init.HighSpeed = 50.0;
    rCharacter.Ship.SeaAI.Init.MinBrakeDist = 15.0;
    rCharacter.Ship.SeaAI.Init.FortEnemyDist = 750.0;
    //#20190313-01
    rCharacter.Ship.SeaAI.Init.MultiBoardDist = 100.0;

    rCharacter.Ship.SeaAI.Init.MaxTackAccumDistance = 200.0;
    rCharacter.Ship.SeaAI.Init.TackOffDistance = 50.0;
    rCharacter.Ship.SeaAI.Init.TackCheckAngle = (PIm2 / 13.0);
    rCharacter.Ship.SeaAI.Init.TackTurnAngle = (PIm2 / 12.0);

    rCharacter.Ship.SeaAI.Init.AttackRotate = 1.65;
    rCharacter.Ship.SeaAI.Init.AttackRand = 50.0;

	rCharacter.Ship.Sounds = "";

	if (bFromCoast == false) { rCharacter.Ship.Speed.z = 1.0; }

	// создаём корабль
	Event("eSwitchPerks", "l", iCharacterIndex);											// Perks
	Event(SHIP_UPDATE_PARAMETERS, "lf", iCharacterIndex, stf(rCharacter.Ship.Speed.z));		// Parameters
	ShipModelrList[iNumShips] = GetCurrentModelrNumber();

	//ugeen --> жесть !! у энкаунтеров не снят атрибут Ship.Sink <-- 3 часа трахался из-за этой ошибки :(  <-- из-за нее некорректно работал мод флагов
	if(CheckAttribute(rCharacter,"Ship.Sink")) {trace("ShipSink rCharacter.id = " + rCharacter.id); DeleteAttribute(rCharacter,"Ship.Sink");}

	CreateEntity(&Characters[iCharacterIndex], "Ship");
    Event(SHIP_UPDATE_PARAMETERS, "lf", iCharacterIndex, stf(rCharacter.Ship.Speed.z));		// Parameters      после создания корабля
	aref arChar, arShip;
	makearef(arChar, characters[iCharacterIndex]);
	makearef(arShip, RealShips[iShipType]);

	Ship_CreateStaticSounds(rCharacter);

	SendMessage(&AISea, "liaa", AI_MESSAGE_ADD_SHIP, &Characters[iCharacterIndex], arChar, arShip);

	ReloadProgressUpdate();
	
	bool bok = CheckAttribute(PChar, "BonsFlintRunning") && rCharacter.id == "BSBons";
	if(CheckAttribute(rCharacter, "ShipWreck") || bok)
	{
		MakeSailDmg(GetCharacterIndex(rCharacter.id), 90);
	}

	Ships[iNumShips] = iCharacterIndex;
	LoadedChars[iNumShips] = iCharacterIndex; // после загрузки локации в море
	rCharacter.curshipnum = iNumShips;
	trace("AIShip  iNumShips : " + iNumShips + " ShipModelrList : " + ShipModelrList[iNumShips] + " ShipName : " + rCharacter.Ship.Name + " Ships = " + Ships[iNumShips]);
	iNumShips++;
}

void Ship_RecreateStaticSounds()
{
	for (int i=0; i<iNumShips; i++) {
		if (makeint(Ships[i]) > -1)
			Ship_CreateStaticSounds(&Characters[Ships[i]]);
	}
	//#20190117-03
	if (CheckAttribute(&WeatherParams, "Rain.Sound") && sti(WeatherParams.Rain.Sound))
        Whr_SetRainSound(true, sti(Weathers[iCurWeatherNum].Night));
}

void Ship_CreateStaticSounds(ref rCharacter)
{
    if (!CheckAttribute(rCharacter, "Ship.Pos.x")) return; // fix
	if (GetCharacterShipType(rCharacter) == SHIP_NOTUSED) return; // может уже не быть

    ref refBaseShip = GetRealShip(GetCharacterShipType(rCharacter));
	// звук воды
	float fX = stf(rCharacter.Ship.Pos.x);
	float fY = stf(rCharacter.Ship.Pos.y);
	float fZ = stf(rCharacter.Ship.Pos.z);
	rCharacter.Ship.Sounds.WaterID.x = 0.0;
	rCharacter.Ship.Sounds.WaterID.y = 0.0;
	rCharacter.Ship.Sounds.WaterID.z = 0.12;
	rCharacter.Ship.Sounds.WaterID = Play3DSoundComplex("ship_water_" + refBaseShip.Soundtype, fX, fY, fZ, true, false);
	if (sti(rCharacter.Ship.Sounds.WaterID) > 0) { Sound_SetVolume(sti(rCharacter.Ship.Sounds.WaterID), 0.0); }
	rCharacter.Ship.Sounds.WaterSpeed = 30.0;

	// звук парусов
	rCharacter.Ship.Sounds.SailsID.x = 0.0;
	rCharacter.Ship.Sounds.SailsID.y = 0.5;
	rCharacter.Ship.Sounds.SailsID.z = 0.0;
	rCharacter.Ship.Sounds.SailsID = Play3DSoundComplex("sails_up_" + refBaseShip.Soundtype, fX, fY, fZ, true, false);
	if (sti(rCharacter.Ship.Sounds.SailsID) > 0) { Sound_SetVolume(sti(rCharacter.Ship.Sounds.SailsID), 0.0); }
	rCharacter.Ship.Sounds.SailsMove = 0.0;

	// звук поворота
	rCharacter.Ship.Sounds.TurnID.x = 0.0;
	rCharacter.Ship.Sounds.TurnID.y = 0.0;
	rCharacter.Ship.Sounds.TurnID.z = 0.0;
	rCharacter.Ship.Sounds.TurnID = Play3DSoundComplex("ship_turn_" + refBaseShip.Soundtype, fX, fY, fZ, true, false);
	if (sti(rCharacter.Ship.Sounds.TurnID) > 0) { Sound_SetVolume(sti(rCharacter.Ship.Sounds.TurnID), 0.0); }
	rCharacter.Ship.Sounds.TurnSpeed = 0.05;
}

void Ship_PlaySound3DComplex(ref rCharacter, string sSoundID, float fVolume, float fX, float fY, float fZ)
{
	if (!CheckAttribute(rCharacter, "Ship.Pos.x")) return; // fix
	float x = stf(rCharacter.Ship.Pos.x);
	float y = stf(rCharacter.Ship.Pos.y);
	float z = stf(rCharacter.Ship.Pos.z);

	float aY = stf(rCharacter.Ship.Ang.y);

    if (!CheckAttribute(rCharacter, "Ship.BoxSize.x")) return; // fix габаритов

	float x1 = fX * stf(rCharacter.Ship.BoxSize.x) / 2.0;
	float z1 = fZ * stf(rCharacter.Ship.BoxSize.z) / 2.0;
	RotateAroundY(&x1, &z1, cos(aY), sin(aY));

	int iSoundID = Play3DSoundRet(sSoundID, x + x1, y + fY * stf(rCharacter.Ship.BoxSize.y) / 2.0, z + z1);
	if (iSoundID > 0) { Sound_SetVolume(iSoundID, fVolume); }
}

void Ship_PlaySound3D(ref rCharacter, string sSoundID, float fVolume)
{
	float x = stf(rCharacter.Ship.Pos.x);
	float y = stf(rCharacter.Ship.Pos.y);
	float z = stf(rCharacter.Ship.Pos.z);

	int iSoundID = Play3DSoundRet(sSoundID, x, y, z);
	if (iSoundID > 0) { Sound_SetVolume(iSoundID, fVolume); }
}

void Ship_OnBortReloaded()
{
	aref aCharacter = GetEventData();
	string sBort = GetEventData();

	if (!bSeaLoaded || bAbordageStarted) { return; }  // fix
	aCharacter.seatime.lastfired = GetSeaTime(); // боевая мораль: корабль в перестрелке (перезарядил борт)
	// далее по коду была глупость, тк прерывание шло по всем НПС, звуков таких не было вовсе в ресурсах, те мина была в игре. "Сапер boal"
	if (sti(aCharacter.index) != nMainCharacterIndex) { return; } // fix

	if (!sti(aCharacter.Tmp.BortsReloaded.Event))
	{
		aCharacter.Tmp.BortsReloaded.Event = true;
		PostEvent("Ship_BortReloadEvent", 300, "a", aCharacter);
	}
	if (sBort == "cannonl") { aCharacter.Tmp.BortsReloaded.cannonl = true; }
	if (sBort == "cannonr") { aCharacter.Tmp.BortsReloaded.cannonr = true; }
	if (sBort == "cannonf") { aCharacter.Tmp.BortsReloaded.cannonf = true; }
	if (sBort == "cannonb") { aCharacter.Tmp.BortsReloaded.cannonb = true; }
}

void Ship_BortReloadEvent()
{
    aref aCharacter = GetEventData();

    if (sti(aCharacter.Tmp.BortsReloaded.Event))
    {
		if (CheckAttribute(aCharacter, "Tmp.BortsReloaded.SkipFirst"))
		{
			// ложный доклад при входе в море: пушки заряжаются мгновенно при создании корабля, см. Sea_FirstInit
			DeleteAttribute(aCharacter, "Tmp.BortsReloaded.SkipFirst");
			Ship_ClearBortsReloadedEvent(aCharacter);
			return;
		}
		LetCharge = LetRecharge();
        bool bLeft = false;
        bool bRight = false;
        bool bFront = false;
        bool bBack = false;
        if (CheckAttribute(aCharacter,"Tmp.BortsReloaded.cannonl")) bLeft = sti(aCharacter.Tmp.BortsReloaded.cannonl) == true;
        if (CheckAttribute(aCharacter,"Tmp.BortsReloaded.cannonr")) bRight = sti(aCharacter.Tmp.BortsReloaded.cannonr) == true;
        if (CheckAttribute(aCharacter,"Tmp.BortsReloaded.cannonf")) bFront = sti(aCharacter.Tmp.BortsReloaded.cannonf) == true;
        if (CheckAttribute(aCharacter,"Tmp.BortsReloaded.cannonb")) bBack = sti(aCharacter.Tmp.BortsReloaded.cannonb) == true;

        if (bLeft && bRight && bFront && bBack)
        {
            PlayVoice("interface\"+LanguageGetLanguage()+"_GunReady.wav");
        }
        else
        {
            if (bLeft)  { PlayVoice("interface\"+LanguageGetLanguage()+"_GunReadyL.wav"); }
            if (bRight) { PlayVoice("interface\"+LanguageGetLanguage()+"_GunReadyR.wav"); }
            if (bBack) { PlayVoice("interface\"+LanguageGetLanguage()+"_Gun_FB_Ready.wav"); }
        }

    Ship_ClearBortsReloadedEvent(aCharacter);
	}
}
void Ship_ClearBortsReloadedEvent(aref aCharacter)
{
	aCharacter.Tmp.BortsReloaded.Event = false;
	aCharacter.Tmp.BortsReloaded.cannonl = false;
	aCharacter.Tmp.BortsReloaded.cannonr = false;
	aCharacter.Tmp.BortsReloaded.cannonf = false;
	aCharacter.Tmp.BortsReloaded.cannonb = false;
}

void Ship_ChangeCharge(ref rCharacter, int iNewChargeType)
{
	SendMessage(&AISea, "la", AI_MESSAGE_CANNON_RELOAD, rCharacter);
	ref rGood = GetGoodByType(iNewChargeType);
	rCharacter.Ship.Cannons.Charge = rGood.name;
	rCharacter.Ship.Cannons.Charge.Type = iNewChargeType;

	Cannon_RecalculateParameters(sti(rCharacter.index));
	LetCharge = LetRecharge();

	DeleteAttribute(rCharacter, "Tmp.BortsReloaded.SkipFirst"); // игрок сменил заряд — доклад о перезарядке должен прозвучать
	Ship_ClearBortsReloadedEvent(rCharacter);
	BI_UpdateLoadedProjectiles(); // обновим иконки и текста заряженных снарядов в BattleInterface
}

void Ship_OnCreate()
{
	ref rCharacter = GetCharacter(GetEventData());

	rCharacter.Ship.Speed.z = 0.0;
	rCharacter.Ship.Speed.y = 0.0;
	rCharacter.Ship.Speed.x = 0.0;
}

/*!
 * \brief Возвращает строку иконки состояния корабля
 * \param task текущая .SeaAI.Task корабля
 * \return string строка, описывающая иконку из "\RESOURCE\INI\fonts_xxx.ini"
 */

string getShipIconFromSeaAITask(int task)
{
	string ret = "";
	
	switch (task)
	{
		case AITASK_NONE:
			ret = "";
			break;
		case AITASK_DRIFT:
			ret = "Ţ";
			break;
		case AITASK_ATTACK:
			ret = "Ś";
			break;
		case AITASK_FOLLOW:
			ret = "Ŝ";
			break;
		case AITASK_DEFEND:
			ret = "Ŝ";
			break;
		case AITASK_DEFEND_GROUP:
			ret = "Ŝ";
			break;
		case AITASK_ABORDAGE:
			ret = "ţ";
			break;
		case AITASK_RUNAWAY:
			ret = "ś";
			break;
		case AITASK_MOVE:
			ret = "ś";
			break;
		case AITASK_BRANDER:
			ret = "ŝ";
			break;
	}
	
	return ret;
}

string GetChargedIcon(int type)
{
	switch (type)
	{
		case 0:
			return "Ŕ";
		break;
		case 1:
			return "ŗ";
		break;
		case 2:
			return "Ŗ";
		break;
		case 3:
			return "ŕ";
		break;
	}
	return "";
}

// LEO: имя корабля-ЦЕЛИ данного корабля (бетта-тест выбора цели ИИ). SeaAI.Task.Target = индекс цели (движок обнуляет в "" при её гибели).
string Ship_GetTaskTargetShipName(ref rCharacter)
{
	if (!CheckAttribute(rCharacter, "SeaAI.Task.Target")) return "";
	string sTgt = rCharacter.SeaAI.Task.Target;
	if (sTgt == "") return "";                  // пустая = нет цели (sti("")==0, НЕ -1!)
	int idx = sti(sTgt);
	if (idx < 0) return "";                      // гвардим ДО GetCharacter (&& в Storm не ленив)
	ref rt = GetCharacter(idx);
	string nm = "";
	if (CheckAttribute(rt, "Ship.Name")) nm = rt.Ship.Name;
	if (nm == "" && CheckAttribute(rt, "Ship.Type")) nm = XI_ConvertString(GetShipTypeName(rt));
	return nm;
}

// LEO: показывать ли ИМЯ ЦЕЛИ в подписи действия над кораблём. Бетта — всем; компаньоны — ВСЕГДА (цель свиты видна,
//      с соблюдением опций подписей — весь caption гейтится ShipCapShowActions выше по стеку); не-компаньоны — если
//      корабль полно-осмотрен «Отличной»/«Эксклюзивной» трубой (флаг SpyglassCheckedFull).
bool Ship_ShowActionTarget(ref rCharacter)
{
	if (bBettaTestMode) return true;
	if (IsCompanion(rCharacter)) return true;
	if (CheckAttribute(rCharacter, "SpyglassCheckedFull")) return true;
	return false;
}

// LEO: строка-иконки действия над кораблём (заряд+задача; +имя цели по Ship_ShowActionTarget). Общий билдер
// для ОБОИХ путей подписи (AIShip.c и sea.c) — чтоб иконка AI-тика и обновление подписей совпадали.
string Ship_BuildActionCaption(ref rCharacter, int aiTask, int iAmmoType)
{
	string s = GetChargedIcon(iAmmoType) + " " + getShipIconFromSeaAITask(aiTask);
	if (Ship_ShowActionTarget(rCharacter))
	{
		string sTgt = Ship_GetTaskTargetShipName(rCharacter);
		if (sTgt != "") s = s + " (" + sTgt + ")";
	}
	return s;
}

// ============================================================================
//   ТАКТИЧЕСКИЙ ИИ: ГЛАВНАЯ ЛОГИКА ПОВЕДЕНИЯ ВРАЖЕСКИХ КОРАБЛЕЙ
//   (валидация целей -> перемирие/сдача -> боезапас -> дерево атака/бегство/абордаж)
// ============================================================================

// LEO: нейтраль к ГГ И ко всей свите (компаньонам). Иначе после сдачи/перемирия компаньоны бьют нейтральную цель.
void Ship_MakeNeutralToPlayerFleet(int iShipIdx)
{
	if (iShipIdx < 0) return;
	SetCharacterRelationBoth(iShipIdx, GetMainCharacterIndex(), RELATION_NEUTRAL);
	ref pc = GetMainCharacter();
	int cn;
	for (int i = 1; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pc, i);
		if (cn < 0) continue;
		SetCharacterRelationBoth(iShipIdx, cn, RELATION_NEUTRAL);
	}
}

// Хелпер Ship_CheckSituation: ПЕРЕМИРИЕ Speak (SpeakTruce). true = обслужен целиком, нужен ранний return.
// Нейтралитет данными; дрейф со спущенными парусами, пока нет ЧУЖИХ врагов (стычки наций/пиратов -> паруса
// вверх, дерётся). Задачу/паруса трогаем ТОЛЬКО при смене режима/цели, иначе тик воюет с ИИ за паруса (дёрганье).
bool Ship_CS_HandleSpeakTruce(ref rCharacter)
{
	if (!CheckAttribute(rCharacter, "SpeakTruce")) return false;

	Ship_MakeNeutralToPlayerFleet(sti(rCharacter.index)); // LEO: нейтраль и к свите, иначе компаньоны бьют перемирную цель
	// ближайшего врага ищем скриптом (Speak_FindNearestFoe): движок даёт лишь дистанцию без индекса; труп не в счёт
	int iTruceFoe = Speak_FindNearestFoe(rCharacter, 1500.0);
	bool bTruceFoeNear = false;
	if (iTruceFoe >= 0) bTruceFoeNear = true;

	string sTruceMode = "drift";
	if (bTruceFoeNear) sTruceMode = "f" + iTruceFoe;
	string sTruceModeOld = "";
	if (CheckAttribute(rCharacter, "SpeakTruce.mode")) sTruceModeOld = rCharacter.SpeakTruce.mode;
	if (sTruceMode != sTruceModeOld)
	{
		rCharacter.SpeakTruce.mode = sTruceMode;
		if (bTruceFoeNear)
		{
			Ship_SetSailState(sti(rCharacter.index), 1.0);
			Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iTruceFoe);
		}
		else
		{
			Ship_SetTaskDrift(SECONDARY_TASK, sti(rCharacter.index));
			Ship_SetSailState(sti(rCharacter.index), 0.0);
		}
	}
	return true;
}

// Хелпер Ship_CheckSituation: СДАВШИЙСЯ (белый флаг Ship_Surrender). true = обслужен (ранний return):
// нейтралитет+дрейф (иначе блок нац-агрессии заагрит заново), один раз (surr.drift). Захват не авто — игрок сам.
bool Ship_CS_HandleSurrendered(ref rCharacter)
{
	if (!CheckAttribute(rCharacter, "surrendered")) return false;

	Ship_MakeNeutralToPlayerFleet(sti(rCharacter.index)); // LEO: нейтраль и к свите, иначе компаньоны бьют сдавшуюся цель
	if (!CheckAttribute(rCharacter, "surr.drift"))
	{
		rCharacter.surr.drift = true;
		Ship_SetTaskDrift(PRIMARY_TASK, sti(rCharacter.index));
		Ship_SetTaskDrift(SECONDARY_TASK, sti(rCharacter.index));
		Ship_SetSailState(sti(rCharacter.index), 0.0);
	}
	return true;
}

// Хелпер Ship_CheckSituation: иконка действия ИИ (заряд+задача) для компаньонов/бетта-теста; выкл. показ -> чистит текст.
void Ship_CS_ShowActionIcon(ref rCharacter, ref rShipObject, int aiTask, int iCurrentBallType, bool bIsCompanion)
{
	// LEO: подпись заряд+задача(+цель) — компаньонам, в бетте, ИЛИ не-компаньону, полно-осмотренному «Отличной»/«Эксклюзивной» трубой.
	if (!bIsCompanion && !bBettaTestMode && !CheckAttribute(rCharacter, "SpyglassCheckedFull")) return;
	if (sti(InterfaceStates.ShipCapShowActions) == 0)
	{
		SendMessage(rCharacter, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, "");
	}
	else
	{
		SendMessage(rShipObject, "ls", MSG_SHIP_SET_CAPTURE_TEXT3, Ship_BuildActionCaption(rCharacter, aiTask, iCurrentBallType));
	}
}

// LEO: параметры сторожевиков форта (тюнинг без пересборки движка — правятся здесь, в скрипте).
#define GUARD_ENGAGE_DIST  1500.0  // радиус, в котором сторож САМ атакует врага у порта (не дожидаясь выстрела)
#define GUARD_LEASH_DIST   2000.0  // уплыл дальше форта -> бросает погоню и возвращается в оборону (поводок > радиуса атаки = буфер, чтоб не йо-йо-ил)

// Хелпер Ship_CheckSituation: ИИ сторожа форта (fortDefender). Всегда обслуживает целиком (true -> ранний return).
//  1) ПОВОДОК: уплыл далеко от форта -> назад в оборону; 2) РЕАКЦИЯ: стрелявший по форту/по нам -> атака группой;
//  3) ПРОАКТИВНО: враг нации (в т.ч. ГГ) в радиусе -> атакуем сами (фикс "памятников"). По своим/коменданту — ничего.
bool Ship_CS_DefenderAI(ref rCharacter, string sGroupID)
{
	ref rTargetedChar = GetFortCommander(rCharacter.IslandShips);
	int meIdx = sti(rCharacter.index);

	// 1) ПОВОДОК: не бросаем порт — уплыл за GUARD_LEASH_DIST от форта -> назад в оборону коменданта
	if (bIsFortAtIsland && GetDistance2D(stf(rCharacter.Ship.Pos.x), stf(rCharacter.Ship.Pos.z), fFort_x, fFort_z) > GUARD_LEASH_DIST)
	{
		Group_UnlockTask(sGroupID); // снять лок погони, чтоб оборона применилась
		Ship_SetTaskDefend(SECONDARY_TASK, meIdx, GetCharacterIndex(rTargetedChar.id));
		return true;
	}

	// 2) кто стрелял по форту (приоритет) или по самому сторожу
	int attackChar;
	if (CheckAttribute(rTargetedChar, "Ship.LastBallCharacter")) attackChar = sti(rTargetedChar.Ship.LastBallCharacter);
	else attackChar = sti(rCharacter.Ship.LastBallCharacter);
	// стрелявший мог УЖЕ погибнуть: LastBallCharacter ФОРТА движок не сбрасывает (у корабля - сбрасывает) ->
	// иначе сторож залипал на атаке ТРУПА (лок задачи: паруса вниз, стоит, лишь пушки бьют). Гвард <0 отдельно (Storm).
	if (attackChar != -1)
	{
		if (LAi_IsDead(GetCharacter(attackChar))) attackChar = -1;
	}

	// 3) ПРОАКТИВНО: никто не стрелял -> сами ищем ближайшего врага у порта
	if (attackChar == -1)
	{
		// ГГ проверяем отдельно (Speak_FindNearestFoe его исключает). Сторож СЛЕДУЕТ ЗА ФОРТОМ: берём отношение
		// коменданта форта к игроку (GetRelation), а НЕ сырое нац-отношение. Форт уже учитывает распознавание
		// флага, скрытность, ЛИЦЕНЗИЮ и ЛЬГОТУ ФРАХТА (FortAgent grace) — форт пропускает -> и сторож пропускает;
		// форт распознал врага (напр. после твоего выстрела) -> и сторож нападает. Заодно фикс "один враг, остальные нет".
		if (bIsFortAtIsland && GetRelation(iFortCommander, GetMainCharacterIndex()) == RELATION_ENEMY && Ship_GetDistance2D(rCharacter, GetMainCharacter()) < GUARD_ENGAGE_DIST)
		{
			attackChar = GetMainCharacterIndex();
		}
		else
		{
			attackChar = Speak_FindNearestFoe(rCharacter, GUARD_ENGAGE_DIST); // третьи стороны — враги нации
		}
	}

	// врагов нет -> вернуться на станцию к форту и держать оборону (иначе после погони дрейфуют где остановились,
	// спускают паруса и стоят). Пере-задаём ТОЛЬКО при смене задачи (task != DEFEND), чтоб не дёргать путь каждый тик.
	if (attackChar == -1)
	{
		if (sti(rCharacter.SeaAI.Task) != AITASK_DEFEND)
		{
			Group_UnlockTask(sGroupID); // снять лок прошлой атаки
			Ship_SetTaskDefend(SECONDARY_TASK, meIdx, GetCharacterIndex(rTargetedChar.id));
		}
		return true;
	}
	if (CheckAttribute(Characters[attackChar], "IslandShips") || attackChar == sti(rTargetedChar.index)) return true; // не бьём своих сторожей/форт

	// LEO: держим ТЕКУЩУЮ цель, пока она жива/враг/в пределах поводка -> анти-мерцание (attackChar = «кто бил форт» скачет по осадникам).
	int iGuardTgt = -1;
	if (CheckAttribute(rCharacter, "GuardCurTgt")) iGuardTgt = sti(rCharacter.GuardCurTgt);
	if (iGuardTgt >= 0 && iGuardTgt != attackChar)
	{
		if (!LAi_IsDead(GetCharacter(iGuardTgt)))
		{
			if (GetRelation(meIdx, iGuardTgt) == RELATION_ENEMY)
			{
				if (Ship_GetDistance2D(rCharacter, GetCharacter(iGuardTgt)) < GUARD_LEASH_DIST) attackChar = iGuardTgt;
			}
		}
	}
	// LEO: ФИКС — атака через Ship-level SECONDARY_TASK (как DEFEND выше). Group_SetTaskAttack (уровень ГРУППЫ) НЕ перебивал
	//   СЕКУНДАРНУЮ DEFEND сторожа -> цель верна, но стоял task=3. Ставим ТОЛЬКО при смене цели / выходе из ATTACK.
	if (iGuardTgt != attackChar || sti(rCharacter.SeaAI.Task) != AITASK_ATTACK)
	{
		rCharacter.GuardCurTgt = attackChar;
		Group_SetEnemyToCharacter(sGroupID, attackChar);
		SetCharacterRelationBoth(meIdx, attackChar, RELATION_ENEMY);
		Group_UnlockTask(sGroupID);
		Ship_SetTaskAttack(SECONDARY_TASK, meIdx, attackChar);
		DoQuestCheckDelay("NationUpdate", 0.5);
	}
	return true;
}

// Хелпер Ship_CheckSituation: тактика НЕ-компаньона по атакующему (когда нет SinkTenPercent). Назначает вражду
// и задачу атака/бегство/брандер по HP/парусам/команде и соотношению сил групп; форт не камикадзит. Вынесено 1:1.
// ===== LEO: Фаза 1 — поведение по типу корабля (Быстрый/Тяжёлый/Торговец) =====
// Тип задаёт ДИСТАНЦИЮ УДЕРЖАНИЯ: быстрый кайтит у края дальности, тяжёлый сближается, торговец держит дистанцию. Курс — движок.
#define AIBEH_BALANCED  0
#define AIBEH_FAST      1
#define AIBEH_HEAVY     2
#define AIBEH_TRADER    3
// классификация (тунабл): «быстрый» — по СКОРОСТИ (не классу); «вёрткий малый» — средняя скорость+поворотливость; «тяжёлый» — по классу
#define AIBEH_FAST_MINSPEED   16.5   // рейтинг скорости >= -> быстрый (кайтер), любой класс. По БАЗЕ корпуса (не по факт. скорости с бонусами)
#define AIBEH_FAST_ALTSPEED   12.0   // ИЛИ: скорость >= это И
#define AIBEH_FAST_MINTURN    18.0   //      поворотливость >= это (вёрткий фелук/тартана/шлюп)
#define AIBEH_HEAVY_MAXCLASS  2      // класс <= -> тяжёлый (линейные; макс скорость ~14.5, не быстрые)
// доли дальности пушки, на которых держится корабль (тунабл — правим без пересборки)
#define AIBEH_FAST_HOLD       0.95   // LEO: здоров: кайт у САМОГО края дальности ядер (юзер: почти все кайтят далеко)
#define AIBEH_FAST_HOLD_HURT  1.00   // потрёпан: у предела дальности ядер
#define AIBEH_FAST_HOLD_FLEE  1.20   // сильно получил: отрыв ЗА дальность (уже не стреляем — бежим)
#define AIBEH_FAST_HP_HURT    75.0
#define AIBEH_FAST_HP_FLEE    45.0
#define AIBEH_HEAVY_HOLD      0.75   // LEO: тяж ТОЖЕ КАЙТИТ (юзер: без сближения под бомбы), просто чуть ближе быстрых
#define AIBEH_TRADER_HOLD     1.00   // торговец: держит МАКС дистанцию
#define AIBEH_TRADER_FLEE_HP  50.0   // торговец: HP% <= этого -> драп (не геройствует), тунабл
#define AIBEH_RETARGET_MAXDIST 3000.0 // переназначение цели: НЕ брать врага дальше этого (иначе гонятся за кораблём в порту за тыщи), тунабл
#define AIBEH_ABORD_CLOSE 300.0 // ближний бой: потолок AttackDistance при сближении (сцепку/спид-гейт больше НЕ решает - см. AIBEH_ABORD_MAXDIST/NOCATCH), тунабл
#define AIBEH_ABORD_GRACE 250.0 // абордаж: грейс-полоса латча — пока цель ближе, анти-джиттер держит намерение; дальше требуем ЧЕСТНУЮ (крейсерскую) догоняемость, иначе реальный отрыв -> латч падает (враг не тащится вечно за быстрым игроком), тунабл. Было 500 - ИИ пёр на абордаж через полкилометра под залпы, не стреляя
#define AIBEH_ABORD_MAXDIST 200.0 // абордаж: ЖЁСТКИЙ потолок дистанции ВЫДАЧИ таска. Дальше — обычный бой (ATTACK, борт-пассы): сблизится под огнём, и уже вблизи решит сцепку. Тунабл
#define AIBEH_ABORD_NOCATCH 80.0  // абордаж: ближе этого спид-гейт НЕ требуется (уже вплотную, дело за кошками; лечит скачки MaxSpeedZ по курсу к ветру). Тунабл
#define AIBEH_BOARD_EXIT_FRAC 0.90 // абордаж: exit-порог латча = fBoardGate×это (1.2×0.90=1.08 > соотношения ~1.0 РАВНОГО боя -> в AI-vs-AI латч ПАДАЕТ, не «все на абордаж с ходу»). Было 0.75 (×1.2=0.9<1.0 -> вечный латч). Тунабл
// Автономный кайт компаньона — ТОЛЬКО без прямого приказа (флаг CompanionAutoEngage, ставится re-decide после абордажа/гибели цели).
#define COMPANION_AUTOKITE     1     // 0 = выключить автономный кайт целиком
#define COMPANION_SWARM_RADIUS 700.0 // враг в этом радиусе считается "рядом" (оценка толпы), тунабл
#define COMPANION_SWARM_N      3     // столько врагов рядом -> откат от толпы, тунабл
#define COMPANION_SWARM_CLEAR  1     // врагов рядом стало <= этого -> снова в атаку с дальней, тунабл
// досягаемость снарядов (доля дальности пушки): ядра дальше всех (1.0), картечь ближе всех. Держимся не дальше этого — иначе не достаём.
#define AMMO_REACH_KNIPPEL 0.8
#define AMMO_REACH_BOMBS   0.6
#define AMMO_REACH_GRAPE   0.3
// LEO: запас над минимумом команды цели, ниже которого картечь бесполезна (репорт игрока: ИИ
//   продолжал бить картечью по кораблю с уже выбитой командой).
#define GRAPE_CREW_MARGIN  1.2
#define AIBEH_BALANCED_HOLD 0.85 // LEO: балансный кайтит на дальности ядер (юзер: почти все кайтят далеко)

// классификатор поведения ИИ по типу корпуса/роли. Оверрайд атрибутом rCharacter.AIBehavior (квесты/точечно).
int Ship_GetAIBehavior(ref rCharacter)
{
	if (CheckAttribute(rCharacter, "AIBehavior")) return sti(rCharacter.AIBehavior);         // явный оверрайд (квесты/точечно)
	if (CheckAttribute(rCharacter, "AIBehaviorAuto")) return sti(rCharacter.AIBehaviorAuto); // кэш на бой (сброс при входе в море) — стабильно, не прыгает от урона
	int beh = AIBEH_BALANCED;
	ref rShip = GetRealShip(sti(rCharacter.Ship.Type));
	// пуре-торговец (драп) = режим trade И НЕ военный корпус. Универсал (merchant&&war) воюет как боевой.
	bool bPureTrader = false;
	if (CheckAttribute(rCharacter, "Ship.Mode"))
	{
		if (rCharacter.Ship.Mode == "trade")
		{
			bPureTrader = true;
			if (CheckAttribute(rShip, "Type.War")) { if (rShip.Type.War == true) bPureTrader = false; } // военный/универсал корпус — не драпает как торгаш
		}
	}
	if (bPureTrader)
	{
		beh = AIBEH_TRADER;
	}
	else
	{
		// по РЕЙТИНГУ корпуса, а НЕ по факт. скорости с бонусами (бонусы задирали ход -> в кайтеры лезли тихоходы и линейники 2 кл.)
		float fSpeed = stf(rShip.SpeedRate);
		float fTurn  = stf(rShip.TurnRate);
		if (fSpeed >= AIBEH_FAST_MINSPEED) beh = AIBEH_FAST;                                                        // быстрый по рейтингу (любой класс)
		if (beh == AIBEH_BALANCED && fSpeed >= AIBEH_FAST_ALTSPEED && fTurn >= AIBEH_FAST_MINTURN) beh = AIBEH_FAST; // вёрткий малый (фелук/тартана)
		if (beh == AIBEH_BALANCED && sti(rShip.Class) <= AIBEH_HEAVY_MAXCLASS) beh = AIBEH_HEAVY;                   // крупный линейный -> тяжёлый (сближается, бомбы)
	}
	rCharacter.AIBehaviorAuto = beh; // кэшируем на бой
	return beh;
}

// доля дальности пушки, на которой держится корабль (управляет кайтом/сближением). Зависит от ВЫБРАННОГО СНАРЯДА (fAmmoMod) — тянет дистанцию под снаряд.
float Ship_AIHoldFraction(ref rCharacter, float fAmmoMod)
{
	// LEO: дистанция удержания связана с выбранным снарядом (fAmmoMod): ядра далеко, книппель/бомбы/картечь ближе; разброс
	//   декоррелирует флот. Фикс. далёкий кайт по РОЛИ -> все вечно на ядрах. Исключение: потрёпанный БЫСТРЫЙ открывает дистанцию.
	if (Ship_GetAIBehavior(rCharacter) == AIBEH_FAST)
	{
		if (GetHullPercent(rCharacter) < AIBEH_FAST_HP_FLEE) return AIBEH_FAST_HOLD_FLEE;
	}
	return fAmmoMod / (1.4 + frandSmall(3.0));
}

// LEO: ближайший ВРАГ среди судов боя (кроме себя/мёртвых/не-врагов/форта). Индекс или -1; для переназначения (целей может быть много).
int Ship_FindNearestEnemy(ref rCharacter)
{
	int meIdx = sti(rCharacter.index);
	int best = -1;
	float bestDist = 1000000000.0;
	int k, idx;
	for (k = 0; k < iNumShips; k++)
	{
		idx = Ships[k];
		if (idx < 0) continue;
		if (idx == meIdx) continue;
		if (idx == iFortCommander) continue;            // на форт не камикадзимся
		if (LAi_IsDead(&characters[idx])) continue;
		if (GetRelation(meIdx, idx) != RELATION_ENEMY) continue;
		float d = Ship_GetDistance2DRel(rCharacter, &characters[idx]); // квадрат дистанции
		if (d > (AIBEH_RETARGET_MAXDIST * AIBEH_RETARGET_MAXDIST)) continue; // слишком далеко — не наш бой (корабль в порту за тыщи)
		if (d < bestDist) { bestDist = d; best = idx; }
	}
	return best;
}

// ═══ LEO: ТАКТИКА ЭСКАДРЫ — разрыв строя на бой (3a) + поза группы по силам (3b). Всё тунабл. ═══
#define SQUAD_ENGAGE_DIST      450.0  // ближайший враг ближе -> эскадра РВЁТ строй в бой (латч на бой), ~1.5x abord-close(300)
#define SQUAD_TAC_INTERVAL     5      // пересчёт тактики группы раз в N сек (кэш на группе, не на кадр)
#define SQUAD_TAC_ENEMY_RADIUS 1500.0 // радиус учёта вражьей СИЛЫ для отношения сил (половина retarget-max 3000)
#define SQUAD_FLEE_SAFE_DIST   1800.0 // LEO: раз решив бежать (FLEE), держим FLEE, пока враг ближе этого -> убегаем ДАЛЬШЕ радиуса скана, не мигаем на границе (анти-фликер RUNAWAY<->ATTACK). 3000 держал вечный драп -> 1800 (=radius 1500 + гистерезис 300)
#define SQUAD_FLEE_RATIO       0.30   // сила_своя / сила_врага < этого -> поза БЕГИ. Было 0.45 (беги при 2.2x) -> слишком часто бежали; 0.30 = беги лишь при 3.3x перевесе (юзер: много побегов)
#define SQUAD_KITE_RATIO       0.80   // < этого -> КАЙТ (обстрел с дистанции, без абордажа)
#define SQUAD_BOARD_RATIO      2.20   // > этого -> АБОРДАЖ-РАШ (ниже порог сцепки)
#define SQUAD_POS_ENGAGE 0
#define SQUAD_POS_KITE   1
#define SQUAD_POS_FLEE   2
#define SQUAD_POS_BOARD  3
#define AGGRO_COMMIT_SEC 10    // LEO: держать ту же цель N сек, не флипаться на каждого последнего стрелявшего (иначе трэш снаряда/перезарядки при кучном огне). Юзер 04.07: 5 мало -> 10
#define AGGRO_MIN_HP     15    // LEO: при позе АБОРДАЖ (группа сильно сильнее) корабль НЕ драпает индивидуально, пока HP выше этого % (иначе доминирующая эскадра разбегается по одному)

#define COMBATPOWER_CANNON_W 20.0 // LEO: вес одной ИНТАКТНОЙ пушки в «мощи» корабля (тунабл). HP сотни-тыщи, команда×10 сотни, пушки×20 сотни — сопоставимо.

// LEO: «мощь» для отношения сил = корпус + команда×10 + пушки×вес (интактные). Без пушек военный недооценивался,
// а команда ГГ раздувалась -> ложные побеги эскадр. Значения ТЕКУЩИЕ (урон снижает HP и число пушек).
float Ship_CombatPower(ref rChar)
{
	float fHP = 0.0;
	if (CheckAttribute(rChar, "Ship.HP")) fHP = stf(rChar.Ship.HP);
	return fHP + makefloat(GetCrewQuantity(rChar)) * 10.0 + makefloat(GetCannonsNum(rChar)) * COMBATPOWER_CANNON_W;
}

// LEO: макс.скорость С УЧЁТОМ ветра/парусов/урона (Ship.MaxSpeedZ, пишет Sea_ApplyMaxSpeedZ). Фолбэк 10, если не посчитан.
float Ship_SquadMaxSpeed(ref rChar)
{
	if (CheckAttribute(rChar, "Ship.MaxSpeedZ")) return stf(rChar.Ship.MaxSpeedZ);
	return 10.0;
}

// LEO: макс.скорость на ЗАДАННОМ курсе к ветру (fWindDot=cos, -1..1). MaxSpeedZ скорректирована на ТЕКУЩИЙ курс, потому
// берём КРЕЙСЕРСКУЮ базу и гоним через Sea_ApplyMaxSpeedZ. Реюз формулы врем. подменой базы в ОДНОМ вызове (без yield).
float Ship_SpeedAtHeading(ref rChar, float fWindDot)
{
	if (!CheckAttribute(rChar, "Ship.CruiseSpeedZ")) return Sea_ApplyMaxSpeedZ(rChar, fWindDot); // ещё не посчитана -> фолбэк
	float fSaved = 0.0;
	if (CheckAttribute(rChar, "Ship.MaxSpeedZ")) fSaved = stf(rChar.Ship.MaxSpeedZ);
	rChar.Ship.MaxSpeedZ = stf(rChar.Ship.CruiseSpeedZ); // база = крейсерская
	float fRes = Sea_ApplyMaxSpeedZ(rChar, fWindDot);    // та же формула ветра/бейдевинда, но уже на нужном курсе
	rChar.Ship.MaxSpeedZ = fSaved;                        // восстановить курс-скорректированную
	return fRes;
}

// LEO: ДОГОНЮ ЛИ цель. МОЯ скорость, встань я НА КУРС ЦЕЛИ, против её MaxSpeedZ. Запас 3%. Учитывает всё
// (команда/груз/урон/навык/апгрейды + курс к ветру/бейдевинд обоих) — чтоб не было глупых погонь.
bool Ship_CanCatch(ref rChar, int iTgt)
{
	if (iTgt < 0) return false;
	float fTgtAng = 0.0;
	if (CheckAttribute(&characters[iTgt], "Ship.Ang.y")) fTgtAng = stf(characters[iTgt].Ship.Ang.y);
	float fWindDotTgt = GetDotProduct(Whr_GetWindAngle(), fTgtAng); // курс ЦЕЛИ к ветру
	float fTgtSpeed = Ship_SquadMaxSpeed(&characters[iTgt]);        // цель на своём курсе -> MaxSpeedZ = её скорость тут
	float fMyAtTgt  = Ship_SpeedAtHeading(rChar, fWindDotTgt);      // МОЯ скорость, встав на курс цели
	return (fMyAtTgt > fTgtSpeed * 1.03);
}

// LEO: ближайший ДОСЯГАЕМЫЙ враг-НЕ-ГГ: стоячий/наступающий — всегда, беглец — лишь если догоним (Ship_CanCatch); иначе пропуск.
int Ship_NearestReachableEnemy(ref rChar)
{
	int iMe = sti(rChar.index);
	int iMain = GetMainCharacterIndex();
	float fBest = 1000000000.0;
	int iBest = -1;
	int sk;
	int e;
	for (sk = 0; sk < iNumShips; sk++)
	{
		e = Ships[sk];
		if (e < 0) continue;
		if (e == iMe) continue;
		if (e == iMain) continue;
		if (e == iFortCommander) continue;              // LEO: форт НЕ берём авто-целью (как в Ship_FindNearestEnemy) — осадная свита бьёт сторожей, а форт добивает через Ship_CS_SiegeAttack (иначе цель мигала форт<->сторож)
		if (LAi_IsDead(&characters[e])) continue;
		if (GetRelation(iMe, e) != RELATION_ENEMY) continue;
		float fd = Ship_GetDistance2D(rChar, &characters[e]);
		if (fd >= fBest) continue;
		bool bFleeing = false;
		if (CheckAttribute(&characters[e], "SeaAI.Task")) { if (sti(characters[e].SeaAI.Task) == AITASK_RUNAWAY) bFleeing = true; }
		if (bFleeing) { if (!Ship_CanCatch(rChar, e)) continue; } // беглеца, кого не догоню — не беру
		fBest = fd;
		iBest = e;
	}
	return iBest;
}

// LEO: цель ПОГОНИ для корабля эскадры: гнать лишь догоняемых и НЕ скопом. Собираем беглецов (RUNAWAY) в радиусе, кого Я
// быстрее; раздаём по слоту (мой слот % число целей) -> разные корабли гонят РАЗНЫХ. Никого не догнать -> -1 (дерусь обычно).
int Ship_Squad_PursuitTarget(ref rCharacter, string sGroupID)
{
	int iMe = sti(rCharacter.index);
	// 1) собрать догоняемых беглецов (RUNAWAY, в радиусе, кого догоним) за ОДИН проход — раньше фильтр и дорогой Ship_CanCatch считались дважды (счётчик+выбор)
	int aFlee[MAX_SHIPS_ON_SEA];
	int nFlee = 0;
	int sk;
	int e;
	for (sk = 0; sk < iNumShips; sk++)
	{
		e = Ships[sk];
		if (e < 0) continue;
		if (e == iMe) continue;
		if (LAi_IsDead(&characters[e])) continue;
		if (GetRelation(iMe, e) != RELATION_ENEMY) continue;
		if (!CheckAttribute(&characters[e], "SeaAI.Task")) continue;
		if (sti(characters[e].SeaAI.Task) != AITASK_RUNAWAY) continue;
		if (Ship_GetDistance2D(rCharacter, &characters[e]) > SQUAD_TAC_ENEMY_RADIUS) continue;
		if (!Ship_CanCatch(rCharacter, e)) continue; // не быстрее -> не догоню -> пропуск
		aFlee[nFlee] = e;
		nFlee++;
	}
	if (nFlee <= 0) return -1;
	// 2) мой порядковый номер среди живых членов группы -> распараллеливание целей
	ref rGrp = Group_GetGroupByID(sGroupID);
	int iNum = Group_GetCharactersNumR(rGrp);
	int iSlot = 0;
	int gi;
	int m;
	for (gi = 0; gi < iNum; gi++)
	{
		m = Group_GetCharacterIndexR(rGrp, gi);
		if (m < 0) continue;
		if (LAi_IsDead(&characters[m])) continue;
		if (m == iMe) break;
		iSlot++;
	}
	return aFlee[iSlot % nFlee];
}

// LEO: поза группы корабля (0 engage / 1 kite / 2 flee / 3 board). Пишет Ship_Squad_UpdateGroupTactics.
int Ship_Squad_Posture(ref rChar)
{
	string sg = Ship_GetGroupID(rChar);
	if (sg == "" || sg == PLAYER_GROUP) return SQUAD_POS_ENGAGE;
	ref rg = Group_GetGroupByID(sg);
	if (CheckAttribute(rg, "Posture")) return sti(rg.Posture);
	return SQUAD_POS_ENGAGE;
}

// LEO: пересчёт тактики группы РАЗ в SQUAD_TAC_INTERVAL сек (кэш rGrp.TacTime). Считает силу своей группы + силу/дистанцию
// врагов; ставит ЛАТЧ боя (Engaged) и ПОЗУ (Posture). Зовётся каждым НЕ-компаньоном (первый считает, прочие — кэш). Пираты — без позы.
void Ship_Squad_UpdateGroupTactics(string sGroupID)
{
	if (sGroupID == "" || sGroupID == PLAYER_GROUP) return;
	ref rGrp = Group_GetGroupByID(sGroupID);
	int iNow = GetSeaTime();
	if (CheckAttribute(rGrp, "TacTime")) { if (iNow - sti(rGrp.TacTime) < SQUAD_TAC_INTERVAL) return; } // кэш на группу
	rGrp.TacTime = iNow;

	// LEO: запоминаем БАЗОВЫЙ таск группы ОДИН раз (конвой/беглец = RUNAWAY, агрессор = ATTACK) — ПРИРОДА группы. На «сдаче
	//   погони» вернём её: конвой уходит своей дорогой, агрессор остаётся в ATTACK (иначе сильные агрессоры удирали от быстрых).
	if (!CheckAttribute(rGrp, "BaseTask") && CheckAttribute(rGrp, "Task")) rGrp.BaseTask = sti(rGrp.Task);

	int iCmd = Group_GetGroupCommanderIndex(sGroupID);
	if (iCmd < 0) return;
	ref rCmd = GetCharacter(iCmd);
	if (CheckAttribute(rCmd, "nation")) { if (sti(rCmd.nation) == PIRATE) return; } // пираты — без позы (вразнобой)

	// сила своей группы + самый медленный ведомый (для полу-парусов флагмана) + был ли обстрелян кто-то (для ответа)
	float fMyPow = 0.0;
	float fMinFollMax = 1000000.0; // мин. макс.скорость среди ВЕДОМЫХ (не командир) -> под неё капим ход флагмана
	bool bAnyShot = false;         // по кому-то из эскадры уже стреляли -> вся группа выходит из строя и отвечает
	int gn = Group_GetCharactersNumR(rGrp);
	int gi;
	int gidx;
	for (gi = 0; gi < gn; gi++)
	{
		gidx = Group_GetCharacterIndexR(rGrp, gi);
		if (gidx < 0) continue;
		if (LAi_IsDead(&characters[gidx])) continue;
		fMyPow += Ship_CombatPower(&characters[gidx]);
		if (gidx != iCmd)
		{
			float fSpd = Ship_SquadMaxSpeed(&characters[gidx]);
			if (fSpd < fMinFollMax) fMinFollMax = fSpd;
		}
		if (!bAnyShot)
		{
			if (CheckAttribute(&characters[gidx], "Ship.LastBallCharacter")) { if (sti(characters[gidx].Ship.LastBallCharacter) >= 0) bAnyShot = true; }
		}
	}

	// враги: сила рядом (в радиусе) + дистанция до ближайшего. Глобальный список активных Ships[]/iNumShips (как в Ship_FindNearestEnemy).
	float fEnPow = 0.0;
	float fNearest = 1000000000.0;
	int sk;
	int eidx;
	int iNearE = -1;
	for (sk = 0; sk < iNumShips; sk++)
	{
		eidx = Ships[sk];
		if (eidx < 0) continue;
		if (eidx == iCmd) continue;
		if (LAi_IsDead(&characters[eidx])) continue;
		if (GetRelation(iCmd, eidx) != RELATION_ENEMY) continue;
		float fdd = Ship_GetDistance2D(rCmd, &characters[eidx]);
		if (fdd < fNearest) { fNearest = fdd; iNearE = eidx; }
		if (fdd < SQUAD_TAC_ENEMY_RADIUS) fEnPow += Ship_CombatPower(&characters[eidx]);
	}

	// 3a: ЛАТЧ боя — распускаем строй в этом бою НАВСЕГДА (весь). Триггеры:
	bool bEngaged = false;
	if (CheckAttribute(rGrp, "Engaged")) { if (sti(rGrp.Engaged) == 1) bEngaged = true; }
	// (1) КОНТАКТ: ближайший враг ближе порога боя.
	if (!bEngaged && fNearest < SQUAD_ENGAGE_DIST) { rGrp.Engaged = 1; bEngaged = true; }
	// (2) ОТВЕТ: по кому-то из эскадры уже стреляли -> вся группа выходит из строя и ОТВЕЧАЕТ обычной атакой (иначе игрок
	//     безнаказанно расстреливает строй; юзер 05.07: «могу безопасно расстреливать корабли формации, если не отвечают»).
	if (!bEngaged && bAnyShot) { rGrp.Engaged = 1; bEngaged = true; }

	// ФЛАГ ПОГОНИ: ближайший враг ДРАПАЕТ (RUNAWAY) в радиусе. Сам выход на погоню — ИНДИВИДУАЛЬНЫЙ пел-офф (быстрые
	//     ведомые уходят из строя за беглецами, распараллелено; медленные держат строй — НЕ скопом). Флаг гейтит дорогой
	//     скан Ship_Squad_PursuitTarget, чтобы не гонять его каждый кадр, когда беглецов нет.
	int bHasFlee = 0;
	if (iNearE >= 0 && fNearest < SQUAD_TAC_ENEMY_RADIUS)
	{
		if (CheckAttribute(&characters[iNearE], "SeaAI.Task")) { if (sti(characters[iNearE].SeaAI.Task) == AITASK_RUNAWAY) bHasFlee = 1; }
	}
	rGrp.HasFleer = bHasFlee;
	// LEO: есть ли у группы враг ещё В БОЮ. «Сдача боя» — когда врага нет: всех положили, либо остались лишь ДАЛЁКИЕ
	//   недогоняемые беглецы. Близкий враг, далёкий НЕ-беглец и догоняемый беглец — «в бою». Иначе агрессор либо сдавался,
	//   не догнав быстрого, либо ЗАВИСАЛ в ATTACK на сбежавших недосягаемых.
	rGrp.HasEnemy = 0;
	if (iNearE >= 0)
	{
		if (fNearest < SQUAD_TAC_ENEMY_RADIUS) rGrp.HasEnemy = 1; // близкий враг — бой идёт
		else
		{
			bool bFarRun = false;
			if (CheckAttribute(&characters[iNearE], "SeaAI.Task")) { if (sti(characters[iNearE].SeaAI.Task) == AITASK_RUNAWAY) bFarRun = true; }
			if (!bFarRun) rGrp.HasEnemy = 1;                         // далёкий, но НЕ беглец (стоит/наступает) — идём к нему
			else if (Ship_CanCatch(rCmd, iNearE)) rGrp.HasEnemy = 1; // беглец за радиусом, но догоним — гонимся
			// беглец за радиусом и НЕ догнать -> HasEnemy=0 -> бой окончен -> C1 реформирует (свита DEFEND, флагман RUNAWAY)
		}
	}

	// 3b: ПОЗА по отношению сил. БЕГИ доступна и НА ПОДХОДЕ: слабая эскадра, заметив сильного врага в радиусе, СРАЗУ уходит
	//     в обход, а не прёт на убой. Прочие позы (кайт/абордаж) — лишь когда бой начался (bEngaged): на дистанции рашить нечего.
	int iPosture = SQUAD_POS_ENGAGE;
	if (fEnPow > 0.5)
	{
		float fRat = fMyPow / (fEnPow + 0.0001);
		if (fRat < SQUAD_FLEE_RATIO) iPosture = SQUAD_POS_FLEE;
		else if (bEngaged)
		{
			if (fRat < SQUAD_KITE_RATIO) iPosture = SQUAD_POS_KITE;
			else if (fRat > SQUAD_BOARD_RATIO) iPosture = SQUAD_POS_BOARD;
			else iPosture = SQUAD_POS_ENGAGE;
		}
	}
	// LEO: латч бегства (анти-фликер) + АНТИ-ЗАЛИПАНИЕ. Раз решив бежать, ДЕРЖИМ FLEE пока враг ближе SAFE_DIST И силы не
	//   восстановились (fRatL<KITE); иначе один просад = вечный драп. Восстановились (fRatL>=KITE) -> латч отпускаем.
	if (fNearest < SQUAD_FLEE_SAFE_DIST)
	{
		if (CheckAttribute(rGrp, "Posture"))
		{
			if (sti(rGrp.Posture) == SQUAD_POS_FLEE)
			{
					// LEO: дефолт 0.0 = «ещё в опасности» (враг ближе SAFE_DIST, но вне радиуса силы 1500). Иначе фликер FLEE<->ENGAGE в [1500,1800].
				float fRatL = 0.0;
				if (fEnPow > 0.5) fRatL = fMyPow / (fEnPow + 0.0001);
				if (fRatL < SQUAD_KITE_RATIO) iPosture = SQUAD_POS_FLEE; // всё ещё слабы (или враг вне радиуса силы) -> держим драп
			}
		}
	}
	// LEO: АГРЕССОР (пришёл с ATTACK: ОЗГ/охотники/боевая эскадра) группой НЕ бежит: FLEE -> KITE (дерётся с дистанции, не сваливает).
	bool bAggroGrp = false;
	if (CheckAttribute(rGrp, "BaseTask")) { if (sti(rGrp.BaseTask) == AITASK_ATTACK) bAggroGrp = true; }
	if (CheckAttribute(rCmd, "OZG")) bAggroGrp = true;
	if (CheckAttribute(rCmd, "AlwaysEnemy")) bAggroGrp = true;
	if (CheckAttribute(rCmd, "hunter")) bAggroGrp = true;
	// LEO: ВОЕННЫЙ/ЭСКОРТ (командир Ship.Mode != "trade") как АГРЕССОР: FLEE->KITE. Только ГРУЗОВЫЕ караваны (Mode=="trade")
	//   трусят. Признак — РОЛЬ в энкаунтере (Ship.Mode), НЕ тип корпуса и НЕ EncType.
	if (CheckAttribute(rCmd, "Ship.Mode")) { if (rCmd.Ship.Mode != "trade") bAggroGrp = true; }
	if (bAggroGrp) { if (iPosture == SQUAD_POS_FLEE) iPosture = SQUAD_POS_KITE; }
	// LEO: ОСАДА — осадная эскадра committed на штурм: группой не бежит и не кайтит. Осадники берут форт в Ship_CS_SiegeAttack
	//   (до HoldFormation -> ранний return), сторожей игнорят. Форс ENGAGE лишь чтоб позой не уйти в FLEE/RUNAWAY.
	if (CheckAttribute(rCmd, "SiegeAssault")) { if (iPosture == SQUAD_POS_FLEE || iPosture == SQUAD_POS_KITE) iPosture = SQUAD_POS_ENGAGE; }
	rGrp.Posture = iPosture;

	// LEO: РЕАКЦИЯ ГРУППЫ на врага — для ЛЮБОЙ эскадры, в т.ч. НЕЙТРАЛЬНОЙ к ГГ (AI-vs-AI с глобалки). Штатный мозг форсит
	//   враждебность к ГГ, на нейтралах звать нельзя. Потому реакция — ГРУППОВЫМ таском на РЕАЛЬНОГО врага (не ГГ) в радиусе:
	//   по позе БЕГИ -> RUNAWAY, иначе ATTACK на его группу. Идущую в ATTACK на ГГ не перенацеливаем. Гард от повторной установки.
	bool bTaskingPlayer = false;
	if (CheckAttribute(rGrp, "Task") && sti(rGrp.Task) == AITASK_ATTACK)
	{
		if (CheckAttribute(rGrp, "Task.Target") && rGrp.Task.Target == PLAYER_GROUP) bTaskingPlayer = true;
	}
	if (!bTaskingPlayer && iNearE >= 0 && iNearE != GetMainCharacterIndex() && fNearest < SQUAD_TAC_ENEMY_RADIUS)
	{
		string sEnGrp = Ship_GetGroupID(&characters[iNearE]);
		if (sEnGrp != "" && sEnGrp != sGroupID && sEnGrp != PLAYER_GROUP)
		{
			bool bTgtSame = false;
			if (CheckAttribute(rGrp, "Task.Target")) { if (rGrp.Task.Target == sEnGrp) bTgtSame = true; }
			bool bEnFlee = false;
			if (CheckAttribute(&characters[iNearE], "SeaAI.Task")) { if (sti(characters[iNearE].SeaAI.Task) == AITASK_RUNAWAY) bEnFlee = true; }
			if (iPosture == SQUAD_POS_FLEE)
			{
				if (!bTgtSame || sti(rGrp.Task) != AITASK_RUNAWAY) Group_SetTaskRunaway(sGroupID, sEnGrp); // мы слабее -> вся группа в обход
			}
			else
			{
				// LEO: БЕГЛЕЦА группой в ATTACK гоним ТОЛЬКО если ФЛАГМАН его догонит (иначе ведёт свиту в пустую погоню). Не догоняет ->
				//   групповой таск не трогаем: быстрые ведомые пелофятся сами (HoldFormation). Стоячий/наступающий враг -> обычный ATTACK.
				bool bLeadChase = true;
				if (bEnFlee) bLeadChase = Ship_CanCatch(rCmd, iNearE);
				if (bLeadChase)
				{
					if (!bTgtSame || sti(rGrp.Task) != AITASK_ATTACK) Group_SetTaskAttack(sGroupID, sEnGrp);
				}
				else
				{
					// LEO: флагман беглеца не догонит. НЕ гасим в DRIFT (спускает паруса и СТОИТ), а если группа гналась (ATTACK) —
					//   возвращаем RUNAWAY (конвой уходит своей дорогой, не прёт впустую). Быстрые ведомые сами пелофятся.
					if (bTgtSame && sti(rGrp.Task) == AITASK_ATTACK)
						{
							int iBase = AITASK_ATTACK;
							if (CheckAttribute(rGrp, "BaseTask")) iBase = sti(rGrp.BaseTask);
							if (iBase == AITASK_RUNAWAY) Group_SetTaskRunaway(sGroupID, sEnGrp); // конвой -> своей дорогой; агрессор -> ОСТАЁТСЯ ATTACK (гонится, не бежит)
						}
				}
			}
		}
	}

	// LEO: погоня «за горизонт». Группа в ATTACK, но беглец УШЁЛ за радиус скана и не догнать -> возвращаем RUNAWAY. Иначе
	//   таск атаки не снимался, когда торгаш уплывал за радиус (внутри-радиусный сброс не срабатывал).
	if (!bTaskingPlayer && iNearE >= 0 && iNearE != GetMainCharacterIndex() && sti(rGrp.Task) == AITASK_ATTACK && fNearest >= SQUAD_TAC_ENEMY_RADIUS)
	{
		bool bFarFlee = false;
		if (CheckAttribute(&characters[iNearE], "SeaAI.Task")) { if (sti(characters[iNearE].SeaAI.Task) == AITASK_RUNAWAY) bFarFlee = true; }
		if (bFarFlee && !Ship_CanCatch(rCmd, iNearE))
		{
			string sFarGrp = Ship_GetGroupID(&characters[iNearE]);
			if (sFarGrp != "" && sFarGrp != sGroupID && sFarGrp != PLAYER_GROUP)
		{
			int iBaseF = AITASK_ATTACK;
			if (CheckAttribute(rGrp, "BaseTask")) iBaseF = sti(rGrp.BaseTask);
			if (iBaseF == AITASK_RUNAWAY) Group_SetTaskRunaway(sGroupID, sFarGrp); // конвой -> уходит; агрессор -> ОСТАЁТСЯ ATTACK (не бежит)
		}
		}
	}

	// LEO: ПОЛУ-ПАРУСА ФЛАГМАНУ адаптивно (медленные не догоняют в строю). Вне боя капим ход командира под самого медленного
	//      ведомого: SailCap = Vмин_вед/Vмакс_ком (кламп 0.4..1.0). В бою (bEngaged) кап снят. Движок множит паруса на Ship.SeaAI.SailCap.
	if (bEngaged)
	{
		if (CheckAttribute(rCmd, "Ship.SeaAI.SailCap")) DeleteAttribute(rCmd, "Ship.SeaAI.SailCap");
	}
	else
	{
		float fCmdMax = Ship_SquadMaxSpeed(rCmd);
		float fCap = 1.0;
		if (fMinFollMax < 999999.0 && fCmdMax > 0.5) fCap = fMinFollMax / fCmdMax;
		if (fCap > 1.0) fCap = 1.0;
		if (fCap < 0.4) fCap = 0.4;
		rCmd.Ship.SeaAI.SailCap = fCap;
	}
}

// LEO: удержание СТРОЯ эскадрой ИИ — не-командиры держат DEFEND за командиром + слот формации (движок ведёт по FormFwd/FormLat).
// Командир ведёт обычной тактикой. true = строй применён. 3a: при латче боя (Engaged) строй РАСПУСКАЕТСЯ -> false -> индивид. тактика.
// LEO: снять крейсерский кап парусов (когда корабль выходит из строя в бой -> идёт полным ходом)
void Ship_Squad_ClearCruiseSail(ref rChar)
{
	if (CheckAttribute(rChar, "Ship.SeaAI.SailCap")) DeleteAttribute(rChar, "Ship.SeaAI.SailCap");
}

bool Ship_Squad_HoldFormation(ref rCharacter, string sGroupID)
{
	if (sGroupID == "" || sGroupID == PLAYER_GROUP) return false; // свита ГГ рулится своим кодом
	if (CheckAttribute(rCharacter, "nation")) { if (sti(rCharacter.nation) == PIRATE) return false; } // пираты — вразнобой, без строя
	int iCmd = Group_GetGroupCommanderIndex(sGroupID);
	if (iCmd < 0) return false;                          // нет командира
	if (iCmd == sti(rCharacter.index)) return false;     // Я командир -> веду
	if (LAi_IsDead(&characters[iCmd])) return false;     // командир мёртв -> обычная тактика (переназначится)
	if (Group_GetLiveCharactersNum(sGroupID) < 2) { Ship_Squad_ClearCruiseSail(rCharacter); return false; } // одиночка -> строя нет

	// LEO: ГРУППОВОЙ разрыв строя — строй лишь РЕЖИМ ПОДХОДА. Группа вступила в бой (латч rGrp.Engaged) ИЛИ приняла позу
	//     НЕ-ENGAGE (БЕГИ/кайт/абордаж) -> строй распускается РАЗОМ, все в индивидуальную тактику. Слабая эскадра не прёт на убой.
	ref rGrpH = Group_GetGroupByID(sGroupID);
	if (CheckAttribute(rGrpH, "Engaged")) { if (sti(rGrpH.Engaged) == 1) { Ship_Squad_ClearCruiseSail(rCharacter); return false; } }
	if (Ship_Squad_Posture(rCharacter) != SQUAD_POS_ENGAGE) { Ship_Squad_ClearCruiseSail(rCharacter); return false; }

	// LEO: ПЕР-ШИП разрыв строя — корабль САМ вступает в бой, когда ЕГО зацепило (стреляют) ИЛИ враг вошёл в его дальность.
	// НЕ ждём флагмана (иначе свиту безнаказанно расстреливали, пока он подходил). Каждый отвечает сам.
	bool bBreak = false;
	int iShotBy = sti(rCharacter.Ship.LastBallCharacter);
	if (iShotBy >= 0) { if (!LAi_IsDead(&characters[iShotBy])) bBreak = true; } // по мне стреляют -> в бой
	if (!bBreak)
	{
		int iNearE = Ship_FindNearestEnemy(rCharacter);
		if (iNearE >= 0) { if (Ship_GetDistance2D(rCharacter, &characters[iNearE]) < SQUAD_ENGAGE_DIST) bBreak = true; } // враг в дальности огня -> в бой
	}
	// LEO: ПЕЛ-ОФФ НА ПОГОНЮ. Быстрый ведомый выходит за убегающим, кого ДОГОНИТ (Ship_Squad_PursuitTarget). Медленные (-1)
	//     ОСТАЮТСЯ в строю. Флаг HasFleer гейтит дорогой скан, когда беглецов нет.
	if (!bBreak && CheckAttribute(rGrpH, "HasFleer"))
	{
		if (sti(rGrpH.HasFleer) == 1)
		{
			if (Ship_Squad_PursuitTarget(rCharacter, sGroupID) >= 0) bBreak = true;
		}
	}
	if (bBreak)
	{
		Ship_Squad_ClearCruiseSail(rCharacter); // дерусь/гоню -> полный ход
		return false;
	}

	// мой СЛОТ = порядковый номер среди живых НЕ-командиров группы
	ref rGrp = Group_GetGroupByID(sGroupID);
	int iNum = Group_GetCharactersNumR(rGrp);
	int iSlot = 0;
	int gi;
	int iMem;
	for (gi = 0; gi < iNum; gi++)
	{
		iMem = Group_GetCharacterIndexR(rGrp, gi);
		if (iMem < 0) continue;
		if (iMem == iCmd) continue;
		if (LAi_IsDead(&characters[iMem])) continue;
		if (iMem == sti(rCharacter.index)) break; // нашёл себя -> iSlot = мой номер
		iSlot++;
	}

	// форма группы (RndFormation ставит CalculateGroupShipPos) + слот в сеа-юнитах (как у компаньонов ГГ)
	int iForm = 0;
	if (CheckAttribute(rGrp, "RndFormation")) iForm = sti(rGrp.RndFormation);
	float fSp = 200.0 * Sea_FormSpacingFactor(rCharacter);
	rCharacter.Ship.SeaAI.FormFwd = Sea_FormationOffset(iForm, iSlot + 1, 0) * fSp;
	rCharacter.Ship.SeaAI.FormLat = Sea_FormationOffset(iForm, iSlot + 1, 1) * fSp;

	// DEFEND за командиром в SECONDARY-слот (НЕ primary!): логин-атака AIAttack_GroupAttack залила SECONDARY=ATTACK, primary
	//     DEFEND был бы ПЕРЕКРЫТ (вот почему строй «не держался»). Пишем DEFEND в secondary -> текущий таск = DEFEND -> движок
	//     ведёт строй, Fire() даёт огонь от линии. Держится пока флагман жив. НЕ переставлять каждый тик (спам движку).
	bool bAlready = false;
	if (CheckAttribute(rCharacter, "SeaAI.Task") && sti(rCharacter.SeaAI.Task) == AITASK_DEFEND)
	{
		if (CheckAttribute(rCharacter, "SeaAI.Task.Target") && sti(rCharacter.SeaAI.Task.Target) == iCmd) bAlready = true;
	}
	if (!bAlready) Ship_SetTaskDefend(SECONDARY_TASK, sti(rCharacter.index), iCmd);
	return true;
}

void Ship_CS_TacticalDecision(ref rCharacter, string sGroupID)
{
	ref   rShip           = GetRealShip(sti(rCharacter.Ship.Type));
	int   SailsPercent    = sti(rCharacter.Ship.SP);
	float HPPercent       = GetHullPercent(rCharacter);
	int   CrewQuantity    = sti(rCharacter.Ship.Crew.Quantity);
	int   MinCrewQuantity = GetMinCrewQuantity(rCharacter);
	int   iCharactersNum1, iCharactersNum2;
	int   iEnemy = sti(rCharacter.Ship.LastBallCharacter); // кто последним стрелял по нам (-1 = никто); в этой функции только читается
	// LEO: КОММИТ ЦЕЛИ — не флипаться на каждого последнего стрелявшего. При кучном огне цель флипалась каждое попадание ->
	//      перезарядка рестартилась и корабль не стрелял. Держим цель AGGRO_COMMIT_SEC, пока жива и враг; иначе переприцел.
	if (iEnemy >= 0)
	{
		if (CheckAttribute(rCharacter, "SeaAI.AggroTgt") && CheckAttribute(rCharacter, "SeaAI.AggroCommitT"))
		{
			int iAgTgt = sti(rCharacter.SeaAI.AggroTgt);
			if (iAgTgt >= 0 && iAgTgt != iEnemy && !LAi_IsDead(&characters[iAgTgt]))
			{
				if (GetRelation(sti(rCharacter.index), iAgTgt) == RELATION_ENEMY)
				{
					if (GetSeaTime() - sti(rCharacter.SeaAI.AggroCommitT) < AGGRO_COMMIT_SEC) iEnemy = iAgTgt; // держим коммит
				}
			}
		}
		if (!CheckAttribute(rCharacter, "SeaAI.AggroTgt") || sti(rCharacter.SeaAI.AggroTgt) != iEnemy)
		{
			rCharacter.SeaAI.AggroTgt = iEnemy;      // зафиксировать НОВУЮ цель + момент коммита
			rCharacter.SeaAI.AggroCommitT = GetSeaTime();
		}
	}

	// ещё не враг ГГ -> делаем врагом и натравливаем группу
	if (GetRelation(sti(rCharacter.index), GetMainCharacterIndex()) != RELATION_ENEMY)
	{
		SetCharacterRelationBoth(sti(rCharacter.index), GetMainCharacterIndex(), RELATION_ENEMY);
		Group_SetTaskAttack(sGroupID, PLAYER_GROUP);
		UpdateRelations();
		DoQuestCheckDelay("NationUpdate", 0.9);
	}

	// ── ПРИЗ (нацио-захват ИИ): бьётся за захватчика и НЕ убегает (ни скелет-командой, ни 1-vs-флот). Форсим атаку
	//    ближайшего врага, минуя дерево бегства. Флаг ставит AINationalizeCapturedShip. ──
	if (CheckAttribute(rCharacter, "PrizeNoFlee"))
	{
		int iPzTgt = iEnemy;
		if (iPzTgt < 0) iPzTgt = Ship_FindNearestEnemy(rCharacter);
		if (iPzTgt >= 0) { Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iPzTgt); return; }
	}

	// LEO: АГРЕССОР (ОЗГ/охотник/боевая эскадра с ATTACK) — «стоит до последнего»: не драпает от меньшинства, держится до
	//   HP<15% (не 25%), без crew-драпа. Squad-поза даёт ему KITE вместо FLEE. Детект: атрибуты ОЗГ ЛИБО базовый таск = ATTACK.
	bool bAggroShip = false;
	if (CheckAttribute(rCharacter, "OZG")) bAggroShip = true;
	if (CheckAttribute(rCharacter, "AlwaysEnemy")) bAggroShip = true;
	if (CheckAttribute(rCharacter, "hunter")) bAggroShip = true;
	// LEO: ВОЕННЫЙ/ЭСКОРТ по РОЛИ (Ship.Mode != "trade") — крепость как у ОЗГ: драп лишь при HP<15%. Только ГРУЗОВЫЕ
	//   торгаши (Ship.Mode=="trade") трусят. Признак — РОЛЬ (Ship.Mode), НЕ тип корпуса и НЕ EncType (у каравана "trade").
	if (CheckAttribute(rCharacter, "Ship.Mode")) { if (rCharacter.Ship.Mode != "trade") bAggroShip = true; }
	if (sGroupID != "")
	{
		ref rGAgg = Group_GetGroupByID(sGroupID);
		if (CheckAttribute(rGAgg, "BaseTask")) { if (sti(rGAgg.BaseTask) == AITASK_ATTACK) bAggroShip = true; }
	}

	// ── ТОРГОВЕЦ: не геройствует — при просадке корпуса до порога сваливает (RUNAWAY, отстреливаясь), даже если шлёт откуп ──
	if (Ship_GetAIBehavior(rCharacter) == AIBEH_TRADER && HPPercent <= AIBEH_TRADER_FLEE_HP)
	{
		int iFleeFrom = iEnemy;
		if (iFleeFrom < 0) iFleeFrom = Ship_FindNearestEnemy(rCharacter);
		if (iFleeFrom >= 0) { Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iFleeFrom); return; }
	}

	// ── ЭСКАДРА (Фаза 3): поза группы БЕГИ — эскадра слабее, сваливает вся. Форсим runaway, минуя дерево ниже. ──
	int iSquadPosture = Ship_Squad_Posture(rCharacter); // LEO: поза группы на этом участке не меняется (пишет только UpdateGroupTactics) — считаем 1 раз, не 3
	if (iSquadPosture == SQUAD_POS_FLEE)
	{
		int iSqFlee = iEnemy;
		if (iSqFlee < 0) iSqFlee = Ship_FindNearestEnemy(rCharacter);
		if (iSqFlee >= 0) { Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iSqFlee); return; }
	}

	// ── ЭСКАДРА (Фаза 3): поза АБОРДАЖ-РАШ (группа сильно сильнее) — не драпает индивидуально пока HP>AGGRO_MIN_HP (иначе
	//    доминирующая эскадра разбегалась по одному). Форсим АТАКУ ближайшего; манёвр делает движок. Крит. HP -> дерево ниже. ──
	if (iSquadPosture == SQUAD_POS_BOARD && HPPercent >= AGGRO_MIN_HP)
	{
		int iAgAtk = iEnemy;
		if (iAgAtk < 0) iAgAtk = Ship_FindNearestEnemy(rCharacter);
		if (iAgAtk >= 0) { Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iAgAtk); return; }
	}

	// ── ЭСКАДРА: ПОГОНЯ за убегающим — распараллелено, только за тем, кого ДОГОНИМ (Ship_Squad_PursuitTarget). Лишь если по
	//    мне не стреляют (iEnemy==-1): под огнём отвечаю стрелявшему. Медленный (-1) -> обычная тактика. Гейт HasFleer. ──
	if (iEnemy == -1 && sGroupID != "" && iSquadPosture == SQUAD_POS_ENGAGE)
	{
		ref rGpF = Group_GetGroupByID(sGroupID);
		if (CheckAttribute(rGpF, "HasFleer"))
		{
			if (sti(rGpF.HasFleer) == 1)
			{
				int iChase = Ship_Squad_PursuitTarget(rCharacter, sGroupID);
				if (iChase >= 0) { Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iChase); return; }
			}
		}
	}

	// ── ПЛЫВЁТ МИМО: по нам не стреляли / уже отдан приказ на абордаж / таск залочен ──
	if (iEnemy == -1 || CheckAttribute(rCharacter, "Ship_SetTaskAbordage") || CheckAttribute(rCharacter, "ShipTaskLock"))
	{
		if (CheckAttribute(rCharacter, "ShipTaskLock")) return;
		// плывём как есть ТОЛЬКО если цель реально ЖИВА. ГОЧА: при гибели цели движок обнуляет её в "" (Ship_CheckSituation:1906),
		// а sti("")==0 -> раньше принималось за живую цель на слоте 0 и корабль СТОЯЛ (компаньоны ГГ выбиты = пустая цель). Переназначим ниже.
		if (CheckAttribute(rCharacter, "SeaAI.Task.Target"))
		{
			if (rCharacter.SeaAI.Task.Target != "")
			{
				int iCurTgt = sti(rCharacter.SeaAI.Task.Target);
				if (iCurTgt >= 0)
				{
					if (!LAi_IsDead(&characters[iCurTgt]))
					{
						// держим цель ТОЛЬКО если она в разумной дистанции; далёкую (напр. назначенный группой корабль в порту за тыщи) бросаем -> переназначимся на ближайшего
						if (Ship_GetDistance2DRel(rCharacter, &characters[iCurTgt]) <= (AIBEH_RETARGET_MAXDIST * AIBEH_RETARGET_MAXDIST)) return;
					}
				}
			}
		}
		// переназначение: берём АДЕКВАТНУЮ цель — ближайшего врага (не форт), а не всегда ГГ; врагов нет — ГГ как запас
		int iReTgt = Ship_FindNearestEnemy(rCharacter);
		if (iReTgt < 0) iReTgt = GetMainCharacterIndex();
		if (CheckAttribute(rCharacter, "SeaAI.Task") && sti(rCharacter.SeaAI.Task) != AITASK_RUNAWAY)
		{
			// праздношатающийся кэп: при анализе кораблей и слабости считаем соотношение групп с ЦЕЛЬЮ
			if (CheckAttribute(rCharacter, "AnalizeShips") && stf(rCharacter.ship.hp) < (stf(characters[iReTgt].ship.hp) / 2))
			{
				iCharactersNum1 = Group_GetLiveCharactersNum(rCharacter.SeaAI.Group.Name);
				iCharactersNum2 = Group_GetLiveCharactersNum(characters[iReTgt].SeaAI.Group.Name);
				if (iCharactersNum2 > 0 && (iCharactersNum1 / iCharactersNum2) < 3.0) Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iReTgt);
				else                                                                   Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iReTgt);
				return;
			}
			Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iReTgt); // есть шанс победить - атакуем
			return;
		}
		// таск = RUNAWAY (или без таска): при хорошем состоянии возвращаемся в атаку (иначе ранэвей залипает)
		if (CheckAttribute(rCharacter, "AnalizeShips") && (HPPercent > 33) && (SailsPercent > 50) && stf(rCharacter.ship.hp) > (stf(characters[iReTgt].ship.hp) / 4))
			Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iReTgt);
		return;
	}

	// ── ПО НАМ СТРЕЛЯЛИ (iEnemy) ──
	// кэп без таска (маловероятно): простое решение атака/бегство по HP
	if (!CheckAttribute(rCharacter, "SeaAI.Task") || sti(rCharacter.SeaAI.Task) == AITASK_NONE)
	{
		if (CheckAttribute(rCharacter, "AnalizeShips") && stf(rCharacter.ship.hp) < (stf(characters[iEnemy].ship.hp) / 4))
			Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
		else
			Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
		return;
	}

	// хорошее состояние и НЕ убегает
	if ((SailsPercent > 50) && (HPPercent > 30) && sti(rCharacter.SeaAI.Task) != AITASK_RUNAWAY)
	{
		if (!CheckAttribute(rCharacter, "AnalizeShips")) { Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iEnemy); return; }
		if (bAggroShip) { Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iEnemy); return; } // LEO: агрессор здоров -> атакует, не бежит от численного/размерного меньшинства
		iCharactersNum1 = Group_GetLiveCharactersNum(rCharacter.SeaAI.Group.Name);
		iCharactersNum2 = Group_GetLiveCharactersNum(characters[iEnemy].SeaAI.Group.Name);
		if (Ship_CombatPower(rCharacter) < (Ship_CombatPower(&characters[iEnemy]) / 2.0)) // LEO: по МОЩИ (HP+команда+пушки), не по абс.корпусу (мелкий на полном HP «слабее» большого по размеру -> ложный драп; аудит RC4.3)
		{
		// LEO: слабее по мощи -> бежать ТОЛЬКО если ещё и в численном меньшинстве. Float-деление + ГАРД на 0 ДО деления
		//   (старое делило до гарда -> латентный div-by-zero при num2=0). Пороги целые — исход тот же, правка про безопасность.
			bool bNotOutnum = false;
			if (iCharactersNum2 > 0) { if ((makefloat(iCharactersNum1) / makefloat(iCharactersNum2)) >= 1.0) bNotOutnum = true; }
			if (bNotOutnum) Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
			else            Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
			return;
		}
		// шанс есть, но не лезем на крупные эскадры выше классом (float-деление + гард на 0)
		bool bWayOutnum = false;
		if (iCharactersNum1 > 0) { if ((makefloat(iCharactersNum2) / makefloat(iCharactersNum1)) >= 3.0) bWayOutnum = true; }
		if (bWayOutnum && sti(RealShips[sti(rCharacter.ship.type)].Class) > sti(RealShips[sti(characters[iEnemy].ship.type)].Class))
			Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
		else
			Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
		return;
	}

	// ── ПЛОХОЕ СОСТОЯНИЕ ──
	// брандер-смертник: почти труп + мало команды + военный корабль (редко: rand 1/20, бьёт всё вокруг)
	if ((HPPercent < 8) && (0.03 * stf(rCharacter.skill.LeaderShip) * MinCrewQuantity >= CrewQuantity) && (rShip.Type.War == true))
	{
		if (rand(20) == 1) Ship_SetTaskBrander(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
		return;
	}
	// сильно побит или мало команды -> бежим. LEO: "мало команды" НЕ считаем у корабля с почти полной командой — мелкие
	// (MaxCrew < 4*MinCrew) драпали целыми на спавне. Бежим по команде лишь при некомплекте < 50% макс.
	bool bLowCrewFlee = (CrewQuantity <= (4 * MinCrewQuantity));
	int  iMaxCrewFlee = GetMaxCrewQuantity(rCharacter);
	if (bLowCrewFlee && iMaxCrewFlee > 0 && (CrewQuantity * 2) >= iMaxCrewFlee) bLowCrewFlee = false;
	float fFleeHP = 25.0;
	if (bAggroShip) { fFleeHP = 15.0; bLowCrewFlee = false; } // LEO: агрессор/ОЗГ стоит до последнего — драп лишь при HP<15%, без crew-драпа
	if ((HPPercent < fFleeHP) || bLowCrewFlee) { Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iEnemy); return; }
	// пограничное состояние: пересматриваем бегство только при анализе кораблей и превосходстве по HP
	if (!CheckAttribute(rCharacter, "AnalizeShips") || stf(rCharacter.ship.hp) <= (stf(characters[iEnemy].ship.hp) / 2)) return;
	// только что ушёл от форта? (fWatchFort.Qty == 200 = пауза оглядки на форт)
	if (sti(rCharacter.Tmp.fWatchFort.Qty) == 200)
	{
		if (sti(rCharacter.Tmp.fWatchFort) >= 199) // за один такт до проверки форта
		{
			if (iEnemy == iFortCommander) Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), GetMainCharacterIndex()); // не камикадзимся на форт
			else                          Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
		}
		return;
	}
	// форт тут ни при чём
	if (iEnemy == iFortCommander) { Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), GetMainCharacterIndex()); return; } // не камикадзимся на форт
	iCharactersNum1 = Group_GetLiveCharactersNum(rCharacter.SeaAI.Group.Name);
	iCharactersNum2 = Group_GetLiveCharactersNum(characters[iEnemy].SeaAI.Group.Name);
	if ((iCharactersNum2 / iCharactersNum1) >= 3.0 && sti(RealShips[sti(rCharacter.ship.type)].Class) > sti(RealShips[sti(characters[iEnemy].ship.type)].Class))
		Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
	else
		Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
}

// Хелпер Ship_CheckSituation: квест мэра про разыскиваемого пирата (MQPirate). Оценивает шансы ГГ (HP/класс/команда/
// компаньоны) и решает напасть/абордаж/убегать; вблизи может абордировать ГГ. Вынесено 1:1 (только чтение, без return).
void Ship_CS_MQPirateQuest(ref rCharacter)
{
	//попытка обстрела до разговора, если ГГ слаб
	if (Ship_GetDistance2D(GetMainCharacter(), rCharacter) < 150 && !CheckAttribute(pchar, "GenQuest.DestroyPirate.WasFirstDeside"))
	{
		pchar.GenQuest.DestroyPirate.WasFirstDeside = true; //кэп принял решение об атаке сразу
		bool fight, shipHealth, shipClass, shipCrew, bFort;
		fight = false;
		bFort = false;
		shipHealth = false;
		shipClass = false;
		shipCrew = false;
		aref arReloadLoc, arFort;
		makearef(arReloadLoc, Islands[FindIsland(pchar.location)].reload);
		string  sLabel;
		int iQty = GetAttributesNum(arReloadLoc);
		Log_TestInfo("Sea.MaxSeaHeight " + Sea.MaxSeaHeight);
		if (bIsFortAtIsland) //т.к. пират, то форт враждебен всегда, не проверяем
		{
			if (GetDistance2D(stf(rCharacter.Ship.Pos.x), stf(rCharacter.Ship.Pos.z), fFort_x, fFort_z) < 1000)
			{
				bFort = true; //форт рядом!!
			}
		}
		//смотрим состояние парусов и корпуса (0..1)
		if ((GetCurrentShipHP(pchar) / GetCharacterShipHP(pchar)) < 0.5 || (sti(pchar.ship.sp) / GetCharacterShipSP(pchar) < 0.4))
		{
			shipHealth = true;
		}
		//смотрим класс корабля
		if ((sti(RealShips[sti(rCharacter.ship.type)].Class) - sti(RealShips[sti(pchar.ship.type)].Class)) <= 1)
		{
			shipClass = true;
		}
		//смотрим кол-во команды на судне, мораль и опыт солдат
		if ((GetCrewQuantity(pchar) / GetMaxCrewQuantity(pchar)) < 0.4 || GetCrewExp(pchar, "Soldiers") < 20 || GetCharacterCrewMorale(pchar) < 20)
		{
			if ((GetCrewQuantity(pchar) / GetCrewQuantity(rCharacter)) < 1.34) shipCrew = true;
		}
		//анализим наши шансы с учетом компаньонов ГГ
		iQty = GetCompanionQuantity(pchar);
		if (iQty == 1)
		{
			if (shipHealth || shipClass || shipCrew) fight = true;
		}
		if (iQty == 2)
		{
			if (shipClass && shipCrew || shipHealth) fight = true;
		}
		if (iQty == 3)
		{
			if (shipClass && shipCrew && shipHealth) fight = true;
		}
		if (fight && !bFort) //есть причина напасть, но не рядом с фортом
		{
			if (sti(rCharacter.nation) != PIRATE)
			{
				rCharacter.nation = PIRATE;
				Ship_NationAgressivePatent(rCharacter);
				Ship_FlagRefresh(characterFromId("MQPirate")); //флаг на лету
			}
			Ship_SetTaskAbordage(SECONDARY_TASK, sti(rCharacter.index), nMainCharacterIndex);
		}
		pchar.GenQuest.DestroyPirate.shipState = shipCrew + shipClass + shipHealth; //в диалоге нужно чтобы не считать дважды
		pchar.GenQuest.DestroyPirate.fortPlace = bFort; //в зоне ли форта, для диалога на палубе
		Log_QuestInfo("Оценка команды (нет-да/0-1): " + shipCrew);
		Log_QuestInfo("Оценка корабля (нет-да/0-1): " + shipClass);
		Log_QuestInfo("Оценка состояния корабля (нет-да/0-1): " + shipHealth);
		Log_QuestInfo("В зоне попадания форта (нет-да/0-1): " + bFort);
	}
	//попытка абордажа, если ГГ подошёл на близкое расстояние. это для хитрых геймеров :)
	if (Ship_GetDistance2D(GetMainCharacter(), rCharacter) < 30 && CheckAttribute(pchar, "GenQuest.DestroyPirate.WasFirstDeside") && !CheckAttribute(pchar, "GenQuest.DestroyPirate.FastAbordage"))
	{
		pchar.GenQuest.DestroyPirate.FastAbordage = true; //проверили абордаж
		if ((GetCrewQuantity(pchar) / GetCrewQuantity(rCharacter)) < 1.0) //один критерий, остальные не важны
		{
			if (sti(rCharacter.nation) != PIRATE)
			{
				rCharacter.nation = PIRATE;
				Ship_NationAgressivePatent(rCharacter);
				Ship_FlagRefresh(characterFromId("MQPirate")); //флаг на лету
			}
			Ship_SetTaskAbordage(SECONDARY_TASK, sti(rCharacter.index), nMainCharacterIndex);
		}
		else
		{
			Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), nMainCharacterIndex); //сваливаем
		}
	}
}

// Хелпер Ship_CheckSituation: единоразовое распознавание врага под ложным флагом. При перемирии/самовозе казённого груза не разнюхивает; не чаще раза в 2 суток, вблизи.
void Ship_CS_CheckFalseFlag(ref rCharacter, bool bSpeakTruce)
{
	if (bSpeakTruce || CheckAttribute(rCharacter, "CheckFlagYet") || FortAgent_DeliverGraceNation(sti(rCharacter.nation))) return;
	if (CheckAttribute(rCharacter, "CheckFlagDate") && GetNpcQuestPastDayParam(rCharacter, "CheckFlagDate") <= 1) return; // второй раз подряд не смотрим
	if (Ship_GetDistance2D(GetMainCharacter(), rCharacter) >= MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER) return;
	rCharacter.CheckFlagYet = true; // всё, проверил
	Ship_CheckFlagEnemy(rCharacter);
	SaveCurrentNpcQuestDateParam(rCharacter, "CheckFlagDate");
}

// Хелпер Ship_CheckSituation: оглядка на форт (WatchFort). Раз в fWatchFort.Qty тактов — если близко к вражескому форту, уводит в бегство от коменданта.
void Ship_CS_WatchFort(ref rCharacter)
{
	if (!CheckAttribute(rCharacter, "WatchFort") || !bIsFortAtIsland || GetRelation(sti(rCharacter.index), iFortCommander) != RELATION_ENEMY || CheckAttribute(rCharacter, "ShipTaskLock")) return;
	if (sti(rCharacter.Tmp.fWatchFort) < sti(rCharacter.Tmp.fWatchFort.Qty)) { rCharacter.Tmp.fWatchFort = sti(rCharacter.Tmp.fWatchFort) + 1; return; }
	rCharacter.Tmp.fWatchFort = 0; // сброс счётчика оптимизации
	if (GetDistance2D(stf(rCharacter.Ship.Pos.x), stf(rCharacter.Ship.Pos.z), fFort_x, fFort_z) >= 2000) return;
	Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iFortCommander);
	rCharacter.Tmp.fWatchFort.Qty = 200; // не лочим таск, но увеличиваем паузу срабатывания
}

// Хелпер Ship_CheckSituation: ветка SinkTenPercent (пираты/ЧЖ на абордаж). Без флага жемчужника -> атака ГГ; PearlTartane -> вблизи грабим жемчуг (квест Sharp).
void Ship_CS_PearlOrAttack(ref rCharacter)
{
	int iCharIdx = sti(rCharacter.index);
	if (CheckAttribute(rCharacter, "PearlTartane") && sti(rCharacter.PearlTartane))
	{
		// без лишнего sqrt
		if (Ship_GetDistance2DRel(GetMainCharacter(), rCharacter) < 256) // 16^ Rel, instead of 16)
		{
			PlaySound("interface\important_item.wav");
			int SmallPearlQty = rand(250) + 100;
			int BigPearlQty = rand(150) + 50;
			TakeNItems(pchar, "jewelry12", SmallPearlQty);
			TakeNItems(pchar, "jewelry11", BigPearlQty);
			Log_SetStringToLog(GetConvertStrWithReplace("Variable_AIShip_8", "Logs.txt", "#space#", " ") + rCharacter.ship.name + GetConvertStrWithReplace("Variable_AIShip_9", "Logs.txt", "#space#", " ") + SmallPearlQty + GetConvertStrWithReplace("Variable_AIShip_10", "Logs.txt", "#space#", " ") + BigPearlQty + GetConvertStrWithReplace("Variable_AIShip_11", "Logs.txt", "#space#", " "));
			rCharacter.PearlTartane = false; //уже ограбили
			pchar.questTemp.Sharp.PearlAreal.SmallPearlQty = sti(pchar.questTemp.Sharp.PearlAreal.SmallPearlQty) + SmallPearlQty;
			pchar.questTemp.Sharp.PearlAreal.BigPearlQty = sti(pchar.questTemp.Sharp.PearlAreal.BigPearlQty) + BigPearlQty;
			pchar.questTemp.Sharp = "SeaPearl_success"; //флаг успешное нападение
		}
	}
	// <-- в целях экономии ресурсов АИ квест жемчужников суем сюда на обработку
	else
	{
		if (!CheckAttribute(rCharacter, "PearlTartane"))
		{
			Ship_SetTaskAttack(SECONDARY_TASK, iCharIdx, GetMainCharacterIndex());
		}
	}
}

// LEO: компаньон после абордажа/гибели цели «залипает» с боевым таском без цели. Цель пуста/мертва -> берём ближайшего
// врага и решаем ПО ПРАВИЛАМ (перевес+скорость с ветром => абордаж; иначе атака); врагов нет -> защита ГГ. Валидную цель не
// трогаем. Берём ближайшего врага ИГРОКА (снимок отношений компаньона стух) -> синкаем вражду, добиваем. Форт/ГГ/сдавшихся пропускаем.
int Ship_Companion_FindNearestPlayerEnemy(ref rCharacter)
{
	int meIdx = sti(rCharacter.index);
	int iMain = GetMainCharacterIndex();
	int best = -1;
	float bestD = AIBEH_RETARGET_MAXDIST * AIBEH_RETARGET_MAXDIST;
	int k, idx;
	float d;
	for (k = 0; k < iNumShips; k++)
	{
		idx = Ships[k];
		if (idx < 0) continue;
		if (idx == meIdx) continue;
		if (idx == iMain) continue;
		if (idx == iFortCommander) continue;                      // на форт автономно не камикадзимся (только под приказом)
		if (LAi_IsDead(&characters[idx])) continue;
		if (GetRelation(iMain, idx) != RELATION_ENEMY) continue;  // враг ИГРОКА (своих/нейтралов/сдавшихся пропускаем)
		d = Ship_GetDistance2DRel(rCharacter, &characters[idx]);
		if (d < bestD) { bestD = d; best = idx; }
	}
	if (best >= 0) SetCharacterRelationBoth(meIdx, best, RELATION_ENEMY); // синк -> компаньон реально стреляет и не сорвётся снова
	return best;
}

void Ship_Companion_Redecide_IfIdle(ref rCharacter)
{
	int iTask = AITASK_NONE;
	if (CheckAttribute(rCharacter, "SeaAI.Task")) iTask = sti(rCharacter.SeaAI.Task);
	if (iTask != AITASK_ABORDAGE && iTask != AITASK_ATTACK) return; // Defend/Follow/Move/Drift не трогаем

	// цель ещё жива И ВРАЖДЕБНА? тогда не вмешиваемся в приказ игрока.
	// (сдача/откуп/перемирие делают цель нейтральной -> бросаем таск, переназначим ниже.)
	if (CheckAttribute(rCharacter, "SeaAI.Task.Target"))
	{
		if (rCharacter.SeaAI.Task.Target != "")
		{
			int iCurT = sti(rCharacter.SeaAI.Task.Target);
			if (iCurT >= 0)
			{
				// LEO: цель = МЁРТВЫЙ/захваченный ФОРТ? Форт НЕ LAi_IsDead (не живой персонаж) и остаётся RELATION_ENEMY,
				//   поэтому прежняя проверка держала разрушенный форт целью -> компаньон уплывал к нему (юзер). Считаем «цель ушла».
				bool bTgtDeadFort = false;
				if (bIsFortAtIsland) { if (iCurT == iFortCommander) { if (CheckAttribute(&characters[iCurT], "Fort.Mode")) { if (sti(characters[iCurT].Fort.Mode) != FORT_NORMAL) bTgtDeadFort = true; } } }
				if (!bTgtDeadFort)
				{
					if (!LAi_IsDead(GetCharacter(iCurT)))
					{
						if (GetRelation(sti(rCharacter.index), iCurT) == RELATION_ENEMY) return; // уже враг компаньону -> держим приказ
						// LEO: компаньон цель врагом не видит (снимок отношений стух), но ИГРОК видит -> синкаем и держим приказ
						//   (иначе бросает в Defend + Fire() не бьёт). Цель сдалась -> у игрока нейтрал -> не синкаем -> бросаем.
						if (GetRelation(GetMainCharacterIndex(), iCurT) == RELATION_ENEMY)
						{
							SetCharacterRelationBoth(sti(rCharacter.index), iCurT, RELATION_ENEMY);
							return;
						}
					}
				}
			}
		}
	}

	// цель пуста/мертва/нейтральна -> выбираем новое действие
	// LEO: сброс форс-боеприпаса игрока (напр. картечь под абордаж) — приказ выполнен, дальше авто-выбор
	// снаряда по типу/дистанции (иначе после абордажа корабль бьёт по атаке залипшей картечью).
	DeleteAttribute(rCharacter, "ShipCannonChargeType");
	rCharacter.CompanionAutoEngage = true; // LEO: компаньон дальше действует АВТОНОМНО -> разрешаем кайт (снимет прямой приказ игрока)
	int iEnemy = Ship_FindNearestEnemy(rCharacter);
	// LEO: снимок отношений компаньона стух -> Ship_FindNearestEnemy (по ЕГО отношению) не видит врагов ИГРОКА и компаньон
	//   сваливался в Defend после каждого приказа (жалоба). Фолбэк: ближайший враг ИГРОКА -> синк отношения -> добиваем.
	if (iEnemy < 0) iEnemy = Ship_Companion_FindNearestPlayerEnemy(rCharacter);
	if (iEnemy < 0)
	{
		Ship_SetTaskDefend(SECONDARY_TASK, sti(rCharacter.index), GetMainCharacterIndex()); // реально врагов нет -> защита флагмана
		return;
	}
	ref rEnemy = GetCharacter(iEnemy);
	// правило абордажа: численный перевес команды + БЛИЗКО (потолок выдачи таска) + реально догоняем.
	// LEO: тот же фикс, что в тактике ИИ - было «ближе 300м скорость не важна» + сырой MaxSpeedZ (скачет по курсу
	//   к ветру). Теперь: дальше AIBEH_ABORD_MAXDIST абордаж не выдаём вообще (идём в ATTACK и сближаемся с огнём),
	//   а в 80..200м требуем ЧЕСТНУЮ догоняемость (Ship_CanCatch: моя скорость на курсе цели против её).
	bool bCanBoard = false;
	if (GetCrewQuantity(rCharacter) > GetCrewQuantity(rEnemy))
	{
		float fEnemyDist = Ship_GetDistance2D(rCharacter, rEnemy);
		if (fEnemyDist <= AIBEH_ABORD_MAXDIST)
		{
			if (fEnemyDist < AIBEH_ABORD_NOCATCH) bCanBoard = true;
			else bCanBoard = Ship_CanCatch(rCharacter, iEnemy);
		}
	}
	if (bCanBoard) Ship_SetTaskAbordage(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
	else           Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iEnemy);
}

// LEO: доля дистанции огня, на которой компаньон КАЙТИТ ПО КРАЮ при скоплении врагов — держит максимум дальности, ещё стреляя.
#define COMPANION_EDGE_HOLD 0.95

// LEO: сколько врагов КОМПАНЬОНА сгрудилось вокруг ЦЕЛИ iTgt (цель уводит в толпу). Дистанцию мерим от ЦЕЛИ, вражду — к КОМПАНЬОНУ.
int Ship_Companion_SwarmAtTarget(ref rCharacter, int iTgt, float fRadius)
{
	if (iTgt < 0) return 0;
	int meIdx = sti(rCharacter.index);
	ref rTgt = GetCharacter(iTgt);
	int cnt = 0;
	int k, idx;
	for (k = 0; k < iNumShips; k++)
	{
		idx = Ships[k];
		if (idx < 0) continue;
		if (idx == meIdx) continue;
		if (idx == iFortCommander) continue;
		if (LAi_IsDead(&characters[idx])) continue;
		if (GetRelation(meIdx, idx) != RELATION_ENEMY) continue; // враг КОМПАНЬОНА
		if (Ship_GetDistance2D(rTgt, &characters[idx]) < fRadius) cnt++; // ...рядом с ЦЕЛЬЮ
	}
	return cnt;
}

// LEO: сколько ВРАЖЕСКИХ кораблей в радиусе fRadius от корабля (оценка "толпы").
int Ship_Companion_ThreatCount(ref rCharacter, float fRadius)
{
	int meIdx = sti(rCharacter.index);
	int cnt = 0;
	int k, idx;
	for (k = 0; k < iNumShips; k++)
	{
		idx = Ships[k];
		if (idx < 0) continue;
		if (idx == meIdx) continue;
		if (idx == iFortCommander) continue;
		if (LAi_IsDead(&characters[idx])) continue;
		if (GetRelation(meIdx, idx) != RELATION_ENEMY) continue;
		if (Ship_GetDistance2D(rCharacter, &characters[idx]) < fRadius) cnt++;
	}
	return cnt;
}

// LEO: АВТОНОМНЫЙ кайт компаньона — ТОЛЬКО при флаге CompanionAutoEngage (ставит re-decide; любой прямой приказ игрока
// снимает). Скопление врагов -> RUNAWAY (движок уводит по ветру). Оторвался -> снова в атаку с дальней. Абордаж/Defend/Follow не трогаем.
void Ship_Companion_AutoKite(ref rCharacter)
{
	if (COMPANION_AUTOKITE == 0) return;
	if (!CheckAttribute(rCharacter, "CompanionAutoEngage")) return; // под прямым приказом игрока — не кайтим

	int iTask = AITASK_NONE;
	if (CheckAttribute(rCharacter, "SeaAI.Task")) iTask = sti(rCharacter.SeaAI.Task);
	if (iTask != AITASK_ATTACK && iTask != AITASK_RUNAWAY) return;

	int nClose = Ship_Companion_ThreatCount(rCharacter, COMPANION_SWARM_RADIUS);
	if (iTask == AITASK_ATTACK)
	{
		if (nClose >= COMPANION_SWARM_N)
		{
			int iFrom = Ship_FindNearestEnemy(rCharacter);
			if (iFrom >= 0) Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), iFrom); // толпа -> откат от угроз
		}
	}
	else // RUNAWAY: оторвался -> возобновляем атаку с дальней дистанции
	{
		if (nClose <= COMPANION_SWARM_CLEAR)
		{
			int iTgt = Ship_FindNearestEnemy(rCharacter);
			if (iTgt >= 0) Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iTgt);
			else           Ship_SetTaskDefend(SECONDARY_TASK, sti(rCharacter.index), GetMainCharacterIndex());
		}
	}
}

// LEO: ОСАДА — осадные корабли АТАКУЮТ ФОРТ. Эталон — сторожевики (Ship_CS_DefenderAI): их ИИ висит РАНЬШЕ гейта «враг ГГ»,
//   поэтому работает и для нейтральной к ГГ эскадры. Вражду к форту ставим ВСЕМ (Fire() бьёт лишь по ENEMY). ФЛАГМАН всегда
//   берёт форт в ATTACK; СВИТА — только когда рядом нет сторожевиков. Движок ведёт БОРТОМ вокруг. true = взяли форт (return).
bool Ship_CS_SiegeAttack(ref rCharacter, string sGroupID)
{
	if (!CheckAttribute(rCharacter, "SiegeAssault")) return false;
	if (sGroupID == "") return false;
	if (iFortCommander < 0) return false;
	if (!bIsFortAtIsland) return false;                                 // форт есть и жив (ставит sea.c при загрузке локации)
	// форт «жив» = Fort.Mode == FORT_NORMAL (LAi_IsDead для форта не годится). LEO: форт ПОБЕЖДЁН = FORT_ABORDAGE (пушки
	//   выбиты) ИЛИ FORT_DEAD — больше НЕ кружим по нему. ДОБИТЬ остаток врагов-кораблей с коммитом цели (SiegeFinTgt, без
	//   мерцания); врагов нет -> ДРЕЙФ (паруса вниз). Мерцание убрано фиксом сторожей (SECONDARY-задача) + коммитом здесь.
	int iFMode = FORT_NORMAL;
	if (CheckAttribute(&characters[iFortCommander], "Fort.Mode")) iFMode = sti(characters[iFortCommander].Fort.Mode);
	bool bFortBeaten = false;
	if (iFMode == FORT_DEAD) bFortBeaten = true;
	if (iFMode == FORT_ABORDAGE) bFortBeaten = true;
	if (bFortBeaten)
	{
		// держим текущую добиваемую цель, пока жива / враг / в пределах ретаргета -> анти-мерцание
		int iFin = -1;
		if (CheckAttribute(rCharacter, "SiegeFinTgt")) iFin = sti(rCharacter.SiegeFinTgt);
		bool bKeepFin = false;
		if (iFin >= 0)
		{
			if (!LAi_IsDead(GetCharacter(iFin)))
			{
				if (GetRelation(sti(rCharacter.index), iFin) == RELATION_ENEMY)
				{
					if (Ship_GetDistance2D(rCharacter, GetCharacter(iFin)) < AIBEH_RETARGET_MAXDIST) bKeepFin = true;
				}
			}
		}
		if (!bKeepFin) iFin = Ship_FindNearestEnemy(rCharacter); // форт исключён внутри -> остаются корабли-сторожа
		if (iFin >= 0)
		{
			rCharacter.SiegeFinTgt = iFin;
			Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iFin); // добиваем остаток
		}
		else
		{
			if (CheckAttribute(rCharacter, "SiegeFinTgt")) DeleteAttribute(rCharacter, "SiegeFinTgt");
			Ship_SetTaskDrift(SECONDARY_TASK, sti(rCharacter.index)); // врагов нет -> дрейф, паруса вниз (карусель по форту выключена)
		}
		return true;
	}
	// вражда осадника к коменданту форта — ОДИН раз (флаг-гард): движок Fire() бьёт только по RELATION_ENEMY
	if (!CheckAttribute(rCharacter, "SiegeFortEnemied")) { rCharacter.SiegeFortEnemied = true; SetCharacterRelationBoth(sti(rCharacter.index), iFortCommander, RELATION_ENEMY); }
	// СВИТА (не командир): пока рядом ЕСТЬ сторожевик-корабль — бьёт ЕГО (обычная тактика ниже), форт не берёт. Форт из
	//   Ship_FindNearestEnemy исключён -> это именно корабли-сторожа. Флагман падает мимо этого гейта -> всегда на форт.
	if (Group_GetGroupCommanderIndex(sGroupID) != sti(rCharacter.index))
	{
		if (Ship_FindNearestEnemy(rCharacter) >= 0) return false;
	}
	// ATTACK по форту как по кораблю — движок маневрирует БОРТОМ вокруг форта на дистанции (AIFort::GetAttackPoint:
	//   бортовая точка на AttackDistance + касательная -> кружит); Fire() бьёт с дистанции, не таранит и не встаёт носом.
	Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iFortCommander);
	return true;
}

void Ship_CheckSituation()
{
    //#20181101-01
    int oldRel, newRel;
	ref		rShip;
	aref	rSituation;

	aref	rCharacter = GetEventData();
	aref	rShipObject = GetEventData();

	if (sti(rCharacter.index) == nMainCharacterIndex)  return;
	if (Boarding_Busy(rCharacter)) return;
	if (LAi_IsDead(rCharacter) || sti(rCharacter.ship.type) == SHIP_NOTUSED) { return; }  // super fix boal
	if (HasSubStr(rCharacter.id, "_DriftCap_"))
	{
		Ship_SetTaskDrift(PRIMARY_TASK, sti(rCharacter.index));
		Ship_SetTaskDrift(SECONDARY_TASK, sti(rCharacter.index));
		return;
	}

	int iCharIdx = sti(rCharacter.index);
	int aiTask = AITASK_NONE;
	if (CheckAttribute(rCharacter, "SeaAI.Task"))
    {
        aiTask = sti(rCharacter.SeaAI.Task);
    }
	else
    {
		trace("Character " + rCharacter.id  + " without task");
		rCharacter.SeaAI.Task = AITASK_NONE;
	}

	rShip = GetRealShip(sti(rCharacter.Ship.Type));
	makearef(rSituation, rCharacter.SeaAI.Update.Situation);

	float	fMinEnemyDistance = stf(rSituation.MinEnemyDistance);
	float	fDistance;
	int nLastBallChar = sti(rCharacter.Ship.LastBallCharacter);
	// boal fix: fMinEnemyDistance не сбрасывался. && в Storm НЕ ленивый — при nLastBallChar==-1
	// старая запись звала GetCharacter(-1) (спам "Incorrect Index" каждый апдейт). Гвард -1 отдельно.
	if (nLastBallChar != -1)
	{
		if (LAi_IsDead(GetCharacter(nLastBallChar))) rCharacter.Ship.LastBallCharacter = -1;
	}
    int nTaskTarg = 0;
	if (CheckAttribute(rCharacter, "SeaAI.Task.Target"))
	{
		if (rCharacter.SeaAI.Task.Target != "" && LAi_IsDead(GetCharacter(sti(rCharacter.SeaAI.Task.Target))))
	    {
	        rCharacter.SeaAI.Task.Target = "";
	    }
		nTaskTarg = sti(rCharacter.SeaAI.Task.Target);
    }
    if (Ship_AutoAbordage(rCharacter, fMinEnemyDistance)) { return; }

	// переговоры (Speak): враг сам выходит на связь (откуп/дань) + тикает его боевая мораль (seaspeak.c)
	Speak_EnemyInitiative(rCharacter);

	// перемирие по переговорам и сдача в плен обслуживаются целиком в хелперах (ранний return)
	if (Ship_CS_HandleSpeakTruce(rCharacter)) return;
	if (Ship_CS_HandleSurrendered(rCharacter)) return;

	// Warship 08.07.09 Запрет спуска парусов
	if(CheckAttribute(rCharacter, "CanDropSails") && !sti(rCharacter.CanDropSails)) //  && Ship_GetSailState(rCharacter) < 1.0 // Так работает через раз
	{
		Ship_SetSailState(sti(rCharacter.index), 1.0);
	}

	// проверка достаточного числа снарядов
	int iShipCannonsNum  = GetCannonsNum(rCharacter);
	int iCurrentBallType = sti(rCharacter.Ship.Cannons.Charge.Type);
	bool bBalls, bKnippels, bBombs, bGrapes;
	bBalls = true; bKnippels = true; bBombs = true; bGrapes = true;
	// LEO: цель=ФОРТ -> игнор форс-снаряда игрока, идём АВТО-путём (форт-гейт даёт ЯДРА; иначе компаньоны перли с картечью)
	bool bTgtFortAmmo = false;
	if (bIsFortAtIsland) { if (iFortCommander >= 0) { if (CheckAttribute(rCharacter, "SeaAI.Task.Target")) { if (rCharacter.SeaAI.Task.Target != "") { if (sti(rCharacter.SeaAI.Task.Target) == iFortCommander) bTgtFortAmmo = true; } } } }
	if (CheckAttribute(rCharacter, "ShipCannonChargeType") && !bTgtFortAmmo) // офам приказ, чем палить все время (но НЕ по форту)
	{
	    // LEO: заказанный игроком снаряд кончился ЛИБО нет ПОРОХА (расход 1:1, AICannon.c) -> снять приказ; else-ветка сблизит на абордаж (bCantFire).
	    bool bForcedGone = false;
	    if (GetCargoGoods(rCharacter, sti(rCharacter.ShipCannonChargeType)) < iShipCannonsNum) bForcedGone = true;
	    if (GetCargoGoods(rCharacter, GOOD_POWDER) < iShipCannonsNum) bForcedGone = true;
	    if (bForcedGone)
	    {
	        DeleteAttribute(rCharacter, "ShipCannonChargeType"); // следующий проход решит на что менять заряд
	    }
	    else
	    {
	        // LEO: держать дистанцию под ЗАДАННЫЙ игроком боеприпас (иначе крутились вплотную, чем бы ни зарядили).
	        // Только при атаке с живой целью; таск переустанавливаем лишь при заметном расхождении дистанции (без спама).
	        if (CheckAttribute(rCharacter, "SeaAI.Task") && sti(rCharacter.SeaAI.Task) == AITASK_ATTACK && CheckAttribute(rCharacter, "SeaAI.Task.Target") && rCharacter.SeaAI.Task.Target != "")
	        {
	            int iForced = sti(rCharacter.ShipCannonChargeType);
	            float fReach = 1.0; // ядра — дальше всех
	            if (iForced == GOOD_KNIPPELS) fReach = AMMO_REACH_KNIPPEL;
	            if (iForced == GOOD_BOMBS)    fReach = AMMO_REACH_BOMBS;
	            if (iForced == GOOD_GRAPES)   fReach = AMMO_REACH_GRAPE;
	            ref rCnF = GetCannonByType(sti(rCharacter.Ship.Cannons.Type));
	            float fRangeF = 500; // LEO: дефолт+ГАРД (зеркало reselect@~2920 и Ship_FindOtherBallType@~3371): кривой/пустой Cannons.Type -> нет FireRange -> stf("")=0 -> AttackDistance=0 -> таран
	            if (CheckAttribute(rCnF, "FireRange")) fRangeF = stf(rCnF.FireRange);
	            float fADf = fRangeF * fReach;
	            float fDiffAD = stf(rCharacter.Ship.SeaAI.Init.AttackDistance) - fADf;
	            if (fDiffAD > 10.0 || fDiffAD < -10.0)
	            {
	                rCharacter.Ship.SeaAI.Init.AttackDistance = fADf;
	                Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), sti(rCharacter.SeaAI.Task.Target));
	            }
	        }
	    }
	}
	else
	{
        if (GetCargoGoods(rCharacter,GOOD_BALLS)    < iShipCannonsNum) { bBalls    = false; }
		if (GetCargoGoods(rCharacter,GOOD_BOMBS)    < iShipCannonsNum) { bBombs    = false; }
		if (GetCargoGoods(rCharacter,GOOD_KNIPPELS) < iShipCannonsNum) { bKnippels = false; }
		if (GetCargoGoods(rCharacter,GOOD_GRAPES)   < iShipCannonsNum) { bGrapes   = false; }
		// LEO: НЕ стрелять = нет ни одного типа снарядов ЛИБО нет ПОРОХА (расход 1:1, AICannon.c). Кайт под пустой
		//   залп бессмысленен -> на абордаж (воюет командой). Гейт на изменение дистанции — без спама таска.
		bool bCantFire = false;
		if (GetCargoGoods(rCharacter, GOOD_POWDER) < iShipCannonsNum) bCantFire = true; // пороха не хватает на залп
		if (!bBalls && !bBombs && !bKnippels && !bGrapes) bCantFire = true;             // ни одного типа снарядов
		if (bCantFire)
		{
			if (CheckAttribute(rCharacter, "SeaAI.Task") && sti(rCharacter.SeaAI.Task) == AITASK_ATTACK && CheckAttribute(rCharacter, "SeaAI.Task.Target") && rCharacter.SeaAI.Task.Target != "")
			{
				if (stf(rCharacter.Ship.SeaAI.Init.AttackDistance) > AIBEH_ABORD_CLOSE)
				{
					rCharacter.Ship.SeaAI.Init.AttackDistance = AIBEH_ABORD_CLOSE;
					Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), sti(rCharacter.SeaAI.Task.Target)); // подхватить новую дистанцию
				}
			}
		}

		// LEO: по ФОРТУ (осада/приказ) стреляем только ЯДРАМИ/БОМБАМИ. Картечь/книппель бесполезны и тянут
		//   вплотную под форт. Гейт — любой корабль с целью=форт (надмножество прежнего SiegeAssault).
		if (bIsFortAtIsland && iFortCommander >= 0)
		{
			if (CheckAttribute(rCharacter, "SeaAI.Task.Target") && rCharacter.SeaAI.Task.Target != "")
			{
				if (sti(rCharacter.SeaAI.Task.Target) == iFortCommander) { bKnippels = false; bGrapes = false; }
			}
		}
		int iNewBallType = iCurrentBallType;
	    if (LetCharge)
	    {
	        iNewBallType = Ship_FindOtherBallType(rCharacter, fMinEnemyDistance, bBalls, bBombs, bGrapes, bKnippels);
	    	if (iNewBallType >= 0 && iNewBallType != iCurrentBallType)
		    {
		   	   Ship_ChangeCharge(rCharacter, iNewBallType);
		    }
	    }
	    else
	    {
				ref rCannon = GetCannonByType(sti(rCharacter.Ship.Cannons.Type));
				float range = 500; // LEO: дефолт + ГВАРД (см. missed firerange — невалидный Cannons.Type)
				if (CheckAttribute(rCannon, "FireRange")) range = stf(rCannon.FireRange);
				// LEO: даже при перезарядке — если заряженный снаряд НЕ ДОСТАЁТ на текущей дистанции, менять свободно
				// (залп не выстрелит, ничего не теряем). Иначе застревал: картечь(30%) -> зона 30..90% -> не стреляет.
				float fCurReach = 1.0; // ядра достают всю дальность
				if (iCurrentBallType == GOOD_GRAPES)   { fCurReach = AMMO_REACH_GRAPE; }
				if (iCurrentBallType == GOOD_BOMBS)     { fCurReach = AMMO_REACH_BOMBS; }
				if (iCurrentBallType == GOOD_KNIPPELS)  { fCurReach = AMMO_REACH_KNIPPEL; }
				// LEO: заряжена КАРТЕЧЬ, а у цели команда уже на минимуме - выбивать нечего (ниже минимума
				//   она не падает). Перезаряжаемся, запретив картечь, иначе корабль до конца боя долбит
				//   пустую палубу. Проверка отдельным флагом: в Storm сложное условие безопаснее вложенным if.
				bool bGrapeWasted = false;
				if (iCurrentBallType == GOOD_GRAPES)
				{
					if (CheckAttribute(rCharacter, "SeaAI.Task.Target") && rCharacter.SeaAI.Task.Target != "")
					{
						int iTgt = sti(rCharacter.SeaAI.Task.Target);
						if (iTgt >= 0)
						{
							if (!LAi_IsDead(&Characters[iTgt]))
							{
								if (GetCrewQuantity(&Characters[iTgt]) <= makeint(GetMinCrewQuantity(&Characters[iTgt]) * GRAPE_CREW_MARGIN)) bGrapeWasted = true;
							}
						}
					}
				}
				if (bGrapeWasted) bGrapes = false;
				if (fMinEnemyDistance > (range * fCurReach) || bGrapeWasted) // не достаём или картечь впустую -> пересчёт
				{
					iNewBallType = Ship_FindOtherBallType(rCharacter, fMinEnemyDistance, bBalls, bBombs, bGrapes, bKnippels);
					if (iNewBallType >= 0 && iNewBallType != iCurrentBallType)
					{
						Ship_ChangeCharge(rCharacter, iNewBallType);
					}
				}
	    }
    }
	bool bIsCompanion = IsCompanion(rCharacter);
	bool bIsDefender = CheckAttribute(rCharacter, "fortDefender") && sti(rCharacter.fortDefender) == 1;
	Ship_CS_ShowActionIcon(rCharacter, rShipObject, aiTask, iCurrentBallType, bIsCompanion); // иконка заряд+задача над кораблём

	// проверка некоторых задач
	if (CheckAttribute(rCharacter, "SeaAI.Task") && CheckAttribute(rCharacter, "SeaAI.Task.Target"))
	{
		switch (sti(rCharacter.SeaAI.Task))
		{
			case AITASK_BRANDER:
				ref rCharacter2Brander = GetCharacter(sti(rCharacter.SeaAI.Task.Target));
				ref rBaseShip = GetRealShip(sti(rCharacter.Ship.Type));
				float fBranderDistance = 45.0 + (7.0 - stf(rBaseShip.Class)) * 15.0; // просьба масс
				fDistance = Ship_GetDistance2D(rCharacter, rCharacter2Brander); // boal 21.01.2004
				if (fBranderDistance > fDistance)
				{
					// поджигаем корабль (брандер)
					Ship_SetExplosion(rCharacter, rShipObject); //boal 27.09.05
					Log_Info("" + XI_ConvertString(rBaseShip.BaseName) + " '" + rCharacter.Ship.Name + "' " + GetShipSexWord(rBaseShip.BaseName, GetConvertStrWithReplace("Variable_AIShip_1", "Logs.txt", "#space#", " "), GetConvertStrWithReplace("Variable_AIShip_2", "Logs.txt", "#space#", " ")) + GetConvertStrWithReplace("Variable_AIShip_3", "Logs.txt", "#space#", " "));
					return;
				}
			break;
			case AITASK_ATTACK:
				//-->> если есть цель для Атаки, то оптимизируем оптимальное расстояние для стрельбы
				//только при полном боекомплекте на первом такте ожидания
				if (CheckAttribute(rCharacter, "Sea_AI.cannon.charge.lasttime") && sti(rCharacter.Sea_AI.cannon.charge.lasttime) == 1)
				{
					if (sti(rCharacter.Ship.SeaAI.Init.AttackDistance.qtyTryChangeCannon) == 2 || sti(rCharacter.Ship.SeaAI.Init.AttackDistance.qtyTryChangeCannon) == 5)
					{
						rCharacter.Ship.SeaAI.Init.AttackDistance = 50; //отрезаем неубегаек
						Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), sti(rCharacter.SeaAI.Task.Target));
					}
					else
					{
						float fAttackDist = Ship_GetDistance2D(rCharacter, &characters[sti(rCharacter.SeaAI.Task.Target)]);
						if (fAttackDist < 500)
						{
							rCharacter.Ship.SeaAI.Init.AttackDistance = fAttackDist;
							Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), sti(rCharacter.SeaAI.Task.Target));
						}
					}
				}
			break;
            // абордаж сам отработает
		}
	}

	// LEO: компаньон после абордажа/гибели цели не залипает — переназначаем (ближайший абордаж по правилам / атака / защита флагмана)
	if (bIsCompanion) Ship_Companion_Redecide_IfIdle(rCharacter);
	// LEO: АВТОНОМНЫЙ кайт (только если нет прямого приказа игрока) — не лезть в гущу после абордажа
	if (bIsCompanion) Ship_Companion_AutoKite(rCharacter);

	// boal AI компа -->
    if (!bIsCompanion) //fix 171204 boal Не нужно наших с толку сбивать
    {
        string sGroupID = Ship_GetGroupID(rCharacter);
		// LEO: ПОЗА ГРУППЫ 3b — сила своей группы против врагов в радиусе 1500 вокруг командира (локально). Ставит Posture/Engaged.
		Ship_Squad_UpdateGroupTactics(sGroupID);
		// LEO: пометка КОМАНДИРА эскадры для ЗВЕЗДЫ над баром (движок читает Ship.SeaAI.IsCommander).
		//      1 только если ведёт живую группу (>=2); при гибели флагмана метка переедет на нового командира.
		bool bMarkCmd = false;
		if (sGroupID != "" && sGroupID != PLAYER_GROUP)
		{
			if (Group_GetGroupCommanderIndex(sGroupID) == sti(rCharacter.index))
			{
				if (Group_GetLiveCharactersNum(sGroupID) >= 2) bMarkCmd = true;
			}
		}
		if (bMarkCmd) rCharacter.Ship.SeaAI.IsCommander = 1;
		else if (CheckAttribute(rCharacter, "Ship.SeaAI.IsCommander")) DeleteAttribute(rCharacter, "Ship.SeaAI.IsCommander");
		// ИИ сторожа форта обслуживается целиком в хелпере (ранний return)
		if (bIsDefender)
		{
			if (Ship_CS_DefenderAI(rCharacter, sGroupID)) return;
		}
		// LEO: ОСАДА — осадный корабль берёт ФОРТ в ATTACK (ранний return как у сторожей, ДО гейта «враг ГГ» -> работает для
		//   нейтральной к ГГ осады, AI-vs-AI). Флагман всегда; свита — когда рядом нет сторожей (иначе false -> бьёт сторожей).
		if (Ship_CS_SiegeAttack(rCharacter, sGroupID)) return;
		// LEO: СТРОЙ-УДЕРЖАНИЕ. Логин-атака залила SECONDARY=ATTACK, а DEFEND шёл в PRIMARY -> secondary перекрывал
		//      (GetCurrentTask=Secondary). HoldFormation пишет DEFEND в SECONDARY -> ведомый держит слот И стреляет от линии.
		//      Строй — режим ПОДХОДА: при латче боя / позе не-ENGAGE строй распускается в индивид. тактику. true -> пропускаем.
		if (Ship_Squad_HoldFormation(rCharacter, sGroupID)) return;
		if (CheckAttribute(rCharacter, "SeaAI.Task.Target"))
		{
			if (rCharacter.SeaAI.Task.Target != "") // пустую цель (труп очищен движком в "") пропускаем — переназначит тактика, не бежим от слота 0
			{
	        if (iNewBallType < 0 || iShipCannonsNum < (sti(rShip.CannonsQuantity) / 4))
			{
				// LEO: нечем стрелять (нет снаряда / выбито >3/4 пушек). Слабая эскадра (FLEE) / корпус<15% / цель=форт -> уходим;
				//   иначе НЕ бежим, а сближаемся на АБОРДАЖ (воюем командой). Раньше ВСЕГДА RUNAWAY — гнало здоровых в драп.
				//   Форт НЕ абордируется -> для цели=форт отход обязателен (иначе стоять под ядрами).
				bool bTgtFortFew = false;
				if (bIsFortAtIsland) { if (iFortCommander >= 0) { if (sti(rCharacter.SeaAI.Task.Target) == iFortCommander) bTgtFortFew = true; } }
				if (Ship_Squad_Posture(rCharacter) == SQUAD_POS_FLEE || GetHullPercent(rCharacter) < 15.0 || bTgtFortFew)
				{
					Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), sti(rCharacter.SeaAI.Task.Target)); // слабы/критический корпус/форт -> отходим
				}
				else
				{
					if (stf(rCharacter.Ship.SeaAI.Init.AttackDistance) > AIBEH_ABORD_CLOSE) rCharacter.Ship.SeaAI.Init.AttackDistance = AIBEH_ABORD_CLOSE;
					Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), sti(rCharacter.SeaAI.Task.Target)); // иначе сближаемся на абордаж
				}
				return;
			}
			}
		}
	// ПЕРЕМИРИЕ Speak (SpeakTruce): блок насильственной враждебности по нации пропускаем,
	// иначе откупленный корабль снова агрился через секунду после сделки.
		if (GetNationRelation2MainCharacter(sti(rCharacter.nation)) == RELATION_ENEMY && !CheckAttribute(rCharacter, "SpeakTruce")) // враг Гл героя
	    {
            if (CheckAttribute(rCharacter, "AlwaysFriend"))
	        {
	            SetCharacterRelationBoth(sti(rCharacter.index), GetMainCharacterIndex(), RELATION_FRIEND);
	        }
	        else
	        {
				//--> eddy. анализим форт. проверяем наличие форта, враждебность и залоченный таск
				Ship_CS_WatchFort(rCharacter);
				//<-- анализим форт
				if (!CheckAttribute(rCharacter, "SinkTenPercent")) // это не ЧЖ и не идущие на абордаж пираты
	            {
					Ship_CS_TacticalDecision(rCharacter, sGroupID);
		        }
		        else
		        {	// --> в целях экономии ресурсов АИ квест жемчужников суем сюда на обработку
		        	Ship_CS_PearlOrAttack(rCharacter);
		        }
	        }
	    }
	    else
	    {
		// ПЕРЕМИРИЕ (SpeakTruce): пока стоит ShipEnemyDisable — вражду НЕ пересчитываем (иначе AlwaysEnemy/пираты/нации агрят после откупа)
	        bool bSpeakTruce = CheckAttribute(rCharacter, "ShipEnemyDisable");
		// перемирие обслуживается ранним блоком в начале функции; далее насильственная враждебность для части НПС.
		// boal 030804: можно делать один раз и оно ПОМНИТ, но тогда нужно ИД проверять (перекрываем выше).
	        if (!bSpeakTruce && CheckAttribute(rCharacter, "AlwaysEnemy") && !CheckAttribute(rCharacter, "PGG_companion")) // Фикс агрессии эскадры ПГГ после взятия ПП в море
	        {
	            SetCharacterRelationBoth(sti(rCharacter.index), GetMainCharacterIndex(), RELATION_ENEMY);
	            Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), GetMainCharacterIndex());
	        }
	        else
	        {
                if (CheckAttribute(rCharacter, "AlwaysFriend"))
		        {
		            SetCharacterRelationBoth(sti(rCharacter.index), GetMainCharacterIndex(), RELATION_FRIEND);
		        }
		        else
		        {
					// фикс отношений
			        if (sti(rCharacter.nation) != PIRATE || bBettaTestMode)//boal 030804 нужно чтоб при тесте флаг с пиратами мирил
			        {
	                    if (!bSpeakTruce) SetCharacterRelationBoth(sti(rCharacter.index), GetMainCharacterIndex(), GetNationRelation2MainCharacter(sti(rCharacter.nation)));
						// проверка на ложный флаг (при перемирии не разнюхивают - bSpeakTruce) -->
						// самовоз казённого конвоя (сделка с агентом): корабли нации-адресата НЕ разоблачают маскировку ГГ - FortAgent_DeliverGraceNation
						Ship_CS_CheckFalseFlag(rCharacter, bSpeakTruce);
						// проверка на ложный флаг <--
						// --> eddy. квест мэра. АИ разыскиваемого пирата
						if (rCharacter.id == "MQPirate") Ship_CS_MQPirateQuest(rCharacter);

			        }
			        else
			        {
			            if (!bSpeakTruce) SetCharacterRelationBoth(sti(rCharacter.index), GetMainCharacterIndex(), RELATION_ENEMY);//на море пираты нападают всегда (при перемирии переговоров - не агрим)
			        }
		        }
	        }
	        // fix на нападение при условии, что нация дружеская, а с НПС мы всё равно враждуем <--

	        if (GetRelation(GetMainCharacterIndex(), sti(rCharacter.index)) == RELATION_ENEMY)
	        {
	            Group_SetTaskAttack(sGroupID, PLAYER_GROUP);
	            UpdateRelations(); // перенес от ниже, тк исходим из того, что изначально все друзья всё равно
	        }
	        else
	        {
		    // LEO: AI-vs-AI РАСПАД В БОЙ. Нейтральный/дружественный к ГГ корабль дерётся со СВОИМ врагом, не с ГГ:
		    //   поза не FLEE + рядом реальный враг-не-игрок -> атакуем ближайшего (индивид. распад строя). Форса вражды к ГГ нет.
	            if (Ship_Squad_Posture(rCharacter) != SQUAD_POS_FLEE)
	            {
	                int iAiEnemy = Ship_NearestReachableEnemy(rCharacter); // беглеца, кого НЕ догоню — не берём (не гоняемся впустую)
	                if (iAiEnemy >= 0)
	                {
	                    if (Ship_GetDistance2D(rCharacter, &characters[iAiEnemy]) < SQUAD_TAC_ENEMY_RADIUS)
	                    {
	                        bool bAlreadyAtk = false;
	                        if (CheckAttribute(rCharacter, "SeaAI.Task") && sti(rCharacter.SeaAI.Task) == AITASK_ATTACK)
	                        {
	                            if (CheckAttribute(rCharacter, "SeaAI.Task.Target") && rCharacter.SeaAI.Task.Target != "")
	                            {
	                                int iCmtT = sti(rCharacter.SeaAI.Task.Target);
	                                if (iCmtT == iAiEnemy) bAlreadyAtk = true;
								// LEO: КОММИТ ЦЕЛИ (анти-фликер сторож_A<->сторож_B в AI-vs-AI осаде) — не флипать на чуть-ближе
								//   подошедшего другого сторожа, пока текущая цель жива/враг/в радиусе и не форт.
	                                if (iCmtT >= 0)
	                                {
	                                    if (iCmtT != iFortCommander)
	                                    {
	                                        if (!LAi_IsDead(&characters[iCmtT]))
	                                        {
	                                            if (GetRelation(sti(rCharacter.index), iCmtT) == RELATION_ENEMY)
	                                            {
	                                                if (Ship_GetDistance2D(rCharacter, &characters[iCmtT]) < SQUAD_TAC_ENEMY_RADIUS) bAlreadyAtk = true;
	                                            }
	                                        }
	                                    }
	                                }
	                            }
	                        }
	                        if (!bAlreadyAtk) Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iAiEnemy);
	                    }
	                }
	                else
	                {
					// LEO: досягаемого врага НЕТ, боевой таск завис без цели -> не стоять. Ведомый -> в строй к живому флагману (DEFEND);
					//   командир/одиночка -> RUNAWAY. Ключ на ЛАТЧ боя ГРУППЫ (rGrp.Engaged, переживает флип таска), НЕ на SeaAI.Task
					//   (ваниль уже перебила в MOVE). Идём ПОСЛЕ UpdateGroupTactics -> последний писатель, перебиваем ванильный MOVE.
	                    bool bWasFighting = false;
	                    bool bNoEnemy = true; // сдаём бой ТОЛЬКО когда живых врагов нет; жив-но-недогоняем -> НЕ сдаём (агрессор гонится)
	                    if (sGroupID != "" && sGroupID != PLAYER_GROUP)
	                    {
	                        ref rGE = Group_GetGroupByID(sGroupID);
	                        if (CheckAttribute(rGE, "Engaged")) { if (sti(rGE.Engaged) == 1) bWasFighting = true; }
	                        if (CheckAttribute(rGE, "HasEnemy")) { if (sti(rGE.HasEnemy) == 1) bNoEnemy = false; }
	                    }
	                    // не долбить каждый кадр: если уже стоит свежий DEFEND/RUNAWAY — не переустанавливаем (иначе трэш/пере-подъём парусов)
	                    bool bStaleTask = true;
	                    if (CheckAttribute(rCharacter, "SeaAI.Task"))
	                    {
	                        int iCurT = sti(rCharacter.SeaAI.Task);
	                        if (iCurT == AITASK_DEFEND || iCurT == AITASK_RUNAWAY) bStaleTask = false;
	                    }
	                    if (bWasFighting && bStaleTask && bNoEnemy)
	                    {
	                        int iCmdIdx = Group_GetGroupCommanderIndex(sGroupID);
	                        if (iCmdIdx >= 0 && iCmdIdx != sti(rCharacter.index) && !LAi_IsDead(&characters[iCmdIdx]))
	                            Ship_SetTaskDefend(SECONDARY_TASK, sti(rCharacter.index), iCmdIdx); // ведомый -> в строй к флагману
	                        else
	                        {
	                            Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), -1);     // командир/одиночка -> уходит
	                            rCharacter.SeaAI.Task.Target = ""; // LEO: -1 в Target палит GetCharacter(-1) (строка непустая, проходит гварды !=""); пустая = «нет цели», движок RUNAWAY читает только aref
	                        }
	                    }
	                }
	            }
	        }
	    }
	    // AI компа <--
    }
}

// проверка флага НПС у ГГ, мож он пират?
void Ship_CheckFlagEnemy(ref rCharacter)
{
	ref     mChar = GetMainCharacter();
	int     iCompan, iClass;
	float   fSneak;
	int     i, cn;
	ref     chref;
	bool    bViewFlag = false;
	int     iNationToChange = sti(mChar.nation);  // было
	bool    bTrFort;  string sTrDist, sTrWhy; // LEO-TRACE (объявления в шапке - Storm не любит в nested-блоке)

    if (sti(rCharacter.nation) == PIRATE) return;
    iCompan = GetCompanionQuantity(mChar);
    iClass  = 7;
	// поиск мин.  те старшего класса
	for (i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(GetMainCharacter(), i);
		if (cn>=0)
		{
			chref = GetCharacter(cn);
            if (GetCharacterShipClass(chref) < iClass) iClass = GetCharacterShipClass(chref);
		}
	}
    if (GetNationRelation(GetBaseHeroNation(), sti(rCharacter.nation)) == RELATION_ENEMY && GetDaysContinueNationLicence(sti(rCharacter.nation)) == -1) // проверка грамот
    {
		iNationToChange = GetBaseHeroNation();
        bViewFlag       = true;
    }
    if (!bViewFlag)
    {
        if (ChangeCharacterHunterScore(mChar, NationShortName(sti(rCharacter.nation)) + "hunter", 0) > (8 + rand( (iClass-1)*(9-iCompan) ) ))
        {
            bViewFlag       = true;
            iNationToChange = PIRATE;
        }
    }
    if (bViewFlag)
    {  // смотрим флаг у ГГ и узнаем в нём...
		// LEO-TRACE (диагностика «опыт скрытности из ниоткуда»): кто/почему триггерит проверку флага + начисление.
		bTrFort = CheckAttribute(rCharacter, "Fort");
		if (bTrFort) sTrDist = "fort"; else sTrDist = "" + makeint(Ship_GetDistance2D(mChar, rCharacter));
		if (iNationToChange == GetBaseHeroNation()) sTrWhy = "nation-enemy"; else sTrWhy = "hunter-score";
		Trace("LEOTRACE Sneak: CHECK id=" + rCharacter.id + " idx=" + sti(rCharacter.index) + " nation=" + sti(rCharacter.nation) + " isFort=" + bTrFort + " dist=" + sTrDist + " baseNat=" + GetBaseHeroNation() + " curFlag=" + sti(mChar.nation) + " why=" + sTrWhy + " day=" + sti(pchar.questTemp.stels.sea) + "/" + GetDataDay());

		fSneak  = stf(mChar.TmpSkill.Sneak); // 0.01..1.0
		int rep = sti(abs(REPUTATION_NEUTRAL - sti(mChar.reputation)) * 0.75);
		int losechance = rand(100) + rand(20) + rand(rep) - GetCharacterSPECIALSimple(pchar, SPECIAL_L)*2;
		int winchance = makeint(fSneak * 10 * iClass * (9-iCompan));
		Log_TestInfo(GetConvertStrWithReplace("Variable_AIShip_12", "Logs.txt", "#space#", " ")+losechance+"/Текущее значение: "+winchance);
		if (losechance > winchance)
		{
			Log_Info(GetConvertStrWithReplace("Variable_AIShip_13", "Logs.txt", "#space#", " ")+ NationNamePeople(sti(rCharacter.nation)) + GetConvertStrWithReplace("Variable_AIShip_14", "Logs.txt", "#space#", " "));
			SetCharacterRelationBoth(sti(rCharacter.index), GetMainCharacterIndex(), RELATION_ENEMY);
			DoQuestCheckDelay(NationShortName(iNationToChange) + "_flag_rise", 0.1); // применение нац отношений флага
			if (!CheckAttribute(pchar,"questTemp.stels.sea")) pchar.questTemp.stels.sea = GetDataDay()-1;
			if (sti(pchar.questTemp.stels.sea) != GetDataDay())
			{
				AddCharacterExpToSkill(mChar, SKILL_SNEAK, 50);
				Trace("LEOTRACE Sneak: +50 (УЗНАЛИ, стал враждебен) id=" + rCharacter.id + " nation=" + sti(rCharacter.nation) + " isFort=" + CheckAttribute(rCharacter, "Fort")); // LEO-TRACE
				pchar.questTemp.stels.sea = GetDataDay();
			}
		}
		else
		{ // не узнал
			if (!CheckAttribute(pchar,"questTemp.stels.sea")) pchar.questTemp.stels.sea = GetDataDay()-1;
			if (sti(pchar.questTemp.stels.sea) != GetDataDay())
			{
				AddCharacterExpToSkill(mChar, SKILL_SNEAK, (iCompan * 200 / iClass));
				Trace("LEOTRACE Sneak: +" + (iCompan * 200 / iClass) + " (ОБМАНУЛИ, не узнали) id=" + rCharacter.id + " nation=" + sti(rCharacter.nation) + " isFort=" + CheckAttribute(rCharacter, "Fort") + " iCompan=" + iCompan + " iClass=" + iClass); // LEO-TRACE
				ChangeCrewExp(mChar, "Sailors", AIShip_isPerksUse(CheckCharacterPerk(mChar, "SeaWolf"), 0.5, 1.0));
				pchar.questTemp.stels.sea = GetDataDay();
			}
		}
	}
}

// ============================================================================
//   ПОЖАР/ДЕТОНАЦИЯ БРАНДЕРА, ДИСТАНЦИИ, ВЫБОР БОЕПРИПАСА,
//   СТОЛКНОВЕНИЯ, ПРИМЕНЕНИЕ УРОНА (команда/корпус)
// ============================================================================
void Ship_ActivateFirePlace()
{
	aref	arShipObject = GetEventData();
	aref	arCharacter = GetEventData();
	int		iFirePlaceIndex = GetEventData();
	string	sSoundName = GetEventData();
	float	fFireTime = GetEventData();
	int		iBallCharacterIndex = GetEventData();

	if (!bAbordageStarted) // fix
	{
		if (sti(InterfaceStates.OtherParticles)) SendMessage(arShipObject, "llsssfl", MSG_SHIP_ACTIVATE_FIRE_PLACE, iFirePlaceIndex, "", "ship_fire_ext", sSoundName, fFireTime, iBallCharacterIndex);
		else SendMessage(arShipObject, "llsssfl", MSG_SHIP_ACTIVATE_FIRE_PLACE, iFirePlaceIndex, "", "ship_fire", sSoundName, fFireTime, iBallCharacterIndex);
		if (arCharacter.index == pchar.index) 
		{
			if (!CheckAttribute(pchar, "FireonShipSound")) pchar.FireonShipSound = 0;
			if (!SoundIsPlaying(sti(pchar.FireonShipSound))) pchar.FireonShipSound = PlayVoice("Kopcapkz\SoundFX\SBVF_"+rand(3)+".mp3"); // LEO: Оповещение о пожаре на корабле
		}
	}
}

void Ship_BranderDetonate()
{
	ref		rCharacter = GetCharacter(GetEventData());

	// LEO: фикс — отложенная детонация (ловушка приза, 3-10с) могла остаться на переиспользованном фантом-индексе без корабля -> null в обработчике смерти (ошибки 2641).
	if (!CheckAttribute(rCharacter, "Ship.Type")) return;
	if (sti(rCharacter.Ship.Type) == SHIP_NOTUSED) return;

	ref		rBranderBaseShip = GetRealShip(sti(rCharacter.Ship.Type));
 	float	fBranderDistance = 1.9 * (45.0 + (7.0 - stf(rBranderBaseShip.Class)) * 15.0); //fix
 	fBranderDistance *= fBranderDistance;
	float   fMaxDamage = 1000.0 + MakeFloat(GetCargoGoods(rCharacter, GOOD_POWDER)) * 4;

	aref	arShipObjects[64];
	int		iBranderNumShips = 1;

	if(CheckAttribute(rCharacter,"Situation") && rCharacter.situation == "Explosion" ) // ситуация со взрывом
	{
		fMaxDamage = 1000.0 + MakeFloat(GetCargoGoods(rCharacter, GOOD_POWDER)) * 3;
		if(GetCargoGoods(rCharacter, GOOD_POWDER) < 300) { fMaxDamage += 400.0; }
		ref mc = GetMainCharacter();
		int iDiffClass = sti(RealShips[sti(mc.Ship.Type)].Class) - sti(RealShips[sti(rCharacter.Ship.Type)].Class);
		if(iDiffClass > 0) fMaxDamage += iDiffClass * 500.0;
	}
	if (!FindEntity(&arShipObjects[0], "ship")) { return; }// can't be
	while (iBranderNumShips < 64 && FindEntityNext(&arShipObjects[iBranderNumShips])) { iBranderNumShips++; }
	// перебор кораблей и нанесение урона
	iBranderNumShips = i_min(63, iBranderNumShips);
	for (int i=0; i<iBranderNumShips; i++)
	{
		int iCharacterIndex = -1;
		SendMessage(arShipObjects[i], "le", MSG_SHIP_GET_CHARACTER_INDEX, &iCharacterIndex);
		if (iCharacterIndex == sti(rCharacter.index)) { continue; }
		ref rOtherCharacter = GetCharacter(iCharacterIndex);
		if (Boarding_DamageBlocked(rOtherCharacter, sti(rCharacter.index))) continue;
		ref	rOtherBaseShip = GetRealShip(sti(rOtherCharacter.Ship.Type));
		float fDistance = Ship_GetDistance2DRel(rCharacter, rOtherCharacter);
		if (fDistance > fBranderDistance) { continue; }
		float fDamageMultiply = Bring2Range(1.0, 0.0, 10.0, sqrt(fBranderDistance), sqrt(fDistance));
		float fBranderDmg = fMaxDamage * fDamageMultiply;
		Ship_ApplyHullHitpoints(rOtherCharacter, fBranderDmg, KILL_BY_SPY, sti(rCharacter.index));

		// LEO: всплывающий урон над каждой целью, задетой взрывом брандера (как в Ship_HullHitEvent). Цель-игрок — под опцией ShipCapShowDamageSelf, прочие — ShipCapShowDamage.
		if (makeint(fBranderDmg) >= 1)
		{
			bool bShowBranderDmg = false;
			if (iCharacterIndex == nMainCharacterIndex) { if (sti(InterfaceStates.ShipCapShowDamageSelf) == 1) bShowBranderDmg = true; }
			else { if (sti(InterfaceStates.ShipCapShowDamage) == 1) bShowBranderDmg = true; }
			if (bShowBranderDmg)
			{
				float bpx = stf(rOtherCharacter.Ship.Pos.x);
				float bpy = stf(rOtherCharacter.Ship.Pos.y);
				float bpz = stf(rOtherCharacter.Ship.Pos.z);
				SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, bpx, bpy, bpz, "-" + its(makeint(fBranderDmg)), argb(255, 255, 60, 60), 3.0, GetShipCapNotifDistance());
			}
		}

		// добавляем кораблю импульс движения
		float x = stf(rOtherCharacter.Ship.Pos.x) - stf(rCharacter.Ship.Pos.x);
		float z = stf(rOtherCharacter.Ship.Pos.z) - stf(rCharacter.Ship.Pos.z);
		float fLen = stf(rOtherBaseShip.Class) * fDamageMultiply * (1.0 / sqrt(x * x + z * z));
		SendMessage(&arShipObjects[i], "llffffff", MSG_SHIP_ADD_MOVE_IMPULSE, false, x * fLen, 0.0, z * fLen, 0.0, 0.0, 0.0);
	}

	Ship_Detonate(rCharacter, true, true);
}

float Ship_GetDistance2D(ref rCharacter1, ref rCharacter2)
{
	if (CheckAttribute(rCharacter1, "Ship.Pos.x") && CheckAttribute(rCharacter2, "Ship.Pos.x")) // fix boal
	{
		return GetDistance2D(stf(rCharacter1.Ship.Pos.x), stf(rCharacter1.Ship.Pos.z), stf(rCharacter2.Ship.Pos.x), stf(rCharacter2.Ship.Pos.z));
	}
	else
	{
		return 1000.0;
	}
}

// без лишнего sqrt
float Ship_GetDistance2DRel(ref rCharacter1, ref rCharacter2)
{
	if (CheckAttribute(rCharacter1, "Ship.Pos.x") && CheckAttribute(rCharacter2, "Ship.Pos.x")) // fix
	{
		return GetDistance2DRel(stf(rCharacter1.Ship.Pos.x), stf(rCharacter1.Ship.Pos.z), stf(rCharacter2.Ship.Pos.x), stf(rCharacter2.Ship.Pos.z));
	}
	else
	{
		return 1000000.0;
	}
}

int Ship_FindOtherBallType(ref rCharacter, float fMinEnemyDistance, bool bBalls, bool bBombs, bool bGrapes, bool bKnippels)
{
	ref	rCannon = GetCannonByType(sti(rCharacter.Ship.Cannons.Type));
	float range = 500; // LEO: дефолт + ГВАРД — у кораблей с кривым/пустым Cannons.Type GetCannonByType даёт невалидный реф ("no rAP data" / спам missed firerange). Пер-шип агрессия вскрыла давнюю дыру.
	if (CheckAttribute(rCannon, "FireRange")) range = stf(rCannon.FireRange);

	// нет валидной цели -> держим дальнобойный заряд, спец-дистанцию не трогаем
	if (!CheckAttribute(rCharacter, "SeaAI.task.target") || rCharacter.SeaAI.task.target == "")
	{
		if (bBalls) return GOOD_BALLS;
		if (bBombs) return GOOD_BOMBS;
		if (bKnippels) return GOOD_KNIPPELS;
		if (bGrapes) return GOOD_GRAPES;
		return -1;
	}

	int enemy = sti(rCharacter.SeaAI.task.target);
	ref rBaseShip = GetRealShip(GetCharacterShipType(rCharacter));
		// LEO: ВЫБОР СНАРЯДА — ванильный, ПРОГРЕССИВНЫЙ ПО ДИСТАНЦИИ. Дальше — ЯДРА; ближе — КНИППЕЛЬ(паруса)/БОМБЫ(корпус)/КАРТЕЧЬ(команда).
		//   fMod связывает дистанцию удержания со снарядом. ФОРТ: только ядра (короткие тянут вплотную), нет ядер -> бомбы фолбэком.
	bool bEnemySails = false;
	if (sti(characters[enemy].ship.sp) > 30) { if (ShipSailState(enemy) >= 0.5) bEnemySails = true; } // 0 нет / 1 боевой / 2 полный
	// LEO: команду цели меряем МИНИМУМОМ ЕЁ корабля, а не своего (rBaseShip - корабль стрелка): линкор
	// не брал картечь по шлюпу, а шлюп лупил ею по фрегату с выбитой командой. Запас 1.2 - ниже минимума
	// команда не падает вовсе (кламп в Ship_ApplyCrewHitpoints), у самого порога выбивать уже нечего.
	bool bEnemyCrew = false;
	if (sti(characters[enemy].ship.crew.quantity) > makeint(GetMinCrewQuantity(&characters[enemy]) * GRAPE_CREW_MARGIN)) bEnemyCrew = true;

	bool bTgtFort = false;
	if (bIsFortAtIsland) { if (iFortCommander >= 0) { if (enemy == iFortCommander) bTgtFort = true; } }
	int iType = -1;
	float fMod = 1.0;
	if (bTgtFort)
	{
		if (bBalls) iType = GOOD_BALLS;
		else if (bBombs) iType = GOOD_BOMBS; // нет ядер -> бомбы дальнобойным фолбэком
	}
	else
	{
		if (bBalls) iType = GOOD_BALLS; // дальше всего -> ядра по умолчанию
		if ((fMinEnemyDistance <= range * AMMO_REACH_KNIPPEL) && bKnippels)
		{
			if (bEnemySails) iType = GOOD_KNIPPELS; // враг под парусами -> книппель
			fMod = AMMO_REACH_KNIPPEL;
		}
		if ((fMinEnemyDistance <= range * AMMO_REACH_BOMBS) && bBombs)
		{
			iType = GOOD_BOMBS; // средняя -> бомбы по корпусу
			fMod = AMMO_REACH_BOMBS;
		}
		if ((fMinEnemyDistance <= range * AMMO_REACH_GRAPE) && bGrapes)
		{
			if (bEnemyCrew) iType = GOOD_GRAPES; // вплотную + есть команда -> картечь
			fMod = AMMO_REACH_GRAPE;
		}
		if (iType < 0) // нет ядер -> любой доступный (дальнобойный вперёд)
		{
			if (bBombs)         iType = GOOD_BOMBS;
			else if (bKnippels) iType = GOOD_KNIPPELS;
			else if (bGrapes)   iType = GOOD_GRAPES;
		}
	}

	// LEO: НЕ стрелять = снарядов нет (iType<0) ИЛИ нет ПОРОХА -> сближаемся на абордаж (сохранённая фича out-of-ammo->board).
	bool bNoFire = false;
	if (iType < 0) bNoFire = true;
	if (GetCargoGoods(rCharacter, GOOD_POWDER) < GetCannonsNum(rCharacter)) bNoFire = true;
	if (bNoFire)
	{
		rCharacter.Ship.SeaAI.Init.AttackDistance = AIBEH_ABORD_CLOSE;
		return iType;
	}
	float fHold = Ship_AIHoldFraction(rCharacter, fMod);
	// LEO: КОМПАНЬОН — не нырять в ГУЩУ за уводящей целью: при скоплении врагов вокруг цели/себя держим МАКС дальность
	//   (COMPANION_EDGE_HOLD), кайтим по краю. Цель НЕ меняем — работает и под прямым приказом атаки.
	if (IsCompanion(rCharacter))
	{
		bool bInSwarm = false;
		if (Ship_Companion_SwarmAtTarget(rCharacter, enemy, COMPANION_SWARM_RADIUS) >= COMPANION_SWARM_N) bInSwarm = true;
		if (Ship_Companion_ThreatCount(rCharacter, COMPANION_SWARM_RADIUS) >= COMPANION_SWARM_N) bInSwarm = true;
		if (bInSwarm) { if (fHold < COMPANION_EDGE_HOLD) fHold = COMPANION_EDGE_HOLD; }
	}
	rCharacter.Ship.SeaAI.Init.AttackDistance = range * fHold;
	return iType;
}

// событие: у ГГ не хватает снарядов для стрельбы
void Ship_NotEnoughBalls() // FPSTODO: зачем вызывается так часто?
{
	bNotEnoughBalls = GetEventData();
	if (!bNotEnoughBalls)
	{
	    if (GetCargoGoods(pchar, GOOD_POWDER) < 1)
	    {
	        bNotEnoughBalls = true;
	    }
	}

}

int Ship_GetCurrentBallsNum() // FPSTODO: вызывается каждый кадр, юзать кэш, прибавка минимальна
{
	aref aCharacter = GetEventData();
	return GetCargoGoods(aCharacter,sti(aCharacter.Ship.Cannons.Charge.Type));
}

void Ship_SailDamage()
{
	float x, y, z;
	int iCharacterIndex = GetEventData();
	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	string sSound;
	int	iBallType = sti(AIBalls.CurrentBallType);
	switch (iBallType)
	{
		case GOOD_BALLS:	sSound = "ball2sail";	break;
		case GOOD_GRAPES:	sSound = "grapes2sail";	break;
		case GOOD_KNIPPELS:	sSound = "knippel2sail";	break;
		case GOOD_BOMBS:	sSound = "ball2sail"; break;
	}

	Play3DSound(sSound, x, y, z);
	if (rand(100) < 15) { Play3DSound("fly_ball_misc", x, y, z); }
}

void Ship_Ship2IslandCollision()
{
	float	fPower, fSlide, x, y, z;
	int		iOurCharacterIndex, iEnemyCharacterIndex, iTouchPoint, prevTask;

	iOurCharacterIndex = GetEventData();
	iEnemyCharacterIndex = GetEventData();
	fPower = abs(GetEventData());
	fSlide = abs(GetEventData());
	iTouchPoint = GetEventData();
	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	ref rOurCharacter = GetCharacter(iOurCharacterIndex);

	if (iOurCharacterIndex != GetMainCharacterIndex())
	{
		Ship_SetTaskDrift(SECONDARY_TASK, iOurCharacterIndex);
		rOurCharacter.TmpPerks.Turn = true;
		rOurCharacter.TmpPerks.Turn.Angle = PI/1.3;//корабли будут пытаться развернуться от острова
	}

    if (!sti(rOurCharacter.TmpPerks.SandbankManeuver) && !CheckAttribute(rOurCharacter, "AlwaysSandbankManeuver"))  //boal
    {
		float fHP = (1.0 - fSlide) * fPower * 3.0;
		Ship_ApplyHullHitpoints(rOurCharacter, fHP, KILL_BY_ISLAND, -1);

		if (fPower > 1.0)  Play3DSound("coll_ship2rock", x, y, z);
	}
	if (iOurCharacterIndex != GetMainCharacterIndex() && sti(rOurCharacter.SeaAI.Task)==AITASK_DRIFT)
	{
		Ship_SetTaskAttack(PRIMARY_TASK, sti(iOurCharacterIndex), GetMainCharacterIndex());//и вернуться к атаке; переделал Lipsar
	}
}

float Ship_GetAttackHP(int iCharacterIndex, float fDistance)
{
	float fAttackHP = 0.0;
	if (bSeaActive)
	{
		SendMessage(&AISea, "lafe", AI_MESSAGE_SHIP_GET_ATTACK_HP, &Characters[iCharacterIndex], fDistance, &fAttackHP);
	}
	return fAttackHP;
}

float Ship_GetHP(ref rCharacter)
{
	return stf(rCharacter.Ship.HP);
}

void Ship_Ship2ShipCollision()
{
	int		iOurCharacterIndex, iEnemyCharacterIndex, iTouchPoint;
	float	fPower, fSlide, x, y, z;

	iOurCharacterIndex = GetEventData();   // кого
	iEnemyCharacterIndex = GetEventData();  // кто
	fPower = abs(GetEventData());
	fSlide = abs(GetEventData());
	iTouchPoint = GetEventData();
	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	ref rOurCharacter = GetCharacter(iOurCharacterIndex);
	if (Boarding_DamageBlocked(rOurCharacter, iEnemyCharacterIndex)) return;
	if (Boarding_Busy(rOurCharacter)) return; // LEO: Не засчитываем физический контакт сцепленных бортов как таран.

	// LEO: столкновение между своими (флот ГГ: флагман+свита) урона корпусу не наносит — в строю касаются, а избегание своих отключено. Врагам урон как обычно.
	bool bBothOurs = false;
	if (iEnemyCharacterIndex >= 0)
	{
		bool bOurA = false;
		if (sti(rOurCharacter.index) == GetMainCharacterIndex()) bOurA = true;
		if (IsCompanion(rOurCharacter)) bOurA = true;
		bool bOurB = false;
		if (iEnemyCharacterIndex == GetMainCharacterIndex()) bOurB = true;
		if (!bOurB) { if (IsCompanion(GetCharacter(iEnemyCharacterIndex))) bOurB = true; }
		if (bOurA) { if (bOurB) bBothOurs = true; }
	}

	float fHP = (1.0 - fSlide) * fPower * 1.0;
	// ПЕРЕМИРИЕ Speak: таран ГГ/компаньонов кораблю под перемирием урона не наносит (seaspeak.c)
	if (!bBothOurs)
	{
		if (!SpeakTruce_HitImmune(rOurCharacter, iEnemyCharacterIndex)) Ship_ApplyHullHitpoints(rOurCharacter, fHP, KILL_BY_TOUCH, iEnemyCharacterIndex);
	}
	// зашита от потопления тараном -->
	if (iEnemyCharacterIndex == GetMainCharacterIndex() && GetHullPercent(rOurCharacter) <= 14  && !LAi_IsDead(rOurCharacter))
	{
		if (!IsCompanion(rOurCharacter) && GetRelation(sti(rOurCharacter.index), GetMainCharacterIndex()) != RELATION_ENEMY)
		{
			Ship_NationAgressive(rOurCharacter, rOurCharacter);
			NationUpdate();
	    	DoQuestCheckDelay("NationUpdate", 0.9);
    	}
	}
	// зашита от потопления тараном <--

	if (fPower>1.0) { Play3DSound("coll_ship2ship", x, y, z); }
}

void Ship_ApplyCrewHitpoints(ref rOurCharacter, float fCrewHP)
{         // че-то распук метод "неподецки" - переделал 29.07.06 boal
	if (Boarding_Busy(rOurCharacter)) return;
	if (LAi_IsImmortal(rOurCharacter))
	{
		return;
	}
    if (MOD_SKILL_ENEMY_RATE == 1 && sti(rOurCharacter.index) == GetMainCharacterIndex())
	{
		fCrewHP = fCrewHP / MOD_Complexity_1_DMG;
	}
	ref rBaseShip = GetRealShip(GetCharacterShipType(rOurCharacter));
	float fMultiply = 1.0 - (0.75 * stf(rOurCharacter.TmpSkill.Defence)); // было 0.05 - что полная хрень, тк скил 0..1

	if(CheckOfficersPerk(rOurCharacter, "Doctor2"))
	{
		fMultiply = fMultiply * 0.8;
	}
	else
	{
		if(CheckOfficersPerk(rOurCharacter, "Doctor1"))
		{
			fMultiply = fMultiply * 0.9;
		}
	}
	if (GetCharacterItem(rOurCharacter, "SkullAztec"))
	{
		fMultiply = fMultiply * 0.8;
	}
	// Gregg: оберег Мухачи («Две сестры») - те же -10% потерь команды, что и в морском
	// расчёте урона; здесь вторая ветка, через которую урон в команду тоже проходит.
	if (GetCharacterItem(rOurCharacter, "MuhachiAmulet"))
	{
		fMultiply = fMultiply * 0.9;
	}
	float fDamage = fCrewHP * fMultiply;

	float fNewCrewQuantity = stf(rOurCharacter.Ship.Crew.Quantity) - fDamage;
	float f5Percent = stf(rBaseShip.MinCrew) * 1.0; //WW boal fix неубиваемые 25 процентов команды- было 0.05;
	if (fNewCrewQuantity >= f5Percent && IsCompanion(rOurCharacter))
	{
       AddCharacterExpToSkill(rOurCharacter, "Defence", makefloat(fCrewHP / 2 + 0.5));
       ChangeCrewExp(rOurCharacter, "Soldiers",  (fCrewHP / stf(rBaseShip.OptCrew))); // to_do может быть много
    }

	if (fNewCrewQuantity < f5Percent) { fNewCrewQuantity = f5Percent; }
	if (fNewCrewQuantity < 0.0) { fNewCrewQuantity = 0.0; }

	if (sti(rOurCharacter.index) == GetMainCharacterIndex() && stf(rBaseShip.MinCrew) > fNewCrewQuantity && stf(rBaseShip.MinCrew) <= stf(rOurCharacter.Ship.Crew.Quantity))
    {  // переход на мин команду
        PlayVoice("interface\"+LanguageGetLanguage()+"_Damage1.wav");
    }
	// boal fix утопленников   -->
	if (stf(rOurCharacter.Ship.Crew.Quantity) > fNewCrewQuantity)
	{   // на случай косяков со знаком fCrewHP
		if (sti(rOurCharacter.index) == GetMainCharacterIndex() || IsCompanion(rOurCharacter))
		{
			Statistic_AddValue(GetMainCharacter(), "Sailors_dead", stf(rOurCharacter.Ship.Crew.Quantity) - fNewCrewQuantity);
		}
		// LEO: РАНЕНЫЕ — часть реально выбывших спасаем в раненых по навыку Защиты (все корабли ИИ)
		Crew_SkimWounded(rOurCharacter, makeint(stf(rOurCharacter.Ship.Crew.Quantity)) - makeint(fNewCrewQuantity)); // LEO: killed = разница ЦЕЛЫХ (реальное уменьшение видимой команды), иначе makeint(дробной потери 0.х)=0 и раненые не копятся при малом уроне
		rOurCharacter.Ship.Crew.Quantity = fNewCrewQuantity;
	}
	if (stf(rOurCharacter.Ship.Crew.Quantity) < 0.0) { rOurCharacter.Ship.Crew.Quantity = 0.0; }
	// boal fix утопленников   <--
}

void Ship_ApplyCrewHitpointsWithCannon(ref rOurCharacter, int iShipOurCharacter, float fCrewHP, int iBallType, ref rBaseShip)
{         // че-то распук метод "неподецки" - переделал 29.07.06 boal
	if (LAi_IsImmortal(rOurCharacter))	{ 
		return;
	}
    if (MOD_SKILL_ENEMY_RATE == 1 && iShipOurCharacter == GetMainCharacterIndex())	{
		fCrewHP = fCrewHP / MOD_Complexity_1_DMG;
	}

	float fDamage = fCrewHP * (Random()*0.4+0.8);//временный рандом до переделки морали

	float fCrewQuantity = stf(rOurCharacter.Ship.Crew.Quantity);
	float fOptCrew = stf(rBaseShip.OptCrew);
	float f5Percent = stf(rBaseShip.MinCrew); //WW boal fix неубиваемые 25 процентов команды- было 0.05;
	if ((iBallType == GOOD_GRAPES) && (fDamage < 0.5)) {fDamage = Random();}//имитация огня по палубе/в пушки - а то совсем жирно будет
	fDamage = fDamage * fCrewQuantity/fOptCrew;//скейл урона от плотности матросни на корабле
	float fNewCrewQuantity = fCrewQuantity - fDamage;

	if (fNewCrewQuantity >= f5Percent && IsCompanion(rOurCharacter))	{
       AddCharacterExpToSkill(rOurCharacter, "Defence", makefloat(fCrewHP / 2 + 0.5));
       ChangeCrewExp(rOurCharacter, "Soldiers",  (fCrewHP / stf(rBaseShip.OptCrew))); // to_do может быть много
    }

	if (fNewCrewQuantity < f5Percent) { fNewCrewQuantity = f5Percent; }
	if (fNewCrewQuantity < 0.0) { fNewCrewQuantity = 0.0; }

	if (sti(rOurCharacter.index) == GetMainCharacterIndex() && stf(rBaseShip.MinCrew) > fNewCrewQuantity && stf(rBaseShip.MinCrew) <= stf(rOurCharacter.Ship.Crew.Quantity))
    {  // переход на мин команду
        PlayVoice("interface\"+LanguageGetLanguage()+"_Damage1.wav");
    }
	// boal fix утопленников   -->
	if (stf(rOurCharacter.Ship.Crew.Quantity) > fNewCrewQuantity)
	{   // на случай косяков со знаком fCrewHP
		if (sti(rOurCharacter.index) == GetMainCharacterIndex() || IsCompanion(rOurCharacter))
		{
			Statistic_AddValue(GetMainCharacter(), "Sailors_dead", stf(rOurCharacter.Ship.Crew.Quantity) - fNewCrewQuantity);
		}
		// LEO: РАНЕНЫЕ — часть реально выбывших спасаем в раненых по навыку Защиты (все корабли ИИ)
		Crew_SkimWounded(rOurCharacter, makeint(stf(rOurCharacter.Ship.Crew.Quantity)) - makeint(fNewCrewQuantity)); // LEO: killed = разница ЦЕЛЫХ (реальное уменьшение видимой команды), иначе makeint(дробной потери 0.х)=0 и раненые не копятся при малом уроне
		rOurCharacter.Ship.Crew.Quantity = fNewCrewQuantity;
	}
	if (stf(rOurCharacter.Ship.Crew.Quantity) < 0.0) { rOurCharacter.Ship.Crew.Quantity = 0.0; }
	// boal fix утопленников   <--
}

void Ship_ApplyHullHitpoints(ref rOurCharacter, float fHP, int iKillStatus, int iKillerCharacterIndex)
{
	if (Boarding_DamageBlocked(rOurCharacter, iKillerCharacterIndex)) return;
   	if (LAi_IsImmortal(rOurCharacter)) { return; }

	float fCurHP;
	float fMinus = 0.0;
	float fPlus = 0.0;

    if (bSeaReloadStarted) { return; }
    if (MOD_SKILL_ENEMY_RATE == 1 && sti(rOurCharacter.index) == GetMainCharacterIndex())
	{
		fHP = fHP / MOD_Complexity_1_DMG;
	}
	if (fHP <= 0.0) { return; }
	if (LAi_IsDead(rOurCharacter) || !CheckAttribute(rOurCharacter, "Ship.HP")) { return; }   // fix

    if (iKillerCharacterIndex >= 0)
	{
		if (sti(Characters[iKillerCharacterIndex].TmpPerks.HullDamageUp)) fPlus = 0.15;
		if (sti(Characters[iKillerCharacterIndex].TmpPerks.CannonProfessional)) fPlus = 0.3;
	}

	if (sti(rOurCharacter.TmpPerks.BasicBattleState))			fMinus = 0.15;
	if (sti(rOurCharacter.TmpPerks.AdvancedBattleState))		fMinus = 0.25;
	if (sti(rOurCharacter.TmpPerks.ShipDefenseProfessional))	fMinus = 0.35;

	fCurHP = stf(rOurCharacter.Ship.HP) - fHP * (1.0 + fPlus - fMinus);
	if (fCurHP <= 0.0)
	{
		fCurHP = 0.0;
		ShipDead(sti(rOurCharacter.index), iKillStatus, iKillerCharacterIndex);
	}

    if(fCurHP > sti(RealShips[sti(rOurCharacter.ship.type)].HP))
	{
		fCurHP = RealShips[sti(rOurCharacter.ship.type)].HP;
	}
	rOurCharacter.Ship.HP = fCurHP;
}

void Ship_ApplyHullHitpointsWithCannon(ref rOurCharacter, float fHP, int iKillStatus, int iKillerCharacterIndex, ref rShipOurCharacter)
{
	if (Boarding_DamageBlocked(rOurCharacter, iKillerCharacterIndex)) return;
   	if (LAi_IsImmortal(rOurCharacter))	{ return; }
    if (bSeaReloadStarted) 	{ return; }
	if (fHP <= 0.0)	{ return; }
	if (LAi_IsDead(rOurCharacter) || !CheckAttribute(rOurCharacter, "Ship.HP")) { return; }   // fix

	float fCurHP;
	float fMinus = 0.0;
	float fPlus = 0.0;

    if (bSeaReloadStarted) { return; }
    if (MOD_SKILL_ENEMY_RATE == 1 && sti(rOurCharacter.index) == GetMainCharacterIndex())
	{
		fHP = fHP / MOD_Complexity_1_DMG;
	}
	if (fHP <= 0.0) { return; }
	if (LAi_IsDead(rOurCharacter) || !CheckAttribute(rOurCharacter, "Ship.HP")) { return; }   // fix

	// LEO: перки на урон (стреляющего) и защиту (цели) уже учтены в Ship_GetShipDamageHull - этот путь зовётся только
	//   из попадания ядра с готовым fHP. Раньше перки применялись здесь ВТОРОЙ раз: бонус стрелка удваивался.
	float fDam = fHP;
	if (fDam < 1.0) fDam = 1;
	fCurHP = stf(rOurCharacter.Ship.HP) - fDam;
	if (fCurHP <= 0.0)
	{
		fCurHP = 0.0;
		ShipDead(sti(rOurCharacter.index), iKillStatus, iKillerCharacterIndex);
	}

	int iShipOurCharacterHP = sti(rShipOurCharacter.HP);
    if(fCurHP > iShipOurCharacterHP)	{ fCurHP = MakeFloat(iShipOurCharacterHP); }//зочем

	rOurCharacter.Ship.HP = fCurHP;
}


void Ship_AddCharacterExp(ref rCharacter, int iExpQuantity)
{
	if (sti(rCharacter.index) == nMainCharacterIndex)
	{
		fSeaExp = fSeaExp + iExpQuantity;
	}
}


int g_AIAbordNoSinkLog = 0; // LEO: 1 = подавить отдельную строку потопления в ShipDead (исход авто-абордажа ИИ пишется одной строкой в AIWinAbordageDecision)

// ============================================================================
//   СМЕРТЬ/ЗАХВАТ КОРАБЛЯ, УРОН (ФОРМУЛЫ), СОБЫТИЯ ПОПАДАНИЙ, ПОЖАР
// ============================================================================
// LEO: переназначение командира группы, если выбыл ИМЕННО он (гибель/захват/нацио/дрейф/release). Сток делал это лишь на сдаче — иначе звезда на трупе, свита без ведущего. Берём первого живого не-выбывшего; сдавшихся/ГГ/PLAYER_GROUP не трогаем. Зовётся из ShipDead/ShipTaken/ShipTakenFree.
void Ship_ReassignCommanderIfLost(int iLostIdx)
{
	if (iLostIdx == GetMainCharacterIndex()) return;
	ref rLost = GetCharacter(iLostIdx);
	if (CheckAttribute(rLost, "surrendered")) return;
	string sGrp = Ship_GetGroupID(&rLost);
	if (sGrp == "" || sGrp == PLAYER_GROUP) return;
	if (Group_GetGroupCommanderIndex(sGrp) != iLostIdx) return; // выбыл не командир — группу не трогаем
	ref rGrp = Group_GetGroupByID(sGrp);
	int iGN = Group_GetCharactersNumR(rGrp);
	int iNew = -1;
	int gk;
	int gm;
	for (gk = 0; gk < iGN; gk++) // первый ЖИВОЙ не-выбывший член (труп на слоте нельзя брать вслепую)
	{
		gm = Group_GetCharacterIndexR(rGrp, gk);
		if (gm < 0) continue;
		if (gm == iLostIdx) continue;
		if (LAi_IsDead(&characters[gm])) continue;
		iNew = gm;
		break;
	}
	if (iNew >= 0)
	{
		ref rNew = GetCharacter(iNew);
		DeleteAttribute(rNew, "relation.UseOtherCharacter");
		Group_SetGroupCommander(sGrp, rNew.id);
		Group_Refresh_Engine_relations(sGrp);
	}
}

void ShipDead(int iDeadCharacterIndex, int iKillStatus, int iKillerCharacterIndex)
{
	ref rDead, rKillerCharacter, rBaseShip, rKillerBaseShip;

	rDead = GetCharacter(iDeadCharacterIndex);

	if (!CheckAttribute(rDead, "Ship.Type")) // fix
    {
        if (MOD_BETTATESTMODE == "On") Log_Info("Error: ShipDead Нет корабля у iDeadCharacterIndex = "+iDeadCharacterIndex);
        return;
    }
    bool bDeadCompanion = IsCompanion(rDead);

	bool isPgg = HasSubStr(rDead.id,"PsHero_");
	bool isntPgg1 = HasSubStr(rDead.id,"_Trader") && CheckAttribute(rDead,"PGG_companion");
	bool isntPgg2 = HasSubStr(rDead.id,"_Hunter") && CheckAttribute(rDead,"PGG_companion");
	// LEO: проверяем НАЛИЧИЕ узла PGGAi, а не значение IsPGG: диалоги ПГГ умеют снимать его в false
	//   (PGG_dlg_process.c), и прежняя гарда открывалась. А подбор шлюпки копирует утопленника в новый
	//   слот целиком, вместе с PGGAi - так ПГГ и клонировали. Шлюпку не роняет ни активный ПГГ, ни бывший.
	bool bIsNotAIPGG = true;
	if (CheckAttribute(rDead, "PGGAi")) bIsNotAIPGG = false;

	rBaseShip = GetRealShip(sti(rDead.Ship.Type));
    //#20180925-01
    float fBaseClass = stf(rBaseShip.Class);
	if (iKillerCharacterIndex != -1 && !HasSubStr(rDead.id, "_DriftCap_"))
	{
		rKillerCharacter = GetCharacter(iKillerCharacterIndex);
		rKillerBaseShip = GetRealShip(sti(rKillerCharacter.Ship.Type));
		if (!bDeadCompanion && iKillStatus != KILL_BY_SPY)
        {
			switch (iKillStatus)
		    {
			    case KILL_BY_TOUCH:
	                AddCharacterExpToSkill(rKillerCharacter, "Sailing", stf(rKillerBaseShip.Class) / fBaseClass * 90);
	                ChangeCrewExp(rKillerCharacter, "Sailors", AIShip_isPerksUse(CheckCharacterPerk(rKillerCharacter, "SeaWolf"), 1.0, 2.0));
	                Statistic_AddValue(rKillerCharacter, "KillShip_" + rBaseShip.Class, 1);
					if (isPgg && !isntPgg1 && !isntPgg2)
					{
						DeleteAttribute(PChar,"PGG_EnemyPP");
						if(IsMainCharacter(rKillerCharacter))
						{

							PChar.PGG_Enemy = "1";
						}
					}
			    break;
			    case KILL_BY_BALL:
			        AddCharacterExpToSkill(rKillerCharacter, "Accuracy", stf(rKillerBaseShip.Class) / fBaseClass * 35);
			        AddCharacterExpToSkill(rKillerCharacter, "Sailing", stf(rKillerBaseShip.Class) / fBaseClass * 65);
			        AddCharacterExpToSkill(rKillerCharacter, "Cannons", stf(rKillerBaseShip.Class) / fBaseClass * 40);
			        ChangeCrewExp(rKillerCharacter, "Sailors", AIShip_isPerksUse(CheckCharacterPerk(rKillerCharacter, "SeaWolf"), 1.0, 2.0));
			        ChangeCrewExp(rKillerCharacter, "Cannoners", AIShip_isPerksUse(CheckCharacterPerk(rKillerCharacter, "SeaWolf"), 2.0, 4.0));
	                Statistic_AddValue(rKillerCharacter, "KillShip_" + rBaseShip.Class, 1);
	                //homo 25/06/07 только враг и 1/4
        			if (rand(4) == 2 && GetRelation(sti(rDead.Index), sti(Pchar.Index))== RELATION_ENEMY && bIsNotAIPGG)
                    {
                        //обычный
                        if(!CheckAttribute(rDead, "DontRansackCaptain") || rDead.DontRansackCaptain == false) AISeaGoods_AddGood(rDead, "enemy_boat", "lo_boat", 1000.0, 1); //homo 25/06/07 спасается на шлюпке
                    }
					if (isPgg && !isntPgg1 && !isntPgg2)
					{
						DeleteAttribute(PChar,"PGG_EnemyPP");
						if(IsMainCharacter(rKillerCharacter))
						{
							PChar.PGG_Enemy = "1";
						}
					}
			    break;
			    case KILL_BY_ABORDAGE:
			        AddCharacterExpToSkill(rKillerCharacter, "Grappling", stf(rKillerBaseShip.Class) / fBaseClass * 110);
			        AddCharacterExpToSkill(rKillerCharacter, "Sailing", stf(rKillerBaseShip.Class) / fBaseClass * 70);
			        AddCharacterExpToSkill(rKillerCharacter, SKILL_FORTUNE, stf(rKillerBaseShip.Class) / fBaseClass * 10);
			        AddCharacterExpToSkill(rKillerCharacter, "Defence", stf(rKillerBaseShip.Class) / fBaseClass * 60);
			        ChangeCrewExp(rKillerCharacter, "Sailors", AIShip_isPerksUse(CheckCharacterPerk(rKillerCharacter, "SeaWolf"), 0.5, 1.0));
	                Statistic_AddValue(rKillerCharacter, "KillAbordShip_" + rBaseShip.Class, 1);
					if (IsMainCharacter(rKillerCharacter)) FlagPerkForCapturedShip(rDead);
					if (isPgg && !isntPgg1 && !isntPgg2)
					{
						DeleteAttribute(PChar,"PGG_EnemyPP");
						if(IsMainCharacter(rKillerCharacter))
						{

							PChar.PGG_Enemy = "1";
						}
					}
			    break;
		    }
			if (iKillStatus != KILL_BY_SELF)
			{
		        AddCharacterExpToSkill(rKillerCharacter, SKILL_FORTUNE, stf(rKillerBaseShip.Class) / fBaseClass * 20);
		        AddCharacterExpToSkill(rKillerCharacter, "Leadership", stf(rKillerBaseShip.Class) / fBaseClass * 25);

	            // статистка по нации
		    	Statistic_AddValue(rKillerCharacter, NationShortName(sti(rDead.nation))+"_KillShip", 1);
		    	if (rand(8) < 3 && sti(rDead.nation) != PIRATE)  // 30 процентов повышаем награду
		    	{
					ChangeCharacterHunterScore(rKillerCharacter, NationShortName(sti(rDead.nation)) + "hunter", 1+rand(1));
				}
			}
        }
	}

	float fX = stf(rDead.Ship.Pos.x);
	float fY = stf(rDead.Ship.Pos.y);
	float fZ = stf(rDead.Ship.Pos.z);

	rDead.Ship.HP = 0;
	rDead.Killer.Status = iKillStatus;
	rDead.Killer.Index = iKillerCharacterIndex;

	LAi_SetCurHP(rDead, 0.0);

	if (CheckAttribute(rDead, "curshipnum"))
	{
		DeleteAttribute(rDead, "curshipnum");
	}
	int iExp = 0;
	if (iKillerCharacterIndex != -1)
	{
		iExp = 2 * MakeInt(stf(rBaseShip.HP) * stf(rKillerBaseShip.Class) / fBaseClass);
	}

	bool bCompanion = false;
	if (iKillerCharacterIndex != -1)
	{
		bCompanion = IsCompanion(rKillerCharacter);
	}

 	if (bCompanion && bDeadCompanion)
	{
		iExp = 0;
		if (iKillerCharacterIndex == GetMainCharacterIndex())
		{
			ChangeCharacterReputation(pchar, -15);
		}
	}

	bool bRealKill = false;
	switch (iKillStatus)
	{
		case KILL_BY_TOUCH:
			if (iKillerCharacterIndex != -1) { bRealKill = true; }
		break;
		case KILL_BY_BALL:
			bRealKill = true;
		break;
		case KILL_BY_ABORDAGE:
			bRealKill = true;
		break;
	}

	// LEO: анти-абуз награды за помощь ПГГ. Если игрок/компаньон сам потопил пиратский корабль (реальное потопление, не риф/шторм) —
	//   пометить ПГГ, который в этом бою (Ships[]) и в стычке (PGG_ChasingPirate): флаг PGG_PlayerHelped + узел PGG_ThanksForHelp -> диалог благодарности (+5). Без флага откроется generic-капитан = награды нет.
	// (условие разбито на вложенные if: Storm-парсер не тянет `&& (A || B)`)
	if (bRealKill && iKillerCharacterIndex != -1 && sti(rDead.nation) == PIRATE)
	{
		bool bPggHelper = IsMainCharacter(rKillerCharacter);
		if (bCompanion) bPggHelper = true;
		if (bPggHelper)
		{
			int iPggS;
			ref rPggShip;
			for (iPggS = 0; iPggS < iNumShips; iPggS++)
			{
				if (Ships[iPggS] < 0) continue;
				rPggShip = GetCharacter(Ships[iPggS]);
				if (!HasSubStr(rPggShip.id, "PsHero_")) continue;
				if (!CheckAttribute(rPggShip, "PGG_ChasingPirate")) continue;
				rPggShip.PGG_PlayerHelped = 1;
				rPggShip.DeckDialogNode = "PGG_ThanksForHelp";
			}
		}
	}

	// LEO: оборона колонии — игрок/компаньон потопил осадный штурмовик ("SiegeCap_"): +1 к счёту, награда по классу ×1500 (мэр платит только за реально убитых)
	if (bRealKill && iKillerCharacterIndex != -1 && HasSubStr(rDead.id, "SiegeCap_") && !CheckAttribute(rDead, "SiegeCounted"))
	{
		bool bSiegeHelper = IsMainCharacter(rKillerCharacter);
		if (bCompanion) bSiegeHelper = true;
		if (bSiegeHelper && CheckAttribute(NullCharacter, "Siege.PlayerSiegeKills"))
		{
			rDead.SiegeCounted = true; // LEO: маркер — не задвоить с захватом (SHIP_CAPTURED)
			NullCharacter.Siege.PlayerSiegeKills = sti(NullCharacter.Siege.PlayerSiegeKills) + 1;
			NullCharacter.Siege.PlayerSiegeReward = sti(NullCharacter.Siege.PlayerSiegeReward) + (7 - GetCharacterShipClass(rDead)) * 10 * 1500;
		}
	}

	// LEO: лог потопления, killer-first иконочный формат: [флаг убийцы слева] [тип]'[имя]' È [тип жертвы]'[имя]' [флаг жертвы справа]
	if (iDeadCharacterIndex != nMainCharacterIndex && !HasSubStr(rDead.id, "_DriftCap_") && g_AIAbordNoSinkLog == 0)
	{
		string sSunkString;
		// LEO: имени корабля может не быть вовсе (форт, стёртый слот) - читаем через проверку, иначе строка лога роняет рантайм
		string sDeadName = "";
		if (CheckAttribute(rDead, "Ship.Name")) sDeadName = rDead.Ship.Name;
		string sDeadPart = XI_ConvertString(rBaseShip.BaseName) + " '" + sDeadName + "' " + TransformNationToIcon(sti(rDead.nation)); // LEO: жертва: тип 'имя' флаг
		if (iKillerCharacterIndex != -1 && bRealKill)
		{
			// имя убийцы читаем ТОЛЬКО здесь: выше по функции rKillerCharacter присваивается под тем же условием
			string sKillerName = "";
			if (CheckAttribute(rKillerCharacter, "Ship.Name")) sKillerName = rKillerCharacter.Ship.Name;
			string sKillFlag = TransformNationToIcon(sti(rKillerCharacter.nation));
			string sKillPart;
			if (sKillerName == "" || XI_ConvertString(rKillerBaseShip.BaseName) == "Fort")
				sKillPart = sKillFlag + " " + GetFullName(rKillerCharacter); // LEO: флаг СЛЕВА + полное имя (форт/без имени корабля)
			else
				sKillPart = sKillFlag + " " + XI_ConvertString(rKillerBaseShip.BaseName) + " '" + sKillerName + "'"; // LEO: флаг убийцы СЛЕВА
			sSunkString = sKillPart + " È " + sDeadPart; // LEO: È = иконка Меткость (потопил)
		}
		else
		{
			sSunkString = sDeadPart + " È"; // LEO: потонул без убийцы (шторм/риф)
		}
		Log_SetStringToLog(sSunkString);
	}

	bool bDetonate = false;
	switch (iKillStatus)
	{
		case KILL_BY_BALL:
			bDetonate = true;
		break;
		// KILL_BY_SPY здесь НЕ детонируем: вылет при повторной рекурсии Ship_Detonate
	}
	if (bDetonate)
	{
		Ship_Detonate(rDead, true, false);
	}

	// атрибуты эффекта затопления
	rDead.Ship.Sink = "";
	aref aSink; makearef(aSink, rDead.Ship.Sink);

	// корабль тонет  - немного добавим драматизьму :-)
	aSink.Speed.y = 0.25 + (frnd() * 0.05);	// speed of sink y
	aSink.Speed.x = 0.021 * (frnd() * 2.0 - 1.0);	// speed sink angle rotate around x
	aSink.Speed.z = 0.04 * (frnd() * 2.0 - 1.0);	// speed sink angle rotate around z

	if(rDead.id == "GhostCapt" && iKillerCharacterIndex == nMainCharacterIndex && !CheckAttribute(pchar, "GenQuest.GhostShip.LastBattle") && !CheckAttribute(pchar, "GenQuest.GhostShip.Prize"))
	{
		pchar.GenQuest.GhostShip.Prize = true;
		InterfaceStates.Buttons.Save.enable = false;
		DoQuestCheckDelay("GhostCapt_OfferPrize", 1.5); // Ван-дер-Декена в студию !!!
	}
	if (iDeadCharacterIndex == nMainCharacterIndex)
	{
		SeaCameras_SetDieCamera();
		FadeOutMusic(2);
		// убил ЛГ
		if (iKillerCharacterIndex != -1 && rKillerCharacter.id == "GhostCapt")
		{
		    DoQuestCheckDelay("Survive_In_Sea", 9.0);
		}
		else
		{
			InterfaceStates.Buttons.Save.enable = false;
			PostEvent(GAME_OVER_EVENT, 15000);
		}
	}
    // спасем офицеров boal 07/02/05
	else
	{
		if (bDeadCompanion && bIsNotAIPGG && CheckOfficersPerk(rDead, "ShipEscape") && GetRemovable(rDead)) // выживаем
		{
			RemoveCharacterCompanion(pchar, rDead);
			pchar.GenQuest.Survive_In_SeaOfficerIdx = rDead.index;
	        DoQuestCheckDelay("Survive_In_SeaOfficer", 1.0);
			if (CheckForContractImmortal(rDead)) pchar.Option_ImmortalOfficers = sti(sti(pchar.Option_ImmortalOfficers) - 1);
		}
		else if (bDeadCompanion && bIsNotAIPGG && CheckAttribute(rDead, "HalfImmortal") && GetRemovable(rDead))
		{
			bool bOkSave = bHalfImmortalPgg && CheckAttribute(rDead, "ImmortalOfficer");
			if(bOkSave || bHalfImmortalPgg == false)
			{
				Log_Info(GetFullName(rDead) + GetConvertStrWithReplace("Variable_AIShip_15", "Logs.txt", "#space#", " ")+NPCharSexPhrase(rDead,GetConvertStrWithReplace("Variable_AIShip_16", "Logs.txt", "#space#", " "),GetConvertStrWithReplace("Variable_AIShip_17", "Logs.txt", "#space#", " "))+GetConvertStrWithReplace("Variable_AIShip_18", "Logs.txt", "#space#", " "));
				AISeaGoods_AddGood(rDead, "boat", "lo_boat", 1000.0, 1);
				RemoveCharacterCompanion(pchar, rDead);
				PlaySound("interface\"+LanguageGetLanguage()+"_EvShip1.wav");
				pchar.Option_ImmortalOfficers = sti(sti(pchar.Option_ImmortalOfficers) - 1);
			}
		}
	}
	// спасем офицеров boal 07/02/05
	Play3DSound("ship_sink", fX, fY, fZ);

	// LEO: погиб командир эскадры -> переназначить (иначе звезда на трупе, свита без ведущего). Единый хелпер.
	Ship_ReassignCommanderIfLost(iDeadCharacterIndex);

	// сообщение в ИИ
	SendMessage(&AISea, "la", AI_MESSAGE_CHARACTER_DEAD, rDead);

	// событие
	Event(SHIP_DEAD, "l", iDeadCharacterIndex);
}

bool CheckForContractImmortal(ref rDead)
{
	bool bOkSave = bHalfImmortalPgg && CheckAttribute(rDead, "ImmortalOfficer");
	if(bOkSave || bHalfImmortalPgg == false)
	{
		return true;
	}
	return false;
}

void ShipTaken(int iDeadCharacterIndex, int iKillStatus, int iKillerCharacterIndex)
{
	ref rDead, rKillerCharacter, rMainCharacter, rBaseShip, rKillerBaseShip;

	rDead = GetCharacter(iDeadCharacterIndex);
	rBaseShip = GetRealShip(sti(rDead.Ship.Type));
	rMainCharacter = GetMainCharacter();
    //#20180925-01
    float fBaseClass = stf(rBaseShip.Class);
	bool bCompanion = false;
	bool bDeadCompanion = IsCompanion(rDead);

    LAi_SetCurHP(rDead, 0.0); // умер
	rDead.Killer.Status = iKillStatus;
	rDead.Killer.Index = iKillerCharacterIndex;
    if (iKillerCharacterIndex != -1 && !bDeadCompanion)
	{
		rKillerCharacter = GetCharacter(iKillerCharacterIndex);
		rKillerBaseShip = GetRealShip(sti(rKillerCharacter.Ship.Type));
		bCompanion = IsCompanion(rKillerCharacter);
        AddCharacterExpToSkill(rKillerCharacter, SKILL_FORTUNE, stf(rKillerBaseShip.Class) / fBaseClass * 20);
        AddCharacterExpToSkill(rKillerCharacter, "Leadership", stf(rKillerBaseShip.Class) / fBaseClass * 25);
        AddCharacterExpToSkill(rKillerCharacter, "Grappling", stf(rKillerBaseShip.Class) / fBaseClass * 120);
		AddCharacterExpToSkill(rKillerCharacter, "Sailing", stf(rKillerBaseShip.Class) / fBaseClass * 70);
		AddCharacterExpToSkill(rKillerCharacter, "Commerce", stf(rKillerBaseShip.Class) / fBaseClass * 30);
		AddCharacterExpToSkill(rKillerCharacter, "Repair", stf(rKillerBaseShip.Class) / fBaseClass * 60);
		AddCharacterExpToSkill(rKillerCharacter, "Defence", stf(rKillerBaseShip.Class) / fBaseClass * 60);
        Statistic_AddValue(rKillerCharacter, "AbordShip_" + rBaseShip.Class, 1);
        Statistic_AddValue(rKillerCharacter, NationShortName(sti(rDead.nation))+"_AbordShip", 1);

        ChangeCrewExp(rKillerCharacter, "Sailors", AIShip_isPerksUse(CheckCharacterPerk(rKillerCharacter, "SeaWolf"), 0.5, 1.0));
	}
	if (iDeadCharacterIndex != GetMainCharacterIndex())
	{
		string sSunkShipType = XI_ConvertString(rBaseShip.BaseName);
		string sKillShipType = XI_ConvertString(rKillerBaseShip.BaseName);

		if(bCompanion && !bDeadCompanion)
		{
            Log_SetStringToLog(sSunkShipType + " '" + rDead.Ship.Name + "' " + GetConvertStrWithReplace("Variable_AIShip_23", "Logs.txt", "#space#", " "));
        }
	}
	// 30 процентов повышаем награду
    if (rand(8) < 3 && !bDeadCompanion && sti(rDead.nation) != PIRATE && (iKillerCharacterIndex == GetMainCharacterIndex()))
    {
        ChangeCharacterHunterScore(rKillerCharacter, NationShortName(sti(rDead.nation)) + "hunter", 1+rand(1));
    }
	// LEO: захват флагмана В СВОЙ ФЛОТ уводит командира из вражьей эскадры -> переназначить ей нового (единый хелпер).
	Ship_ReassignCommanderIfLost(iDeadCharacterIndex);
}
// захваченный корабль, отпущен
void ShipTakenFree(int iDeadCharacterIndex, int iKillStatus, int iKillerCharacterIndex)
{
	ref rDead, rKillerCharacter, rMainCharacter, rBaseShip, rKillerBaseShip;

	rDead = GetCharacter(iDeadCharacterIndex);
	rBaseShip = GetRealShip(sti(rDead.Ship.Type));
	rMainCharacter = GetMainCharacter();
    //#20180925-01
    float fBaseClass = stf(rBaseShip.Class);
    LAi_SetCurHP(rDead, 0.0); // умер
	rDead.Killer.Status = iKillStatus;
	rDead.Killer.Index = iKillerCharacterIndex;
    if (iKillerCharacterIndex != -1)
	{
		rKillerCharacter = GetCharacter(iKillerCharacterIndex);
		rKillerBaseShip = GetRealShip(sti(rKillerCharacter.Ship.Type));

        AddCharacterExpToSkill(rKillerCharacter, "Leadership", stf(rKillerBaseShip.Class) / fBaseClass * 50);
        AddCharacterExpToSkill(rKillerCharacter, "Grappling", stf(rKillerBaseShip.Class) / fBaseClass * 120);
		AddCharacterExpToSkill(rKillerCharacter, "Sailing", stf(rKillerBaseShip.Class) / fBaseClass * 80);
		AddCharacterExpToSkill(rKillerCharacter, SKILL_FORTUNE, stf(rKillerBaseShip.Class) / fBaseClass * 70);
		AddCharacterExpToSkill(rKillerCharacter, "Repair", stf(rKillerBaseShip.Class) / fBaseClass * 10);
		AddCharacterExpToSkill(rKillerCharacter, "Defence", stf(rKillerBaseShip.Class) / fBaseClass * 10);

		Statistic_AddValue(rKillerCharacter, "AbordShipFree_" + rBaseShip.Class, 1);  // эту статистику пока нигде не видно to_do
        Statistic_AddValue(rKillerCharacter, NationShortName(sti(rDead.nation))+"_AbordShipFree", 1);
        Statistic_AddValue(rKillerCharacter, "AbordShipFree", 1); // отпущено

        ChangeCrewExp(rKillerCharacter, "Sailors", AIShip_isPerksUse(CheckCharacterPerk(rKillerCharacter, "SeaWolf"), 0.5, 1.0));
		if(!CheckAttribute(rDead,"Situation") && GetCharacterIndex ("_DriftCap_" + rDead.id) == -1 && GetCharacterIndex (rDead.id + "_free") != -1)
		{
			ChangeCharacterReputation(rKillerCharacter, 2); // хорошее дело
			OfficersReaction("good");
		}
	}
	if (iDeadCharacterIndex != GetMainCharacterIndex() && !CheckAttribute(rDead,"Situation") && GetCharacterIndex ("_DriftCap_" + rDead.id) == -1 && GetCharacterIndex (rDead.id + "_free") != -1)
	{
		string sSunkShipType = XI_ConvertString(rBaseShip.BaseName);
		string sKillShipType = XI_ConvertString(rKillerBaseShip.BaseName);

        Log_SetStringToLog(sSunkShipType + " '" + rDead.Ship.Name + "' " + GetConvertStrWithReplace("Variable_AIShip_24", "Logs.txt", "#space#", " "));
	}
	// 14 процентов повышаем награду
    if (rand(20) < 3 && sti(rDead.nation) != PIRATE && (iKillerCharacterIndex == GetMainCharacterIndex()))
    {
        ChangeCharacterHunterScore(rKillerCharacter, NationShortName(sti(rDead.nation)) + "hunter", 1+rand(1));
    }

	// LEO: захвачен на АБОРДАЖ командир эскадры (нацио/дрейф/release идут через ShipTakenFree) -> переназначить (единый хелпер).
	Ship_ReassignCommanderIfLost(iDeadCharacterIndex);
}

void Ship_PlayVictory(string sSound, int iSoundTime)
{
	FadeOutMusic(3); // boal 3000
	PlayStereoOgg(sSound);

	iVicSoundTime = iSoundTime;
}
  /// странные методы ??? не было их в ПКМ и все было гут
void Ship_VicSoundTime()
{
	if (bAbordageStarted || sti(InterfaceStates.Launched)) { return; }
	// LEO: Кадровое событие приходит и на паузе; морские таймеры в ней не идут.
	if (GetDeltaTime() == 0) return;

	// морское время для боевой морали (GetSeaTime, секунды): копим кадровые мс
	iSeaTimeAccumMs = iSeaTimeAccumMs + iRealDeltaTime;
	if (iSeaTimeAccumMs >= 1000)
	{
		iSeaTimeAccumMs = iSeaTimeAccumMs - 1000;
		pchar.seatime = GetSeaTime() + 1;
		Boarding_RefreshCooldown();
	}

	if (iVicSoundTime > 0)
	{
		iVicSoundTime = iVicSoundTime - iRealDeltaTime;
		if (iVicSoundTime <= 0)
		{
			bUpdatePosMode = true;
		}
	}
}

//считает защиту от брони корабля с учетом множителя расстояния. rCannon - пушка стреляющего, rShipOurCharacter - корабль защищающегося, fDistanceDamageMultiply - множитель расстояния (0.8...1.05), bSArmor - проверка на супер-броню
float Ship_GetCannonDamageMultiply(ref rCannon, ref rShipOurCharacter, float fDistanceDamageMultiply, bool bSArmor)
{
	float fCannonDamageMultiply = stf(rCannon.DamageMultiply);
	float fCaliberWDist = stf(rCannon.caliber) * fDistanceDamageMultiply;
	float fMinusArmor = 0;
	if (!bSArmor) { fMinusArmor = stf(rShipOurCharacter.HullArmor) / 2.0; }//в среднем 50% // СФ, -30-50% урона
	if (fMinusArmor > 0.75 * fCaliberWDist) { fMinusArmor = 0.75 * fCaliberWDist; }//больше 75% урона броня не поглощает
	fCannonDamageMultiply *= (fCaliberWDist - fMinusArmor) / fCaliberWDist;
	return fCannonDamageMultiply;
}

//[0, 0.15]
float Ship_GetShipDamageCrewFromPerks_Ball(ref rBallCharacter)
{
	return 1 + 0.15 * stf(rBallCharacter.TmpPerks.CrewDamageUp);
}

//[1, 0.9, 0.8]
float Ship_GetShipDamageCrewFromPerks_Our(ref rOurCharacter)
{
	return (1.0 - 0.1 * stf(rOurCharacter.TmpPerks.Doctor1) - 0.1 * stf(rOurCharacter.TmpPerks.Doctor2));
}

//[~1.0...0.6;]
float Ship_GetShipDamageCrewFromSkill_Our(ref rOurCharacter); 
{
	return (1.04 - 0.4 * stf(rOurCharacter.TmpSkill.Defence));
}

//Добавлять все редкие особенности сюда
float Ship_GetShipDamageCrewFromSpecialFeatures(ref rOurCharacter, ref rShipOurCharacter, bool bSArmor);
{
	//CФ
	float fMult = (1 - 0.85 * MakeInt(bSArmor));
	fMult *= (1.0 - 0.2 * GetCharacterItem(rOurCharacter, "SkullAztec"));
	// Gregg: оберег Мухачи из квеста «Две сестры» - работает из инвентаря, как череп.
	fMult *= (1.0 - 0.1 * GetCharacterItem(rOurCharacter, "MuhachiAmulet"));
	if (CheckForUpgrade(rShipOurCharacter,"SUP_BARRICADES")) fMult *= 0.8; //Баррикады
	if (CheckForUpgrade(rShipOurCharacter,"SUP_FALCONETS")) 
	{
		if (CheckShipType(rShipOurCharacter,"Universal")) fMult *= 1.2; //Усиленные фальконеты
		else fMult *= 1.1; //Усиленные фальконеты
	}
	return fMult;
}

//считает урон в команду. fBall = sti(&Goods[iGoodType].DamageHull).		//fCannonDamageMultiply = 1 с rBallCharacter = GetCharacter(TOTAL_CHARACTERS) дает показатель защиты корабля
// LEO: число для всплывающего урона. Раньше в попапах стояло makeint(урон) >= 1, то есть дробная часть
// ОТБРАСЫВАЛАСЬ и урон меньше единицы не показывался вовсе. У картечи по кораблю ГГ так выходило всегда:
// её базовый урон в команду мал, а множитель жертвы (1.04 - 0.4*Защита, перки врача, баррикады) режет его
// ещё сильнее - до целого не доходило. По врагам с низкой Защитой доходило, отсюда и жалоба, что картечь
// по врагам пишется, а по себе нет. Округляем и показываем минимум единицу, если урон вообще был.
int Ship_DamagePopupValue(float fDamage)
{
	if (fDamage <= 0.0) return 0;
	int iVal = makeint(fDamage + 0.5);
	if (iVal < 1) iVal = 1;
	return iVal;
}

float Ship_GetShipDamageCrew(float fBallDamage, int iBallType, ref rBallCharacter, ref rOurCharacter, ref rShipOurCharacter, float fCannonDamageMultiply, bool bSArmor)
{
	float fMultiply = Ship_GetShipDamageCrewFromPerks_Ball(rBallCharacter);
	fMultiply *= Ship_GetShipDamageCrewFromSkill_Our(rOurCharacter);
	fMultiply *= Ship_GetShipDamageCrewFromPerks_Our(rOurCharacter);
	fMultiply *= Ship_GetShipDamageCrewFromSpecialFeatures(rOurCharacter, rShipOurCharacter, bSArmor);

	return fBallDamage * fCannonDamageMultiply * fMultiply;
}

//[0, 0.15, 0.3]
float Ship_GetShipDamageHullFromPerks_Ball(ref rBallCharacter)
{
	return 0.15 * stf(rBallCharacter.TmpPerks.HullDamageUp) + 0.15 * stf(rBallCharacter.TmpPerks.CannonProfessional);
}

//[0, 0.15, 0.25, 0.35]
float Ship_GetShipDamageHullFromPerks_Our(ref rOurCharacter)
{
	// LEO: у NPC старший перк может быть выдан без предыдущих ступеней.
	if (sti(rOurCharacter.TmpPerks.ShipDefenseProfessional)) return 0.35;
	if (sti(rOurCharacter.TmpPerks.AdvancedBattleState)) return 0.25;
	if (sti(rOurCharacter.TmpPerks.BasicBattleState)) return 0.15;
	return 0.0;
}

//Добавлять все редкие особенности сюда
float Ship_GetShipDamageHullFromSpecialFeatures(ref rBallCharacter, int iBallType, bool bSArmor)
{
	//CФ
	float fMult = (1 - 0.85 * MakeFloat(bSArmor));
	return fMult;
}

//считает урон в корпус до учета перков. fBall = sti(&Goods[iGoodType].DamageHull).		//fCannonDamageMultiply = 1 с rBallCharacter = GetCharacter(TOTAL_CHARACTERS) дает показатель защиты корабля
float Ship_GetShipDamageHull(float fBallDamage, int iBallType, ref rBallCharacter, ref rOurCharacter, ref rShipBallCharacter, ref rShipOurCharacter, float fCannonDamageMultiply, bool bSArmor)
{
	//спец множители отдельно
	fCannonDamageMultiply *= Ship_GetShipDamageHullFromSpecialFeatures(rBallCharacter, iBallType, bSArmor);

	float fMult = 1;
	//плюсы
	fMult += Ship_GetShipDamageHullFromPerks_Ball(rBallCharacter);
	fCannonDamageMultiply *= fMult;

	//минусы отдельно
	// LEO: защитные перки берём у ЦЕЛИ (rOurCharacter). Было rBallCharacter - урон резали перки защиты СТРЕЛЯЮЩЕГО.
	fMult = 1;
	fMult -= Ship_GetShipDamageHullFromPerks_Our(rOurCharacter);
	fCannonDamageMultiply *= fMult;
	
	fMult = fBallDamage * fCannonDamageMultiply * 0.85;//0.85 убрать с НИ патчем
	return fMult;	
}

//считает шанс крита. iBallType: 0 ядра, 1 картечь, 2 книппели, 3 бомбы. rOurCharacter = GetCharacter(TOTAL_CHARACTERS) дает крит без перка на защиту от крита
float Ship_GetShipCritChance(ref rBallCharacter, ref rOurCharacter, ref rShipBallCharacter, ref rShipOurCharacter, int iBallType, int iShipBallClass)
{
	float fCritChance = 0.0;
	switch (iBallType)	{
		case GOOD_BALLS:
			fCritChance += 0.01;
		break;
		case GOOD_GRAPES:
			return 0.0;
		break;
		case GOOD_KNIPPELS:
			return 0.0;
		break;
		case GOOD_BOMBS:
			fCritChance += 0.04;
		break;
	}
	// LEO: было `=` - затирало базовый шанс из switch выше (ядра 1%, бомбы 4%), он не работал никогда
	fCritChance += 0.05 * sti(rBallCharacter.TmpPerks.CriticalShoot) + 0.10 * sti(rBallCharacter.TmpPerks.CannonProfessional);
	
	if (CheckForUpgrade(rShipBallCharacter,"SUP_GUNS") && iBallType != GOOD_GRAPES)
	{
		if (CheckShipType(rShipBallCharacter,"Universal")) fCritChance += 0.04 + (iShipBallClass * 0.005); //Железные пушки крит плюс
		else fCritChance += 0.04 + (iShipBallClass * 0.01); 
	}
	if (CheckForUpgrade(rShipOurCharacter,"SUP_TRADEARMOUR")) //Защита трюма
	{
		fCritChance *= 0.6; //Защита трюма от крита
	}
	if (CheckForUpgrade(rShipOurCharacter,"SUP_COMBATARSENAL"))
	{
		if (sti(rShipOurCharacter.class) > 4) fCritChance -= 0.1; //Боевой арсенал
		else if (sti(rShipOurCharacter.class) > 2) fCritChance -= 0.15;
		else fCritChance -= 0.2;
	}
	if (CheckForUpgrade(rShipOurCharacter,"SUP_ADDITIONALARSENAL")) //Дополнительный арсенал
	{
		if (CheckShipType(rShipOurCharacter,"Universal")) fCritChance *= 1.2;
		else fCritChance *= 1.1; 
	}
	
	return fCritChance *= (1 - 0.7 * sti(rOurCharacter.TmpPerks.ShipDefenseProfessional));
}

//считает шанс пожара. iBallType: 0 ядра, 1 картечь, 2 книппели, 3 бомбы.
float Ship_GetShipInFlameChance(int iBallType, ref rShipOurCharacter)
{
	float InFlame = 0.0;
	switch (iBallType)	{
		case GOOD_BALLS:
			InFlame = 0.02;
		break;
		case GOOD_GRAPES:
			return 0.0;
		break;
		case GOOD_KNIPPELS:
			return 0.0;
		break;
		case GOOD_BOMBS:
			InFlame = 0.20;
		break;
	}
	if (CheckForUpgrade(rShipOurCharacter,"SUP_TRADEARMOUR")) //Защита трюма
	{
		InFlame *= 0.33; //Защита трюма от пожара
	}
	return InFlame;
}


void Ship_SailHitEvent()
{
}

void Ship_HullHitEvent()
{
	aref	rShipObject = GetEventData();

	int		iBallCharacterIndex = GetEventData();
	int		iOurCharacterIndex = GetEventData();


	float	x = GetEventData();
	float	y = GetEventData();
	float	z = GetEventData();

	// LEO ТЕСТ спрайт-эффектов: эффект "ship_explode" в ТОЧКЕ ПОПАДАНИЯ ядра в корпус (быстрый тест — срабатывает на каждый хит)

	int		iFirePlaceIndex = GetEventData();
	float	fFirePlaceDistance = GetEventData();

	bool	bSeriousBoom = false;
	bool	bInflame = false;
	
	ref		rOurCharacter = GetCharacter(iOurCharacterIndex);   // по кому
	if (Boarding_DamageBlocked(rOurCharacter, iBallCharacterIndex)) return;
	ref		rBallCharacter = GetCharacter(iBallCharacterIndex);	// кто пуляет

	// ПЕРЕМИРИЕ Speak: снаряды ГГ и компаньонов кораблю под перемирием урона не наносят (seaspeak.c)
	if (SpeakTruce_HitImmune(rOurCharacter, iBallCharacterIndex)) return;

	rOurCharacter.Ship.LastBallCharacter = iBallCharacterIndex;
	bool	bDead = LAi_IsDead(rOurCharacter) && CheckAttribute(rBallCharacter, "Ship.Cannons.Type");  // boal fix
	if (bDead) return; // фикс, не нужно обсчитывать труп

	// боевая мораль (Ship_CheckMorale): когда били и сколько было команды/корпуса на момент
	// прошлого пересчёта (lastcrew/lastHP трутся при каждом тике морали - пишем только если пусто)
	rOurCharacter.seatime.lasthit = GetSeaTime();
	if (!CheckAttribute(rOurCharacter, "seatime.lastcrew")) rOurCharacter.seatime.lastcrew = GetCrewQuantity(rOurCharacter);
	if (!CheckAttribute(rOurCharacter, "seatime.lastHP")) rOurCharacter.seatime.lastHP = rOurCharacter.Ship.HP;

	float	fHP, fCrewDamage;
	int		iPopupHP, iPopupCrew;

	int		iBallType = sti(AIBalls.CurrentBallType);

	int		iShipBallCharacter = sti(rBallCharacter.Ship.Type);
	int		iShipOurCharacter = sti(rOurCharacter.Ship.Type);
	ref 	rShipBallCharacter = GetRealShip(iShipBallCharacter);	
	ref 	rShipOurCharacter = GetRealShip(iShipOurCharacter);
    bool    isOurCompanion   = IsCompanion(rOurCharacter);
	bool	bSArmor = S_armor(rOurCharacter);
	int		iShipBallClass = sti(rShipBallCharacter.class);
	
	
	float fDistanceDamageMultiply = Bring2Range(1.05, 0.8, 0.0, stf(AIBalls.CurrentMaxBallDistance), stf(AIBalls.CurrentBallDistance));
	ref rCannon = GetCannonByType(sti(rBallCharacter.Ship.Cannons.Type));
	float fCannonDamageMultiply = Ship_GetCannonDamageMultiply(rCannon, rShipOurCharacter, fDistanceDamageMultiply, bSArmor);

	float fCritChance = Ship_GetShipCritChance(rBallCharacter, rOurCharacter, rShipBallCharacter, rShipOurCharacter, iBallType, iShipBallClass);
	float fInflameChance = Ship_GetShipInFlameChance(iBallType, rShipOurCharacter);

	if (Random() < fCritChance) { bSeriousBoom = true; }
	if (Random() < fInflameChance) { bInflame = true; }
	if (bSArmor) { bSeriousBoom = false; bInflame = false; } // СФ, -15% урона от ядер, -40% от бомб

	//TODO: выписать крит шанс ишанс пожара в параметры товаров 
	switch (iBallType)	{
		case GOOD_BALLS:
			Play3DSound("ball2bort", x, y, z);
		break;
		case GOOD_GRAPES:
			Play3DSound("grapes2bort", x, y, z);
            if (rand(100) < 30) {Play3DSound("episode_boom", x, y, z);}
		break;
		case GOOD_KNIPPELS:
			Play3DSound("ball2bort", x, y, z);
		break;
		case GOOD_BOMBS:
			Play3DSound("bomb2bort", x, y, z);
		break;
		
	}
	if (iBallType == GOOD_BOMBS)
	{
		if (Random() > 0.5)
		{
			CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0);
		}
		else
		{
			if (sti(InterfaceStates.OtherParticles))
			{
				CreateParticleSystem("blast_bomb", x, y, z, 0.0, 0.0, 0.0, 0);
				CreateParticleSystem("blast_bomb_ptr", x, y, z, 0.0, 0.0, 0.0, 0);
			}
			else CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0);
		}
	}
	CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0);

	ref rBall = GetGoodByType(iBallType);
	if (iBallType == GOOD_BALLS && rBall.DamageCrew > 0.025) {Goods[GOOD_BALLS].DamageCrew = 0.025;}
    fCrewDamage = Ship_GetShipDamageCrew(stf(rBall.DamageCrew), iBallType, rBallCharacter, rOurCharacter, rShipOurCharacter, fCannonDamageMultiply, bSArmor);
	fHP = Ship_GetShipDamageHull(stf(rBall.DamageHull), iBallType, rBallCharacter, rOurCharacter, rShipBallCharacter, rShipOurCharacter, fCannonDamageMultiply, bSArmor);

	if (bSeriousBoom)	{
		fCrewDamage = fCrewDamage * 3.0;
		fHP = fHP * 3.0; // 4.0
		if (iBallCharacterIndex == nMainCharacterIndex)
		{
			// LEO: крит по кораблю. ShipCapShowCrit вкл -> попап "Критическое попадание!" над кораблём БЕЗ строки в лог
			// (масштаб/дальность из настроек подписей); выкл -> по-старому строка в лог.
			if (sti(InterfaceStates.ShipCapShowCrit) == 1)
			{
				SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, x, y, z, LanguageConvertString(iSeaSectionLang, "Ship_critical"), argb(255, 255, 64, 64), GetShipCapNotifScale(), GetShipCapNotifDistance());
			}
			else
			{
				bGameLogSkip = 1; // крит — на экран, но НЕ в Журнал игры (спам)
				Log_SetStringToLog(LanguageConvertString(iSeaSectionLang, "Ship_critical"));
				bGameLogSkip = 0;
			}
		}
		// LEO: крит ПО кораблю ИГРОКА — показываем под опцией урона по своему кораблю (ShipCapShowDamageSelf)
		if (iOurCharacterIndex == nMainCharacterIndex && sti(InterfaceStates.ShipCapShowDamageSelf) == 1)
		{
			SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, x, y, z, LanguageConvertString(iSeaSectionLang, "Ship_critical"), argb(255, 255, 64, 64), GetShipCapNotifScale(), GetShipCapNotifDistance());
		}

		if (!isOurCompanion && IsCompanion(rBallCharacter) && bDisableMapEnter)	{
            AddCharacterExpToSkill(rBallCharacter, "Accuracy", 10);
            AddCharacterExpToSkill(rBallCharacter, SKILL_FORTUNE, 10);
            AddCharacterExpToSkill(rBallCharacter, "Leadership", 2);
            AddCharacterExpToSkill(rBallCharacter, "Sailing", 5);
            AddCharacterExpToSkill(rBallCharacter, "Cannons", 10);
            ChangeCrewExp(rBallCharacter, "Sailors", AIShip_isPerksUse(CheckCharacterPerk(rBallCharacter, "SeaWolf"), 0.02, 0.4));
			ChangeCrewExp(rBallCharacter, "Cannoners", AIShip_isPerksUse(CheckCharacterPerk(rBallCharacter, "SeaWolf"), 0.2, 0.4));
        }
	}
	else	{
		if (!isOurCompanion && IsCompanion(rBallCharacter) && bDisableMapEnter)	{
            AddCharacterExpToSkill(rBallCharacter, "Accuracy", 2);
            AddCharacterExpToSkill(rBallCharacter, "Sailing", 1);
            AddCharacterExpToSkill(rBallCharacter, "Cannons", 1);
            ChangeCrewExp(rBallCharacter, "Cannoners", AIShip_isPerksUse(CheckCharacterPerk(rBallCharacter, "SeaWolf"), 0.01, 0.02));
        }
	}

	Ship_ApplyHullHitpointsWithCannon(rOurCharacter, fHP, KILL_BY_BALL, iBallCharacterIndex, rShipOurCharacter);
	Ship_ApplyCrewHitpointsWithCannon(rOurCharacter, iOurCharacterIndex, fCrewDamage, iBallType, rShipOurCharacter);

	// LEO: МОРТИРА ФОРТА (локаторы mortar* -> слот Fort.Cannons.Type.3 = LBS48) рвётся как мина: эффект+звук+splash по врагам форта.
	// ПЕРФ: первым стоит int-сравнение типа пушки ЭТОГО снаряда (AIBalls.CurrentBallCannonType движок обновляет перед каждой
	//       трассой, здесь оно валидно). LBS48 кораблям недоступен - верфи дают максимум LBS42 - поэтому все обычные попадания
	//       отсекаются одним сравнением, и тяжёлый FindEntity-обход внутри Fort_MortarBoom не стартует.
	//       Тип пушки берём именно у снаряда: rBallCharacter.Ship.Cannons.Type у форта - это последняя обработанная группа стволов, не эта.
	if (sti(AIBalls.CurrentBallCannonType) >= CANNON_TYPE_CANNON_LBS48)
	{
		if (CheckAttribute(rBallCharacter, "Fort.Cannons.Type.3"))
		{
			if (sti(rBallCharacter.Fort.Cannons.Type.3) == sti(AIBalls.CurrentBallCannonType))
			{
				Fort_MortarBoom(rBallCharacter, iOurCharacterIndex, x, y, z);
			}
		}
	}

	// LEO: попап нанесённого урона над целью — корпус КРАСНЫМ, команда БЕЛЫМ (на 3м выше, чтоб не перекрывались).
	// Гейт «игрок наносит» + опция ShipCapShowDamage; блинк и масштаб — как у крит-попапа, минус перед числом.
	if (iBallCharacterIndex == nMainCharacterIndex && sti(InterfaceStates.ShipCapShowDamage) == 1)
	{
		iPopupHP = Ship_DamagePopupValue(fHP);
		iPopupCrew = Ship_DamagePopupValue(fCrewDamage);
		if (iPopupHP >= 1)
			SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, x, y, z, "-" + its(iPopupHP), argb(255, 255, 60, 60), GetShipCapNotifScale(), GetShipCapNotifDistance());
		if (iPopupCrew >= 1)
			SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, x, y + 3.0, z, "-" + its(iPopupCrew), argb(255, 255, 255, 255), GetShipCapNotifScale(), GetShipCapNotifDistance());
	}

	// LEO: всплывающий ПОЛУЧЕННЫЙ урон над кораблём ИГРОКА (игрок — жертва) — отдельная опция ShipCapShowDamageSelf.
	if (iOurCharacterIndex == nMainCharacterIndex && sti(InterfaceStates.ShipCapShowDamageSelf) == 1)
	{
		iPopupHP = Ship_DamagePopupValue(fHP);
		iPopupCrew = Ship_DamagePopupValue(fCrewDamage);
		if (iPopupHP >= 1)
			SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, x, y, z, "-" + its(iPopupHP), argb(255, 255, 60, 60), GetShipCapNotifScale(), GetShipCapNotifDistance());
		if (iPopupCrew >= 1)
			SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, x, y + 3.0, z, "-" + its(iPopupCrew), argb(255, 255, 255, 255), GetShipCapNotifScale(), GetShipCapNotifDistance());
	}

	if (bInflame == true && fFirePlaceDistance < 16.0 && iFirePlaceIndex >= 0)
	{
		int iRandStartTime = rand(1000);
		float fTotalFireTime = Ship_GetTotalFireTime(rOurCharacter);

		PostEvent(SHIP_ACTIVATE_FIRE_PLACE, iRandStartTime, "ialsfl", rShipObject, rOurCharacter, iFirePlaceIndex, "ship_onfire", fTotalFireTime, iBallCharacterIndex);
		PostEvent(SHIP_FIRE_DAMAGE, iRandStartTime, "lllf", iOurCharacterIndex, iBallCharacterIndex, iFirePlaceIndex, fTotalFireTime);
	}

	if (bSeriousBoom)//не хватило места в крюйт-камере для пороха, случайный подрыв запасов
	{
		if (iBallType == GOOD_BOMBS)
		{
			if (Random() > 0.5) {
			CreateBlast(x, y, z, 0.0, 0.0, "geo")
			if (sti(InterfaceStates.OtherParticles))
			{
				CreateParticleSystem("blast_bomb", x, y, z, 0.0, 0.0, 0.0, 0);
				CreateParticleSystem("blast_bomb_big", x, y, z, 0.0, 0.0, 0.0, 0);
			}
			else CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0);
		}
		else
		{
			if (sti(InterfaceStates.OtherParticles)) CreateParticleSystem("ShipExplode_ext", x, y, z, 0.0, 0.0, 0.0, 0);
			else CreateParticleSystem("ShipExplode", x, y, z, 0.0, 0.0, 0.0, 0);
		}
		}
		else
		{
			if (Random() > 0.5)
			{
				CreateBlast(x, y, z, 0.0, 0.0, "geo")
			}
			if (sti(InterfaceStates.OtherParticles)) CreateParticleSystem("ShipExplode_ext", x, y, z, 0.0, 0.0, 0.0, 0);
			else CreateParticleSystem("ShipExplode", x, y, z, 0.0, 0.0, 0.0, 0);
		}
		Play3DSound("ship_explosion", x, y, z);
		if (rand(1) == 1 && (GetCargoGoods(rOurCharacter, GOOD_POWDER) / 20.0) > (GetCargoMaxSpace(rOurCharacter) * 0.25))
		{
			if (CheckForUpgrade(&RealShips[sti(rOurCharacter.Ship.Type)],"SUP_COMBATARSENAL") && rand(9) > 0) //Боевой арсенал
			{}
			else
			{
				Ship_SetExplosion(rOurCharacter, rShipObject);
				Log_Info(XI_ConvertString("On_ship") + " " + rOurCharacter.Ship.Name + " " + XI_ConvertString("detonated"));
			}
		}
	}
    SendMessage(&AISea, "laffff", AI_MESSAGE_CANNONS_BOOM_CHECK, rOurCharacter, fHP, x, y, z);  // fix перенес с проверкой на труп
	// boal 290704 раз в минуту проверяем обиду на гл героя, если жухлит через желтый прицел -->
	if (sti(rOurCharacter.Ship.LastBallCharacter) == GetMainCharacterIndex() && GetNationRelation2MainCharacter(sti(rOurCharacter.nation)) != RELATION_ENEMY)
    {
        if (!isOurCompanion && !CheckAttribute(rOurCharacter, "Coastal_Captain")) // по своим можно палить таможня пофиг
        {
            Ship_NationAgressive(rOurCharacter, rOurCharacter);// boal 030804 в один метод
            NationUpdate();
            DoQuestCheckDelay("NationUpdate", 0.7);
        }
    }
    // boal 290704 раз в минуту проверяем обиду на гл героя, если жухлит через желтый прицел <--
    //#20190113-06
    if(GetNationRelation(sti(rBallCharacter.Nation), sti(rOurCharacter.nation)) != RELATION_FRIEND)
	{
        if(CheckAttribute(rOurCharacter, "SeaAI.fortSanctuary"))
		{
            DeleteAttribute(rOurCharacter, "SeaAI.fortSanctuary");
            rOurCharacter.SeaAI.hitInSanctuary = true;
        }
    }
}

float Ship_GetTotalFireTime(ref rCharacter)
{
	if (CheckAttribute(rCharacter, "TmpSkill.Repair")) {
		return MakeFloat(SHIP_DEFAULT_FIRE_TIME + (7 - GetCharacterShipClass(rCharacter))) - stf(rCharacter.TmpSkill.Repair) * 10.0;//~10...15 секунд
	}
	else {
		return MakeFloat(SHIP_DEFAULT_FIRE_TIME);
	}
}

float Ship_CalculateFireDamage(ref rCannon, bool bOnOffRandom)
{
	float fHP = 0;
	if (bOnOffRandom){
		fHP = Random();
	}
	fHP += stf(rCannon.DamageMultiply) / 6;//*12...20с = 2...3.3x от урона снаряда
	return fHP;
}

void Ship_FireDamage()
{
	int iOurCharacterIndex = GetEventData();
	int iBallCharacterIndex = GetEventData();

	int iFirePlaceIndex = GetEventData();
	float fTotalFireTime = GetEventData();

	ref rOurCharacter = GetCharacter(iOurCharacterIndex);
	if(sti(rOurCharacter.ship.type) == SHIP_NOTUSED || LAi_IsDead(rOurCharacter))
	{
		return;
	}
	if (sti(AIBalls.CurrentBallCannonType) >= 0) 
	{
		ref rCannon = GetCannonByType(sti(AIBalls.CurrentBallCannonType));
	}
	else 
	{
		trace("No cannons on " + rOurCharacter.name + " ship! ")
		return;// нет пушки, не запускаем огонь, не считаем урон
	}

	float fHP = Ship_CalculateFireDamage(rCannon, true);
	int iTime = 1000;
	
	// если идёт абордаж или открыт интерфейс
	if (!bAbordageStarted && !sti(InterfaceStates.Launched))
	{
		// ПЕРЕМИРИЕ Speak: пожар от снарядов ГГ/компаньонов (в т.ч. устроенный до сделки) урона не наносит (seaspeak.c)
		if (!SpeakTruce_HitImmune(rOurCharacter, iBallCharacterIndex)) Ship_ApplyHullHitpoints(rOurCharacter, fHP, KILL_BY_BALL, iBallCharacterIndex);

		fTotalFireTime = fTotalFireTime - MakeFloat(iTime) / 1000.0;
	}

	if (fTotalFireTime > 0.0)
	{
		PostEvent(SHIP_FIRE_DAMAGE, iTime, "lllf", iOurCharacterIndex, iBallCharacterIndex, iFirePlaceIndex, fTotalFireTime);
	}
}

void Ship_Serious_Boom(float x, float y, float z)
{
	CreateBlast(x, y, z, 0.0, 0.0, "geo")
	CreateBlast(x, y, z, 0.0, 0.0, "geo2")
	if (sti(InterfaceStates.OtherParticles)) CreateParticleSystem("ShipExplode_ext", x, y, z, 0.0, 0.0, 0.0, 0);
	else CreateParticleSystem("ShipExplode", x, y, z, 0.0, 0.0, 0.0, 0);
	Play3DSound("ship_explosion", x, y, z);
}

void Ship_SetFantomData(ref rFantom)
{
	ref		rBaseShip;

	rBaseShip = GetRealShip(sti(rFantom.Ship.Type));

	int iOptCrew = sti(rBaseShip.OptCrew);

	int iBaseHP = sti(rBaseShip.HP);
	int iCapacity = sti(rBaseShip.Capacity);

	rFantom.ship.Crew.Quantity = iOptCrew - rand(makeint(iOptCrew / 3)) + makeint(iOptCrew / 5); //отп команды +-20 процентов

	rFantom.Ship.HP = iBaseHP;
	if (rand(3) == 2)
	{
		rFantom.Ship.HP = iBaseHP - rand(makeint(iBaseHP / 8));
	}
	if (CheckAttribute(rFantom, "Ship.Masts")) { RepairMasts(rFantom); }
	if (CheckAttribute(rFantom, "Ship.Blots")) { DeleteAttribute(rFantom, "Ship.Blots"); }
	if (CheckAttribute(rFantom, "Ship.Sails")) { DeleteAttribute(rFantom, "Ship.Sails"); }
}

void CharacterUpdateShipFromBaseShip(int iCharacterIndex)
{
	ref rCharacter, rBaseShip;

	rCharacter = GetCharacter(iCharacterIndex);
	rBaseShip = GetRealShip(sti(rCharacter.Ship.Type));

	if (!CheckAttribute(rCharacter,"Ship.Crew.Quantity"))
	{
		rCharacter.ship.Crew.Quantity = rBaseShip.MaxCrew;
	}

	if (!CheckAttribute(rCharacter,"Ship.HP"))
	{
		rCharacter.Ship.HP = rBaseShip.HP;
	}
	if (!CheckAttribute(rCharacter,"Ship.SP"))
	{
		rCharacter.Ship.SP = rBaseShip.SP;
	}

	if (!CheckAttribute(rCharacter,"Ship.Cannons.Charge.Type"))
	{
		rCharacter.Ship.Cannons.Charge.Type = GOOD_BOMBS;
	}

	if (!CheckAttribute(rCharacter,"Ship.Cannons.Type"))
	{
		rCharacter.Ship.Cannons.Type = CANNON_TYPE_CULVERINE_LBS8;
	}

	rCharacter.Ship.Cannons.Borts = "";
}

float Ship_CalcGrapplingPerk(ref rCharacter)
{
	aref	arTmpPerks;
	makearef(arTmpPerks, rCharacter.TmpPerks);

	float fGrapplingPerk = AIShip_isPerksUse(rCharacter.TmpPerks.LongRangeGrappling, 1.0, 1.15);
	return AIShip_isPerksUse(rCharacter.TmpPerks.GrapplingProfessional, fGrapplingPerk, 1.25);

	return fGrapplingPerk;
}

float Ship_CalcMaxAbordageDistance(float fGrappling, float fGrapplingPerk)
{
	return fGrapplingPerk * Bring2Range(15.0, 40.0, 0.0, 1.0, fGrappling);
}

float Ship_CalcMaxAbordageAngle(float fGrappling)
{
	return Bring2Range(0.70, 0.45, 0.0, 1.0, fGrappling);
}

float Ship_CalcMaxAbordageSpeed(float fGrappling)
{
	return Bring2Range(3.0, 5.0, 0.0, 1.0, fGrappling);
}

// ============================================================================
//   ПРОВЕРКИ ВОКРУГ ГГ, МЕЛЬ/ДЕТОНАЦИЯ, ПОКАДРОВЫЙ АПДЕЙТ ПАРАМЕТРОВ,
//   ПЕРКИ/ПОГОДА, ИНИЦИАЛИЗАЦИЯ И ЗАГРУЗКА КОРАБЛЯ
// ============================================================================
void Ship_CheckEnemiesAroundMainCharacter()
{
}

void Ship_CheckMainCharacter()
{
	aref	arUpdate;
	ref		rIsland;
	int		i, iRelation;
	float	x, z, fBestDistance, fDistance;
	float	fMinEnemyDistance = 1000000.0;
	float	fMinFortAbordDistance = 1000000.0; // для работы доплыть до при разбитом форте
	float	fortDistance = 1000000.0; //для разделения дистанций выхода на карту кораблей и фортов
	bool	bAbordageStartNow = false;
	int		iAbordageStartNowCharacter = -1;

	PostEvent(SHIP_CHECK_RELOAD_ENABLE, 1000);

	ref rCharacter = pchar;

	// если идёт абордаж — выходим
	if (bAbordageStarted) { return; }

	// если открыт интерфейс — выходим
	if (sti(InterfaceStates.Launched)) { return; }

	// если ГГ мёртв — пропускаем
	if (LAi_IsDead(rCharacter)) { return; }

	iStormLockSeconds = iStormLockSeconds - 1;
	if (iStormLockSeconds < 0) { iStormLockSeconds = 0; }

	bAbordageFortCanBe = false;
	bAbordageShipCanBe = false;
	bCanEnterToLand = false;
	bDisableSailTo = false;
	sIslandLocator = "";
	sIslandID = "";

	makearef(arUpdate, rCharacter.SeaAI.Update);

	if (!CheckAttribute(arUpdate, "Ships")) { return; }
	if (!CheckAttribute(arUpdate, "Forts")) { return; }
    // boal время в море идёт!!! -->
    locTmpTime = locTmpTime + 0.5;  // можно засунуть проверку locDisableUpdateTime, если нужно выключить
    if (locTmpTime > 9)
	{
		    CalcLocalTime(2);
      		BattleInterface.textinfo.Date.text = XI_convertString("Date:")+GetQuestBookData();
	}
	// boal время в море идёт!!! <--

	x = stf(rCharacter.Ship.Pos.x);
	z = stf(rCharacter.Ship.Pos.z);

	float fOurHP = Ship_GetHP(rCharacter);
	float fOurFencing = stf(rCharacter.TmpSkill.Fencing);
	float fOurGrappling = stf(rCharacter.TmpSkill.Grappling);
	float fOurCrewFencing = (0.1 + fOurFencing * stf(rCharacter.Ship.Crew.Quantity));

	float fOurBoxSizeX = stf(rCharacter.Ship.BoxSize.x) / 2.0;

	bool bGrapplingProfessional = sti(rCharacter.TmpPerks.GrapplingProfessional);

	// макс. параметры абордажа для ГГ
	float fOurGrapplingPerk = Ship_CalcGrapplingPerk(rCharacter);

	float fOurMaxAbordageDistance = Ship_CalcMaxAbordageDistance(fOurGrappling, fOurGrapplingPerk);
	float fOurMaxAbordageAngle = Ship_CalcMaxAbordageAngle(fOurGrappling);
	float fOurMaxAbordageSpeed = Ship_CalcMaxAbordageSpeed(fOurGrappling);

	string sPlayerLocation = rCharacter.location;
	string sPlayerLocator = rCharacter.location.locator;

	// ищем остров
	int iIslandIndex = FindIsland(sPlayerLocation);
	if (iIslandIndex >= 0)
	{
		rIsland = GetIslandByIndex(iIslandIndex);
	}

	aref aShips, aForts;
	makearef(aShips, arUpdate.Ships);
	makearef(aForts, arUpdate.Forts);

	// проверяем форты на абордаж
	int iFortsNum = GetAttributesNum(aForts);
	fBestDistance = 10000000.0;
	for (i=0; i<iFortsNum; i++)
	{
		aref aFort = GetAttributeN(aForts, i);
		iRelation = sti(aFort.relation);

		ref rFortCharacter = GetCharacter(sti(aFort.idx));
		if (LAi_IsDead(rFortCharacter)) { continue; }
		int iFortMode = sti(rFortCharacter.Fort.Mode);
		aref aRLoc = FindIslandReloadLocator(sPlayerLocation, rFortCharacter.location.locator);
		if (!CheckAttribute(aRLoc, "Radius")) { continue; }  // fix
		float fRadius = stf(aRLoc.Radius);
		fDistance = stf(aFort.Distance);
		switch (iFortMode)
		{
			case FORT_NORMAL:
				if (iRelation == RELATION_ENEMY)
				{
					if (fMinEnemyDistance > fDistance) { fortDistance = fDistance; }
				}
				else
				{
					// единоразовое узнавание врага; ложный флаг: самовоз груза по сделке с агентом — 2 суток форт нации-заказчика не разоблачает (FortAgent_FortGrace) -->
					if (FortAgent_FortGrace(rFortCharacter) || CheckAttribute(rFortCharacter, "CheckFlagYet")) continue; // при перемирии/самовозе груза не разнюхивают
					if (CheckAttribute(rFortCharacter, "CheckFlagDate") && GetNpcQuestPastDayParam(rFortCharacter, "CheckFlagDate") <= 1) continue; // второй раз подряд не смотрим
					if (fDistance >= (MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER_FORT * 1.5)) continue;
					rFortCharacter.CheckFlagYet = true; // всё, проверил
					Ship_CheckFlagEnemy(rFortCharacter);
					SaveCurrentNpcQuestDateParam(rFortCharacter, "CheckFlagDate");
					// проверка на ложный флаг <--
				}
			break;
			case FORT_DEAD:
                if (iRelation == RELATION_FRIEND) { continue; }
				if (fDistance > fRadius) { continue; }
				if (fBestDistance > fDistance)
				{
					fBestDistance = fDistance;

					bCanEnterToLand = true;

					sIslandID = rIsland.id;
					makearef(arIslandReload, rIsland.reload);
					sIslandLocator = aRLoc.GoAfterBoard.location;
				}
			break;
			case FORT_ABORDAGE:
                if (iRelation == RELATION_FRIEND) { continue; }
				if (iRelation == RELATION_ENEMY)
				{
					if (fMinFortAbordDistance > fDistance) { fMinFortAbordDistance = fDistance; } //fix входа в порт
				}
				if (fDistance > fRadius) { continue; }
				if (!Character_IsAbordageEnable(rFortCharacter) || bDisableMapEnter)  { continue; } // bDisableMapEnter - значит враги рядом, нефиг брать форт под пушками кораблей boal

				if (fBestDistance > fDistance)
				{
					fBestDistance = fDistance;

					bAbordageFortCanBe = true;
					sAbordageLocator = rFortCharacter.location.locator;
					iAbordageFortEnemyCharacter = sti(rFortCharacter.index);
				}
			break;
		}
	}

	// проверяем корабли на абордаж
	int iShipsNum = GetAttributesNum(aShips);
	fBestDistance = 10000000.0;
	for (i=0; i<iShipsNum; i++)
	{
		aref aShip = GetAttributeN(aShips, i);

		ref rShipCharacter = GetCharacter(sti(aShip.idx));
		fDistance = stf(aShip.Distance);		// distance between player ship and this ship
		iRelation = sti(aShip.relation);

		if (LAi_IsDead(rShipCharacter))
		{
			continue;
		}
		// сдавшийся (белый флаг) - не враг, но кошку по нему дать НАДО (грабёж/захват при сближении)
		bool bSurrTarget = CheckAttribute(rShipCharacter, "surrendered");
		if (iRelation != RELATION_ENEMY && !bSurrTarget)
		{
			continue;
		}
		if (iRelation == RELATION_ENEMY && fMinEnemyDistance > fDistance) // учёт ближайшего ВРАГА - только по врагам
		{
			fMinEnemyDistance = fDistance;
		}

		if (!Character_IsAbordageEnable(rShipCharacter))
		{
			continue;
		}

		float fAng = stf(aShip.d_ay);						// it's dot.ay between ships
		float fRelativeSpeed = stf(aShip.RelativeSpeed);	// it's relative speed about player ship
		float fEnemyBoxSizeX = stf(rShipCharacter.Ship.BoxSize.x) / 2.0;

		bool bOurAngleTest = abs(fAng) > fOurMaxAbordageAngle && fRelativeSpeed < fOurMaxAbordageSpeed;
		float fAbordageDistance = fDistance - fEnemyBoxSizeX - fOurBoxSizeX;

		// проверка нашего корабля против врага
		if (bGrapplingProfessional || bOurAngleTest)
		{
			if (fAbordageDistance < fOurMaxAbordageDistance && fBestDistance > fDistance)
			{
				// абордаж доступен
				fBestDistance = fDistance;
				bAbordageShipCanBe = true;
				iAbordageShipEnemyCharacter = sti(rShipCharacter.index);
			}
		}

		// проверка вражеского корабля против нашего
		float fEnemyGrappling = stf(rShipCharacter.TmpSkill.Grappling);

		// макс. параметры абордажа для ГГ
		float fEnemyGrapplingPerk = Ship_CalcGrapplingPerk(rShipCharacter);

		float fEnemyMaxAbordageDistance = Ship_CalcMaxAbordageDistance(fEnemyGrappling, fEnemyGrapplingPerk);
		float fEnemyMaxAbordageAngle = Ship_CalcMaxAbordageAngle(fEnemyGrappling);
		float fEnemyMaxAbordageSpeed = Ship_CalcMaxAbordageSpeed(fEnemyGrappling);

		bool bEnemyAngleTest = abs(fAng) > fEnemyMaxAbordageAngle && abs(fRelativeSpeed) < fEnemyMaxAbordageSpeed;
		if (!bEnemyAngleTest) continue;
		if (fAbordageDistance >= fEnemyMaxAbordageDistance) continue;
		// может, встречный корабль сам хочет нас абордировать?
		float fEnemyFencing = stf(rShipCharacter.TmpSkill.Fencing);
		float fEnemyCrewFencing = (0.1 + fEnemyFencing * stf(rShipCharacter.Ship.Crew.Quantity));
		float fRatio = fEnemyCrewFencing / fOurCrewFencing;
		if (sti(rShipCharacter.nation) == PIRATE) fRatio = fRatio * 1.6;
		if (fRatio > 1.2)
		{
			iAbordageStartNowCharacter = sti(rShipCharacter.index);
			bAbordageStartNow = true;
			break;
		}
	}

	// проверка возможности причалить к локациям
	if (iIslandIndex >= 0 && fMinEnemyDistance > MIN_ENEMY_DISTANCE_TO_DISABLE_ENTER_2_LOCATION)
	{
		if (Island_isReloadEnableGlobal(sPlayerLocation))
		{
			aref arReload;
			makearef(arReload, rIsland.Reload);

			int iNumLocators = GetAttributesNum(arReload);
			for (int j=0; j<iNumLocators; j++)
			{
				aref arLocator;
				arLocator = GetAttributeN(arReload, j);
				string sLocatorName = GetAttributeName(arLocator);
				if (Island_isReloadFort(sPlayerLocation, arLocator.name)) { continue; }
				if (!Island_isReloadEnableLocal(sPlayerLocation, arLocator.name)) { continue; }
				if(!CheckAttribute(rIsland, "reload." + sLocatorName + ".x") || !CheckAttribute(rIsland, "reload." + sLocatorName + ".z"))
				{
					continue;
				}

				float x1 = stf(rIsland.reload.(sLocatorName).x);
				float z1 = stf(rIsland.reload.(sLocatorName).z);
				float r = stf(rIsland.reload.(sLocatorName).radius);
				// без лишнего sqrt
				r *= r;
				if ((sqr(x1 - x) + sqr(z1 - z)) < r)
				{
					bCanEnterToLand = true;
					sIslandID = rIsland.id;
					makearef(arIslandReload, rIsland.reload);
					sIslandLocator = rIsland.reload.(sLocatorName).name;

                    int nFile = LanguageOpenFile("LocLables.txt");
					string MoorName = LanguageConvertString(nFile, rIsland.reload.(sLocatorName).label);
                    LanguageCloseFile(nFile);
					if (!CheckAttribute(pchar, "MoorName")) pchar.MoorName = " ";
					if (MoorName != pchar.MoorName)
                    {
						if (pchar.MoorName != " " && MoorName != " ")
						{
							boal_soundOn = false;
							PlaySound("interface\_Yakordrop.wav");
						}
						pchar.MoorName = MoorName;
                    }

					break;
				}
			}
			BattleInterface.textinfo.Location.text = GetBILocationName();
		}
	}
	else
	{
		pchar.Moorname = " ";
		BattleInterface.textinfo.Location.text = GetBILocationName();
	}


	// разрешение/запрет выхода на карту
	// обновление режима людей на корабле
	int iPrevShipPOSMode = sti(rCharacter.Ship.POS.Mode);
	rCharacter.Ship.POS.Mode = SHIP_SAIL;

	bDisableMapEnter = false;
	if (fMinEnemyDistance < MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER)
	{
		bCanEnterToLand = false;
		bDisableMapEnter = true;

		rCharacter.Ship.POS.Mode = SHIP_WAR;
	}
	if (fortDistance < MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER_FORT)
	{
		bCanEnterToLand = false;
		bDisableMapEnter = true;
		//#20190205-02
		rCharacter.Ship.POS.Mode = SHIP_WAR;
	}
    // boal fix входа в порт -->
    if (fMinFortAbordDistance < MIN_ENEMY_DISTANCE_TO_DISABLE_ENTER_2_LOCATION)
    {
        bCanEnterToLand = false;
    }
	if (iStormLockSeconds)
	{
		bDisableMapEnter = true;
		bCanEnterToLand = false;
		bDisableSailTo = true;

		rCharacter.Ship.POS.Mode = SHIP_STORM;
	}

	// новая музыка
	bool bMakeCurrentUpdate = bUpdatePosMode;
	if( iVicSoundTime<=0 && sti(rCharacter.Ship.POS.Mode) != iPrevShipPOSMode ) {
		bMakeCurrentUpdate = true;
	}
	if( bMakeCurrentUpdate )
	{
		switch (sti(rCharacter.Ship.POS.Mode))
		{
			case SHIP_SAIL:
				if (!Whr_IsStorm())
				{
					seaAlarmed = false;
					// LEO (баг 1): НЕ играть победу, если враги «исчезли» из-за СМЕНЫ ФЛАГА (стали дружественны), а не разгрома.
					//   DisableChangeFlagMode держится ~15с во время смены флага -> подавляем ложную победу в этом окне.
					if (iPrevShipPOSMode == SHIP_WAR && !CheckAttribute(pchar, "DisableChangeFlagMode"))
					{
                   		Ship_PlayVictory("music_ship_victory", 12000);
					} // fix boal
					else
					{
						FadeOutMusic(3); //fix
						if(Whr_IsDay()) SetMusic("music_sea_day");
						else SetMusic("music_sea_night");
					}
				}
				else
				{
					FadeOutMusic(3); //fix
					PlayStereoOGG("music_End_storm");
				}
			break;
			case SHIP_WAR:
				FadeOutMusic(3); // fix
				seaAlarmed = true;
				SetMusic("music_sea_battle");
			break;
		}

		bUpdatePosMode = false;
	}

	// если квест запрещает выход на карту
	if (bQuestDisableMapEnter) { bDisableMapEnter = true; }
	if (Boarding_RetryLeft(pchar) > 0) bAbordageShipCanBe = false;
	if (Boarding_Busy(pchar))
	{
		bAbordageShipCanBe = false;
		bAbordageFortCanBe = false;
		bCanEnterToLand = false;
		bDisableMapEnter = true;
		bDisableSailTo = true;
		return;
	}

	// авто-старт абордажа
	if (bAbordageStartNow)
	{
		ref rTargetCharacter = GetCharacter(iAbordageStartNowCharacter);
		if (GetRelation(sti(rCharacter.index), iAbordageStartNowCharacter) == RELATION_ENEMY && !LAi_IsDead(rCharacter) && !LAi_IsDead(rTargetCharacter))
		{
			iAbordageShipEnemyCharacter = iAbordageStartNowCharacter;
			Sea_AbordageLoad(SHIP_ABORDAGE, false);
		}
	}
}

void Ship_OnStrandNext()
{
	ref	rCharacter = GetCharacter(GetEventData());
	if (LAi_IsDead(rCharacter)) { return; }

	bool bOnStrand = sti(rCharacter.Ship.Strand);
	if (!bOnStrand)
	{
		int iSoundID = sti(rCharacter.Ship.Strand.SndID);
		if (iSoundID != 0) { StopSound(sti(rCharacter.Ship.Strand.SndID), 3000); }
		rCharacter.Ship.Strand.SndID = SOUND_INVALID_ID;
		return;
	}
	if (!CheckAttribute(rCharacter, "AlwaysSandbankManeuver"))  //boal
	{
		float fCurrentSpeedZ = stf(rCharacter.Ship.Speed.z);
		float fHP = 4.0 * fCurrentSpeedZ / 7.0;
		Ship_ApplyHullHitpoints(rCharacter, fHP, KILL_BY_ISLAND, -1);
	}
	PostEvent(SHIP_TO_STRAND_NEXT, 100 + rand(100), "l", sti(rCharacter.index));
}

void Ship_OnStrand()
{
	ref rCharacter = GetCharacter(GetEventData());
	bool bNewStrand = GetEventData();

	bool bPrevStrand = sti(rCharacter.Ship.Strand);

	rCharacter.Ship.Strand = bNewStrand;

	float x = stf(rCharacter.Ship.Pos.x);
	float y = stf(rCharacter.Ship.Pos.y);
	float z = stf(rCharacter.Ship.Pos.z);

	if (bPrevStrand == 0 && bNewStrand == 1)
	{
		PostEvent(SHIP_TO_STRAND_NEXT, 100, "l", sti(rCharacter.index));
		rCharacter.Ship.Strand.SndID = Play3DSoundComplex("ship_aground", x, y, z, true, false);
	}
}

// малый взрыв
void Ship_DetonateSmall()
{
	ref		rCharacter = GetCharacter(GetEventData());
	aref	arCharShip; makearef(arCharShip, rCharacter.Ship);

    if (bAbordageStarted) { return; }
	if (sti(InterfaceStates.Launched)) { return; }

	float x = stf(arCharShip.Pos.x);
	float y = stf(arCharShip.Pos.y);
	float z = stf(arCharShip.Pos.z);

	if (y > -3.0)
	{
		int iSClass = GetCharacterShipClass(rCharacter);
		PostEvent(SHIP_DETONATE_SMALL, 200 + iSClass * 300 + rand(900), "l", sti(rCharacter.index));
	}

	float bx = arCharShip.BoxSize.x;
	float by = arCharShip.BoxSize.y;
	float bz = arCharShip.BoxSize.z;

	float fCos = cos(stf(arCharShip.Ang.y));
	float fSin = sin(stf(arCharShip.Ang.y));

	float x1, y1, z1;
	x1 = (frnd() - 0.5) * bx;
	z1 = (frnd() - 0.5) * bz;

	RotateAroundY(&x1, &z1, fCos, fSin);

	y1 = 1.0 + y + frnd() * 3.0;
	Ship_Serious_Boom(x1 + x, y1, z1 + z);
	CreateParticleSystem("blast", x1 + x, y1, z1 + z, 0.0, 0.0, 0.0, 0);
}

// большой взрыв на корабле
void Ship_Detonate(ref rCharacter, bool bMakeSmallBoom, bool bMakeDead)
{
	if (!CheckAttribute(rCharacter, "Ship.Pos.x"))
	{
		Trace("Ship_Detonate: Can't find Ship.Pos in character with id: " + rCharacter.id);
		return;
	}
	aref	arCharShip; makearef(arCharShip, rCharacter.Ship);

	float x = stf(arCharShip.Pos.x);
	float y = stf(arCharShip.Pos.y);
	float z = stf(arCharShip.Pos.z);

	int iMinBoom = MakeInt(Bring2Range(6.0, 2.0, 1.0, 7.0, stf(GetCharacterShipClass(rCharacter))));

	float bx = arCharShip.BoxSize.x;
	float by = arCharShip.BoxSize.y;
	float bz = arCharShip.BoxSize.z;

	float fCos = cos(stf(arCharShip.Ang.y));
	float fSin = sin(stf(arCharShip.Ang.y));

	bool isBoomAlready = false;
	int iMax = iMinBoom + rand(2); //fix
	for (int i=0; i<iMax; i++)
	{
		float x1, y1, z1;
		x1 = (frnd() - 0.5) * bx;
		z1 = (frnd() - 0.5) * bz;

		RotateAroundY(&x1, &z1, fCos, fSin);

		y1 = 1.0 + y + frnd() * 3.0;
		if(isBoomAlready == false)
		{
			Ship_Serious_Boom(x1 + x, y1, z1 + z);
		}
		isBoomAlready = true;
		CreateParticleSystem("blast_inv_ext", x1 + x, y1, z1 + z, 0.0, 0.0, 0.0, 0);
		CreateParticleSystem("blast_bomb_ptr", x1 + x, y1, z1 + z, 0.0, 0.0, 0.0, 0);
		if (GetCharacterShipClass(rCharacter) < 4) CreateBlast(x, y, z, 0.0, 0.0, "CANNONDESTR2");
		else CreateBlast(x, y, z, 0.0, 0.0, "CANNONDESTR1");
	}
	if (bMakeDead) { ShipDead(sti(rCharacter.Index), KILL_BY_SPY, -1); }

	if (bMakeSmallBoom) { PostEvent(SHIP_DETONATE_SMALL, 600 + rand(1000), "l", sti(rCharacter.index)); }
	
	bool bLargeExplosion = rand(1) == 0 && GetCharacterShipClass(rCharacter) < 6;
	if (bLargeExplosion && sti(InterfaceStates.OtherParticles))
	{
		CreateParticleSystem("Cruit_camera", x, y, z,0,1.0,0,0);
		CreateParticleSystem("ShipExplode2", x, y, z,0,1.0,0,0);
		CreateParticleSystem("ShipExplode3", x, y, z,0,1.0,0,0);
		CreateParticleSystem("ShipFire2", x, y, z,0,1.0,0,0);
		CreateParticleSystem("blast_big", x, y, z,0,1.0,0,0);
		CreateParticleSystem("blast_wave", x, y, z,0,1.0,0,0);
		Play3DSound("Cruit_camera", stf(rCharacter.Ship.pos.x), stf(rCharacter.Ship.pos.y), stf(rCharacter.Ship.pos.z));
		SendMessage(rCharacter, "lfff", MSG_SHIP_DETONATE, 5.0, 4.0, 1500.0);
	}
	// LEO: Реальная детонация и крупный случайный взрыв; графическая опция не меняет механику.
	if (bMakeDead || bLargeExplosion) ShipDestruction_FatalDetonation(rCharacter);
}

float AIShip_isPerksUse(string sPerk, float fOff, float fOn)
{
	if (sti(sPerk)) { return fOn; }
	return fOff;
}

// СОБЫТИЕ Ship_UpdateParameters: пересчёт динамических параметров раз в секунду.
// Хелпер: тактика по текущей цели — абордаж/атака/бегство по силам и скоростям; при бегстве оптимизации (паруса, огни, сброс груза/мины). Вынесено 1:1.
void Ship_UP_TargetTactics(ref rCharacter, ref rShip)
{
	aref arCharShip; makearef(arCharShip, rCharacter.Ship);
	if (sti(rCharacter.SeaAI.Task.Target) <= 0) return;

	ref rTargetCharacter = GetCharacter(sti(rCharacter.SeaAI.Task.Target));
	bool bSkipTarget = false;
	if (!CheckAttribute(rTargetCharacter, "location")) bSkipTarget = true;
	if (rTargetCharacter.location == "none") bSkipTarget = true;
	if (rTargetCharacter.location != rCharacter.location) bSkipTarget = true;
	if (bSkipTarget) return;

	ref rTargetCharacterShip = GetRealShip(sti(rTargetCharacter.Ship.type));
	float fDistance = Ship_GetDistance2D(rCharacter, rTargetCharacter);
	float fEnemyLeadership = stf(rTargetCharacter.TmpSkill.Leadership);
	float fEnemyFencing = stf(rTargetCharacter.TmpSkill.Fencing);
	// LEO ФИКС: команды были ПЕРЕПУТАНЫ — вражья сила считалась с НАШЕЙ командой, наша с ВРАЖЬЕЙ,
	// fRatio инвертировался (выше наш фехт -> НИЖЕ шанс абордажа). Ставим команды на места + our/enemy.
	float fEnemyCrewFencing = (0.1 + fEnemyLeadership + fEnemyFencing * stf(rTargetCharacter.Ship.Crew.Quantity)); // враг: ЕГО скиллы × ЕГО команда
	float fOurLeadership = stf(rCharacter.TmpSkill.Leadership);
	float fOurFencing = stf(rCharacter.TmpSkill.Fencing);
	float fOurCrewFencing = (0.1 + fOurLeadership + fOurFencing * stf(rCharacter.Ship.Crew.Quantity)); // мы: НАШИ скиллы × НАША команда
	float fRatio = fOurCrewFencing / fEnemyCrewFencing; // >1 => мы сильнее в абордажной свалке -> идём на абордаж
	if (sti(rCharacter.nation) == PIRATE) fRatio = fRatio * 1.6;
	if (sti(rCharacter.rank) > sti(rTargetCharacter.rank)) fRatio = fRatio * 1.2;

	// сильнее + быстрее цели -> запоминаем намерение на абордаж (не сбивая таск).
	// LEO: жёсткий гейт — абордаж ТОЛЬКО при численном перевесе команды И превосходстве по ФАКТ. скорости
	// С УЧЁТОМ ВЕТРА (Ship.MaxSpeedZ, см. Sea_ApplyMaxSpeedZ) — иначе не догнать уклоняющуюся цель.
	// LEO: спид-гейт БЕЗ обхода на 300м (было: ближе AIBEH_ABORD_CLOSE скорость игнорировалась ВООБЩЕ -> ИИ шёл
	//   на абордаж, заведомо не догоняя). Обход оставлен только вплотную (AIBEH_ABORD_NOCATCH - там уже кошки).
	//   Дальше - ЧЕСТНАЯ догоняемость Ship_CanCatch (моя скорость НА КУРСЕ ЦЕЛИ против её), а не сырой MaxSpeedZ,
	//   который скачет от текущего курса к ветру (корабль в довороте показывал ложно-низкую скорость).
	bool bCanCatch = false;
	if (fDistance < AIBEH_ABORD_NOCATCH) bCanCatch = true;
	else bCanCatch = Ship_CanCatch(rCharacter, sti(rTargetCharacter.index));
	// LEO: компаньон под ПРЯМЫМ приказом (нет CompanionAutoEngage) НЕ эскалирует в абордаж — держит атаку с дистанции.
	bool bOrderedCompanion = IsCompanion(rCharacter) && !CheckAttribute(rCharacter, "CompanionAutoEngage");
	// LEO: задержка абордажа после входа в бой — сперва ДАЛЬНИЙ бой (виден фокус-огонь), потом сцепки. Время ставит SeaLogin.
	bool bBoardDelayOk = true;
	if (CheckAttribute(pchar, "SeaBoardEnableTime")) { if (GetSeaTime() < sti(pchar.SeaBoardEnableTime)) bBoardDelayOk = false; }
	// LEO: поза ЭСКАДРЫ смещает решение: BOARD-раш — ниже порог, перевес команды не нужен; KITE — сцепка запрещена.
	// LEO: осадники (SiegeAssault) и корабли с ЦЕЛЬЮ=ФОРТ не ведут здесь ship-vs-ship абордаж/ретаргет (форт не отсекается
	//   location-гейтом -> Fix-B дёргал цель осадников каждый тик = мерцание осады); их цель ведёт Ship_CS_SiegeAttack.
	bool bNoBoardTac = false;
	if (CheckAttribute(rCharacter, "SiegeAssault")) bNoBoardTac = true;
	if (bIsFortAtIsland) { if (iFortCommander >= 0) { if (sti(rTargetCharacter.index) == iFortCommander) bNoBoardTac = true; } }
	float fBoardGate = 1.2;
	bool bBoardCrewOk = GetCrewQuantity(rCharacter) > GetCrewQuantity(rTargetCharacter);
	int iSqPosture = Ship_Squad_Posture(rCharacter);
	if (iSqPosture == SQUAD_POS_BOARD) { fBoardGate = 0.7; bBoardCrewOk = true; }
	if (iSqPosture == SQUAD_POS_KITE) fBoardGate = 999999.0;
	// LEO: ГИСТЕРЕЗИС абордажа (анти-мерцание ABORDAGE<->ATTACK). Гейт считается каждый кадр; без раздельных enter/exit
	//   порогов корабль у границы 1.2 мигал (fRatio/скорость/команда дрейфуют). Раз решив — ДЕРЖИМ по СНИЖЕННОМУ порогу
	//   выхода, латч сносим ТОЛЬКО при реальном провале ниже exit-порога (образец латча FLEE ~1997).
	bool bBoardLatched = CheckAttribute(rCharacter, "Ship_SetTaskAbordage");
	float fBoardActiveGate = fBoardGate;
	bool bBoardCanCatchOk = bCanCatch;
	bool bBoardCrewActiveOk = bBoardCrewOk;
	if (bBoardLatched)
	{
		fBoardActiveGate = fBoardGate * AIBEH_BOARD_EXIT_FRAC; // exit-порог латча (1.2×0.90=1.08 > ~1.0 равного боя -> в AI-vs-AI латч падает, не «все на абордаж»); было 0.75
		bBoardCanCatchOk = true;              // уже в абордаже: не бросаем из-за скачка MaxSpeedZ у порога (пока цель БЛИЗКО)
			// LEO: но если цель честно оторвалась дальше грейса (крейсерская скорость, курс-независимо) — реальный отрыв,
			//   снимаем анти-джиттер -> латч упадёт (иначе медленный, но сильный враг вечно тащился в кильватере, не стреляя).
		if (fDistance >= AIBEH_ABORD_GRACE) { if (!Ship_CanCatch(rCharacter, sti(rTargetCharacter.index))) bBoardCanCatchOk = false; }
		if (!bBoardCrewOk)                    // релакс команды к ~ПАРИТЕТУ (было 0.8 -> тоже подкармливал липкий латч в равном бою; теперь ~0.95)
		{
			if (GetCrewQuantity(rCharacter) * 20 >= GetCrewQuantity(rTargetCharacter) * 19) bBoardCrewActiveOk = true;
		}
	}
	// LEO: ЖЁСТКИЙ потолок дистанции. ВЫДАЁМ таск только вблизи (AIBEH_ABORD_MAXDIST) - иначе ИИ получал абордаж
	//   за сотни метров и тупо пёр по прямой на цель/ГГ, не стреляя и ловя залпы без ответа. Дальше пусть работает
	//   обычный бой (ATTACK), а сцепку решает уже сблизившись. Залатчившийся держим до грейса (анти-мерцание).
	bool bBoardDistOk = false;
	if (fDistance <= AIBEH_ABORD_MAXDIST) bBoardDistOk = true;
	if (bBoardLatched) { if (fDistance <= AIBEH_ABORD_GRACE) bBoardDistOk = true; } // уже сцепляется - не сбиваем на мелком отрыве
	bool bBoardWant = false;
	if (!bNoBoardTac && !bOrderedCompanion && bBoardDelayOk && bBoardDistOk && fRatio > fBoardActiveGate && bBoardCrewActiveOk && bBoardCanCatchOk) bBoardWant = true;
	if (bBoardWant)
	{
		if (GetRelation(sti(rCharacter.index), sti(rTargetCharacter.index)) == RELATION_ENEMY)
		{
			Ship_SetTaskAbordage(SECONDARY_TASK, sti(rCharacter.index), sti(rTargetCharacter.index));
			rCharacter.Ship_SetTaskAbordage = true;
		}
	}
	else
	{
		bool bHadLatch = bBoardLatched;
		DeleteAttribute(rCharacter, "Ship_SetTaskAbordage");
		// LEO: латч УПАЛ из-за реального отрыва -> не тащимся в кильватере: берём ДОСТИЖИМУЮ цель (ПГГ) через
		//   Ship_NearestReachableEnemy, иначе погоня-с-огнём по той же таском ATTACK (борт-пассы). Гейт — не спамить SendMessage.
		if (!bNoBoardTac && bHadLatch && fDistance >= AIBEH_ABORD_GRACE)
		{
			if (!Ship_CanCatch(rCharacter, sti(rTargetCharacter.index)))
			{
				int iAltT = Ship_NearestReachableEnemy(rCharacter);
				if (iAltT >= 0 && iAltT != sti(rTargetCharacter.index))
					Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), iAltT); // переключаемся на достижимого (ПГГ)
				else if (sti(rCharacter.SeaAI.Task) != AITASK_ATTACK)
					Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), sti(rTargetCharacter.index)); // погоня-с-огнём по той же
			}
		}
	}

	// убегающий: оптимизации (паруса вверх, гашение огней, вероятный сброс груза/мины)
	if (sti(rCharacter.SeaAI.Task) == AITASK_RUNAWAY)
	{
		Ship_SetSailState(sti(rCharacter.index), 1);  // to_do нужно проверять, может довольно одного раза или в ядре будет дурить
		if (sti(rCharacter.Ship.Lights) == 1) Ship_SetLightsOff(rCharacter, 15.0, true, true, false);
		float speedDiff = FindShipSpeed(rCharacter, rShip) - FindShipSpeed(rTargetCharacter, rTargetCharacterShip);
		if ((speedDiff > -1) && (speedDiff > 3) && (rand(100) > 80)) PostEvent(SHIP_DROP_GOOD, 1000, "a", rCharacter); // не выкидывать, если всё равно не убежать / убежать и так можно
		if (sti(rCharacter.Ship.SP) < 60 && GetCargoGoods(rCharacter, GOOD_POWDER) >= MINE_POWDER*2 && rand(39) == 5) SetMineFree(rCharacter, 1); // сброс мины
		return;
	}

	// решение об абордаже ИИ по ГГ: только по своей цели-ГГ, живым и на дистанции/угле/скорости кошки
	if (sti(rCharacter.SeaAI.Task) != AITASK_ABORDAGE) return;
	if (GetRelation(sti(rCharacter.index), sti(rTargetCharacter.index)) != RELATION_ENEMY) return;
	if (LAi_IsDead(rCharacter) || LAi_IsDead(rTargetCharacter)) return;
	if (sti(rTargetCharacter.index) != nMainCharacterIndex) return;

	float fCharacterGrappling = stf(rCharacter.TmpSkill.Grappling);
	float fCharacterGrapplingPerk  = Ship_CalcGrapplingPerk(rCharacter);
	float fCharacterDistance = Ship_CalcMaxAbordageDistance(fCharacterGrappling, fCharacterGrapplingPerk);
	float fCharacterAngle = Ship_CalcMaxAbordageAngle(fCharacterGrappling);
	float fCharacterSpeed = Ship_CalcMaxAbordageSpeed(fCharacterGrappling);

	// LEO: SeaAI.Update.Ships у ИИ обновляется не каждый кадр; берём курс и скорость из самого корабля.
	float fAng = cos(stf(arCharShip.Ang.y) - stf(rTargetCharacter.Ship.Ang.y));
	float fRelativeSpeed = abs(stf(arCharShip.Speed.z) - fAng * stf(rTargetCharacter.Ship.Speed.z)) * 0.514444;
	float fEnemyBoxSizeX = stf(rTargetCharacter.Ship.BoxSize.x) / 2.0;
	float fCharacterBoxSizeX = stf(rCharacter.Ship.BoxSize.x) / 2.0;
	float fAbordageDistance = fDistance - fEnemyBoxSizeX - fCharacterBoxSizeX;
	if (fAbordageDistance >= fCharacterDistance || abs(fAng) <= fCharacterAngle || fRelativeSpeed >= fCharacterSpeed) return;

	if (fRatio > fBoardActiveGate) // LEO: тот же гистерезис-порог, что и у гейта выше — иначе ГГ-ветка сама мигала ABORDAGE(грузим сцепку)<->ATTACK(5467) у голого 1.2
	{
		iAbordageShipEnemyCharacter = sti(rCharacter.index);
		if (!CheckAttribute(rCharacter, "CantBoardEnemies")) Sea_AbordageLoad(SHIP_ABORDAGE, false);
		return;
	}
	if (stf(rCharacter.ship.hp) > (stf(rTargetCharacter.ship.hp) / 4) || GetCrewQuantity(rCharacter) > GetCrewQuantity(rTargetCharacter)) //boal fix
		Ship_SetTaskAttack(SECONDARY_TASK, sti(rCharacter.index), sti(rTargetCharacter.index));
	else
		Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), sti(rCharacter.SeaAI.Task.Target));
}

// ============ АВТО-ПЕРКИ ИИ В МОРЕ (LEO) ============
// ИИ-корабли (враги и компаньоны, кроме флагмана игрока) сами жмут боевые перки, если перк есть и не на КД.
// Дёргается из Ship_UpdateParameters каждый тик, реально считает раз в ~AIPERK_CHECK_TICKS (троттл).

// попытка активировать перк: есть + не на КД (GetCharacterPerkUsing) + не активен -> активируем. true=активировали.
bool Ship_AI_TryPerk(ref rCharacter, string perk)
{
	if (!GetCharacterPerkUsing(rCharacter, perk)) return false;                    // нет перка ЛИБО на КД
	if (CheckAttribute(rCharacter, "perks.list." + perk + ".active")) return false; // уже активен
	ActivateCharacterPerk(rCharacter, perk);
	return true;
}

// всплывашка «применён перк» над кораблём (зелёная); уважает мастер-опцию «Подписи над кораблями»
void Ship_AI_PerkPopup(ref rCharacter, string sKey)
{
	if (CheckAttribute(&InterfaceStates, "ShipCap")) { if (sti(InterfaceStates.ShipCap) == 0) return; }
	float x = stf(rCharacter.Ship.Pos.x);
	float y = stf(rCharacter.Ship.Pos.y) + 10.0;
	float z = stf(rCharacter.Ship.Pos.z);
	string sTxt = LanguageConvertString(iSeaSectionLang, sKey);
	SendMessage(&BattleInterface, "lfffslff", BI_MSG_SEAPOPUP, x, y, z, sTxt, argb(255, 120, 230, 120), GetShipCapNotifScale(), GetShipCapNotifDistance());
}

// LEO: «идёт реальный бой» — гейт тактических перков. Раньше сигналом был Ship.FiredBort, но он ставится один раз
//   (shipBortFire.c) и НИКОГДА не чистится -> навсегда true (ИИ в порту жал перки). Теперь: враг + движемся/по нам прилетело.
bool Ship_AI_InActiveCombat(ref rMe, int idxMe, int tgt, float hppNow, float prevHpp)
{
	if (GetRelation(idxMe, tgt) != RELATION_ENEMY) return false; // цель на друге/форте/нейтрале — это не бой
	if (stf(rMe.Ship.Speed.z) > 0.5) return true;                // в порту стоит ~0; в бою заметно движется
	if (hppNow < prevHpp - 0.01) return true;                    // корпус просел за тик => по нам стреляют
	return false;
}

void Ship_AI_AutoUsePerks(ref rCharacter)
{
	if (!bSeaActive) return;
	if (bAbordageStarted) return;
	int idx = sti(rCharacter.index);
	if (idx == GetMainCharacterIndex()) return; // игрок сам жмёт хоткеи
	if (LAi_IsDead(rCharacter)) return;

	// РЕМОНТ проверяем КАЖДЫЙ тик (срочно — иначе под огнём корабль просаживается ниже порога между проверками).
	float hpp = GetHullPercent(rCharacter);
	float spp = GetSailPercent(rCharacter);

	// LEO: слепок HP для детектора «по нам стреляют» — пишем КАЖДЫЙ тик ДО любых return (иначе return ремонта рвут сигнал урона).
	float prevHpp = hpp;
	if (CheckAttribute(rCharacter, "Tmp.LastHpp")) prevHpp = stf(rCharacter.Tmp.LastHpp);
	rCharacter.Tmp.LastHpp = hpp;

	// 1) РЕМОНТ. ПРИОРИТЕТ — InstantRepair при корпусе ИЛИ парусах <40%.
	//    LEO: убран ЧИТ (дарились 300 досок+парусины) — теперь гейтим честно по грузу (CheckInstantRepairCondition); нет материала/КД -> бесплатный LightRepair.
	bool bWantHull = (hpp < 40.0);
	bool bWantSail = (spp < 40.0);
	if (bWantHull || bWantSail)
	{
		bool bCanInstant = false;
		if (bWantHull) { if (GetCargoGoods(rCharacter, GOOD_PLANKS)    > 0) bCanInstant = true; }
		if (bWantSail) { if (GetCargoGoods(rCharacter, GOOD_SAILCLOTH) > 0) bCanInstant = true; }
		if (bCanInstant)
		{
			if (Ship_AI_TryPerk(rCharacter, "InstantRepair"))
			{
				ActivateSpecRepair(rCharacter, 1);
				Ship_AI_PerkPopup(rCharacter, "perk_InstantRepair");
				return;
			}
		}
		// нет материала ЛИБО InstantRepair на КД -> бесплатный лёгкий ремонт при критическом корпусе/парусах
		if (hpp < 20.0 || spp < 20.0)
		{
			if (Ship_AI_TryPerk(rCharacter, "LightRepair")) { ActivateSpecRepair(rCharacter, 0); Ship_AI_PerkPopup(rCharacter, "perk_LightRepair"); return; }
		}
	}

	// тактические перки (перезарядка/разворот) НЕ срочны -> троттл ~раз/сек (счётчик со случайным стартом — декорреляция)
	int chk = 0;
	if (CheckAttribute(rCharacter, "Tmp.fPerkChk")) chk = sti(rCharacter.Tmp.fPerkChk);
	else chk = rand(40);
	if (chk < 40) { rCharacter.Tmp.fPerkChk = chk + 1; return; }
	rCharacter.Tmp.fPerkChk = 0;

	// тактические перки — нужна живая цель
	if (!CheckAttribute(rCharacter, "SeaAI.Task.Target")) return;
	if (rCharacter.SeaAI.Task.Target == "") return;
	int tgt = sti(rCharacter.SeaAI.Task.Target);
	if (tgt < 0) return;
	if (LAi_IsDead(&characters[tgt])) return;
	float fDist = Ship_GetDistance2D(rCharacter, &characters[tgt]);

	// LEO: единый гейт «реальный бой» — цель враждебна И (движемся в бою ЛИБО по нам прилетело). Отсекает порт/стоянку.
	bool bInCombat = Ship_AI_InActiveCombat(rCharacter, idx, tgt, hpp, prevHpp);

		// LEO: цель=ФОРТ? Форт неподвижен: нет Ship.MaxSpeedZ (было "missed attribute: maxspeedz") и align-разворот бессмыслен (карусель осады крутит оба борта). Детектим -> не читаем атрибуты форта.
	bool bTgtIsFort = false;
	if (bIsFortAtIsland) { if (iFortCommander >= 0) { if (tgt == iFortCommander) bTgtIsFort = true; } }

	// 2) ПЕРЕЗАРЯДКА. В бою (близко) + выигран ветер/скорость над целью -> ImmediateReload (буст 60с).
	if (fDist < 800.0)
	{
		if (bInCombat) // реально идёт бой, а не «стрелял когда-то давно»
		{
			bool bSpeedAdv = bTgtIsFort; // форт неподвижен (скорость 0) -> преимущество всегда наше, скорость не читаем
			if (!bTgtIsFort) { if (stf(rCharacter.Ship.MaxSpeedZ) > stf(characters[tgt].Ship.MaxSpeedZ)) bSpeedAdv = true; }
			if (bSpeedAdv)
			{
				if (Ship_AI_TryPerk(rCharacter, "ImmediateReload")) { Ship_AI_PerkPopup(rCharacter, "perk_ImmediateReload"); return; }
			}
		}
	}

	// 3) РАЗВОРОТ. Только В БОЮ + дистанция <800 + цель ПОДВИЖНА (не форт). Иначе Turn180 срабатывал на входе в энкаунтер (fAlign<0.25) без выстрела либо на стоянке в порту.
	if (fDist < 800.0)
	{
		if (bInCombat && !bTgtIsFort)
		{
			float fHead = stf(rCharacter.Ship.Ang.y);
			float hx = sin(fHead);
			float hz = cos(fHead);
			float dx = stf(characters[tgt].Ship.Pos.x) - stf(rCharacter.Ship.Pos.x);
			float dz = stf(characters[tgt].Ship.Pos.z) - stf(rCharacter.Ship.Pos.z);
			float fAlign = 1.0;
			if (fDist > 1.0) fAlign = (hx * dx + hz * dz) / fDist; // 1=цель прямо по курсу, <0=сзади
			if (fAlign < 0.25) // цель >~75 град от курса -> крупный разворот
			{
				if (Ship_AI_TryPerk(rCharacter, "Turn180")) { Ship_AI_PerkPopup(rCharacter, "perk_Turn180"); return; }
			}
		}
	}
}

void Ship_UpdateParameters()
{
	int iCharacterIndex = GetEventData();
	// LEO: движок шлёт SHIP_UPDATE_PARAMETERS КАЖДЫЙ кадр по всем кораблям, вкл. мёртвых/невалидных (-1).
	//      Гвард стоял ПОСЛЕ GetCharacter(-1) → спам "Incorrect Index" сотнями/сек. Отсекаем -1 ДО GetCharacter — главный источник спама.
	if (iCharacterIndex < 0) return;
	ref rCharacter = GetCharacter(iCharacterIndex);
	float fSailState = GetEventData();

	bool bDead = LAi_IsDead(rCharacter);

	if(sti(rCharacter.ship.type) == SHIP_NOTUSED || bDead)   // super fix boal
	{
		return;
	}
	aref arTmpPerks;
	makearef(arTmpPerks, rCharacter.TmpPerks);

	// обновляем временное хранилище навыков
	Ship_UpdateTmpSkills(rCharacter);

	Cannon_RecalculateParameters(sti(rCharacter.index));

	// часть параметров корабля
	aref arCharShip; makearef(arCharShip, rCharacter.Ship);
	ref rShip = GetRealShip(sti(arCharShip.Type));

	float fTRFromSailDamage = Bring2Range(0.1, 1.0, 0.1, 100.0, stf(rCharacter.ship.sp)); //0.3
	float fShipSpeed    = FindShipSpeed(rCharacter, rShip) / fTRFromSailDamage;//убирает движковое дублирование расчета скорости от урона по парусам
	float fShipTurnRate = FindShipTurnRate(rCharacter, rShip);

	// кэш
	rCharacter.Tmp.fShipSpeed    = fShipSpeed;
	rCharacter.Tmp.fShipTurnRate = fShipTurnRate;

	// сила ветра
	float	fWindPower = Whr_GetWindSpeed() / WIND_NORMAL_POWER;
	float	fWindDotShip = GetDotProduct(Whr_GetWindAngle(), stf(arCharShip.Ang.y));		// Wind.ay | Ship.ay
	fShipSpeed = fShipSpeed * fWindPower;  // boal крейсерская скорость до уменьшений при данном ветре.
	arCharShip.MaxSpeedZ = fShipSpeed;
	arCharShip.CruiseSpeedZ = fShipSpeed; // LEO: КРЕЙСЕРСКАЯ (курс-НЕзависимая, все бафы: команда/груз/урон/навык/апгрейды/сила ветра) -> честное «догоню ли» на курсе беглеца (Ship_CanCatch). MaxSpeedZ ниже перезапишется курс-скорректированной (только текущий курс).
	rCharacter.Ship.MaxSpeedZ = Sea_ApplyMaxSpeedZ(rCharacter, fWindDotShip); // учет парусов в др месте

	float fMaxSpeedY = stf(rShip.TurnRate) / Sea_TurnRateMagicNumber(); //boal

	float fTRFromSailState = 1.0;
	switch (MakeInt(fSailState * 2.0))
	{
		case 0: fTRFromSailState = 0.75; break;
		case 1: fTRFromSailState = 1.0; break;
		case 2: fTRFromSailState = 0.9; break;
	}
	// boal зависимость от скорости на маневр -->
    float	fTRFromSpeed = 1.0;
    float fCurrentSpeedZ = stf(rCharacter.Ship.Speed.z);
	if (iCharacterIndex == GetMainCharacterIndex())
	{
        if (MOD_SKILL_ENEMY_RATE > 2) // халява и юнга - послабление)
        {
            if (iArcadeSails == 1)
        	{
               fTRFromSpeed = 1.0 - 0.2 * (1.0 - Clampf(fCurrentSpeedZ / fShipSpeed));
    		}
    		else
    		{
    		    fTRFromSpeed = 1.0 - 0.75 * (1.0 - Clampf(fCurrentSpeedZ / fShipSpeed));
    		}
        }
	}
	else
	{
		if (iArcadeSails == 1)
    	{
           fTRFromSpeed = 1.0 - (0.2 - MOD_SKILL_ENEMY_RATE*3*0.002) * (1.0 - Clampf(fCurrentSpeedZ / fShipSpeed));
		}
		else
		{
		    fTRFromSpeed = 1.0 - (0.75 - MOD_SKILL_ENEMY_RATE*3*0.005) * (1.0 - Clampf(fCurrentSpeedZ / fShipSpeed));//чем медленнее текущая скорость корабля, тем медленнее он поворачивает
		}
	}
	// boal зависимость от скорости на маневр <--
	float fTRResult = Clampf(fMaxSpeedY * fShipTurnRate * fTRFromSailState * fTRFromSpeed);

	arCharShip.MaxSpeedY =	fTRResult;

    // применяем аркадный режим
	if (iArcadeSails == 1)
	{
		rCharacter.Ship.MaxSpeedZ = (3.0 * stf(rCharacter.Ship.MaxSpeedZ));
		arCharShip.MaxSpeedY = (2.0 * stf(arCharShip.MaxSpeedY));
	}
	else
	{
		rCharacter.Ship.MaxSpeedZ = (1.2 * stf(rCharacter.Ship.MaxSpeedZ));
	}

	// расчёт погружения
	float fLoad = Clampf(GetCargoLoad(rCharacter) / stf(rShip.Capacity));
	arCharShip.Immersion = (stf(rShip.SubSeaDependWeight) * fLoad); // это уровень погружения от веса

	// do damage if ship hull < 20 процентов, sinking
	float fBaseSailHP = stf(rShip.SP);
	float fBaseShipHP = stf(rShip.HP);
	float fCurHP = stf(arCharShip.HP);
	if(fBaseShipHP < fCurHP)
	{
		fBaseShipHP = fCurHP;
	}
	if (bSeaActive && (fCurHP / fBaseShipHP < 0.2))
	{
		if (!CheckForUpgrade(rShip,"SUP_PUMPS")) //Помпы
		{
			float fLightRepair = 1.0;
			if (CheckOfficersPerk(rCharacter, "LightRepair")) fLightRepair = fLightRepair/2.0; 	// заменяю на проверку наличия перка, а не активен ли он сейчас
			if (sti(rCharacter.ship.soiling) > 30) fLightRepair = fLightRepair/2.0; //если обрастание больше 30%, скорость затопления ещё в два раза меньше

			// это спец атрибут по убиванию этой функции
			if (CheckAttribute(rCharacter, "SinkTenPercent") && !sti(rCharacter.SinkTenPercent)) { fLightRepair = 0.0; }
			float fSinkDamage = fLightRepair * (fBaseShipHP / 600.0); // теряем шестисотую долю от максХП в секунду??
			if (CheckAttribute(rCharacter, "Ship.LastBallCharacter"))
			{
				Ship_ApplyHullHitpoints(rCharacter, fSinkDamage, KILL_BY_BALL, sti(rCharacter.Ship.LastBallCharacter));
			}
			else
			{
				Ship_ApplyHullHitpoints(rCharacter, fSinkDamage, KILL_BY_TOUCH, -1);
			}
			if (iCharacterIndex == nMainCharacterIndex)
			{
				if(!IsPerkIntoList("sink"))
	        	{
					if (rand(1) == 0)
					{
						PlayVoice("Interface\"+LanguageGetLanguage()+"_Abandon1.wav");
					}
					else
					{
						PlayVoice("interface\"+LanguageGetLanguage()+"_Abandon"+(1+rand(3))+".wav");
					}
				}
				AddPerkToActiveList("sink");
			}
		}
	}
	else
	{
		if (iCharacterIndex == nMainCharacterIndex) { DelPerkFromActiveList("sink"); }
	}

	float fStormProfessional = AIShip_isPerksUse(arTmpPerks.StormProfessional, 1.0, 0.7);

    if (CheckAttribute(rCharacter, "DontHitInStorm"))
    {
		fStormProfessional = 0;
	}
	// урон при шторме/торнадо
	if (bStorm && bSeaActive)
	{
		// LEO: урон шторма ТОЛЬКО против ветра. fAgainstWind = -fWindDotShip, кламп 0..1: 0 по/поперёк ветра (урона нет), 1 строго против.
		float fAgainstWind = 0.0 - fWindDotShip;
		if (fAgainstWind < 0.0) fAgainstWind = 0.0;
		if (fAgainstWind > 1.0) fAgainstWind = 1.0;

		float fDamageMultiply = fStormProfessional * fAgainstWind * isEquippedArtefactUse(rCharacter, "talisman2", 1.0, 0.7) * STORM_DMG_SCALE;
		if (S_armor(rCharacter)) fDamageMultiply /= 2.0;
		// урон корпусу
		float fDamageHP = (fBaseShipHP / 100.0) * 1.1;
		float fHullDmg = fDamageMultiply * fDamageHP; // фактический урон корпусу в этот тик (0, если идём по/поперёк ветра)
		Ship_ApplyHullHitpoints(rCharacter, fHullDmg, KILL_BY_TOUCH, -1);

		// урон парусам
		switch (MakeInt(fSailState * 2.0))
		{
			case 0: MakeSailDmg(iCharacterIndex, fDamageMultiply * (fBaseSailHP / 100.0) * 0.15); break;
			case 1: MakeSailDmg(iCharacterIndex, fDamageMultiply * (fBaseSailHP / 100.0) * 0.55); break;
			case 2: MakeSailDmg(iCharacterIndex, fDamageMultiply * (fBaseSailHP / 100.0) * 1.1); break;
		}

		// experience - LEO: опыт за шторм ТОЛЬКО при реальном уроне корпусу (по ветру урона нет → опыта нет)
  		if (!bDead && fHullDmg > 0.0)
        {
			float fExp = fHullDmg / (8 - GetCharacterShipClass(rCharacter)); // пропорционально нанесённому урону (для мановара не слишком жирно)
			fExp = fExp / 10;
            Ship_AddCharacterExp(rCharacter, MakeInt(fExp));
            if (IsCompanion(rCharacter))
            {
               AddCharacterExpToSkill(rCharacter, "Sailing", fExp);
               ChangeCrewExp(rCharacter, "Sailors", AIShip_isPerksUse(CheckCharacterPerk(rCharacter, "SeaWolf"), 0.5, 1.0));
            }
        }

		// импульс вращения
  		float fRotate = stf(rCharacter.Ship.Tornado.Rotate.y) + (frnd() * 0.08 - 0.04);  //fix boal
		if (fRotate > 0.01)
		{
			fRotate = 0.01;
		}
		if (fRotate < -0.01)
		{
			fRotate = -0.01;
		}

		rCharacter.Ship.Tornado.Rotate.y = fRotate;  //fix boal
		arCharShip.Impulse.Rotate.y = fRotate;
	}

	if (bTornado && bSeaActive)
	{
		float fTornadoDistance = GetDistance2D(stf(Tornado.x), stf(Tornado.z), stf(arCharShip.Pos.x), stf(arCharShip.Pos.z));
		float fTornadoDamageMultiply = fStormProfessional * Bring2Range(1.0, 0.0, 0.0, 100.0, fTornadoDistance);

		// урон корпусу
		Ship_ApplyHullHitpoints(rCharacter, fTornadoDamageMultiply * (fBaseShipHP / 100.0) * 8.5, KILL_BY_TOUCH, -1);

		// урон парусам
		switch (MakeInt(fSailState * 2.0))
		{
			case 0:
				MakeSailDmg(iCharacterIndex, fTornadoDamageMultiply * (fBaseSailHP / 100.0) * 1.5);
			break;
			case 1:
				MakeSailDmg(iCharacterIndex, fTornadoDamageMultiply * (fBaseSailHP / 100.0) * 4.5);
			break;
			case 2:
				MakeSailDmg(iCharacterIndex, fTornadoDamageMultiply * (fBaseSailHP / 100.0) * 8.5);
			break;
		}
	}

	// секция лога опыта
	if (iCharacterIndex == nMainCharacterIndex && bSeaActive)
	{
		fSeaExpTimer = fSeaExpTimer + 1.0;
		if (fSeaExpTimer >= 15.0 && bStorm)
		{
			if (!bDead)
			{
			}

			fSeaExpTimer = 0.0;
			fSeaExp = 0.0;
		}
	}

	// пересчёт загрузки трюма при необходимости
	if (CheckAttribute(rCharacter, "Ship.Cargo.RecalculateCargoLoad") && sti(rCharacter.Ship.Cargo.RecalculateCargoLoad))
	{
		if (rand(5) == 1)  // оптимизация, сильные тормоза
		{
			RecalculateCargoLoad(rCharacter);
			rCharacter.Ship.Cargo.RecalculateCargoLoad = 0;
		}
	}
    //  to_do не было в ПКМ -->
	if (bSeaLoaded && CheckAttribute(rCharacter, "SeaAI.Task.Target")) Ship_UP_TargetTactics(rCharacter, rShip);
	Ship_AI_AutoUsePerks(rCharacter); // LEO: авто-использование боевых перков ИИ в море
	// запуск случайных звуков :: ПАРУСА
	if (rand(40) <= fSailState * 5.0) { Ship_PlaySound3DComplex(rCharacter, "sails_ambient", fSailState * 0.5, 0.0, 0.0, 0.5 + frnd() * 0.0); }

	// запуск случайных звуков :: КОРАБЛИ
	if (rand(2) == 1) { Ship_PlaySound3DComplex(rCharacter, "squeak_sea"+sSeaSoundPostfix, 0.9, frnd() * 1.2 - 0.6, 0.0, frnd() * 1.6 - 0.8); }
}

void Ship_SailsMoveSound()
{
	aref arCharacter = GetEventData();
	bool bMove = GetEventData();

	// разные фиксы
    if(!CheckAttribute(arCharacter, "Ship.Sounds.SailsID"))
        return;
	// обновление звуков :: ПАРУСА — движение
	if (CheckAttribute(arCharacter, "Ship.Sounds") && sti(arCharacter.Ship.Sounds.SailsID) > 0)
		{ Sound_SetVolume(sti(arCharacter.Ship.Sounds.SailsID), 0.75 * bMove); }
}

void Ship_PrintExp(int iExp)
{
	if (iExp <= 0) { return; }

	object oRes;
	string sExpQuantity = iExp;
	string sExpString = LanguageConvertString(iSeaSectionLang, "sea_exp_add");

	Event(PARSE_STRING, "asls", &oRes, sExpString, 1, sExpQuantity);

	Log_SetStringToLog(oRes.Str);
}

void Ship_TornadoDamage()
{
	float fTime = GetEventData();

	Tornado.x = GetEventData();
	Tornado.z = GetEventData();
}

void Ship_UpdateTmpSkills(ref rCharacter)
{
	///// с оптимизацией
	if (!CheckAttribute(rCharacter, "TmpSkillRecall") || sti(rCharacter.TmpSkillRecall) <= 0)
    {
        rCharacter.TmpSkill = "";

    	aref aTmpSkill; makearef(aTmpSkill, rCharacter.TmpSkill);

    	// расчёт навыков для оптимизации скорости
    	aTmpSkill.Leadership = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_LEADERSHIP)) / SKILL_MAX;
    	aTmpSkill.Sneak     = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_SNEAK)) / SKILL_MAX;
    	aTmpSkill.Defence   = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_DEFENCE)) / SKILL_MAX;
    	aTmpSkill.Grappling = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_GRAPPLING)) / SKILL_MAX;
    	aTmpSkill.Sailing   = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_SAILING)) / SKILL_MAX;
    	aTmpSkill.Repair    = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_REPAIR)) / SKILL_MAX;
    	aTmpSkill.Fencing   = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_FENCING)) / SKILL_MAX;
    	aTmpSkill.Accuracy  = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_ACCURACY)) / SKILL_MAX;
    	aTmpSkill.Cannons   = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_CANNONS)) / SKILL_MAX;

        if (sti(rCharacter.index) == GetMainCharacterIndex())
        {
            if (SeaCameras.Camera == "SeaDeckCamera")
        	{
        		aTmpSkill.Accuracy = Clampf(stf(aTmpSkill.Accuracy) + 0.15); // было 0.4
        	}
        	if (rand(10) == 3)//boal fix отношений, редко обновляется дял профилактики
        	{
        	    DoQuestCheckDelay("NationUpdate", 1.0);
        	}
    	}
    	rCharacter.TmpSkillRecall = 25 + rand(49); // разное время - размазанный пересчет, а то все скопом всё равно //"60"; // считаем раз в минуту
    	// анализ погони
		if (GetNpcQuestPastTimeParam(rCharacter, "Add2SeaTime") > 55) // 2 дня Только у фантомов есть признак, остальные 0
		{
		    if (CheckAttribute(rCharacter, "SeaAI.Task") && rCharacter.SeaAI.Task != AITASK_RUNAWAY) // че б не было, всё равно может гнать за ГГ
	        {
	        	Ship_SetTaskRunaway(SECONDARY_TASK, sti(rCharacter.index), sti(pchar.index));
	        }
		}
	}
	else
	{
        rCharacter.TmpSkillRecall = sti(rCharacter.TmpSkillRecall) - 1;

        if (sti(rCharacter.index) == GetMainCharacterIndex())
    	{
            rCharacter.TmpSkill.Accuracy  = MakeFloat(GetSummonSkillFromName(rCharacter, SKILL_ACCURACY)) / SKILL_MAX;
            if (SeaCameras.Camera == "SeaDeckCamera")
            {
                rCharacter.TmpSkill.Accuracy = Clampf(stf(rCharacter.TmpSkill.Accuracy) + 0.15);
            }
    	}
	}
}

void Ship_UpdatePerks()
{
	int iCharacterIndex = GetEventData();
	ref rCharacter = GetCharacter(iCharacterIndex);

	if (sti(rCharacter.ship.type) == SHIP_NOTUSED) return; // fix для смены корабля
	rCharacter.TmpPerks = "";
	aref aTmpPerks; makearef(aTmpPerks, rCharacter.TmpPerks);

	aTmpPerks.StormProfessional	  	    = CheckOfficersPerk(rCharacter, "StormProfessional");
    aTmpPerks.FastReload				= CheckOfficersPerk(rCharacter, "FastReload");
	aTmpPerks.ImmediateReload			= CheckOfficersPerk(rCharacter, "ImmediateReload");
	aTmpPerks.HullDamageUp			 	= CheckOfficersPerk(rCharacter, "HullDamageUp");
	aTmpPerks.CrewDamageUp				= CheckOfficersPerk(rCharacter, "CrewDamageUp");
	aTmpPerks.CriticalShoot				= CheckOfficersPerk(rCharacter, "CriticalShoot");
	aTmpPerks.LongRangeShoot			= CheckOfficersPerk(rCharacter, "LongRangeShoot");
	aTmpPerks.CannonProfessional		= CheckOfficersPerk(rCharacter, "CannonProfessional");
	aTmpPerks.LongRangeGrappling		= CheckOfficersPerk(rCharacter, "LongRangeGrappling");
	aTmpPerks.GrapplingProfessional		= CheckOfficersPerk(rCharacter, "GrapplingProfessional");
	aTmpPerks.BasicBattleState			= CheckOfficersPerk(rCharacter, "BasicBattleState");
	aTmpPerks.AdvancedBattleState		= CheckOfficersPerk(rCharacter, "AdvancedBattleState");
	aTmpPerks.ShipDefenseProfessional	= CheckOfficersPerk(rCharacter, "ShipDefenseProfessional");
	aTmpPerks.ShipSpeedUp				= CheckOfficersPerk(rCharacter, "ShipSpeedUp");
	aTmpPerks.ShipTurnRateUp			= CheckOfficersPerk(rCharacter, "ShipTurnRateUp");
	aTmpPerks.SandbankManeuver			= CheckOfficersPerk(rCharacter, "SandbankManeuver");
	aTmpPerks.SailingProfessional		= CheckOfficersPerk(rCharacter, "SailingProfessional");
	aTmpPerks.LightRepair				= CheckOfficersPerk(rCharacter, "LightRepair");
	aTmpPerks.Doctor1					= CheckOfficersPerk(rCharacter, "Doctor1");
	aTmpPerks.Doctor2					= CheckOfficersPerk(rCharacter, "Doctor2");
	
	// update main character if rCharacter is companion   - это было, но зачем?? (boal)
	if (iCharacterIndex != nMainCharacterIndex && IsCompanion(rCharacter))
	{
		Event("eSwitchPerks", "l", nMainCharacterIndex);
	}
}

void Ship_Lightning()
{
	if (!bSeaActive) { return; }

	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	for (int i=0; i<iNumShips; i++)
	{
	    if(Ships[i] < 0) continue;
		ref rCharacter = GetCharacter(Ships[i]);
		if (S_armor(rCharacter)) continue; // СФ
		aref arShipChar; makearef(arShipChar, rCharacter.Ship);

		float sx = stf(arShipChar.Pos.x);
		float sy = stf(arShipChar.Pos.y);
		float sz = stf(arShipChar.Pos.z);
		float bx = stf(arShipChar.BoxSize.x);
		float by = stf(arShipChar.BoxSize.y);
		float bz = stf(arShipChar.BoxSize.z);
		float fDist = GetDistance2D(x, z, sx, sz);
		float fMinLightningDist = 20.0 + bz / 1.5;
		if (fDist < fMinLightningDist)
		{
			int iMinBoom = MakeInt(Bring2Range(5.0, 2.0, 1.0, 7.0, stf(GetCharacterShipClass(rCharacter))));
			float fCos = cos(stf(arShipChar.Ang.y));
			float fSin = sin(stf(arShipChar.Ang.y));
			int iMax = iMinBoom + rand(2);
			for (int k=0; k<iMax; k++)
			{
				float dx = 0.6 * bx * (frnd() - 0.5);
				float dz = 0.7 * bz * (frnd() - 0.5);
				RotateAroundY(&dx, &dz, fCos, fSin);
				Ship_Serious_Boom(sx + dx, sy + 0.1 * by * frnd(), sz + dz);
			}

			Ship_ApplyHullHitpoints(rCharacter, iMinBoom * 40.0, KILL_BY_TOUCH, -1);
			Ship_ApplyCrewHitpoints(rCharacter, MakeFloat(rand(3)));

			int iNumFirePlaces = 0;
			SendMessage(rCharacter, "le", MSG_SHIP_GET_NUM_FIRE_PLACES, &iNumFirePlaces);

			float fFirePlaces = Bring2Range(1.0, 0.1, 0.0, fMinLightningDist, fDist);

			float fCurr = 0.0;
			float fTest = (1.0 - fFirePlaces) * iNumFirePlaces;
			for (int j=0; j<iNumFirePlaces; j++)
			{
				fCurr = fCurr + 1.0;
				if (fCurr < fTest) { continue; }

				fCurr = 0.0;
				int iRandStartTime = rand(10000);
				float fTotalFireTime = Ship_GetTotalFireTime(rCharacter);

				PostEvent(SHIP_ACTIVATE_FIRE_PLACE, iRandStartTime, "ialsfl", rCharacter, rCharacter, j, "ship_onfire", fTotalFireTime, -1);
				PostEvent(SHIP_FIRE_DAMAGE, iRandStartTime, "lllf", Ships[i], -1, j, fTotalFireTime);
			}
		}
	}
}

void Ship_ClearImpulseStrength(ref rCharacter)
{
	aref arImpulse; makearef(arImpulse, rCharacter.Ship.Impulse);
	arImpulse.Rotate.x = 0.0;
	arImpulse.Rotate.y = 0.0;
	arImpulse.Rotate.z = 0.0;
	arImpulse.Speed.x = 0.0;
	arImpulse.Speed.y = 0.0;
	arImpulse.Speed.z = 0.0;

	rCharacter.Ship.Tornado.Rotate.y = 0.0;  // fix boal
}

void Ship_PrepareShipForLocation(ref rCharacter)
{
	Ship_ClearImpulseStrength(rCharacter);
}
//  далее новые методы, не было в ПКМ
void Ship_LoadShip()
{
	int iCharacterIndex = GetEventData();

	ref rCharacter = GetCharacter(iCharacterIndex);

	if (sti(rCharacter.ship.type) != SHIP_NOTUSED)
	{
		Event("eSwitchPerks", "l", iCharacterIndex);
		ShipModelrList[iNumShips] = GetCurrentModelrNumber();

		Ships[iNumShips] = iCharacterIndex;
		LoadedChars[iNumShips] = iCharacterIndex; // после загрузки сейва в море
		rCharacter.curshipnum = iNumShips;
		Dev_Trace("Ship_LoadShip = " + iCharacterIndex + " id = " + rCharacter.id + " iNumShips = " + iNumShips + " curshipnum = " + rCharacter.curshipnum + " ShipModelrList[iNumShips] = " + ShipModelrList[iNumShips]);
		Event(SHIP_UPDATE_PARAMETERS, "lf", iCharacterIndex, stf(rCharacter.Ship.Speed.z));		// Parameters      после создания корабля
		iNumShips++;
		
		rCharacter.Ship.MaxCannonDist = 0.0;
	}
}

void Ship_LoadShipActivateFirePlace()
{
	int iOurCharacterIndex = GetEventData();
	int iBallCharacterIndex = GetEventData();
	int iFirePlaceIndex = GetEventData();
	float fTotalFireTime = GetEventData();

	Dev_Trace("iBallCharacterIndex = " + iBallCharacterIndex);
	int iStartTime = 500;

	PostEvent(SHIP_FIRE_DAMAGE, iStartTime, "lllf", iOurCharacterIndex, iBallCharacterIndex, iFirePlaceIndex, fTotalFireTime);
}

void SetTextureForShip(aref refBaseShip, aref rCharacter)
{
	if(!CheckAttribute(refBaseShip, "name"))
	{
		trace("SetTextureForShip(): ship " + refBaseShip.basetype + " have no name! NPCid = "+ rCharacter.id);
		if (MOD_BETTATESTMODE == "On") Log_Info("SetTextureForShip(): ship " + refBaseShip.basetype + " have no name! NPCid = "+ rCharacter.id);
		return;
	}

	string sPath = "ships\" + refBaseShip.name + "\";
	string sPath2 = "";

	if(CheckAttribute(refBaseShip, "ship.Upgrades.hull"))  // boal 16/07/06 просто разные корпуса, без НПС
	{
		sPath2 = sPath + "hull" + refBaseShip.ship.upgrades.hull + "\";
		SetTexturePath(2, sPath2);
	}

	SetTexturePath(1, sPath);
	SetTexturePath(0, "");
}

void DropGoodsToSea()
{
	aref rCharacter = GetEventData();

	ref rGood;
	string sGood;

	for (int i = GOOD_MEDICAMENT; i < GOODS_QUANTITY; i++)
	{
		// Не выбрасывать дорогие товары.
		if (Goods[i].Cost / Goods[i].Weight > 20) continue;

		sGood = Goods[i].name;

		if(CheckAttribute(rCharacter, "Ship.Cargo.Goods."+sGood))
		{
			int	iQuantity = sti(rCharacter.Ship.Cargo.Goods.(sGood));

			if(CheckAttribute(rCharacter, "Ship.Cargo.Goods."+sGood+".Lock")) continue; // залоченные товары не выкидывать !!!!! <-- ugeen fix

			if (iQuantity < 1 && rand(1) == 1) continue;
			iQuantity = makeint(iQuantity/2 + rand(makeint(iQuantity/3)));   // выкидываем не все и не всегда, а то под ноль

			if (iQuantity < 1) continue;

			RemoveCharacterGoods(rCharacter, i, iQuantity);

			iQuantity = iQuantity / sti(Goods[i].Units);

			if (iQuantity <= 1)
			{
				continue;
			}

			rCharacter.Ship.Cargo.Goods.(sGood) = 0;
            rCharacter.Ship.Cargo.RecalculateCargoLoad = true; // boal fix оптимизация

			if (CheckAttribute(&Goods[i], "Swim"))
			{
				int iSwimQuantity = makeint(iQuantity/3 + rand(makeint(iQuantity/3)) + rand(makeint(iQuantity/3)));

				if(iSwimQuantity < 1)
				{
					iSwimQuantity = 1;
				}
				float fTime = stf(Goods[i].Swim.Time);
				string sModel = Goods[i].Swim.Model;


				fTime = fTime + (frnd() - 0.5) * fTime / 2.0;

				AISeaGoods_AddGood(rCharacter, sGood, sModel, fTime * 60.0, iSwimQuantity);
			}
			return;
		}
	}
}

int Ship_GetRunAwayPoint()
{
	aref arChar = GetEventData();
	float fX = GetEventData();
	float fZ = GetEventData();

	float fRadius = 10000.0;
	float fWindAngle = Whr_GetWindAngle();

	arChar.SeaAI.WindAngle = fWindAngle;
	arChar.SeaAI.RunAwayPnt.x = stf(arChar.Ship.Pos.x) + fRadius * sin(fWindAngle);
	arChar.SeaAI.RunAwayPnt.z = stf(arChar.Ship.Pos.z) + fRadius * cos(fWindAngle);

	return 0;
}

string GetShipSexWord(string _type, string _man, string _woman)
{
    if (_type == "Tartane" || _type == "Schooner" || _type == "Caravel" ||
        _type == "Xebec" || _type == "XebecLight" || _type == "XebecBattle" )
    {
        return _woman;
    }
    else
    {
        return _man;
    }
}
// boal 08.08.06 метод пересчета орудий на корабле  - доработал ядро
void Ship_ResearchCannons(ref aCharacter)
{
	if (bSeaLoaded)
	{
		aCharacter.BOAL_ReadyCharge = "1";
		SendMessage(&AISea, "la", AI_MESSAGE_RESEARCH_CANNONS, aCharacter);
		RefreshBattleInterface();
		Ship_ChangeCharge(aCharacter, sti(aCharacter.Ship.Cannons.Charge.Type));
	}
}
// boal 20.08.06 перерисовка флага -->
void Ship_FlagRefresh(ref rCharacter)
{
    if (bSeaLoaded)
	{
		SendMessage(rCharacter, "l", MSG_SHIP_FLAG_REFRESH);
	}
}

void EmptyFantom_DropGoodsToSea(ref rChar, int iFantomType)
{
	float fTime;
	string sGood, sModel;
	int iQty;
	if(iFantomType == ENCOUNTER_TYPE_BARREL)
	{
		int irand = drand(100);
		if(irand > 92) // делаем бум
		{
			AISeaGoods_AddGood_Special(rChar, "powder1"+ "|" + pindex, "barrel_treasure", 1500.0, 1); // нарвались на беспризорную мину
			pindex++;
		}
		else
		{
			if(irand < 40)
			{
				for (int i=GOOD_MEDICAMENT; i<GOOD_GOLD; i++)
				{
					sGood = Goods[i].name;
					if (rand(3) > 1) continue;
					if (CheckAttribute(&Goods[i], "Swim"))
					{
						iQty = rand(15) + 5;
						fTime = stf(Goods[i].Swim.Time);
						sModel = Goods[i].Swim.Model;
						fTime = fTime + (frnd() - 0.5) * fTime / 2.0;

						AISeaGoods_AddGood_Special(rChar, sGood, sModel, fTime * 60.0, iQty);
					}
				}
			}
			AISeaGoods_AddGood_Special(rChar, "barrel_treasure", "barrel_treasure", 1500.0, 1);
		}
	}
	if(iFantomType == ENCOUNTER_TYPE_BOAT)
	{
		AISeaGoods_AddGood_Special(rChar, "unknown_boat", "lo_boat", 1500.0, 1);
	}
	SendMessage(&AISea, "la", AI_MESSAGE_CHARACTER_DEAD, rChar);
}

// ============================================================================
//   БОЕВАЯ МОРАЛЬ, СДАЧА В ПЛЕН, НЕЙТРАЛИТЕТ, СООТНОШЕНИЕ СИЛ
// ============================================================================
#define SURR_CAPTAIN_TOUGHNESS 0.6 // LEO: множитель СТОЙКОСТИ капитана на порог сдачи/откупа (<1 = реже сдаются/откупаются, «покрепче»). Тунабл.
bool Ship_CheckMorale(int chridx, bool checkNow)
{
	// РЕАНИМИРОВАНО (был return true; вся боевая мораль мертва, Ship_Check_Surrender всегда отказывал).
	// Кормится Speak_EnemyInitiative; крошки lasthit/lastcrew/lastHP/lastfired/enemydead пишут Hull/BortReloaded/OnDead; время — Ship_VicSoundTime.
	ref rCharacter = GetCharacter(chridx);
	if(CheckAttribute(rCharacter,"surrendered")) return false; // NK surrender 05-04-20

	if (!CheckAttribute(rCharacter,"ship.type")) {
		trace("missing ship type for character: " + rCharacter.id + ", index: " + chridx);
		return false; //
	}
	if(rCharacter.ship.type == SHIP_FORT) return false;

	if (!checkNow) {
		// проверяем изредка
		int timecheck = CHANGE_MORALE_EVERY
		if(chridx == GetMainCharacterIndex()) {
			timecheck *= 50 //seem to get ~50 mainchar ticks per NPC tick
		}
		int timesincelast = 0;
		if(CheckAttribute(rCharacter,"seatime.timesincelast"))
		{	timesincelast = sti(rCharacter.seatime.timesincelast); }
		if(timesincelast < timecheck)
		{ //increment one and skip; this is to do this once every CHANGE_MORALE_EVERY seconds
			rCharacter.seatime.timesincelast = timesincelast+1;
			return false;
		}
		rCharacter.seatime.timesincelast = 0; //reset counter
	}

	//#20191022-02
	rCharacter.seatime.surmargin = SURR_MARGIN;
	if(!CheckAttribute(rCharacter,"Ship.Crew.Morale")) { rCharacter.Ship.Crew.Morale = 45; }
	if(!CheckAttribute(rCharacter,"seatime.basecrewmorale")) { rCharacter.seatime.basecrewmorale = rCharacter.Ship.Crew.Morale; }

	int CrewQuantity = GetCrewQuantity(rCharacter);
	if(!CheckAttribute(rCharacter,"seatime.basecrewquantity")) { rCharacter.seatime.basecrewquantity = CrewQuantity; }

	int basemorale = sti(rCharacter.seatime.basecrewmorale);

	float fMaxTempMorale = (MORALE_MAX + basemorale)/2.0;  //how much can morale increase if things are going well

	float tempmorale = stf(rCharacter.ship.crew.morale); if(CheckAttribute(rCharacter,"seatime.tempmorale")) tempmorale = stf(rCharacter.seatime.tempmorale); //i.e. a float morale
	rCharacter.seatime.tempmorale = tempmorale;

	float enemydistance = DIST_NOSHIP;
	//#20190703-01
	aref rSituation;
    makearef(rSituation, rCharacter.SeaAI.Update.Situation);

	// движок пишет в Situation ТОЛЬКО MinEnemyDistance (ai_ship.cpp:211) — индексов/дистанции друга НЕТ (порт из GOF). Читаем с гвардами
	int targetidx = -1;
	if (CheckAttribute(rSituation, "MinEnemyIndex")) targetidx = sti(rSituation.MinEnemyIndex);
	if (CheckAttribute(rSituation, "MinEnemyDistance")) enemydistance = stf(rSituation.MinEnemyDistance); // finds nearest hostile ship
	float fdist = DIST_NOSHIP;
	int frndidx = -1;
	if (CheckAttribute(rSituation, "MinFriendIndex")) frndidx = sti(rSituation.MinFriendIndex);
	if (CheckAttribute(rSituation, "MinFriendDistance")) fdist = stf(rSituation.MinFriendDistance);

	int currenttime = GetSeaTime();
	int lasthit = -1; if(CheckAttribute(rCharacter,"seatime.lasthit")) lasthit = sti(rCharacter.seatime.lasthit); //last time we were hit
	int lastfired = -1; if(CheckAttribute(rCharacter,"seatime.lastfired")) lastfired = sti(rCharacter.seatime.lastfired); // last time we fired or reloaded


	// корректировка морали за погибшую команду
	if(CheckAttribute(rCharacter,"seatime.lastcrew"))
	{
		if (sti(rCharacter.seatime.basecrewquantity)  != 0) //Skip if basecrew zero
			tempmorale = tempmorale - (stf(rCharacter.seatime.lastcrew) - makefloat(CrewQuantity)) / stf(rCharacter.seatime.basecrewquantity) / CREW_PCT_FOR_MORALE;
		DeleteAttribute(rCharacter,"seatime.lastcrew");
	}
	// корректировка морали за повреждённый корабль
	if(CheckAttribute(rCharacter,"seatime.lastHP"))
	{
		if (makefloat(GetCharacterShipHP(rCharacter)) != 0.0) //Skip if ShipHP is zero...would be dead anyway
			tempmorale = tempmorale - (stf(rCharacter.seatime.lastHP) - makefloat(rCharacter.Ship.HP)) / makefloat(GetCharacterShipHP(rCharacter)) / CREW_PCT_FOR_HPDAM;
		DeleteAttribute(rCharacter,"seatime.lastHP");
	}
	// корректировка морали за участие в бою
	float combatlevel = COMBATLEVEL_NONE;
	if(enemydistance <= RANGE_TO_ENEMY) combatlevel = COMBATLEVEL_ENEMYHERE;
	if(lastfired != -1) { if((currenttime - lastfired) < TIME_SINCE_GUNS) combatlevel = COMBATLEVEL_GUNS; }
	if(lasthit != -1) { if((currenttime - lasthit) < TIME_SINCE_HIT) combatlevel = COMBATLEVEL_HIT; }

	if(combatlevel == COMBATLEVEL_NONE)
	{
		if(makeint(tempmorale) != basemorale)
		{
			if(tempmorale > basemorale) tempmorale -= MORALE_AMT_COMEBACK;
			else tempmorale += MORALE_AMT_COMEBACK;
		}
	}
	else
	{
		if(tempmorale > 5.0 / combatlevel)
		{
			tempmorale -= retmin(COMBATLEVEL_HIT*CHANGE_MORALE_EVERY, combatlevel*2.5);
		}
	}
	if(CheckAttribute(rCharacter,"seatime.enemydead"))
	{
		tempmorale += stf(rCharacter.seatime.enemydead);
		DeleteAttribute(rCharacter,"seatime.enemydead"); // 04-09-22 fix
	}
	float strengthrat = FindPowerRatio(chridx);

	if (strengthrat < 0.6 && fdist < enemydistance && combatlevel != COMBATLEVEL_NONE)
	{
		if(tempmorale < fMaxTempMorale) tempmorale += COMBATLEVEL_ENEMYHERE*2.5;
	}
	if(tempmorale > MORALE_MAX) tempmorale = MORALE_MAX;
	if(tempmorale < MORALE_MIN) tempmorale = MORALE_MIN;

	rCharacter.seatime.tempmorale = tempmorale;

	// порог морали, при котором думаем о сдаче
	float surmorale;
	rCharacter.seatime.surmorale = 0.0;  //default, no surrender chance

	// targetidx убран: MinEnemyIndex движок не пишет (всегда -1); наличие врага гейтится ниже по enemydistance > 1600
	if(IsCompanion(rCharacter) || lasthit == -1 || CheckAttribute(rCharacter, "cannotsurrender")) {
		return false;
	}
	float ftmult = 1.0;
	//#20191022-02
	float surrMarg = SURR_MARGIN;
	//#20200309-01
	// модификатор по типу встречи
	if(CheckAttribute(rCharacter,"EncType") && rCharacter.EncType == "trade") {
        ftmult *= 1.5; surrMarg += 2.0; //ftmult was 2.0 and surrMarg was 2.5 //#20200307-03
	}
	// модификатор по нации
	int nCharNat = sti(rCharacter.Nation);
	switch(nCharNat) {
        case PIRATE:
            int nOtherNation = sti(pchar.nation);
            int nLastBallChar = -1;
            if(CheckAttribute(rCharacter, "ship.LastBallCharacter"))
                nLastBallChar = sti(rCharacter.ship.LastBallCharacter);
            if(nLastBallChar > -1)
                nOtherNation = sti(Characters[nLastBallChar].nation);
            if(nOtherNation == PIRATE)
                ftmult *= 0.85; surrMarg *= 1.0; break;
            else
                ftmult *= 0.35; surrMarg *= 1.5; break;
        break;
        case SPAIN:
            ftmult *= 0.15;
        break;
        case HOLLAND:
            ftmult *= 0.85;
        break;
	}
	// модификатор по классу корабля
	int nShip = GetCharacterShipType(rCharacter);
	if (nShip != SHIP_NOTUSED) {
        int nClass = 7;
        if(CheckAttribute(RealShips[nShip], "class"))
            nClass = sti(RealShips[nShip].Class);
        switch(nClass) {
            case 5:
                ftmult *= 0.85; surrMarg *= 0.75; break;
            break;
            case 4:
                ftmult *= 0.65; surrMarg *= 0.55; break;
            break;
            case 3:
                ftmult *= 0.45; surrMarg *= 0.35; break;
            break;
            case 2:
                ftmult *= 0.25; surrMarg *= 0.15; break;
            break;
            case 1:
                ftmult = 0.0; surrMarg = 0.0; break;
            break;
        }
	}
	// модификатор по рангу НПС
	int nRank = sti(pchar.rank);
	if (nRank > 10) {
        float fRnk = Bring2Range(1.0, 0.5, 10.0, 50.0, stf(pchar.rank));
        ftmult *= fRnk;
	}
	rCharacter.seatime.surmargin = surrMarg; //This only affects likely to flee
	float fIronWill = 0.0; // перк есть не у всех - гвард от "missed attribute"
	if (CheckAttribute(rCharacter, "TmpPerks.IronWill")) fIronWill = AIShip_isPerksUse(rCharacter.TmpPerks.IronWill, 0.0, 0.4);
	float mrlmod = Bring2Range(SURR_MRL_SCL_MAX, SURR_MRL_SCL_MIN, 0.1, 1.4, stf(rCharacter.TmpSkill.Leadership) + fIronWill);
	ref pch = GetMainCharacter();

	surmorale = SURR_MAX_MORALE * mrlmod * ftmult;

	if (enemydistance > 1600.0) {
        //#20200307-03
        DeleteAttribute(rCharacter, "seatime.thinkSurr");
        return false;				// LDH don't surrender if enemy not on radar
	}
	float HPp = GetHullPercent(rCharacter);

    if (HPp > 40.0 && fdist <= enemydistance && strengthrat < 0.8) return false;

	if(HPp < 40.0)
	{
		surmorale *= Bring2Range(6.0, 1.0, 0.1, 6.325, sqrt(HPp)); //how broken are we? x6 change at really broken, x0.5 at 40 percents
		if (fdist < enemydistance - 200.0) {
			surmorale *= 0.5; //is a friend close to help?
		}
	}
	if(strengthrat > 1.0) // they're stronger
	{
		float SP = GetSailPercent(rCharacter);
		if(SP < 40.0) surmorale *= Bring2Range(5.0, 1.0, 0.1, 7.071, sqrt(SP)); //panic if can't get away
	}
	else
	{
		surmorale *= 0.3;
	}
	if (GetDistDeltaToNearestEnemy(chridx) > 1.0) {
		surmorale *= 0.1;
	}
	surmorale *= SURR_CAPTAIN_TOUGHNESS; // LEO: капитан покрепче — реже думает о сдаче/откупе
	rCharacter.seatime.surmorale = surmorale;

	if(tempmorale > surmorale)
	{
        if(CheckAttribute(rCharacter, "seatime.thinkSurr")) {
            int nSur = sti(rCharacter.seatime.thinkSurr);
            if(nSur <= 1)
                DeleteAttribute(rCharacter, "seatime.thinkSurr");
            else
                rCharacter.seatime.thinkSurr = nSur - 1;
        }
		return false;
	} //#20200307-03
	else {
        if(CheckAttribute(rCharacter, "seatime.thinkSurr"))
            rCharacter.seatime.thinkSurr = sti(rCharacter.seatime.thinkSurr) + 1;
        else
            rCharacter.seatime.thinkSurr = 1;
	}
	return true;  //chance to surrender
}

float GetDistDeltaToNearestEnemy(int idx)
{
	float fDistDelta = 0.0;
	ref chr = GetCharacter(idx);
	float fEnemyDistance = DIST_NOSHIP;
	//#20190703-01
	aref rSituation;
    makearef(rSituation, chr.SeaAI.Update.Situation);
	// движок MinEnemyIndex НЕ пишет (только дистанцию) — следим за дельтой дистанции до ближайшего врага
	int iClosestIdx = -1;
	if (CheckAttribute(rSituation, "MinEnemyIndex")) iClosestIdx = sti(rSituation.MinEnemyIndex);
	if (CheckAttribute(rSituation, "MinEnemyDistance")) fEnemyDistance = stf(rSituation.MinEnemyDistance);
	if (iClosestIdx == -1)
	{
		if (fEnemyDistance >= DIST_NOSHIP) { return 0; } // врагов нет вовсе
		iClosestIdx = 0; // идентичности цели движок не даёт - псевдо-id
	}
	float fNow = GetSeaTime();

	if (CheckAttribute(chr, "seatime.nearestenemy.id") && iClosestIdx == sti(chr.seatime.nearestenemy.id))
	{
		float fTimeDelta = fNow - stf(chr.seatime.nearestenemy.lastcheck)
		if (fTimeDelta > 1)
		{
			// расчёт дистанции
			fDistDelta = (fEnemyDistance - stf(chr.seatime.nearestenemy.dist))/fTimeDelta;
		}
		else
		{
			// прошло слишком мало времени
			return stf(chr.seatime.nearestenemy.LastDelta);
		}
	}
	chr.seatime.nearestenemy.id = iClosestIdx;
	chr.seatime.nearestenemy.dist = fEnemyDistance;
	chr.seatime.nearestenemy.lastcheck = GetSeaTime();
	chr.seatime.nearestenemy.LastDelta = fDistDelta;
	return fDistDelta;
}


bool Ship_CheckMoraleFail(int chridx, float fSurrMult)
{
	ref rCharacter = GetCharacter(chridx);

	if(!CheckAttribute(rCharacter,"seatime.tempmorale")) {
		// пересчёт морали
		Ship_CheckMorale(chridx, true);
	}
	float tempmorale = rCharacter.seatime.tempmorale;

	if (!CheckAttribute(rCharacter, "seatime.surmorale")) {
		return false;
	}
	float surmorale = stf(rCharacter.seatime.surmorale) * fSurrMult;

	if(tempmorale < surmorale)
	{
		return true;
	}
	return false;
}

bool CheckCanSurrender(ref chr)
{
	if(CheckAttribute(chr, "cannotsurrender"))
	{
		return false;
	}
	if(CheckAttribute(chr, "DontRansackCaptain"))
	{
		return false;
	}
	if (!CheckAttribute(chr,"Ship.Type") || sti(chr.Ship.Type) < 0 || sti(chr.Ship.Type) == SHIP_NOTUSED)
	{
		return false;
	}
	if (GetPrisonerQty() > PRISONER_MAX)
	{
		return false;
	}
	return true;
}

void AddKillMorale(ref rKillerCharacter, ref rDead) {
	ref schr;
	int iDeadCharacterIndex = GetCharacterIndex(rDead.id);
	float tmped = 0;
	if(rKillerCharacter.id != rDead.id) {
		if(CheckAttribute(rKillerCharacter,"seatime.enemydead")) tmped = stf(rKillerCharacter.seatime.enemydead);
		float origed = makefloat(GetCharacterShipClass(rKillerCharacter)) / makefloat(GetCharacterShipClass(rDead)) * CLASS_SCALAR_FOR_MORALE;
		tmped += origed;
		rKillerCharacter.seatime.enemydead = tmped;
	}
	int iDeadChNation = sti(rDead.nation);
	for(int s = 0; s < iNumShips; s++)
	{
		if(Ships[s] < 0 || Ships[s] == iDeadCharacterIndex) continue; // KK
		schr = GetCharacter(Ships[s]);
		bool bOldEnemy = CheckAttribute(rDead, "surrendered") && GetNationRelation2Character(iDeadChNation,Ships[s]) == RELATION_ENEMY;
		if(SeaAI_GetRelation(iDeadCharacterIndex, Ships[s]) == RELATION_ENEMY || bOldEnemy)
		{
			tmped = 0;
			if(CheckAttribute(schr,"seatime.enemydead")) tmped = stf(schr.seatime.enemydead);
			tmped += origed * CLASS_SCALAR_FOR_MORALE_ORIG_TO_OTHER + makefloat(GetCharacterShipClass(schr)) / makefloat(GetCharacterShipClass(rDead)) * CLASS_SCALAR_FOR_MORALE_OTHER;
			schr.seatime.enemydead = tmped;
		}
	}
}

int GetSeaTime()
{
	ref pchar = GetMainCharacter();
	if(!CheckAttribute(pchar,"seatime")) return 0;
	return sti(pchar.seatime);
}

float FindPowerRatio(int idx)
{
	ref chr = GetCharacter(idx);

	if (CheckAttribute(chr,"seatime.lastPowercheck.val"))
	{
		int iPowercheckdelay = sti(chr.seatime.lastPowercheck);

		if(iPowercheckdelay < 3)
		{
			chr.seatime.lastPowercheck = iPowercheckdelay + 1;
			return stf(chr.seatime.lastPowercheck.val);
		}
	}
	// нет прошлого значения или пора пересчитать:
	chr.seatime.lastPowercheck = 0; //reset counter

	float epower;
	float fpower;

	epower = 0.5;
	fpower = 0.5;


	if(epower < 0.5) epower = 1.0;
	if(fpower < 0.5) fpower = 1.0;

	float fResult = epower / fpower;

	chr.seatime.lastPowercheck.val = fResult; //Fix missing .val
	return fResult;
}

bool Ship_Check_Surrender(int chridx, float fSurrMult)
{
	// старт проверки сдачи (используется и для откупа)
	//#20200307-03
	int nSurr = surrenderenabled;
	if(fSurrMult > 0.0) {
	    switch(nSurr) {
	        case 1: //occasional surrender
                fSurrMult -= frand(0.15);
                fSurrMult += frand(0.1);
            break;
            case 2: //rare surrender
                fSurrMult -= frand(0.25);
                fSurrMult += frand(0.15);
            break;
            case 3://very rare surrender
                fSurrMult -= frand(0.25);
                fSurrMult -= frand(0.25);
            break;
        }
	}

	ref rCharacter = GetCharacter(chridx);

	if(!CheckAttribute(rCharacter,"seatime.tempmorale")) {
		// пересчёт морали
		if (!Ship_CheckMorale(chridx, true)) return false;  //Boyer fix for when Ship_CheckMorale does not set .seatime.tempmorale #20170318-45
	}
	float tempmorale = rCharacter.seatime.tempmorale;
	//#20200307-03
	if(tempmorale < 0.05) {
	    switch(nSurr) {
	        case 1: //occasional surrender
                tempmorale += frand(0.15);
            break;
            case 2: //rare surrender
                tempmorale += frand(0.25);
                tempmorale += frand(0.25);
            break;
            case 3://very rare surrender
                tempmorale += frand(0.5);
                tempmorale += frand(0.5);
            break;
        }
	}
	if (!CheckAttribute(rCharacter, "seatime.surmorale") || stf(rCharacter.seatime.surmorale) < 0.01) {
		return false;
	}
	float surmorale = stf(rCharacter.seatime.surmorale) * fSurrMult;
    //#20200307-03
	if(surmorale < tempmorale) return false;

	int nThink = 25;
	if(CheckAttribute(rCharacter, "seatime.thinkSurr"))
        nThink = sti(rCharacter.seatime.thinkSurr);
    nThink = nThink % 50;
    switch(nSurr) {
        case 1: //occasional surrender
            if(rand(nThink) > 20) return false;
        break;
        case 2: //rare surrender
            if(rand(nThink) > 10) return false;
        break;
        case 3://very rare surrender
            if(rand(nThink) > 5) return false;
        break;
    }

	float surch;
	surch = Bring2Range(0.75, 0.01, 0.01, surmorale, tempmorale);

	surch *= SURR_GLOBAL_SCL; // LDH

	if (surch < 0.1) return false;  //just don't surrender if less then 10 percent chance

	// случайный шанс сдачи
	float fRoll = frnd();
	if(fRoll < surch)
	{
		return true;
	}
	return false;
}

void Ship_Surrender(int chridx)
{
	ref chr = GetCharacter(chridx);
	int pchridx = GetMainCharacterIndex();
	string chid = chr.id;

	if (CheckAttribute(chr, "surrendered") || CheckAttribute(chr, "cannotsurrender")) {
		return;
	}

	int nRel = SeaAI_GetRelation(chridx, pchridx);
	// ставим сдачу
	chr.surrendered = true;
	chr.surrendered.seatime = GetSeaTime();

	// корабль больше ничего не предпринимает
	Ship_SetTaskDrift(PRIMARY_TASK, chridx);
	Ship_SetTaskDrift(SECONDARY_TASK, chridx);

	// поднимаем белый флаг
	Ship_FlagRefresh(chr);

	// флаг: врагам сдавшихся кораблей поднимаем мораль
	AddKillMorale(chr, chr);

	// забываем, что по нам стреляли
	chr.Ship.LastBallCharacter = -1;

	// оповещение о сдаче корабля
	if (nRel == RELATION_ENEMY) {
		Log_SetStringToLog(xiStr("MSG_sea_10") + chr.ship.name + xiStr("MSG_sea_11"));
		PlaySound("interface\notebook.wav");
		PlayVoice("interface\abordage_wining.wav");
	} else {
		Log_SetStringToLog(xiStr("MSG_sea_10") + chr.ship.name + xiStr("MSG_sea_12"));
	}
	Ship_Neutral(chridx, "SURR_GROUP");
}

void Ship_Neutral(int chridx, string sGroup)
{
	ref chr = GetCharacter(chridx);
	string chid = chr.id;

	// запоминаем старые отношения
	if(CheckAttribute(chr,"relation")) {
		chr.oldrelation = "";
		aref arRel; makearef(arRel, chr.relation);
		aref arOldRel; makearef(arOldRel, chr.oldrelation);
		CopyAttributes(arOldRel, arRel);
	}
	// забываем, что по нам стреляли
	chr.Ship.LastBallCharacter = -1;

	// фикс членства в группе: нейтральный корабль меняет группу
	int i;
	ref rGroup = Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup, "trade");

	ref pchar = GetMainCharacter();
	int pchridx = GetMainCharacterIndex();
	ref schr;
	string ogroup = Ship_GetGroupID(&chr);

	int cmdridx = Group_GetGroupCommanderIndex(ogroup);
	if (cmdridx < 0) return;
	ref cmdr = GetCharacter(cmdridx);

	bool cmdrchange = cmdridx == chridx;

	ref rOGroup = Group_GetGroupByID(ogroup);
	bool firstchar = Group_GetCharacterIndexR(rOGroup, 0) == chridx;  // was this char the commander?
	//#20180914-03
	int og_ships = Group_GetLiveCharactersNum(ogroup);

	if (cmdrchange) {
		// новый командир, если командир группы сдался
		if (og_ships > 1) {
			cmdridx = Group_GetCharacterIndexR(rOGroup, firstchar);  //if was the commander, get the second ship in group
			cmdr = GetCharacter(cmdridx);
			DeleteAttribute(cmdr,"relation.UseOtherCharacter");
			Group_SetGroupCommander(ogroup, cmdr.id); //set new commander
			Group_Refresh_Engine_relations(ogroup);
		} else {
		    Group_SetAddress(ogroup, "None", "", "");
			Group_DeleteGroup(ogroup); // KK
		}
	}
	// LEO: сдавшийся корабль каравана ПП уходит из группы квеста в SURR_GROUP. Без метки караван считался разбитым,
	//   как только из группы уходили остальные, и ГГ увозило на берег, а сдавшийся с грузом оставался в море.
	//   Ждёт его PGG_Q1WaitPrizes. Откуп и перемирие (другие группы) бой не держат - корабль уходит с миром.
	if (sGroup == "SURR_GROUP" && CheckAttribute(PChar, "GenQuest.PGG_Quest"))
	{
		if (ogroup == "PGGQuest" || ogroup == "PGGQuestE") PChar.GenQuest.PGG_Quest.SurrPending = true;
	}
	// убираем корабль из старой группы

	Group_DelCharacter(ogroup, chid);

	Event(SHIP_UPDATE_PARAMETERS, "lf", chridx, 1.0);		// Parameters

	DeleteAttribute(chr,"relation.UseOtherCharacter");
	Group_ChangeCharacter(sGroup, chid);
	Group_SetGroupCommander(sGroup, chid);
	ChangeCharacterShipGroup(chr, sGroup); //forces a engine group boss refresh for this char
	Group_DeleteAtEnd(sGroup);

	//#20190728-01
    bool bHasOld = false;
    if(CheckAttribute(chr, "surrendered")) {
        bHasOld = true;
        int bOlSurr = sti(chr.surrendered);
        int nOldTime = sti(chr.surrendered.seatime);
        DeleteAttribute(chr, "surrendered");
    }
	// делаем корабль нейтральным к своим врагам
	for (i = 0; i < iNumShips; i++) {
		if (ships[i] == -1) continue;
		schr = GetCharacter(ships[i]);
		if (chr.id == schr.id) continue;

		if(GetRelation(chridx, ships[i]) == RELATION_ENEMY || GetNationRelation2Character(sti(schr.nation),chridx) == RELATION_ENEMY)
		{
			SetCharacterRelationBoth(chridx, ships[i], RELATION_NEUTRAL);
		}
	}
	// делаем корабли нейтральными к фортам
	if (CheckAttribute(pchar, "seaAI.update.forts")) {
		aref fortattr; makearef(fortattr, PChar.seaAI.update.forts);
		int iFortidx;
		int num = GetAttributesNum(fortattr);
		for (i = 0; i < num; i++)
		{
		    string fortstr = "l" + i;
			iFortidx = sti(fortattr.(fortstr).idx);
			//#20181116-03
			if(GetRelation(nMainCharacterIndex, iFortidx) == RELATION_ENEMY) continue;
			if(GetRelation(chridx, iFortidx)  == RELATION_ENEMY)
			{
				SetCharacterRelationBoth(chridx, iFortidx, RELATION_NEUTRAL);
			}
		}
	}
	//#20190728-01
	if(bHasOld) {
        chr.surrendered = bOlSurr;
        chr.surrendered.seatime = nOldTime;
    }
	NationUpdate();
    DoQuestCheckDelay("NationUpdate", 0.9);
}


void FlagPerkForCapturedShip(ref refChar)
{
	string sMessage = "";
	string sPerk = "";

	if(!CheckAttribute(refChar, "nation")) return;
	switch(sti(refChar.nation))
	{
		case PIRATE:
			sMessage = GetConvertStr("Variable_AIShip_32", "Logs.txt");
			sPerk = "FlagPir";
		break;
		case ENGLAND:
			sMessage = GetConvertStr("Variable_AIShip_33", "Logs.txt");
			sPerk = "FlagEng";
		break;
		case SPAIN:
			sMessage = GetConvertStr("Variable_AIShip_34", "Logs.txt");
			sPerk = "FlagSpa";
		break;
		case FRANCE:
			sMessage = GetConvertStr("Variable_AIShip_35", "Logs.txt");
			sPerk = "FlagFra";
		break;
		case HOLLAND:
			sMessage = GetConvertStr("Variable_AIShip_36", "Logs.txt");
			sPerk = "FlagHol";
		break;
	}
	if(!CheckCharacterPerk(pchar, sPerk)) {
		SetCharacterPerk(pchar, sPerk);
		Log_SetStringToLog(sMessage);
	}
}
