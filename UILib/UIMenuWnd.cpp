#include "stdafx.h"
#include "UIMenuWnd.h"

#define TIMER_ID	1
#define WM_SELECT	WM_USER + 1000

CUIMenuWnd::CUIMenuWnd(CUIMenu *pUIMenu) : m_pUIMenu(pUIMenu), m_pParent(NULL), m_pChild(NULL), m_ptMouse(MAXINT16, MAXINT16), m_nItemId(-1), m_nCurSel(-1)
{
}

CUIMenuWnd::~CUIMenuWnd()
{
	if (m_pChild)
		delete m_pChild;
}

HWND CUIMenuWnd::Create(HWND hParent, LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, bool bDropShadow)
{
	static CWndClassInfo wcs[] =
	{
		{ { sizeof(WNDCLASSEX), CS_SAVEBITS, StartWindowProc, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL }, NULL, NULL, IDC_ARROW, TRUE, 0, _T("") },
		{ { sizeof(WNDCLASSEX), CS_SAVEBITS | CS_DROPSHADOW, StartWindowProc, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL }, NULL, NULL, IDC_ARROW, TRUE, 0, _T("") }
	};

	return __super::Create(hParent, lpRect, NULL, dwStyle, dwExStyle, 0U, wcs[bDropShadow].Register(&m_pfnSuperWindowProc));
}

bool CUIMenuWnd::Init(HWND hParent, int x1, int y1, int x2, int y2)
{
	// 计算窗口大小
	int nWidth = 0, nHeight = 0;

	for (int i = 0; i != m_pUIMenu->m_vecItems.size(); i++)
	{
		CSize size;
		m_pUIMenu->MeasureItem(i, &size);

		if (nWidth < size.cx)
			nWidth = size.cx;

		nHeight += size.cy;
		m_pUIMenu->m_vecItems[i].m_nHeight = size.cy;
	}

	CRect rcInf;
	m_pUIMenu->InflateRect(rcInf);

	nWidth += rcInf.Width();
	nHeight += rcInf.Height();

	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(MonitorFromPoint(CPoint(x1, y1), MONITOR_DEFAULTTONEAREST), &mi);

	if (x1 + nWidth > mi.rcWork.right)
		x1 = (x2 != MAXINT16 ? x2 : mi.rcWork.right) - nWidth;

	if (y1 + nHeight > mi.rcWork.bottom)
		y1 = (y2 != MAXINT16 ? y2 : mi.rcWork.bottom) - nHeight;

	// 创建并显示窗口
	HWND hWnd = Create(hParent, CRect(x1, y1, x1 + nWidth, y1 + nHeight), WS_POPUP, WS_EX_TOOLWINDOW | (m_pUIMenu->GetWndAlpha() ? WS_EX_LAYERED : 0), m_pUIMenu->IsDropShadow());
	if (hWnd == NULL)
		return false;

	m_imageWnd.Create(nWidth, nHeight, 32, CImage::createAlphaChannel);
	OnSelChanged(-1, true);
	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	return true;
}

