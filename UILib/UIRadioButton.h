#pragma once

#include "UIButton.h"

// 单选按钮，比如像浏览器标签，只能有1个选中

class UILIB_API CUIRadioButton : public CUIButton
{
public:
	CUIRadioButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIRadioButton();

	void SetCheck(bool bCheck);
	bool IsChecked() const { return m_bKeepEnter; }

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnCheck();
};
