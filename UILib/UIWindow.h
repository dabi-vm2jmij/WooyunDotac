#pragma once

class CUIShadowWnd;

class CUIWindow : public CWindowImpl<CUIWindow>, public IUIWindow
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, -1)

	CUIWindow();
	virtual ~CUIWindow();

	static CUIWindow *FromHwnd(HWND hWnd);

	CUIRootView *GetRootView() { return &m_rootView; }
	bool CreateFromXml(LPCWSTR lpXmlName, HWND hParent = NULL);
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult, DWORD dwMsgMapID = 0) override;

protected:
	virtual void OnFinalMessage(HWND hWnd) override { delete this; }
	virtual void OnLoadUI(const IUIXmlAttrs &attrs) override;
	virtual HWND GetHwnd() const override { return m_hWnd; }
	virtual void InvalidateRect(LPCRECT lpRect) override;
	virtual void DoCreate(int nWidth, int nHeight, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyle);
	virtual void DoPaint(CUIDC &dc) const;
	virtual void UpdateLayered(HDC hDC);

	virtual void OnCreate() { DefWindowProc(); }
	virtual void OnClose() { DefWindowProc(); }
	virtual void OnDestroy() { DefWindowProc(); }
	virtual void OnSysCommand(UINT nID, LPARAM lParam) { DefWindowProc(); }
	virtual void OnGetMinMaxInfo(MINMAXINFO *lpMMI) { DefWindowProc(); }
	virtual void OnSetFocus(HWND hOldWnd) { DefWindowProc(); }
	virtual void OnKillFocus(HWND hNewWnd) { DefWindowProc(); }
	virtual BOOL OnNcActivate(BOOL bActive) { return TRUE; }
	virtual void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpNCSP);
	virtual long OnNcHitTest(CPoint point);
	virtual void OnPaint();
	virtual void OnPrint(HDC hDC);
	virtual void OnSize(UINT nType, CSize size);
	virtual void OnTimer(UINT_PTR nIDEvent) { DefWindowProc(); }
	virtual void OnWindowPosChanging(WINDOWPOS *lpWndPos) { DefWindowProc(); }
	virtual void OnWindowPosChanged(WINDOWPOS *lpWndPos);

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
	std::unique_ptr<CUIShadowWnd> m_pShadowWnd;
};
