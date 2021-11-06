#include "stdafx.h"
#include "UIShadowWnd.h"

#define ID_TIMER_SHOW	100

CUIShadowWnd::CUIShadowWnd(const CImagex &shadowImage, int nShadowSize) : m_shadowImage(shadowImage), m_nShadowSize(nShadowSize), m_hOwnerWnd(NULL), m_nOwnerType(SIZE_RESTORED), m_bNoUpdate(FALSE)
{
	ATLASSERT(shadowImage.Image().GetBPP() == 32);
}

void CUIShadowWnd::CreateWnd(HWND hOwnerWnd, BOOL bThickFrame)
{
	m_hOwnerWnd = hOwnerWnd;
	Create(hOwnerWnd, CRect(), NULL, WS_POPUP | (bThickFrame ? WS_THICKFRAME : 0), WS_EX_LAYERED | WS_EX_NOACTIVATE);

	// 初始延迟显示
	if (::IsIconic(hOwnerWnd))
		m_nOwnerType = SIZE_MINIMIZED;
	else if (::IsZoomed(hOwnerWnd))
		m_nOwnerType = SIZE_MAXIMIZED;
	else
		m_bNoUpdate = SetTimer(ID_TIMER_SHOW, 250, NULL);
}

void CUIShadowWnd::OnOwnerSize(int nNewType)
{
	if (m_nOwnerType != nNewType)
	{
		int nOldType = m_nOwnerType;
		m_nOwnerType = nNewType;

		// 最小化 => 还原，延迟显示
		if (nOldType == SIZE_MINIMIZED && nNewType == SIZE_RESTORED)
			m_bNoUpdate = SetTimer(ID_TIMER_SHOW, 250, NULL);

		// 最大化 => 还原，延迟显示
		if (nOldType != SIZE_MINIMIZED && nNewType == SIZE_RESTORED)
			m_bNoUpdate = SetTimer(ID_TIMER_SHOW, 200, NULL);

		// 还原 => 最大化，立即隐藏
		if (nOldType == SIZE_RESTORED && nNewType != SIZE_MINIMIZED)
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
	}
}

void CUIShadowWnd::OnOwnerPosChanged(const WINDOWPOS *lpWndPos)
{
	if (m_nOwnerType != SIZE_RESTORED || m_bNoUpdate)
		return;

	int nFlags = lpWndPos->flags & (SWP_NOMOVE | SWP_SHOWWINDOW);

	if ((lpWndPos->flags & SWP_NOSIZE) == 0)
	{
		// 位置、大小一起更新
		Update(nFlags & SWP_NOMOVE ? NULL : &CPoint(lpWndPos->x - m_nShadowSize, lpWndPos->y - m_nShadowSize), CSize(lpWndPos->cx + m_nShadowSize * 2, lpWndPos->cy + m_nShadowSize * 2));
		nFlags |= SWP_NOMOVE;
	}

	if (nFlags != SWP_NOMOVE)
		SetWindowPos(NULL, lpWndPos->x - m_nShadowSize, lpWndPos->y - m_nShadowSize, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE | nFlags);
}

LRESULT CUIShadowWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (GetWindowLong(GWL_STYLE) & WS_THICKFRAME)
	{
		CPoint point(lParam);
		CRect rect;
		GetWindowRect(rect);
		rect.DeflateRect(m_nShadowSize, m_nShadowSize);

		int nFlags = 0;

		if (point.x < rect.left)
			nFlags |= 1;
		else if (point.x >= rect.right)
			nFlags |= 2;

		if (point.y < rect.top)
			nFlags |= 4;
		else if (point.y >= rect.bottom)
			nFlags |= 8;

		switch (nFlags)
		{
		case 1:
			return HTLEFT;
		case 2:
			return HTRIGHT;
		case 4:
			return HTTOP;
		case 5:
			return HTTOPLEFT;
		case 6:
			return HTTOPRIGHT;
		case 8:
			return HTBOTTOM;
		case 9:
			return HTBOTTOMLEFT;
		case 10:
			return HTBOTTOMRIGHT;
		}
	}

	return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT CUIShadowWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	MINMAXINFO *lpMMI = (MINMAXINFO *)lParam, mmi = *lpMMI;
	::SendMessage(m_hOwnerWnd, WM_GETMINMAXINFO, wParam, (LPARAM)&mmi);

	lpMMI->ptMinTrackSize.x = mmi.ptMinTrackSize.x + m_nShadowSize * 2;
	lpMMI->ptMinTrackSize.y = mmi.ptMinTrackSize.y + m_nShadowSize * 2;
	return 0;
}

LRESULT CUIShadowWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (wParam == ID_TIMER_SHOW)
	{
		KillTimer(wParam);
		m_bNoUpdate = FALSE;

		if (m_nOwnerType == SIZE_RESTORED && ::IsWindowVisible(m_hOwnerWnd))
		{
			CRect rect;
			::GetWindowRect(m_hOwnerWnd, rect);
			rect.InflateRect(m_nShadowSize, m_nShadowSize);
			Update(&rect.TopLeft(), rect.Size());
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
	}

	return 0;
}

LRESULT CUIShadowWnd::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	WINDOWPOS *lpWndPos = (WINDOWPOS *)lParam;

	if ((lpWndPos->flags & (SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE)) == 0)
	{
		CRect rect;
		GetWindowRect(rect);

		// 防止 y < 0 时自动移到 0
		if (rect.top < lpWndPos->y && lpWndPos->y <= 0 && (lpWndPos->cy != rect.Height() || lpWndPos->cx == rect.Width()))
			lpWndPos->flags |= SWP_NOMOVE | SWP_NOSIZE;
	}

	if (m_nOwnerType != SIZE_RESTORED || m_bNoUpdate)
		lpWndPos->flags &= ~SWP_SHOWWINDOW;

	return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT CUIShadowWnd::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	WINDOWPOS *lpWndPos = (WINDOWPOS *)lParam;

	if ((lpWndPos->flags & SWP_NOSIZE) == 0)
	{
		Update(NULL, CSize(lpWndPos->cx, lpWndPos->cy));

		m_bNoUpdate = TRUE;
		int nNoMove = lpWndPos->flags & SWP_NOMOVE;
		::SetWindowPos(m_hOwnerWnd, NULL, lpWndPos->x + m_nShadowSize, lpWndPos->y + m_nShadowSize, lpWndPos->cx - m_nShadowSize * 2, lpWndPos->cy - m_nShadowSize * 2, SWP_NOACTIVATE | SWP_NOZORDER | nNoMove);
		m_bNoUpdate = FALSE;
	}

	return DefWindowProc(uMsg, wParam, lParam);
}

void CUIShadowWnd::Update(POINT *pPoint, SIZE &size) const
{
	CImage image;
	image.Create(size.cx, size.cy, 32, CImage::createAlphaChannel);

	HDC hImgDC = image.GetDC();
	ExcludeClipRect(hImgDC, m_nShadowSize, m_nShadowSize, size.cx - m_nShadowSize, size.cy - m_nShadowSize);
	m_shadowImage.Scale9Draw(hImgDC, CRect(0, 0, size.cx, size.cy), false);
	SelectClipRgn(hImgDC, NULL);

	HDC hDC = ::GetDC(m_hWnd);
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	UpdateLayeredWindow(m_hWnd, hDC, pPoint, &size, hImgDC, &CPoint(), 0, &bf, ULW_ALPHA);
	::ReleaseDC(m_hWnd, hDC);
	image.ReleaseDC();
}
