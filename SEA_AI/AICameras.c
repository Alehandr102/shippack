object	SeaCameras;
aref	Crosshair;
object	SeaShipCamera,SeaFreeCamera,SeaDeckCamera;
ref		SeaShipCharacterForCamera;
bool	bCanSwitchCameras = true;

// === Тактическая пауза (вид сверху) =========================================
// Вариант A: одна клавиша морозит бой (SetTimeScale 0) и поднимает камеру
// в обзорный вид сверху от своего корабля. Приказы кораблям отдаются штатным
// командным меню (Enter / СКМ), которое работает и на паузе.
// Признак «мы в тактическом виде» — наличие атрибута SeaShipCamera.TacSave
// (без новых глобалов, чтобы не задеть совместимость сейвов).
// На вид влияют эти атрибуты SHIP_CAMERA (проверено по движку):
//   MinAngleX/MaxAngleX — границы клампа наклона (рад), движок жмёт угол в них;
//   Distance — нормализованный зум 0..1 (1.0 = макс. отдаление до потолка высоты);
//   MaxCamHeight — потолок высоты камеры над кораблём (м). Новый атрибут движка
//     (ship_camera.cpp): вертикальная полуось орбиты, по умолчанию 70. Подняв его,
//     получаем настоящий обзор «сверху на всю свалку». Старое ограничение ~70 м снято.
// MinDistance/MaxDistance/MaxHeightOnShip движком не используются — не трогаем.
#define TACVIEW_ANGLE_MAX	-1.45	// (не влияет на тактический наклон — он задаётся fTacTilt в движке)
#define TACVIEW_ANGLE_MIN	-1.50	// клампы vAng.x; тактическая камера считает позицию напрямую
#define TACVIEW_DIST		1.0		// макс. отдаление камеры от корабля
#define TACVIEW_HEIGHT		600.0	// высота камеры над кораблём (м); крути под нужный обзор
#define TACVIEW_HEIGHT_DEF	70.0	// ванильный потолок высоты (дефолт движка) — для возврата
// Пан/зум на паузе (Вариант B, Стадия 2). Зум меняет MaxCamHeight (работает на паузе),
// пан двигает центр орбиты камеры (движковый хук TacCenter* в SHIP_CAMERA).
#define TACVIEW_HEIGHT_MIN	150.0	// мин. высота при зуме (м)
#define TACVIEW_HEIGHT_MAX	900.0	// макс. высота при зуме (м); согласован с движковым TAC_HEIGHT_MAX
#define TACVIEW_ZOOM_STEP	60.0	// шаг зума за тик колеса (м)
#define TACVIEW_PAN_FACTOR	0.012	// шаг пана за нажатие = текущая_высота * фактор
#define TAC_FIRE_SNAP_R2	14400.0	// радиус снапа залпа на врага (квадрат, 120 ед.): клик в этом радиусе от живого врага -> упреждение по нему
#define TAC_LEAD_SCALE		1.0		// множитель упреждения для тактического залпа (движок, AIShipCannonController::Fire). ИИ юзает 2.0 (перелетало). Калибруется без пересборки
#define TACVIEW_PAUSE_TIMESCALE	0.0625	// «пауза» тактики = slow-mo (1/16): мир ползёт видимо и согласованно (на этом скейле целочисленная игровая дельта остаётся ≥1). 0.0 = жёсткая заморозка

//#20171223-01 Camera perspective option
#define DEFAULT_CAM_PERSP 1.285
#define DEFAULT_CAM_PERSP_DEN 1.25

void DeleteSeaCamerasEnvironment()
{
	// выходим из моря — снять тактическую паузу, если была активна
	if (CheckAttribute(&SeaShipCamera, "TacSave"))
	{
		DeleteAttribute(pchar, "pause");
		SetTimeScale(1.0);
		TimeScaleCounter = 0;
		if (IsPerkIntoList("TimeSpeed")) { DelPerkFromActiveList("TimeSpeed"); }
	}
	DeleteClass(&SeaCameras);
	DeleteClass(&SeaFreeCamera);
	DeleteClass(&SeaShipCamera);
	DeleteClass(&SeaDeckCamera);
	DelEventHandler("SeaCameras_Switch", "SeaCameras_Switch");
	DelEventHandler(TELESCOPE_ACTIVE, "SeaCameras_TelescopeActive");
}

float CalcLandRadiusNew()
{
    return 2.0 + Clampf(stf(InterfaceStates.RadDetails))*3.0;
}

//#20171223-01 Camera perspective option
void SetPerspectiveSettings()
{
    float fCamPersp = CalcSeaPerspective();

    SeaFreeCamera.Perspective = fCamPersp;
	SeaShipCamera.Perspective = fCamPersp;

	float fMod = GetMultiplyFromScreenRatio(); //добавил множитель от соотношения сторон экрана
	SendMessage(&locCamera, "lf", MSG_CAMERA_SET_RADIUS, CalcLandRadiusNew() * fMod);
	SendMessage(&locCamera, "lf", MSG_CAMERA_SET_PERSPECTIVE, CalcLandPerspective());
}
//#20171223-01 Camera perspective option
float CalcSeaPerspective()
{
    float fCamPerspAdj = 0.0;
    if(CheckAttribute(&InterfaceStates, "PerspDetails"))
       fCamPerspAdj = stf(InterfaceStates.PerspDetails) / DEFAULT_CAM_PERSP_DEN;

    return (DEFAULT_CAM_PERSP + fCamPerspAdj);
}

// LEO: «Свободная камера в море» - откреп орбиты от курса корабля (движок: SHIP_CAMERA, атрибут FreeAzimuth,
// читается КАЖДЫЙ кадр, поэтому применяется мгновенно). Зовётся при создании камер и из клика по опции.
// Атрибут пишем всегда - без него движок читает 0, то есть ванильное поведение «камера жёстко за кормой».
void SeaCamFreeAzimuth_ApplyLive()
{
	int iOn = 0;
	if (CheckAttribute(&InterfaceStates, "SeaCamFreeAzimuth")) { iOn = sti(InterfaceStates.SeaCamFreeAzimuth); }
	SeaShipCamera.FreeAzimuth = iOn;
}

