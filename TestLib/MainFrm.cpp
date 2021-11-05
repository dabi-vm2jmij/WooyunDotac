#include "stdafx.h"
#include "MainFrm.h"
#include "DemoWnd1.h"
#include "DemoWnd2.h"

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

void CMainFrame::OnLoadUI(const IUIXmlAttrs &attrs)
{
	m_bgImagex = UILib::GetImage(attrs.GetStr(L"背景上"));
	m_bgImagex2 = UILib::GetImage(attrs.GetStr(L"背景下"));

	m_pViews[0] = m_rootView.SearchCast(L"底部");
	m_pViews[0]->SetHeight(m_bgImagex2.Rect().Height());
	m_pViews[1] = m_rootView.SearchCast(L"中部");
	m_pViews[2] = m_rootView.SearchCast(L"中部2");

	CUIMenuButton *pMenuBtn = m_rootView.SearchCast(L"主菜单");
	pMenuBtn->BindPopup([this, pMenuBtn]
	{
		if (m_uiMenu.GetCount() == 0)
		{
			HMENU hMenu = LoadMenu(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU1));
			m_uiMenu.CreateFromMenu(GetSubMenu(hMenu, 0));
			DestroyMenu(hMenu);
		}

		CRect rcBtn = pMenuBtn->GetWindowRect();
		UINT nCmdId = m_uiMenu.Popup(m_hWnd, rcBtn.left, rcBtn.bottom, MAXINT16, rcBtn.top);
		if (nCmdId)
			PostMessage(WM_COMMAND, nCmdId);
	});

	CUIButton *pButton = m_rootView.SearchCast(L"安装");
	pButton->BindClick([this]{ DoSetup(); });

	CUICheckBox *pChkBox = m_rootView.SearchCast(L"check1");
	pChkBox->BindClick([pChkBox, pButton]{ pButton->SetEnabled(pChkBox->IsChecked()); });

	pButton = m_rootView.SearchCast(L"自定义");
	pButton->BindClick([this, pButton]{ pButton->SetVisible(false); ShowBottom(true); });

	m_pProgress = m_rootView.SearchCast(L"进度条");

	pButton = m_rootView.SearchCast(L"Demo1");
	pButton->BindClick([this]{ ShowDemo1(); });

	pButton = m_rootView.SearchCast(L"Demo2");
	pButton->BindClick([this]{ ShowDemo2(); });

	__super::OnLoadUI(attrs);
}

void CMainFrame::DoPaint(CUIDC &dc) const
{
	CRect rect;
	GetClientRect(rect);
	m_bgImagex.BitBlt(dc, rect.left, rect.top);

	if (m_pViews[0]->IsRealVisible())
		m_bgImagex2.BitBlt(dc, rect.left, rect.bottom - m_bgImagex2.Rect().Height());

	__super::DoPaint(dc);
}

void CMainFrame::OnCreate()
{
	__super::OnCreate();

	HICON hIcon = LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_TESTLIB));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	CenterWindow();
}

void CMainFrame::OnClose()
{
	if (m_hThread && WaitForSingleObject(m_hThread, 0) != WAIT_OBJECT_0)
		return;

	__super::OnClose();
}

LRESULT CMainFrame::OnShowAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	CUIResPath resPath(L"关于");

	CUIDialog *pDlg = new CUIDialog(true);
	pDlg->CreateFromXml(L"关于.xml", m_hWnd);
	pDlg->ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CMainFrame::OnTestFunc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	ShowDemo1();
	return 0;
}

LRESULT CMainFrame::OnProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_pProgress->SetCurPos(wParam);
	return 0;
}

void CMainFrame::DoSetup()
{
	m_pViews[1]->SetVisible(false);
	m_pViews[2]->SetVisible(true);
	ShowBottom(false);

	auto fnThread = [](LPVOID pParam)
	{
		HWND hWnd = (HWND)pParam;

		for (int i = 0; i <= 100; i++)
		{
			::PostMessage(hWnd, WM_USER + 1000, i, 0);
			Sleep(10);
		}

		return 0u;
	};

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, fnThread, m_hWnd, 0, NULL);
}

void CMainFrame::ShowBottom(bool bShow)
{
	m_pViews[0]->SetVisible(bShow);

	int nHeight = m_bgImagex.Rect().Height();
	if (bShow)
		nHeight += m_bgImagex2.Rect().Height() - 12;

	SetWindowPos(NULL, 0, 0, m_bgImagex.Rect().Width(), nHeight, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

void CMainFrame::ShowDemo1()
{
	CDemoWnd1 *pWnd = new CDemoWnd1;
	pWnd->Create(NULL, CRect(0, 0, 1000, 600), _T("Demo1"), WS_OVERLAPPEDWINDOW);
	pWnd->ShowWindow(SW_SHOW);
}

void CMainFrame::ShowDemo2()
{
	CDemoWnd2 *pWnd = new CDemoWnd2;
	pWnd->CreateFromXml(L"例子2\\例子2.xml");
	pWnd->ShowWindow(SW_SHOW);
}
