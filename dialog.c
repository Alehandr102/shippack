
#include "dialog_func.c"


#define EVENT_DIALOG_START		"evntDialogStart"
#define EVENT_DIALOG_EXIT		"evntDialogExit"

#event_handler("dlgReady", "StartDialogWithMainCharacter");
#event_handler("EmergencyDialogExit","DialogExit");

extern void ProcessDialogEvent();
extern void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag); // метод, содержащий ветку квест и др ветки конкрентого НПС

bool dialogDisable = false;
object	Dialog;
ref		CharacterRef;
bool	dialogRun = false;

bool	dialogSelf = false;

string  FullDialogPath;
string	PathDlgLngExtn = "";//fix

string dialogEditStrings[10];

void  ProcessCommonDialog(ref NPChar, aref Link, aref NextDiag)
{
    ProcessCommonDialogEvent(NPChar, Link, NextDiag);
}
//Инициализация
void DialogsInit()
{
	Set_inDialog_Attributes(); // boal
}

//Начать диалог
bool DialogMain(ref Character)
{
	//Если диалог запущен, выходим
	if(dialogRun != false) return false;
	//Ссылка на главного персонажа
	ref mainChr = GetMainCharacter();
	//Если когото не заведено, выходим
	if(!IsEntity(mainChr)) return false;
	if(!IsEntity(Character)) return false;
	if(LAi_IsDead(mainChr)) return false;
	if(LAi_IsDead(Character)) return false;
	//Проверим на существование текущего нода
	if(!CheckAttribute(Character, "Dialog.CurrentNode"))
	{
		Trace("Dialog: Character <" + Character.id + "> can't have field Dialog.CurrentNode, exit from dialog!")
		return false;
	}
	//Если персонаж не готов говорить выходим
	if(!LAi_Character_CanDialog(mainChr, Character)) return false;
	//Если персонаж не готов говорить выходим
	if(!LAi_Character_CanDialog(Character, mainChr)) return false;
	//Сохраняем ссыклу на того с кем говорим
	CharacterRef = Character;
	// LEO: без атрибута Dialog.Filename обращение к нему валит скрипт ("missed attribute: filename")
	// ЕЩЁ ДО подмены на заглушку, и разговор кончается вылетом. Такие NPC в моде есть: генерируемые
	// бойцы, которым файл диалога не задавали вовсе.
	string sDlgFile = "";
	bool bDlgLoaded = false;
	if( CheckAttribute(Character, "Dialog.Filename") ) sDlgFile = Character.Dialog.Filename;
	// Попытка загрузить текст дилога
	if( sDlgFile != "" ) bDlgLoaded = LoadDialogFiles(sDlgFile);
	if( !bDlgLoaded ) {
		// имеем ошибочный диалог
		Trace("Dialog: Character <" + Character.id + "> has no dialog file, using error_dialog.c");
		// заглушка знает только узел "First time" - на чужом диалог вышел бы пустым, без реплик и выхода.
		// Узел правим лишь когда файла нет вовсе: у такого персонажа он всё равно ни на что не указывает.
		if( sDlgFile == "" ) Character.Dialog.CurrentNode = "First time";
		if( !LoadDialogFiles("error_dialog.c") ) {
			return false;
		}
	}
	log_testinfo(sDlgFile);
	//Можем начинать диалог
	string sProf = "";
	if (IsCompanion(Character)) sProf += " (" + XI_ConvertString("companionship") + ")";
	if (IsOfficer(Character)) sProf += " (" + XI_ConvertString("fighter") + ")";
	if (sti(pchar.Fellows.Passengers.navigator) == sti(Character.index)) sProf += " (" + XI_ConvertString("navigator") + ")";
	if (sti(pchar.Fellows.Passengers.boatswain) == sti(Character.index)) sProf += " (" + XI_ConvertString("boatswain") + ")";
	if (sti(pchar.Fellows.Passengers.cannoner) == sti(Character.index)) sProf += " (" + XI_ConvertString("cannoner") + ")";
	if (sti(pchar.Fellows.Passengers.doctor) == sti(Character.index)) sProf += " (" + XI_ConvertString("doctor") + ")";
	if (sti(pchar.Fellows.Passengers.carpenter) == sti(Character.index)) sProf += " (" + XI_ConvertString("carpenter") + ")";
    if (sti(pchar.Fellows.Passengers.treasurer) == sti(Character.index)) sProf += " (" + XI_ConvertString("treasurer") + ")";
	Character.lastnamesave = Character.lastname;
	Character.lastname = Character.lastname + sProf;

	dialogRun = true;
	dialogSelf = false;
	LAi_Character_StartDialog(mainChr, Character);
	LAi_Character_StartDialog(Character, mainChr);
	SendMessage(mainChr, "lsl", MSG_CHARACTER_EX_MSG, "InDialog", 1);
	SendMessage(Character, "lsl", MSG_CHARACTER_EX_MSG, "InDialog", 1);
	//Запускаем диалог
	Dialog.CurrentNode = CharacterRef.Dialog.CurrentNode;
	startDialogMainCounter = 0;
	SetEventHandler("frame", "StartDialogMain", 1);
	SetTimeScale(0.0);
	if (locCameraCurMode == LOCCAMERA_FOLLOW && !CheckAttribute(loadedLocation, "lockCamAngle") && mainChr.location.group != "sit") // для квестов
	{
		SetCameraDialogMode(Character);  // boal
	}
	return true;
}

