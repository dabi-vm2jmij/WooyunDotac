#include "stdafx.h"
#include "UIWebTab.h"

CUIWebTab::CUIWebTab(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName)
{
	SetStretch(true);
}

CUIWebTab::~CUIWebTab()
{
}

CUIWebTabBar *CUIWebTab::GetWebTabBar() const
{
	CUIWebTabBar *pWebTabBar = dynamic_cast<CUIWebTabBar *>(GetParent());
	ATLASSERT(pWebTabBar);
	return pWebTabBar;
}

bool CUIWebTab::DoMouseLeave(bool bForce)
{
	bool bRet = __super::DoMouseLeave(bForce);
	m_bLButtonDown = m_bRButtonDown = false;	// 清除鼠标状态，防止不能拖动标签
	return bRet;
}

void CUIWebTab::OnLButtonDown(CPoint point)
{
	__super::OnLButtonDown(point);

	m_bKeepEnter = true;
	OnActivate(true);
	GetWebTabBar()->ActivateTab(this);

	if (m_fnOnClick)
		m_fnOnClick();
}

void CUIWebTab::SetActive(bool bActive)
{
	if (bActive)
		bActive = true;

	if (m_bKeepEnter == bActive)
		return;

	if (m_bKeepEnter = bActive)
	{
		DoMouseEnter();
		OnLButtonDown(m_rect.TopLeft());
	}
	else
	{
		OnActivate(false);
		OnDrawState(1);
		GetRootView()->RaiseMouseMove();
	}
}
