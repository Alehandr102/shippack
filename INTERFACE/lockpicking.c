int checkingstagenum = 10;
int pickposition = 90;
int openingcount = 0;

int correctposition;

string wrenchtex, picktex;
string chestID;

bool openingstarted = false;
bool cylinderstarted = false;
bool lockclick = false;
bool g_bToolTipStarted = false;

bool	LckPckngProcessBooster	 = true;

bool firsttime;
bool opened = false;
bool broken = false;

bool mousechangepos = true;
string mpos;
string ompos;

void InitInterface_gm(string iniName)
{
	string sHeader = "TEST";

	string sText1, sText2, sText3;
	LAi_SetActorTypeNoGroup(pchar);
	locCameraSleep(true);
	SetTimeScale(0.0);

	if(LckPckngProcessBooster == false) EngineLayersOffOn(true);
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("frame","IProcessFrame",0);
	SetEventHandler("eSlideChange","procSlideChange",0);
	SetEventHandler("ShowInfo", "ShowInfo", 0);
	SetEventHandler("MouseRClickUP","HideInfo",0);

	if (CheckAttribute(pchar, "questTemp.lockpicking.firsttime"))
	{
		if(makeint(pchar.questTemp.lockpicking.firsttime) == 1) firsttime = true;
		else firsttime = false;
	}
	else
	{
		firsttime = true;
	}

	PlaySound("interface\lockpicking\ui_lockpicking_enter_01.wav"); // Звук начала процедуры
	chestID = pchar.questTemp.lockpicking.ID;

	correctposition = 1+rand(179);

	if(correctposition < 15) correctposition = 15;
	if(correctposition > 165) correctposition = 165;

	sHeader = GetConvertStrWithReplace("Variable_lockpicking_1", "Interface.txt", "#space#", " ");
	sText1 = GetConvertStrWithReplace("Variable_lockpicking_guide_1", "Interface.txt", "#space#", " ");
	sText2 = GetConvertStrWithReplace("Variable_lockpicking_guide_2", "Interface.txt", "#space#", " ");
	sText3 = GetConvertStrWithReplace("Variable_lockpicking_guide_3", "Interface.txt", "#space#", " ");

	if(firsttime == true) CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,255,255,255), "", argb(255,255,255,255), "none", "none", "none", 64, 64);
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();

    switch(nodName)
	{

	}
}

void procSlideChange()
{
	string sNodeName = GetEventData();
	int nVal = GetEventData();
	float fVal = GetEventData();
}

