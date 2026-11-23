// =====================================================================================
//  Страница "Об авторах" (титры). Плавно прокручивающийся снизу вверх список
//  разработчиков и помощников проекта.
//
//  КАК ДОБАВИТЬ НОВОГО АВТОРА / РАЗРАБОТЧИКА:
//    Найдите ниже функцию BuildCredits() и допишите ОДНУ строку:
//        CL("текст по-русски", "english text", "цвет");
//    Цвет - один из: ""  (белый, по умолчанию), "yellow", "blue", "violet", "red".
//    Русская и английская версии задаются рядом, в одном вызове - больше НЕ нужно
//    держать три отдельных массива синхронными и вручную считать индексы строк.
//    Пустая строка-разделитель между блоками: CL_GAP();
//
//  Соглашение по цветам (как сложилось исторически):
//    blue   - крупные заголовки разделов;
//    violet - команды/имена с акцентом (LEOPARD's team, инвесторы и т.п.);
//    yellow - подзаголовки и имена авторов;
//    ""     - обычный поясняющий текст (белый).
// =====================================================================================

float fNewPos; // текущая позиция прокрутки титров: 0.0 (начало) .. 1.0 (конец)

// --- Настройки прокрутки -------------------------------------------------------------
#define CREDITS_TOP_PADDING   35        // пустых строк сверху: титры стартуют ниже экрана и выезжают снизу
#define CREDITS_START_DELAY   370       // пауза перед началом прокрутки, мс
#define CREDITS_TICK_MS       16        // период кадра прокрутки, мс (меньше = плавнее; раньше было 20)
#define CREDITS_STEP          0.00016   // сдвиг позиции за кадр (0.00016 за 16мс = та же общая скорость, что старые 0.0002 за 20мс, но плавнее)
#define CREDITS_MAX           512       // потолок числа строк титров (с запасом; фактически используется nCreditQty)

// --- Хранилище строк титров (заполняется в BuildCredits) -----------------------------
string g_CredRus[CREDITS_MAX];   // текст строки на русском
string g_CredEng[CREDITS_MAX];   // текст строки на английском
string g_CredCol[CREDITS_MAX];   // цвет строки: "" / "yellow" / "blue" / "violet" / "red"
int    nCreditQty;               // сколько строк фактически заполнено

// добавить одну строку титров: русский текст, английский текст, цвет
void CL(string sRus, string sEng, string sCol)
{
	if (nCreditQty >= CREDITS_MAX) return; // защита от переполнения массивов
	g_CredRus[nCreditQty] = sRus;
	g_CredEng[nCreditQty] = sEng;
	g_CredCol[nCreditQty] = sCol;
	nCreditQty = nCreditQty + 1;
}

// добавить пустую строку-разделитель
void CL_GAP()
{
	CL("", "", "");
}

// покрасить строку титров с индексом iLine в нужный цвет ("" - оставить белой)
void ApplyLineColor(int iLine, string sCol)
{
	if (sCol == "") return;
	int color = argb(255, 255, 255, 255);
	if (sCol == "yellow") color = argb(255, 251, 237, 68);
	if (sCol == "red")    color = argb(255, 255, 162, 162);
	if (sCol == "blue")   color = argb(255, 0, 255, 255);
	if (sCol == "violet") color = argb(255, 255, 20, 147);
	SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "INFO_TEXT", 8, iLine, color);
}

