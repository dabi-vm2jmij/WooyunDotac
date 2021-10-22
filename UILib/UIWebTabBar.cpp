#include "stdafx.h"
#include "UIWebTabBar.h"

CUIWebTabBar::CUIWebTabBar(CUIView *pParent, LPCWSTR lpFileName) : CUIView(pParent), m_nTabWidth(100), m_nTabSpace(0)
{
	if (lpFileName == NULL)
		return;

	// 预加载标签图片
	UINT nCount = SplitImage(lpFileName, m_imgxTabs);
	m_nTabWidth = m_imgxTabs[0].Rect().Width();

	// 如果为3态，则为：0、1、2、0
	for (UINT i = nCount; i < 4; i++)
		m_imgxTabs[i] = m_imgxTabs[0];

	// 如果为2态，则为：0、0、1、0
	if (nCount == 2)
		m_imgxTabs[2].Swap(m_imgxTabs[1]);
}

CUIWebTabBar::~CUIWebTabBar()
{
}

UINT CUIWebTabBar::GetCurSel() const
{
	for (UINT i = 0; i != m_vecChilds.size(); i++)
	{
		if (((CUIWebTab *)m_vecChilds[i])->IsSelected())
			return i;
	}

	return -1;
}

CUIWebTab *CUIWebTabBar::GetCurTab() const
{
	UINT nCurSel = GetCurSel();
	return nCurSel != -1 ? (CUIWebTab *)m_vecChilds[nCurSel] : NULL;
}

CUIWebTab *CUIWebTabBar::AddWebTab()
{
	auto pWebTab = new CUIWebTab(this, NULL);
	pWebTab->ResetImages(m_imgxTabs, _countof(m_imgxTabs));

	AddChild(pWebTab);
	GetParent()->InvalidateLayout();
	return pWebTab;
}

void CUIWebTabBar::DeleteTab(CUIWebTab *pWebTab)
{
	if (pWebTab->IsSelected())
	{
		if (pWebTab != m_vecChilds.back())
			SelectNext();
		else
			SelectPrev();
	}

	DeleteChild(pWebTab);
	GetParent()->InvalidateLayout();
}

void CUIWebTabBar::SelectTab(CUIWebTab *pWebTab)
{
	if (pWebTab->IsSelected())
		return;

	CUIWebTab *pOldTab = GetCurTab();
	pWebTab->OnSelect(true);

	if (pOldTab)
		pOldTab->OnSelect(false);

	if (m_fnOnSelect)
		m_fnOnSelect();
}

void CUIWebTabBar::SelectNext()
{
	if (m_vecChilds.size() < 2)
		return;

	SelectTab((CUIWebTab *)m_vecChilds[(GetCurSel() + 1) % m_vecChilds.size()]);
}

void CUIWebTabBar::SelectPrev()
{
	if (m_vecChilds.size() < 2)
		return;

	SelectTab((CUIWebTab *)m_vecChilds[(GetCurSel() - 1 + m_vecChilds.size()) % m_vecChilds.size()]);
}

void CUIWebTabBar::CalcRect(LPRECT lpRect, LPRECT lpClipRect)
{
	if (IsVisible() && !IsRectEmpty(lpRect) && m_offset.left >> 16 && m_size.cx == 0)
	{
		CRect rect = lpRect;
		CalcLeftRight(rect.left, rect.right, m_offset.left, m_offset.right, m_size.cx);

		int nNeedWidth = 0;
		if (m_vecChilds.size())
			nNeedWidth = (m_nTabWidth + m_nTabSpace) * m_vecChilds.size() - m_nTabSpace;

		// 可用宽度大于需要宽度时，使用需要宽度
		if (rect.Width() > nNeedWidth)
		{
			int nRight = lpRect->right;
			lpRect->right -= rect.Width() - nNeedWidth;
			__super::CalcRect(lpRect, lpClipRect);
			lpRect->right = nRight;
			return;
		}
	}

	__super::CalcRect(lpRect, lpClipRect);
}

void CUIWebTabBar::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	// 计算每个标签宽度，不能整除的宽度，平均分给前 more 个
	int nTabWidth = m_nTabWidth, nCount = m_vecChilds.size(), nMore = 0;
	int nAllWidth = m_rect.Width() - m_nTabSpace * (nCount - 1);

	if (nTabWidth * nCount > nAllWidth)
	{
		nTabWidth = nAllWidth / nCount;

		if (nTabWidth < 1)
			nTabWidth = 1;
		else
			nMore = nAllWidth - nTabWidth * nCount;
	}

	CRect rect(m_rect);

	for (auto pItem : m_vecChilds)
	{
		rect.right = rect.left + nTabWidth;
		rect.top = rect.bottom - pItem->GetSize().cy;

		if (nMore)
		{
			nMore--;
			rect.right++;
		}

		int nRight = rect.right + m_nTabSpace;

		// 如果正在拖动标签，使用拖到的位置
		if (FRIEND(pItem)->m_offset.left != MAXINT16)
		{
			rect.MoveToX(FRIEND(pItem)->m_offset.left);
		}

		FRIEND(pItem)->SetRect(rect, lpClipRect);
		rect.left = nRight;
	}
}

void CUIWebTabBar::OnChildMoving(CUIControl *pCtrl, CPoint point)
{
	CRect rect = pCtrl->GetRect();

	if (point.x < m_rect.left)
		point.x = m_rect.left;

	if (point.x > m_rect.right - rect.Width())
		point.x = m_rect.right - rect.Width();

	if (point.x == rect.left)
		return;

	int nRight = point.x + rect.Width();

	// 找出新位置
	UINT nCurIndex = -1, nNewIndex = -1;

	for (UINT i = 0; (nCurIndex == -1 || nNewIndex == -1) && i != m_vecChilds.size(); i++)
	{
		if (m_vecChilds[i] == pCtrl)
		{
			nCurIndex = i;
			continue;
		}

		int nCenter = m_vecChilds[i]->GetRect().CenterPoint().x;
		if (nCenter > point.x && nCenter <= nRight)
			nNewIndex = i;
	}

	if (nCurIndex == -1)
		return;

	// 插入到新位置
	if (nCurIndex != nNewIndex && nNewIndex != -1)
	{
		if (nCurIndex < nNewIndex)
			memmove(&m_vecChilds[nCurIndex], &m_vecChilds[nCurIndex + 1], (nNewIndex - nCurIndex) * 4);
		else
			memmove(&m_vecChilds[nNewIndex + 1], &m_vecChilds[nNewIndex], (nCurIndex - nNewIndex) * 4);

		m_vecChilds[nNewIndex] = pCtrl;
	}

	pCtrl->SetLeft(point.x);
}

void CUIWebTabBar::OnChildMoved(CUIControl *pCtrl, CPoint point)
{
	pCtrl->SetLeft(MAXINT16);
}

void CUIWebTabBar::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"tabWidth", &nValue))
		SetTabWidth(nValue);

	if (attrs.GetInt(L"tabSpace", &nValue))
		SetTabSpace(nValue);
}
