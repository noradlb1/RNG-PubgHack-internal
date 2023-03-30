#define _CRT_SECURE_NO_WARNINGS
#undef _DEBUG
// clang-format off
#include <winsock2.h>

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <io.h>
#include <cstdint>
#include <cstdlib>
#include <dwmapi.h>
#include <comdef.h>
#include <winternl.h>

#include <dwmapi.h>
#include <comdef.h>
#include <d3d11.h>
#include <d3d9.h>
#include <sapi.h>


#include <inttypes.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")
//#pragma comment(lib, "kdmapper.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Ws2_32.Lib")
#pragma comment(lib, "Wldap32.Lib")
#pragma comment(lib, "Crypt32.Lib")

#include "Imgui/imgui.h"
#include "Imgui/examples/imgui_impl_dx11.h"
#include "Imgui/examples/imgui_impl_win32.h"

#include "vector3.h"
#include "driver.h"
#include "process.h"
#include "colors.h"
#include "overlay.h"

#include <random>
#include <vector>
#include "xor.hpp"

#include "api.hpp"

#include <imgui_internal.h>
#include "resource.h"
#include "syscall.h"
#include "RNG.hpp"
#include <format>
#include "imgui_custom.hpp"
#include "images.hpp"
//#define smartgagaversion
// clang-format on
using namespace std;
using namespace r_auth;


const MARGINS Margin = {0, 0, Width, Height};
DWORD ScreenCenterX;
DWORD ScreenCenterY;

RECT GameRect = {NULL};
HWND GameWnd = NULL;
HWND SubGameWnd = NULL;
void create_console();
ISpVoice *pVoice = NULL;
HRESULT hrvoice;
HRESULT DirectXInit(HWND hWnd);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void SetupWindow();
WPARAM MainLoop();

int isTopwin();
#define TopWindowGame 11
#define TopWindowMvoe 22

static ID3D11Device *ms_vD3dDevice;
static ID3D11DeviceContext *ms_vDeviceContext;
static IDXGISwapChain *ms_vSwapChain;
static ID3D11RenderTargetView *ms_vRenderTargetView;

#define M_Name xorstr_(L"Magnifier")
HWND MyWnd = NULL;
MSG Message = {NULL};
// using namespace winreg;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);
// bool Load_Kernel_Driver();
#define M_PI 3.1415926535
HANDLE aimthread;
HANDLE cachethread;
auto NtProtectVirtualMemory =
    makesyscall<NTSTATUS>(xorstr_("NtProtectVirtualMemory"));
std::string emulatorpath;
NTSTATUS NtProtectVirtualMemoryEx(_In_ HANDLE hProcess, _In_ LPVOID lpAddress,
                                  _In_ SIZE_T dwSize, _In_ DWORD flNewProtect,
                                  _Out_ PDWORD lpflOldProtect)
{
    return NtProtectVirtualMemory(hProcess, &lpAddress, &dwSize, flNewProtect,
                                  &lpflOldProtect);
}

template <typename... T>
void writeBytes(uint32_t addr, T... towrite)
{
    BYTE pBuf[] = {towrite...};
    DWORD oldAccess = 0;
    NtProtectVirtualMemoryEx(m_Handle, (LPVOID)addr, sizeof(pBuf),
                             PAGE_EXECUTE_READWRITE, &oldAccess);
    Controller->WriteProcessMemory(addr, pBuf, sizeof(pBuf));
    // NtProtectVirtualMemoryEx(m_Handle, (LPVOID)addr, sizeof(pBuf), oldAccess,
    //	&oldAccess);
    // delete[] pBuf;
}
namespace offsets
{
    uint32_t PlayerName = 0x308;    // FString PlayerName;//[Offset: 0x5f8 , Size: 12]
    uint32_t Nation = 0x768;        // FString Nation;//[Offset: 0x644 , Size: 12]
    uint32_t PlayerUID = 0x684;     // FString PlayerUID;//[Offset: 0x664 , Size: 12]
    uint32_t viewWorld = 0x57811ed0; // 02 00 00 00 80 00 00 00 FF FF FF FF 00 00 00 00 01
    uint32_t gNames = 0x7767450;    // GNameEntryPoolAllocator + 0x14
    uint32_t GEngine = 0x7583B08;
    uint32_t GameViewport = 0x6ec;    // GameViewportClient* GameViewport;//[Offset: 0x6ec, Size: 4]
    uint32_t World = 0x7c;            // World* World;//[Offset: 0x58, Size: 4]
    uint32_t uLevel = 0x20;           // Level* PersistentLevel;//[Offset: 0x20 , Size: 4]
    uint32_t gameInstance = 0x24;     // NetDriver* NetDriver;//[Offset: 0x24 , Size: 4]
    uint32_t playerController = 0x60; // NetConnection* ServerConnection;//[Offset: 0x60, Size: 4]
    uint32_t playerCarry = 0x20;
    uint32_t uMyObject = 0x358; // Pawn* AcknowledgedPawn;//[Offset: 0x320 , Size: 4]
    uint32_t entityEntry = 0x70;
    uint32_t entityCount = 0x74;
    uint32_t uObjectWorld = 0x164;     // SceneComponent* RootComponent;//[Offset: 0x14c , Size: 4]
    uint32_t uObjectPosition = 0x118;  // Vector RelativeLocation;//[Offset: 0x120 , Size: 12]
    uint32_t RelativeRotation = 0x124; // Rotator RelativeRotation;//[Offset: 0x12c , Size: 12]
    uint32_t uObjectPose = 0x120;
    uint32_t ScopeFov = 0x130c;                       // float ScopeFov;//[Offset: 0x1030 , Size: 4]
    uint32_t Health = 0x9a4;                          // float Health;//[Offset: 0x780 , Size: 4]
    uint32_t HealthMax = 0x9a8;                       // float HealthMax;//[Offset: 0x784 , Size: 4]
    uint32_t NearDeathBreath = 0x1234;                // float NearDeathBreath;//[Offset: 0xe58 , Size: 4]
    uint32_t IsRobot = 0x919;                         // bool bIsAI;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x6e8 , Size: 1]
    uint32_t CurBulletNumInClip = 0xab8;              // int CurBulletNumInClip;//[Offset: 0x954 , Size: 4]
    uint32_t CurMaxBulletNumInOneClip = 0xacc;        // int CurMaxBulletNumInOneClip;//[Offset: 0x968 , Size: 4]
    uint32_t TeamID = 0x308;                          // int TeamID;//[Offset: 0x610 , Size: 4]
    uint32_t ParachuteComponent = 0xf98;              // CharacterParachuteComponent* ParachuteComponent
    uint32_t CurrentFallSpeed = 0x2c0;                // float CurrentFallSpeed;//[Offset: 0x1d0 , Size: 4]
    uint32_t Weapon = 0x1a9c - 4;                     // AnimStatusKeyList CurUpdateStatusKeyList - 4
    uint32_t ShootWeaponEntityComp = 0xbc0;           // ShootWeaponEntity* ShootWeaponEntityComp
    uint32_t BulletFireSpeed = 0x404;                 // float BulletFireSpeed
    uint32_t Mesh = 0x344;                            // SkeletalMeshComponent* Mesh;
    uint32_t AnimInstance = 0x740;                    // AnimInstance* AnimScriptInstance;
    uint32_t WheeledVehicleMovementComponent = 0x740; // WheeledVehicleMovementComponent* WheeledVehicleMovementComponent;
    uint32_t PlayerCameraManager = 0x368;             // PlayerCameraManager* PlayerCameraManager;//[Offset: 0x328 , Size: 4]
    uint32_t CameraCache = 0x370;                     // CameraCacheEntry CameraCache;//[Offset: 0x350 , Size: 1424]
    uint32_t ControlRotation = 0x31c;                 // Rotator ControlRotation;//[Offset: 0x2e0 , Size: 12]
    uint32_t CharacterWeaponManager = 0x17b0;         // CharacterWeaponManagerComponent* CharacterWeaponManager;//[Offset: 0x28a8 , Size: 4]
    uint32_t AliveTeamNum = 0x7a0;                    // int AliveTeamNum;//[Offset: 0x740 , Size: 4]
    uint32_t ServerStartTime = 0xd44;                 // FString FServerStartTime;//[Offset: 0xcec , Size: 12]
    uint32_t gmName = 0xfb4;                          // FString GameModeDisplayName;//[Offset: 0x6b4 , Size: 12]
    uint32_t gameState = 0x12c;                       // GameStateBase* GameState;//[Offset: 0x16c , Size: 4]
    uint32_t playerState = 0x53c;                     // PlayerState* PlayerState;//[Offset: 0x2e0 , Size: 4]
    uint32_t alivePlayers = 0x53c;                    // int PlayerNum;//[Offset: 0x53c , Size: 4]
    uint32_t aliveInGame = 0x79c;                     // int AlivePlayerNum;//[Offset: 0x73c , Size: 4]
    uint32_t getKills = 0x4cc;                        // int Kills;//[Offset: 0x468 , Size: 4]
    uint32_t CurrentWeaponReplicated = 0x46c;         // STExtraWeapon* CurrentWeaponReplicated;//[Offset: 0x41c , Size: 4]
    uint32_t AccessoriesVRecoilFactor = 0x860;        // float AccessoriesVRecoilFactor;//[Offset: 0x784 , Size: 4]
    uint32_t AccessoriesHRecoilFactor = 0x864;        // float AccessoriesHRecoilFactor;//[Offset: 0x788 , Size: 4]
    uint32_t RecoilKickADS = 0x968;                   // float RecoilKickADS;//[Offset: 0x890 , Size: 4]
    uint32_t AnimationKick = 0x980;                   // float AnimationKick;//[Offset: 0x8a8 , Size: 4]
    uint32_t ComponentVelocity = 0x1a0;               // Vector ComponentVelocity;//[Offset: 0x1a0 , Size: 12]
    uint32_t VehicleCommon = 0x6f8;                   // VehicleCommonComponent* VehicleCommon;//[Offset: 0x698 , Size: 4]
    uint32_t IsEngineStarted = 0x6d9;                 // bool bIsEngineStarted;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x679 , Size: 1]
    uint32_t VehicleHP = 0x50c;                       // float HP;//[Offset: 0x11c , Size: 4]
    uint32_t VehicleHPMax = 0x36c;                    // float HPMax;//[Offset: 0x118 , Size: 4]
    uint32_t VehicleFuel = 0x1fc;                     // float Fuel;//[Offset: 0x134 , Size: 4]
    uint32_t VehicleFuelMax = 0x1f8;                  // float FuelMax;//[Offset: 0x130 , Size: 4]
    uint32_t IsFPPGameMode = 0x680;                   // bool IsFPPGameMode;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x644 , Size: 1]
    uint32_t bIsGunADS = 0xb95;                       // bool bIsGunADS;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0xa99 , Size: 1]
    uint32_t ScopingCamera = 0x3324;                  // CameraComponent* ScopingCamera;//[Offset: 0x288c , Size: 4]
    uint32_t ScopeCameraFOV = 0x260;                  // float FieldOfView;//[Offset: 0x270 , Size: 4]
    uint32_t GodView = 0x1c20;                        // float StandHalfHeight;//[Offset: 0x18cc , Size: 4]
    uint32_t bDead = 0x9e0;                           // bool bDead;//(ByteOffset: 0, ByteMask: 1, FieldMask: 1)[Offset: 0x964 , Size: 1]
    uint32_t ExtraHitPerformScale = 0x130;            // float ExtraHitPerformScale;//[Offset: 0x894 , Size: 4]
    uint32_t DrawCrosshairWhenScope = 0x8dc;          // bool bDrawCrosshairWhenScope;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x820 , Size: 1]
    uint32_t bReplicatedIsStartParachute = 0x1f70;    // bool bReplicatedIsStartParachute;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x1b6c , Size: 1]
    uint32_t GameDeviationFactor = 0x8bc;             // float GameDeviationFactor;//[Offset: 0x860, Size: 4]
    uint32_t ShotGunHorizontalSpread = 0x8b8;         // float ShotGunHorizontalSpread;//[Offset: 0x85c, Size: 4]
    uint32_t ShotGunVerticalSpread = 0x8b4;           // float ShotGunVerticalSpread;//[Offset: 0x858, Size: 4]
    uint32_t SwitchWeaponSpeedScale = 0x1ba8;         // float SwitchWeaponSpeedScale;//[Offset: 0x1948, Size: 4]
    uint32_t PickUpRadius = 0x13d8;                   // float PickUpRadius;//[Offset: 0x11bc, Size: 4]
    uint32_t ShootInterval = 0x430;                   // float ShootInterval;//[Offset: 0x3f8 , Size: 4]
    uint32_t FakePing = 0x304;                        // byte Ping;//[Offset: 0x2e0 , Size: 1]
    uint32_t CharacterMovementComponent = 0x348;      // CharacterMovementComponent* CharacterMovement;//[Offset: 0x324 , Size: 4]
    uint32_t GravityScale = 0x2f0;                    // float GravityScale;//[Offset: 0x140 , Size: 4]
    uint32_t GameID = 0xb28;                          // uint64 GameID;//[Offset: 0xab8 , Size: 8]
    uint32_t PickUpDataList = 0x690;                  // PickUpItemData[] PickUpDataList;//[Offset: 0x664, Size: 12]
    uint32_t Count = 0x400;                           // int Count;//[Offset: 0x3e0, Size: 4]
    uint32_t WorldSettings = 0x2b8;                   // WorldSettings* WorldSettings;//[Offset: 0x1c4, Size: 4]
    uint32_t MinUndilatedFrameTime = 0x4c0;           // float MinUndilatedFrameTime;//[Offset: 0x488, Size: 4]
    uint32_t CamView = 0x3C24C14;
    uint32_t FastRun = 0x2220;
    uint32_t GameSpeed = 0x42B0644;
    uint32_t CurAnimParamList = 0x1894; // AnimParamList CurAnimParamList;//[Offset: 0x1854, Size: 308]
    uint32_t ComponentSpaceTransformsArray = 0x5E8;
    uint32_t ThirdPersonCameraComponent = 0x1328;
    uint32_t FieldOfView = 0x260;
    uint32_t CurrentVehicle = 0xa00; // STExtraVehicleBase *CurrentVehicle; //[Offset: 0xa00, Size: 4];
    unsigned char magicbyte0 = 0x78;
    unsigned char magiccompare = 0x47;
    unsigned char magicbyte1 = 0xE8;
    unsigned char magicbyte2 = 0x73;
} // namespace offsets
Modules gamemodules;
namespace settings
{
    static bool showMenu = true;
    static bool showMenu1 = true;
    static bool ipadview = false;
    static float ipadviewslider = 90;

    static bool combatmode = false;

    // esp
    static bool playerbox = true;
    static float boxcolor[] = {1.00f, 1.00f, 1.00f};
    static bool playerhealth = true;
    static bool playerline = true;
    static float linecolor[] = {1.00f, 1.00f, 1.00f};
    static bool playerbone = true;
    static float bonecolor[] = {1.00f, 1.00f, 1.00f};
    static bool botesp = true;
    static float botcolor[] = {0.00f, 1.00f, 1.00f};
    static bool itemesp = true;
    static float maxdistance = 500;
    static bool playername = true;
    static bool playerteam = true;
    static float playerteamcolor[] = {1.00f, 0.00f, 0.00f};
    static bool networkreset = false;

    static bool aimline = true;
    static float aimlinecolor[] = {1.00f, 0.00f, 0.00f};

    static float namecolor[] = {1.00f, 1.00f, 1.00f};
    static bool playerweapon = true;
    static float weaponcolor[] = {1.00f, 1.00f, 1.00f};
    static bool caresp = true;
    static float carcolor[] = {0.00f, 1.00f, 0.00f};
    static float vhealthcolor[] = {1, 1, 1};
    static float phealthcolor[] = {1, 1, 1};

    static float boxthickness = 1.0;
    static float bonethickness = 1.0;
    static float linethickness = 1.0;

    static float itemfontsize = 10.0;
    static float vehiclefontsize = 10.0;
    static float playercountsize = 14.5;

    // aimbot
    static bool enableaimbot = false;
    static bool fovcircle;
    static float circlecolor[] = {0.90f, 0.90f, 0.90f};
    static int aimbotkey = VK_LBUTTON;
    static int whitelistkey = VK_LBUTTON;

    static int aimtype = 0;
    static int aimbotfov = 360;
    static float aimbotsmooth = 4;
    static int aimbotselection = 0;
    static int aimbotbone = 0;
    static bool aimKnocked;
    static bool aimprediction = true;
    static bool autorefresh = true;

    // misc

    static bool instanthit = false;
    static bool instanthit2 = false;
    static bool norecoil = false;

    static bool fastcar;
    static bool speedrun;
    static bool cartrack = false;
    static bool forcethirdperson;
    static bool flycar;
    static float flycarspeed = 150;

    static bool luffyhand = false;
    static bool fastland;
    static bool antiheadshot = false;

    static bool antiscreenshot = true;

    static int flycarkey = VK_SPACE;
    static int fastcarkey = VK_SHIFT;
    static int speedrunkey = VK_MBUTTON;
    static int fastlandkey = VK_SHIFT;
    static int luffykey = 0;
    static int vehiclehealthgastype = 0;
    static int playerhealthtype = 0;

    static int androidver = 4;
} // namespace settings
int discordstatus = 0;
D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
    D3DMATRIX pOut;
    pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 +
               pM1._14 * pM2._41;
    pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 +
               pM1._14 * pM2._42;
    pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 +
               pM1._14 * pM2._43;
    pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 +
               pM1._14 * pM2._44;
    pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 +
               pM1._24 * pM2._41;
    pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 +
               pM1._24 * pM2._42;
    pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 +
               pM1._24 * pM2._43;
    pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 +
               pM1._24 * pM2._44;
    pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 +
               pM1._34 * pM2._41;
    pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 +
               pM1._34 * pM2._42;
    pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 +
               pM1._34 * pM2._43;
    pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 +
               pM1._34 * pM2._44;
    pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 +
               pM1._44 * pM2._41;
    pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 +
               pM1._44 * pM2._42;
    pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 +
               pM1._44 * pM2._43;
    pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 +
               pM1._44 * pM2._44;

    return pOut;
}

FTransform GetBoneIndex(uint32_t dwbonemesh, int indexnum)
{
    uint32_t boneAddv = read<uint32_t>(dwbonemesh + offsets::ComponentSpaceTransformsArray) + 0x30;
    return read<FTransform>(boneAddv + (indexnum * 0x30));
}

