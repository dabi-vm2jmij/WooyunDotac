#include "stdafx.h"
#include "UIButtonEx.h"
#include "UILibApp.h"

CUIButtonEx::CUIButtonEx(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName), m_nIconLeft(MAXINT16), m_nTextLeft(MAXINT16), m_bUnderline(false)
{
	FillMemory(m_colors, sizeof(m_colors), -1);
}

// 不足4态的图标扩展成4态
static void Fill4Icons(CImagex imagexs[], int nCount)
{
	switch (nCount)
	{
	case 1:
		imagexs[1] = imagexs[0];

	case 2:
		imagexs[2] = imagexs[1];

	case 3:
		imagexs[3] = imagexs[0];
	}
}

void CUIButtonEx::SetIcon(const CImagex &imagex)
{
	int nCount = imagex.GetFrameCount();

	for (int i = 0; i < _countof(m_iconxs) && i < nCount; i++)
	{
		m_iconxs[i] = imagex;
		m_iconxs[i].SetFrameIndex(i);
	}

	Fill4Icons(m_iconxs, nCount);
	OnTextSize(m_textSize);
	InvalidateRect();
}

void CUIButtonEx::SetIcons(const CImagex imagexs[], int nCount)
{
	for (int i = 0; i < _countof(m_iconxs) && i < nCount; i++)
		m_iconxs[i] = imagexs[i];

	Fill4Icons(m_iconxs, nCount);
	OnTextSize(m_textSize);
	InvalidateRect();
}

void CUIButtonEx::SetIconLeft(int nIconLeft)
{
	if (m_nIconLeft != nIconLeft)
	{
		m_nIconLeft = nIconLeft;
		OnTextSize(m_textSize);
		InvalidateRect();
	}
}

void CUIButtonEx::SetTextLeft(int nTextLeft)
{
	if (m_nTextLeft != nTextLeft)
	{
		m_nTextLeft = nTextLeft;
		OnTextSize(m_textSize);
		InvalidateRect();
	}
}

void CUIButtonEx::SetTextColors(const COLORREF colors[], int nCount)
{
	ATLASSERT(nCount > 1);

	for (int i = 0; i < _countof(m_colors) && i < nCount; i++)
		m_colors[i] = colors[i];

	switch (nCount)
	{
	case 1:
		m_colors[1] = m_colors[0];

	case 2:
		m_colors[2] = m_colors[1];

	case 3:
		m_colors[3] = m_colors[0];
	}

	SetTextColor(m_colors[m_btnState]);
}

void CUIButtonEx::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	// 画图标
	if (m_iconxs[m_btnState])
	{
		CRect rect(m_rect);

		if (m_nIconLeft != MAXINT16)
			rect.left += m_nIconLeft;
		else
			rect.left = (rect.left + rect.right - m_iconxs[m_btnState].Rect().Width()) / 2;

		m_iconxs[m_btnState].Draw(dc, rect.left, (rect.top + rect.bottom - m_iconxs[m_btnState].Rect().Height()) / 2);
	}

	// 写文字
	CRect rect(m_rect);
	rect.top = (rect.top + rect.bottom - m_textSize.cy) / 2;
	rect.bottom = rect.top + m_textSize.cy;

	if (m_nTextLeft != MAXINT16)
		rect.left += m_nTextLeft;
	else if (rect.Width() > m_textSize.cx)
		rect.left = (rect.left + rect.right - m_textSize.cx) / 2;

	OnDrawText(dc, rect, DT_END_ELLIPSIS);
}

void CUIButtonEx::OnButtonState(ButtonState btnState)
{
	if (m_bUnderline)
	{
		CUIFontMgr::FontKey fontKey(m_hFont);
		fontKey.m_bUnderline = btnState == Hover || btnState == Press;
		HFONT hFont = g_theApp.GetFontMgr().GetCachedFont(fontKey);

		if (m_hFont != hFont)
		{
			m_hFont = hFont;
			InvalidateRect();
		}
	}

	if (m_iconxs[m_btnState] != m_iconxs[btnState])
		InvalidateRect();

	if (m_colors[m_btnState] != m_colors[btnState])
		SetTextColor(m_colors[btnState]);

	__super::OnButtonState(btnState);
}

void CUIButtonEx::OnTextSize(CSize size)
{
	if (m_imagexs[1])	// 有背景图不用算
		return;

	if (m_nTextLeft != MAXINT16)
		size.cx += m_nTextLeft;

	if (m_iconxs[0])
	{
		CSize size2 = m_iconxs[0].Rect().Size();

		if (m_nIconLeft != MAXINT16)
			size2.cx += m_nIconLeft;

		if (size.cx < size2.cx)
			size.cx = size2.cx;

		if (size.cy < size2.cy)
			size.cy = size2.cy;
	}

	SetSize(size);
}

void CUIButtonEx::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	if (LPCWSTR lpFileName = attrs.GetStr(L"icon"))
		SetIcon(::GetImage(lpFileName));

	int nValue;
	if (attrs.GetInt(L"iconLeft", &nValue))
		SetIconLeft(nValue);

	if (attrs.GetInt(L"textLeft", &nValue))
		SetTextLeft(nValue);

	if (attrs.GetInt(L"underline", &nValue) && nValue)
		SetUnderline(true);

	OnLoadText(attrs);

	// 四态颜色："#aaaaaa,#bbbbbb,#cccccc"
	LPCWSTR lpStr = attrs.GetStr(L"colors");
	if (lpStr == NULL)
		return;

	COLORREF colors[_countof(m_colors)] = {};

	for (int i = 0; i != _countof(colors) && *lpStr == '#'; i++)
	{
		LPWSTR lpEnd = NULL;
		DWORD value = wcstoul(++lpStr, &lpEnd, 16);

		if (lpStr + 6 != lpEnd || *lpEnd && *lpEnd != ',')
			break;

		lpStr = lpEnd + 1;
		colors[i] = BswapRGB(value);

		if (*lpEnd == 0)
		{
			SetTextColors(colors, i + 1);
			return;
		}
	}

	ATLASSERT(0);
}
