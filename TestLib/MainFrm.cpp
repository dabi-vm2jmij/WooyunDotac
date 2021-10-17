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
	PostQuitMessage(0);
}

void CMainFrame::OnLoadedUI(const IUIXmlAttrs &attrs)
{
	m_imagexBg = UILib::GetImage(attrs.GetStr(L"背景上"));
	m_imagexBg2 = UILib::GetImage(attrs.GetStr(L"背景下"));

	m_pViews[0] = m_rootView.SearchCast(L"底部");
	m_pViews[0]->SetHeight(m_imagexBg2.Rect().Height());
	m_pViews[1] = m_rootView.SearchCast(L"中部");
	m_pViews[2] = m_rootView.SearchCast(L"中部2");

	CUIMenuButton *pMenuBtn = m_rootView.SearchCast(L"主菜单");
	pMenuBtn->BindGetUIMenu([this]
	{
		if (m_uiMenu.GetCount() == 0)
		{
			HMENU hMenu = LoadMenu(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU1));
			m_uiMenu.CreateFromMenu(GetSubMenu(hMenu, 0));
			DestroyMenu(hMenu);
		}

		return &m_uiMenu;
	});

	CUIButton *pButton = m_rootView.SearchCast(L"安装");
	pButton->BindClick([this]{ DoSetup(); });

	CUICheckBox *pChkBox = m_rootView.SearchCast(L"check1");
	pChkBox->BindClick([pChkBox, pButton]{ pButton->SetEnable(pChkBox->IsChecked()); });

	pButton = m_rootView.SearchCast(L"自定义");
	pButton->BindClick([this, pButton]{ pButton->SetVisible(false); ShowBottom(true); });

	m_pProgress = m_rootView.SearchCast(L"进度条");

	__super::OnLoadedUI(attrs);
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

void CMainFrame::OnDrawBg(CUIDC &dc, LPCRECT lpRect) const
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

	CUISimpleWnd *pWnd = new CUISimpleWnd(true);
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
