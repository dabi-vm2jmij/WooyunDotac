#include "stdafx.h"
#include "UIWindow.h"
#include "UIShadowWnd.h"

static UINT g_nMsgId;

CUIWindow::CUIWindow() : m_rootView(this), m_hWndParent(NULL), m_nBorderSize(0), m_nCaptionSize(0)
{
}

CUIWindow::~CUIWindow()
{
}

CUIWindow *CUIWindow::FromHwnd(HWND hWnd)
{
	if (g_nMsgId == 0)
	{
		wchar_t szName[64];
		swprintf_s(szName, L"GetUIWindow_%d", GetCurrentProcessId());
		g_nMsgId = RegisterWindowMessageW(szName);
	}

	return (CUIWindow *)::SendMessage(hWnd, g_nMsgId, 0, 0);
}

bool CUIWindow::CreateFromXml(LPCWSTR lpXmlName, HWND hParent)
{
	m_hWndParent = hParent;
	return LoadFromXml(lpXmlName, &m_rootView);
}

void CUIWindow::OnLoadUI(const IUIXmlAttrs &attrs)
{
	LPCWSTR lpFileName, lpNameEnd;

	// 阴影窗口、大小
	if ((lpFileName = attrs.GetStr(L"shadow")) && (lpNameEnd = wcsrchr(lpFileName, '|')))
	{
		m_pShadowWnd = std::make_unique<CUIShadowWnd>(::GetImage(wstring(lpFileName, lpNameEnd).c_str()), _wtoi(lpNameEnd + 1));
	}
	else
		ATLASSERT(lpFileName == NULL);

	// 边框图片、大小
	if ((lpFileName = attrs.GetStr(L"border")) && (lpNameEnd = wcsrchr(lpFileName, '|')))
	{
		m_borderImage = ::GetImage(wstring(lpFileName, lpNameEnd).c_str());
		m_nBorderSize = _wtoi(lpNameEnd + 1);
	}
	else
		ATLASSERT(lpFileName == NULL);

	m_nCaptionSize = attrs.GetInt(L"caption");

	auto pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"关闭"));
	if (pButton)
		pButton->BindClick([this]{ this->PostMessage(WM_SYSCOMMAND, SC_CLOSE); });

	pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"最小化"));
	if (pButton)
		pButton->BindClick([this]{ this->PostMessage(WM_SYSCOMMAND, SC_MINIMIZE); });

	if (m_hWnd == NULL)
		DoCreate(attrs.GetInt(L"width"), attrs.GetInt(L"height"), attrs.GetStr(L"title"), attrs.GetInt(L"style"), attrs.GetInt(L"exStyle"));
}

void CUIWindow::DoCreate(int nWidth, int nHeight, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyle)
{
	if (dwExStyle & WS_EX_LAYERED)
		m_pLayered = std::make_unique<Layered>();

	USES_CONVERSION;
	Create(m_hWndParent, CRect(0, 0, nWidth, nHeight), W2CT(lpszTitle), dwStyle, dwExStyle);
}

