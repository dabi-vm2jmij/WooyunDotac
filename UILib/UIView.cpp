#include "stdafx.h"
#include "UIView.h"
#include "UILibApp.h"

CUIView::CUIView(CUIView *pParent) : m_pParent(pParent), m_offset(MAXINT16, MAXINT16, MAXINT16, MAXINT16), m_rect(MAXINT16, 0, MAXINT16, 0), m_bgColor(-1)
	, m_bEnable(true), m_bVisible(true), m_bMouseEnter(false), m_bNeedLayout(false)
{
}

CUIView::~CUIView()
{
	while (m_vecChilds.size())
	{
		auto pItem = m_vecChilds.back();
		m_vecChilds.pop_back();
		delete pItem;
	}
}

static int Int2Offset(int nValue, bool bAlign)
{
	ATLASSERT(nValue > -32768 && nValue <= 32767);

	*((short *)&nValue + 1) = bAlign;
	return nValue;
}

void CUIView::SetLeft(int nLeft, bool bAlign)
{
	nLeft = Int2Offset(nLeft, bAlign);

	if (m_offset.left != nLeft)
	{
		m_offset.left = nLeft;
		GetParent()->InvalidateLayout();
	}
}

void CUIView::SetRight(int nRight, bool bAlign)
{
	nRight = Int2Offset(nRight, bAlign);

	if (m_offset.right != nRight)
	{
		m_offset.right = nRight;
		GetParent()->InvalidateLayout();
	}
}

void CUIView::SetTop(int nTop, bool bAlign)
{
	nTop = Int2Offset(nTop, bAlign);

	if (m_offset.top != nTop)
	{
		m_offset.top = nTop;
		GetParent()->InvalidateLayout();
	}
}

void CUIView::SetBottom(int nBottom, bool bAlign)
{
	nBottom = Int2Offset(nBottom, bAlign);

	if (m_offset.bottom != nBottom)
	{
		m_offset.bottom = nBottom;
		GetParent()->InvalidateLayout();
	}
}

void CUIView::SetWidth(int nWidth)
{
	if (m_size.cx != nWidth)
	{
		m_size.cx = nWidth;
		GetParent()->InvalidateLayout();
	}
}

void CUIView::SetHeight(int nHeight)
{
	if (m_size.cy != nHeight)
	{
		m_size.cy = nHeight;
		GetParent()->InvalidateLayout();
	}
}

void CUIView::SetSize(CSize size)
{
	if (m_size != size)
	{
		m_size = size;
		GetParent()->InvalidateLayout();
	}
}

void CUIView::SetRect(LPCRECT lpRect, LPRECT lpClipRect)
{
	CRect rect, rcReal;

	if (IsVisible() && !IsRectEmpty(lpRect))
		rcReal = rect = lpRect;

	MySetRect(rect, rcReal, lpClipRect);
}

// 从给定的 lpRect 中切出自己的区域
void CUIView::CalcRect(LPRECT lpRect, LPRECT lpClipRect)
{
	CRect rect, rcReal;

	if (IsVisible() && !IsRectEmpty(lpRect))
	{
		rcReal = rect = lpRect;
		CalcLeftRight(rect.left, rect.right, m_offset.left, m_offset.right, m_size.cx);
		CalcLeftRight(rect.top, rect.bottom, m_offset.top, m_offset.bottom, m_size.cy);
		rcReal &= rect;

		// 返回切剩余的区域
		if (m_offset.left >> 16)
			lpRect->left = rect.right;
		else if (m_offset.right >> 16)
			lpRect->right = rect.left;

		if (m_offset.top >> 16)
			lpRect->top = rect.bottom;
		else if (m_offset.bottom >> 16)
			lpRect->bottom = rect.top;
	}

	MySetRect(rect, rcReal, lpClipRect);
}

void CUIView::CalcLeftRight(long &nLeft, long &nRight, long nOffsetLeft, long nOffsetRight, long nWidth)
{
	bool bNoWidth = nWidth == 0;

	if (nWidth < 0)	// -nWidth 为百分比
		nWidth = (nRight - nLeft) * -nWidth / 100;

	// 算左、右偏移
	if (nOffsetLeft != MAXINT16)
		nLeft += (short)nOffsetLeft;

	if (nOffsetRight != MAXINT16)
		nRight -= (short)nOffsetRight;

	if (nOffsetLeft != MAXINT16 && nOffsetRight != MAXINT16 || bNoWidth)
		return;

	if (nOffsetLeft != MAXINT16)
	{
		// 指定宽、左，算出右
		nRight = nLeft + nWidth;
	}
	else if (nOffsetRight != MAXINT16)
	{
		// 指定宽、右，算出左
		nLeft = nRight - nWidth;
	}
	else
	{
		// 只指定宽，左右居中
		nLeft = (nLeft + nRight - nWidth) / 2;
		nRight = nLeft + nWidth;
	}
}

