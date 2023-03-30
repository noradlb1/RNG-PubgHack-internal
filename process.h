#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <Psapi.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "Psapi.lib")

class PIDManager
{
public:
	PIDManager();
	~PIDManager();
	static int GetProcessIdByName(LPCTSTR szProcess);
	static std::wstring GetProcessNameById(int pid);
	static BOOL EnableDebugPriv();
	static DWORD_PTR GetModuleBase(DWORD dwPid, LPCTSTR szModName);
	static int GetAowProcId(const wchar_t* procname);
	static void killProcessByName(LPCWSTR name);
};