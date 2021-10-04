#include "stdafx.h"
#include "UIWebTabBar.h"
#include "UIWebTab.h"

CUIWebTabBar::CUIWebTabBar(CUIView *pParent, LPCWSTR lpFileName) : CUIView(pParent)
{
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

void CUIWebTabBar::ActivateTab(CUIWebTab *pWebTab)
{
	for (auto pItem : m_vecChilds)
	{
		if (pItem != pWebTab)
			((CUIWebTab *)pItem)->SetActive(false);
	}

	if (pWebTab)
		pWebTab->SetActive(true);
}

UINT CUIWebTabBar::GetTabIndex(CUIWebTab *pWebTab) const
{
	for (UINT i = 0; i != m_vecChilds.size(); i++)
	{
		if (m_vecChilds[i] == pWebTab)
			return i;
	}

	return -1;
}

CUIWebTab *CUIWebTabBar::AddWebTab(CUIWebTab *pWebTab)
{
	if (pWebTab == NULL)
	{
		pWebTab = new CUIWebTab(this, NULL);
		pWebTab->ResetImages(m_imgxTabs, _countof(m_imgxTabs));
	}

	pWebTab->SetBottom(0);
	pWebTab->SetWidth(0);
	AddChild(pWebTab);

	return pWebTab;
}

void CUIWebTabBar::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	// 计算每个标签宽度，不能整除的宽度，平均分给前 nMore 个
	int nWidth = m_nTabWidth, nCount = m_vecChilds.size(), nMore = 0;

	if (nWidth * nCount > m_rect.Width())
	{
		nWidth = m_rect.Width() / nCount;
		nMore  = m_rect.Width() - nWidth * nCount;
	}

	CRect rect(m_rect);

	for (auto pItem : m_vecChilds)
	{
		rect.right = rect.left + nWidth;

		if (nMore)
		{
			nMore--;
			rect.right++;
		}

		int nRight = rect.right;

		// 如果正在拖动标签，使用拖到的位置
		if (FRIEND(pItem)->m_offset.left != MAXINT16)
		{
			rect.right = m_rect.left + FRIEND(pItem)->m_offset.left + rect.Width();
			rect.left = m_rect.left;
		}

		FRIEND(pItem)->CalcRect(rect, lpClipRect);
		rect.left = nRight;
	}
}

void CUIWebTabBar::OnChildMoving(CUIControl *pCtrl, CPoint point)
{
	CRect rect = pCtrl->GetClientRect();

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

		int nCenter = m_vecChilds[i]->GetClientRect().CenterPoint().x;
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

	pCtrl->SetLeft(point.x - m_rect.left);
}

void CUIWebTabBar::OnChildMoved(CUIControl *pCtrl, CPoint point)
{
	pCtrl->SetLeft(MAXINT16);
}

void CUIWebTabBar::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"tabWidth", &nValue))
		SetTabWidth(nValue);
}
