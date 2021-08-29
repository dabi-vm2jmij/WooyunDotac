#pragma once

#include "UIView.h"

class UILIB_API CUIScrollView : public CUIView
{
public:
	CUIScrollView(CUIView *pParent);

	void SetVScroll(CUIVScroll *pVScroll);
	void CheckVScroll();

private:
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	virtual int  CalcHeight() const;

	CUIVScroll *m_pVScroll;
};
