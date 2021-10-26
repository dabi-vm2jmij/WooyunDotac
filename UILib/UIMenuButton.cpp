#include "stdafx.h"
#include "UIMenuButton.h"

CUIMenuButton::CUIMenuButton(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName), m_bNoLeave(false)
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

	if (!m_fnOnPopup)
		return;

	m_bNoLeave = true;
	m_fnOnPopup();
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
