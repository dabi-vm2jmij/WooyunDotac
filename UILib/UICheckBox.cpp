#include "stdafx.h"
#include "UICheckBox.h"

CUICheckBox::CUICheckBox(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_nSpacing(6), m_bCheck(false)
{
	m_imagex = GetImage(lpFileName);
	SetSize(m_imagex.Rect().Size());
	ATLASSERT(m_imagex.GetFrameCount() > 1);
}

void CUICheckBox::SetSpacing(int nSpacing)
{
	if (m_nSpacing != nSpacing)
	{
		m_nSpacing = nSpacing;
		OnTextSize(m_textSize);
	}
}

void CUICheckBox::SetCheck(bool bCheck)
{
	if (bCheck)
		bCheck = true;

	if (m_bCheck == bCheck)
		return;

	m_bCheck = bCheck;
	m_imagex.SetFrameIndex(bCheck);
	InvalidateRect();

	if (bCheck)
		OnChecked();
}

void CUICheckBox::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	if (m_imagex)
		m_imagex.Draw(dc, m_rect.left, (m_rect.top + m_rect.bottom - m_imagex.Rect().Height()) / 2);

	CRect rect(m_rect);
	rect.top = (rect.top + rect.bottom - m_textSize.cy) / 2;
	rect.bottom = rect.top + m_textSize.cy;
	rect.left += m_imagex.Rect().Width() + m_nSpacing;
	OnDrawText(dc, rect, DT_END_ELLIPSIS);
}

void CUICheckBox::OnLButtonUp(CPoint point)
{
	SetCheck(!m_bCheck);

	if (m_fnOnClick)
		m_fnOnClick();
}

void CUICheckBox::OnTextSize(CSize size)
{
	SetSize({ m_imagex.Rect().Width() + m_nSpacing + size.cx, max(m_imagex.Rect().Height(), size.cy) });
}

void CUICheckBox::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"spacing", &nValue))
		SetSpacing(nValue);

	if (attrs.GetInt(L"check", &nValue) && nValue)
		SetCheck(true);

	OnLoadText(attrs);
}
