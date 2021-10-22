#include "stdafx.h"
#include "UIControl.h"

CUIControl::CUIControl(CUIView *pParent) : CUIView(pParent), m_bClickable(true), m_bDraggable(false), m_hCursor(NULL), m_bLButtonDown(false), m_bRButtonDown(false)
	, m_ptClick(MAXINT16, MAXINT16)
{
}

CUIControl::~CUIControl()
{
}

bool CUIControl::OnHitTest(UIHitTest &hitTest)
{
	bool bRet = __super::OnHitTest(hitTest);

	if (m_bClickable)
	{
		hitTest.Add(this);
		bRet = true;
	}

	return bRet;
}

bool CUIControl::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (__super::OnMessage(uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		if (m_ptClick.cx != MAXINT16)
		{
			FRIEND(GetParent())->OnChildMoving(this, CPoint(lParam) - m_ptClick);
			return true;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if (!m_bLButtonDown)
		{
			if (m_bDraggable)
			{
				m_ptClick = CPoint(lParam) - m_rect.TopLeft();
				GetRootView()->SetCapture(this);
			}

			m_bLButtonDown = true;
			OnLButtonDown(lParam);
		}
		return true;

	case WM_LBUTTONUP:
		if (m_bLButtonDown)
		{
			if (m_ptClick.cx != MAXINT16)
			{
				GetRootView()->SetCapture(NULL);
				FRIEND(GetParent())->OnChildMoved(this, CPoint(lParam) - m_ptClick);
				m_ptClick.SetSize(MAXINT16, MAXINT16);
			}

			m_bLButtonDown = false;
			OnLButtonUp(lParam);
		}
		return true;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		if (!m_bRButtonDown)
		{
			m_bRButtonDown = true;
			OnRButtonDown(lParam);
		}
		return true;

	case WM_RBUTTONUP:
		if (m_bRButtonDown)
		{
			m_bRButtonDown = false;
			OnRButtonUp(lParam);
		}
		return true;
	}

	return false;
}

void CUIControl::OnEnable(bool bEnable)
{
	__super::OnEnable(bEnable);

	if (!bEnable)
	{
		auto pRootView = GetRootView();
		if (pRootView->GetFocus() == this)
			pRootView->SetFocus(NULL);
	}
}

void CUIControl::OnVisible(bool bVisible)
{
	__super::OnVisible(bVisible);

	if (!bVisible)
	{
		auto pRootView = GetRootView();
		if (pRootView->GetFocus() == this)
			pRootView->SetFocus(NULL);
	}
}

void CUIControl::OnMouseLeave()
{
	m_bLButtonDown = m_bRButtonDown = false;

	__super::OnMouseLeave();
}

void CUIControl::OnRButtonUp(CPoint point)
{
	auto pRootView = GetRootView();
	FRIEND(pRootView)->OnMessage(WM_CONTEXTMENU, (WPARAM)pRootView->GetHwnd(), MAKELPARAM(point.x, point.y));
}

void CUIControl::OnLostCapture()
{
	if (m_ptClick.cx != MAXINT16)
	{
		CPoint point;
		GetCursorPos(&point);
		GetRootView()->ScreenToClient(&point);

		FRIEND(GetParent())->OnChildMoved(this, point - m_ptClick);
		m_ptClick.SetSize(MAXINT16, MAXINT16);
	}
}

void CUIControl::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"tooltip"))
		SetToolTip(lpStr);

	if (lpStr = attrs.GetStr(L"cursor"))
	{
		if (_wcsicmp(lpStr, L"hand") == 0)
			SetCursor(LoadCursor(NULL, IDC_HAND));
		else if (_wcsicmp(lpStr, L"ibeam") == 0)
			SetCursor(LoadCursor(NULL, IDC_IBEAM));
	}
}
