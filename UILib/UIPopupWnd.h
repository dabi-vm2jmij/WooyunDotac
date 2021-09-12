#pragma once

#include "UIWindow.h"

class CUIPopupWnd : public CUIWindow
{
public:
	CUIPopupWnd(bool bModal = false) : m_bModal(bModal) {}

	void OnInitUI(std::function<void(const IUILoadAttrs &)> &&fnInitUI) { m_fnInitUI = std::move(fnInitUI); }

private:
	void OnLoadedUI(const IUILoadAttrs &attrs) override;
	int  OnCreate(LPCREATESTRUCT lpCreateStruct) override;
	void OnWindowPosChanging(WINDOWPOS *lpWndPos) override;
	BOOL OnNcActivate(BOOL bActive) override { return TRUE; }
	void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpNCSP) override {}

	bool m_bModal;
	std::function<void(const IUILoadAttrs &)> m_fnInitUI;
};

inline void CUIPopupWnd::OnLoadedUI(const IUILoadAttrs &attrs)
{
	if (m_fnInitUI)
		m_fnInitUI(attrs);

	__super::OnLoadedUI(attrs);
}

inline int CUIPopupWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowRgn(CUIRgn(0, 0, 4000, 3000));
	CenterWindow();
	return 0;
}

inline void CUIPopupWnd::OnWindowPosChanging(WINDOWPOS *lpWndPos)
{
	__super::OnWindowPosChanging(lpWndPos);

	if (!m_bModal)
	{
	}
	else if (lpWndPos->flags & SWP_SHOWWINDOW)
	{
		GetParent().EnableWindow(FALSE);
	}
	else if (lpWndPos->flags & SWP_HIDEWINDOW)
	{
		GetParent().EnableWindow(TRUE);
	}
}
