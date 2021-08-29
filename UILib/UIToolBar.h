#pragma once

#include "UIView.h"

// 工具栏，控件都按左对齐添加，显示不了的折叠到“更多工具”

class UILIB_API CUIToolBar : public CUIView
{
public:
	CUIToolBar(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIToolBar();

	void SetMoreBg(LPCWSTR lpFileName);
	void CloseMoreWnd();

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	int  GetMoreIndex() const;
	void OnMoreBtn();

	wstring  m_strImageBg;
	HWND     m_hMoreWnd;
	std::vector<CUIBase *> m_vecMoreItems;
};
