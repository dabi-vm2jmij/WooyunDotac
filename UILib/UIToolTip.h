#pragma once

class CUIToolTip : public CWindowImpl<CUIToolTip>
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_SAVEBITS, -1)

	void Show(HWND hParent, LPCWSTR lpText);
	void Hide();

private:
	BEGIN_MSG_MAP(CUIToolTip)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	END_MSG_MAP()

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	void ShowWnd();

	wstring m_strText;
	HWND    m_hParent;
};
