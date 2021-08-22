#include "stdafx.h"
#include "UIRadioBox.h"

CUIRadioBox::CUIRadioBox(CUIView *pParent, LPCWSTR lpFileName) : CUICheckBox(pParent, lpFileName)
{
}

CUIRadioBox::~CUIRadioBox()
{
}

void CUIRadioBox::OnLButtonUp(CPoint point)
{
	if (m_bCheck)
		return;

	__super::OnLButtonUp(point);
}

void CUIRadioBox::OnCheckChanged()
{
	__super::OnCheckChanged();

	if (!m_bCheck)
		return;

	GetParent()->OnRadioCheck(this);
}
