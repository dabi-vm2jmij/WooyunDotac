#include "stdafx.h"
#include "UIRadioButton.h"

CUIRadioButton::CUIRadioButton(CUIView *pParent, LPCWSTR lpFileName) : CUICheckButton(pParent, lpFileName)
{
}

CUIRadioButton::~CUIRadioButton()
{
}

void CUIRadioButton::OnLButtonUp(CPoint point)
{
	if (m_bCheck)
		return;

	__super::OnLButtonUp(point);
}

void CUIRadioButton::OnChecked()
{
	for (auto pItem : GetParent()->GetChilds())
	{
		auto pRadio = dynamic_cast<CUIRadioButton *>(pItem);

		if (pRadio && pRadio != this && pRadio->m_bCheck)
		{
			pRadio->m_bCheck = false;
			pRadio->OnButtonState(Normal);
		}
	}
}
