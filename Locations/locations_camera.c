#define LOCCAMERA_FOLLOW	1
#define LOCCAMERA_TOPOS		2
#define LOCCAMERA_FREE		3
//#20190815-01
#define DEFAULT_CAM_RAD 2.0
#define DEFAULT_CAM_RAD_DEN 0.22
#define DEFAULT_LCAM_PERSP 1.285
#define DEFAULT_LCAM_PERSP_DEN 1.25

// Warship 20.07.09 Новое
#define LOCCAMERA_MAX_STATES 15
#define LOCCAMERA_ROTATE "Rotate"
#define LOCCAMERA_FLYTOPOS "FlyToPosition"
#define LOCCAMERA_NEARHERO "LockNearHero"

// Gregg: виды наземной камеры, циклятся по TAB (контрол ChrCamCameraSwitch).
// Смена плеча - отдельный контрол ChrCamShoulderSwap; из любого другого вида он же включает вид от плеча.
// Значения обязаны совпадать с enum CameraViewMode в движке (location_camera.h).
#define LOCCAMERA_VIEW_BACK			0
#define LOCCAMERA_VIEW_SHOULDER		1
#define LOCCAMERA_VIEW_FIRST		2

// Gregg: настройки вида от плеча. Крутить здесь - в экран опций пока не вынесены.
// SH_OFFSET     - боковой вынос камеры и точки взгляда, м. Больше = персонаж дальше от центра кадра.
// SH_HEIGHT     - подъём камеры и точки взгляда над обычной высотой, м.
// SH_AIM        - доворот кадра: дополнительный боковой сдвиг ОДНОЙ точки взгляда, м. Плюс - персонаж
//                 ближе к краю кадра, минус - камера подворачивается обратно на него, 0 - строго параллельно.
// SH_RADIUS     - множитель дистанции до персонажа. Базовую дистанцию по-прежнему задают опции и колёсико,
//                 режим только домножает её, поэтому колёсико продолжает работать.
// SH_FOV        - множитель угла обзора. Меньше единицы = уже угол, ближе к «прицельному» кадру.
// SH_ROTINERTIA - инерция доворота камеры за персонажем. Из-за спины она 5.0; сбоку задержка заметнее,
//                 поэтому доворот делаем быстрее. 0 = оставить как из-за спины.
#define SH_OFFSET		0.8
#define SH_HEIGHT		0.1
#define SH_AIM			0.15
#define SH_RADIUS		0.8
#define SH_FOV			0.92
#define SH_ROTINERTIA	9.0

#event_handler("frame", "locCameraUpdate");

Object objLocCameraStates[LOCCAMERA_MAX_STATES];
int iLocCameraCurState = -1;
int locCameraCurMode;
bool locCameraEnableSpecialMode;
bool locCameraEnableFree;

void locCameraInit()
{
	locCameraEnableFree = false;
	locCameraEnableSpecialMode = false;
	locCameraCurMode = LOCCAMERA_FOLLOW;
}

// LEO: геттер состояния скриптовой камеры (катсцены) для чтения из ДРУГИХ файлов — глобал iLocCameraCurState
// кросс-файл в Storm ненадёжен (зависит от порядка include), а функция глобальна и безопасна.
// -1 = катсцены нет; >=0 идёт кинематографичная камера (пан/облёт/фикс на герое).
int LocCamera_GetCurState()
{
	return iLocCameraCurState;
}

//Set camera follow mode
bool locCameraFollow()
{
	if(IsEntity(&locCamera) == 0) return false;
	bool res = SendMessage(&locCamera, "l", MSG_CAMERA_FOLLOW);
	locCameraCurMode = LOCCAMERA_FOLLOW;
	return res;
}

// Gregg: настройки вида от плеча -> в движок плюс восстановление запомненного вида.
// Зовётся сразу после создания камеры локации: камера пересоздаётся на КАЖДОМ переходе, и без этого
// выбранный игроком вид сбрасывался бы на «из-за спины» в каждой двери.
void locCameraApplySettings()
{
	if(IsEntity(&locCamera) == 0) return;
	SendMessage(&locCamera, "lffffff", -5, SH_OFFSET, SH_HEIGHT, SH_AIM, SH_RADIUS, SH_FOV, SH_ROTINERTIA);

	int iView = LOCCAMERA_VIEW_BACK;
	float fSide = 1.0;
	if(CheckAttribute(&InterfaceStates, "LandCamView")) iView = sti(InterfaceStates.LandCamView);
	if(CheckAttribute(&InterfaceStates, "LandCamSide")) fSide = stf(InterfaceStates.LandCamSide);
	SendMessage(&locCamera, "llf", -6, iView, fSide);
}

