#pragma once

#include "UIView.h"

class UILIB_API CUIScrollView : public CUIView
{
public:
	CUIScrollView(CUIView *pParent);

	void SetVScroll(CUIVScroll *pVScroll);
	void CheckVScroll();

protected:
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnNeedLayout(LPRECT lpClipRect) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	virtual int  CalcHeight() const;

	CUIVScroll *m_pVScroll;
};
