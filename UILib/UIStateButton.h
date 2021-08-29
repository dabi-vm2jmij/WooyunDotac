#pragma once

#include "UIView.h"

// 状态按钮，多个按钮组合，当前只显示1个，点击后切换到下个，比如系统的最大化、还原按钮

class UILIB_API CUIStateButton : public CUIView
{
public:
	CUIStateButton(CUIView *pParent);
	virtual ~CUIStateButton();

	void OnClick(std::function<void(int)> &&fnOnClick) { m_fnOnClick = std::move(fnOnClick); }
	void SetState(int nState);
	void EndAddChild();

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	virtual void OnChildClick();

	int  m_nState;
	std::function<void(int)> m_fnOnClick;
};
