#pragma once

#include <winsock2.h>
#include <exception>
#include <iostream>
#include <ws2tcpip.h>
#include <stdio.h>
#include "discord.h"
#include "difficulty.h"
#include <time.h>
#include "difficulty.h"

void InjectDivaHooks(HMODULE hModule);
struct SongData {
	int songID;
	char unknown[4];
	char songName[24];
	char isLong;
};
char* GetSongName();
char* GetSongName(SongData* song);
SongData* GetSongData();