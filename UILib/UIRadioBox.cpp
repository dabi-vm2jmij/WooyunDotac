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

void CUIRadioBox::OnChecked()
{
	for (auto pItem : GetParent()->GetChilds())
	{
		auto pRadio = dynamic_cast<CUIRadioBox *>(pItem);

		if (pRadio && pRadio != this && pRadio->m_bCheck)
		{
			pRadio->m_bCheck = false;
			pRadio->InvalidateRect(NULL);
		}
	}
}
