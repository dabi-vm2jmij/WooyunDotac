#include "stdafx.h"
#include "UIButtonEx.h"
#include "UILibApp.h"

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

// ���� icon �� text ���
void CUIButtonEx::SetTextLeft(int nTextLeft)
{
	m_nTextLeft = nTextLeft;
	OnTextSize(m_sizeText);
}

void CUIButtonEx::DoPaint(CUIDC &dc) const
{
	CRect rect(m_rect);
	const CImagex &curImgxIcon = m_imgxIcons[m_btnState];

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
			InvalidateRect(NULL);
		}
	}

	if (m_imgxIcons[m_btnState] != m_imgxIcons[btnState])
		InvalidateRect(NULL);

	if (m_colors[m_btnState] != m_colors[btnState])
		SetTextColor(m_colors[btnState]);

	__super::OnButtonState(btnState);
}

void CUIButtonEx::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	if (LPCWSTR lpFileName = attrs.GetStr(L"icon"))
		SetIconImage(lpFileName, attrs.GetInt(L"iconLeft", m_nIconLeft));

	int nValue;
	if (attrs.GetInt(L"textLeft", &nValue))
		SetTextLeft(nValue);

	if (attrs.GetInt(L"underline", &nValue) && nValue)
		SetUnderline(true);

	OnLoadedText(attrs);

	// ��̬��ɫ��color0��color1��color2��color3
	COLORREF colors[4] = {};

	for (int i = 0; i != _countof(colors); i++)
	{
		wchar_t szName[64];
		swprintf_s(szName, L"color%d", i);

		if (LPCWSTR lpStr = attrs.GetStr(szName))
		{
			ATLVERIFY(StrToColor(lpStr, colors[i]));
		}
		else
		{
			if (i)
				SetTextColors(colors, i);
			break;
		}
	}
}
