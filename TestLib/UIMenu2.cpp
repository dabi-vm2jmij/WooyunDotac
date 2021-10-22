#include "stdafx.h"
#include "UIMenu2.h"

void CUIMenu2::GetMargins(LPRECT lpRect) const
{
	SetRect(lpRect, 8, 8, 8, 8);
}

void CUIMenu2::DrawBg(CUIDC &dc, LPCRECT lpRect)
{
	if (!m_bgImagex)
		m_bgImagex = UILib::GetImage(L"²Ëµ¥bg.png");

	m_bgImagex.Scale9Draw(dc, *lpRect, false);
}
