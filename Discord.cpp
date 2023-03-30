#include "Discord.h"
#include <thread>

void Discord::Init()
{
    DiscordEventHandlers handle;
    memset(&handle, 0, sizeof(handle));
    Discord_Initialize(VMProtectDecryptStringA("731547748511252590"), &handle, 1, NULL);
}

void Discord::Update(const char* mode, const char* details,int64_t timestamp)
{
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.state = mode;
    discordPresence.details = details;
    discordPresence.startTimestamp = timestamp;
    discordPresence.largeImageKey = VMProtectDecryptStringA("razevip");
    discordPresence.largeImageText = VMProtectDecryptStringA("RAZE-VIP");
    Discord_UpdatePresence(&discordPresence);
}
