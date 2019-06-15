
#pragma once

enum Difficulty {
	Easy,
	Normal,
	Hard,
	Extreme,
	Extra
};

char* DifficultyToString(Difficulty difficulty);