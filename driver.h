DWORD m_pid;
DWORD usermode_pid;
DWORD m_base;
DWORD m_game_manager;
DWORD isin_game;
DWORD m_gdi32base;
HANDLE m_Handle;
#include "DriverController.h"
#include <emmintrin.h>
#include <intrin.h>
DriverController* Controller = nullptr;
inline bool issmartgaga = false;
#include <list>
#include "ParallelWorker.h"
template <typename T>
T read(const uintptr_t address)
{
	T buffer{ };
	Controller->ReadProcessMemory(address, &buffer, sizeof(T));

	return buffer;
}

template<typename T>
bool write(uintptr_t address, T buffer)
{
	if (address > 0x7FFFFFFFFFFF || address < 1) return 0;

	return Controller->WriteProcessMemory(address, &buffer, sizeof(T));

	return true;
}
bool readwtf(uintptr_t Address, void* Buffer, SIZE_T Size)
{
	if (Address > 0x7FFFFFFFFFFF || Address < 1) return 0;

	Controller->ReadProcessMemory(Address, Buffer, Size);

	return true;
}
std::string ReadString(DWORD Adress)
{
	char bstring[128];
	Controller->ReadProcessMemory((unsigned long)Adress, &bstring, 64);
	return bstring;
}
typedef struct _MEMORY_REGION
{
	DWORD_PTR dwBaseAddr;
	DWORD_PTR dwMemorySize;
}MEMORY_REGION;

int find(BYTE* buffer, int dwBufferSize, BYTE* bstr, DWORD dwStrLen) {
	if (dwBufferSize < 0) {
		return -1;
	}
	DWORD  i, j;
	for (i = 0; i < dwBufferSize; i++) {
		for (j = 0; j < dwStrLen; j++) {
			if (buffer[i + j] != bstr[j] && bstr[j] != '?')
				break;
		}
		if (j == dwStrLen)
			return i;
	}
	return -1;
}
BYTE testxqw[32];
struct Modules
{
	uintptr_t libanogs = 0;
    uintptr_t libue4 = 1308819456;
	uintptr_t libgcloud = 0;
	uintptr_t libtprt = 0;
	uintptr_t libtdatamaster = 0;
	uintptr_t libcubehawk = 0;
	uintptr_t libigshare = 0;
    uint32_t libc = 0;
};
Modules GetModules()
{
	MEMORY_BASIC_INFORMATION mbi;
	SYSTEM_INFO system_information;
	GetSystemInfo(&system_information);
	uintptr_t dwAddress = reinterpret_cast<uintptr_t>(system_information.lpMinimumApplicationAddress);
	uintptr_t dwEndAddr = reinterpret_cast<uintptr_t>(system_information.lpMaximumApplicationAddress);
	VirtualQueryEx(m_Handle, (LPCVOID)dwAddress, &mbi, sizeof(mbi));
	Modules gModules{};
	for (uintptr_t curr = dwAddress; curr < dwEndAddr; curr += mbi.RegionSize)
	{
		if (!VirtualQueryEx(m_Handle, (LPCVOID)curr, &mbi, sizeof(mbi))) break;
        if (mbi.State == MEM_COMMIT && (mbi.Protect & (PAGE_READONLY | PAGE_EXECUTE_READ)) && mbi.Type == MEM_PRIVATE)
		{
			if ((mbi.Protect & PAGE_GUARD) != 0 || (mbi.Protect & (PAGE_NOACCESS | PAGE_WRITECOPY | PAGE_TARGETS_INVALID)) != 0)
			{
				curr += mbi.RegionSize;
				continue;
			}
			uintptr_t BaseAdress = (uintptr_t)mbi.BaseAddress;
            
			if (read<uintptr_t>(BaseAdress) == 1179403647)
			{
				if (read<BYTE>(BaseAdress + 0x12) == 0x28)
				{
                    
					if (read<uintptr_t>(BaseAdress + 0x20) == 126506532 && gModules.libue4 == NULL)
					{
                        gModules.libue4 = 1308819456;

						printf("libue4 %x\n", gModules.libue4);
					}
					if (read<uintptr_t>(BaseAdress + 0x20) == 126494244 && gModules.libue4 == NULL)
					{
                        gModules.libue4 = 1308819456;
						//printf("libue4 %x\n", gModules.libue4);
					}
					if (read<uintptr_t>(BaseAdress + 0x20) == 3984728 && gModules.libanogs == NULL)
					{
						gModules.libanogs = BaseAdress;
						//printf("libtersafe %x\n", gModules.libtersafe);
					}
					if (read<uintptr_t>(BaseAdress + 0x20) == 3645668 && gModules.libgcloud == NULL)
					{
						gModules.libgcloud = BaseAdress;
						//printf("libgcloud %x\n", gModules.libgcloud);
					}
					if (read<uintptr_t>(BaseAdress + 0x20) == 479392 && gModules.libtprt == NULL)
					{
						gModules.libtprt = BaseAdress;
						//printf("libtprt %x\n", gModules.libtprt);
					}
					if (read<uintptr_t>(BaseAdress + 0x20) == 2486336 && gModules.libtdatamaster == NULL)
					{
						gModules.libtdatamaster = BaseAdress;
						//printf("libtdatamaster %x\n", gModules.libtdatamaster);
					}
					if (read<uintptr_t>(BaseAdress + 0x20) == 874756 && gModules.libcubehawk == NULL)
					{
						gModules.libcubehawk = BaseAdress;
						//printf("libcubehawk %x\n", gModules.libcubehawk);
					}
					if (read<uintptr_t>(BaseAdress + 0x20) == 12668 && gModules.libigshare == NULL)
					{
						gModules.libigshare = BaseAdress;
						//printf("libcubehawk %x\n", gModules.libcubehawk);
					}
				}
                if (read<uint8_t>(BaseAdress + 0x12) == 0x03)
                {
                    if (read<uint32_t>(BaseAdress + 0x20) == 1043792 && gModules.libc == NULL)
                    {
                        gModules.libc = BaseAdress;
                    }
                }
			}
		}
	}
	return gModules;
}