void CreateSeaCamerasEnvironment()
{
	bCanSwitchCameras = true;

	CreateEntity(&SeaCameras, "SEA_CAMERAS");
	CreateEntity(&SeaFreeCamera, "FREE_CAMERA");
	CreateEntity(&SeaShipCamera, "SHIP_CAMERA");
	CreateEntity(&SeaDeckCamera, "DECK_CAMERA");

	LayerAddObject(SEA_EXECUTE, &SeaShipCamera, iShipPriorityExecute + 5);
	LayerAddObject(SEA_EXECUTE, &SeaFreeCamera, 1);
	LayerAddObject(SEA_EXECUTE, &SeaDeckCamera, iShipPriorityExecute + 5);
	LayerAddObject(SEA_REALIZE, &SeaShipCamera, 100); // SHIP_CAMERA::Realize, проход 0: 3D-оверлеи (прицел/линии), только в тактике
	LayerAddObject(INTERFACE_REALIZE, &SeaShipCamera, 100); // проход 1: 2D HUD-текст ПОСЛЕ воды (иначе море замывает текст)
	LayerAddObject(SEA_EXECUTE, &SeaCameras, -2); // LEO: шаг плавного бленда камер — приоритет ниже камер => тикает ПОСЛЕ них (порядок слоя по убыванию)

    //#20171223-01 Camera perspective option
    SetPerspectiveSettings();

	// Ship camera parameters
	SeaShipCamera.SensivityDistance = 30.0;
	SeaShipCamera.SensivityHeightAngle = 0.02;
	SeaShipCamera.SensivityHeightAngleOnShip = 0.005;
	SeaShipCamera.SensivityAzimuthAngle = 0.04;
	SeaShipCamera.MaxAngleX = 0.07;
	SeaShipCamera.MinAngleX = -1.4;
	SeaShipCamera.Distance = 70.0;
	SeaShipCamera.MinDistance = 25.0;
	SeaShipCamera.MaxDistance = 1500.0;
	SeaShipCamera.MinHeightOnSea = 1.0;
	SeaShipCamera.MaxHeightOnShip = 5.0;
	SeaShipCamera.InvertMouseX = 1.0;
	SeaShipCamera.InvertMouseY = -1.0;
	SeaCamFreeAzimuth_ApplyLive(); // LEO: откреп камеры от курса корабля - по опции «Свободная камера в море»

	// Deck camera parameters
	SeaDeckCamera.Perspective = 1.285;
	SeaDeckCamera.SensivityDistance = 0.01;
	SeaDeckCamera.SensivityHeightAngle = 0.0015;
	SeaDeckCamera.SensivityAzimuthAngle = 0.0015;
	SeaDeckCamera.StepUp = 0.5;
	SeaDeckCamera.StepMin = 0.01;
	SeaDeckCamera.h_max = 2.0;
	SeaDeckCamera.h_min = 1.6;
	SeaDeckCamera.h_step = 0.1;
	SeaDeckCamera.h_def = 1.6;
	SeaDeckCamera.MaxAngleX = 0.7;
	SeaDeckCamera.MinAngleX = -1.3;
	SeaDeckCamera.RockingX = 0.5;
	SeaDeckCamera.RockingZ = 0.5;
	// LEO: режим "Марс" (воронье гнездо) — параметры под-режима палубной камеры
	SeaDeckCamera.MarsMode = 0;        // выкл по умолчанию
	SeaDeckCamera.MarsDropK = 0.1;     // крепиться чуть ниже клотика (доля высоты мачты)
	SeaDeckCamera.MarsMaxAngleX = 1.4; // обзор вниз/вверх шире, чем с палубы (~±80°)
	SeaDeckCamera.MarsMinAngleX = -1.4;

	makearef(Crosshair,SeaCameras.Crosshair);

	if (!bSeaLoad)
	{
		SeaCameras.Camera = "SeaShipCamera";

		Crosshair.OutsideCamera = true;
	}

	Crosshair.Texture = "crosshair\crosshair.tga";
	Crosshair.Size = 0.05;
	Crosshair.Technique = "Crosshair";
	Crosshair.SubTexX = 1;
	Crosshair.SubTexY = 1;
	Crosshair.Colors.Default = argb(0, 255, 255, 0);
	Crosshair.Colors.Enemy = argb(0, 255, 0, 0);
	Crosshair.Colors.Friend = argb(0, 0, 255, 0);
	Crosshair.Colors.Neutral = argb(0, 128, 128, 128);

	// Gregg: синхронизировать штатный ретикл с сохранённым режимом наводки (переключатель X) — показываем его
	// ТОЛЬКО в режиме 0 (обычный прицел); в режимах 1/2 работает предиктивный помощник и ретикл скрыт.
	int nAimModeInit = 1;
	if (CheckAttribute(pchar, "DeckAimMode")) nAimModeInit = sti(pchar.DeckAimMode);
	if (nAimModeInit == 0) { Crosshair.Show = 1; } else { Crosshair.Show = 0; }
	// Gregg: дистанция скрытия рампы/круга при стрельбе В УПОР (м) — крути под себя. Движок читает
	// pchar.DeckAimHideDist в DrawAimTrajectory; ближе неё ни рампа, ни круг не рисуются (точки остаются).
	pchar.DeckAimHideDist = 30.0;

	SendMessage(&AISea, "la", AI_MESSAGE_SET_CAMERAS_ATTRIBUTE, &SeaCameras);

	SetEventHandler("SeaCameras_Switch", "SeaCameras_Switch", 1);
	SetEventHandler(TELESCOPE_ACTIVE, "SeaCameras_TelescopeActive", 0);
	SetEventHandler("evntMarsAutoExit", "Sea_MarsAutoExit", 0); // LEO: движок шлёт при потере всех мачт в режиме "Марс"

	// add cameras to list
	SendMessage(&SeaCameras, "li", AI_CAMERAS_ADD_CAMERA, &SeaShipCamera);
	SendMessage(&SeaCameras, "li", AI_CAMERAS_ADD_CAMERA, &SeaFreeCamera);
	SendMessage(&SeaCameras, "li", AI_CAMERAS_ADD_CAMERA, &SeaDeckCamera);
}

void SeaCameras_TelescopeActive()
{
	int iTelescopeActive = GetEventData();
	if (iTelescopeActive)
	{
		bCanSwitchCameras = false;
		Crosshair.OutsideCamera = true;
	}
	else
	{
		bCanSwitchCameras = true;
		Crosshair.OutsideCamera = SeaCameras_isCameraOutside();
	}
}

void SeaCameras_Switch()
{
	if (!bCanSwitchCameras) return;

	// LEO: TAB в режиме "Марс" — спускаемся на палубу (выход из Марса), саму камеру не переключаем
	if (CheckAttribute(&SeaDeckCamera, "MarsMode"))
	{
		if (sti(SeaDeckCamera.MarsMode) != 0) { SeaDeckCamera.MarsMode = 0; return; }
	}

	int bSwitch = false;
	// Коммент - выбираем состояние, ИЗ КОТОРОГО ПЕРЕКЛЮЧАЕМСЯ
	// если Sailors.IsOnDeck == 1, значит, мы на палубе, и матросов бегающих мы не увидем
	switch (SeaCameras.Camera)
	{
		case "SeaFreeCamera":
			SeaCameras.Camera = "SeaShipCamera";
			Crosshair.OutsideCamera = true;
			Sailors.IsOnDeck = 0;
			bSwitch = true;
		break;
		case "SeaShipCamera":
			if (!LAi_IsDead(&Characters[nMainCharacterIndex]))
			{
				SeaCameras.Camera = "SeaDeckCamera";
				Crosshair.OutsideCamera = false;
				//Boyer change...show sailors when on deck first person
				Sailors.IsOnDeck = 0;
				bSwitch = true;
			}
		break;
		case "SeaDeckCamera":
			SeaCameras.Camera = "SeaShipCamera";
			// boal -->
			if (locCameraEnableFree)
            {
			    SeaCameras.Camera = "SeaFreeCamera";
			}
			// boal <--
			Crosshair.OutsideCamera = true;
			Sailors.IsOnDeck = 0;
			bSwitch = true;
		break;
	}
	if (bSwitch) SeaCameras_UpdateCamera();
}