void CUIView::MySetRect(CRect &rect, CRect &rcReal, LPRECT lpClipRect)
{
	if (rcReal.IsRectEmpty() || m_pParent && !rcReal.IntersectRect(rcReal, m_pParent->m_rcReal))
		rcReal = rect = CRect();

	CRect rcOld;
	if (!m_rect.IsRectEmpty())
		rcOld = m_rect;

	if (rcOld != rect || m_rcReal != rcReal)
	{
		if (lpClipRect && IsPaint())
		{
			UnionRect(lpClipRect, lpClipRect, m_rcReal);
			UnionRect(lpClipRect, lpClipRect, rcReal);
		}

		m_rect = rect;
		m_rcReal = rcReal;
		OnRectChange(rcOld, lpClipRect);

		if (rect.IsRectEmpty())
			OnVisible(false);
		else if (rcOld.IsRectEmpty())
			OnVisible(true);
	}
}

CRect CUIView::GetWindowRect() const
{
	CRect rect(m_rect);
	auto pRootView = GetRootView();
	pRootView->ClientToScreen((LPPOINT)&rect);
	pRootView->ClientToScreen((LPPOINT)&rect + 1);
	return rect;
}

void CUIView::InvalidateRect(LPCRECT lpRect)
{
	CRect rect(m_rcReal);
	if (rect.IsRectEmpty() || lpRect && !rect.IntersectRect(rect, lpRect))
		return;

	GetRootView()->InvalidateRect(rect);
}

void CUIView::InvalidateLayout()
{
	if (m_bNeedLayout || m_rect.IsRectEmpty() || m_vecChilds.empty())
		return;

	// 利用 PostMessage 来延迟 RecalcLayout
	m_bNeedLayout = true;
	g_theApp.DelayLayout(GetRootView());
}

void CUIView::SetBgColor(COLORREF color)
{
	if (m_bgColor != color)
	{
		m_bgColor = color;
		InvalidateRect();
	}
}

void CUIView::SetBgImage(const CImagex &imagex)
{
	if (m_bgImagex != imagex)
	{
		m_bgImagex = imagex;
		InvalidateRect();
	}
}

void CUIView::SetEnabled(bool bEnable)
{
	if (bEnable)
		bEnable = true;

	if (m_bEnable != bEnable)
		OnEnable(m_bEnable = bEnable);
}

void CUIView::SetVisible(bool bVisible)
{
	if (bVisible)
		bVisible = true;

	if (m_bVisible != bVisible)
	{
		m_bVisible = bVisible;
		GetParent()->InvalidateLayout();
	}
}

bool CUIView::IsRealEnabled() const
{
	for (auto pItem = this; pItem; pItem = pItem->m_pParent)
	{
		if (!pItem->IsEnabled())
			return false;
	}

	return true;
}

bool CUIView::IsRealVisible() const
{
	return !m_rect.IsRectEmpty();
}

bool CUIView::IsFullVisible() const
{
	return !m_rect.IsRectEmpty() && m_rect == m_rcReal;
}

bool CUIView::IsChild(const CUIView *pItem) const
{
	for (; pItem; pItem = pItem->m_pParent)
	{
		if (pItem == this)
			return true;
	}

	return false;
}

CUIView *CUIView::GetParent() const
{
	ATLASSERT(m_pParent);
	return m_pParent;
}

CUIRootView *CUIView::GetRootView() const
{
	auto pItem = const_cast<CUIView *>(this);
	for (CUIView *pParent; pParent = pItem->m_pParent; pItem = pParent);

	ATLASSERT((CUIRootView *)pItem == dynamic_cast<CUIRootView *>(pItem));
	return (CUIRootView *)pItem;
}

bool CUIView::OnHitTest(UIHitTest &hitTest)
{
	bool bRet = false;
	UINT nIndex = hitTest.nCount;

	// 按照逆序：后画的控件先处理消息
	for (auto it = m_vecChilds.rbegin(); it != m_vecChilds.rend(); ++it)
	{
		auto pItem = *it;

		if (pItem->m_rcReal.PtInRect(hitTest.point) && pItem->OnHitTest(hitTest))
		{
			bRet = true;
			break;
		}
	}

	if (!IsEnabled())
	{
		for (int i = nIndex; i != hitTest.nCount; i++)
			hitTest.items[i].bEnable = false;
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
			auto pItem = *it;

			if (pItem->IsEnabled() && pItem->m_rcReal.PtInRect(point) && pItem->OnMessage(uMsg, wParam, lParam))
				return true;
		}
	}

	return false;
}

