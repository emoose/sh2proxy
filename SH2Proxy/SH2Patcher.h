#ifndef SH2PATCHER_H_
#define SH2PATCHER_H_
#include "stdafx.h"

class SH2Patcher
{
private:
	HANDLE handle;
	DWORD_PTR baseAddr;

	int dwVideoWidth;
	int dwVideoHeight;
	bool bVideoWindowed;

	bool bWindowBorderless;
	int dwWindowX;
	int dwWindowY;

	bool bPatchSingleCore;
	bool bPatchDisableSafeMode;
	bool bPatchFastTransitions;
	int wTransitionWidth;
	int wTransitionHeight;

	bool bExtraCreateLocalFix;

public:
	bool Init();
	bool IsSH2();

	bool PatchResolution(D3DPRESENT_PARAMETERS* pPresentationParameters);
	bool PatchCode();
};

#endif