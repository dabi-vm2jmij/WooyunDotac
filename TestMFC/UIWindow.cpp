// UIWindow.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UIWindow.h"

static UINT g_nMsgId;

// CUIWindow

IMPLEMENT_DYNAMIC(CUIWindow, CWnd)

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

BEGIN_MESSAGE_MAP(CUIWindow, CWnd)
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

bool CUIWindow::CreateFromXml(LPCWSTR lpXmlName, HWND hParent)
{
	m_hWndParent = hParent;
	return UILib::LoadFromXml(lpXmlName, &m_rootView);
}

void CUIWindow::OnLoadUI(const IUIXmlAttrs &attrs)
{
	LPCWSTR lpFileName, lpNameEnd;

	// �߿�ͼƬ����С
	if ((lpFileName = attrs.GetStr(L"border")) && (lpNameEnd = wcsrchr(lpFileName, '|')))
	{
		m_borderImage = UILib::GetImage(wstring(lpFileName, lpNameEnd).c_str());
		m_nBorderSize = _wtoi(lpNameEnd + 1);
	}
	else
		ATLASSERT(lpFileName == NULL);

	m_nCaptionSize = attrs.GetInt(L"caption");

	auto pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"�ر�"));
	if (pButton)
		pButton->BindClick([this]{ this->PostMessage(WM_SYSCOMMAND, SC_CLOSE); });

	pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"��С��"));
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
	CreateEx(dwExStyle, NULL, W2CT(lpszTitle), dwStyle, 0, 0, nWidth, nHeight, m_hWndParent, 0);
}

// CUIWindow ��Ϣ�������

BOOL CUIWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (cs.lpszClass == NULL)
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, LoadCursor(NULL, IDC_ARROW));

	return __super::PreCreateWindow(cs);
}

BOOL CUIWindow::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (message && message == g_nMsgId)
	{
		*pResult = (LRESULT)this;
		return TRUE;
	}

	if (m_rootView.ProcessWindowMessage(m_hWnd, message, wParam, lParam, *pResult))
		return TRUE;

	switch (message)
	{
	case WM_ERASEBKGND:
		*pResult = TRUE;

	case WM_NCPAINT:
	case 0x00AE:	// WM_NCUAHDRAWCAPTION
	case 0x00AF:	// WM_NCUAHDRAWFRAME
		return TRUE;
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

BOOL CUIWindow::OnNcActivate(BOOL bActive)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
//	return __super::OnNcActivate(bActive);
}

void CUIWindow::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (!IsZoomed())
		return;

	ATLASSERT((GetStyle() & WS_CAPTION) == WS_CAPTION);

	LPRECT lpRect = lpncsp->rgrc;
	long nSum = lpRect->top + lpRect->bottom;
	__super::OnNcCalcSize(bCalcValidRects, lpncsp);
	lpRect->top = nSum - lpRect->bottom;
}

LRESULT CUIWindow::OnNcHitTest(CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
			return GetStyle() & WS_THICKFRAME ? nHit : HTBORDER;
	}

	if (m_nCaptionSize < 0 || m_nCaptionSize > 0 && point.y < rect.top + m_nCaptionSize)
		return HTCAPTION;

	return __super::OnNcHitTest(point);
}

void CUIWindow::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� __super::OnPaint()
	if (m_pLayered)
		UpdateLayered(dc);
	else
		DoPaint(CUIDC(CUIMemDC(dc, &dc.m_ps.rcPaint), -(CPoint &)dc.m_ps.rcPaint, NULL));
}

void CUIWindow::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	if (nType == SIZE_MINIMIZED)
		return;

	SetWindowRgn(nType == SIZE_RESTORED ? CreateRectRgn(0, 0, cx, cy) : NULL, TRUE);

	// ���� RootView ���򣬻�ԭʱ�����߿�
	CRect rect(0, 0, cx, cy), clipRect;
	if (nType == SIZE_RESTORED)
		rect.DeflateRect(m_nBorderSize, m_nBorderSize);
	m_rootView.SetRect(rect, clipRect);

	if (m_pLayered)
	{
		CRect rcWnd;
		GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);

		// ��С��ԭ��ı���ؽ�����ͼ������¼ԭ��
		CImage &imageWnd = m_pLayered->m_imageWnd;

		if (imageWnd.IsNull() || imageWnd.GetWidth() != rcWnd.Width() || imageWnd.GetHeight() != rcWnd.Height() || m_pLayered->m_point != -rcWnd.TopLeft())
		{
			imageWnd.Destroy();
			imageWnd.Create(rcWnd.Width(), rcWnd.Height(), 32, CImage::createAlphaChannel);
			m_pLayered->m_clipRect = rcWnd;
			m_pLayered->m_point = -rcWnd.TopLeft();
		}

		UpdateLayered(CClientDC(this));
	}
	else if (!clipRect.IsRectEmpty())
	{
		InvalidateRect(clipRect);
	}
}

void CUIWindow::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);

	// TODO:  �ڴ˴������Ϣ����������
	if ((lpwndpos->flags & SWP_SHOWWINDOW) && m_pLayered && !m_pLayered->m_clipRect.IsRectEmpty())
	{
		UpdateLayered(CClientDC(this));
	}
}

void CUIWindow::InvalidateRect(LPCRECT lpRect)
{
	CWnd::InvalidateRect(lpRect);

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
		// ���ʱ�����߿�
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
		// ԭ���Ƶ��ͻ�����ֻ����Ч����
		CPoint point;
		SetViewportOrgEx(hImgDC, m_pLayered->m_point.x, m_pLayered->m_point.y, &point);
		IntersectClipRect(hImgDC, m_pLayered->m_clipRect.left, m_pLayered->m_clipRect.top, m_pLayered->m_clipRect.right, m_pLayered->m_clipRect.bottom);

		if (!m_borderImage)
		{
			// FillSolidRect ��������
			SetBkColor(hImgDC, 0);
			ExtTextOut(hImgDC, 0, 0, ETO_OPAQUE, m_pLayered->m_clipRect, NULL, 0, NULL);
		}

		DoPaint(CUIDC(hImgDC, m_pLayered->m_point, &imageWnd));
		SelectClipRgn(hImgDC, NULL);
		SetViewportOrgEx(hImgDC, point.x, point.y, NULL);
		m_pLayered->m_clipRect.SetRectEmpty();
	}

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	::UpdateLayeredWindow(m_hWnd, hDC, NULL, &CSize(imageWnd.GetWidth(), imageWnd.GetHeight()), hImgDC, &CPoint(), 0, &bf, ULW_ALPHA);
	imageWnd.ReleaseDC();
}
