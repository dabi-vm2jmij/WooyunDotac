#include "stdafx.h"
#include "UIMenuButton.h"

CUIMenuButton::CUIMenuButton(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName), m_bNoLeave(false)
{
}

CUIMenuButton::~CUIMenuButton()
{
}

void CUIMenuButton::OnMouseLeave()
{
	if (m_bNoLeave)
		return;

	__super::OnMouseLeave();
}

void CUIMenuButton::OnLButtonDown(CPoint point)
{
	__super::OnLButtonDown(point);

	CRect rect(0, 0, MAXINT16, MAXINT16);
	GetPopupPos(rect);

	m_bNoLeave = true;
	m_fnGetUIMenu()->Popup(GetRootView()->GetHwnd(), rect.left, rect.top, rect.right, rect.bottom, true);
	m_bNoLeave = false;

	// 检查鼠标是否离开
	GetCursorPos(&point);
	GetRootView()->ScreenToClient(&point);

	if (!m_rect.PtInRect(point))
	{
		OnMouseLeave();
	}
	else if (GetKeyState(VK_LBUTTON) >= 0)
	{
		m_bLButtonDown = false;
		OnLButtonUp(point);
	}
}

void CUIMenuButton::GetPopupPos(LPRECT lpRect)
{
	CRect rect = GetWindowRect();

	lpRect->left = rect.left;
	lpRect->top = rect.bottom;
//	lpRect->right = rect.right;
	lpRect->bottom = rect.top;
}
