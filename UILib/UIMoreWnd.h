#pragma once

#include "UIRootView2.h"

// ��������ť�۵���ͨ����������ఴť��չ���Ĵ���

class CUIMoreWnd : public CWindowImpl<CUIMoreWnd>
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS | CS_DROPSHADOW, -1)
	DECLARE_UI_ROOTVIEW_EX(CUIRootView2, m_rootView)

	CUIMoreWnd(LPCWSTR lpFileName);
	~CUIMoreWnd();

	HWND Init(HWND hParent, CPoint point, const std::vector<CUIBase *> &vecItems);
	void OnFinalMessage(HWND hWnd) override;

	BEGIN_MSG_MAP(CUIMoreWnd)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	END_MSG_MAP()

private:
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	CImagex m_imagexBg;
};
