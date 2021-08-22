#include "stdafx.h"
#include "UIToolTip.h"

LRESULT CUIToolTip::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (wParam == 1)
	{
		KillTimer(wParam);
		ShowWnd();
	}

	return 0;
}

LRESULT CUIToolTip::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	CRect rect;
	GetClientRect(rect);

	CUIPaintDC dc(m_hWnd);
	SetBkColor(dc, RGB(255, 255, 225));
	ExtTextOut(dc, 0, 0, ETO_OPAQUE, rect, NULL, 0, NULL);

	SelectObject(dc, GetDefaultFont());
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(0, 0, 0));

	DrawTextW(dc, m_strText.c_str(), -1, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	return 0;
}

LRESULT CUIToolTip::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	Hide();
	return 0;
}

void CUIToolTip::Show(HWND hParent, LPCWSTR lpText)
{
	if (m_strText.empty() ? (*lpText == 0) : (m_strText == lpText && m_hParent == hParent))
		return;

	m_strText = lpText;

	if (m_strText.empty())
	{
		Hide();
		return;
	}

	if (m_hWnd && m_hParent != hParent)
		DestroyWindow();

	if (m_hWnd == NULL)
		Create(m_hParent = hParent, CRect(), NULL, WS_POPUP | WS_BORDER, WS_EX_TOOLWINDOW);

	if (IsWindowVisible())
		ShowWnd();
	else
		SetTimer(1, 500, NULL);
}

void CUIToolTip::Hide()
{
	if (m_hWnd == NULL)
		return;

	KillTimer(1);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
}

void CUIToolTip::ShowWnd()
{
	CRect rect;
	DrawTextW(CUIComDC(GetDefaultFont()), m_strText.c_str(), -1, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
	int nWidth = rect.Width() + 12, nHeight = rect.Height() + 8;

	CPoint point;
	GetCursorPos(&point);

	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST), &mi);

	if (point.x > mi.rcMonitor.right - nWidth)
		point.x = mi.rcMonitor.right - nWidth;

	if (point.y + 21 > mi.rcMonitor.bottom - nHeight)
		point.y -= 25;
	else
		point.y += 21;

	Invalidate();
	SetWindowPos(HWND_TOPMOST, point.x, point.y, nWidth, nHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void CUIToolTip::ShowTip(HWND hParent, LPCWSTR lpText)
{
	static CUIToolTip s_toolTip;

	if (lpText)
		s_toolTip.Show(hParent, lpText);
	else
		s_toolTip.Hide();
}
