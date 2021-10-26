#include "stdafx.h"
#include "UIRadioButton.h"

CUIRadioButton::CUIRadioButton(CUIView *pParent, LPCWSTR lpFileName) : CUICheckButton(pParent, lpFileName)
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

		if (pRadio && pRadio != this)
			pRadio->SetCheck(false);
	}
}
