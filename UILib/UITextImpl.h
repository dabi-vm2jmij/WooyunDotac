#pragma once

// 文字控件的基类

class UILIB_API CUITextImpl
{
public:
	CUITextImpl();
	virtual ~CUITextImpl();

	void SetFont(HFONT hFont);
	HFONT GetFont() const { return m_hFont; }
	void SetTextColor(COLORREF color);
	void SetText(LPCWSTR lpText);
	LPCWSTR GetText() const { return m_strText.c_str(); }
	void SetGdiplus(bool bGdiplus);
	void SetWordBreak(bool bWordBreak);
	void SetMaxWidth(int nWidth);
	CSize GetTextSize() const { return m_textSize; }

protected:
	virtual void OnTextSize(CSize size) = 0;
	void OnLoadText(const IUIXmlAttrs &attrs);
	void OnDrawText(CUIDC &dc, LPCRECT lpRect, UINT nFormat) const;
	void OnDrawTextEx(HDC hDC, LPRECT lpRect, LPSIZE lpSize) const;
	void Invalidate();
	void RecalcSize();

	wstring  m_strText;
	HFONT    m_hFont;
	COLORREF m_color;
	bool     m_bGdiplus;	// 使用 gdiplus 绘制
	bool     m_bWordBreak;	// 超过 maxWidth 自动换行
	int      m_nMaxWidth;	// calcSize 的最大宽度
	CSize    m_textSize;
};