int startDialogMainCounter = 0;
int dialogWaitGreetingSound = 0;
string dialogGreetingSound = "";

// LEO: фон окна диалога по настройке игрока. Атрибут заводится лишь при первом заходе в
//   настройки, поэтому читаем с проверкой - иначе имя текстуры собиралось пустым.
string GetDialogBackgroundTexture()
{
	string sStyle = "1";
	if (CheckAttribute(&InterfaceStates, "DialogStyle")) sStyle = InterfaceStates.DialogStyle;
	return "dialog\\" + "dialog" + sStyle + ".dds";
}

void StartDialogMain()
{
	startDialogMainCounter++;
	if(startDialogMainCounter < 3) return;

	DelEventHandler("frame", "StartDialogMain");
	Dialog.backgroundTexture = GetDialogBackgroundTexture();
	// Gregg: модельный портрет собеседника — по настройке игрока. Ставим ДО CreateEntity: он зовёт Init(),
	//        а тот читает атрибуты. Позже — и движок увидит значение, оставшееся от прошлого диалога.
	if (CheckAttribute(&InterfaceStates, "DialogPortrait")) Dialog.Portrait = sti(InterfaceStates.DialogPortrait);
	CreateEntity(&Dialog, "dialog");
	Dialog.headModel = CharacterRef.headModel;
	Dialog.gender = CharacterRef.sex;

	DeleteAttribute(&Dialog,"Links");
	DeleteAttribute(&Dialog,"Text");
	//#20190615-01
	DeleteAttribute(&Dialog,"CancelDelay");

	// LEO: задержка приёма ввода диалога (сек) из настройки-слайдера — движок гейтит DlgAction первые N сек
	//      на каждой ноде, чтобы маш по кнопкам ударов (внезапный диалог поверженного капитана) не скипал ноды
	if (CheckAttribute(&InterfaceStates, "DialogInputDelay")) Dialog.InputDelay = stf(InterfaceStates.DialogInputDelay);
	else Dialog.InputDelay = 0.5;
	if (CheckAttribute(&InterfaceStates, "DialogDelay")) { if (sti(InterfaceStates.DialogDelay) == 0) Dialog.InputDelay = 0.0; } // опция выключена -> без задержки

	if(CheckAttribute(CharacterRef, "greeting"))
	{
		if(CharacterRef.greeting != "")
		{
			if (!CheckAttribute(CharacterRef, "greeting.minute"))
			{
				dialogGreetingSound = CharacterRef.greeting;
				dialogWaitGreetingSound = 0;
				SetEventHandler("frame", "DialogPlayGreeting", 1);
			}
			else
			{
				if (sti(CharacterRef.greeting.minute) != sti(Environment.date.min))
				{
					dialogGreetingSound = CharacterRef.greeting;
					dialogWaitGreetingSound = 0;
					SetEventHandler("frame", "DialogPlayGreeting", 1);
				}
			}
		}
	}

	object persRef = GetCharacterModel(Characters[GetMainCharacterIndex()]);
	SendMessage(&Dialog, "lii", 0, &Characters[GetMainCharacterIndex()], &persRef);

	object charRef = GetCharacterModel(Characters[makeint(CharacterRef.index)]);
	SendMessage(&Dialog, "lii", 1, &Characters[makeint(CharacterRef.index)], &charRef);

	LayerSetRealize(REALIZE)
	LayerAddObject(REALIZE,Dialog,-256);
	Set_inDialog_Attributes();
	ProcessDialogEvent();

	SetEventHandler("DialogEvent","ProcessDialogEvent",0);

	Event(EVENT_DIALOG_START,"");
}

