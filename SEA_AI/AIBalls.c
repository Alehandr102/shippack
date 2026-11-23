object	AIBalls;
aref	Grapes, Knippels, Balls, Bombs;
#define CANNON_HEIGHT_MULTIPLY 0.3
// Gregg: палубный прицел 1-го лица — фактор вертикали наводки (theta_launch = theta_cam / фактор).
//        1.0 = ванильный 1:1 (макс. подъём, по форту достаётся); 0.3 = старое сжатие ×3.33; 0.7 = баланс.
//        НЕ путать с CANNON_HEIGHT_MULTIPLY (настильность полёта ядра/дальности ИИ) — это ТОЛЬКО эргономика прицела.
#define DECK_AIM_ANGLE_FACTOR 0.7
// Gregg: палубный залп 1-го лица ГГ — крутизна арки ЯДРА (ванильная параболла). 1.0 = ваниль (высокая дуга),
//        0.3 = как обычный бой (пологая). ТОЛЬКО твои палубные залпы; ИИ/3-е лицо остаются на CANNON_HEIGHT_MULTIPLY.
#define DECK_BALL_ARC 1.0
// Gregg: фокусировка палубного прицела — макс. доля снижения СЛУЧАЙНОГО разброса при полном фокусе (зажатие ЛКМ).
//        ОДНО значение для скрипта (ядра) и движка (овал), иначе разойдутся. 0.5 = до −50% разброса при полном фокусе.
#define DECK_FOCUS_MAX_REDUCTION 0.5
// Gregg: СВЕДЕНИЕ веера (fCannonAngle) по меткости+фокусу — чтобы прокачка/фокус решали, а не только джиттер.
//        Множитель сведения = 1 − Меткость·SKILL − фокус·FOCUS, не ниже FLOOR. Ниже = теснее борт к точке прицела.
//        ДЕРЖАТЬ РАВНЫМИ движковым константам в ai_ship_cannon_controller.cpp (fConvergeK), иначе овал ≠ ядра.
#define DECK_CONV_SKILL 0.35
#define DECK_CONV_FOCUS 0.35
#define DECK_CONV_FLOOR 0.3
// Gregg: МИНИМУМ разброса палубного залпа ГГ — чтобы «мало пушек + высокий скилл» не давали ПИНПОИНТ
//        (Calc_CannonsNumPenalty при малом числе стволов уходит в минус → fAccuracy→0). Флор до сохранения
//        AimAcc → овал и ядра floored синхронно; фокус жмёт уже ОТ флора. ~0.12 ≈ ±2.7 м на 200 м базового скаттера.
#define DECK_MIN_ACC 0.12

// LEO: ядра в полёте (счётчик пишет движок, ai_balls.cpp). Переход в другую локацию зовёт
//   AIBalls.Clear и стирает летящие ядра ВМЕСТЕ с их попаданиями - то есть уводит от чужого залпа
//   без урона. Под залпом такие переходы не пускаем.
bool Sea_BallsInAir()
{
	if (!bSeaActive) return false;
	if (!CheckAttribute(&AIBalls, "BallsInAir")) return false;
	return sti(AIBalls.BallsInAir) > 0;
}

// true = переход запрещён; отказной звук и строка в лог уже выданы. Зовётся из каюты, шлюпки и хоткея.
bool Sea_BlockedByBalls()
{
	if (!Sea_BallsInAir()) return false;
	PlaySound("interface\knock.wav");
	Log_InfoColored("orange", GetConvertStrWithReplace("Variable_BattleInterface_BallsInAir", "Logs.txt", "#space#", " "));
	return true;
}

void DeleteBallsEnvironment()
{
	DeleteClass(&AIBalls);

	DelEventHandler(BALL_WATER_HIT, "Ball_WaterHitEvent");
	DelEventHandler(BALL_ISLAND_HIT, "Ball_IslandHit");
	DelEventHandler(BALL_FLY_UPDATE, "Ball_OnFlyUpdate");
	DelEventHandler(BALL_FORT_HIT, "Ball_FortHit");
	DelEventHandler(BALL_FLY_NEAR_CAMERA, "Ball_FlyNearCamera");
}

