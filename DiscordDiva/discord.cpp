#include "stdafx.h"
#include "discord.h"


void SetupDiscord() {

}

void StopDiscord() {

}

void ChangeActivity(int isPlaying, char* songName, int isPV, Difficulty difficulty, long long timeSinceStart) {
	char songNamePath[100];
	GetSongNamePath(songNamePath, sizeof(songNamePath));
	std::ofstream songNameFile (songNamePath);

	char difficultyPath[100];
	GetDifficultyPath(difficultyPath, sizeof(difficultyPath));
	std::ofstream difficultyFile(difficultyPath);

	std::cout << DISCORD_PREFIX_ << "Updating activity" << std::endl;
	if (isPlaying)
	{
		songNameFile << songName;
		difficultyFile << DifficultyToString(difficulty);
	}
	else
	{
		songNameFile << "";
		difficultyFile << "";
	}
	songNameFile.close();
	difficultyFile.close();
}