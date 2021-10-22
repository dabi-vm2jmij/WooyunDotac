#pragma once

#include "UIView.h"

// 工具栏，控件都按左对齐添加，显示不了的折叠到“更多工具”

class UILIB_API CUIToolBar : public CUIView
{
public:
	CUIToolBar(CUIView *pParent);
	virtual ~CUIToolBar();

	void GetMoreItems(vector<CUIView *> &vecItems);
	void SetMoreWnd(HWND hMoreWnd) { m_hMoreWnd = hMoreWnd; }

protected:
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	int  GetMoreIndex() const;
	void CloseMoreWnd();

	HWND m_hMoreWnd;
	vector<CUIView *> m_vecMoreItems;
};
