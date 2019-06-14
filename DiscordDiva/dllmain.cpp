// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <winsock2.h>
#include <exception>
#include <iostream>
#include <ws2tcpip.h>
#include <stdio.h>
#include "discord.h"
#include "difficulty.h"
#include <time.h>




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
//TODO: Merge jmp stud and call stud
void InstallHook(void* source, void* destination, int length)
{
	const DWORD minLen = 0xE;

	if (length < minLen)
		//return NULL;
		return;

	//Jump to a certain address
	BYTE stub[] =
	{
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ?ptr [$+6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr???
	};

	//Thank God for Compiler Explorer
	//Call function and jump to a certain address
	BYTE call_stub[] = {
		0xe8, // call offset ?ptr [$+1]
		0x00, 0x00, 0x00, 0x00,  // ptr???
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ?ptr [$+11]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr???
	};

	void* trampoline = VirtualAlloc(0, length + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	void* callInstruction = VirtualAlloc(0, sizeof(call_stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	void* functionHack = VirtualAlloc(0, sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	DWORD oldProtect;
	//Remove protection from memory range of PDFT
	VirtualProtect(source, length, PAGE_EXECUTE_READWRITE, &oldProtect);

	//Calculate offset for call stub
	int offset = (int)((long long)functionHack - (long long)(callInstruction) - 5);
	//Populate call stub with offset
	memcpy(&call_stub[1], &offset, 4);
	//Populate call stub with trampoline address
	memcpy(&call_stub[11], &trampoline, 8);
	//Copy call stub to persistent memory
	memcpy(callInstruction, call_stub, sizeof(call_stub));
	
	//Populate the jmp stub with the destination address
	memcpy(stub + 6, &destination, 8);
	//Copy jmp stub to hack
	memcpy(functionHack, stub, sizeof(stub));

	//Calculate address of next instruction in line
	DWORD64 returnAddress = (DWORD64)source + length;
	//Populate the jmp stub with it
	memcpy(stub + 6, &returnAddress, 8);
	//Copy original code to trampoline
	memcpy((void*)((DWORD_PTR)trampoline), source, length);
	//Populate trampoline with return address jump
	memcpy((void*)((DWORD_PTR)trampoline + length), stub, sizeof(stub));

	// Populate the jmp stub with the address of the call stub
	memcpy(stub + 6, &callInstruction, 8);
	//Overwrite original code
	memcpy(source, stub, sizeof(stub));

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

char* GetSongName() {
	size_t* ptr = (size_t*)0x140d0a920;
	ptr = (size_t*)(((char*)*ptr) + 0x20);
	unsigned int isLong = (*((int*)(ptr))) > 0xf;
	ptr = (size_t*)0x140d0a920;
	ptr = (size_t*)(((char*)*ptr) + 0x8);
	if (isLong)
	{
		return (char*)(*ptr);
	}
	return (char*)ptr;
}

void OnGameStateChange() {
	time_t ltime;
	time(&ltime);
	ChangeActivity(*IS_PLAYING_GAME, GetSongName(), *IS_PV, GetDifficulty(), (long long)ltime);
}

void InjectDivaHooks(HMODULE hModule) {
	InstallHook((void*)0x1400dfaf8, (void*)OnGameStateChange, 17);
	InstallHook((void*)0x1400DFEC8, (void*)OnGameStateChange, 14);
	//InstallHook((void*)0x000000014018CC40, (void*)DiscordThread, 0xe);
}
