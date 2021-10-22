#include "stdafx.h"
#include "DemoWnd1.h"
#include "UIMoreTools.h"

CDemoWnd1::CDemoWnd1() : m_pWebTabBar(NULL), m_pTabView(NULL)
{
	m_borderImage = UILib::GetImage(L"����1\\������.png");
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

	CUIButton *pButton = pView->AddButton(L"����1\\�ر�.png:4");
	pButton->SetRight(1, true);
	pButton->SetTop(0);
	pButton->BindClick([this]{ this->PostMessage(WM_SYSCOMMAND, SC_CLOSE); });
	pButton->SetToolTip(L"�ر�");

	auto pBtnMax = pView->AddStateButton();
	pBtnMax->SetRight(0, true);
	pBtnMax->SetTop(0);
	pBtnMax->SetId(L"���");
	pBtnMax->AddButton(L"����1\\���.png:4")->SetToolTip(L"���");
	pBtnMax->AddButton(L"����1\\��ԭ.png:4")->SetToolTip(L"���»�ԭ");
	pBtnMax->EndAddChild();
	pBtnMax->BindClick([this](int nState){ this->SendMessage(WM_SYSCOMMAND, nState == 0 ? SC_MAXIMIZE : SC_RESTORE); });

	pButton = pView->AddButton(L"����1\\��С��.png:4");
	pButton->SetRight(0, true);
	pButton->SetTop(0);
	pButton->BindClick([this]{ this->PostMessage(WM_SYSCOMMAND, SC_MINIMIZE); });
	pButton->SetToolTip(L"��С��");

	////////////////////////////////////////////////////////////////////////////////
	// ��ǩ����
	m_pWebTabBar = pView->AddWebTabBar(L"����1\\��ǩ��\\��ǩ.png:4");
	m_pWebTabBar->SetLeft(-2, true);
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
	CUIToolBar *pToolBar = m_rootView.AddToolBar();
	UILib::LoadFromXml(L"����1\\������.xml", pToolBar);

	if (pButton = pToolBar->SearchCast(L"���๤��"))
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
	// ��������
	pView = pClient->AddView();
	pView->SetLeft(300);
	pView->SetRight(300);
	pView->SetTop(10, true);
	pView->SetHeight(30);

	CUISlider *pSlider = pView->AddSlider(L"����1\\����.png:3", L"����1\\����bg.png:2");
	pSlider->SetMaxPos(200);
	pSlider->SetCurPos(50);
	pSlider->BindChange([pSlider]{ ATLTRACE(_T("Slider: %d\n"), pSlider->GetCurPos()); });

	////////////////////////////////////////////////////////////////////////////////
	// ����������
	pView = pClient->AddView();
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

	CUIScrollBar *pVScroll = pView->AddScrollBar(L"����1\\������.png:3", L"����1\\������bg.png");
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
	pView = m_pTabView = pClient->AddView();
	pView->SetLeft(50);
	pView->SetRight(50);
	pView->SetTop(50);
	pView->SetBottom(50);
	pView->SetBgColor(RGB(230, 230, 230));

	// ���4����ǩ
	for (int i = 0; i != 4; i++)
		AddWebTab();
}

void CDemoWnd1::AddWebTab()
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
		auto pBtnMax = dynamic_cast<CUIStateButton *>(m_rootView.Search(L"���", 2));
		if (pBtnMax)
			pBtnMax->SetState(nType != SIZE_RESTORED);
	}

	CRect rect1 = m_rootView.GetRect();
	__super::OnSize(nType, size);
	CRect rect2 = m_rootView.GetRect();

	if (rect1.IsRectEmpty())
		return;

	// ��Ч�ҡ��±߿�
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
