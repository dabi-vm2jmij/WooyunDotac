#include "stdafx.h"
#include "UIBase.h"

CUIBase::CUIBase(CUIView *pParent) : m_pParent(pParent), m_offset(MAXINT16, MAXINT16, MAXINT16, MAXINT16), m_rect(MAXINT16, 0, MAXINT16, 0)
	, m_bControl(false), m_bEnabled(true), m_bVisible(true), m_bKeepEnter(false), m_ppEnter(NULL)
{
}

CUIBase::~CUIBase()
{
	if (m_ppEnter)
		*m_ppEnter = NULL;
}

static int Int2Offset(int nValue, bool bClip)
{
	ATLASSERT(nValue > -32768 && nValue <= 32767);

	*((short *)&nValue + 1) = bClip;
	return nValue;
}

void CUIBase::SetLeft(int nLeft, bool bClip)
{
	m_offset.left = Int2Offset(nLeft, bClip);
}

void CUIBase::SetRight(int nRight, bool bClip)
{
	m_offset.right = Int2Offset(nRight, bClip);
}

void CUIBase::SetTop(int nTop, bool bClip)
{
	m_offset.top = Int2Offset(nTop, bClip);
}

void CUIBase::SetBottom(int nBottom, bool bClip)
{
	m_offset.bottom = Int2Offset(nBottom, bClip);
}

// �Ӹ����� lpRect ���г��Լ���Ҫ�Ĳ���
void CUIBase::CalcRect(LPRECT lpRect, LPRECT lpClipRect)
{
	CRect rect, rcReal;

	if (IsVisible() && !IsRectEmpty(lpRect))
	{
		rcReal = rect = *(CRect *)lpRect;

		// ����
		if (m_offset.left != MAXINT16)
		{
			rect.left += (short)m_offset.left;

			if (m_size.cx >= 0)
				rect.right = rect.left + m_size.cx;

			if (m_offset.left >> 16)
				lpRect->left = rect.right;
		}
		else if (m_offset.right != MAXINT16)
		{
			rect.right -= (short)m_offset.right;

			if (m_size.cx >= 0)
				rect.left = rect.right - m_size.cx;

			if (m_offset.right >> 16)
				lpRect->right = rect.left;
		}
		else if (m_size.cx > 0)
		{
			rect.left = (rect.left + rect.right - m_size.cx) / 2;
			rect.right = rect.left + m_size.cx;
		}

		// ����
		if (m_offset.top != MAXINT16)
		{
			rect.top += (short)m_offset.top;

			if (m_size.cy >= 0)
				rect.bottom = rect.top + m_size.cy;

			if (m_offset.top >> 16)
				lpRect->top = rect.bottom;
		}
		else if (m_offset.bottom != MAXINT16)
		{
			rect.bottom -= (short)m_offset.bottom;

			if (m_size.cy >= 0)
				rect.top = rect.bottom - m_size.cy;

			if (m_offset.bottom >> 16)
				lpRect->bottom = rect.top;
		}
		else if (m_size.cy > 0)
		{
			rect.top = (rect.top + rect.bottom - m_size.cy) / 2;
			rect.bottom = rect.top + m_size.cy;
		}

		// �����ɼ�����
		rcReal &= rect;

		if (m_pParent)
			rcReal &= m_pParent->m_rcReal;

		if (rcReal.IsRectEmpty())
			rcReal = rect = CRect();
	}

	CRect rcOld(m_rect);

	if (rcOld.IsRectEmpty())
		rcOld = CRect();

	if (rcOld != rect || m_rcReal != rcReal)
	{
		if (lpClipRect && IsControl())
		{
			UnionRect(lpClipRect, lpClipRect, m_rcReal);
			UnionRect(lpClipRect, lpClipRect, rcReal);
		}

		m_rect = rect;
		m_rcReal = rcReal;
		OnRectChanged(rcOld, lpClipRect);
	}
}

