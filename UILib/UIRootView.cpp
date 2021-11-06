#include "stdafx.h"
#include "UIRootView.h"
#include "UILibApp.h"

CUIRootView::CUIRootView(IUIWindow *pOwner) : CUIView(NULL), m_pOwner(pOwner), m_hCursor(NULL), m_hToolTip(NULL), m_pFocus(NULL), m_pCapture(NULL), m_bComposing(false), m_caretTimer([this]{ OnCaretTimer(); })
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
	BOOL bRet = FALSE;

	switch (uMsg)
	{
	case WM_CREATE:
		ImmAssociateContextEx(hWnd, NULL, 0);
		break;

	case WM_NCHITTEST:
		if (OnNcHitTest(lParam))
		{
			lResult = HTCLIENT;
			bRet = TRUE;
		}
		break;

	case WM_SETCURSOR:
		if (m_hCursor)
		{
			SetCursor(m_hCursor);
			lResult = TRUE;
			bRet = TRUE;
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
		bRet = TRUE;
		break;

	case WM_MOUSEMOVE:
		if (!m_bMouseEnter)
		{
			TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd };
			m_bMouseEnter = TrackMouseEvent(&tme) != 0;
		}

		if (m_hToolTip)
		{
			MSG msg;
			msg.hwnd = hWnd;
			msg.message = uMsg;
			msg.wParam = wParam;
			msg.lParam = lParam;
			SendMessage(m_hToolTip, TTM_RELAYEVENT, 0, (LPARAM)&msg);
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
		bRet = TRUE;
		break;

	case WM_MOUSEWHEEL:
	case WM_CONTEXTMENU:
		if (m_pCapture == NULL)
		{
			CPoint point(lParam);
			ScreenToClient(&point);
			OnMessage(uMsg, wParam, MAKELPARAM(point.x, point.y));
		}
		bRet = TRUE;
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		if (m_pFocus)
			m_pFocus->OnMessage(uMsg, wParam, lParam);
		break;

	case WM_SYSCOMMAND:
		bRet = GET_SC_WPARAM(wParam) == SC_KEYMENU;
		break;

	case WM_IME_STARTCOMPOSITION:
		m_bComposing = true;
		InvalidateRect(m_caretRect);
		SetCaretPos(m_caretRect);
		break;

	case WM_IME_ENDCOMPOSITION:
		m_bComposing = false;
		InvalidateRect(m_caretRect);
		break;

	case WM_IME_COMPOSITION:
		if (lParam & GCS_RESULTSTR)
		{
			auto pEdit = dynamic_cast<CUIEdit *>(m_pFocus);
			HIMC hImc;
			if (pEdit && (hImc = ImmGetContext(hWnd)))
			{
				wchar_t szText[256];
				szText[ImmGetCompositionStringW(hImc, GCS_RESULTSTR, szText, sizeof(szText) - 2) / 2] = 0;
				ImmReleaseContext(hWnd, hImc);
				pEdit->Insert(szText);
			}
			bRet = TRUE;
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

			if (wParam && ::IsChild(hWnd, (HWND)wParam))
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

	return bRet;
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
		InvalidateRect(rect);

	if (m_bMouseEnter && m_pCapture == NULL)
	{
		UIHitTest hitTest;
		GetCursorPos(&hitTest.point);
		ScreenToClient(&hitTest.point);
		OnHitTest(hitTest);

		for (auto hit : hitTest)
		{
			if (hit.bEnable)
				DoMouseEnter(hit.pItem);
		}

		CheckMouseLeave(hitTest);
	}
}

void CUIRootView::PaintChilds(CUIDC &dc) const
{
	__super::PaintChilds(dc);

	// 自绘光标
	if (!m_bComposing && m_caretRect.Width() > 0)
	{
		int nOldRop = SetROP2(dc, R2_NOT);
		MoveToEx(dc, m_caretRect.left, m_caretRect.top, NULL);
		LineTo(dc, m_caretRect.left, m_caretRect.bottom);
		SetROP2(dc, nOldRop);
		dc.FillAlpha(m_caretRect, 255);
	}
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
		OnMessage(WM_CONTEXTMENU, 0, lParam);
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

void CUIRootView::PostRemoveItems(const vector<CUIView *> &vecItems)
{
	CRect rect;
	for (auto pItem : vecItems)
		pItem->SetRect(NULL, rect);

	InvalidateRect(rect);

	// 安全处理，防止野指针
	if (m_pFocus && !m_pFocus->IsRealVisible())
		SetFocus(NULL);

	if (m_pCapture && !m_pCapture->IsRealVisible())
		SetCapture(NULL);

	vector<CUIView *> vecLeaveItems;

	for (auto it = m_vecEnterItems.begin(); it != m_vecEnterItems.end(); )
	{
		if (!(*it)->IsRealVisible())
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

UINT CUIRootView::PopupUIMenu(CUIMenu *pUIMenu, int x1, int y1, int x2, int y2)
{
	// 弹出菜单时关闭输入法、暂停光标
	HWND hWnd = GetHwnd();
	HIMC hImc = ImmGetContext(hWnd);
	if (hImc)
		ImmSetOpenStatus(hImc, FALSE);

	bool bCaret = m_caretTimer.Running();
	if (bCaret)
		m_caretTimer.Stop();

	UINT nCmdId = pUIMenu->Popup(hWnd, x1, y1, x2, y2);

	if (bCaret)
		m_caretTimer.Start(GetCaretBlinkTime());

	if (hImc)
	{
		ImmSetOpenStatus(hImc, TRUE);
		ImmReleaseContext(hWnd, hImc);
	}

	return nCmdId;
}

void CUIRootView::SetCaretPos(LPCRECT lpRect)
{
	if (m_caretRect != *lpRect)
	{
		InvalidateRect(m_caretRect);
		m_caretRect = lpRect;
		InvalidateRect(m_caretRect);

		if (m_caretRect.Width() > 0)
			m_caretTimer.Start(GetCaretBlinkTime());
		else
			m_caretTimer.Stop();
	}

	// 设置输入法窗口位置
	HWND hWnd = GetHwnd();
	HIMC hImc;
	if (m_bComposing && (hImc = ImmGetContext(hWnd)))
	{
		COMPOSITIONFORM compForm = { CFS_POINT, m_caretRect.TopLeft() };
		ImmSetCompositionWindow(hImc, &compForm);
		ImmReleaseContext(hWnd, hImc);
	}
}

void CUIRootView::SetFocus(CUIControl *pFocus)
{
	if (m_pFocus == pFocus || m_rect.IsRectEmpty())
		return;

	// 开关输入法
	HWND hWnd = GetHwnd();
	auto pEdit = dynamic_cast<CUIEdit *>(pFocus);
	ImmAssociateContextEx(hWnd, NULL, pEdit && pEdit->IsImeEnabled() ? IACE_DEFAULT : 0);

	HIMC hImc;
	if (pEdit && (hImc = ImmGetContext(hWnd)))
	{
		LOGFONTW lf;
		GetObjectW(pEdit->GetFont(), sizeof(lf), &lf);
		ImmSetCompositionFontW(hImc, &lf);
		ImmReleaseContext(hWnd, hImc);
	}

	auto pOldFocus = m_pFocus;
	m_pFocus = pFocus;

	if (pOldFocus)
		pOldFocus->OnKillFocus();

	if (pFocus && IsWindowVisible(hWnd))
	{
		if (::GetFocus() != hWnd)
			::SetFocus(hWnd);
		else
			pFocus->OnSetFocus();
	}
}

void CUIRootView::SetCapture(CUIControl *pCapture)
{
	if (m_pCapture == pCapture)
		return;

	auto pOldCapture = m_pCapture;
	m_pCapture = pCapture;

	if (pOldCapture == NULL)
		::SetCapture(GetHwnd());
	else if (pCapture == NULL)
		::ReleaseCapture();
	else
		pOldCapture->OnLostCapture();
}

void CUIRootView::OnCaretTimer()
{
	InvalidateRect(m_caretRect);
	m_caretRect.right = m_caretRect.left * 2 - m_caretRect.right;
	InvalidateRect(m_caretRect);
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
