
#include "pch.h"
#include <string.h>
#include <cstring>
#include <stdio.h>

#include "AbstractHook.h"
#include "HwbpHook.h"
#include "Command.h"
#include "functions.h"
#include "dx.h"

#define _CRT_SECURE_NO_DEPRECATE

typedef HRESULT(*VirtualOverloadEndScene)(IDirect3DDevice9* pd3dDevice);
VirtualOverloadEndScene oOverloadEndScene = NULL;

typedef bool(*fDoIncludeScript)(const char*, unsigned long long);

DWORD WINAPI HookThread(HMODULE hModule);
BOOL  HookDevice();
void  Dispose();

void* d3d9Device[119];
extern LPDIRECT3DDEVICE9 pDevice = nullptr;


IDeferredCommands* deferredCommands;
AbstractHook* hook;

bool scriptLoaded = FALSE;

void Dispose() {
	//hook->UnsetHook(oOverload);
	hook->UnsetHook(oOverloadEndScene);
	delete deferredCommands;
	delete hook;
}

LONG OnExceptionHandler(EXCEPTION_POINTERS* exceptionPointers) {
	if (exceptionPointers->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP)
		return EXCEPTION_EXIT_UNWIND;

	for (int i = 0; i < DEBUG_REG_COUNT; i++) {
		if (exceptionPointers->ContextRecord->Rip == (unsigned long long) hook->GetInfo()->GetItem(i)->source) {
			exceptionPointers->ContextRecord->Dr7 &= ~(1ULL << (2 * i));
			exceptionPointers->ContextRecord->Rip = (unsigned long long) hook->GetInfo()->GetItem(i)->destination;
			IDeferredCommand* cmd = new SetD7Command(hook, GetCurrentThreadId(), i);
			deferredCommands->Enqueue(cmd);
			break;
		}
	}
	return EXCEPTION_CONTINUE_EXECUTION;
}

HRESULT APIENTRY EndSceneHook(LPDIRECT3DDEVICE9 o_pDevice) {
	if (!pDevice)
		pDevice = o_pDevice;

	if (!scriptLoaded)
	{
		fDoIncludeScript DoIncludeScript = (fDoIncludeScript)FindPattern(L"client.dll", "40 57 48 81 EC ?? ?? ?? ?? 48 83 3D ?? ?? ?? ?? ?? 48 8B F9 0F 84");

		DoIncludeScript("esp", 0);
		scriptLoaded = TRUE;
	}

	std::cout << "EndScene Hook!\n";

	auto record = hook->GetRecordBySource(oOverloadEndScene);
	VirtualOverloadEndScene pTrampoline = (VirtualOverloadEndScene)record->pTrampoline;
	auto result = pTrampoline(pDevice);

	deferredCommands->Run();

	return result;
}

BOOL HookDevice() {
	deferredCommands = new DeferredCommands();
	hook		     = new HwbpHook();

	VirtualOverloadEndScene pointerEndScene = (VirtualOverloadEndScene)((char*)d3d9Device[42]);
	oOverloadEndScene = pointerEndScene;

	hook->SetExceptionHandler(OnExceptionHandler);
	hook->SetHook((char*)d3d9Device[42], &EndSceneHook); //haha

	return TRUE;
}

DWORD WINAPI HookThread(HMODULE hModule) {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);

	std::cout << "Thread started\n";
	
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		HookDevice();
	}

	Dispose();

	Sleep(3000);
	fclose(fp);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr) 
{
	if (reason == DLL_PROCESS_ATTACH) {
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HookThread, hModule, 0, 0));
	}
	
	return TRUE;
}

