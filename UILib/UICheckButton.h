#pragma once

#include "UIButton.h"

// 复选按钮，点击可以选中或者取消选中

class UILIB_API CUICheckButton : public CUIButton
{
public:
	CUICheckButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUICheckButton();

	void SetCheck(bool bCheck);
	bool IsChecked() const { return m_bCheck; }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnButtonState(ButtonState btnState) override;
	virtual void OnChecked() {}

	bool m_bCheck;
};
