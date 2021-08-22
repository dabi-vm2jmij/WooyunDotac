#pragma once

class CTabDemoWnd : public CUIWindow
{
public:
	CTabDemoWnd();
	~CTabDemoWnd();

	void OnFinalMessage(HWND hWnd) override;

private:
	int  OnCreate(LPCREATESTRUCT lpCreateStruct) override;
	void OnSize(UINT nType, CSize size) override;
	void OnGetMinMaxInfo(MINMAXINFO *lpMMI) override;
	BOOL OnNcActivate(BOOL bActive) override { return TRUE; }
	void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpNCSP) override;

	void InitUI();
	void DrawBg(CUIDC &dc, LPCRECT lpRect);

	CImagex       m_imagexBg;
	CImagex       m_imagexBg2;
	CSize         m_sizeBg;
	CUIWebTabBar *m_pWebTabBar;
};
