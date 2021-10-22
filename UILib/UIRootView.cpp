#include "stdafx.h"
#include "UIRootView.h"
#include "UILibApp.h"

CUIRootView::CUIRootView(IUIWindow *pOwner) : CUIView(NULL), m_pOwner(pOwner), m_hImc(NULL), m_hCursor(NULL), m_hToolTip(NULL), m_pFocus(NULL), m_pCapture(NULL)
{
	ATLASSERT(pOwner);
	m_vecEnterItems.reserve(8);
}

CUIRootView::~CUIRootView()
{
	Destroy();
	g_theApp.RemoveLayout(this);
}

void CUIRootView::Destroy()
{
	m_pFocus = NULL;
	m_pCapture = NULL;
	m_vecEdits.clear();
	m_vecEnterItems.clear();

	while (m_vecChilds.size())
	{
		auto pItem = m_vecChilds.back();
		m_vecChilds.pop_back();
		delete pItem;
	}
}

BOOL CUIRootView::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
	lResult = 0;

	switch (uMsg)
	{
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
		EnableImm(m_pFocus && m_pFocus->IsImmEnabled());

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

	case WM_CAPTURECHANGED:
		if (m_pCapture)
		{
			m_pCapture->OnLostCapture();
			m_pCapture = NULL;
		}
		break;
	}

	return FALSE;
}

HWND CUIRootView::GetHwnd() const
{
	HWND hWnd = m_pOwner->GetHwnd();
	ATLASSERT(hWnd);
	return hWnd;
}

void CUIRootView::UpdateLayout()
{
	// 重新布局所有延迟布局的控件
	CRect rect;
	OnNeedLayout(rect);

	if (!rect.IsRectEmpty())
	{
		InvalidateRect(rect);

		if (m_pCapture == NULL)
			DoMouseMove();
	}
}

void CUIRootView::OnPaint(CUIDC &dc) const
{
	// 画背景
	if (m_bgColor != -1)
		dc.FillSolidRect(m_rect, m_bgColor);

	if (m_bgImagex)
		m_bgImagex.Scale9Draw(dc, m_rect, false);
}

bool CUIRootView::OnNcHitTest(CPoint point)
{
	UIHitTest hitTest;
	hitTest.point = point;
	ScreenToClient(&hitTest.point);
	OnHitTest(hitTest);

	// 用于 WM_SETCURSOR
	m_hCursor = NULL;

	for (auto hit : hitTest)
	{
		if (auto pCtrl = dynamic_cast<CUIControl *>(hit.pItem))
		{
			if (hit.bEnable)
				m_hCursor = pCtrl->GetCursor();
			break;
		}
	}

	return hitTest.nCount != 0;
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
			if (auto pCtrl = dynamic_cast<CUIControl *>(hit.pItem))
			{
				lpTipText = pCtrl->GetToolTip();
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
	bool bHandled = false;

	for (auto hit : hitTest)
	{
		if (auto pCtrl = dynamic_cast<CUIControl *>(hit.pItem))
		{
			if (hit.bEnable)
			{
				bHandled = hit.pItem->OnMessage(uMsg, wParam, lParam);

				// 使输入框失去焦点
				if (m_pFocus && m_pFocus != hit.pItem && (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN))
					SetFocus(NULL);
			}
			break;
		}
	}

	if (!bHandled && uMsg == WM_RBUTTONUP)
		OnMessage(WM_CONTEXTMENU, (WPARAM)GetHwnd(), lParam);
}

void CUIRootView::DoMouseMove()
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

void CUIRootView::DoMouseEnter(CUIView *pItem)
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
	vector<CUIView *> vecLeaveItems;

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
	if (rect.IsRectEmpty() || lpRect && !rect.IntersectRect(rect, lpRect))
		return;

	m_pOwner->InvalidateRect(rect);
}

void CUIRootView::ClientToScreen(LPPOINT lpPoint)
{
	::ClientToScreen(GetHwnd(), lpPoint);
}

void CUIRootView::ScreenToClient(LPPOINT lpPoint)
{
	::ScreenToClient(GetHwnd(), lpPoint);
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

	EnableImm(m_pFocus && m_pFocus->IsImmEnabled());	// 输入法

	if (m_pFocus)
		m_pFocus->OnSetFocus();
}

void CUIRootView::SetCapture(CUIControl *pCtrl)
{
	if (m_pCapture == pCtrl)
		return;

	auto pOldCapture = m_pCapture;
	m_pCapture = pCtrl;

	if (pOldCapture == NULL)
		::SetCapture(GetHwnd());
	else if (pCtrl == NULL)
		::ReleaseCapture();
	else
		pOldCapture->OnLostCapture();
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

void CUIRootView::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	m_pOwner->OnLoadUI(attrs);
}
