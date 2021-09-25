#pragma once

#include "UIView.h"

// 工具栏，控件都按左对齐添加，显示不了的折叠到“更多工具”

class UILIB_API CUIToolBar : public CUIView
{
public:
	CUIToolBar(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIToolBar();

	void SetMoreBg(COLORREF color) { m_moreBg = color; }
	void CloseMoreWnd();

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	virtual void OnMoreBtn();
	int  GetMoreIndex() const;

	COLORREF m_moreBg;
	HWND     m_hMoreWnd;
	std::vector<CUIBase *> m_vecMoreItems;
};