void DialogPlayGreeting()
{
	dialogWaitGreetingSound++;
	if(dialogWaitGreetingSound < 10) return;
	dialogWaitGreetingSound = 0;
	DelEventHandler("frame", "DialogPlayGreeting");
	Dialog.greeting = CharacterRef.greeting;
	CharacterRef.greeting.minute = GetMinute();
}

//Начать диалог с самим собой
void SelfDialog(ref Character)
{
	//Если диалог запущен, выходим
	if(dialogRun != false) return false;
	//Если когото не заведено, выходим
	if(!IsEntity(Character)) return false;
	//Проверим на существование текущего нода
	if(!CheckAttribute(Character, "Dialog.CurrentNode"))
	{
		Trace("SelfDialog: Character <" + Character.id + "> can't have field Dialog.CurrentNode, exit from dialog!")
		return false;
	}
	//Сохраняем ссыклу на того с кем говорим
	CharacterRef = Character;
	// Попытка загрузить текст дилога (о гарде см. StartDialog выше)
	string sSelfDlgFile = "";
	bool bSelfDlgLoaded = false;
	if( CheckAttribute(Character, "Dialog.Filename") ) sSelfDlgFile = Character.Dialog.Filename;
	if( sSelfDlgFile != "" ) bSelfDlgLoaded = LoadDialogFiles(sSelfDlgFile);
	if( !bSelfDlgLoaded ) {
		// имеем ошибочный диалог
		Trace("SelfDialog: Character <" + Character.id + "> has no dialog file, using error_dialog.c");
		if( !LoadDialogFiles("error_dialog.c") ) {
			return false;
		}
	}
	//Если персонаж не готов говорить выходим
	LAi_Character_CanDialog(Character, Character);
	//Можем начинать диалог
	dialogRun = true;
	dialogSelf = true;
	SendMessage(Character, "lsl", MSG_CHARACTER_EX_MSG, "InDialog", 1);
	//Запускаем диалог
	Dialog.CurrentNode = CharacterRef.Dialog.CurrentNode;
	// Gregg: портрет собеседника — по настройке игрока, до CreateEntity (см. StartDialogMain)
	if (CheckAttribute(&InterfaceStates, "DialogPortrait")) Dialog.Portrait = sti(InterfaceStates.DialogPortrait);
	// LEO: фон - как в обычном диалоге. Раньше в мыслях вслух его не ставили вовсе, и окно
	//   самодиалога (билд при старте без корабля, реплики ГГ на берегу) шло без фона.
	Dialog.backgroundTexture = GetDialogBackgroundTexture();
	CreateEntity(&Dialog, "dialog");
	Dialog.headModel = Character.headModel;
	Dialog.gender = Character.sex;

	// LEO: задержка приёма ввода (как в DialogMain), из настройки
	if (CheckAttribute(&InterfaceStates, "DialogInputDelay")) Dialog.InputDelay = stf(InterfaceStates.DialogInputDelay);
	else Dialog.InputDelay = 0.5;
	if (CheckAttribute(&InterfaceStates, "DialogDelay")) { if (sti(InterfaceStates.DialogDelay) == 0) Dialog.InputDelay = 0.0; } // опция выключена -> без задержки

	object persRef = GetCharacterModel(Characters[GetMainCharacterIndex()]);
	SendMessage(&Dialog, "lii", 0, Character, &persRef);
	SendMessage(&Dialog, "lii", 1, Character, &persRef);

	LayerSetRealize(REALIZE)
	LayerAddObject(REALIZE,Dialog,-256);
	Set_inDialog_Attributes();
	ProcessDialogEvent();

	SetEventHandler("DialogEvent","ProcessDialogEvent",0);

	Event(EVENT_DIALOG_START,"");
}

