#include "stdafx.h"
#include "UITextImpl.h"
#include "UILibApp.h"

#define BRACE_L		2
#define BRACE_R		3

CUITextImpl::CUITextImpl() : m_hFont(GetDefaultFont()), m_color(0), m_nMaxWidth(MAXINT16)
{
}

CUITextImpl::~CUITextImpl()
{
}

void CUITextImpl::OnDrawText(CUIDC &dc, LPRECT lpRect, UINT nFormat) const
{
	if (m_strText.empty() || IsRectEmpty(lpRect))
		return;

	if (wcschr(m_strText.c_str(), BRACE_L))
	{
		CRect rect(lpRect);

		if (nFormat & DT_CENTER)
			rect.left = (rect.left + rect.right - m_textSize.cx) / 2;
		else if (nFormat & DT_RIGHT)
			rect.left = rect.right - m_textSize.cx;

		if (nFormat & DT_VCENTER)
			rect.bottom = (rect.top + rect.bottom + m_textSize.cy) / 2;
		else if ((nFormat & DT_BOTTOM) == 0)	// DT_TOP
			rect.bottom = rect.top + m_textSize.cy;

		OnDrawTextEx(dc, rect, NULL);
	}
	else
	{
		SelectObject(dc, m_hFont);
		::SetTextColor(dc, m_color);
		DrawTextW(dc, m_strText.c_str(), -1, lpRect, nFormat | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);
	}

	// DrawText 后填充 alpha
	dc.FillAlpha(lpRect, 255);
}

/*	格式说明
	Font：      {f 宋体:12}text{/}  可省略为：{f 宋体}、{f :12}
	Weight：    {w N}text{/}        N（1～9）：4=Normal, 7=Bold
	Bold：      {b}text{/}          等于：{w 7}
	Italic：    {i}text{/}
	Underline： {u}text{/}
	Color：     {#xxxxxx}text{/}
	Space：     {N}text             N（1～999）
	可嵌套为：  {f 宋体:12}{b}{i}{u}{#121212}text{/////}
*/

namespace
{

struct DrawInfo
{
	CUIFontMgr::FontKey m_fontKey;
	COLORREF m_color;