void SeaCameras_UpdateCamera()
{
	switch (SeaCameras.Camera)
	{
		case "SeaShipCamera":
			SendMessage(&SeaCameras, "lia", AI_CAMERAS_SET_CAMERA, &SeaShipCamera, &SeaShipCharacterForCamera);
		break;
		case "SeaFreeCamera":
			SendMessage(&SeaCameras, "lia", AI_CAMERAS_SET_CAMERA, &SeaFreeCamera, &SeaShipCharacterForCamera);
		break;
		case "SeaDeckCamera":
			SendMessage(&SeaCameras, "lia", AI_CAMERAS_SET_CAMERA, &SeaDeckCamera, &SeaShipCharacterForCamera);
		break;
	}
}

void SeaCameras_SetDieCamera()
{
	bool bOldCanSwitchCameras;
	if (SeaCameras.Camera == "SeaDeckCamera")
	{
		SeaCameras.Camera = "SeaShipCamera";
		Crosshair.OutsideCamera = true;
		Sailors.IsOnDeck = 0;
		DeleteClass(&Telescope);
		SeaCameras_UpdateCamera();
		SendMessage(&objISpyGlass, "ll", MSG_TELESCOPE_REQUEST,0); // выключить телескоп
	}
}

bool SeaCameras_isCameraOutside()
{
	if (SeaCameras.Camera == "SeaShipCamera") return true;
	if (SeaCameras.Camera == "SeaFreeCamera") return true;
	if (SeaCameras.Camera == "SeaDeckCamera") return false;
	return false;
}

void SeaCameras_SetShipForSeaCamera(object Character)
{
	makeref(SeaShipCharacterForCamera, Character);
	SeaCameras_UpdateCamera();
}

// === Режим "Марс" (воронье гнездо) ==========================================
// LEO: вход/выход Z. Камера садится на верхушку ЦЕЛОЙ мачты (под-режим DECK_CAMERA в движке),
// переход между мачтами < и > опрашивает сам движок. Вход только с палубной камеры (1-е лицо).
void Sea_ToggleMarsView()
{
	if (!bSeaActive || bAbordageStarted) return;

	bool bOn = false;
	if (CheckAttribute(&SeaDeckCamera, "MarsMode"))
	{
		if (sti(SeaDeckCamera.MarsMode) != 0) bOn = true;
	}
	if (bOn)
	{
		// ВЫХОД на палубу — всегда (в т.ч. при активной подзорной трубе)
		SeaDeckCamera.MarsMode = 0;
		return;
	}

	// ВХОД — не из трубы/тактики (там bCanSwitchCameras == false)
	if (!bCanSwitchCameras) return;
	// только с палубы (от первого лица)
	if (SeaCameras.Camera != "SeaDeckCamera")
	{
		Log_SetStringToLog("Поднимитесь на палубу (вид от первого лица), чтобы залезть на марс");
		PlaySound("interface\knock.wav");
		return;
	}
	// нужны целые мачты
	int intact = GetShipMastsQuantity(pchar) - GetShipFallMastsQuantity(pchar);
	if (intact <= 0)
	{
		Log_SetStringToLog("Все мачты сбиты — подняться на марс некуда");
		PlaySound("interface\knock.wav");
		return;
	}
	SeaDeckCamera.MarsMode = 1;
	// LEO: НЕ трогаем bCanSwitchCameras — в Марсе можно смотреть в подзорную трубу (она привязана к палубной
	// камере). Смену камеры (TAB) и тактику в Марсе перехватываем отдельно — выходом из Марса, см.
	// SeaCameras_Switch / Sea_TacticalViewOn — а не глобальной блокировкой, иначе труба бы не работала.
	// LEO: реплика при подъёме на марс ("Каков обзор на марсе?") — 2 вариации, не языко-зависимые
	PlayVoice("Interface\Mars_"+rand(1)+".ogg"); // rand(1) включителен -> 0 или 1
}

// Gregg: X — циклическое переключение режима палубного прицела:
//   0 = обычный прицел (штатный ретикл), 1 = дуга + круг накрытия + точки, 2 = круг накрытия + точки (без дуги).
// Режим хранится на pchar.DeckAimMode (атрибут => сейв-безопасно); движок читает его в DrawAimTrajectory.
// Штатный ретикл — движковый гейт по SeaCameras.Crosshair.Show: показываем только в режиме 0.
void Sea_ToggleAimMode()
{
	if (!bSeaActive || bAbordageStarted) return;

	int mode = 1;
	if (CheckAttribute(pchar, "DeckAimMode")) mode = sti(pchar.DeckAimMode);
	mode = mode + 1;
	if (mode > 2) mode = 0;
	pchar.DeckAimMode = mode;

	if (mode == 0) { SeaCameras.Crosshair.Show = 1; } // обычный прицел — вернуть штатный ретикл
	else           { SeaCameras.Crosshair.Show = 0; } // предиктивный помощник — ретикл скрыт

	Log_SetStringToLog(XI_ConvertString("DeckAimMode_" + mode)); // локализация из common.ini
}

// === Быстрый вход в подзорную трубу от 3-го лица (CTRL) =====================
// LEO: зажал CTRL в 3-м лице -> плавный перелёт на палубу (дефолт) или на ближайший марс
//      (опция InterfaceStates.SpyglassFromMars) + поднять трубу; отпустил -> назад в 3-е лицо.
//      Направление взгляда сохраняется в обе стороны блендом камер + InheritView.
//      Вызовы из telescope.c Telescope_Request (press/release CTRL = TelescopeIn/TelescopeOut).
void QuickSpyglass_Enter()
{
	SeaDeckCamera.QuickSpyReturn = 1; // пометка: вошли быстрым режимом из 3-го лица -> на выходе вернуть 3-е лицо
	SeaCameras.Camera = "SeaDeckCamera";
	Sailors.IsOnDeck = 0;
	Crosshair.OutsideCamera = false;
	SeaCameras_UpdateCamera(); // бленд 3->1 лицо + InheritView (направление сохраняется)
	// опция: смотреть с марса (ближайшая целая мачта выбирается в движке deck_camera.cpp)
	bool bMars = false;
	if (CheckAttribute(&InterfaceStates, "SpyglassFromMars"))
	{
		if (sti(InterfaceStates.SpyglassFromMars) != 0) bMars = true;
	}
	if (bMars)
	{
		int intact = GetShipMastsQuantity(pchar) - GetShipFallMastsQuantity(pchar);
		if (intact > 0) SeaDeckCamera.MarsMode = 1; // если мачт нет — остаёмся на палубе (откат)
	}
	SendMessage(&objISpyGlass, "ll", MSG_TELESCOPE_REQUEST, 1); // поднять трубу
}

void QuickSpyglass_Exit()
{
	if (!CheckAttribute(&SeaDeckCamera, "QuickSpyReturn")) return;
	if (sti(SeaDeckCamera.QuickSpyReturn) == 0) return;
	DeleteAttribute(&SeaDeckCamera, "QuickSpyReturn");
	if (CheckAttribute(&SeaDeckCamera, "MarsMode")) SeaDeckCamera.MarsMode = 0;
	SeaCameras.Camera = "SeaShipCamera";
	Sailors.IsOnDeck = 0;
	Crosshair.OutsideCamera = true;
	SeaCameras_UpdateCamera(); // бленд 1->3 лицо + InheritView (направление сохраняется)
}

