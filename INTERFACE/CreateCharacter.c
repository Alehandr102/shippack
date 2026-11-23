int curmodel = 0;
int curmanim = 0;
string sBlockPGG = "None";
int iBlockPGG = 1;
int FState_SEX[3] = {0,0,0};
bool FState_ANIM = false;
bool heroPreviewDragging = false;
float heroPreviewDefaultYaw = 0.0;

void InitInterface(string iniName)
{
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	StartAboveForm(false);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("CheckButtonChange", "ProcessFilter", 0);

	LAi_SetActorTypeNoGroup(PChar);
	
	LoadStartGameParam();
	startHeroType = 1;
	if (CheckAttribute(pchar,"RemovePGG.PGG1") && pchar.RemovePGG.PGG1 == 1)
	{
		CheckButton_SetState("REMOVE_PGG_CHECKBOX", 1, 1);
	}
	
	SelectOperation();
	GetCharacterAy(pchar, &heroPreviewDefaultYaw);
	heroPreviewDragging = false;
	SetEventHandler("frame","UpdateHeroPreviewDrag",0);
}

void UpdateHeroPreviewDrag()
{
	if (heroPreviewDragging)
		heroPreviewDragging = SendMessage(pchar, "ls", MSG_CHARACTER_EX_MSG, "DragHeroPreview");
}

void ResetHeroPreviewYaw()
{
	heroPreviewDragging = false;
	SendMessage(pchar, "lsf", MSG_CHARACTER_EX_MSG, "SetHeroPreviewYaw", heroPreviewDefaultYaw);
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_ANY_EXIT );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_ANY_EXIT );
}

void IDoExit(int exitCode)
{
	DelEventHandler("frame","UpdateHeroPreviewDrag");
	ResetHeroPreviewYaw();
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("CheckButtonChange", "ProcessFilter");

	EndAboveForm(true);
	interfaceResultCommand = exitCode;
	EndCancelInterface(true);

	EndQuestMovie();
	LAi_SetPlayerType(pchar);

	EndSelectMainCharacter(pchar);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	if (nodName == "HERO_PREVIEW_DRAG")
	{
		if (comName == "click_down") heroPreviewDragging = true;
		if (comName == "click_up") heroPreviewDragging = false;
		return;
	}

	if(comName=="leftstep")
	{
		SelectMainCharacter(true);
		return;
	}
	if(comName=="rightstep")
	{
		SelectMainCharacter(false);
		return;
	}

	switch(nodName)
	{
		case "SELECT_LEFT_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				SelectMainCharacter(true);
			}
		break;

		case "SELECT_RIGHT_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				SelectMainCharacter(false);
			}
		break;

		case "COSTUMES_LEFT_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				CycleCirass(false);
			}
		break;

		case "COSTUMES_RIGHT_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				CycleCirass(true);
			}
		break;

		case "ANIMATION_LEFT_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				CycleAnimation(false);
			}
		break;

		case "ANIMATION_RIGHT_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				CycleAnimation(true);
			}
		break;

		case "SELECT_CHARACTER_DONE":
			if(comName=="activate" || comName=="click")
			{
				SelectMainCharacterDone();
			}
		break;

		case "REMOVE_ALL_PGG_BUTTON":
			if(comName=="activate" || comName=="click")
			{
				int  heroQty   = sti(GetNewMainCharacterParam("ps_hero_qty"));
				for (int n=1; n<=heroQty; n++)
				{
					sBlockPGG = "PGG" + n;
					pchar.RemovePGG.(sBlockPGG) = iBlockPGG;
					CheckButton_SetState("REMOVE_PGG_CHECKBOX", 1, iBlockPGG);
				}
				if (iBlockPGG == 1)	iBlockPGG = 0;
				else iBlockPGG = 1;
			}
		break;
	}
}

