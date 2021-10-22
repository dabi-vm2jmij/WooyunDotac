#pragma once

#include "UIWindow.h"

class CUIDialog : public CUIWindow
{
	DECLARE_DYNAMIC(CUIDialog)
public:
	CUIDialog(bool bModal = false);

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

	bool m_bModal;
};
