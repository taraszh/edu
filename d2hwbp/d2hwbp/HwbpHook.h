#pragma once
#include "pch.h"

#include "AbstractHook.h"

const int DEBUG_REG_COUNT = 4;

const int HW_EXECUTE = 0;
const int HW_READ = 1;
const int HW_WRITE = 2;
const int HW_ACCESS = 3;

const int HW_LENGTH = 0;

class HwbpHook : public AbstractHook {
public:
	HwbpHook() : AbstractHook() {
		pException = nullptr;
		info = new Vector<HookRecord*>();
		
		for (int i = 0; i < DEBUG_REG_COUNT; i++) {
			auto record = new HookRecord();
			info->PushBack(record);
		}
	}
	
	~HwbpHook() {
		info->Clear();
		delete info;
		RemoveVectoredExceptionHandler(pException);
	}
	
	void SetExceptionHandler(PVECTORED_EXCEPTION_HANDLER pVecExcHandler) override;
	int SetHook  (void* source, void* destination) override;
	int UnsetHook(void* source) override;
	
	Vector<HookRecord*>* GetInfo() override;
	HookRecord* GetRecordBySource(void* source) override;

	
private:
	int SetHook   (void* source, void* destination, HANDLE* hThread, int* reg);
	int UnsetHook (void* source, HANDLE* hThread);
	int GetFreeReg(unsigned long long* mask);

	Vector<HookRecord*>* info;
	PVOID pException;
};