void CreateBallsEnvironment()
{
	CreateEntity(&AIBalls, "AIBalls");
	LayerAddObject(SEA_EXECUTE, &AIBalls, -1);
	LayerAddObject(SEA_REALIZE, &AIBalls, 65532);

	// LEO: счётчик ядер в полёте едет в СЕЙВ вместе с объектом. Сохранился под чужим залпом - и после
	//   загрузки атрибут остался ненулевым, хотя ядер нет: движок публикует его только при ИЗМЕНЕНИИ,
	//   а его внутренний кэш после пересоздания сущности тоже ноль, так что сравнение молчит. Итог -
	//   каюта и шлюпка закрыты навсегда вне всякого боя. Гасим на каждый вход в море.
	AIBalls.BallsInAir = 0;
	AIBalls.CurrentBallCannonType = -1;
	AIBalls.CurrentBallDistance = 0.0;
	AIBalls.CurrentMaxBallDistance = 0.0;
	AIBalls.BallFlySoundDistance = 15.0;
	AIBalls.BallFlySoundStereoMultiplyer = 2.0;

	AIBalls.SpeedMultiply = 3.0;
	AIBalls.Texture = "AllBalls.tga";
	AIBalls.SubTexX = 2;
	AIBalls.SubTexY = 2;

	makearef(Grapes,AIBalls.Balls.Grapes);
	makearef(Knippels,AIBalls.Balls.Knippels);
	makearef(Balls,AIBalls.Balls.Balls);
	makearef(Bombs,AIBalls.Balls.Bombs);

	Bombs.SubTexIndex = 0;		Bombs.Size = 0.3;		Bombs.GoodIndex = GOOD_BOMBS;
	Balls.SubTexIndex = 2;		Balls.Size = 0.2;		Balls.GoodIndex = GOOD_BALLS;
	Knippels.SubTexIndex = 3;	Knippels.Size = 0.2;	Knippels.GoodIndex = GOOD_KNIPPELS;
	switch (sti(InterfaceStates.BombsParticles))
	{
		case 0:
			Bombs.Particle = "bomb_smoke_old";
			Balls.Particle = "";
			Knippels.Particle = "";
			Grapes.Particle = "";
		break;
		case 1:
			Bombs.Particle = "bomb_smoke_low";
			Balls.Particle = "ball_smoke_low";
			Knippels.Particle = "ball_smoke_low";
			Grapes.Particle = "ball_smoke_low";
		break;
		case 2:
			Bombs.Particle = "bomb_smoke_high";
			Balls.Particle = "ball_smoke_high";
			Knippels.Particle = "ball_smoke_low";
			Grapes.Particle = "ball_smoke_low";
		break;
	}

	Grapes.SubTexIndex = 1;		Grapes.Size = 0.2;		Grapes.GoodIndex = GOOD_GRAPES;

	AIBalls.isDone = 1;

	// cheat - fire from camera
	AIBalls.FireBallFromCamera = true;
	AIBalls.HeightMultiply    = CANNON_HEIGHT_MULTIPLY;
	AIBalls.DeckAimAngleFactor = DECK_AIM_ANGLE_FACTOR; // Gregg: вертикаль палубного прицела (крутится живьём загрузкой сейва)
	AIBalls.DeckBallArc = DECK_BALL_ARC; // Gregg: крутизна арки палубных ядер 1-го лица (движок читает для рампы/веера)
	AIBalls.DeckFocusMaxReduction = DECK_FOCUS_MAX_REDUCTION; // Gregg: потолок снижения разброса фокусировкой (движок читает для овала)

	SetEventHandler(BALL_WATER_HIT, "Ball_WaterHitEvent", 0);
	SetEventHandler(BALL_ISLAND_HIT, "Ball_IslandHit", 0);
	SetEventHandler(BALL_FLY_UPDATE, "Ball_OnFlyUpdate", 0);
	SetEventHandler(BALL_FORT_HIT, "Ball_FortHit", 0);
	SetEventHandler(BALL_FLY_NEAR_CAMERA, "Ball_FlyNearCamera", 0);
	SetEventHandler("SeaPrecalcAimSpread", "SeaPrecalcAimSpread_Event", 0); // Gregg: предрасчёт разброса до 1-го залпа (палубный прицел)
}

void Ball_FlyNearCamera()
{
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	Play3DSound("fly_ball", x, y, z);
}

