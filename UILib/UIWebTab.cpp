#include "stdafx.h"
#include "UIWebTab.h"

CUIWebTab::CUIWebTab(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName), m_bSelect(false), m_pTabPage(NULL)
{
	SetDraggable(true);

	// Í¼Æ¬Ö»ÓÐ2Ì¬Ê±£¬Hover = Normal
	if (m_imagexs[1] == m_imagexs[2])
		m_imagexs[1] = m_imagexs[0];
}

CUIWebTab::~CUIWebTab()
{
}

CUIWebTabBar *CUIWebTab::GetWebTabBar() const
{
	auto pWebTabBar = dynamic_cast<CUIWebTabBar *>(GetParent());
	ATLASSERT(pWebTabBar);
	return pWebTabBar;
}

void CUIWebTab::OnLButtonDown(CPoint point)
{
	__super::OnLButtonDown(point);

	GetWebTabBar()->SelectTab(this);
}

void CUIWebTab::OnButtonState(ButtonState btnState)
{
	__super::OnButtonState(m_bSelect ? Press : btnState);
}

void CUIWebTab::OnSelect(bool bSelect)
{
	m_bSelect = bSelect;
	OnButtonState(Normal);

	if (m_pTabPage)
		m_pTabPage->SetVisible(bSelect);
}
