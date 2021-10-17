#include "stdafx.h"
#include "UIButton.h"

CUIButton::CUIButton(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_btnState(Normal)
{
	if (lpFileName == NULL)
		return;

	ResetImages(m_imagexs, SplitImage(lpFileName, m_imagexs));
}

CUIButton::~CUIButton()
{
}

void CUIButton::ResetImages(const CImagex imagexs[], int nCount)
{
	for (int i = 0; i < nCount && i < 4; i++)
		m_imagexs[i] = imagexs[i];

	Fill4States(m_imagexs, nCount);
	SetSize(m_imagexs[0].Rect().Size());
	SetCurImage(m_imagexs[m_btnState]);
}

// 不足4态的图片扩展成4态：3态为（0、1、2、0），2态为（0、1、1、0）
void CUIButton::Fill4States(CImagex imagexs[], int nCount)
{
	for (int i = max(1, nCount); i < 4; i++)
		imagexs[i] = imagexs[0];

	if (nCount == 2)
		imagexs[2] = imagexs[1];
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
}

void CUIButton::OnMouseLeave()
{
	if (m_btnState != Disable)
		OnButtonState(Normal);

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
	SetCurImage(m_imagexs[m_btnState = btnState]);
}
