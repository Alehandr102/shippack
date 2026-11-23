

// LEO: ЗАЛ СЛАВЫ вынесен в отдельную функцию: её же зовёт FinalGame_Start перед переходом.
//   Каталог локаций приходит из СЕЙВА позиционно, миграций в моде нет - на партии, начатой до
//   правки этого блока, зал приехал бы со старыми атрибутами (без неба и моря). Переинициализация
//   перед входом чинит это и для уже начатых игр.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ЗАЛ СЛАВЫ - финал игры. Стенды берут снимки, снятые при получении рангов, из папки
	//   профиля (см. scripts\FinalGame.c). Переходы reload2..reload23 - телепорты ВНУТРИ зала,
	//   а reload15 в последней комнате - выход, и ведёт он не в локацию, а в титры.
	//   Вынесено в функцию, чтобы блок не тонул среди прочих квестовых локаций.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DefineFinalLocation(int n)
{
	locations[n].id = "Final";
	locations[n].id.label = "Final";
	locations[n].image = "loading\jonny_load\inside\Final.dds";
	//Town sack
	locations[n].townsack = "Havana";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "Final";   // свой тип: по нему sound.c ставит музыку зала
	//Models
	locations[n].filespath.models = "locations\inside\Final\";
	//Always
    locations[n].models.always.Final = "Final_BSP";
	locations[n].models.always.L1 = "Final";
	locations[n].models.always.L2 = "Rangs";
	locations[n].models.always.L3 = "Potolok";
	locations[n].models.always.locators = "Final_locators";
	locations[n].models.always.RoomCrystals = "Room_crystals";
	locations[n].models.always.RoomCrystals.tech = "EnvironmentShader";
	locations[n].models.always.RoomCrystals.reflection = 0.07;
	//Day
	locations[n].models.day.charactersPatch = "Final_patch";
	Locations[n].models.day.jumpPatch = "FINAL_jump_patch";
	//Night
	locations[n].models.night.charactersPatch = "Final_patch";
	Locations[n].models.night.jumpPatch = "FINAL_jump_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
	// LEO: прибойная пена для зала не размечена - без этого висит неподвижным пятном посреди воды
	locations[n].notCrateFoam = true;

	//Reload map
	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Final";
	locations[n].reload.l2.emerge = "reload16";
	locations[n].reload.l2.autoreload = "1";

	locations[n].reload.l3.name = "reload3";
	locations[n].reload.l3.go = "Final";
	locations[n].reload.l3.emerge = "reload17";
	locations[n].reload.l3.autoreload = "1";

	locations[n].reload.l4.name = "reload4";
	locations[n].reload.l4.go = "Final";
	locations[n].reload.l4.emerge = "reload18";
	locations[n].reload.l4.autoreload = "1";

	locations[n].reload.l5.name = "reload5";
	locations[n].reload.l5.go = "Final";
	locations[n].reload.l5.emerge = "reload19";
	locations[n].reload.l5.autoreload = "1";

	locations[n].reload.l6.name = "reload6";
	locations[n].reload.l6.go = "Final";
	locations[n].reload.l6.emerge = "reload20";
	locations[n].reload.l6.autoreload = "1";

	locations[n].reload.l7.name = "reload7";
	locations[n].reload.l7.go = "Final";
	locations[n].reload.l7.emerge = "reload20";
	locations[n].reload.l7.autoreload = "1";

	locations[n].reload.l8.name = "reload8";
	locations[n].reload.l8.go = "Final";
	locations[n].reload.l8.emerge = "reload20";
	locations[n].reload.l8.autoreload = "1";

	locations[n].reload.l9.name = "reload9";
	locations[n].reload.l9.go = "Final";
	locations[n].reload.l9.emerge = "reload20";
	locations[n].reload.l9.autoreload = "1";

	locations[n].reload.l10.name = "reload10";
	locations[n].reload.l10.go = "Final";
	locations[n].reload.l10.emerge = "reload20";
	locations[n].reload.l10.autoreload = "1";

	locations[n].reload.l11.name = "reload11";
	locations[n].reload.l11.go = "Final";
	locations[n].reload.l11.emerge = "reload20";
	locations[n].reload.l11.autoreload = "1";

	locations[n].reload.l12.name = "reload12";
	locations[n].reload.l12.go = "Final";
	locations[n].reload.l12.emerge = "reload20";
	locations[n].reload.l12.autoreload = "1";

	locations[n].reload.l13.name = "reload13";
	locations[n].reload.l13.go = "Final";
	locations[n].reload.l13.emerge = "reload20";
	locations[n].reload.l13.autoreload = "1";

	locations[n].reload.l14.name = "reload14";
	locations[n].reload.l14.go = "Final";
	locations[n].reload.l14.emerge = "reload20";
	locations[n].reload.l14.autoreload = "1";
	
	// LEO: ВЫХОД из зала - центр последней комнаты. Переход перехватывает FinalGame_CatchExit и
	//   вместо загрузки локации запускает титры; go/emerge тут формальные, но запись нужна -
	//   без неё движок не даст сойти с локатора, и перехватывать будет нечего.
	locations[n].reload.l15.name = "reload15";
	locations[n].reload.l15.go = "Final";
	locations[n].reload.l15.emerge = "reload1";
	locations[n].reload.l15.autoreload = "1";
	locations[n].locators_radius.reload.Reload15 = 1.1;


	locations[n].reload.l21.name = "reload21";
	locations[n].reload.l21.go = "Final";
	locations[n].reload.l21.emerge = "reload23";
	locations[n].reload.l21.autoreload = "1";

	locations[n].reload.l22.name = "reload22";
	locations[n].reload.l22.go = "Final";
	locations[n].reload.l22.emerge = "reload24";
	locations[n].reload.l22.autoreload = "1";

	locations[n].reload.l23.name = "reload23";
	locations[n].reload.l23.go = "Final";
	locations[n].reload.l23.emerge = "reload21";
	locations[n].reload.l23.autoreload = "1";

}