void IProcessFrame()
{
	int randsound;
	int squeaksound;
	int randclicksound;

	int difference;
	float dmultiplier;

	float mouseposition;
	int movsound;

	if (CheckAttribute(GameInterface,"mousepos")) mouseposition = 600.0-(stf(GameInterface.mousepos.y));

	if(mouseposition > 600) mouseposition = 600;
	if(mouseposition < 9) mouseposition = 9;

	pickposition = mouseposition*0.3;
	picktex = "interfaces\LockPicking\pick\" + makeint(pickposition/2) + ".tga";
	SetNewPicture("PICK_PIC",picktex);

	if(lockclick == false)
	{
		SetRealMouseSensitivity();
	}
	else
	{
		XI_SetMouseSensitivity(0.0, 0.0);
	}

	if(checkingstagenum < 460)
	{
		if(mousechangepos == false)
		{
			mpos = GameInterface.mousepos.y;

			if(mpos != ompos)
			{
				if(rand(20) == 0)
				{
					movsound = rand(11)+1;

					if(movsound < 10) PlaySound("interface\lockpicking\mov\ui_lockpicking_pickmovement_0"+ movsound +".wav");
					else PlaySound("interface\lockpicking\mov\ui_lockpicking_pickmovement_"+ movsound +".wav");
				}
			}

			mousechangepos = true;
		}

		if(mousechangepos == true)
		{
			if (CheckAttribute(GameInterface,"mousepos")) ompos = GameInterface.mousepos.y;
			mousechangepos = false;
		}
	}

	if(checkingstagenum < 460)
	{
		if((pickposition - correctposition) <= 0) difference = correctposition - pickposition;
		else difference = pickposition - correctposition;

		dmultiplier = makefloat(difference)/180.0;

		if(XI_IsKeyPressed("control")) // Двигаем цилиндр, если нажата кнопка Ctrl
		{
			checkingstagenum = checkingstagenum + 10;

			if(cylinderstarted == false) // Звук начала поворота цилиндра
			{
				randsound = rand(7)+1;
				PlaySound("interface\lockpicking\cylturn\ui_lockpicking_cylinderturn_0"+ randsound +".wav");
				cylinderstarted = true;
			}
		}
		else // Если кнопка не нажата, то возвращаем в исходное положение
		{
			checkingstagenum = checkingstagenum - 10;

			if(cylinderstarted == true) // Звук начала поворота цилиндра
			{
				randsound = rand(7)+1;
				PlaySound("interface\lockpicking\cylturn\ui_lockpicking_cylinderturn_0"+ randsound +".wav");
				cylinderstarted = false;
			}
		}

		if(makeint(dmultiplier*100.0) > 2)
		{
			if(checkingstagenum > makeint((1.0 - dmultiplier)*460.0))
			{
				checkingstagenum = makeint((1.0 - dmultiplier)*460.0);

				if(lockclick == false)
				{
					lockclick = true;

					randclicksound = rand(2)+1;
					PlaySound("interface\lockpicking\cylstop\ui_lockpicking_cylinderstop_0"+ randclicksound +".wav");
					if (rand(20)==0 && pickposition != 0)
					{
						broken = true;
						IDoExit(RC_INTERFACE_DO_RESUME_GAME, true);
					}
				}

			}
		}

		if(checkingstagenum > 460) checkingstagenum = 460;

		if(checkingstagenum < 10)
		{
			checkingstagenum = 10;
			lockclick = false;
		}

		if(checkingstagenum == 460) OpenProcedure(); // Если цилиндр повернут на 90 градусов, то запускаем процесс открытия

		if(checkingstagenum != 10 && checkingstagenum != 460 && checkingstagenum != makeint((1.0 - dmultiplier)*460.0)) // Поскрипывания цилиндра при его движении
		{
			if(rand(160) == 0)
			{
				squeaksound = rand(11)+1;

				if(squeaksound < 10) PlaySound("interface\lockpicking\cylsqueak\ui_lockpicking_cylindersqueak_0"+ squeaksound +".wav");
				else PlaySound("interface\lockpicking\cylsqueak\ui_lockpicking_cylindersqueak_"+ squeaksound +".wav");
			}
		}

		// Меняем визуальное положение цилиндра
		wrenchtex = "interfaces\LockPicking\wrench\" + makeint(checkingstagenum/10) + ".tga";
		SetNewPicture("WRENCH_PIC",wrenchtex);
	}

	if(openingstarted == true)
	{
		openingcount = openingcount + 4;

		if(openingcount > 200)
		{
			openingstarted = false;
			opened = true;

			SetNodeUsing("BACK_PIC",false);
			SetNodeUsing("LOCK_PIC",false);
			SetNodeUsing("WRENCH_PIC",false);
			SetNodeUsing("PICK_PIC",false);
			SetNodeUsing("PICKSLIDE",false);

			OpenChest();
		}
	}
}

void OpenProcedure() // Процесс открытия
{
	int randsound = rand(2)+1;

	openingstarted = true;
	PlaySound("interface\lockpicking\ui_lockpicking_unlock_0"+ randsound +".wav"); // Звук щелчка замка
}

void OpenChest()
{
	AddCharacterExpToSkill(PChar, "Fortune", 50);
	AddCharacterExpToSkill(PChar, "Repair", 50);

	// Gregg: этой же мини-игрой в квесте «Две сестры» вскрывают дверь трюма «Церебуса».
	// Ни сундука в инвентаре, ни добычи там нет: не трогаем ни TakeItemFromCharacter,
	// ни счётчик вскрытых сундуков, ни ChestRollLoot. Отмычку не отбираем - её роль
	// играет квестовая шпилька, её забирает сам квест. Продолжение уходит через
	// DoQuestFunctionDelay: мини-игра держит SetTimeScale(0), поэтому функция отработает
	// ровно после закрытия экрана, а не под ним.
	if (chestID == "TwoSisters_HoldDoor")
	{
		DoQuestFunctionDelay("TwoSistersLusQuestDoorOpened", 0.5);
		// Gregg: и закрываем экран сами. У сундука это делает окно добычи из
		// ChestRollLoot, а на двери показывать нечего - без явного выхода игрок
		// остаётся перед чёрным экраном и жмёт ESC.
		IDoExit(RC_INTERFACE_DO_RESUME_GAME, true);
		return;
	}

	TakeItemFromCharacter(pchar, chestID);
	IncAndUnlock("AchChest", 1);

	TakeNItems(pchar, "Lockpick", -1);
	ChestRollLoot(chestID, "", "");	// выдаст добычу игроку и покажет список
}