int ballNumber;

// Gregg: ПРЕДРАСЧЁТ эффективного разброса ДО первого залпа. Та же формула, что в Ball_AddBall (дублирована 1:1 —
//   при правке баланса разброса в Ball_AddBall обновить и ЗДЕСЬ!), но БЕЗ выстрела: кладёт Aim* в Ship.Cannons,
//   чтобы движок рисовал ТОЧНОЕ накрытие сразу, а не грубый фолбэк от дистанции. Только ГГ. Зовётся движком
//   троттлингом, пока целишься с палубы (core.Event "SeaPrecalcAimSpread" в DrawAimTrajectory) — свежо, подхватывает
//   смену заряда. Нюанс: fCannonsNumPenalty точен лишь ПОСЛЕ 1-го залпа (до него Ship.FiredBort пуст → член=0).
void Ball_PrecalcAimSpread(ref aCharacter)
{
	if (aCharacter.Index != GetMainCharacterIndex()) return;
	int iCannonType = sti(aCharacter.Ship.Cannons.Type);
	AIBalls.Type = Goods[sti(aCharacter.Ship.Cannons.Charge.Type)].Name; // как в Ball_AddBall (для идентичной коэрции GOOD_*)
	float fTempDispersionY = Degree2Radian(9.0);
	float fTempDispersionX = Degree2Radian(7.5);
	if (SeaCameras.Camera == "SeaDeckCamera" && aCharacter.Index == GetMainCharacterIndex())
		fTempDispersionY *= 0.6;
	if (AIBalls.Type == GOOD_KNIPPELS)
		fTempDispersionX *= 3;
	float fCannons = stf(aCharacter.TmpSkill.Cannons);
	float fAccuracy = stf(aCharacter.TmpSkill.Accuracy);
	int nShipType = sti(aCharacter.ship.type);
	ref refBaseShip = GetRealShip(nShipType);
	string sFiredBort = "";
	if (CheckAttribute(aCharacter, "Ship.FiredBort")) {sFiredBort = aCharacter.Ship.FiredBort;}
	int iCannonsNum = 0;
	float fCannonsNumPenalty = 0;
	if (sFiredBort == "cannonl" || sFiredBort == "cannonr")
	{
		iCannonsNum = GetBortIntactCannonsNum(aCharacter, sFiredBort, sti(refBaseShip.(sFiredBort)));
		fCannonsNumPenalty = Calc_CannonsNumPenalty(fAccuracy, iCannonsNum);
	}
	fAccuracy = 0.15 * (3 - fAccuracy) + Calc_CannonsCaliberPenalty(fCannons, iCannonType) + fCannonsNumPenalty;
	if (fAccuracy > 0.6) {fAccuracy = 0.6;}
	if (AIBalls.Type == GOOD_BALLS) {fAccuracy *= 0.57;}
	if (fAccuracy < DECK_MIN_ACC) fAccuracy = DECK_MIN_ACC; // Gregg: минимум палубного разброса (не пинпоинт при малом борте)
	aCharacter.Ship.Cannons.AimAcc = fAccuracy;
	aCharacter.Ship.Cannons.AimDispY = fTempDispersionY;
	aCharacter.Ship.Cannons.AimDispX = fTempDispersionX;
}

void SeaPrecalcAimSpread_Event()
{
	int idx = GetEventData();
	Ball_PrecalcAimSpread(GetCharacter(idx));
}

