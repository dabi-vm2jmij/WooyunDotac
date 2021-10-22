#include "stdafx.h"
#include "UISlider.h"

CUISlider::CUISlider(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpBgFileName) : CUIControl(pParent), m_pProgress(NULL), m_nMaxPos(100), m_fCurPos(0)
{
	if (lpBgFileName)
	{
		CImagex imagex = GetImage(lpBgFileName);

		if (imagex.GetFrameCount() < 2)
		{
			CUIImage *pImage = AddImage(lpBgFileName);
			pImage->SetWidth(0);
			pImage->SetStretch(true);
		}
		else
		{
			m_pProgress = AddProgress(lpBgFileName);
			m_pProgress->SetWidth(0);
		}
	}

	m_pButton = AddButton(lpFileName);
	m_pButton->SetLeft(0);
	m_pButton->SetDraggable(true);

	// 设置最大高度
	int nMaxHeight = 0;

	for (auto pItem : m_vecChilds)
	{
		CSize size = pItem->GetSize();

		if (nMaxHeight < size.cy)
			nMaxHeight = size.cy;
	}

	SetHeight(nMaxHeight);
}

CUISlider::~CUISlider()
{
}

void CUISlider::SetMaxPos(int nMaxPos)
{
	if (nMaxPos <= 0 || nMaxPos == m_nMaxPos)
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
		m_fnOnChange(GetCurPos());
}

void CUISlider::SetCurPos(int nCurPos)
{
	if (nCurPos < 0)
		nCurPos = 0;
	else if (nCurPos > m_nMaxPos)
		nCurPos = m_nMaxPos;

	if (nCurPos == GetCurPos())
		return;

	m_fCurPos = nCurPos;
	ResetOffset(0, false);

	if (m_fnOnChange)
		m_fnOnChange(GetCurPos());
}

void CUISlider::OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	if (m_rect.Size() != ((CRect *)lpOldRect)->Size())
	{
		ResetOffset(0, false);
	}

	__super::OnRectChange(lpOldRect, lpClipRect);
}

void CUISlider::OnLButtonDown(CPoint point)
{
	// 移动滑块中心到点击处
	CSize size = m_pButton->GetSize();
	ResetOffset(point.x - m_rect.left - size.cx / 2, true);

	// 鼠标消息转给滑块
	GetRootView()->UpdateLayout();
	FRIEND(m_pButton)->OnMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(point.x, point.y));

	// LButtonUp 消息会被滑块处理，重置以响应下次点击
	m_bLButtonDown = false;
}

void CUISlider::OnChildMoving(CUIControl *, CPoint point)
{
	ResetOffset(point.x - m_rect.left, true);
}

void CUISlider::ResetOffset(int nOffset, bool bSetPos)
{
	CSize size = m_pButton->GetSize();

	int nWidth = m_rect.Width() - size.cx;
	if (nWidth <= 0)
		return;

	if (!bSetPos)
		nOffset = (int)(m_fCurPos * nWidth / m_nMaxPos + 0.5);
	else if (nOffset < 0)
		nOffset = 0;
	else if (nOffset > nWidth)
		nOffset = nWidth;

	m_pButton->SetLeft(nOffset);

	if (m_pProgress)
		m_pProgress->SetValue(nOffset + size.cx / 2, nWidth + size.cx);

	if (!bSetPos)
		return;

	int nOldPos = GetCurPos();
	m_fCurPos = nOffset * m_nMaxPos / (double)nWidth;

	if (m_fnOnChange && GetCurPos() != nOldPos)
		m_fnOnChange(GetCurPos());
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
