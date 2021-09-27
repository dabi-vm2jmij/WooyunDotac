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
	SetSize(size);
}

void CUILabel::OnLoaded(const IUILoadAttrs &attrs)
{
	OnLoadedText(attrs);

	__super::OnLoaded(attrs);
}