// FPSTODO: метод очень тяжелый, особенно в замесах проседания видны невооруженным глазом
void Ball_AddBall(aref aCharacter, float fX, float fY, float fZ, float fSpeedV0, float fDirAng, float fHeightAng, float fCannonDirAng, float fMaxFireDistance, float fAngle, int iDeckShot)
{
	// LEO: режим фиксируется пушкой при команде огня; смена камеры не меняет оставшуюся часть залпа.
	// LEO: у загруженного из сейва начатого залпа режим неизвестен - сохраняем прежнее поведение без смены формата.
	if (iDeckShot < 0) iDeckShot = SeaCameras.Camera == "SeaDeckCamera";
	bool bDeckShot = iDeckShot != 0 && sti(aCharacter.Index) == GetMainCharacterIndex();
	int iCannonType = sti(aCharacter.Ship.Cannons.Type);
	ref rCannon = GetCannonByType(iCannonType);
	float fCannonHeightMultiply = stf(rCannon.HeightMultiply);

	EntityUpdate(0);
	AIBalls.CannonType = iCannonType;
	AIBalls.x = fX;
	AIBalls.y = fY;
	AIBalls.z = fZ;
	AIBalls.CharacterIndex    = aCharacter.Index;
	AIBalls.Type = Goods[sti(aCharacter.Ship.Cannons.Charge.Type)].Name;
	float fTempDispersionY = Degree2Radian(9.0); //отклонение по горизонтали	//LEO: Важные параметры разброса снарядов, ванильное: 15.0
	float fTempDispersionX = Degree2Radian(7.5); //отклонение по вертикали		//(5.0)
	if(bDeckShot)
	{
		fTempDispersionY *= 0.6;
	}
	if(AIBalls.Type == GOOD_KNIPPELS)
	{
		fTempDispersionX *= 3; 
	}
	// Gregg: палубный залп 1-го лица ГГ — ванильная (крутая) арка вместо пологой; ИИ/3-е лицо как прежде.
	// LEO: тот же сохранённый режим выстрела, что для разброса выше.
	if (bDeckShot)
		fCannonHeightMultiply *= DECK_BALL_ARC;//высота траектории (палуба)
	else
		fCannonHeightMultiply *= CANNON_HEIGHT_MULTIPLY;//высота траектории
	AIBalls.HeightMultiply    = fCannonHeightMultiply;
	AIBalls.SizeMultiply      = rCannon.SizeMultiply;
	AIBalls.TimeSpeedMultiply = rCannon.TimeSpeedMultiply;
	AIBalls.MaxFireDistance   = fMaxFireDistance;
	AIBalls.RawAng = fAngle;



	float fCannons = stf(aCharacter.TmpSkill.Cannons);
	float fAccuracy = stf(aCharacter.TmpSkill.Accuracy);

	int nShipType = sti(aCharacter.ship.type);
	ref refBaseShip = GetRealShip(nShipType);
	string sFiredBort = "";
	if (CheckAttribute(aCharacter, "Ship.FiredBort")){sFiredBort = aCharacter.Ship.FiredBort;}
	
	float fCannonAngle = 0;
	int iCannonsNum = 0;
	float fCannonsNumPenalty = 0;

	if (sFiredBort == "cannonl" || sFiredBort == "cannonr"){	//погонные орудия легче навести
		iCannonsNum = GetBortIntactCannonsNum(aCharacter, sFiredBort, sti(refBaseShip.(sFiredBort)));
		fCannonsNumPenalty = Calc_CannonsNumPenalty(fAccuracy, iCannonsNum);//0...0.1
		
		float fDistanceToEnemy = 200;//костыль; должно получаться из движка
		if(checkAttribute(aCharacter, "DistanceToEnemy")){fDistanceToEnemy = stf(aCharacter.DistanceToEnemy);} 
		float fShipPosX = stf(aCharacter.ship.pos.x);
		float fShipPosZ = stf(aCharacter.ship.pos.z);
		fCannonAngle = sqrt(sqr(fX-fShipPosX)+sqr(fZ-fShipPosZ));
		if (!CheckAttribute(aCharacter, "Ship.MaxCannonDist")) {
			aCharacter.Ship.MaxCannonDist = 0;
		}
		fShipPosX = stf(aCharacter.Ship.MaxCannonDist);
		if (fShipPosX > stf(aCharacter.ship.BoxSize.z) / 2) {
			Log_TestInfo("Ship.MaxCannonDist is broken! recalculating...")
			aCharacter.Ship.MaxCannonDist = 0.0;
			fShipPosX = 0.0;
		}
		if (fShipPosX < fCannonAngle) { 
			fShipPosX = fCannonAngle;
			aCharacter.Ship.MaxCannonDist = fCannonAngle;
		}
		fCannonAngle /= fDistanceToEnemy;
		fShipPosX = fShipPosX / fDistanceToEnemy;
		fCannonAngle = - asin(Clampf(fCannonAngle)) + 0.5 * asin(Clampf(fShipPosX));//указание угла. Делим сдвиг на 2 чтобы целило не концом залпа а его серединой
		// Gregg: СВЕДЕНИЕ веера по меткости+фокусу (палуба ГГ) — мастер/сфокусированный сводит борт теснее к
		//        точке прицела. ИИ/3-е лицо/автозалп не трогаем (гейт). Тот же фактор в движке (fConvergeK) → овал=ядра.
		if (bDeckShot)
		{
			float fFoc = 0.0;
			if (CheckAttribute(aCharacter, "Ship.Cannons.AimFocusFired")) fFoc = stf(aCharacter.Ship.Cannons.AimFocusFired);
			float fConv = 1.0 - stf(aCharacter.TmpSkill.Accuracy) * DECK_CONV_SKILL - fFoc * DECK_CONV_FOCUS;
			if (fConv < DECK_CONV_FLOOR) fConv = DECK_CONV_FLOOR;
			fCannonAngle *= fConv;
		}
	}

	fAccuracy = 0.15 * (3 - fAccuracy) + Calc_CannonsCaliberPenalty(fCannons, iCannonType) + fCannonsNumPenalty;
	fCannons = 15.0 + MOD_SKILL_ENEMY_RATE*3 - 10.0 * fCannons;//сложность влияет на частоту нанесения урона! И на неписей тоже распространяется???

	

	if (fAccuracy > 0.6) {fAccuracy = 0.6;}
	if (AIBalls.Type == GOOD_BALLS) {fAccuracy *= 0.57;}//ядра в 3 раза точнее
	// Gregg: МИНИМУМ палубного разброса ГГ (не пинпоинт при малом борте / высоком скилле). До сохранения AimAcc,
	//        поэтому овал и ядра floored синхронно; фокус/сведение жмут уже от флора. Гейт: палуба + ГГ (ИИ не трогаем).
	if (bDeckShot)
	{
		if (fAccuracy < DECK_MIN_ACC) fAccuracy = DECK_MIN_ACC;
	}

	//Gregg: сохраняем ФАКТИЧЕСКИЙ эффективный разброс для предиктивного овала накрытия (палубный прицел).
	//       Значения уже учитывают меткость/орудия/заряд/палубу/число пушек И реальное поведение веток выше —
	//       движок не гадает формулу, а берёт эти 3 числа и строит овал геометрией. Только для ГГ (перф).
	if (AIBalls.CharacterIndex == GetMainCharacterIndex())
	{
		aCharacter.Ship.Cannons.AimAcc = fAccuracy;
		aCharacter.Ship.Cannons.AimDispY = fTempDispersionY;
		aCharacter.Ship.Cannons.AimDispX = fTempDispersionX;
	}

	// Gregg: ФОКУСИРОВКА — палубный залп ГГ с НАБРАННОЙ точностью. Движок латчит достигнутый фокус в
	//        Ship.Cannons.AimFocusFired (в момент отпускания огня). Снижаем РАЗБРОС ядер (девиации ниже),
	//        НЕ трогая сохранённый AimAcc (овал уменьшает движок по ЖИВОМУ фокусу — иначе двойное снижение).
	//        Гейт: палуба + ГГ (в 3-е лицо/автоогонь фокус не течёт).
	if (bDeckShot && CheckAttribute(aCharacter, "Ship.Cannons.AimFocusFired"))
	{
		fAccuracy *= (1.0 - stf(aCharacter.Ship.Cannons.AimFocusFired) * DECK_FOCUS_MAX_REDUCTION);
	}
	AIBalls.Dir = fDirAng + fCannonAngle + fAccuracy * 1.2 * fTempDispersionY * (frnd() +frnd() - 1);//горизонтальная_наводка + разброс
	AIBalls.SpdV0 = fSpeedV0 + 0.5 * fCannonAngle + fAccuracy * (10.0 * fTempDispersionY) * (frnd() - 0.5);//скорость_снаряда + разброс_скорости
	AIBalls.Ang = fHeightAng + 0.33 * fCannonAngle + fAccuracy * (fTempDispersionX) * (frnd() + frnd() - 1);//вертикальная_наводка + разброс 
	AIBalls.Event = "";

	EntityUpdate(1);
	AIBalls.Add = "";

	string sParticleName = "cancloud_fire";
	if (InterfaceStates.FireParticles) sParticleName = "cancloud_fire_ext";

	if (iCannonType >= CANNON_TYPE_CANNON_LBS48) {
        sParticleName = "Bombard";
    }

	if (iCannonType >= CANNON_TYPE_CANNON_LBS8)//пушки
	{
		if (iCannonType >= CANNON_TYPE_CANNON_LBS32) {
			if (InterfaceStates.FireParticles) sParticleName = "cancloud_fire_big_ext";
			else sParticleName = "cancloud_fire_big";
		}
		else if (iCannonType <= CANNON_TYPE_CANNON_LBS12) {
			if (InterfaceStates.FireParticles) sParticleName = "cancloud_fire_small";
			else sParticleName = "cancloud_fire";
		}
	}
	else//кулеврины
	{
		if (iCannonType >= CANNON_TYPE_CULVERINE_LBS32) {
			if (InterfaceStates.FireParticles) sParticleName = "cancloud_fire_big_ext";
			else sParticleName = "cancloud_fire_big";
		}
		else if (iCannonType <= CANNON_TYPE_CULVERINE_LBS12) {
			if (InterfaceStates.FireParticles) sParticleName = "cancloud_fire_small";
			else sParticleName = "cancloud_fire";
		}
	}
	CreateParticleSystemNoHeat(sParticleName, fX, fY, fZ, -fHeightAng * fCannonHeightMultiply, fDirAng, 0.0, 5);
	
	if (CheckAttribute(aCharacter, "fort"))
	{
		Play3DSound("cannon_fort_fire_cannon_48", fX, fY, fZ);
	}
	else if (sti(InterfaceStates.EnabledAltSoundsGun) != 0)
	{
		Play3DSound(rCannon.Sound + "_alt", fX, fY, fZ);
	}
	else
	{
		Play3DSound(rCannon.Sound, fX, fY, fZ);
	}
}

