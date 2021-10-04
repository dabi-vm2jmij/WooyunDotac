#pragma once

// CUIWnd，其它 UI 窗口类都从此类继承

class CUIWnd : public CWnd, public IUIWindow
{
	DECLARE_DYNAMIC(CUIWnd)
public:
	CUIWnd(bool bModal = false);
	virtual ~CUIWnd();

	CUIRootView *GetRootView() { return &m_rootView; }
	bool CreateFromXml(LPCWSTR lpXmlName, HWND hParent = NULL);

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void PostNcDestroy();
	virtual void OnLoadedUI(const IUIXmlAttrs &attrs) override;
	virtual HWND GetHwnd() const override { return m_hWnd; }
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);

	CUIRootView m_rootView;
	bool m_bModal;
	HWND m_hWndParent;
	int  m_nBorderSize;
	int  m_nCaptionHeight;
	CUIStateButton *m_pBtnMax;
};