int LocationInitQuestLocations(int n)
{
	// LEO: зал славы - тело в DefineFinalLocation (её же зовёт FinalGame_Start)
	DefineFinalLocation(n); n = n + 1;


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Локация клон
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "Clone_location";
	locations[n].id.label = "Farm Field";
	locations[n].filespath.models = "locations\";
	locations[n].image = "loading\Farmfield.tga";
	//Sound
	locations[n].type = "land";
	//Models
	//Always
	locations[n].models.always.house = "farmfield";
	locations[n].models.always.locators = "farmfield_locators";
	locations[n].models.always.grassPatch = "farmfield_grass";

	locations[n].models.always.plan = "farmfield_plan";
	locations[n].models.always.plan.level = 9;

	//Day
	locations[n].models.day.charactersPatch = "farmfield_patch";
	locations[n].models.day.fonar = "farmfield_fd";

	//Night
	locations[n].models.night.charactersPatch = "farmfield_patch";
	locations[n].models.night.fonar = "farmfield_fn";

	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";

	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "Tortuga_tavern";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Town";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Cob_house";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "House";

	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Корабельная палуба  для карта - море
	////////////////////////////////////////////////////////////
	Locations[n].id = "Ship_deck";
	Locations[n].image = "loading\jonny_load\inside ship\deckMedium.dds";
	locations[n].id.label = "Boarding deck";
	//Sound
	Locations[n].type = "deck";
	// нельзя, иначе нет моря Locations[n].boarding = "true";
	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;
	//Models
	//Always
	Locations[n].filespath.models = "locations\decks\deckMedium";
	Locations[n].models.always.deckMedium = "deckMedium";
	Locations[n].models.always.locators = "deckMedium_locators";
	//Day
	locations[n].models.day.charactersPatch = "deckMedium_patch";
	Locations[n].models.day.deckMediumFonarsDay = "deckMedium_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckMedium_patch";
	Locations[n].models.night.deckMediumFonarsNight= "deckMedium_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Корабельная палуба  для море - море
	////////////////////////////////////////////////////////////
    //ID
	Locations[n].id = "Deck_Near_Ship";   // выслать туда шлюпку
	Locations[n].id.label = "Boarding deck";
	//Info
	Locations[n].image = "loading\jonny_load\inside ship\deckMedium.dds";
	//Sound
	Locations[n].type = "residence";
	Locations[n].boarding = "true";
	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;
	//Models
	//Always
	Locations[n].filespath.models = "locations\decks\deckMedium";
	Locations[n].models.always.deckMedium = "deckMedium";
	Locations[n].models.always.locators = "deckMedium_locators";
	//Day
	locations[n].models.day.charactersPatch = "deckMedium_patch";
	Locations[n].models.always.deckMediumFonarsDay = "deckMedium_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckMedium_patch";
	Locations[n].models.always.deckMediumFonarsNight= "deckMedium_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	//Reload map
	LAi_LocationFightDisable(&Locations[n], true);
	n = n + 1;

  	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Имение Оглторпа
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "Estate";
	locations[n].id.label = "Estate";
	locations[n].filespath.models = "locations\Estate";
	locations[n].image = "loading\jonny_load\quest\Estate.dds";
	//Sound
	locations[n].type = "land";
	//Models
	//Always
	locations[n].models.always.estate = "estate";

	locations[n].models.always.windows = "estate_windows";
	Locations[n].models.always.windows.tech = "LocationWindows";
	Locations[n].models.always.windows.level = 66532;
	//VolumeLight
	Locations[n].models.always.vlight = "estate_vlight";
	Locations[n].models.always.vlight.uvslide.v0 = 0.05;
    Locations[n].models.always.vlight.uvslide.v1 = 0.0;
	Locations[n].models.always.vlight.tech = "LocationWaterFall";
	Locations[n].models.always.vlight.level = 99950;

	locations[n].models.always.locators = "estate_locators";
	locations[n].models.always.grassPatch = "estate_grass";
	//Day
	locations[n].models.day.charactersPatch = "estate_patch";
	locations[n].models.day.fonar = "estate_fd";
	//Night
	locations[n].models.night.charactersPatch = "estate_patch";
	locations[n].models.night.fonar = "estate_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	Locations[n].QuestlockWeather = "20 Hour";
	Locations[n].QuestlockWeather.hours = 20;
	Locations[n].QuestlockWeather.minutes = 00;
	//Reload map
	locations[n].reload.l1.name = "reload3";
	locations[n].reload.l1.go = "EstateBadRoom1";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Bedroom";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "EstateBadRoom2";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Bedroom";

	locations[n].reload.l3.name = "reload5";
	locations[n].reload.l3.go = "EstateOffice";
	locations[n].reload.l3.emerge = "reload1";
	locations[n].reload.l3.autoreload = "0";
	locations[n].reload.l3.label = "Cabinet";
	locations[n].locators_radius.item.item1 = 1.0;

	//homo
	locations[n].private1.key = "key3";     //сундук на балконе
	locations[n].private1.key.delItem = true; //eddy. нужно забрать ключ, иначе в ГПК у геймера будет суперлафа
	locations[n].private1.items.blade2 = 1;
	locations[n].private1.items.pistol1 = 1;
	locations[n].private1.items.bullet = 10;
	locations[n].private1.items.GunPowder = 10; // Warship fix 19.03.09 добавим порох
	n = n + 1;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Спальня #1 Оглторпа
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "EstateBadRoom1";
	locations[n].id.label = "Bedroom";
	locations[n].filespath.models = "locations\EstateRooms\EstateBadRoom1";
	locations[n].image = "loading\jonny_load\quest\estateBadRoom1.dds";
	//Models
	//Sound
	locations[n].type = "house";
	//Always
	locations[n].models.always.estateBadRoom = "estateBadRoom1";
	locations[n].models.always.windows = "estateBadRoom1_windows";
	Locations[n].models.always.windows.tech = "LocationWindows";
	Locations[n].models.always.windows.level = 66532;
	//VolumeLight
	Locations[n].models.always.vlight = "estateBadRoom1_vlight";
	Locations[n].models.always.vlight.uvslide.v0 = 0.05;
    Locations[n].models.always.vlight.uvslide.v1 = 0.0;
	Locations[n].models.always.vlight.tech = "LocationWaterFall";
	Locations[n].models.always.vlight.level = 99950;

	locations[n].models.always.locators = "estateBadRoom1_locators";
	locations[n].models.always.grassPatch = "estateRooms_grass";
	//Day
	locations[n].models.day.charactersPatch = "estateBadRoom1_patch";
	locations[n].models.day.fonar = "estateRooms_fd";
	//Night
	locations[n].models.night.charactersPatch = "estateBadRoom1_patch";
	locations[n].models.night.fonar = "estateRooms_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	Locations[n].QuestlockWeather = "20 Hour";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "Estate";
	locations[n].reload.l1.emerge = "reload3";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Estate";
	n = n + 1;

 	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Спальня #2 Оглторпа
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "EstateBadRoom2";
	locations[n].id.label = "Bedroom";
	locations[n].filespath.models = "locations\EstateRooms\EstateBadRoom2";
	locations[n].image = "loading\jonny_load\quest\EstateBadRoom1.dds";
	//Sound
	locations[n].type = "house";
	//Models
	//Always
	locations[n].models.always.estateBadRoom = "estateBadRoom2";
	locations[n].models.always.windows = "estateBadRoom2_windows";
	Locations[n].models.always.windows.tech = "LocationWindows";
	Locations[n].models.always.windows.level = 66532;
	//VolumeLight
	Locations[n].models.always.vlight = "estateBadRoom2_vlight";
	Locations[n].models.always.vlight.uvslide.v0 = 0.05;
   	Locations[n].models.always.vlight.uvslide.v1 = 0.0;
	Locations[n].models.always.vlight.tech = "LocationWaterFall";
	Locations[n].models.always.vlight.level = 99950;

	locations[n].models.always.locators = "estateBadRoom2_locators";
	locations[n].models.always.grassPatch = "estateRooms_grass";
	//Day
	locations[n].models.day.charactersPatch = "estateBadRoom2_patch";
	locations[n].models.day.fonar = "estateRooms_fd";
	//Night
	locations[n].models.night.charactersPatch = "estateBadRoom2_patch";
	locations[n].models.night.fonar = "estateRooms_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	Locations[n].QuestlockWeather = "20 Hour";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "Estate";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Estate";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Кабинет #1 Оглторпа
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "EstateOffice";
	locations[n].id.label = "Cabinet";
	locations[n].filespath.models = "locations\EstateRooms\EstateOffice";
	locations[n].image = "loading\jonny_load\quest\EstateOffice.dds";
	//Sound
	locations[n].type = "residence";
	//Models
	//Always
	locations[n].models.always.EstateOffice = "estateOffice";
	locations[n].models.always.windows = "estateOffice_windows";
	Locations[n].models.always.windows.tech = "LocationWindows";
	Locations[n].models.always.windows.level = 66532;
	//VolumeLight
	Locations[n].models.always.vlight = "estateOffice_vlight";
	Locations[n].models.always.vlight.uvslide.v0 = 0.05;
   	Locations[n].models.always.vlight.uvslide.v1 = 0.0;
	Locations[n].models.always.vlight.tech = "LocationWaterFall";
	Locations[n].models.always.vlight.level = 99950;

	locations[n].models.always.locators = "estateOffice_locators";
	locations[n].models.always.grassPatch = "estateOffice_grass";
	//Day
	locations[n].models.day.charactersPatch = "estateOffice_patch";
	locations[n].models.day.fonar = "estateOffice_fd";
	//Night
	locations[n].models.night.charactersPatch = "estateOffice_patch";
	locations[n].models.night.fonar = "estateOffice_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	Locations[n].QuestlockWeather = "20 Hour";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "Estate";
	locations[n].reload.l1.emerge = "reload5";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Estate";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Подводная Локация
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "UnderWater";
	locations[n].id.label = "UnderWater";
	locations[n].filespath.models = "locations\UnderWater";
	locations[n].image = "loading\jonny_load\quest\underwater.dds";
	//Sound
	locations[n].type = "underwater";
	locations[n].type.LSC = true; //локация ГПК
	//Models
	//Always
	Locations[n].models.always.UnderWater = "UnderWater";
	Locations[n].models.always.UnderWater.tech = "DLightModel";

	Locations[n].models.always.outside = "UnderWater_outside";
	Locations[n].models.always.outside.tech = "DLightModel";

	Locations[n].models.always.reflect = "UnderWater_reflect";
	Locations[n].models.always.reflect .tech = "DLightModel";
	Locations[n].models.always.reflect.sea_reflection = 1;

	Locations[n].models.always.sails = "UnderWater_sails";
	Locations[n].models.always.sails.tech = "DLightModel";

	Locations[n].models.always.plants = "UnderWater_plants";
	Locations[n].models.always.plants.tech = "DLightModel";
	//Locations[n].models.always.plants.tech = "LocationWindows";
	//Locations[n].models.always.plants.level = 66532;

	Locations[n].models.always.plan1 = "Plan1";
	Locations[n].models.always.plan1.sea_reflection = 1;

	Locations[n].models.always.plan2 = "Plan2";

	Locations[n].models.always.locators = "UnderWater_locators";

	Locations[n].models.always.grassPatch = "UnderWater_grass";
	Locations[n].models.always.grassPatch.texture = "grass\algaeU2.tga";
	//Day
	locations[n].models.day.charactersPatch = "UnderWater_patch_day";
	//Night
	locations[n].models.night.charactersPatch = "UnderWater_patch_day";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
	locations[n].underwater = true;
	Locations[n].QuestlockWeather = "Underwater";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "LostShipsCity_town";
	locations[n].reload.l1.emerge = "reload73";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "LSC Town";

	locations[n].locators_radius.reload.reload1 = 2.0;
	locations[n].locators_radius.item.item1 = 0.6;
	locations[n].locators_radius.item.item2 = 0.6;
	locations[n].locators_radius.item.item3 = 0.6;
	locations[n].locators_radius.item.item4 = 0.6;
	locations[n].locators_radius.item.item5 = 0.6;

	//в сундуках
	locations[n].private1.money = 174366;
	locations[n].private1.items.indian1 = 1;
	locations[n].private1.items.indian2 = 1;
	locations[n].private1.items.indian3 = 2;
	locations[n].private1.items.indian4 = 2;
	locations[n].private1.items.indian5 = 1;
	locations[n].private1.items.indian6 = 1;
	locations[n].private1.items.indian7 = 1;
	locations[n].private1.items.indian8 = 2;
	locations[n].private1.items.indian9 = 1;
	locations[n].private1.items.statue1 = 5;
	locations[n].private1.items.potion2 = 21;

	locations[n].private2.money = 72890;
	locations[n].private2.items.potion1 = 59;
	locations[n].private2.items.potionrum = 100;
	locations[n].private2.items.potionwine = 200;
	locations[n].private2.items.potion4 = 41;
	locations[n].private2.items.jewelry1 = 4;
	locations[n].private2.items.jewelry3 = 19;
	locations[n].private2.items.jewelry2 = 72;

	locations[n].private3.money = 375000;
	locations[n].private3.items.indian10 = 1;
	locations[n].private3.items.indian11 = 2;
	locations[n].private3.items.indian12 = 1;
	locations[n].private3.items.indian13 = 2;
	locations[n].private3.items.indian15 = 1;
	locations[n].private3.items.indian16 = 1;
	locations[n].private3.items.indian18 = 1;
	locations[n].private3.items.indian20 = 2;
	locations[n].private3.items.indian21 = 1;
	locations[n].private3.items.indian22 = 1;
	locations[n].private3.items.potion2 = 21;

	locations[n].private4.money = 69720;
	locations[n].private4.items.Mineral2 = 2;
	locations[n].private4.items.Mineral3 = 59332;
	locations[n].private4.items.Mineral5 = 1;
	locations[n].private4.items.Mineral6 = 61;
	locations[n].private4.items.Mineral7 = 5;
	locations[n].private4.items.Mineral8 = 63;
	locations[n].private4.items.Mineral9 = 2;
	locations[n].private4.items.indian15 = 1;
	locations[n].private4.items.indian17 = 2;
	locations[n].private4.items.indian19 = 1;

	locations[n].private5.money = 275900;
	locations[n].private5.items.jewelry5 = 126;
	locations[n].private5.items.jewelry17 = 332;

	locations[n].private6.money = 395100;
	locations[n].private6.items.potion1 = 4;
	locations[n].private6.items.jewelry18 = 14;
	locations[n].private6.items.jewelry16 = 28;
	locations[n].private6.items.jewelry15 = 23;
	locations[n].private6.items.jewelry14 = 34;
	locations[n].private6.items.jewelry13 = 11;
	locations[n].private6.items.jewelry10 = 27;
	locations[n].private6.items.blade27 = 1;

	locations[n].private7.money = 53670;
	locations[n].private7.items.jewelry7 = 10;
	locations[n].private7.items.jewelry11 = 123;
	locations[n].private7.items.jewelry12 = 345;
	locations[n].private7.items.potion1 = 14;
	locations[n].private7.items.potion2 = 21;
	locations[n].private7.items.potion4 = 27;

	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Лока создания персонажа
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Locations[n].id = "SelectMainCharacter_Cabine";   // Lugger: Выбор ГГ
    	locations[n].id.label = "cabine2";
	locations[n].filespath.models = "locations\inside\cabin02\";
	locations[n].image = "loading\jonny_load\inside\cabin02.dds";
	//Sound
	locations[n].type = "residence";
	locations[n].LoadCabinType.Type = "Cabin";
	//Models
	//Always
	locations[n].models.always.l1 = "cabin02";
	locations[n].models.always.l1.level = 65538;
	locations[n].models.always.locators = "cabin02_locators";
	locations[n].models.always.window = "cabin02_fonar";
	Locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	//Day
	locations[n].models.day.charactersPatch = "cabin02_patch";

	//Night
	locations[n].models.night.charactersPatch = "cabin02_patch";

	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	//Locations[n].boarding = "false";
	//Locations[n].camshuttle = 0;

	locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box3 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box4 = Items_MakeTime(0, 1, 1, 2003);

    	LAi_LocationFightDisable(&Locations[n], false);

	Locations[n].locators_radius.randitem.randitem1 = 1;
    	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "";
	Locations[n].reload.l1.emerge = "";
	Locations[n].reload.l1.label = "";
	Locations[n].items.randitem1 = "";

	locations[n].environment.weather.rain = false;
	n = n + 1;
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Киберпанк 2077
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "Cyberpunk2077";
	//locations[n].id.label = "Dungeon";
	locations[n].image = "loading\jonny_load\inside\Collector.dds";
	//Town sack
	//locations[n].townsack = "Pirates";
	//Sound
	locations[n].type = "Dungeon";
	//locations[n].islandId = "Bermudes";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\Collector";
	locations[n].models.always.DungeonVault1 = "Collector";
	locations[n].models.always.InquisitionDungeonVault1Fonars = "Collector_fonars";
	locations[n].models.always.locators = "Collector_locators";

	//Fontain
	locations[n].models.always.Waterfall1 = "fontain1";
	locations[n].models.always.Waterfall1.uvslide.v0 = 0.5;
	locations[n].models.always.Waterfall1.uvslide.v1 = 0.0;
	locations[n].models.always.Waterfall1.tech = "LocationWaterFall";
	locations[n].models.always.Waterfall1.level = 90;

	//WindMill Fan
	locations[n].models.always.MillFan = "MillFan";
	Locations[n].models.always.MillFan.locator.group = "WindMill";
	Locations[n].models.always.MillFan.locator.name ="Fan";
	Locations[n].models.always.MillFan.rotate.x = 0.0;
	Locations[n].models.always.MillFan.rotate.y = 0.0;
	Locations[n].models.always.MillFan.rotate.z = 1.2;

	//Day
	locations[n].models.day.charactersPatch = "Collector_patch";
	//Night
	locations[n].models.night.charactersPatch = "Collector_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "Pirates_Shipyard";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "0";

	locations[n].reload.l2.name = "reload2_back";
	locations[n].reload.l2.go = "Shore3";
	locations[n].reload.l2.emerge = "reload2";
	locations[n].reload.l2.autoreload = "1";
	locations[n].reload.l2.label = "Shore3";
	locations[n].locators_radius.reload.reload2_back = 1.5;
	n = n + 1;

	n = LocationInitTwoSisters(n);

	return n;
}

