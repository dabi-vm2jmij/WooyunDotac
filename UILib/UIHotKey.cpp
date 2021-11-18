#include "stdafx.h"
#include "UIHotKey.h"

CUIHotKey::CUIHotKey(CUIView *pParent) : CUIControl(pParent), m_bFocus(false)
{
	SetHotKey(0);
}

namespace
{

wstring GetKeyName(UINT nCode)
{
	long lScan = MapVirtualKey(nCode, MAPVK_VK_TO_VSC) << 16;
	if (nCode >= VK_PRIOR && nCode <= VK_HELP || nCode == VK_DIVIDE || nCode == VK_NUMLOCK)
		lScan |= 0x01000000;

	wchar_t szName[64] = {};
	GetKeyNameTextW(lScan, szName, _countof(szName));
	return szName;
}

UINT GetModifiers()
{
	UINT nModifiers = 0;

	if (GetKeyState(VK_CONTROL) < 0)
		nModifiers |= MOD_CONTROL;

	if (GetKeyState(VK_SHIFT) < 0)
		nModifiers |= MOD_SHIFT;

	if (GetKeyState(VK_MENU) < 0)
		nModifiers |= MOD_ALT;

	return nModifiers;
}

}	// namespace

void CUIHotKey::SetHotKey(UINT nHotKey)
{
	m_nHotKey = nHotKey;
	wstring strText;

	if (m_nModifiers & MOD_CONTROL)
	{
		strText += GetKeyName(VK_CONTROL);
		strText += L" + ";
	}

	if (m_nModifiers & MOD_SHIFT)
	{
		strText += GetKeyName(VK_SHIFT);
		strText += L" + ";
	}

	if (m_nModifiers & MOD_ALT)
	{
		strText += GetKeyName(VK_MENU);
		strText += L" + ";
	}

	int nOldSize = strText.size();

	switch (m_nVKCode)
	{
	case 0:
	case VK_BACK:
	case VK_RETURN:
	case VK_ESCAPE:
	case VK_CONTROL:
	case VK_SHIFT:
	case VK_MENU:
		break;

	default:
		strText += GetKeyName(m_nVKCode);
	}

	if (strText.size() == nOldSize)
		m_nVKCode = 0;

	SetText(strText.size() ? strText.c_str() : L"无");
}

UINT CUIHotKey::GetHotKey() const
{
	return m_nVKCode ? m_nHotKey : 0;
}

void CUIHotKey::SetFocus()
{
	if (IsRealVisible() && IsRealEnabled())
		GetRootView()->SetFocus(this);
}

bool CUIHotKey::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		SetHotKey(GetModifiers() << 16 | wParam);
		return true;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (m_nVKCode == 0)
			SetHotKey(GetModifiers() << 16);
		return true;
	}

	return __super::OnMessage(uMsg, wParam, lParam);
}

void CUIHotKey::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	CRect rect(m_rect);
	rect.top = (rect.top + rect.bottom - m_textSize.cy) / 2;
	rect.bottom = rect.top + m_textSize.cy;

	SelectObject(dc, m_hFont);
	::SetTextColor(dc, IsRealEnabled() ? m_color : GetSysColor(COLOR_GRAYTEXT));
	DrawTextW(dc, m_strText.c_str(), -1, rect, DT_SINGLELINE);

	// DrawText 后填充 alpha
	dc.FillAlpha(rect, 255);
}

void CUIHotKey::OnEnable(bool bEnable)
{
	__super::OnEnable(bEnable);

	InvalidateRect();

	if (!bEnable)
	{
		auto pRootView = GetRootView();
		if (pRootView->GetFocus() == this)
			pRootView->SetFocus(NULL);
	}
}

void CUIHotKey::OnVisible(bool bVisible)
{
	__super::OnVisible(bVisible);

	if (!bVisible)
	{
		auto pRootView = GetRootView();
		if (pRootView->GetFocus() == this)
			pRootView->SetFocus(NULL);
	}
}

void CUIHotKey::OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	__super::OnRectChange(lpOldRect, lpClipRect);

	// 重设光标位置
	OnTextSize(m_textSize);
}

void CUIHotKey::OnLButtonDown(CPoint point)
{
	if (!m_bFocus)
		GetRootView()->SetFocus(this);
}

void CUIHotKey::OnSetFocus()
{
	if (m_bFocus)
		return;

	m_bFocus = true;
	OnTextSize(m_textSize);
}

void CUIHotKey::OnKillFocus()
{
	if (!m_bFocus)
		return;

	m_bFocus = false;
	GetRootView()->SetCaretPos(CRect());

	if (m_nVKCode == 0)
		SetHotKey(0);
}

void CUIHotKey::OnTextSize(CSize size)
{
	if (!m_bFocus)
		return;

	CPoint point(m_rect.left + size.cx, (m_rect.top + m_rect.bottom - size.cy) / 2);
	GetRootView()->SetCaretPos(CRect(point, CSize(1, size.cy)) & m_rcReal);
}

void CUIHotKey::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	OnLoadText(attrs);
}