// Gregg: запомнить выбранный вид перед выгрузкой локации. Атрибуты пишет движок при каждом
// переключении, а InterfaceStates переживает переход и уходит в сейв.
void locCameraStoreView()
{
	if(IsEntity(&locCamera) == 0) return;
	if(CheckAttribute(&locCamera, "viewmode")) InterfaceStates.LandCamView = sti(locCamera.viewmode);
	if(CheckAttribute(&locCamera, "shoulderside")) InterfaceStates.LandCamSide = stf(locCamera.shoulderside);
}

void locCameraSelectCabin(ref loc)
{
	if(!IsEntity(&locCamera)) return;
	if(!CheckAttribute(loc, "locators.camera.reload1.x") || !CheckAttribute(loc, "locators.camera.reload1.y")
		|| !CheckAttribute(loc, "locators.camera.reload1.z")
		|| !CheckAttribute(loc, "locators.goto.SelectCharacter.x") || !CheckAttribute(loc, "locators.goto.SelectCharacter.y")
		|| !CheckAttribute(loc, "locators.goto.SelectCharacter.z")) return;
	aref from, to;
	makearef(from, loc.locators.camera.reload1);
	makearef(to, loc.locators.goto.SelectCharacter);
	SendMessage(&locCamera, "lffffff", MSG_CAMERA_FIXED_SHOT,
		stf(from.x) + 0.5, stf(from.y) - 0.5, stf(from.z) + 0.5,
		stf(to.x), stf(to.y) + 1.44, stf(to.z));
	locCameraCurMode = LOCCAMERA_TOPOS;
}

//Set camera toPos mode
bool locCameraToPos(float x, float y, float z, bool isTeleport)
{
	if(IsEntity(&locCamera) == 0) return false;
	if(locCameraEnableFree == true) return true;
	bool res = SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, x, y, z, isTeleport);
	locCameraCurMode = LOCCAMERA_TOPOS;
	return res;
}

//Set camera move mode (speed meter per second)
bool locCameraFree()
{
	if(IsEntity(&locCamera) == 0) return false;
	bool res = SendMessage(&locCamera, "l", MSG_CAMERA_FREE);
	locCameraCurMode = LOCCAMERA_FREE;
	return res;
}

bool locCameraLock(float ax)
{
	if(IsEntity(&locCamera) == 0) return false;
	bool res = SendMessage(&locCamera, "lf", MSG_CAMERA_MOVE, ax);
	return res;
}

void locCameraSleep(bool isSleep)
{
	SendMessage(&locCamera, "ll", MSG_CAMERA_SLEEP, isSleep);
}

void locCameraSwitch()
{
	if(locCameraEnableFree == false) return;
	string controlName = GetEventData();
	if(controlName != "ChrCamCameraSwitch") return;
	if(locCameraCurMode != LOCCAMERA_FREE)
	{
		locCameraFree();
	}else{
		locCameraFollow();
	}
}

void locCameraSetSpecialMode(bool isEnable)
{
	locCameraEnableSpecialMode = isEnable;
}

#event_handler("EventGetSpecialMode", "locCameraGetSpecialMode");
int locCameraGetSpecialMode()
{
	return false;
}
void LoadTrackCamera(string sTrackName,float fTrackTime,aref arTrackPause)
{
	SendMessage(&locCamera, "lsfa", -2, sTrackName, fTrackTime, arTrackPause);
}

void TurnOffTrackCamera()
{
	SendMessage(&locCamera, "l", -3);
}
// boal -->
void locCameraTarget(ref _char)
{
    SendMessage(&locCamera, "li", MSG_CAMERA_SETTARGET, _char);
}
// boal <--
// to_do нет в ядре
bool locCameraFromToPos(float from_x,float from_y,float from_z, bool isTeleport, float to_x,float to_y,float to_z)
{
	if(IsEntity(&locCamera) == 0)
	{
		return false;
	}
	if(locCameraEnableFree == true)
	{
		return true;
	}
	bool res = SendMessage(&locCamera, "lffflfff", -4, from_x,from_y,from_z, isTeleport, to_x,to_y,to_z);
	locCameraCurMode = LOCCAMERA_TOPOS;
	return res;
}
//#20190815-01
void SetPerspective2Settings()
{
    float fCamPersp = CalcLandPerspective();
    float fCamRad = CalcLandRadius();

    locCamera.FOV = fCamPersp;
    locCamera.radius = fCamRad;
}

