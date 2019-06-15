#include "stdafx.h"
#include "difficulty.h"

char* DifficultyToString(Difficulty difficulty) {
	switch (difficulty)
	{
	case Easy:
		return "Easy";
		break;
	case Normal:
		return "Normal";
		break;
	case Hard:
		return "Hard";
		break;
	case Extreme:
		return "Extreme";
		break;
	case Extra:
		return "EX-Extreme";
		break;
	}
	return "Unknown";
}