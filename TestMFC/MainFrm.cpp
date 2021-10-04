// MainFrm.cpp : CMainFrame ���ʵ��
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

// CMainFrame ����/����

CMainFrame::CMainFrame() : m_rootView(this)
{
	// TODO:  �ڴ���ӳ�Ա��ʼ������
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

	// TODO:  �ڴ˴�ͨ���޸�
	// CREATESTRUCT cs ���޸Ĵ��������ʽ
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
	// TODO:  �ڴ����ר�ô����/����û���
	if (m_rootView.ProcessWindowMessage(m_hWnd, message, wParam, lParam, *pResult))
		return TRUE;

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

// CMainFrame ���

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

// CMainFrame ��Ϣ�������

BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
//	return __super::OnNcActivate(bActive);
}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

//	__super::OnNcCalcSize(bCalcValidRects, lpncsp);
}

LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
	// TODO:  �ڴ���������������
	CUIResPath resPath(L"����");

	CUIAboutWnd *pWnd = new CUIAboutWnd;
	pWnd->CreateFromXml(L"����.xml", m_hWnd);
}

void CMainFrame::OnTestFunc()
{
	// TODO:  �ڴ���������������
	MessageBox(_T("TestLib ���в��Թ���"), NULL, MB_ICONERROR);
}

void CMainFrame::OnLoadedUI(const IUIXmlAttrs &attrs)
{
	m_imagexBg = UILib::GetImage(attrs.GetStr(L"������"));
	m_imagexBg2 = UILib::GetImage(attrs.GetStr(L"������"));

	m_pViews[0] = m_rootView.SearchCast(L"�ײ�");
	m_pViews[0]->SetHeight(m_imagexBg2.Rect().Height());
	m_pViews[1] = m_rootView.SearchCast(L"�в�");
	m_pViews[2] = m_rootView.SearchCast(L"�в�2");

	CUIMenuButton *pMenuBtn = m_rootView.SearchCast(L"���˵�");
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

	CUIButton *pButton = m_rootView.SearchCast(L"�ر�");
	pButton->OnClick([this]{ this->SendMessage(WM_SYSCOMMAND, SC_CLOSE); });

	pButton = m_rootView.SearchCast(L"��װ");
	pButton->OnClick([this]{ DoSetup(); });

	CUICheckBox *pChkBox = m_rootView.SearchCast(L"check1");
	pChkBox->OnClick([pButton](bool bCheck){ pButton->SetEnabled(bCheck); });

	pButton = m_rootView.SearchCast(L"�Զ���");
	pButton->OnClick([this, pButton]{ pButton->SetVisible(false); ShowBottom(true); });

	m_pProgress = m_rootView.SearchCast(L"������");
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
