#include "pch.h"
#include "AbstractHook.h"

int AbstractHook::Freeze() {
	THREADENTRY32 entry;
	HANDLE hThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); // check
	if (hThread == INVALID_HANDLE_VALUE)
		return ERROR_ENUM_THREAD_START;

	entry.dwSize = sizeof THREADENTRY32;

	if (!Thread32First(hThread, &entry)) {
		CloseHandle(hThread);
		return ERROR_ENUM_THREAD_START;
	}

	DWORD dwOwnerPID = GetCurrentProcessId();
	do {
		if (entry.th32OwnerProcessID == dwOwnerPID && entry.th32ThreadID != GetCurrentThreadId()) {
			HANDLE openThread = OpenThread(THREAD_ALL_ACCESS, FALSE, entry.th32ThreadID);
			if (openThread != NULL) {
				SuspendThread(openThread);
				CloseHandle(openThread);
			}
		}
	} while (Thread32Next(hThread, &entry));
	CloseHandle(hThread);
	
	return SUCCESS_CODE;
}

int AbstractHook::Unfreeze() {
	THREADENTRY32 entry;
	HANDLE hThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThread == INVALID_HANDLE_VALUE)
		return ERROR_ENUM_THREAD_START;

	entry.dwSize = sizeof THREADENTRY32;

	if (!Thread32First(hThread, &entry)) {
		CloseHandle(hThread);
		return ERROR_ENUM_THREAD_START;
	}

	DWORD dwOwnerPID = GetCurrentProcessId();
	do {
		if (entry.th32OwnerProcessID == dwOwnerPID && entry.th32ThreadID != GetCurrentThreadId()) {
			HANDLE openThread = OpenThread(THREAD_ALL_ACCESS, FALSE, entry.th32ThreadID);
			if (openThread != NULL) {
				ResumeThread(openThread);
				CloseHandle(openThread);
			}
		}
	} while (Thread32Next(hThread, &entry));
	CloseHandle(hThread);
	
	return SUCCESS_CODE;
}