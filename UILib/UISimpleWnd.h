#pragma once

#include "UIWindow.h"

class CUISimpleWnd : public CUIWindow
{
public:
	CUISimpleWnd(bool bModal = false) : m_bModal(bModal) {}

	void BindInitUI(function<void(const IUIXmlAttrs &)> &&fnInitUI) { m_fnInitUI = std::move(fnInitUI); }

private:
	virtual void OnLoadUI(const IUIXmlAttrs &attrs) override;
	virtual int  OnCreate(LPCREATESTRUCT lpCreateStruct) override;
	virtual void OnWindowPosChanging(WINDOWPOS *lpWndPos) override;

	bool m_bModal;
	function<void(const IUIXmlAttrs &)> m_fnInitUI;
};

inline void CUISimpleWnd::OnLoadUI(const IUIXmlAttrs &attrs)
{
	if (m_fnInitUI)
		m_fnInitUI(attrs);

	__super::OnLoadUI(attrs);
}

inline int CUISimpleWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CenterWindow();
	return 0;
}

inline void CUISimpleWnd::OnWindowPosChanging(WINDOWPOS *lpWndPos)
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