	DrawInfo(HFONT hFont, COLORREF color) : m_fontKey(hFont), m_color(color) {}
};

void MyDrawText(const DrawInfo &drawInfo, HDC hDC, LPCWSTR lpText, int nSize, LPRECT lpRect, LPSIZE lpSize)
{
	SelectObject(hDC, g_theApp.GetFontMgr().GetCachedFont(drawInfo.m_fontKey));

	CRect rect;
	DrawTextW(hDC, lpText, nSize, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);

	if (lpSize)
	{
		lpSize->cx += rect.Width();

		if (lpSize->cy < rect.Height())
			lpSize->cy = rect.Height();
	}
	else
	{
		::SetTextColor(hDC, drawInfo.m_color);
		DrawTextW(hDC, lpText, nSize, lpRect, DT_NOPREFIX | DT_SINGLELINE | DT_BOTTOM);
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

bool Transcode(LPCWSTR lpSrc, LPWSTR lpDst)
{
	LPCWSTR lpExt = NULL;
	int  nSteps = 0;
	bool bFound = false;

	for (wchar_t ch; ch = *lpSrc; lpSrc++)
	{
		if (lpExt == NULL)
		{
			if (ch == '\\')
			{
				ch = *++lpSrc;

				if (ch != '\\' && ch != '{' && ch != '}')	// 转义字符只能这几个
					return false;
			}
			else if (ch == '{')
			{
				ch = BRACE_L;
				lpExt = lpSrc + 1;
			}
			else if (ch == '}')
			{
				return false;	// 没有左花括号配对
			}
		}
		else
		{
			if (ch == '\\' || ch == '{')	// 花括号内不能有特殊字符
				return false;

			if (ch == '}')
			{
				int nCount = lpSrc - lpExt;
				if (nCount == 0 || nCount >= 64)
					return false;

				// 括号内 '/' 个数
				int nSlash = 0;

				for (LPCWSTR lp = lpExt; lp != lpSrc; lp++)
				{
					if (*lp == '/')
						nSlash++;
				}

				if (nSlash == 0)	// 没有 '/'
				{
					if (*lpExt < '0' || *lpExt > '9')
						nSteps++;

					bFound = true;
				}
				else if (nSlash == nCount)	// 全是 '/'
				{
					if ((nSteps -= nCount) < 0)
						return false;
				}
				else
					return false;

				ch = BRACE_R;
				lpExt = NULL;
			}
		}

		*lpDst++ = ch;
	}

	*lpDst = 0;

	return lpExt == NULL && nSteps == 0 && bFound;
}

}	// namespace

void CUITextImpl::OnDrawTextEx(HDC hDC, LPRECT lpRect, LPSIZE lpSize) const
{
	vector<DrawInfo> vecDrawInfos;
	DrawInfo drawInfo(m_hFont, m_color);

	for (LPCWSTR lpHead = m_strText.c_str(), lpTail; *lpHead; lpHead = lpTail + 1)
	{
		if (lpSize == NULL && IsRectEmpty(lpRect))
			break;

		if ((lpTail = wcschr(lpHead, BRACE_L)) == NULL)
		{
			ATLASSERT(vecDrawInfos.empty());
			MyDrawText(drawInfo, hDC, lpHead, -1, lpRect, lpSize);
			break;
		}

		if (lpHead != lpTail)
		{
			MyDrawText(drawInfo, hDC, lpHead, lpTail - lpHead, lpRect, lpSize);
			lpHead = lpTail;
		}

		if ((lpTail = wcschr(lpHead, BRACE_R)) == NULL)
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
				nSize = StrToColor(szText, drawInfo.m_color);
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

		default:
			if (CountNumber(szText, false) <= 3)
			{
				nSize = _wtoi(szText);

				if (lpSize)
					lpSize->cx += nSize;
				else
					lpRect->left += nSize;
			}
			break;
		}

		ATLASSERT(nSize);
	}
}

void CUITextImpl::SetFont(HFONT hFont)
{
	ATLASSERT(hFont);
	if (m_hFont != hFont && hFont)
	{
		m_hFont = hFont;
		RecalcSize();
		Invalidate();
	}
}

void CUITextImpl::SetTextColor(COLORREF color)
{
	if (m_color != color)
	{
		m_color = color;
		Invalidate();
	}
}

void CUITextImpl::SetMaxWidth(int nWidth)
{
	if (m_nMaxWidth != nWidth)
	{
		m_nMaxWidth = nWidth;
		RecalcSize();
	}
}

void CUITextImpl::SetText(LPCWSTR lpText)
{
	if (lpText == NULL)
		lpText = L"";

	if (wcschr(lpText, '{'))
	{
		LPWSTR szText = (LPWSTR)alloca((wcslen(lpText) + 1) * 2);

		if (Transcode(lpText, szText))
			lpText = szText;
	}

	if (m_strText != lpText)
	{
		m_strText = lpText;
		RecalcSize();
		Invalidate();
	}
}

void CUITextImpl::Invalidate()
{
	if (m_textSize.cx && m_textSize.cy)
	{
		auto pCtrl = dynamic_cast<CUIControl *>(this);
		ATLASSERT(pCtrl);
		pCtrl->InvalidateRect();
	}
}

void CUITextImpl::RecalcSize()
{
	CSize size;

	if (m_strText.size())
	{
		if (wcschr(m_strText.c_str(), BRACE_L))
		{
			OnDrawTextEx(CUIComDC(NULL), NULL, &size);
		}
		else
		{
			CRect rect;
			DrawTextW(CUIComDC(m_hFont), m_strText.c_str(), -1, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
			size = rect.Size();
		}

		if (size.cx > m_nMaxWidth)
			size.cx = m_nMaxWidth;
	}

	if (m_textSize != size)
		OnTextSize(m_textSize = size);
}

void CUITextImpl::OnLoadText(const IUIXmlAttrs &attrs)
{
	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"font"))
		SetFont(attrs.GetFont(lpStr));

	if (lpStr = attrs.GetStr(L"color"))
	{
		COLORREF color = 0;
		ATLVERIFY(StrToColor(lpStr, color));
		SetTextColor(color);
	}

	int nWidth;
	if (attrs.GetInt(L"maxWidth", &nWidth))
		SetMaxWidth(nWidth);

	if (lpStr = attrs.GetStr(L"text"))
		SetText(lpStr);
}