float CalcLandPerspective()
{
    float fCamPerspAdj = 0.0;
    if(CheckAttribute(&InterfaceStates, "Persp2Details"))
       fCamPerspAdj = stf(InterfaceStates.Persp2Details) / DEFAULT_LCAM_PERSP_DEN;

    return (DEFAULT_LCAM_PERSP + fCamPerspAdj);
}

float CalcLandRadius()
{
    float fCamRadAdj = 0.0;
    if(CheckAttribute(&InterfaceStates, "RadDetails"))
       fCamRadAdj = stf(InterfaceStates.RadDetails) / DEFAULT_CAM_RAD;

    return (DEFAULT_CAM_RAD + fCamRadAdj);
}

float GetMultiplyFromScreenRatio()
{
    float w = stf(showWindow.width);
    float h = stf(showWindow.height);
    return w / h;
}


/////////////////////////////////////////////////////////////////////////////////////////
//							Warship 20.07.09 НОВОЕ - СНИМАЕМ КИНО
/////////////////////////////////////////////////////////////////////////////////////////

// Функция запуска вращения камеры вокруг персонажа.
// Параметры:
// float  _offsetX ... _offsetZ - смещение камеры относительно текущей позиции персонажа
// float _rotateX, _rotateY - скорость фращения по данной оси, от 0.01 и меньше до 0.3, может быть отрицательна
// float _radius - радиус, он же дистанция от начальных точек до камеры
// float _startAngle- начальный угол, откуда крутим
// int _time - время вращения В КАДРАХ. Она секунда - в среднем 30 кадров, но если учитывать тормоза, то может быть и 10
// bool _teleport - если true, переместится в заданную точку мгновенно, иначе - плавный полет
bool locCameraRotateAroundHero(float _offsetX, float _offsetY, float _offsetZ, float _rotateX, float _rotateY,  float _radius, float _startAngle, int _time)
{
	ref curCameraState;
	int cameraCurState = locCameraGetFirstEmptyState();
	float charX, charY, charZ;

	if(cameraCurState == -1 || !GetCharacterPos(PChar, &charX, &charY, &charZ)) return false;

	curCameraState = &objLocCameraStates[cameraCurState];
	curCameraState.curCameraX = charX + _offsetX;
	curCameraState.curCameraY = charY + _offsetY;
	curCameraState.curCameraZ = charZ + _offsetZ;
	curCameraState.offsetX = _offsetX;
	curCameraState.offsetY = _offsetY;
	curCameraState.offsetZ = _offsetZ;
	curCameraState.rotateX = _rotateX;
	curCameraState.rotateY = _rotateY;
	curCameraState.rotateRadius = _radius;
	curCameraState.time = _time;
	curCameraState.angle = _startAngle;
	curCameraState.type = LOCCAMERA_ROTATE; // Тип камеры

	if(iLocCameraCurState == -1) iLocCameraCurState = 0;

	return true;
}

// Полет камеры от начальных точек _startX ... _startZ до конечных точек _endX ... _endZ
// float _speed - множитель скорости в режиме полета _time == -1. Если _speed == 1, то это станрадтрая скорость
// int _time - кол-во кадров, за которое должно долететь. Если -1, то высчитывается исходя из расстояния
// bool _fromCurCameraPos - если true, то при переходе на эту функци полета, начальная позиция будет считаться как текущая позиция камеры
bool locCameraFlyToPosition(float _startX, float _startY, float _startZ, float _endX, float _endY, float _endZ, float _speed, int _time)
{
	ref curCameraState;
	int cameraCurState = locCameraGetFirstEmptyState();
	float distance;

	if(cameraCurState == -1) return false;

	distance = GetDistance3D(_startX, _startY, _startZ, _endX, _endY, _endZ);

	curCameraState = &objLocCameraStates[cameraCurState];
	curCameraState.curCameraX = _startX;
	curCameraState.curCameraY = _startY;
	curCameraState.curCameraZ = _startZ;
	curCameraState.endCameraX = _endX;
	curCameraState.endCameraY = _endY;
	curCameraState.endCameraZ = _endZ;

	if(_time == -1)
	{
		curCameraState.speedX = (_endX - _startX) / (distance * (1 / _speed));
		curCameraState.speedY = (_endY - _startY) / (distance * (1 / _speed));
		curCameraState.speedZ = (_endZ - _startZ) / (distance * (1 / _speed));
	}
	else
	{
		curCameraState.speedX = (_endX - _startX) / _time;
		curCameraState.speedY = (_endY - _startY) / _time;
		curCameraState.speedZ = (_endZ - _startZ) / _time;
	}

	curCameraState.speed = _speed;
	curCameraState.time = _time;
	curCameraState.type = LOCCAMERA_FLYTOPOS; // Тип камеры

	if(iLocCameraCurState == -1) iLocCameraCurState = 0;

	return true;
}

