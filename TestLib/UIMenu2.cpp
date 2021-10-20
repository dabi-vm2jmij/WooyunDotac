#include "stdafx.h"
#include "UIMenu2.h"

void CUIMenu2::GetMargins(LPRECT lpRect) const
{
	SetRect(lpRect, 8, 8, 8, 8);
}

void CUIMenu2::DrawBg(CUIDC &dc, LPCRECT lpRect)
{
	if (!m_imagexBg)
		m_imagexBg = UILib::GetImage(L"²Ëµ¥bg.png");

	m_imagexBg.StretchDraw(dc, *lpRect, false);
}
