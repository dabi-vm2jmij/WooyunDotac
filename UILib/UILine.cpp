#include "stdafx.h"
#include "UILine.h"

CUILine::CUILine(CUIView *pParent) : CUIControl(pParent), m_nStyle(0)
{
	m_bClickable = false;
}

CUILine::~CUILine()
{
}

// 设置线的颜色，实线或虚线
void CUILine::SetLineStyle(COLORREF color, int nStyle)
{
	wchar_t szColor[16];
	swprintf_s(szColor, L"#%06x", BswapRGB(color));

	m_imagex = GetImage(szColor);
	m_nStyle = (WORD)nStyle;
}

void CUILine::MyPaint(CUIDC &dc) const
{
	if (!m_imagex)
		return;

	int nSolid = (BYTE)m_nStyle, nSpace = m_nStyle >> 8;
	if (nSpace == 0)
		nSpace = nSolid;

	if (nSolid == 0 || m_rect.Width() == m_rect.Height())
	{
		m_imagex.Draw(dc, m_rect);
	}
	else if (m_rect.Width() > m_rect.Height())
	{
		// 横线
		for (int nLeft = m_rect.left; nLeft < m_rect.right; nLeft += nSolid + nSpace)
		{
			m_imagex.Draw(dc, nLeft, m_rect.top, min(nSolid, m_rect.right - nLeft), m_rect.Height());
		}
	}
	else
	{
		// 竖线
		for (int nTop = m_rect.top; nTop < m_rect.bottom; nTop += nSolid + nSpace)
		{
			m_imagex.Draw(dc, m_rect.left, nTop, m_rect.Width(), min(nSolid, m_rect.bottom - nTop));
		}
	}
}

void CUILine::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	if (LPCWSTR lpStr = attrs.GetStr(L"color"))
	{
		COLORREF color = 0;
		ATLVERIFY(IsColorStr(lpStr, &color));
		SetLineStyle(color, attrs.GetInt(L"style", m_nStyle));
	}
}
