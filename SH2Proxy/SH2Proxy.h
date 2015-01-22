// proxydll.h
#pragma once
#include <dinput.h>

// Exported function
HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter);

// regular functions
void InitInstance(HANDLE hModule);
void ExitInstance(void);
void LoadOriginalDll(void);