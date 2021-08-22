#include "stdafx.h"
#include "UIButton.h"

CUIButton::CUIButton(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_nDrawState(0)
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
	SetCurImage(m_imagexs[m_nDrawState]);
}

// 不足4态的图片扩展成4态：3态为（0、1、2、0），2态为（0、1、1、0）
void CUIButton::Fill4States(CImagex imagexs[], int nCount) const
{
	for (int i = max(1, nCount); i < 4; i++)
		imagexs[i] = imagexs[0];

	if (nCount == 2)
		imagexs[2] = imagexs[1];
}

void CUIButton::OnEnabled(bool bEnabled)
{
	__super::OnEnabled(bEnabled);

	if (bEnabled)
	{
		if (m_nDrawState == 3)
			OnDrawState(0);
	}
	else
	{
		if (!m_bKeepEnter || m_nDrawState != 2)
			OnDrawState(3);
	}
}

void CUIButton::OnMouseEnter()
{
	__super::OnMouseEnter();

	if (m_nDrawState == 0)
		OnDrawState(1);

	if (CUIComboButton *pParent = dynamic_cast<CUIComboButton *>(GetParent()))
		pParent->OnChildEnter(this);
}

void CUIButton::OnMouseLeave()
{
	if (m_nDrawState != 3)
		OnDrawState(0);

	if (CUIComboButton *pParent = dynamic_cast<CUIComboButton *>(GetParent()))
		pParent->OnChildLeave(this);

	__super::OnMouseLeave();
}

void CUIButton::OnLButtonDown(CPoint point)
{
	OnDrawState(2);
}

void CUIButton::OnLButtonUp(CPoint point)
{
	OnDrawState(1);

	if (m_fnOnClick)
		m_fnOnClick();
}

void CUIButton::OnDrawState(UINT nState)
{
	SetCurImage(m_imagexs[m_nDrawState = nState]);
}
