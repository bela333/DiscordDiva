#include "stdafx.h"
#include "config.h"

//From Rayduxz's DIVA-Loader
std::string DirPath() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

std::string CONFIG_FILE_STRING = DirPath() + "\\plugins\\discorddiva.ini";
LPCSTR CONFIG_FILE = CONFIG_FILE_STRING.c_str();

void GetOverrideName(int songID, char* defaultName, char* buffer, rsize_t bufferSize) {
	char configID[10];
	sprintf_s(configID, "pv_%03d", songID);
	GetPrivateProfileStringA("override", configID, defaultName, buffer, bufferSize, CONFIG_FILE);
}