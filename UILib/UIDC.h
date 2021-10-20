#pragma once

// ���ټ��� DC������ʱ����ָ�� hBitmap��hFont

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

// �߼��ڴ� DC�����Ը������ѡ�� BufferedPaint��CompatibleBitmap��DIBSection

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
	CRect   m_clipRect;
	CPoint  m_point;
};

// �� OnPaint ��ʹ�ã����Լ�¼ CUIRootView ����������������

class UILIB_API CUIDC
{
public:
	CUIDC(HDC hDC, LPCRECT lpClipRect, bool bLayered);
	~CUIDC();

	operator HDC() const { return m_hDC; }
	void     Delete();
	bool     GetRealRect(LPRECT lpRect) const;	// ��������������ཻ������
	bool     IsLayered() const;
	HFONT    SelectFont(HFONT hFont);
	COLORREF SetTextColor(COLORREF color);
	void     FillSolidRect(LPCRECT lpRect, COLORREF color);
	void     FillAlpha(LPCRECT lpRect, BYTE nAlpha);

private:
	HDC      m_hDC;
	LPCRECT  m_lpClipRect;
	bool     m_bLayered;
	HFONT    m_hOldFont;
	HFONT    m_hCurFont;
	COLORREF m_curColor;
	CImage   m_image;	// 1 * 1 ����ɫͼƬ
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
