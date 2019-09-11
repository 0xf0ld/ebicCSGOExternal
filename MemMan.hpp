#pragma once
#include <vector>
#include <Windows.h>

#define WIN32_LEAN_AND_MEAN

class MemMan
{
public:
	MemMan();
	~MemMan();
	template <typename T>
	bool readMem(uintptr_t addr, T const& val)
	{
		return !!ReadProcessMemory(PROC, reinterpret_cast<LPVOID>(address), reinterpret_cast<LPCVOID>(&val), sizeof(val), nullptr);
		//return !!ReadProcessMemory(PROC, (LPBYTE*)addr, &x, sizeof(x), NULL);
	}
	template <typename T>
	bool writeMem(uintptr_t addr, T const& val)
	{
		return !!WriteProcessMemory(PROC, reinterpret_cast<LPVOID>(address), reinterpret_cast<LPCVOID>(&val), sizeof(val), nullptr);
		//return WriteProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
	}

	uintptr_t getProcess(const wchar_t*);
	uintptr_t getModule(uintptr_t, const wchar_t*);
	uintptr_t getAddress(uintptr_t, std::vector<uintptr_t>);

private:
	HANDLE handle;
};