#include "stdafx.h"
#include "UIRootView.h"
#include "UIToolTip.h"

UINT CUIRootView::m_nLayoutMsgId = RegisterWindowMessage(_T("UILibLayout"));

CUIRootView::CUIRootView(IUIWindow *pWindow) : CUIView(NULL), m_pWindow(pWindow), m_nWndAlpha(255), m_bLayered(false), m_bMouseEnter(false), m_bPostLayout(false), m_hImc(NULL), m_hCursor(NULL)
	, m_pCapture(NULL), m_pCurFocus(NULL)
{
	m_vecEnterItems.reserve(8);
}

CUIRootView::~CUIRootView()
{
	for (auto pItem : m_vecChilds)
		delete pItem;

	m_vecChilds.clear();
}

BOOL CUIRootView::OnWndMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
	lResult = 0;

	if (uMsg == m_nLayoutMsgId)
	{
		m_bPostLayout = false;

		// 重新布局所有延迟布局的控件
		CRect rect;
		IsNeedLayout(rect);

		if (!rect.IsRectEmpty())
			InvalidateRect(rect);

		if (m_pCapture == NULL)
			RaiseMouseMove();

		return TRUE;
	}

	switch (uMsg)
	{
	case WM_CREATE:
		if (GetWindowLong(GetHwnd(), GWL_EXSTYLE) & WS_EX_LAYERED)
			m_bLayered = true;
		break;

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
			OnSize(lParam);
		break;

	case WM_PAINT:
		OnPaint();

	case WM_NCPAINT:
	case 0x00AE:	// WM_NCUAHDRAWCAPTION
	case 0x00AF:	// WM_NCUAHDRAWFRAME
		return TRUE;

	case WM_ERASEBKGND:
		lResult = TRUE;
		return TRUE;

	case WM_NCHITTEST:
		if (OnNcHitTest(lParam))
		{
			lResult = HTCLIENT;
			return TRUE;
		}
		break;

	case WM_SETCURSOR:
		if (m_hCursor)
		{
			SetCursor(m_hCursor);
			lResult = TRUE;
			return TRUE;
		}
		break;

	case WM_MOUSELEAVE:
		m_bMouseEnter = false;
		CheckMouseLeave(UIHitTest());
		CUIToolTip::ShowTip(GetHwnd(), L"");
		return TRUE;

	case WM_MOUSEMOVE:
		if (!m_bMouseEnter)
		{
			TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, GetHwnd() };
			m_bMouseEnter = TrackMouseEvent(&tme) != 0;
		}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		if (m_pCapture)
			m_pCapture->OnMessage(uMsg, wParam, lParam);
		else
			OnMouseMessage(uMsg, wParam, lParam);
		return TRUE;

	case WM_MOUSEWHEEL:
	case WM_CONTEXTMENU:
		{
			CPoint point(lParam);
			ScreenToClient(GetHwnd(), &point);
			OnMessage(uMsg, wParam, MAKELPARAM(point.x, point.y));
		}
		return TRUE;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_IME_CHAR:
		if (m_pCurFocus)
		{
			m_pCurFocus->OnMessage(uMsg, wParam, lParam);
			return TRUE;
		}
		break;

	case WM_SETFOCUS:
		if (m_pCurFocus)
			m_pCurFocus->OnSetFocus();
		break;

	case WM_KILLFOCUS:
		if (m_pCurFocus)
		{
			m_pCurFocus->OnKillFocus();

			if (wParam && ::IsChild(GetHwnd(), (HWND)wParam))
				m_pCurFocus = NULL;
		}
		break;

	case WM_PARENTNOTIFY:
		if (m_pCurFocus)
		{
			switch (LOWORD(wParam))
			{
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				m_pCurFocus->OnKillFocus();
				m_pCurFocus = NULL;
			}
		}
		break;

	case WM_INPUTLANGCHANGE:
		if (m_pCurFocus && wParam)
		{
			if (CUIEdit *pEdit = dynamic_cast<CUIEdit *>(m_pCurFocus))
			{
				pEdit->OnInputLangChange(lParam);
				lResult = TRUE;
				return TRUE;
			}
		}
		break;

	case WM_CAPTURECHANGED:
		if (auto pCapture = m_pCapture)
		{
			m_pCapture = NULL;
			pCapture->OnLostCapture();
			RaiseMouseMove();
			return TRUE;
		}
		break;
	}

	return FALSE;
}

HWND CUIRootView::GetHwnd() const
{
	return m_pWindow->GetHwnd();
}

void CUIRootView::SetWndAlpha(BYTE nWndAlpha)
{
	if (m_nWndAlpha == nWndAlpha)
		return;

	m_nWndAlpha = nWndAlpha;

	if (m_bLayered)
		OnPaintLayered(CUIClientDC(GetHwnd()));
}

