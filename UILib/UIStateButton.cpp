#include "stdafx.h"
#include "UIStateButton.h"

CUIStateButton::CUIStateButton(CUIView *pParent) : CUIControl(pParent), m_nState(0)
{
}

void CUIStateButton::EndAddChild()
{
	if (m_vecChilds.size() < 2)
	{
		ATLASSERT(0);
		return;
	}

	int nWidth = 0, nHeight = 0;

	for (auto pItem : m_vecChilds)
	{
		if (dynamic_cast<CUIButton *>(pItem) == NULL)
		{
			ATLASSERT(0);
			return;
		}

		CSize size = pItem->GetSize();

		if (nWidth < size.cx)
			nWidth = size.cx;

		if (nHeight < size.cy)
			nHeight = size.cy;
	}

	SetSize({ nWidth, nHeight });
}

void CUIStateButton::SetState(UINT nState)
{
	nState %= m_vecChilds.size();

	if (m_nState != nState)
	{
		m_nState = nState;
		InvalidateLayout();
	}
}

HCURSOR CUIStateButton::GetCursor() const
{
	return ((CUIControl *)m_vecChilds[m_nState])->GetCursor();
}

LPCWSTR CUIStateButton::GetToolTip() const
{
	return ((CUIControl *)m_vecChilds[m_nState])->GetToolTip();
}

bool CUIStateButton::OnHitTest(UIHitTest &hitTest)
{
	if (m_vecChilds.size() <= m_nState || !m_vecChilds[m_nState]->GetRect().PtInRect(hitTest.point))
		return false;

	hitTest.Add(this);
	hitTest.Add(m_vecChilds[m_nState]);

	if (!IsEnabled())
		hitTest.items[hitTest.nCount - 1].bEnable = false;

	return true;
}

void CUIStateButton::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	for (int i = 0; i != m_vecChilds.size(); i++)
		FRIEND(m_vecChilds[i])->CalcRect(i == m_nState ? &CRect(m_rect) : NULL, lpClipRect);
}

void CUIStateButton::OnLButtonDown(CPoint point)
{
	FRIEND(m_vecChilds[m_nState])->OnLButtonDown(point);
}

void CUIStateButton::OnLButtonUp(CPoint point)
{
	FRIEND(m_vecChilds[m_nState])->OnLButtonUp(point);

	// œ‘ æœ¬“ªÃ¨
	UINT nState = m_nState;
	SetState(m_nState + 1);

	if (m_fnOnClick)
		m_fnOnClick(nState);
}

void CUIStateButton::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	EndAddChild();

	int nValue;
	if (attrs.GetInt(L"state", &nValue))
		SetState(nValue);
}
