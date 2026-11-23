// LEO: ЗАВЕРШЕНИЕ ИГРЫ - переход в зал славы, где на стендах висят снимки, снятые при получении
//   каждого ранга. Устроено так:
//   - при повышении ранга кадр сохраняется в RESOURCE\Textures\Ranks\<профиль>\Rank<N>.jpg
//     (движковая MakeShotToFile, снимок делается после отрисовки кадра);
//   - перед загрузкой зала SetTexturePath ставит слот на папку профиля, поэтому модель стендов
//     одна на всех: движок сам берёт снимок профиля, а чего нет - общую заглушку из
//     RESOURCE\Textures\Rank<N>.jpg. Профили при этом не пересекаются и модель не дублируется.

// Стендов в модели ровно столько; сколько из них займёт капитан - решает его Обучаемость.
#define FINAL_GAME_STANDS		50
// Достижений нужно закрыть ПОЛНОСТЬЮ (третий уровень), чтобы путь в зал открылся
#define FINAL_GAME_ACHIEVEMENTS	40
// Уровень полностью закрытого достижения
#define FINAL_ACH_DONE_LEVEL	3
// Запас к потолку рангов: формула даёт идеальный предел, а последние доли опыта до него
//   не добираются - с навыками по сотне капитан встаёт на пару рангов ниже.
#define FINAL_RANK_TOLERANCE	2

// 0 снимает проверку ранга и достижений - удобно, когда зал нужно посмотреть без прохождения
#define FINAL_GAME_CHECK_PROGRESS	1
#define FINAL_GAME_LOCATION		"Final"
// 0 - снимок в разрешении экрана (как задано в engine.ini), без ужимания
#define FINAL_SHOT_WIDTH		0
#define FINAL_SHOT_HEIGHT		0

// Слот пути текстур для стендов. 0 занят обычными раскрасками, 3 держим свободным.
#define FINAL_TEXTURE_SLOT		1

// пауза перед снимком первого ранга, мс: ждём, пока догорит фейдер перехода
#define FINAL_START_SHOT_DELAY	4000

// Вход - reload1, выход - reload15: он в центре последней комнаты зала, за ним не локация,
//   а титры. Стенды рангов, до которых капитан не добрался, стоят с заглушками.
// Прочие переходы reload2..23 - телепорты ВНУТРИ зала, их не трогаем.
#define FINAL_ENTER_LOCATOR		"reload1"
#define FINAL_EXIT_LOCATOR		"reload15"

// Имя профиля - это уже имя папки в SAVE\, значит как имя каталога оно заведомо годится.
//   Читаем объект напрямую: GetCurrentProfile живёт в сегменте окна сохранений и отсюда не видна.
string FinalGame_GetProfileDir()
{
	string sProfile = "";
	if( CheckAttribute(&PlayerProfile, "name") ) sProfile = PlayerProfile.name;
	if( sProfile == "" ) sProfile = "default";
	return sProfile;
}

// Папка снимков профиля - внутри RESOURCE\Textures, иначе слот пути её не увидит.
string FinalGame_GetProfileShotsDir(string sProfile)
{
	if( sProfile == "" ) sProfile = "default";
	return "RESOURCE\Textures\Ranks\" + sProfile;
}

string FinalGame_GetShotPath(int iRank)
{
	return FinalGame_GetProfileShotsDir(FinalGame_GetProfileDir()) + "\Rank" + iRank + ".jpg";
}

// Снимок кадра под стенд ранга. Зовётся в момент повышения; сам кадр движок снимет после
// отрисовки, поэтому в снимок попадает ровно то, что видит игрок.
void FinalGame_MakeRankShot(int iRank)
{
	if( iRank < 1 ) return;
	if( iRank > FINAL_GAME_STANDS ) return;	// стендов больше нет
	MakeShotToFile(FinalGame_GetShotPath(iRank), FINAL_SHOT_WIDTH, FINAL_SHOT_HEIGHT);
}

// LEO: первый ранг выдан со стартом игры, повышения не происходит - снимок под него делаем сами.
//   ⚠️ Ждём не по таймеру, а по СОСТОЯНИЮ: первая локация грузится ДО экрана создания персонажа,
//   и снимок по таймеру ловил интерфейс генерации. Ждём, пока интерфейсов нет и игрок в игре,
//   переспрашивая раз в FINAL_START_SHOT_DELAY. Метка в атрибутах переживает сейв, так что
//   снимок делается ровно один раз за игру.
void FinalGame_StartShot()
{
	DelEventHandler("FinalGameStartShot", "FinalGame_StartShot");

	if( CheckAttribute(pchar, "FinalGame.shot1") ) return;
	if( FinalGame_GetRank() != 1 ) return;

	bool bReady = true;
	if( InterfaceStates.Launched != 0 ) bReady = false;	// открыт интерфейс - кадр занят им
	if( !CanSave() ) bReady = false;					// «нигде»: середина перехода

	if( !bReady )
	{
		FinalGame_ScheduleStartShot();
		return;
	}

	pchar.FinalGame.shot1 = true;
	FinalGame_MakeRankShot(1);
}

// Ставится при каждой загрузке локации - гарды отсекают всё, кроме первого раза на первом ранге.
void FinalGame_ScheduleStartShot()
{
	if( CheckAttribute(pchar, "FinalGame.shot1") ) return;
	if( FinalGame_GetRank() != 1 ) return;
	SetEventHandler("FinalGameStartShot", "FinalGame_StartShot", 0);
	PostEvent("FinalGameStartShot", FINAL_START_SHOT_DELAY);
}

