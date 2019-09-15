#pragma once
#include <vector>
#include <Windows.h>

class MemMan
{
public:
	MemMan();
	~MemMan();
	template <typename T>
	T readMem(uintptr_t addr)
	{
		T x;
		ReadProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}
	template <typename T>
	bool writeMem(uintptr_t addr, T const& val)
	{
		return WriteProcessMemory(handle, (LPBYTE*)addr, &val, sizeof(val), NULL);
		//return val;
	}

	uintptr_t getProcess(const wchar_t*);
	uintptr_t getModule(uintptr_t, const wchar_t*);
	uintptr_t getAddress(uintptr_t, std::vector<uintptr_t>);
	
private:
	HANDLE handle;

};