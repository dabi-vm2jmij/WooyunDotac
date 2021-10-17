#pragma once

#include "UICheckBox.h"

// µ¥Ñ¡¿ò

class UILIB_API CUIRadioBox : public CUICheckBox
{
public:
	CUIRadioBox(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIRadioBox();

protected:
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnChecked() override;
};
