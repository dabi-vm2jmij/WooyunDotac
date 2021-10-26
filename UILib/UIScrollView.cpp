#include "stdafx.h"
#include "UIScrollView.h"

CUIScrollView::CUIScrollView(CUIView *pParent) : CUIView(pParent), m_pVScroll(NULL)
{
}

void CUIScrollView::SetVScroll(CUIScrollBar *pVScroll)
{
	ATLASSERT(pVScroll->IsVertical() && !IsChild(pVScroll));
	m_pVScroll = pVScroll;
	m_pVScroll->BindChange([this]{ InvalidateLayout(); });
	CheckVScroll();
}

void CUIScrollView::CheckVScroll()
{
	if (m_pVScroll == NULL)
		return;

	int nHeight;

	if (m_rect.IsRectEmpty())
	{
		m_pVScroll->SetVisible(false);
	}
	else if ((nHeight = CalcHeight()) <= m_rect.Height())
	{
		m_pVScroll->SetVisible(false);
		m_pVScroll->SetCurPos(0);
	}
	else
	{
		m_pVScroll->SetRange(m_rect.Height(), nHeight);
		m_pVScroll->SetVisible(true);
	}
}

bool CUIScrollView::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (__super::OnMessage(uMsg, wParam, lParam))
		return true;

	return uMsg == WM_MOUSEWHEEL && m_pVScroll && m_pVScroll->IsEnabled() && m_pVScroll->IsVisible() && FRIEND(m_pVScroll)->OnMessage(uMsg, wParam, lParam);
}

void CUIScrollView::OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	if (m_rect.Height() != lpOldRect->bottom - lpOldRect->top)
		CheckVScroll();

	__super::OnRectChange(lpOldRect, lpClipRect);
}

void CUIScrollView::OnNeedLayout(LPRECT lpClipRect)
{
	if (m_bNeedLayout)
		CheckVScroll();

	__super::OnNeedLayout(lpClipRect);
}

void CUIScrollView::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	CRect rect(m_rect);

	if (m_pVScroll)
		rect.top -= m_pVScroll->GetCurPos();

	for (auto pItem : m_vecChilds)
		FRIEND(pItem)->CalcRect(rect, lpClipRect);
}

int CUIScrollView::CalcHeight() const
{
	int nHeight = 0;

	for (auto pItem : m_vecChilds)
	{
		CSize size = pItem->GetSize();
		ATLASSERT(size.cy != MAXINT16);

		if (pItem->GetOffset().top >> 16)
			nHeight += size.cy + (short)pItem->GetOffset().top;
		else if (pItem->GetOffset().bottom >> 16)
			nHeight += size.cy + (short)pItem->GetOffset().bottom;
	}

	return nHeight;
}
