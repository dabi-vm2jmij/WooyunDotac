#pragma once

class CTabDemoWnd : public CUIWindow
{
public:
	CTabDemoWnd();
	~CTabDemoWnd();

private:
	int  OnCreate(LPCREATESTRUCT lpCreateStruct) override;
	void OnSize(UINT nType, CSize size) override;
	void OnGetMinMaxInfo(MINMAXINFO *lpMMI) override;
	void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpNCSP) override;

	void InitUI();
	void AddWebTab();
	void DelWebTab(CUIWebTab *pWebTab);
	void OnDrawBg(CUIDC &dc, LPCRECT lpRect) const override;

	CImagex       m_imagexBg;
	CImagex       m_imagexBg2;
	CSize         m_sizeBg;
	CUIWebTabBar *m_pWebTabBar;
	CUIView      *m_pTabView;
};
