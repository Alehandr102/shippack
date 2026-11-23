// Gregg: ==> Альтернативный пролог «Необитаемый остров» (перенос из К:ПС, глава 1 «Начало пути»)
//
// Источник: К:ПС, PROGRAM\lugger\MainLine.c, раздел «1. НАЧАЛО ПУТИ».
// Имена функций сохранены как в оригинале, чтобы можно было сверяться с источником.
//
// Сюжет: капитана предал старый приятель Грифо Сайфус - расстрелял его шебеку картечью
// и бросил команду умирать у Доминики. ГГ приходит в себя на берегу без памяти, находит
// двух выживших, отсиживается в гроте, забирает чужой схрон и отбивает у мошенника люггер.
//
// Отличия от оригинала (подробности - в комментариях по коду):
// - выброшены механики, которых в CSP нет: DigTreasure с киркой и локаторами dig,
//   «письмо от разработчиков», открытие арены (у CSP свой StartGameOpenArena);
// - НЕ переписываются чужие персонажи. В К:ПС пролог перекрашивал слоты Эдварда Мэнсфилда
//   и Ричарда Соукинса под злодеев поздних глав; в CSP это живые сюжетные лица;
// - сцена в гроте переставлена с goto3/goto4 на точки внутри патча ходьбы.

// Gregg: точки укрытия в гроте. В К:ПС прятались на goto3/goto4, но в патче ходьбы CSP
// под этими локаторами нет ни одного треугольника - персонаж встаёт вне проходимой области,
// маршрут к нему не строится и приказ идти вырождается в телепорт (см. LAi_runto.c).
// Проверено расчётом по grotto2_patch.ptc: из группы goto на патче лежат только
// goto1, goto2, goto5 и goto6, а вся группа monsters - целиком на патче.
// Прячемся в дальней части грота (z ~ 6.6..8.6), пираты остаются у входа (z ~ 0.4..1.3).
#define DDP_HIDE_PLAYER_GROUP	"monsters"
#define DDP_HIDE_PLAYER_LOC		"monster3"
#define DDP_HIDE_JAMES_GROUP	"monsters"
#define DDP_HIDE_JAMES_LOC		"monster4"
// Камера сцены в гроте: точка съёмки - локатор группы camera, направление взгляда - его ось Z
//   (крутишь локатор в редакторе - едет и кадр). Дистанция нужна лишь чтобы отложить точку взгляда.
#define DDP_GROT_CAM_LOCATOR	"cutscene_1"
#define DDP_GROT_CAM_LOOK_DIST	10.0

#define DDP_HIDE_ANDY_GROUP		"goto"
#define DDP_HIDE_ANDY_LOC		"goto2"

void StartDamnedDestiny(string qName)
{
	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	Pchar.questTemp.CapBloodLine = false;

	// Gregg: reload.l2 на обоих берегах Доминики - выход в море («boat»). Пока идёт пролог,
	// уплыть с острова нельзя: корабля нет, а вернуть игрока будет некуда.
	Locations[FindLocation("Shore26")].reload.l2.disable = 1;
	Locations[FindLocation("Shore27")].reload.l2.disable = 1;

	bDisableCharacterMenu = true;
	bDisableQuestInterface = true;

	ChangeCharacterAddressGroup(PChar, PChar.location, "goto", "goto25");

	LAi_SetActorType(PChar);
	LAi_ActorSetLayMode(PChar);

	SendMessage(&locCamera, "lfffl", MSG_CAMERA_TOPOS, 18, 6.0, 45, false);

	DoQuestFunctionDelay("_StartDamnedDestiny", 5.0);
}

