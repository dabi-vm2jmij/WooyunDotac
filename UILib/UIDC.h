#pragma once

// 快速兼容 DC，构造时可以指定 hBitmap、hFont

class UILIB_API CUIComDC
{
public:
	CUIComDC(HGDIOBJ hObject);
	~CUIComDC();

	operator HDC() const { return m_hDC; }

private:
	HDC     m_hDC;
	HGDIOBJ m_hOldObj;
};

// 高级内存 DC，可以根据情况选用 BufferedPaint、CompatibleBitmap、DIBSection

class UILIB_API CUIMemDC
{
public:
	CUIMemDC(HDC hDC, LPCRECT lpClipRect, bool bImage = false);
	~CUIMemDC();

	operator HDC() const { return m_hMemDC; }

private:
	HDC     m_hDC;
	HDC     m_hMemDC;
	HANDLE  m_hBufferedPaint;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBmp;
	CRect   m_rectClip;
	CPoint  m_point;
};

// 在 OnPaint 中使用，可以记录 CUIRootView 传下来的其它参数

class UILIB_API CUIDC
{
public:
	CUIDC(HDC hDC, LPCRECT lpClipRect, bool bLayered);
	~CUIDC();

	operator HDC() const { return m_hDC; }
	void     Delete();
	bool     GetRealRect(LPRECT lpRect) const;	// 计算与剪裁区域相交的区域
	bool     IsLayered() const;
	HFONT    SelectFont(HFONT hFont);
	COLORREF SetTextColor(COLORREF color);
	void     FillSolidRect(LPCRECT lpRect, COLORREF color);

private:
	HDC      m_hDC;
	LPCRECT  m_lpClipRect;
	bool     m_bLayered;
	HFONT    m_hOldFont;
	HFONT    m_hCurFont;
	COLORREF m_curColor;
};

class CUIPaintDC
{
public:
	CUIPaintDC(HWND hWnd) : m_hWnd(hWnd), m_hDC(BeginPaint(hWnd, &m_ps))
	{
		ATLASSERT(IsWindow(hWnd));
	}

	~CUIPaintDC()
	{
		EndPaint(m_hWnd, &m_ps);
	}

	operator HDC() const { return m_hDC; }

	LPCRECT PaintRect() const { return &m_ps.rcPaint; }

private:
	HWND m_hWnd;
	HDC  m_hDC;
	PAINTSTRUCT m_ps;
};

class CUIClientDC
{
public:
	CUIClientDC(HWND hWnd) : m_hWnd(hWnd), m_hDC(GetDC(hWnd))
	{
		ATLASSERT(hWnd == NULL || IsWindow(hWnd));
	}

	~CUIClientDC()
	{
		ReleaseDC(m_hWnd, m_hDC);
	}

	operator HDC() const { return m_hDC; }

private:
	HWND m_hWnd;
	HDC  m_hDC;
};

class CUIRgn
{
public:
	CUIRgn(HRGN hRgn) : m_hRgn(hRgn)
	{
		ATLASSERT(hRgn);
	}

	CUIRgn(int x1, int y1, int x2, int y2) : m_hRgn(CreateRectRgn(x1, y1, x2, y2))
	{
	}

	~CUIRgn()
	{
		DeleteObject(m_hRgn);
	}

	operator HRGN() const { return m_hRgn; }

private:
	HRGN m_hRgn;
};
