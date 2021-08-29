#pragma once

#include "UIControl.h"

class UILIB_API CUIImage : public CUIControl
{
public:
	CUIImage(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIImage();

	void SetImage(const CImagex &imagex);
};
