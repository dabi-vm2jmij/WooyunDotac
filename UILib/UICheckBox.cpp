#include "stdafx.h"
#include "UICheckBox.h"

CUICheckBox::CUICheckBox(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_nTextLeft(6), m_bCheck(false)
{
	SplitImage(lpFileName, m_imagexs);
	SetSize(m_imagexs[0].Rect().Size());
}

CUICheckBox::~CUICheckBox()
{
}

void CUICheckBox::SetTextLeft(int nTextLeft)
{
	m_nTextLeft = nTextLeft;
	OnTextSize(m_sizeText);
}

void CUICheckBox::SetCheck(bool bCheck)
{
	if (bCheck)
		bCheck = true;

	if (m_bCheck == bCheck)
		return;

	m_bCheck = bCheck;
	InvalidateRect(NULL);

	if (bCheck)
		OnChecked();
}

void CUICheckBox::DoPaint(CUIDC &dc) const
{
	if (m_imagexs[0])
		m_imagexs[m_bCheck].Draw(dc, m_rect.left, m_rect.top + (m_rect.Height() - m_imagexs[0].Rect().Height()) / 2);

	CRect rect(m_rect);
	rect.left += m_nTextLeft + m_imagexs[0].Rect().Width();
	OnDrawText(dc, rect, DT_VCENTER);
}

void CUICheckBox::OnLButtonUp(CPoint point)
{
	SetCheck(!m_bCheck);

	if (m_fnOnClick)
		m_fnOnClick();
}

void CUICheckBox::OnTextSize(CSize size)
{
	SetSize({ m_imagexs[0].Rect().Width() + m_nTextLeft + size.cx, max(m_imagexs[0].Rect().Height(), size.cy) });
}

void CUICheckBox::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"textLeft", &nValue))
		SetTextLeft(nValue);

	if (attrs.GetInt(L"check", &nValue) && nValue)
		SetCheck(true);

	OnLoadedText(attrs);
}
