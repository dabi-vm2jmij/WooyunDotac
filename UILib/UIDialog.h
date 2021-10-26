#pragma once

#include "UIWindow.h"

class CUIDialog : public CUIWindow
{
public:
	CUIDialog(bool bModal = false);

	BEGIN_MSG_MAP(CUIDialog)
		CHAIN_MSG_MAP(CUIWindow)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	void BindClose(function<void()> &&fnOnClose) { m_fnOnClose = std::move(fnOnClose); }
	void BindOK(function<void()> &&fnOnOK) { m_fnOnOK = std::move(fnOnOK); }
	void BindCancel(function<void()> &&fnOnCancel) { m_fnOnCancel = std::move(fnOnCancel); }

protected:
	virtual void OnCreate() override;
	virtual void OnWindowPosChanging(WINDOWPOS *lpWndPos) override;
	virtual void OnClose() override;
	virtual void OnOK();
	virtual void OnCancel();
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	bool m_bModal;
	function<void()> m_fnOnClose;
	function<void()> m_fnOnOK;
	function<void()> m_fnOnCancel;
};
