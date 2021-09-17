#pragma once

// UIMenu 的实现窗口

class CUIMenuWnd : public CWindowImplBase
{
public:
	CUIMenuWnd(CUIMenu *pUIMenu);
	~CUIMenuWnd();

	UINT Popup(HWND hParent, int x1, int y1, int x2, int y2, bool bPostMsg);

private:
	BEGIN_MSG_MAP(CUIMenuWnd)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	END_MSG_MAP()

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	HWND Create(HWND hParent, LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, bool bDropShadow);
	bool Init(HWND hParent, int x1, int y1, int x2, int y2);
	bool IsMenuWnd(HWND hWnd) const;
	void ResetSel();
	void OnTimeOut(bool bReset);
	UINT Point2Sel(CPoint point) const;
	void OnSelChanged(UINT nCurSel, bool bInit);

	CUIMenu    *m_pUIMenu;
	CUIMenuWnd *m_pParent;
	CUIMenuWnd *m_pChild;
	CImage      m_imageWnd;	// 窗口的缓存图
	CPoint      m_ptMouse;
	UINT        m_nItemId;
	UINT        m_nCurSel;
};
