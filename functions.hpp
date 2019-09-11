#ifndef _INCLUDES_HPP_
#define _INCLUDES_HPP_
#include <cmath>
#include <Windows.h>
#include <thread>
#include <vector>
#include "MemMan.hpp"
#include "csgo.hpp"

#define FL_ONGROUND (1 << 0)
//#define IN_JUMP (1 << 0)

struct offsets {
	uintptr_t bSendPacket = hazedumper::signatures::dwbSendPackets;
	uintptr_t localPlayer = hazedumper::signatures::dwLocalPlayer;
	uintptr_t fJump = hazedumper::signatures::dwForceJump;
	uintptr_t flags = hazedumper::netvars::m_fFlags;
	uintptr_t forceATTACK = hazedumper::signatures::dwForceAttack;
	uintptr_t entityList = hazedumper::signatures::dwEntityList;
	uintptr_t crosshair = hazedumper::netvars::m_iCrosshairId;
	uintptr_t team = hazedumper::netvars::m_iTeamNum;
	uintptr_t health = hazedumper::netvars::m_iHealth;
	uintptr_t glowIndex = hazedumper::netvars::m_iGlowIndex;
	uintptr_t glowObjMan = hazedumper::signatures::dwGlowObjectManager;
	uintptr_t isDefusing = hazedumper::netvars::m_bIsDefusing;
	uintptr_t flashDuration = hazedumper::netvars::m_flFlashDuration;
	uintptr_t isSpotted = hazedumper::netvars::m_bSpotted;
	uintptr_t vecOrigin = hazedumper::netvars::m_vecOrigin;
	uintptr_t activeWeapon = hazedumper::netvars::m_hActiveWeapon;
	uintptr_t itemDefIndex = hazedumper::netvars::m_iItemDefinitionIndex;
	uintptr_t isScoped = hazedumper::netvars::m_bIsScoped;
};

struct values {
	uintptr_t localPlayer, process, gameModule, glowObject;
	int friendlyTeam, triggerDelay, currentWeaponID;
	BYTE flag;
};

struct glowStruct {
	BYTE base[4]; // 4 bytes
	float red, green, blue, alpha; // 4, 4, 4, 4 bytes
	BYTE buffer[16]; // 16 bytes
	bool renderWhenIncluded; // 1 byte
	bool renderWhenNotIncluded; // 1 byte
	bool fullBloom; // 1 byte
	BYTE buffer2[5]; // 5 bytes
	int glowStyle; // 4 bytes
};

struct vector {
	float x, y, z;
};

extern MemMan MemClass;
extern offsets offset;
extern values val;
extern glowStruct Glow;
extern vector vec;

void fakeLag();
bool isScoped();
bool checkTrigger();
void handleTriggerbot();
void bunnyHop();
void noFlash();
void alwaysRadar();
void setTriggerDelay(float distance);
void getCurrentWeapon();
void setEnemyGlow(uintptr_t entity, int glowIndex);
void setTeamGlow(uintptr_t entity, int glowIndex);
void handleGlow();
void fire();
float getDistance(uintptr_t entity);

//bool canFire = false;


#endif