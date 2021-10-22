#include "stdafx.h"
#include "UIComboButton.h"

CUIComboButton::CUIComboButton(CUIView *pParent) : CUIControl(pParent), m_bNoLeave(false)
{
}

CUIComboButton::~CUIComboButton()
{
}

static UINT GetAlign(const CRect &offset)
{
	UINT nAlign = 0;

	if (offset.left >> 16)
		nAlign |= 1;

	if (offset.right >> 16)
		nAlign |= 2;

	if (offset.top >> 16)
		nAlign |= 4;

	if (offset.bottom >> 16)
		nAlign |= 8;

	return nAlign;
}

void CUIComboButton::EndAddChild()
{
	if (m_vecChilds.size() < 2)
	{
		ATLASSERT(0);
		return;
	}

	int  nWidth = 0, nHeight = 0;
	UINT nAlign = GetAlign(FRIEND(m_vecChilds[0])->m_offset);

	if (nAlign == 0 || (nAlign & (nAlign - 1)))
	{
		ATLASSERT(0);
		return;
	}

	for (auto pItem : m_vecChilds)
	{
		if (dynamic_cast<CUIButton *>(pItem) == NULL || GetAlign(FRIEND(pItem)->m_offset) != nAlign)
		{
			ATLASSERT(0);
			return;
		}

		CSize size = pItem->GetSize();

		if (nAlign <= 2)
		{
			nWidth += size.cx;

			if (nHeight < size.cy)
				nHeight = size.cy;
		}
		else
		{
			nHeight += size.cy;

			if (nWidth < size.cx)
				nWidth = size.cx;
		}
	}

	SetSize({ nWidth, nHeight });
}

bool CUIComboButton::OnHitTest(UIHitTest &hitTest)
{
	UINT nCount = hitTest.nCount;
	hitTest.Add(this);

	if (!CUIView::OnHitTest(hitTest))
	{
		// 没有进入子控件
		hitTest.nCount = nCount;
		return false;
	}

	// 记录子控件的 cursor、tooltip
	for (UINT i = nCount + 1; i < hitTest.nCount; i++)
	{
		auto hit = hitTest.items[i];

		if (hit.pItem->IsControl())
		{
			if (hit.bEnable)
				m_hCursor = FRIEND(hit.pItem)->m_hCursor;

			m_strToolTip = FRIEND(hit.pItem)->m_strToolTip;
			break;
		}
	}

	return true;
}

bool CUIComboButton::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		OnMouseEnter();
		return false;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONUP:
		break;

	default:
		return __super::OnMessage(uMsg, wParam, lParam);
	}

	// 转发消息给子控件
	if (auto pCtrl = GetHitChild(lParam))
	{
		bool bLBtnDown = uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK;
		m_bNoLeave = bLBtnDown;
		FRIEND(pCtrl)->OnMessage(uMsg, wParam, lParam);
		m_bNoLeave = false;

		if (bLBtnDown && !IsMouseEnter())
		{
			// MenuButton 菜单关闭
			UIHitTest hitTest;
			GetCursorPos(&hitTest.point);
			GetRootView()->ScreenToClient(&hitTest.point);

			if (CUIView::OnHitTest(hitTest))
				OnMouseEnter();
			else
				OnMouseLeave();
		}
	}

	return true;
}

void CUIComboButton::OnMouseEnter()
{
	for (auto pItem : m_vecChilds)
		FRIEND(pItem)->OnMouseEnter();
}

void CUIComboButton::OnMouseLeave()
{
	if (m_bNoLeave)
		return;

	for (auto pItem : m_vecChilds)
		FRIEND(pItem)->OnMouseLeave();
}

CUIControl *CUIComboButton::GetHitChild(CPoint point)
{
	UIHitTest hitTest;
	hitTest.point = point;
	CUIView::OnHitTest(hitTest);

	for (auto hit : hitTest)
	{
		if (hit.pItem->IsControl())
		{
			if (hit.bEnable)
				return (CUIControl *)hit.pItem;
			break;
		}
	}

	return NULL;
}

void CUIComboButton::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	EndAddChild();
}