//штраф от калибра орудий. -0.1...0...0.1f
//10 навыка "орудия" нивелируют штраф от 8 фт, 100 навыка - от 24фт.
//приемлемый штраф - до 0.1 - 24фт при 10 навыка и 42фт при 100 навыка
//fCannons - навык "орудия" канонира, iCannonType - тип орудий
float Calc_CannonsCaliberPenalty(float fCannons, int iCannonType)
{
    return (GetCannonCaliber(iCannonType) - 6 - fCannons*20)/160;
}

//штраф от текущего количества пушек одного борта. -0.1...0...0.1f
//Стартует с 6 пушек, макс. навык перекрывает 23 пушки.
//приемлемый штраф - до 0.1 - при превышении количества в ~2 раза на 10 навыка и в ~3 на 100
//fAccuracy - навык "меткость" канонира, iCannonsNum количество целых пушек борта
float Calc_CannonsNumPenalty(float fAccuracy, int iCannonsNum)
{
	return (2 * MakeFloat(iCannonsNum)/(10+MakeFloat(iCannonsNum)) - fAccuracy - 0.4)/4;
}

void Ball_WaterHitEvent()
{
	int		iCharacterIndex;
	float	x, y, z, vx, vy, vz;

	iCharacterIndex = GetEventData();
	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	int nBallType = sti(AIBalls.CurrentBallCannonType);
	if (nBallType >= 0)
	{
		ref rCannon = GetCannonByType(nBallType);
		if (CheckAttribute(rCannon, "BigBall") && sti(rCannon.BigBall))
			SendMessage(&BallSplash, "lffffff", MSG_BALLSPLASH_ADD, x, y, z, vx, vy, vz);
		else
			SendMessage(&BallSplash, "lffffff", MSG_BALLSPLASH_ADD, x, y, z, vx, vy, vz);
	}
	Play3DSound("ball_splash", x, y, z);
}

