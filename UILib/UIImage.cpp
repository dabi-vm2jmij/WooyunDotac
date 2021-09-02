#include "stdafx.h"
#include "UIImage.h"

CUIImage::CUIImage(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent)
{
	m_bClickable = false;

	if (lpFileName == NULL)
		return;

	m_curImagex = GetImage(lpFileName);
	SetSize(m_curImagex.Rect().Size());
}

CUIImage::~CUIImage()
{
}

void CUIImage::SetImage(const CImagex &imagex)
{
	SetCurImage(imagex);
	SetSize(m_curImagex.Rect().Size());
}