void _StartDamnedDestiny(string _tmp)
{
	ref chr = GetCharacter(NPC_GenerateCharacter("James Robinson", "officer_13", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("DDP_JamesName");
	chr.lastname = iTxt("DDP_JamesLastName");
	chr.greeting = "Gr_AllowToDump";
	chr.Dialog.FileName = "Quest\KPS\DDPrologue\DDPrologue_James.c";
	chr.Dialog.CurrentNode = "Start_1";
	chr.Skill.Accuracy = 25 + rand(5);
	chr.Skill.Cannons = 20 + rand(10);
	chr.perks.list.FastReload = true;
	chr.perks.list.SailsDamageUp = true;
	chr.loyality = MAX_LOYALITY;
	chr.OfficerWantToGo.DontGo = true;
	DeleteAttribute(chr, "items");
	chr.items = "";

	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");

	LAi_SetActorType(chr);
	LAi_ActorWaitDialog(PChar, chr);
	LAi_ActorDialog(chr, PChar, "", 15.0, 1.0);
}

void StartDamnedDestinyStartFade()
{
	LAi_Fade("", "");

	ref chr = CharacterFromID("James Robinson");
	chr.Dialog.CurrentNode = "Start_4";

	SetDialogCharacter2Character(PChar, chr, 10.0, true);
}

void StartGameGoToGrot()
{
	locCameraFollow();

	PChar.StartDamnedDestiny = "1";

	EndQuestMovie();

	bDisableCharacterMenu = false;
	bDisableQuestInterface = false;
	InterfaceStates.Buttons.Save.enable = true;

	LAi_SetActorType(CharacterFromID("James Robinson"));
	LAi_ActorFollowEverywhere(CharacterFromID("James Robinson"), "", -1);

	SetQuestHeader("StartDamnedDestiny");
	AddQuestRecord("StartDamnedDestiny", "1");

	LAi_SetPlayerType(PChar);

	PChar.quest.StartGameInGrot.win_condition.l1 = "location";
	PChar.quest.StartGameInGrot.win_condition.l1.location = "Dominica_Grot";
	PChar.quest.StartGameInGrot.function = "StartGameInGrot";

	// Gregg: чужой схрон в гроте. Условие победы в StartGameSearchedItems перечисляет
	// тот же набор, кроме пороха - см. пояснение у него.
	PChar.GenQuestBox.Dominica_Grot.box1.money = 3000;
	PChar.GenQuestBox.Dominica_Grot.box1.items.jewelry12 = 10;
	PChar.GenQuestBox.Dominica_Grot.box1.items.blade2 = 1;
	// Gregg: в К:ПС тут лежал blade1 «Канонирский тесак» - единственный ordinary среди двух
	// poor, вчетверо выше шпаги по нижней границе урона. Выбор в сундуке был предрешён,
	// поэтому заменён на blade09 «Абордажный тесак»: то же poor, тот же вес и роль.
	PChar.GenQuestBox.Dominica_Grot.box1.items.blade09 = 1;
	PChar.GenQuestBox.Dominica_Grot.box1.items.blade10 = 1;
	PChar.GenQuestBox.Dominica_Grot.box1.items.pistol1 = 1;
	PChar.GenQuestBox.Dominica_Grot.box1.items.bullet = 10;
	// Gregg: без пороха пистолет из этого же сундука бесполезен. В условие победы ниже
	// порох НЕ входит: чем меньше обязательных к выносу позиций, тем меньше способов
	// подвесить этап, если игрок упрётся в переносимый вес.
	PChar.GenQuestBox.Dominica_Grot.box1.items.GunPowder = 10;
	PChar.GenQuestBox.Dominica_Grot.box1.items.potion1 = 5;
	PChar.GenQuestBox.Dominica_Grot.box1.items.food1 = 7;
	PChar.GenQuestBox.Dominica_Grot.box1.items.jewelry9 = 1;
}

void StartGameInGrot(string qName)
{
	PChar.StartDamnedDestiny = "2";

	StartQuestMovie(true, true, true);
	InterfaceStates.Buttons.Save.enable = false;

	ref chr = GetCharacter(NPC_GenerateCharacter("Andy Rejdol", "officer_15", "man", "man", 1, PIRATE, -1, false));
	chr.name = iTxt("DDP_AndyName");
	chr.lastname = iTxt("DDP_AndyLastName");
	chr.greeting = "Gr_AllowToDump";
	chr.Dialog.FileName = "Quest\KPS\DDPrologue\DDPrologue_Andy.c";
	chr.Dialog.CurrentNode = "Start_1";
	chr.Skill.Grappling = 20 + rand(10);
	chr.Skill.Defence = 10 + rand(20);
	// Gregg: боцману брандер практически бесполезен - даём то, что работает
	// на абордажной роли: лечение потерь и увеличенная дистанция сцепки.
	chr.perks.list.Doctor1 = true;
	chr.perks.list.LongRangeGrappling = true;
	chr.loyality = MAX_LOYALITY;
	chr.OfficerWantToGo.DontGo = true;

	DeleteAttribute(chr, "items");
	chr.items = "";

	LAi_SetImmortal(chr, true);
	// Gregg: в К:ПС адрес ставился дважды подряд, goto5 и сразу goto6 - первая строка
	// не имела смысла. Оставлена только goto6, обе точки на патче.
	ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto6");

	SetDialogCharacter2Character(PChar, chr, 2.5, true);
}

void StartGameEnemyInGrot()
{
	LAi_SetActorType(CharacterFromID("James Robinson"));
	LAi_SetActorType(CharacterFromID("Andy Rejdol"));
	LAi_SetActorType(PChar);

	LAi_ActorRunToLocator(CharacterFromID("James Robinson"), DDP_HIDE_JAMES_GROUP, DDP_HIDE_JAMES_LOC, "", 0.0);
	LAi_ActorRunToLocator(CharacterFromID("Andy Rejdol"), DDP_HIDE_ANDY_GROUP, DDP_HIDE_ANDY_LOC, "", 0.0);
	// Gregg: квест шаблона актёра приходит в QuestComplete с ПУСТЫМ qname, поэтому
	// обработчик регистрируется по имени самого квеста (см. дополнение в quests_reaction.c)
	PChar.quest.DDPrologue_Hidden.function = "StartGameEnemyInGrotInit";
	LAi_ActorGoToLocator(PChar, DDP_HIDE_PLAYER_GROUP, DDP_HIDE_PLAYER_LOC, "DDPrologue_Hidden", 20.0);
}

void StartGameEnemyInGrotInit(string qName)
{
	LAi_SetActorType(CharacterFromID("James Robinson"));
	LAi_SetActorType(CharacterFromID("Andy Rejdol"));
	LAi_SetActorType(PChar);

	// Gregg: все трое смотрят в сторону входа - оттуда сейчас спустятся люди Сайфуса
	LAi_ActorTurnToLocator(CharacterFromID("James Robinson"), "monsters", "monster2");
	LAi_ActorTurnToLocator(CharacterFromID("Andy Rejdol"), "monsters", "monster2");
	LAi_ActorTurnToLocator(PChar, "monsters", "monster2");

	// LEO: камеру ставим ИЗ точки В точку, а не «в точке, смотреть на героя»: locCameraToPos
	//   разворачивает камеру на ГГ, и поворот локатора на кадр не влиял вовсе.
	float x, y, z;
	string sCamLoc = DDP_GROT_CAM_LOCATOR;   // в (...) обращении Storm ждёт переменную, не дефайн
	if( CheckAttribute(loadedLocation, "locators.camera." + sCamLoc + ".x") )
	{
		aref arCam;
		makearef(arCam, loadedLocation.locators.camera.(sCamLoc));
		x = stf(arCam.x);
		y = stf(arCam.y);
		z = stf(arCam.z);
		// точка взгляда - вдоль оси Z локатора
		float tx = x + stf(arCam.vz.x) * DDP_GROT_CAM_LOOK_DIST;
		float ty = y + stf(arCam.vz.y) * DDP_GROT_CAM_LOOK_DIST;
		float tz = z + stf(arCam.vz.z) * DDP_GROT_CAM_LOOK_DIST;
		locCameraFromToPos(x, y, z, false, tx, ty, tz);
	}
	else
	{
		// на модели без этого локатора остаётся прежний расчёт от героя
		GetCharacterPos(PChar, &x, &y, &z);
		y += 2;
		z -= 6;
		x += 6;
		locCameraToPos(x, y, z, false);
	}

	ref chr = GetCharacter(NPC_GenerateCharacter("Pierre Arnu", "officer_1", "man", "man", 1, PIRATE, 0, false));
	chr.name = iTxt("DDP_PierreName");
	chr.lastname = iTxt("DDP_PierreLastName");
	chr.Dialog.FileName = "Quest\KPS\DDPrologue\DDPrologue_Pirates.c";
	chr.Dialog.CurrentNode = "1";
	chr.greeting = "Gr_AllowToDump";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");

	LAi_SetActorType(chr);
	LAi_ActorGoToLocator(chr, "goto", "goto5", "", 0.0);

	chr = GetCharacter(NPC_GenerateCharacter("Ren Matezh", "officer_19", "man", "man", 1, PIRATE, 0, false));
	chr.name = iTxt("DDP_ReneName");
	chr.lastname = iTxt("DDP_ReneLastName");
	chr.Dialog.FileName = "Quest\KPS\DDPrologue\DDPrologue_Pirates.c";
	chr.Dialog.CurrentNode = "1";
	LAi_SetImmortal(chr, true);
	ChangeCharacterAddressGroup(chr, PChar.location, "reload", "reload1_back");

	LAi_SetActorType(chr);
	PChar.quest.DDPrologue_PiratesTalk.function = "StartGameEnemyInGrotDialog";
	LAi_ActorGoToLocator(chr, "goto", "goto6", "DDPrologue_PiratesTalk", 10.0);
}

void StartGameEnemyInGrotDialog(string qName)
{
	LAi_SetActorType(CharacterFromID("Pierre Arnu"));
	LAi_SetActorType(CharacterFromID("Ren Matezh"));
	LAi_ActorTurnToCharacter(CharacterFromID("Pierre Arnu"), CharacterFromID("Ren Matezh"));
	LAi_ActorTurnToCharacter(CharacterFromID("Ren Matezh"), CharacterFromID("Pierre Arnu"));

	SetDialogCharacter2Character(PChar, CharacterFromID("Pierre Arnu"), 0.1, true);
}

void StartGameEnemyInGrotExit()
{
	LAi_SetActorType(CharacterFromID("Pierre Arnu"));
	LAi_SetActorType(CharacterFromID("Ren Matezh"));

	// Gregg: таймаут конечный, а не -1. Выход reload1_back лежит на патче и маршрут
	// обычно строится, но при провале LAi_tmpl_runto_Teleport спасает только при
	// timeout >= 0; с -1 шаблон встаёт в "falure" и пират застывает посреди сцены.
	LAi_ActorGoToLocation(CharacterFromID("Pierre Arnu"), "reload", "reload1_back", "none", "", "", "", 8.0);
	LAi_ActorGoToLocation(CharacterFromID("Ren Matezh"), "reload", "reload1_back", "none", "", "", "", 8.0);

	DoQuestFunctionDelay("StartGameEnemyInGrotNewDialog", 5.0);
}

void StartGameEnemyInGrotNewDialog(string _tmp)
{
	EndQuestMovie();
	InterfaceStates.Buttons.Save.enable = true;

	LAi_Fade("", "");

	locCameraFollow();

	ref chr = CharacterFromID("Andy Rejdol");
	chr.Dialog.CurrentNode = "Start_8";
	SetDialogCharacter2Character(PChar, chr, 2.5, true);

	chrDisableReloadToLocation = true;
}

void StartGameSearchedItems()
{
	PChar.StartDamnedDestiny = "3";
	LAi_SetPlayerType(PChar);

	string sItem = "";
	int iQty = 0;
	string cnd = "";

	// Gregg: счётчик локальный, а не общий i - внутри цикла идёт работа с атрибутами
	int iSlot;
	for (iSlot = 1; iSlot <= 9; iSlot++)
	{
		switch (iSlot)
		{
			case 1: sItem = "jewelry12"; iQty = 10; break;
			case 2: sItem = "blade2"; iQty = 1; break;
			case 3: sItem = "blade09"; iQty = 1; break;
			case 4: sItem = "blade10"; iQty = 1; break;
			case 5: sItem = "pistol1"; iQty = 1; break;
			case 6: sItem = "bullet"; iQty = 10; break;
			case 7: sItem = "potion1"; iQty = 5; break;
			case 8: sItem = "food1"; iQty = 7; break;
			case 9: sItem = "jewelry9"; iQty = 1; break;
		}

		cnd = "l" + iSlot;
		PChar.quest.StartGameSearchedItemsGood.win_condition.(cnd) = "item_qty";
		PChar.quest.StartGameSearchedItemsGood.win_condition.(cnd).item = sItem;
		PChar.quest.StartGameSearchedItemsGood.win_condition.(cnd).item.quantity = iQty;
	}

	// Gregg: деньги отдельным условием. В CSP это "Money" с операцией, а не голое число,
	// как в К:ПС - см. quests_check.c, TestIntValue.
	PChar.quest.StartGameSearchedItemsGood.win_condition.l10 = "Money";
	PChar.quest.StartGameSearchedItemsGood.win_condition.l10.value = 3000;
	PChar.quest.StartGameSearchedItemsGood.win_condition.l10.operation = ">=";

	PChar.quest.StartGameSearchedItemsGood.function = "StartGameSearchedItemsGood";
}

void StartGameSearchedItemsGood(string qName)
{
	bDisableCharacterMenu = true;
	bDisableQuestInterface = true;

	ref chr = CharacterFromID("Andy Rejdol");
	chr.Dialog.CurrentNode = "Start_11";
	SetDialogCharacter2Character(PChar, chr, 3.0, true);
}

void StartGameSearchedItemsEnd()
{
	LAi_type_actor_Reset(CharacterFromID("James Robinson"));
	LAi_type_actor_Reset(CharacterFromID("Andy Rejdol"));

	ref chr = CharacterFromID("James Robinson");
	chr.Dialog.FileName = "Enc_Officer_dialog.c";
	chr.Dialog.CurrentNode = "hired";
	chr.greeting = "Gr_Officer";
	LAi_SetImmortal(chr, false);
	AddPassenger(PChar, chr, false);
	chr.Money = 0;
	chr.location = "None";
	chr.DontClearDead = true;
	DeleteAttribute(chr, "LifeDay");

	chr = CharacterFromID("Andy Rejdol");
	chr.Dialog.FileName = "Enc_Officer_dialog.c";
	chr.Dialog.CurrentNode = "hired";
	chr.greeting = "Gr_Officer";
	LAi_SetImmortal(chr, false);
	AddPassenger(PChar, chr, false);
	chr.Money = 0;
	chr.location = "None";
	chr.DontClearDead = true;
	// Gregg: в К:ПС здесь по опечатке второй раз чистился LifeDay у Джеймса,
	// а у Энди он оставался - и боцман исчезал через сутки.
	DeleteAttribute(chr, "LifeDay");
}

void StartGameNextDay(string _tmp)
{
	// Gregg: в К:ПС продолжение вешалось на квест-имя таблички и ловилось кейсом в свитче.
	// В CSP для этого есть штатная пара Run_Function + SetLaunchFrameRunFunctionParam.
	SetLaunchFrameFormParam(iTxt("DDP_ThreeDaysLater"), "Run_Function", 3.0, 3.0);
	SetLaunchFrameRunFunctionParam("StartGameNextDayDialog", 1.0);
	LaunchFrameForm();
}

void StartGameNextDayDialog()
{
	WaitDate("", 0, 0, 3, 0, 0);
	SetCurrentTime(6, 30);
	RecalculateJumpTable();
	RefreshWeather();
	RefreshLandTime();

	ref chr = CharacterFromID("James Robinson");
	chr.Dialog.CurrentNode = "Start_9";
	SetDialogCharacter2Character(PChar, chr, 3.0, true);
}

void StartGameNextDayToLugger()
{
	PChar.StartDamnedDestiny = "4";

	StartGameSearchedItemsEnd();

	bDisableCharacterMenu = false;
	bDisableQuestInterface = false;
	chrDisableReloadToLocation = false;

	AddQuestRecord("StartDamnedDestiny", "2");

	LAi_SetPlayerType(PChar);

	// Gregg: в К:ПС он генерился 15-м рангом, а NPC_GenerateCharacter прогоняет ранг через
	// SetFantomParamFromRank -> SetFantomHP, и получался боец за 250 HP против ГГ 1-го ранга,
	// который дерётся один: Джеймс и Энди к этому моменту уже пассажиры и в локации их нет.
	// Делаем мошенника мошенником: скромный ранг и фиксированные HP по сложностям.
	ref chr = GetCharacter(NPC_GenerateCharacter("Tommi Fatman", "trader_11", "man", "man", 3, PIRATE, -1, true));
	int iFatHP = 100;
	if (MOD_SKILL_ENEMY_RATE == SAILOR_DIFFICULTY) iFatHP = 80;
	if (MOD_SKILL_ENEMY_RATE == ADMIRAL_DIFFICULTY) iFatHP = 125;
	chr.chr_ai.hp = iFatHP;
	chr.chr_ai.hp_max = iFatHP;
	chr.name = iTxt("DDP_TommiName");
	chr.lastname = iTxt("DDP_TommiLastName");
	chr.Dialog.FileName = "Quest\KPS\DDPrologue\DDPrologue_Lugger.c";
	chr.Dialog.CurrentNode = "1";
	chr.SaveItemsForDead = true;
	chr.money = 2500;
	TakeNItems(chr, "jewelry3", 5);
	TakeNItems(chr, "jewelry8", 1);
	// Gregg: самая дешёвая труба (zoom 2.5) - ГГ раздет подчистую, а без трубы в море слепой.
	// Достаётся с трупа: у Томми стоит SaveItemsForDead.
	TakeNItems(chr, "spyglass1", 1);
	SetFoodToCharacter(chr, 2, 15);
	LAi_SetImmortal(chr, true);
	chr.ship.type = GenerateShip(SHIP_LUGGER, 0);
	LAi_SetStayType(chr);
	SetCharacterShipLocation(chr, "Shore26");

	ref rShip = GetRealShip(sti(chr.ship.type));
	rShip.ship.upgrades.hull = 3;

	chr = GetCharacter(NPC_GenerateCharacter("Walter Korten", "officer_6", "man", "man", 5, PIRATE, -1, true));
	chr.name = iTxt("DDP_WalterName");
	chr.lastname = iTxt("DDP_WalterLastName");
	chr.Dialog.FileName = "Quest\KPS\DDPrologue\DDPrologue_Lugger.c";
	chr.Dialog.CurrentNode = "First time";
	chr.greeting = "Gr_AllowToDump";
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);

	chr = GetCharacter(NPC_GenerateCharacter("Johny Norton", "officer_12", "man", "man", 5, PIRATE, -1, true));
	chr.name = iTxt("DDP_JohnyName");
	chr.lastname = iTxt("DDP_JohnyLastName");
	chr.Dialog.FileName = "Quest\KPS\DDPrologue\DDPrologue_Lugger.c";
	chr.Dialog.CurrentNode = "First time";
	chr.greeting = "Gr_AllowToDump";
	LAi_SetStayType(chr);
	LAi_SetImmortal(chr, true);

	PChar.quest.StartGameLuggerCapDialog.win_condition.l1 = "location";
	PChar.quest.StartGameLuggerCapDialog.win_condition.l1.location = "Shore26";
	PChar.quest.StartGameLuggerCapDialog.function = "StartGameLuggerCapDialog";
}

void StartGameLuggerCapDialog(string qName)
{
	chrDisableReloadToLocation = true;

	ChangeCharacterAddressGroup(CharacterFromID("Tommi Fatman"), PChar.location, "goto", "goto1");
	ChangeCharacterAddressGroup(CharacterFromID("Walter Korten"), PChar.location, "goto", "goto2");
	ChangeCharacterAddressGroup(CharacterFromID("Johny Norton"), PChar.location, "goto", "goto4");
}

void StartGameLuggerCapBattle()
{
	SetCharacterShipLocation(CharacterFromID("Tommi Fatman"), "None");

	LAi_SetImmortal(CharacterFromID("Tommi Fatman"), false);
	LAi_group_MoveCharacter(CharacterFromID("Tommi Fatman"), "EnemyFight");
	LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
	// Gregg: ГГ выхватывает клинок сам - иначе первый удар прилетает по убранному оружию
	LAi_SetFightMode(PChar, true);

	PChar.quest.StartGameLuggerCapBattleWin.win_condition.l1 = "NPC_Death";
	PChar.quest.StartGameLuggerCapBattleWin.win_condition.l1.character = "Tommi Fatman";
	PChar.quest.StartGameLuggerCapBattleWin.function = "StartGameLuggerCapBattleWin";
}

void StartGameLuggerCapBattleWin(string qName)
{
	chrDisableReloadToLocation = false;

	ref chr = CharacterFromID("Johny Norton");
	chr.Dialog.CurrentNode = "14";
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

	chr = CharacterFromID("Walter Korten");
	chr.Dialog.CurrentNode = "7";
	LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);
	SetDialogCharacter2Character(PChar, chr, 30.0, true);
}

