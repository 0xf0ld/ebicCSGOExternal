#include <Windows.h>
#include <iostream>
#include <cmath>
#include <thread>
#include "MemMan.h"
#include "csgo.hpp"

MemMan MemClass;

bool isBhopRunning, isTriggerbotHandlerRunning, isTriggerbotFiring, noFlashEnabled, isRadarEnabled = false;

struct offsets {
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
}offset;

struct values {
	uintptr_t localPlayer, process, gameModule, glowObject;
	int friendlyTeam, triggerDelay, currentWeaponID;
	BYTE flag;
}val;

struct glowStruct {
	BYTE base[4]; // 4 bytes
	float red, green, blue, alpha; // 4, 4, 4, 4 bytes
	BYTE buffer[16]; // 16 bytes
	bool renderWhenIncluded; // 1 byte
	bool renderWhenNotIncluded; // 1 byte
	bool fullBloom; // 1 byte
	BYTE buffer2[5]; // 5 bytes
	int glowStyle; // 4 bytes
}Glow;

struct vector {
	float x, y, z;
};

glowStruct SetGlowColour(glowStruct Glow, uintptr_t entity) {
	bool isDefusing = MemClass.readMem<bool>(entity + offset.isDefusing);
	if (isDefusing) {
		Glow.red = 1.0f;
		Glow.green = 1.0f;
		Glow.blue = 1.0f;
		Glow.alpha = 1.0f;
		Glow.fullBloom = true;
	}
	else {
		int health = MemClass.readMem<int>(entity + offset.health);
		Glow.red = health * -0.01 + 1;
		Glow.green = health * 0.01;
		Glow.alpha = 1.0f;
		Glow.fullBloom = false;
		//Glow.glowStyle = 1, 2, 3;
	}
	Glow.renderWhenIncluded = true;
	Glow.renderWhenNotIncluded = false;

	return Glow;
}

bool isScoped() {
	return MemClass.readMem<bool>(val.localPlayer + offset.isScoped);
}

void setTriggerDelay(float distance) {
	float delay;

	switch (val.currentWeaponID)
	{
	case 262204: delay = 3.3; break; //M4A1-S (might be wrong)
	case 61: delay = 3.3; break; //USP-S (might be wrong)
	case 60: delay = 3.3; break; //M4A1-S (might be wrong)
	case 7: delay = 3.3; break; //CV-47 (might be wrong)
	case 40: delay = 0.2; break; //SSG 08
	case 9: delay = 0.2; break; //AWP
	case 4: delay = 3.3; break; //GLOCK
	case 8: delay = 3.3; break; //AUG
	case 2: delay = 3.3; break; //Dual Berettas
	case 16: delay = 3.3; break; //M4A4 (might be wrong)
	case 10: delay = 3.3; break; //Famas
	case 39: delay = 3.3; break; //SG553
	case 30: delay = 3.3; break; //Tec 9
	case 34: delay = 3.3; break; //MP9
	case 13: delay = 3.3; break; //Galil
	case 38: delay = 3.3; break; //SCAR-20
	case 11: delay = 3.3; break; //G38G1 (T auto)
	default: delay = 0;
	}
	val.triggerDelay = delay * distance;
	std::cout << val.triggerDelay << std::endl;
}

void getCurrentWeapon() {
	int weapon = MemClass.readMem<int>(val.localPlayer + offset.activeWeapon);
	int weaponEntity = MemClass.readMem<int>(val.gameModule + offset.entityList + ((weapon & 0xFFF) - 1) * 0x10); 
	if (weaponEntity != NULL)
		val.currentWeaponID = MemClass.readMem<int>(weaponEntity + offset.itemDefIndex);
}

float getDistance(uintptr_t entity) {
	vector myLoc = MemClass.readMem<vector>(val.localPlayer + offset.vecOrigin);
	vector enemyLoc = MemClass.readMem<vector>(entity + offset.vecOrigin);

	return sqrt(pow(myLoc.x - enemyLoc.x, 2) + pow(myLoc.y - enemyLoc.y, 2) + pow(myLoc.z - enemyLoc.z, 2)) * 0.0254; // * 0.0254 converts to metres from Source units
}

void setEnemyGlow(uintptr_t entity, int glowIndex) {
	glowStruct EnemyGlow;
	EnemyGlow = MemClass.readMem<glowStruct>(val.glowObject + (glowIndex * 0x38));
	EnemyGlow = SetGlowColour(EnemyGlow, entity);

	MemClass.writeMem<glowStruct>(val.glowObject + (glowIndex * 0x38), EnemyGlow);
}

void setTeamGlow(uintptr_t entity, int glowIndex) {
	glowStruct TeamGlow;
	TeamGlow = MemClass.readMem<glowStruct>(val.glowObject + (glowIndex * 0x38));
	TeamGlow.blue = 1.0f;
	TeamGlow.alpha = 1.0f;
	TeamGlow.renderWhenIncluded = true;
	TeamGlow.renderWhenNotIncluded = false;
	MemClass.writeMem<glowStruct>(val.glowObject + (glowIndex * 0x38), TeamGlow);
}

