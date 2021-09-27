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

CUILibApp::CUILibApp() : m_bPostMsg(false)
{
}

CUILibApp::~CUILibApp()
{
	if (IsWindow())
		DestroyWindow();
}

void CUILibApp::RemoveLayout(CUIRootView *pRootView)
{
	for (auto it = m_deqRootViews.begin(); it != m_deqRootViews.end(); ++it)
	{
		if (*it == pRootView)
		{
			m_deqRootViews.erase(it);
			break;
		}
	}
}

void CUILibApp::DelayLayout(CUIRootView *pRootView)
{
	if (m_hWnd == NULL)
		Create(HWND_MESSAGE);

	ATLASSERT(GetWindowThreadProcessId(m_hWnd, NULL) == GetCurrentThreadId());

	for (auto p : m_deqRootViews)
	{
		if (p == pRootView)
			return;
	}

	m_deqRootViews.push_back(pRootView);

	if (!m_bPostMsg)
	{
		m_bPostMsg = true;
		PostMessage(WM_NEEDLAYOUT);
	}
}

LRESULT CUILibApp::OnNeedLayout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LRESULT lResult;

	while (m_deqRootViews.size())
	{
		auto pRootView = m_deqRootViews.front();
		m_deqRootViews.pop_front();
		pRootView->ProcessWindowMessage(NULL, uMsg, 0, 0, lResult);
	}

	m_bPostMsg = false;
	return 0;
}
