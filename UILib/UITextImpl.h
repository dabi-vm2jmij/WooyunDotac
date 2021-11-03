#pragma once

// ���ֿؼ��Ļ���

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
	bool     m_bGdiplus;	// ʹ�� gdiplus ����
	bool     m_bWordBreak;	// ���� maxWidth �Զ�����
	int      m_nMaxWidth;	// calcSize �������
	CSize    m_textSize;
};
