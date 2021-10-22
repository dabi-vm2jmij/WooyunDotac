#include "stdafx.h"
#include "UIMoreTools.h"

namespace
{

class CUIMoreView : public CUIRootView
{
public:
	void AddItems(const vector<CUIView *> &vecItems)
	{
		m_rcReal.SetRect(1, 1, 1, 1);

		for (auto pItem : vecItems)
		{
			((CUIMoreView *)pItem)->m_pParent = this;
			AddChild(pItem);

			CSize size = pItem->GetSize();
			m_rcReal.bottom += size.cy;

			if (m_rcReal.right < size.cx + 1)
				m_rcReal.right = size.cx + 1;

			pItem->SetRect(CRect(1, m_rcReal.bottom - size.cy, size.cx + 1, m_rcReal.bottom), NULL);
		}

		m_rcReal.InflateRect(1, 1);
		m_rect = m_rcReal;
	}

	void ClearItems(CUIView *pToolBar)
	{
		CheckMouseLeave(UIHitTest());

		for (auto pItem : m_vecChilds)
		{
			if (((CUIMoreView *)pItem)->m_pParent == this)
			{
				pItem->SetRect(NULL, NULL);
				((CUIMoreView *)pItem)->m_pParent = pToolBar;
			}
		}

		m_vecChilds.clear();
	}
};

}	// namespace

CUIMoreTools::CUIMoreTools() : m_pToolBar(NULL)
{
	m_rootView.SetBgColor(RGB(60, 123, 212));
}

CUIMoreTools::~CUIMoreTools()
{
}

HWND CUIMoreTools::Popup(const vector<CUIView *> &vecItems, HWND hParent, CPoint point)
{
	m_pToolBar = vecItems[0]->GetParent();
	((CUIMoreView &)m_rootView).AddItems(vecItems);

	ATOM atom = GetWndClassInfo().Register(&m_pfnSuperWindowProc);
	return CWindowImplBaseT::Create(hParent, m_rootView.GetRect() + point, NULL, WS_POPUP | WS_VISIBLE, 0, 0u, atom);
}

void CUIMoreTools::OnKillFocus(HWND hNewWnd)
{
	__super::OnKillFocus(hNewWnd);

	PostMessage(WM_CLOSE);
}

void CUIMoreTools::OnDestroy()
{
	__super::OnDestroy();

	((CUIMoreView &)m_rootView).ClearItems(m_pToolBar);
}
