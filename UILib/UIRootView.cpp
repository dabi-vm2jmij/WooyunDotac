#include "stdafx.h"
#include "UIRootView.h"
#include "UILibApp.h"

CUIRootView::CUIRootView(IUIWindow *pOwner) : CUIView(NULL), m_pOwner(pOwner), m_nWndAlpha(255), m_bLayered(false), m_hImc(NULL), m_hCursor(NULL)
	, m_hToolTip(NULL), m_pFocus(NULL), m_pCapture(NULL)
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
	case WM_CREATE:
		if (GetWindowLong(GetHwnd(), GWL_EXSTYLE) & WS_EX_LAYERED)
			m_bLayered = true;
		break;

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			OnSize(lParam);

			if (m_bLayered)
				OnPaintLayered(CUIClientDC(GetHwnd()));
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

		if (m_strTipText.size())
		{
			m_strTipText.clear();
			ShowToolTip(NULL);
		}
		return TRUE;

	case WM_MOUSEMOVE:
		if (!m_bMouseEnter)
		{
			TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, GetHwnd() };
			m_bMouseEnter = TrackMouseEvent(&tme) != 0;
		}

		if (m_hToolTip)
		{
			MSG msg;
			msg.hwnd = GetHwnd();
			msg.message = uMsg;
			msg.wParam = wParam;
			msg.lParam = lParam;
			SendMessage(m_hToolTip, TTM_RELAYEVENT, 0, (LPARAM)&msg);
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
		if (m_pFocus)
		{
			m_pFocus->OnMessage(uMsg, wParam, lParam);
			return TRUE;
		}
		break;

	case WM_SETFOCUS:
		if (m_pFocus)
			m_pFocus->OnSetFocus();
		break;

	case WM_KILLFOCUS:
		if (m_pFocus)
		{
			m_pFocus->OnKillFocus();

			if (wParam && ::IsChild(GetHwnd(), (HWND)wParam))
				m_pFocus = NULL;
		}
		break;

	case WM_PARENTNOTIFY:
		if (m_pFocus)
		{
			switch (LOWORD(wParam))
			{
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				m_pFocus->OnKillFocus();
				m_pFocus = NULL;
			}
		}
		break;

	case WM_INPUTLANGCHANGE:
		if (m_pFocus && wParam)
		{
			if (auto pEdit = dynamic_cast<CUIEdit *>(m_pFocus))
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
	return m_pOwner->GetHwnd();
}

void CUIRootView::SetWndAlpha(BYTE nWndAlpha)
{
	if (m_nWndAlpha == nWndAlpha)
		return;

	m_nWndAlpha = nWndAlpha;

	if (m_bLayered)
		OnPaintLayered(CUIClientDC(GetHwnd()));
}

void CUIRootView::UpdateLayout()
{
	// 重新布局所有延迟布局的控件
	CRect rect;
	OnNeedLayout(rect);

	if (!rect.IsRectEmpty())
		InvalidateRect(rect);

	if (m_pCapture == NULL)
		RaiseMouseMove();
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
			if (hit.bEnable)
				m_hCursor = ((CUIControl *)hit.pItem)->GetCursor();
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
		OnRectChange(NULL, rect);

		if (!rect.IsRectEmpty())
			InvalidateRect(rect);

		m_imageWnd.Destroy();
	}
}

void CUIRootView::OnPaint()
{
	CUIPaintDC dc(GetHwnd());

	if (m_bLayered)
		OnPaintLayered(dc);
	else
		DoPaint(CUIMemDC(dc, dc.PaintRect()), dc.PaintRect());

	m_clipRect.SetRectEmpty();
}

void CUIRootView::OnPaintLayered(HDC hDC)
{
	if (m_imageWnd.IsNull())
	{
		m_imageWnd.Create(m_rect.Width(), m_rect.Height(), 32, CImage::createAlphaChannel);
		m_clipRect = m_rect;
	}

	HDC hImgDC = m_imageWnd.GetDC();

	if (!m_clipRect.IsRectEmpty())
	{
		DoPaint(CUIMemDC(hImgDC, m_clipRect, true), m_clipRect);
		m_clipRect.SetRectEmpty();
	}

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, m_nWndAlpha, AC_SRC_ALPHA };
	UpdateLayeredWindow(GetHwnd(), hDC, NULL, &m_rect.Size(), hImgDC, &CPoint(), 0, &bf, ULW_ALPHA);
	m_imageWnd.ReleaseDC();
}

void CUIRootView::DoPaint(HDC hDC, LPCRECT lpClipRect) const
{
	CUIDC dc(hDC, lpClipRect, m_bLayered);
	SetBkMode(dc, TRANSPARENT);

	if (m_colorBg != -1)
		dc.FillSolidRect(m_rect, m_colorBg);
	else
		m_pOwner->OnDrawBg(dc, m_rect);

	__super::OnPaint(dc);

	// 最后画在最上
	if (m_pCapture)
		m_pCapture->MyPaint(dc);
}

