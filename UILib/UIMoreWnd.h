#pragma once

#include "UIMoreView.h"

// ��������ť�۵���ͨ����������ఴť��չ���Ĵ���

class CUIMoreWnd : public CWindowImpl<CUIMoreWnd>, public IUIWindow
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS | CS_DROPSHADOW, -1)

	CUIMoreWnd(LPCWSTR lpFileName);
	~CUIMoreWnd();

	HWND Init(HWND hParent, CPoint point, const std::vector<CUIBase *> &vecItems);

private:
	BEGIN_MSG_MAP(CUIMoreWnd)
		CHAIN_MSG_MAP_MEMBER(m_rootView)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	END_MSG_MAP()

	void OnFinalMessage(HWND hWnd) override;
	void OnDrawBg(CUIDC &dc, LPCRECT lpRect) override;
	HWND GetHwnd() const override { return m_hWnd; }

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	CUIMoreView m_rootView;
	CImagex m_imagexBg;
};
