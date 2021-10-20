#include "stdafx.h"
#include "UIBase.h"

CUIBase::CUIBase(CUIView *pParent) : m_pParent(pParent), m_offset(MAXINT16, MAXINT16, MAXINT16, MAXINT16), m_rect(MAXINT16, 0, MAXINT16, 0), m_colorBg(-1)
	, m_bEnable(true), m_bVisible(true), m_bControl(false), m_bMouseEnter(false)
{
}

CUIBase::~CUIBase()
{
}

static int Int2Offset(int nValue, bool bAlign)
{
	ATLASSERT(nValue > -32768 && nValue <= 32767);

	*((short *)&nValue + 1) = bAlign;
	return nValue;
}

void CUIBase::SetLeft(int nLeft, bool bAlign)
{
	nLeft = Int2Offset(nLeft, bAlign);

	if (m_offset.left != nLeft)
	{
		m_offset.left = nLeft;
		GetParent()->InvalidateLayout();
	}
}

void CUIBase::SetRight(int nRight, bool bAlign)
{
	nRight = Int2Offset(nRight, bAlign);

	if (m_offset.right != nRight)
	{
		m_offset.right = nRight;
		GetParent()->InvalidateLayout();
	}
}

void CUIBase::SetTop(int nTop, bool bAlign)
{
	nTop = Int2Offset(nTop, bAlign);

	if (m_offset.top != nTop)
	{
		m_offset.top = nTop;
		GetParent()->InvalidateLayout();
	}
}

void CUIBase::SetBottom(int nBottom, bool bAlign)
{
	nBottom = Int2Offset(nBottom, bAlign);

	if (m_offset.bottom != nBottom)
	{
		m_offset.bottom = nBottom;
		GetParent()->InvalidateLayout();
	}
}

void CUIBase::SetWidth(int nWidth)
{
	if (m_size.cx != nWidth)
	{
		m_size.cx = nWidth;
		GetParent()->InvalidateLayout();
	}
}

void CUIBase::SetHeight(int nHeight)
{
	if (m_size.cy != nHeight)
	{
		m_size.cy = nHeight;
		GetParent()->InvalidateLayout();
	}
}

void CUIBase::SetSize(CSize size)
{
	if (m_size != size)
	{
		m_size = size;
		GetParent()->InvalidateLayout();
	}
}

void CUIBase::SetRect(LPCRECT lpRect, LPRECT lpClipRect)
{
	CRect rect, rcReal;

	if (IsVisible() && !IsRectEmpty(lpRect))
	{
		rcReal = rect = lpRect;
	}

	MySetRect(rect, rcReal, lpClipRect);
}

// 从给定的 lpRect 中切出自己的区域
void CUIBase::CalcRect(LPRECT lpRect, LPRECT lpClipRect)
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

void CUIBase::CalcLeftRight(long &nLeft, long &nRight, long nOffsetLeft, long nOffsetRight, long nWidth)
{
	bool bNoWidth = nWidth == 0;

	if (nWidth < 0)	// -nWidth 为百分比
		nWidth = (nLeft - nRight) * nWidth / 100;

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

void CUIBase::MySetRect(CRect &rect, CRect &rcReal, LPRECT lpClipRect)
{
	if (m_pParent)
		rcReal &= m_pParent->m_rcReal;

	if (rcReal.IsRectEmpty())
		rcReal = rect = CRect();

	CRect rcOld;
	if (!m_rect.IsRectEmpty())
		rcOld = m_rect;

	if (rcOld != rect || m_rcReal != rcReal)
	{
		if (lpClipRect && (IsControl() || m_colorBg != -1))
		{
			UnionRect(lpClipRect, lpClipRect, m_rcReal);
			UnionRect(lpClipRect, lpClipRect, rcReal);
		}

		m_rect = rect;
		m_rcReal = rcReal;
		OnRectChange(rcOld, lpClipRect);
	}
}

void CUIBase::MyPaint(CUIDC &dc) const
{
	CRect rect(m_rcReal);

	if (dc.GetRealRect(rect))
	{
		// 填充背景色
		if (m_colorBg != -1)
			dc.FillSolidRect(m_rcReal, m_colorBg);

		if (m_rect != m_rcReal || dynamic_cast<const CUIEdit *>(this))
		{
			// 剪裁超出的区域
			HRGN hRgn = CreateRectRgnIndirect(rect);
			SelectClipRgn(dc, hRgn);
			DeleteObject(hRgn);

			OnPaint(dc);
			SelectClipRgn(dc, NULL);
		}
		else
			OnPaint(dc);
	}
}

CUIView *CUIBase::GetParent() const
{
	ATLASSERT(m_pParent);
	return m_pParent;
}

CUIRootView *CUIBase::GetRootView() const
{
	auto pItem = const_cast<CUIBase *>(this);
	for (CUIBase *pParent; pParent = pItem->m_pParent; pItem = pParent);

	auto pRootView = dynamic_cast<CUIRootView *>(pItem);
	ATLASSERT(pRootView);
	return pRootView;
}

CRect CUIBase::GetWindowRect() const
{
	CRect rect(m_rect);
	auto pRootView = GetRootView();
	pRootView->ClientToScreen((LPPOINT)&rect);
	pRootView->ClientToScreen((LPPOINT)&rect + 1);
	return rect;
}

void CUIBase::InvalidateRect(LPCRECT lpRect)
{
	CRect rect(m_rcReal);
	if (lpRect)
		rect &= *lpRect;

	if (rect.IsRectEmpty())
		return;

	GetRootView()->InvalidateRect(rect);
}

void CUIBase::SetEnable(bool bEnable)
{
	if (bEnable)
		bEnable = true;

	if (m_bEnable == bEnable)
		return;

	OnEnable(m_bEnable = bEnable);
}

void CUIBase::SetVisible(bool bVisible)
{
	if (bVisible)
		bVisible = true;

	if (m_bVisible == bVisible)
		return;

	m_bVisible = bVisible;	// 不需要 OnVisible，因为有 OnRectChange
	GetParent()->InvalidateLayout();
}

bool CUIBase::IsRealEnabled() const
{
	for (auto pItem = this; pItem; pItem = pItem->m_pParent)
	{
		if (!pItem->IsEnabled())
			return false;
	}

	return true;
}

bool CUIBase::IsRealVisible() const
{
	return !m_rect.IsRectEmpty();
}

bool CUIBase::IsFullVisible() const
{
	return !m_rect.IsRectEmpty() && m_rect == m_rcReal;
}

bool CUIBase::IsChild(const CUIBase *pItem) const
{
	for (; pItem; pItem = pItem->m_pParent)
	{
		if (pItem == this)
			return true;
	}

	return false;
}

static int Str2Offset(LPCWSTR lpStr)
{
	LPCWSTR lpEnd = lpStr;
	int nValue = wcstoul(lpStr, (LPWSTR *)&lpEnd, 10);

	ATLASSERT(lpEnd != lpStr);
	ATLASSERT(*lpEnd == 0 || *(int *)lpEnd == '*');

	return Int2Offset(nValue, *lpEnd == '*');
}

void CUIBase::OnLoaded(const IUIXmlAttrs &attrs)
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

	if (attrs.GetInt(L"enable", &nValue) && nValue == 0)
		SetEnable(false);

	if (attrs.GetInt(L"visible", &nValue) && nValue == 0)
		SetVisible(false);

	if (lpStr = attrs.GetStr(L"bgColor"))
	{
		COLORREF color = 0;
		ATLVERIFY(StrToColor(lpStr, color));
		SetBgColor(color);
	}
}
