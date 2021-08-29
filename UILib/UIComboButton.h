#pragma once

#include "UIControl.h"

// 组合按钮，多个按钮共享鼠标进入、离开
// 最常用为：左边是普通按钮，右边是下拉按钮或菜单按钮

class UILIB_API CUIComboButton : public CUIView
{
	friend class CUIButton;
public:
	CUIComboButton(CUIView *pParent);
	virtual ~CUIComboButton();

	void EndAddChild();

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void OnChildEnter(CUIButton *pChild);
	virtual void OnChildLeave(CUIButton *pChild);

	bool m_bInNotify;
};