void CUIRootView::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UIHitTest hitTest;
	hitTest.point = lParam;
	OnHitTest(hitTest);

	if (uMsg == WM_MOUSEMOVE)
	{
		LPCWSTR lpTipText = L"";

		for (auto hit : hitTest)
		{
			if (hit.pItem->IsControl())
			{
				lpTipText = ((CUIControl *)hit.pItem)->GetToolTip();
				break;
			}
		}

		if (m_strTipText != lpTipText && (*lpTipText == 0 || (wParam & (MK_LBUTTON | MK_RBUTTON)) == 0))
		{
			m_strTipText = lpTipText;
			ShowToolTip(m_strTipText.c_str());
		}
	}
	else if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN)
	{
		if (m_strTipText.size())
			ShowToolTip(NULL);
	}

	if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN)
	{
		for (auto hit : hitTest)
		{
			if (hit.bEnable)
				DoMouseEnter(hit.pItem);
		}

		CheckMouseLeave(hitTest);
	}

	// 最上层 Control 处理消息
	for (auto hit : hitTest)
	{
		if (hit.pItem->IsControl())
		{
			if (hit.bEnable)
			{
				if (m_pFocus && m_pFocus != hit.pItem && (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN))
					SetFocus(NULL);		// 使输入框失去焦点

				hit.pItem->OnMessage(uMsg, wParam, lParam);
			}
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
		ScreenToClient(&hitTest.point);
		OnHitTest(hitTest);
	}

	if (m_bMouseEnter)
	{
		for (auto hit : hitTest)
		{
			if (hit.bEnable)
				DoMouseEnter(hit.pItem);
		}
	}

	CheckMouseLeave(hitTest);
}

void CUIRootView::DoMouseEnter(CUIBase *pItem)
{
	for (auto it = m_vecEnterItems.begin(); it != m_vecEnterItems.end(); ++it)
	{
		if (*it == pItem)
			return;
	}

	m_vecEnterItems.push_back(pItem);
	pItem->m_bMouseEnter = true;
	pItem->OnMouseEnter();
}

void CUIRootView::CheckMouseLeave(const UIHitTest &hitTest)
{
	vector<CUIBase *> vecLeaveItems;

	for (auto it = m_vecEnterItems.begin(); it != m_vecEnterItems.end(); )
	{
		if (*it == m_pCapture)
		{
			++it;
			continue;
		}

		bool bEnter = false;

		for (auto hit : hitTest)
		{
			if (hit.pItem == *it)
			{
				if (hit.bEnable)
					bEnter = true;
				break;
			}
		}

		if (!bEnter)
		{
			vecLeaveItems.push_back(*it);
			it = m_vecEnterItems.erase(it);
		}
		else
			++it;
	}

	for (auto pItem : vecLeaveItems)
	{
		pItem->m_bMouseEnter = false;
		pItem->OnMouseLeave();
	}
}

void CUIRootView::InvalidateRect(LPCRECT lpRect)
{
	CRect rect(m_rect);
	if (lpRect)
		rect &= *lpRect;

	if (rect.IsRectEmpty())
		return;

	::InvalidateRect(GetHwnd(), rect, TRUE);
	m_clipRect |= rect;
}

BOOL CUIRootView::ClientToScreen(LPPOINT lpPoint)
{
	return ::ClientToScreen(GetHwnd(), lpPoint);
}

BOOL CUIRootView::ScreenToClient(LPPOINT lpPoint)
{
	return ::ScreenToClient(GetHwnd(), lpPoint);
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
	if (m_pFocus == pCtrl)
		return;

	auto pOldFocus = m_pFocus;
	m_pFocus = pCtrl;

	if (m_rect.IsRectEmpty())
		return;

	if (pOldFocus)
		pOldFocus->OnKillFocus();
	else
		::SetFocus(GetHwnd());

	if (pCtrl)
		pCtrl->OnSetFocus();
}

void CUIRootView::EnableImm(bool bEnable)
{
	if (!m_hImc == !bEnable)
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
		if (m_vecEdits[i] == m_pFocus)
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

		if (m_vecEdits[i]->IsRealEnabled() && m_vecEdits[i]->IsRealVisible())
		{
			m_vecEdits[i]->SetSel(0, -1);
			SetFocus(m_vecEdits[i]);
			break;
		}
	}
}

void CUIRootView::ShowToolTip(LPCWSTR lpTipText)
{
	if (lpTipText && *lpTipText)
	{
		HWND hWnd = GetHwnd();

		TOOLINFOW ti = { sizeof(ti) };
		ti.uFlags    = TTF_IDISHWND;
		ti.hwnd      = ::GetParent(hWnd);
		ti.uId       = (UINT_PTR)hWnd;
		ti.hinst     = _AtlBaseModule.GetResourceInstance();
		ti.lpszText  = (LPWSTR)lpTipText;

		if (m_hToolTip)
		{
			SendMessage(m_hToolTip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
			SendMessage(m_hToolTip, TTM_ACTIVATE, TRUE, 0);
		}
		else
		{
			m_hToolTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, _AtlBaseModule.GetModuleInstance(), NULL);
			SendMessage(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, 960);
			SendMessage(m_hToolTip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
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

void CUIRootView::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	m_pOwner->OnLoadedUI(attrs);
}
