#pragma once

#include "UIButton.h"

// 复选按钮，点击可以选中或者取消选中

class UILIB_API CUICheckButton : public CUIButton
{
public:
	CUICheckButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUICheckButton();

	void OnClick(std::function<void(bool)> &&fnOnClick) { m_fnOnClick = std::move(fnOnClick); }
	void SetCheck(bool bCheck);
	bool IsChecked() const { return m_bKeepEnter; }

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void OnLButtonUp(CPoint point) override;

	std::function<void(bool)> m_fnOnClick;
};