void CUIRootView::PrintWindow(CImage &image)
{
	if (m_rect.IsRectEmpty())
		return;

	image.Create(m_rect.Width(), m_rect.Height(), 32, CImage::createAlphaChannel);
	DoPaint(CImageDC(image), NULL);
}

bool CUIRootView::OnNcHitTest(CPoint point)
{
	UIHitTest hitTest;
	hitTest.point = point;
	ScreenToClient(GetHwnd(), &hitTest.point);
	OnHitTest(hitTest);

	m_hCursor = NULL;	// 用于 WM_SETCURSOR

	for (auto hit : hitTest)
	{
		if (hit.pItem->IsControl())
		{
			if (hit.bEnabled)
				m_hCursor = ((CUIControl *)hit.pItem)->m_hCursor;
			break;
		}
	}

	return hitTest.nCount != 0;
}

void CUIRootView::OnSize(CSize size)
{
	if (m_rect.Width() != size.cx || m_rect.Height() != size.cy)
	{
		// 这里不用 CalcRect，而是直接赋值
		m_rect = m_rcReal = CRect(0, 0, size.cx, size.cy);

		CRect rect;
		OnRectChanged(NULL, rect);

		if (!rect.IsRectEmpty())
			InvalidateRect(rect);

		m_imageWnd.Destroy();
	}

	if (m_bLayered)
		OnPaintLayered(CUIClientDC(GetHwnd()));
}

void CUIRootView::OnPaint()
{
	CUIPaintDC dc(GetHwnd());

	if (m_bLayered)
		OnPaintLayered(dc);
	else
		DoPaint(CUIMemDC(dc, dc.PaintRect()), dc.PaintRect());

	m_rectClip = CRect();
}

void CUIRootView::OnPaintLayered(HDC hDC)
{
	if (m_imageWnd.IsNull())
	{
		m_imageWnd.Create(m_rect.Width(), m_rect.Height(), 32, CImage::createAlphaChannel);
		m_rectClip = m_rect;
	}

	HDC hImgDC = m_imageWnd.GetDC();

	if (!m_rectClip.IsRectEmpty())
	{
		DoPaint(CUIMemDC(hImgDC, m_rectClip, true), m_rectClip);
		m_rectClip = CRect();
	}

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, m_nWndAlpha, AC_SRC_ALPHA };
	UpdateLayeredWindow(GetHwnd(), hDC, NULL, &m_rect.Size(), hImgDC, &CPoint(), 0, &bf, ULW_ALPHA);
	m_imageWnd.ReleaseDC();
}

void CUIRootView::DoPaint(HDC hDC, LPCRECT lpClipRect)
{
	CUIDC dc(hDC, lpClipRect, m_bLayered);
	SetBkMode(dc, TRANSPARENT);
	m_pWindow->OnDrawBg(dc, m_rect);
	__super::OnPaint(dc);
}

void CUIRootView::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UIHitTest hitTest;
	hitTest.point = lParam;
	OnHitTest(hitTest);

	if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN)
	{
		LPCWSTR lpToolTip = L"";

		for (auto hit : hitTest)
		{
			if (hit.pItem->IsControl())
			{
				if (uMsg == WM_MOUSEMOVE)
				{
					lpToolTip = ((CUIControl *)hit.pItem)->m_strToolTip.c_str();
				}
				else if (hit.bEnabled && dynamic_cast<CUIEdit *>(hit.pItem) == NULL)
				{
					SetFocus(NULL);		// 使输入框失去焦点
				}
				break;
			}
		}

		if (uMsg == WM_MOUSEMOVE)
			CUIToolTip::ShowTip(GetHwnd(), lpToolTip);
		else
			CUIToolTip::ShowTip(GetHwnd(), NULL);

		for (auto hit : hitTest)
		{
			if (hit.bEnabled)
				DoMouseEnter(hit.pItem);
		}
	}

	if (uMsg == WM_MOUSEMOVE)
		CheckMouseLeave(hitTest);

	for (auto hit : hitTest)
	{
		if (hit.pItem->IsControl())
		{
			if (hit.bEnabled)
				hit.pItem->OnMessage(uMsg, wParam, lParam);
			break;
		}
	}
}

void CUIRootView::RaiseMouseMove()
{
	if (m_rect.IsRectEmpty())
		return;

	UIHitTest hitTest;
	GetCursorPos(&hitTest.point);

	if (WindowFromPoint(hitTest.point) == GetHwnd())
	{
		ScreenToClient(GetHwnd(), &hitTest.point);
		OnHitTest(hitTest);
	}

	if (m_bMouseEnter)
	{
		for (auto hit : hitTest)
		{
			if (hit.bEnabled)
				DoMouseEnter(hit.pItem);
		}
	}

	CheckMouseLeave(hitTest);
}

