#pragma once

#include "UIControl.h"

class UILIB_API CUILine : public CUIControl
{
public:
	CUILine(CUIView *pParent);
	virtual ~CUILine();

	void SetLineStyle(COLORREF color, int nStyle = 0);	// nStyle 默认为实线，否则每画 nStyle 间隔 nStyle

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void MyPaint(CUIDC &dc) const override;

	CImagex m_imagex;
	int     m_nStyle;
};
