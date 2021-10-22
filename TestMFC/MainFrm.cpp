// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "TestMFC.h"
#include "MainFrm.h"
#include "UIDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CUIWindow)

BEGIN_MESSAGE_MAP(CMainFrame, CUIWindow)
	ON_WM_CREATE()
	ON_COMMAND(32779, &CMainFrame::OnShowAbout)
	ON_COMMAND(32780, &CMainFrame::OnTestFunc)
	ON_MESSAGE(WM_USER + 1000, &CMainFrame::OnProgress)
END_MESSAGE_MAP()

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO:  �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}

// CMainFrame ��Ϣ�������

void CMainFrame::OnLoadUI(const IUIXmlAttrs &attrs)
{
	m_bgImagex = UILib::GetImage(attrs.GetStr(L"������"));
	m_bgImagex2 = UILib::GetImage(attrs.GetStr(L"������"));

	m_pViews[0] = m_rootView.SearchCast(L"�ײ�");
	m_pViews[0]->SetHeight(m_bgImagex2.Rect().Height());
	m_pViews[1] = m_rootView.SearchCast(L"�в�");
	m_pViews[2] = m_rootView.SearchCast(L"�в�2");

	CUIMenuButton *pMenuBtn = m_rootView.SearchCast(L"���˵�");
	pMenuBtn->BindPopup([this, pMenuBtn]
	{
		if (m_uiMenu.GetCount() == 0)
		{
			HMENU hMenu = LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
			m_uiMenu.CreateFromMenu(GetSubMenu(hMenu, 0));
			DestroyMenu(hMenu);
		}

		CRect rcBtn = pMenuBtn->GetWindowRect();
		m_uiMenu.Popup(m_hWnd, rcBtn.left, rcBtn.bottom, MAXINT16, rcBtn.top, true);
	});

	CUIButton *pButton = m_rootView.SearchCast(L"��װ");
	pButton->BindClick([this]{ DoSetup(); });

	CUICheckBox *pChkBox = m_rootView.SearchCast(L"check1");
	pChkBox->BindClick([pChkBox, pButton]{ pButton->SetEnabled(pChkBox->IsChecked()); });

	pButton = m_rootView.SearchCast(L"�Զ���");
	pButton->BindClick([this, pButton]{ pButton->SetVisible(false); ShowBottom(true); });

	m_pProgress = m_rootView.SearchCast(L"������");

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

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CUIWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	CenterWindow();
	return 0;
}

void CMainFrame::OnShowAbout()
{
	// TODO:  �ڴ���������������
	CUIResPath resPath(L"����");

	CUIDialog *pWnd = new CUIDialog(true);
	pWnd->CreateFromXml(L"����.xml", m_hWnd);
}

void CMainFrame::OnTestFunc()
{
	// TODO:  �ڴ���������������
	MessageBox(_T("TestLib ���в��Թ���"), NULL, MB_ICONERROR);
}

LRESULT CMainFrame::OnProgress(WPARAM wParam, LPARAM lParam)
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

	CloseHandle((HANDLE)_beginthreadex(NULL, 0, fnThread, m_hWnd, 0, NULL));
}

void CMainFrame::ShowBottom(bool bShow)
{
	m_pViews[0]->SetVisible(bShow);

	int nHeight = m_bgImagex.Rect().Height();
	if (bShow)
		nHeight += m_bgImagex2.Rect().Height() - 12;

	SetWindowPos(NULL, 0, 0, m_bgImagex.Rect().Width(), nHeight, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}
