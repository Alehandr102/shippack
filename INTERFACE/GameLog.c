// =====================================================================================
//  LEO: «Журнал игры» — прокручиваемый список всех записей Log_Info за текущую сессию.
//  Данные берутся из движкового хранилища (нативы GameLog*), которое ПЕРЕЖИВАЕТ загрузку
//  сейва и СБРАСЫВАЕТСЯ на рестарте игры. Log_TestInfo сюда НЕ попадает.
//  Прокрутка: колесо мыши / стрелки (нода FORMATEDTEXT скроллит по строкам сама).
// =====================================================================================

// нативы GameLogGetCount/GameLogGetLine объявлены глобально в locations.c (#libriary ScriptLocationLibrary)

void InitInterface(string iniName)
{
	GameInterface.title = "";

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);

	SetEventHandler("InterfaceBreak", "ProcessCancelExit", 0);
	SetEventHandler("exitCancel", "ProcessCancelExit", 0);

	// заголовок
	string sTitle = "Game Log";
	if (LanguageGetLanguage() == "russian") sTitle = "Журнал игры";
	SetFormatedText("TITLE_TEXT", sTitle);

	// наполнение списком записей
	SetFormatedText("LOG_TEXT", "");
	int n = GameLogGetCount();
	if (n <= 0)
	{
		string sEmpty = "(empty)";
		if (LanguageGetLanguage() == "russian") sEmpty = "(пусто)";
		SendMessage(&GameInterface, "lsle", MSG_INTERFACE_MSG_TO_NODE, "LOG_TEXT", 0, sEmpty);
	}
	else
	{
		// LEO: выводим от СВЕЖИХ к старым. Раньше список шёл в хронологическом порядке, и самое
		//   интересное оказывалось в самом низу - открыв журнал, приходилось сначала мотать колесом.
		//   Автопрокрутка в конец тут не помогала, поэтому просто переворачиваем порядок вывода.
		int i;
		for (i = n - 1; i >= 0; i--)
		{
			SendMessage(&GameInterface, "lsle", MSG_INTERFACE_MSG_TO_NODE, "LOG_TEXT", 0, GameLogGetLine(i));
		}
	}
}

void ProcessCancelExit()
{
	IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU); // выход возвращает в игровое меню (откуда пришли)
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak", "ProcessCancelExit");
	DelEventHandler("exitCancel", "ProcessCancelExit");

	interfaceResultCommand = exitCode;
	EndCancelInterface(false);
}
