#include "includes.hpp"

int main() {
	int proc = MemClass.getProcess(L"csgo.exe");
	val.gameModule = MemClass.getModule(proc, L"client_panorama.dll");
	val.localPlayer = MemClass.readMem<uintptr_t>(val.gameModule + offset.localPlayer);

	if (val.localPlayer == NULL)
		while (val.localPlayer == NULL)
			val.localPlayer = MemClass.readMem<uintptr_t>(val.gameModule + offset.localPlayer);
	std::cout << std::hex << val.localPlayer << " = LocalPlayer" << std::endl;


	while (val.localPlayer != NULL && val.gameModule != NULL)
	{
		val.flag = MemClass.readMem<BYTE>(val.localPlayer + offset.flags);

		//if (GetKeyState(VK_F6))
		//{
		//	std::thread FAKELAG(fakeLag);
		//	FAKELAG.detach();
		//}
		//std::thread FAKELAG(fakeLag);
		//FAKELAG.detach();

		if (GetKeyState(VK_F9))
		{
			std::thread RADARHACK(alwaysRadar);
			RADARHACK.detach();
		}

		if (GetKeyState(VK_F7))
		{
			std::thread NOFLASH(noFlash);
			NOFLASH.detach();
		}

		if (GetKeyState(VK_F8))
		{
			std::thread ESP(handleGlow);
			ESP.detach();
		}

		if (GetAsyncKeyState(VK_SPACE)) {
			if (val.flag & (FL_ONGROUND)) {
				std::thread bhop(bunnyHop);
				bhop.detach();
			}
			else
			{
				MemClass.writeMem<uintptr_t>(val.gameModule + offset.fJump, 4);
			}
		}

		if (GetKeyState(VK_F2))
		{
			val.friendlyTeam = MemClass.readMem<int>(val.localPlayer + offset.team);

			//canFire = !canFire;
			//if (canFire == true) {
				std::thread triggerbot(handleTriggerbot);
				triggerbot.detach();
			//}
		}


		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}