//Закончить диалог
void DialogExit()
{
	//Если диалога уже не ведётся, выйдем
	if(dialogRun == false) return;
	if (checkattribute(CharacterRef, "lastnamesave")) CharacterRef.lastname = CharacterRef.lastnamesave;

	DelEventHandler("frame", "DialogPlayGreeting");
	//Освобождаем ресурсы
	DeleteClass(&Dialog);
	if(FullDialogPath!="")
	{
		if (CheckAttribute(CharacterRef, "FileDialog2"))
		{
		    UnloadSegment(CharacterRef.FileDialog2);
		    // мешало выходу в сегменте DeleteAttribute(CharacterRef, "FileDialog2");
		}
		UnloadSegment(FullDialogPath);
	}
	if(PathDlgLngExtn!="") UnloadSegment(PathDlgLngExtn);
	if(dialogSelf == false)
	{
		//Ссылка на главного персонажа
		ref mainChr = GetMainCharacter();
		//Отметим, что персонажи освободились от диалога
		LAi_Character_EndDialog(mainChr, CharacterRef);
		LAi_Character_EndDialog(CharacterRef, mainChr);
		SendMessage(mainChr, "lsl", MSG_CHARACTER_EX_MSG, "InDialog", 0);
		SendMessage(CharacterRef, "lsl", MSG_CHARACTER_EX_MSG, "InDialog", 0);
		// boal
		if (locCameraCurMode != LOCCAMERA_FOLLOW)
		{
			locCameraTarget(mainChr);
			locCameraFollow();
		}
	}else{
		LAi_Character_EndDialog(CharacterRef, CharacterRef);
		SendMessage(CharacterRef, "lsl", MSG_CHARACTER_EX_MSG, "InDialog", 0);
	}
	dialogRun = false;
	//Сообщим об окончании диалога
	PostEvent(EVENT_DIALOG_EXIT, 1, "l", sti(CharacterRef.index));
}

//Это событие приходит от Player
void StartDialogWithMainCharacter()
{
	if(LAi_IsBoardingProcess()) return;
	if(dialogDisable) return;
	//С кем хотим говорить
	int person = GetEventData();
	//Сими с собой не беседуем
	//Boyer fix #20170326-01 Invalid index -1 [size:1000]
	if(person<0) return;
	if(person == GetMainCharacterIndex()) return;
	//С непрогруженными персонажами не беседуем
	if(!IsEntity(&Characters[person])) return;
	//Начинаем диалог
	DialogMain(&Characters[person]);
}

bool LoadDialogFiles(string dialogPath)
{
	FullDialogPath = "dialogs\" + LanguageGetLanguage() + "\" + dialogPath;

	bool retVal;

	if( !LoadSegment(FullDialogPath) )
	{
		Trace("Dialog: Missing dialog file: " + FullDialogPath);
		retVal = false;
		// лишнее это UnloadSegment(FullDialogPath);
	} else {
		if(!retVal) {
			retVal = true;
			PathDlgLngExtn = "";
		}
	}

	return retVal;
}


string DText(string sString)
{
	return sString;
}

bool SetCameraDialogMode(ref chrRef)
{
	float x1,y1,z1, x2, y2, z2;
	if( false==GetCharacterPos(pchar,&x1,&y1,&z1) ) return false;
    if( false==GetCharacterPos(chrRef,&x2,&y2,&z2) ) return false;

    float a = 0.1;
    float len = GetDistance2D(x1,z1, x2,z2);
	float dx = x1*(1-a)+x2*a;
	float dz = z1*(1-a)+z2*a;
	len = 1;
	float s1 = (dx-x1)*len;
	float s2 = (dz-z1)*len;

    float xcam;
	float zcam;
	if (rand(1) == 0)
	{
		xcam = dx-s2;
		zcam = dz+s1;
	}
	else
	{
		xcam = dx+s2;
		zcam = dz-s1;
	}
	locCameraTarget(chrRef);
	float fH = 1.7;
	if (chrRef.location.group == "sit")
	{
	    fH = 1.15;
	}
	if (CheckAttribute(chrRef, "dialogCamH"))   // LEO: индивидуальная высота камеры диалога (напр. сидящие на земле у костра)
	{
	    return locCameraFromToPos(x1, y1+1.15, z1, false, x2, y2-1.0, z2);
	}
	return locCameraToPos(xcam,y1+fH,zcam,false);
}
