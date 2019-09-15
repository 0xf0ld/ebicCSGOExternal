#include "includes.hpp"

MemMan MemClass;
offsets offset;
vector vec;
values val;
glowStruct Glow;

bool isBhopRunning, isTriggerbotHandlerRunning, isTriggerbotFiring, noFlashEnabled, isRadarEnabled = false;

glowStruct SetGlowColour(glowStruct Glow, uintptr_t entity) {
	bool isDefusing = MemClass.readMem<bool>(entity + offset.isDefusing);
	//int friendlyTeam = MemClass.readMem<int>(val.localPlayer + offset.team);
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

/*void getCurrentWeapon() {

}*/

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
	TeamGlow.red = 0.0f;
	TeamGlow.green = 0.0f;
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
	//if (isBhopRunning == true || noFlashEnabled == true)
	//	return;
	if (isTriggerbotFiring == true) {
		std::cout << "Firing" << std::endl;
		isTriggerbotFiring = !isTriggerbotFiring;
		std::this_thread::sleep_for(std::chrono::milliseconds(val.triggerDelay));
		MemClass.writeMem<int>(val.gameModule + offset.forceATTACK, 5);
	}
	isTriggerbotFiring = !isTriggerbotFiring;

	if (isTriggerbotFiring == false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		MemClass.writeMem<int>(val.gameModule + offset.forceATTACK, 4);
	}
}

bool checkTrigger() {
	int crosshair = MemClass.readMem<int>(val.localPlayer + offset.crosshair);
	if (crosshair != 0 && crosshair <= 64) {
		uintptr_t entity = MemClass.readMem<uintptr_t>(val.gameModule + offset.entityList + ((crosshair - 1) * 0x10));
		int enemyTeam = MemClass.readMem<int>(entity + offset.team);
		int enemyHealth = MemClass.readMem<int>(entity + offset.health);
		if (enemyTeam != val.friendlyTeam && enemyHealth > 0) {
			float distance = getDistance(entity);
			int weapon = MemClass.readMem<int>(val.localPlayer + offset.activeWeapon);
			int weaponEntity = MemClass.readMem<int>(val.gameModule + offset.entityList + ((weapon & 0xFFF) - 1) * 0x10);
			if (weaponEntity != NULL)
				val.currentWeaponID = MemClass.readMem<int>(weaponEntity + offset.itemDefIndex);

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
	/*if (isBhopRunning == true || noFlashEnabled == true)
		return;
	isTriggerbotHandlerRunning = !isTriggerbotHandlerRunning;*/
	//if (checkTrigger()) {
	std::thread triggerCheck(checkTrigger);
	triggerCheck.detach();

	if (checkTrigger() == true) {
		std::thread fire(fire);
		fire.detach();
	}
	//}
	//isTriggerbotHandlerRunning = !isTriggerbotHandlerRunning;
}

void bunnyHop() {
	while (isTriggerbotFiring == true) // || isTriggerbotHandlerRunning == true || noFlashEnabled == true)
		return;
	isBhopRunning = !isBhopRunning;
	std::cout << "Jumping" << std::endl;
	MemClass.writeMem<uintptr_t>(val.gameModule + offset.fJump, 6);
	isBhopRunning = !isBhopRunning;
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

}

void noFlash() {
	/*if (isTriggerbotFiring == true || isBhopRunning == true)
		return;
	noFlashEnabled = !noFlashEnabled;*/
	int flashDuration = 0;
	flashDuration = MemClass.readMem<int>(val.localPlayer + offset.flashDuration);
	if (flashDuration > 0)
		MemClass.writeMem<int>(val.localPlayer + offset.flashDuration, 0);
}

void alwaysRadar() {
	/*if (isTriggerbotFiring == true || isBhopRunning == true || isTriggerbotHandlerRunning == true)
		return;
	isRadarEnabled = !isRadarEnabled;*/
	for (short int entityID = 0; entityID <= 64; entityID++)
	{
		uintptr_t entity = MemClass.readMem<uintptr_t>(val.gameModule + offset.entityList + entityID * 0x10);
		if (entity != NULL)
			MemClass.writeMem<bool>(entity + offset.isSpotted, true);
	}
}


//this might not work at the moment.
/*void fakeLag() {
	BYTE SendPacket = MemClass.readMem<BYTE>(val.gameModule + offset.bSendPacket);

	if (SendPacket != 0) {
		MemClass.writeMem<BYTE>(val.gameModule + offset.bSendPacket, 0);
		Sleep(200);
		MemClass.writeMem<BYTE>(val.gameModule + offset.bSendPacket, 1);
	}
}*/