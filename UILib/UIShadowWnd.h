#pragma once

class CUIShadowWnd : public CWindowImpl<CUIShadowWnd>
{
public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1)

	CUIShadowWnd(const CImagex &shadowImage, int nShadowSize);

	BEGIN_MSG_MAP(CUIShadowWnd)
		if (uMsg == WM_CLOSE || uMsg == WM_NCCALCSIZE) return TRUE;
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
	END_MSG_MAP()

	void CreateWnd(HWND hOwnerWnd, BOOL bThickFrame);
	void OnOwnerSize(int nNewType);
	void OnOwnerPosChanged(const WINDOWPOS *lpWndPos);

private:
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void Update(POINT *pPoint, SIZE &size) const;

	CImagex m_shadowImage;
	int     m_nShadowSize;
	HWND    m_hOwnerWnd;
	int     m_nOwnerType;
	BOOL    m_bNoUpdate;
};