// LEO: авто-выход из Марса — движок прислал evntMarsAutoExit (целых мачт не осталось, последнюю сбили)
void Sea_MarsAutoExit()
{
	if (CheckAttribute(&SeaDeckCamera, "MarsMode")) SeaDeckCamera.MarsMode = 0;
	Log_SetStringToLog("Мачта под вами рухнула — вы снова на палубе");
	PlaySound("interface\knock.wav");
}

// === Тактическая пауза (вид сверху) =========================================
void Sea_ToggleTacticalView()
{
	if (!CheckAttribute(&SeaShipCamera, "TacSave")) { Sea_TacticalViewOn(); }
	else                                            { Sea_TacticalViewOff(); }
}

// Дальность стрельбы корабля по ТЕКУЩЕМУ заряду (для кольца дальности в тактике): мапим
// загруженный заряд Ship.Cannons.Charge.Type -> строку для GetShootDistance.
// Тип int — как у GetShootDistance (makeint): float-объявление давало RUNTIME ERROR
// «...function return integer value» (Storm НЕ приводит int->float в return, compiler.cpp:4503).
int Sea_TacShipFireDist(int idx)
{
	string sCharge = "ball";
	if (CheckAttribute(&Characters[idx], "Ship.Cannons.Charge.Type"))
	{
		int ct = sti(Characters[idx].Ship.Cannons.Charge.Type);
		if (ct == GOOD_GRAPES)        sCharge = "grape";
		else if (ct == GOOD_KNIPPELS) sCharge = "knippel";
		else if (ct == GOOD_BOMBS)    sCharge = "bomb";
	}
	return GetShootDistance(&Characters[idx], sCharge);
}

// Готовность корабля-компаньона к брандеру: перк «Брандер» доступен (ГГ ИЛИ его боцман, с учётом
// кулдауна — та же проверка GetOfficersPerkUsing, что гейтит кнопку BI_Brander в боевом интерфейсе)
// И корабль расходуемый (GetRemovable — не квестовый/незаменимый). Камера читает результат как
// Characters[i].TacBranderReady и серит/активирует пункт «Брандер».
bool Sea_TacBranderReady(int shipIdx)
{
	if (shipIdx < 0) return false;
	if (!GetOfficersPerkUsing(pchar, "Brander")) return false;
	return GetRemovable(GetCharacter(shipIdx));
}

void Sea_TacticalViewOn()
{
	if (CheckAttribute(&SeaShipCamera, "TacSave")) return;
	if (!bSeaActive || bAbordageStarted) return;
	// LEO: из режима "Марс" в тактику — сначала выйти из Марса (вернуться с мачты на палубу)
	if (CheckAttribute(&SeaDeckCamera, "MarsMode"))
	{
		if (sti(SeaDeckCamera.MarsMode) != 0) SeaDeckCamera.MarsMode = 0;
	}

	// 1) Заморозить бой
	pchar.pause = true;
	SetTimeScale(TACVIEW_PAUSE_TIMESCALE); // slow-mo вместо жёсткой заморозки (см. #define)
	TimeScaleCounter = -3;
	if (!IsPerkIntoList("TimeSpeed")) { AddPerkToActiveList("TimeSpeed"); }

	// 2) Принудительно обзорная (наружная) камера от своего корабля
	if (SeaCameras.Camera != "SeaShipCamera")
	{
		SeaCameras.Camera = "SeaShipCamera";
		Sailors.IsOnDeck = 0;
		SeaCameras_UpdateCamera();
	}
	Crosshair.OutsideCamera = true;

	// 3) Сохранить параметры камеры и задать вид сверху.
	//    Движок пер-кадрово клампит текущий наклон в [MinAngleX, MaxAngleX],
	//    поэтому сужение диапазона тут же задирает камеру вниз. Distance=1.0 —
	//    максимальное отдаление, а MaxCamHeight поднимает потолок высоты,
	//    чтобы камера ушла высоко и было видно всю свалку, а не только свой борт.
	SeaShipCamera.TacSave.MaxAngleX = SeaShipCamera.MaxAngleX;
	SeaShipCamera.TacSave.MinAngleX = SeaShipCamera.MinAngleX;
	SeaShipCamera.TacSave.Distance  = SeaShipCamera.Distance;
	if (CheckAttribute(&SeaShipCamera, "MaxCamHeight")) SeaShipCamera.TacSave.MaxCamHeight = SeaShipCamera.MaxCamHeight;
	else                                                SeaShipCamera.TacSave.MaxCamHeight = TACVIEW_HEIGHT_DEF;

	SeaShipCamera.MaxAngleX   = TACVIEW_ANGLE_MAX;
	SeaShipCamera.MinAngleX   = TACVIEW_ANGLE_MIN;
	SeaShipCamera.Distance    = TACVIEW_DIST;
	SeaShipCamera.MaxCamHeight = TACVIEW_HEIGHT;

	// 3b) Пан: центр орбиты камеры = позиция своего корабля, включить override центра.
	//     Дальше пан двигает этот центр (Sea_TacticalPan), зум — высоту (Sea_TacticalZoom).
	SeaShipCamera.TacCenterX = stf(pchar.Ship.Pos.x);
	SeaShipCamera.TacCenterY = 0.0;
	SeaShipCamera.TacCenterZ = stf(pchar.Ship.Pos.z);
	SeaShipCamera.TacFollow = 0; // каждая сессия тактики стартует БЕЗ фиксации на ГГ (X включит)
	SeaShipCamera.TacStartBehindShip = 1;
	SeaShipCamera.TacCenterOn = 1;

	// выгрузить 3D-позиции вражеских пушек форта в атрибуты камеры (для подсветки ближайшей к курсору).
	// Форты не сущности — камера сама их не видит; движок (sea_ai) пишет позиции в &SeaShipCamera.
	SendMessage(&AISea, "laa", AI_MESSAGE_DUMP_FORT_CANNONS, &Characters[nMainCharacterIndex], &SeaShipCamera);

	// запретить переключение камер, чтобы не сломать обзор
	bCanSwitchCameras = false;

	// 4) Метки над всеми кораблями: имя/тип + свой/чужой + действие (Вариант B, Стадия 1)
	Sea_TacticalShipLabels(true);

	// 4b) Пометить компаньонов «мой корабль» — для линий приказов (движок читает их AI-задачу).
	int tci;
	for (tci = 1; tci < COMPANION_MAX; tci++)
	{
		int tcc = GetCompanionIndex(pchar, tci);
		if (tcc >= 0)
		{
			Characters[tcc].IsMyTacShip = 1;
			Characters[tcc].TacFireDist = Sea_TacShipFireDist(tcc); // радиус кольца дальности по текущему заряду
			Characters[tcc].TacBranderReady = 0; // активирует/серит пункт «Брандер» в меню
			if (Sea_TacBranderReady(tcc)) Characters[tcc].TacBranderReady = 1;
		}
	}
	// 4c) Пометить вражеские корабли «враг» — движок по ПКМ на них открывает контекстное меню.
	int tsi;
	for (tsi = 0; tsi < MAX_SHIPS_ON_SEA; tsi++)
	{
		if (Ships[tsi] <= 0) continue;
		if (GetRelation(sti(pchar.index), Ships[tsi]) == RELATION_ENEMY)
			Characters[Ships[tsi]].IsEnemyTacShip = 1;
	}

	// 5) Приказ по прицелу (Стадия 3, экспериментально): движок шлёт evntTacOrderAttack
	//    с индексом корабля под центром обзора, обработчик отдаёт приказ компаньонам.
	SetEventHandler("evntTacOrderAttack", "Sea_TacOrderAttackHandler", 0);
	SetEventHandler("evntTacOrderMove", "Sea_TacOrderMoveHandler", 0);
	SetEventHandler("evntTacOrderDefend", "Sea_TacOrderDefendHandler", 0);
	SetEventHandler("evntTacOrderRunaway", "Sea_TacOrderRunawayHandler", 0);
	// Курсорный приказ (Этап 2): движок шлёт выбранный корабль + цель/точку под курсором.
	SetEventHandler("evntTacCmd", "Sea_TacCmdHandler", 0);
	// Контекстное меню по ПКМ на враге: движок шлёт (selIdx, tgtIdx, action) — атака/абордаж/заряд.
	SetEventHandler("evntTacMenuCmd", "Sea_TacMenuCmdHandler", 0);
	// Залп бортом ГГ по ЛКМ: движок шлёт evntTacFire при клике по пустой воде/врагу,
	// когда не выбран ни один союзник (см. Sea_TacFireHandler — стреляем только при снятой паузе).
	SetEventHandler("evntTacFire", "Sea_TacFireHandler", 0);

	PlaySound("interface\ok.wav");
}