// Gregg: ==> локации квеста «Две сестры» (глава 10, перенос из К:ПС)
// Бриг «Церебус» капитана Касандера - четыре палубы. В К:ПС под них были свои модели
// локаторов (hold_l_lugger, deck2_l_lugger), которых в CSP нет; берём штатные hold_l
// и deck2_l. Из-за этого меняется и разметка переходов: у CSP свои имена дверей между
// палубами (reload_hold / reload_camp / reload_cabin), по ним цепочку и прошиваем.
// Драку в трюме и кают-компании глушим - там сюжетные сцены, оружия у ГГ ещё нет.
int LocationInitTwoSisters(int n)
{
	////////////////////////////////////////////////////////////
	// Трюм «Церебуса» - сюда ГГ приходит в себя после выстрела Касандера
	////////////////////////////////////////////////////////////
	Locations[n].id = "Cerebus_Hold";
	Locations[n].id.label = "Hold";
	Locations[n].filespath.models = "locations\decks\hold";
	Locations[n].image = "loading\jonny_load\inside ship\Hold.dds";
	Locations[n].type = "deck";
	//Models
	Locations[n].models.always.locators = "hold_l";
	Locations[n].models.always.l1 = "hold";
	Locations[n].models.always.l1.level = 65538;
	//Day
	Locations[n].models.day.charactersPatch = "hold_p";
	Locations[n].models.day.fonar = "hold_fd";
	//Night
	Locations[n].models.night.charactersPatch = "hold_p";
	Locations[n].models.night.fonar = "hold_fn";
	//Environment
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "false";
	Locations[n].environment.weather.rain = false;
	Locations[n].camshuttle = 1;
	LAi_LocationFightDisable(&Locations[n], true);
	// Gregg: ящик, в котором лежит шпилька. Время - как у типовых палуб CSP.
	Locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	Locations[n].locators_radius.box.box1 = 1.0;

	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Cerebus_WardRoom";
	Locations[n].reload.l1.emerge = "reload_hold";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "WardRoom";
	// Gregg: дверь заперта с самого начала - её и вскрывает шпилька. Именно disable,
	// а НЕ глобальный chrDisableReloadToLocation: с тем флагом гаснет пункт «поговорить
	// с собой» в командном меню (LandInterface.c:2059), а через него и запускается взлом.
	Locations[n].reload.l1.disable = 1;
	Locations[n].locators_radius.reload.reload1 = 2.0;
	n = n + 1;

	////////////////////////////////////////////////////////////
	// Кают-компания «Церебуса»
	////////////////////////////////////////////////////////////
	Locations[n].id = "Cerebus_WardRoom";
	Locations[n].id.label = "WardRoom";
	Locations[n].filespath.models = "locations\decks\camp_deck";
	Locations[n].image = "loading\jonny_load\inside ship\camp_deck.dds";
	Locations[n].type = "deck";
	//Models
	Locations[n].models.always.locators = "camp_deck_l";
	Locations[n].models.always.l1 = "camp_deck";
	Locations[n].models.always.l1.level = 65538;
	Locations[n].models.always.window = "camp_deck_w";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65539;
	//Day
	Locations[n].models.day.charactersPatch = "camp_deck_pd";
	Locations[n].models.day.fonar = "camp_deck_fd";
	//Night
	Locations[n].models.night.charactersPatch = "camp_deck_pn";
	Locations[n].models.night.fonar = "camp_deck_fn";
	//Environment
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather.rain = false;
	Locations[n].camshuttle = 1;
	LAi_LocationFightDisable(&Locations[n], true);
	// Gregg: в К:ПС сабля лежала на локаторе dagger, которого в CSP-модели нет.
	// Кладём её в ящик - там же, где и в оригинале её берут со стола.
	Locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	Locations[n].locators_radius.box.box1 = 1.0;

	Locations[n].reload.l1.name = "reload_hold";
	Locations[n].reload.l1.go = "Cerebus_Hold";
	Locations[n].reload.l1.emerge = "reload1";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "Hold";

	Locations[n].reload.l2.name = "reload2";
	Locations[n].reload.l2.go = "Cerebus_CannonDeck";
	Locations[n].reload.l2.emerge = "reload_camp";
	Locations[n].reload.l2.autoreload = "0";
	Locations[n].reload.l2.label = "CannonDeck";
	n = n + 1;

	////////////////////////////////////////////////////////////
	// Орудийная палуба «Церебуса» - тут ждёт патруль анимистов
	////////////////////////////////////////////////////////////
	Locations[n].id = "Cerebus_CannonDeck";
	Locations[n].id.label = "CannonDeck";
	Locations[n].filespath.models = "locations\decks\deck2";
	Locations[n].image = "loading\jonny_load\inside ship\Deck2.dds";
	Locations[n].type = "deck";
	//Models
	Locations[n].models.always.locators = "deck2_l";
	Locations[n].models.always.l1 = "deck2";
	Locations[n].models.always.l1.level = 65538;
	//Day
	Locations[n].models.day.charactersPatch = "deck2_p";
	Locations[n].models.day.fonar = "deck2_fd";
	//Night
	Locations[n].models.night.charactersPatch = "deck2_p";
	Locations[n].models.night.fonar = "deck2_fn";
	//Environment
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather.rain = false;
	Locations[n].camshuttle = 1;

	Locations[n].reload.l1.name = "reload_camp";
	Locations[n].reload.l1.go = "Cerebus_WardRoom";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "WardRoom";

	Locations[n].reload.l2.name = "reload_cabin";
	Locations[n].reload.l2.go = "Cerebus_Cabin";
	Locations[n].reload.l2.emerge = "reload1";
	Locations[n].reload.l2.autoreload = "0";
	Locations[n].reload.l2.label = "Cabin";
	Locations[n].locators_radius.reload.reload_cabin = 2.0;
	n = n + 1;

	////////////////////////////////////////////////////////////
	// Каюта Касандера
	////////////////////////////////////////////////////////////
	Locations[n].id = "Cerebus_Cabin";
	Locations[n].id.label = "Cabin";
	Locations[n].filespath.models = "locations\decks\capmd";
	Locations[n].image = "loading\jonny_load\inside ship\capmd.dds";
	Locations[n].type = "cap_cabine";
	//Models
	Locations[n].models.always.locators = "capmd_l";
	Locations[n].models.always.l1 = "capmd";
	Locations[n].models.always.l1.level = 65538;
	Locations[n].models.always.window = "capmd_w";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65539;
	//Day
	Locations[n].models.day.charactersPatch = "capmd_pd";
	Locations[n].models.day.fonar = "capmd_fd";
	//Night
	Locations[n].models.night.charactersPatch = "capmd_pn";
	Locations[n].models.night.fonar = "capmd_fn";
	//Environment
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather.rain = false;
	Locations[n].camshuttle = 1;
	// Gregg: сюда квест кладёт отобранные Касандером вещи ГГ - в оригинале их возвращали
	// сундуком в каюте.
	Locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	Locations[n].locators_radius.box.box1 = 1.0;

	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Cerebus_CannonDeck";
	Locations[n].reload.l1.emerge = "reload_cabin";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "CannonDeck";
	n = n + 1;

	////////////////////////////////////////////////////////////
	// Бриг Ортодоксе Малкина - ветка Натали Ремис на Бермудах.
	// Gregg: в К:ПС под неё переиспользовали палубы «Церебуса», но у нас ветки идут
	// параллельно и «Церебус» к тому времени может быть уже в эскадре игрока. Свои три
	// локации на тех же моделях - дешевле, чем разруливать пересечение двух эпизодов.
	////////////////////////////////////////////////////////////
	Locations[n].id = "Malkin_Cabin";
	Locations[n].id.label = "Cabin";
	Locations[n].filespath.models = "locations\decks\capmd";
	Locations[n].image = "loading\jonny_load\inside ship\capmd.dds";
	Locations[n].type = "cap_cabine";
	Locations[n].models.always.locators = "capmd_l";
	Locations[n].models.always.l1 = "capmd";
	Locations[n].models.always.l1.level = 65538;
	Locations[n].models.always.window = "capmd_w";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65539;
	Locations[n].models.day.charactersPatch = "capmd_pd";
	Locations[n].models.day.fonar = "capmd_fd";
	Locations[n].models.night.charactersPatch = "capmd_pn";
	Locations[n].models.night.fonar = "capmd_fn";
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather.rain = false;
	Locations[n].camshuttle = 1;

	// Gregg: в модели капитанской каюты reload-локатор ровно ОДИН (capmd_l), второго
	// перехода тут физически нет. Поэтому маршрут по бригу односторонний, как и в
	// оригинале К:ПС: каюта ведёт в кают-компанию, а наружу выпускает трюм - его дверь
	// квест перецепляет на посёлок, когда все трое подельников мертвы.
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Malkin_WardRoom";
	Locations[n].reload.l1.emerge = "reload1";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "WardRoom";
	n = n + 1;

	Locations[n].id = "Malkin_WardRoom";
	Locations[n].id.label = "WardRoom";
	Locations[n].filespath.models = "locations\decks\camp_deck";
	Locations[n].image = "loading\jonny_load\inside ship\camp_deck.dds";
	Locations[n].type = "deck";
	Locations[n].models.always.locators = "camp_deck_l";
	Locations[n].models.always.l1 = "camp_deck";
	Locations[n].models.always.l1.level = 65538;
	Locations[n].models.always.window = "camp_deck_w";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65539;
	Locations[n].models.day.charactersPatch = "camp_deck_pd";
	Locations[n].models.day.fonar = "camp_deck_fd";
	Locations[n].models.night.charactersPatch = "camp_deck_pn";
	Locations[n].models.night.fonar = "camp_deck_fn";
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather.rain = false;
	Locations[n].camshuttle = 1;

	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Malkin_Cabin";
	Locations[n].reload.l1.emerge = "rld";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "Cabin";

	Locations[n].reload.l2.name = "reload_hold";
	Locations[n].reload.l2.go = "Malkin_Hold";
	Locations[n].reload.l2.emerge = "reload1";
	Locations[n].reload.l2.autoreload = "0";
	Locations[n].reload.l2.label = "Hold";
	n = n + 1;

	Locations[n].id = "Malkin_Hold";
	Locations[n].id.label = "Hold";
	Locations[n].filespath.models = "locations\decks\hold";
	Locations[n].image = "loading\jonny_load\inside ship\Hold.dds";
	Locations[n].type = "deck";
	Locations[n].models.always.locators = "hold_l";
	Locations[n].models.always.l1 = "hold";
	Locations[n].models.always.l1.level = 65538;
	Locations[n].models.day.charactersPatch = "hold_p";
	Locations[n].models.day.fonar = "hold_fd";
	Locations[n].models.night.charactersPatch = "hold_p";
	Locations[n].models.night.fonar = "hold_fn";
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "false";
	Locations[n].environment.weather.rain = false;
	Locations[n].camshuttle = 1;
	Locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	Locations[n].locators_radius.box.box1 = 1.0;

	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Malkin_WardRoom";
	Locations[n].reload.l1.emerge = "reload_hold";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "WardRoom";
	n = n + 1;

	////////////////////////////////////////////////////////////
	// Берег Мухачи - сюда «Церебус» приходит по пути на Тортугу
	////////////////////////////////////////////////////////////
	locations[n].id = "Muhachi_Shore";
	locations[n].id.label = "Shore";
	locations[n].image = "loading\jonny_load\shores\Shore05.dds";
	locations[n].type = "seashore";
	locations[n].islandId = "Mein";
	locations[n].islandIdAreal = "Beliz";
	// Gregg: случайных встреч здесь быть не должно - весь берег занят сценой.
	locations[n].DisableEncounters = true;
	//Models
	locations[n].filespath.models = "locations\Outside\Shores\Shore05";
	Locations[n].models.always.shore05 = "shore05";
	locations[n].models.always.shore05.sea_reflection = 1;
	Locations[n].models.always.shore05seabed = "shore05_sb";
	Locations[n].models.always.locators = "shore05_locators";
	Locations[n].models.always.grassPatch = "shore05_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";
	Locations[n].models.always.l2 = "plan2";
	Locations[n].models.always.l2.level = 8;
	Locations[n].models.always.l2.tech = "LocationModelBlend";
	Locations[n].models.always.l3 = "plan3";
	Locations[n].models.always.l3.level = 7;
	Locations[n].models.always.l3.tech = "LocationModelBlend";
	//Day
	locations[n].models.day.charactersPatch = "shore05_patch";
	//Night
	locations[n].models.night.charactersPatch = "shore05_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	//Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Muhachi_Jungle_01";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].reload.l1.label = "Jungle";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "boat";
	locations[n].reload.l2.go = "Beliz";
	locations[n].reload.l2.emerge = "reload_5";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Sea";
	// Gregg: шлюпка заперта. Берега Мухачи нет на карте мира, и ушедший в море игрок
	// оказывался в заливе Гибели без дороги назад - с Мишель, оставшейся в джунглях.
	// Квест открывает выход только тогда, когда возвращаться уже есть с чем.
	locations[n].reload.l2.disable = 1;
	locations[n].locators_radius.reload.boat = 9.0;
	n = n + 1;

	////////////////////////////////////////////////////////////
	// Джунгли по дороге к лагерю Мухачи
	////////////////////////////////////////////////////////////
	locations[n].id = "Muhachi_Jungle_01";
	locations[n].id.label = "Jungle";
	locations[n].image = "loading\jonny_load\jungle\Jungle8.dds";
	locations[n].type = "jungle";
	locations[n].islandId = "Mein";
	locations[n].islandIdAreal = "Beliz";
	locations[n].DisableEncounters = true;
	//Models
	locations[n].filespath.models = "locations\Outside\Jungles\jungle8";
	Locations[n].models.always.jungle = "jungle8";
	Locations[n].models.always.locators = "jungle8_locators";
	Locations[n].models.always.grassPatch = "jungle8_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";
	Locations[n].models.always.l2 = "plan2";
	Locations[n].models.always.l2.level = 8;
	Locations[n].models.always.l2.tech = "LocationModelBlend";
	Locations[n].models.always.l3 = "plan3";
	Locations[n].models.always.l3.level = 7;
	Locations[n].models.always.l3.tech = "LocationModelBlend";
	//Day
	locations[n].models.day.charactersPatch = "jungle8_patch";
	//Night
	locations[n].models.night.charactersPatch = "jungle8_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Muhachi_Shore";
	locations[n].reload.l1.emerge = "reload1_back";
	locations[n].reload.l1.autoreload = "1";
	locations[n].reload.l1.label = "Shore";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2_back";
	locations[n].reload.l2.go = "Muhachi_Jungle_02";
	locations[n].reload.l2.emerge = "reload1_back";
	locations[n].reload.l2.autoreload = "1";
	locations[n].reload.l2.label = "Jungle";
	locations[n].locators_radius.reload.reload2_back = 2.0;
	n = n + 1;

	////////////////////////////////////////////////////////////
	// Джунгли, второй перегон
	////////////////////////////////////////////////////////////
	locations[n].id = "Muhachi_Jungle_02";
	locations[n].id.label = "Jungle";
	locations[n].image = "loading\jonny_load\jungle\Jungle1.dds";
	locations[n].type = "jungle";
	locations[n].islandId = "Mein";
	locations[n].islandIdAreal = "Beliz";
	locations[n].DisableEncounters = true;
	//Models
	locations[n].filespath.models = "locations\Outside\Jungles\jungle1";
	Locations[n].models.always.jungle = "jungle1";
	Locations[n].models.always.locators = "jungle1_locators";
	Locations[n].models.always.grassPatch = "jungle1_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";
	Locations[n].models.always.l2 = "plan2";
	Locations[n].models.always.l2.level = 8;
	Locations[n].models.always.l2.tech = "LocationModelBlend";
	Locations[n].models.always.l3 = "plan3";
	Locations[n].models.always.l3.level = 7;
	Locations[n].models.always.l3.tech = "LocationModelBlend";
	//Day
	locations[n].models.day.charactersPatch = "jungle1_patch";
	//Night
	locations[n].models.night.charactersPatch = "jungle1_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Muhachi_Jungle_01";
	locations[n].reload.l1.emerge = "reload2_back";
	locations[n].reload.l1.autoreload = "1";
	locations[n].reload.l1.label = "Jungle";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2_back";
	locations[n].reload.l2.go = "Muhachi_Jungle_03";
	locations[n].reload.l2.emerge = "reload2_back";
	locations[n].reload.l2.autoreload = "1";
	locations[n].reload.l2.label = "Jungle";
	locations[n].locators_radius.reload.reload2_back = 2.0;
	n = n + 1;

	////////////////////////////////////////////////////////////
	// Лагерь Мухачи. Модель Jungle_2 в CSP не занята ни одной локацией,
	// поэтому место не спутать с бесчисленными джунглями Мейна.
	////////////////////////////////////////////////////////////
	locations[n].id = "Muhachi_Jungle_03";
	locations[n].id.label = "Jungle";
	locations[n].image = "loading\jonny_load\jungle\Jungle2.dds";
	locations[n].type = "jungle";
	locations[n].islandId = "Mein";
	locations[n].islandIdAreal = "Beliz";
	locations[n].DisableEncounters = true;
	//Models
	Locations[n].filespath.models = "locations\Outside\Jungle_2";
	Locations[n].models.always.locators = "Jungle02_l";
	Locations[n].models.always.jungle = "Jungle02";
	Locations[n].models.always.grassPatch = "JUNGLE02_g";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	Locations[n].models.always.l1 = "plan_1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";
	Locations[n].models.always.l2 = "plan_2";
	Locations[n].models.always.l2.level = 8;
	Locations[n].models.always.l2.tech = "LocationModelBlend";
	Locations[n].models.always.l3 = "plan_3";
	Locations[n].models.always.l3.level = 7;
	Locations[n].models.always.l3.tech = "LocationModelBlend";
	//Day
	Locations[n].models.day.charactersPatch = "Jungle02_p";
	//Night
	Locations[n].models.night.charactersPatch = "Jungle02_p";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload2_back";
	locations[n].reload.l1.go = "Muhachi_Jungle_02";
	locations[n].reload.l1.emerge = "reload2_back";
	locations[n].reload.l1.autoreload = "1";
	locations[n].reload.l1.label = "Jungle";
	locations[n].locators_radius.reload.reload2_back = 2.0;
	n = n + 1;

	return n;
}
// Gregg: <== локации квеста «Две сестры»
