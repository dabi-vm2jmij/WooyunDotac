#include "stdafx.h"
#include "TabDemoWnd.h"

CTabDemoWnd::CTabDemoWnd() : m_pWebTabBar(NULL), m_pTabView(NULL)
{
	m_nBorderSize = 4;
	m_nCaptionHeight = 36;

	m_bgImagex = UILib::GetImage(L"����1\\������.png");
	m_bgImagex2 = UILib::GetImage(L"����1\\������bg.png");
}

CTabDemoWnd::~CTabDemoWnd()
{
}

int CTabDemoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitUI();
	CenterWindow();
	return 0;
}

void CTabDemoWnd::InitUI()
{
	CUIView *pView = m_rootView.AddView();
	pView->SetTop(0, true);
	pView->SetHeight(36);

	CUIButton *pButton = pView->AddButton(L"����1\\�ر�.png:4");
	pButton->SetRight(1, true);
	pButton->SetTop(0);
	pButton->BindClick([this]{ this->SendMessage(WM_SYSCOMMAND, SC_CLOSE); });
	pButton->SetToolTip(L"�ر�");

	m_pBtnMax = pView->AddStateButton();
	m_pBtnMax->SetRight(0, true);
	m_pBtnMax->SetTop(0);
	m_pBtnMax->AddButton(L"����1\\���.png:4")->SetToolTip(L"���");
	m_pBtnMax->AddButton(L"����1\\��ԭ.png:4")->SetToolTip(L"���»�ԭ");
	m_pBtnMax->EndAddChild();
	m_pBtnMax->BindClick([this](int nState){ this->SendMessage(WM_SYSCOMMAND, nState == 0 ? SC_MAXIMIZE : SC_RESTORE); });

	pButton = pView->AddButton(L"����1\\��С��.png:4");
	pButton->SetRight(0, true);
	pButton->SetTop(0);
	pButton->BindClick([this]{ this->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE); });
	pButton->SetToolTip(L"��С��");

	////////////////////////////////////////////////////////////////////////////////
	// ��ǩ����
	m_pWebTabBar = pView->AddWebTabBar(L"����1\\��ǩ��\\��ǩ.png:4");
	m_pWebTabBar->SetLeft(m_nBorderSize - 2, true);
	m_pWebTabBar->SetRight(40);
	m_pWebTabBar->SetTabWidth(180);

	CUIButtonEx *pBtnEx = pView->AddButtonEx(L"����1\\��ǩ��\\��ǩ.png:4");
	pBtnEx->SetText(L"��");
	pBtnEx->SetLeft(0);
	pBtnEx->SetBottom(0);
	pBtnEx->SetToolTip(L"����±�ǩҳ");
	pBtnEx->BindClick([this]{ AddWebTab(); });

	////////////////////////////////////////////////////////////////////////////////
	// ����������
	CUIToolBar *pToolBar = m_rootView.AddToolBar(L"���˵�.png:3");
	UILib::LoadFromXml(L"����1\\������.xml", pToolBar);

	////////////////////////////////////////////////////////////////////////////////
	// ��������
	pView = m_rootView.AddView();
	pView->SetLeft(300);
	pView->SetRight(300);
	pView->SetTop(10, true);
	pView->SetHeight(30);

	CUISlider *pSlider = pView->AddSlider(L"����1\\����.png:3", L"����1\\����bg.png:2");
	pSlider->SetMaxPos(200);
	pSlider->SetCurPos(50);
	pSlider->BindChange([](int nPos){ ATLTRACE(_T("Slider: %d\n"), nPos); });

	////////////////////////////////////////////////////////////////////////////////
	// ����������
	pView = m_rootView.AddView();
	pView->SetRight(30, true);
	pView->SetWidth(200);
	pView->SetTop(10);
	pView->SetBottom(30);
	pView->SetBgColor(RGB(250, 250, 250));

	CUIButtonEx *pBtnAdd = pView->AddButtonEx(L"��ť.png:3");
	pBtnAdd->SetLeft(50);
	pBtnAdd->SetWidth(40);
	pBtnAdd->SetTop(0);
	pBtnAdd->SetText(L"���");

	CUIButtonEx *pBtnDel = pView->AddButtonEx(L"��ť.png:3");
	pBtnDel->SetRight(50);
	pBtnDel->SetWidth(40);
	pBtnDel->SetTop(0);
	pBtnDel->SetText(L"ɾ��");

	CUIVScroll *pVScroll = pView->AddVScroll(L"����1\\������.png:3", L"����1\\������bg.png");
	pVScroll->SetRight(10, true);
	pVScroll->SetTop(40);

	CUIScrollView *pView2 = pView->AddScrollView();
	pView2->SetRight(10);
	pView2->SetTop(40);
	pView2->SetBgColor(RGB(240, 240, 240));
	pView2->SetVScroll(pVScroll);

	// ���ģ������
	for (int i = 0; i != 20; i++)
	{
		wchar_t szText[64];
		swprintf_s(szText, L"i = %d", i + 1);

		pBtnEx = pView2->AddButtonEx(L"��ť.png:3");
		pBtnEx->SetTop(16, true);
		pBtnEx->SetTextColor(RGB(51, 51, 51));
		pBtnEx->SetText(szText);
		pBtnEx->SetToolTip(szText);
		pBtnEx->BindClick([pBtnEx]{ pBtnEx->GetParent()->DeleteChild(pBtnEx); });
	}

	pBtnAdd->BindClick([pView2]
	{
		wchar_t szText[64];
		swprintf_s(szText, L"%d", GetTickCount());

		CUIButtonEx *pBtnEx = pView2->AddButtonEx(L"��ť.png:3");
		pBtnEx->SetTop(16, true);
		pBtnEx->SetText(szText);
		pBtnEx->SetTextColor(RGB(51, 51, 51));
	});

	pBtnDel->BindClick([pView2]
	{
		if (pView2->GetChilds().size() == 0)
			return;

		UINT nIndex = GetTickCount() % pView2->GetChilds().size();
		pView2->DeleteChild(pView2->GetChilds()[nIndex]);
	});

	// �ͻ���
	pView = m_pTabView = m_rootView.AddView();
	pView->SetLeft(50);
	pView->SetRight(50);
	pView->SetTop(50);
	pView->SetBottom(50);
	pView->SetBgColor(RGB(230, 230, 230));

	// ���4����ǩ
	for (int i = 0; i != 4; i++)
		AddWebTab();
}