void Sea_TacticalViewOff()
{
	// вернуть параметры камеры
	if (CheckAttribute(&SeaShipCamera, "TacSave"))
	{
		SeaShipCamera.MaxAngleX   = SeaShipCamera.TacSave.MaxAngleX;
		SeaShipCamera.MinAngleX   = SeaShipCamera.TacSave.MinAngleX;
		SeaShipCamera.Distance    = SeaShipCamera.TacSave.Distance;
		SeaShipCamera.MaxCamHeight = SeaShipCamera.TacSave.MaxCamHeight;
		DeleteAttribute(&SeaShipCamera, "TacSave");
	}
	// выключить override центра (вернуть камеру на свой корабль)
	SeaShipCamera.TacFollow = 0; // снять фиксацию на ГГ
	SeaShipCamera.TacCenterOn = 0;
	bCanSwitchCameras = true;

	// убрать тактические метки и вернуть отображение к настройкам игрока
	Sea_TacticalShipLabels(false);
	ShipShowName();
	DelEventHandler("evntTacOrderAttack", "Sea_TacOrderAttackHandler");
	DelEventHandler("evntTacOrderMove", "Sea_TacOrderMoveHandler");
	DelEventHandler("evntTacOrderDefend", "Sea_TacOrderDefendHandler");
	DelEventHandler("evntTacOrderRunaway", "Sea_TacOrderRunawayHandler");
	DelEventHandler("evntTacCmd", "Sea_TacCmdHandler");
	DelEventHandler("evntTacMenuCmd", "Sea_TacMenuCmdHandler");
	DelEventHandler("evntTacFire", "Sea_TacFireHandler");
	// снять подсветку цели (если была)
	if (CheckAttribute(&objShipPointer, "textures"))
		SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, -1, 0, 0.005, 1.0, 1.0, 1.0, 5.0);
	// снять флаг «мой корабль» и метки приказов с компаньонов (линии приказов)
	int ci;
	for (ci = 1; ci < COMPANION_MAX; ci++)
	{
		int cci = GetCompanionIndex(pchar, ci);
		if (cci >= 0)
		{
			DeleteAttribute(&Characters[cci], "IsMyTacShip");
			DeleteAttribute(&Characters[cci], "TacOrderTarget");
			DeleteAttribute(&Characters[cci], "TacFireDist");
			DeleteAttribute(&Characters[cci], "TacBranderReady");
		}
	}
	// снять флаг «враг» и тинт-подсветку со всех кораблей боя
	int csi;
	for (csi = 0; csi < MAX_SHIPS_ON_SEA; csi++)
	{
		if (Ships[csi] <= 0) continue;
		DeleteAttribute(&Characters[Ships[csi]], "IsEnemyTacShip");
		DeleteAttribute(&Characters[Ships[csi]], "TacTint"); // снять тинт корпуса (его пишет камера в тактике)
	}

	// снять паузу
	DeleteAttribute(pchar, "pause");
	SetTimeScale(1.0);
	TimeScaleCounter = 0;
	if (IsPerkIntoList("TimeSpeed")) { DelPerkFromActiveList("TimeSpeed"); }

	PlaySound("interface\ok.wav");
}

// LEO: ПРОБЕЛ в тактическом виде - поставить/снять паузу (TimeScale 0 <-> 1), НЕ выходя из тактики.
// Зеркалит заморозку из Sea_TacticalViewOn и снятие паузы из Sea_TacticalViewOff.
void Sea_ToggleTacticalPause()
{
	if (!Sea_InTacticalView()) return;
	if (CheckAttribute(pchar, "pause"))
	{
		// снять паузу - бой пойдёт с видом сверху
		DeleteAttribute(pchar, "pause");
		SetTimeScale(1.0);
		TimeScaleCounter = 0;
		if (IsPerkIntoList("TimeSpeed")) { DelPerkFromActiveList("TimeSpeed"); }
	}
	else
	{
		// поставить паузу - slow-mo вместо жёсткой заморозки (как при входе в тактику)
		pchar.pause = true;
		SetTimeScale(TACVIEW_PAUSE_TIMESCALE);
		TimeScaleCounter = -3;
		if (!IsPerkIntoList("TimeSpeed")) { AddPerkToActiveList("TimeSpeed"); }
	}
}

// Кнопка (тактика): вернуть центр обзора камеры на корабль ГГ после пана стрелками.
// Пишем атрибуты центра — движок подхватит их в AttributeChanged (перебьёт панорамленный vTacCenter).
void Sea_TacticalRecenterSelf()
{
	if (!Sea_InTacticalView()) return;
	// (2) любое нажатие X ВСЕГДА мгновенно центрует камеру на корабле ГГ (как было)
	SeaShipCamera.TacCenterX = stf(pchar.Ship.Pos.x);
	SeaShipCamera.TacCenterZ = stf(pchar.Ship.Pos.z);
	// (1) + переключатель ФИКСАЦИИ: камера прицепляется к кораблю ГГ (как на глобальной карте).
	// Движок ведёт слежение покадрово по атрибуту TacFollow (см. SHIP_CAMERA::Move / AttributeChanged).
	int follow = 0;
	if (CheckAttribute(&SeaShipCamera, "TacFollow")) follow = sti(SeaShipCamera.TacFollow);
	if (follow != 0) SeaShipCamera.TacFollow = 0;
	else SeaShipCamera.TacFollow = 1;
	PlaySound("interface\ok.wav");
}

