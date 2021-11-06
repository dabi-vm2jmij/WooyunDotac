#pragma once

class CUIWindow : public CWnd, public IUIWindow
{
	DECLARE_DYNAMIC(CUIWindow)
public:
	CUIWindow();
	virtual ~CUIWindow();

	static CUIWindow *FromHwnd(HWND hWnd);

	CUIRootView *GetRootView() { return &m_rootView; }
	bool CreateFromXml(LPCWSTR lpXmlName, HWND hParent = NULL);

	DECLARE_MESSAGE_MAP()
protected:
	virtual void PostNcDestroy() override { delete this; }
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;
	virtual void OnLoadUI(const IUIXmlAttrs &attrs) override;
	virtual HWND GetHwnd() const override { return m_hWnd; }
	virtual void InvalidateRect(LPCRECT lpRect) override;
	virtual void DoCreate(int nWidth, int nHeight, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyle);
	virtual void DoPaint(CUIDC &dc) const;
	virtual void UpdateLayered(HDC hDC);

	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	struct Layered
	{
		CImage m_imageWnd;
		CRect  m_clipRect;
		CPoint m_point;		// ×ø±êÔ­µã
	};

	CUIRootView m_rootView;
	HWND        m_hWndParent;
	CImagex     m_borderImage;
	int         m_nBorderSize;
	int         m_nCaptionSize;
	std::unique_ptr<Layered> m_pLayered;
};
