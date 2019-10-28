#include "stdafx.h"
#include "discord.h"


struct IDiscordCore* core = nullptr;
struct IDiscordActivityManager* activities = nullptr;
std::mutex coreMutex;

void DiscordThread(void*) {
	for (;;)
	{
		coreMutex.lock();
		if (core != nullptr)
		{
			core->run_callbacks(core);
			coreMutex.unlock();
		}
		else
		{
			coreMutex.unlock();
			break;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

}
void SetupDiscord() {
	EDiscordResult result;
	struct DiscordCreateParams params;
	std::cout << DISCORD_PREFIX_ << "Creating params list" << std::endl;
	DiscordCreateParamsSetDefault(&params);
	params.client_id = 587576492301746199;
	params.flags = DiscordCreateFlags_NoRequireDiscord;
	std::cout << DISCORD_PREFIX_ << "Creating client" << std::endl;
	result = DiscordCreate(DISCORD_VERSION, &params, &core);
	if (result != DiscordResult_Ok)
	{
		std::cout << DISCORD_PREFIX_ << "Failed to create Discord client. Reason: " << result << std::endl;
		core = nullptr;
		return;
	}
	activities = core->get_activity_manager(core);
	_beginthread(DiscordThread, 20, NULL);
}

void StopDiscord() {
	coreMutex.lock();
	core->destroy(core);
	core = nullptr;
	coreMutex.unlock();
}

void UpdateActivityCallback(void* data, enum EDiscordResult result)
{
	if (result == DiscordResult_Ok)
	{
		std::cout << DISCORD_PREFIX_ << "Activity updated!" << std::endl;
	}
	else
	{
		std::cout << DISCORD_PREFIX_ << "Couldn't update activity: " << result << std::endl;
	}
	
}

void ChangeActivity(int isPlaying, char* songName, int isPV, Difficulty difficulty, long long timeSinceStart) {
	std::cout << DISCORD_PREFIX_ << "Updating activity" << std::endl;


	struct DiscordActivity activity;
	memset(&activity, 0, sizeof(activity));
	strcpy_s(activity.assets.large_image, "icon");
	strcpy_s(activity.assets.large_text, "Hatsune Miku: Project DIVA Future Tone");
	if (isPlaying)
	{
		strcpy_s(activity.details, songName);
		if (isPV)
			strcpy_s(activity.state, "Watching a PV");
		else
			sprintf_s(activity.state, "Playing at %s difficulty.", DifficultyToString(difficulty));
		activity.timestamps.start = timeSinceStart;
	}
	else
	{
		strcpy_s(activity.state, "Waiting");
	}
	coreMutex.lock();
	if (activities == nullptr || core == nullptr)
	{
		coreMutex.unlock();
		return;
	}
	activities->update_activity(activities, &activity, NULL, UpdateActivityCallback);
	coreMutex.unlock();
}