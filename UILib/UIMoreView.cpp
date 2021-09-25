#include "stdafx.h"
#include "UIMoreView.h"

CUIMoreView::CUIMoreView(IUIWindow *pWindow) : CUIRootView(pWindow)
{
}

void CUIMoreView::InitItems(const std::vector<CUIBase *> &pItems)
{
	for (auto pItem : pItems)
	{
		if (pItem->IsVisible() && dynamic_cast<CUIView *>(pItem))
		{
			FRIEND(pItem)->m_pParent = this;
			AddChild(pItem);
		}
	}
}

void CUIMoreView::ClearItems()
{
	for (auto pItem : m_vecEnterItems)
	{
		if (pItem)
			FRIEND(pItem)->DoMouseLeave(true);
	}

	for (auto pItem : m_vecChilds)
	{
		if (FRIEND(pItem)->m_pParent == this)
		{
			FRIEND(pItem)->CalcRect(NULL, NULL);
			FRIEND(pItem)->m_pParent = NULL;
		}
	}

	m_vecChilds.clear();
}

void CUIMoreView::RecalcLayout(LPRECT lpClipRect)
{
	CRect rect(m_rect);
	rect.DeflateRect(1, 1);

	for (auto pItem : m_vecChilds)
	{
		CSize size;
		pItem->GetSize(&size);

		CRect rcItem(rect);
		rcItem.right = rect.left + size.cx;
		rcItem.bottom = (rect.top += size.cy);

		// 需要修正左、上、下，使 CalcRect 切出正确的 rect
		ATLASSERT(FRIEND(pItem)->m_offset.left >> 16);
		rcItem.left -= (short)FRIEND(pItem)->m_offset.left;

		if (FRIEND(pItem)->m_offset.top != MAXINT16)
		{
			rcItem.top -= (short)FRIEND(pItem)->m_offset.top;
		}
		else if (FRIEND(pItem)->m_offset.bottom != MAXINT16)
		{
			rcItem.bottom += (short)FRIEND(pItem)->m_offset.bottom;
		}

		FRIEND(pItem)->CalcRect(rcItem, lpClipRect);
	}
}

void CUIMoreView::CalcSize(LPSIZE lpSize) const
{
	int nWidth = 0, nHeight = 0;

	for (auto pItem : m_vecChilds)
	{
		CSize size;
		pItem->GetSize(&size);

		if (nWidth < size.cx)
			nWidth = size.cx;

		nHeight += size.cy;
	}

	lpSize->cx = 2 + nWidth;
	lpSize->cy = 2 + nHeight;
}
