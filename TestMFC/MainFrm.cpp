// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "TestMFC.h"
#include "MainFrm.h"
#include "UIAboutWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_MESSAGE(WM_USER + 1000, &CMainFrame::OnProgress)
	ON_COMMAND(32779, &CMainFrame::OnShowAbout)
	ON_COMMAND(32780, &CMainFrame::OnTestFunc)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame() : m_rootView(this)
{
	// TODO:  在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	UILib::LoadFromXml(L"Main.xml", &m_rootView);
	CenterWindow();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	// TODO:  在此处通过修改
	// CREATESTRUCT cs 来修改窗口类或样式
	if (cs.hMenu)
	{
		DestroyMenu(cs.hMenu);
		cs.hMenu = NULL;
	}

	cs.cx = 632;
	cs.cy = 370;
	cs.dwExStyle = WS_EX_LAYERED;
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, LoadCursor(NULL, IDC_ARROW));
	return TRUE;
}

BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (m_rootView.ProcessWindowMessage(m_hWnd, message, wParam, lParam, *pResult))
		return TRUE;

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

// CMainFrame 消息处理程序

BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return TRUE;
//	return __super::OnNcActivate(bActive);
}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

//	__super::OnNcCalcSize(bCalcValidRects, lpncsp);
}

LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return HTCAPTION;
//	return __super::OnNcHitTest(point);
}

LRESULT CMainFrame::OnProgress(WPARAM wParam, LPARAM lParam)
{
	m_pProgress->SetValue(wParam);
	return 0;
}

void CMainFrame::OnShowAbout()
{
	// TODO:  在此添加命令处理程序代码
	CUIResPath resPath(L"关于");

	CUIAboutWnd *pWnd = new CUIAboutWnd;
	pWnd->CreateFromXml(L"关于.xml", m_hWnd);
}

void CMainFrame::OnTestFunc()
{
	// TODO:  在此添加命令处理程序代码
	MessageBox(_T("TestLib 中有测试功能"), NULL, MB_ICONERROR);
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
	pMenuBtn->OnGetUIMenu([this]
	{
		if (m_uiMenu.GetCount() == 0)
		{
			HMENU hMenu = LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
			m_uiMenu.CreateFromMenu(GetSubMenu(hMenu, 0));
			DestroyMenu(hMenu);
		}

		return &m_uiMenu;
	});

	CUIButton *pButton = m_rootView.SearchCast(L"关闭");
	pButton->OnClick([this]{ this->SendMessage(WM_SYSCOMMAND, SC_CLOSE); });

	pButton = m_rootView.SearchCast(L"安装");
	pButton->OnClick([this]{ DoSetup(); });

	CUICheckBox *pChkBox = m_rootView.SearchCast(L"check1");
	pChkBox->OnClick([pButton](bool bCheck){ pButton->SetEnabled(bCheck); });

	pButton = m_rootView.SearchCast(L"自定义");
	pButton->OnClick([this, pButton]{ pButton->SetVisible(false); ShowBottom(true); });

	m_pProgress = m_rootView.SearchCast(L"进度条");
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

void CMainFrame::DoSetup()
{
	m_pViews[1]->SetVisible(false);
	m_pViews[2]->SetVisible(true);
	ShowBottom(false);

	CloseHandle((HANDLE)_beginthreadex(NULL, 0, WorkThread, m_hWnd, 0, NULL));
}

UINT CMainFrame::WorkThread(LPVOID pParam)
{
	HWND hWnd = (HWND)pParam;

	for (int i = 0; i <= 100; i++)
	{
		::PostMessage(hWnd, WM_USER + 1000, i, 0);
		Sleep(10);
	}

	return 0;
}
