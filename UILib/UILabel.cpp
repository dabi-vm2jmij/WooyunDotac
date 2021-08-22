#include "stdafx.h"
#include "UILabel.h"

CUILabel::CUILabel(CUIView *pParent) : CUIControl(pParent)
{
	m_bClickable = false;
}

CUILabel::~CUILabel()
{
}

void CUILabel::MyPaint(CUIDC &dc) const
{
	OnDrawText(dc, CRect(m_rect), 0);
}

void CUILabel::OnTextSize(CSize size)
{
	if (m_size != size)
	{
		SetSize(size);
		GetParent()->UpdateChilds();
	}
}

void CUILabel::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	OnLoadedText(attrs);
}
