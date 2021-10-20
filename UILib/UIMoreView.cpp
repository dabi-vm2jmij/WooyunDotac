#include "stdafx.h"
#include "UIMoreView.h"

CUIMoreView::CUIMoreView(IUIWindow *pOwner) : CUIRootView(pOwner), m_pToolBar(NULL)
{
}

CUIMoreView::~CUIMoreView()
{
	ClearItems();
}

void CUIMoreView::InitItems(const vector<CUIView *> &vecItems)
{
	m_pToolBar = vecItems[0]->GetParent();

	for (auto pItem : vecItems)
	{
		if (pItem->IsVisible())
		{
			FRIEND(pItem)->m_pParent = this;
			AddChild(pItem);
		}
	}
}

void CUIMoreView::ClearItems()
{
	CheckMouseLeave(UIHitTest());

	for (auto pItem : m_vecChilds)
	{
		if (FRIEND(pItem)->m_pParent == this)
		{
			FRIEND(pItem)->SetRect(NULL, NULL);
			FRIEND(pItem)->m_pParent = m_pToolBar;
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
		CSize size = pItem->GetSize();

		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;
		FRIEND(pItem)->SetRect(rect, lpClipRect);
		rect.top = rect.bottom;
	}
}

void CUIMoreView::CalcSize(LPSIZE lpSize) const
{
	int nWidth = 0, nHeight = 0;

	for (auto pItem : m_vecChilds)
	{
		CSize size = pItem->GetSize();

		if (nWidth < size.cx)
			nWidth = size.cx;

		nHeight += size.cy;
	}

	lpSize->cx = 2 + nWidth;
	lpSize->cy = 2 + nHeight;
}