UINT CUIMenuWnd::Popup(HWND hParent, int x1, int y1, int x2, int y2, bool bPostMsg)
{
	if (!Init(hParent, x1, y1, x2, y2))
		return 0;

	SetCapture();
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	GetCursorPos(&m_ptMouse);	// 设置初始位置，防止菜单弹出就选中
	ScreenToClient(&m_ptMouse);

	MSG msg;
	HWND hOldWnd = NULL;

	while (m_hWnd && GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_SELECT && IsMenuWnd(msg.hwnd))
		{
			DestroyWindow();

			if (bPostMsg)
				::PostMessage(hParent, WM_COMMAND, msg.wParam, 0);

			return msg.wParam;
		}

		if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
		{
			msg.hwnd = m_hWnd;
		}

		if ((msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) && msg.wParam == VK_MENU)
		{
			DestroyWindow();
			break;
		}

		if (msg.message >= WM_MOUSEMOVE && msg.message <= WM_RBUTTONDBLCLK)
		{
			CPoint point(msg.pt);
			HWND hWnd = WindowFromPoint(point);

			if (IsMenuWnd(hWnd))
			{
				if (hWnd != m_hWnd)
				{
					::ScreenToClient(hWnd, &point);
					msg.hwnd = hWnd;
					msg.lParam = MAKELPARAM(point.x, point.y);
				}

				DispatchMessage(&msg);
			}
			else
			{
				if (msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN)
				{
					DestroyWindow();	// 必须在 PostMessage 之前销毁菜单

					if (GetWindowThreadProcessId(GetAncestor(hWnd, GA_ROOT), NULL) == GetWindowThreadProcessId(GetAncestor(hParent, GA_ROOT), NULL))
					{
						LRESULT nHit = ::SendMessage(hWnd, WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));

						if (nHit == HTCLIENT)
						{
							::ScreenToClient(hWnd, &point);
							::PostMessage(hWnd, msg.message, msg.wParam, MAKELPARAM(point.x, point.y));
						}
						else if (nHit >= HTCAPTION)
						{
							::PostMessage(hWnd, msg.message == WM_LBUTTONDOWN ? WM_NCLBUTTONDOWN : WM_NCRBUTTONDOWN, nHit, MAKELPARAM(point.x, point.y));
						}
					}

					break;
				}

				hWnd = NULL;
			}

			if (msg.message == WM_MOUSEMOVE)
			{
				// 通知旧窗口鼠标已离开
				if (hOldWnd && hOldWnd != hWnd)
				{
					msg.hwnd = hOldWnd;
					msg.lParam = MAKELPARAM(MAXINT16, MAXINT16);
					DispatchMessage(&msg);
				}

				hOldWnd = hWnd;
			}

			continue;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

bool CUIMenuWnd::IsMenuWnd(HWND hWnd) const
{
	return m_hWnd == hWnd || m_pChild && *m_pChild && m_pChild->IsMenuWnd(hWnd);
}

LRESULT CUIMenuWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_imageWnd.Destroy();
	m_ptMouse = CPoint(MAXINT16, MAXINT16);
	m_nItemId = -1;
	m_nCurSel = -1;
	return 0;
}

LRESULT CUIMenuWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	CUIPaintDC dc(m_hWnd);
	m_imageWnd.BitBlt(dc, *dc.PaintRect(), *(POINT *)dc.PaintRect());
	return 0;
}

LRESULT CUIMenuWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	switch (wParam)
	{
	case TIMER_ID:
		OnTimeOut(false);
	}

	return 0;
}

LRESULT CUIMenuWnd::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pChild && *m_pChild)
	{
		m_pChild->OnChar(uMsg, wParam, lParam, bHandled);
	}
	else if (GetKeyState(VK_LBUTTON) >= 0)
	{
		if (isgraph(wParam))
		{
			wchar_t szKey[4] = { '&', (wchar_t)tolower(wParam) };

			for (int i = 0; i != m_pUIMenu->m_vecItems.size(); i++)
			{
				const auto &item = m_pUIMenu->m_vecItems[i];

				wchar_t szText[256];
				wcsncpy_s(szText, item.m_strText.c_str(), _countof(szText) - 1);
				CharLowerW(szText);

				if (item.m_bEnabled && item.m_nId && wcsstr(szText, szKey))
				{
					if (item.GetSubMenu())
					{
						OnSelChanged(i, false);
						OnTimeOut(true);
						break;
					}
					else
					{
						PostMessage(WM_SELECT, item.m_nId);
						break;
					}
				}
			}
		}
	}

	return 0;
}

