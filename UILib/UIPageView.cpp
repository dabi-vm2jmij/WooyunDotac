#include "stdafx.h"
#include "UIPageView.h"

CUIPageView::CUIPageView(CUIView *pParent, LPCWSTR lpFileName) : CUIView(pParent), m_nSpace(0)
{
	if (lpFileName)
		m_strImage = lpFileName;
}

CUIPageView::~CUIPageView()
{
}

void CUIPageView::SetCount(UINT nCount)
{
	if (m_vecChilds.size() == nCount)
		return;

	if (m_vecChilds.size() < nCount)
	{
		for (int i = m_vecChilds.size(); i != nCount; i++)
		{
			CUIRadioBox *pRadio = NULL;

			if (m_fnNewRadio)
			{
				pRadio = m_fnNewRadio(i);
				ATLASSERT(pRadio);

				if (pRadio)
					AddChild(pRadio);
			}

			if (pRadio == NULL)
			{
				pRadio = AddRadioBox(m_strImage.c_str());
				pRadio->SetCursor(LoadCursor(NULL, IDC_HAND));
			}

			pRadio->BindClick([this, i]
			{
				if (m_fnOnChange)
					m_fnOnChange(i);
			});
		}
	}
	else
	{
		vector<CUIView *> vecChilds;
		vecChilds.insert(vecChilds.end(), m_vecChilds.begin() + nCount, m_vecChilds.end());
		m_vecChilds.erase(m_vecChilds.begin() + nCount, m_vecChilds.end());

		CRect rect;
		RecalcLayout(rect);

		for (auto pItem : vecChilds)
			FRIEND(pItem)->SetRect(NULL, rect);

		InvalidateRect(rect);
		GetRootView()->RaiseMouseMove();

		for (auto pItem : vecChilds)
			delete pItem;
	}

	if (GetIndex() == -1)
		SetIndex(nCount - 1);
}

void CUIPageView::SetIndex(UINT nIndex)
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

UINT CUIPageView::GetIndex() const
{
	for (int i = 0; i != m_vecChilds.size(); i++)
	{
		auto pRadio = (CUIRadioBox *)m_vecChilds[i];

		if (pRadio->IsChecked())
			return i;
	}

	return -1;
}

void CUIPageView::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	int nWidth = m_vecChilds[0]->GetSize().cx;
	CRect rect(m_rect);
	rect.left = (int)(rect.left + rect.right - (nWidth + m_nSpace) * m_vecChilds.size() + m_nSpace) / 2;

	for (auto pItem : m_vecChilds)
	{
		rect.right = rect.left + nWidth;
		FRIEND(pItem)->SetRect(rect, lpClipRect);
		rect.left = rect.right + m_nSpace;
	}
}

void CUIPageView::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"space", &nValue))
		SetSpace(nValue);
}
