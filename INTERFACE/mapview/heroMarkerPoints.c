// Gregg: координаты метки героя на картах атласа (подключается из heroMarker.c)
// u, v - пиксели текстуры карты 1024x1024 (interfaces\Maps\<imageTga>.dds), как в графическом редакторе.
// Размечаются уличные локации; таверны, дома и т.п. получают точку ближайшей локации по переходам.
// Город и выход из города ставим на якорь порта.

void HM_Set(string mapid, string locid, int u, int v)
{
	oHeroMarks.(mapid).(locid).u = u;
	oHeroMarks.(mapid).(locid).v = v;
}

void InitHeroMarks()
{
	DeleteAttribute(&oHeroMarks, "");

	// ---------------- Антигуа ----------------
	HM_Set("map_antigua", "SentJons_town",        300, 433); // город - на якоре порта
	HM_Set("map_antigua", "SentJons_ExitTown",    334, 416); // выход из города - развилка: форт / джунгли
	HM_Set("map_antigua", "SentJons_fort",        278, 357); // форт
	HM_Set("map_antigua", "Antigua_Jungle_01",    405, 400); // развилка: город / мыс Бун / вглубь острова
	HM_Set("map_antigua", "Shore1",               346, 207); // мыс Бун
	HM_Set("map_antigua", "Antigua_jungle_02",    455, 474); // развилка: пещера / залив Уиллоуби
	HM_Set("map_antigua", "Antigua_CaveEntrance", 655, 500); // вход в пещеру
	HM_Set("map_antigua", "Antigua_Grot",         665, 500); // грот (внутри пещеры)
	HM_Set("map_antigua", "Shore2",               587, 756); // залив Уиллоуби

	// ---------------- Багамы ----------------
	HM_Set("map_Bahames", "Nassau_town",          420, 227); // город - на якоре порта
	HM_Set("map_Bahames", "Nassau_ExitTown",      430, 366); // выход из города - развилка: петля на запад / форт и пещера на юг
	HM_Set("map_Bahames", "Bahames_jungle_05",    456, 422); // развилка: форт / пещера
	HM_Set("map_Bahames", "Nassau_fort",          534, 398); // форт
	HM_Set("map_Bahames", "Bahames_CaveEntrance", 436, 514); // вход в пещеру
	HM_Set("map_Bahames", "Bahames_Grot",         442, 522); // грот (внутри пещеры)
	HM_Set("map_Bahames", "Bahames_jungle_01",    375, 372); // развилка: город / северная и южная дуги петли
	HM_Set("map_Bahames", "Bahames_jungle_03",    310, 428); // южная дуга петли
	HM_Set("map_Bahames", "Bahames_jungle_02",    290, 340); // северная дуга петли
	HM_Set("map_Bahames", "Bahames_jungle_04",    228, 415); // западная развилка петли / бухта Приватира
	HM_Set("map_Bahames", "Shore68",              218, 486); // бухта Приватира
	HM_Set("map_Bahames", "Shore69",              544, 508); // пляж Корсара
	HM_Set("map_Bahames", "Bahames_Jungle_06",    518, 538); // тропа от пляжа Корсара
	HM_Set("map_Bahames", "Bahames_deadlock",     466, 577); // тупик в конце тропы

	// ---------------- Барбадос ----------------
	HM_Set("map_barbados", "Bridgetown_town",       279, 763); // город - на якоре порта
	HM_Set("map_barbados", "Bridgetown_ExitTown",   334, 713); // выход из города - развилка: форт / джунгли
	HM_Set("map_barbados", "Bridgetown_fort",       389, 822); // форт
	HM_Set("map_barbados", "Bridgetown_Plantation", 300, 718); // плантация (на карте не нарисована) - между городом и маяком
	HM_Set("map_barbados", "Mayak2",                306, 680); // маяк Барбадоса
	HM_Set("map_barbados", "Barbados_jungle_02",    414, 690); // развилка: мыс Гаррисон / дальше на восток
	HM_Set("map_barbados", "Shore4",                291, 166); // мыс Гаррисон
	HM_Set("map_barbados", "Barbados_jungle_03",    500, 702); // развилка: пещера / мыс Раггед Поинт
	HM_Set("map_barbados", "Barbados_CaveEntrance", 502, 568); // вход в пещеру
	HM_Set("map_barbados", "Barbados_Cave",         508, 574); // пещера
	HM_Set("map_barbados", "Shore5",                704, 765); // мыс Раггед Поинт

	// ---------------- Белиз (Мэйн) ----------------
	// Shore6/7/_ship2 ведут в общие джунгли Common_jungle_01 - их метка берётся от бухты, из которой пришли.
	HM_Set("map_beliz", "Beliz_town",           144, 888); // город - на якоре порта
	HM_Set("map_beliz", "Beliz_ExitTown",       105, 766); // выход из города - развилка: форт / джунгли
	HM_Set("map_beliz", "Beliz_Fort",           142, 810); // форт
	HM_Set("map_beliz", "Beliz_Jungle_01",       78, 792); // развилка: пещера на севере / пещера на юге
	HM_Set("map_beliz", "Beliz_CaveEntrance_1",  50, 766); // вход в пещеру (северный, из джунглей у города)
	HM_Set("map_beliz", "Beliz_Cave",            39, 718); // пещера - сквозной проход к бухте Четумаль
	HM_Set("map_beliz", "Beliz_CaveEntrance_3",  85, 650); // второй вход в ту же пещеру
	HM_Set("map_beliz", "Beliz_jungle_03",      122, 618); // тропа от бухты Четумаль к пещере
	HM_Set("map_beliz", "Shore8",               168, 631); // бухта Четумаль
	HM_Set("map_beliz", "Beliz_CaveEntrance_2",  54, 897); // вход в пещеру (южный)
	HM_Set("map_beliz", "Beliz_Cave_2",          58, 902); // пещера (южная)
	HM_Set("map_beliz", "Shore7",               215, 445); // залив Косумель
	HM_Set("map_beliz", "Shore6",               220, 131); // мыс Каточе
	HM_Set("map_beliz", "Shore_ship2",           92, 133); // залив Гибели

	// ---------------- Бермуды ----------------
	HM_Set("map_bermudas", "Pirates_town",          463, 490); // Шарп-Таун - на якоре порта (выхода в джунгли нет)
	HM_Set("map_bermudas", "Bermudes_Dungeon",      461, 400); // подземелье от верфи к заливу Руны (на карте не нарисовано) - между городом и заливом
	HM_Set("map_bermudas", "Shore3",                459, 296); // залив Руны
	HM_Set("map_bermudas", "Bermudes_jungle_01",    482, 330); // тропа от залива Руны к пещере
	HM_Set("map_bermudas", "Bermudes_CaveEntrance", 519, 375); // вход в пещеру
	HM_Set("map_bermudas", "Bermudes_Cavern",       577, 400); // пещера - сквозной проход к бухте Разбитого корыта
	HM_Set("map_bermudas", "Shore_ship1",           634, 529); // бухта Разбитого корыта

	// ---------------- Кайман ----------------
	// Колонию строит игрок (Colony_CaimanBuild.c): после постройки переходы перенаправляются через
	// выход из города, плантацию и рудники. На карте их нет - точки поставлены по смыслу.
	// Рудники, их пещера и форт охраны находятся поиском по переходам.
	HM_Set("map_cayman", "Shore17",              409, 641); // залив Эмеральды (после постройки колонии - порт)
	HM_Set("map_cayman", "Shore16",              741, 495); // Пустынный пляж
	HM_Set("map_cayman", "Caiman_Jungle_01",     500, 569); // развилка: залив Эмеральды / пещера / Пустынный пляж
	HM_Set("map_cayman", "Caiman_CaveEntrance",  443, 465); // вход в пещеру
	HM_Set("map_cayman", "Caiman_Grot",          449, 471); // грот
	// постройки колонии
	HM_Set("map_cayman", "Caiman_town",          409, 641); // город - на заливе Эмеральды
	HM_Set("map_cayman", "LandGuardingPort",     409, 641); // порт под охраной форта - там же
	HM_Set("map_cayman", "Caiman_ExitTown",      440, 628); // выход из города - на дороге от залива вглубь острова
	HM_Set("map_cayman", "Caiman_Plantation",    478, 604); // плантация - дальше по той же дороге, к развилке
	HM_Set("map_cayman", "Caiman_ExitMines",     472, 515); // выход к рудникам - между развилкой и пещерой

	// ---------------- Мэйн: карта Маракайбо (Каракас, Маракайбо, Дес-Мойнес, Картахена) ----------------
	// Каракас
	HM_Set("map_maracaibo", "Caracas_town",              926, 828); // город - на якоре порта
	HM_Set("map_maracaibo", "Caracas_ExitTown",          890, 880); // выход из города - развилка дорог от города и форта
	HM_Set("map_maracaibo", "Caracas_Fort",              889, 786); // форт
	HM_Set("map_maracaibo", "Caracas_Jungle_01",         845, 790); // развилка: мыс Аруба / пещера / город
	HM_Set("map_maracaibo", "Shore21",                   879, 737); // мыс Аруба
	HM_Set("map_maracaibo", "Caracas_CaveEntrance",      748, 699); // вход в пещеру
	HM_Set("map_maracaibo", "Caracas_Grot",              754, 704); // грот
	// Маракайбо
	HM_Set("map_maracaibo", "Maracaibo_town",            461, 776); // город - на якоре порта
	HM_Set("map_maracaibo", "Maracaibo_ExitTown",        427, 752); // выход из города - развилка: город / форт / джунгли
	HM_Set("map_maracaibo", "Maracaibo_Fort",            468, 666); // форт
	HM_Set("map_maracaibo", "Maracaibo_jungle_01",       413, 807); // джунгли за выходом из города
	HM_Set("map_maracaibo", "Maracaibo_jungle_03",       398, 838); // развилка: пещера / на юг к Дес-Мойнесу
	HM_Set("map_maracaibo", "Maracaibo_CaveEntrance",    367, 759); // вход в пещеру (южный)
	HM_Set("map_maracaibo", "Maracaibo_Cave",            343, 706); // пещера - сквозной проход к бухте Гуахира
	HM_Set("map_maracaibo", "Maracaibo_CaveEntrance_02", 353, 642); // вход в пещеру (северный)
	HM_Set("map_maracaibo", "Maracaibo_jungle_02",       403, 598); // тропа к бухте Гуахира, у колодца
	HM_Set("map_maracaibo", "Shore37",                   473, 522); // бухта Гуахира
	HM_Set("map_maracaibo", "Maracaibo_Jungle_04",       398, 913); // дорога на юг к Дес-Мойнесу
	// Дес-Мойнес
	HM_Set("map_maracaibo", "DesMoines_town",            562, 951); // поселение
	HM_Set("map_maracaibo", "DesMoines_port",            562, 951); // порт - у поселения
	HM_Set("map_maracaibo", "DesMoines_ExitTown",        458, 956); // выход из поселения
	HM_Set("map_maracaibo", "OutRHouse",                 375, 960); // дорога на запад от поселения, к лесному домику
	HM_Set("map_maracaibo", "Shore_ship3",               550, 903); // мыс Несбывшихся надежд
	// Картахена
	HM_Set("map_maracaibo", "Cartahena_town",            209, 667); // город - на якоре порта
	HM_Set("map_maracaibo", "Cartahena_ExitTown",        180, 732); // выход из города - развилка: город / форт / маяк / джунгли
	HM_Set("map_maracaibo", "Cartahena_fort",            172, 669); // форт
	HM_Set("map_maracaibo", "Mayak11",                   164, 601); // маяк
	HM_Set("map_maracaibo", "Cartahena_jungle_01",       150, 845); // джунгли: дорога от города к пещере
	HM_Set("map_maracaibo", "Cartahena_CaveEntrance",     43, 945); // вход в пещеру
	HM_Set("map_maracaibo", "Cartahena_Cave",             57, 888); // пещера - проход к джунглям у бухты Ковеньяс
	HM_Set("map_maracaibo", "Cartahena_jungle_02",        86, 863); // петля у колодца, бухта Ковеньяс
	HM_Set("map_maracaibo", "Shore25",                    86, 790); // бухта Ковеньяс

	// ---------------- Кумана (Мэйн) ----------------
	HM_Set("map_cumana", "Cumana_town",         386, 661); // город - на якоре порта
	HM_Set("map_cumana", "Cumana_ExitTown",     400, 756); // выход из города - узел дорог от города и форта
	HM_Set("map_cumana", "Cumana_Fort",         432, 651); // форт
	HM_Set("map_cumana", "Cumana_Jungle_01",    434, 855); // развилка: пещера на западе / джунгли на востоке
	HM_Set("map_cumana", "Cumana_CaveEntrance", 256, 912); // вход в пещеру
	HM_Set("map_cumana", "Cumana_Cave",         400, 946); // пещера - проход к джунглям у колодца
	HM_Set("map_cumana", "Cumana_Jungle_02",    478, 840); // петля у колодца (выход из пещеры)
	HM_Set("map_cumana", "Cumana_Jungle_03",    512, 785); // тропа к бухте Каррупано
	HM_Set("map_cumana", "Shore19",             527, 731); // бухта Каррупано
	HM_Set("map_cumana", "Shore20",             138, 741); // пляж Лос Текес (общие джунгли)
	HM_Set("map_cumana", "Shore18",             730, 827); // берег Бока де ла Сьерпе (общие джунгли)

	// ---------------- Кюрасао ----------------
	HM_Set("map_Curacao", "Villemstad_town",      603, 754); // город - на якоре порта
	HM_Set("map_Curacao", "Villemstad_ExitTown",  626, 725); // выход из города - узел: город / форт / джунгли / кладбище
	HM_Set("map_Curacao", "Villemstad_fort",      641, 777); // форт
	HM_Set("map_Curacao", "Villemstad_Graveyard", 760, 681); // кладбище - на дороге к мысу Приют Контрабандистов
	HM_Set("map_Curacao", "Shore24",              899, 827); // мыс Приют Контрабандистов
	HM_Set("map_Curacao", "Curacao_jungle_01",    608, 654); // развилка: пещера / джунгли на запад
	HM_Set("map_Curacao", "Curacao_CaveEntrance", 613, 598); // вход в пещеру
	HM_Set("map_Curacao", "Curacao_Cave",         566, 604); // пещера - проход к колодцу
	HM_Set("map_Curacao", "Curacao_jungle_02",    505, 650); // проходные джунгли
	HM_Set("map_Curacao", "Curacao_jungle_03",    449, 612); // развилка у колодца: Пальмовый берег / мыс Кальвинистов
	HM_Set("map_Curacao", "Shore22",              416, 536); // Пальмовый берег
	HM_Set("map_Curacao", "Shore23",              391, 660); // мыс Кальвинистов

	// ---------------- Доминика ----------------
	// Поселение FencingTown (форт, арена) на карте не нарисовано - точки поставлены по переходам.
	HM_Set("map_dominica", "Dominica_Jungle_01",    511, 567); // развилка: пещера / мыс Скотс Хэд / дорога на восток
	HM_Set("map_dominica", "Dominica_CaveEntrance", 452, 453); // вход в пещеру
	HM_Set("map_dominica", "Dominica_Grot",         458, 459); // грот
	HM_Set("map_dominica", "Shore27",               473, 842); // мыс Скотс Хэд
	HM_Set("map_dominica", "FencingTown_ExitTown",  607, 500); // дорога на восток к пляжу Касл Брус (выход из поселения)
	HM_Set("map_dominica", "FencingTown_Fort",      578, 488); // поселение (на карте не нарисовано) - у восточной дороги
	HM_Set("map_dominica", "Shore26",               649, 410); // пляж Касл Брус

	// ---------------- Гваделупа ----------------
	HM_Set("map_guad", "BasTer_town",             302, 777); // город - на якоре порта
	HM_Set("map_guad", "BasTer_ExitTown",         332, 760); // выход из города - узел: город / форт / кладбище / джунгли
	HM_Set("map_guad", "BasTer_Fort",             320, 809); // форт
	HM_Set("map_guad", "BasTer_Graveyard",        390, 752); // кладбище - на дороге к пляжу Капстер
	HM_Set("map_guad", "Shore29",                 404, 803); // пляж Капстер
	HM_Set("map_guad", "Guadeloupe_jungle_01",    312, 692); // развилка: маяк / джунгли на север
	HM_Set("map_guad", "Mayak4",                  235, 672); // маяк
	HM_Set("map_guad", "Guadeloupe_jungle_02",    315, 624); // развилка: пещера / бухта Морн л'О
	HM_Set("map_guad", "Guadeloupe_CaveEntrance", 278, 524); // вход в пещеру
	HM_Set("map_guad", "Guadeloupe_Cave",         284, 530); // пещера
	HM_Set("map_guad", "Shore28",                 532, 243); // бухта Морн л'О
	// Мария Галанте
	HM_Set("map_guad", "Shore30",                 574, 808); // бухта Гран Бур
	HM_Set("map_guad", "Shore31",                 715, 805); // мыс Недоступный
	HM_Set("map_guad", "Guadeloupe_jungle_add",   620, 797); // тропа через остров
	HM_Set("map_guad", "Guadeloupe_deadlock",     650, 782); // тупик к северу от тропы (дальше пещера сатанистов)

	// ---------------- Куба (Гавана, Пуэрто-Принсипе, Сантьяго) ----------------
	// Гавана
	HM_Set("map_cuba", "Havana_town",             186, 262); // город - на якоре порта
	HM_Set("map_cuba", "Havana_ExitTown",         222, 302); // выход из города - узел: город / форт / кладбище / джунгли
	HM_Set("map_cuba", "Havana_Fort",             266, 258); // форт
	HM_Set("map_cuba", "Havana_Graveyard",        180, 318); // кладбище - на дороге к маяку
	HM_Set("map_cuba", "Mayak10",                 152, 330); // маяк
	HM_Set("map_cuba", "Cuba_Jungle_10",          272, 325); // развилка за городом: на восток / на юго-запад
	HM_Set("map_cuba", "Cuba_jungle_11",          345, 362); // развилка: залив Батабано / дальше на восток
	HM_Set("map_cuba", "Shore13",                 254, 360); // залив Батабано
	HM_Set("map_cuba", "Cuba_jungle_12",          168, 382); // развилка: бухта Хувентуд / на юг
	HM_Set("map_cuba", "Shore14",                 162, 503); // бухта Хувентуд
	HM_Set("map_cuba", "Cuba_jungle_13",          110, 490); // тропа к мысу Сан Антонио
	HM_Set("map_cuba", "Shore15",                  82, 558); // мыс Сан Антонио
	// центр
	HM_Set("map_cuba", "Cuba_Jungle_07",          445, 372); // развилка у пещеры
	HM_Set("map_cuba", "Cuba_CaveEntrance",       432, 330); // вход в пещеру
	HM_Set("map_cuba", "Cuba_Grot",               438, 336); // грот
	HM_Set("map_cuba", "Cuba_Jungle_06",          565, 402); // развилка: залив Анна Мария / дальше
	HM_Set("map_cuba", "Shore12",                 550, 443); // залив Анна Мария
	HM_Set("map_cuba", "Cuba_Jungle_05",          655, 455); // развилка у петли: мыс Камагуэй / на запад
	HM_Set("map_cuba", "Shore11",                 668, 333); // мыс Камагуэй
	HM_Set("map_cuba", "Cuba_Jungle_04",          730, 505); // проходные джунгли у колодца
	// Пуэрто-Принсипе и Сантьяго
	HM_Set("map_cuba", "Cuba_Jungle_03",          790, 515); // развилка: Пуэрто-Принсипе / на запад / на юг к Сантьяго
	HM_Set("map_cuba", "PuertoPrincipe_ExitTown", 790, 485); // выход из поселения
	HM_Set("map_cuba", "PuertoPrincipe_town",     818, 490); // поселение Пуэрто-Принсипе
	HM_Set("map_cuba", "PuertoPrincipe_port",     832, 467); // порт Пуэрто-Принсипе
	HM_Set("map_cuba", "Cuba_Jungle_01",          835, 548); // развилка: Сантьяго / маяк / на север
	HM_Set("map_cuba", "Cuba_Jungle_02",          845, 618); // дорога вдоль восточного берега к маяку
	HM_Set("map_cuba", "Mayak9",                  762, 683); // маяк
	HM_Set("map_cuba", "Santiago_town",           718, 587); // Сантьяго - на якоре порта
	HM_Set("map_cuba", "Santiago_ExitTown",       778, 582); // выход из города - развилка: форт / джунгли
	HM_Set("map_cuba", "Santiago_fort",           790, 628); // форт

	// ---------------- Эспаньола (Санто-Доминго, Ла Вега, Порт-о-Пренс) ----------------
	// Mayak7 (маяк у Порт-о-Пренса) ведёт только в море и на карте не нарисован - без точки.
	// Санто-Доминго
	HM_Set("map_hisp", "SantoDomingo_town",        783, 697); // город - на якоре порта
	HM_Set("map_hisp", "SantoDomingo_ExitTown",    752, 648); // выход из города - узел: город / форт / джунгли
	HM_Set("map_hisp", "SantoDomingo_fort",        718, 680); // форт
	HM_Set("map_hisp", "Hispaniola_Jungle_01",     712, 600); // развилка: маяк / на север
	HM_Set("map_hisp", "Mayak8",                   683, 718); // маяк
	HM_Set("map_hisp", "Hispaniola_Jungle_02",     722, 525); // развилка у колодца: Ла Вега / бухта Самана / пещера
	HM_Set("map_hisp", "Shore33",                  870, 600); // бухта Самана
	// Ла Вега
	HM_Set("map_hisp", "LaVega_town",              686, 424); // поселение Ла Вега
	HM_Set("map_hisp", "LaVega_ExitTown",          670, 458); // выход из поселения - развилка дорог
	// центр и запад
	HM_Set("map_hisp", "Hispaniola_Jungle_04",     622, 470); // развилка: мыс Изабела / на запад
	HM_Set("map_hisp", "Shore32",                  536, 324); // мыс Изабела
	HM_Set("map_hisp", "Hispaniola_Grot",          585, 350); // грот у мыса Изабела
	HM_Set("map_hisp", "Hispaniola_Jungle_03",     598, 503); // проходные джунгли
	HM_Set("map_hisp", "Hispaniola_Jungle_05",     565, 525); // развилка у петли: пещера / на запад
	HM_Set("map_hisp", "Hispaniola_CaveEntrance",  578, 608); // вход в пещеру
	HM_Set("map_hisp", "Hispaniola_Cave",          650, 560); // пещера - проход к колодцу
	HM_Set("map_hisp", "Hispaniola_Jungle_06",     512, 522); // развилка: бухта Гонаив / Порт-о-Пренс
	HM_Set("map_hisp", "Shore34",                  350, 392); // бухта Гонаив
	// Порт-о-Пренс
	HM_Set("map_hisp", "PortPax_town",             383, 562); // город - на якоре порта
	HM_Set("map_hisp", "PortPax_ExitTown",         433, 545); // выход из города - развилка: форт / джунгли
	HM_Set("map_hisp", "PortPax_Fort",             380, 605); // форт

	// ---------------- Ямайка ----------------
	HM_Set("map_jam", "PortRoyal_town",         680, 588); // Порт-Роял - на якоре порта
	HM_Set("map_jam", "PortRoyal_ExitTown",     672, 540); // выход из города - узел: город / форт / кладбище / джунгли
	HM_Set("map_jam", "PortRoyal_fort",         627, 640); // форт
	HM_Set("map_jam", "PortRoyal_Graveyard",    765, 565); // кладбище - на дороге к маяку
	HM_Set("map_jam", "Mayak3",                 703, 640); // маяк
	HM_Set("map_jam", "Jamaica_jungle_01",      607, 488); // развилка: пещера / на запад
	HM_Set("map_jam", "Jamaica_CaveEntrance",   565, 390); // вход в пещеру
	HM_Set("map_jam", "Jamaica_Grot",           571, 396); // грот
	HM_Set("map_jam", "Jamaica_jungle_02",      557, 545); // развилка: бухта Портлэнд / на запад
	HM_Set("map_jam", "Shore36",                580, 645); // бухта Портлэнд
	HM_Set("map_jam", "Jamaica_Jungle_03",      400, 488); // длинная дорога на запад к Форт Оранж
	// Форт Оранж
	HM_Set("map_jam", "FortOrange_ExitTown",    240, 362); // выход из поселения
	HM_Set("map_jam", "FortOrange_Grot",        230, 372); // грот у выхода (на карте не нарисован)
	HM_Set("map_jam", "FortOrange_town",        145, 383); // поселение Форт Оранж
	HM_Set("map_jam", "FortOrange_port",        125, 447); // порт - мыс Негрил

	// ---------------- Город потерянных кораблей ----------------
	// Метка ставится на корабль, внутри которого герой; на открытых палубах - в центр города.
	HM_Set("map_LSC", "LostShipsCity_town",        500, 640); // палубы города - одна локация на весь город, точка в центре
	HM_Set("map_LSC", "SanAugustineResidence",     428, 365); // Сан Августин - резиденция
	HM_Set("map_LSC", "CarolineBank",              518, 478); // Каролина - ростовщик
	HM_Set("map_LSC", "GloriaChurch",              560, 512); // Глория - церковь
	HM_Set("map_LSC", "EsmeraldaStoreBig",         568, 590); // Эсмеральда - магазин
	HM_Set("map_LSC", "PlutoStoreSmall",           435, 550); // Плуто
	HM_Set("map_LSC", "SantaFlorentinaShipInside4", 400, 592); // Санта Флорентина
	HM_Set("map_LSC", "AvaShipInside3",            420, 665); // Ева
	HM_Set("map_LSC", "FleuronTavern",             318, 595); // Флерон - таверна
	HM_Set("map_LSC", "FurieShipInside2",          315, 488); // Фурия
	HM_Set("map_LSC", "ProtectorFisher",           215, 510); // Протектор
	HM_Set("map_LSC", "VelascoShipInside1",        262, 730); // Веласко
	HM_Set("map_LSC", "TartarusPrison",            565, 752); // Тартарус - тюрьма
	HM_Set("map_LSC", "SanGabrielMechanic",        683, 480); // Сан Габриэль
	HM_Set("map_LSC", "FenixPlatform",             783, 385); // Феникс
	HM_Set("map_LSC", "FernandaDiffIndoor",        870, 485); // Фернандо
	HM_Set("map_LSC", "CeresSmithy",               720, 590); // Церес
	HM_Set("map_LSC", "ExternalRingDeck",          512, 150); // внешнее кольцо - правый обломок на севере
	HM_Set("map_LSC", "ExternalRingInside",        512, 150); // внешнее кольцо - внутри
	HM_Set("map_LSC", "ExternalRingCabin1",        512, 150); // внешнее кольцо - каюта

	// ---------------- Мартиника ----------------
	HM_Set("map_martiniqua", "FortFrance_town",       443, 557); // Форт-де-Франс - на якоре порта
	HM_Set("map_martiniqua", "FortFrance_ExitTown",   487, 520); // выход из города - узел: город / форт / джунгли
	HM_Set("map_martiniqua", "FortFrance_fort",       505, 592); // форт
	HM_Set("map_martiniqua", "Martinique_jungle_01",  506, 467); // развилка: пляж Ламантен на север / на восток
	HM_Set("map_martiniqua", "Shore38",               210, 272); // пляж Ламантен
	HM_Set("map_martiniqua", "Martinique_jungle_02",  560, 495); // развилка: Ле Франсуа / бухта Ле Марен
	HM_Set("map_martiniqua", "Martinique_jungle_03",  618, 700); // дорога на юг к бухте Ле Марен
	HM_Set("map_martiniqua", "Shore39",               683, 826); // бухта Ле Марен
	HM_Set("map_martiniqua", "LeFransua_ExitTown",    628, 508); // выход из поселения Ле Франсуа
	HM_Set("map_martiniqua", "LeFransua_town",        640, 548); // поселение Ле Франсуа
	HM_Set("map_martiniqua", "LeFransua_deadlock",    626, 472); // тупик - отросток дороги на север

	// ---------------- Невис ----------------
	HM_Set("map_nevis", "Charles_town",         313, 625); // Чарльзтаун - на якоре порта
	HM_Set("map_nevis", "Charles_ExitTown",     418, 607); // выход из города - развилка: на север / на юг
	HM_Set("map_nevis", "Charles_Jungle_05",    400, 692); // развилка у петли: форт / маяк
	HM_Set("map_nevis", "Charles_fort",         343, 690); // форт
	HM_Set("map_nevis", "Mayak5",               325, 740); // маяк
	HM_Set("map_nevis", "Charles_Jungle_02",    478, 560); // развилка: пляж Бат / на север
	HM_Set("map_nevis", "Shore43",              542, 866); // пляж Бат
	HM_Set("map_nevis", "Charles_Jungle_03",    535, 505); // развилка у петли: пещера / на север
	HM_Set("map_nevis", "Charles_CaveEntrance", 580, 545); // вход в пещеру
	HM_Set("map_nevis", "Charles_Grot",         586, 551); // грот
	HM_Set("map_nevis", "Charles_jungle_04",    485, 300); // длинная дорога на север к бухте Ньюкасл
	HM_Set("map_nevis", "Shore42",              505, 136); // бухта Ньюкасл

	// ---------------- Мэйн: карта Панамы (Панама, Порто-Белло) ----------------
	// Shore46 (общие джунгли) на карте не нарисован - без точки.
	// Панама
	HM_Set("map_panama", "Panama_town",             625, 612); // Панама - на якоре порта
	HM_Set("map_panama", "Panama_ExitTown",         650, 560); // выход из города - узел над заливом
	HM_Set("map_panama", "Panama_Fort",             735, 648); // форт
	HM_Set("map_panama", "Panama_jungle_01",        540, 560); // дорога на запад к Порто-Белло
	HM_Set("map_panama", "Panama_jungle_02",        495, 585); // развилка: пещера на юге / Порто-Белло
	HM_Set("map_panama", "Panama_CaveEntrance",     526, 702); // вход в пещеру
	HM_Set("map_panama", "Panama_Cave",             530, 708); // пещера
	HM_Set("map_panama", "Panama_jungle_03",        700, 520); // дорога на восток мимо колодца
	HM_Set("map_panama", "Panama_jungle_04",        790, 555); // развилка: залив Сан Мигель / на восток
	HM_Set("map_panama", "Shore63",                 805, 903); // залив Сан Мигель
	HM_Set("map_panama", "Panama_jungle_05",        880, 620); // дорога к Дарьенскому заливу
	HM_Set("map_panama", "Shore48",                 942, 712); // Дарьенский залив
	// Порто-Белло
	HM_Set("map_panama", "PortoBello_town",         583, 462); // Порто-Белло - на якоре порта
	HM_Set("map_panama", "PortoBello_ExitTown",     515, 545); // выход из города - узел под городом
	HM_Set("map_panama", "PortoBello_fort",         518, 460); // форт
	HM_Set("map_panama", "PortoBello_Jungle_01",    450, 562); // развилка: Панама / на запад
	HM_Set("map_panama", "PortoBello_Jungle_02",    300, 760); // развилка: залив Москитос / пещера
	HM_Set("map_panama", "Shore47",                 275, 748); // залив Москитос
	HM_Set("map_panama", "PortoBello_CaveEntrance", 188, 858); // вход в пещеру
	HM_Set("map_panama", "PortoBello_Cave",         194, 864); // пещера

	// ---------------- Мэйн: жемчужные промыслы ----------------
	// Сам город Санта Каталина и Теночтитлан - на карте Санта Каталины.
	// западное побережье (левая часть карты)
	HM_Set("map_Pearl", "Shore9",                          105,  90); // бухта Аматике
	HM_Set("map_Pearl", "Beliz_jungle_02",                  72, 165); // тропа от бухты Аматике на юг
	HM_Set("map_Pearl", "Tenotchitlan_Jungle_03",          100, 212); // развилка: пещера / на юг
	HM_Set("map_Pearl", "Tenotchitlan_CaveEntrance",        55, 240); // вход в пещеру
	HM_Set("map_Pearl", "Tenotchitlan_Cave",                61, 246); // пещера
	HM_Set("map_Pearl", "Tenotchitlan_Jungle_02",          150, 268); // развилка: храм инков / на юг
	HM_Set("map_Pearl", "Temple",                           68, 322); // храм инков
	HM_Set("map_Pearl", "Temple_Inside",                    68, 322); // храм инков - внутри
	HM_Set("map_Pearl", "Tenotchitlan_Jungle_01",          178, 370); // проходные джунгли у колодца
	HM_Set("map_Pearl", "Pearl_Jungle_04",                 175, 440); // проходные джунгли
	HM_Set("map_Pearl", "Pearl_Jungle_03",                 178, 500); // развилка: грот / на юг
	HM_Set("map_Pearl", "Pearl_GrotEntrance",              125, 505); // вход в грот
	HM_Set("map_Pearl", "Pearl_Grot",                      131, 511); // грот
	HM_Set("map_Pearl", "Pearl_Jungle_02",                 235, 640); // развилка: посёлок ловцов / на юг
	HM_Set("map_Pearl", "Pearl_town_1",                    292, 580); // посёлок ловцов
	HM_Set("map_Pearl", "Pearl_Jungle_01",                 215, 825); // развилка: залив Папагайо / залив Никоя
	HM_Set("map_Pearl", "Shore49",                         133, 783); // залив Папагайо
	HM_Set("map_Pearl", "Shore50",                         148, 893); // залив Никоя
	// район Санта Каталины (правая вставка)
	HM_Set("map_Pearl", "Shore51",                         635, 493); // залив Коронадо
	HM_Set("map_Pearl", "Pearl_town_2",                    712, 505); // посёлок ловцов
	HM_Set("map_Pearl", "Pearl_Jungle_07",                 715, 570); // дорога на юг от посёлка
	HM_Set("map_Pearl", "Pearl_Jungle_06",                 705, 640); // развилка: пещера / на юго-запад
	HM_Set("map_Pearl", "Pearl_CaveEntrance",              710, 672); // вход в пещеру (северный)
	HM_Set("map_Pearl", "SantaCatalina_PearlCave",         740, 690); // пещера - сквозной проход
	HM_Set("map_Pearl", "SantaCatalina_PearlCaveEntrance", 765, 712); // вход в пещеру (южный)
	HM_Set("map_Pearl", "Pearl_Jungle_08",                 665, 690); // развилка: индейская деревня / тупик
	HM_Set("map_Pearl", "Pearl_Jungle_09",                 650, 648); // тупиковая петля
	HM_Set("map_Pearl", "IndianVillage",                   620, 718); // индейская деревня
	HM_Set("map_Pearl", "SantaCatalina_jungle_03",         770, 668); // развилка: пещера / грот / залив
	HM_Set("map_Pearl", "SantaCatalina_jungle_04",         765, 635); // тропа к гроту
	HM_Set("map_Pearl", "SantaCatalina_GrotEntrance",      768, 598); // вход в грот
	HM_Set("map_Pearl", "SantaCatalina_Grot",              774, 604); // грот
	HM_Set("map_Pearl", "SantaCatalina_jungle_02",         805, 645); // петля к заливу Сан Хуан дель Норте
	HM_Set("map_Pearl", "Shore55",                         827, 613); // залив Сан Хуан дель Норте

	// ---------------- Мэйн: карта Санта Каталины ----------------
	// Теночтитлан и индейская деревня на этой карте не нарисованы.
	// Санта Каталина
	HM_Set("map_santa", "SantaCatalina_town",              245, 400); // город - на якоре порта
	HM_Set("map_santa", "SantaCatalina_ExitTown",          215, 368); // выход из города - узел у колодца
	HM_Set("map_santa", "SantaCatalina_Fort",              232, 310); // форт
	HM_Set("map_santa", "SantaCatalina_Jungle_01",         225, 268); // развилка: пещера / мыс Перлас
	HM_Set("map_santa", "Shore54",                         262, 290); // мыс Перлас
	HM_Set("map_santa", "SantaCatalina_CaveEntrance",      190, 235); // вход в пещеру
	HM_Set("map_santa", "SantaCatalina_Cave",              190, 300); // пещера - сквозной проход к выходу из города
	HM_Set("map_santa", "Shore53",                         231,  70); // берег Москитов
	HM_Set("map_santa", "Shore46",                         304, 785); // лагуна Чирики (общие джунгли)
	// север: посёлок ловцов и заливы
	HM_Set("map_santa", "Pearl_town_1",                    200, 122); // посёлок ловцов
	HM_Set("map_santa", "Pearl_Jungle_02",                 175, 172); // развилка: посёлок / на юг
	HM_Set("map_santa", "Pearl_Jungle_01",                 150, 305); // развилка: залив Папагайо / залив Никоя
	HM_Set("map_santa", "Shore49",                          95, 292); // залив Папагайо
	HM_Set("map_santa", "Shore50",                         100, 375); // залив Никоя
	// юг: район жемчужных промыслов
	HM_Set("map_santa", "Shore51",                         152, 540); // залив Коронадо
	HM_Set("map_santa", "Pearl_town_2",                    222, 527); // посёлок ловцов
	HM_Set("map_santa", "Pearl_Jungle_07",                 190, 550); // дорога от посёлка на юг
	HM_Set("map_santa", "Pearl_Jungle_06",                 182, 565); // развилка у пещеры
	HM_Set("map_santa", "Pearl_CaveEntrance",              180, 585); // вход в пещеру (северный)
	HM_Set("map_santa", "SantaCatalina_PearlCave",         160, 650); // пещера - сквозной проход
	HM_Set("map_santa", "SantaCatalina_PearlCaveEntrance", 200, 712); // вход в пещеру (южный)
	HM_Set("map_santa", "SantaCatalina_jungle_03",         232, 688); // развилка: пещера / грот / залив
	HM_Set("map_santa", "SantaCatalina_jungle_04",         232, 640); // тропа к гроту
	HM_Set("map_santa", "SantaCatalina_GrotEntrance",      255, 583); // вход в грот
	HM_Set("map_santa", "SantaCatalina_Grot",              261, 589); // грот
	HM_Set("map_santa", "SantaCatalina_jungle_02",         292, 655); // петля к заливу Сан Хуан дель Норте
	HM_Set("map_santa", "Shore55",                         273, 618); // залив Сан Хуан дель Норте

	// ---------------- Сан-Мартин ----------------
	HM_Set("map_sm", "Marigo_town",          322, 640); // Мариго - на якоре порта
	HM_Set("map_sm", "Marigo_Jungle_03",     368, 612); // выход из города на север
	HM_Set("map_sm", "Marigo_ExitTown",      372, 665); // выход из города на юг - развилка: форт / джунгли
	HM_Set("map_sm", "Marigo_fort",          325, 697); // форт
	HM_Set("map_sm", "Marigo_jungle_04",     378, 580); // развилка: бухта Неистового ветра / на восток
	HM_Set("map_sm", "Shore41",              298, 452); // бухта Неистового ветра
	HM_Set("map_sm", "Marigo_jungle_01",     492, 665); // развилка: пещера / на север
	HM_Set("map_sm", "Marigo_CaveEntrance",  540, 705); // вход в пещеру
	HM_Set("map_sm", "Marigo_Cave",          527, 640); // пещера - проход к колодцу
	HM_Set("map_sm", "Marigo_Jungle_02",     497, 555); // развилка у колодца: берег Спасения / запад / юг
	HM_Set("map_sm", "Shore40",              462, 197); // берег Спасения

	// ---------------- Сент-Люсия ----------------
	HM_Set("map_SentLuchia", "Shore66",              378, 232); // мыс Пеликана
	HM_Set("map_SentLuchia", "StLucia_GrotKNS",      378, 232); // грот у мыса Пеликана (на карте не нарисован)
	HM_Set("map_SentLuchia", "StLucia_Jungle_10",    405, 320); // тропа от мыса Пеликана к пещере
	HM_Set("map_SentLuchia", "StLucia_CaveEntrance", 345, 405); // вход в пещеру
	HM_Set("map_SentLuchia", "StLucia_Grot",         351, 411); // грот
	HM_Set("map_SentLuchia", "dungeon_02",           455, 525); // подземелье - от брошенного поселения к мысу Пеликана
	HM_Set("map_SentLuchia", "Ruins",                478, 580); // брошенное поселение
	HM_Set("map_SentLuchia", "StLucia_Jungle_09",    470, 660); // тропа от залива Буканьеров к поселению
	HM_Set("map_SentLuchia", "Shore65",              530, 733); // залив Буканьеров

	// ---------------- Теркс ----------------
	HM_Set("map_terks", "Shore56",              432, 505); // залив Северный
	HM_Set("map_terks", "Terks_Jungle_01",      385, 610); // развилка: оба залива / пещера
	HM_Set("map_terks", "Terks_CaveEntrance",   335, 562); // вход в пещеру
	HM_Set("map_terks", "Terks_Grot",           341, 568); // грот
	HM_Set("map_terks", "Shore57",              352, 725); // залив Южный

	// ---------------- Тортуга ----------------
	HM_Set("map_TORTUGA", "Tortuga_town",         620, 703); // город - на якоре порта
	HM_Set("map_TORTUGA", "Tortuga_ExitTown",     622, 648); // выход из города - узел: форт / маяк / джунгли
	HM_Set("map_TORTUGA", "Tortuga_Fort",         540, 652); // форт
	HM_Set("map_TORTUGA", "Mayak6",               690, 692); // маяк
	HM_Set("map_TORTUGA", "Tortuga_jungle_02",    615, 596); // дорога на север
	HM_Set("map_TORTUGA", "Tortuga_jungle_01",    603, 560); // развилка: пляж Глаз рыбы / пещера
	HM_Set("map_TORTUGA", "Tortuga_CaveEntrance", 525, 378); // вход в пещеру
	HM_Set("map_TORTUGA", "Tortuga_Grot",         531, 384); // грот
	HM_Set("map_TORTUGA", "Shore58",              708, 488); // пляж Глаз рыбы
	HM_Set("map_TORTUGA", "Tortuga_Cave",         660, 600); // подземелье от города к пляжу (на карте не нарисовано)

	// ---------------- Тринидад и Тобаго ----------------
	HM_Set("map_trinidad", "PortSpein_town",       244, 739); // Порт-оф-Спейн - на якоре порта
	HM_Set("map_trinidad", "PortSpein_ExitTown",   279, 713); // выход из города - узел: форт / джунгли
	HM_Set("map_trinidad", "PortSpein_Fort",       189, 702); // форт
	HM_Set("map_trinidad", "Trinidad_Jungle_01",   302, 635); // развилка: пещера / маяк
	HM_Set("map_trinidad", "Trinidad_CaveEntrance", 198, 609); // вход в пещеру
	HM_Set("map_trinidad", "Trinidad_Grot",        204, 615); // грот
	HM_Set("map_trinidad", "Mayak1",               172, 852); // маяк - левый, к юго-западу от города (см. примечание на карте)
	HM_Set("map_trinidad", "Trinidad_Jungle_02",   470, 640); // длинная дорога на мыс Галера
	HM_Set("map_trinidad", "Shore60",              523, 499); // мыс Галера
	HM_Set("map_trinidad", "Shore59",              632, 240); // бухта Скарборо (Тобаго)

	// ---------------- Пуэрто-Рико ----------------
	HM_Set("map_puerto", "SanJuan_town",         620, 462); // город - на якоре порта
	HM_Set("map_puerto", "SanJuan_ExitTown",     630, 517); // выход из города - развилка: форт / джунгли
	HM_Set("map_puerto", "SanJuan_fort",         692, 470); // форт
	HM_Set("map_puerto", "SanJuan_jungle_01",    617, 600); // развилка: город / пляж Фахардо / вглубь острова
	HM_Set("map_puerto", "Shore45",              813, 645); // пляж Фахардо
	HM_Set("map_puerto", "SanJuan_Jungle_02",    545, 566); // развилка: вход в пещеру / дальше на запад
	HM_Set("map_puerto", "SanJuan_CaveEntrance", 443, 595); // вход в пещеру
	HM_Set("map_puerto", "SanJuan_Cave",         432, 553); // пещера (между входом и колодцем)
	HM_Set("map_puerto", "SanJuan_Jungle_03",    465, 492); // проходные джунгли с колодцем
	HM_Set("map_puerto", "Shore44",              180, 358); // бухта Агуадилья
}