void CUIBase::DoPaint(CUIDC &dc) const
{
	CRect rcReal(m_rcReal);

	if (dc.GetRealRect(rcReal))
	{
		if (m_rect != m_rcReal || dynamic_cast<const CUIEdit *>(this))
		{
			// ���ó���������
			SelectClipRgn(dc, CUIRgn(CreateRectRgnIndirect(rcReal)));
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
	CUIBase *pItem = const_cast<CUIBase *>(this);
	for (CUIBase *pParent; pParent = pItem->m_pParent; pItem = pParent);

	CUIRootView *pRootView = dynamic_cast<CUIRootView *>(pItem);
	ATLASSERT(pRootView);
	return pRootView;
}

void CUIBase::GetSize(LPSIZE lpSize) const
{
	*lpSize = m_size;
}

void CUIBase::GetClientRect(LPRECT lpRect) const
{
	*lpRect = m_rect;
}

void CUIBase::GetWindowRect(LPRECT lpRect) const
{
	*lpRect = m_rect;

	HWND hWnd = GetRootView()->GetHwnd();
	ClientToScreen(hWnd, (LPPOINT)lpRect);
	ClientToScreen(hWnd, (LPPOINT)lpRect + 1);
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

void CUIBase::SetEnabled(bool bEnabled)
{
	if (bEnabled)
		bEnabled = true;

	if (m_bEnabled == bEnabled)
		return;

	OnEnabled(m_bEnabled = bEnabled);
	GetRootView()->RaiseMouseMove();
}

void CUIBase::SetVisible(bool bVisible, bool bUpdate)
{
	if (bVisible)
		bVisible = true;

	if (m_bVisible == bVisible)
		return;

	m_bVisible = bVisible;

	if (!bUpdate)
		return;

	GetParent()->UpdateChilds();
	GetRootView()->RaiseMouseMove();
}

bool CUIBase::IsRealEnabled() const
{
	for (const CUIBase *pItem = this; pItem; pItem = pItem->m_pParent)
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

bool CUIBase::IsChild(const CUIBase *pItem) const
{
	for (; pItem; pItem = pItem->m_pParent)
	{
		if (pItem == this)
			return true;
	}

	return false;
}

void CUIBase::DoMouseEnter()
{
	GetRootView()->DoMouseEnter(this);
}

bool CUIBase::DoMouseLeave(bool bForce)
{
	if (m_ppEnter == NULL)
		return true;

	if (m_bKeepEnter && !bForce)
		return false;

	*m_ppEnter = NULL;
	m_ppEnter = NULL;
	OnMouseLeave();
	return true;
}

static int Str2Offset(LPCWSTR lpStr)
{
	LPCWSTR lpEnd = lpStr;
	int nValue = wcstoul(lpStr, (LPWSTR *)&lpEnd, 10);

	ATLASSERT(lpEnd != lpStr);
	ATLASSERT(*lpEnd == 0 || *(int *)lpEnd == '*');

	return Int2Offset(nValue, *lpEnd == '*');
}

void CUIBase::OnLoaded(const IUILoadAttrs &attrs)
{
	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"left"))
		m_offset.left = Str2Offset(lpStr);

	if (lpStr = attrs.GetStr(L"right"))
		m_offset.right = Str2Offset(lpStr);

	if (lpStr = attrs.GetStr(L"top"))
		m_offset.top = Str2Offset(lpStr);

	if (lpStr = attrs.GetStr(L"bottom"))
		m_offset.bottom = Str2Offset(lpStr);

	ATLASSERT(m_offset.left == MAXINT16 || m_offset.right == MAXINT16);
	ATLASSERT(m_offset.top == MAXINT16 || m_offset.bottom == MAXINT16);

	int nValue;
	if (attrs.GetInt(L"width", &nValue))
		SetWidth(nValue);

	if (attrs.GetInt(L"height", &nValue))
		SetHeight(nValue);

	if (attrs.GetInt(L"enabled", &nValue))
		SetEnabled(nValue != 0);

	if (attrs.GetInt(L"visible", &nValue))
		SetVisible(nValue != 0);
}