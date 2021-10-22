#include "stdafx.h"
#include "UILabel.h"

CUILabel::CUILabel(CUIView *pParent) : CUIControl(pParent)
{
	m_bClickable = false;
	SetSize({ 1, 1 });
}

CUILabel::~CUILabel()
{
}

void CUILabel::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	OnDrawText(dc, CRect(m_rect), 0);
}

void CUILabel::OnTextSize(CSize size)
{
	if (size.cx == 0)
		size.SetSize(1, 1);

	SetSize(size);
}

void CUILabel::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	OnLoadText(attrs);
}
