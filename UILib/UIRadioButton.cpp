#include "stdafx.h"
#include "UIRadioButton.h"

CUIRadioButton::CUIRadioButton(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName)
{
}

CUIRadioButton::~CUIRadioButton()
{
}

void CUIRadioButton::SetCheck(bool bCheck)
{
	if (bCheck)
		bCheck = true;

	if (m_bKeepEnter == bCheck)
		return;

	if (m_bKeepEnter = bCheck)
	{
		GetRootView()->DoMouseEnter(this);
		OnLButtonDown(m_rect.TopLeft());
		OnCheck();
	}
	else
		GetRootView()->RaiseMouseMove();
}

void CUIRadioButton::OnLButtonUp(CPoint point)
{
	if (m_bKeepEnter)
		return;

	m_bKeepEnter = true;
	OnCheck();

	if (m_fnOnClick)
		m_fnOnClick();
}

void CUIRadioButton::OnCheck()
{
	GetParent()->OnRadioCheck(this);
}

void CUIRadioButton::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"check", &nValue))
		SetCheck(nValue != 0);
}
