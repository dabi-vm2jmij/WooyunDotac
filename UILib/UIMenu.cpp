#include "stdafx.h"
#include "UIMenu.h"
#include "UIMenuWnd.h"

CUIMenu::Item::Item(UINT nId, LPCWSTR lpText) : m_nId(nId), m_bCheck(false), m_bEnable(true), m_pSubMenu(NULL)
{
	if (lpText)
		m_strText = lpText;
}

void CUIMenu::Item::SetSubMenu(CUIMenu *pUIMenu)
{
	if (m_pSubMenu != pUIMenu)
	{
		if (m_pSubMenu)
			delete m_pSubMenu;

		m_pSubMenu = pUIMenu;
	}
}

CUIMenu::CUIMenu()
{
}

CUIMenu::~CUIMenu()
{
	for (auto &item : m_vecItems)
		item.SetSubMenu(NULL);
}

void CUIMenu::CreateFromMenu(HMENU hMenu)
{
	for (int i = 0, n = GetMenuItemCount(hMenu); i != n; i++)
	{
		Item item;
		UINT nState = GetMenuState(hMenu, i, MF_BYPOSITION);

		wchar_t szText[256];
		GetMenuStringW(hMenu, i, szText, _countof(szText), MF_BYPOSITION);

		if (nState & MF_POPUP)
		{
			item.m_nId = -1;
			item.m_strText = szText;

			item.SetSubMenu(NewUIMenu());
			item.GetSubMenu()->CreateFromMenu(GetSubMenu(hMenu, i));
		}
		else if (nState & MF_SEPARATOR)
		{
			item.m_nId = 0;
		}
		else
		{
			item.m_nId = GetMenuItemID(hMenu, i);
			item.m_strText = szText;
		}

		if (nState & MF_CHECKED)
			item.m_bCheck = true;

		if (nState & MF_DISABLED)
			item.m_bEnable = false;

		m_vecItems.push_back(item);
	}
}

CUIMenu::Item *CUIMenu::GetItem(UINT nId)
{
	if (nId < m_vecItems.size())
		return &m_vecItems[nId];

	for (auto &item : m_vecItems)
	{
		if (item.m_nId == nId)
			return &item;
	}

	m_vecItems.push_back((int)nId < 0 ? 0 : nId);
	return &m_vecItems.back();
}

UINT CUIMenu::Popup(HWND hParent, int x1, int y1, int x2, int y2, bool bPostMsg)
{
	return CUIMenuWnd(this).Popup(hParent, x1, y1, x2, y2, bPostMsg);
}

void CUIMenu::GetMargins(LPRECT lpRect) const
{
	SetRect(lpRect, 1, 3, 1, 3);
}

UINT CUIMenu::GetShowDelay() const
{
	DWORD dwDelay = 0;
	SystemParametersInfo(SPI_GETMENUSHOWDELAY, 0, &dwDelay, 0);
	return dwDelay;
}

void CUIMenu::MeasureItem(UINT nIndex, LPSIZE lpSize) const
{
	if (m_vecItems[nIndex].m_nId)
	{
		lpSize->cx = 160;
		lpSize->cy = 24;
	}
	else
	{
		lpSize->cx = 1;
		lpSize->cy = 11;
	}
}

void CUIMenu::DrawBg(CUIDC &dc, LPCRECT lpRect)
{
	CRect rect(lpRect);
	dc.FillSolidRect(rect, RGB(186, 186, 186));
	rect.DeflateRect(1, 1);
	dc.FillSolidRect(rect, RGB(255, 255, 255));
}

void CUIMenu::DrawItem(CUIDC &dc, LPCRECT lpRect, UINT nIndex, bool bSelected)
{
	const Item &item = m_vecItems[nIndex];
	CRect rect(lpRect);

	if (item.m_nId)
	{
		if (bSelected)
			dc.FillSolidRect(rect, RGB(235, 235, 235));

		if (item.m_bEnable)
			SetTextColor(dc, RGB(0, 0, 0));
		else
			SetTextColor(dc, RGB(161, 161, 161));

		if (item.m_bCheck)
		{
			rect.left += 10;
			DrawTextW(dc, L"¡Ì", -1, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			rect.left -= 10;
		}

		rect.left += 32;
		DrawTextW(dc, item.m_strText.c_str(), -1, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		if (item.GetSubMenu())
		{
			rect.left = rect.right - 20;
			DrawTextW(dc, L"¡ú", -1, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}

		// DrawText ºóÌî³ä alpha
		dc.FillAlpha(lpRect, 255);
	}
	else
	{
		rect.top = (rect.top + rect.bottom - 1) / 2;
		rect.bottom = rect.top + 1;
		rect.DeflateRect(1, 0);
		dc.FillSolidRect(rect, RGB(233, 233, 233));
	}
}
