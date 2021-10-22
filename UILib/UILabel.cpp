#include "stdafx.h"
#include "UILabel.h"

CUILabel::CUILabel(CUIView *pParent) : CUIControl(pParent)
{
	m_bClickable = false;
}

CUILabel::~CUILabel()
{
}

void CUILabel::DoPaint(CUIDC &dc) const
{
	OnDrawText(dc, CRect(m_rect), 0);
}

void CUILabel::OnTextSize(CSize size)
{
	SetSize(size);
}

void CUILabel::OnLoad(const IUIXmlAttrs &attrs)
{
	OnLoadText(attrs);

	__super::OnLoad(attrs);
}
