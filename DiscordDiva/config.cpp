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

	int lenDefaultName = MultiByteToWideChar(CP_UTF8, 0, defaultName, -1, 0, 0);
	wchar_t* wDefaultName = new wchar_t[lenDefaultName];
	MultiByteToWideChar(CP_UTF8, 0, defaultName, -1, wDefaultName, lenDefaultName);

	//Populate wBuffer with either the default name or the override
	wchar_t* wBuffer = new wchar_t[bufferSize*2];
	GetPrivateProfileStringW(L"override", configID, wDefaultName, wBuffer, bufferSize*2, CONFIG_FILE);

	int lenName = WideCharToMultiByte(CP_UTF8, 0, wBuffer, -1, 0, 0, 0, 0);
	char* name = new char[lenName];
	WideCharToMultiByte(CP_UTF8, 0, wBuffer, -1, buffer, bufferSize, 0, 0);


	//Freeing resources
	delete[] wBuffer;
	delete[] wDefaultName;
	delete[] name;
	
}