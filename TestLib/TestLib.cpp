// TestLib.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MainFrm.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	INITCOMMONCONTROLSEX iccx = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
	InitCommonControlsEx(&iccx);

#ifdef _DEBUG
	wchar_t szSkinPath[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, szSkinPath);
	PathAppendW(szSkinPath, L"res\\Skin");
	UILib::LoadSkin(szSkinPath);
#else
	UILib::LoadSkin(MAKEINTRESOURCEW(IDR_SKIN1));
#endif

	CMainFrame mainFrm;
	mainFrm.CreateFromXml(L"Main.xml");
	mainFrm.ShowWindow(SW_SHOW);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
