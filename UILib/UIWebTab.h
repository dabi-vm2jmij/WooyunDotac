#pragma once
#include "UIButton.h"

// ä¯ÀÀÆ÷±êÇ©

class UILIB_API CUIWebTab : public CUIButton
{
	friend class CUIWebTabBar;
public:
	CUIWebTab(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIWebTab();

	CUIWebTabBar *GetWebTabBar() const;

protected:
	virtual bool DoMouseLeave(bool bForce) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnLButtonUp(CPoint point) final {}
	virtual void OnActivate(bool bActive) {}
	void SetActive(bool bActive);
};