void CUIView::DoPaint(CUIDC &dc) const
{
	if (m_rect.IsRectEmpty() || !RectVisible(dc, m_rcReal))
		return;

	if (int nPaint = IsPaint())
	{
		if (nPaint > 1 || m_rect != m_rcReal)
		{
			// 剪裁超出的区域
			int nSaved = SaveDC(dc);
			IntersectClipRect(dc, m_rcReal.left, m_rcReal.top, m_rcReal.right, m_rcReal.bottom);
			OnPaint(dc);
			RestoreDC(dc, nSaved);
		}
		else
			OnPaint(dc);
	}

	PaintChilds(dc);
}

void CUIView::OnPaint(CUIDC &dc) const
{
	if (m_bgColor != -1)
		dc.FillSolidRect(m_rect, m_bgColor);

	if (m_bgImagex)
		m_bgImagex.Scale9Draw(dc, m_rect);
}

void CUIView::PaintChilds(CUIDC &dc) const
{
	for (auto pItem : m_vecChilds)
		pItem->DoPaint(dc);
}

void CUIView::OnEnable(bool bEnable)
{
	for (auto pItem : m_vecChilds)
		pItem->OnEnable(bEnable);
}

void CUIView::OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	m_bNeedLayout = false;
	RecalcLayout(lpClipRect);
}

// 检查所有需要重新布局的控件
void CUIView::OnNeedLayout(LPRECT lpClipRect)
{
	if (m_bNeedLayout)
	{
		m_bNeedLayout = false;
		RecalcLayout(lpClipRect);
	}

	if (m_rect.IsRectEmpty())
		return;

	for (auto pItem : m_vecChilds)
		pItem->OnNeedLayout(lpClipRect);
}

// 将自己的区域分给子控件
void CUIView::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	CRect rect(m_rect);

	for (auto pItem : m_vecChilds)
		pItem->CalcRect(rect, lpClipRect);
}

void CUIView::DeleteChild(CUIView *pItem)
{
	for (auto it = m_vecChilds.begin(); it != m_vecChilds.end(); ++it)
	{
		if (*it == pItem)
		{
			m_vecChilds.erase(it);
			InvalidateLayout();

			CRect rect;
			pItem->SetRect(NULL, rect);

			auto pRootView = GetRootView();
			pRootView->InvalidateRect(rect);
			pRootView->DoMouseMove();	// 先 OnMouseLeave 再 delete
			delete pItem;
			break;
		}
	}
}

CUIView *CUIView::Search(LPCWSTR lpszId, UINT nDepth) const
{
	if (lpszId == NULL || *lpszId == 0)
	{
		ATLASSERT(0);
		return NULL;
	}

	return this ? MySearch(lpszId, nDepth) : NULL;
}

CUIView *CUIView::MySearch(LPCWSTR lpszId, UINT nDepth) const
{
	if (m_strId.size() && _wcsicmp(m_strId.c_str(), lpszId) == 0)
		return const_cast<CUIView *>(this);

	if (nDepth == 0)
		return NULL;

	for (auto pItem : m_vecChilds)
	{
		if (auto pRet = pItem->MySearch(lpszId, nDepth - 1))
			return pRet;
	}

	return NULL;
}

CUIView *CUIView::OnCustomUI(LPCWSTR lpName, CUIView *pParent)
{
	return m_pParent->OnCustomUI(lpName, pParent);
}

void CUIView::OnChildMoving(CUIView *pItem, CPoint point)
{
	if (m_pParent)
		m_pParent->OnChildMoving(pItem, point);
}

void CUIView::OnChildMoved(CUIView *pItem, CPoint point)
{
	if (m_pParent)
		m_pParent->OnChildMoved(pItem, point);
}

CUIView *CUIView::AddChild(CUIView *pItem)
{
	ATLASSERT(pItem->m_pParent == this);
	pItem->m_pParent = this;
	MyAddChild(pItem);
	return pItem;
}

CUIView *CUIView::AddView()
{
	auto pItem = new CUIView(this);
	MyAddChild(pItem);
	return pItem;
}

