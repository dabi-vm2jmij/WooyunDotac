#pragma once

#include "UIRootView.h"

// �� CUIMoreWnd ���Ƶ� CUIRootView

class CUIMoreView : public CUIRootView
{
public:
	CUIMoreView(IUIWindow *pWindow);

	void InitItems(const std::vector<CUIBase *> &pItems);
	void ClearItems();
	void CalcSize(LPSIZE lpSize) const;

protected:
	virtual void RecalcLayout(LPRECT lpClipRect) override;
};
