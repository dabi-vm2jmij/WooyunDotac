#include "stdafx.h"
#include "MainFrm.h"
#include "TabDemoWnd.h"

CMainFrame::CMainFrame() : m_hThread(NULL)
{
}

CMainFrame::~CMainFrame()
{
	if (m_hThread)
		CloseHandle(m_hThread);
}

void CMainFrame::OnFinalMessage(HWND hWnd)
{
	delete this;
	PostQuitMessage(0);
}

void CMainFrame::OnLoadedUI(const IUILoadAttrs &loaded)
{
	m_imagexBg = UILib::GetImage(loaded.GetStr(L"背景上"));
	m_imagexBg2 = UILib::GetImage(loaded.GetStr(L"背景下"));

	m_rootView.OnDrawBg([this](CUIDC &dc, LPCRECT lpRect){ DrawBg(dc, lpRect); });

	m_pViews[0] = loaded[L"底部"];
	m_pViews[0]->SetHeight(m_imagexBg2.Rect().Height());
	m_pViews[1] = loaded[L"中部"];
	m_pViews[2] = loaded[L"中部2"];

	CUIMenuButton *pMenuBtn = loaded[L"主菜单"];
	pMenuBtn->OnGetUIMenu([this]
	{
		if (m_uiMenu.GetCount() == 0)
		{
			HMENU hMenu = LoadMenu(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU1));
			m_uiMenu.CreateFromMenu(GetSubMenu(hMenu, 0));
			DestroyMenu(hMenu);
		}

		return &m_uiMenu;
	});

	CUIButton *pButton = loaded[L"安装"];
	pButton->OnClick([this]{ DoSetup(); });

	CUICheckBox *pChkBox = loaded[L"check1"];
	pChkBox->OnClick([pButton](bool bCheck){ pButton->SetEnabled(bCheck); });

	pButton = loaded[L"自定义"];
	pButton->OnClick([this, pButton]{ pButton->SetVisible(false); ShowBottom(true); });

	m_pProgress = loaded[L"进度条"];

	__super::OnLoadedUI(loaded);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hIcon = LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_TESTLIB));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	SetWindowRgn(CUIRgn(0, 0, 4000, 3000));
	CenterWindow();
	return 0;
}

void CMainFrame::OnClose()
{
	if (m_hThread && WaitForSingleObject(m_hThread, 0) != WAIT_OBJECT_0)
		return;

	__super::OnClose();
}

LRESULT CMainFrame::OnThreadResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return 0;
}

void CMainFrame::DrawBg(CUIDC &dc, LPCRECT lpRect)
{
	m_imagexBg.BitBlt(dc, lpRect->left, lpRect->top);

	if (lpRect->bottom - lpRect->top > m_imagexBg.Rect().Height())
	{
		m_imagexBg2.BitBlt(dc, lpRect->left, lpRect->bottom - m_imagexBg2.Rect().Height());
	}
}

void CMainFrame::ShowBottom(bool bShow)
{
	m_pViews[0]->SetVisible(bShow);

	int nHeight = m_imagexBg.Rect().Height();
	if (bShow)
		nHeight += m_imagexBg2.Rect().Height() - 12;

	SetWindowPos(NULL, 0, 0, m_imagexBg.Rect().Width(), nHeight, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

LRESULT CMainFrame::OnShowAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	CUIResPath resPath(L"关于");

	CUIPopupWnd *pWnd = new CUIPopupWnd(true);
	pWnd->CreateFromXml(L"关于.xml", m_hWnd);

	return 0;
}

LRESULT CMainFrame::OnTestFunc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	CTabDemoWnd *pWnd = new CTabDemoWnd;
	pWnd->Create(NULL, CRect(0, 0, 1000, 600), _T("TabDemo"), WS_OVERLAPPEDWINDOW);
	pWnd->ShowWindow(SW_SHOW);
	return 0;
}

void CMainFrame::DoSetup()
{
	m_pViews[1]->SetVisible(false);
	m_pViews[2]->SetVisible(true);
	ShowBottom(false);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
}

UINT CMainFrame::WorkThread(LPVOID pParam)
{
	CMainFrame *pMainFrm = (CMainFrame *)pParam;

	for (int i = 0; i <= 100; i++)
	{
		pMainFrm->m_pProgress->SetValue(i);
		Sleep(10);
	}

	pMainFrm->PostMessage(WM_USER + 1000, TRUE);
	return 0;
}