void handleGlow() {
	val.glowObject = MemClass.readMem<uintptr_t>(val.gameModule + offset.glowObjMan);
	int friendlyTeam = MemClass.readMem<int>(val.localPlayer + offset.team);

	for (short int entityID = 0; entityID <= 64; entityID++)
	{
		uintptr_t entity = MemClass.readMem<uintptr_t>(val.gameModule + offset.entityList + entityID * 0x10); // scan through entity list for player enemy and team IDs
		if (entity != NULL)
		{
			int glowIndex = MemClass.readMem<int>(entity + offset.glowIndex);
			int enemyTeam = MemClass.readMem<int>(entity + offset.team);

			if (friendlyTeam == enemyTeam) //Team ESP
				setTeamGlow(entity, glowIndex);
			else
				setEnemyGlow(entity, glowIndex);
		}
	}
}

void fire() {
	if (isBhopRunning == true || noFlashEnabled == true)
		return;
	std::cout << "Firing" << std::endl;
	isTriggerbotFiring = !isTriggerbotFiring;
	Sleep(val.triggerDelay);
	MemClass.writeMem<int>(val.gameModule + offset.forceATTACK, 5);
	Sleep(20);
	MemClass.writeMem<int>(val.gameModule + offset.forceATTACK, 4);
	isTriggerbotFiring = !isTriggerbotFiring;
}

bool checkTrigger() {
	int crosshair = MemClass.readMem<int>(val.localPlayer + offset.crosshair);
	if (crosshair != 0 && crosshair <= 64) {
		uintptr_t entity = MemClass.readMem<uintptr_t>(val.gameModule + offset.entityList + ((crosshair - 1) * 0x10));
		int enemyTeam = MemClass.readMem<int>(entity + offset.team);
		int enemyHealth = MemClass.readMem<int>(entity + offset.health);
		if (enemyTeam != val.friendlyTeam && enemyHealth > 0) {	
			float distance = getDistance(entity);
			std::thread getcurrentweapon(getCurrentWeapon);
			getcurrentweapon.detach();

			setTriggerDelay(distance);
			if (val.currentWeaponID == 40 || val.currentWeaponID == 9) // check if using scout or AWP
				return isScoped();
			else
				return true;
		}
		else
			return false;
	}
	else return false;
}

void handleTriggerbot() { 
	if (isBhopRunning == true || noFlashEnabled == true)
		return;
	isTriggerbotHandlerRunning = !isTriggerbotHandlerRunning;
		if (checkTrigger()) {
			std::thread fire(fire);
			fire.detach();
		}
	isTriggerbotHandlerRunning = !isTriggerbotHandlerRunning;
}

void bunnyHop() {
	if (isTriggerbotFiring == true || isTriggerbotHandlerRunning == true || noFlashEnabled == true)
		return;
	isBhopRunning = !isBhopRunning;
	std::cout << "Jumping" << std::endl;
	MemClass.writeMem<uintptr_t>(val.gameModule + offset.fJump, 6);
	isBhopRunning = !isBhopRunning;
}

void noFlash() {
	if (isTriggerbotFiring == true || isBhopRunning == true)
		return;
	noFlashEnabled = !noFlashEnabled;
	int flashDuration = 0;
	flashDuration = MemClass.readMem<int>(val.localPlayer + offset.flashDuration);
	if (flashDuration > 0)
		MemClass.writeMem<int>(val.localPlayer + offset.flashDuration, 0);
	noFlashEnabled = !noFlashEnabled;
}

void alwaysRadar() {
	if (isTriggerbotFiring == true || isBhopRunning == true || isTriggerbotHandlerRunning == true)
		return;
	isRadarEnabled = !isRadarEnabled;
	for (short int entityID = 0; entityID <= 64; entityID++)
	{
		uintptr_t entity = MemClass.readMem<uintptr_t>(val.gameModule + offset.entityList + entityID * 0x10);
		if (entity != NULL)
			MemClass.writeMem<bool>(entity + offset.isSpotted, true);
	}
	isRadarEnabled = !isRadarEnabled;
}

int main() {
	bool canFire = false;

	int proc = MemClass.getProcess(L"csgo.exe");
	val.gameModule = MemClass.getModule(proc, L"client_panorama.dll");
	val.localPlayer = MemClass.readMem<uintptr_t>(val.gameModule + offset.localPlayer);

	if (val.localPlayer == NULL)
		while (val.localPlayer == NULL)
			val.localPlayer = MemClass.readMem<uintptr_t>(val.gameModule + offset.localPlayer);

	while (true)
	{
		val.flag = MemClass.readMem<BYTE>(val.localPlayer + offset.flags);

		if (GetKeyState(VK_F9) & 1)
		{
			std::thread RADARHACK(alwaysRadar);
			RADARHACK.detach();
		}

		if (GetKeyState(VK_F7) & 1)
		{
			std::thread NOFLASH(noFlash);
			NOFLASH.detach();
		}

		if (GetKeyState(VK_F8) & 1)
		{
			std::thread ESP(handleGlow);
			ESP.detach();
		}

		if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0)) {
			std::thread bhop(bunnyHop);
			bhop.detach();
			Sleep(1);
		}

		if (GetAsyncKeyState(VK_F2) & 1) //Triggerbot toggle
		{
			val.friendlyTeam = MemClass.readMem<int>(val.localPlayer + offset.team);
			canFire = !canFire;
		}

		if (canFire) {
			std::thread triggerbot(handleTriggerbot);
			triggerbot.detach();
		}

		Sleep(1);

	}

	return 0;
}