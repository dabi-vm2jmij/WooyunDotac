#pragma once

class CUIMenu2 : public CUIMenu
{
private:
	CUIMenu *NewUIMenu() const override { return new CUIMenu2; }
	bool IsDropShadow() const override { return false; }
	void InflateRect(LPRECT lpRect) const override { ::InflateRect(lpRect, 8, 8); }
	BYTE GetWndAlpha() const override { return 230; }
	void DrawBg(CUIDC &dc, LPCRECT lpRect) override;
	void DrawItem(CUIDC &dc, LPCRECT lpRect, UINT nIndex, bool bSelected) override;

	CImagex m_imagexBg;
};