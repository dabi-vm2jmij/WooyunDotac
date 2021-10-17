#pragma once

#include "UICheckButton.h"

// 单选按钮，比如像浏览器标签，只能有1个选中

class UILIB_API CUIRadioButton : public CUICheckButton
{
public:
	CUIRadioButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIRadioButton();

protected:
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnChecked() override;
};
