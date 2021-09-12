#pragma once

#include "UIMoreView.h"

// 工具栏按钮折叠后，通过点击“更多按钮”展开的窗口

class CUIMoreWnd : public CWindowImpl<CUIMoreWnd>, public IUIWindow
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS | CS_DROPSHADOW, -1)

	CUIMoreWnd(LPCWSTR lpFileName);
	~CUIMoreWnd();

	HWND Init(HWND hParent, CPoint point, const std::vector<CUIBase *> &vecItems);
	void OnFinalMessage(HWND hWnd) override;

	BEGIN_MSG_MAP(CUIMoreWnd)
		CHAIN_MSG_MAP_MEMBER(m_rootView)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	END_MSG_MAP()

private:
	CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) override { return NULL; }
	void OnLoadedUI(const IUILoadAttrs &attrs) override {}
	void OnDrawBg(CUIDC &dc, LPCRECT lpRect) override;
	HWND GetHwnd() const override { return m_hWnd; }

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	CUIMoreView m_rootView;
	CImagex m_imagexBg;
};
