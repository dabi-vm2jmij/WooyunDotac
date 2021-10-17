#include "stdafx.h"
#include "UICheckButton.h"

CUICheckButton::CUICheckButton(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName), m_bCheck(false)
{
}

CUICheckButton::~CUICheckButton()
{
}

void CUICheckButton::SetCheck(bool bCheck)
{
	if (bCheck)
		bCheck = true;

	if (m_bCheck == bCheck)
		return;

	m_bCheck = bCheck;
	OnButtonState(Normal);

	if (bCheck)
		OnChecked();
}

void CUICheckButton::OnLButtonUp(CPoint point)
{
	SetCheck(!m_bCheck);

	__super::OnLButtonUp(point);
}

void CUICheckButton::OnButtonState(ButtonState btnState)
{
	__super::OnButtonState(m_bCheck ? Press : btnState);
}

void CUICheckButton::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"check", &nValue) && nValue)
		SetCheck(true);
}
