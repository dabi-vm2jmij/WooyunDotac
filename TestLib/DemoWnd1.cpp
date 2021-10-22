#include "stdafx.h"
#include "DemoWnd1.h"
#include "UIMoreTools.h"

CDemoWnd1::CDemoWnd1() : m_pWebTabBar(NULL), m_pTabView(NULL)
{
	m_borderImage = UILib::GetImage(L"例子1\\主背景.png");
	m_nBorderSize = 4;
	m_nCaptionSize = 36;
}

CDemoWnd1::~CDemoWnd1()
{
}

void CDemoWnd1::OnCreate()
{
	__super::OnCreate();

	InitUI();
	CenterWindow();
}

void CDemoWnd1::InitUI()
{
	CUIView *pView = m_rootView.AddView();
	pView->SetTop(0, true);
	pView->SetHeight(36);

	CUIButton *pButton = pView->AddButton(L"例子1\\关闭.png:4");
	pButton->SetRight(1, true);
	pButton->SetTop(0);
	pButton->BindClick([this]{ this->PostMessage(WM_SYSCOMMAND, SC_CLOSE); });
	pButton->SetToolTip(L"关闭");

	auto pBtnMax = pView->AddStateButton();
	pBtnMax->SetRight(0, true);
	pBtnMax->SetTop(0);
	pBtnMax->SetId(L"最大化");
	pBtnMax->AddButton(L"例子1\\最大化.png:4")->SetToolTip(L"最大化");
	pBtnMax->AddButton(L"例子1\\还原.png:4")->SetToolTip(L"向下还原");
	pBtnMax->EndAddChild();
	pBtnMax->BindClick([this](int nState){ this->SendMessage(WM_SYSCOMMAND, nState == 0 ? SC_MAXIMIZE : SC_RESTORE); });

	pButton = pView->AddButton(L"例子1\\最小化.png:4");
	pButton->SetRight(0, true);
	pButton->SetTop(0);
	pButton->BindClick([this]{ this->PostMessage(WM_SYSCOMMAND, SC_MINIMIZE); });
	pButton->SetToolTip(L"最小化");

	////////////////////////////////////////////////////////////////////////////////
	// 标签例子
	m_pWebTabBar = pView->AddWebTabBar(L"例子1\\标签栏\\标签.png:4");
	m_pWebTabBar->SetLeft(-2, true);
	m_pWebTabBar->SetRight(40);
	m_pWebTabBar->SetTabWidth(180);

	CUIButtonEx *pBtnEx = pView->AddButtonEx(L"例子1\\标签栏\\标签.png:4");
	pBtnEx->SetText(L"＋");
	pBtnEx->SetLeft(0);
	pBtnEx->SetBottom(0);
	pBtnEx->SetToolTip(L"添加新标签页");
	pBtnEx->BindClick([this]{ AddWebTab(); });

	////////////////////////////////////////////////////////////////////////////////
	// 工具栏例子
	CUIToolBar *pToolBar = m_rootView.AddToolBar();
	UILib::LoadFromXml(L"例子1\\工具栏.xml", pToolBar);

	if (pButton = pToolBar->SearchCast(L"更多工具"))
	{
		pButton->BindClick([this, pToolBar, pButton]
		{
			vector<CUIView *> vecItems;
			pToolBar->GetMoreItems(vecItems);

			if (vecItems.size())
			{
				CRect rect = pButton->GetWindowRect();
				CUIMoreTools *pWnd = new CUIMoreTools;
				pToolBar->SetMoreWnd(pWnd->Popup(vecItems, m_hWnd, CPoint(rect.left, rect.bottom)));
			}
		});
	}

	CUIView *pClient = m_rootView.AddView();
	pClient->SetBgColor(RGB(255, 255, 255));

	////////////////////////////////////////////////////////////////////////////////
	// 滑块例子
	pView = pClient->AddView();
	pView->SetLeft(300);
	pView->SetRight(300);
	pView->SetTop(10, true);
	pView->SetHeight(30);

	CUISlider *pSlider = pView->AddSlider(L"例子1\\滑块.png:3", L"例子1\\滑块bg.png:2");
	pSlider->SetMaxPos(200);
	pSlider->SetCurPos(50);
	pSlider->BindChange([pSlider]{ ATLTRACE(_T("Slider: %d\n"), pSlider->GetCurPos()); });

	////////////////////////////////////////////////////////////////////////////////
	// 滚动条例子
	pView = pClient->AddView();
	pView->SetRight(30, true);
	pView->SetWidth(200);
	pView->SetTop(10);
	pView->SetBottom(30);
	pView->SetBgColor(RGB(250, 250, 250));

	CUIButtonEx *pBtnAdd = pView->AddButtonEx(L"按钮.png:3");
	pBtnAdd->SetLeft(50);
	pBtnAdd->SetWidth(40);
	pBtnAdd->SetTop(0);
	pBtnAdd->SetText(L"添加");

	CUIButtonEx *pBtnDel = pView->AddButtonEx(L"按钮.png:3");
	pBtnDel->SetRight(50);
	pBtnDel->SetWidth(40);
	pBtnDel->SetTop(0);
	pBtnDel->SetText(L"删除");

	CUIScrollBar *pVScroll = pView->AddScrollBar(L"例子1\\滚动条.png:3", L"例子1\\滚动条bg.png");
	pVScroll->SetRight(10, true);
	pVScroll->SetTop(40);

	CUIScrollView *pView2 = pView->AddScrollView();
	pView2->SetRight(10);
	pView2->SetTop(40);
	pView2->SetBgColor(RGB(240, 240, 240));
	pView2->SetVScroll(pVScroll);

	// 添加模拟数据
	for (int i = 0; i != 20; i++)
	{
		wchar_t szText[64];
		swprintf_s(szText, L"i = %d", i + 1);

		pBtnEx = pView2->AddButtonEx(L"按钮.png:3");
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

		CUIButtonEx *pBtnEx = pView2->AddButtonEx(L"按钮.png:3");
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

	// 客户区
	pView = m_pTabView = pClient->AddView();
	pView->SetLeft(50);
	pView->SetRight(50);
	pView->SetTop(50);
	pView->SetBottom(50);
	pView->SetBgColor(RGB(230, 230, 230));

	// 添加4个标签
	for (int i = 0; i != 4; i++)
		AddWebTab();
}

void CDemoWnd1::AddWebTab()
{
	static int s_nTestId;
	wchar_t szText[64];
	swprintf_s(szText, L"新标签页%d", ++s_nTestId);

	CUIView *pTabPage = m_pTabView->AddView();
	pTabPage->SetVisible(false);
	pTabPage->AddLabel()->SetText(szText);

	CUIWebTab *pWebTab = m_pWebTabBar->AddWebTab();
	pWebTab->SetTabPage(pTabPage);	// 关联页

	CUIButton *pButton = pWebTab->AddButton(L"例子1\\标签栏\\关闭.png:4");
	pButton->SetRight(5, true);
	pButton->BindClick([this, pWebTab]{ DelWebTab(pWebTab); });

	CUILabel *pLabel = pWebTab->AddLabel();
	pLabel->SetLeft(10, true);
	pLabel->SetText(szText);

	m_pWebTabBar->SelectTab(pWebTab);
}

void CDemoWnd1::DelWebTab(CUIWebTab *pWebTab)
{
	if (m_pWebTabBar->GetChilds().size() < 2)
		return;

	CUIView *pTabPage = pWebTab->GetTabPage();
	m_pWebTabBar->DeleteTab(pWebTab);
	pTabPage->GetParent()->DeleteChild(pTabPage);
}

void CDemoWnd1::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 300;

	__super::OnGetMinMaxInfo(lpMMI);
}

void CDemoWnd1::OnSize(UINT nType, CSize size)
{
	if (nType != SIZE_MINIMIZED)
	{
		auto pBtnMax = dynamic_cast<CUIStateButton *>(m_rootView.Search(L"最大化", 2));
		if (pBtnMax)
			pBtnMax->SetState(nType != SIZE_RESTORED);
	}

	CRect rect1 = m_rootView.GetRect();
	__super::OnSize(nType, size);
	CRect rect2 = m_rootView.GetRect();

	if (rect1.IsRectEmpty())
		return;

	// 无效右、下边框
	if (rect1.right != rect2.right)
	{
		CRect rect(rect1.right, 0, rect2.right, size.cy);
		rect.NormalizeRect();
		rect.right += m_nBorderSize;
		InvalidateRect(rect);
	}

	if (rect1.bottom != rect2.bottom)
	{
		CRect rect(0, rect1.bottom, size.cx, rect2.bottom);
		rect.NormalizeRect();
		rect.bottom += m_nBorderSize;
		InvalidateRect(rect);
	}
}
