#pragma once

#include "UIRootView.h"

// �� CUIMoreWnd ���Ƶ� CUIRootView

class CUIMoreView : public CUIRootView
{
public:
	CUIMoreView(IUIWindow *pOwner);
	~CUIMoreView();

	void InitItems(const vector<CUIView *> &vecItems);
	void ClearItems();
	void CalcSize(LPSIZE lpSize) const;

protected:
	virtual void RecalcLayout(LPRECT lpClipRect) override;

	CUIView *m_pToolBar;
};
