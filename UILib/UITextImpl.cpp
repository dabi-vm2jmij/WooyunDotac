#include "stdafx.h"
#include "UITextImpl.h"
#include "UIFontMgr.h"

CUITextImpl::CUITextImpl() : m_bTextEx(false), m_hFont(GetDefaultFont()), m_color(0)
{
}

CUITextImpl::~CUITextImpl()
{
}

void CUITextImpl::OnDrawText(CUIDC &dc, LPRECT lpRect, UINT nFormat) const
{
	if (m_strText.empty() || IsRectEmpty(lpRect))
		return;

	if (m_bTextEx)
	{
		CRect rect(lpRect);

		if (nFormat & DT_CENTER)
		{
			rect.left = (rect.left + rect.right - m_sizeText.cx) / 2;
			rect.right = rect.left + m_sizeText.cx;
		}
		else if (nFormat & DT_RIGHT)
		{
			rect.left = rect.right - m_sizeText.cx;
		}

		if (nFormat & DT_VCENTER)
		{
			rect.top = (rect.top + rect.bottom - m_sizeText.cy) / 2;
			rect.bottom = rect.top + m_sizeText.cy;
		}
		else if ((nFormat & DT_BOTTOM) == 0)	// DT_TOP
		{
			rect.bottom = rect.top + m_sizeText.cy;
		}

		OnDrawTextEx(dc, rect, NULL);
	}
	else
	{
		dc.SelectFont(m_hFont);
		dc.SetTextColor(m_color);
		DrawTextW(dc, m_strText.c_str(), -1, lpRect, nFormat | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);
	}

	if (dc.IsLayered())
		FillAlpha(dc, lpRect, 255);
}

/*	格式说明
	Font：      {f 宋体:12}text{/}  可省略为：{f 宋体}、{f :12}
	Weight：    {w n}text{/}        n（1～9）：4=Normal, 7=Bold
	Bold：      {b}text{/}          等于：{w 7}
	Italic：    {i}text{/}
	Underline： {u}text{/}
	Color：     {#xxxxxx}text{/}
	Space：     {s n}text           n（1～9999）
	可嵌套为：  {f 宋体:12}{w 7}{i}{u}{c 121212}text{/////}
*/

namespace
{

struct DrawInfo
{
	CUIFontMgr::FontKey m_fontKey;
	COLORREF m_color;

	DrawInfo(HFONT hFont, COLORREF color) : m_fontKey(hFont), m_color(color) {}
};

void MyDrawText(const DrawInfo &drawInfo, CUIDC &dc, LPCWSTR lpText, int nSize, LPRECT lpRect, LPSIZE lpSize)
{
	dc.SelectFont(CUIFontMgr::Get().GetFont(CUIFontMgr::FontKey(drawInfo.m_fontKey)));

	CRect rect;
	DrawTextW(dc, lpText, nSize, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);

	if (lpSize)
	{
		lpSize->cx += rect.Width();

		if (lpSize->cy < rect.Height())
			lpSize->cy = rect.Height();
	}
	else
	{
		dc.SetTextColor(drawInfo.m_color);
		DrawTextW(dc, lpText, nSize, lpRect, DT_NOPREFIX | DT_SINGLELINE | DT_BOTTOM);
		lpRect->left += rect.Width();
	}
}

UINT CountNumber(LPCWSTR lpStr, bool bHex)
{
	UINT nCount = 0;

	while (int ch = lpStr[nCount++])
	{
		if (ch >= '0' && ch <= '9' || bHex && (ch >= 'A' && ch <= 'F' || ch >= 'a' && ch <= 'f'))
			;
		else
			return -1;
	}

	return nCount - 1;
}

UINT CountSlash(LPCWSTR lpStr)
{
	UINT nCount = 0;

	while (int ch = lpStr[nCount++])
	{
		if (ch != '/')
			return 0;
	}

	return nCount - 1;
}

}	// namespace

