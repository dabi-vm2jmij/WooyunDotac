#pragma once
#include "UIRootView.h"

// �� CUIMoreWnd ���Ƶ� CUIRootView

class CUIRootView2 : public CUIRootView
{
public:
	void InitItems(const std::vector<CUIBase *> &pItems);
	void ClearItems();
	void GetSize(LPSIZE lpSize) const;

protected:
	virtual void RecalcLayout(LPRECT lpClipRect) override;
};