// Залп бортом ГГ из тактического режима (вид сверху). Движок шлёт evntTacFire по ЛКМ,
// когда клик пришёлся НЕ по своему кораблю и НЕ выбран ни один союзник для команд; в аргументах —
// индекс врага под курсором (или -1; не используем — флаг движка ненадёжен) и точка курсора (x,z).
// Стреляем ТОЛЬКО при снятой паузе: на полной паузе (вход в тактику) — игнор; после
// +/- (TimeScale пошёл, атрибут pause удалён) — обычный бой с видом сверху.
// ПРИЦЕЛИВАНИЕ: клик по/рядом с живым врагом -> прицельный залп с авто-упреждением движка (вариант 2,
// масштаб TAC_LEAD_SCALE); клик по открытой воде -> залп ровно в точку курсора (ручное упреждение).
void Sea_TacFireHandler()
{
	int   tgtIdx = GetEventData(); // индекс врага под курсором от движка — НЕ используем (флаг ненадёжен), цель ищем ниже сами
	float posX   = GetEventData();
	float posZ   = GetEventData();

	if (CheckAttribute(pchar, "pause")) return; // на паузе не стреляем

	// СНАП НА ВРАГА: точка курсора попала в радиус TAC_FIRE_SNAP_R2 от живого врага -> бьём по его
	// ЦЕНТРУ (точное попадание, не нужно целить пиксель-в-пиксель; флаг IsEnemyTacShip движка ненадёжен
	// для поздних врагов — относим к врагам через GetRelation). Иначе (вода/впереди цели) -> точка курсора.
	// ближайший живой враг к точке курсора
	int   ni = -1;
	float nd2 = 1000000000.0;
	int   ii;
	for (ii = 0; ii < MAX_SHIPS_ON_SEA; ii++)
	{
		if (Ships[ii] <= 0) continue;
		int si = Ships[ii];
		if (LAi_IsDead(&Characters[si])) continue;
		if (GetRelation(sti(pchar.index), si) != RELATION_ENEMY) continue;
		float ddx = stf(Characters[si].Ship.Pos.x) - posX;
		float ddz = stf(Characters[si].Ship.Pos.z) - posZ;
		float ed2 = ddx*ddx + ddz*ddz;
		if (ed2 < nd2) { nd2 = ed2; ni = si; }
	}

	if (ni >= 0 && nd2 < TAC_FIRE_SNAP_R2)
	{
		// клик по врагу -> прицельный залп с упреждением. Упреждение считает ДВИЖОК
		// (AIShipCannonController::Fire: множитель ядра + баллистика + TouchMove), множитель упреждения
		// = TAC_LEAD_SCALE (ИИ юзает 2.0 — перелетало; для ГГ откалибровано на 1.0).
		Ship_DoFireAt(ni, 0.0, 0.0, TAC_LEAD_SCALE);
	}
	else
	{
		// клик по открытой воде (далеко от врага) -> залп ровно в точку курсора (ручное упреждение)
		Ship_DoFireAt(-1, posX, posZ, 0.0);
	}
}

// Обработчик приказа по прицелу (Стадия 3, экспериментально). Движок шлёт сюда индекс
// корабля под центром обзора. Если это враг — все живые компаньоны получают приказ атаковать.
void Sea_TacOrderAttackHandler()
{
	float posX = GetEventData();
	float posZ = GetEventData();
	int bFort = GetEventData(); // курсор над пушкой форта -> приказ атаковать форт

	int targetIdx = -1;
	int bAttackFort = 0;
	// Курсор над пушкой форта -> цель = командир форта (валидная цель ship-task: ИИ так и делает,
	// напр. Ship_SetTaskRunaway(...,iFortCommander) в AIShip.c). GetRelation на форте работает.
	if (bFort)
	{
		if (bIsFortAtIsland)
		{
			if (GetRelation(sti(pchar.index), iFortCommander) == RELATION_ENEMY)
			{
				targetIdx = iFortCommander;
				bAttackFort = 1;
			}
		}
	}

	int i;
	int sidx;
	float dx, dz, d2;
	if (!bAttackFort)
	{
		// найти ближайшего ВРАГА-КОРАБЛЯ к точке прицела
		float bestD2 = 1000000000.0;
		for (i = 0; i < MAX_SHIPS_ON_SEA; i++)
		{
			if (Ships[i] <= 0) continue;
			sidx = Ships[i];
			if (LAi_IsDead(&Characters[sidx])) continue;
			if (GetRelation(sti(pchar.index), sidx) != RELATION_ENEMY) continue;
			dx = stf(Characters[sidx].Ship.Pos.x) - posX;
			dz = stf(Characters[sidx].Ship.Pos.z) - posZ;
			d2 = dx*dx + dz*dz;
			if (d2 < bestD2) { bestD2 = d2; targetIdx = sidx; }
		}
	}
	if (targetIdx < 0) { PlaySound("interface\knock.wav"); return; } // рядом нет цели

	// приказ всем живым компаньонам
	int n = 0;
	int cidx;
	for (i = 1; i < COMPANION_MAX; i++) // i=0 — сам ГГ
	{
		cidx = GetCompanionIndex(pchar, i);
		if (cidx < 0) continue;
		if (LAi_IsDead(&Characters[cidx])) continue;
		SeaTowOrders_Cancel(GetCharacter(cidx));
		Ship_SetTaskAttack(SECONDARY_TASK, cidx, targetIdx);
		// LEO: приказ бить ФОРТ -> снять автономию, иначе компаньон сорвётся в авто-кайт/абордаж на форт и полезет вплотную.
		//   Тогда bOrderedCompanion=true -> держит ordered-дистанцию (пушечную, ядрами), не эскалирует в абордаж форта.
		DeleteAttribute(&Characters[cidx], "CompanionAutoEngage"); // LEO: любой прямой приказ атаки снимает автономию (было только для форта -> Redecide сносил форс-заряд под обычным приказом)
		Characters[cidx].TacOrderTarget = targetIdx; // для линии приказа
		Characters[cidx].TacOrderDefend = 0;         // атака — оранжевая линия
		n++;
	}
	if (n > 0)
	{
		Sea_TacticalShipLabels(true); // обновить иконки задач — видно, что приказ принят
		// указатель цели — только для корабля (форт не корабль, ship-pointer на него не наведёшь)
		if (!bAttackFort)
		{
			if (CheckAttribute(&objShipPointer, "textures"))
				SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, targetIdx, 0, 0.005, 1.0, 1.0, 1.0, 5.0);
		}
		PlayVoice("INTERFACE\SignalCompanion.wav");
	}
	else PlaySound("interface\knock.wav"); // нет компаньонов
}

// Обработчик приказа «плыть к точке прицела» (Стадия 3, экспериментально). Движок шлёт
// координаты центра обзора; все живые компаньоны получают приказ плыть туда.
void Sea_TacOrderMoveHandler()
{
	float posX = GetEventData();
	float posZ = GetEventData();
	int n = 0;
	int i;
	for (i = 1; i < COMPANION_MAX; i++) // i=0 — сам ГГ
	{
		int cidx = GetCompanionIndex(pchar, i);
		if (cidx < 0) continue;
		if (LAi_IsDead(&Characters[cidx])) continue;
		SeaTowOrders_Cancel(GetCharacter(cidx));
		Ship_SetTaskMove(SECONDARY_TASK, cidx, posX, posZ);
		DeleteAttribute(&Characters[cidx], "TacOrderTarget"); // move — без линии (есть зелёный маркер)
		n++;
	}
	if (n > 0)
	{
		Sea_TacticalShipLabels(true); // обновить иконки задач
		PlayVoice("INTERFACE\SignalCompanion.wav");
	}
	else PlaySound("interface\knock.wav"); // нет компаньонов
}