void CTabDemoWnd::AddWebTab()
{
	static int s_nTestId;
	wchar_t szText[64];
	swprintf_s(szText, L"�±�ǩҳ%d", ++s_nTestId);

	CUIView *pTabPage = m_pTabView->AddView();
	pTabPage->SetVisible(false);
	pTabPage->AddLabel()->SetText(szText);

	CUIWebTab *pWebTab = m_pWebTabBar->AddWebTab();
	pWebTab->SetTabPage(pTabPage);	// ����ҳ

	CUIButton *pButton = pWebTab->AddButton(L"����1\\��ǩ��\\�ر�.png:4");
	pButton->SetRight(5, true);
	pButton->BindClick([this, pWebTab]{ DelWebTab(pWebTab); });

	CUILabel *pLabel = pWebTab->AddLabel();
	pLabel->SetLeft(10, true);
	pLabel->SetText(szText);

	m_pWebTabBar->SelectTab(pWebTab);
}

void CTabDemoWnd::DelWebTab(CUIWebTab *pWebTab)
{
	if (m_pWebTabBar->GetChilds().size() < 2)
		return;

	CUIView *pTabPage = pWebTab->GetTabPage();
	m_pWebTabBar->DeleteTab(pWebTab);
	pTabPage->GetParent()->DeleteChild(pTabPage);
}

void CTabDemoWnd::OnDrawBg(CUIDC &dc, LPCRECT lpRect) const
{
	CRect rect(lpRect);
	m_bgImagex.StretchBlt(dc, rect);

	rect.DeflateRect(m_nBorderSize, 36);
	rect.bottom = rect.top + m_bgImagex2.Rect().Height();
	m_bgImagex2.Draw(dc, rect);

	rect.top = rect.bottom;
	rect.bottom = lpRect->bottom - m_nBorderSize;
	dc.FillSolidRect(rect, RGB(255, 255, 255));
}

void CTabDemoWnd::OnSize(UINT nType, CSize size)
{
	__super::OnSize(nType, size);

	if (m_bgSize.cx != size.cx)
	{
		CRect rect(m_bgSize.cx, 0, size.cx, size.cy);
		rect.NormalizeRect();
		rect.left -= m_nBorderSize;
		InvalidateRect(rect);
	}

	if (m_bgSize.cy != size.cy)
	{
		CRect rect(0, m_bgSize.cy, size.cx, size.cy);
		rect.NormalizeRect();
		rect.top -= m_nBorderSize;
		InvalidateRect(rect);
	}

	m_bgSize = size;
}

void CTabDemoWnd::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 800;
	lpMMI->ptMinTrackSize.y = 450;

	__super::OnGetMinMaxInfo(lpMMI);
}

void CTabDemoWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpNCSP)
{
	if (!IsZoomed())
		return;

	// �� WS_CAPTION ʱ��Ҫ
	LPRECT lpRect = lpNCSP->rgrc;
	long nSum = lpRect->top + lpRect->bottom;
	DefWindowProc();
	lpRect->top = nSum - lpRect->bottom;
}
