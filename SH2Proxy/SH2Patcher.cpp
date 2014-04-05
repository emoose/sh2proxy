#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SH2Patcher.h"

static char gameTitle[] = "Silent Hill 2";

PCHAR*
CommandLineToArgvA(
PCHAR CmdLine,
int* _argc
)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len + 2) / 2)*sizeof(PVOID)+sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len + 2)*sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while (a = CmdLine[i]) {
		if (in_QM) {
			if (a == '\"') {
				in_QM = FALSE;
			}
			else {
				_argv[j] = a;
				j++;
			}
		}
		else {
			switch (a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool SH2Patcher::Init()
{
	this->dwVideoWidth = (int)GetPrivateProfileInt("Video", "Width", 1280, ".\\sh2proxy.ini");
	this->dwVideoHeight = (int)GetPrivateProfileInt("Video", "Height", 720, ".\\sh2proxy.ini");
	this->bVideoWindowed = (GetPrivateProfileInt("Video", "Windowed", 0, ".\\sh2proxy.ini") == 1);

	this->bWindowBorderless = (GetPrivateProfileInt("Window", "Borderless", 0, ".\\sh2proxy.ini") == 1);
	this->dwWindowX = GetPrivateProfileInt("Window", "PositionX", 0, ".\\sh2proxy.ini");
	this->dwWindowY = GetPrivateProfileInt("Window", "PositionY", 0, ".\\sh2proxy.ini");

	LPSTR *szArgList;
	int argCount;

	szArgList = CommandLineToArgvA(GetCommandLine(), &argCount);
	if (szArgList != NULL && argCount > 1)
	{
		for (int i = 1; i < argCount; i++)
		{
			char* pos = strstr(szArgList[i], "=");
			int intval = 0;
			if (pos != NULL)
			{
				pos += 1;
				intval = atoi(pos);
			}

			if (strstr(szArgList[i], "-width=") != NULL || strstr(szArgList[i], "-w=") != NULL)
				this->dwVideoWidth = intval;

			if (strstr(szArgList[i], "-height=") != NULL || strstr(szArgList[i], "-h=") != NULL)
				this->dwVideoHeight = intval;

			if (strcmp(szArgList[i], "-windowed") == 0 || strcmp(szArgList[i], "-window") == 0 || strcmp(szArgList[i], "-w") == 0)
				this->bVideoWindowed = true;

			if (strcmp(szArgList[i], "-fullscreen") == 0 || strcmp(szArgList[i], "-fs") == 0 || strcmp(szArgList[i], "-f") == 0)
				this->bVideoWindowed = false;

			if (strcmp(szArgList[i], "-borderless") == 0 || strcmp(szArgList[i], "-bl") == 0)
				this->bWindowBorderless = true;

			if (strcmp(szArgList[i], "-borders") == 0 || strcmp(szArgList[i], "-b") == 0)
				this->bWindowBorderless = false;

			if (strstr(szArgList[i], "-x=") != NULL)
				this->dwWindowX = intval;

			if (strstr(szArgList[i], "-y=") != NULL)
				this->dwWindowY = intval;
		}
	}

	this->bPatchSingleCore = (GetPrivateProfileInt("Patches", "SingleCoreAffinity", 1, ".\\sh2proxy.ini") == 1);
	this->bPatchDisableSafeMode = (GetPrivateProfileInt("Patches", "DisableSafeMode", 1, ".\\sh2proxy.ini") == 1);
	this->bPatchFastTransitions = (GetPrivateProfileInt("Patches", "FastTransitions", 1, ".\\sh2proxy.ini") == 1);

	this->wTransitionWidth = (int)GetPrivateProfileInt("Patches", "FastTransitionWidth", 0, ".\\sh2proxy.ini");
	this->wTransitionHeight = (int)GetPrivateProfileInt("Patches", "FastTransitionHeight", 0, ".\\sh2proxy.ini");

	this->bExtraCreateLocalFix = (GetPrivateProfileInt("Extra", "CreateLocalFix", 1, ".\\sh2proxy.ini") == 1);

	this->handle = GetCurrentProcess();
	this->baseAddr = (DWORD_PTR)GetModuleHandle("sh2pc.exe");

	if (this->IsSH2())
	{
		this->PatchCode();

		if (this->bExtraCreateLocalFix)
		{
			if (FileExists("local.fix"))
				return true;

			std::ofstream myfile;
			myfile.open("local.fix");
			myfile << "HACK DX_CONFIG_DRIVER_AA_NOT_BROKEN 1";
			myfile.close();
		}
	}

	return true;
}

bool SH2Patcher::IsSH2()
{
	byte test[0x15];
	char* compare = "Silent_Hill_2_unique";
	bool isSH2 = true;
	if (this->baseAddr != (DWORD_PTR)0x400000)
		return false;

	// read a location that we know contains this string
	ReadProcessMemory(this->handle, (LPVOID)0x62E074, test, 0x15, NULL);
	for (int i = 0; i < 0x14; i++)
	{
		if (test[i] == compare[i])
			continue;
		isSH2 = false;
		break;
	}

	return isSH2;
}

bool SH2Patcher::PatchCode()
{
	byte nop = 0x90;
	byte null = 0x0;

	// patches create window code to allow for custom height, x/y positions, window style and window title
	BYTE windowedPatch[] =	  { 0x68,

								// 0x1 - Window height
								0x00, 0x00, 0x00, 0x00,
								0x68,

								// 0x6 - Window width (default 0x80000000)
								0x00, 0x00, 0x00, 0x80,
								0x68,

								// 0xB - Window y pos
								0x00, 0x00, 0x00, 0x00,
								0x68,

							    // 0x10 - Window x pos
								0x00, 0x00, 0x00, 0x00,
								0x68,

							    // 0x15 - Window style, last byte to 0x80 for borderless
								0x00, 0x00, 0x00, 0x00,
								0x68,

							    // 0x1A - Window title offset
								0x4C, 0xE0, 0x62, 0x00,
								0x68,

							    // 0x1F - Window class offset
							    0x3C, 0xE0, 0x62, 0x00,
								
								0x6A, 0x00, 0xFF, 0x15, 0xE4, 0x9A, 0x4A, 0x02, 0x85, 0xC0, 0xA3, 0x6C, 0x4D, 0x93, 0x00, 0x74,
								0x21, 0x8B, 0x0D, 0xA4, 0x4D, 0x93, 0x00, 0x6A, 0x00, 0x68, 0xA8, 0x4D, 0x93, 0x00, 0x68, 0x18,
								0x5E, 0x63, 0x00, 0x68, 0x00, 0x08, 0x00, 0x00, 0x51, 0xE8,

							    // next byte needs updating if above size changed!
							    0x0E,

							    0x71, 0x03, 0x00, 0x85, 0xC0, 0x7D, 0x03, 0x33, 0xC0, 0xC3, 0x8B,
							    0x54, 0x24, 0x08, 0xA1, 0x6C, 0x4D, 0x93, 0x00, 0x52, 0x50, 0xFF, 0x15, 0xE0, 0x9A, 0x4A, 0x02,
							    0x8B, 0x0D, 0x6C, 0x4D, 0x93, 0x00, 0x51, 0xFF, 0x15, 0xDC, 0x9A, 0x4A, 0x02, 0xB8, 0x01, 0x00,
							    0x00, 0x00, 0xC3, 0x90, 0x90 };

	// patches some code that afaik creates a snapshot of the game for use in eg. inventory status screen
	// it gets run every transition though (map/movie/etc) instead of just the inventory, and snapshotting 1080p+ can take a while
	// patch makes it use a static number instead of reading the current screen resolution from memory, allowing us to change it later on as needed
	BYTE transitionPatch[] = { 0x6A, 0x15, 0x68, 0x20, 0x03, 0x00, 0x00, 0x90, 0x90, 0x68, 0x58, 0x02, 0x00, 0x00, 0x90, 0x90 };


	if (!this->IsSH2())
		return false;

	if (this->bPatchSingleCore)
	{
		// set affinity to one core, fixes sound issues
		DWORD_PTR processAffinityMask = 1;
		SetProcessAffinityMask(this->handle, processAffinityMask);
	}

	if (this->bPatchDisableSafeMode)
	{
		// nop out the call to the safe mode function
		for (int i = 0; i < 5; i++)
			WriteProcessMemory(this->handle, (LPVOID)(0x4F6DC0 + i), &nop, 1, NULL);
	}

	if (this->bPatchFastTransitions)
	{
		// use ini height/width values if set, otherwise just use half the screen resolution
		int height = this->wTransitionHeight;
		if (height > this->dwVideoHeight) height = 0;
		if (height == 0) height = this->dwVideoHeight / 2;

		int width = this->wTransitionWidth;
		if (width > this->dwVideoWidth) height = 0;
		if (width == 0) width = this->dwVideoWidth / 2;

		memcpy(transitionPatch + 3, &height, 4);
		memcpy(transitionPatch + 10, &width, 4);

		WriteProcessMemory(this->handle, (LPVOID)(0x477518), &transitionPatch, sizeof(transitionPatch) / sizeof(BYTE), NULL);
	}

	// change the window code height and width
	memcpy(windowedPatch + 1, &this->dwVideoHeight, 4);
	memcpy(windowedPatch + 6, &this->dwVideoWidth, 4);

	// change window code x/y position
	memcpy(windowedPatch + 0xB, &this->dwWindowY, 4);
	memcpy(windowedPatch + 0x10, &this->dwWindowX, 4);

	if (this->bVideoWindowed && this->bWindowBorderless) // set CreateWindowExA exStyle param to 0x80000000 (WS_POPUP)
		windowedPatch[0x18] = 0x80;

	// patch window code to show gameTitle above as window title
	int gameTitleOffset = (int)&gameTitle;
	memcpy(windowedPatch + 0x1A, &gameTitleOffset, 4);

	WriteProcessMemory(this->handle, (LPVOID)(0x4063CB), &windowedPatch, sizeof(windowedPatch) / sizeof(BYTE), NULL);
	return true;
}

bool SH2Patcher::PatchResolution(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	int safeMode = 0;
	int selectedRes = 1;
	int resCount = 5;

	// first offset in these arrays is the address of the main screen res choices (480p, 600p, etc)
	// code will loop on the first offset and fill in each of the screen resolutions (5 of them afaik) to be the same
	int widthPatch[3] = { 0x8A1474, 0xA35498, 0xA36080 };
	int heightPatch[3] = { 0x8A1478, 0xA3549C, 0xA36084 };
	
	if (pPresentationParameters)
	{
		// update the CreateDevice params
		pPresentationParameters->BackBufferWidth = this->dwVideoWidth;
		pPresentationParameters->BackBufferHeight = this->dwVideoHeight;
		pPresentationParameters->Windowed = this->bVideoWindowed;
	}

	if (!this->IsSH2())
		return false;

	// disable safemode and selected resolution to 1, probably don't work but meh
	WriteProcessMemory(this->handle, (LPVOID)(0xA36024), &safeMode, 4, NULL);
	WriteProcessMemory(this->handle, (LPVOID)(0xA36028), &selectedRes, 4, NULL);

	// patch static + loaded widths and heights
	for (int i = 0; i < 3; i++)
	{
		int offWidth = widthPatch[i];
		int offHeight = heightPatch[i];

		int count = 1;
		if (i == 0) count = resCount; // if we're on the first set of patches (which are pointers to the first in the res selection)
									  // then we'll patch all the resolutions on the selection

		for (int y = 0; y < count; y++)
		{
			WriteProcessMemory(this->handle, (LPVOID)(offWidth), &this->dwVideoWidth, 4, NULL);
			WriteProcessMemory(this->handle, (LPVOID)(offHeight), &this->dwVideoHeight, 4, NULL);
			offWidth += 8;
			offHeight += 8;
		}
	}

	return true;
}