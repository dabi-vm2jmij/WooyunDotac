#include "stdafx.h"
#include "UIDC.h"

namespace
{

class CDCCache
{
public:
	CDCCache();
	~CDCCache();

	HDC  GetDC();
	void ReleaseDC(HDC hDC);

private:
	HDC  m_hDCs[4];
};

CDCCache g_cache;

}	// namespace

CDCCache::CDCCache()
{
	ZeroMemory(m_hDCs, sizeof(m_hDCs));
}

CDCCache::~CDCCache()
{
	for (int i = 0; i != _countof(m_hDCs); i++)
	{
		if (m_hDCs[i])
			DeleteDC(m_hDCs[i]);
	}
}

HDC  CDCCache::GetDC()
{
	for (int i = 0; i != _countof(m_hDCs); i++)
	{
		HDC hDC = (HDC)InterlockedExchange((long *)&m_hDCs[i], NULL);
		if (hDC)
			return hDC;
	}

	return CreateCompatibleDC(NULL);
}

void CDCCache::ReleaseDC(HDC hDC)
{
	for (int i = 0; i != _countof(m_hDCs); i++)
	{
		hDC = (HDC)InterlockedExchange((long *)&m_hDCs[i], (long)hDC);
		if (hDC == NULL)
			return;
	}

	DeleteDC(hDC);
}

CUIComDC::CUIComDC(HGDIOBJ hObject) : m_hDC(g_cache.GetDC()), m_hOldObj(NULL)
{
	if (hObject)
		m_hOldObj = SelectObject(m_hDC, hObject);
}

CUIComDC::~CUIComDC()
{
	if (m_hOldObj)
		SelectObject(m_hDC, m_hOldObj);

	g_cache.ReleaseDC(m_hDC);
}

////////////////////////////////////////////////////////////////////////////////

namespace
{

enum BP_BUFFERFORMAT
{
	BPBF_COMPATIBLEBITMAP,
	BPBF_DIB,
	BPBF_TOPDOWNDIB,
	BPBF_TOPDOWNMONODIB
};

class CUITheme
{
public:
	CUITheme();
	~CUITheme();

	HRESULT (WINAPI *m_pBufferedPaintInit)();
	HRESULT (WINAPI *m_pBufferedPaintUnInit)();
	HANDLE  (WINAPI *m_pBeginBufferedPaint)(HDC, LPCRECT, BP_BUFFERFORMAT, LPVOID, HDC *);
	HRESULT (WINAPI *m_pEndBufferedPaint)(HANDLE, BOOL);

private:
	HMODULE m_hModule;
};

CUITheme g_theme;

}	// namespace

CUITheme::CUITheme()
{
	if ((m_hModule = GetModuleHandle(_T("UxTheme.dll"))) == NULL && (m_hModule = LoadLibrary(_T("UxTheme.dll"))) == NULL)
		return;

	(PVOID &)m_pBufferedPaintInit   = GetProcAddress(m_hModule, "BufferedPaintInit");
	(PVOID &)m_pBufferedPaintUnInit = GetProcAddress(m_hModule, "BufferedPaintUnInit");

	if (m_pBufferedPaintInit == NULL || m_pBufferedPaintUnInit == NULL || m_pBufferedPaintInit() != S_OK)
	{
		FreeLibrary(m_hModule);
		m_hModule = NULL;
		return;
	}

	(PVOID &)m_pBeginBufferedPaint = GetProcAddress(m_hModule, "BeginBufferedPaint");
	(PVOID &)m_pEndBufferedPaint   = GetProcAddress(m_hModule, "EndBufferedPaint");
}

CUITheme::~CUITheme()
{
	if (m_hModule)
	{
		m_pBufferedPaintUnInit();
		FreeLibrary(m_hModule);
	}
}

CUIMemDC::CUIMemDC(HDC hDC, LPCRECT lpClipRect) : m_hDC(hDC), m_hMemDC(NULL), m_hBufferedPaint(NULL), m_clipRect(*lpClipRect)
{
	if (g_theme.m_pBeginBufferedPaint && g_theme.m_pEndBufferedPaint)
	{
		if (m_hBufferedPaint = g_theme.m_pBeginBufferedPaint(hDC, m_clipRect, BPBF_TOPDOWNDIB, NULL, &m_hMemDC))
			return;
	}

	if ((m_hBitmap = CreateCompatibleBitmap(m_hDC, m_clipRect.Width(), m_clipRect.Height())) == NULL)
	{
		CImage image;
		image.Create(m_clipRect.Width(), m_clipRect.Height(), 32, CImage::createAlphaChannel);
		m_hBitmap = image.Detach();
	}

	m_hMemDC  = g_cache.GetDC();
	m_hOldBmp = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);
	SetViewportOrgEx(m_hMemDC, -m_clipRect.left, -m_clipRect.top, &m_point);
	ATLASSERT(m_point.x == 0 && m_point.y == 0);
}

CUIMemDC::~CUIMemDC()
{
	if (m_hBufferedPaint)
	{
		g_theme.m_pEndBufferedPaint(m_hBufferedPaint, TRUE);
		return;
	}

	BitBlt(m_hDC, m_clipRect.left, m_clipRect.top, m_clipRect.Width(), m_clipRect.Height(), m_hMemDC, m_clipRect.left, m_clipRect.top, SRCCOPY);
	SetViewportOrgEx(m_hMemDC, m_point.x, m_point.y, NULL);
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hBitmap);
	g_cache.ReleaseDC(m_hMemDC);
}

CUIDC::CUIDC(HDC hDC, CPoint point, CImage *pImage) : m_hDC(hDC), m_point(point), m_pImage(pImage)
{
	ATLASSERT(hDC);
	SetBkMode(hDC, TRANSPARENT);
	m_hOldFont = (HFONT)SelectObject(hDC, GetDefaultFont());
}

CUIDC::~CUIDC()
{
	SelectObject(m_hDC, m_hOldFont);
}

void CUIDC::FillSolidRect(LPCRECT lpRect, COLORREF color)
{
	if (m_pImage == NULL)
	{
		COLORREF oldColor = SetBkColor(m_hDC, color);

		if (oldColor != CLR_INVALID)
		{
			ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
			SetBkColor(m_hDC, oldColor);
		}
		return;
	}

	CRect rect;
	if (GetClipBox(m_hDC, rect) == NULLREGION || !rect.IntersectRect(rect, lpRect))
		return;

	rect += m_point;
	DWORD dwColor = BswapRGB(color) | 0xff000000;

	for (int x = rect.left; x < rect.right; x++)
	{
		for (int y = rect.top; y < rect.bottom; y++)
			*(DWORD *)m_pImage->GetPixelAddress(x, y) = dwColor;
	}
}

void CUIDC::FillAlpha(LPCRECT lpRect, BYTE nAlpha)
{
	CRect rect;
	if (m_pImage == NULL || GetClipBox(m_hDC, rect) == NULLREGION || !rect.IntersectRect(rect, lpRect))
		return;

	rect += m_point;

	for (int x = rect.left; x < rect.right; x++)
	{
		for (int y = rect.top; y < rect.bottom; y++)
			((BYTE *)m_pImage->GetPixelAddress(x, y))[3] = nAlpha;
	}
}