void CUITextImpl::OnDrawTextEx(CUIDC &dc, LPRECT lpRect, LPSIZE lpSize) const
{
	dc.SelectFont(m_hFont);
	std::vector<DrawInfo> vecDrawInfos;
	DrawInfo drawInfo((HFONT)GetCurrentObject(dc, OBJ_FONT), m_color);

	for (LPCWSTR lpHead = m_strText.c_str(), lpTail; *lpHead; lpHead = lpTail + 1)
	{
		if (lpSize == NULL && IsRectEmpty(lpRect))
			break;

		if ((lpTail = wcschr(lpHead, '{')) == NULL)
		{
			ATLASSERT(vecDrawInfos.empty());
			MyDrawText(drawInfo, dc, lpHead, -1, lpRect, lpSize);
			break;
		}

		if (lpTail[1] == '{')
		{
			MyDrawText(drawInfo, dc, lpHead, ++lpTail - lpHead, lpRect, lpSize);
			continue;
		}

		if (lpHead != lpTail)
		{
			MyDrawText(drawInfo, dc, lpHead, lpTail - lpHead, lpRect, lpSize);
			lpHead = lpTail;
		}

		if ((lpTail = wcschr(lpHead, '}')) == NULL)
		{
			ATLASSERT(0);
			break;
		}

		wchar_t szText[64];
		UINT nSize = lpTail - lpHead;

		if (nSize > _countof(szText))
		{
			ATLASSERT(0);
			continue;
		}

		wcsncpy_s(szText, nSize, lpHead + 1, nSize - 1);
		nSize = 0;

		switch (szText[0])
		{
		case 'f':
			if (szText[1] == ' ' && szText[2])
			{
				nSize = 1;
				vecDrawInfos.push_back(drawInfo);
				drawInfo.m_fontKey.ParseName(szText + 2);
			}
			break;

		case 'w':
			if (szText[1] == ' ' && CountNumber(szText + 2, false) == 1)
			{
				nSize = szText[2] - '0';
				vecDrawInfos.push_back(drawInfo);
				drawInfo.m_fontKey.m_nWeight = nSize * 100;
			}
			break;

		case 'b':
			if (szText[1] == 0)
			{
				nSize = 1;
				vecDrawInfos.push_back(drawInfo);
				drawInfo.m_fontKey.m_nWeight = FW_BOLD;
			}
			break;

		case 'i':
			if (szText[1] == 0)
			{
				nSize = 1;
				vecDrawInfos.push_back(drawInfo);
				drawInfo.m_fontKey.m_bItalic = TRUE;
			}
			break;

		case 'u':
			if (szText[1] == 0)
			{
				nSize = 1;
				vecDrawInfos.push_back(drawInfo);
				drawInfo.m_fontKey.m_bUnderline = TRUE;
			}
			break;

		case '#':
			if (CountNumber(szText + 1, true) == 6)
			{
				vecDrawInfos.push_back(drawInfo);
				nSize = IsStrColor(szText, &drawInfo.m_color);
			}
			break;

		case 's':
			if (szText[1] == ' ' && CountNumber(szText + 2, false) <= 4)
			{
				nSize = _wtoi(szText + 2);

				if (lpSize)
					lpSize->cx += nSize;
				else
					lpRect->left += nSize;
			}
			break;

		case '/':
			if (nSize = CountSlash(szText))
			{
				if (nSize > vecDrawInfos.size())
				{
					ATLASSERT(0);
					nSize = vecDrawInfos.size();
				}

				auto it = vecDrawInfos.end() - nSize;
				drawInfo = *it;
				vecDrawInfos.erase(it, vecDrawInfos.end());
			}
			break;
		}

		ATLASSERT(nSize);
	}
}

void CUITextImpl::SetText(LPCWSTR lpText)
{
	DoSetText(lpText, false);
}

void CUITextImpl::SetTextEx(LPCWSTR lpText)
{
	DoSetText(lpText, true);
}

void CUITextImpl::DoSetText(LPCWSTR lpText, bool bTextEx)
{
	if (lpText == NULL)
		lpText = L"";

	if (m_strText == lpText && m_bTextEx == bTextEx)
		return;

	m_strText = lpText;
	m_bTextEx = bTextEx;
	RecalcSize();
	Invalidate();
}

void CUITextImpl::SetFont(HFONT hFont)
{
	ATLASSERT(hFont);
	if (m_hFont == hFont || hFont == NULL)
		return;

	m_hFont = hFont;
	RecalcSize();
	Invalidate();
}

void CUITextImpl::SetTextColor(COLORREF color)
{
	if (m_color == color)
		return;

	m_color = color;
	Invalidate();
}

void CUITextImpl::Invalidate()
{
	if (m_sizeText.cx == 0 || m_sizeText.cy == 0)
		return;

	CUIControl *pCtrl = dynamic_cast<CUIControl *>(this);
	ATLASSERT(pCtrl);
	pCtrl->InvalidateRect(NULL);
}

void CUITextImpl::RecalcSize()
{
	CSize size;

	if (m_strText.size())
	{
		if (m_bTextEx)
		{
			OnDrawTextEx(CUIDC(CUIComDC(NULL), NULL, false), NULL, &size);
		}
		else
		{
			CRect rect;
			DrawTextW(CUIComDC(m_hFont), m_strText.c_str(), -1, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
			size = rect.Size();
		}
	}

	if (m_sizeText != size)
		OnTextSize(m_sizeText = size);
}

void CUITextImpl::OnLoadedText(const IUILoadAttrs &attrs)
{
	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"font"))
		SetFont(attrs.GetFont(lpStr));

	if (lpStr = attrs.GetStr(L"color"))
	{
		COLORREF color = 0;
		ATLVERIFY(IsStrColor(lpStr, &color));
		SetTextColor(color);
	}

	if (lpStr = attrs.GetStr(L"textEx"))
		SetTextEx(lpStr);
	else if (lpStr = attrs.GetStr(L"text"))
		SetText(lpStr);
}
