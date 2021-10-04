#include "stdafx.h"
#include "UIStateButton.h"

CUIStateButton::CUIStateButton(CUIView *pParent) : CUIView(pParent), m_nState(0)
{
}

CUIStateButton::~CUIStateButton()
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

		((CUIButton *)pItem)->OnClick([this]{ OnChildClick(); });

		CSize size = pItem->GetSize();

		if (nWidth < size.cx)
			nWidth = size.cx;

		if (nHeight < size.cy)
			nHeight = size.cy;
	}

	SetSize({ nWidth, nHeight });
}

void CUIStateButton::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	for (int i = 0; i != m_vecChilds.size(); i++)
	{
		FRIEND(m_vecChilds[i])->CalcRect(i == m_nState ? &CRect(m_rect) : NULL, lpClipRect);
	}
}

void CUIStateButton::SetState(int nState)
{
	nState = (nState + m_vecChilds.size()) % m_vecChilds.size();

	if (m_nState == nState)
		return;

	m_nState = nState;
	InvalidateLayout();
}

void CUIStateButton::OnChildClick()
{
	int nState = m_nState;
	m_nState = (nState + 1 + m_vecChilds.size()) % m_vecChilds.size();

	if (m_fnOnClick)
		m_fnOnClick(nState);

	if (m_nState == nState)
		return;

	InvalidateLayout();
}

void CUIStateButton::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	EndAddChild();

	int nValue;
	if (attrs.GetInt(L"state", &nValue))
		SetState(nValue);
}
