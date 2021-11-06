#include "stdafx.h"
#include "UIScrollBar.h"

CUIScrollBar::CUIScrollBar(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpBgFileName) : CUIControl(pParent), m_nWheelSize(-10), m_nMinPos(100), m_nMaxPos(100), m_fCurPos(0)
{
	// ���ݹ�����ͼƬȷ��������δʵ�ֺ���
	CImagex imagex = ::GetImage(lpFileName);
	m_bVertical = imagex.Rect().Width() <= imagex.Rect().Height();

	m_pButton = AddButton(lpFileName);
	m_pButton->SetTop(0);
	m_pButton->SetDraggable(true);

	if (lpBgFileName)
		SetBgImage(::GetImage(lpBgFileName));

	// ���������
	int nWidth1 = m_pButton->GetSize().cx, nWidth2 = m_bgImagex.Rect().Width();
	SetWidth(max(nWidth1, nWidth2));
}

void CUIScrollBar::SetRange(int nMinPos, int nMaxPos)
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

	bool bChange = false;
	if (m_fCurPos > nMaxPos - nMinPos)
	{
		m_fCurPos = nMaxPos - nMinPos;
		bChange  = true;
	}

	m_nMinPos = nMinPos;
	m_nMaxPos = nMaxPos;
	m_pButton->SetHeight((int)(m_rect.Height() * m_nMinPos / (double)m_nMaxPos + 0.5));
	ResetOffset(0, false);

	if (m_fnOnChange && bChange)
		m_fnOnChange();
}

void CUIScrollBar::SetCurPos(int nCurPos)
{
	if (nCurPos < 0)
		nCurPos = 0;
	else if (nCurPos > m_nMaxPos - m_nMinPos)
		nCurPos = m_nMaxPos - m_nMinPos;

	if (nCurPos == GetCurPos())
		return;

	m_fCurPos = nCurPos;
	ResetOffset(0, false);

	if (m_fnOnChange)
		m_fnOnChange();
}

bool CUIScrollBar::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOUSEWHEEL)
	{
		int nWheelSize = m_nWheelSize;
		if (nWheelSize < 0)	// -nWheelSize Ϊ�ٷֱ�
			nWheelSize = m_nMinPos * -nWheelSize / 100;

		int zDelta = (short)HIWORD(wParam);
		SetCurPos(GetCurPos() - nWheelSize * zDelta / WHEEL_DELTA);
		return true;
	}

	return __super::OnMessage(uMsg, wParam, lParam);
}

void CUIScrollBar::OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	if (m_rect.Size() != ((CRect *)lpOldRect)->Size())
	{
		m_pButton->SetHeight((int)(m_rect.Height() * m_nMinPos / (double)m_nMaxPos + 0.5));
		ResetOffset(0, false);
	}

	__super::OnRectChange(lpOldRect, lpClipRect);
}

void CUIScrollBar::OnChildMoving(CUIView *, CPoint point)
{
	ResetOffset(point.y - m_rect.top, true);
}

void CUIScrollBar::OnLButtonDown(CPoint point)
{
	CRect rect = m_pButton->GetRect();

	// ����һ�������С
	if (point.y < rect.top)
		SetCurPos(GetCurPos() - m_nMinPos);
	else if (point.y > rect.bottom)
		SetCurPos(GetCurPos() + m_nMinPos);

	// LButtonUp ��Ϣ�ᱻ���鴦����������Ӧ�´ε��
	m_bLButtonDown = false;
}

void CUIScrollBar::ResetOffset(int nOffset, bool bSetPos)
{
	int nHeight = m_rect.Height() - m_pButton->GetSize().cy;
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

	if (m_fnOnChange && GetCurPos() != nOldPos)
		m_fnOnChange();
}

void CUIScrollBar::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	if (int nSize = attrs.GetInt(L"wheelSize"))
		SetWheelSize(nSize);

	int nMinPos, nMaxPos;
	if (attrs.GetInt(L"minPos", &nMinPos) && attrs.GetInt(L"maxPos", &nMaxPos))
		SetRange(nMinPos, nMaxPos);
}
