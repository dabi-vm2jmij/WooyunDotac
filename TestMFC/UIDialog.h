#pragma once

#include "UIWindow.h"

class CUIDialog : public CUIWindow
{
	DECLARE_DYNAMIC(CUIDialog)
public:
	CUIDialog(bool bModal = false);

	void BindClose(function<bool()> &&fnOnClose) { m_fnOnClose = std::move(fnOnClose); }
	void BindDestroy(function<void()> &&fnOnDestroy) { m_fnOnDestroy = std::move(fnOnDestroy); }
	void BindOK(function<void()> &&fnOnOK) { m_fnOnOK = std::move(fnOnOK); }
	void BindCancel(function<void()> &&fnOnCancel) { m_fnOnCancel = std::move(fnOnCancel); }

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnOK();
	virtual void OnCancel();

	bool m_bModal;
	function<bool()> m_fnOnClose;
	function<void()> m_fnOnDestroy;
	function<void()> m_fnOnOK;
	function<void()> m_fnOnCancel;
};