void CUIRootView::DoMouseEnter(CUIBase *pItem)
{
	auto itNull = m_vecEnterItems.end();

	for (auto it = m_vecEnterItems.begin(); it != m_vecEnterItems.end(); ++it)
	{
		if (pItem == *it)
			return;

		if (itNull == m_vecEnterItems.end() && *it == NULL)
			itNull = it;
	}

	if (itNull == m_vecEnterItems.end())
	{
		auto pData = m_vecEnterItems.data();
		m_vecEnterItems.push_back(pItem);

		if (pData != m_vecEnterItems.data())
		{
			// vector 内存地址变了，需要更新
			for (auto it = m_vecEnterItems.begin(); it != m_vecEnterItems.end(); ++it)
			{
				(*it)->m_ppEnter = &*it;
			}
		}
		else
			pItem->m_ppEnter = &m_vecEnterItems.back();
	}
	else
	{
		*itNull = pItem;
		pItem->m_ppEnter = &*itNull;
	}

	pItem->OnMouseEnter();
}

void CUIRootView::CheckMouseLeave(const UIHitTest &hitTest)
{
	for (auto pItem : m_vecEnterItems)
	{
		if (pItem == NULL || pItem == m_pCapture)
			continue;

		bool bEnter = false;

		for (auto hit : hitTest)
		{
			if (pItem == hit.pItem)
			{
				if (hit.bEnabled)
					bEnter = true;
				break;
			}
		}

		if (!bEnter)
			pItem->DoMouseLeave(false);
	}
}

void CUIRootView::DoInvalidateRect(LPCRECT lpRect)
{
	CRect rect(m_rect);
	if (lpRect)
		rect &= *lpRect;

	if (rect.IsRectEmpty())
		return;

	::InvalidateRect(GetHwnd(), rect, TRUE);
	m_rectClip |= rect;
}

void CUIRootView::DoInvalidateLayout()
{
	if (m_bPostLayout)
		return;

	// 延迟布局所有需要重新布局的控件
	m_bPostLayout = true;
	PostMessage(GetHwnd(), m_nLayoutMsgId, 0, 0);
}

void CUIRootView::SetCapture(CUIControl *pCtrl)
{
	if (!m_pCapture == !pCtrl)
		return;

	if (m_pCapture = pCtrl)
		::SetCapture(GetHwnd());
	else
		::ReleaseCapture();
}

void CUIRootView::SetFocus(CUIControl *pCtrl)
{
	if (m_pCurFocus == pCtrl)
		return;

	CUIControl *pOldFocus = m_pCurFocus;
	m_pCurFocus = pCtrl;

	if (pOldFocus)
		pOldFocus->OnKillFocus();
	else
		::SetFocus(GetHwnd());

	if (pCtrl)
		pCtrl->OnSetFocus();
}

void CUIRootView::EnableImm(bool bEnabled)
{
	if (!m_hImc == !bEnabled)
		m_hImc = ImmAssociateContext(GetHwnd(), m_hImc);
}

void CUIRootView::AddTabsEdit(CUIEdit *pEdit)
{
	for (auto it = m_vecEdits.begin(); it != m_vecEdits.end(); ++it)
	{
		if (*it == pEdit)
			return;
	}

	m_vecEdits.push_back(pEdit);
}

void CUIRootView::DelTabsEdit(CUIEdit *pEdit)
{
	if (m_pCurFocus == pEdit)
	{
		m_pCurFocus->OnKillFocus();
		m_pCurFocus = NULL;
	}

	for (auto it = m_vecEdits.begin(); it != m_vecEdits.end(); ++it)
	{
		if (*it == pEdit)
		{
			m_vecEdits.erase(it);
			break;
		}
	}
}

void CUIRootView::NextTabsEdit()
{
	int nSize = m_vecEdits.size();
	if (nSize < 2)
		return;

	int iCurId = -1;

	for (int i = 0; i != nSize; i++)
	{
		if (m_vecEdits[i] == m_pCurFocus)
		{
			iCurId = i;
			break;
		}
	}

	if (iCurId == -1)
		return;

	for (int i = iCurId; ; )
	{
		if (GetKeyState(VK_SHIFT) < 0)
			i = (i + nSize - 1) % nSize;
		else
			i = (i + 1) % nSize;

		if (i == iCurId)
			break;

		if (m_vecEdits[i]->IsRealEnabled() && !m_vecEdits[i]->m_rect.IsRectEmpty())
		{
			m_vecEdits[i]->SetSel(0, -1);
			SetFocus(m_vecEdits[i]);
			break;
		}
	}
}

CUIView *CUIRootView::OnCustomUI(LPCWSTR lpName, CUIView *pParent)
{
	return m_pWindow->OnCustomUI(lpName, pParent);
}

void CUIRootView::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	m_pWindow->OnLoadedUI(attrs);
}
