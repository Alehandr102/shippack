// Утилиты для городских диалогов: проверка наличия файла перед LoadSegment,
// чтобы движок не сыпал в лог "COMPILE ERROR ... file not found" на города без своего файла.

// Проверка существования файла через движковый FINDFILESINTODIRECTORY.
// folderName задаётся от корня игры, напр. "PROGRAM\DIALOGS\russian\Church".
bool IsFileExist(string folderName, string fileName)
{
	object fileFinder;
	fileFinder.dir  = folderName;
	fileFinder.mask = fileName;
	CreateEntity(&fileFinder, "FINDFILESINTODIRECTORY");
	DeleteClass(&fileFinder);
	aref fileList;
	makearef(fileList, fileFinder.filelist);
	return GetAttributesNum(fileList) > 0;
}

// Есть ли у города свой файл диалога <sCity><sSuffix> в подпапке DIALOGS\<язык>\<sSubDir>.
// Пример: HasCityDialogFile("Church", NPChar.City, "_Church.c").
bool HasCityDialogFile(string sSubDir, string sCity, string sSuffix)
{
	return IsFileExist("PROGRAM\DIALOGS\" + LanguageGetLanguage() + "\" + sSubDir, sCity + sSuffix);
}
