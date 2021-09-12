#include "stdafx.h"
#include "TabDemoWnd.h"

CTabDemoWnd::CTabDemoWnd()
{
	m_nBorderSize = 4;
	m_nCaptionHeight = 36;

	CUIResPath resPath(L"例子1");

	m_imagexBg = UILib::GetImage(L"主背景.png");
	m_imagexBg2 = UILib::GetImage(L"工具栏bg.png");
}

CTabDemoWnd::~CTabDemoWnd()
{
}

int CTabDemoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowRgn(CUIRgn(0, 0, 4000, 3000));

	InitUI();
	CenterWindow();
	return 0;
}

void CTabDemoWnd::InitUI()
{
	CUIResPath resPath(L"例子1");

	CUIView *pView = m_rootView.AddView();
	pView->SetTop(0, true);
	pView->SetHeight(36);

	CUIButton *pButton = pView->AddButton(L"关闭.png:4");
	pButton->SetRight(1, true);
	pButton->SetTop(0);
	pButton->OnClick([this]{ this->SendMessage(WM_SYSCOMMAND, SC_CLOSE); });
	pButton->SetToolTip(L"关闭");

	m_pBtnMax = pView->AddStateButton();
	m_pBtnMax->SetRight(0, true);
	m_pBtnMax->SetTop(0);
	m_pBtnMax->AddButton(L"最大化.png:4")->SetToolTip(L"最大化");
	m_pBtnMax->AddButton(L"还原.png:4")->SetToolTip(L"还原");
	m_pBtnMax->EndAddChild();
	m_pBtnMax->OnClick([this](int nState){ this->SendMessage(WM_SYSCOMMAND, nState == 0 ? SC_MAXIMIZE : SC_RESTORE); });

	pButton = pView->AddButton(L"最小化.png:4");
	pButton->SetRight(0, true);
	pButton->SetTop(0);
	pButton->OnClick([this]{ this->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE); });
	pButton->SetToolTip(L"最小化");

	////////////////////////////////////////////////////////////////////////////////
	// 标签例子
	m_pWebTabBar = pView->AddWebTabBar(L"标签栏\\标签.png:4");
	m_pWebTabBar->SetLeft(m_nBorderSize - 2);
	m_pWebTabBar->SetTabWidth(180);

	CUIWebTab *pWebTab = m_pWebTabBar->AddWebTab();
	CUILabel *pLabel = pWebTab->AddLabel();
	pLabel->SetLeft(10, true);
	pLabel->SetText(L"新标签页");

	for (int i = 1; i != 5; i++)
	{
		pWebTab = m_pWebTabBar->AddWebTab();
		pButton = pWebTab->AddButton(L"标签栏\\关闭.png:4");
		pButton->SetRight(5, true);

		pButton->OnClick([pWebTab]
		{
			CUIWebTabBar *pWebTabBar = pWebTab->GetWebTabBar();
			pWebTabBar->RemoveChild(pWebTab);
		});
	}

	m_pWebTabBar->ActivateTab(pWebTab);

	////////////////////////////////////////////////////////////////////////////////
	// 工具栏例子
	pView = m_rootView.AddView();
	pView->SetTop(0, true);
	pView->SetHeight(34);

	pView->AddBlank()->SetLeft(50, true);
	pView->AddBlank()->SetRight(50, true);

	CUIToolBar *pToolBar = pView->AddToolBar(L"\\主菜单.png:3");
	UILib::LoadFromXml(L"工具栏.xml", pToolBar);

	////////////////////////////////////////////////////////////////////////////////
	// 滑块例子
	pView = m_rootView.AddView();
	pView->SetTop(10, true);
	pView->SetHeight(30);

	pView->AddBlank()->SetLeft(300, true);
	pView->AddBlank()->SetRight(300, true);

	CUISlider *pSlider = pView->AddSlider(L"滑块.png:3", L"滑块bg.png:2");
	pSlider->SetMaxPos(200);
	pSlider->SetCurPos(50);
	pSlider->OnChanged([](int nPos){ ATLTRACE(_T("Changed: %d\n"), nPos); });

	////////////////////////////////////////////////////////////////////////////////
	// 滚动条例子
	pView = m_rootView.AddView();
	pView->AddBlank()->SetLeft(80, true);
	pView->AddBlank()->SetRight(30, true);
	pView->AddBlank()->SetTop(20, true);
	pView->AddBlank()->SetBottom(50, true);

	CUIButtonEx *pBtnDel = pView->AddButtonEx(L"\\按钮.png:3");
	pBtnDel->SetRight(0, true);
	pBtnDel->SetWidth(40);
	pBtnDel->SetText(L"删除");

	CUIButtonEx *pBtnAdd = pView->AddButtonEx(L"\\按钮.png:3");
	pBtnAdd->SetRight(10, true);
	pBtnAdd->SetWidth(40);
	pBtnAdd->SetText(L"添加");

	CUIVScroll *pVScroll = pView->AddVScroll(L"滚动条.png:3", L"滚动条bg.png");
	pVScroll->SetRight(30, true);

	CUIScrollView *pView2 = pView->AddScrollView();
	pView2->SetVScroll(pVScroll);

	// 添加模拟数据
	for (int i = 0; i != 20; i++)
	{
		wchar_t szText[64];
		swprintf_s(szText, L"i = %d", i + 1);

		CUIButtonEx *pButton = pView2->AddButtonEx(L"\\按钮.png:3");
		pButton->SetTop(16, true);
		pButton->SetText(szText);
		pButton->SetTextColor(RGB(51, 51, 51));
	}

	pBtnAdd->OnClick([pView2]
	{
		wchar_t szText[64];
		swprintf_s(szText, L"%d", GetTickCount());

		CUIButtonEx *pButton = pView2->AddButtonEx(L"\\按钮.png:3");
		pButton->SetTop(16, true);
		pButton->SetText(szText);
		pButton->SetTextColor(RGB(51, 51, 51));
	});

	pBtnDel->OnClick([pView2]
	{
		if (pView2->GetCount() == 0)
			return;

		int nIndex = GetTickCount() % pView2->GetCount();
		pView2->RemoveChild(pView2->GetChild(nIndex));
	});
}

