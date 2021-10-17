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

	CRect rect;

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
		while (m_vecChilds.size() != nCount)
		{
			auto pItem = m_vecChilds.back();
			m_vecChilds.pop_back();

			FRIEND(pItem)->CalcRect(NULL, rect);
			delete pItem;
		}
	}

	RecalcLayout(rect);
	InvalidateRect(rect);

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

	CSize size = m_vecChilds[0]->GetSize();

	CRect rect(m_rect);
	rect.left += (int)(m_rect.Width() - (size.cx + m_nSpace) * m_vecChilds.size() + m_nSpace) / 2;
	rect.right = rect.left + size.cx;

	for (auto pItem : m_vecChilds)
	{
		FRIEND(pItem)->CalcRect(CRect(rect), lpClipRect);
		rect.MoveToX(rect.right + m_nSpace);
	}
}

void CUIPageView::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"space", &nValue))
		SetSpace(nValue);
}
