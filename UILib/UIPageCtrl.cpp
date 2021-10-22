#include "stdafx.h"
#include "UIPageCtrl.h"

CUIPageCtrl::CUIPageCtrl(CUIView *pParent, LPCWSTR lpFileName) : CUIView(pParent), m_nSpacing(0)
{
	if (lpFileName)
		m_strTabImage = lpFileName;
}

CUIPageCtrl::~CUIPageCtrl()
{
}

void CUIPageCtrl::SetCount(UINT nCount)
{
	if (m_vecChilds.size() == nCount)
		return;

	if (m_vecChilds.size() < nCount)
	{
		for (int i = m_vecChilds.size(); i != nCount; i++)
		{
			CUIRadioBox *pRadio = AddRadioBox(m_strTabImage.c_str());
			pRadio->SetCursor(LoadCursor(NULL, IDC_HAND));

			pRadio->BindClick([this, i]
			{
				if (m_fnOnChange)
					m_fnOnChange(i);
			});
		}
	}
	else
	{
		vector<CUIView *> vecChilds(m_vecChilds.begin() + nCount, m_vecChilds.end());
		m_vecChilds.erase(m_vecChilds.begin() + nCount, m_vecChilds.end());
		InvalidateLayout();

		CRect rect;
		for (auto pItem : vecChilds)
			pItem->SetRect(NULL, rect);

		auto pRootView = GetRootView();
		pRootView->InvalidateRect(rect);
		pRootView->DoMouseMove();

		for (auto pItem : vecChilds)
			delete pItem;
	}

	if (GetIndex() == -1)
		SetIndex(0);
}

void CUIPageCtrl::SetIndex(UINT nIndex)
{
	if (nIndex < m_vecChilds.size())
	{
		auto pRadio = (CUIRadioBox *)m_vecChilds[nIndex];

		if (!pRadio->IsChecked())
		{
			pRadio->SetCheck(true);

			if (m_fnOnChange)
				m_fnOnChange(nIndex);
		}
	}
}

UINT CUIPageCtrl::GetIndex() const
{
	for (int i = 0; i != m_vecChilds.size(); i++)
	{
		auto pRadio = (CUIRadioBox *)m_vecChilds[i];

		if (pRadio->IsChecked())
			return i;
	}

	return -1;
}

void CUIPageCtrl::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	int nWidth = m_vecChilds[0]->GetSize().cx;
	CRect rect(m_rect);
	rect.left = (int)(rect.left + rect.right - (nWidth + m_nSpacing) * m_vecChilds.size() + m_nSpacing) / 2;

	for (auto pItem : m_vecChilds)
	{
		rect.right = rect.left + nWidth;
		pItem->SetRect(rect, lpClipRect);
		rect.left = rect.right + m_nSpacing;
	}
}

void CUIPageCtrl::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"spacing", &nValue))
		SetSpacing(nValue);
}