// LEO: Preview only clips found in the currently loaded animation profile.
string FindHeroPreviewAction(string name, int variants)
{
	if (variants == 0)
	{
		if (SendMessage(pchar, "lss", MSG_CHARACTER_EX_MSG, "HasAnimationAction", name) != 0) return name;
		return "";
	}
	string actions[11];
	int count = 0;
	for (int i = 1; i <= variants; i++)
	{
		string action = name + i;
		if (SendMessage(pchar, "lss", MSG_CHARACTER_EX_MSG, "HasAnimationAction", action) != 0)
		{
			actions[count] = action;
			count++;
		}
	}
	if (count == 0) return "";
	return actions[rand(count - 1)];
}

void PlayHeroInitialPreview()
{
	string pose = FindHeroPreviewAction("csp_preview_pose", 0);
	if (pose != "") LAi_ActorAnimation(pchar, pose, "", -1.0);
}

void CycleAnimation(bool dir)
{
	string anim = "";
	if (!dir)
	{
		if (curmanim == 0) curmanim = 9;
		else curmanim--;
	}
	if (dir)
	{
		if (curmanim == 9) curmanim = 0;
		else curmanim++;
	}
	LAi_SetActorTypeNoGroup(pchar);
	switch (curmanim)
	{
		case 0:
			anim = FindHeroPreviewAction("idle_", 11);
		break;
		case 1:
			anim = FindHeroPreviewAction("normal to fight", 0);
		break;
		case 2:
			anim = FindHeroPreviewAction("attack_fast_", 4);
		break;
		case 3:
			anim = FindHeroPreviewAction("attack_force_", 4);
		break;
		case 4:
			anim = FindHeroPreviewAction("attack_feintc_", 4);
		break;
		case 5:
			anim = FindHeroPreviewAction("attack_break_1", 0);
		break;
		case 6:
			anim = FindHeroPreviewAction("parry_", 4);
		break;
		case 7:
			anim = FindHeroPreviewAction("attack_feint_", 4);
		break;
		case 8:
			anim = FindHeroPreviewAction("jump", 0);
		break;
		case 9:
			anim = FindHeroPreviewAction("fight to normal", 0);
		break;
	}
	if (anim != "") LAi_ActorAnimation(pchar, anim, "", 0.6);
}

void CycleCirass(bool dir)
{
	string itemID;
	string costume;
	if (!dir)
	{
		if (curmodel == 0) curmodel = 8;
		else curmodel--;
	}
	if (dir)
	{
		if (curmodel == 8) curmodel = 0;
		else curmodel++;
	}
	int idLngFile = LanguageOpenFile("CreateCharacter.txt");
	switch (curmodel)
	{
		case 0:
			pchar.model = GetSubStringByNum(pchar.HeroModel, 0);
			DeleteAttribute(pchar, "cirassId");
			SetNewModelToChar(pchar);//boal
			PlayHeroInitialPreview();
			costume =" : "+ LanguageConvertString(idLngFile,"Default");
		break;
		case 1:
			itemID = "cirass1";
			costume = " : "+ LanguageConvertString(idLngFile,"CirassO");
		break;
		case 2:
			itemID = "cirass2";
			costume = " : "+ LanguageConvertString(idLngFile,"CirassTw");
		break;
		case 3:
			itemID = "cirass3";
			costume = " : "+ LanguageConvertString(idLngFile,"CirassTh");
		break;
		case 4:
			itemID = "cirass4";
			costume = " : "+ LanguageConvertString(idLngFile,"CirassFo");
		break;
		case 5:
			itemID = "cirass5";
			costume = " : "+ LanguageConvertString(idLngFile,"CirassFi");
		break;
		case 6:
			itemID = "suit_1";
			costume = " : "+ LanguageConvertString(idLngFile,"SuitO");
		break;
		case 7:
			itemID = "suit_2";
			costume = " : "+ LanguageConvertString(idLngFile,"SuitTw");
		break;
		case 8:
			itemID = "suit_3";
			costume = " : "+ LanguageConvertString(idLngFile,"SuitTh");
		break;
	}
	SetFormatedText("COSTUME", LanguageConvertString(idLngFile,"SelectedCostume")+costume);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"COSTUME",5);
	LanguageCloseFile(idLngFile);
	if (curmodel != 0) ShowCirassOnCharacter(pchar, CIRASS_ITEM_TYPE, itemID);
}

