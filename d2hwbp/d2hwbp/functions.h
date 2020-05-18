#pragma once

#include "pch.h"
#include <iostream>
#include <tlhelp32.h>
#include <Psapi.h>

const char SKIP_SYMBOL = '??';
const int MAX_PATTERN_SIZE = 100;


int GetModuleInfo(LPCTSTR moduleNamePointer, MODULEENTRY32* entry) {
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());

	entry->dwSize = sizeof MODULEENTRY32;

	if (Module32First(snap, entry))
	{
		do
		{
			if (!lstrcmpi(moduleNamePointer, entry->szModule))
			{
				CloseHandle(snap);
				return (uintptr_t)entry->hModule;
			}
		} while (Module32Next(snap, entry));
	}

	CloseHandle(snap);
	
	std::cout << "Failed to find " << moduleNamePointer << std::endl;
	return 0;
}

unsigned char Parse(char byte) { // some magic values
	if (byte >= '0' && byte <= '9')
		return byte - '0';
	else if (byte >= 'a' && byte <= 'f')
		return byte - 'a' + 10;
	else if (byte >= 'A' && byte <= 'F')
		return byte - 'A' + 10;
	return 0;
}

unsigned char* Parse(int& len, const char* strPattern, unsigned char* skipByteMask) {
	int strPatternLen = strlen(strPattern);
	unsigned char* pattern = new unsigned char[strPatternLen];
	for (int i = 0; i < strPatternLen; i++)
		pattern[i] = 0;
	len = 0;
	for (int i = 0; i < strPatternLen; i += 2) {
		unsigned char code = 0;
		if (strPattern[i] == SKIP_SYMBOL)
			skipByteMask[len] = 1;
		else
			code = Parse(strPattern[i]) * 16 + Parse(strPattern[i + 1]);
		i++;
		pattern[len++] = code;
	}
	return pattern;
}

void* FindPattern(LPCTSTR module, const char* strPattern) {
	int patternLen = 0;
	int j = 0;

	unsigned char* skipByteMask = new unsigned char[MAX_PATTERN_SIZE];
	for (int k = 0; k < MAX_PATTERN_SIZE; k++)
		skipByteMask[k] = 0;
	unsigned char* pattern = Parse(patternLen, strPattern, skipByteMask);
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	MEMORY_BASIC_INFORMATION info;

	MODULEENTRY32 moduleEntry;
	GetModuleInfo(module, &moduleEntry);
	void* pStart = moduleEntry.modBaseAddr;
	void* pFinish = moduleEntry.modBaseAddr + moduleEntry.modBaseSize;

	unsigned char* current = (unsigned char*)pStart;
	for (; current < pFinish && j < patternLen; current++) {
		if (!VirtualQuery((LPCVOID)current, &info, sizeof(info)))
			continue;

		unsigned long long protectMask = PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE | PAGE_EXECUTE_READ;
		if (info.State == MEM_COMMIT && info.Protect & protectMask && !(info.Protect & PAGE_GUARD)) {
			unsigned long long finish = (unsigned long long)pFinish < (unsigned long long)info.BaseAddress + info.RegionSize ?
				(unsigned long long)pFinish : (unsigned long long)info.BaseAddress + info.RegionSize;
			current = (unsigned char*)info.BaseAddress;
			unsigned char* rip = 0;
			for (unsigned long long k = (unsigned long long)info.BaseAddress; k < finish && j < patternLen; k++, current++) {
				if (skipByteMask[j] || pattern[j] == *current) {
					if (j == 0)
						rip = current;
					j++;
				}
				else {
					j = 0;
					if (pattern[0] == *current) {
						rip = current;
						j = 1;
					}
				}

			}
			if (j == patternLen) {
				current = rip;
				break;
			}
		}
		else
			current += sysInfo.dwPageSize;
	}
	delete[] skipByteMask;
	delete[] pattern;
	if (j == patternLen)
		return (void*)current;

	return nullptr;
}