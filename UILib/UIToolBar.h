#pragma once

#include "UIView.h"

// ���������ؼ������������ӣ���ʾ���˵��۵��������๤�ߡ�

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
