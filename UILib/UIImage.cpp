#include "stdafx.h"
#include "UIImage.h"

CUIImage::CUIImage(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_bClipRgn(true)
{
	m_bClickable = false;

	if (lpFileName)
	{
		m_imagex = GetImage(lpFileName);
		SetSize(m_imagex.Rect().Size());
	}
}

CUIImage::~CUIImage()
{
}

void CUIImage::SetImage(const CImagex &imagex)
{
	if (m_imagex != imagex)
	{
		m_imagex = imagex;
		InvalidateRect();
	}
}

void CUIImage::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	if (m_imagex)
		m_imagex.Draw(dc, m_rect);
}

void CUIImage::CalcRect(LPRECT lpRect, LPRECT lpClipRect)
{
	__super::CalcRect(lpRect, lpClipRect);

	if (m_bClipRgn || m_rect.IsRectEmpty())
		return;

	m_rcReal = m_rect;

	if (lpClipRect)
		UnionRect(lpClipRect, lpClipRect, m_rect);
}

void CUIImage::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"clipRgn", &nValue) && nValue == 0)
		SetNoClipRgn();
}
