#include "stdafx.h"
#include "UIMenu2.h"

void CUIMenu2::DrawBg(CUIDC &dc, LPCRECT lpRect)
{
	if (!m_imagexBg)
		m_imagexBg = UILib::GetImage(L"²Ëµ¥bg.png");

	auto fnDraw = [](HDC hdcDst, int xDst, int yDst, int wDst, int hDst, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc)
	{
		StretchBlt(hdcDst, xDst, yDst, wDst, hDst, hdcSrc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
	};

	m_imagexBg.StretchDraw(dc, *lpRect, false);
}
