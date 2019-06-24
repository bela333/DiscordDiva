// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "dllmain.h"




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		std::cout << DISCORD_PREFIX_ << "Loading DivaDiscord..." << std::endl;
		std::cout << DISCORD_PREFIX_ << "Injecting hooks" << std::endl;
		InjectDivaHooks(hModule);
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

//Modified code from Samyuu's TotallyLegitArcadeController
void InstallHook(void* source, void* destination, int length)
{
	const DWORD minLen = 0xE;

	if (length < minLen)
		//return NULL;
		return;

	//Jump to a certain address
	BYTE jump_stub[] =
	{
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ?ptr [$+6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr???
	};

	//Thank God for Compiler Explorer
	//Call function and jump to a certain address
	BYTE call_stub[] = {
		0xe8, // call offset ?ptr [$+1]
		0x00, 0x00, 0x00, 0x00,  // ptr???
	};

	BYTE* callInstruction = (BYTE*)VirtualAlloc(0, sizeof(call_stub) + length + sizeof(jump_stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	BYTE* functionHack = (BYTE*)VirtualAlloc(0, sizeof(jump_stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	DWORD oldProtect;
	//Remove protection from memory range of PDAFT
	VirtualProtect(source, length, PAGE_EXECUTE_READWRITE, &oldProtect);

	//Calculate offset for call stub
	int offset = (int)((long long)functionHack - (long long)(callInstruction) - 5);
	//Populate call stub with offset
	memcpy(&call_stub[1], &offset, 4);

	//Populate jump_stub with the address of the next instruction
	DWORD64 returnAddress = (DWORD64)source + length;
	memcpy(jump_stub + 6, &returnAddress, 8);

	//Copy call stub to allocated memory
	memcpy(&callInstruction[0], call_stub, sizeof(call_stub));
	//Copy old code to allocated memory
	memcpy(&callInstruction[sizeof(call_stub)], source, length);
	//Copy jump stub to allocated memory
	memcpy(&callInstruction[sizeof(call_stub) + length], jump_stub, sizeof(jump_stub));
	
	//Populate the jmp stub with the destination address
	memcpy(jump_stub + 6, &destination, 8);
	//Copy jmp stub to hack
	memcpy(functionHack, jump_stub, sizeof(jump_stub));

	// Populate the jmp stub with the address of the call stub
	memcpy(jump_stub + 6, &callInstruction, 8);
	//Overwrite original code
	memcpy(source, jump_stub, sizeof(jump_stub));

	//Fill unused memory with NOP
	for (int i = minLen; i < length; i++)
		*(BYTE*)((DWORD_PTR)source + i) = NOP_OPCODE;

	//Protect
	VirtualProtect(source, length, oldProtect, &oldProtect);
}




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

void InjectDivaHooks(HMODULE hModule) {
	InstallHook((void*)0x1400dfaf8, (void*)OnGameStateChange, 17);
	InstallHook((void*)0x1400DFEC8, (void*)OnGameStateChange, 14);
	//InstallHook((void*)0x000000014018CC40, (void*)DiscordThread, 0xe);
}
