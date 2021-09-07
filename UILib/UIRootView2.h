#pragma once

#include "UIRootView.h"

// 给 CUIMoreWnd 定制的 CUIRootView

class CUIRootView2 : public CUIRootView
{
public:
	CUIRootView2(IUIWindow *pWindow);

	void InitItems(const std::vector<CUIBase *> &pItems);
	void ClearItems();
	void GetSize(LPSIZE lpSize) const;

protected:
	virtual void RecalcLayout(LPRECT lpClipRect) override;
};
