#include <Windows.h>
#include <iostream>
#include <intrin.h>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <string>
#include <filesystem>

#include <Dwmapi.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "DriverController.h"
#include "xor.hpp"
#include <ntstatus.h>
#include <winternl.h>
#include "syscall.h"
#pragma comment (lib, "ntdll.lib")

auto NtWriteVirtualMemory = makesyscall<bool>(xorstr_("NtWriteVirtualMemory"));
auto NtReadVirtualMemory = makesyscall<bool>(xorstr_("NtReadVirtualMemory"));

unsigned __int32 DriverController::GetProcessPidByName(const wchar_t* ProcessName)
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);
	return 0;
}

DriverController::DriverController(HANDLE hProcess)
{
	this->hProcess = hProcess;
}

DriverController::~DriverController()
{
	if (this->hProcess != INVALID_HANDLE_VALUE)
		CloseHandle(this->hProcess);
}

bool DriverController::WriteProcessMemory(uintptr_t Address, void* Buffer, unsigned __int32 Length)
{
	if (!Address || !Buffer || !Length)
		return false;
	NtWriteVirtualMemory(this->hProcess, (LPCVOID)Address, Buffer, Length, 0);
	return true;
}

bool DriverController::ReadProcessMemory(uintptr_t Address, void* Buffer, unsigned __int32 Length)
{
	if (!Address || !Buffer || !Length)
		return false;

	NtReadVirtualMemory(this->hProcess, (LPCVOID)Address, Buffer, Length, 0);
	return true;
}
DWORD DriverController::GetModuleInfo(const char* moduleName)
{
	return 0;
}