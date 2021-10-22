#pragma once

// 文字控件的基类

class UILIB_API CUITextImpl
{
public:
	CUITextImpl();
	virtual ~CUITextImpl();

	void SetFont(HFONT hFont);
	void SetTextColor(COLORREF color);
	void SetMaxWidth(int nWidth);
	void SetText(LPCWSTR lpText);
	LPCWSTR GetText() const { return m_strText.c_str(); }
	CSize GetTextSize() const { return m_textSize; }

protected:
	virtual void OnTextSize(CSize size) = 0;
	void OnLoadText(const IUIXmlAttrs &attrs);
	void OnDrawText(CUIDC &dc, LPRECT lpRect, UINT nFormat) const;
	void OnDrawTextEx(HDC hDC, LPRECT lpRect, LPSIZE lpSize) const;
	void Invalidate();
	void RecalcSize();

	wstring  m_strText;
	HFONT    m_hFont;
	COLORREF m_color;
	int      m_nMaxWidth;
	CSize    m_textSize;
};
