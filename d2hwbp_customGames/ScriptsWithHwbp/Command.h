#pragma once

#include "AbstractHook.h"
#include "Queue.h"

class IDeferredCommand {
protected:
	IDeferredCommand(AbstractHook* _hook) {
		hook = _hook;
	}
public:
	virtual ~IDeferredCommand() {
		hook = nullptr;
	}
	virtual void Run() = 0;
protected:
	AbstractHook* hook;
};

class SetD7Command : public IDeferredCommand {
public:
	SetD7Command(AbstractHook* _hook, unsigned long long _threadId, int _reg) : IDeferredCommand(_hook) {
		threadId = _threadId;
		reg = _reg;
	}
	
	void Run() {
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadId);
		if (hThread != NULL) {
			bool res = SetD7(&hThread);
			CloseHandle(hThread);
		}
	}

	
private:
	bool SetD7(HANDLE* hThread) {
		CONTEXT context;
		ZeroMemory(&context, sizeof(context));
		context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
		if (!GetThreadContext(*hThread, &context))
			return false;
		*(&context.Dr0 + reg) = (unsigned long long)hook->GetInfo()->GetItem(reg)->source;
		context.Dr7 |= 1ULL << (2 * reg);

		if (!SetThreadContext(*hThread, &context))
			return false;
		return true;
	}

	unsigned long long threadId;
	int reg;
};

class IDeferredCommands : public Queue<IDeferredCommand*>, public IDeferredCommand {
protected:
	IDeferredCommands() : Queue(), IDeferredCommand(nullptr) { }

public:
	virtual ~IDeferredCommands() { }
};

class DeferredCommands : public IDeferredCommands {
public:
	DeferredCommands() : IDeferredCommands() { }
	~DeferredCommands() { }
	
	void Run() {
		auto len = Length();
		for (int i = 0; i < len; i++) {
			auto cmd = Dequeue();
			cmd->Run();
			delete cmd;
		}
	}
};