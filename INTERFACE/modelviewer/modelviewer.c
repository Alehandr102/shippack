string currentWindow = "";
Object real_ship_model;
Object fake_ship;
Object fake_sail;
Object fake_rope;
Object fake_flag;
ref mv_chref;
ref mv_shref;

int whrBckpColor = 0;
int whrBckpAmbient = 0;

string modelViewerID = "MODEL_VIEWER";

void InitModelViewer(string viewerID) {
    modelViewerID = viewerID;
	setLighting();
    configureModelViewerOptions(true);
}

void ExitModelViewer() {
	resetLighting();
    emptyModelViewer();
}

void configureModelViewerOptions(bool isShip) {
    if (isShip) {		
        SendMessage(&GameInterface, "lslffffffffffff", MSG_INTERFACE_MSG_TO_NODE, modelViewerID, 1, 
		/*начальный aX камеры*/ PI*0.7,
		/*базовый aY камеры (PI = вид строго сбоку)*/ PI,
		/* макс амплитуда камеры aY */ PI / 8,
		/* гориз. сенса */ 0.2,
		/* верт. сенса */ 0.4,
		/*множитель инерции*/ 1.0,
		/*множитель мин. дистанции*/ 0.3,
		/*множитель макс. дистанции*/ 1.7,
		/*множитель шага зума*/ 0.1,
		/*множитель начальной высоты объекта относительно половинной высоты бокса (1.0 - центр) */ 0.6,
		/*множитель мин. высоты объекта относительно половинной высоты бокса*/ 0.4, 
		/*множитель макс. высоты объекта относительно половинной высоты бокса*/ 1.1
		);
    } else {
		SendMessage(&GameInterface, "lslffffffffffff", MSG_INTERFACE_MSG_TO_NODE, modelViewerID, 1,
		PI*0.7, PI, PI / 8, 0.2, 0.4, 1.0, 0.5, 1.7, 0.1, 0.6, 0.4, 1.1
		);
    }
}

void resolveModelViewerShip(int hullID) {
	if (!IsEntity(fake_ship))
    	CreateEntity(&fake_ship, "ship");
	if (!IsEntity(fake_sail))
    	CreateEntity(&fake_sail, "sail");
	if (!IsEntity(fake_rope))
    	CreateEntity(&fake_rope, "rope");
	if (!IsEntity(fake_flag))
		CreateEntity(&fake_flag, "flag");

	SendMessage(&fake_ship, "la", MSG_SHIP_SET_CHAR, &mv_chref);
	
	SetTexturePath(2, "Ships/"+mv_shref.name+"/Hull"+hullID+"/");
	SetTexturePath(1, "Ships/"+mv_shref.name+"/");
	SetTexturePath(0, "");

	if (!IsEntity(real_ship_model))
    	CreateEntity(&real_ship_model, "modelr");

	SendMessage(&GameInterface, "lsli", MSG_INTERFACE_MSG_TO_NODE, modelViewerID, 3, &fake_rope);
	SendMessage(&GameInterface, "lsli", MSG_INTERFACE_MSG_TO_NODE, modelViewerID, 3, &fake_sail);
	SendMessage(&GameInterface, "lsli", MSG_INTERFACE_MSG_TO_NODE, modelViewerID, 3, &fake_flag);

    if(IsEntity(&real_ship_model)) {
        // LEO: modelr резолвит текстуры как texturePath(глобал геометрии)+имя; вьювер — единственный, кто не
        // сбрасывал этот глобал перед загрузкой -> от прошлого загрузчика мог остаться мусорный префикс -> серый корпус.
        // Текстуры корабля идут через слоты SetTexturePath(2/1/0), поэтому глобал держим пустым.
        SendMessage(&real_ship_model, "ls", MSG_MODEL_SET_DIRPATH, "");
        SendMessage(&real_ship_model, "ls", MSG_MODEL_LOAD_GEO, "Ships/"+mv_shref.name+"/"+mv_shref.name);

        // выставляем MODELR
        SendMessage(&GameInterface, "lsli", MSG_INTERFACE_MSG_TO_NODE, modelViewerID, 0, &real_ship_model);

        SendMessage(&fake_rope, "lii", MSG_ROPE_INIT, &fake_ship, &real_ship_model);
        SendMessage(&fake_sail, "liil", MSG_SAIL_INIT, &fake_ship, &real_ship_model, 1);
        SendMessage(&fake_flag, "lili", MSG_FLAG_INIT, &real_ship_model, sti(pchar.nation), &fake_ship);
    }
}

