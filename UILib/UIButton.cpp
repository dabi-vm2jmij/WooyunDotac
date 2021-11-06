#include "stdafx.h"
#include "UIButton.h"

CUIButton::CUIButton(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_btnState(Normal)
{
	if (lpFileName)
	{
		SetImage(::GetImage(lpFileName));
		SetSize(m_imagexs[1].Rect().Size());
	}
}

// 不足4态的图片扩展成4态
static void Fill4Images(CImagex imagexs[], int nCount)
{
	switch (nCount)
	{
	case 1:
		imagexs[1] = imagexs[0];
		imagexs[0] = nullptr;

	case 2:
		imagexs[2] = imagexs[1];

	case 3:
		imagexs[3] = imagexs[0];
	}
}

void CUIButton::SetImage(const CImagex &imagex)
{
	int nCount = imagex.GetFrameCount();

	for (int i = 0; i < _countof(m_imagexs) && i < nCount; i++)
	{
		m_imagexs[i] = imagex;
		m_imagexs[i].SetFrameIndex(i);
	}

	Fill4Images(m_imagexs, nCount);
	InvalidateRect();
}

void CUIButton::SetImages(const CImagex imagexs[], int nCount)
{
	for (int i = 0; i < _countof(m_imagexs) && i < nCount; i++)
		m_imagexs[i] = imagexs[i];

	Fill4Images(m_imagexs, nCount);
	InvalidateRect();
}

void CUIButton::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	if (m_imagexs[m_btnState])
		m_imagexs[m_btnState].Scale9Draw(dc, m_rect);
}

void CUIButton::OnEnable(bool bEnable)
{
	__super::OnEnable(bEnable);

	if (!bEnable)
		OnButtonState(Disable);
	else if (m_btnState == Disable)
		OnButtonState(Normal);
}

void CUIButton::OnMouseEnter()
{
	__super::OnMouseEnter();

	if (m_btnState == Normal)
		OnButtonState(Hover);

	if (m_fnOnHover)
		m_fnOnHover(true);
}

void CUIButton::OnMouseLeave()
{
	if (m_btnState != Disable)
		OnButtonState(Normal);

	if (m_fnOnHover)
		m_fnOnHover(false);

	__super::OnMouseLeave();
}

void CUIButton::OnLButtonDown(CPoint point)
{
	OnButtonState(Press);
}

void CUIButton::OnLButtonUp(CPoint point)
{
	OnButtonState(Hover);

	if (m_fnOnClick)
		m_fnOnClick();
}

void CUIButton::OnButtonState(ButtonState btnState)
{
	if (m_imagexs[m_btnState] != m_imagexs[btnState])
		InvalidateRect();

	m_btnState = btnState;
}
