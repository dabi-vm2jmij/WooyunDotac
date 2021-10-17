#pragma once

#include "UIControl.h"

// 组合按钮，多个按钮共享鼠标进入、离开
// 最常用为：左边是普通按钮，右边是下拉按钮或菜单按钮

class UILIB_API CUIComboButton : public CUIControl
{
public:
	CUIComboButton(CUIView *pParent);
	virtual ~CUIComboButton();

	void EndAddChild();

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual bool OnHitTest(UIHitTest &hitTest) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	CUIControl *GetHitChild(CPoint point);

	bool m_bNoLeave;
};
