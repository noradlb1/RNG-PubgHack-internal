#pragma once
#include <discord_register.h>
#include <discord_rpc.h>
#include <VMProtectSDK.h>
#include <Windows.h>

class Discord
{
public:
	void Init();
	void Update(const char* mode, const char* details, int64_t timestamp);
};