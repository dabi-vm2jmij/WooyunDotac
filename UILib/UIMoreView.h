#pragma once

#include "UIRootView.h"

// 给 CUIMoreWnd 定制的 CUIRootView

class CUIMoreView : public CUIRootView
{
public:
	CUIMoreView(IUIWindow *pOwner);

	void InitItems(const vector<CUIBase *> &pItems);
	void ClearItems();
	void CalcSize(LPSIZE lpSize) const;

protected:
	virtual void RecalcLayout(LPRECT lpClipRect) override;
};
