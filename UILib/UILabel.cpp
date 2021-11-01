#include "stdafx.h"
#include "UILabel.h"

CUILabel::CUILabel(CUIView *pParent) : CUIControl(pParent)
{
	m_bClickable = false;
	SetSize({ 0, 0 });
}

void CUILabel::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	OnDrawText(dc, m_rect, DT_END_ELLIPSIS);
}

void CUILabel::OnTextSize(CSize size)
{
	SetSize(size);
}

void CUILabel::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	OnLoadText(attrs);
}
