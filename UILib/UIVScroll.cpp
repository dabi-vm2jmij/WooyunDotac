#include "stdafx.h"
#include "UIVScroll.h"

CUIVScroll::CUIVScroll(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpFileNameBg) : CUIControl(pParent), m_nMinPos(100), m_nMaxPos(100), m_fCurPos(0)
{
	if (lpFileNameBg)
	{
		CUIImage *pImage = AddImage(lpFileNameBg);
		pImage->SetHeight(-1);
		pImage->SetStretch(true);
	}

	m_pButton = AddButton(lpFileName);
	m_pButton->SetTop(0);
	m_pButton->SetStretch(true);
	m_pButton->SetDraggable(true);

	// 设置最大宽度
	int nMaxWidth = 0;

	for (auto pItem : m_vecChilds)
	{
		CSize size;
		pItem->GetSize(&size);

		if (nMaxWidth < size.cx)
			nMaxWidth = size.cx;
	}

	SetWidth(nMaxWidth);
}

CUIVScroll::~CUIVScroll()
{
}

void CUIVScroll::SetRange(int nMinPos, int nMaxPos)
{
	if (nMinPos <= 0)
	{
		ATLASSERT(0);
		return;
	}

	if (nMaxPos < nMinPos)
		nMaxPos = nMinPos;

	if (m_nMinPos == nMinPos && m_nMaxPos == nMaxPos)
		return;

	bool bChanged = false;
	if (m_fCurPos > nMaxPos - nMinPos)
	{
		m_fCurPos = nMaxPos - nMinPos;
		bChanged  = true;
	}

	m_nMinPos = nMinPos;
	m_nMaxPos = nMaxPos;
	m_pButton->SetHeight((int)(m_rect.Height() * m_nMinPos / (double)m_nMaxPos + 0.5));
	ResetOffset(0, false);

	if (m_fnOnChanged && bChanged)
		m_fnOnChanged(GetCurPos());
}

void CUIVScroll::SetCurPos(int nCurPos)
{
	if (nCurPos < 0)
		nCurPos = 0;
	else if (nCurPos > m_nMaxPos - m_nMinPos)
		nCurPos = m_nMaxPos - m_nMinPos;

	if (nCurPos == GetCurPos())
		return;

	m_fCurPos = nCurPos;
	ResetOffset(0, false);

	if (m_fnOnChanged)
		m_fnOnChanged(GetCurPos());
}

bool CUIVScroll::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOUSEWHEEL)
	{
		short zDelta = (short)HIWORD(wParam);
		SetCurPos(GetCurPos() - zDelta / WHEEL_DELTA * m_nMinPos);
		return true;
	}

	return __super::OnMessage(uMsg, wParam, lParam);
}

void CUIVScroll::OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	if (m_rect.Size() != ((CRect *)lpOldRect)->Size())
	{
		m_pButton->SetHeight((int)(m_rect.Height() * m_nMinPos / (double)m_nMaxPos + 0.5));
		ResetOffset(0, false);
	}

	__super::OnRectChanged(lpOldRect, lpClipRect);
}

void CUIVScroll::OnLButtonDown(CPoint point)
{
	CRect rect;
	m_pButton->GetClientRect(rect);

	// 滚动一个滑块大小
	if (point.y < rect.top)
		SetCurPos(GetCurPos() - m_nMinPos);
	else if (point.y > rect.bottom)
		SetCurPos(GetCurPos() + m_nMinPos);

	// LButtonUp 消息会被滑块处理，重置以响应下次点击
	m_bLButtonDown = false;
}

void CUIVScroll::OnChildMoving(CUIControl *, CPoint point)
{
	ResetOffset(point.y - m_rect.top, true);
}

void CUIVScroll::ResetOffset(int nOffset, bool bSetPos)
{
	CSize size;
	m_pButton->GetSize(&size);

	int nHeight = m_rect.Height() - size.cy;
	if (nHeight <= 0)
		return;

	if (!bSetPos)
		nOffset = (int)(m_fCurPos * nHeight / (m_nMaxPos - m_nMinPos) + 0.5);
	else if (nOffset < 0)
		nOffset = 0;
	else if (nOffset > nHeight)
		nOffset = nHeight;

	m_pButton->SetTop(nOffset);

	if (!bSetPos)
		return;

	int nOldPos = GetCurPos();
	m_fCurPos = nOffset * (m_nMaxPos - m_nMinPos) / (double)nHeight;

	if (m_fnOnChanged && GetCurPos() != nOldPos)
		m_fnOnChanged(GetCurPos());
}

void CUIVScroll::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	SetRange(attrs.GetInt(L"minPos", m_nMinPos), attrs.GetInt(L"maxPos", m_nMaxPos));
}
