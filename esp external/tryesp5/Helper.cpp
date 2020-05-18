#include "Helper.h"

DWORD Helper::getProcessIdByName(LPCTSTR ProcessNamePointer)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof PROCESSENTRY32;

	if (Process32First(snap, &entry))
	{
		do
		{
			if (!lstrcmpi(ProcessNamePointer, entry.szExeFile))
			{
				CloseHandle(snap);
				return entry.th32ProcessID;
			}
		} while (Process32Next(snap, &entry));
	}

	CloseHandle(snap);
	std::cout << "Failed to find 'csgo.exe' process";
	exit(1);
}

uintptr_t Helper::getModuleByName(DWORD ProcessId, LPCTSTR ModuleNamePointer)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessId);

	MODULEENTRY32 entry;
	entry.dwSize = sizeof MODULEENTRY32;

	if (Module32First(snap, &entry))
	{
		do
		{
			if (!lstrcmpi(ModuleNamePointer, entry.szModule))
			{
				CloseHandle(snap);
				return (uintptr_t)entry.hModule;
			}
		} while (Module32Next(snap, &entry));
	}

	CloseHandle(snap);
	std::cout << "Failed to find 'client_panorama.dll' module";
	exit(1);
}
