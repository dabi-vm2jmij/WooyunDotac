#pragma once

// 文字控件的基类

class UILIB_API CUITextImpl
{
public:
	CUITextImpl();
	virtual ~CUITextImpl();

	LPCWSTR GetText() const { return m_strText.c_str(); }
	void SetText(LPCWSTR lpText);
	void SetTextEx(LPCWSTR lpText);
	void SetFont(HFONT hFont);
	void SetTextColor(COLORREF color);

protected:
	virtual void OnTextSize(CSize size) = 0;
	void OnLoadedText(const IUILoadAttrs &attrs);
	void DoSetText(LPCWSTR lpText, bool bTextEx);
	void OnDrawText(CUIDC &dc, LPRECT lpRect, UINT nFormat) const;
	void OnDrawTextEx(CUIDC &dc, LPRECT lpRect, LPSIZE lpSize) const;
	void Invalidate();
	void RecalcSize();

	wstring  m_strText;
	bool     m_bTextEx;
	HFONT    m_hFont;
	COLORREF m_color;
	CSize    m_sizeText;
};