void resolveModelViewerItem(string item, string shader) {
    // LEO: чистим слоты от возможного прошлого превью корабля, чтобы они не перебивали путь к текстурам Ammo
    SetTexturePath(2, "");
    SetTexturePath(1, "");
    SetTexturePath(0, "");

	if (!IsEntity(real_ship_model))
    	CreateEntity(&real_ship_model, "modelr");

    if(IsEntity(&real_ship_model)) {
        // LEO: как в Items_LoadModel — путь к текстурам предмета через глобал геометрии (DIRPATH), сбрасываем после
        SendMessage(&real_ship_model, "ls", MSG_MODEL_SET_DIRPATH, "Ammo\");
        SendMessage(&real_ship_model, "ls", MSG_MODEL_LOAD_GEO, "Ammo/"+item);
        SendMessage(&real_ship_model, "ls", MSG_MODEL_SET_DIRPATH, "");
		SendMessage(&real_ship_model, "ls", MSG_MODEL_SET_TECHNIQUE, shader);

        // выставляем MODELR
        SendMessage(&GameInterface, "lsli", MSG_INTERFACE_MSG_TO_NODE, modelViewerID, 0, &real_ship_model);
    }
}

void emptyModelViewer() {
    SendMessage(&fake_rope, "li", MSG_ROPE_DEL_GROUP, &real_ship_model);
    SendMessage(&fake_sail, "li", MSG_SAIL_DEL_GROUP, &fake_ship);
    SendMessage(&fake_flag, "li", MSG_FLAG_DEL_GROUP, &real_ship_model);

	if (IsEntity(fake_rope))
    	DeleteClass(&fake_rope);
	if (IsEntity(fake_flag))
		DeleteClass(&fake_flag);
	if (IsEntity(fake_sail))
    	DeleteClass(&fake_sail);
	if (IsEntity(real_ship_model))
    	DeleteClass(&real_ship_model);
	if (IsEntity(fake_ship))
		DeleteClass(&fake_ship);
}

void setLighting() {
	if (!IsEntity(&Weather) || !CheckAttribute(Weather, "Sun")) return;

    float scolor = argb(255,255,255,250);
    float sambient = argb(255,120,120,120);

    if (Weather.Sun.Color == scolor && Weather.Sun.Ambient == sambient) {
        return;
    }

	whrBckpColor = Weather.Sun.Color;
	whrBckpAmbient = Weather.Sun.Ambient;

	Weather.Sun.Color = scolor;
	Weather.Sun.Ambient = sambient;

	Weather.isDone = "";
}

void resetLighting() {
	if (whrBckpColor == 0 || whrBckpAmbient == 0) return;

	Weather.Sun.Color = whrBckpColor;
	Weather.Sun.Ambient = whrBckpAmbient;

	Weather.isDone = "";
}

void SailsGeraldShipModelViewer(ref smv_chref, ref smv_shref, int hullid) {
    mv_chref = smv_chref;
    mv_shref = smv_shref;
    emptyModelViewer();
    resolveModelViewerShip(hullid);
}

void QuestBookSetModelViewerParams(int left, int top, int right, int bottom, bool isShip) {
	SendMessage(&GameInterface,"lsllllll",MSG_INTERFACE_MSG_TO_NODE,modelViewerID, -1, 4, left,top,right,bottom);
	SendMessage(&GameInterface,"lsllllll",MSG_INTERFACE_MSG_TO_NODE,modelViewerID+"_VIDEO_BG", -1, 4, left,top,right,bottom);
	SendMessage(&GameInterface,"lsllllll",MSG_INTERFACE_MSG_TO_NODE,modelViewerID+"_FRAME", -1, 4, left,top,right,bottom);
	SendMessage(&GameInterface,"lsllllll",MSG_INTERFACE_MSG_TO_NODE,modelViewerID+"_BUTTON_CANCEL", -1, 4, right-29, top+10,right-10,top+29);
    configureModelViewerOptions(isShip);
}

void QuestBookShowModelViewer(string window, string shipTable) {
    setLighting();
	currentWindow = window;
	SetNodeUsing(modelViewerID+"_VIDEO_BG", true);
	SetNodeUsing("EXIT_ENC_BTN", false);
	XI_WindowDisable(currentWindow, true);
	XI_WindowDisable("ENCYCLOPEDIA_INFO_BUTTONS", true);
	XI_WindowDisable("ENCYCLOPEDIA_INFO_BUTTONS", true);
	XI_WindowShow(modelViewerID+"_WINDOW", true);
	XI_WindowDisable(modelViewerID+"_WINDOW", false);
	QuestBookResolveModelViewer(shipTable);
}

void QuestBookShipModelViewer(string selectedTable, string selectedRow) {
	if (sti(GameInterface.(selectedTable).(selectedRow).index) == -1) {
		trace("unable to load model viewer for empty ship id");
		return;
	}

	mv_chref = &Characters[NPC_FindOrCreateCharacter("modelviewer_character")];
	mv_chref.ship.type = GenerateShipExt(sti(GameInterface.(selectedTable).(selectedRow).index), false, mv_chref);
	mv_shref = GetRealShip(sti(mv_chref.ship.type));
	mv_shref.ship.upgrades.sails = 1;
	mv_shref.shipsails.sailscolor = argb(255,255,255,255);
    
	resolveModelViewerShip(1+rand(2));
	ShowModelViewerBoxSize();
}

// LEO: габариты корабля в окне энциклопедии. Раньше тут стоял закомментированный вывод
// mv_chref.Ship.BoxSize - он не мог работать никогда: BoxSize пишет сущность SHIP, а в энциклопедии
// корабля нет, только превью-модель.
// Считает движковая GetShipModelBox по .gm-иерархии (корпус + мачты + реи) - той же формулой, что и
// у корабля на воде, поэтому цифры совпадают. Ей нужно только ИМЯ модели, загруженная сущность не
// требуется, и результат кэшируется. Ось: 0 - ширина по реям, 1 - высота по мачтам, 2 - длина корпуса.
void ShowModelViewerBoxSize()
{
	string sModel;
	if (!CheckAttribute(mv_shref, "name"))
	{
		SetFormatedText("MODEL_VIEWER_BOXSIZE", "");
		return;
	}
	sModel = "Ships/" + mv_shref.name + "/" + mv_shref.name;
	SetFormatedText("MODEL_VIEWER_BOXSIZE",
		XI_ConvertString("ship_length") + ": " + FloatToString(GetShipModelBox(sModel, 2), 1) + XI_ConvertString("m") + "\n"
		+ XI_ConvertString("sails_width") + ": " + FloatToString(GetShipModelBox(sModel, 0), 1) + XI_ConvertString("m") + "\n"
		+ XI_ConvertString("sails_height") + ": " + FloatToString(GetShipModelBox(sModel, 1), 1) + XI_ConvertString("m"));
}

void QuestBookResolveModelViewer(string shipTable) {
	string row;
	string table;
	switch (currentWindow)
	{
	case "ENC_INFO_SHIP":
		QuestBookSetModelViewerParams(133,75,933,525, true);
		table = "SHIP_TABLE_LIST_"+shipTable;
		row = "tr"+GameInterface.(table).select;
		QuestBookShipModelViewer(table, row);
		break;

	case "ENC_INFO_BLADE":
		QuestBookSetModelViewerParams(250,140,816,460, false);
		table = "BLADE_TABLE_LIST";
		row = "tr"+GameInterface.(table).select;
		QuestBookItemModelViewer(table, row);
		break;
		
	case "ENC_INFO_GUN":
		QuestBookSetModelViewerParams(250,140,816,460, false);
		table = "GUN_TABLE_LIST";
		row = "tr"+GameInterface.(table).select;
		QuestBookItemModelViewer(table, row);
		break;
	}
}

void QuestBookExitModelViewer() {
	SetNodeUsing(modelViewerID+"_VIDEO_BG", false);
	SetNodeUsing("EXIT_ENC_BTN", true);
	XI_WindowDisable(modelViewerID+"_WINDOW", true);
	XI_WindowShow(modelViewerID+"_WINDOW", false);
	XI_WindowDisable(currentWindow, false);
	XI_WindowDisable("ENCYCLOPEDIA_INFO_BUTTONS", false);

	currentWindow = "";
    ExitModelViewer();
}

void QuestBookItemModelViewer(string selectedTable, string selectedRow) {
	if (sti(GameInterface.(selectedTable).(selectedRow).index) == -1) {
		trace("unable to load model viewer for empty item id");
		return;
	}
	
	ref itm = &Items[sti(GameInterface.(selectedTable).(selectedRow).index)];
	string shader = "EnvAmmoShader";
	if (itm.groupID == GUN_ITEM_TYPE) {
		shader += "2";
	}
	if (itm.groupID == GUN_ITEM_TYPE || itm.groupID == BLADE_ITEM_TYPE) {
		resolveModelViewerItem(itm.model, shader);
		SetFormatedText("MODEL_VIEWER_BOXSIZE", ""); // LEO: у предмета габариты не показываем - иначе висели бы от прошлого корабля
	}
}