Vector3 GetBoneWorldPosition(uint32_t player, int index)
{
    uint32_t SkeletalMeshComponent = read<uint32_t>(player + offsets::Mesh);
    FTransform bone = GetBoneIndex(SkeletalMeshComponent, index);
    FTransform ComponentToWorld = read<FTransform>(SkeletalMeshComponent + 0x140);
    D3DMATRIX Matrix = MatrixMultiplication(bone.ToMatrixWithScale(),
                                            ComponentToWorld.ToMatrixWithScale());
    return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

char drawBuff[64];
uint32_t uMyself;
uint32_t gNames;

uint32_t uWorlds;
uint32_t uLevel;
uint32_t gameInstance;
uint32_t playerController;
uint32_t uMyObject;
uint32_t ViewMatrixBase;
uint32_t playerCarry;
uint32_t entityEntry;
uint32_t entityCount;
uint32_t uCamera;
uint32_t LocalPlayerAddr;

auto FLOAT2RGBA(float Col[]) -> RGBA
{
    return {(Col[0] * 255), (Col[1] * 255), (Col[2] * 255), 255};
}

auto CreateRenderTarget() -> VOID
{
    ID3D11Texture2D *pBackBuffer = nullptr;
    ms_vSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    ms_vD3dDevice->CreateRenderTargetView(pBackBuffer, NULL,
                                          &ms_vRenderTargetView);
    pBackBuffer->Release();
}

auto CleanupRenderTarget() -> VOID
{
    if (!ms_vRenderTargetView)
        return;
    ms_vRenderTargetView->Release();
    ms_vRenderTargetView = NULL;
}

auto CleanupDeviceD3D() -> VOID
{
    CleanupRenderTarget();
    if (ms_vSwapChain)
    {
        ms_vSwapChain->Release();
        ms_vSwapChain = NULL;
    }
    if (ms_vDeviceContext)
    {
        ms_vDeviceContext->Release();
        ms_vDeviceContext = NULL;
    }
    if (ms_vD3dDevice)
    {
        ms_vD3dDevice->Release();
        ms_vD3dDevice = NULL;
    }
}

bool areEqual(BYTE arr1[], BYTE arr2[])
{
    for (int i = 0; i < 7; i++)
        if (arr1[i] != arr2[i])
            return false;
    return true;
}

bool threadcreated = false;
std::vector<uint32_t> MagicBulletList;
unsigned char ShellCode[60]{
    0xC7, 0x45, 0x78, 0x00, 0x00, 0x00, 0x00,                   // mov[ebp + 78],00000000
    0x83, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x01,                   // cmp dword ptr[00000000],01
    0x74, 0x0E,                                                 // je 0000001E
    0x60,                                                       // pushad
    0x8B, 0x5D, 0x50,                                           // mov ebx,[ebp + 00]
    0x8B, 0x53, 0x18,                                           // mov edx,[ebx + 18]
    0x8A, 0x02,                                                 // mov al,[edx]
    0x7C, 0xE2,                                                 // 27                                                     // cmp al,30    // Need update
    0x74, 0x02,                                                 // je 0000001F
    0x61,                                                       // popad
    0xC3,                                                       // ret
    0x8A, 0x42, 0x01,                                           // mov al,[edx + 01]
    0x7C, 0x89,                                                 // 36                                                      // cmp al,C2    // Need update
    0x75, 0x14,                                                 // jne 0000003A
    0xC7, 0x83, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov[ebx + 00000230],00000000
    0xC7, 0x83, 0x24, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov[ebx + 00000234],00000000
    0xEB, 0xE1};
#pragma warning(push)
#pragma warning(disable : 4100)
void GetMagic()
{
    // 66 0F 6E ? A8 00 00 00 66 0F 7E ? 28 02 00 00 C7 45 68 38
    // 66 0F 7E ?? F8 01 00 00 C7 45 ?8 38
    // 66 0F 7E A3 28 02 00 00 C7 45 68 B0 37 0B 2A E9 3B 0B 8B FF

    uint32_t scanBetween[2] = {0x17000000, 0x20000000};
    ShellCode[26] = offsets::magicbyte1;
    ShellCode[35] = offsets::magicbyte2;
    if (settings::androidver == 7)
    {
        ShellCode[19] = 0x50;
        scanBetween[0] = 0x0E000000;
        scanBetween[1] = 0x10000000;
    }
    auto start = std::chrono::steady_clock::now();

    auto tmpMagic = FindPatternEx(scanBetween[0], scanBetween[1], {6, 0x0F, 0x7E, '?', 0x28, 0x02, 0x00, 0x00, 0xC7, 0x45, '?', 0xD8, offsets::magicbyte0, offsets::magiccompare});
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    // std::cout << std::chrono::duration <double, milli>(diff).count() << " ms"
    // << endl;

    if (tmpMagic.size() != NULL)
    {
        MagicBulletList.clear();
        for (auto addv : tmpMagic)
        {
            if (read<BYTE>(addv + 11) == offsets::magicbyte0)
            {
                MagicBulletList.push_back(addv);
            }
        }
    }
}
uint32_t MagicBulletHook;
void HookCall(uint32_t Address, uint32_t HookAdress)
{
    unsigned char Shell[7] = {0xE8, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90};
    *(uint32_t *)(Shell + 1) = (uint32_t)HookAdress - Address - 5;
    Controller->WriteProcessMemory(Address, (void *)Shell, sizeof(Shell));
}
BYTE realCode[7];
void initMagic()
{
    if (MagicBulletList.size() == NULL)
    {
        threadcreated = false;
        return;
    }
    uint32_t addv = MagicBulletList[0];
    if (!MagicBulletHook)
        MagicBulletHook = (uint32_t)VirtualAllocEx(m_Handle, 0, 500, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    // ShellCode[0x2F] = 0x14;
    // ShellCode[0x49] = 0xC0;
    *(uint32_t *)(ShellCode + 9) = (uint32_t)MagicBulletHook + 94;
    Controller->WriteProcessMemory(MagicBulletHook, (void *)ShellCode, sizeof(ShellCode));
    Controller->ReadProcessMemory(addv + 8, &realCode, 7);
    Controller->WriteProcessMemory(MagicBulletHook, &realCode, 7);
}
bool Hooked(uint32_t addr)
{
    BYTE tmp[7];
    Controller->ReadProcessMemory(addr + 8, &tmp, 7);
    if (tmp[0] == 0xE8 && tmp[5] == 0x90 && tmp[6] == 0x90)
        return true;
    return false;
}
void RestoreMagic()
{
    for (auto addv : MagicBulletList)
    {
        if (Hooked(addv))
        {
            Controller->WriteProcessMemory(addv + 8, (void *)&realCode, 7);
        }
    }
}

bool magiciniting = false;

bool HookMagic(Frotator rotator)
{
    if (MagicBulletList.size() != NULL)
    {
        for (auto addv : MagicBulletList)
        {
            if (read<BYTE>(addv + 11) != offsets::magicbyte0 && !Hooked(addv))
                return false;
            BYTE tmp[7];
            Controller->ReadProcessMemory(addv + 8, &tmp, 7);
            if (areEqual(tmp, realCode))
            {
                HookCall(addv + 8, MagicBulletHook);
            }
        }
        write<bool>(MagicBulletHook + 13, true);
        write<float>(MagicBulletHook + 44, rotator.Pitch);
        write<float>(MagicBulletHook + 54, rotator.Yaw);
        return true;
    }
    return false;
}

void MagicThread()
{
    magiciniting = true;
    RestoreMagic();
    GetMagic();
    initMagic();
    magiciniting = false;
}
#pragma warning(push)
#pragma warning(disable : 4100)

bool IsPlayer(string str)
{
    if (str.find(xorstr_("FakePlayer_AIPawn")) != string::npos)
        return true;
    if (str.find(xorstr_("BP_PlayerCharacter")) != string::npos)
        return true;
    if (str.find(xorstr_("BP_PlayerPawn")) != string::npos)
        return true;
    if (str.find(xorstr_("PlayerPawn_Infec_")) != string::npos)
        return true;
    if (str.find(xorstr_("AlPawn")) != string::npos)
        return true;
    if (str.find(xorstr_("BP_AIPawn_C")) != string::npos)
        return true;
    if (str.find(xorstr_("PlanX_PlayerCharacter_AI_C")) != string::npos)
        return true;
    if (str.find(xorstr_("BP_PlayerCharacter_PlanX_C")) != string::npos)
        return true;
    return false;
}
bool isWeapon(string name)
{
    if (name.find(xorstr_("BP_")) != string::npos)
    {
        if (name.find(xorstr_("_C")) != string::npos)
        {
            if (name.find(xorstr_("Grenade")) == string::npos)
                return true;
        }
    }
    return false;
}
std::string getEntityClassName(uint32_t gNames, uint32_t id)
{
    char Name[128];
    memset(Name, 0, 128);
    uint32_t gname = gNames;
    if (id > 0 && id < 2000000)
    {
        uint32_t page = id / 16384;
        uint32_t index = id % 16384;
        uint32_t secPartAddv = read<uint32_t>(gname + page * 4);
        if (secPartAddv > 0)
        {
            uint32_t nameAddv = read<uint32_t>(secPartAddv + index * 4);
            if (nameAddv > 0)
            {
                Controller->ReadProcessMemory(nameAddv + 8, &Name, 32);
            }
        }
    }
    return Name;
}
#include <codecvt>
#include <locale>
static std::string utf8_encode(const std::wstring &wstr)
{
    if (wstr.empty())
        return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(),
                                          NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0],
                        size_needed, NULL, NULL);

    return strTo;
}
std::string getPlayerName(uint32_t entityAddv)
{
    wchar_t szChar[64] = {'\0'};
    Controller->ReadProcessMemory(read<uint32_t>(entityAddv + offsets::PlayerName),
                                  &szChar, sizeof(szChar));
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(szChar);
}
std::string getPlayerId(uint32_t entityAddv)
{
    wchar_t szChar[64] = {'\0'};
    Controller->ReadProcessMemory(read<uint32_t>(entityAddv + offsets::PlayerUID), &szChar,
                                  sizeof(szChar));
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(szChar);
}
string GetVehicleType(std::string str)
{
    //
    //
    if (str.find(xorstr_("BRDM")) != std::string::npos)
    {
        return xorstr_("Airdrop Car");
    }
    if (str.find(xorstr_("Scooter")) != std::string::npos)
    {
        return xorstr_("Scooter");
    }
    if (str.find(xorstr_("VH_Motorcycle_C")) != std::string::npos)
    {
        return xorstr_("Motorcycle");
    }
    if (str.find(xorstr_("VH_Motorcycle_1_C")) != std::string::npos)
    {
        return xorstr_("Motorcycle");
    }
    if (str.find(xorstr_("VH_CoupeRB_1_C")) != std::string::npos)
    {
        return xorstr_("Coupe RB");
    }
    if (str.find(xorstr_("MotorcycleCart")) != std::string::npos)
    {
        return xorstr_("MotorcycleCart");
    }
    if (str.find(xorstr_("Snowmobile")) != std::string::npos)
    {
        return xorstr_("Snowmobile");
    }
    if (str.find(xorstr_("Tuk")) != std::string::npos)
    {
        return xorstr_("Tuk");
    }
    if (str.find(xorstr_("Buggy")) != std::string::npos)
    {
        return xorstr_("Buggy");
    }
    if (str.find(xorstr_("open")) != std::string::npos)
    {
        return xorstr_("Sport Car");
    }
    if (str.find(xorstr_("close")) != std::string::npos)
    {
        return xorstr_("Sport Car");
    }
    if (str.find(xorstr_("Dacia")) != std::string::npos)
    {
        return xorstr_("Dacia");
    }
    if (str.find(xorstr_("Rony")) != std::string::npos)
    {
        return xorstr_("Rony");
    }
    if (str.find(xorstr_("UAZ")) != std::string::npos)
    {
        return xorstr_("UAZ");
    }
    if (str.find(xorstr_("MiniBus")) != std::string::npos)
    {
        return xorstr_("MiniBus");
    }
    if (str.find(xorstr_("PG117")) != std::string::npos)
    {
        return xorstr_("PG117");
    }
    if (str.find(xorstr_("AquaRai")) != std::string::npos)
    {
        return xorstr_("AquaRai");
    }
    if (str.find(xorstr_("BusVehicle_C")) != std::string::npos)
    {
        return xorstr_("Bus");
    }
    if (str.find(xorstr_("VH_ModelY_C")) != std::string::npos)
    {
        return xorstr_("Tesla Car");
    }
    if (str.find(xorstr_("VH_ATGMotorCycle1_C")) != std::string::npos)
    {
        return xorstr_("Flying Jet Bike");
    }
    if (str.find(xorstr_("VH_Motorglider_C")) != std::string::npos)
    {
        return xorstr_("Motor Glider");
    }
    // if (str.find(xorstr_("BP_AirDropPlane_C")) != std::string::npos)
    //{
    //	return xorstr_("Plane");
    //}
    return xorstr_("Unknown");
    //
    //
}
string GetGrenadeType(std::string str)
{
    if (str.find(xorstr_("BP_Grenade_Smoke_C")) != std::string::npos)
    {
        return xorstr_("Smoke Grenade");
    }
    if (str.find(xorstr_("BP_Grenade_Burn_C")) != std::string::npos)
    {
        return xorstr_("Molotov Grenade");
    }
    if (str.find(xorstr_("BP_Grenade_tun_C")) != std::string::npos)
    {
        return xorstr_("Flash Grenade");
    }
    if (str.find(xorstr_("BP_Grenade_Shoulei_C")) != std::string::npos)
    {
        return xorstr_("Frag Grenade");
    }
    return xorstr_("Unknown");
    //
    //
}
string GetItemType(std::string str)
{
    // VM_TIGER_WHITE_START
    // STR_ENCRYPT_START
    // level 1

    if (str.find(xorstr_("BP_Helmet_Lv1")) != std::string::npos)
    {
        return xorstr_("[level1]Helmet lvl1");
    }
    if (str.find(xorstr_("BP_MZJ_QX_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Holo");
    }

    if (str.find(xorstr_("GasCan_Destructible_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Fuel Battery");
    }
    if (str.find(xorstr_("BP_Grenade_Shoulei_Weapon_Wrapper_C")) !=
        std::string::npos)
    {
        return xorstr_("[level1]Grenade");
    }

    if (str.find(xorstr_("BP_WB_ThumbGrip_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Thumb Grip");
    }
    if (str.find(xorstr_("BP_WB_LightGrip_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Light Grip");
    }
    if (str.find(xorstr_("BP_WB_Vertical_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Vertical Grip");
    }
    if (str.find(xorstr_("BP_WB_ThumbGrip_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Thumb Grip");
    }
    if (str.find(xorstr_("BP_MZJ_2X_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]2X Scope");
    }
    if (str.find(xorstr_("BP_MZJ_3X_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]3X Scope");
    }
    if (str.find(xorstr_("BP_QK_Large_Suppressor_Pickup_C")) !=
        std::string::npos)
    {
        return xorstr_("[level1]Suppressor");
    }
    if (str.find(xorstr_("BP_QK_Sniper_Suppressor_Pickup_C")) !=
        std::string::npos)
    {
        return xorstr_("[level1]Sniper Suppressor");
    }
    if (str.find(xorstr_("BP_QT_A_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Tactical Stock");
    }
    if (str.find(xorstr_("BP_QK_Large_Compensator_Pickup_C")) !=
        std::string::npos)
    {
        return xorstr_("[level1]AR Compensator");
    }
    if (str.find(xorstr_("BP_QK_Sniper_Compensator_Pickup_C")) !=
        std::string::npos)
    {
        return xorstr_("[level1]Sniper Compensator");
    }
    if (str.find(xorstr_("BP_QK_Mid_Compensator_Pickup_C")) !=
        std::string::npos)
    {
        return xorstr_("[level1]SMG Compensator");
    }
    if (str.find(xorstr_("BP_ZDD_Sniper_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Bullet loop");
    }
    if (str.find(xorstr_("BP_DJ_Sniper_EQ_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Extended Sniper Mag");
    }
    if (str.find(xorstr_("BP_DJ_Sniper_E_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Extended Sniper Mag");
    }
    if (str.find(xorstr_("BP_DJ_Large_EQ_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Extended Rifle Mag");
    }
    if (str.find(xorstr_("BP_DJ_Large_E_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Extended Rifle Mag");
    }
    if (str.find(xorstr_("BP_Dj_Mid_WQ_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level1]Extended SMG Meg");
    }
    if (str.find(xorstr_("BP_Bag_Lv1")) != std::string::npos)
    {
        return xorstr_("[level1]BackPack lvl1");
    }
    if (str.find(xorstr_("BP_Armor_Lv1")) != std::string::npos)
    {
        return xorstr_("[level1]Armor lvl1");
    }

    if (str.find(xorstr_("BP_MachineGun_UMP9_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level1]UMP9");
    }
    if (str.find(xorstr_("BP_MachineGun_PP19_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level1]BIZON");
    }
    if (str.find(xorstr_("BP_MachineGun_Vector_Wrapper_C")) !=
        std::string::npos)
    {
        return xorstr_("[level1]VECTOR");
    }
    if (str.find(xorstr_("BP_MachineGun_Uzi_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level1]UZI");
    }
    if (str.find(xorstr_("BP_MachineGun_MP5K_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level1]MP5K");
    }
    if (str.find(xorstr_("BP_Pistol_P18C_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level1]P18C");
    }
    if (str.find(xorstr_("BP_Pistol_Vz61_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level1]Scorpion");
    }
    if (str.find(xorstr_("BP_MachineGun_TommyGun_Wrapper_C")) !=
        std::string::npos)
    {
        return xorstr_("[level1]THOMPSON");
    }

    // Level 2
    if (str.find(xorstr_("BP_Sniper_SLR_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level2]SLR");
    }
    if (str.find(xorstr_("BP_Sniper_SKS_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level2]SKS");
    }
    if (str.find(xorstr_("BP_Rifle_Mk47_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level2]MK47");
    }
    if (str.find(xorstr_("BP_Rifle_M16A4_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level2]M16A-4");
    }
    if (str.find(xorstr_("BP_Sniper_VSS_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level2]VSS");
    }

    // Level 3
    if (str.find(xorstr_("BP_ShotGun_S12K_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level3]S12K");
    }
    if (str.find(xorstr_("BP_ShotGun_DP12_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level3]DBS");
    }
    if (str.find(xorstr_("BP_ShotGun_S686_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level3]S686");
    }
    if (str.find(xorstr_("BP_ShotGun_S1897_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level3]S1897");
    }
    if (str.find(xorstr_("BP_ShotGun_SawedOff_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level3]SawedOff");
    }
    if (str.find(xorstr_("BP_Other_CrossBow_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level3]CROSSBOW");
    }

    // Level 4
    if (str.find(xorstr_("BP_MZJ_8X_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level4]8X Scope");
    }
    if (str.find(xorstr_("BP_MZJ_6X_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level4]6X Scope");
    }
    if (str.find(xorstr_("BP_MZJ_4X_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level4]4X Scope");
    }
    if (str.find(xorstr_("BP_Ammo_300Magnum_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level4]AWM Ammo");
    }
    if (str.find(xorstr_("BP_Other_M249_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]M249");
    }
    if (str.find(xorstr_("BP_Rifle_AKM_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]AKM");
    }
    if (str.find(xorstr_("BP_Rifle_M416_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]M416");
    }
    if (str.find(xorstr_("BP_Rifle_Groza_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]GROZA");
    }
    if (str.find(xorstr_("BP_WEP_Mk14_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level4]Mk14");
    }
    if (str.find(xorstr_("BP_Other_DP28_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]DP28");
    }
    if (str.find(xorstr_("BP_Rifle_M762_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]M762");
    }
    if (str.find(xorstr_("BP_Sniper_AWM_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]AWM");
    }
    if (str.find(xorstr_("BP_Rifle_AN94_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]ASM Abakan AR");
    }
    if (str.find(xorstr_("BP_Rifle_AUG_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]AUG-A3");
    }
    if (str.find(xorstr_("BP_Rifle_G36_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]G36C");
    }
    if (str.find(xorstr_("BP_MachineGun_PP19_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]Bizon");
    }
    if (str.find(xorstr_("BP_Rifle_QBZ_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]QBZ");
    }
    if (str.find(xorstr_("BP_Rifle_FAMAS_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]FAMAS");
    }
    if (str.find(xorstr_("BP_Rifle_SCAR_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]SCAR-L");
    }
    if (str.find(xorstr_("BP_Pistol_Flaregun_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level4]Flare Gun");
    }
    if (str.find(xorstr_("FirstAidbox_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level4]Med Kit");
    }
    // if (str.find(xorstr_("BP_WEP_Pan_C")) != std::string::npos)
    //{
    //	return xorstr_("[level4]Pan");
    //}
    if (str.find(xorstr_("BP_WEP_Pan_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level4]Pan");
    }
    if (str.find(xorstr_("Injection_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level4]Adrenaline Syringe");
    }
    if (str.find(xorstr_("BP_Grenade_Shoulei_Weapon_Wrapper_C")) !=
        std::string::npos)
    {
        return xorstr_("[level4]Grenade");
    }
    if (str.find(xorstr_("BP_Helmet_Lv3")) != std::string::npos)
    {
        return xorstr_("[level4]Helmet lvl3");
    }
    if (str.find(xorstr_("BP_Bag_Lv3")) != std::string::npos)
    {
        return xorstr_("[level4]BackPack lvl3");
    }
    if (str.find(xorstr_("BP_Armor_Lv3")) != std::string::npos)
    {
        return xorstr_("[level4]Armor lvl3");
    }

    // Level 5
    if (str.find(xorstr_("BP_Sniper_Kar98k_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level5]Kar98-K");
    }
    if (str.find(xorstr_("BP_Sniper_M24_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level5]M24");
    }
    if (str.find(xorstr_("BP_Sniper_Mini14_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level5]Mini-14");
    }
    if (str.find(xorstr_("BP_Sniper_QBU_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level5]QBU");
    }
    if (str.find(xorstr_("BP_Sniper_MK12_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level5]MK12");
    }
    if (str.find(xorstr_("BP_Sniper_Mosin_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level5]Mosin Nagant SR");
    }
    if (str.find(xorstr_("BP_Sniper_Win94_Wrapper_C")) != std::string::npos)
    {
        return xorstr_("[level5]Win94");
    }
    // Level 6
    if (str.find(xorstr_("Drink_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level6]Enery Drink");
    }

    // Level 7
    if (str.find(xorstr_("Pills_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level7]PainKiller");
    }

    // Level 8
    if (str.find(xorstr_("BP_Helmet_Lv2")) != std::string::npos)
    {
        return xorstr_("[level8]Helmet lvl2");
    }
    if (str.find(xorstr_("BP_Bag_Lv2")) != std::string::npos)
    {
        return xorstr_("[level8]BackPack lvl2");
    }
    if (str.find(xorstr_("BP_Armor_Lv2")) != std::string::npos)
    {
        return xorstr_("[level8]Armor lvl2");
    }
    if (str.find(xorstr_("Firstaid_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level8]First Aid Kit");
    }

    // level 9
    if (str.find(xorstr_("BP_Ammo_556mm_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level9]5.56mm");
    }
    // level 10
    if (str.find(xorstr_("BP_Ammo_762mm_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level10]7.62mm");
    }
    // level 11
    if (str.find(xorstr_("BP_Ammo_9mm_Pickup_C")) != std::string::npos)
    {
        return xorstr_("[level11]9mm");
    }

    // level 12
    if (str.find(xorstr_("PickUpListWrapperActor")) != std::string::npos)
    {
        return xorstr_("[level12]Dead box");
    }

    // level 13
    if (str.find(xorstr_("AirDrop")) != std::string::npos)
    {
        return xorstr_("[level13]Airdrop");
    }
    return xorstr_("Unknown");
}
std::vector<PlayerData> playerList2;
std::vector<PlayerData> botlist;

std::vector<VehicleData> vehicleList;
std::vector<ItemData> itemList;
std::vector<GrenadeData> grenadeList;
PlayerData LocalPlayer;
uint32_t viewWorld;
uint32_t GameViewport;

Vector3 GetPlayerPos(uint32_t obj)
{
    return read<Vector3>(obj + offsets::uObjectPosition);
}
Vector3 GetObjectPos(uint32_t obj)
{
    Vector3 Pos;
    Pos.x = read<float>(obj);
    Pos.y = read<float>(obj + 4);
    Pos.z = read<float>(obj + 8);
    return Pos;
}
// 0x1F44D3F8
bool IsInGame()
{
    if (gameInstance)
        return true;
    return false;
}
FMinimalViewInfo CameraCacheEntry = FMinimalViewInfo();
std::vector<std::string> friends;

std::string GetBoxItems(int code)
{
    if (code == 101008)
        return xorstr_("M762");
    else if (code == 101003)
        return xorstr_("SCAR-L");
    else if (code == 101004)
        return xorstr_("M416");
    else if (code == 101002)
        return xorstr_("M16A-4");
    else if (code == 103003)
        return xorstr_("AWM");
    else if (code == 103010)
        return xorstr_("QBU");
    else if (code == 103009)
        return xorstr_("SLR");
    else if (code == 103004)
        return xorstr_("SKS");
    else if (code == 103006)
        return xorstr_("Mini14");
    else if (code == 103002)
        return xorstr_("M24");
    else if (code == 103001)
        return xorstr_("Kar98k");
    else if (code == 103005)
        return xorstr_("VSS");
    else if (code == 103008)
        return xorstr_("Win94");
    else if (code == 101009)
        return xorstr_("Mk47 Mutant");
    else if (code == 101010)
        return xorstr_("G36C");
    else if (code == 101007)
        return xorstr_("QBZ");
    else if (code == 101001)
        return xorstr_("AKM");
    else if (code == 101005)
        return xorstr_("Groza");
    else if (code == 101006)
        return xorstr_("AUG_A3");
    else if (code == 104003)
        return xorstr_("S12K");
    else if (code == 104004)
        return xorstr_("DBS");
    else if (code == 104001)
        return xorstr_("S686");
    else if (code == 104002)
        return xorstr_("S1897");
    else if (code == 106006)
        return xorstr_("SawedOff");
    else if (code == 102005)
        return xorstr_("PP19 Bizon");
    else if (code == 102004)
        return xorstr_("TommyGun");
    else if (code == 102007)
        return xorstr_("MP5K");
    else if (code == 102002)
        return xorstr_("UMP9");
    else if (code == 102003)
        return xorstr_("Vector");
    else if (code == 102001)
        return xorstr_("Uzi");
    else if (code == 106003)
        return xorstr_("R1895");
    else if (code == 106008)
        return xorstr_("Vz61");
    else if (code == 106001)
        return xorstr_("P92");
    else if (code == 106004)
        return xorstr_("P18C");
    else if (code == 106005)
        return xorstr_("R45");
    else if (code == 106002)
        return xorstr_("P1911");
    else if (code == 106010)
        return xorstr_("DesertEagle");
    else if (code == 108003)
        return xorstr_("Sickle");
    else if (code == 108001)
        return xorstr_("Machete");
    else if (code == 107001)
        return xorstr_("Cross Bow");
    else if (code == 108004)
        return xorstr_("Pan");
    else if (code == 103007)
        return xorstr_("Mk14");
    else if (code == 302001)
        return xorstr_("7.62");
    else if (code == 305001)
        return xorstr_("45ACP");
    else if (code == 303001)
        return xorstr_("5.56");
    else if (code == 301001)
        return xorstr_("9mm");
    else if (code == 306001)
        return xorstr_("300Magnum");
    else if (code == 304001)
        return xorstr_("12Guage");
    else if (code == 307001)
        return xorstr_("Arrows");
    else if (code == 201010)
        return xorstr_("Rifle-FlashHider");
    else if (code == 201009)
        return xorstr_("Rifle-Compensator");
    else if (code == 201004)
        return xorstr_("Mid Compensator");
    else if (code == 205002)
        return xorstr_("Tactical_Rifle");
    else if (code == 201012)
        return xorstr_("Duckbill");
    else if (code == 201005)
        return xorstr_("Sniper_FlashHider");
    else if (code == 201006)
        return xorstr_("Mid_Suppressor");
    else if (code == 205003)
        return xorstr_("Chekpad Sniper");
    else if (code == 201001)
        return xorstr_("Choke");
    else if (code == 205001)
        return xorstr_("UZI_Pickup");
    else if (code == 201003)
        return xorstr_("Sniper-Compensator");
    else if (code == 201007)
        return xorstr_("Sniper-Suppressor");
    else if (code == 201011)
        return xorstr_("Suppressor-AR");
    else if (code == 204009)
        return xorstr_("Sniper-Extended");
    else if (code == 204004)
        return xorstr_("Mid_E");
    else if (code == 204005)
        return xorstr_("Mid_Q");
    else if (code == 204007)
        return xorstr_("Sniper_Mag");
    else if (code == 204008)
        return xorstr_("Sniper_QuickMag");
    else if (code == 204012)
        return xorstr_("Rifle_Mag");
    else if (code == 204013)
        return xorstr_("Exteded AR");
    else if (code == 204011)
        return xorstr_("Mag-AR");
    else if (code == 204006)
        return xorstr_("Mid_EQ");
    else if (code == 205004)
        return xorstr_("Crossbow");
    else if (code == 204014)
        return xorstr_("ZDD_Sniper");
    else if (code == 203005)
        return xorstr_("8x");
    else if (code == 203003)
        return xorstr_("2x");
    else if (code == 203001)
        return xorstr_("Lazer");
    else if (code == 203014)
        return xorstr_("3X");
    else if (code == 203002)
        return xorstr_("Holo");
    else if (code == 203015)
        return xorstr_("6x");
    else if (code == 203004)
        return xorstr_("4x");
    else if (code == 105002)
        return xorstr_("DP28");
    else if (code == 107001)
        return xorstr_("CrossBow");
    else if (code == 105001)
        return xorstr_("M249");
    else if (code == 501006)
        return xorstr_("Bag Lv.3");
    else if (code == 501004)
        return xorstr_("Bag Lv.1");
    else if (code == 501005)
        return xorstr_("Bag Lv.2");
    else if (code == 503002)
        return xorstr_("Armour Lv.2");
    else if (code == 503001)
        return xorstr_("Armour Lv.1");
    else if (code == 503003)
        return xorstr_("Armour Lv.3");
    else if (code == 502002)
        return xorstr_("Helmet Lv.2");
    else if (code == 502001)
        return xorstr_("Helmet Lv.1");
    else if (code == 502003)
        return xorstr_("Helmet Lv.3");
    else if (code == 602004)
        return xorstr_("Grenade");
    else if (code == 602002)
        return xorstr_("Smoke");
    else if (code == 602003)
        return xorstr_("Molotof");
    else if (code == 602005)
        return xorstr_("Apple");
    else if (code == 601003)
        return xorstr_("Painkiller");
    else if (code == 601002)
        return xorstr_("Adrenaline");
    else if (code == 601001)
        return xorstr_("Energy Drink");
    else if (code == 601005)
        return xorstr_("FirstaidKit");
    else if (code == 601004)
        return xorstr_("Bandage");
    else if (code == 202006)
        return xorstr_("ThumbGrip");
    else if (code == 202007)
        return xorstr_("Lasersight");
    else if (code == 202001)
        return xorstr_("Angled");
    else if (code == 202004)
        return xorstr_("LightGrip");
    else if (code == 106007)
        return xorstr_("Flaregun");
    else if (code == 3000312)
        return xorstr_("GameCoin");
    else if (code == 601006)
        return xorstr_("Medkit");
    else if (code == 308001)
        return xorstr_("Flaregun");
    else if (code == 501003)
        return xorstr_("Bag Lv.3");
    else if (code == 501002)
        return xorstr_("Bag Lv.2");
    else if (code == 501001)
        return xorstr_("Bag Lv.1");
    else if (code == 201002)
        return xorstr_("Mid_Compensator");
    else if (code == 502005)
        return xorstr_("Helmet Lv.2");
    else if (code == 403989)
        return xorstr_("Arctic-Suit");
    else if (code == 403045)
        return xorstr_("Woodland-Suit");
    else if (code == 403187)
        return xorstr_("Desert-Suit");
    else if (code == 403188)
        return xorstr_("Desert-Suit");

    return xorstr_("");
}
int system_no_output(std::string command);
DWORD lasttick;
#include <future>
void cache()
{
    if (!gamemodules.libue4)
    {
        MessageBoxA(GameWnd, xorstr_("Failed: Cant find game adress!"),
                    xorstr_("Error"), MB_ICONERROR);
        ExitProcess(1);
    }
    if (gamemodules.libue4 > 0x30000000)
        settings::androidver = 7;
    VM_START

    if (read<uint32_t>(gamemodules.libue4 + 0x20) == 126494244)
    {
        offsets::viewWorld = 0x7B0D0E0;
        offsets::GEngine = 0x7643938;
        offsets::gNames = 0x78F2E04;
    }

    while (!GameViewport)
    {
        GameViewport = read<uint32_t>(read<uint32_t>(gamemodules.libue4 + offsets::GEngine) + offsets::GameViewport);
    }

    gNames = read<uint32_t>(read<uint32_t>(gamemodules.libue4 + offsets::gNames) + 0x14) + 0x900;
    viewWorld = gamemodules.libue4 + offsets::viewWorld;

    VM_END
    // if (read<uint32_t>(gamemodules.libue4 + 0x20) == 123954724)
    //{
    //	uWorld = gamemodules.libue4 + 0x70AB2BC;
    //	gNames = gamemodules.libue4 + 0x7364D94;
    //	viewWorld = gamemodules.libue4 + 0x753B110;
    //	offsets::magicbyte1 = 0xD0;
    //	offsets::magicbyte2 = 0xC6;
    // }
    //  cout << "libUE4: 0x" << hex << libUE4 << endl;
    //  cout << "libUE4: 0x" << hex << m_pid << endl;

    ////cout << "uWorld: 0x" << hex << uWorld << endl;
    ////cout << "gNames: 0x" << gNames << endl;
    // cout << "viewWorld: 0x" << viewWorld << endl;
    while (read<uint32_t>(gamemodules.libue4 + 0x7780318) < 4702)
    {
    }
    lasttick = GetTickCount();

    while (true)
    {
       /* VM_START
        if (r_userdata::expiry < 1)
        {
            playerList2[999].Address = 0;
            exit(0);
        }
        VM_END*/

        std::vector<PlayerData> playerList;
        std::vector<PlayerData> botlistx;

        std::vector<VehicleData> vehicleListx;
        std::vector<ItemData> itemListx;
        std::vector<GrenadeData> grenadeListx;
        uWorlds = read<uint32_t>(GameViewport + offsets::World);
        uLevel = read<uint32_t>(uWorlds + offsets::uLevel);
        gameInstance = read<uint32_t>(uWorlds + offsets::gameInstance);
        playerController = read<uint32_t>(gameInstance + offsets::playerController);

        playerCarry = read<uint32_t>(playerController + offsets::playerCarry);
        uCamera = read<uint32_t>(playerCarry + offsets::PlayerCameraManager) + 0x370;
        CameraCacheEntry = read<FMinimalViewInfo>(uCamera);
        uMyObject = read<uint32_t>(playerCarry + offsets::uMyObject);
        LocalPlayerAddr = read<uint32_t>(playerCarry + 0x1b68);
        ViewMatrixBase = viewWorld;
        entityEntry = read<uint32_t>(uLevel + offsets::entityEntry);
        entityCount = read<uint32_t>(uLevel + offsets::entityCount);
        if (settings::networkreset)
        {
            static bool blockconnection = false;
            if (GetTickCount64() - lasttick > 210000)
            {
                lasttick = GetTickCount64();
                blockconnection = true;
                auto fut = std::async([] { return pVoice->Speak(xorstr_(L"Network reset starting"), 0, NULL); });
                static std::string blockstring = xorstr_("netsh advfirewall firewall add rule name=Block12345 dir=out action=block program=\"") + emulatorpath + "\"";
                system_no_output(blockstring);
            }
            if ((GetTickCount64() - lasttick > 5000) && blockconnection)
            {
                blockconnection = false;
                system_no_output(xorstr_("netsh advfirewall firewall delete rule name=Block12345"));
                auto fut = std::async([] { return pVoice->Speak(xorstr_(L"Network reset done"), 0, NULL); });
            }
        }

        // printf("gNames: %X\n", gNames);

        // printf("uWorld: %X\n", uWorlds);
        // printf("uLevel: %X\n", uLevel);
        // printf("gameInstance: %X\n", gameInstance);
        // printf("playerController: %X\n", playerController);
        // printf("playerCarry: %X\n", playerCarry);
        // printf("uMyObject: %X\n", uMyObject);

        if (entityCount > 2048)
        {
            entityCount = 2048;
        }
        if (IsInGame())
        {
            for (int i = 0; i < entityCount; i++)
            {
                uint32_t entityAddv = read<uint32_t>(entityEntry + i * 4);
                if (read<uint8_t>(entityAddv + 0x14d) != 0x16)
                    continue;
                uint32_t entityStruct = read<uint32_t>(entityAddv + 16);
                if (!entityAddv)
                    continue;
                if (!entityStruct)
                    continue;
                std::string entityClassName = getEntityClassName(gNames, entityStruct);
                if (settings::caresp)
                {

                    if (std::find_if(vehicleListx.cbegin(), vehicleListx.cend(), [entityAddv](const VehicleData &s) {
                            return s.Address == entityAddv;
                        }) != vehicleListx.cend())
                        continue;
                    std::string vehicle = GetVehicleType(entityClassName);
                    if (vehicle.find(xorstr_("Unknown")) == string::npos)
                    {
                        if (entityAddv == LocalPlayer.CurrentVehicle)
                            continue;
                        uint32_t vehicleWorld = read<uint32_t>(entityAddv + offsets::uObjectWorld);
                        uint32_t VehicleCommon = read<uint32_t>(entityAddv + offsets::VehicleCommon);
                        if (!VehicleCommon)
                            continue;
                        VehicleData vehicledata;
                        vehicledata.Position = read<Vector3>(vehicleWorld + 0xFC);
                        vehicledata.Name = vehicle;
                        vehicledata.bIsEngineStarted = read<bool>(entityAddv + offsets::IsEngineStarted);
                        vehicledata.Address = entityAddv;
                        vehicledata.HPMax = read<float>(VehicleCommon + offsets::VehicleHPMax);
                        vehicledata.HP = read<float>(VehicleCommon + offsets::VehicleHP);
                        vehicledata.FuelMax = read<float>(VehicleCommon + offsets::VehicleFuelMax);
                        vehicledata.Fuel = read<float>(VehicleCommon + offsets::VehicleFuel);
                        vehicleListx.push_back(vehicledata);
                        continue;
                    }
                }
                if (settings::itemesp)
                {
                    std::string item = GetItemType(entityClassName);
                    if (item.find(xorstr_("Unknown")) == string::npos)
                    {
                        uint32_t itemWorld = read<uint32_t>(entityAddv + offsets::uObjectWorld);
                        ItemData itemdata;
                        itemdata.Position = read<Vector3>(itemWorld + offsets::uObjectPosition);
                        itemdata.Name = item;
                        if (strcmp(item.c_str(), xorstr_("[level13]Airdrop")) == 0 ||
                            strcmp(item.c_str(), xorstr_("[level12]Dead box")) == 0)
                        {
                            itemdata.pickupbox = true;
                            auto boxitementry = read<uint32_t>(entityAddv + 0x684);
                            auto boxitemcount = read<uint32_t>(entityAddv + 0x684 + 0x4);
                            if (boxitementry)
                            {
                                if (boxitemcount >= 60)
                                    boxitemcount = 59;
                                for (int h = 0; h < boxitemcount; h++)
                                {
                                    uint32_t itemAddv = boxitementry + h * 48;
                                    auto itemid = read<uint32_t>(itemAddv + 4);
                                    auto itemname = GetBoxItems(itemid);
                                    if (itemname != "")
                                    {
                                        auto itemcount = read<int>(itemAddv + 0x18);
                                        itemdata.items.push_back(itemcount > 1 ? itemname + " * " + to_string(itemcount) : itemname);
                                    }
                                }
                            }
                        }
                        itemListx.push_back(itemdata);
                        continue;
                    }
                }
                std::string grenade = GetGrenadeType(entityClassName);
                if (grenade.find(xorstr_("Unknown")) == string::npos)
                {
                    uint32_t grenadeWorld = read<uint32_t>(entityAddv + offsets::uObjectWorld);
                    GrenadeData grenadedata;
                    grenadedata.Position = read<Vector3>(grenadeWorld + offsets::uObjectPosition);
                    grenadedata.Name = grenade;
                    grenadeListx.push_back(grenadedata);
                    continue;
                }
                if (IsPlayer(entityClassName))
                {
                    if (std::find_if(playerList.cbegin(), playerList.cend(), [entityAddv](const PlayerData &s) {
                            return s.Address == entityAddv;
                        }) != playerList.cend())
                        continue;
                    uint32_t playerWorld = read<uint32_t>(entityAddv + offsets::uObjectWorld);
                    PlayerData playerData;
                    playerData.Type = entityClassName;
                    playerData.Address = entityAddv;
                    playerData.Position = read<Vector3>(playerWorld + offsets::uObjectPosition);
                    playerData.Pose = read<int>(playerWorld + offsets::uObjectPose);
                    playerData.IsRobot = read<bool>(entityAddv + offsets::IsRobot);
                    // Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object
                    playerData.Health = read<float>(entityAddv + offsets::Health);
                    playerData.MaxHealth = read<float>(entityAddv + offsets::HealthMax);
                    playerData.NearDeathBreath = read<float>(entityAddv + offsets::NearDeathBreath);
                    // playerData.Rotation = read<Frotator>(entityAddv + 0xe24); //0xe24
                    playerData.Name = getPlayerName(entityAddv);
                    playerData.ID = getPlayerId(entityAddv);
                    // UAECharacter.Character.Pawn.Actor.Object
                    playerData.TeamID = read<uint32_t>(entityAddv + offsets::TeamID);
                    playerData.CurrentVehicle = read<uint32_t>(entityAddv + offsets::CurrentVehicle);
                    if (std::find(friends.begin(), friends.end(), playerData.ID) != friends.end())
                    {
                        playerData.isfriend = true;
                    }
                    auto CharacterWeaponManager = read<uint32_t>(entityAddv + offsets::CharacterWeaponManager);
                    if (CharacterWeaponManager)
                    {
                        auto CurrentWeaponReplicated = read<uint32_t>(CharacterWeaponManager + offsets::CurrentWeaponReplicated);
                        playerData.CurBulletNumInClip = read<int>(CurrentWeaponReplicated + offsets::CurBulletNumInClip);
                        playerData.CurMaxBulletNumInOneClip = read<int>(CurrentWeaponReplicated + offsets::CurMaxBulletNumInOneClip);
                        if (strcmp(entityClassName.c_str(), xorstr_("PlayerPawn_Infec_NormalZombie_C")) == 0 ||
                            strcmp(entityClassName.c_str(), xorstr_("PlayerPawn_Infec_InvisibleZombie")) == 0 ||
                            strcmp(entityClassName.c_str(), xorstr_("PlayerPawn_Infec_Revenger_C")) == 0 ||
                            strcmp(entityClassName.c_str(), xorstr_("PlayerPawn_Infec_KingZombie_C")) == 0)
                        {
                            playerData.WeaponName = xorstr_("Fist");
                        }
                        else
                        {
                            if (CurrentWeaponReplicated)
                            {
                                playerData.WeaponName = getEntityClassName(
                                    gNames, read<uint32_t>(CurrentWeaponReplicated + 16));
                                try
                                {
                                    auto weaponshit = r_utils::split(playerData.WeaponName, xorstr_("_"));

                                    playerData.WeaponName = weaponshit[2];
                                }
                                catch (...)
                                {
                                    playerData.WeaponName = xorstr_("Unknown");
                                }
                            }
                            else
                            {
                                playerData.WeaponName = xorstr_("Fist");
                            }
                        }
                    }
                    if (entityAddv == LocalPlayerAddr)
                    {
                        LocalPlayer = playerData;
                        continue;
                    }
                    if (LocalPlayer.TeamID == playerData.TeamID)
                    {
                        continue;
                    }
                    if (playerData.IsRobot && !settings::botesp)
                    {
                        continue;
                    }
                    playerList.push_back(playerData);
                    if (playerData.IsRobot)
                    {
                        botlistx.push_back(playerData);
                    }
                    continue;
                }
            }
        }

        playerList2 = playerList;
        botlist = botlistx;
        vehicleList = vehicleListx;
        itemList = itemListx;
        grenadeList = grenadeListx;
        Sleep(20);
    }
}

// typedef struct _D3DMATRIX {
//	union {
//		struct {
//			float        _11, _12, _13, _14;
//			float        _21, _22, _23, _24;
//			float        _31, _32, _33, _34;
//			float        _41, _42, _43, _44;
//
//		};
//		float m[4][4];
//	};
//} D3DMATRIX;
D3DMATRIX viewMatrix;
bool WorldToScreen(Vector3 pos, Vector3 &screen, int *distance)
{
    float screenW = (viewMatrix._14 * pos.x) + (viewMatrix._24 * pos.y) +
                    (viewMatrix._34 * pos.z + viewMatrix._44);
    screen.z = screenW;
    *distance = (screenW / 100);
    if (screenW < 0.0001f)
        return false;
    screenW = 1 / screenW;
    float sightX = (Width / 2);
    float sightY = (Height / 2);
    screen.x = sightX + (viewMatrix._11 * pos.x + viewMatrix._21 * pos.y +
                         viewMatrix._31 * pos.z + viewMatrix._41) *
                            screenW * sightX;
    screen.y = sightY - (viewMatrix._12 * pos.x + viewMatrix._22 * pos.y +
                         viewMatrix._32 * pos.z + viewMatrix._42) *
                            screenW * sightY;
    return true;
}
bool WorldToScreenItem(Vector3 pos, Vector2 &screen)
{
    float screenW = (viewMatrix._14 * pos.x) + (viewMatrix._24 * pos.y) +
                    (viewMatrix._34 * pos.z + viewMatrix._44);
    if (screenW < 0.0001f)
        return false;
    screenW = 1 / screenW;
    float sightX = (Width / 2);
    float sightY = (Height / 2);
    screen.x = sightX + (viewMatrix._11 * pos.x + viewMatrix._21 * pos.y +
                         viewMatrix._31 * pos.z + viewMatrix._41) *
                            screenW * sightX;
    screen.y = sightY - (viewMatrix._12 * pos.x + viewMatrix._22 * pos.y +
                         viewMatrix._32 * pos.z + viewMatrix._42) *
                            screenW * sightY;
    return true;
}

bool WorldToScreenPlayer(Vector3 pos, Vector3 &screen)
{
    float screenW = (viewMatrix._14 * pos.x) + (viewMatrix._24 * pos.y) +
                    (viewMatrix._34 * pos.z + viewMatrix._44);
    if (screenW < 0.0001f)
        return false;
    float screenY = (viewMatrix._12 * pos.x) + (viewMatrix._22 * pos.y) +
                    (viewMatrix._32 * (pos.z + 85) + viewMatrix._42);
    float screenX = (viewMatrix._11 * pos.x) + (viewMatrix._21 * pos.y) +
                    (viewMatrix._31 * pos.z + viewMatrix._41);
    screen.y = (Height / 2) - (Height / 2) * screenY / screenW;
    screen.x = (Width / 2) + (Width / 2) * screenX / screenW;
    float y1 = (Height / 2) - (viewMatrix._12 * pos.x + viewMatrix._22 * pos.y +
                               viewMatrix._32 * (pos.z - 95) + viewMatrix._42) *
                                  (Height / 2) / screenW;
    screen.z = y1 - screen.y;
    return true;
}
void DrawPlayerBlood(float x, float y, float h, float w, float Health,
                     float maxhealth, float NearDeathBreath)
{
    /**/

    int CurrentHP = max(0, min((int)Health, (int)maxhealth));
    auto r = min((510 * ((int)maxhealth - CurrentHP)) / (int)maxhealth, 255);
    auto g = min((510 * CurrentHP) / (int)maxhealth, 255);
    auto HPColor = RGBA(r, g, 0, 255);
    if (Health <= 0)
    {
        Health = NearDeathBreath;
        HPColor = Col.red;
    }
    int BarHeight = h * Health / maxhealth;

    DrawFilledRect(x, y, w, h, &Col.black);
    DrawFilledRect(x, y + (h - BarHeight), w, BarHeight, &HPColor);
}
void DrawPlayerBloodHorizonal(float x, float y, float h, float w, float Health,
                              float maxhealth)
{
    int CurrentHP = max(0, min((int)Health, (int)maxhealth));
    auto r = min((510 * ((int)maxhealth - CurrentHP)) / (int)maxhealth, 255);
    auto g = min((510 * CurrentHP) / (int)maxhealth, 255);

    DrawFilledRect(x, y, w, h, &Col.black);
    auto healthcolor = RGBA(r, g, 0, 255);
    DrawFilledRect(x, y, w * Health / maxhealth, h, &healthcolor);
}
void DrawVehicleBloodHorizonal(float x, float y, float h, float w, float fBlood,
                               float maxhealth)
{
    /**/
    if (fBlood > maxhealth - maxhealth / 4)
    {
        DrawFilledRect(x, y, w, h, &Col.black);
        DrawFilledRect(x, y, w * fBlood / maxhealth, h, &Col.green);
    }
    else if (fBlood > maxhealth - maxhealth / 2)
    {
        DrawFilledRect(x, y, w, h, &Col.black);
        DrawFilledRect(x, y, w * fBlood / maxhealth, h, &Col.yellow);
    }
    else
    {
        DrawFilledRect(x, y, w, h, &Col.black);
        DrawFilledRect(x, y, w * fBlood / maxhealth, h, &Col.red);
    }
    /**/
}
void DrawVehicleFuelHorizonal(float x, float y, float h, float w, float fBlood,
                              float maxhealth)
{
    DrawFilledRect(x, y, w, h, &Col.black);
    auto fuelcolor = RGBA(255, 128, 0);
    DrawFilledRect(x, y, w * fBlood / maxhealth, h, &fuelcolor);
    /**/
}
void DrawPlayerBone(uint32_t Object, float w, RGBA color, float thickness)
{
    float sightX = Width / 2, sightY = Height / 2;
    int distance = 0;
    Vector3 headPos = GetBoneWorldPosition(Object, 5);
    headPos.z += 7;
    Vector2 head;
    WorldToScreenItem(headPos, head);
    Vector2 neck = head;
    Vector2 chest;
    Vector3 chestPos = GetBoneWorldPosition(Object, 4);
    WorldToScreenItem(chestPos, chest);
    Vector2 pelvis;
    Vector3 pelvisPos = GetBoneWorldPosition(Object, 1);
    WorldToScreenItem(pelvisPos, pelvis);
    Vector2 lSholder;
    Vector3 lSholderPos = GetBoneWorldPosition(Object, 11);
    WorldToScreenItem(lSholderPos, lSholder);
    Vector2 rSholder;
    Vector3 rSholderPos = GetBoneWorldPosition(Object, 32);
    WorldToScreenItem(rSholderPos, rSholder);
    Vector2 lElbow;
    Vector3 lElbowPos = GetBoneWorldPosition(Object, 12);
    WorldToScreenItem(lElbowPos, lElbow);
    Vector2 rElbow;
    Vector3 rElbowPos = GetBoneWorldPosition(Object, 33);
    WorldToScreenItem(rElbowPos, rElbow);
    Vector2 lWrist;
    Vector3 lWristPos = GetBoneWorldPosition(Object, 63);
    WorldToScreenItem(lWristPos, lWrist);
    Vector2 rWrist;
    Vector3 rWristPos = GetBoneWorldPosition(Object, 62);
    WorldToScreenItem(rWristPos, rWrist);
    Vector2 lThigh;
    Vector3 lThighPos = GetBoneWorldPosition(Object, 52);
    WorldToScreenItem(lThighPos, lThigh);
    Vector2 rThigh;
    Vector3 rThighPos = GetBoneWorldPosition(Object, 56);
    WorldToScreenItem(rThighPos, rThigh);
    Vector2 lKnee;
    Vector3 lKneePos = GetBoneWorldPosition(Object, 53);
    WorldToScreenItem(lKneePos, lKnee);
    Vector2 rKnee;
    Vector3 rKneePos = GetBoneWorldPosition(Object, 57);
    WorldToScreenItem(rKneePos, rKnee);
    Vector2 lAnkle;
    Vector3 lAnklePos = GetBoneWorldPosition(Object, 54);
    WorldToScreenItem(lAnklePos, lAnkle);
    Vector2 rAnkle;
    Vector3 rAnklePos = GetBoneWorldPosition(Object, 58);

    WorldToScreenItem(rAnklePos, rAnkle);
    if (lAnklePos.x != NULL && chest.x != NULL && pelvis.x != NULL &&
        lSholder.x != NULL && rSholder.x != NULL && lElbow.x != NULL &&
        rElbow.x != NULL && lWrist.x != NULL && rWrist.x != NULL &&
        lThigh.x != NULL && rThigh.x != NULL && lKnee.x != NULL &&
        lAnkle.x != NULL && rAnkle.x != NULL)
    {
        DrawCircle(head.x, head.y, w / 6, &color, 20, thickness);
        // DrawLine(neck.x, neck.y, chest.x, chest.y, &Col.white, 1);
        DrawLine(chest.x, chest.y, pelvis.x, pelvis.y, &color, thickness);

        DrawLine(chest.x, chest.y, lSholder.x, lSholder.y, &color, thickness);
        DrawLine(chest.x, chest.y, rSholder.x, rSholder.y, &color, thickness);

        DrawLine(lSholder.x, lSholder.y, lElbow.x, lElbow.y, &color, thickness);
        DrawLine(rSholder.x, rSholder.y, rElbow.x, rElbow.y, &color, thickness);

        DrawLine(lElbow.x, lElbow.y, lWrist.x, lWrist.y, &color, thickness);
        DrawLine(rElbow.x, rElbow.y, rWrist.x, rWrist.y, &color, thickness);

        DrawLine(pelvis.x, pelvis.y, lThigh.x, lThigh.y, &color, thickness);
        DrawLine(pelvis.x, pelvis.y, rThigh.x, rThigh.y, &color, thickness);

        DrawLine(lThigh.x, lThigh.y, lKnee.x, lKnee.y, &color, thickness);
        DrawLine(rThigh.x, rThigh.y, rKnee.x, rKnee.y, &color, thickness);

        DrawLine(lKnee.x, lKnee.y, lAnkle.x, lAnkle.y, &color, thickness);
        DrawLine(rKnee.x, rKnee.y, rAnkle.x, rAnkle.y, &color, thickness);
    }
}
int closestenemy = -1;
float aimdistance;
void aimbot(float x, float y)
{
    float ScreenCenterX = (Width / 2);
    float ScreenCenterY = (Height / 2);
    int AimSpeed = settings::aimbotsmooth;
    float TargetX = 0;
    float TargetY = 0;

    if (x != 0)
    {
        if (x > ScreenCenterX)
        {
            TargetX = -(ScreenCenterX - x);
            TargetX /= AimSpeed;
            if (TargetX + ScreenCenterX > ScreenCenterX * 2)
                TargetX = 0;
        }

        if (x < ScreenCenterX)
        {
            TargetX = x - ScreenCenterX;
            TargetX /= AimSpeed;
            if (TargetX + ScreenCenterX < 0)
                TargetX = 0;
        }
    }

    if (y != 0)
    {
        if (y > ScreenCenterY)
        {
            TargetY = -(ScreenCenterY - y);
            TargetY /= AimSpeed;
            if (TargetY + ScreenCenterY > ScreenCenterY * 2)
                TargetY = 0;
        }

        if (y < ScreenCenterY)
        {
            TargetY = y - ScreenCenterY;
            TargetY /= AimSpeed;
            if (TargetY + ScreenCenterY < 0)
                TargetY = 0;
        }
    }

    mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);

    return;
}
Vector2 Get2dBonePos(uint32_t Object, int id)
{
    int distance;
    auto bonepos = GetBoneWorldPosition(Object, id);
    if (settings::aimbotbone == 0 && id == 5)
        bonepos.z += 9.0f;
    if (settings::aimbotbone == 3 && id == 4)
        bonepos.z -= 16.0f;
    if (settings::aimbotbone == 2 && id == 4)
        bonepos.z -= 8.0f;
    Vector2 result;
    auto toscreen = WorldToScreenItem(bonepos, result);
    return result;
}
HANDLE magicxt;
Vector3 GetBonePos(uint32_t Object, int id)
{
    auto bonepos = GetBoneWorldPosition(Object, id);
    bonepos.z -= 9.0f;
    if (settings::aimbotbone == 3 && id == 4)
        bonepos.z -= 16.0f;
    if (settings::aimbotbone == 2 && id == 4)
        bonepos.z -= 8.0f;

    return bonepos;
}
void ClampAngles(Frotator &vAngles)
{
    while (vAngles.Yaw < -180.0f)
        vAngles.Yaw += 360.0f;
    while (vAngles.Yaw > 180.0f)
        vAngles.Yaw -= 360.0f;

    if (vAngles.Pitch < -74.f)
        vAngles.Pitch = -74.f;
    if (vAngles.Pitch > 74.f)
        vAngles.Pitch = 74.f;
}
Frotator ToRotator(Vector3 MyPosition, Vector3 AimPosition)
{
    auto vecDelta = AimPosition - MyPosition;
    Frotator coord;
    coord.Yaw = std::atan2(vecDelta.y, vecDelta.x) * 180.0f / M_PI;
    coord.Pitch = std::atan2(vecDelta.z, std::sqrt(vecDelta.x * vecDelta.x +
                                                   vecDelta.y * vecDelta.y)) *
                  180.0f / M_PI;
    ClampAngles(coord);
    return coord;
}

Frotator CalcAngle_Enemy(Vector3 LocalPosition, Vector3 EnemyHeadPosition)
{
    auto vecDelta = EnemyHeadPosition - LocalPosition;
    float num = (float)std::sqrt(
        (double)(vecDelta.x * vecDelta.x + vecDelta.y * vecDelta.y));
    Frotator coord;
    coord.Pitch = -(float)std::atan((double)(vecDelta.z / num)) * 57.29578f;
    coord.Yaw = (float)::atan((double)(vecDelta.y / vecDelta.x)) * 57.29578f;
    if (vecDelta.x >= 0.f)
    {
        coord.Yaw += 180.f;
    }
    return coord;
}
bool IsAiming(PlayerData Enemy)
{
    auto Rotation =
        CalcAngle_Enemy(LocalPlayer.Position, GetBonePos(Enemy.Address, 5));
    return std::abs(Enemy.Rotation.Pitch - Rotation.Pitch) +
               std::abs(Enemy.Rotation.Yaw - Rotation.Yaw) <
           4.f;
}
VehicleData GetClosestVehicle()
{
    VehicleData closestvehicle{};
    float closestDistance = FLT_MAX;
    for (auto vehicle : vehicleList)
    {
        Vector2 screenloc{};
        WorldToScreenItem(vehicle.Position, screenloc);
        auto dx = screenloc.x - (Width / 2);
        auto dy = screenloc.y - (Height / 2);
        auto dist = sqrtf(dx * dx + dy * dy);
        if (dist < closestDistance)
        {
            if (vehicle.Address == uMyObject)
                continue;
            closestvehicle = vehicle;
            closestDistance = dist;
        }
    }
    return closestvehicle;
}
PlayerData aimenemy;
bool cartrackheld = false;
VehicleData closestvehicle{};

uint32_t magichookaddr = 0;
uint32_t orig_shoot_event = 0;

//// Enum ShadowTrackerExtra.EWeaponType
// enum class EWeaponType : uint8_t
//{
//	AWT_None = 0,
//	AWT_Pistol = 1,
//	AWT_PistolSilencer = 2,
//	AWT_ShotGun = 3,
//	AWT_SubmachineGun = 4,
//	AWT_RPG = 5,
//	AWT_ChargeGun = 6,
//	AWT_Knife = 7,
//	AWT_EnegyCaptureWhip = 8,
//	AWT_RifleGun = 9,
//	AWT_RPG7 = 10,
//	AWT_MAX = 11
// };

void AimBot()
{
    while (true)
    {
        MUTATE_START
        auto CharacterWeaponManager = read<uint32_t>(LocalPlayerAddr + offsets::CharacterWeaponManager);
        if (!CharacterWeaponManager)
            continue;
        auto CurrentWeaponReplicated = read<uint32_t>(CharacterWeaponManager + offsets::CurrentWeaponReplicated);
        if (!CharacterWeaponManager)
            continue;
        auto weapontype = read<uint8_t>(LocalPlayerAddr + offsets::CurAnimParamList + 0x38);
        if (!weapontype || weapontype == 6 || weapontype == 7)
            continue;
        // if (settings::cartrack)
        //{
        //	cartrackheld = GetAsyncKeyState(VK_CAPITAL) & 0x8000;
        //	if (cartrackheld)
        //	{
        //		closestvehicle = GetClosestVehicle();
        //		if (closestvehicle.Address)
        //		{
        //			continue;
        //		}
        //	}
        // }
        MUTATE_END
        if (settings::showMenu == true || settings::enableaimbot == false)
        {
            Sleep(10);
            continue;
        }
        if (!threadcreated)
        {
            if (settings::aimtype == 2)
            {
                magicxt = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(MagicThread), nullptr, NULL, nullptr);
                threadcreated = true;
            }
        }
        if (!GetAsyncKeyState(settings::aimbotkey))
            if (MagicBulletHook)
                write<bool>(MagicBulletHook + 13, false);
        float closestDistance = FLT_MAX;
        float whiteclosest = FLT_MAX;

        uint32_t viewMatrixAddr = read<uint32_t>(read<uint32_t>(ViewMatrixBase) + 32) + 512;
        viewMatrix = read<D3DMATRIX>(viewMatrixAddr);
        int boneid = 5;
        if (settings::aimbotbone == 0 || settings::aimbotbone == 1)
            boneid = 5;
        else if (settings::aimbotbone == 2 || settings::aimbotbone == 3)
            boneid = 4;
        else if (settings::aimbotbone == 4)
            boneid = 1;
        else
            boneid = 5;
        Vector2 aimBone;
        Vector3 MagicHead;
        PlayerData aimentity;
        PlayerData whitelist;

        if (settings::enableaimbot)
        {
            for (int i = 0; i < playerList2.size(); i++)
            {
                auto bonepos = Get2dBonePos(playerList2[i].Address, boneid);
                float MaxFov = settings::aimbotfov;
                auto dx = bonepos.x - (Width / 2);
                auto dy = bonepos.y - (Height / 2);
                auto dist = sqrtf(dx * dx + dy * dy);
                if (dist < MaxFov && dist < whiteclosest)
                {
                    if (playerList2[i].Address == uMyObject)
                        continue;
                    whitelist = playerList2[i];
                    whiteclosest = dist;
                }
                if (dist < MaxFov && dist < closestDistance)
                {
                    if (playerList2[i].Address == uMyObject)
                        continue;
                    if (!settings::aimKnocked)
                    {
                        if (!playerList2[i].Health > 0.f)
                        {
                            continue;
                        }
                    }
                    aimentity = playerList2[i];
                    closestDistance = dist;
                    aimBone = bonepos;
                }
            }

            if (whiteclosest != FLT_MAX)
            {
                if (GetAsyncKeyState(settings::whitelistkey) & 1)
                {
                    if (std::find(friends.begin(), friends.end(), whitelist.ID) ==
                        friends.end())
                    {
                        friends.push_back(whitelist.ID);
                    }
                    else
                    {
                        friends.erase(
                            std::find(friends.begin(), friends.end(), whitelist.ID));
                    }
                }
            }

            // Vector3 MyPos = GetPlayerPos(CameraAddress);

            if (closestDistance != FLT_MAX)
            {
                aimenemy = aimentity;
                if (GetAsyncKeyState(settings::aimbotkey))
                {
                    auto ShootWeaponEntityComp = read<uint32_t>(CurrentWeaponReplicated + offsets::ShootWeaponEntityComp);
                    auto flBulletSpeed = read<float>(ShootWeaponEntityComp + offsets::BulletFireSpeed);
                    auto aimPos = GetBonePos(aimentity.Address, boneid);
                    auto entrootcomp = read<uint32_t>(aimentity.Address + offsets::uObjectWorld);
                    auto Distance = LocalPlayer.Position.Distance(aimPos) / 100.f;
                    auto Time = Distance / (flBulletSpeed / 100.f);
                    auto Velocity = read<Vector3>(entrootcomp + offsets::ComponentVelocity);
                    if (settings::aimprediction)
                        aimPos = aimPos + Velocity * Time;
                    if (settings::aimtype == 2 && magiciniting == false)
                    {
                        auto MyPos = GetPlayerPos(read<uint32_t>(CurrentWeaponReplicated + offsets::uObjectWorld));
                        auto Magic = ToRotator(MyPos, aimPos);
                        if (!HookMagic(Magic))
                        {
                            if (settings::autorefresh)
                            {
                                magiciniting = true;
                                threadcreated = false;
                            }
                            continue;
                        }
                    }
                    else if (settings::aimtype == 1)
                    {
                        auto Magic = ToRotator(read<Vector3>(uCamera), aimPos);
                        write<Frotator>(playerCarry + offsets::ControlRotation, Magic);
                    }
                    else
                    {
                        aimbot(aimBone.x, aimBone.y);
                    }
                    //
                }
                // closestDistance = FLT_MAX;
            }
            else
            {
                aimentity.Address = 0;
                if (settings::aimtype == 2)
                {
                    RestoreMagic();
                }
            }
        }

        //
        //
        Sleep(2);
    }
}
RGBA GetItemColor(string name)
{
    if (name.find(xorstr_("[level1]")) != string::npos)
    {
        return {240, 255, 240, 255};
    }
    if (name.find(xorstr_("[level2]")) != string::npos)
    {
        return {255, 222, 173, 255};
    }
    if (name.find(xorstr_("[level3]")) != string::npos)
    {
        return {169, 169, 169, 255};
    }
    if (name.find(xorstr_("[level4]")) != string::npos)
    {
        return Col.red;
    }
    if (name.find(xorstr_("[level5]")) != string::npos)
    {
        return {0, 208, 255, 255};
    }
    if (name.find(xorstr_("[level6]")) != string::npos)
    {
        return {173, 216, 230, 255};
    }
    if (name.find(xorstr_("[level7]")) != string::npos)
    {
        return {255, 165, 0, 255};
    }
    if (name.find(xorstr_("[level8]")) != string::npos)
    {
        return {255, 0, 255, 255};
    }
    if (name.find(xorstr_("[level9]")) != string::npos)
    {
        return {124, 252, 0, 255};
    }
    if (name.find(xorstr_("[level10]")) != string::npos)
    {
        return {255, 255, 0, 255};
    }
    if (name.find(xorstr_("[level11]")) != string::npos)
    {
        return {255, 165, 0, 255};
    }
    if (name.find(xorstr_("[level12]")) != string::npos)
    {
        return {0, 139, 139, 255};
    }
    if (name.find(xorstr_("[level13]")) != string::npos)
    {
        return {255, 215, 0, 255};
    }
    return Col.green;
}
bool isdefault(float color[])
{
    if (color[0] == 1.00f && color[1] == 1.00f && color[2] == 1.00f)
        return true;
    return false;
}
bool IsMouseOver(ImGuiIO io, float x, float y, float w, float h)
{
    return (io.MousePos.x > x && io.MousePos.x < x + w && io.MousePos.y > y &&
            io.MousePos.y < y + h);
}

Vector2 network_reset_pos{0, 0};
Vector2 enemy_count_pos{0, 0};
std::once_flag positions;
void ESP()
{
    std::call_once(positions, [&] {
        std::string temptext = xorstr_("Total: 0 Player: 0 Bot: 0");
        auto tsize = m_pFont->CalcTextSizeA(settings::playercountsize, FLT_MAX, 0, temptext.c_str());
        enemy_count_pos = Vector2((Width / 2) - tsize.x, 100 + tsize.y);
        network_reset_pos = Vector2(enemy_count_pos.x, enemy_count_pos.y + 20);
    });
    //
    //
    uint32_t viewMatrixAddr = read<uint32_t>(read<uint32_t>(ViewMatrixBase) + 32) + 512;
    viewMatrix = read<D3DMATRIX>(viewMatrixAddr);
    if (settings::caresp)
    {
        for (int i = 0; i < vehicleList.size(); i++)
        {
            Vector2 screen;
            if (WorldToScreenItem(vehicleList[i].Position, screen))
            {
                int distance = LocalPlayer.Position.Distance(vehicleList[i].Position) / 100;
                RGBA Color = vehicleList[i].bIsEngineStarted ? Col.red : FLOAT2RGBA(settings::carcolor);
                if (vehicleList[i].Name == xorstr_("Buggy"))
                    Color = Col.green;
                if (distance > 1000)
                    continue;
                if (cartrackheld && vehicleList[i].Address == closestvehicle.Address)
                    Color = Col.red;
                float x = screen.x;
                float y = screen.y;
                std::string name = vehicleList[i].Name + " [" + to_string(distance) + "M]";
                auto textsize = m_pFont->CalcTextSizeA(settings::vehiclefontsize, FLT_MAX, 0, name.c_str());
                DrawStrokeText(x, y, &Color, name.c_str(), settings::vehiclefontsize);
                if (settings::vehiclehealthgastype)
                {
                    char carhp[64];
                    char carfuel[64];
                    auto hpcolor = FLOAT2RGBA(settings::vhealthcolor);
                    sprintf(carhp, xorstr_("HP: %.0f%% - Gas: %.0f%%"), vehicleList[i].HP / vehicleList[i].HPMax * 100, vehicleList[i].Fuel / vehicleList[i].FuelMax * 100);
                    DrawStrokeText(x, y + settings::vehiclefontsize, &hpcolor, carhp, settings::vehiclefontsize);
                }
                else
                {
                    DrawVehicleBloodHorizonal(x, y + 20, 3, textsize.x, vehicleList[i].HP, vehicleList[i].HPMax);
                    DrawVehicleFuelHorizonal(x, y + 30, 3, textsize.x, vehicleList[i].Fuel, vehicleList[i].FuelMax);
                }
            }
        }
    }
    if (settings::itemesp)
    {
        for (int i = 0; i < itemList.size(); i++)
        {
            string name = itemList[i].Name.c_str();
            RGBA colx = GetItemColor(name);
            r_encryption::replace_all(name, xorstr_("[level1]"), "");
            r_encryption::replace_all(name, xorstr_("[level2]"), "");
            r_encryption::replace_all(name, xorstr_("[level3]"), "");
            r_encryption::replace_all(name, xorstr_("[level4]"), "");
            r_encryption::replace_all(name, xorstr_("[level5]"), "");
            r_encryption::replace_all(name, xorstr_("[level6]"), "");
            r_encryption::replace_all(name, xorstr_("[level7]"), "");
            r_encryption::replace_all(name, xorstr_("[level8]"), "");
            r_encryption::replace_all(name, xorstr_("[level9]"), "");
            r_encryption::replace_all(name, xorstr_("[level10]"), "");
            r_encryption::replace_all(name, xorstr_("[level11]"), "");
            r_encryption::replace_all(name, xorstr_("[level12]"), "");
            r_encryption::replace_all(name, xorstr_("[level13]"), "");
            Vector2 screen;
            if (WorldToScreenItem(itemList[i].Position, screen))
            {
                int distance = LocalPlayer.Position.Distance(itemList[i].Position) / 100;
                if (itemList[i].Name.find(xorstr_("Airdrop")) == string::npos)
                {
                    if (itemList[i].Name.find(xorstr_("Dead box")) == string::npos)
                    {
                        if (distance > 40)
                            continue;
                    }
                    else
                    {
                        if (distance > 100)
                            continue;
                    }
                }

                float x = screen.x;
                float y = screen.y;
                name += " [" + to_string(distance) + xorstr_("M]");
                DrawStrokeText(x, y, &colx, name.c_str(), settings::itemfontsize);

                if (itemList[i].pickupbox)
                {
                    for (int h = 0; h < itemList[i].items.size(); h++)
                    {
                        DrawStrokeText(x, y - ((h + 1) * 15), &Col.yellow,
                                       itemList[i].items[h].c_str(),
                                       settings::itemfontsize);
                    }
                }
            }
        }
    }
    for (int i = 0; i < grenadeList.size(); i++)
    {
        // DrawShadowText()
        Vector2 screen;
        if (WorldToScreenItem(grenadeList[i].Position, screen))
        {
            int distance = LocalPlayer.Position.Distance(grenadeList[i].Position) / 100;
            float x = screen.x;
            float y = screen.y;
            std::string distances = "[" + to_string(distance) + xorstr_("M]");
            DrawStrokeText(x, y, &Col.red, grenadeList[i].Name.c_str());
            DrawStrokeText(x, y + 15, &Col.red, distances.c_str());
        }
    }
    if (settings::fovcircle && settings::enableaimbot)
    {
        auto circlecolor = FLOAT2RGBA(settings::circlecolor);
        DrawCircle(Width / 2, Height / 2, settings::aimbotfov, &circlecolor, 50);
    }
    for (int i = 0; i < playerList2.size(); i++)
    {
        RGBA BoxColor = FLOAT2RGBA(settings::boxcolor);
        RGBA BoneColor = FLOAT2RGBA(settings::bonecolor);
        RGBA LineColor = FLOAT2RGBA(settings::linecolor);
        if (isdefault(settings::boxcolor))
        {
            BoxColor.random_color(playerList2[i].TeamID);
        }
        if (isdefault(settings::bonecolor))
        {
            BoneColor.random_color(playerList2[i].TeamID);
        }
        if (isdefault(settings::linecolor))
        {
            LineColor.random_color(playerList2[i].TeamID);
        }
        if (playerList2[i].IsRobot)
        {
            BoxColor = FLOAT2RGBA(settings::botcolor);
            LineColor = BoxColor;
            BoneColor = BoxColor;
        }
        char tecmp[256];
        sprintf_s(tecmp, xorstr_("Total: %d Player: %d Bot: %d"),
                  playerList2.size(), playerList2.size() - botlist.size(),
                  botlist.size());
        auto tsize =
            m_pFont->CalcTextSizeA(settings::playercountsize, FLT_MAX, 0, tecmp);
        char resetbuf[256];
        sprintf_s(resetbuf, xorstr_("Resetting network in %d seconds!"), ((lasttick + 210000) - GetTickCount()) / 1000);
        auto networksize = m_pFont->CalcTextSizeA(20, FLT_MAX, 0, resetbuf);
        DrawStrokeText(enemy_count_pos.x, enemy_count_pos.y, &Col.red, tecmp,
                       settings::playercountsize);
        string aimbotposshit[5] = {xorstr_("Head"), xorstr_("Neck"),
                                   xorstr_("Chest"), xorstr_("Stomach"),
                                   xorstr_("Pelvis")};
        DrawStrokeText(Width / 2 - 100.f, Height - 170, &Col.green, string(xorstr_("Current aim position : ") + aimbotposshit[settings::aimbotbone]).c_str(), 17);

        static int dragindex = 0;
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        if (io.MouseDown[0] && settings::showMenu)
        {
            if (!dragindex)
            {
                if (IsMouseOver(io, enemy_count_pos.x, enemy_count_pos.y, tsize.x, tsize.y))
                    dragindex = 1;
                else if (IsMouseOver(io, network_reset_pos.x, network_reset_pos.y, networksize.x, networksize.y))
                    dragindex = 2;
            }
            else
            {
                Vector2 pos = {io.MousePos.x, io.MousePos.y};
                if (dragindex == 1)
                {
                    if (enemy_count_pos.x != pos.x || enemy_count_pos.y != pos.y)
                    {
                        enemy_count_pos = pos;
                    }
                }
                else if (dragindex == 2)
                {
                    if (network_reset_pos.x != pos.x || network_reset_pos.y != pos.y)
                    {
                        network_reset_pos = pos;
                    }
                }
            }
        }
        else
        {
            dragindex = 0;
        }

        Vector3 screen;
        if (WorldToScreenPlayer(playerList2[i].Position, screen))
        {
            int distance = LocalPlayer.Position.Distance(playerList2[i].Position) / 100;
            if (distance > settings::maxdistance)
                continue;
            bool robot = false;
            if (playerList2[i].IsRobot)
            {
                robot = true;
                // playerList2[i].Name = xorstr_(("[BOT]"));
            }

            sprintf(drawBuff, " %s ", playerList2[i].Name.c_str());

            // sprintf_s(drawBuff, u8"%s", playerList2[i].Name.c_str());
            // sprintf(drawBuff, "%s", playerList2[i].Name);
            int revise = strlen(drawBuff) * 7 + 28;
            float x = screen.x;
            float y = screen.y;
            float h = screen.z;
            float w = screen.z / 2;

            if (playerList2[i].Pose == 1114636288)
            {
                y = screen.y + screen.z / 5;
                h -= screen.z / 5;
            }
            if (playerList2[i].Pose == 1112014848)
            {
                y = screen.y + screen.z / 4;
                h -= screen.z / 4;
            }
            Vector2 headscreen;
            WorldToScreenItem(GetBonePos(playerList2[i].Address, 5), headscreen);
            if (settings::playerline)
            {
                DrawLine(Width / 2, 0, headscreen.x, headscreen.y, &LineColor,
                         settings::linethickness);
            }
            if (aimenemy.Address == playerList2[i].Address && settings::aimline)
            {
                auto aimlinecolor = FLOAT2RGBA(settings::aimlinecolor);
                DrawLine(Width / 2, Height / 2, headscreen.x, headscreen.y,
                         &aimlinecolor, settings::linethickness);
            }
            if (settings::playerbox)
            {
                DrawCornerBox(x - screen.z / 4 - 3, y - 5, w + 3, h + 5, settings::boxthickness, &BoxColor);
            }

            if (settings::playerhealth)
            {
                if (settings::playerhealthtype)
                {
                    auto hpcolor = FLOAT2RGBA(settings::phealthcolor);

                    char healthtext[64];
                    sprintf(healthtext, xorstr_("%.0f%% HP"), playerList2[i].Health / playerList2[i].MaxHealth * 100);
                    DrawNewText2(healthtext, ImVec2(screen.x + w, screen.y + 20), 10, &hpcolor, true);
                }
                else
                {
                    DrawPlayerBlood(x - screen.z / 4 - 6, y - 5, h + 5, 3, playerList2[i].Health, playerList2[i].MaxHealth, playerList2[i].NearDeathBreath);
                }
            }
            if (settings::playername)
            {
                auto menubackground = RGBA(88, 48, 224, 200);
                auto namecolr = RGBA(255, 128, 0, 255);
                auto distancetext = std::format("  {}M  ", distance);
                auto textsize = m_pFont->CalcTextSizeA(12, FLT_MAX, 0, drawBuff);
                auto distancetextsize = m_pFont->CalcTextSizeA(12, FLT_MAX, 0, distancetext.c_str());
                auto bottextsize = m_pFont->CalcTextSizeA(12, FLT_MAX, 0, playerList2[i].IsRobot ? "Bot  " : "Man  ");
                DrawFilledRect(screen.x - (textsize.x / 2) - bottextsize.x, screen.y - 28, textsize.x + distancetextsize.x + bottextsize.x, 16, &menubackground); // background
                DrawStrokeText(screen.x - (textsize.x / 2), screen.y - 27, &namecolr, drawBuff);
                if (playerList2[i].IsRobot)
                {
                    DrawStrokeText(screen.x - (textsize.x / 2) - bottextsize.x + 3, screen.y - 27, &Col.green, "Bot");
                }
                else
                {
                    DrawStrokeText(screen.x - (textsize.x / 2) - bottextsize.x + 3, screen.y - 27, &Col.red, "Man");
                }
                DrawStrokeText(screen.x + (textsize.x / 2), screen.y - 27, &Col.yellow, distancetext.c_str());

                // DrawStrokeText(screen.x + (revise / 2) + (Rect.w / 2) - 17, screen.y
                // + Rect.z - 27, &Col.yellow, string(to_string(distance) +
                // "M").c_str());

                // DrawNewText2(drawBuff, ImVec2(screen.x + screen.w / 2, screen.y +
                // screen.z), 10, &FLOAT2RGBA(settings::namecolor), true);
                // sprintf(drawBuff, "[%dM]", distance);
                // DrawNewText2(drawBuff, ImVec2(screen.x + screen.w / 2, screen.y +
                // screen.z + 10), 10, &FLOAT2RGBA(settings::namecolor), true);
            }
            int yheight = 10;
            if (settings::playerweapon)
            {
                char temptext[512];
                if (playerList2[i].CurMaxBulletNumInOneClip <= 200 && playerList2[i].CurMaxBulletNumInOneClip > 0)
                {
                    sprintf_s(temptext, xorstr_("%s (%d/%d)"), playerList2[i].WeaponName.c_str(), playerList2[i].CurBulletNumInClip, playerList2[i].CurMaxBulletNumInOneClip);
                }
                else
                {
                    sprintf_s(temptext, xorstr_("%s"), playerList2[i].WeaponName.c_str());
                }
                auto weaponcolor = FLOAT2RGBA(settings::weaponcolor);
                DrawStrokeText(x, y + h + yheight, &weaponcolor, temptext, 12, true);
                // DrawNewText2(temptext, ImVec2(x - screen.z / 4 - 3, y + h + yheight), 10, &weaponcolor, true);
            }
            if (playerList2[i].isfriend)
            {
                yheight += 15;
                auto friendcolor = RGBA(255, 255, 255);
                DrawStrokeText(x, y + h + yheight, &friendcolor, xorstr_("Friend"), 12, true);
            }
            if (settings::playerteam)
            {
                yheight += 10;
                auto teamcolor = FLOAT2RGBA(settings::playerteamcolor);
                DrawStrokeText(x, y + h + yheight, &teamcolor, std::format("{} {}", xorstr_("Team: "), playerList2[i].TeamID).c_str(), 12, true);
            }
            // if (IsAiming(playerList2[i]) && settings::playeraims)
            //{
            //	yheight += 10;
            //	DrawNewText2(xorstr_("Aiming"), ImVec2(x - screen.z / 4 - 3, y +
            // h + yheight), 10, &Col.red, true);
            //}
            if (settings::playerbone)
            {
                DrawPlayerBone(playerList2[i].Address, w, BoneColor, settings::bonethickness);
            }
        }
    }
    //
    //
}
HMODULE currentmodule;
BOOL FreeResFile(DWORD dwResName, LPCSTR lpResType, LPCSTR lpFilePathName)
{
    HMODULE hInstance = currentmodule;
    HRSRC hResID =
        ::FindResourceA(hInstance, MAKEINTRESOURCEA(dwResName), lpResType);
    HGLOBAL hRes = ::LoadResource(hInstance, hResID);
    LPVOID pRes = ::LockResource(hRes);
    if (pRes == NULL)
    {
        return FALSE;
    }
    DWORD dwResSize = ::SizeofResource(hInstance, hResID);
    HANDLE hResFile = CreateFileA(lpFilePathName, GENERIC_WRITE, 0, NULL,
                                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hResFile)
    {
        return FALSE;
    }

    DWORD dwWritten = 0;
    WriteFile(hResFile, pRes, dwResSize, &dwWritten, NULL);
    CloseHandle(hResFile);

    return (dwResSize == dwWritten);
}

const wchar_t *GetWC(const char *c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t *wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);
    return wc;
}
void killProcessByName(const char *filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (wcscmp(pEntry.szExeFile, GetWC(filename)) == 0)
        {
            HANDLE hProcess =
                OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}
bool firstrun;

bool verify_game()
{
    VM_START
again:
    m_pid = PIDManager::GetAowProcId(skCrypt(L"AndroidProcess.exe"));
    if (!m_pid)
    {
        m_pid = PIDManager::GetAowProcId(skCrypt(L"aow_exe.exe"));
    }

    m_Handle = OpenProcess(PROCESS_ALL_ACCESS, 0, m_pid);
    Controller = new DriverController(m_Handle);
    gamemodules = GetModules();
    cout << hex << gamemodules.libue4 << endl;
    if (gamemodules.libue4 == NULL /*|| gamemodules.libanogs == NULL ||
        gamemodules.libgcloud == NULL || gamemodules.libtprt == NULL ||
        gamemodules.libtdatamaster == NULL || gamemodules.libcubehawk == NULL*/
    )
    {
        delete Controller;
        Sleep(10);
        goto again;
    }
    auto pname = PIDManager::GetProcessNameById(m_pid);
    if (pname.find(xorstr_(L"Android")) != std::string::npos)
        issmartgaga = true;
    ULONG size = 0;
    char temp_directory[MAX_PATH] = {0};
    GetTempPathA(sizeof(temp_directory), temp_directory);
    char commandex[MAX_PATH];
    sprintf(commandex, xorstr_("/C del /s /q %s > Nul"), temp_directory);
    ShellExecuteA(0, 0, xorstr_("cmd.exe"), commandex, 0, 0);
    VM_END
    return true;
}
#define COL(value) (value / 255)
#pragma comment(lib, "legacy_stdio_definitions.lib")
#include "Shlobj_core.h"
HRESULT DirectXInit(HWND hWnd)
{
    //
    //
    DXGI_SWAP_CHAIN_DESC vSwapChainDesc;
    ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
    vSwapChainDesc.BufferCount = 2;
    vSwapChainDesc.BufferDesc.Width = 0;
    vSwapChainDesc.BufferDesc.Height = 0;
    vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    vSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    vSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    vSwapChainDesc.OutputWindow = hWnd;
    vSwapChainDesc.SampleDesc.Count = 1;
    vSwapChainDesc.SampleDesc.Quality = 0;
    vSwapChainDesc.Windowed = TRUE;
    vSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL vFeatureLevel;
    D3D_FEATURE_LEVEL vFeatureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };
    if (D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, vFeatureLevelArray, 2,
            D3D11_SDK_VERSION, &vSwapChainDesc, &ms_vSwapChain, &ms_vD3dDevice,
            &vFeatureLevel, &ms_vDeviceContext) != S_OK)
        return FALSE;

    ID3D11Texture2D *pBackBuffer = nullptr;
    ms_vSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    ms_vD3dDevice->CreateRenderTargetView(pBackBuffer, NULL,
                                          &ms_vRenderTargetView);
    pBackBuffer->Release();

    // IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    // grey 0.862f, 0.862f, 0.862f, .90f
    // blue 0.f, 0.749f, 1.f, 1.00f

    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);

    colors[ImGuiCol_WindowBg] = ImVec4(0.125f, 0.125f, 0.149f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.227f, 1.00f);
    // colors[ImGuiCol_Border] = ImVec4(0.917, 0.223, 0.223, .25f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.070f, 0.070f, 0.101f, 1.00f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.137f, 0.137f, 0.160f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.231f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.862f, 0.862f, 0.862f, .50f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

    colors[ImGuiCol_SliderGrab] = ImVec4(0.862f, 0.862f, 0.862f, .90f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.137f, 0.137f, 0.160f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.2f, 0.231f, 1.00f);

    /* List Box */
    colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.231f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.231f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.2f, 0.2f, 0.231f, 1.00f);

    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.862f, 0.862f, 0.862f, .90f);
    // colors[ImGuiCol_CloseButton]          = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);
    // colors[ImGuiCol_CloseButtonHovered]   = ImVec4(0.40f, 0.00f, 0.00f, 1.00f);
    // colors[ImGuiCol_CloseButtonActive]    = ImVec4(0.70f, 0.20f, 0.00f, 0.83f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    // colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 0.54f, 0.00f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.54f, 0.00f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.54f, 0.00f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.50f, 0.00f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 0.00f, 0.52f);
    colors[ImGuiCol_Separator] = ImVec4(0.00f, 1.00f, 0.00f, 0.24f);

    style.Alpha = 1.f;
    style.WindowPadding = ImVec2(8, 8);
    style.WindowMinSize = ImVec2(32, 32);
    style.WindowRounding = 0.0f; // 4.0 for slight curve
    style.WindowTitleAlign = ImVec2(0.5f, 1.5f);
    style.ChildRounding = 0.0f;
    style.FramePadding = ImVec2(4, 3);
    style.FrameRounding = 0.0f; // 2.0
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 3.0f;
    style.ScrollbarSize = 6.0f;
    style.ScrollbarRounding = 16.0f; // 16.0
    style.GrabMinSize = 0.1f;
    style.GrabRounding = 16.0f; // 16.0
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.DisplayWindowPadding = ImVec2(22, 22);
    style.DisplaySafeAreaPadding = ImVec2(4, 4);
    style.AntiAliasedLines = true;
    style.CurveTessellationTol = 1.25f;

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    char szPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, NULL, szPath);

    std::string Path = szPath;
    std::string pTahoma{Path + xorstr_("\\Tahoma.ttf")};
    std::string pArialbd{Path + xorstr_("\\arialbd.ttf")};
    std::string pSegoeui{Path + xorstr_("\\msyhbd.ttc")};

    Font16 = io.Fonts->AddFontFromFileTTF(pTahoma.c_str(), 16.0f);
    Font12 = io.Fonts->AddFontFromFileTTF(pTahoma.c_str(), 12.0f);
    Font14 = io.Fonts->AddFontFromFileTTF(pTahoma.c_str(), 14.0f);
    Font18 = io.Fonts->AddFontFromFileTTF(pTahoma.c_str(), 16.0f);
    Font24 = io.Fonts->AddFontFromFileTTF(pTahoma.c_str(), 24.0f);
    fontname = io.Fonts->AddFontFromFileTTF(pArialbd.c_str(), 14.0f);
    m_pFont = io.Fonts->AddFontFromFileTTF(pSegoeui.c_str(), 14.0f, nullptr,
                                           io.Fonts->GetGlyphRangesDefault());
    LoadTextureFromFile(images::visuals, sizeof(images::visuals), &icons[0]);
    LoadTextureFromFile(images::aimbot, sizeof(images::aimbot), &icons[1]);
    LoadTextureFromFile(images::memory, sizeof(images::memory), &icons[2]);
    LoadTextureFromFile(images::settings, sizeof(images::settings), &icons[3]);
    LoadTextureFromFile(images::logo, sizeof(images::logo), &icons[4]);
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(ms_vD3dDevice, ms_vDeviceContext);
    return S_OK;
    //
    //
}
HWND g_HWND = NULL;
BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd, &lpdwProcessId);
    if (lpdwProcessId == lParam)
    {
        g_HWND = hwnd;
        return FALSE;
    }
    return TRUE;
}
struct handle_data
{
    unsigned long process_id;
    HWND window_handle;
};
BOOL is_main_window(HWND handle)
{
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}
BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
    handle_data &data = *(handle_data *)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);
    if (data.process_id != process_id || !is_main_window(handle))
        return TRUE;
    data.window_handle = handle;
    return FALSE;
}

HWND find_main_window(unsigned long process_id)
{
    handle_data data;
    data.process_id = process_id;
    data.window_handle = 0;
    EnumWindows(enum_windows_callback, (LPARAM)&data);
    return data.window_handle;
}
bool file_Exist(const std::string &name)
{
    ifstream f(name.c_str());
    return f.good();
}

std::wstring s2ws(const std::string &s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t *buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
WNDCLASSEX windowClass;

void SetupWindow()
{
    // CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

    LPCWSTR randomwindow = s2ws(r_encryption::random_string(12)).c_str();

    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = WinProc;
    windowClass.hInstance = GetModuleHandleA(0);
    // windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    // windowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
    windowClass.lpszClassName = randomwindow;

    if (!RegisterClassExW(&windowClass))
    {
        MessageBoxA(0, to_string(GetLastError()).c_str(), 0, 0);
        exit(1);
    }
    int emulatorpid = 0;
#ifndef _EXE_
    emulatorpid = GetCurrentProcessId();
    if (issmartgaga)
        GameWnd = FindWindowA(xorstr_("TitanRenderWindowClass"), xorstr_("SmartGaGa RenderWindow"));
    else
        GameWnd = find_main_window(GetCurrentProcessId());
#else
    if (issmartgaga)
    {
        emulatorpid = PIDManager::GetProcessIdByName(xorstr_(L"projecttitan"));
        GameWnd = FindWindowA(xorstr_("TitanRenderWindowClass"), xorstr_("SmartGaGa RenderWindow"));
    }
    else
    {
        emulatorpid = PIDManager::GetProcessIdByName(xorstr_(L"androidemulator"));
        GameWnd = find_main_window(emulatorpid);
    }
#endif // !_EXE_
    auto emulatorhandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, emulatorpid);
    char ppath[MAX_PATH] = {"\0"};
    DWORD psize = MAX_PATH;
    QueryFullProcessImageNameA(emulatorhandle, 0, ppath, &psize);
    CloseHandle(emulatorhandle);
    emulatorpath = ppath;

    SetForegroundWindow(GameWnd);
    if (GameWnd)
    {
        if (!issmartgaga)
            SubGameWnd = FindWindowExW(GameWnd, 0, xorstr_(L"AEngineRenderWindowClass"), 0);
        else
            SubGameWnd = FindWindowExW(GameWnd, 0, xorstr_(L"TitanOpenglWindowClass"), 0);
        SetForegroundWindow(SubGameWnd);
        GetClientRect(SubGameWnd, &GameRect);
        POINT xy;
        ClientToScreen(SubGameWnd, &xy);
        GameRect.left = xy.x;
        GameRect.top = xy.y;

        Width = GameRect.right;
        Height = GameRect.bottom;
    }
    else
    {
        MessageBoxA(0, xorstr_("Failed: Cant find game window!"), xorstr_("Error"),
                    MB_ICONERROR);
        ExitProcess(1);
    }
    // loadSettings();
    MyWnd = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
                           windowClass.lpszClassName, windowClass.lpszClassName,
                           WS_POPUP, GameRect.left, GameRect.top, Width, Height,
                           NULL, NULL, windowClass.hInstance, NULL);
    MARGINS vMargins{-1};
    DwmExtendFrameIntoClientArea(MyWnd, &vMargins);
    // SetLayeredWindowAttributes(MyWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
    // SetLayeredWindowAttributes(MyWnd, 0, 255, LWA_ALPHA);
    // SetWindowDisplayAffinity(MyWnd, WDA_MONITOR);
    ShowWindow(MyWnd, SW_SHOW);
}
bool speedreload = true;
int instanthitvalue = 10;

static ImVec4 TabSlider = {0.929f, 0.290f, 0.290f, 1.00f};
static ImVec4 TabTitleText = {0, 1.00f, 0.00f, 1.00f};
static ImVec4 RedText = {1.f, 0.0f, 0.0f, .95f};
static ImVec4 ActiveButton = {0.917, 0.223, 0.223, 1.f};
static ImVec4 GreenText = {0.f, 1.0f, 0.0f, .95f};
static ImVec4 Underline = {0.f, 0.749f, 1.f, 1.00f};
static ImVec4 ActiveBut = {0.2f, 0.2f, 0.231f, 1.00f};
static ImVec4 RegButton = {0.137f, 0.137f, 0.160f, 1.00f};

static int iPage = 0;
void render_buttons()
{
    auto style = ImGui::GetStyle();
    if (iPage == 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              style.Colors[ImGuiCol_ButtonActive]);
        if (ImGui::Button(xorstr_("Visuals"), ImVec2(85, 0)))
            iPage = 0;
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    else
    {
        if (ImGui::Button(xorstr_("Visuals"), ImVec2(85, 0)))
            iPage = 0;
    }
    ImGui::SameLine();
    if (iPage == 1)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              style.Colors[ImGuiCol_ButtonActive]);
        if (ImGui::Button(xorstr_("Aimbot"), ImVec2(85, 0)))
            iPage = 1;
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    else
    {
        if (ImGui::Button(xorstr_("Aimbot"), ImVec2(85, 0)))
            iPage = 1;
    }
    ImGui::SameLine();
    if (iPage == 2)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              style.Colors[ImGuiCol_ButtonActive]);
        if (ImGui::Button(xorstr_("Misc"), ImVec2(85, 0)))
            iPage = 2;
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    else
    {
        if (ImGui::Button(xorstr_("Misc"), ImVec2(85, 0)))
            iPage = 2;
    }
    ImGui::SameLine();
    if (iPage == 3)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              style.Colors[ImGuiCol_ButtonActive]);
        if (ImGui::Button(xorstr_("Settings"), ImVec2(85, 0)))
            iPage = 3;
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    else
    {
        if (ImGui::Button(xorstr_("Settings"), ImVec2(85, 0)))
            iPage = 3;
    }
}

void same_line_dummy(int xSpacing)
{
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(xSpacing, 0));
    ImGui::SameLine();
}
void bar()
{
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
}
static inline char *memdup(const char *s)
{
    char *t = (char *)malloc(64);
    memcpy(t, s, 64);
    return t;
}

const char *SelectionType[] = {memdup(xorstr_("Closest FOV"))};
const char *BoneType[] = {memdup(xorstr_("Head")), memdup(xorstr_("Neck")),
                          memdup(xorstr_("Chest")), memdup(xorstr_("Stomach")),
                          memdup(xorstr_("Pelvis"))};
void ClickThrough(bool v)
{
    LONG style = GetWindowLong(MyWnd, GWL_EXSTYLE);

    if (v)
    {
        style |= WS_EX_LAYERED;
        SetWindowLong(MyWnd, GWL_EXSTYLE, style);
    }
    else
    {
        style &= ~WS_EX_LAYERED;
        SetWindowLong(MyWnd, GWL_EXSTYLE, style);
    }
}
DWORD luffyhandaddr = NULL;

bool scanluffyhand = true;

void saveSettings()
{
    FILE *p_stream;
    fopen_s(&p_stream, "settings.ini", "w+");
    fseek(p_stream, 0, SEEK_SET);
    fwrite(&settings::playerbox, sizeof(settings::playerbox), 1, p_stream);
    fwrite(&settings::boxcolor, sizeof(settings::boxcolor), 1, p_stream);
    fwrite(&settings::playerhealth, sizeof(settings::playerhealth), 1, p_stream);
    fwrite(&settings::playerline, sizeof(settings::playerline), 1, p_stream);
    fwrite(&settings::linecolor, sizeof(settings::linecolor), 1, p_stream);
    fwrite(&settings::playerbone, sizeof(settings::playerbone), 1, p_stream);
    fwrite(&settings::bonecolor, sizeof(settings::bonecolor), 1, p_stream);
    fwrite(&settings::botesp, sizeof(settings::botesp), 1, p_stream);
    fwrite(&settings::botcolor, sizeof(settings::botcolor), 1, p_stream);
    fwrite(&settings::itemesp, sizeof(settings::itemesp), 1, p_stream);
    fwrite(&settings::maxdistance, sizeof(settings::maxdistance), 1, p_stream);
    fwrite(&settings::playername, sizeof(settings::playername), 1, p_stream);
    fwrite(&settings::namecolor, sizeof(settings::namecolor), 1, p_stream);
    fwrite(&settings::boxthickness, sizeof(settings::boxthickness), 1, p_stream);
    fwrite(&settings::bonethickness, sizeof(settings::bonethickness), 1,
           p_stream);
    fwrite(&settings::linethickness, sizeof(settings::linethickness), 1,
           p_stream);

    fwrite(&settings::caresp, sizeof(settings::caresp), 1, p_stream);
    fwrite(&settings::carcolor, sizeof(settings::carcolor), 1, p_stream);
    fwrite(&settings::enableaimbot, sizeof(settings::enableaimbot), 1, p_stream);
    fwrite(&settings::fovcircle, sizeof(settings::fovcircle), 1, p_stream);
    fwrite(&settings::circlecolor, sizeof(settings::circlecolor), 1, p_stream);
    fwrite(&settings::aimbotkey, sizeof(settings::aimbotkey), 1, p_stream);
    fwrite(&settings::aimtype, sizeof(settings::aimtype), 1, p_stream);
    fwrite(&settings::aimbotfov, sizeof(settings::aimbotfov), 1, p_stream);
    fwrite(&settings::aimbotsmooth, sizeof(settings::aimbotsmooth), 1, p_stream);
    fwrite(&settings::aimbotselection, sizeof(settings::aimbotselection), 1,
           p_stream);
    fwrite(&settings::aimbotbone, sizeof(settings::aimbotbone), 1, p_stream);
    fwrite(&settings::aimKnocked, sizeof(settings::aimKnocked), 1, p_stream);

    fwrite(&settings::instanthit, sizeof(settings::instanthit), 1, p_stream);
    fwrite(&settings::fastcar, sizeof(settings::fastcar), 1, p_stream);
    fwrite(&settings::speedrun, sizeof(settings::speedrun), 1, p_stream);
    fwrite(&settings::forcethirdperson, sizeof(settings::forcethirdperson), 1,
           p_stream);
    fwrite(&settings::flycar, sizeof(settings::flycar), 1, p_stream);
    fwrite(&settings::luffyhand, sizeof(settings::luffyhand), 1, p_stream);
    fwrite(&settings::fastland, sizeof(settings::fastland), 1, p_stream);
    fwrite(&settings::antiscreenshot, sizeof(settings::antiscreenshot), 1,
           p_stream);
    fwrite(&settings::flycarkey, sizeof(settings::flycarkey), 1, p_stream);
    fwrite(&settings::fastcarkey, sizeof(settings::fastcarkey), 1, p_stream);
    fwrite(&settings::speedrunkey, sizeof(settings::speedrunkey), 1, p_stream);
    fwrite(&settings::luffykey, sizeof(settings::luffykey), 1, p_stream);

    fclose(p_stream);
}

void loadSettings()
{
    FILE *p_stream;
    fopen_s(&p_stream, "settings.ini", "r+");
    fseek(p_stream, 0, SEEK_SET);
    fread(&settings::playerbox, sizeof(settings::playerbox), 1, p_stream);
    fread(&settings::boxcolor, sizeof(settings::boxcolor), 1, p_stream);
    fread(&settings::playerhealth, sizeof(settings::playerhealth), 1, p_stream);
    fread(&settings::playerline, sizeof(settings::playerline), 1, p_stream);
    fread(&settings::linecolor, sizeof(settings::linecolor), 1, p_stream);
    fread(&settings::playerbone, sizeof(settings::playerbone), 1, p_stream);
    fread(&settings::bonecolor, sizeof(settings::bonecolor), 1, p_stream);
    fread(&settings::botesp, sizeof(settings::botesp), 1, p_stream);
    fread(&settings::botcolor, sizeof(settings::botcolor), 1, p_stream);
    fread(&settings::itemesp, sizeof(settings::itemesp), 1, p_stream);
    fread(&settings::maxdistance, sizeof(settings::maxdistance), 1, p_stream);
    fread(&settings::playername, sizeof(settings::playername), 1, p_stream);
    fread(&settings::namecolor, sizeof(settings::namecolor), 1, p_stream);
    fread(&settings::boxthickness, sizeof(settings::boxthickness), 1, p_stream);
    fread(&settings::bonethickness, sizeof(settings::bonethickness), 1, p_stream);
    fread(&settings::linethickness, sizeof(settings::linethickness), 1, p_stream);
    fread(&settings::caresp, sizeof(settings::caresp), 1, p_stream);
    fread(&settings::carcolor, sizeof(settings::carcolor), 1, p_stream);

    fread(&settings::enableaimbot, sizeof(settings::enableaimbot), 1, p_stream);
    fread(&settings::fovcircle, sizeof(settings::fovcircle), 1, p_stream);
    fread(&settings::circlecolor, sizeof(settings::circlecolor), 1, p_stream);
    fread(&settings::aimbotkey, sizeof(settings::aimbotkey), 1, p_stream);
    fread(&settings::aimtype, sizeof(settings::aimtype), 1, p_stream);
    fread(&settings::aimbotfov, sizeof(settings::aimbotfov), 1, p_stream);
    fread(&settings::aimbotsmooth, sizeof(settings::aimbotsmooth), 1, p_stream);
    fread(&settings::aimbotselection, sizeof(settings::aimbotselection), 1,
          p_stream);
    fread(&settings::aimbotbone, sizeof(settings::aimbotbone), 1, p_stream);
    fread(&settings::aimKnocked, sizeof(settings::aimKnocked), 1, p_stream);

    fread(&settings::instanthit, sizeof(settings::instanthit), 1, p_stream);
    fread(&settings::fastcar, sizeof(settings::fastcar), 1, p_stream);
    fread(&settings::speedrun, sizeof(settings::speedrun), 1, p_stream);
    fread(&settings::forcethirdperson, sizeof(settings::forcethirdperson), 1,
          p_stream);
    fread(&settings::flycar, sizeof(settings::flycar), 1, p_stream);
    fread(&settings::luffyhand, sizeof(settings::luffyhand), 1, p_stream);
    fread(&settings::fastland, sizeof(settings::fastland), 1, p_stream);
    fread(&settings::antiscreenshot, sizeof(settings::antiscreenshot), 1,
          p_stream);
    fread(&settings::flycarkey, sizeof(settings::flycarkey), 1, p_stream);
    fread(&settings::fastcarkey, sizeof(settings::fastcarkey), 1, p_stream);
    fread(&settings::speedrunkey, sizeof(settings::speedrunkey), 1, p_stream);
    fread(&settings::luffykey, sizeof(settings::luffykey), 1, p_stream);

    fclose(p_stream);
}

struct iteminfo
{
    std::string title{};
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    bool enabled = true;
};
struct carinfo
{
    std::string title{};
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    bool enabled = true;
};
struct fsettings
{
    bool playerbox = true;
    bool playerline = true;
    bool playerhealth = true;
    bool playerbone = true;
    bool playerteam = true;
    bool playerweapon = true;
    ImColor playerboxcolor = {1.0f, 1.0f, 1.0f, 1.0f};
    ImColor playerlinecolor = {1.0f, 1.0f, 1.0f, 1.0f};
    ImColor playerhealthcolor = {1.0f, 1.0f, 1.0f, 1.0f};
    ImColor playerbonecolor = {1.0f, 1.0f, 1.0f, 1.0f};
    ImColor playerteamcolor = {1.0f, 1.0f, 1.0f, 1.0f};
    ImColor playerweaponcolor = {1.0f, 1.0f, 1.0f, 1.0f};

    std::map<int, iteminfo> items;
    std::unordered_map<uint8_t, carinfo> vehicles;

    static fsettings getdefault()
    {
        fsettings s;
        s.vehicles = {
            {1, {xorstr_("Motorcycle"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {2, {xorstr_("Sidecar Motorcycle"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {3, {xorstr_("Dacia"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {4, {xorstr_("Mini Bus"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {5, {xorstr_("Pickup (Open Top)"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {6, {xorstr_("Pickup (Closed Top)"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {7, {xorstr_("Buggy"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {8, {xorstr_("UAZ"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {9, {xorstr_("UAZ (Closed Top)"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {10, {xorstr_("UAZ (Open Top)"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {11, {xorstr_("PG-117"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {12, {xorstr_("Jet Ski"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            //{13, {xorstr_("UAZ"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {14, {xorstr_("Mirado (Closed Top)"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {15, {xorstr_("Mirado (Open Top)"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {16, {xorstr_("Rony"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {17, {xorstr_("Scooter"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {18, {xorstr_("Snowmobile"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {19, {xorstr_("Tukshai"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {20, {xorstr_("Snowbike"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {23, {xorstr_("Helicopter UH-60"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            //{25, {xorstr_("Zima"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            //{29, {xorstr_("Zima"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {33, {xorstr_("Mirado (Gold)"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {35, {xorstr_("Armed Dacia"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {36, {xorstr_("Armed Pickup"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {37, {xorstr_("Armed Buggy"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {38, {xorstr_("Armed UAZ"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {39, {xorstr_("Armed Helicopter UH-60"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {41, {xorstr_("Motor Glider"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            //{42, {xorstr_("Coupe RB"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {24, {xorstr_("BRDM"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {53, {xorstr_("Monster Truck"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {61, {xorstr_("Coupe RB"), {0.660f, 1.0f, 0.0f, 1.0f}}},
            {51, {xorstr_("Zima UAZ"), {1.0f, 0.586f, 0.0f, 1.0f}}},
            {65, {xorstr_("Mountain Bike"), {0.660f, 1.0f, 0.0f, 1.0f}}},
        };
        s.items = {
            {101008, {xorstr_("M762"), {1.0f, 1.0f, 0.0f, 1.0f}, true}},
            {101003, {xorstr_("SCAR-L"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {101004, {xorstr_("M416"), {1.0f, 0.f, 0.f, 1.0f}, true}},
            {101002, {xorstr_("M16A4"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {103003, {xorstr_("AWM"), {1.0f, 0.f, 0.f, 1.0f}, true}},
            {103010, {xorstr_("QBU"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {103009, {xorstr_("SLR"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {103004, {xorstr_("SKS"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {103006, {xorstr_("Mini14"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {103002, {xorstr_("M24"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {103001, {xorstr_("Kar98K"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {103005, {xorstr_("VSS"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {103008, {xorstr_("Win94"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {101009, {xorstr_("Mk47 Mutant"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {101010, {xorstr_("G36C"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {101007, {xorstr_("QBZ"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {101001, {xorstr_("AKM"), {1.0f, 0.f, 0.f, 1.0f}, true}},
            {101005, {xorstr_("GROZA"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {101006, {xorstr_("AUG A3"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {104003, {xorstr_("S12K"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {104004, {xorstr_("DBS"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {104001, {xorstr_("S686"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {104002, {xorstr_("S1897"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {106006, {xorstr_("Sawed-Off"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {102005, {xorstr_("PP-19 Bizon"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {102004, {xorstr_("Thompson SMG"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {102007, {xorstr_("MP5K"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {102002, {xorstr_("UMP45"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {102003, {xorstr_("Vector"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {102001, {xorstr_("UZI"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {106003, {xorstr_("R1895"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {106008, {xorstr_("Skorpion"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {106001, {xorstr_("P92"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {106004, {xorstr_("P18C"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {106005, {xorstr_("R45"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {106002, {xorstr_("P1911"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {106010, {xorstr_("Desert Eagle"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {108003, {xorstr_("Sickle"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {108001, {xorstr_("Machete"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {107001, {xorstr_("Crossbow"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {108002, {xorstr_("Crowbar"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {108004, {xorstr_("Pan"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {103007, {xorstr_("Mk14"), {1.0f, 0.f, 0.f, 1.0f}, true}},
            {302001, {xorstr_("7.62mm"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {305001, {xorstr_(".45 ACP"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {303001, {xorstr_("5.56mm"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {301001, {xorstr_("9mm"), {0.45f, 0.40f, 0.019f, 1.0f}, true}},
            {306001, {xorstr_(".300 Magnum"), {1.0f, 0.f, 0.f, 1.0f}, true}},
            {304001, {xorstr_("12 Gauge"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {307001, {xorstr_("Bolt"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201010, {xorstr_("Flash Hider (AR)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201009, {xorstr_("Compensator (AR)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201004, {xorstr_("Flash Hider (SMG)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {205002, {xorstr_("Tactical Stock (Rifles, SMG)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201012, {xorstr_("Duckbill (Shotguns)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201005, {xorstr_("Flash Hider (Snipers)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201006, {xorstr_("Suppressor (SMG, Pistols)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {205003, {xorstr_("Cheek pad (Snipers)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201001, {xorstr_("Choke (SG)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {205001, {xorstr_("Stock (Micro UZI)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201003, {xorstr_("Compensator (Snipers)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201007, {xorstr_("Suppressor (Snipers)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201011, {xorstr_("Suppressor (AR)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204009, {xorstr_("Extended Quickdraw Mag (Snipers)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204004, {xorstr_("Extended Mag (SMG, Pistols)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204005, {xorstr_("Quickdraw Mag (SMG, Pistols)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204007, {xorstr_("Extended Mag (Snipers)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204008, {xorstr_("Quickdraw Mag (Snipers)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204012, {xorstr_("Quickdraw Mag (AR)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204013, {xorstr_("Extended Quickdraw Mag (AR)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204011, {xorstr_("Extended Mag (AR)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204006, {xorstr_("Extended Quickdraw Mag (SMG, Pistols)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {205004, {xorstr_("Quiver (Crossbow)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {204014, {xorstr_("Bullet Loop (Shotguns, Sniper Rifles)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {203005, {xorstr_("8x Scope"), {1.0f, 0.f, 0.f, 1.0f}, true}},
            {203003, {xorstr_("2x Scope"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {203001, {xorstr_("Red Dot Sight"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {203014, {xorstr_("3x Scope"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {203002, {xorstr_("Holographic Sight"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {203015, {xorstr_("6x Scope"), {1.0f, 0.f, 0.f, 1.0f}, true}},
            {203004, {xorstr_("4x Scope"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {105002, {xorstr_("DP-28"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {105001, {xorstr_("M249"), {1.0f, 0.f, 0.f, 1.0f}, true}},
            {501006, {xorstr_("Backpack (Lv. 3)"), {1.0f, 0.f, 1.0f, 1.0f}, true}},
            {501004, {xorstr_("Backpack (Lv. 1)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {501005, {xorstr_("Backpack (Lv. 2)"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {503002, {xorstr_("Police Vest (Lv. 2)"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {503001, {xorstr_("Police Vest (Lv. 1)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {503003, {xorstr_("Military Vest (Lv. 3)"), {1.0f, 0.f, 1.0f, 1.0f}, true}},
            {502002, {xorstr_("Military Helmet (Lv. 2)"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {502001, {xorstr_("Motorcycle Helmet (Lv. 1)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {502003, {xorstr_("Spetsnaz Helmet (Lv. 3)"), {1.0f, 0.f, 1.0f, 1.0f}, true}},
            {602004, {xorstr_("Frag Grenade"), {1.0f, 0.f, 0.0f, 1.0f}, true}},
            {602002, {xorstr_("Smoke Grenade"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {602003, {xorstr_("Molotov Grenade"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {602005, {xorstr_("Apple"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {601003, {xorstr_("Painkiller"), {0.78f, 0.50f, 0.78f, 1.0f}, true}},
            {601002, {xorstr_("Adrenaline Syringe"), {0.78f, 0.50f, 0.78f, 1.0f}, true}},
            {601001, {xorstr_("Energy Drink"), {0.78f, 0.50f, 0.78f, 1.0f}, true}},
            {601005, {xorstr_("First Aid Kit"), {0.78f, 0.50f, 0.78f, 1.0f}, true}},
            {601004, {xorstr_("Bandages"), {0.78f, 0.50f, 0.78f, 1.0f}, true}},
            {202006, {xorstr_("Thumb Grip"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {202007, {xorstr_("Laser Sight"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {202001, {xorstr_("Angled Foregrip"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {202004, {xorstr_("Light Grip"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {106007, {xorstr_("Flare Gun"), {1.0f, 0.0f, 0.0f, 1.0f}, true}},
            {601006, {xorstr_("Med Kit"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {308001, {xorstr_("Flare"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {501003, {xorstr_("Backpack (Lv. 3)"), {1.0f, 0.f, 1.0f, 1.0f}, true}},
            {501002, {xorstr_("Backpack (Lv. 2)"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {501001, {xorstr_("Backpack (Lv. 1)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {201002, {xorstr_("Compensator (SMG)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {502005, {xorstr_("Military Helmet (Lv. 2)"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {403989, {xorstr_("Ghillie Suit (Snow)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {403045, {xorstr_("Ghillie Suit"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {403187, {xorstr_("Ghillie Suit (Desert)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {403188, {xorstr_("Ghillie Suit (Dark Brown)"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {105010, {xorstr_("MG3 Light Machine Gun"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {3000324, {xorstr_("Shop Token"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {605007, {xorstr_("Riot Shield"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {202002, {xorstr_("Vertical Foregrip"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {102105, {xorstr_("P90"), {0.f, 0.f, 1.0f, 1.0f}, true}},
            {101101, {xorstr_("ASM Abakan AR"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {202005, {xorstr_("Half Grip"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {103100, {xorstr_("Mk12"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {101100, {xorstr_("FAMAS"), {0.f, 1.0f, 0.f, 1.0f}, true}},
            {103011, {xorstr_("Mosin Nagant Sniper Rifle"), {1.0f, 1.0f, 0.f, 1.0f}, true}},
            {104101, {xorstr_("M1014 Shotgun"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {203018, {xorstr_("Canted Sight"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {307099, {xorstr_("40mm Grenade"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {107096, {xorstr_("M79 Sawed-off"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {107097, {xorstr_("M79"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {603001, {xorstr_("Fuel Battery Pack"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {602001, {xorstr_("Stun Grenade"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {107099, {xorstr_("M3E1-A"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {307002, {xorstr_("RPG-7/M3E1-A Ammo"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {602036, {xorstr_("Spike Trap"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {107098, {xorstr_("MGL"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {602026, {xorstr_("Air Strike Beacon"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {105003, {xorstr_("M134"), {1.0f, 0.0f, 0.0f, 1.0f}, true}},
            {603003, {xorstr_("Vehicle Repair Pack"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {602045, {xorstr_("Sticky Bomb"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {107005, {xorstr_("Panzerfaust"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
            {103902, {xorstr_("M24-XT"), {1.0f, 1.0f, 0.0f, 1.0f}, true}},
            {101903, {xorstr_("SCAR-XT"), {0.0f, 1.0f, 0.0f, 1.0f}, true}},
            {101908, {xorstr_("M762-XT"), {1.0f, 1.0f, 0.0f, 1.0f}, true}},
            {101901, {xorstr_("AKM-XT"), {1.0f, 0.0f, 0.0f, 1.0f}, true}},
            {106908, {xorstr_("Vz61-XT"), {0.0f, 0.0f, 1.0f, 1.0f}, true}},
            {102903, {xorstr_("Vector-XT"), {0.0f, 0.0f, 1.0f, 1.0f}, true}},
            {102901, {xorstr_("UZI-XT"), {0.0f, 0.0f, 1.0f, 1.0f}, true}},
            {604007, {xorstr_("Mountain Bike"), {1.0f, 1.0f, 1.0f, 1.0f}, true}},
        };
        return s;
    }
};
fsettings g_settings;
char disablemagic[1];

void render()
{
    VM_START
    /*if (r_userdata::expiry < 1)
    {
        ExitProcess(0);
        disablemagic[19]--;
        return;
    }*/
    //if (r_api::integritycheck1 != 0x35796538)
    //{
    //    ExitProcess(0);
    //    disablemagic[19]--;
    //    return;
    //}
    if (!gamemodules.libue4)
        return;
    VM_END
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ;
    // sprintf(fpsinfo, xorstr_("Overlay FPS: %0.f", ImGui::GetIO().Framerate);
    // DrawStrokeText(30, 44, &Col.red, fpsinfo);
    if (GetAsyncKeyState(VK_HOME) & 1)
    {
        settings::showMenu1 = !settings::showMenu1;
    }

    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        settings::showMenu = !settings::showMenu;
        ClickThrough(!settings::showMenu);
    }
    static const auto cpFlags =
        ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoDragDrop |
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
    /*  auto windowflags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
      static int selectedtab = 0;
      ImGui::GetStyle().WindowRounding = 5.0f;
      ImGui::Begin("test", 0, windowflags);
      ImGui::SetWindowSize(ImVec2(800, 500));
      ImGui::SetCursorPosX(20.0f);
      ImGui::SetCursorPosY(120.0f);
      if (ImGui::TabEx(xorstr_("Visuals"), 0, selectedtab == 1, ImVec2(150, 63)))
      {
          selectedtab = 1;
      }
      ImGui::SetCursorPosX(20.0f);
      ImGui::SetCursorPosY(160.0f);
      if (ImGui::TabEx(xorstr_("Aimbot"), 1, selectedtab == 2, ImVec2(150, 63)))
      {
          selectedtab = 2;
      }
      ImGui::SetCursorPosX(20.0f);
      ImGui::SetCursorPosY(200.0f);
      if (ImGui::TabEx(xorstr_("Memory"), 2, selectedtab == 3, ImVec2(150, 63)))
      {
          selectedtab = 3;
      }
      ImGui::SetCursorPosX(20.0f);
      ImGui::SetCursorPosY(240.0f);
      if (ImGui::TabEx(xorstr_("Settings"), 3, selectedtab == 4, ImVec2(150, 63)))
      {
          selectedtab = 4;
      }
      ImVec2 p = ImGui::GetWindowPos();
      ImColor c = ImColor(32, 114, 247);
      ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + 150, p.y), ImVec2(p.x + 153, p.y + ImGui::GetWindowHeight()), ImColor(255, 140, 0, 255), ImColor(255, 140, 0, 255), ImColor(255, 200, 0, 255), ImColor(255, 200, 0, 255));

      ImGui::GetWindowDrawList()->AddImage(icons[4], ImVec2(p.x + 10, p.y + 20), ImVec2(p.x + 150, p.y + 120));

      ImGui::SetCursorPosX(205.0f);
      ImGui::SetCursorPosY(20);
      ImGui::Text("Player ESP");
      ImGui::SetCursorPosX(200.0f);

      ImGui::BeginChild("test", ImVec2(270, 160));
      ImGui::Spacing();
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Box"), &g_settings.playerbox);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##boxcolor"), &g_settings.playerboxcolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Line"), &g_settings.playerline);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##linecolor"), &g_settings.playerlinecolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Health"), &g_settings.playerhealth);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##healthcolor"), &g_settings.playerhealthcolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Bone"), &g_settings.playerbone);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##bonecolor"), &g_settings.playerbonecolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Team"), &g_settings.playerteam);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##teamcolor"), &g_settings.playerteamcolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Weapon"), &g_settings.playerweapon);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##weaponcolor"), &g_settings.playerweaponcolor.Value.x, cpFlags);
      ImGui::EndChild();


      ImGui::SetCursorPosX(505.0f);
      ImGui::SetCursorPosY(20);
      ImGui::Text("Bot ESP");
      ImGui::SetCursorPosX(500.0f);

      ImGui::BeginChild("##botesp", ImVec2(270, 160));
      ImGui::Spacing();
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Box"), &g_settings.playerbox);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##boxcolor"), &g_settings.playerboxcolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Line"), &g_settings.playerline);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##linecolor"), &g_settings.playerlinecolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Health"), &g_settings.playerhealth);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##healthcolor"), &g_settings.playerhealthcolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Bone"), &g_settings.playerbone);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##bonecolor"), &g_settings.playerbonecolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Team"), &g_settings.playerteam);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##teamcolor"), &g_settings.playerteamcolor.Value.x, cpFlags);
      ImGui::SetCursorPosX(10);
      ImGui::Checkbox(xorstr_("Weapon"), &g_settings.playerweapon);
      ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      ImGui::ColorEdit3(xorstr_("##weaponcolor"), &g_settings.playerweaponcolor.Value.x, cpFlags);
      ImGui::EndChild();


      ImGui::SetCursorPosX(205.0f);
      ImGui::SetCursorPosY(210);
      ImGui::Text("Item ESP");
      ImGui::SetCursorPosX(200.0f);

      ImGui::BeginChild("##itemesp", ImVec2(270, 230));
      ImGui::Spacing();
      for (auto &item : g_settings.items)
      {
          ImGui::SetCursorPosX(10);
          ImGui::Checkbox(item.second.title.c_str(), &item.second.enabled);
          ImGui::SameLine(ImGui::GetWindowWidth() - 30);
          ImGui::ColorEdit3(item.second.title.c_str(), item.second.color, cpFlags);
      }
      ImGui::EndChild();



      ImGui::SetCursorPosX(505.0f);
      ImGui::SetCursorPosY(210);
      ImGui::Text("Vehicle ESP");
      ImGui::SetCursorPosX(500.0f);

      ImGui::BeginChild("##vehicleesp", ImVec2(270, 230));
      ImGui::Spacing();
      for (auto &vehicle : g_settings.vehicles)
      {
          ImGui::SetCursorPosX(10);
          ImGui::Checkbox(vehicle.second.title.c_str(), &vehicle.second.enabled);
          ImGui::SameLine(ImGui::GetWindowWidth() - 30);
          ImGui::ColorEdit3(vehicle.second.title.c_str(), vehicle.second.color, cpFlags);
      }
      ImGui::EndChild();


      ImGui::End();*/
    if (settings::showMenu)
    {
        if (iPage == 0)
        {
            ImGui::SetNextWindowSize(ImVec2(396, 515));
        }
        else if (iPage == 1)
        {
            ImGui::SetNextWindowSize(ImVec2(396, 398));
        }
        else if (iPage == 2)
        {
            ImGui::SetNextWindowSize(ImVec2(396, 350));
        }
        else if (iPage == 3)
        {
            ImGui::SetNextWindowSize(ImVec2(396, 300));
        }
        else
        {
            ImGui::SetNextWindowSize(ImVec2(396, 300));
        }
        static const auto dwFlags =
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoTitleBar;
        static const auto cpFlags =
            ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoDragDrop |
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;

        ImGui::Begin(xorstr_("SX"), 0, dwFlags);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImColor c = ImColor(32, 114, 247);
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(p.x, p.y + 22), ImVec2(p.x + ImGui::GetWindowWidth() - 16, p.y),
            ImColor(30, 30, 39));
        ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
            ImVec2(p.x, p.y + 22),
            ImVec2(p.x + ImGui::GetWindowWidth() - 16, p.y + 24),
            ImColor(57, 234, 57, 255), ImColor(31, 58, 87, 255),
            ImColor(31, 58, 87, 255), ImColor(24, 167, 71, 255));
        ImGui::Spacing();
        ImGui::PushFont(Font18);
        ImGui::PushStyleColor(ImGuiCol_Text, TabTitleText);
        ImGui::SameLine(396 / 2 - 30);
        ImGui::Text(xorstr_("HADES"));
        ImGui::PopStyleColor();
        ImGui::PopFont();
        // Main

        ImGui::SetCursorPosX(8);
        ImGui::SetCursorPosY(32);
        ImGui::BeginChild(xorstr_("Main"), ImVec2(380, 0), true);
        {
            ImGui::PushFont(Font18);
            render_buttons();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();
            if (iPage == 0)
            {
                ImGui::Spacing();

                ImGui::Checkbox(xorstr_("Player Box"), &settings::playerbox);
                ImGui::SameLine(115);
                ImGui::ColorEdit3(xorstr_("##boxcolor"), settings::boxcolor, cpFlags);
                ImGui::SameLine(230);
                ImGui::Checkbox(xorstr_("Player Health"), &settings::playerhealth);
                ImGui::Checkbox(xorstr_("Player Line"), &settings::playerline);
                ImGui::SameLine(115);
                ImGui::ColorEdit3(xorstr_("##linecolor"), settings::linecolor, cpFlags);
                ImGui::SameLine(230);
                ImGui::Checkbox(xorstr_("Player Bone"), &settings::playerbone);
                ImGui::SameLine(340);
                ImGui::ColorEdit3(xorstr_("##bonecolor"), settings::bonecolor, cpFlags);
                ImGui::Checkbox(xorstr_("Bot Esp"), &settings::botesp);
                ImGui::SameLine(115);
                ImGui::ColorEdit3(xorstr_("##botcolor"), settings::botcolor, cpFlags);
                ImGui::SameLine(230);
                ImGui::Checkbox(xorstr_("Item Esp"), &settings::itemesp);
                ImGui::Checkbox(xorstr_("Car Esp"), &settings::caresp);
                ImGui::SameLine(115);
                ImGui::ColorEdit3(xorstr_("##carcolor"), settings::carcolor, cpFlags);
                ImGui::SameLine(230);
                ImGui::Checkbox(xorstr_("Player Name"), &settings::playername);
                ImGui::SameLine(340);
                ImGui::ColorEdit3(xorstr_("##namecolor"), settings::namecolor, cpFlags);
                ImGui::Checkbox(xorstr_("Weapon"), &settings::playerweapon);
                ImGui::SameLine(115);
                ImGui::ColorEdit3(xorstr_("##pweaponcolor"), settings::weaponcolor, cpFlags);
                ImGui::SameLine(230);
                ImGui::Checkbox(xorstr_("Aim Line"), &settings::aimline);
                ImGui::SameLine(340);
                ImGui::ColorEdit3(xorstr_("##aimcolor"), settings::aimlinecolor,
                                  cpFlags);
                ImGui::Checkbox(xorstr_("Player Team"), &settings::playerteam);
                ImGui::SameLine(115);
                ImGui::ColorEdit3(xorstr_("##teamcolor"), settings::playerteamcolor,
                                  cpFlags);
                // ImGui::SameLine(230);
                // ImGui::Checkbox(xorstr_("Network Reset"), &settings::networkreset);
                ImGui::Text(xorstr_("Vehicle: "));
                ImGui::SameLine(100);
                ImGui::RadioButton(xorstr_("Bar"), &settings::vehiclehealthgastype, 0);
                ImGui::SameLine(230);
                ImGui::RadioButton(xorstr_("Text"), &settings::vehiclehealthgastype, 1);
                ImGui::SameLine(340);
                ImGui::ColorEdit3(xorstr_("##vhealthcolor"), settings::vhealthcolor,
                                  cpFlags);
                ImGui::Text(xorstr_("Player: "));
                ImGui::SameLine(100);
                ImGui::RadioButton(xorstr_("Bar##Player"), &settings::playerhealthtype,
                                   0);
                ImGui::SameLine(230);
                ImGui::RadioButton(xorstr_("Text##Player2"),
                                   &settings::playerhealthtype, 1);
                ImGui::SameLine(340);
                ImGui::ColorEdit3(xorstr_("##phealthcolor"), settings::phealthcolor,
                                  cpFlags);
                ImGui::Text(xorstr_("Distance"));
                ImGui::SameLine(85);
                ImGui::SliderFloat(xorstr_("##  #  "), &settings::maxdistance, 0, 500,
                                   xorstr_("%.0f M"));
                ImGui::Text(xorstr_("Box Thick"));
                ImGui::SameLine(85);
                ImGui::SliderFloat(xorstr_("##boxthick"), &settings::boxthickness, 1, 5,
                                   xorstr_("%.1f"));

                ImGui::Text(xorstr_("Bone Thick"));
                ImGui::SameLine(85);
                ImGui::SliderFloat(xorstr_("##bonethick"), &settings::bonethickness,
                                   0.1, 5, xorstr_("%.1f"));

                ImGui::Text(xorstr_("Line Thick"));
                ImGui::SameLine(85);
                ImGui::SliderFloat(xorstr_("##linethick"), &settings::linethickness,
                                   0.1, 5, xorstr_("%.1f"));

                ImGui::Text(xorstr_("Item Font"));
                ImGui::SameLine(85);
                ImGui::SliderFloat(xorstr_("##itemfont"), &settings::itemfontsize, 10,
                                   20, xorstr_("%.1f"));

                ImGui::Text(xorstr_("Vehicle Font"));
                ImGui::SameLine(85);
                ImGui::SliderFloat(xorstr_("##vehiclefont"), &settings::vehiclefontsize,
                                   10, 20, xorstr_("%.1f"));

                ImGui::Text(xorstr_("Enemy Size"));
                ImGui::SameLine(85);
                ImGui::SliderFloat(xorstr_("##enemysize"), &settings::playercountsize,
                                   10, 20, xorstr_("%.1f"));
            }
            if (iPage == 1)
            {
                ImGui::Spacing();
                ImGui::Checkbox(xorstr_("Enable Aimbot"), &settings::enableaimbot);
                same_line_dummy(2);
                ImGui::Checkbox(xorstr_("Aim Knocked"), &settings::aimKnocked);
                same_line_dummy(2);
                ImGui::Checkbox(xorstr_("Fov Circle"), &settings::fovcircle);
                ImGui::SameLine();
                ImGui::ColorEdit3(xorstr_("##fovcolor"), settings::circlecolor,
                                  cpFlags);
                ImGui::Spacing();
                ImGui::Text(xorstr_("Aim Key: "));
                ImGui::SameLine();
                ImGui::HotKey(xorstr_("##aimkey"), &settings::aimbotkey, Font16);
                bar();
                ImGui::RadioButton(xorstr_("Default"), &settings::aimtype, 0);
                ImGui::SameLine(140);
                ImGui::RadioButton(xorstr_("Memory"), &settings::aimtype, 1);
                ImGui::SameLine(270);
                ImGui::RadioButton(xorstr_("Bullet Track"), &settings::aimtype, 2);
                bar();
                ImGui::Checkbox(xorstr_("Prediction"), &settings::aimprediction);
                ImGui::SameLine(140);
                ImGui::Checkbox(xorstr_("Auto Refresh"), &settings::autorefresh);
                ImGui::SameLine(270);
                ImGui::Checkbox(xorstr_("No Recoil"), &settings::norecoil);
                bar();

                ImGui::PushItemWidth(144);
                ImGui::Text(xorstr_(
                    "     Aimbot FOV                            Aimbot Smooth"));
                ImGui::SetCursorPosX(25);
                ImGui::SliderInt(xorstr_("##aimfov"), &settings::aimbotfov, 60, 500);

                ImGui::SameLine();

                ImGui::PushItemWidth(144);
                ImGui::SetCursorPosX(195 + 16);
                ImGui::SliderFloat(xorstr_("##aimsmooth"), &settings::aimbotsmooth, 4,
                                   8, ("%.1f"));

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::PushItemWidth(144);
                ImGui::Text(
                    xorstr_("     Aimbot Selection                     Aimbot Bone"));
                ImGui::SetCursorPosX(25);
                ImGui::Combo(xorstr_("##aimselect"), &settings::aimbotselection,
                             SelectionType, IM_ARRAYSIZE(SelectionType));

                ImGui::SameLine();

                ImGui::PushItemWidth(144);
                ImGui::SetCursorPosX(195 + 16);
                ImGui::Combo(xorstr_("##aimbone"), &settings::aimbotbone, BoneType,
                             IM_ARRAYSIZE(BoneType));
            }
            if (iPage == 2)
            {
                ImGui::Spacing();
                ImGui::PushStyleColor(ImGuiCol_Text, TabTitleText);
                ImGui::Checkbox(xorstr_("Instant Hit"), &settings::instanthit);
                ImGui::SameLine(220);
                ImGui::Checkbox(xorstr_("Instant Hit 2"), &settings::instanthit2);
                ImGui::PopStyleColor();
                ImGui::Checkbox(xorstr_("Force thirdperson"),
                                &settings::forcethirdperson);
                ImGui::SameLine(220);
                if (ImGui::Checkbox(xorstr_("Anti Screen"),
                                    &settings::antiscreenshot))
                {
                    if (settings::antiscreenshot)
                    {
                        SetWindowDisplayAffinity(MyWnd, WDA_EXCLUDEFROMCAPTURE);
                    }
                    else
                    {
                        SetWindowDisplayAffinity(MyWnd, WDA_NONE);
                    }
                }
                if (ImGui::Checkbox(xorstr_("Anti headshot"), &settings::antiheadshot))
                {
                  /*  if (settings::antiheadshot)
                    {
                        writeBytes(gamemodules.libue4 + 0x145A448, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
                        writeBytes(gamemodules.libue4 + 0x1653AD4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
                    }
                    else
                    {
                        writeBytes(gamemodules.libue4 + 0x145A448, 0x70, 0x4C, 0x2D, 0xE9, 0x10, 0xB0, 0x8D, 0xE2);
                        writeBytes(gamemodules.libue4 + 0x1653AD4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
                    }*/
                }

                if (ImGui::Checkbox(xorstr_("Fov"), &settings::ipadview))
                {
                }
                ImGui::SameLine(100);
                ImGui::SliderFloat(xorstr_("##fieldofview"), &settings::ipadviewslider, 90, 160, xorstr_(""));

                ImGui::Checkbox(xorstr_("Fly car"), &settings::flycar);
                ImGui::SameLine(100);
                ImGui::HotKey(xorstr_("##flycarkey"), &settings::flycarkey, Font16);
                ImGui::Checkbox(xorstr_("Fast Car"), &settings::fastcar);
                ImGui::SameLine(100);
                ImGui::HotKey(xorstr_("##fastcarkey"), &settings::fastcarkey, Font16);

                ImGui::Checkbox(xorstr_("Speed run"), &settings::speedrun);
                ImGui::SameLine(100);
                ImGui::HotKey(xorstr_("##speedrunkey"), &settings::speedrunkey, Font16);
                ImGui::Checkbox(xorstr_("Fast Land"), &settings::fastland);
                ImGui::SameLine(100);
                ImGui::HotKey(xorstr_("##fastlandkey"), &settings::fastlandkey, Font16);

                ImGui::Text(xorstr_("Fly car"));
                ImGui::SameLine(100);
                ImGui::SliderFloat(xorstr_("##flycarspeed"), &settings::flycarspeed,
                                   150, 400, xorstr_("%.1f"));
            }
            if (iPage == 3)
            {
                ImGui::Spacing();

                ImGui::Text(xorstr_("Whitelist"));
                ImGui::SameLine(100);
                ImGui::HotKey(xorstr_("##whitelistkey"), &settings::whitelistkey,
                              Font16);

                if (ImGui::Button(xorstr_("Save settings")))
                {
                    saveSettings();
                }
                ImGui::SameLine(ImGui::GetWindowWidth() - 95);
                if (ImGui::Button(xorstr_("Load settings")))
                {
                    loadSettings();
                    if (settings::antiscreenshot)
                    {
                        SetWindowDisplayAffinity(MyWnd, WDA_EXCLUDEFROMCAPTURE);
                    }
                    else
                    {
                        SetWindowDisplayAffinity(MyWnd, WDA_NONE);
                    }
                }
                ImGui::Checkbox(xorstr_("Network Reset"), &settings::networkreset);

                // ImGui::Button(xorstr_("Save settings"));
            }
            // ImGui::PushFont(Font18);
            ImGui::PushStyleColor(ImGuiCol_Text, TabTitleText);
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 25);
            //ImGui::Text(xorstr_("Expire: %s"), r_userdata::expirytime.c_str());
            ImGui::Text(xorstr_("HADES"));
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 65);
            //ImGui::Text(xorstr_("HADES"));
            ImGui::PopStyleColor();
            ImGui::PopFont();
        }
        ImGui::EndChild();
        ImGui::End();
    }

    if (settings::showMenu1)
    {
        auto menubackground = RGBA(0, 0, 0, 200);
        DrawFilledRect(25, Height / 2 - 20.0f, 130, 170, &menubackground);
        // DrawFilledRect(28, Height / 2 + 160, 155, 3, &Col.lightblue);
        // DrawFilledRect(28, Height / 2 + 13, 155, 3, &Col.lightblue);
        DrawBox(25, Height / 2 - 20.0f, 130, 170, 3, &Col.blue);

        DrawStrokeText(70, Height / 2 - 7, &Col.green, xorstr_("HADES"));

        DrawStrokeText(35, Height / 2 + 10, &Col.red, xorstr_("F1"));
        if (settings::enableaimbot && settings::aimtype == 2)
        {
            DrawStrokeText(55, Height / 2 + 10, &Col.green, xorstr_("Magic Bullet"));
        }
        else
        {
            DrawStrokeText(55, Height / 2 + 10, &Col.red, xorstr_("Magic Bullet"));
        }
        DrawStrokeText(35, Height / 2 + 30, &Col.red, xorstr_("F3"));
        if (settings::cartrack)
        {
            DrawStrokeText(55, Height / 2 + 30, &Col.green, xorstr_("Car Track(CAPS)"));
        }
        else
        {
            DrawStrokeText(55, Height / 2 + 30, &Col.red, xorstr_("Car Track(CAPS)"));
        }
        DrawStrokeText(35, Height / 2 + 50, &Col.red, xorstr_("F5"));
        if (settings::itemesp)
        {
            DrawStrokeText(55, Height / 2 + 50, &Col.green, xorstr_("Material"));
        }
        else
        {
            DrawStrokeText(55, Height / 2 + 50, &Col.red, xorstr_("Material"));
        }
        DrawStrokeText(35, Height / 2 + 70, &Col.red, xorstr_("F6"));
        if (settings::aimKnocked)
        {
            DrawStrokeText(55, Height / 2 + 70, &Col.green, xorstr_("Aim Knocked"));
        }
        else
        {
            DrawStrokeText(55, Height / 2 + 70, &Col.red, xorstr_("Aim Knocked"));
        }
        DrawStrokeText(35, Height / 2 + 90, &Col.red, xorstr_("Del"));
        if (settings::combatmode)
        {
            DrawStrokeText(55, Height / 2 + 90, &Col.green, xorstr_("Combat Mode"));
        }
        else
        {
            DrawStrokeText(55, Height / 2 + 90, &Col.red, xorstr_("Combat Mode"));
        }
        DrawStrokeText(35, Height / 2 + 110, &Col.red, xorstr_("F8"));
        if (settings::playerbox)
        {
            DrawStrokeText(55, Height / 2 + 110, &Col.green, xorstr_("Box Esp"));
        }
        else
        {
            DrawStrokeText(55, Height / 2 + 110, &Col.red, xorstr_("Box Esp"));
        }

        DrawStrokeText(35, Height / 2 + 130, &Col.red, xorstr_("F9"));
        if (settings::aimbotbone == 0)
        {
            DrawStrokeText(55, Height / 2 + 130, &Col.red, xorstr_("Aim Pos:Head"));
        }
        else if (settings::aimbotbone == 1)
        {
            DrawStrokeText(55, Height / 2 + 130, &Col.green, xorstr_("Aim Pos:Chest"));
        }

        DrawStrokeText(35, Height / 2 + 150, &Col.yellow, xorstr_("Home Hide | End exit"));
    }

    if (GetAsyncKeyState(VK_F1) & 1)
    {
        if (settings::enableaimbot && settings::aimtype == 2)
        {
            settings::enableaimbot = false;
            settings::aimtype = 0;
        }
        else
        {
            settings::enableaimbot = true;
            settings::aimtype = 2;
        }
    }
    // if (GetAsyncKeyState(VK_F2) & 1)
    //{
    //	settings::flycar = !settings::flycar;
    //}
    // if (GetAsyncKeyState(VK_F3) & 1)
    //{
    //	settings::instanthit = !settings::instanthit;
    //}
    // if (GetAsyncKeyState(VK_F4) & 1)
    //{
    //	settings::fastcar = !settings::fastcar;
    //}
    if (GetAsyncKeyState(VK_F9) & 1)
    {
        if (settings::aimbotbone == 1)
        {
            settings::aimbotbone = 0;
        }
        else
        {
            settings::aimbotbone = 1;
        }
    }
    if (GetAsyncKeyState(VK_F3) & 1)
    {
        settings::cartrack = !settings::cartrack;
    }
    if (GetAsyncKeyState(VK_F5) & 1)
    {
        settings::itemesp = !settings::itemesp;
    }
    if (GetAsyncKeyState(VK_F6) & 1)
    {
        settings::aimKnocked = !settings::aimKnocked;
    }
    if (GetAsyncKeyState(VK_F8) & 1)
    {
        settings::playerbox = !settings::playerbox;
    }
    if (GetAsyncKeyState(VK_DELETE) & 1)
    {
        settings::itemesp = !settings::itemesp;
        settings::caresp = !settings::caresp;
        settings::combatmode = !settings::combatmode;
    }
    if (GetAsyncKeyState(VK_F10) & 1)
        threadcreated = false;
    ESP();
    try
    {
        uint32_t MyWorld = read<uint32_t>(uMyObject + offsets::uObjectWorld);
        auto camviewaddr = gamemodules.libue4 + offsets::CamView;
        if (IsInGame() && LocalPlayerAddr != NULL)
        {
            MUTATE_START
            if (magiciniting)
            {
                DrawStrokeText(Width / 2, Height / 2, &Col.red, xorstr_("Initializing Magic"));
            }
            if (settings::networkreset)
            {
                char resetbuf[128];
                sprintf_s(resetbuf, xorstr_("Resetting network in %d seconds!"), ((lasttick + 210000) - GetTickCount()) / 1000);
                DrawStrokeText(network_reset_pos.x, network_reset_pos.y, &Col.red, resetbuf, 20);
            }
            //printf("%d\n", read<uint8_t>(LocalPlayerAddr + offsets::CurAnimParamList + 0xf3));

            if (settings::fastland && GetAsyncKeyState(VK_SHIFT))
            {
                auto ParachuteState = read<uint8_t>(LocalPlayerAddr + offsets::CurAnimParamList + 0xf3);
                if (ParachuteState)
                {
                    write<float>(read<uint32_t>(LocalPlayerAddr + offsets::ParachuteComponent) + offsets::CurrentFallSpeed, 35000);
                }
            }
            auto Camera = read<uint32_t>(LocalPlayerAddr + offsets::ThirdPersonCameraComponent);

            if (settings::ipadview)
            {
                write<float>(Camera + offsets::FieldOfView, settings::ipadviewslider);
            }

            if (!settings::ipadview && (read<float>(Camera + offsets::FieldOfView) > 91))
            {
                write<float>(Camera + offsets::FieldOfView, 80);
            }
            uint32_t VehicleAnimInstance = read<uint32_t>(MyWorld + offsets::AnimInstance); // AnimInstance* AnimScriptInstance;
            if (VehicleAnimInstance)
            {
                uint32_t WheeledVehicleMovementComponent = read<uint32_t>(VehicleAnimInstance + offsets::WheeledVehicleMovementComponent);
                if (WheeledVehicleMovementComponent)
                {
                    uint32_t ptrCar = read<uint32_t>(read<uint32_t>(WheeledVehicleMovementComponent + 404) + 88);
                    if (ptrCar)
                    {
                        // if (MyStatus == 7)
                        //{
                        // WheeledVehicleMovementComponent*
                        // WheeledVehicleMovementComponent;//[Offset: 0x6f0 , Size: 4]
                        if (settings::fastcar)
                        {
                            for (int x = 0; x <= 8; x++)
                            {
                                if (GetAsyncKeyState(settings::fastcarkey))
                                {
                                    write<float>(ptrCar + 4 * x, 200);
                                }
                                else if (GetAsyncKeyState('S'))
                                {
                                    write<float>(ptrCar + 4 * x, -200);
                                }
                            }
                        }
                        if (settings::flycar)
                        {
                            for (int x = 20; x <= 32; x++)
                            {
                                if (GetKeyState(settings::flycarkey) & 0x8000)
                                {
                                    write<float>(ptrCar + 4 * x, settings::flycarspeed * -1);
                                }
                            }
                        }

                        /*}*/
                    }
                }
            }

            auto CharacterWeaponManager = read<uint32_t>(LocalPlayerAddr + offsets::CharacterWeaponManager);
            if (CharacterWeaponManager)
            {
                // printf("CharacterWeaponManager: %x\n", CharacterWeaponManager);
                auto CurrentWeaponReplicated = read<uint32_t>(CharacterWeaponManager + offsets::CurrentWeaponReplicated);
                if (CurrentWeaponReplicated)
                {
                    // printf("CurrentWeaponReplicated: %x\n", CurrentWeaponReplicated);
                    uint32_t ShootWeaponEntityComp = read<uint32_t>(CurrentWeaponReplicated + offsets::ShootWeaponEntityComp);
                    if (ShootWeaponEntityComp)
                    {
                        // printf("ShootWeaponEntityComp: %x\n", ShootWeaponEntityComp);
                        if (settings::instanthit || settings::instanthit2)
                        {
                            write<float>(ShootWeaponEntityComp + offsets::BulletFireSpeed, instanthitvalue * 99999);
                        }
                        if (settings::norecoil)
                        {
                            write<float>(ShootWeaponEntityComp + offsets::AccessoriesVRecoilFactor, 0); // AccessoriesVRecoilFactor
                        }
                    }
                }
            }
            static bool speedrun2 = false;
            if ((GetAsyncKeyState(settings::speedrunkey) & 1) && settings::speedrun)
            {
                speedrun2 = !speedrun2;
            }
            if (!settings::speedrun)
            {
                speedrun2 = false;
            }
            if (GetAsyncKeyState(settings::luffykey) & 1)
            {
                settings::luffyhand = !settings::luffyhand;
            }
            auto PlayerSpeed = LocalPlayerAddr + offsets::FastRun;
            auto value = read<float>(PlayerSpeed);
            if (value == 3 && speedrun2 == false)
            {
                write<float>(PlayerSpeed, 1);
            }
            else if (value == 1 && speedrun2 == true)
            {
                write<float>(PlayerSpeed, 3);
            }
            if (settings::forcethirdperson)
            {
                uint32_t GameState = read<uint32_t>(uWorlds + offsets::gameState);
                if (GameState)
                {
                    write<uint32_t>(GameState + offsets::IsFPPGameMode, 0);
                }
            }
            MUTATE_END
        }
        else
        {
            luffyhandaddr = NULL;
            scanluffyhand = true;
            speedreload = true;
        }
    }
    catch (...)
    {
    }
    // AimBot();

    ImVec4 vCleanColor{0, 0, 0, 0};
    ImGui::Render();
    ms_vDeviceContext->OMSetRenderTargets(1, &ms_vRenderTargetView, NULL);
    ms_vDeviceContext->ClearRenderTargetView(ms_vRenderTargetView,
                                             (float *)&vCleanColor);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ms_vSwapChain->Present(1, 0);
    // VMProtectEnd();
}

auto CleanupImGuiContext() -> VOID
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
//#include <MinHook.h>

WPARAM MainLoop()
{
    /**/
    //vercheck();
    ZeroMemory(&Message, sizeof(MSG));
    RECT rect;
    while (Message.message != WM_QUIT)
    {
        if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        ZeroMemory(&rect, sizeof(RECT));
        GetWindowRect(SubGameWnd, &rect);
        Width = rect.right - rect.left;
        Height = rect.bottom - rect.top;
        MoveWindow(MyWnd, rect.left, rect.top, Width, Height, true);
        HWND hwnd_active = GetForegroundWindow();

        if (hwnd_active == GameWnd)
        {
            HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
            SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        UpdateWindow(MyWnd);
        render();
        if (GetAsyncKeyState(VK_END) & 1)
            break;
        ;
        DWORD Exitcode;
        GetExitCodeProcess(m_Handle, &Exitcode);
        if (Exitcode != STILL_ACTIVE)
            exit(0);
        Sleep(10);
    }
    if (cachethread != INVALID_HANDLE_VALUE)
        TerminateThread(cachethread, 0);

    if (aimthread != INVALID_HANDLE_VALUE)
        TerminateThread(aimthread, 0);

    if (r_userdata::expirethread != INVALID_HANDLE_VALUE)
        TerminateThread(r_userdata::expirethread, 0);
    if (read<uint64_t>(luffyhandaddr) == 4848124999984742400)
    {
        write<uint64_t>(luffyhandaddr, 4138667321167981973);
    }
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    // MH_RemoveHook(MH_ALL_HOOKS);
    // MH_Uninitialize();
    DestroyWindow(MyWnd);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
    FreeLibraryAndExitThread(currentmodule, 0);

    /**/
    return Message.wParam;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam,
                         LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
        return true;

    switch (Message)
    {
    case WM_DESTROY:
        break;
    case WM_CREATE:
        DwmExtendFrameIntoClientArea(hWnd, &Margin);
        break;
    case WM_SIZE:
        if (ms_vD3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            if (issmartgaga)
            {
                GameWnd = FindWindowA(xorstr_("TitanRenderWindowClass"), xorstr_("SmartGaGa RenderWindow"));
                SubGameWnd = FindWindowExW(GameWnd, 0, xorstr_(L"TitanOpenglWindowClass"), 0);
            }

            CleanupRenderTarget();
            Width = (FLOAT)LOWORD(lParam);
            Height = (FLOAT)HIWORD(lParam);
            ms_vSwapChain->ResizeBuffers(0, (UINT)Width, (UINT)Height,
                                         DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
            return S_OK;
        }
        break;
    default:
        return DefWindowProc(hWnd, Message, wParam, lParam);
        break;
    }
    return 0;
}
int isTopwin()
{
    HWND hWnd = GetForegroundWindow();
    if (hWnd == GameWnd)
        return TopWindowGame;
    if (hWnd == MyWnd)
        return TopWindowMvoe;

    return 0;
}
void create_console()
{
    if (!AllocConsole())
    {
        char buffer[1024] = {0};
        return;
    }

    auto lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    auto hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
    auto fp = _fdopen(hConHandle, xorstr_("w"));

    freopen_s(&fp, xorstr_("CONOUT$"), xorstr_("w"), stdout);

    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);
}

// auto writetoreadonly(DWORD addr, void *buffer,unsigned long length)->void
//{
//
//	DWORD old;
//	VirtualProtectEx(m_Handle, (LPVOID)addr, length,PAGE_READWRITE,&old);
//	Controller->WriteProcessMemory(addr, buffer, length);
//	VirtualProtectEx(m_Handle, (LPVOID)addr, length, old, &old);
//}

int system_no_output(std::string command)
{
    command.insert(0, "/C ");

    SHELLEXECUTEINFOA ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = "cmd.exe";
    ShExecInfo.lpParameters = command.c_str();
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    if (ShellExecuteExA(&ShExecInfo) == FALSE)
        return -1;

    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    DWORD rv;
    GetExitCodeProcess(ShExecInfo.hProcess, &rv);
    CloseHandle(ShExecInfo.hProcess);

    return rv;
}

typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG(NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);
typedef LPVOID(__cdecl *tAEngine_ExecuteShell)(LPVOID a1, LPCWCH lpWideCharStr);

tAEngine_ExecuteShell AEngine_ExecuteShell;

void HookThumb(uint32_t Address, uint32_t shellcodeAddr)
{
    uint8_t shellcode[8] = {0xDF, 0xF8, 0x04, 0xF0, 0x00, 0x00, 0x00, 0x00};
    *(uint32_t *)(shellcode + 4) = (uint32_t)shellcodeAddr;
    DWORD oldprotect;
    NtProtectVirtualMemoryEx(m_Handle, (LPVOID)Address, sizeof(shellcode), PAGE_READWRITE, &oldprotect);
    Controller->WriteProcessMemory(Address, shellcode, 8);
}
//#include <keystone/keystone.h>
// ks_engine* ks;
// void hookaddr(uint32_t addr, uint32_t shelladdr)
//{
//	char temp[64];
//	sprintf_s(temp, xorstr_("movw r4, #0x%04x; movt r4, #0x%04x; bx r4"), (uint16_t)shelladdr, (shelladdr & 0xFFff0000) >> 16);
//	unsigned char* shellcode;
//	size_t size = 0;
//	size_t count = 0;
//	static auto err = ks_open(KS_ARCH_ARM, KS_MODE_ARM, &ks);
//	if (ks_asm(ks, temp, 0, &shellcode, &size, &count) == KS_ERR_OK) {
//		DWORD oldAccess = 0;
//		NtProtectVirtualMemoryEx(m_Handle, (LPVOID)(addr), size, PAGE_EXECUTE_READWRITE, &oldAccess);
//		Controller->WriteProcessMemory(addr, shellcode, size);
//	}
//}
//

auto initantiban() -> void
{
    // antiban
    VM_START
    DWORD libanogs = gamemodules.libanogs;
    DWORD libue4 = gamemodules.libue4;
    DWORD libgcloud = gamemodules.libgcloud;

    writeBytes(libanogs + 0x34230, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x3715B, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59);
    writeBytes(libanogs + 0x37168, 0x59);
    writeBytes(libanogs + 0x3716A, 0x59);
    writeBytes(libanogs + 0x3716E, 0xA8, 0x00, 0xA8, 0x00, 0x59);
    writeBytes(libanogs + 0x37174, 0x59, 0x00, 0x59, 0x00);
    writeBytes(libanogs + 0x3717A, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59);
    writeBytes(libanogs + 0x37184, 0x59, 0x00, 0x59, 0x00, 0x59);
    writeBytes(libanogs + 0x3718A, 0x59, 0x00);
    writeBytes(libanogs + 0x37190, 0xA8, 0x00);
    writeBytes(libanogs + 0x37194, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00);
    writeBytes(libanogs + 0x371A8, 0x59, 0x00);
    writeBytes(libanogs + 0x371AC, 0xA8, 0x00);
    writeBytes(libanogs + 0x371B0, 0xA8, 0x00);
    writeBytes(libanogs + 0x371B4, 0x59, 0x00);
    writeBytes(libanogs + 0x5E9B6, 0xA8, 0x00, 0x00, 0x00);
    writeBytes(libanogs + 0x5E9D4, 0xA8, 0x00, 0x00, 0x00);
    writeBytes(libanogs + 0x5EBC6, 0xA8, 0x00, 0x00, 0x00);
    writeBytes(libanogs + 0x5F912, 0xA8, 0x00, 0x00, 0x00);
    writeBytes(libanogs + 0x69448, 0x00, 0x20, 0x70, 0x47);
    writeBytes(libanogs + 0x6E2AA, 0x00, 0x20, 0x70, 0x47);
    writeBytes(libanogs + 0x841B0, 0x00, 0x20, 0x70, 0x47);
    writeBytes(libanogs + 0x84B98, 0x00, 0x20, 0x70, 0x47);
    writeBytes(libanogs + 0xDE2E0, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xDE314, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xDE348, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xDE5D8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xDF4F8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xE3228, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xE33F8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xE367D, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xE5CB0, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0xE7448, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x17644C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x176568, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x17684C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x178FE8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x17B97C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x17C4DC, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x17C9F4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x25D8D8, 0xA8, 0x00, 0x00, 0x00);
    writeBytes(libanogs + 0x25D900, 0xA8, 0x00, 0x00, 0x00);
    writeBytes(libanogs + 0x279FC2, 0x00, 0x20, 0x70, 0x47);
    writeBytes(libanogs + 0x29AAC8, 0x00, 0x20, 0x70, 0x47);
    writeBytes(libanogs + 0x29AB1C, 0x00, 0x20, 0x70, 0x47);
    writeBytes(libanogs + 0x2A4244, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A43BC, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A43C6, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A4488, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A44E4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A53C0, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A54D0, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A5E1C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A5F04, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A6330, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A6D58, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A6EA4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A823C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A9620, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A9718, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A993C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A9990, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A99EC, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2A9A10, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AA7A0, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AA7E0, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AA854, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AA85C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AA864, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AA860, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AD1D4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AD644, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2ADEFC, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AE0D4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2AE25C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2B67F0, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2B6DF8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2B8800, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2B89E4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2BCAEC, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2C00CC, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2C2CEC, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2C3A10, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2C3C6C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2C6E94, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2C6FB8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2CA7D8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2CD970, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2CF724, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2CFCF4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2D089C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2D52E8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2D5400, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2D5590, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2D5F50, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2D68F8, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2EC6A4, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2ECB2C, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x2ECC00, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);
    writeBytes(libanogs + 0x304354, 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1);

    writeBytes(libue4 + 0xBDDCA0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xC0CF60, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xE944D0, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xEF1600, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xEF451D, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0xEF45B0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xEFD018, 0x18, 0x04, 0x9F, 0xE5, 0x00, 0x00, 0x9F, 0xE7);
    writeBytes(libue4 + 0xEFE9EC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xEFF88C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF043FC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF06FC0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF1FFB4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF50238, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF67B10, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF68894, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF69130, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF6977C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xF838F8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xFBD574, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0xFBD5E8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x101D9C8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x101E31C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x10AFC1C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1124648, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1140139, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x11694D0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1169670, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1169FC4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116A0D0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116A228, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116A5B4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116A6C0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116AA20, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116AD24, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116B0D0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116B2B4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116B8A8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x116BC74, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x118A79C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x118B920, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x118C0CC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x118CBB8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x118E27C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x118F1EC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x118FF78, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1199D3C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x119B494, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x119B52C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x119BB7C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x119BD18, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x119BEB4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x119C1F4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x119C434, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A33F4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A39BC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A3BCC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A3F80, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A4450, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A4908, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A4AD4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A5620, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A5B60, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A5D00, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A5E88, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A6048, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A606C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A6244, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A68E4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A6DA0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A72D4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A74D8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A78C4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A7988, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A7A38, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A7EC8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11A7F28, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11AC7D6, 0x50);
    writeBytes(libue4 + 0x11AC7D8, 0x00, 0x00, 0xA0, 0x03);
    writeBytes(libue4 + 0x11AC7E8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11AC9D4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11ACA68, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11ACBD8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11ACC1C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11ACD6C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11AD6E4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11AD804, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11ADFA0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11AE080, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11AEA78, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11AEC08, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11AF074, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11B5C00, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11B7EB8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x11B8164, 0xF4, 0x86, 0xFF, 0xEA, 0xF0, 0x48, 0x2D, 0xE9);
    writeBytes(libue4 + 0x11B8790, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x122244C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1222524, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12266F0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1238650, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1238E00, 0x88, 0xC7, 0x00);
    writeBytes(libue4 + 0x1238E04, 0x0C, 0x00, 0x40, 0xE0);
    writeBytes(libue4 + 0x12A1D24, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12A37EC, 0x88, 0xC7, 0x00, 0xE3, 0x0C, 0x00, 0x40, 0xE0);
    writeBytes(libue4 + 0x12D3BD0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12D4534, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12E56D0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12ED1F8, 0x70, 0x4F, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12ED3D8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12ED428, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EDB20, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EE180, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EE294, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EE3EC, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EE590, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EE826, 0x2F, 0x06, 0x010, 0xA8, 0x2F, 0x06, 0x70, 0x97);
    writeBytes(libue4 + 0x12EE83C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EEAA4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EEC6C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EF318, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x12EF50C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14724D0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14727A8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x147536C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x147545C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1475534, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14C929C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CA28C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CA49C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CA618, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CAA10, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CABAC, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CAFA4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CB34C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CB7F8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CC764, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CCB40, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CCF30, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CD670, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14CFF1C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D0574, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D08D8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D0F50, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D18A8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D1A44, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D1BBC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D2988, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D3130, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D3254, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D3BA8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D4B48, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D4F28, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D51CC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D5444, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D5694, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D586C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D5978, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D7418, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D78F4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14D8880, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F2B2C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F3A20, 0x70, 0x4C, 0x2D, 0xE9);
    writeBytes(libue4 + 0x14F3CC4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F3F9C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F425C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2, 0x18, 0xD0, 0x4D, 0xE2);
    writeBytes(libue4 + 0x14F43FC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F45B8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F4B68, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F4D84, 0xFF, 0xFF, 0xFF, 0xEA, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F6018, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F61C4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F67E0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F68FC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F6D60, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F78D8, 0xF0, 0x4F, 0x2D, 0xE9);
    writeBytes(libue4 + 0x14F7AA4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F7BC0, 0xFF, 0xFF, 0xFF, 0xEA, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F8AB0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F8CA4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F8E1C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F91B4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F93C0, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F9758, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F9A34, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14F9DCC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14FA2C4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14FA5A0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14FA734, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14FA840, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14FA9C8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14FAFA4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x14FB38C, 0x04, 0xFF, 0xFF, 0xEA);
    writeBytes(libue4 + 0x15151A8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15154A0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1545A58, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15463F8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x154E4D0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15B0334, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15B0B90, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15B76B8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15BA420, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15BAD94, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15BB11C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15C2D00, 0x30, 0x48, 0x2D, 0xE9, 0x20, 0x20, 0x9F, 0xE5);
    writeBytes(libue4 + 0x15C3FEC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15C5510, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15C5954, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15C5B94, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15C9A68, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15DB7EC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15DC02C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15DC22C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15DCDA9, 0x48, 0x2D, 0xE9, 0x0D, 0xB0);
    writeBytes(libue4 + 0x15DCDAF, 0xE1, 0x010, 0xD0, 0x4D, 0xE2);
    writeBytes(libue4 + 0x15DCE34, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15DD888, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15DDC64, 0xFF, 0xFF, 0xFF, 0xEA, 0xF0, 0x4D, 0x2D, 0xE9);
    writeBytes(libue4 + 0x15DE864, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15E0A94, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15E13BC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15E4A78, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15EA320, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15EBE2C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15FA008, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x15FC5C8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1600EF0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16011A0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16012B4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1602EEC, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x160495C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1609D6C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16284C8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1643820, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1644018, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16499C4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1649B20, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x164BD34, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x164FAB4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1656E60, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x165B644, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x165BA40, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x165C0C8, 0xFF, 0xFF, 0xFF, 0xEA, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x165C66C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x165CB18, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16655BC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16663EC, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x166CF0C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x166E0C4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x166F02C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x166F4EC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x166F950, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x168BE7C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x168DE24, 0x03, 0x010, 0xD0, 0xE5, 0x01, 0x010, 0x81, 0xE2);
    writeBytes(libue4 + 0x168DE38, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x168E070, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x168E0E4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x168F2D4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1692B00, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16987FC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x169E2A4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x169E614, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16A20C0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16B6CEC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16DE398, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16DE80C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16DECA8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16DF32C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16DF638, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16DF918, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16DFE88, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x16FC850, 0xEF, 0xF1);
    writeBytes(libue4 + 0x16FC853, 0xEA);
    writeBytes(libue4 + 0x179C134, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x179EE49, 0x010, 0xA0, 0xE1, 0x08, 0x01, 0xD1, 0xE5);
    writeBytes(libue4 + 0x17C8C44, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17C8D5C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17C9084, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17C92CC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17C9838, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17CA424, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17CAF6C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17CB080, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17CB0DC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17CB138, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17D45A4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17D484C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17D53A8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17D9E90, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17DA034, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17DB94C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x17DC678, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x18650E4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x187EC28, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1886204, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x18E0A98, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1936D74, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x193AF40, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x193B138, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x193B4F0, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1957CD8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1959674, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x195C450, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x195C500, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x195C5A8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x195CE3C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x195EA88, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1967BB0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x196B0F4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1971650, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1972CE0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1976740, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1976BA8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19870D0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1987640, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1988848, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1988E70, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x198A3C0, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x198A778, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x198B0D8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1990BA8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199199C, 0x78);
    writeBytes(libue4 + 0x199199E, 0x9F, 0xE5, 0x00, 0x00, 0x9F, 0xE7);
    writeBytes(libue4 + 0x1991B90, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19920F0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1992710, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199295C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1993004, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1993308, 0x8C);
    writeBytes(libue4 + 0x199330A, 0x9F, 0xE5, 0x00, 0x010, 0x9F, 0xE7);
    writeBytes(libue4 + 0x1993BD1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x1993E1C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1994D20, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1997168, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199759C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1997780, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1997B74, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19982BC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1998324, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1998488, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1998AF8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1998B6C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1998D48, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1998D90, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199936C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1999440, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1999A38, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1999B10, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1999C30, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199A2F8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199A538, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199A70C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199A8B4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199A96C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199AA24, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199F69C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199F7C0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199F938, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199FAD4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199FCB0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x199FFF4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A012C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A01D8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0454, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0684, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A08C4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0B08, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0BA0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0C38, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0CC4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0D50, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0DBC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0F58, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A0FC0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A10C8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A1184, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A12BC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A17F1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x19A1824, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A1A70, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A1D04, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A20D0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A23C8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A262C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A2950, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A29E4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A2A50, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A2AA8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A2B5C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A2DF4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A2EFC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A2F94, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A2FF0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A3718, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A3740, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A39DC, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A3AC8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A3CF0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A488C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A49A8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A4A54, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A4D14, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A5054, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19A53DC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AD8A8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AE4AC, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AE958, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AF148, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AF17C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AF260, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AF728, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AFAE0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AFD28, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AFD6C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AFE58, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19AFF40, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B0028, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B0388, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B0478, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B0621, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x19B06B8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B0780, 0x18);
    writeBytes(libue4 + 0x19B0782, 0x90, 0xE5, 0x00, 0x00, 0x50, 0xE3);
    writeBytes(libue4 + 0x19B0844, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B08C8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B094C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B0DD8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B0E4C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B0F94, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B10B0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B128C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B1328, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B15A8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B227C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B27D0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B2C64, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B2DEC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B2FD8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B3100, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B318C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B3518, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B3748, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B39C4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B3DF8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B3F70, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B40E8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B428C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B43B4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B44DC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B47C4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B4C8C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B4D30, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B4DE8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B4E80, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B5B18, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B5BC0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B5C24, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B5CA4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B5D5C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B5F40, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B6078, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B6618, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B6D20, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19B6F24, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19BB068, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19BB1AD, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x19BB264, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19BB374, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19BBCDC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x19BBDA4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7987C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7A028, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7A5F5, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x1A7A720, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7AA1C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7AEA4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7AFD0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7B2D8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7B770, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7BDC0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7C12C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7E2B8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7E890, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7EBC4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7F01C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7F3BC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7F91C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7FCAC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A7FD94, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A80034, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A800B9, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x1A8010C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A803C8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A80494, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A80838, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A80D4C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A81060, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A81470, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A81B24, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A81F2C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A8273C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A8391C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A83FE8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A84D68, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A851A4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A861D0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A86718, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A868E0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1A975F0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1BAD028, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE6074, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE6448, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE6504, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE6C50, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE6F0C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE79E0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE844C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE8E40, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE93F0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1CE94E0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D23E10, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D63190, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D63400, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D63A8C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D63E24, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D641DC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D645D0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D75240, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D7558C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D76268, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1D78390, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DBAAD8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DBC7B8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DBC8FC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DBCD00, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DBCD44, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DBD26C, 0x00, 0x010);
    writeBytes(libue4 + 0x1DBD26F, 0xE3, 0x64, 0x11, 0x80, 0xE5);
    writeBytes(libue4 + 0x1DBD278, 0x00, 0x010);
    writeBytes(libue4 + 0x1DBD27B, 0xE3, 0x64, 0x11, 0x80, 0xE5);
    writeBytes(libue4 + 0x1DBD704, 0xF0, 0x2E, 0xE8, 0xEB);
    writeBytes(libue4 + 0x1DC4AE8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DC5070, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DC51C8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DC5380, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DC59D0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DC5C14, 0x09, 0x60, 0xA0, 0xE1, 0x00, 0x00, 0x9F, 0xE7);
    writeBytes(libue4 + 0x1DC6904, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DC69F8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DC6ABA, 0x00, 0xEA, 0x00, 0x00, 0x00, 0x00);
    writeBytes(libue4 + 0x1DEED24, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DFE3D0, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1DFFB60, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E0001C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E0E778, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E33AD8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E348A0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E45980, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E4A770, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E4C5F8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E4FD64, 0x50, 0x0A, 0x90, 0xE5, 0x00, 0x00, 0x50, 0xE3);
    writeBytes(libue4 + 0x1E50024, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E5087C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E5B4C0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E5B4F0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E5B648, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E5B678, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E5CDE8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E63B28, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E63E64, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E64798, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E6640C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E674F4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E67860, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E68A44, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E69CAC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E6A310, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E6AAFC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E6B580, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E6E2F4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E6F07C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E6FC68, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E71A60, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E7F1F8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E7F4A4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E7F588, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E7F728, 0x01, 0x20, 0xA0);
    writeBytes(libue4 + 0x1E7F72C, 0x00, 0x30, 0xA0, 0xE3);
    writeBytes(libue4 + 0x1E7FAF0, 0x90, 0x010, 0x9F, 0xE5, 0x00, 0x30, 0x00, 0xE3);
    writeBytes(libue4 + 0x1E7FB9C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E7FFC4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E805CC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E80AF0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E814C8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E81E54, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E81FCC, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E821AC, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E898DC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1E8E344, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFA7F8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFA8B4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFA970, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFAA70, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFAAE8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFB4E0, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x1FFBB2C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFC3FC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFC634, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFC818, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x1FFC940, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x1FFC968, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFC9E0, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x1FFCCAC, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x1FFD078, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x1FFD220, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x1FFD25C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFD61C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFDC8C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFEC90, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFEE68, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFEF24, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x1FFF39C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2000C74, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2000D28, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2000F18, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x200120C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2001410, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x200149C, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x20015B0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2001708, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2001930, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2001A80, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2001BD0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2001C8C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2001EC0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2001F68, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2001FE0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20023D8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20024D8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2002744, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2002994, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2002B0C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2002BA0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2002C20, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2003350, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2003B44, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2003BC4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2003D48, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2004300, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20044D0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x200467C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20046F4, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2004708, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2004C64, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2004D04, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2004D18, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20051D8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20052D8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20053D8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x200562C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20058EC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2018A28, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2032CD0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2032D94, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2032E14, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2032ED8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2032F9C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2033060, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x206262C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20627A8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x206D610, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x206D71C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20952A4, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x20952CC, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x20953A4, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x20A365C, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x20A3838, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20A38B0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20A3928, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20A3A30, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20A3AA8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20A3B98, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x20BBB10, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20C4BF4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20E1264, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x20E128C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20E1450, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20E20F4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20E21C4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20E2220, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x20E22D0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2108A54, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2108A90, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x21E7BAC, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x21EFC98, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x22149A4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x22149CC, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x22176A8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x221CBC8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x221D7AC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x221DCAC, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x221DCE8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x221DE60, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x2244A24, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2245594, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x22637CC, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x22F6754, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2319EB0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x231A734, 0xC0);
    writeBytes(libue4 + 0x231A736, 0x40, 0xE2, 0x81, 0xFD, 0xFF, 0xEA);
    writeBytes(libue4 + 0x23673B8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2367FF4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23682C0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23CCAC8, 0x010, 0x20, 0x91, 0xE5, 0x00, 0x00, 0x52, 0xE3);
    writeBytes(libue4 + 0x23D4988, 0x34);
    writeBytes(libue4 + 0x23D498A, 0x9F, 0xE5, 0x00, 0x00, 0x9F, 0xE7);
    writeBytes(libue4 + 0x23D49D1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x23DBE66, 0x50);
    writeBytes(libue4 + 0x23DBE68, 0x00, 0x00, 0x00, 0x0A);
    writeBytes(libue4 + 0x23DC414, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23DC61E, 0x50);
    writeBytes(libue4 + 0x23DC620, 0x00, 0x00, 0x00, 0x0A);
    writeBytes(libue4 + 0x23DC8A0, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23DCA28, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23DCDE6, 0x50);
    writeBytes(libue4 + 0x23DCDE8, 0x00, 0x00, 0x00, 0x0A);
    writeBytes(libue4 + 0x23DD069, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x23DE684, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23DE984, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23E0A14, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23E3CBC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23E3E74, 0x90);
    writeBytes(libue4 + 0x23E3E76, 0x9F, 0xE5, 0x00, 0x00, 0x9F, 0xE7);
    writeBytes(libue4 + 0x23E3F29, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x23E3F58, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23E48E4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23E54C4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23E6D3C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23E70E8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23EA770, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23EAB4C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23EB568, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23EB5DC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23F403C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23F5360, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23F5DD0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23F78EC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23FAA10, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23FAB48, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23FB470, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23FBA80, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23FC984, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x23FD2AC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2400B0C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2401484, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2402154, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x240296C, 0x08);
    writeBytes(libue4 + 0x240296E, 0x9F, 0xE5, 0x00, 0x00, 0x8F, 0xE0);
    writeBytes(libue4 + 0x2402F14, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24032F0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24039BC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2404DE8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2404F40, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2405050, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2405320, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2405420, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2405538, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x240B6D8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x240BA18, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x240C660, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x240EF68, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2410254, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x241041C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2411BA4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2426DAC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2426FE0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2428C38, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2429A34, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2429D48, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242A1C0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242A308, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242A81C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242AA50, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242C6AC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242D5A8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242F11C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242F490, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x242FC4C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2430EBC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24310D4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2432940, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2433AAC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2433FBC, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2434140, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2434620, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2435234, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2437648, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2438560, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2438C24, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243A498, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243A934, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243ADA0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243AF80, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243BFC0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243C5D4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243C8A8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243CD78, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243E98C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243EBE4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243F98C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x243FE94, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24408D8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2441220, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2441E44, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2441F8C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2442D0C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24439EC, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2443A30, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2443DBC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2454870, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24569AC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x245FE50, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2460238, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2463470, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x246356C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24653DC, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2465B4C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24660D0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2466740, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x246B3B4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x246CE3C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2470C30, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2470D04, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2470F9C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2471098, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2471194, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2471BA8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2471E04, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2472D60, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x247306C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2473168, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x247681C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2478D50, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x247B5B4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x247BC6C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x247BD34, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x247BF3C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x247CC38, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x247E8C4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2480CDC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24818AC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2483510, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24838F8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2483D00, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2484A70, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24850F4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2485660, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24860A0, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x248C104, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x248D850, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x248F17C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2492064, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24950CC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24951CC, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24952C8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24953C4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2495498, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2495594, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2495690, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24972B4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24973B0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2497578, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249764C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2497ED4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2498B50, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24992A0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2499548, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249961C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24996E8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24997B4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2499D68, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249A100, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249A3A8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249A47C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249A8FC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249ADB0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249AE70, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249B410, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249B4D0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249BB30, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249BD0C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249C0C4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249D034, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249E4C8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249EDEC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249EEB8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249F168, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249F544, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x249FB0C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A0444, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A0BAC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A0C6C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A0DEC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A0EAC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A0F6C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A102C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A10EC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A3414, 0xF0, 0x4B, 0x2D, 0xE9);
    writeBytes(libue4 + 0x24A3968, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A3C74, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A3F80, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A459C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A4774, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A4870, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A4C0C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A62A0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A639C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24A6654, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24AE2FC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B2150, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B226C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B244C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B291C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B39C8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B3CD4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B3FF8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B481C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B4B40, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B4E74, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24B52C4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BC710, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BCC28, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BE5B8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BE934, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BEE68, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BEF30, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BF298, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BF600, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24BFE10, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C0184, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C0500, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C0D10, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C1070, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C14CC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C15B8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C16AC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C1DCC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C2040, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C213C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C2238, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C2414, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C25DC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C26A8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C277C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C28DC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C3420, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C34E0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24C3720, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24CD660, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24D59A0, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24D91A8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24D9540, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24DABF4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24DBCA4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24DC324, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24E04BC, 0xC8, 0x03, 0x90, 0xE5, 0x00, 0x00, 0x50, 0xE3);
    writeBytes(libue4 + 0x24E04D0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24E4CF8, 0x01, 0x20, 0xA0, 0xE1, 0x00, 0x010, 0xA0);
    writeBytes(libue4 + 0x24E4FEC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24E52E4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24E576C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24E6A50, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24EA234, 0x88);
    writeBytes(libue4 + 0x24EA236, 0x9F, 0xE5, 0x00, 0x00, 0x8F, 0xE0);
    writeBytes(libue4 + 0x24EA2DC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24EA94C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x24EBAE0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2521D50, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2523AF8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2523C48, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2524B88, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25264D8, 0x78, 0x20, 0x90, 0xE5, 0x04, 0x20, 0x82, 0xE3);
    writeBytes(libue4 + 0x2526500, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25280B4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x252B148, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x252C9B8, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x252FC38, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2530408, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x253E930, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x253EB1C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x253F776, 0x9F, 0xE7, 0x00, 0x00, 0xD0, 0xE5);
    writeBytes(libue4 + 0x253F818, 0x88);
    writeBytes(libue4 + 0x253F81A, 0x9F, 0xE5, 0x00, 0x00, 0x9F, 0xE7);
    writeBytes(libue4 + 0x253F8C0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x253FA24, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x253FAA4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x253FB34, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2540158, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25407C8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2540F04, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2541524, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25415F4, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25419F8, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2541B8C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25468C4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2546E84, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2547004, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254719C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2547324, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254749C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2547614, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25477FC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25479D4, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2547C11, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2547D1C, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2548008, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2548275, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2548388, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2548520, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25486E0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25489D4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2548B68, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2548F14, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25492EA, 0x50);
    writeBytes(libue4 + 0x25492EC, 0x00, 0x00, 0x00, 0x0A);
    writeBytes(libue4 + 0x2549380, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25494CC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254961A, 0x50);
    writeBytes(libue4 + 0x254961C, 0x02, 0x00, 0x00, 0x0A);
    writeBytes(libue4 + 0x2549ACC, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254A3E8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254A764, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254AFF0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254B060, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254BFC8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254C0D0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254C274, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254C334, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254DBE0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254EE1C, 0x01, 0x010);
    writeBytes(libue4 + 0x254EE20, 0x00, 0x010, 0x80, 0xE5);
    writeBytes(libue4 + 0x254EEC8, 0x01, 0x20);
    writeBytes(libue4 + 0x254EECC, 0x00, 0x20, 0x80, 0xE5);
    writeBytes(libue4 + 0x254F104, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254F38C, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254FAD8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254FB44, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x254FC94, 0x01, 0xC0);
    writeBytes(libue4 + 0x254FC98, 0x00, 0xC0, 0x80, 0xE5);
    writeBytes(libue4 + 0x254FD40, 0x01, 0xC0);
    writeBytes(libue4 + 0x254FD44, 0x00, 0xC0, 0x80, 0xE5);
    writeBytes(libue4 + 0x254FDEC, 0x01, 0x20);
    writeBytes(libue4 + 0x254FDF0, 0x00, 0x20, 0x80, 0xE5);
    writeBytes(libue4 + 0x254FF44, 0x01, 0xC0);
    writeBytes(libue4 + 0x254FF48, 0x00, 0xC0, 0x80, 0xE5);
    writeBytes(libue4 + 0x2550A2C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2551360, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25517BC, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2551AD1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2552B04, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2552CD8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25532C4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255356C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25538D0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2553CE0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2554E14, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2554F84, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2555690, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25577E9, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255807C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2558CA0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2559330, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2559F34, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255A0E0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255A9B8, 0x00, 0x010, 0xA0, 0xE3);
    writeBytes(libue4 + 0x255A9C2, 0x50);
    writeBytes(libue4 + 0x255A9C4, 0x00, 0x00, 0xA0, 0x03);
    writeBytes(libue4 + 0x255A9CC, 0xBA, 0x16, 0xFE, 0xEA);
    writeBytes(libue4 + 0x255AEB8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255B4F8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255B694, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255BBEC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255BDDC, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255C288, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255CA04, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255CAA4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255CB59, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255CBE9, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255CC79, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255CD21, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255CDB1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255CE40, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255CED9, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255CF69, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255CFF8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255D1B1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255D2A1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255D391, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x255D430, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255D578, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255D8D0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255DCA0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255E4F8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x255FB90, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25619E8, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2561D14, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2562BB5, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2566B78, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256826C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25695E8, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256967C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2569710, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256B0AC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256B958, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256B9D4, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256BBD4, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256BD44, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256CAC0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256CBD0, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256CF6C, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256D324, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256D3F0, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x256E75C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2570DD4, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2570E74, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25710E0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2572020, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25729D1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2572A84, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2572CA0, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25730B4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25736CC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2573B28, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2573CBD, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2573D54, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2574AD0, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2575568, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2575C99, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2575F00, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2577CAC, 0x3D, 0x85, 0xFD, 0xEA, 0x010, 0x4C, 0x2D, 0xE9);
    writeBytes(libue4 + 0x2579034, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2579554, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25798DC, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2579BD1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2579DF5, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x2579E80, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x257A201, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x257A235, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x257A428, 0x010, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x257A570, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x257ACA5, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x257B135, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x257B838, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x257C258, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2588580, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x258CA20, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2590F40, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25914C8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x25DA61A, 0x5A);
    writeBytes(libue4 + 0x25DA61C, 0x63, 0x00, 0x00, 0x0A);
    writeBytes(libue4 + 0x2A93FE8, 0x6C, 0x62, 0x03, 0xEB);
    writeBytes(libue4 + 0x2AD3D50, 0x04, 0x010, 0x9F, 0xE5);
    writeBytes(libue4 + 0x2D9E9A4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x2F15860, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CAB2C, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CB378, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CB4C8, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CB618, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CB768, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CB9DC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CBC8C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CC028, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CC394, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CC6FC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CC9C4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CCD30, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x38CD734, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3943398, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3A4F078, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3A4F538, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3A4F830, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3B54600, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3B54BA8, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3C79B54, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3C7A230, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CAA0D0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC34C4, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC3F0C, 0x30, 0x48, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC4120, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC4930, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC4B7C, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC53B4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC5B30, 0x70, 0x4C, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC5F80, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC630C, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC66AC, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x3CC6EA4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x44147EC, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x46D1478, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x46D17EC, 0xF0, 0x4D, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x46D18E8, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x46D1E84, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x46D2478, 0xF0, 0x48, 0x2D, 0xE9, 0x010, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x46D27EC, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4B417CC, 0x2D, 0xE9, 0xF0, 0x4F, 0xC7, 0xB0, 0x89, 0x46);
    writeBytes(libue4 + 0x4B41F44, 0x2D, 0xE9, 0x30, 0x48, 0x86, 0xB0, 0xDF, 0xF8);
    writeBytes(libue4 + 0x4C6BF7D, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x4CD1889, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x4CD18C1, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x4CD1C65, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x4D22EAD, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x4D22EE5, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x4D245E9, 0x48, 0x2D, 0xE9, 0x0D, 0xB0, 0xA0);
    writeBytes(libue4 + 0x4E283F4, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4EE1A18, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4EE5314, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4EE5CAC, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4EE6618, 0xF0, 0x4B, 0x2D, 0xE9, 0x18, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4EE8A84, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4EE90C0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4EF4E38, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);
    writeBytes(libue4 + 0x4EF55A0, 0xF0, 0x4F, 0x2D, 0xE9, 0x1C, 0xB0, 0x8D, 0xE2);

    VM_END
}

void EraseHeaders(HMODULE hModule)
{
    DWORD dwModule = reinterpret_cast<DWORD>(hModule);

    PIMAGE_DOS_HEADER pImageDOS = (IMAGE_DOS_HEADER *)dwModule;
    PIMAGE_NT_HEADERS32 pImageNTHeader =
        (IMAGE_NT_HEADERS32 *)(dwModule + pImageDOS->e_lfanew);

    DWORD dwOld = 0;
    auto header_size = pImageNTHeader->OptionalHeader.SizeOfHeaders;

    VirtualProtect(reinterpret_cast<void *>(dwModule), header_size,
                   PAGE_EXECUTE_READWRITE, &dwOld);
    RtlSecureZeroMemory(reinterpret_cast<void *>(dwModule), header_size);
    VirtualProtect(reinterpret_cast<void *>(dwModule), header_size, PAGE_NOACCESS,
                   &dwOld);
}
//#include <mhook-lib/mhook.h>
typedef INT(WSAAPI *_getaddrinfo)(PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *);
typedef unsigned long(WINAPI *_RegSetValueExW)(HKEY, LPCWSTR, DWORD, DWORD,
                                               const BYTE *, DWORD);

_getaddrinfo getaddrinfo = (_getaddrinfo)GetProcAddress(
    GetModuleHandle(skCrypt(L"ws2_32")), skCrypt("getaddrinfo"));
_RegSetValueExW pRegSetValueExW = (_RegSetValueExW)GetProcAddress(
    GetModuleHandle(skCrypt(L"Advapi32.dll")), skCrypt("RegSetValueExW"));

INT WSAAPI Hookgetaddrinfo(PCSTR pNodeName, PCSTR pServiceName,
                           const ADDRINFOA *pHints, PADDRINFOA *ppResult)
{
    if (lstrcmpA(pNodeName, xorstr_("intldlgs.qq.com")) == 0)
    {
        return -1;
    }
    return getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
}
unsigned long WINAPI HookRegSetValueExW(HKEY hKey, LPCWSTR lpValueName,
                                        DWORD Reserved, DWORD dwType,
                                        const BYTE *lpData, DWORD cbData)
{
    if (_tcsicmp(lpValueName, xorstr_(L"Version")) == 0)
        return ERROR_SUCCESS;
    return pRegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

void initd3d9();

#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib")
void *originalsendto{};
void *originalsend{};
int __stdcall HookSendTo(SOCKET s, char *buf, int len, int flags,
                         const struct sockaddr *to, int tolen)
{
    if ((len == 26) || (len >= 180) && (len <= 195) || (len >= 600))
    {
        buf[0] = 0x00;
        buf[1] = 0x00;
        buf[2] = 0x00;
        buf[3] = 0x00;
        return -1;
    }
    else
    {
        /*     printf("\nAllowed UDP Request: %d", len);*/
    }

    return static_cast<decltype(&HookSendTo)>(originalsendto)(s, buf, len, flags,
                                                              to, tolen);
}

int __stdcall HookSend(SOCKET s, const char *buf, int len, int flags)
{
    return -1;
}
bool CreateFileFromMemory(const std::string &desired_file_path, const char *address, size_t size)
{
    std::ofstream file_ofstream(desired_file_path.c_str(), std::ios_base::out | std::ios_base::binary);

    if (!file_ofstream.write(address, size))
    {
        file_ofstream.close();
        return false;
    }

    file_ofstream.close();
    return true;
}
//Login

auto initialize(HMODULE mod) -> int
{
    /*xChetoApp.init();
    if (!login())
        exit(69);*/
    VM_START
    STR_ENCRYPT_START

    g_settings = fsettings::getdefault();
    // create_console();
    // Mhook_SetHook((PVOID*)&pRegSetValueExW, HookRegSetValueExW);
    // Mhook_SetHook((PVOID*)&getaddrinfo, Hookgetaddrinfo);
    const char *str = "SeMHDkQcTbAABBGNG2zeQaeL0iFDdBTB";

    HANDLE h;
    std::string clipboardtext = "NOT SET";
    if (!OpenClipboard(NULL))
        MessageBoxA(0, "Failed Cant Open Clipboard", "Error", MB_ICONERROR);

    h = GetClipboardData(CF_TEXT);
    CloseClipboard();
    if (h != NULL)
    {
        clipboardtext = (char *)h;
    }
    currentmodule = mod;
    //if (!r_api::r_login(clipboardtext, str))
    //{
        // MH_Initialize();
        // MH_CreateHook(sendto, &HookSendTo, &originalsendto);
        //// MH_CreateHook(send, &HookSend, &originalsend);
        // MH_EnableHook(MH_ALL_HOOKS);
        //  system_no_output(VMProtectDecryptStringA("adb kill-server"));
        //  system_no_output(xorstr_("adb start-server"));
        if (FAILED(::CoInitialize(NULL)))
            return FALSE;
        hrvoice = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
#ifndef NOLOGIN
#ifdef _EXE_
        char windir[MAX_PATH];
        auto ret = GetWindowsDirectoryA(windir, MAX_PATH);
        auto windows = string(windir);
        auto rng = windows + xorstr_("\\RNG.sys");
        system_no_output(xorstr_("sc stop RNG"));
        system_no_output(xorstr_("sc delete RNG"));
        CreateFileFromMemory(rng, reinterpret_cast<const char *>(rawData), sizeof(rawData));
        system_no_output(xorstr_("sc create RNG type= filesys start= demand binPath=%windir%\\RNG.sys"));
        system_no_output(xorstr_("sc start RNG"));
#endif // !_EXE_

        // system_no_output(VMProtectDecryptStringA("adb -s 127.0.0.1:5555 shell rm
        // -rf /data/data/com.tencent.ig/shared_prefs/*"));
        // system_no_output(VMProtectsDecryptStringA("adb -s 127.0.0.1:5555 shell
        // settings put secure android_id ") + r_encryption::random_string(16));
#endif
        create_console();
        printf(xorstr_("Waiting for game...\n"));
        verify_game();
//#ifndef NOLOGIN
//        fclose(stdout);
//        fclose(stdin);
//        FreeConsole();
//        system_no_output(xorstr_("sc stop RNG"));
//        system_no_output(xorstr_("sc delete RNG"));
//#endif // !NOLOGIN

        SetupWindow();
        DirectXInit(MyWnd);

        // initantiban();

        // system_no_output(xorstr_("sc stop RNG"));
        // system_no_output(xorstr_("sc delete RNG"));
        cachethread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(cache), nullptr, NULL, nullptr);
        aimthread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(AimBot), nullptr, NULL, nullptr);
        while (TRUE)
        {
            MainLoop();
        }
    //}
    delete str;
    ExitProcess(1);
    STR_ENCRYPT_END
    VM_END
    return 0;
}

typedef NTSTATUS(NTAPI *pLdrAddRefDll)(IN ULONG Flags, IN PVOID BaseAddress);
pLdrAddRefDll LdrAddRefDll = (pLdrAddRefDll)GetProcAddress(
    GetModuleHandleA(xorstr_("ntdll.dll")), xorstr_("LdrAddRefDll"));
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    initialize(GetModuleHandleA(0));
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        initd3d9();

        CreateThread(NULL, NULL,
                     reinterpret_cast<LPTHREAD_START_ROUTINE>(initialize),
                     hModule, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

bool load;
extern "C" __declspec(dllexport) int NextHook(int code, WPARAM wParam,
                                              LPARAM lParam)
{
    if (!load)
    {
        LdrAddRefDll(0, (HMODULE)&__ImageBase);
        load = true;
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}

//
// int main()
//{
//	initialize();
//}

#include "d3d9.h"

HRESULT f_IDirect3DDevice9::Present(CONST RECT *pSourceRect,
                                    CONST RECT *pDestRect,
                                    HWND hDestWindowOverride,
                                    CONST RGNDATA *pDirtyRegion)
{
    return f_pD3DDevice->Present(pSourceRect, pDestRect, hDestWindowOverride,
                                 pDirtyRegion);
}
HRESULT f_iD3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType,
                              HWND hFocusWindow, DWORD BehaviorFlags,
                              D3DPRESENT_PARAMETERS *pPresentationParameters,
                              IDirect3DDevice9 **ppReturnedDeviceInterface)
{
    LPDIRECT3DDEVICE9 *temp = ppReturnedDeviceInterface;

    *temp = new f_IDirect3DDevice9(*ppReturnedDeviceInterface,
                                   &ppReturnedDeviceInterface);
    *ppReturnedDeviceInterface = *temp;
    HRESULT hr =
        f_pD3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags,
                             pPresentationParameters, ppReturnedDeviceInterface);
    return hr;
}
HRESULT f_IDirect3DDevice9::Reset(
    D3DPRESENT_PARAMETERS *pPresentationParameters)
{
    HRESULT hr = f_pD3DDevice->Reset(pPresentationParameters);
    return hr;
}
INT WINAPI f_D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR wszName)
{
    return d3d9.D3DPERF_BeginEvent(col, wszName);
}
INT WINAPI f_D3DPERF_EndEvent()
{
    return d3d9.D3DPERF_EndEvent();
}
DWORD WINAPI f_D3DPERF_GetStatus()
{
    return d3d9.D3DPERF_GetStatus();
}
BOOL WINAPI f_D3DPERF_QueryRepeatFrame()
{
    return d3d9.D3DPERF_QueryRepeatFrame();
}
VOID WINAPI f_D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR wszName)
{
    return d3d9.D3DPERF_SetMarker(col, wszName);
}
VOID WINAPI f_D3DPERF_SetOptions(DWORD dwOptions)
{
    return d3d9.D3DPERF_SetOptions(dwOptions);
}
VOID WINAPI f_D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR wszName)
{
    return d3d9.D3DPERF_SetRegion(col, wszName);
}
VOID WINAPIV f_DebugSetLevel()
{
    return d3d9.DebugSetLevel();
}
VOID WINAPIV f_DebugSetMute()
{
    return d3d9.DebugSetMute();
}
INT WINAPI f_Direct3D9EnableMaximizedWindowedModeShim(INT a)
{
    return d3d9.Direct3D9EnableMaximizedWindowedModeShim(a);
}
IDirect3D9 *WINAPI f_Direct3DCreate9(UINT SDKVersion)
{
    return new f_iD3D9(d3d9.Direct3DCreate9(SDKVersion));
}
HRESULT WINAPI f_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
    return d3d9.Direct3DCreate9Ex(SDKVersion, ppD3D);
}
struct IDirect3DShaderValidator9 *WINAPI f_Direct3DShaderValidatorCreate9()
{
    return d3d9.Direct3DShaderValidatorCreate9();
}
VOID WINAPI f_PSGPError(class D3DFE_PROCESSVERTICES *a, enum PSGPERRORID b,
                        unsigned int c)
{
    return d3d9.PSGPError(a, b, c);
}
VOID WINAPI f_PSGPSampleTexture(class D3DFE_PROCESSVERTICES *a, unsigned int b,
                                float (*const c)[4], unsigned int d,
                                float (*const e)[4])
{
    return d3d9.PSGPSampleTexture(a, b, c, d, e);
}

void initd3d9()
{
    char path[MAX_PATH];
    HMODULE hm = NULL;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCSTR)&f_Direct3DCreate9, &hm);
    GetSystemDirectoryA(path, MAX_PATH);
    auto din8DLL = LoadLibraryA(
        std::string(path + std::string(xorstr_("\\dinput8.dll"))).c_str());
    strcat_s(path, xorstr_("\\d3d9.dll"));
    d3d9.dll = LoadLibraryA(path);
    oDirectInput8Create = (tDirectInput8Create)GetProcAddress(
        din8DLL, xorstr_("DirectInput8Create"));
    d3d9.D3DPERF_BeginEvent = (LPD3DPERF_BEGINEVENT)GetProcAddress(
        d3d9.dll, xorstr_("D3DPERF_BeginEvent"));
    d3d9.D3DPERF_EndEvent =
        (LPD3DPERF_ENDEVENT)GetProcAddress(d3d9.dll, xorstr_("D3DPERF_EndEvent"));
    d3d9.D3DPERF_GetStatus = (LPD3DPERF_GETSTATUS)GetProcAddress(
        d3d9.dll, xorstr_("D3DPERF_GetStatus"));
    d3d9.D3DPERF_QueryRepeatFrame = (LPD3DPERF_QUERYREPEATFRAME)GetProcAddress(
        d3d9.dll, xorstr_("D3DPERF_QueryRepeatFrame"));
    d3d9.D3DPERF_SetMarker = (LPD3DPERF_SETMARKER)GetProcAddress(
        d3d9.dll, xorstr_("D3DPERF_SetMarker"));
    d3d9.D3DPERF_SetOptions = (LPD3DPERF_SETOPTIONS)GetProcAddress(
        d3d9.dll, xorstr_("D3DPERF_SetOptions"));
    d3d9.D3DPERF_SetRegion = (LPD3DPERF_SETREGION)GetProcAddress(
        d3d9.dll, xorstr_("D3DPERF_SetRegion"));
    d3d9.DebugSetLevel =
        (LPDEBUGSETLEVEL)GetProcAddress(d3d9.dll, xorstr_("DebugSetLevel"));
    d3d9.DebugSetMute =
        (LPDEBUGSETMUTE)GetProcAddress(d3d9.dll, xorstr_("DebugSetMute"));
    d3d9.Direct3D9EnableMaximizedWindowedModeShim =
        (LPDIRECT3D9ENABLEMAXIMIZEDWINDOWEDMODESHIM)GetProcAddress(
            d3d9.dll, xorstr_("Direct3D9EnableMaximizedWindowedModeShim"));
    d3d9.Direct3DCreate9 =
        (LPDIRECT3DCREATE9)GetProcAddress(d3d9.dll, xorstr_("Direct3DCreate9"));
    d3d9.Direct3DCreate9Ex = (LPDIRECT3DCREATE9EX)GetProcAddress(
        d3d9.dll, xorstr_("Direct3DCreate9Ex"));
    d3d9.Direct3DShaderValidatorCreate9 =
        (LPDIRECT3DSHADERVALIDATORCREATE9)GetProcAddress(
            d3d9.dll, xorstr_("Direct3DShaderValidatorCreate9"));
    d3d9.PSGPError = (LPPSGPERROR)GetProcAddress(d3d9.dll, xorstr_("PSGPError"));
    d3d9.PSGPSampleTexture = (LPPSGPSAMPLETEXTURE)GetProcAddress(
        d3d9.dll, xorstr_("PSGPSampleTexture"));
}

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
                                  REFIID riidltf, LPVOID *ppvOut,
                                  LPUNKNOWN punkOuter)
{
    return oDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}
HRESULT f_IDirect3DDevice9::EndScene()
{
    return f_pD3DDevice->EndScene();
}
ULONG f_iD3D9::AddRef()
{
    return f_pD3D->AddRef();
}

HRESULT f_iD3D9::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    *ppvObj = NULL;
    HRESULT hr = f_pD3D->QueryInterface(riid, ppvObj);
    if (hr == D3D_OK)
        *ppvObj = this;
    return hr;
}

ULONG f_iD3D9::Release()
{
    ULONG count = f_pD3D->Release();
    if (count == 0)
    {
        delete (this);
    }
    return (count);
}

HRESULT f_iD3D9::EnumAdapterModes(THIS_ UINT Adapter, D3DFORMAT Format,
                                  UINT Mode, D3DDISPLAYMODE *pMode)
{
    return f_pD3D->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

UINT f_iD3D9::GetAdapterCount()
{
    return f_pD3D->GetAdapterCount();
}

HRESULT f_iD3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode)
{
    return f_pD3D->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT f_iD3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags,
                                      D3DADAPTER_IDENTIFIER9 *pIdentifier)
{
    return f_pD3D->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT f_iD3D9::GetAdapterModeCount(THIS_ UINT Adapter, D3DFORMAT Format)
{
    return f_pD3D->GetAdapterModeCount(Adapter, Format);
}

HMONITOR f_iD3D9::GetAdapterMonitor(UINT Adapter)
{
    return f_pD3D->GetAdapterMonitor(Adapter);
}

HRESULT f_iD3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType,
                               D3DCAPS9 *pCaps)
{
    return f_pD3D->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HRESULT f_iD3D9::RegisterSoftwareDevice(void *pInitializeFunction)
{
    return f_pD3D->RegisterSoftwareDevice(pInitializeFunction);
}

HRESULT f_iD3D9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType,
                                        D3DFORMAT AdapterFormat,
                                        D3DFORMAT RenderTargetFormat,
                                        D3DFORMAT DepthStencilFormat)
{
    return f_pD3D->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat,
                                          RenderTargetFormat, DepthStencilFormat);
}

HRESULT f_iD3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType,
                                   D3DFORMAT AdapterFormat, DWORD Usage,
                                   D3DRESOURCETYPE RType,
                                   D3DFORMAT CheckFormat)
{
    return f_pD3D->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage,
                                     RType, CheckFormat);
}

HRESULT f_iD3D9::CheckDeviceMultiSampleType(
    THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat,
    BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD *pQualityLevels)
{
    return f_pD3D->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat,
                                              Windowed, MultiSampleType,
                                              pQualityLevels);
}

HRESULT f_iD3D9::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType,
                                 D3DFORMAT DisplayFormat,
                                 D3DFORMAT BackBufferFormat, BOOL Windowed)
{
    return f_pD3D->CheckDeviceType(Adapter, CheckType, DisplayFormat,
                                   BackBufferFormat, Windowed);
}

HRESULT f_iD3D9::CheckDeviceFormatConversion(THIS_ UINT Adapter,
                                             D3DDEVTYPE DeviceType,
                                             D3DFORMAT SourceFormat,
                                             D3DFORMAT TargetFormat)
{
    return f_pD3D->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat,
                                               TargetFormat);
}

f_IDirect3DDevice9::f_IDirect3DDevice9(LPDIRECT3DDEVICE9 pDevice,
                                       LPDIRECT3DDEVICE9 **ppDevice)
{
    f_pD3DDevice = pDevice;
    *ppDevice = &f_pD3DDevice;
}

ULONG f_IDirect3DDevice9::AddRef()
{
    return f_pD3DDevice->AddRef();
}

HRESULT f_IDirect3DDevice9::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    *ppvObj = NULL;
    HRESULT hr = f_pD3DDevice->QueryInterface(riid, ppvObj);
    if (hr == D3D_OK)
        *ppvObj = this;
    return hr;
}

ULONG f_IDirect3DDevice9::Release()
{
    ULONG count = f_pD3DDevice->Release();
    if (count == 0)
    {
        delete (this);
    }
    return (count);
}

void f_IDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags)
{
    f_pD3DDevice->SetCursorPosition(X, Y, Flags);
}

HRESULT f_IDirect3DDevice9::SetCursorProperties(
    UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap)
{
    return f_pD3DDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

BOOL f_IDirect3DDevice9::ShowCursor(BOOL bShow)
{
    return f_pD3DDevice->ShowCursor(bShow);
}

HRESULT f_IDirect3DDevice9::CreateAdditionalSwapChain(
    D3DPRESENT_PARAMETERS *pPresentationParameters,
    IDirect3DSwapChain9 **ppSwapChain)
{
    return f_pD3DDevice->CreateAdditionalSwapChain(pPresentationParameters,
                                                   ppSwapChain);
}

HRESULT f_IDirect3DDevice9::CreateCubeTexture(
    THIS_ UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format,
    D3DPOOL Pool, IDirect3DCubeTexture9 **ppCubeTexture,
    HANDLE *pSharedHandle)
{
    return f_pD3DDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format,
                                           Pool, ppCubeTexture, pSharedHandle);
}

HRESULT f_IDirect3DDevice9::CreateDepthStencilSurface(
    THIS_ UINT Width, UINT Height, D3DFORMAT Format,
    D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard,
    IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
    return f_pD3DDevice->CreateDepthStencilSurface(
        Width, Height, Format, MultiSample, MultisampleQuality, Discard,
        ppSurface, pSharedHandle);
}

HRESULT f_IDirect3DDevice9::CreateIndexBuffer(
    THIS_ UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
    IDirect3DIndexBuffer9 **ppIndexBuffer, HANDLE *pSharedHandle)
{
    return f_pD3DDevice->CreateIndexBuffer(Length, Usage, Format, Pool,
                                           ppIndexBuffer, pSharedHandle);
}

HRESULT f_IDirect3DDevice9::CreateRenderTarget(
    THIS_ UINT Width, UINT Height, D3DFORMAT Format,
    D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable,
    IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
    return f_pD3DDevice->CreateRenderTarget(Width, Height, Format, MultiSample,
                                            MultisampleQuality, Lockable,
                                            ppSurface, pSharedHandle);
}

HRESULT f_IDirect3DDevice9::CreateTexture(THIS_ UINT Width, UINT Height,
                                          UINT Levels, DWORD Usage,
                                          D3DFORMAT Format, D3DPOOL Pool,
                                          IDirect3DTexture9 **ppTexture,
                                          HANDLE *pSharedHandle)
{
    return f_pD3DDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool,
                                       ppTexture, pSharedHandle);
}

HRESULT f_IDirect3DDevice9::CreateVertexBuffer(
    THIS_ UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool,
    IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle)
{
    return f_pD3DDevice->CreateVertexBuffer(Length, Usage, FVF, Pool,
                                            ppVertexBuffer, pSharedHandle);
}

HRESULT f_IDirect3DDevice9::CreateVolumeTexture(
    THIS_ UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage,
    D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9 **ppVolumeTexture,
    HANDLE *pSharedHandle)
{
    return f_pD3DDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage,
                                             Format, Pool, ppVolumeTexture,
                                             pSharedHandle);
}

HRESULT f_IDirect3DDevice9::BeginStateBlock()
{
    return f_pD3DDevice->BeginStateBlock();
}

HRESULT f_IDirect3DDevice9::CreateStateBlock(THIS_ D3DSTATEBLOCKTYPE Type,
                                             IDirect3DStateBlock9 **ppSB)
{
    return f_pD3DDevice->CreateStateBlock(Type, ppSB);
}

HRESULT f_IDirect3DDevice9::EndStateBlock(THIS_ IDirect3DStateBlock9 **ppSB)
{
    return f_pD3DDevice->EndStateBlock(ppSB);
}

HRESULT f_IDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
    return f_pD3DDevice->GetClipStatus(pClipStatus);
}

HRESULT f_IDirect3DDevice9::GetDisplayMode(THIS_ UINT iSwapChain,
                                           D3DDISPLAYMODE *pMode)
{
    return f_pD3DDevice->GetDisplayMode(iSwapChain, pMode);
}

HRESULT f_IDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State,
                                           DWORD *pValue)
{
    return f_pD3DDevice->GetRenderState(State, pValue);
}

HRESULT f_IDirect3DDevice9::GetRenderTarget(
    THIS_ DWORD RenderTargetIndex, IDirect3DSurface9 **ppRenderTarget)
{
    return f_pD3DDevice->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT f_IDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,
                                         D3DMATRIX *pMatrix)
{
    return f_pD3DDevice->GetTransform(State, pMatrix);
}

HRESULT f_IDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9 *pClipStatus)
{
    return f_pD3DDevice->SetClipStatus(pClipStatus);
}

HRESULT f_IDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State,
                                           DWORD Value)
{
    return f_pD3DDevice->SetRenderState(State, Value);
}

HRESULT f_IDirect3DDevice9::SetRenderTarget(THIS_ DWORD RenderTargetIndex,
                                            IDirect3DSurface9 *pRenderTarget)
{
    return f_pD3DDevice->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT f_IDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,
                                         CONST D3DMATRIX *pMatrix)
{
    return f_pD3DDevice->SetTransform(State, pMatrix);
}

void f_IDirect3DDevice9::GetGammaRamp(THIS_ UINT iSwapChain,
                                      D3DGAMMARAMP *pRamp)
{
    f_pD3DDevice->GetGammaRamp(iSwapChain, pRamp);
}

void f_IDirect3DDevice9::SetGammaRamp(THIS_ UINT iSwapChain, DWORD Flags,
                                      CONST D3DGAMMARAMP *pRamp)
{
    f_pD3DDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
}

HRESULT f_IDirect3DDevice9::DeletePatch(UINT Handle)
{
    return f_pD3DDevice->DeletePatch(Handle);
}

HRESULT f_IDirect3DDevice9::DrawRectPatch(
    UINT Handle, CONST float *pNumSegs,
    CONST D3DRECTPATCH_INFO *pRectPatchInfo)
{
    return f_pD3DDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT f_IDirect3DDevice9::DrawTriPatch(
    UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo)
{
    return f_pD3DDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT f_IDirect3DDevice9::GetIndices(
    THIS_ IDirect3DIndexBuffer9 **ppIndexData)
{
    return f_pD3DDevice->GetIndices(ppIndexData);
}

HRESULT f_IDirect3DDevice9::SetIndices(
    THIS_ IDirect3DIndexBuffer9 *pIndexData)
{
    return f_pD3DDevice->SetIndices(pIndexData);
}

UINT f_IDirect3DDevice9::GetAvailableTextureMem()
{
    return f_pD3DDevice->GetAvailableTextureMem();
}

HRESULT f_IDirect3DDevice9::GetCreationParameters(
    D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
    return f_pD3DDevice->GetCreationParameters(pParameters);
}

HRESULT f_IDirect3DDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
    return f_pD3DDevice->GetDeviceCaps(pCaps);
}

HRESULT f_IDirect3DDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
    return f_pD3DDevice->GetDirect3D(ppD3D9);
}

HRESULT f_IDirect3DDevice9::GetRasterStatus(THIS_ UINT iSwapChain,
                                            D3DRASTER_STATUS *pRasterStatus)
{
    return f_pD3DDevice->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT f_IDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9 *pLight)
{
    return f_pD3DDevice->GetLight(Index, pLight);
}

HRESULT f_IDirect3DDevice9::GetLightEnable(DWORD Index, BOOL *pEnable)
{
    return f_pD3DDevice->GetLightEnable(Index, pEnable);
}

HRESULT f_IDirect3DDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
    return f_pD3DDevice->GetMaterial(pMaterial);
}

HRESULT f_IDirect3DDevice9::LightEnable(DWORD LightIndex, BOOL bEnable)
{
    return f_pD3DDevice->LightEnable(LightIndex, bEnable);
}

HRESULT f_IDirect3DDevice9::SetLight(DWORD Index, CONST D3DLIGHT9 *pLight)
{
    return f_pD3DDevice->SetLight(Index, pLight);
}

HRESULT f_IDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9 *pMaterial)
{
    return f_pD3DDevice->SetMaterial(pMaterial);
}

HRESULT f_IDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,
                                              CONST D3DMATRIX *pMatrix)
{
    return f_pD3DDevice->MultiplyTransform(State, pMatrix);
}

HRESULT f_IDirect3DDevice9::ProcessVertices(
    THIS_ UINT SrcStartIndex, UINT DestIndex, UINT VertexCount,
    IDirect3DVertexBuffer9 *pDestBuffer,
    IDirect3DVertexDeclaration9 *pVertexDecl, DWORD Flags)
{
    return f_pD3DDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount,
                                         pDestBuffer, pVertexDecl, Flags);
}

HRESULT f_IDirect3DDevice9::TestCooperativeLevel()
{
    return f_pD3DDevice->TestCooperativeLevel();
}

HRESULT f_IDirect3DDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
    return f_pD3DDevice->GetCurrentTexturePalette(pPaletteNumber);
}

HRESULT f_IDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber,
                                              PALETTEENTRY *pEntries)
{
    return f_pD3DDevice->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT f_IDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
    return f_pD3DDevice->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT f_IDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber,
                                              CONST PALETTEENTRY *pEntries)
{
    return f_pD3DDevice->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT f_IDirect3DDevice9::CreatePixelShader(
    THIS_ CONST DWORD *pFunction, IDirect3DPixelShader9 **ppShader)
{
    return f_pD3DDevice->CreatePixelShader(pFunction, ppShader);
}

HRESULT f_IDirect3DDevice9::GetPixelShader(
    THIS_ IDirect3DPixelShader9 **ppShader)
{
    return f_pD3DDevice->GetPixelShader(ppShader);
}

HRESULT f_IDirect3DDevice9::SetPixelShader(
    THIS_ IDirect3DPixelShader9 *pShader)
{
    return f_pD3DDevice->SetPixelShader(pShader);
}

// HRESULT f_IDirect3DDevice9::Present(CONST RECT *pSourceRect, CONST RECT
// *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
//{
//   return f_pD3DDevice->Present(pSourceRect, pDestRect, hDestWindowOverride,
//   pDirtyRegion);
//}

HRESULT f_IDirect3DDevice9::DrawIndexedPrimitive(
    THIS_ D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex,
    UINT NumVertices, UINT startIndex, UINT primCount)
{
    return f_pD3DDevice->DrawIndexedPrimitive(Type, BaseVertexIndex,
                                              MinVertexIndex, NumVertices,
                                              startIndex, primCount);
}

HRESULT f_IDirect3DDevice9::DrawIndexedPrimitiveUP(
    D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices,
    UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat,
    CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return f_pD3DDevice->DrawIndexedPrimitiveUP(
        PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData,
        IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT f_IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,
                                          UINT StartVertex,
                                          UINT PrimitiveCount)
{
    return f_pD3DDevice->DrawPrimitive(PrimitiveType, StartVertex,
                                       PrimitiveCount);
}

HRESULT f_IDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,
                                            UINT PrimitiveCount,
                                            CONST void *pVertexStreamZeroData,
                                            UINT VertexStreamZeroStride)
{
    return f_pD3DDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount,
                                         pVertexStreamZeroData,
                                         VertexStreamZeroStride);
}

HRESULT f_IDirect3DDevice9::BeginScene()
{
    return f_pD3DDevice->BeginScene();
}

HRESULT f_IDirect3DDevice9::GetStreamSource(
    THIS_ UINT StreamNumber, IDirect3DVertexBuffer9 **ppStreamData,
    UINT *OffsetInBytes, UINT *pStride)
{
    return f_pD3DDevice->GetStreamSource(StreamNumber, ppStreamData,
                                         OffsetInBytes, pStride);
}

HRESULT f_IDirect3DDevice9::SetStreamSource(THIS_ UINT StreamNumber,
                                            IDirect3DVertexBuffer9 *pStreamData,
                                            UINT OffsetInBytes, UINT Stride)
{
    return f_pD3DDevice->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes,
                                         Stride);
}

HRESULT f_IDirect3DDevice9::GetBackBuffer(THIS_ UINT iSwapChain,
                                          UINT iBackBuffer,
                                          D3DBACKBUFFER_TYPE Type,
                                          IDirect3DSurface9 **ppBackBuffer)
{
    return f_pD3DDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type,
                                       ppBackBuffer);
}

HRESULT f_IDirect3DDevice9::GetDepthStencilSurface(
    IDirect3DSurface9 **ppZStencilSurface)
{
    return f_pD3DDevice->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT f_IDirect3DDevice9::GetTexture(DWORD Stage,
                                       IDirect3DBaseTexture9 **ppTexture)
{
    return f_pD3DDevice->GetTexture(Stage, ppTexture);
}

HRESULT f_IDirect3DDevice9::GetTextureStageState(DWORD Stage,
                                                 D3DTEXTURESTAGESTATETYPE Type,
                                                 DWORD *pValue)
{
    return f_pD3DDevice->GetTextureStageState(Stage, Type, pValue);
}

HRESULT f_IDirect3DDevice9::SetTexture(DWORD Stage,
                                       IDirect3DBaseTexture9 *pTexture)
{
    return f_pD3DDevice->SetTexture(Stage, pTexture);
}

HRESULT f_IDirect3DDevice9::SetTextureStageState(DWORD Stage,
                                                 D3DTEXTURESTAGESTATETYPE Type,
                                                 DWORD Value)
{
    return f_pD3DDevice->SetTextureStageState(Stage, Type, Value);
}

HRESULT f_IDirect3DDevice9::UpdateTexture(
    IDirect3DBaseTexture9 *pSourceTexture,
    IDirect3DBaseTexture9 *pDestinationTexture)
{
    return f_pD3DDevice->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT f_IDirect3DDevice9::ValidateDevice(DWORD *pNumPasses)
{
    return f_pD3DDevice->ValidateDevice(pNumPasses);
}

HRESULT f_IDirect3DDevice9::GetClipPlane(DWORD Index, float *pPlane)
{
    return f_pD3DDevice->GetClipPlane(Index, pPlane);
}

HRESULT f_IDirect3DDevice9::SetClipPlane(DWORD Index, CONST float *pPlane)
{
    return f_pD3DDevice->SetClipPlane(Index, pPlane);
}

HRESULT f_IDirect3DDevice9::Clear(DWORD Count, CONST D3DRECT *pRects,
                                  DWORD Flags, D3DCOLOR Color, float Z,
                                  DWORD Stencil)
{
    return f_pD3DDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT f_IDirect3DDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
    return f_pD3DDevice->GetViewport(pViewport);
}

HRESULT f_IDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9 *pViewport)
{
    return f_pD3DDevice->SetViewport(pViewport);
}

HRESULT f_IDirect3DDevice9::CreateVertexShader(
    THIS_ CONST DWORD *pFunction, IDirect3DVertexShader9 **ppShader)
{
    return f_pD3DDevice->CreateVertexShader(pFunction, ppShader);
}

HRESULT f_IDirect3DDevice9::GetVertexShader(
    THIS_ IDirect3DVertexShader9 **ppShader)
{
    return f_pD3DDevice->GetVertexShader(ppShader);
}

HRESULT f_IDirect3DDevice9::SetVertexShader(
    THIS_ IDirect3DVertexShader9 *pShader)
{
    return f_pD3DDevice->SetVertexShader(pShader);
}

HRESULT f_IDirect3DDevice9::CreateQuery(THIS_ D3DQUERYTYPE Type,
                                        IDirect3DQuery9 **ppQuery)
{
    return f_pD3DDevice->CreateQuery(Type, ppQuery);
}

HRESULT f_IDirect3DDevice9::SetPixelShaderConstantB(THIS_ UINT StartRegister,
                                                    CONST BOOL *pConstantData,
                                                    UINT BoolCount)
{
    return f_pD3DDevice->SetPixelShaderConstantB(StartRegister, pConstantData,
                                                 BoolCount);
}

HRESULT f_IDirect3DDevice9::GetPixelShaderConstantB(THIS_ UINT StartRegister,
                                                    BOOL *pConstantData,
                                                    UINT BoolCount)
{
    return f_pD3DDevice->GetPixelShaderConstantB(StartRegister, pConstantData,
                                                 BoolCount);
}

HRESULT f_IDirect3DDevice9::SetPixelShaderConstantI(THIS_ UINT StartRegister,
                                                    CONST int *pConstantData,
                                                    UINT Vector4iCount)
{
    return f_pD3DDevice->SetPixelShaderConstantI(StartRegister, pConstantData,
                                                 Vector4iCount);
}

HRESULT f_IDirect3DDevice9::GetPixelShaderConstantI(THIS_ UINT StartRegister,
                                                    int *pConstantData,
                                                    UINT Vector4iCount)
{
    return f_pD3DDevice->GetPixelShaderConstantI(StartRegister, pConstantData,
                                                 Vector4iCount);
}

HRESULT f_IDirect3DDevice9::SetPixelShaderConstantF(THIS_ UINT StartRegister,
                                                    CONST float *pConstantData,
                                                    UINT Vector4fCount)
{
    return f_pD3DDevice->SetPixelShaderConstantF(StartRegister, pConstantData,
                                                 Vector4fCount);
}

HRESULT f_IDirect3DDevice9::GetPixelShaderConstantF(THIS_ UINT StartRegister,
                                                    float *pConstantData,
                                                    UINT Vector4fCount)
{
    return f_pD3DDevice->GetPixelShaderConstantF(StartRegister, pConstantData,
                                                 Vector4fCount);
}

HRESULT f_IDirect3DDevice9::SetStreamSourceFreq(THIS_ UINT StreamNumber,
                                                UINT Divider)
{
    return f_pD3DDevice->SetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT f_IDirect3DDevice9::GetStreamSourceFreq(THIS_ UINT StreamNumber,
                                                UINT *Divider)
{
    return f_pD3DDevice->GetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT f_IDirect3DDevice9::SetVertexShaderConstantB(THIS_ UINT StartRegister,
                                                     CONST BOOL *pConstantData,
                                                     UINT BoolCount)
{
    return f_pD3DDevice->SetVertexShaderConstantB(StartRegister, pConstantData,
                                                  BoolCount);
}

HRESULT f_IDirect3DDevice9::GetVertexShaderConstantB(THIS_ UINT StartRegister,
                                                     BOOL *pConstantData,
                                                     UINT BoolCount)
{
    return f_pD3DDevice->GetVertexShaderConstantB(StartRegister, pConstantData,
                                                  BoolCount);
}

HRESULT f_IDirect3DDevice9::SetVertexShaderConstantF(THIS_ UINT StartRegister,
                                                     CONST float *pConstantData,
                                                     UINT Vector4fCount)
{
    return f_pD3DDevice->SetVertexShaderConstantF(StartRegister, pConstantData,
                                                  Vector4fCount);
}

HRESULT f_IDirect3DDevice9::GetVertexShaderConstantF(THIS_ UINT StartRegister,
                                                     float *pConstantData,
                                                     UINT Vector4fCount)
{
    return f_pD3DDevice->GetVertexShaderConstantF(StartRegister, pConstantData,
                                                  Vector4fCount);
}

HRESULT f_IDirect3DDevice9::SetVertexShaderConstantI(THIS_ UINT StartRegister,
                                                     CONST int *pConstantData,
                                                     UINT Vector4iCount)
{
    return f_pD3DDevice->SetVertexShaderConstantI(StartRegister, pConstantData,
                                                  Vector4iCount);
}

HRESULT f_IDirect3DDevice9::GetVertexShaderConstantI(THIS_ UINT StartRegister,
                                                     int *pConstantData,
                                                     UINT Vector4iCount)
{
    return f_pD3DDevice->GetVertexShaderConstantI(StartRegister, pConstantData,
                                                  Vector4iCount);
}

HRESULT f_IDirect3DDevice9::SetFVF(THIS_ DWORD FVF)
{
    return f_pD3DDevice->SetFVF(FVF);
}

HRESULT f_IDirect3DDevice9::GetFVF(THIS_ DWORD *pFVF)
{
    return f_pD3DDevice->GetFVF(pFVF);
}

HRESULT f_IDirect3DDevice9::CreateVertexDeclaration(
    THIS_ CONST D3DVERTEXELEMENT9 *pVertexElements,
    IDirect3DVertexDeclaration9 **ppDecl)
{
    return f_pD3DDevice->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT f_IDirect3DDevice9::SetVertexDeclaration(
    THIS_ IDirect3DVertexDeclaration9 *pDecl)
{
    return f_pD3DDevice->SetVertexDeclaration(pDecl);
}

HRESULT f_IDirect3DDevice9::GetVertexDeclaration(
    THIS_ IDirect3DVertexDeclaration9 **ppDecl)
{
    return f_pD3DDevice->GetVertexDeclaration(ppDecl);
}

HRESULT f_IDirect3DDevice9::SetNPatchMode(THIS_ float nSegments)
{
    return f_pD3DDevice->SetNPatchMode(nSegments);
}

float f_IDirect3DDevice9::GetNPatchMode(THIS)
{
    return f_pD3DDevice->GetNPatchMode();
}

int f_IDirect3DDevice9::GetSoftwareVertexProcessing(THIS)
{
    return f_pD3DDevice->GetSoftwareVertexProcessing();
}

unsigned int f_IDirect3DDevice9::GetNumberOfSwapChains(THIS)
{
    return f_pD3DDevice->GetNumberOfSwapChains();
}

HRESULT f_IDirect3DDevice9::EvictManagedResources(THIS)
{
    return f_pD3DDevice->EvictManagedResources();
}

HRESULT f_IDirect3DDevice9::SetSoftwareVertexProcessing(THIS_ BOOL bSoftware)
{
    return f_pD3DDevice->SetSoftwareVertexProcessing(bSoftware);
}

HRESULT f_IDirect3DDevice9::SetScissorRect(THIS_ CONST RECT *pRect)
{
    return f_pD3DDevice->SetScissorRect(pRect);
}

HRESULT f_IDirect3DDevice9::GetScissorRect(THIS_ RECT *pRect)
{
    return f_pD3DDevice->GetScissorRect(pRect);
}

HRESULT f_IDirect3DDevice9::GetSamplerState(THIS_ DWORD Sampler,
                                            D3DSAMPLERSTATETYPE Type,
                                            DWORD *pValue)
{
    return f_pD3DDevice->GetSamplerState(Sampler, Type, pValue);
}

HRESULT f_IDirect3DDevice9::SetSamplerState(THIS_ DWORD Sampler,
                                            D3DSAMPLERSTATETYPE Type,
                                            DWORD Value)
{
    return f_pD3DDevice->SetSamplerState(Sampler, Type, Value);
}

HRESULT f_IDirect3DDevice9::SetDepthStencilSurface(
    THIS_ IDirect3DSurface9 *pNewZStencil)
{
    return f_pD3DDevice->SetDepthStencilSurface(pNewZStencil);
}

HRESULT f_IDirect3DDevice9::CreateOffscreenPlainSurface(
    THIS_ UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool,
    IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
    return f_pD3DDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool,
                                                     ppSurface, pSharedHandle);
}

HRESULT f_IDirect3DDevice9::ColorFill(THIS_ IDirect3DSurface9 *pSurface,
                                      CONST RECT *pRect, D3DCOLOR color)
{
    return f_pD3DDevice->ColorFill(pSurface, pRect, color);
}

HRESULT f_IDirect3DDevice9::StretchRect(THIS_ IDirect3DSurface9 *pSourceSurface,
                                        CONST RECT *pSourceRect,
                                        IDirect3DSurface9 *pDestSurface,
                                        CONST RECT *pDestRect,
                                        D3DTEXTUREFILTERTYPE Filter)
{
    return f_pD3DDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface,
                                     pDestRect, Filter);
}

HRESULT f_IDirect3DDevice9::GetFrontBufferData(
    THIS_ UINT iSwapChain, IDirect3DSurface9 *pDestSurface)
{
    return f_pD3DDevice->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT f_IDirect3DDevice9::GetRenderTargetData(
    THIS_ IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pDestSurface)
{
    return f_pD3DDevice->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT f_IDirect3DDevice9::UpdateSurface(
    THIS_ IDirect3DSurface9 *pSourceSurface, CONST RECT *pSourceRect,
    IDirect3DSurface9 *pDestinationSurface, CONST POINT *pDestPoint)
{
    return f_pD3DDevice->UpdateSurface(pSourceSurface, pSourceRect,
                                       pDestinationSurface, pDestPoint);
}

HRESULT f_IDirect3DDevice9::SetDialogBoxMode(THIS_ BOOL bEnableDialogs)
{
    return f_pD3DDevice->SetDialogBoxMode(bEnableDialogs);
}

HRESULT f_IDirect3DDevice9::GetSwapChain(THIS_ UINT iSwapChain,
                                         IDirect3DSwapChain9 **pSwapChain)
{
    return f_pD3DDevice->GetSwapChain(iSwapChain, pSwapChain);
}