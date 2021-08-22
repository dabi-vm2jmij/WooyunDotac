#include "stdafx.h"
#include "UIToolBar.h"
#include "UIMoreWnd.h"

CUIToolBar::CUIToolBar(CUIView *pParent, LPCWSTR lpFileName) : CUIView(pParent), m_hMoreWnd(NULL)
{
	CUIButton *pButton = AddButton(lpFileName);
	pButton->SetRight(0, true);
	pButton->SetToolTip(L"更多工具");
	pButton->OnClick([this]{ OnMoreBtn(); });
}

CUIToolBar::~CUIToolBar()
{
	for (auto pItem : m_vecMoreItems)
		delete pItem;
}

void CUIToolBar::SetMoreBg(LPCWSTR lpFileName)
{
	m_strImageBg = lpFileName;
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

		for (auto pItem : m_vecMoreItems)
		{
			FRIEND(pItem)->m_pParent = this;
			AddChild(pItem);
		}

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
			FRIEND(*it)->CalcRect(NULL, lpClipRect);
		}

		m_vecMoreItems.insert(m_vecMoreItems.end(), itm, m_vecChilds.end());
		m_vecChilds.erase(itm, m_vecChilds.end());
	}
	else
	{
		FRIEND(m_vecChilds[0])->CalcRect(NULL, lpClipRect);

		for (int i = 1; i != m_vecChilds.size(); i++)
		{
			FRIEND(m_vecChilds[i])->CalcRect(rect, lpClipRect);
		}
	}
}

int CUIToolBar::GetMoreIndex() const
{
	CSize sizeBtn;
	m_vecChilds[0]->GetSize(&sizeBtn);

	int nWidth = 0, nIndex = 0;

	for (int i = 1; i != m_vecChilds.size(); i++)
	{
		CUIBase *pItem = m_vecChilds[i];

		if (!pItem->IsVisible())
			continue;

		CSize size;
		pItem->GetSize(&size);

		ATLASSERT(FRIEND(pItem)->m_offset.left >> 16);
		ATLASSERT(size.cx >= 0);

		nWidth += (short)FRIEND(pItem)->m_offset.left + size.cx;

		if (nWidth > m_rect.Width())
			return nIndex + 1;

		if (nWidth + sizeBtn.cx <= m_rect.Width())
			nIndex = i;
	}

	return 0;
}

void CUIToolBar::OnMoreBtn()
{
	CRect rect;
	m_vecChilds[0]->GetWindowRect(rect);

	CUIMoreWnd *pWnd = new CUIMoreWnd(m_strImageBg.c_str());
	m_hMoreWnd = pWnd->Init(GetRootView()->GetHwnd(), CPoint(rect.left, rect.bottom + 1), m_vecMoreItems);
}

void CUIToolBar::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"imageBg"))
		SetMoreBg(lpStr);
}