LRESULT CUIMenuWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pChild && *m_pChild)
	{
		m_pChild->OnKeyDown(uMsg, wParam, lParam, bHandled);
	}
	else if (GetKeyState(VK_LBUTTON) >= 0)
	{
		switch (wParam)
		{
		case VK_UP:
			for (int nIndex = m_nCurSel == -1 ? 0 : m_nCurSel, nCount = m_pUIMenu->m_vecItems.size(), i = 0; i != nCount; i++)
			{
				nIndex = (nIndex - 1 + nCount) % nCount;

				if (m_pUIMenu->m_vecItems[nIndex].m_bEnabled && m_pUIMenu->m_vecItems[nIndex].m_nId)
				{
					OnSelChanged(nIndex, false);
					break;
				}
			}
			break;

		case VK_DOWN:
			for (int nIndex = m_nCurSel, nCount = m_pUIMenu->m_vecItems.size(), i = 0; i != nCount; i++)
			{
				nIndex = (nIndex + 1) % nCount;

				if (m_pUIMenu->m_vecItems[nIndex].m_bEnabled && m_pUIMenu->m_vecItems[nIndex].m_nId)
				{
					OnSelChanged(nIndex, false);
					break;
				}
			}
			break;

		case VK_LEFT:
			if (m_pParent == NULL)
				break;

		case VK_ESCAPE:
			PostMessage(WM_CLOSE);
			break;

		case VK_RIGHT:
		case VK_RETURN:
			if (m_nCurSel != -1)
			{
				if (m_pUIMenu->m_vecItems[m_nCurSel].GetSubMenu())
				{
					OnTimeOut(false);
					OnKeyDown(WM_KEYDOWN, VK_DOWN, 1, bHandled);	// 选中子菜单第1项
				}
				else if (wParam == VK_RETURN)
				{
					PostMessage(WM_SELECT, m_pUIMenu->m_vecItems[m_nCurSel].m_nId);
				}
			}
			break;
		}
	}

	return 0;
}

LRESULT CUIMenuWnd::OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CUIMenuWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return MA_NOACTIVATE;
}

LRESULT CUIMenuWnd::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	UINT nCurSel = -1;
	CPoint point(lParam);

	if (m_ptMouse == point || (nCurSel = Point2Sel(m_ptMouse = point)) == m_nCurSel)
		return 0;

	if (nCurSel != -1)
	{
		if (m_pParent)
			m_pParent->OnSelChanged(m_nItemId, false);	// 确保父菜单已选中

		SetTimer(TIMER_ID, m_pUIMenu->GetShowDelay(), NULL);
	}
	else if (m_pChild && *m_pChild && m_pChild->m_nItemId == m_nCurSel)
	{
		return 0;	// 已弹出子菜单
	}

	OnSelChanged(nCurSel, false);
	return 0;
}

LRESULT CUIMenuWnd::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	UINT nCurSel = Point2Sel(lParam);

	if (nCurSel != -1)
	{
		OnSelChanged(nCurSel, false);
		OnTimeOut(true);
	}

	return 0;
}

LRESULT CUIMenuWnd::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	UINT nCurSel = Point2Sel(lParam);

	if (nCurSel != -1 && m_pUIMenu->m_vecItems[nCurSel].GetSubMenu() == NULL)
	{
		PostMessage(WM_SELECT, m_pUIMenu->m_vecItems[nCurSel].m_nId);
	}

	return 0;
}

void CUIMenuWnd::ResetSel()
{
	if (m_pChild && *m_pChild)
	{
		m_pChild->DestroyWindow();
		OnSelChanged(-1, false);
	}
}

