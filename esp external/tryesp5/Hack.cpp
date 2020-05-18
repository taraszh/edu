#pragma once

#include "Hack.h"
#include "offset.h"

Hack::Hack()
{
	DWORD processID = Helper::getProcessIdByName(L"csgo.exe");
	moduleAddress   = Helper::getModuleByName(processID, L"client_panorama.dll");
	processHandle   = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
}

Hack::~Hack()
{
	CloseHandle(processHandle);
}

void Hack::glowEsp()
{
	const PlayerInfo playerInfo = getPlayerInfo();
	glowObject = readMemory<uintptr_t>(moduleAddress + offsets::dwGlowObjectManager);
	
	for (short int i = 0; i < 33; i++)
	{
		const auto entity = readMemory<uintptr_t>(moduleAddress + offsets::dwEntityList + i * 0x10);
		
		if (isEntityValid(entity, playerInfo))
		{
			int glowIndex = readMemory<int>(entity + offsets::m_iGlowIndex);
			
			writeMemory<color>(entity + offsets::m_clrRender, clrEnemy);
			
			setEnemyGlow(entity, glowIndex);
		}
	}
}

PlayerInfo Hack::getPlayerInfo()
{
	auto localPlayerAddress = readMemory<uintptr_t>(moduleAddress + offsets::dwLocalPlayer);
	
	if (localPlayerAddress == NULL)
		while (localPlayerAddress == NULL)
		{
			std::cout << "Player address not found, keep searching.";

			localPlayerAddress = readMemory<uintptr_t>(moduleAddress + offsets::dwLocalPlayer);
			Sleep(1000);
		}
	
	PlayerInfo player;
	player.address = localPlayerAddress;
	player.team = readMemory<int>(localPlayerAddress + offsets::m_iTeamNum);
	
	return player;
}

bool Hack::isEntityValid(uintptr_t entity, PlayerInfo PlayerInfo)
{
	if (entity == NULL)
		return false;

	if (PlayerInfo.team == readMemory<int>(entity + offsets::m_iTeamNum))
		return false;
	
	if (readMemory<bool>(entity + offsets::m_bDormant))
		return false;

	const int enemyHP = readMemory<int>(entity + offsets::m_iHealth);
	if (enemyHP <= 0 || enemyHP > 100)
		return false;
	

	return true;
}

void Hack::setBrightness()
{
	clrTeam.red = 0;
	clrTeam.blue = 255;
	clrTeam.green = 0;

	clrEnemy.red = 255;
	clrEnemy.blue = 0;
	clrEnemy.green = 0;

	float brightness = 1.0f;

	int ptr = readMemory<int>(moduleAddress + offsets::model_ambient_min);
	int xorptr = *(int*)&brightness ^ ptr;
	writeMemory<int>(moduleAddress + offsets::model_ambient_min, xorptr);
}

void Hack::setEnemyGlow(uintptr_t entity, int glowIndex)
{
	GlowStruct EGlow;
	EGlow = readMemory<GlowStruct>(glowObject + (glowIndex * 0x38));
	EGlow = setGlowColor(EGlow, entity);
	writeMemory<GlowStruct>(glowObject + (glowIndex * 0x38), EGlow);
}

GlowStruct Hack::setGlowColor(GlowStruct Glow, uintptr_t entity)
{
	bool defusing = readMemory<bool>(entity + offsets::m_bIsDefusing);

	int health = readMemory<int>(entity + offsets::m_iHealth);
	
	Glow.red = health * -0.01 + 1;
	Glow.green = health * 0.01;
	Glow.alpha = 0.5f;
	Glow.renderWhenOccluded = true;
	Glow.renderWhenUnOccluded = false;
	return Glow;
}
