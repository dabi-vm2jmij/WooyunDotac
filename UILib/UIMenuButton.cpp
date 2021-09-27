#include "stdafx.h"
#include "UIMenuButton.h"

CUIMenuButton::CUIMenuButton(CUIView *pParent, LPCWSTR lpFileName) : CUIButton(pParent, lpFileName)
{
}

CUIMenuButton::~CUIMenuButton()
{
}

void CUIMenuButton::OnLButtonDown(CPoint point)
{
	__super::OnLButtonDown(point);

	CRect rect(0, 0, MAXINT16, MAXINT16);
	GetPopupPos(rect);

	m_bKeepEnter = true;
	m_fnGetUIMenu()->Popup(GetRootView()->GetOwnerWnd(), rect.left, rect.top, rect.right, rect.bottom, true);
	m_bKeepEnter = false;

	if (GetAsyncKeyState(VK_LBUTTON) >= 0)	// 通过键盘关闭菜单
	{
		m_bLButtonDown = false;
		OnLButtonUp(point);
	}

	GetRootView()->RaiseMouseMove();
}

void CUIMenuButton::GetPopupPos(LPRECT lpRect)
{
	CRect rect;
	GetWindowRect(rect);

	lpRect->left = rect.left;
	lpRect->top = rect.bottom;
//	lpRect->right = rect.right;
	lpRect->bottom = rect.top;
}
