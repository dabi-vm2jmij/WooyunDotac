#include "stdafx.h"
#include "UILib.h"

#ifdef UILIB_EXPORTS

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

#endif
