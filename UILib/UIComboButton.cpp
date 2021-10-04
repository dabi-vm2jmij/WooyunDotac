#include "stdafx.h"
#include "UIComboButton.h"

CUIComboButton::CUIComboButton(CUIView *pParent) : CUIView(pParent), m_bInNotify(false)
{
}

CUIComboButton::~CUIComboButton()
{
}

static UINT GetAlign(const CRect &offset)
{
	UINT nAlign = 0;

	if (offset.left >> 16)
		nAlign |= 1;

	if (offset.right >> 16)
		nAlign |= 2;

	if (offset.top >> 16)
		nAlign |= 4;

	if (offset.bottom >> 16)
		nAlign |= 8;

	return nAlign;
}

void CUIComboButton::EndAddChild()
{
	if (m_vecChilds.size() < 2)
	{
		ATLASSERT(0);
		return;
	}

	int  nWidth = 0, nHeight = 0;
	UINT nAlign = GetAlign(FRIEND(m_vecChilds[0])->m_offset);

	if (nAlign == 0 || (nAlign & (nAlign - 1)))
	{
		ATLASSERT(0);
		return;
	}

	for (auto pItem : m_vecChilds)
	{
		if (dynamic_cast<CUIButton *>(pItem) == NULL || GetAlign(FRIEND(pItem)->m_offset) != nAlign)
		{
			ATLASSERT(0);
			return;
		}

		CSize size = pItem->GetSize();

		if (nAlign <= 2)
		{
			nWidth += size.cx;

			if (nHeight < size.cy)
				nHeight = size.cy;
		}
		else
		{
			nHeight += size.cy;

			if (nWidth < size.cx)
				nWidth = size.cx;
		}
	}

	SetSize({ nWidth, nHeight });
}

void CUIComboButton::OnChildEnter(CUIButton *pChild)
{
	if (m_bInNotify)
		return;

	m_bInNotify = true;

	for (auto pItem : m_vecChilds)
	{
		if (pItem != pChild && !pItem->IsMouseEnter())
			FRIEND(pItem)->OnMouseEnter();
	}

	m_bInNotify = false;
}

void CUIComboButton::OnChildLeave(CUIButton *pChild)
{
	if (m_bInNotify)
		return;

	m_bInNotify = true;

	for (auto pItem : m_vecChilds)
	{
		if (pItem != pChild && pItem->IsMouseEnter())
		{
			FRIEND(pChild)->OnMouseEnter();
			m_bInNotify = false;
			return;
		}
	}

	for (auto pItem : m_vecChilds)
	{
		if (pItem != pChild && !pItem->IsMouseEnter())
			FRIEND(pItem)->OnMouseLeave();
	}

	m_bInNotify = false;
}

void CUIComboButton::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	EndAddChild();
}