void Ball_FortHit()
{
	float	x, y, z;
	int iBallCharacterIndex = GetEventData();
	x = GetEventData();
	y = GetEventData();
	z = GetEventData();
	int iFortCharacterIndex = GetEventData();
	// LEO: владелец ядра / форт могут быть -1 (корабль-стрелок ГГ взорвался, ядра в полёте) -> GetCharacter(-1) спам. Гвард ДО GetCharacter. (|| безопасен — int-сравнения.)
	if (iBallCharacterIndex < 0 || iFortCharacterIndex < 0) return;
	if (iBallCharacterIndex == iFortCharacterIndex) { return; }
	ref rBallCharacter = GetCharacter(iBallCharacterIndex);
	ref rFortCharacter = GetCharacter(iFortCharacterIndex);
	//FORT_ABORDAGE 1 //FORT_DEAD 2
	if (rFortCharacter.Fort.Mode != FORT_NORMAL) {
		return;
	}
	bool bImmortal = LAi_IsImmortal(rFortCharacter);
	if(bImmortal) {
		return;
	}
	ref rCannon = GetCannonByType(sti(rBallCharacter.Ship.Cannons.Type));
	float fCannonDamageMultiply = stf(rCannon.DamageMultiply);

	ref rBall = GetGoodByType(sti(AIBalls.CurrentBallType));
	//1% у ядер, 4% у бомб
	if (sti(AIBalls.CurrentBallType) != GOOD_GRAPES && 0.01 * (1.0 + stf(AIBalls.CurrentBallType)) > Random())
	{
		fCannonDamageMultiply *= 5.0;
		if (iBallCharacterIndex == nMainCharacterIndex) {
			// LEO: крит по ФОРТУ. Чекбокс ShipCapShowCrit вкл -> всплывающий "Критическое попадание!" (красный, флэш)
			// над фортом, БЕЗ строки в логинфо. Выкл -> по-старому строка в лог. (Тот же чекбокс, что и для крита по кораблю.)
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
		if (!IsCompanion(rFortCharacter) && IsCompanion(rBallCharacter) && bDisableMapEnter)
		{
			AddCharacterExpToSkill(rBallCharacter, "Accuracy", 10);
			AddCharacterExpToSkill(rBallCharacter, SKILL_FORTUNE, 10);
			AddCharacterExpToSkill(rBallCharacter, "Leadership", 2);
			AddCharacterExpToSkill(rBallCharacter, "Cannons", 10);
			ChangeCrewExp(rBallCharacter, "Cannoners", AIShip_isPerksUse(CheckCharacterPerk(rBallCharacter, "SeaWolf"), 0.02, 0.04));
		}
		Ship_Serious_Boom(x, y, z);
	}
	float fHullDamage = 0.09 * stf(rBall.DamageHull) * fCannonDamageMultiply;
	float fCrewDamage = 0.09 * stf(rBall.DamageCrew) * fCannonDamageMultiply;
	float fDistanceDamageMultiply = Bring2Range(1.25, 0.25, 0.0, stf(AIBalls.CurrentMaxBallDistance), stf(AIBalls.CurrentBallDistance));

	rFortCharacter.Ship.HP = stf(rFortCharacter.Ship.HP) - fHullDamage * fDistanceDamageMultiply;
	Ship_ApplyCrewHitpoints(rFortCharacter, fCrewDamage);
	Fort_ChangeRelation2MainCharacter_FromDamage(rFortCharacter, iFortCharacterIndex, iBallCharacterIndex, 0.1 * fHullDamage);

	// 1% хп должно остаться на адмирале, 26% на капитане, 51% на матросе 	
	if (rFortCharacter.Ship.HP < (0.76 - 0.25 * MOD_SKILL_ENEMY_RATE) * stf(rFortCharacter.Fort.HP) ) {
		Fort_SetAbordageMode(rBallCharacter, rFortCharacter);
		SetCharacterGoods(rFortCharacter, GOOD_BOMBS,  200 + rand(100));
		SetCharacterGoods(rFortCharacter, GOOD_POWDER,  200 + rand(100));
	}

	CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0); // boal fix
	SendMessage(&AIFort, "llfff", AI_MESSAGE_FORT_HIT, iBallCharacterIndex, x, y, z);
}

void Ball_IslandHit()
{
	int		iCharacterIndex;
	float	x, y, z;

	iCharacterIndex = GetEventData();

	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	if (rand(2) == 1) CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0); // boal fix

}

void Ball_OnFlyUpdate()
{
	int charIndex = GetEventData();
	int ballAlive = GetEventData();
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	float lx = GetEventData();
	float ly = GetEventData();
	float lz = GetEventData();
	SendMessage(&SeaOperator, "lalffffff", MSG_SEA_OPERATOR_BALL_UPDATE, &Characters[charIndex], ballAlive, x, y, z, lx, ly, lz);
}