// Приказ «защищать» (Стадия 3): ближайший к прицелу СОЮЗНИК, компаньоны его прикрывают.
void Sea_TacOrderDefendHandler()
{
	float posX = GetEventData();
	float posZ = GetEventData();
	int targetIdx = -1;
	float bestD2 = 1000000000.0;
	int i;
	int sidx;
	float dx, dz, d2;
	for (i = 0; i < MAX_SHIPS_ON_SEA; i++)
	{
		if (Ships[i] <= 0) continue;
		sidx = Ships[i];
		if (LAi_IsDead(&Characters[sidx])) continue;
		if (GetRelation(sti(pchar.index), sidx) == RELATION_ENEMY) continue; // только не-враги
		dx = stf(Characters[sidx].Ship.Pos.x) - posX;
		dz = stf(Characters[sidx].Ship.Pos.z) - posZ;
		d2 = dx*dx + dz*dz;
		if (d2 < bestD2) { bestD2 = d2; targetIdx = sidx; }
	}
	if (targetIdx < 0) { PlaySound("interface\knock.wav"); return; }
	int n = 0;
	int cidx;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cidx = GetCompanionIndex(pchar, i);
		if (cidx < 0) continue;
		if (LAi_IsDead(&Characters[cidx])) continue;
		if (cidx == targetIdx) continue; // не назначать защищать сам себя
		SeaTowOrders_Cancel(GetCharacter(cidx));
		Ship_SetTaskDefend(SECONDARY_TASK, cidx, targetIdx);
		Characters[cidx].TacOrderTarget = targetIdx; // для линии приказа (к защищаемому)
		Characters[cidx].TacOrderDefend = 1;         // защита — голубая линия
		n++;
	}
	if (n > 0)
	{
		Sea_TacticalShipLabels(true);
		// подсветить защищаемого союзника указателем (как своего)
		if (CheckAttribute(&objShipPointer, "textures"))
			SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, targetIdx, 1, 0.005, 1.0, 1.0, 1.0, 5.0);
		PlayVoice("INTERFACE\SignalCompanion.wav");
	}
	else PlaySound("interface\knock.wav");
}

// Приказ «отступать» (Стадия 3): компаньоны уходят от ближайшего к ГГ врага.
void Sea_TacOrderRunawayHandler()
{
	int dummy = GetEventData();
	int enemyIdx = -1;
	float bestD2 = 1000000000.0;
	float px = stf(pchar.Ship.Pos.x);
	float pz = stf(pchar.Ship.Pos.z);
	int i;
	int sidx;
	float dx, dz, d2;
	for (i = 0; i < MAX_SHIPS_ON_SEA; i++)
	{
		if (Ships[i] <= 0) continue;
		sidx = Ships[i];
		if (LAi_IsDead(&Characters[sidx])) continue;
		if (GetRelation(sti(pchar.index), sidx) != RELATION_ENEMY) continue;
		dx = stf(Characters[sidx].Ship.Pos.x) - px;
		dz = stf(Characters[sidx].Ship.Pos.z) - pz;
		d2 = dx*dx + dz*dz;
		if (d2 < bestD2) { bestD2 = d2; enemyIdx = sidx; }
	}
	if (enemyIdx < 0) { PlaySound("interface\knock.wav"); return; }
	int n = 0;
	int cidx;
	for (i = 1; i < COMPANION_MAX; i++)
	{
		cidx = GetCompanionIndex(pchar, i);
		if (cidx < 0) continue;
		if (LAi_IsDead(&Characters[cidx])) continue;
		SeaTowOrders_Cancel(GetCharacter(cidx));
		Ship_SetTaskRunAway(SECONDARY_TASK, cidx, enemyIdx);
		DeleteAttribute(&Characters[cidx], "TacOrderTarget"); // побег — без линии
		n++;
	}
	if (n > 0)
	{
		Sea_TacticalShipLabels(true);
		PlayVoice("INTERFACE\SignalCompanion.wav");
	}
	else PlaySound("interface\knock.wav");
}

// LEO: Один вызов на выбранный корабль; bFirst начинает общий приказ по цели или точке моря.
void Sea_TacCmdHandler()
{
	int selIdx = GetEventData();
	int tgtIdx = GetEventData();
	float posX = GetEventData();
	float posZ = GetEventData();
	int bFirst = GetEventData(); // 1 у первого корабля группы (групповой приказ) — голос/звук играем один раз
	int bFort = GetEventData();  // курсор над пушкой форта -> выбранный корабль атакует форт
	if (bFirst) DeleteAttribute(&SeaTowOrders, "StrategyHandledTarget");
	// Вид приказа кладём в SeaShipCamera.TacCmdKind (1 атака/2 защита/3 плыть/0 неуд.) — движок
	// читает его сразу после core.Event и рисует верную подпись (раньше угадывал по IsMyTacShip
	// и ошибался на союзниках-некомпаньонах).
	if (selIdx < 0 || selIdx >= TOTAL_CHARACTERS || tgtIdx >= TOTAL_CHARACTERS) { SeaShipCamera.TacCmdKind = 0; if (bFirst) PlaySound("interface\knock.wav"); return; }
	if (LAi_IsDead(&Characters[selIdx])) { SeaShipCamera.TacCmdKind = 0; if (bFirst) PlaySound("interface\knock.wav"); return; }
	// LEO: Свой приз отцепляется, свободный берётся на буксир; чужой приз не заменяет прежний приказ.
	if (!bFort && tgtIdx >= 0)
	{
		if (CheckAttribute(&Characters[tgtIdx], "id") && HasSubStr(Characters[tgtIdx].id, "_DriftCap_"))
		{
			SeaShipCamera.TacCmdKind = 0;
			// LEO: Принятый зацеп/отцеп выполняется один раз на группу, а не переключается следующим кораблём.
			if (CheckAttribute(&SeaTowOrders, "StrategyHandledTarget"))
			{
				if (sti(SeaTowOrders.StrategyHandledTarget) == tgtIdx) return;
			}
			bool towAccepted = SeaTowOrders_Toggle(GetCharacter(selIdx), GetCharacter(tgtIdx));
			if (towAccepted) SeaTowOrders.StrategyHandledTarget = tgtIdx;
			if (towAccepted) Sea_TacticalShipLabels(true);
			if (bFirst)
			{
				if (towAccepted) { Log_SetStringToLog(XI_ConvertString("SeaTowOrder")); PlayVoice("INTERFACE\SignalCompanion.wav"); }
				else PlaySound("interface\knock.wav");
			}
			return;
		}
	}
	// LEO: любой прямой приказ (атака/защита/движение/форт) снимает автономию компаньона — иначе Redecide сносит форс-заряд/авто-ретаргетит под приказом.
	DeleteAttribute(&Characters[selIdx], "CompanionAutoEngage");

	// курсор над пушкой форта -> выбранный корабль атакует ФОРТ (командир форта — валидная ship-task цель)
	if (bFort)
	{
		int bFortOk = 0;
		if (bIsFortAtIsland)
		{
			if (GetRelation(sti(pchar.index), iFortCommander) == RELATION_ENEMY) bFortOk = 1;
		}
		if (bFortOk)
		{
			SeaTowOrders_Cancel(GetCharacter(selIdx));
			Ship_SetTaskAttack(SECONDARY_TASK, selIdx, iFortCommander);
			// LEO: приказ бить ФОРТ -> снять автономию, иначе компаньон полезет к форту вплотную (авто-кайт/абордаж).
			DeleteAttribute(&Characters[selIdx], "CompanionAutoEngage");
			Characters[selIdx].TacOrderTarget = iFortCommander;
			Characters[selIdx].TacOrderDefend = 0;
			SeaShipCamera.TacCmdKind = 1;
			Sea_TacticalShipLabels(true);
			if (bFirst) PlayVoice("INTERFACE\SignalCompanion.wav");
			return;
		}
	}

	if (tgtIdx >= 0)
	{
		if (GetRelation(sti(pchar.index), tgtIdx) == RELATION_ENEMY)
		{
			SeaTowOrders_Cancel(GetCharacter(selIdx));
			Ship_SetTaskAttack(SECONDARY_TASK, selIdx, tgtIdx);
			Characters[selIdx].TacOrderTarget = tgtIdx;
			Characters[selIdx].TacOrderDefend = 0;
			if (CheckAttribute(&objShipPointer, "textures"))
				SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, tgtIdx, 0, 0.005, 1.0, 1.0, 1.0, 5.0);
			SeaShipCamera.TacCmdKind = 1;
		}
		else
		{
			if (selIdx == tgtIdx) { SeaShipCamera.TacCmdKind = 0; if (bFirst) PlaySound("interface\knock.wav"); return; } // не защищать сам себя
			SeaTowOrders_Cancel(GetCharacter(selIdx));
			Ship_SetTaskDefend(SECONDARY_TASK, selIdx, tgtIdx);
			Characters[selIdx].TacOrderTarget = tgtIdx;
			Characters[selIdx].TacOrderDefend = 1;
			if (CheckAttribute(&objShipPointer, "textures"))
				SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, tgtIdx, 1, 0.005, 1.0, 1.0, 1.0, 5.0);
			SeaShipCamera.TacCmdKind = 2;
		}
	}
	else
	{
		SeaTowOrders_Cancel(GetCharacter(selIdx));
		Ship_SetTaskMove(SECONDARY_TASK, selIdx, posX, posZ);
		DeleteAttribute(&Characters[selIdx], "TacOrderTarget");
		SeaShipCamera.TacCmdKind = 3;
	}
	Sea_TacticalShipLabels(true);
	if (bFirst) PlayVoice("INTERFACE\SignalCompanion.wav");
}

