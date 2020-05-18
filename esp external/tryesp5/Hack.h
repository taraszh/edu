#pragma once

#include <cstdint>
#include <Windows.h>

#include "Hack.h"
#include "Helper.h"

struct PlayerInfo
{
	int team;
	uintptr_t address;
};

struct color
{
	BYTE red, green, blue;
};

color clrEnemy;
color clrTeam;

struct GlowStruct
{
	BYTE base[4];
	float red;
	float green;
	float blue;
	float alpha;
	BYTE buffer[16];
	bool renderWhenOccluded;
	bool renderWhenUnOccluded;
	bool fullBloom;
	BYTE buffer1[5];
	int glowStyle;
};

class Hack
{
public:

	Hack();
	~Hack();

	void glowEsp();
	void setBrightness();
	void setEnemyGlow(uintptr_t entity, int glowIndex);

	GlowStruct setGlowColor(GlowStruct Glow, uintptr_t entity);
	
	PlayerInfo getPlayerInfo() ;
	bool       isEntityValid(uintptr_t entity, PlayerInfo playerInfo) ;

	uintptr_t moduleAddress;


	template <typename someType>
	someType readMemory(uintptr_t address)
	{
		someType buffer;
		ReadProcessMemory(processHandle, (LPBYTE*)address, &buffer, sizeof buffer, nullptr);
		return buffer;
	}

	template <typename someType>
	someType writeMemory(uintptr_t address, someType x)
	{
		WriteProcessMemory(processHandle, (LPBYTE*)address, &x, sizeof(x), nullptr);
		return x;
	}

	
private:
	//Memory* memory;
	HANDLE processHandle;
	uintptr_t glowObject;

};

