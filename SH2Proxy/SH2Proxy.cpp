// proxydll.cpp
#include "stdafx.h"
#include "SH2Proxy.h"
#include "SH2Patcher.h"

// global variables
#pragma data_seg (".dinput8_shared")
HINSTANCE           gl_hOriginalDll;
HINSTANCE           gl_hThisInstance;
SH2Patcher			gl_patcher;
#pragma data_seg ()

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	// to avoid compiler lvl4 warnings 
	LPVOID lpDummy = lpReserved;
	lpDummy = NULL;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: InitInstance(hModule); break;
	case DLL_PROCESS_DETACH: ExitInstance(); break;

	case DLL_THREAD_ATTACH:  break;
	case DLL_THREAD_DETACH:  break;
	}
	return TRUE;
}

// Exported function
HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
{
	if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the right dll

	// Hooking IDirect3D Object from Original Library
	typedef HRESULT(WINAPI* DI8_Type)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter);
	DI8_Type DI8Create_fn = (DI8_Type)GetProcAddress(gl_hOriginalDll, "DirectInput8Create");

	// Debug
	if (!DI8Create_fn)
	{
		OutputDebugString("PROXYDLL: Pointer to original DirectInput8Create function not received ERROR ****\r\n");
		::ExitProcess(0); // exit the hard way
	}

	HRESULT res = DI8Create_fn(hinst, dwVersion, riidltf, ppvOut, punkOuter);

	gl_patcher.PatchCode();

	// Return pointer to hooking Object instead of "real one"
	return res;
}

void InitInstance(HANDLE hModule)
{
	OutputDebugString("PROXYDLL: InitInstance called.\r\n");

	// Initialisation
	gl_hOriginalDll = NULL;
	gl_hThisInstance = NULL;

	// Storing Instance handle into global var
	gl_hThisInstance = (HINSTANCE)hModule;

	gl_patcher.Init();
}

void LoadOriginalDll(void)
{
	char buffer[MAX_PATH];

	// Getting path to system dir and to dll
	::GetSystemDirectory(buffer, MAX_PATH);

	// Append dll name
	strcat_s(buffer, MAX_PATH, "\\dinput8.dll");

	// try to load the system's dll, if pointer empty
	if (!gl_hOriginalDll) gl_hOriginalDll = ::LoadLibrary(buffer);

	// Debug
	if (!gl_hOriginalDll)
	{
		OutputDebugString("PROXYDLL: Original dinput8.dll not loaded ERROR ****\r\n");
		::ExitProcess(0); // exit the hard way
	}
}

void ExitInstance()
{
	OutputDebugString("PROXYDLL: ExitInstance called.\r\n");

	// Release the system's dll
	if (gl_hOriginalDll)
	{
		::FreeLibrary(gl_hOriginalDll);
		gl_hOriginalDll = NULL;
	}
}

