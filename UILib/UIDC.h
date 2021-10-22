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
	CUIMemDC(HDC hDC, LPCRECT lpClipRect);
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

// 在 OnPaint 中使用，可以记录 CUIRootView 传下来的其它参数

class UILIB_API CUIDC
{
public:
	CUIDC(HDC hDC, CPoint point, CImage *pImage);
	~CUIDC();

	operator HDC() const { return m_hDC; }
	void FillSolidRect(LPCRECT lpRect, COLORREF color);
	void FillAlpha(LPCRECT lpRect, BYTE nAlpha);

private:
	HDC     m_hDC;
	CPoint  m_point;
	CImage *m_pImage;
	HFONT   m_hOldFont;
};
