#include "stdafx.h"
#include "UIToolBar.h"

CUIToolBar::CUIToolBar(CUIView *pParent) : CUIView(pParent), m_hMoreWnd(NULL)
{
}

CUIToolBar::~CUIToolBar()
{
	if (m_vecMoreItems.size())
	{
		CloseMoreWnd();
		m_vecChilds.insert(m_vecChilds.end(), m_vecMoreItems.begin(), m_vecMoreItems.end());
	}
}

void CUIToolBar::GetMoreItems(vector<CUIView *> &vecItems)
{
	vecItems.reserve(m_vecMoreItems.size());

	for (auto pItem : m_vecMoreItems)
	{
		if (pItem->IsVisible())
			vecItems.push_back(pItem);
	}
}

void CUIToolBar::CloseMoreWnd()
{
	if (m_hMoreWnd)
	{
		if (IsWindow(m_hMoreWnd))
			DestroyWindow(m_hMoreWnd);

		m_hMoreWnd = NULL;
	}
}

void CUIToolBar::RecalcLayout(LPRECT lpClipRect)
{
	if (m_vecChilds.empty() || _wcsicmp(m_vecChilds[0]->GetId(), L"更多工具"))
	{
		__super::RecalcLayout(lpClipRect);
		return;
	}

	if (m_vecMoreItems.size())
	{
		CloseMoreWnd();
		m_vecChilds.insert(m_vecChilds.end(), m_vecMoreItems.begin(), m_vecMoreItems.end());
		m_vecMoreItems.clear();
	}

	if (m_rect.left == MAXINT16 || m_vecChilds.size() < 2)
		return;

	CRect rect(m_rect);

	if (int nIndex = GetMoreIndex())
	{
		for (int i = 1; i != nIndex; i++)
			FRIEND(m_vecChilds[i])->CalcRect(rect, lpClipRect);

		for (int i = nIndex; i != m_vecChilds.size(); i++)
			m_vecChilds[i]->SetRect(NULL, lpClipRect);

		FRIEND(m_vecChilds[0])->CalcRect(rect, lpClipRect);

		auto it = m_vecChilds.begin() + nIndex;
		m_vecMoreItems.insert(m_vecMoreItems.end(), it, m_vecChilds.end());
		m_vecChilds.erase(it, m_vecChilds.end());
	}
	else
	{
		for (int i = 1; i != m_vecChilds.size(); i++)
			FRIEND(m_vecChilds[i])->CalcRect(rect, lpClipRect);

		m_vecChilds[0]->SetRect(NULL, lpClipRect);
	}
}

int CUIToolBar::GetMoreIndex() const
{
	ATLASSERT(m_vecChilds[0]->GetOffset().left != MAXINT16);
	int nBtnWidth = (short)m_vecChilds[0]->GetOffset().left + m_vecChilds[0]->GetSize().cx;
	int nAllWidth = 0, nIndex = 0;

	for (int i = 1; i != m_vecChilds.size(); i++)
	{
		auto pItem = m_vecChilds[i];

		if (!pItem->IsVisible())
			continue;

		ATLASSERT(pItem->GetOffset().left >> 16);
		nAllWidth += (short)pItem->GetOffset().left + pItem->GetSize().cx;

		if (nAllWidth > m_rect.Width())
			return nIndex + 1;

		if (nAllWidth + nBtnWidth <= m_rect.Width())
			nIndex = i;
	}

	return 0;
}