void ShowCirassOnCharacter(ref chref, string groupID, string itemID)
{
	object emptyItm;
	aref arItm;
	makearef(arItm,emptyItm);
 	int itemNum; // boal 23.01.2004

	if(itemID != "")
	{
		itemNum = Items_FindItem(itemID,&arItm);// boal 23.01.2004
	}

	switch(groupID)
	{
    case CIRASS_ITEM_TYPE:
        if (CheckAttribute(chref, "HeroModel")) // все, у кого есть что одеть
        {
            if (CheckAttribute(arItm, "model"))
            {
                chref.model = GetSubStringByNum(chref.HeroModel, sti(arItm.model));
				if (startherotype == 2 && sti(arItm.model) == 5 && findsubstr(chref.HeroModel, "Whisper" , 0) != -1 && CheckCharacterItem(pchar,"hatWhisper") &&  !CheckAttribute(pchar, "Whisper.NanoCostume"))
				{
					chref.model = "PGG_Whisper_5_Cirass"
				}
                chref.cirassId = itemNum;
            }
            else
            {
                chref.model = GetSubStringByNum(chref.HeroModel, 0);
                DeleteAttribute(chref, "cirassId");
            }
        }
        else
        {
            // тут анализ модели офицера или пирата
            if(CheckAttribute(arItm, "model"))
            {
                chref.cirassId = itemNum;
            }
            else
            {
                DeleteAttribute(chref, "cirassId");
            }
        }
        SetNewModelToChar(chref);//boal
	break;
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void SelectMainCharacterDone()
{
	PlaySound("notebook");
	PChar.Selected = true;

    int idLngFile = LanguageOpenFile("HeroDescribe.txt");
	string sName = LanguageConvertString(idLngFile, "heroName_" + startHeroType);
	string sLastName = LanguageConvertString(idLngFile, "heroLastname_" + startHeroType);
	string sCharacterName = sName + " " + sLastName;
	LanguageCloseFile(idLngFile);
	Log_SetStringToLog(GetConvertStrWithReplace("Variable_CreateCharacter_7", "Interface.txt", "#space#", " ") + sCharacterName + ".");

	PChar.DefaultModel = PChar.model;

	SetNewModelToChar(PChar);
	FaceMaker(PChar);

	DeleteAttribute(pchar,"items");
	DeleteAttribute(pchar,"equip");

	ProcessCancelExit();
}

bool HeroSupportsMusket(int description, int index, string animation)
{
	string model = GetSubStringByNum(LanguageConvertString(description, "heroModel_" + index), 0);
	return NativeHasMusketResources(model, ResolveCharacterGunAnimation(model, animation)) != 0;
}

void SelectMainCharacter(bool bLeft)
{
	int idLngFile = LanguageOpenFile("HeroDescribe.txt");
	string sQuantity = LanguageConvertString(idLngFile, "hero_qty");
	int iQuantity = sti(sQuantity);
	string sSex, sAnim;
	int remaining = iQuantity;

	while (1)
	{
		if (remaining <= 0)
		{
			LanguageCloseFile(idLngFile);
			return;
		}
		remaining--;
		if (bLeft)
		{
			startHeroType = startHeroType - 1;
			if (startHeroType <= 0) { startHeroType = iQuantity; }
		}
		else
		{
			startHeroType = startHeroType + 1;
			if (startHeroType > iQuantity) { startHeroType = 1; }
		}

		sSex = LanguageConvertString(idLngFile, "sex_" + startHeroType);
		sAnim = LanguageConvertString(idLngFile, "animation_" + startHeroType);
		if (FState_SEX[0] && sSex == "man") {continue;}
		if (FState_SEX[1] && sSex == "woman") {continue;}
		if (FState_SEX[2] && sSex == "skeleton") {continue;}

		if (FState_ANIM && !HeroSupportsMusket(idLngFile, startHeroType, sAnim)) continue;
		break;//выходим из цикла, если все иф не сработали = перс подходит
	}

	string sName = LanguageConvertString(idLngFile, "heroName_" + startHeroType);
	string sLastName = LanguageConvertString(idLngFile, "heroLastname_" + startHeroType);
	string sCharacterName = sName + " " + sLastName;

	string sHeroModel = LanguageConvertString(idLngFile, "heroModel_" + startHeroType);
	PChar.HeroModel = sHeroModel;
	string sModel = GetSubStringByNum(PChar.HeroModel, 0);
	LanguageCloseFile(idLngFile);

	PChar.model = sModel;
	PChar.model.animation = sAnim;
	SetNewModelToChar(PChar);
	LAi_SetActorTypeNoGroup(PChar);
	ResetHeroPreviewYaw();
	PlayHeroInitialPreview();

	CreateString(true, "SelectCharacterName", sCharacterName, FONT_CAPTION, COLOR_NORMAL, 395, 545, SCRIPT_ALIGN_CENTER, 1.0);

	curmodel = 0;
	curmanim = 0;
	idLngFile = LanguageOpenFile("CreateCharacter.txt");
	string costume = " : "+LanguageConvertString(idLngFile, "Default")
	SetFormatedText("ANIMATION", LanguageConvertString(idLngFile, "Animation"));
	SetFormatedText("COSTUME", LanguageConvertString(idLngFile,"SelectedCostume")+costume);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"COSTUME",5);
	UpdateHeroInfo();
	LanguageCloseFile(idLngFile);

	bool bDelPGG = false;
	sBlockPGG = "PGG" + startHeroType;
	if (CheckAttribute(pchar,"RemovePGG." + sBlockPGG)) bDelPGG = sti(pchar.RemovePGG.(sBlockPGG));
	CheckButton_SetState("REMOVE_PGG_CHECKBOX", 1, bDelPGG);
}

void SelectOperation()
{
	int idLngFile = LanguageOpenFile("HeroDescribe.txt");

	SetNodeUsing("SELECT_CHARACTER_DONE", true);
	SetNodeUsing("SELECT_CHARACTER_PICTURE", true);
	SetNodeUsing("SELECT_LEFT_BUTTON", true);
	SetNodeUsing("SELECT_RIGHT_BUTTON", true);
	SetNodeUsing("COSTUMES_LEFT_BUTTON", true);
	SetNodeUsing("CHECK_HERO_SEX", true);
	SetNodeUsing("CHECK_HERO_ANIM", true);
	SetNodeUsing("COSTUMES_RIGHT_BUTTON", true);
	SetNodeUsing("ANIMATION_LEFT_BUTTON", true);
	SetNodeUsing("ANIMATION_RIGHT_BUTTON", true);
	SetNodeUsing("ANIMATION", true);
	SetNodeUsing("COSTUME", true);
	SetNodeUsing("INFO_TEXT", true);

	int idLngFile1 = LanguageOpenFile("CreateCharacter.txt");
	string costume = " : "+LanguageConvertString(idLngFile1, "Default")
	SetFormatedText("ANIMATION", LanguageConvertString(idLngFile1,"Animation"));
	SetFormatedText("COSTUME", LanguageConvertString(idLngFile1,"SelectedCostume")+costume);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"COSTUME",5);

	string sName = LanguageConvertString(idLngFile, "heroName_" + startHeroType);
	string sLastName = LanguageConvertString(idLngFile, "heroLastname_" + startHeroType);
	string sCharacterName = sName + " " + sLastName;

	string sHeroModel = LanguageConvertString(idLngFile, "heroModel_" + startHeroType);
	PChar.HeroModel = sHeroModel;
	string sModel = GetSubStringByNum(PChar.HeroModel, 0);
	string sAni = LanguageConvertString(idLngFile, "animation_" + startHeroType);

	PChar.model = sModel;
	PChar.model.animation = sAni;
	SetNewModelToChar(PChar);
	PlayHeroInitialPreview();

	UpdateHeroInfo();
	CreateString(true, "SelectCharacterName", sCharacterName, FONT_CAPTION, COLOR_NORMAL, 395, 545, SCRIPT_ALIGN_CENTER, 1.0);

    LanguageCloseFile(idLngFile);
	LanguageCloseFile(idLngFile1);
}