// Текущий ранг ГГ. Атрибут может быть не заведён у совсем свежего персонажа.
int FinalGame_GetRank()
{
	if( !CheckAttribute(pchar, "rank") ) return 0;
	return sti(pchar.rank);
}

// Потолок рангов ЭТОГО капитана: цена ранга = 35 - Обучаемость, поэтому чем она выше, тем
//   больше стендов в зале он способен занять. Больше, чем есть стендов, не считаем.
int FinalGame_GetMaxRank()
{
	int iMax = GetCharacterMaxRank(pchar);
	if( iMax > FINAL_GAME_STANDS ) iMax = FINAL_GAME_STANDS;
	if( iMax < 1 ) iMax = 1;
	return iMax;
}

// Ранг, с которого зал открыт: потолок минус запас.
int FinalGame_GetRequiredRank()
{
	int iRank = FinalGame_GetMaxRank() - FINAL_RANK_TOLERANCE;
	if( iRank < 1 ) iRank = 1;
	return iRank;
}

// Закрытые достижения - только доведённые до конца: у многоуровневых это третий уровень,
//   одноуровневые сразу выдаются третьим (см. список в SetTableRowByAchievement).
int FinalGame_GetAchievementsDone()
{
	if( !CheckAttribute(pchar, "achievements") ) return 0;
	int iDone = 0;
	aref arRoot, arCur;
	makearef(arRoot, pchar.achievements);
	int iNum = GetAttributesNum(arRoot);
	string sName;
	for(int n = 0; n < iNum; n++)
	{
		arCur = GetAttributeN(arRoot, n);
		sName = GetAttributeName(arCur);
		if( sti(pchar.achievements.(sName)) == FINAL_ACH_DONE_LEVEL ) iDone++;
	}
	return iDone;
}

// Из самого зала уходить некуда - он и есть финал.
bool FinalGame_IsInHall()
{
	if( !CheckAttribute(pchar, "location") ) return false;
	return pchar.location == FINAL_GAME_LOCATION;
}

// В зал ведёт дверь, поэтому уйти туда можно только с суши - иначе и возвращаться будет некуда.
//   ⚠️ По атрибуту location это не определить: он держит ПОСЛЕДНЮЮ локацию и в море, и на карте.
//   Спрашиваем у мира, как это делает CanSave: есть карта или море - значит не на суше.
bool FinalGame_IsOnLand()
{
	if( IsEntity(&worldMap) != 0 ) return false;
	if( IsEntity(&AISea) != 0 ) return false;
	if( FindLoadedLocation() < 0 ) return false;	// середина перехода: локации ещё нет
	return true;
}

// Условия завершения игры: капитан выбрал свой потолок рангов и закрыл нужное число достижений.
bool FinalGame_CheckConditions()
{
	if( FinalGame_IsInHall() ) return false;
	if( !FinalGame_IsOnLand() ) return false;
	if( FINAL_GAME_CHECK_PROGRESS == 1 )
	{
		if( FinalGame_GetRank() < FinalGame_GetRequiredRank() ) return false;
		if( FinalGame_GetAchievementsDone() < FINAL_GAME_ACHIEVEMENTS ) return false;
	}
	return true;
}

// Слот пути текстур: стенды берут снимки профиля, чего нет - общую заглушку.
// Ставить ДО загрузки локации, снимать сразу после - слот глобальный для рендера.
void FinalGame_SetRankTexturePath(bool bSet)
{
	if( bSet )
	{
		SetTexturePath(FINAL_TEXTURE_SLOT, "Ranks/" + FinalGame_GetProfileDir() + "/");
	}
	else
	{
		SetTexturePath(FINAL_TEXTURE_SLOT, "");
	}
}

// Окружение зала правим из кода, а не атрибутами локации: каталог локаций приходит из сейва,
//   и на уже начатой партии атрибуты до зала не доезжают.
void FinalGame_ApplyEnv()
{
	// Пена прибоя создаётся для любой локации с морем, но разметки для зала нет
	// (resource/foam/locations/Final.ini) - висела неподвижным пятном посреди воды.
	DeleteCoastFoamEnvironment();
}

// Выход из зала: титры, а после них - главное меню (about.c сам зовёт ReturnToMainMenu, если
//   не выставлен showGameMenuOnExit). Возврата в игру отсюда нет - это и есть завершение.
void FinalGame_Finish()
{
	InterfaceStates.showGameMenuOnExit = false;
	LaunchAboutScreen();
}

// Перехват выхода из зала: за последним локатором нет локации, там титры.
//   Зовётся из Reload() до разбора точки перехода.
bool FinalGame_CatchExit(string sCurLocation, string sLocator)
{
	if( sCurLocation != FINAL_GAME_LOCATION ) return false;
	if( sLocator != FINAL_EXIT_LOCATOR ) return false;
	FinalGame_Finish();
	return true;
}

// Переход в зал славы из гейм-меню.
void FinalGame_Start()
{
	if( !FinalGame_CheckConditions() ) return;

	DoQuestReloadToLocation(FINAL_GAME_LOCATION, "reload", FINAL_ENTER_LOCATOR, "");
}
