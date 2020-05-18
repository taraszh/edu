#pragma once

#include <string>
#include <iostream>

#include "Windows.h"
#include <TlHelp32.h>

class Helper
{
public:
	static DWORD getProcessIdByName(LPCTSTR ProcessNamePointer);
	static uintptr_t getModuleByName(DWORD ProcessId, LPCTSTR ModuleNamePointer);
};

