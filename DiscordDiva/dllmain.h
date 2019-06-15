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