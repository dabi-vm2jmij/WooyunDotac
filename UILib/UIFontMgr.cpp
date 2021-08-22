#include "stdafx.h"
#include "UIFontMgr.h"

CUIFontMgr::FontKey::FontKey(HFONT hFont)
{
	LOGFONTW lf;
	GetObjectW(hFont, sizeof(lf), &lf);

	m_nHeight    = lf.lfHeight;
	m_nWeight    = lf.lfWeight;
	m_bItalic    = lf.lfItalic;
	m_bUnderline = lf.lfUnderline;

	wcscpy_s(m_szFaceName, lf.lfFaceName);
}

CUIFontMgr::FontKey::FontKey(LPCWSTR lpszName, int nWeight, BYTE bItalic, BYTE bUnderline) : m_nWeight(nWeight), m_bItalic(bItalic), m_bUnderline(bUnderline)
{
	ParseName(lpszName);
}

static BOOL IsWindowsVersionOrGreater(DWORD dwMajorVersion)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), dwMajorVersion };
	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION, VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL));
}

// 格式为“字体W7:高度W7|字体XP:高度XP”
void CUIFontMgr::FontKey::ParseName(LPCWSTR lpszName)
{
	wchar_t szName[64];

	if (lpszName == NULL)
		lpszName = L"";

	if (LPCWSTR lpFind = wcschr(lpszName, '|'))
	{
		static BOOL s_bVista = IsWindowsVersionOrGreater(6);

		if (s_bVista)
		{
			wcsncpy_s(szName, lpszName, lpFind - lpszName);
			lpszName = szName;
		}
		else
			lpszName = lpFind + 1;
	}

	if (LPCWSTR lpFind = wcschr(lpszName, ':'))
	{
		wcsncpy_s(m_szFaceName, lpszName, lpFind - lpszName);
		m_nHeight = _wtoi(lpFind + 1);
	}
	else
	{
		wcscpy_s(m_szFaceName, lpszName);
		m_nHeight = 0;
	}
}

bool CUIFontMgr::FontKey::operator<(const FontKey &_Right) const
{
	int nCmp = _wcsicmp(m_szFaceName, _Right.m_szFaceName);
	return (nCmp ? nCmp : memcmp(this, &_Right, offsetof(FontKey, m_szFaceName))) < 0;
}

CUIFontMgr::CUIFontMgr() : m_fontKey(L"微软雅黑:-12|Tahoma:-12", FW_NORMAL, false, false)
{
	InitializeCriticalSection(&m_critSect);
}

CUIFontMgr::~CUIFontMgr()
{
	for (auto &it : m_mapFonts)
	{
		if (it.second)
			DeleteObject(it.second);
	}

	DeleteCriticalSection(&m_critSect);
}

CUIFontMgr &CUIFontMgr::Get()
{
	static CUIFontMgr s_fontMgr;
	return s_fontMgr;
}

void CUIFontMgr::SetFont(LPCWSTR lpszName)
{
	m_fontKey.ParseName(lpszName);
	ATLASSERT(*m_fontKey.m_szFaceName && m_fontKey.m_nHeight);
}

HFONT CUIFontMgr::GetFont(LPCWSTR lpszName, int nWeight, BYTE bItalic, BYTE bUnderline)
{
	return GetFont(FontKey(lpszName, nWeight, bItalic, bUnderline));
}

HFONT CUIFontMgr::GetFont(FontKey &fontKey)
{
	if (*fontKey.m_szFaceName == 0)
		wcscpy_s(fontKey.m_szFaceName, m_fontKey.m_szFaceName);

	if (fontKey.m_nHeight == 0)
		fontKey.m_nHeight = m_fontKey.m_nHeight;

	EnterCriticalSection(&m_critSect);
	HFONT hFont = m_mapFonts[fontKey];

	if (hFont == NULL)
	{
		hFont = CreateFontW(
			fontKey.m_nHeight,			// nHeight
			0,							// nWidth
			0,							// nEscapement
			0,							// nOrientation
			fontKey.m_nWeight,			// nWeight
			fontKey.m_bItalic,			// bItalic
			fontKey.m_bUnderline,		// bUnderline
			FALSE,						// cStrikeOut
			DEFAULT_CHARSET,			// nCharSet
			OUT_DEFAULT_PRECIS,			// nOutPrecision
			CLIP_DEFAULT_PRECIS,		// nClipPrecision
			DEFAULT_QUALITY,			// nQuality
			DEFAULT_PITCH | FF_DONTCARE,// nPitchAndFamily
			fontKey.m_szFaceName		// lpszFacename
		);

		m_mapFonts[fontKey] = hFont;
	}

	LeaveCriticalSection(&m_critSect);
	return hFont;
}
