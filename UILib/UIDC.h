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

// �� OnPaint ��ʹ�ã����Լ�¼ CUIRootView ����������������

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
