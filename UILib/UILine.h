#pragma once

#include "UIControl.h"

class UILIB_API CUILine : public CUIControl
{
public:
	CUILine(CUIView *pParent);

	void SetLineStyle(COLORREF color, int nStyle = 0);	// nStyle 默认为实线，否则每画 nStyle 间隔 nStyle

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;

	COLORREF m_color;
	int      m_nStyle;
};
