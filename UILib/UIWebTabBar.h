#pragma once
#include "UIView.h"

// ä¯ÀÀÆ÷±êÇ©À¸

class UILIB_API CUIWebTabBar : public CUIView
{
public:
	CUIWebTabBar(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIWebTabBar();

	void SetTabWidth(int nWidth) { m_nTabWidth = nWidth; }
	void ActivateTab(CUIWebTab *pWebTab);
	UINT GetTabIndex(CUIWebTab *pWebTab) const;
	CUIWebTab *AddWebTab(CUIWebTab *pWebTab = NULL);

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	virtual void OnChildMoving(CUIControl *, CPoint point) override;
	virtual void OnChildMoved(CUIControl *, CPoint point) override;

	CImagex m_imgxTabs[4];
	int     m_nTabWidth;
};
