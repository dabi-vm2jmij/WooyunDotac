#pragma once

#include "UICheckButton.h"

// ��ѡ��ť���������������ǩ��ֻ����1��ѡ��

class UILIB_API CUIRadioButton : public CUICheckButton
{
public:
	CUIRadioButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIRadioButton();

protected:
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnChecked() override;
};
