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

	CImagex       m_bgImagex;
	CImagex       m_bgImagex2;
	CSize         m_bgSize;		// Layered 或无边框窗口不用
	CUIWebTabBar *m_pWebTabBar;
	CUIView      *m_pTabView;
};
