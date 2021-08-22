#include "stdafx.h"
#include "UIButtonEx.h"
#include "UIFontMgr.h"

CUIButtonEx::CUIButtonEx(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName), m_nIconLeft(0), m_nTextLeft(MAXINT16), m_bUnderline(false)
{
	FillMemory(m_colors, sizeof(m_colors), -1);
	SetStretch(true);
}

CUIButtonEx::~CUIButtonEx()
{
}

void CUIButtonEx::SetIconImage(LPCWSTR lpFileName, int nIconLeft)
{
	Fill4States(m_imgxIcons, SplitImage(lpFileName, m_imgxIcons));

	if (nIconLeft >= 0)
		m_nIconLeft = nIconLeft;

	if (m_nTextLeft == MAXINT16)
		m_nTextLeft = 0;

	OnTextSize(m_sizeText);
}

void CUIButtonEx::SetTextColors(COLORREF *colors, int nCount)
{
	for (int i = 0; i < nCount && i < 4; i++)
		m_colors[i] = colors[i];

	for (int i = max(1, nCount); i < 4; i++)
		m_colors[i] = m_colors[0];

	if (nCount == 2)
		m_colors[2] = m_colors[1];

	SetTextColor(m_colors[0]);
}

// 设置 icon 与 text 间距
void CUIButtonEx::SetTextLeft(int nTextLeft)
{
	m_nTextLeft = nTextLeft;
	OnTextSize(m_sizeText);
}

void CUIButtonEx::MyPaint(CUIDC &dc) const
{
	CRect rect(m_rect);
	const CImagex &curImgxIcon = m_imgxIcons[m_nDrawState];

	if (curImgxIcon)
	{
		rect.left += m_nIconLeft;
		curImgxIcon.Draw(dc, rect.left, rect.top + (rect.Height() - curImgxIcon.Rect().Height()) / 2);
		rect.left += curImgxIcon.Rect().Width();
	}

	UINT nFormat = DT_VCENTER;

	if (rect.left != m_rect.left || m_nTextLeft != MAXINT16)
		rect.left += m_nTextLeft;
	else
		nFormat |= DT_CENTER;

	OnDrawText(dc, rect, nFormat);
}

void CUIButtonEx::OnMouseEnter()
{
	__super::OnMouseEnter();

	if (m_bUnderline)
	{
		CUIFontMgr::FontKey fontKey(m_hFont);
		fontKey.m_bUnderline = true;
		m_hFont = CUIFontMgr::Get().GetFont(fontKey);
		InvalidateRect(NULL);
	}
}

void CUIButtonEx::OnMouseLeave()
{
	if (m_bUnderline)
	{
		CUIFontMgr::FontKey fontKey(m_hFont);
		fontKey.m_bUnderline = false;
		m_hFont = CUIFontMgr::Get().GetFont(fontKey);
		InvalidateRect(NULL);
	}

	__super::OnMouseLeave();
}

void CUIButtonEx::OnTextSize(CSize size)
{
	if (m_imagexs[0])
		return;

	if (m_imgxIcons[0])
	{
		size.cx += m_nIconLeft + m_imgxIcons[0].Rect().Width() + m_nTextLeft;

		if (size.cy < m_imgxIcons[0].Rect().Height())
			size.cy = m_imgxIcons[0].Rect().Height();
	}
	else if (m_nTextLeft != MAXINT16)
	{
		size.cx += m_nTextLeft;
	}

	SetSize(size);
}

void CUIButtonEx::OnDrawState(UINT nState)
{
	if (m_imgxIcons[m_nDrawState] != m_imgxIcons[nState])
		InvalidateRect(NULL);

	if (m_colors[m_nDrawState] != m_colors[nState])
		SetTextColor(m_colors[nState]);

	__super::OnDrawState(nState);
}

void CUIButtonEx::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	if (LPCWSTR lpFileName = attrs.GetStr(L"icon"))
		SetIconImage(lpFileName, attrs.GetInt(L"iconLeft", m_nIconLeft));

	int nValue;
	if (attrs.GetInt(L"textLeft", &nValue))
		SetTextLeft(nValue);

	if (attrs.GetInt(L"underline", &nValue))
		SetUnderline(nValue != 0);

	OnLoadedText(attrs);

	// 四态颜色：color0、color1、color2、color3
	COLORREF colors[4] = {};

	for (int i = 0; i != _countof(colors); i++)
	{
		wchar_t szName[64];
		swprintf_s(szName, L"color%d", i);

		if (LPCWSTR lpStr = attrs.GetStr(szName))
		{
			ATLVERIFY(IsColorStr(lpStr, &colors[i]));
		}
		else
		{
			if (i)
				SetTextColors(colors, i);
			break;
		}
	}
}
