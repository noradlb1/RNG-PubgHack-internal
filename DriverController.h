#pragma once
class DriverController
{
public:
	DriverController(HANDLE hProcess);
	~DriverController();
	bool WriteProcessMemory(uintptr_t Address, void* Buffer, unsigned __int32 Length);
	bool ReadProcessMemory(uintptr_t Address, void* Buffer, unsigned __int32 Length);
	DWORD GetModuleInfo(const char* moduleName);
private:
	unsigned __int32 TargetProcessPid;
	unsigned __int32 CurrentProcessPid;
	HANDLE DriverHandle;
	HANDLE hProcess;

	unsigned __int32 GetProcessPidByName(const wchar_t* ProcessName);
};
