#include "stdafx.h"
#include "UIMoreWnd.h"

CUIMoreWnd::CUIMoreWnd(COLORREF color) : m_rootView(this)
{
	m_rootView.SetBgColor(color);
}

CUIMoreWnd::~CUIMoreWnd()
{
	m_rootView.ClearItems();
}

HWND CUIMoreWnd::Init(HWND hParent, CPoint point, const std::vector<CUIBase *> &vecItems)
{
	m_rootView.InitItems(vecItems);

	CSize size;
	m_rootView.CalcSize(&size);

	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST), &mi);

	if (point.x + size.cx > mi.rcMonitor.right)
		point.x -= size.cx;

	if (point.y + size.cy > mi.rcMonitor.bottom)
		point.y -= size.cy;

	return Create(hParent, CRect(point, size), NULL, WS_POPUP | WS_VISIBLE);
}

void CUIMoreWnd::OnFinalMessage(HWND hWnd)
{
	delete this;
}

LRESULT CUIMoreWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PostMessage(WM_CLOSE);
	return 0;
}
