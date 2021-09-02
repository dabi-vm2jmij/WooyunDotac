#include "stdafx.h"
#include "UIView.h"

CUIView::CUIView(CUIView *pParent) : CUIBase(pParent), m_bNeedLayout(false)
{
	m_size.SetSize(-1, -1);	// 自适应
}

CUIView::~CUIView()
{
	for (auto pItem : m_vecChilds)
		delete pItem;
}

bool CUIView::OnHitTest(UIHitTest &hitTest)
{
	bool bRet = false;
	UINT nIndex = hitTest.nCount;

	// 按照逆序：后画的控件先处理消息
	for (auto it = m_vecChilds.rbegin(); it != m_vecChilds.rend(); ++it)
	{
		CUIBase *pItem = *it;

		if (pItem->m_rcReal.PtInRect(hitTest.point))
		{
			if (pItem->OnHitTest(hitTest))
			{
				bRet = true;
				break;
			}
		}
	}

	if (!IsEnabled())
	{
		for (int i = nIndex; i != hitTest.nCount; i++)
		{
			hitTest.items[i].bEnabled = false;
		}
	}

	return bRet;
}

bool CUIView::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOUSEWHEEL || uMsg == WM_CONTEXTMENU)
	{
		CPoint point(lParam);

		for (auto it = m_vecChilds.rbegin(); it != m_vecChilds.rend(); ++it)
		{
			CUIBase *pItem = *it;

			if (pItem->IsEnabled() && pItem->m_rcReal.PtInRect(point))
			{
				if (pItem->OnMessage(uMsg, wParam, lParam))
					return true;
			}
		}
	}

	return false;
}

void CUIView::OnPaint(CUIDC &dc) const
{
	CUIBase *pCapture = GetRootView()->m_pCapture;
	bool bFound = false;

	for (auto pItem : m_vecChilds)
	{
		if (pItem == pCapture)
			bFound = true;
		else
			pItem->DoPaint(dc);
	}

	// 最后画，保证在上
	if (bFound)
		pCapture->DoPaint(dc);
}

void CUIView::OnEnabled(bool bEnabled)
{
	for (auto pItem : m_vecChilds)
		pItem->OnEnabled(bEnabled);
}

void CUIView::OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	m_bNeedLayout = false;
	RecalcLayout(lpClipRect);
}

// 检查所有需要重新布局的控件
void CUIView::IsNeedLayout(LPRECT lpClipRect)
{
	if (m_bNeedLayout)
	{
		m_bNeedLayout = false;
		RecalcLayout(lpClipRect);
	}

	if (m_rect.IsRectEmpty())
		return;

	for (auto pItem : m_vecChilds)
	{
		pItem->IsNeedLayout(lpClipRect);
	}
}

// 将自己的区域分给子控件
void CUIView::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	CRect rect(m_rect);

	for (auto pItem : m_vecChilds)
	{
		pItem->CalcRect(rect, lpClipRect);
	}
}

void CUIView::InvalidateRect(LPCRECT lpRect)
{
	CRect rect(m_rcReal);
	if (lpRect)
		rect &= *lpRect;

	if (rect.IsRectEmpty())
		return;

	GetRootView()->DoInvalidateRect(rect);
}

void CUIView::InvalidateLayout()
{
	if (m_bNeedLayout || m_rect.IsRectEmpty())
		return;

	m_bNeedLayout = true;
	GetRootView()->DoInvalidateLayout();
}

void CUIView::RemoveChild(CUIBase *pItem)
{
	for (auto it = m_vecChilds.begin(); it != m_vecChilds.end(); ++it)
	{
		if (*it == pItem)
		{
			CRect rect;
			pItem->CalcRect(NULL, rect);
			InvalidateRect(rect);

			m_vecChilds.erase(it);
			delete pItem;
			InvalidateLayout();
			break;
		}
	}
}

CUIBase *CUIView::GetChild(UINT nIndex) const
{
	return nIndex < m_vecChilds.size() ? m_vecChilds[nIndex] : NULL;
}

CUIBase *CUIView::AddChild(CUIBase *pItem)
{
	ATLASSERT(pItem->m_pParent == NULL || pItem->m_pParent == this);
	pItem->m_pParent = this;
	PushBackChild(pItem);
	return pItem;
}

CUIBlank *CUIView::AddBlank()
{
	CUIBlank *pItem = new CUIBlank(this);
	PushBackChild(pItem);
	return pItem;
}

CUIView *CUIView::AddView()
{
	CUIView *pItem = new CUIView(this);
	PushBackChild(pItem);
	return pItem;
}

