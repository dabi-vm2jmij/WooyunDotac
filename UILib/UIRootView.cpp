#include "stdafx.h"
#include "UIRootView.h"
#include "UILibApp.h"

CUIRootView::CUIRootView(IUIWindow *pOwner) : CUIView(NULL), m_pOwner(pOwner), m_nWndAlpha(255), m_bLayered(false), m_bMouseEnter(false), m_hImc(NULL), m_hCursor(NULL)
	, m_hToolTip(NULL), m_pCapture(NULL), m_pCurFocus(NULL)
{
	m_vecEnterItems.reserve(8);
}

CUIRootView::~CUIRootView()
{
	for (auto pItem : m_vecChilds)
		delete pItem;

	m_vecChilds.clear();
	g_theApp.RemoveLayout(this);
}

BOOL CUIRootView::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
	lResult = 0;

	switch (uMsg)
	{
	case WM_NEEDLAYOUT:
		{
			// 重新布局所有延迟布局的控件
			CRect rect;
			OnNeedLayout(rect);

			if (!rect.IsRectEmpty())
				InvalidateRect(rect);

			if (m_pCapture == NULL)
				RaiseMouseMove();
		}
		return TRUE;

	case WM_CREATE:
		if (GetWindowLong(GetOwnerWnd(), GWL_EXSTYLE) & WS_EX_LAYERED)
			m_bLayered = true;
		break;

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			OnSize(lParam);

			if (m_bLayered)
				OnPaintLayered(CUIClientDC(GetOwnerWnd()));
		}
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
		ShowToolTip(L"");
		return TRUE;

	case WM_MOUSEMOVE:
		if (!m_bMouseEnter)
		{
			TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, GetOwnerWnd() };
			m_bMouseEnter = TrackMouseEvent(&tme) != 0;
		}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		if (m_pCapture)
		{
			m_pCapture->OnMessage(uMsg, wParam, lParam);
			return TRUE;
		}

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		if (m_hToolTip)
		{
			MSG msg;
			msg.hwnd = GetOwnerWnd();
			msg.message = uMsg;
			msg.wParam = wParam;
			msg.lParam = lParam;
			SendMessage(m_hToolTip, TTM_RELAYEVENT, 0, (LPARAM)&msg);
		}

		OnMouseMessage(uMsg, wParam, lParam);
		return TRUE;

	case WM_MOUSEWHEEL:
	case WM_CONTEXTMENU:
		{
			CPoint point(lParam);
			ScreenToClient(&point);
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

			if (wParam && ::IsChild(GetOwnerWnd(), (HWND)wParam))
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

HWND CUIRootView::GetOwnerWnd() const
{
	return m_pOwner->GetHwnd();
}

void CUIRootView::SetWndAlpha(BYTE nWndAlpha)
{
	if (m_nWndAlpha == nWndAlpha)
		return;

	m_nWndAlpha = nWndAlpha;

	if (m_bLayered)
		OnPaintLayered(CUIClientDC(GetOwnerWnd()));
}

bool CUIRootView::OnNcHitTest(CPoint point)
{
	UIHitTest hitTest;
	hitTest.point = point;
	ScreenToClient(&hitTest.point);
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
}

void CUIRootView::OnPaint()
{
	CUIPaintDC dc(GetOwnerWnd());

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
	UpdateLayeredWindow(GetOwnerWnd(), hDC, NULL, &m_rect.Size(), hImgDC, &CPoint(), 0, &bf, ULW_ALPHA);
	m_imageWnd.ReleaseDC();
}

void CUIRootView::DoPaint(HDC hDC, LPCRECT lpClipRect) const
{
	CUIDC dc(hDC, lpClipRect, m_bLayered);
	SetBkMode(dc, TRANSPARENT);

	// 填充背景色
	if (m_colorBg != -1)
		dc.FillSolidRect(m_rect, m_colorBg);

	m_pOwner->OnDrawBg(dc, m_rect);
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
			ShowToolTip(lpToolTip);
		else
			ShowToolTip(NULL);

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

	if (WindowFromPoint(hitTest.point) == GetOwnerWnd())
	{
		ScreenToClient(&hitTest.point);
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

void CUIRootView::InvalidateRect(LPCRECT lpRect)
{
	CRect rect(m_rect);
	if (lpRect)
		rect &= *lpRect;

	if (rect.IsRectEmpty())
		return;

	::InvalidateRect(GetOwnerWnd(), rect, TRUE);
	m_rectClip |= rect;
}

BOOL CUIRootView::ClientToScreen(LPPOINT lpPoint)
{
	return ::ClientToScreen(GetOwnerWnd(), lpPoint);
}

BOOL CUIRootView::ScreenToClient(LPPOINT lpPoint)
{
	return ::ScreenToClient(GetOwnerWnd(), lpPoint);
}

void CUIRootView::SetCapture(CUIControl *pCtrl)
{
	if (!m_pCapture == !pCtrl)
		return;

	if (m_pCapture = pCtrl)
		::SetCapture(GetOwnerWnd());
	else
		::ReleaseCapture();
}

void CUIRootView::SetFocus(CUIControl *pCtrl)
{
	if (m_pCurFocus == pCtrl)
		return;

	CUIControl *pOldFocus = m_pCurFocus;
	m_pCurFocus = pCtrl;

	if (m_rect.IsRectEmpty())
		return;

	if (pOldFocus)
		pOldFocus->OnKillFocus();
	else
		::SetFocus(GetOwnerWnd());

	if (pCtrl)
		pCtrl->OnSetFocus();
}

void CUIRootView::EnableImm(bool bEnabled)
{
	if (!m_hImc == !bEnabled)
		m_hImc = ImmAssociateContext(GetOwnerWnd(), m_hImc);
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

void CUIRootView::ShowToolTip(LPCWSTR lpTipText)
{
	if (m_strTipText == (lpTipText ? lpTipText : L""))
		return;

	if (lpTipText)
		m_strTipText = lpTipText;

	if (lpTipText && *lpTipText)
	{
		HWND hWnd = GetOwnerWnd();

		TOOLINFO ti = { sizeof(ti) };
		ti.uFlags   = TTF_IDISHWND;
		ti.hwnd     = ::GetParent(hWnd);
		ti.uId      = (UINT_PTR)hWnd;
		ti.hinst    = _AtlBaseModule.GetResourceInstance();
		ti.lpszText = (LPTSTR)m_strTipText.c_str();

		if (m_hToolTip)
		{
			SendMessage(m_hToolTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
			SendMessage(m_hToolTip, TTM_ACTIVATE, TRUE, 0);
		}
		else
		{
			m_hToolTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, _AtlBaseModule.GetModuleInstance(), NULL);
			SendMessage(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, 960);
			SendMessage(m_hToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
		}
	}
	else if (m_hToolTip)
	{
		SendMessage(m_hToolTip, TTM_ACTIVATE, FALSE, 0);
	}
}

CUIView *CUIRootView::OnCustomUI(LPCWSTR lpName, CUIView *pParent)
{
	return m_pOwner->OnCustomUI(lpName, pParent);
}

void CUIRootView::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	m_pOwner->OnLoadedUI(attrs);
}
