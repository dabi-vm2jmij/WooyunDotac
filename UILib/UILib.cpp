#include "stdafx.h"
#include "UILib.h"
#include "UILibApp.h"

#ifdef UILIB_EXPORTS

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
#ifdef _DLL
		DisableThreadLibraryCalls(hModule);
#endif
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

#endif

CUILibApp g_theApp;

CUILibApp::CUILibApp() : m_nLayoutMsg(RegisterWindowMessage(_T("UILibLayout")))
{
}

void CUILibApp::ShowTip(HWND hParent, LPCWSTR lpText)
{
	if (lpText)
		m_toolTip.Show(hParent, lpText);
	else
		m_toolTip.Hide();
}