BOOL CUIWindow::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult, DWORD dwMsgMapID)
{
	if (dwMsgMapID)
	{
		ATLASSERT(0);
		return FALSE;
	}

	if (uMsg && uMsg == g_nMsgId)
	{
		lResult = (LRESULT)this;
		return TRUE;
	}

	lResult = 0;

	if (m_rootView.ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult))
		return TRUE;

	switch (uMsg)
	{
	case WM_ERASEBKGND:
		lResult = TRUE;

	case WM_NCPAINT:
	case 0x00AE:	// WM_NCUAHDRAWCAPTION
	case 0x00AF:	// WM_NCUAHDRAWFRAME
		break;

	case WM_CREATE:
		OnCreate();
		break;

	case WM_CLOSE:
		OnClose();
		break;

	case WM_DESTROY:
		OnDestroy();
		break;

	case WM_SYSCOMMAND:
		OnSysCommand(wParam, lParam);
		break;

	case WM_GETMINMAXINFO:
		OnGetMinMaxInfo(reinterpret_cast<MINMAXINFO *>(lParam));
		break;

	case WM_SETFOCUS:
		OnSetFocus(reinterpret_cast<HWND>(wParam));
		break;

	case WM_KILLFOCUS:
		OnKillFocus(reinterpret_cast<HWND>(wParam));
		break;

	case WM_NCACTIVATE:
		lResult = OnNcActivate(wParam);
		break;

	case WM_NCCALCSIZE:
		OnNcCalcSize(wParam, reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam));
		break;

	case WM_NCHITTEST:
		lResult = OnNcHitTest(lParam);
		break;

	case WM_PAINT:
		OnPaint();
		break;

	case WM_PRINTCLIENT:
		OnPrint((HDC)wParam);
		break;

	case WM_SIZE:
		OnSize(wParam, lParam);
		break;

	case WM_TIMER:
		OnTimer(wParam);
		break;

	case WM_WINDOWPOSCHANGING:
		OnWindowPosChanging(reinterpret_cast<WINDOWPOS *>(lParam));
		break;

	case WM_WINDOWPOSCHANGED:
		OnWindowPosChanged(reinterpret_cast<WINDOWPOS *>(lParam));
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

void CUIWindow::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpNCSP)
{
	if (!IsZoomed())
		return;

	ATLASSERT((GetWindowLong(GWL_STYLE) & WS_CAPTION) == WS_CAPTION);

	LPRECT lpRect = lpNCSP->rgrc;
	long nSum = lpRect->top + lpRect->bottom;
	DefWindowProc();
	lpRect->top = nSum - lpRect->bottom;
}

long CUIWindow::OnNcHitTest(CPoint point)
{
	CRect rect = m_rootView.GetRect();
	ClientToScreen(rect);

	if (!IsZoomed())
	{
		int nFlags = 0;

		if (point.x < rect.left)
			nFlags |= 1;
		else if (point.x >= rect.right)
			nFlags |= 2;

		if (point.y < rect.top)
			nFlags |= 4;
		else if (point.y >= rect.bottom)
			nFlags |= 8;

		long nHit = HTNOWHERE;

		switch (nFlags)
		{
		case 1:
			nHit = HTLEFT;
			break;

		case 2:
			nHit = HTRIGHT;
			break;

		case 4:
			nHit = HTTOP;
			break;

		case 5:
			nHit = HTTOPLEFT;
			break;

		case 6:
			nHit = HTTOPRIGHT;
			break;

		case 8:
			nHit = HTBOTTOM;
			break;

		case 9:
			nHit = HTBOTTOMLEFT;
			break;

		case 10:
			nHit = HTBOTTOMRIGHT;
		}

		if (nHit != HTNOWHERE)
			return GetWindowLong(GWL_STYLE) & WS_THICKFRAME ? nHit : HTBORDER;
	}

	if (m_nCaptionSize < 0 || m_nCaptionSize > 0 && point.y < rect.top + m_nCaptionSize)
		return HTCAPTION;

	return DefWindowProc();
}

void CUIWindow::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(&ps);

	if (m_pLayered)
		UpdateLayered(hDC);
	else
		DoPaint(CUIDC(CUIMemDC(hDC, &ps.rcPaint), -(CPoint &)ps.rcPaint, NULL));

	EndPaint(&ps);
}

void CUIWindow::OnPrint(HDC hDC)
{
	if (m_pLayered)
		UpdateLayered(hDC);
	else
		DoPaint(CUIDC(hDC, CPoint(), NULL));
}

