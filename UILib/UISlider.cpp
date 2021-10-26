#include "stdafx.h"
#include "UISlider.h"

CUISlider::CUISlider(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpBgFileName) : CUIControl(pParent), m_nMaxPos(100), m_fCurPos(0)
{
	m_pButton = AddButton(lpFileName);
	m_pButton->SetLeft(0);
	m_pButton->SetDraggable(true);

	if (lpBgFileName)
		SetBgImage(GetImage(lpBgFileName));

	// �������߶�
	int nHeight1 = m_pButton->GetSize().cy, nHeight2 = m_bgImagex.Rect().Height();
	SetHeight(max(nHeight1, nHeight2));
}

void CUISlider::SetMaxPos(int nMaxPos)
{
	if (nMaxPos <= 0 || m_nMaxPos == nMaxPos)
		return;

	bool bChange = false;
	if (m_fCurPos > nMaxPos)
	{
		m_fCurPos = nMaxPos;
		bChange  = true;
	}

	m_nMaxPos = nMaxPos;
	ResetOffset(0, false);

	if (m_fnOnChange && bChange)
		m_fnOnChange();
}

void CUISlider::SetCurPos(int nCurPos)
{
	if (nCurPos < 0)
		nCurPos = 0;
	else if (nCurPos > m_nMaxPos)
		nCurPos = m_nMaxPos;

	if (GetCurPos() == nCurPos)
		return;

	m_fCurPos = nCurPos;
	ResetOffset(0, false);

	if (m_fnOnChange)
		m_fnOnChange();
}

void CUISlider::OnPaint(CUIDC &dc) const
{
	if (m_bgColor != -1)
		dc.FillSolidRect(m_rect, m_bgColor);

	if (m_bgImagex)
	{
		// ���������¾���
		CRect rect(m_rect);
		rect.top = (rect.top + rect.bottom - m_bgImagex.Rect().Height()) / 2;
		rect.bottom = rect.top + m_bgImagex.Rect().Height();
		m_bgImagex.Scale9Draw(dc, rect);

		if (m_bgImagex.GetFrameCount() > 1)
		{
			// �����ȵ���ť����
			CImagex imagex(m_bgImagex);
			imagex.SetFrameIndex(1);
			rect.right = rect.left + m_pButton->GetOffset().left + m_pButton->GetSize().cx / 2;
			imagex.Scale9Draw(dc, rect);
		}
	}
}

void CUISlider::OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	if (m_rect.Size() != ((CRect *)lpOldRect)->Size())
		ResetOffset(0, false);

	__super::OnRectChange(lpOldRect, lpClipRect);
}

void CUISlider::OnChildMoving(CUIView *, CPoint point)
{
	ResetOffset(point.x - m_rect.left, true);
}

void CUISlider::OnLButtonDown(CPoint point)
{
	// �ƶ��������ĵ������
	ResetOffset(point.x - m_rect.left - m_pButton->GetSize().cx / 2, true);

	// �����Ϣת������
	GetRootView()->UpdateLayout();
	FRIEND(m_pButton)->OnMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(point.x, point.y));

	// LButtonUp ��Ϣ�ᱻ���鴦����������Ӧ�´ε��
	m_bLButtonDown = false;
}

void CUISlider::ResetOffset(int nOffset, bool bSetPos)
{
	int nWidth = m_rect.Width() - m_pButton->GetSize().cx;
	if (nWidth <= 0)
		return;

	if (!bSetPos)
		nOffset = (int)(m_fCurPos * nWidth / m_nMaxPos + 0.5);
	else if (nOffset < 0)
		nOffset = 0;
	else if (nOffset > nWidth)
		nOffset = nWidth;

	m_pButton->SetLeft(nOffset);

	if (!bSetPos)
		return;

	int nOldPos = GetCurPos();
	m_fCurPos = nOffset * m_nMaxPos / (double)nWidth;

	if (m_fnOnChange && GetCurPos() != nOldPos)
		m_fnOnChange();
}

void CUISlider::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"maxPos", &nValue))
		SetMaxPos(nValue);

	if (attrs.GetInt(L"curPos", &nValue))
		SetCurPos(nValue);
}
