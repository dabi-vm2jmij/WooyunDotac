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

void CUICheckBox::MyPaint(CUIDC &dc) const
{
	if (m_imagexs[0])
		m_imagexs[m_bCheck].Draw(dc, m_rect.left, m_rect.top + (m_rect.Height() - m_imagexs[0].Rect().Height()) / 2);

	CRect rect(m_rect);
	rect.left += m_nTextLeft + m_imagexs[0].Rect().Width();
	OnDrawText(dc, rect, DT_VCENTER);
}

void CUICheckBox::OnLButtonUp(CPoint point)
{
	OnCheckChanged();

	if (m_fnOnClick)
		m_fnOnClick(m_bCheck);
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

	OnCheckChanged();

	if (m_fnOnClick)
		m_fnOnClick(m_bCheck);
}

void CUICheckBox::OnTextSize(CSize size)
{
	SetSize({ m_imagexs[0].Rect().Width() + m_nTextLeft + size.cx, max(m_imagexs[0].Rect().Height(), size.cy) });
}

void CUICheckBox::OnCheckChanged()
{
	m_bCheck = !m_bCheck;
	InvalidateRect(NULL);
}

void CUICheckBox::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"textLeft", &nValue))
		SetTextLeft(nValue);

	if (attrs.GetInt(L"check", &nValue))
		SetCheck(nValue != 0);

	OnLoadedText(attrs);
}
