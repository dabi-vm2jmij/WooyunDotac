#pragma once

class CUIMenu2 : public CUIMenu
{
private:
	CUIMenu *NewUIMenu() const override { return new CUIMenu2; }
	bool IsDropShadow() const override { return false; }
	void GetMargins(LPRECT lpRect) const override;
	int  GetSpacing() const override { return -8; }
	BYTE GetWndAlpha() const override { return 230; }
	void DrawBg(CUIDC &dc, LPCRECT lpRect) override;

	CImagex m_bgImagex;
};
