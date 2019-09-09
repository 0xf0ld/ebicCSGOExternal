#include "includes.hpp"

int main() {
	bool canFire = false;

	int proc = MemClass.getProcess(L"csgo.exe");
	val.gameModule = MemClass.getModule(proc, L"client_panorama.dll");
	val.localPlayer = MemClass.readMem<uintptr_t>(val.gameModule + offset.localPlayer);

	if (val.localPlayer == NULL)
		while (val.localPlayer == NULL)
			val.localPlayer = MemClass.readMem<uintptr_t>(val.gameModule + offset.localPlayer);
	std::cout << std::hex << val.localPlayer << " = LocalPlayer" << std::endl;
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

		if (GetAsyncKeyState(VK_SPACE)) {
			if (val.flag & (IN_JUMP)) {
				std::thread bhop(bunnyHop);
				bhop.detach();
				Sleep(1);
			}
			else
			{
				MemClass.writeMem<uintptr_t>(val.gameModule + offset.fJump, 4);
			}
		}

		if (GetAsyncKeyState(VK_F2) & 1)
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