void StartGameHappyEnd()
{
	LAi_SetPlayerType(PChar);

	ref chr = CharacterFromID("Walter Korten");
	chr.Dialog.CurrentNode = "14";
	chr.LifeDay = 0;

	chr = CharacterFromID("Johny Norton");
	chr.LifeDay = 0;

	PChar.ship.type = GenerateShip(SHIP_LUGGER, 0);
	SetCharacterShipLocation(PChar, "Shore26");

	ref rShip = GetRealShip(sti(PChar.ship.type));
	rShip.ship.upgrades.hull = 3;

	SetBaseShipData(PChar);

	// Gregg: люггер достаётся битым - предыдущий капитан водил его на галеон
	int iHp = sti(PChar.Ship.HP) / 100;
	iHp = iHp * 65;

	PChar.Ship.Name = iTxt("DDP_ShipName");
	PChar.ship.SP = sti(PChar.Ship.SP) - 20;
	PChar.ship.HP = iHp;
	SetCrewQuantity(PChar, GetMinCrewQuantity(PChar));

	// Gregg: тот самый груз какао для Ричарда Тилльмана, о котором говорит команда люггера
	AddCharacterGoodsSimple(PChar, GOOD_CHOCOLATE, 50);

	Locations[FindLocation("Shore26")].reload.l2.disable = 0;
	Locations[FindLocation("Shore27")].reload.l2.disable = 0;

	PChar.StartDamnedDestiny = "END";
	AddQuestRecord("StartDamnedDestiny", "3");
	CloseQuestHeader("StartDamnedDestiny");
	pchar.questdata.StartDamnedDestiny = 1;	// LEO: отметка в энциклопедии

	setWDMPointXZ("Shore26");

	AddSimpleRumourToAllNations(iTxt("DDP_Rumour1") + GetFullName(PChar) + iTxt("DDP_Rumour2"), 5, 1);

	// Gregg: обязательство доставить какао - это и есть завязка «Товара для Ричарда
	// Тилльманна», о ней прямо говорит команда люггера. Вызов появился позже пролога:
	// пока квест не был перенесён, ГГ уплывал с грузом, но без поручения, и потом брал
	// его заново у трактирщика - по ветке «мимо пролога», без всякого Толстяка.
	UneasyDeliveryStart();

	// Gregg: «Предательство Грифо Сайфуса» в К:ПС стартует здесь же и от «Товара»
	// не зависит: обе главы просто выдаются финалом пролога. Сайфуса ГГ знает лично -
	// это он увёл «Пиранью» у Доминики, - поэтому прологовый вход идёт без флага
	// NoPiranha, в отличие от входа через трактирщика.
	TreacheryOfGrifoSajfusStart();

	// Gregg: шлюпка у берега создаётся ТОЛЬКО при загрузке локации, а корабль игрок получает
	// уже стоя на берегу - без неё локатор выхода в море не виден. Раньше здесь стояла
	// перезагрузка берега, но она уносила труп Томми вместе с лутом; создаём лодку на месте.
	LocCreatePlayerBoatNow();
}
// <== Gregg: альтернативный пролог «Необитаемый остров»
