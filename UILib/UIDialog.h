#pragma once

#include "UIWindow.h"

class CUIDialog : public CUIWindow
{
public:
	CUIDialog(bool bModal = false);

protected:
	virtual void OnCreate() override;
	virtual void OnWindowPosChanging(WINDOWPOS *lpWndPos) override;

	bool m_bModal;
};
