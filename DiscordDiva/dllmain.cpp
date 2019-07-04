// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "dllmain.h"
#include <detours.h>
#pragma comment(lib, "detours.lib")

char* IS_PLAYING_GAME = (char*)0x140d1e480;
char* IS_PV = (char*)0x14cc53b6d;
char* DIFFICULTY = (char*)0x14cc12444;
char* IS_EXTRA = (char*)0x14cc12448;

Difficulty GetDifficulty() {
	if (*IS_EXTRA) {
		return Difficulty::Extra;
	}
	return (Difficulty)(*DIFFICULTY);
}

SongData GetSongData() {
	SongData sd;
	sd.isLong = *(char*)0x140D0A590;
	memcpy(sd.songName, (void*)0x140D0A578, sizeof(sd.songName));
	sd.songID = *(int*)0x140CDD8E0;
	return sd;
}

void GetSongName(SongData song, char* buffer, rsize_t bufferSize) {
	char* defaultSongName;
	if (song.isLong > 0x0f)
	{
		defaultSongName = *(char**)song.songName;
	}
	else
	{
		defaultSongName = song.songName;
	}
	GetOverrideName(song.songID, defaultSongName, buffer, bufferSize);
}
void GetSongName(char* buffer, rsize_t bufferSize) {
	auto song = GetSongData();
	return GetSongName(song, buffer, bufferSize);
}

char lastState = 0x01;

void OnGameStateChange() {
	auto song = GetSongData();
	//Filter out the Dummy stage
	char isPlayingGame = song.songID == 999 ? 0 : *IS_PLAYING_GAME;
	if (isPlayingGame != lastState)
	{
		char songName[100];
		lastState = isPlayingGame;
		time_t ltime;
		time(&ltime);
		//Getting the song name has turned out to be quite a challenge. If it isn't required, don't do it.
		if (isPlayingGame)
		{
			GetSongName(song, songName, sizeof(songName));
		}
		ChangeActivity(isPlayingGame, songName, *IS_PV, GetDifficulty(), (long long)ltime);
	}
}

signed __int64 hookedDivaSongStart(__int64 a1, __int64 a2)
{
	divaSongStart(a1, a2);
	OnGameStateChange();
	return 0;
}

signed __int64 hookedDivaSongEnd(__int64 a1)
{
	divaSongEnd(a1);
	OnGameStateChange();
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		std::cout << DISCORD_PREFIX_ << "Loading DivaDiscord..." << std::endl;
		std::cout << DISCORD_PREFIX_ << "Injecting hooks" << std::endl;

		//InstallHook((void*)0x1400dfaf8, (void*)OnGameStateChange, 17);
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaSongStart, (PVOID)hookedDivaSongStart);
		DetourTransactionCommit();

		//InstallHook((void*)0x1400DFEC8, (void*)OnGameStateChange, 14);
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaSongEnd, (PVOID)hookedDivaSongEnd);
		DetourTransactionCommit();

		std::cout << DISCORD_PREFIX_ << "Connecting to Discord" << std::endl;
		SetupDiscord();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		StopDiscord();
		break;
	}
	return TRUE;
}