CUIButton *CUIView::AddButton(LPCWSTR lpFileName)
{
	auto pItem = new CUIButton(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIButtonEx *CUIView::AddButtonEx(LPCWSTR lpFileName)
{
	auto pItem = new CUIButtonEx(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIMenuButton *CUIView::AddMenuButton(LPCWSTR lpFileName)
{
	auto pItem = new CUIMenuButton(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUICheckButton *CUIView::AddCheckButton(LPCWSTR lpFileName)
{
	auto pItem = new CUICheckButton(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIRadioButton *CUIView::AddRadioButton(LPCWSTR lpFileName)
{
	auto pItem = new CUIRadioButton(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIComboButton *CUIView::AddComboButton()
{
	auto pItem = new CUIComboButton(this);
	MyAddChild(pItem);
	return pItem;
}

CUIStateButton *CUIView::AddStateButton()
{
	auto pItem = new CUIStateButton(this);
	MyAddChild(pItem);
	return pItem;
}

CUICheckBox *CUIView::AddCheckBox(LPCWSTR lpFileName)
{
	auto pItem = new CUICheckBox(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIRadioBox *CUIView::AddRadioBox(LPCWSTR lpFileName)
{
	auto pItem = new CUIRadioBox(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIAnimation *CUIView::AddAnimation(LPCWSTR lpFileName)
{
	auto pItem = new CUIAnimation(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIEdit *CUIView::AddEdit()
{
	auto pItem = new CUIEdit(this);
	MyAddChild(pItem);
	return pItem;
}

CUIGif *CUIView::AddGif(LPCWSTR lpFileName)
{
	auto pItem = new CUIGif(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIImage *CUIView::AddImage(LPCWSTR lpFileName)
{
	auto pItem = new CUIImage(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUILabel *CUIView::AddLabel()
{
	auto pItem = new CUILabel(this);
	MyAddChild(pItem);
	return pItem;
}

CUILine *CUIView::AddLine()
{
	auto pItem = new CUILine(this);
	MyAddChild(pItem);
	return pItem;
}

CUIProgress *CUIView::AddProgress(LPCWSTR lpFileName)
{
	auto pItem = new CUIProgress(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIPageCtrl *CUIView::AddPageCtrl(LPCWSTR lpFileName)
{
	auto pItem = new CUIPageCtrl(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

CUICarousel *CUIView::AddCarousel()
{
	auto pItem = new CUICarousel(this);
	MyAddChild(pItem);
	return pItem;
}

CUIScrollView *CUIView::AddScrollView()
{
	auto pItem = new CUIScrollView(this);
	MyAddChild(pItem);
	return pItem;
}

CUISlider *CUIView::AddSlider(LPCWSTR lpFileName, LPCWSTR lpBgFileName)
{
	auto pItem = new CUISlider(this, lpFileName, lpBgFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIScrollBar *CUIView::AddScrollBar(LPCWSTR lpFileName, LPCWSTR lpBgFileName)
{
	auto pItem = new CUIScrollBar(this, lpFileName, lpBgFileName);
	MyAddChild(pItem);
	return pItem;
}

CUIToolBar *CUIView::AddToolBar()
{
	auto pItem = new CUIToolBar(this);
	MyAddChild(pItem);
	return pItem;
}

CUIWebTabBar *CUIView::AddWebTabBar(LPCWSTR lpFileName)
{
	auto pItem = new CUIWebTabBar(this, lpFileName);
	MyAddChild(pItem);
	return pItem;
}

void CUIView::MyAddChild(CUIView *pItem)
{
	m_vecChilds.reserve(4);
	m_vecChilds.push_back(pItem);
	InvalidateLayout();
}

static int Str2Offset(LPCWSTR lpStr)
{
	LPCWSTR lpEnd = lpStr;
	int nValue = wcstoul(lpStr, (LPWSTR *)&lpEnd, 10);

	ATLASSERT(lpEnd != lpStr);
	ATLASSERT(*lpEnd == 0 || *(int *)lpEnd == '*');

	return Int2Offset(nValue, *lpEnd == '*');
}

void CUIView::OnLoad(const IUIXmlAttrs &attrs)
{
	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"id"))
		SetId(lpStr);

	if (lpStr = attrs.GetStr(L"left"))
		m_offset.left = Str2Offset(lpStr);

	if (lpStr = attrs.GetStr(L"right"))
		m_offset.right = Str2Offset(lpStr);

	if (lpStr = attrs.GetStr(L"top"))
		m_offset.top = Str2Offset(lpStr);

	if (lpStr = attrs.GetStr(L"bottom"))
		m_offset.bottom = Str2Offset(lpStr);

	int nValue;
	if (attrs.GetInt(L"width", &nValue))
	{
		m_size.cx = nValue;
		ATLASSERT(m_offset.left == MAXINT16 || m_offset.right == MAXINT16);
	}

	if (attrs.GetInt(L"height", &nValue))
	{
		m_size.cy = nValue;
		ATLASSERT(m_offset.top == MAXINT16 || m_offset.bottom == MAXINT16);
	}

	if (attrs.GetInt(L"enabled", &nValue) && nValue == 0)
		SetEnabled(false);

	if (attrs.GetInt(L"visible", &nValue) && nValue == 0)
		SetVisible(false);

	if (lpStr = attrs.GetStr(L"bgColor"))
	{
		COLORREF color = 0;
		ATLVERIFY(StrToColor(lpStr, color));
		SetBgColor(color);
	}

	if (LPCWSTR lpFileName = attrs.GetStr(L"bgImage"))
		SetBgImage(GetImage(lpFileName));
}
