#pragma once

#include "UICheckBox.h"

// ��ѡ��

class UILIB_API CUIRadioBox : public CUICheckBox
{
public:
	CUIRadioBox(CUIView *pParent, LPCWSTR lpFileName);

protected:
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnChecked() override;
};
