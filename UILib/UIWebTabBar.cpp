#include "stdafx.h"
#include "UIWebTabBar.h"

CUIWebTabBar::CUIWebTabBar(CUIView *pParent, LPCWSTR lpFileName) : CUIView(pParent), m_nTabWidth(100), m_nSpacing(0)
{
	if (lpFileName)
		m_strTabImage = lpFileName;
}

UINT CUIWebTabBar::GetCurSel() const
{
	for (int i = 0; i != m_vecChilds.size(); i++)
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
	auto pWebTab = new CUIWebTab(this, m_strTabImage.c_str());
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
	if (m_vecChilds.size() > 1)
		SelectTab((CUIWebTab *)m_vecChilds[(GetCurSel() + 1) % m_vecChilds.size()]);
}

void CUIWebTabBar::SelectPrev()
{
	if (m_vecChilds.size() > 1)
		SelectTab((CUIWebTab *)m_vecChilds[(GetCurSel() - 1 + m_vecChilds.size()) % m_vecChilds.size()]);
}

void CUIWebTabBar::PaintChilds(CUIDC &dc) const
{
	// 最后画选中的标签
	CUIView *pSelTab = NULL;

	for (auto pItem : m_vecChilds)
	{
		if (((CUIWebTab *)pItem)->IsSelected())
			pSelTab = pItem;
		else
			pItem->DoPaint(dc);
	}

	if (pSelTab)
		pSelTab->DoPaint(dc);
}

void CUIWebTabBar::CalcRect(LPRECT lpRect, LPRECT lpClipRect)
{
	if (IsVisible() && !IsRectEmpty(lpRect) && m_offset.left >> 16 && m_size.cx == MAXINT16)
	{
		CRect rect(lpRect);
		CalcLeftRight(rect.left, rect.right, m_offset.left, m_offset.right, m_size.cx);

		int nNeedWidth = 0;
		if (m_vecChilds.size())
			nNeedWidth = (m_nTabWidth + m_nSpacing) * m_vecChilds.size() - m_nSpacing;

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
	int nAllWidth = m_rect.Width() - m_nSpacing * (nCount - 1);

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

		int nRight = rect.right + m_nSpacing;

		// 如果正在拖动标签，使用拖到的位置
		if (pItem->GetOffset().left != MAXINT16)
			rect.MoveToX((short)pItem->GetOffset().left);

		pItem->SetRect(rect, lpClipRect);
		rect.left = nRight;
	}
}

void CUIWebTabBar::OnChildMoving(CUIView *pItem, CPoint point)
{
	CRect rect = pItem->GetRect();

	if (point.x < m_rect.left)
		point.x = m_rect.left;

	if (point.x > m_rect.right - rect.Width())
		point.x = m_rect.right - rect.Width();

	if (point.x == rect.left)
		return;

	// 找出新位置
	UINT nCurIndex = -1, nNewIndex = -1;

	for (int i = 0; (nCurIndex == -1 || nNewIndex == -1) && i != m_vecChilds.size(); i++)
	{
		if (m_vecChilds[i] == pItem)
		{
			nCurIndex = i;
			continue;
		}

		int nCenter = m_vecChilds[i]->GetRect().CenterPoint().x;
		if (nCenter >= point.x && nCenter < point.x + rect.Width())
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

		m_vecChilds[nNewIndex] = pItem;
	}

	pItem->SetLeft(point.x);
}

void CUIWebTabBar::OnChildMoved(CUIView *pItem, CPoint point)
{
	pItem->SetLeft(MAXINT16);
}

void CUIWebTabBar::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"tabWidth", &nValue))
		SetTabWidth(nValue);

	if (attrs.GetInt(L"spacing", &nValue))
		SetSpacing(nValue);
}
