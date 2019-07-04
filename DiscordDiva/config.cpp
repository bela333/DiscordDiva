#include "stdafx.h"
#include "config.h"

//From Rayduxz's DIVA-Loader
std::wstring DirPath() {
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

std::wstring CONFIG_FILE_STRING = DirPath() + L"\\plugins\\discorddiva.ini";
const wchar_t* CONFIG_FILE = CONFIG_FILE_STRING.c_str();

void GetOverrideName(int songID, char* defaultName, char* buffer, rsize_t bufferSize) {
	wchar_t configID[7]; //pv_000\0
	swprintf_s(configID, L"pv_%03d", songID);

	//Converting arguments to wchar_t*
	//defaultName
	size_t defaultName_len = strlen(defaultName)+1;
	wchar_t* wDefaultName = new wchar_t[defaultName_len];
	mbstowcs_s((size_t*)NULL, wDefaultName, defaultName_len, defaultName, _TRUNCATE);

	wchar_t* wBuffer = new wchar_t[bufferSize];

	GetPrivateProfileStringW(L"override", configID, wDefaultName, wBuffer, bufferSize, CONFIG_FILE);

	//Convert buffer from wchar_t*
	wcstombs_s((size_t)NULL, buffer, bufferSize, wBuffer, _TRUNCATE);

	delete wDefaultName;
	delete wBuffer;
}