// Фиксирование камеры в определённой точке относительно ГГ
// float _offsetX ... _offsetZ - смещение относительно координат ГГ, для определения точки, где будет находиться камера
// int _time - кол-во кадров, сколько будет висеть. Если -1 - будет висеть вечно
// bool _canRotate - если true, то камера будет крутиться вместе с персонажем
bool locCameraLockNearHero(float _offsetX, float _offsetY, float _offsetZ, int _time, bool _canRotate)
{
	ref curCameraState;
	int cameraCurState = locCameraGetFirstEmptyState();
	float charX, charY, charZ, offsetAY;

	if(cameraCurState == -1 || !GetCharacterPos(PChar, &charX, &charY, &charZ)) return false;

	offsetAY = atan2(_offsetZ, _offsetX);

	if(_offsetX > 0 && _offsetZ > 0)
	{
		offsetAY = offsetAY + PI;
	}

	curCameraState = &objLocCameraStates[cameraCurState];
	curCameraState.curCameraX = charX;
	curCameraState.curCameraY = charY;
	curCameraState.curCameraZ = charZ;
	curCameraState.offsetX = _offsetX;
	curCameraState.offsetY = _offsetY;
	curCameraState.offsetZ = _offsetZ;
	curCameraState.time = _time;
	curCameraState.canRotate = _canRotate;
	curCameraState.offsetAY = offsetAY;
	curCameraState.type = LOCCAMERA_NEARHERO; // Тип камеры

	if(iLocCameraCurState == -1) iLocCameraCurState = 0;

	return true;
}

int locCameraGetFirstEmptyState()
{
	for(int i = 0; i < LOCCAMERA_MAX_STATES; i++)
	{
		if(!CheckAttribute(&objLocCameraStates[i], "time"))
		{
			return i;
		}
	}

	return -1;
}

// Переход к следующему этапу последовательности перемещений камеры.
void locCameraNextState()
{
	if (CheckAttribute(PChar, "funcaftermovie"))
	{
		locCameraResetState();
		string func = PChar.funcaftermovie;
		call func();
		return;
	}
	ref prevCamera, curCamera, nextCamera;
	float distance;
	int time;

	prevCamera = &objLocCameraStates[iLocCameraCurState];
	DeleteAttribute(prevCamera, "time"); // Критерий ненужность
	iLocCameraCurState++;

	curCamera = &objLocCameraStates[iLocCameraCurState];

	if(iLocCameraCurState == LOCCAMERA_MAX_STATES || !CheckAttribute(&curCamera, "curCameraX"))
	{
		locCameraResetState();
		return;
	}

	time = sti(curCamera.time);

	Log_TestInfo("locCameraNextState() == " + curCamera.type);
}

// Сброс специальных состояний камеры - привязывание к персонажу
void locCameraResetState()
{
	locCameraFollow();

	iLocCameraCurState = -1;

	// Потрем все
	for(int i = 0; i < LOCCAMERA_MAX_STATES; i++)
	{
		DeleteAttribute(&objLocCameraStates[i], "");
	}

	Log_TestInfo("locCameraResetState()");
}

