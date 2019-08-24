#pragma once

#include <thread>
#include <stdio.h>
#include <iostream>
#include <process.h>
#include "difficulty.h"
#include <mutex>
#include <fstream>
#include "config.h"

void SetupDiscord();
void ChangeActivity(int isPlaying, char* songName, int isPV, Difficulty difficulty, long long timeSinceStart);
void StopDiscord();