CUIButton *CUIView::AddButton(LPCWSTR lpFileName)
{
	CUIButton *pItem = new CUIButton(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIButtonEx *CUIView::AddButtonEx(LPCWSTR lpFileName)
{
	CUIButtonEx *pItem = new CUIButtonEx(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIMenuButton *CUIView::AddMenuButton(LPCWSTR lpFileName)
{
	CUIMenuButton *pItem = new CUIMenuButton(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUICheckButton *CUIView::AddCheckButton(LPCWSTR lpFileName)
{
	CUICheckButton *pItem = new CUICheckButton(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIRadioButton *CUIView::AddRadioButton(LPCWSTR lpFileName)
{
	CUIRadioButton *pItem = new CUIRadioButton(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIComboButton *CUIView::AddComboButton()
{
	CUIComboButton *pItem = new CUIComboButton(this);
	PushBackChild(pItem);
	return pItem;
}

CUIStateButton *CUIView::AddStateButton()
{
	CUIStateButton *pItem = new CUIStateButton(this);
	PushBackChild(pItem);
	return pItem;
}

CUICheckBox *CUIView::AddCheckBox(LPCWSTR lpFileName)
{
	CUICheckBox *pItem = new CUICheckBox(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIRadioBox *CUIView::AddRadioBox(LPCWSTR lpFileName)
{
	CUIRadioBox *pItem = new CUIRadioBox(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIAnimation *CUIView::AddAnimation(LPCWSTR lpFileName)
{
	CUIAnimation *pItem = new CUIAnimation(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIEdit *CUIView::AddEdit()
{
	CUIEdit *pItem = new CUIEdit(this);
	PushBackChild(pItem);
	return pItem;
}

CUIGif *CUIView::AddGif(LPCWSTR lpFileName)
{
	CUIGif *pItem = new CUIGif(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIImage *CUIView::AddImage(LPCWSTR lpFileName)
{
	CUIImage *pItem = new CUIImage(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUILabel *CUIView::AddLabel()
{
	CUILabel *pItem = new CUILabel(this);
	PushBackChild(pItem);
	return pItem;
}

CUILine *CUIView::AddLine()
{
	CUILine *pItem = new CUILine(this);
	PushBackChild(pItem);
	return pItem;
}

CUINotice *CUIView::AddNotice()
{
	CUINotice *pItem = new CUINotice(this);
	PushBackChild(pItem);
	return pItem;
}

CUIProgress *CUIView::AddProgress(LPCWSTR lpFileName)
{
	CUIProgress *pItem = new CUIProgress(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIPageView *CUIView::AddPageView(LPCWSTR lpFileName)
{
	CUIPageView *pItem = new CUIPageView(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIScrollView *CUIView::AddScrollView()
{
	CUIScrollView *pItem = new CUIScrollView(this);
	PushBackChild(pItem);
	return pItem;
}

CUISlider *CUIView::AddSlider(LPCWSTR lpFileName, LPCWSTR lpFileNameBg)
{
	CUISlider *pItem = new CUISlider(this, lpFileName, lpFileNameBg);
	PushBackChild(pItem);
	return pItem;
}

CUIToolBar *CUIView::AddToolBar(LPCWSTR lpFileName)
{
	CUIToolBar *pItem = new CUIToolBar(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

CUIVScroll *CUIView::AddVScroll(LPCWSTR lpFileName, LPCWSTR lpFileNameBg)
{
	CUIVScroll *pItem = new CUIVScroll(this, lpFileName, lpFileNameBg);
	PushBackChild(pItem);
	return pItem;
}

CUIWebTabBar *CUIView::AddWebTabBar(LPCWSTR lpFileName)
{
	CUIWebTabBar *pItem = new CUIWebTabBar(this, lpFileName);
	PushBackChild(pItem);
	return pItem;
}

void CUIView::PushBackChild(CUIBase *pItem)
{
	m_vecChilds.reserve(4);
	m_vecChilds.push_back(pItem);
	InvalidateLayout();
}

void CUIView::OnRadioCheck(const CUIRadioBox *pItem)
{
	for (auto pIt : m_vecChilds)
	{
		if (CUIRadioBox *pRadio = dynamic_cast<CUIRadioBox *>(pIt))
		{
			if (pRadio != pItem)
				pRadio->SetCheck(false);
		}
	}
}

void CUIView::OnRadioCheck(const CUIRadioButton *pItem)
{
	for (auto pIt : m_vecChilds)
	{
		if (CUIRadioButton *pRadio = dynamic_cast<CUIRadioButton *>(pIt))
		{
			if (pRadio != pItem)
				pRadio->SetCheck(false);
		}
	}
}