void ShowInfo()
{
	if(firsttime == false && opened == false)
	{
		g_bToolTipStarted = true;
		string sHeader = "TEST";
		string sNode = GetCurrentNode();

		string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
		sPicture = "none";
		sGroup = "none";
		sGroupPicture = "none";

		switch (sNode)
		{
			case "BACK_PIC":
				sHeader = GetConvertStrWithReplace("Variable_lockpicking_6", "Interface.txt", "#space#", " ");
				sText1 = GetConvertStrWithReplace("Variable_lockpicking_guide_1", "Interface.txt", "#space#", " ");
				sText2 = GetConvertStrWithReplace("Variable_lockpicking_guide_2", "Interface.txt", "#space#", " ");
				sText3 = GetConvertStrWithReplace("Variable_lockpicking_guide_3", "Interface.txt", "#space#", " ");
			break;
		}

		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,255,255,255), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
	}
}

void HideInfo()
{
	if(firsttime == false && opened == false)
	{
		if( g_bToolTipStarted )
		{
		g_bToolTipStarted = false;
		CloseTooltip();
		SetCurrentNode("PICKSLIDE");
		}
	}
}

void ProcessCancelExit()
{
	if(openingstarted == false)
	{
		if(firsttime == false || opened == true)
		{
			CloseTooltip();
			IDoExit(RC_INTERFACE_DO_RESUME_GAME, true);
		}
		else
		{
			firsttime = false;
			pchar.questTemp.lockpicking.firsttime = false;
			CloseTooltip();
			SetCurrentNode("PICKSLIDE");
		}
	}
}

void IDoExit(int exitCode, bool bClear)
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("frame","IProcessFrame");
	DelEventHandler("eSlideChange","procSlideChange");
	DelEventHandler("ShowInfo", "ShowInfo");
	DelEventHandler("MouseRClickUP","HideInfo");

	LAi_SetPlayerType(pchar);
	SetRealMouseSensitivity();

	TimeScaleCounter = 0;
	DelPerkFromActiveList("TimeSpeed");
	locCameraSleep(false);

	SetTimeScale(1.0);
	TimeScaleCounter = 0;
	DeleteAttribute(pchar, "pause");
	DelPerkFromActiveList("TimeSpeed");

	interfaceResultCommand = exitCode;

	if( interfaceResultCommand == RC_INTERFACE_DO_RESUME_GAME )
	{
		DeleteEntitiesByType("scrshoter");
	}

	// Gregg: дверь трюма из «Двух сестёр» вскрывают шпилькой, а не отмычкой - поломку
	// отмычки на ней не крутим, иначе игрок получает лог про сломанный инструмент,
	// которого у него нет. Шпильку забирает сам квест, когда замок поддался.
	bool bQuestDoor = false;
	if (chestID == "TwoSisters_HoldDoor") bQuestDoor = true;

	if((rand(99)+1) <= 10*(MOD_SKILL_ENEMY_RATE*3/2) && opened == false && !bQuestDoor) // доработка взлома. Шанс поломки отмычки при отмене взлома
    {
	    TakeNItems(pchar, "Lockpick", -1);
	    PlaySound("interface\lockpicking\lockpick_breaks.wav"); // Звук поломки
	    Log_info(GetConvertStrWithReplace("Variable_lockpicking_7", "Interface.txt", "#space#", " "));
	}
	else
	{
		if(broken)
		{
			TakeNItems(pchar, "Lockpick", -1);
			PlaySound("interface\lockpicking\lockpick_breaks.wav"); // Звук поломки
			Log_info(GetConvertStrWithReplace("Variable_lockpicking_8", "Interface.txt", "#space#", " "));
		}
	}

	EndCancelInterface(bClear);
}

bool CheckSkull(string SkullId)
{
    if(CheckCharacterItem(pchar, "sculMa1") || CheckAttribute(pchar, "OwnsSkull1"))
	{
		if(!CheckAttribute(pchar, "OwnsSkull1")) pchar.OwnsSkull1 = true;
		return true;
	}
    if(CheckCharacterItem(pchar, "sculMa2") || CheckAttribute(pchar, "OwnsSkull2"))
	{
		if(!CheckAttribute(pchar, "OwnsSkull2")) pchar.OwnsSkull2 = true;
		return true;
	}
    if(CheckCharacterItem(pchar, "sculMa3") || CheckAttribute(pchar, "OwnsSkull3"))
	{
		if(!CheckAttribute(pchar, "OwnsSkull3")) pchar.OwnsSkull3 = true;
		return true;
	}
    return false;
}
