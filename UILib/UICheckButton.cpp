#include "stdafx.h"
#include "UICheckButton.h"

CUICheckButton::CUICheckButton(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName)
{
}

CUICheckButton::~CUICheckButton()
{
}

void CUICheckButton::SetCheck(bool bCheck)
{
	if (bCheck)
		bCheck = true;

	if (m_bKeepEnter == bCheck)
		return;

	if (m_bKeepEnter = bCheck)
	{
		DoMouseEnter();
		OnLButtonDown(m_rect.TopLeft());
	}
	else
		GetRootView()->RaiseMouseMove();
}

void CUICheckButton::OnLButtonUp(CPoint point)
{
	if (m_bKeepEnter)
	{
		m_bKeepEnter = false;
		__super::OnLButtonUp(point);
	}
	else
		m_bKeepEnter = true;

	if (m_fnOnClick)
		m_fnOnClick(m_bKeepEnter);
}

void CUICheckButton::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"check", &nValue))
		SetCheck(nValue != 0);
}