void UpdateHeroInfo()
{
	int idLngFile = LanguageOpenFile("HeroDescribe.txt");
	string totalInfo = LanguageConvertString(idLngFile, "hero_" + startHeroType);
	if (MOD_BETTATESTMODE == "On") SetFormatedText("INFO_TEXT", totalInfo+"\n( PMC num: "+startHeroType+" | Model: "+PChar.model+" | Animation: "+PChar.model.animation+" )");
	else SetFormatedText("INFO_TEXT", totalInfo);
    SetVAligmentFormatedText("INFO_TEXT");
	LanguageCloseFile(idLngFile);
}

void ProcessFilter()
{
	string sControl = GetEventData();
	int iSelectedCB = GetEventData();
	int iNewState = GetEventData();
	int idLngFile;
	if (sControl == "CHECK_HERO_SEX")
	{
		FState_SEX[iSelectedCB-1] = iNewState;
		if (!iNewState && FState_SEX[0]+FState_SEX[1]+FState_SEX[2] == 1)//нужно разлочить кнопку
		{
			for(i=1;i<4;i++) {SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, "CHECK_HERO_SEX", 5, i, 0);} //разблочим все кнопки
		}
		if (FState_SEX[0]+FState_SEX[1]+FState_SEX[2] == 2) //блочим оставшийся единственным ненажатым чекбокс. иначе вообще все герои будут недоступны и бесконечный цикл
		{
			for(i=1;i<4;i++)
			{
				if (SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CHECK_HERO_SEX", 3, i)) continue;
					else {SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, "CHECK_HERO_SEX", 5, i, 1); break;}//блочим ненажатую кнопку и выходим из цикла, так как она одна
			}
		}
		if (iNewState) //отключили какой-то пол
		{
			idLngFile = LanguageOpenFile("HeroDescribe.txt");
			string sSex = LanguageConvertString(idLngFile, "sex_" + startHeroType);
			switch (iSelectedCB-1)
			{
				case 0: if (sSex == "man") SelectMainCharacter(0); break;//если был выбран муж. и мы сняли чекбокс муж., то переходим к следующему персу
				case 1: if (sSex == "woman") SelectMainCharacter(0); break;
				case 2: if (sSex == "skeleton") SelectMainCharacter(0); break;
			}
			LanguageCloseFile(idLngFile);
		}
		return;
	}
	if (sControl == "CHECK_HERO_ANIM")
	{
		FState_ANIM = iNewState;
		if (iNewState) //отключили анимки
		{
			idLngFile = LanguageOpenFile("HeroDescribe.txt");
			string sAnim = LanguageConvertString(idLngFile, "animation_" + startHeroType);
			bool supported = HeroSupportsMusket(idLngFile, startHeroType, sAnim);
			LanguageCloseFile(idLngFile);
			if (!supported) SelectMainCharacter(false);
		}
		return;
	}

	if (sControl == "REMOVE_PGG_CHECKBOX")
	{
		sBlockPGG = "PGG" + startHeroType;
		if (iNewState)
		{
			pchar.RemovePGG.(sBlockPGG) = 1;
		}
		else
		{
			pchar.RemovePGG.(sBlockPGG) = 0;
		}
		return;
	}
}
