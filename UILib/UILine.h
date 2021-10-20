#pragma once

#include "UIControl.h"

class UILIB_API CUILine : public CUIControl
{
public:
	CUILine(CUIView *pParent);
	virtual ~CUILine();

	void SetLineStyle(COLORREF color, int nStyle = 0);	// nStyle 默认为实线，否则每画 nStyle 间隔 nStyle

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual void DoPaint(CUIDC &dc) const override;

	COLORREF m_color;
	int      m_nStyle;
};