// Контекстное меню по ПКМ на враге (движок шлёт selIdx, tgtIdx, action):
// action 0 атака / 1 абордаж / 2 ядра / 3 картечь / 4 книппели / 5 бомбы / 6 брандер. Приказ — ВЫБРАННОМУ кораблю.
void Sea_TacMenuCmdHandler()
{
	int selIdx = GetEventData();
	int tgtIdx = GetEventData();
	int action = GetEventData();
	int bFirst = GetEventData(); // 1 у первого корабля группы — голос/звук один раз
	if (selIdx < 0 || selIdx >= TOTAL_CHARACTERS || tgtIdx < 0 || tgtIdx >= TOTAL_CHARACTERS)
	{
		if (bFirst) PlaySound("interface\knock.wav");
		return;
	}
	if (LAi_IsDead(&Characters[selIdx]) || LAi_IsDead(&Characters[tgtIdx]))
	{
		if (bFirst) PlaySound("interface\knock.wav");
		return;
	}
	// Прямой приказ снимает автономию, чтобы ИИ не менял цель и выбранный заряд.
	DeleteAttribute(&Characters[selIdx], "CompanionAutoEngage");
	if (action == 1) // абордаж
	{
		SeaTowOrders_Cancel(GetCharacter(selIdx));
		Ship_SetTaskAbordage(SECONDARY_TASK, selIdx, tgtIdx);
		Characters[selIdx].TacOrderTarget = tgtIdx;
		Characters[selIdx].TacOrderDefend = 0;
	}
	else if (action == 6) // брандер — выбранный компаньон идёт на таран и подрывается (как кнопка BI_Brander)
	{
		if (!Sea_TacBranderReady(selIdx)) // живая перепроверка: перк/кулдаун/расходуемость могли смениться
		{
			if (bFirst) PlaySound("interface\knock.wav");
			return;
		}
		SeaTowOrders_Cancel(GetCharacter(selIdx));
		Ship_SetTaskBrander(SECONDARY_TASK, selIdx, tgtIdx);
		Characters[selIdx].TacOrderTarget = tgtIdx;
		Characters[selIdx].TacOrderDefend = 0;
	}
	else // атака (0) либо атака с выбором заряда (2..5)
	{
		SeaTowOrders_Cancel(GetCharacter(selIdx));
		Ship_SetTaskAttack(SECONDARY_TASK, selIdx, tgtIdx);
		Characters[selIdx].TacOrderTarget = tgtIdx;
		Characters[selIdx].TacOrderDefend = 0;
		// заряд: приказ «Атака» очищает ShipCannonChargeType, поэтому ставим его ПОСЛЕ
		int charge = -1;
		if (action == 2) charge = GOOD_BALLS;
		else if (action == 3) charge = GOOD_GRAPES;
		else if (action == 4) charge = GOOD_KNIPPELS;
		else if (action == 5) charge = GOOD_BOMBS;
		if (charge >= 0)
		{
			Ship_ChangeCharge(&Characters[selIdx], charge);
			Characters[selIdx].ShipCannonChargeType = charge; // офицеры палят этим до след. приказа/конца снарядов
		}
	}
	Characters[selIdx].TacFireDist = Sea_TacShipFireDist(selIdx); // заряд мог смениться — обновить кольцо дальности
	// подсветить цель указателем (как врага) + обновить иконки задач + голос
	if (CheckAttribute(&objShipPointer, "textures"))
		SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, tgtIdx, 0, 0.005, 1.0, 1.0, 1.0, 5.0);
	Sea_TacticalShipLabels(true);
	if (bFirst) PlayVoice("INTERFACE\SignalCompanion.wav");
}

// Признак «мы в тактическом виде» — для гейтинга стрельбы по ЛКМ (seadogs.c).
bool Sea_InTacticalView()
{
	return CheckAttribute(&SeaShipCamera, "TacSave");
}

// Пан тактической камеры: сдвиг центра орбиты в мировых X/Z (шаг растёт с высотой).
// Зовётся из ProcessControls по клавишам-стрелкам, только когда мы в тактическом виде.
void Sea_TacticalPan(float dx, float dz)
{
	if (!CheckAttribute(&SeaShipCamera, "TacSave")) return; // только в тактическом виде
	float fStep = stf(SeaShipCamera.MaxCamHeight) * TACVIEW_PAN_FACTOR;
	SeaShipCamera.TacCenterX = stf(SeaShipCamera.TacCenterX) + dx * fStep;
	SeaShipCamera.TacCenterZ = stf(SeaShipCamera.TacCenterZ) + dz * fStep;
}

// Зум тактической камеры: dir = +1 приблизить (ниже), -1 отдалить (выше). Меняет MaxCamHeight.
void Sea_TacticalZoom(float dir)
{
	if (!CheckAttribute(&SeaShipCamera, "TacSave")) return;
	float h = stf(SeaShipCamera.MaxCamHeight) - dir * TACVIEW_ZOOM_STEP;
	if (h < TACVIEW_HEIGHT_MIN) h = TACVIEW_HEIGHT_MIN;
	if (h > TACVIEW_HEIGHT_MAX) h = TACVIEW_HEIGHT_MAX;
	SeaShipCamera.MaxCamHeight = h;
}