void CTabDemoWnd::OnDrawBg(CUIDC &dc, LPCRECT lpRect)
{
	CRect rect(lpRect);
	m_imagexBg.StretchBlt(dc, rect);

	rect.DeflateRect(m_nBorderSize, 36);
	rect.bottom = rect.top + m_imagexBg2.Rect().Height();
	m_imagexBg2.Draw(dc, rect);

	rect.top = rect.bottom;
	rect.bottom = lpRect->bottom - m_nBorderSize;
	dc.FillSolidRect(rect, RGB(255, 255, 255));
}

void CTabDemoWnd::OnSize(UINT nType, CSize size)
{
	__super::OnSize(nType, size);

	if (m_sizeBg.cx != size.cx)
	{
		CRect rect(m_sizeBg.cx, 0, size.cx, min(m_sizeBg.cy, size.cy));
		rect.NormalizeRect();
		rect.left -= m_nBorderSize;
		InvalidateRect(rect);
	}

	if (m_sizeBg.cy != size.cy)
	{
		CRect rect(0, m_sizeBg.cy, min(m_sizeBg.cx, size.cx), size.cy);
		rect.NormalizeRect();
		rect.top -= m_nBorderSize;
		InvalidateRect(rect);
	}

	m_sizeBg = size;
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

	long nSum = lpNCSP->rgrc[0].top + lpNCSP->rgrc[0].bottom;
	__super::OnNcCalcSize(bCalcValidRects, lpNCSP);
	lpNCSP->rgrc[0].top = nSum - lpNCSP->rgrc[0].bottom;
}