inline
bool isMatch(PBYTE addr, PBYTE pat, size_t length)
{
	for (size_t n = 0; n < length; n++)
	{
		if (addr[n] == pat[n] || pat[n] == '?')
		{
			continue;
		}
		return false;
	}
	return true;
}

#include <mutex>
using RegionHolder = std::pair<uintptr_t, size_t>;

std::vector<uintptr_t> FindPatternEx(uintptr_t dwStart, uintptr_t dwEnd, const std::list<BYTE>& bytes, const bool firstOnly = false)
{
	std::vector<RegionHolder> mem_regions;
	std::vector<uintptr_t> result;
	MEMORY_BASIC_INFORMATION info = { 0 };
	uintptr_t currentAddress = dwStart;
	size_t length = bytes.size();
	BYTE* pattern = new BYTE[length];
	std::copy(bytes.begin(), bytes.end(), pattern);
	do
	{
		if (info.State != MEM_COMMIT || (info.Protect & PAGE_GUARD) != 0 || (info.Protect & (PAGE_NOACCESS | PAGE_WRITECOPY | PAGE_TARGETS_INVALID)) != 0)
		{
			currentAddress += info.RegionSize;
			continue;
		}
		const size_t allocSize = dwEnd - dwStart >= info.RegionSize ? info.RegionSize : dwEnd - dwStart;
		mem_regions.emplace_back(currentAddress, allocSize);
		currentAddress += info.RegionSize;
	} while ((VirtualQueryEx(m_Handle, reinterpret_cast<LPVOID>(currentAddress), &info, sizeof info) == sizeof info && currentAddress < dwEnd));
	ParallelQueue<std::vector<RegionHolder>, RegionHolder>
		worker(mem_regions, 0, std::thread::hardware_concurrency(), [&](RegionHolder& memRegion, ParallelOptions& options)
			{
				const auto pBuf = new BYTE[memRegion.second];
				Controller->ReadProcessMemory(memRegion.first, pBuf, memRegion.second);

				for (size_t j = 0; j <= memRegion.second; j++)
				{
					if (isMatch((PBYTE)pBuf + j, pattern, length))
					{
						std::lock_guard<std::mutex> lock(options.Locker);
						result.push_back(memRegion.first + j);
						if (firstOnly)
						{
							options.ForceStop = true;
							break;
						}
					}
				}
				delete[] pBuf;
			});

	worker.Start();
	worker.WaitAll();

	return result;
}