// Обновление позиции камеры при входе в кадр
void locCameraUpdate()
{
	ref curCameraState;
	float charX, charY, charZ, charAY;
	float offsetX, offsetZ;
	float rotateRadius, rotateAngle;
	float time; // Здесь время дробное, т.к. учитывается ещё ускорение времени

	float timeScale = 1 + TimeScaleCounter * 0.25; // Текущее ускорение времени

	if(iLocCameraCurState != -1 && !sti(InterfaceStates.Launched))
	{
		if(GetCharacterPos(PChar, &charX, &charY, &charZ))
		{
			curCameraState = &objLocCameraStates[iLocCameraCurState];

			time = stf(curCameraState.time);

			switch(curCameraState.type)
			{
				case LOCCAMERA_ROTATE:
					rotateRadius = stf(curCameraState.rotateRadius);
					rotateAngle = stf(curCameraState.angle);

					// X rotation
					if(stf(curCameraState.rotateX) != 0.0)
					{
						curCameraState.curCameraX = charX + sin(rotateAngle) * rotateRadius + stf(curCameraState.offsetX);
						curCameraState.curCameraZ = charZ + cos(rotateAngle) * rotateRadius + stf(curCameraState.offsetZ);
						curCameraState.angle = rotateAngle + stf(curCameraState.rotateX) * timeScale;
					}
					else // Иначе просто обновление
					{
						curCameraState.curCameraX = charX + stf(curCameraState.offsetX);
						curCameraState.curCameraZ = charZ + stf(curCameraState.offsetZ);
					}

					// Y rotation
					if(stf(curCameraState.rotateY) != 0.0)
					{
						curCameraState.curCameraY = charY + cos(rotateAngle) * rotateRadius + stf(curCameraState.offsetY);
						curCameraState.curCameraX = charX + sin(rotateAngle) * rotateRadius + stf(curCameraState.offsetX);
						curCameraState.angle = rotateAngle + stf(curCameraState.rotateY) * timeScale;
					}
					else
					{
						curCameraState.curCameraY = charY + stf(curCameraState.offsetY);
					}
				break;

				case LOCCAMERA_FLYTOPOS:
					curCameraState.curCameraX = stf(curCameraState.curCameraX) + stf(curCameraState.speedX) * timeScale;
					curCameraState.curCameraY = stf(curCameraState.curCameraY) + stf(curCameraState.speedY) * timeScale;
					curCameraState.curCameraZ = stf(curCameraState.curCameraZ) + stf(curCameraState.speedZ) * timeScale;
				break;

				case LOCCAMERA_NEARHERO:
					offsetX = stf(curCameraState.offsetX);
					offsetZ = stf(curCameraState.offsetZ);

					if(sti(curCameraState.canRotate) && GetCharacterAy(PChar, &charAY))
					{
						curCameraState.curCameraX = charX + offsetX * sin(charAY + stf(curCameraState.offsetAY));
						curCameraState.curCameraZ = charZ + offsetZ * cos(charAY + stf(curCameraState.offsetAY));
					}
					else
					{
						curCameraState.curCameraX = charX + offsetX;
						curCameraState.curCameraZ = charZ + offsetZ;
					}

					curCameraState.curCameraY = charY + stf(curCameraState.offsetY);
				break;
			}

			locCameraToPos(stf(curCameraState.curCameraX), stf(curCameraState.curCameraY), stf(curCameraState.curCameraZ), true);

			if(time != -1.0)
			{
				time -= 1 * timeScale; // Учет ускорения времени
				curCameraState.time = time;
			}

			if(time <= 0.0 && time != -1.0)
			{
				locCameraNextState();
			}

			// Тут высчитываем попадание в нужную точку
			if(time == -1.0 && curCameraState.type == LOCCAMERA_FLYTOPOS)
			{
				if(stf(curCameraState.curCameraX) + stf(curCameraState.speedX) * timeScale >= stf(curCameraState.endCameraX) &&
					stf(curCameraState.curCameraX) - stf(curCameraState.speedX) * timeScale <= stf(curCameraState.endCameraX) &&
					stf(curCameraState.curCameraY) + stf(curCameraState.speedY) * timeScale >= stf(curCameraState.endCameraY) &&
					stf(curCameraState.curCameraY) - stf(curCameraState.speedY) * timeScale <= stf(curCameraState.endCameraY) &&
					stf(curCameraState.curCameraZ) + stf(curCameraState.speedZ) * timeScale >= stf(curCameraState.endCameraZ) &&
					stf(curCameraState.curCameraZ) - stf(curCameraState.speedZ) * timeScale <= stf(curCameraState.endCameraZ))
				{
					locCameraNextState();
				}
			}
		}
	}
}
