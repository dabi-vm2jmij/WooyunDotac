#pragma once

#include "UIRootView.h"

// �� CUIMoreWnd ���Ƶ� CUIRootView

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