// =====================================================================================
//  СОДЕРЖИМОЕ ТИТРОВ. Редактируйте только здесь: одна строка экрана = один вызов CL().
// =====================================================================================
void BuildCredits()
{
	nCreditQty = 0;

	int i;
	for (i = 0; i < CREDITS_TOP_PADDING; i++) CL_GAP(); // верхний отступ - чтобы текст начинался ниже экрана


		// ===== Разработчики проекта / Project developers =====
		CL("Разработчики проекта", "Project developers", "blue");
		CL("LEOPARD's team", "LEOPARD's team", "violet");
		CL("Corsairs Ship Pack: OpenSource Edition", "Corsairs Ship Pack: OpenSource Edition", "yellow");
		CL("Основатель аддона", "The founder of the addon", "yellow");
		CL("LEOPARD", "LEOPARD", "");
		CL_GAP();
		CL("Кодеры", "Coders", "yellow");
		CL("LEOPARD", "LEOPARD", "");
		CL("Gregg", "Gregg", "");
		CL("Qwerry, St.", "Qwerry St.", "");
		CL("Bestreducer", "Bestreducer", "");
		CL("retardokillas", "retardokillas", "");
		CL("DmitrySusokolov", "DmitrySusokolov", "");
		CL("Zendayo", "Zendayo", "");
		CL("Assainisateur", "Assainisateur", "");
		CL_GAP();
		CL("Правка и доработка интерфейсов", "Editing and refinement of interfaces", "yellow");
		CL("LEOPARD", "LEOPARD", "");
		CL("Gregg", "Gregg", "");
		CL("Qwerry, St.", "Qwerry St.", "");
		CL("retardokillas", "retardokillas", "");
		CL("DmitrySusokolov", "DmitrySusokolov", "");
		CL_GAP();
		CL("Моделирование и работа с моделями кораблей и локаций", "Modeling of ships and locations", "yellow");
		CL("Noctus", "Noctus", "");
		CL("Nikk", "Nikk", "");
		CL("Shanks", "Shanks", "");
		CL("IG Baron", "IG Baron", "");
		CL_GAP();
		CL("Тестирование", "Testing", "yellow");
		CL("LEOPARD", "LEOPARD", "");
		CL("Gregg", "Gregg", "");
		CL("Googman", "Googman", "");
		CL("Qwerry, St.", "Qwerry, St.", "");
		CL("Bestreducer", "Bestreducer", "");
		CL("Noctus", "Noctus", "");
		CL("Nikk", "Nikk", "");
		CL("Shanks", "Shanks", "");
		CL("IG Baron", "IG Baron", "");
		CL("DmitrySusokolov", "DmitrySusokolov", "");
		CL("Zendayo", "Zendayo", "");
		CL("Assainisateur", "Assainisateur", "");
		CL_GAP();
		CL_GAP();

		// ===== Особый вклад: DH27 =====
		CL("DH27", "DH27", "blue");
		CL("За его бесценный вклад в объединение сообщества", "For his invaluable contribution to community union,", "yellow");
		CL("веб сайт на корсарскую тему и самое крупное спонсирование", "the Corsair website and the largest sponsorship", "yellow");
		CL_GAP();
		CL_GAP();
		CL_GAP();

		// ===== Инструменты для Blender / Blender tools: Arty aka Artes999 =====
		CL("Переиздание старых плагинов для моддинга", "Republishing old modding plugins and improving them", "yellow");
		CL("и их усовершенствование под современные программы", "for modern Blender programs, as well as many small programs", "yellow");
		CL("для Blender'а, а также многих мелких программ", "for Blender and many other helpfull programs", "yellow");
		CL("Arty aka Artes999", "Arty aka Artes999", "violet");
		CL_GAP();

		// ===== Лаунчер / Launcher: DmitrySusokolov =====
		CL("Создание лаунчера для Corsairs Ship Pack", "Creating a launcher for Corsairs Ship Pack", "yellow");
		CL("DmitrySusokolov", "DmitrySusokolov", "violet");
		CL_GAP();

		// ===== Использованные ресурсы / Resources used =====
		CL("При создании модификации были", "When creating the modification, there were", "blue");
		CL("использованы следующие ресурсы", "the following resources have been used", "blue");
		CL_GAP();
		CL("Код и ресурсы из 'Корсары: Проклятые Судьбой'", "Code and resources from Corsairs: Cursed by Fate", "yellow");
		CL_GAP();
		CL("Код и ресурсы из 'Корсары: Каждому Свое'", "Code and resources from Corsairs: To each His Own", "yellow");
		CL_GAP();
		CL("Код и ресурсы из 'Корсары: История Пирата'", "Code and Resources from Corsairs: A Pirate's Story", "yellow");
		CL_GAP();
		CL("Ресурсы из 'Корсары 2: Пираты Карибского Моря'", "Resources from Corsairs 2: Pirates of the Caribbean", "yellow");
		CL_GAP();
		CL("Код и ресурсы из 'Корсары: New abilities 0.4.0'", "Code and resources from Corsairs: New abilities 0.4.0", "yellow");
		CL_GAP();
		CL("Код и ресурсы из 'Gentlemen of Fortune: Eras II'", "Code and resources from Gentlemen of Fortune: Eras II", "yellow");
		CL_GAP();
		CL("Ресурсы с сайта 'Гавань Корсаров'", "Resources from the Corsairs Harbor website", "yellow");
		CL_GAP();
		CL("Код и ресурсы из 'Reconstruction'", "Code and resources from 'Reconstruction'", "yellow");
		CL_GAP();
		CL_GAP();

		// ===== Благодарности контрибьюторам / Many thanks to contributors =====
		CL("Огромное спасибо следующим людям", "Many thanks to the following people", "blue");
		CL("чьи моды, предложения и прочие новинки", "whose mods, offers and other novelties", "blue");
		CL("официально вошли в проект CSP", "officially joined the CSP project", "blue");
		CL("Nyahow и Trixlike", "Yerilein", "yellow");
		CL("За подробные гайды по квестам", "for his unique character 'The Dark Wanderer' and for helping with the little things", "");
		CL("Ерилейн", "Erilane", "yellow");
		CL("За его уникального героя 'Темный Странник'", "For his unique hero 'Dark Wanderer'", "");
		CL("Konstrush", "Konstrush", "yellow");
		CL("За его описание для героев", "for his description for the heroes", "");
		CL("Etienne и Narve", "Etienne и Narve", "yellow");
		CL("За шикарных персонажей из Чёрных парусов и не только", "for the gorgeous characters from Black Sails and more", "");
		CL("johnny88", "johnny88", "yellow");
		CL("За классный худ и доработку всех портретов", "for the cool HUD and the redesign of all the portraits", "");
		CL("А также за кучу переделок по графике", "and also for a bunch of rework on the schedule", "");
		CL("Kormach", "Kormach", "yellow");
		CL("За переделку множества персонажей", "for remaking a lot of characters", "");
		CL("EvgAnat", "EvgAnat", "yellow");
		CL("За шикарную озвучку боссов-нежить и за уникальные механики", "for the gorgeous voice acting of undead bosses and for the unique mechanics", "");
		CL("Shanks", "Shanks", "yellow");
		CL("За предоставление качественных кораблей и локаций", "for providing quality of ships and locations", "");
		CL("MaxTorp", "MaxTorp", "yellow");
		CL("За высокое качество портретов персонажей", "for the high quality of character portraits", "");
		CL("Zendayo", "Zendayo", "yellow");
		CL("За великолепные правки по квестам и за багофиксы", "For the great edits to the quests and for the bug fixes", "");
		CL("ВДД", "ВДД", "yellow");
		CL("За добавление верёвочек на корабли", "For adding ropes to ships", "");
		CL("mimokrokodil и Fleet_Admiral_Dill_Doe", "mimokrokodil и Fleet_Admiral_Dill_Doe", "yellow");
		CL("За помощь новичкам, подсказкам и советам", "For help to newbies, tips and advice", "");
		CL("Nord117", "Nord117", "yellow");
		CL("За лучшую внимательность к орфографии и грамматике текста", "For the best attention to spelling and grammar of the text", "");
		CL("Senf-Man", "Senf-Man", "yellow");
		CL("За предоставление текстур для кораблей", "For providing textures for ships", "");
		CL_GAP();

		// ===== Ранее помогавшие по проекту / Previously helped =====
		CL("Люди, ранее помогавшие по проекту", "Guys who previously helped on the project", "yellow");
		CL("Korsar Maxim", "Korsar Maxim", "");
		CL("Zik", "Zik", "");
		CL("crem_soda", "crem_soda", "");
		CL("Krah_an_Krait", "Krah_an_Krait", "");
		CL("Shaharan aka mimokrokodil", "Shaharan aka mimokrokodil", "");
		CL("Sinistra", "Sinistra", "");
		CL("BlackThorn", "BlackThorn", "");
		CL("Lipsar", "Lipsar", "");
		CL("Arty", "Arty", "");
		CL("annoyer", "annoyer", "");
		CL_GAP();
		CL_GAP();

		// ===== Инвесторы / Biggest donators =====
		CL("Инвесторы проекта", "Biggest donators", "blue");
		CL("Роман Вячеславович", "Roman Vyacheslavovich", "violet");
		CL("Heli_cat", "Heli_cat", "violet");
		CL("MrGray", "MrGray", "violet");
		CL("Dangetsu", "Dangetsu", "violet");
		CL_GAP();
		CL_GAP();

		// ===== Разработчики движка и штурман / Engine developers & navigator =====
		CL("Особая благодарность всем разработчикам движка", "Special thanks to all the developers of the engine", "yellow");
		CL("OpenSource, а также нашему главному штурману", "OpenSource, as well as our chief navigator", "yellow");
		CL("kb31", "kb31", "violet");
		CL("Без него мы вряд ли бы увидели развитие проекта!", "Without him, we would hardly have seen the development of the project!", "blue");
}

