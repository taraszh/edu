#pragma once

#include "Windows.h"
#include <TlHelp32.h>
#include "Hack.h"

//class Memory
//{
//private:
//	HANDLE processHandle;
//
//public:
//	Memory(DWORD processId)
//	{
//		processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
//	}
//
//	~Memory()
//	{
//		CloseHandle(processHandle);
//	}
//
//	template <class someType>
//	someType read(uintptr_t address)
//	{
//		someType buffer;
//		ReadProcessMemory(processHandle, (LPBYTE*)address, &buffer, sizeof buffer, nullptr);
//		return buffer;
//	}
//};