void CUIWindow::OnSize(UINT nType, CSize size)
{
	DefWindowProc();

	if (m_pShadowWnd && m_pShadowWnd->m_hWnd)
		m_pShadowWnd->OnOwnerSize(nType);

	if (nType == SIZE_MINIMIZED)
		return;

	SetWindowRgn(nType == SIZE_RESTORED ? CreateRectRgn(0, 0, size.cx, size.cy) : NULL, TRUE);

	// 设置 RootView 区域，还原时缩进边框
	CRect rect(0, 0, size.cx, size.cy), clipRect;
	if (nType == SIZE_RESTORED)
		rect.DeflateRect(m_nBorderSize, m_nBorderSize);
	m_rootView.SetRect(rect, clipRect);

	if (m_pLayered)
	{
		CRect rcWnd;
		GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);

		// 大小或原点改变后重建缓存图，并记录原点
		CImage &imageWnd = m_pLayered->m_imageWnd;

		if (imageWnd.IsNull() || imageWnd.GetWidth() != rcWnd.Width() || imageWnd.GetHeight() != rcWnd.Height() || m_pLayered->m_point != -rcWnd.TopLeft())
		{
			imageWnd.Destroy();
			imageWnd.Create(rcWnd.Width(), rcWnd.Height(), 32, CImage::createAlphaChannel);
			m_pLayered->m_clipRect = rcWnd;
			m_pLayered->m_point = -rcWnd.TopLeft();
		}

		HDC hDC = GetDC();
		UpdateLayered(hDC);
		ReleaseDC(hDC);
	}
	else if (!clipRect.IsRectEmpty())
	{
		InvalidateRect(clipRect);
	}
}

void CUIWindow::OnWindowPosChanged(WINDOWPOS *lpWndPos)
{
	DefWindowProc();

	if (m_pShadowWnd)
	{
		if (m_pShadowWnd->m_hWnd)
			m_pShadowWnd->OnOwnerPosChanged(lpWndPos);
		else if (lpWndPos->flags & SWP_SHOWWINDOW)
			m_pShadowWnd->CreateWnd(m_hWnd, GetWindowLong(GWL_STYLE) & WS_THICKFRAME);
	}

	if ((lpWndPos->flags & SWP_SHOWWINDOW) && m_pLayered && !m_pLayered->m_clipRect.IsRectEmpty())
	{
		HDC hDC = GetDC();
		UpdateLayered(hDC);
		ReleaseDC(hDC);
	}
}

void CUIWindow::InvalidateRect(LPCRECT lpRect)
{
	CWindow::InvalidateRect(lpRect);

	if (m_pLayered)
	{
		ATLASSERT(lpRect);
		m_pLayered->m_clipRect |= *lpRect;
	}
}

void CUIWindow::DoPaint(CUIDC &dc) const
{
	if (m_borderImage)
	{
		// 最大化时不画边框
		CImagex image(m_borderImage);
		if (IsZoomed())
			((CRect &)image.Rect()).DeflateRect(m_nBorderSize, m_nBorderSize);

		CRect rect;
		GetClientRect(rect);
		image.Scale9Draw(dc, rect, false);
	}

	m_rootView.DoPaint(dc);
}

void CUIWindow::UpdateLayered(HDC hDC)
{
	CImage &imageWnd = m_pLayered->m_imageWnd;

	if (!m_pLayered || imageWnd.IsNull())
		return;

	HDC hImgDC = imageWnd.GetDC();

	if (!m_pLayered->m_clipRect.IsRectEmpty())
	{
		// 原点移到客户区，只画无效区域
		CPoint point;
		SetViewportOrgEx(hImgDC, m_pLayered->m_point.x, m_pLayered->m_point.y, &point);
		IntersectClipRect(hImgDC, m_pLayered->m_clipRect.left, m_pLayered->m_clipRect.top, m_pLayered->m_clipRect.right, m_pLayered->m_clipRect.bottom);

		if (!m_borderImage)
		{
			// FillSolidRect 擦除背景
			SetBkColor(hImgDC, 0);
			ExtTextOut(hImgDC, 0, 0, ETO_OPAQUE, m_pLayered->m_clipRect, NULL, 0, NULL);
		}

		DoPaint(CUIDC(hImgDC, m_pLayered->m_point, &imageWnd));
		SelectClipRgn(hImgDC, NULL);
		SetViewportOrgEx(hImgDC, point.x, point.y, NULL);
		m_pLayered->m_clipRect.SetRectEmpty();
	}

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	UpdateLayeredWindow(m_hWnd, hDC, NULL, &CSize(imageWnd.GetWidth(), imageWnd.GetHeight()), hImgDC, &CPoint(), 0, &bf, ULW_ALPHA);
	imageWnd.ReleaseDC();
}
