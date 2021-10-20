#include "stdafx.h"
#include "UIToolBar.h"
#include "UIMoreWnd.h"

CUIToolBar::CUIToolBar(CUIView *pParent, LPCWSTR lpFileName) : CUIView(pParent), m_moreBg(0x767676), m_hMoreWnd(NULL)
{
	CUIButton *pButton = AddButton(lpFileName);
	pButton->SetRight(0, true);
	pButton->SetToolTip(L"更多工具");
	pButton->BindClick([this]{ OnMoreBtn(); });
}

CUIToolBar::~CUIToolBar()
{
	if (m_vecMoreItems.size())
	{
		CloseMoreWnd();
		m_vecChilds.insert(m_vecChilds.end(), m_vecMoreItems.begin(), m_vecMoreItems.end());
	}
}

void CUIToolBar::CloseMoreWnd()
{
	if (m_hMoreWnd == NULL)
		return;

	if (IsWindow(m_hMoreWnd))
		DestroyWindow(m_hMoreWnd);

	m_hMoreWnd = NULL;
}

void CUIToolBar::RecalcLayout(LPRECT lpClipRect)
{
	if (m_vecMoreItems.size())
	{
		CloseMoreWnd();
		m_vecChilds.insert(m_vecChilds.end(), m_vecMoreItems.begin(), m_vecMoreItems.end());
		m_vecMoreItems.clear();
	}

	if (m_rect.left == MAXINT16 || m_vecChilds.size() <= 1)
		return;

	CRect rect(m_rect);

	if (int nIndex = GetMoreIndex())
	{
		auto itm = m_vecChilds.begin() + nIndex;

		for (auto it = m_vecChilds.begin(); it != itm; ++it)
		{
			FRIEND(*it)->CalcRect(rect, lpClipRect);
		}

		for (auto it = itm; it != m_vecChilds.end(); ++it)
		{
			FRIEND(*it)->SetRect(NULL, lpClipRect);
		}

		m_vecMoreItems.insert(m_vecMoreItems.end(), itm, m_vecChilds.end());
		m_vecChilds.erase(itm, m_vecChilds.end());
	}
	else
	{
		FRIEND(m_vecChilds[0])->SetRect(NULL, lpClipRect);

		for (int i = 1; i != m_vecChilds.size(); i++)
		{
			FRIEND(m_vecChilds[i])->CalcRect(rect, lpClipRect);
		}
	}
}

void CUIToolBar::OnMoreBtn()
{
	CRect rect = m_vecChilds[0]->GetWindowRect();

	CUIMoreWnd *pWnd = new CUIMoreWnd(m_moreBg);
	m_hMoreWnd = pWnd->Init(GetRootView()->GetHwnd(), CPoint(rect.left, rect.bottom + 1), m_vecMoreItems);
}

int CUIToolBar::GetMoreIndex() const
{
	int nAllWidth = 0, nIndex = 0;
	int nBtnWidth = m_vecChilds[0]->GetSize().cx;

	for (int i = 1; i != m_vecChilds.size(); i++)
	{
		auto pItem = m_vecChilds[i];

		if (!pItem->IsVisible())
			continue;

		ATLASSERT(FRIEND(pItem)->m_offset.left >> 16);
		ATLASSERT(pItem->GetSize().cx >= 0);

		nAllWidth += (short)FRIEND(pItem)->m_offset.left + pItem->GetSize().cx;

		if (nAllWidth > m_rect.Width())
			return nIndex + 1;

		if (nAllWidth + nBtnWidth <= m_rect.Width())
			nIndex = i;
	}

	return 0;
}

void CUIToolBar::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"moreBg"))
	{
		COLORREF color = 0;
		ATLVERIFY(StrToColor(lpStr, color));
		SetMoreBg(color);
	}
}
