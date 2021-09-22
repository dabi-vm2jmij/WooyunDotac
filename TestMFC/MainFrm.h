// MainFrm.h : CMainFrame 类的接口
//

#pragma once

class CMainFrame : public CFrameWnd, public IUIWindow
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnLoadedUI(const IUILoadAttrs &loaded) override;
	virtual void OnDrawBg(CUIDC &dc, LPCRECT lpRect) override;
	virtual HWND GetHwnd() const override { return m_hWnd; }
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnProgress(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowAbout();
	afx_msg void OnTestFunc();

	void ShowBottom(bool bShow);
	void DoSetup();
	static UINT WINAPI WorkThread(LPVOID pParam);

	CUIRootView  m_rootView;
	CImagex      m_imagexBg;
	CImagex      m_imagexBg2;
	CUIView     *m_pViews[3];
	CUIProgress *m_pProgress;
	CUIMenu      m_uiMenu;
};