void InitInterface(string iniName)
{
	GameInterface.title = "";

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);

	SetEventHandler("InterfaceBreak", "ProcessCancelExit", 0);
	SetEventHandler("exitCancel", "ProcessCancelExit", 0);
	SetEventHandler("My_MoveText", "MoveText", 0);

	SetFormatedText("INFO_TEXT", "");

	BuildCredits(); // собрать все строки титров (содержимое - в BuildCredits выше)

	bool bRus = (LanguageGetLanguage() == "russian");
	string sLine;
	int i;
	for (i = 0; i < nCreditQty; i++)
	{
		if (bRus) sLine = g_CredRus[i];
		else      sLine = g_CredEng[i];

		// выводим строку + вертикальный интервал (пустые строки) до следующей
		SendMessage(&GameInterface, "lsle", MSG_INTERFACE_MSG_TO_NODE, "INFO_TEXT", 0, sLine + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

		ApplyLineColor(i, g_CredCol[i]); // цвет строки (если задан)
	}

	PostEvent("My_MoveText", CREDITS_START_DELAY); // запуск прокрутки после стартовой паузы
	fNewPos = 0;
	SetMusic("music_Credits"); // LEO: Об авторах - другой музон
}

void ProcessCancelExit()
{
	if(CheckAttribute(&InterfaceStates, "showGameMenuOnExit") && sti(InterfaceStates.showGameMenuOnExit) == true)
	{
		IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU);
		return;
	}

	IDoExit(RC_INTERFACE_OPTIONSCREEN_EXIT);
	if(!CheckAttribute(&InterfaceStates, "InstantExit") || sti(InterfaceStates.InstantExit) == false) ReturnToMainMenu();
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("My_MoveText","MoveText");

	interfaceResultCommand = exitCode;
	if(CheckAttribute(&InterfaceStates,"InstantExit") && sti(InterfaceStates.InstantExit) == true)
		EndCancelInterface(true);
	else
		EndCancelInterface(false);
}

// плавная прокрутка: каждые CREDITS_TICK_MS мс двигаем позицию на CREDITS_STEP, пока титры не доедут до конца
void MoveText()
{
	if(fNewPos >= 1.0)
	{
		ProcessCancelExit(); // титры закончились - выходим со страницы
		return;
	}
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",2, fNewPos);
	PostEvent("My_MoveText", CREDITS_TICK_MS);
	fNewPos = fNewPos + CREDITS_STEP;
}