void CUIMenuWnd::OnTimeOut(bool bReset)
{
	KillTimer(TIMER_ID);

	if (m_nCurSel != -1 && m_pUIMenu->m_vecItems[m_nCurSel].GetSubMenu())	// 选中项有子菜单
	{
		if (m_pChild && *m_pChild)	// 已经弹出子菜单
		{
			if (m_pChild->m_nItemId == m_nCurSel)	// 是选中项的子菜单
			{
				if (bReset)
					m_pChild->ResetSel();
				return;
			}

			m_pChild->DestroyWindow();	// 关闭不需要的子菜单
		}

		CRect rcInf;
		m_pUIMenu->InflateRect(rcInf);
		int nSpace = m_pUIMenu->GetHoriSpace();

		// 计算选中项的矩形
		CRect rect;
		GetWindowRect(rect);

		for (int i = 0; i != m_nCurSel; i++)
		{
			rect.top += m_pUIMenu->m_vecItems[i].m_nHeight;
		}

		rect.bottom = rect.top + m_pUIMenu->m_vecItems[m_nCurSel].m_nHeight + rcInf.Height();

		// 弹出子菜单
		if (m_pChild == NULL)
			m_pChild = new CUIMenuWnd(NULL);

		m_pChild->m_pUIMenu = m_pUIMenu->m_vecItems[m_nCurSel].GetSubMenu();
		m_pChild->m_pParent = this;
		m_pChild->m_nItemId = m_nCurSel;

		m_pChild->Init(m_hWnd, rect.right + nSpace, rect.top, rect.left - nSpace, rect.bottom);
	}
	else if (m_pChild && *m_pChild)
	{
		m_pChild->DestroyWindow();
	}
}

UINT CUIMenuWnd::Point2Sel(CPoint point) const
{
	CRect rcInf;
	m_pUIMenu->InflateRect(rcInf);

	CRect rect(0, 0, m_imageWnd.GetWidth(), m_imageWnd.GetHeight());
	rect.DeflateRect(-rcInf.left, -rcInf.top, rcInf.right, rcInf.bottom);

	if (rect.PtInRect(point))
	{
		int cy = point.y - rect.top, nHeight = 0;

		for (int i = 0; i != m_pUIMenu->m_vecItems.size(); i++)
		{
			const auto &item = m_pUIMenu->m_vecItems[i];
			nHeight += item.m_nHeight;

			if (cy < nHeight)
				return item.m_bEnabled && item.m_nId ? i : -1;
		}
	}

	return -1;
}

void CUIMenuWnd::OnSelChanged(UINT nCurSel, bool bInit)
{
	if (nCurSel == m_nCurSel && !bInit)
		return;

	CRect rcInf;
	m_pUIMenu->InflateRect(rcInf);
	BYTE nAlpha = m_pUIMenu->GetWndAlpha();

	int nWidth = m_imageWnd.GetWidth(), nHeight = m_imageWnd.GetHeight();
	CImage image;
	image.Create(nWidth, nHeight, 32, CImage::createAlphaChannel);

	CUIDC dc(image.GetDC(), NULL, true);
	SetBkMode(dc, TRANSPARENT);

	CRect rect(0, 0, nWidth, nHeight);
	m_pUIMenu->DrawBg(dc, rect);
	rect.DeflateRect(-rcInf.left, -rcInf.top, rcInf.right, rcInf.bottom);

	for (int i = 0; i != m_pUIMenu->m_vecItems.size(); i++)
	{
		rect.bottom = rect.top + m_pUIMenu->m_vecItems[i].m_nHeight;

		if (bInit)
		{
			m_pUIMenu->DrawItem(dc, rect, i, false);
		}
		else if (i == m_nCurSel || i == nCurSel)
		{
			m_pUIMenu->DrawItem(dc, rect, i, i == nCurSel);
			BitBlt(CImageDC(m_imageWnd), rect.left, rect.top, rect.Width(), rect.Height(), dc, rect.left, rect.top, SRCCOPY);

			if (nAlpha == 0)
				InvalidateRect(rect);
		}

		rect.top = rect.bottom;
	}

	if (bInit)
		BitBlt(CImageDC(m_imageWnd), 0, 0, nWidth, nHeight, dc, 0, 0, SRCCOPY);

	dc.Delete();
	image.ReleaseDC();
	m_nCurSel = nCurSel;

	if (nAlpha)
	{
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, nAlpha, AC_SRC_ALPHA };
		UpdateLayeredWindow(m_hWnd, CUIClientDC(m_hWnd), NULL, &CSize(nWidth, nHeight), CImageDC(m_imageWnd), &CPoint(), 0, &bf, ULW_ALPHA);
	}
}
